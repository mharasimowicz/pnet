/*
 * lib_thread.c - Internalcall methods for "System.Threading.*".
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
 *
 * Contributions from Thong Nguyen <tum@veridicus.com>
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

#include "engine.h"
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 *	Timeout value the managed WaitHandle uses.
 */
#define WIN32_WAIT_TIMEOUT (258L)

/*
 * Maximum number of wait handles that can be processed at once.
 */
#define	IL_MAX_WAIT_HANDLES		64

/*
 * Cache of the System.Threading.ThreadStart.Invoke() method.
 */
static ILMethod *c_ThreadStartInvokeMethod = NULL;

/*
 *	Predeclartion of code that handles thread aborts & interruption.
 */
static void HandleWaitResult(ILExecThread *thread, int result);

/*
 * public static void Enter(Object obj);
 */
void _IL_Monitor_Enter(ILExecThread *thread, ILObject *obj)
{
	_IL_Monitor_InternalTryEnter(thread, obj, IL_WAIT_INFINITE);	
}

/*
 *	Checks if the monitor is longer being used and returns it to the free list and
 * zeros 
 */
static int _IL_Monitor_CheckAndReturnMonitorToFreeList(ILExecThread *thread, ILExecMonitor *monitor, ILObject *obj)
{	
	if (monitor->waiters == 0 && ILWaitMonitorCanClose(monitor->supportMonitor))
	{
		/* Remove the monitor from the object if the object hasn't been assigned
		      a new monitor */
		CompareAndExchangeObjectLockWord(thread, obj, 0, IL_LW_MARK(monitor));
		
		/* Return the monitor to the free list */
		monitor->next = thread->freeMonitor;
		thread->freeMonitor = monitor;

		return 1;
	}

	return 0;
}

/*
 * public static bool InternalTryEnter(Object obj, int timeout);
 */
ILBool _IL_Monitor_InternalTryEnter(ILExecThread *thread,
					   	    		ILObject *obj, ILInt32 timeout)
{
	int result;
	ILLockWord raw;
	ILExecMonitor *monitor, *next;
	
	/* Make sure the object isn't null */
	if (obj == 0)
	{
		ILExecThreadThrowArgNull(thread, "obj");

		return 0;
	}

	/* Make sure the object isn't a value type */
	if (ILType_IsValueType(GetObjectClass(obj)))
	{
		ILExecThreadThrowSystem(thread, "System.ArgumentException",
																	"Arg_IsValueType");

		return 0;
	}

	/* Make sure the timeout is within range */
	if (timeout < -1)
	{
		ILExecThreadThrowArgRange(thread, "timeout", (const char *)0);

		return 0;
	}

retry:

	/* Get the raw lock word */		
	raw = GetObjectLockWord(thread, obj);

	if (raw == 0)
	{
		/* There is no monitor installed for this object */

		monitor = thread->freeMonitor;

		if (monitor == NULL)
		{
			monitor = ILExecMonitorCreate();
		}

		next = monitor->next;
		
		/*	Try to install the new monitor. */
		if (CompareAndExchangeObjectLockWord
			(
				thread,
				obj,
				monitor,
				raw
			) == raw)
		{
			/* Succesfully installed the new monitor */
			
			/* Remove the monitor from the free list */

			thread->freeMonitor = next;

			/* Compete for the monitor */

			goto retry;
		}
		else
		{
			/* A thread managed to install a monitor first.
					Add the monitor to the free list and retry. */

			monitor->next = thread->freeMonitor;
			thread->freeMonitor = monitor;

			goto retry;
		}
	}

	if (IL_LW_MARKED(raw))
	{
		goto retry;
	}

	monitor = IL_LW_UNMARK(raw);

	_IL_Interlocked_Increment_Ri(thread, (ILInt32 *)&monitor->waiters);

	raw = GetObjectLockWord(thread, obj);
	
	if (IL_LW_MARKED(raw))
	{
		_IL_Interlocked_Decrement_Ri(thread, (ILInt32 *)&monitor->waiters);

		_IL_Monitor_CheckAndReturnMonitorToFreeList(thread, monitor, obj);

		goto retry;
	}

	/* If the thread manages to get here then no other thread is going to return
	      obj's monitor to the freelist.  The monitor we have is not stale. */

	/* Try entering the monitor */
	
	result = ILWaitMonitorTryEnter(monitor->supportMonitor, timeout);

	_IL_Interlocked_Decrement_Ri(thread, (ILInt32 *)&monitor->waiters);

	/* Failed or timed out somehow */
	if (result != 0)
	{
		_IL_Monitor_CheckAndReturnMonitorToFreeList(thread, monitor, obj);
	}

	/* Handle ThreadAbort etc */	
	HandleWaitResult(thread, result);

	return result == 0;
}

/*
 * public static void Exit(Object obj);
 */
