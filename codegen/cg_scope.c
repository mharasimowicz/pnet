/*
 * cg_scope.c - Scope handling declarations.
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

#include "cg_nodes.h"
#include "cg_scope.h"
#include "cg_rbtree.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Data that is associated with a scope item.
 */
struct _tagILScopeData
{
	ILRBTreeNode		rbnode;		/* Red-black node information */
	const char	       *name;		/* Name associated with the item */
	void			   *data;		/* Data associated with the item */

};

/*
 * Information about a "using" directive.
 */
typedef struct _tagILScopeUsingInfo ILScopeUsingInfo;
struct _tagILScopeUsingInfo
{
	ILScope			   *refScope;	/* Referenced using scope */
	ILScopeUsingInfo   *next;		/* Next using declaration */

};

/*
 * Internal structure of a scope record.
 */
struct _tagILScope
{
	ILGenInfo	   *info;		/* Code generator associated with the scope */
	ILScope		   *parent;		/* Parent scope */
	ILRBTree		nameTree;	/* Tree containing all names in the scope */
	ILScopeUsingInfo *using;	/* Using declarations for the scope */
	void		   *data;		/* Data attached to the scope for searching */

	/* Function for looking up an item within the scope */
	ILScopeData *(*lookup)(ILScope *scope, const char *name);

};

/*
 * Comparison function for finding a name within a scope.
 */
static int NameCompare(void *key, ILRBTreeNode *node)
{
	const char *name = (const char *)key;
	ILScopeData *data = (ILScopeData *)node;
	return strcmp(name, data->name);
}

/*
 * Lookup function for normal scopes.
 */
static ILScopeData *NormalScope_Lookup(ILScope *scope, const char *name)
{
	return (ILScopeData *)ILRBTreeSearch(&(scope->nameTree), (void *)name);
}

/*
 * Lookup function for normal scopes with "using" directives.
 */
static ILScopeData *UsingScope_Lookup(ILScope *scope, const char *name)
{
	ILScopeData *data;
	ILScopeUsingInfo *using;

	/* Try looking in the scope itself */
	data = NormalScope_Lookup(scope, name);
	if(data != 0)
	{
		return data;
	}

	/* Search each of the "using" scopes */
	using = scope->using;
	while(using != 0)
	{
		data = (*(using->refScope->lookup))(using->refScope, name);
		if(data != 0)
		{
			return data;
		}
		using = using->next;
	}

	/* Could not find the name */
	return 0;
}

/*
 * Lookup function for type scopes.
 */
static ILScopeData *TypeScope_Lookup(ILScope *scope, const char *name)
{
	return NormalScope_Lookup(scope, name);
}

void ILScopeInit(ILGenInfo *info)
{
	ILMemPoolInitType(&(info->scopePool), ILScope, 0);
	ILMemPoolInitType(&(info->scopeDataPool), ILScopeData, 0);
}

ILScope *ILScopeCreate(ILGenInfo *info, ILScope *parent)
{
	ILScope *scope = ILMemPoolAlloc(&(info->scopePool), ILScope);
	if(!scope)
	{
		ILGenOutOfMemory(info);
	}
	scope->info = info;
	scope->parent = parent;
	ILRBTreeInit(&(scope->nameTree), NameCompare, 0);
	scope->using = 0;
	scope->data = 0;
	scope->lookup = NormalScope_Lookup;
	return scope;
}

/*
 * Add an item to a particular scope.
 */
static void AddToScope(ILScope *scope, const char *name,
					   int kind, void *data)
{
	ILScopeData *sdata;
	sdata = ILMemPoolAlloc(&(scope->info->scopeDataPool), ILScopeData);
	if(!sdata)
	{
		ILGenOutOfMemory(scope->info);
	}
	sdata->rbnode.kind = kind;
	sdata->name = name;
	sdata->data = data;
	ILRBTreeInsert(&(scope->nameTree), &(sdata->rbnode), (void *)name);
}

/*
 * Find a namespace scope from a name.  If the namespace
 * scope does not exist, then create it.
 */
