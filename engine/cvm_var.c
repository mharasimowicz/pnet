/*
 * cvm_var.c - Opcodes for accessing variables.
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

#if defined(IL_CVM_GLOBALS)

/* No globals required */

#elif defined(IL_CVM_LOCALS)

ILUInt32 tempNum;

#elif defined(IL_CVM_MAIN)

#define	COP_LOAD_N(n)	\
case COP_ILOAD_##n: \
{ \
	stacktop[0].intValue = frame[(n)].intValue; \
	MODIFY_PC_AND_STACK(1, 1); \
} \
break; \
case COP_PLOAD_##n: \
{ \
	stacktop[0].ptrValue = frame[(n)].ptrValue; \
	MODIFY_PC_AND_STACK(1, 1); \
} \
break

/**
 * <opcode name="iload_&lt;n&gt;">
 *   <operation>Load <code>int32</code> variable <i>n</i>
 *              onto the stack</operation>
 *
 *   <format>iload_&lt;n&gt;</format>
 *
 *   <form name="iload_0" code="COP_ILOAD_0"/>
 *   <form name="iload_1" code="COP_ILOAD_1"/>
 *   <form name="iload_2" code="COP_ILOAD_2"/>
 *   <form name="iload_3" code="COP_ILOAD_3"/>
 *
 *   <before>...</before>
 *   <after>..., value</after>
 *
 *   <description>Load the <code>int32</code> variable from position
 *   <i>n</i> in the local variable frame and push its <i>value</i>
 *   onto the stack.</description>
 *
 *   <notes>These instructions can also be used to load variables
 *   of type <code>uint32</code> onto the stack.</notes>
 * </opcode>
 */
/**
 * <opcode name="pload_&lt;n&gt;">
 *   <operation>Load <code>ptr</code> variable <i>n</i>
 *              onto the stack</operation>
 *
 *   <format>pload_&lt;n&gt;</format>
 *
 *   <form name="pload_0" code="COP_PLOAD_0"/>
 *   <form name="pload_1" code="COP_PLOAD_1"/>
 *   <form name="pload_2" code="COP_PLOAD_2"/>
 *   <form name="pload_3" code="COP_PLOAD_3"/>
 *
 *   <before>...</before>
 *   <after>..., value</after>
 *
 *   <description>Load the <code>ptr</code> variable from position
 *   <i>n</i> in the local variable frame and push its <i>value</i>
 *   onto the stack.</description>
 *
 *   <notes>These instructions must not be confused with the
 *   <i>iload_&lt;n&gt;</i> instructions.  Values of type
 *   <code>int32</code> and <code>ptr</code> do not necessarily
 *   occupy the same amount of space in a stack word on
 *   all platforms.</notes>
 * </opcode>
 */
/* Load integer or pointer values from the frame onto the stack */
COP_LOAD_N(0);
COP_LOAD_N(1);
COP_LOAD_N(2);
COP_LOAD_N(3);

/**
 * <opcode name="iload">
 *   <operation>Load <code>int32</code> variable
 *              onto the stack</operation>
 *
 *   <format>iload<fsep/>N[1]</format>
 *   <format>wide<fsep/>iload<fsep/>N[4]</format>
 *
 *   <form name="iload" code="COP_ILOAD"/>
 *
 *   <before>...</before>
 *   <after>..., value</after>
 *
 *   <description>Load the <code>int32</code> variable from position
 *   <i>N</i> in the local variable frame and push its <i>value</i>
 *   onto the stack.</description>
 *
 *   <notes>This instruction can also be used to load variables
 *   of type <code>uint32</code> onto the stack.</notes>
 * </opcode>
 */
case COP_ILOAD:
{
	/* Load an integer value from the frame */
	stacktop[0].intValue = frame[pc[1]].intValue;
	MODIFY_PC_AND_STACK(2, 1);
}
break;

