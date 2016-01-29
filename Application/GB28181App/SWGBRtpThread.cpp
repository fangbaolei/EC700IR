

#include "SWFC.h"
#include "SWGBRtpThread.h"

SWGBRtpThread::SWGBRtpThread()
{
    INT iSize = 0;
    m_pbBuf = new BYTE[1*1024*1024];
    if (NULL == m_pbBuf)
    {
        SW_TRACE_NORMAL("new buf error  Err: NULL == GBRtpbuf \n");
        return;
    }
    m_GBthread_list.SetMaxCount(25);
    m_Rtpsock.Create();
    m_Rtpsock.Bind(NULL,5080);
    m_sendFirst      = TRUE;
	m_bPause = false;
	m_historystop = FALSE;
}

SWGBRtpThread::~SWGBRtpThread()
{
    delete[] m_pbBuf;
}

HRESULT SWGBRtpThread::Stop()
{
    SW_TRACE_DEBUG("CSWGB28181App--SWGBRtpThread CSWThread::Stop \n");
	CSWThread::Stop();
	return S_OK;
}

HRESULT	SWGBRtpThread::Start()
{
	SW_TRACE_DEBUG("CSWGB28181App--SWGBRtpThread::Setup ....\n");
    m_sendFirst = TRUE;
	m_bPause = false;
	m_historystop = FALSE;

	return CSWThread::Start();
}

void SWGBRtpThread::get_Timerecord(SWPA_DATETIME_TM* ptemp)
{
	m_cMutexLock_time.Lock();
	swpa_memcpy(ptemp,&timerecord,sizeof(SWPA_DATETIME_TM));
	m_cMutexLock_time.Unlock();	
}

BOOL SWGBRtpThread::get_Historystopflag()
{
	SW_TRACE_DEBUG("CSWGB28181App--SWGBRtpThread::get_Historystopflag:start..\n");         
	BOOL t_flag;
	m_cMutexLock_historystop.Lock();
	t_flag = m_historystop;
	m_cMutexLock_historystop.Unlock();

	return t_flag;
}

int SWGBRtpThread::set_GBAPPlist(PVOID pvArg)
{
	SW_TRACE_DEBUG("CSWGB28181App--SWGBRtpThread::set_GBAPPlist:start..\n");         
	client_info * client_info_tmp = (client_info*)pvArg;
	
	static DWORD m_numtemp = 0;
	SW_POSITION m_headpostion = NULL;
	
	m_cMutexLock.Lock();
	m_GBthread_list.AddTail(client_info_tmp);
	m_cMutexLock.Unlock();
	SW_TRACE_DEBUG("CSWGB28181App--SWGBRtpThread::set_GBAPPlist:end..\n");         
	return 0;
}

BOOL SWGBRtpThread::remove_listelment(int did)
{
	SW_TRACE_DEBUG("CSWGB28181App--SWGBRtpThread::remove_listelment:start..\n");         
	static DWORD m_numoflist_close = 0;
	
	SW_POSITION m_headpostion = NULL;
	SW_POSITION m_postion_tmp = NULL;
	
	m_cMutexLock.Lock();
	if(m_GBthread_list.IsEmpty())
	{
		m_cMutexLock.Unlock();
		SW_TRACE_DEBUG("CSWGB28181App--SWGBRtpThread::remove_listelment:IsEmpty\n");         
		return TRUE;
	}
	
	m_numoflist_close = m_GBthread_list.GetCount();
	m_headpostion = m_GBthread_list.GetHeadPosition();
	if(NULL == m_headpostion)
	{
		SW_TRACE_DEBUG("CSWGB28181App--SWGBRtpThread::remove_listelment:getheadposition failed \n");         
//		return -1;
	}
	
	for(int i=0;i<m_numoflist_close;i++)
	{
		client_info* client_info_tmp = NULL;
		m_postion_tmp = m_headpostion;
		client_info_tmp = m_GBthread_list.GetNext(m_headpostion);	//获取头位置后，自动指向下一个元素
		if( did == client_info_tmp->did)
		{
			SW_TRACE_DEBUG("CSWGB28181App--SWGBRtpThread::remove_listelment:RemoveAt \n");         
			m_GBthread_list.RemoveAt(m_postion_tmp);				//
		}
	}

	if(m_GBthread_list.IsEmpty())
	{
		m_cMutexLock.Unlock();
		SW_TRACE_DEBUG("CSWGB28181App--SWGBRtpThread::remove_listelment:IsEmpty\n");         
		return TRUE;
	}
	else
	{
		m_cMutexLock.Unlock();
		SW_TRACE_DEBUG("CSWGB28181App--SWGBRtpThread::remove_listelment:have date\n");         
		return FALSE;
	}
}


