/**
*/
#include "SWFC.h"
#include "swpa.h"
#include "Autotest.h"
#include "SWUpgradeApplication.h"
#include "Tester.h"
#include "SWTCPSocket.h"
#include "drv_device.h"
#include "SWNetOpt.h"
#include <sys/socket.h>
#include <arpa/inet.h>

#include "tinyxml.h"


#define PRINT SW_TRACE_DEBUG


// 命令包头
typedef struct tag_CameraCmdHeader
{
    DWORD dwID;
    DWORD dwInfoSize;
}
CAMERA_CMD_HEADER;


// 响应包头
typedef struct tag_CameraResponseHeader
{
    DWORD dwID;
    DWORD dwInfoSize;
	INT	  iReturn;
}
RESPONSE_CMD_HEADER;





CAutotest::CAutotest()
	:m_fInited(FALSE)
	,m_pAutotestThread(NULL)
	,m_iNGCount(0)
	,m_iTestCount(0)
	,m_iRunCount(0)
	,m_dwTestProgress(0)
	,m_fStop(TRUE)
{
	
}

CAutotest::~CAutotest()
{
	ReleaseObj();
}






HRESULT CAutotest::Initialize()
{
	if (m_fInited)
	{
		return S_OK;
	}

	//swpa_memset(m_szSavePath, 0, sizeof(m_szSavePath));
	swpa_memset(m_szSendIp, 0, sizeof(m_szSendIp));
	m_wSendPort = 0;
	m_iTestCount = 0;

	if (NULL == m_pAutotestThread)
	{
		m_pAutotestThread = new CSWThread();
		if (NULL == m_pAutotestThread)
		{
			PRINT("Err: no memory for m_pAutotestThread\n");
			return E_OUTOFMEMORY;
		}		
	}

	INT iCount = 0;	
	
	CSWUpgradeApp::Breath();

	INT iRetryCount = 32;
	while (SWPAR_OK != swpa_device_set_resolution(1)) //1080p
	{
		if (0 > --iRetryCount)
		{
			SW_TRACE_NORMAL("Err: failed to set H.264 resolution(1080p) to dm368\n");
			break;
		}
		CSWApplication::Sleep(1000);
	}
	
	// 等待368启动
	CSWUpgradeApp::Breath();
	while( 0 != swpa_device_dm368_ready() )
	{
		PRINT("Info: waiting for DM368 ready.\n");
		CSWUpgradeApp::Breath();
		swpa_thread_sleep_ms(1000);
		CSWUpgradeApp::Breath();
	}
	PRINT("Info: DM368 is ready.\n");
	
	
	// 设置USB网络
	if( 0 != swpa_utils_shell("ifconfig usb0 123.123.219.218", NULL) )
	{
		PRINT("Err: failed to set usb0 ip 123.123.219.218 !\n");
		return E_FAIL;
	}
	else 
	{
		// 设置子网掩码
		swpa_utils_shell("ifconfig usb0 netmask 255.255.255.0", NULL);
	}

	//inif VPIF
	iCount = 10;
	CSWUpgradeApp::Breath();
	while (FAILED(CTester::InitVPIF()) && 0 < iCount--)
	{
		PRINT("Err: failed to init vpif! try again...\n");
		CSWUpgradeApp::Breath();
		swpa_thread_sleep_ms(1000);
		CSWUpgradeApp::Breath();
	}

	if (0 >= iCount)
	{
		PRINT("Info: Init VPIF -- Failed!\n");
		return E_FAIL;
	}
	
	PRINT("Info: Init VPIF -- OK!\n");
	
	m_fInited = TRUE;
	
	return S_OK;
	
}




HRESULT CAutotest::ReleaseObj()
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

	CTester::DeinitVPIF();
	
	m_fInited = FALSE;

	PRINT("Info: Autotest Released!!\n");

	return S_OK;
}




