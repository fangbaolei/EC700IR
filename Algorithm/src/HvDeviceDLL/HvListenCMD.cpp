#include "HvListenCMD.h"
#include "HvCameraLink.h"
#include <atlstr.h>
#include <atltime.h>
#include <WinSock.h>

using std::vector;
//#define OUT_DEBUG_LOG	 1
CHvLitenCMD* gIHvLitenCMD = NULL;
BOOL CHvLitenCMD_GetXmlData(char* pRecvBuf,DWORD64* dwTime,DWORD* dwRoadID,DWORD* trigerID);
BOOL CHvLitenCMD_CheckIsHistory(HVAPI_LISTEN_HANDLE_CONTEXT* pContext,DWORD trigerID);
void CHvLitenCMD_WriteTestLog(LPCSTR lpszIP,DWORD64 dwTime,DWORD dwRoadID);
static DWORD WINAPI HvLitenThreadReviceFunc(LPVOID lpParameter)
{
	if ( NULL == lpParameter )
	{
		return -1;
	}
	
	CHvLitenCMD* pLitenCMD = (CHvLitenCMD*)lpParameter;
	if (!pLitenCMD->Init())
	{
		// 创建监听失败
		return -1;
	}
	return 0;
}

static DWORD WINAPI HvLitenThreadDealFunc(LPVOID lpParameter)
{
	if ( NULL == lpParameter )
	{
		return -1;
	}
	
	SOCKET hSocket = (SOCKET)lpParameter ;
	IHvLitenCMD* pCMD = IHvLitenCMD::GetInstance();
	if (!pCMD)
	{
		ForceCloseSocket(hSocket);
		return 0;
	}
	char szIP[16] = {0};
	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	int len = sizeof(addr);
	int ret = getpeername(hSocket,(struct sockaddr*)&addr,(int*)&len);
	strcpy(szIP,inet_ntoa(addr.sin_addr));

	HVAPI_LISTEN_HANDLE_CONTEXT* pHandle = pCMD->GetHandlContext(szIP);
	if (!pHandle)
	{
		ForceCloseSocket(hSocket);
		return 0;
	}

	EnterCriticalSection(&pHandle->csCallback);
	char buf[1024] = {0};
	HiVideo::CAMERA_CMD_HEADER cCmdRespond;
	if ( sizeof(cCmdRespond) == recv(hSocket, (char*)&cCmdRespond, sizeof(cCmdRespond), 0) )
	{
		if ( CAMERA_XML_EXT_CMD == cCmdRespond.dwID
			&& 0 < cCmdRespond.dwInfoSize )
		{
			int iRecvLen = RecvAll(hSocket, buf, cCmdRespond.dwInfoSize);
			if ( cCmdRespond.dwInfoSize ==  iRecvLen)
			{
				DWORD64 dwTime = 0;
				DWORD trigerID = 0;
				DWORD dwRoadID = 0;
				if(CHvLitenCMD_GetXmlData(buf,&dwTime,&dwRoadID,&trigerID)==S_OK){
					if (CHvLitenCMD_CheckIsHistory(pHandle,trigerID)!= TRUE)
					{
						if (pHandle->pCallBackFun!= NULL)
						{
							pHandle->pCallBackFun(pHandle->pUserData,dwTime,dwRoadID,trigerID);
						}
						if(pHandle->pCallBackFunEx!= NULL)
						{
							pHandle->pCallBackFunEx(pHandle->pUserData,buf,iRecvLen);
						}
					}
					
					//CHvLitenCMD_WriteTestLog(szIP,dwTime,trigerID);
				}
			}	
		}
	}
	ForceCloseSocket(hSocket);
	LeaveCriticalSection(&pHandle->csCallback);

	return 0;
}

void CHvLitenCMD_WriteTestLog(LPCSTR lpszIP,DWORD64 dwTime,DWORD trigerID)
{
	#ifdef OUT_DEBUG_LOG
	CTime testTime((dwTime)/1000);
	CString szTime = testTime.Format("%Y-%m-%d %H:%M:%S");
	CString ttTime;
	ttTime.Format("%s %d",szTime,(dwTime)%1000);
	CString szStr;
	szStr.Format("trigerID:%d ",trigerID);
	szStr+=szTime;

	CTime cTime = CTime::GetCurrentTime();
	char szCurTime[256] = {0};
	char   path[MAX_PATH];   
	GetCurrentDirectory(MAX_PATH,path); 
	CString szPath(path);
	szPath+="\\HvDeviceNewLog.txt";
	FILE* fp = fopen(szPath.GetBuffer(), "a+");
	szPath.ReleaseBuffer();
	if(fp)
	{
		sprintf(szCurTime, "%s  <%s>  ", cTime.Format("%Y-%m-%d %H:%M:%S"), lpszIP);
		fwrite(szCurTime, strlen(szCurTime), 1, fp);
		fwrite(szStr, strlen(szStr), 1, fp);
		fwrite("\n", 1, 1, fp);
		fclose(fp);
	}
	#endif
}

