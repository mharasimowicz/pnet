/* This file is automatically generated - do not edit */

extern ILObject * _IL_Object_GetType(ILExecThread * _thread, ILObject * _this);
extern ILInt32 _IL_Object_GetHashCode(ILExecThread * _thread, ILObject * _this);
extern ILBool _IL_Object_Equals(ILExecThread * _thread, ILObject * _this, ILObject * obj);
extern ILObject * _IL_Object_MemberwiseClone(ILExecThread * _thread, ILObject * _this);

extern void _IL_ArgIterator_ctor_RuntimeArgumentHandle(ILExecThread * _thread, void * _this, void * argList);
extern void _IL_ArgIterator_ctor_RuntimeArgumentHandlepV(ILExecThread * _thread, void * _this, void * argList, void * ptr);
extern void _IL_ArgIterator_End(ILExecThread * _thread, void * _this);
extern ILTypedRef _IL_ArgIterator_GetNextArg_(ILExecThread * _thread, void * _this);
extern ILTypedRef _IL_ArgIterator_GetNextArg_RuntimeTypeHandle(ILExecThread * _thread, void * _this, void * type);
extern void _IL_ArgIterator_GetNextArgType(ILExecThread * _thread, void * _result, void * _this);
extern ILInt32 _IL_ArgIterator_GetRemainingCount(ILExecThread * _thread, void * _this);

extern void _IL_Array_Clear(ILExecThread * _thread, ILObject * array, ILInt32 index, ILInt32 length);
extern void _IL_Array_Initialize(ILExecThread * _thread, ILObject * _this);
extern void _IL_Array_InternalCopy(ILExecThread * _thread, ILObject * sourceArray, ILInt32 sourceIndex, ILObject * destinationArray, ILInt32 destinationIndex, ILInt32 length);
extern ILObject * _IL_Array_CreateArray_jiiii(ILExecThread * _thread, ILNativeInt elementType, ILInt32 rank, ILInt32 length1, ILInt32 length2, ILInt32 length3);
extern ILObject * _IL_Array_CreateArray_jaiai(ILExecThread * _thread, ILNativeInt elementType, System_Array * lengths, System_Array * lowerBounds);
extern ILInt32 _IL_Array_GetLength_i(ILExecThread * _thread, ILObject * _this, ILInt32 dimension);
extern ILInt32 _IL_Array_GetLowerBound(ILExecThread * _thread, ILObject * _this, ILInt32 dimension);
extern ILInt32 _IL_Array_GetUpperBound(ILExecThread * _thread, ILObject * _this, ILInt32 dimension);
extern ILObject * _IL_Array_Get_iii(ILExecThread * _thread, ILObject * _this, ILInt32 index1, ILInt32 index2, ILInt32 index3);
extern ILObject * _IL_Array_Get_ai(ILExecThread * _thread, ILObject * _this, System_Array * indices);
extern ILObject * _IL_Array_GetRelative(ILExecThread * _thread, ILObject * _this, ILInt32 index);
extern void _IL_Array_Set_Objectiii(ILExecThread * _thread, ILObject * _this, ILObject * value, ILInt32 index1, ILInt32 index2, ILInt32 index3);
extern void _IL_Array_Set_Objectai(ILExecThread * _thread, ILObject * _this, ILObject * value, System_Array * indices);
extern void _IL_Array_SetRelative(ILExecThread * _thread, ILObject * _this, ILObject * value, ILInt32 index);
extern ILInt32 _IL_Array_GetLength_(ILExecThread * _thread, ILObject * _this);
extern ILInt32 _IL_Array_GetRank(ILExecThread * _thread, ILObject * _this);

extern ILObject * _IL_Delegate_CreateBlankDelegate(ILExecThread * _thread, ILObject * type, ILObject * method);

extern ILObject * _IL_Enum_GetEnumValue(ILExecThread * _thread, ILObject * _this);
extern ILString * _IL_Enum_GetEnumName(ILExecThread * _thread, ILObject * enumType, ILObject * value);
extern ILBool _IL_Enum_IsEnumValue(ILExecThread * _thread, ILObject * enumType, ILObject * value);
extern ILObject * _IL_Enum_GetEnumValueFromName(ILExecThread * _thread, ILObject * enumType, ILString * name, ILBool ignoreCase);
extern ILObject * _IL_Enum_EnumValueOr(ILExecThread * _thread, ILObject * value1, ILObject * value2);
extern ILObject * _IL_Enum_EnumIntToObject(ILExecThread * _thread, ILObject * enumType, ILInt32 value);
extern ILObject * _IL_Enum_EnumLongToObject(ILExecThread * _thread, ILObject * enumType, ILInt64 value);
extern ILString * _IL_Enum_FormatEnumWithFlags(ILExecThread * _thread, ILObject * enumType, ILObject * value);

extern ILBool _IL_BitConverter_GetLittleEndian(ILExecThread * _thread);
extern ILInt64 _IL_BitConverter_DoubleToInt64Bits(ILExecThread * _thread, ILDouble value);
extern ILDouble _IL_BitConverter_Int64BitsToDouble(ILExecThread * _thread, ILInt64 value);
extern ILInt32 _IL_BitConverter_FloatToInt32Bits(ILExecThread * _thread, ILFloat value);
extern ILFloat _IL_BitConverter_Int32BitsToFloat(ILExecThread * _thread, ILInt32 value);

extern void _IL_Buffer_Copy(ILExecThread * _thread, ILObject * src, ILInt32 srcOffset, ILObject * dst, ILInt32 dstOffset, ILInt32 count);
extern ILInt32 _IL_Buffer_GetLength(ILExecThread * _thread, ILObject * array);
extern ILUInt8 _IL_Buffer_GetElement(ILExecThread * _thread, ILObject * array, ILInt32 index);
extern void _IL_Buffer_SetElement(ILExecThread * _thread, ILObject * array, ILInt32 index, ILUInt8 value);

