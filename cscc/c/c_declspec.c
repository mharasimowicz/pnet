/*
 * c_declspec.c - Declaration specifiers for the C programming language.
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
 * List of valid storage class specifier combinations and names.
 */
typedef struct
{
	int			specifiers;
	int			applicableKinds;
	const char *name;

} StorageClassInfo;
static StorageClassInfo const storageClasses[] = {
	{C_SPEC_TYPEDEF,
		C_KIND_GLOBAL_NAME | C_KIND_LOCAL_NAME,
		"typedef"},
	{C_SPEC_EXTERN,
		C_KIND_GLOBAL_NAME | C_KIND_LOCAL_NAME | C_KIND_FUNCTION,
		"extern"},
	{C_SPEC_STATIC | C_SPEC_INLINE,
		C_KIND_FUNCTION,
		"static inline"},
	{C_SPEC_STATIC,
		C_KIND_GLOBAL_NAME | C_KIND_LOCAL_NAME | C_KIND_FUNCTION,
		"static"},
	{C_SPEC_AUTO,
		C_KIND_LOCAL_NAME | C_KIND_PARAMETER_NAME,
		"auto"},
	{C_SPEC_REGISTER,
		C_KIND_LOCAL_NAME | C_KIND_PARAMETER_NAME,
		"register"},
	{C_SPEC_INLINE,
		C_KIND_FUNCTION,
		"inline"},
};
#define	numStorageClasses	(sizeof(storageClasses) / sizeof(StorageClassInfo))

const char *CStorageClassToName(int specifier)
{
	int index;
	for(index = 0; index < numStorageClasses; ++index)
	{
		if(specifier == storageClasses[index].specifiers)
		{
			return storageClasses[index].name;
		}
	}
	return 0;
}

CDeclSpec CDeclSpecEmpty(void)
{
	CDeclSpec result;
	result.specifiers = 0;
	result.dupSpecifiers = 0;
	result.baseType = 0;
	return result;
}

