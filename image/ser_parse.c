/*
 * ser_parse.c - Parse serialized attribute values.
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

#include "il_serialize.h"
#include "program.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Internal structure of a serialization reader.
 */
struct _tagILSerializeReader
{
	ILMetaDataRead		meta;
	ILClass			   *info;
	ILType			   *signature;
	ILUInt32			param;

};

ILSerializeReader *ILSerializeReaderInit(ILMethod *method,
									     const void *blob,
										 unsigned long len)
{
	ILSerializeReader *reader;

	/* Resolve the method to it final class, so that we can
	   obtain the necessary field and property information */
	method = (ILMethod *)ILMemberResolve((ILMember *)method);

	/* Initialize the reader */
	if((reader = (ILSerializeReader *)ILMalloc(sizeof(ILSerializeReader))) == 0)
	{
		return 0;
	}
	reader->meta.data = (const unsigned char *)blob;
	reader->meta.len = len;
	reader->meta.error = 0;
	reader->info = method->member.owner;
	reader->signature = method->member.signature;
	reader->param = 0;

	/* Check the blob header */
	if(reader->meta.len < 2 || IL_READ_UINT16(reader->meta.data) != 1)
	{
		return 0;
	}
	reader->meta.data += 2;
	reader->meta.len -= 2;

	/* Ready to go */
	return reader;
}

void ILSerializeReaderDestroy(ILSerializeReader *reader)
{
	ILFree(reader);
}

int ILSerializeGetType(ILType *type)
{
	ILClass *classInfo;
	int elemType;

	/* Resolve enumerated type references to get the underlying type */
	type = ILTypeGetEnumType(type);

	/* Determine how to serialize the value */
	if(ILType_IsPrimitive(type))
	{
		/* Determine the primitive serialization type */
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_BOOLEAN:	return IL_META_SERIALTYPE_BOOLEAN;
			case IL_META_ELEMTYPE_I1:		return IL_META_SERIALTYPE_I1;
			case IL_META_ELEMTYPE_U1:		return IL_META_SERIALTYPE_U1;
			case IL_META_ELEMTYPE_I2:		return IL_META_SERIALTYPE_I2;
			case IL_META_ELEMTYPE_U2:		return IL_META_SERIALTYPE_U2;
			case IL_META_ELEMTYPE_CHAR:		return IL_META_SERIALTYPE_CHAR;
			case IL_META_ELEMTYPE_I4:		return IL_META_SERIALTYPE_I4;
			case IL_META_ELEMTYPE_U4:		return IL_META_SERIALTYPE_U4;
			case IL_META_ELEMTYPE_I8:		return IL_META_SERIALTYPE_I8;
			case IL_META_ELEMTYPE_U8:		return IL_META_SERIALTYPE_U8;
			case IL_META_ELEMTYPE_R4:		return IL_META_SERIALTYPE_R4;
			case IL_META_ELEMTYPE_R8:		return IL_META_SERIALTYPE_R8;
			default:						break;
		}
	}
	else if(ILType_IsClass(type))
	{
		/* Check for "System.String" and "System.Type" */
		classInfo = ILType_ToClass(type);
		if(!strcmp(classInfo->name, "String"))
		{
			if(classInfo->namespace &&
			   !strcmp(classInfo->namespace, "System") &&
			   ILClassGetNestedParent(classInfo) == 0)
			{
				return IL_META_SERIALTYPE_STRING;
			}
		}
		else if(!strcmp(classInfo->name, "Type"))
		{
			if(classInfo->namespace &&
			   !strcmp(classInfo->namespace, "System") &&
			   ILClassGetNestedParent(classInfo) == 0)
			{
				return IL_META_SERIALTYPE_TYPE;
			}
		}
	}
	else if(type != 0 && ILType_IsComplex(type) &&
			type->kind == IL_TYPE_COMPLEX_ARRAY &&
			type->un.array.lowBound == 0)
	{
		/* Determine if this is an array of simple types */
		elemType = ILSerializeGetType(type->un.array.elemType);
		if(elemType != -1 && (elemType & IL_META_SERIALTYPE_ARRAYOF) == 0)
		{
			return (IL_META_SERIALTYPE_ARRAYOF | elemType);
		}
	}

	/* The type is not serializable */
	return -1;
}

/*
 * Determine if there is sufficient data for a serialized value.
 */
