/*
 * lib_serial.c - Internalcall methods for the "Platform.PortMethods" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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
 * Serial port parameters.  Must match "PortMethods.Parameters".
 */
typedef struct
{
	ILInt32 baudRate;
	ILInt32 parity;
	ILInt32 dataBits;
	ILInt32 stopBits;
	ILInt32 handshake;
	ILUInt8 parityReplace;
	ILBool discardNull;
	ILInt32 readBufferSize;
	ILInt32 writeBufferSize;
	ILInt32 receivedBytesThreshold;
	ILInt32 readTimeout;
	ILInt32 writeTimeout;

} SerialParameters;

/*
 * Serial port types.
 */
#define	IL_SERIAL_REGULAR		0
#define	IL_SERIAL_INFRARED		1

/*
 * Bits for various serial pins.
 */
#define IL_PIN_BREAK			(1<<0)
#define IL_PIN_CD				(1<<1)
#define IL_PIN_CTS				(1<<2)
#define IL_PIN_DSR				(1<<3)
#define IL_PIN_DTR				(1<<4)
#define IL_PIN_RTS				(1<<5)

/*
 * public static bool IsValid(int type, int portNumber);
 */
ILBool _IL_PortMethods_IsValid
			(ILExecThread *_thread, ILInt32 type, ILInt32 portNumber)
{
	/* TODO */
	return 0;
}

/*
 * public static bool IsAccessible(int type, int portNumber);
 */
ILBool _IL_PortMethods_IsAccessible
			(ILExecThread *_thread, ILInt32 type, ILInt32 portNumber)
{
	/* TODO */
	return 0;
}

/*
 * public static IntPtr Open(int type, int portNumber,
 *							 Parameters parameters);
 */
ILNativeInt _IL_PortMethods_Open(ILExecThread *_thread, ILInt32 type,
								 ILInt32 portNumber, ILObject *parameters)
{
	/* TODO */
	return 0;
}

/*
 * public static void Close(IntPtr handle);
 */
void _IL_PortMethods_Close(ILExecThread *_thread, ILNativeInt handle)
{
	/* TODO */
}

/*
 * public static void Modify(IntPtr handle, Parameters parameters);
 */
void _IL_PortMethods_Modify(ILExecThread *_thread, ILNativeInt handle,
						    ILObject *parameters)
{
	/* TODO */
}

/*
 * public static int GetBytesToRead(IntPtr handle);
 */
ILInt32 _IL_PortMethods_GetBytesToRead(ILExecThread *_thread,
									   ILNativeInt handle)
{
	/* TODO */
	return 0;
}

/*
 * public static int GetBytesToWrite(IntPtr handle);
 */
ILInt32 _IL_PortMethods_GetBytesToWrite(ILExecThread *_thread,
										ILNativeInt handle)
{
	/* TODO */
	return 0;
}

/*
 * public static int ReadPins(IntPtr handle);
 */
ILInt32 _IL_PortMethods_ReadPins(ILExecThread *_thread, ILNativeInt handle)
{
	/* TODO */
	return 0;
}

/*
 * public static void WritePins(IntPtr handle, int mask, int value);
 */
void _IL_PortMethods_WritePins(ILExecThread *_thread,
							   ILNativeInt handle,
							   ILInt32 mask, ILInt32 value)
{
	/* TODO */
}

/*
 * public static void GetRecommendedBufferSizes
 *			(out int readBufferSize, out int writeBufferSize,
 *			 out int receivedBytesThreshold);
 */
void _IL_PortMethods_GetRecommendedBufferSizes
			(ILExecThread *_thread, ILInt32 *readBufferSize,
			 ILInt32 *writeBufferSize, ILInt32 *receivedBytesThreshold)
{
	/* TODO */
	*readBufferSize = 1024;
	*writeBufferSize = 1024;
	*receivedBytesThreshold = 768;
}

/*
 * public static void DiscardInBuffer(IntPtr handle);
 */
void _IL_PortMethods_DiscardInBuffer(ILExecThread *_thread, ILNativeInt handle)
{
	/* TODO */
}

/*
 * public static void DiscardOutBuffer(IntPtr handle);
 */
void _IL_PortMethods_DiscardOutBuffer(ILExecThread *_thread, ILNativeInt handle)
{
	/* TODO */
}

/*
 * public static void DrainOutBuffer(IntPtr handle);
 */
void _IL_PortMethods_DrainOutBuffer(ILExecThread * _thread, ILNativeInt handle)
{
	/* TODO */
}

/*
 * public static int Read(IntPtr handle, byte[] buffer, int offset, int count);
 */
ILInt32 _IL_PortMethods_Read(ILExecThread *_thread, ILNativeInt handle,
							 System_Array *buffer, ILInt32 offset,
							 ILInt32 count)
{
	/* TODO */
	return 0;
}

/*
 * public static void Write
 *			(IntPtr handle, byte[] buffer, int offset, int count);
 */
void _IL_PortMethods_Write(ILExecThread *_thread, ILNativeInt handle,
						   System_Array *buffer, ILInt32 offset,
						   ILInt32 count)
{
	/* TODO */
}

#ifdef	__cplusplus
};
#endif
