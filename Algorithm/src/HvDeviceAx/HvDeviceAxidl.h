

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Mon Aug 17 11:15:45 2015
 */
/* Compiler settings for HvDeviceAx.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __HvDeviceAxidl_h__
#define __HvDeviceAxidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DHvDeviceAx_FWD_DEFINED__
#define ___DHvDeviceAx_FWD_DEFINED__
typedef interface _DHvDeviceAx _DHvDeviceAx;
#endif 	/* ___DHvDeviceAx_FWD_DEFINED__ */


#ifndef ___DHvDeviceAxEvents_FWD_DEFINED__
#define ___DHvDeviceAxEvents_FWD_DEFINED__
typedef interface _DHvDeviceAxEvents _DHvDeviceAxEvents;
#endif 	/* ___DHvDeviceAxEvents_FWD_DEFINED__ */


#ifndef __HvDeviceAx_FWD_DEFINED__
#define __HvDeviceAx_FWD_DEFINED__

#ifdef __cplusplus
typedef class HvDeviceAx HvDeviceAx;
#else
typedef struct HvDeviceAx HvDeviceAx;
#endif /* __cplusplus */

#endif 	/* __HvDeviceAx_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __HvDeviceAxLib_LIBRARY_DEFINED__
#define __HvDeviceAxLib_LIBRARY_DEFINED__

/* library HvDeviceAxLib */
/* [control][helpstring][helpfile][version][uuid] */ 


EXTERN_C const IID LIBID_HvDeviceAxLib;

#ifndef ___DHvDeviceAx_DISPINTERFACE_DEFINED__
#define ___DHvDeviceAx_DISPINTERFACE_DEFINED__

/* dispinterface _DHvDeviceAx */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DHvDeviceAx;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("FBAD7529-8CC6-49B8-8411-B65AC8D98574")
    _DHvDeviceAx : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DHvDeviceAxVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DHvDeviceAx * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DHvDeviceAx * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DHvDeviceAx * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DHvDeviceAx * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DHvDeviceAx * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DHvDeviceAx * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DHvDeviceAx * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DHvDeviceAxVtbl;

    interface _DHvDeviceAx
    {
        CONST_VTBL struct _DHvDeviceAxVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DHvDeviceAx_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DHvDeviceAx_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DHvDeviceAx_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DHvDeviceAx_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DHvDeviceAx_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DHvDeviceAx_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DHvDeviceAx_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DHvDeviceAx_DISPINTERFACE_DEFINED__ */


#ifndef ___DHvDeviceAxEvents_DISPINTERFACE_DEFINED__
#define ___DHvDeviceAxEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DHvDeviceAxEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DHvDeviceAxEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("E6101EF2-EBDE-47C7-A8C0-22D81243BD64")
    _DHvDeviceAxEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DHvDeviceAxEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DHvDeviceAxEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DHvDeviceAxEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DHvDeviceAxEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DHvDeviceAxEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DHvDeviceAxEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DHvDeviceAxEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DHvDeviceAxEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DHvDeviceAxEventsVtbl;

    interface _DHvDeviceAxEvents
    {
        CONST_VTBL struct _DHvDeviceAxEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DHvDeviceAxEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DHvDeviceAxEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DHvDeviceAxEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DHvDeviceAxEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DHvDeviceAxEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DHvDeviceAxEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DHvDeviceAxEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DHvDeviceAxEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_HvDeviceAx;

#ifdef __cplusplus

class DECLSPEC_UUID("7F3E4C52-224D-415A-9568-FD4BB30167CF")
HvDeviceAx;
#endif
#endif /* __HvDeviceAxLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


