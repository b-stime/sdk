// Copyright (c) 2019, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#ifndef RUNTIME_VM_COMPILER_RECOGNIZED_METHODS_LIST_H_
#define RUNTIME_VM_COMPILER_RECOGNIZED_METHODS_LIST_H_

namespace dart {

// clang-format off
// (class-name, function-name, recognized enum, fingerprint).
// When adding a new function, add a 0 as the fingerprint and run the build in
// debug mode to get the correct fingerprint from the mismatch error.
#define OTHER_RECOGNIZED_LIST(V)                                               \
  V(::, identical, ObjectIdentical, 0x04168315)                                \
  V(ClassID, getID, ClassIDgetID, 0xbe1d6669)                                  \
  V(Object, Object., ObjectConstructor, 0xab6d6cfa)                            \
  V(List, ., ListFactory, 0xbc820cf9)                                          \
  V(_List, ., ObjectArrayAllocate, 0xd693eee6)                                 \
  V(_List, []=, ObjectArraySetIndexed, 0xac001598)                             \
  V(_GrowableList, []=, GrowableArraySetIndexed, 0xac001598)                   \
  V(_TypedList, _getInt8, ByteArrayBaseGetInt8, 0x1623dc34)                    \
  V(_TypedList, _getUint8, ByteArrayBaseGetUint8, 0x177ffe2a)                  \
  V(_TypedList, _getInt16, ByteArrayBaseGetInt16, 0x2e40964f)                  \
  V(_TypedList, _getUint16, ByteArrayBaseGetUint16, 0x2fc1f6b9)                \
  V(_TypedList, _getInt32, ByteArrayBaseGetInt32, 0x19182d0a)                  \
  V(_TypedList, _getUint32, ByteArrayBaseGetUint32, 0x195d6e7b)                \
  V(_TypedList, _getInt64, ByteArrayBaseGetInt64, 0xf660bfff)                  \
  V(_TypedList, _getUint64, ByteArrayBaseGetUint64, 0x2c5b7959)                \
  V(_TypedList, _getFloat32, ByteArrayBaseGetFloat32, 0xe8f6a107)              \
  V(_TypedList, _getFloat64, ByteArrayBaseGetFloat64, 0xf82a3634)              \
  V(_TypedList, _getFloat32x4, ByteArrayBaseGetFloat32x4, 0xaf2d0ce5)          \
  V(_TypedList, _getInt32x4, ByteArrayBaseGetInt32x4, 0x5573740b)              \
  V(_TypedList, _setInt8, ByteArrayBaseSetInt8, 0xe18943a2)                    \
  V(_TypedList, _setUint8, ByteArrayBaseSetUint8, 0xaf59b748)                  \
  V(_TypedList, _setInt16, ByteArrayBaseSetInt16, 0xbae64027)                  \
  V(_TypedList, _setUint16, ByteArrayBaseSetUint16, 0xce22484f)                \
  V(_TypedList, _setInt32, ByteArrayBaseSetInt32, 0xbddaab40)                  \
  V(_TypedList, _setUint32, ByteArrayBaseSetUint32, 0xb966a3b2)                \
  V(_TypedList, _setInt64, ByteArrayBaseSetInt64, 0xc8cd4f7a)                  \
  V(_TypedList, _setUint64, ByteArrayBaseSetUint64, 0xda473205)                \
  V(_TypedList, _setFloat32, ByteArrayBaseSetFloat32, 0x2f362de0)              \
  V(_TypedList, _setFloat64, ByteArrayBaseSetFloat64, 0x2359f8d2)              \
  V(_TypedList, _setFloat32x4, ByteArrayBaseSetFloat32x4, 0x38c6295a)          \
  V(_TypedList, _setInt32x4, ByteArrayBaseSetInt32x4, 0x5ce9025b)              \
  V(ByteData, ., ByteDataFactory, 0xd12ef748)                                  \
  V(_ByteDataView, get:offsetInBytes, ByteDataViewOffsetInBytes, 0x60cef22c)   \
  V(_ByteDataView, get:_typedData, ByteDataViewTypedData, 0xb9d15ffa)          \
  V(_TypedListView, get:offsetInBytes, TypedDataViewOffsetInBytes, 0x60cef22c) \
  V(_TypedListView, get:_typedData, TypedDataViewTypedData, 0xb9d15ffa)        \
  V(_ByteDataView, ._, TypedData_ByteDataView_factory, 0x3187137c)             \
  V(_Int8ArrayView, ._, TypedData_Int8ArrayView_factory, 0x445611ca)           \
  V(_Uint8ArrayView, ._, TypedData_Uint8ArrayView_factory, 0x96008895)         \
  V(_Uint8ClampedArrayView, ._, TypedData_Uint8ClampedArrayView_factory,       \
    0x05397869)                                                                \
  V(_Int16ArrayView, ._, TypedData_Int16ArrayView_factory, 0x490e13db)         \
  V(_Uint16ArrayView, ._, TypedData_Uint16ArrayView_factory, 0x9ff8c632)       \
  V(_Int32ArrayView, ._, TypedData_Int32ArrayView_factory, 0xe2e9aa79)         \
  V(_Uint32ArrayView, ._, TypedData_Uint32ArrayView_factory, 0x8682baa1)       \
  V(_Int64ArrayView, ._, TypedData_Int64ArrayView_factory, 0x12c74eaf)         \
  V(_Uint64ArrayView, ._, TypedData_Uint64ArrayView_factory, 0x25c66efd)       \
  V(_Float32ArrayView, ._, TypedData_Float32ArrayView_factory, 0xdc968c44)     \
  V(_Float64ArrayView, ._, TypedData_Float64ArrayView_factory, 0xcb765517)     \
  V(_Float32x4ArrayView, ._, TypedData_Float32x4ArrayView_factory, 0x665eaec0) \
  V(_Int32x4ArrayView, ._, TypedData_Int32x4ArrayView_factory, 0x04b05d05)     \
  V(_Float64x2ArrayView, ._, TypedData_Float64x2ArrayView_factory, 0x42e25ba4) \
  V(Int8List, ., TypedData_Int8Array_factory, 0x660dd888)                      \
  V(Uint8List, ., TypedData_Uint8Array_factory, 0xede3f64f)                    \
  V(Uint8ClampedList, ., TypedData_Uint8ClampedArray_factory, 0x28063755)      \
  V(Int16List, ., TypedData_Int16Array_factory, 0xd0cd98f3)                    \
  V(Uint16List, ., TypedData_Uint16Array_factory, 0x3cb5fb6a)                  \
  V(Int32List, ., TypedData_Int32Array_factory, 0x1b8ff320)                    \
  V(Uint32List, ., TypedData_Uint32Array_factory, 0x2b2f9a8b)                  \
  V(Int64List, ., TypedData_Int64Array_factory, 0xfb71de2f)                    \
  V(Uint64List, ., TypedData_Uint64Array_factory, 0xe3cfcff8)                  \
  V(Float32List, ., TypedData_Float32Array_factory, 0xa39068fe)                \
  V(Float64List, ., TypedData_Float64Array_factory, 0xa0c64e91)                \
  V(Float32x4List, ., TypedData_Float32x4Array_factory, 0x0a7d7b88)            \
  V(Int32x4List, ., TypedData_Int32x4Array_factory, 0x5a17b46e)                \
  V(Float64x2List, ., TypedData_Float64x2Array_factory, 0xeccaff6a)            \
  V(::, _toClampedUint8, ConvertIntToClampedUint8, 0x6f65a4d0)                 \
  V(::, copyRangeFromUint8ListToOneByteString,                                 \
    CopyRangeFromUint8ListToOneByteString, 0x8302f9d9)                         \
  V(_StringBase, _interpolate, StringBaseInterpolate, 0xec019c89)              \
  V(_IntegerImplementation, toDouble, IntegerToDouble, 0x97728b46)             \
  V(_Double, _add, DoubleAdd, 0xea666327)                                      \
  V(_Double, _sub, DoubleSub, 0x28474c2e)                                      \
  V(_Double, _mul, DoubleMul, 0x1f98c76c)                                      \
  V(_Double, _div, DoubleDiv, 0x287d3791)                                      \
  V(::, min, MathMin, 0x1cee3d43)                                              \
  V(::, max, MathMax, 0xbf7a293e)                                              \
  V(::, _doublePow, MathDoublePow, 0x701a00c4)                                 \
  V(::, _intPow, MathIntPow, 0x9e2bb83a)                                       \
  V(Float32x4, _Float32x4FromDoubles, Float32x4FromDoubles, 0x1845792b)        \
  V(Float32x4, Float32x4.zero, Float32x4Zero, 0xd3b64002)                      \
  V(Float32x4, _Float32x4Splat, Float32x4Splat, 0x13a552c3)                    \
  V(Float32x4, Float32x4.fromInt32x4Bits, Int32x4ToFloat32x4, 0x7ed59542)      \
  V(Float32x4, Float32x4.fromFloat64x2, Float64x2ToFloat32x4, 0x50be8d8d)      \
  V(_Float32x4, shuffle, Float32x4Shuffle, 0xa7f1b7eb)                         \
  V(_Float32x4, shuffleMix, Float32x4ShuffleMix, 0x79a0c2cc)                   \
  V(_Float32x4, get:signMask, Float32x4GetSignMask, 0x7c6b11ea)                \
  V(_Float32x4, equal, Float32x4Equal, 0x445aed76)                             \
  V(_Float32x4, greaterThan, Float32x4GreaterThan, 0x524d7d7f)                 \
  V(_Float32x4, greaterThanOrEqual, Float32x4GreaterThanOrEqual, 0x4e6e61b7)   \
  V(_Float32x4, lessThan, Float32x4LessThan, 0x4a18711d)                       \
  V(_Float32x4, lessThanOrEqual, Float32x4LessThanOrEqual, 0x46775340)         \
  V(_Float32x4, notEqual, Float32x4NotEqual, 0x644f3543)                       \
  V(_Float32x4, min, Float32x4Min, 0xe41e9e92)                                 \
  V(_Float32x4, max, Float32x4Max, 0xc64e2063)                                 \
  V(_Float32x4, scale, Float32x4Scale, 0xa3b74802)                             \
  V(_Float32x4, sqrt, Float32x4Sqrt, 0xe4f6fab2)                               \
  V(_Float32x4, reciprocalSqrt, Float32x4ReciprocalSqrt, 0xddd7f238)           \
  V(_Float32x4, reciprocal, Float32x4Reciprocal, 0xd4522272)                   \
  V(_Float32x4, unary-, Float32x4Negate, 0xe6abc412)                           \
  V(_Float32x4, abs, Float32x4Abs, 0xeb467e48)                                 \
  V(_Float32x4, clamp, Float32x4Clamp, 0x77cd71dd)                             \
  V(_Float32x4, _withX, Float32x4WithX, 0xa3179faf)                            \
  V(_Float32x4, _withY, Float32x4WithY, 0x9bb9b443)                            \
  V(_Float32x4, _withZ, Float32x4WithZ, 0x97d62ea0)                            \
  V(_Float32x4, _withW, Float32x4WithW, 0x9533225b)                            \
  V(Float64x2, _Float64x2FromDoubles, Float64x2FromDoubles, 0xd858e051)        \
  V(Float64x2, Float64x2.zero, Float64x2Zero, 0x82948918)                      \
  V(Float64x2, _Float64x2Splat, Float64x2Splat, 0x5b136bc4)                    \
  V(Float64x2, Float64x2.fromFloat32x4, Float32x4ToFloat64x2, 0x6ea79c66)      \
  V(_Float64x2, get:x, Float64x2GetX, 0x3a398530)                              \
  V(_Float64x2, get:y, Float64x2GetY, 0x27cae053)                              \
  V(_Float64x2, unary-, Float64x2Negate, 0x958a0d28)                           \
  V(_Float64x2, abs, Float64x2Abs, 0x9a24c75e)                                 \
  V(_Float64x2, sqrt, Float64x2Sqrt, 0x93d543c8)                               \
  V(_Float64x2, get:signMask, Float64x2GetSignMask, 0x7c6b11ea)                \
  V(_Float64x2, scale, Float64x2Scale, 0x52959118)                             \
  V(_Float64x2, _withX, Float64x2WithX, 0x51f5e8c5)                            \
  V(_Float64x2, _withY, Float64x2WithY, 0x4a97fd59)                            \
  V(_Float64x2, min, Float64x2Min,  0x362edc52)                                \
  V(_Float64x2, max, Float64x2Max,  0x185e5e23)                                \
  V(Int32x4, _Int32x4FromInts, Int32x4FromInts, 0xa900bd30)                    \
  V(Int32x4, _Int32x4FromBools, Int32x4FromBools, 0xf56c8fc8)                  \
  V(Int32x4, Int32x4.fromFloat32x4Bits, Float32x4ToInt32x4, 0x45727561)        \
  V(_Int32x4, get:flagX, Int32x4GetFlagX, 0xc29f03d8)                          \
  V(_Int32x4, get:flagY, Int32x4GetFlagY, 0xde0f3ab8)                          \
  V(_Int32x4, get:flagZ, Int32x4GetFlagZ, 0xebb8d60b)                          \
  V(_Int32x4, get:flagW, Int32x4GetFlagW, 0xf4d8e84c)                          \
  V(_Int32x4, get:signMask, Int32x4GetSignMask, 0x7c6b11ea)                    \
  V(_Int32x4, shuffle, Int32x4Shuffle, 0x406211d3)                             \
  V(_Int32x4, shuffleMix, Int32x4ShuffleMix, 0x4fe8349c)                       \
  V(_Int32x4, select, Int32x4Select, 0x68ca9fa0)                               \
  V(_Int32x4, _withFlagX, Int32x4WithFlagX, 0xb7df0063)                        \
  V(_Int32x4, _withFlagY, Int32x4WithFlagY, 0xa8cf9ba6)                        \
  V(_Int32x4, _withFlagZ, Int32x4WithFlagZ, 0xa8058854)                        \
  V(_Int32x4, _withFlagW, Int32x4WithFlagW, 0xb333f958)                        \
  V(_HashVMBase, get:_index, LinkedHashMap_getIndex, 0x882671dc)               \
  V(_HashVMBase, set:_index, LinkedHashMap_setIndex, 0xa2be9418)               \
  V(_HashVMBase, get:_data, LinkedHashMap_getData, 0x780e14ad)                 \
  V(_HashVMBase, set:_data, LinkedHashMap_setData, 0xb6a5c369)                 \
  V(_HashVMBase, get:_usedData, LinkedHashMap_getUsedData, 0x470893ed)         \
  V(_HashVMBase, set:_usedData, LinkedHashMap_setUsedData, 0xb3c887a9)         \
  V(_HashVMBase, get:_hashMask, LinkedHashMap_getHashMask, 0x4f0ec79c)         \
  V(_HashVMBase, set:_hashMask, LinkedHashMap_setHashMask, 0xbbcebb58)         \
  V(_HashVMBase, get:_deletedKeys, LinkedHashMap_getDeletedKeys, 0x510dc4a0)   \
  V(_HashVMBase, set:_deletedKeys, LinkedHashMap_setDeletedKeys, 0xbdcdb85c)   \
  V(_WeakProperty, get:key, WeakProperty_getKey, 0xde00e462)                   \
  V(_WeakProperty, set:key, WeakProperty_setKey, 0x963a095f)                   \
  V(_WeakProperty, get:value, WeakProperty_getValue, 0xd2f28aae)               \
  V(_WeakProperty, set:value, WeakProperty_setValue, 0x8b2bafab)               \
  V(::, _classRangeCheck, ClassRangeCheck, 0x5fd51e68)                         \
  V(::, _abi, FfiAbi, 0x7c4ab775)                                              \
  V(::, _asFunctionInternal, FfiAsFunctionInternal, 0xbbcb235a)                \
  V(::, _nativeCallbackFunction, FfiNativeCallbackFunction, 0x3ff5ae9c)        \
  V(::, _nativeEffect, NativeEffect, 0x61e00b59)                               \
  V(::, _loadInt8, FfiLoadInt8, 0x0f04dfd6)                                    \
  V(::, _loadInt16, FfiLoadInt16, 0xec44312d)                                  \
  V(::, _loadInt32, FfiLoadInt32, 0xee223fc3)                                  \
  V(::, _loadInt64, FfiLoadInt64, 0xdeefbfa3)                                  \
  V(::, _loadUint8, FfiLoadUint8, 0xe14e1cd1)                                  \
  V(::, _loadUint16, FfiLoadUint16, 0x0cd65cea)                                \
  V(::, _loadUint32, FfiLoadUint32, 0xf66e9055)                                \
  V(::, _loadUint64, FfiLoadUint64, 0x0505fdcc)                                \
  V(::, _loadIntPtr, FfiLoadIntPtr, 0xebd9b43e)                                \
  V(::, _loadFloat, FfiLoadFloat, 0xf8d9845d)                                  \
  V(::, _loadFloatUnaligned, FfiLoadFloatUnaligned, 0xc8c8dfff)                \
  V(::, _loadDouble, FfiLoadDouble, 0xf70cc619)                                \
  V(::, _loadDoubleUnaligned, FfiLoadDoubleUnaligned, 0xc99ebd39)              \
  V(::, _loadPointer, FfiLoadPointer, 0x4e79d0fc)                              \
  V(::, _storeInt8, FfiStoreInt8, 0xdf50af0c)                                  \
  V(::, _storeInt16, FfiStoreInt16, 0xd84df332)                                \
  V(::, _storeInt32, FfiStoreInt32, 0xfbe62c5d)                                \
  V(::, _storeInt64, FfiStoreInt64, 0xf1d40d7a)                                \
  V(::, _storeUint8, FfiStoreUint8, 0x056dd2f6)                                \
  V(::, _storeUint16, FfiStoreUint16, 0xe2fdaade)                              \
  V(::, _storeUint32, FfiStoreUint32, 0xe5d7e8c5)                              \
  V(::, _storeUint64, FfiStoreUint64, 0xe2d93239)                              \
  V(::, _storeIntPtr, FfiStoreIntPtr, 0x080266a8)                              \
  V(::, _storeFloat, FfiStoreFloat, 0x6484f07e)                                \
  V(::, _storeFloatUnaligned, FfiStoreFloatUnaligned, 0x600a9203)              \
  V(::, _storeDouble, FfiStoreDouble, 0x42998c64)                              \
  V(::, _storeDoubleUnaligned, FfiStoreDoubleUnaligned, 0x3dced75b)            \
  V(::, _storePointer, FfiStorePointer, 0xea6b7751)                            \
  V(::, _fromAddress, FfiFromAddress, 0xfd8cb1cc)                              \
  V(Pointer, get:address, FfiGetAddress, 0x7cde87be)                           \
  V(::, reachabilityFence, ReachabilityFence, 0x619235c1)                      \
  V(_Utf8Decoder, _scan, Utf8DecoderScan, 0x4983e111)                          \
  V(_Future, timeout, FutureTimeout, 0xc83eaf79)                               \
  V(Future, wait, FutureWait, 0xc71e731d)                                      \
  V(_RootZone, runUnary, RootZoneRunUnary, 0x966a802c)                         \
  V(_FutureListener, handleValue, FutureListenerHandleValue, 0x165b47c4)       \

// List of intrinsics:
// (class-name, function-name, intrinsification method, fingerprint).
#define CORE_LIB_INTRINSIC_LIST(V)                                             \
  V(_Smi, get:bitLength, Smi_bitLength, 0x7ab50ceb)                            \
  V(_BigIntImpl, _lsh, Bigint_lsh, 0x3f8b105e)                                 \
  V(_BigIntImpl, _rsh, Bigint_rsh, 0x117ed3f3)                                 \
  V(_BigIntImpl, _absAdd, Bigint_absAdd, 0xd55235d1)                           \
  V(_BigIntImpl, _absSub, Bigint_absSub, 0xe4a9dacf)                           \
  V(_BigIntImpl, _mulAdd, Bigint_mulAdd, 0x6dab2009)                           \
  V(_BigIntImpl, _sqrAdd, Bigint_sqrAdd, 0x4ea2b411)                           \
  V(_BigIntImpl, _estimateQuotientDigit, Bigint_estimateQuotientDigit,         \
    0x898ea14c)                                                                \
  V(_BigIntMontgomeryReduction, _mulMod, Montgomery_mulMod, 0x0d038dd0)        \
  V(_Double, >, Double_greaterThan, 0xe450adaf)                                \
  V(_Double, >=, Double_greaterEqualThan, 0xbc280c13)                          \
  V(_Double, <, Double_lessThan, 0x39643178)                                   \
  V(_Double, <=, Double_lessEqualThan, 0x73d2a9f5)                             \
  V(_Double, ==, Double_equal, 0xaca03d47)                                     \
  V(_Double, +, Double_add, 0x11250707)                                        \
  V(_Double, -, Double_sub, 0x04121358)                                        \
  V(_Double, *, Double_mul, 0x45991d55)                                        \
  V(_Double, /, Double_div, 0x3bc7ab91)                                        \
  V(_Double, get:hashCode, Double_hashCode, 0x75e0d093)                        \
  V(_Double, get:_identityHashCode, Double_identityHash, 0x47a56551)           \
  V(_Double, get:isNaN, Double_getIsNaN, 0xd4890713)                           \
  V(_Double, get:isInfinite, Double_getIsInfinite, 0xc4facbd2)                 \
  V(_Double, get:isNegative, Double_getIsNegative, 0xd4715091)                 \
  V(_Double, _mulFromInteger, Double_mulFromInteger, 0xf9e516ab)               \
  V(_Double, .fromInteger, DoubleFromInteger, 0x7d0fd999)                      \
  V(_GrowableList, ._withData, GrowableArray_Allocate, 0xa32d060b)             \
  V(_RegExp, _ExecuteMatch, RegExp_ExecuteMatch, 0x9911d549)                   \
  V(_RegExp, _ExecuteMatchSticky, RegExp_ExecuteMatchSticky, 0x91dd880f)       \
  V(Object, ==, ObjectEquals, 0x46587030)                                      \
  V(Object, get:runtimeType, ObjectRuntimeType, 0x0381c851)                    \
  V(Object, _haveSameRuntimeType, ObjectHaveSameRuntimeType, 0xce4e6295)       \
  V(_StringBase, get:hashCode, String_getHashCode, 0x75e0d454)                 \
  V(_StringBase, get:_identityHashCode, String_identityHash, 0x47a56912)       \
  V(_StringBase, get:isEmpty, StringBaseIsEmpty, 0x9ce63f77)                   \
  V(_StringBase, _substringMatches, StringBaseSubstringMatches, 0x03fdc6ce)    \
  V(_StringBase, [], StringBaseCharAt, 0xd06fc6bf)                             \
  V(_OneByteString, get:hashCode, OneByteString_getHashCode, 0x75e0d454)       \
  V(_OneByteString, _substringUncheckedNative,                                 \
    OneByteString_substringUnchecked,  0x9b18195e)                             \
  V(_OneByteString, ==, OneByteString_equality, 0xb50039a8)                    \
  V(_TwoByteString, ==, TwoByteString_equality, 0xb50039a8)                    \
  V(_Type, get:hashCode, Type_getHashCode, 0x75e0d454)                         \
  V(_Type, ==, Type_equality, 0x465868ae)                                      \
  V(_FunctionType, get:hashCode, FunctionType_getHashCode, 0x75e0d454)         \
  V(_FunctionType, ==, FunctionType_equality, 0x465868ae)                      \
  V(::, _getHash, Object_getHash, 0xc60ff758)                                  \
  V(::, _setHash, Object_setHash, 0x8f2a5b0b)                                  \

#define CORE_INTEGER_LIB_INTRINSIC_LIST(V)                                     \
  V(_IntegerImplementation, >, Integer_greaterThan, 0x402b12df)                \
  V(_IntegerImplementation, ==, Integer_equal, 0x509c9146)                     \
  V(_IntegerImplementation, _equalToInteger, Integer_equalToInteger,           \
    0x710f18c2)                                                                \
  V(_IntegerImplementation, <, Integer_lessThan, 0x39643178)                   \
  V(_IntegerImplementation, <=, Integer_lessEqualThan, 0x73d2a9f5)             \
  V(_IntegerImplementation, >=, Integer_greaterEqualThan, 0xbc280c13)          \
  V(_IntegerImplementation, <<, Integer_shl, 0x766f00e5)                       \
  V(_Double, toInt, DoubleToInteger, 0x676f1ce8)                               \

#define MATH_LIB_INTRINSIC_LIST(V)                                             \
  V(::, sqrt, MathSqrt, 0x58c2a87e)                                            \
  V(_Random, _nextState, Random_nextState, 0x7207677d)                         \

#define GRAPH_MATH_LIB_INTRINSIC_LIST(V)                                       \
  V(::, sin, MathSin, 0xf3c67be5)                                              \
  V(::, cos, MathCos, 0xbdcdaf99)                                              \
  V(::, tan, MathTan, 0xa0e4e2cf)                                              \
  V(::, asin, MathAsin, 0xb94f82b0)                                            \
  V(::, acos, MathAcos, 0xffb03167)                                            \
  V(::, atan, MathAtan, 0xf1ecb41a)                                            \
  V(::, atan2, MathAtan2, 0xff585505)                                          \
  V(::, exp, MathExp, 0xbb87ac43)                                              \
  V(::, log, MathLog, 0x27ec861f)                                              \

#define GRAPH_TYPED_DATA_INTRINSICS_LIST(V)                                    \
  V(_Int8List, [], Int8ArrayGetIndexed, 0x281e2e42)                            \
  V(_Int8List, []=, Int8ArraySetIndexed, 0x7eb45f63)                           \
  V(_Uint8List, [], Uint8ArrayGetIndexed, 0x8d96b202)                          \
  V(_Uint8List, []=, Uint8ArraySetIndexed, 0xcb436b27)                         \
  V(_ExternalUint8Array, [], ExternalUint8ArrayGetIndexed, 0x8d96b202)         \
  V(_ExternalUint8Array, []=, ExternalUint8ArraySetIndexed, 0xcb436b27)        \
  V(_Uint8ClampedList, [], Uint8ClampedArrayGetIndexed, 0x8d96b202)            \
  V(_Uint8ClampedList, []=, Uint8ClampedArraySetIndexed, 0xc14320c7)           \
  V(_ExternalUint8ClampedArray, [], ExternalUint8ClampedArrayGetIndexed,       \
    0x8d96b202)                                                                \
  V(_ExternalUint8ClampedArray, []=, ExternalUint8ClampedArraySetIndexed,      \
    0xc14320c7)                                                                \
  V(_Int16List, [], Int16ArrayGetIndexed, 0x081c8da2)                          \
  V(_Int16List, []=, Int16ArraySetIndexed, 0x0f3422ee)                         \
  V(_Uint16List, [], Uint16ArrayGetIndexed, 0xebf569a2)                        \
  V(_Uint16List, []=, Uint16ArraySetIndexed, 0xf776ca65)                       \
  V(_Int32List, [], Int32ArrayGetIndexed, 0x09ba3341)                          \
  V(_Int32List, []=, Int32ArraySetIndexed, 0xed689f8d)                         \
  V(_Uint32List, [], Uint32ArrayGetIndexed, 0x590c98e1)                        \
  V(_Uint32List, []=, Uint32ArraySetIndexed, 0xd90c140d)                       \
  V(_Int64List, [], Int64ArrayGetIndexed, 0x42828d81)                          \
  V(_Int64List, []=, Int64ArraySetIndexed, 0xd7ba387b)                         \
  V(_Uint64List, [], Uint64ArrayGetIndexed, 0x7f46f301)                        \
  V(_Uint64List, []=, Uint64ArraySetIndexed, 0xa31ba9b3)                       \
  V(_Float64List, [], Float64ArrayGetIndexed, 0x5fa1c248)                      \
  V(_Float64List, []=, Float64ArraySetIndexed, 0x6ad3ba59)                     \
  V(_Float32List, [], Float32ArrayGetIndexed, 0x359fc368)                      \
  V(_Float32List, []=, Float32ArraySetIndexed, 0xc6221787)                     \
  V(_Float32x4List, [], Float32x4ArrayGetIndexed, 0xdf87c0c1)                  \
  V(_Float32x4List, []=, Float32x4ArraySetIndexed, 0x889551f4)                 \
  V(_Int32x4List, [], Int32x4ArrayGetIndexed, 0x75703b39)                      \
  V(_Int32x4List, []=, Int32x4ArraySetIndexed, 0x952ee084)                     \
  V(_Float64x2List, [], Float64x2ArrayGetIndexed, 0xdc2fab6b)                  \
  V(_Float64x2List, []=, Float64x2ArraySetIndexed, 0x0884bf1c)                 \
  V(_TypedList, get:length, TypedListLength, 0x5850f06b)                       \
  V(_TypedListView, get:length, TypedListViewLength, 0x5850f06b)               \
  V(_ByteDataView, get:length, ByteDataViewLength, 0x5850f06b)                 \
  V(_Float32x4, get:x, Float32x4ShuffleX, 0x3a398530)                          \
  V(_Float32x4, get:y, Float32x4ShuffleY, 0x27cae053)                          \
  V(_Float32x4, get:z, Float32x4ShuffleZ, 0x5d964be9)                          \
  V(_Float32x4, get:w, Float32x4ShuffleW, 0x3fd6906b)                          \
  V(_Float32x4, *, Float32x4Mul, 0xe5507c87)                                   \
  V(_Float32x4, /, Float32x4Div, 0xc09f2f62)                                   \
  V(_Float32x4, -, Float32x4Sub, 0xdd326c4a)                                   \
  V(_Float32x4, +, Float32x4Add, 0xb7f9a1d9)                                   \
  V(_Float64x2, *, Float64x2Mul, 0x3760b686)                                   \
  V(_Float64x2, /, Float64x2Div, 0x12af6d22)                                   \
  V(_Float64x2, -, Float64x2Sub, 0x2f42a649)                                   \
  V(_Float64x2, +, Float64x2Add, 0x0a09dbd8)                                   \

#define GRAPH_CORE_INTRINSICS_LIST(V)                                          \
  V(_List, get:length, ObjectArrayLength, 0x5850f06b)                          \
  V(_List, [], ObjectArrayGetIndexed, 0x57b029cf)                              \
  V(_List, _setIndexed, ObjectArraySetIndexedUnchecked, 0x02f293ae)            \
  V(_ImmutableList, get:length, ImmutableArrayLength, 0x5850f06b)              \
  V(_ImmutableList, [], ImmutableArrayGetIndexed, 0x57b029cf)                  \
  V(_GrowableList, get:length, GrowableArrayLength, 0x5850f06b)                \
  V(_GrowableList, get:_capacity, GrowableArrayCapacity, 0x7d9f9bf2)           \
  V(_GrowableList, _setData, GrowableArraySetData, 0xbdda401b)                 \
  V(_GrowableList, _setLength, GrowableArraySetLength, 0xcc1bf9b6)             \
  V(_GrowableList, [], GrowableArrayGetIndexed, 0x57b029cf)                    \
  V(_GrowableList, _setIndexed, GrowableArraySetIndexedUnchecked, 0xfb40ee4f)  \
  V(_StringBase, get:length, StringBaseLength, 0x5850f06b)                     \
  V(_OneByteString, codeUnitAt, OneByteStringCodeUnitAt, 0x17f90910)           \
  V(_TwoByteString, codeUnitAt, TwoByteStringCodeUnitAt, 0x17f90910)           \
  V(_ExternalOneByteString, codeUnitAt, ExternalOneByteStringCodeUnitAt,       \
    0x17f90910)                                                                \
  V(_ExternalTwoByteString, codeUnitAt, ExternalTwoByteStringCodeUnitAt,       \
    0x17f90910)                                                                \
  V(_Smi, ~, Smi_bitNegate, 0x8254f51b)                                        \
  V(_IntegerImplementation, +, Integer_add, 0xd561008f)                        \
  V(_IntegerImplementation, -, Integer_sub, 0xc96a0f80)                        \
  V(_IntegerImplementation, *, Integer_mul, 0xacd9641d)                        \
  V(_IntegerImplementation, %, Integer_mod, 0xfcf7cc13)                        \
  V(_IntegerImplementation, ~/, Integer_truncDivide, 0xdda49e7f)               \
  V(_IntegerImplementation, unary-, Integer_negate, 0xf7a9a696)                \
  V(_IntegerImplementation, &, Integer_bitAnd, 0x8b9d7c33)                     \
  V(_IntegerImplementation, |, Integer_bitOr, 0x8f47f5eb)                      \
  V(_IntegerImplementation, ^, Integer_bitXor, 0xd838bef2)                     \
  V(_IntegerImplementation, >>, Integer_sar, 0x931fbb8a)                       \
  V(_IntegerImplementation, >>>, Integer_shr, 0x7495f7ec)                      \
  V(_Double, unary-, DoubleFlipSignBit, 0x3d39082b)                            \
  V(_Double, truncateToDouble, DoubleTruncate, 0x62d48298)                     \
  V(_Double, roundToDouble, DoubleRound, 0x5649c63f)                           \
  V(_Double, floorToDouble, DoubleFloor, 0x54b4c787)                           \
  V(_Double, ceilToDouble, DoubleCeil, 0x5f1bcb18)                             \
  V(_Double, _modulo, DoubleMod, 0xfdb3942e)

#define GRAPH_INTRINSICS_LIST(V)                                               \
  GRAPH_CORE_INTRINSICS_LIST(V)                                                \
  GRAPH_TYPED_DATA_INTRINSICS_LIST(V)                                          \
  GRAPH_MATH_LIB_INTRINSIC_LIST(V)                                             \

#define DEVELOPER_LIB_INTRINSIC_LIST(V)                                        \
  V(_UserTag, makeCurrent, UserTag_makeCurrent, 0x5bd9b88e)                    \
  V(::, _getDefaultTag, UserTag_defaultTag, 0x6c19c8a5)                        \
  V(::, _getCurrentTag, Profiler_getCurrentTag, 0x70ead08e)                    \
  V(::, _isDartStreamEnabled, Timeline_isDartStreamEnabled, 0xc97aafb3)        \

#define INTERNAL_LIB_INTRINSIC_LIST(V)                                         \
  V(::, allocateOneByteString, AllocateOneByteString, 0x9e774214)              \
  V(::, allocateTwoByteString, AllocateTwoByteString, 0xa63c7db1)              \
  V(::, writeIntoOneByteString, WriteIntoOneByteString, 0xd8729161)            \
  V(::, writeIntoTwoByteString, WriteIntoTwoByteString, 0xcfc7982a)            \

#define ALL_INTRINSICS_NO_INTEGER_LIB_LIST(V)                                  \
  CORE_LIB_INTRINSIC_LIST(V)                                                   \
  DEVELOPER_LIB_INTRINSIC_LIST(V)                                              \
  INTERNAL_LIB_INTRINSIC_LIST(V)                                               \
  MATH_LIB_INTRINSIC_LIST(V)                                                   \

#define ALL_INTRINSICS_LIST(V)                                                 \
  ALL_INTRINSICS_NO_INTEGER_LIB_LIST(V)                                        \
  CORE_INTEGER_LIB_INTRINSIC_LIST(V)

#define RECOGNIZED_LIST(V)                                                     \
  OTHER_RECOGNIZED_LIST(V)                                                     \
  ALL_INTRINSICS_LIST(V)                                                       \
  GRAPH_INTRINSICS_LIST(V)

// A list of core functions that internally dispatch based on received id.
#define POLYMORPHIC_TARGET_LIST(V)                                             \
  V(_StringBase, [], StringBaseCharAt, 0xd06fc6bf)                             \
  V(_TypedList, _getInt8, ByteArrayBaseGetInt8, 0x1623dc34)                    \
  V(_TypedList, _getUint8, ByteArrayBaseGetUint8, 0x177ffe2a)                  \
  V(_TypedList, _getInt16, ByteArrayBaseGetInt16, 0x2e40964f)                  \
  V(_TypedList, _getUint16, ByteArrayBaseGetUint16, 0x2fc1f6b9)                \
  V(_TypedList, _getInt32, ByteArrayBaseGetInt32, 0x19182d0a)                  \
  V(_TypedList, _getUint32, ByteArrayBaseGetUint32, 0x195d6e7b)                \
  V(_TypedList, _getInt64, ByteArrayBaseGetInt64, 0xf660bfff)                  \
  V(_TypedList, _getUint64, ByteArrayBaseGetUint64, 0x2c5b7959)                \
  V(_TypedList, _getFloat32, ByteArrayBaseGetFloat32, 0xe8f6a107)              \
  V(_TypedList, _getFloat64, ByteArrayBaseGetFloat64, 0xf82a3634)              \
  V(_TypedList, _getFloat32x4, ByteArrayBaseGetFloat32x4, 0xaf2d0ce5)          \
  V(_TypedList, _getInt32x4, ByteArrayBaseGetInt32x4, 0x5573740b)              \
  V(_TypedList, _setInt8, ByteArrayBaseSetInt8, 0xe18943a2)                    \
  V(_TypedList, _setUint8, ByteArrayBaseSetInt8, 0xaf59b748)                   \
  V(_TypedList, _setInt16, ByteArrayBaseSetInt16, 0xbae64027)                  \
  V(_TypedList, _setUint16, ByteArrayBaseSetInt16, 0xce22484f)                 \
  V(_TypedList, _setInt32, ByteArrayBaseSetInt32, 0xbddaab40)                  \
  V(_TypedList, _setUint32, ByteArrayBaseSetUint32, 0xb966a3b2)                \
  V(_TypedList, _setInt64, ByteArrayBaseSetInt64, 0xc8cd4f7a)                  \
  V(_TypedList, _setUint64, ByteArrayBaseSetUint64, 0xda473205)                \
  V(_TypedList, _setFloat32, ByteArrayBaseSetFloat32, 0x2f362de0)              \
  V(_TypedList, _setFloat64, ByteArrayBaseSetFloat64, 0x2359f8d2)              \
  V(_TypedList, _setFloat32x4, ByteArrayBaseSetFloat32x4, 0x38c6295a)          \
  V(_TypedList, _setInt32x4, ByteArrayBaseSetInt32x4, 0x5ce9025b)              \
  V(Object, get:runtimeType, ObjectRuntimeType, 0x0381c851)

// List of recognized list factories:
// (factory-name-symbol, class-name-string, constructor-name-string,
//  result-cid, fingerprint).
#define RECOGNIZED_LIST_FACTORY_LIST(V)                                        \
  V(_ListFactory, _List, ., kArrayCid, 0xd693eee6)                             \
  V(_ListFilledFactory, _List, .filled, kArrayCid, 0x4c76526a)                 \
  V(_ListGenerateFactory, _List, .generate, kArrayCid, 0x4f848337)             \
  V(_GrowableListFactory, _GrowableList, ., kGrowableObjectArrayCid,           \
    0x434de9b9)                                                                \
  V(_GrowableListFilledFactory, _GrowableList, .filled,                        \
    kGrowableObjectArrayCid, 0x261b3cc2)                                       \
  V(_GrowableListGenerateFactory, _GrowableList, .generate,                    \
    kGrowableObjectArrayCid, 0x0c7be78f)                                       \
  V(_GrowableListWithData, _GrowableList, ._withData, kGrowableObjectArrayCid, \
    0xa32d060b)                                                                \
  V(_Int8ArrayFactory, Int8List, ., kTypedDataInt8ArrayCid, 0x660dd888)        \
  V(_Uint8ArrayFactory, Uint8List, ., kTypedDataUint8ArrayCid, 0xede3f64f)     \
  V(_Uint8ClampedArrayFactory, Uint8ClampedList, .,                            \
    kTypedDataUint8ClampedArrayCid, 0x28063755)                                \
  V(_Int16ArrayFactory, Int16List, ., kTypedDataInt16ArrayCid, 0xd0cd98f3)     \
  V(_Uint16ArrayFactory, Uint16List, ., kTypedDataUint16ArrayCid, 0x3cb5fb6a)  \
  V(_Int32ArrayFactory, Int32List, ., kTypedDataInt32ArrayCid, 0x1b8ff320)     \
  V(_Uint32ArrayFactory, Uint32List, ., kTypedDataUint32ArrayCid, 0x2b2f9a8b)  \
  V(_Int64ArrayFactory, Int64List, ., kTypedDataInt64ArrayCid, 0xfb71de2f)     \
  V(_Uint64ArrayFactory, Uint64List, ., kTypedDataUint64ArrayCid, 0xe3cfcff8)  \
  V(_Float64ArrayFactory, Float64List, ., kTypedDataFloat64ArrayCid,           \
    0xa0c64e91)                                                                \
  V(_Float32ArrayFactory, Float32List, ., kTypedDataFloat32ArrayCid,           \
    0xa39068fe)                                                                \
  V(_Float32x4ArrayFactory, Float32x4List, ., kTypedDataFloat32x4ArrayCid,     \
    0x0a7d7b88)

// clang-format on

}  // namespace dart

#endif  // RUNTIME_VM_COMPILER_RECOGNIZED_METHODS_LIST_H_