HRESULT CAutotest::AutotestRunOnce(_TEST_RESULT* sResult, DWORD& dwCount)
{
	DWORD dwItem = 0;
	BOOL fOK = TRUE;


	swpa_strncpy(sResult[dwItem].szItemName, "DM6467T_SPI_FLASH", sizeof(sResult[dwItem].szItemName)-1);
	sResult[dwItem].fOK = TRUE;
	if (FAILED(CTester::TestDM6467Flash()))
	{
		PRINT("Err: test DM6467 Flash : FAILED!\n");
		sResult[dwItem].fOK = FALSE;
		sResult[dwItem].dwFailCount ++;
		fOK = FALSE;
	}
	if (dwItem+1>dwCount)
	{
		return E_OUTOFMEMORY;
	}
	dwItem++;
	if (GetAutotestStopFlag())
	{
		PRINT("Info: Autotest is forced to stop!\n");
		return S_OK;
	}
	

	swpa_strncpy(sResult[dwItem].szItemName, "DM6467T_IIC_EEPROM", sizeof(sResult[dwItem].szItemName)-1);
	sResult[dwItem].fOK = TRUE;
	if (FAILED(CTester::TestDM6467EEPROM()))
	{
		PRINT("Err: test DM6467 EEPROM : FAILED!\n");
		sResult[dwItem].fOK = FALSE;
		sResult[dwItem].dwFailCount ++;
		fOK = FALSE;
	}
	if (dwItem+1>dwCount)
	{
		return E_OUTOFMEMORY;
	}
	dwItem++;
	if (GetAutotestStopFlag())
	{
		PRINT("Info: Autotest is forced to stop!\n");
		return S_OK;
	}
	

	swpa_strncpy(sResult[dwItem].szItemName, "DM6467T_RTC", sizeof(sResult[dwItem].szItemName)-1);
	sResult[dwItem].fOK = TRUE;
	if (FAILED(CTester::TestDM6467RTC()))
	{
		PRINT("Err: test DM6467 RTC : FAILED!\n");
		sResult[dwItem].fOK = FALSE;
		sResult[dwItem].dwFailCount ++;
		fOK = FALSE;
	}
	if (dwItem+1>dwCount)
	{
		return E_OUTOFMEMORY;
	}
	dwItem++;
	if (GetAutotestStopFlag())
	{
		PRINT("Info: Autotest is forced to stop!\n");
		return S_OK;
	}
	

	swpa_strncpy(sResult[dwItem].szItemName, "LM75", sizeof(sResult[dwItem].szItemName)-1);
	sResult[dwItem].fOK = TRUE;
	if (FAILED(CTester::TestLM75()))
	{
		PRINT("Err: test DM6467 LM75 : FAILED!\n");
		sResult[dwItem].fOK = FALSE;
		sResult[dwItem].dwFailCount ++;
		fOK = FALSE;
	}
	if (dwItem+1>dwCount)
	{
		return E_OUTOFMEMORY;
	}
	dwItem++;
	if (GetAutotestStopFlag())
	{
		PRINT("Info: Autotest is forced to stop!\n");
		return S_OK;
	}
	

	swpa_strncpy(sResult[dwItem].szItemName, "DM6467_VPIF", sizeof(sResult[dwItem].szItemName)-1);
	sResult[dwItem].fOK = TRUE;
	if (FAILED(CTester::TestVPIF()))
	{
		PRINT("Err: test DM6467 : FAILED!\n");
		sResult[dwItem].fOK = FALSE;
		sResult[dwItem].dwFailCount ++;
		fOK = FALSE;
	}
	if (dwItem+1>dwCount)
	{
		return E_OUTOFMEMORY;
	}
	dwItem++;
	if (GetAutotestStopFlag())
	{
		PRINT("Info: Autotest is forced to stop!\n");
		return S_OK;
	}


	swpa_strncpy(sResult[dwItem].szItemName, "AT88SC", sizeof(sResult[dwItem].szItemName)-1);
	sResult[dwItem].fOK = TRUE;
	if (FAILED(CTester::TestAT88SC()))
	{
		PRINT("Err: test DM6467 AT88SC : FAILED!\n");
		sResult[dwItem].fOK = FALSE;
		sResult[dwItem].dwFailCount ++;
		fOK = FALSE;
	}
	if (dwItem+1>dwCount)
	{
		return E_OUTOFMEMORY;
	}
	dwItem++;
	if (GetAutotestStopFlag())
	{
		PRINT("Info: Autotest is forced to stop!\n");
		return S_OK;
	}
	

	
	/*
	swpa_strncpy(sResult[dwItem].szItemName, "USB", sizeof(sResult[dwItem].szItemName)-1);
	sResult[dwItem].fOK = TRUE;
	if (FAILED(CTester::TestUSB()))
	{
		PRINT("Err: test DM6467 USB : FAILED!\n");
		sResult[dwItem].fOK = FALSE;
		sResult[dwItem].dwFailCount ++;
		fOK = FALSE;
	}				
	if (dwItem+1>dwCount)
	{
		return E_OUTOFMEMORY;
	}
	dwItem++;
	if (GetAutotestStopFlag())
	{
		PRINT("Info: Autotest is forced to stop!\n");
		return S_OK;
	}
	*/
	
	swpa_strncpy(sResult[dwItem].szItemName, "UART", sizeof(sResult[dwItem].szItemName)-1);
	sResult[dwItem].fOK = TRUE;
	if (FAILED(CTester::TestUART()))
	{
		PRINT("Err: test DM6467 SerialComm : FAILED!\n");
		sResult[dwItem].fOK = FALSE;
		sResult[dwItem].dwFailCount ++;
		fOK = FALSE;
	}						
	if (dwItem+1>dwCount)
	{
		return E_OUTOFMEMORY;
	}
	dwItem++;
	if (GetAutotestStopFlag())
	{
		PRINT("Info: Autotest is forced to stop!\n");
		return S_OK;
	}


	swpa_strncpy(sResult[dwItem].szItemName, "DM368_FLASH", sizeof(sResult[dwItem].szItemName)-1);
	sResult[dwItem].fOK = TRUE;
	if (FAILED(CTester::TestDM368Flash()))
	{
		PRINT("Err: test DM368 Flash : FAILED!\n");
		sResult[dwItem].fOK = FALSE;
		sResult[dwItem].dwFailCount ++;
		fOK = FALSE;
	}
	if (dwItem+1>dwCount)
	{
		return E_OUTOFMEMORY;
	}
	dwItem++;
	if (GetAutotestStopFlag())
	{
		PRINT("Info: Autotest is forced to stop!\n");
		return S_OK;
	}


	swpa_strncpy(sResult[dwItem].szItemName, "DM6467T_FPGA_IO", sizeof(sResult[dwItem].szItemName)-1);
	sResult[dwItem].fOK = TRUE;
	if (FAILED(CTester::TestDM6467FPGAIO()))
	{
		PRINT("Err: test IO between DM6467 and FPGA : FAILED!\n");
		sResult[dwItem].fOK = FALSE;
		sResult[dwItem].dwFailCount ++;
		fOK = FALSE;
	}
	if (dwItem+1>dwCount)
	{
		return E_OUTOFMEMORY;
	}
	dwItem++;
	if (GetAutotestStopFlag())
	{
		PRINT("Info: Autotest is forced to stop!\n");
		return S_OK;
	}
	

	/*
	swpa_strncpy(sResult[dwItem].szItemName, "DM6467T_DM368_IO", sizeof(sResult[dwItem].szItemName)-1);
	sResult[dwItem].fOK = TRUE;
	if (FAILED(CTester::TestDM6467DM368IO()))
	{
		PRINT("Err: test IO between DM6467 and DM368 : FAILED!\n");
		sResult[dwItem].fOK = FALSE;
		sResult[dwItem].dwFailCount ++;
		fOK = FALSE;
	}
	if (dwItem+1>dwCount)
	{
		return E_OUTOFMEMORY;
	}
	dwItem++;
	if (GetAutotestStopFlag())
	{
		PRINT("Info: Autotest is forced to stop!\n");
		return S_OK;
	}*/

	swpa_strncpy(sResult[dwItem].szItemName, "DM368_FPGA_IO", sizeof(sResult[dwItem].szItemName)-1);
	sResult[dwItem].fOK = TRUE;
	if (FAILED(CTester::TestDM368FPGAIO()))
	{
		PRINT("Err: test IO between DM368 and FPGA : FAILED!\n");
		sResult[dwItem].fOK = FALSE;
		sResult[dwItem].dwFailCount ++;
		fOK = FALSE;
	}
	if (dwItem+1>dwCount)
	{		
		return E_OUTOFMEMORY;
	}
	dwItem++;
	if (GetAutotestStopFlag())
	{
		PRINT("Info: Autotest is forced to stop!\n");
		return S_OK;
	}

	dwCount = dwItem;

	PRINT("Info: fOK = %d!\n", fOK);
	if (!fOK)
	{
		m_iNGCount++;
	}
	PRINT("Info: m_iNGCount = %d!\n", m_iNGCount);
	
	return S_OK;
}


