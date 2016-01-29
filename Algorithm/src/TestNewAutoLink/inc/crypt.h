#ifndef __CRYPT_INCLUDED__
#define __CRYPT_INCLUDED__

#include "swBaseType.h"

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned int* xxtea_initkey(void* pKeyIn,unsigned int nKeyLenght,unsigned int* pKeyOut);
extern int xxtea_encrypt(unsigned char* pbIn , unsigned int nDatLenght , unsigned char* pbOut , unsigned int* pKey);
extern int xxtea_decrypt(unsigned char* pbIn , unsigned int nDatLenght , unsigned int* pKey);
extern int encrypt_block(unsigned char* pbIn , unsigned int nLenght , unsigned char* pbOut , unsigned int* pKey);
extern int decrypt_block(unsigned char* pbIn , unsigned int nLenght , unsigned int* pKey);

extern void HS_EncodeKey(int nIndex,BYTE8* pbKey,BYTE8* pbBuf,BYTE8* pbUID);
extern void HS_DecodeKey(int nIndex,BYTE8* pbKey,BYTE8* pbBuf,BYTE8* pbUID);

extern void HS_XorEnc(BYTE8* rgCode, UINT dwCodeLen, BYTE8* pbBuf, UINT dwBufLen);

#ifdef __cplusplus
}
#endif

#endif
