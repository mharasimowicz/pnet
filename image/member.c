/*
 * member.c - Process class member information from an image file.
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

#include "program.h"

#ifdef	__cplusplus
extern	"C" {
#endif

ILClass *ILMemberGetOwner(ILMember *member)
{
	return member->owner;
}

int ILMemberGetKind(ILMember *member)
{
	return (int)(member->kind);
}

const char *ILMemberGetName(ILMember *member)
{
	return member->name;
}

ILType *ILMemberGetSignature(ILMember *member)
{
	return member->signature;
}

void ILMemberSetSignature(ILMember *member, ILType *signature)
{
	member->signature = signature;
}

ILUInt32 ILMemberGetAttrs(ILMember *member)
{
	return (ILUInt32)(member->attributes);
}

void ILMemberSetAttrs(ILMember *member, ILUInt32 mask, ILUInt32 attrs)
{
	member->attributes = (ILUInt16)((member->attributes & ~mask) | attrs);
}

int ILMemberAccessible(ILMember *member, ILClass *scope)
{
	ILClass *info = member->owner;
	ILMethod *accessor;
	scope = (ILClass *)(_ILProgramItemResolve(&(scope->programItem)));
	if(!ILClassAccessible(info, scope))
	{
		return 0;
	}
	if(ILMember_IsProperty(member))
	{
		accessor = ILProperty_Getter((ILProperty *)member);
		if(!accessor)
		{
			accessor = ILProperty_Setter((ILProperty *)member);
		}
		if(!accessor)
		{
			return 0;
		}
		member = (ILMember *)accessor;
	}
	if(ILMember_IsMethod(member) || ILMember_IsField(member))
	{
		switch(member->attributes & IL_META_METHODDEF_MEMBER_ACCESS_MASK)
		{
			case IL_META_METHODDEF_COMPILER_CONTROLLED:
			{
				/* Accessible to the class and any of its nested classes */
				if(info == scope || ILClassIsNested(info, scope))
				{
					return 1;
				}
			}
			break;

			case IL_META_METHODDEF_PRIVATE:
			{
				/* Only accessible to the class itself */
				return (info == scope);
			}
			/* Not reached */

			case IL_META_METHODDEF_FAM_AND_ASSEM:
			{
				/* Scope must be in the family and the same assembly */
				if(!ILClassInheritsFrom(scope, info))
				{
					return 0;
				}
				if(scope && ILClassToImage(info) == ILClassToImage(scope))
				{
					return 1;
				}
			}
			break;

			case IL_META_METHODDEF_ASSEM:
			{
				/* Scope must be in the same assembly */
				if(scope && ILClassToImage(info) == ILClassToImage(scope))
				{
					return 1;
				}
			}
			break;

			case IL_META_METHODDEF_FAMILY:
			{
				/* Scope must be in the same family */
				return ILClassInheritsFrom(scope, info);
			}
			/* Not reached */

			case IL_META_METHODDEF_FAM_OR_ASSEM:
			{
				/* Scope must be in the same family or the same assembly */
				if(ILClassInheritsFrom(scope, info))
				{
					return 1;
				}
				if(scope && ILClassToImage(info) == ILClassToImage(scope))
				{
					return 1;
				}
			}
			break;

			case IL_META_METHODDEF_PUBLIC:
			{
				/* Accessible to everyone */
				return 1;
			}
			/* Not reached */
		}
	}
	return 0;
}

ILMember *ILMemberImport(ILImage *image, ILMember *member)
{
	ILClass *classInfo;
	ILMember *newMember;
	ILMethod *method;
	ILField *field;

	/* If the member is already in the image, then bail out */
	if(ILProgramItem_Image(member) == image)
	{
		return member;
	}

	/* Do we have a link from the member back to the required image? */
	newMember = (ILMember *)_ILProgramItemLinkedBackTo
							((ILProgramItem *)member, image);
	if(newMember)
	{
		return newMember;
	}

	/* Import the member's owner */
	classInfo = ILClassImport(image, ILMember_Owner(member));
	if(!classInfo)
	{
		return 0;
	}

	/* Create a matching declaration in the imported owner */
	if(ILMember_IsMethod(member))
	{
		/* Create a matching method declaration */
		method = ILMethodCreate(classInfo, (ILToken)IL_MAX_UINT32,
							    ILMember_Name(member),
								ILMember_Attrs(member));
		if(!method)
		{
			return 0;
		}
		ILMethodSetCallConv(method, ILMethodGetCallConv((ILMethod *)member));
		ILMemberSetSignature((ILMember *)method,
							 ILMemberGetSignature(member));
		newMember = (ILMember *)method;
	}
	else if(ILMember_IsField(member))
	{
		/* Create a matching field declaration */
		field = ILFieldCreate(classInfo, (ILToken)IL_MAX_UINT32,
							  ILMember_Name(member),
							  ILMember_Attrs(member));
		if(!field)
		{
			return 0;
		}
		ILMemberSetSignature((ILMember *)field,
							 ILMemberGetSignature(member));
		newMember = (ILMember *)field;
	}
	else
	{
		/* Shouldn't happen, but do something useful anyway */
		return member;
	}

	/* Link the reference to the original member */
	if(!_ILProgramItemLink(&(newMember->programItem),
						   &(member->programItem)))
	{
		return 0;
	}

	/* Done */
	return newMember;
}

