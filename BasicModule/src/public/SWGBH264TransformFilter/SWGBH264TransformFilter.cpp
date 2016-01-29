/**
* @file SWGBH264Control.cpp
* @brief GBH264模块
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-07-19
* @version 1.0
*/


#include "SWFC.h"
#include "SWGBH264TransformFilter.h"
#include "DataTransmitter.h"
#include <dlfcn.h>

CSWGBH264TransformFilter::CSWGBH264TransformFilter()
    :CSWBaseFilter(2, 0)
    ,CSWMessage(MSG_GB28181_FILTER_BEGIN, MSG_GB28181_FILTER_END)
	,m_fInited(FALSE)
    ,m_dwQueueSize(25)
    ,m_nH264FramRate(25)
    ,m_pTransformClient(NULL)
    ,m_fCurrentSpeed(1.0)
    ,m_fIsHistoryVideo(FALSE)
    ,m_fIsPause(FALSE)
    ,m_fBackward(FALSE)
    ,m_fNeedReadNextSecond(FALSE)
{
	GetIn(0)->AddObject(CLASSID(CSWImage));
    GetIn(1)->AddObject(CLASSID(CSWImage));
}

CSWGBH264TransformFilter::~CSWGBH264TransformFilter()
{
	if (m_fInited)
	{
		Stop();
		m_fInited = FALSE;
	}
}

HRESULT CSWGBH264TransformFilter::Initialize()
{
	if (m_fInited)
	{
		return S_OK;
	}

	if (FAILED(m_cSemaSync.Create(0, m_dwQueueSize)))
	{
        SW_TRACE_NORMAL("[Initialize]: failed to create m_cSemaSync\n");
		return E_FAIL;
	}

	m_lstFrames.SetMaxCount(m_dwQueueSize);

    CHAR szGBH264SockFile[128] = "/tmp/GBh264.sock";
    if (FAILED(m_cLocalTcpSock.Create(TRUE)))
    {
        SW_TRACE_NORMAL("[Initialize]: GB28181 filter open %s failed.", szGBH264SockFile);
        return E_FAIL;
    }

    INT nRetryTimes = 50;
    HRESULT hr = S_OK;
	 hr = m_cLocalTcpSock.Bind(szGBH264SockFile);
    if (FAILED(hr))
    {
        SW_TRACE_NORMAL("[Initialize]: GBH264 filter bind %s failed.",szGBH264SockFile);
		return E_FAIL;
        
    }
    m_cLocalTcpSock.SetRecvTimeout(300000);
    m_cLocalTcpSock.SetSendTimeout(300000);
    if (FAILED(m_cLocalTcpSock.Listen(1)))
    {
        SW_TRACE_NORMAL("[Initialize]: failed to listen!\n");
        return E_FAIL;
    }
	
	m_fInited = TRUE;
    SW_TRACE_NORMAL("[Initialize]: Initialize OK...\n");
	
	return S_OK;
}

