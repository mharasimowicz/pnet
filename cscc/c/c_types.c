/*
 * c_types.c - Type representation for the C programming language.
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

#include <cscc/c/c_internal.h>

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Forward declarations.
 */
static char *AppendThree(ILGenInfo *info, const char *prefix,
						 char *str, const char *suffix);

ILType *CTypeCreateStructOrUnion(ILGenInfo *info, const char *name,
								 int kind, const char *funcName)
{
	const char *prefix;
	int prefixLen;
	int funcNameLen;
	ILClass *classInfo;
	char *newName;

	/* Determine which prefix to use */
	if(kind == C_STKIND_STRUCT || kind == C_STKIND_STRUCT_NATIVE)
	{
		prefix = "struct ";
		prefixLen = 7;
	}
	else
	{
		prefix = "union ";
		prefixLen = 6;
	}

	/* Determine if we need to qualify the name using a function name */
	if(!funcName || *funcName == '\0')
	{
		funcNameLen = 0;
	}
	else
	{
		funcNameLen = strlen(funcName) + 1;
	}

	/* Create a new name by prepending the prefix and function to the name */
	newName = (char *)ILMalloc(strlen(name) + prefixLen + funcNameLen + 1);
	if(!newName)
	{
		ILGenOutOfMemory(info);
	}
	strcpy(newName, prefix);
	if(funcNameLen > 0)
	{
		strcpy(newName + prefixLen, funcName);
		newName[prefixLen + funcNameLen - 1] = '.';
	}
	strcpy(newName + prefixLen + funcNameLen, name);

	/* Search for a class information block with the name */
	classInfo = ILClassLookup(ILClassGlobalScope(info->image), newName, 0);
	if(classInfo)
	{
		ILFree(newName);
		return ILType_FromValueType(classInfo);
	}

	/* Create a new class reference with the specified name */
	classInfo = ILClassCreateRef(ILClassGlobalScope(info->image), 0,
							     newName, 0);
	if(!classInfo)
	{
		ILGenOutOfMemory(info);
	}

	/* Clean up and exit */
	ILFree(newName);
	return ILType_FromValueType(classInfo);
}

ILType *CTypeCreateEnum(ILGenInfo *info, const char *name,
						const char *funcName)
{
	char *newName;
	ILClass *classInfo;
	int funcNameLen;

	/* Determine if we need to qualify the name using a function name */
	if(!funcName || *funcName == '\0')
	{
		funcNameLen = 0;
	}
	else
	{
		funcNameLen = strlen(funcName) + 1;
	}

	/* Create a new name by prepending "enum " to the name */
	newName = (char *)ILMalloc(strlen(name) + funcNameLen + 6);
	if(!newName)
	{
		ILGenOutOfMemory(info);
	}
	strcpy(newName, "enum ");
	if(funcNameLen > 0)
	{
		strcpy(newName + 5, funcName);
		newName[5 + funcNameLen - 1] = '.';
	}
	strcpy(newName + funcNameLen + 5, name);

	/* Search for a class information block with the name */
	classInfo = ILClassLookup(ILClassGlobalScope(info->image), newName, 0);
	if(classInfo)
	{
		ILFree(newName);
		return ILType_FromValueType(classInfo);
	}

	/* Create a new class with the specified name */
	classInfo = ILClassCreateRef(ILClassGlobalScope(info->image), 0,
							     newName, 0);
	if(!classInfo)
	{
		ILGenOutOfMemory(info);
	}

	/* Clean up and exit */
	ILFree(newName);
	return ILType_FromValueType(classInfo);
}

/*
 * Format the name of an array type.
 */
static char *FormatArrayName(ILGenInfo *info, ILType *elemType,
							 ILUInt32 size, int isOpen)
{
	char *innerName;
	char sizeName[64];
	ILType *type;

	/* Find the innermost element type */
	type = elemType;
	while(CTypeIsArray(type))
	{
		type = CTypeGetElemType(type);
	}

	/* Format the innermost element type and the passed-in array size */
	innerName = CTypeToName(info, type);
	if(isOpen)
	{
		innerName = AppendThree(info, 0, innerName, "[]");
	}
	else
	{
		sprintf(sizeName, "[%lu]", (unsigned long)size);
		innerName = AppendThree(info, 0, innerName, sizeName);
	}

	/* Format the dimension specifiers for the other dimensions */
	type = elemType;
	while(CTypeIsArray(type))
	{
		if(CTypeIsOpenArray(type))
		{
			innerName = AppendThree(info, 0, innerName, "[]");
		}
		else
		{
			sprintf(sizeName, "[%lu]", (unsigned long)(CTypeGetNumElems(type)));
			innerName = AppendThree(info, 0, innerName, sizeName);
		}
		type = CTypeGetElemType(type);
	}

	/* Return the formatted name to the caller */
	return innerName;
}

/*
 * Create an array type, with either a size or an open-ended definition.
 */
