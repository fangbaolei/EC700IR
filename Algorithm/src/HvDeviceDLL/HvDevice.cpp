#include "HvDevice.h"
#include "HVAPI_HANDLE_CONTEXT.h"
#include "HvDeviceDLL.h"
#include "ximage.h"
#include "Console.h"
#include "Buffer.h"
#include <map>
using namespace std;

#pragma warning( disable : 4996 )

CRITICAL_SECTION g_csConnection;
map<HVAPI_HANDLE_CONTEXT *, int>g_connMap;

/* HvDeviceDLL.cpp */
extern bool OnDllProcessAttach();

using namespace HiVideo;

bool g_fHvDeviceInitialized = false;	// HvApi全局初始化标识
CRITICAL_SECTION g_csGlobalSync;		// 全局同步
CRITICAL_SECTION g_csWriteLog;			// HV_WriteLog同步

//#include <atlstr.h>
//#include <atltime.h>
//
//void TestReconnectLog(LPCSTR pstr)
//{
//	CTime cTime = CTime::GetCurrentTime();
//	CString strTemp = cTime.Format("%Y-%m-%d_%H:%M:%S");
//	FILE* fp = fopen("D:\\HvDeviceDll.log", "a+");
//	if(fp)
//	{
//		fwrite(strTemp.GetBuffer(), strTemp.GetLength(), 1, fp);
//		fwrite(pstr, strlen(pstr), 1, fp);
//		fwrite("\n", 1, 1, fp);
//		fclose(fp);
//	}
//}


static bool InitHvDevice()
{
	if ( true == g_fHvDeviceInitialized )
	{
		return true;
	}
	else
	{
		if ( true == OnDllProcessAttach() )
		{
			InitializeCriticalSection(&g_csConnection);
			g_fHvDeviceInitialized = true;
			return true;
		}
		else
		{
			return false;
		}
	}
}

HV_API DWORD CDECL HVAPI_SearchDevice(DWORD dwDevType, LPSTR szDevInfoList, INT nBufLen)
{
#ifndef _DEBUG
	//CConsole::Open();
//	CConsole::SetLogFile("HVDevicedll.txt");
#endif
	if ( false == InitHvDevice() )
	{
		return 0;
	}
//	CConsole::Write("excute HVAPI_SearchDevice");

	if ( NULL == szDevInfoList )
	{
		return 0;
	}

	DWORD32 dwCount = 0;
	DWORD32 dwRetCount = 0;
	DWORD64 dw64MacAddr = 0;
	DWORD32 dwIP = 0;
	DWORD32 dwMask = 0;
	DWORD32 dwGateway = 0;
	char szMacAddr[32] = {0};
	char szIP[16] = {0};
	char szMask[16] = {0};
	char szGateway[16] = {0};
	char szDeviceNetInfo[128] = {0};

	strcpy(szDevInfoList, "");

	if ( S_OK == SearchHVDeviceCount(&dwCount) )
	{
		for ( DWORD32 i = 0; i < dwCount; ++i )
		{
			if ( S_OK == GetHVDeviceAddr(i, &dw64MacAddr, &dwIP, &dwMask, &dwGateway) )
			{
				if ( TRUE == MyGetMacString(dw64MacAddr, szMacAddr, dwDevType) )
				{
					MyGetIpString(dwIP, szIP);
					MyGetIpString(dwMask, szMask);
					MyGetIpString(dwGateway, szGateway);

					sprintf(szDeviceNetInfo, "%s,%s,%s,%s;", szMacAddr, szIP, szMask, szGateway);
					if ( strlen(szDevInfoList) + strlen(szDeviceNetInfo) < (size_t)nBufLen )
					{
						strcat(szDevInfoList, szDeviceNetInfo);
						dwRetCount++;
					}
					else
					{
						return dwRetCount;
					}
				}
			}
		}
	}

	return dwRetCount;
}

HV_API HRESULT CDECL HVAPI_SetIPFromMac(DWORD64 dw64MacAddr,
                                        DWORD32 dwIP,
                                        DWORD32 dwMask,
                                        DWORD32 dwGateway)
{
    return SetIPFromMac(dw64MacAddr, dwIP, dwMask, dwGateway);
}

HV_API HRESULT CDECL HVAPI_GetDevType(PSTR pcIP, int* iDeviceType)
{
	const int iBufLen = (1024 << 4);
	char* pszXmlBuf = new char[iBufLen];
	int iXmlBufLen = iBufLen;
	*iDeviceType = 0;
	if(!pszXmlBuf)
	{
		return E_FAIL;
	}
	memset(pszXmlBuf, 0, iBufLen);
	//DWORD dwXmlVersion = 0;
	//HvGetXmlProtocolVersion(pcIP, &dwXmlVersion);
	bool fIsNewXml = (PROTOCOL_VERSION_2 == GetProtocolVersion(pcIP));//(dwXmlVersion == 0) ? false : true; 
	if(HvMakeXmlInfoByString(fIsNewXml ? PROTOCOL_VERSION_2 : PROTOCOL_VERSION_1, "GetDevType", 10, pszXmlBuf, iXmlBufLen) == E_FAIL)
	{
		return E_FAIL;
	}
	char* pszRetBuf = new char[1024];
	if(!pszRetBuf)
	{
		SAFE_DELETE(pszXmlBuf);
		return E_FAIL;
	}
	memset(pszRetBuf, 0, 1024);
	int iRetBufLen = 1024;
	SOCKET sktCmd = INVALID_SOCKET;
	if(ExecXmlExtCmdEx(pcIP, (char*)pszXmlBuf, (char*)pszRetBuf, iRetBufLen, sktCmd) == false)
	{
		SAFE_DELETE(pszRetBuf);
		SAFE_DELETE(pszXmlBuf);
		return S_OK;
	}
	char szRetcode[128] = {0};
	if(!fIsNewXml)
	{
		if(FAILED(HvParseXmlInfoRespValue(pszRetBuf, "GetDevType", "DevType", szRetcode)))
		{
			SAFE_DELETE(pszRetBuf);
			SAFE_DELETE(pszXmlBuf);
			return S_OK;
		}
	}
	else
	{
		if(FAILED(HvParseXmlCmdRespRetcode2(pszRetBuf, "GetDevType", "DevType", szRetcode)))
		{
			SAFE_DELETE(pszRetBuf);
			SAFE_DELETE(pszXmlBuf);
			return S_OK;
		}
	}

	if(strstr(szRetcode, "DM6467"))
	{
		if(strstr(szRetcode, "DM6467_1080P_SECURITY"))  //正式版本device 不支持安防相机 显示类型未知
		{
			*iDeviceType = DEV_TYPE_UNKNOWN;
		}
		else if(strstr(szRetcode, "_S"))
		{
			*iDeviceType = DEV_TYPE_HVSIGLE;
		}
		else if(strstr(szRetcode, "_200W"))
		{
			*iDeviceType = DEV_TYPE_HVCAM_200W;
		}
		else if(strstr(szRetcode, "_500W"))
		{
			*iDeviceType = DEV_TYPE_HVCAM_500W;
		}
		else
		{
			iDeviceType = DEV_TYPE_UNKNOWN;
		}
	}
	else
	{
		*iDeviceType = DEV_TYPE_UNKNOWN;
	}
	SAFE_DELETE(pszRetBuf);
	SAFE_DELETE(pszXmlBuf);
	return S_OK;
}

// 连接状态监控线程
static DWORD WINAPI HvSocketStatusMonitorThreadFunc(LPVOID lpParameter)
{
	if ( NULL == lpParameter )
	{
		return -1;
	}

	HVAPI_HANDLE_CONTEXT* pHHC = (HVAPI_HANDLE_CONTEXT*)lpParameter;

	BOOL fNeedToSleep = TRUE;

	while ( !pHHC->fThreadSocketStatusMonitorExit )
	{
		fNeedToSleep = TRUE;
		{
			CCSLock sLock(&pHHC->csCallbackJpeg);
			if ( CONN_STATUS_RECONN == pHHC->dwImageConnStatus && pHHC->pFuncCallbackJpeg)
			{
				fNeedToSleep = FALSE;
//				CConsole::Write("reconnecting image socket.");
				if ( ConnectCamera(pHHC->szIP, CAMERA_IMAGE_LINK_PORT, pHHC->sktImage) )
				{
					if ( strlen((const char*)pHHC->szImageConnCmd) > 0 )
					{
						char szRetBuf[256] = {0};
						if ( S_OK == HvSendXmlCmd(pHHC->szIP, (char*)pHHC->szImageConnCmd, szRetBuf, sizeof(szRetBuf), NULL, pHHC->sktImage) )
						{
							pHHC->dwImageConnStatus = CONN_STATUS_NORMAL;
							pHHC->dwJpegStreamRecvThreadLastTick = GetTickCount();
						}
					}
					else
					{
						pHHC->dwImageConnStatus = CONN_STATUS_NORMAL;
						pHHC->dwJpegStreamRecvThreadLastTick = GetTickCount();
					}
//					CConsole::Write("reconnect image socket ok.");
				}
			}
		}
		{
			CCSLock sLock(&pHHC->csCallbackH264);
			if ( CONN_STATUS_RECONN == pHHC->dwVideoConnStatus && pHHC->pCallbackH264UserData)
			{
				fNeedToSleep = FALSE;				
//				CConsole::Write("reconnecting video socket.");
				if ( ConnectCamera(pHHC->szIP, CAMERA_VIDEO_LINK_PORT, pHHC->sktVideo) )
				{
					if ( strlen((const char*)pHHC->szVideoConnCmd) > 0 )
					{
						char szRetBuf[256] = {0};
						if ( S_OK == HvSendXmlCmd(pHHC->szIP, (char*)pHHC->szVideoConnCmd, szRetBuf, sizeof(szRetBuf), NULL, pHHC->sktVideo) )
						{
							pHHC->dwVideoConnStatus = CONN_STATUS_NORMAL;
							pHHC->dwH264StreamRecvThreadLastTick = GetTickCount();
						}
					}
					else
					{
						pHHC->dwVideoConnStatus = CONN_STATUS_NORMAL;
						pHHC->dwH264StreamRecvThreadLastTick = GetTickCount();
					}
				}
//				CConsole::Write("reconnect video socket ok.");
			}
		}
		{
			CCSLock sLock(&pHHC->csCallbackRecord);
			if ( CONN_STATUS_RECONN == pHHC->dwRecordConnStatus && pHHC->pFuncCallbackRecord)
			{
				fNeedToSleep = FALSE;				
//				CConsole::Write("reconnecting record socket.");
				if ( ConnectCamera(pHHC->szIP, CAMERA_RECORD_LINK_PORT, pHHC->sktRecord) )
				{
					if ( strlen((const char*)pHHC->szRecordConnCmd) > 0 )
					{
						char szRetBuf[256] = {0};
						if ( S_OK == HvSendXmlCmd(pHHC->szIP, (char*)pHHC->szRecordConnCmd, szRetBuf, sizeof(szRetBuf), NULL, pHHC->sktRecord) )
						{
							pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
							pHHC->dwRecordStreamRecvThreadLastTick = GetTickCount();
						}
					}
					else
					{
						pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
						pHHC->dwRecordStreamRecvThreadLastTick = GetTickCount();
					}
				}
//				CConsole::Write("reconnect record socket ok.");
			}

		}
		if (fNeedToSleep)
		{
			Sleep(1000);
		}
	} // while ( !pHHC->fThreadSocketStatusMonitorExit )

	return 0;
}

HV_API HVAPI_HANDLE CDECL HVAPI_Open(LPCSTR szIp, LPCSTR szApiVer)
{
//	CConsole::Write("excute HVAPI_Open");
	if ( false == InitHvDevice() )
	{
		return NULL;
	}

	if ( NULL == szIp )
	{
		return NULL;
	}

	if ( NULL == szApiVer )
	{
		szApiVer = HVAPI_API_VERSION;
	}

	HVAPI_HANDLE hRet = NULL;

	if ( 0 == strcmp(HVAPI_API_VERSION, szApiVer) )
	{

		HVAPI_HANDLE_CONTEXT* handle = new HVAPI_HANDLE_CONTEXT;

		if ( handle )
		{
			CCSLock sLock(&g_csConnection);
			g_connMap[handle] = 1;
//			CConsole::Write("add connection 0x%x", (int)handle);

			strcpy(handle->szVersion, szApiVer);
			strcpy(handle->szIP, szIp);

			hRet = (HVAPI_HANDLE)handle;

			handle->fNewProtocol = (PROTOCOL_VERSION_2 == GetProtocolVersion(handle->szIP));
//			CConsole::Write("NewProtocol = %d", handle->fNewProtocol);
			char szRetBuf[512] = {0};
			if ( S_OK != HVAPI_GetInfo(hRet, "GetVersionString", szRetBuf, sizeof(szRetBuf), NULL) )
			{
				g_connMap.erase(handle);
				delete handle;
				return NULL;
			}
			else
			{
//				CConsole::Write(szRetBuf);
				handle->fThreadSocketStatusMonitorExit = FALSE;
				handle->hThreadSocketStatusMonitor = CreateThread(NULL, 0, HvSocketStatusMonitorThreadFunc, handle, 0, NULL);
				if ( NULL == handle->hThreadSocketStatusMonitor )
				{
					g_connMap.erase(handle);
					delete handle;
					return NULL;
				}
			}
		}
	}
	return hRet;
}

