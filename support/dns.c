/*
 * dns.c - Routines for processing DNS queries
 *
 * Copyright (C) 2002  Free Software Foundation, Inc.
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

#include "il_system.h"
#include "il_utils.h"
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_NETDB_H
#include <netdb.h>
#endif 

#ifdef	__cplusplus
extern	"C" {
#endif

struct hostent* ILGetHostByName(const char *name)
{
#ifdef HAVE_GETHOSTBYNAME
	return gethostbyname(name);
#else
	return NULL;
#endif

}

struct hostent* ILGetHostByAddr(const void* addr, unsigned int len,int type)
{
#ifdef HAVE_GETHOSTBYADDR
	return gethostbyaddr(addr,len,type);
#else
	return NULL;
#endif
}
#ifdef	__cplusplus
};
#endif