static ILType *CreateArray(ILGenInfo *info, ILType *elemType,
						   ILUInt32 size, int isOpen)
{
	char *name;
	ILUInt32 elemSize, align;
	ILUInt32 attrs;
	ILClass *classInfo;
	ILField *field;

	/* Format the name of the array type */
	name = AppendThree(info, "array ",
				FormatArrayName(info, elemType, size, isOpen), 0);

	/* See if we already have a type with this name */
	classInfo = ILClassLookup(ILClassGlobalScope(info->image), name, 0);
	if(classInfo)
	{
		ILFree(name);
		return ILType_FromValueType(classInfo);
	}

	/* Get the size and alignment of the element type */
	elemSize = CTypeSizeAndAlign(elemType, &align);
	if(elemSize == CTYPE_DYNAMIC || elemSize == CTYPE_UNKNOWN)
	{
		return 0;
	}

	/* Validate the array size: it must not overflow the ".size" field
	   within the class's metadata structure */
	if((((ILUInt64)elemSize) * ((ILUInt64)size)) >
			(ILUInt64)(ILInt64)IL_MAX_INT32)
	{
		return 0;
	}

	/* Determine the attributes for the array type */
	if(ILType_IsValueType(elemType) &&
	   !ILClass_IsPublic(ILType_ToValueType(elemType)))
	{
		/* The element type is not exported, so neither should the array.
		   This can happen when creating arrays of anonymous structs */
		attrs = IL_META_TYPEDEF_NOT_PUBLIC;
	}
	else
	{
		/* Export the array type to match the element type */
		attrs = IL_META_TYPEDEF_PUBLIC;
	}
	attrs |= IL_META_TYPEDEF_SEALED |
			 IL_META_TYPEDEF_SERIALIZABLE |
			 IL_META_TYPEDEF_VALUE_TYPE |
			 IL_META_TYPEDEF_EXPLICIT_LAYOUT;

	/* Create the class that corresponds to the array type */
	classInfo = ILType_ToClass(ILFindSystemType(info, "ValueType"));
	classInfo = ILClassCreate(ILClassGlobalScope(info->image), 0,
							  name, 0, classInfo);
	if(!classInfo)
	{
		ILGenOutOfMemory(info);
	}
	ILClassSetAttrs(classInfo, ~((ILUInt32)0), attrs);

	/* Set the explicit size and alignment of the entire array type */
	if(!ILClassLayoutCreate(info->image, 0, classInfo,
						    align, elemSize * size))
	{
		ILGenOutOfMemory(info);
	}

	/* Create the "elem__" field which defines the type */
	if(isOpen)
	{
		/* Zero-sized arrays store the type as a "private static" field */
		field = ILFieldCreate(classInfo, 0, "elem__",
							  IL_META_FIELDDEF_PRIVATE |
							  IL_META_FIELDDEF_STATIC |
							  IL_META_FIELDDEF_SPECIAL_NAME);
	}
	else if(size == 0)
	{
		/* Zero-sized arrays store the type as a "public static" field */
		field = ILFieldCreate(classInfo, 0, "elem__",
							  IL_META_FIELDDEF_PUBLIC |
							  IL_META_FIELDDEF_STATIC |
							  IL_META_FIELDDEF_SPECIAL_NAME);
	}
	else
	{
		/* Other arrays store the type in a regular field */
		field = ILFieldCreate(classInfo, 0, "elem__",
							  IL_META_FIELDDEF_PUBLIC |
							  IL_META_FIELDDEF_SPECIAL_NAME);
		if(!ILFieldLayoutCreate(info->image, 0, field, 0))
		{
			ILGenOutOfMemory(info);
		}
	}
	if(!field)
	{
		ILGenOutOfMemory(info);
	}
	ILMemberSetSignature((ILMember *)field, elemType);

	/* Return the array type to the caller */
	return ILType_FromValueType(classInfo);
}

ILType *CTypeCreateArray(ILGenInfo *info, ILType *elemType, ILUInt32 size)
{
	return CreateArray(info, elemType, size, 0);
}

ILType *CTypeCreateOpenArray(ILGenInfo *info, ILType *elemType)
{
	return CreateArray(info, elemType, 0, 1);
}

ILType *CTypeCreatePointer(ILGenInfo *info, ILType *refType)
{
	ILType *type = ILTypeCreateRef(info->context, IL_TYPE_COMPLEX_PTR, refType);
	if(!type)
	{
		ILGenOutOfMemory(info);
	}
	return type;
}

ILType *CTypeCreateVaList(ILGenInfo *info)
{
	/* The base class library's "System.ArgIterator" class is the
	   underlying representation for "__builtin_va_list" */
	return ILFindSystemType(info, "ArgIterator");
}

ILType *CTypeCreateVoidPtr(ILGenInfo *info)
{
	static ILType *voidPtr = 0;
	if(!voidPtr)
	{
		voidPtr = CTypeCreatePointer(info, ILType_Void);
	}
	return voidPtr;
}

ILType *CTypeCreateCharPtr(ILGenInfo *info)
{
	static ILType *charPtr = 0;
	if(!charPtr)
	{
		charPtr = CTypeCreatePointer(info, ILType_Int8);
	}
	return charPtr;
}

ILType *CTypeAddConst(ILGenInfo *info, ILType *type)
{
	ILClass *classInfo;
	ILType *modifiers;
	classInfo = ILType_ToClass(ILFindNonSystemType
			(info, "IsConst", "OpenSystem.C"));
	if(ILTypeHasModifier(type, classInfo))
	{
		/* The type already has the specified modifier, so don't add again */
		return type;
	}
	else
	{
		/* Add a modifier prefix to the type */
		modifiers = ILTypeCreateModifier(info->context, 0,
										 IL_TYPE_COMPLEX_CMOD_OPT,
										 classInfo);
		if(!modifiers)
		{
			ILGenOutOfMemory(info);
		}
		return ILTypeAddModifiers(info->context, modifiers, type);
	}
}

ILType *CTypeAddVolatile(ILGenInfo *info, ILType *type)
{
	ILClass *classInfo;
	ILType *modifiers;
	classInfo = ILType_ToClass(ILFindNonSystemType
			(info, "IsVolatile", "System.Runtime.CompilerServices"));
	if(ILTypeHasModifier(type, classInfo))
	{
		/* The type already has the specified modifier, so don't add again */
		return type;
	}
	else
	{
		/* Add a modifier prefix to the type */
		modifiers = ILTypeCreateModifier(info->context, 0,
										 IL_TYPE_COMPLEX_CMOD_REQD,
										 classInfo);
		if(!modifiers)
		{
			ILGenOutOfMemory(info);
		}
		return ILTypeAddModifiers(info->context, modifiers, type);
	}
}