CHvLitenCMD::CHvLitenCMD(int iPort,int iLitenNum):m_iPort(iPort),m_iLitenNum(iLitenNum),
												m_hThreadRevice(NULL),m_hThreadDeal(NULL),
												m_fExit(TRUE),m_pCallFunc(NULL),m_pUserData(NULL),
												m_bInit(FALSE),m_pCallFuncEx(NULL)
{
	InitializeCriticalSection(&m_csConnection);
}

CHvLitenCMD::~CHvLitenCMD()
{
	Stop();
	DeleteCriticalSection(&m_csConnection);
}

BOOL CHvLitenCMD::Start()
{
	EnterCriticalSection(&m_csConnection);
	m_fExit = FALSE;
	LeaveCriticalSection(&m_csConnection);
	if (!m_hThreadRevice)
	{
		 m_hThreadRevice = CreateThread(NULL, 0, HvLitenThreadReviceFunc, this, 0, NULL);
	}
	m_bInit = TRUE;
	return FALSE;
}



BOOL CHvLitenCMD::Stop()
{
	EnterCriticalSection(&m_csConnection);
	m_fExit = TRUE;
	LeaveCriticalSection(&m_csConnection);

	HvSafeCloseThread(m_hThreadRevice);
	
	m_hThreadRevice = NULL;
	m_hThreadDeal = NULL;
	m_bInit = FALSE;
	return TRUE;
}

BOOL CHvLitenCMD::Close()
{
	Stop();
	std::vector<HVAPI_LISTEN_HANDLE_CONTEXT*>::iterator pIter = m_verLitenHANDLE.begin();
	for (;pIter!= m_verLitenHANDLE.end();++pIter)
	{
		HVAPI_LISTEN_HANDLE_CONTEXT* tmp = (HVAPI_LISTEN_HANDLE_CONTEXT*)(*pIter);
		ClearOneHandle(tmp);
		EnterCriticalSection(&tmp->csCallback);
		delete tmp;
		LeaveCriticalSection(&tmp->csCallback);
	}
	m_verLitenHANDLE.clear();
	gIHvLitenCMD = NULL;
	return TRUE;
}

BOOL CHvLitenCMD::Init()
{
	int nWaitMs = 3000;
	int nTimeOutMs = 4000;

	m_hSocket = HvCreateSocket();
	if ( INVALID_SOCKET == m_hSocket )
	{
		return FALSE;
	}
	if ( S_OK != HvListen(m_hSocket,m_iPort , m_iLitenNum) )
	{
		return FALSE;
	}
	

	SOCKET hNewSocket;
	while (!m_fExit)
	{
		hNewSocket = INVALID_SOCKET;
		HRESULT hr = HvAccept(m_hSocket, hNewSocket, nWaitMs);
		if (S_OK == hr)
		{
			// 创建新的连接
			setsockopt(hNewSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&nTimeOutMs,sizeof(nTimeOutMs));
			setsockopt(hNewSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&nTimeOutMs,sizeof(nTimeOutMs));
			if ( TRUE != DoDealData(hNewSocket) )
			{
				if ( INVALID_SOCKET != hNewSocket )
				{
					ForceCloseSocket(hNewSocket);
					hNewSocket = INVALID_SOCKET;
				}
			}
		}
	}
	ForceCloseSocket(m_hSocket);
	m_hSocket = INVALID_SOCKET;
	return TRUE;
}

HRESULT CHvLitenCMD::SetCallBack(HVAPI_HANDLE_EX hHandle,PVOID pFunc, PVOID pUserData)
{
	HVAPI_LISTEN_HANDLE_CONTEXT* pHanleContext = (HVAPI_LISTEN_HANDLE_CONTEXT*)hHandle;
	pHanleContext->pCallBackFun = (HVAPI_CALLBACK_LISTEN_CMD)pFunc;
	pHanleContext->pUserData = pUserData;
	return TRUE;
}

HRESULT CHvLitenCMD::SetCallBackEx(HVAPI_HANDLE_EX hHandle,INT iCallBackType,PVOID pFunc, PVOID pUserData)
{
	HVAPI_LISTEN_HANDLE_CONTEXT* pHanleContext = (HVAPI_LISTEN_HANDLE_CONTEXT*)hHandle;
	if (iCallBackType == CALLBACK_TYPE_LISTEN_XML_DATE)
	{
		pHanleContext->pCallBackFunEx = (HVAPI_CALLBACK_LISTEN_CMDEX)pFunc;
	}
	
	pHanleContext->pUserData = pUserData;
	return TRUE;
}


