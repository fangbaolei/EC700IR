///////////////////////////////////////////////////////////
// CSWTimeSpan.cpp
// Implementation of the Class CSWTimeSpan
// Created on:   28-二月-2013 14:09:52
// Original author: zhouy
///////////////////////////////////////////////////////////

#include "SWTimeSpan.h"

/**
 * @brief 构造函数
 */
CSWTimeSpan::CSWTimeSpan(){
	//swpa_memset(&m_cDateTime, 0, sizeof(m_cDateTime));
	swpa_memset(&m_cTime, 0, sizeof(m_cTime));
}

/**
 * @brief 析构函数
 */
CSWTimeSpan::~CSWTimeSpan(){

}

/**
 * @brief 时间格式转换
 * 
 * @param [in] szFmt : 时间转换的格式化字符串
 * @return
 * - 转换后的时间CSWString串
 */
CSWString CSWTimeSpan::Format(char* szFmt){

    //todo:
    CSWString t;
	return t;
}

/**
 * @brief 返回此CSWTimeSpan对象中的完整的天数
 * 
 * @return
 * - 完整的天数
 */
INT CSWTimeSpan::GetDays(){

	return m_cTime.sec / 3600 / 24;
}

/**
 * @brief 返回此CSWTimeSpan对象中的小时数
 * 
 * @return
 * - 完整的小时数
 */
INT CSWTimeSpan::GetHours(){

	return m_cTime.sec / 3600;
}

/**
 * @brief 返回此CSWTimeSpan对象中的分钟数
 * 
 * @return
 * - 分钟数
 */
INT CSWTimeSpan::GetMinutes(){

	return m_cTime.sec / 60;
}

/**
 * @brief 返回此CSWTimeSpan对象中的秒数
 * 
 * @return
 * - 秒数
 */
INT CSWTimeSpan::GetSeconds(){


	return m_cTime.sec;
}


/**
 * @brief 返回此CSWTimeSpan对象中的毫秒数
 * 
 * @return
 * - 毫秒数
 */
INT CSWTimeSpan::GetMSeconds(){

	INT iMsec = m_cTime.sec * 1000 + m_cTime.msec;
	return iMsec >= 0 ? iMsec : -1;
}


/**
 * @brief 重载两个CSWTimeSpan之间的减法运算
 * 
 * @param [in] Span 减去的对象
 * @return
 * - 被减对象的引用
 */
CSWTimeSpan& CSWTimeSpan::operator-=(CSWTimeSpan& Span){

    if ( m_cTime.sec > Span.m_cTime.sec )
    {
        if ( m_cTime.msec >= Span.m_cTime.msec )
        {
            m_cTime.sec = m_cTime.sec - Span.m_cTime.sec;
            m_cTime.msec = m_cTime.msec - Span.m_cTime.msec;
        }
        else
        {
            m_cTime.sec = m_cTime.sec - 1 - Span.m_cTime.sec;
            m_cTime.msec = m_cTime.msec + 1000 - Span.m_cTime.msec;
        }
    }
    else if ( m_cTime.sec == Span.m_cTime.sec )
    {
        m_cTime.sec = 0;

        if ( m_cTime.msec >= Span.m_cTime.msec )
        {
            m_cTime.msec = m_cTime.msec - Span.m_cTime.msec;
        }
        else
        {
            m_cTime.msec = Span.m_cTime.msec - m_cTime.msec;
        }
    }
    else
    {
        if ( Span.m_cTime.msec >= m_cTime.msec )
        {
            m_cTime.sec = Span.m_cTime.sec - m_cTime.sec;
            m_cTime.msec = Span.m_cTime.msec - m_cTime.msec;
        }
        else
        {
            m_cTime.sec = Span.m_cTime.sec - 1 - m_cTime.sec;
            m_cTime.msec = Span.m_cTime.msec + 1000 - m_cTime.msec;
        }
    }

	return (*this);
}

/**
 * @brief 重载两个CSWTimeSpan之间的加法运算
 * 
 * @param [in] Span 加上的对象
 * @return
 * - 被加对象的引用
 */
CSWTimeSpan& CSWTimeSpan::operator+=(CSWTimeSpan& Span){

    m_cTime.sec += Span.m_cTime.sec;
    m_cTime.msec += Span.m_cTime.msec;
    if ( m_cTime.msec >= 1000 )
    {
        m_cTime.sec += 1;
        m_cTime.msec -= 1000;
    }

	return (*this);
}

/**
 * @brief 重载两个CSWTimeSpan之间的不等于运算
 * 
 * @param [in] Span 用作不等于比较的对象
 * @return
 * - TRUE : 该对象与Span对象两者的Time Span值不相等
 * - FALSE : 该对象与Span对象两者的Time Span值相等
 */
BOOL CSWTimeSpan::operator!=(CSWTimeSpan& Span){

    if ( m_cTime.sec != Span.m_cTime.sec
        || m_cTime.msec != Span.m_cTime.msec )
    {
        return TRUE;
    }
	return FALSE;
}

/**
 * @brief 重载两个CSWTimeSpan之间的赋值运算
 * 
 * @param [in] Span : 赋值的对象
 * @return
 * - 被赋值对象的引用
 */
CSWTimeSpan& CSWTimeSpan::operator=(CSWTimeSpan Span){

    m_cTime.sec = Span.m_cTime.sec;
    m_cTime.msec = Span.m_cTime.msec;
	return (*this);
}

/**
 * @brief 重载两个CSWTimeSpan之间的等于运算
 * 
 * @param [in] Span 用作等于比较的对象
 * @return
 * - TRUE : 该对象与Span对象两者的Time Span值相等
 * - FALSE : 该对象与Span对象两者的Time Span值不相等
 */
BOOL CSWTimeSpan::operator==(CSWTimeSpan& Span){

	if ( m_cTime.sec == Span.m_cTime.sec
        && m_cTime.msec == Span.m_cTime.msec )
    {
        return TRUE;
    }
	return FALSE;
}