HRESULT CSWGBH264TransformFilter::Receive(CSWObject* obj)
{
	if (!m_fInited)
	{
        SW_TRACE_NORMAL("[Receive]: not initialized yet\n");
		return E_NOTIMPL;
	}

    //如果网络没有连接,直接丢弃
    if(NULL == m_pTransformClient)
    {
    	//SW_TRACE_DEBUG("%s:m_pTransformClient = NULL,don't receive data \n",__FUNCTION__);
        return S_OK;
    }

	if (IsDecendant(CSWImage, obj))
	{		
		CSWImage * pImage = (CSWImage*) obj;
		if (NULL == pImage)
		{
            SW_TRACE_NORMAL("[Receive]: NULL == pImage\n");
			return E_INVALIDARG;
		}
		
		if(m_fIsHistoryVideo)
		{
		
			if(pImage->GetType() == SW_IMAGE_H264)
			{
				return S_OK;
			}

			//如果队列满，则睡眠不丢帧
			while (m_lstFrames.IsFull() && FILTER_RUNNING == GetState())
			{
			     swpa_thread_sleep_ms(100); 
			}
			
            if(0 == pImage->GetWidth() && 0 == pImage->GetHeight())		//HDD模块发送最后一帧时，把图像宽高置零
            {
                //读取下一秒的标记
                BOOL fReadNextSecond = FALSE;

                //每次只发第一秒
                if(!m_fBackward)
                {
                    if(m_dtHistoryFrom < m_dtHistoryEnd || m_dtHistoryFrom == m_dtHistoryEnd)
                    {
                        fReadNextSecond = TRUE;
                    }
                    else
                    {
                        fReadNextSecond = FALSE;
                    }
                }
                else
                {
                    if(m_dtHistoryEnd > m_dtHistoryFrom || m_dtHistoryEnd == m_dtHistoryFrom)
                    {
                        fReadNextSecond = TRUE;
                    }
                    else
                    {
                        fReadNextSecond = FALSE;
                    }
                }

				m_fNeedReadNextSecond = fReadNextSecond;
                SW_TRACE_DEBUG("Get a End Frame,NeedReadNextSecond = %d,m_lstFrames.GetCount()=%d",m_fNeedReadNextSecond,m_lstFrames.GetCount());
       
                if(fReadNextSecond)
                {
                    return S_OK;
                }
            }
		}
		else
		{
			if(pImage->GetType() == SW_IMAGE_H264_HISTORY)
			{
				return S_OK;
			}
		}

		if (pImage->GetSize() > 384*1024) //Max= 384KB/F
		{
            SW_TRACE_NORMAL("Warning: Image Size > 384KB, can't handle this\n");
			return E_FAIL;
		}

        BOOL isFull = FALSE;
		m_cMutexLock.Lock();        
        //如果队列满丢弃上一帧
		if (m_lstFrames.IsFull())
		{
            SW_TRACE_DEBUG("Warning: GBH264 send queue is full, discards prev frame\n");
            CSWImage* pImageToBeDeleted = m_lstFrames.RemoveTail();
            SAFE_RELEASE(pImageToBeDeleted);
            isFull = TRUE;
		}

        //加入到队列尾
		SAFE_ADDREF(pImage);
		m_lstFrames.AddTail(pImage);
		m_cMutexLock.Unlock();

        if(!isFull)
        {
            if (FAILED(m_cSemaSync.Post()))
            {
                SW_TRACE_DEBUG("Warning: m_cSemaSync.Post() Failed \n");
            }
        }
	}

	return S_OK;
}

HRESULT CSWGBH264TransformFilter::Run()
{
	
	if (!m_fInited)
	{
        SW_TRACE_NORMAL("[FilterRun]: not inited yet\n");
		return E_OBJ_NO_INIT;
	}

	if (FAILED(CSWBaseFilter::Run()))
	{
        SW_TRACE_NORMAL("[FilterRun]: Failed to run CSWGBH264TransformFilter\n");
		return E_FAIL;
	}

	if (FAILED(m_cThreadAccept.Start(AcceptLinkProxy, (PVOID)this)))
	{
        SW_TRACE_NORMAL("[FilterRun]: Failed to start AcceptLink thread\n");
		return E_FAIL;
	}

    SW_TRACE_NORMAL("[FilterRun]: CSWGBH264TransformFilter running...\n");

	return S_OK;
}



PVOID CSWGBH264TransformFilter::AcceptLinkProxy(PVOID pvArg)
{
	if (NULL == pvArg)
	{
        SW_TRACE_NORMAL("[AcceptLinkProxy]:NULL == pvArg\n");
		return NULL;
	}
    return (PVOID)((CSWGBH264TransformFilter*)pvArg)->AcceptLink();
}


