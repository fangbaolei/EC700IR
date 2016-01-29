#ifndef _HV_SECURITY_H
#define _HV_SECURITY_H

#include "swbasetype.h"
#include "rsa.h"
#include "crypt.h"

#define HS_MASK_1 0x8ac9673e
#define HS_MASK_2 0x3a6f8b27
#define HS_MASK_3 0x5b33af68
#define HS_MASK_4 0x2fd5869a
#define HS_MASK_5 0x0B,0xF1,0x18,0x14,0x05,0x81,0xC5,0xA0,0x01,0xC7,0xEA,0x14,0x03,0xA2,0x9C,0x90

#define HS_KEYFILE_MAIN 980
#define HS_KEYFILE_BAK 982

#define CONVERTSID(p) { BYTE8 pbCode[6]={0x2f,0x5b,0x3a,0x8a,0xc9,0x6f}; ;HS_XorEnc(pbCode,6,p,6);}

#if RUN_PLATFORM == PLATFORM_WINDOWS

#include "crypt_Lib.h"
#pragma comment(lib, "Cryptst.lib")

#include "sense4.h"
#pragma comment(lib, "Sense4ST.lib")

extern void HS_ConvertKey(R_RSA_PRIVATE_KEY& rkey,RSA_PRIVATE_KEY& hskey);
extern void HS_ConvertKey(R_RSA_PUBLIC_KEY& rkey,RSA_KEY& hskey);

extern HRESULT HS_LoadKeyFile(LPCSTR szPath, R_RSA_PUBLIC_KEY& r_pubkey, BYTE8* rgMac, BYTE8* rgSID);
extern HRESULT HS_LoadKeyFile(LPCSTR szPath, RSA_PRIVATE_KEY& prikey, BYTE8* rgMac, BYTE8* rgSID);

extern HRESULT HS_SaveKeyFile(LPCSTR szPath, R_RSA_PUBLIC_KEY& r_pubkey, BYTE8* rgMac, BYTE8* rgSID);
extern HRESULT HS_SaveKeyFile(LPCSTR szPath, RSA_PRIVATE_KEY& prikey, BYTE8* rgMac, BYTE8* rgSID);

#endif

#if RUN_PLATFORM == PLATFORM_DSP_BIOS

extern int HS_GetPriKey(RSA_PRIVATE_KEY& prikey);

extern int HS_TestPriKey(BYTE8* pbTestMsg);

#endif

#endif //_HV_SECURITY_H
