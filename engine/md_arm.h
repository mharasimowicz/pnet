/*
 * md_arm.h - Machine-dependent definitions for ARM.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

#ifndef	_ENGINE_MD_ARM_H
#define	_ENGINE_MD_ARM_H

#include "arm_codegen.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Register numbers in the standard register allocation order.
 * -1 is the list terminator.
 */
#define	MD_REG_0		ARM_R0
#define	MD_REG_1		ARM_R1
#define	MD_REG_2		ARM_R2
#define	MD_REG_3		ARM_R3
#define	MD_REG_4		ARM_R4
#define	MD_REG_5		ARM_R7
#define	MD_REG_6		ARM_R8
#define	MD_REG_7		ARM_R9
#define	MD_REG_8		-1
#define	MD_REG_9		-1
#define	MD_REG_10		-1
#define	MD_REG_11		-1
#define	MD_REG_12		-1
#define	MD_REG_13		-1
#define	MD_REG_14		-1
#define	MD_REG_15		-1

/*
 * Mask that indicates a floating-point register.
 */
#define	MD_FREG_MASK	0x0010

/*
 * Floating point register numbers in the standard allocation order.
 * -1 is the list terminator.  The floating point register numbers
 * must include the MD_FREG_MASK value.
 */
#define	MD_FREG_0		-1
#define	MD_FREG_1		-1
#define	MD_FREG_2		-1
#define	MD_FREG_3		-1
#define	MD_FREG_4		-1
#define	MD_FREG_5		-1
#define	MD_FREG_6		-1
#define	MD_FREG_7		-1
#define	MD_FREG_8		-1
#define	MD_FREG_9		-1
#define	MD_FREG_10		-1
#define	MD_FREG_11		-1
#define	MD_FREG_12		-1
#define	MD_FREG_13		-1
#define	MD_FREG_14		-1
#define	MD_FREG_15		-1

/*
 * Set this to a non-zero value if floating-point registers are organised
 * in a stack (e.g. the x87 FPU).
 */
#define	MD_FP_STACK_SIZE	0

/*
 * The register that contains the CVM program counter.  This may be
 * present in the standard register allocation order.  This can be
 * set to -1 if MD_STATE_ALREADY_IN_REGS is 0.
 */
#define	MD_REG_PC		ARM_R4

/*
 * The register that contains the CVM stacktop pointer.  This must not
 * be present in the standard register allocation order.
 */
#define	MD_REG_STACK	ARM_R5

/*
 * The register that contains the CVM frame pointer.  This must not
 * be present in the standard register allocation order.
 */
#define	MD_REG_FRAME	ARM_R6

/*
 * Set this to 1 if "pc", "stacktop", and "frame" are already in
 * the above registers when unrolled code is entered.  i.e. the
 * CVM interpreter has manual assignments of registers to variables
 * in the file "cvm.c".  If the state is not already in registers,
 * then set this value to zero.
 */
#define	MD_STATE_ALREADY_IN_REGS	1

/*
 * Registers that must be saved on the system stack prior to their use
 * in unrolled code for temporary stack values.
 */
#define	MD_REGS_TO_BE_SAVED	\
			((1 << ARM_R7) | (1 << ARM_R8) | (1 << ARM_R9))

/*
 * Registers with special meanings (pc, stacktop, frame) that must
 * be saved if MD_STATE_ALREADY_IN_REGS is 0.
 */
#define	MD_SPECIAL_REGS_TO_BE_SAVED	0

/*
 * Set this to 1 if the CPU has integer division operations.
 * Set it to zero if integer division is too hard to be performed
 * inline using a simple opcode.
 */
#define	MD_HAS_INT_DIVISION			0

/*
 * Type of the instruction pointer for outputting code.
 */
typedef arm_inst_ptr	md_inst_ptr;

/*
 * Push a word register onto the system stack.
 */
#define	md_push_reg(inst,reg)	arm_push_reg((inst), (reg))

/*
 * Pop a word register from the system stack.
 */
#define	md_pop_reg(inst,reg)	arm_pop_reg((inst), (reg))

/*
 * Discard the contents of a floating-point register.
 */
#define	md_discard_freg(inst,reg)	do { ; } while (0)

/*
 * Load a 32-bit integer constant into a register.  This will sign-extend
 * if the native word size is larger.
 */
#define	md_load_const_32(inst,reg,value)	\
			arm_mov_reg_imm((inst), (reg), (value))

/*
 * Load a native integer constant into a register.
 */
#define	md_load_const_native(inst,reg,value)	\
			arm_mov_reg_imm((inst), (reg), (value))

/*
 * Load a 32-bit word register from an offset from a pointer register.
 * This will sign-extend if the native word size is larger.
 */
