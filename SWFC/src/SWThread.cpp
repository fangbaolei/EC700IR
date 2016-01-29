///////////////////////////////////////////////////////////
//  CSWThread.cpp
//  Implementation of the Class CSWThread
//  Created on:      28-二月-2013 14:09:46
//  Original author: zy
///////////////////////////////////////////////////////////

#include "SWThread.h"

typedef struct tga_THREAD_ATTR
{
    int stack_size;
    int priotiry;
}THREAD_ATTR_HANDLE;

LPVOID CSWThread::ThreadFunc(LPVOID pArg)
{
    CSWThread* pcThread = (CSWThread*)pArg;
    pcThread->m_fExit = FALSE;
    if(pcThread->m_pfRoutine)
    {
    	pcThread->m_pfRoutine(pcThread->m_pvArg);
    }
    else if(E_NOTIMPL == pcThread->Run())
    {
    	swpa_thread_sleep_ms(1000);
    }
    pcThread->m_fExit = TRUE;
    return NULL;
}

CSWThread::CSWThread(){

	m_pfRoutine = NULL;
	m_pvArg = NULL;
	
    m_dwPriority = NORMAL_PRIORITY_LEVEL;
	m_dwStackSize = NORMAL_STACK_SIZE;

    m_fExit = TRUE;
	m_hThread = 0;
}

/**
 * @brief 构造函数
 * @param  [in] szName : 子线程名
 * @param [in] dwStackSize: 线程的栈大小
 * @param [in] dwPriority: 线程优先级
 */
CSWThread::CSWThread(PCSTR szName, DWORD dwStackSize, DWORD dwPriority){

	m_pfRoutine = NULL;
	m_pvArg = NULL;
	
    m_dwPriority = dwPriority;
	m_dwStackSize = dwStackSize;

	m_fExit = TRUE;
	m_hThread = 0;
}


/**
 * @brief 析构函数
 */
CSWThread::~CSWThread(){

    Stop();	
}


/**
 * @brief 获取线程最新状态
 * @param  [out] dwStatus : 状态码
 * @return - S_OK : 成功 - E_FAIL : 失败
 */
HRESULT CSWThread::GetLastStatus(DWORD& dwStatus){

	return  E_NOTIMPL;
}


/**
 * @brief 判断线程是否有效，是否存活
 * @return - S_OK : 成功 - E_FAIL : 失败
 */
HRESULT CSWThread::IsValid(){

	return  m_fExit ? E_FAIL : S_OK;
}


/**
 * @brief 线程主运行程序，各线程需重载该函数实现其功能。
 * @return - S_OK : 成功 - E_FAIL : 失败
 */
HRESULT CSWThread::Run(){

	return  E_NOTIMPL;
}


/**
 * @brief 创建并启动线程
 * @return - S_OK : 成功 - E_FAIL : 失败
 */
HRESULT CSWThread::Start(){

	THREAD_ATTR_HANDLE cAttr;
	cAttr.priotiry = m_dwPriority;
	cAttr.stack_size = m_dwStackSize;

	if (0 != m_hThread)
	{
		return E_ACCESSDENIED;
	}
	
	m_fExit = FALSE;
	swpa_thread_create(&m_hThread, cAttr.stack_size, cAttr.priotiry, (LPVOID (*)(LPVOID))ThreadFunc, this);
	if ( 0 != m_hThread )
	{
	    return S_OK;
	}
	return E_FAIL;
}


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
HRESULT CSWThread::Start(START_ROUTINE pfRoutine, PVOID pvArg){

	THREAD_ATTR_HANDLE cAttr;
	cAttr.priotiry = m_dwPriority;
	cAttr.stack_size = m_dwStackSize;

	if (0 != m_hThread)
	{
		return E_ACCESSDENIED;
	}
	
	m_pfRoutine = pfRoutine;
	m_pvArg = pvArg;
	
	m_fExit = FALSE;
	swpa_thread_create(&m_hThread, cAttr.stack_size, cAttr.priotiry, (LPVOID (*)(LPVOID))ThreadFunc, this);
	if ( 0 != m_hThread )
	{
		return S_OK;
	}
	return E_FAIL;
}


/**
 * @brief 停止线程并退出
 * @return - S_OK : 成功 - E_FAIL : 失败
 */
HRESULT CSWThread::Stop(){

	m_fExit = TRUE;
	void* retval = NULL;
	
	if (0 != m_hThread)
	{
		swpa_thread_join(m_hThread, &retval);
		swpa_thread_destroy(m_hThread);
		m_hThread = 0;
	}
	return S_OK;
}

