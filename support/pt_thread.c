/*
 * pt_thread.c - Thread management, based on pthreads.
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

#include "il_gc.h"
#include "pt_include.h"

#ifdef IL_USE_PTHREADS

#include <private/gc_priv.h>	/* For SIG_SUSPEND */
#include <signal.h>

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Signals that are used to bang threads on the head and notify
 * them of various conditions.  Finding a free signal is a bit
 * of a pain as most of the obvious candidates are already in
 * use by pthreads or libgc.  Unix needs more signals!
 */
#define	IL_SIG_SUSPEND		SIGALRM
#define	IL_SIG_RESUME		SIGVTALRM
#define	IL_SIG_INTERRUPT	SIGFPE

/*
 * Make sure that "ThreadInit" is only called once.
 */
static pthread_once_t threadInitOnce = PTHREAD_ONCE_INIT;

/*
 * Thread-specific key that is used to store the thread object.
 */
static pthread_key_t threadObjectKey;

/*
 * Semaphores that are used to acknowledge thread suspension and resumption.
 */
static sem_t suspendAck;
static sem_t resumeAck;

/*
 * Global state that is mutexed between all threads.
 */
static pthread_mutex_t threadLockAll = PTHREAD_MUTEX_INITIALIZER;
static long volatile numThreads = 0;
static long volatile numBackgroundThreads = 0;
static sem_t startupAck;

/*
 * Suspend until we receive either IL_SIG_RESUME or SIG_SUSPEND.
 *
 * This implementation assumes a Linux-like threads implementation
 * that uses one process per thread, and probably won't work on
 * other pthreads implementations.
 *
 * The "SIG_SUSPEND" signal is used by the garbage collector to stop
 * the world for garbage collection, and so we must not block it.
 *
 * It would be nice if we could use the thread-aware "sigwait"
 * function instead of the process-aware "sigsuspend", but
 * "sigwait" does not deliver signals to handlers, and we need
 * the GC suspend signal to be delivered properly.
 */
static void SuspendUntilResumed(ILThread *self)
{
	sigset_t mask;

	/* Set up the signal mask to allow through only selected signals */
	sigfillset(&mask);
	sigdelset(&mask, IL_SIG_RESUME);
#ifdef SIG_SUSPEND
	sigdelset(&mask, SIG_SUSPEND);
#endif
	sigdelset(&mask, SIGINT);
	sigdelset(&mask, SIGQUIT);
	sigdelset(&mask, SIGTERM);
	sigdelset(&mask, SIGABRT);

	/* Suspend until we receive IL_SIG_RESUME from ourselves */
	do
	{
		sigsuspend(&mask);
	}
	while(!(self->resumeRequested));
}

/*
 * Signal handler for IL_SIG_SUSPEND.
 */
static void SuspendSignal(int sig)
{
	ILThread *self = ILThreadSelf();

	/* Tell the "ILThreadSuspend" function that we are now suspended */
	sem_post(&suspendAck);

	/* Suspend until we are resumed by some other thread */
	SuspendUntilResumed(self);

	/* Tell the "ILThreadResume" function that we have now resumed */
	sem_post(&resumeAck);
}

/*
 * Signal handler for IL_SIG_RESUME.
 */
static void ResumeSignal(int sig)
{
	/* There is nothing to do here - "SuspendUntilResumed" will
	   be interrupted in the call to "sigsuspend", and then will
	   test the "resumeRequested" flag to detect the request */
}

/*
 * Signal handler for IL_SIG_INTERRUPT.
 */
static void InterruptSignal(int sig)
{
	/* There is nothing to do here - this signal exists purely
	   to force system calls to exit with EINTR */
}

/*
 * Thread library initialization routines that are called once only.
 */