HV_API HRESULT CDECL HVAPI_Close(HVAPI_HANDLE hHandle)
{
	if ( NULL == hHandle )
	{
		return E_FAIL;
	}
//	CConsole::Write("execute HVAPI_Close");

	HVAPI_HANDLE_CONTEXT* pHHC = (HVAPI_HANDLE_CONTEXT*)hHandle;

	CCSLock sLock(&g_csConnection);
	map<HVAPI_HANDLE_CONTEXT *,int>::iterator itr = g_connMap.find(pHHC);
	if(itr == g_connMap.end())
	{
//		CConsole::Write("handle 0x%x is not valid.", (int)hHandle);
		return false;
	}
	g_connMap.erase(itr);
	sLock.Unlock();

	pHHC->fThreadSocketStatusMonitorExit = TRUE;
	HvSafeCloseThread(pHHC->hThreadSocketStatusMonitor);

	pHHC->fThreadImageExit = TRUE;
	pHHC->fThreadVideoExit = TRUE;
	pHHC->fThreadRecordExit = TRUE;

	HvSafeCloseThread(pHHC->hThreadRecvImage);
	HvSafeCloseThread(pHHC->hThreadRecvVideo);
	HvSafeCloseThread(pHHC->hThreadRecvRecord);

	ForceCloseSocket(pHHC->sktImage);
	ForceCloseSocket(pHHC->sktVideo);
	ForceCloseSocket(pHHC->sktRecord);
	
	delete pHHC;
	return S_OK;
}

HV_API HRESULT CDECL HVAPI_GetParam(HVAPI_HANDLE hHandle, LPSTR szXmlParam, INT nBufLen, INT* pnRetLen)
{
//	CConsole::Write("execute HVAPI_GetParam");
	if ( NULL == hHandle || NULL == szXmlParam )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT* pHHC = (HVAPI_HANDLE_CONTEXT*)hHandle;

	CCSLock sLock(&g_csConnection);
	map<HVAPI_HANDLE_CONTEXT *,int>::iterator itr = g_connMap.find(pHHC);
	if(itr == g_connMap.end())
	{
//		CConsole::Write("handle 0x%x is not valid.", (int)hHandle);
		return E_FAIL;
	}

	if ( 0 != strcmp(HVAPI_API_VERSION, pHHC->szVersion) )
	{
//		CConsole::Write("HVAPI_GetParam, version error, version string=%s", pHHC->szVersion);
		return E_FAIL;
	}

	const int iBufLen = (1024 << 10);
	char* rgchXmlParamBuf = new char[iBufLen];  // 1MB
	if(rgchXmlParamBuf == NULL)
	{
		return E_FAIL;
	}
	memset(rgchXmlParamBuf, 0, sizeof(rgchXmlParamBuf));

	const WORD wPort = CAMERA_CMD_LINK_PORT;
	SOCKET hSocketCmd = INVALID_SOCKET;

	CAMERA_CMD_HEADER cCmdHeader;
	CAMERA_CMD_RESPOND cCmdRespond;

	if ( ConnectCamera(pHHC->szIP, wPort, hSocketCmd) )
	{
		cCmdHeader.dwID = CAMERA_GET_PARAM_CMD;
		cCmdHeader.dwInfoSize = 0;
		if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0) )
		{
			ForceCloseSocket(hSocketCmd);
			SAFE_DELETE(rgchXmlParamBuf);
//			CConsole::Write("send CAMERA_GET_PARAM_CMD error.");
			return E_FAIL;
		}

		if ( sizeof(cCmdRespond) == recv(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0) )
		{
			if ( CAMERA_GET_PARAM_CMD == cCmdRespond.dwID
				&& 0 == cCmdRespond.dwResult 
				&& 0 < cCmdRespond.dwInfoSize )
			{
				if ( cCmdRespond.dwInfoSize == RecvAll(hSocketCmd, rgchXmlParamBuf, cCmdRespond.dwInfoSize) )
				{
					if ( nBufLen > (int)cCmdRespond.dwInfoSize )
					{
						if ( pnRetLen )
						{
							*pnRetLen = cCmdRespond.dwInfoSize;
						}
						memcpy(szXmlParam, rgchXmlParamBuf, cCmdRespond.dwInfoSize);
						ForceCloseSocket(hSocketCmd);
						SAFE_DELETE(rgchXmlParamBuf);
						return S_OK;
					}
					else
					{
						if ( pnRetLen )
						{
							*pnRetLen = cCmdRespond.dwInfoSize;
						}
						ForceCloseSocket(hSocketCmd);
						SAFE_DELETE(rgchXmlParamBuf);
						return E_FAIL;
					}
				}
			}
		}

		ForceCloseSocket(hSocketCmd);
	}
	SAFE_DELETE(rgchXmlParamBuf);
	return E_FAIL;
}

HV_API HRESULT CDECL HVAPI_SetParam(HVAPI_HANDLE hHandle, LPCSTR szXmlParam)
{
	if ( NULL == hHandle || NULL == szXmlParam )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT* pHHC = (HVAPI_HANDLE_CONTEXT*)hHandle;

	CCSLock sLock(&g_csConnection);
	map<HVAPI_HANDLE_CONTEXT *,int>::iterator itr = g_connMap.find(pHHC);
	if(itr == g_connMap.end())
	{
		return E_FAIL;
	}

	if ( 0 != strcmp(HVAPI_API_VERSION, pHHC->szVersion) )
	{
//		CConsole::Write("HVAPI_SetParam version = %s, error return", HVAPI_API_VERSION);
		return E_FAIL;
	}

	const WORD wPort = CAMERA_CMD_LINK_PORT;
	SOCKET hSocketCmd = INVALID_SOCKET;

	CAMERA_CMD_HEADER cCmdHeader;
	CAMERA_CMD_RESPOND cCmdRespond;

	if ( ConnectCamera(pHHC->szIP, wPort, hSocketCmd) )
	{
		cCmdHeader.dwID = CAMERA_SET_PARAM_CMD;
		cCmdHeader.dwInfoSize = (int)strlen(szXmlParam)+1;
		int iReciveTimeOutMS = 60000;
		setsockopt(hSocketCmd, SOL_SOCKET, SO_RCVTIMEO, (char*)&iReciveTimeOutMS, sizeof(iReciveTimeOutMS));
		if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0) )
		{
			ForceCloseSocket(hSocketCmd);
			return E_FAIL;
		}
		if ( cCmdHeader.dwInfoSize != send(hSocketCmd, (const char*)szXmlParam, cCmdHeader.dwInfoSize, 0) )
		{
			ForceCloseSocket(hSocketCmd);
			return E_FAIL;
		}

		if ( sizeof(cCmdRespond) == recv(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0) )
		{
			if ( CAMERA_SET_PARAM_CMD == cCmdRespond.dwID
				&& 0 == cCmdRespond.dwResult )
			{
				ForceCloseSocket(hSocketCmd);
				return S_OK;
			}
		}

		ForceCloseSocket(hSocketCmd);
	}

	return E_FAIL;
}

HV_API HRESULT CDECL HVAPI_SendControllPannelUpdateFile(HVAPI_HANDLE hHandle, PBYTE pUpdateFileBuffer, DWORD dwFileSize)
{
//	CConsole::Write("execute HVAPI_SendControllPannelUpdateFile");
	if ( NULL == hHandle || NULL == pUpdateFileBuffer )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT* pHHC = (HVAPI_HANDLE_CONTEXT*)hHandle;

	CCSLock sLock(&g_csConnection);
	map<HVAPI_HANDLE_CONTEXT *,int>::iterator itr = g_connMap.find(pHHC);
	if(itr == g_connMap.end())
	{
//		CConsole::Write("handle 0x%x is not valid.", (int)hHandle);
		return E_FAIL;
	}

	if ( 0 != strcmp(HVAPI_API_VERSION, pHHC->szVersion) )
	{
		return E_FAIL;
	}

	const WORD wPort = CAMERA_CMD_LINK_PORT;
	SOCKET hSocketCmd = INVALID_SOCKET;

	CAMERA_CMD_HEADER cCmdHeader;
	CAMERA_CMD_RESPOND cCmdRespond;

	if ( ConnectCamera(pHHC->szIP, wPort, hSocketCmd) )
	{
		cCmdHeader.dwID = CAMERA_UPDATE_CONTROLL_PANNEL;
		cCmdHeader.dwInfoSize = (int)(dwFileSize+1);
		if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0) )
		{
			ForceCloseSocket(hSocketCmd);
			return E_FAIL;
		}
		if ( cCmdHeader.dwInfoSize != send(hSocketCmd, (const char*)pUpdateFileBuffer, cCmdHeader.dwInfoSize, 0) )
		{
			ForceCloseSocket(hSocketCmd);
			return E_FAIL;
		}

		if ( sizeof(cCmdRespond) == recv(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0) )
		{
			if ( CAMERA_UPDATE_CONTROLL_PANNEL == cCmdRespond.dwID
				&& 0 == cCmdRespond.dwResult )
			{
				ForceCloseSocket(hSocketCmd);
				return S_OK;
			}
		}

		ForceCloseSocket(hSocketCmd);
	}

	return E_FAIL;
}

HV_API HRESULT CDECL HVAPI_ExecCmd(HVAPI_HANDLE hHandle, LPCSTR szCmd, LPSTR szRetBuf, INT nBufLen, INT* pnRetLen)
{
//	CConsole::Write("execute HVAPI_ExecCmd:%s", szCmd);
	if ( NULL == hHandle 
		|| NULL == szCmd 
		|| NULL == szRetBuf 
		|| nBufLen <= 0 )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT* pHHC = (HVAPI_HANDLE_CONTEXT*)hHandle;

	CCSLock sLock(&g_csConnection);
	map<HVAPI_HANDLE_CONTEXT *,int>::iterator itr = g_connMap.find(pHHC);
	if(itr == g_connMap.end())
	{
//		CConsole::Write("handle 0x%x is not valid.", (int)hHandle);
		return E_FAIL;
	}

	if ( 0 != strcmp(HVAPI_API_VERSION, pHHC->szVersion) )
	{
		return E_FAIL;
	}

	const int iBufLen = (1024 << 4);
	char* pszXmlBuf = new char[iBufLen];  // 16KB
	if(pszXmlBuf == NULL)
	{
		return E_FAIL;
	}
	int iXmlBufLen = iBufLen;

	TiXmlDocument cXmlDoc;
	if ( !cXmlDoc.Parse(szCmd) )  // 检查szCmd是否是XML
	{
		if ( E_FAIL == HvMakeXmlCmdByString(pHHC->fNewProtocol ? PROTOCOL_VERSION_2 : PROTOCOL_VERSION_1, szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen) )
		{
			SAFE_DELETE(pszXmlBuf);
			return E_FAIL;
		}

		szCmd = (char*)pszXmlBuf;
	}

	bool fRet = ExecXmlExtCmd(pHHC->szIP, (char*)szCmd, (char*)szRetBuf, nBufLen);
	if ( pnRetLen )
	{
		*pnRetLen = nBufLen;
	}

	SAFE_DELETE(pszXmlBuf);
	return (true == fRet) ? S_OK : E_FAIL;
}

HV_API HRESULT CDECL HVAPI_GetInfo(HVAPI_HANDLE hHandle, LPCSTR szCmd, LPSTR szRetBuf, INT nBufLen, INT* pnRetLen)
{
//	CConsole::Write("execute HVAPI_GetInfo");
	if ( NULL == hHandle 
		|| NULL == szCmd 
		|| NULL == szRetBuf 
		|| nBufLen <= 0 )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT* pHHC = (HVAPI_HANDLE_CONTEXT*)hHandle;
//	CConsole::Write("HVAPI_GetInfo=%s, HVAPI_API_VERSION=%s, version=%s, fNewProtocol = %d", szCmd, HVAPI_API_VERSION, pHHC->szVersion, pHHC->fNewProtocol);

	CCSLock sLock(&g_csConnection);
	map<HVAPI_HANDLE_CONTEXT *,int>::iterator itr = g_connMap.find(pHHC);
	if(itr == g_connMap.end())
	{
//		CConsole::Write("handle 0x%x is not valid.", (int)hHandle);
		return E_FAIL;
	}

	if ( 0 != strcmp(HVAPI_API_VERSION, pHHC->szVersion) )
	{
		return E_FAIL;
	}

	const int iBufLen = (1024 << 4);
	char* szXmlBuf = new char[iBufLen];  // 16KB
	if(szXmlBuf == NULL)
	{
		return E_FAIL;
	}
	int nXmlBufLen = iBufLen;

	TiXmlDocument cXmlDoc;
	if ( !cXmlDoc.Parse(szCmd) )  // 检查szCmd是否是XML
	{
		if ( E_FAIL == HvMakeXmlInfoByString(pHHC->fNewProtocol ? PROTOCOL_VERSION_2 : PROTOCOL_VERSION_1, szCmd, (int)strlen(szCmd), szXmlBuf, nXmlBufLen) )
		{
			SAFE_DELETE(szXmlBuf);
//			CConsole::Write("HvMakeXmlInfoByString=%s, fNewProtocol = %d, error", szCmd, pHHC->fNewProtocol);
			return E_FAIL;
		}

		szCmd = (char*)szXmlBuf;
	}

	bool fRet = ExecXmlExtCmd(pHHC->szIP, (char*)szCmd, (char*)szRetBuf, nBufLen);
	if ( pnRetLen )
	{
		*pnRetLen = nBufLen;
//		CConsole::Write("szRetBuf=%s", szRetBuf);
	}
//	CConsole::Write("fRet=%s", (true == fRet) ? "S_OK" : "E_FAIL");
	SAFE_DELETE(szXmlBuf);
	return (true == fRet) ? S_OK : E_FAIL;
}

