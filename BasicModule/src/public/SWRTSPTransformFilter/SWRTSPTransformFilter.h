/**
* @file SWRTSP .h 
* @brief RTSP模块
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-04-01
* @version 1.0
*/


#ifndef __SW_RTSP_TRANSFORM_FILTER_H__
#define __SW_RTSP_TRANSFORM_FILTER_H__

#include "SWBaseFilter.h"
#include "SWClassFactory.h"

#include "SWOnvifRtspParameter.h"
/**
* @brief RTSP 模块
*
*
*
* 
*/
typedef struct _STAT_INFO { 	//statistics infomation
	DWORD				dwOriginBitrate;			//原始用户设置的码率
	DWORD				dwLastBitrate;			//上次程序修改的码率
	DWORD				dwOriginIFrameInterval; 	//原始用户设置的I帧间隔
	DWORD				dwLastIFrameInterval;		//上次修改的I帧间隔
	DWORD				dwLastReduceTick;			//最后一次减小码率时的TICK数
	DWORD				dwLastLostTick;
	DWORD				dwLostCounter;			//当前码率丢包计数器
	DWORD				dwLastAscendInterval;		//上一次升的带宽次数
	DWORD				dwNextAscendTick;
	DWORD				dwLastAscendBitrate;
	DWORD				dwLastLostBitrate;			//上一次丢包时的码率
}STAT_INFO;



class CSWRTSPTransformFilter : public CSWBaseFilter
{
	CLASSINFO(CSWRTSPTransformFilter, CSWBaseFilter);

public:
	/*
	* @brief 构造函数
	*
	*/
	CSWRTSPTransformFilter();
	
	/*
	* @brief 析构函数
	*
	*/
	virtual ~CSWRTSPTransformFilter();

	/*
	* @brief 初始化函数
	* @param [in] iChannelId 接收H264数据通道号
	* @retval S_OK : 成功
	* @retval E_OUTOFMEMORY : 内存不足
	*/
	HRESULT Initialize(INT iChannelId);

	
	/**
	 * @brief 接收输入的数据包
	 * @param [in] pdu 接收数据包
	 * @成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT Receive(CSWObject* obj);

	/*
	* @brief 开始运行函数
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	*/
	virtual HRESULT Run();

	/*
	* @brief 停止运行函数
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	*/
	virtual HRESULT Stop();


private:
	/**
	   @brief 降低码率，设置I 帧间隔
	   @param [IN] pStatusHeader 状态信息头部
	   @param [IN] pStatusSock 封装socket
	 */
	HRESULT ReduceStreamBitrate(H264_STATUS_HEADER *pStatusHeader, CSWTCPSocket *pStatusSock);

	/**
	   @brief 提高码率，设置I 帧间隔
	   @param [IN] pStatusHeader 状态信息头部
	   @param [IN] pStatusSock 封装socket
	 */
	HRESULT AscendStreamBitrate(H264_STATUS_HEADER *pStatusHeader, CSWTCPSocket *pStatusSock);

	/**
	 @brief 更新连接信息
	 @param [IN] pStatusHeader 状态信息头部
	 @param [IN] pStatusSock 封装socket
	 */
	HRESULT UpdateConnectionInfo(H264_STATUS_HEADER *pStatusHeader, CSWTCPSocket *pStatusSock);

	/**
	   @brief 验证RTSP用户
	   @param [IN] pStatusHeader 状态信息头部
	   @param [IN] pStatusSock 封装socket
	 */
	HRESULT Authenticate(H264_STATUS_HEADER *pStatusHeader, CSWTCPSocket *pStatusSock);

protected:

	SW_BEGIN_DISP_MAP(CSWRTSPTransformFilter,CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 1)
	SW_END_DISP_MAP();

	static PVOID OnSendDataProxy(PVOID pvArg);
	static PVOID OnGetConnStatusProxy(PVOID pvArg);
	HRESULT OnSendData();
	HRESULT OnGetConnStatus();

private:

	BOOL				m_fInited;

	DWORD				m_dwQueueSize;
	CSWSemaphore		m_cSemaSync;
	CSWThread			m_cThreadSend;
	CSWThread           m_cThreadGetConnStatus;
	CSWMutex			m_cMutexLock;

	CSWList<CSWImage*>	m_lstFrames;

	INT m_iChannelId;	//H264通道号

	//调整码率统计信息
	STAT_INFO m_cStatInfo[2];
};

REGISTER_CLASS(CSWRTSPTransformFilter);
	
#endif //__SW_RTSP_TRANSFORM_FILTER_H__





