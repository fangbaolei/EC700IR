/**
*/
#include "SWFC.h"
#include "swpa.h"
#include "SW2AApplication.h"
#include "SW_Alg_Aewb.h"
#include <errno.h>


#define SW_2A_CTRL_CHECK(fun)   \
{								\
	int ret = fun;				\
	if ((ret))					\
	{							\
		printf("[%s:%d] SW_2A_Ctrl failed!ret=%d\n", __FILE__, __LINE__, ret);\
		return E_FAIL;			\
	}							\
}								\


CREATE_PROCESS(CSW2AApp);

static CSW2AApp* g_pApp = NULL;

static VOID SW_2A_Trace(const CHAR *string)
{
	//SW_TRACE_DEBUG("%s\n",string);
}


static PVOID SW_2A_Malloc(INT nSize)
{
	return swpa_mem_alloc(nSize);
}


static VOID SW_2A_Free(PVOID pPoint)
{
	SAFE_MEM_FREE(pPoint);
}


static INT SW_2A_FPGA_Reg_Read(DWORD dwAddr,PDWORD pValue)
{
	return swpa_fpga_reg_read(dwAddr, pValue);
}


static INT SW_2A_FPGA_Reg_Write(DWORD dwAddr, DWORD dwValue)
{
	return swpa_fpga_reg_write(dwAddr, dwValue);
}

static INT SW_2A_FPGA_Reg_Write_Mult(PDWORD pdwAddr, PDWORD pdwValue, DWORD dwCount)
{
	return swpa_fpga_multi_reg_write(dwCount, pdwAddr, pdwValue);
}

static VOID SW_2A_Set_PWM_DC(DWORD dwDuty, DWORD dwConstDuty)
{
	//todo
	static DWORD dwOldDuty = 0;
	if (dwConstDuty != dwOldDuty)// 先设置固定占空比值为75%
	{
		spwa_autoiris_pwm(dwDuty);

		dwOldDuty = dwConstDuty;
	}

	spwa_autoiris_pwm(dwDuty);
}

static VOID SW_2A_Set_Denoise_Status(DWORD dwTnfStatus, DWORD dwSnfStatus)
{
	//todo
	if (g_pApp)
	{
		g_pApp->SetDenoiseStatus(dwTnfStatus, dwSnfStatus);
	}
}


CSW2AApp::CSW2AApp()
	: CSWMessage(MSG_2A_BEGIN, MSG_2A_END)
	, m_pThreadExtendData(NULL)
	, m_dwReadRegAddr(0x0)
{
	CALL_MESSAGE_REGISTER_FUNCTION();

	m_fIsGetParam = FALSE;
}

CSW2AApp::~CSW2AApp()
{
}

HRESULT CSW2AApp::InitInstance(const WORD wArgc, const CHAR** szArgv)
{
	
	if(FAILED(CSWApplication::InitInstance(wArgc, szArgv)))
	{
		SW_TRACE_NORMAL("Err: failed to init CSWRtspApp\n");
		return E_FAIL;
	}
	
	g_pApp = this;
	
	SW_2A_Platform_Operator tPlatformCallBack;
	tPlatformCallBack.Printf = SW_2A_Trace;
	tPlatformCallBack.Malloc = SW_2A_Malloc;
	tPlatformCallBack.Free = SW_2A_Free;
	tPlatformCallBack.Spi_Read = SW_2A_FPGA_Reg_Read;
	tPlatformCallBack.Spi_Write = SW_2A_FPGA_Reg_Write;
	tPlatformCallBack.Spi_Write_Mult = SW_2A_FPGA_Reg_Write_Mult;
#if 1
	tPlatformCallBack.Set_Denoise_status = SW_2A_Set_Denoise_Status;
#else
	tPlatformCallBack.Set_Denoise_status = SetDenoiseStatusCallBack;
#endif
	tPlatformCallBack.Set_PWM_DC = SW_2A_Set_PWM_DC;

	if(SW_2A_Init(tPlatformCallBack))
	{	
		SW_TRACE_NORMAL("Err: failed to init SW2A\n");
		Exit(0);
	}
	m_pThreadExtendData = new CSWThread();
	if (NULL == m_pThreadExtendData)
	{
		SW_TRACE_NORMAL("Create ExtendData Thread Failed!\n");
		return E_OUTOFMEMORY;
	}
	
	return S_OK;
}

