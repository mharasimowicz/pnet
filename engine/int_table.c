/* This file is automatically generated - do not edit */

IL_METHOD_BEGIN(Object_Methods)
	IL_METHOD("GetType", "(T)oSystem.Type;", _IL_Object_GetType)
	IL_METHOD("GetHashCode", "(T)i", _IL_Object_GetHashCode)
	IL_METHOD("Equals", "(ToSystem.Object;)Z", _IL_Object_Equals)
	IL_METHOD("MemberwiseClone", "(T)oSystem.Object;", _IL_Object_MemberwiseClone)
IL_METHOD_END

IL_METHOD_BEGIN(ArgIterator_Methods)
	IL_CONSTRUCTOR(".ctor", "(TvSystem.RuntimeArgumentHandle;)V", _IL_ArgIterator_ctor_RuntimeArgumentHandle, 0)
	IL_CONSTRUCTOR(".ctor", "(TvSystem.RuntimeArgumentHandle;*V)V", _IL_ArgIterator_ctor_RuntimeArgumentHandlepV, 0)
	IL_METHOD("GetNextArg", "(T)r", _IL_ArgIterator_GetNextArg_)
	IL_METHOD("GetNextArg", "(TvSystem.RuntimeTypeHandle;)r", _IL_ArgIterator_GetNextArg_RuntimeTypeHandle)
	IL_METHOD("GetNextArgType", "(T)vSystem.RuntimeTypeHandle;", _IL_ArgIterator_GetNextArgType)
	IL_METHOD("GetRemainingCount", "(T)i", _IL_ArgIterator_GetRemainingCount)
IL_METHOD_END

IL_METHOD_BEGIN(Array_Methods)
	IL_METHOD("Clear", "(oSystem.Array;ii)V", _IL_Array_Clear)
	IL_METHOD("Initialize", "(T)V", _IL_Array_Initialize)
	IL_METHOD("InternalCopy", "(oSystem.Array;ioSystem.Array;ii)V", _IL_Array_InternalCopy)
	IL_METHOD("CreateArray", "(jiiii)oSystem.Array;", _IL_Array_CreateArray_jiiii)
	IL_METHOD("CreateArray", "(j[i[i)oSystem.Array;", _IL_Array_CreateArray_jaiai)
	IL_METHOD("GetLength", "(Ti)i", _IL_Array_GetLength_i)
	IL_METHOD("GetLowerBound", "(Ti)i", _IL_Array_GetLowerBound)
	IL_METHOD("GetUpperBound", "(Ti)i", _IL_Array_GetUpperBound)
	IL_METHOD("Get", "(Tiii)oSystem.Object;", _IL_Array_Get_iii)
	IL_METHOD("Get", "(T[i)oSystem.Object;", _IL_Array_Get_ai)
	IL_METHOD("Set", "(ToSystem.Object;iii)V", _IL_Array_Set_Objectiii)
	IL_METHOD("Set", "(ToSystem.Object;[i)V", _IL_Array_Set_Objectai)
	IL_METHOD("GetLength", "(T)i", _IL_Array_GetLength_)
	IL_METHOD("GetRank", "(T)i", _IL_Array_GetRank)
IL_METHOD_END

IL_METHOD_BEGIN(Delegate_Methods)
	IL_METHOD("CreateBlankDelegate", "(oSystem.Type;oSystem.Reflection.ClrMethod;)oSystem.Delegate;", _IL_Delegate_CreateBlankDelegate)
IL_METHOD_END

IL_METHOD_BEGIN(Enum_Methods)
	IL_METHOD("GetEnumValue", "(T)oSystem.Object;", _IL_Enum_GetEnumValue)
	IL_METHOD("GetEnumName", "(oSystem.Type;oSystem.Object;)oSystem.String;", _IL_Enum_GetEnumName)
	IL_METHOD("IsEnumValue", "(oSystem.Type;oSystem.Object;)Z", _IL_Enum_IsEnumValue)
	IL_METHOD("GetEnumValueFromName", "(oSystem.Type;oSystem.Object;Z)oSystem.Object;", _IL_Enum_GetEnumValueFromName)
	IL_METHOD("EnumValueOr", "(oSystem.Object;oSystem.Object;)oSystem.Object;", _IL_Enum_EnumValueOr)
	IL_METHOD("EnumIntToObject", "(oSystem.Type;i)oSystem.Object;", _IL_Enum_EnumIntToObject)
	IL_METHOD("EnumLongToObject", "(oSystem.Type;l)oSystem.Object;", _IL_Enum_EnumLongToObject)
	IL_METHOD("FormatEnumWithFlags", "(oSystem.Type;oSystem.Object;)oSystem.String;", _IL_Enum_FormatEnumWithFlags)
IL_METHOD_END

IL_METHOD_BEGIN(BitConverter_Methods)
	IL_METHOD("GetLittleEndian", "()Z", _IL_BitConverter_GetLittleEndian)
	IL_METHOD("DoubleToInt64Bits", "(d)l", _IL_BitConverter_DoubleToInt64Bits)
	IL_METHOD("Int64BitsToDouble", "(l)d", _IL_BitConverter_Int64BitsToDouble)
	IL_METHOD("FloatToInt32Bits", "(f)i", _IL_BitConverter_FloatToInt32Bits)
	IL_METHOD("Int32BitsToFloat", "(i)f", _IL_BitConverter_Int32BitsToFloat)
IL_METHOD_END

IL_METHOD_BEGIN(Buffer_Methods)
	IL_METHOD("Copy", "(oSystem.Array;ioSystem.Array;ii)V", _IL_Buffer_Copy)
	IL_METHOD("GetLength", "(oSystem.Array;)i", _IL_Buffer_GetLength)
	IL_METHOD("GetElement", "(oSystem.Array;i)B", _IL_Buffer_GetElement)
	IL_METHOD("SetElement", "(oSystem.Array;iB)V", _IL_Buffer_SetElement)
IL_METHOD_END