HRESULT CAutotest::GenerateReport(const DWORD dwReportNo, const _TEST_RESULT* psResult, const DWORD dwCount, CHAR** ppszReport, BOOL* pbOK)
{
	if (NULL == ppszReport || NULL == pbOK)
	{
		return E_INVALIDARG;
	}

	PRINT("Info: generating report...\n");

	
	const DWORD dwReportLen = 2*1024;
	*ppszReport = (CHAR*)swpa_mem_alloc(dwReportLen);
	if (NULL == *ppszReport)
	{
		PRINT("Err: no memory for ppszReport\n");
		return E_OUTOFMEMORY;
	}
	swpa_memset(*ppszReport, 0, dwReportLen);

	*pbOK = TRUE;
	CHAR szIp[32] = {0};
	CHAR szMask[32] = {0};
	CHAR szGateway[32] = {0};
	CHAR szMac[32] = {0};

	if (SWPAR_OK != swpa_tcpip_getinfo("eth0",
		szIp, sizeof(szIp),
		szMask, sizeof(szMask),
		szGateway, sizeof(szGateway),
		szMac, sizeof(szMac)))

	{
		PRINT("Err: failed to get net info!\n");
		return E_FAIL;
	}
	
	CHAR szSN[128]= {0};
	DWORD dwLen = sizeof(szSN) - 1;	

	if (SWPAR_OK != swpa_device_read_sn(szSN, &dwLen))
	{
		PRINT("Err: failed to get device sn!\n");
		return E_FAIL;
	}
	
	CHAR szStr[256] = {0};

	swpa_memset(szStr, 0, sizeof(szStr)); 
	swpa_sprintf(szStr, "---------------------------设备信息---------------------------\r\n");
	swpa_strcat(*ppszReport, szStr);
	swpa_memset(szStr, 0, sizeof(szStr)); 
	swpa_sprintf(szStr, "[  I  P  ]：%s\r\n", szIp);
	swpa_strcat(*ppszReport, szStr);
	swpa_memset(szStr, 0, sizeof(szStr)); 
	swpa_sprintf(szStr, "[子网掩码]：%s\r\n", szMask);
	swpa_strcat(*ppszReport, szStr);
	swpa_memset(szStr, 0, sizeof(szStr)); 
	swpa_sprintf(szStr, "[ 网  关 ]：%s\r\n", szGateway);
	swpa_strcat(*ppszReport, szStr);
	/*swpa_memset(szStr, 0, sizeof(szStr)); 
	swpa_sprintf(szStr, "[ 服务器 ]：%s\n", szMac);
	swpa_strcat(*ppszReport, szStr);*/
	swpa_memset(szStr, 0, sizeof(szStr)); 
	swpa_sprintf(szStr, "[物理地址]：%s\r\n", szMac);
	swpa_strcat(*ppszReport, szStr);
	swpa_memset(szStr, 0, sizeof(szStr)); 
	swpa_sprintf(szStr, "[ 序列号 ]：%s\r\n", szSN);
	swpa_strcat(*ppszReport, szStr);
	swpa_memset(szStr, 0, sizeof(szStr)); 
	swpa_sprintf(szStr, "[ 模  式 ]：生产测试模式\r\n");
	swpa_strcat(*ppszReport, szStr);
	/*swpa_memset(szStr, 0, sizeof(szStr)); 
	swpa_sprintf(szStr, "[设备状态]：执行中\n");
	swpa_strcat(*ppszReport, szStr);*/

	swpa_memset(szStr, 0, sizeof(szStr)); 
	swpa_snprintf(szStr, sizeof(szStr)-1, "\r\n---------------------------测试信息---------------------------\r\n");
	swpa_strcat(*ppszReport, szStr);
	swpa_memset(szStr, 0, sizeof(szStr)); 
	swpa_snprintf(szStr, sizeof(szStr)-1, "测试记录报告:\r\n");
	swpa_strcat(*ppszReport, szStr);
	swpa_memset(szStr, 0, sizeof(szStr)); 
	swpa_snprintf(szStr, sizeof(szStr)-1, "剩余测试次数: %d\r\n", m_iTestCount - dwReportNo);
	swpa_strcat(*ppszReport, szStr);
	swpa_memset(szStr, 0, sizeof(szStr)); 
	swpa_snprintf(szStr, sizeof(szStr)-1, "生产测试总数: %d\r\n", m_iTestCount);
	swpa_strcat(*ppszReport, szStr);
	swpa_memset(szStr, 0, sizeof(szStr)); 
	swpa_snprintf(szStr, sizeof(szStr)-1, "测试错误总数: %d\r\n", m_iNGCount);
	swpa_strcat(*ppszReport, szStr);
	swpa_memset(szStr, 0, sizeof(szStr)); 
	//todo:swpa_snprintf(szStr, sizeof(szStr)-1, "复位计数    	本次: %d	 总数: %d\n", );
	swpa_strcat(*ppszReport, szStr);
	swpa_memset(szStr, 0, sizeof(szStr)); 
	swpa_snprintf(szStr, sizeof(szStr)-1, "---------------------------详细结果---------------------------\r\n");
	swpa_strcat(*ppszReport, szStr);
	
	for (DWORD i=0; i<dwCount; i++)
	{
		CHAR szStr[64] = {0};
		swpa_memset(szStr, 0, sizeof(szStr)); 
		swpa_snprintf(szStr, sizeof(szStr)-1, "%-18s计数:\t", psResult[i].szItemName);
		swpa_strcat(*ppszReport, szStr);

		swpa_memset(szStr, 0, sizeof(szStr));
		swpa_snprintf(szStr, sizeof(szStr)-1, "\t正确:%8d\t错误:%8d\r\n", dwReportNo-psResult[i].dwFailCount, psResult[i].dwFailCount);
		swpa_strcat(*ppszReport, szStr);

		if (*pbOK && !psResult[i].fOK )
		{
			*pbOK = FALSE;
		}
	}
	swpa_strcat(*ppszReport, "\r\n");
	
	PRINT("Info: Generating report -- OK\n");
	PRINT("Info: report:\n%s\n", *ppszReport);

	return S_OK;
}



