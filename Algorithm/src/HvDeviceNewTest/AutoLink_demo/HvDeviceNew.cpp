#include "HvDeviceNew.h"
#include "HvDevice.h"
#include "HVAPI_HANDLE_CONTEXT_EX.h"
#include "HvDeviceDLL.h"
#include <gdiplus.h>
#include <atltime.h>
#include <atlstr.h>
#include "HvAutoLink.h"

using namespace HiVideo;
using namespace Gdiplus;

CLSID	g_jpgClsid = {0};
ULONG_PTR g_gdiplusToken = 0;
BOOL g_fGdiInitialized = FALSE;

#define DEFAULT_CONNCMD  "DownloadRecord,Enable[0],BeginTime[2012.01.01_01],Index[0],EndTime[0]"

extern HRESULT IsEmptyPackCache( HVAPI_HANDLE_CONTEXT_EX* pHandle , PACK_TYPE eType , bool& fIsEmptyCache);
extern HRESULT GetPackCache( HVAPI_HANDLE_CONTEXT_EX* pHandle , PACK_TYPE eType ,unsigned char** ppBuffer , int& iBufferLen );
extern HRESULT UpdatePackCache(HVAPI_HANDLE_CONTEXT_EX* pHandle, PACK_TYPE eType , char* pBuffer , int nBufferLen );

extern HRESULT CloseAutoLinkHHC(LPSTR szDevSN);
extern HRESULT OutPutDebugInfor(char* pDebugInfo);
extern HRESULT SetConnAutoLinkHHC(LPSTR szDevSN);

void WrightLogEx(LPCSTR lpszIP, LPCSTR lpszLogInfo)
{
	CTime cTime = CTime::GetCurrentTime();
	char szTime[256] = {0};
	FILE* fp = fopen("D:\\HvDeviceNewLog.txt", "a+");
	if(fp)
	{
		sprintf(szTime, "%s  <%s>  ", cTime.Format("%Y-%m-%d %H:%M:%S"), lpszIP);
		fwrite(szTime, strlen(szTime), 1, fp);
		fwrite(lpszLogInfo, strlen(lpszLogInfo), 1, fp);
		fwrite("\n", 1, 1, fp);
		fclose(fp);
	}
}

static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j; 
		}    
	}

	free(pImageCodecInfo);
	return -1; 
}

void GdiInitialized(void)
{
	if( !g_fGdiInitialized )
	{
		GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);
		GetEncoderClsid(L"image/jpeg", &g_jpgClsid);
		g_fGdiInitialized = true;
	}
}

HV_API_EX HRESULT HVAPI_SearchDeviceCount(DWORD32* pdwDeviceCount)
{
	if(pdwDeviceCount == NULL) 
	{
		return E_FAIL;
	}
	DWORD32 dwDeviceCount = 0;
	SearchHVDeviceCount(&dwDeviceCount);
	*pdwDeviceCount = dwDeviceCount;
	return S_OK;
}

HV_API_EX HRESULT HVAPI_GetDeviceAddr(DWORD32 dwIndex, DWORD64* dw64MacAddr, DWORD32* dwIP,
								   DWORD32* dwMask, DWORD32* dwGateWay)
{
	return GetHVDeviceAddr(dwIndex, dw64MacAddr, dwIP, dwMask, dwGateWay);
}

HV_API_EX HRESULT HVAPI_GetDeviceInfoEx(int iIndex, LPSTR lpExtInfo, int iBufLen)
{
	return HvGetDeviceExteInfo(iIndex, lpExtInfo, iBufLen);
}

HV_API HRESULT CDECL HVAPI_SetIPByMacAddr(DWORD64 dw64MacAddr, DWORD32 dwIP, DWORD32 dwMask, DWORD32 dwGateway)
{
	return SetIPFromMac(dw64MacAddr, dwIP, dwMask, dwGateway);
}

