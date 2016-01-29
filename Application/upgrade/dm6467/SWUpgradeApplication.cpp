/**
*/
#include "SWFC.h"
#include "swpa.h"
#include "SWUpgradeApplication.h"
#include "Tester.h"
#include "SWTCPSocket.h"
#include "drv_device.h"
#include "SWNetOpt.h"

#define PRINT //SW_TRACE_DEBUG


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



CREATE_PROCESS(CSWUpgradeApp);


CSWUpgradeApp::CSWUpgradeApp()
	:m_fInited(FALSE)
	,m_wPort(9910)
	,m_pUpgradeThread(NULL)
	,m_pProbeThread(NULL)
	,m_dwGETIP_COMMAND(0x00000101)
	,m_dwSETIP_COMMAND(0x00000102)
	,m_wProbePort(6666)
	,m_iUpgradeProgress(-1)
	,m_fGoingToExit(FALSE)
	,m_iExitCode(0)
#ifdef ENABLE_AUTOTEST
	,m_fAutotestStarted(FALSE)
#endif
{
	
}

CSWUpgradeApp::~CSWUpgradeApp()
{
	

}


HRESULT CSWUpgradeApp::ReleaseObj()
{
	if (!m_fInited)
	{
		return S_OK;
	}
	
	PRINT("Info: releasing the app!!\n");


	if (NULL != m_pUpgradeThread)
	{
		delete m_pUpgradeThread;
		m_pUpgradeThread = NULL;
	}

	//停止设备侦测监听线程
	if (NULL != m_pProbeThread)
	{
		delete m_pProbeThread;
		m_pProbeThread = NULL;
	}
	
#ifdef ENABLE_AUTOTEST
	if (m_fAutotestStarted)
	{
		m_cAutotest.ReleaseObj();
	}
#endif

	m_fInited = FALSE;

	return S_OK;
}




HRESULT CSWUpgradeApp::OnException(INT iSignalNo)
{
	PRINT("Info: Got exception %d!!\n", iSignalNo);

	
	CSWApplication::OnException(iSignalNo);

	exit(0);
}




HRESULT CSWUpgradeApp::InitInstance(const WORD wArgc, const CHAR** szArgv)
{
	
	if(FAILED(CSWApplication::InitInstance(wArgc, szArgv)))
	{
		PRINT("Err: failed to init CSWApplication\n");
		return E_FAIL;
	}

	return Initialize();
}



HRESULT CSWUpgradeApp::ReleaseInstance()
{
	if(FAILED(CSWApplication::ReleaseInstance()))
	{
		PRINT("Err: failed to release CSWApplication\n");
		return E_FAIL;
	}

	ReleaseObj();

	return S_OK;
}






HRESULT CSWUpgradeApp::Run()
{
	HRESULT hr = S_OK;
	
	if (!m_fInited)
	{
		PRINT("Err: not inited yet!\n");
		return E_NOTIMPL;
	}

	CHAR szIP[32] = {0};
	CHAR szMask[32] = {0};
	CHAR szGateway[32] = {0};

	HeartBeat();

	PRINT("Info: waiting CommandApp ready...\n");
	
	CSWApplication::Sleep(4000);

	printf("Info: UpgradeAPP Version: %d\n", GetVersion());

#ifndef ENABLE_AUTOTEST	
	INT iMode = 2;
	if (SWPAR_OK != swpa_device_get_resetmode(&iMode))
	{
		PRINT("Err: failed to get reset_mode, set it to Upgrade Mode!\n");
		if (SWPAR_OK != swpa_device_set_resetmode(2))
		{
			PRINT("Err: failed to set working_mode to Upgrade Mode!\n");
			hr = E_FAIL;
		}
	}
	else if (0 == iMode)
	{
		PRINT("Info: Resetmode is Normal Mode, switch to Normal Mode!\n");
		Exit(1);//switch to Normal Mode
		return E_FAIL;
	}
#endif

	
	if (SWPAR_OK != swpa_device_read_ipinfo(szIP, sizeof(szIP)-1, szMask, sizeof(szMask)-1, szGateway, sizeof(szGateway)-1))
	{
		PRINT("Err: failed to GET ipinfo from device!\n");
		return E_NOTIMPL;
	}

	PRINT("Info: from EEPROM: IP = %s, Mask = %s, Gateway = %s!\n", szIP, szMask, szGateway);

	if (SWPAR_OK != swpa_tcpip_setinfo("eth0", szIP, szMask, szGateway))
	{
		PRINT("Err: failed to SET ipinfo to system!\n");
		return E_FAIL;
	}


#ifdef ENABLE_AUTOTEST	
	m_cAutotest.RestoreAutotest();
#endif
	
	//开启设备侦测监听线程
	hr = m_pProbeThread->Start(OnProbeProxy, (VOID*)this);
	if (FAILED(hr))
	{
		PRINT("Err: failed to start probe thread\n");
		return E_FAIL;
	}
	

	CSWTCPSocket sockServer;

	if (FAILED(sockServer.Create()))
	{
		PRINT("Err: failed to create pServer!\n");
		return E_FAIL;
	}

	// 设置启动标志
	if( 0 == swpa_device_set_bootcount_flag(1) )
	{
		PRINT("info: set boot flag 1 ok.\n");
	}
	else
	{
		PRINT("Err: set boot flag 1 failed.\n");
	}

	while (FAILED(sockServer.Bind(NULL, m_wPort)))
	{
		PRINT("Err: failed to bind Port %d, trying again... !\n", m_wPort);

		CSWApplication::Sleep(1000);
	}

	sockServer.SetRecvTimeout(4000);
	sockServer.SetSendTimeout(4000);
	if (FAILED(sockServer.Listen()))
	{
		PRINT("Err: failed to listen!\n");
		return E_FAIL;
	}


#ifndef ENABLE_AUTOTEST
	//clear the bootcount if set runs in Normal Mode, o.w., keep the count
	swpa_device_set_bootcount(0);
#endif

	
	swpa_alarm_led_on();
	swpa_heart_led_on();

	BOOL fLedOn = TRUE;
	while (!IsExited())
	{		
		if (fLedOn)
		{
			swpa_alarm_led_on();
		    swpa_heart_led_on();
		}
		else
		{
			swpa_alarm_led_off();
		    swpa_heart_led_off();
		}

		fLedOn = !fLedOn;

		HeartBeat();		
		//PRINT("Info: HeartBea\n");
		
		SWPA_SOCKET_T outSock;
		hr = S_OK;
		
		if (!FAILED(sockServer.Accept(outSock)))
		{
			PRINT("Info: Cmd socket got a connection...\n");
			
			CSWTCPSocket sockData;
			sockData.Attach(outSock);

			PBYTE pbBuf = NULL;
			DWORD dwSize = 0;
			DWORD dwLen = 0;
			CAMERA_CMD_HEADER	sHeader;

			sockData.SetRecvTimeout(10000);

			if (FAILED(sockData.Read(&sHeader, sizeof(sHeader), &dwLen))
				|| sizeof(sHeader) != dwLen)
			{
				PRINT("Err: failed to read command header!\n");
				continue;
			}			

			RESPONSE_CMD_HEADER sResponse;
			PVOID pvOutBuf = NULL;
			DWORD dwOutSize = 0;
			
			if (CAMERA_XML_EXT_CMD == sHeader.dwID) // XML cmd
			{
				PRINT("Info: Got XML cmd!\n");

				dwSize = sHeader.dwInfoSize;
				PRINT("Info: Data size = %d!\n", dwSize);
				pbBuf = (PBYTE)swpa_mem_alloc(dwSize);
				if (NULL == pbBuf)
				{
					PRINT("Err: no memory for pbBuf!\n");
					continue;
				}

				PRINT("Info: Receiving data...!\n");
				if (FAILED(sockData.Read(pbBuf, dwSize, &dwLen))
					|| dwSize != dwLen)
				{
					PRINT("Err: failed to read command data!\n");

					SAFE_MEM_FREE(pbBuf);
					
					continue;
				}
				
				PRINT("Info: Received data size = %d!\n", dwLen);
				
				hr = ProcessXmlCmd(pbBuf, dwSize, &pvOutBuf, &dwOutSize);
				sResponse.dwID = CAMERA_XML_EXT_CMD;
				sResponse.dwInfoSize = dwOutSize;
				sResponse.iReturn = FAILED(hr) ? -1 : 0;

				SAFE_MEM_FREE(pbBuf);
			}
			else if (0xFFEE0001 == sHeader.dwID) // Upgrade Kernel cmd				
			{
				PRINT("Info: Got Upgrade Kernel cmd!\n");

				dwSize = sHeader.dwInfoSize;
				PRINT("Info: Data size = %d!\n", dwSize);
				pbBuf = (PBYTE)swpa_mem_alloc(dwSize);
				if (NULL == pbBuf)
				{
					PRINT("Err: no memory for pbBuf!\n");
					continue;
				}
				
				PRINT("Info: Receiving data...!\n");
				if (FAILED(sockData.Read(pbBuf, dwSize, &dwLen))
					|| dwSize != dwLen)
				{
					PRINT("Err: failed to read command data!\n");
					
					SAFE_MEM_FREE(pbBuf);
					continue;
				}
				
				PRINT("Info: Received data size = %d!\n", dwLen);				
				
				hr = UpgradeKernelBak(pbBuf, dwSize);
				sResponse.dwID = 0xFFEE0001;
				sResponse.dwInfoSize = 0;
				sResponse.iReturn = FAILED(hr) ? -1 : 0;

				SAFE_MEM_FREE(pbBuf);
			}
			else if (0xFFEE0002 == sHeader.dwID)	// Upgrade Rootfs cmd
			{
				PRINT("Info: Got Upgrade RootFS cmd!\n");

				dwSize = sHeader.dwInfoSize;
				PRINT("Info: Data size = %d!\n", dwSize);
				pbBuf = (PBYTE)swpa_mem_alloc(dwSize);
				if (NULL == pbBuf)
				{
					PRINT("Err: no memory for pbBuf!\n");
					continue;
				}
				
				PRINT("Info: Receiving data...!\n");
				if (FAILED(sockData.Read(pbBuf, dwSize, &dwLen))
					|| dwSize != dwLen)
				{
					PRINT("Err: failed to read command data!\n");
					
					SAFE_MEM_FREE(pbBuf);
					continue;
				}
				
				PRINT("Info: Received data size = %d!\n", dwLen);				
				
				hr = UpgradeRootfsBak(pbBuf, dwSize);
				sResponse.dwID = 0xFFEE0002;
				sResponse.dwInfoSize = 0;
				sResponse.iReturn = FAILED(hr) ? -1 : 0;

				SAFE_MEM_FREE(pbBuf);
			}
			else if (0xFFEE0003 == sHeader.dwID)	// Upgrade the whole package		
			{
				PRINT("Info: Got Upgrade Package cmd!\n");

				m_dwImageLen = sHeader.dwInfoSize;
				PRINT("Info: Data size = %d!\n", m_dwImageLen);
				m_pbUpgradeImage = (PBYTE)swpa_mem_alloc(m_dwImageLen);
				if (NULL == m_pbUpgradeImage)
				{
					PRINT("Err: no memory for pbBuf!\n");
					continue;
				}
				
				PRINT("Info: Receiving data...!\n");
				if (FAILED(sockData.Read(m_pbUpgradeImage, m_dwImageLen, &dwLen))
					|| m_dwImageLen != dwLen)
				{
					PRINT("Err: failed to read command data!\n");
					
					SAFE_MEM_FREE(m_pbUpgradeImage);
					continue;
				}
				
				PRINT("Info: Received data size = %d!\n", dwLen);				
				
				hr = UpgradeImage();
				sResponse.dwID = 0xFFEE0003;
				sResponse.dwInfoSize = 0;
				sResponse.iReturn = FAILED(hr) ? -1 : 0;

				if (FAILED(hr))
				{
					SAFE_MEM_FREE(m_pbUpgradeImage);
					m_dwImageLen = 0;
				}

				//SAFE_MEM_FREE(m_pbUpgradeImage); should be freed at upgrading finished in the Upgrade thread
			}
			else if (0xFFEE0004 == sHeader.dwID)	// get Upgrade status			
			{
				PRINT("Info: Got Get Upgrade Status cmd!\n");
				
				
				hr = GetUpgradeImageStatus(&pvOutBuf, &dwOutSize);
				sResponse.dwID = 0xFFEE0004;
				sResponse.dwInfoSize = dwOutSize;
				sResponse.iReturn = FAILED(hr) ? -1 : 0;
			}
			else if (0xFFEE0005 == sHeader.dwID)	// get DeviceInfo 		
			{
				PRINT("Info: Got GetDeviceInfo cmd!\n");
				
				
				hr = GetDeviceInfo(&pvOutBuf, &dwOutSize);
				sResponse.dwID = 0xFFEE0005;
				sResponse.dwInfoSize = dwOutSize;
				sResponse.iReturn = FAILED(hr) ? -1 : 0;
			}
			else
			{
				PRINT("Info: Got unsupported cmd : %#X, ignore it!\n", sHeader.dwID);
				continue;
			}

			PRINT("Info: cmd processing return %#x!\n", hr);
			PRINT("Info: Sending response data...\n");
			if (FAILED(sockData.Send(&sResponse, sizeof(sResponse), &dwLen))
				|| sizeof(sResponse) != dwLen)
			{
				PRINT("Err: failed to send response msg!\n");
				continue;
			}

			if (0 != sResponse.dwInfoSize && NULL != pvOutBuf && 0 != dwOutSize)
			{
				if (FAILED(sockData.Send(pvOutBuf, dwOutSize, &dwLen))
					|| dwOutSize != dwLen)
				{
					PRINT("Err: failed to send response data!\n");

					SAFE_MEM_FREE(pvOutBuf);
					dwOutSize = 0;
					continue;
				}

				SAFE_MEM_FREE(pvOutBuf);
				dwOutSize = 0;
			}
			
			PRINT("Info: Sent response data -- OK (len = %d)\n", dwLen);

			if (m_fGoingToExit)
			{
				Exit(m_iExitCode);
				break;
			}
		}
	}
	
	sockServer.Close();

	return S_OK;
}




