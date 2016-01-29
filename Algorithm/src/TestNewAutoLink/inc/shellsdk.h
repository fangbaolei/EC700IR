#ifndef _ShellSDK_
#define _ShellSDK_

#include <windows.h>

 #define SAFENET_CRYPT_BEGIN \
 __asm _emit 0xEB \
 __asm _emit 0x08 \
 __asm _emit 0xEB \
 __asm _emit 0x05 \
 __asm _emit 0x89 \
 __asm _emit 0x01 \
 __asm _emit 0xEB \
 __asm _emit 0x05 \
 __asm _emit 0x89 \
 __asm _emit 0x01		

 #define SAFENET_CRYPT_END \
 __asm _emit 0xEB \
 __asm _emit 0x08 \
 __asm _emit 0xEB \
 __asm _emit 0x05 \
 __asm _emit 0x99 \
 __asm _emit 0x01 \
 __asm _emit 0xEB \
 __asm _emit 0x05 \
 __asm _emit 0x99 \
 __asm _emit 0x01 

#define SAFENET_CONSTANT \
 __asm _emit 0xEB \
 __asm _emit 0x08 \
 __asm _emit 0xEB \
 __asm _emit 0x05 \
 __asm _emit 0x90 \
 __asm _emit 0x01 \
 __asm _emit 0xEB \
 __asm _emit 0x05 \
 __asm _emit 0x90 \
 __asm _emit 0x01 

#define SAFENET_USE_STRING \
 __asm _emit 0xEB \
 __asm _emit 0x08 \
 __asm _emit 0xEB \
 __asm _emit 0x05 \
 __asm _emit 0x91 \
 __asm _emit 0x01 \
 __asm _emit 0xEB \
 __asm _emit 0x05 \
 __asm _emit 0x91 \
 __asm _emit 0x01 

#define SAFENET_FREE_STRING \
 __asm _emit 0xEB \
 __asm _emit 0x08 \
 __asm _emit 0xEB \
 __asm _emit 0x05 \
 __asm _emit 0x92 \
 __asm _emit 0x01 \
 __asm _emit 0xEB \
 __asm _emit 0x05 \
 __asm _emit 0x92 \
 __asm _emit 0x01 


#ifdef  __cplusplus
extern "C" {
#endif


extern int __stdcall SafeNetConstant(int iTemp);

extern char * __stdcall SafeNetString(char * szString);

extern void __stdcall SafeNetFreeString(char * szString);


#ifdef  __cplusplus
}
#endif

#endif //_ShellSDK_