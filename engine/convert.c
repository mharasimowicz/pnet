/*
 * convert.c - Convert methods using a coder.
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

#include "engine_private.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Locate or load an external module that is being referenced via "PInvoke".
 * Returns the system module pointer, or NULL if it could not be loaded.
 */
static void *LocateExternalModule(ILExecProcess *process, const char *name,
								  ILPInvoke *pinvoke)
{
	ILLoadedModule *loaded;
	char *pathname;

	/* Search for an already-loaded module with the same name */
	loaded = process->loadedModules;
	while(loaded != 0)
	{
		if(!ILStrICmp(loaded->name, name))
		{
			return loaded->handle;
		}
		loaded = loaded->next;
	}

	/* Create a new module structure.  We keep this structure even
	   if we cannot load the actual module.  This ensures that
	   future requests for the same module will be rejected without
	   re-trying the open */
	loaded = (ILLoadedModule *)ILMalloc(sizeof(ILLoadedModule) + strlen(name));
	if(!loaded)
	{
		return 0;
	}
	loaded->next = process->loadedModules;
	loaded->handle = 0;
	strcpy(loaded->name, name);
	process->loadedModules = loaded;

	/* Resolve the module name to a library name */
	pathname = ILPInvokeResolveModule(pinvoke);
	if(!pathname)
	{
		return 0;
	}

	/* Attempt to open the module */
	loaded->handle = ILDynLibraryOpen(pathname);
	ILFree(pathname);
	return loaded->handle;
}