HV_API_EX HRESULT CDECL HVAPI_GetDevTypeEx(PSTR pcIP, int* iDeviceType)
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
	DWORD dwXmlVersion = 0;
	HvGetXmlProtocolVersion(pcIP, &dwXmlVersion);
	bool fIsNewXml = (dwXmlVersion == 0) ? false : true; 
	if(HvMakeXmlInfoByString(fIsNewXml, "GetDevType", 10, pszXmlBuf, iXmlBufLen) == E_FAIL)
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
	char szRetcode[20] = {0};
	if(dwXmlVersion == 0)
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
		if(strstr(szRetcode, "_S"))
		{
			*iDeviceType = DEV_TYPE_HVSIGLE;
		}
		else if(strstr(szRetcode, "DM6467_200W_LITE"))
		{
			*iDeviceType = DEV_TYPE_HVCAM_SINGLE;
		}
		else if (strstr(szRetcode, "200W_CAMERA"))
		{
			*iDeviceType = DEV_TYPE_HVCAMERA;
		}
		else if(strstr(szRetcode, "_500W"))
		{
			*iDeviceType = DEV_TYPE_HVCAM_500W;
		}
		else if (strstr(szRetcode, "_200W"))
		{
			*iDeviceType = DEV_TYPE_HVCAM_200W;
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

static DWORD WINAPI HvSocketStatusMonitorThreadFuncEx(LPVOID lpParam)
{
	if(lpParam == NULL)
	{
		return -1;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)lpParam;
	BOOL fIsNeedSleep = TRUE;
	while (!pHHC->fIsThreadSocketStatusMonitorExit)
	{
		fIsNeedSleep = TRUE;

		if(pHHC->dwRecordConnStatus == CONN_STATUS_RECONN)
		{
			fIsNeedSleep = FALSE;
			if(ConnectCamera(pHHC->szIP, CAMERA_RECORD_LINK_PORT, pHHC->sktRecord))
			{
				if(strlen((const char*)pHHC->szRecordConnCmd) > 0)
				{
					if(pHHC->fIsRecvHistoryRecord)
					{
						if(pHHC->iRecordDataInfo == -1)
						{
							sprintf((char*)pHHC->szRecordConnCmd, "DownloadRecord,BeginTime[%s],Index[%d],Enable[1],EndTime[%s]",
								pHHC->szRecordBeginTimeStr, pHHC->dwRecordStartIndex, pHHC->szRecordEndTimeStr);
						}
						else
						{
							sprintf((char*)pHHC->szRecordConnCmd, "DownloadRecord,BeginTime[%s],Index[%d],Enable[1],EndTime[%s],DataInfo[%d]",
								pHHC->szRecordBeginTimeStr, pHHC->dwRecordStartIndex,
								pHHC->szRecordEndTimeStr, pHHC->iRecordDataInfo);
						}
					}

					char szRetBuf[256] = {0};
					if(HvSendXmlCmd(pHHC->szIP, (char*)pHHC->szRecordConnCmd, 
						szRetBuf, sizeof(szRetBuf), NULL, pHHC->sktRecord) == S_OK)
					{
						pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
						pHHC->dwRecordStreamTick = GetTickCount();
						pHHC->dwRecordReconectTimes++;
					}
				}
				else
				{
					pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
					pHHC->dwRecordStreamTick = GetTickCount();
					pHHC->dwRecordReconectTimes++;
				}

			}
		}

		if(pHHC->dwImageConnStatus == CONN_STATUS_RECONN)
		{
			fIsNeedSleep = FALSE;
			if(ConnectCamera(pHHC->szIP, CAMERA_IMAGE_LINK_PORT, pHHC->sktImage))
			{
				if(strlen((const char*)pHHC->szImageConnCmd) > 0)
				{
					char szRetBuf[256] = {0};
					if(HvSendXmlCmd(pHHC->szIP, (char*)pHHC->szImageConnCmd, 
						szRetBuf, sizeof(szRetBuf), NULL, pHHC->sktImage) == S_OK)
					{
						pHHC->dwImageConnStatus = CONN_STATUS_NORMAL;
						pHHC->dwImageStreamTick = GetTickCount();
						pHHC->dwImageReconnectTimes++;
					}
				}
				else
				{
					pHHC->dwImageConnStatus = CONN_STATUS_NORMAL;
					pHHC->dwImageStreamTick = GetTickCount();
					pHHC->dwImageReconnectTimes++;
				}
			}
		}

		if(pHHC->dwVideoConnStatus == CONN_STATUS_RECONN)
		{
			fIsNeedSleep = FALSE;
			if(ConnectCamera(pHHC->szIP, CAMERA_VIDEO_LINK_PORT, pHHC->sktVideo))
			{
				if(strlen((const char*)pHHC->szVideoConnCmd) > 0)
				{
					if(pHHC->fIsConnectHistoryVideo)
					{
						sprintf((char*)pHHC->szVideoConnCmd, "DownloadVideo,BeginTime[%s],EndTime[%s],Enable[1]",
							pHHC->szVideoBeginTimeStr, pHHC->szVideoEndTimeStr);
					}
					char szRetBuf[256] = {0};
					if(HvSendXmlCmd(pHHC->szIP, (char*)pHHC->szVideoConnCmd, 
						szRetBuf, sizeof(szRetBuf), NULL, pHHC->sktVideo) == S_OK)
					{
						pHHC->dwVideoConnStatus = CONN_STATUS_NORMAL;
						pHHC->dwVideoStreamTick = GetTickCount();
						pHHC->dwVideoReconnectTimes++;
					}
				}
				else
				{
					pHHC->dwVideoConnStatus = CONN_STATUS_NORMAL;
					pHHC->dwVideoStreamTick = GetTickCount();
					pHHC->dwVideoReconnectTimes++;
				}
			}
		}

		if(fIsNeedSleep)
		{
			Sleep(1000);
		}
	}
	return 0;
}

HV_API_EX HVAPI_HANDLE_EX CDECL HVAPI_OpenEx(LPCSTR szIp, LPCSTR szApiVer)
{
	if (szIp == NULL)
	{
		return NULL;
	}

	if (szApiVer == NULL)
	{
		szApiVer = HVAPI_API_VERSION_EX;
	}

	HVAPI_HANDLE_EX hRetHandle = NULL;

	if (strcmp(HVAPI_API_VERSION_EX, szApiVer) == 0)
	{
		HVAPI_HANDLE_CONTEXT_EX* hContextHandle = new HVAPI_HANDLE_CONTEXT_EX;
		if (hContextHandle)
		{
			strcpy(hContextHandle->szVersion, szApiVer);
			strcpy(hContextHandle->szIP, szIp);
			hContextHandle->dwOpenType = 1;
			hRetHandle = (HVAPI_HANDLE)hContextHandle;

			hContextHandle->fNewProtocol = IsNewProtocol(hContextHandle->szIP);
			char szRetBuf[512] = {0};
			if(HVAPI_ExecCmdEx(hRetHandle, "GetVersionString", szRetBuf, sizeof(szRetBuf), NULL) != S_OK)
			{
				delete hContextHandle;
				return NULL;
			}

			hContextHandle->fIsThreadSocketStatusMonitorExit = FALSE;
			hContextHandle->hThreadSocketStatusMonitor = CreateThread(NULL, 0, HvSocketStatusMonitorThreadFuncEx, hContextHandle, 0, NULL);
			if(hContextHandle->hThreadSocketStatusMonitor == NULL)
			{
				delete hContextHandle;
				return NULL;
			}
		}
	}
	return hRetHandle;
}

HV_API HRESULT CDECL HVAPI_CloseEx(HVAPI_HANDLE_EX hHandle)
{
	if(hHandle == NULL)
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;

	if(pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	pHHC->fIsThreadSocketStatusMonitorExit = TRUE;
	HvSafeCloseThread(pHHC->hThreadSocketStatusMonitor);

	pHHC->fIsThreadRecvRecordExit = TRUE;
	pHHC->fIsThreadRecvImageExit = TRUE;
	pHHC->fIsThreadRecvVideoExit = TRUE;
	HvSafeCloseThread(pHHC->hThreadRecvRecord);
	HvSafeCloseThread(pHHC->hThreadRecvImage);
	HvSafeCloseThread(pHHC->hThreadRecvVideo);
	ForceCloseSocket(pHHC->sktRecord);
	ForceCloseSocket(pHHC->sktImage);
	ForceCloseSocket(pHHC->sktVideo);

	/*HVAPI_CALLBACK_SET* pTemp;
	for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
	{
		memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
	}*/

	pHHC->fVailPackResumeCache = FALSE;
	if (NULL != pHHC->pPackResumeCache)
	{
		PACK_RESUME_CACHE* pPackResumeCache = pHHC->pPackResumeCache;
		if (NULL != pPackResumeCache->pInfor)
		{
			delete pPackResumeCache->pInfor;
			pPackResumeCache->pInfor = NULL;
		}

		if (NULL != pPackResumeCache->pData)
		{
			delete pPackResumeCache->pData;
			pPackResumeCache->pData = NULL;
		}
	}

	if (pHHC->fAutoLink)
	{	
		CloseAutoLinkHHC(pHHC->szDevSN);
	}
	else
	{
		delete pHHC;
	}
	return S_OK;
}

HV_API_EX HRESULT CDECL HVAPI_GetXmlVersionEx(HVAPI_HANDLE_EX hHandle, bool* pfIsNewProtol)
{
	if(hHandle == NULL || pfIsNewProtol == NULL)
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}
	*pfIsNewProtol = pHHC->fNewProtocol;
	return S_OK;
}

HV_API_EX HRESULT CDECL HVAPI_GetConnStatusEx(HVAPI_HANDLE_EX hHandle, INT nStreamType, DWORD* pdwConnStatus)
{
	if ( NULL == hHandle || NULL == pdwConnStatus )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if ( 0 != strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion)
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	if ( CONN_TYPE_IMAGE == nStreamType )
	{
		*pdwConnStatus = pHHC->dwImageConnStatus;
	}
	else if ( CONN_TYPE_VIDEO == nStreamType )
	{
		*pdwConnStatus = pHHC->dwVideoConnStatus;
	}
	else if ( CONN_TYPE_RECORD == nStreamType )
	{
		*pdwConnStatus = pHHC->dwRecordConnStatus;
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

HV_API_EX HRESULT CDECL HVAPI_GetReConnectTimesEx(HVAPI_HANDLE_EX hHandle, INT nStreamType, DWORD* pdwReConnectTimes, BOOL fIsReset)
{
	if ( hHandle == NULL || pdwReConnectTimes ==NULL )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if ( 0 != strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) 
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	if (nStreamType == CONN_TYPE_IMAGE)
	{
		*pdwReConnectTimes = pHHC->dwImageReconnectTimes;
		if(fIsReset)
		{
			pHHC->dwImageReconnectTimes = 0;
		}
	}
	else if (nStreamType == CONN_TYPE_VIDEO)
	{
		*pdwReConnectTimes = pHHC->dwVideoReconnectTimes;
		if(fIsReset)
		{
			pHHC->dwVideoReconnectTimes = 0;
		}
	}
	else if (nStreamType == CONN_TYPE_RECORD)
	{
		*pdwReConnectTimes = pHHC->dwRecordReconectTimes;
		if(fIsReset)
		{
			pHHC->dwRecordReconectTimes = 0;
		}
	}
	else
	{
		return E_FAIL;
	}
	return S_OK;
}

HV_API_EX HRESULT CDECL HVAPI_ExecCmdEx(HVAPI_HANDLE hHandle,
								   LPCSTR szCmd,
								   LPSTR szRetBuf,
								   INT nBufLen,
								   INT* pnRetLen)
{
	if(hHandle == NULL || szCmd == NULL
		|| szRetBuf == NULL || nBufLen <= 0)
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	const int iBufLen = (1024 << 4);
	char* pszXmlBuf = new char[iBufLen];
	if(pszXmlBuf == NULL)
	{
		return E_FAIL;
	}
	int iXmlBufLen = iBufLen;

	TiXmlDocument cXmlDoc;
	if(!cXmlDoc.Parse(szCmd))
	{
		if(pHHC->fNewProtocol)
		{
			if(HvMakeXmlCmdByString(pHHC->fNewProtocol, szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen)
				!= S_OK)
			{
				SAFE_DELETE(pszXmlBuf);
				return E_FAIL;
			}

			szCmd = (char*)pszXmlBuf;
		}
		else
		{
			if(strstr(szCmd, "GetWorkModeIndex"))
			{
				if(HvMakeXmlCmdByString(pHHC->fNewProtocol, szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen)
					!= S_OK)
				{
					SAFE_DELETE(pszXmlBuf);
					return E_FAIL;
				}

				szCmd = (char*)pszXmlBuf;	
			}
			else
			{
				char szTempCmd[4] = {0};
				szTempCmd[0] = szCmd[0];
				szTempCmd[1] = szCmd[1];
				szTempCmd[2] = szCmd[2];
				szTempCmd[3] = '\0';
				if((strcmp(szTempCmd, "Set") == 0)
					|| (strcmp(szTempCmd, "Res") == 0)
					|| (strcmp(szTempCmd, "For") == 0)
					|| (strcmp(szTempCmd, "Sof") == 0))
				{
					if(HvMakeXmlCmdByString(pHHC->fNewProtocol, szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen)
						!= S_OK)
					{
						SAFE_DELETE(pszXmlBuf);
						return E_FAIL;
					}

					szCmd = (char*)pszXmlBuf;
				}
				else
				{
					if(HvMakeXmlInfoByString(pHHC->fNewProtocol, szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen)
						!= S_OK)
					{
						SAFE_DELETE(pszXmlBuf);
						return E_FAIL;
					}

					szCmd = (char*)pszXmlBuf;
				}
			}
		}
	}

	SOCKET sktCmd = INVALID_SOCKET;
	bool fRet = ExecXmlExtCmdEx(pHHC->szIP, (char*)szCmd, (char*)szRetBuf, nBufLen, sktCmd);
	if ( pnRetLen )
	{
		*pnRetLen = nBufLen;
	}

	SAFE_DELETE(pszXmlBuf);
	return (true == fRet) ? S_OK : E_FAIL;
}

HV_API_EX HRESULT CDECL HVAPI_GetParamEx(HVAPI_HANDLE_EX hHandle, LPSTR szXmlParam, INT nBufLen, INT* pnRetLen)
{
	if ( NULL == hHandle || NULL == szXmlParam )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if ( 0 != strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) 
		|| pHHC->dwOpenType != 1)
	{
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
	SOCKET hSocketCmd;

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

HV_API_EX HRESULT CDECL HVAPI_SetParamEx(HVAPI_HANDLE_EX hHandle, LPCSTR szXmlParam)
{
	if ( NULL == hHandle || NULL == szXmlParam )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if ( 0 != strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) 
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	const WORD wPort = CAMERA_CMD_LINK_PORT;
	SOCKET hSocketCmd;

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

HV_API_EX HRESULT CDECL HVAPI_SendControllPannelUpdateFileEx(HVAPI_HANDLE_EX hHandle, PBYTE pUpdateFileBuffer, DWORD dwFileSize)
{
	if ( NULL == hHandle || NULL == pUpdateFileBuffer )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if ( 0 != strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion)
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	const WORD wPort = CAMERA_CMD_LINK_PORT;
	SOCKET hSocketCmd;

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

typedef struct _PCI_IMAGE_INFO
{
	DWORD32 dwCarID;
	DWORD32 dwImgType;
	DWORD32 dwImgWidth;
	DWORD32 dwImgHeight;
	DWORD32 dwTimeLow;
	DWORD32 dwTimeHigh;
	DWORD32 dwEddyType;
	RECT rcPlate;
	RECT rcRedLightPos[20];
	int nRedLightCount;
	RECT rcFacePos[20];
	int nFaceCount;

	_PCI_IMAGE_INFO()
	{
		memset(this, 0, sizeof(*this));
	}
}PCI_IMAGE_INFO;

HRESULT ProcBigImageCallBack(HVAPI_CALLBACK_SET* pCallBackSet, DWORD dwType, RECORD_IMAGE_EX* pcImage, BOOL fIsPeccancy,
							 DWORD dwEnhanceFlag, INT iBrightness, INT iHueThreshold,
							 INT iCompressRate, DWORD dwRecordType, DWORD64 dwTimeMS)
{
	if(pCallBackSet == NULL || pcImage == NULL)
	{
		return E_FAIL;
	}
	PCI_IMAGE_INFO cImgInfo;
	memset(&cImgInfo, 0, sizeof(cImgInfo));

	int iBuffLen;
	iBuffLen = (sizeof(PCI_IMAGE_INFO) < pcImage->dwImgInfoLen) ? sizeof(PCI_IMAGE_INFO) : pcImage->dwImgInfoLen;
	memcpy(&cImgInfo, pcImage->pbImgInfo, iBuffLen);
	if(cImgInfo.nRedLightCount <= 0 || dwEnhanceFlag == 0)
	{
		pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
			(WORD)dwType, pcImage->cImgInfo.dwWidth,
			pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
			pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
/*		switch(dwType)
		{
		case RECORD_BIGIMG_BEST_SNAPSHOT:
			{
				if(pCallBackSet->dwBestSnapShotPicBufLen >= pcImage->dwImgDataLen)
				{
					memcpy(pCallBackSet->pBestSnapshotPicBuf, pcImage->pbImgData, pcImage->dwImgDataLen);
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pCallBackSet->pBestSnapshotPicBuf,
						pcImage->dwImgDataLen, dwTimeMS);
				}
			}
			break;
		case RECORD_BIGIMG_LAST_SNAPSHOT:
			{
				if(pCallBackSet->dwLastSnapShotPicBufLen >= pcImage->dwImgDataLen)
				{
					memcpy(pCallBackSet->pLastSnapshotPicBuf, pcImage->pbImgData, pcImage->dwImgDataLen);
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pCallBackSet->pLastSnapshotPicBuf,
						pcImage->dwImgDataLen, dwTimeMS);
				}
			}
			break;
		case RECORD_BIGIMG_BEGIN_CAPTURE:
			{
				pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
					RECORD_BIGIMG_BEGIN_CAPTURE, pcImage->cImgInfo.dwWidth,
					pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
					pcImage->dwImgDataLen, dwTimeMS);
			}
			break;
		case RECORD_BIGIMG_BEST_CAPTURE:
			{
				pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
					RECORD_BIGIMG_BEST_CAPTURE, pcImage->cImgInfo.dwWidth,
					pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
					pcImage->dwImgDataLen, dwTimeMS);
			}
			break;
		case RECORD_BIGIMG_LAST_CAPTURE:
			{
				if(pCallBackSet->dwLastCapturePicBufLen >= pcImage->dwImgDataLen)
				{
					memcpy(pCallBackSet->pLastCapturePicBuf, pcImage->pbImgData, pcImage->dwImgDataLen);
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pCallBackSet->pLastCapturePicBuf,
						pcImage->dwImgDataLen, dwTimeMS);
				}
			}
			break;
		}*/
		return S_OK;
	}
	
	DWORD dwRetBufLen = 1024;
	PBYTE pTempPic = NULL;
	switch(dwType)
	{
	case RECORD_BIGIMG_BEST_SNAPSHOT:
		{
			if((dwEnhanceFlag == 1 && fIsPeccancy)
				|| dwEnhanceFlag == 2)
			{
				dwRetBufLen = dwRetBufLen << 10;
				pTempPic = new BYTE[dwRetBufLen];
				if(pTempPic == NULL)
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
				}
				HRESULT hr = HvEnhanceTrafficLight(pcImage->pbImgData, pcImage->dwImgDataLen,
					cImgInfo.nRedLightCount, (PBYTE)&cImgInfo.rcRedLightPos, pTempPic,
					dwRetBufLen, iBrightness, iHueThreshold, iCompressRate);
				if(hr == S_OK)
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pTempPic,
						dwRetBufLen, dwRecordType, dwTimeMS);
				}
				else
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
				}
				delete[] pTempPic;
				pTempPic = NULL;
			}
			else
			{
				pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
					RECORD_BIGIMG_BEST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
					pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
					pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
			}

		}
		break;
	case RECORD_BIGIMG_LAST_SNAPSHOT:
		{
			if((dwEnhanceFlag == 1 && fIsPeccancy)
				|| dwEnhanceFlag == 2)
			{
				dwRetBufLen = dwRetBufLen << 10;
				pTempPic = new BYTE[dwRetBufLen];
				if(pTempPic == NULL)
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
				}
				HRESULT hr = HvEnhanceTrafficLight(pcImage->pbImgData, pcImage->dwImgDataLen,
					cImgInfo.nRedLightCount, (PBYTE)&cImgInfo.rcRedLightPos, pTempPic,
					dwRetBufLen, iBrightness, iHueThreshold, iCompressRate);
				if(hr == S_OK)
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pTempPic,
						dwRetBufLen, dwRecordType, dwTimeMS);
				}
				else
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
				}
				delete[] pTempPic;
				pTempPic = NULL;
			}
			else
			{
				pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
					RECORD_BIGIMG_LAST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
					pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
					pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
			}
		}
		break;
	case RECORD_BIGIMG_BEGIN_CAPTURE:
		{
			if((dwEnhanceFlag == 1 && fIsPeccancy)
				|| dwEnhanceFlag == 2 || dwEnhanceFlag == 3)
			{
				dwRetBufLen = dwRetBufLen << 10;
				pTempPic = new BYTE[dwRetBufLen];
				if(pTempPic == NULL)
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEGIN_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
				}
				HRESULT hr = HvEnhanceTrafficLight(pcImage->pbImgData, pcImage->dwImgDataLen,
					cImgInfo.nRedLightCount, (PBYTE)&cImgInfo.rcRedLightPos, pTempPic,
					dwRetBufLen, iBrightness, iHueThreshold, iCompressRate);
				if(hr == S_OK)
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEGIN_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pTempPic,
						dwRetBufLen, dwRecordType, dwTimeMS);
				}
				else
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEGIN_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
				}
				delete[] pTempPic;
				pTempPic = NULL;
			}
			else
			{
				pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
					RECORD_BIGIMG_BEGIN_CAPTURE, pcImage->cImgInfo.dwWidth,
					pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
					pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
			}
		}
		break;
	case RECORD_BIGIMG_BEST_CAPTURE:
		{
			if((dwEnhanceFlag == 1 && fIsPeccancy)
				|| dwEnhanceFlag == 2 || dwEnhanceFlag == 3)
			{
				dwRetBufLen = dwRetBufLen << 10;
				pTempPic = new BYTE[dwRetBufLen];
				if(pTempPic == NULL)
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
				}
				HRESULT hr = HvEnhanceTrafficLight(pcImage->pbImgData, pcImage->dwImgDataLen,
					cImgInfo.nRedLightCount, (PBYTE)&cImgInfo.rcRedLightPos, pTempPic,
					dwRetBufLen, iBrightness, iHueThreshold, iCompressRate);
				if(hr == S_OK)
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pTempPic,
						dwRetBufLen, dwRecordType, dwTimeMS);
				}
				else
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
				}
				delete[] pTempPic;
				pTempPic = NULL;
			}
			else
			{
				pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
					RECORD_BIGIMG_BEST_CAPTURE, pcImage->cImgInfo.dwWidth,
					pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
					pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
			}
		}
		break;
	case RECORD_BIGIMG_LAST_CAPTURE:
		{
			if((dwEnhanceFlag == 1 && fIsPeccancy)
				|| dwEnhanceFlag == 2 || dwEnhanceFlag == 3)
			{
				dwRetBufLen = dwRetBufLen << 10;
				pTempPic = new BYTE[dwRetBufLen];
				if(pTempPic == NULL)
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
				}
				HRESULT hr = HvEnhanceTrafficLight(pcImage->pbImgData, pcImage->dwImgDataLen,
					cImgInfo.nRedLightCount, (PBYTE)&cImgInfo.rcRedLightPos, pTempPic,
					dwRetBufLen, iBrightness, iHueThreshold, iCompressRate);
				if(hr == S_OK)
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pTempPic,
						dwRetBufLen, dwRecordType, dwTimeMS);
				}
				else
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
				}
				delete[] pTempPic;
				pTempPic = NULL;
			}
			else
			{
				pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
					RECORD_BIGIMG_LAST_CAPTURE, pcImage->cImgInfo.dwWidth,
					pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
					pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
			}
		}
		break;
	}

	/*switch(dwType)
	{
	case RECORD_BIGIMG_BEST_SNAPSHOT:
		{
			if((dwEnhanceFlag == 1 && fIsPeccancy)
				|| dwEnhanceFlag == 2)
			{
				DWORD dwRetBufLen = pCallBackSet->dwBestSnapShotPicBufLen;
				HRESULT hr = HvEnhanceTrafficLight(pcImage->pbImgData, pcImage->dwImgDataLen,
					cImgInfo.nRedLightCount, (PBYTE)&cImgInfo.rcRedLightPos, pCallBackSet->pBestSnapshotPicBuf,
					dwRetBufLen, iBrightness, iHueThreshold, iCompressRate);
				if(hr == S_OK)
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pCallBackSet->pBestSnapshotPicBuf,
						dwRetBufLen, dwTimeMS);
				}
				else
				{
					if(pCallBackSet->dwBestSnapShotPicBufLen >= pcImage->dwImgDataLen)
					{
						memcpy(pCallBackSet->pBestSnapshotPicBuf, pcImage->pbImgData, pcImage->dwImgDataLen);
						pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
							RECORD_BIGIMG_BEST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
							pcImage->cImgInfo.dwHeight, pCallBackSet->pBestSnapshotPicBuf,
							pcImage->dwImgDataLen, dwTimeMS);
					}
				}
			}
			else
			{
				if(pCallBackSet->dwBestSnapShotPicBufLen >= pcImage->dwImgDataLen)
				{
					memcpy(pCallBackSet->pBestSnapshotPicBuf, pcImage->pbImgData, pcImage->dwImgDataLen);
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pCallBackSet->pBestSnapshotPicBuf,
						pcImage->dwImgDataLen, dwTimeMS);
				}
			}
		}
		break;
	case RECORD_BIGIMG_LAST_SNAPSHOT:
		{
			if((dwEnhanceFlag == 1 && fIsPeccancy)
				|| dwEnhanceFlag == 2)
			{
				DWORD dwRetBufLen = pCallBackSet->dwLastSnapShotPicBufLen;
				HRESULT hr = HvEnhanceTrafficLight(pcImage->pbImgData, pcImage->dwImgDataLen,
					cImgInfo.nRedLightCount, (PBYTE)&cImgInfo.rcRedLightPos, pCallBackSet->pLastSnapshotPicBuf,
					dwRetBufLen, iBrightness, iHueThreshold, iCompressRate);
				if(hr == S_OK)
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pCallBackSet->pLastSnapshotPicBuf,
						dwRetBufLen, dwTimeMS);
				}
				else
				{
					if(pCallBackSet->dwLastSnapShotPicBufLen >= pcImage->dwImgDataLen)
					{
						memcpy(pCallBackSet->pLastSnapshotPicBuf, pcImage->pbImgData, pcImage->dwImgDataLen);
						pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
							RECORD_BIGIMG_LAST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
							pcImage->cImgInfo.dwHeight, pCallBackSet->pLastSnapshotPicBuf,
							pcImage->dwImgDataLen, dwTimeMS);
					}
				}
			}
			else
			{
				if(pCallBackSet->dwLastSnapShotPicBufLen >= pcImage->dwImgDataLen)
				{
					memcpy(pCallBackSet->pLastSnapshotPicBuf, pcImage->pbImgData, pcImage->dwImgDataLen);
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pCallBackSet->pLastSnapshotPicBuf,
						pcImage->dwImgDataLen, dwTimeMS);
				}
			}
		}
		break;
	case RECORD_BIGIMG_BEGIN_CAPTURE:
		{
			if((dwEnhanceFlag == 1 && fIsPeccancy)
				|| dwEnhanceFlag == 2 || dwEnhanceFlag == 3)
			{
				DWORD dwRetBufLen = pCallBackSet->dwBeginCapturePicBufLen;
				HRESULT hr = HvEnhanceTrafficLight(pcImage->pbImgData, pcImage->dwImgDataLen,
					cImgInfo.nRedLightCount, (PBYTE)&cImgInfo.rcRedLightPos, pCallBackSet->pBeginCapturePicBuf,
					dwRetBufLen, iBrightness, iHueThreshold, iCompressRate);
				if(hr == S_OK)
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEGIN_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pCallBackSet->pBeginCapturePicBuf,
						dwRetBufLen, dwTimeMS);
				}
				else
				{
					if(pCallBackSet->dwBeginCapturePicBufLen >= pcImage->dwImgDataLen)
					{
						memcpy(pCallBackSet->pBeginCapturePicBuf, pcImage->pbImgData, pcImage->dwImgDataLen);
						pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
							RECORD_BIGIMG_BEGIN_CAPTURE, pcImage->cImgInfo.dwWidth,
							pcImage->cImgInfo.dwHeight, pCallBackSet->pBeginCapturePicBuf,
							pcImage->dwImgDataLen, dwTimeMS);
					}
				}
			}
			else
			{
				if(pCallBackSet->dwBeginCapturePicBufLen >= pcImage->dwImgDataLen)
				{
					memcpy(pCallBackSet->pBeginCapturePicBuf, pcImage->pbImgData, pcImage->dwImgDataLen);
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEGIN_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pCallBackSet->pBeginCapturePicBuf,
						pcImage->dwImgDataLen, dwTimeMS);
				}
			}
		}
		break;
	case RECORD_BIGIMG_BEST_CAPTURE:
		{
			if((dwEnhanceFlag == 1 && fIsPeccancy)
				|| dwEnhanceFlag == 2 || dwEnhanceFlag == 3)
			{
				DWORD dwRetBufLen = pCallBackSet->dwBestCapturePicBufLen;
				HRESULT hr = HvEnhanceTrafficLight(pcImage->pbImgData, pcImage->dwImgDataLen,
					cImgInfo.nRedLightCount, (PBYTE)&cImgInfo.rcRedLightPos, pCallBackSet->pBestCapturePicBuf,
					dwRetBufLen, iBrightness, iHueThreshold, iCompressRate);
				if(hr == S_OK)
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pCallBackSet->pBestCapturePicBuf,
						dwRetBufLen, dwTimeMS);
				}
				else
				{
					if(pCallBackSet->dwBestCapturePicBufLen >= pcImage->dwImgDataLen)
					{
						memcpy(pCallBackSet->pBestCapturePicBuf, pcImage->pbImgData, pcImage->dwImgDataLen);
						pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
							RECORD_BIGIMG_BEST_CAPTURE, pcImage->cImgInfo.dwWidth,
							pcImage->cImgInfo.dwHeight, pCallBackSet->pBestCapturePicBuf,
							pcImage->dwImgDataLen, dwTimeMS);
					}
				}
			}
			else
			{
				if(pCallBackSet->dwBestCapturePicBufLen >= pcImage->dwImgDataLen)
				{
					memcpy(pCallBackSet->pBestCapturePicBuf, pcImage->pbImgData, pcImage->dwImgDataLen);
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pCallBackSet->pBestCapturePicBuf,
						pcImage->dwImgDataLen, dwTimeMS);
				}
			}
		}
		break;
	case RECORD_BIGIMG_LAST_CAPTURE:
		{
			if((dwEnhanceFlag == 1 && fIsPeccancy)
				|| dwEnhanceFlag == 2 || dwEnhanceFlag == 3)
			{
				DWORD dwRetBufLen = pCallBackSet->dwLastCapturePicBufLen;
				HRESULT hr = HvEnhanceTrafficLight(pcImage->pbImgData, pcImage->dwImgDataLen,
					cImgInfo.nRedLightCount, (PBYTE)&cImgInfo.rcRedLightPos, pCallBackSet->pLastCapturePicBuf,
					dwRetBufLen, iBrightness, iHueThreshold, iCompressRate);
				if(hr == S_OK)
				{
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pCallBackSet->pLastCapturePicBuf,
						dwRetBufLen, dwTimeMS);
				}
				else
				{
					if(pCallBackSet->dwLastCapturePicBufLen >= pcImage->dwImgDataLen)
					{
						memcpy(pCallBackSet->pLastCapturePicBuf, pcImage->pbImgData, pcImage->dwImgDataLen);
						pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
							RECORD_BIGIMG_LAST_CAPTURE, pcImage->cImgInfo.dwWidth,
							pcImage->cImgInfo.dwHeight, pCallBackSet->pLastCapturePicBuf,
							pcImage->dwImgDataLen, dwTimeMS);
					}
				}
			}
			else
			{
				if(pCallBackSet->dwLastCapturePicBufLen >= pcImage->dwImgDataLen)
				{
					memcpy(pCallBackSet->pLastCapturePicBuf, pcImage->pbImgData, pcImage->dwImgDataLen);
					pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pCallBackSet->pLastCapturePicBuf,
						pcImage->dwImgDataLen, dwTimeMS);
				}
			}
		}
		break;
	}
	*/
	SAFE_DELETE(pTempPic);
	return S_OK;
}