int CTypeAlreadyDefined(ILType *type)
{
	if(ILType_IsValueType(type))
	{
		return !(ILClassIsRef(ILType_ToValueType(type)));
	}
	else
	{
		return 0;
	}
}

/*
 * Set the correct class attributes for a struct or union.
 */
static void SetupStructAttrs(ILGenInfo *info, ILClass *classInfo, int kind)
{
	if(kind != C_STKIND_STRUCT_NATIVE)
	{
		/* Mark the structure as needing explicit layout */
		ILClassSetAttrs(classInfo, ~((ILUInt32)0),
						IL_META_TYPEDEF_PUBLIC |
						IL_META_TYPEDEF_SERIALIZABLE |
						IL_META_TYPEDEF_EXPLICIT_LAYOUT |
						IL_META_TYPEDEF_SEALED |
						IL_META_TYPEDEF_VALUE_TYPE);
	
		/* The type initially has a packing alignment
		   of 1 and a total size of 0 */
		if(kind != C_STKIND_UNION_NATIVE)
		{
			if(!ILClassLayoutCreate(info->image, 0, classInfo, 1, 0))
			{
				ILGenOutOfMemory(info);
			}
		}
	}
	else
	{
		/* Mark native structures with sequential layout */
		ILClassSetAttrs(classInfo, ~((ILUInt32)0),
						IL_META_TYPEDEF_PUBLIC |
						IL_META_TYPEDEF_SERIALIZABLE |
						IL_META_TYPEDEF_LAYOUT_SEQUENTIAL |
						IL_META_TYPEDEF_SEALED |
						IL_META_TYPEDEF_VALUE_TYPE);
	}
}

ILType *CTypeDefineStructOrUnion(ILGenInfo *info, const char *name,
								 int kind, const char *funcName)
{
	ILType *type;
	ILClass *classInfo;
	ILClass *parent;

	/* Create the type reference, and bail out if already defined */
	type = CTypeCreateStructOrUnion(info, name, kind, funcName);
	if(CTypeAlreadyDefined(type))
	{
		return 0;
	}

	/* Convert the reference into an actual class definition */
	classInfo = ILType_ToValueType(type);
	parent = ILType_ToClass(ILFindSystemType(info, "ValueType"));
	classInfo = ILClassCreate(ILClassGlobalScope(info->image), 0,
							  ILClass_Name(classInfo), 0, parent);
	if(!classInfo)
	{
		ILGenOutOfMemory(info);
	}
	SetupStructAttrs(info, classInfo, kind);

	/* The type definition is ready to go */
	return type;
}

ILType *CTypeDefineAnonStructOrUnion(ILGenInfo *info, ILType *parent,
							  		 const char *funcName, int kind)
{
	int parentKind;
	long number;
	ILNestedInfo *nested;
	ILClass *parentInfo;
	ILClass *classInfo;
	ILProgramItem *scope;
	char name[64];
	char *newName;
	ILUInt32 attrs;

	/* Get the number to assign to the anonymous type */
	if(parent)
	{
		/* If the parent is native, then the child must be too */
		parentKind = CTypeGetStructKind(parent);
		if(parentKind == C_STKIND_STRUCT_NATIVE ||
		   parentKind == C_STKIND_UNION_NATIVE)
		{
			if(kind == C_STKIND_STRUCT)
			{
				kind = C_STKIND_STRUCT_NATIVE;
			}
			else if(kind == C_STKIND_UNION)
			{
				kind = C_STKIND_UNION_NATIVE;
			}
		}

		/* Count the nested types to determine the number */
		parentInfo = ILType_ToValueType(parent);
		number = 1;
		nested = 0;
		while((nested = ILClassNextNested(parentInfo, nested)) != 0)
		{
			++number;
		}
		scope = ILToProgramItem(parentInfo);
		attrs = IL_META_TYPEDEF_NESTED_PUBLIC;
	}
	else
	{
		/* Use the size of the TypeDef table to determine the number */
		number = (long)(ILImageNumTokens(info->image,
										 IL_META_TOKEN_TYPE_DEF) + 1);
		scope = ILClassGlobalScope(info->image);
		attrs = IL_META_TYPEDEF_NOT_PUBLIC;
	}

	/* Format the name of the type */
	if(funcName && *funcName != '\0')
	{
		/* Format the name as "struct func(N)" */
		sprintf(name, "(%ld)", number);
		newName = ILDupString(funcName);
		if(!newName)
		{
			ILGenOutOfMemory(info);
		}
		if(kind == C_STKIND_STRUCT || kind == C_STKIND_STRUCT_NATIVE)
		{
			newName = AppendThree(info, "struct ", newName, name);
		}
		else
		{
			newName = AppendThree(info, "union ", newName, name);
		}
	}
	else
	{
		/* Format the name as "struct (N)" */
		if(kind == C_STKIND_STRUCT || kind == C_STKIND_STRUCT_NATIVE)
		{
			sprintf(name, "struct (%ld)", number);
		}
		else
		{
			sprintf(name, "union (%ld)", number);
		}
		newName = ILDupString(name);
		if(!newName)
		{
			ILGenOutOfMemory(info);
		}
	}

	/* Create the anonymous type */
	parentInfo = ILType_ToClass(ILFindSystemType(info, "ValueType"));
	classInfo = ILClassCreate(scope, 0, newName, 0, parentInfo);
	if(!classInfo)
	{
		ILGenOutOfMemory(info);
	}
	SetupStructAttrs(info, classInfo, kind);

	/* The type definition is ready to go */
	ILFree(newName);
	return ILType_FromValueType(classInfo);
}

