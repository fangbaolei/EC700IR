
#include "HvDeviceEx.h"
#include "HvAutoLink.h"
#include "HvAutoLinkDevice.h"

#include <sys/time.h>
#include <time.h>
#include<stdio.h>

extern  void * RecordRecvThreadFuncEx(LPVOID lpParam);



HRESULT GetConnectionRequest(SOCKET hSocket ,LPSTR lpszDevSN , BOOL& fSafeSaverEnable);
HRESULT FindAutoLinkDev(LPSTR szDevSN , AUTOLINK_DEV_CONTEXT** ppADC);
HRESULT CreateAutoLinkDev(LPSTR szDevSN , AUTOLINK_DEV_CONTEXT** ppADC);
HRESULT GetAutoLinkConnCmd(HVAPI_HANDLE_CONTEXT_EX* pHandle , BOOL fSafeSaverEnable ,LPCSTR pAutoLinkConnCmd ,INT& nAutoLinkConnCmdLen);


//#include <atlstr.h>
ThreadMutex g_mutexDevList;
MONITOR_CONTEXT  *g_pMonitor = NULL;
const int MAX_MONITOR_NUM = 100;

static unsigned long GetTickCount2()
{
   // tms tm;
    //return times;
    unsigned long currentTime;
    struct timeval current;
    gettimeofday(&current, NULL);
    currentTime = current.tv_sec * 1000 + current.tv_usec/1000;

    return currentTime;
}


HRESULT GetConnectionRequest(SOCKET hSocket ,LPSTR lpszDevSN , BOOL& fSafeSaverEnable)
{
	HRESULT hr = E_FAIL;
	int iRecvBufLen = 0;
	CAMERA_CMD_HEADER cmdHeader;

	iRecvBufLen = RecvAll(hSocket , (char*)&cmdHeader , sizeof(cmdHeader));
	if (iRecvBufLen != sizeof(cmdHeader) || cmdHeader.dwID != CAMERA_XML_EXT_CMD)
	{
		return E_FAIL;
	}

	char* pRecvBuf = new char[cmdHeader.dwInfoSize];
	if (NULL == pRecvBuf)
	{
		return E_OUTOFMEMORY;
	}

	//ZeroMemory(pRecvBuf , cmdHeader.dwInfoSize);
	memset(pRecvBuf, 0,  cmdHeader.dwInfoSize);

	iRecvBufLen = RecvAll(hSocket , pRecvBuf , cmdHeader.dwInfoSize);
	if (iRecvBufLen != cmdHeader.dwInfoSize)
	{
		if (NULL == pRecvBuf)
		{
			delete[] pRecvBuf;
			pRecvBuf = NULL;
		}
		return E_NETFAIL;
	}
	else
	{
		int iSafeSaverFlag = 0;
		TiXmlDocument cXmlDoc;
		if ( cXmlDoc.Parse(pRecvBuf))
		{
			const TiXmlElement* pRootElement = cXmlDoc.RootElement();
			if ( pRootElement != NULL && 0 == strcmp("1.0", pRootElement->Attribute("ver")) )
			{
				if ( 0 == strcmp("HvCmd", pRootElement->Value()) )
				{
					if(0 == strcmp("ConnectionRequest" ,  pRootElement->FirstChildElement("CmdName")->GetText()))
					{
						const TiXmlElement* pCmdArgElement = pRootElement->FirstChildElement("CmdArg");
						if ( pCmdArgElement != NULL)
						{
							if(GetParamStringFromXml(pCmdArgElement , "SN" , lpszDevSN , 128) == S_OK &&
								GetParamIntFromXml(pCmdArgElement , "SafeSaverEnable" , &iSafeSaverFlag) == S_OK)
							{
								hr = S_OK;
							}
							else
							{
								hr = E_FAIL;
							}
							fSafeSaverEnable = (iSafeSaverFlag == 1);
						}
					}
				}
			}
		}

	}

	CAMERA_CMD_RESPOND cmdRespone;
	cmdRespone.dwID = CAMERA_XML_EXT_CMD;
	cmdRespone.dwInfoSize = 0;
	cmdRespone.dwResult = S_OK == hr?0:-1;

	int iSendLen = send(hSocket , (char*)&cmdRespone , sizeof(cmdRespone) , 0);

	hr = iSendLen != sizeof(cmdRespone)?E_FAIL:S_OK;
	if (NULL != pRecvBuf)
	{
		delete[] pRecvBuf;
		pRecvBuf = NULL;
	}
	return hr;
}


