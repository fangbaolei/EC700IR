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


static const CHAR* szSavePath = "EEPROM/0/RESERVED";

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

/*
	INT iCount = 0;
	//inif VPIF
	iCount = 10;
	CSWUpgradeApp::Breath();
	while (FAILED(CTester::InitVPIF()) && 0 < iCount--)
	{
		SW_TRACE_DEBUG("Err: failed to init vpif! try again...\n");
		CSWUpgradeApp::Breath();
		swpa_thread_sleep_ms(1000);
		CSWUpgradeApp::Breath();
	}

	if (0 >= iCount)
	{
		SW_TRACE_DEBUG("Info: Init VPIF -- Failed!\n");
		return E_FAIL;
	}
	
	SW_TRACE_NORMAL("Info: Init VPIF -- OK!\n");	
*/	
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

//	CTester::DeinitVPIF();//todo:no vpif on venus??
	
	m_fInited = FALSE;

	SW_TRACE_NORMAL("Info: Autotest Released!!\n");

	return S_OK;
}


HRESULT CAutotest::AutotestRunOnce(_TEST_RESULT* sResult, DWORD& dwCount)
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
	//RUN_TEST("VPIF", CTester::TestVPIF);
	RUN_TEST("AT88SC", CTester::TestAT88SC);
	RUN_TEST("SDCard", CTester::TestSDcard);
	RUN_TEST("UART", CTester::TestUART);
	RUN_TEST("DM8127T_FPGA_IO", CTester::TestDM8127FPGAIO);

	dwCount = dwItem;

	if (!fOK)
	{
		m_iNGCount++;
	}
	SW_TRACE_NORMAL("Info: m_iNGCount = %d!\n", m_iNGCount);
	
	return S_OK;
}


HRESULT CAutotest::GenerateReport(const DWORD dwReportNo, const _TEST_RESULT* psResult, const DWORD dwCount, CHAR** ppszReport, BOOL* pbOK)
{
	if (NULL == ppszReport || NULL == pbOK)
	{
		return E_INVALIDARG;
	}

	SW_TRACE_NORMAL("Info: generating autotest report...\n");

	
	const DWORD dwReportLen = 2*1024;
	*ppszReport = (CHAR*)swpa_mem_alloc(dwReportLen);
	if (NULL == *ppszReport)
	{
		SW_TRACE_DEBUG("Err: no memory for ppszReport\n");
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
		SW_TRACE_DEBUG("Err: failed to get net info!\n");
		return E_FAIL;
	}
	
	CHAR szSN[128]= {0};
	DWORD dwLen = sizeof(szSN) - 1;	

	if (SWPAR_OK != swpa_device_read_sn(szSN, &dwLen))
	{
		SW_TRACE_DEBUG("Err: failed to get device sn!\n");
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
	
	SW_TRACE_NORMAL("Info: Generating report -- OK\n");
	SW_TRACE_DEBUG("Info: report:\n%s\n", *ppszReport);

	return S_OK;
}



HRESULT CAutotest::SendReport(const CHAR* pszReport, const BOOL bOK)
{
	if (NULL == pszReport)
	{
		return E_INVALIDARG;
	}

	SW_TRACE_NORMAL("Info: sending Autotest report...\n");
	
	CSWTCPSocket sockClient;

	if (FAILED(sockClient.Create()))
	{
		SW_TRACE_DEBUG("Err: failed to create sockClient!\n");
		return E_FAIL;
	}

	DWORD dwTryCount = 3;

	sockClient.SetRecvTimeout(4000);
	sockClient.SetSendTimeout(4000);

	while (dwTryCount--)
	{
		//SW_TRACE_DEBUG("Info: connect to %s:%d\n", m_szSendIp, m_wSendPort);
		if (FAILED(sockClient.Connect(m_szSendIp, m_wSendPort)))
		{
			SW_TRACE_DEBUG("Err: failed to connect to %s:%d (trying another %d times)\n", m_szSendIp, m_wSendPort, dwTryCount);

			if (GetAutotestStopFlag())
			{
				SW_TRACE_DEBUG("Info: Autotest is forced to stop!\n");
				return S_OK;
			}
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
				SW_TRACE_DEBUG("Err: failed to send report header to %s:%d\n", m_szSendIp, m_wSendPort);
				sockClient.Close();
				
				return E_FAIL;
			}
			
			DWORD dwReportSize = swpa_strlen(pszReport) + 1;
			
			if (FAILED(sockClient.Send((VOID*)pszReport, dwReportSize, 0)))
			{
				SW_TRACE_DEBUG("Err: failed to send report to %s:%d\n", m_szSendIp, m_wSendPort);
				sockClient.Close();
				
				return E_FAIL;
			}

			DWORD dwMakeSurePeerReceivedData = 0;
			sockClient.Read(&dwMakeSurePeerReceivedData, sizeof(dwMakeSurePeerReceivedData), 0);

			sockClient.Close();

			SW_TRACE_DEBUG("Info: Sending report -- OK %s:%d\n", m_szSendIp, m_wSendPort);
			return S_OK;			
		}
	}

	sockClient.Close();
	
	SW_TRACE_NORMAL("Err: failed to connect to %s:%d\n", m_szSendIp, m_wSendPort);
	
	return E_FAIL;
}