extern void _IL_Decimal_Add(ILExecThread * _thread, ILDecimal * _result, ILDecimal * x, ILDecimal * y);
extern ILInt32 _IL_Decimal_Compare(ILExecThread * _thread, ILDecimal * x, ILDecimal * y);
extern void _IL_Decimal_Divide(ILExecThread * _thread, ILDecimal * _result, ILDecimal * x, ILDecimal * y);
extern void _IL_Decimal_Floor(ILExecThread * _thread, ILDecimal * _result, ILDecimal * x);
extern void _IL_Decimal_Remainder(ILExecThread * _thread, ILDecimal * _result, ILDecimal * x, ILDecimal * y);
extern void _IL_Decimal_Multiply(ILExecThread * _thread, ILDecimal * _result, ILDecimal * x, ILDecimal * y);
extern void _IL_Decimal_Negate(ILExecThread * _thread, ILDecimal * _result, ILDecimal * x);
extern void _IL_Decimal_Round(ILExecThread * _thread, ILDecimal * _result, ILDecimal * x, ILInt32 decimals);
extern void _IL_Decimal_Subtract(ILExecThread * _thread, ILDecimal * _result, ILDecimal * x, ILDecimal * y);
extern void _IL_Decimal_Truncate(ILExecThread * _thread, ILDecimal * _result, ILDecimal * x);
extern void _IL_Decimal_ctor_f(ILExecThread * _thread, ILDecimal * _this, ILFloat value);
extern void _IL_Decimal_ctor_d(ILExecThread * _thread, ILDecimal * _this, ILDouble value);
extern ILFloat _IL_Decimal_ToSingle(ILExecThread * _thread, ILDecimal * value);
extern ILDouble _IL_Decimal_ToDouble(ILExecThread * _thread, ILDecimal * value);

extern ILBool _IL_Double_IsNaN(ILExecThread * _thread, ILDouble d);
extern ILInt32 _IL_Double_TestInfinity(ILExecThread * _thread, ILDouble d);

extern void _IL_GC_KeepAlive(ILExecThread * _thread, ILObject * obj);
extern void _IL_GC_ReRegisterForFinalize(ILExecThread * _thread, ILObject * obj);
extern void _IL_GC_SuppressFinalize(ILExecThread * _thread, ILObject * obj);
extern void _IL_GC_WaitForPendingFinalizers(ILExecThread * _thread);
extern void _IL_GC_Collect(ILExecThread * _thread);
extern ILInt64 _IL_GC_GetTotalMemory(ILExecThread * _thread, ILBool forceFullCollection);

extern void _IL_Guid_NewGuid(ILExecThread * _thread, void * _result);

extern ILDouble _IL_Math_Acos(ILExecThread * _thread, ILDouble d);
extern ILDouble _IL_Math_Asin(ILExecThread * _thread, ILDouble d);
extern ILDouble _IL_Math_Atan(ILExecThread * _thread, ILDouble d);
extern ILDouble _IL_Math_Atan2(ILExecThread * _thread, ILDouble y, ILDouble x);
extern ILDouble _IL_Math_Ceiling(ILExecThread * _thread, ILDouble a);
extern ILDouble _IL_Math_Cos(ILExecThread * _thread, ILDouble d);
extern ILDouble _IL_Math_Cosh(ILExecThread * _thread, ILDouble value);
extern ILDouble _IL_Math_Exp(ILExecThread * _thread, ILDouble d);
extern ILDouble _IL_Math_Floor(ILExecThread * _thread, ILDouble d);
extern ILDouble _IL_Math_IEEERemainder(ILExecThread * _thread, ILDouble x, ILDouble y);
extern ILDouble _IL_Math_Log(ILExecThread * _thread, ILDouble d);
extern ILDouble _IL_Math_Log10(ILExecThread * _thread, ILDouble d);
extern ILDouble _IL_Math_Pow(ILExecThread * _thread, ILDouble x, ILDouble y);
extern ILDouble _IL_Math_Round(ILExecThread * _thread, ILDouble a);
extern ILDouble _IL_Math_RoundDouble(ILExecThread * _thread, ILDouble value, ILInt32 digits);
extern ILDouble _IL_Math_Sin(ILExecThread * _thread, ILDouble a);
extern ILDouble _IL_Math_Sinh(ILExecThread * _thread, ILDouble a);
extern ILDouble _IL_Math_Sqrt(ILExecThread * _thread, ILDouble a);
extern ILDouble _IL_Math_Tan(ILExecThread * _thread, ILDouble a);
extern ILDouble _IL_Math_Tanh(ILExecThread * _thread, ILDouble value);

extern ILNativeInt _IL_RuntimeMethodHandle_GetFunctionPointer(ILExecThread * _thread, void * _this);

extern ILBool _IL_Single_IsNaN(ILExecThread * _thread, ILFloat f);
extern ILInt32 _IL_Single_TestInfinity(ILExecThread * _thread, ILFloat f);

