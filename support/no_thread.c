/*
 * no_thread.c - Thread management stubs for single-threaded systems.
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

#include "il_gc.h"
#include "pt_include.h"

#ifdef IL_NO_THREADS

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Internal structure of a thread object.
 */
struct _tagILThread
{
	void	*objectArg;
	int		 isBackground;
};

/*
 * Global data for the stubs.
 */
static ILThread *globalThread = 0;

void ILThreadInit(void)
{
	/* Bail out if already initialized */
	if(globalThread != 0)
	{
		return;
	}

	/* Allocate a block for the single global thread object */
	globalThread = (ILThread *)ILMalloc(sizeof(ILThread));
	if(globalThread)
	{
		globalThread->objectArg = 0;
		globalThread->isBackground = 0;
	}
}

ILThread *ILThreadCreate(ILThreadStartFunc startFunc, void *objectArg)
{
	/* We cannot create new threads with this implementation */
	return 0;
}

ILThread *ILThreadSelf(void)
{
	return globalThread;
}

void *ILThreadGetObject(ILThread *thread)
{
	return thread->objectArg;
}

void ILThreadSetObject(ILThread *thread, void *objectArg)
{
	thread->objectArg = objectArg;
}

int ILThreadSuspend(ILThread *thread)
{
	/* It isn't possible to suspend the only thread in the system */
	return 0;
}

void ILThreadResume(ILThread *thread)
{
	/* The global thread is always "resumed" */
}

void ILThreadInterrupt(ILThread *thread)
{
	/* Since we cannot get into a wait state, there is no need to interrupt */
}

int ILThreadGetBackground(ILThread *thread)
{
	return thread->isBackground;
}

void ILThreadSetBackground(ILThread *thread, int flag)
{
	thread->isBackground = flag;
}

int ILThreadGetState(ILThread *thread)
{
	/* The global thread is always running */
	return IL_TS_RUNNING;
}

void ILThreadAtomicStart(void)
{
	/* Nothing to do here */
}

void ILThreadAtomicEnd(void)
{
	/* Nothing to do here */
}

void ILThreadMemoryBarrier(void)
{
	/* Nothing to do here */
}

#ifdef	__cplusplus
};
#endif

#endif /* !IL_USE_PTHREADS */
