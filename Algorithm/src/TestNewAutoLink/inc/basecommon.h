#ifndef _BASECOMMON_H
#define _BASECOMMON_H

#include "swBaseType.h"

#ifdef __cplusplus
	extern "C" {
#endif /* #ifdef __cplusplus */

#define Stringer( variable )	#variable

void DWORD32_BYTE8(
	DWORD32				dwValue,
	PBYTE8				pbData
);
void WORD16_BYTE8( 
	WORD16				wValue, 
	PBYTE8				pbData
);
DWORD32 BYTE8_DWORD32( BYTE8 *pbData );
WORD16 BYTE8_WORD16( BYTE8 *pbData );

DWORD32 CalculateCrc32( 
	PBYTE8				pbData,
	DWORD32				dwLen
);
WORD16 CalculateCrc16(
	PBYTE8				pbData,
	DWORD32				dwLen
);
BYTE8 CalculateCrc8(
	PBYTE8				pbData,
	DWORD32				dwLen
);

int StringCmp( 
	LPCSTR				pcszString1, 
	LPCSTR				pcszString2
);

typedef struct tagBmpImageInfo {
	int nImageWidth;
	int nImageHeight;
	int nCtrlWidth;
	int nCtrlHeight;
	int nImageSize;
} BmpImageInfo;

int GetBmpInformation( 
	PBYTE8			pbData,
	DWORD32			dwLen,
	PDWORD32		pdwOffset,
	BmpImageInfo	*pInfo
);

#ifdef __cplusplus
	}
#endif /* #ifdef __cplusplus */

#endif
