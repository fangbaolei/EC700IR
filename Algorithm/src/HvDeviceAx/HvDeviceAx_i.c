

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_HvDeviceAxLib,0x52C4A44D,0xD2FF,0x4CBA,0x8B,0x58,0x16,0x6C,0xEA,0x92,0xBE,0x1E);


MIDL_DEFINE_GUID(IID, DIID__DHvDeviceAx,0xFBAD7529,0x8CC6,0x49B8,0x84,0x11,0xB6,0x5A,0xC8,0xD9,0x85,0x74);


MIDL_DEFINE_GUID(IID, DIID__DHvDeviceAxEvents,0xE6101EF2,0xEBDE,0x47C7,0xA8,0xC0,0x22,0xD8,0x12,0x43,0xBD,0x64);


MIDL_DEFINE_GUID(CLSID, CLSID_HvDeviceAx,0x7F3E4C52,0x224D,0x415A,0x95,0x68,0xFD,0x4B,0xB3,0x01,0x67,0xCF);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



