/*
 * lib_emit.c - Internalcall methods for the "Reflection.Emit" classes.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

#include "engine_private.h"
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * private void ClrSetEntryPoint(IntPtr nativeMethod, PEFileKinds fileKind);
 */
void _IL_AssemblyBuilder_ClrSetEntryPoint(ILExecThread *thread,
										  ILObject *_this,
										  ILNativeInt clrMethod,
										  ILInt32 fileKind)
{
	/* TODO */
}

/*
 * private IntPtr ClrModuleCreate(IntPtr assembly, String name);
 */
ILNativeInt _IL_ModuleBuilder_ClrModuleCreate(ILExecThread *_thread,
											  ILNativeInt assembly,
											  ILString *name)
{
	/* TODO */
	return 0;
}

#ifdef	__cplusplus
};
#endif