HRESULT ProcRecordDataPackt(HVAPI_HANDLE_CONTEXT_EX* pHHC, unsigned char* pbRecordData, 
							DWORD dwDataLen, DWORD dwRecordType, LPCSTR szAppendInfo)
{
	if(dwRecordType != RECORD_TYPE_NORMAL && dwRecordType != RECORD_TYPE_HISTORY)
	{
		return E_FAIL;
	}
	if(szAppendInfo == NULL)
	{
		return E_FAIL;
	}

	DWORD dwBufLen = 50;
	char* pszPlateBuf = new char[dwBufLen];
	memset(pszPlateBuf, 0, dwBufLen);
	if(HVAPIUTILS_GetRecordInfoFromAppenedStringEx(szAppendInfo, "PlateName", pszPlateBuf, 50) != S_OK)
	{
		SAFE_DELETE(pszPlateBuf);
		return E_FAIL;
	}

	DWORD dwCarID, dwTimeHigh, dwTimeLow;
	char* pszValue = new char[dwBufLen];
	memset(pszValue, 0, dwBufLen);
	if(HVAPIUTILS_GetRecordInfoFromAppenedStringEx(szAppendInfo, "CarID", pszValue, dwBufLen) != S_OK)
	{
		SAFE_DELETE(pszPlateBuf);
		SAFE_DELETE(pszValue);
		return E_FAIL;
	}
	dwCarID = atoi(pszValue);

	memset(pszValue, 0, dwBufLen);
	if(HVAPIUTILS_GetRecordInfoFromAppenedStringEx(szAppendInfo, "TimeHigh", pszValue, dwBufLen) != S_OK)
	{
		SAFE_DELETE(pszPlateBuf);
		SAFE_DELETE(pszValue);
		return E_FAIL;
	}
	dwTimeHigh = atoi(pszValue);
	
	memset(pszValue, 0, dwBufLen);
	if(HVAPIUTILS_GetRecordInfoFromAppenedStringEx(szAppendInfo, "TimeLow", pszValue, dwBufLen) != S_OK)
	{
		SAFE_DELETE(pszPlateBuf);
		SAFE_DELETE(pszValue);
		return E_FAIL;
	}
	dwTimeLow = atoi(pszValue);

	SAFE_DELETE(pszValue);
	DWORD64 dw64TimeMS = ((DWORD64)(dwTimeHigh)<<32) | dwTimeLow;
	HVAPI_CALLBACK_SET* pTemp;
	for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
	{
		if(pTemp->pOnRecordBegin)
		{
			pTemp->pOnRecordBegin(pTemp->pOnRecordBeginParam, dwCarID);
		}
		if(pTemp->pOnPlate)
		{
			pTemp->pOnPlate(pTemp->pOnPlateParam, dwCarID, pszPlateBuf, szAppendInfo, dwRecordType, dw64TimeMS);
		}
	}

	SAFE_DELETE(pszPlateBuf);
	if(pbRecordData == NULL || dwDataLen == 0)
	{
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnRecordEnd)
			{
				pTemp->pOnRecordEnd(pTemp->pOnRecordEndParam, dwCarID);
			}
		}
		return S_OK;
	}
	
	RECORD_IMAGE_GROUP_EX cImgGroup;
	if(HvGetRecordImage(pbRecordData, dwDataLen, &cImgGroup) != S_OK)
	{
		WrightLogEx(pHHC->szIP, "Get Picture Frome Result Faile...");
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnRecordEnd)
			{
				pTemp->pOnRecordEnd(pTemp->pOnRecordEndParam, dwCarID);
			}
		}
		return E_FAIL;
	}
	
	BOOL fIsPeccancy = FALSE;
	if(strstr(szAppendInfo, "违章:是"))
	{
		fIsPeccancy = TRUE;
	}

	for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
	{
		if(pTemp->iVideoID == 0)
		{
			if(pTemp->pOnBigImage != NULL)
			{
				if(cImgGroup.cBestSnapshot.pbImgData)
				{
					pHHC->rcPlate[0] = cImgGroup.cBestSnapshot.cImgInfo.rcPlate;
					pHHC->nFaceCount[0] = cImgGroup.cBestSnapshot.cImgInfo.nFaceCount;
					memcpy(pHHC->rcFacePos[0], cImgGroup.cBestSnapshot.cImgInfo.rcFacePos, sizeof(pHHC->rcFacePos[0]));
					ProcBigImageCallBack(pTemp, RECORD_BIGIMG_BEST_SNAPSHOT, &cImgGroup.cBestSnapshot, fIsPeccancy,
						pHHC->dwEnhanceRedLightFlag, pHHC->iBigPicBrightness, pHHC->iBigPicHueThrshold,
						pHHC->iBigPicCompressRate, dwRecordType, cImgGroup.cBestSnapshot.cImgInfo.dw64TimeMS);
				}

				if(cImgGroup.cLastSnapshot.pbImgData)
				{
					pHHC->rcPlate[1] = cImgGroup.cLastSnapshot.cImgInfo.rcPlate;
					pHHC->nFaceCount[1] = cImgGroup.cLastSnapshot.cImgInfo.nFaceCount;
					memcpy(pHHC->rcFacePos[1], cImgGroup.cLastSnapshot.cImgInfo.rcFacePos, sizeof(pHHC->rcFacePos[1]));
					ProcBigImageCallBack(pTemp, RECORD_BIGIMG_LAST_SNAPSHOT, &cImgGroup.cLastSnapshot, fIsPeccancy,
						pHHC->dwEnhanceRedLightFlag, pHHC->iBigPicBrightness, pHHC->iBigPicHueThrshold,
						pHHC->iBigPicCompressRate, dwRecordType, cImgGroup.cLastSnapshot.cImgInfo.dw64TimeMS);
				}

				if(cImgGroup.cBeginCapture.pbImgData)
				{
					pHHC->rcPlate[2] = cImgGroup.cBeginCapture.cImgInfo.rcPlate;
					pHHC->nFaceCount[2] = cImgGroup.cBeginCapture.cImgInfo.nFaceCount;
					memcpy(pHHC->rcFacePos[2], cImgGroup.cBeginCapture.cImgInfo.rcFacePos, sizeof(pHHC->rcFacePos[2]));
					ProcBigImageCallBack(pTemp, RECORD_BIGIMG_BEGIN_CAPTURE, &cImgGroup.cBeginCapture, fIsPeccancy,
						pHHC->dwEnhanceRedLightFlag, pHHC->iBigPicBrightness, pHHC->iBigPicHueThrshold,
						pHHC->iBigPicCompressRate, dwRecordType, cImgGroup.cBeginCapture.cImgInfo.dw64TimeMS);
				}

				if(cImgGroup.cBestCapture.pbImgData)
				{
					pHHC->rcPlate[3] = cImgGroup.cBestCapture.cImgInfo.rcPlate;
					pHHC->nFaceCount[3] = cImgGroup.cBestCapture.cImgInfo.nFaceCount;
					memcpy(pHHC->rcFacePos[3], cImgGroup.cBestCapture.cImgInfo.rcFacePos, sizeof(pHHC->rcFacePos[3]));
					ProcBigImageCallBack(pTemp, RECORD_BIGIMG_BEST_CAPTURE, &cImgGroup.cBestCapture, fIsPeccancy,
						pHHC->dwEnhanceRedLightFlag, pHHC->iBigPicBrightness, pHHC->iBigPicHueThrshold,
						pHHC->iBigPicCompressRate, dwRecordType, cImgGroup.cBestCapture.cImgInfo.dw64TimeMS);
				}

				if(cImgGroup.cLastCapture.pbImgData)
				{
					pHHC->rcPlate[4] = cImgGroup.cLastCapture.cImgInfo.rcPlate;
					pHHC->nFaceCount[4] = cImgGroup.cLastCapture.cImgInfo.nFaceCount;
					memcpy(pHHC->rcFacePos[4], cImgGroup.cLastCapture.cImgInfo.rcFacePos, sizeof(pHHC->rcFacePos[4]));
					ProcBigImageCallBack(pTemp, RECORD_BIGIMG_LAST_CAPTURE, &cImgGroup.cLastCapture, fIsPeccancy,
						pHHC->dwEnhanceRedLightFlag, pHHC->iBigPicBrightness, pHHC->iBigPicHueThrshold,
						pHHC->iBigPicCompressRate, dwRecordType, cImgGroup.cLastCapture.cImgInfo.dw64TimeMS);
				}
			}
		}
	}

	if(cImgGroup.cPlatePicture.pbImgData)
	{
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnSmallImage)
			{
				pTemp->pOnSmallImage(pTemp->pOnSmallImageParam, cImgGroup.cPlatePicture.cImgInfo.dwCarID,
					cImgGroup.cPlatePicture.cImgInfo.dwWidth, cImgGroup.cPlatePicture.cImgInfo.dwHeight,
					cImgGroup.cPlatePicture.pbImgData, cImgGroup.cPlatePicture.dwImgDataLen, dwRecordType, dw64TimeMS);
			}
		}
	}

	if(cImgGroup.cPlateBinary.pbImgData)
	{
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnBinaryImage)
			{
				pTemp->pOnBinaryImage(pTemp->pOnBinaryImageParam, cImgGroup.cPlateBinary.cImgInfo.dwCarID,
					cImgGroup.cPlateBinary.cImgInfo.dwWidth, cImgGroup.cPlateBinary.cImgInfo.dwHeight,
					cImgGroup.cPlateBinary.pbImgData, cImgGroup.cPlateBinary.dwImgDataLen, dwRecordType, dw64TimeMS);
			}
		}
	}

	for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
	{
		if(pTemp->pOnRecordEnd)
		{
			pTemp->pOnRecordEnd(pTemp->pOnRecordEndParam, dwCarID);
		}
	}

	if(pHHC->fIsRecvHistoryRecord)
	{
		CTime cTimeCurrentTime(dw64TimeMS/1000);
		CString strCurrentTime;
		strCurrentTime = cTimeCurrentTime.Format("%Y.%m.%d_%H");
		memcpy(pHHC->szRecordBeginTimeStr, strCurrentTime.GetBuffer(), 13);
		pHHC->szRecordBeginTimeStr[13] = '\0';
		pHHC->dwRecordStartIndex = dwCarID+1;

		if (pHHC->fAutoLink && pHHC->fVailPackResumeCache)
		{
			PACK_RESUME_CACHE* pPackResumeCache = pHHC->pPackResumeCache;

			pPackResumeCache->fVailHeader = FALSE;
			pPackResumeCache->fVailInfor = FALSE;
			pPackResumeCache->fVailData = FALSE;

			pPackResumeCache->nDataLen = 0;
			pPackResumeCache->nInforLen = 0;

			pPackResumeCache->nInfoOffset = 0;
			pPackResumeCache->nDataOffset = 0;
		}
	}
	return S_OK;
}