// Jpeg数据流接收线程函数
static DWORD WINAPI HvJpegStreamRecvThreadFunc(LPVOID lpParameter)
{
	if ( NULL == lpParameter )
	{
		return -1;
	}
	HVAPI_HANDLE_CONTEXT* pHHC = (HVAPI_HANDLE_CONTEXT*)lpParameter;
	INFO_HEADER cInfoHeader;
	INFO_HEADER cInfoHeaderResponse;
	BLOCK_HEADER cBlockHeader;
	unsigned char* pbInfo = NULL;
	unsigned char* pbData = NULL;
//	CBuffer infoBuffer, dataBuffer;
	cInfoHeaderResponse.dwType = CAMERA_THROB_RESPONSE;
	cInfoHeaderResponse.dwDataLen = 0;
	cInfoHeaderResponse.dwInfoLen = 0;
	pHHC->dwJpegStreamRecvThreadLastTick = GetTickCount();
//	CConsole::Write("HvJpegStreamRecvThreadFunc starting.");
	while ( !pHHC->fThreadImageExit )
	{
		if ( CONN_STATUS_NORMAL == pHHC->dwImageConnStatus )
		{
			if ( GetTickCount() - pHHC->dwJpegStreamRecvThreadLastTick > 3000 )
			{
				pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
			}
		}
		if ( CONN_STATUS_RECONN == pHHC->dwImageConnStatus
			|| CONN_STATUS_DISCONN == pHHC->dwImageConnStatus )
		{
			Sleep(1000);
			continue;
		}

		if ( sizeof(cInfoHeader) 
			== RecvAll(pHHC->sktImage, (char*)&cInfoHeader, sizeof(cInfoHeader)) )
		{
			SAFE_DELETE(pbInfo);
			SAFE_DELETE(pbData);
//			CConsole::Write("HvJpegStreamRecvThreadFunc:recv cInfoHeader dwType=0x%x, dwDataLen=%d, dwInfoLen=%d", cInfoHeader.dwType, cInfoHeader.dwDataLen, cInfoHeader.dwInfoLen);
			pHHC->dwJpegStreamRecvThreadLastTick = GetTickCount();

			if ( cInfoHeader.dwType == CAMERA_THROB )  // 心跳包
			{
				if(send(pHHC->sktImage, (char*)&cInfoHeaderResponse, sizeof(cInfoHeaderResponse), 0)
					!= sizeof(cInfoHeaderResponse))
				{
					pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
				}
				continue;
			}

			if ( cInfoHeader.dwType == CAMERA_HISTORY_END )  // 结束标志
			{
				pHHC->dwImageConnStatus = CONN_STATUS_RECVDONE;
				break;
			}

			DWORD32 dwBuffLen = 10 * 1024 * 1024;
			if (cInfoHeader.dwInfoLen > dwBuffLen || cInfoHeader.dwDataLen > dwBuffLen)
			{
				pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
				continue;
			}
			pbInfo = new unsigned char[cInfoHeader.dwInfoLen + 1];
			pbData = new unsigned char[cInfoHeader.dwDataLen + 1];
			int ipbInfoSize = cInfoHeader.dwInfoLen + 1;

			if(pbInfo == NULL || pbData == NULL)
			{
				Sleep(100);
				pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
				continue;
			}
			else
			{
				ZeroMemory(pbInfo, cInfoHeader.dwInfoLen + 1);
				ZeroMemory(pbData, cInfoHeader.dwDataLen + 1);
			}
			//infoBuffer.Resize(cInfoHeader.dwInfoLen);
			//dataBuffer.Resize(cInfoHeader.dwDataLen);

			if( cInfoHeader.dwInfoLen > 0 )
			{
				if ( cInfoHeader.dwInfoLen 
					!= RecvAll(pHHC->sktImage, (char*)pbInfo, cInfoHeader.dwInfoLen))//infoBuffer.GetBuffer(), infoBuffer.GetSize()))
				{
					Sleep(100);
					pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
					continue;
				}
			}

			if( cInfoHeader.dwDataLen > 0 )
			{
				if ( cInfoHeader.dwDataLen 
					!= RecvAll(pHHC->sktImage, (char*)pbData, cInfoHeader.dwDataLen))//dataBuffer.GetBuffer(), dataBuffer.GetSize()))
				{
					Sleep(100);
					pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
					continue;
				}

				if( cInfoHeader.dwType == CAMERA_IMAGE ) //Jpeg图片流
				{
					// 数据流附加信息解析
					DWORD32 dwImageType = 0;
					DWORD32 dwImageWidth = 0;
					DWORD32 dwImageHeight = 0;
					DWORD64 dw64ImageTime = 0;
					DWORD32 dwImageOffset = 0;
					ImageExtInfo cImageExtInfo = {0};
					unsigned char* pbTemp = pbInfo;//(unsigned char *)infoBuffer.GetBuffer();
					int iFlag = 0;
					for( int i = 0; i < (int)cInfoHeader.dwInfoLen;)//infoBuffer.GetSize(); )
					{
						if ((ipbInfoSize - i) < sizeof(BLOCK_HEADER))
						{
							iFlag = 1;
							break;
						}
						memcpy(&cBlockHeader,  pbTemp, sizeof(BLOCK_HEADER));
						pbTemp += sizeof(BLOCK_HEADER);
						i += sizeof(BLOCK_HEADER);

						if( cBlockHeader.dwID == BLOCK_IMAGE_TYPE )
						{
							if ((ipbInfoSize - i) < sizeof(dwImageType))
							{
								iFlag = 1;
								break;
							}
							memcpy(&dwImageType, pbTemp, sizeof(dwImageType));
						}
						else if( cBlockHeader.dwID == BLOCK_IMAGE_WIDTH )
						{
							if ((ipbInfoSize - i) < sizeof(dwImageWidth))
							{
								iFlag = 1;
								break;
							}
							memcpy(&dwImageWidth, pbTemp, sizeof(dwImageWidth));
						}
						else if( cBlockHeader.dwID == BLOCK_IMAGE_HEIGHT )
						{
							if ((ipbInfoSize - i) < sizeof(dwImageHeight))
							{
								iFlag = 1;
								break;
							}
							memcpy(&dwImageHeight, pbTemp, sizeof(dwImageHeight));
						}
						else if( cBlockHeader.dwID == BLOCK_IMAGE_TIME )
						{
							memcpy(&dw64ImageTime, pbTemp, sizeof(dw64ImageTime));
						}
						else if( cBlockHeader.dwID == BLOCK_IMAGE_OFFSET )
						{
							if ((ipbInfoSize - i) < sizeof(dwImageOffset))
							{
								iFlag = 1;
								break;
							}
							memcpy(&dwImageOffset, pbTemp, sizeof(dwImageOffset));
						}
						else if( cBlockHeader.dwID == BLOCK_IMAGE_EXT_INFO )
						{
							if ((ipbInfoSize - i) < sizeof(cImageExtInfo))
							{
								iFlag = 1;
								break;
							}
							memcpy(&cImageExtInfo, pbTemp, sizeof(cImageExtInfo));
						}

						pbTemp += cBlockHeader.dwLen;
						i += cBlockHeader.dwLen;
					}

					if (iFlag)
					{
						Sleep(100);
						pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
						continue;
					}

					DWORD dwType = IMAGE_TYPE_UNKNOWN;

					if ( CAMERA_IMAGE_JPEG == dwImageType )
					{
						dwType = IMAGE_TYPE_JPEG_NORMAL;
					}
					else if ( CAMERA_IMAGE_JPEG_CAPTURE == dwImageType )
					{
						dwType = IMAGE_TYPE_JPEG_CAPTURE;
					}
					else if ( CAMERA_IMAGE_JPEG_SLAVE == dwImageType )
					{
						dwType = IMAGE_TYPE_JPEG_LPR;
					}
					CCSLock sLock(&pHHC->csCallbackJpeg);
					if ( pHHC->pFuncCallbackJpeg )
					{
						char* pszVideoExtInfo = new char[128];
						if(pszVideoExtInfo)
						{
							sprintf(pszVideoExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d,FrameTime:%I64u",
								cImageExtInfo.iShutter, cImageExtInfo.iGain, cImageExtInfo.iGainR,
								cImageExtInfo.iGainG, cImageExtInfo.iGainB, dw64ImageTime);
						}
						pHHC->pFuncCallbackJpeg(
							pHHC->pCallbackJpegUserData, 
							pbData+dwImageOffset,//(PBYTE)dataBuffer.GetBuffer()+dwImageOffset, 
							cInfoHeader.dwDataLen-dwImageOffset,//dataBuffer.GetSize()-dwImageOffset, 
							pbData,//(PBYTE)dataBuffer.GetBuffer(),
							dwImageOffset,
							dwType, pszVideoExtInfo);
						SAFE_DELETE(pszVideoExtInfo);
					}
				}
			}
		}
		else
		{
			Sleep(100);
		}
	} // while ( !pHHC->fThreadImageExit )
//	CConsole::Write("HvJpegStreamRecvThreadFunc end..");
	return 0;
}

