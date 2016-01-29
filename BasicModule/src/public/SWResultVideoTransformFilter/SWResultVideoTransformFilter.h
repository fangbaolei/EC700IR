/**
* @file SWResultVideoTransformFilter.h 
* @brief H264保存模块
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-22
* @version 1.0
*/


#ifndef __SW_RESULT_VIDEO_TRANSFORM_FILTER_H__
#define __SW_RESULT_VIDEO_TRANSFORM_FILTER_H__

#include "SWBaseFilter.h"
#include "SWMessage.h"
#include "SWRecord.h"
#include "SWList.h"




/**
* @brief H264保存模块
*
*
* 两个功能：一个是保存数据，另一个是读取数据。
* 
*/
class CSWResultVideoTransformFilter : public CSWBaseFilter, CSWMessage
{
	CLASSINFO(CSWResultVideoTransformFilter, CSWBaseFilter)
		
public:

	/**
	* @brief 构造函数
	* 
	*/
	CSWResultVideoTransformFilter();

	/**
	* @brief 析构函数
	* 
	*/
	virtual ~CSWResultVideoTransformFilter();

	/**
	* @brief 初始化函数
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	* @retval E_FAIL : 初始化失败
	* 
	*/
	HRESULT Initialize();

	virtual HRESULT Run();

	virtual HRESULT Stop();

	virtual HRESULT Receive(CSWObject* obj);	

	


	SW_BEGIN_DISP_MAP(CSWResultVideoTransformFilter,CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 0)
	SW_END_DISP_MAP();

protected:




	
private:

	static PVOID AddVideoProxy(PVOID pvArg);

	HRESULT AddVideo();



	SW_BEGIN_MESSAGE_MAP(CSWResultVideoTransformFilter, CSWMessage)
		
	SW_END_MESSAGE_MAP();

private:

	BOOL				m_fInited;
	
	CSWList<CSWImage*>	m_lstFrame;	/// 帧队列
	
	CSWList<CSWRecord*>	m_lstRecord;	/// 对象队列
	CSWSemaphore		m_cRecordLock;	
		
	DWORD				m_dwQueueSize;
	CSWSemaphore		m_cSemaQueueSync;

	CSWMemoryFactory * 	m_pcMemoryFactory;

	CSWSemaphore        m_cTickLock;	/// 触发存储时间队列锁	

	CSWThread			m_cThread;
	
};

REGISTER_CLASS(CSWResultVideoTransformFilter)
	
#endif //__SW_RESULT_VIDEO_TRANSFORM_FILTER_H__


