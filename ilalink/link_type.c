/*
 * link_type.c - Convert a type into a final image type.
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

#include "linker.h"
#include "../image/program.h"
#include "il_dumpasm.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Result codes for "ConvertClassRef".
 */
#define	CONVERT_REF_FAILED		0
#define	CONVERT_REF_MEMORY		1
#define	CONVERT_REF_LIBRARY		2
#define	CONVERT_REF_LOCAL		3

/*
 * Convert a class reference from a foreign image.
 */
static int ConvertClassRef(ILLinker *linker, ILClass *classInfo,
						   ILLibraryFind *find, ILClass **resultInfo)
{
	ILClass *parent;
	ILClass *newClass;
	ILProgramItem *scope;
	ILAssembly *assem;
	ILLibrary *library;
	int result;

	/* Is the class nested? */
	parent = ILClassGetNestedParent(classInfo);
	if(parent)
	{
		/* Convert the nested parent, which determines if the top-level
		   class is in a library or should be locally defined */
		result = ConvertClassRef(linker, parent, find, resultInfo);
		if(result == CONVERT_REF_FAILED || result == CONVERT_REF_MEMORY)
		{
			return result;
		}
		else if(result == CONVERT_REF_LIBRARY)
		{
			/* We have a library find context as the parent */
			if(!_ILLinkerFindClass(find, classInfo->name, classInfo->namespace))
			{
				return CONVERT_REF_FAILED;
			}
			return CONVERT_REF_LIBRARY;
		}
		else
		{
			/* We have an actual TypeDef or TypeRef as the parent */
			scope = ILToProgramItem(*resultInfo);
			newClass = ILClassLookup(scope, classInfo->name,
									 classInfo->namespace);
			if(newClass)
			{
				*resultInfo = newClass;
				return CONVERT_REF_LOCAL;
			}
			newClass = ILClassCreateRef(scope, 0, classInfo->name,
										classInfo->namespace);
			if(!newClass)
			{
				_ILLinkerOutOfMemory(linker);
				return CONVERT_REF_MEMORY;
			}
			*resultInfo = newClass;
			return CONVERT_REF_LOCAL;
		}
	}
	else if(ILClassIsRef(classInfo))
	{
		/* Converting a top-level class reference to outside the image */
		scope = classInfo->scope;
		assem = ILProgramItemToAssembly(scope);
		if(assem && ILAssemblyIsRef(assem) &&
		   (library = _ILLinkerFindLibrary(linker, assem->name)) != 0)
		{
			/* Resolved to a specific library */
			_ILLinkerFindInit(find, linker, library);
			if(_ILLinkerFindClass(find, classInfo->name, classInfo->namespace))
			{
				return CONVERT_REF_LIBRARY;
			}
			else
			{
				return CONVERT_REF_FAILED;
			}
		}
		_ILLinkerFindInit(find, linker, 0);
		if(_ILLinkerFindClass(find, classInfo->name, classInfo->namespace))
		{
			/* Resolved to one of the libraries */
			return CONVERT_REF_LIBRARY;
		}
	}

	/* If we get here, then we assume that the global class will be
	   in the final image, and so we create a reference to it.  When
	   the link completes, we will scan for dangling TypeRef's */
	scope = ILClassGlobalScope(linker->image);
	newClass = ILClassLookup(scope, classInfo->name, classInfo->namespace);
	if(newClass)
	{
		*resultInfo = newClass;
		return CONVERT_REF_LOCAL;
	}
	newClass = ILClassCreateRef(scope, 0, classInfo->name,
								classInfo->namespace);
	if(!newClass)
	{
		_ILLinkerOutOfMemory(linker);
		return CONVERT_REF_MEMORY;
	}
	*resultInfo = newClass;
	return CONVERT_REF_LOCAL;
}

ILClass *_ILLinkerConvertClassRef(ILLinker *linker, ILClass *classInfo)
{
	ILLibraryFind find;
	ILClass *newClass;
	int result = ConvertClassRef(linker, classInfo, &find, &newClass);
	if(result == CONVERT_REF_FAILED)
	{
		/* Report that we couldn't resolve the class */
		ILDumpClassName(stderr, ILClassToImage(classInfo), classInfo, 0);
		fputs(": unresolved type reference\n", stderr);
		linker->error = 1;
		return 0;
	}
	else if(result == CONVERT_REF_MEMORY)
	{
		/* We ran out of memory while converting the reference */
		return 0;
	}
	else if(result == CONVERT_REF_LIBRARY)
	{
		/* Convert the library reference into a TypeRef */
		return _ILLinkerMakeTypeRef(&find, linker->image);
	}
	else
	{
		/* Return the local or dangling reference */
		return newClass;
	}
}