CDeclSpec CDeclSpecCombine(CDeclSpec spec1, CDeclSpec spec2)
{
	CDeclSpec result;
	int okSpecifiers;

	/* Swap the values so that the base type is in "spec1" */
	if(!(spec1.baseType) && spec2.baseType)
	{
		result = spec1;
		spec1 = spec2;
		spec2 = result;
	}

	/* Copy any duplicates that we got on the previous call */
	result.dupSpecifiers = spec1.dupSpecifiers | spec2.dupSpecifiers;

	/* Combine the storage classes and common type specifiers */
	result.specifiers =
			((spec1.specifiers | spec2.specifiers) &
					(C_SPEC_STORAGE | C_SPEC_TYPE_COMMON));

	/* Detect duplicates in the storage classes and common type specifiers */
	result.dupSpecifiers |=
			(spec1.specifiers & spec2.specifiers &
			 (C_SPEC_STORAGE | C_SPEC_TYPE_COMMON));

	/* If both specifiers have a base type, then record an error for later */
	if(spec1.baseType && spec2.baseType)
	{
		result.dupSpecifiers |= C_SPEC_MULTIPLE_BASES;
	}

	/* If both are "long", then change one into "long long" */
	if((spec1.specifiers & C_SPEC_LONG) != 0 &&
	   (spec2.specifiers & C_SPEC_LONG) != 0)
	{
		spec2.specifiers =
			(spec2.specifiers & ~C_SPEC_LONG) | C_SPEC_LONG_LONG;
	}

	/* Apply type specifiers to the base type */
	result.baseType = spec1.baseType;
	if(spec1.baseType != 0)
	{
		okSpecifiers = 0;
		if(ILType_IsPrimitive(spec1.baseType))
		{
			switch(ILType_ToElement(spec1.baseType))
			{
				case IL_META_ELEMTYPE_I1:
				{
					/* Look for "unsigned" to change int8 into uint8 */
					if(((spec1.specifiers | spec2.specifiers)
							& C_SPEC_UNSIGNED) != 0)
					{
						result.baseType = ILType_UInt8;
						okSpecifiers = C_SPEC_UNSIGNED;
					}
				}
				break;

				case IL_META_ELEMTYPE_I2:
				{
					/* Look for "unsigned" to change int16 into uint16 */
					if(((spec1.specifiers | spec2.specifiers)
							& C_SPEC_UNSIGNED) != 0)
					{
						result.baseType = ILType_UInt16;
						okSpecifiers = C_SPEC_UNSIGNED;
					}
				}
				break;

				case IL_META_ELEMTYPE_I4:
				{
					/* Look for "signed", "unsigned", "short",
					   "long", or "long long" */
					switch((spec1.specifiers | spec2.specifiers)
							& (C_SPEC_SIGNED | C_SPEC_UNSIGNED |
							   C_SPEC_SHORT | C_SPEC_LONG |
							   C_SPEC_LONG_LONG))
					{
						case C_SPEC_SIGNED:
						{
							result.baseType = ILType_Int32;
							okSpecifiers = C_SPEC_SIGNED;
						}
						break;

						case C_SPEC_UNSIGNED:
						{
							result.baseType = ILType_UInt32;
							okSpecifiers = C_SPEC_UNSIGNED;
						}
						break;

						case C_SPEC_SHORT:
						case C_SPEC_SIGNED | C_SPEC_SHORT:
						{
							result.baseType = ILType_Int16;
							okSpecifiers = C_SPEC_SIGNED | C_SPEC_SHORT;
						}
						break;

						case C_SPEC_UNSIGNED | C_SPEC_SHORT:
						{
							result.baseType = ILType_UInt16;
							okSpecifiers = C_SPEC_UNSIGNED | C_SPEC_SHORT;
						}
						break;

						case C_SPEC_LONG:
						case C_SPEC_SIGNED | C_SPEC_LONG:
						{
							if(gen_32bit_only)
							{
								result.baseType = ILType_Int32;
							}
							else
							{
								result.baseType = ILType_Int64;
							}
							okSpecifiers = C_SPEC_SIGNED | C_SPEC_LONG;
						}
						break;

						case C_SPEC_UNSIGNED | C_SPEC_LONG:
						{
							if(gen_32bit_only)
							{
								result.baseType = ILType_UInt32;
							}
							else
							{
								result.baseType = ILType_UInt64;
							}
							okSpecifiers = C_SPEC_UNSIGNED | C_SPEC_LONG;
						}
						break;

						case C_SPEC_SIGNED | C_SPEC_LONG_LONG:
						case C_SPEC_SIGNED | C_SPEC_LONG | C_SPEC_LONG_LONG:
						case C_SPEC_LONG_LONG:
						case C_SPEC_LONG | C_SPEC_LONG_LONG:
						{
							result.baseType = ILType_Int64;
							okSpecifiers = C_SPEC_SIGNED | C_SPEC_LONG |
										   C_SPEC_LONG_LONG;
						}
						break;

						case C_SPEC_UNSIGNED | C_SPEC_LONG_LONG:
						case C_SPEC_UNSIGNED | C_SPEC_LONG | C_SPEC_LONG_LONG:
						{
							result.baseType = ILType_UInt64;
							okSpecifiers = C_SPEC_UNSIGNED | C_SPEC_LONG |
										   C_SPEC_LONG_LONG;
						}
						break;
					}
				}
				break;

				case IL_META_ELEMTYPE_U4:
				{
					/* Look for "short", "long", or "long long" */
					switch((spec1.specifiers | spec2.specifiers)
							& (C_SPEC_SHORT | C_SPEC_LONG |
							   C_SPEC_LONG_LONG))
					{
						case C_SPEC_SHORT:
						{
							result.baseType = ILType_UInt16;
							okSpecifiers = C_SPEC_SHORT;
						}
						break;

						case C_SPEC_LONG:
						{
							if(gen_32bit_only)
							{
								result.baseType = ILType_UInt32;
							}
							else
							{
								result.baseType = ILType_UInt64;
							}
							okSpecifiers = C_SPEC_LONG;
						}
						break;

						case C_SPEC_LONG_LONG:
						case C_SPEC_LONG | C_SPEC_LONG_LONG:
						{
							result.baseType = ILType_UInt64;
							okSpecifiers = C_SPEC_LONG | C_SPEC_LONG_LONG;
						}
						break;
					}
				}
				break;

				case IL_META_ELEMTYPE_I8:
				{
					/* Look for "signed" and "unsigned" */
					if(((spec1.specifiers | spec2.specifiers)
							& C_SPEC_UNSIGNED) != 0)
					{
						result.baseType = ILType_UInt64;
						okSpecifiers = C_SPEC_UNSIGNED;
					}
					else if(((spec1.specifiers | spec2.specifiers)
								& C_SPEC_SIGNED) != 0)
					{
						result.baseType = ILType_Int64;
						okSpecifiers = C_SPEC_SIGNED;
					}
				}
				break;

				case IL_META_ELEMTYPE_U8:
				{
					/* Look for "unsigned" */
					if(((spec1.specifiers | spec2.specifiers)
							& C_SPEC_UNSIGNED) != 0)
					{
						result.baseType = ILType_UInt64;
						okSpecifiers = C_SPEC_UNSIGNED;
					}
				}
				break;

				case IL_META_ELEMTYPE_I:
				{
					/* Look for "signed" and "unsigned" */
					if(((spec1.specifiers | spec2.specifiers)
							& C_SPEC_UNSIGNED) != 0)
					{
						result.baseType = ILType_UInt;
						okSpecifiers = C_SPEC_UNSIGNED;
					}
					else if(((spec1.specifiers | spec2.specifiers)
								& C_SPEC_SIGNED) != 0)
					{
						result.baseType = ILType_Int;
						okSpecifiers = C_SPEC_SIGNED;
					}
				}
				break;

				case IL_META_ELEMTYPE_U:
				{
					/* Look for "unsigned" */
					if(((spec1.specifiers | spec2.specifiers)
							& C_SPEC_UNSIGNED) != 0)
					{
						result.baseType = ILType_UInt;
						okSpecifiers = C_SPEC_UNSIGNED;
					}
				}
				break;

				case IL_META_ELEMTYPE_R4:
				{
					/* Look for "long" and "long long" */
					if(((spec1.specifiers | spec2.specifiers)
							& C_SPEC_LONG_LONG) != 0)
					{
						/* "long long float" == "long double" */
						result.baseType = ILType_Float;
						okSpecifiers = C_SPEC_LONG | C_SPEC_LONG_LONG;
					}
					else if(((spec1.specifiers | spec2.specifiers)
									& C_SPEC_LONG) != 0)
					{
						/* "long float" == "double" */
						result.baseType = ILType_Float32;
						okSpecifiers = C_SPEC_LONG;
					}
				}
				break;

				case IL_META_ELEMTYPE_R8:
				{
					/* Look for "long" and "long long" */
					if(((spec1.specifiers | spec2.specifiers)
							& (C_SPEC_LONG | C_SPEC_LONG_LONG)) != 0)
					{
						result.baseType = ILType_Float;
						okSpecifiers = C_SPEC_LONG | C_SPEC_LONG_LONG;
					}
				}
				break;

				case IL_META_ELEMTYPE_R:
				{
					/* Look for "long" and "long long" */
					if(((spec1.specifiers | spec2.specifiers)
							& (C_SPEC_LONG | C_SPEC_LONG_LONG)) != 0)
					{
						/* "long long double" == "long double" */
						result.baseType = ILType_Float;
						okSpecifiers = C_SPEC_LONG | C_SPEC_LONG_LONG;
					}
				}
				break;
			}
		}
		result.specifiers |= okSpecifiers;
		if(((spec1.specifiers | spec2.specifiers) &
				(C_SPEC_TYPE_CHANGE & ~okSpecifiers)) != 0)
		{
			result.dupSpecifiers |= C_SPEC_INVALID_COMBO;
		}
	}
	else
	{
		/* Copy the type change specifiers, but don't inspect them yet */
		result.specifiers |= ((spec1.specifiers | spec2.specifiers) &
							  C_SPEC_TYPE_CHANGE);
	}

	/* Check for "signed" and "unsigned" or "short" and "long" together */
	if(((spec1.specifiers | spec2.specifiers) &
			(C_SPEC_SIGNED | C_SPEC_UNSIGNED)) ==
					(C_SPEC_SIGNED | C_SPEC_UNSIGNED))
	{
		result.dupSpecifiers |= C_SPEC_SIGN_AND_UNSIGN;
	}
	if(((spec1.specifiers | spec2.specifiers) &
			(C_SPEC_SHORT | C_SPEC_LONG)) == (C_SPEC_SHORT | C_SPEC_LONG))
	{
		result.dupSpecifiers |= C_SPEC_LONG_AND_SHORT;
	}

	/* Check for duplicate "signed", "unsigned", and "short",
	   but don't worry about "long" as we max out the sizes above */
	result.dupSpecifiers |= ((spec1.specifiers | spec2.specifiers) &
								(C_SPEC_SIGNED | C_SPEC_UNSIGNED |
								 C_SPEC_SHORT));

	/* Done */
	return result;
}

