#include "basecommon.h"

#define _CRC_BIT_CALC_			//CRC使用位运算.

void DWORD32_BYTE8(
	DWORD32				dwValue,
	PBYTE8				pbData
) {
	BYTE8 *pbTemp = pbData;

	*( pbTemp ++ ) = dwValue & 0xFF;
	*( pbTemp ++ ) = ( dwValue >> 8 ) & 0xFF;
	*( pbTemp ++ ) = ( dwValue >> 16 ) & 0xFF;
	*pbTemp = ( dwValue >> 24 ) & 0xFF;
}

void WORD16_BYTE8(
	WORD16				wValue,
	PBYTE8				pbData
) {
	BYTE8 *pbTemp = pbData;

	*( pbTemp ++ ) = wValue & 0xFF;
	*pbTemp = ( wValue >> 8 ) & 0xFF;
}

DWORD32 BYTE8_DWORD32( BYTE8 *pbData ) {
	return ( ( pbData[3] << 24 ) | ( pbData[2] << 16 ) | ( pbData[1] << 8 ) | pbData[0] );
}

WORD16 BYTE8_WORD16( BYTE8 *pbData ) {
	return ( ( pbData[1] << 8 ) | pbData[0] );
}

DWORD32 CalculateBitCrc32(
	PBYTE8				pbData,
	DWORD32				dwLen
) {
	DWORD32 dwCrcValue = 0;
	BYTE8 bTemp, *pbTemp = pbData;

	while( ( dwLen -- ) != 0 ) {
		for ( bTemp = 1; bTemp != 0; bTemp <<= 1 ) {
			if ( ( dwCrcValue & 0x80000000 ) != 0 ) {
				dwCrcValue <<= 1;
				dwCrcValue ^= 0x04C11DB7;
			} else {
				dwCrcValue <<= 1;
			}
			if ( ( *pbTemp & bTemp ) != 0 ) dwCrcValue ^= 0x04C11DB7;
		}
		pbTemp ++;
	}
	return dwCrcValue;
}

WORD16 CalculateBitCrc16(
	PBYTE8				pbData,
	DWORD32				dwLen
) {
	WORD16 wCrcValue = 0;

	return 	wCrcValue;
}

BYTE8 CalculateBitCrc8(
	PBYTE8				pbData,
	DWORD32				dwLen
) {
	BYTE8 bCrcVal = 0, *pbTemp = pbData;
	BYTE8 bTemp;

  	while( ( dwLen -- ) != 0 ) {
  		for( bTemp = 1; bTemp != 0; bTemp = ( bTemp << 1 ) ) {
      		if( ( bCrcVal & 0x01 ) != 0 ) {
      			bCrcVal = bCrcVal >> 1;
      			bCrcVal = bCrcVal ^ 0x8c;
      		} else {
        		bCrcVal = bCrcVal >> 1;
      		}
            if( ( *pbTemp & bTemp ) != 0 ) bCrcVal = bCrcVal ^ 0x8c;
    	}
    	pbTemp ++;
  	}
  	return bCrcVal;
}

DWORD32 CalculateCrc32(
	PBYTE8				pbData,
	DWORD32				dwLen
) {
#ifdef _CRC_BIT_CALC_
	return CalculateBitCrc32( pbData, dwLen );
#else
	return 0;
#endif
}

WORD16 CalculateCrc16(
	PBYTE8				pbData,
	DWORD32				dwLen
) {
#ifdef _CRC_BIT_CALC_
	return CalculateBitCrc16( pbData, dwLen );
#else
	return 0;
#endif
}

BYTE8 CalculateCrc8(
	PBYTE8				pbData,
	DWORD32				dwLen
){
#ifdef _CRC_BIT_CALC_
	return CalculateBitCrc8( pbData, dwLen );
#else
	return 0;
#endif
}

int StringCmp( LPCSTR pcszString1, LPCSTR pcszString2 ) {
	do {
		if ( *pcszString1 != *pcszString2 ) return 1;
		if ( *pcszString1 == '\0' ) break;
		pcszString1 ++;
		pcszString2 ++;
	} while ( 1 );
	return 0;
}

#ifndef _DSP_LOADER

int GetBmpInformation(
	PBYTE8			pbData,
	DWORD32			dwLen,
	PDWORD32		pdwOffset,
	BmpImageInfo	*pInfo
) {
	DWORD32 bmpSize, offset;
	DWORD32 byteNumber = 14;
	DWORD32 dwImageWidth, dwImageHeight, dwImageSize;
	PBYTE8 pbTemp = pbData;

	if ( ( pbData == NULL ) || ( pdwOffset == NULL ) || ( pInfo == NULL ) ) return -10;
	if ( ( pbTemp[0] != 'B' ) || ( pbTemp[1] != 'M' ) ) return -1;

	bmpSize = BYTE8_DWORD32( &pbTemp[2] );
	if ( bmpSize != dwLen ) return -2;
	offset = BYTE8_DWORD32( &pbTemp[10] );

	//取得余下的有关该BMP文件有关信息,如图象的宽度和高度以及象素等其他方面信息.
	pbTemp = pbData + byteNumber;
	byteNumber = offset - byteNumber;
	dwImageWidth = BYTE8_DWORD32( &pbTemp[4] );
	dwImageHeight = BYTE8_DWORD32( &pbTemp[8] );
	dwImageSize = BYTE8_DWORD32( &pbTemp[20] );
	if ( ( dwImageWidth % 16 ) == 0 ) {
		pInfo->nCtrlWidth = dwImageWidth;
	} else {
		pInfo->nCtrlWidth = dwImageWidth + 16 - ( dwImageWidth % 16 );
	}
	if ( ( dwImageHeight % 16 ) == 0 ) {
		pInfo->nCtrlHeight = dwImageHeight;
	} else {
		pInfo->nCtrlHeight = dwImageHeight + 16 - ( dwImageHeight % 16 );
	}

	pInfo->nImageWidth = dwImageWidth;
	pInfo->nImageHeight = dwImageHeight;
	pInfo->nImageSize = dwImageSize;
	return 0;
}

#endif