HRESULT CSW2AApp::ReleaseInstance()
{
	if (m_pThreadExtendData != NULL)
	{
		m_pThreadExtendData->Stop();
		SAFE_RELEASE(m_pThreadExtendData);
	}
	SW_2A_Release();
	if(FAILED(CSWApplication::ReleaseInstance()))
	{
		SW_TRACE_NORMAL("Err: failed to release CSW2AApp\n");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSW2AApp::Run()
{
	HRESULT hr;
	hr = m_pThreadExtendData->Start((START_ROUTINE)&CSW2AApp::OnProcessExtendDataProxy, (PVOID)this);

	while (!IsExited())
	{
		HeartBeat();		
		//PRINT("Info: HeartBea\n");
		swpa_thread_sleep_ms(6000);
	}

	return S_OK;
}

VOID CSW2AApp::OnProcessExtendDataProxy(PVOID pvParam)
{
	if (pvParam != NULL)
	{
		CSW2AApp *pThis = (CSW2AApp *)pvParam;
		pThis->OnProcessExtendData();
	}
}

HRESULT CSW2AApp::OnProcessExtendData(VOID)
{
	SW_TRACE_DEBUG("running ..........");

	HRESULT hr = S_OK;	
	BOOL isConnected = FALSE;
	CHAR szExtendDataSockFile[128] = {"/tmp/extenddata.sock"};
	
	CSWTCPSocket cLocalTcpSock;

	if (FAILED(cLocalTcpSock.Create(TRUE)))
	{
        SW_TRACE_NORMAL("Info: 2AApp open %s failed.", szExtendDataSockFile);
		return E_FAIL;
	}
	PBYTE pExtendData = (PBYTE)swpa_mem_alloc(8*3072);
	if (NULL == pExtendData)
	{
		SW_TRACE_NORMAL("alloc extendData buffer failed!\n");
		return E_OUTOFMEMORY;
	}

	while (S_OK == m_pThreadExtendData->IsValid() && !IsExited())
	{
		if(false == cLocalTcpSock.IsConnect())
		{
			if (false == cLocalTcpSock.IsValid() && FAILED(cLocalTcpSock.Create(TRUE)))
			{
		        SW_TRACE_NORMAL("Info: 2AApp try open %s failed.", szExtendDataSockFile);
				swpa_thread_sleep_ms(2000);
				continue;
			}
			hr = cLocalTcpSock.Connect(szExtendDataSockFile);
			if (FAILED(hr))
			{
				SW_TRACE_DEBUG("2AApp connect %s failed.\n", szExtendDataSockFile);
				swpa_thread_sleep_ms(2000);
				cLocalTcpSock.Close();
				continue;
			}
			cLocalTcpSock.SetRecvTimeout(3000);
			cLocalTcpSock.SetSendTimeout(3000);
			SW_TRACE_DEBUG("2AApp: connect %s sucess\n", szExtendDataSockFile);
		}
		
		typedef struct framehead
		{
			BYTE bySync;
			BYTE byType;
			BYTE byReserve[2]; 
			DWORD dwTimeStamp;
			DWORD dwDataLen;
						
#define EXTEND_HEAD_SYNC 0x47
#define EXTEND_DATA_TYPE 0x2
			framehead()
			{
				bySync = EXTEND_HEAD_SYNC;
				byType = EXTEND_DATA_TYPE;
				dwTimeStamp = 0;
				dwDataLen = 0;
			}
		}tDataHead;

		tDataHead head;

        memset(&head,0,sizeof(head));
		DWORD dwReadLen = 0;
		HRESULT hd = cLocalTcpSock.Read(&head,sizeof(head),&dwReadLen);
        if (S_OK != hd)
        {
			SW_TRACE_DEBUG("2AApp: cLocalTcpSock.Read(&iSize,sizeof(iSize)); %d\n",errno);
			swpa_thread_sleep_ms(1000);
			cLocalTcpSock.Close();
            continue;
        }

		if(head.dwDataLen<=0)
		{
			SW_TRACE_DEBUG("2AApp: cLocalTcpSock.Read head len=%d \n",head.dwDataLen);
			swpa_thread_sleep_ms(1000);
			cLocalTcpSock.Close();
            continue;
		}
		
        hd = cLocalTcpSock.Read(pExtendData,head.dwDataLen,&dwReadLen);
        if (S_OK != hd)
        {
            SW_TRACE_DEBUG("2AApp: cLocalTcpSock.Read(m_pbBuf,iSize);; \n");
			swpa_thread_sleep_ms(1000);
			cLocalTcpSock.Close();
            continue;
        }
		SW_2A_Process((void *)pExtendData);
#if 1
		static INT nCount = 0;
		if (nCount++%100 == 0)
			SW_TRACE_NORMAL("CSW2AApp Process Extend Data,data len:%d ..............\n",dwReadLen);
		
		INT *pInfo = (INT *)pExtendData;
		if (pInfo[7])
		{
			DWORD dwCurTick = CSWDateTime::GetSystemTick();
			SW_TRACE_NORMAL("capture info:%d,LprApp timestamp:%d, cur:%d, interval:%d\n",
				pInfo[8], head.dwTimeStamp, dwCurTick, dwCurTick - head.dwTimeStamp);
		}
#endif	
	}
	
	cLocalTcpSock.Close();

	SW_TRACE_NORMAL("2AApp extend data process thread exit.\n");

}


/**
 *@brief 取得代码的版本号 
 *@return 返回版本号,失败返回-1
 */
INT CSW2AApp::GetVersion(VOID)
{
	return CSWApplication::GetVersion(); 
}

/*VOID CSW2AApp::SetDenoiseStatusCallBack(VOID *pvContext, DWORD dwTnfStatus, DWORD dwSnfStatus)
{
	if (pvContext != NULL)
	{
		CSW2AApp* pThis = (CSW2AApp *)pvContext;
		pThis->SetDenoiseStatus(dwTnfStatus, dwSnfStatus);
	}
	
}*/

VOID CSW2AApp::SetDenoiseStatus(DWORD dwTnfStatus, DWORD dwSnfStatus)
{
	DWORD dwDenoiseStatus[2];
	dwDenoiseStatus[0] = dwTnfStatus;
	dwDenoiseStatus[1] = dwSnfStatus;
	SW_TRACE_DEBUG("CSW2App SetDenoiseStatus %d,%d\n", dwTnfStatus, dwSnfStatus);
	
	SendMessage(MSG_APP_SET_DENOISE_STATUS, (WPARAM)&dwDenoiseStatus, sizeof(dwDenoiseStatus));
}

HRESULT CSW2AApp::OnSetExp(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_SHUTTER, pvBuffer, iSize));
	
    return S_OK;
}
HRESULT CSW2AApp::OnGetExp(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_SHUTTER, pvBuffer, iSize));
	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);
    return S_OK;
}
HRESULT CSW2AApp::OnSetGain(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_GAIN, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetGain(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_GAIN, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);
	
    return S_OK;
}
HRESULT CSW2AApp::OnSetRGB(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_RGB_GAIN, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetRGB(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_RGB_GAIN, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}
HRESULT CSW2AApp::OnSetAEState(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_AE_STATE, pvBuffer, iSize));

    return S_OK;
}
HRESULT CSW2AApp::OnGetAEState(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_AE_STATE, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}
HRESULT CSW2AApp::OnSetAEThreshold(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_AE_THRESHOLD, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetAEThreshold(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_AE_THRESHOLD, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}

HRESULT CSW2AApp::OnSetAEExpRange(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_AE_EXP_RANGE, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetAEExpRange(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_AE_EXP_RANGE, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}
HRESULT CSW2AApp::OnSetAEZone(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_AE_ZONE, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetAEZone(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_AE_ZONE, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}
HRESULT CSW2AApp::OnSetAEGainRange(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_AE_GAIN_RANGE, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetAEGainRange(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_AE_GAIN_RANGE, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}
HRESULT CSW2AApp::OnSetScene(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_SCENE, pvBuffer, iSize));
    return S_OK;
}
//
HRESULT CSW2AApp::OnSetAWBState(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_AWB_STATE, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetAWBState(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_AWB_STATE, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}

HRESULT CSW2AApp::OnSetWDRStrength(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_WDR_STRENGHT, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetWDRStrength(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_WDR_STRENGHT, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}
//todo已经不支持
/*
HRESULT CSW2AApp::OnSetSharpnessState(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("%s.......\n",__FUNCTION__);
	//
    return S_OK;
}
HRESULT CSW2AApp::OnGetSharpnessState(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("%s.......\n",__FUNCTION__);
	
    return S_OK;
}*/
//todo已经不支持
/*
HRESULT CSW2AApp::OnSetSharpness(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	
	//SW_2A_Ctrl(SW2A_CMD_SET_SHARPNESS, pvBuffer, iSize);//mark

    return S_OK;
}
HRESULT CSW2AApp::OnGetSharpness(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("%s.......\n",__FUNCTION__);

    return S_OK;
}
*/
HRESULT CSW2AApp::OnSetSaturationContrastState(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_COLOR_ENABLE, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetSaturationContrastState(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_COLOR_ENABLE, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}
HRESULT CSW2AApp::OnSetSaturation(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_SATURATION, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetSaturation(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_SATURATION, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}

HRESULT CSW2AApp::OnSetContrast(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_CONTRAST, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetContrast(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_CONTRAST, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}

HRESULT CSW2AApp::OnSetTNFState(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_TNF_STATUS, pvBuffer, iSize));
	return S_OK;
}
HRESULT CSW2AApp::OnGetTNFState(WPARAM wParam, LPARAM lParam)
{
	
	return S_OK;
}
HRESULT CSW2AApp::OnSetSNFState(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_SNF_STATUS, pvBuffer, iSize));
	return S_OK;
}
HRESULT CSW2AApp::OnGetSNFState(WPARAM wParam, LPARAM lParam)
{
	
	return S_OK;
}

//todo已经不支持
/*
HRESULT CSW2AApp::OnSetTrafficLigthEnhanceState(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("%s.......\n",__FUNCTION__);

    return S_OK;
}
HRESULT CSW2AApp::OnGetTrafficLigthEnhanceState(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("%s.......\n",__FUNCTION__);

    return S_OK;
}*/
	
HRESULT CSW2AApp::OnSetTrafficLigthEnhanceRegion(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s.......\n",__FUNCTION__);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_TRAFFIC_LIGHT_ENHANCE_REGION, pvBuffer, iSize));
    return S_OK;
}

HRESULT CSW2AApp::OnSetTrafficLigthLUMTh(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_TRAFFIC_LIGHT_LUM_TH, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnSetCammaEnable(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_GAMMA_ENABLE, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetCammaEnable(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_GAMMA_ENABLE, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}
HRESULT CSW2AApp::OnSetCamma(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_GAMMA, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetCamma(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_GAMMA, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}

HRESULT CSW2AApp::OnSetEDGEEnhance(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_EDGE_ENHANCE, pvBuffer, iSize));

    return S_OK;
}
HRESULT CSW2AApp::OnGetEDGEEnhance(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_EDGE_ENHANCE, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}
HRESULT CSW2AApp::OnSetDCIRISAutoState(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_AUTO_DC_STATE, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetDCIRISAutoState(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_AUTO_DC_STATE, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}
HRESULT CSW2AApp::OnSetZOOMDCIRIS(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("%s.......\n",__FUNCTION__);

    return S_OK;
}
HRESULT CSW2AApp::OnSetShrinkDCIRIS(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("%s.......\n",__FUNCTION__);

    return S_OK;
}
HRESULT CSW2AApp::OnSetFilterState(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("%s.......\n",__FUNCTION__);

    return S_OK;
}
//
HRESULT CSW2AApp::OnSetAcSyncState(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_AC_SYNC_STATE, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetAcSyncState(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_AC_SYNC_STATE, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}

HRESULT CSW2AApp::OnSetAcSyncDelay(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_AC_SYNC_PARAM, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetAcSyncDelay(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_AC_SYNC_PARAM, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}
HRESULT CSW2AApp::OnSetIOArg(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s.......\n",__FUNCTION__);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_IO_ARG, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetIOArg(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_IO_ARG, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s.......\n",__FUNCTION__);

    return S_OK;
}
HRESULT CSW2AApp::OnSetEDGEMod(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_EDGE_MOD, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetEDGEMod(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_EDGE_MOD, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}
HRESULT CSW2AApp::OnSetGrayImageState(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_GRAY_IMAGE_STATE, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetGrayImageState(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_GRAY_IMAGE_STATE, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);

    return S_OK;
}
HRESULT CSW2AApp::OnSetFPGAReg(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	PDWORD pValue = (PDWORD)pvBuffer;
	DWORD Value = (pValue[0] << 20) | (pValue[1] & 0xFFFFF);
	SW_TRACE_DEBUG("%s.......addr:0x%x,value:0x%x,setvalue:0x%x\n",
		__FUNCTION__,pValue[0],pValue[1],Value);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_REG, (PVOID)&Value, sizeof(DWORD)));
    return S_OK;
}
HRESULT CSW2AApp::OnSetFPGAReadRegAddr(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	PDWORD pValue = (PDWORD)pvBuffer;
	m_dwReadRegAddr = pValue[0];
	
	SW_TRACE_DEBUG("%s.......read reg addr:0x%x,\n",
		__FUNCTION__,m_dwReadRegAddr);

    return S_OK;
}
HRESULT CSW2AApp::OnGetFPGAReg(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	PDWORD pValue = (PDWORD)pvBuffer;
#if 1		//todo:现有进程间消息机制无法带读取寄存器地址过来
	DWORD Value = ((m_dwReadRegAddr << 20) & 0xFFFF0000);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_REG, &Value, sizeof(DWORD)));
	pValue[0] = (Value & 0xFFFFFFFF);
	SW_TRACE_DEBUG("%s.......addr:0x%x,getvalue:0x%x\n",
		__FUNCTION__,m_dwReadRegAddr,pValue[0]);

#else
	DWORD Value = ((pValue[0] << 20) & 0xFFFF0000);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_REG, &Value, sizeof(DWORD)));
	pValue[1] = (Value & 0xFFFFFFFF);
	SW_TRACE_DEBUG("%s.......addr:0x%x,getvalue:0x%x\n",
		__FUNCTION__,pValue[0],pValue[1]);
#endif
	return S_OK;
}
HRESULT CSW2AApp::OnSetWorkMode(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_WORK_MODE, pvBuffer, iSize));

    return S_OK;
}
HRESULT CSW2AApp::OnSetSoftCapture(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_SOFT_CAPTURE, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnSetFlashGate(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_FLASH_GATE, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetVersion(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;
	CHAR *szVersion = SW_2A_Get_Version();
	SW_TRACE_DEBUG("%s.......:%s\n",__FUNCTION__, szVersion);
	swpa_memset(pvBuffer, 0x00, iSize);
	swpa_strncpy((CHAR *)pvBuffer, szVersion, iSize);
    return S_OK;
}
HRESULT CSW2AApp::OnSetCapShutter(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_CAPT_SHUTTER, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnSetCapGain(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)wParam);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_CAPT_GAIN, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnSetCapSharpen(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_CAPT_SHARPEN, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnSetCapRGB(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_CAPT_RGB, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnGetDayNight(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)lParam;
	INT iSize = (INT)wParam;

	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_GET_DAY_LIGHT, pvBuffer, iSize));

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__,*(PDWORD)pvBuffer);
	
    return S_OK;
}
HRESULT CSW2AApp::OnSetBackLight(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_BACK_LIGHT, pvBuffer, iSize));
    return S_OK;
}

HRESULT CSW2AApp::OnSetAWBMode(WPARAM wParam, LPARAM lParam)
{
	PVOID pvBuffer = (PVOID)wParam;
	INT iSize = (INT)lParam;

	SW_TRACE_DEBUG("%s....... %d\n",__FUNCTION__, *(PDWORD)pvBuffer);
	SW_2A_CTRL_CHECK(SW_2A_Ctrl(SW2A_CMD_SET_AWB_MODE, pvBuffer, iSize));
    return S_OK;
}
HRESULT CSW2AApp::OnSetRelayState(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("%s.......\n",__FUNCTION__);

    return S_OK;
}

