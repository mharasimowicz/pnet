/*
 * cg_gen.c - Helper routines for "ILGenInfo".
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

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Create a basic image structure with an initial module,
 * assembly, and "<Module>" type.
 */
static ILImage *CreateBasicImage(ILContext *context, const char *assemName)
{
	ILImage *image;

	/* Create the image */
	if((image = ILImageCreate(context)) == 0)
	{
		return 0;
	}

	/* Create the module definition */
	if(!ILModuleCreate(image, 0, "<Module>", 0))
	{
		ILImageDestroy(image);
		return 0;
	}

	/* Create the assembly definition */
	if(!ILAssemblyCreate(image, 0, assemName, 0))
	{
		ILImageDestroy(image);
		return 0;
	}

	/* Create the module type */
	if(!ILClassCreate(ILClassGlobalScope(image), 0, "<Module>", 0, 0))
	{
		ILImageDestroy(image);
		return 0;
	}

	/* Done */
	return image;
}

void ILGenInfoInit(ILGenInfo *info, char *progname, FILE *asmOutput)
{
	info->progname = progname;
	info->asmOutput = asmOutput;
	if((info->context = ILContextCreate()) == 0)
	{
		ILGenOutOfMemory(info);
	}
	if((info->image = CreateBasicImage(info->context, "<Assembly>")) == 0)
	{
		ILGenOutOfMemory(info);
	}
	if((info->libImage = CreateBasicImage(info->context, "mscorlib")) == 0)
	{
		ILGenOutOfMemory(info);
	}
	ILMemPoolInitType(&(info->nodePool), ILNode, 0);
	ILScopeInit(info);
	info->nextLabel = 1;
	info->overflowInsns = 1;
	info->pedanticArith = 0;
	info->clsCompliant = 0;
	info->semAnalysis = 0;
	info->useJavaLib = 0;
	info->outputIsJava = 0;
	info->decimalRoundMode = IL_DECIMAL_ROUND_HALF_EVEN;
	info->stackHeight = 0;
	info->maxStackHeight = 0;
	info->loopStack = 0;
	info->loopStackSize = 0;
	info->loopStackMax = 0;
	info->returnType = IL_TYPE_VOID;
	info->returnVar = -1;
	info->returnLabel = ILLabel_Undefined;
	info->throwVariable = -1;
	info->gotoList = 0;
	info->scopeLevel = 0;
	info->localVars = 0;
	info->numLocalVars = 0;
	info->maxLocalVars = 0;
	info->currentScope = 0;
	info->javaInfo = 0;
	info->numLoops = 0;
	info->numSwitches = 0;
	ILGenMakeLibrary(info);
}

void ILGenInfoToJava(ILGenInfo *info)
{
	info->outputIsJava = 1;
	JavaGenInit(info);
}

void ILGenInfoDestroy(ILGenInfo *info)
{
	ILGotoEntry *gotoEntry, *nextGoto;

	/* Destroy the memory pools */
	ILMemPoolDestroy(&(info->nodePool));
	ILMemPoolDestroy(&(info->scopePool));
	ILMemPoolDestroy(&(info->scopeDataPool));

	/* Destroy the loop stack */
	if(info->loopStack)
	{
		ILFree(info->loopStack);
	}

	/* Destroy the goto list */
	gotoEntry = info->gotoList;
	while(gotoEntry != 0)
	{
		nextGoto = gotoEntry->next;
		ILFree(gotoEntry);
		gotoEntry = nextGoto;
	}

	/* Free the local variable array */
	if(info->localVars)
	{
		ILFree(info->localVars);
	}

	/* Destroy Java-specific information */
	JavaGenDestroy(info);

	/* Destroy the image and context */
	ILImageDestroy(info->image);
	ILContextDestroy(info->context);
}

void ILGenOutOfMemory(ILGenInfo *info)
{
	fprintf(stderr, "%s: virtual memory exhausted\n", info->progname);
	exit(1);
}

