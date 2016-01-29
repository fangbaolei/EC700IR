///////////////////////////////////////////////////////////
//  CSWMutex.h
//  Implementation of the Class CSWMutex
//  Created on:      28-二月-2013 14:09:49
//  Original author: zhouy
///////////////////////////////////////////////////////////

#if !defined(EA_2A2A5D15_8D0A_41d8_98D7_CCCD4E4FDC37__INCLUDED_)
#define EA_2A2A5D15_8D0A_41d8_98D7_CCCD4E4FDC37__INCLUDED_

#include "SWObject.h"
#include "swpa_sem.h"

/**
 * @brief 同步对象基类
 */
class CSWSyncObject : public CSWObject
{
CLASSINFO(CSWSyncObject,CSWObject)

public:
	CSWSyncObject();
	virtual ~CSWSyncObject();

	virtual HRESULT Create(int MutexId = 0);
	virtual HRESULT Delete();

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
	virtual HRESULT Lock(INT iTimeout = -1);
	/**
	 * @brief 解锁
	 * 
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT Unlock();
};

/**
 * @brief 互斥体管理基类
 */
class CSWMutex : public CSWSyncObject
{
CLASSINFO(CSWMutex,CSWSyncObject)

public:
	CSWMutex(int MutexId = 0);
	virtual ~CSWMutex();

	HRESULT Create(int MutexId = 0);
	HRESULT Delete();

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
	HRESULT Lock(INT iTimeout = -1);
	/**
	 * @brief 解锁
	 * 
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	HRESULT Unlock();

	/*****************************************************************************
	 函 数 名  : CSWMutex.GetId
	 功能描述  : 返回进程锁的标识id
	 输入参数  : 无
	 输出参数  : 无
	 返 回 值  : HRESULT
	 调用函数  : 
	 注意事项  : 在进程间信号量使用
	 
	 修改历史      :
	  1.日    期   : 2015年9月4日
	    作    者   : huangdch
	    修改内容   : 新生成函数

	*****************************************************************************/
	HRESULT GetId();
private:
    int m_cMutex;
	int m_mutex_id; //进程间锁的标识
};

/**
 * @brief 读写锁基类
 */
class CSWRWMutex : public CSWSyncObject
{
CLASSINFO(CSWRWMutex,CSWSyncObject)

public:
	CSWRWMutex();
	virtual ~CSWRWMutex();

	HRESULT Create();
	HRESULT Delete();

	/**
	 * @brief 读锁定
	 * 
	 * @param [in] dwTimeout : 锁操作超时时长，单位为毫秒
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 * @note .
	 */
	HRESULT RLock(INT iTimeout = -1);

	/**
	 * @brief 写锁定
	 * 
	 * @param [in] dwTimeout : 锁操作超时时长，单位为毫秒
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 * @note .
	 */
	HRESULT WLock(INT iTimeout = -1);

    /**
	 * @brief 解锁
	 * 
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	HRESULT Unlock();

private:
    int m_cRwlock;
};

#endif // !defined(EA_2A2A5D15_8D0A_41d8_98D7_CCCD4E4FDC37__INCLUDED_)

