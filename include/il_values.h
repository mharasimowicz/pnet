/*
 * il_values.h - Primitive value types for the IL interpreter.
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

#ifndef	_IL_VALUES_H
#define	_IL_VALUES_H

#if defined(WIN32) && !defined(__CYGWIN__)
/* Probably compiling with Visual Studio, so use
   an alternative set of configuration settings */
#include "il_config_win.h"
#define	IL_NATIVE_WIN32
#else
#include "il_config.h"
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Define the 8-bit and 16-bit numeric types.
 */
typedef char						ILInt8;
typedef unsigned char				ILUInt8;
typedef short						ILInt16;
typedef unsigned short				ILUInt16;
typedef ILInt8 						ILBool;

/*
 * Determine which types should be used for 32-bit numeric values.
 */
#ifdef SIZEOF_INT
	#if SIZEOF_INT == 4
		typedef int					ILInt32;
		typedef unsigned int		ILUInt32;
		#define	IL_HAVE_INT32
	#endif
#endif
#ifndef	IL_HAVE_INT32
	typedef long					ILInt32;
	typedef unsigned long			ILUInt32;
#endif

/*
 * Determine which types should be used for 64-bit numeric values.
 */
#ifdef SIZEOF_LONG_LONG
	#if SIZEOF_LONG_LONG == 8
		typedef long long			ILInt64;
		typedef unsigned long long	ILUInt64;
		#define	IL_HAVE_INT64
	#endif
#endif
#if (!defined(IL_HAVE_INT64)) && defined(SIZEOF_LONG)
	#if SIZEOF_LONG == 8
		typedef long				ILInt64;
		typedef unsigned long		ILUInt64;
		#define	IL_HAVE_INT64
	#endif
#endif
#if (!defined(IL_HAVE_INT64)) && defined(IL_NATIVE_WIN32)
	typedef __int64					ILInt64;
	typedef unsigned __int64		ILUInt64;
	#define IL_HAVE_INT64
#endif
#ifndef IL_HAVE_INT64
	#error "Could not detect the 64-bit integer type on this compiler"
#endif

/*
 * Determine the best native integer format.
 */
#ifdef SIZEOF_LONG
	#if SIZEOF_LONG == 8
		typedef ILInt64				ILNativeInt;
		typedef ILUInt64			ILNativeUInt;
		#define	IL_NATIVE_INT64
	#endif
#endif
#ifndef	IL_NATIVE_INT64
	typedef ILInt32					ILNativeInt;
	typedef ILUInt32				ILNativeUInt;
	#define	IL_NATIVE_INT32
#endif

/*
 * Determine which type should be used for 32-bit floating point values.
 */
#ifdef SIZEOF_FLOAT
	#if SIZEOF_FLOAT == 4
		typedef float				ILFloat;
		#define	IL_HAVE_FLOAT
	#else
		#error "Compiler's float is not 32 bits!"
	#endif
#endif
#ifndef	IL_HAVE_FLOAT
	typedef	float					ILFloat;
#endif

/*
 * Determine which type should be used for 64-bit floating point values.
 */
#ifdef SIZEOF_DOUBLE
	#if SIZEOF_DOUBLE == 8
		typedef double				ILDouble;
		#define	IL_HAVE_DOUBLE
	#else
		#error "Compiler's double is not 64 bits!"
	#endif
#endif
#ifndef	IL_HAVE_DOUBLE
	typedef	double					ILDouble;
#endif

/*
 * Determine the best native floating point format.
 *
 * Note: we check that sizeof(long double) is different from
 * sizeof(double) because some versions of gcc complain about
 * "long double" if the underlying CPU doesn't really have it.
 */
#ifdef SIZEOF_LONG_DOUBLE
	#if SIZEOF_LONG_DOUBLE != 0
		#if SIZEOF_LONG_DOUBLE != SIZEOF_DOUBLE
			typedef long double			ILNativeFloat;
			#define	IL_NATIVE_FLOAT
		#endif
	#endif
#endif
#ifndef	IL_NATIVE_FLOAT
	typedef ILDouble				ILNativeFloat;
#endif

/*
 * Useful min/max constants for the above types.
 */
