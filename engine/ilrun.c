/*
 * ilrun.c - Command-line version of the runtime engine.
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

#include "il_engine.h"
#include "il_system.h"
#include "il_image.h"
#include "il_utils.h"
#if defined(HAVE_UNISTD_H) && !defined(_MSC_VER)
#include <unistd.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Imports from "register.c".
 */
int _ILRegisterWithKernel(const char *progname);
int _ILUnregisterFromKernel(void);

/*
 * Imports from "cvm_dasm.c".
 */
int _ILDumpInsnProfile(FILE *stream);
int _ILDumpVarProfile(FILE *stream);

/*
 * Imports from "cvmc.c".
 */
int _ILDumpMethodProfile(FILE *stream, ILExecProcess *process);


/*
 * Table of command-line options.
 */
static ILCmdLineOption const options[] = {
	{"-H", 'H', 1, 0, 0},
	{"--heap-size", 'H', 1,
		"--heap-size value       or -H value",
		"Set the maximum size of the heap to `value' kilobytes."},
	{"-S", 'S', 1, 0, 0},
	{"--stack-size", 'S', 1, 
	        "--stack-size value  or -S value",
	        "Set the operation stack size to `value' kilobytes."},
	{"-L", 'L', 1, 0, 0},
	{"--library-dir", 'L', 1,
		"--library-dir dir       or -L dir",
		"Specify a directory to search for libraries."},
#if defined(linux) || defined(__linux) || defined(__linux__)
	{"--register", 'r', 0,
		"--register [fullpath]",
		"Register ilrun with the operating system."},
	{"--unregister", 'u', 0,
		"--unregister",
		"Unregister ilrun from the operating system."},
#endif
	{"-v", 'v', 0, 0, 0},
	{"--version", 'v', 0,
		"--version               or -v",
		"Print the version of the program"},
	{"--help", 'h', 0,
		"--help",
		"Print this help message."},

#ifndef IL_CONFIG_REDUCE_CODE
	/* Special internal options that are used for debugging.
	   Note: -I won't do anything unless IL_PROFILE_CVM_INSNS
	   is defined in "cvm.c", -M won't do anything unless
	   IL_PROFILE_CVM_METHODS is defined in "cvm.c", and -V
	   won't do anything unless IL_PROFILE_CVM_VAR_USAGE is
	   defined in "cvm.c" */
	{"-I", 'I', 0, 0, 0},
	{"--insn-profile", 'I', 0, 0, 0},
	{"-M", 'M', 0, 0, 0},
	{"--method-profile", 'M', 0, 0, 0},
	{"-V", 'V', 0, 0, 0},
	{"--var-profile", 'V', 0, 0, 0},
	{"-P", 'P', 0, 0, 0},
	{"--dump-params", 'P', 0, 0, 0},
#endif

	{0, 0, 0, 0, 0}
};

static void usage(const char *progname);
static void version(void);