static ILScope *FindNamespaceScope(ILScope *scope, const char *name)
{
	ILScopeData *data;
	ILScope *newScope;
	int len;
	const char *newName;
	while(*name != '\0')
	{
		if(*name == '.')
		{
			++name;
			continue;
		}
		len = 0;
		while(name[len] != '\0' && name[len] != '.')
		{
			++len;
		}
		if(name[len] == '\0')
		{
			newName = name;
		}
		else
		{
			newName = (ILInternString((char *)name, len)).string;
		}
		data = ILScopeLookup(scope, newName, 0);
		if(data != 0)
		{
			if(data->rbnode.kind == IL_SCOPE_SUBSCOPE)
			{
				/* We already have a namespace with this name */
				scope = (ILScope *)(data->data);
			}
			else
			{
				/* There is something already declared here that
				   is not a namespace */
				return 0;
			}
		}
		else
		{
			newScope = ILScopeCreate(scope->info, scope);
			AddToScope(scope, newName, IL_SCOPE_SUBSCOPE, newScope);
			scope = newScope;
		}
		name += len;
	}
	return scope;
}

/*
 * Import a type into a scope.  A new scope is created
 * for the type to access its members and nested children.
 */
static void ImportType(ILScope *scope, ILClass *info, const char *name)
{
	ILScope *newScope;
	ILNestedInfo *nested;
	ILClass *child;
	const char *nestedName;
	int len;

	/* Bail out if the type is already declared */
	if(ILScopeLookup(scope, name, 0) != 0)
	{
		return;
	}

	/* Create a new scope for the type */
	newScope = ILScopeCreate(scope->info, scope);
	newScope->lookup = TypeScope_Lookup;
	newScope->data = (void *)info;

	/* Add the new scope to the original scope, attached to the type name */
	AddToScope(scope, name, IL_SCOPE_IMPORTED_TYPE, newScope);

	/* Add the nested children to sub-scopes */
	nested = 0;
	while((nested = ILClassNextNested(info, nested)) != 0)
	{
		/* Get the child type from the nested information token */
		child = ILNestedInfoGetChild(nested);

		/* Get the name of the nested type, without prefixes */
		nestedName = ILClass_Name(child);
		len = strlen(nestedName);
		while(len > 0 && nestedName[len - 1] != '$')
		{
			--len;
		}

		/* Import the nested type into a sub-scope */
		ImportType(newScope, child, nestedName + len);
	}
}

void ILScopeImport(ILScope *scope, ILImage *image)
{
	unsigned long numTokens;
	unsigned long token;
	ILClass *info;
	ILScope *namespaceScope = 0;
	const char *namespaceName = 0;
	const char *namespaceTest;

	/* Scan the entire TypeDef table for top-level types */
	numTokens = ILImageNumTokens(image, IL_META_TOKEN_TYPE_DEF);
	for(token = 1; token <= numTokens; ++token)
	{
		info = (ILClass *)(ILImageTokenInfo
								(image, token | IL_META_TOKEN_TYPE_DEF));
		if(info && !ILClass_NestedParent(info) &&
		   strcmp(ILClass_Name(info), "<Module>") != 0)
		{
			/* Find the scope corresponding to the namespace */
			namespaceTest = ILClass_Namespace(info);
			if(namespaceTest)
			{
				if(!namespaceName ||
				   !(namespaceName == namespaceTest ||
				     !strcmp(namespaceName, namespaceTest)))
				{
					namespaceName = namespaceTest;
					namespaceScope = FindNamespaceScope(scope, namespaceTest);
				}
			}
			else
			{
				namespaceScope = scope;
				namespaceName = 0;
			}

			/* Import the type into the namespace scope */
			if(namespaceScope != 0)
			{
				ImportType(namespaceScope, info, ILClass_Name(info));
			}
			else
			{
				/* There is some kind of conflict between the namespace
				   name and a type declaration - ignore it for now */
				namespaceName = 0;
			}
		}
	}
}

int ILScopeUsing(ILScope *scope, const char *identifier, const char *alias)
{
	ILScope *namespaceScope = FindNamespaceScope(scope, identifier);
	if(!namespaceScope)
	{
		return 0;
	}
	if(alias)
	{
		/* Create a sub-scope that links across to the "using" namespace */
		AddToScope(scope, alias, IL_SCOPE_SUBSCOPE, namespaceScope);
	}
	else
	{
		/* Add the "using" declaration to the scope as an attribute.
		   We allocate from the data pool, because it isn't worth
		   creating a special pool just for "using" declarations */
		ILScopeUsingInfo *using;
		using = ILMemPoolAlloc(&(scope->info->scopeDataPool), ILScopeUsingInfo);
		if(!using)
		{
			ILGenOutOfMemory(scope->info);
		}
		using->refScope = namespaceScope;
		using->next = scope->using;
		scope->using = using;

		/* Change the lookup function to one which handles "using" clauses */
		scope->lookup = UsingScope_Lookup;
	}
	return 1;
}

