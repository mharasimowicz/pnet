/*
 * ilasm_build.c - Data structure building helper routines.
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

#include "il_values.h"
#include "ilasm_build.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Imports.
 */
void ILAsmPrintMessage(const char *filename, long linenum,
					   const char *format, ...);
void ILAsmOutOfMemory(void);
extern char *ILAsmFilename;
extern long  ILAsmLineNum;
extern int   ILAsmErrors;

/*
 * Globals that are exported.
 */
ILContext *ILAsmContext;
ILImage *ILAsmImage;
ILModule *ILAsmModule;
ILAssembly *ILAsmAssembly;
ILUInt32 ILAsmLastToken;
ILProgramItem *ILAsmCurrScope;
ILClass *ILAsmClass;
ILAssembly *ILAsmCurrAssemblyRef;
int ILAsmDebugMode;
char *ILAsmDebugLastFile = "";

/*
 * Globals that are local to this module.
 */
static ILIntString namespace;
static ILClass **classStack;
static int classStackSize;
static int classStackMax;
static ILProgramItem **scopeStack;
static int scopeStackSize;
static int scopeStackMax;

void ILAsmBuildInit(const char *outputFilename)
{
	int len;

	/* Create the context and its image */
	if(!(ILAsmContext = ILContextCreate()))
	{
		ILAsmOutOfMemory();
	}
	if(!(ILAsmImage = ILImageCreate(ILAsmContext)))
	{
		ILAsmOutOfMemory();
	}

	/* Get the base part of the filename */
	len = strlen(outputFilename);
	while(len > 0 && outputFilename[len - 1] != '/' &&
	      outputFilename[len - 1] != '\\')
	{
		--len;
	}
	outputFilename += len;

	/* Create the default module record */
	if(!(ILAsmModule = ILModuleCreate(ILAsmImage, 0, outputFilename, 0)))
	{
		ILAsmOutOfMemory();
	}

	/* Create the default assembly record */
	if(!(ILAsmAssembly = ILAssemblyCreate(ILAsmImage, 0, outputFilename, 0)))
	{
		ILAsmOutOfMemory();
	}

	/* Create the TypeDef for the module */
	if(!(ILAsmClass = ILClassCreate((ILProgramItem *)ILAsmModule, 0,
								    "<Module>", 0, 0)))
	{
		ILAsmOutOfMemory();
	}

	/* Initialize other globals */
	ILAsmCurrAssemblyRef = 0;
	namespace = ILInternString("", 0);
	classStack = 0;
	classStackSize = 0;
	classStackMax = 0;
	scopeStack = 0;
	scopeStackSize = 0;
	scopeStackMax = 0;

	/* The current scope is the module TypeDef */
	ILAsmCurrScope = ILToProgramItem(ILAsmClass);

	/* The last token is currently the module */
	ILAsmLastToken = ILProgramItemGetToken(ILToProgramItem(ILAsmModule));
}

void ILAsmBuildPushNamespace(ILIntString name)
{
	if(name.len == 0)
	{
		return;
	}
	if(namespace.len)
	{
		ILIntString dot;
		dot.string = ".";
		dot.len = 1;
		namespace = ILInternAppendedString(namespace,
						ILInternAppendedString(dot, name));
	}
	else
	{
		namespace = name;
	}
}

void ILAsmBuildPopNamespace(int nameLen)
{
	if(nameLen != 0)
	{
		if(nameLen == namespace.len)
		{
			namespace = ILInternString("", 0);
		}
		else
		{
			namespace = ILInternString(namespace.string,
									   namespace.len - nameLen - 1);
		}
	}
}

void _ILAsmBuildPushScope(ILProgramItem *item)
{
	/* Add the current scope to the scope stack */
	if(scopeStackSize >= scopeStackMax)
	{
		scopeStack = (ILProgramItem **)ILRealloc(scopeStack,
							sizeof(ILProgramItem *) * (scopeStackMax + 4));
		if(!scopeStack)
		{
			ILAsmOutOfMemory();
		}
		scopeStackMax += 4;
	}
	scopeStack[scopeStackSize++] = ILAsmCurrScope;
	ILAsmCurrScope = (item ? item : ILToProgramItem(ILAsmModule));
	ILAsmLastToken = ILProgramItem_Token(ILAsmCurrScope);
}

void ILAsmBuildPopScope(void)
{
	if(scopeStackSize > 0)
	{
		ILAsmCurrScope = scopeStack[--scopeStackSize];
		ILAsmLastToken = ILProgramItem_Token(ILAsmCurrScope);
	}
}

