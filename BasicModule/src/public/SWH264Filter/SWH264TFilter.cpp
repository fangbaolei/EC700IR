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
#include "SWH264Filter.h"

#include <dlfcn.h>

#define PRINT SW_TRACE_DEBUG

CSWH264Filter::CSWH264Filter()
    :CSWBaseFilter(1, 0),CSWMessage(MSG_H264_BEGIN, MSG_H264_END)
	,m_fInited(FALSE)
	,m_dwQueueSize(25)
{
	GetIn(0)->AddObject(CLASSID(CSWImage));
    swpa_memset(m_lstPreview,0,sizeof(m_lstPreview));
    m_H264Rtp.SetPackMode(1);
    m_Rtpsock.Create();
    m_Rtpsock.Bind(NULL,5088);
    m_PreviewCount = 0;

}


CSWH264Filter::~CSWH264Filter()
{
	if (m_fInited)
	{
		Stop();
		m_fInited = FALSE;
	}
}

HRESULT CSWH264Filter::Initialize()
{
	if (m_fInited)
	{
		return S_OK;
	}

	if (FAILED(m_cSemaSync.Create(0, m_dwQueueSize)))
	{
		PRINT("Err: failed to create m_cSemaSync\n");
		return E_FAIL;
	}

	m_lstFrames.SetMaxCount(m_dwQueueSize);
	
	m_fInited = TRUE;
	return S_OK;
}



HRESULT CSWH264Filter::Receive(CSWObject* obj)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	if (IsDecendant(CSWImage, obj))
	{		
		CSWImage * pImage = (CSWImage*) obj;
		if (NULL == pImage)
		{
			PRINT("Err: NULL == pImage\n");
			return E_INVALIDARG;
		}

		if (pImage->GetSize() > 384*1024) //Max= 384KB/F
		{
			PRINT("Err: Image Size > 384KB, can't handle this\n");
			return E_FAIL;
		}

		m_cMutexLock.Lock();
		if (m_lstFrames.IsFull())
		{
            //PRINT("Warning: GBH264 send queue is full, discards oldest frame\n");
			CSWImage* pImageToBeDeleted = m_lstFrames.RemoveHead();
			SAFE_RELEASE(pImageToBeDeleted);

		}
		SAFE_ADDREF(pImage);
		m_lstFrames.AddTail(pImage);
		m_cMutexLock.Unlock();

		if (FAILED(m_cSemaSync.Post()))
		{
			//PRINT("Err: failed to post, discards the last H264 obj\n");
		}
	}

	return S_OK;
}



HRESULT CSWH264Filter::Run()
{
	if (!m_fInited)
	{
		SW_TRACE_NORMAL("Err: not inited yet\n");
		return E_OBJ_NO_INIT;
	}

	if (FAILED(CSWBaseFilter::Run()))
	{
        SW_TRACE_NORMAL("Err: Failed to run CSWH264Filter\n");
		return E_FAIL;
	}

	if (FAILED(m_cThreadSend.Start(OnSendDataProxy, (PVOID)this)))
	{
        PRINT("Err: failed to start RTP H264Transform send thread\n");
		return E_FAIL;
	}

    SW_TRACE_NORMAL("Info: CSWH264Filter running...\n");
	return S_OK;
}


HRESULT CSWH264Filter::Stop()
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_OBJ_NO_INIT;
	}

	if (FAILED(CSWBaseFilter::Stop()))
	{
        PRINT("Err: Failed to stop CSWH264Filter\n");
		return E_FAIL;
	}

	m_cMutexLock.Lock();
	while (!m_lstFrames.IsEmpty())
	{
		CSWImage* pImageToBeDeleted = m_lstFrames.RemoveHead();
		SAFE_RELEASE(pImageToBeDeleted);
	}
	m_cMutexLock.Unlock();

	

	if (FAILED(m_cThreadSend.Stop()))
	{
        PRINT("Err: failed to stop RTP H264Transform send thread\n");

		return E_FAIL;
	}

    SW_TRACE_NORMAL("Info: CSWH264Filter stopped\n");
	return S_OK;
}
	

