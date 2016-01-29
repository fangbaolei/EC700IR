///////////////////////////////////////////////////////////
// CSWDateTime.cpp
// Implementation of the Class CSWDateTime
// Created on:   28-二月-2013 14:09:50
// Original author: zhouy
///////////////////////////////////////////////////////////

#include "SWDateTime.h"
#include "SWLog.h"

/**
 * @brief 构造函数
 * 
 * @param [in] sRealTime : 用作初始化该对象的实时结构体
 */
CSWDateTime::CSWDateTime(const SWPA_DATETIME_TM* sRealTime){

    swpa_datetime_gettime(&m_cTime);

    if ( sRealTime != NULL )
    {
        SWPA_TIME t;
        if ( 0 == swpa_datetime_tm2time(*sRealTime, &t) )
        {
            m_cTime = t;
        }
    }
}

/**
 * @brief 构造函数
 * 
 * @param [in] sTime : 用作初始化该对象的时间结构体
 */
CSWDateTime::CSWDateTime(const SWPA_TIME& sTime){

    m_cTime.sec = sTime.sec;
    m_cTime.msec = sTime.msec;
}

/**
 * @brief 构造函数,此函数为相对时间转换绝对时间
 * @param [in] dwTick 相对时间
 */
CSWDateTime::CSWDateTime(const DWORD dwTick)
{
	LONG lTick = GetSystemTick() - dwTick;
	
	swpa_datetime_gettime(&m_cTime);
	
	DWORD dwTickMsec = lTick % 1000;
	if( dwTickMsec > m_cTime.msec ) 
	{
		m_cTime.sec -= 1;
		m_cTime.msec = (m_cTime.msec + 1000 - dwTickMsec);
	}
	else
	{
		m_cTime.msec -= dwTickMsec;
	}
	m_cTime.sec -= (lTick/1000);
}

/**
 * @brief 构造函数，用当前系统时间初始化的函数
 */
CSWDateTime::CSWDateTime(){

    swpa_datetime_gettime(&m_cTime);
}

/**
 * @brief 析构函数
 */
CSWDateTime::~CSWDateTime(){

}

/**
 * @brief 时间格式转换
 * 
 * @param [in] szFmt : 时间转换的格式化字符串
 * @return
 * - 转换后的时间CSWString串
 */
CSWString CSWDateTime::Format(const char* szFmt){

    //todo:
    CSWString t;
	return t;
}

/**
 * @brief 获取对象保存的时间值
 * 
 * @param [out] psTime : 获取到的时间
 * @return
 * - S_OK : 成功
 * - E_FAIL : 获取失败
 */
HRESULT CSWDateTime::GetTime(SWPA_TIME* psTime){

  *psTime = m_cTime;
	return S_OK;
}

/**
 * @brief 获取对象保存的时间值
 * 
 * @param [out] psRealTime : 时间结构体
 * @return
 * - S_OK : 成功
 * - E_FAIL : 获取失败
 */
HRESULT CSWDateTime::GetTime(SWPA_DATETIME_TM* psRealTime){

	SWPA_DATETIME_TM tm;
  if ( 0 == swpa_datetime_time2tm(m_cTime, &tm) )
  {
      *psRealTime = tm;
      return S_OK;
  }
	return E_FAIL;
}

/**
* @brief 获取年
*/
INT CSWDateTime::GetYear(){
    
    SWPA_DATETIME_TM sRealTime;
    if ( S_OK == GetTime(&sRealTime) )
    {
        return sRealTime.year;
    }

    return -1;
}

/**
* @brief 获取月
*/
INT CSWDateTime::GetMonth(){

    SWPA_DATETIME_TM sRealTime;
    if ( S_OK == GetTime(&sRealTime) )
    {
        return sRealTime.month;
    }

    return -1;
}

/**
* @brief 获取日
*/
INT CSWDateTime::GetDay(){

    SWPA_DATETIME_TM sRealTime;
    if ( S_OK == GetTime(&sRealTime) )
    {
        return sRealTime.day;
    }

    return -1;
}


/**
* @brief 获取星期
*/
INT CSWDateTime::GetWeekday(){

    SWPA_DATETIME_TM sRealTime;
    if ( S_OK == GetTime(&sRealTime) )
    {
        return sRealTime.weeknum;
    }

    return -1;
}


/**
* @brief 获取时
*/
INT CSWDateTime::GetHour(){

    SWPA_DATETIME_TM sRealTime;
    if ( S_OK == GetTime(&sRealTime) )
    {
        return sRealTime.hour;
    }

    return -1;
}

/**
* @brief 获取分
*/
INT CSWDateTime::GetMinute(){

    SWPA_DATETIME_TM sRealTime;
    if ( S_OK == GetTime(&sRealTime) )
    {
        return sRealTime.min;
    }

    return -1;
}

