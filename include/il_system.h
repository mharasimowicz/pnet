/*
 * il_system.h - Wrapper around system support services that we need.
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

#ifndef	_IL_SYSTEM_H
#define	_IL_SYSTEM_H

#include "il_values.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/* Include useful C headers from the system */
#ifdef HAVE_STRING_H
	#include <string.h>
#else
	#ifdef HAVE_STRINGS_H
		#include <strings.h>
	#endif
#endif
#ifdef HAVE_MEMORY_H
	#include <memory.h>
#endif
#ifdef HAVE_STDLIB_H
	#include <stdlib.h>
#endif

/* Memory allocation routines */
void *ILMalloc(unsigned long size);
void *ILRealloc(void *ptr, unsigned long size);
void *ILCalloc(unsigned long nelems, unsigned long size);
void  ILFree(void *ptr);
char *ILDupString(const char *str);

/* Page-based system allocation routines */
unsigned long ILPageAllocSize(void);	/* For memory allocations */
unsigned long ILPageMapSize(void);		/* For mapping files */
void *ILPageAlloc(unsigned long size);
void  ILPageFree(void *ptr, unsigned long size);

/* Memory copy, compare, set, etc, routines */
#ifdef HAVE_MEMSET
	#define	ILMemSet(dest,ch,len)	(memset((dest), (ch), (len)))
	#define	ILMemZero(dest,len)		(memset((dest), 0, (len)))
#else
	void *ILMemSet(void *dest, int ch, unsigned long len);
	#ifdef HAVE_BZERO
		#define	ILMemZero(dest,len)	(bzero((char *)(dest), (len)))
	#else
		void ILMemZero(void *dest, unsigned long len);
	#endif
#endif
#ifdef HAVE_MEMCPY
	#define	ILMemCpy(dest,src,len)	(memcpy((dest), (src), (len)))
#else
	#ifdef HAVE_BCOPY
		#define	ILMemCpy(dest,src,len)	\
					(bcopy((char *)(src), (char *)(dest), (len)))
	#else
		void *ILMemCpy(void *dest, const void *src, unsigned long len);
	#endif
#endif
#ifdef HAVE_MEMMOVE
	#define	ILMemMove(dest,src,len)	(memmove((dest), (src), (len)))
#else
	void *ILMemMove(void *dest, const void *src, unsigned long len);
#endif
#ifdef HAVE_MEMCMP
	#define	ILMemCmp(s1,s2,len)		(memcmp((s1), (s2), (len)))
#else
	#ifdef HAVE_BCMP
		#define	ILMemCmp(s1,s2,len)	\
					(bcmp((char *)(s1), (char *)(s2), (len)))
	#else
		int ILMemCmp(const void *s1, const void *s2, unsigned long len);
	#endif
#endif
#ifdef HAVE_MEMCHR
	#define	ILMemChr(str,ch,len)	(memchr((str), (ch), (len)))
#else
	void *ILMemChr(const void *str, int ch, unsigned long len);
#endif

/* String routines */
int ILStrICmp(const char *str1, const char *str2);

/* Memory mapped files */
int ILMapFileToMemory(int fd, unsigned long start, unsigned long end,
					  void **mapAddress, unsigned long *mapLength,
					  char **addrOfStart);
void ILUnmapFileFromMemory(void *mapAddress, unsigned long mapLength);

/* Delete a file from disk */
void ILDeleteFile(const char *filename);

/* Determine if a file exists.  If "newExePath" is not NULL,
   then the file is expected to be executable.  If it is an
   executable and its name is not the same as "filename",
   then allocate a new pathname and return it in "newExePath".
   Use "ILFree" to free the path later */
int ILFileExists(const char *filename, char **newExePath);

/* Get the current time in nanoseconds since 12:00am Jan 1, 0001 */
typedef struct
{
	ILInt64			secs;
	ILUInt32		nsecs;

} ILCurrTime;
void ILGetCurrTime(ILCurrTime *timeValue);

/* Get the current time in nanoseconds since the system was rebooted.
   If it isn't possible to get the since-reboot time, this returns zero */
int ILGetSinceRebootTime(ILCurrTime *timeValue);

/* Get the number of seconds West of GMT for the local timezone */
ILInt32 ILGetTimeZoneAdjust(void);

/* Get platform directory pathname information */
typedef struct
{
	ILUInt16		dirSep;
	ILUInt16		altDirSep;
	ILUInt16		volumeSep;
	ILUInt16		pathSep;
	const char     *invalidPathChars;

} ILPathInfo;
void ILGetPathInfo(ILPathInfo *info);

/* Get the platform-specific newline sequence */
const char *ILGetNewLine(void);

/* Dynamic library support */
void *ILDynLibraryOpen(const char *name);
void  ILDynLibraryClose(void *handle);
void *ILDynLibraryGetSymbol(void *handle, const char *symbol);

/*gopz's hacks*/
typedef void* ILFileHandle;
ILFileHandle ILFileOpen(const char * path, const char * mode);
ILBool ILFileClose(ILFileHandle handle);
ILBool ILFileCreate(const char * path);
int ILFileRead(void *buf,int size,int num,ILFileHandle handle);
int ILFileWrite(void *buf,int size,int num,ILFileHandle handle);
ILBool ILFileCheckHandleAccess(ILFileHandle handle, ILUInt32 access);
int ILFileSeek(ILFileHandle handle, long offset,int from);
long ILFileTell (ILFileHandle handle);
int ILFileFlush(ILFileHandle handle);
int ILFileTruncate(ILFileHandle handle,long value);
int ILFileHasAsync(ILFileHandle handle);
#ifdef	__cplusplus
};
#endif

#endif	/* _IL_SYSTEM_H */