/*
 * Report a warning for a duplicate specifier.
 */
static void ReportDuplicate(ILNode *node, int specifiers,
						    int flag, const char *specName)
{
	if((specifiers & flag) != 0)
	{
		if(node)
		{
			CCWarningOnLine(yygetfilename(node), yygetlinenum(node),
							_("duplicate `%s'"), specName);
		}
		else
		{
			CCWarning(_("duplicate `%s'"), specName);
		}
	}
}

/*
 * Report an error, when we may not have a node or name to
 * report against.
 */
static void ReportError(ILNode *node, const char *name,
						const char *msg1, const char *msg2)
{
	if(node)
	{
		if(name)
		{
			CCErrorOnLine(yygetfilename(node), yygetlinenum(node), msg1, name);
		}
		else
		{
			CCErrorOnLine(yygetfilename(node), yygetlinenum(node), msg2);
		}
	}
	else
	{
		if(name)
		{
			CCError(msg1, name);
		}
		else
		{
			CCError(msg2);
		}
	}
}

/*
 * Report an warning, when we may not have a node or name to
 * report against.
 */
static void ReportWarning(ILNode *node, const char *name,
						  const char *msg1, const char *msg2)
{
	if(node)
	{
		if(name)
		{
			CCWarningOnLine(yygetfilename(node), yygetlinenum(node),
							msg1, name);
		}
		else
		{
			CCWarningOnLine(yygetfilename(node), yygetlinenum(node), msg2);
		}
	}
	else
	{
		if(name)
		{
			CCWarning(msg1, name);
		}
		else
		{
			CCWarning(msg2);
		}
	}
}

