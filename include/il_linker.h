/*
 * il_linker.h - Routines for linking IL images together.
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

#ifndef	_IL_LINKER_H
#define	_IL_LINKER_H

#include "il_image.h"
#include "il_serialize.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Opaque type for a linker context.
 */
typedef struct _tagILLinker ILLinker;

/*
 * Create a linker context.  The supplied parameters
 * are used to create an ILWriter for the final image.
 * Returns NULL if out of memory.
 */
ILLinker *ILLinkerCreate(FILE *stream, int seekable, int type, int flags);

/*
 * Destroy a linker context.  Returns 1 if OK,
 * zero if an error occurred during linking, or
 * -1 if out of memory.
 */
int ILLinkerDestroy(ILLinker *linker);

/*
 * Create the module and assembly information for the image.
 */
int ILLinkerCreateModuleAndAssembly(ILLinker *linker,
									const char *moduleName,
									const char *assemblyName,
									ILUInt16 *assemblyVersion,
									int hashAlgorithm);

/*
 * Add a directory to a linker context to search for libraries.
 * Returns zero on error.
 */
int ILLinkerAddLibraryDir(ILLinker *linker, const char *pathname);

/*
 * Resolve a library name into a full pathname.  Returns an
 * ILMalloc'ed copy of the full pathname, or NULL if not found.
 */
char *ILLinkerResolveLibrary(ILLinker *linker, const char *name);

/*
 * Add a named assembly to a linker context as a library.
 * Returns zero on error.
 */
int ILLinkerAddLibrary(ILLinker *linker, const char *name);

/*
 * Add an image to a linker context as one of the primary objects.
 * This must be done after all libraries have been added.
 */
int ILLinkerAddImage(ILLinker *linker, ILImage *image, const char *filename);

/*
 * Add a C object file image to a linker context as one of the primary objects.
 * This must be done after all libraries have been added.
 */
int ILLinkerAddCObject(ILLinker *linker, ILImage *image,
					   const char *filename, int memoryModel,
					   int alignFlags);

/*
 * Add a binary resource to a linker context.  Returns zero
 * if out of memory.
 */
int ILLinkerAddResource(ILLinker *linker, const char *name,
						int isPrivate, FILE *stream);

/*
 * Set the entry point to a particular class or method name.
 * Returns zero if the name could not be located.
 */
int ILLinkerSetEntryPoint(ILLinker *linker, const char *name);

/*
 * Determine if the final output image has an entry point.
 */
int ILLinkerHasEntryPoint(ILLinker *linker);

/*
 * Find a particular attribute attached to a program item.
 * Returns NULL if no such attribute.
 */
ILAttribute *ILLinkerFindAttribute(ILProgramItem *item,
								   const char *name,
								   const char *namespace,
								   ILType *arg1Type,
								   ILType *arg2Type);

/*
 * Begin reading the contents of an attribute data block.
 * Returns NULL if the attribute cannot be read.
 */
ILSerializeReader *ILLinkerReadAttribute(ILAttribute *attr);

/*
 * Get the C memory model associated with an image.  Returns
 * 32 or 64 for the memory model, or zero if not a C image.
 * Optional native alignment flags are returned in "alignFlags".
 */
int ILLinkerCMemoryModel(ILImage *image, int *alignFlags);

/*
 * Call the linker as if it were an executable with command-line
 * options.  It is really linked into the calling program.
 */
int ILLinkerMain(int argc, char *argv[]);

#ifdef	__cplusplus
};
#endif

#endif	/* _IL_LINKER_H */