/**
 * <opcode name="pload">
 *   <operation>Load <code>ptr</code> variable
 *              onto the stack</operation>
 *
 *   <format>pload<fsep/>N[1]</format>
 *   <format>wide<fsep/>pload<fsep/>N[4]</format>
 *
 *   <form name="pload" code="COP_PLOAD"/>
 *
 *   <before>...</before>
 *   <after>..., value</after>
 *
 *   <description>Load the <code>ptr</code> variable from position
 *   <i>N</i> in the local variable frame and push its <i>value</i>
 *   onto the stack.</description>
 *
 *   <notes>This instruction must not be confused with <i>iload</i>.
 *   Values of type <code>int32</code> and <code>ptr</code> do not
 *   necessarily occupy the same amount of space in a stack word on
 *   all platforms.</notes>
 * </opcode>
 */
case COP_PLOAD:
{
	/* Load a pointer value from the frame */
	stacktop[0].ptrValue = frame[pc[1]].ptrValue;
	MODIFY_PC_AND_STACK(2, 1);
}
break;

#define	COP_STORE_N(n)	\
case COP_ISTORE_##n: \
{ \
	frame[(n)].intValue = stacktop[-1].intValue; \
	MODIFY_PC_AND_STACK(1, -1); \
} \
break; \
case COP_PSTORE_##n: \
{ \
	frame[(n)].ptrValue = stacktop[-1].ptrValue; \
	MODIFY_PC_AND_STACK(1, -1); \
} \
break

/**
 * <opcode name="istore_&lt;n&gt;">
 *   <operation>Store the top of stack into <code>int32</code>
 *              variable <i>n</i></operation>
 *
 *   <format>istore_&lt;n&gt;</format>
 *
 *   <form name="istore_0" code="COP_ISTORE_0"/>
 *   <form name="istore_1" code="COP_ISTORE_1"/>
 *   <form name="istore_2" code="COP_ISTORE_2"/>
 *   <form name="istore_3" code="COP_ISTORE_3"/>
 *
 *   <before>..., value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>value</i> from the stack as type <code>int32</code>
 *   and store it at position <i>n</i> in the local variable frame.
 *   </description>
 *
 *   <notes>These instructions can also be used to store to variables
 *   of type <code>uint32</code>.</notes>
 * </opcode>
 */
/**
 * <opcode name="pstore_&lt;n&gt;">
 *   <operation>Store the top of stack into <code>ptr</code>
 *              variable <i>n</i></operation>
 *
 *   <format>pstore_&lt;n&gt;</format>
 *
 *   <form name="pstore_0" code="COP_PSTORE_0"/>
 *   <form name="pstore_1" code="COP_PSTORE_1"/>
 *   <form name="pstore_2" code="COP_PSTORE_2"/>
 *   <form name="pstore_3" code="COP_PSTORE_3"/>
 *
 *   <before>..., value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>value</i> from the stack as type <code>ptr</code>
 *   and store it at position <i>n</i> in the local variable frame.
 *   </description>
 *
 *   <notes>These instructions must not be confused with the
 *   <i>istore_&lt;n&gt;</i> instructions.  Values of type
 *   <code>int32</code> and <code>ptr</code> do not necessarily
 *   occupy the same amount of space in a stack word on
 *   all platforms.</notes>
 * </opcode>
 */
/* Store integer or pointer values from the stack to the frame */
COP_STORE_N(0);
COP_STORE_N(1);
COP_STORE_N(2);
COP_STORE_N(3);

/**
 * <opcode name="istore">
 *   <operation>Store the top of stack into <code>int32</code>
 *              variable</operation>
 *
 *   <format>istore<fsep/>N[1]</format>
 *   <format>wide<fsep/>istore<fsep/>N[4]</format>
 *
 *   <form name="istore" code="COP_ISTORE"/>
 *
 *   <before>..., value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>value</i> from the stack as type <code>int32</code>
 *   and store it at position <i>N</i> in the local variable frame.
 *   </description>
 *
 *   <notes>This instruction can also be used to store to variables
 *   of type <code>uint32</code>.</notes>
 * </opcode>
 */
