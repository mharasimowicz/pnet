/*
 * lib_object.c - Internalcall methods for "System.Object".
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

#include "engine_private.h"
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Get the private data attached to a class.
 */
#define	IL_CLASS_PRIVATE(classInfo)	\
				((ILClassPrivate *)((classInfo)->userData))

ILObject *_IL_Object_GetType(ILExecThread *thread, ILObject *_this)
{
	ILObject *obj;

	/* Does the class already have a "ClrType" instance? */
	if(IL_CLASS_PRIVATE(GetObjectClass(_this))->clrType)
	{
		return IL_CLASS_PRIVATE(GetObjectClass(_this))->clrType;
	}

	/* Create a new "ClrType" instance for the "ILClass" structure */
	obj = _ILGetClrType(thread, GetObjectClass(_this));
	if(!obj)
	{
		return 0;
	}

	/* Return the object to the caller */
	return obj;
}

ILInt32 _IL_Object_GetHashCode(ILExecThread *thread, ILObject *_this)
{
	return (ILInt32)_this;
}

ILBool _IL_Object_Equals(ILExecThread *thread, ILObject *_this, ILObject *obj)
{
	return (_this == obj);
}

ILObject *_IL_Object_MemberwiseClone(ILExecThread *thread, ILObject *_this)
{
	ILObject *obj;

	/* Test for arrays, which must be cloned differently */
	if(_ILIsSArray((System_Array *)_this))
	{
		return _ILCloneSArray(thread, (System_Array *)_this);
	}
#ifdef IL_CONFIG_NON_VECTOR_ARRAYS
	else if(_ILIsMArray((System_Array *)_this))
	{
		return _ILCloneMArray(thread, (System_MArray *)_this);
	}
#endif

	/* Allocate a new object of the same class */
	obj = _ILEngineAllocObject(thread, GetObjectClass(_this));
	if(!obj)
	{
		return 0;
	}

	/* Copy the contents of "this" into the new object */
	if(IL_CLASS_PRIVATE(GetObjectClass(_this))->size != 0)
	{
		ILMemCpy(obj, _this, IL_CLASS_PRIVATE(GetObjectClass(_this))->size);
	}

	/* Return the cloned object to the caller */
	return obj;
}

ILObject *_ILGetClrType(ILExecThread *thread, ILClass *classInfo)
{
	ILObject *obj;

	/* Make sure that the class has been laid out */
	IL_METADATA_WRLOCK(thread);
	if(!_ILLayoutClass(classInfo))
	{
		IL_METADATA_UNLOCK(thread);
		thread->thrownException = _ILSystemException
			(thread, "System.TypeInitializationException");
		return 0;
	}
	IL_METADATA_UNLOCK(thread);

	/* Does the class already have a "ClrType" instance? */
	if(((ILClassPrivate *)(classInfo->userData))->clrType)
	{
		return ((ILClassPrivate *)(classInfo->userData))->clrType;
	}

	/* Create a new "ClrType" instance */
	if(!(thread->process->clrTypeClass))
	{
		thread->thrownException = _ILSystemException
			(thread, "System.TypeInitializationException");
		return 0;
	}
	obj = _ILEngineAllocObject(thread, thread->process->clrTypeClass);
	if(!obj)
	{
		return 0;
	}

	/* Fill in the object with the class information */
	((System_Reflection *)obj)->privateData = classInfo;

	/* Attach the object to the class so that it will be returned
	   for future calls to this function */
	((ILClassPrivate *)(classInfo->userData))->clrType = obj;

	/* Return the object to the caller */
	return obj;
}

ILClass *_ILGetClrClass(ILExecThread *thread, ILObject *type)
{
	if(type)
	{
		/* Make sure that "type" is an instance of "ClrType" */
		if(ILClassInheritsFrom(GetObjectClass(type),
							   thread->process->clrTypeClass))
		{
			return (ILClass *)(((System_Reflection *)type)->privateData);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

#ifdef	__cplusplus
};
#endif
