/*
 * lib_profiling.c - Internalcall methods for the "DotGNU.Profiling" class.
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

#include "engine_private.h"
#include "lib_defs.h"
#include "il_utils.h"
#include "il_sysio.h"
#ifndef IL_WITHOUT_TOOLS
#include "il_dumpasm.h"
#endif
#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_PWD_H
#include <pwd.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)

/*
 * Dump the profiling information for the given methods.
 * A NULL method marks the end of the list.
 * Returns 1 if profiling information was dumped otherwise 0.
 */
int _ILProfilingDump(FILE *stream, ILMethod **methods)
{
	ILMethod *method;
	ILMethod **temp;
	int haveCounts = 0;

	if(!methods)
	{
		return 0;
	}

	/* Sort the method list into decreasing order of count */
	if(methods[0] != 0 && methods[1] != 0)
	{
		ILMethod **outer;
		ILMethod **inner;
		for(outer = methods; outer[1] != 0; ++outer)
		{
			for(inner = outer + 1; inner[0] != 0; ++inner)
			{
				if(outer[0]->count < inner[0]->count)
				{
					method = outer[0];
					outer[0] = inner[0];
					inner[0] = method;
				}
			}
		}
	}

	/* Print the method information */
	temp = methods;
#ifdef ENHANCED_PROFILER
	printf ("Count      Total time Average time Method\n");
#else
	printf ("Count      Method\n");
#endif
	while((method = *temp++) != 0)
	{
		if(!(method->count))
		{
			continue;
		}
#ifdef ENHANCED_PROFILER
		printf("%-11lu%-11lu%-13lu", (unsigned long)(method->count),
			(unsigned long)(method->time), (unsigned long)(method->time) / (unsigned long)(method->count));
#else
 		printf("%-11lu", (unsigned long)(method->count));
#endif
		ILDumpMethodType(stdout, ILProgramItem_Image(method),
						 ILMethod_Signature(method), 0,
						 ILMethod_Owner(method), ILMethod_Name(method), 0);
		putc('\n', stdout);
		haveCounts = 1;
	}

	return haveCounts;
}

#ifdef ENHANCED_PROFILER
/*
 * Get the start profiling timestamp.
 */
void _ILProfilingStart(ILCurrTime *timestamp)
{
	ILGetSinceRebootTime(timestamp);
}

/*
 * End profiling for the method given and add the difference between the
 * timestamp given and the current time to the total execution time of
 * the method.
 * Increase the execution counter here too so that the number of executions
 * and spent time in the method matches.
 */
void _ILProfilingEnd(ILMethod *method, ILCurrTime *startTimestamp)
{
	ILCurrTime timestamp;
	ILInt64 seconds;
	ILUInt32 nanoSeconds;
	ILUInt32 timeDiff;

	if(!method || !startTimestamp)
	{
		return;
	}

	ILGetSinceRebootTime(&timestamp);

	if(timestamp.nsecs < startTimestamp->nsecs)
	{
		seconds = timestamp.secs - startTimestamp->secs - 1;
		nanoSeconds = (timestamp.nsecs + 1000000000) - startTimestamp->nsecs;
	}
	else
	{
		seconds = timestamp.secs - startTimestamp->secs;
		nanoSeconds = timestamp.nsecs - startTimestamp->nsecs;
	}

	/* calculate the timedifference in microseconds */
	timeDiff = (seconds * 1000000) + (nanoSeconds / 1000);

	method->time += timeDiff;
	++method->count;
}
#endif /* ENHANCED_PROFILER */

#endif /* !IL_CONFIG_REDUCE_CODE && !IL_WITHOUT_TOOLS */

/*
 * public static void StartProfiling();
 */
void _IL_Profiling_StartProfiling(ILExecThread *thread)
{
#ifdef ENHANCED_PROFILER
	thread->profilingEnabled = 1;
#endif
}

/*
 * public static void StopProfiling();
 */
void _IL_Profiling_StopProfiling(ILExecThread *thread)
{
#ifdef ENHANCED_PROFILER
	thread->profilingEnabled = 0;
#endif
}

/*
 * public static bool IsProfilingEnabled();
 */
ILBool _IL_Profiling_IsProfilingEnabled(ILExecThread *thread)
{
#ifdef ENHANCED_PROFILER
	return (ILBool) thread->profilingEnabled;
#else
	return (ILBool) 0;
#endif
}

/*
 * public static bool IsProfilingSupported();
 */
ILBool _IL_Profiling_IsProfilingSupported(ILExecThread *thread)
{
#ifdef ENHANCED_PROFILER
	return (ILBool) (ILCoderGetFlags (thread->process->coder) & IL_CODER_FLAG_METHOD_PROFILE);
#else
	return (ILBool) 0;
#endif
}

#ifdef	__cplusplus
};
#endif