// H264数据流接收线程函数
static DWORD WINAPI HvH264StreamRecvThreadFunc(LPVOID lpParameter)
{
	if ( NULL == lpParameter )
	{
		return -1;
	}

	HVAPI_HANDLE_CONTEXT* pHHC = (HVAPI_HANDLE_CONTEXT*)lpParameter;
	INFO_HEADER cInfoHeader;
	INFO_HEADER cInfoHeaderResponse;
	BLOCK_HEADER cBlockHeader;
	unsigned char* pbInfo = NULL;
	unsigned char* pbData = NULL;
//	CBuffer infoBuffer, dataBuffer;
	cInfoHeaderResponse.dwType = CAMERA_THROB_RESPONSE;
	cInfoHeaderResponse.dwDataLen = 0;
	cInfoHeaderResponse.dwInfoLen = 0;
	pHHC->dwH264StreamRecvThreadLastTick = GetTickCount();
//	CConsole::Write("HvH264StreamRecvThreadFunc starting.");
	while ( !pHHC->fThreadVideoExit )
	{
		if ( CONN_STATUS_NORMAL == pHHC->dwVideoConnStatus )
		{
			if ( GetTickCount() - pHHC->dwH264StreamRecvThreadLastTick > 3000 )
			{
				pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
			}
		}
		if ( CONN_STATUS_RECONN == pHHC->dwVideoConnStatus
			|| CONN_STATUS_DISCONN == pHHC->dwVideoConnStatus )
		{
			Sleep(1000);
			continue;
		}

		if ( sizeof(cInfoHeader) 
			== RecvAll(pHHC->sktVideo, (char*)&cInfoHeader, sizeof(cInfoHeader)) )
		{
			SAFE_DELETE(pbInfo);
			SAFE_DELETE(pbData);
//			CConsole::Write("HvH264StreamRecvThreadFunc:recv cInfoHeader dwType=0x%x, dwDataLen=%d, dwInfoLen=%d", cInfoHeader.dwType, cInfoHeader.dwDataLen, cInfoHeader.dwInfoLen);
			pHHC->dwH264StreamRecvThreadLastTick = GetTickCount();

			if ( cInfoHeader.dwType == CAMERA_THROB )  // 心跳包
			{
				if(send(pHHC->sktVideo, (char*)&cInfoHeaderResponse, sizeof(cInfoHeaderResponse), 0)
					!= sizeof(cInfoHeaderResponse))
				{
					pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
				}
				continue;
			}
			
			if ( cInfoHeader.dwType == CAMERA_HISTORY_END )  // 结束标志
			{
				pHHC->dwVideoConnStatus = CONN_STATUS_RECVDONE;
				break;
			}
			//infoBuffer.Resize(cInfoHeader.dwInfoLen);
			//dataBuffer.Resize(cInfoHeader.dwDataLen);
			DWORD32 dwBuffLen = 10 * 1024 * 1024;
			if (cInfoHeader.dwInfoLen > dwBuffLen || cInfoHeader.dwDataLen > dwBuffLen)
			{
				pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
				continue;
			}
			
			pbInfo = new unsigned char[cInfoHeader.dwInfoLen + 1];
			pbData = new unsigned char[cInfoHeader.dwDataLen + 1];
			int ipbInfoSize = cInfoHeader.dwInfoLen + 1;
			if( pbInfo == NULL || pbData == NULL )
			{
				Sleep(100);
				pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
				continue;
			}
			else
			{
				ZeroMemory(pbInfo, cInfoHeader.dwInfoLen + 1);
				ZeroMemory(pbData, cInfoHeader.dwDataLen + 1);
			}

			if( cInfoHeader.dwInfoLen > 0 )
			{
				if ( cInfoHeader.dwInfoLen 
					!= RecvAll(pHHC->sktVideo, (char*)pbInfo, cInfoHeader.dwInfoLen))//infoBuffer.GetBuffer(), infoBuffer.GetSize()))
				{
					Sleep(100);
					pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
					continue;
				}
			}

			if( cInfoHeader.dwDataLen > 0 )
			{
				if ( cInfoHeader.dwDataLen 
					!= RecvAll(pHHC->sktVideo, (char*)pbData, cInfoHeader.dwDataLen))//dataBuffer.GetBuffer(), dataBuffer.GetSize()) )
				{
					Sleep(100);
					pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
					continue;
				}

				if( cInfoHeader.dwType == CAMERA_VIDEO ) //视频流
				{
					// 数据流附加信息解析
					DWORD32 dwVideoType = 0;
					DWORD32 dwFrameType = 0;
					DWORD64 dw64VideoTime = 0;
					VideoWidth cVideoWidth = {0};
					VideoHeight cVideoHeight = {0};
					VideoExtInfo cVideoExtInfo = {0};

					unsigned char* pbTemp = pbInfo;//(PBYTE)infoBuffer.GetBuffer();
					int iFlag = 0;
					for( int i = 0; i < (int)cInfoHeader.dwInfoLen;)//infoBuffer.GetSize(); )
					{
						if ((ipbInfoSize - i) < sizeof(BLOCK_HEADER))
						{
							iFlag = 1;
							break;
						}
						memcpy(&cBlockHeader,  pbTemp, sizeof(BLOCK_HEADER));
						pbTemp += sizeof(BLOCK_HEADER);
						i += sizeof(BLOCK_HEADER);

						if( cBlockHeader.dwID == BLOCK_VIDEO_TYPE )
						{
							if ((ipbInfoSize - i) < sizeof(dwVideoType))
							{
								iFlag = 1;
								break;
							}
							memcpy(&dwVideoType, pbTemp, sizeof(dwVideoType));
						}
						else if( cBlockHeader.dwID == BLOCK_FRAME_TYPE )
						{
							if ((ipbInfoSize - i) < sizeof(dwFrameType))
							{
								iFlag = 1;
								break;
							}
							memcpy(&dwFrameType, pbTemp, sizeof(dwFrameType));
						}
						else if ( cBlockHeader.dwID == BLOCK_VIDEO_TIME )
						{
							if ((ipbInfoSize - i) < sizeof(dw64VideoTime))
							{
								iFlag = 1;
								break;
							}
							memcpy(&dw64VideoTime, pbTemp, sizeof(dw64VideoTime));
						}
						else if (cBlockHeader.dwID == BLOCK_VIDEO_WIDTH)
						{
							if ((ipbInfoSize - i) < sizeof(cVideoWidth))
							{
								iFlag = 1;
								break;
							}
							memcpy(&cVideoWidth, pbTemp, sizeof(cVideoWidth));
						}
						else if (cBlockHeader.dwID == BLOCK_VIDEO_HEIGHT)
						{
							if ((ipbInfoSize - i) < sizeof(cVideoHeight))
							{
								iFlag = 1;
								break;
							}
							memcpy(&cVideoHeight, pbTemp, sizeof(cVideoHeight));
						}
						else if ( cBlockHeader.dwID == BLOCK_VIDEO_EXT_INFO )
						{
							if ((ipbInfoSize - i) < sizeof(cVideoExtInfo))
							{
								iFlag = 1;
								break;
							}
							memcpy(&cVideoExtInfo, pbTemp, sizeof(cVideoExtInfo));
						}

						pbTemp += cBlockHeader.dwLen;
						i += cBlockHeader.dwLen;
					}
					if (iFlag)
					{
						Sleep(100);
						pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
						continue;
					}

					if ( CAMERA_VIDEO_H264 == dwVideoType )
					{
						DWORD dwType = VIDEO_TYPE_UNKNOWN;

						if ( CAMERA_FRAME_I == dwFrameType )
						{
							dwType = VIDEO_TYPE_H264_NORMAL_I;
						}
						else if ( CAMERA_FRAME_P == dwFrameType )
						{
							dwType = VIDEO_TYPE_H264_NORMAL_P;
						}
						else if ( CAMERA_FRAME_IP_ONE_SECOND == dwFrameType )
						{
							if(pHHC->pFuncCallbackH264)
							{
								char* pszVideoExtInfo = new char[128];
								if(pszVideoExtInfo)
								{
									sprintf(pszVideoExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d,FrameTime:%I64u,Width:%d,Height:%d",
										cVideoExtInfo.iShutter, cVideoExtInfo.iGain, cVideoExtInfo.iGainR,
										cVideoExtInfo.iGainG, cVideoExtInfo.iGainB, dw64VideoTime, cVideoWidth.iWidth, cVideoHeight.iHight);
								}

								PBYTE pPacketData = pbData;//(PBYTE)dataBuffer.GetBuffer();
								int iTempBufferLem = cInfoHeader.dwDataLen;
								int iFrameLen = 0;
								int iFrameType = 0;
								
								while(iTempBufferLem > 0)
								{
									iFrameType = *(int*)pPacketData;
									pPacketData += 4;
									iFrameLen = *(int*)pPacketData;
									pPacketData += 4;
									if(iFrameType == 4096) dwType = VIDEO_TYPE_H264_HISTORY_I;
									else if(iFrameType == 4097) dwType = VIDEO_TYPE_H264_HISTORY_P;
									CCSLock sLock(&pHHC->csCallbackH264);
									if(pHHC->pFuncCallbackH264)
									{
										pHHC->pFuncCallbackH264(
										pHHC->pCallbackH264UserData, 
										pPacketData, 
										iFrameLen, 
										dwType, pszVideoExtInfo);
									}
									pPacketData += iFrameLen;
									iTempBufferLem = iTempBufferLem - 8 - iFrameLen;
								}
								SAFE_DELETE(pszVideoExtInfo);
								continue;
							}
						}
						CCSLock sLock(&pHHC->csCallbackH264);
						if ( pHHC->pFuncCallbackH264 )
						{
							char* pszVideoExtInfo = new char[128];
							if(pszVideoExtInfo)
							{
								sprintf(pszVideoExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d,FrameTime:%I64u,Width:%d,Height:%d",
									cVideoExtInfo.iShutter, cVideoExtInfo.iGain, cVideoExtInfo.iGainR,
									cVideoExtInfo.iGainG, cVideoExtInfo.iGainB, dw64VideoTime, cVideoWidth.iWidth, cVideoHeight.iHight);
							}
							pHHC->pFuncCallbackH264(
								pHHC->pCallbackH264UserData, 
								pbData,//(PBYTE)dataBuffer.GetBuffer(), 
								cInfoHeader.dwDataLen,//dataBuffer.GetSize(), 
								dwType, pszVideoExtInfo);
							SAFE_DELETE(pszVideoExtInfo);
						}
					}
					else if( CAMERA_VIDEO_JPEG == dwVideoType)
					{
						if(pHHC->pFuncCallbackH264)
						{
							char* pszVideoExtInfo = new char[128];
							if(pszVideoExtInfo)
							{
								sprintf(pszVideoExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d,FrameTime:%I64u,Width:%d,Height:%d",
									cVideoExtInfo.iShutter, cVideoExtInfo.iGain, cVideoExtInfo.iGainR,
									cVideoExtInfo.iGainG, cVideoExtInfo.iGainB, dw64VideoTime, cVideoWidth.iWidth, cVideoHeight.iHight);
							}
							//pHHC->pFuncCallbackH264(pHHC->pCallbackH264UserData, pbData, 
							//	cInfoHeader.dwDataLen, VIDEO_TYPE_JPEG_HISTORY, pszVideoExtInfo);
							PBYTE pPacketData = pbData;//(PBYTE)dataBuffer.GetBuffer();
							int iTempBufferLem = cInfoHeader.dwDataLen;//dataBuffer.GetSize();
							int iFrameLen = 0;
							int iReadedCount = 0;
							int iSize = 0;
							int iRedLightCount = 0;
							char szTempInfo[10] = {0};
							while(iTempBufferLem > 0)
							{
								iSize = *(int*)pPacketData;
								pPacketData += 4;
								memcpy(szTempInfo, pPacketData, 8);
								if(strcmp(szTempInfo, "redlight") == 0)
								{
									pPacketData += 8;
									iRedLightCount = *(int*)pPacketData;
									iFrameLen = 16 + iSize + sizeof(RECT) * iRedLightCount;
									pPacketData = pPacketData + 4 + iSize + (sizeof(RECT)*iRedLightCount);
								}
								else
								{
									iFrameLen = 4 + iSize;
									pPacketData += iSize;
								}
								CCSLock sLock(&pHHC->csCallbackH264);
								if(pHHC->pFuncCallbackH264)
								{
									pHHC->pFuncCallbackH264(pHHC->pCallbackH264UserData, pbData + iReadedCount,//(PBYTE)dataBuffer.GetBuffer() + iReadedCount,
										iFrameLen, VIDEO_TYPE_JPEG_HISTORY, pszVideoExtInfo);
								}
								iReadedCount += iFrameLen;
								iTempBufferLem -= iFrameLen;
							}
							SAFE_DELETE(pszVideoExtInfo);
						}
					}
				}
			}
		}
		else
		{
			Sleep(100);
		}
	} // while ( !pHHC->fThreadVideoExit )

//	CConsole::Write("HvH264StreamRecvThreadFunc end.");
	return 0;
}

