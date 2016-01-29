#include <sys/stat.h>
#include <unistd.h>
#include "swpa.h"
#include "SWFC.h"
#include "SWLogApplication.h"
#include "SWLogDevice.h"

CSWLogApplication::CSWLogApplication()
{
}

CSWLogApplication::~CSWLogApplication()
{
}

HRESULT CSWLogApplication::InitInstance(const WORD wArgc, const CHAR** szArgv)
{
	HRESULT hr;
	if(SUCCEEDED(hr = CSWApplication::InitInstance(wArgc, szArgv)))
	{
		hr = CSWLogDevice::Initialize();
	}
	return hr;
}

#ifdef USE_LOCALSOCKET


HRESULT CSWLogApplication::ReleaseInstance()
{
	for (INT i=0; i<8; i++)
	{
		m_cSemaLock[i].Pend();
		while (!m_lstLog[i].IsEmpty())
		{
			CHAR * szLog = (CHAR *)m_lstLog[i].RemoveHead();
			swpa_mem_free(szLog);
		}
		m_cSemaLock[i].Post();
	}

	
	CSWLogDevice::Clear();
	return CSWApplication::ReleaseInstance();
}




struct _LogThreadParam
{
	CSWLogApplication * pApp;
	SWPA_SOCKET_T outSock;
	DWORD dwID;
};


HRESULT CSWLogApplication::Run()
{
	HeartBeat();

	for (INT i=0; i < sizeof(m_cLogThreads)/sizeof(m_cLogThreads[0]); i++)
	{
		m_lstLog[i].SetMaxCount(256);
		m_cSema[i].Create(0, 256);
		m_cSemaLock[i].Create(1, 1);
	}

	

	HeartBeat();

	m_cPrintThread.Start(OnPrint, this);
	
	HeartBeat();
	CSWTCPSocket cSockLogServer;
	while (FAILED(cSockLogServer.Create(TRUE)))
	{
		printf("Err: Create log server socket failed. trying again.\n");
	}
	
	HeartBeat();
	while (FAILED(cSockLogServer.Bind("/tmp/log.sock")))
	{
		printf("Err: bind log server socket failed. trying again.\n");
	}

	HeartBeat();
	cSockLogServer.SetRecvTimeout(4000);
	while (FAILED(cSockLogServer.Listen(8)))
	{
		printf("Err: listen log server socket failed. trying again.\n");
	}

	HeartBeat();

	while(!IsExited())
	{
		HeartBeat();
		
		SWPA_SOCKET_T outSock;
		if (SUCCEEDED(cSockLogServer.Accept(outSock)))
		{
			//printf("log server got a connection...\n");

			struct _LogThreadParam* pParam = 
				(struct _LogThreadParam*)swpa_mem_alloc(sizeof(struct _LogThreadParam));
			if (NULL == pParam)
			{
				printf("Err: no mem for log param. exit\n");
				Exit(0);
			}
			pParam->pApp = this;
			pParam->outSock = outSock;
			
			for (INT i=0; i<sizeof(m_cLogThreads)/sizeof(m_cLogThreads[0]); i++)
			{
				if (S_OK != m_cLogThreads[i].IsValid())
				{
					//printf("log server the %d-th sock is available...\n", i);

					m_cLogThreads[i].Stop();
					pParam->dwID = (DWORD)i;
					if (FAILED(m_cLogThreads[i].Start(OnLog, pParam)))
					{	
						printf("Err: failed to start the %d-th log thread...\n", i);
						swpa_mem_free(pParam);
					}
					break;
				}
			}

		}

		Sleep(50);
			
	}
	
	for (INT i=0; i<sizeof(m_cLogThreads)/sizeof(m_cLogThreads[0]); i++)
	{
		m_cLogThreads[i].Stop();
	}

	return S_OK;
}

PVOID CSWLogApplication::OnLog(PVOID pvParam)
{
	struct _LogThreadParam* pParam = (struct _LogThreadParam *)pvParam;
	if (NULL == pParam || NULL == pParam->pApp || 0 == pParam->outSock)
	{	
		printf("Err: invalid args.\n");
		return 0;
	}
	
	CSWLogApplication* pThis = pParam->pApp;
	DWORD dwID = pParam->dwID;
	CSWTCPSocket cSockData;
	cSockData.Attach(pParam->outSock);

	cSockData.SetSendTimeout(1000);
	cSockData.SetRecvTimeout(1000);

	SAFE_MEM_FREE(pvParam);

	const DWORD dwLogMaxLen = SW_LOG_MAX_LEN;//4096 + 64;

	DWORD dwBufSize = 128*1024;
	if (FAILED(cSockData.SetSendBufferSize(dwBufSize)))
	{
		printf("err: set send buffer size to %dKB failed\n", dwBufSize>>10);
	}
	if (FAILED(cSockData.SetRecvBufferSize(dwBufSize)))
	{
		printf("err: set recv buffer size to %dKB failed\n", dwBufSize>>10);
	}
	

	DWORD dwLen = 0;
	DWORD dwFailCount = 0;
	//DWORD dwPrevDebugCount = 0;
	while(!pThis->IsExited())
	{
		if (FAILED(cSockData.Read(&dwLen, sizeof(dwLen))))
		{
			dwFailCount++;
		}
		else if (dwLen > 0 && dwLen <= dwLogMaxLen)
		{
			CHAR *szLog = (CHAR*)swpa_mem_alloc(dwLen);
			if (NULL == szLog)
			{
				printf("Err: no mem for szLog\n");
				break;
			}
			else
			{
				DWORD dwReadLen = 0;

				/*DWORD dwDebugCount = 0;
				if (SUCCEEDED(cSockData.Read(&dwDebugCount, 4)))
				{
					if (dwDebugCount != dwPrevDebugCount+1)
					{
						printf("Err: thread #%d lost log (%d, %d)!!!!\n", dwID, dwPrevDebugCount, dwDebugCount);
					}
					dwPrevDebugCount = dwDebugCount;
				}
				else
				{
					printf("Err: failed to read log count!\n");
					dwFailCount++;
					SAFE_MEM_FREE(szLog);
					break;
				}
				*/
				if (SUCCEEDED(cSockData.Read(szLog, dwLen, &dwReadLen)))
				{
					if (dwReadLen != dwLen)
					{
						printf("err: log server only read %d bytes\n", dwReadLen);
						dwFailCount++;
						SAFE_MEM_FREE(szLog);
						break;
					}
					
					dwFailCount = 0;
					pThis->m_cSemaLock[dwID].Pend();
					pThis->m_lstLog[dwID].AddTail(szLog);
					pThis->m_cSema[dwID].Post();
					pThis->m_cSemaLock[dwID].Post();
				}
				else
				{
					dwFailCount++;
					SAFE_MEM_FREE(szLog);
				}
			}
		}
		else
		{
			printf("Err: log server #%d got invalid loglen %d.\n", dwID, dwLen);
		}

		if (dwFailCount > 10)
		{
			//连续接收日志失败超过10次，则退出线程，以便下次重连恢复
			//printf("Err: log server socket read data failed > 10.\n");
			break;
		}
	}

	cSockData.Close();
	
	return 0;
}