HRESULT CAutotest::SendReport(const CHAR* pszReport, const BOOL bOK)
{
	if (NULL == pszReport)
	{
		return E_INVALIDARG;
	}

	PRINT("Info: sending report...\n");
	
	CSWTCPSocket sockClient;

	if (FAILED(sockClient.Create()))
	{
		PRINT("Err: failed to create sockClient!\n");
		return E_FAIL;
	}

	DWORD dwTryCount = 10;

	sockClient.SetRecvTimeout(4000);
	sockClient.SetSendTimeout(4000);

	while (dwTryCount--)
	{
		PRINT("Info: connect to %s:%d\n", m_szSendIp, m_wSendPort);
		if (FAILED(sockClient.Connect(m_szSendIp, m_wSendPort)))
		{
			PRINT("Err: failed to connect to %s:%d (trying another %d times)\n", m_szSendIp, m_wSendPort, dwTryCount);
			swpa_thread_sleep_ms(1000);
		}
		else
		{
			RESPONSE_CMD_HEADER sHeader;
			sHeader.dwID = 0;
			sHeader.dwInfoSize = swpa_strlen(pszReport) + 1;
			sHeader.iReturn = bOK ? 0 : 1;
			
			if (FAILED(sockClient.Send((VOID*)&sHeader, sizeof(sHeader), 0)))
			{
				PRINT("Err: failed to send report header to %s:%d\n", m_szSendIp, m_wSendPort);
				sockClient.Close();
				
				return E_FAIL;
			}
			
			DWORD dwReportSize = swpa_strlen(pszReport) + 1;
			
			if (FAILED(sockClient.Send((VOID*)pszReport, dwReportSize, 0)))
			{
				PRINT("Err: failed to send report to %s:%d\n", m_szSendIp, m_wSendPort);
				sockClient.Close();
				
				return E_FAIL;
			}

			DWORD dwMakeSurePeerReceivedData = 0;
			sockClient.Read(&dwMakeSurePeerReceivedData, sizeof(dwMakeSurePeerReceivedData), 0);

			sockClient.Close();

			PRINT("Info: Sending report -- OK %s:%d\n");
			return S_OK;			
		}
	}

	sockClient.Close();
	
	PRINT("Err: failed to connect to %s:%d\n", m_szSendIp, m_wSendPort);
	
	return E_FAIL;
}