IL_METHOD_BEGIN(Decimal_Methods)
	IL_METHOD("Add", "(vSystem.Decimal;vSystem.Decimal;)vSystem.Decimal;", _IL_Decimal_Add)
	IL_METHOD("Compare", "(vSystem.Decimal;vSystem.Decimal;)i", _IL_Decimal_Compare)
	IL_METHOD("Divide", "(vSystem.Decimal;vSystem.Decimal;)vSystem.Decimal;", _IL_Decimal_Divide)
	IL_METHOD("Floor", "(vSystem.Decimal;)vSystem.Decimal;", _IL_Decimal_Floor)
	IL_METHOD("Remainder", "(vSystem.Decimal;vSystem.Decimal;)vSystem.Decimal;", _IL_Decimal_Remainder)
	IL_METHOD("Multiply", "(vSystem.Decimal;vSystem.Decimal;)vSystem.Decimal;", _IL_Decimal_Multiply)
	IL_METHOD("Negate", "(vSystem.Decimal;)vSystem.Decimal;", _IL_Decimal_Negate)
	IL_METHOD("Round", "(vSystem.Decimal;i)vSystem.Decimal;", _IL_Decimal_Round)
	IL_METHOD("Subtract", "(vSystem.Decimal;vSystem.Decimal;)vSystem.Decimal;", _IL_Decimal_Subtract)
	IL_METHOD("Truncate", "(vSystem.Decimal;)vSystem.Decimal;", _IL_Decimal_Truncate)
	IL_CONSTRUCTOR(".ctor", "(Tf)V", _IL_Decimal_ctor_f, 0)
	IL_CONSTRUCTOR(".ctor", "(Td)V", _IL_Decimal_ctor_d, 0)
	IL_METHOD("ToSingle", "(vSystem.Decimal;)f", _IL_Decimal_ToSingle)
	IL_METHOD("ToDouble", "(vSystem.Decimal;)d", _IL_Decimal_ToDouble)
IL_METHOD_END

IL_METHOD_BEGIN(Double_Methods)
	IL_METHOD("IsNaN", "(d)Z", _IL_Double_IsNaN)
	IL_METHOD("TestInfinity", "(d)i", _IL_Double_TestInfinity)
IL_METHOD_END

IL_METHOD_BEGIN(GC_Methods)
	IL_METHOD("KeepAlive", "(oSystem.Object;)V", _IL_GC_KeepAlive)
	IL_METHOD("ReRegisterForFinalize", "(oSystem.Object;)V", _IL_GC_ReRegisterForFinalize)
	IL_METHOD("SuppressFinalize", "(oSystem.Object;)V", _IL_GC_SuppressFinalize)
	IL_METHOD("WaitForPendingFinalizers", "()V", _IL_GC_WaitForPendingFinalizers)
IL_METHOD_END

IL_METHOD_BEGIN(Guid_Methods)
	IL_METHOD("NewGuid", "()vSystem.Guid;", _IL_Guid_NewGuid)
IL_METHOD_END

IL_METHOD_BEGIN(Math_Methods)
	IL_METHOD("Acos", "(d)d", _IL_Math_Acos)
	IL_METHOD("Asin", "(d)d", _IL_Math_Asin)
	IL_METHOD("Atan", "(d)d", _IL_Math_Atan)
	IL_METHOD("Atan2", "(dd)d", _IL_Math_Atan2)
	IL_METHOD("Ceiling", "(d)d", _IL_Math_Ceiling)
	IL_METHOD("Cos", "(d)d", _IL_Math_Cos)
	IL_METHOD("Cosh", "(d)d", _IL_Math_Cosh)
	IL_METHOD("Exp", "(d)d", _IL_Math_Exp)
	IL_METHOD("Floor", "(d)d", _IL_Math_Floor)
	IL_METHOD("IEEERemainder", "(dd)d", _IL_Math_IEEERemainder)
	IL_METHOD("Log", "(d)d", _IL_Math_Log)
	IL_METHOD("Log10", "(d)d", _IL_Math_Log10)
	IL_METHOD("Pow", "(dd)d", _IL_Math_Pow)
	IL_METHOD("Round", "(d)d", _IL_Math_Round)
	IL_METHOD("RoundDouble", "(di)d", _IL_Math_RoundDouble)
	IL_METHOD("Sin", "(d)d", _IL_Math_Sin)
	IL_METHOD("Sinh", "(d)d", _IL_Math_Sinh)
	IL_METHOD("Sqrt", "(d)d", _IL_Math_Sqrt)
	IL_METHOD("Tan", "(d)d", _IL_Math_Tan)
	IL_METHOD("Tanh", "(d)d", _IL_Math_Tanh)
IL_METHOD_END

IL_METHOD_BEGIN(RuntimeMethodHandle_Methods)
	IL_METHOD("GetFunctionPointer", "(T)j", _IL_RuntimeMethodHandle_GetFunctionPointer)
IL_METHOD_END

IL_METHOD_BEGIN(Single_Methods)
	IL_METHOD("IsNaN", "(f)Z", _IL_Single_IsNaN)
	IL_METHOD("TestInfinity", "(f)i", _IL_Single_TestInfinity)
IL_METHOD_END