CDeclSpec CDeclSpecFinalize(CDeclSpec spec, ILNode *node,
							const char *name, int kind)
{
	CDeclSpec result;
	int storageClass, test;
	int index;

	/* Validate the storage class specifiers */
	storageClass = (spec.specifiers & C_SPEC_STORAGE);
	result.specifiers = 0;
	result.dupSpecifiers = 0;
	for(index = 0; index < numStorageClasses; ++index)
	{
		test = storageClasses[index].specifiers;
		if((storageClass & test) == test)
		{
			if((storageClasses[index].applicableKinds & kind) != 0)
			{
				result.specifiers = (storageClass & test);
				if((storageClass & ~test) != 0)
				{
					ReportError(node, name,
						_("multiple storage classes in declaration of `%s'"),
						_("multiple storage classes in declaration"));
				}
			}
			else if(node)
			{
				if(name)
				{
					CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
					   _("`%s' is not applicable to the declaration of `%s'"),
					   storageClasses[index].name, name);
				}
				else
				{
					CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
					   			  _("`%s' is not applicable here"),
					   			  storageClasses[index].name);
				}
			}
			else
			{
				if(name)
				{
					CCError
					  (_("`%s' is not applicable to the declaration of `%s'"),
					   storageClasses[index].name, name);
				}
				else
				{
					CCError(_("`%s' is not applicable here"),
					   		 storageClasses[index].name);
				}
			}
			break;
		}
	}

	/* Report duplicate storage classes and type specifiers */
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_TYPEDEF, "typedef");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_EXTERN, "extern");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_STATIC, "static");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_AUTO, "auto");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_REGISTER, "register");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_INLINE, "inline");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_CONST, "const");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_VOLATILE, "volatile");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_NATIVE, "__native__");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_SIGNED, "signed");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_UNSIGNED, "unsigned");
	ReportDuplicate(node, spec.dupSpecifiers, C_SPEC_SHORT, "short");

	/* Print pending errors that were detected by "CDeclSpecCombine" */
	if((spec.dupSpecifiers & C_SPEC_MULTIPLE_BASES) != 0)
	{
		ReportError(node, name,
					_("two or more data types in declaration of `%s'"),
					_("two or more data types in declaration"));
	}
	if((spec.dupSpecifiers & C_SPEC_SIGN_AND_UNSIGN) != 0)
	{
		ReportError(node, name,
					_("both signed and unsigned specified for `%s'"),
					_("both signed and unsigned specified"));
	}
	if((spec.dupSpecifiers & C_SPEC_LONG_AND_SHORT) != 0)
	{
		ReportError(node, name,
					_("both long and short specified for `%s'"),
					_("both long and short specified"));
	}
	if((spec.dupSpecifiers & (C_SPEC_LONG_AND_SHORT |
							  C_SPEC_SIGN_AND_UNSIGN |
							  C_SPEC_INVALID_COMBO))
			== C_SPEC_INVALID_COMBO)
	{
		ReportError(node, name,
					_("long, short, signed, or unsigned invalid for `%s'"),
					_("long, short, signed, or unsigned invalid here"));
	}

	/* Copy the common type qualifiers to "result.specifiers" */
	result.specifiers |= (spec.specifiers & C_SPEC_TYPE_COMMON);

	/* If we don't have a base type yet, then infer the most obvious one */
	if(spec.baseType == 0)
	{
		if((spec.specifiers & C_SPEC_SHORT) != 0)
		{
			if((spec.specifiers & C_SPEC_UNSIGNED) != 0)
			{
				result.baseType = ILType_UInt16;
			}
			else
			{
				result.baseType = ILType_Int16;
			}
		}
		else if((spec.specifiers & C_SPEC_LONG) != 0 && gen_32bit_only)
		{
			if((spec.specifiers & C_SPEC_UNSIGNED) != 0)
			{
				result.baseType = ILType_UInt32;
			}
			else
			{
				result.baseType = ILType_Int32;
			}
		}
		else if((spec.specifiers & (C_SPEC_LONG | C_SPEC_LONG_LONG)) != 0)
		{
			if((spec.specifiers & C_SPEC_UNSIGNED) != 0)
			{
				result.baseType = ILType_UInt64;
			}
			else
			{
				result.baseType = ILType_Int64;
			}
		}
		else if((spec.specifiers & C_SPEC_UNSIGNED) != 0)
		{
			result.baseType = ILType_UInt32;
		}
		else
		{
			ReportWarning(node, name,
					  	  _("type defaults to `int' in declaration of `%s'"),
					  	  _("type defaults to `int' in declaration"));
			result.baseType = ILType_Int32;
		}
	}
	else
	{
		result.baseType = spec.baseType;
	}

	/* Return the result to the caller */
	return result;
}

