/*
 * verify_var.c - Verify instructions related to variables.
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
 * Get the type of a parameter to the current method.
 */
static ILType *GetParamType(ILType *signature, ILMethod *method, ILUInt32 num)
{
	ILClass *owner;
	if((signature->kind & (IL_META_CALLCONV_HASTHIS << 8)) != 0 &&
	   (signature->kind & (IL_META_CALLCONV_EXPLICITTHIS << 8)) == 0)
	{
		/* This method has a "this" parameter */
		if(!num)
		{
			owner = ILMethod_Owner(method);
			if(!ILMethod_IsVirtual(method) && ILClassIsValueType(owner))
			{
				/* The "this" parameter is a value type, which is
				   being passed as a managed pointer.  Return
				   ILType_Invalid to tell the caller that special
				   handling is required */
				return ILType_Invalid;
			}
			return ILType_FromClass(owner);
		}
		else
		{
			return ILTypeGetParam(signature, num);
		}
	}
	else
	{
		return ILTypeGetParam(signature, num + 1);
	}
}

#elif defined(IL_VERIFY_LOCALS)

ILUInt32 argNum;

#else /* IL_VERIFY_CODE */

case IL_OP_LDARG_0:
case IL_OP_LDARG_1:
case IL_OP_LDARG_2:
case IL_OP_LDARG_3:
{
	/* Load one of the first four arguments onto the stack */
	argNum = opcode - IL_OP_LDARG_0;
	goto checkLDArg;
}
/* Not reached */

case IL_OP_LDARG_S:
{
	/* Load an argument onto the stack */
	argNum = (ILUInt32)(pc[1]);
	goto checkLDArg;
}
/* Not reached */

case IL_OP_PREFIX + IL_PREFIX_OP_LDARG:
{
	/* Load an argument onto the stack */
	argNum = (ILUInt32)(IL_READ_UINT16(pc + 2));
checkLDArg:
	if(argNum >= numArgs)
	{
		VERIFY_INSN_ERROR();
	}
	stack[stackSize].typeInfo = GetParamType(signature, method, argNum);
	if(stack[stackSize].typeInfo == ILType_Invalid)
	{
		/* The "this" parameter is being passed as a managed pointer */
		stack[stackSize].engineType = ILEngineType_M;
		stack[stackSize].typeInfo =
			ILType_FromValueType(ILMethod_Owner(method));
	}
	else if((stack[stackSize].engineType =
				TypeToEngineType(stack[stackSize].typeInfo)) == ILEngineType_M)
	{
		/* Convert the type of a "BYREF" parameter */
		stack[stackSize].typeInfo = stack[stackSize].typeInfo->un.refType;
	}
	ILCoderLoadArg(coder, argNum, stack[stackSize].typeInfo);
	++stackSize;
}
break;

case IL_OP_STARG_S:
{
	/* Store the top of the stack into an argument */
	argNum = (ILUInt32)(pc[1]);
	goto checkSTArg;
}
/* Not reached */

case IL_OP_PREFIX + IL_PREFIX_OP_STARG:
{
	/* Store the top of the stack into an argument */
	argNum = (ILUInt32)(pc[1]);
checkSTArg:
	if(argNum >= numArgs)
	{
		VERIFY_INSN_ERROR();
	}
	type = GetParamType(signature, method, argNum);
	if(type == ILType_Invalid)
	{
		/* Storing into the "this" argument of a value type method.
		   This should be done using "stobj" instead */
		VERIFY_TYPE_ERROR();
	}
	else if(!AssignCompatible(&(stack[stackSize - 1]), type))
	{
		VERIFY_TYPE_ERROR();
	}
	ILCoderStoreArg(coder, argNum,
				    stack[stackSize - 1].engineType, type);
	--stackSize;
}
break;