DWORD WINAPI RecordRecvThreadFuncEx(LPVOID lpParam)
{
	if(lpParam == NULL)
	{
		return -1;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)lpParam;
	if(strcmp(pHHC->szVersion, HVAPI_API_VERSION_EX) != 0
		|| pHHC->dwOpenType != 1)
	{
		return -1;
	}
	INFO_HEADER cInfoHeader;
	INFO_HEADER cInfoHeaderResponse;
	BLOCK_HEADER cBlockHeader;
	unsigned char* pbInfo = NULL;
	unsigned char* pbData = NULL;
	cInfoHeaderResponse.dwType = CAMERA_THROB_RESPONSE;
	cInfoHeaderResponse.dwDataLen = 0;
	cInfoHeaderResponse.dwInfoLen = 0;
	pHHC->dwRecordStreamTick = GetTickCount();
	while(!pHHC->fIsThreadRecvRecordExit)
	{
		if(pHHC->dwRecordConnStatus == CONN_STATUS_NORMAL)
		{
			if(GetTickCount() - pHHC->dwRecordStreamTick > 8000)
			{
				WrightLogEx(pHHC->szIP, "[RecordLink] Reconnect,Last Result Proces TimeOut...");
				pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
			}
		}

		if(pHHC->dwRecordConnStatus == CONN_STATUS_RECONN
			|| pHHC->dwRecordConnStatus == CONN_STATUS_DISCONN)
		{
			if (pHHC->fAutoLink)
			{
				ForceCloseSocket(pHHC->sktRecord);
			}
			Sleep(1000);
			continue;
		}

		if(RecvAll(pHHC->sktRecord, (char*)&cInfoHeader, sizeof(cInfoHeader)) == 
			sizeof(cInfoHeader))
		{
			SAFE_DELETE_ARG(pbInfo);
			SAFE_DELETE_ARG(pbData);
			pHHC->dwRecordStreamTick = GetTickCount();
			if(cInfoHeader.dwType == CAMERA_THROB)
			{
				if(send(pHHC->sktRecord, (char*)&cInfoHeaderResponse, 
					sizeof(cInfoHeaderResponse), 0) != sizeof(cInfoHeaderResponse))
				{
					WrightLogEx(pHHC->szIP, "[RecordLink] Reconnect,Throb Response Send Failed...");
					pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
				}
				continue;
			}
			
			if (pHHC->fAutoLink && pHHC->fVailPackResumeCache && cInfoHeader.dwType == CAMERA_RECORD)
			{
				UpdatePackCache(pHHC , PACK_TYPE_HEADER  , (char*)&cInfoHeader , sizeof(cInfoHeader));
			}

			if(cInfoHeader.dwType == CAMERA_HISTORY_END)
			{
				WrightLogEx(pHHC->szIP, "[RecordLink] Quit,Recive Done...");
				if (pHHC->fAutoLink)
				{
					pHHC->fVailPackResumeCache = FALSE;
					ForceCloseSocket(pHHC->sktRecord);
					pHHC->fIsThreadRecvRecordExit = TRUE;
					pHHC->hThreadRecvRecord = NULL;
				}
				pHHC->dwRecordConnStatus = CONN_STATUS_RECVDONE;
				ZeroMemory(pHHC->szRecordConnCmd, sizeof(pHHC->szRecordConnCmd));
				break;
			}

			DWORD32 dwBuffLen = 10 * 1024 * 1024;
			if (cInfoHeader.dwInfoLen > dwBuffLen || cInfoHeader.dwDataLen > dwBuffLen)
			{
				WrightLogEx(pHHC->szIP, "[RecordLink] Quit,Recive Data Error...");
				if (pHHC->fAutoLink)
				{
					pHHC->fVailPackResumeCache = FALSE;
					ForceCloseSocket(pHHC->sktRecord);
					pHHC->fIsThreadRecvRecordExit = TRUE;
					pHHC->hThreadRecvRecord = NULL;
				}
				pHHC->dwRecordConnStatus = CONN_STATUS_RECVDONE;
				ZeroMemory(pHHC->szRecordConnCmd, sizeof(pHHC->szRecordConnCmd));
				break;
			}

			pbInfo = new unsigned char[cInfoHeader.dwInfoLen + 1];
			pbData = new unsigned char[cInfoHeader.dwDataLen + 1];
			int ipbInfoSize = (int)cInfoHeader.dwInfoLen + 1;

			if(pbInfo == NULL || pbData == NULL)
			{
				Sleep(100);
				WrightLogEx(pHHC->szIP, "[RecordLink] Reconnect,Malloc Buffer Failed...");
				pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
				continue;
			}
			else
			{
				ZeroMemory(pbInfo, cInfoHeader.dwInfoLen);
				ZeroMemory(pbData, cInfoHeader.dwDataLen);
			}

			if(cInfoHeader.dwInfoLen > 0)
			{
				int nInfoLen = 0;
				int nRet = RecvAll(pHHC->sktRecord, (char*)pbInfo, cInfoHeader.dwInfoLen ,nInfoLen);
				bool fRecvFail = cInfoHeader.dwInfoLen !=  nInfoLen;

				if ( pHHC->fAutoLink && pHHC->fVailPackResumeCache && cInfoHeader.dwType == CAMERA_RECORD)
				{
					UpdatePackCache(pHHC , PACK_TYPE_INFO , (char*)pbInfo , nInfoLen);
				}

				if (fRecvFail)
				{
					Sleep(100);
					WrightLogEx(pHHC->szIP, "[RecordLink] Reconnect,Recv Packet Header Failed...");
					pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
					continue;
				}
			}

			if(cInfoHeader.dwDataLen > 0)
			{
				int nDataLen = 0;
				int nRet = RecvAll(pHHC->sktRecord, (char*)pbData, cInfoHeader.dwDataLen , nDataLen);
				bool fRecvFail = cInfoHeader.dwDataLen != nRet;

				if ( pHHC->fAutoLink && pHHC->fVailPackResumeCache && cInfoHeader.dwType == CAMERA_RECORD)
				{
					if (NULL != pHHC->pPackResumeCache)
					{
						pHHC->pPackResumeCache->nDataOffset = nDataLen;
					}

					bool fIsEmptyDataPackCache = true;
					if (S_OK == IsEmptyPackCache(pHHC , PACK_TYPE_DATA ,fIsEmptyDataPackCache))
					{
						if (fRecvFail || !fIsEmptyDataPackCache)
						{
							UpdatePackCache(pHHC , PACK_TYPE_DATA , (char*)pbData , nDataLen);
						}
					}
				}
				if (fRecvFail)
				{
					Sleep(100);
					WrightLogEx(pHHC->szIP, "[RecordLink] Reconnect,Recv Packet Data Failed...");
					pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
					continue;
				}

				unsigned char* pRecordInfo = pbInfo;
				unsigned char* pRecordData = pbData;
				int nRecordInfoLen = cInfoHeader.dwInfoLen;
				int nRecordDataLen = cInfoHeader.dwDataLen;

				if (pHHC->fAutoLink && pHHC->fVailPackResumeCache && cInfoHeader.dwType == CAMERA_RECORD)
				{
					bool fEmptyCacheInfoPack = true;
					bool fEmptyCacheDataPack = true;
					if (S_OK == IsEmptyPackCache(pHHC , PACK_TYPE_INFO ,fEmptyCacheInfoPack))
					{
						if (!fEmptyCacheInfoPack)
						{
							GetPackCache(pHHC , PACK_TYPE_INFO , &pRecordInfo , nRecordInfoLen);
						}
					}

					if (S_OK == IsEmptyPackCache(pHHC , PACK_TYPE_DATA , fEmptyCacheDataPack))
					{
						if (!fEmptyCacheDataPack)
						{
							GetPackCache(pHHC , PACK_TYPE_DATA , &pRecordData , nRecordDataLen);
						}
					}
				}

				if(cInfoHeader.dwType == CAMERA_RECORD)
				{
					DWORD32 dwRecordType = 0;
					DWORD dwAppendInfoBufLen = 1024;
					dwAppendInfoBufLen = (dwAppendInfoBufLen << 5);

					int nAppendInfoLen = 0;

					char* szAppendInfo = new char[dwAppendInfoBufLen];
					memset(szAppendInfo, 0, dwAppendInfoBufLen);
					unsigned char* pbTemp =  pRecordInfo;
					int iFlag = 0;
					for(int i=0; i<(int)nRecordInfoLen;)
					{
						if ((ipbInfoSize - i) < sizeof(BLOCK_HEADER))
						{
							iFlag = 1;
							break;
						}
						memcpy(&cBlockHeader, pbTemp, sizeof(BLOCK_HEADER));
						pbTemp += sizeof(BLOCK_HEADER);
						i += sizeof(BLOCK_HEADER);

						if(cBlockHeader.dwID == BLOCK_RECORD_TYPE)
						{
							if(cBlockHeader.dwLen == sizeof(dwRecordType))
							{
								if ((ipbInfoSize - i) < (int)cBlockHeader.dwLen)
								{
									iFlag = 1;
									break;
								}
								memcpy(&dwRecordType, pbTemp, cBlockHeader.dwLen);
							}
						}
						else if(cBlockHeader.dwID == BLOCK_XML_TYPE)
						{
							if(cBlockHeader.dwLen <= dwAppendInfoBufLen)
							{
								if ((ipbInfoSize - i) < (int)cBlockHeader.dwLen)
								{
									iFlag = 1;
									break;
								}
								memcpy(szAppendInfo, pbTemp, cBlockHeader.dwLen);
								nAppendInfoLen = cBlockHeader.dwLen;	
							}
						}
						pbTemp += cBlockHeader.dwLen;
						i += cBlockHeader.dwLen;
					}

					if (iFlag)
					{
						Sleep(100);
						WrightLogEx(pHHC->szIP, "[RecordLink] Reconnect,Copy InfoData Failed...");
						pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
						continue;
					}

					DWORD dwType = RECORD_TYPE_UNKNOWN;
					if(dwRecordType == CAMERA_RECORD_NORMAL)
					{
						dwType = RECORD_TYPE_NORMAL;
					}
					else if(dwRecordType == CAMERA_RECORD_HISTORY)
					{
						dwType = RECORD_TYPE_HISTORY;
					}

					if(dwRecordType == CAMERA_RECORD_NORMAL 
						|| dwRecordType == CAMERA_RECORD_HISTORY)
					{
						ProcRecordDataPackt(pHHC, pRecordData, nRecordDataLen, dwType, szAppendInfo);
					}
					SAFE_DELETE(szAppendInfo);
				}
				else if(cInfoHeader.dwType == CAMERA_STRING)
				{
					HVAPI_CALLBACK_SET* pTemp;
					for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
					{
						if(pTemp->pOnString)
						{
							pTemp->pOnString(pTemp->pOnStringParam, (LPCSTR)pbData, cInfoHeader.dwDataLen);
						}
					}
				}
			}
			else
			{
				if(cInfoHeader.dwType == CAMERA_RECORD)
				{
					DWORD32 dwRecordType = 0;
					DWORD dwAppendInfoBufLen = 1024;
					dwAppendInfoBufLen = (dwAppendInfoBufLen << 5);

					int nAppendInfoLen = 0;

					char* szAppendInfo = new char[dwAppendInfoBufLen];
					memset(szAppendInfo, 0, dwAppendInfoBufLen);
					unsigned char* pbTemp =  pbInfo;
					int iFlag = 0;
					for(int i=0; i<(int)cInfoHeader.dwInfoLen;)
					{
						if ((ipbInfoSize - i) < sizeof(BLOCK_HEADER))
						{
							iFlag = 1;
							break;
						}
						memcpy(&cBlockHeader, pbTemp, sizeof(BLOCK_HEADER));
						pbTemp += sizeof(BLOCK_HEADER);
						i += sizeof(BLOCK_HEADER);

						if(cBlockHeader.dwID == BLOCK_RECORD_TYPE)
						{
							if(cBlockHeader.dwLen == sizeof(dwRecordType))
							{
								if ((ipbInfoSize - i) < (int)cBlockHeader.dwLen)
								{
									iFlag = 1;
									break;
								}
								memcpy(&dwRecordType, pbTemp, cBlockHeader.dwLen);
							}
						}
						else if(cBlockHeader.dwID == BLOCK_XML_TYPE)
						{
							if(cBlockHeader.dwLen <= dwAppendInfoBufLen)
							{
								if ((ipbInfoSize - i) < (int)cBlockHeader.dwLen)
								{
									iFlag = 1;
									break;
								}
								memcpy(szAppendInfo, pbTemp, cBlockHeader.dwLen);
								nAppendInfoLen = cBlockHeader.dwLen;
							}
						}
						pbTemp += cBlockHeader.dwLen;
						i += cBlockHeader.dwLen;
					}

					if (iFlag)
					{
						Sleep(100);
						WrightLogEx(pHHC->szIP, "[RecordLink] Reconnect,Copy InfoData Failed...");
						pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
						continue;
					}

					DWORD dwType = RECORD_TYPE_UNKNOWN;
					if(dwRecordType == CAMERA_RECORD_NORMAL)
					{
						dwType = RECORD_TYPE_NORMAL;
					}
					else if(dwRecordType == CAMERA_RECORD_HISTORY)
					{
						dwType = RECORD_TYPE_HISTORY;
					}

					if(dwRecordType == CAMERA_RECORD_NORMAL 
						|| dwRecordType == CAMERA_RECORD_HISTORY)
					{
						ProcRecordDataPackt(pHHC, NULL, 0, dwType, szAppendInfo);
					}
					SAFE_DELETE(szAppendInfo);
				}
			}
		}
		else
		{
			WrightLogEx(pHHC->szIP, "[RecordLink] Reconnect,Recv Throb Packet TimeOut...");
			pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
			Sleep(100);
		}
	}

	SAFE_DELETE(pbInfo);
	SAFE_DELETE(pbData);
	return 0;
}

HRESULT SaveRecordConnCmd(HVAPI_HANDLE_CONTEXT_EX* pHHC)
{
	if(strstr((char*)pHHC->szRecordConnCmd, "DownloadRecord")
		&& strstr((char*)pHHC->szRecordConnCmd, "Enable[1]"))
	{
		pHHC->fIsRecvHistoryRecord = TRUE;
		char* pTempStr = strstr((char*)pHHC->szRecordConnCmd, "BeginTime");
		if(pTempStr)
		{
			memcpy(pHHC->szRecordBeginTimeStr, pTempStr+10, 13);
			pHHC->szRecordBeginTimeStr[13] = '\0';
		}

		pTempStr = strstr((char*)pHHC->szRecordConnCmd, "Index");
		if(pTempStr)
		{
			sscanf(pTempStr, "Index[%d]", &pHHC->dwRecordStartIndex);
		}

		pTempStr = strstr((char*)pHHC->szRecordConnCmd, "EndTime");
		if(pTempStr)
		{
			if(pTempStr[9] == ']')
			{
				memset(pHHC->szRecordEndTimeStr, 0, 14);
				pHHC->szRecordEndTimeStr[0] = '0';
			}
			else
			{
				memcpy(pHHC->szRecordEndTimeStr, pTempStr+10, 13);
				pHHC->szRecordEndTimeStr[13] = '\0';
			}
		}

		pTempStr = strstr((char*)pHHC->szRecordConnCmd, "DataInfo");
		if(pTempStr)
		{
			sscanf(pTempStr, "DataInfo[%d]", &pHHC->iRecordDataInfo);
		}
	}

	return S_OK;;
}


