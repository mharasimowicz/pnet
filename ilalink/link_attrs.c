/*
 * link_attrs.c - Convert custom attributes and copy them to the final image.
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

#ifdef	__cplusplus
extern	"C" {
#endif

int _ILLinkerConvertAttrs(ILLinker *linker, ILProgramItem *oldItem,
						  ILProgramItem *newItem)
{
	ILAttribute *attr;
	ILAttribute *newAttr;
	ILProgramItem *item;
	ILMethod *method;
	const void *blob;
	unsigned long blobLen;

	/* Scan through the attributes on the old item */
	attr = 0;
	while((attr = ILProgramItemNextAttribute(oldItem, attr)) != 0)
	{
		/* Create the new attribute block */
		newAttr = ILAttributeCreate(linker->image, 0);
		if(!newAttr)
		{
			_ILLinkerOutOfMemory(linker);
			return 0;
		}

		/* Determine how to convert the attribute's type */
		item = ILAttributeTypeAsItem(attr);
		if(item)
		{
			method = ILProgramItemToMethod(item);
			if(method)
			{
				method = (ILMethod *)_ILLinkerConvertMemberRef
							(linker, (ILMember *)method);
				if(!method)
				{
					_ILLinkerOutOfMemory(linker);
					return 0;
				}
				ILAttributeSetType(newAttr, (ILProgramItem *)method);
			}
		}

		/* Copy the attribute's value */
		blob = ILAttributeGetValue(attr, &blobLen);
		if(blob)
		{
			if(!ILAttributeSetValue(newAttr, blob, blobLen))
			{
				_ILLinkerOutOfMemory(linker);
				return 0;
			}
		}
	}

	/* Done */
	return 1;
}

int _ILLinkerConvertSecurity(ILLinker *linker, ILProgramItem *oldItem,
						     ILProgramItem *newItem)
{
	ILDeclSecurity *decl;
	ILDeclSecurity *newDecl;
	const void *blob;
	unsigned long blobLen;

	/* Get the security declaration from the old item */
	decl = ILDeclSecurityGetFromOwner(oldItem);
	if(!decl)
	{
		return 1;
	}

	/* Create a security declaration on the new item */
	newDecl = ILDeclSecurityCreate(linker->image, 0, newItem,
								   ILDeclSecurity_Type(decl));
	if(!newDecl)
	{
		_ILLinkerOutOfMemory(linker);
		return 0;
	}

	/* Copy the security blob */
	blob = ILDeclSecurityGetBlob(decl, &blobLen);
	if(blob)
	{
		if(!ILDeclSecuritySetBlob(newDecl, blob, blobLen))
		{
			_ILLinkerOutOfMemory(linker);
			return 0;
		}
	}

	/* Done */
	return 1;
}

#ifdef	__cplusplus
};
#endif