HRESULT CAutotest::SaveReport(const CHAR* pszReport)
{
	if (NULL == pszReport)
	{
		return E_INVALIDARG;
	}

	SaveAutotestProgress();

	CHAR* szSavePath = "EEPROM/0/RESERVED";

	PRINT("Info: saving report to EEPROM: %s...\n", szSavePath);

	CSWFile myReportFile;
	if (FAILED(myReportFile.Open(szSavePath, "w+")))
	{
		PRINT("Info: failed to open report file with write mode !!\n", szSavePath);
		if (FAILED( myReportFile.Open(szSavePath, "w")))
		{
			PRINT("Err: failed to open report file %s\n", szSavePath);
			return E_FAIL;
		}
	}

	DWORD dwLen = swpa_strlen(pszReport)+1;
	if (FAILED(myReportFile.Seek(4+256, SWPA_SEEK_SET,NULL))
		|| FAILED(myReportFile.Write((VOID*)&dwLen, sizeof(dwLen),NULL))
		|| FAILED(myReportFile.Write((VOID*)pszReport, dwLen,NULL)))
	{
		PRINT("Err: failed to write report file\n");
		return E_FAIL;
	}
	myReportFile.Close();

	PRINT("Info: saving report -- OK\n");

	return S_OK;
}



HRESULT CAutotest::SendReportSavedInEEPROM(const CHAR* szSendIp, const WORD wSendPort)
{
	CHAR* szSavePath = "EEPROM/0/RESERVED";
	CSWFile myReportFile;
	if (FAILED(myReportFile.Open(szSavePath, "r")))
	{
		PRINT("Err: failed to open report file in EEPROM with READ mode\n");
		return E_FAIL;
	}

	const DWORD dwReportLen = 2*1024;
	CHAR* pszReport = (CHAR*)swpa_mem_alloc(dwReportLen);
	if (NULL == pszReport)
	{
		PRINT("Err: no memory for ppszReport\n");
		return E_OUTOFMEMORY;
	}
	swpa_memset(pszReport, 0, dwReportLen);

	DWORD dwLen = 0;
	if (FAILED(myReportFile.Seek(4+256, SWPA_SEEK_SET, NULL))
		|| FAILED(myReportFile.Read((VOID*)&dwLen, sizeof(dwLen),NULL))
		|| FAILED(myReportFile.Read((VOID*)pszReport, dwLen  > dwReportLen ? dwReportLen - 1 : dwLen,NULL)))
	{
		PRINT("Err: failed to read report file\n");
		return E_FAIL;
	}
	
	myReportFile.Close();

	swpa_strncpy(m_szSendIp, szSendIp, sizeof(m_szSendIp)-1);
	m_wSendPort = wSendPort;

	HRESULT hr = SendReport(pszReport, (NULL != swpa_strstr(pszReport, " NG ")) ? FALSE : TRUE);

	swpa_mem_free(pszReport);
	pszReport = NULL;

	return hr;
	
}


HRESULT CAutotest::LoadFactoryModeFPGA()
{
	CSWFile cFPGAFile;

	PRINT("Info: Loading factory-mode fpga...");

	if (FAILED(cFPGAFile.Open("/usr/local/signalway/fpga.xsvf", "r")))
	{
		PRINT("Err: failed to open factory-mode FPGA file\n");
		return E_FAIL;
	}
	
	DWORD dwFpgaFileSize = 0;
	cFPGAFile.GetSize(&dwFpgaFileSize);
	PBYTE pbFpgaFileBuf = (PBYTE)swpa_mem_alloc(dwFpgaFileSize);
	if (NULL == pbFpgaFileBuf)
	{
		PRINT("Err: no memory for pbFpgaFileBuf\n");
		return E_OUTOFMEMORY;
	}
	swpa_memset(pbFpgaFileBuf, 0, dwFpgaFileSize);

	DWORD dwReadSize = 0;
	if (FAILED(cFPGAFile.Read(pbFpgaFileBuf, dwFpgaFileSize, &dwReadSize))
		|| dwFpgaFileSize != dwReadSize)
	{
		PRINT("Err: failed to open factory-mode FPGA file\n");
		SAFE_MEM_FREE(pbFpgaFileBuf);
		return E_FAIL;
	}
	
	if (0 != drv_fpga_load(DEVID_SPI_FPGA, pbFpgaFileBuf, (INT)dwFpgaFileSize))
	{
		PRINT("Err: failed to load factory-mode fpga image\n");
		SAFE_MEM_FREE(pbFpgaFileBuf);
		return E_FAIL;
	}

	SAFE_MEM_FREE(pbFpgaFileBuf);
	
	PRINT("Info: Loading factory-mode fpga -- OK\n");
	
	return cFPGAFile.Close();
}