static void ThreadInit(void)
{
	struct sigaction action;
	sigset_t set;
	ILThread *thread;

	/* Set up the signal handlers that we require */
	ILMemZero(&action, sizeof(action));
	action.sa_flags = SA_RESTART;
	sigfillset(&(action.sa_mask));
	sigdelset(&(action.sa_mask), SIGINT);
	sigdelset(&(action.sa_mask), SIGQUIT);
	sigdelset(&(action.sa_mask), SIGTERM);
	sigdelset(&(action.sa_mask), SIGABRT);
	action.sa_handler = SuspendSignal;
	sigaction(IL_SIG_SUSPEND, &action, (struct sigaction *)0);
	action.sa_handler = ResumeSignal;
	sigaction(IL_SIG_RESUME, &action, (struct sigaction *)0);
	action.sa_handler = InterruptSignal;
	sigaction(IL_SIG_INTERRUPT, &action, (struct sigaction *)0);

	/* We need a thread-specific key for storing thread objects */
	pthread_key_create(&threadObjectKey, (void (*)(void *))0);

	/* Initialize the semaphores that we need */
	sem_init(&suspendAck, 0, 0);
	sem_init(&resumeAck, 0, 0);
	sem_init(&startupAck, 0, 0);

	/* Create an "ILThread" object for the current thread */
	thread = (ILThread *)ILMalloc(sizeof(ILThread));
	if(thread)
	{
		pthread_mutex_init(&(thread->lock), (pthread_mutexattr_t *)0);
		thread->threadId        = pthread_self();
		thread->state           = IL_TS_RUNNING;
		thread->resumeRequested = 0;
		thread->startFunc       = 0;
		thread->objectArg       = 0;
		pthread_mutex_init(&(thread->wakeupMutex), (pthread_mutexattr_t *)0);
		thread->wakeupType      = IL_WAKEUP_NONE;
		sem_init(&(thread->wakeup), 0, 0);
		++numThreads;
	}

	/* Block the IL_SIG_RESUME signal in the current thread.
	   It will be unblocked by "SuspendUntilResumed" */
	sigemptyset(&set);
	sigaddset(&set, IL_SIG_RESUME);
	pthread_sigmask(SIG_BLOCK, &set, (sigset_t *)0);

	/* Unblock the IL_SIG_SUSPEND and IL_SIG_INTERRUPT signals */
	sigemptyset(&set);
	sigaddset(&set, IL_SIG_SUSPEND);
	sigaddset(&set, IL_SIG_INTERRUPT);
	pthread_sigmask(SIG_UNBLOCK, &set, (sigset_t *)0);
}

void ILThreadInit(void)
{
	pthread_once(&threadInitOnce, ThreadInit);
}

/*
 * Entry point for a pthread.  This initializes the thread
 * and then blocks waiting for the initial resume signal.
 */
static void *ThreadStart(void *arg)
{
	ILThread *thread = (ILThread *)arg;
	sigset_t set;

	/* Store the thread identifier into the object */
	thread->threadId = pthread_self();

	/* Attach the thread object to the thread identifier */
	pthread_setspecific(threadObjectKey, (void *)thread);

	/* Detach ourselves because we won't be using "pthread_join"
	   to detect when the thread exits */
	pthread_detach(thread->threadId);

	/* Block the IL_SIG_RESUME signal in the current thread.
	   It will be unblocked by "SuspendUntilResumed" */
	sigemptyset(&set);
	sigaddset(&set, IL_SIG_RESUME);
	pthread_sigmask(SIG_BLOCK, &set, (sigset_t *)0);

	/* Unblock the IL_SIG_SUSPEND and IL_SIG_INTERRUPT signals */
	sigemptyset(&set);
	sigaddset(&set, IL_SIG_SUSPEND);
	sigaddset(&set, IL_SIG_INTERRUPT);
	pthread_sigmask(SIG_UNBLOCK, &set, (sigset_t *)0);

	/* Tell "ILThreadCreate" that the thread is ready */
	sem_post(&startupAck);

	/* Wait until the thread is really started by a call to "ILThreadStart" */
	SuspendUntilResumed(thread);
	sem_post(&resumeAck);

	/* If we still have a startup function, then execute it.
	   The field may have been replaced with NULL if the thread
	   was aborted before it even got going */
	if(thread->startFunc)
	{
		(*(thread->startFunc))(thread->objectArg);
	}

	/* Mark the thread as stopped */
	pthread_mutex_lock(&(thread->lock));
	thread->state |= IL_TS_STOPPED;
	pthread_mutex_unlock(&(thread->lock));

	/* The thread has exited back through its start function */
	return 0;
}

