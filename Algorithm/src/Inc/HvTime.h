/*
 *	(C)版权所有 2010 北京信路威科技发展有限公司
 */

/**
* @file		HvTime.h
* @version	1.0
* @brief	有关时间的操作函数
* @author	Shaorg
* @date		2010-8-20
*/

#ifndef _HVTIME_H_
#define _HVTIME_H_

#include "hvutils.h"

//普通年份每月的天数
const BYTE8 baMDays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//闰年年份每月的天数
const BYTE8 baLMDays[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

typedef struct _REAL_TIME_STRUCT
{
    WORD16				wYear;			//年.
    WORD16				wMonth;			//月.
    WORD16				wDay;			//日.
    WORD16				wWeekNum;		//当前日期的星期数. /* 0为周日，1为周一，以此类推*/
    WORD16				wHour;			//时, 24小时制.
    WORD16				wMinute;		//分.
    WORD16				wSecond;		//秒.
    WORD16				wMSecond;		//毫秒.
} REAL_TIME_STRUCT;

#define ConvertTickToSystemTime(tick, stlow, sthigh) \
{ \
	int iTickDiff = GetSystemTick() - (tick); \
	if (iTickDiff < 0) \
	{\
		iTickDiff = 0;\
	}\
	DWORD32 dwTickDiff = iTickDiff; \
	GetSystemTime(&(stlow), &(sthigh)); \
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

int ChangeSystemTime(
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
);

int SetSystemTime(
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
);

int SetSystemTime(
    REAL_TIME_STRUCT tTime
);

int GetSystemTime(
    DWORD32* pdwTimeMsLow,
    DWORD32* pdwTimeMsHigh
);

int GetNowDateTime(
    int* piYear,
    int* piMon,
    int* piDay,
    int* piHour,
    int* piMin,
    int* piSec
);

void ConvertMsToTime(
    DWORD32 dwMSCountLow,
    DWORD32 dwMSCountHigh,
    REAL_TIME_STRUCT *pRealTime
);

void ConvertTimeToMs(
    const REAL_TIME_STRUCT *pRealTime,
    DWORD32 *pdwMSCountLow,
    DWORD32 *pdwMSCountHigh
);

#endif