void ILAsmSplitName(const char *str, int len, const char **name,
					const char **namespace)
{
	int dotposn = len;
	while(dotposn > 0 && str[dotposn - 1] != '.')
	{
		--dotposn;
	}
	if(dotposn > 0)
	{
		*name = str + dotposn;
		*namespace = (ILInternString((char *)str, dotposn - 1)).string;
	}
	else
	{
		*name = str;
		*namespace = 0;
	}
	if(str[len] != '\0')
	{
		*name = (ILInternString((char *)(*name), len - dotposn)).string;
	}
}

void ILAsmBuildNewClass(const char *name, ILClass *parent)
{
	ILClass *info;
	static long nextUnique = 1;
	char uniqueName[64];

	/* If we are compiling "System.Object", then ignore the parent */
	if(!strcmp(name, "Object") && !strcmp(namespace.string, "System"))
	{
		parent = 0;
	}

	/* Do we already have a class with this name? */
	if(classStackSize == 0)
	{
		/* Declaring a class at the outer-most level */
		info = ILClassLookup((ILProgramItem *)ILAsmModule, name,
							 (namespace.len ? namespace.string : 0));
		if(info && !ILClassIsRef(info))
		{
			ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
							  "class is already declared");
			ILAsmErrors = 1;
	
			/* Convert the name into something else, to prevent collisions */
			sprintf(uniqueName, "&%ld&", nextUnique++);
			name = uniqueName;
		}

		/* Create the class record */
		info = ILClassCreate((ILProgramItem *)ILAsmModule, 0, name,
						     (namespace.len ? namespace.string : 0), parent);
		if(!info)
		{
			ILAsmOutOfMemory();
		}
	}
	else
	{
		/* Declaring a nested class */
		info = ILClassLookup((ILProgramItem *)ILAsmClass, name, 0);
		if(info && !ILClassIsRef(info))
		{
			ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
							  "nested class is already declared");
			ILAsmErrors = 1;
	
			/* Convert the name into something else, to prevent collisions */
			sprintf(uniqueName, "&%ld&", nextUnique++);
			name = uniqueName;
		}

		/* Create the class record, nested within "ILAsmClass" */
		info = ILClassCreate((ILProgramItem *)ILAsmClass, 0, name, 0, parent);
		if(!info)
		{
			ILAsmOutOfMemory();
		}
	}

	/* Add the previous class to the class stack */
	if(classStackSize >= classStackMax)
	{
		classStack = (ILClass **)ILRealloc(classStack,
							sizeof(ILClass *) * (classStackMax + 4));
		if(!classStack)
		{
			ILAsmOutOfMemory();
		}
		classStackMax += 4;
	}
	classStack[classStackSize++] = ILAsmClass;
	ILAsmClass = info;
}

void ILAsmBuildPopClass(void)
{
	ILMember *member;
	ILNestedInfo *nested;
	ILClass *child;

	/* Search for any methods or fields that are still MemberRef's,
	   which means they were referenced, but not defined */
	member = 0;
	while((member = (ILMember *)ILClassNextMember(ILAsmClass, member)) != 0)
	{
		if((ILMember_Token(member) & IL_META_TOKEN_MASK)
				== IL_META_TOKEN_MEMBER_REF)
		{
			ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
							  "referenced member `%s' is not defined",
							  ILMember_Name(member));
			ILAsmErrors = 1;
		}
	}

	/* Search for any nested children that are still references,
	   which means they were referenced, but not defined */
	nested = 0;
	while((nested = ILClassNextNested(ILAsmClass, nested)) != 0)
	{
		child = ILNestedInfoGetChild(nested);
		if(ILClassIsRef(child))
		{
			ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
							  "referenced nested class `%s' is not defined",
							  ILClass_Name(child));
			ILAsmErrors = 1;
		}
	}

	/* Mark the class as complete */
	ILClassMarkComplete(ILAsmClass);

	/* If this is a class at the outermost level, then sort its members */
	if(classStackSize <= 1)
	{
		ILClassSortMembers(ILAsmClass);
	}

	/* Return to the previous level */
	ILAsmClass = classStack[--classStackSize];
}