void ILScopeClearUsing(ILScope *scope)
{
	scope->using = 0;
}

ILScopeData *ILScopeLookup(ILScope *scope, const char *identifier, int up)
{
	ILScopeData *data;
	while(scope != 0)
	{
		/* Look for the name in this scope */
		data = (*(scope->lookup))(scope, identifier);
		if(data != 0)
		{
			/* We have found a match for the name */
			return data;
		}

		/* Move up to the parent scope and try again */
		if(up)
		{
			scope = scope->parent;
		}
		else
		{
			break;
		}
	}
	return 0;
}

ILScopeData *ILScopeNextItem(ILScopeData *data)
{
	return (ILScopeData *)(ILRBTreeNext(&(data->rbnode)));
}

int ILScopeDeclareType(ILScope *scope, ILNode *node, const char *name,
					   const char *namespace, ILScope **resultScope,
					   ILNode **origDefn)
{
	ILScope *namespaceScope;
	ILScopeData *data;
	ILScope *usingScope;
	ILScope *typeScope;

	/* Find the scope that is associated with the namespace */
	if(namespace && *namespace != '\0')
	{
		namespaceScope = FindNamespaceScope(scope, namespace);
		if(!namespaceScope)
		{
			return IL_SCOPE_ERROR_CANT_CREATE_NAMESPACE;
		}
	}
	else
	{
		namespaceScope = scope;
	}

	/* Determine if there is a declaration for the name already */
	data = ILScopeLookup(namespaceScope, name, 0);
	if(data != 0)
	{
		if(data->rbnode.kind == IL_SCOPE_DECLARED_TYPE)
		{
			/* Declaration conflicts with a type the user already declared */
			*origDefn = (ILNode *)(((ILScope *)(data->data))->data);
			return IL_SCOPE_ERROR_REDECLARED;
		}
		else if(data->rbnode.kind == IL_SCOPE_SUBSCOPE)
		{
			/* There is already a namespace with that name in existence */
			return IL_SCOPE_ERROR_NAME_IS_NAMESPACE;
		}
		else if(data->rbnode.kind == IL_SCOPE_IMPORTED_TYPE)
		{
			/* Conflict with an imported type */
			return IL_SCOPE_ERROR_IMPORT_CONFLICT;
		}
		else
		{
			return IL_SCOPE_ERROR_OTHER;
		}
	}

	/* Create a new scope to hold the "using" context for the type.
	   We must do this because the global "using" context will be
	   cleared at the end of the parse, but we need the information
	   it contains after the parse */
	usingScope = ILScopeCreate(scope->info, scope);
	usingScope->using = scope->using;
	usingScope->lookup = UsingScope_Lookup;

	/* Create a scope to hold the type itself */
	typeScope = ILScopeCreate(scope->info, usingScope);
	typeScope->data = (void *)node;

	/* Add the type to the namespace scope */
	AddToScope(namespaceScope, name, IL_SCOPE_DECLARED_TYPE, typeScope);

	/* Done */
	*resultScope = typeScope;
	return IL_SCOPE_ERROR_OK;
}

/*
 * Resolve a qualified identifier.
 */
