/**
*/
#include "SWFC.h"
#include "swpa.h"
#include "Autotest.h"
#include "AutotestJupiter.h"
#include "SWUpgradeApplication.h"
#include "Tester.h"
#include "SWTCPSocket.h"
#include "drv_device.h"
#include "SWNetOpt.h"
#include <sys/socket.h>
#include <arpa/inet.h>

#include "tinyxml.h"

HRESULT CAutotestJupiter::Initialize()
{
	if (m_fInited)
	{
		return S_OK;
	}

    // 初始化球机控制 必须在自动化测试之前
    if (S_OK != swpa_camera_init())
    {
        SW_TRACE_DEBUG("Err: failed to Initial Camera!\n");
        return E_FAIL;
    }

    if (S_OK != swpa_camera_basicparam_set_AF(AF_AUTO))
    {
        SW_TRACE_DEBUG("Err: failed to Set AF!\n");
        return E_FAIL;
    }
    else
    {
        SW_TRACE_NORMAL("Suc: Set AF auto!!\n");
    }

    // 读取机芯初始参数，判断是否需要设置，一般在生产时已设置。
    // 需断电上电才能生效。
    MONITOR_MODE  mMode;
    int iNeedResetCamera = 0;
    int iSyncMode;
    if( 0 == swpa_camera_get_monitoring_mode(&mMode)
            && mMode != MODE_1080P_25 )
    {
        SW_TRACE_DEBUG("Suc: set monitoring_mode %d.\n", MODE_1080P_25);
        swpa_camera_set_monitoring_mode(MODE_1080P_25);

        iNeedResetCamera = 1;
    }

    if( 0 == swpa_camera_get_sync_mode(&iSyncMode)
            && iSyncMode != 0)
    {
        SW_TRACE_DEBUG("Suc: set sync_mode 0.\n");
        swpa_camera_set_sync_mode(0);

        iNeedResetCamera = 1;

    }

    if (iNeedResetCamera == 1)
    {
        swpa_camera_reset();
    }

	//swpa_memset(m_szSavePath, 0, sizeof(m_szSavePath));
	swpa_memset(m_szSendIp, 0, sizeof(m_szSendIp));
	swpa_memset(m_sResultTable, 0, sizeof(m_sResultTable));
	m_wSendPort = 0;
	m_iTestCount = 0;

	if (NULL == m_pAutotestThread)
	{
		m_pAutotestThread = new CSWThread();
		if (NULL == m_pAutotestThread)
		{
			SW_TRACE_DEBUG("Err: no memory for m_pAutotestThread\n");
			return E_OUTOFMEMORY;
		}		
	}

	m_fInited = TRUE;

	return S_OK;
	
}

HRESULT CAutotestJupiter::ReleaseObj()
{
	if (!m_fInited)
	{
		return S_OK;
	}
	
	SetAutotestStopFlag(TRUE);

	if (NULL != m_pAutotestThread)
	{
		delete m_pAutotestThread;
		m_pAutotestThread = NULL;
	}

//	CTester::DeinitVPIF();//todo:no vpif on venus??
	
	m_fInited = FALSE;

    if (S_OK != swpa_camera_deinit())
    {
        SW_TRACE_DEBUG("Err: failed to DeInitial Camera!\n");
        return E_FAIL;
    }

	SW_TRACE_NORMAL("Info: Autotest Released!!\n");

	return S_OK;
}