ILClass *ILAsmClassLookup(const char *name, ILProgramItem *scope)
{
	const char *baseName;
	const char *namespace;
	ILClass *info = 0;
	ILClass *newInfo;
	int namelen;
	for(;;)
	{
		/* Find the next occurrence of '/' */
		namelen = 0;
		while(name[namelen] != '\0' && name[namelen] != '/')
		{
			++namelen;
		}

		/* Split the component into base name and namespace */
		ILAsmSplitName(name, namelen, &baseName, &namespace);

		/* Look up the name */
		if(!info)
		{
			/* Look for the class at the outermost level */
			info = ILClassLookup(scope, baseName, namespace);
			if(!info)
			{
				info = ILClassCreateRef(scope, 0, baseName, namespace);
				if(!info)
				{
					ILAsmOutOfMemory();
				}
			}
		}
		else
		{
			/* Look for a nested class */
			newInfo = ILClassLookup((ILProgramItem *)info, baseName, namespace);
			if(!newInfo)
			{
				if(!ILClassIsComplete(info))
				{
					/* The class is not complete, so add a TypeRef */
					newInfo = ILClassCreateRef((ILProgramItem *)info, 0,
											   baseName, namespace);
					if(!newInfo)
					{
						ILAsmOutOfMemory();
					}
				}
				else
				{
					/* The class is complete, so report an error */
					ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
									  "cannot resolve nested class `%s%s%s'",
									  (namespace ? namespace : ""),
									  (namespace ? "." : ""), baseName);
					ILAsmErrors = 1;
					newInfo = ILClassCreateRef((ILProgramItem *)info, 0,
											   baseName, namespace);
					if(!newInfo)
					{
						ILAsmOutOfMemory();
					}
				}
			}
			info = newInfo;
		}

		/* Move on to the next component */
		name += namelen;
		if(*name == '/')
		{
			++name;
		}
		else
		{
			break;
		}
	}
	return info;
}

ILAssembly *ILAsmFindAssemblyRef(const char *name)
{
	ILAssembly *assem;

	/* Scan the AssemblyRef table looking for a match */
	assem = 0;
	while((assem = (ILAssembly *)ILImageNextToken
				(ILAsmImage, IL_META_TOKEN_ASSEMBLY_REF, assem)) != 0)
	{
		if(!ILStrICmp(ILAssembly_Name(assem), name))
		{
			return assem;
		}
	}

	/* Scan the Assembly table looking for a match.  If we find one,
	   then return NULL to indicate the current module */
	while((assem = (ILAssembly *)ILImageNextToken
				(ILAsmImage, IL_META_TOKEN_ASSEMBLY, assem)) != 0)
	{
		if(!ILStrICmp(ILAssembly_Name(assem), name))
		{
			return 0;
		}
	}

	/* Could not find a match if we get here */
	ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
					  "assembly `%s' is not declared", name);
	ILAsmErrors = 1;

	/* Create a new AssemblyRef for the name */
	assem = ILAssemblyCreate(ILAsmImage, 0, name, 1);
	if(!assem)
	{
		ILAsmOutOfMemory();
	}
	return assem;
}

ILModule *ILAsmFindModuleRef(const char *name)
{
	ILModule *module;

	/* Scan the ModuleRef table looking for a match */
	module = 0;
	while((module = (ILModule *)ILImageNextToken
				(ILAsmImage, IL_META_TOKEN_MODULE_REF, module)) != 0)
	{
		if(!ILStrICmp(ILModule_Name(module), name))
		{
			return module;
		}
	}

	/* Scan the Module table looking for a match.  If we find one,
	   then return NULL to indicate the current module */
	while((module = (ILModule *)ILImageNextToken
				(ILAsmImage, IL_META_TOKEN_MODULE, module)) != 0)
	{
		if(!ILStrICmp(ILModule_Name(module), name))
		{
			return 0;
		}
	}

	/* Could not find a match if we get here */
	ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
					  "module `%s' is not declared", name);
	ILAsmErrors = 1;

	/* Create a new ModuleRef for the name */
	module = ILModuleRefCreate(ILAsmImage, 0, name);
	if(!module)
	{
		ILAsmOutOfMemory();
	}
	return module;
}

ILFileDecl *ILAsmFindFile(const char *name, ILUInt32 attrs, int ref)
{
	ILFileDecl *decl;

	/* Scan the File table looking for a match */
	decl = 0;
	while((decl = (ILFileDecl *)ILImageNextToken
				(ILAsmImage, IL_META_TOKEN_FILE, decl)) != 0)
	{
		if(!strcmp(ILFileDecl_Name(decl), name))
		{
			if(!ref)
			{
				/* We are declaring a new file, but the name
				   already exists within the image */
				ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
								  "file `%s' declared multiple times", name);
				ILAsmErrors = 1;
			}
			return decl;
		}
	}

	/* Print an error if looking for a reference and we didn't find one */
	if(ref)
	{
		ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
						  "file `%s' is not declared", name);
		ILAsmErrors = 1;
	}

	/* Create a new file declaration record */
	decl = ILFileDeclCreate(ILAsmImage, 0, name, attrs);
	if(!decl)
	{
		ILAsmOutOfMemory();
	}
	return decl;
}

