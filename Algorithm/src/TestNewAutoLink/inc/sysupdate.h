#ifndef _SYS_UPDATE_H
#define _SYS_UPDATE_H

#ifdef __cplusplus
extern "C" {
#endif		// #ifdef __cplusplus

#include "swBaseType.h"

typedef struct _FILE_DATA_BLOCK {
	DWORD32 dwFileSerialNumber;				
	DWORD32 dwFileVersion;
	DWORD32 dwFileLength;
	char pFileInfo[ 32 ];
	BYTE8 *pBlockData;
} FILE_DATA_BLOCK;

#ifdef __cplusplus
}
#endif		// #ifdef __cplusplus

#endif //#ifndef _SYS_UPDATE_H

