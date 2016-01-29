/**
* @file swpa_thread.h
* @brief 线程管理
* @author Shaorg
* @date 2013-02-26
* @version v0.1
* @note 包含线程管理的一组函数。
*/

#ifndef _SWPA_THREAD_H_
#define _SWPA_THREAD_H_

#ifdef __cplusplus
extern "C"
{
#define SWPA_THREAD_DEFAULT(var) =var
#else	
#define SWPA_THREAD_DEFAULT(var)
#endif

/**
* @brief 创建线程
* @param [out] thread 线程结构体指针
* @param [in] stacksize 线程栈大小（字节）
* @param [in] priority 线程优先级，目前无效,默认线程优先级都是0.
* @param [in] start_routine 线程函数。注，该函数的参数即为arg
* @param [in] arg 线程函数参数
* @retval 0 成功
* @retval -1 失败
* @see swpa_thread.h
*/
extern int swpa_thread_create(
    int* thread,
    const int stacksize,
	const int priority,
    void* (*start_routine)(void*),
    void* arg
);





/**
* @brief 分离线程
* @param [in] thread 线程结构体指针地址
* @retval 0 成功
* @retval -1 失败
* @see swpa_thread.h
*/
extern int swpa_thread_detach(int thread);




/**
* @brief 销毁线程
* @param [in] thread 线程结构体指针地址
* @retval 0 成功
* @retval -1 失败
* @see swpa_thread.h
*/
extern int swpa_thread_destroy(int thread);



/**
* @brief 退出当前线程
* @param [out] retval 线程退出码
* @retval 0 成功
* @retval -1 失败
* @see swpa_thread.h
* @note 该函数等价于在线程函数中使用return函数。
*/
extern int swpa_thread_exit(void* retval);

/**
* @brief 等待线程退出
* @param [in] thread 线程结构体指针地址
* @param [out] retval 线程函数返回值
* @retval 0 成功
* @retval -1 失败
* @see swpa_thread.h
*/
extern int swpa_thread_join(int thread, void** retval);

/**
* @brief 取消线程执行
* @param [in] thread 线程结构体
* @retval 0 成功
* @retval -1 失败
* @see swpa_thread.h
*/
extern int swpa_thread_cancel(int thread);

/**
* @brief 获取线程ID号
* @param [in] thread 线程结构体指针地址
* @param [out] id 线程ID号
* @retval 0 成功
* @retval -1 失败
* @see swpa_thread.h
*/
extern int swpa_thread_getid(int thread, int* id);

/**
* @brief 线程休眠
* @param [in] ms 具体休眠时间。单位：毫秒
* @retval 无
* @see swpa_thread.h
*/
extern void swpa_thread_sleep_ms(unsigned int ms);

/**
* @brief 创建进程
* @param [out] process 进程结构体
* @param [in] attr 进程属性
* @param [in] filename 可执行文件名
* @param [in] cmd 命令行参数
* @retval 0 成功
* @retval -1 失败
* @see swpa_thread.h
*/
extern int swpa_process_create(
    int* process,
    const int* attr,
    char* filename,
    char* cmdv[]
);

/**
* @brief 终止进程
* @param [in] process 进程结构体
* @retval 0 成功
* @retval -1 失败
* @see swpa_thread.h
*/
extern int swpa_process_kill(int process);

/**
* @brief 等待进程
* @param [in] process 进程结构体
* @param [in] timeout 等待超时。单位：毫秒
* @param [out] ikill_code 程序被kill的退出码
* @param [out] exit_code 程序正常退出的退出码
* @retval 0 成功
* @retval -1 失败
* @see swpa_thread.h
* @note 当timeout为-1时，表示阻塞等待；当timeout大于0时，即表示超时等待。
*/
extern int swpa_process_wait(
	int process, 
	int timeout, 
	int* ikill_code SWPA_THREAD_DEFAULT(0), 
	int *exit_code SWPA_THREAD_DEFAULT(0)
);

#ifdef __cplusplus
}
#endif

#endif