HRESULT CloseAutoLinkHHC(LPSTR szDevSN)
{
	if (NULL == szDevSN || NULL == g_pMonitor)
	{
		return E_POINTER;
	}

	if (NULL == g_pMonitor->prgAutoLinkDevList)
	{
		return E_POINTER;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = NULL;
	BOOL fDevFree = TRUE;

	//EnterCriticalSection(&g_csDevList);
	g_mutexDevList.lock();
	for (int i = 0 ; i < g_pMonitor->nMaxMonitorCount ; ++i)
	{
		pHHC = &g_pMonitor->prgAutoLinkDevList[i].handle;
		fDevFree = g_pMonitor->prgAutoLinkDevList[i].fFree;

		if (!fDevFree)
		{
			if (0 == strcmp(pHHC->szDevSN , szDevSN))
			{
				g_pMonitor->prgAutoLinkDevList[i].fFree = TRUE;
				g_pMonitor->prgAutoLinkDevList[i].eStatus = DEV_STATUS_UNKOWN;
				g_pMonitor->prgAutoLinkDevList[i].handle.dwVideoReconnectTimes = 0;

				//ZeroMemory(pHHC->szDevSN , sizeof(pHHC->szDevSN));
				memset(pHHC->szDevSN , 0, sizeof(pHHC->szDevSN));

				pHHC->dwVideoConnStatus = CONN_STATUS_UNKNOWN;
				pHHC->dwRecordConnStatus = CONN_STATUS_UNKNOWN;

				pHHC->fAutoLink = FALSE;

				HVAPI_CALLBACK_SET* pTemp = NULL;
				for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
				{
					memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
				}

                g_mutexDevList.unlock();
				//LeaveCriticalSection(& g_csDevList);
				return S_OK;
			}
		}
	}

	g_mutexDevList.unlock();
	//LeaveCriticalSection(& g_csDevList);

	return E_FAIL;
}



HRESULT FindAutoLinkDev(LPSTR szDevSN , AUTOLINK_DEV_CONTEXT** ppADC)
{
	if (NULL == szDevSN || NULL == g_pMonitor || NULL == ppADC)
	{
		return E_POINTER;
	}

	if (NULL == g_pMonitor->prgAutoLinkDevList)
	{
		return E_POINTER;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = NULL;
	BOOL fFreeHHC = TRUE;

	//EnterCriticalSection(&g_csDevList);
	g_mutexDevList.lock();
	for (int i = 0 ; i<g_pMonitor->nMaxMonitorCount ; ++i)
	{
		pHHC = &g_pMonitor->prgAutoLinkDevList[i].handle;
		fFreeHHC = g_pMonitor->prgAutoLinkDevList[i].fFree;

		if (!fFreeHHC)
		{
			if (0 == strcmp(pHHC->szDevSN , szDevSN))
			{
				*ppADC = &g_pMonitor->prgAutoLinkDevList[i];
				//LeaveCriticalSection(& g_csDevList);
				g_mutexDevList.unlock();
				return S_OK;
			}
		}
	}

	g_mutexDevList.unlock();
	//LeaveCriticalSection(& g_csDevList);

	*ppADC = NULL;
	return S_OK;
}


HRESULT CreateAutoLinkDev(LPSTR szDevSN , AUTOLINK_DEV_CONTEXT** ppADC)
{
	if (NULL == szDevSN || NULL == g_pMonitor || NULL == ppADC)
	{
		return E_POINTER;
	}

	if (NULL == g_pMonitor->prgAutoLinkDevList)
	{
		return E_POINTER;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = NULL;
	BOOL fDevFree = NULL;

//	EnterCriticalSection(& g_csDevList);
    g_mutexDevList.lock();
	for (int i = 0 ; i<g_pMonitor->nMaxMonitorCount ; ++i)
	{
		pHHC = &g_pMonitor->prgAutoLinkDevList[i].handle;
		fDevFree = g_pMonitor->prgAutoLinkDevList[i].fFree;

		if (fDevFree)
		{
			pHHC->fAutoLink = TRUE;
			strcpy(pHHC->szDevSN , szDevSN);
			pHHC->dwRecordStreamTick = GetTickCount2();

			//g_pMonitor->prgAutoLinkDevList[i].dwRecordConnectLastTick = GetTickCount();
			g_pMonitor->prgAutoLinkDevList[i].fFree = FALSE;

			*ppADC = &g_pMonitor->prgAutoLinkDevList[i];
			g_mutexDevList.unlock();
		//	LeaveCriticalSection(&g_csDevList);
			return S_OK;
		}
	}
	g_mutexDevList.unlock();
	//LeaveCriticalSection(& g_csDevList);


	*ppADC = NULL;
	return E_OUTOFMEMORY;
}


 HRESULT GetCmdValue(LPCSTR pCmd , LPCSTR pId ,CHAR* pValueBuf , INT nValueBufLen )
{
	char* pValueBegin = NULL;
	char* pValueEnd = NULL;

	char* pIdBegin = strstr((char*)pCmd , pId);
	if (NULL != pIdBegin)
	{
		pValueBegin = strstr(pIdBegin ,"[");
		pValueEnd = strstr(pIdBegin , "]");
		if (NULL != pValueBegin && NULL != pValueEnd)
		{
			int nValueLen = (pValueEnd - 1) - (pValueBegin +1)+1;
			if (nValueLen < nValueBufLen)
			{
				memcpy(pValueBuf ,pValueBegin +1 ,nValueLen );
				pValueBuf[nValueLen] = '\0';
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

BOOL IsRecvHistory(LPCSTR pRecordConCmd)
{
	if (NULL == strstr(pRecordConCmd , "DownloadRecord"))
	{
		return FALSE;
	}

	int nValueLen = 128;
	char szValue[128] = {0};
	//ZeroMemory(szValue , sizeof(szValue));
	if (S_OK == GetCmdValue(pRecordConCmd , "Enable" ,szValue ,nValueLen))
	{
		return atoi(szValue);
	}

	return FALSE;
}


HRESULT WriteRecordCacheInfo(LPCSTR pRecordConCmd ,HVAPI_HANDLE_CONTEXT_EX* pHandle)
{
	PACK_RESUME_CACHE* pPackResumeCache = pHandle->pPackResumeCache;

	if (NULL == pPackResumeCache)
	{
		return E_POINTER;
	}

	int nValueLen = 128;
	char szValue[128];
	//ZeroMemory(szValue , sizeof(szValue));
	memset(szValue, 0, sizeof(szValue));

	if (S_OK == pHandle->fIsRecvHistoryRecord)
	{
		//ZeroMemory(szValue , sizeof(szValue));
		memset(szValue, 0, sizeof(szValue));
		nValueLen = 128;
		GetCmdValue(pRecordConCmd , "DataOffset" ,szValue ,nValueLen);
		pPackResumeCache->nDataOffset = atoi(szValue);

		//ZeroMemory(szValue , sizeof(szValue));
		memset(szValue, 0, sizeof(szValue));
		nValueLen = 128;
		GetCmdValue(pRecordConCmd , "InfoOffset" ,szValue ,nValueLen);
		pPackResumeCache->nInfoOffset = atoi(szValue);
	}

	return S_OK;
}
HRESULT ReadRecordCacheInfo(HVAPI_HANDLE_CONTEXT_EX* pHandle ,LPCSTR pRecordConCmd , INT& nRecordConCmdLen)
{
	PACK_RESUME_CACHE* pPackResumeCache = pHandle->pPackResumeCache;
	if (NULL == pRecordConCmd || NULL == pPackResumeCache)
	{
		return E_POINTER;
	}

	char szTemp[256] = {0};
	//ZeroMemory(szTemp , sizeof(szTemp));

	sprintf(szTemp , "DownloadRecord,"
		"Enable[%d],"
		"BeginTime[%s],"
		"Index[%d],"
		"EndTime[%s],"
		"DataInfo[%d],"
		"DataOffset[%d],"
		"InfoOffset[%d]"
		,pHandle->fIsRecvHistoryRecord
		,pHandle->szRecordBeginTimeStr
		,pHandle->dwRecordStartIndex
		,pHandle->szRecordEndTimeStr
		,pHandle->iRecordDataInfo
		,pPackResumeCache->nDataOffset
		,pPackResumeCache->nInfoOffset
		);

	if (nRecordConCmdLen > strlen(szTemp) + 1)
	{
		strcpy((char*)pRecordConCmd , szTemp);
		nRecordConCmdLen = strlen(pRecordConCmd);

		return S_OK;
	}

	return E_FAIL;
}


 HRESULT GetAutoLinkConnCmd(HVAPI_HANDLE_CONTEXT_EX* pHandle , BOOL fSafeSaverEnable ,LPCSTR pAutoLinkConnCmd ,INT& nAutoLinkConnCmdLen)
{
	if (NULL == pHandle || NULL == pAutoLinkConnCmd)
	{
		return E_POINTER;
	}

	//ÈôÎªÎÞÐ§µÄœá¹û»ºŽæ ÔòÖ±œÓÓÃ SetCallBackžøµÄÃüÁî
	if (!pHandle->fVailPackResumeCache)
	{
		if (strlen((char*)pHandle->szRecordConnCmd) < nAutoLinkConnCmdLen)
		{
			strcpy((char*)pAutoLinkConnCmd , (char*)pHandle->szRecordConnCmd);
			nAutoLinkConnCmdLen = strlen((char*)pHandle->szRecordConnCmd);
		}

		//¿ªÆô¿É¿¿ÐÔ±£Žæ ÇÒ œÓÊÕÀúÊ· ²ÅÆôÓÃ¶ÏµãÐøŽ«
		if (fSafeSaverEnable && pHandle->fIsRecvHistoryRecord)
		{
			if (NULL == pHandle->pPackResumeCache)
			{
				pHandle->pPackResumeCache = new PACK_RESUME_CACHE;
				if (NULL == pHandle->pPackResumeCache)
				{
					return E_OUTOFMEMORY;
				}
				//³õÊŒ»¯ °ü»º³å
				PACK_RESUME_CACHE* pPackResumeCache = pHandle->pPackResumeCache;

				pPackResumeCache->pInfor = new CHAR[DEFAULT_INFOR_LEN];
				if (NULL == pPackResumeCache->pInfor)
				{
					return E_OUTOFMEMORY;
				}
				pPackResumeCache->nMaxInforLen = DEFAULT_INFOR_LEN;

				pPackResumeCache->pData = new CHAR[DEFAULT_DATA_LEN];
				if (NULL == pPackResumeCache->pData)
				{
					delete pPackResumeCache->pInfor;
					return E_OUTOFMEMORY;
				}
				pPackResumeCache->nMaxDataLen = DEFAULT_DATA_LEN;
			}
			else
			{
				PACK_RESUME_CACHE * pPackResumeCache = pHandle->pPackResumeCache;
				pPackResumeCache->fVailHeader = FALSE;
				pPackResumeCache->fVailInfor = FALSE;
				pPackResumeCache->fVailData = FALSE;

				pPackResumeCache->nDataLen = 0;
				pPackResumeCache->nInforLen = 0;

			}

			//œ«ÃüÁî ÐŽÈë »ºŽæÐÅÏ¢ÖÐ£šÓÃÓÚ¶ÏµãÐøŽ«£©
			if (nAutoLinkConnCmdLen > 0)
			{
				WriteRecordCacheInfo(pAutoLinkConnCmd , pHandle);
			}
			pHandle->fVailPackResumeCache = TRUE;
		}
	}
	else
	{
		return ReadRecordCacheInfo(pHandle , pAutoLinkConnCmd ,nAutoLinkConnCmdLen  );
	}

	return S_OK;
}


 void * ProcessRecordNewLinkThreadFunc(LPVOID lpParameter)
{
	SOCKET hNewSocket = (SOCKET)lpParameter;

	char rgDevSN[128];
	//ZeroMemory(rgDevSN ,sizeof(rgDevSN));
	memset(rgDevSN, 0, 128);
	BOOL fSafeSaverEnable = FALSE;

	if ( S_OK != GetConnectionRequest(hNewSocket , rgDevSN , fSafeSaverEnable) )
	{
		return 0;
	}

	BOOL fNewLink = FALSE;
	AUTOLINK_DEV_CONTEXT* pADC = NULL;
	if (S_OK != FindAutoLinkDev(rgDevSN , &pADC))
	{
		return 0;
	}
	//ÈôÎªÐÂÉè±žµÄÁ¬œÓ ÔòŽŽœš
	if (NULL == pADC)
	{
		if (S_OK != CreateAutoLinkDev(rgDevSN , &pADC))
		{
			ForceCloseSocket(hNewSocket);
			return 0;
		}
	}

	if (NULL == pADC)
	{
		return 0;
	}

	bool fSendCmdRequest = false;
	int nConCmdLen = 512;
	char rgConCmd[512];
//	ZeroMemory(rgConCmd ,sizeof(rgConCmd));
    memset(rgConCmd, 0, 512);

	HVAPI_HANDLE_CONTEXT_EX* pHHC = &pADC->handle;

	bool fVailCallback = FALSE;
	HVAPI_CALLBACK_SET* pTemp = NULL;

	for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
	{
		if (NULL != pTemp->pOnPlate)
		{
			fVailCallback = TRUE;
		}
	}

	if (NULL != pADC)
	{
		HVAPI_HANDLE_CONTEXT_EX* pHHC = &pADC->handle;

		//pADC->dwRecordConnectLastTick = GetTickCount();

		if (DEV_STATUS_SETCONN == pADC->eStatus &&
			(CONN_STATUS_DISCONN == pHHC->dwRecordConnStatus ||
			CONN_STATUS_RECONN == pHHC->dwRecordConnStatus ))
		{
			//pHHC->dwRecordReconectTimes++;
			pHHC->dwRecordReconnectTimes++;
		}
	}
	else
	{
		return 0;
	}

	if (fVailCallback && CONN_STATUS_RECVDONE != pHHC->dwRecordConnStatus)
	{
		if (S_OK != GetAutoLinkConnCmd(pHHC, fSafeSaverEnable ,  rgConCmd , nConCmdLen ))
		{
			return 0;
		}


		char szRetBuf[256] = {0};
		if ( S_OK != HvSendXmlCmd((char*)rgConCmd, szRetBuf, sizeof(szRetBuf), NULL , NULL, hNewSocket) )
		{
			ForceCloseSocket(hNewSocket);
			pHHC->sktRecord = INVALID_SOCKET;
			pHHC->dwRecordConnStatus = CONN_STATUS_DISCONN;

			return 0;
		}
		//printf("%s\n", szRetBuf);

		pHHC->sktRecord = hNewSocket;
		pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
		pHHC->dwRecordStreamTick = GetTickCount2();

		if ( 0  == pHHC->pthreadRecvRecord)
		{
			pHHC->fIsThreadRecvRecordExit = FALSE;
			//pHHC->hThreadRecvRecord = CreateThread(NULL, 0, RecordRecvThreadFuncEx, pHHC, 0, NULL);
			//pthread_create( &(pHHC->pthreadRecvRecord), NULL,  RecordRecvThreadFuncEx, (void*)pHHC );
			if (pthread_create( &(pHHC->pthreadRecvRecord), NULL,  RecordRecvThreadFuncEx, (void*)pHHC ))
			{
				return 0;
			}
			/*else{
			    printf(" Recv start\n");
			}*/
		}
	}
	else
	{
		ForceCloseSocket(hNewSocket);
		return 0;
	}

}


HRESULT SetNewLink(LPVOID lpParameter ,SOCKET hNewSocket )
{
	SERVER_CONTEXT* pServer = (SERVER_CONTEXT*)lpParameter;

	if (NULL == pServer || INVALID_SOCKET == hNewSocket)
	{
		return E_FAIL;
	}

	if (NULL == pServer->prgProcessNewLinkThread)
	{
		return E_POINTER;
	}

	//²éÕÒ¿ÕÏÐµÄÏß³Ì
	pthread_t * phFreeThread = NULL;
	DWORD dwExitCode = 0;
	for (int i = 0 ; i< pServer->nMaxMonitorCount; ++i)
	{
		if (NULL == pServer->prgProcessNewLinkThread[i])
		{
			phFreeThread = &pServer->prgProcessNewLinkThread[i];
			break;
		}
		/*
		if (GetExitCodeThread(pServer->prgProcessNewLinkThread[i] ,&dwExitCode ))
		{
			if (STILL_ACTIVE != dwExitCode)
			{
				phFreeThread = &pServer->prgProcessNewLinkThread[i];
				break;
			}
		}
		else
		{
			return E_FAIL;
		}*/

	}

	if (LISTEN_TYPE_RECORD == pServer->nMonitorType && NULL != phFreeThread)
	{
		//*phFreeThread = CreateThread(NULL, 0, ProcessRecordNewLinkThreadFunc, (LPVOID)hNewSocket, 0, NULL);

        pthread_t th = 0;

		if (  pthread_create( &th,  NULL, ProcessRecordNewLinkThreadFunc, (LPVOID)hNewSocket ) )
		{
			return E_FAIL;
		}
		*phFreeThread = th;
	}
	return S_OK;
}


HRESULT GetAutoLinkCount(INT nType , INT & nLinkCount)
{
	if (NULL == g_pMonitor)
	{
		return E_POINTER;
	}

	if (NULL == g_pMonitor->prgAutoLinkDevList)
	{
		return E_POINTER;
	}

	INT nRecordActiveLinkCount = 0;

	HVAPI_HANDLE_CONTEXT_EX* pHHC = NULL;
	BOOL fFreeDev = TRUE;

	INT nNowTickCount = GetTickCount2();

	//EnterCriticalSection(&g_csDevList);
	g_mutexDevList.lock();
	for (int i = 0 ; i<g_pMonitor->nMaxMonitorCount ; ++i)
	{
		pHHC = &g_pMonitor->prgAutoLinkDevList[i].handle;
		fFreeDev = g_pMonitor->prgAutoLinkDevList[i].fFree;

		if (!fFreeDev)
		{
			nRecordActiveLinkCount++;
		}
	}
	g_mutexDevList.unlock();
//	LeaveCriticalSection(&g_csDevList);


	if (LISTEN_TYPE_RECORD == nType)
	{
		nLinkCount = nRecordActiveLinkCount;
	}

	return S_OK;
}


HV_API HRESULT CDECL HVAPI_LoadMonitor(INT nMaxMonitorCount, LPCSTR szApiVer)
{
    if (NULL != g_pMonitor)
	{
		return S_OK;
	}

	g_pMonitor = new MONITOR_CONTEXT;
	if (NULL == g_pMonitor)
	{
		return E_OUTOFMEMORY;
	}
	if (nMaxMonitorCount>MAX_MONITOR_NUM || nMaxMonitorCount < 1 )
	{
		nMaxMonitorCount = MAX_MONITOR_NUM;
	}

	g_pMonitor->prgAutoLinkDevList = new AUTOLINK_DEV_CONTEXT[nMaxMonitorCount];
	if (NULL == g_pMonitor->prgAutoLinkDevList)
	{
		delete g_pMonitor;
		g_pMonitor = NULL;
		return E_OUTOFMEMORY;
	}

	g_pMonitor->nMaxMonitorCount = nMaxMonitorCount;

	//InitializeCriticalSection(&g_csDevList);InitializeCriticalSection(&g_csDevList);
	return  NULL == g_pMonitor ? E_FAIL:S_OK;
}

void*  HvServerMonitorThreadFunc(LPVOID lpParameter)
{
    if ( NULL == lpParameter )
	{
		return NULL;
	}

	SERVER_CONTEXT* pServer = (SERVER_CONTEXT*)lpParameter;

	int nWaitMs = 3000;
	int nTimeOutMs = 4000;

	int & hSocket = pServer->hSocket;
	int  hNewSocket = INVALID_SOCKET;

	pServer->prgProcessNewLinkThread = new pthread_t[pServer->nMaxMonitorCount];
	for( int iPos = 0; iPos < pServer->nMaxMonitorCount; iPos++)
	pServer->prgProcessNewLinkThread[iPos] = 0;
	//memset(pServer->prgProcessNewLinkThread , 0, sizeof(pthread_t) * pServer->nMaxMonitorCount);

	while ( !pServer->fThreadMonitorExit )
	{
		if (S_OK != GetAutoLinkCount(pServer->nMonitorType , pServer->nLinkCount))
		{
			ForceCloseSocket(hSocket);
			hSocket = INVALID_SOCKET;
			break;
		}

		if (INVALID_SOCKET == hSocket && pServer->nLinkCount < pServer->nMaxMonitorCount)
		{
			hSocket = HvCreateSocket();
			if ( INVALID_SOCKET == hSocket )
			{
				sleep(2);
				continue;
			}
			if ( S_OK != HvListen(hSocket, pServer->nMonitorPort , 1) )
			{
				sleep(2);
				continue;
			}
		}


		if ( pServer->nLinkCount > pServer->nMaxMonitorCount )
		{
		//	OutputDebugString("³¬¹ýÏÞÖÆÁŽœÓÊý£¡");
			if ( INVALID_SOCKET != hSocket )
			{
				ForceCloseSocket(hSocket);
				hSocket = INVALID_SOCKET;
			}
			sleep(2);
			continue;
		}


		hNewSocket = INVALID_SOCKET;
		HRESULT hr = HvAccept(hSocket, hNewSocket, nWaitMs);
		if (S_OK == hr)
		{

			//setsockopt(hNewSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&nTimeOutMs,sizeof(nTimeOutMs));
			//setsockopt(hNewSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&nTimeOutMs,sizeof(nTimeOutMs));
			HvSetRecvTimeOut( hNewSocket,nTimeOutMs );
			HvSetSendTimeOut( hNewSocket, nTimeOutMs );


			pServer->nLinkCount++;
			if ( S_OK != SetNewLink((LPVOID)pServer , hNewSocket) )
			{
				if ( INVALID_SOCKET != hNewSocket )
				{
					ForceCloseSocket(hNewSocket);
					hNewSocket = INVALID_SOCKET;
				}
			}
		}
	} // while ( !pServerHHC->fThreadServerMonitorExit )
    return NULL;
}


HRESULT StartServerMonitor(SERVER_CONTEXT** ppServer ,INT nPort , INT nMaxMonitorCount , INT nMonitorType)
{
	if (NULL == ppServer)
	{
		return E_FAIL;
	}

	SERVER_CONTEXT* pServer = new SERVER_CONTEXT;
	if (NULL == pServer)
	{
		return E_OUTOFMEMORY;
	}
	pServer->nMonitorPort = nPort;
	pServer->nMaxMonitorCount = nMaxMonitorCount;
	pServer->nMonitorType = nMonitorType;
	pServer->fThreadMonitorExit = FALSE;

	//pServer->hThreadMonitor = CreateThread(NULL, 0, HvServerMonitorThreadFunc, pServer, 0, NULL);
  // pServer->hThreadMonitor = CreateThread(NULL, 0, HvServerMonitorThreadFunc, pServer, 0, NULL);
   if ( pthread_create(& (pServer->hThreadMonitor), NULL, HvServerMonitorThreadFunc, pServer) )
	{
		delete pServer;
		*ppServer = NULL;
		return E_FAIL;
	}
	*ppServer = pServer;
	return S_OK;
}

HV_API HRESULT CDECL HVAPI_OpenServer(INT nPort , INT nListenType, LPCSTR szApiVer)
{
    	if ( NULL == szApiVer )
	{
		szApiVer = HVAPI_API_VERSION_EX;
	}

	MONITOR_CONTEXT* pMonitor = NULL;

	if ( 0 == strcmp(HVAPI_API_VERSION_EX, szApiVer))
	{
		if (S_OK != HVAPI_LoadMonitor(MAX_MONITOR_NUM , szApiVer))
		{
			return E_FAIL;
		}

		pMonitor = g_pMonitor;
		if ( pMonitor )
		{
			strcpy(pMonitor->szVersion, szApiVer);

			if (LISTEN_TYPE_RECORD == nListenType)
			{
				if(S_OK != StartServerMonitor(&pMonitor->pRecordServer ,nPort , pMonitor->nMaxMonitorCount ,LISTEN_TYPE_RECORD ))
				{
					return E_FAIL;
				}
			}
			else if (LISTEN_TYPE_VEDIO == nListenType)
			{
				return E_FAIL;
			}
			else if (LISTEN_TYPE_IMAGE == nListenType)
			{
				return NULL;
			}
			else if (LISTEN_TYPE_CMD == nListenType)
			{
				return E_FAIL;
			}
			else
			{
				return E_FAIL;
			}

		}
	}


	return NULL == pMonitor?E_FAIL :S_OK;

}

HV_API HRESULT CDECL HVAPI_CloseServer(INT nType)
{
    if ( NULL == g_pMonitor )
	{
		return E_FAIL;
	}

	if (NULL != g_pMonitor->pRecordServer && LISTEN_TYPE_RECORD == nType)
	{
		SERVER_CONTEXT* pRecordServer =  g_pMonitor->pRecordServer;

		ForceCloseSocket(pRecordServer->hSocket);

		pRecordServer->fThreadMonitorExit = TRUE;
		HvSafeCloseThread(pRecordServer->hThreadMonitor);


		if (NULL != pRecordServer->prgProcessNewLinkThread)
		{
			for (int i = 0 ; i<pRecordServer->nMaxMonitorCount; ++i)
			{
				HvSafeCloseThread(pRecordServer->prgProcessNewLinkThread[i]);
			}

			delete[] pRecordServer->prgProcessNewLinkThread;
			pRecordServer->prgProcessNewLinkThread = NULL;
		}

		delete g_pMonitor->pRecordServer;
		g_pMonitor->pRecordServer = NULL;
	}

	return S_OK;
}

HV_API HRESULT CDECL HVAPI_UnLoadMonitor()
{
    if (NULL == g_pMonitor)
	{
		return E_FAIL;
	}

	if (NULL != g_pMonitor->pRecordServer)
	{
		HVAPI_CloseServer(LISTEN_TYPE_RECORD);
	}

	if (NULL != g_pMonitor->prgAutoLinkDevList)
	{
		for (int i = 0 ; i<g_pMonitor->nMaxMonitorCount ; ++i)
		{
			if (!g_pMonitor->prgAutoLinkDevList[i].fFree)
			{
			    //
			   // printf("start\n");
				HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)&g_pMonitor->prgAutoLinkDevList[i].handle;

				HVAPI_CloseEx((HVAPI_HANDLE_EX)pHHC);
				pHHC = NULL;
				//printf("finish\n");
			}
		}
	}

	if (NULL != g_pMonitor->prgAutoLinkDevList)
	{
		delete[] g_pMonitor->prgAutoLinkDevList;
		g_pMonitor->prgAutoLinkDevList = NULL;
	}

	//DeleteCriticalSection(&g_csDevList);

	delete g_pMonitor;
	g_pMonitor = NULL;
	return S_OK;
}

HV_API HRESULT CDECL HVAPI_GetDeviceListSize(INT& nDevListSize , LPCSTR szApiVer)
{
    if ( NULL == szApiVer )
	{
		szApiVer = HVAPI_API_VERSION_EX;
	}

	if (0 == strcmp(HVAPI_API_VERSION_EX , szApiVer))
	{
		if (S_OK != HVAPI_LoadMonitor(MAX_MONITOR_NUM , szApiVer))
		{
			return E_FAIL;
		}
	}

	if (NULL == g_pMonitor->prgAutoLinkDevList)
	{
		return E_FAIL;
	}

	int nDevCount = 0;

	HVAPI_HANDLE_CONTEXT_EX* pHHC = NULL;
	BOOL fDevFree = true;

	//EnterCriticalSection(&g_csDevList);
	g_mutexDevList.lock();
	for (int i = 0; i <g_pMonitor->nMaxMonitorCount ; ++i)
	{
		pHHC = &g_pMonitor->prgAutoLinkDevList[i].handle;
		fDevFree = g_pMonitor->prgAutoLinkDevList[i].fFree;
		if (pHHC->fAutoLink && !fDevFree)
		{
			nDevCount++;
		}

	}
	//LeaveCriticalSection(&g_csDevList);
	g_mutexDevList.unlock();

	nDevListSize = nDevCount*129 + 1;

	return S_OK;
}


HV_API int CDECL HVAPI_GetDeviceList(LPSTR szDevList , INT nDevListLen , LPCSTR szApiVer)
{
	if (NULL == szDevList)
	{
		return -1;
	}


	if ( NULL == szApiVer )
	{
		szApiVer = HVAPI_API_VERSION_EX;
	}

	if (0 == strcmp(HVAPI_API_VERSION_EX , szApiVer))
	{
		if (S_OK != HVAPI_LoadMonitor(MAX_MONITOR_NUM , szApiVer))
		{
			return -1;
		}
	}

	if (NULL == g_pMonitor->prgAutoLinkDevList)
	{
		return -1;
	}

	//ZeroMemory(szDevList ,nDevListLen );
	memset(szDevList, 0, nDevListLen);


	HVAPI_HANDLE_CONTEXT_EX* pHHC = NULL;
	BOOL fDevFree = true;

	//EnterCriticalSection(&g_csDevList);
	g_mutexDevList.lock();
	for (int i = 0; i <g_pMonitor->nMaxMonitorCount ; ++i)
	{
		pHHC = &g_pMonitor->prgAutoLinkDevList[i].handle;
		fDevFree = g_pMonitor->prgAutoLinkDevList[i].fFree;
		if (pHHC->fAutoLink && !fDevFree)
		{
			int nStrlen = strlen(szDevList) + strlen(pHHC->szDevSN) + 2;
			if (strlen(szDevList) + strlen(pHHC->szDevSN) + 2 <= nDevListLen)
			{
				strcat(szDevList , pHHC->szDevSN);
				strcat(szDevList , ";");
			}

		}

	}
	g_mutexDevList.unlock();
	//LeaveCriticalSection(&g_csDevList);


	return S_OK;
}


HV_API HRESULT CDECL HVAPI_GetDeviceStatus(LPSTR szDevSN ,INT& nRecordLinkStatus , INT& nRecordLinkReConCount , LPCSTR szApiVer)
{
	if (NULL == szDevSN)
	{
		return E_FAIL;
	}

	if ( NULL == szApiVer )
	{
		szApiVer = HVAPI_API_VERSION_EX;
	}

	if (0 == strcmp(HVAPI_API_VERSION_EX , szApiVer))
	{
		if (S_OK != HVAPI_LoadMonitor(MAX_MONITOR_NUM , szApiVer))
		{
			return E_FAIL;
		}
	}

	AUTOLINK_DEV_CONTEXT* pADC = NULL;
	if (S_OK != FindAutoLinkDev(szDevSN , &pADC))
	{
		return E_FAIL;
	}

	if (NULL != pADC)
	{
		nRecordLinkReConCount = pADC->handle.dwRecordReconnectTimes;//pADC->nRecordReConnCount;
		nRecordLinkStatus = pADC->handle.dwRecordConnStatus;

		return S_OK;
	}

	return E_FAIL;
}


HRESULT GetRecordInfor(char* pResultInfo , DWORD32& dw32TimeL, DWORD32& dw32TimeH , DWORD32& dw32Index)
{
	HRESULT hr = E_FAIL;
	const TiXmlElement* pElementname;
	TiXmlDocument cXmlDoc;


	if ( cXmlDoc.Parse(pResultInfo) )
	{
		const TiXmlElement* pRootElement = cXmlDoc.RootElement();
		if ( pRootElement )
		{
			const TiXmlElement* pElementResultSet = pRootElement->FirstChildElement("ResultSet");
			if ( pElementResultSet )
			{
				const TiXmlElement* pElementResult = pElementResultSet->FirstChildElement("Result");
				if ( pElementResult )
				{
					const TiXmlElement* pElementPlateName = pElementResult->FirstChildElement("PlateName");
					if ( pElementPlateName )
					{
						// Ê¶±ðœá¹ûÊ±Œä
						pElementname = pElementResult->FirstChildElement("TimeLow");
						if (pElementname)
						{
							sscanf(pElementname->Attribute("value"), "%u", &dw32TimeL);
						}

						pElementname = pElementResult->FirstChildElement("TimeHigh");
						if (pElementname)
						{
							sscanf(pElementname->Attribute("value"), "%u", &dw32TimeH);
						}

						pElementname = pElementResult->FirstChildElement("CarID");
						if (pElementname)
						{
							dw32Index = atoi(pElementname->Attribute("value"));
						}
						hr = S_OK;
					}

				}
			}
		}
	}

	return hr;

}

HRESULT UpdatePackCache(HVAPI_HANDLE_CONTEXT_EX* pHandle, PACK_TYPE eType , char* pBuffer , int nBufferLen )
{
	if (NULL == pHandle)
	{
		return E_POINTER;
	}

	if (NULL == pHandle->pPackResumeCache)
	{
		return E_POINTER;
	}
	PACK_RESUME_CACHE* pPackResumeCache = pHandle->pPackResumeCache;

	if (PACK_TYPE_HEADER == eType)
	{
		if (NULL == pBuffer)
		{
			pPackResumeCache->fVailHeader = FALSE;
		}
		//ÓÐÊýŸÝÔò²»žüÐÂÍ·
		else if(0 == pPackResumeCache->nInforLen && 0 == pPackResumeCache->nDataLen)
		{
			if (nBufferLen == sizeof(pPackResumeCache->header))
			{
				memcpy((void*)&pPackResumeCache->header ,pBuffer ,nBufferLen);
				pPackResumeCache->fVailHeader = TRUE;

				//²é¿ŽInfo»º³åŽóÐ¡ÊÇ·ñÂú×ã ²»Âú×ãÔòÀ©Žó
				DWORD32 dw32MaxInfoLen = pPackResumeCache->header.dwInfoLen;
				if (dw32MaxInfoLen > pPackResumeCache->nMaxInforLen && dw32MaxInfoLen< MAX_INFOR_LEN)
				{
					if (NULL != pPackResumeCache->pInfor)
					{
						delete[] pPackResumeCache->pInfor;
						pPackResumeCache->pInfor = NULL;
					}
					dw32MaxInfoLen *= 1.5;
					pPackResumeCache->pInfor = new CHAR[dw32MaxInfoLen];
					if (NULL == pPackResumeCache->pInfor)
					{
						return E_OUTOFMEMORY;
					}
					pPackResumeCache->nMaxInforLen = dw32MaxInfoLen;
				}
				else
				{
					return E_FAIL;
				}

				//²é¿ŽData»º³åŽóÐ¡ÊÇ·ñÂú×ã ²»Âú×ãÔòÀ©Žó
				DWORD32 dw32MaxDataLen = pPackResumeCache->header.dwDataLen;
				if (dw32MaxDataLen > pPackResumeCache->nMaxDataLen && dw32MaxDataLen < MAX_DATA_LEN)
				{
					if (NULL != pPackResumeCache->pData)
					{
						delete[] pPackResumeCache->pData;
						pPackResumeCache->pData = NULL;
					}
					dw32MaxDataLen *= 1.5;
					pPackResumeCache->pData = new CHAR[dw32MaxDataLen];
					if (NULL == pPackResumeCache->pData)
					{
						return E_OUTOFMEMORY;
					}
					pPackResumeCache->nMaxDataLen = dw32MaxDataLen;

				}
				else
				{
					return E_FAIL;
				}

			}
			else
			{
				return E_FAIL;
			}

		}
	}

	if (!pPackResumeCache->fVailHeader)
	{
		pPackResumeCache->fVailInfor = FALSE;
		pPackResumeCache->nInforLen = 0;
		pPackResumeCache->fVailData = FALSE;
		pPackResumeCache->nDataLen = 0 ;
		return S_OK;
	}

	if (PACK_TYPE_INFO == eType)
	{
		if (NULL == pBuffer)
		{
			pPackResumeCache->fVailInfor = FALSE;
			pPackResumeCache->nInforLen = 0;
		}
		else
		{

			if (nBufferLen + pPackResumeCache->nInforLen <= pPackResumeCache->header.dwInfoLen &&
				pPackResumeCache->header.dwInfoLen <= pPackResumeCache->nMaxInforLen)
			{
				memcpy(pPackResumeCache->pInfor + pPackResumeCache->nInforLen ,pBuffer , nBufferLen);
				pPackResumeCache->nInforLen += nBufferLen;
			}
			else
			{
				pPackResumeCache->nInforLen = 0;
				return E_FAIL;
			}

			if (pPackResumeCache->header.dwInfoLen == pPackResumeCache->nInforLen)
			{
				pPackResumeCache->fVailInfor = TRUE;
			}
			pPackResumeCache->nInfoOffset = pPackResumeCache->nInforLen;
		}
	}

	if (PACK_TYPE_DATA == eType)
	{
		if (NULL == pBuffer)
		{
			pPackResumeCache->fVailData = FALSE;
			pPackResumeCache->nDataLen = 0;
		}
		else
		{

			if (pPackResumeCache->nDataLen + nBufferLen <= pPackResumeCache->header.dwDataLen &&
				pPackResumeCache->header.dwDataLen <= pPackResumeCache->nMaxDataLen)
			{
				memcpy(pPackResumeCache->pData + pPackResumeCache->nDataLen ,pBuffer , nBufferLen);
				pPackResumeCache->nDataLen += nBufferLen;
			}
			else
			{
				pPackResumeCache->nDataLen = 0;
				return E_FAIL;
			}

			if(pPackResumeCache->header.dwDataLen == pPackResumeCache->nDataLen)
			{
				pPackResumeCache->fVailData = TRUE;
			}

			pPackResumeCache->nDataOffset = pPackResumeCache->nDataLen;
		}

	}

	return S_OK;
}



HRESULT GetPackCache( HVAPI_HANDLE_CONTEXT_EX* pHandle , PACK_TYPE eType ,unsigned char** ppBuffer , int& iBufferLen )
{
	if (NULL == pHandle)
	{
		return E_FAIL;
	}

	if (NULL == pHandle->pPackResumeCache)
	{
		return E_POINTER;
	}

	PACK_RESUME_CACHE* pPackResumeCache = pHandle->pPackResumeCache;
	if (PACK_TYPE_HEADER == eType)
	{
		return E_FAIL;
	}

	if (PACK_TYPE_INFO == eType)
	{
		if (pPackResumeCache->fVailHeader && pPackResumeCache->fVailInfor)
		{
			*ppBuffer = (unsigned char*)pPackResumeCache->pInfor;
			iBufferLen = pPackResumeCache->nInforLen;
			return S_OK;
		}
	}

	if (PACK_TYPE_DATA == eType)
	{
		if (pPackResumeCache->fVailHeader && pPackResumeCache->fVailData)
		{
			*ppBuffer = (unsigned char*)pPackResumeCache->pData;
			iBufferLen = pPackResumeCache->nDataLen;
			return S_OK;
		}
	}

	return E_FAIL;;
}


HRESULT IsEmptyPackCache( HVAPI_HANDLE_CONTEXT_EX* pHandle , PACK_TYPE eType , bool& fIsEmptyCache)
{
	fIsEmptyCache = TRUE;
	if (NULL == pHandle)
	{
		return E_FAIL;
	}

	if (NULL == pHandle->pPackResumeCache)
	{
		return E_POINTER;
	}

	PACK_RESUME_CACHE* pPackResumeCache = pHandle->pPackResumeCache;
	if (PACK_TYPE_HEADER == eType)
	{
		return E_FAIL;
	}

	if (PACK_TYPE_INFO == eType)
	{
		if (pPackResumeCache->fVailHeader)
		{
			fIsEmptyCache = (pPackResumeCache->nInforLen == 0);
			return S_OK;
		}
	}

	if (PACK_TYPE_DATA == eType)
	{
		if (pPackResumeCache->fVailHeader)
		{
			fIsEmptyCache = (pPackResumeCache->nDataLen == 0);
			return S_OK;
		}
	}

	return E_FAIL;;
}

HRESULT SetConnAutoLinkHHC(LPSTR szDevSN)
{
	if (NULL == szDevSN || NULL == g_pMonitor)
	{
		return E_POINTER;
	}

	if (NULL == g_pMonitor->prgAutoLinkDevList)
	{
		return E_POINTER;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = NULL;
	BOOL fDevFree = TRUE;
	DEV_STATUS eDevStatus = DEV_STATUS_UNKOWN;

	//EnterCriticalSection(&g_csDevList);
	g_mutexDevList.lock();
	for (int i = 0 ; i<g_pMonitor->nMaxMonitorCount ; ++i)
	{
		pHHC = &g_pMonitor->prgAutoLinkDevList[i].handle;
		fDevFree = g_pMonitor->prgAutoLinkDevList[i].fFree;
		eDevStatus = g_pMonitor->prgAutoLinkDevList[i].eStatus;

		if (!fDevFree)
		{
			if (0 == strcmp(pHHC->szDevSN , szDevSN) && DEV_STATUS_OPEN == eDevStatus)
			{
				g_pMonitor->prgAutoLinkDevList[i].eStatus = DEV_STATUS_SETCONN;
                g_mutexDevList.unlock();
				//LeaveCriticalSection(& g_csDevList);
				return S_OK;
			}
		}
	}
	g_mutexDevList.unlock();
	//LeaveCriticalSection(& g_csDevList);

	return E_FAIL;
}




HRESULT OpenAutoLinkHHC(LPSTR szDevSN , HVAPI_HANDLE_CONTEXT_EX** ppHHC)
{
	if (NULL == szDevSN || NULL == g_pMonitor || NULL == ppHHC)
	{
		return E_POINTER;
	}

//	if (NULL == g_pMonitor->prgAutoLinkDevList)
//	{
	//	return E_POINTER;
//	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = NULL;
	BOOL fDevFree = TRUE;
	DEV_STATUS eDevStatus = DEV_STATUS_UNKOWN;

	//EnterCriticalSection(&g_csDevList);
	g_mutexDevList.lock();
	for (int i = 0 ; i<g_pMonitor->nMaxMonitorCount ; ++i)
	{
		pHHC = &g_pMonitor->prgAutoLinkDevList[i].handle;
		fDevFree = g_pMonitor->prgAutoLinkDevList[i].fFree;
		eDevStatus = g_pMonitor->prgAutoLinkDevList[i].eStatus;

		if (!fDevFree)
		{
			if (0 == strcmp(pHHC->szDevSN , szDevSN) && DEV_STATUS_UNKOWN == eDevStatus)
			{
			    //printf("Find*******\n");
				g_pMonitor->prgAutoLinkDevList[i].eStatus = DEV_STATUS_OPEN;

				*ppHHC = pHHC;
				//LeaveCriticalSection(& g_csDevList);
				g_mutexDevList.unlock();
				return S_OK;
			}
		}
	}
	g_mutexDevList.unlock();
	//LeaveCriticalSection(& g_csDevList);

	*ppHHC = NULL;
	return E_FAIL;

}


HV_API HVAPI_HANDLE_EX CDECL HVAPI_OpenAutoLink(LPCSTR szDevName, LPCSTR szApiVer)
{


	if ( NULL == szDevName )
	{
		return NULL;
	}

	if ( NULL == szApiVer )
	{
		szApiVer = HVAPI_API_VERSION_EX;
	}

	HVAPI_HANDLE_EX hRet = NULL;

	if ( 0 == strcmp(HVAPI_API_VERSION_EX, szApiVer) )
	{
		HVAPI_HANDLE_CONTEXT_EX* handle = NULL;

		if (NULL != g_pMonitor)
		{
			OpenAutoLinkHHC((LPSTR)szDevName , &handle );
		}

		if (handle)
		{
			strcpy(handle->szVersion, szApiVer);
			strcpy(handle->szDevSN , szDevName);
			handle->fAutoLink = TRUE;
			handle->dwOpenType = 1;
		}

		hRet = (HVAPI_HANDLE_EX)handle;
	}

	return hRet;
}