void _IL_Monitor_Exit(ILExecThread *thread, ILObject *obj)
{
	ILLockWord raw;
	ILExecMonitor *monitor;

	/* Make sure obj isn't null */
	if(obj == 0)
	{
		ILExecThreadThrowArgNull(thread, "obj");

		return;
	}

retry:	
	
	/* Get the monitor */		
	raw = GetObjectLockWord(thread, obj);	
	
	monitor = IL_LW_UNMARK(raw);

	if (monitor == 0)
	{
		/* Hmm.  Can't call Monitor.Exit before Monitor.Enter */

		ILExecThreadThrowSystem
			(
				thread,
				"System.Threading.SynchronizationLockException",	
				"Exception_ThreadNeedsLock"
			);

		return;
	}

	/*
	 *	Problem:
	 *
	 * The following code relies on the fact that no thread is allowed to enter,
	 * wait or exit the monitor.  If this condition is met and there are no and
	 * the monitor nolonger has owners or waiters, we can return the monitor
	 * to the free list.
	 *
	 * Solution:
	 * Use a marker in the object pointer (the first bit).
	 * When this marker is marked, threads attempting to enter/wait/exit will spin.
	 * Note:  Threads attempting to enter/wait/exit need to:
	 *               Check the marker.  If marker is 1 then spin.
	 *               Increment the monitor's waiter count.
	 *               Check the marker.  If the marker is one then decrement and spin.
	 *               Do the enter/wait/exit.
	 */

	if (CompareAndExchangeObjectLockWord(thread, obj, IL_LW_MARK(raw), 
		IL_LW_UNMARK(raw)) != IL_LW_UNMARK(raw))
	{
		/* Some other thread is trying to exit the same object!  Either this or the
			other thread is mistaken.  Spin until the other thread exits enter. */

		goto retry;
	}

	/* From this point on, other new threads attempting to wait, enter or exit 
		the object will spin. */

	/* This is the number of waiters there are.  This can not increase until
		we unmark the lock word, but, it can decrease */
	
	if (ILWaitMonitorSpeculativeLeave(monitor->supportMonitor) == 1 
		/* Note: No need to check for aborts on call to leave */)
	{		
		if (!_IL_Monitor_CheckAndReturnMonitorToFreeList(thread, monitor, obj))
		{
			/* There are waiters */
			/* Simply unmark the lock word */
			
			SetObjectLockWord(thread, obj, IL_LW_UNMARK(raw));			
		}
		else
		{			
			/* There are no waiters.  The monitor has been returned to the free list. */	
		}

		ILWaitMonitorCompleteLeave(monitor->supportMonitor);
	}
	else
	{
		/* We don't own the monitor */
		
		ILExecThreadThrowSystem
			(
				thread,					
				"System.Threading.SynchronizationLockException",
				"Exception_ThreadNeedsLock"
			);

		/* Restore the lockword back to its unoriginal unmarked state */
		SetObjectLockWord(thread, obj, IL_LW_UNMARK(raw));
	}	
}

/*
 * public static bool InternalWait(Object obj, int timeout);
 */