#define	md_load_membase_word_32(inst,reg,basereg,offset)	\
			arm_load_membase((inst), (reg), (basereg), (offset))

/*
 * Load a native-sized word register from an offset from a pointer register.
 */
#define	md_load_membase_word_native(inst,reg,basereg,offset)	\
			arm_load_membase((inst), (reg), (basereg), (offset))

/*
 * Load a byte value from an offset from a pointer register.
 */
#define	md_load_membase_byte(inst,reg,basereg,offset)	\
			arm_load_membase_byte((inst), (reg), (basereg), (offset))

/*
 * Load a signed byte value from an offset from a pointer register.
 */
#define	md_load_membase_sbyte(inst,reg,basereg,offset)	\
			arm_load_membase_sbyte((inst), (reg), (basereg), (offset))

/*
 * Load a short value from an offset from a pointer register.
 */
#define	md_load_membase_short(inst,reg,basereg,offset)	\
			arm_load_membase_short((inst), (reg), (basereg), (offset))

/*
 * Load an unsigned short value from an offset from a pointer register.
 */
#define	md_load_membase_ushort(inst,reg,basereg,offset)	\
			arm_load_membase_ushort((inst), (reg), (basereg), (offset))

/*
 * Load a floating-point value from an offset from a pointer register.
 * If the system uses floating-point registers, then the value is
 * loaded into "reg".  Otherwise it is loaded onto the top of the
 * floating-point stack.
 */
#define	md_load_membase_float_32(inst,reg,basereg,offset)	\
			do { ; } while (0)
#define	md_load_membase_float_64(inst,reg,basereg,offset)	\
			do { ; } while (0)
#define	md_load_membase_float_native(inst,reg,basereg,offset)	\
			do { ; } while (0)

/*
 * Store a 32-bit word register to an offset from a pointer register.
 */
#define	md_store_membase_word_32(inst,reg,basereg,offset)	\
			arm_store_membase((inst), (reg), (basereg), (offset))

/*
 * Store a native-sized word register to an offset from a pointer register.
 */
#define	md_store_membase_word_native(inst,reg,basereg,offset)	\
			arm_store_membase((inst), (reg), (basereg), (offset))

/*
 * Store a byte value to an offset from a pointer register.
 */
#define	md_store_membase_byte(inst,reg,basereg,offset)	\
			arm_store_membase_byte((inst), (reg), (basereg), (offset))

/*
 * Store a signed byte value to an offset from a pointer register.
 */
#define	md_store_membase_sbyte(inst,reg,basereg,offset)	\
			arm_store_membase_sbyte((inst), (reg), (basereg), (offset))

/*
 * Store a short value to an offset from a pointer register.
 */
#define	md_store_membase_short(inst,reg,basereg,offset)	\
			arm_store_membase_short((inst), (reg), (basereg), (offset))

/*
 * Store an unsigned short value to an offset from a pointer register.
 */
#define	md_store_membase_ushort(inst,reg,basereg,offset)	\
			arm_store_membase_ushort((inst), (reg), (basereg), (offset))

/*
 * Store a floating-point value to an offset from a pointer register.
 * If the system uses floating-point registers, then the value is
 * stored from "reg".  Otherwise it is stored from the top of the
 * floating-point stack.
 */
#define	md_store_membase_float_32(inst,reg,basereg,offset)	\
			do { ; } while (0)
#define	md_store_membase_float_64(inst,reg,basereg,offset)	\
			do { ; } while (0)
#define	md_store_membase_float_native(inst,reg,basereg,offset)	\
			do { ; } while (0)

/*
 * Add an immediate value to a register.
 */
#define	md_add_reg_imm(inst,reg,imm)	\
			arm_alu_reg_imm((inst), ARM_ADD, (reg), (reg), (imm))

/*
 * Subtract an immediate value from a register.
 */
#define	md_sub_reg_imm(inst,reg,imm)	\
			arm_alu_reg_imm((inst), ARM_SUB, (reg), (reg), (imm))

/*
 * Perform arithmetic and logical operations on 32-bit word registers.
 */
#define	md_add_reg_reg_word_32(inst,reg1,reg2)	\
			arm_alu_reg_reg((inst), ARM_ADD, (reg1), (reg1), (reg2))
#define	md_sub_reg_reg_word_32(inst,reg1,reg2)	\
			arm_alu_reg_reg((inst), ARM_SUB, (reg1), (reg1), (reg2))
#define	md_mul_reg_reg_word_32(inst,reg1,reg2)	\
			arm_mul_reg_reg((inst), (reg1), (reg1), (reg2))
#define	md_div_reg_reg_word_32(inst,reg1,reg2)	\
			do { ; } while (0)