case COP_ISTORE:
{
	/* Store an integer value to the frame */
	frame[pc[1]].intValue = stacktop[-1].intValue;
	MODIFY_PC_AND_STACK(2, -1);
}
break;

/**
 * <opcode name="pstore">
 *   <operation>Store the top of stack into <code>ptr</code>
 *              variable</operation>
 *
 *   <format>pstore<fsep/>N[1]</format>
 *   <format>wide<fsep/>pstore<fsep/>N[4]</format>
 *
 *   <form name="pstore" code="COP_PSTORE"/>
 *
 *   <before>..., value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>value</i> from the stack as type <code>ptr</code>
 *   and store it at position <i>N</i> in the local variable frame.
 *   </description>
 *
 *   <notes>This instructions must not be confused with <i>istore</i>.
 *   Values of type <code>int32</code> and <code>ptr</code> do not
 *   necessarily occupy the same amount of space in a stack word on
 *   all platforms.</notes>
 * </opcode>
 */
case COP_PSTORE:
{
	/* Store a pointer value to the frame */
	frame[pc[1]].ptrValue = stacktop[-1].ptrValue;
	MODIFY_PC_AND_STACK(2, -1);
}
break;

/**
 * <opcode name="mload">
 *   <operation>Load multiple stack words from a variable
 *              onto the stack</operation>
 *
 *   <format>mload<fsep/>N[1]<fsep/>M[1]</format>
 *   <format>wide<fsep/>mload<fsep/>N[4]<fsep/>M[4]</format>
 *
 *   <form name="mload" code="COP_MLOAD"/>
 *
 *   <before>...</before>
 *   <after>..., value1, ..., valueM</after>
 *
 *   <description>Load the <i>M</i> stack words from position
 *   <i>N</i> in the local variable frame and push them
 *   onto the stack.</description>
 * </opcode>
 */
case COP_MLOAD:
{
	/* Load a value consisting of multiple stack words */
	IL_MEMCPY(stacktop, &(frame[pc[1]]), sizeof(CVMWord) * (unsigned)(pc[2]));
	stacktop += (unsigned)(pc[2]);
	pc += 3;
}
break;

/**
 * <opcode name="mstore">
 *   <operation>Store multiple stack words from the stack
 *              to a variable</operation>
 *
 *   <format>mstore<fsep/>N[1]<fsep/>M[1]</format>
 *   <format>wide<fsep/>mstore<fsep/>N[4]<fsep/>M[4]</format>
 *
 *   <form name="mload" code="COP_MLOAD"/>
 *
 *   <before>..., value1, ..., valueM</before>
 *   <after>...</after>
 *
 *   <description>Pop the <i>M</i> stack words from the top of
 *   the stack and store them at position <i>N</i> in the local
 *   variable frame.</description>
 * </opcode>
 */
case COP_MSTORE:
{
	/* Store a value consisting of multiple stack words */
	stacktop -= (unsigned)(pc[2]);
	IL_MEMCPY(&(frame[pc[1]]), stacktop, sizeof(CVMWord) * (unsigned)(pc[2]));
	pc += 3;
}
break;

/**
 * <opcode name="waddr">
 *   <operation>Load the address of a variable onto the stack</operation>
 *
 *   <format>waddr<fsep/>N[1]</format>
 *   <format>wide<fsep/>waddr<fsep/>N[4]</format>
 *
 *   <form name="waddr" code="COP_WADDR"/>
 *
 *   <before>...</before>
 *   <after>..., pointer</after>
 *
 *   <description>Set <i>pointer</i> to the address of the word at
 *   position <i>N</i> in the local variable frame.  Push <i>pointer</i>
 *   onto the stack as type <code>ptr</code>.</description>
 * </opcode>
 */