HRESULT CSWH264Filter::OnSendData()
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_OBJ_NO_INIT;
	}


	HRESULT hr = S_OK;
    DWORD temp;
    SW_TRACE_NORMAL("Info: RTP H264 filter run.");
    CHAR rtpBuf[1500];
	INT iFrameCount = 0;
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
            m_cPreviewMutexLock.Lock();
            if (NULL != pImage && m_PreviewCount)
			{
				SW_COMPONENT_IMAGE sComponent;
                swpa_memset(&sComponent, 0, sizeof(sComponent));
                pImage->GetImage(&sComponent);
                if(swpa_strcmp(pImage->GetFrameName(),"IFrame")==0)
                {
                    temp = m_H264Rtp.AddFrame((const char*)sComponent.rgpbData[0],sComponent.iSize,PACK_TYPE_FRAME_I);
                }
                else
                    temp = m_H264Rtp.AddFrame((const char*)sComponent.rgpbData[0],sComponent.iSize,PACK_TYPE_FRAME_P);

                memset(rtpBuf,0,sizeof(rtpBuf));
                INT iRtpLen;

                while((iRtpLen = m_H264Rtp.GetRtpPacket(rtpBuf,sizeof(rtpBuf))) > 0)
                {
                    DWORD iSucceslen;
                    for(int i = 0;i< MAX_PREVIEW;i++)
                    {
                        if(  m_lstPreview[i].port != 0 && swpa_strlen(m_lstPreview[i].ip) != 0)
                        {
                            hr = m_Rtpsock.SendTo(rtpBuf,iRtpLen,m_lstPreview[i].ip,m_lstPreview[i].port,&iSucceslen);
                            if (S_OK != hr || iSucceslen != iRtpLen)
                            {
                                SW_TRACE_NORMAL("Err: sendto RTP H264 m_Rtpsock.SendTo \n");
                            }
                        }
                    }
                }

				//check heartbeat
				if (0 == iFrameCount++%(25*10))	//10s check
				{
					DWORD dwTimeTick = CSWDateTime::GetSystemTick();
					for (int i=0; i<MAX_PREVIEW; i++)
					{
						if (m_lstPreview[i].port != 0 
							&& swpa_strlen(m_lstPreview[i].ip) != 0
							&& (dwTimeTick - m_lstPreview[i].dwHeartBeatTime) > 30*1000)	//30s timeout
						{
							SW_TRACE_NORMAL("RTP H264 <%s:%d> Heartbeat time out:%d, last: %d,cur:%d",
								m_lstPreview[i].ip,m_lstPreview[i].port,dwTimeTick - m_lstPreview[i].dwHeartBeatTime,
								m_lstPreview[i].dwHeartBeatTime,dwTimeTick);
							swpa_memset(m_lstPreview[i].ip,0,sizeof(m_lstPreview[i].ip));
                			m_lstPreview[i].port = 0;
							m_lstPreview[i].dwHeartBeatTime = 0;
						}
					}
				}
				
			}
            SAFE_RELEASE(pImage);
            m_cPreviewMutexLock.Unlock();

        }
	}
	return S_OK;
}


PVOID CSWH264Filter::OnSendDataProxy(PVOID pvArg)
{
	if (NULL == pvArg)
	{
		PRINT("Err: NULL == pvArg\n");
		return NULL;
	}
    return (PVOID)((CSWH264Filter*)pvArg)->OnSendData();
}

HRESULT CSWH264Filter::OnH264DoPreview(WPARAM wParam, LPARAM lParam)
{
    if ((NULL == wParam) || (NULL == (wParam+1)) || (NULL == (wParam+2)))
    {
        SW_TRACE_NORMAL("Err: OnH264DoPreview NULL\n");
        return NULL;
    }
    DWORD *msg = (DWORD *)wParam;
    char ip[32];
    sprintf(ip,"%d.%d.%d.%d",msg[1] & 0xff,
            (msg[1] & 0xff00) >> 8,
            (msg[1] & 0xff0000) >> 16,
            (msg[1] & 0xff000000) >> 24);
    SW_TRACE_NORMAL("OnH264DoPreview %d %s:%d PreviewCount:%d\n",msg[0],ip,msg[2],m_PreviewCount);
    if(msg[0])
    {
    	for (INT i = 0; i<MAX_PREVIEW; i++)		//心跳
    	{
    		if(m_lstPreview[i].port == msg[2] && swpa_strcmp(m_lstPreview[i].ip,ip) ==0 )
        	{
        		m_lstPreview[i].dwHeartBeatTime = CSWDateTime::GetSystemTick();
				return S_OK;
        	}
    	}
		
        for(INT i = 0;i<MAX_PREVIEW;i++)
        {
            if(m_lstPreview[i].port == 0 && swpa_strlen(m_lstPreview[i].ip) == 0)	//增加
            {
                m_cPreviewMutexLock.Lock();
				m_lstPreview[i].dwHeartBeatTime = CSWDateTime::GetSystemTick();	//有先更新心跳，线程同步问题
                swpa_strcpy(m_lstPreview[i].ip,ip);
                m_lstPreview[i].port = msg[2];
                m_PreviewCount++;
                m_cPreviewMutexLock.Unlock();
                return S_OK;
            }
        }
        return E_FAIL;
    }
    else
    {
        for(INT i = 0;i< MAX_PREVIEW;i++)
        {
            if(m_lstPreview[i].port == msg[2] && swpa_strcmp(m_lstPreview[i].ip,ip) ==0 )
            {
                m_cPreviewMutexLock.Lock();
                swpa_memset(m_lstPreview[i].ip,0,sizeof(m_lstPreview[i].ip));
                m_lstPreview[i].port = 0;
				m_lstPreview[i].dwHeartBeatTime = 0;
                m_PreviewCount--;
                m_cPreviewMutexLock.Unlock();
                return S_OK;
            }
        }
        return E_FAIL;
    }
    return E_FAIL;

   // int i = 0;
}