HRESULT CAutotest::Autotest()
{
	HRESULT hr = S_OK;

	PRINT("Info: Autotest begins...\n");
	DWORD dwTableSize = 32;
	_TEST_RESULT sResultTable[dwTableSize];
	swpa_memset(sResultTable, 0, sizeof(sResultTable));
	
	SaveAutotestProgress();

	
	for (INT i = m_iRunCount; i < m_iTestCount; i++)
	{	
		m_dwTestProgress = 1;
		
		PRINT("Info: TestLoop #%d\n", i+1);	
		
		if (GetAutotestStopFlag())
		{
			PRINT("Info: Autotest is forced to stop!\n");
			break;
		}
		if( i < 10)
		{
			CTester::ProductSwitcherTest(5);
			CTester::ProductDcCtlTEST(5);
		}
		
		AutotestRunOnce(sResultTable, dwTableSize);
		m_iRunCount++;

		if (GetAutotestStopFlag())
		{
			PRINT("Info: Autotest is forced to stop!\n");
			break;
		}
		CHAR* pszReport = NULL;
		BOOL bOK = TRUE;
		
		GenerateReport((DWORD)i+1, sResultTable, dwTableSize, &pszReport, &bOK);		
		
		if (GetAutotestStopFlag())
		{
			PRINT("Info: Autotest is forced to stop!\n");
			SAFE_MEM_FREE(pszReport);
			break;
		}
		
		SaveReport(pszReport);

		if (GetAutotestStopFlag())
		{
			PRINT("Info: Autotest is forced to stop!\n");
			SAFE_MEM_FREE(pszReport);
			break;
		}

		SendReport(pszReport, bOK);

		if (GetAutotestStopFlag())
		{
			PRINT("Info: Autotest is forced to stop!\n");
			SAFE_MEM_FREE(pszReport);
			break;
		}

		SAFE_MEM_FREE(pszReport);

	}

	if (0 == m_iNGCount)
	{
		m_dwTestProgress = 0;//test OK
	}
	else
	{
		m_dwTestProgress = 2;//test failed
	}

	SetAutotestStopFlag(TRUE);
	PRINT("Info: Autotest %s!\n", m_iRunCount >= m_iTestCount-1 ? "Done" : "Canceled");	
	
	return S_OK;
}




PVOID CAutotest::AutotestProxy(PVOID pvArg)
{
	if (NULL == pvArg)
	{
		PRINT("Err: NULL == pvArg\n");
		return (PVOID)E_INVALIDARG;
	}
	
	CAutotest* pThis = (CAutotest*)pvArg;
	return (PVOID)pThis->Autotest();	
}


HRESULT CAutotest::ProcessAutotestCmd(const INT iTestCount, const CHAR* szSendIp, const WORD wSendPort)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet!\n");
		return E_NOTIMPL;
	}	
	
	//设定的测试次数是否非法
	if (0 >= iTestCount)
	{
		PRINT("Err: TestCount(%d) is invalid.\n", iTestCount);
		return E_INVALIDARG;
	}
	m_iTestCount = iTestCount;
	
	
	if (NULL == szSendIp)
	{
		PRINT("Err: NULL == szSendIp.\n");
		return E_INVALIDARG;
	}
	swpa_strncpy(m_szSendIp, szSendIp, sizeof(m_szSendIp));

	m_wSendPort = wSendPort;

	m_iNGCount = 0;
	m_iRunCount = 0;

	SetAutotestStopFlag(TRUE);
	m_pAutotestThread->Stop();

	//restart the thread
	SetAutotestStopFlag(FALSE);
	if (FAILED(m_pAutotestThread->Start(AutotestProxy, this)))
	{
		PRINT("Err: failed to start Autotest Thread.\n");
		return E_FAIL;
	}

	return S_OK;
}



HRESULT CAutotest::ProcessXmlCommand(const PBYTE pbBuf, const DWORD dwSize, PVOID* ppvOutBuf, DWORD* pdwOutSize)
{
	HRESULT hr = S_OK;

	TiXmlDocument xmlDoc;

	if (NULL == pbBuf || 0 == dwSize)
	{
		PRINT("Err: NULL == pbBuf || 0 == dwSize\n");
		return E_INVALIDARG;
	}
	
	if (0 == xmlDoc.Parse((const CHAR*)pbBuf))
	{
		PRINT("Err: failed to parse cmd xml\n");
		return E_FAIL;
	}
	
	TiXmlElement * pEleRoot = NULL;	
	
	pEleRoot = xmlDoc.RootElement();	
	for (TiXmlElement *Ele = pEleRoot->FirstChildElement(); Ele; Ele = Ele->NextSiblingElement())
	{
		if (NULL != Ele->GetText())
		{
			if (0 == swpa_strcmp(Ele->GetText(), "StartAutotest"))
			{
				INT iTestCount = 0;
				if (NULL == Ele->Attribute("TestCount"))
				{
					PRINT("Err: no TestCount info\n");
					return E_FAIL;
				}
				iTestCount = (INT)swpa_atoi(Ele->Attribute("TestCount"));

				const CHAR* szSendToIp = (CHAR*)Ele->Attribute("SendToIp");
				if (NULL == szSendToIp)
				{
					PRINT("Err: no szSendToIp info, use default save path instead.\n");
					szSendToIp = "todo:default ip is??";
				}

				WORD wPort = 0;
				if (NULL == Ele->Attribute("SendToPort"))
				{
					PRINT("Err: no SendToPort info, use default save path instead.\n");
					wPort = 8989;
				}
				wPort = (WORD)swpa_atoi(Ele->Attribute("SendToPort"));

				if (FAILED(ProcessAutotestCmd(iTestCount, szSendToIp, wPort)))
				{
					PRINT("Err: failed to process autotest cmd.\n");
					return E_FAIL;
					
				}
			}			
			else if (0 == swpa_strcmp(Ele->GetText(), "GetLatestReport"))
			{
				const CHAR* szSendToIp = (CHAR*)Ele->Attribute("SendToIp");
				if (NULL == szSendToIp)
				{
					PRINT("Err: no szSendToIp info, use default save path instead.\n");
					szSendToIp = "todo:default ip is??";
				}

				WORD wPort = 0;
				if (NULL == Ele->Attribute("SendToPort"))
				{
					PRINT("Err: no SendToPort info, use default save path instead.\n");
					wPort = 8989;//todo: this is the default port???
				}
				wPort = (WORD)swpa_atoi(Ele->Attribute("SendToPort"));
				
				hr = SendReportSavedInEEPROM(szSendToIp, wPort);
				PRINT("Info: SendReportSavedInEEPROM ret %#x!\n", hr);	
			}
			else if (0 == swpa_strcmp(Ele->GetText(), "GetAutotestProgress"))
			{
				hr = GetAutotestStatus(ppvOutBuf, pdwOutSize);
				//PRINT("Info: GetAutotestProgress ret %#x!\n", hr);	
			}
		}
	}
	
	return hr;
}



