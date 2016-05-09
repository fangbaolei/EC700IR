/**
* @file SWRTSPControl.cpp
* @brief RTSP模块
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-07-19
* @version 1.0
*/


#include "SWFC.h"
#include "SWRTSPTransformFilter.h"
#include "SWMessage.h"

#include <dlfcn.h>


CSWRTSPTransformFilter::CSWRTSPTransformFilter()
	:CSWBaseFilter(1, 0)
	,m_fInited(FALSE)
	,m_dwQueueSize(5)
	,m_iChannelId(0)
{
	GetIn(0)->AddObject(CLASSID(CSWImage));
}


CSWRTSPTransformFilter::~CSWRTSPTransformFilter()
{
	if (m_fInited)
	{
		Stop();
		m_fInited = FALSE;
	}
}

HRESULT CSWRTSPTransformFilter::Initialize(INT iChannelId)
{
	if (m_fInited)
	{
		return S_OK;
	}

	if (FAILED(m_cSemaSync.Create(0, m_dwQueueSize)))
	{
		SW_TRACE_NORMAL("Err: failed to create m_cSemaSync\n");
		return E_FAIL;
	}

	m_lstFrames.SetMaxCount(m_dwQueueSize);
	m_iChannelId = iChannelId;
	
	m_fInited = TRUE;

	return S_OK;
}



