// SWFC标准错误码
#ifndef _SWFC_ERRCODE_
#define _SWFC_ERRCODE_

#if defined (_MSC_VER) && (_MSC_VER >= 1020) && !defined(__midl)
#pragma once
#endif

#ifdef _HRESULT_TYPEDEF_
#undef _HRESULT_TYPEDEF_
#endif
#define _HRESULT_TYPEDEF_(_sc)	((HRESULT)_sc)


//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//

#ifdef SUCCEEDED
#undef SUCCEEDED
#endif
#define SUCCEEDED(Status) ((HRESULT)(Status) >= 0)

//
// and the inverse
//

#ifdef FAILED
#undef FAILED
#endif
#define FAILED(Status) ((HRESULT)(Status)<0)

//
// Success codes
//
#ifndef S_OK
#define S_OK							((HRESULT)0x00000000L)
#endif

#ifndef S_FALSE
#define S_FALSE							((HRESULT)0x00000001L)
#endif

//
// Error codes
//

//
// MessageId: E_UNEXPECTED
//
// MessageText:
//
//  Catastrophic failure
//
#define E_UNEXPECTED                     _HRESULT_TYPEDEF_(0x8000FFFFL)

//
// MessageId: E_NOTIMPL
//
// MessageText:
//
//  Not implemented
//
#ifndef E_NOTIMPL
#define E_NOTIMPL						_HRESULT_TYPEDEF_(0x80004001L)
#endif
//
// MessageId: E_OUTOFMEMORY
//
// MessageText:
//
//  Ran out of memory
//
#ifndef E_OUTOFMEMORY
#define E_OUTOFMEMORY					_HRESULT_TYPEDEF_(0x8007000EL)
#endif
//
// MessageId: E_INVALIDARG
//
// MessageText:
//
//  One or more arguments are invalid
//
#ifndef E_INVALIDARG
#define E_INVALIDARG					_HRESULT_TYPEDEF_(0x80070057L)
#endif
//
// MessageId: E_NOINTERFACE
//
// MessageText:
//
//  No such interface supported
//
#ifndef E_NOINTERFACE
#define E_NOINTERFACE					_HRESULT_TYPEDEF_(0x80004002L)
#endif
//
// MessageId: E_POINTER
//
// MessageText:
//
//  Invalid pointer
//
#ifndef E_POINTER
#define E_POINTER						_HRESULT_TYPEDEF_(0x80004003L)
#endif
//
// MessageId: E_HANDLE
//
// MessageText:
//
//  Invalid handle
//
#ifndef E_HANDLE
#define E_HANDLE						_HRESULT_TYPEDEF_(0x80070006L)
#endif
//
// MessageId: E_ABORT
//
// MessageText:
//
//  Operation aborted
//
#ifndef E_ABORT
#define E_ABORT							_HRESULT_TYPEDEF_(0x80004004L)
#endif
//
// MessageId: E_FAIL
//
// MessageText:
//
//  Unspecified error
//
#ifndef E_FAIL
#define E_FAIL							_HRESULT_TYPEDEF_(0x80004005L)
#endif
//
// MessageId: E_ACCESSDENIED
//
// MessageText:
//
//  General access denied error
//
#ifndef E_ACCESSDENIED
#define E_ACCESSDENIED					_HRESULT_TYPEDEF_(0x80070005L)
#endif
//
// MessageId: E_OBJ_NO_INIT
//
// MessageText:
//
//  Object has not been init
//
#ifndef E_OBJ_NO_INIT
#define E_OBJ_NO_INIT					_HRESULT_TYPEDEF_(0x8000000AL)
#endif

#ifndef CLASS_E_NOAGGREGATION
#define CLASS_E_NOAGGREGATION			_HRESULT_TYPEDEF_(0x80040110L)
#endif

#ifndef CLASS_E_CLASSNOTAVAILABLE
#define CLASS_E_CLASSNOTAVAILABLE	 	_HRESULT_TYPEDEF_(0x80040111L)
#endif


#ifndef E_OUTOFTIME
#define E_OUTOFTIME	 					_HRESULT_TYPEDEF_(0x80040112L)
#endif




#endif // _SWFC_ERRCODE_