case COP_WADDR:
{
	/* Get the address of the value starting at a frame word */
	stacktop[0].ptrValue = (void *)(&(frame[pc[1]]));
	MODIFY_PC_AND_STACK(2, 1);
}
break;

/**
 * <opcode name="maddr">
 *   <operation>Load the address of a stack word onto the stack</operation>
 *
 *   <format>maddr<fsep/>N[1]</format>
 *   <format>wide<fsep/>maddr<fsep/>N[4]</format>
 *
 *   <form name="maddr" code="COP_MADDR"/>
 *
 *   <before>...</before>
 *   <after>..., pointer</after>
 *
 *   <description>Set <i>pointer</i> to the address of the word at
 *   <i>N</i> positions down the stack.  Push <i>pointer</i>
 *   onto the stack as type <code>ptr</code>.  <i>N == 1</i> indicates
 *   the address of the top-most stack word prior to the operation.
 *   </description>
 *
 *   <notes>This instruction is typically used to get the address of
 *   a managed value on the stack, so that the value can be manipulated
 *   with pointer operations.</notes>
 * </opcode>
 */
case COP_MADDR:
{
	/* Get the address of a managed value N words down the stack */
	stacktop[0].ptrValue = (void *)(stacktop - ((int)(pc[1])));
	MODIFY_PC_AND_STACK(2, 1);
}
break;

/**
 * <opcode name="bfixup">
 *   <operation>Fix up <code>int8</code> variable</operation>
 *
 *   <format>bfixup<fsep/>N[1]</format>
 *   <format>wide<fsep/>bfixup<fsep/>N[4]</format>
 *
 *   <form name="bfixup" code="COP_BFIXUP"/>
 *
 *   <description>Retrieve the contents of position <i>N</i> in the
 *   local variable frame, truncate the value to 8 bits and write it
 *   back to the same variable.  The destination is aligned at the
 *   start of the stack word that contains the variable.</description>
 *
 *   <notes>This instruction is used to align <code>int8</code> and
 *   <code>uint8</code> values that were passed as arguments to the
 *   current method.<p/>
 *
 *   The result is guaranteed to be aligned on the start of a stack
 *   word so that <i>waddr M</i> will push the correct address of
 *   the byte.<p/>
 *
 *   This instruction is not normally required on little-endian platforms,
 *   but it is definitely required on big-endian platforms.<p/>
 *
 *   The contents of an <code>int8</code> argument can be fetched
 *   using <i>waddr N, bread</i> once <i>bfixup</i> has been used to
 *   align its contents.</notes>
 * </opcode>
 */
case COP_BFIXUP:
{
	/* Perform a byte fixup on a frame offset that corresponds
	   to an argument.  Byte arguments are passed from the caller
	   as int parameters, but inside the method we need to access
	   them by an address that is always aligned on the start
	   of a stack word.  This operation fixes address mismatches
	   on CPU's that aren't little-endian */
	*((ILInt8 *)(&(frame[pc[1]]))) = (ILInt8)(frame[pc[1]].intValue);
	MODIFY_PC_AND_STACK(2, 0);
}
break;

/**
 * <opcode name="sfixup">
 *   <operation>Fix up <code>int16</code> variable</operation>
 *
 *   <format>sfixup<fsep/>N[1]</format>
 *   <format>wide<fsep/>sfixup<fsep/>N[4]</format>
 *
 *   <form name="sfixup" code="COP_SFIXUP"/>
 *
 *   <description>Retrieve the contents of position <i>N</i> in the
 *   local variable frame, truncate the value to 16 bits and write it
 *   back to the same variable.  The destination is aligned at the
 *   start of the stack word that contains the variable.</description>
 *
 *   <notes>This instruction is used to align <code>int16</code> and
 *   <code>uint16</code> values that were passed as arguments to the
 *   current method.<p/>
 *
 *   The result is guaranteed to be aligned on the start of a stack
 *   word so that <i>waddr M</i> will push the correct address of
 *   the 16 bit value.<p/>
 *
 *   This instruction is not normally required on little-endian platforms,
 *   but it is definitely required on big-endian platforms.<p/>
 *
 *   The contents of an <code>int16</code> argument can be fetched
 *   using <i>waddr N, sread</i> once <i>sfixup</i> has been used to
 *   align its contents.</notes>
 * </opcode>
 */