CDeclarator CDeclCreateArray(ILGenInfo *info, CDeclarator elem)
{
	CDeclarator result;
	ILType *type;

	/* Simple array types are identical to pointer types */
	type = ILTypeCreateRef(info->context, IL_TYPE_COMPLEX_PTR,
						   ILType_Invalid);
	if(!type)
	{
		ILGenOutOfMemory(info);
	}

	/* Insert "type" into the hole in "elem" to create "result" */
	result.name = elem.name;
	result.node = elem.node;
	if(elem.typeHole != 0)
	{
		result.type = elem.type;
		*(elem.typeHole) = type;
		result.typeHole = elem.typeHole;
	}
	else
	{
		result.type = type;
		result.typeHole = &(ILType_Ref(type));
	}
	result.isKR = 0;
	result.params = elem.params;
	result.attrs = elem.attrs;
	return result;
}

CDeclarator CDeclCreateSizedArray(ILGenInfo *info, CDeclarator elem,
								  ILUInt32 size)
{
	CDeclarator result;
	ILType *type;

	/* Create a C#-style array reference, which we will turn into a
	   C-style array type when "CDeclFinalize" is called.  For now,
	   this is just a place-holder */
	type = ILTypeCreateArray(info->context, 1, ILType_Invalid);
	if(!type)
	{
		ILGenOutOfMemory(info);
	}
	ILTypeSetSize(type, 0, (unsigned long)size);

	/* Insert "type" into the hole in "elem" to create "result" */
	result.name = elem.name;
	result.node = elem.node;
	if(elem.typeHole != 0)
	{
		result.type = elem.type;
		*(elem.typeHole) = type;
		result.typeHole = elem.typeHole;
	}
	else
	{
		result.type = type;
		result.typeHole = &(ILType_ElemType(type));
	}
	result.isKR = 0;
	result.params = elem.params;
	result.attrs = elem.attrs;
	return result;
}