/**
* @brief 获取秒
*/
INT CSWDateTime::GetSecond(){

    SWPA_DATETIME_TM sRealTime;
    if ( S_OK == GetTime(&sRealTime) )
    {
        return sRealTime.sec;
    }

    return -1;
}

/**
* @brief 获取毫秒
*/
INT CSWDateTime::GetMSSecond(){

    SWPA_DATETIME_TM sRealTime;
    if ( S_OK == GetTime(&sRealTime) )
    {
        return sRealTime.msec;
    }

    return -1;
}

/**
 * @brief 设置对象的时间
 * 
 * @param [in] sTime : 时间值结构体
 * @param [in] fSetSysTime : 是否需要同步设置系统时钟，默认为真
 * @return
 * - S_OK  : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWDateTime::SetTime(const SWPA_TIME& sTime, const BOOL fSetSysTime){

	m_cTime = sTime;
    if (fSetSysTime)
   	{
		return (0 == swpa_datetime_settime(&sTime)) ? S_OK : E_FAIL;
    }
	return S_OK;
}

/**
 * @brief 设置对象的时间
 * 
 * @param [in] sRealTime : 时间结构体
 * @param [in] fSetSysTime : 是否需要同步设置系统时钟，默认为真
 * @return
 * - S_OK  : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWDateTime::SetTime(const SWPA_DATETIME_TM& sRealTime, const BOOL fSetSysTime){

    SWPA_TIME t;
    if ( 0 == swpa_datetime_tm2time(sRealTime, &t) )
    {
        return SetTime(t, fSetSysTime);
    }
	return E_FAIL;
}



CSWDateTime CSWDateTime::operator+(const CSWDateTime&  time){

    CSWDateTime cDateTimeResult;
    cDateTimeResult = m_cTime;
    cDateTimeResult += time;
    return cDateTimeResult;
}

CSWDateTime CSWDateTime::operator+(const SWPA_TIME& sTime){

    CSWDateTime cDateTimeResult;
    cDateTimeResult = m_cTime;
    cDateTimeResult += sTime;
    return cDateTimeResult;
}

CSWTimeSpan CSWDateTime::operator-(const CSWDateTime&  time){

    CSWTimeSpan cTimeSpanResult;
    CSWTimeSpan cTimeSpan;
    cTimeSpanResult.m_cTime = m_cTime;
    cTimeSpan.m_cTime = time.m_cTime;
    cTimeSpanResult -= cTimeSpan;
	return cTimeSpanResult;
}

CSWTimeSpan CSWDateTime::operator-(const SWPA_TIME& sTime){

	CSWTimeSpan cTimeSpan;
    CSWDateTime cDateTime(sTime);
    cTimeSpan = (*this) - cDateTime;
	return cTimeSpan;
}

CSWDateTime& CSWDateTime::operator=(const SWPA_DATETIME_TM& sRealTime){

    SWPA_TIME t;
    if ( 0 == swpa_datetime_tm2time(sRealTime, &t) )
    {
        m_cTime = t;
    }

	return (*this);
}

CSWDateTime& CSWDateTime::operator=(const SWPA_TIME& sTime){

    m_cTime = sTime;
	return (*this);
}

CSWDateTime& CSWDateTime::operator-=(const CSWTimeSpan&  Span){

    CSWTimeSpan cSpanTmp;
    CSWDateTime cTimeTmp;
    cTimeTmp.m_cTime = Span.m_cTime;
    cSpanTmp = (*this) - cTimeTmp;
    m_cTime = cSpanTmp.m_cTime;
	return (*this);
}


CSWDateTime& CSWDateTime::operator-=(const SWPA_TIME& sTime){

	if (m_cTime.sec * 1000 + m_cTime.msec >= sTime.sec * 1000 + sTime.msec)
	{
		m_cTime.sec -= sTime.sec;
	    m_cTime.msec -= sTime.msec;
	    if ( m_cTime.msec < 0 )
	    {
	        m_cTime.sec -= 1;
	        m_cTime.msec += 1000;
	    }
	}
	else
	{
		SW_TRACE_DEBUG("Err: Time operation overflowed!!\n");
	}
	
	return (*this);
}


CSWDateTime& CSWDateTime::operator+=(const CSWTimeSpan&  Span){

    m_cTime.sec += Span.m_cTime.sec;
    m_cTime.msec += Span.m_cTime.msec;
    if ( m_cTime.msec >= 1000 )
    {
        m_cTime.sec += 1;
        m_cTime.msec -= 1000;
    }
	return (*this);
}

CSWDateTime& CSWDateTime::operator+=(const CSWDateTime&  DateTime){

    m_cTime.sec += DateTime.m_cTime.sec;
    m_cTime.msec += DateTime.m_cTime.msec;
    if ( m_cTime.msec >= 1000 )
    {
        m_cTime.sec += 1;
        m_cTime.msec -= 1000;
    }
	return (*this);
}

CSWDateTime& CSWDateTime::operator+=(const SWPA_TIME& sTime){

    m_cTime.sec += sTime.sec;
    m_cTime.msec += sTime.msec;
    if ( m_cTime.msec >= 1000 )
    {
        m_cTime.sec += 1;
        m_cTime.msec -= 1000;
    }
	return (*this);
}

BOOL CSWDateTime::operator!=(const CSWDateTime&  DateTime){

    if ( m_cTime.sec != DateTime.m_cTime.sec
        || m_cTime.msec != DateTime.m_cTime.msec )
    {
        return TRUE;
    }
	return FALSE;
}

BOOL CSWDateTime::operator<(const CSWDateTime& DateTime){

    if ( m_cTime.sec < DateTime.m_cTime.sec )
    {
        return TRUE;
    }
    else if ( m_cTime.sec == DateTime.m_cTime.sec )
    {
        if ( m_cTime.msec < DateTime.m_cTime.msec )
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}

BOOL CSWDateTime::operator==(const CSWDateTime&  DateTime){

	if ( m_cTime.sec == DateTime.m_cTime.sec
        && m_cTime.msec == DateTime.m_cTime.msec )
    {
        return TRUE;
    }
	return FALSE;
}

BOOL CSWDateTime::operator>(const CSWDateTime&  DateTime){

	if ( m_cTime.sec > DateTime.m_cTime.sec )
    {
        return TRUE;
    }
    else if ( m_cTime.sec == DateTime.m_cTime.sec )
    {
        if ( m_cTime.msec > DateTime.m_cTime.msec )
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}

/**
 * @brief 设置对象保存的时区
 * 
 * @param [in] dwTimeZone : 时区值
 * @return
 * - S_OK : 成功
 * - E_FAIL : 获取失败
 */
