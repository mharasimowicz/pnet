/*
 * interlocked.h - Implementation of interlocked functions.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 *
 * Authors: Thong Nguyen (tum@veridicus.com)
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

#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include "il_config.h"
#include "il_values.h"

#if defined(__i386) || defined(__i386__)
struct _tagILInterruptContext
{
	void *address;
	
	/* Integer registers */
	unsigned int Eax;
	unsigned int Ebx;	
	unsigned int Ecx;
	unsigned int Edx;
	unsigned int Edi;
	unsigned int Esi;

	/* Control registers */
	unsigned int Ebp;
	unsigned int Eip;
	unsigned int Esp;
};
#else
struct _tagILInterruptContext
{
	void *address;
};
#endif

#if (defined(HAVE_SETJMP) || defined(HAVE_SETJMP_H)) \
	&& defined(HAVE_LONGJMP)

	#include <setjmp.h>

	#define IL_JMP_BUFFER jmp_buf

	#if defined(HAVE_SIGSETJMP) && defined(HAVE_SIGLONGJMP)
		#define IL_SETJMP(buf) \
			sigsetjmp(buf)

		#define IL_LONGJMP(buf, arg) \
			siglongjmp(buf, arg)
	#else
		#define IL_SETJMP(buf) \
			setjmp(buf)

		#define IL_LONGJMP(buf, arg) \
			longjmp(buf, arg)
	#endif

	#ifdef IL_WIN32_PLATFORM
		#define IL_INTERRUPT_SUPPORTS 1
		#define IL_INTERRUPT_SUPPORTS_ILLEGAL_MEMORY_ACCESS 1
		#define IL_INTERRUPT_WIN32 1
		#if defined(__i386) || defined(__i386__)
			#define IL_INTERRUPT_HAVE_X86_CONTEXT 1
		#endif
	#elif defined(linux) || defined(__linux) || defined(__linux__) \
		&& (defined(HAVE_SIGNAL) || defined(HAVE_SIGACTION))
		#define IL_INTERRUPT_SUPPORTS 1
		#define IL_INTERRUPT_SUPPORTS_ILLEGAL_MEMORY_ACCESS 1
		#define IL_INTERRUPT_POSIX 1

		#if (defined(__i386) || defined(__i386__)) \
			&& defined(HAVE_SIGACTION) && defined(HAVE_SYS_UCONTEXT_H)
			#define IL_INTERRUPT_HAVE_X86_CONTEXT 1
		#endif
	#endif

#endif

#endif /* _INTERRUPT_H */