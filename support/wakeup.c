/*
 * wakeup.c - Implementation of "thread wakeup" objects and queues.
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

/*

Wakeup objects are similar to condition variables, except that they
also support "signal at N" and "interrupt" semantics.

*/

#include "thr_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

void _ILWakeupCreate(_ILWakeup *wakeup)
{
	_ILCondMutexCreate(&(wakeup->lock));
	_ILCondVarCreate(&(wakeup->condition));
	wakeup->count = 0;
	wakeup->limit = 0;
	wakeup->interrupted = 0;
	wakeup->object = 0;
}

void _ILWakeupDestroy(_ILWakeup *wakeup)
{
	_ILCondVarDestroy(&(wakeup->condition));
	_ILCondMutexDestroy(&(wakeup->lock));
}

int _ILWakeupWait(_ILWakeup *wakeup, ILUInt32 ms,
				  ILUInt32 limit, void **object)
{
	int result;

	/* Lock down the wakeup object.  We use an "unsafe" mutex lock
	   because we will be giving up the lock later on.  It is OK
	   to do this because the current thread will always be in the
	   "wait/sleep/join" state, which prevents thread suspension */
	_ILCondMutexLockUnsafe(&(wakeup->lock));

	/* Is there a pending interrupt? */
	if(!(wakeup->interrupted))
	{
		/* Reset the control variables */
		wakeup->count = 0;
		wakeup->limit = limit;
		wakeup->object = 0;

		/* Give up the lock and wait for someone to signal us */
		if(_ILCondVarTimedWait(&(wakeup->condition), &(wakeup->lock), ms))
		{
			if(wakeup->interrupted)
			{
				/* The wakeup object was interrupted */
				wakeup->interrupted = 0;
				result = -1;
			}
			else
			{
				/* All signals that we were expecting have arrived */
				if(object)
				{
					/* Return the last object that was signalled */
					*object = wakeup->object;
				}
				result = 1;
			}
		}
		else
		{
			/* The wakeup object timed out.  We still check for interrupt
			   because we may have been interrupted just after timeout,
			   but before this thread was re-awoken */
			result = (wakeup->interrupted ? -1 : 0);
			wakeup->interrupted = 0;
		}
		wakeup->count = 0;
		wakeup->limit = 0;
	}
	else
	{
		/* The thread was already interrupted before we got this far */
		wakeup->interrupted = 0;
		result = -1;
	}

	/* Unlock the wakeup object and return */
	_ILCondMutexUnlockUnsafe(&(wakeup->lock));
	return result;

}

int _ILWakeupSignal(_ILWakeup *wakeup, void *object)
{
	int result;

	/* Lock down the wakeup object */
	_ILCondMutexLock(&(wakeup->lock));

	/* Determine what to do based on the wakeup object's state */
	if(wakeup->interrupted || wakeup->count >= wakeup->limit)
	{
		/* The wakeup was interrupted or we have already reached the limit */
		result = 0;
	}
	else
	{
		/* Increase the wakeup count */
		++(wakeup->count);

		/* Record the object to be returned from "_ILWakeupWait" */
		wakeup->object = object;

		/* Signal the waiting thread if we have reached the limit */
		if(wakeup->count >= wakeup->limit)
		{
			_ILCondVarSignal(&(wakeup->condition));
		}

		/* The signal operation succeeded */
		result = 1;
	}

	/* Unlock the wakeup object and return */
	_ILCondMutexUnlock(&(wakeup->lock));
	return result;
}

void _ILWakeupInterrupt(_ILWakeup *wakeup)
{
	/* Lock down the wakeup object */
	_ILCondMutexLock(&(wakeup->lock));

	/* Nothing to do if the thread is already marked for interruption */
	if(!(wakeup->interrupted))
	{
		/* Mark the thread for interruption */
		wakeup->interrupted = 1;

		/* Signal anyone who is waiting that interruption occurred */
		if(wakeup->count < wakeup->limit || wakeup->limit == 0)
		{
			_ILCondVarSignal(&(wakeup->condition));
		}
	}

	/* Unlock the wakeup object */
	_ILCondMutexUnlock(&(wakeup->lock));
}

void _ILWakeupQueueCreate(_ILWakeupQueue *queue)
{
	queue->first = 0;
	queue->last = 0;
	queue->spaceUsed = 0;
}

void _ILWakeupQueueDestroy(_ILWakeupQueue *queue)
{
	_ILWakeupItem *item, *next;
	item = queue->first;
	while(item != 0)
	{
		next = item->next;
		if(item != &(queue->space))
		{
			ILFree(item);
		}
		item = next;
	}
	queue->first = 0;
	queue->last = 0;
	queue->spaceUsed = 0;
}

int _ILWakeupQueueAdd(_ILWakeupQueue *queue, _ILWakeup *wakeup, void *object)
{
	_ILWakeupItem *item;

	/* Allocate space for the item */
	if(!(queue->spaceUsed))
	{
		/* Reuse the pre-allocated "space" item */
		item = &(queue->space);
		queue->spaceUsed = 1;
	}
	else if((item = (_ILWakeupItem *)ILMalloc(sizeof(_ILWakeupItem))) == 0)
	{
		/* Out of memory */
		return 0;
	}

	/* Populate the item and add it to the queue */
	item->next = 0;
	item->wakeup = wakeup;
	item->object = object;
	if(queue->first)
	{
		queue->last->next = item;
	}
	else
	{
		queue->first = item;
	}
	queue->last = item;
	return 1;
}

void _ILWakeupQueueRemove(_ILWakeupQueue *queue, _ILWakeup *wakeup)
{
	_ILWakeupItem *item, *prev;
	item = queue->first;
	prev = 0;
	while(item != 0)
	{
		if(item->wakeup == wakeup)
		{
			if(prev)
			{
				prev->next = item->next;
			}
			else
			{
				queue->first = item->next;
			}
			if(!(item->next))
			{
				queue->last = prev;
			}
			if(item != &(queue->space))
			{
				ILFree(item);
			}
			else
			{
				queue->spaceUsed = 0;
			}
			break;
		}
		prev = item;
		item = item->next;
	}
}

int _ILWakeupQueueWake(_ILWakeupQueue *queue)
{
	_ILWakeupItem *item, *next;
	int woken = 0;
	item = queue->first;
	while(item != 0 && !woken)
	{
		next = item->next;
		if(_ILWakeupSignal(item->wakeup, item->object))
		{
			woken = 1;
		}
		if(item != &(queue->space))
		{
			ILFree(item);
		}
		else
		{
			queue->spaceUsed = 0;
		}
		item = next;
	}
	queue->first = item;
	if(!item)
	{
		queue->last = 0;
	}
	return woken;
}

void _ILWakeupQueueWakeAll(_ILWakeupQueue *queue)
{
	_ILWakeupItem *item, *next;
	item = queue->first;
	while(item != 0)
	{
		next = item->next;
		_ILWakeupSignal(item->wakeup, item->object);
		if(item != &(queue->space))
		{
			ILFree(item);
		}
		item = next;
	}
	queue->first = 0;
	queue->last = 0;
	queue->spaceUsed = 0;
}

#ifdef	__cplusplus
};
#endif
