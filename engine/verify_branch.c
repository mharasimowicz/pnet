/*
 * verify_branch.c - Verify instructions related to branching.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#if defined(IL_VERIFY_GLOBALS)

/*
 * Type inference matrix for binary comparison operations.
 */
static char const binaryCompareMatrix
			[ILEngineType_ValidTypes][ILEngineType_ValidTypes] =
{
		    /* I4    I8    I     F     &     O     *     MV */
	/* I4: */ {T_I4, T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I8: */ {T_NO, T_I8, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I:  */ {T_I,  T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* F:  */ {T_NO, T_NO, T_NO, T_F,  T_NO, T_NO, T_NO, T_NO},
	/* &:  */ {T_NO, T_NO, T_NO, T_NO, T_M,  T_NO, T_T,  T_NO},
	/* O:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* *:  */ {T_NO, T_NO, T_NO, T_NO, T_T,  T_NO, T_T,  T_NO},
	/* MV: */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
};

/*
 * Type inference matrix for binary equality operations.
 */
static char const binaryEqualityMatrix
			[ILEngineType_ValidTypes][ILEngineType_ValidTypes] =
{
		    /* I4    I8    I     F     &     O     *     MV */
	/* I4: */ {T_I4, T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I8: */ {T_NO, T_I8, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I:  */ {T_I,  T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* F:  */ {T_NO, T_NO, T_NO, T_F,  T_NO, T_NO, T_NO, T_NO},
	/* &:  */ {T_NO, T_NO, T_NO, T_NO, T_M,  T_NO, T_T,  T_NO},
	/* O:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_O,  T_NO, T_NO},
	/* *:  */ {T_NO, T_NO, T_NO, T_NO, T_T,  T_NO, T_T,  T_NO},
	/* MV: */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
};

/*
 * Type inference matrix for binary comparison operations,
 * when unsafe pointer comparisons are allowed.
 */
static char const unsafeCompareMatrix
			[ILEngineType_ValidTypes][ILEngineType_ValidTypes] =
{
		    /* I4    I8    I     F     &     O     *     MV */
	/* I4: */ {T_I4, T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I8: */ {T_NO, T_I8, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I:  */ {T_I,  T_NO, T_I,  T_NO, T_M,  T_NO, T_T,  T_NO},
	/* F:  */ {T_NO, T_NO, T_NO, T_F,  T_NO, T_NO, T_NO, T_NO},
	/* &:  */ {T_NO, T_NO, T_M,  T_NO, T_M,  T_NO, T_T,  T_NO},
	/* O:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_O,  T_NO, T_NO},
	/* *:  */ {T_NO, T_NO, T_T,  T_NO, T_T,  T_NO, T_T,  T_NO},
	/* MV: */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
};

/*
 * Type inference matrix for unary branch operators.
 */
static char const unaryBranchMatrix[ILEngineType_ValidTypes] =
{
    /* I4    I8    I     F     &     O     *    MV */
     T_I4, T_I8, T_I,  T_F,  T_M,  T_O,  T_T, T_NO,
};

/*
 * Information that is stored for a label.
 */
typedef struct _tagBranchLabel BranchLabel;
struct _tagBranchLabel
{
	ILUInt32	 address;
	BranchLabel *next;
	ILUInt32	 stackSize;

};

/*
 * Get the destination for a short or long branch instruction.
 */
#define	GET_SHORT_DEST()	(offset + 2 + ((ILUInt32)(ILInt32)(pc[1])))
#define	GET_LONG_DEST()		(offset + 5 + ((ILUInt32)IL_READ_INT32(pc + 1)))

/*
 * Find an existing branch label information block.
 */
static BranchLabel *FindLabel(BranchLabel *labelList, ILUInt32 address)
{
	while(labelList != 0)
	{
		if(labelList->address == address)
		{
			return labelList;
		}
		labelList = labelList->next;
	}
	return 0;
}

/*
 * Validate the current contents of the stack against
 * information that was previously recorded.
 */
static int ValidateStack(BranchLabel *label, ILEngineStackItem *stack,
						 ILUInt32 stackSize)
{
	ILUInt32 posn;
	ILEngineStackItem *labelStack;
	if(stackSize != label->stackSize)
	{
		return 0;
	}
	labelStack = (ILEngineStackItem *)(label + 1);
	for(posn = 0; posn < stackSize; ++posn)
	{
		if(stack[posn].engineType != labelStack[posn].engineType)
		{
			return 0;
		}
		if(stack[posn].engineType == ILEngineType_M ||
		   stack[posn].engineType == ILEngineType_T ||
		   stack[posn].engineType == ILEngineType_MV)
		{
			if(!ILTypeIdentical(stack[posn].typeInfo,
								labelStack[posn].typeInfo))
			{
				return 0;
			}
		}
		else if(stack[posn].engineType == ILEngineType_O)
		{
			/* The current stack must be a sub-class of the label stack */
			if(stack[posn].typeInfo == 0)
			{
				/* The current stack is "null", so use the label type */
				stack[posn].typeInfo = labelStack[posn].typeInfo;
			}
			else if(labelStack[posn].typeInfo != 0 &&
			        ILType_IsClass(labelStack[posn].typeInfo) &&
					IsSubClass(stack[posn].typeInfo,
							   ILType_ToClass(labelStack[posn].typeInfo)))
			{
				/* TODO: may need a better sub-class test above */
				stack[posn].typeInfo = labelStack[posn].typeInfo;
			}
			else
			{
				return 0;
			}
		}
	}
	return 1;
}

/*
 * Copy the current contents of the stack to a new label block.
 * Returns NULL if out of memory.
 */
static BranchLabel *CopyStack(TempAllocator *allocator, ILUInt32 address,
							  ILEngineStackItem *stack, ILUInt32 stackSize)
{
	BranchLabel *label;

	/* Allocate space for the label */
	label = (BranchLabel *)TempAllocate
				(allocator, sizeof(BranchLabel) +
							sizeof(ILEngineStackItem) * stackSize);
	if(!label)
	{
		return 0;
	}

	/* Copy the address and stack information to the label */
	label->address = address;
	label->stackSize = stackSize;
	if(stackSize > 0)
	{
		ILMemCpy(label + 1, stack, stackSize * sizeof(ILEngineStackItem));
	}

	/* The label has been initialized */
	return label;
}

/*
 * Copy exception information to a new label block.
 * Returns NULL if out of memory.
 */
static BranchLabel *CopyExceptionStack(TempAllocator *allocator,
									   ILUInt32 address, ILClass *classInfo,
									   ILUInt32 stackSize)
{
	BranchLabel *label;

	/* Allocate space for the label */
	label = (BranchLabel *)TempAllocate
				(allocator, sizeof(BranchLabel) +
							sizeof(ILEngineStackItem) * stackSize);
	if(!label)
	{
		return 0;
	}

	/* Copy the address and stack information to the label */
	label->address = address;
	label->stackSize = stackSize;
	if(stackSize > 0)
	{
		((ILEngineStackItem *)(label + 1))->engineType = ILEngineType_O;
		((ILEngineStackItem *)(label + 1))->typeInfo =
				ILType_FromClass(classInfo);
	}

	/* The label has been initialized */
	return label;
}

/*
 * Reload the contents of the stack from a pre-existing label.
 * Returns the new stack size.
 */
static ILUInt32 ReloadStack(BranchLabel *label, ILEngineStackItem *stack)
{
	if(label->stackSize > 0)
	{
		ILMemCpy(stack, label + 1,
				 label->stackSize * sizeof(ILEngineStackItem));
	}
	return label->stackSize;
}

/*
 * Validate or record the stack information for a destination label.
 */
#define	VALIDATE_BRANCH_STACK(dest)	\
			do { \
				currLabel = FindLabel(labelList, (dest)); \
				if(currLabel) \
				{ \
					if(!ValidateStack(currLabel, stack, stackSize)) \
					{ \
						VERIFY_STACK_ERROR(); \
					} \
				} \
				else \
				{ \
					currLabel = CopyStack(&allocator, (dest), \
										  stack, stackSize); \
					if(!currLabel) \
					{ \
						VERIFY_MEMORY_ERROR(); \
					} \
					currLabel->next = labelList; \
					labelList = currLabel; \
				} \
			} while (0)

/*
 * Validate or record the stack information for a jump target.
 */
#define	VALIDATE_TARGET_STACK(address) \
			do { \
				currLabel = FindLabel(labelList, (address)); \
				if(currLabel) \
				{ \
					if(lastWasJump) \
					{ \
						/* Reload the stack contents from the label */ \
						stackSize = ReloadStack(currLabel, stack); \
					} \
					else if(!ValidateStack(currLabel, stack, stackSize)) \
					{ \
						VERIFY_STACK_ERROR(); \
					} \
				} \
				else \
				{ \
					if(lastWasJump) \
					{ \
						/* This is probably the head of a while loop */ \
						/* which is assumed to always be zero-sized. */ \
						stackSize = 0; \
					} \
					currLabel = CopyStack(&allocator, (address), \
										  stack, stackSize); \
					if(!currLabel) \
					{ \
						VERIFY_MEMORY_ERROR(); \
					} \
					currLabel->next = labelList; \
					labelList = currLabel; \
				} \
			} while (0)

/*
 * Set the contents of the stack at a particular point
 * in the method to a given exception object type.
 */
#define	SET_TARGET_STACK(address,classInfo)	\
			do { \
				currLabel = FindLabel(labelList, (address)); \
				if(currLabel) \
				{ \
					/* Check the stack contents for equality */ \
					if(currLabel->stackSize != 1 || \
					   ((ILEngineStackItem *)(currLabel + 1))->engineType \
					   		!= ILEngineType_O || \
					   ((ILEngineStackItem *)(currLabel + 1))->typeInfo \
					   		!= ILType_FromClass((classInfo))) \
					{ \
						VERIFY_STACK_ERROR(); \
					} \
				} \
				else \
				{ \
					currLabel = CopyExceptionStack(&allocator, (address), \
										  		   (classInfo), 1); \
					if(!currLabel) \
					{ \
						VERIFY_MEMORY_ERROR(); \
					} \
					currLabel->next = labelList; \
					labelList = currLabel; \
				} \
			} while (0)

/*
 * Set the contents of the stack at a particular point
 * in the method to empty.
 */
#define	SET_TARGET_STACK_EMPTY(address)	\
			do { \
				currLabel = FindLabel(labelList, (address)); \
				if(currLabel) \
				{ \
					/* Check the stack contents for equality */ \
					if(currLabel->stackSize != 0) \
					{ \
						VERIFY_STACK_ERROR(); \
					} \
				} \
				else \
				{ \
					currLabel = CopyExceptionStack(&allocator, (address), \
										  		   0, 0); \
					if(!currLabel) \
					{ \
						VERIFY_MEMORY_ERROR(); \
					} \
					currLabel->next = labelList; \
					labelList = currLabel; \
				} \
			} while (0)

#elif defined(IL_VERIFY_LOCALS)

ILUInt32 dest;
ILUInt32 numEntries;
BranchLabel *labelList;
BranchLabel *currLabel;

#else /* IL_VERIFY_CODE */

case IL_OP_BR_S:
{
	/* Unconditional short branch */
	dest = GET_SHORT_DEST();
	ILCoderBranch(coder, opcode, dest, ILEngineType_I4, ILEngineType_I4);
	VALIDATE_BRANCH_STACK(dest);
	lastWasJump = 1;
}
break;

case IL_OP_BRFALSE_S:
case IL_OP_BRTRUE_S:
{
	/* Unary conditional short branch */
  	dest = GET_SHORT_DEST();
unaryBranch:
	commonType = unaryBranchMatrix[STK_UNARY];
	if(commonType != ILEngineType_Invalid)
	{
		ILCoderBranch(coder, opcode, dest, commonType, commonType);
		--stackSize;
		VALIDATE_BRANCH_STACK(dest);
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_BEQ_S:
case IL_OP_BNE_UN_S:
{
	/* Binary equality short branch */
	dest = GET_SHORT_DEST();
binaryEquality:
	if(unsafeAllowed)
	{
		commonType = unsafeCompareMatrix[STK_BINARY_1][STK_BINARY_2];
	}
	else
	{
		commonType = binaryEqualityMatrix[STK_BINARY_1][STK_BINARY_2];
	}
	if(commonType == ILEngineType_M || commonType == ILEngineType_T)
	{
		ILCoderBranchPtr(coder, opcode, dest, STK_BINARY_1, STK_BINARY_2);
		stackSize -= 2;
		VALIDATE_BRANCH_STACK(dest);
	}
	else if(commonType != ILEngineType_Invalid)
	{
		ILCoderBranch(coder, opcode, dest, STK_BINARY_1, STK_BINARY_2);
		stackSize -= 2;
		VALIDATE_BRANCH_STACK(dest);
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_BGT_UN_S:
{
	if(STK_BINARY_1 == ILEngineType_O && STK_BINARY_2 == ILEngineType_O)
	{
		/* "bgt.un" can be used on object references */
		dest = GET_SHORT_DEST();
		ILCoderBranch(coder, opcode, dest, STK_BINARY_1, STK_BINARY_2);
		stackSize -= 2;
		VALIDATE_BRANCH_STACK(dest);
		break;
	}
}
/* Fall through */

case IL_OP_BGE_S:
case IL_OP_BGT_S:
case IL_OP_BLE_S:
case IL_OP_BLT_S:
case IL_OP_BGE_UN_S:
case IL_OP_BLE_UN_S:
case IL_OP_BLT_UN_S:
{
	/* Binary conditional short branch */
	dest = GET_SHORT_DEST();
binaryBranch:
	if(unsafeAllowed)
	{
		commonType = unsafeCompareMatrix[STK_BINARY_1][STK_BINARY_2];
	}
	else
	{
		commonType = binaryCompareMatrix[STK_BINARY_1][STK_BINARY_2];
	}
	if(commonType == ILEngineType_M || commonType == ILEngineType_T)
	{
		ILCoderBranchPtr(coder, opcode, dest, STK_BINARY_1, STK_BINARY_2);
		stackSize -= 2;
		VALIDATE_BRANCH_STACK(dest);
	}
	else if(commonType != ILEngineType_Invalid)
	{
		ILCoderBranch(coder, opcode, dest, STK_BINARY_1, STK_BINARY_2);
		stackSize -= 2;
		VALIDATE_BRANCH_STACK(dest);
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_BR:
{
	/* Unconditional long branch */
	dest = GET_LONG_DEST();
	ILCoderBranch(coder, opcode, dest, ILEngineType_I4, ILEngineType_I4);
	VALIDATE_BRANCH_STACK(dest);
	lastWasJump = 1;
}
break;

case IL_OP_BRFALSE:
case IL_OP_BRTRUE:
{
	/* Unary conditional long branch */
	dest = GET_LONG_DEST();
	goto unaryBranch;
}
/* Not reached */

case IL_OP_BEQ:
case IL_OP_BNE_UN:
{
	/* Binary equality long branch */
	dest = GET_LONG_DEST();
	goto binaryEquality;
}
/* Not reached */

case IL_OP_BGT_UN:
{
	if(STK_BINARY_1 == ILEngineType_O && STK_BINARY_2 == ILEngineType_O)
	{
		/* "bgt.un" can be used on object references */
		dest = GET_LONG_DEST();
		ILCoderBranch(coder, opcode, dest, STK_BINARY_1, STK_BINARY_2);
		stackSize -= 2;
		VALIDATE_BRANCH_STACK(dest);
		break;
	}
}
/* Fall through */

case IL_OP_BGE:
case IL_OP_BGT:
case IL_OP_BLE:
case IL_OP_BLT:
case IL_OP_BGE_UN:
case IL_OP_BLE_UN:
case IL_OP_BLT_UN:
{
	/* Binary conditional long branch */
	dest = GET_LONG_DEST();
	goto binaryBranch;
}
/* Not reached */

case IL_OP_SWITCH:
{
	/* Switch statement */
	if(STK_UNARY == ILEngineType_I4)
	{
		numEntries = IL_READ_UINT32(pc + 1);
		insnSize = 5 + numEntries * 4;
		ILCoderSwitchStart(coder, numArgs);
		for(argNum = 0; argNum < numEntries; ++argNum)
		{
			dest = (ILUInt32)((pc + insnSize) - (unsigned char *)(code->code)) +
				   (ILUInt32)(IL_READ_INT32(pc + 5 + argNum * 4));
			ILCoderSwitchEntry(coder, dest);
			VALIDATE_BRANCH_STACK(dest);
		}
		--stackSize;
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;


case IL_OP_PREFIX + IL_PREFIX_OP_CEQ:
{
	/* Binary equality comparison */
	if(unsafeAllowed)
	{
		commonType = unsafeCompareMatrix[STK_BINARY_1][STK_BINARY_2];
	}
	else
	{
		commonType = binaryEqualityMatrix[STK_BINARY_1][STK_BINARY_2];
	}
	if(commonType == ILEngineType_M || commonType == ILEngineType_T)
	{
		ILCoderComparePtr(coder, opcode, STK_BINARY_1, STK_BINARY_2);
		STK_BINARY_1 = ILEngineType_I4;
		--stackSize;
	}
	else if(commonType != ILEngineType_Invalid)
	{
		ILCoderCompare(coder, opcode, STK_BINARY_1, STK_BINARY_2);
		STK_BINARY_1 = ILEngineType_I4;
		--stackSize;
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_CGT:
case IL_OP_PREFIX + IL_PREFIX_OP_CGT_UN:
case IL_OP_PREFIX + IL_PREFIX_OP_CLT:
case IL_OP_PREFIX + IL_PREFIX_OP_CLT_UN:
{
	/* Binary conditional comparison */
	if(unsafeAllowed)
	{
		commonType = unsafeCompareMatrix[STK_BINARY_1][STK_BINARY_2];
	}
	else
	{
		commonType = binaryCompareMatrix[STK_BINARY_1][STK_BINARY_2];
	}
	if(commonType == ILEngineType_M || commonType == ILEngineType_T)
	{
		ILCoderComparePtr(coder, opcode, STK_BINARY_1, STK_BINARY_2);
		STK_BINARY_1 = ILEngineType_I4;
		--stackSize;
	}
	else if(commonType != ILEngineType_Invalid)
	{
		ILCoderCompare(coder, opcode, STK_BINARY_1, STK_BINARY_2);
		STK_BINARY_1 = ILEngineType_I4;
		--stackSize;
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

#endif /* IL_VERIFY_CODE */