ILToken ILAsmResolveMember(ILProgramItem *scope, const char *name,
						   ILType *sig, int kind)
{
	ILTypeSpec *spec;
	ILClass *classInfo;
	ILMember *member;
	ILMethod *method;
	ILField *field;

	/* Convert TypeSpec tokens into class tokens */
	if((spec = ILProgramItemToTypeSpec(scope)) != 0)
	{
		classInfo = ILTypeSpecGetClass(spec);
		if(!classInfo)
		{
			ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
				  "cannot convert the type specification into a class");
			ILAsmErrors = 1;
			return 0;
		}
	}
	else if((classInfo = ILProgramItemToClass(scope)) == 0)
	{
		ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
						  "invalid scope for member resolution");
		ILAsmErrors = 1;
		return 0;
	}

	/* Look for a name and signature match on a member */
	member = 0;
	while((member = ILClassNextMemberByKind(classInfo, member, kind)) != 0)
	{
		if(!strcmp(ILMember_Name(member), name) &&
		   ILTypeIdentical(ILMember_Signature(member), sig))
		{
			return ILMember_Token(member);
		}
	}

	/* If the class is "complete", then we have already seen its
	   definition and we cannot add anything else to it */
	if(ILClassIsComplete(classInfo))
	{
		ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
						  "cannot resolve member `%s'", name);
		ILAsmErrors = 1;
		return 0;
	}

	/* Generate a MemberRef token within the class, which will hopefully
	   be fixed up later when we encounter the real definition */
	if(kind == IL_META_MEMBERKIND_METHOD)
	{
		method = ILMethodCreate(classInfo, (ILToken)IL_MAX_UINT32,
								name, 0);
		if(!method)
		{
			ILAsmOutOfMemory();
		}
		ILMemberSetSignature((ILMember *)method, sig);
		ILMethodSetCallConv(method, (sig->kind >> 8));
		return ILMethod_Token(method);
	}
	else
	{
		field = ILFieldCreate(classInfo, (ILToken)IL_MAX_UINT32,
							  name, 0);
		if(!field)
		{
			ILAsmOutOfMemory();
		}
		ILMemberSetSignature((ILMember *)field, sig);
		return ILField_Token(field);
	}
}

ILMethod *ILAsmMethodCreate(ILClass *classInfo, const char *name,
							ILUInt32 attributes, ILType *sig)
{
	ILMethod *method;

	/* See if there is already a method with a matching signature */
	method = 0;
	while((method = (ILMethod *)ILClassNextMemberByKind
			(classInfo, (ILMember *)method, IL_META_MEMBERKIND_METHOD)) != 0)
	{
		if(!strcmp(ILMethod_Name(method), name) &&
		   ILTypeIdentical(ILMethod_Signature(method), sig))
		{
			/* We already have a definition */
			if((ILMethod_Token(method) & IL_META_TOKEN_MASK)
					== IL_META_TOKEN_METHOD_DEF &&
			   ILMethod_RVA(method) != 0)
			{
				/* This is a duplicate */
				ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
								  "duplicate definition for `%s'", name);
				ILAsmErrors = 1;
			}
			else
			{
				/* Convert the MemberRef into a MethodDef */
				ILMethodSetCallConv(method, (sig->kind >> 8));
				ILMemberSetAttrs((ILMember *)method,
								 ~((ILUInt32)0), attributes);
				if((ILMethod_Token(method) & IL_META_TOKEN_MASK)
						== IL_META_TOKEN_MEMBER_REF)
				{
					ILMethodNewToken(method);
				}
			}
			return method;
		}
	}

	/* Create a new method block */
	method = ILMethodCreate(classInfo, 0, name, attributes);
	if(!method)
	{
		ILAsmOutOfMemory();
	}
	ILMemberSetSignature((ILMember *)method, sig);
	ILMethodSetCallConv(method, (sig->kind >> 8));
	return method;
}