HVAPI_LISTEN_HANDLE_CONTEXT* CHvLitenCMD::GetHandlContext(char* szIP)
{
	std::vector<HVAPI_LISTEN_HANDLE_CONTEXT*>::iterator pIter = m_verLitenHANDLE.begin();
	for (;pIter!= m_verLitenHANDLE.end();++pIter)
	{
		HVAPI_LISTEN_HANDLE_CONTEXT* tmp = (HVAPI_LISTEN_HANDLE_CONTEXT*)(*pIter);
		if (strcmp(szIP,tmp->szIP)==0)
		{
			return tmp;
		}
	}

	return NULL;
}

BOOL CHvLitenCMD::DoDealData(SOCKET hSocket)
{
	char szIP[16] = {0};
	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	int len = sizeof(addr);
	int ret = getpeername(hSocket,(struct sockaddr*)&addr,(int*)&len);
	strcpy(szIP,inet_ntoa(addr.sin_addr));
	HVAPI_LISTEN_HANDLE_CONTEXT* pHandle = GetHandlContext(szIP);
	if (!pHandle)
	{
		return FALSE;
	}
	
	HANDLE	hNewThread = CreateThread(NULL,0,HvLitenThreadDealFunc,(LPVOID)hSocket,0,NULL);
	CloseHandle(hNewThread);
	//HANDLE hThreadDeal = CreateThread(NULL, 0, HvLitenThreadDealFunc, this, 0, NULL); 
	//ForceCloseSocket(hSocket);
	return TRUE;
}

BOOL CHvLitenCMD_GetXmlData(char* pRecvBuf,DWORD64* dwTime,DWORD* dwRoadID,DWORD* trigerID)
{
	HRESULT hr = E_FAIL;
	TiXmlDocument cXmlDoc;
	if ( cXmlDoc.Parse(pRecvBuf))
	{
		const TiXmlElement* pRootElement = cXmlDoc.RootElement();
		if ( pRootElement != NULL && 0 == strcmp("3.0", pRootElement->Attribute("Ver")) )
		{
			if ( 0 == strcmp("HvCmd", pRootElement->Value()) )
			{
				const TiXmlElement* pCmdArgElement = pRootElement->FirstChildElement("CmdName");
				if(pCmdArgElement != NULL && 0 == strcmp("TriggerPlateRecog" ,  pCmdArgElement->GetText()))
				{
					if ( pCmdArgElement != NULL)
					{
						const char* szValue = pCmdArgElement->Attribute("Value");
						if (szValue != NULL)
						{
							CString cszValue(szValue);
							INT pos_begin = cszValue.Find(":", 0);
							INT pos_end = cszValue.Find(";", pos_begin + 1);

							if (pos_begin == -1|| pos_end == -1)
							{
								hr = E_FAIL;
								return hr;
							}

							CString szTriggerID = cszValue.Mid(pos_begin + 1,  pos_end - pos_begin - 1);
							*trigerID = atoi(szTriggerID);
							// 是否考虑多触发的问题？

							pos_begin = cszValue.Find(":", pos_end + 1);
							pos_end = cszValue.Find(";", pos_begin + 1);

							if (pos_begin == -1 || pos_end == -1)
							{
								hr = E_FAIL;
								return hr;
							}

							CString szTriggerTime = cszValue.Mid(pos_begin + 1,  pos_end - pos_begin - 1);

							pos_begin = cszValue.Find(":", pos_end + 1);
							pos_end = cszValue.Find("]", pos_begin);

							if (pos_begin == -1 || pos_end == -1)
							{
								hr = E_FAIL;
								return hr;
							}

							CString szTriggerParam = cszValue.Mid(pos_begin + 1, pos_end - pos_begin -1);
							
							int nYear, nMonth, nDay, nHour, nMin, nSec,nMill;
							sscanf(szTriggerTime.GetBuffer(szTriggerTime.GetLength()), "%d-%d-%d %d:%d:%d %d", &nYear, &nMonth, &nDay, &nHour, &nMin, &nSec,&nMill);
							SYSTEMTIME st;
							st.wYear = nYear;
							st.wMonth = nMonth;
							st.wDay = nDay;
							st.wHour = nHour;
							st.wMinute = nMin;
							st.wSecond = nSec;
							//st.wMilliseconds = nMill;

							CTime tmpCtime(st);
							*dwTime = (DWORD64)tmpCtime.GetTime();
							
							(*dwTime) = (*dwTime) * 1000 + nMill;

							szTriggerTime.ReleaseBuffer();
							/*
							CTime testTime((*dwTime)/1000);
							CString szTime = testTime.Format("%Y-%m-%d %H:%M:%S");
							CString ttTime;
							ttTime.Format("%s %d",szTime,(*dwTime)%1000);
							*/
							hr = S_OK;
						}
					}
				}
			
				pCmdArgElement = pRootElement->FirstChildElement("RoadID");
				if ( !pCmdArgElement)
				{
					return E_FAIL;
				}
				
				const char* szValue = pCmdArgElement->Attribute("Value");
				if (!szValue)
				{
					return E_FAIL;
				}

				*dwRoadID = atoi(szValue);
				hr = S_OK;
			}
		}
	}
	return hr;
}