ILType *CTypeDefineEnum(ILGenInfo *info, const char *name,
						const char *funcName)
{
	ILType *type;
	ILClass *classInfo;
	ILClass *parent;
	ILField *field;

	/* Create the enum type reference, and bail out if already defined */
	type = CTypeCreateEnum(info, name, funcName);
	if(CTypeAlreadyDefined(type))
	{
		return 0;
	}

	/* Convert the reference into an actual class definition */
	classInfo = ILType_ToValueType(type);
	parent = ILType_ToClass(ILFindSystemType(info, "Enum"));
	classInfo = ILClassCreate(ILClassGlobalScope(info->image), 0,
							  ILClass_Name(classInfo), 0, parent);
	if(!classInfo)
	{
		ILGenOutOfMemory(info);
	}

	/* Set the attributes on the enumerated type correctly */
	ILClassSetAttrs(classInfo, ~((ILUInt32)0),
					IL_META_TYPEDEF_PUBLIC |
					IL_META_TYPEDEF_SERIALIZABLE |
					IL_META_TYPEDEF_SEALED);

	/* Add the "value__" field to hold the enum's value */
	field = ILFieldCreate(classInfo, 0, "value__",
						  IL_META_FIELDDEF_PUBLIC |
						  IL_META_FIELDDEF_SPECIAL_NAME |
						  IL_META_FIELDDEF_RT_SPECIAL_NAME);
	if(!field)
	{
		ILGenOutOfMemory(info);
	}
	ILMemberSetSignature((ILMember *)field, ILType_Int32);

	/* The enum definition is ready to go */
	return type;
}

ILType *CTypeDefineAnonEnum(ILGenInfo *info, const char *funcName)
{
	int funcNameLen;
	long number;
	char name[64];
	char *newName;
	ILClass *classInfo;
	ILClass *parent;
	ILField *field;

	/* Determine if we need to qualify the name using a function name */
	if(!funcName)
	{
		funcNameLen = 0;
	}
	else
	{
		funcNameLen = strlen(funcName);
	}

	/* Get a unique number for the enumeration based on the TypeDef table */
	number = (long)(ILImageNumTokens(info->image, IL_META_TOKEN_TYPE_DEF) + 1);
	sprintf(name, "(%ld)", number);

	/* Create a new name by prepending "enum " to the name */
	newName = (char *)ILMalloc(strlen(name) + funcNameLen + 1);
	if(!newName)
	{
		ILGenOutOfMemory(info);
	}
	strcpy(newName, "enum ");
	if(funcNameLen > 0)
	{
		strcpy(newName + 5, funcName);
	}
	strcpy(newName + funcNameLen + 5, name);

	/* Create the anonymous type */
	parent = ILType_ToClass(ILFindSystemType(info, "Enum"));
	classInfo = ILClassCreate(ILClassGlobalScope(info->image), 0,
							  newName, 0, parent);
	if(!classInfo)
	{
		ILGenOutOfMemory(info);
	}

	/* Set the attributes on the anonymous type correctly */
	ILClassSetAttrs(classInfo, ~((ILUInt32)0),
					IL_META_TYPEDEF_NOT_PUBLIC |
					IL_META_TYPEDEF_SERIALIZABLE |
					IL_META_TYPEDEF_SEALED);

	/* Add the "value__" field to hold the enum's value */
	field = ILFieldCreate(classInfo, 0, "value__",
						  IL_META_FIELDDEF_PUBLIC |
						  IL_META_FIELDDEF_SPECIAL_NAME |
						  IL_META_FIELDDEF_RT_SPECIAL_NAME);
	if(!field)
	{
		ILGenOutOfMemory(info);
	}
	ILMemberSetSignature((ILMember *)field, ILType_Int32);

	/* The enum definition is ready to go */
	ILFree(newName);
	return ILType_FromValueType(classInfo);
}

ILField *CTypeDefineField(ILGenInfo *info, ILType *structType,
					 	  const char *fieldName, ILType *fieldType)
{
	ILClass *classInfo = ILType_ToValueType(structType);
	ILClassLayout *layout = ILClassLayoutGetFromOwner(classInfo);
	ILUInt32 size, align;
	ILUInt32 classSize, offset;
	int isUnion = (strncmp(ILClass_Name(classInfo), "union ", 6) == 0);
	ILField *field;

	/* Determine the size and alignment of the new field */
	size = CTypeSizeAndAlign(fieldType, &align);
	if(layout)
	{
		if(size == CTYPE_DYNAMIC || size == CTYPE_UNKNOWN)
		{
			return 0;
		}
	}
	else
	{
		if(size == CTYPE_UNKNOWN)
		{
			return 0;
		}
	}

	/* Create the new field */
	field = ILFieldCreate(classInfo, 0, fieldName, IL_META_FIELDDEF_PUBLIC);
	if(!field)
	{
		ILGenOutOfMemory(info);
	}
	ILMemberSetSignature((ILMember *)field, fieldType);

	/* Bail out early if this is a native struct or union */
	if(!layout)
	{
		if(isUnion)
		{
			/* Unions lay out all of their fields at offset zero */
			if(!ILFieldLayoutCreate(info->image, 0, field, 0))
			{
				ILGenOutOfMemory(info);
			}
		}
		return field;
	}

	/* Perform explicit layout on the field */
	classSize = ILClassLayoutGetClassSize(layout);
	if(isUnion)
	{
		offset = 0;
	}
	else
	{
		offset = classSize;
	}
	if((offset % align) != 0)
	{
		offset += align - (offset % align);
	}
	if(!ILFieldLayoutCreate(info->image, 0, field, offset))
	{
		ILGenOutOfMemory(info);
	}
	offset += size;
	if(offset > classSize)
	{
		ILClassLayoutSetClassSize(layout, offset);
	}
	if(align > ILClassLayoutGetPackingSize(layout))
	{
		ILClassLayoutSetPackingSize(layout, align);
	}

	/* Return the final field to the caller */
	return field;
}

