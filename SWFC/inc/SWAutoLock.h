///////////////////////////////////////////////////////////
//  CSWAutoLock.h
//  Implementation of the Class CSWAutoLock
//  Created on:      28-二月-2013 14:09:54
//  Original author: zhouy
///////////////////////////////////////////////////////////

#if !defined(EA_93B26E57_2917_49df_AE82_3954D8766736__INCLUDED_)
#define EA_93B26E57_2917_49df_AE82_3954D8766736__INCLUDED_

#include "SWMutex.h"

/**
 * @brief 自动锁类。
 */
class CSWAutoLock : public CSWObject
{
CLASSINFO(CSWAutoLock, CSWObject)

public:
	CSWAutoLock();

	/**
	 * @brief 构造函数，在该函数中调用pcSyncObj的lock()方法对pcSyncObj加锁。
	 */
	CSWAutoLock(CSWSyncObject* pcSyncObj);

	/**
	 * @brief 析构函数，在这里调用pcSyncObj的unlock()方法，对pcSyncObj解锁。
	 */
	~CSWAutoLock();

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

private:
    CSWSyncObject* m_pcSyncObj;

};
#endif // !defined(EA_93B26E57_2917_49df_AE82_3954D8766736__INCLUDED_)

