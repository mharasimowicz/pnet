/*
 * jitc_var.c - Coder implementation for JIT variables.
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

#ifdef IL_JITC_CODE

/*
 * Convert a local/arg to the type needed on the stack.
 * Returns the converted value when conversion is needed or value as it is.
 */
static ILJitValue ConvertToStack(ILJITCoder *coder, ILJitValue value)
{
	ILJitType type = jit_value_get_type(value);
	ILJitType newType = jit_type_promote_int(type);

	if(type != newType)
	{
		return jit_insn_convert(coder->jitFunction, value, newType, 0);
	}
	if((type == _IL_JIT_TYPE_SINGLE) || (type == _IL_JIT_TYPE_DOUBLE))
	{
		return jit_insn_convert(coder->jitFunction, value, _IL_JIT_TYPE_NFLOAT, 0);
	}
	return value;
}

/*
 * Handle a load from an argument.
 */
static void JITCoder_LoadArg(ILCoder *coder, ILUInt32 argNum, ILType *type)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	/* We need argNum + 1 because the ILExecThread is added as param 0 */
	ILJitValue param = jit_value_get_param(jitCoder->jitFunction, argNum + 1);
	ILJitValue newParam = ConvertToStack(jitCoder, param);

	jitCoder->jitStack[jitCoder->stackTop] = newParam;
	JITC_ADJUST(jitCoder, 1);
}

/*
 * Handle a load from a local variable.
 */
static void JITCoder_LoadLocal(ILCoder *coder, ILUInt32 localNum, ILType *type)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue localValue = jitCoder->jitLocals[localNum];

	jitCoder->jitStack[jitCoder->stackTop] = 
					ConvertToStack(jitCoder, localValue);
	JITC_ADJUST(jitCoder, 1);
}

/*
 * Handle a store to an argument.
 */
static void JITCoder_StoreArg(ILCoder *coder, ILUInt32 argNum,
							  ILEngineType engineType, ILType *type)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	/* We need argNum + 1 because the ILExecThread is added as param 0 */
	ILJitValue argValue = jit_value_get_param(jitCoder->jitFunction, argNum + 1);

	jit_insn_store(jitCoder->jitFunction, argValue,
					jitCoder->jitStack[jitCoder->stackTop - 1]);

	JITC_ADJUST(jitCoder, -1);
}

/*
 * Handle a store to a local variable.
 */
static void JITCoder_StoreLocal(ILCoder *coder, ILUInt32 localNum,
								ILEngineType engineType, ILType *type)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

	jit_insn_store(jitCoder->jitFunction, jitCoder->jitLocals[localNum],
					jitCoder->jitStack[jitCoder->stackTop - 1]);

	JITC_ADJUST(jitCoder, -1);
}

/*
 * Load the address of an argument onto the stack.
 */
static void JITCoder_AddrOfArg(ILCoder *coder, ILUInt32 argNum)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);



}

/*
 * Load the address of a local onto the stack.
 */
static void JITCoder_AddrOfLocal(ILCoder *coder, ILUInt32 localNum)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

}

/*
 * Allocate local stack space.
 */
static void JITCoder_LocalAlloc(ILCoder *coder, ILEngineType sizeType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

}

#endif	/* IL_JITC_CODE */