HRESULT CAutotest::SaveReport(const CHAR* pszReport)
{
	if (NULL == pszReport)
	{
		return E_INVALIDARG;
	}

	SaveAutotestProgress();

	SW_TRACE_NORMAL("Info: saving Autotest report to EEPROM: %s...\n", szSavePath);

	CSWFile myReportFile;
	if (FAILED(myReportFile.Open(szSavePath, "w+")))
	{
		SW_TRACE_DEBUG("Info: failed to open report file with rw mode !!\n", szSavePath);
		if (FAILED( myReportFile.Open(szSavePath, "w")))
		{
			SW_TRACE_DEBUG("Err: failed to open report file %s\n", szSavePath);
			return E_FAIL;
		}
	}

	DWORD dwLen = swpa_strlen(pszReport)+1;
	if (FAILED(myReportFile.Seek(1024, SWPA_SEEK_SET,NULL))//the first 1K saved progress info
		|| FAILED(myReportFile.Write((VOID*)&dwLen, sizeof(dwLen),NULL))
		|| FAILED(myReportFile.Write((VOID*)pszReport, dwLen,NULL)))
	{
		SW_TRACE_DEBUG("Err: failed to write report file\n");
		return E_FAIL;
	}
	myReportFile.Close();

	SW_TRACE_NORMAL("Info: saving Autotest report -- OK\n");

	return S_OK;
}



HRESULT CAutotest::SendReportSavedInEEPROM(const CHAR* szSendIp, const WORD wSendPort)
{
	CSWFile myReportFile;
	if (FAILED(myReportFile.Open(szSavePath, "r")))
	{
		SW_TRACE_DEBUG("Err: failed to open report file in EEPROM with READ mode\n");
		return E_FAIL;
	}

	const DWORD dwReportLen = 2*1024;
	CHAR* pszReport = (CHAR*)swpa_mem_alloc(dwReportLen);
	if (NULL == pszReport)
	{
		SW_TRACE_DEBUG("Err: no memory for ppszReport\n");
		return E_OUTOFMEMORY;
	}
	swpa_memset(pszReport, 0, dwReportLen);

	DWORD dwLen = 0;
	if (FAILED(myReportFile.Seek(1024, SWPA_SEEK_SET, NULL))//the first 1K saved progress info
		|| FAILED(myReportFile.Read((VOID*)&dwLen, sizeof(dwLen),NULL))
		|| FAILED(myReportFile.Read((VOID*)pszReport, dwLen  > dwReportLen ? dwReportLen - 1 : dwLen,NULL)))
	{
		SW_TRACE_DEBUG("Err: failed to read report file\n");
		return E_FAIL;
	}
	
	myReportFile.Close();

	swpa_strncpy(m_szSendIp, szSendIp, sizeof(m_szSendIp)-1);
	m_wSendPort = wSendPort;

	//debug
	SW_TRACE_DEBUG("Test Report in EEPROM:\n%s\n", pszReport);

	HRESULT hr = SendReport(pszReport, (NULL != swpa_strstr(pszReport, " NG ")) ? FALSE : TRUE);

	swpa_mem_free(pszReport);
	pszReport = NULL;

	return hr;
	
}