HRESULT CSWDateTime::SetTimeZone(const DWORD dwTimeZone){

	if ( 0 == swpa_datetime_settimezone(dwTimeZone) )
    {
	    return S_OK;
    }
    return E_FAIL;
}

/**
 * @brief 获取对象保存的时区
 * 
 * @return
 * - 获取到的时区值
 */
DWORD CSWDateTime::GetTimeZone(){

	return swpa_datetime_gettimezone();
}

/**
 * @brief 获取系统时钟tick数
 * 
 * @return
 * - 系统时钟tick数
 */
DWORD CSWDateTime::GetSystemTick(){

    return swpa_datetime_gettick();
}


HRESULT CSWDateTime::TimeConvert(const DWORD dwRefTime, DWORD* pdwTimeHigh, DWORD* pdwTimeLow)
{
	if (NULL == pdwTimeHigh || NULL == pdwTimeLow)
	{
		//PRINT("Err: NULL == pdwTimeHigh || NULL == pdwTimeLow\n");
		return E_INVALIDARG;
	}

	CSWDateTime cTime(dwRefTime);
	SWPA_TIME sTime;
	cTime.GetTime(&sTime);

	UInt64 ui64Time = sTime.sec;
	ui64Time *= 1000;
	ui64Time += sTime.msec;
	
	*pdwTimeHigh =  (DWORD)(ui64Time >> 32);
	*pdwTimeLow = (DWORD)(( ui64Time << 32 ) >> 32);
	
	return S_OK;
}


HRESULT CSWDateTime::TimeConvert(const SWPA_TIME& sTime, DWORD* pdwTimeHigh, DWORD* pdwTimeLow)
{
	if (NULL == pdwTimeHigh || NULL == pdwTimeLow)
	{
		//PRINT("Err: NULL == pdwTimeHigh || NULL == pdwTimeLow\n");
		return E_INVALIDARG;
	}

	UInt64 ui64Time = (UInt64)sTime.sec * 1000 + (UInt64)sTime.msec;

	
	*pdwTimeHigh =  (DWORD)(ui64Time >> 32);
	*pdwTimeLow = (DWORD)(ui64Time & 0xFFFFFFFF);
	
	return S_OK;
}



HRESULT CSWDateTime::TimeConvert(const DWORD dwTimeHigh,  const DWORD dwTimeLow, SWPA_TIME* psTime)
{
	if (NULL == psTime)
	{
		//PRINT("Err: NULL == pdwTimeHigh || NULL == pdwTimeLow\n");
		return E_INVALIDARG;
	}

	UInt64 ui64Time = ((UInt64)dwTimeHigh)<<32 | ((UInt64)dwTimeLow);

	psTime->sec = ui64Time / 1000;
	psTime->msec = ui64Time % 1000;
	
	return S_OK;
}


