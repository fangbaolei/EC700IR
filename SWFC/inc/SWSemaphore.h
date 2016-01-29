///////////////////////////////////////////////////////////
//  CSWSemaphore.h
//  Implementation of the Class CSWSemaphore
//  Created on:      28-二月-2013 14:09:49
//  Original author: zhouy
///////////////////////////////////////////////////////////

#if !defined(EA_C99D8074_9380_4865_9D8F_B1E9BAE3622E__INCLUDED_)
#define EA_C99D8074_9380_4865_9D8F_B1E9BAE3622E__INCLUDED_

#include "SWObject.h"
#include "swpa_sem.h"

/**
 * @brief 信号量管理基类
 */
class CSWSemaphore : public CSWObject
{
CLASSINFO(CSWSemaphore,CSWObject)
public:
	CSWSemaphore();

	/**
	 * @brief 构造函数
	 * 
	 * @param [in] dwInitCount : 初始数目，默认为1
	 * @param [in] dwMaxCount : 最大数目，默认为1
	 * @param [in] dwSemId : 进程间信号量标识,0表示进程内,>0表示进程间
	 */
	CSWSemaphore(DWORD dwInitCount, DWORD dwMaxCount, DWORD dwSemId = 0);
	/**
	 * @brief 析构函数
	 */
	virtual ~CSWSemaphore();
	/**
	 * @brief 信号量创建函数
	 * 
	 * @param [in] dwInitCount : 初始数目，默认为1
	 * @param [in] dwMaxCount : 最大数目，默认为1
	 * @param [in] dwSemId : 进程间信号量标识,0表示进程内,>0表示进程间
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	HRESULT Create(DWORD dwInitCount, DWORD dwMaxCount, DWORD dwSemId = 0);
	HRESULT Delete();
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
	HRESULT Pend(const INT sdwTimeout = -1);
	/**
	 * @brief 解除对该同步对象控制的资源的锁定
	 * 
	 * @param [in] dwTimeout : 解锁操作超时时长，单位为毫秒@return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	HRESULT Post();

	/*****************************************************************************
	 函 数 名  : CSWSemaphore.Value
	 功能描述  : 返回信号量的当前值
	 输入参数  : 无
	 输出参数  : 无
	 返 回 值  : HRESULT
	 调用函数  : 
	 注意事项  : 
	 
	 修改历史      :
	  1.日    期   : 2015年9月4日
	    作    者   : huangdch
	    修改内容   : 新生成函数

	*****************************************************************************/
	HRESULT Value();

	/*****************************************************************************
	 函 数 名  : CSWSemaphore.GetId
	 功能描述  : 返回信号量的标识id
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
    int m_cSem;
	int m_sem_id;//进程间信号量标识

};

#endif // !defined(EA_C99D8074_9380_4865_9D8F_B1E9BAE3622E__INCLUDED_)