HRESULT CAutotest::ReceiveCmd(const DWORD dwCmdID, const PBYTE pbCmdData, const DWORD dwDataSize, PVOID* ppvOutBuf, DWORD* pdwOutSize)
{
	HRESULT hr = S_OK;
	
	if (!m_fInited)
	{
		PRINT("Err: not inited yet!\n");
		return E_NOTIMPL;
	}	

	PBYTE pbBuf = (PBYTE)pbCmdData;
	DWORD dwSize = dwDataSize;
	
	if (CAMERA_XML_EXT_CMD == dwCmdID) // XML cmd
	{		
		hr = ProcessXmlCommand(pbBuf, dwSize, ppvOutBuf, pdwOutSize);
		
		//PRINT("Info: ProcessXmlCommand ret %#x!\n", hr);		
	}
	else
	{
		PRINT("Info: Got unsupported cmd : %#X!\n", dwCmdID);
		return E_INVALIDARG;
	}	

	return hr;
}



HRESULT CAutotest::SaveAutotestProgress()
{	
	if (!m_fInited)
	{
		PRINT("Err: not inited yet!\n");
		return E_NOTIMPL;
	}	
	
	CHAR* szSavePath = "EEPROM/0/RESERVED";

	CHAR szProgressInfo[256] = {0};

	swpa_snprintf(szProgressInfo, sizeof(szProgressInfo)-1, 
		"TestCount:%d, FailCount:%d, RunCount:%d, SendToPort:%d, SendToIp:%s",
		m_iTestCount, m_iNGCount, m_iRunCount, m_wSendPort, m_szSendIp);
	
	DWORD dwCrc = CSWUtils::CalcCrc32(0, (BYTE *)szProgressInfo, sizeof(szProgressInfo));
	CSWFile cFile;
	if (FAILED(cFile.Open(szSavePath, "w+")))
	{
		PRINT("Err: failed to open %s to save autotest progress!\n", szSavePath);
	}

	if (FAILED(cFile.Write(&dwCrc, sizeof(dwCrc), NULL))
		|| FAILED(cFile.Write(szProgressInfo, sizeof(szProgressInfo), NULL))
		)
	{
		PRINT("Err: failed to save autotest progress!\n", szSavePath);
		return E_FAIL;
	}

	PRINT("Info: save autotest progress -- OK!\n");

	return S_OK;
	
}




HRESULT CAutotest::RestoreAutotest()
{	
	CHAR* szSavePath = "EEPROM/0/RESERVED";
	BYTE bBuf[4+256] = {0};
	DWORD dwLen = 0;

	if (!m_fInited && Initialize())
	{
		PRINT("Err: failed to initialize Autotest!\n");
		return E_FAIL;
	}

	CSWFile cFile;
	if (FAILED(cFile.Open(szSavePath, "r+")))
	{
		PRINT("Err: failed to open %s to get autotest progress!\n", szSavePath);
	}

	if (FAILED(cFile.Read(bBuf, sizeof(bBuf), &dwLen))
		|| 4+256 != dwLen)
	{
		PRINT("Err: failed to read autotest progress!\n");
		return E_FAIL;
	}

	PRINT("Info: Progress: %s\n", (CHAR*)bBuf+4);

	DWORD dwCrcStored = 0;
	DWORD dwCrcCalced = 0;

	swpa_memcpy(&dwCrcStored, bBuf, sizeof(dwCrcStored));	
	
	if (5 != swpa_sscanf((CHAR*)bBuf+4,	"TestCount:%d, FailCount:%d, RunCount:%d, SendToPort:%d, SendToIp:%s",
			&m_iTestCount, &m_iNGCount, &m_iRunCount, &m_wSendPort, m_szSendIp))
	{
		PRINT("Err: failed to get autotest progress info!\n");
		return E_FAIL;
	}

	PRINT("Info: m_szSendIp: %s\n", m_szSendIp);
	PRINT("Info: m_wSendPort: %d\n", m_wSendPort);
	PRINT("Info: m_iTestCount: %d\n", m_iTestCount);
	PRINT("Info: m_iNGCount: %d\n", m_iNGCount);
	PRINT("Info: m_iRunCount: %d\n", m_iRunCount);
	

	dwCrcCalced = CSWUtils::CalcCrc32(0, bBuf+4, sizeof(bBuf)-4);

	PRINT("Info: dwCrcStored = %u, dwCrcCalced=%u\n", dwCrcStored, dwCrcCalced);
	if (dwCrcCalced != dwCrcStored)
	{
		PRINT("Err: autotest progress is Invalid, ignore it...!\n");
		return E_FAIL;
	}
		
	if (m_iTestCount >= m_iRunCount+1)
	{
		PRINT("Info: needs to start Autotest automatically...!\n");

		SetAutotestStopFlag(TRUE);
		m_pAutotestThread->Stop();
		//restart
		SetAutotestStopFlag(FALSE);
		if (FAILED(m_pAutotestThread->Start(AutotestProxy, this)))
		{
			PRINT("Err: failed to start Autotest Thread.\n");
			return E_FAIL;
		}
	}
	else
	{
		PRINT("Info: NO needs to start Autotest !\n");
	}

	return S_OK;
}