ILField *CTypeDefineBitField(ILGenInfo *info, ILType *structType,
					 	     const char *fieldName, ILType *fieldType,
							 ILUInt32 numBits)
{
	/* TODO */
	return 0;
}

void CTypeEndStruct(ILGenInfo *info, ILType *structType)
{
	ILClass *classInfo = ILType_ToValueType(structType);
	ILClassLayout *layout = ILClassLayoutGetFromOwner(classInfo);
	ILUInt32 size, align;
	if(layout != 0)
	{
		size = ILClassLayoutGetClassSize(layout);
		align = ILClassLayoutGetPackingSize(layout);
		if((size % align) != 0)
		{
			size += align - (size % align);
			ILClassLayoutSetClassSize(layout, size);
		}
	}
}

ILType *CTypeWithoutQuals(ILType *type)
{
	/* Qualifiers are stored in the IL type as custom modifiers */
	return ILTypeStripPrefixes(type);
}

static int CheckForModifier(ILType *type, const char *name,
						    const char *namespace)
{
	ILClass *classInfo;
	while(type != 0 && ILType_IsComplex(type))
	{
		if(ILType_Kind(type) == IL_TYPE_COMPLEX_CMOD_OPT ||
		   ILType_Kind(type) == IL_TYPE_COMPLEX_CMOD_REQD)
		{
			classInfo = type->un.modifier__.info__;
			if(!strcmp(ILClass_Name(classInfo), name) &&
			   ILClass_Namespace(classInfo) != 0 &&
			   !strcmp(ILClass_Namespace(classInfo), namespace))
			{
				return 1;
			}
			type = type->un.modifier__.type__;
		}
		else
		{
			break;
		}
	}
	return 0;
}

int CTypeIsConst(ILType *type)
{
	return CheckForModifier(type, "IsConst", "OpenSystem.C");
}

int CTypeIsVolatile(ILType *type)
{
	return CheckForModifier(type, "IsVolatile",
							"System.Runtime.CompilerServices");
}

