/*
 * file.c - File-related functions.
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2002  Free Software Foundation, Inc.
 * Copyright (C) 2002  Richard Baumann
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

#include <stdio.h>
#include <errno.h>
#include "il_system.h"
#include "il_sysio.h"
#include "il_errno.h"
#ifdef HAVE_SYS_STAT_H
	#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TYPES_H
	#include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
        #include <fcntl.h>
#endif
#ifdef _WIN32
	#include <windows.h>
	#include <io.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

void ILDeleteFile(const char *filename)
{
#ifdef HAVE_UNLINK
	unlink(filename);
#else
#ifdef HAVE_REMOVE
	remove(filename);
#endif
#endif
}

int ILFileExists(const char *filename, char **newExePath)
{
	if(newExePath)
	{
		*newExePath = 0;
	}

#ifdef __CYGWIN__
	/* If we are on Windows, then check for ".exe" first */
	if(newExePath)
	{
		char *newPath = (char *)ILMalloc(strlen(filename) + 5);
		if(newPath)
		{
			strcpy(newPath, filename);
			strcat(newPath, ".exe");
			if(access(newPath, 0) >= 0)
			{
				*newExePath = newPath;
				return 1;
			}
			ILFree(newPath);
		}
	}
#endif

#ifdef HAVE_ACCESS
	if(access(filename, 0) >= 0)
	{
		return 1;
	}
#else
#ifdef HAVE_STAT
	{
		struct stat st;
		if(stat(filename, &st) >= 0)
		{
			return 1;
		}
	}
#else
	{
		FILE *file = fopen(filename, "r");
		if(file)
		{
			fclose(file);
			return 1;
		}
	}
#endif
#endif

	return 0;
}

/* TODO: I haven't implemented the Windows/Cygwin parts yet - coming soon */

int ILSysIOGetErrno(void)
{
	return ILSysIOConvertErrno(errno);
}

void ILSysIOSetErrno(int code)
{
	code = ILSysIOConvertFromErrno(code);
	if(code != -1)
	{
		errno = code;
	}
	else
	{
		errno = EPERM;
	}
}

const char *ILSysIOGetErrnoMessage(int code)
{
#ifdef HAVE_STRERROR
	code = ILSysIOConvertFromErrno(code);
	if(code != -1)
	{
		return strerror(code);
	}
	else
	{
		return 0;
	}
#else
	return 0;
#endif
}

int ILSysIOValidatePathname(const char *path)
{
	/* TODO */
	return 1;
}

ILSysIOHandle ILSysIOOpenFile(const char *path, ILUInt32 mode,
						      ILUInt32 access, ILUInt32 share)
{
	int result;
	int newAccess;

	switch(access)
	{
		case ILFileAccess_Read:			newAccess = O_RDONLY; break;
		case ILFileAccess_Write:		newAccess = O_WRONLY; break;
		case ILFileAccess_ReadWrite:	newAccess = O_RDWR; break;

		default:
		{
			errno = EACCES;
			return (ILSysIOHandle)(ILNativeInt)(-1);
		}
		/* Not reached */
	}

	switch(mode)
	{
		case ILFileMode_CreateNew:
		{
			/* Create a file only if it doesn't already exist */
			result = open(path, newAccess | O_CREAT | O_EXCL, 0666);
		}
		break;

		case ILFileMode_Create:
		{
			/* Open in create/truncate mode */
			result = open(path, newAccess | O_CREAT | O_TRUNC, 0666);
		}
		break;

		case ILFileMode_Open:
		{
			/* Open the file in the regular mode */
			result = open(path, newAccess, 0666);
		}
		break;

		case ILFileMode_OpenOrCreate:
		{
			/* Open an existing file or create a new one */
			result = open(path, newAccess | O_CREAT, 0666);
		}
		break;

		case ILFileMode_Truncate:
		{
			/* Truncate an existing file */
			result = open(path, newAccess | O_TRUNC, 0666);
		}
		break;

		case ILFileMode_Append:
		{
			/* Open in append mode */
			result = open(path, newAccess | O_CREAT | O_APPEND, 0666);
		}
		break;

		default:
		{
			/* We have no idea what this mode is */
			result = -1;
			errno = EACCES;
		}
		break;
	}

	/* TODO: sharing modes */

	return (ILSysIOHandle)(ILNativeInt)result;
}