ILBool _IL_Monitor_InternalWait(ILExecThread *thread,
							    ILObject *obj, ILInt32 timeout)
{
	int result;
	ILLockWord raw;
	ILExecMonitor *monitor;

retry:
	
	/* Make sure obj isn't null */
	if (obj == 0)
	{		
		ILExecThreadThrowArgNull(thread, "obj");

		return 0;
	}

	raw = GetObjectLockWord(thread, obj);

	if (IL_LW_MARKED(raw))
	{
		/* Exit is being called on the object's monitor.  We have to spin. */

		goto retry;
	}
	
	monitor = IL_LW_UNMARK(raw);

	if (monitor == 0)
	{
		/* Object doesn't even have a lock */

		ILExecThreadThrowSystem
			(
			thread,
			"System.Threading.SynchronizationLockException",
			"Exception_ThreadNeedsLock"
			);

		return 0;
	}

	/*
	 * Problem:
	 *
	 *	A gets here.
	 * B marks the lock word.
	 * B notices there are no waiters.
	 * B returns obj's monitor to the free list.
	 * A increments waiters and then uses the stale monitor!
	 * B unmarks the lockword.
	 *
	 * Solution:
	 * 
	 * Increment waiters and then double check that lockword isn't marked.
	 * If lockword is marked then reverse the increment and spin.
	 */

	_IL_Interlocked_Increment_Ri(thread, (ILInt32 *)&monitor->waiters);
	
	raw = GetObjectLockWord(thread, obj);

	if (IL_LW_MARKED(raw))
	{
		/* 
		 * Problem:
		 *
		 * A gets here after failing the double check cause B *just* marked the lock word.
		 * B notices there is one waiter.
		 * B doesn't return the monitor to the free list.
		 * A notices that B has marked the lock word.
		 * A decrements the waiter count.
		 * The monitor now has no owner and has not been returned to the free list.
		 * B unmarks the lock word.
		 * "obj" gets garbage collected and the monitor leaks.
		 *
		 * Solution:
		 *
		 * A should return obj's monitor to the free list.
		 */

		_IL_Interlocked_Decrement_Ri(thread, (ILInt32 *)&monitor->waiters);

		/* 
		 * Problem:
		 *
		 * A gets here after failing the double check cause B *just* marked the lock word.
	 	 * B notices there are no waiters.
	 	 * B returns the monitor to the free list.
		 * A notices that B has marked the lock word.
		 * A decrements the waiter count.
		 * B unmarks the lockword.
		 * C enters obj and creates a new monitor for obj.
		 * A returns the obj's new monitor to the free list.
		 * C calls exit and fails because the obj's monitor has been removed.
		 *
		 * Solution:
		 *
		 * Make A free the monitor it knows, not obj's current monitor.
		 * Don't zero obj's lockword if it doesn't match the current lockword.
		 * Both of these are performed in _IL_Monitor_CheckAndReturnMonitorToFreeList
		 */

		_IL_Monitor_CheckAndReturnMonitorToFreeList(thread, monitor, obj);

		goto retry;
	}
	
	/* If we get here then we know the object's monitor can't be removed 
	    by anyone. */

	switch (result = ILWaitMonitorWait(monitor->supportMonitor, timeout))
	{
	case 0:

		/* Thread doesn't currently own lock */

		_IL_Interlocked_Decrement_Ri(thread, (ILInt32 *)&monitor->waiters);

		ILExecThreadThrowSystem
			(
				thread,
				"System.Threading.SynchronizationLockException",
				"Exception_ThreadNeedsLock"
			);
		
		return 0;

	case 1:

		/* Successfully waited */

		_IL_Interlocked_Decrement_Ri(thread, (ILInt32 *)&monitor->waiters);

		return 1;

	case IL_WAIT_TIMEOUT:
	case IL_WAIT_FAILED:

		/* Timed out */

		_IL_Interlocked_Decrement_Ri(thread, (ILInt32 *)&monitor->waiters);
		_IL_Monitor_CheckAndReturnMonitorToFreeList(thread, monitor, obj);

		return 0;

	default:

		/* Handle ThreadAbort etc */

		_IL_Interlocked_Decrement_Ri(thread, (ILInt32 *)&monitor->waiters);

		HandleWaitResult(thread, result);

		return 0;
	}
}

/*
 * public static void Pulse(Object obj);
 */
void _IL_Monitor_Pulse(ILExecThread *thread, ILObject *obj)
{
	int result;
	ILExecMonitor *monitor;

	if(obj)
	{		
		monitor = GetObjectMonitor(thread, obj);

		if (monitor == 0)
		{
			/* Object doesn't even have a lock */

			ILExecThreadThrowSystem
				(
					thread,
					"System.Threading.SynchronizationLockException",
					"Exception_ThreadNeedsLock"
				);

			return;
		}

		switch (result = ILWaitMonitorPulse(monitor->supportMonitor))
		{
		case 0:

			/* Thread doesn't currently own lock */

			ILExecThreadThrowSystem
				(
					thread,
					"System.Threading.SynchronizationLockException",
					"Exception_ThreadNeedsLock"
				);

			return;

		case 1:

			/* Successfully pulsed */

			return;

		default:

			/* Handle ThreadAbort etc */

			HandleWaitResult(thread, result);

			return;
		}
	}
	else
	{
		ILExecThreadThrowArgNull(thread, "obj");
	}
}

/*
 * public static void PulseAll(Object obj);
 */
void _IL_Monitor_PulseAll(ILExecThread *thread, ILObject *obj)
{
	int result;
	ILExecMonitor *monitor;

	if(obj)
	{		
		monitor = GetObjectMonitor(thread, obj);

		if (monitor == 0)
		{
			/* Object doesn't even have a lock */

			ILExecThreadThrowSystem
				(
					thread,
					"System.Threading.SynchronizationLockException",
					"Exception_ThreadNeedsLock"
				);

			return;
		}

		switch (result = ILWaitMonitorPulseAll(monitor->supportMonitor))
		{
		case 0:

			/* Thread doesn't currently own lock */

			ILExecThreadThrowSystem
				(
					thread,
					"System.Threading.SynchronizationLockException",
					"Exception_ThreadNeedsLock"
				);

			return;

		case 1:

			/* Successfully pulsed */

			return;

		default:

			/* Handle ThreadAbort etc */

			HandleWaitResult(thread, result);

			return;
		}
	}
	else
	{
		ILExecThreadThrowArgNull(thread, "obj");
	}
}

/*
 * public static int CompareExchange(ref int location1, int value,
 *									 int comparand);
 */
