///////////////////////////////////////////////////////////
//  CSWTimeSpan.h
//  Implementation of the Class CSWTimeSpan
//  Created on:      28-二月-2013 14:09:52
//  Original author: zhouy
///////////////////////////////////////////////////////////

#if !defined(EA_84143B1B_B80C_41a0_BD8B_7B06060B1CEE__INCLUDED_)
#define EA_84143B1B_B80C_41a0_BD8B_7B06060B1CEE__INCLUDED_

#include "SWString.h"

/**
 * @brief 时间段基类，用作日期时间的加减运算
 */
class CSWTimeSpan : public CSWObject
{
CLASSINFO(CSWTimeSpan,CSWObject)

public:
	/**
	 * @brief 构造函数
	 */
	CSWTimeSpan();
	/**
	 * @brief 析构函数
	 */
	virtual ~CSWTimeSpan();

	/**
	 * @brief 时间格式转换
	 * 
	 * @param [in] szFmt :  时间转换的格式化字符串
	 * @return
	 * - 转换后的时间CSWString串
	 */
	CSWString Format(char* szFmt);
	/**
	 * @brief 返回此CSWTimeSpan对象中的完整的天数
	 * 
	 * @return
	 * - 完整的天数
	 */
	INT GetDays();
	/**
	 * @brief 返回此CSWTimeSpan对象中的小时数
	 * 
	 * @return
	 * - 完整的小时数
	 */
	INT GetHours();
	/**
	 * @brief 返回此CSWTimeSpan对象中的分钟数
	 * 
	 * @return
	 * - 分钟数
	 */
	INT GetMinutes();
	/**
	 * @brief 返回此CSWTimeSpan对象中的秒数
	 * 
	 * @return
	 * - 秒数
	 */
	INT GetSeconds();

	/**
	 * @brief 返回此CSWTimeSpan对象中的毫秒数
	 * 
	 * @return
	 * - 毫秒数，若为负数则说明溢出!
	 */
	INT GetMSeconds();

	/**
	 * @brief 重载两个CSWTimeSpan之间的减法运算
	 * 
	 * @param [in] Span 减去的对象
	 * @return
	 * - 被减对象的引用
	 */
	CSWTimeSpan& operator-=(CSWTimeSpan& Span);

    /**
	 * @brief 重载两个CSWTimeSpan之间的加法运算
	 * 
	 * @param [in] Span 加上的对象
	 * @return
	 * - 被加对象的引用
	 */
	CSWTimeSpan& operator+=(CSWTimeSpan& Span);

	/**
	 * @brief 重载两个CSWTimeSpan之间的不等于运算
	 * 
	 * @param [in] Span 用作不等于比较的对象
	 * @return
	 * - TRUE : 该对象与Span对象两者的Time Span值不相等
	 * - FALSE : 该对象与Span对象两者的Time Span值相等
	 */
	BOOL operator!=(CSWTimeSpan& Span);
	
	/**
	 * @brief 重载两个CSWTimeSpan之间的赋值运算
	 * 
	 * @param [in] Span : 赋值的对象
	 * @return
	 * - 被赋值对象的引用
	 */
	CSWTimeSpan& operator=(CSWTimeSpan Span);

	/**
	 * @brief 重载两个CSWTimeSpan之间的等于运算
	 * 
	 * @param [in] Span 用作等于比较的对象
	 * @return
	 * - TRUE : 该对象与Span对象两者的Time Span值相等
	 * - FALSE : 该对象与Span对象两者的Time Span值不相等
	 */
	BOOL operator==(CSWTimeSpan& Span);

private:
    SWPA_TIME m_cTime;
    //SWPA_DATETIME_TM m_cDateTime;

    friend class CSWDateTime;
};

#endif // !defined(EA_84143B1B_B80C_41a0_BD8B_7B06060B1CEE__INCLUDED_)

