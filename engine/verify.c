/*
 * verify.c - Bytecode verifier for the engine.
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

#include "engine.h"
#include "il_coder.h"
#include "il_opcodes.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Temporary memory allocator.
 */
typedef struct
{
	char	buffer[256];
	int		posn;
	void   *overflow;

} TempAllocator;

/*
 * Allocate a block of zero'ed memory from a temporary memory allocator.
 */
static void *TempAllocate(TempAllocator *allocator, unsigned long size)
{
	void *ptr;

	/* Round the size to a multiple of the machine word size */
	if(sizeof(unsigned long) > sizeof(void *))
	{
		size = (size + sizeof(unsigned long) - 1) &
				~(sizeof(unsigned long) - 1);
	}
	else
	{
		size = (size + sizeof(void *) - 1) & ~(sizeof(void *) - 1);
	}

	/* Can we allocate from the primary buffer? */
	if(size < 256 && (allocator->posn + (int)size) < 256)
	{
		ptr = (void *)(allocator->buffer + allocator->posn);
		allocator->posn += (int)size;
		return ptr;
	}

	/* Allocate from the system heap */
	ptr = ILMalloc(sizeof(void *) + size);
	if(!ptr)
	{
		return 0;
	}
	*((void **)ptr) = allocator->overflow;
	allocator->overflow = ptr;
	return (void *)(((void **)ptr) + 1);
}

/*
 * Destroy a temporary memory allocator.
 */
static void TempAllocatorDestroy(TempAllocator *allocator)
{
	void *ptr, *nextPtr;
	ptr = allocator->overflow;
	while(ptr != 0)
	{
		nextPtr = *((void **)ptr);
		ILFree(ptr);
		ptr = nextPtr;
	}
}

/*
 * Number of bytes needed for a bit mask of a specific size.
 * 3 bits are allocated for each item: "instruction start",
 * "jump target", and "special jump target".
 */
#define	BYTES_FOR_MASK(size)	\
			(((size) * 3 + (sizeof(unsigned long) * 8) - 1) / 8)

/*
 * Mark an instruction starting point in a jump mask.
 */
static IL_INLINE void MarkInsnStart(unsigned long *jumpMask, ILUInt32 offset)
{
	offset *= 3;
	jumpMask[offset / (sizeof(unsigned long) * 8)] |=
		(((unsigned long)1) << (offset % (sizeof(unsigned long) * 8)));
}

/*
 * Mark a jump target point in a jump mask.
 */
static IL_INLINE void MarkJumpTarget(unsigned long *jumpMask, ILUInt32 offset)
{
	offset = offset * 3 + 1;
	jumpMask[offset / (sizeof(unsigned long) * 8)] |=
		(((unsigned long)1) << (offset % (sizeof(unsigned long) * 8)));
}

/*
 * Mark a special jump target point in a jump mask.
 */
static IL_INLINE void MarkSpecialJumpTarget(unsigned long *jumpMask,
											ILUInt32 offset)
{
	offset = offset * 3 + 2;
	jumpMask[offset / (sizeof(unsigned long) * 8)] |=
		(((unsigned long)1) << (offset % (sizeof(unsigned long) * 8)));
}

/*
 * Determine if a code offset is an instruction start.
 */
static IL_INLINE int IsInsnStart(unsigned long *jumpMask, ILUInt32 offset)
{
	offset *= 3;
	return ((jumpMask[offset / (sizeof(unsigned long) * 8)] &
		(((unsigned long)1) << (offset % (sizeof(unsigned long) * 8))))
				!= 0);
}

/*
 * Determine if a code offset is a jump target.
 */
static IL_INLINE int IsJumpTarget(unsigned long *jumpMask, ILUInt32 offset)
{
	offset = offset * 3 + 1;
	return ((jumpMask[offset / (sizeof(unsigned long) * 8)] &
		(((unsigned long)1) << (offset % (sizeof(unsigned long) * 8))))
				!= 0);
}

/*
 * Determine if a code offset is a special jump target.
 */
static IL_INLINE int IsSpecialJumpTarget(unsigned long *jumpMask,
										 ILUInt32 offset)
{
	offset = offset * 3 + 2;
	return ((jumpMask[offset / (sizeof(unsigned long) * 8)] &
		(((unsigned long)1) << (offset % (sizeof(unsigned long) * 8))))
				!= 0);
}

/*
 * Convert a type into an engine type.
 */
