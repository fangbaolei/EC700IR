///////////////////////////////////////////////////////////
//  CSWMutex.cpp
//  Implementation of the Class CSWMutex
//  Created on:      28-二月-2013 14:09:49
//  Original author: zhouy
///////////////////////////////////////////////////////////

#include "SWMutex.h"

CSWMutex::CSWMutex(int MutexId){

	m_cMutex = 0;
	m_mutex_id = MutexId;
	Create(MutexId);
}

CSWMutex::~CSWMutex(){

    Delete();
}

HRESULT CSWMutex::Create(int MutexId){

	if (0 != m_cMutex)
	{
		Delete();
	}
	m_mutex_id = MutexId;
	if (0 == MutexId)
	{
		if (0 == swpa_mutex_create(&m_cMutex, NULL))
		{
			return S_OK;
		}
	}
	else
	{
		if (0 == swpa_processmutex_create(&m_cMutex, MutexId))
		{
			return S_OK;
		}
	}
	return  E_FAIL;
}

HRESULT CSWMutex::Delete(){

	if (0 == m_cMutex)
	{
		return S_OK;
	}
	else if (0 == swpa_mutex_delete(&m_cMutex))
	{
		if (0 == m_mutex_id)
		{
			m_cMutex = 0;
		}
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}


/**
 * @brief 用来获得对该同步对象控制的资源的访问。
 * 
 * @param [in] dwTimeout : 锁操作超时时长，单位为毫秒
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 * @note
 * - 如果该同步对象是可用的， 则Lock()将成功地返回，并且线程从现在起拥有了这个对象;
 * - 如果同步对象不可用，则Lock()将在<i>dwTimeOut</i>参数指定的毫秒数内等待此同步对象变为可用；
 * - 如果这个同步对象在指定量的时间内没有变为可用的， 则Lock()返回失败。
 */
HRESULT CSWMutex::Lock(INT iTimeout/* = -1 */){

    if ( 0 == m_cMutex )
    {
        return E_FAIL;
    }
	if ( 0 == swpa_mutex_lock(&m_cMutex, iTimeout) )
    {
        return S_OK;
    }
	return  E_FAIL;
}


/**
 * @brief 解锁
 * 
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWMutex::Unlock(){

    if ( 0 == m_cMutex )
    {
        return E_FAIL;
    }
	if ( 0 == swpa_mutex_unlock(&m_cMutex) )
    {
        return S_OK;
    }
	return  E_FAIL;
}

HRESULT CSWMutex::GetId()
{
	return m_mutex_id;
}


CSWRWMutex::CSWRWMutex(){

    m_cRwlock = 0;
    Create();
}


CSWRWMutex::~CSWRWMutex(){

    Delete();
}


HRESULT CSWRWMutex::Create(){

    Delete();

    if ( 0 == swpa_rwlock_create(&m_cRwlock, NULL) )
    {
        return S_OK;
    }
	return  E_FAIL;
}


HRESULT CSWRWMutex::Delete(){

    if ( 0 == m_cRwlock )
    {
        return S_OK;
    }
	else if ( 0 == swpa_rwlock_delete(&m_cRwlock) )
    {
        return S_OK;
    }
    else
    {
	    return E_FAIL;
    }
}


/**
 * @brief 读锁定
 * 
 * @param [in] dwTimeout : 锁操作超时时长，单位为毫秒
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 * @note .
 */
HRESULT CSWRWMutex::RLock(INT iTimeout/* = -1 */){

    if ( 0 == m_cRwlock )
    {
        return E_FAIL;
    }
	if ( 0 == swpa_rwlock_lock(&m_cRwlock, iTimeout, 1) )
    {
        return S_OK;
    }
	return  E_FAIL;
}


/**
 * @brief 解锁
 * 
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWRWMutex::Unlock(){

    if ( 0 == m_cRwlock )
    {
        return E_FAIL;
    }
	if ( 0 == swpa_rwlock_unlock(&m_cRwlock) )
    {
        return S_OK;
    }
	return  E_FAIL;
}


/**
 * @brief 写锁定
 * 
 * @param [in] dwTimeout : 锁操作超时时长，单位为毫秒
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 * @note .
 */
HRESULT CSWRWMutex::WLock(INT iTimeout/* = -1 */){

    if ( 0 == m_cRwlock )
    {
        return E_FAIL;
    }
	if ( 0 == swpa_rwlock_lock(&m_cRwlock, iTimeout, 0) )
    {
        return S_OK;
    }
	return  E_FAIL;
}




CSWSyncObject::CSWSyncObject(){

}


CSWSyncObject::~CSWSyncObject(){

}


HRESULT CSWSyncObject::Create(int MutexId){

	return  E_NOTIMPL;
}


HRESULT CSWSyncObject::Delete(){

	return  E_NOTIMPL;
}


/**
 * @brief 用来获得对该同步对象控制的资源的访问。
 * 
 * @param [in] dwTimeout : 锁操作超时时长，单位为毫秒
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 * @note
 * - 如果该同步对象是可用的， 则Lock()将成功地返回，并且线程从现在起拥有了这个对象;
 * - 如果同步对象不可用，则Lock()将在<i>dwTimeOut</i>参数指定的毫秒数内等待此同步对象变为可用；
 * - 如果这个同步对象在指定量的时间内没有变为可用的， 则Lock()返回失败。
 */
HRESULT CSWSyncObject::Lock(INT iTimeout/* = -1 */){

	return  E_NOTIMPL;
}


/**
 * @brief 解锁
 * 
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWSyncObject::Unlock(){

	return  E_NOTIMPL;
}