ILInt32 _IL_Interlocked_CompareExchange_Riii
						(ILExecThread *thread,
						 ILInt32 *location1,
						 ILInt32 value,
						 ILInt32 comparand)
{
	ILInt32 orig;
	ILThreadAtomicStart();
	if((orig = *location1) == comparand)
	{
		*location1 = value;
	}
	ILThreadAtomicEnd();
	return orig;
}

/*
 * public static float CompareExchange(ref float location1, float value,
 *									   float comparand);
 */
ILFloat _IL_Interlocked_CompareExchange_Rfff
						(ILExecThread *thread,
						 ILFloat *location1,
						 ILFloat value,
						 ILFloat comparand)
{
	ILFloat orig;
	ILThreadAtomicStart();
	if((orig = *location1) == comparand)
	{
		*location1 = value;
	}
	ILThreadAtomicEnd();
	return orig;
}

/*
 * public static Object CompareExchange(ref Object location1, Object value,
 *									    Object comparand);
 */
ILObject *_IL_Interlocked_CompareExchange_RObjectObjectObject
						(ILExecThread *thread,
						 ILObject **location1,
						 ILObject *value,
						 ILObject *comparand)
{
	ILObject *orig;
	ILThreadAtomicStart();
	if((orig = *location1) == comparand)
	{
		*location1 = value;
	}
	ILThreadAtomicEnd();
	return orig;
}

/*
 * public static int Decrement(ref int location);
 */
