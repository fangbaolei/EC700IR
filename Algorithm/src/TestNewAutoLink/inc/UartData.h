#ifndef _UART_DATA_H
#define _UART_DATA_H

#include "swBaseType.h"

#ifdef __cplusplus
	extern "C" {
#endif

int InitUartData( BOOL fComTerminalOpen );

int UartWriteData( BYTE8 *pDataSrc, int iDataLenb, int iType );

int UartReadData( BYTE8 *pDataDest, int iReadLen, int iType );

int UartWriteString( LPSTR pszString, int iType );

#ifdef __cplusplus
	}
#endif

#endif