case COP_SFIXUP:
{
	/* Perform a short fixup on a frame offset that corresponds
	   to an argument.  See above for the rationale */
	*((ILInt16 *)(&(frame[pc[1]]))) = (ILInt16)(frame[pc[1]].intValue);
	MODIFY_PC_AND_STACK(2, 0);
}
break;

/**
 * <opcode name="ffixup">
 *   <operation>Fix up <code>float32</code> variable</operation>
 *
 *   <format>ffixup<fsep/>N[1]</format>
 *   <format>wide<fsep/>ffixup<fsep/>N[4]</format>
 *
 *   <form name="ffixup" code="COP_FFIXUP"/>
 *
 *   <description>Retrieve the contents of position <i>N</i> in the
 *   local variable frame as type <code>native float</code>, truncate
 *   the value to <code>float32</code> and write it back to the same
 *   variable.  The destination is aligned at the start of the stack
 *   word that contains the variable.</description>
 *
 *   <notes>This instruction is used to convert <code>native float</code>
 *   values that were passed as arguments to the current method into
 *   the <code>float32</code> for internal local variable access.<p/>
 *
 *   The result is guaranteed to be aligned on the start of a stack
 *   word so that <i>waddr M</i> will push the correct address of
 *   the <code>float32</code> value.<p/>
 *
 *   The contents of a <code>float32</code> argument can be fetched
 *   using <i>waddr N, fread</i> once <i>ffixup</i> has been used to
 *   convert its contents.</notes>
 * </opcode>
 */
case COP_FFIXUP:
{
	/* Perform a float fixup on a frame offset that corresponds
	   to an argument.  Float arguments are passed from the caller
	   as "native float" values, but inside the method we need to
	   access them by an address that is a pointer to "float" */
	*((ILFloat *)(&(frame[pc[1]]))) = (ILFloat)ReadFloat(&(frame[pc[1]]));
	MODIFY_PC_AND_STACK(2, 0);
}
break;

/**
 * <opcode name="dfixup">
 *   <operation>Fix up <code>float64</code> variable</operation>
 *
 *   <format>dfixup<fsep/>N[1]</format>
 *   <format>wide<fsep/>dfixup<fsep/>N[4]</format>
 *
 *   <form name="dfixup" code="COP_DFIXUP"/>
 *
 *   <description>Retrieve the contents of position <i>N</i> in the
 *   local variable frame as type <code>native float</code>, truncate
 *   the value to <code>float64</code> and write it back to the same
 *   variable.  The destination is aligned at the start of the stack
 *   word that contains the variable.</description>
 *
 *   <notes>This instruction is used to convert <code>native float</code>
 *   values that were passed as arguments to the current method into
 *   the <code>float64</code> for internal local variable access.<p/>
 *
 *   The result is guaranteed to be aligned on the start of a stack
 *   word so that <i>waddr M</i> will push the correct address of
 *   the <code>float64</code> value.<p/>
 *
 *   The contents of a <code>float64</code> argument can be fetched
 *   using <i>waddr N, dread</i> once <i>dfixup</i> has been used to
 *   convert its contents.</notes>
 * </opcode>
 */
case COP_DFIXUP:
{
	/* Perform a double fixup on a frame offset that corresponds
	   to an argument.  Double arguments are passed from the caller
	   as "native float" values, but inside the method we need to
	   access them by an address that is a pointer to "double" */
	WriteDouble(&(frame[pc[1]]), (ILDouble)ReadFloat(&(frame[pc[1]])));
	MODIFY_PC_AND_STACK(2, 0);
}
break;

