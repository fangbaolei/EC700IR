/**
 * @file   drv_log.h
 * @author lujy
 * @date   2013-11-12
 *
 * @brief  写日志接口
 *
 * @note   本接口调用syslog将日志写入系统日志文件中(/var/log/messages和/var/log/syslog)\n
 *         注意：为了保证每一行字符串前都有日期等信息，调用一次drv_log只能写一行字符串\n
 *
 * 
 */

#ifndef DRV_LOG_H
#define DRV_LOG_H

#include <syslog.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 打印日志函数
 *
 * @param log_src [in]   : 日志源
 * @param log_level [in] : 日志等级
 * @param fmt [in]       : 可变参数
 * @param args [in]      : 可变参数，可无
 *
 * @note 等效于下面的函数：
 * void drv_log(const char* log_src, int log_level, const char *fmt, ...)
 */
#define drv_log(log_src, log_level, fmt, args...)  \
{ \
    syslog(LOG_USER|LOG_NOTICE, "| %d | %s | " fmt, log_level, log_src, ##args); \
}

#ifdef __cplusplus
}
#endif

#endif // DRV_LOG_H
