/*
 * pt_include.h - Include the necessary headers for pthreads.
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

#ifndef	_PT_INCLUDE_H
#define	_PT_INCLUDE_H

#include "il_config.h"
#include "il_thread.h"

/*
 * An easy way to turn off threads completely for testing.
 */
/*#define IL_NO_THREADS*/

/*
 * Determine if we can use pthreads.  Right now, we only do this
 * for Linux, but we will extend it to other systems later.
 */
#if !defined(IL_NO_THREADS)
#if defined(linux) || defined(__linux) || defined(__linux__)
#define	IL_USE_PTHREADS
#endif
#if !defined(IL_USE_PTHREADS)
#define	IL_NO_THREADS
#endif
#endif

#ifdef IL_USE_PTHREADS

#define	GC_LINUX_THREADS

/* We need to include <pthread.h> via the garbage collector,
   because it redirects some of the routines elsewhere to
   ensure that the GC is notified of thread creation */
#include <gc.h>
#include <semaphore.h>

#ifdef	__cplusplus
extern	"C" {
#endif

/* Invalid thread identifier */
extern pthread_t const _ILInvalidThread;

/* Convert a timeout value into a timespec */
void _ILTimeoutToTimeSpec(struct timespec *ts, ILInt32 timeout);

/* Initialize a recursive mutex */
int _ILMutexInitRecursive(pthread_mutex_t *mutex);

#ifdef	__cplusplus
};
#endif

#endif /* IL_USE_PTHREADS */

#endif	/* _PT_INCLUDE_H */