/**
 * <opcode name="mk_local_1">
 *   <operation>Make one local variable slot</operation>
 *
 *   <format>mk_local_1</format>
 *
 *   <form name="mk_local_1" code="COP_MK_LOCAL_1"/>
 *
 *   <before>...</before>
 *   <after>..., zero</after>
 *
 *   <description>Push a single zeroed word onto the stack.</description>
 *
 *   <notes>This instruction is used to allocate local variable space
 *   at the start of a method.</notes>
 * </opcode>
 */
case COP_MK_LOCAL_1:
{
	/* Make a single local variable slot on the stack */
#ifdef CVM_WORDS_AND_PTRS_SAME_SIZE
	stacktop[0].ptrValue = 0;
#else
	IL_MEMZERO(&(stacktop[0]), sizeof(CVMWord));
#endif
	MODIFY_PC_AND_STACK(1, 1);
}
break;

/**
 * <opcode name="mk_local_2">
 *   <operation>Make two local variable slots</operation>
 *
 *   <format>mk_local_2</format>
 *
 *   <form name="mk_local_2" code="COP_MK_LOCAL_2"/>
 *
 *   <before>...</before>
 *   <after>..., zero1, zero2</after>
 *
 *   <description>Push two zeroed words onto the stack.</description>
 *
 *   <notes>This instruction is used to allocate local variable space
 *   at the start of a method.</notes>
 * </opcode>
 */
case COP_MK_LOCAL_2:
{
	/* Make two local variable slots on the stack */
#ifdef CVM_WORDS_AND_PTRS_SAME_SIZE
	stacktop[0].ptrValue = 0;
	stacktop[1].ptrValue = 0;
#else
	IL_MEMZERO(&(stacktop[0]), sizeof(CVMWord) * 2);
#endif
	MODIFY_PC_AND_STACK(1, 2);
}
break;

/**
 * <opcode name="mk_local_3">
 *   <operation>Make three local variable slots</operation>
 *
 *   <format>mk_local_3</format>
 *
 *   <form name="mk_local_3" code="COP_MK_LOCAL_3"/>
 *
 *   <before>...</before>
 *   <after>..., zero1, zero2, zero3</after>
 *
 *   <description>Push three zeroed words onto the stack.</description>
 *
 *   <notes>This instruction is used to allocate local variable space
 *   at the start of a method.</notes>
 * </opcode>
 */
case COP_MK_LOCAL_3:
{
	/* Make three local variable slots on the stack */
#ifdef CVM_WORDS_AND_PTRS_SAME_SIZE
	stacktop[0].ptrValue = 0;
	stacktop[1].ptrValue = 0;
	stacktop[2].ptrValue = 0;
#else
	IL_MEMZERO(&(stacktop[0]), sizeof(CVMWord) * 3);
#endif
	MODIFY_PC_AND_STACK(1, 3);
}
break;

/**
 * <opcode name="mk_local_n">
 *   <operation>Make <i>N</i> local variable slots</operation>
 *
 *   <format>mk_local_n<fsep/>N[1]</format>
 *   <format>wide<fsep/>mk_local_n<fsep/>N[4]</format>
 *
 *   <form name="mk_local_n" code="COP_MK_LOCAL_N"/>
 *
 *   <before>...</before>
 *   <after>..., zero1, ..., zeroN</after>
 *
 *   <description>Push <i>N</i> zeroed words onto the stack.</description>
 *
 *   <notes>This instruction is used to allocate local variable space
 *   at the start of a method.</notes>
 * </opcode>
 */
case COP_MK_LOCAL_N:
{
	/* Make an arbitrary number of local variable slots */
	IL_MEMZERO(&(stacktop[0]), sizeof(CVMWord) * ((unsigned)(pc[1])));
	stacktop += ((unsigned)(pc[1]));
	pc += 2;
}
break;