#define	IL_MIN_INT32				((ILInt32)0x80000000L)
#define	IL_MAX_INT32				((ILInt32)0x7FFFFFFFL)
#define	IL_MAX_UINT32				((ILUInt32)0xFFFFFFFFL)
#ifndef IL_NATIVE_WIN32
#define	IL_MIN_INT64				((ILInt64)0x8000000000000000LL)
#define	IL_MAX_INT64				((ILInt64)0x7FFFFFFFFFFFFFFFLL)
#define	IL_MAX_UINT64				((ILUInt64)0xFFFFFFFFFFFFFFFFLL)
#else
#define	IL_MIN_INT64				((ILInt64)0x8000000000000000L)
#define	IL_MAX_INT64				((ILInt64)0x7FFFFFFFFFFFFFFFL)
#define	IL_MAX_UINT64				((ILUInt64)0xFFFFFFFFFFFFFFFFUL)
#endif
#ifdef IL_NATIVE_INT32
#define	IL_MIN_NATIVE_INT			IL_MIN_INT32
#define	IL_MAX_NATIVE_INT			IL_MAX_INT32
#define	IL_MAX_NATIVE_UINT			IL_MAX_UINT32
#else
#define	IL_MIN_NATIVE_INT			IL_MIN_INT64
#define	IL_MAX_NATIVE_INT			IL_MAX_INT64
#define	IL_MAX_NATIVE_UINT			IL_MAX_UINT64
#endif

/*
 * Read little-endian values of various sizes from memory buffers.
 */
#define	_IL_READ_BYTE(buf,offset)	\
			((ILUInt32)(ILUInt8)((buf)[(offset)]))
#define	_IL_READ_BYTE_SHIFT(buf,offset,shift)	\
			(((ILUInt32)(ILUInt8)((buf)[(offset)])) << (shift))
#if defined(__i386) || defined(__i386__)
/* The i386 family of CPU's can read little-endian values on any
   byte boundary, so we can implement the macros more simply */
#define	IL_READ_INT16(buf)	(*((ILInt16 *)(buf)))
#define	IL_READ_UINT16(buf)	(*((ILUInt16 *)(buf)))
#define	IL_READ_INT32(buf)	(*((ILInt32 *)(buf)))
#define	IL_READ_UINT32(buf)	(*((ILUInt32 *)(buf)))
#define	IL_READ_INT64(buf)	(*((ILInt64 *)(buf)))
#define	IL_READ_UINT64(buf)	(*((ILUInt64 *)(buf)))
#else
#define	IL_READ_INT16(buf)	((ILInt16)(_IL_READ_BYTE((buf), 0) | \
									   _IL_READ_BYTE_SHIFT((buf), 1, 8)))
#define	IL_READ_UINT16(buf)	((ILUInt16)(_IL_READ_BYTE((buf), 0) | \
									    _IL_READ_BYTE_SHIFT((buf), 1, 8)))
#define	IL_READ_INT32(buf)	((ILInt32)(_IL_READ_BYTE((buf), 0) | \
									   _IL_READ_BYTE_SHIFT((buf), 1, 8) | \
									   _IL_READ_BYTE_SHIFT((buf), 2, 16) | \
									   _IL_READ_BYTE_SHIFT((buf), 3, 24)))
#define	IL_READ_UINT32(buf)	((ILUInt32)(_IL_READ_BYTE((buf), 0) | \
									    _IL_READ_BYTE_SHIFT((buf), 1, 8) | \
									    _IL_READ_BYTE_SHIFT((buf), 2, 16) | \
									    _IL_READ_BYTE_SHIFT((buf), 3, 24)))
#define	IL_READ_INT64(buf)	\
			(((ILInt64)(IL_READ_UINT32((buf)))) | \
			 (((ILInt64)(IL_READ_INT32((buf) + 4))) << 32))
#define	IL_READ_UINT64(buf)	\
			(((ILUInt64)(IL_READ_UINT32((buf)))) | \
			 (((ILUInt64)(IL_READ_UINT32((buf) + 4))) << 32))
#endif
ILNativeFloat _ILReadFloat(const unsigned char *buf);
#define	IL_READ_FLOAT(buf)	(_ILReadFloat((buf)))
ILNativeFloat _ILReadDouble(const unsigned char *buf);
#define	IL_READ_DOUBLE(buf)	(_ILReadDouble((buf)))

/*
 * Write little-endian values of various sizes to memory buffers.
 */
void _ILWriteFloat(unsigned char *buf, ILFloat value);
#define	IL_WRITE_FLOAT(buf,value)	(_ILWriteFloat((buf), (value)))
void _ILWriteDouble(unsigned char *buf, ILDouble value);
#define	IL_WRITE_DOUBLE(buf,value)	(_ILWriteDouble((buf), (value)))

#ifdef	__cplusplus
};
#endif

#endif	/* _IL_VALUES_H */