HRESULT CSWRTSPTransformFilter::Receive(CSWObject* obj)
{
	if (!m_fInited)
	{
		SW_TRACE_NORMAL("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	if (IsDecendant(CSWImage, obj))
	{		
		CSWImage * pImage = (CSWImage*) obj;
		if (NULL == pImage)
		{
			SW_TRACE_NORMAL("Err: NULL == pImage\n");
			return E_INVALIDARG;
		}

		if (pImage->GetSize() > 384*1024) //Max= 384KB/F
		{
			SW_TRACE_NORMAL("Err: Image Size > 384KB, can't handle this\n");
			return E_FAIL;
		}

		m_cMutexLock.Lock();
		if (m_lstFrames.IsFull())
		{
			static INT iDropFrameCount = 0;
			if (0 == (iDropFrameCount++ % 200))
			{
				SW_TRACE_NORMAL("Warning: Channel <%d> RTSP send queue is full, discards oldest frame count %d\n",
					m_iChannelId,iDropFrameCount);
			}
			CSWImage* pImageToBeDeleted = m_lstFrames.RemoveHead();
			SAFE_RELEASE(pImageToBeDeleted);

		}
		SAFE_ADDREF(pImage);
		m_lstFrames.AddTail(pImage);
		m_cMutexLock.Unlock();

		if (FAILED(m_cSemaSync.Post()))
		{
			//SW_TRACE_NORMAL("Err: failed to post, discards the last H264 obj\n");
		}
	}

	return S_OK;
}



HRESULT CSWRTSPTransformFilter::Run()
{
	if (!m_fInited)
	{
		SW_TRACE_NORMAL("Err: not inited yet\n");
		return E_OBJ_NO_INIT;
	}

	if (FAILED(CSWBaseFilter::Run()))
	{
		SW_TRACE_NORMAL("Err: Failed to run CSWRTSPTransformFilter\n");
		return E_FAIL;
	}

	if (FAILED(m_cThreadSend.Start(OnSendDataProxy, (PVOID)this)))
	{
		SW_TRACE_NORMAL("Err: failed to start RTSPTransform send thread\n");
		return E_FAIL;
	}

	if(FAILED(m_cThreadGetConnStatus.Start(OnGetConnStatusProxy, (PVOID)this)))
	{
		SW_TRACE_NORMAL("Err: failed to start RTSPTransform get connection status thread\n");
		return E_FAIL;
	}

	SW_TRACE_NORMAL("Info: CSWRTSPTransformFilter running...\n");	
	
	return S_OK;
}


HRESULT CSWRTSPTransformFilter::Stop()
{
	if (!m_fInited)
	{
		SW_TRACE_NORMAL("Err: not inited yet\n");
		return E_OBJ_NO_INIT;
	}

	if (FAILED(CSWBaseFilter::Stop()))
	{
		SW_TRACE_NORMAL("Err: Failed to stop CSWRTSPTransformFilter\n");
		return E_FAIL;
	}

	m_cMutexLock.Lock();
	while (!m_lstFrames.IsEmpty())
	{
		CSWImage* pImageToBeDeleted = m_lstFrames.RemoveHead();
		SAFE_RELEASE(pImageToBeDeleted);
	}
	m_cMutexLock.Unlock();

	//swpa_utils_shell("clear_h264fifo", NULL);
	
	if (FAILED(m_cThreadSend.Stop()))
	{
		SW_TRACE_NORMAL("Err: failed to stop RTSPTransform send thread\n");
		//swpa_utils_shell("restore_h264fifo", NULL);
		return E_FAIL;
	}

	if(FAILED(m_cThreadGetConnStatus.Stop()))
	{
		SW_TRACE_NORMAL("Err: failed to stop RTSPTransform get connection staus thread\n");
		return E_FAIL;
	}

	//swpa_utils_shell("restore_h264fifo", NULL);
	
	SW_TRACE_NORMAL("Info: CSWRTSPTransformFilter stopped\n");
	
	return S_OK;
}
	

HRESULT CSWRTSPTransformFilter::OnSendData()
{
	if (!m_fInited)
	{
		SW_TRACE_NORMAL("Err: not inited yet\n");
		return E_OBJ_NO_INIT;
	}

	HRESULT hr = S_OK;

	SW_TRACE_NORMAL("Info: RTSP filter run.");
	
	BOOL fIsHaveConnect = FALSE;
	CHAR szRTSPH264SockFile[128] = {""};

	if (SWPA_VPSS_H264_CHANNEL == m_iChannelId)		//主H264
	{
		swpa_strcpy(szRTSPH264SockFile,"/tmp/rtsph264.sock");
	}
	else if(SWPA_VPSS_H264_SECOND_CHANNEL == m_iChannelId)	//辅H264
	{
		swpa_strcpy(szRTSPH264SockFile,"/tmp/rtsph264Second.sock");
	}
	
	CSWTCPSocket cLocalTcpSock;

	if (FAILED(cLocalTcpSock.Create(TRUE)))
	{
        SW_TRACE_NORMAL("Info: RTSP filter open %s failed.", szRTSPH264SockFile);
		return E_FAIL;
	}

	do
	{
		hr = cLocalTcpSock.Bind(szRTSPH264SockFile);
		if (FAILED(hr))
		{
			SW_TRACE_NORMAL("Info: RTSP filter bind %s failed.",szRTSPH264SockFile);
			swpa_thread_sleep_ms(2000);
			continue;
		}
	}while(S_OK != hr && FILTER_RUNNING == GetState());

	cLocalTcpSock.SetRecvTimeout(4000);
	cLocalTcpSock.SetSendTimeout(4000);
	if (FAILED(cLocalTcpSock.Listen()))
	{
		SW_TRACE_NORMAL("Err: failed to listen!\n");
		return E_FAIL;
	}
	CSWTCPSocket sockData;

	while (FILTER_RUNNING == GetState())
	{	
		if (SUCCEEDED(m_cSemaSync.Pend(40)))
		{
			m_cMutexLock.Lock();
			CSWImage* pImage = NULL;
			if(!m_lstFrames.IsEmpty())
			{
				pImage = m_lstFrames.RemoveHead();
			}
			m_cMutexLock.Unlock();
			if (NULL == pImage )
			{
				swpa_thread_sleep_ms(40);
				continue;
			}

			if (FALSE == fIsHaveConnect)
			{
				SWPA_SOCKET_T outSock;
				
				if (FAILED(cLocalTcpSock.Accept(outSock)))
				{
					//SW_TRACE_NORMAL("Err: Accept failed...");
					SAFE_RELEASE(pImage);
					continue;
				}
				
				SW_TRACE_NORMAL("Info: RTSP local socket got a connection...\n");
				sockData.Attach(outSock);
				sockData.SetSendTimeout(1000);
				sockData.SetSendBufferSize(32768);	//
				fIsHaveConnect = TRUE;
			}
			
			SW_COMPONENT_IMAGE sComponent;
			swpa_memset(&sComponent, 0, sizeof(sComponent));
				
			pImage->GetImage(&sComponent);

			DWORD dwOutLengthSent = 0;


			typedef struct framehead
			{
				BYTE bySync;
				BYTE byType;
				BYTE byReserve[2]; 
				DWORD dwTimeStamp;
				DWORD dwDataLen;
							
#define FRAME_HEAD_SYNC 0x47
#define FRAME_DATA_TYPE 0x1
				framehead()
				{
					bySync = FRAME_HEAD_SYNC;
					byType = FRAME_DATA_TYPE;
					dwTimeStamp = 0;
					dwDataLen = 0;
				}
			}tFrameHead;

			tFrameHead head;
			head.dwDataLen = sComponent.iSize;

#if 0
			head.dwTimeStamp = CSWDateTime::GetSystemTick();
#endif

			HRESULT hr = sockData.Send((void*)&head, sizeof(head),&dwOutLengthSent);
			if (FAILED(hr) || dwOutLengthSent != sizeof(head))
			{
				SW_TRACE_NORMAL("Err: failed to send H264 head size via RTSP hr:0x%08x,Len:%d Sent:%d\n",
					hr,sComponent.iSize,dwOutLengthSent);
				sockData.Close();
				fIsHaveConnect = FALSE;
			}
						
			hr = sockData.Send((void*)sComponent.rgpbData[0], sComponent.iSize,&dwOutLengthSent);
			if (FAILED(hr) || dwOutLengthSent != sComponent.iSize)
			{
				SW_TRACE_NORMAL("Err: failed to send H264 size via RTSP hr:0x%08x,Len:%d Sent:%d\n",
					hr,sComponent.iSize,dwOutLengthSent);
				sockData.Close();
				fIsHaveConnect = FALSE;
			}

			SAFE_RELEASE(pImage);
			
		}

	}

	sockData.Close();
	cLocalTcpSock.Close();

	SW_TRACE_NORMAL("Info: %s: %s() exited\n", __FILE__, __FUNCTION__);
	
	return S_OK;
}

/*HRESULT CSWRTSPTransformFilter::ReduceStreamBitrate(H264_STATUS_HEADER *pStatusHeader, CSWTCPSocket *pGetStatusSock)
{
	if(pStatusHeader == NULL || pGetStatusSock == NULL)
		return E_FAIL;
	INT iCurBitrate = 0;
	INT iRateControl = 0;
	INT iCurIFrameInterval = 0;
	const INT cniReduceUnit = 1024;	//2 MB
	const INT cniMinBitrate = 2048;

	DWORD dwConnectCount = 0;
	DWORD dwInDataLen = 0;
	DWORD dwLastLostBitrate = 0;
	HRESULT hr = S_OK;
	INT iStatInfoIndex = 0;

	DWORD dwCurTick = CSWDateTime::GetSystemTick();	


	//获取连接数
	if(pStatusHeader->iDataLen != sizeof(DWORD))
		return E_FAIL;
	hr = pGetStatusSock->Read((VOID *)&dwConnectCount, pStatusHeader->iDataLen, &dwInDataLen);
	if(FAILED(hr) || (dwInDataLen != pStatusHeader->iDataLen) || (dwInDataLen == 0))
	{
		SW_TRACE_DEBUG("Err: Read RTSP connection info fail\n");
		return hr;
	}	

	if(H264_STATUS_FIRST_STREAM_REDUCE == pStatusHeader->eH264StatusCmd)     // 0 major stream
	{
		iStatInfoIndex = 0;
		CSWMessage::SendMessage(MSG_GET_H264_I_FRAME_INTERVAL, 0, (LPARAM)&iCurIFrameInterval);
		CSWMessage::SendMessage(MSG_GET_H264_BITRATE, 0, (LPARAM)&iCurBitrate);
	}
	else 
	{
		iStatInfoIndex = 1;
		CSWMessage::SendMessage(MSG_GET_H264_SECOND_I_FRAME_INTERVAL, 0, (LPARAM)&iCurIFrameInterval);
		CSWMessage::SendMessage(MSG_GET_H264_SECOND_BITRATE, 0, (LPARAM)&iCurBitrate);
	}

	//码率被用户更改
	if(iCurBitrate != m_cStatInfo[iStatInfoIndex].dwLastBitrate)
	{
		m_cStatInfo[iStatInfoIndex].dwOriginBitrate = iCurBitrate;
	}

	//计算上次丢包的时间是否符合,距离上次丢包30秒以上则重新计数，否则增加计数
	if(dwCurTick - m_cStatInfo[iStatInfoIndex].dwLastLostTick > 30000)
	{
		m_cStatInfo[iStatInfoIndex].dwLostCounter = 1;
	}
	else
	{
		m_cStatInfo[iStatInfoIndex].dwLostCounter++;
	}
	m_cStatInfo[iStatInfoIndex].dwLastLostTick = dwCurTick;

	//当前码率未小于最小码率，并且连续丢包达到5次，则减小当前码率值
	if(iCurBitrate > cniMinBitrate)
	{
		if(((dwCurTick - m_cStatInfo[iStatInfoIndex].dwLastReduceTick) < 30000) && 
		   (m_cStatInfo[iStatInfoIndex].dwLostCounter < 5))
		{
			return S_OK;
		}
		dwLastLostBitrate = iCurBitrate;
		iCurBitrate -= cniReduceUnit;
	}

	iCurBitrate = iCurBitrate < cniMinBitrate ? cniMinBitrate : iCurBitrate;

	//丢包计数复位
	m_cStatInfo[iStatInfoIndex].dwLostCounter = 0;

	//重新计算提升码率时间
	if(m_cStatInfo[iStatInfoIndex].dwLastAscendInterval == 0)
	{
		m_cStatInfo[iStatInfoIndex].dwLastAscendInterval = 60000;    // 1分钟
	}
	if(	m_cStatInfo[iStatInfoIndex].dwLastLostBitrate == dwLastLostBitrate)	//同一个码率连续丢则指数增加调整时间
	{
		m_cStatInfo[iStatInfoIndex].dwNextAscendTick = dwCurTick + (m_cStatInfo[iStatInfoIndex].dwLastAscendInterval << 2); 
		m_cStatInfo[iStatInfoIndex].dwLastAscendInterval <<= 2; 
	}
	else
	{
		m_cStatInfo[iStatInfoIndex].dwLastLostBitrate = dwLastLostBitrate;
		m_cStatInfo[iStatInfoIndex].dwLastAscendInterval = 60000;    // 1分钟
		m_cStatInfo[iStatInfoIndex].dwNextAscendTick = dwCurTick + m_cStatInfo[iStatInfoIndex].dwLastAscendInterval; 
	}

	//I帧间隔被更改重新置位
	if(iCurIFrameInterval != m_cStatInfo[iStatInfoIndex].dwLastIFrameInterval)
	{
		m_cStatInfo[iStatInfoIndex].dwOriginIFrameInterval = iCurIFrameInterval;
	}

	//判断I帧间隔范围值，同等码率下，I 帧间隔越大画面越好, 码率大于8Mbps的不调整I 帧间隔
	if(iCurBitrate < (8 << 10) && iCurBitrate >= (4 << 10) )
	{
		iCurIFrameInterval = iCurIFrameInterval < 10 ? 10 : iCurIFrameInterval;
	}
	else if(iCurBitrate < (4 << 10) && (iCurBitrate >= (2 << 10)))
	{
		iCurIFrameInterval = iCurIFrameInterval < 20 ? 20 : iCurIFrameInterval;
	}

	m_cStatInfo[iStatInfoIndex].dwLastIFrameInterval = iCurIFrameInterval;

	//设置码率
	if(H264_STATUS_FIRST_STREAM_REDUCE == pStatusHeader->eH264StatusCmd)
	{
		CSWMessage::SendMessage(MSG_SET_H264_BITRATE, (WPARAM)iCurBitrate, 0);
		CSWMessage::SendMessage(MSG_SET_H264_I_FRAME_INTERVAL, (WPARAM)iCurIFrameInterval, 0);
		SW_TRACE_DEBUG("RTSP stream 0 reduce bitrate. current bitrate:%d IFrameInterval:%d\n", 
					   iCurBitrate, iCurIFrameInterval);
	}
	else
	{
	   	CSWMessage::SendMessage(MSG_SET_H264_SECOND_BITRATE, (WPARAM)iCurBitrate, 0);
		CSWMessage::SendMessage(MSG_SET_H264_SECOND_I_FRAME_INTERVAL, (WPARAM)iCurIFrameInterval, 0);
		SW_TRACE_DEBUG("RTSP stream 1 reduce bitrate. current bitrate:%d IFrameInterval:%d\n", 
					   iCurBitrate, iCurIFrameInterval);
	}
	m_cStatInfo[iStatInfoIndex].dwLastBitrate = iCurBitrate;
	m_cStatInfo[iStatInfoIndex].dwLastReduceTick = dwCurTick;

	return S_OK;
}

HRESULT CSWRTSPTransformFilter::AscendStreamBitrate(H264_STATUS_HEADER *pStatusHeader, CSWTCPSocket *pGetStatusSock)
{
	if(pStatusHeader == NULL || pGetStatusSock == NULL)
		return E_FAIL;
	const INT cniAscendUnit = 1024;	//1 MB
	INT iCurBitrate = 0;
	INT iRateControl = 0;
	INT iCurIFrameInterval = 0;
	INT iStatInfoIndex = 0;
	HRESULT hr = S_OK;
	DWORD dwInDataLen = 0;
	DWORD dwConnectCount = 0;
	DWORD dwCurTick = CSWDateTime::GetSystemTick();	

	if(pStatusHeader->iDataLen != sizeof(DWORD))
		return E_FAIL;
	hr = pGetStatusSock->Read((VOID *)&dwConnectCount, pStatusHeader->iDataLen, &dwInDataLen);
	if(FAILED(hr) || (dwInDataLen != pStatusHeader->iDataLen) || (dwInDataLen == 0))
	{
		SW_TRACE_DEBUG("Err: Read RTSP connection info fail\n");
		return hr;
	}	

	if(H264_STATUS_FIRST_STREAM_ASCEND == pStatusHeader->eH264StatusCmd)		// 0 major stream
	{
		iStatInfoIndex = 0;
		CSWMessage::SendMessage(MSG_GET_H264_I_FRAME_INTERVAL, 0, (LPARAM)&iCurIFrameInterval);
		CSWMessage::SendMessage(MSG_GET_H264_BITRATE, 0, (LPARAM)&iCurBitrate);
	}
	else 
	{
		iStatInfoIndex = 1;
		CSWMessage::SendMessage(MSG_GET_H264_SECOND_I_FRAME_INTERVAL, 0, (LPARAM)&iCurIFrameInterval);
		CSWMessage::SendMessage(MSG_GET_H264_SECOND_BITRATE, 0, (LPARAM)&iCurBitrate);
	}
	//I帧间隔被用户改变
	if(iCurIFrameInterval != m_cStatInfo[iStatInfoIndex].dwLastIFrameInterval)
	{
		m_cStatInfo[iStatInfoIndex].dwOriginIFrameInterval = iCurIFrameInterval;
		m_cStatInfo[iStatInfoIndex].dwLastIFrameInterval = iCurIFrameInterval;
	}

	//如果码率被用户改变或已恢复成原码率则不做修改设置
	if((iCurBitrate != m_cStatInfo[iStatInfoIndex].dwLastBitrate) || 
	   (iCurBitrate == m_cStatInfo[iStatInfoIndex].dwOriginBitrate))
	{
		m_cStatInfo[iStatInfoIndex].dwOriginBitrate = iCurBitrate;
		m_cStatInfo[iStatInfoIndex].dwLastBitrate = iCurBitrate;
		m_cStatInfo[iStatInfoIndex].dwLastAscendInterval = 60000;
		m_cStatInfo[iStatInfoIndex].dwNextAscendTick = 0;
		return S_OK;
	}

	if(dwCurTick < m_cStatInfo[iStatInfoIndex].dwNextAscendTick)
	{
		SW_TRACE_DEBUG("RTSP stream ascend bitrate. Countdown:%d s\n", (m_cStatInfo[iStatInfoIndex].dwNextAscendTick - dwCurTick) / 1000);
		return S_OK;
	}

	if(m_cStatInfo[iStatInfoIndex].dwLastAscendBitrate == iCurBitrate)
	{
		m_cStatInfo[iStatInfoIndex].dwLastAscendInterval = 60000;
		m_cStatInfo[iStatInfoIndex].dwNextAscendTick = 0;
	}


	//要设置的码率大小
	 if(iCurBitrate < m_cStatInfo[iStatInfoIndex].dwOriginBitrate)
	 	iCurBitrate += cniAscendUnit;

	 iCurBitrate = iCurBitrate < m_cStatInfo[iStatInfoIndex].dwOriginBitrate ? iCurBitrate : m_cStatInfo[iStatInfoIndex].dwOriginBitrate;


	 //if(iCurBitrate == m_cStatInfo[iStatInfoIndex].dwLastBitrate)
	 //	return S_OK;

	//升回码率时恢复I帧间隔
	if(iCurBitrate == m_cStatInfo[iStatInfoIndex].dwOriginBitrate)
	{
		iCurIFrameInterval = m_cStatInfo[iStatInfoIndex].dwOriginIFrameInterval;
		m_cStatInfo[iStatInfoIndex].dwLastIFrameInterval = iCurIFrameInterval;
	}

	if(H264_STATUS_FIRST_STREAM_ASCEND == pStatusHeader->eH264StatusCmd)
	{
		CSWMessage::SendMessage(MSG_SET_H264_BITRATE, (WPARAM)iCurBitrate, 0);
		CSWMessage::SendMessage(MSG_SET_H264_I_FRAME_INTERVAL, (WPARAM)iCurIFrameInterval, 0);
		SW_TRACE_DEBUG("RTSP stream 0 ascend bitrate. current bitrate:%d IFrameInterval:%d\n", 
					   iCurBitrate, iCurIFrameInterval);
	}
	else
	{
		CSWMessage::SendMessage(MSG_SET_H264_SECOND_BITRATE, (WPARAM)iCurBitrate, 0);
		CSWMessage::SendMessage(MSG_SET_H264_SECOND_I_FRAME_INTERVAL, (WPARAM)iCurIFrameInterval, 0);
		SW_TRACE_DEBUG("RTSP stream 1 ascend bitrate. current bitrate:%d IFrameInterval:%d\n", 
					   iCurBitrate, iCurIFrameInterval);
	}
	m_cStatInfo[iStatInfoIndex].dwLastBitrate = iCurBitrate;
	m_cStatInfo[iStatInfoIndex].dwLastAscendBitrate = iCurBitrate;

	return S_OK;
}

HRESULT CSWRTSPTransformFilter::UpdateConnectionInfo(H264_STATUS_HEADER *pStatusHeader,CSWTCPSocket *pGetStatusSock)
{
	if(pStatusHeader == NULL || pGetStatusSock == NULL)
		return E_FAIL;
	CHAR szConnInfo[512] = {0};
	CHAR szMsg[512] = {0};
	DWORD dwInDataLen = 0;
	CSWString strInfo = "";
	HRESULT hr = S_OK;
	

	if(pStatusHeader->iDataLen >= sizeof(szConnInfo))
		pStatusHeader->iDataLen = sizeof(szConnInfo) - 1;
	hr = pGetStatusSock->Read((VOID *)szConnInfo, pStatusHeader->iDataLen, &dwInDataLen);
	if(FAILED(hr) || (dwInDataLen != pStatusHeader->iDataLen) || (dwInDataLen == 0))
	{
		SW_TRACE_DEBUG("Err: Read RTSP connection info fail\n");
		return hr;
	}
	if(pStatusHeader->eH264StatusCmd == H264_STATUS_FIRST_STREAM_CONN)
		strInfo.Append("RTSP链接:");
	else
		strInfo.Append("RTSP第二路链接:");

	strInfo.Append(szConnInfo);
	swpa_strncpy(szMsg, (const CHAR*)strInfo, sizeof(szMsg) - 1);
	CSWMessage::SendMessage(MSG_APP_UPDATE_STATUS, (WPARAM)szMsg, 0);
	SW_TRACE_NORMAL("RTSP connect: %s\n", szMsg); 

	return S_OK;
}

HRESULT CSWRTSPTransformFilter::Authenticate(H264_STATUS_HEADER *pStatusHeader, CSWTCPSocket *pGetStatusSock)
{
	HRESULT hr = S_OK;
	DWORD dwInDataLen = 0;
	CHAR szUserName[128] = {0};
	CHAR szPassword[128] = {0};

	if(pStatusHeader->iDataLen >= sizeof(szUserName))
		pStatusHeader->iDataLen = sizeof(szUserName) - 1;

	hr = pGetStatusSock->Read((VOID *)szUserName, pStatusHeader->iDataLen, &dwInDataLen);
	if(FAILED(hr) || (dwInDataLen != pStatusHeader->iDataLen) || (dwInDataLen == 0))
	{
		SW_TRACE_DEBUG("Err: Read RTSP authenticate info fail\n");
		return hr;
	}


	CSWMessage::SendMessage(MSG_USER_GETPASSWORD, (WPARAM)szUserName, (LPARAM)szPassword);
	if(0 == swpa_strlen(szPassword))
	{
		SW_TRACE_DEBUG("Err:authenticate fail, unknow username:%s\n", szUserName);
		return E_FAIL;
	}
//	SW_TRACE_DEBUG("************authenticate username:%s password:%s\n", szUserName, szPassword);
	hr = pGetStatusSock->Send((VOID *)szPassword, strlen(szPassword), &dwInDataLen);
	if(FAILED(hr) || (dwInDataLen != strlen(szPassword)) || (dwInDataLen == 0))
	{
		SW_TRACE_DEBUG("Err: Read RTSP authenticate info fail\n");
		return hr;
	}

	return S_OK;
}*/


PVOID CSWRTSPTransformFilter::OnSendDataProxy(PVOID pvArg)
{
	if (NULL == pvArg)
	{
		SW_TRACE_NORMAL("Err: NULL == pvArg\n");
		return NULL;
	}

	return (PVOID)((CSWRTSPTransformFilter*)pvArg)->OnSendData();
}

HRESULT CSWRTSPTransformFilter::OnGetConnStatus()
{
	if (!m_fInited)
	{
		SW_TRACE_NORMAL("Err: not inited yet\n");
		return E_OBJ_NO_INIT;
	}

	if (SWPA_VPSS_H264_CHANNEL != m_iChannelId)		//主H264, 只需要一个通道获取状态就可以了
	{
		return S_OK;
	}

	HRESULT hr = S_OK;
	CSWTCPSocket cGetStatusSock;
	H264_STATUS_HEADER cStatusHeader;
	BOOL isConnected = FALSE;
	INT eGetConnStatusCmd = 0;
	INT iCurBitrate = 0;
	DWORD dwInDataLen = 0;

	DWORD dwLastUpdateTimeMs = CSWDateTime::GetSystemTick();	
	if (FAILED(cGetStatusSock.Create()))
	{
		SW_TRACE_NORMAL("Info: RTSP filter get connection status Create socket failed\n");
		return E_FAIL;
	}
	cGetStatusSock.SetRecvTimeout(1000);
	cGetStatusSock.SetSendTimeout(4000);
	while (FILTER_RUNNING == GetState())
    {	
		while(!isConnected && FILTER_RUNNING == GetState())
		{
			if(!cGetStatusSock.IsValid())
			{
				if (FAILED(cGetStatusSock.Create()))
				{
					SW_TRACE_NORMAL("Info: RTSP filter get connection status Create socket failed\n");
					return E_FAIL;
				}
				cGetStatusSock.SetRecvTimeout(1000);
				cGetStatusSock.SetSendTimeout(4000);
			}
			hr = cGetStatusSock.Connect("127.0.0.1", 10086);
			if (FAILED(hr))
			{
				SW_TRACE_NORMAL("Info: RTSP filter connect port 10086 failed.\n");
				swpa_thread_sleep_ms(2000);
				continue;
			}
			isConnected = TRUE;
			//cGetStatusSock.SetRecvTimeout(60000);
		}

		if (FILTER_RUNNING != GetState())
		{
			break;
		}
	
		/*swpa_memset(&cStatusHeader, 0, sizeof(H264_STATUS_HEADER));
		dwInDataLen = 0;
			
		//	SW_TRACE_DEBUG("-------------------- Read Status Cmd\n");
		hr = cGetStatusSock.Read((VOID *)&cStatusHeader, sizeof(H264_STATUS_HEADER), &dwInDataLen);
		if(FAILED(hr))
		{
			//一段时间后不丢包则复位参数，避免段时间出现丢包引起的设置码率震荡
			// DWORD dwCurTick = CSWDateTime::GetSystemTick();
			// if( (dwCurTick - m_dwLastReduceTick) > 180000)
			// 	m_dwLastReduceTick = 0;

			cGetStatusSock.Close();
			isConnected = FALSE;
			continue;
		}
		//SW_TRACE_DEBUG("-+++++++++++++++++++++++++ Read Status Cmd %d, dataLen %d, InDataLen %d\n", (INT)cStatusHeader.eH264StatusCmd, cStatusHeader.iDataLen, dwInDataLen);
		switch(cStatusHeader.eH264StatusCmd)
		{
		case H264_STATUS_FIRST_STREAM_CONN:
		case H264_STATUS_SECOND_STREAM_CONN:
			hr = UpdateConnectionInfo(&cStatusHeader, &cGetStatusSock);
			if(FAILED(hr))
			{
				SW_TRACE_DEBUG("Err: Read RTSP second stream connection info fail\n");
				cGetStatusSock.Close();
				isConnected = FALSE;
			}
			break;
		case H264_STATUS_FIRST_STREAM_REDUCE:
		case H264_STATUS_SECOND_STREAM_REDUCE:
			hr = ReduceStreamBitrate(&cStatusHeader, &cGetStatusSock);
			if(FAILED(hr))
			{
				SW_TRACE_DEBUG("Err: RTSP second stream reduce bitrate fail\n");
				cGetStatusSock.Close();
				isConnected = FALSE;
			}
			break;
		case H264_STATUS_FIRST_STREAM_ASCEND:
		case H264_STATUS_SECOND_STREAM_ASCEND:
			hr = AscendStreamBitrate(&cStatusHeader, &cGetStatusSock);
			if(FAILED(hr))
			{
				SW_TRACE_DEBUG("Err: RTSP second stream ascend bitrate fail\n");
				cGetStatusSock.Close();
				isConnected = FALSE;
			}
			break;
		case H264_STATUS_AUTHENTICATE:
			hr = Authenticate(&cStatusHeader, &cGetStatusSock);
			if(FAILED(hr))
			{
				SW_TRACE_DEBUG("Err: RTSP authenticate fail\n");
				cGetStatusSock.Close();
				isConnected = FALSE;
			}
			break;
		default:
			SW_TRACE_DEBUG("RTSP read connect status:unknow command.");
			cGetStatusSock.Close();
			isConnected = FALSE;
			break;
		}
	}*/
	
	if( CSWDateTime::GetSystemTick() - dwLastUpdateTimeMs > 5 * 1000 )
		{
			CSWString strInfo = "";
			CHAR byConnInfo[512] = {0};
			DWORD dwOutDataLen = 0;
			DWORD dwInDataLen = 0;
			INT iGetStrLen = 0;
			
			if( m_iChannelId == 0 )
			{
				strInfo.Append("RTSPá′?ó:");
				eGetConnStatusCmd = H264_STATUS_FIRST_STREAM_CONN;
			}
			else
			{
				strInfo.Append("RTSPμú?t?·á′?ó:");
				eGetConnStatusCmd = H264_STATUS_SECOND_STREAM_CONN;
			}
			hr = cGetStatusSock.Send((VOID *)&eGetConnStatusCmd, sizeof(INT), &dwOutDataLen);
			if(FAILED(hr))
			{
				SW_TRACE_NORMAL("Err: send RTSP connection command fail\n");
				cGetStatusSock.Close();
				isConnected = FALSE;
				swpa_thread_sleep_ms(2000);
				continue;
			}
			hr = cGetStatusSock.Read((VOID *)&iGetStrLen, sizeof(INT), &dwInDataLen);
			if(FAILED(hr))
			{
				SW_TRACE_NORMAL("Err: Read RTSP connection info length fail\n");
				cGetStatusSock.Close();
				isConnected = FALSE;
				swpa_thread_sleep_ms(2000);
				continue;
			}
			hr = cGetStatusSock.Read((VOID *)byConnInfo, iGetStrLen, &dwInDataLen);
			if(FAILED(hr) || (dwInDataLen != iGetStrLen) || (dwInDataLen == 0))
			{
				SW_TRACE_NORMAL("Err: Read RTSP connection info fail\n");
				cGetStatusSock.Close();
				isConnected = FALSE;
				swpa_thread_sleep_ms(2000);
				continue;
			}
			strInfo.Append(byConnInfo);
			CHAR szMsg[512] = {0};
			swpa_strncpy(szMsg, (const CHAR*)strInfo, sizeof(szMsg));
			CSWMessage::SendMessage(MSG_APP_UPDATE_STATUS, (WPARAM)szMsg, 0);

			dwLastUpdateTimeMs = CSWDateTime::GetSystemTick();
		}
		else
		{
			swpa_thread_sleep_ms(2000);
		}
	}

	if(cGetStatusSock.IsValid())
		cGetStatusSock.Close();

	SW_TRACE_NORMAL("Info: %s: %s() exited\n", __FILE__, __FUNCTION__);
	
	return S_OK;
}

PVOID CSWRTSPTransformFilter::OnGetConnStatusProxy(PVOID pvArg)
{
	if(NULL == pvArg)
	{
		SW_TRACE_NORMAL("Err: NULL == pvArg\n");
		return NULL;
	}

	return (PVOID)((CSWRTSPTransformFilter *)pvArg)->OnGetConnStatus();
}




