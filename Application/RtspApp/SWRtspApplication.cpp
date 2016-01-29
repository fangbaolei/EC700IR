/**
*/
#include "SWFC.h"
#include "swpa.h"
#include "SWRtspApplication.h"

#include "RTSPLIB.h"

CREATE_PROCESS(CSWRtspApp);

CSWRtspApp::CSWRtspApp()
	: CSWMessage(MSG_RTSP_BEGIN, MSG_RTSP_END)
{
	m_fIsGetParam = FALSE;
}

CSWRtspApp::~CSWRtspApp()
{
}

HRESULT CSWRtspApp::InitInstance(const WORD wArgc, const CHAR** szArgv)
{
	
	if(FAILED(CSWApplication::InitInstance(wArgc, szArgv)))
	{
		SW_TRACE_NORMAL("Err: failed to init CSWRtspApp\n");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSWRtspApp::ReleaseInstance()
{
	if(FAILED(CSWApplication::ReleaseInstance()))
	{
		SW_TRACE_NORMAL("Err: failed to release CSWRtspApp\n");
		return E_FAIL;
	}

	INT iErr = 0;
	if (!StopRTSP(&iErr))
	{
		SW_TRACE_NORMAL("Err: failed to stop RTSP Service, errcode=%d\n", iErr);
		return E_FAIL;
	}
	else
	{
		SW_TRACE_NORMAL("Info: RTSP Service stopped!\n");
		return S_OK;
	}

	return S_OK;
}

HRESULT CSWRtspApp::Run()
{
	BOOL fIsReady = FALSE;
	CHAR szIP[32] = {0};
	CHAR szMask[32] = {0};
	CHAR szGateway[32] = {0};
	CHAR szMac[32] = {0};

	while (!IsExited())
	{		
		if( !fIsReady && m_fIsGetParam)
		{
			if( 0 == swpa_tcpip_getinfo("eth0", szIP, 32, szMask, 32, szGateway, 32, szMac, 32)
				&& 0 != swpa_strcmp("0.0.0.0", szIP) )
			{
				//SW_TRACE_NORMAL("RTSP get ip:%s.", szIP);
				INT iErr = 0;
				//SW_TRACE_DEBUG("Info: CommunicationMode=%d FilterNum=%d\n", 
				//				m_cRtspParamInfo.iCommunicationMode,
				//				m_cRtspParamInfo.iRTSPStreamNum);
				if (!StartRTSP(&iErr, &m_cRtspParamInfo))
				{
					SW_TRACE_NORMAL("Err: failed to start RTSP Service, errcode=%d\n", iErr);
				}
				else
				{
					SW_TRACE_NORMAL("Info: RTSP Service started!\n");
					fIsReady = TRUE;
				}
			}
			else
			{
				SW_TRACE_NORMAL("RTSP get ip:%s.", szIP);
			}
		}
		if(!m_fIsGetParam)
		{
			//此处休眠是为了让LPRAPP初始化完毕
			swpa_thread_sleep_ms(2000);
			HRESULT hd = SendRemoteMessage(MSG_APP_GET_RTSP_PARAM, NULL, 0);
            if(hd == S_OK)
			{
				SW_TRACE_DEBUG(" SendRemoteMessage(MSG_APP_GET_RTSP_PARAM) OK\n");
			}
		}

		HeartBeat();		
		//PRINT("Info: HeartBea\n");
		swpa_thread_sleep_ms(6000);
	}

	return S_OK;
}

/**
 *@brief 取得代码的版本号 
 *@return 返回版本号,失败返回-1
 */
INT CSWRtspApp::GetVersion(VOID)
{
	return CSWApplication::GetVersion(); 
}

HRESULT CSWRtspApp::OnRtspGetParam(PVOID pvBuffer, INT iSize)
{

	RTSP_PARAM_INFO *pRtspParamInfo = (RTSP_PARAM_INFO *)pvBuffer;
	if(pRtspParamInfo == NULL)
	{
		SW_TRACE_NORMAL("Err: RTSP get param fail, errcode=%d\n");
		return S_FALSE;
	}

	swpa_memcpy(&m_cRtspParamInfo, pRtspParamInfo, sizeof(RTSP_PARAM_INFO));
	m_fIsGetParam = TRUE;

	SW_TRACE_NORMAL("CSWRtspApp::OnRtspGetParam\n");	
	return S_OK;
}

/**
 *@brief 进程退出,让守护进程重启
 *@return S_OK;
*/
HRESULT CSWRtspApp::OnRtspRestart(PVOID pvBuffer, INT iSize)
{
	SW_TRACE_DEBUG("Restart...\n");
	Exit(0x00);
	return S_OK;
}




