/*
 * interlocked.h - Implementation of interlocked functions.
 *
 * Copyright (C) 2004  Southern Storm Software, Pty Ltd.
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

#include "interrupt.h"

#ifdef IL_INTERRUPT_WIN32

#include "thr_defs.h"
#include "il_thread.h"
#include <windows.h>

#ifdef	__cplusplus
extern	"C" {
#endif

static LPTOP_LEVEL_EXCEPTION_FILTER __previousFilter;

#ifdef IL_INTERRUPT_SUPPORTS_ILLEGAL_MEMORY_ACCESS
static LONG CALLBACK __UnhandledExceptionFilter(EXCEPTION_POINTERS* ExceptionInfo)
{
	ILThread *thread = ILThreadSelf();

	switch (ExceptionInfo->ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		thread->illegalMemoryAccessHandler(ExceptionInfo->ExceptionRecord->ExceptionAddress);
		break;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}
#endif

void _ILInterruptInit()
{
#ifdef IL_INTERRUPT_SUPPORTS_ILLEGAL_MEMORY_ACCESS
	__previousFilter = SetUnhandledExceptionFilter(__UnhandledExceptionFilter);
#endif
}

void _ILInterruptDeinit()
{
	SetUnhandledExceptionFilter(__previousFilter);
}

#ifdef	__cplusplus
};
#endif

#endif /* IL_INTERRUPT_WIN32 */