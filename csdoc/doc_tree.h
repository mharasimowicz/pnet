/*
 * doc_tree.h - Storage for a documentation tree in memory.
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

#ifndef	_CSDOC_DOC_TREE_H
#define	_CSDOC_DOC_TREE_H

#include "il_xml.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Forward declarations.
 */
typedef struct _tagILDocTree		ILDocTree;
typedef struct _tagILDocLibrary		ILDocLibrary;
typedef struct _tagILDocType		ILDocType;
typedef struct _tagILDocInterface	ILDocInterface;
typedef struct _tagILDocAttribute	ILDocAttribute;
typedef struct _tagILDocMember		ILDocMember;
typedef struct _tagILDocParameter	ILDocParameter;
typedef struct _tagILDocText		ILDocText;

/*
 * Information that is stored about an entire documentation tree.
 */
struct _tagILDocTree
{
	ILDocLibrary   *libraries;		/* Libraries within the tree */
	ILDocLibrary   *lastLibrary;	/* Last library within the tree */

};

/*
 * Information that is stored about a type library.
 */
struct _tagILDocLibrary
{
	ILDocTree	   *tree;			/* Tree that this type belongs to */
	char		   *name;			/* Name of the library, or NULL if none */
	ILDocType	   *types;			/* List of types in the library */
	ILDocLibrary   *next;			/* Next library within the tree */

};

/*
 * Information that is stored about a type.
 */
struct _tagILDocType
{
	ILDocTree	   *tree;			/* Tree that this type belongs to */
	ILDocLibrary   *library;		/* Library that this type belongs to */
	char		   *name;			/* Name, without the namespace qualifier */
	char		   *fullName;		/* Full name of the type */
	char		   *ilasmSignature;	/* ILASM signature for the type */
	char		   *csSignature;	/* C# signature for the type */
	char		   *baseType;		/* Full name of the base type */
	ILDocInterface *interfaces;		/* List of the type's interfaces */
	ILDocAttribute *attributes;		/* Attributes attached to the type */
	ILDocText      *doc;			/* Text of the type's documentation */
	ILDocMember    *members;		/* List of type members */
	ILDocType	   *next;			/* Next type in the same library */

};

/*
 * Information that is stored about an interface.
 */
struct _tagILDocInterface
{
	char		   *name;			/* Name of the interface */
	ILDocInterface *next;			/* Next interface for the type */

};

/*
 * Information that is stored about and attribute.
 */
struct _tagILDocAttribute
{
	char		   *name;			/* Name and parameters for the attribute */
	ILDocAttribute *next;			/* Next attribute for the type */

};

/*
 * Supported member types.
 */
typedef enum
{
	ILDocMemberType_Field,
	ILDocMemberType_Method,
	ILDocMemberType_Constructor,
	ILDocMemberType_Property,
	ILDocMemberType_Event,
	ILDocMemberType_Unknown,

} ILDocMemberType;

/*
 * Information that is stored about a type member.
 */
struct _tagILDocMember
{
	ILDocTree	   *tree;			/* Tree that this member belongs to */
	ILDocType	   *type;			/* Type that this member belongs to */
	char		   *name;			/* Name of the member */
	ILDocMemberType	memberType;		/* Type of member */
	char		   *ilasmSignature;	/* ILASM signature for the type */
	char		   *csSignature;	/* C# signature for the type */
	char		   *returnType;		/* Return type (NULL for "void") */
	ILDocParameter *parameters;		/* List of member parameters */
	ILDocAttribute *attributes;		/* Attributes attached to the type */
	ILDocText      *doc;			/* Text of the type's documentation */
	ILDocMember    *next;			/* Next member within the type */

};

/*
 * Information that is stored about a member parameter.
 */
struct _tagILDocParameter
{
	char		   *name;			/* Name of the parameter */
	char		   *type;			/* Type of the parameter */
	ILDocParameter *next;			/* Next parameter for the member */

};

/*
 * Information that is stored about the documentation text for an item.
 */
struct _tagILDocText
{
	ILDocText	   *parent;			/* Parent of this node (NULL if topmost) */
	ILDocText	   *next;			/* Next in the list of doc nodes */
	int				isTag;			/* Non-zero if a tag, zero if text */
	int				size;			/* Used internally by ILDocTextGetParam */
	ILDocText	   *children;		/* List of children of a tag node */
	char			text[1];		/* Text in the node, or the tag name */

};

/*
 * Create a documentation tree.  Returns NULL if out of memory.
 */
ILDocTree *ILDocTreeCreate(void);

/*
 * Load the contents of an XML stream into a documentation tree.
 * Returns zero if out of memory.
 */
int ILDocTreeLoad(ILDocTree *tree, ILXMLReader *reader);

/*
 * Destroy a documentation tree.
 */
void ILDocTreeDestroy(ILDocTree *tree);

/*
 * Get a particular parameter from a documentation tag node.
 * Returns NULL if not a tag node, or the parameter isn't present.
 */
const char *ILDocTextGetParam(ILDocText *text, const char *name);

#ifdef	__cplusplus
};
#endif

#endif	/* _CSDOC_DOC_TREE_H */