CDeclarator CDeclCreatePointer(ILGenInfo *info, int qualifiers,
							   CDeclarator *refType)
{
	CDeclarator result;
	ILType *type;

	/* Create the actual pointer type */
	if(refType)
	{
		type = CTypeCreatePointer(info, refType->type,
								  ((qualifiers & C_SPEC_NATIVE) != 0));
		result.typeHole = refType->typeHole;
	}
	else
	{
		type = CTypeCreatePointer(info, ILType_Invalid,
								  ((qualifiers & C_SPEC_NATIVE) != 0));
		result.typeHole = &(ILType_Ref(type));
	}

	/* Wrap the pointer type in the "const" and "volatile" qualifiers */
	if((qualifiers & C_SPEC_CONST) != 0)
	{
		type = CTypeAddConst(info, type);
	}
	if((qualifiers & C_SPEC_VOLATILE) != 0)
	{
		type = CTypeAddVolatile(info, type);
	}

	/* Construct the return value (the type hole was set above) */
	result.name = 0;
	result.node = 0;
	result.type = type;
	result.isKR = 0;
	if(refType)
	{
		result.params = refType->params;
		result.attrs = refType->attrs;
	}
	else
	{
		result.params = 0;
		result.attrs = 0;
	}
	return result;
}

/*
 * Print an error that warns about parameter redeclaration.
 */
static void ParamRedeclared(ILNode *newName, ILNode *oldName)
{
	CCErrorOnLine(yygetfilename(newName), yygetlinenum(newName),
				  _("redeclaration of `%s'"),
				  ILQualIdentName(newName, 0));
	if(oldName)
	{
		CCErrorOnLine(yygetfilename(oldName), yygetlinenum(oldName),
					  _("`%s' previously declared here"),
					  ILQualIdentName(newName, 0));
	}
}

/*
 * Turn a parameter list into a function signature.  "declaredParams"
 * is the list of parameters that were declared using old-style K&R
 * conventions, or NULL if no such list.  "method" is the method block
 * for a global function header, or NULL if building a pointer type.
 */