IL_METHOD_BEGIN(String_Methods)
	IL_CONSTRUCTOR(".ctor", "(T[cii)V", 0, _IL_String_ctor_acii)
	IL_CONSTRUCTOR(".ctor", "(T[c)V", 0, _IL_String_ctor_ac)
	IL_CONSTRUCTOR(".ctor", "(Tci)V", 0, _IL_String_ctor_ci)
	IL_CONSTRUCTOR(".ctor", "(T*cii)V", 0, _IL_String_ctor_pcii)
	IL_CONSTRUCTOR(".ctor", "(T*c)V", 0, _IL_String_ctor_pc)
	IL_CONSTRUCTOR(".ctor", "(T*biioSystem.Text.Encoding;)V", 0, _IL_String_ctor_pbiiEncoding)
	IL_CONSTRUCTOR(".ctor", "(T*bii)V", 0, _IL_String_ctor_pbii)
	IL_CONSTRUCTOR(".ctor", "(T*b)V", 0, _IL_String_ctor_pb)
	IL_METHOD("Compare", "(oSystem.String;oSystem.String;)i", _IL_String_Compare)
	IL_METHOD("InternalCompare", "(oSystem.String;iioSystem.String;iiZoSystem.Globalization.CultureInfo;)i", _IL_String_InternalCompare)
	IL_METHOD("InternalOrdinal", "(oSystem.String;iioSystem.String;ii)i", _IL_String_InternalOrdinal)
	IL_METHOD("NewString", "(i)oSystem.String;", _IL_String_NewString)
	IL_METHOD("NewBuilder", "(oSystem.String;i)oSystem.String;", _IL_String_NewBuilder)
	IL_METHOD("Copy", "(oSystem.String;ioSystem.String;)V", _IL_String_Copy_StringiString)
	IL_METHOD("Copy", "(oSystem.String;ioSystem.String;ii)V", _IL_String_Copy_StringiStringii)
	IL_METHOD("InsertSpace", "(oSystem.String;ii)V", _IL_String_InsertSpace)
	IL_METHOD("RemoveSpace", "(oSystem.String;ii)V", _IL_String_RemoveSpace)
	IL_METHOD("Concat", "(oSystem.String;oSystem.String;)oSystem.String;", _IL_String_Concat_StringString)
	IL_METHOD("Concat", "(oSystem.String;oSystem.String;oSystem.String;)oSystem.String;", _IL_String_Concat_StringStringString)
	IL_METHOD("CopyToChecked", "(Ti[cii)V", _IL_String_CopyToChecked)
	IL_METHOD("Equals", "(oSystem.String;oSystem.String;)Z", _IL_String_Equals)
	IL_METHOD("GetHashCode", "(T)i", _IL_String_GetHashCode)
	IL_METHOD("IndexOf", "(Tcii)i", _IL_String_IndexOf)
	IL_METHOD("EqualRange", "(TiioSystem.String;i)Z", _IL_String_EqualRange)
	IL_METHOD("IndexOfAny", "(T[cii)i", _IL_String_IndexOfAny)
	IL_METHOD("Intern", "(oSystem.String;)oSystem.String;", _IL_String_Intern)
	IL_METHOD("IsInterned", "(oSystem.String;)oSystem.String;", _IL_String_IsInterned)
	IL_METHOD("LastIndexOf", "(Tcii)i", _IL_String_LastIndexOf)
	IL_METHOD("LastIndexOfAny", "(T[cii)i", _IL_String_LastIndexOfAny)
	IL_METHOD("CharFill", "(oSystem.String;iic)V", _IL_String_CharFill_Stringiic)
	IL_METHOD("CharFill", "(oSystem.String;i[cii)V", _IL_String_CharFill_Stringiacii)
	IL_METHOD("Replace", "(Tcc)oSystem.String;", _IL_String_Replace_cc)
	IL_METHOD("Replace", "(ToSystem.String;oSystem.String;)oSystem.String;", _IL_String_Replace_StringString)
	IL_METHOD("Trim", "(T[ci)oSystem.String;", _IL_String_Trim)
	IL_METHOD("GetChar", "(Ti)c", _IL_String_GetChar)
	IL_METHOD("SetChar", "(Tic)V", _IL_String_SetChar)
IL_METHOD_END

IL_METHOD_BEGIN(Type_Methods)
	IL_METHOD("GetType", "(oSystem.String;ZZ)oSystem.Type;", _IL_Type_GetType)
	IL_METHOD("GetTypeHandle", "(oSystem.Object;)vSystem.RuntimeTypeHandle;", _IL_Type_GetTypeHandle)
	IL_METHOD("GetTypeFromHandle", "(vSystem.RuntimeTypeHandle;)oSystem.Type;", _IL_Type_GetTypeFromHandle)
IL_METHOD_END

IL_METHOD_BEGIN(TypedReference_Methods)
	IL_METHOD("ClrMakeTypedReference", "(oSystem.Object;[oSystem.Reflection.FieldInfo;)r", _IL_TypedReference_ClrMakeTypedReference)
	IL_METHOD("SetTypedReference", "(roSystem.Object;)V", _IL_TypedReference_SetTypedReference)
	IL_METHOD("ToObject", "(r)oSystem.Object;", _IL_TypedReference_ToObject)
IL_METHOD_END

IL_METHOD_BEGIN(Interlocked_Methods)
	IL_METHOD("CompareExchange", "(&iii)i", _IL_Interlocked_CompareExchange_Riii)
	IL_METHOD("CompareExchange", "(&fff)f", _IL_Interlocked_CompareExchange_Rfff)
	IL_METHOD("CompareExchange", "(&oSystem.Object;oSystem.Object;oSystem.Object;)oSystem.Object;", _IL_Interlocked_CompareExchange_RObjectObjectObject)
	IL_METHOD("Decrement", "(&i)i", _IL_Interlocked_Decrement_Ri)
	IL_METHOD("Decrement", "(&l)l", _IL_Interlocked_Decrement_Rl)
	IL_METHOD("Exchange", "(&ii)i", _IL_Interlocked_Exchange_Rii)
	IL_METHOD("Exchange", "(&ff)f", _IL_Interlocked_Exchange_Rff)
	IL_METHOD("Exchange", "(&oSystem.Object;oSystem.Object;)oSystem.Object;", _IL_Interlocked_Exchange_RObjectObject)
	IL_METHOD("Increment", "(&i)i", _IL_Interlocked_Increment_Ri)
	IL_METHOD("Increment", "(&l)l", _IL_Interlocked_Increment_Rl)
IL_METHOD_END

IL_METHOD_BEGIN(Monitor_Methods)
	IL_METHOD("Enter", "(oSystem.Object;)V", _IL_Monitor_Enter)
	IL_METHOD("InternalTryEnter", "(oSystem.Object;i)Z", _IL_Monitor_InternalTryEnter)
	IL_METHOD("Exit", "(oSystem.Object;)V", _IL_Monitor_Exit)
	IL_METHOD("InternalWait", "(oSystem.Object;i)Z", _IL_Monitor_InternalWait)
	IL_METHOD("Pulse", "(oSystem.Object;)V", _IL_Monitor_Pulse)
	IL_METHOD("PulseAll", "(oSystem.Object;)V", _IL_Monitor_PulseAll)
IL_METHOD_END

IL_METHOD_BEGIN(WaitHandle_Methods)
	IL_METHOD("InternalClose", "(j)V", _IL_WaitHandle_InternalClose)
	IL_METHOD("InternalWaitAll", "([oSystem.Threading.WaitHandle;iZ)Z", _IL_WaitHandle_InternalWaitAll)
	IL_METHOD("InternalWaitAny", "([oSystem.Threading.WaitHandle;iZ)i", _IL_WaitHandle_InternalWaitAny)
	IL_METHOD("InternalWaitOne", "(ji)Z", _IL_WaitHandle_InternalWaitOne)
IL_METHOD_END

IL_METHOD_BEGIN(Mutex_Methods)
	IL_METHOD("InternalCreateMutex", "(ZoSystem.String;&Z)j", _IL_Mutex_InternalCreateMutex)
	IL_METHOD("InternalReleaseMutex", "(j)V", _IL_Mutex_InternalReleaseMutex)
IL_METHOD_END