extern System_String * _IL_String_ctor_acii(ILExecThread * _thread, System_Array * value, ILInt32 startIndex, ILInt32 length);
extern System_String * _IL_String_ctor_ac(ILExecThread * _thread, System_Array * value);
extern System_String * _IL_String_ctor_ci(ILExecThread * _thread, ILUInt16 c, ILInt32 count);
extern System_String * _IL_String_ctor_pcii(ILExecThread * _thread, ILUInt16 * value, ILInt32 startIndex, ILInt32 length);
extern System_String * _IL_String_ctor_pc(ILExecThread * _thread, ILUInt16 * value);
extern System_String * _IL_String_ctor_pbiiEncoding(ILExecThread * _thread, ILInt8 * value, ILInt32 startIndex, ILInt32 length, ILObject * enc);
extern System_String * _IL_String_ctor_pbii(ILExecThread * _thread, ILInt8 * value, ILInt32 startIndex, ILInt32 length);
extern System_String * _IL_String_ctor_pb(ILExecThread * _thread, ILInt8 * value);
extern ILInt32 _IL_String_Compare(ILExecThread * _thread, System_String * strA, System_String * strB);
extern ILInt32 _IL_String_InternalCompare(ILExecThread * _thread, System_String * strA, ILInt32 indexA, ILInt32 lengthA, System_String * strB, ILInt32 indexB, ILInt32 lengthB, ILBool ignoreCase, ILObject * culture);
extern ILInt32 _IL_String_InternalOrdinal(ILExecThread * _thread, System_String * strA, ILInt32 indexA, ILInt32 lengthA, System_String * strB, ILInt32 indexB, ILInt32 lengthB);
extern System_String * _IL_String_NewString(ILExecThread * _thread, ILInt32 length);
extern System_String * _IL_String_NewBuilder(ILExecThread * _thread, System_String * value, ILInt32 length);
extern void _IL_String_Copy_StringiString(ILExecThread * _thread, System_String * dest, ILInt32 destPos, System_String * src);
extern void _IL_String_Copy_StringiStringii(ILExecThread * _thread, System_String * dest, ILInt32 destPos, System_String * src, ILInt32 srcPos, ILInt32 length);
extern void _IL_String_InsertSpace(ILExecThread * _thread, System_String * str, ILInt32 srcPos, ILInt32 destPos);
extern void _IL_String_RemoveSpace(ILExecThread * _thread, System_String * str, ILInt32 index, ILInt32 length);
extern System_String * _IL_String_Concat_StringString(ILExecThread * _thread, System_String * str1, System_String * str2);
extern System_String * _IL_String_Concat_StringStringString(ILExecThread * _thread, System_String * str1, System_String * str2, System_String * str3);
extern void _IL_String_CopyToChecked(ILExecThread * _thread, System_String * _this, ILInt32 sourceIndex, System_Array * destination, ILInt32 destinationIndex, ILInt32 count);
extern ILBool _IL_String_Equals(ILExecThread * _thread, System_String * a, System_String * b);
extern ILInt32 _IL_String_GetHashCode(ILExecThread * _thread, System_String * _this);
extern ILInt32 _IL_String_IndexOf(ILExecThread * _thread, System_String * _this, ILUInt16 value, ILInt32 startIndex, ILInt32 count);
extern ILBool _IL_String_EqualRange(ILExecThread * _thread, System_String * _this, ILInt32 srcIndex, ILInt32 count, System_String * dest, ILInt32 destIndex);
extern ILInt32 _IL_String_IndexOfAny(ILExecThread * _thread, System_String * _this, System_Array * anyOf, ILInt32 startIndex, ILInt32 count);
extern System_String * _IL_String_Intern(ILExecThread * _thread, System_String * str);
extern System_String * _IL_String_IsInterned(ILExecThread * _thread, System_String * str);
extern ILInt32 _IL_String_LastIndexOf(ILExecThread * _thread, System_String * _this, ILUInt16 value, ILInt32 startIndex, ILInt32 count);
extern ILInt32 _IL_String_LastIndexOfAny(ILExecThread * _thread, System_String * _this, System_Array * anyOf, ILInt32 startIndex, ILInt32 count);
extern void _IL_String_CharFill_Stringiic(ILExecThread * _thread, System_String * str, ILInt32 start, ILInt32 count, ILUInt16 ch);
extern void _IL_String_CharFill_Stringiacii(ILExecThread * _thread, System_String * str, ILInt32 start, System_Array * chars, ILInt32 index, ILInt32 count);
extern System_String * _IL_String_Replace_cc(ILExecThread * _thread, System_String * _this, ILUInt16 oldChar, ILUInt16 newChar);
extern System_String * _IL_String_Replace_StringString(ILExecThread * _thread, System_String * _this, System_String * oldValue, System_String * newValue);
extern System_String * _IL_String_Trim(ILExecThread * _thread, System_String * _this, System_Array * trimChars, ILInt32 trimFlags);
extern ILUInt16 _IL_String_GetChar(ILExecThread * _thread, System_String * _this, ILInt32 posn);
extern void _IL_String_SetChar(ILExecThread * _thread, System_String * _this, ILInt32 posn, ILUInt16 value);

extern ILObject * _IL_Type_GetType(ILExecThread * _thread, ILString * name, ILBool throwOnError, ILBool ignoreCase);
extern void _IL_Type_GetTypeHandle(ILExecThread * _thread, void * _result, ILObject * obj);
extern ILObject * _IL_Type_GetTypeFromHandle(ILExecThread * _thread, void * handle);

extern ILTypedRef _IL_TypedReference_ClrMakeTypedReference(ILExecThread * _thread, ILObject * target, System_Array * flds);
extern ILBool _IL_TypedReference_ClrSetTypedReference(ILExecThread * _thread, ILTypedRef target, ILObject * value);
extern ILObject * _IL_TypedReference_ToObject(ILExecThread * _thread, ILTypedRef value);

extern ILInt32 _IL_Interlocked_CompareExchange_Riii(ILExecThread * _thread, ILInt32 * location1, ILInt32 value, ILInt32 comparand);
extern ILFloat _IL_Interlocked_CompareExchange_Rfff(ILExecThread * _thread, ILFloat * location1, ILFloat value, ILFloat comparand);
extern ILObject * _IL_Interlocked_CompareExchange_RObjectObjectObject(ILExecThread * _thread, ILObject * * location1, ILObject * value, ILObject * comparand);
extern ILInt32 _IL_Interlocked_Decrement_Ri(ILExecThread * _thread, ILInt32 * location);
extern ILInt64 _IL_Interlocked_Decrement_Rl(ILExecThread * _thread, ILInt64 * location);
extern ILInt32 _IL_Interlocked_Exchange_Rii(ILExecThread * _thread, ILInt32 * location, ILInt32 value);
extern ILFloat _IL_Interlocked_Exchange_Rff(ILExecThread * _thread, ILFloat * location, ILFloat value);
extern ILObject * _IL_Interlocked_Exchange_RObjectObject(ILExecThread * _thread, ILObject * * location, ILObject * value);
extern ILInt32 _IL_Interlocked_Increment_Ri(ILExecThread * _thread, ILInt32 * location);
extern ILInt64 _IL_Interlocked_Increment_Rl(ILExecThread * _thread, ILInt64 * location);