static ILEngineType TypeToEngineType(ILType *type)
{
	type = ILTypeGetEnumType(type);
	if(ILType_IsPrimitive(type))
	{
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_BOOLEAN:
			case IL_META_ELEMTYPE_I1:
			case IL_META_ELEMTYPE_U1:
			case IL_META_ELEMTYPE_I2:
			case IL_META_ELEMTYPE_U2:
			case IL_META_ELEMTYPE_CHAR:
			case IL_META_ELEMTYPE_I4:
			case IL_META_ELEMTYPE_U4:		return ILEngineType_I4;

			case IL_META_ELEMTYPE_I8:
			case IL_META_ELEMTYPE_U8:		return ILEngineType_I8;

			case IL_META_ELEMTYPE_I:
			case IL_META_ELEMTYPE_U:		return ILEngineType_I;

			case IL_META_ELEMTYPE_R4:
			case IL_META_ELEMTYPE_R8:
			case IL_META_ELEMTYPE_R:		return ILEngineType_F;
		}
		return ILEngineType_I4;
	}
	else if(ILType_IsValueType(type))
	{
		return ILEngineType_MV;
	}
	else if(ILType_IsComplex(type) && type != 0)
	{
		switch(type->kind & 0xFF)
		{
			case IL_TYPE_COMPLEX_PTR:
			{
				/* Unsafe pointers are represented as native integers */
				return ILEngineType_I;
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_BYREF:
			{
				/* Reference values are managed pointers */
				return ILEngineType_M;
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_PINNED:
			{
				/* Pinned types are the same as their underlying type */
				return TypeToEngineType(type->un.refType);
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_CMOD_REQD:
			case IL_TYPE_COMPLEX_CMOD_OPT:
			{
				/* Strip the modifier and inspect the underlying type */
				return TypeToEngineType(type->un.modifier.type);
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_METHOD:
			case IL_TYPE_COMPLEX_METHOD | IL_TYPE_COMPLEX_METHOD_SENTINEL:
			{
				/* Pass method pointers around the system as "I".  This is
				   not strictly ECMA-compliant, but is useful nonetheless.
				   Logic and functional languages pass method pointers
				   around to implement continuations.  Normally this results
				   in unverifiable code.  However, by treating method types
				   in this way, we can create verifiable continuation code */
				return ILEngineType_I;
			}
			/* Not reached */
		}
	}
	return ILEngineType_O;
}

/*
 * Determine if a type is represented as an object reference.
 */
static int IsObjectRef(ILType *type)
{
	if(type == 0)
	{
		/* This is the "null" type, which is always an object reference */
		return 1;
	}
	else if(ILType_IsClass(type))
	{
		return 1;
	}
	else if(ILType_IsComplex(type) &&
	        (type->kind == IL_TYPE_COMPLEX_ARRAY ||
			 type->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * Determine if a stack item is assignment-compatible with
 * a particular memory slot (argument, local, field, etc).
 */
static int AssignCompatible(ILMethod *method, ILEngineStackItem *item,
							ILType *type)
{
	ILImage *image;
	ILClass *classInfo;
	ILClass *classInfo2;

	if(item->engineType == ILEngineType_I &&
	   item->typeInfo != 0 && ILType_IsComplex(item->typeInfo))
	{
		/* May be trying to assign a method pointer to a method type.
		   See the comments in "TypeToEngineType" for further details */
		if((item->typeInfo->kind & IL_TYPE_COMPLEX_METHOD) != 0)
		{
			return ILTypeIdentical(item->typeInfo, type);
		}
		else
		{
			return 0;
		}
	}
	else if(item->engineType == ILEngineType_I4 ||
	        item->engineType == ILEngineType_I)
	{
		type = ILTypeGetEnumType(type);
		switch((unsigned long)type)
		{
			case (unsigned long)ILType_Boolean:
			case (unsigned long)ILType_Int8:
			case (unsigned long)ILType_UInt8:
			case (unsigned long)ILType_Int16:
			case (unsigned long)ILType_UInt16:
			case (unsigned long)ILType_Char:
			case (unsigned long)ILType_Int32:
			case (unsigned long)ILType_UInt32:
			case (unsigned long)ILType_Int:
			case (unsigned long)ILType_UInt:	return 1;
			default: break;
		}
		return 0;
	}
	else if(item->engineType == ILEngineType_I8)
	{
		type = ILTypeGetEnumType(type);
		return (type == ILType_Int64 || type == ILType_UInt64);
	}
	else if(item->engineType == ILEngineType_F)
	{
		return (type == ILType_Float32 ||
		        type == ILType_Float64 ||
		        type == ILType_Float);
	}
	else if(item->engineType == ILEngineType_O)
	{
		if(!(item->typeInfo))
		{
			/* A "null" constant was pushed, which is
			   compatible with any object reference type */
			return IsObjectRef(type);
		}
		if(!IsObjectRef(type) || !IsObjectRef(item->typeInfo))
		{
			/* Both types must be object references */
			return 0;
		}
		image = ILProgramItem_Image(method);
		classInfo = ILClassFromType(image, 0, type, 0);
		classInfo2 = ILClassFromType(image, 0, item->typeInfo, 0);
		if(classInfo && classInfo2)
		{
			/* Is the type a regular class or an interface? */
			if(!ILClass_IsInterface(classInfo))
			{
				/* Regular class: the value must inherit from the type */
				return ILClassInheritsFrom(classInfo2, classInfo);
			}
			else
			{
				/* Interface which the value must implement or inherit from */
				return ILClassImplements(classInfo2, classInfo) ||
				       ILClassInheritsFrom(classInfo2, classInfo);
			}
		}
		else
		{
			return 0;
		}
	}
	else if(item->engineType == ILEngineType_MV)
	{
		/* Can only assign managed values to an exact type destination */
		return ILTypeIdentical(item->typeInfo, type);
	}
	else
	{
		/* TODO */
		return 0;
	}
}

/*
 * Bailout routines for various kinds of verification failure.
 */
/*#define	IL_VERIFY_DEBUG*/
#ifdef IL_VERIFY_DEBUG
#define	VERIFY_REPORT()	\
			do { \
				fprintf(stderr, "%s::%s [%lX] - %s\n", \
						ILClass_Name(ILMethod_Owner(method)), \
						ILMethod_Name(method), \
						(unsigned long)(offset + ILMethod_RVA(method)), \
						insn->name); \
			} while (0)
#else
#define	VERIFY_REPORT()	do {} while (0)
#endif
#define	VERIFY_TRUNCATED()		VERIFY_REPORT(); goto cleanup
#define	VERIFY_BRANCH_ERROR()	VERIFY_REPORT(); goto cleanup
#define	VERIFY_INSN_ERROR()		VERIFY_REPORT(); goto cleanup
#define	VERIFY_STACK_ERROR()	VERIFY_REPORT(); goto cleanup
#define	VERIFY_TYPE_ERROR()		VERIFY_REPORT(); goto cleanup
#define	VERIFY_MEMORY_ERROR()	VERIFY_REPORT(); goto cleanup

/*
 * Declare global definitions that are required by the include files.
 */
#define	IL_VERIFY_GLOBALS
#include "verify_var.c"
#include "verify_const.c"
#include "verify_arith.c"
#include "verify_conv.c"
#include "verify_stack.c"
#include "verify_ptr.c"
#include "verify_call.c"
#include "verify_obj.c"
#include "verify_branch.c"
#include "verify_except.c"
#include "verify_ann.c"
#undef IL_VERIFY_GLOBALS

int _ILVerify(ILCoder *coder, unsigned char **start, ILMethod *method,
			  ILMethodCode *code, int unsafeAllowed)
{
	TempAllocator allocator;
	unsigned long *jumpMask;
	unsigned char *pc;
	ILUInt32 len;
	int result;
	unsigned opcode;
	ILUInt32 insnSize;
	int insnType;
	ILUInt32 offset;
	ILEngineStackItem *stack;
	ILUInt32 stackSize;
	const ILOpcodeInfo *insn;
	ILType *signature;
	ILType *type;
	ILUInt32 numArgs;
	ILUInt32 numLocals;
	ILType *localVars;
	int lastWasJump;
	ILException *exceptions;
	ILException *exception;

	/* Include local variables that are required by the include files */
#define IL_VERIFY_LOCALS
#include "verify_var.c"
#include "verify_const.c"
#include "verify_arith.c"
#include "verify_conv.c"
#include "verify_stack.c"
#include "verify_ptr.c"
#include "verify_call.c"
#include "verify_obj.c"
#include "verify_branch.c"
#include "verify_except.c"
#include "verify_ann.c"
#undef IL_VERIFY_LOCALS

	/* Get the exception list */
	if(!ILMethodGetExceptions(method, code, &exceptions))
	{
		return 0;
	}

restart:
	result = 0;
	labelList = 0;

	/* Initialize the memory allocator that is used for temporary
	   allocation during bytecode verification */
	ILMemZero(allocator.buffer, sizeof(allocator.buffer));
	allocator.posn = 0;
	allocator.overflow = 0;

	/* Set up the coder to process the method */
	if(!ILCoderSetup(coder, start, method, code))
	{
		VERIFY_MEMORY_ERROR();
	}

	/* Allocate the jump target mask */
	jumpMask = (unsigned long *)TempAllocate
					(&allocator, BYTES_FOR_MASK(code->codeLen));
	if(!jumpMask)
	{
		VERIFY_MEMORY_ERROR();
	}

	/* Scan the code looking for all jump targets, and validating
	   that all instructions are more or less valid */
	pc = code->code;
	len = code->codeLen;
	while(len > 0)
	{
		/* Mark this position in the jump mask as an instruction start */
		MarkInsnStart(jumpMask, (ILUInt32)(pc - (unsigned char *)(code->code)));

		/* Fetch the instruction size and type */
		opcode = (unsigned)(pc[0]);
		if(opcode != IL_OP_PREFIX)
		{
			/* Regular opcode */
			insnSize = (ILUInt32)(ILMainOpcodeTable[opcode].size);
			if(len < insnSize)
			{
				VERIFY_TRUNCATED();
			}
			insnType = ILMainOpcodeTable[opcode].args;
		}
		else
		{
			/* Prefixed opcode */
			if(len < 2)
			{
				VERIFY_TRUNCATED();
			}
			opcode = (unsigned)(pc[1]);
			insnSize = (ILUInt32)(ILPrefixOpcodeTable[opcode].size);
			if(len < insnSize)
			{
				VERIFY_TRUNCATED();
			}
			insnType = ILPrefixOpcodeTable[opcode].args;
			opcode += IL_OP_PREFIX;
		}

		/* Determine how to handle this type of instruction */
		switch(insnType)
		{
			case IL_OPCODE_ARGS_SHORT_JUMP:
			{
				/* 8-bit jump offset */
				offset = (ILUInt32)((pc + insnSize) -
										(unsigned char *)(code->code)) +
						 (ILUInt32)(ILInt32)(ILInt8)(pc[1]);
				if(offset >= code->codeLen)
				{
					VERIFY_BRANCH_ERROR();
				}
				MarkJumpTarget(jumpMask, offset);
			}
			break;

			case IL_OPCODE_ARGS_LONG_JUMP:
			{
				/* 32-bit jump offset */
				offset = (ILUInt32)((pc + insnSize) -
										(unsigned char *)(code->code)) +
						 (ILUInt32)(IL_READ_INT32(pc + 1));
				if(offset >= code->codeLen)
				{
					VERIFY_BRANCH_ERROR();
				}
				MarkJumpTarget(jumpMask, offset);
			}
			break;

			case IL_OPCODE_ARGS_SWITCH:
			{
				/* Switch statement */
				if(len < 5)
				{
					VERIFY_TRUNCATED();
				}
				numArgs = IL_READ_UINT32(pc + 1);
				insnSize = 5 + numArgs * 4;
				if(numArgs >= 0x20000000 || len < insnSize)
				{
					VERIFY_TRUNCATED();
				}
				while(numArgs > 0)
				{
					--numArgs;
					offset = (ILUInt32)((pc + insnSize) -
											(unsigned char *)(code->code)) +
							 (ILUInt32)(IL_READ_INT32(pc + 5 + numArgs * 4));
					if(offset >= code->codeLen)
					{
						VERIFY_BRANCH_ERROR();
					}
					MarkJumpTarget(jumpMask, offset);
				}
			}
			break;

			case IL_OPCODE_ARGS_ANN_DATA:
			{
				/* Variable-length annotation data */
				if(opcode == IL_OP_ANN_DATA_S)
				{
					if(len < 2)
					{
						VERIFY_TRUNCATED();
					}
					insnSize = (((ILUInt32)(pc[1])) & 0xFF) + 2;
					if(len < insnSize)
					{
						VERIFY_TRUNCATED();
					}
				}
				else
				{
					if(len < 6)
					{
						VERIFY_TRUNCATED();
					}
					insnSize = (IL_READ_UINT32(pc + 2) + 6);
					if(len < insnSize)
					{
						VERIFY_TRUNCATED();
					}
				}
			}
			break;

			case IL_OPCODE_ARGS_ANN_PHI:
			{
				/* Variable-length annotation data */
				if(len < 3)
				{
					VERIFY_TRUNCATED();
				}
				insnSize = ((ILUInt32)IL_READ_UINT16(pc + 1)) * 2 + 3;
				if(len < insnSize)
				{
					VERIFY_TRUNCATED();
				}
			}
			break;

			case IL_OPCODE_ARGS_INVALID:
			{
				VERIFY_INSN_ERROR();
			}
			break;

			default: break;
		}

		/* Advance to the next instruction */
		pc += insnSize;
		len -= insnSize;
	}

	/* Mark the start and end of exception blocks as special jump targets */
	exception = exceptions;
	while(exception != 0)
	{
		/* Mark the start and end of the try region */
		if(exception->tryOffset >= code->codeLen ||
		   (exception->tryOffset + exception->tryLength) <
		   		exception->tryOffset || /* Wrap-around check */
		   (exception->tryOffset + exception->tryLength) > code->codeLen)
		{
			VERIFY_BRANCH_ERROR();
		}
		MarkJumpTarget(jumpMask, exception->tryOffset);
		MarkSpecialJumpTarget(jumpMask, exception->tryOffset);
		MarkJumpTarget(jumpMask, exception->tryOffset + exception->tryLength);
		MarkSpecialJumpTarget
			(jumpMask, exception->tryOffset + exception->tryLength);

		/* The stack must be empty on entry to the try region */
		SET_TARGET_STACK_EMPTY(exception->tryOffset);

		/* What else do we need to do? */
		if((exception->flags & IL_META_EXCEPTION_FILTER) != 0)
		{
			/* This is an exception filter */
			if(exception->extraArg >= code->codeLen)
			{
				VERIFY_BRANCH_ERROR();
			}
			MarkJumpTarget(jumpMask, exception->extraArg);
			MarkSpecialJumpTarget(jumpMask, exception->extraArg);

			/* The filter label will be called with an object on the stack,
			   so record that in the label list for later */
			classInfo = ILClassResolveSystem(ILProgramItem_Image(method), 0,
											 "Object", "System");
			if(!classInfo)
			{
				/* Ran out of memory trying to create "System.Object" */
				VERIFY_MEMORY_ERROR();
			}
			SET_TARGET_STACK(exception->extraArg, classInfo);
		}
		else if((exception->flags & IL_META_EXCEPTION_FINALLY) != 0 ||
		        (exception->flags & IL_META_EXCEPTION_FAULT) != 0)
		{
			/* This is a finally or fault clause */
			if(exception->handlerOffset >= code->codeLen ||
			   (exception->handlerOffset + exception->handlerLength) <
			   		exception->handlerOffset || /* Wrap-around check */
			   (exception->handlerOffset + exception->handlerLength) >
			   		code->codeLen)
			{
				VERIFY_BRANCH_ERROR();
			}
			MarkJumpTarget(jumpMask, exception->handlerOffset);
			MarkSpecialJumpTarget(jumpMask, exception->handlerOffset);
			MarkJumpTarget
				(jumpMask, exception->handlerOffset + exception->handlerLength);
			MarkSpecialJumpTarget
				(jumpMask, exception->handlerOffset + exception->handlerLength);

			/* The clause will be called with nothing on the stack */
			SET_TARGET_STACK_EMPTY(exception->handlerOffset);
		}
		else
		{
			/* This is a catch block */
			if(exception->handlerOffset >= code->codeLen ||
			   (exception->handlerOffset + exception->handlerLength) <
			   		exception->handlerOffset || /* Wrap-around check */
			   (exception->handlerOffset + exception->handlerLength) >
			   		code->codeLen)
			{
				VERIFY_BRANCH_ERROR();
			}
			MarkJumpTarget(jumpMask, exception->handlerOffset);
			MarkSpecialJumpTarget(jumpMask, exception->handlerOffset);
			MarkJumpTarget
				(jumpMask, exception->handlerOffset + exception->handlerLength);
			MarkSpecialJumpTarget
				(jumpMask, exception->handlerOffset + exception->handlerLength);

			/* Validate the class token */
			classInfo = ILProgramItemToClass
				((ILProgramItem *)ILImageTokenInfo(ILProgramItem_Image(method),
												   exception->extraArg));
			if(classInfo &&
			   ILClassAccessible(classInfo, ILMethod_Owner(method)))
			{
				/* The handler label will be called with an object on the
				   stack, so record that in the label list for later */
				SET_TARGET_STACK(exception->handlerOffset, classInfo);
			}
			else
			{
				/* The class token is invalid, or not accessible to us */
				VERIFY_TYPE_ERROR();
			}
		}

		/* Move on to the next exception */
		exception = exception->next;
	}

	/* Make sure that all jump targets are instruction starts */
	len = code->codeLen;
	while(len > 0)
	{
		--len;
		if(IsJumpTarget(jumpMask, len) && !IsInsnStart(jumpMask, len))
		{
			VERIFY_BRANCH_ERROR();
		}
	}

	/* Create the stack.  We need two extra "slop" items to allow for
	   stack expansion during object construction.  See "verify_call.c"
	   for further details */
	stack = (ILEngineStackItem *)TempAllocate
				(&allocator, sizeof(ILEngineStackItem) * (code->maxStack + 2));
	if(!stack)
	{
		VERIFY_MEMORY_ERROR();
	}
	stackSize = 0;

	/* Get the method signature, plus the number of arguments and locals */
	signature = ILMethod_Signature(method);
	numArgs = signature->num;
	if(ILType_HasThis(signature))
	{
		/* Account for the "this" argument */
		++numArgs;
	}
	if(code->localVarSig)
	{
		localVars = ILStandAloneSigGetType(code->localVarSig);
		numLocals = ILTypeNumLocals(localVars);
	}
	else
	{
		localVars = 0;
		numLocals = 0;
	}

	/* Verify the code */
	pc = code->code;
	len = code->codeLen;
	lastWasJump = 0;
	while(len > 0)
	{
		/* Is this a jump target? */
		offset = (ILUInt32)(pc - (unsigned char *)(code->code));
		if(IsJumpTarget(jumpMask, offset))
		{
			/* Validate the stack information */
			VALIDATE_TARGET_STACK(offset);

			/* Notify the coder of a label at this position */
			ILCoderLabel(coder, offset);
			ILCoderStackRefresh(coder, stack, stackSize);
		}
		else if(lastWasJump)
		{
			/* An instruction just after an opcode that jumps to
			   somewhere else in the flow of control.  As this
			   isn't a jump target, we assume that the stack
			   must be empty at this point */
			if(stackSize != 0)
			{
				VERIFY_STACK_ERROR();
			}

			/* Reset the coder's notion of the stack to empty */
			ILCoderStackRefresh(coder, stack, stackSize);
		}

		/* Fetch the instruction information block */
		opcode = pc[0];
		if(opcode != IL_OP_PREFIX)
		{
			insn = &(ILMainOpcodeTable[opcode]);
		}
		else
		{
			opcode = pc[1];
			insn = &(ILPrefixOpcodeTable[opcode]);
			opcode += IL_OP_PREFIX;
		}
		insnSize = (ILUInt32)(insn->size);

		/* Validate the stack height changes */
		if(stackSize < ((ILUInt32)(insn->popped)) ||
		   (stackSize - ((ILUInt32)(insn->popped)) + ((ILUInt32)(insn->pushed)))
		   		> code->maxStack)
		{
			VERIFY_STACK_ERROR();
		}

		/* Verify the instruction */
		lastWasJump = 0;
		switch(opcode)
		{
			case IL_OP_NOP: break;

#define	IL_VERIFY_CODE
#include "verify_var.c"
#include "verify_const.c"
#include "verify_arith.c"
#include "verify_conv.c"
#include "verify_stack.c"
#include "verify_ptr.c"
#include "verify_call.c"
#include "verify_obj.c"
#include "verify_branch.c"
#include "verify_except.c"
#include "verify_ann.c"
#undef IL_VERIFY_CODE

		}

		/* Advance to the next instruction */
		pc += insnSize;
		len -= insnSize;
	}

	/* If the last instruction was not a jump, then the code
	   may fall off the end of the method */
	if(!lastWasJump)
	{
		VERIFY_INSN_ERROR();
	}

	/* Finish processing using the coder */
	result = ILCoderFinish(coder);

	/* Do we need to restart due to cache exhaustion in the coder? */
	if(ILCoderRestart(coder))
	{
		TempAllocatorDestroy(&allocator);
		goto restart;
	}

	/* Clean up and exit */
cleanup:
	TempAllocatorDestroy(&allocator);
	if(exceptions)
	{
		ILMethodFreeExceptions(exceptions);
	}
	return result;
}

#ifdef	__cplusplus
};
#endif