HRESULT CAutotest::LoadFactoryModeFPGA()
{
	CSWFile cFPGAFile;

	SW_TRACE_NORMAL("Info: Loading factory-mode fpga...");

	if (FAILED(cFPGAFile.Open("/opt/ipnc/fpga.xsvf", "r")))
	{
		SW_TRACE_DEBUG("Err: failed to open factory-mode FPGA file\n");
		return E_FAIL;
	}
	
	DWORD dwFpgaFileSize = 0;
	cFPGAFile.GetSize(&dwFpgaFileSize);
	PBYTE pbFpgaFileBuf = (PBYTE)swpa_mem_alloc(dwFpgaFileSize);
	if (NULL == pbFpgaFileBuf)
	{
		SW_TRACE_DEBUG("Err: no memory for pbFpgaFileBuf\n");
		return E_OUTOFMEMORY;
	}
	swpa_memset(pbFpgaFileBuf, 0, dwFpgaFileSize);

	DWORD dwReadSize = 0;
	if (FAILED(cFPGAFile.Read(pbFpgaFileBuf, dwFpgaFileSize, &dwReadSize))
		|| dwFpgaFileSize != dwReadSize)
	{
		SW_TRACE_DEBUG("Err: failed to open factory-mode FPGA file\n");
		SAFE_MEM_FREE(pbFpgaFileBuf);
		return E_FAIL;
	}
	
	if (0 != drv_fpga_load(DEVID_SPI_FPGA, pbFpgaFileBuf, (INT)dwFpgaFileSize))
	{
		SW_TRACE_DEBUG("Err: failed to load factory-mode fpga image\n");
		SAFE_MEM_FREE(pbFpgaFileBuf);
		return E_FAIL;
	}

	SAFE_MEM_FREE(pbFpgaFileBuf);
	
	SW_TRACE_NORMAL("Info: Loading factory-mode fpga -- OK\n");
	
	return cFPGAFile.Close();
}





