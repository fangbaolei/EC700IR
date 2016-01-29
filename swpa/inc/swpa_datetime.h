/**
* @file swpa_datetime.h
* @brief 日期时间管理
* @copyright Signalway All Rights Reserved
* @author Shaorg
* @date 2013-02-26
* @version v0.1
* @note 包含获取以及操作日期时间的相关函数。
*/

#ifndef _SWPA_DATETIME_H_
#define _SWPA_DATETIME_H_

/**
* @brief 时间结构体
* @see swpa_datetime.h
*/
typedef struct _SWPA_TIME
{
    unsigned int sec; /**< 从1970-01-01 00:00:00开始至目前所经过的秒数 */
    unsigned int msec; /**< 余下的当前毫秒数 */
} SWPA_TIME;

typedef struct _SWPA_DATETIME_TM
{
    short msec;
    short year;
    short month;
    short day;
    short hour;
    short min;
    short sec;
    short weeknum;
} SWPA_DATETIME_TM;

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief 获取系统当前的时钟滴答数
* 
* @retval tick 时钟滴答
* @see swpa_datetime.h
*/
unsigned long swpa_datetime_gettick(void);

/**
* @brief 获取当前时区
*
* @retval 当前时区
* @see swpa_datetime.h
* @note 该时区为一整数值。例如：8则表示+8时区，即北京时间。
*/
int swpa_datetime_gettimezone(void);

/**
* @brief 设置当前时区
* @param [in] tz 当前时区
* @retval 0 成功
* @retval -1 失败
* @see swpa_datetime.h
* @note 该时区为一整数值。例如：8则表示+8时区，即北京时间。
*/
int swpa_datetime_settimezone(int tz);

/**
* @brief 获取时间
* @param [out] t 时间结构体
* @retval 0 成功
* @retval -1 失败
* @see swpa_datetime.h
* @note 该时间结构体包含从1970-01-01 00:00:00开始至目前所经过的秒数以及余下的当前毫秒数。注：与当前所处时区相关。
*/
int swpa_datetime_gettime(struct _SWPA_TIME* t);

/**
* @brief 设置时间
* @param [in] t 时间结构体
* @retval 0 成功
* @retval -1 失败
* @see swpa_datetime.h
* @note 该时间结构体包含从1970-01-01 00:00:00开始至目前所经过的秒数以及余下的当前毫秒数。注：与当前所处时区相关。
*/
int swpa_datetime_settime(const struct _SWPA_TIME* t);

/**
* @brief 将秒数时间转为日期时间
* @param [in] t 时间结构体
* @param [out] tm 日期时间
* @retval 0 成功
* @retval -1 失败
* @see swpa_datetime.h
*/
int swpa_datetime_time2tm(struct _SWPA_TIME t, struct _SWPA_DATETIME_TM* tm);

/**
* @brief 将日期时间转为秒数时间
* @param [in] tm 日期时间
* @param [out] t 时间结构体
* @retval 0 成功
* @retval -1 失败
* @see swpa_datetime.h
*/
int swpa_datetime_tm2time(struct _SWPA_DATETIME_TM tm, struct _SWPA_TIME* t);

#ifdef __cplusplus
}
#endif

#endif

