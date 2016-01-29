///////////////////////////////////////////////////////////
//  CSWThread.h
//  Implementation of the Class CSWThread
//  Created on:      28-二月-2013 14:09:46
//  Original author: zy
///////////////////////////////////////////////////////////

#if !defined(EA_F011E105_F2B1_4c85_A41B_DA0A82F78B7A__INCLUDED_)
#define EA_F011E105_F2B1_4c85_A41B_DA0A82F78B7A__INCLUDED_

#include "SWObject.h"
#include "swpa_thread.h"

typedef VOID* (*START_ROUTINE)(VOID*);

/**
 * @brief 线程管理基类。提供线程运行和管理的基本函数，所有线程类需继承该类。
 */
class CSWThread : public CSWObject
{
CLASSINFO(CSWThread,CSWObject)
public:
	CSWThread();
	/**
	 * @brief 构造函数
	 * @param  [in] szName : 子线程名
	 * @param [in] dwStackSize: 线程的栈大小
	 * @param [in] dwPriority: 线程优先级
	 */
	CSWThread(PCSTR szName, DWORD dwStackSize = NORMAL_STACK_SIZE, DWORD dwPriority = NORMAL_PRIORITY_LEVEL);
	/**
	 * @brief 析构函数
	 */
	virtual ~CSWThread();
	/**
	 * @brief 获取线程最新状态
	 * @param  [out] dwStatus : 状态码
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
	virtual HRESULT GetLastStatus(DWORD& dwStatus);
	/**
	 * @brief 创建并启动线程
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
	virtual HRESULT Start();
	/**
	 * @brief 创建并启动线程,执行传入的线程主函数
	 * 
	 * @param [in] pfRoutine : 线程主函数
	 * @param [in] pvArg : 线程主函数的参数
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 * @note
	 * - START_ROUTINE的定义为 typedef VOID * (*START_ROUTINE)(VOID *);
	 */
	virtual HRESULT Start(START_ROUTINE pfRoutine, PVOID pvArg);
	/**
	 * @brief 停止线程并退出
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
	virtual HRESULT Stop();

	/**
	 * @brief 判断线程是否有效，是否存活
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
	virtual HRESULT IsValid();

protected:
	/**
	 * @brief 线程主运行程序，各线程需重载该函数实现其功能。
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
	virtual HRESULT Run();

    BOOL m_fExit;

private:
	/**
	 * @brief 线程优先级
	 */
	DWORD m_dwPriority;
	/**
	 * @brief 线程栈大小，单位为字节。
	 */
	DWORD m_dwStackSize;

private:

	START_ROUTINE m_pfRoutine;
	PVOID m_pvArg;
	
#ifdef TEST_UNDER_WIN32
	HANDLE m_hThread;
#else
    int m_hThread;
#endif
    
    static LPVOID ThreadFunc(LPVOID pArg);

static const DWORD NORMAL_STACK_SIZE = 64 * 1024;
static const DWORD NORMAL_PRIORITY_LEVEL = 0;

};
#endif // !defined(EA_F011E105_F2B1_4c85_A41B_DA0A82F78B7A__INCLUDED_)

