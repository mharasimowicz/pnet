/*
 * il_engine.h - Interface to the runtime engine.
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

#ifndef	_IL_ENGINE_H
#define	_IL_ENGINE_H

#include "il_program.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Execution control context for a process.
 */
typedef struct _tagILExecProcess ILExecProcess;

/*
 * Execution control context for a single thread.
 */
typedef struct _tagILExecThread ILExecThread;

/*
 * Object and string handles.
 */
typedef struct _tagILObject ILObject;
typedef struct _tagILString ILString;

/*
 * Types that are useful for passing values through va lists.
 */
#if SIZEOF_INT > 4
typedef int				ILVaInt;
typedef unsigned int	ILVaUInt;
#else
typedef ILInt32			ILVaInt;
typedef ILUInt32		ILVaUInt;
#endif
typedef double			ILVaDouble;

/*
 * The structure that is used to pass typed references around.
 */
typedef struct
{
	void *type;
	void *value;

} ILTypedRef;

/*
 * A structure that can be used to represent any execution engine value.
 * This is typically used when it is inconvenient to use vararg calls.
 */
typedef union
{
	ILInt32			int32Value;
	ILUInt32		uint32Value;
	ILInt64			int64Value;
	ILUInt64		uint64Value;
	ILNativeInt		nintValue;
	ILNativeUInt	nuintValue;
	ILNativeFloat	floatValue;
	ILTypedRef		typedRefValue;
	void		   *ptrValue;
	ILObject	   *objValue;
	ILString	   *strValue;

} ILExecValue;

/*
 * Initialize the engine and set a default maximum heap size.
 * If the size is zero, then use all of memory for the heap.
 * This should be called only once per application.
 */
void ILExecInit(unsigned long maxSize);

/*
 * Create a new process, including the "main" thread.
 */
ILExecProcess *ILExecProcessCreate(void);

/*
 * Destroy a process and all threads associated with it.
 */
void ILExecProcessDestroy(ILExecProcess *process);

/*
 * Set the list of directories to be used for library path
 * searching, before inspecting the standard directories.
 * It is assumed that the list will persist for the lifetime
 * of the process.
 */
void ILExecProcessSetLibraryDirs(ILExecProcess *process,
								 char **libraryDirs,
								 int numLibraryDirs);

/*
 * Get the "main" thread for a process.
 */
ILExecThread *ILExecProcessGetMain(ILExecProcess *process);

/*
 * Load an image into a process.  Returns 0 if OK, or
 * an image load error code otherwise (see "il_image.h").
 */
int ILExecProcessLoadImage(ILExecProcess *process, FILE *file);

/*
 * Load the contents of a file as an image into a process.
 * Returns 0 if OK, -1 if the file could not be opened,
 * or an image load error code otherwise.
 */
int ILExecProcessLoadFile(ILExecProcess *process, const char *filename);

/*
 * Get the exit status for a process.
 */
int ILExecProcessGetStatus(ILExecProcess *process);

/*
 * Get the entry point method for a process.
 */
ILMethod *ILExecProcessGetEntry(ILExecProcess *process);

/*
 * Create a new thread and attach it to a process.
 * Returns NULL if out of memory.
 */
ILExecThread *ILExecThreadCreate(ILExecProcess *process);

/*
 * Destroy a thread and detach it from its process.
 */
void ILExecThreadDestroy(ILExecThread *thread);

/*
 * Get the process that corresponds to a thread.
 */
ILExecProcess *ILExecThreadGetProcess(ILExecThread *thread);

/*
 * Get the method that is executing "num" steps up
 * the thread stack.  Zero indicates the currently
 * executing method.  Returns NULL if "num" is invalid.
 */
ILMethod *ILExecThreadStackMethod(ILExecThread *thread, unsigned long num);

/*
 * Determine the size of a type's values in bytes.
 */
ILUInt32 ILSizeOfType(ILType *type);

/*
 * Look up a class name within a particular thread's context.
 * Returns NULL if the name could not be found.
 */
ILClass *ILExecThreadLookupClass(ILExecThread *thread, const char *className);

/*
 * Look up a type name within a particular thread's context.
 * Returns NULL if the name could not be found.
 */
ILType *ILExecThreadLookupType(ILExecThread *thread, const char *typeName);