HRESULT	SWGBRtpThread::Run()
{
    H264Header header;
    CHAR rtpBuf[1500];
    DWORD temp;
	INT iFrameCount=0;
	DWORD getsystime=0;

	HRESULT hr = S_OK;	
	BOOL isConnected = FALSE;
	CHAR szGBH264SockFile[128] = {""};
	swpa_strcpy(szGBH264SockFile,"/tmp/GBh264.sock");
	
	CSWTCPSocket cLocalTcpSock;
	CSWDateTime cGetSystemtime;

	if (FAILED(cLocalTcpSock.Create(TRUE)))
	{
        printf("Info: GBH264 filter open %s failed.", szGBH264SockFile);
		return E_FAIL;
	}
	
    SW_TRACE_DEBUG("CSWGB28181App--SWGBRtpThread::Run()...\n");
    while(!m_fExit)
    {    	
		if (m_bPause)       
		{
			swpa_thread_sleep_ms(1000);
			continue;
		}

		if(false == cLocalTcpSock.IsConnect())
		{
			hr = cLocalTcpSock.Connect(szGBH264SockFile);
			if (FAILED(hr))
			{
				SW_TRACE_DEBUG("CSWGB28181App--SWGBRtpThread::Run(): connect /tmp/GBh264.sock failed.\n");
				swpa_thread_sleep_ms(2000);
				continue;
			}
			cLocalTcpSock.SetRecvTimeout(3000);
			cLocalTcpSock.SetSendTimeout(3000);
			SW_TRACE_DEBUG("CSWGB28181App--SWGBRtpThread::Run(): connect /tmp/GBh264.sock suees\n");
		}

        memset(&header,0,sizeof(header));
		HRESULT hd = cLocalTcpSock.Read(&header,sizeof(header),&temp);
        if (S_OK != hd)
        {
			SW_TRACE_DEBUG("CSWGB28181App--SWGBRtpThread::Run():Err:  cLocalTcpSock.Read(&iSize,sizeof(iSize)); \n");
			swpa_thread_sleep_ms(1000);
//			cLocalTcpSock.Close();
            continue;
        }

		if(header.iType == -1)
		{
			SW_TRACE_DEBUG("history over flag,header.iType = -1 \n");
			m_cMutexLock_historystop.Lock();
			m_historystop = TRUE;
			m_cMutexLock_historystop.Unlock();
		}
		
        set_Size(header.iWidth,header.iHeight);
        hd = cLocalTcpSock.Read(m_pbBuf,header.iSize,&temp);
        if (S_OK != hd)
        {
            SW_TRACE_DEBUG("CSWGB28181App--SWGBRtpThread::Run():cLocalTcpSock.Read(m_pbBuf,iSize);; \n");
			swpa_thread_sleep_ms(1000);
//			cLocalTcpSock.Close();
            continue;
        }

        if(temp != header.iSize)
        {
			SW_TRACE_DEBUG("Err:  cLocalTcpSock.Read size error  \n");
			continue;
        }

        static INT www = 0;
		if( www > 100 )
		{
			printf("CSWGB28181App--SWGBRtpThread::Run(): is ok\n");
			www = 0;
		}
		www++;
       
        if(m_sendFirst)
        {
            if(header.iType == 1)
            {   
            	printf("CSWGB28181App--SWGBRtpThread--????????I? header.iType == 1\n");
                m_sendFirst = FALSE;//第一帧为I帧
            }
            else
            {
            	//SW_TRACE_NORMAL("header.iType == else\n");
            	  continue;
            }
        }

        if(header.iType ==1)
        {
            temp = add_Frame((const char*)m_pbBuf,header.iSize,PACK_TYPE_FRAME_I);
			if(0 != temp )
				printf("CSWGB28181App--SWGBRtpThread---- PACK_TYPE_FRAME_IIIIIIIIIII\n");
        }
        else if(header.iType == 0)
        {
            temp = add_Frame((const char*)m_pbBuf,header.iSize,PACK_TYPE_FRAME_P);
			if(0 != temp )
				printf("CSWGB28181App--SWGBRtpThread---- PACK_TYPE_FRAME_PPPPPPPPPPP\n");
        } 
		
		m_cMutexLock_time.Lock();		
		swpa_memcpy(&timerecord,&header.timerecord,sizeof(SWPA_DATETIME_TM));
		m_cMutexLock_time.Unlock();	

    	memset(rtpBuf,0,sizeof(rtpBuf));
        INT iRtpLen;
	 	while((iRtpLen = get_RtpPacket(rtpBuf,sizeof(rtpBuf))) > 0)
    	{
			m_cMutexLock.Lock();    	
    		static DWORD m_numoflist = 0;
			SW_POSITION m_headpostion = NULL;
			m_numoflist= m_GBthread_list.GetCount();
			m_headpostion = m_GBthread_list.GetHeadPosition();

			for(int i=0;i<m_numoflist;i++)
			{
				client_info* client_info_tmp;
				client_info_tmp = m_GBthread_list.GetNext(m_headpostion);
				if (NULL==client_info_tmp)
				{
					printf("CSWGB28181App--SWGBRtpThread--get the element of m_GBthread_list failed \n");
					continue;
				}
				
		        DWORD iSucceslen;
	            hd = m_Rtpsock.SendTo(rtpBuf,iRtpLen,client_info_tmp->videoSendIP,client_info_tmp->videoSendPort,&iSucceslen);
	            if (S_OK != hd || iSucceslen != iRtpLen)
	            {
	                printf("CSWGB28181App--SWGBRtpThread--Err: sendto gb28181 m_Rtpsock.SendTo \n");
	            }
			}
			m_cMutexLock.Unlock();
		}
	}
	
	cLocalTcpSock.Close();
    printf("SWGBRtpThread::Run  return \n");
    return 0;
}