extern void _IL_Monitor_Enter(ILExecThread * _thread, ILObject * obj);
extern ILBool _IL_Monitor_InternalTryEnter(ILExecThread * _thread, ILObject * obj, ILInt32 timeout);
extern void _IL_Monitor_Exit(ILExecThread * _thread, ILObject * obj);
extern ILBool _IL_Monitor_InternalWait(ILExecThread * _thread, ILObject * obj, ILInt32 timeout);
extern void _IL_Monitor_Pulse(ILExecThread * _thread, ILObject * obj);
extern void _IL_Monitor_PulseAll(ILExecThread * _thread, ILObject * obj);

extern void _IL_WaitHandle_InternalClose(ILExecThread * _thread, ILNativeInt privateData);
extern ILBool _IL_WaitHandle_InternalWaitAll(ILExecThread * _thread, System_Array * waitHandles, ILInt32 timeout, ILBool exitContext);
extern ILInt32 _IL_WaitHandle_InternalWaitAny(ILExecThread * _thread, System_Array * waitHandles, ILInt32 timeout, ILBool exitContext);
extern ILBool _IL_WaitHandle_InternalWaitOne(ILExecThread * _thread, ILNativeInt privateData, ILInt32 timeout);

extern ILNativeInt _IL_Mutex_InternalCreateMutex(ILExecThread * _thread, ILBool initiallyOwned, ILString * name, ILBool * gotOwnership);
extern void _IL_Mutex_InternalReleaseMutex(ILExecThread * _thread, ILNativeInt mutex);

extern void _IL_Thread_FinalizeThread(ILExecThread * _thread, ILObject * _this);
extern void _IL_Thread_Abort(ILExecThread * _thread, ILObject * _this);
extern ILBool _IL_Thread_InternalJoin(ILExecThread * _thread, ILObject * _this, ILInt32 timeout);
extern void _IL_Thread_MemoryBarrier(ILExecThread * _thread);
extern void _IL_Thread_ResetAbort(ILExecThread * _thread);
extern void _IL_Thread_InternalSleep(ILExecThread * _thread, ILInt32 timeout);
extern void _IL_Thread_Start(ILExecThread * _thread, ILObject * _this);
extern ILUInt8 _IL_Thread_VolatileRead_RB(ILExecThread * _thread, ILUInt8 * address);
extern ILInt8 _IL_Thread_VolatileRead_Rb(ILExecThread * _thread, ILInt8 * address);
extern ILInt16 _IL_Thread_VolatileRead_Rs(ILExecThread * _thread, ILInt16 * address);
extern ILUInt16 _IL_Thread_VolatileRead_RS(ILExecThread * _thread, ILUInt16 * address);
extern ILInt32 _IL_Thread_VolatileRead_Ri(ILExecThread * _thread, ILInt32 * address);
extern ILUInt32 _IL_Thread_VolatileRead_RI(ILExecThread * _thread, ILUInt32 * address);
extern ILInt64 _IL_Thread_VolatileRead_Rl(ILExecThread * _thread, ILInt64 * address);
extern ILUInt64 _IL_Thread_VolatileRead_RL(ILExecThread * _thread, ILUInt64 * address);
extern ILNativeInt _IL_Thread_VolatileRead_Rj(ILExecThread * _thread, ILNativeInt * address);
extern ILNativeUInt _IL_Thread_VolatileRead_RJ(ILExecThread * _thread, ILNativeUInt * address);
extern ILFloat _IL_Thread_VolatileRead_Rf(ILExecThread * _thread, ILFloat * address);
extern ILDouble _IL_Thread_VolatileRead_Rd(ILExecThread * _thread, ILDouble * address);
extern ILObject * _IL_Thread_VolatileRead_RObject(ILExecThread * _thread, ILObject * * address);
extern void _IL_Thread_VolatileWrite_RBB(ILExecThread * _thread, ILUInt8 * address, ILUInt8 value);
extern void _IL_Thread_VolatileWrite_Rbb(ILExecThread * _thread, ILInt8 * address, ILInt8 value);
extern void _IL_Thread_VolatileWrite_Rss(ILExecThread * _thread, ILInt16 * address, ILInt16 value);
extern void _IL_Thread_VolatileWrite_RSS(ILExecThread * _thread, ILUInt16 * address, ILUInt16 value);
extern void _IL_Thread_VolatileWrite_Rii(ILExecThread * _thread, ILInt32 * address, ILInt32 value);
extern void _IL_Thread_VolatileWrite_RII(ILExecThread * _thread, ILUInt32 * address, ILUInt32 value);
extern void _IL_Thread_VolatileWrite_Rll(ILExecThread * _thread, ILInt64 * address, ILInt64 value);
extern void _IL_Thread_VolatileWrite_RLL(ILExecThread * _thread, ILUInt64 * address, ILUInt64 value);
extern void _IL_Thread_VolatileWrite_Rjj(ILExecThread * _thread, ILNativeInt * address, ILNativeInt value);
extern void _IL_Thread_VolatileWrite_RJJ(ILExecThread * _thread, ILNativeUInt * address, ILNativeUInt value);
extern void _IL_Thread_VolatileWrite_Rff(ILExecThread * _thread, ILFloat * address, ILFloat value);
extern void _IL_Thread_VolatileWrite_Rdd(ILExecThread * _thread, ILDouble * address, ILDouble value);
extern void _IL_Thread_VolatileWrite_RObjectObject(ILExecThread * _thread, ILObject * * address, ILObject * value);
extern ILObject * _IL_Thread_InternalCurrentThread(ILExecThread * _thread);
extern void _IL_Thread_InternalSetBackground(ILExecThread * _thread, ILObject * _this, ILBool value);
extern ILInt32 _IL_Thread_InternalGetPriority(ILExecThread * _thread, ILObject * _this);
extern void _IL_Thread_InternalSetPriority(ILExecThread * _thread, ILObject * _this, ILInt32 value);
extern ILInt32 _IL_Thread_InternalGetState(ILExecThread * _thread, ILObject * _this);