HRESULT CSWUpgradeApp::Initialize()
{
	if (m_fInited)
	{
		return S_OK;
	}

	HeartBeat();

#ifdef ENABLE_AUTOTEST
	if (SWPAR_OK != swpa_utils_shell("ifconfig eth0 192.168.1.10 netmask 255.255.255.0"))
	{
		PRINT("Err: failed to SET default ip (192.168.1.10) info to system!\n");
		return E_FAIL;
	}
#endif

	if (NULL == m_pUpgradeThread)
	{
		m_pUpgradeThread = new CSWThread();
		if (NULL == m_pUpgradeThread)
		{
			PRINT("Err: no memory for m_pUpgradeThread\n");
			return E_OUTOFMEMORY;
		}		
	}

	if (NULL == m_pProbeThread)
	{
		m_pProbeThread = new CSWThread();
		if (NULL == m_pProbeThread)
		{
			PRINT("Err: no memory for m_pProbeThread\n");
			return E_OUTOFMEMORY;
		}
	}

	m_fInited = TRUE;
	
	return S_OK;
	
}




HRESULT CSWUpgradeApp::GenerateProbeReplyXml(CHAR ** ppszXmlBuf, DWORD *pdwXmlLen)
{
#define HV_XML_CMD_VERSION_NO "3.0"
	
	
#define HVXML_VER 					"Ver"
#define HVXML_HVCMD					"HvCmd"
#define HVXML_HVCMDRESPOND 			"HvCmdRespond"
#define HVXML_RETCODE 				"RetCode"
#define HVXML_RETMSG 				"RetMsg"
#define HVXML_CMDNAME				"CmdName"

	const DWORD _INFO_LEN = 256;
	const DWORD _MSG_LEN = _INFO_LEN * 2;
	HRESULT hr = S_OK;
	INT	iRet = SWPAR_OK;
	//CHAR szMsg[_MSG_LEN] = {0};
	CHAR szSN[_INFO_LEN] = {0};
	CHAR szMode[_INFO_LEN] = {0};//{0};
	DWORD dwLen = _INFO_LEN;

	CHAR * pszXmlData = NULL;
	TiXmlDocument * pXmlOutputDoc = NULL;
	TiXmlDeclaration *pDeclaration = NULL;
	TiXmlElement *pRootEle = NULL;
	TiXmlElement *pReplyEle = NULL;	
    TiXmlPrinter * pXmlPrinter = NULL; 
	TiXmlText *pReplyText = NULL;
	
	CHAR szIP[32] = {0};
	CHAR szNetMask[32] = {0};
	CHAR szMAC[32] = {0};
	CHAR szGateway[32] = {0};


	if (NULL == ppszXmlBuf || NULL == pdwXmlLen)
	{
		PRINT("Err: NULL == ppszXmlBuf || NULL == pdwXmlLen\n");
		return E_INVALIDARG;
	}

	//CAMERA_CMD_RESPOND sCmdHeader;
	//swpa_memset(&sCmdHeader, 0, sizeof(sCmdHeader));
	
	
	DWORD dwNetID = 0;
	CHAR szEthernet[8] = {0};
	do {		
		swpa_sprintf(szEthernet, "eth%d", dwNetID);
		iRet = swpa_tcpip_getinfo(szEthernet, szIP, 32, szNetMask, 32, szGateway, 32, szMAC, 32);
		dwNetID++;
	} while ( SWPAR_OK != iRet && 5 > dwNetID );

	if (5 <= dwNetID)
	{
		PRINT("Err: failed to get net info! (eth0~eth%d)\n", dwNetID);
		return E_FAIL;
	}

	iRet = swpa_device_read_sn(szSN, &dwLen);
	if (SWPAR_OK != iRet)
	{
		PRINT("Err: failed to get device serial number\n");
		return E_FAIL;
	}

	//PRINT("Info: Probe sn = %s\n", szSN);

	INT iMode;
#ifndef ENABLE_AUTOTEST	
	iRet = swpa_device_get_resetmode(&iMode);
	if (SWPAR_OK != iRet)
	{
		PRINT("Err: failed to get device working mode\n");
		return E_FAIL;
	}
#else
	iMode = 1;
#endif
	swpa_snprintf(szMode, sizeof(iMode)-1, "%s", 0 == iMode ? "正常模式" : "升级模式");	
	
	//swpa_snprintf(szMsg, _MSG_LEN - 1, "IP:%s;MAC:%s;SN:%s;MODE:%s;", szIP, szMAC, szSN, szMode);

	pXmlOutputDoc = new TiXmlDocument();
	if (NULL == pXmlOutputDoc)
	{
		PRINT("Err: no memory for pXmlOutputDoc\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}	

	pDeclaration = new TiXmlDeclaration("1.0","GB2312","yes");
	if (NULL == pDeclaration)
	{
		PRINT("Err: no memory for pDeclaration\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}
	pXmlOutputDoc->LinkEndChild(pDeclaration);

	pRootEle = new TiXmlElement(HVXML_HVCMDRESPOND);
	if (NULL == pRootEle)
	{
		PRINT("Err: no memory for pRootEle\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}
	pRootEle->SetAttribute(HVXML_VER, HV_XML_CMD_VERSION_NO);
	

	pReplyEle = new TiXmlElement(HVXML_CMDNAME);
	if (NULL == pReplyEle)
	{
		PRINT("Err: no memory for pReplyEle\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}

	pReplyEle->SetAttribute("IP", szIP);
	pReplyEle->SetAttribute("Mask", szNetMask);
	pReplyEle->SetAttribute("Gateway", szGateway);
	pReplyEle->SetAttribute("MAC", szMAC);
	pReplyEle->SetAttribute("SN", szSN);
	pReplyEle->SetAttribute("Mode", szMode);
	
	pReplyEle->SetAttribute(HVXML_RETCODE, 0);
	pReplyEle->SetAttribute(HVXML_RETMSG, "OK");

	pReplyText = new TiXmlText("Probe");  
	if (NULL == pReplyText)
	{
		PRINT("Err: no memory for pReplyText\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}
	pReplyEle->LinkEndChild(pReplyText);
	
	pRootEle->LinkEndChild(pReplyEle);
	
	pXmlOutputDoc->LinkEndChild(pRootEle);

	pXmlPrinter = new TiXmlPrinter();
	if (NULL == pXmlPrinter)
	{
		PRINT("Err: no memory for pXmlPrinter\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}
	
    pXmlOutputDoc->Accept(pXmlPrinter);
	
	pszXmlData = (CHAR*)swpa_mem_alloc(pXmlPrinter->Size() /* + sizeof(sCmdHeader)*/);
	if (NULL == pszXmlData)
	{
		PRINT("Err: no memory for pszXmlData\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}

	//sCmdHeader.dwID = CAMERA_XML_EXT_CMD;
	//sCmdHeader.dwInfoSize = pXmlPrinter->Size();
	//sCmdHeader.iResult = 0;
	
    swpa_memset(pszXmlData, 0x0, pXmlPrinter->Size() /*+ sizeof(sCmdHeader)*/);
	
	//swpa_memcpy(pszXmlData, &sCmdHeader, sizeof(sCmdHeader));
    swpa_memcpy(pszXmlData/*+sizeof(sCmdHeader)*/, pXmlPrinter->CStr(), pXmlPrinter->Size());

	*ppszXmlBuf = pszXmlData;
	*pdwXmlLen = pXmlPrinter->Size() /*+sizeof(sCmdHeader)*/;
		 
OUT:

	SAFE_DELETE(pXmlPrinter);
	SAFE_DELETE(pXmlOutputDoc);

	return hr;
}


HRESULT CSWUpgradeApp::ParseProbeXmlMsg(CHAR * pszMsg)
{
	if (NULL == pszMsg)
	{
		PRINT("Err: NULL == pszMsg\n");
		return E_INVALIDARG;
	}
	
	TiXmlDocument  XmlDoc;

	XmlDoc.Parse(pszMsg);

	TiXmlElement * pEleRoot = XmlDoc.RootElement();
	if(NULL != pEleRoot)
	{
		for (TiXmlElement *Ele = pEleRoot->FirstChildElement(); NULL != Ele; Ele = Ele->NextSiblingElement())
		{			
			const CHAR * szText = Ele->GetText();
			//PRINT("Info: Ele->GetText() = %s\n", szText);
			if (NULL != szText)
			{
				if (0 == swpa_strcmp("Probe", szText)
					|| 0 == swpa_strcmp("probe", szText)
					|| 0 == swpa_strcmp("PROBE", szText))
				{
					return S_OK;
				}
			}
		}
	}

	return E_FAIL;
}



HRESULT CSWUpgradeApp::GetDeviceInfo(PVOID* ppvOutBuf, DWORD* pdwOutLen)
{
	if (NULL == ppvOutBuf || NULL == pdwOutLen)
	{
		return E_INVALIDARG;
	}

	*ppvOutBuf = NULL;
	*pdwOutLen = 0;

	INT iRet = SWPAR_OK;	
	CHAR szIP[32] = {0};
	CHAR szMask[32] = {0};
	CHAR szGateway[32] = {0};
	CHAR szMAC[32] = {0};
	
	BYTE bMsg[256] = {0};

	DWORD dwIP[4] = {0};
	DWORD dwMask[4] = {0};
	DWORD dwGateway[4] = {0};
	DWORD dwMAC[6] = {0};

	CHAR szSN[256] = {0};
	CHAR szMode[128] = {0};
	DWORD dwLen = 0;

	DWORD dwNetID = 0;
	CHAR szEthernet[8] = {0};
	do {		
		swpa_sprintf(szEthernet, "eth%d", dwNetID);
		iRet = swpa_tcpip_getinfo(szEthernet, szIP, 32, szMask, 32, szGateway, 32, szMAC, 32);
		dwNetID++;
	} while ( SWPAR_OK != iRet && 5 > dwNetID );

	if (5 <= dwNetID)
	{
		PRINT("Err: failed to get net info! (eth0~eth%d)\n", dwNetID);
		return E_FAIL;
	}
	
	
	PRINT("Info: got ip info...\n");
	PRINT("Info: szIP: %s; szMask: %s; szMAC: %s; szGateway: %s\n", szIP,  szMask,  szMAC, szGateway);
	
	swpa_sscanf(szIP, "%d.%d.%d.%d", &dwIP[0], &dwIP[1], &dwIP[2], &dwIP[3]);
	swpa_sscanf(szMask, "%d.%d.%d.%d", &dwMask[0], &dwMask[1], &dwMask[2], &dwMask[3]);
	swpa_sscanf(szGateway, "%d.%d.%d.%d", &dwGateway[0], &dwGateway[1], &dwGateway[2], &dwGateway[3]);
	swpa_sscanf(szMAC, "%x:%x:%x:%x:%x:%x", &dwMAC[0], &dwMAC[1], &dwMAC[2], &dwMAC[3], &dwMAC[4], &dwMAC[5]);

	DWORD dwIPVal = 0;
	DWORD dwMaskVal = 0;
	DWORD dwGatewayVal = 0;
	BYTE bMACVal[6] = {0};
	//DWORD dwIPVal = 0;
	dwIPVal = dwIP[0]<<24 | dwIP[1]<<16 | dwIP[2]<<8 | dwIP[3];	
	dwMaskVal = dwMask[0]<<24 | dwMask[1]<<16 | dwMask[2]<<8 | dwMask[3];
	dwGatewayVal = dwGateway[0]<<24 | dwGateway[1]<<16 | dwGateway[2]<<8 | dwGateway[3];
	for (DWORD i=0; i<sizeof(bMACVal); i++)
	{
		bMACVal[i] = dwMAC[i] & 0xFF;
		//PRINT("Info: bMACVal[i] = %#x\n", bMACVal[i]);
	}

	DWORD dwCmd = 0xFFEE0005;
	swpa_memcpy(&bMsg[0], &dwCmd, 4);
	swpa_memcpy(&bMsg[4], bMACVal, 6);
	swpa_memcpy(&bMsg[10], &dwIPVal, 4);
	swpa_memcpy(&bMsg[14], &dwMaskVal, 4);
	swpa_memcpy(&bMsg[18], &dwGatewayVal, 4);
		
	dwLen = sizeof(szSN);
	iRet = swpa_device_read_sn(szSN, &dwLen);
	if (SWPAR_OK != iRet)
	{
		PRINT("Err: failed to get device serial number\n");
		return E_FAIL;
	}

	if (dwLen > 0 && dwLen < 32)
	{
		swpa_memcpy(&bMsg[22], szSN, dwLen);
		*pdwOutLen = 22+dwLen;
	}
	else
	{
		PRINT("Err: Got invalid SN from EEPROM\n");
		return E_FAIL;
	}
	
	*ppvOutBuf = swpa_mem_alloc(*pdwOutLen);
	if (NULL == *ppvOutBuf)
	{
		PRINT("Err: no memory for *pszReplyPack\n");
		return E_OUTOFMEMORY;
	}
	swpa_memset(*ppvOutBuf, 0, *pdwOutLen);
	swpa_memcpy(*ppvOutBuf, bMsg, *pdwOutLen);
	return S_OK;
}




HRESULT CSWUpgradeApp::GenerateSetIPCMDReplay(const BYTE* pbMsg, const DWORD dwMsgLen, BYTE** pbReplyPack, DWORD* pdwPackLen)
{
	
	if (NULL == pbMsg || NULL == pbReplyPack || NULL == pdwPackLen)
	{
		return E_INVALIDARG;
	}

	INT iRet = SWPAR_OK;	
	CHAR szIP[32] = {0};
	CHAR szMask[32] = {0};
	CHAR szGateway[32] = {0};
	CHAR szMAC[32] = {0};
	
	BYTE bMsg[256] = {0};

	BYTE bIP[4] = {0};
	BYTE bMask[4] = {0};
	BYTE bGateway[4] = {0};
	BYTE bMAC[6] = {0};
	BYTE bThisMAC[6] = {0};
	DWORD dwThisMAC[6] = {0};

	CHAR szSN[128] = {0};
	CHAR szMode[128] = {0};
	INT iLen = 0;

	DWORD dwNetID = 0;
	CHAR szEthernet[8] = {0};
	do {		
		swpa_sprintf(szEthernet, "eth%d", dwNetID);
		iRet = swpa_tcpip_getinfo(szEthernet, szIP, 32, szMask, 32, szGateway, 32, szMAC, 32);
		dwNetID++;
	} while ( SWPAR_OK != iRet && 5 > dwNetID );

	if (5 <= dwNetID)
	{
		PRINT("Err: failed to get net info! (eth0~eth%d)\n", dwNetID);
		return E_FAIL;
	}
	
	swpa_sscanf(szMAC, "%x:%x:%x:%x:%x:%x", &dwThisMAC[0], &dwThisMAC[1], &dwThisMAC[2], &dwThisMAC[3], &dwThisMAC[4], &dwThisMAC[5]);
	for (DWORD i=0; i<sizeof(bThisMAC); i++)
	{
		bThisMAC[i] = dwThisMAC[i] & 0xFF;
	}
	swpa_memcpy(bMAC, &pbMsg[4], 6);

	if (0 == swpa_memcmp(bMAC, bThisMAC, sizeof(bMAC)))
	{
		swpa_memcpy(bIP, &pbMsg[10], 4);
		swpa_memcpy(bMask, &pbMsg[14], 4);
		swpa_memcpy(bGateway, &pbMsg[18], 4);

		swpa_snprintf(szIP, sizeof(szIP)-1, "%d.%d.%d.%d", bIP[3], bIP[2], bIP[1], bIP[0]);
		swpa_snprintf(szMask, sizeof(szMask)-1, "%d.%d.%d.%d", bMask[3], bMask[2], bMask[1], bMask[0]);
		swpa_snprintf(szGateway, sizeof(szGateway)-1, "%d.%d.%d.%d", bGateway[3], bGateway[2], bGateway[1], bGateway[0]);
		
		PRINT("Info: szIP = %s\n", szIP);
		PRINT("Info: szMask = %s\n", szMask);
		PRINT("Info: szGateway = %s\n", szGateway);

		iRet = swpa_tcpip_setinfo(szEthernet, szIP, szMask, szGateway);
		if (SWPAR_OK != iRet)
		{	
			PRINT("Err: failed to set net info\n");
			return E_FAIL;
		}

		*pdwPackLen = 8;
		*pbReplyPack = (BYTE*)swpa_mem_alloc(*pdwPackLen);
		if (NULL == *pbReplyPack)
		{
			PRINT("Err: no memory for *pszReplyPack\n");
			return E_OUTOFMEMORY;
		}
		swpa_memset(*pbReplyPack, 0, *pdwPackLen);
		swpa_memcpy(*pbReplyPack, &m_dwSETIP_COMMAND, sizeof(m_dwSETIP_COMMAND));
	}


	return S_OK;
	
}




HRESULT CSWUpgradeApp::OnProbe(VOID)
{
	HRESULT hr = S_OK;

	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}	


	CSWUDPSocket * pUDPSock = NULL;
	pUDPSock = new CSWUDPSocket();
	if (NULL == pUDPSock || FAILED(pUDPSock->Create()))
	{
		PRINT("Err: no memory for pUDPSock\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}

	while (FAILED(pUDPSock->Bind(NULL, m_wProbePort)))
	{
		PRINT("Warning: udpsock failed to bind to Port #%d\n", m_wProbePort);
		CSWApplication::Sleep(1000);
	}

	pUDPSock->SetRecvTimeout(4000);
	pUDPSock->SetSendTimeout(4000);

	PRINT("Info: Probe Thread listening Port #%d\n", m_wProbePort);
	while (!IsExited())
	{
		BYTE bMsgIn[512] = {0};
		DWORD dwRecvLen = 0;
		DWORD dwSendLen = 0;

		hr = pUDPSock->RecvFrom((VOID*)bMsgIn, sizeof(bMsgIn), &dwRecvLen);
		if (FAILED(hr) || 0 >= dwRecvLen)
		{			
			//CSWApplication::Sleep(2000);
			continue;
		}

		PRINT("Info: dwRecvLen = %d\n", dwRecvLen);
		
		BYTE* pbReplyPack = NULL;
		DWORD dwPackLen = 0;
		if (sizeof(DWORD) == dwRecvLen && (0 == swpa_memcmp(&bMsgIn[0], &m_dwGETIP_COMMAND, sizeof(m_dwGETIP_COMMAND))))
		{
			PRINT("Info: Got GetIp CMD\n");
			
			if (FAILED(GenerateProbeReplyXml((CHAR**)&pbReplyPack, &dwPackLen)))
			{
				PRINT("Err: failed to generate GetIP reply xml\n");
				
				SAFE_MEM_FREE(pbReplyPack);
				dwPackLen = 0;
				continue;//return E_FAIL;
			}

			PRINT("Info: GetIp -- OK\n");
		}
		else if (22 == dwRecvLen && 0 == swpa_memcmp(&bMsgIn[0], &m_dwSETIP_COMMAND, sizeof(m_dwSETIP_COMMAND)))
		{
			PRINT("Info: Got SetIp CMD\n");
			if (FAILED(GenerateSetIPCMDReplay(bMsgIn, dwRecvLen, &pbReplyPack, &dwPackLen)))
			{
				PRINT("Err: failed to generate SetIP Reply Msg\n");
				SAFE_MEM_FREE(pbReplyPack);
				dwPackLen = 0;
				continue;
			}

			PRINT("Info: SetIp -- OK\n");
		}
		else
		{
			//PRINT("Err: discards unknown cmd\n");
			continue;
		}

 		if (NULL != pbReplyPack && 0 != dwPackLen)
		{
			WORD wDstPort = 0;
			pUDPSock->GetPeerName(NULL, &wDstPort);		
			if (FAILED(pUDPSock->Broadcast(wDstPort, pbReplyPack, dwPackLen)) )
			{
				PRINT("Err: failed to broadcast response data\n");
				
				SAFE_MEM_FREE(pbReplyPack);
				dwPackLen = 0;
				continue;				
			}

			PRINT("Info: Broadcast response -- OK\n");

			SAFE_MEM_FREE(pbReplyPack);
			dwPackLen = 0;
		}
	}

OUT:


	SAFE_DELETE(pUDPSock);

	return hr;
}


VOID* CSWUpgradeApp::OnProbeProxy(VOID* pvParam)
{
	if (NULL == pvParam)
	{
		return (VOID*)E_INVALIDARG;
	}

	CSWUpgradeApp * pThis = (CSWUpgradeApp *)pvParam;

	return (VOID*)pThis->OnProbe();
}




HRESULT CSWUpgradeApp::UpgradeRootfsBak(const PVOID pvBuf, const DWORD dwSize)
{
	if (NULL == pvBuf || 0 == dwSize)
	{
		PRINT("Err: NULL == pvBuf || 0 == dwSize\n");
		return E_INVALIDARG;
	}

	PRINT("Info: Upgrading RootFS...\n");
	
	CSWFile sFile;

	if (FAILED(sFile.Open("FLASH/0/ROOTFS_BAK", "w")))
	{
		PRINT("Err: failed to open %s\n", "FLASH/0/ROOTFS_BAK");
		return E_FAIL;
	}

	DWORD dwWrittenLen = 0;
	if (FAILED(sFile.Write(pvBuf, dwSize, &dwWrittenLen)) || dwSize != dwWrittenLen)
	{
		PRINT("Err: failed to write %s\n", "FLASH/0/ROOTFS_BAK");
		return E_FAIL;
	}

	PRINT("Info: Upgrading RootFS -- OK\n");
	
	return sFile.Close();
}


HRESULT CSWUpgradeApp::UpgradeKernelBak(const PVOID pvBuf, const DWORD dwSize)
{
	if (NULL == pvBuf || 0 == dwSize)
	{
		PRINT("Err: NULL == pvBuf || 0 == dwSize\n");
		return E_INVALIDARG;
	}

	PRINT("Info: Upgrading Kernel...\n");
	
	CSWFile sFile;

	if (FAILED(sFile.Open("FLASH/0/KERNEL_BAK", "w")))
	{
		PRINT("Err: failed to open %s\n", "FLASH/0/KERNEL_BAK");
		return E_FAIL;
	}

	DWORD dwWrittenLen = 0;
	if (FAILED(sFile.Write(pvBuf, dwSize, &dwWrittenLen)) || dwSize != dwWrittenLen)
	{
		PRINT("Err: failed to write %s\n", "FLASH/0/KERNEL_BAK");
		return E_FAIL;
	}

	PRINT("Info: Upgrading Kernel -- OK\n");
	
	return sFile.Close();
}



HRESULT CSWUpgradeApp::OnUpgradeImage()
{
	if (!m_fInited)
	{
		return E_NOTIMPL;
	}

	if (NULL == m_pbUpgradeImage || 0 == m_dwImageLen)
	{
		return E_INVALIDARG;
	}
	
	PRINT("Info: Upgrading Image ...\n");

	HRESULT hr = WriteUpgradePacket((const CHAR*)m_pbUpgradeImage, m_dwImageLen);
	if (FAILED(hr))
	{
		PRINT("Err: failed to Write Upgrade Image\n");
		return hr;
	}

	PRINT("Info: Upgrading Image -- OK\n");

	SAFE_MEM_FREE(m_pbUpgradeImage);
	m_dwImageLen = 0;

	return m_pUpgradeThread->Stop();
}





PVOID CSWUpgradeApp::OnUpgradeImageProxy(PVOID pvArg)
{
	if (NULL == pvArg)
	{
		return (PVOID)E_INVALIDARG;
	}
	
	CSWUpgradeApp* pThis = (CSWUpgradeApp*)pvArg;

	return (PVOID)pThis->OnUpgradeImage();
}





HRESULT CSWUpgradeApp::OnOptResetMode(PVOID* ppvOutBuf, DWORD* pdwOutSize)
{
	if (NULL == ppvOutBuf || NULL == pdwOutSize)
	{
		PRINT("Err: NULL == ppvOutBuf || NULL == pdwOutSize\n");
		return E_INVALIDARG;
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

	
	TiXmlText * pReplyText = new TiXmlText("OptResetMode");
	if (NULL == pReplyText)
	{	
		PRINT("Err: no memory for pReplyText\n");
		return E_OUTOFMEMORY;
	}
	pReplyEle->LinkEndChild(pReplyText);

	pReplyEle->SetAttribute(HVXML_RETCODE, 0);

	INT iMode = 0;
#ifndef ENABLE_AUTOTEST	
	if (SWPAR_OK != swpa_device_get_resetmode(&iMode))
	{
		PRINT("Err: failed to get reset mode value!\n");
		pReplyEle->SetAttribute(HVXML_RETCODE, -1);
		pReplyEle->SetAttribute(HVXML_RETMSG, "FAILED");
	}
	//PRINT("Info: iMode = %d\n", iMode);
#else
	iMode = 1;
#endif
	pReplyEle->SetAttribute(HVXML_RETMSG, iMode);

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

	return S_OK;	
}



HRESULT CSWUpgradeApp::OnRestoreFactorySetting(PVOID* ppvOutBuf, DWORD* pdwOutSize)
{
	PRINT("Info: Got RestoreFactorySetting Cmd\n");
	
	if (NULL == ppvOutBuf || NULL == pdwOutSize)
	{
		PRINT("Err: NULL == ppvOutBuf || NULL == pdwOutSize\n");
		return E_INVALIDARG;
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

	
	TiXmlText * pReplyText = new TiXmlText("RestoreFactorySetting");
	if (NULL == pReplyText)
	{	
		PRINT("Err: no memory for pReplyText\n");
		return E_OUTOFMEMORY;
	}
	pReplyEle->LinkEndChild(pReplyText);


	if (SWPAR_OK == swpa_device_lock_uboot()
		&& SWPAR_OK == swpa_device_lock_linuxinfo()
		&& SWPAR_OK == swpa_device_set_bootdelay(0)
		&& SWPAR_OK == swpa_device_restore_defaultinfo()
		)
	{	
		pReplyEle->SetAttribute(HVXML_RETCODE, 0);
		pReplyEle->SetAttribute(HVXML_RETMSG, "OK");
	}
	else
	{
		pReplyEle->SetAttribute(HVXML_RETCODE, -1);
		pReplyEle->SetAttribute(HVXML_RETMSG, "NG");
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

	PRINT("Info: RestoreFactorySetting Cmd processed -- OK\n");

	return S_OK;	
}




HRESULT CSWUpgradeApp::OnUnlockFactorySetting(PVOID* ppvOutBuf, DWORD* pdwOutSize)
{
	PRINT("Info: Got UnlockFactorySetting Cmd\n");
	
	if (NULL == ppvOutBuf || NULL == pdwOutSize)
	{
		PRINT("Err: NULL == ppvOutBuf || NULL == pdwOutSize\n");
		return E_INVALIDARG;
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

	
	TiXmlText * pReplyText = new TiXmlText("UnlockFactorySetting");
	if (NULL == pReplyText)
	{	
		PRINT("Err: no memory for pReplyText\n");
		return E_OUTOFMEMORY;
	}
	pReplyEle->LinkEndChild(pReplyText);	


	if (SWPAR_OK == swpa_device_unlock_uboot()
		&& SWPAR_OK == swpa_device_unlock_linuxinfo()
		//&& SWPAR_OK == swpa_device_set_bootdelay(3)
		)
	{	
		pReplyEle->SetAttribute(HVXML_RETCODE, 0);
		pReplyEle->SetAttribute(HVXML_RETMSG, "OK");
	}
	else
	{
		pReplyEle->SetAttribute(HVXML_RETCODE, -1);
		pReplyEle->SetAttribute(HVXML_RETMSG, "NG");
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

	PRINT("Info: UnlockFactorySetting Cmd processed -- OK\n");
	
	return S_OK;	
}




HRESULT CSWUpgradeApp::OnWriteSN(const CHAR* szSN, PVOID* ppvOutBuf, DWORD* pdwOutSize)
{
	PRINT("Info: Got WriteSN Cmd\n");
	
	if (NULL == szSN || NULL == ppvOutBuf || NULL == pdwOutSize)
	{
		PRINT("Err: NULL == szSN || NULL == ppvOutBuf || NULL == pdwOutSize\n");
		return E_INVALIDARG;
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

	
	TiXmlText * pReplyText = new TiXmlText("WriteSN");
	if (NULL == pReplyText)
	{	
		PRINT("Err: no memory for pReplyText\n");
		return E_OUTOFMEMORY;
	}
	pReplyEle->LinkEndChild(pReplyText);	

	CHAR szSNtoSave[22] = {0}; //restricted max len to 21
	swpa_strncpy(szSNtoSave, szSN, sizeof(szSNtoSave)-1);
	if (SWPAR_OK == swpa_device_write_sn((CHAR*)szSNtoSave))
	{	
		pReplyEle->SetAttribute(HVXML_RETCODE, 0);
		pReplyEle->SetAttribute(HVXML_RETMSG, "OK");
	}
	else
	{
		pReplyEle->SetAttribute(HVXML_RETCODE, -1);
		pReplyEle->SetAttribute(HVXML_RETMSG, "NG");
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

	PRINT("Info: WriteSN Cmd processed -- OK\n");
	
	return S_OK;	
}







HRESULT CSWUpgradeApp::OnResetDevice(const INT iExitCode)
{

	PRINT("Info: iExitCode = %d!\n", iExitCode);
	
	if (1 == iExitCode) //switch to Normal Mode(used only after Upgrade being finished), need to clear the boot count
	{
		swpa_device_set_bootcount(0);

		if (SWPAR_OK != swpa_device_set_resetmode(0))
		{
			PRINT("Err: failed to set working_mode to normal!\n");
			return E_FAIL;
		}

		m_iExitCode = 2; //force to reboot after Upgrade		
		m_fGoingToExit = TRUE;
	}
	else if (2 == iExitCode) //Reboot
	{
		m_iExitCode = iExitCode;
		m_fGoingToExit = TRUE;
	}
#ifdef ENABLE_AUTOTEST
	else if (3 == iExitCode) //switch to Normal Mode(response to switch Factory-Mode to Normal-Mode), need to clear the boot count
	{
		swpa_device_set_bootcount(0);
		
		if (SWPAR_OK != swpa_device_set_resetmode(0))
		{
			PRINT("Err: failed to set working_mode to normal!\n");
			return E_FAIL;
		}
		m_iExitCode = 2; //force to reboot
		m_fGoingToExit = TRUE;
	}
#endif	
	
	
	return S_OK;	
}




HRESULT CSWUpgradeApp::ProcessXmlCmd(const PBYTE pbBuf, const DWORD dwSize, PVOID* ppvOutBuf, DWORD* pdwOutSize)
{
	HRESULT hr = S_OK;

	TiXmlDocument xmlDoc;

	if (NULL == pbBuf || 0 == dwSize || NULL == ppvOutBuf || NULL == pdwOutSize)
	{
		PRINT("Err: NULL == pbBuf || 0 == dwSize || NULL == ppvOutBuf || NULL == pdwOutSize\n");
		return E_INVALIDARG;
	}

	PRINT("Info: pbBuf = %s\n", (const CHAR*)pbBuf);
	
	if (0 == xmlDoc.Parse((const CHAR*)pbBuf))
	{
		PRINT("Err: failed to parse cmd xml\n");
		return E_FAIL;
	}
	
	TiXmlElement * pEleRoot = NULL; 
	
	pEleRoot = xmlDoc.RootElement();	
	for (TiXmlElement *Ele = pEleRoot->FirstChildElement(); Ele; Ele = Ele->NextSiblingElement())
	{
		if (NULL != Ele->GetText() )
		{
			if (0 == swpa_strcmp(Ele->GetText(), "ResetDevice"))
			{				
				PRINT("Info: Got ResetDevice cmd!\n");
				const CHAR * szExitCode = Ele->Attribute("Value");
				INT iExitCode = (NULL == szExitCode) ? 0 : swpa_atoi(szExitCode);
				
				if (FAILED(OnResetDevice(iExitCode)))
				{
					PRINT("Err: failed to Process OptResetMode cmd\n");
					return E_FAIL;
				}
			}
			else if (0 == swpa_strcmp(Ele->GetText(), "OptResetMode"))
			{
				if (FAILED(OnOptResetMode(ppvOutBuf, pdwOutSize)))
				{
					PRINT("Err: failed to Process OptResetMode cmd\n");
					return E_FAIL;
				}
			}
			else if (0 == swpa_strcmp(Ele->GetText(), "RestoreFactorySetting"))
			{
				if (FAILED(OnRestoreFactorySetting(ppvOutBuf, pdwOutSize)))
				{
					PRINT("Err: failed to Process RestoreFactorySetting cmd\n");
					return E_FAIL;
				}
			}
			else if (0 == swpa_strcmp(Ele->GetText(), "UnlockFactorySetting"))
			{
				if (FAILED(OnUnlockFactorySetting(ppvOutBuf, pdwOutSize)))
				{
					PRINT("Err: failed to Process UnlockFactorySetting cmd\n");
					return E_FAIL;
				}
			}
#ifdef ENABLE_AUTOTEST
			else if (0 == swpa_strcmp(Ele->GetText(), "WriteSN"))
			{
				if (NULL == Ele->Attribute("SN"))
				{
					PRINT("Err: no SN info\n");
					return E_INVALIDARG;
				}
				
				if (FAILED(OnWriteSN(Ele->Attribute("SN"), ppvOutBuf, pdwOutSize)))
				{
					PRINT("Err: failed to Process WriteSN cmd\n");
					return E_FAIL;
				}
			}
			else if (0 == swpa_strcmp(Ele->GetText(), "ProgramEncryptionChip"))
			{
				PRINT("Info: got ProgramEncryptionChip Cmd\n");
				
				const CHAR* szServerIp = (CHAR*)Ele->Attribute("ServerIp");
				if (NULL == szServerIp)
				{
					PRINT("Err: no szSendToIp info, use default save path instead.\n");
					return E_FAIL;
				}

				WORD wServerPort = 0;
				if (NULL == Ele->Attribute("ServerPort"))
				{
					PRINT("Err: no SendToPort info, use default save path instead.\n");
					//wServerPort = 8989;//todo: this is the default port???
					return E_FAIL;
				}
				wServerPort = (WORD)swpa_atoi(Ele->Attribute("ServerPort"));

				if (FAILED(ProcessWriteSecCodeCmd(szServerIp, wServerPort)))
				{
					PRINT("Err: failed to process ProgramEncryptionChip cmd.\n");
					return E_FAIL;
					
				}
			}
			
			else if (0 == swpa_strcmp(Ele->GetText(), "StartAutotest")
				|| 0 == swpa_strcmp(Ele->GetText(), "GetLatestReport")
				|| 0 == swpa_strcmp(Ele->GetText(), "GetAutotestProgress")
				)
			{
				if (!m_fAutotestStarted)
				{	
					HeartBeat();
					if (FAILED(m_cAutotest.Initialize()))
					{
						PRINT("Err: Failed to start Autotest\n");
						return E_FAIL;
					}
					
					m_fAutotestStarted = TRUE;
				}
				HeartBeat();
				return m_cAutotest.ReceiveCmd(CAMERA_XML_EXT_CMD, pbBuf, dwSize, ppvOutBuf, pdwOutSize);
			
			}
#endif			
			else 
			{
				PRINT("Err: got invalid cmd: %s, ignore it\n", Ele->GetText());
				return E_FAIL;
			}
				
			
		}
		
	}
	
	return hr;
}




HRESULT CSWUpgradeApp::UpgradeImage()
{
	if (NULL == m_pbUpgradeImage || 0 == m_dwImageLen)
	{
		PRINT("Err: NULL == m_pbUpgradeImage || 0 == m_dwImageLen\n");
		return E_INVALIDARG;
	}


	if (NULL != m_pUpgradeThread)
	{
		if (!m_pUpgradeThread->IsValid())
		{
			return E_ACCESSDENIED;
		}
		
		return m_pUpgradeThread->Start(OnUpgradeImageProxy, this);
	}
	else
	{
		return E_NOTIMPL;
	}
}




HRESULT CSWUpgradeApp::GetUpgradeImageStatus(PVOID* pvBuf, DWORD* pdwSize)
{
	if (NULL == pvBuf || NULL == pdwSize)
	{
		PRINT("Err: NULL == pvBuf || NULL == pdwSize\n");
		return E_FAIL;
	}

	TiXmlDocument XmlOutputDoc;
	TiXmlDeclaration *pDeclaration = NULL;
	TiXmlElement *pRootEle = NULL;
	TiXmlElement *pStatusEle = NULL;
	
	pDeclaration = new TiXmlDeclaration("1.0","GB2312","yes");
	if (NULL == pDeclaration)
	{
		PRINT("Err: no memory for pDeclaration\n");
		return E_OUTOFMEMORY;
	}
	XmlOutputDoc.LinkEndChild(pDeclaration);

	pRootEle = new TiXmlElement("UpgradeInfo");
	if (NULL == pRootEle)
	{	
		PRINT("Err: no memory for pRootEle\n");
		return E_OUTOFMEMORY;
	}
	XmlOutputDoc.LinkEndChild(pRootEle);
	
	pRootEle->SetAttribute(HVXML_VER, HV_XML_CMD_VERSION_NO);

	pStatusEle = new TiXmlElement("Status");
	if (NULL == pStatusEle)
	{	
		PRINT("Err: no memory for pStatusEle\n");
		return E_OUTOFMEMORY;
	}
	pRootEle->LinkEndChild(pStatusEle);

	CHAR szText[32] = {0};
	if (0 > m_iUpgradeProgress)
	{
		swpa_strncpy(szText, "NotStarted", sizeof(szText)-1);
	}
	else if (100 <= m_iUpgradeProgress)
	{
		swpa_strncpy(szText, "Finished", sizeof(szText)-1);
	}
	else
	{
		swpa_strncpy(szText, "Upgrading", sizeof(szText)-1);
		pStatusEle->SetAttribute("Progress", m_iUpgradeProgress);
	}
		

	TiXmlText* pStatusEleText = new TiXmlText(szText);
	if (NULL == pStatusEleText)
	{
		PRINT("Err: no memory for pStatusEleText\n");
		return E_OUTOFMEMORY;
	}
	pStatusEle->LinkEndChild(pStatusEleText);


	TiXmlPrinter XmlPrinter;
	XmlOutputDoc.Accept(&XmlPrinter);

	*pvBuf = swpa_mem_alloc(XmlPrinter.Size() + 1);
	if (NULL == *pvBuf)
	{
		PRINT("Err: no memory for *pvBuf\n");
		return E_OUTOFMEMORY;
	}
	swpa_memset(*pvBuf, 0, XmlPrinter.Size() + 1);
	swpa_strncpy((CHAR*)*pvBuf, XmlPrinter.CStr(), XmlPrinter.Size());

	*pdwSize = XmlPrinter.Size() + 1;

	return S_OK;

}



HRESULT CSWUpgradeApp::WriteUpgradePacket(const CHAR* pbData, const INT iLen)
{
	if ( NULL == pbData || iLen <= 0 )
	{
		PRINT("Err: NULL == pbData || iLen <= 0\n");
		return E_FAIL;
	}
	
	BYTE* pbUpgradePacket = (BYTE*)pbData;
	IMAGE_PACKAGE_HEADER cHeader;

	m_iUpgradeProgress = 0;

	if (FAILED(DecryptPackage(pbUpgradePacket)))
	{
		PRINT("Err: failed to decrypt the package\n");
		m_iUpgradeProgress = -1; 
		return E_FAIL; 
	}	

	//提取镜像头
	swpa_memcpy(&cHeader, pbUpgradePacket, sizeof(cHeader));
	cHeader.ih_magic = MyNetToHostLong(cHeader.ih_magic);
	cHeader.ih_hcrc = MyNetToHostLong(cHeader.ih_hcrc);
	//cHeader.ih_time = MyNetToHostLong(cHeader.ih_time);
	cHeader.ih_size = MyNetToHostLong(cHeader.ih_size);
	//cHeader.ih_load = MyNetToHostLong(cHeader.ih_load);
	//cHeader.ih_ep = MyNetToHostLong(cHeader.ih_ep);
	cHeader.ih_dcrc = MyNetToHostLong(cHeader.ih_dcrc);
	pbUpgradePacket += sizeof(cHeader);
	
	
	if ( cHeader.ih_type != IH_TYPE_MULTI )
	{
		PRINT("Err: cHeader.ih_type != IH_TYPE_MULTI\n");
		m_iUpgradeProgress = -1; 
		return E_FAIL; // 不被支持的升级包
	}
	
	PRINT("Info: ih_name = [%s] [%d] type:[%d]\n", cHeader.ih_name, cHeader.ih_size, cHeader.ih_type);

	if ( cHeader.ih_dcrc != CSWUtils::CalcCrc32(0, (BYTE*)pbUpgradePacket, cHeader.ih_size) ) //确保整个升级包镜像数据的正确性
	{
		PRINT("Err: CRC check failed!!\n");
		m_iUpgradeProgress = -1; 
		return E_FAIL; //CRC异常！
	}
	
	INT rgiFileSize[16];
	DWORD dwCount = 0;
	DWORD dwTotalSize = 0;
	for (dwCount = 0; dwCount < 16; ++dwCount )
	{
		swpa_memcpy(&(rgiFileSize[dwCount]), pbUpgradePacket, sizeof(INT));
		rgiFileSize[dwCount] = MyNetToHostLong(rgiFileSize[dwCount]);
		pbUpgradePacket += sizeof(INT);
		
		PRINT("Info: fileSize = [%d]\n", rgiFileSize[dwCount]);
		if ( 0 == rgiFileSize[dwCount] )
		{
			break;
		}
		dwTotalSize += rgiFileSize[dwCount];
	}

	DWORD dwUpgradedSize = 0;
	for (INT n = 0; n < dwCount; ++n )
	{
		//提取镜像头
		IMAGE_PACKAGE_HEADER sHeaderForWrite;
		swpa_memcpy(&sHeaderForWrite, pbUpgradePacket, sizeof(sHeaderForWrite));
		swpa_memcpy(&cHeader, pbUpgradePacket, sizeof(cHeader));
		DWORD dwOriginalDataCRC = cHeader.ih_dcrc;
		//cHeader.ih_magic = MyNetToHostLong(cHeader.ih_magic);
		//cHeader.ih_hcrc = MyNetToHostLong(cHeader.ih_hcrc);
		//cHeader.ih_time = MyNetToHostLong(cHeader.ih_time);
		cHeader.ih_size = MyNetToHostLong(cHeader.ih_size);
		//cHeader.ih_load = MyNetToHostLong(cHeader.ih_load);
		//cHeader.ih_ep = MyNetToHostLong(cHeader.ih_ep);
		cHeader.ih_dcrc = MyNetToHostLong(cHeader.ih_dcrc);
		pbUpgradePacket += sizeof(cHeader);
		
		PRINT("Info: ih_name = [%s] [%d] type:[%d]\n", cHeader.ih_name, cHeader.ih_size, cHeader.ih_type);		
		
		if ( cHeader.ih_dcrc != CSWUtils::CalcCrc32(0, (BYTE*)pbUpgradePacket, cHeader.ih_size) ) //确保镜像部件数据的正确性
		{
			PRINT("Err: %s CRC Check failed\n", cHeader.ih_name);
			return E_FAIL; 
		}

		if ( NeedUpgrade((CHAR*)cHeader.ih_name, dwOriginalDataCRC/*cHeader.ih_dcrc*/) ) // 判断该文件是否需要升级
		{	
			
			if ( FAILED( WritePackage(&sHeaderForWrite, pbUpgradePacket, cHeader.ih_size) )) // 将该文件写入Flash中对应的位置
			{
				PRINT("Err: failed to upgrade %s\n", cHeader.ih_name );
				m_iUpgradeProgress = -1; //
				return E_FAIL; //写Flash异常！
			}
		}

		dwUpgradedSize += cHeader.ih_size;

		m_iUpgradeProgress = dwUpgradedSize * 100/ dwTotalSize;
		
		pbUpgradePacket += cHeader.ih_size;
		//镜像部件数据要求4字节对齐
		if ( (cHeader.ih_size%4) != 0 )
		{
			pbUpgradePacket += (4 - (cHeader.ih_size%4));
		}
	}

	m_iUpgradeProgress = 100;
	
	PRINT("Info: Write Package -- OK!\n");
	
	return S_OK; // 升级成功

}



BOOL CSWUpgradeApp::NeedUpgrade(const CHAR* szName, const DWORD dwNowCrc)
{

	if ( swpa_strcmp(szName, "kernel") == 0 )
	{
		CSWFile cFile;
		DWORD dwReadLen = 0;
		IMAGE_PACKAGE_HEADER cHeader;
		if (FAILED(cFile.Open("FLASH/0/KERNEL", "r"))
			|| FAILED(cFile.Read(&cHeader, sizeof(cHeader), &dwReadLen))
			|| sizeof(cHeader) != dwReadLen)
		{
			PRINT("Err: Failed to read DM6467 KERNEL header!\n");
			return TRUE;
		}

		PRINT("Info: cHeader.ih_dcrc = %x, dwNowCrc = %x", cHeader.ih_dcrc, dwNowCrc);
		
		if (cHeader.ih_dcrc != dwNowCrc)
		{
			PRINT("Info: DM6467 KERNEL needs upgrade!\n");
			return TRUE;
		}
		else
		{
			return FALSE;
		}

	}
	else if ( swpa_strcmp(szName, "rootfs") == 0 )
	{		
		CSWFile cFile;
		DWORD dwReadLen = 0;
		IMAGE_PACKAGE_HEADER cHeader;
		if (FAILED(cFile.Open("FLASH/0/ROOTFS", "r"))
			|| FAILED(cFile.Read(&cHeader, sizeof(cHeader), &dwReadLen))
			|| sizeof(cHeader) != dwReadLen)
		{
			PRINT("Err: Failed to read DM6467 ROOTFS header!\n");
			return TRUE;
		}

		PRINT("Info: cHeader.ih_dcrc = %x, dwNowCrc = %x", cHeader.ih_dcrc, dwNowCrc);
		
		if (cHeader.ih_dcrc != dwNowCrc)
		{
			PRINT("Info: DM6467 ROOTFS needs upgrade!\n");
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		
	}
	else if ( swpa_strcmp(szName, "app") == 0 )
	{	
		CSWFile cFile;
		DWORD dwReadLen = 0;
		IMAGE_PACKAGE_HEADER cHeader;
		if (FAILED(cFile.Open("FLASH/0/ARM_APP", "r"))
			|| FAILED(cFile.Read(&cHeader, sizeof(cHeader), &dwReadLen))
			|| sizeof(cHeader) != dwReadLen)
		{
			PRINT("Err: Failed to read ARM_APP header!\n");
			return TRUE;
		}

		PRINT("Info: cHeader.ih_dcrc = %x, dwNowCrc = %x", cHeader.ih_dcrc, dwNowCrc);
		
		if (cHeader.ih_dcrc != dwNowCrc)
		{
			PRINT("Info: ARM_APP needs upgrade!\n");
			return TRUE;
		}
		else
		{
			return FALSE;
		}

	}
	else if ( swpa_strcmp(szName, "dsp") == 0 )
	{
		CSWFile cFile;
		DWORD dwReadLen = 0;
		IMAGE_PACKAGE_HEADER cHeader;
		if (FAILED(cFile.Open("FLASH/0/DSP_APP", "r"))
			|| FAILED(cFile.Read(&cHeader, sizeof(cHeader), &dwReadLen))
			|| sizeof(cHeader) != dwReadLen)
		{
			PRINT("Err: Failed to read DSP_APP header!\n");
			return TRUE;
		}
		
		PRINT("Info: cHeader.ih_dcrc = %x, dwNowCrc = %x", cHeader.ih_dcrc, dwNowCrc);
		
		if (cHeader.ih_dcrc != dwNowCrc)
		{
			PRINT("Info: DSP_APP needs upgrade!\n");
			return TRUE;
		}
		else
		{
			return FALSE;
		}

	}
	else if ( swpa_strcmp(szName, "fpga") == 0 )
	{	
		CSWFile cFile;
		DWORD dwReadLen = 0;
		IMAGE_PACKAGE_HEADER cHeader;
		if (FAILED(cFile.Open("FLASH/0/FFGA_ROM", "r"))
			|| FAILED(cFile.Read(&cHeader, sizeof(cHeader), &dwReadLen))
			|| sizeof(cHeader) != dwReadLen)
		{
			PRINT("Err: Failed to read FFGA_ROM header!\n");
			return TRUE;
		}

		PRINT("Info: cHeader.ih_dcrc = %x, dwNowCrc = %x", cHeader.ih_dcrc, dwNowCrc);
		
		if (cHeader.ih_dcrc != dwNowCrc)
		{
			PRINT("Info: FFGA_ROM needs upgrade!\n");
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		
	}
	else if ( swpa_strcmp(szName, "kernel_368") == 0 )//todo: "dm368_kernel" is right??
	{	
		CSWFile cFile;
		DWORD dwReadLen = 0;
		IMAGE_PACKAGE_HEADER cHeader;
		if (FAILED(cFile.Open("FLASH/1/KERNEL", "r"))
			|| FAILED(cFile.Read(&cHeader, sizeof(cHeader), &dwReadLen))
			|| sizeof(cHeader) != dwReadLen)
		{
			PRINT("Err: Failed to read DM368 KERNEL header!\n");
			return TRUE;
		}

		PRINT("Info: cHeader.ih_dcrc = %x, dwNowCrc = %x", cHeader.ih_dcrc, dwNowCrc);
		
		if (cHeader.ih_dcrc != dwNowCrc)
		{
			PRINT("Info: DM368 KERNEL needs upgrade!\n");
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		
	}
	else if ( swpa_strcmp(szName, "rootfs_368") == 0 )//todo: "dm368_kernel" is right??
	{	
		CSWFile cFile;
		DWORD dwReadLen = 0;
		IMAGE_PACKAGE_HEADER cHeader;
		if (FAILED(cFile.Open("FLASH/1/ROOTFS", "r"))
			|| FAILED(cFile.Read(&cHeader, sizeof(cHeader), &dwReadLen))
			|| sizeof(cHeader) != dwReadLen)
		{
			PRINT("Err: Failed to read DM368 ROOTFS header!\n");
			return TRUE;
		}

		PRINT("Info: cHeader.ih_dcrc = %x, dwNowCrc = %x", cHeader.ih_dcrc, dwNowCrc);
		
		if (cHeader.ih_dcrc != dwNowCrc)
		{
			PRINT("Info: DM368 ROOTFS needs upgrade!\n");
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		
	}
	else if ( swpa_strcmp(szName, "kernel_bak") == 0 )
	{	
		CSWFile cFile;
		DWORD dwReadLen = 0;
		IMAGE_PACKAGE_HEADER cHeader;
		swpa_memset(&cHeader, 0, sizeof(cHeader));
		if (FAILED(cFile.Open("FLASH/0/KERNEL_BAK", "r"))
			|| FAILED(cFile.Read(&cHeader, sizeof(cHeader), &dwReadLen))
			|| sizeof(cHeader) != dwReadLen)
		{
			PRINT("Err: Failed to read DM6467 KERNEL_BAK header!\n");
			return TRUE;
		}

		PRINT("Info: cHeader.ih_dcrc = %x, dwNowCrc = %x", cHeader.ih_dcrc, dwNowCrc);
		
		if (cHeader.ih_dcrc != dwNowCrc)
		{
			PRINT("Info: DM6467 KERNEL_BAK needs upgrade!\n");
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		
	}
	else if ( swpa_strcmp(szName, "rootfs_bak") == 0 )
	{	
		CSWFile cFile;
		DWORD dwReadLen = 0;
		IMAGE_PACKAGE_HEADER cHeader;
		if (FAILED(cFile.Open("FLASH/0/ROOTFS_BAK", "r"))
			|| FAILED(cFile.Read(&cHeader, sizeof(cHeader), &dwReadLen))
			|| sizeof(cHeader) != dwReadLen)
		{
			PRINT("Err: Failed to read DM6467 ROOTFS_BAK header!\n");
			return TRUE;
		}

		PRINT("Info: cHeader.ih_dcrc = %x, dwNowCrc = %x", cHeader.ih_dcrc, dwNowCrc);
		
		if (cHeader.ih_dcrc != dwNowCrc)
		{
			PRINT("Info: DM6467 ROOTFS_BAK needs upgrade!\n");
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		
	}
	else if ( swpa_strcmp(szName, "uboot_master") == 0 )
	{	
		return TRUE; //NOTE: return TRUE directly if the Image Package has uboot data
	}
	else if ( swpa_strcmp(szName, "ubl_master") == 0 )
	{	
		return TRUE; //NOTE: return TRUE directly if the Image Package has ubl data
	}
	else if ( swpa_strcmp(szName, "uBoot_368") == 0 )
	{	
		return TRUE; //NOTE: return TRUE directly if the Image Package has uboot data
	}
	else if ( swpa_strcmp(szName, "ubl_368") == 0 )
	{	
		return TRUE; //NOTE: return TRUE directly if the Image Package has ubl data
	}
	else
	{
		//todo:其它部件
		return FALSE;
	}

	return FALSE;
	
}


HRESULT CSWUpgradeApp::WriteFile(const CHAR* szFileName, const PVOID pvHeader, const PBYTE pbData, const DWORD dwSize)
{
	if (NULL == szFileName || NULL == pbData || 0 == dwSize)
	{
		PRINT("Err: NULL == szFileName || NULL == pbData || 0 == dwSize\n");
		return E_INVALIDARG;
	}	
	
	
	CSWFile cFile;
	DWORD dwWrittenLen = 0;
	DWORD dwNewPos = 0;

	if (FAILED(cFile.Open(szFileName, "w+")))
	{
		PRINT("Err: failed to open %s\n", szFileName);
		return E_FAIL;
	}
	
	if (NULL == pvHeader)
	{
		if (FAILED(cFile.Seek(0, SWPA_SEEK_SET, &dwNewPos))
			|| 0 != dwNewPos
			|| FAILED(cFile.Write(pbData, dwSize, &dwWrittenLen))
			|| dwWrittenLen != dwSize)
		{
			PRINT("Err: Failed to write %s!\n", szFileName);
			return E_FAIL;
		}
	}
	else
	{
		IMAGE_PACKAGE_HEADER cNewHeader;
		swpa_memcpy(&cNewHeader, pvHeader, sizeof(cNewHeader));

		IMAGE_PACKAGE_HEADER cNullHeader;
		swpa_memset(&cNullHeader, 0, sizeof(cNullHeader));

		if (FAILED(cFile.Write((PVOID)&cNullHeader, sizeof(cNullHeader), &dwWrittenLen)) 	//1. clear the old header
			|| sizeof(cNullHeader) != dwWrittenLen
			|| FAILED(cFile.Write(pbData, dwSize, &dwWrittenLen))							//2. write data
			|| dwWrittenLen != dwSize			
			|| FAILED(cFile.Seek(0, SWPA_SEEK_SET, &dwNewPos))
			|| 0 != dwNewPos
			|| FAILED(cFile.Write((PVOID)&cNewHeader, sizeof(cNewHeader), &dwWrittenLen))			//3. write new header
			|| sizeof(cNewHeader) != dwWrittenLen
			)
		{
			PRINT("Err: Failed to write %s!\n", szFileName);
			return E_FAIL;
		}		
	}

	PRINT("Info: Write %s -- OK!\n", szFileName);
	
	return cFile.Close();		
}




HRESULT CSWUpgradeApp::WritePackage(const PVOID pvHeader, const PBYTE pbData, const DWORD dwSize)
{	
	IMAGE_PACKAGE_HEADER cHeader;
	swpa_memcpy(&cHeader, pvHeader, sizeof(cHeader));
	
	if ( swpa_strcmp((const char*)cHeader.ih_name, "kernel") == 0 )
	{
		PRINT("Info: Upgrading DM6467 KERNEL...!\n");
		
		if (FAILED(WriteFile("FLASH/0/KERNEL", pvHeader, pbData, dwSize)))
		{
			PRINT("Err: Upgrade DM6467 KERNEL...NG!\n");
			return E_FAIL;
		}

		PRINT("Info: Upgrade DM6467 KERNEL...OK!\n");
		return S_OK;
	}
	else if ( swpa_strcmp((const char*)cHeader.ih_name, "rootfs") == 0 )
	{	
		PRINT("Info: Upgrading DM6467 ROOTFS...!\n");

		if (FAILED(WriteFile("FLASH/0/ROOTFS", pvHeader, pbData, dwSize)))
		{
			PRINT("Err: Upgrade DM6467 ROOTFS...NG!\n");
			return E_FAIL;
		}

		PRINT("Info: Upgrade DM6467 ROOTFS...OK!\n");		
		return S_OK;
		
	}
	else if ( swpa_strcmp((const char*)cHeader.ih_name, "app") == 0 )
	{	
		PRINT("Info: Upgrading DM6467 ARM_APP...!\n");

		if (FAILED(WriteFile("FLASH/0/ARM_APP", pvHeader, pbData, dwSize)))
		{
			PRINT("Err: Upgrade DM6467 ARM_APP...NG!\n");
			return E_FAIL;
		}

		PRINT("Info: Upgrade DM6467 ARM_APP...OK!\n");		
		return S_OK;

	}
	else if ( swpa_strcmp((const char*)cHeader.ih_name, "dsp") == 0 )
	{
		PRINT("Info: Upgrading DSP_APP...!\n");

		if (FAILED(WriteFile("FLASH/0/DSP_APP", pvHeader, pbData, dwSize)))
		{
			PRINT("Err: Upgrade DSP_APP...NG!\n");
			return E_FAIL;
		}

		PRINT("Info: Upgrade DSP_APP...OK!\n");		
		return S_OK;

	}
	else if ( swpa_strcmp((const char*)cHeader.ih_name, "fpga") == 0 )
	{	
		PRINT("Info: Upgrading FPGA_ROM...!\n");

		if (FAILED(WriteFile("FLASH/0/FFGA_ROM", pvHeader, pbData, dwSize)))
		{
			PRINT("Err: Upgrade FFGA_ROM...NG!\n");
			return E_FAIL;
		}
		
		PRINT("Info: Upgrade FFGA_ROM...OK!\n");
		return S_OK;
	}
	else if ( swpa_strcmp((const char*)cHeader.ih_name, "kernel_368") == 0 )
	{	
		PRINT("Info: Upgrading DM368 KERNEL...!\n");

		if (FAILED(WriteFile("FLASH/1/KERNEL", pvHeader, pbData, dwSize)))
		{
			PRINT("Err: Upgrade DM368 KERNEL...NG!\n");
			return E_FAIL;
		}

		PRINT("Info: Upgrade DM368 KERNEL...OK!\n");		
		return S_OK;
	}
	else if ( swpa_strcmp((const char*)cHeader.ih_name, "rootfs_368") == 0 )
	{	
		PRINT("Info: Upgrading DM368 ROOTFS...!\n");

		if (FAILED(WriteFile("FLASH/1/ROOTFS", pvHeader, pbData, dwSize)))
		{
			PRINT("Err: Upgrade DM368 ROOTFS...NG!\n");
			return E_FAIL;
		}

		PRINT("Info: Upgrade DM368 ROOTFS...OK!\n");		
		return S_OK;
	}
	else if ( swpa_strcmp((const char*)cHeader.ih_name, "rootfs_bak") == 0 )
	{	
		PRINT("Info: Upgrading DM6467 ROOTFS_BAK...!\n");

		if (FAILED(WriteFile("FLASH/0/ROOTFS_BAK", pvHeader, pbData, dwSize)))
		{
			PRINT("Err: Upgrade DM6467 ROOTFS_BAK...NG!\n");
			return E_FAIL;
		}

		PRINT("Info: Upgrade DM6467 ROOTFS_BAK...OK!\n");		
		return S_OK;
	}
	else if ( swpa_strcmp((const char*)cHeader.ih_name, "kernel_bak") == 0 )
	{	
		PRINT("Info: Upgrading DM6467 KERNEL_BAK...!\n");

		if (FAILED(WriteFile("FLASH/0/KERNEL_BAK", pvHeader, pbData, dwSize)))
		{
			PRINT("Err: Upgrade DM6467 KERNEL_BAK...NG!\n");
			return E_FAIL;
		}
		
		PRINT("Info: Upgrade DM6467 KERNEL_BAK...OK!\n");		
		return S_OK;
	}
	else if ( swpa_strcmp((const char*)cHeader.ih_name, "uboot_master") == 0 )
	{	
		PRINT("Info: Upgrading DM6467 UBOOT...!\n");

		if (FAILED(WriteFile("FLASH/0/UBOOT", NULL/*no header to write*/, pbData, dwSize)))
		{
			PRINT("Err: Upgrade DM6467 UBOOT...NG!\n");
			return E_FAIL;
		}

		PRINT("Info: Upgrade DM6467 UBOOT...OK!\n");		
		return S_OK;
	}
	else if ( swpa_strcmp((const char*)cHeader.ih_name, "ubl_master") == 0 )
	{	
		PRINT("Info: Upgrading DM6467 UBL...!\n");

		/*CSWFile cFile;
		DWORD dwWrittenLen = 0;
		if (FAILED(cFile.Open("EEPROM/1/UBL", "w"))
			//NOTE: cannot write the header : || FAILED(cFile.Write((PVOID)&cHeader, sizeof(cHeader), &dwWrittenLen))
			//NOTE: cannot write the header : || dwWrittenLen != sizeof(cHeader)
			|| FAILED(cFile.Write(pbData+sizeof(cHeader), dwSize, &dwWrittenLen))
			|| dwWrittenLen != dwSize)
		{
			PRINT("Err: Failed to upgrade DM6467 UBL!\n");
			return E_FAIL;
		}*/

		UINT uiLen = dwSize;
		INT iRet = drv_eeprom_write(DEVID_SPI_EEPROM, 0x0, pbData, &uiLen);
		if (0 != iRet 
			|| dwSize != uiLen)
		{
			PRINT("Err: Failed to upgrade DM6467 UBL!\n");
			return E_FAIL;
		}
		
		PRINT("Info: Upgrade DM6467 UBL...OK!\n");
		
		return S_OK;
	}
	else if ( swpa_strcmp((const char*)cHeader.ih_name, "uBoot_368") == 0 )
	{	
		PRINT("Info: Upgrading DM368 UBOOT...!\n");

		if (FAILED(WriteFile("FLASH/1/UBOOT", NULL/*no header to write*/, pbData, dwSize)))
		{
			PRINT("Err: Upgrade DM368 UBOOT...NG!\n");
			return E_FAIL;
		}
		
		PRINT("Info: Upgrade DM368 UBOOT...OK!\n");		
		return S_OK;
	}
	else if ( swpa_strcmp((const char*)cHeader.ih_name, "ubl_368") == 0 )
	{	
		PRINT("Info: Upgrading DM368 UBL...!\n");

		if (FAILED(WriteFile("FLASH/1/UBL", NULL/*no header to write*/, pbData, dwSize)))
		{
			PRINT("Err: Upgrade DM368 UBL...NG!\n");
			return E_FAIL;
		}
		
		PRINT("Info: Upgrade DM368 UBL...OK!\n");		
		return S_OK;
	}
	else
	{
		//todo:其它部件
		PRINT("Err: Upgrade %s -- Not implemented!\n", cHeader.ih_name);
		return E_NOTIMPL;
	}

	return S_OK;
}


DWORD CSWUpgradeApp::MyHostToNetLong(const DWORD dwVal)
{
	return MyNetToHostLong(dwVal);
}



DWORD CSWUpgradeApp::MyNetToHostLong(const DWORD dwVal)
{
	BYTE pbTemp[4] = {0};
	BYTE bTemp = 0;
	DWORD dwRetVal = 0;

	swpa_memcpy(pbTemp, &dwVal, 4);
	bTemp = pbTemp[0];
	pbTemp[0] = pbTemp[3];
	pbTemp[3] = bTemp;

	bTemp = pbTemp[1];
	pbTemp[1] = pbTemp[2];
	pbTemp[2] = bTemp;

	swpa_memcpy(&dwRetVal, pbTemp, 4);

	return dwRetVal;
}


HRESULT CSWUpgradeApp::DecryptPackage(PBYTE& pbUpgradePacket)
{
	
	IMAGE_PACKAGE_HEADER cHeader;
	
	//提取镜像头
	swpa_memcpy(&cHeader, pbUpgradePacket, sizeof(cHeader));
	cHeader.ih_magic = MyNetToHostLong(cHeader.ih_magic);
	cHeader.ih_hcrc = MyNetToHostLong(cHeader.ih_hcrc);
	//cHeader.ih_time = MyNetToHostLong(cHeader.ih_time);
	cHeader.ih_size = MyNetToHostLong(cHeader.ih_size);
	//cHeader.ih_load = MyNetToHostLong(cHeader.ih_load);
	//cHeader.ih_ep = MyNetToHostLong(cHeader.ih_ep);
	cHeader.ih_dcrc = MyNetToHostLong(cHeader.ih_dcrc);
	
	if (0 == swpa_strcmp("upgrade", (const CHAR*)cHeader.ih_name))//encrypted
	{
		pbUpgradePacket += sizeof(cHeader);
			
		PRINT("Info: ih_name = [%s] [%d] type:[%d]\n", cHeader.ih_name, cHeader.ih_size, cHeader.ih_type);
		
		CHAR szIp[32] = {0};
		CHAR szMask[32] = {0};
		CHAR szGateway[32] = {0};
		CHAR szMAC[32] = {0};
		DWORD dwMAC[6] = {0};
		BYTE bMac[6] = {0};
		
		if (SWPAR_OK != swpa_tcpip_getinfo("eth0", 
			szIp, sizeof(szIp)-1,
			szMask, sizeof(szMask)-1,
			szGateway, sizeof(szGateway)-1,
			szMAC, sizeof(szMAC)-1))
		{
			PRINT("Err: failed to get Mac Addr\n");
			m_iUpgradeProgress = -1; //
			return E_FAIL; 
		}
		else
		{
			swpa_sscanf(szMAC, "%x:%x:%x:%x:%x:%x", &dwMAC[0], &dwMAC[1], &dwMAC[2], &dwMAC[3], &dwMAC[4], &dwMAC[5]);
			for (DWORD i=0; i<sizeof(bMac); i++)
			{
				bMac[i] = dwMAC[i] & 0xFF;
				//PRINT("Info: bMACVal[i] = %#x\n", bMac[i]);
			}
		}

		INT iEncryptedSize = 0;
		swpa_memcpy(&iEncryptedSize, pbUpgradePacket, sizeof(INT));
		iEncryptedSize = MyNetToHostLong(iEncryptedSize);
		pbUpgradePacket += sizeof(iEncryptedSize);

		INT iFlag = 0;
		swpa_memcpy(&iFlag, pbUpgradePacket, sizeof(INT));
		iFlag = MyNetToHostLong(iFlag);
		pbUpgradePacket += sizeof(iFlag);

		if (0 != iFlag)
		{
			//只支持一个加密包，因此这4个字节需为0
			PRINT("Err: 0 != iFlag\n");
			return E_FAIL; 
		}

		ULONG ulPos = 0;
		DecryptPackageByMAC(pbUpgradePacket, iEncryptedSize, bMac, &ulPos);		

	}

	return S_OK;
}


// 解密
VOID CSWUpgradeApp::DecryptPackageByMAC(BYTE* pbData, INT iLen, BYTE* pbKeyMAC, ULONG* ulPos)
{
#define M_NUM 0x75DA63BF
#define MAKE_KEY(m1, m2, m3) ((m1*m2+m3) + (~m1)*(m1<<2)*((~m2)<<3) + m2 + (~m3) + (m3>>1))
#define MAKE_XOR_CODE(m1, m2, m3) (m1*(M_NUM&m2)*m3)
#define ENCODE(m1, m2, m3) (m1 ^ MAKE_XOR_CODE(m2,m3,M_NUM))

    BYTE k0 = MAKE_KEY(pbKeyMAC[3], pbKeyMAC[4], pbKeyMAC[5]);
    while ( iLen-- )
    {
        *pbData++ = ENCODE(*pbData, k0, (BYTE)*ulPos);
        (*ulPos)++;
    }
}




HRESULT CSWUpgradeApp::ProcessWriteSecCodeCmd(const CHAR* szServerIp, const WORD wServerPort)
{
	CSWTCPSocket cSockClient;

	

	BYTE bFuseStatus = 0;
	if (SWPAR_OK != swpa_device_get_fuse_status(&bFuseStatus))
	{
		PRINT("Err: failed to get fuse status.\n");
		return E_FAIL;
	}

	if (0 == bFuseStatus)//fused already, do nothing
	{
		INT iRet = swpa_device_crypt_init();

		BYTE bBuf[32] = {0};
        UINT iLen = 0x07;
        iRet = swpa_device_crypt_read(0x0, bBuf, &iLen);    // nc
		PRINT("Info: NC= %02X %02X %02X %02X %02X %02X %02X\n", bBuf[0], bBuf[1], bBuf[2], bBuf[3], bBuf[4], bBuf[5], bBuf[6]);

       	iLen = 0x10;
        iRet = swpa_device_crypt_read(0x40, bBuf, &iLen); // key
		PRINT("Info: Already fused, do nothing and return OK.\n");
		return E_FAIL;
	}
	
	
	if (FAILED(cSockClient.Create()))
	{
		PRINT("Err: failed to create client socket.\n");
		return E_FAIL;
	}

	cSockClient.SetRecvTimeout(4000);
	cSockClient.SetSendTimeout(4000);
	INT iTryCount = 10;
	while (iTryCount--)
	{
		if (FAILED(cSockClient.Connect(szServerIp, wServerPort)))
		{
			PRINT("Err: failed to Connect to %s:%d, trying another %d times\n", szServerIp, wServerPort, iTryCount);
			swpa_thread_sleep_ms(1000);
		}
		else
		{
			break;
		}
	}

	if (0 > iTryCount)
	{
		PRINT("Err: failed to Connect to %s:%d\n", szServerIp, wServerPort);
		return E_FAIL;
	}

	CHAR szSN[512] = {0};
	DWORD dwLen = sizeof(szSN);
	INT iRet = swpa_device_read_sn(szSN, &dwLen);
	if (SWPAR_OK != iRet)
	{
		PRINT("Err: failed to get device serial number\n");
		return E_FAIL;
	}


	CHAR szIp[32]={0}, szMask[32]={0}, szGateway[32]={0}, szMac[32]={0};
	iRet = swpa_tcpip_getinfo("eth0", szIp, 31, szMask, 31, szGateway, 31, szMac, 31);
	if (SWPAR_OK != iRet)
	{
		PRINT("Err: failed to get MAC\n");
		return E_FAIL;
	}
	DWORD dwThisMAC[6] = {0};
	BYTE bThisMAC[6] = {0};

	swpa_sscanf(szMac, "%x:%x:%x:%x:%x:%x", &dwThisMAC[0], &dwThisMAC[1], &dwThisMAC[2], &dwThisMAC[3], &dwThisMAC[4], &dwThisMAC[5]);
	for (DWORD i=0; i<sizeof(bThisMAC); i++)
	{
		bThisMAC[i] = dwThisMAC[i] & 0xFF;
	}

	// Network command head
	typedef struct _CMD_PROTOCOL_T {
		UINT uiLen;
		UINT uiType;
		UINT uiID;
		UINT uiPass;
	} CMD_PROTOCOL_T;

	struct CMD_INFO {
		CMD_PROTOCOL_T *cp;
		VOID *pvParam;
	};

#define COMMAND_PASSWRD	0x20101213

	CMD_PROTOCOL_T tCmd;
	
	tCmd.uiType = MyHostToNetLong(0x1204);
	tCmd.uiID = MyHostToNetLong(0);
	tCmd.uiLen = MyHostToNetLong(146);
	tCmd.uiPass = MyHostToNetLong(COMMAND_PASSWRD);
	
	BYTE pbBuf[256] = {0};

	swpa_memcpy(pbBuf, &tCmd, sizeof(tCmd));
	swpa_memcpy(pbBuf+16, szSN, 128);
	swpa_memcpy(pbBuf+16+128, bThisMAC, 6);
	if (FAILED(cSockClient.Send(pbBuf, sizeof(pbBuf), NULL)))
	{
		PRINT("Err: failed to send data to %s:%d.\n", szServerIp, wServerPort);
		return E_FAIL;
	}
	
	if (FAILED(cSockClient.Read(pbBuf, sizeof(tCmd)+8+16, NULL)))
	{
		PRINT("Err: failed to read data from %s:%d.\n", szServerIp, wServerPort);
		return E_FAIL;
	}

	swpa_memset(&tCmd, 0, sizeof(tCmd));
	swpa_memcpy(&tCmd, pbBuf, sizeof(tCmd));

	BYTE bDevNC[8]= {0};
	BYTE bDevData[16] = {0};

	if (1 == MyNetToHostLong(tCmd.uiID)
		&& 36 == MyNetToHostLong(tCmd.uiLen)
		&& 0x1204 == MyNetToHostLong(tCmd.uiType)
		&& COMMAND_PASSWRD == MyNetToHostLong(tCmd.uiPass))
	{
		swpa_memcpy(bDevNC, pbBuf + 16, 8);
		swpa_memcpy(bDevData, pbBuf + 24, 16);
	}
	else
	{
		PRINT("Err: Got invalid cmd response from server.\n");
		return E_FAIL;
	}

	if (0 != bDevNC[7])
	{
		PRINT("Err: Got invalid NC from server.\n");
		return E_FAIL;
	}

	PRINT("Info: Got Server NC= %02X %02X %02X %02X %02X %02X %02X\n", bDevNC[0], bDevNC[1], bDevNC[2], bDevNC[3], bDevNC[4], bDevNC[5], bDevNC[6]);
	
	if (SWPAR_OK != swpa_device_init_crypt_config(bDevNC, bDevData))
	{
		PRINT("Err: failed to init_crypt_config.\n");
		return E_FAIL;
	}

	

	if (SWPAR_OK != swpa_device_fuse_write())
	{
		PRINT("Err: failed to fuse.\n");
		return E_FAIL;
	}	

	cSockClient.Close();

	PRINT("Info: Program Sec Chip -- OK.\n");
	
	return S_OK;
}



/**
 *@brief 取得代码的版本号 
 *@return 返回版本号,失败返回-1
 */
INT CSWUpgradeApp::GetVersion(VOID)
{
	//return CSWApplication::GetVersion(); 
#ifdef VERSION
	return VERSION;
#else
	return  -1;
#endif
}