ILType *ILFindSystemType(ILGenInfo *info, const char *name)
{
	ILClass *classInfo;
	ILProgramItem *scope;

	/* Look in the program itself first */
	scope = ILClassGlobalScope(info->image);
	if(scope)
	{
		classInfo = ILClassLookup(scope, name, "System");
		if(classInfo)
		{
			return ILType_FromClass(classInfo);
		}
	}

	/* Look in the library image */
	scope = ILClassGlobalScope(info->libImage);
	if(scope)
	{
		classInfo = ILClassLookup(scope, name, "System");
		return ILType_FromClass(classInfo);
	}
	else
	{
		return 0;
	}
}

ILType *ILValueTypeToType(ILGenInfo *info, ILMachineType valueType)
{
	switch(valueType)
	{
		case ILMachineType_Boolean:		return ILType_Boolean;
		case ILMachineType_Int8:		return ILType_Int8;
		case ILMachineType_UInt8:		return ILType_UInt8;
		case ILMachineType_Int16:		return ILType_Int16;
		case ILMachineType_UInt16:		return ILType_UInt16;
		case ILMachineType_Char:		return ILType_Char;
		case ILMachineType_Int32:		return ILType_Int32;
		case ILMachineType_UInt32:		return ILType_UInt32;
		case ILMachineType_NativeInt:	return ILType_Int;
		case ILMachineType_NativeUInt:	return ILType_UInt;
		case ILMachineType_Int64:		return ILType_Int64;
		case ILMachineType_UInt64:		return ILType_UInt64;
		case ILMachineType_Float32:		return ILType_Float32;
		case ILMachineType_Float64:		return ILType_Float64;
		case ILMachineType_NativeFloat:	return ILType_Float;
		case ILMachineType_ObjectRef:	return ILFindSystemType(info, "Object");
		default:						break;
	}
	return ILType_Invalid;
}

unsigned ILGenTempVar(ILGenInfo *info, ILMachineType type)
{
	return ILGenTempTypedVar(info, ILValueTypeToType(info, type));
}

unsigned ILGenTempTypedVar(ILGenInfo *info, ILType *type)
{
	unsigned varNum;

	/* See if we can re-use a free temporary variable */
	for(varNum = 0; varNum < info->numLocalVars; ++varNum)
	{
		if(!(info->localVars[varNum].allocated) &&
		   ILTypeIdentical(info->localVars[varNum].type, type))
		{
			info->localVars[varNum].allocated = 1;
			return varNum;
		}
	}

	/* Abort if too many local variables */
	if(info->numLocalVars == (unsigned)0xFFFF)
	{
		fprintf(stderr, "%s: too many local variables - aborting\n",
				info->progname);
		exit(1);
	}

	/* Add a new local variable to the current method */
	if(info->numLocalVars >= info->maxLocalVars)
	{
		ILLocalVar *newvars;
		newvars = (ILLocalVar *)ILRealloc(info->localVars,
										  sizeof(ILLocalVar) *
										  		(info->maxLocalVars + 16));
		if(!newvars)
		{
			ILGenOutOfMemory(info);
		}
		info->localVars = newvars;
		info->maxLocalVars += 16;
	}
	info->localVars[info->numLocalVars].name = 0;
	info->localVars[info->numLocalVars].scopeLevel = -1;
	info->localVars[info->numLocalVars].type = type;
	info->localVars[info->numLocalVars].allocated = 1;

	/* Generate assembly code to allocate the local */
	ILGenAllocLocal(info, type, (const char *)0);

	/* Return the new variable index to the caller */
	return (info->numLocalVars)++;
}

void ILGenReleaseTempVar(ILGenInfo *info, unsigned localNum)
{
	if(localNum < info->numLocalVars &&
	   info->localVars[localNum].scopeLevel == -1)
	{
		info->localVars[localNum].allocated = 0;
	}
}

#ifdef	__cplusplus
};
#endif