HRESULT CSWGBH264TransformFilter::AcceptLink()
{
	SW_TRACE_DEBUG("thread running...");
	while ((FILTER_RUNNING == GetState())&&(S_OK == m_cThreadAccept.IsValid()))
    {      
        //如果已经连接
        swpa_thread_sleep_ms(200);
        m_cMutexThread.Lock();
        if(NULL != m_pTransformClient)
        {
            if(FALSE == m_pTransformClient->GetTCP().IsConnect())
            {
            	SW_TRACE_DEBUG("TransformClient Socket Is Close.\n");
                m_pTransformClient->Stop();
                m_pTransformClient->Release();
                m_pTransformClient = NULL;
//				swpa_thread_sleep_ms(100);
            }
            else
            {
				if(m_fNeedReadNextSecond)
				{
					if((m_dwQueueSize - m_lstFrames.GetCount()) >= 15)
					{
						SW_TRACE_DEBUG("Begin Send ReadNextSecond Data,m_lstFrames.GetCount()=%d",m_lstFrames.GetCount());
						ReadNextSecondVideoData();
						m_fNeedReadNextSecond = FALSE;
					}
					swpa_thread_sleep_ms(20);
				}
				else
				{
					swpa_thread_sleep_ms(100);
				}
				m_cMutexThread.Unlock();
				continue;
            }
        }
		m_cMutexThread.Unlock();

        SWPA_SOCKET_T outSock;
        if (FAILED(m_cLocalTcpSock.Accept(outSock)))
        {
        	static INT tmpcount = 0;
			tmpcount++;
			if(tmpcount >= 100)
			{
				tmpcount = 0;
				SW_TRACE_NORMAL("[AcceptLink]: Accept failed!");
			}
        }
        else
        {
        	m_cMutexThread.Lock();
            m_pTransformClient = new CSWH264TransformClient(outSock,this);
            if(NULL == m_pTransformClient)
            {
                CSWTCPSocket sockData;
                sockData.Attach(outSock);
                sockData.Close();

                SW_TRACE_NORMAL("[AcceptLink]: Create New Send Thread Failed!\n");
            }

            if(FAILED(m_pTransformClient->Start()))
            {
               m_pTransformClient->Release();
               m_pTransformClient = NULL;
            }
			m_cMutexThread.Unlock();

            SW_TRACE_DEBUG("[AcceptLink]: GBH264 local socket make new connect.\n");
        }
    }
	SW_TRACE_DEBUG("thread stop...");

	return S_OK;

}

HRESULT CSWGBH264TransformFilter::Stop()
{
	if (!m_fInited)
	{
        SW_TRACE_NORMAL("[FilterStop]: not inited yet\n");
		return E_OBJ_NO_INIT;
	}

    

    if (FAILED(m_cThreadAccept.Stop()))
    {
        SW_TRACE_NORMAL("[FilterStop]: AcceptThread stop Failed\n");
        return E_FAIL;
    }
    
    //关闭Socket
    m_cLocalTcpSock.Close();

    //关闭数据发送线程
    m_cMutexThread.Lock();
    if(NULL != m_pTransformClient)
    {
        m_pTransformClient->Stop();
        m_pTransformClient->Release();
        m_pTransformClient = NULL;
    }
	m_cMutexThread.Unlock();

     //清除队列
    CSWImage* pImageToBeDeleted = NULL;
    while(NULL != (pImageToBeDeleted = GetFirstImage()))
    {
        SAFE_RELEASE(pImageToBeDeleted);
    }
    
    if (FAILED(CSWBaseFilter::Stop()))
		{
			SW_TRACE_NORMAL("Err: Failed to stop CSWRTSPTransformFilter\n");
			return E_FAIL;
		}

    SW_TRACE_NORMAL("[FilterStop]: CSWGBH264TransformFilter stopped\n");

    return S_OK;
}

CSWImage *CSWGBH264TransformFilter::GetFirstImage()
{
    CSWImage* pImage = NULL;

    if (SUCCEEDED(m_cSemaSync.Pend(40)))
    {
        m_cMutexLock.Lock();

        if(!m_lstFrames.IsEmpty())
        {
            pImage = m_lstFrames.RemoveHead();
        }
        m_cMutexLock.Unlock();
    }

    return pImage;
}

