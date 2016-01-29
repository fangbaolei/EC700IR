///////////////////////////////////////////////////////////
//  CSWDateTime.h
//  Implementation of the Class CSWDateTime
//  Created on:      28-二月-2013 14:09:50
//  Original author: zhouy
///////////////////////////////////////////////////////////

#if !defined(EA_607047C4_27B7_41dc_9CAC_5B645882FA98__INCLUDED_)
#define EA_607047C4_27B7_41dc_9CAC_5B645882FA98__INCLUDED_

#include "SWString.h"
#include "SWTimeSpan.h"
#include "SWObject.h"
#include "swpa_datetime.h"


/**
 * @brief 日期时间管理基类
 */
class CSWDateTime : public CSWObject
{
CLASSINFO(CSWDateTime,CSWObject);

public:
	/**
	 * @brief 构造函数
	 * 
	 * @param [in] sRealTime : 用作初始化该对象的实时结构体
	 */
	CSWDateTime(const SWPA_DATETIME_TM* sRealTime);
	/**
	 * @brief 构造函数
	 * 
	 * @param [in] sTime : 用作初始化该对象的时间结构体
	 * @note
	 * - SWPA_TIME 的定义为：typedef struct _TIME_STRUCT {DWORD dwSec; DWORD dwMSec }
	 * SWPA_TIME ;
	 */
	CSWDateTime(const SWPA_TIME& sTime);
	
	/**
	 * @brief 构造函数,此函数为相对时间转换绝对时间
	 * @param [in] dwTick 相对时间
	 */
	CSWDateTime(const DWORD dwTick);	
	/**
	 * @brief 构造函数，用当前系统时间初始化的函数
	 */
	CSWDateTime();
	/**
	 * @brief 析构函数
	 */
	virtual ~CSWDateTime();
	/**
	 * @brief 时间格式转换
	 * 
	 * @param [in] szFmt :  时间转换的格式化字符串
	 * @return
	 * - 转换后的时间CSWString串
	 */
	CSWString Format(const char* szFmt);
	
	/**
	 * @brief 获取对象保存的时间值
	 * 
	 * @param [out] psTime : 获取到的时间
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 获取失败
	 */
	HRESULT GetTime(SWPA_TIME* psTime);
	/**
	 * @brief 获取对象保存的时间值
	 * 
	 * @param [out] psRealTime : 时间结构体
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 获取失败
	 */
	HRESULT GetTime(SWPA_DATETIME_TM* psRealTime);

    /**
	 * @brief 获取年
	 */
    INT GetYear();

    /**
	 * @brief 获取月
	 */
    INT GetMonth();

    /**
	 * @brief 获取日
	 */
    INT GetDay();

	INT GetWeekday();

    /**
	 * @brief 获取时
	 */
    INT GetHour();

    /**
	 * @brief 获取分
	 */
    INT GetMinute();

    /**
	 * @brief 获取秒
	 */
    INT GetSecond();
    
    /**
	 * @brief 获取毫秒
	 */
    INT GetMSSecond();

	/**
	 * @brief 设置对象的时间
	 * 
	 * @param [in] sTime : 时间值结构体
	 * @param [in] fSetSysTime : 是否需要同步设置系统时钟，默认为真。
	 * @return
	 * - S_OK    : 成功
	 * - E_FAIL : 失败
	 */
	HRESULT SetTime(const SWPA_TIME& sTime, const BOOL fSetSysTime=TRUE);
	/**
	 * @brief 设置对象的时间
	 * 
	 * @param [in] sRealTime : 时间结构体
	 * @param [in] fSetSysTime : 是否需要同步设置系统时钟，默认为真
	 * @return
	 * - S_OK    : 成功
	 * - E_FAIL : 失败
	 */
	HRESULT SetTime(const SWPA_DATETIME_TM& sRealTime, const BOOL fSetSysTime=TRUE);

    CSWDateTime operator+(const CSWDateTime& time);
    CSWDateTime operator+(const SWPA_TIME& sTime);
    CSWTimeSpan operator-(const CSWDateTime& time);
	CSWTimeSpan operator-(const SWPA_TIME& sTime);

    CSWDateTime& operator=(const SWPA_DATETIME_TM& sRealTime);
	CSWDateTime& operator=(const SWPA_TIME& sTime);
	CSWDateTime& operator-=(const CSWTimeSpan& Span);
	CSWDateTime& operator-=(const SWPA_TIME& sTime);
    CSWDateTime& operator+=(const CSWTimeSpan& Span);
    CSWDateTime& operator+=(const CSWDateTime& DateTime);
    CSWDateTime& operator+=(const SWPA_TIME& sTime);

	BOOL operator!=(const CSWDateTime& DateTime);
	BOOL operator<(const CSWDateTime& DateTime);
	BOOL operator==(const CSWDateTime& DateTime);
	BOOL operator>(const CSWDateTime& DateTime);

	/**
	 * @brief 设置对象保存的时区
	 * 
	 * @param [in] dwTimeZone : 时区值
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 获取失败
	 */
	static HRESULT SetTimeZone(const DWORD dwTimeZone);

    /**
	 * @brief 获取对象保存的时区
	 * 
	 * @return
	 * - 获取到的时区值
	 */
	static DWORD GetTimeZone();

    /**
	 * @brief 获取系统时钟tick数
	 * 
	 * @return
	 * - 系统时钟tick数
	 */
	static DWORD GetSystemTick();

	
	static HRESULT TimeConvert(const DWORD dwRefTime, DWORD* pdwTimeHigh, DWORD* pdwTimeLow);

	static HRESULT TimeConvert(const SWPA_TIME& sTime, DWORD* pdwTimeHigh, DWORD* pdwTimeLow);

	static HRESULT TimeConvert(const DWORD dwTimeHigh,  const DWORD dwTimeLow, SWPA_TIME* psTime);

private:
    SWPA_TIME m_cTime;
};

#endif // !defined(EA_607047C4_27B7_41dc_9CAC_5B645882FA98__INCLUDED_)

