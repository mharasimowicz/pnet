/*
 * c_crt0.c - Output the glue logic necessary to invoke "main" functions.
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
#include "il_dumpasm.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Flags that indicate which "main" parameters were provided.
 */
#define	C_MAIN_RETURNS_INT		(1<<0)
#define	C_MAIN_ARGC				(1<<1)
#define	C_MAIN_ARGV				(1<<2)
#define	C_MAIN_ENVP				(1<<3)

/*
 * Print a warning about a specific node.
 */
static void PrintWarning(ILNode *node, const char *msg)
{
	if(node)
	{
		CCWarningOnLine(yygetfilename(node), yygetlinenum(node), msg);
	}
	else
	{
		CCWarning(msg);
	}
}

/*
 * Print an error about a specific node.
 */
static void PrintError(ILNode *node, const char *msg)
{
	if(node)
	{
		CCErrorOnLine(yygetfilename(node), yygetlinenum(node), msg);
	}
	else
	{
		CCError(msg);
	}
}

/*
 * Determine if a type is "char **", ignoring "const" and other
 * such qualifiers.
 */
static int IsCharPtrPtr(ILType *type)
{
	if(type != 0 && ILType_IsComplex(type) &&
	   ILType_Kind(type) == IL_TYPE_COMPLEX_PTR)
	{
		type = ILTypeStripPrefixes(ILType_Ref(type));
		if(type != 0 && ILType_IsComplex(type) &&
		   ILType_Kind(type) == IL_TYPE_COMPLEX_PTR)
		{
			type = ILTypeStripPrefixes(ILType_Ref(type));
			return (type == ILType_Int8);
		}
	}
	return 0;
}

