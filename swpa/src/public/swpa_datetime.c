#include <stdio.h>
#include <string.h>
#include "swpa.h"
#include "swpa_private.h"
#include "swpa_datetime.h"
#include <time.h>
#include <sys/time.h>

#ifdef SWPA_DATETIME
#define SWPA_DATETIME_PRINT(fmt, ...) SWPA_PRINT("[%s:%d]"fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define SWPA_DATETIME_CHECK(arg)      {if (!(arg)){SWPA_PRINT("[%s:%d]Check failed : %s [%d]\n", __FILE__, __LINE__, #arg, SWPAR_INVALIDARG);return SWPAR_INVALIDARG;}}
#else
#define SWPA_DATETIME_PRINT(fmt, ...)
#define SWPA_DATETIME_CHECK(arg)	{if (!(arg)){return SWPAR_INVALIDARG;}}	//日志可以不输出，但检查不能关闭掉
#endif

static int g_nTimeZone = 0;

/**
* @brief 获取系统当前的时钟滴答数
* 
* @retval tick 时钟滴答
* @see swpa_datetime.h
*/
unsigned long swpa_datetime_gettick(void)
{
    unsigned long tick;
    
    //SWPA_DATETIME_CHECK(tick != NULL);
    //SWPA_PRINT_ULONG(tick);
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    tick = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    
    return tick;
}

/**
* @brief 获取当前时区
*
* @retval 当前时区
* @see swpa_datetime.h
* @note 该时区为一整数值。例如：8则表示+8时区，即北京时间。
*/
int swpa_datetime_gettimezone(void)
{
    return g_nTimeZone;
}
#define ZONE_INFO "/usr/share/zoneinfo/"
#define ZONE_LOCAL "/usr/share/zoneinfo/localtime"
#define ZONE_POSIX "/usr/share/zoneinfo/posixrules"
static const char *TZfiles[][2] = {
	{"Etc/GMT+12", "Pacific/Kwajalein"},
	{"Etc/GMT+11", "Pacific/Midway"},
	{"Etc/GMT+10", "US/Hawaii"},
	{"Etc/GMT+9", "US/Alaska"},
	{"Etc/GMT+8", "US/Pacific"},
	{"Etc/GMT+7", "US/Mountain"},
	{"Etc/GMT+6", "US/Central"},
	{"Etc/GMT+5", "US/Eastern"},
	{"Etc/GMT+4", "Canada/Atlantic"},
	{"Etc/GMT+3", "America/Buenos_Aires"},
	{"Etc/GMT+2", "Etc/GMT+2"},
	{"Etc/GMT+1", "Atlantic/Azores"},
	{"Etc/GMT", "Europe/London"},
	{"Etc/GMT-1", "Europe/Berlin"},
	{"Etc/GMT-2", "Europe/Athens"},
	{"Etc/GMT-3", "Europe/Moscow"},
	{"Etc/GMT-4", "Asia/Muscat"},
	{"Etc/GMT-5", "Asia/Karachi"},
	{"Etc/GMT-6", "Asia/Dhaka"},
	{"Etc/GMT-7", "Asia/Bangkok"},
	{"Etc/GMT-8", "Asia/Taipei"},
	{"Etc/GMT-9", "Asia/Tokyo"},
	{"Etc/GMT-10", "Australia/Brisbane"},
	{"Etc/GMT-11", "Asia/Magadan"},
	{"Etc/GMT-12", "Pacific/Fiji"}};

/**
* @brief 设置当前时区
* @param [in] tz 当前时区
* @retval 0 成功
* @retval -1 失败
* @see swpa_datetime.h
* @note 该时区为一整数值。例如：8则表示+8时区，即北京时间。
*/
int swpa_datetime_settimezone(int tz)
{
    SWPA_DATETIME_CHECK(tz >= 0 && tz <=24);
    SWPA_DATETIME_PRINT("tz=%d\n", tz);
    //g_nTimeZone = tz - 20;		//系统本身就是东八区,GMT+8,0为GMT-12

	unsigned char idx1, idx2;
	char tzcmd[100];

	idx1 = tz;
	idx2 = 0;
	sprintf(tzcmd,"cp -f " ZONE_INFO "%s " ZONE_LOCAL, TZfiles[idx1][idx2]);
	if (SWPAR_OK != swpa_utils_shell(tzcmd, NULL))
	{
		SWPA_DATETIME_PRINT("%s system cmd %s failed\n", __FUNCTION__,tzcmd);
		return SWPAR_FAIL;
	}
	sprintf(tzcmd,"cp -f " ZONE_INFO "%s " ZONE_POSIX, TZfiles[idx1][idx2]);
	if (SWPAR_OK != swpa_utils_shell(tzcmd, NULL))
	{
		SWPA_DATETIME_PRINT("%s system cmd %s failed\n", __FUNCTION__,tzcmd);
		return SWPAR_FAIL;
	}
	sprintf(tzcmd,"ln -sf " ZONE_INFO "%s /etc/localtime\n", TZfiles[idx1][idx2]);
	if (SWPAR_OK != swpa_utils_shell(tzcmd, NULL))
	{
		SWPA_DATETIME_PRINT("%s system cmd %s failed\n", __FUNCTION__,tzcmd);
		return SWPAR_FAIL;
	}
	
	SWPA_DATETIME_PRINT("changeTimeZone:\n %s\n", tzcmd);
	tzset();
	g_nTimeZone = tz;
	
    return SWPAR_OK;
}