extern ILInt32 _IL_DefaultEncoding_InternalGetByteCount_acii(ILExecThread * _thread, System_Array * chars, ILInt32 index, ILInt32 count);
extern ILInt32 _IL_DefaultEncoding_InternalGetByteCount_Stringii(ILExecThread * _thread, ILString * s, ILInt32 index, ILInt32 count);
extern ILInt32 _IL_DefaultEncoding_InternalGetBytes_aciiaBi(ILExecThread * _thread, System_Array * chars, ILInt32 charIndex, ILInt32 charCount, System_Array * bytes, ILInt32 byteIndex);
extern ILInt32 _IL_DefaultEncoding_InternalGetBytes_StringiiaBi(ILExecThread * _thread, ILString * s, ILInt32 charIndex, ILInt32 charCount, System_Array * bytes, ILInt32 byteIndex);
extern ILInt32 _IL_DefaultEncoding_InternalGetCharCount(ILExecThread * _thread, System_Array * bytes, ILInt32 index, ILInt32 count);
extern ILInt32 _IL_DefaultEncoding_InternalGetChars(ILExecThread * _thread, System_Array * bytes, ILInt32 byteIndex, ILInt32 byteCount, System_Array * chars, ILInt32 charIndex);
extern ILInt32 _IL_DefaultEncoding_InternalGetMaxByteCount(ILExecThread * _thread, ILInt32 charCount);
extern ILInt32 _IL_DefaultEncoding_InternalGetMaxCharCount(ILExecThread * _thread, ILInt32 byteCount);
extern ILString * _IL_DefaultEncoding_InternalGetString(ILExecThread * _thread, System_Array * bytes, ILInt32 index, ILInt32 count);
extern ILInt32 _IL_DefaultEncoding_InternalCodePage(ILExecThread * _thread);

extern ILBool _IL_ClrSecurity_Assert(ILExecThread * _thread, ILObject * perm, ILInt32 skipFrames);
extern ILBool _IL_ClrSecurity_Demand(ILExecThread * _thread, ILObject * perm, ILInt32 skipFrames);
extern void _IL_ClrSecurity_Deny(ILExecThread * _thread, ILObject * perm, ILInt32 skipFrames);
extern void _IL_ClrSecurity_SetPermitOnlyBlock(ILExecThread * _thread, ILInt32 skipFrames);
extern void _IL_ClrSecurity_PermitOnly(ILExecThread * _thread, ILObject * perm, ILInt32 skipFrames);

extern void _IL_CodeAccessPermission_RevertAll(ILExecThread * _thread);
extern void _IL_CodeAccessPermission_RevertAssert(ILExecThread * _thread);
extern void _IL_CodeAccessPermission_RevertDeny(ILExecThread * _thread);
extern void _IL_CodeAccessPermission_RevertPermitOnly(ILExecThread * _thread);

extern ILNativeInt _IL_GCHandle_GCAddrOfPinnedObject(ILExecThread * _thread, ILInt32 handle);
extern ILInt32 _IL_GCHandle_GCAlloc(ILExecThread * _thread, ILObject * value, ILInt32 type);
extern void _IL_GCHandle_GCFree(ILExecThread * _thread, ILInt32 handle);
extern ILBool _IL_GCHandle_GCValidate(ILExecThread * _thread, ILInt32 handle);
extern ILObject * _IL_GCHandle_GCGetTarget(ILExecThread * _thread, ILInt32 handle);
extern void _IL_GCHandle_GCSetTarget(ILExecThread * _thread, ILInt32 handle, ILObject * value);

extern void _IL_RuntimeHelpers_InitializeArray(ILExecThread * _thread, ILObject * array, void * field);
extern void _IL_RuntimeHelpers_RunClassConstructor(ILExecThread * _thread, void * type);
extern ILInt32 _IL_RuntimeHelpers_InternalOffsetToStringData(ILExecThread * _thread);
extern ILObject * _IL_RuntimeHelpers_GetObjectValue(ILExecThread * _thread, ILObject * obj);

extern ILObject * _IL_Assembly_GetCallingAssembly(ILExecThread * _thread);
extern ILObject * _IL_Assembly_GetExecutingAssembly(ILExecThread * _thread);
extern ILObject * _IL_Assembly_GetEntryAssembly(ILExecThread * _thread);
extern System_Array * _IL_Assembly_GetExportedTypes(ILExecThread * _thread, ILObject * _this);
extern ILObject * _IL_Assembly_GetFile(ILExecThread * _thread, ILObject * _this, ILString * name);
extern System_Array * _IL_Assembly_GetFiles(ILExecThread * _thread, ILObject * _this, ILBool getResourceModules);
extern ILObject * _IL_Assembly_GetManifestResourceInfo(ILExecThread * _thread, ILObject * _this, ILString * resourceName);
extern System_Array * _IL_Assembly_GetManifestResourceNames(ILExecThread * _thread, ILObject * _this);
extern ILObject * _IL_Assembly_GetManifestResourceStream(ILExecThread * _thread, ILObject * _this, ILString * name);
extern ILObject * _IL_Assembly_GetType(ILExecThread * _thread, ILObject * _this, ILString * typeName, ILBool throwOnError, ILBool ignoreCase);
extern System_Array * _IL_Assembly_GetTypes(ILExecThread * _thread, ILObject * _this);
extern ILObject * _IL_Assembly_LoadFromName(ILExecThread * _thread, ILString * name, ILInt32 * error, ILObject * parent);
extern ILObject * _IL_Assembly_LoadFromFile(ILExecThread * _thread, ILString * name, ILInt32 * error, ILObject * parent);
extern void _IL_Assembly_GetEntryPoint(ILExecThread * _thread, void * _result, ILObject * _this);

extern ILObject * _IL_MethodBase_GetMethodFromHandle(ILExecThread * _thread, void * handle);
extern ILObject * _IL_MethodBase_GetCurrentMethod(ILExecThread * _thread);

extern ILObject * _IL_ClrConstructor_Invoke(ILExecThread * _thread, ILObject * _this, ILInt32 invokeAttr, ILObject * binder, System_Array * parameters, ILObject * culture);