/*
 * Common function for creating members and attaching them to a class.
 */
static ILMember *MemberCreate(ILClass *info, ILToken token,
							  ILToken tokenKind, const char *name,
							  ILUInt32 kind, ILUInt32 attributes,
							  unsigned size)
{
	ILImage *image = info->programItem.image;
	ILMember *member;

	/* Allocate space for the member from the memory stack */
	member = (ILMember *)ILMemStackAllocItem(&(image->memStack), size);
	if(!member)
	{
		return 0;
	}

	/* Convert the member name into a persistent string */
	member->name = _ILContextPersistString(image, name);
	if(!(member->name))
	{
		return 0;
	}

	/* Attach the member to its owning class */
	member->owner = info;
	member->nextMember = 0;
	if(info->lastMember)
		info->lastMember->nextMember = member;
	else
		info->firstMember = member;
	info->lastMember = member;

	/* Initialize the other member fields */
	member->programItem.image = info->programItem.image;
	member->kind = (ILUInt16)kind;
	member->attributes = (ILUInt16)attributes;
	member->signature = 0;
	member->signatureBlob = 0;

	/* Set the token for the member */
	if(token != 0 || image->type == IL_IMAGETYPE_BUILDING)
	{
		if(token == (ILToken)IL_MAX_UINT32)
		{
			/* We are creating a member reference from the assembler */
			tokenKind = IL_META_TOKEN_MEMBER_REF;
			token = 0;
		}
		if(!_ILImageSetToken(image, &(member->programItem), token, tokenKind))
		{
			return 0;
		}
	}

	/* Return the block to the caller */
	return member;
}

ILMember *ILMemberCreateRef(ILMember *member, ILToken token)
{
	ILMemberRef *ref;

	/* Initialize the common member parts of the reference */
	ref = (ILMemberRef *)(MemberCreate(member->owner, token,
									   IL_META_TOKEN_MEMBER_REF,
									   member->name,
						  			   IL_META_MEMBERKIND_REF, 0,
						  			   sizeof(ILMemberRef)));
	if(!ref)
	{
		return 0;
	}

	/* Initialize the other reference fields */
	ref->ref = member;

	/* Return the block to the caller */
	return (ILMember *)ref;
}

ILMember *ILMemberResolveRef(ILMember *member)
{
	if(member->kind == IL_META_MEMBERKIND_REF)
	{
		return ((ILMemberRef *)member)->ref;
	}
	else
	{
		return member;
	}
}

ILMember *ILMemberResolve(ILMember *member)
{
	return (ILMember *)_ILProgramItemResolve(&(member->programItem));
}

void _ILMemberSetSignatureIndex(ILMember *member, ILUInt32 index)
{
	member->signatureBlob = index;
}

ILMethod *ILMethodCreate(ILClass *info, ILToken token,
						 const char *name, ILUInt32 attributes)
{
	ILMethod *method;

	/* Initialize the common member parts of the method block */
	method = (ILMethod *)(MemberCreate(info, token,
									   IL_META_TOKEN_METHOD_DEF, name,
						  			   IL_META_MEMBERKIND_METHOD, attributes,
						  			   sizeof(ILMethod)));
	if(!method)
	{
		return 0;
	}

	/* Initialize the other method fields */
	method->implementAttrs = 0;
	method->callingConventions = 0;
	method->rva = 0;
	method->parameters = 0;

	/* Return the block to the caller */
	return method;
}

