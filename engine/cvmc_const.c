/*
 * cvmc_const.c - Coder implementation for CVM constants.
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

#ifdef IL_CVMC_CODE

/*
 * Handle a constant opcode.
 */
static void CVMCoder_Constant(ILCoder *coder, int opcode, unsigned char *arg)
{
	CVM_BYTE(opcode - IL_OP_LDNULL + COP_LDNULL);
	CVM_ADJUST(1);
	if(opcode == IL_OP_LDC_I4_S)
	{
		CVM_BYTE(arg[0]);
	}
	else if(opcode == IL_OP_LDC_I4)
	{
		CVM_BYTE(arg[0]);
		CVM_BYTE(arg[1]);
		CVM_BYTE(arg[2]);
		CVM_BYTE(arg[3]);
	}
	else if(opcode == IL_OP_LDC_R4)
	{
		CVM_BYTE(arg[0]);
		CVM_BYTE(arg[1]);
		CVM_BYTE(arg[2]);
		CVM_BYTE(arg[3]);
		CVM_ADJUST(CVM_WORDS_PER_NATIVE_FLOAT - 1);
	}
	else if(opcode == IL_OP_LDC_I8)
	{
		CVM_BYTE(arg[0]);
		CVM_BYTE(arg[1]);
		CVM_BYTE(arg[2]);
		CVM_BYTE(arg[3]);
		CVM_BYTE(arg[4]);
		CVM_BYTE(arg[5]);
		CVM_BYTE(arg[6]);
		CVM_BYTE(arg[7]);
		CVM_ADJUST(CVM_WORDS_PER_LONG - 1);
	}
	else if(opcode == IL_OP_LDC_R8)
	{
		CVM_BYTE(arg[0]);
		CVM_BYTE(arg[1]);
		CVM_BYTE(arg[2]);
		CVM_BYTE(arg[3]);
		CVM_BYTE(arg[4]);
		CVM_BYTE(arg[5]);
		CVM_BYTE(arg[6]);
		CVM_BYTE(arg[7]);
		CVM_ADJUST(CVM_WORDS_PER_NATIVE_FLOAT - 1);
	}
	else if(opcode == IL_OP_LDSTR)
	{
		/* TODO */
	}
}

#endif	/* IL_CVMC_CODE */