static int HasSufficientSpace(ILSerializeReader *reader, int type)
{
	ILInt32 length;

	switch(type)
	{
		case IL_META_SERIALTYPE_BOOLEAN:
		case IL_META_SERIALTYPE_I1:
		case IL_META_SERIALTYPE_U1:
		{
			return (reader->meta.len >= 1);
		}
		/* Not reached */

		case IL_META_SERIALTYPE_I2:
		case IL_META_SERIALTYPE_U2:
		case IL_META_SERIALTYPE_CHAR:
		{
			return (reader->meta.len >= 2);
		}
		/* Not reached */

		case IL_META_SERIALTYPE_I4:
		case IL_META_SERIALTYPE_U4:
		case IL_META_SERIALTYPE_R4:
		{
			return (reader->meta.len >= 4);
		}
		/* Not reached */

		case IL_META_SERIALTYPE_I8:
		case IL_META_SERIALTYPE_U8:
		case IL_META_SERIALTYPE_R8:
		{
			return (reader->meta.len >= 8);
		}
		/* Not reached */

		case IL_META_SERIALTYPE_STRING:
		case IL_META_SERIALTYPE_TYPE:
		{
			/* Assume that space is sufficient, and check for real later */
			return 1;
		}
		/* Not reached */

		case IL_META_SERIALTYPE_ARRAYOF | IL_META_SERIALTYPE_BOOLEAN:
		case IL_META_SERIALTYPE_ARRAYOF | IL_META_SERIALTYPE_I1:
		case IL_META_SERIALTYPE_ARRAYOF | IL_META_SERIALTYPE_U1:
		{
			if(reader->meta.len < 4)
			{
				return 0;
			}
			length = IL_READ_INT32(reader->meta.data);
			if(length < 0 ||
			   ((unsigned long)length) > (reader->meta.len - 4))
			{
				return 0;
			}
			return 1;
		}
		/* Not reached */

		case IL_META_SERIALTYPE_ARRAYOF | IL_META_SERIALTYPE_I2:
		case IL_META_SERIALTYPE_ARRAYOF | IL_META_SERIALTYPE_U2:
		case IL_META_SERIALTYPE_ARRAYOF | IL_META_SERIALTYPE_CHAR:
		{
			if(reader->meta.len < 4)
			{
				return 0;
			}
			length = IL_READ_INT32(reader->meta.data);
			if(length < 0 ||
			   ((unsigned long)length) > ((reader->meta.len - 4) / 2))
			{
				return 0;
			}
			return 1;
		}
		/* Not reached */

		case IL_META_SERIALTYPE_ARRAYOF | IL_META_SERIALTYPE_I4:
		case IL_META_SERIALTYPE_ARRAYOF | IL_META_SERIALTYPE_U4:
		case IL_META_SERIALTYPE_ARRAYOF | IL_META_SERIALTYPE_R4:
		{
			if(reader->meta.len < 4)
			{
				return 0;
			}
			length = IL_READ_INT32(reader->meta.data);
			if(length < 0 ||
			   ((unsigned long)length) > ((reader->meta.len - 4) / 4))
			{
				return 0;
			}
			return 1;
		}
		/* Not reached */

		case IL_META_SERIALTYPE_ARRAYOF | IL_META_SERIALTYPE_I8:
		case IL_META_SERIALTYPE_ARRAYOF | IL_META_SERIALTYPE_U8:
		case IL_META_SERIALTYPE_ARRAYOF | IL_META_SERIALTYPE_R8:
		{
			if(reader->meta.len < 4)
			{
				return 0;
			}
			length = IL_READ_INT32(reader->meta.data);
			if(length < 0 ||
			   ((unsigned long)length) > ((reader->meta.len - 4) / 8))
			{
				return 0;
			}
			return 1;
		}
		/* Not reached */

		case IL_META_SERIALTYPE_ARRAYOF | IL_META_SERIALTYPE_STRING:
		case IL_META_SERIALTYPE_ARRAYOF | IL_META_SERIALTYPE_TYPE:
		{
			/* Assume that space is sufficient if we have a positive
			   length value, and check for real later */
			if(reader->meta.len < 4)
			{
				return 0;
			}
			length = IL_READ_INT32(reader->meta.data);
			return (length >= 0);
		}
		/* Not reached */
	}

	return 0;
}