extern ILObject * _IL_FieldInfo_GetFieldFromHandle(ILExecThread * _thread, void * handle);

extern ILObject * _IL_ClrField_GetValue(ILExecThread * _thread, ILObject * _this, ILObject * obj);
extern void _IL_ClrField_SetValue(ILExecThread * _thread, ILObject * _this, ILObject * obj, ILObject * value, ILInt32 invokeAttr, ILObject * binder, ILObject * culture);
extern ILObject * _IL_ClrField_GetFieldType(ILExecThread * _thread, ILNativeInt item);
extern ILObject * _IL_ClrField_GetValueDirect(ILExecThread * _thread, ILObject * _this, ILTypedRef obj);
extern void _IL_ClrField_SetValueDirect(ILExecThread * _thread, ILObject * _this, ILTypedRef obj, ILObject * value);

extern System_Array * _IL_ClrHelpers_GetCustomAttributes(ILExecThread * _thread, ILNativeInt item, ILNativeInt type, ILBool inherit);
extern ILBool _IL_ClrHelpers_IsDefined(ILExecThread * _thread, ILNativeInt item, ILNativeInt type, ILBool inherit);
extern ILNativeInt _IL_ClrHelpers_GetDeclaringType(ILExecThread * _thread, ILNativeInt item);
extern ILString * _IL_ClrHelpers_GetName(ILExecThread * _thread, ILNativeInt item);
extern ILNativeInt _IL_ClrHelpers_GetParameter(ILExecThread * _thread, ILNativeInt item, ILInt32 num);
extern ILObject * _IL_ClrHelpers_GetParameterType(ILExecThread * _thread, ILNativeInt item, ILInt32 num);
extern ILInt32 _IL_ClrHelpers_GetNumParameters(ILExecThread * _thread, ILNativeInt item);
extern ILInt32 _IL_ClrHelpers_GetMemberAttrs(ILExecThread * _thread, ILNativeInt item);
extern ILInt32 _IL_ClrHelpers_GetCallConv(ILExecThread * _thread, ILNativeInt item);
extern ILInt32 _IL_ClrHelpers_GetImplAttrs(ILExecThread * _thread, ILNativeInt item);
extern ILObject * _IL_ClrHelpers_GetSemantics(ILExecThread * _thread, ILNativeInt item, ILInt32 type, ILBool nonPublic);
extern ILBool _IL_ClrHelpers_HasSemantics(ILExecThread * _thread, ILNativeInt item, ILInt32 type, ILBool nonPublic);
extern ILBool _IL_ClrHelpers_CanAccess(ILExecThread * _thread, ILNativeInt item);

extern ILObject * _IL_ClrMethod_Invoke(ILExecThread * _thread, ILObject * _this, ILObject * obj, ILInt32 invokeAttr, ILObject * binder, System_Array * parameters, ILObject * culture);
extern ILObject * _IL_ClrMethod_GetBaseDefinition(ILExecThread * _thread, ILObject * _this);

extern ILInt32 _IL_ClrParameter_GetParamAttrs(ILExecThread * _thread, ILNativeInt item);
extern ILString * _IL_ClrParameter_GetParamName(ILExecThread * _thread, ILNativeInt item);
extern ILObject * _IL_ClrParameter_GetDefault(ILExecThread * _thread, ILNativeInt item);

extern ILObject * _IL_ClrProperty_GetPropertyType(ILExecThread * _thread, ILNativeInt item);

extern ILInt32 _IL_ClrResourceStream_ResourceRead(ILExecThread * _thread, ILNativeInt handle, ILInt64 position, System_Array * buffer, ILInt32 offset, ILInt32 count);
extern ILInt32 _IL_ClrResourceStream_ResourceReadByte(ILExecThread * _thread, ILNativeInt handle, ILInt64 position);
extern ILUInt8 * _IL_ClrResourceStream_ResourceGetAddress(ILExecThread * _thread, ILNativeInt handle, ILInt64 position);

extern ILInt32 _IL_ClrType_GetClrArrayRank(ILExecThread * _thread, ILObject * _this);
extern ILInt32 _IL_ClrType_GetAttributeFlagsImpl(ILExecThread * _thread, ILObject * _this);
extern ILObject * _IL_ClrType_GetElementType(ILExecThread * _thread, ILObject * _this);
extern ILObject * _IL_ClrType_GetInterface(ILExecThread * _thread, ILObject * _this, ILString * name, ILBool ignoreCase);
extern System_Array * _IL_ClrType_GetInterfaces(ILExecThread * _thread, ILObject * _this);
extern ILObject * _IL_ClrType_GetMemberImpl(ILExecThread * _thread, ILObject * _this, ILString * name, ILInt32 memberTypes, ILInt32 bindingAttr, ILObject * binder, ILInt32 callingConventions, System_Array * types, System_Array * modifiers);
extern ILObject * _IL_ClrType_GetMembersImpl(ILExecThread * _thread, ILObject * _this, ILInt32 memberTypes, ILInt32 bindingAttr, ILObject * arrayType, ILString * name);
extern ILInt32 _IL_ClrType_GetClrTypeCategory(ILExecThread * _thread, ILObject * _this);
extern ILBool _IL_ClrType_IsSubclassOf(ILExecThread * _thread, ILObject * _this, ILObject * c);
extern ILBool _IL_ClrType_IsClrNestedType(ILExecThread * _thread, ILObject * _this);
extern ILObject * _IL_ClrType_GetClrAssembly(ILExecThread * _thread, ILObject * _this);
extern ILObject * _IL_ClrType_GetClrBaseType(ILExecThread * _thread, ILObject * _this);
extern ILString * _IL_ClrType_GetClrFullName(ILExecThread * _thread, ILObject * _this);
extern void _IL_ClrType_GetClrGUID(ILExecThread * _thread, void * _result, ILObject * _this);
extern ILObject * _IL_ClrType_GetClrModule(ILExecThread * _thread, ILObject * _this);
extern ILObject * _IL_ClrType_GetClrNestedDeclaringType(ILExecThread * _thread, ILObject * _this);
extern ILString * _IL_ClrType_GetClrName(ILExecThread * _thread, ILObject * _this);
extern ILString * _IL_ClrType_GetClrNamespace(ILExecThread * _thread, ILObject * _this);