void CGenCrt0(ILGenInfo *info, FILE *stream)
{
	ILClass *moduleClass;
	ILMethod *method;
	void *data;
	ILNode *node;
	ILType *signature;
	ILType *type;
	int flags, ptrSize;
	unsigned long numParams;
	char *libcName;

	/* Find the module class */
	moduleClass = ILClassLookup(ILClassGlobalScope(info->image), "<Module>", 0);

	/* See if we actually have a "main" function in this module */
	method = 0;
	while((method = (ILMethod *)ILClassNextMemberByKind
				(moduleClass, (ILMember *)method,
				 IL_META_MEMBERKIND_METHOD)) != 0)
	{
		if(!strcmp(ILMethod_Name(method), "main") &&
		   ILMethod_IsStatic(method))
		{
			break;
		}
	}
	if(!method)
	{
		/* If there is no "main" function, then there is no need for crt0 */
		return;
	}

	/* Try to find an error-reporting node for "main" in the global scope */
	data = CScopeLookup("main");
	if(data && CScopeGetKind(data) == C_SCDATA_FUNCTION)
	{
		node = CScopeGetNode(data);
	}
	else
	{
		node = 0;
	}

	/* Validate the return type and parameters for "main" */
	flags = 0;
	signature = ILMethod_Signature(method);
	type = ILTypeGetReturn(signature);
	if(type == ILType_Int32)
	{
		flags |= C_MAIN_RETURNS_INT;
	}
	else if(type == ILType_Void)
	{
		/* The "main" function returns "void", but we can work around it */
		PrintWarning(node, _("`main' returns `void' instead of `int'"));
	}
	else
	{
		PrintError(node, _("`main' does not return `int'"));
	}
	numParams = ILTypeNumParams(signature);
	if(numParams > 3)
	{
		PrintError(node, _("`main' has more than 3 parameters"));
	}
	if(numParams >= 1)
	{
		type = ILTypeGetParam(signature, 1);
		if(type != ILType_Int32)
		{
			PrintError(node, _("first parameter of `main' is not `int'"));
		}
		flags |= C_MAIN_ARGC;
	}
	if(numParams >= 2)
	{
		type = ILTypeGetParam(signature, 2);
		if(!IsCharPtrPtr(type))
		{
			PrintError(node, _("second parameter of `main' is not `char **'"));
		}
		flags |= C_MAIN_ARGV;
	}
	if(numParams >= 3)
	{
		type = ILTypeGetParam(signature, 3);
		if(!IsCharPtrPtr(type))
		{
			PrintError(node, _("third parameter of `main' is not `char **'"));
		}
		flags |= C_MAIN_ENVP;
	}

	/* Bail out if we had some errors */
	if(CCHaveErrors)
	{
		return;
	}

	/* Generate the ".start" crt0 code for the program */
	fputs(".method public static void '.start'"
				"([.library]System.String[]) cil managed\n", stream);
	fputs("{\n\t.entrypoint\n", stream);
	fputs("\t.maxstack\t3\n", stream);
	fputs("\t.locals (int32, int8 * *, int8 * *, [.library]System.Object)\n",
		  stream);

	/* Wrap the body of the method in a try block */
	fputs(".try { \n", stream);

	/* Determine the "argc", "argv", and "envp" values for the program */
	ptrSize = (gen_32bit_only ? 4 : 8);
	fputs("\tldarg.0\n", stream);
	fprintf(stream, "\tldc.i4.%d\n", ptrSize);
	fputs("\tldloca\t0\n", stream);
	fputs("\tcall\tnative int 'OpenSystem.Languages.C'.'Crt0'::"
			"GetArgV([.library]System.String[], int32, int32 &)\n", stream);
	fputs("\tstloc.1\n", stream);
	fputs("\tcall\tnative int 'OpenSystem.Languages.C'.'Crt0'::"
				"GetEnvironment()\n", stream);
	fputs("\tstloc.2\n", stream);

	/* Push the name of the "libc" library assembly onto the stack, so
	   that the "Crt0" class knows where to look for global definitions */
	libcName = CCStringListGetValue(extension_flags, num_extension_flags,
									"libc-name");
	if(libcName != 0 && *libcName != '\0')
	{
		fprintf(stream, "\tldstr \"%s\"\n", libcName);
	}
	else
	{
		fprintf(stream, "\tldstr \"libc%d\"\n", ptrSize * 8);
	}

	/* Perform other system startup tasks */
	fputs("\tcall\tvoid 'OpenSystem.Languages.C'.'Crt0'::Startup"
				"([.library]System.String)\n", stream);

	/* Invoke the "main" function with the required arguments */
	if((flags & C_MAIN_ARGC) != 0)
	{
		fputs("\tldloc.0\n", stream);
	}
	if((flags & C_MAIN_ARGV) != 0)
	{
		fputs("\tldloc.1\n", stream);
	}
	if((flags & C_MAIN_ENVP) != 0)
	{
		fputs("\tldloc.2\n", stream);
	}
	fputs("\tcall\t", stream);
	ILDumpMethodType(stream, info->image, signature, IL_DUMP_QUOTE_NAMES,
					 moduleClass, ILMethod_Name(method), method);
	putc('\n', stream);

	/* Push a default status value if "main" returns void */
	if((flags & C_MAIN_RETURNS_INT) == 0)
	{
		fputs("\tldc.i4.0\n", stream);
	}

	/* Perform system shutdown tasks, including calling "exit" */
	fputs("\tcall\tvoid 'OpenSystem.Languages.C'.'Crt0'::"
				"Shutdown(int32)\n", stream);

	/* Handle exceptions that are caught by the try block */
	fputs("\tleave\tL1\n", stream);
	fputs("} catch [.library]System.OutOfMemoryException {\n", stream);
	fputs("\trethrow\n", stream);
	fputs("} catch [.library]System.Object {\n", stream);
	fputs("\tstloc.3\n", stream);
	fputs("\tldloc.3\n", stream);
	fputs("\tcall\t[.library]System.Object 'OpenSystem.Languages.C'.'Crt0'::"
				"ShutdownWithException([.library]System.Object)\n", stream);
	fputs("\tthrow\n", stream);
	fputs("}\n", stream);
	fputs("L1:\n", stream);

	/* Generate the method footer for ".start" */
	fputs("\tret\n", stream);
	fputs("} // method .start\n", stream);
}

#ifdef	__cplusplus
};
#endif