IL_METHOD_BEGIN(Thread_Methods)
	IL_METHOD("FinalizeThread", "(T)V", _IL_Thread_FinalizeThread)
	IL_METHOD("Abort", "(T)V", _IL_Thread_Abort)
	IL_METHOD("InternalJoin", "(Ti)Z", _IL_Thread_InternalJoin)
	IL_METHOD("MemoryBarrier", "()V", _IL_Thread_MemoryBarrier)
	IL_METHOD("ResetAbort", "()V", _IL_Thread_ResetAbort)
	IL_METHOD("InternalSleep", "(i)V", _IL_Thread_InternalSleep)
	IL_METHOD("Start", "(T)V", _IL_Thread_Start)
	IL_METHOD("VolatileRead", "(&B)B", _IL_Thread_VolatileRead_RB)
	IL_METHOD("VolatileRead", "(&b)b", _IL_Thread_VolatileRead_Rb)
	IL_METHOD("VolatileRead", "(&s)s", _IL_Thread_VolatileRead_Rs)
	IL_METHOD("VolatileRead", "(&S)S", _IL_Thread_VolatileRead_RS)
	IL_METHOD("VolatileRead", "(&i)i", _IL_Thread_VolatileRead_Ri)
	IL_METHOD("VolatileRead", "(&I)I", _IL_Thread_VolatileRead_RI)
	IL_METHOD("VolatileRead", "(&l)l", _IL_Thread_VolatileRead_Rl)
	IL_METHOD("VolatileRead", "(&L)L", _IL_Thread_VolatileRead_RL)
	IL_METHOD("VolatileRead", "(&j)j", _IL_Thread_VolatileRead_Rj)
	IL_METHOD("VolatileRead", "(&J)J", _IL_Thread_VolatileRead_RJ)
	IL_METHOD("VolatileRead", "(&f)f", _IL_Thread_VolatileRead_Rf)
	IL_METHOD("VolatileRead", "(&d)d", _IL_Thread_VolatileRead_Rd)
	IL_METHOD("VolatileRead", "(&oSystem.Object;)oSystem.Object;", _IL_Thread_VolatileRead_RObject)
	IL_METHOD("VolatileWrite", "(&BB)V", _IL_Thread_VolatileWrite_RBB)
	IL_METHOD("VolatileWrite", "(&bb)V", _IL_Thread_VolatileWrite_Rbb)
	IL_METHOD("VolatileWrite", "(&ss)V", _IL_Thread_VolatileWrite_Rss)
	IL_METHOD("VolatileWrite", "(&SS)V", _IL_Thread_VolatileWrite_RSS)
	IL_METHOD("VolatileWrite", "(&ii)V", _IL_Thread_VolatileWrite_Rii)
	IL_METHOD("VolatileWrite", "(&II)V", _IL_Thread_VolatileWrite_RII)
	IL_METHOD("VolatileWrite", "(&ll)V", _IL_Thread_VolatileWrite_Rll)
	IL_METHOD("VolatileWrite", "(&LL)V", _IL_Thread_VolatileWrite_RLL)
	IL_METHOD("VolatileWrite", "(&jj)V", _IL_Thread_VolatileWrite_Rjj)
	IL_METHOD("VolatileWrite", "(&JJ)V", _IL_Thread_VolatileWrite_RJJ)
	IL_METHOD("VolatileWrite", "(&ff)V", _IL_Thread_VolatileWrite_Rff)
	IL_METHOD("VolatileWrite", "(&dd)V", _IL_Thread_VolatileWrite_Rdd)
	IL_METHOD("VolatileWrite", "(&oSystem.Object;oSystem.Object;)V", _IL_Thread_VolatileWrite_RObjectObject)
	IL_METHOD("InternalCurrentThread", "()oSystem.Threading.Thread;", _IL_Thread_InternalCurrentThread)
	IL_METHOD("InternalSetBackground", "(TZ)V", _IL_Thread_InternalSetBackground)
	IL_METHOD("InternalGetPriority", "(T)vSystem.Threading.ThreadPriority;", _IL_Thread_InternalGetPriority)
	IL_METHOD("InternalSetPriority", "(TvSystem.Threading.ThreadPriority;)V", _IL_Thread_InternalSetPriority)
	IL_METHOD("InternalGetState", "(T)vSystem.Threading.ThreadState;", _IL_Thread_InternalGetState)
IL_METHOD_END

IL_METHOD_BEGIN(DefaultEncoding_Methods)
	IL_METHOD("InternalGetByteCount", "([cii)i", _IL_DefaultEncoding_InternalGetByteCount_acii)
	IL_METHOD("InternalGetByteCount", "(oSystem.String;ii)i", _IL_DefaultEncoding_InternalGetByteCount_Stringii)
	IL_METHOD("InternalGetBytes", "([cii[Bi)i", _IL_DefaultEncoding_InternalGetBytes_aciiaBi)
	IL_METHOD("InternalGetBytes", "(oSystem.String;ii[Bi)i", _IL_DefaultEncoding_InternalGetBytes_StringiiaBi)
	IL_METHOD("InternalGetCharCount", "([Bii)i", _IL_DefaultEncoding_InternalGetCharCount)
	IL_METHOD("InternalGetChars", "([Bii[ci)i", _IL_DefaultEncoding_InternalGetChars)
	IL_METHOD("InternalGetMaxByteCount", "(i)i", _IL_DefaultEncoding_InternalGetMaxByteCount)
	IL_METHOD("InternalGetMaxCharCount", "(i)i", _IL_DefaultEncoding_InternalGetMaxCharCount)
	IL_METHOD("InternalGetString", "([Bii)oSystem.String;", _IL_DefaultEncoding_InternalGetString)
	IL_METHOD("InternalCodePage", "()i", _IL_DefaultEncoding_InternalCodePage)
IL_METHOD_END

IL_METHOD_BEGIN(ClrSecurity_Methods)
	IL_METHOD("Assert", "(oSystem.Security.CodeAccessPermission;i)Z", _IL_ClrSecurity_Assert)
	IL_METHOD("Demand", "(oSystem.Security.CodeAccessPermission;i)Z", _IL_ClrSecurity_Demand)
	IL_METHOD("Deny", "(oSystem.Security.CodeAccessPermission;i)V", _IL_ClrSecurity_Deny)
	IL_METHOD("SetPermitOnlyBlock", "(i)V", _IL_ClrSecurity_SetPermitOnlyBlock)
	IL_METHOD("PermitOnly", "(oSystem.Security.CodeAccessPermission;i)V", _IL_ClrSecurity_PermitOnly)
IL_METHOD_END

IL_METHOD_BEGIN(CodeAccessPermission_Methods)
	IL_METHOD("RevertAll", "()V", _IL_CodeAccessPermission_RevertAll)
	IL_METHOD("RevertAssert", "()V", _IL_CodeAccessPermission_RevertAssert)
	IL_METHOD("RevertDeny", "()V", _IL_CodeAccessPermission_RevertDeny)
	IL_METHOD("RevertPermitOnly", "()V", _IL_CodeAccessPermission_RevertPermitOnly)