extern ILObject * _IL_Module_GetType(ILExecThread * _thread, ILObject * _this, ILString * name, ILBool throwOnError, ILBool ignoreCase);
extern System_Array * _IL_Module_GetTypes(ILExecThread * _thread, ILObject * _this);
extern ILBool _IL_Module_IsResource(ILExecThread * _thread, ILObject * _this);
extern ILObject * _IL_Module_GetModuleType(ILExecThread * _thread, ILObject * _this);
extern ILObject * _IL_Module_GetAssembly(ILExecThread * _thread, ILObject * _this);
extern ILString * _IL_Module_GetFullName(ILExecThread * _thread, ILObject * _this);

extern void _IL_AssemblyBuilder_ClrSetEntryPoint(ILExecThread * _thread, ILObject * _this, ILNativeInt clrMethod, ILInt32 fileKind);

extern ILNativeInt _IL_ModuleBuilder_ClrModuleCreate(ILExecThread * _thread, ILNativeInt assembly, ILString * name);

extern ILBool _IL_Debugger_InternalIsAttached(ILExecThread * _thread);
extern void _IL_Debugger_Break(ILExecThread * _thread);
extern ILBool _IL_Debugger_IsLogging(ILExecThread * _thread);
extern ILBool _IL_Debugger_InternalLaunch(ILExecThread * _thread);
extern void _IL_Debugger_Log(ILExecThread * _thread, ILInt32 level, ILString * category, ILString * message);

extern ILInt32 _IL_StackFrame_InternalGetTotalFrames(ILExecThread * _thread);
extern void _IL_StackFrame_InternalGetMethod(ILExecThread * _thread, void * _result, ILInt32 skipFrames);
extern ILInt32 _IL_StackFrame_InternalGetILOffset(ILExecThread * _thread, ILInt32 skipFrames);
extern ILInt32 _IL_StackFrame_InternalGetNativeOffset(ILExecThread * _thread, ILInt32 skipFrames);
extern ILString * _IL_StackFrame_InternalGetDebugInfo(ILExecThread * _thread, void * method, ILInt32 offset, ILInt32 * line, ILInt32 * column);
extern System_Array * _IL_StackFrame_GetExceptionStackTrace(ILExecThread * _thread);

extern void _IL_DirMethods_GetPathInfo(ILExecThread * _thread, void * _result);
extern ILString * _IL_DirMethods_GetSystemDirectory(ILExecThread * _thread);
extern ILInt32 _IL_DirMethods_GetLastAccess(ILExecThread * _thread, ILString * path, ILInt64 * lastac);
extern ILInt32 _IL_DirMethods_GetLastModification(ILExecThread * _thread, ILString * path, ILInt64 * last_mod);
extern ILInt32 _IL_DirMethods_GetCreationTime(ILExecThread * _thread, ILString * path, ILInt64 * create_time);
extern ILInt32 _IL_DirMethods_Copy(ILExecThread * _thread, ILString * src, ILString * dest);
extern ILInt32 _IL_DirMethods_Delete(ILExecThread * _thread, ILString * path);
extern ILInt32 _IL_DirMethods_Rename(ILExecThread * _thread, ILString * old_name, ILString * new_name);
extern ILString * _IL_DirMethods_GetCurrentDirectory(ILExecThread * _thread);
extern ILInt32 _IL_DirMethods_GetFilesInDirectory(ILExecThread * _thread, ILString * path, ILString * * Files);

extern ILNativeInt _IL_FileMethods_GetInvalidHandle(ILExecThread * _thread);
extern ILBool _IL_FileMethods_ValidatePathname(ILExecThread * _thread, ILString * path);
extern ILBool _IL_FileMethods_Open(ILExecThread * _thread, ILString * path, ILInt32 mode, ILInt32 access, ILInt32 share, ILNativeInt * handle);
extern ILBool _IL_FileMethods_HasAsync(ILExecThread * _thread);
extern ILBool _IL_FileMethods_CanSeek(ILExecThread * _thread, ILNativeInt handle);
extern ILBool _IL_FileMethods_CheckHandleAccess(ILExecThread * _thread, ILNativeInt handle, ILInt32 access);
extern ILInt64 _IL_FileMethods_Seek(ILExecThread * _thread, ILNativeInt handle, ILInt64 offset, ILInt32 origin);
extern ILBool _IL_FileMethods_Write(ILExecThread * _thread, ILNativeInt handle, System_Array * buffer, ILInt32 offset, ILInt32 count);
extern ILInt32 _IL_FileMethods_Read(ILExecThread * _thread, ILNativeInt handle, System_Array * buffer, ILInt32 offset, ILInt32 count);
extern ILBool _IL_FileMethods_Close(ILExecThread * _thread, ILNativeInt handle);
extern ILBool _IL_FileMethods_FlushWrite(ILExecThread * _thread, ILNativeInt handle);
extern ILBool _IL_FileMethods_SetLength(ILExecThread * _thread, ILNativeInt handle, ILInt64 value);
extern ILInt32 _IL_FileMethods_GetErrno(ILExecThread * _thread);
extern ILString * _IL_FileMethods_GetErrnoMessage(ILExecThread * _thread, ILInt32 errno);

extern ILBool _IL_RuntimeSecurityManager_CanUseFileHandle(ILExecThread * _thread, ILObject * _this, ILNativeInt handle);
extern ILBool _IL_RuntimeSecurityManager_CanOpenFile(ILExecThread * _thread, ILObject * _this, ILString * path, ILInt32 mode, ILInt32 access, ILInt32 share);

extern ILObject * _IL_Security_GetSecurityManager(ILExecThread * _thread);
extern void _IL_Security_SetSecurityManager(ILExecThread * _thread, ILObject * mgr);