HRESULT CAutotest::Autotest()
{
	HRESULT hr = S_OK;
	DWORD dwTableSize = sizeof(m_sResultTable) / sizeof(m_sResultTable[0]);

	SW_TRACE_NORMAL("Info: Autotest begins...\n");
	
	SaveAutotestProgress();

	
	for (INT i = m_iRunCount; i < m_iTestCount; i++)
	{	
		m_dwTestProgress = 1;
		
		SW_TRACE_NORMAL("Info: TestLoop #%d\n", i+1);	
		
		if (GetAutotestStopFlag())
		{
			SW_TRACE_DEBUG("Info: Autotest is forced to stop!\n");
			break;
		}
		
		if( i < 10)
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




PVOID CAutotest::AutotestProxy(PVOID pvArg)
{
	if (NULL == pvArg)
	{
		SW_TRACE_DEBUG("Err: NULL == pvArg\n");
		return (PVOID)E_INVALIDARG;
	}
	
	CAutotest* pThis = (CAutotest*)pvArg;
	return (PVOID)pThis->Autotest();	
}


HRESULT CAutotest::ProcessAutotestCmd(const INT iTestCount, const CHAR* szSendIp, const WORD wSendPort)
{
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not inited yet!\n");
		return E_NOTIMPL;
	}	
	
	//设定的测试次数是否非法
	if (0 > iTestCount)
	{
		SW_TRACE_DEBUG("Err: TestCount(%d) is invalid.\n", iTestCount);
		return E_INVALIDARG;
	}
	m_iTestCount = iTestCount;

	
	if (NULL == szSendIp)
	{
		SW_TRACE_DEBUG("Err: NULL == szSendIp.\n");
		return E_INVALIDARG;
	}
	swpa_strncpy(m_szSendIp, szSendIp, sizeof(m_szSendIp));

	m_wSendPort = wSendPort;

	SetAutotestStopFlag(TRUE);
	m_pAutotestThread->Stop();

	//restart the thread
	m_iNGCount = 0;
	m_iRunCount = 0;
	swpa_memset(m_sResultTable, 0, sizeof(m_sResultTable));
	
	if (0 < iTestCount)
	{
		SetAutotestStopFlag(FALSE);
		if (FAILED(m_pAutotestThread->Start(AutotestProxy, this)))
		{
			SW_TRACE_DEBUG("Err: failed to start Autotest Thread.\n");
			return E_FAIL;
		}
	}
	else
	{
		SaveAutotestProgress();
	}

	return S_OK;
}



HRESULT CAutotest::ProcessXmlCommand(const PBYTE pbBuf, const DWORD dwSize, PVOID* ppvOutBuf, DWORD* pdwOutSize)
{
	HRESULT hr = S_OK;

	TiXmlDocument xmlDoc;

	if (NULL == pbBuf || 0 == dwSize)
	{
		SW_TRACE_DEBUG("Err: NULL == pbBuf || 0 == dwSize\n");
		return E_INVALIDARG;
	}
	
	if (0 == xmlDoc.Parse((const CHAR*)pbBuf))
	{
		SW_TRACE_DEBUG("Err: failed to parse cmd xml\n");
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
				SW_TRACE_NORMAL("Info: Got StartAutotest cmd\n");
				
				INT iTestCount = 0;
				if (NULL == Ele->Attribute("TestCount"))
				{
					SW_TRACE_DEBUG("Err: no TestCount info\n");
					return E_FAIL;
				}
				iTestCount = (INT)swpa_atoi(Ele->Attribute("TestCount"));

				const CHAR* szSendToIp = (CHAR*)Ele->Attribute("SendToIp");
				if (NULL == szSendToIp)
				{
					SW_TRACE_DEBUG("Err: no szSendToIp info, use default save path instead.\n");
					szSendToIp = "todo:default ip is??";
				}

				WORD wPort = 0;
				if (NULL == Ele->Attribute("SendToPort"))
				{
					SW_TRACE_DEBUG("Err: no SendToPort info, use default save path instead.\n");
					wPort = 8989;
				}
				wPort = (WORD)swpa_atoi(Ele->Attribute("SendToPort"));

				if (FAILED(ProcessAutotestCmd(iTestCount, szSendToIp, wPort)))
				{
					SW_TRACE_DEBUG("Err: failed to process autotest cmd.\n");
					return E_FAIL;
				}

				SW_TRACE_NORMAL("Info: StartAutotest cmd... OK\n");	
			}			
			else if (0 == swpa_strcmp(Ele->GetText(), "GetLatestReport"))
			{
				SW_TRACE_DEBUG("Info: Got GetLatestReport cmd\n");
				
				const CHAR* szSendToIp = (CHAR*)Ele->Attribute("SendToIp");
				if (NULL == szSendToIp)
				{
					SW_TRACE_DEBUG("Err: no szSendToIp info, use default save path instead.\n");
					szSendToIp = "todo:default ip is??";
				}

				WORD wPort = 0;
				if (NULL == Ele->Attribute("SendToPort"))
				{
					SW_TRACE_DEBUG("Err: no SendToPort info, use default save path instead.\n");
					wPort = 8989;//todo: this is the default port???
				}
				wPort = (WORD)swpa_atoi(Ele->Attribute("SendToPort"));
				
				hr = SendReportSavedInEEPROM(szSendToIp, wPort);
				
				SW_TRACE_DEBUG("Info: GetLatestReport ret 0x%x!\n", hr);	
			}
			else if (0 == swpa_strcmp(Ele->GetText(), "GetAutotestProgress"))
			{
				//SW_TRACE_NORMAL("Info: Got GetAutotestProgress cmd\n");
				hr = GetAutotestStatus(ppvOutBuf, pdwOutSize);
				//SW_TRACE_NORMAL("Info: GetAutotestProgress ret %#x!\n", hr);	
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
		SW_TRACE_DEBUG("Err: not inited yet!\n");
		return E_NOTIMPL;
	}	

	PBYTE pbBuf = (PBYTE)pbCmdData;
	DWORD dwSize = dwDataSize;
	
	if (CAMERA_XML_EXT_CMD == dwCmdID) // XML cmd
	{		
		hr = ProcessXmlCommand(pbBuf, dwSize, ppvOutBuf, pdwOutSize);
		
		//SW_TRACE_DEBUG("Info: ProcessXmlCommand ret %#x!\n", hr);		
	}
	else
	{
		SW_TRACE_DEBUG("Info: Got unsupported cmd : %#X!\n", dwCmdID);
		return E_INVALIDARG;
	}	

	return hr;
}



HRESULT CAutotest::SaveAutotestProgress()
{	
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not inited yet!\n");
		return E_NOTIMPL;
	}	
	
	CHAR szProgressInfo[256] = {0};
	BYTE bBuf[1024] = {0};
	
	swpa_snprintf(szProgressInfo, sizeof(szProgressInfo)-1, 
		"TestCount:%d, FailCount:%d, RunCount:%d, SendToPort:%d, SendToIp:%s",
		m_iTestCount, m_iNGCount, m_iRunCount, m_wSendPort, m_szSendIp);

	swpa_memcpy(bBuf+4, szProgressInfo, sizeof(szProgressInfo));
	assert(4+sizeof(szProgressInfo)+sizeof(m_sResultTable) <= sizeof(bBuf));
	swpa_memcpy(bBuf+4+sizeof(szProgressInfo), m_sResultTable, sizeof(m_sResultTable));
	
	DWORD dwCrc = CSWUtils::CalcCrc32(0, bBuf+4, sizeof(bBuf)-4);
	swpa_memcpy(bBuf, &dwCrc, sizeof(dwCrc));
	
	CSWFile cFile;
	if (FAILED(cFile.Open(szSavePath, "w+")))
	{
		SW_TRACE_DEBUG("Err: failed to open %s to save autotest progress!\n", szSavePath);
		return E_FAIL;
	}

	if (FAILED(cFile.Write(bBuf, sizeof(bBuf))))
	{
		SW_TRACE_DEBUG("Err: failed to save autotest progress!\n", szSavePath);
		return E_FAIL;
	}

	SW_TRACE_DEBUG("Info: save autotest progress -- OK!\n");

	return S_OK;
	
}