int ILMethodNewToken(ILMethod *method)
{
	return _ILImageSetToken(method->member.programItem.image,
						    &(method->member.programItem),
						    0, IL_META_TOKEN_METHOD_DEF);
}

ILUInt32 ILMethodGetImplAttrs(ILMethod *method)
{
	return method->implementAttrs;
}

void ILMethodSetImplAttrs(ILMethod *method, ILUInt32 mask, ILUInt32 attrs)
{
	method->implementAttrs =
		(ILUInt16)((method->implementAttrs & ~mask) | attrs);
}

ILUInt32 ILMethodGetCallConv(ILMethod *method)
{
	return method->callingConventions;
}

void ILMethodSetCallConv(ILMethod *method, ILUInt32 callConv)
{
	method->callingConventions = callConv;
}

ILUInt32 ILMethodGetRVA(ILMethod *method)
{
	return method->rva;
}

void ILMethodSetRVA(ILMethod *method, ILUInt32 rva)
{
	method->rva = rva;
}

ILParameter *ILMethodNextParam(ILMethod *method, ILParameter *last)
{
	if(last)
	{
		return last->next;
	}
	else
	{
		return method->parameters;
	}
}

int ILMethodGetCode(ILMethod *method, ILMethodCode *code)
{
	unsigned char *addr;
	unsigned long len;
	ILToken localVars;

	/* Bail out if the method does not have any IL code */
	if(!(method->rva))
	{
		return 0;
	}
	addr = (unsigned char *)ILImageMapRVA(method->member.programItem.image,
										  method->rva, &len);
	if(!addr || !len)
	{
		return 0;
	}

	/* Determine whether the method header is tiny or fat format */
	if((*addr & 0x03) == 0x02)
	{
		/* Tiny format header */
		code->codeLen = (ILUInt32)((*addr >> 2) & 0x3F);
		code->maxStack = 8;
		code->headerSize = 1;
		code->localVarSig = 0;
		code->initLocals = 0;
		code->moreSections = 0;
	}
	else if((*addr & 0x07) == 0x03)
	{
		/* Fat format header */
		if(len < 12)
		{
			return 0;
		}
		code->moreSections = ((*addr & 0x08) != 0);
		code->initLocals = ((*addr & 0x10) != 0);
		code->maxStack = (ILUInt32)(IL_READ_UINT16(addr + 2));
		code->codeLen = (ILUInt32)(IL_READ_UINT32(addr + 4));
		localVars = (ILToken)(IL_READ_UINT32(addr + 8));
		if(localVars)
		{
			if((localVars & IL_META_TOKEN_MASK) !=
					IL_META_TOKEN_STAND_ALONE_SIG)
			{
				return 0;
			}
			code->localVarSig = ILStandAloneSig_FromToken
					(method->member.programItem.image, localVars);
			if(!(code->localVarSig) ||
			   !ILStandAloneSigIsLocals(code->localVarSig))
			{
				return 0;
			}
		}
		else
		{
			code->localVarSig = 0;
		}
		code->headerSize = ((ILUInt32)((addr[1] >> 4) & 0x0F)) * 4;
		if(code->headerSize < 12)
		{
			return 0;
		}
	}
	else
	{
		/* Invalid method header */
		return 0;
	}
	addr += code->headerSize;
	len -= code->headerSize;

	/* Extract the code area */
	if(!(code->codeLen) || code->codeLen > len)
	{
		return 0;
	}
	code->code = (void *)addr;
	addr += code->codeLen;
	len -= code->codeLen;

	/* Remember the remaining space for later exception handling */
	code->remaining = (ILUInt32)len;

	/* Ready to go */
	return 1;
}