BOOL  CSWGBH264TransformFilter::GetCanBeginPlay()
{
    if(m_FirstPlay)
    {
        DWORD nListFrame = m_lstFrames.GetCount();
        if(nListFrame >= 15)
        {
       		m_FirstPlay = FALSE;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

HRESULT CSWGBH264TransformFilter::ReadNextSecondVideoData(INT seconds)
{
    if(seconds < 1)
    {
        return E_FAIL;
    }

    SWPA_DATETIME_TM    m_dtTempVarTimeFrom;        //临时缓冲用时间变量
    SWPA_DATETIME_TM    m_dtTempVarTimeEnd;         //临时缓冲用时间变量

    SWPA_TIME tiSecond;
    tiSecond.msec = 0;
    tiSecond.sec = 1;
    //读取下一秒的标记
    BOOL fReadNextSecond = FALSE;

    CSWDateTime tmpDateTime;

    SWPA_TIME paFrom;
    m_dtHistoryFrom.GetTime(&paFrom);
    SWPA_TIME paEnd;
    m_dtHistoryEnd.GetTime(&paEnd);
    INT span = paEnd.sec -  paFrom.sec;

    //每次只发第一秒,现在是直接从开始时间一直发送到结束时间
    if(!m_fBackward)
    {
        if(m_dtHistoryFrom < m_dtHistoryEnd || m_dtHistoryFrom == m_dtHistoryEnd)
        {
            m_dtHistoryFrom.GetTime(&m_dtTempVarTimeFrom);
            tmpDateTime = m_dtTempVarTimeFrom;
            if(span >= seconds)
            {
                tiSecond.sec = seconds - 1;
            }
            else
            {
                //如果跨距大于结束时间差调整为最后时间
                tiSecond.sec = span;
            }
            tmpDateTime += tiSecond;
            tmpDateTime.GetTime(&m_dtTempVarTimeEnd);

            tiSecond.sec = seconds;
            m_dtHistoryFrom += tiSecond;
            fReadNextSecond = TRUE;
        }
        else
        {
            fReadNextSecond = FALSE;
        }
    }
    else
    {
        if(m_dtHistoryEnd > m_dtHistoryFrom || m_dtHistoryEnd == m_dtHistoryFrom)
        {
            m_dtHistoryEnd.GetTime(&m_dtTempVarTimeEnd);
            tmpDateTime = m_dtTempVarTimeEnd;
            if(span >= seconds)
            {
                tiSecond.sec = -(seconds - 1);
            }
            else
            {
                //如果跨距大于结束时间差调整为最后时间
                tiSecond.sec = -span;
            }
            tmpDateTime += tiSecond;
            tmpDateTime.GetTime(&m_dtTempVarTimeFrom);

            tiSecond.sec = -seconds;
            m_dtHistoryEnd += tiSecond;
            fReadNextSecond = TRUE;
        }
        else
        {
            fReadNextSecond = FALSE;
        }
    }

    if(fReadNextSecond)
    {
        DWORD dwPinID = 3;
        HISTORY_GB28181_TRANSMITTING_PARAM sParam;
        //不是真正结束,读取下一秒
        sParam.psBeginTime = &m_dtTempVarTimeFrom;
        sParam.psEndTime = &m_dtTempVarTimeEnd;
        sParam.fBackward = m_fBackward;
        SW_TRACE_DEBUG("ReadNextSecondVideoData %d:%d:%d ~ %d:%d:%d",m_dtTempVarTimeFrom.hour,m_dtTempVarTimeFrom.min,m_dtTempVarTimeFrom.sec,
                                            m_dtTempVarTimeEnd.hour,m_dtTempVarTimeEnd.min,m_dtTempVarTimeEnd.sec);
        CSWMessage::SendMessage(MSG_H264HDD_FILTER_GB28181_FILE_TRANSMITTING_START,
                                (WPARAM)dwPinID, (LPARAM)&sParam);
    }

    return fReadNextSecond ? S_OK:E_FAIL;
}

HRESULT CSWGBH264TransformFilter::OnGB28181Command(PVOID pvBuffer, INT iSize)
{
	GB28181_Control_Param* pParam = (GB28181_Control_Param*)pvBuffer;
    m_dtHistoryFrom = pParam->sBeginTime;
    m_dtHistoryEnd = pParam->sEndTime;

    SW_TRACE_DEBUG("%s: Channel_ID: %d Command ID:%d Start:%d:%d:%d End:%d:%d:%d\n",__FUNCTION__,
																					pParam->Channel_ID,
																					pParam->Messege_ID,
					                                                                m_dtHistoryFrom.GetHour(),
					                                                                m_dtHistoryFrom.GetMinute(),
					                                                                m_dtHistoryFrom.GetSecond(),
					                                                                m_dtHistoryEnd.GetHour(),
					                                                                m_dtHistoryEnd.GetMinute(),
					                                                                m_dtHistoryEnd.GetSecond());
    

    m_fBackward = FALSE;
    m_fIsPause = FALSE;
    switch(pParam->Messege_ID)
    {
        case GB28181_CMD_PLAY:
            m_fCurrentSpeed = pParam->fltScale;
            m_fIsHistoryVideo = FALSE;
            CSWMessage::SendMessage(MSG_H264HDD_FILTER_GB28181_FILE_TRANSMITTING_STOP,NULL,NULL);
            break;
        case GB28181_CMD_PLAYBACK:
        case GB28181_CMD_DOWNLOAD:
        case GB28181_CMD_DRAG:
            m_fCurrentSpeed = pParam->fltScale;
            m_fIsHistoryVideo = TRUE;
            break;
        case GB28181_CMD_BYE:
            if(m_fIsHistoryVideo)
            {
                CSWMessage::SendMessage(MSG_H264HDD_FILTER_GB28181_FILE_TRANSMITTING_STOP,NULL,NULL);
            }
			m_cMutexThread.Lock();
            if(NULL != m_pTransformClient)
            {
                m_pTransformClient->Stop();
                m_pTransformClient->Release();
                m_pTransformClient = NULL;
            }
			m_cMutexThread.Unlock();
            m_fIsHistoryVideo = FALSE;
            break;
        case GB28181_CMD_BACKAWAY:
            m_fCurrentSpeed = pParam->fltScale;
            m_fIsHistoryVideo = TRUE;
            m_fBackward = TRUE;
            break;
        case GB28181_CMD_FORWARD:
            m_fCurrentSpeed = pParam->fltScale;
            m_fIsHistoryVideo = TRUE;
            break;
        case GB28181_CMD_PAUSE:
            m_fIsPause = TRUE;
            if(m_fIsHistoryVideo)
            {
                CSWMessage::SendMessage(MSG_H264HDD_FILTER_GB28181_FILE_TRANSMITTING_STOP,NULL,NULL);
            }
            m_fIsHistoryVideo = FALSE;
            break;
    }

    m_FirstPlay = TRUE;
    if(	GB28181_CMD_PLAY == pParam->Messege_ID 
		|| GB28181_CMD_PAUSE == pParam->Messege_ID
        || GB28181_CMD_BYE == pParam->Messege_ID
        || GB28181_CMD_DOWNLOAD == pParam->Messege_ID)
    {
        m_FirstPlay = FALSE;
    }

	//清除队列
	CSWImage* pImageToBeDeleted = NULL;
	while(NULL != (pImageToBeDeleted = GetFirstImage()))
	{
	   SAFE_RELEASE(pImageToBeDeleted);
	}
	SW_TRACE_DEBUG("%s:clean the Receive queue \n",__FUNCTION__);
   
	if(m_fIsHistoryVideo)
	{
	    SWPA_TIME paFrom;
		m_dtHistoryFrom.GetTime(&paFrom);
		SWPA_TIME paEnd;
		m_dtHistoryEnd.GetTime(&paEnd);
		INT span = paEnd.sec -  paFrom.sec+1;
		ReadNextSecondVideoData(span);
	}
}

HRESULT CSWH264TransformClient::Run()
{
    SW_TRACE_DEBUG("[H264TransformClientRun]:CSWH264TransformClient Running...\n");
    while (!m_fExit)
    {
        CSWImage* pImage = NULL;
        //如果网络出问题,等待处理
        if(!m_tcpClient.IsValid() || !m_tcpClient.IsConnect())
        {
            pImage = m_pFilter->GetFirstImage();
            if(NULL != pImage) SAFE_RELEASE(pImage);
            swpa_thread_sleep_ms(100);
			SW_TRACE_DEBUG("LocalSocket Is Disconnect.\n");
            continue;
        }

        //如果首次播放,则缓存到指定帧数
        if(FALSE == m_pFilter->GetCanBeginPlay())
        {
        	SW_TRACE_DEBUG("GetCanBeginPlay is false \n");
            swpa_thread_sleep_ms(100);
            continue;
        }

        //暂停
        if(m_pFilter->GetIsPause())
        {
            pImage = m_pFilter->GetFirstImage();
            if(NULL != pImage) SAFE_RELEASE(pImage);
            swpa_thread_sleep_ms(100);
			SW_TRACE_DEBUG("Send Is Pause.\n");
            continue;
        }

        //计算速度
        INT nInternet = (INT)(1000.0 / (m_pFilter->GetSpeed() * m_pFilter->GetH264FramRate()));
		INT LastSendTime = nInternet - (swpa_datetime_gettick() - m_dtLastSendTime);
        if(LastSendTime > 8)
        {
           // SW_TRACE_DEBUG("[H264TransformClientRun]: speed = %f H264FramRate=%d Internet=%d LastSendTime=%d...\n",
           // 			m_pFilter->GetSpeed(),m_pFilter->GetH264FramRate(),nInternet,LastSendTime);
			
            swpa_thread_sleep_ms(10);
			
            continue;
        }

        pImage = m_pFilter->GetFirstImage();
        if(pImage == NULL)
        {
        	SW_TRACE_DEBUG("GetFirstImage is empty \n");
            swpa_thread_sleep_ms(20);
            continue;
        }

				
        //发送头
        DWORD dwTimeHi = 0;
        DWORD dwTimeLow = 0;
        SWPA_TIME spaTime;
        swpa_memset(&spaTime,0,sizeof(SWPA_TIME));
        pImage->GetRealTime(&dwTimeHi,&dwTimeLow);
        CSWDateTime::TimeConvert(dwTimeHi,dwTimeLow,&spaTime);
        CSWDateTime frameTimeStamp;
        frameTimeStamp = spaTime;
        H264Header header;
        frameTimeStamp.GetTime(&header.timerecord);
        header.iHeight  =  pImage->GetHeight();
        header.iWidth   =  pImage->GetWidth();
        header.iSize    =  pImage->GetSize();
        if(0 == header.iHeight && 0 == header.iWidth)
        {
            header.iType = -1;
			SW_TRACE_DEBUG("history over flag,header.iType = -1 \n");
        }
        else
        {
            if(swpa_strcmp(pImage->GetFrameName(),"IFrame")==0)
            {
                header.iType = 1;
            }
            else
                header.iType = 0;
        }

        DWORD dwOutLengthSent = 0;
        HRESULT hr = m_tcpClient.Send((void*)&header, sizeof(header),&dwOutLengthSent);
        if (FAILED(hr) || dwOutLengthSent != sizeof(header))
        {
            SW_TRACE_DEBUG("[H264TransformClientRun]: failed to send H264 head size via GB28181 hr:0x%08x,Len:%d Sent:%d\n",
                hr,sizeof(header),dwOutLengthSent);
            m_tcpClient.Close();
            SAFE_RELEASE(pImage);
            continue;
        }

        //发送数据
        if(pImage->GetSize() > 0)
        {
            SW_COMPONENT_IMAGE sComponent;
            swpa_memset(&sComponent, 0, sizeof(sComponent));
            pImage->GetImage(&sComponent);
            dwOutLengthSent = 0;
            hr = m_tcpClient.Send((void*)sComponent.rgpbData[0], sComponent.iSize,&dwOutLengthSent);
            if (FAILED(hr) || dwOutLengthSent != sComponent.iSize)
            {
                SW_TRACE_DEBUG("[H264TransformClientRun]: failed to send H264 data size via RTSP hr:0x%08x,Len:%d Sent:%d\n",
                               hr,sComponent.iSize,dwOutLengthSent);
                m_tcpClient.Close();
            }
            else
            {
                //保存最后发送时间
                SW_TRACE_DEBUG("Send one frame %d \n",swpa_datetime_gettick()-m_dtLastSendTime);
                m_dtLastSendTime = swpa_datetime_gettick();
            }
        }

        SAFE_RELEASE(pImage);
    }
    SW_TRACE_DEBUG("[H264TransformClientRun]:CSWH264TransformClient Exited...\n");
}
