/**
* @file swpa_timer.h
* @brief 定时器
* @author Shaorg
* @date 2013-02-26
* @version v0.1
* @note 包含定时器管理的一组函数。
*/

#ifndef _SWPA_TIMER_H_
#define _SWPA_TIMER_H_


#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief 创建定时器
* @param [out] timer 定时器结构体指针
* @param [in] timeout 定时时间。单位：毫秒
* @param [in] alarm_routine 定时器警报函数。注：该函数的参数即为“定时器唯一标识码”
* @retval 0 成功
* @retval -1 失败
* @see swpa_timer.h
*/
int swpa_timer_create(
    int* timer,
    unsigned int timeout,
    void (*alarm_routine)(int)
);

/**
* @brief 获取定时器唯一标识码
* @param [in] timer 定时器结构体指针
* @param [out] id 定时器唯一标识码
* @retval 0 成功
* @retval -1 失败
* @see swpa_timer.h
*/
int swpa_timer_getid(int timer, unsigned int* id);

/**
* @brief 删除定时器
* @param [in] timer 定时器结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_timer.h
*/
int swpa_timer_delete(int timer);

/**
* @brief 启动定时器
* @param [in] timer 定时器结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_timer.h
*/
int swpa_timer_start(int timer);

/**
* @brief 停止定时器
* @param [in] timer 定时器结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_timer.h
*/
int swpa_timer_stop(int timer);

/**
* @brief 重置定时器
* @param [in] timer 定时器结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_timer.h
*/
int swpa_timer_reset(int timer);

#ifdef __cplusplus
}
#endif

#endif