int ILMethodGetExceptions(ILMethod *method, ILMethodCode *code,
						  ILException **exceptions)
{
	ILException *lastException;
	ILException *newException;
	int moreSections;
	unsigned char *addr;
	unsigned long len;
	unsigned long rva;
	unsigned long adjust;
	unsigned long sectSize;
	unsigned long posn;
	int kind, isTiny;

	/* Initialize the exception list */
	*exceptions = 0;
	lastException = 0;

	/* Determine the address of the end of the method code */
	addr = ((unsigned char *)(code->code)) + code->codeLen;
	rva = method->rva + code->headerSize + code->codeLen;
	len = code->remaining;

	/* Read all of the method's sections.  A truncated section
	   is treated as the end of the list */
	moreSections = code->moreSections;
	while(moreSections)
	{
		/* Align the section on the next DWORD boundary */
		if((rva & 3) != 0)
		{
			adjust = 4 - (rva & 3);
			if(adjust > len)
			{
				break;
			}
			addr += adjust;
			len -= adjust;
			rva += adjust;
		}

		/* Read the section header.  Note: early versions of the
		   ECMA specs say that "sectSize" does not include the size
		   of the section header.  This is incorrect. */
		if(len < 4)
		{
			break;
		}
		kind = *addr;
		moreSections = ((kind & 0x80) != 0);
		if((kind & 0x40) == 0)
		{
			/* Tiny format section header */
			sectSize = (((unsigned long)(addr[1])) & 0xFF);
			isTiny = 1;
		}
		else
		{
			/* Fat format section header */
			sectSize = (((unsigned long)(addr[1])) & 0xFF) |
			          ((((unsigned long)(addr[2])) & 0xFF) << 8) |
			          ((((unsigned long)(addr[3])) & 0xFF) << 16);
			isTiny = 0;
		}
		if(sectSize < 4 || len < sectSize)
		{
			break;
		}
		kind &= 0x3F;
		addr += 4;
		len -= 4;
		rva += 4;
		sectSize -= 4;

		/* Parse any exception clauses within the section */
		if(kind == 0x01 && isTiny)
		{
			/* Tiny format exception clauses */
			for(posn = 0; (posn + 12) <= sectSize; posn += 12)
			{
				newException = (ILException *)ILMalloc(sizeof(ILException));
				if(!newException)
				{
					ILMethodFreeExceptions(*exceptions);
					return 0;
				}
				newException->flags =
					(ILUInt32)(IL_READ_UINT16(addr + posn));
				newException->tryOffset =
					(ILUInt32)(IL_READ_UINT16(addr + posn + 2));
				newException->tryLength =
					(((ILUInt32)(addr[posn + 4])) & (ILUInt32)0xFF);
				newException->handlerOffset =
					(ILUInt32)(IL_READ_UINT16(addr + posn + 5));
				newException->handlerLength =
					(((ILUInt32)(addr[posn + 7])) & (ILUInt32)0xFF);
				newException->extraArg = IL_READ_UINT32(addr + posn + 8);
				newException->next = 0;
				if(lastException)
				{
					lastException->next = newException;
				}
				else
				{
					*exceptions = newException;
				}
				lastException = newException;
			}
		}
		else if(kind == 0x01 && !isTiny)
		{
			/* Fat format exception clauses */
			for(posn = 0; (posn + 24) <= sectSize; posn += 24)
			{
				newException = (ILException *)ILMalloc(sizeof(ILException));
				if(!newException)
				{
					ILMethodFreeExceptions(*exceptions);
					return 0;
				}
				newException->flags = IL_READ_UINT32(addr + posn);
				newException->tryOffset = IL_READ_UINT32(addr + posn + 4);
				newException->tryLength = IL_READ_UINT32(addr + posn + 8);
				newException->handlerOffset = IL_READ_UINT32(addr + posn + 12);
				newException->handlerLength = IL_READ_UINT32(addr + posn + 16);
				newException->extraArg = IL_READ_UINT32(addr + posn + 20);
				newException->next = 0;
				if(lastException)
				{
					lastException->next = newException;
				}
				else
				{
					*exceptions = newException;
				}
				lastException = newException;
			}
		}

		/* Advance to the next section */
		addr += sectSize;
		len -= sectSize;
		rva += sectSize;
	}

	/* Done */
	return 1;
}

void ILMethodFreeExceptions(ILException *exceptions)
{
	ILException *next;
	while(exceptions != 0)
	{
		next = exceptions->next;
		ILFree(exceptions);
		exceptions = next;
	}
}

void ILMethodSetUserData(ILMethod *method, void *userData1, void *userData2)
{
	method->userData1 = userData1;
	method->userData2 = userData2;
}

void *ILMethodGetUserData1(ILMethod *method)
{
	return method->userData1;
}

void *ILMethodGetUserData2(ILMethod *method)
{
	return method->userData2;
}