HVAPI_LISTEN_HANDLE_CONTEXT* CHvLitenCMD::OpenHandle(LPCSTR szIP)
{
	if(m_fExit)
	{
		return NULL;
	}

	std::vector<HVAPI_LISTEN_HANDLE_CONTEXT*>::iterator pIter = m_verLitenHANDLE.begin();
	for (;pIter!= m_verLitenHANDLE.end();++pIter)
	{
		HVAPI_LISTEN_HANDLE_CONTEXT* tmp = (HVAPI_LISTEN_HANDLE_CONTEXT*)(*pIter);
		if (strcmp(szIP,tmp->szIP)==0)
		{
			return tmp;
		}
	}
	EnterCriticalSection(&m_csConnection);
	HVAPI_LISTEN_HANDLE_CONTEXT* listenHandle = new HVAPI_LISTEN_HANDLE_CONTEXT();
	strcpy(listenHandle->szIP,szIP);
	m_verLitenHANDLE.push_back(listenHandle);
	LeaveCriticalSection(&m_csConnection);
	return listenHandle;
}

BOOL CHvLitenCMD_CheckIsHistory(HVAPI_LISTEN_HANDLE_CONTEXT* pContext,DWORD trigerID)
{
	if (!pContext)
	{
		return TRUE;
	}
	
	if (pContext->historyTrigerID ==  trigerID)
	{
		return TRUE;
	}
	pContext->historyTrigerID = trigerID;
	return FALSE;
}


BOOL CHvLitenCMD::CloseListenHandle(HVAPI_LISTEN_HANDLE_CONTEXT* handle)
{
	//std::vector<_HVAPI_LISTEN_HANDLE_CONTEXT>::const_iterator pIter = m_verLitenHANDLE.begin();
	EnterCriticalSection(&m_csConnection);
	
	std::vector<HVAPI_LISTEN_HANDLE_CONTEXT*>::iterator pIter = m_verLitenHANDLE.begin();
	for (;pIter!= m_verLitenHANDLE.end();++pIter)
	{
		HVAPI_LISTEN_HANDLE_CONTEXT* tmp = (HVAPI_LISTEN_HANDLE_CONTEXT*)(*pIter);
		if (strcmp(handle->szIP,tmp->szIP)==0)
		{
			ClearOneHandle(tmp);
			m_verLitenHANDLE.erase(pIter);
			LeaveCriticalSection(&m_csConnection);
			
			EnterCriticalSection(&tmp->csCallback);
			delete tmp;
			LeaveCriticalSection(&tmp->csCallback);
			return TRUE;;
		}
	}
	LeaveCriticalSection(&m_csConnection);
	return FALSE;
}

void CHvLitenCMD::ClearOneHandle(HVAPI_LISTEN_HANDLE_CONTEXT* pHandle)
{
	if (!pHandle)
	{
		return;
	}
	if (!(pHandle->hThread))
	{
		CloseHandle((pHandle->hThread));
		pHandle->hThread = NULL;
	}
	pHandle->historyTrigerID = -1;
	//memset(pHandle->historyTrigerID,-1,sizeof(pHandle->historyTrigerID));
}

HRESULT CHvLitenCMD::CloseListenHanle(HVAPI_HANDLE_EX hHandle)
{
	HVAPI_LISTEN_HANDLE_CONTEXT* pHandle = (HVAPI_LISTEN_HANDLE_CONTEXT*)hHandle;
	if (CloseListenHandle(pHandle))
	{
		return S_OK;
	}
	return E_FAIL;
}

HVAPI_HANDLE_EX CHvLitenCMD::OpenListenHanle(LPCSTR szIp)
{
	HVAPI_LISTEN_HANDLE_CONTEXT* pHandle = OpenHandle(szIp);
	if (!pHandle)
	{
		return NULL;
	}
	return (HVAPI_HANDLE_EX)pHandle;
	
}

IHvLitenCMD* IHvLitenCMD::CreateInstance(int iPort,int iLitenNum)
{
	if (gIHvLitenCMD)
	{
		return gIHvLitenCMD;
	}

	if ((gIHvLitenCMD = new CHvLitenCMD(iPort, iLitenNum)) == NULL)
			return NULL;

	return gIHvLitenCMD;
}

IHvLitenCMD* IHvLitenCMD::GetInstance()
{
	return gIHvLitenCMD;
}