/*
 * Look up a method by type name, method name, and signature.
 * Returns NULL if the method could not be found.  This function
 * will search ancestor classes if the method is not found in
 * a child class.
 */
ILMethod *ILExecThreadLookupMethod(ILExecThread *thread,
								   const char *typeName,
								   const char *methodName,
								   const char *signature);

/*
 * Look up a field by type name, field name, and signature.
 * Returns NULL if the field could not be found.  This function
 * will search ancestor classes if the field is not found in
 * a child class.
 */
ILField *ILExecThreadLookupField(ILExecThread *thread,
								 const char *typeName,
								 const char *fieldName,
								 const char *signature);

/*
 * Call a particular method within a thread's context.
 * Returns non-zero if an exception was thrown during
 * the processing of the method.  Zero otherwise.
 * The return value, if any, is placed in "*result".
 */
int ILExecThreadCall(ILExecThread *thread, ILMethod *method,
					 void *result, ...);

/*
 * Call a particular method within a thread's context,
 * passing the parameter and return values from
 * "ILExecValue" structures.  Returns non-zero if an
 * exception was thrown.  Zero otherwise.  The return
 * value, if any, is placed in "*result".
 */
int ILExecThreadCallV(ILExecThread *thread, ILMethod *method,
					  ILExecValue *result, ILExecValue *args);

/*
 * Call a constructor within a thread's context, passing
 * the parameter and return values from "ILExecValue"
 * structures.  Returns the new object, or zero if an
 * exception was thrown.
 */
ILObject *ILExecThreadCallCtorV(ILExecThread *thread, ILMethod *ctor,
								ILExecValue *args);

/*
 * Call a particular virtual method within a thread's context.
 * If the method isn't marked as virtual, then an ordinary
 * method call will be used instead.
 */
int ILExecThreadCallVirtual(ILExecThread *thread, ILMethod *method,
							void *result, void *_this, ...);

/*
 * Call a particular virtual method using "ILExecValue" parameters.
 */
int ILExecThreadCallVirtualV(ILExecThread *thread, ILMethod *method,
							 ILExecValue *result, void *_this,
							 ILExecValue *args);

/*
 * Look up a method by name and then call it.  If the
 * method is not found, an exception will be thrown.
 */
int ILExecThreadCallNamed(ILExecThread *thread, const char *typeName,
						  const char *methodName, const char *signature,
						  void *result, ...);

/*
 * Look up a method by name and call it with "ILExecValue" parameters.
 */
int ILExecThreadCallNamedV(ILExecThread *thread, const char *typeName,
						   const char *methodName, const char *signature,
						   ILExecValue *result, ILExecValue *args);

/*
 * Look up a method by name and then call it using a
 * virtual call.  If the method is not found, an exception
 * will be thrown.
 */
int ILExecThreadCallNamedVirtual(ILExecThread *thread, const char *typeName,
						         const char *methodName, const char *signature,
						         void *result, void *_this, ...);

/*
 * Look up a virtual method by name and call it with
 * "ILExecValue" parameters.
 */
int ILExecThreadCallNamedVirtualV(ILExecThread *thread, const char *typeName,
						          const char *methodName, const char *signature,
						          ILExecValue *result, void *_this,
								  ILExecValue *args);

/*
 * Create a new object instance of a class and call its constructor.
 * Returns NULL if an exception occurred.
 */
ILObject *ILExecThreadNew(ILExecThread *thread, const char *typeName,
						  const char *signature, ...);

/*
 * Create a new object instance of a class and call its constructor
 * using "ILExecValue" parameters.
 */
ILObject *ILExecThreadNewV(ILExecThread *thread, const char *typeName,
						   const char *signature, ILExecValue *args);

/*
 * Determine if there is a last-occuring exception
 * for a thread.  Returns non-zero if so.
 */
int ILExecThreadHasException(ILExecThread *thread);

/*
 * Get the last-occurring exception for a thread.
 * Returns NULL if there is no exception object.
 */
ILObject *ILExecThreadGetException(ILExecThread *thread);

/*
 * Set the last-occuring exception for a thread.
 */
void ILExecThreadSetException(ILExecThread *thread, ILObject *obj);

/*
 * Clear the last-occuring exception for a thread.  If an
 * exception is not cleared, it will be re-thrown from the
 * current method.
 */
void ILExecThreadClearException(ILExecThread *thread);

