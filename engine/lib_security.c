/*
 * lib_security.c - Internalcall methods for security-related classes.
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

#include "engine.h"
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * public static ClrPermissions GetPermissionsFrom(int skipFrames);
 */
ILObject *_IL_ClrSecurity_GetPermissionsFrom(ILExecThread *_thread,
											 ILInt32 skipFrames)
{
	ILCallFrame *frame = _ILGetCallFrame(_thread, skipFrames);
	if(frame)
	{
		while(frame != 0)
		{
			if(frame->permissions)
			{
				return (ILObject *)(frame->permissions);
			}
			frame = _ILGetNextCallFrame(_thread, frame);
		}
		return 0;
	}
	else
	{
		return 0;
	}
}

/*
 * public static ClrPermissions GetPermissions(int skipFrames);
 */
ILObject *_IL_ClrSecurity_GetPermissions(ILExecThread *_thread,
										 ILInt32 skipFrames)
{
	ILCallFrame *frame = _ILGetCallFrame(_thread, skipFrames);
	if(frame)
	{
		return (ILObject *)(frame->permissions);
	}
	else
	{
		return 0;
	}
}

/*
 * public static void SetPermissions(ClrPermissions perm, int skipFrames);
 */
void _IL_ClrSecurity_SetPermissions(ILExecThread *_thread,
									ILObject *perm, ILInt32 skipFrames)
{
	ILCallFrame *frame = _ILGetCallFrame(_thread, skipFrames);
	if(frame)
	{
		frame->permissions = (void *)perm;
	}
}

/*
 * public bool CanUseFileHandle(IntPtr handle);
 */
ILBool _IL_RuntimeSecurityManager_CanUseFileHandle(ILExecThread *_thread,
												   ILObject *_this,
												   ILNativeInt handle)
{
	/* TODO */
	return 0;
}

/*
 * public bool CanOpenFile(String path, FileMode mode,
 *						   FileAccess access, FileShare share);
 */
ILBool _IL_RuntimeSecurityManager_CanOpenFile(ILExecThread *_thread,
											  ILObject *_this,
											  ILString * path,
											  ILInt32 mode, ILInt32 access,
											  ILInt32 share)
{
	/* TODO */
	return 1;
}

/*
 * private static ISecurityManager GetSecurityManager();
 */
ILObject *_IL_Security_GetSecurityManager(ILExecThread *_thread)
{
	ILObject *mgr = _thread->securityManager;
	if(!mgr)
	{
		mgr = ILExecThreadNew(_thread, "Platform.RuntimeSecurityManager",
							  "(T)V");
		_thread->securityManager = mgr;
	}
	return mgr;
}

/*
 * private static void SetSecurityManager(ISecurityManager mgr);
 */
void _IL_Security_SetSecurityManager(ILExecThread *_thread, ILObject *mgr)
{
	_thread->securityManager = mgr;
}

#ifdef	__cplusplus
};
#endif