int ILMethodIsConstructor(ILMethod *method)
{
	if(!strcmp(method->member.name, ".ctor") &&
	   (method->member.attributes & IL_META_METHODDEF_RT_SPECIAL_NAME) != 0 &&
	   (method->member.attributes & IL_META_METHODDEF_VIRTUAL) == 0 &&
	   ILType_HasThis(method->member.signature) &&
	   method->member.signature->un.method.retType == ILType_Void)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int ILMethodIsStaticConstructor(ILMethod *method)
{
	if(!strcmp(method->member.name, ".cctor") &&
	   (method->member.attributes & IL_META_METHODDEF_RT_SPECIAL_NAME) != 0 &&
	   (method->member.attributes & IL_META_METHODDEF_VIRTUAL) == 0 &&
	   !ILType_HasThis(method->member.signature) &&
	   method->member.signature->un.method.retType == ILType_Void &&
	   method->member.signature->num == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

ILParameter *ILParameterCreate(ILMethod *method, ILToken token,
							   const char *name, ILUInt32 attributes,
							   ILUInt32 paramNum)
{
	ILImage *image = method->member.owner->programItem.image;
	ILParameter *param;
	ILParameter *current;
	ILParameter *prev;

	/* Allocate space for the parameter from the memory stack */
	param = ILMemStackAlloc(&(image->memStack), ILParameter);
	if(!param)
	{
		return 0;
	}

	/* Convert the parameter name into a persistent string */
	param->name = _ILContextPersistString(image, name);
	if(!(param->name))
	{
		return 0;
	}

	/* Assign a token code to the parameter */
	if(!_ILImageSetToken(image, &(param->programItem),
						 token, IL_META_TOKEN_PARAM_DEF))
	{
		return 0;
	}

	/* Set the other parameter fields */
	param->programItem.image = image;
	param->attributes = (ILUInt16)attributes;
	param->paramNum = (ILUInt16)paramNum;

	/* Attach the parameter to the method */
	current = method->parameters;
	prev = 0;
	while(current != 0 && current->paramNum < param->paramNum)
	{
		prev = current;
		current = current->next;
	}
	param->next = current;
	if(prev)
	{
		prev->next = param;
	}
	else
	{
		method->parameters = param;
	}

	/* Return the parameter to the caller */
	return param;
}

const char *ILParameterGetName(ILParameter *param)
{
	return param->name;
}

ILUInt32 ILParameterGetNum(ILParameter *param)
{
	return param->paramNum;
}

ILUInt32 ILParameterGetAttrs(ILParameter *param)
{
	return param->attributes;
}

void ILParameterSetAttrs(ILParameter *param, ILUInt32 mask, ILUInt32 attrs)
{
	param->attributes = (ILUInt16)((param->attributes & ~mask) | attrs);
}

ILField *ILFieldCreate(ILClass *info, ILToken token,
					   const char *name, ILUInt32 attributes)
{
	return (ILField *)(MemberCreate(info, token,
								    IL_META_TOKEN_FIELD_DEF, name,
								    IL_META_MEMBERKIND_FIELD, attributes,
								    sizeof(ILField)));
}

int ILFieldNewToken(ILField *field)
{
	return _ILImageSetToken(field->member.programItem.image,
						    &(field->member.programItem),
						    0, IL_META_TOKEN_FIELD_DEF);
}

ILEvent *ILEventCreate(ILClass *info, ILToken token,
					   const char *name, ILUInt32 attributes,
					   ILClass *type)
{
	ILEvent *event;
	
	/* Initialize the common member parts of the event block */
	event = (ILEvent *)(MemberCreate(info, token,
									 IL_META_TOKEN_EVENT, name,
								     IL_META_MEMBERKIND_EVENT, attributes,
								     sizeof(ILEvent)));
	if(!event)
	{
		return 0;
	}

	/* Initialize the other event fields */
	event->member.signature = (type ? ILType_FromClass(type) : ILType_Invalid);
	event->semantics = 0;

	/* Return the block to the caller */
	return event;
}

ILMethod *ILEventGetAddOn(ILEvent *event)
{
	return ILMethodSemGetByType(&(event->member.programItem),
								IL_META_METHODSEM_ADD_ON);
}

ILMethod *ILEventGetRemoveOn(ILEvent *event)
{
	return ILMethodSemGetByType(&(event->member.programItem),
								IL_META_METHODSEM_REMOVE_ON);
}

ILMethod *ILEventGetFire(ILEvent *event)
{
	return ILMethodSemGetByType(&(event->member.programItem),
								IL_META_METHODSEM_FIRE);
}

ILMethod *ILEventGetOther(ILEvent *event)
{
	return ILMethodSemGetByType(&(event->member.programItem),
								IL_META_METHODSEM_OTHER);
}

ILProperty *ILPropertyCreate(ILClass *info, ILToken token,
					   		 const char *name, ILUInt32 attributes,
							 ILType *signature)
{
	ILProperty *property;

	/* Initialize the common member parts of the property block */
	property = (ILProperty *)(MemberCreate(info, token,
									 	   IL_META_TOKEN_PROPERTY, name,
						  			       IL_META_MEMBERKIND_PROPERTY,
										   attributes,
						  			  	   sizeof(ILProperty)));
	if(!property)
	{
		return 0;
	}

	/* Initialize the other property fields */
	property->member.signature = signature;
	property->semantics = 0;

	/* Return the block to the caller */
	return property;
}

ILMethod *ILPropertyGetGetter(ILProperty *property)
{
	return ILMethodSemGetByType(&(property->member.programItem),
								IL_META_METHODSEM_GETTER);
}

ILMethod *ILPropertyGetSetter(ILProperty *property)
{
	return ILMethodSemGetByType(&(property->member.programItem),
								IL_META_METHODSEM_SETTER);
}

ILMethod *ILPropertyGetOther(ILProperty *property)
{
	return ILMethodSemGetByType(&(property->member.programItem),
								IL_META_METHODSEM_OTHER);
}

ILPInvoke *ILPInvokeCreate(ILMethod *method, ILToken token,
						   ILUInt32 attributes, ILModule *module,
						   const char *aliasName)
{
	ILClass *info = method->member.owner;
	ILImage *image = info->programItem.image;
	ILPInvoke *pinvoke;

	/* Initialize the common member parts of the PInvoke block */
	pinvoke = (ILPInvoke *)(MemberCreate(info, token, IL_META_TOKEN_IMPL_MAP,
										 method->member.name,
						  			     IL_META_MEMBERKIND_PINVOKE,
										 attributes,
						  			  	 sizeof(ILPInvoke)));
	if(!pinvoke)
	{
		return 0;
	}

	/* Initialize the other PInvoke fields */
	pinvoke->method = method;
	pinvoke->module = module;
	if(aliasName && aliasName[0] != '\0')
	{
		pinvoke->aliasName = _ILContextPersistString(image, aliasName);
		if(!(pinvoke->aliasName))
		{
			return 0;
		}
	}
	else
	{
		pinvoke->aliasName = method->member.name;
	}

	/* Return the block to the caller */
	return pinvoke;
}

ILMethod *ILPInvokeGetMethod(ILPInvoke *pinvoke)
{
	return pinvoke->method;
}

ILModule *ILPInvokeGetModule(ILPInvoke *pinvoke)
{
	return pinvoke->module;
}

const char *ILPInvokeGetAlias(ILPInvoke *pinvoke)
{
	return pinvoke->aliasName;
}

ILPInvoke *ILPInvokeFind(ILMethod *method)
{
	ILMember *member = method->member.nextMember;
	while(member != 0)
	{
		if(member->kind == IL_META_MEMBERKIND_PINVOKE &&
		   ((ILPInvoke *)member)->method == method)
		{
			return ((ILPInvoke *)member);
		}
		member = member->nextMember;
	}
	return 0;
}

ILOverride *ILOverrideCreate(ILClass *info, ILToken token,
					   		 ILMethod *decl, ILMethod *body)
{
	ILOverride *over;

	/* Initialize the common member parts of the override block */
	over = (ILOverride *)(MemberCreate(info, token,
									   IL_META_TOKEN_METHOD_IMPL,
									   decl->member.name,
					  			       IL_META_MEMBERKIND_OVERRIDE,
									   0, sizeof(ILOverride)));
	if(!over)
	{
		return 0;
	}

	/* Initialize the other override fields */
	over->member.signature = ILMethod_Signature(decl);
	over->decl = decl;
	over->body = body;

	/* Return the block to the caller */
	return over;
}

ILMethod *ILOverrideGetDecl(ILOverride *over)
{
	return over->decl;
}

ILMethod *ILOverrideGetBody(ILOverride *over)
{
	return over->body;
}

ILOverride *ILOverrideFromMethod(ILMethod *method)
{
	ILMember *member = 0;
	while((member = ILClassNextMemberByKind(method->member.owner, member,
											IL_META_MEMBERKIND_OVERRIDE)) != 0)
	{
		if(((ILOverride *)member)->body == method)
		{
			return ((ILOverride *)member);
		}
	}
	return 0;
}

ILEventMap *ILEventMapCreate(ILImage *image, ILToken token,
							 ILClass *info, ILEvent *firstEvent)
{
	ILEventMap *map;

	/* Allocate space for the EventMap block from the memory stack */
	map = ILMemStackAlloc(&(image->memStack), ILEventMap);
	if(!map)
	{
		return 0;
	}

	/* Set the EventMap information fields */
	map->programItem.image = image;
	map->classInfo = info;
	map->firstEvent = firstEvent;

	/* Assign a token code to the EventMap information block */
	if(!_ILImageSetToken(image, &(map->programItem), token,
						 IL_META_TOKEN_EVENT_MAP))
	{
		return 0;
	}

	/* Return the EventMap information block to the caller */
	return map;
}

ILClass *ILEventMapGetClass(ILEventMap *map)
{
	return map->classInfo;
}

ILEvent *ILEventMapGetEvent(ILEventMap *map)
{
	return map->firstEvent;
}

ILPropertyMap *ILPropertyMapCreate(ILImage *image, ILToken token,
							 	   ILClass *info, ILProperty *firstProperty)
{
	ILPropertyMap *map;

	/* Allocate space for the PropertyMap block from the memory stack */
	map = ILMemStackAlloc(&(image->memStack), ILPropertyMap);
	if(!map)
	{
		return 0;
	}

	/* Set the PropertyMap information fields */
	map->programItem.image = image;
	map->classInfo = info;
	map->firstProperty = firstProperty;

	/* Assign a token code to the PropertyMap information block */
	if(!_ILImageSetToken(image, &(map->programItem), token,
						 IL_META_TOKEN_PROPERTY_MAP))
	{
		return 0;
	}

	/* Return the PropertyMap information block to the caller */
	return map;
}

ILClass *ILPropertyMapGetClass(ILPropertyMap *map)
{
	return map->classInfo;
}

ILProperty *ILPropertyMapGetProperty(ILPropertyMap *map)
{
	return map->firstProperty;
}

ILMethodSem *ILMethodSemCreate(ILProgramItem *item, ILToken token,
							   ILUInt32 type, ILMethod *method)
{
	ILMethodSem *sem;
	ILProperty *property;
	ILEvent *event;

	/* Allocate space for the MethodSem block from the memory stack */
	sem = ILMemStackAlloc(&(item->image->memStack), ILMethodSem);
	if(!sem)
	{
		return 0;
	}

	/* Set the MethodSem information fields */
	sem->programItem.image = item->image;
	sem->owner = item;
	sem->type = type;
	sem->method = method;

	/* Assign a token code to the MethodSem information block */
	if(!_ILImageSetToken(item->image, &(sem->programItem), token,
						 IL_META_TOKEN_METHOD_SEMANTICS))
	{
		return 0;
	}

	/* Attach this record to the property or event */
	if((property = ILProgramItemToProperty(item)) != 0)
	{
		sem->next = property->semantics;
		property->semantics = sem;
	}
	else if((event = ILProgramItemToEvent(item)) != 0)
	{
		sem->next = event->semantics;
		event->semantics = sem;
	}

	/* Return the MethodSem information block to the caller */
	return sem;
}

ILEvent *ILMethodSemGetEvent(ILMethodSem *sem)
{
	return ILProgramItemToEvent(sem->owner);
}

ILProperty *ILMethodSemGetProperty(ILMethodSem *sem)
{
	return ILProgramItemToProperty(sem->owner);
}

ILUInt32 ILMethodSemGetType(ILMethodSem *sem)
{
	return sem->type;
}

ILMethod *ILMethodSemGetMethod(ILMethodSem *sem)
{
	return sem->method;
}

ILMethod *ILMethodSemGetByType(ILProgramItem *item, ILUInt32 type)
{
	ILMethodSem *sem;
	ILProperty *property;
	ILEvent *event;
	if((property = ILProgramItemToProperty(item)) != 0)
	{
		sem = property->semantics;
	}
	else if((event = ILProgramItemToEvent(item)) != 0)
	{
		sem = event->semantics;
	}
	else
	{
		sem = 0;
	}
	while(sem != 0)
	{
		if(sem->type == type)
		{
			return sem->method;
		}
		sem = sem->next;
	}
	return 0;
}

#ifdef	__cplusplus
};
#endif