ILInt32 _IL_Interlocked_Decrement_Ri(ILExecThread *thread, ILInt32 *location)
{
	ILInt32 value;
	ILThreadAtomicStart();
	value = --(*location);
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static long Decrement(ref long location);
 */
ILInt64 _IL_Interlocked_Decrement_Rl(ILExecThread *thread, ILInt64 *location)
{
	ILInt64 value;
	ILThreadAtomicStart();
	ILMemCpy(&value, location, sizeof(ILInt64));
	--value;
	ILMemCpy(location, &value, sizeof(ILInt64));
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static int Exchange(ref int location, int value);
 */
ILInt32 _IL_Interlocked_Exchange_Rii(ILExecThread *thread,
								     ILInt32 *location, ILInt32 value)
{
	ILInt32 orig;
	ILThreadAtomicStart();
	orig = *location;
	*location = value;
	ILThreadAtomicEnd();
	return orig;
}

/*
 * public static float Exchange(ref float location, float value);
 */
ILFloat _IL_Interlocked_Exchange_Rff(ILExecThread *thread,
								     ILFloat *location, ILFloat value)
{
	ILFloat orig;
	ILThreadAtomicStart();
	orig = *location;
	*location = value;
	ILThreadAtomicEnd();
	return orig;
}

/*
 * public static Object Exchange(ref Object location, Object value);
 */
ILObject *_IL_Interlocked_Exchange_RObjectObject
				(ILExecThread *thread, ILObject **location, ILObject *value)
{
	ILObject *orig;
	ILThreadAtomicStart();
	orig = *location;
	*location = value;
	ILThreadAtomicEnd();
	return orig;
}

/*
 * public static int Increment(ref int location);
 */
ILInt32 _IL_Interlocked_Increment_Ri(ILExecThread *thread, ILInt32 *location)
{
	ILInt32 value;
	ILThreadAtomicStart();
	value = ++(*location);
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static long Increment(ref long location);
 */
ILInt64 _IL_Interlocked_Increment_Rl(ILExecThread *thread, ILInt64 *location)
{
	ILInt64 value;
	ILThreadAtomicStart();
	ILMemCpy(&value, location, sizeof(ILInt64));
	++value;
	ILMemCpy(location, &value, sizeof(ILInt64));
	ILThreadAtomicEnd();
	return value;
}

/*
 *	 Thread function for ILThread.
 *
 *  @param objectArg	The ILExecThread for the new thread.
 */
static void __PrivateThreadStart(void *objectArg)
{
	ILMethod *method;
	ILExecValue result;
	ILExecThread *thread;

	thread = (ILExecThread *)objectArg;

	/* Get the ThreadStart.Invoke method */

	if (c_ThreadStartInvokeMethod == NULL)
	{
		method = ILExecThreadLookupMethod(thread,
			"System.Threading.ThreadStart", "Invoke", "(T)V");
		
		c_ThreadStartInvokeMethod = method;
	}
	else
	{
		method = c_ThreadStartInvokeMethod;
	}

	/* Invoke the ThreadStart delegate */
	ILExecThreadCall(thread, method, &result, *((ILObject **)thread->clrThread + 2));

	/* Print out any uncaught exceptions */
	if (ILExecThreadHasException(thread))
	{
		ILExecThreadPrintException(thread);
	}
	
	ILExecThreadDestroy(thread);
}

/*
* private void InitializeThread();
*/
void _IL_Thread_InitializeThread(ILExecThread *thread, ILObject *_this)
{
	ILThread *osThread;
	ILExecThread *newThread;

	if (*((ILThread **)_this) != NULL)
	{
		/* InitializeThread can't be called more than once */

		ILExecThreadThrowSystem(thread, "System.Threading.ThreadStateException", (const char *)0);

		return;
	}

	/* Create a new OS-level thread */

	if ((osThread = ILThreadCreate(__PrivateThreadStart, 0)) == 0)
	{
		/* Threading isn't supported */

		ILExecThreadThrowSystem(thread, "System.NotSupportedException", 
			"Exception_ThreadsNotSupported");

		return;
	}

	/* Register the OS-level thread for managed code execution */

	if (ILThreadRegisterForManagedExecution(thread->process, osThread, 1) == 0)
	{
		/* Runtime isn't fully initialized.  This should never happen */

		ILExecThreadThrowSystem(thread, "System.ExecutionEngineException",
			"Exception_UnexpectedEngineState");

		return;
	}

	/* Get the ILExecThread from the OS-level thread */
	/* The ILExecThread should already be associated with the OS-level thread */
	newThread = (ILExecThread *)ILThreadGetObject(osThread);
	
	/* Associate the CLR-level thread with the engine-level thread */
	newThread->clrThread = _this;

	/* Associate the OS-level thread with the CLR-level thread */
	*((ILThread **)_this) = osThread;
}

/*
 * private void FinalizeThread();
 */
void _IL_Thread_FinalizeThread(ILExecThread *thread, ILObject *_this)
{
	/*
	 * TODO:
	 *
	 * Some parts of ILThreadDestroy should probably be performed immediately
	 * after the thread has finished executing (to free up OS handles early).
	 */

	/* This will be null if threads aren't supported and someone creates a Thread */

	if (*((ILThread **)_this))
	{
		ILThreadDestroy(*((ILThread **)_this));
	}
}

/*
 * public void Abort();
 */
void _IL_Thread_Abort(ILExecThread *thread, ILObject *_this)
{	
	ILThreadAbort(*((ILThread **)_this));
}

/*
 * private bool InternalJoin(int timeout);
 */
ILBool _IL_Thread_InternalJoin(ILExecThread *thread, ILObject *_this,
							   ILInt32 timeout)
{	
	switch (ILThreadJoin((*((ILThread **)_this)), timeout))
	{	
	case IL_JOIN_OK:
	case IL_JOIN_SELF:		
		return 1;
	case IL_JOIN_TIMEOUT:		
		return 0;
	case IL_JOIN_MEMORY:		
		ILExecThreadThrowOutOfMemory(thread);
		return 0;
	case IL_JOIN_INTERRUPTED:
		
		ILExecThreadThrowSystem
			(
				thread,
				"System.Threading.ThreadInterruptedException",
				(const char *)0
			);
		
		return 0;

	case IL_JOIN_ABORTED:

		ILExecThreadThrowSystem
			(
				thread,
				"System.Threading.ThreadAbortException",
				(const char *)0
			);

		return 0;

	default:	

		ILExecThreadThrowSystem
			(
				thread,
				"System.Threading.ThreadStateException",
				(const char *)0
			);

		return 0;
	}
}

/*
 * public static void MemoryBarrier();
 */
void _IL_Thread_MemoryBarrier(ILExecThread *thread)
{
	ILThreadMemoryBarrier();
}

/*
 * public static void ResetAbort();
 */
void _IL_Thread_ResetAbort(ILExecThread *thread)
{
	if ((ILThreadGetState(thread->osThread) & IL_TS_ABORT_REQUESTED) != 0)
	{
		/* No abort has been requested */

		ILExecThreadThrowSystem(thread, "System.Threading.ThreadStateException", (const char *)0);

		return;
	}

	ILThreadAbortReset();
}

/*
 * public static void InternalSleep(int timeout);
 */
void _IL_Thread_InternalSleep(ILExecThread *thread, ILInt32 timeout)
{
	ILThreadSleep((ILUInt32)timeout);
}

/*
 * public void Start();
 */
void _IL_Thread_Start(ILExecThread *thread, ILObject *_this)
{
	if ((ILThreadGetState(*((ILThread **)_this)) & IL_TS_UNSTARTED) == 0)
	{
		/* Thread has already been started or has ended. */

		ILExecThreadThrowSystem(thread, "System.Threading.ThreadStateException", 
			"Exception_ThreadAlreadyStarted");

		return;
	}

	ILThreadStart(*((ILThread **)_this));
}

/*
 * public static Thread InternalCurrentThread();
 */
ILObject *_IL_Thread_InternalCurrentThread(ILExecThread *thread)
{
	return _ILGetCurrentClrThread(thread);
}

/*
 * public void InternalSetBackground(bool value);
 */
void _IL_Thread_InternalSetBackground(ILExecThread *thread,
									  ILObject *_this, ILBool value)
{
	ILThreadSetBackground(*((ILThread **)_this), value);
}

/*
 * public ThreadPriority InternalGetPriority();
 */
ILInt32 _IL_Thread_InternalGetPriority(ILExecThread *thread, ILObject *_this)
{
	if ((ILThreadGetState(*((ILThread **)_this)) 
			& (IL_TS_ABORTED | IL_TS_ABORT_REQUESTED | IL_TS_STOPPED)) != 0)
	{
		ILExecThreadThrowSystem(thread, "System.Threading.ThreadStateException", (const char *)0);

		return 1;
	}
	
	/* TODO */

	return 2;	/* Normal */
}

/*
 * public void InternalSetPriority(ThreadPriority priority);
 */
void _IL_Thread_InternalSetPriority(ILExecThread *thread, ILObject *_this,
									ILInt32 priority)
{
	if ((ILThreadGetState(*((ILThread **)_this))
			& (IL_TS_ABORTED | IL_TS_ABORT_REQUESTED | IL_TS_STOPPED)) != 0)
	{
		ILExecThreadThrowSystem(thread, "System.Threading.ThreadStateException", (const char *)0);

		return;
	}
}

/*
 * public ThreadState InternalGetState();
 */
ILInt32 _IL_Thread_InternalGetState(ILExecThread *thread, ILObject *_this)
{
	return ILThreadGetState(*((ILThread **)_this));
}

/*
 * internal static bool CanStartThreads();
 */
ILBool _IL_Thread_CanStartThreads(ILExecThread *_thread)
{
	return ILHasThreads();
}

/*
 * public static sbyte VolatileRead(ref sbyte address);
 */
ILInt8 _IL_Thread_VolatileRead_Rb(ILExecThread *thread, ILInt8 *address)
{
	ILInt8 value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static byte VolatileRead(ref byte address);
 */
ILUInt8 _IL_Thread_VolatileRead_RB(ILExecThread *thread, ILUInt8 *address)
{
	ILUInt8 value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static short VolatileRead(ref short address);
 */
ILInt16 _IL_Thread_VolatileRead_Rs(ILExecThread *thread, ILInt16 *address)
{
	ILInt16 value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static ushort VolatileRead(ref ushort address);
 */
ILUInt16 _IL_Thread_VolatileRead_RS(ILExecThread *thread, ILUInt16 *address)
{
	ILUInt16 value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static int VolatileRead(ref int address);
 */
ILInt32 _IL_Thread_VolatileRead_Ri(ILExecThread *thread, ILInt32 *address)
{
	ILInt32 value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static uint VolatileRead(ref uint address);
 */
ILUInt32 _IL_Thread_VolatileRead_RI(ILExecThread *thread, ILUInt32 *address)
{
	ILUInt32 value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static long VolatileRead(ref long address);
 */
ILInt64 _IL_Thread_VolatileRead_Rl(ILExecThread *thread, ILInt64 *address)
{
	ILInt64 value;
	ILThreadAtomicStart();
	ILMemCpy(&value, address, sizeof(ILInt64));
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static ulong VolatileRead(ref ulong address);
 */
ILUInt64 _IL_Thread_VolatileRead_RL(ILExecThread *thread, ILUInt64 *address)
{
	ILUInt64 value;
	ILThreadAtomicStart();
	ILMemCpy(&value, address, sizeof(ILUInt64));
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static IntPtr VolatileRead(ref IntPtr address);
 */
ILNativeInt _IL_Thread_VolatileRead_Rj(ILExecThread *thread,
									   ILNativeInt *address)
{
	ILNativeInt value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static UIntPtr VolatileRead(ref UIntPtr address);
 */
ILNativeUInt _IL_Thread_VolatileRead_RJ(ILExecThread *thread,
										ILNativeUInt *address)
{
	ILNativeUInt value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static float VolatileRead(ref float address);
 */
ILFloat _IL_Thread_VolatileRead_Rf(ILExecThread *thread, ILFloat *address)
{
	ILFloat value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static double VolatileRead(ref double address);
 */
ILDouble _IL_Thread_VolatileRead_Rd(ILExecThread *thread, ILDouble *address)
{
	ILDouble value;
	ILThreadAtomicStart();
	ILMemCpy(&value, address, sizeof(ILDouble));
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static Object VolatileRead(ref Object address);
 */
ILObject *_IL_Thread_VolatileRead_RObject(ILExecThread *thread,
										  ILObject **address)
{
	ILObject *value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static void VolatileWrite(ref sbyte address, sbyte value);
 */
void _IL_Thread_VolatileWrite_Rbb(ILExecThread *thread,
								  ILInt8 *address, ILInt8 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref byte address, byte value);
 */
void _IL_Thread_VolatileWrite_RBB(ILExecThread *thread,
								  ILUInt8 *address, ILUInt8 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref short address, short value);
 */
void _IL_Thread_VolatileWrite_Rss(ILExecThread *thread,
								  ILInt16 *address, ILInt16 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref ushort address, ushort value);
 */
void _IL_Thread_VolatileWrite_RSS(ILExecThread *thread,
								  ILUInt16 *address, ILUInt16 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref int address, int value);
 */
void _IL_Thread_VolatileWrite_Rii(ILExecThread *thread,
								  ILInt32 *address, ILInt32 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref uint address, uint value);
 */
void _IL_Thread_VolatileWrite_RII(ILExecThread *thread,
								  ILUInt32 *address, ILUInt32 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref long address, long value);
 */
void _IL_Thread_VolatileWrite_Rll(ILExecThread *thread,
								  ILInt64 *address, ILInt64 value)
{
	ILThreadAtomicStart();
	ILMemCpy(address, &value, sizeof(ILInt64));
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref ulong address, ulong value);
 */
void _IL_Thread_VolatileWrite_RLL(ILExecThread *thread,
								  ILUInt64 *address, ILUInt64 value)
{
	ILThreadAtomicStart();
	ILMemCpy(address, &value, sizeof(ILUInt64));
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref IntPtr address, IntPtr value);
 */
void _IL_Thread_VolatileWrite_Rjj(ILExecThread *thread,
								  ILNativeInt *address, ILNativeInt value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref UIntPtr address, UIntPtr value);
 */
void _IL_Thread_VolatileWrite_RJJ(ILExecThread *thread,
								  ILNativeUInt *address, ILNativeUInt value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref float address, float value);
 */
void _IL_Thread_VolatileWrite_Rff(ILExecThread *thread,
								  ILFloat *address, ILFloat value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref double address, double value);
 */
void _IL_Thread_VolatileWrite_Rdd(ILExecThread *thread,
								  ILDouble *address, ILDouble value)
{
	ILThreadAtomicStart();
	ILMemCpy(address, &value, sizeof(ILDouble));
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref Object address, Object value);
 */
void _IL_Thread_VolatileWrite_RObjectObject(ILExecThread *thread,
											ILObject **address, ILObject *value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * internal static int InternalGetThreadId();
 */
ILInt32 _IL_Thread_InternalGetThreadId(ILExecThread *_thread)
{
	return (ILInt32)(*((ILThread **)_thread));
}

/*
 * private static void InternalClose(IntPtr privateData);
 */
void _IL_WaitHandle_InternalClose(ILExecThread *_thread,
								  ILNativeInt privateData)
{
	if(privateData)
	{
		ILWaitHandleClose((ILWaitHandle *)privateData);
	}
}

/*
 * Handle the result from an "ILWait*" function call.
 */
static void HandleWaitResult(ILExecThread *thread, int result)
{
	if(result == IL_WAIT_INTERRUPTED)
	{
		ILExecThreadThrowSystem
			(thread, "System.Threading.ThreadInterruptedException",
			 (const char *)0);
	}
	else if(result == IL_WAIT_ABORTED)
	{
		/* Determine if we currently have an abort in progress,
		   or if we need to throw a new abort exception */
		if(ILThreadIsAborting())
		{
			if(ILThreadAbort(ILThreadSelf()))
			{
				/* Allocate an instance of "ThreadAbortException" and throw */
				/* TODO */

				ILExecThreadThrowSystem
					(
						thread,
						"System.Threading.ThreadAbortException",
						(const char *)0
					);
			}
		}
	}
}

/*
 * Convert a C# array of wait handles into a C array.
 * Returns zero if something is wrong.
 */
static int ConvertWaitHandleArray(ILExecThread *_thread,
								  System_Array *waitHandles,
								  ILWaitHandle **handles)
{
	ILInt32 index;

	/* Validate the length of the incoming array */
	if(waitHandles->length > IL_MAX_WAIT_HANDLES)
	{
		ILExecThreadThrowSystem
			(_thread, "System.NotSupportedException",
			 (const char *)0);
		return 0;
	}

	/* Convert the WaitHandle objects into system wait handles */
	for(index = 0; index < waitHandles->length; ++index)
	{
		handles[index] = *((ILWaitHandle **)
			(((ILObject **)(ArrayToBuffer(waitHandles)))[index]));
		if(handles[index] == 0)
		{
			ILExecThreadThrowArgNull(_thread, "waitHandles");
			return 0;
		}
	}

	/* Ready to go */
	return 1;
}

/*
 * private static bool InternalWaitAll(WaitHandle[] waitHandles,
 *									   int timeout, bool exitContext);
 */
ILBool _IL_WaitHandle_InternalWaitAll(ILExecThread *_thread,
									  System_Array *waitHandles,
									  ILInt32 timeout,
									  ILBool exitContext)
{
	ILWaitHandle *handles[IL_MAX_WAIT_HANDLES];
	int result;

	/* Return immediately if there are no handles */
	if(!(waitHandles->length))
	{
		return 1;
	}

	/* Convert the WaitHandle objects into system wait handles */
	if(!ConvertWaitHandleArray(_thread, waitHandles, handles))
	{
		return 0;
	}

	/* Perform the wait */
	result = ILWaitAll(handles, (ILUInt32)(waitHandles->length), timeout);
	HandleWaitResult(_thread, result);
	return (result == 0);
}

/*
 * private static int InternalWaitAny(WaitHandle[] waitHandles,
 *									  int timeout, bool exitContext);
 */
ILInt32 _IL_WaitHandle_InternalWaitAny(ILExecThread *_thread,
									   System_Array *waitHandles,
									   ILInt32 timeout, ILBool exitContext)
{
	ILWaitHandle *handles[IL_MAX_WAIT_HANDLES];
	int result;

	/* Return immediately if there are no handles */
	if(!(waitHandles->length))
	{
		return 1;
	}

	/* Convert the WaitHandle objects into system wait handles */
	if(!ConvertWaitHandleArray(_thread, waitHandles, handles))
	{
		return 0;
	}

	/* Perform the wait */
	result = ILWaitAny(handles, (ILUInt32)(waitHandles->length), timeout);
	HandleWaitResult(_thread, result);
	/*return (result == 0);*/
	
	/* Now returns index of the handle that was set and proper timeout value */

	if (result == IL_WAIT_TIMEOUT)
	{
		result = WIN32_WAIT_TIMEOUT;
	}
	else if (result < 0)
	{
		/* TODO: FIXME: Docs don't state how to report an error! */
	}

	return result;
}

/*
 * private static bool InternalWaitOne(IntPtr privateData, int timeout);
 */
ILBool _IL_WaitHandle_InternalWaitOne(ILExecThread *_thread,
									  ILNativeInt privateData,
									  ILInt32 timeout)
{
	if(privateData)
	{
		int result = ILWaitOne((ILWaitHandle *)privateData, timeout);
		HandleWaitResult(_thread, result);
		return (result == 0);
	}
	return 0;
}

/*
 * private static IntPtr InternalCreateMutex(bool initiallyOwned,
 *											 String name,
 *                                           out bool gotOwnership);
 */
ILNativeInt _IL_Mutex_InternalCreateMutex(ILExecThread *_thread,
										  ILBool initiallyOwned,
										  ILString *name,
										  ILBool *gotOwnership)
{
	ILWaitHandle *handle;
	if(!name)
	{
		/* Create an ordinary mutex */
		handle = ILWaitMutexCreate(initiallyOwned);
		*gotOwnership = initiallyOwned;
	}
	else
	{
		/* Create a named mutex */
		char *nameStr = ILStringToUTF8(_thread, name);
		if(nameStr)
		{
			int gotOwn = 0;
			handle = ILWaitMutexNamedCreate(nameStr, initiallyOwned, &gotOwn);
			*gotOwnership = (ILBool)gotOwn;
		}
		else
		{
			handle = 0;
		}
	}
	if(handle)
	{
		return (ILNativeInt)handle;
	}
	else
	{
		ILExecThreadThrowOutOfMemory(_thread);
		return 0;
	}
}

/*
 * private static void InternalReleaseMutex(IntPtr mutex);
 */
void _IL_Mutex_InternalReleaseMutex(ILExecThread *_thread, ILNativeInt mutex)
{
	if(mutex != 0)
	{
		ILWaitMutexRelease((ILWaitHandle *)mutex);
	}
}

/*
 * Internal WaitEvent methods.
 */

/*
 * internal static extern IntPtr InternalCreateEvent(bool manualReset, bool initialState);
 */
ILNativeInt _IL_WaitEvent_InternalCreateEvent(ILExecThread *_thread, ILBool manualReset, ILBool initialState)
{
	ILWaitHandle *event;

	event = ILWaitEventCreate((int)manualReset, (int)initialState);

	if (event == 0)
	{
		ILExecThreadThrowOutOfMemory(_thread);
	}

	return (ILNativeInt)event;
}

/*
 * internal static extern bool InternalSetEvent(IntPtr handle);
 */
ILBool _IL_WaitEvent_InternalResetEvent(ILExecThread *_thread, ILNativeInt event)
{
	return (ILBool)ILWaitEventReset((ILWaitHandle *)event);
}

/*
 * internal static extern bool InternalResetEvent(IntPtr handle);
 */
ILBool _IL_WaitEvent_InternalSetEvent(ILExecThread *_thread, ILNativeInt event)
{
	return (ILBool)ILWaitEventSet((ILWaitHandle *)event);
}

#ifdef	__cplusplus
};
#endif