int ILSerializeReaderGetParamType(ILSerializeReader *reader)
{
	int type;
	if(reader->param < ((ILUInt32)(reader->signature->num)))
	{
		++(reader->param);
		type = ILSerializeGetType(ILTypeGetParam(reader->signature,
												 reader->param));
		if(HasSufficientSpace(reader, type))
		{
			return type;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return 0;
	}
}

ILInt32 ILSerializeReaderGetInt32(ILSerializeReader *reader, int type)
{
	ILInt32 value;
	switch(type)
	{
		case IL_META_SERIALTYPE_BOOLEAN:
		case IL_META_SERIALTYPE_I1:
		{
			value = (ILInt32)(*((ILInt8 *)(reader->meta.data)));
			++(reader->meta.data);
			--(reader->meta.len);
			return value;
		}
		/* Not reached */

		case IL_META_SERIALTYPE_U1:
		{
			value = (ILInt32)(*((ILUInt8 *)(reader->meta.data)));
			++(reader->meta.data);
			--(reader->meta.len);
			return value;
		}
		/* Not reached */

		case IL_META_SERIALTYPE_I2:
		{
			value = (ILInt32)(IL_READ_INT16(reader->meta.data));
			reader->meta.data += 2;
			reader->meta.len -= 2;
			return value;
		}
		/* Not reached */

		case IL_META_SERIALTYPE_U2:
		case IL_META_SERIALTYPE_CHAR:
		{
			value = (ILInt32)(IL_READ_UINT16(reader->meta.data));
			reader->meta.data += 2;
			reader->meta.len -= 2;
			return value;
		}
		/* Not reached */

		default: break;
	}
	value = IL_READ_INT32(reader->meta.data);
	reader->meta.data += 4;
	reader->meta.len -= 4;
	return value;
}

ILUInt32 ILSerializeReaderGetUInt32(ILSerializeReader *reader, int type)
{
	ILInt32 value;
	switch(type)
	{
		case IL_META_SERIALTYPE_BOOLEAN:
		case IL_META_SERIALTYPE_I1:
		{
			value = (ILUInt32)(ILInt32)(*((ILInt8 *)(reader->meta.data)));
			++(reader->meta.data);
			--(reader->meta.len);
			return value;
		}
		/* Not reached */

		case IL_META_SERIALTYPE_U1:
		{
			value = (ILUInt32)(*((ILUInt8 *)(reader->meta.data)));
			++(reader->meta.data);
			--(reader->meta.len);
			return value;
		}
		/* Not reached */

		case IL_META_SERIALTYPE_I2:
		{
			value = (ILUInt32)(ILInt32)(IL_READ_INT16(reader->meta.data));
			reader->meta.data += 2;
			reader->meta.len -= 2;
			return value;
		}
		/* Not reached */

		case IL_META_SERIALTYPE_U2:
		case IL_META_SERIALTYPE_CHAR:
		{
			value = (ILUInt32)(IL_READ_UINT16(reader->meta.data));
			reader->meta.data += 2;
			reader->meta.len -= 2;
			return value;
		}
		/* Not reached */

		default: break;
	}
	value = IL_READ_UINT32(reader->meta.data);
	reader->meta.data += 4;
	reader->meta.len -= 4;
	return value;
}

ILInt64 ILSerializeReaderGetInt64(ILSerializeReader *reader)
{
	ILInt64 value = IL_READ_INT64(reader->meta.data);
	reader->meta.data += 8;
	reader->meta.len -= 8;
	return value;
}

ILUInt64 ILSerializeReaderGetUInt64(ILSerializeReader *reader)
{
	ILUInt64 value = IL_READ_UINT64(reader->meta.data);
	reader->meta.data += 8;
	reader->meta.len -= 8;
	return value;
}

ILFloat ILSerializeReaderGetFloat32(ILSerializeReader *reader)
{
	ILFloat value = IL_READ_FLOAT(reader->meta.data);
	reader->meta.data += 4;
	reader->meta.len -= 4;
	return value;
}

ILDouble ILSerializeReaderGetFloat64(ILSerializeReader *reader)
{
	ILDouble value = IL_READ_DOUBLE(reader->meta.data);
	reader->meta.data += 8;
	reader->meta.len -= 8;
	return value;
}

int ILSerializeReaderGetString(ILSerializeReader *reader, const char **str)
{
	unsigned long length = ILMetaUncompressData(&(reader->meta));
	if(reader->meta.error || length > reader->meta.len)
	{
		return -1;
	}
	*str = (const char *)(reader->meta.data);
	reader->meta.data += length;
	reader->meta.len -= length;
	return (int)length;
}

ILInt32 ILSerializeReaderGetArrayLen(ILSerializeReader *reader)
{
	ILInt32 value = IL_READ_INT32(reader->meta.data);
	reader->meta.data += 4;
	reader->meta.len -= 4;
	return value;
}

int ILSerializeReaderGetNumExtra(ILSerializeReader *reader)
{
	int value;
	if(reader->meta.len < 2)
	{
		return -1;
	}
	value = (int)(IL_READ_UINT16(reader->meta.data));
	reader->meta.data += 2;
	reader->meta.len -= 2;
	return value;
}

int ILSerializeReaderGetExtra(ILSerializeReader *reader,
							  ILMember **memberReturn,
							  const char **nameReturn,
							  int *nameLenReturn)
{
	int type;
	ILClass *info;
	ILMember *member;
	const char *name;
	int nameLen;
	ILMethod *setter;
	ILType *memberType;

	/* Get the type of extra data (field or property) */
	if(reader->meta.len < 1)
	{
		return -1;
	}
	type = (int)(*(reader->meta.data));
	reader->meta.data += 1;
	reader->meta.len -= 1;
	if(type != IL_META_SERIALTYPE_FIELD &&
	   type != IL_META_SERIALTYPE_PROPERTY)
	{
		return -1;
	}

	/* Get the member's name */
	nameLen = ILSerializeReaderGetString(reader, &name);
	if(nameLen == -1)
	{
		return -1;
	}

	/* Search for the field or property within the class and its ancestors */
	if(type == IL_META_SERIALTYPE_FIELD)
	{
		type = IL_META_MEMBERKIND_FIELD;
	}
	else
	{
		type = IL_META_MEMBERKIND_PROPERTY;
	}
	info = reader->info;
	member = 0;
	while(info != 0)
	{
		while((member = ILClassNextMemberByKind(info, member, type)) != 0)
		{
			if(!strncmp(member->name, name, nameLen) &&
			   member->name[nameLen] == '\0')
			{
				break;
			}
		}
		if(member != 0)
		{
			break;
		}
		info = ILClassGetParent(info);
	}

	/* The member must have public access and be an instance member.
	   If it is a property, then it must also have a setter */
	if(member != 0)
	{
		if(type == IL_META_MEMBERKIND_FIELD)
		{
			if((member->attributes & IL_META_FIELDDEF_FIELD_ACCESS_MASK)
					!= IL_META_FIELDDEF_PUBLIC ||
			   (member->attributes & IL_META_FIELDDEF_STATIC) != 0)
			{
				return -1;
			}
			memberType = member->signature;
		}
		else
		{
			setter = ILPropertyGetSetter((ILProperty *)member);
			if(!setter)
			{
				return -1;
			}
			if((setter->member.attributes &
							IL_META_METHODDEF_MEMBER_ACCESS_MASK)
					!= IL_META_METHODDEF_PUBLIC ||
			   (setter->member.attributes & IL_META_METHODDEF_STATIC) != 0)
			{
				return -1;
			}
			memberType = member->signature->un.method.retType;
		}

		/* Convert the member type into a serialization type */
		type = ILSerializeGetType(memberType);
		if(type == -1 || (type & IL_META_SERIALTYPE_ARRAYOF) != 0)
		{
			return -1;
		}
	}
	else
	{
		type = -2;
	}

	/* Read the serialization type from the blob and check it */
	if(reader->meta.len < 1)
	{
		return -1;
	}
	if(type == -2)
	{
		/* We don't have a member, so believe what the blob tells us */
		type = ((int)(*(reader->meta.data)));
		switch(type)
		{
			case IL_META_SERIALTYPE_BOOLEAN:
			case IL_META_SERIALTYPE_I1:
			case IL_META_SERIALTYPE_U1:
			case IL_META_SERIALTYPE_I2:
			case IL_META_SERIALTYPE_U2:
			case IL_META_SERIALTYPE_CHAR:
			case IL_META_SERIALTYPE_I4:
			case IL_META_SERIALTYPE_U4:
			case IL_META_SERIALTYPE_I8:
			case IL_META_SERIALTYPE_U8:
			case IL_META_SERIALTYPE_R4:
			case IL_META_SERIALTYPE_R8:
			case IL_META_SERIALTYPE_STRING:
			case IL_META_SERIALTYPE_TYPE:		break;

			/* The blob specified an invalid serialization type */
			default: return -1;
		}
	}
	else
	{
		/* Check the blob against the member */
		if(((int)(*(reader->meta.data))) != type)
		{
			return -1;
		}
	}
	reader->meta.data += 1;
	reader->meta.len -= 1;

	/* Check that we have sufficient space in the blob for the value */
	if(!HasSufficientSpace(reader, type))
	{
		return -1;
	}

	/* Return the member and type details to the caller */
	*memberReturn = member;
	*nameReturn = name;
	*nameLenReturn = nameLen;
	return type;
}

#ifdef	__cplusplus
};
#endif