static ILType *ParamsToSignature(ILGenInfo *info, ILNode *params,
								 ILNode *declaredParams, ILMethod *method,
								 ILType ***hole)
{
	ILType *signature;
	ILNode_ListIter iter;
	ILNode_ListIter iter2;
	ILNode_FormalParameter *formal;
	ILNode_FormalParameter *formal2;
	ILNode_FormalParameter *formal3;
	ILNode *name;
	ILType *type;

	/* Create the bare function signature and find the return type hole */
	signature = ILTypeCreateMethod(info->context, ILType_Invalid);
	if(!signature)
	{
		ILGenOutOfMemory(info);
	}
	*hole = &(signature->un.method__.retType__);

	/* Create a default declared parameter list */
	if(!declaredParams)
	{
		declaredParams = ILNode_List_create();
	}

	/* Add the formal parameters to the signature */
	if(params)
	{
		ILNode_ListIter_Init(&iter, params);
		while((formal = (ILNode_FormalParameter *)
					ILNode_ListIter_Next(&iter)) != 0)
		{
			/* Process special arguments */
			if(formal->pmod == ILParamMod_arglist)
			{
				/* The signature includes a "..." argument */
				ILTypeSetCallConv(signature, IL_META_CALLCONV_VARARG);
				continue;
			}
			else if(formal->type && ILTypeIdentical
					 (ILType_Void, ((ILNode_MarkType *)(formal->type))->type))
			{
				/* Skip "void" arguments, which exist to distinguish
				   ANSI C functions with no arguments from K&R prototypes
				   that may have undeclared arguments */
				if(formal->name)
				{
					CCErrorOnLine(yygetfilename(formal->name),
								  yygetlinenum(formal->name),
								  _("parameter `%s' has incomplete type"),
								  ILQualIdentName(formal->name, 0));
				}
				continue;
			}

			/* Get the name and type information from the formal parmameter */
			name = formal->name;
			if(formal->type)
			{
				type = ((ILNode_MarkType *)(formal->type))->type;
			}
			else
			{
				type = ILType_Invalid;
			}

			/* If we have a name, then look for a declared type */
			if(name)
			{
				ILNode_ListIter_Init(&iter2, declaredParams);
				formal3 = 0;
				while((formal2 = (ILNode_FormalParameter *)
							ILNode_ListIter_Next(&iter2)) != 0)
				{
					if(ILQualIdentName(name, 0) ==
					   ILQualIdentName(formal2->name, 0))
					{
						if(formal3 != 0)
						{
							/* Two or more K&R declarations for the name */
							formal2->pmod = ILParamMod_ref;
						}
						else
						{
							formal3 = formal2;
						}
					}
				}
				formal2 = formal3;
				if(formal2 != 0)
				{
					if(formal2->pmod == ILParamMod_out)
					{
						/* We've processed this parameter already */
						ParamRedeclared(name, formal2->name);
					}
					else if(type == ILType_Invalid)
					{
						/* Retrieve the type from the declared params list */
						type = ((ILNode_MarkType *)(formal2->type))->type;

						/* Mark the declared parameter as already seen */
						formal2->pmod = ILParamMod_out;
					}
					else
					{
						/* Given a type in both ANSI-style and K&R-style */
						ParamRedeclared(name, formal2->name);
					}
				}
			}
			else
			{
				formal2 = 0;
			}

			/* If we don't have a type yet, then assume "int" */
			if(type == ILType_Invalid)
			{
				type = ILType_Int32;
			}

			/* Add an entry to "declaredParams" to allow us to detect
			   multiple declarations of the same parameter in ANSI mode */
			if(name != 0 && formal2 == 0)
			{
				ILNode_List_Add(declaredParams,
					ILNode_FormalParameter_create
						(0, ILParamMod_out,
						 ILNode_MarkType_create(0, type), name));
			}

			/* Decay array types to their pointer forms */
			type = CTypeDecay(info, type);

			/* Add the new parameter to the function signature */
			if(!ILTypeAddParam(info->context, signature, type))
			{
				ILGenOutOfMemory(info);
			}

			/* Add a parameter information block to the method for the name */
			if(name && method)
			{
				if(!ILParameterCreate(method, 0, ILQualIdentName(name, 0), 0,
									  (ILUInt32)ILTypeNumParams(signature)))
				{
					ILGenOutOfMemory(info);
				}
			}
		}
	}

	/* Check that all declared parameters have been used */
	ILNode_ListIter_Init(&iter, declaredParams);
	while((formal = (ILNode_FormalParameter *)
				ILNode_ListIter_Next(&iter)) != 0)
	{
		if(formal->pmod == ILParamMod_empty)
		{
			CCErrorOnLine
				(yygetfilename(formal->name), yygetlinenum(formal->name),
				_("declaration for parameter `%s', but no such parameter"),
				ILQualIdentName(formal->name, 0));
		}
		else if(formal->pmod == ILParamMod_ref)
		{
			ParamRedeclared(formal->name, 0);
		}
	}

	/* The signature is ready to go */
	return signature;
}

CDeclarator CDeclCreatePrototype(ILGenInfo *info, CDeclarator decl,
								 ILNode *params, ILNode *attributes)
{
	ILType *signature;
	ILType **returnHole;

	/* Bail out if we already have parameters in the declarator */
	if(decl.params)
	{
		CCError(_("cannot declare a function returning a function"));
		return decl;
	}

	/* If the declarator has a type hole, then create a signature
	   inside it, and replace the hole with the return type */
	if(decl.typeHole)
	{
		signature = ParamsToSignature(info, params, 0, 0, &returnHole);
		*returnHole = decl.type;
		decl.type = signature;
		return decl;
	}

	/* Modify the declarator and return it */
	if(params)
	{
		decl.params = params;
		decl.isKR = 0;
	}
	else
	{
		decl.params = ILNode_List_create();
		decl.isKR = 1;
	}
	decl.attrs = attributes;
	return decl;
}