ILType *_ILLinkerConvertType(ILLinker *linker, ILType *type)
{
	ILClass *classInfo;
	ILType *newType;
	unsigned long param;
	unsigned long numLocals;
	ILType *paramType;

	if(ILType_IsPrimitive(type))
	{
		/* Primitive types always map as themselves */
		return type;
	}
	else if(ILType_IsClass(type))
	{
		/* Convert the class reference */
		classInfo = _ILLinkerConvertClassRef(linker, ILType_ToClass(type));
		if(classInfo)
		{
			return ILType_FromClass(classInfo);
		}
	}
	else if(ILType_IsValueType(type))
	{
		/* Convert the value type reference */
		classInfo = _ILLinkerConvertClassRef
						(linker, ILType_ToValueType(type));
		if(classInfo)
		{
			return ILType_FromValueType(classInfo);
		}
	}
	else if(type != 0 && ILType_IsComplex(type))
	{
		/* Convert a complex type */
		newType = ILMemPoolCalloc(&(linker->context->typePool), ILType);
		if(!newType)
		{
			_ILLinkerOutOfMemory(linker);
			return ILType_Invalid;
		}
		*newType = *type;
		switch(type->kind & 0xFF)
		{
			case IL_TYPE_COMPLEX_BYREF:
			case IL_TYPE_COMPLEX_PTR:
			case IL_TYPE_COMPLEX_PINNED:
			{
				if((newType->un.refType =
					_ILLinkerConvertType(linker, type->un.refType)) != 0)
				{
					return newType;
				}
			}
			break;

			case IL_TYPE_COMPLEX_ARRAY:
			case IL_TYPE_COMPLEX_ARRAY_CONTINUE:
			{
				if((newType->un.array.elemType =
					_ILLinkerConvertType(linker, type->un.array.elemType)) != 0)
				{
					return newType;
				}
			}
			break;

			case IL_TYPE_COMPLEX_CMOD_REQD:
			case IL_TYPE_COMPLEX_CMOD_OPT:
			{
				newType->un.modifier.info = _ILLinkerConvertClassRef
								(linker, type->un.modifier.info);
				newType->un.modifier.type = _ILLinkerConvertType
								(linker, type->un.modifier.type);
				if(newType->un.modifier.info != 0&&
				   newType->un.modifier.type != 0)
				{
					return newType;
				}
			}
			break;

			case IL_TYPE_COMPLEX_SENTINEL:
			{
				return newType;
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_LOCALS:
			{
				newType->num = 0;
				numLocals = ILTypeNumLocals(type);
				for(param = 0; param < numLocals; ++param)
				{
					paramType = _ILLinkerConvertType
						(linker, ILTypeGetLocal(type, param));
					if(!paramType)
					{
						return ILType_Invalid;
					}
					if(!ILTypeAddLocal(linker->context, newType, paramType))
					{
						_ILLinkerOutOfMemory(linker);
						return ILType_Invalid;
					}
				}
				return newType;
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_PROPERTY:
			case IL_TYPE_COMPLEX_METHOD:
			case IL_TYPE_COMPLEX_METHOD | IL_TYPE_COMPLEX_METHOD_SENTINEL:
			{
				newType->num = 0;
				newType->un.method.retType =
					_ILLinkerConvertType(linker, type->un.method.retType);
				if(!(newType->un.method.retType))
				{
					return ILType_Invalid;
				}
				for(param = 1; param <= (ILUInt32)(type->num); ++param)
				{
					paramType = _ILLinkerConvertType
						(linker, ILTypeGetParam(type, param));
					if(!paramType)
					{
						return ILType_Invalid;
					}
					if(!ILTypeAddParam(linker->context, newType, paramType))
					{
						_ILLinkerOutOfMemory(linker);
						return ILType_Invalid;
					}
				}
				return newType;
			}
			/* Not reached */
		}
	}
	return ILType_Invalid;
}

#ifdef	__cplusplus
};
#endif