HRESULT ProcLprImageFrame(HVAPI_HANDLE_CONTEXT_EX* pHHC, PBYTE pbImgData, DWORD dwImgDataLen,
						  DWORD dwImgDataOffSet, char* pszImageExtInfo)
{
	HVAPI_CALLBACK_SET* pTemp = NULL;
	int iPlateCount = 0;
	char* pszTemp = strstr((char*)pbImgData, "rect");
	if(pszTemp == NULL)
	{
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnJpegFrame)
			{
				pTemp->pOnJpegFrame(pTemp->pOnJpegFrameParam, pbImgData+dwImgDataOffSet, 
					dwImgDataLen-dwImgDataOffSet, IMAGE_TYPE_JPEG_LPR, pszImageExtInfo);
			}
		}
		return S_OK;
	}
	pszTemp = pszTemp + 4;
	memcpy(&iPlateCount, pszTemp, 4);
	if(iPlateCount <= 0 || iPlateCount > 20)
	{
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnJpegFrame)
			{
				pTemp->pOnJpegFrame(pTemp->pOnJpegFrameParam, pbImgData+dwImgDataOffSet, 
					dwImgDataLen-dwImgDataOffSet, IMAGE_TYPE_JPEG_LPR, pszImageExtInfo);
			}
		}
		return S_OK;
	}
	pszTemp = pszTemp + 4;
	IStream* pStm = NULL;
	CreateStreamOnHGlobal(NULL, TRUE, &pStm);
	if(pStm == NULL)
	{
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnJpegFrame)
			{
				pTemp->pOnJpegFrame(pTemp->pOnJpegFrameParam, pbImgData+dwImgDataOffSet, 
					dwImgDataLen-dwImgDataOffSet, IMAGE_TYPE_JPEG_LPR, pszImageExtInfo);
			}
		}
		return S_OK;
	}

	IStream* pStmDest = NULL;
	CreateStreamOnHGlobal(NULL, TRUE, &pStmDest);
	if(pStmDest == NULL)
	{
		pStm->Release();
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnJpegFrame)
			{
				pTemp->pOnJpegFrame(pTemp->pOnJpegFrameParam, pbImgData+dwImgDataOffSet, 
					dwImgDataLen-dwImgDataOffSet, IMAGE_TYPE_JPEG_LPR, pszImageExtInfo);
			}
		}
		return S_OK;
	}

	LARGE_INTEGER liTmp = {0};
	pStm->Write(pbImgData+dwImgDataOffSet, dwImgDataLen-dwImgDataOffSet, NULL);
	Bitmap* pbmp = Bitmap::FromStream(pStm);
	Graphics grfTmp(pbmp);
	SolidBrush sBrush(Color(255, 255, 0, 0));
	Pen redPen(&sBrush, 5.0f);
	int iIndex = 0;
	for(iIndex=0; iIndex<iPlateCount; iIndex++)
	{
		RECT pRect;
		memset(&pRect, 0, sizeof(RECT));
		memcpy(&pRect, pszTemp, sizeof(RECT));
		pszTemp += sizeof(RECT);
		grfTmp.DrawRectangle(&redPen, (REAL)pRect.left, (REAL)pRect.top,
			(REAL)(pRect.right - pRect.left), (REAL)(pRect.bottom - pRect.top));
	}

	pStm->Seek(liTmp, STREAM_SEEK_SET, NULL);
	pStmDest->Seek(liTmp, STREAM_SEEK_SET, NULL);
	pbmp->Save(pStmDest, &g_jpgClsid);
	pStm->Seek(liTmp, STREAM_SEEK_SET, NULL);
	pStmDest->Seek(liTmp, STREAM_SEEK_SET, NULL);

	ULONG ulTmp = 0;
	PBYTE pbImageData = new BYTE[(dwImgDataLen-dwImgDataOffSet)<<1];
	pStmDest->Read(pbImageData, (dwImgDataLen-dwImgDataOffSet)<<1, &ulTmp);
	if(pbmp)
	{
		delete pbmp;
	}
	pStm->Release();
	pStmDest->Release();

	for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
	{
		if(pTemp->pOnJpegFrame)
		{
			pTemp->pOnJpegFrame(pTemp->pOnJpegFrameParam, pbImageData, 
				(DWORD)ulTmp, IMAGE_TYPE_JPEG_LPR, pszImageExtInfo);
		}
	}

	SAFE_DELETE(pbImageData);
	return S_OK;
}

static DWORD WINAPI ImageRecvThreadFuncEx(LPVOID lpParam)
{
	if(lpParam == NULL)
	{
		return -1;
	}
	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)lpParam;
	INFO_HEADER cInfoHeader;
	INFO_HEADER cInfoHeaderResponse;
	BLOCK_HEADER cBlockHeader;
	unsigned char* pbInfo = NULL;
	unsigned char* pbData = NULL;
	cInfoHeaderResponse.dwType = CAMERA_THROB_RESPONSE;
	cInfoHeaderResponse.dwInfoLen = 0;
	cInfoHeaderResponse.dwDataLen = 0;
	pHHC->dwImageStreamTick = GetTickCount();

	while(pHHC->fIsThreadRecvImageExit != TRUE)
	{
		if(pHHC->dwImageConnStatus == CONN_STATUS_NORMAL)
		{
			if(GetTickCount() - pHHC->dwImageStreamTick > 8000)
			{
				WrightLogEx(pHHC->szIP, "[ImageLink] Reconnect,Last Result Proces TimeOut...");
				pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
			}
		}
		if(pHHC->dwImageConnStatus == CONN_STATUS_RECONN
			|| pHHC->dwImageConnStatus == CONN_STATUS_DISCONN)
		{
			Sleep(1000);
			continue;
		}

		if(RecvAll(pHHC->sktImage, (char*)&cInfoHeader, sizeof(cInfoHeader)) == sizeof(cInfoHeader))
		{
			SAFE_DELETE(pbInfo);
			SAFE_DELETE(pbData);
			pHHC->dwImageStreamTick = GetTickCount();

			if(cInfoHeader.dwType == CAMERA_THROB)
			{
				if(send(pHHC->sktImage, (char*)&cInfoHeaderResponse, sizeof(cInfoHeaderResponse), 0)
					!= sizeof(cInfoHeaderResponse))
				{
					WrightLogEx(pHHC->szIP, "[ImageLink] Reconnect,Throb Response Send Failed...");
					pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
				}
				continue;
			}

			if(cInfoHeader.dwType == CAMERA_HISTORY_END)
			{
				WrightLogEx(pHHC->szIP, "[ImagedLink] Quit,Recive Done...");
				pHHC->dwImageConnStatus = CONN_STATUS_RECVDONE;
				ZeroMemory(pHHC->szImageConnCmd, sizeof(pHHC->szImageConnCmd));
				break;
			}

			DWORD32 dwBuffLen = 10 * 1024 * 1024;
			if (cInfoHeader.dwInfoLen > dwBuffLen || cInfoHeader.dwDataLen > dwBuffLen)
			{
				WrightLogEx(pHHC->szIP, "[ImagedLink] Quit,Recive Data Error...");
				pHHC->dwImageConnStatus = CONN_STATUS_RECVDONE;
				ZeroMemory(pHHC->szImageConnCmd, sizeof(pHHC->szImageConnCmd));
				break;
			}

			pbInfo = new unsigned char[cInfoHeader.dwInfoLen + 1];
			pbData = new unsigned char[cInfoHeader.dwDataLen + 1];
			int ipbInfoLSize = (int)cInfoHeader.dwInfoLen + 1;
			if(pbInfo == NULL || pbData == NULL)
			{
				WrightLogEx(pHHC->szIP, "[ImageLink] Reconnect,Malloc Buffer Failed...");
				Sleep(100);
				pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
				continue;
			}
			else
			{
				ZeroMemory(pbInfo, cInfoHeader.dwInfoLen+1);
				ZeroMemory(pbData, cInfoHeader.dwDataLen+1);
			}

			if(cInfoHeader.dwInfoLen > 0)
			{
				if(RecvAll(pHHC->sktImage, (char*)pbInfo, cInfoHeader.dwInfoLen)
					!= cInfoHeader.dwInfoLen)
				{
					Sleep(100);
					WrightLogEx(pHHC->szIP, "[ImageLink] Reconnect,Recv Packet Header Failed...");
					pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
					continue;
				}
			}

			if(cInfoHeader.dwDataLen > 0)
			{
				if(RecvAll(pHHC->sktImage, (char*)pbData, cInfoHeader.dwDataLen) 
					!= cInfoHeader.dwDataLen)
				{
					Sleep(100);
					WrightLogEx(pHHC->szIP, "[ImageLink] Reconnect,Recv Packet Data Failed...");
					pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
					continue;
				}

				if(cInfoHeader.dwType == CAMERA_IMAGE)
				{
					DWORD32 dwImageType = 0;
					DWORD32 dwImageWidth = 0;
					DWORD32 dwImageHeight = 0;
					DWORD64 dw64ImageTime = 0;
					DWORD32 dwImageOffset = 0;
					ImageExtInfo cImageExtInfo = {0};
					unsigned char* pbTemp = pbInfo;
					int iFlag = 0;
					for( int i = 0; i < (int)cInfoHeader.dwInfoLen;)
					{
						if ((ipbInfoLSize - i) < sizeof(BLOCK_HEADER))
						{
							iFlag = 1;
							break;
						}
						memcpy(&cBlockHeader,  pbTemp, sizeof(BLOCK_HEADER));
						pbTemp += sizeof(BLOCK_HEADER);
						i += sizeof(BLOCK_HEADER);

						if( cBlockHeader.dwID == BLOCK_IMAGE_TYPE )
						{
							if ((ipbInfoLSize - i) < sizeof(dwImageType))
							{
								iFlag = 1;
								break;
							}
							memcpy(&dwImageType, pbTemp, sizeof(dwImageType));
						}
						else if( cBlockHeader.dwID == BLOCK_IMAGE_WIDTH )
						{
							if ((ipbInfoLSize - i) < sizeof(dwImageWidth))
							{
								iFlag = 1;
								break;
							}
							memcpy(&dwImageWidth, pbTemp, sizeof(dwImageWidth));
						}
						else if( cBlockHeader.dwID == BLOCK_IMAGE_HEIGHT )
						{
							if ((ipbInfoLSize - i) < sizeof(dwImageHeight))
							{
								iFlag = 1;
								break;
							}
							memcpy(&dwImageHeight, pbTemp, sizeof(dwImageHeight));
						}
						else if( cBlockHeader.dwID == BLOCK_IMAGE_TIME )
						{
							if ((ipbInfoLSize - i) < sizeof(dw64ImageTime))
							{
								iFlag = 1;
								break;
							}
							memcpy(&dw64ImageTime, pbTemp, sizeof(dw64ImageTime));
						}
						else if( cBlockHeader.dwID == BLOCK_IMAGE_OFFSET )
						{
							if ((ipbInfoLSize - i) < sizeof(dwImageOffset))
							{
								iFlag = 1;
								break;
							}
							memcpy(&dwImageOffset, pbTemp, sizeof(dwImageOffset));
						}
						else if( cBlockHeader.dwID == BLOCK_IMAGE_EXT_INFO )
						{
							if ((ipbInfoLSize - i) < sizeof(cImageExtInfo))
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
						WrightLogEx(pHHC->szIP, "[ImageLink] Reconnect,Copy Data Failed...");
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
						char* pszImageExtInfo = new char[128];
						if(pszImageExtInfo)
						{
							sprintf(pszImageExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d,FrameTime:%I64u",
								cImageExtInfo.iShutter, cImageExtInfo.iGain, cImageExtInfo.iGainR,
								cImageExtInfo.iGainG, cImageExtInfo.iGainB, dw64ImageTime);
						}
						ProcLprImageFrame(pHHC, pbData, cInfoHeader.dwDataLen, dwImageOffset, pszImageExtInfo);
						SAFE_DELETE(pszImageExtInfo);
						continue;
					}

					char* pszImageExtInfo = new char[128];
					if(pszImageExtInfo)
					{
						sprintf(pszImageExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d,FrameTime:%I64u",
							cImageExtInfo.iShutter, cImageExtInfo.iGain, cImageExtInfo.iGainR,
							cImageExtInfo.iGainG, cImageExtInfo.iGainB, dw64ImageTime);
					}
					HVAPI_CALLBACK_SET* pTemp = NULL;
					for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
					{
						if(pTemp->pOnJpegFrame)
						{
							pTemp->pOnJpegFrame(pTemp->pOnJpegFrameParam, pbData+dwImageOffset, 
								cInfoHeader.dwDataLen-dwImageOffset, dwType, pszImageExtInfo);
						}
					}
					SAFE_DELETE(pszImageExtInfo);
				}
			}
		}
		else
		{
			WrightLogEx(pHHC->szIP, "[ImageLink] Reconnect,Recv Throb Packet TimeOut...");
			pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
			Sleep(100);
		}
	}
	SAFE_DELETE(pbInfo);
	SAFE_DELETE(pbData);
	return 0;
}

HRESULT ProcHistoryVideoJpegFrame(HVAPI_HANDLE_CONTEXT_EX* pHHC, PBYTE pbFrameData, DWORD dwFrameDataLen, LPCSTR szVideoExtInfo)
{
	if(pHHC == NULL || pbFrameData == NULL || dwFrameDataLen <= 0)
	{
		return E_FAIL;
	}
	PBYTE pbTemp = pbFrameData;
	int iSize = 0;
	char szBuf[10] = {0};
	iSize = *(int*)pbTemp;
	pbTemp += 4;
	memcpy(szBuf, pbTemp, 8);
	if(strcmp(szBuf, "redlight") != 0)
	{
		HVAPI_CALLBACK_SET* pTemp = NULL;
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnHistoryVideo)
			{
				pTemp->pOnHistoryVideo(pTemp->pOnHistoryVideoParam, pbTemp, iSize, VIDEO_TYPE_JPEG_HISTORY, szVideoExtInfo);
			}
		}
		return S_OK;
	}
	pbTemp += 8;
	int iRedLightCount = *(int*)pbTemp;
	int iRedLightPosBufLen = iRedLightCount * sizeof(RECT);
	PBYTE pbRedLightPos = new BYTE[iRedLightPosBufLen];
	pbTemp += 4;
	memcpy(pbRedLightPos, pbTemp, iRedLightPosBufLen);
	pbTemp += iRedLightPosBufLen;
	if(pHHC->dwVideoEnhanceRedLightFlag == 0)
	{
		HVAPI_CALLBACK_SET* pTemp = NULL;
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnHistoryVideo)
			{
				pTemp->pOnHistoryVideo(pTemp->pOnHistoryVideoParam, pbTemp, iSize, VIDEO_TYPE_JPEG_HISTORY, szVideoExtInfo);
			}
		}
		delete[] pbRedLightPos;
		return S_OK;
	}

	DWORD dwDestImgBufLen = 1024;
	dwDestImgBufLen = (dwDestImgBufLen << 10);
	PBYTE pbDestImgBuf = new BYTE[dwDestImgBufLen];
	if(pbDestImgBuf == NULL)
	{
		HVAPI_CALLBACK_SET* pTemp = NULL;
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnHistoryVideo)
			{
				pTemp->pOnHistoryVideo(pTemp->pOnHistoryVideoParam, pbTemp, iSize, VIDEO_TYPE_JPEG_HISTORY, szVideoExtInfo);
			}
		}
		delete[] pbRedLightPos;
		return S_OK;
	}

	if(HvEnhanceTrafficLight(pbTemp, iSize, iRedLightCount, pbRedLightPos, pbDestImgBuf, dwDestImgBufLen,
		pHHC->iBrightness, pHHC->iHueThrshold, pHHC->iCompressRate) != S_OK)
	{
		HVAPI_CALLBACK_SET* pTemp = NULL;
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnHistoryVideo)
			{
				pTemp->pOnHistoryVideo(pTemp->pOnHistoryVideoParam, pbTemp, iSize, VIDEO_TYPE_JPEG_HISTORY, szVideoExtInfo);
			}
		}
		delete[] pbRedLightPos;
		delete[] pbDestImgBuf;
		return S_OK;
	}

	HVAPI_CALLBACK_SET* pTemp = NULL;
	for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
	{
		if(pTemp->pOnHistoryVideo)
		{
			pTemp->pOnHistoryVideo(pTemp->pOnHistoryVideoParam, pbDestImgBuf, dwDestImgBufLen, VIDEO_TYPE_JPEG_HISTORY, szVideoExtInfo);
		}
	}
	delete[] pbDestImgBuf;
	delete[] pbRedLightPos;
	return S_OK;
}