// Record数据流接收线程函数
static DWORD WINAPI HvRecordStreamRecvThreadFunc(LPVOID lpParameter)
{
	if ( NULL == lpParameter )
	{
		return -1;
	}

	HVAPI_HANDLE_CONTEXT* pHHC = (HVAPI_HANDLE_CONTEXT*)lpParameter;
	INFO_HEADER cInfoHeader;
	INFO_HEADER cInfoHeaderResponse;
	BLOCK_HEADER cBlockHeader;
	unsigned char* pbInfo = NULL;
	unsigned char* pbData = NULL;
//	CBuffer infoBuffer, dataBuffer, appendBuffer;
	cInfoHeaderResponse.dwType = CAMERA_THROB_RESPONSE;
	cInfoHeaderResponse.dwDataLen = 0;
	cInfoHeaderResponse.dwInfoLen = 0;
	pHHC->dwRecordStreamRecvThreadLastTick = GetTickCount();
//	CConsole::Write("HvRecordStreamRecvThreadFunc starting.");
	while ( !pHHC->fThreadRecordExit )
	{
		if ( CONN_STATUS_NORMAL == pHHC->dwRecordConnStatus )
		{
			if ( GetTickCount() - pHHC->dwRecordStreamRecvThreadLastTick > 3000 )
			{
				pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
			}
		}

		if ( CONN_STATUS_RECONN == pHHC->dwRecordConnStatus
			|| CONN_STATUS_DISCONN == pHHC->dwRecordConnStatus )
		{
			Sleep(1000);
			continue;
		}

		if ( sizeof(cInfoHeader) 
			== RecvAll(pHHC->sktRecord, (char*)&cInfoHeader, sizeof(cInfoHeader)) )
		{
//			CConsole::Write("HvRecordStreamRecvThreadFunc:recv cInfoHeader dwType=0x%x, dwDataLen=%d, dwInfoLen=%d", cInfoHeader.dwType, cInfoHeader.dwDataLen, cInfoHeader.dwInfoLen);
			SAFE_DELETE_ARG(pbInfo);
			SAFE_DELETE_ARG(pbData);

			pHHC->dwRecordStreamRecvThreadLastTick = GetTickCount();

			if ( cInfoHeader.dwType == CAMERA_THROB )  // 心跳包
			{
				if(send(pHHC->sktRecord, (char*)&cInfoHeaderResponse, sizeof(cInfoHeaderResponse), 0) 
					!= sizeof(cInfoHeaderResponse))
				{
					pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
				}
				continue;
			}

			if ( cInfoHeader.dwType == CAMERA_HISTORY_END )  // 结束标志
			{
				pHHC->dwRecordConnStatus = CONN_STATUS_RECVDONE;
				break;
			}

			DWORD32 dwBuffLen = 10 * 1024 * 1024;
			if (cInfoHeader.dwInfoLen > dwBuffLen || cInfoHeader.dwDataLen > dwBuffLen)
			{
				pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
				continue;
			}

			pbInfo = new unsigned char[cInfoHeader.dwInfoLen + 1];
			pbData = new unsigned char[cInfoHeader.dwDataLen + 1];
			int ipbInfoSize = cInfoHeader.dwInfoLen + 1;

			if(pbInfo == NULL || pbData == NULL)
			{
				Sleep(100);
				pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
				continue;
			}
			else
			{
				ZeroMemory(pbInfo, cInfoHeader.dwInfoLen+1);
				ZeroMemory(pbData, cInfoHeader.dwDataLen+1);
			}

			if(cInfoHeader.dwInfoLen > 0)
			{
				if(cInfoHeader.dwInfoLen != RecvAll(pHHC->sktRecord, (char*)pbInfo, cInfoHeader.dwInfoLen))
				{
					Sleep(100);
					pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
					continue;
				}
			}

			//infoBuffer.Resize(cInfoHeader.dwInfoLen);
			//dataBuffer.Resize(cInfoHeader.dwDataLen);
			//if( infoBuffer.GetSize()> 0 )
			//{
			//	if ( cInfoHeader.dwInfoLen 
			//		!= RecvAll(pHHC->sktRecord, infoBuffer.GetBuffer(), infoBuffer.GetSize()) )
			//	{
			//		Sleep(100);
			//		pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
			//		continue;
			//	}
			//}

			//if( dataBuffer.GetSize() > 0 )
			if(cInfoHeader.dwDataLen > 0)
			{
				//if ( cInfoHeader.dwDataLen 
				//	!= RecvAll(pHHC->sktRecord, dataBuffer.GetBuffer(), dataBuffer.GetSize()) )
				if(cInfoHeader.dwDataLen
					!= RecvAll(pHHC->sktRecord, (char*)pbData, cInfoHeader.dwDataLen))
				{
					Sleep(100);
					pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
					continue;
				}

				if( cInfoHeader.dwType == CAMERA_RECORD ) //识别结果流
				{
					// 数据流附加信息解析
					DWORD32 dwRecordType = 0;
					char* pszAppendInfo = new char[1024<<5];

					unsigned char* pbTemp = pbInfo;//(PBYTE)infoBuffer.GetBuffer();
					int iFlag = 0;
					for( int i = 0; i < (int)cInfoHeader.dwInfoLen;)//infoBuffer.GetSize(); )
					{
						if ((ipbInfoSize - i) < sizeof(BLOCK_HEADER))
						{
							iFlag = 1;
							break;
						}
						memcpy(&cBlockHeader, pbTemp, sizeof(BLOCK_HEADER));
						pbTemp += sizeof(BLOCK_HEADER);
						i += sizeof(BLOCK_HEADER);

						if ( cBlockHeader.dwID == BLOCK_RECORD_TYPE )  // 记录集类型
						{
							if ( sizeof(dwRecordType) == cBlockHeader.dwLen )
							{
								if ((ipbInfoSize - i) < (int)cBlockHeader.dwLen)
								{
									iFlag = 1;
									break;
								}
								memcpy(&dwRecordType, pbTemp, cBlockHeader.dwLen);
							}
						}
						else if ( cBlockHeader.dwID == BLOCK_XML_TYPE )  // 识别结果XML附加信息
						{
							//appendBuffer.Resize(cBlockHeader.dwLen);
							//memcpy(appendBuffer.GetBuffer(), pbTemp, cBlockHeader.dwLen);
							if(cBlockHeader.dwLen <= (1024<<5))
							{
								if ((ipbInfoSize - i) < (int)cBlockHeader.dwLen)
								{
									iFlag = 1;
									break;
								}
								memcpy(pszAppendInfo, pbTemp, cBlockHeader.dwLen);
							}
						}

						pbTemp += cBlockHeader.dwLen;
						i += cBlockHeader.dwLen;
					}

					if (iFlag)
					{
						Sleep(100);
						pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
						continue;
					}

					DWORD dwType = RECORD_TYPE_UNKNOWN;

					if ( CAMERA_RECORD_NORMAL == dwRecordType )
					{
						dwType = RECORD_TYPE_NORMAL;
					}
					else if ( CAMERA_RECORD_HISTORY == dwRecordType )
					{
						dwType = RECORD_TYPE_HISTORY;
					}
					else if ( CAMERA_RECORD_STRING == dwRecordType )
					{
						dwType = RECORD_TYPE_STRING;
					}
					else if ( CAMERA_RECORD_INLEFT == dwRecordType )
					{
						dwType = RECORD_TYPE_INLEFT;
					}
					CCSLock sLock(&pHHC->csCallbackRecord);
					if ( pHHC->pFuncCallbackRecord )
					{
						pHHC->pFuncCallbackRecord(
							pHHC->pCallbackRecordUserData, 
							pbData,//(PBYTE)dataBuffer.GetBuffer(), 
							cInfoHeader.dwDataLen,//dataBuffer.GetSize(), 
							dwType, pszAppendInfo);//appendBuffer.GetBuffer());
					}
					SAFE_DELETE(pszAppendInfo);
				}
				else if(cInfoHeader.dwType == CAMERA_STRING)
				{
					CCSLock sLock(&pHHC->csCallbackGatherInfo);
					if(pHHC->pFuncCallbackGatherInfo)
					{
						pHHC->pFuncCallbackGatherInfo(pHHC->pCallbackGatherInfoData, pbData, cInfoHeader.dwDataLen);//(PBYTE)dataBuffer.GetBuffer(), dataBuffer.GetSize());
					}
				}
			}
			else
			{
				if( cInfoHeader.dwType == CAMERA_RECORD ) //识别结果流
				{
					// 数据流附加信息解析
					DWORD32 dwRecordType = 0;
					char* pszAppendInfo = new char[1024<<5];
					unsigned char* pbTemp = pbInfo;//(PBYTE)infoBuffer.GetBuffer();
					int iFlag = 0;
					for( int i = 0; i < (int)cInfoHeader.dwInfoLen;)//infoBuffer.GetSize(); )
					{
						if ((ipbInfoSize - i) < sizeof(BLOCK_HEADER))
						{
							iFlag = 1;
							break;
						}
						memcpy(&cBlockHeader, pbTemp, sizeof(BLOCK_HEADER));
						pbTemp += sizeof(BLOCK_HEADER);
						i += sizeof(BLOCK_HEADER);

						if ( cBlockHeader.dwID == BLOCK_RECORD_TYPE )  // 记录集类型
						{
							if ( sizeof(dwRecordType) == cBlockHeader.dwLen )
							{
								if ((ipbInfoSize - i) < (int)cBlockHeader.dwLen)
								{
									iFlag = 1;
									break;
								}
								memcpy(&dwRecordType, pbTemp, cBlockHeader.dwLen);
							}
						}
						else if ( cBlockHeader.dwID == BLOCK_XML_TYPE )  // 识别结果XML附加信息
						{
							//appendBuffer.Resize(cBlockHeader.dwLen);
							//memcpy(appendBuffer.GetBuffer(), pbTemp, cBlockHeader.dwLen);
							if(cBlockHeader.dwLen <= sizeof(pszAppendInfo))
							{
								if ((ipbInfoSize - i) < (int)cBlockHeader.dwLen)
								{
									iFlag = 1;
									break;
								}
								memcpy(pszAppendInfo, pbTemp, cBlockHeader.dwLen);
							}
						}

						pbTemp += cBlockHeader.dwLen;
						i += cBlockHeader.dwLen;
					}
					if (iFlag)
					{
						Sleep(100);
						pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
						continue;
					}

					DWORD dwType = RECORD_TYPE_UNKNOWN;

					if ( CAMERA_RECORD_NORMAL == dwRecordType )
					{
						dwType = RECORD_TYPE_NORMAL;
					}
					else if ( CAMERA_RECORD_HISTORY == dwRecordType )
					{
						dwType = RECORD_TYPE_HISTORY;
					}
					else if ( CAMERA_RECORD_STRING == dwRecordType )
					{
						dwType = RECORD_TYPE_STRING;
					}
					else if ( CAMERA_RECORD_INLEFT == dwRecordType )
					{
						dwType = RECORD_TYPE_INLEFT;
					}
//					CConsole::Write("send result data to callback function.");
					CCSLock sLock(&pHHC->csCallbackRecord);
					if ( pHHC->pFuncCallbackRecord )
					{
						pHHC->pFuncCallbackRecord(
							pHHC->pCallbackRecordUserData, 
							NULL, 
							0, 
							dwType, pszAppendInfo);//appendBuffer.GetBuffer());
					}
					SAFE_DELETE(pszAppendInfo);
				}
			}
		}
		else
		{
			pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
			Sleep(100);
		}
	} // while ( !pHHC->fThreadRecordExit )

//	CConsole::Write("HvRecordStreamRecvThreadFunc end..");
	return 0;
}

HV_API HRESULT CDECL HVAPI_SetCallBack(HVAPI_HANDLE hHandle, PVOID pFunc, PVOID pUserData, INT nStreamType, LPCSTR szConnCmd)
{
	if ( NULL == hHandle )
	{
//		CConsole::Write("HVAPI_SetCallBack return E_FAIL");
		return E_FAIL;
	}

//	CConsole::Write("HVAPI_SetCallBack, hHandle=0x%x, nStreamType=0x%x, pFunc = 0x%x, szConnCmd = %s", (int)hHandle, nStreamType, (int)pFunc, szConnCmd);

	HVAPI_HANDLE_CONTEXT* pHHC = (HVAPI_HANDLE_CONTEXT*)hHandle;

	CCSLock sLock(&g_csConnection);
	map<HVAPI_HANDLE_CONTEXT *,int>::iterator itr = g_connMap.find(pHHC);
	if(itr == g_connMap.end())
	{
//		CConsole::Write("handle 0x%x is not valid.", (int)hHandle);
		return E_FAIL;
	}
	sLock.Unlock();

	if ( 0 != strcmp(HVAPI_API_VERSION, pHHC->szVersion) )
	{
//		CConsole::Write("HVAPI_SetCallBack return E_FAIL");
		return E_FAIL;
	}

	if ( STREAM_TYPE_IMAGE == nStreamType )
	{
		HVAPI_CALLBACK_IMAGE pTmp = (HVAPI_CALLBACK_IMAGE)pFunc;
		if ( pTmp != NULL )
		{
			CCSLock sLock(&pHHC->csCallbackJpeg);
			pHHC->pFuncCallbackJpeg = pTmp;
			if ( ConnectCamera(pHHC->szIP, CAMERA_IMAGE_LINK_PORT, pHHC->sktImage, 10) )
			{
				if ( szConnCmd != NULL && sizeof(pHHC->szImageConnCmd) > strlen(szConnCmd) )
				{
					strcpy((char*)pHHC->szImageConnCmd, szConnCmd);

					char szRetBuf[256] = {0};
					if ( S_OK != HvSendXmlCmd(pHHC->szIP, (char*)pHHC->szImageConnCmd, szRetBuf, sizeof(szRetBuf), NULL, pHHC->sktImage) )
					{
						ForceCloseSocket(pHHC->sktImage);
						pHHC->pFuncCallbackJpeg = NULL;
//						CConsole::Write("HVAPI_SetCallBack return E_FAIL");
						return E_FAIL;
					}
				}

				pHHC->pCallbackJpegUserData = pUserData;
				pHHC->fThreadImageExit = FALSE;
				pHHC->hThreadRecvImage = CreateThread(NULL, 0, HvJpegStreamRecvThreadFunc, pHHC, 0, NULL);

				pHHC->dwImageConnStatus = CONN_STATUS_NORMAL;
			}
			else
			{
				pHHC->pFuncCallbackJpeg = NULL;
//				CConsole::Write("HVAPI_SetCallBack return E_FAIL");
				return E_FAIL;
			}
		}
		else
		{
			pHHC->fThreadImageExit = TRUE;
			HvSafeCloseThread(pHHC->hThreadRecvImage);			
			ForceCloseSocket(pHHC->sktImage);
			CCSLock sLock(&pHHC->csCallbackJpeg);
			pHHC->dwImageConnStatus = CONN_STATUS_DISCONN;
			pHHC->szImageConnCmd[0] = '\0';
			pHHC->pCallbackJpegUserData = NULL;
			pHHC->pFuncCallbackJpeg = NULL;
		}
//		CConsole::Write("HVAPI_SetCallBack return S_OK");
		return S_OK;
	}
	else if ( STREAM_TYPE_VIDEO == nStreamType )
	{
		HVAPI_CALLBACK_VIDEO pTmp = (HVAPI_CALLBACK_VIDEO)pFunc;
		if ( pTmp != NULL )
		{
			CCSLock sLock(&pHHC->csCallbackH264);
			pHHC->pFuncCallbackH264 = pTmp;
			if ( ConnectCamera(pHHC->szIP, CAMERA_VIDEO_LINK_PORT, pHHC->sktVideo, 10) )
			{
				if ( szConnCmd != NULL && sizeof(pHHC->szVideoConnCmd) > strlen(szConnCmd) )
				{
					strcpy((char*)pHHC->szVideoConnCmd, szConnCmd);

					char szRetBuf[256] = {0};
					if ( S_OK != HvSendXmlCmd(pHHC->szIP, (char*)pHHC->szVideoConnCmd, szRetBuf, sizeof(szRetBuf), NULL, pHHC->sktVideo) )
					{
						ForceCloseSocket(pHHC->sktVideo);
						pHHC->pFuncCallbackH264 = NULL;
//						CConsole::Write("HVAPI_SetCallBack return E_FAIL");
						return E_FAIL;
					}
				}

				pHHC->pCallbackH264UserData = pUserData;
				pHHC->fThreadVideoExit = FALSE;
				pHHC->hThreadRecvVideo = CreateThread(NULL, 0, HvH264StreamRecvThreadFunc, pHHC, 0, NULL);

				pHHC->dwVideoConnStatus = CONN_STATUS_NORMAL;
			}
			else
			{
				pHHC->pFuncCallbackH264 = NULL;
//				CConsole::Write("HVAPI_SetCallBack return E_FAIL");
				return E_FAIL;
			}
		}
		else
		{
			pHHC->fThreadVideoExit = TRUE;
			HvSafeCloseThread(pHHC->hThreadRecvVideo);
			ForceCloseSocket(pHHC->sktVideo);
			CCSLock sLock(&pHHC->csCallbackH264);
			pHHC->dwVideoConnStatus = CONN_STATUS_DISCONN;
			pHHC->szVideoConnCmd[0] = '\0';
			pHHC->pCallbackH264UserData = NULL;
			pHHC->pFuncCallbackH264 = NULL;
		}
//		CConsole::Write("HVAPI_SetCallBack return S_OK");
		return S_OK;
	}
	else if ( STREAM_TYPE_RECORD == nStreamType )
	{
		HVAPI_CALLBACK_RECORD pTmp = (HVAPI_CALLBACK_RECORD)pFunc;
		if ( pTmp != NULL )
		{
			CCSLock sLock(&pHHC->csCallbackRecord);
			pHHC->pFuncCallbackRecord = pTmp;
			if ( ConnectCamera(pHHC->szIP, CAMERA_RECORD_LINK_PORT, pHHC->sktRecord, 10) )
			{
				if ( szConnCmd != NULL && sizeof(pHHC->szRecordConnCmd) > strlen(szConnCmd) )
				{
					strcpy((char*)pHHC->szRecordConnCmd, szConnCmd);

					char szRetBuf[256] = {0};
					if ( S_OK != HvSendXmlCmd(pHHC->szIP, (char*)pHHC->szRecordConnCmd, szRetBuf, sizeof(szRetBuf), NULL, pHHC->sktRecord) )
					{
//						CConsole::Write("HvSendXmlCmd error.");
						ForceCloseSocket(pHHC->sktRecord);
						pHHC->pFuncCallbackRecord = NULL;
//						CConsole::Write("HVAPI_SetCallBack return E_FAIL");
						return E_FAIL;
					}
				}

				pHHC->pCallbackRecordUserData = pUserData;
				pHHC->fThreadRecordExit = FALSE;
				pHHC->hThreadRecvRecord = CreateThread(NULL, 0, HvRecordStreamRecvThreadFunc, pHHC, 0, NULL);

				pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
			}
			else
			{
//				CConsole::Write("STREAM_TYPE_RECORD,ConnectCamera IP=%s, failed", pHHC->szIP);
				pHHC->pFuncCallbackRecord = NULL;
//				CConsole::Write("HVAPI_SetCallBack return E_FAIL");
				return E_FAIL;
			}
		}
		else
		{
			pHHC->fThreadRecordExit = TRUE;
			HvSafeCloseThread(pHHC->hThreadRecvRecord);			
			ForceCloseSocket(pHHC->sktRecord);
			CCSLock sLock(&pHHC->csCallbackRecord);
			pHHC->dwRecordConnStatus = CONN_STATUS_DISCONN;
			pHHC->szRecordConnCmd[0] = '\0';
			pHHC->pCallbackRecordUserData = NULL;
			pHHC->pFuncCallbackRecord = NULL;
		}
//		CConsole::Write("HVAPI_SetCallBack return S_OK");
		return S_OK;
	}
	else if(STREAM_TYPE_GATHER_INFO == nStreamType)
	{
		CCSLock sLock(&pHHC->csCallbackGatherInfo);
		pHHC->pFuncCallbackGatherInfo = (HVAPI_CALLBACK_GATHER_INFO)pFunc;
		pHHC->pCallbackGatherInfoData = pUserData;
		return S_OK;
	}
//	CConsole::Write("HVAPI_SetCallBack return E_FAIL");
	return E_FAIL;
}