ILThread *ILThreadCreate(ILThreadStartFunc startFunc, void *objectArg)
{
	ILThread *thread;
	pthread_t id;

	/* Create a new thread object and populate it */
	thread = (ILThread *)ILMalloc(sizeof(ILThread));
	if(!thread)
	{
		return 0;
	}
	pthread_mutex_init(&(thread->lock), (pthread_mutexattr_t *)0);
	ILMemZero(&(thread->threadId), sizeof(thread->threadId));
	thread->state = IL_TS_UNSTARTED;
	thread->resumeRequested = 0;
	thread->startFunc = startFunc;
	thread->objectArg = objectArg;
	pthread_mutex_init(&(thread->wakeupMutex), (pthread_mutexattr_t *)0);
	thread->wakeupType = IL_WAKEUP_NONE;
	sem_init(&(thread->wakeup), 0, 0);

	/* Lock out the thread system */
	pthread_mutex_lock(&threadLockAll);

	/* Create the new thread */
	if(pthread_create(&id, (pthread_attr_t *)0,
					  ThreadStart, (void *)thread) != 0)
	{
		pthread_mutex_unlock(&threadLockAll);
		ILFree(thread);
		return 0;
	}

	/* Wait until the thread startup ack is posted, so that
	   we know that the thread is ready to go */
	sem_wait(&startupAck);

	/* We have one extra thread in the system at present */
	++numThreads;

	/* Unlock the thread system and return */
	pthread_mutex_unlock(&threadLockAll);
	return thread;
}

int ILThreadStart(ILThread *thread)
{
	int result;

	/* Lock down the thread object */
	pthread_mutex_lock(&(thread->lock));

	/* Are we in the correct state to start? */
	if((thread->state & IL_TS_UNSTARTED) != 0)
	{
		/* Resume the thread, which is suspended in "ThreadStart" */
		thread->state &= ~IL_TS_UNSTARTED;
		pthread_kill(thread->threadId, IL_SIG_RESUME);
		sem_wait(&resumeAck);
		result = 1;
	}
	else
	{
		result = 0;
	}

	/* Unlock the thread object and return */
	pthread_mutex_unlock(&(thread->lock));
	return result;
}

ILThread *ILThreadSelf(void)
{
	return (ILThread *)(pthread_getspecific(threadObjectKey));
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
	unsigned int state;
	int result = 1;

	/* Lock down the thread object */
	pthread_mutex_lock(&(thread->lock));

	/* Determine what to do based on the thread's state */
	state = thread->state;
	if((state & IL_TS_SUSPENDED) != 0)
	{
		/* Nothing to do here - it is already suspended or stopped */
	}
	else if((state & IL_TS_WAIT_SLEEP_JOIN) != 0)
	{
		/* Request a suspend, but otherwise ignore the request */
		thread->state |= IL_TS_SUSPEND_REQUESTED;
	}
	else if((state & (IL_TS_ABORT_REQUESTED | IL_TS_ABORTED |
					  IL_TS_UNSTARTED | IL_TS_STOPPED)) != 0)
	{
		/* We cannot suspend a thread that is being aborted,
		   was never started in the first place, or is done */
		result = 0;
	}
	else if(pthread_equal(thread->threadId, pthread_self()))
	{
		/* Mark the thread as suspended */
		thread->state |= IL_TS_SUSPENDED;
		
		/* Unlock the thread object prior to suspending */
		pthread_mutex_unlock(&(thread->lock));

		/* Suspend until we receive the IL_SIG_RESUME signal */
		SuspendUntilResumed(thread);
		sem_post(&resumeAck);

		/* We are resumed, and the thread object is already unlocked */
		return 1;
	}
	else
	{
		/* Mark the thread as suspended and waiting for a resume */
		thread->state |= IL_TS_SUSPENDED;
		thread->resumeRequested = 0;

		/* Suspend the thread by sending it the IL_SIG_SUSPEND signal */
		pthread_kill(thread->threadId, IL_SIG_SUSPEND);

		/* Wait for "SuspendSignal" to notify us that it has suspended */
		sem_wait(&suspendAck);
	}

	/* Unlock the thread object and return */
	pthread_mutex_unlock(&(thread->lock));
	return result;
}