HRESULT CAutotest::GetAutotestStatus(PVOID* ppvOutBuf, DWORD* pdwOutSize)
{
#define HV_XML_CMD_VERSION_NO "3.0"
	
	
#define HVXML_VER 					"Ver"
#define HVXML_HVCMD					"HvCmd"
#define HVXML_HVCMDRESPOND 			"HvCmdRespond"
#define HVXML_RETCODE 				"RetCode"
#define HVXML_RETMSG 				"RetMsg"
#define HVXML_CMDNAME				"CmdName"
#define HVXML_TYPE 					"Type"
#define HVXML_VALUE 				"Value"
#define HVXML_CLASS 				"Class"
#define HVXML_GETTER 				"GETTER"
#define HVXML_SETTER 				"SETTER"
#define HVXML_RETURNLEN				"RetLen"


	//PRINT("Info: Got GetAutotestProgress Cmd\n");
	
	if (NULL == ppvOutBuf || NULL == pdwOutSize)
	{
		PRINT("Err: NULL == ppvOutBuf || NULL == pdwOutSize\n");
		return E_INVALIDARG;
	}
	
	if (!m_fInited)
	{
		PRINT("Err: not inited yet!\n");
		return E_NOTIMPL;
	}

	TiXmlDocument xmlOutDoc;	
	TiXmlDeclaration *pDeclaration = NULL;
	TiXmlElement *pRootEle = NULL;

	pDeclaration = new TiXmlDeclaration("1.0","GB2312","yes");
	if (NULL == pDeclaration)
	{
		PRINT("Err: no memory for pDeclaration\n");
		return E_OUTOFMEMORY;
	}
	xmlOutDoc.LinkEndChild(pDeclaration);

	pRootEle = new TiXmlElement(HVXML_HVCMDRESPOND);
	if (NULL == pRootEle)
	{	
		PRINT("Err: no memory for pRootEle\n");
		return E_OUTOFMEMORY;
	}
	xmlOutDoc.LinkEndChild(pRootEle);
	
	pRootEle->SetAttribute(HVXML_VER, HV_XML_CMD_VERSION_NO);

	TiXmlElement *pReplyEle =  new TiXmlElement(HVXML_CMDNAME);
	if (NULL == pReplyEle)
	{	
		PRINT("Err: no memory for pReplyEle\n");
		return E_OUTOFMEMORY;
	}
	pRootEle->LinkEndChild(pReplyEle);

	
	TiXmlText * pReplyText = new TiXmlText("GetAutotestProgress");
	if (NULL == pReplyText)
	{	
		PRINT("Err: no memory for pReplyText\n");
		return E_OUTOFMEMORY;
	}
	pReplyEle->LinkEndChild(pReplyText);	


	if (0 == m_dwTestProgress)
	{	
		pReplyEle->SetAttribute(HVXML_RETCODE, 0);
		pReplyEle->SetAttribute(HVXML_RETMSG, "TestOK");
	}
	else if (1 == m_dwTestProgress)
	{
		pReplyEle->SetAttribute(HVXML_RETCODE, 0);
		pReplyEle->SetAttribute(HVXML_RETMSG, "Testing");
	}
	else if (2 == m_dwTestProgress)
	{
		pReplyEle->SetAttribute(HVXML_RETCODE, 0);
		pReplyEle->SetAttribute(HVXML_RETMSG, "TestFailed");
	}
	else
	{
		pReplyEle->SetAttribute(HVXML_RETCODE, -1);
		pReplyEle->SetAttribute(HVXML_RETMSG, "GetAutotestProgress Failed");
	}
	
	TiXmlPrinter xmlPrinter;

	xmlOutDoc.Accept(&xmlPrinter);

	*pdwOutSize = xmlPrinter.Size()+1;
	*ppvOutBuf = swpa_mem_alloc(*pdwOutSize);
	if (NULL == *ppvOutBuf)
	{
		PRINT("Err: no memory for *ppvOutBuf!\n");
		return E_OUTOFMEMORY;
	}
	swpa_memset(*ppvOutBuf, 0, *pdwOutSize);

	swpa_strncpy((CHAR*)*ppvOutBuf, xmlPrinter.CStr(), xmlPrinter.Size());

	//PRINT("Info: GetAutotestProgress Cmd processed -- OK\n");
	
	return S_OK;	
}



HRESULT CAutotest::SetAutotestStopFlag(const BOOL fStop)
{
	if ((fStop && m_fStop) || (!fStop && !m_fStop))
	{
		return S_OK;
	}

	
	CTester::SetForceStopFlag(fStop);

	m_fStop = fStop;
};

BOOL CAutotest::GetAutotestStopFlag()
{
	return m_fStop;
};