int main(int argc, char *argv[])
{
	char *progname = argv[0];
	unsigned long heapSize = IL_CONFIG_GC_HEAP_SIZE;
	unsigned long stackSize = IL_CONFIG_STACK_SIZE;
	char **libraryDirs;
	int numLibraryDirs;
	int state, opt;
	char *param;
	ILExecProcess *process;
	ILMethod *method;
	int error;
	ILInt32 retval;
	ILExecThread *thread;
	ILObject *args;
	int sawException;
	int registerMode = 0;
	char *ilprogram;
	int ilprogramLen;
	int flags=0;
#ifndef IL_CONFIG_REDUCE_CODE
	int dumpInsnProfile = 0;
	int dumpVarProfile = 0;
	int dumpMethodProfile = 0;
	int dumpParams = 0;
#endif

	/* Initialize the locale routines */
	ILInitLocale();

	/* Allocate space for the library list */
	libraryDirs = (char **)ILMalloc(sizeof(char *) * argc);
	numLibraryDirs = 0;

	/* Parse the command-line arguments */
	state = 0;
	while((opt = ILCmdLineNextOption(&argc, &argv, &state,
									 options, &param)) != 0)
	{
		switch(opt)
		{
			case 'S':
			{
				stackSize = 0;
				while(*param >= '0' && *param <= '9')
				{
					stackSize = stackSize * 10 + (unsigned long)(*param - '0');
					++param;
				}
				stackSize *= 1024;
			}
			break;

			case 'H':
			{
				heapSize = 0;
				while(*param >= '0' && *param <= '9')
				{
					heapSize = heapSize * 10 + (unsigned long)(*param - '0');
					++param;
				}
				heapSize *= 1024;
			}
			break;

			case 'L':
			{
				if(libraryDirs != 0)
				{
					libraryDirs[numLibraryDirs++] = param;
				}
			}
			break;

			case 'r': case 'u':
			{
				registerMode = opt;
			}
			break;

		#ifndef IL_CONFIG_REDUCE_CODE
			case 'I':
			{
				dumpInsnProfile = 1;
			}
			break;

			case 'M':
			{
				/*TODO*/
				/*flags |= IL_CODER_FLAG_METHOD_PROFILE;
				 *Should have been something like the above
				 *but for making it work temporarily, doing this
				 **/
				flags |= 2;
				dumpMethodProfile = 1;
			}
			break;

			case 'V':
			{
				dumpVarProfile = 1;
			}
			break;

			case 'P':
			{
				dumpParams = 1;
			}
			break;
		#endif

			case 'v':
			{
				version();
				return 0;
			}
			/* Not reached */

			default:
			{
				usage(progname);
				return 1;
			}
			/* Not reached */
		}
	}

	/* Check for register/unregister modes */
	if(registerMode == 'r')
	{
		if(argc <= 1)
		{
			return _ILRegisterWithKernel(progname);
		}
		else
		{
			return _ILRegisterWithKernel(argv[1]);
		}
	}
	else if(registerMode == 'u')
	{
		return _ILUnregisterFromKernel();
	}

	/* We need at least one input file argument */
	if(argc <= 1)
	{
		usage(progname);
		return 1;
	}

	/* Initialize the engine and set the maximum heap size */
	ILExecInit(heapSize);

	/* Create a process to load the program into */
	process = ILExecProcessCreate(stackSize);
	if(!process)
	{
		fprintf(stderr, "%s: could not create process\n", progname);
		return 1;
	}

	ILExecProcessSetCoderFlags(process,flags);

	/* Set the list of directories to use for path searching */
	if(numLibraryDirs > 0)
	{
		ILExecProcessSetLibraryDirs(process, libraryDirs, numLibraryDirs);
	}

	/* Get the name of the IL program, appending ".exe" if necessary */
	ilprogram = argv[1];
	ilprogramLen = strlen(ilprogram);
	if(!ILFileExists(ilprogram, (char **)0) &&
	   (ilprogramLen < 4 ||
	    ILStrICmp(ilprogram + ilprogramLen - 4, ".exe") != 0))
	{
		ilprogram = (char *)ILMalloc(ilprogramLen + 5);
		if(ilprogram)
		{
			strcpy(ilprogram, argv[1]);
			strcat(ilprogram, ".exe");
		}
		else
		{
			ilprogram = argv[1];
		}
	}

	/* Attempt to load the program into the process */
	error = ILExecProcessLoadFile(process, ilprogram);
	if(error < 0)
	{
		perror(ilprogram);
		return 1;
	}
	else if(error == IL_LOADERR_NOT_IL)
	{
		/* This is a regular Windows executable */
		argv[1] = ilprogram;
	#ifndef IL_WIN32_PLATFORM
		/* Hand the program off to Wine for execution */
		argv[0] = getenv("WINE");
		if(!(argv[0]))
		{
			argv[0] = "wine";
		}
		execvp(argv[0], argv);
		perror(argv[0]);
		return 1;
	#else
		/* We are running under Windows, so execute the program directly */
		return ILSpawnProcess(argv + 1);
	#endif
	}
	else if(error > 0)
	{
		fprintf(stderr, "%s: %s\n", ilprogram, ILImageLoadError(error));
		return 1;
	}

	/* Find the entry point method */
	method = ILExecProcessGetEntry(process);
	if(!method)
	{
		fprintf(stderr, "%s: no program entry point\n", ilprogram);
		ILExecProcessDestroy(process);
		return 1;
	}

	/* Validate the entry point */
	if(ILExecProcessEntryType(method) == IL_ENTRY_INVALID)
	{
		fprintf(stderr, "%s: invalid entry point\n", ilprogram);
		ILExecProcessDestroy(process);
		return 1;
	}

	/* Convert the arguments into an array of strings */
	thread = ILExecProcessGetMain(process);
	args = ILExecProcessSetCommandLine(process, ilprogram, argv + 2);

	/* Call the entry point */
	sawException = 0;
	if(args != 0 && !ILExecThreadHasException(thread))
	{
		retval = 0;
		if(ILExecThreadCall(thread, method, &retval, args))
		{
			/* An exception was thrown while executing the program */
			sawException = 1;
			retval = 1;
		}
	}
	else
	{
		/* An exception was thrown while building the argument array */
		retval = 1;
	}

	/* Print the top-level exception that occurred */
	if(sawException)
	{
		ILExecThreadPrintException(thread);
	}

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	/* Print profile information if requested */
	if(dumpInsnProfile)
	{
		if(!_ILDumpInsnProfile(stdout))
		{
			fprintf(stderr, "%s: instruction profiles are not available\n",
					progname);
		}
	}
	if(dumpMethodProfile)
	{
		if(!_ILDumpMethodProfile(stdout, process))
		{
			fprintf(stderr, "%s: method profiles are not available\n",
					progname);
		}
	}
	if(dumpVarProfile)
	{
		if(!_ILDumpVarProfile(stdout))
		{
			fprintf(stderr, "%s: variable profiles are not available\n",
					progname);
		}
	}
	if(dumpParams)
	{
		long mallocMax;
		printf("GC Heap Size      = %ld\n",
			   ILExecProcessGetParam(process, IL_EXEC_PARAM_GC_SIZE));
		printf("Method Cache Size = %ld\n",
			   ILExecProcessGetParam(process, IL_EXEC_PARAM_MC_SIZE));
		mallocMax = ILExecProcessGetParam(process, IL_EXEC_PARAM_MALLOC_MAX);
		if(mallocMax != -1)
		{
			printf("Max Malloc Usage  = %ld\n", mallocMax);
		}
	}
#endif

	/* Clean up the process and exit */
	error = ILExecProcessGetStatus(process);
	ILExecProcessDestroy(process);
	return (int)retval;
}

static void usage(const char *progname)
{
	fprintf(stdout, "ILRUN " VERSION " - IL Program Runtime\n");
	fprintf(stdout, "Copyright (c) 2001 Southern Storm Software, Pty Ltd.\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Usage: %s [options] program [args]\n", progname);
	fprintf(stdout, "\n");
	ILCmdLineHelp(options);
}

static void version(void)
{
	printf("ILRUN " VERSION " - IL Program Runtime\n");
	printf("Copyright (c) 2001 Southern Storm Software, Pty Ltd.\n");
	printf("\n");
	printf("ILRUN comes with ABSOLUTELY NO WARRANTY.  This is free software,\n");
	printf("and you are welcome to redistribute it under the terms of the\n");
	printf("GNU General Public License.  See the file COPYING for further details.\n");
	printf("\n");
	printf("Use the `--help' option to get help on the command-line options.\n");
}

#ifdef	__cplusplus
};
#endif