extern ILBool _IL_SocketMethods_Create(ILExecThread * _thread, ILInt32 af, ILInt32 st, ILInt32 pt, ILNativeInt * handle);
extern ILBool _IL_SocketMethods_Bind(ILExecThread * _thread, ILNativeInt handle, ILInt32 af, ILInt64 address, ILInt32 port);
extern ILBool _IL_SocketMethods_Shutdown(ILExecThread * _thread, ILNativeInt handle, ILInt32 how);
extern ILBool _IL_SocketMethods_Listen(ILExecThread * _thread, ILNativeInt handle, ILInt32 backlog);
extern ILBool _IL_SocketMethods_Accept(ILExecThread * _thread, ILNativeInt handle, ILInt64 * address, ILInt32 * port, ILNativeInt * newHandle);
extern ILBool _IL_SocketMethods_Connect(ILExecThread * _thread, ILNativeInt handle, ILInt32 af, ILInt64 address, ILInt32 port);
extern ILInt32 _IL_SocketMethods_Receive(ILExecThread * _thread, ILNativeInt handle, System_Array * buffer, ILInt32 offset, ILInt32 size, ILInt32 flags);
extern ILInt32 _IL_SocketMethods_ReceiveFrom(ILExecThread * _thread, ILNativeInt handle, System_Array * buffer, ILInt32 offset, ILInt32 size, ILInt32 flags, ILInt64 * address, ILInt32 * port);
extern ILInt32 _IL_SocketMethods_Send(ILExecThread * _thread, ILNativeInt handle, System_Array * buffer, ILInt32 offset, ILInt32 size, ILInt32 flags);
extern ILInt32 _IL_SocketMethods_SendTo(ILExecThread * _thread, ILNativeInt handle, System_Array * buffer, ILInt32 offset, ILInt32 size, ILInt32 flags, ILInt64 address, ILInt32 port);
extern ILBool _IL_SocketMethods_Close(ILExecThread * _thread, ILNativeInt handle);
extern ILInt32 _IL_SocketMethods_Select(ILExecThread * _thread, System_Array * readarray, System_Array * writearray, System_Array * errorarray, ILInt64 timeout);
extern ILInt32 _IL_SocketMethods_GetErrno(ILExecThread * _thread);
extern ILString * _IL_SocketMethods_GetErrnoMessage(ILExecThread * _thread, ILInt32 errno);

extern void _IL_Stdio_StdClose(ILExecThread * _thread, ILInt32 fd);
extern void _IL_Stdio_StdFlush(ILExecThread * _thread, ILInt32 fd);
extern void _IL_Stdio_StdWrite_ic(ILExecThread * _thread, ILInt32 fd, ILUInt16 value);
extern void _IL_Stdio_StdWrite_iacii(ILExecThread * _thread, ILInt32 fd, System_Array * value, ILInt32 index, ILInt32 count);
extern void _IL_Stdio_StdWrite_iaBii(ILExecThread * _thread, ILInt32 fd, System_Array * value, ILInt32 index, ILInt32 count);
extern void _IL_Stdio_StdWrite_iString(ILExecThread * _thread, ILInt32 fd, ILString * value);
extern ILInt32 _IL_Stdio_StdRead_i(ILExecThread * _thread, ILInt32 fd);
extern ILInt32 _IL_Stdio_StdRead_iacii(ILExecThread * _thread, ILInt32 fd, System_Array * value, ILInt32 index, ILInt32 count);
extern ILInt32 _IL_Stdio_StdRead_iaBii(ILExecThread * _thread, ILInt32 fd, System_Array * value, ILInt32 index, ILInt32 count);
extern ILInt32 _IL_Stdio_StdPeek(ILExecThread * _thread, ILInt32 fd);

extern ILInt32 _IL_SysCharInfo_GetUnicodeCategory(ILExecThread * _thread, ILUInt16 ch);
extern ILDouble _IL_SysCharInfo_GetNumericValue(ILExecThread * _thread, ILUInt16 ch);
extern ILString * _IL_SysCharInfo_GetNewLine(ILExecThread * _thread);

extern void _IL_TaskMethods_Exit(ILExecThread * _thread, ILInt32 exitCode);
extern void _IL_TaskMethods_SetExitCode(ILExecThread * _thread, ILInt32 exitCode);
extern System_Array * _IL_TaskMethods_GetCommandLineArgs(ILExecThread * _thread);
extern ILString * _IL_TaskMethods_GetEnvironmentVariable(ILExecThread * _thread, ILString * variable);
extern ILInt32 _IL_TaskMethods_GetEnvironmentCount(ILExecThread * _thread);
extern ILString * _IL_TaskMethods_GetEnvironmentKey(ILExecThread * _thread, ILInt32 posn);
extern ILString * _IL_TaskMethods_GetEnvironmentValue(ILExecThread * _thread, ILInt32 posn);

extern ILInt64 _IL_TimeMethods_GetCurrentTime(ILExecThread * _thread);
extern ILInt64 _IL_TimeMethods_GetCurrentUtcTime(ILExecThread * _thread);
extern ILInt32 _IL_TimeMethods_GetTimeZoneAdjust(ILExecThread * _thread);
extern ILInt32 _IL_TimeMethods_GetUpTime(ILExecThread * _thread);

extern ILInt64 _IL_IPAddress_HostToNetworkOrder_l(ILExecThread * _thread, ILInt64 host);
extern ILInt32 _IL_IPAddress_HostToNetworkOrder_i(ILExecThread * _thread, ILInt32 host);
extern ILInt16 _IL_IPAddress_HostToNetworkOrder_s(ILExecThread * _thread, ILInt16 host);
extern ILInt64 _IL_IPAddress_NetworkToHostOrder_l(ILExecThread * _thread, ILInt64 network);
extern ILInt32 _IL_IPAddress_NetworkToHostOrder_i(ILExecThread * _thread, ILInt32 network);
extern ILInt16 _IL_IPAddress_NetworkToHostOrder_s(ILExecThread * _thread, ILInt16 network);

extern ILUInt8 * _IL_CodeTable_GetAddress(ILExecThread * _thread, ILObject * stream, ILInt64 position);