static DWORD WINAPI VideoRecvThreadFuncEx(LPVOID lpParam)
{
	if(lpParam == NULL)
	{
		return -1;
	}
	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)lpParam;
	INFO_HEADER cInfoHeader;
	INFO_HEADER cInfoHeaderResponse;
	BLOCK_HEADER cBlockHeader;
	unsigned char* pbInfo = NULL;
	unsigned char* pbData = NULL;
	cInfoHeaderResponse.dwType = CAMERA_THROB_RESPONSE;
	cInfoHeaderResponse.dwInfoLen = 0;
	cInfoHeaderResponse.dwDataLen = 0;
	pHHC->dwVideoStreamTick = GetTickCount();
	while(!pHHC->fIsThreadRecvVideoExit)
	{
		if(pHHC->dwVideoConnStatus == CONN_STATUS_NORMAL)
		{
			if(GetTickCount() - pHHC->dwVideoStreamTick > 8000)
			{
				WrightLogEx(pHHC->szIP, "[VideoLink] Reconnect,Last Result Proces TimeOut...");
				pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
			}
		}
		if(pHHC->dwVideoConnStatus == CONN_STATUS_RECONN
			|| pHHC->dwVideoConnStatus == CONN_STATUS_DISCONN)
		{
			Sleep(1000);
			continue;
		}

		if(RecvAll(pHHC->sktVideo, (char*)&cInfoHeader, sizeof(cInfoHeader)) == sizeof(cInfoHeader))
		{
			SAFE_DELETE(pbInfo);
			SAFE_DELETE(pbData);

			pHHC->dwVideoStreamTick = GetTickCount();
			if(cInfoHeader.dwType == CAMERA_THROB)
			{
				if(send(pHHC->sktVideo, (char*)&cInfoHeaderResponse, sizeof(cInfoHeaderResponse), 0) 
					!= sizeof(cInfoHeaderResponse))
				{
					WrightLogEx(pHHC->szIP, "[VideoLink] Reconnect,Throb Response Send Failed...");
					pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
				}
				continue;
			}

			if(cInfoHeader.dwType == CAMERA_HISTORY_END)
			{
				WrightLogEx(pHHC->szIP, "[VideoLink] Quit,Recive Done...");
				pHHC->dwVideoConnStatus = CONN_STATUS_RECVDONE;
				pHHC->fIsConnectHistoryVideo = FALSE;
				ZeroMemory(pHHC->szVideoConnCmd, sizeof(pHHC->szVideoConnCmd));
				ForceCloseSocket(pHHC->sktVideo);
				break;
			}

			DWORD32 dwBuffLen = 10 * 1024 * 1024;
			if (cInfoHeader.dwInfoLen > dwBuffLen || cInfoHeader.dwDataLen > dwBuffLen)
			{
				WrightLogEx(pHHC->szIP, "[VideoLink] Quit,Recive Data Error...");
				pHHC->dwVideoConnStatus = CONN_STATUS_RECVDONE;
				ZeroMemory(pHHC->szVideoConnCmd, sizeof(pHHC->szVideoConnCmd));
				break;
			}

			pbInfo = new unsigned char[cInfoHeader.dwInfoLen + 1];
			pbData = new unsigned char[cInfoHeader.dwDataLen + 1];
			int ipbInfoSize = cInfoHeader.dwInfoLen + 1;

			if(pbInfo == NULL || pbData == NULL)
			{
				WrightLogEx(pHHC->szIP, "[VideoLink] Reconnect,Malloc Buffer Failed...");
				Sleep(100);
				pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
				continue;
			}
			else
			{
				ZeroMemory(pbInfo, cInfoHeader.dwInfoLen + 1);
				ZeroMemory(pbData, cInfoHeader.dwDataLen + 1);
			}

			if(cInfoHeader.dwInfoLen > 0)
			{
				if(RecvAll(pHHC->sktVideo, (char*)pbInfo, cInfoHeader.dwInfoLen)
					!= cInfoHeader.dwInfoLen)
				{
					WrightLogEx(pHHC->szIP, "[VideoLink] Reconnect,Recv Packet Header Failed...");
					Sleep(100);
					pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
					continue;
				}
			}

			if(cInfoHeader.dwDataLen > 0)
			{
				if(RecvAll(pHHC->sktVideo, (char*)pbData, cInfoHeader.dwDataLen) 
					!= cInfoHeader.dwDataLen)
				{
					WrightLogEx(pHHC->szIP, "[VideoLink] Reconnect,Recv Packet Data Failed...");
					Sleep(100);
					pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
					continue;
				}
				if(cInfoHeader.dwType == CAMERA_VIDEO)
				{
					// 数据流附加信息解析
					DWORD32 dwVideoType = 0;
					DWORD32 dwFrameType = 0;
					DWORD64 dw64VideoTime = 0;
					VideoWidth cVideoWidth = {0};
					VideoHeight cVideoHeight = {0};
					VideoExtInfo cVideoExtInfo = {0};

					unsigned char* pbTemp = pbInfo;
					int iFlag = 0;
					for( int i = 0; i < (int)cInfoHeader.dwInfoLen; )
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

					if ( dwVideoType == CAMERA_VIDEO_H264 )
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
							char* pszVideoExtInfo = new char[128];
							if(pszVideoExtInfo)
							{
								sprintf(pszVideoExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d,FrameTime:%I64u,Width:%d,Height:%d",
									cVideoExtInfo.iShutter, cVideoExtInfo.iGain, cVideoExtInfo.iGainR,
									cVideoExtInfo.iGainG, cVideoExtInfo.iGainB, dw64VideoTime, cVideoWidth.iWidth, cVideoHeight.iHight);
							}

							PBYTE pPacketData = pbData;
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
								HVAPI_CALLBACK_SET* pTemp = NULL;
								for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
								{
									if(pTemp->pOnHistoryVideo)
									{
										pTemp->pOnHistoryVideo(pTemp->pOnHistoryVideoParam, pPacketData, iFrameLen, dwType, pszVideoExtInfo);
									}
								}
								pPacketData += iFrameLen;
								iTempBufferLem = iTempBufferLem - 8 - iFrameLen;
							}
							SAFE_DELETE(pszVideoExtInfo);
							CTime cTime(dw64VideoTime/1000);
							CString cStr = cTime.Format("%Y.%m.%d_%H:%M:%S");
							memcpy(pHHC->szVideoBeginTimeStr, cStr.GetBuffer(), 19);
							pHHC->szVideoBeginTimeStr[19] = '\0';
							continue;
						}

						char* pszVideoExtInfo = new char[128];
						if(pszVideoExtInfo)
						{
							sprintf(pszVideoExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d,FrameTime:%I64u,Width:%d,Height:%d",
								cVideoExtInfo.iShutter, cVideoExtInfo.iGain, cVideoExtInfo.iGainR,
								cVideoExtInfo.iGainG, cVideoExtInfo.iGainB, dw64VideoTime, cVideoWidth.iWidth, cVideoHeight.iHight);
						}
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->pOnH264)
							{
								pTemp->pOnH264(pTemp->pOnH264Param, pbData, cInfoHeader.dwDataLen, dwType, pszVideoExtInfo);
							}
						}
						SAFE_DELETE(pszVideoExtInfo);
					}
					else if(dwVideoType == CAMERA_VIDEO_JPEG)
					{
						char* pszVideoExtInfo = new char[128];
						if(pszVideoExtInfo)
						{
							sprintf(pszVideoExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d,FrameTime:%I64u,Width:%d,Height:%d",
								cVideoExtInfo.iShutter, cVideoExtInfo.iGain, cVideoExtInfo.iGainR,
								cVideoExtInfo.iGainG, cVideoExtInfo.iGainB, dw64VideoTime, cVideoWidth.iWidth, cVideoHeight.iHight);
						}

						PBYTE pPacketData = pbData;
						int iTempBufferLem = cInfoHeader.dwDataLen;
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
							ProcHistoryVideoJpegFrame(pHHC, pbData+iReadedCount, iFrameLen, pszVideoExtInfo);
							iReadedCount += iFrameLen;
							iTempBufferLem -= iFrameLen;
						}
						SAFE_DELETE(pszVideoExtInfo);
					}
				}
			}
		}
		else
		{
			WrightLogEx(pHHC->szIP, "[VideoLink] Reconnect,Recv Throb Packet TimeOut...");
			pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
			Sleep(100);
		}
	}
	SAFE_DELETE(pbInfo);
	SAFE_DELETE(pbData);
	return 0;
}

