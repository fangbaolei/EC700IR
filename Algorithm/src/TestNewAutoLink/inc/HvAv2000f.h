#ifndef _HVAV2000F_H_
#define _HVAV2000F_H_

#include "swbasetype.h"

#if (RUN_PLATFORM == PLATFORM_WINDOWS)

#ifdef HVAV2000F_API_EXPORTS
#define HVAV2000F_API extern "C" __declspec(dllexport) HRESULT WINAPI
#else
#define HVAV2000F_API extern "C" __declspec(dllimport) HRESULT WINAPI
#endif

#else
#define HVAV2000F_API HRESULT

#endif

typedef void * HVAV2000F_HANDLE;

HVAV2000F_API HVAV2000F_Open(
							LPCSTR pszAddress,
							HVAV2000F_HANDLE *pHandle
							);
HVAV2000F_API HVAV2000F_Close(
							HVAV2000F_HANDLE handle
							);
HVAV2000F_API HVAV2000F_GetDefaultImage(
							HVAV2000F_HANDLE handle,
							unsigned char *pImageData,
							int *piImageSize,
							DWORD32* pdwTime
							);
HVAV2000F_API HVAV2000F_SetImageQuality(
							HVAV2000F_HANDLE handle,
							unsigned short wQuality
							);
HVAV2000F_API HVAV2000F_GetRegister(
							HVAV2000F_HANDLE handle,
							unsigned char bRegNum,
							unsigned short &wRegVal
							);
HVAV2000F_API HVAV2000F_SetRegister(
							HVAV2000F_HANDLE handle,
							unsigned char bRegNum,
							unsigned short wRegVal
							);
HVAV2000F_API HVAV2000F_GetHeight(
							HVAV2000F_HANDLE handle,
							unsigned short &wHeight
							);
HVAV2000F_API HVAV2000F_GetWidth(
							HVAV2000F_HANDLE handle,
							unsigned short &wWidth
							);
HVAV2000F_API HVAV2000F_GetDevVer(
							HVAV2000F_HANDLE handle,
							unsigned short &wDevVer
							);
HVAV2000F_API HVAV2000F_GetDevType(
							HVAV2000F_HANDLE handle,
							unsigned short &wDevType
							);
HVAV2000F_API HVAV2000F_SaveConfigForever(
							HVAV2000F_HANDLE handle
							);
HVAV2000F_API HVAV2000F_RestoreDefaultConfig(
							HVAV2000F_HANDLE handle
							);
HVAV2000F_API HVAV2000F_SearchDevCount(
							int& iCount
							);
HVAV2000F_API HVAV2000F_GetDevAddr(
							int iIndex, 
							QWORD64& dw64MacAddr, 
							DWORD32& dw32IP
							);
HVAV2000F_API HVAV2000F_SetDevIPFromMac(
							QWORD64 dw64Mac, 
							DWORD32 dw32IP
							);
#endif
