/**
* @file SWH264QueueRenderFilter.h 
* @brief H264保存模块
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-22
* @version 1.0
*/


#ifndef __SW_H264_QUEUE_RENDER_FILTER_H__
#define __SW_H264_QUEUE_RENDER_FILTER_H__

#include "SWBaseFilter.h"
#include "SWMessage.h"
#include "SWImage.h"
#include "SWList.h"




/**
* @brief H264保存模块
*
*
* 两个功能：一个是保存数据，另一个是读取数据。
* 
*/
class CSWH264QueueRenderFilter : public CSWBaseFilter, CSWMessage
{
	CLASSINFO(CSWH264QueueRenderFilter, CSWBaseFilter)
		
public:

	/**
	* @brief 构造函数
	* 
	*/
	CSWH264QueueRenderFilter();

	/**
	* @brief 析构函数
	* 
	*/
	virtual ~CSWH264QueueRenderFilter();

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
	
	
public:


	SW_BEGIN_DISP_MAP(CSWH264QueueRenderFilter,CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 0)
	SW_END_DISP_MAP();


protected:


	
	HRESULT FrameEnqueue(CSWImage* pImage);

	/**
	* @brief 把关键帧放到帧队列
	* @param [in] pImage : 关键帧对象指针
	* @param [out] piBufSize : 保存该帧数据大小
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	* @retval E_FAIL : 失败
	*/
	HRESULT KeyFrameEnqueue(CSWImage* pImage, INT* piBufSize);


	/**
	* @brief 把非关键帧放到帧队列
	* @param [in] pImage : 非关键帧对象指针
	* @param [out] piBufSize : 保存该帧数据大小
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	* @retval E_FAIL : 失败
	*/
	HRESULT NonKeyFrameEnqueue(CSWImage* pImage, INT* piBufSize);

	/**
	* @brief 把文件放置保存队列
	* @param [in] sTimeStamp : 文件的时间戳
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	* @retval E_FAIL : 失败
	*/
	HRESULT FileEnqueue(const SWPA_TIME& sTimeStamp);

	/**
	* @brief H264视频文件保存线程主函数
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	* @retval E_FAIL : 失败
	*/
	HRESULT OnHistoryFileSaving(VOID);


	

	/**
	* @brief H264视频历史文件发送线程主函数
	* @param [in] dwPinID : 历史文件发送Pin ID
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	* @retval E_FAIL : 失败
	*/
	HRESULT OnHistoryFileTransmitting(const DWORD dwPinID);


	
	HRESULT GetImageTimeStamp(CSWImage * pImage, SWPA_TIME* pTimeStamp);


	
private:
	
	HRESULT ClearBuffer();
	
	static PVOID H264EnqueueProxy(PVOID pvArg);

	HRESULT H264Enqueue();

	
	HRESULT OnGetH264(WPARAM wParam, LPARAM lParam);



	SW_BEGIN_MESSAGE_MAP(CSWH264QueueRenderFilter, CSWMessage)
		SW_MESSAGE_HANDLER(MSG_H264_QUEUE_GET_VIDEO, OnGetH264)
	SW_END_MESSAGE_MAP();

private:

	BOOL				m_fInited;
	
	CSWList<CSWImage*>	m_lstFrame;	/// 帧队列
	
	CSWList<CSWObject*>	m_lstObj;	/// 对象队列
	CSWSemaphore		m_cObjLock;	/// 对象保存队列锁	
	
	CSWList<CSWImage*>	m_lstFile;		/// 文件保存队列	
	CSWSemaphore		m_cFileLock;	/// 文件保存队列锁	
	
	DWORD				m_dwQueueSize;
	CSWSemaphore		m_cSemaObjSync;

	CSWMemoryFactory * 	m_pcMemoryFactory;

	CSWThread			m_cThread;
	
};

REGISTER_CLASS(CSWH264QueueRenderFilter)
	
#endif //__SW_H264_QUEUE_RENDER_FILTER_H__