#define	md_udiv_reg_reg_word_32(inst,reg1,reg2)	\
			do { ; } while (0)
#define	md_rem_reg_reg_word_32(inst,reg1,reg2)	\
			do { ; } while (0)
#define	md_urem_reg_reg_word_32(inst,reg1,reg2)	\
			do { ; } while (0)
#define	md_neg_reg_word_32(inst,reg)	\
			arm_alu_reg_imm((inst), ARM_RSB, (reg), (reg), 0)
#define	md_and_reg_reg_word_32(inst,reg1,reg2)	\
			arm_alu_reg_reg((inst), ARM_AND, (reg1), (reg1), (reg2))
#define	md_xor_reg_reg_word_32(inst,reg1,reg2)	\
			arm_alu_reg_reg((inst), ARM_XOR, (reg1), (reg1), (reg2))
#define	md_or_reg_reg_word_32(inst,reg1,reg2)	\
			arm_alu_reg_reg((inst), ARM_ORR, (reg1), (reg1), (reg2))
#define	md_not_reg_word_32(inst,reg)	\
			arm_alu_reg((inst), ARM_MVN, (reg), (reg))
#define	md_shl_reg_reg_word_32(inst,reg1,reg2)	\
			arm_shift_reg_reg((inst), ARM_SHL, (reg1), (reg1), (reg2))
#define	md_shr_reg_reg_word_32(inst,reg1,reg2)	\
			arm_shift_reg_reg((inst), ARM_SAR, (reg1), (reg1), (reg2))
#define	md_ushr_reg_reg_word_32(inst,reg1,reg2)	\
			arm_shift_reg_reg((inst), ARM_SHR, (reg1), (reg1), (reg2))

/*
 * Convert word registers between various types.
 */
#define	md_reg_to_byte(inst,reg)	\
			arm_alu_reg_imm((inst), ARM_AND, (reg), (reg), 0xFF)
#define	md_reg_to_sbyte(inst,reg)	\
			do { \
				arm_shift_reg_imm((inst), ARM_SHL, (reg), (reg), 24); \
				arm_shift_reg_imm((inst), ARM_SAR, (reg), (reg), 24); \
			} while (0)
#define	md_reg_to_short(inst,reg)	\
			do { \
				arm_shift_reg_imm((inst), ARM_SHL, (reg), (reg), 16); \
				arm_shift_reg_imm((inst), ARM_SAR, (reg), (reg), 16); \
			} while (0)
#define	md_reg_to_ushort(inst,reg)	\
			do { \
				arm_shift_reg_imm((inst), ARM_SHL, (reg), (reg), 16); \
				arm_shift_reg_imm((inst), ARM_SHR, (reg), (reg), 16); \
			} while (0)
#define	md_reg_to_word_32(inst,reg)	\
			do { ; } while (0)
#define	md_reg_to_word_native(inst,reg)	\
			do { ; } while (0)

/*
 * Truncate floating point values to 32-bit or 64-bit.
 */
#define	md_reg_to_float_32(inst,reg)	\
			do { ; } while (0)
#define	md_reg_to_float_64(inst,reg)	\
			do { ; } while (0)

/*
 * Swap the top two items on the floating-point stack.
 */
#define	md_freg_swap(inst)		do { ; } while (0)

/*
 * Jump back into the CVM interpreter to execute the instruction
 * at "pc".  If "label" is non-NULL, then it indicates the address
 * of the CVM instruction handler to jump directly to.
 */
#define	md_jump_to_cvm(inst,pc,label)	\
			do { \
				/* TODO */ \
			} while (0)

/*
 * Perform a clear operation at a memory base.
 */
#define	md_clear_membase_start(inst)	\
			do { \
				arm_mov_reg_imm((inst), ARM_WORK, 0); \
			} while (0)
#define	md_clear_membase(inst,reg,offset)	\
			do { \
				arm_store_membase((inst), ARM_WORK, (reg), (offset)); \
			} while (0)

/*
 * Load the effective address of a memory base into a register.
 */
#define	md_lea_membase(inst,reg,basereg,offset)	\
			do { \
				int __value = (int)(offset); \
				arm_mov_reg_reg((inst), (reg), (basereg)); \
				if(__value != 0) \
				{ \
					arm_alu_reg_imm((inst), ARM_ADD, (reg), (reg), __value); \
				} \
			} while (0)

/*
 * Move values between registers.
 */
#define	md_mov_reg_reg(inst,dreg,sreg)	\
			arm_mov_reg_reg((inst), (dreg), (sreg))

/*
 * Helper routine for the complex cases of "arm_mov_reg_imm".
 * TODO: make this static.
 */