IL_METHOD_END

IL_METHOD_BEGIN(GCHandle_Methods)
	IL_METHOD("GCAddrOfPinnedObject", "(i)j", _IL_GCHandle_GCAddrOfPinnedObject)
	IL_METHOD("GCAlloc", "(oSystem.Object;vSystem.Runtime.InteropServices.GCHandleType;)i", _IL_GCHandle_GCAlloc)
	IL_METHOD("GCFree", "(i)V", _IL_GCHandle_GCFree)
	IL_METHOD("GCValidate", "(i)Z", _IL_GCHandle_GCValidate)
	IL_METHOD("GCGetTarget", "(i)oSystem.Object;", _IL_GCHandle_GCGetTarget)
	IL_METHOD("GCSetTarget", "(ioSystem.Object;Z)V", _IL_GCHandle_GCSetTarget)
IL_METHOD_END

IL_METHOD_BEGIN(RuntimeHelpers_Methods)
	IL_METHOD("InitializeArray", "(oSystem.Array;vSystem.RuntimeFieldHandle;)V", _IL_RuntimeHelpers_InitializeArray)
	IL_METHOD("RunClassConstructor", "(vSystem.RuntimeTypeHandle;)V", _IL_RuntimeHelpers_RunClassConstructor)
	IL_METHOD("InternalOffsetToStringData", "()i", _IL_RuntimeHelpers_InternalOffsetToStringData)
	IL_METHOD("GetObjectValue", "(oSystem.Object;)oSystem.Object;", _IL_RuntimeHelpers_GetObjectValue)
IL_METHOD_END

IL_METHOD_BEGIN(Assembly_Methods)
	IL_METHOD("GetCallingAssembly", "()oSystem.Reflection.Assembly;", _IL_Assembly_GetCallingAssembly)
	IL_METHOD("GetExecutingAssembly", "()oSystem.Reflection.Assembly;", _IL_Assembly_GetExecutingAssembly)
	IL_METHOD("GetEntryAssembly", "()oSystem.Reflection.Assembly;", _IL_Assembly_GetEntryAssembly)
	IL_METHOD("GetExportedTypes", "(T)[oSystem.Type;", _IL_Assembly_GetExportedTypes)
	IL_METHOD("GetFile", "(ToSystem.String;)oSystem.IO.FileStream;", _IL_Assembly_GetFile)
	IL_METHOD("GetFiles", "(TZ)[oSystem.IO.FileStream;", _IL_Assembly_GetFiles)
	IL_METHOD("GetManifestResourceInfo", "(ToSystem.String;)oSystem.Reflection.ManifestResourceInfo;", _IL_Assembly_GetManifestResourceInfo)
	IL_METHOD("GetManifestResourceNames", "(T)[oSystem.String;", _IL_Assembly_GetManifestResourceNames)
	IL_METHOD("GetManifestResourceStream", "(ToSystem.String;)oSystem.IO.Stream;", _IL_Assembly_GetManifestResourceStream)
	IL_METHOD("GetType", "(ToSystem.String;ZZ)oSystem.Type;", _IL_Assembly_GetType)
	IL_METHOD("GetTypes", "(T)[oSystem.Type;", _IL_Assembly_GetTypes)
	IL_METHOD("LoadFromName", "(oSystem.String;&ioSystem.Reflection.Assembly;)oSystem.Reflection.Assembly;", _IL_Assembly_LoadFromName)
	IL_METHOD("LoadFromFile", "(oSystem.String;&ioSystem.Reflection.Assembly;)oSystem.Reflection.Assembly;", _IL_Assembly_LoadFromFile)
IL_METHOD_END

IL_METHOD_BEGIN(MethodBase_Methods)
	IL_METHOD("GetMethodFromHandle", "(vSystem.RuntimeMethodHandle;)oSystem.Reflection.MethodBase;", _IL_MethodBase_GetMethodFromHandle)
	IL_METHOD("GetCurrentMethod", "()oSystem.Reflection.MethodBase;", _IL_MethodBase_GetCurrentMethod)
IL_METHOD_END

IL_METHOD_BEGIN(ClrConstructor_Methods)
	IL_METHOD("Invoke", "(TvSystem.Reflection.BindingFlags;oSystem.Reflection.Binder;[oSystem.Object;oSystem.Globalization.CultureInfo;)oSystem.Object;", _IL_ClrConstructor_Invoke)
IL_METHOD_END

IL_METHOD_BEGIN(FieldInfo_Methods)
	IL_METHOD("GetFieldFromHandle", "(vSystem.RuntimeFieldHandle;)oSystem.Reflection.FieldInfo;", _IL_FieldInfo_GetFieldFromHandle)
IL_METHOD_END

IL_METHOD_BEGIN(ClrField_Methods)
	IL_METHOD("GetValue", "(ToSystem.Object;)oSystem.Object;", _IL_ClrField_GetValue)
	IL_METHOD("SetValue", "(ToSystem.Object;oSystem.Object;vSystem.Reflection.BindingFlags;oSystem.Reflection.Binder;oSystem.Globalization.CultureInfo;)V", _IL_ClrField_SetValue)
	IL_METHOD("GetFieldType", "(j)oSystem.Type;", _IL_ClrField_GetFieldType)
	IL_METHOD("GetValueDirect", "(Tr)oSystem.Object;", _IL_ClrField_GetValueDirect)
	IL_METHOD("SetValueDirect", "(TroSystem.Object;)V", _IL_ClrField_SetValueDirect)
IL_METHOD_END

IL_METHOD_BEGIN(ClrHelpers_Methods)
	IL_METHOD("GetCustomAttributes", "(jjZ)[oSystem.Object;", _IL_ClrHelpers_GetCustomAttributes)
	IL_METHOD("IsDefined", "(jjZ)Z", _IL_ClrHelpers_IsDefined)
	IL_METHOD("GetDeclaringType", "(j)j", _IL_ClrHelpers_GetDeclaringType)
	IL_METHOD("GetName", "(j)oSystem.String;", _IL_ClrHelpers_GetName)
	IL_METHOD("GetParameter", "(ji)j", _IL_ClrHelpers_GetParameter)
	IL_METHOD("GetParameterType", "(ji)oSystem.Type;", _IL_ClrHelpers_GetParameterType)
	IL_METHOD("GetNumParameters", "(j)i", _IL_ClrHelpers_GetNumParameters)
	IL_METHOD("GetMemberAttrs", "(j)i", _IL_ClrHelpers_GetMemberAttrs)
	IL_METHOD("GetCallConv", "(j)vSystem.Reflection.CallingConventions;", _IL_ClrHelpers_GetCallConv)
	IL_METHOD("GetImplAttrs", "(j)vSystem.Reflection.MethodImplAttributes;", _IL_ClrHelpers_GetImplAttrs)
	IL_METHOD("GetSemantics", "(jvSystem.Reflection.MethodSemanticsAttributes;Z)oSystem.Reflection.MethodInfo;", _IL_ClrHelpers_GetSemantics)
	IL_METHOD("HasSemantics", "(jvSystem.Reflection.MethodSemanticsAttributes;Z)Z", _IL_ClrHelpers_HasSemantics)
	IL_METHOD("CanAccess", "(j)Z", _IL_ClrHelpers_CanAccess)