void ILThreadResume(ILThread *thread)
{
	unsigned int state;

	/* Lock down the thread object */
	pthread_mutex_lock(&(thread->lock));

	/* Determine what to do based on the thread's state */
	state = thread->state;
	if((state & IL_TS_SUSPENDED) != 0)
	{
		/* Wake up the suspended thread by sending it IL_SIG_RESUME */
		thread->resumeRequested = 1;
		pthread_kill(thread->threadId, IL_SIG_RESUME);
		sem_wait(&resumeAck);
		thread->state &= ~IL_TS_SUSPENDED;
	}
	else if((state & IL_TS_SUSPEND_REQUESTED) != 0)
	{
		/* A suspend was requested, but it hadn't started yet */
		thread->state &= ~IL_TS_SUSPEND_REQUESTED;
	}

	/* Unlock the thread object */
	pthread_mutex_unlock(&(thread->lock));
}

void ILThreadInterrupt(ILThread *thread)
{
	unsigned int state;

	/* Lock down the thread object */
	pthread_mutex_lock(&(thread->lock));

	/* Determine what to do based on the thread's state */
	state = thread->state;
	if((state & (IL_TS_INTERRUPTED | IL_TS_STOPPED)) != 0)
	{
		/* Nothing to do here - an interrupt is already pending
		   or the thread is in the stopped state */
	}
	else if((state & IL_TS_WAIT_SLEEP_JOIN) != 0)
	{
		/* The thread is in the "wait/sleep/join" state.  Send it
		   an IL_SIG_INTERRUPT signal, which will cause waiting
		   "pthread_cond_timedwait" calls to abort with EINTR */
		thread->state |= IL_TS_INTERRUPTED;
		pthread_kill(thread->threadId, IL_SIG_INTERRUPT);
	}
	else
	{
		/* Mark the thread as needing to be interrupted the next
		   time a "wait/sleep/join" occurs */
		thread->state |= IL_TS_INTERRUPTED;
	}

	/* Unlock the thread object */
	pthread_mutex_unlock(&(thread->lock));
}

int ILThreadGetBackground(ILThread *thread)
{
	int flag;

	/* Lock down the thread object */
	pthread_mutex_lock(&(thread->lock));

	/* Determine if this is a background thread */
	flag = ((thread->state & IL_TS_BACKGROUND) != 0);

	/* Unlock the thread object and return */
	pthread_mutex_unlock(&(thread->lock));
	return flag;
}

void ILThreadSetBackground(ILThread *thread, int flag)
{
	int change = 0;

	/* Lock down the thread object */
	pthread_mutex_lock(&(thread->lock));

	/* Change the background state of the thread */
	if(flag)
	{
		if((thread->state & IL_TS_BACKGROUND) == 0)
		{
			thread->state |= IL_TS_BACKGROUND;
			change = 1;
		}
	}
	else
	{
		if((thread->state & IL_TS_BACKGROUND) != 0)
		{
			thread->state &= ~IL_TS_BACKGROUND;
			change = -1;
		}
	}

	/* Unlock the thread object */
	pthread_mutex_unlock(&(thread->lock));

	/* Adjust "numBackgroundThreads" */
	pthread_mutex_lock(&threadLockAll);
	numBackgroundThreads += change;
	pthread_mutex_unlock(&threadLockAll);
}

int ILThreadGetState(ILThread *thread)
{
	unsigned int state;

	/* Lock down the thread object */
	pthread_mutex_lock(&(thread->lock));

	/* Retrieve the current thread state */
	state = thread->state;

	/* Unlock the thread object */
	pthread_mutex_unlock(&(thread->lock));

	/* Return the publicly-interesting flags to the caller */
	return (int)(state & IL_TS_PUBLIC_FLAGS);
}

/*
 * Global mutex for atomic operations.
 */
static pthread_mutex_t atomicLock = PTHREAD_MUTEX_INITIALIZER;

void ILThreadAtomicStart(void)
{
	pthread_mutex_lock(&atomicLock);
}

void ILThreadAtomicEnd(void)
{
	pthread_mutex_unlock(&atomicLock);
}

void ILThreadMemoryBarrier(void)
{
	/* TODO */

	/* For now, we just acquire and release a mutex, under the
	   assumption that pthreads will do data synchronisation
	   for us as part of the mutex code */
	pthread_mutex_lock(&atomicLock);
	pthread_mutex_unlock(&atomicLock);
}

#ifdef	__cplusplus
};
#endif

#endif /* IL_USE_PTHREADS */