PVOID CSWLogApplication::OnPrint(PVOID pvParam)
{
	CSWLogApplication* pThis = (CSWLogApplication*)pvParam;

	if (NULL == pThis)
	{
		printf("Err: OnPrint() invalid arg.\n");
		return 0;
	}
	
	while (!pThis->IsExited())
	{
		for (INT i=0; i<sizeof(m_cLogThreads)/sizeof(m_cLogThreads[0]); i++)
		{
			if (SUCCEEDED(pThis->m_cSema[i].Pend(1)))
			{
				pThis->m_cSemaLock[i].Pend();
				if (pThis->m_lstLog[i].GetCount() > 200)
				{
					printf("Info: thread #%d log list count = %d\n", i, pThis->m_lstLog[i].GetCount());
				}
				
				CHAR * szLog = (CHAR*)pThis->m_lstLog[i].RemoveHead();
				pThis->m_cSemaLock[i].Post();

				//DWORD dwPrevTick = CSWDateTime::GetSystemTick();
				if (NULL != szLog)
				{
					CSWLogDevice::Log(szLog);
					SAFE_MEM_FREE(szLog);
				}
				//DWORD dwTick = CSWDateTime::GetSystemTick();
				
				//if (dwTick - dwPrevTick > 20)
				//	printf("info: output a log costs %d ms\n", dwTick - dwPrevTick);
			}
		}
	}

	return 0;
}

#else

HRESULT CSWLogApplication::ReleaseInstance()
{
	CSWLogDevice::Clear();
	return CSWApplication::ReleaseInstance();
}


#ifndef USE_TAIL
HRESULT CSWLogApplication::Run()
{
	CSWString strINI = GetCommandString("-i", "/var/log/messages");
	FILE *fp = NULL;
	CHAR* szLog =  (CHAR *)swpa_mem_alloc(1024);
	DWORD dwLen = 0, dwNode;
	while(!IsExited())
	{
		HeartBeat();
		if(NULL == fp && (fp = fopen(strINI, "r")))
		{
			fseek(fp, 0, SEEK_END);
			dwNode = GetINode(strINI);
		}
		if(NULL != fp)
		{
			if(NULL != fgets(szLog, 1024, fp))
			{
				CSWLogDevice::Log(szLog);
			}
			else if(dwNode != GetINode(strINI))
			{
				fclose(fp);
				fp = NULL;
			}
			else
			{
				Sleep(800);
			}
		}
		else
		{
			Sleep(800);
		}
	}
	swpa_mem_free(szLog);
}

DWORD CSWLogApplication::GetINode(LPCSTR szFileName)
{
	struct stat buf;
	if(!access(szFileName, 0) && !stat(szFileName, &buf))
	{
		return buf.st_ino;
	}
	return 0;
}
#else
HRESULT CSWLogApplication::Run()
{
	m_cLogThread.Start(OnLog, this);
	while(!IsExited() && S_OK == m_cLogThread.IsValid())
	{
		HeartBeat();
		Sleep(800);
	}
}

PVOID CSWLogApplication::OnLog(PVOID pvParam)
{
	CSWLogApplication *pThis = (CSWLogApplication *)pvParam;
	FILE *fp = NULL;
	CSWString strINI;
	strINI.Format("tail -f %s", (LPCSTR)GetCommandString("-i", "/var/log/messages"));
	CHAR* szLog =  (CHAR *)swpa_mem_alloc(1024);
	while(!pThis->IsExited() && S_OK == pThis->m_cLogThread.IsValid())
	{
		if(NULL == fp)
		{
			fp = popen(strINI, "r");
		}
		if(NULL != fp && NULL != fgets(szLog, 1024, fp))
		{
				CSWLogDevice::Log(szLog);
		}
		else
		{
			Sleep(800);
		}
	}
	swpa_mem_free(szLog);
	return 0;
}
#endif
#endif
