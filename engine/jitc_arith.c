/*
 * jitc_arith.c - Coder implementation for JIT arithmetic operations.
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
 * Handle a binary opcode.
 */
static void JITCoder_Binary(ILCoder *coder, int opcode,
							ILEngineType type1, ILEngineType type2)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	_ILJitStackItemNew(value2);
	_ILJitStackItemNew(value1);
	ILJitValue result = 0;

	_ILJitStackPop(jitCoder, value2);
	_ILJitStackPop(jitCoder, value1);
	switch(opcode)
	{
		case IL_OP_ADD:
		{
			AdjustMixedBinary(jitCoder, 0,
							  &(_ILJitStackItemValue(value1)),
							  &(_ILJitStackItemValue(value2)));
			result = jit_insn_add(jitCoder->jitFunction,
								  _ILJitStackItemValue(value1),
								  _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_ADD_OVF:
		{
			AdjustMixedBinary(jitCoder, 0,
							  &(_ILJitStackItemValue(value1)),
							  &(_ILJitStackItemValue(value2)));
			result = jit_insn_add_ovf(jitCoder->jitFunction,
									  _ILJitStackItemValue(value1),
									  _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_ADD_OVF_UN:
		{
			AdjustMixedBinary(jitCoder, 1,
							  &(_ILJitStackItemValue(value1)),
							  &(_ILJitStackItemValue(value2)));
			result = jit_insn_add_ovf(jitCoder->jitFunction,
									 _ILJitStackItemValue(value1),
									 _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_SUB:
		{
			AdjustMixedBinary(jitCoder, 0,
							  &(_ILJitStackItemValue(value1)),
							  &(_ILJitStackItemValue(value2)));
			result = jit_insn_sub(jitCoder->jitFunction,
								  _ILJitStackItemValue(value1),
								  _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_SUB_OVF:
		{
			AdjustMixedBinary(jitCoder, 0,
							  &(_ILJitStackItemValue(value1)),
							  &(_ILJitStackItemValue(value2)));
			result = jit_insn_sub_ovf(jitCoder->jitFunction,
									  _ILJitStackItemValue(value1),
									  _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_SUB_OVF_UN:
		{
			AdjustMixedBinary(jitCoder, 1,
							  &(_ILJitStackItemValue(value1)),
							  &(_ILJitStackItemValue(value2)));
			result = jit_insn_sub_ovf(jitCoder->jitFunction,
									  _ILJitStackItemValue(value1),
									  _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_MUL:
		{
			AdjustMixedBinary(jitCoder, 0,
							  &(_ILJitStackItemValue(value1)),
							  &(_ILJitStackItemValue(value2)));
			result = jit_insn_mul(jitCoder->jitFunction,
								  _ILJitStackItemValue(value1),
								  _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_MUL_OVF:
		{
			AdjustMixedBinary(jitCoder, 0,
							  &(_ILJitStackItemValue(value1)),
							  &(_ILJitStackItemValue(value2)));
			result = jit_insn_mul_ovf(jitCoder->jitFunction,
									  _ILJitStackItemValue(value1),
									  _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_MUL_OVF_UN:
		{
			AdjustMixedBinary(jitCoder, 1,
							  &(_ILJitStackItemValue(value1)),
							  &(_ILJitStackItemValue(value2)));
			result = jit_insn_mul_ovf(jitCoder->jitFunction,
									  _ILJitStackItemValue(value1),
									  _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_DIV:
		{
			AdjustMixedBinary(jitCoder, 0,
							  &(_ILJitStackItemValue(value1)),
							  &(_ILJitStackItemValue(value2)));
			result = jit_insn_div(jitCoder->jitFunction,
								  _ILJitStackItemValue(value1),
								  _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_DIV_UN:
		{
			AdjustMixedBinary(jitCoder, 1,
							  &(_ILJitStackItemValue(value1)),
							  &(_ILJitStackItemValue(value2)));
			result = jit_insn_div(jitCoder->jitFunction,
								  _ILJitStackItemValue(value1),
								  _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_REM:
		{
			AdjustMixedBinary(jitCoder, 0,
							  &(_ILJitStackItemValue(value1)),
							  &(_ILJitStackItemValue(value2)));
			result = jit_insn_rem_ieee(jitCoder->jitFunction,
									   _ILJitStackItemValue(value1),
									   _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_REM_UN:
		{
			AdjustMixedBinary(jitCoder, 1,
							  &(_ILJitStackItemValue(value1)),
							  &(_ILJitStackItemValue(value2)));
			result = jit_insn_rem_ieee(jitCoder->jitFunction,
									   _ILJitStackItemValue(value1),
									   _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_AND:
		{
			result = jit_insn_and(jitCoder->jitFunction,
								  _ILJitStackItemValue(value1),
								  _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_OR:
		{
			result = jit_insn_or(jitCoder->jitFunction,
								 _ILJitStackItemValue(value1),
								 _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_XOR:
		{
			result = jit_insn_xor(jitCoder->jitFunction,
								  _ILJitStackItemValue(value1),
								  _ILJitStackItemValue(value2));
		}
		break;
		
		default:
		{
			return;
		}
	}
	_ILJitStackPushValue(jitCoder, result);
}

/*
 * Handle a binary opcode when pointer arithmetic is involved.
 */
static void JITCoder_BinaryPtr(ILCoder *coder, int opcode,
							   ILEngineType type1, ILEngineType type2)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	_ILJitStackItemNew(value2);
	_ILJitStackItemNew(value1);
	ILJitValue result = 0;

	_ILJitStackPop(jitCoder, value2);
	_ILJitStackPop(jitCoder, value1);
	switch(opcode)
	{
		case IL_OP_ADD:
		{
			result = jit_insn_add(jitCoder->jitFunction,
								  _ILJitStackItemValue(value1),
								  _ILJitStackItemValue(value2));
		}
		case IL_OP_ADD_OVF_UN:
		{
			result = jit_insn_add_ovf(jitCoder->jitFunction,
									  _ILJitStackItemValue(value1),
									  _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_SUB:
		{
			result = jit_insn_sub(jitCoder->jitFunction,
								  _ILJitStackItemValue(value1),
								  _ILJitStackItemValue(value2));
		}
		case IL_OP_SUB_OVF_UN:
		{
			result = jit_insn_sub_ovf(jitCoder->jitFunction,
									  _ILJitStackItemValue(value1),
									  _ILJitStackItemValue(value2));
		}
		break;
	}
	_ILJitStackPushValue(jitCoder, result);
}

/*
 * Handle a shift opcode.
 */
static void JITCoder_Shift(ILCoder *coder, int opcode,
						   ILEngineType type1, ILEngineType type2)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	_ILJitStackItemNew(value2);
	_ILJitStackItemNew(value1);
	ILJitValue result = 0;

	_ILJitStackPop(jitCoder, value2);
	_ILJitStackPop(jitCoder, value1);
	/* Determine how to perform the operation */
	switch(opcode)
	{
		case IL_OP_SHL:
		{
			AdjustSign(jitCoder->jitFunction,
					   &(_ILJitStackItemValue(value1)), 0, 0);
			result = jit_insn_shl(jitCoder->jitFunction,
								  _ILJitStackItemValue(value1),
								  _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_SHR:
		{
			AdjustSign(jitCoder->jitFunction,
					   &(_ILJitStackItemValue(value1)), 0, 0);
			result= jit_insn_shr(jitCoder->jitFunction,
								 _ILJitStackItemValue(value1),
								 _ILJitStackItemValue(value2));
		}
		break;

		case IL_OP_SHR_UN:
		{
			AdjustSign(jitCoder->jitFunction, 
					   &(_ILJitStackItemValue(value1)), 1, 0);
			result = jit_insn_shr(jitCoder->jitFunction,
								  _ILJitStackItemValue(value1),
								  _ILJitStackItemValue(value2));
		}
		break;
	}
	_ILJitStackPushValue(jitCoder, result);
}

/*
 * Handle a unary opcode.
 */
static void JITCoder_Unary(ILCoder *coder, int opcode, ILEngineType type)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	_ILJitStackItemNew(value);
	ILJitValue result = 0;

	_ILJitStackPop(jitCoder, value);
	switch(opcode)
	{
		case IL_OP_NEG:
		{
			result = jit_insn_neg(jitCoder->jitFunction,
								  _ILJitStackItemValue(value));
		}
		break;

		case IL_OP_NOT:
		{
			result = jit_insn_not(jitCoder->jitFunction,
								  _ILJitStackItemValue(value));
		}
		break;

		case IL_OP_CKFINITE:
		{
			/* Check the stack Top-most F value to see if it is finite */
			result = jit_insn_is_finite(jitCoder->jitFunction,
										_ILJitStackItemValue(value));
		}
		break;
	}
	_ILJitStackPushValue(jitCoder, result);
}

#endif	/* IL_JITC_CODE */