IL_METHOD_END

IL_METHOD_BEGIN(ClrMethod_Methods)
	IL_METHOD("Invoke", "(ToSystem.Object;vSystem.Reflection.BindingFlags;oSystem.Reflection.Binder;[oSystem.Object;oSystem.Globalization.CultureInfo;)oSystem.Object;", _IL_ClrMethod_Invoke)
	IL_METHOD("GetBaseDefinition", "(T)oSystem.Reflection.MethodInfo;", _IL_ClrMethod_GetBaseDefinition)
IL_METHOD_END

IL_METHOD_BEGIN(ClrParameter_Methods)
	IL_METHOD("GetParamAttrs", "(j)vSystem.Reflection.ParameterAttributes;", _IL_ClrParameter_GetParamAttrs)
	IL_METHOD("GetParamName", "(j)oSystem.String;", _IL_ClrParameter_GetParamName)
	IL_METHOD("GetDefault", "(j)oSystem.Object;", _IL_ClrParameter_GetDefault)
IL_METHOD_END

IL_METHOD_BEGIN(ClrProperty_Methods)
	IL_METHOD("GetPropertyType", "(j)oSystem.Type;", _IL_ClrProperty_GetPropertyType)
IL_METHOD_END

IL_METHOD_BEGIN(ClrResourceStream_Methods)
	IL_METHOD("ResourceRead", "(jl[Bii)i", _IL_ClrResourceStream_ResourceRead)
	IL_METHOD("ResourceReadByte", "(jl)i", _IL_ClrResourceStream_ResourceReadByte)
	IL_METHOD("ResourceGetAddress", "(jl)*B", _IL_ClrResourceStream_ResourceGetAddress)
IL_METHOD_END

IL_METHOD_BEGIN(ClrType_Methods)
	IL_METHOD("GetClrArrayRank", "(T)i", _IL_ClrType_GetClrArrayRank)
	IL_METHOD("GetAttributeFlagsImpl", "(T)vSystem.Reflection.TypeAttributes;", _IL_ClrType_GetAttributeFlagsImpl)
	IL_METHOD("GetElementType", "(T)oSystem.Type;", _IL_ClrType_GetElementType)
	IL_METHOD("GetInterface", "(ToSystem.String;Z)oSystem.Type;", _IL_ClrType_GetInterface)
	IL_METHOD("GetInterfaces", "(T)[oSystem.Type;", _IL_ClrType_GetInterfaces)
	IL_METHOD("GetMemberImpl", "(ToSystem.String;vSystem.Reflection.MemberTypes;vSystem.Reflection.BindingFlags;oSystem.Reflection.Binder;vSystem.Reflection.CallingConventions;[oSystem.Type;[vSystem.Reflection.ParameterModifier;)oSystem.Reflection.MemberInfo;", _IL_ClrType_GetMemberImpl)
	IL_METHOD("GetMembersImpl", "(TvSystem.Reflection.MemberTypes;vSystem.Reflection.BindingFlags;oSystem.Type;oSystem.String;)oSystem.Object;", _IL_ClrType_GetMembersImpl)
	IL_METHOD("GetClrTypeCategory", "(T)vSystem.Reflection.ClrTypeCategory;", _IL_ClrType_GetClrTypeCategory)
	IL_METHOD("IsSubclassOf", "(ToSystem.Type;)Z", _IL_ClrType_IsSubclassOf)
	IL_METHOD("IsClrNestedType", "(T)Z", _IL_ClrType_IsClrNestedType)
	IL_METHOD("GetClrAssembly", "(T)oSystem.Reflection.Assembly;", _IL_ClrType_GetClrAssembly)
	IL_METHOD("GetClrBaseType", "(T)oSystem.Type;", _IL_ClrType_GetClrBaseType)
	IL_METHOD("GetClrFullName", "(T)oSystem.String;", _IL_ClrType_GetClrFullName)
	IL_METHOD("GetClrGUID", "(T)vSystem.Guid;", _IL_ClrType_GetClrGUID)
	IL_METHOD("GetClrModule", "(T)oSystem.Reflection.Module;", _IL_ClrType_GetClrModule)
	IL_METHOD("GetClrNestedDeclaringType", "(T)oSystem.Type;", _IL_ClrType_GetClrNestedDeclaringType)
	IL_METHOD("GetClrName", "(T)oSystem.String;", _IL_ClrType_GetClrName)
	IL_METHOD("GetClrNamespace", "(T)oSystem.String;", _IL_ClrType_GetClrNamespace)
IL_METHOD_END

IL_METHOD_BEGIN(Module_Methods)
	IL_METHOD("GetType", "(ToSystem.String;ZZ)oSystem.Type;", _IL_Module_GetType)
	IL_METHOD("GetTypes", "(T)[oSystem.Type;", _IL_Module_GetTypes)
	IL_METHOD("IsResource", "(T)Z", _IL_Module_IsResource)
	IL_METHOD("GetModuleType", "(T)oSystem.Type;", _IL_Module_GetModuleType)
	IL_METHOD("GetAssembly", "(T)oSystem.Reflection.Assembly;", _IL_Module_GetAssembly)
	IL_METHOD("GetFullName", "(T)oSystem.String;", _IL_Module_GetFullName)
IL_METHOD_END

IL_METHOD_BEGIN(Debugger_Methods)
	IL_METHOD("InternalIsAttached", "()Z", _IL_Debugger_InternalIsAttached)
	IL_METHOD("Break", "()V", _IL_Debugger_Break)
	IL_METHOD("IsLogging", "()Z", _IL_Debugger_IsLogging)
	IL_METHOD("InternalLaunch", "()Z", _IL_Debugger_InternalLaunch)
	IL_METHOD("Log", "(ioSystem.String;oSystem.String;)V", _IL_Debugger_Log)
