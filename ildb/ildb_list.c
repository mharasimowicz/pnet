/*
 * ildb_list.c - Processing for "list" commands.
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

#include "ildb_context.h"
#include "ildb_utils.h"
#include "ildb_cmd.h"
#include "ildb_source.h"
#include "il_system.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Show a range of lines from a source file.
 */
static void ShowLines(ILDb *db, ILDbSourceFile *file, long first, long last)
{
	char *line;
	long len;
	while(first <= last)
	{
		len = ILDbSourceGetLine(file, first, &line);
		if(len < 0)
		{
			break;
		}
		printf("%-7ld\t", first);
		fwrite(line, 1, len, stdout);
		putc('\n', stdout);
		++first;
	}
	fflush(stdout);
}

/*
 * List the contents of a file, function, etc.
 */
static void List(ILDb *db, char *args)
{
	ILDbSourceFile *file;
	if(!strcmp(args, ",main"))
	{
		/* Used by xxgdb to request the source for the program entry point */
		if(db->process && db->entryPoint)
		{
			file = ILDbSourceGet(db, (ILClass *)0,
								 (ILMember *)(db->entryPoint));
			if(file)
			{
				ShowLines(db, file, 1, file->numLines);
				db->currFile = file;
				db->currLine = file->numLines;
				return;
			}
		}
	}
	if(*args != '\0')
	{
		ILDbError(db, "Function \"%s\" not defined.", args);
	}
}

/*
 * Table of "list" commands.
 */
ILDbCmdInfo ILDbListCommands[] = {
	{"l", 1, 0, 0, 0, List, 0, 0},
	{"list", 2, 0, 0, 0, List,
		"list source lines",
		0},
};
int ILDbNumListCommands = (sizeof(ILDbListCommands) / sizeof(ILDbCmdInfo));

#ifdef	__cplusplus
};
#endif
