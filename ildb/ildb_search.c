/*
 * ildb_search.c - Support for searching for source files.
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

#include "ildb_context.h"
#include "ildb_utils.h"
#include "ildb_search.h"
#include "il_system.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

void ILDbSearchDestroy(ILDb *db)
{
	int posn;
	for(posn = 0; posn < db->dirSearchNum; ++posn)
	{
		ILFree(db->dirSearch[posn]);
	}
	if(db->dirSearch)
	{
		ILFree(db->dirSearch);
	}
	db->dirSearch = 0;
	db->dirSearchNum = 0;
}

void ILDbSearchReset(ILDb *db)
{
	ILDbSearchDestroy(db);
	ILDbSearchAdd(db, "$cwd");
	ILDbSearchAdd(db, "$cdir");
}

void ILDbSearchAdd(ILDb *db, const char *directory)
{
	char *dir;
	int posn;

	/* If the directory is not "$cdir" or "$cwd", then expand it */
	if(strcmp(directory, "$cwd") != 0 && strcmp(directory, "$cdir") != 0)
	{
		dir = ILExpandFilename(directory, (char *)0);
		if(!dir)
		{
			ILDbOutOfMemory(db);
		}

		/* Check that the directory does indeed exist */
	#ifdef HAVE_STAT
		{
			struct stat st;
			if(stat(dir, &st) < 0)
			{
				fputs("Warning: ", stderr);
				perror(dir);
				fflush(stderr);
			}
			else if(!S_ISDIR(st.st_mode))
			{
				fprintf(stderr, "Warning: %s is not a directory.\n", dir);
				fflush(stderr);
			}
		}
	#endif
	}
	else
	{
		dir = ILDupString(directory);
		if(!dir)
		{
			ILDbOutOfMemory(db);
		}
	}

	/* If the directory is already on the path, then bring it forward.
	   Note: the first search directory is at the end of the array */
	for(posn = 0; posn < db->dirSearchNum; ++posn)
	{
		if(!strcmp(db->dirSearch[posn], dir))
		{
			ILFree(db->dirSearch[posn]);
			while(posn < (db->dirSearchNum - 1))
			{
				db->dirSearch[posn] = db->dirSearch[posn + 1];
				++posn;
			}
			db->dirSearch[db->dirSearchNum - 1] = dir;
			return;
		}
	}

	/* Add the new directory to the search path */
	db->dirSearch = (char **)ILRealloc
		(db->dirSearch, sizeof(char *) * (db->dirSearchNum + 1));
	if(!(db->dirSearch))
	{
		ILDbOutOfMemory(db);
	}
	db->dirSearch[(db->dirSearchNum)++] = dir;
}

void ILDbSearchPrint(ILDb *db)
{
	int dir;
	fputs("Source directories searched: ", stdout);
	for(dir = db->dirSearchNum - 1; dir >= 0; --dir)
	{
		if(dir != (db->dirSearchNum - 1))
		{
			putc(':', stdout);
		}
		fputs(db->dirSearch[dir], stdout);
	}
	putc('\n', stdout);
	fflush(stdout);
}

/*
 * Find a relative file within a specific directory.
 */
static char *FindInDirectory(ILDb *db, const char *dir, int dirlen,
							 const char *filename, const char *basename)
{
	char *path;
	char *result;

	/* Try the full relative filename */
	path = (char *)ILMalloc(dirlen + strlen(filename) + 2);
	if(!path)
	{
		ILDbOutOfMemory(db);
	}
	strncpy(path, dir, dirlen);
	path[dirlen] = '/';
	strcpy(path + dirlen + 1, filename);
	if(ILFileExists(path, (char **)0))
	{
		result = ILExpandFilename(path, (char *)0);
		if(!result)
		{
			ILDbOutOfMemory(db);
		}
		ILFree(path);
		return result;
	}
	ILFree(path);

	/* Try the base name also */
	if(basename != filename)
	{
		path = (char *)ILMalloc(dirlen + strlen(basename) + 2);
		if(!path)
		{
			ILDbOutOfMemory(db);
		}
		strncpy(path, dir, dirlen);
		path[dirlen] = '/';
		strcpy(path + dirlen + 1, basename);
		if(ILFileExists(path, (char **)0))
		{
			result = ILExpandFilename(path, (char *)0);
			if(!result)
			{
				ILDbOutOfMemory(db);
			}
			ILFree(path);
			return result;
		}
		ILFree(path);
	}

	/* We were unable to locate the file */
	return 0;
}

char *ILDbSearchFind(ILDb *db, const char *filename)
{
	int len, dir;
	const char *basename;
	char *result;

	/* Strip the filename down to its base name */
	len = strlen(filename);
	while(len > 0 && filename[len - 1] != '/' && filename[len - 1] != '\\')
	{
		--len;
	}
	basename = filename + len;

	/* If the filename is absolute, then try using it directly.
	   Otherwise strip it down to its base name */
	if(filename[0] == '/')
	{
		if(ILFileExists(filename, (char **)0))
		{
			result = ILExpandFilename(filename, (char *)0);
			if(!result)
			{
				ILDbOutOfMemory(db);
			}
			return result;
		}
		filename = basename;
	}

	/* Search the specified path */
	for(dir = db->dirSearchNum - 1; dir >= 0; --dir)
	{
		if(!strcmp(db->dirSearch[dir], "$cdir") && db->debugProgram)
		{
			/* Search the directory containing the executable */
			len = strlen(db->debugProgram);
			while(len > 0 && db->debugProgram[len - 1] != '/' &&
			      db->debugProgram[len - 1] != '\\')
			{
				--len;
			}
			if(len > 0)
			{
				result = FindInDirectory(db, db->debugProgram, len - 1,
										 filename, basename);
				if(result)
				{
					return result;
				}
			}
			else
			{
				result = FindInDirectory(db, ".", 1, filename, basename);
				if(result)
				{
					return result;
				}
			}
		}
		else if(!strcmp(db->dirSearch[dir], "$cwd"))
		{
			/* Search the current working directory */
			result = FindInDirectory(db, ".", 1, filename, basename);
			if(result)
			{
				return result;
			}
		}
		else
		{
			/* Search some other directory */
			result = FindInDirectory(db, db->dirSearch[dir],
									 strlen(db->dirSearch[dir]),
									 filename, basename);
			if(result)
			{
				return result;
			}
		}
	}
	return 0;
}

#ifdef	__cplusplus
};
#endif