IL_METHOD_END

IL_METHOD_BEGIN(StackFrame_Methods)
	IL_METHOD("InternalGetTotalFrames", "()i", _IL_StackFrame_InternalGetTotalFrames)
	IL_METHOD("InternalGetMethod", "(i)vSystem.RuntimeMethodHandle;", _IL_StackFrame_InternalGetMethod)
	IL_METHOD("InternalGetILOffset", "(i)i", _IL_StackFrame_InternalGetILOffset)
	IL_METHOD("InternalGetNativeOffset", "(i)i", _IL_StackFrame_InternalGetNativeOffset)
	IL_METHOD("InternalGetDebugInfo", "(vSystem.RuntimeMethodHandle;i&i&i)oSystem.String;", _IL_StackFrame_InternalGetDebugInfo)
	IL_METHOD("GetExceptionStackTrace", "()[vSystem.Diagnostics.PackedStackFrame;", _IL_StackFrame_GetExceptionStackTrace)
IL_METHOD_END

IL_METHOD_BEGIN(DirMethods_Methods)
	IL_METHOD("GetPathInfo", "()vPlatform.PathInfo;", _IL_DirMethods_GetPathInfo)
	IL_METHOD("GetSystemDirectory", "()oSystem.String;", _IL_DirMethods_GetSystemDirectory)
	IL_METHOD("GetLastAccess", "(oSystem.String;&l)vPlatform.Errno;", _IL_DirMethods_GetLastAccess)
	IL_METHOD("GetLastModification", "(oSystem.String;&l)vPlatform.Errno;", _IL_DirMethods_GetLastModification)
	IL_METHOD("GetCreationTime", "(oSystem.String;&l)vPlatform.Errno;", _IL_DirMethods_GetCreationTime)
	IL_METHOD("Copy", "(oSystem.String;oSystem.String;)vPlatform.Errno;", _IL_DirMethods_Copy)
	IL_METHOD("Delete", "(oSystem.String;)vPlatform.Errno;", _IL_DirMethods_Delete)
	IL_METHOD("Rename", "(oSystem.String;oSystem.String;)vPlatform.Errno;", _IL_DirMethods_Rename)
IL_METHOD_END

IL_METHOD_BEGIN(FileMethods_Methods)
	IL_METHOD("GetInvalidHandle", "()j", _IL_FileMethods_GetInvalidHandle)
	IL_METHOD("ValidatePathname", "(oSystem.String;)Z", _IL_FileMethods_ValidatePathname)
	IL_METHOD("Open", "(oSystem.String;vSystem.IO.FileMode;vSystem.IO.FileAccess;vSystem.IO.FileShare;&j)Z", _IL_FileMethods_Open)
	IL_METHOD("HasAsync", "()Z", _IL_FileMethods_HasAsync)
	IL_METHOD("CanSeek", "(j)Z", _IL_FileMethods_CanSeek)
	IL_METHOD("CheckHandleAccess", "(jvSystem.IO.FileAccess;)Z", _IL_FileMethods_CheckHandleAccess)
	IL_METHOD("Seek", "(jlvSystem.IO.SeekOrigin;)l", _IL_FileMethods_Seek)
	IL_METHOD("Write", "(j[Bii)Z", _IL_FileMethods_Write)
	IL_METHOD("Read", "(j[Bii)i", _IL_FileMethods_Read)
	IL_METHOD("Close", "(j)Z", _IL_FileMethods_Close)
	IL_METHOD("FlushWrite", "(j)Z", _IL_FileMethods_FlushWrite)
	IL_METHOD("SetLength", "(jl)Z", _IL_FileMethods_SetLength)
	IL_METHOD("GetErrno", "()vPlatform.Errno;", _IL_FileMethods_GetErrno)
	IL_METHOD("GetErrnoMessage", "(vPlatform.Errno;)oSystem.String;", _IL_FileMethods_GetErrnoMessage)
IL_METHOD_END

IL_METHOD_BEGIN(RuntimeSecurityManager_Methods)
	IL_METHOD("CanUseFileHandle", "(Tj)Z", _IL_RuntimeSecurityManager_CanUseFileHandle)
	IL_METHOD("CanOpenFile", "(ToSystem.String;vSystem.IO.FileMode;vSystem.IO.FileAccess;vSystem.IO.FileShare;)Z", _IL_RuntimeSecurityManager_CanOpenFile)
IL_METHOD_END

IL_METHOD_BEGIN(Security_Methods)
	IL_METHOD("GetSecurityManager", "()oPlatform.ISecurityManager;", _IL_Security_GetSecurityManager)
	IL_METHOD("SetSecurityManager", "(oPlatform.ISecurityManager;)V", _IL_Security_SetSecurityManager)
IL_METHOD_END

IL_METHOD_BEGIN(SocketMethods_Methods)
	IL_METHOD("Create", "(iii&j)Z", _IL_SocketMethods_Create)
	IL_METHOD("Bind", "(jili)Z", _IL_SocketMethods_Bind)
	IL_METHOD("Shutdown", "(ji)Z", _IL_SocketMethods_Shutdown)
	IL_METHOD("Listen", "(ji)Z", _IL_SocketMethods_Listen)
	IL_METHOD("Accept", "(j&l&i&j)Z", _IL_SocketMethods_Accept)
	IL_METHOD("Connect", "(jili)Z", _IL_SocketMethods_Connect)
	IL_METHOD("Receive", "(j[Biii)i", _IL_SocketMethods_Receive)
	IL_METHOD("ReceiveFrom", "(j[Biii&l&i)i", _IL_SocketMethods_ReceiveFrom)
	IL_METHOD("Send", "(j[Biii)i", _IL_SocketMethods_Send)
	IL_METHOD("SendTo", "(j[Biiili)i", _IL_SocketMethods_SendTo)
	IL_METHOD("Close", "(j)Z", _IL_SocketMethods_Close)
	IL_METHOD("Select", "([j[j[jl)i", _IL_SocketMethods_Select)
	IL_METHOD("GetErrno", "()vPlatform.Errno;", _IL_SocketMethods_GetErrno)
	IL_METHOD("GetErrnoMessage", "(vPlatform.Errno;)oSystem.String;", _IL_SocketMethods_GetErrnoMessage)
IL_METHOD_END