#elif defined(IL_CVM_WIDE)

case COP_ILOAD:
{
	/* Wide version of "iload" */
	stacktop[0].intValue = frame[IL_READ_UINT32(pc + 2)].intValue;
	MODIFY_PC_AND_STACK(6, 1);
}
break;

case COP_PLOAD:
{
	/* Wide version of "pload" */
	stacktop[0].ptrValue = frame[IL_READ_UINT32(pc + 2)].ptrValue;
	MODIFY_PC_AND_STACK(6, 1);
}
break;

case COP_ISTORE:
{
	/* Wide version of "istore" */
	frame[IL_READ_UINT32(pc + 2)].intValue = stacktop[-1].intValue;
	MODIFY_PC_AND_STACK(6, -1);
}
break;

case COP_PSTORE:
{
	/* Wide version of "pstore" */
	frame[IL_READ_UINT32(pc + 2)].ptrValue = stacktop[-1].ptrValue;
	MODIFY_PC_AND_STACK(6, -1);
}
break;

case COP_MLOAD:
{
	/* Wide version of "mload" */
	tempNum = IL_READ_UINT32(pc + 6);
	IL_MEMCPY(stacktop, &(frame[IL_READ_UINT32(pc + 2)]),
			  sizeof(CVMWord) * tempNum);
	stacktop += tempNum;
	pc += 10;
}
break;

case COP_MSTORE:
{
	/* Wide version of "mstore" */
	tempNum = IL_READ_UINT32(pc + 6);
	stacktop -= tempNum;
	IL_MEMCPY(&(frame[IL_READ_UINT32(pc + 2)]), stacktop,
			  sizeof(CVMWord) * tempNum);
	pc += 10;
}
break;

case COP_WADDR:
{
	/* Wide version of "waddr" */
	stacktop[0].ptrValue = (void *)(&(frame[IL_READ_UINT32(pc + 2)]));
	MODIFY_PC_AND_STACK(6, 1);
}
break;

case COP_MADDR:
{
	/* Wide version of "maddr" */
	stacktop[0].ptrValue = (void *)(stacktop - IL_READ_UINT32(pc + 2));
	MODIFY_PC_AND_STACK(6, 1);
}
break;

case COP_BFIXUP:
{
	/* Wide version of "bfixup" */
	tempNum = IL_READ_UINT32(pc + 2);
	*((ILInt8 *)(&(frame[tempNum]))) = (ILInt8)(frame[tempNum].intValue);
	MODIFY_PC_AND_STACK(6, 0);
}
break;

case COP_SFIXUP:
{
	/* Wide version of "sfixup" */
	tempNum = IL_READ_UINT32(pc + 2);
	*((ILInt16 *)(&(frame[tempNum]))) = (ILInt16)(frame[tempNum].intValue);
	MODIFY_PC_AND_STACK(6, 0);
}
break;

case COP_FFIXUP:
{
	/* Wide version of "ffixup" */
	tempNum = IL_READ_UINT32(pc + 2);
	*((ILFloat *)(&(frame[tempNum]))) = (ILFloat)ReadFloat(&(frame[tempNum]));
	MODIFY_PC_AND_STACK(6, 0);
}
break;

case COP_DFIXUP:
{
	/* Wide version of "dfixup" */
	tempNum = IL_READ_UINT32(pc + 2);
	WriteDouble(&(frame[tempNum]), (ILDouble)ReadFloat(&(frame[tempNum])));
	MODIFY_PC_AND_STACK(6, 0);
}
break;

case COP_MK_LOCAL_N:
{
	/* Wide version of "mk_local_n" */
	tempNum = IL_READ_UINT32(pc + 2);
	IL_MEMZERO(&(stacktop[0]), sizeof(CVMWord) * tempNum);
	MODIFY_PC_AND_STACK(6, tempNum);
}
break;

#endif /* IL_CVM_WIDE */
