/*
 * winres_reader.c - Read the data in the ".rsrc" section of an IL binary.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

#include "image.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Information about a resource entry.
 */
typedef struct _tagILResourceEntry ILResourceEntry;
struct _tagILResourceEntry
{
	int					isDirectory;
	char			   *name;
	int					nameLen;
	ILResourceEntry	   *children;
	ILResourceEntry	   *next;
	unsigned char	   *data;
	unsigned long		length;

};

/*
 * Information about the resource section.
 */
struct _tagILResourceSection
{
	ILImage			   *image;
	unsigned char	   *data;
	unsigned long		length;
	ILResourceEntry	   *rootDirectory;

};

/*
 * Length-delimited string comparison with ignore case.
 */
static int StrNICmp(const char *str1, const char *str2, int len)
{
	char ch1;
	char ch2;
	while(len > 0 && *str1 != '\0' && *str2 != '\0')
	{
		ch1 = *str1++;
		if(ch1 >= 'A' && ch1 <= 'Z')
		{
			ch1 = (ch1 - 'A' + 'a');
		}
		ch2 = *str2++;
		if(ch2 >= 'A' && ch2 <= 'Z')
		{
			ch2 = (ch2 - 'A' + 'a');
		}
		if(ch1 < ch2)
		{
			return -1;
		}
		else if(ch1 > ch2)
		{
			return 1;
		}
		--len;
	}
	if(!len)
	{
		return 0;
	}
	if(*str1 != '\0')
	{
		return 1;
	}
	else if(*str2 != '\0')
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

/*
 * Read the name of a resource from the resource section.
 */
static char *ReadResourceName(ILResourceSection *section, unsigned long offset)
{
	ILUInt32 slen;
	ILUInt32 index;
	char *name;

	/* Read the length of the Unicode string */
	if(offset >= section->length || (offset + 2) > section->length)
	{
		return ILDupString("");
	}
	slen = IL_READ_UINT16(section->data + offset);
	if((section->length - (offset + 2)) < (slen * 2))
	{
		return ILDupString("");
	}

	/* Convert the string from Unicode into ASCII.  We use a simple
	   conversion algorithm because the tag names will almost always
	   be simple ASCII names.  Change to UTF8 later if necessary */
	name = (char *)ILMalloc(slen + 1);
	if(!name)
	{
		return 0;
	}
	for(index = 0; index < slen; ++index)
	{
		name[index] = (char)(section->data[offset + 2 + index * 2]);
	}
	name[slen] = 0;
	return name;
}

/*
 * Convert a resource number into a name.
 */
static char *ResourceNumberToName(unsigned long number)
{
	char buffer[64];
	sprintf(buffer, "%ld", number);
	return ILDupString(buffer);
}

/*
 * Free a resource entry and all of its children.
 */
static void FreeResourceEntry(ILResourceEntry *entry)
{
	ILResourceEntry *current, *next;
	if(entry->name)
	{
		ILFree(entry->name);
	}
	current = entry->children;
	while(current != 0)
	{
		next = current->next;
		FreeResourceEntry(current);
		current = next;
	}
	ILFree(entry);
}

/*
 * Parse the contents of a resource data entry.
 */
static ILResourceEntry *ParseResourceEntry
			(ILResourceSection *section, unsigned long offset, char *name)
{
	ILResourceEntry *entry;
	unsigned long rva;
	unsigned long length;

	/* Allocate a structure to hold the entry's contents */
	if((entry = (ILResourceEntry *)ILMalloc(sizeof(ILResourceEntry))) == 0)
	{
		if(name)
		{
			ILFree(name);
		}
		return 0;
	}
	entry->isDirectory = 0;
	entry->name = name;
	entry->nameLen = strlen(name);
	entry->children = 0;
	entry->next = 0;
	entry->data = 0;
	entry->length = 0;

	/* Parse the resource data entry */
	if(offset >= section->length || (offset + 16) >= section->length)
	{
		return entry;
	}
	rva = IL_READ_UINT32(section->data + offset);
	length = IL_READ_UINT32(section->data + offset + 4);

	/* Validate the entry information */
	if(rva >= section->length || length > section->length ||
	   rva > (section->length - length))
	{
		return entry;
	}

	/* Record the location of the resource's data */
	entry->data = section->data + rva;
	entry->length = length;
	return entry;
}

/*
 * Parse the contents of a resource directory.
 */
static ILResourceEntry *ParseResourceDirectory
			(ILResourceSection *section, unsigned long offset, char *name)
{
	ILResourceEntry *dir;
	ILResourceEntry *entry;
	ILResourceEntry *last;
	ILUInt32 numNamedEntries;
	ILUInt32 numIdEntries;
	unsigned long rva;

	/* Allocate a structure to hold the directory's contents */
	if((dir = (ILResourceEntry *)ILMalloc(sizeof(ILResourceEntry))) == 0)
	{
		if(name)
		{
			ILFree(name);
		}
		return 0;
	}
	dir->isDirectory = 1;
	dir->name = name;
	dir->children = 0;
	dir->next = 0;
	dir->data = 0;
	dir->length = 0;

	/* Parse the header for the resource directory table */
	if(offset >= section->length || (offset + 16) >= section->length)
	{
		return dir;
	}
	numNamedEntries = IL_READ_UINT16(section->data + offset + 12);
	numIdEntries = IL_READ_UINT16(section->data + offset + 14);
	offset += 16;

	/* Process the named directory entries */
	last = 0;
	while(numNamedEntries > 0)
	{
		if(offset > (section->length - 8))
		{
			break;
		}
		name = ReadResourceName
			(section, IL_READ_UINT32(section->data + offset));
		if(!name)
		{
			FreeResourceEntry(dir);
			return 0;
		}
		rva = IL_READ_UINT32(section->data + offset + 4);
		if((rva & (unsigned long)0x80000000) != 0)
		{
			/* Process a sub-directory */
			rva &= (unsigned long)0x7FFFFFFF;
			entry = ParseResourceDirectory(section, rva, name);
		}
		else
		{
			/* Process a regular data entry */
			entry = ParseResourceEntry(section, rva, name);
		}
		if(!entry)
		{
			FreeResourceEntry(dir);
			return 0;
		}
		if(last)
		{
			last->next = entry;
		}
		else
		{
			dir->children = entry;
		}
		last = entry;
		offset += 8;
		--numNamedEntries;
	}

	/* Process the identifier-based directory entries */
	while(numIdEntries > 0)
	{
		if(offset > (section->length - 8))
		{
			break;
		}
		name = ResourceNumberToName(IL_READ_UINT32(section->data + offset));
		if(!name)
		{
			FreeResourceEntry(dir);
			return 0;
		}
		rva = IL_READ_UINT32(section->data + offset + 4);
		if((rva & (unsigned long)0x80000000) != 0)
		{
			/* Process a sub-directory */
			rva &= (unsigned long)0x7FFFFFFF;
			entry = ParseResourceDirectory(section, rva, name);
		}
		else
		{
			/* Process a regular data entry */
			entry = ParseResourceEntry(section, rva, name);
		}
		if(!entry)
		{
			FreeResourceEntry(dir);
			return 0;
		}
		if(last)
		{
			last->next = entry;
		}
		else
		{
			dir->children = entry;
		}
		last = entry;
		offset += 8;
		--numIdEntries;
	}

	/* Return the completed directory to the caller */
	return dir;
}

ILResourceSection *ILResourceSectionCreate(ILImage *image)
{
	ILResourceSection *section;

	/* Allocate space for the section information */
	if((section = (ILResourceSection *)ILMalloc(sizeof(ILResourceSection)))
			== 0)
	{
		return 0;
	}

	/* Initialize the section information */
	section->image = image;
	if(!ILImageGetSection(image, IL_SECTION_WINRES,
						  (void **)&(section->data), &(section->length)))
	{
		section->data = 0;
		section->length = 0;
	}

	/* Parse the directory structure */
	section->rootDirectory = ParseResourceDirectory(section, 0, 0);
	if(!(section->rootDirectory))
	{
		ILFree(section);
		return 0;
	}

	/* Return the section information to the caller */
	return section;
}

void ILResourceSectionDestroy(ILResourceSection *section)
{
	if(section)
	{
		if(section->rootDirectory)
		{
			FreeResourceEntry(section->rootDirectory);
		}
		ILFree(section);
	}
}

static ILResourceEntry *FindEntry(ILResourceSection *section, const char *name)
{
	ILResourceEntry *entry;
	int posn;

	/* Search down the hierarchy for the specified entry */
	entry = section->rootDirectory;
	while(entry != 0)
	{
		if(*name == '/')
		{
			++name;
			continue;
		}
		else if(*name == '\0')
		{
			break;
		}
		posn = 0;
		while(name[posn] != '\0' && name[posn] != '/')
		{
			++posn;
		}
		entry = entry->children;
		while(entry != 0)
		{
			if(entry->nameLen == posn &&
			   !StrNICmp(entry->name, name, posn))
			{
				break;
			}
			entry = entry->next;
		}
		name += posn;
	}

	/* Return the result to the caller */
	return entry;
}

void *ILResourceSectionGetEntry(ILResourceSection *section, const char *name,
								unsigned long *length)
{
	ILResourceEntry *entry;

	/* Validate the parameters */
	if(!section || !name || !length)
	{
		if(length)
		{
			*length = 0;
		}
		return 0;
	}

	/* Search down the hierarchy for the specified entry */
	entry = FindEntry(section, name);

	/* Return the result to the caller */
	if(entry && !(entry->isDirectory))
	{
		*length = entry->length;
		return entry->data;
	}
	else
	{
		*length = 0;
		return 0;
	}
}

void *ILResourceSectionGetFirstEntry(ILResourceSection *section,
									 const char *name, unsigned long *length)
{
	ILResourceEntry *entry;

	/* Validate the parameters */
	if(!section || !name || !length)
	{
		if(length)
		{
			*length = 0;
		}
		return 0;
	}

	/* Search down the hierarchy for the specified entry */
	entry = FindEntry(section, name);

	/* Find the first leaf node under the entry */
	while(entry != 0 && entry->children != 0)
	{
		entry = entry->children;
	}

	/* Return the result to the caller */
	if(entry && !(entry->isDirectory))
	{
		*length = entry->length;
		return entry->data;
	}
	else
	{
		*length = 0;
		return 0;
	}
}

#ifdef	__cplusplus
};
#endif