HV_API HRESULT CDECL HVAPI_UpdateCallBackCmd(HVAPI_HANDLE hHandle, INT nStreamType, LPCSTR szConnCmd)
{
//	CConsole::Write("execute HVAPI_UpdateCallBackCmd");
	if(hHandle == NULL || szConnCmd == NULL)
	{
		return E_HANDLE;
	}
	HVAPI_HANDLE_CONTEXT* pHHC = (HVAPI_HANDLE_CONTEXT*)hHandle;

	CCSLock sLock(&g_csConnection);
	map<HVAPI_HANDLE_CONTEXT *,int>::iterator itr = g_connMap.find(pHHC);
	if(itr == g_connMap.end())
	{
//		CConsole::Write("handle 0x%x is not valid.", (int)hHandle);
		return E_FAIL;
	}

	if(STREAM_TYPE_IMAGE == nStreamType)
	{
		strcpy((char*)pHHC->szImageConnCmd, szConnCmd);
	}
	else if(STREAM_TYPE_VIDEO == nStreamType)
	{
		strcpy((char*)pHHC->szVideoConnCmd, szConnCmd);
	}
	else if(STREAM_TYPE_RECORD == nStreamType)
	{
		strcpy((char*)pHHC->szRecordConnCmd, szConnCmd);
	}
	return S_OK;
}

HV_API HRESULT CDECL HVAPI_GetConnStatus(HVAPI_HANDLE hHandle, INT nStreamType, DWORD* pdwConnStatus)
{
	if ( NULL == hHandle || NULL == pdwConnStatus )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT* pHHC = (HVAPI_HANDLE_CONTEXT*)hHandle;

	CCSLock sLock(&g_csConnection);
	map<HVAPI_HANDLE_CONTEXT *,int>::iterator itr = g_connMap.find(pHHC);
	if(itr == g_connMap.end())
	{
//		CConsole::Write("handle 0x%x is not valid.", (int)hHandle);
		return E_FAIL;
	}

	if ( 0 != strcmp(HVAPI_API_VERSION, pHHC->szVersion) )
	{
		return E_FAIL;
	}

	if ( STREAM_TYPE_IMAGE == nStreamType )
	{
		*pdwConnStatus = pHHC->dwImageConnStatus;
	}
	else if ( STREAM_TYPE_VIDEO == nStreamType )
	{
		*pdwConnStatus = pHHC->dwVideoConnStatus;
	}
	else if ( STREAM_TYPE_RECORD == nStreamType )
	{
		*pdwConnStatus = pHHC->dwRecordConnStatus;
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

// --------------------------- 识别结果图片解析工具函数 ---------------------------

HV_API HRESULT CDECL HVAPIUTILS_GetPlateString(LPCSTR szResultInfo, LPSTR szPlateBuf, INT nBufLen)
{
//	CConsole::Write("execute HVAPIUTILS_GetPlateString");
	if ( NULL == szResultInfo || NULL == szPlateBuf )
	{
		return E_FAIL;
	}

	TiXmlDocument cXmlDoc;
	if ( cXmlDoc.Parse(szResultInfo) )
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
						if (NULL != pElementPlateName->GetText() && nBufLen > (int)strlen(pElementPlateName->GetText()) )
						{
							strcpy(szPlateBuf, pElementPlateName->GetText());
							return S_OK;
						}
					}
				}
			}
		}
	}
	return E_FAIL;
}

// -------------------- HvPciLinkApi.h 部分 [START] --------------------

typedef RECT HV_RECT;

// PCI图片类型
typedef enum
{
    PCILINK_IMAGE_BEST_SNAPSHOT = 0,
    PCILINK_IMAGE_LAST_SNAPSHOT,
    PCILINK_IMAGE_BEGIN_CAPTURE,
    PCILINK_IMAGE_BEST_CAPTURE,
    PCILINK_IMAGE_LAST_CAPTURE,
    PCILINK_IMAGE_SMALL_IMAGE,
    PCILINK_IMAGE_BIN_IMAGE
}
PCILINK_IMAGE_TYPE;

// 图片信息结构体
typedef struct tag_PciImage_Info
{
    DWORD32 dwCarID;
    DWORD32 dwImgType;
    DWORD32 dwImgWidth;
    DWORD32 dwImgHeight;
    DWORD32 dwTimeLow;
    DWORD32 dwTimeHigh;
    DWORD32 dwEddyType;
    HV_RECT rcPlate;
    HV_RECT rcRedLightPos[20];
    int nRedLightCount;
	RECT rcFacePos[20];
	int nFaceCount;

    tag_PciImage_Info()
    {
        memset(this, 0, sizeof(*this));
    }
}
PCI_IMAGE_INFO;

// -------------------- HvPciLinkApi.h 部分 [END] --------------------

HV_API HRESULT CDECL HVAPIUTILS_GetRecordImage(PBYTE pbResultPacket, DWORD dwPacketLen, RECORD_IMAGE_TYPE eImageType, RECORD_IMAGE* pcRecordImage)
{
//	CConsole::Write("execute HVAPIUTILS_GetRecordImage");
	HRESULT hr = E_FAIL;

	PCI_IMAGE_INFO cImgInfo;
	DWORD dwImgInfoLen;
	DWORD dwImgDataLen;

	PBYTE pbTemp = pbResultPacket;
	PBYTE pbTempInfo = NULL;
	DWORD dwRemainLen = dwPacketLen;

	if(pbResultPacket == NULL || dwPacketLen <= 0)
	{
		return E_FAIL;
	}

	while ( dwRemainLen > 0 )
	{
		if(dwRemainLen < 4)
		{
			return E_FAIL;
		}
		memcpy(&dwImgInfoLen, pbTemp, 4);
		pbTemp += 4;
		if(dwImgInfoLen > dwRemainLen-4)
		{
			return E_FAIL;
		}

		int iBuffLen;
		iBuffLen = (sizeof(PCI_IMAGE_INFO) < dwImgInfoLen) ? sizeof(PCI_IMAGE_INFO) : dwImgInfoLen;

		memcpy(&cImgInfo, pbTemp, iBuffLen);
		pbTempInfo = pbTemp;
		pbTemp += dwImgInfoLen;
		if(dwRemainLen-dwImgInfoLen-4 < 4)
		{
			return E_FAIL;
		}

		memcpy(&dwImgDataLen, pbTemp, 4);
		pbTemp += 4;
		if(dwRemainLen-dwImgInfoLen-8 < dwImgDataLen)
		{
			return E_FAIL;
		}

		if ( eImageType == cImgInfo.dwImgType )
		{
			if ( pcRecordImage != NULL )
			{
				pcRecordImage->cImgInfo.dwCarID = cImgInfo.dwCarID;
				pcRecordImage->cImgInfo.dwHeight = cImgInfo.dwImgHeight;
				pcRecordImage->cImgInfo.dwWidth = cImgInfo.dwImgWidth;
				pcRecordImage->cImgInfo.dw64TimeMs = cImgInfo.dwTimeHigh;
				pcRecordImage->cImgInfo.dw64TimeMs <<= 32;
				pcRecordImage->cImgInfo.dw64TimeMs |= cImgInfo.dwTimeLow;

				pcRecordImage->pbImgData = pbTemp;
				pcRecordImage->pbImgInfoEx = pbTempInfo;
				pcRecordImage->dwImgDataLen = dwImgDataLen;
				pcRecordImage->dwImgInfoLen = dwImgInfoLen;
				hr = S_OK;
			}

			break;
		}
		else
		{
			pbTemp += dwImgDataLen;
		}

		dwRemainLen -= (4 + 4 + dwImgInfoLen + dwImgDataLen);
	}
	

	return hr;
}