ILField *ILAsmFieldCreate(ILClass *classInfo, const char *name,
						  ILUInt32 attributes, ILType *sig)
{
	ILField *field;

	/* See if there is already a field with a matching signature */
	field = 0;
	while((field = (ILField *)ILClassNextMemberByKind
			(classInfo, (ILMember *)field, IL_META_MEMBERKIND_FIELD)) != 0)
	{
		if(!strcmp(ILField_Name(field), name) &&
		   ILTypeIdentical(ILField_Type(field), sig))
		{
			/* We already have a definition */
			if((ILField_Token(field) & IL_META_TOKEN_MASK)
					== IL_META_TOKEN_FIELD_DEF)
			{
				/* This is a duplicate */
				ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
								  "duplicate definition for `%s'", name);
				ILAsmErrors = 1;
			}
			else
			{
				/* Convert the MemberRef into a FieldDef */
				ILMemberSetAttrs((ILMember *)field,
								 ~((ILUInt32)0), attributes);
				if((ILField_Token(field) & IL_META_TOKEN_MASK)
						== IL_META_TOKEN_MEMBER_REF)
				{
					ILFieldNewToken(field);
				}
			}
			return field;
		}
	}

	/* Create a new field block */
	field = ILFieldCreate(classInfo, 0, name, attributes);
	if(!field)
	{
		ILAsmOutOfMemory();
	}
	ILMemberSetSignature((ILMember *)field, sig);
	return field;
}

void ILAsmAttributeCreate(ILProgramItem *type, ILIntString *value)
{
	ILAttribute *attr;
	ILProgramItem *owner;
	ILMethod *method;
	ILClass *classInfo;
	ILToken token;

	/* Create the attribute */
	attr = ILAttributeCreate(ILAsmImage, 0);
	if(!attr)
	{
		ILAsmOutOfMemory();
	}

	/* Set the attribute's type */
	if(type)
	{
		if((method = ILProgramItemToMethod(type)) != 0)
		{
			ILAttributeSetType(attr, ILToProgramItem(method));
		}
		else if((classInfo = ILProgramItemToClass(type)) != 0)
		{
			token = ILClass_Token(classInfo);
			if((token & IL_META_TOKEN_MASK) == IL_META_TOKEN_TYPE_REF ||
			   (token & IL_META_TOKEN_MASK) == IL_META_TOKEN_TYPE_DEF)
			{
				ILAttributeSetType(attr, ILToProgramItem(classInfo));
			}
			else
			{
				ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
								  "cannot use complex type specifications "
								  "as custom attribute types");
				ILAsmErrors = 1;
			}
		}
		else
		{
			ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
							  "custom attribute type is not a class or method");
			ILAsmErrors = 1;
		}
	}
	else
	{
		ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
						  "unknown type for custom attribute");
		ILAsmErrors = 1;
	}

	/* Set the value for the custom attribute */
	if(value)
	{
		if(!ILAttributeSetValue(attr, value->string, value->len))
		{
			ILAsmOutOfMemory();
		}
	}

	/* Attach the attribute to its owner */
	owner = ILProgramItem_FromToken(ILAsmImage, ILAsmLastToken);
	if(!owner)
	{
		ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
						  "unknown owner for custom attribute");
		ILAsmErrors = 1;
	}
	else
	{
		ILProgramItemAddAttribute(owner, attr);
	}
}

ILParameter *ILAsmFindParameter(ILMethod *method, ILUInt32 paramNum)
{
	ILParameter *param = 0;
	while((param = ILMethodNextParam(method, param)) != 0)
	{
		if(ILParameter_Num(param) == paramNum)
		{
			return param;
		}
	}
	ILAsmPrintMessage(ILAsmFilename, ILAsmLineNum,
					  "no parameter numbered %lu for the current method",
					  (unsigned long)paramNum);
	ILAsmErrors = 1;
	return 0;
}

void ILAsmAddSemantics(int type, ILToken token)
{
	if(token)
	{
		if(!ILMethodSemCreate(ILAsmCurrScope, 0, type,
							  ILMethod_FromToken(ILAsmImage, token)))
		{
			ILAsmOutOfMemory();
		}
	}
}

void ILAsmDebugLine(ILUInt32 line, char *filename)
{
	if(ILAsmDebugMode)
	{
		if((ILProgramItem_Token(ILAsmCurrScope) &
				IL_META_TOKEN_MASK) == IL_META_TOKEN_METHOD_DEF)
		{
			/* Debug line information within the body of a method */
		}
		else
		{
			/* Debug line information attached to some other kind of token */
		}
		ILAsmDebugLastFile = filename;
	}
}

ILClass *ILAsmSystemClass(const char *name)
{
	ILProgramItem *scope;
	ILClass *info;
	info = ILClassLookup((ILProgramItem *)ILAsmModule, name, "System");
	if(!info)
	{
		scope = (ILProgramItem *)ILAsmFindAssemblyRef("mscorlib");
		if(!scope)
		{
			/* We are assembling "mscorlib" itself, so create the
			   class within the current module */
			scope = (ILProgramItem *)ILAsmModule;
		}
		info = ILClassCreateRef(scope, 0, name, "System");
		if(!info)
		{
			ILAsmOutOfMemory();
		}
	}
	return info;
}

#ifdef	__cplusplus
};
#endif