HRESULT CAutotestJupiter::AutotestRunOnce(_TEST_RESULT* sResult, DWORD& dwCount)
{
#define RUN_TEST(TEST_ITEM, TEST_FNC)\
	{\
		swpa_strncpy(sResult[dwItem].szItemName, TEST_ITEM, sizeof(sResult[dwItem].szItemName)-1);\
		sResult[dwItem].fOK = TRUE;\
		if (FAILED(TEST_FNC()))\
		{\
			SW_TRACE_DEBUG("Err: test %s : FAILED!\n", TEST_ITEM);\
			sResult[dwItem].fOK = FALSE;\
			sResult[dwItem].dwFailCount ++;\
			fOK = FALSE;\
		}\
		if (dwItem+1>dwCount)\
		{\
			return E_OUTOFMEMORY;\
		}\
		dwItem++;\
		if (GetAutotestStopFlag())\
		{\
			SW_TRACE_DEBUG("Info: Autotest is forced to stop!\n");\
			return S_OK;\
		}\
	}

	DWORD dwItem = 0;
	BOOL fOK = TRUE;

	RUN_TEST("FLASH", CTester::TestFlash);
	RUN_TEST("IIC_EEPROM", CTester::TestEEPROM);
	RUN_TEST("RTC", CTester::TestRTC);
    RUN_TEST("LM75", CTester::TestLM75);
	RUN_TEST("AT88SC", CTester::TestAT88SC);
	RUN_TEST("SDCard", CTester::TestSDcard);
    RUN_TEST("UART", CTester::TestUART);

	dwCount = dwItem;

	if (!fOK)
	{
		m_iNGCount++;
	}
	SW_TRACE_NORMAL("Info: m_iNGCount = %d!\n", m_iNGCount);
	
	return S_OK;
}


HRESULT CAutotestJupiter::Autotest()
{
	HRESULT hr = S_OK;
	DWORD dwTableSize = sizeof(m_sResultTable) / sizeof(m_sResultTable[0]);

	SW_TRACE_NORMAL("Info: Autotest begins...\n");
	
    m_dwTestProgress = 1;

	SaveAutotestProgress();


	CTester::Ball_IntoTestMode();    //使球机进入生产老化模式；


	for (INT i = m_iRunCount; i < m_iTestCount; i++)
	{	
		SW_TRACE_NORMAL("Info: TestLoop #%d\n", i+1);	
		
		if (GetAutotestStopFlag())
		{
			SW_TRACE_DEBUG("Info: Autotest is forced to stop!\n");
			break;
		}
		
		//if( i < 10)
		if (0)
		{
			CTester::Product_SwitcherTest(3);
			CTester::Product_DcCtl_TEST(5);
		}
		
		AutotestRunOnce(m_sResultTable, dwTableSize);
		m_iRunCount++;

		if (GetAutotestStopFlag())
		{
			SW_TRACE_DEBUG("Info: Autotest is forced to stop!\n");
			break;
		}
		CHAR* pszReport = NULL;
		BOOL bOK = TRUE;
		
		GenerateReport((DWORD)i+1, m_sResultTable, dwTableSize, &pszReport, &bOK);		
		
		if (GetAutotestStopFlag())
		{
			SW_TRACE_DEBUG("Info: Autotest is forced to stop!\n");
			SAFE_MEM_FREE(pszReport);
			break;
		}
		
		SaveReport(pszReport);

		if (GetAutotestStopFlag())
		{
			SW_TRACE_DEBUG("Info: Autotest is forced to stop!\n");
			SAFE_MEM_FREE(pszReport);
			break;
		}

		SendReport(pszReport, bOK);

		if (GetAutotestStopFlag())
		{
			SW_TRACE_DEBUG("Info: Autotest is forced to stop!\n");
			SAFE_MEM_FREE(pszReport);
			break;
		}

		SAFE_MEM_FREE(pszReport);

	}

    CTester::Ball_StopTestMode();// 使球机退出老化测试模式。


	if (0 == m_iNGCount)
	{
		m_dwTestProgress = 0;//test OK
	}
	else
	{
		m_dwTestProgress = 2;//test failed
	}

	SetAutotestStopFlag(TRUE);
	SW_TRACE_NORMAL("Info: Autotest %s!\n", m_iRunCount >= m_iTestCount-1 ? "Done" : "Canceled");	
	
	return S_OK;
}