HV_API HRESULT CDECL HVAPIUTILS_GetRecordImageAll(PBYTE pbResultPacket, DWORD dwPacketLen, RECORD_IMAGE_GROUP* pcRecordImageGroup)
{
//	CConsole::Write("execute HVAPIUTILS_GetRecordImageAll");
	PCI_IMAGE_INFO cImgInfo;
	DWORD dwImgInfoLen;
	DWORD dwImgDataLen;
	PBYTE pbTemp = pbResultPacket;
	PBYTE pbTempInfo = NULL;
	DWORD dwRemainLen = dwPacketLen;
	if(!pbResultPacket || !pcRecordImageGroup || dwPacketLen <= 0)
	{
		return E_FAIL;
	}
	memset(pcRecordImageGroup, 0, sizeof(RECORD_IMAGE_GROUP));
	while(dwRemainLen > 0)
	{
		memcpy(&dwImgInfoLen, pbTemp, 4);
		pbTemp += 4;
		if(dwImgInfoLen > dwRemainLen)
		{
			return E_FAIL;
		}
		memcpy(&cImgInfo, pbTemp, dwImgInfoLen);
		pbTempInfo = pbTemp;
		pbTemp += dwImgInfoLen;

		memcpy(&dwImgDataLen, pbTemp, 4);
		pbTemp += 4;
		if(dwImgDataLen > dwRemainLen)
		{
			return E_FAIL;
		}
		switch(cImgInfo.dwImgType)
		{
		case RECORD_IMAGE_BEST_SNAPSHOT:
			pcRecordImageGroup->cImgInfoBestSnapshot.dwCarID = cImgInfo.dwCarID;
			pcRecordImageGroup->cImgInfoBestSnapshot.dwHeight = cImgInfo.dwImgHeight;
			pcRecordImageGroup->cImgInfoBestSnapshot.dwWidth = cImgInfo.dwImgWidth;
			pcRecordImageGroup->cImgInfoBestSnapshot.dw64TimeMs = cImgInfo.dwTimeHigh;
			pcRecordImageGroup->cImgInfoBestSnapshot.dw64TimeMs <<= 32;
			pcRecordImageGroup->cImgInfoBestSnapshot.dw64TimeMs |= cImgInfo.dwTimeLow;
			pcRecordImageGroup->pbImgDataBestSnapShot = pbTemp;
			pcRecordImageGroup->pbImgInfoBestSnapShot = pbTempInfo;
			pcRecordImageGroup->dwImgDataBestSnapShotLen = dwImgDataLen;
			pcRecordImageGroup->dwImgInfoBestSnapShotLen = dwImgInfoLen;
			break;
		case RECORD_IMAGE_LAST_SNAPSHOT:
			pcRecordImageGroup->cImgInfoLastSnapshot.dwCarID = cImgInfo.dwCarID;
			pcRecordImageGroup->cImgInfoLastSnapshot.dwHeight = cImgInfo.dwImgHeight;
			pcRecordImageGroup->cImgInfoLastSnapshot.dwWidth = cImgInfo.dwImgWidth;
			pcRecordImageGroup->cImgInfoLastSnapshot.dw64TimeMs = cImgInfo.dwTimeHigh;
			pcRecordImageGroup->cImgInfoLastSnapshot.dw64TimeMs <<= 32;
			pcRecordImageGroup->cImgInfoLastSnapshot.dw64TimeMs |= cImgInfo.dwTimeLow;
			pcRecordImageGroup->pbImgDataLastSnapShot = pbTemp;
			pcRecordImageGroup->pbImgInfoLastSnapShot = pbTempInfo;
			pcRecordImageGroup->dwImgDataLastSnapShotLen = dwImgDataLen;
			pcRecordImageGroup->dwImgInfoLastSnapShotLen = dwImgInfoLen;
			break;
		case RECORD_IMAGE_BEGIN_CAPTURE:
			pcRecordImageGroup->cImgInfoBeginCapture.dwCarID = cImgInfo.dwCarID;
			pcRecordImageGroup->cImgInfoBeginCapture.dwHeight = cImgInfo.dwImgHeight;
			pcRecordImageGroup->cImgInfoBeginCapture.dwWidth = cImgInfo.dwImgWidth;
			pcRecordImageGroup->cImgInfoBeginCapture.dw64TimeMs = cImgInfo.dwTimeHigh;
			pcRecordImageGroup->cImgInfoBeginCapture.dw64TimeMs <<= 32;
			pcRecordImageGroup->cImgInfoBeginCapture.dw64TimeMs |= cImgInfo.dwTimeLow;
			pcRecordImageGroup->pbImgDataBeginCapture = pbTemp;
			pcRecordImageGroup->pbImgInfoBeginCapture = pbTempInfo;
			pcRecordImageGroup->dwImgDataBeginCaptureLen = dwImgDataLen;
			pcRecordImageGroup->dwImgInfoBeginCaptureLen = dwImgInfoLen;
			break;
		case RECORD_IMAGE_BEST_CAPTURE:
			pcRecordImageGroup->cImgInfoBestCapture.dwCarID = cImgInfo.dwCarID;
			pcRecordImageGroup->cImgInfoBestCapture.dwHeight = cImgInfo.dwImgHeight;
			pcRecordImageGroup->cImgInfoBestCapture.dwWidth = cImgInfo.dwImgWidth;
			pcRecordImageGroup->cImgInfoBestCapture.dw64TimeMs = cImgInfo.dwTimeHigh;
			pcRecordImageGroup->cImgInfoBestCapture.dw64TimeMs <<= 32;
			pcRecordImageGroup->cImgInfoBestCapture.dw64TimeMs |= cImgInfo.dwTimeLow;
			pcRecordImageGroup->pbImgDataBestCapture = pbTemp;
			pcRecordImageGroup->pbImgInfoBestCapture = pbTempInfo;
			pcRecordImageGroup->dwImgDataBestCaptureLen = dwImgDataLen;
			pcRecordImageGroup->dwImgInfoBestCaptureLen = dwImgInfoLen;
			break;
		case RECORD_IMAGE_LAST_CAPTURE:
			pcRecordImageGroup->cImgInfoLastCapture.dwCarID = cImgInfo.dwCarID;
			pcRecordImageGroup->cImgInfoLastCapture.dwHeight = cImgInfo.dwImgHeight;
			pcRecordImageGroup->cImgInfoLastCapture.dwWidth = cImgInfo.dwImgWidth;
			pcRecordImageGroup->cImgInfoLastCapture.dw64TimeMs = cImgInfo.dwTimeHigh;
			pcRecordImageGroup->cImgInfoLastCapture.dw64TimeMs <<= 32;
			pcRecordImageGroup->cImgInfoLastCapture.dw64TimeMs |= cImgInfo.dwTimeLow;
			pcRecordImageGroup->pbImgDataLastCapture = pbTemp;
			pcRecordImageGroup->pbImgInfoLastCapture = pbTempInfo;
			pcRecordImageGroup->dwImgDataLastCaptureLen = dwImgDataLen;
			pcRecordImageGroup->dwImgInfoLastCaptureLen = dwImgInfoLen;
			break;
		case RECORD_IMAGE_SMALL_IMAGE:
			pcRecordImageGroup->cImgInfoSmaller.dwCarID = cImgInfo.dwCarID;
			pcRecordImageGroup->cImgInfoSmaller.dwHeight = cImgInfo.dwImgHeight;
			pcRecordImageGroup->cImgInfoSmaller.dwWidth = cImgInfo.dwImgWidth;
			pcRecordImageGroup->cImgInfoSmaller.dw64TimeMs = cImgInfo.dwTimeHigh;
			pcRecordImageGroup->cImgInfoSmaller.dw64TimeMs <<= 32;
			pcRecordImageGroup->cImgInfoSmaller.dw64TimeMs |= cImgInfo.dwTimeLow;
			pcRecordImageGroup->pbImgDataSmaller = pbTemp;
			pcRecordImageGroup->pbImgInfoSmaller = pbTempInfo;
			pcRecordImageGroup->dwImgDataSmallerLen = dwImgDataLen;
			pcRecordImageGroup->dwImgInfoSmallerLen = dwImgInfoLen;
			break;
		case RECORD_IMAGE_BIN_IMAGE:
			pcRecordImageGroup->cImgInfoBinary.dwCarID = cImgInfo.dwCarID;
			pcRecordImageGroup->cImgInfoBinary.dwHeight = cImgInfo.dwImgHeight;
			pcRecordImageGroup->cImgInfoBinary.dwWidth = cImgInfo.dwImgWidth;
			pcRecordImageGroup->cImgInfoBinary.dw64TimeMs = cImgInfo.dwTimeHigh;
			pcRecordImageGroup->cImgInfoBinary.dw64TimeMs <<= 32;
			pcRecordImageGroup->cImgInfoBinary.dw64TimeMs |= cImgInfo.dwTimeLow;
			pcRecordImageGroup->pbImgDataBinary = pbTemp;
			pcRecordImageGroup->pbImgInfoBinary = pbTempInfo;
			pcRecordImageGroup->dwImgDataBinaryLen = dwImgDataLen;
			pcRecordImageGroup->dwImgInfoBinaryLen = dwImgInfoLen;
			break;
		default:
			break;
		}
		pbTemp += dwImgDataLen;
		dwRemainLen -= (8 + dwImgInfoLen + dwImgDataLen);
	}
	return S_OK;
}

HV_API HRESULT CDECL HVAPIUTILS_ParseHistoryVideoFrame(PBYTE pbVideoFrameData, DWORD dwFrameLen, int& iRedLightCount,
													   int iRedLightBufLen, PBYTE pbRedLightPos, PBYTE& pbJPEGData, DWORD& dwJPEGDataLen)
{
//	CConsole::Write("execute HVAPIUTILS_ParseHistoryVideoFrame");
	if(pbVideoFrameData == NULL || dwFrameLen <= 0) return E_FAIL;
	if(iRedLightBufLen <= 0 || pbRedLightPos == NULL) return E_FAIL;

	PBYTE pbTemp = pbVideoFrameData;
	int iSize = 0;
	char szBuf[10] = {0};
	iSize = *(int*)pbTemp;
	pbTemp += 4;
	memcpy(szBuf, pbTemp, 8);
	if(strcmp(szBuf, "redlight") != 0)
	{
		iRedLightCount = 0;
		memset(pbRedLightPos, 0, iRedLightBufLen);
		pbJPEGData = pbVideoFrameData + 4;
		dwJPEGDataLen = iSize;
		return S_OK;
	}
	pbTemp += 8;
	iRedLightCount = *(int*)pbTemp;
	if((int)(iRedLightCount*sizeof(RECT)) > iRedLightBufLen)
	{
		iRedLightCount = 0;
		return E_FAIL;
	}
	pbTemp += 4;
	memcpy(pbRedLightPos, pbTemp, iRedLightCount*sizeof(RECT));
	pbJPEGData = pbVideoFrameData + 16 + iRedLightCount*sizeof(RECT);
	dwJPEGDataLen = iSize;
	return S_OK;
}

HV_API HRESULT CDECL HVAPIUTILS_SmallImageToBitmap(PBYTE pbSmallImageData, INT nSmallImageWidth, INT nSmallImageHeight, PBYTE pbBitmapData, INT* pnBitmapDataLen)
{
//	CConsole::Write("execute HVAPIUTILS_SmallImageToBitmap");
	HRESULT hr = E_FAIL;

	int iBmpLen = *pnBitmapDataLen;
	hr = Yuv2BMP(pbBitmapData, *pnBitmapDataLen, &iBmpLen, pbSmallImageData, nSmallImageWidth, nSmallImageHeight);
	*pnBitmapDataLen = iBmpLen;

	return hr;
}

HV_API HRESULT CDECL HVAPIUTILS_BinImageToBitmap(PBYTE pbBinImageData, PBYTE pbBitmapData, INT* pnBitmapDataLen)
{
//	CConsole::Write("execute HVAPIUTILS_BinImageToBitmap");
	if ( NULL == pbBinImageData || NULL == pbBitmapData || NULL == pnBitmapDataLen )
	{
		return E_FAIL;
	}

	INT nBmpLen = *pnBitmapDataLen;
	Bin2BMP(pbBinImageData, pbBitmapData, nBmpLen);
	*pnBitmapDataLen = nBmpLen;

	return S_OK;
}

HV_API HRESULT CDECL HVAPIUTILS_TrafficLightEnhance(PBYTE pbSrcImg, int iSrcImgDataLen, int iRedLightCount,
													PBYTE pbRedLightPos, PBYTE pbDstImage, int& iDstBufLen, 
													int iBrightness, int iHueThreshold, int iCompressRate)
{
//	CConsole::Write("execute HVAPIUTILS_TrafficLightEnhance");
	if(pbSrcImg == NULL || iSrcImgDataLen <= 0) return E_FAIL;
	if(pbDstImage == NULL || iDstBufLen <= 0) return E_FAIL;
	if(pbRedLightPos == NULL && sizeof(pbRedLightPos)/sizeof(HV_RECT) < iRedLightCount)return E_FAIL;

	HV_RECT* pcRect = new HV_RECT[iRedLightCount*sizeof(HV_RECT)];
	if(pcRect == NULL)return E_FAIL;
	memcpy(pcRect, pbRedLightPos, iRedLightCount*sizeof(HV_RECT));
	if(iBrightness < -255) iBrightness = -255;
	if(iBrightness > 255) iBrightness = 255;

	CxImage imgSrc(pbSrcImg, iSrcImgDataLen, CXIMAGE_FORMAT_UNKNOWN);
	if(!imgSrc.IsValid()) return E_FAIL;
	int iWidth = imgSrc.GetWidth();
	int iHeight = imgSrc.GetHeight();

	if(iRedLightCount < 1)
	{
		pcRect[0].left = 0;
		pcRect[0].top = 0;
		pcRect[0].right = iWidth;
		pcRect[0].bottom = iHeight;
		iRedLightCount = 1;
	}

	for(int i=0; i<iRedLightCount; i++)
	{
		RECT cTempRect;
		LONG lTemp;
		cTempRect.left = pcRect[i].left;
		cTempRect.top = pcRect[i].top;
		cTempRect.right = pcRect[i].right;
		cTempRect.bottom = pcRect[i].bottom;

		if(cTempRect.left > cTempRect.right)
		{
			lTemp = cTempRect.left;
			cTempRect.left = cTempRect.right;
			cTempRect.right = lTemp;
		}

		if(cTempRect.top > cTempRect.bottom)
		{
			lTemp = cTempRect.top;
			cTempRect.top = cTempRect.bottom;
			cTempRect.bottom = lTemp;
		}

		int iRectWidht, iRectHeight;
		iRectWidht = cTempRect.right - cTempRect.left;
		iRectHeight = cTempRect.bottom - cTempRect.top;
		if(iRectWidht <= 0 || iRectHeight <= 0)
		{
			continue;
		}
		CxImage imgCrop, imgH, imgS, imgV;
		if(!imgSrc.Crop(cTempRect, &imgCrop))
		{
			continue;
		}
		if(!imgCrop.SplitHSL(&imgH, &imgS, &imgV))
		{
			continue;
		}

		RGBQUAD h, s, v;
		int iSumH = 0, iSumV = 0, iSumS = 0;
		int iPixelCount(0);

		for(int y=0; y<iRectHeight; y++)
		{
			for(int x=0; x<iRectWidht; x++)
			{
				h = imgH.GetPixelColor(x, y);
				s = imgS.GetPixelColor(x, y);
				v = imgV.GetPixelColor(x, y);
				if(v.rgbBlue > 40 && v.rgbBlue < 230)
				{
					iSumH += h.rgbBlue;
					iSumS += s.rgbBlue;
					iSumV += v.rgbBlue;
					iPixelCount++;
				}
			}
		}
		if(iPixelCount == 0)
		{
			continue;
		}
		int iAvgH = iSumH / iPixelCount;
		if(iAvgH > iHueThreshold)
		{
			continue;
		}
		int iAvgV = iSumV / iPixelCount;
		int iAvgS = iSumS / iPixelCount;

		iSumV = 0;
		iPixelCount = 0;
		for(int y=0; y<iRectHeight; y++)
		{
			for(int x=0; x<iRectWidht; x++)
			{
				v = imgV.GetPixelColor(x, y);
				if(v.rgbBlue >= iAvgV)
				{
					iSumV += v.rgbBlue;
					iPixelCount++;
				}
			}
		}
		int iHeightAvgV = iAvgV;
		if(iPixelCount != 0) iHeightAvgV = iSumV / iPixelCount;

		for(int y=0; y<iRectHeight; y++)
		{
			for(int x=0; x<iRectWidht; x++)
			{
				h = imgH.GetPixelColor(x, y);
				s = imgS.GetPixelColor(x, y);
				v = imgV.GetPixelColor(x, y);
				if(v.rgbBlue <= 20 || s.rgbBlue < 40 || (h.rgbBlue > 60 && h.rgbBlue < 230)) continue;
				if(v.rgbBlue >= iHeightAvgV)
				{
					v.rgbRed = (BYTE)(v.rgbBlue - (v.rgbBlue-iHeightAvgV)*0.4);
					v.rgbGreen = v.rgbRed;
					v.rgbBlue = v.rgbRed;
					imgV.SetPixelColor(x, y, v);

					s.rgbRed = iAvgS;
					s.rgbGreen = s.rgbRed;
					s.rgbBlue = s.rgbRed;
					imgS.SetPixelColor(x, y, s);
				}
				h.rgbBlue = 0;
				h.rgbGreen = 0;
				h.rgbRed = 0;
				imgH.SetPixelColor(x, y, h);
			}
		}
		imgCrop.Combine(&imgH, &imgS, &imgV, NULL, 1);
		imgSrc.Mix(imgCrop, CxImage::OpDstCopy, -cTempRect.left, cTempRect.bottom-iHeight);
	}
	delete[] pcRect;
	if(iBrightness)
	{
		imgSrc.Light(iBrightness);
	}
	long lsize = 0;
	BYTE* pbuffer = NULL;
	imgSrc.SetJpegQuality(iCompressRate);
	if(!imgSrc.Encode(pbuffer, lsize, CXIMAGE_FORMAT_JPG)) return E_FAIL;

	if(lsize > iDstBufLen)
	{
		imgSrc.FreeMemory(pbuffer);
		iDstBufLen = lsize;
		return E_FAIL;
	}
	memcpy(pbDstImage, pbuffer, lsize);
	iDstBufLen = lsize;
	imgSrc.FreeMemory(pbuffer);
	return S_OK;
}