int ILSysIOCheckHandleAccess(ILSysIOHandle handle, ILUInt32 access)
{
#if defined(HAVE_FCNTL) && defined(F_GETFL)
	int flags = fcntl((int)(ILNativeInt)handle, F_GETFL, 0);
  	if(flags != -1)
    {
		switch(access)
		{
			case ILFileAccess_Read:		return ((flags & O_RDONLY) != 0);
			case ILFileAccess_Write:	return ((flags & O_WRONLY) != 0);
			case ILFileAccess_ReadWrite:
					return ((flags & O_RDWR) == O_RDWR);
		}
	}
	return 0;
#else
	return 0;
#endif
}

int ILSysIOClose(ILSysIOHandle handle)
{
	int result;
	while((result = close((int)(ILNativeInt)handle)) < 0)
	{
		/* Retry if the system call was interrupted */
		if(errno != EINTR)
		{
			break;
		}
	}
	return (result == 0);
}

ILInt32 ILSysIORead(ILSysIOHandle handle, void *buf, ILInt32 size)
{
	int result;
	while((result = read((int)(ILNativeInt)handle,
						 buf, (unsigned int)size)) < 0)
	{
		/* Retry if the system call was interrupted */
		if(errno != EINTR)
		{
			break;
		}
	}
	return (ILInt32)result;
}

ILInt32 ILSysIOWrite(ILSysIOHandle handle, const void *buf, ILInt32 size)
{
	int written = 0;
	int result = 0;
	while(size > 0)
	{
		/* Write as much as we can, and retry if system call was interrupted */
		result = write((int)(ILNativeInt)handle, buf, (unsigned int)size);
		if(result >= 0)
		{
			written += result;
			size -= result;
		}
		else if(errno != EINTR)
		{
			break;
		}
	}
	if(written > 0)
	{
		return written;
	}
	else
	{
		return ((result < 0) ? -1 : 0);
	}
}

ILInt64 ILSysIOSeek(ILSysIOHandle handle, ILInt64 offset, int whence)
{
	ILInt64 result;
	while((result = (ILInt64)(lseek((int)(ILNativeInt)handle,
									(off_t)offset, whence)))
				== (ILInt64)(-1))
	{
		/* Retry if the system call was interrupted */
		if(errno != EINTR)
		{
			break;
		}
	}
	return result;
}

int ILSysIOFlushRead(ILSysIOHandle handle)
{
	/* TODO: mostly of use for tty devices, not files or sockets */
	return 1;
}

int ILSysIOFlushWrite(ILSysIOHandle handle)
{
	/* TODO: mostly of use for tty devices, not files or sockets */
	return 1;
}

int ILSysIOTruncate(ILSysIOHandle handle, ILInt64 posn)
{
#ifdef HAVE_FTRUNCATE
	int result;
	while((result = ftruncate((int)(ILNativeInt)handle, (off_t)posn)) < 0)
	{
		/* Retry if the system call was interrupted */
		if(errno != EINTR)
		{
			break;
		}
	}
	return (result == 0);
#else
	errno = EINVAL;
	return 0;
#endif
}

int ILSysIOHasAsync(void)
{
	/* TODO: asynchronous I/O is not yet supported */
	return 0;
}

int ILSysIOPathGetLastAccess(const char *path, ILInt64 *time)
{
	int err;
	struct stat buf;
	if (!(err = stat(path,&buf)))
	{
		*time = ILUnixToCLITime(buf.st_atime);
	}
	else
	{
		err = ILSysIOGetErrno();
	}
	return err;
}

int ILSysIOPathGetLastModification(const char *path, ILInt64 *time)
{
	int err;
	struct stat buf;
	if (!(err = stat(path,&buf)))
	{
		*time = ILUnixToCLITime(buf.st_mtime);
	}
	else
	{
		err = ILSysIOGetErrno();
	}
	return err;
}

int ILSysIOPathGetCreation(const char *path, ILInt64 *time)
{
	int err;
	struct stat buf;
	if (!(err = stat(path,&buf)))
	{
		*time = ILUnixToCLITime(buf.st_ctime);
	}
	else
	{
		err = ILSysIOGetErrno();
	}
	return err;
}

#ifdef	__cplusplus
};
#endif
