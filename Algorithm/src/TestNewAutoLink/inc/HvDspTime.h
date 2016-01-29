#ifndef _HV_DSP_TIME_
#define _HV_DSP_TIME_

#ifdef __cplusplus
extern "C" {
#endif	/* #ifdef __cplusplus */

#include "swBaseType.h"

/* 数据结构说明:用来定义实时时间. */
typedef struct _REAL_TIME_STRUCT {
	WORD16				wYear;			//年数.
	WORD16				wMonth;			//月数.
	WORD16				wDay;			//号数.
	WORD16				wWeekNum;		//当前日期的星期数.
	WORD16				wHour;			//小数数,24小时制.	
	WORD16				wMinute;		//小时数.
	WORD16				wSecond;		//秒数.
	WORD16				wMSecond;		//毫秒数.
} REAL_TIME_STRUCT;

int HV_GetSystemTime( DWORD32* pdwTimeLow, DWORD32* pdwTimeHigh );

int HV_SetSystemTime( DWORD32 dwTimeLow, DWORD32 dwTimeHigh );

int InitSysWorkTimer( void );

void ConvertMsToTime( 
			    REAL_TIME_STRUCT	*pRealTime, 
			    DWORD32 			dwMSCountLow, 
			    DWORD32 			dwMSCountHigh
			    );

void ConvertTimeToMs( 
		     REAL_TIME_STRUCT 	*pRealTime, 
		     DWORD32 			*pdwMSCountLow, 
		     DWORD32 			*pdwMSCountHigh
		     );

#define ConvertTickToSystemTime(tick, stlow, sthigh) \
{ \
	DWORD32 dwTickDiff = GetSystemTick() - (tick); \
	HV_GetSystemTime(&(stlow), &(sthigh)); \
	if (dwTickDiff <= (stlow)) \
	{ \
		(stlow) -= dwTickDiff; \
	} \
	else \
	{ \
		(sthigh)--; \
		(stlow) += (0xFFFFFFFF - dwTickDiff) ; \
	} \
}

#define ConvertSystemTimeToTick(stlow, tick) \
{ \
	DWORD32 dwLow, dwHigh, dwTick; \
	dwTick = GetSystemTick(); \
	HV_GetSystemTime(&dwLow, &dwHigh); \
	if (dwLow <= (stlow)) \
	{ \
		dwLow += (0xFFFFFFFF - (stlow)); \
		(tick) = dwTick - dwLow; \
	} \
	else \
	{ \
		(tick) = dwTick - (dwLow - (stlow)); \
	} \
}

#ifdef __cplusplus
}
#endif	/* #ifdef __cplusplus */

#endif