HRESULT CAutotest::RestoreAutotest()
{	
	BYTE bBuf[1024] = {0};
	DWORD dwLen = 0;

	if (!m_fInited && Initialize())
	{
		SW_TRACE_DEBUG("Err: failed to initialize Autotest!\n");
		return E_FAIL;
	}

	CSWFile cFile;
	if (FAILED(cFile.Open(szSavePath, "r")))
	{
		SW_TRACE_DEBUG("Err: failed to open %s to get autotest progress!\n", szSavePath);
		return E_FAIL;
	}

	if (FAILED(cFile.Read(bBuf, sizeof(bBuf), &dwLen))
		|| FAILED(cFile.Close())
		|| sizeof(bBuf) != dwLen)
	{
		SW_TRACE_DEBUG("Err: failed to read autotest progress!\n");
		return E_FAIL;
	}

	SW_TRACE_NORMAL("Info: Progress: %s\n", (CHAR*)bBuf+4);

	DWORD dwCrcStored = 0;
	DWORD dwCrcCalced = 0;

	swpa_memcpy(&dwCrcStored, bBuf, sizeof(dwCrcStored));
	dwCrcCalced = CSWUtils::CalcCrc32(0, bBuf+4, sizeof(bBuf)-4);

	SW_TRACE_DEBUG("Info: dwCrcStored = 0x%x, dwCrcCalced = 0x%x\n", dwCrcStored, dwCrcCalced);
	if (dwCrcCalced != dwCrcStored)
	{
		SW_TRACE_DEBUG("Err: autotest progress is Invalid, ignore it...!\n");
		return E_FAIL;
	}

	if (5 != swpa_sscanf((CHAR*)bBuf+4,	"TestCount:%d, FailCount:%d, RunCount:%d, SendToPort:%d, SendToIp:%s",
			&m_iTestCount, &m_iNGCount, &m_iRunCount, &m_wSendPort, m_szSendIp))
	{
		SW_TRACE_DEBUG("Err: failed to get autotest progress info!\n");
		return E_FAIL;
	}
	
	assert(4+256+sizeof(m_sResultTable) <= sizeof(bBuf));
	swpa_memcpy(m_sResultTable, bBuf+4+256, sizeof(m_sResultTable));

	SW_TRACE_DEBUG("Info: m_szSendIp: %s\n", m_szSendIp);
	SW_TRACE_DEBUG("Info: m_wSendPort: %d\n", m_wSendPort);
	SW_TRACE_DEBUG("Info: m_iTestCount: %d\n", m_iTestCount);
	SW_TRACE_DEBUG("Info: m_iNGCount: %d\n", m_iNGCount);
	SW_TRACE_DEBUG("Info: m_iRunCount: %d\n", m_iRunCount);

	
	if (m_iTestCount >= m_iRunCount+1)
	{
		SW_TRACE_DEBUG("Info: needs to start Autotest automatically...!\n");

		SetAutotestStopFlag(TRUE);
		m_pAutotestThread->Stop();
		//restart
		SetAutotestStopFlag(FALSE);
		if (FAILED(m_pAutotestThread->Start(AutotestProxy, this)))
		{
			SW_TRACE_DEBUG("Err: failed to start Autotest Thread.\n");
			return E_FAIL;
		}
	}
	else
	{
		SW_TRACE_DEBUG("Info: NO needs to start Autotest !\n");
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
#define HVXML_RETURNLEN				"RetLen"

	
	if (NULL == ppvOutBuf || NULL == pdwOutSize)
	{
		SW_TRACE_DEBUG("Err: NULL == ppvOutBuf || NULL == pdwOutSize\n");
		return E_INVALIDARG;
	}
	
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not inited yet!\n");
		return E_NOTIMPL;
	}

	TiXmlDocument xmlOutDoc;	
	TiXmlDeclaration *pDeclaration = NULL;
	TiXmlElement *pRootEle = NULL;

	pDeclaration = new TiXmlDeclaration("1.0","GB2312","yes");
	if (NULL == pDeclaration)
	{
		SW_TRACE_DEBUG("Err: no memory for pDeclaration\n");
		return E_OUTOFMEMORY;
	}
	xmlOutDoc.LinkEndChild(pDeclaration);

	pRootEle = new TiXmlElement(HVXML_HVCMDRESPOND);
	if (NULL == pRootEle)
	{	
		SW_TRACE_DEBUG("Err: no memory for pRootEle\n");
		return E_OUTOFMEMORY;
	}
	xmlOutDoc.LinkEndChild(pRootEle);
	
	pRootEle->SetAttribute(HVXML_VER, HV_XML_CMD_VERSION_NO);

	TiXmlElement *pReplyEle =  new TiXmlElement(HVXML_CMDNAME);
	if (NULL == pReplyEle)
	{	
		SW_TRACE_DEBUG("Err: no memory for pReplyEle\n");
		return E_OUTOFMEMORY;
	}
	pRootEle->LinkEndChild(pReplyEle);

	
	TiXmlText * pReplyText = new TiXmlText("GetAutotestProgress");
	if (NULL == pReplyText)
	{	
		SW_TRACE_DEBUG("Err: no memory for pReplyText\n");
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
		SW_TRACE_DEBUG("Err: no memory for *ppvOutBuf!\n");
		return E_OUTOFMEMORY;
	}
	swpa_memset(*ppvOutBuf, 0, *pdwOutSize);

	swpa_strncpy((CHAR*)*ppvOutBuf, xmlPrinter.CStr(), xmlPrinter.Size());
	
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