CDeclarator CDeclCombine(CDeclarator decl1, CDeclarator decl2)
{
	CDeclarator result;
	result.name = decl2.name;
	result.node = decl2.node;
	if(decl2.typeHole)
	{
		result.type = decl2.type;
		*(decl2.typeHole) = decl1.type;
		result.typeHole = decl1.typeHole;
	}
	else
	{
		result.type = decl1.type;
		result.typeHole = decl1.typeHole;
	}
	result.isKR = 0;
	result.params = decl2.params;
	result.attrs = decl2.attrs;
	return result;
}

/*
 * Replace references to C#-style array types in a type declarator tree.
 */
static ILType *ReplaceArrayTypes(ILGenInfo *info, ILType *type)
{
	ILType *elemType;
	if(type != 0 && ILType_IsComplex(type))
	{
		switch(ILType_Kind(type))
		{
			case IL_TYPE_COMPLEX_PTR:
			{
				/* Replace array types in the pointed-to type */
				ILType_Ref(type) = ReplaceArrayTypes(info, ILType_Ref(type));
			}
			break;

			case IL_TYPE_COMPLEX_ARRAY:
			{
				/* Replace array types in the element type */
				elemType = ReplaceArrayTypes(info, ILType_ElemType(type));

				/* Report an error if the type is not defined yet */
				if(!CTypeAlreadyDefined(elemType))
				{
					char *name = CTypeToName(info, elemType);
					CCError(_("storage size of `%s' is not known"), name);
					ILFree(name);
				}

				/* Construct a new C-style array type */
				type = CTypeCreateArray(info, elemType, ILType_Size(type));
			}
			break;

			case IL_TYPE_COMPLEX_CMOD_OPT:
			case IL_TYPE_COMPLEX_CMOD_REQD:
			{
				/* Replace array types in the modifier's argument */
				type->un.modifier__.type__ =
					ReplaceArrayTypes(info, type->un.modifier__.type__);
			}
			break;

			default: break;
		}
	}
	return type;
}

ILType *CDeclFinalize(ILGenInfo *info, CDeclSpec spec, CDeclarator decl,
					  ILNode *declaredParams, ILMethod *method)
{
	ILType *type;
	ILType *signature;
	ILType **returnHole;

	/* Get the base type from "spec".  The default type is assumed to
	   be "int", for functions that don't have a return type specified */
	if(spec.baseType != ILType_Invalid)
	{
		type = spec.baseType;
	}
	else
	{
		type = ILType_Int32;
	}

	/* Add the "const" and "volatile" qualifiers from "spec" */
	if((spec.specifiers & C_SPEC_CONST) != 0)
	{
		type = CTypeAddConst(info, type);
	}
	if((spec.specifiers & C_SPEC_VOLATILE) != 0)
	{
		type = CTypeAddVolatile(info, type);
	}

	/* Insert the base type into the type hole in "decl" */
	if(decl.typeHole)
	{
		*(decl.typeHole) = type;
		type = decl.type;
	}

	/* Replace C#-style array types with C-style array types */
	type = ReplaceArrayTypes(info, type);

	/* If we have a method declaration, then build a function prototype */
	if(method)
	{
		if(!(decl.params))
		{
			CCError(_("missing parameters for function declaration"));
		}
		signature = ParamsToSignature(info, decl.params, declaredParams,
									  method, &returnHole);
		*returnHole = type;
		type = signature;
		ILMemberSetSignature((ILMember *)method, signature);
		ILMethodSetCallConv(method, ILType_CallConv(signature));
	}
	else if(decl.params)
	{
		/* Create a function signature pointer type */
		signature = ParamsToSignature(info, decl.params, 0, 0, &returnHole);
		*returnHole = type;
		type = signature;
		/* TODO: strip the unnecessary pointer level from the type */
	}

	/* Return the final type to the caller */
	return type;
}

#ifdef	__cplusplus
};
#endif