/**
* @brief 获取时间
* @param [out] t 时间结构体
* @retval 0 成功
* @retval -1 失败
* @see swpa_datetime.h
* @note 该时间结构体包含从1970-01-01 00:00:00开始至目前所经过的秒数以及余下的当前毫秒数。注：与当前所处时区相关。
*/
int swpa_datetime_gettime(struct _SWPA_TIME* t)
{
    SWPA_DATETIME_CHECK(t != NULL);
    SWPA_DATETIME_PRINT("t=0x%08x\n", (unsigned int)t);

	//gettime 之前先同步下时区，避免某个进程修改时区后别的进程时区不同的问题
	tzset();
	
#if 0	//这里用time()函数取得秒数是画蛇添足了，且可移植性也不好，在ITTS上工作不正常
    time_t tCurTime = time(NULL);
    struct timeval tv;
    gettimeofday(&tv , NULL);
    long long dw64TimeMs = tCurTime;
    dw64TimeMs *= 1000;
    dw64TimeMs += (tv.tv_usec / 1000);
    //dw64TimeMs = dw64TimeMs - g_nTimeZone * 3600000;
    t->sec = dw64TimeMs / 1000;
    t->msec = dw64TimeMs % 1000;
#else
	struct timeval tv;
    if (0 != gettimeofday(&tv , NULL))
    {	
    	return SWPAR_FAIL;
    }

    t->sec = tv.tv_sec;
    t->msec = tv.tv_usec / 1000;
#endif	
    return SWPAR_OK;
}

/**
* @brief 设置时间
* @param [in] t 时间结构体
* @retval 0 成功
* @retval -1 失败
* @see swpa_datetime.h
* @note 该时间结构体包含从1970-01-01 00:00:00开始至目前所经过的秒数以及余下的当前毫秒数。注：与当前所处时区相关。
*/
int swpa_datetime_settime(const struct _SWPA_TIME* t)
{
    SWPA_DATETIME_CHECK(t != NULL);
    SWPA_DATETIME_PRINT("t=0x%08x\n", (unsigned int)t);
    long long dw64Time = t->sec;
    dw64Time = dw64Time * 1000 + t->msec;
    struct timeval tv;
    //dw64Time = dw64Time + g_nTimeZone * 3600000;
    tv.tv_sec = dw64Time / 1000;
    tv.tv_usec = (dw64Time % 1000) * 1000;
    if(settimeofday(&tv, NULL) < 0)
    {
        return SWPAR_FAIL;
    }
    return SWPAR_OK;
}

/**
* @brief 将秒数时间转为日期时间
* @param [in] t 时间结构体
* @param [out] tm 日期时间
* @retval 0 成功
* @retval -1 失败
* @see swpa_datetime.h
*/
int swpa_datetime_time2tm(struct _SWPA_TIME t, struct _SWPA_DATETIME_TM* tm)
{
    SWPA_DATETIME_CHECK(tm != NULL);
    SWPA_DATETIME_PRINT("t.sec=%d\n", t.sec);
    SWPA_DATETIME_PRINT("t.msec=%d\n", t.msec);
    SWPA_DATETIME_PRINT("tm=0x%08x\n", (unsigned int)tm);

    long long dw64MsCount = t.sec;
    dw64MsCount = dw64MsCount * 1000 + t.msec;
    //dw64MsCount = dw64MsCount + g_nTimeZone * 3600000;
    tm->msec = dw64MsCount % 1000;
    time_t tTotalSecond = dw64MsCount / 1000;
    struct tm cDateTime = {0};
    localtime_r((const time_t*)&tTotalSecond, &cDateTime);
    tm->year = cDateTime.tm_year + 1900;
    tm->month = cDateTime.tm_mon + 1;
    tm->day = cDateTime.tm_mday;
    tm->hour = cDateTime.tm_hour;
    tm->min = cDateTime.tm_min;
    tm->sec = cDateTime.tm_sec;
    tm->weeknum = cDateTime.tm_wday;

    return SWPAR_OK;
}

/**
* @brief 将日期时间转为秒数时间
* @param [in] tm 日期时间
* @param [out] t 时间结构体
* @retval 0 成功
* @retval -1 失败
* @see swpa_datetime.h
*/
int swpa_datetime_tm2time(struct _SWPA_DATETIME_TM tm, struct _SWPA_TIME* t)
{
    SWPA_DATETIME_CHECK(t != NULL);
    SWPA_DATETIME_PRINT("tm.year=%d\n", tm.year);
    SWPA_DATETIME_PRINT("tm.month=%d\n", tm.month);
    SWPA_DATETIME_PRINT("tm.day=%d\n", tm.day);
    SWPA_DATETIME_PRINT("tm.hour=%d\n", tm.hour);
    SWPA_DATETIME_PRINT("tm.min=%d\n", tm.min);
    SWPA_DATETIME_PRINT("tm.sec=%d\n", tm.sec);
    SWPA_DATETIME_PRINT("tm.msec=%d\n", tm.msec);
    SWPA_DATETIME_PRINT("tm.weeknum=%d\n", tm.weeknum);
    SWPA_DATETIME_PRINT("t=0x%08x\n", (unsigned int)t);

    struct tm tm_time;
    memset(&tm_time, 0, sizeof(tm_time));
    tm_time.tm_year = tm.year - 1900;
    tm_time.tm_mon = tm.month - 1;
    tm_time.tm_mday = tm.day;
    tm_time.tm_hour = tm.hour;
    tm_time.tm_min = tm.min;
    tm_time.tm_sec = tm.sec;
    time_t time = mktime(&tm_time);
    long long dw64TimeMs = time;
    dw64TimeMs *= 1000;
    //dw64TimeMs -= g_nTimeZone * 3600000;
    t->sec = dw64TimeMs / 1000;
    t->msec = dw64TimeMs % 1000;

    return SWPAR_OK;
}