static ILScopeData *ResolveQualIdent(ILScope *scope, ILNode *identifier)
{
	ILScopeData *data;

	if(!identifier)
	{
		return 0;
	}
	if(yykind(identifier) == yykindof(ILNode_Identifier))
	{
		/* Singleton identifier */
		return ILScopeLookup(scope, ((ILNode_Identifier *)identifier)->name, 1);
	}
	else if(yykind(identifier) == yykindof(ILNode_QualIdent))
	{
		/* Qualified identifier */
		ILNode_QualIdent *ident = (ILNode_QualIdent *)identifier;
		ILNode_Identifier *right;
		data = ResolveQualIdent(scope, ident->left);
		if(!data)
		{
			return 0;
		}
		if(yykind(ident->right) != yykindof(ILNode_Identifier))
		{
			return 0;
		}
		right = (ILNode_Identifier *)(ident->right);
		if(data->rbnode.kind == IL_SCOPE_SUBSCOPE)
		{
			/* Search for the name within a namespace */
			return ILScopeLookup((ILScope *)(data->data), right->name, 0);
		}
		else if(data->rbnode.kind == IL_SCOPE_IMPORTED_TYPE ||
				data->rbnode.kind == IL_SCOPE_DECLARED_TYPE)
		{
			/* Search for a nested type */
			return ILScopeLookup((ILScope *)(data->data), right->name, 0);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		/* Don't know what kind of node this is */
		return 0;
	}
}

int ILScopeResolveType(ILScope *scope, ILNode *identifier,
					   const char *namespace, ILClass **classInfo,
					   ILNode **nodeInfo)
{
	ILScopeData *data;
	ILScope *namespaceScope;

	/* If we have a namespace and the identifier is a singleton,
	   then try looking in the namespace first */
	if(namespace && identifier &&
	   yykind(identifier) == yykindof(ILNode_Identifier))
	{
		namespaceScope = FindNamespaceScope(scope, namespace);
		if(namespaceScope)
		{
			data = ILScopeLookup(namespaceScope,
						((ILNode_Identifier *)identifier)->name, 0);
			if(data)
			{
				if(data->rbnode.kind == IL_SCOPE_DECLARED_TYPE)
				{
					*classInfo = 0;
					*nodeInfo = (ILNode *)(((ILScope *)(data->data))->data);
					return 1;
				}
				else if(data->rbnode.kind == IL_SCOPE_IMPORTED_TYPE)
				{
					*classInfo = (ILClass *)(((ILScope *)(data->data))->data);
					*nodeInfo = 0;
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
	}

	/* See if we have a declaration for the identifier */
	data = ResolveQualIdent(scope, identifier);
	if(data)
	{
		if(data->rbnode.kind == IL_SCOPE_DECLARED_TYPE)
		{
			*classInfo = 0;
			*nodeInfo = (ILNode *)(((ILScope *)(data->data))->data);
			return 1;
		}
		else if(data->rbnode.kind == IL_SCOPE_IMPORTED_TYPE)
		{
			*classInfo = (ILClass *)(((ILScope *)(data->data))->data);
			*nodeInfo = 0;
			return 1;
		}
		else
		{
			return 0;
		}
	}

	/* This is not a declared or imported type.  It may be
	   a built-in "System" library type instead */
	if(identifier != 0 && yykind(identifier) == yykindof(ILNode_QualIdent))
	{
		ILNode_QualIdent *qident = (ILNode_QualIdent *)identifier;
		if(qident->left != 0 &&
		   yykind(qident->left) == yykindof(ILNode_Identifier) &&
		   qident->right != 0 &&
		   yykind(qident->right) == yykindof(ILNode_Identifier))
		{
			*classInfo = ILClassLookup
					(ILClassGlobalScope(scope->info->libImage),
				     ((ILNode_Identifier *)(qident->right))->name,
				     ((ILNode_Identifier *)(qident->left))->name);
			if((*classInfo) != 0)
			{
				/* Import the library type into the main image */
				*classInfo = ILClassImport(scope->info->image, *classInfo);
				if(!(*classInfo))
				{
					ILGenOutOfMemory(scope->info);
				}

				/* Return the details to the caller */
				*nodeInfo = 0;
				return 1;
			}
		}
	}

	/* Could not find the type */
	return 0;
}

int ILScopeDataGetKind(ILScopeData *data)
{
	return data->rbnode.kind;
}

ILClass *ILScopeDataGetClass(ILScopeData *data)
{
	if(data->rbnode.kind == IL_SCOPE_DECLARED_TYPE)
	{
		ILNode *node = (ILNode *)(((ILScope *)(data->data))->data);
		return ((ILNode_ClassDefn *)node)->classInfo;
	}
	else if(data->rbnode.kind == IL_SCOPE_IMPORTED_TYPE)
	{
		return (ILClass *)(((ILScope *)(data->data))->data);
	}
	else
	{
		return 0;
	}
}

ILScope *ILScopeDataGetSubScope(ILScopeData *data)
{
	return (ILScope *)(data->data);
}

ILMember *ILScopeDataGetMember(ILScopeData *data)
{
	return (ILMember *)(data->data);
}

#ifdef	__cplusplus
};
#endif