/*
 * Throw a system exception with a particular type and
 * resource name.
 */
void ILExecThreadThrowSystem(ILExecThread *thread, const char *typeName,
							 const char *resourceName);

/*
 * Throw an "ArgumentOutOfRangeException" for a particular
 * parameter and resource name.
 */
void ILExecThreadThrowArgRange(ILExecThread *thread, const char *paramName,
							   const char *resourceName);

/*
 * Throw an "ArgumentNullException" for a particular
 * parameter name.
 */
void ILExecThreadThrowArgNull(ILExecThread *thread, const char *paramName);

/*
 * Throw an "OutOfMemoryException".
 */
void ILExecThreadThrowOutOfMemory(ILExecThread *thread);

/*
 * Create a string from a NUL-terminated C string.  Returns
 * NULL if out of memory, with the thread's current exception
 * set to an "OutOfMemoryException" instance.
 */
ILString *ILStringCreate(ILExecThread *thread, const char *str);

/*
 * Create a string from a length-delimited C string.
 */
ILString *ILStringCreateLen(ILExecThread *thread, const char *str, int len);

/*
 * Create a string from a zero-terminated wide character string.
 */
ILString *ILStringWCreate(ILExecThread *thread, const ILUInt16 *str);

/*
 * Create a string from a length-delimited wide character string.
 */
ILString *ILStringWCreateLen(ILExecThread *thread,
							 const ILUInt16 *str, int len);

/*
 * Compare two strings, returning -1, 0, or 1, depending
 * upon the relationship between the values.
 */
int ILStringCompare(ILExecThread *thread, ILString *strA, ILString *strB);

/*
 * Compare two strings, while ignoring case.
 */
int ILStringCompareIgnoreCase(ILExecThread *thread, ILString *strA,
							  ILString *strB);

/*
 * Compare the ordinal values of two strings.
 */
int ILStringCompareOrdinal(ILExecThread *thread, ILString *strA,
						   ILString *strB);

/*
 * Determine if two strings are equal.
 */
int ILStringEquals(ILExecThread *thread, ILString *strA, ILString *strB);

/*
 * Concatenate two strings.
 */
ILString *ILStringConcat(ILExecThread *thread, ILString *strA, ILString *strB);

/*
 * Concatenate three strings.
 */
ILString *ILStringConcat3(ILExecThread *thread, ILString *strA,
						  ILString *strB, ILString *strC);

/*
 * Concatenate four strings.
 */
ILString *ILStringConcat4(ILExecThread *thread, ILString *strA,
						  ILString *strB, ILString *strC, ILString *strD);

/*
 * Convert an object into a string, using "ToString".
 * NULL is returned if "object" is NULL.
 */
ILString *ILObjectToString(ILExecThread *thread, ILObject *object);

/*
 * Internalize a string.
 */
ILString *ILStringIntern(ILExecThread *thread, ILString *str);

/*
 * Convert a string into a NUL-terminated UTF-8 buffer,
 * allocated within the garbage collected heap.  If "str"
 * is NULL, then NULL will be returned.
 */
char *ILStringToUTF8(ILExecThread *thread, ILString *str);

/*
 * Convert a string into an "ANSI" string in the local
 * character set, allocated within the garbage collected heap.
 * If "str" is NULL, then NULL will be returned.
 */
char *ILStringToAnsi(ILExecThread *thread, ILString *str);

/*
 * Get an element of an array.  Returns non-zero if an exception occurred.
 */
int ILExecThreadGetElem(ILExecThread *thread, void *value,
						ILObject *array, ILInt32 index);

/*
 * Set an element of an array.  Returns non-zero if an exception occurred.
 */
int ILExecThreadSetElem(ILExecThread *thread, ILObject *array,
						ILInt32 index, ...);

/*
 * Box the value at a pointer using a specific primitive or value type.
 */
ILObject *ILExecThreadBox(ILExecThread *thread, ILType *type, void *ptr);

/*
 * Unbox an object using a specific primitive or value type and
 * write the contents to a pointer.  Returns zero if the object
 * is NULL or is not of the correct type.
 */
int ILExecThreadUnbox(ILExecThread *thread, ILType *type,
					  ILObject *object, void *ptr);

#ifdef	__cplusplus
};
#endif

#endif	/* _IL_ENGINE_H */