IL_METHOD_BEGIN(Stdio_Methods)
	IL_METHOD("StdClose", "(i)V", _IL_Stdio_StdClose)
	IL_METHOD("StdFlush", "(i)V", _IL_Stdio_StdFlush)
	IL_METHOD("StdWrite", "(ic)V", _IL_Stdio_StdWrite_ic)
	IL_METHOD("StdWrite", "(i[cii)V", _IL_Stdio_StdWrite_iacii)
	IL_METHOD("StdWrite", "(i[Bii)V", _IL_Stdio_StdWrite_iaBii)
	IL_METHOD("StdWrite", "(ioSystem.String;)V", _IL_Stdio_StdWrite_iString)
	IL_METHOD("StdRead", "(i)i", _IL_Stdio_StdRead_i)
	IL_METHOD("StdRead", "(i[cii)i", _IL_Stdio_StdRead_iacii)
	IL_METHOD("StdRead", "(i[Bii)i", _IL_Stdio_StdRead_iaBii)
	IL_METHOD("StdPeek", "(i)i", _IL_Stdio_StdPeek)
IL_METHOD_END

IL_METHOD_BEGIN(SysCharInfo_Methods)
	IL_METHOD("GetUnicodeCategory", "(c)vSystem.Globalization.UnicodeCategory;", _IL_SysCharInfo_GetUnicodeCategory)
	IL_METHOD("GetNumericValue", "(c)d", _IL_SysCharInfo_GetNumericValue)
	IL_METHOD("GetNewLine", "()oSystem.String;", _IL_SysCharInfo_GetNewLine)
IL_METHOD_END

IL_METHOD_BEGIN(TaskMethods_Methods)
	IL_METHOD("Exit", "(i)V", _IL_TaskMethods_Exit)
	IL_METHOD("SetExitCode", "(i)V", _IL_TaskMethods_SetExitCode)
	IL_METHOD("GetCommandLineArgs", "()[oSystem.String;", _IL_TaskMethods_GetCommandLineArgs)
	IL_METHOD("GetEnvironmentVariable", "(oSystem.String;)oSystem.String;", _IL_TaskMethods_GetEnvironmentVariable)
	IL_METHOD("GetEnvironmentCount", "()i", _IL_TaskMethods_GetEnvironmentCount)
	IL_METHOD("GetEnvironmentKey", "(i)oSystem.String;", _IL_TaskMethods_GetEnvironmentKey)
	IL_METHOD("GetEnvironmentValue", "(i)oSystem.String;", _IL_TaskMethods_GetEnvironmentValue)
IL_METHOD_END

IL_METHOD_BEGIN(TimeMethods_Methods)
	IL_METHOD("GetCurrentTime", "()l", _IL_TimeMethods_GetCurrentTime)
	IL_METHOD("GetCurrentUtcTime", "()l", _IL_TimeMethods_GetCurrentUtcTime)
	IL_METHOD("GetTimeZoneAdjust", "()i", _IL_TimeMethods_GetTimeZoneAdjust)
	IL_METHOD("GetUpTime", "()i", _IL_TimeMethods_GetUpTime)
IL_METHOD_END

IL_METHOD_BEGIN(CodeTable_Methods)
	IL_METHOD("GetAddress", "(oSystem.IO.Stream;l)*B", _IL_CodeTable_GetAddress)
IL_METHOD_END

typedef struct
{
	const char *name;
	const char *namespace;
	const ILMethodTableEntry *entry;

} InternalClassInfo;
static InternalClassInfo const internalClassTable[] = {
	{"ArgIterator", "System", ArgIterator_Methods},
	{"Array", "System", Array_Methods},
	{"Assembly", "System.Reflection", Assembly_Methods},
	{"BitConverter", "System", BitConverter_Methods},
	{"Buffer", "System", Buffer_Methods},
	{"ClrConstructor", "System.Reflection", ClrConstructor_Methods},
	{"ClrField", "System.Reflection", ClrField_Methods},
	{"ClrHelpers", "System.Reflection", ClrHelpers_Methods},
	{"ClrMethod", "System.Reflection", ClrMethod_Methods},
	{"ClrParameter", "System.Reflection", ClrParameter_Methods},
	{"ClrProperty", "System.Reflection", ClrProperty_Methods},
	{"ClrResourceStream", "System.Reflection", ClrResourceStream_Methods},
	{"ClrSecurity", "System.Security", ClrSecurity_Methods},
	{"ClrType", "System.Reflection", ClrType_Methods},
	{"CodeAccessPermission", "System.Security", CodeAccessPermission_Methods},
	{"CodeTable", "I18N.CJK", CodeTable_Methods},
	{"Debugger", "System.Diagnostics", Debugger_Methods},
	{"Decimal", "System", Decimal_Methods},
	{"DefaultEncoding", "System.Text", DefaultEncoding_Methods},
	{"Delegate", "System", Delegate_Methods},
	{"DirMethods", "Platform", DirMethods_Methods},
	{"Double", "System", Double_Methods},
	{"Enum", "System", Enum_Methods},
	{"FieldInfo", "System.Reflection", FieldInfo_Methods},
	{"FileMethods", "Platform", FileMethods_Methods},
	{"GC", "System", GC_Methods},
	{"GCHandle", "System.Runtime.InteropServices", GCHandle_Methods},
	{"Guid", "System", Guid_Methods},
	{"Interlocked", "System.Threading", Interlocked_Methods},
	{"Math", "System", Math_Methods},
	{"MethodBase", "System.Reflection", MethodBase_Methods},
	{"Module", "System.Reflection", Module_Methods},
	{"Monitor", "System.Threading", Monitor_Methods},
	{"Mutex", "System.Threading", Mutex_Methods},
	{"Object", "System", Object_Methods},
	{"RuntimeHelpers", "System.Runtime.CompilerServices", RuntimeHelpers_Methods},
	{"RuntimeMethodHandle", "System", RuntimeMethodHandle_Methods},
	{"RuntimeSecurityManager", "Platform", RuntimeSecurityManager_Methods},
	{"Security", "Platform", Security_Methods},
	{"Single", "System", Single_Methods},
	{"SocketMethods", "Platform", SocketMethods_Methods},
	{"StackFrame", "System.Diagnostics", StackFrame_Methods},
	{"Stdio", "Platform", Stdio_Methods},
	{"String", "System", String_Methods},
	{"SysCharInfo", "Platform", SysCharInfo_Methods},
	{"TaskMethods", "Platform", TaskMethods_Methods},
	{"Thread", "System.Threading", Thread_Methods},
	{"TimeMethods", "Platform", TimeMethods_Methods},
	{"Type", "System", Type_Methods},
	{"TypedReference", "System", TypedReference_Methods},
	{"WaitHandle", "System.Threading", WaitHandle_Methods},
};
#define numInternalClasses (sizeof(internalClassTable) / sizeof(InternalClassInfo))
