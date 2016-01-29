///////////////////////////////////////////////////////////
//  CSWSemaphore.cpp
//  Implementation of the Class CSWSemaphore
//  Created on:      28-二月-2013 14:09:49
//  Original author: zhouy
///////////////////////////////////////////////////////////

#include "SWSemaphore.h"

CSWSemaphore::CSWSemaphore(){

    m_cSem = 0;
}

/**
 * @brief 构造函数
 * 
 * @param [in] dwInitCount : 初始数目，默认为1
 * @param [in] dwMaxCount : 最大数目，默认为1
 */
CSWSemaphore::CSWSemaphore(DWORD dwInitCount, DWORD dwMaxCount, DWORD dwSemId){

	m_cSem = 0;
	m_sem_id = dwSemId;
	if (dwSemId == 0)
	{
		/* 进程内的信号量 */
		swpa_sem_create(&m_cSem, dwInitCount, dwMaxCount);
	}
	else
	{
		/* 进程间的信号量 */
		swpa_processsem_create(&m_cSem, dwInitCount, dwMaxCount, dwSemId);
	}
}

/**
 * @brief 析构函数
 */
CSWSemaphore::~CSWSemaphore(){

    Delete();
}

/**
 * @brief 信号量创建函数
 * 
 * @param [in] dwInitCount : 初始数目，默认为1
 * @param [in] dwMaxCount : 最大数目，默认为1
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWSemaphore::Create(DWORD dwInitCount, DWORD dwMaxCount, DWORD dwSemId){

	m_sem_id = dwSemId;

	Delete();
	if (dwSemId == 0)
	{
		/* 进程内的信号量 */
		if (0 == swpa_sem_create(&m_cSem, dwInitCount, dwMaxCount))
		{
			return S_OK;
		}
	}
	else
	{
		/* 进程间的信号量 */
		if (0 == swpa_processsem_create(&m_cSem, dwInitCount, dwMaxCount, dwSemId))
		{
			return S_OK;
		}
	}
	
	return  E_FAIL;
}

HRESULT CSWSemaphore::Delete(){

	if (0 == m_cSem)
	{
		return S_OK;
	}
	if (0 == swpa_sem_delete(&m_cSem))
	{
		if (0 == m_sem_id)
		{
			m_cSem = 0;
		}
		return S_OK;
	}
	return  E_FAIL;
}

/**
 * @brief 锁定该同步对象控制的资源。
 * 
 * @param [in] dwTimeout : 锁操作超时时长，单位为毫秒
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 * @note
 * - 如果该同步对象是可用的， 则Pend()将成功地返回，并且线程从现在起拥有了这个对象;
 * - 如果同步对象不可用，则Pend()将在<i>dwTimeOut</i>参数指定的毫秒数内等待此同步对象变为可用；
 * - 如果这个同步对象在指定量的时间内没有变为可用的， 则Pend()返回失败。
 */
HRESULT CSWSemaphore::Pend(const INT sdwTimeout){

    if ( 0 == m_cSem )
    {
        return E_FAIL;
    }
    if ( 0 == swpa_sem_pend(&m_cSem, sdwTimeout) )
    {
        return S_OK;
    }
	return  E_FAIL;
}

/**
 * @brief 解除对该同步对象控制的资源的锁定
 * 
 * @param [in] dwTimeout : 解锁操作超时时长，单位为毫秒@return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWSemaphore::Post(){

    if ( 0 == m_cSem )
    {
        return E_FAIL;
    }
	if ( 0 == swpa_sem_post(&m_cSem) )
    {
        return S_OK;
    }
	return  E_FAIL;
}

HRESULT CSWSemaphore::Value()
{
	return swpa_sem_value(&m_cSem);
}

HRESULT CSWSemaphore::GetId()
{
	return m_sem_id;
}