case IL_OP_LDLOC_0:
case IL_OP_LDLOC_1:
case IL_OP_LDLOC_2:
case IL_OP_LDLOC_3:
{
	/* Load one of the first four locals onto the stack */
	argNum = opcode - IL_OP_LDLOC_0;
	goto checkLDLoc;
}
/* Not reached */

case IL_OP_LDLOC_S:
{
	/* Load a local variable onto the stack */
	argNum = (ILUInt32)(pc[1]);
	goto checkLDLoc;
}
/* Not reached */

case IL_OP_PREFIX + IL_PREFIX_OP_LDLOC:
{
	/* Load a local variable onto the stack */
	argNum = (ILUInt32)(IL_READ_UINT16(pc + 2));
checkLDLoc:
	if(argNum >= numLocals)
	{
		VERIFY_INSN_ERROR();
	}
	stack[stackSize].typeInfo =
			ILTypeGetLocal(localVars, argNum);
	stack[stackSize].engineType =
			TypeToEngineType(stack[stackSize].typeInfo);
	ILCoderLoadLocal(coder, argNum, stack[stackSize].typeInfo);
	++stackSize;
}
break;

case IL_OP_STLOC_0:
case IL_OP_STLOC_1:
case IL_OP_STLOC_2:
case IL_OP_STLOC_3:
{
	/* Store the stack top to one of the first four locals */
	argNum = opcode - IL_OP_STLOC_0;
	goto checkSTLoc;
}
/* Not reached */

case IL_OP_STLOC_S:
{
	/* Store the top of the stack into a local variable */
	argNum = (ILUInt32)(pc[1]);
	goto checkSTLoc;
}
/* Not reached */

case IL_OP_PREFIX + IL_PREFIX_OP_STLOC:
{
	/* Store the top of the stack into a local variable */
	argNum = (ILUInt32)(IL_READ_UINT16(pc + 2));
checkSTLoc:
	if(argNum >= numLocals)
	{
		VERIFY_INSN_ERROR();
	}
	type = ILTypeGetLocal(localVars, argNum);
	if(!AssignCompatible(&(stack[stackSize - 1]), type))
	{
		VERIFY_TYPE_ERROR();
	}
	ILCoderStoreLocal(coder, argNum,
				      stack[stackSize - 1].engineType, type);
	--stackSize;
}
break;

case IL_OP_LDARGA_S:
{
	/* Load the address of an argument onto the stack */
	argNum = (ILUInt32)(pc[1]);
	goto checkLDArgA;
}
/* Not reached */

case IL_OP_PREFIX + IL_PREFIX_OP_LDARGA:
{
	/* Load the address of an argument onto the stack */
	argNum = (ILUInt32)(IL_READ_UINT16(pc + 2));
checkLDArgA:
	if(argNum >= numArgs)
	{
		VERIFY_INSN_ERROR();
	}
	stack[stackSize].typeInfo = GetParamType(signature, method, argNum);
	if(stack[stackSize].typeInfo == ILType_Invalid)
	{
		/* Cannot take the address of the "this" parameter in
		   a value type method: use "ldarg" instead */
		VERIFY_TYPE_ERROR();
	}
	stack[stackSize].engineType = ILEngineType_T;
	ILCoderAddrOfArg(coder, argNum);
	++stackSize;
}
break;

case IL_OP_LDLOCA_S:
{
	/* Load the address of a local variable onto the stack */
	argNum = (ILUInt32)(pc[1]);
	goto checkLDLocA;
}
/* Not reached */

case IL_OP_PREFIX + IL_PREFIX_OP_LDLOCA:
{
	/* Load the address of a local variable onto the stack */
	argNum = (ILUInt32)(pc[1]);
checkLDLocA:
	if(argNum >= numLocals)
	{
		VERIFY_INSN_ERROR();
	}
	stack[stackSize].typeInfo = ILTypeGetLocal(localVars, argNum);
	stack[stackSize].engineType = ILEngineType_T;
	ILCoderAddrOfLocal(coder, argNum);
	++stackSize;
}
break;

#endif /* IL_VERIFY_CODE */