int CTypeIsPrimitive(ILType *type)
{
	type = ILTypeStripPrefixes(type);
	if(ILType_IsPrimitive(type) || ILTypeIsEnum(type))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int CTypeIsInteger(ILType *type)
{
	type = ILTypeStripPrefixes(type);
	if(ILType_IsPrimitive(type))
	{
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_I1:
			case IL_META_ELEMTYPE_U1:
			case IL_META_ELEMTYPE_I2:
			case IL_META_ELEMTYPE_U2:
			case IL_META_ELEMTYPE_CHAR:
			case IL_META_ELEMTYPE_I4:
			case IL_META_ELEMTYPE_U4:
			case IL_META_ELEMTYPE_I8:
			case IL_META_ELEMTYPE_U8:
			case IL_META_ELEMTYPE_I:
			case IL_META_ELEMTYPE_U:	return 1;
		}
		return 0;
	}
	else if(ILTypeIsEnum(type))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int CTypeIsStruct(ILType *type)
{
	type = ILTypeStripPrefixes(type);
	if(ILType_IsValueType(type))
	{
		if(!strncmp(ILClass_Name(ILType_ToValueType(type)), "struct ", 7))
		{
			return 1;
		}
	}
	return 0;
}

int CTypeIsUnion(ILType *type)
{
	type = ILTypeStripPrefixes(type);
	if(ILType_IsValueType(type))
	{
		if(!strncmp(ILClass_Name(ILType_ToValueType(type)), "union ", 6))
		{
			return 1;
		}
	}
	return 0;
}

int CTypeGetStructKind(ILType *type)
{
	type = ILTypeStripPrefixes(type);
	if(ILType_IsValueType(type))
	{
		if(!strncmp(ILClass_Name(ILType_ToValueType(type)), "struct ", 7))
		{
			if(ILClass_IsExplicitLayout(ILType_ToValueType(type)))
			{
				return C_STKIND_STRUCT;
			}
			else
			{
				return C_STKIND_STRUCT_NATIVE;
			}
		}
		else if(!strncmp(ILClass_Name(ILType_ToValueType(type)), "union ", 6))
		{
			if(ILClassLayoutGetFromOwner(ILType_ToValueType(type)) != 0)
			{
				return C_STKIND_UNION;
			}
			else
			{
				return C_STKIND_UNION_NATIVE;
			}
		}
	}
	return -1;
}

int CTypeIsEnum(ILType *type)
{
	type = ILTypeStripPrefixes(type);
	if(ILType_IsValueType(type))
	{
		if(!strncmp(ILClass_Name(ILType_ToValueType(type)), "enum ", 5))
		{
			return 1;
		}
	}
	return 0;
}

int CTypeIsArray(ILType *type)
{
	type = ILTypeStripPrefixes(type);
	if(ILType_IsValueType(type))
	{
		if(!strncmp(ILClass_Name(ILType_ToValueType(type)), "array ", 6))
		{
			return 1;
		}
	}
	return 0;
}

/*
 * Find the "elem__" field within an array type.
 */
static ILField *FindArrayElemField(ILClass *classInfo)
{
	ILField *field = 0;
	while((field = (ILField *)ILClassNextMemberByKind
				(classInfo, (ILMember *)field, IL_META_MEMBERKIND_FIELD)) != 0)
	{
		if(!strcmp(ILField_Name(field), "elem__"))
		{
			return field;
		}
	}
	return 0;
}

int CTypeIsOpenArray(ILType *type)
{
	ILField *field;
	type = ILTypeStripPrefixes(type);
	if(ILType_IsValueType(type))
	{
		if(!strncmp(ILClass_Name(ILType_ToValueType(type)), "array ", 6))
		{
			field = FindArrayElemField(ILType_ToValueType(type));
			if(field)
			{
				return ILField_IsPrivate(field);
			}
			else
			{
				return 0;
			}
		}
	}
	return 0;
}

int CTypeIsPointer(ILType *type)
{
	type = ILTypeStripPrefixes(type);
	if(type != 0 && ILType_IsComplex(type) &&
	   ILType_Kind(type) == IL_TYPE_COMPLEX_PTR)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int CTypeIsMethod(ILType *type)
{
	type = ILTypeStripPrefixes(type);
	return ILType_IsMethod(type);
}

int CTypeIsForeign(ILType *type)
{
	const char *name;
	type = ILTypeStripPrefixes(type);
	if(ILType_IsValueType(type))
	{
		name = ILClass_Name(ILType_ToValueType(type));
		if(!strncmp(name, "struct ", 7) ||
		   !strncmp(name, "union ", 6) ||
		   !strncmp(name, "enum ", 5) ||
		   !strncmp(name, "array ", 6))
		{
			/* This is a specially-constructed value type of our own */
			return 0;
		}
		else if(!strcmp(name, "ArgIterator"))
		{
			/* The "System.ArgIterator" class corresponds to C's
			   "__builtin_va_list" type */
			name = ILClass_Namespace(ILType_ToValueType(type));
			if(name && !strcmp(name, "System"))
			{
				return 0;
			}
		}
	}
	else if(ILType_IsPrimitive(type))
	{
		/* Most primitive types are known to C */
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:
			case IL_META_ELEMTYPE_BOOLEAN:
			case IL_META_ELEMTYPE_I1:
			case IL_META_ELEMTYPE_U1:
			case IL_META_ELEMTYPE_I2:
			case IL_META_ELEMTYPE_U2:
			case IL_META_ELEMTYPE_CHAR:
			case IL_META_ELEMTYPE_I4:
			case IL_META_ELEMTYPE_U4:
			case IL_META_ELEMTYPE_I:
			case IL_META_ELEMTYPE_U:
			case IL_META_ELEMTYPE_I8:
			case IL_META_ELEMTYPE_U8:
			case IL_META_ELEMTYPE_R4:
			case IL_META_ELEMTYPE_R8:
			case IL_META_ELEMTYPE_R:
			{
				return 0;
			}
			/* Not reached */
		}
	}
	else if(type != 0 && ILType_IsComplex(type))
	{
		if(ILType_Kind(type) == IL_TYPE_COMPLEX_PTR ||
		   ILType_IsMethod(type))
		{
			/* Pointer and method types are known to C */
			return 0;
		}
	}
	return 1;
}

int CTypeToElementType(ILType *type)
{
	type = ILTypeGetEnumType(ILTypeStripPrefixes(type));
	if(ILType_IsPrimitive(type))
	{
		return ILType_ToElement(type);
	}
	else
	{
		return 0;
	}
}

ILUInt32 CTypeGetNumElems(ILType *type)
{
	ILClass *classInfo;
	ILField *field;
	ILType *elemType;
	ILUInt32 elemSize;
	ILClassLayout *layout;

	/* Strip the prefixes and check that this is actually an array type */
	type = ILTypeStripPrefixes(type);
	if(!CTypeIsArray(type))
	{
		return 0;
	}

	/* Search for the "elem__" field within the array type */
	classInfo = ILType_ToValueType(type);
	field = FindArrayElemField(classInfo);
	if(!field)
	{
		return 0;
	}
	elemType = ILField_Type(field);

	/* Determine the size */
	layout = ILClassLayoutGetFromOwner(classInfo);
	if(layout != 0)
	{
		/* The size is determined from the class and element sizes */
		elemSize = CTypeSizeAndAlign(elemType, 0);
		if(elemSize == 0)
		{
			/* Avoid divide by zero errors when the element type is empty */
			return 0;
		}
		else
		{
			return (ILClassLayoutGetClassSize(layout) / elemSize);
		}
	}
	else
	{
		/* No explicit layout: this shouldn't happen */
		return 0;
	}
}

ILType *CTypeGetElemType(ILType *type)
{
	ILClass *classInfo;
	ILField *field;

	/* Strip the prefixes and check that this is actually an array type */
	type = ILTypeStripPrefixes(type);
	if(!CTypeIsArray(type))
	{
		return 0;
	}

	/* Search for the "elem__" field within the array type */
	classInfo = ILType_ToValueType(type);
	field = FindArrayElemField(classInfo);
	if(field)
	{
		return ILField_Type(field);
	}
	else
	{
		return 0;
	}
}

ILType *CTypeGetPtrRef(ILType *type)
{
	type = ILTypeStripPrefixes(type);
	if(type != 0 && ILType_IsComplex(type) &&
	   ILType_Kind(type) == IL_TYPE_COMPLEX_PTR)
	{
		return ILType_Ref(type);
	}
	else
	{
		return 0;
	}
}

ILType *CTypeDecay(ILGenInfo *info, ILType *type)
{
	ILType *ptrType;

	/* Bail out if not an array type */
	if(!CTypeIsArray(type))
	{
		return type;
	}

	/* Build a pointer type from the array element type */
	ptrType = CTypeCreatePointer(info, CTypeGetElemType(type));

	/* Add back any "const" or "volatile" prefixes */
	if(CTypeIsConst(type))
	{
		ptrType = CTypeAddConst(info, ptrType);
	}
	if(CTypeIsVolatile(type))
	{
		ptrType = CTypeAddVolatile(info, ptrType);
	}

	/* Return the decayed type to the caller */
	return ptrType;
}

int CTypeIsIdentical(ILType *type1, ILType *type2)
{
	/* TODO: handle qualifiers */
	return ILTypeIdentical(type1, type2);
}

/*
 * Size and alignment values for the primitive types.
 */
#define	SIZE_INT8				1
#define	ALIGN_INT8				1
#define	SIZE_INT16				2
#define	ALIGN_INT16				2
#define	SIZE_INT32				4
#define	ALIGN_INT32				4
#define	SIZE_INT64				8
#define	ALIGN_INT64				8
#define	SIZE_FLOAT32			4
#define	ALIGN_FLOAT32			4
#define	SIZE_FLOAT64			8
#define	ALIGN_FLOAT64			8
#define	SIZE_PTR32				4
#define	ALIGN_PTR32				4
#define	SIZE_PTR64				8
#define	ALIGN_PTR64				8

ILUInt32 CTypeSizeAndAlign(ILType *_type, ILUInt32 *align)
{
	ILType *type = ILTypeStripPrefixes(_type);
	ILUInt32 alignTemp;
	ILType *subType;
	ILClass *classInfo;
	ILClassLayout *layout;

	if(!align)
	{
		align = &alignTemp;
	}
	if(ILType_IsPrimitive(type))
	{
		/* Decode the primitive type */
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:
			case IL_META_ELEMTYPE_BOOLEAN:
			case IL_META_ELEMTYPE_I1:
			case IL_META_ELEMTYPE_U1:
			{
				*align = ALIGN_INT8;
				return SIZE_INT8;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_I2:
			case IL_META_ELEMTYPE_U2:
			case IL_META_ELEMTYPE_CHAR:
			{
				*align = ALIGN_INT16;
				return SIZE_INT16;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_I4:
			case IL_META_ELEMTYPE_U4:
			{
				*align = ALIGN_INT32;
				return SIZE_INT32;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_I8:
			case IL_META_ELEMTYPE_U8:
			{
				*align = ALIGN_INT64;
				return SIZE_INT64;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_I:
			case IL_META_ELEMTYPE_U:
			{
				*align = ALIGN_INT64;
				return CTYPE_DYNAMIC;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_R4:
			{
				*align = ALIGN_FLOAT32;
				return SIZE_FLOAT32;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_R8:
			{
				*align = ALIGN_FLOAT64;
				return SIZE_FLOAT64;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_R:
			{
				*align = ALIGN_FLOAT64;
				return CTYPE_DYNAMIC;
			}
			/* Not reached */
		}
		*align = ALIGN_INT32;
		return SIZE_INT32;
	}
	else if(ILType_IsValueType(type))
	{
		/* Check for enumerated types */
		if((subType = ILTypeGetEnumType(type)) != type)
		{
			/* Use the size of the type underlying the enumeration */
			return CTypeSizeAndAlign(subType, align);
		}

		/* Bail out if this is a struct or union with unknown size */
		classInfo = ILClassResolve(ILType_ToClass(type));
		if(ILClassIsRef(classInfo))
		{
			*align = 1;
			return CTYPE_UNKNOWN;
		}

		/* If the type is an open-ended array, then it is unknown */
		if(!strncmp(ILClass_Name(classInfo), "array ", 6))
		{
			if(CTypeIsOpenArray(type))
			{
				*align = 1;
				return CTYPE_UNKNOWN;
			}
		}

		/* Look for explicit size and alignment values on the class */
		layout = ILClassLayoutGetFromOwner(classInfo);
		if(layout != 0)
		{
			*align = ILClassLayoutGetPackingSize(layout);
			return ILClassLayoutGetClassSize(layout);
		}

		/* If we don't have explicit information, then this is probably a
		   native structure or foreign value type which has a dynamic size */
		*align = 1;
		return CTYPE_DYNAMIC;
	}
	else if(type != 0 && ILType_IsComplex(type))
	{
		/* Check for pointer and method pointer types */
		if(ILType_Kind(type) == IL_TYPE_COMPLEX_PTR ||
		   (ILType_Kind(type) & IL_TYPE_COMPLEX_METHOD) != 0)
		{
			if(gen_32bit_only)
			{
				*align = ALIGN_PTR32;
				return SIZE_PTR32;
			}
			else
			{
				*align = ALIGN_PTR64;
				return SIZE_PTR64;
			}
		}
	}

	/* Assume that everything else is dynamic */
	*align = ALIGN_PTR64;
	return CTYPE_DYNAMIC;
}

/*
 * Append three strings, where the middle one is realloc'able.
 */
static char *AppendThree(ILGenInfo *info, const char *prefix,
						 char *str, const char *suffix)
{
	int prefixLen = (prefix ? strlen(prefix) : 0);
	int suffixLen = (suffix ? strlen(suffix) : 0);
	char *result;
	if(prefixLen)
	{
		result = (char *)ILMalloc(strlen(str) + prefixLen + suffixLen + 1);
		if(!result)
		{
			ILGenOutOfMemory(info);
		}
		strcpy(result, prefix);
		strcat(result, str);
		if(suffixLen)
		{
			strcat(result, suffix);
		}
		ILFree(str);
	}
	else
	{
		result = (char *)ILRealloc
			(str, strlen(str) + prefixLen + suffixLen + 1);
		if(!result)
		{
			ILGenOutOfMemory(info);
		}
		if(suffixLen)
		{
			strcat(result, suffix);
		}
	}
	return result;
}

char *CTypeToName(ILGenInfo *info, ILType *type)
{
	const char *cname;
	char *name;
	ILType *stripped;
	int modFlags;
	static const char * const beforeModifiers[] =
		{0, "const ", "volatile ", "const volatile "};
	static const char * const afterModifiers[] =
		{0, " const", " volatile", " const volatile"};

	/* Determine what kind of C type we have */
	if(ILType_IsPrimitive(type))
	{
		/* Recognise the primitive C types */
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:		cname = "void"; break;
			case IL_META_ELEMTYPE_BOOLEAN:	cname = "__bool__"; break;
			case IL_META_ELEMTYPE_I1:		cname = "char"; break;
			case IL_META_ELEMTYPE_U1:		cname = "unsigned char"; break;
			case IL_META_ELEMTYPE_I2:		cname = "short"; break;
			case IL_META_ELEMTYPE_U2:		cname = "unsigned short"; break;
			case IL_META_ELEMTYPE_CHAR:		cname = "__wchar__"; break;
			case IL_META_ELEMTYPE_I4:		cname = "int"; break;
			case IL_META_ELEMTYPE_U4:		cname = "unsigned int"; break;
			case IL_META_ELEMTYPE_I:		cname = "__native__ int"; break;
			case IL_META_ELEMTYPE_U:
				cname = "unsigned __native__ int"; break;

			case IL_META_ELEMTYPE_I8:
			{
				if(gen_32bit_only)
					cname = "long long";
				else
					cname = "long";
			}
			break;

			case IL_META_ELEMTYPE_U8:
			{
				if(gen_32bit_only)
					cname = "unsigned long long";
				else
					cname = "unsigned long";
			}
			break;

			case IL_META_ELEMTYPE_R4:		cname = "float"; break;
			case IL_META_ELEMTYPE_R8:		cname = "double"; break;
			case IL_META_ELEMTYPE_R:		cname = "long double"; break;
			default:						cname = 0; break;
		}
		if(cname)
		{
			name = ILDupString(cname);
			if(!name)
			{
				ILGenOutOfMemory(info);
			}
			return name;
		}
	}
	else if(ILType_IsValueType(type))
	{
		/* Recognise C value types */
		cname = ILClass_Name(ILType_ToValueType(type));
		if(!strncmp(cname, "struct ", 7) ||
		   !strncmp(cname, "union ", 6) ||
		   !strncmp(cname, "enum ", 5))
		{
			name = ILDupString(cname);
			if(!name)
			{
				ILGenOutOfMemory(info);
			}
			return name;
		}
		else if(!strncmp(cname, "array ", 6))
		{
			if(CTypeIsOpenArray(type))
			{
				return FormatArrayName(info, CTypeGetElemType(type), 0, 1);
			}
			else
			{
				return FormatArrayName(info, CTypeGetElemType(type),
									   CTypeGetNumElems(type), 0);
			}
		}
		else if(!strcmp(cname, "ArgIterator"))
		{
			cname = ILClass_Namespace(ILType_ToValueType(type));
			if(cname != 0 && !strcmp(cname, "System"))
			{
				/* "System.ArgIterator" is known to C as "__builtin_va_list" */
				name = ILDupString("__builtin_va_list");
				if(!name)
				{
					ILGenOutOfMemory(info);
				}
				return name;
			}
		}
	}
	else if(type != 0 && ILType_IsComplex(type))
	{
		/* Recognise complex C types */
		if(ILType_Kind(type) == IL_TYPE_COMPLEX_PTR)
		{
			/* Convert a pointer type */
			return AppendThree
				(info, 0, CTypeToName(info, ILType_Ref(type)), " *");
		}
		else if(ILType_Kind(type) == IL_TYPE_COMPLEX_CMOD_OPT ||
		        ILType_Kind(type) == IL_TYPE_COMPLEX_CMOD_REQD)
		{
			/* Look for "const" and "volatile" qualifiers */
			stripped = ILTypeStripPrefixes(type);
			modFlags = 0;
			if(CheckForModifier(type, "IsConst",
							    "OpenSystem.C"))
			{
				modFlags |= 1;
			}
			if(CheckForModifier(type, "IsVolatile",
						        "System.Runtime.CompilerServices"))
			{
				modFlags |= 2;
			}
			name = CTypeToName(info, stripped);
			if(modFlags != 0)
			{
				if(CTypeIsPointer(stripped) || CTypeIsArray(stripped))
				{
					/* Put the qualifiers after the type, not before */
					name = AppendThree(info, 0, name,
									   afterModifiers[modFlags]);
				}
				else
				{
					/* Put the qualifiers before the type */
					return AppendThree(info, beforeModifiers[modFlags],
									   name, 0);
				}
			}
			return name;
		}
		else if(ILType_IsMethod(type))
		{
			/* Convert a method pointer type */
			/* TODO */
		}
	}

	/* If we get here, then the type is foreign to C */
	name = ILTypeToName(type);
	if(!name)
	{
		ILGenOutOfMemory(info);
	}
	return name;
}

ILType *CTypeFromCSharp(ILGenInfo *info, ILNode *node)
{
	char *name;
	char *namespace;

	/* Break the identifier into name and namespace */
	if(yyisa(node, ILNode_Identifier))
	{
		name = ILQualIdentName(node, 0);
		namespace = 0;

		/* Recognise C# builtin types that aren't C keywords */
		if(!strcmp(name, "byte"))
		{
			return ILType_UInt8;
		}
		else if(!strcmp(name, "sbyte"))
		{
			return ILType_Int8;
		}
		else if(!strcmp(name, "ushort"))
		{
			return ILType_UInt16;
		}
		else if(!strcmp(name, "uint"))
		{
			return ILType_UInt32;
		}
		else if(!strcmp(name, "ulong"))
		{
			return ILType_UInt64;
		}
	}
	else
	{
		name = ILQualIdentName(((ILNode_QualIdent *)node)->right, 0);
		namespace = ILQualIdentName(((ILNode_QualIdent *)node)->left, 0);
	}

	/* Look up the type (returns NULL if not found) */
	return ILFindNonSystemType(info, name, namespace);
}

#ifdef	__cplusplus
};
#endif
