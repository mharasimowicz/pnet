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
 * Add an image to a linker context as a library.
 * Returns zero on error.
 */
int ILLinkerAddLibrary(ILLinker *linker, ILImage *image, const char *filename);

/*
 * Add an image to a linker context as one of the primary objects.
 * This must be done after all libraries have been added.
 */
int ILLinkerAddImage(ILLinker *linker, ILImage *image, const char *filename);

#ifdef	__cplusplus
};
#endif

#endif	/* _IL_LINKER_H */