arm_inst_ptr _arm_mov_reg_imm(arm_inst_ptr inst, int reg, int value)
{
	/* Handle bytes in various positions */
	if((value & 0x000000FF) == value)
	{
		arm_mov_reg_imm8(inst, reg, value);
		return inst;
	}
	else if((value & 0x0000FF00) == value)
	{
		arm_mov_reg_imm8_rotate(inst, reg, (value >> 8), 12);
		return inst;
	}
	else if((value & 0x00FF0000) == value)
	{
		arm_mov_reg_imm8_rotate(inst, reg, (value >> 16), 8);
		return inst;
	}
	else if((value & 0xFF000000) == value)
	{
		arm_mov_reg_imm8_rotate(inst, reg, ((value >> 24) & 0xFF), 4);
		return inst;
	}

	/* Handle inverted bytes in various positions */
	value = ~value;
	if((value & 0x000000FF) == value)
	{
		arm_mov_reg_imm8(inst, reg, value);
		arm_alu_reg(inst, ARM_MVN, reg, reg);
		return inst;
	}
	else if((value & 0x0000FF00) == value)
	{
		arm_mov_reg_imm8_rotate(inst, reg, (value >> 8), 12);
		arm_alu_reg(inst, ARM_MVN, reg, reg);
		return inst;
	}
	else if((value & 0x00FF0000) == value)
	{
		arm_mov_reg_imm8_rotate(inst, reg, (value >> 16), 8);
		arm_alu_reg(inst, ARM_MVN, reg, reg);
		return inst;
	}
	else if((value & 0xFF000000) == value)
	{
		arm_mov_reg_imm8_rotate(inst, reg, ((value >> 24) & 0xFF), 4);
		arm_alu_reg(inst, ARM_MVN, reg, reg);
		return inst;
	}

	/* Build the value the hard way, byte by byte */
	value = ~value;
	if((value & 0xFF000000) != 0)
	{
		arm_mov_reg_imm8_rotate(inst, reg, ((value >> 24) & 0xFF), 4);
		if((value & 0x00FF0000) != 0)
		{
			arm_alu_reg_imm8_rotate
				(inst, ARM_ADD, reg, reg, ((value >> 16) & 0xFF), 8);
		}
		if((value & 0x0000FF00) != 0)
		{
			arm_alu_reg_imm8_rotate
				(inst, ARM_ADD, reg, reg, ((value >> 8) & 0xFF), 12);
		}
		if((value & 0x000000FF) != 0)
		{
			arm_alu_reg_imm8(inst, ARM_ADD, reg, reg, (value & 0xFF));
		}
	}
	else if((value & 0x00FF0000) != 0)
	{
		arm_mov_reg_imm8_rotate(inst, reg, ((value >> 16) & 0xFF), 8);
		if((value & 0x0000FF00) != 0)
		{
			arm_alu_reg_imm8_rotate
				(inst, ARM_ADD, reg, reg, ((value >> 8) & 0xFF), 12);
		}
		if((value & 0x000000FF) != 0)
		{
			arm_alu_reg_imm8(inst, ARM_ADD, reg, reg, (value & 0xFF));
		}
	}
	else if((value & 0x0000FF00) != 0)
	{
		arm_mov_reg_imm8_rotate(inst, reg, ((value >> 8) & 0xFF), 12);
		if((value & 0x000000FF) != 0)
		{
			arm_alu_reg_imm8(inst, ARM_ADD, reg, reg, (value & 0xFF));
		}
	}
	else
	{
		arm_mov_reg_imm8(inst, reg, (value & 0xFF));
	}
	return inst;
}

/*
 * Helper routine for the complex cases of "arm_alu_reg_imm".
 */
arm_inst_ptr _arm_alu_reg_imm(arm_inst_ptr inst, int opc,
					          int dreg, int sreg, int imm,
					          int saveWork)
{
	int tempreg;
	if(saveWork)
	{
		if(dreg != ARM_R2 && sreg != ARM_R2)
		{
			tempreg = ARM_R2;
		}
		else if(dreg != ARM_R3 && sreg != ARM_R3)
		{
			tempreg = ARM_R3;
		}
		else
		{
			tempreg = ARM_R4;
		}
		arm_push_reg(inst, tempreg);
	}
	else
	{
		tempreg = ARM_WORK;
	}
	_arm_mov_reg_imm(inst, tempreg, imm);
	arm_alu_reg_reg(inst, opc, dreg, sreg, tempreg);
	if(saveWork)
	{
		arm_pop_reg(inst, tempreg);
	}
	return inst;
}

#ifdef	__cplusplus
};
#endif

#endif /* _ENGINE_MD_ARM_H */