HV_API HRESULT CDECL HVAPIUTILS_GetRedLightPosFromeRecordImage(RECORD_IMAGE* pcRecordImage, int iRedLightPosBufLen, 
															   PBYTE pbRedLightPos, int& iRedLightCount)
{
//	CConsole::Write("execute HVAPIUTILS_GetRedLightPosFromeRecordImage");
	if(pbRedLightPos == NULL) return E_FAIL;
	if(pcRecordImage == NULL)
	{
		iRedLightCount = 0;
		memset(pbRedLightPos, 0, iRedLightPosBufLen);
		return E_FAIL;
	}
	PCI_IMAGE_INFO cImgInfo;
	memset(&cImgInfo, 0, sizeof(cImgInfo));
	memcpy(&cImgInfo, pcRecordImage->pbImgInfoEx, pcRecordImage->dwImgInfoLen);
	iRedLightCount = cImgInfo.nRedLightCount;
	if(iRedLightCount <= 0)
	{
		iRedLightCount = 0;
		memset(pbRedLightPos, 0, iRedLightPosBufLen);
		return E_FAIL;
	}
	if(iRedLightPosBufLen < (int)(sizeof(HV_RECT)*iRedLightCount))
	{
		iRedLightCount = 0;
		memset(pbRedLightPos, 0, iRedLightPosBufLen);
		return E_FAIL;
	}
	memcpy(pbRedLightPos, cImgInfo.rcRedLightPos, (int)(sizeof(HV_RECT)*iRedLightCount));
	return S_OK;
}

HV_API HRESULT CDECL HVAPIUTILS_GetPlatePosFromeJpegInfo(PBYTE pbJpegInfo, DWORD dwJpegInfoLen, int iPlatePosBufLen, 
														 PBYTE pbPlatePos, int& iPlateCount)
{
//	CConsole::Write("execute HVAPIUTILS_GetPlatePosFromeJpegInfo");
	iPlateCount = 0;
	if(pbPlatePos == NULL || iPlatePosBufLen <= 0) return E_FAIL;
	memset(pbPlatePos, 0, iPlatePosBufLen);
	if(pbJpegInfo == NULL || dwJpegInfoLen <= 0) return E_FAIL;
	char* pszTemp = strstr((char*)pbJpegInfo, "rect");
	if(pszTemp == NULL) return E_FAIL;
	pszTemp = pszTemp + 4;
	memcpy(&iPlateCount, pszTemp, 4);
	if(iPlateCount <= 0 || iPlateCount > 30) return E_FAIL;  //原数组为20  导致若红绿灯数+车牌跟踪框大于20时 接收不到图片
	int iPosInfoLen = iPlateCount * sizeof(HV_RECT);
	if(iPlatePosBufLen < iPosInfoLen) return E_FAIL;
	memcpy(pbPlatePos, pszTemp+4, iPosInfoLen);
	return S_OK;
}

HV_API HRESULT CDECL HVAPIUTILS_ParseXmlCmdRespRetcode(const char* szXmlBuf,
                                                       char* szCommand)
{
//	CConsole::Write("execute HVAPIUTILS_ParseXmlCmdRespRetcode");
    return HvParseXmlCmdRespRetcode((char*)szXmlBuf, szCommand);
}

HV_API HRESULT CDECL HVAPIUTILS_ParseXmlCmdRespMsg(const char* szXmlBuf,
                                                   const char* szMsgID,
                                                   char* szMsgOut)
{
//	CConsole::Write("execute HVAPIUTILS_ParseXmlCmdRespMsg");
    return HvParseXmlCmdRespMsg((char*)szXmlBuf, (char*)szMsgID, szMsgOut);
}

HV_API HRESULT CDECL HVAPIUTILS_ParseXmlInfoRespValue(const char* szXmlBuf,
                                                      const char* szInfoName, 
                                                      const char* nInfoValueName,
                                                      char* szInfoValueText)
{
//	CConsole::Write("execute HVAPIUTILS_ParseXmlInfoRespValue");
    return HvParseXmlInfoRespValue((char*)szXmlBuf, (char*)szInfoName, (char*)nInfoValueName, szInfoValueText);
}



HV_API HRESULT CDECL HVAPIUTILS_GetExecXmlCmdResString(HVAPI_HANDLE hHandle, 
													   char* szXmlBuf, 
													   char* szCmdName, 
													   char* szCmdValueName, 
													   char* szCmdValueText)
{
	if(hHandle == NULL)
	{
		return E_FAIL;
	}
	HVAPI_HANDLE_CONTEXT* pHHC = (HVAPI_HANDLE_CONTEXT*)hHandle;
	if(pHHC->fNewProtocol)
	{
		return HvParseXmlCmdRespRetcode2(szXmlBuf, szCmdName, szCmdValueName, szCmdValueText);
	}
	else
	{
		if(strstr(szCmdName, "GetWorkModeIndex"))
		{
			return HvParseXmlCmdRespMsg(szXmlBuf, szCmdName, szCmdValueText);
		}
		else
		{
			char szTempCmd[4] = {0};
			szTempCmd[0] = szCmdName[0];
			szTempCmd[1] = szCmdName[1];
			szTempCmd[2] = szCmdName[2];
			szTempCmd[3] = '\0';
			if((strcmp(szTempCmd, "Set") == 0)
				|| (strcmp(szTempCmd, "Res") == 0)
				|| (strcmp(szTempCmd, "For") == 0))
			{
				return HvParseXmlCmdRespRetcode(szXmlBuf, szCmdValueText);
			}
			else
			{
				return HvParseXmlInfoRespValue(szXmlBuf, szCmdName, szCmdValueName, szCmdValueText);
			}
		}
	}
}

HV_API HRESULT CDECL HVAPIUTILS_GetRecordInfoFromAppenedString(LPCSTR szAppened, LPCSTR szInfoName, LPSTR szRetInfo, INT iRetInfoBufLen)
{
	if(szAppened == NULL || szInfoName == NULL 
		|| szRetInfo == NULL || iRetInfoBufLen <= 0)
	{
		return E_FAIL;
	}
	TiXmlDocument cXmlDoc;
	if(cXmlDoc.Parse(szAppened))
	{
		const TiXmlElement* pRootElement = cXmlDoc.RootElement();
		if(pRootElement)
		{
			const TiXmlElement* pElementResultSet = pRootElement->FirstChildElement("ResultSet");
			if(pElementResultSet)
			{
				const TiXmlElement* pElementResult = pElementResultSet->FirstChildElement("Result");
				if(pElementResult)
				{
					const TiXmlElement* pElementname = pElementResult->FirstChildElement(szInfoName);
					if(!pElementname) return E_FAIL;

					if(strcmp(szInfoName, "PlateName") == 0)
					{
						if(iRetInfoBufLen >= (int)strlen(pElementname->GetText()))
						{
							strcpy(szRetInfo, pElementname->GetText());
							return S_OK;
						}
					}

					const char* pszChnName = NULL;
					const char* pszValue = NULL;

					pszChnName = pElementname->Attribute("chnname");
					pszValue =pElementname->Attribute("value");

					if(pszValue == NULL)
					{
						return E_FAIL;
					}

					if(pszChnName == NULL)
					{
						if(iRetInfoBufLen < (int)strlen(pszValue))
						{
							return E_FAIL;
						}
						strcpy(szRetInfo, pszValue);
						return S_OK;
					}

					if(strstr(pszChnName, "事件检测"))
					{
						if(iRetInfoBufLen < (int)strlen(pszValue))
						{
							return E_FAIL;
						}
						strcpy(szRetInfo, pszValue);
						return S_OK;
					}

					if(iRetInfoBufLen < (int)(strlen(pszChnName)
						+ strlen(pszValue) + 1))
					{
						return E_FAIL;
					}

					strcpy(szRetInfo, pszChnName);
					strcat(szRetInfo, ":");
					strcat(szRetInfo, pszValue);
					return S_OK;
				}
			}
		}
	}
	return E_FAIL;
}

HV_API HRESULT CDECL HVAPIUTILS_ParsePlateXmlString(LPCSTR pszXmlPlateInfo, LPSTR pszPlateInfoBuf, INT iPlateInfoBufLen)
{
	if(pszXmlPlateInfo == NULL || pszPlateInfoBuf == NULL
		|| iPlateInfoBufLen <= 0)
	{
		return E_FAIL;
	}
	int iBufRemainLen = iPlateInfoBufLen;
	int iResultValueLen = 0;
	BOOL fHadTimeInfo = FALSE;
	TiXmlDocument cXmlDoc;
	if(cXmlDoc.Parse(pszXmlPlateInfo))
	{
		const TiXmlElement* pRootElement = cXmlDoc.RootElement();
		if(pRootElement)
		{
			const TiXmlElement* pElementResultSet = pRootElement->FirstChildElement("ResultSet");
			if(pElementResultSet)
			{
				const TiXmlElement* pElementResult = pElementResultSet->FirstChildElement("Result");
				if(pElementResult)
				{
					DWORD dwTimeHigh, dwTimeLow;

					const TiXmlElement* pElementName = pElementResult->FirstChildElement("TimeHigh");
					{
						if(pElementName)
						{
							dwTimeHigh = atoi(pElementName->Attribute("value"));
						}
					}

					pElementName = pElementResult->FirstChildElement("TimeLow");
					{
						if(pElementName)
						{
							dwTimeLow = atoi(pElementName->Attribute("value"));
							fHadTimeInfo = TRUE;
						}
					}

					if(fHadTimeInfo)
					{
						const char* pszChnName;
						const char* pszValue;
						pElementName = pElementResult->FirstChildElement();
						while(pElementName)
						{
							pszChnName = pElementName->Attribute("chnname");
							pszValue = pElementName->Attribute("value");
							if(pszChnName && pszValue)
							{
								if(strstr(pszChnName, "事件检测"))
								{
									iResultValueLen = (int)strlen(pszValue) + 1;
									if(iResultValueLen >= iBufRemainLen)
									{
										return E_FAIL;
									}
									strcat(pszPlateInfoBuf, "\n");
									strcat(pszPlateInfoBuf, pszValue);
									iBufRemainLen -= iResultValueLen;
								}
								else
								{
									iResultValueLen = (int)strlen(pszChnName) + (int)strlen(pszValue) + 2;
									if(iResultValueLen >= iBufRemainLen)
									{
										return E_FAIL;
									}
									strcat(pszPlateInfoBuf, "\n");
									strcat(pszPlateInfoBuf, pszChnName);
									strcat(pszPlateInfoBuf, ":");
									strcat(pszPlateInfoBuf, pszValue);
									iBufRemainLen -= iResultValueLen;
								}
							}
							pElementName = pElementName->NextSiblingElement();
						}
					}
					else
					{
						pElementName = pElementResult->FirstChildElement("ReverseRun");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 14;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "车辆逆向行驶:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("VideoScaleSpeed");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 10;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "视频测速:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("SpeedLimit");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 8;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "限速值:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("ScaleSpeedOfDistance");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 18;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "距离测量误差比例:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("ObservedFrames");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 10;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "有效帧数:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("Confidence");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 12;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "平均可信度:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("FirstCharConf");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 12;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "首字可信度:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("CarArriveTime");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 14;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "车辆检测时间:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("CarType");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 10;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "车辆类型:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("CarColor");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 10;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "车身颜色:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("RoadNumber");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 6;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "车道:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("BeginRoadNumber");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 12;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "起始车道号:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("StreetName");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 10;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "路口名称:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("StreetDirection");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 10;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "路口方向:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("EventCheck");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 1;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("FrameName");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 10;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "视频帧名:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("PlateLightType");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 16;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "摄像机亮度级别:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("AmbientLight");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 10;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "环境亮度:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("PlateLight");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 10;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "车牌亮度:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}

						pElementName = pElementResult->FirstChildElement("PlateVariance");
						if(pElementName)
						{
							iResultValueLen = (int)strlen(pElementName->Attribute("value")) + 12;
							if(iResultValueLen >= iBufRemainLen)
							{
								return E_FAIL;
							}
							strcat(pszPlateInfoBuf, "\n");
							strcat(pszPlateInfoBuf, "车牌对比度:");
							strcat(pszPlateInfoBuf, pElementName->Attribute("value"));
							iBufRemainLen -= iResultValueLen;
						}
					}
					if(!strstr(pszPlateInfoBuf, "车辆检测时间:"))
					{
						strcat(pszPlateInfoBuf, "\n车辆检测时间:");
						DWORD64 dw64TimeMs = ((DWORD64)dwTimeHigh<<32) | (DWORD64)dwTimeLow;
						char szTmpTime[20] = {0};
						sprintf(szTmpTime, "%I64u", dw64TimeMs);
						strcat(pszPlateInfoBuf, szTmpTime);
					}
				}
			}
		}
	}
	return S_OK;
}