HV_API_EX HRESULT CDECL HVAPI_SetCallBackEx(HVAPI_HANDLE_EX hHandle, PVOID pFunc, PVOID pUserData,
											INT iVideoID, INT iCallBackType, LPCSTR szConnCmd)
{
	if(hHandle == NULL) 
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(pHHC->szVersion, HVAPI_API_VERSION_EX) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	switch(iCallBackType)
	{
	case CALLBACK_TYPE_RECORD_PLATE:
		{
			if (pFunc)
			{
				if(pHHC->fAutoLink)
				{
					if (INVALID_SOCKET != pHHC->sktRecord)
					{
						ForceCloseSocket(pHHC->sktRecord);
					}

					if ( szConnCmd != NULL && sizeof(pHHC->szRecordConnCmd) > strlen(szConnCmd) )
					{
						strcpy((char*)pHHC->szRecordConnCmd, szConnCmd);
					}
					else
					{
						strcpy((char*)pHHC->szRecordConnCmd, DEFAULT_CONNCMD);
					}
					SaveRecordConnCmd(pHHC);

					HVAPI_CALLBACK_SET* pTemp = NULL;
					for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
					{
						if(pTemp->iVideoID == iVideoID)
						{
							pTemp->pOnPlate = (HVAPI_CALLBACK_RECORD_PLATE)pFunc;
							pTemp->pOnPlateParam = (PVOID)pUserData;
							break;
						}
					}
					if(pTemp == NULL)
					{
						pTemp = new HVAPI_CALLBACK_SET();
						memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
						pTemp->iVideoID = iVideoID;
						pTemp->pOnPlate = (HVAPI_CALLBACK_RECORD_PLATE)pFunc;
						pTemp->pOnPlateParam = (PVOID)pUserData;
						pTemp->pNext = pHHC->pCallBackSet;
						pHHC->pCallBackSet = pTemp;
					}
					pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
					pHHC->fVailPackResumeCache = FALSE;

					SetConnAutoLinkHHC(pHHC->szDevSN);
					return S_OK;
				}

				if (pHHC->dwRecordConnStatus == CONN_STATUS_UNKNOWN
					|| pHHC->dwRecordConnStatus == CONN_STATUS_DISCONN)
				{
					if (ConnectCamera(pHHC->szIP, CAMERA_RECORD_LINK_PORT, pHHC->sktRecord))
					{
						if ( szConnCmd != NULL && sizeof(pHHC->szRecordConnCmd) > strlen(szConnCmd))
						{
							strcpy((char*)pHHC->szRecordConnCmd, szConnCmd);
							char szRetBuf[256] = {0};
							if(HvSendXmlCmd(pHHC->szIP, (char*)pHHC->szRecordConnCmd, szRetBuf, sizeof(szRetBuf),
								NULL, pHHC->sktRecord) != S_OK)
							{
								ForceCloseSocket(pHHC->sktImage);
								return E_FAIL;
							}
							SaveRecordConnCmd(pHHC);
							
							if(strstr((char*)pHHC->szRecordConnCmd, "DownloadRecord")
								&& strstr((char*)pHHC->szRecordConnCmd, "Enable[1]"))
							{
								pHHC->fIsRecvHistoryRecord = TRUE;
								char* pTempStr = strstr((char*)pHHC->szRecordConnCmd, "BeginTime");
								if(pTempStr)
								{
									memcpy(pHHC->szRecordBeginTimeStr, pTempStr+10, 13);
									pHHC->szRecordBeginTimeStr[13] = '\0';
								}

								pTempStr = strstr((char*)pHHC->szRecordConnCmd, "Index");
								if(pTempStr)
								{
									sscanf(pTempStr, "Index[%d]", &pHHC->dwRecordStartIndex);
								}

								pTempStr = strstr((char*)pHHC->szRecordConnCmd, "EndTime");
								if(pTempStr)
								{
									if(pTempStr[9] == ']')
									{
										memset(pHHC->szRecordEndTimeStr, 0, 14);
										pHHC->szRecordEndTimeStr[0] = '0';
									}
									else
									{
										memcpy(pHHC->szRecordEndTimeStr, pTempStr+10, 13);
										pHHC->szRecordEndTimeStr[13] = '\0';
									}
								}

								pTempStr = strstr((char*)pHHC->szRecordConnCmd, "DataInfo");
								if(pTempStr)
								{
									sscanf(pTempStr, "DataInfo[%d]", &pHHC->iRecordDataInfo);
								}
							}
						}
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnPlate = (HVAPI_CALLBACK_RECORD_PLATE)pFunc;
								pTemp->pOnPlateParam = (PVOID)pUserData;
								break;
							}
						}
						if(pTemp == NULL)
						{
							pTemp = new HVAPI_CALLBACK_SET();
							memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
							pTemp->iVideoID = iVideoID;
							pTemp->pOnPlate = (HVAPI_CALLBACK_RECORD_PLATE)pFunc;
							pTemp->pOnPlateParam = (PVOID)pUserData;
							pTemp->pNext = pHHC->pCallBackSet;
							pHHC->pCallBackSet = pTemp;
						}

						pHHC->fIsThreadRecvRecordExit = FALSE;
						pHHC->hThreadRecvRecord = CreateThread(NULL, 0, RecordRecvThreadFuncEx, pHHC, 0, NULL);
						pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
					}
					else
					{
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnPlate = NULL;
								pTemp->pOnPlateParam = NULL;
								break;
							}
						}
						return E_FAIL;
					}
				}
				else
				{
					HVAPI_CALLBACK_SET* pTemp = NULL;
					for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
					{
						if(pTemp->iVideoID == iVideoID)
						{
							pTemp->pOnPlate = (HVAPI_CALLBACK_RECORD_PLATE)pFunc;
							pTemp->pOnPlateParam = (PVOID)pUserData;
							break;
						}
					}
					if(pTemp == NULL)
					{
						pTemp = new HVAPI_CALLBACK_SET();
						memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
						pTemp->iVideoID = iVideoID;
						pTemp->pOnPlate = (HVAPI_CALLBACK_RECORD_PLATE)pFunc;
						pTemp->pOnPlateParam = (PVOID)pUserData;
						pTemp->pNext = pHHC->pCallBackSet;
						pHHC->pCallBackSet = pTemp;
					}
					return S_OK;
				}
			}
			else
			{
				pHHC->dwRecordConnStatus = CONN_STATUS_DISCONN;
				pHHC->fIsThreadRecvRecordExit = TRUE;
				HvSafeCloseThread(pHHC->hThreadRecvRecord);
				ForceCloseSocket(pHHC->sktRecord);
				pHHC->hThreadRecvRecord = NULL;

				HVAPI_CALLBACK_SET* pTemp = NULL;
				for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
				{
					if(pTemp->iVideoID == iVideoID)
					{
						pTemp->pOnPlate = NULL;
						pTemp->pOnPlateParam = NULL;
						break;
					}
				}
				pHHC->fVailPackResumeCache = FALSE;
			}
		}
		break;
	case CALLBACK_TYPE_RECORD_BIGIMAGE:
		{
			HVAPI_CALLBACK_SET* pTemp = NULL;
			for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
			{
				if(pTemp->iVideoID == iVideoID)
				{
					if(pFunc)
					{
						pTemp->pOnBigImage = (HVAPI_CALLBACK_RECORD_BIGIMAGE)pFunc;
						pTemp->pOnBigImageParam = (PVOID)pUserData;
						break;
					}
					else
					{
						pTemp->pOnBigImage = NULL;
						pTemp->pOnBigImageParam = NULL;
						return S_OK;
					}
				}
			}
			if(pTemp == NULL)
			{
				pTemp = new HVAPI_CALLBACK_SET();
				memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
				pTemp->iVideoID = iVideoID;
				pTemp->pOnBigImage = (HVAPI_CALLBACK_RECORD_BIGIMAGE)pFunc;
				pTemp->pOnBigImageParam = (PVOID)pUserData;
				pTemp->pNext = pHHC->pCallBackSet;
				pHHC->pCallBackSet = pTemp;
			}
		}
		break;
	case CALLBACK_TYPE_RECORD_SMALLIMAGE:
		{
			HVAPI_CALLBACK_SET* pTemp = NULL;
			for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
			{
				if(pTemp->iVideoID == iVideoID)
				{
					if(pFunc)
					{
						pTemp->pOnSmallImage = (HVAPI_CALLBACK_RECORD_SMALLIMAGE)pFunc;
						pTemp->pOnSmallImageParam = (PVOID)pUserData;
						break;
					}
					else
					{
						pTemp->pOnSmallImage = NULL;
						pTemp->pOnSmallImageParam = NULL;
						return S_OK;
					}
				}
			}
			if(pTemp == NULL)
			{
				pTemp = new HVAPI_CALLBACK_SET();
				memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
				pTemp->iVideoID = iVideoID;
				pTemp->pOnSmallImage = (HVAPI_CALLBACK_RECORD_SMALLIMAGE)pFunc;
				pTemp->pOnSmallImageParam = (PVOID)pUserData;
				pTemp->pNext = pHHC->pCallBackSet;
				pHHC->pCallBackSet = pTemp;
			}
		}
		break;
	case CALLBACK_TYPE_RECORD_BINARYIMAGE:
		{
			HVAPI_CALLBACK_SET* pTemp = NULL;
			for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
			{
				if(pTemp->iVideoID == iVideoID)
				{
					if(pFunc)
					{
						pTemp->pOnBinaryImage = (HVAPI_CALLBACK_RECORD_BINARYIMAGE)pFunc;
						pTemp->pOnBinaryImageParam = (PVOID)pUserData;
						break;
					}
					else
					{
						pTemp->pOnBinaryImage = NULL;
						pTemp->pOnBinaryImageParam = NULL;
						return S_OK;
					}
				}
			}
			if(pTemp == NULL)
			{
				pTemp = new HVAPI_CALLBACK_SET();
				memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
				pTemp->iVideoID = iVideoID;
				pTemp->pOnBinaryImage = (HVAPI_CALLBACK_RECORD_BINARYIMAGE)pFunc;
				pTemp->pOnBinaryImageParam = (PVOID)pUserData;
				pTemp->pNext = pHHC->pCallBackSet;
				pHHC->pCallBackSet = pTemp;
			}
		}
		break;
	case CALLBACK_TYPE_RECORD_INFOBEGIN:
		{
			HVAPI_CALLBACK_SET* pTemp = NULL;
			for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
			{
				if(pTemp->iVideoID == iVideoID)
				{
					if(pFunc)
					{
						pTemp->pOnRecordBegin = (HVAPI_CALLBACK_RECORD_INFOBEGIN)pFunc;
						pTemp->pOnRecordBeginParam = (PVOID)pUserData;
						break;
					}
					else
					{
						pTemp->pOnRecordBegin = NULL;
						pTemp->pOnRecordBeginParam = NULL;
						return S_OK;
					}
				}
			}
			if(pTemp == NULL)
			{
				pTemp = new HVAPI_CALLBACK_SET();
				memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
				pTemp->iVideoID = iVideoID;
				pTemp->pOnRecordBegin = (HVAPI_CALLBACK_RECORD_INFOBEGIN)pFunc;
				pTemp->pOnRecordBeginParam = (PVOID)pUserData;
				pTemp->pNext = pHHC->pCallBackSet;
				pHHC->pCallBackSet = pTemp;
			}
		}
		break;
	case CALLBACK_TYPE_RECORD_INFOEND:
		{
			HVAPI_CALLBACK_SET* pTemp = NULL;
			for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
			{
				if(pTemp->iVideoID == iVideoID)
				{
					if(pFunc)
					{
						pTemp->pOnRecordEnd = (HVAPI_CALLBACK_RECORD_INFOEND)pFunc;
						pTemp->pOnRecordEndParam = (PVOID)pUserData;
						break;
					}
					else
					{
						pTemp->pOnRecordEnd = NULL;
						pTemp->pOnRecordEndParam = NULL;
						return S_OK;
					}
				}
			}
			if(pTemp == NULL)
			{
				pTemp = new HVAPI_CALLBACK_SET();
				memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
				pTemp->iVideoID = iVideoID;
				pTemp->pOnRecordEnd = (HVAPI_CALLBACK_RECORD_INFOEND)pFunc;
				pTemp->pOnRecordEndParam = (PVOID)pUserData;
				pTemp->pNext = pHHC->pCallBackSet;
				pHHC->pCallBackSet = pTemp;
			}
		}
		break;
	case CALLBACK_TYPE_STRING:
		{
			HVAPI_CALLBACK_SET* pTemp = NULL;
			for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
			{
				if(pTemp->iVideoID == iVideoID)
				{
					if(pFunc)
					{
						pTemp->pOnString = (HVAPI_CALLBACK_STRING)pFunc;
						pTemp->pOnStringParam = (PVOID)pUserData;
						break;
					}
					else
					{
						pTemp->pOnString = NULL;
						pTemp->pOnStringParam = NULL;
						return S_OK;
					}
				}
			}
			if(pTemp == NULL)
			{
				pTemp = new HVAPI_CALLBACK_SET();
				memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
				pTemp->iVideoID = iVideoID;
				pTemp->pOnString = (HVAPI_CALLBACK_STRING)pFunc;
				pTemp->pOnStringParam = (PVOID)pUserData;
				pTemp->pNext = pHHC->pCallBackSet;
				pHHC->pCallBackSet = pTemp;
			}
		}
		break;
	case CALLBACK_TYPE_JPEG_FRAME:
		{
			if(pFunc)
			{
				if (pHHC->fAutoLink)
				{
					return E_NOTIMPL;
				}
				if(pHHC->dwImageConnStatus == CONN_STATUS_UNKNOWN
					|| pHHC->dwImageConnStatus == CONN_STATUS_DISCONN)
				{
					if(ConnectCamera(pHHC->szIP, CAMERA_IMAGE_LINK_PORT, pHHC->sktImage))
					{
						if(szConnCmd != NULL)
						{
							strcpy((char*)pHHC->szImageConnCmd, szConnCmd);
							char szRetBuf[256] = {0};
							if(HvSendXmlCmd(pHHC->szIP, (char*)pHHC->szImageConnCmd, szRetBuf, sizeof(szRetBuf),
								NULL, pHHC->sktImage) != S_OK)
							{
								ForceCloseSocket(pHHC->sktImage);
								return E_FAIL;
							}
						}
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnJpegFrame = (HVAPI_CALLBACK_JPEG)pFunc;
								pTemp->pOnJpegFrameParam = (PVOID)pUserData;
								break;
							}
						}
						if(pTemp == NULL)
						{
							pTemp = new HVAPI_CALLBACK_SET();
							memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
							pTemp->iVideoID = iVideoID;
							pTemp->pOnJpegFrame = (HVAPI_CALLBACK_JPEG)pFunc;
							pTemp->pOnJpegFrameParam = (PVOID)pUserData;
							pTemp->pNext = pHHC->pCallBackSet;
							pHHC->pCallBackSet = pTemp;
						}
						pHHC->fIsThreadRecvImageExit = FALSE;
						pHHC->hThreadRecvImage = CreateThread(NULL, 0, ImageRecvThreadFuncEx, pHHC, 0, NULL);
						pHHC->dwImageConnStatus = CONN_STATUS_NORMAL;
					}
					else
					{
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnJpegFrame = NULL;
								pTemp->pOnJpegFrameParam = NULL;
								break;
							}
						}
						return E_FAIL;
					}
				}
				else
				{
					HVAPI_CALLBACK_SET* pTemp = NULL;
					for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
					{
						if(pTemp->iVideoID == iVideoID)
						{
							pTemp->pOnJpegFrame = (HVAPI_CALLBACK_JPEG)pFunc;
							pTemp->pOnJpegFrameParam = (PVOID)pUserData;
							break;
						}
					}
					if(pTemp == NULL)
					{
						pTemp = new HVAPI_CALLBACK_SET();
						memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
						pTemp->iVideoID = iVideoID;
						pTemp->pOnJpegFrame = (HVAPI_CALLBACK_JPEG)pFunc;
						pTemp->pOnJpegFrameParam = (PVOID)pUserData;
						pTemp->pNext = pHHC->pCallBackSet;
						pHHC->pCallBackSet = pTemp;
					}
					return S_OK;
				}
			}
			else
			{
				pHHC->dwImageConnStatus = CONN_STATUS_DISCONN;
				pHHC->fIsThreadRecvImageExit = TRUE;
				HvSafeCloseThread(pHHC->hThreadRecvImage);
				ForceCloseSocket(pHHC->sktImage);

				HVAPI_CALLBACK_SET* pTemp = NULL;
				for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
				{
					if(pTemp->iVideoID == iVideoID)
					{
						pTemp->pOnJpegFrame = NULL;
						pTemp->pOnJpegFrameParam = NULL;
						break;
					}
				}
			}
		}
		break;
	case CALLBACK_TYPE_H264_VIDEO:
		{
			if(pHHC->fIsConnectHistoryVideo)
			{
				return E_FAIL;
			}
			if(pFunc)
			{
				if (pHHC->fAutoLink)
				{
					return E_NOTIMPL;
				}
				if(pHHC->dwVideoConnStatus == CONN_STATUS_UNKNOWN
					|| pHHC->dwVideoConnStatus == CONN_STATUS_DISCONN
					|| pHHC->dwVideoConnStatus == CONN_STATUS_RECVDONE)
				{
					if(szConnCmd != NULL)
					{
						if(strstr(szConnCmd, "DownloadVideo") && 
							strstr(szConnCmd, "Enable[1]"))
						{
							return E_FAIL;
						}
					}
					if(ConnectCamera(pHHC->szIP, CAMERA_VIDEO_LINK_PORT, pHHC->sktVideo))
					{
						if(szConnCmd != NULL)
						{
							strcpy((char*)pHHC->szVideoConnCmd, szConnCmd);
							char szRetBuf[256] = {0};
							if(HvSendXmlCmd(pHHC->szIP, (char*)pHHC->szVideoConnCmd, szRetBuf, sizeof(szRetBuf),
								NULL, pHHC->sktVideo) != S_OK)
							{
								ForceCloseSocket(pHHC->sktVideo);
								return E_FAIL;
							}
						}
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp !=NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnH264 = (HVAPI_CALLBACK_H264)pFunc;
								pTemp->pOnH264Param = (PVOID)pUserData;
								break;
							}
						}
						if(pTemp == NULL)
						{
							pTemp = new HVAPI_CALLBACK_SET();
							memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
							pTemp->iVideoID = iVideoID;
							pTemp->pOnH264 = (HVAPI_CALLBACK_H264)pFunc;
							pTemp->pOnH264Param = (PVOID)pUserData;
							pTemp->pNext = pHHC->pCallBackSet;
							pHHC->pCallBackSet = pTemp;
						}
						pHHC->fIsThreadRecvVideoExit = FALSE;
						pHHC->hThreadRecvVideo = CreateThread(NULL, 0, VideoRecvThreadFuncEx, pHHC, 0, NULL);
						pHHC->dwVideoConnStatus = CONN_STATUS_NORMAL;
					}
					else
					{
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp!= NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnH264 = NULL;
								pTemp->pOnH264Param = NULL;
								break;
							}
						}
						return E_FAIL;
					}
				}
				else
				{
					HVAPI_CALLBACK_SET* pTemp = NULL;
					for(pTemp = pHHC->pCallBackSet; pTemp !=NULL; pTemp = pTemp->pNext)
					{
						if(pTemp->iVideoID == iVideoID)
						{
							pTemp->pOnH264 = (HVAPI_CALLBACK_H264)pFunc;
							pTemp->pOnH264Param = (PVOID)pUserData;
							break;
						}
					}
					if(pTemp == NULL)
					{
						pTemp = new HVAPI_CALLBACK_SET();
						memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
						pTemp->iVideoID = iVideoID;
						pTemp->pOnH264 = (HVAPI_CALLBACK_H264)pFunc;
						pTemp->pOnH264Param = (PVOID)pUserData;
						pTemp->pNext = pHHC->pCallBackSet;
						pHHC->pCallBackSet = pTemp;
					}
				}
			}
			else
			{
				pHHC->dwVideoConnStatus = CONN_STATUS_DISCONN;
				pHHC->fIsThreadRecvVideoExit = TRUE;
				HvSafeCloseThread(pHHC->hThreadRecvVideo);
				ForceCloseSocket(pHHC->sktVideo);

				HVAPI_CALLBACK_SET* pTemp = NULL;
				for(pTemp = pHHC->pCallBackSet; pTemp!= NULL; pTemp = pTemp->pNext)
				{
					if(pTemp->iVideoID == iVideoID)
					{
						pTemp->pOnH264 = NULL;
						pTemp->pOnH264Param = NULL;
						break;
					}
				}
			}
		}
		break;
	case CALLBACK_TYPE_HISTORY_VIDEO:
		{
			if(pFunc)
			{
				if (pHHC->fAutoLink)
				{
					return E_NOTIMPL;
				}
				if(szConnCmd == NULL)
				{
					return E_FAIL;
				}
				if(!strstr(szConnCmd, "DownloadVideo") || 
					!strstr(szConnCmd, "Enable[1]"))
				{
					return E_FAIL;
				}
				if(pHHC->dwVideoConnStatus == CONN_STATUS_UNKNOWN
					|| pHHC->dwVideoConnStatus == CONN_STATUS_DISCONN
					|| pHHC->dwVideoConnStatus == CONN_STATUS_RECVDONE)
				{
					if(ConnectCamera(pHHC->szIP, CAMERA_VIDEO_LINK_PORT, pHHC->sktVideo))
					{
						strcpy((char*)pHHC->szVideoConnCmd, szConnCmd);
						char szRetBuf[256] = {0};
						if(HvSendXmlCmd(pHHC->szIP, (char*)pHHC->szVideoConnCmd, szRetBuf, sizeof(szRetBuf),
							NULL, pHHC->sktVideo) != S_OK)
						{
							ForceCloseSocket(pHHC->sktVideo);
							pHHC->fIsConnectHistoryVideo = FALSE;
							return E_FAIL;
						}
						char* pTempStr = strstr((char*)pHHC->szVideoConnCmd, "BeginTime");
						if(pTempStr)
						{
							memcpy(pHHC->szVideoBeginTimeStr, pTempStr+10, 19);
							pHHC->szVideoBeginTimeStr[19] = '\0';
						}

						pTempStr = strstr((char*)pHHC->szVideoConnCmd, "EndTime");
						if(pTempStr)
						{
							if(pTempStr[9] == ']')
							{
								memset(pHHC->szRecordEndTimeStr, 0, 14);
								pHHC->szRecordEndTimeStr[0] = '0';
							}
							else
							{
								memcpy(pHHC->szVideoEndTimeStr, pTempStr+8, 19);
								pHHC->szVideoEndTimeStr[19] = '\0';
							}
						}

						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnHistoryVideo = (HVAPI_CALLBACK_HISTORY_VIDEO)pFunc;
								pTemp->pOnHistoryVideoParam = (PVOID)pUserData;
								break;
							}
						}
						if(pTemp == NULL)
						{
							pTemp = new HVAPI_CALLBACK_SET();
							memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
							pTemp->iVideoID = iVideoID;
							pTemp->pOnHistoryVideo = (HVAPI_CALLBACK_HISTORY_VIDEO)pFunc;
							pTemp->pOnHistoryVideoParam = (PVOID)pUserData;
							pTemp->pNext = pHHC->pCallBackSet;
							pHHC->pCallBackSet = pTemp;
						}
						pHHC->fIsConnectHistoryVideo = TRUE;
						pHHC->fIsThreadRecvVideoExit = FALSE;
						pHHC->hThreadRecvVideo = CreateThread(NULL, 0, VideoRecvThreadFuncEx, pHHC, 0, NULL);
						pHHC->dwVideoConnStatus = CONN_STATUS_NORMAL;
					}
					else
					{
						pHHC->fIsConnectHistoryVideo = FALSE;
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnHistoryVideo = NULL;
								pTemp->pOnHistoryVideoParam = NULL;
								break;
							}
						}
						return E_FAIL;
					}
				}
				else
				{
					if(pHHC->fIsConnectHistoryVideo)
					{
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnHistoryVideo = (HVAPI_CALLBACK_HISTORY_VIDEO)pFunc;
								pTemp->pOnHistoryVideoParam = (PVOID)pUserData;
								break;
							}
						}
						if(pTemp == NULL)
						{
							pTemp = new HVAPI_CALLBACK_SET();
							memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
							pTemp->iVideoID = iVideoID;
							pTemp->pOnHistoryVideo = (HVAPI_CALLBACK_HISTORY_VIDEO)pFunc;
							pTemp->pOnHistoryVideoParam = (PVOID)pUserData;
							pTemp->pNext = pHHC->pCallBackSet;
							pHHC->pCallBackSet = pTemp;
						}
					}
					else
					{
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnHistoryVideo = NULL;
								pTemp->pOnHistoryVideoParam = NULL;
								break;
							}
						}
						return E_FAIL;
					}
				}
			}
			else
			{
				pHHC->dwVideoConnStatus = CONN_STATUS_DISCONN;
				if(pHHC->sktVideo != INVALID_SOCKET)
				{
					ForceCloseSocket(pHHC->sktVideo);
				}
				pHHC->fIsThreadRecvVideoExit = TRUE;
				HvSafeCloseThread(pHHC->hThreadRecvVideo);

				HVAPI_CALLBACK_SET* pTemp = NULL;
				for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
				{
					if(pTemp->iVideoID == iVideoID)
					{
						pTemp->pOnHistoryVideo = NULL;
						pTemp->pOnHistoryVideoParam = NULL;
						break;
					}
				}
			}
		}
		break;
	}

	return S_OK;
}

