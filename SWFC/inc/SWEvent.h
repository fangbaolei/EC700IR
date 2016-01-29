///////////////////////////////////////////////////////////
//  CSWEvent.h
//  Implementation of the Class CSWEvent
//  Created on:      28-二月-2013 14:09:50
//  Original author: zhouy
///////////////////////////////////////////////////////////

#if !defined(EA_AFB81B9F_591A_42e0_A1D3_7B7BC4260061__INCLUDED_)
#define EA_AFB81B9F_591A_42e0_A1D3_7B7BC4260061__INCLUDED_

#include "SWMutex.h"

/**
 * @brief 事件管理基类
 */
class CSWEvent : public CSWSyncObject
{
CLASSINFO(CSWEvent,CSWSyncObject)
public:
	CSWEvent();
	virtual ~CSWEvent();
	HRESULT Create();
	HRESULT Delete();
	/**
	 * @brief Sets the event to available (signaled), releases waiting threads, and
	 * sets the event to unavailable (nonsignaled).
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 * @note .
	 */
	HRESULT PulseEvent();
	/**
	 * @brief Sets the event to unavailable (nonsignaled).
	 * 
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 * @note .
	 */
	HRESULT ResetEvent();
	/**
	 * @brief Sets the event to available (signaled) and releases any waiting threads.
	 * 
	 * 
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 * @note .
	 */
	HRESULT SetEvent();
	/**
	 * @brief 解锁
	 * 
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	HRESULT Unlock();

};
#endif // !defined(EA_AFB81B9F_591A_42e0_A1D3_7B7BC4260061__INCLUDED_)

