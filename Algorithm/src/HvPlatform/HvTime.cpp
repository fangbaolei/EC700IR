#include <time.h>
#include <sys/time.h>

#include "HvTime.h"
#include "misc.h"

//Comment by Shaorg:
//目前在DM6467平台上，
//存入RTC的time即为当前时区的time，因为板上的时区默认且必需为0（即：GMT），
//所以时区调整要在应用层进行。

// 时区设定：东8区
const DWORD32 g_dwTimeMsZone = 8 * 3600 * 1000;
#define GMT2LOCAL(ms) (ms - g_dwTimeMsZone)
#define LOCAL2GMT(ms) (ms + g_dwTimeMsZone)

static time_t ConvertMsToSecond(
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
)
{
    time_t time = 0;

    DWORD64 dw64TimeMs = 0;
    dw64TimeMs = dwTimeMsHigh;
    dw64TimeMs <<= 32;
    dw64TimeMs |= dwTimeMsLow;

    time = LOCAL2GMT(dw64TimeMs) / 1000;
    return time;
}

int ChangeSystemTime(
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
)
{
    time_t t = ConvertMsToSecond(dwTimeMsLow, dwTimeMsHigh);
    return ( 0 == stime(&t) ) ? (0) : (-1);
}

int SetSystemTime(
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
)
{
/*
    time_t t = ConvertMsToSecond(dwTimeMsLow, dwTimeMsHigh);

    //设置系统时间并将系统时间写入RTC
    if ( 0 == stime(&t) && 0 == HV_System("hwclock -w") )
    {
        return 0;
    }
    else
    {
        return -1;
    }
*/
    DWORD64 dw64Time = dwTimeMsHigh;
    dw64Time <<= 32;
    dw64Time |= dwTimeMsLow;

	struct timeval tv;
	tv.tv_sec = LOCAL2GMT(dw64Time) / 1000;
	tv.tv_usec = (dw64Time % 1000) * 1000;

	if(settimeofday(&tv, NULL) < 0)
	{
		HV_Trace(5, "SetSystemTime error, code=%s.\n", strerror(errno));
		return -1;
	}
	else
	{
		printf("SetSystemTime ok.\n");
		//将系统时间写入RTC
		HV_System("hwclock -w");
		return 0;
	}

}

int SetSystemTime(
    REAL_TIME_STRUCT tTime
)
{
    DWORD32 dwTimeMsLow = 0;
    DWORD32 dwTimeMsHigh = 0;
    ConvertTimeToMs(&tTime, &dwTimeMsLow, &dwTimeMsHigh);
    return SetSystemTime(dwTimeMsLow, dwTimeMsHigh);
}

int GetSystemTime(
    DWORD32* pdwTimeMsLow,
    DWORD32* pdwTimeMsHigh
)
{
    if ( !pdwTimeMsLow || !pdwTimeMsHigh ) return -1;

    //获得秒数
    time_t tCurTime = time(NULL);

    //获得微秒数
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv , &tz);

    //Attention by Shaorg:
    //下面这段运算要特别留意64位整型与32位整型之间运算时的类型转换，
    //否则会导致溢出。在Linux下与Windows下有着显著的异同。

    //转换为毫秒
    DWORD64 dw64TimeMs = tCurTime;
    dw64TimeMs *= 1000;
    dw64TimeMs += (tv.tv_usec / 1000);
    dw64TimeMs = GMT2LOCAL(dw64TimeMs);

    *pdwTimeMsLow = (DWORD32)(dw64TimeMs);
    *pdwTimeMsHigh = (DWORD32)(dw64TimeMs>>32);

    return 0;
}

int GetNowDateTime(
    int* piYear,
    int* piMon,
    int* piDay,
    int* piHour,
    int* piMin,
    int* piSec
)
{
    struct tm cNowDateTime = {0};
    time_t tNowTime = time(NULL);

    if ( NULL == gmtime_r((const time_t*)&tNowTime, &cNowDateTime) )
    {
        return -1;
    }
    else
    {
        if (piYear) *piYear = cNowDateTime.tm_year + 1900;
        if (piMon)  *piMon  = cNowDateTime.tm_mon + 1;
        if (piDay)  *piDay  = cNowDateTime.tm_mday;
        if (piHour) *piHour = cNowDateTime.tm_hour;
        if (piMin)  *piMin  = cNowDateTime.tm_min;
        if (piSec)  *piSec  = cNowDateTime.tm_sec;

        return 0;
    }
}

void ConvertMsToTime(
    DWORD32 dwMSCountLow,
    DWORD32 dwMSCountHigh,
    REAL_TIME_STRUCT *pRealTime
)
{
    //Attention by Shaorg:
    //下面这段运算要特别留意64位整型与32位整型之间运算时的类型转换，
    //否则会导致溢出。在Linux下与Windows下有着显著的异同。
    DWORD64 dw64MsCount = dwMSCountHigh;
    dw64MsCount <<= 32;
    dw64MsCount |= dwMSCountLow;
    dw64MsCount = LOCAL2GMT(dw64MsCount);

    pRealTime->wMSecond = dw64MsCount % 1000;
    time_t tTotalSecond = (dw64MsCount - pRealTime->wMSecond) / 1000;

    struct tm cDateTime = {0};
    gmtime_r((const time_t*)&tTotalSecond, &cDateTime);

    pRealTime->wYear = (cDateTime.tm_year + 1900);
    pRealTime->wMonth = (cDateTime.tm_mon + 1);
    pRealTime->wDay = cDateTime.tm_mday;
    pRealTime->wHour = cDateTime.tm_hour;
    pRealTime->wMinute = cDateTime.tm_min;
    pRealTime->wSecond = cDateTime.tm_sec;

    pRealTime->wWeekNum = cDateTime.tm_wday; /* 0为周日，1为周一，以此类推*/
}

void ConvertTimeToMs(
    const REAL_TIME_STRUCT *pRealTime,
    DWORD32 *pdwMSCountLow,
    DWORD32 *pdwMSCountHigh
)
{
    if ( (NULL == pRealTime)
            || (NULL ==pdwMSCountLow)
            || (NULL == pdwMSCountHigh) )
    {
        return;
    }

    struct tm tm_time;
    memset(&tm_time, 0, sizeof(tm_time));
    tm_time.tm_year = pRealTime->wYear-1900;
    tm_time.tm_mon = pRealTime->wMonth-1;
    tm_time.tm_mday = pRealTime->wDay;
    tm_time.tm_hour = pRealTime->wHour;
    tm_time.tm_min = pRealTime->wMinute;
    tm_time.tm_sec = pRealTime->wSecond;

    time_t time = mktime(&tm_time);
    DWORD64 dw64TimeMs = time;
    dw64TimeMs *= 1000;
    dw64TimeMs = GMT2LOCAL(dw64TimeMs);

    *pdwMSCountLow = (DWORD32)(dw64TimeMs);
    *pdwMSCountHigh = (DWORD32)(dw64TimeMs>>32);
}