HV_API_EX HRESULT CDECL HVAPI_SetEnhanceRedLightFlagEx(HVAPI_HANDLE_EX hHandle, DWORD dwEnhanceStyle, INT iBrightness, INT iHubThreshold, INT iCompressRate)
{
	if(hHandle == NULL)
	{
		return E_FAIL;
	}
	if(dwEnhanceStyle<0 || dwEnhanceStyle>3) 
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	pHHC->dwEnhanceRedLightFlag = dwEnhanceStyle;
	pHHC->iBigPicBrightness = iBrightness;
	pHHC->iBigPicHueThrshold = iHubThreshold;
	pHHC->iBigPicCompressRate = iCompressRate;
	return S_OK;
}

HV_API_EX HRESULT CDECL HVAPI_SetHistoryVideoEnhanceRedLightFlagEx(HVAPI_HANDLE_EX hHandle, DWORD dwEnhanceStyle, INT iBrightness, INT iHubThreshold, INT iCompressRate)
{
	if(hHandle == NULL)
	{
		return E_FAIL;
	}
	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	pHHC->dwVideoEnhanceRedLightFlag = dwEnhanceStyle;
	if(pHHC->dwVideoEnhanceRedLightFlag == 0) return S_OK;
	pHHC->iBrightness = iBrightness;
	if(pHHC->iBrightness < -255)
	{
		pHHC->iBrightness = -255;
	}
	if(pHHC->iBrightness > 255)
	{
		pHHC->iBrightness = 255;
	}
	pHHC->iHueThrshold = iHubThreshold;
	pHHC->iCompressRate = iCompressRate;
	return S_OK;
}

typedef struct _tag_Face_Info
{
	int nFaceCount;
	RECT rcFacePos[20];
} 
FACE_INFO;

HV_API_EX HRESULT CDECL HVAPI_GetExtensionInfoEx(HVAPI_HANDLE_EX hHandle, DWORD dwType, LPVOID pRetData, INT* iBufLen)
{
	if (hHandle == NULL)
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	int* pValue;
	FACE_INFO* pcFaceInfo;
	HRESULT hr = E_FAIL;

	switch (dwType)
	{
	//车牌坐标
	case PLATE_RECT_BEST_SNAPSHOT:
		pValue = (int*)pRetData;
		if (*iBufLen != 16)
		{
			break;
		}
		for (int i = 0; i < 4; i++)
		{
			int iTmpValue;
			switch (i)
			{
			case 0:
				iTmpValue = pHHC->rcPlate[0].top;
				break;
			case 1:
				iTmpValue = pHHC->rcPlate[0].left;
				break;
			case 2:
				iTmpValue = pHHC->rcPlate[0].bottom;
				break;
			case 3:
				iTmpValue = pHHC->rcPlate[0].right;
				break;
			}
			*pValue = iTmpValue;
			pValue ++;
		}
		hr = S_OK;
		break;
		
	case PLATE_RECT_LAST_SNAPSHOT:
		pValue = (int*)pRetData;
		if (*iBufLen != 16)
		{
			break;
		}
		for (int i = 0; i < 4; i++)
		{
			int iTmpValue;
			switch (i)
			{
			case 0:
				iTmpValue = pHHC->rcPlate[1].top;
				break;
			case 1:
				iTmpValue = pHHC->rcPlate[1].left;
				break;
			case 2:
				iTmpValue = pHHC->rcPlate[1].bottom;
				break;
			case 3:
				iTmpValue = pHHC->rcPlate[1].right;
				break;
			}
			*pValue = iTmpValue;
			pValue ++;
		}
		hr = S_OK;
		break;

	case PLATE_RECT_BEGIN_CAPTURE:
		pValue = (int*)pRetData;
		if (*iBufLen != 16)
		{
			break;
		}
		for (int i = 0; i < 4; i++)
		{
			int iTmpValue;
			switch (i)
			{
			case 0:
				iTmpValue = pHHC->rcPlate[2].top;
				break;
			case 1:
				iTmpValue = pHHC->rcPlate[2].left;
				break;
			case 2:
				iTmpValue = pHHC->rcPlate[2].bottom;
				break;
			case 3:
				iTmpValue = pHHC->rcPlate[2].right;
				break;
			}
			*pValue = iTmpValue;
			pValue ++;
		}
		hr = S_OK;
		break;
	case PLATE_RECT_BEST_CAPTURE:
		 pValue = (int*)pRetData;
		 if (*iBufLen != 16)
		 {
			 break;
		 }
		for (int i = 0; i < 4; i++)
		{
			int iTmpValue;
			switch (i)
			{
			case 0:
				iTmpValue = pHHC->rcPlate[3].top;
				break;
			case 1:
				iTmpValue = pHHC->rcPlate[3].left;
				break;
			case 2:
				iTmpValue = pHHC->rcPlate[3].bottom;
				break;
			case 3:
				iTmpValue = pHHC->rcPlate[3].right;
				break;
			}
			*pValue = iTmpValue;
			pValue ++;
		}
		hr = S_OK;
		break;
	case PLATE_RECT_LAST_CAPTURE:
		pValue = (int*)pRetData;
		if (*iBufLen != 16)
		{
			break;
		}
		for (int i = 0; i < 4; i++)
		{
			int iTmpValue;
			switch (i)
			{
			case 0:
				iTmpValue = pHHC->rcPlate[4].top;
				break;
			case 1:
				iTmpValue = pHHC->rcPlate[4].left;
				break;
			case 2:
				iTmpValue = pHHC->rcPlate[4].bottom;
				break;
			case 3:
				iTmpValue = pHHC->rcPlate[4].right;
				break;
			}
			*pValue = iTmpValue;
			pValue ++;
		}
		hr = S_OK;
		break;

	//人脸信息
	case FACE_RECT_BEST_SNAPSHOT:
		if (*iBufLen != sizeof(FACE_INFO))
		{
			break;
		}
		pcFaceInfo = (FACE_INFO*)pRetData;
		pcFaceInfo->nFaceCount = pHHC->nFaceCount[0];
		memcpy(pcFaceInfo->rcFacePos, pHHC->rcFacePos[0], sizeof(pcFaceInfo->rcFacePos));
		hr = S_OK;
		break;
	case FACE_RECT_LAST_SNAPSHOT:
		if (*iBufLen != sizeof(FACE_INFO))
		{
			break;
		}
		pcFaceInfo = (FACE_INFO*)pRetData;
		pcFaceInfo->nFaceCount = pHHC->nFaceCount[1];
		memcpy(pcFaceInfo->rcFacePos, pHHC->rcFacePos[1], sizeof(pcFaceInfo->rcFacePos));;
		hr = S_OK;
		break;
	case FACE_RECT_BEGIN_CAPTURE:
		if (*iBufLen != sizeof(FACE_INFO))
		{
			break;
		}
		pcFaceInfo = (FACE_INFO*)pRetData;
		pcFaceInfo->nFaceCount = pHHC->nFaceCount[2];
		memcpy(pcFaceInfo->rcFacePos, pHHC->rcFacePos[2], sizeof(pcFaceInfo->rcFacePos));
		hr = S_OK;
		break;
	case FACE_RECT_BEST_CAPTURE:
		if (*iBufLen != sizeof(FACE_INFO))
		{
			break;
		}
		pcFaceInfo = (FACE_INFO*)pRetData;
		pcFaceInfo->nFaceCount = pHHC->nFaceCount[3];
		memcpy(pcFaceInfo->rcFacePos, pHHC->rcFacePos[3], sizeof(pcFaceInfo->rcFacePos));
		hr = S_OK;
	case FACE_RECT_LAST_CAPTURE:
		if (*iBufLen != sizeof(FACE_INFO))
		{
			break;
		}
		pcFaceInfo = (FACE_INFO*)pRetData;
		pcFaceInfo->nFaceCount = pHHC->nFaceCount[4];
		memcpy(pcFaceInfo->rcFacePos, pHHC->rcFacePos[4], sizeof(pcFaceInfo->rcFacePos));
		hr = S_OK;
		break;
	}

	return hr;
}

HV_API_EX HRESULT CDECL HVAPIUTILS_GetRecordInfoFromAppenedStringEx(LPCSTR szAppened, LPCSTR szInfoName, LPSTR szRetInfo, INT iRetInfoBufLen)
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

HV_API_EX HRESULT CDECL HVAPIUTILS_ParsePlateXmlStringEx(LPCSTR pszXmlPlateInfo, LPSTR pszPlateInfoBuf, INT iPlateInfoBufLen)
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

HV_API_EX HRESULT CDECL HVAPIUTILS_SmallImageToBitmapEx(PBYTE pbSmallImageData, INT nSmallImageWidth, INT nSmallImageHeight, PBYTE pbBitmapData, INT* pnBitmapDataLen)
{
	HRESULT hr = E_FAIL;

	int iBmpLen = *pnBitmapDataLen;
	hr = Yuv2BMP(pbBitmapData, *pnBitmapDataLen, &iBmpLen, pbSmallImageData, nSmallImageWidth, nSmallImageHeight);
	*pnBitmapDataLen = iBmpLen;

	return hr;
}

HV_API_EX HRESULT CDECL HVAPIUTILS_BinImageToBitmapEx(PBYTE pbBinImageData, PBYTE pbBitmapData, INT* pnBitmapDataLen)
{
	if ( NULL == pbBinImageData || NULL == pbBitmapData || NULL == pnBitmapDataLen )
	{
		return E_FAIL;
	}

	INT nBmpLen = *pnBitmapDataLen;
	Bin2BMP(pbBinImageData, pbBitmapData, nBmpLen);
	*pnBitmapDataLen = nBmpLen;

	return S_OK;
}

HV_API_EX HRESULT CDECL HVAPIUTILS_GetExeCmdRetInfoEx(BOOL fIsNewXmlProtocol, LPCSTR pszRetXmlStr, LPCSTR pszCmdName, 
													  LPCSTR pszInfoName, LPSTR pszInfoValue)
{
	if(fIsNewXmlProtocol)
	{
		return HvParseXmlCmdRespRetcode2((char*)pszRetXmlStr, (char*)pszCmdName, (char*)pszInfoName, pszInfoValue);
	}
	else
	{
		if(strcmp(pszCmdName, "RetCode") == 0)
		{
			return HvParseXmlCmdRespRetcode((char*)pszRetXmlStr, pszInfoValue);
		}
		else
		{
			return HvParseXmlInfoRespValue((char*)pszRetXmlStr, (char*)pszCmdName, (char*)pszInfoName, pszInfoValue);
		}
	}
	return E_FAIL;
}

BOOL IsCharacter(const char ch)
{
	int i = ch;
	if ((i >= 97 && i <= 122) 
		|| (i >= 48 && i <= 57) 
		|| (i >= 65 && i <= 90))
	{
		return TRUE;
	}

	if (ch == ' ')
	{
		return TRUE;
	}

	return FALSE;
}

HV_API HRESULT CDECL HVAPIUTILS_SetCharacterValue(HVAPI_HANDLE_EX hHandle, const char* szCharacterDataBuff,
												  INT nTopLeftX, INT nTopLeftY, INT nDateType, INT nSize, INT nR, INT nG, INT nB)
{
	const WORD wPort = CAMERA_CMD_LINK_PORT;
	SOCKET hSocketCmd;
	CAMERA_CMD_HEADER cCmdHeader;
	CAMERA_CMD_RESPOND cCmdRespond;

	if (hHandle == NULL)
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(!ConnectCamera(pHHC->szIP, wPort, hSocketCmd))
	{
		return E_FAIL;
	}

	CHAR_DATA cDataSend;
	cDataSend.nTopLeftX = nTopLeftX;
	cDataSend.nTopLeftY = nTopLeftY;
	cDataSend.nSize = nSize;
	cDataSend.nDateType = nDateType;

	cDataSend.nRGB = 0;
	cDataSend.nRGB = cDataSend.nRGB | nB;
	cDataSend.nRGB = cDataSend.nRGB | (nG << 8);
	cDataSend.nRGB = cDataSend.nRGB | (nR << 16);

	HFONT hf = CreateFont(nSize, 0, 0, 0, /*FW_BOLD*/FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("宋体"));
	HDC hdcCompatible = CreateCompatibleDC(NULL);
	SelectObject(hdcCompatible, hf);

	HBITMAP hbmScreen = CreateCompatibleBitmap(hdcCompatible, nSize, nSize); 

	if (hbmScreen == 0)
	{
		return E_FAIL;
	}

	if (!SelectObject(hdcCompatible, hbmScreen) )
	{
		return E_FAIL;
	}

	// Select the bitmaps into the compatible DC. 
	//SetBkColor(hdcCompatible, GetBkColor(hdcCompatible));

	int nCharacterNum;
	nCharacterNum = (int)strlen(szCharacterDataBuff);    //字符个数
	int nCharacterLen = nCharacterNum * (nSize / 2);    //字符总列数
	int iCharDataLen = nSize % 8;    //计算每一列所需空间长度
	if (iCharDataLen)
	{
		iCharDataLen = nSize / 8 + 1;
	}
	else
	{
		iCharDataLen = nSize / 8;
	}

	int nRow, nColumn;
	nRow = nSize;
	int iTmp = iCharDataLen * nCharacterLen;
	PBYTE pszDate= new BYTE[iTmp];
	memset(pszDate, 0, iTmp);
	PBYTE pszTmp;
	pszTmp = pszDate;

	BYTE rgb[8] = {1, 2, 4, 8, 16, 32, 64, 128};
	COLORREF color = 0;

	int ii = 0;
	for (int i = 0; i < nCharacterNum; i++)
	{
		BOOL bChar = IsCharacter(szCharacterDataBuff[i]);
		char szValue[3] = {0};
		int iCount = 0;
		if (bChar)
		{
			nColumn = nSize / 2;
			szValue[0] = szCharacterDataBuff[i];
			szValue[1] = '\0';
			iCount = 1;
		}
		else
		{
			nColumn = nSize;
			szValue[0] = szCharacterDataBuff[i];
			szValue[1] = szCharacterDataBuff[i+1];
			szValue[2] = '\0';
			iCount = 2;
			i++;
		}

		TextOut(hdcCompatible, 0, 0, szValue, iCount);

		for (int ic = 0; ic < nColumn; ic++)
		{
			for (int ir = 0; ir < nRow; ir++)
			{
				int iFlag = (ir + 1) % 8;
				if (!iFlag && ir != (nRow - 1))
				{
					pszTmp++;
				}

				COLORREF colorTmp = GetPixel(hdcCompatible, ic, ir);
				if (colorTmp == color)
				{
					*pszTmp = *pszTmp | rgb[iFlag];
					ii++;
				}
			}
			pszTmp++;
		}
	}


	cCmdHeader.dwID = CAMERA_SET_CHARACTER;
	cCmdHeader.dwInfoSize = sizeof(cDataSend) + iTmp;

	if ( sizeof(cCmdHeader) != send(hSocketCmd, (char*)&cCmdHeader, sizeof(cCmdHeader), 0) )
	{
		ForceCloseSocket(hSocketCmd);
		delete[] pszDate;
		return E_FAIL;
	}

	//发送字符信息
	if ( sizeof(cDataSend) != send(hSocketCmd, (char*)&cDataSend, sizeof(cDataSend), 0))
	{
		ForceCloseSocket(hSocketCmd);
		delete[] pszDate;
		return E_FAIL;
	}

	//发送点阵数据
	if ( iTmp != send(hSocketCmd, (char*) pszDate, iTmp, 0))
	{
		ForceCloseSocket(hSocketCmd);
		delete[] pszDate;
		return E_FAIL;
	}

	if (sizeof(cCmdRespond) == recv(hSocketCmd, (char*) &cCmdRespond, sizeof(cCmdRespond), 0))
	{
		if ( CAMERA_SET_CHARACTER == cCmdRespond.dwID
			&& 0 == cCmdRespond.dwResult )
		{
			ForceCloseSocket(hSocketCmd);
			delete[] pszDate;
			return S_OK;
		}
	}

	ForceCloseSocket(hSocketCmd);

	delete[] pszDate;
	return E_FAIL;
}