unsigned char *_ILConvertMethod(ILExecThread *thread, ILMethod *method)
{
	ILMethodCode code;
	ILPInvoke *pinv;
	ILCoder *coder = thread->process->coder;
	unsigned char *start;
	void *cif;
	void *ctorcif;
	int isConstructor;
	ILModule *module;
	const char *name;
	void *moduleHandle;
	int result;
	ILInternalInfo fnInfo;
	ILInternalInfo ctorfnInfo;

	/* We need the metadata write lock */
	IL_METADATA_WRLOCK(thread);

	/* Is the method already converted? */
	if((start = (unsigned char *)(method->userData)) != 0)
	{
		IL_METADATA_UNLOCK(thread);
		return start;
	}

	/* Get the method code */
	if(!ILMethodGetCode(method, &code))
	{
		code.code = 0;
	}

	/* The conversion is different depending upon whether the
	   method is written in IL or not */
	if(code.code)
	{
		/* Use the bytecode verifier and coder to convert the method */
		if(!_ILVerify(coder, &start, method, &code,
					  ILImageIsSecure(ILProgramItem_Image(method))))
		{
			IL_METADATA_UNLOCK(thread);
			return 0;
		}
	}
	else
	{
		/* This is a "PInvoke", "internalcall", or "runtime" method */
		pinv = ILPInvokeFind(method);
		fnInfo.func = 0;
		fnInfo.marshal = 0;
		ctorfnInfo.func = 0;
		ctorfnInfo.marshal = 0;
		isConstructor = ILMethod_IsConstructor(method);
		switch(method->implementAttrs &
					(IL_META_METHODIMPL_CODE_TYPE_MASK |
					 IL_META_METHODIMPL_INTERNAL_CALL |
					 IL_META_METHODIMPL_JAVA))
		{
			case IL_META_METHODIMPL_IL:
			case IL_META_METHODIMPL_OPTIL:
			{
				/* If we don't have a PInvoke record, then we don't
				   know what to map this method call to */
				if(!pinv)
				{
					IL_METADATA_UNLOCK(thread);
					return 0;
				}

				/* Find the module for the PInvoke record */
				module = ILPInvoke_Module(pinv);
				if(!module)
				{
					IL_METADATA_UNLOCK(thread);
					return 0;
				}
				name = ILModule_Name(module);
				if(!name || *name == '\0')
				{
					IL_METADATA_UNLOCK(thread);
					return 0;
				}
				moduleHandle = LocateExternalModule
									(thread->process, name, pinv);
				if(!moduleHandle)
				{
					IL_METADATA_UNLOCK(thread);
					return 0;
				}

				/* Get the name of the function within the module */
				name = ILPInvoke_Alias(pinv);
				if(!name || *name == '\0')
				{
					name = ILMethod_Name(method);
				}

				/* Look up the method within the module */
				fnInfo.func = ILDynLibraryGetSymbol(moduleHandle, name);
			}
			break;

			case IL_META_METHODIMPL_RUNTIME:
			case IL_META_METHODIMPL_IL | IL_META_METHODIMPL_INTERNAL_CALL:
			{
				/* "internalcall" and "runtime" methods must not
				   have PInvoke records associated with them */
				if(pinv)
				{
					IL_METADATA_UNLOCK(thread);
					return 0;
				}

				/* Look up the internalcall function details */
				if(!_ILFindInternalCall(method, 0, &fnInfo))
				{
					if(isConstructor)
					{
						if(!_ILFindInternalCall(method, 1, &ctorfnInfo))
						{
							IL_METADATA_UNLOCK(thread);
							return 0;
						}
					}
					else
					{
						IL_METADATA_UNLOCK(thread);
						return 0;
					}
				}
				else if(isConstructor)
				{
					_ILFindInternalCall(method, 1, &ctorfnInfo);
				}
			}
			break;

			default:
			{
				/* No idea how to invoke this method */
				IL_METADATA_UNLOCK(thread);
				return 0;
			}
			/* Not reached */
		}

		/* Bail out if we did not find the underlying native method */
		if(!(fnInfo.func) && !(ctorfnInfo.func))
		{
			IL_METADATA_UNLOCK(thread);
			return 0;
		}

	#if defined(HAVE_LIBFFI)
		/* Generate a "cif" structure to handle the native call details */
		if(fnInfo.func)
		{
			/* Make the "cif" structure for the normal method entry */
			cif = _ILMakeCifForMethod(method, (pinv == 0));
			if(!cif)
			{
				IL_METADATA_UNLOCK(thread);
				return 0;
			}
		}
		else
		{
			cif = 0;
		}
		if(ctorfnInfo.func)
		{
			/* Make the "cif" structure for the allocating constructor */
			ctorcif = _ILMakeCifForConstructor(method, (pinv == 0));
			if(!ctorcif)
			{
				IL_METADATA_UNLOCK(thread);
				return 0;
			}
		}
		else
		{
			ctorcif = 0;
		}
	#else
		/* Use the marshalling function pointer as the cif if no libffi */
		cif = fnInfo.marshal;
		ctorcif = ctorfnInfo.marshal;
	#endif

		/* Generate the coder stub for marshalling the call */
		if(!isConstructor)
		{
			/* We only need the method entry point */
			if(!ILCoderSetupExtern(coder, &start, method,
								   fnInfo.func, cif, (pinv == 0)))
			{
				IL_METADATA_UNLOCK(thread);
				return 0;
			}
			while((result = ILCoderFinish(coder)) != IL_CODER_END_OK)
			{
				/* Do we need a coder restart due to cache overflow? */
				if(result != IL_CODER_END_RESTART)
				{
					IL_METADATA_UNLOCK(thread);
					return 0;
				}
				if(!ILCoderSetupExtern(coder, &start, method,
									   fnInfo.func, cif, (pinv == 0)))
				{
					IL_METADATA_UNLOCK(thread);
					return 0;
				}
			}
		}
		else
		{
			/* We need both the method and constructor entry points */
			if(!ILCoderSetupExternCtor(coder, &start, method,
								       fnInfo.func, cif,
									   ctorfnInfo.func, ctorcif,
									   (pinv == 0)))
			{
				IL_METADATA_UNLOCK(thread);
				return 0;
			}
			while((result = ILCoderFinish(coder)) != IL_CODER_END_OK)
			{
				/* Do we need a coder restart due to cache overflow? */
				if(result != IL_CODER_END_RESTART)
				{
					IL_METADATA_UNLOCK(thread);
					return 0;
				}
				if(!ILCoderSetupExternCtor(coder, &start, method,
									       fnInfo.func, cif,
										   ctorfnInfo.func, ctorcif,
										   (pinv == 0)))
				{
					IL_METADATA_UNLOCK(thread);
					return 0;
				}
			}
		}
	}

	/* The method is converted now */
	method->userData = (void *)start;
	IL_METADATA_UNLOCK(thread);
	return start;
}

#ifdef	__cplusplus
};
#endif
