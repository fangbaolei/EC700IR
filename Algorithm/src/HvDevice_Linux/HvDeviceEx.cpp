#include <sys/time.h>
#include <time.h>
#include<stdio.h>
#include<string.h>
#include<string>
#include <errno.h>
#include <map>


#include "HvDeviceEx.h"
#include "HvDeviceUtils.h"
#include "inifile.h"
#include "encode.h"



#include "CarFaceCtrl.h"




#define DEFAULT_CONNCMD  "DownloadRecord,BeginTime[2012.01.01_01],Index[0],Enable[0],EndTime[0],DataInfo[0]"

const int MAX_DEVICE_COUNT = 1000;

HVAPI_HANDLE_EX g_HandleArray[MAX_DEVICE_COUNT] = {NULL};

extern HRESULT IsEmptyPackCache( HVAPI_HANDLE_CONTEXT_EX* pHandle , PACK_TYPE eType , bool& fIsEmptyCache);
extern HRESULT GetPackCache( HVAPI_HANDLE_CONTEXT_EX* pHandle , PACK_TYPE eType ,unsigned char** ppBuffer , int& iBufferLen );
extern HRESULT UpdatePackCache(HVAPI_HANDLE_CONTEXT_EX* pHandle, PACK_TYPE eType , char* pBuffer , int nBufferLen );

extern HRESULT CloseAutoLinkHHC(LPSTR szDevSN);
//extern HRESULT OutPutDebugInfor(char* pDebugInfo);
extern HRESULT SetConnAutoLinkHHC(LPSTR szDevSN);

static void* StatusMonitorThreadFuncEx(LPVOID lpParam);
 void * RecordRecvThreadFuncEx(LPVOID lpParam);
static void *ImageRecvThreadFuncEx(LPVOID lpParam);
static void * VideoRecvThreadFuncEx(LPVOID lpParam);
static unsigned long GetTickCount();
HRESULT ProcHistoryVideoJpegFrame(HVAPI_HANDLE_CONTEXT_EX* pHHC,  PBYTE pbFrameData, DWORD dwFrameDataLen, LPCSTR szVideoExtInfo );
HRESULT ProcRecordDataPacket(HVAPI_HANDLE_CONTEXT_EX* pHHC,  unsigned char* pbRecordData,  DWORD dwDataLen,  DWORD dwRecordType, LPCSTR szAppendInfo );
HRESULT ProcBigImageCallBack(HVAPI_CALLBACK_SET* pCallBackSet,DWORD dwType, RECORD_IMAGE_EX* pcImage, BOOL fIsPeccancy,
DWORD dwEnhanceFlag, INT iBrightness, INT iHueThreshold, INT iCompressRate, DWORD dwRecordType, DWORD64 dwTimeMS );
HRESULT ProcLprImageFrame(HVAPI_HANDLE_CONTEXT_EX* pHHC, PBYTE pbImgData, DWORD dwImgDataLen, DWORD dwImgDataOffSet, char *pszImageExtInfo);
HRESULT SaveRecordConnCmd(HVAPI_HANDLE_CONTEXT_EX* pHHC);

int FindEmptyHandle()
{
    int index = -1;
    for ( int i=0; i<MAX_DEVICE_COUNT; i++)
    {
        if ( g_HandleArray[i] == NULL )
        {
            index = i;
            break;
        }
    }
    return index;
}


 static int svTraceCallback(
        const char* szInfo,     ///< 字符串
        int nLen                ///< 字符串长度+1
    )
    {
        // todo 打印t
        FILE *myFile = fopen("./log.txt", "a+");
        if (myFile)
        {
            fprintf(myFile, "%s\r\n", szInfo);
            fclose(myFile);
        }
        return 0;
    }



 HV_API_EX HRESULT  CDECL HVAPI_SearchDeviceCount(DWORD32 *pdwDeviceCount)
{
    if ( pdwDeviceCount == NULL )
        return E_FAIL;

    DWORD32 dwDeviceCount = 0;
    HRESULT hr = SearchHVDeviceCount( &dwDeviceCount );
    *pdwDeviceCount = dwDeviceCount;
    return hr;
}

 HV_API_EX HRESULT  CDECL HVAPI_GetDeviceAddr(DWORD32 dwIndex , DWORD64 *dw64MacAddr, DWORD32 *dwIP, DWORD32 *dwMask, DWORD32 *dwGateWay)
{
    return  GetHVDeviceAddr(dwIndex,  dw64MacAddr, dwIP, dwMask, dwGateWay);
}

 HV_API_EX HRESULT  CDECL  HVAPI_SetIPByMacAddr( DWORD64 dw64MacAddr, DWORD32 dwIP, DWORD32 dwMask, DWORD32 dwGateWay)
{
    return SetIPFromMac(dw64MacAddr, dwIP,dwMask, dwGateWay);
}

 HV_API_EX HRESULT  CDECL HVAPI_GetDeviceInfoEx(int iIndex, LPSTR  lpDevTypeInfo, int iBufLen)
{
    return HvGetDeviceExteInfo(iIndex, lpDevTypeInfo, iBufLen);
}

 HV_API_EX HRESULT  CDECL HVAPI_GetDevTypeEx(PSTR pcIP, int *iDeviceType)
{
    const int iBufLen = (1024 << 4);
    char *pszXmlBuf = new char[iBufLen];
    int iXmlBufLen = iBufLen;
    *iDeviceType = 0;
    if ( pszXmlBuf == NULL )
        return E_FAIL;

    memset(pszXmlBuf, 0, iBufLen );
    DWORD dwXmlVersion = 0;
    HvGetXmlProtocolVersion(pcIP, &dwXmlVersion);
    bool fIsNewXml = (dwXmlVersion ==0 ) ? false: true;

    if ( HvMakeXmlInfoByString(fIsNewXml, "GetDevType", 10, pszXmlBuf, iXmlBufLen)  == E_FAIL )
    {
        SAFE_DELETE_ARG(pszXmlBuf);
        return E_FAIL;
    }

    char *pszRetBuf = new char[1024];
    if ( pszRetBuf == NULL )
    {
        SAFE_DELETE_ARG(pszXmlBuf);
        return E_FAIL;
    }
    memset(pszRetBuf, 0 , 1024);
    int iRetBufLen = 1024;
    int sktCmd = INVALID_SOCKET;
    if (  ! ExecXmlExtCmdEx(pcIP, (char*)pszXmlBuf, (char*)pszRetBuf, iRetBufLen, sktCmd)  )
    {
        SAFE_DELETE_ARG(pszRetBuf);
        SAFE_DELETE_ARG(pszXmlBuf);
        return E_FAIL;
    }

    char szRetCode[20] = {0};
    if (dwXmlVersion == 0 )
    {
         if ( HvParseXmlInfoRespValue(pszRetBuf, "GetDevType", "DevType", szRetCode) == E_FAIL)
         {
             SAFE_DELETE_ARG(pszRetBuf);
             SAFE_DELETE_ARG(pszXmlBuf);
             return E_FAIL;
         }
    }
    else
    {
       if ( HvParseXmlCmdRespRetcode2(pszRetBuf, "GetDevType", "DevType", szRetCode) == E_FAIL)
       {
           SAFE_DELETE_ARG(pszRetBuf);
           SAFE_DELETE_ARG(pszXmlBuf);
           return E_FAIL;
       }
    }

    if ( strstr(szRetCode, "DM6467") != NULL )
    {
        if ( strstr(szRetCode, "_S") != NULL )
            *iDeviceType = DEV_TYPE_HVSIGLE;
        else if ( strstr(szRetCode, "DM6467_200W_LITE") != NULL )
            *iDeviceType = DEV_TYPE_HVCAM_SINGLE;
        else if ( strstr(szRetCode, "200W_CAMERA") != NULL )
            *iDeviceType = DEV_TYPE_HVCAMERA;
        else if ( strstr(szRetCode, "_200W") != NULL )
            *iDeviceType = DEV_TYPE_HVCAM_200W;
        else if (strstr(szRetCode, "_500W") != NULL )
            *iDeviceType = DEV_TYPE_HVCAM_500W;
        else
            *iDeviceType = DEV_TYPE_UNKNOWN;
    }
    else
    {
        *iDeviceType = DEV_TYPE_UNKNOWN;
    }

    SAFE_DELETE_ARG(pszRetBuf);
    SAFE_DELETE_ARG(pszXmlBuf);
    return S_OK;
}


 HV_API_EX HVAPI_HANDLE_EX  CDECL HVAPI_OpenEx(LPCSTR szIp, LPCSTR szApiVer)
{
    if ( szIp == NULL )
        return NULL;
    if ( szApiVer == NULL )
        szApiVer = HVAPI_API_VERSION_EX;

    HVAPI_HANDLE_EX hRetHandle = NULL;
    if ( strcmp( HVAPI_API_VERSION_EX, szApiVer ) == 0 )
    {
        HVAPI_HANDLE_CONTEXT_EX *hContextHandle = new HVAPI_HANDLE_CONTEXT_EX();
        if ( hContextHandle != NULL )
        {
        	strcpy(hContextHandle->szVersion, szApiVer);
            strcpy(hContextHandle->szIP, szIp);
            hContextHandle->dwOpenType = 1;
            hRetHandle = (HVAPI_HANDLE_EX)hContextHandle;

           // hContextHandle->fNewProtocol  =  IsNewProtocol(hContextHandle->szIP);

			hContextHandle->emProtocolVersion = GetProtocolVersion(hContextHandle->szIP);
		   	if ( PROTOCOL_VERSION_1 == hContextHandle->emProtocolVersion || PROTOCOL_VERSION_2 == hContextHandle->emProtocolVersion)
		   	{
			   	char szRetBuf[512] = {0};

	            if ( HVAPI_ExecCmdEx( hRetHandle,  "GetVersionString", szRetBuf, sizeof(szRetBuf), NULL) != S_OK )
	            {
	                char szLog[256] = {0};
	                sprintf(szLog, "[HvDevice] %s:GetVersionString Fail", szIp);
	                WriteLog(szLog);

	                delete hContextHandle;
	                hContextHandle = NULL;
	                return NULL;
	            }
		   	}


            if ( pthread_create( &hContextHandle->pthreadStatusMonitor, NULL, StatusMonitorThreadFuncEx,  (void*)hContextHandle) )
            {
                delete hContextHandle;
                hContextHandle = NULL;

                char szLog[256] = {0};
                sprintf(szLog, "[HvDevice] %s:pthread_create status Monitor  Fail", szIp);
                WriteLog(szLog);
                return NULL;
            }

        }
    }

    char szLog[256] = {0};
    sprintf(szLog, "[HvDevice] Addr:%s, Handle: 0x%x",szIp, (DWORD32)hRetHandle);
    WriteLog(szLog);

  return hRetHandle;
}



 HV_API_EX HRESULT  CDECL HVAPI_CloseEx(HVAPI_HANDLE_EX hHandle)
{
    if ( hHandle == NULL )
        return E_FAIL;

    HVAPI_HANDLE_CONTEXT_EX *pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
    if ( pHHC->dwOpenType != 1 )
        return E_FAIL;

    pHHC->fStatusMonotorExit = true;
    pHHC->fIsThreadRecvRecordExit = true;
    pHHC->fIsThreadRecvImageExit = true;
    pHHC->fIsThreadRecvVideoExit = true;

    char szLog[1024] = {0};
    if ( pHHC->pthreadStatusMonitor != 0 )
   {
        HvSafeCloseThread(pHHC->pthreadStatusMonitor);
        sprintf(szLog, "[LibHvDevice]  %s Thread status check thread Quit ", pHHC->szIP);
        WriteLog(szLog);
   }

    if ( pHHC->pthreadRecvRecord !=  0 )
    {
        HvSafeCloseThread(pHHC->pthreadRecvRecord);
        sprintf(szLog, "[LibHvDevice]  %s Record Rev Thread Quit ", pHHC->szIP);
        WriteLog(szLog);
    }

    if ( pHHC->pthreadRecvImage != 0 )
   {
        HvSafeCloseThread(pHHC->pthreadRecvImage);
        sprintf(szLog, "[LibHvDevice]  %s Image Video Rev Thread Quit ", pHHC->szIP);
        WriteLog(szLog);
   }

    if ( pHHC->pthreadRecvVideo !=  0  )
    {
        HvSafeCloseThread(pHHC->pthreadRecvVideo);
        sprintf(szLog, "[LibHvDevice]  %s H264 Video Rev  Thread Quit ", pHHC->szIP);
        WriteLog(szLog);
    }

    ForceCloseSocket(pHHC->sktRecord);
    ForceCloseSocket(pHHC->sktImage);
    ForceCloseSocket(pHHC->sktVideo);

    pHHC->fVailPackResumeCache = FALSE;
	if (NULL != pHHC->pPackResumeCache)
	{
		PACK_RESUME_CACHE* pPackResumeCache = pHHC->pPackResumeCache;
		if (NULL != pPackResumeCache->pInfor)
		{
			delete[] pPackResumeCache->pInfor;
			pPackResumeCache->pInfor = NULL;
		}

		if (NULL != pPackResumeCache->pData)
		{
			delete[] pPackResumeCache->pData;
			pPackResumeCache->pData = NULL;
		}
	}

	if ( pHHC->pCarFaceRecogCtrl != NULL )
	{
		delete pHHC->pCarFaceRecogCtrl;
        pHHC->pCarFaceRecogCtrl = NULL;
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
 HV_API_EX HRESULT  CDECL HVAPI_GetXmlVersionEx(HVAPI_HANDLE_EX hHandle, PROTOCOL_VERSION* pemProtocolVersion)
{
    if ( hHandle == NULL || pemProtocolVersion == NULL )
        return E_FAIL;
    HVAPI_HANDLE_CONTEXT_EX *pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
    if (pHHC->dwOpenType != 1)
        return E_FAIL;

    *pemProtocolVersion = pHHC->emProtocolVersion;

    return S_OK;

}
 HV_API_EX HRESULT  CDECL HVAPI_GetConnStatusEx(HVAPI_HANDLE_EX hHandle, int nStreamType, DWORD *pdwConStatus)
{
    if ( hHandle == NULL || pdwConStatus == NULL )
        return E_FAIL;

    HVAPI_HANDLE_CONTEXT_EX *pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
    if ( 0!=strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) || pHHC->dwOpenType != 1 )
        return E_FAIL;

   if (CONN_TYPE_RECORD == nStreamType)
    {
        *pdwConStatus = pHHC->dwRecordConnStatus;
    }
    else if ( CONN_TYPE_IMAGE == nStreamType )
    {
        *pdwConStatus = pHHC->dwImageConnStatus;
    }
    else if (CONN_TYPE_VIDEO == nStreamType)
    {
         *pdwConStatus = pHHC->dwVideoConnStatus;
    }
    else
    {
        return E_FAIL;
    }

    return S_OK;
}

 HV_API_EX HRESULT  CDECL HVAPI_GetReConnectTimesEx(HVAPI_HANDLE_EX hHandle, INT nStreamType, DWORD *pdwReConnectTimes, BOOL fIsReset)
{
   if ( hHandle == NULL || pdwReConnectTimes == NULL )
        return E_FAIL;

    HVAPI_HANDLE_CONTEXT_EX *pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
    if ( 0!=strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) || pHHC->dwOpenType != 1 )
        return E_FAIL;

   if (CONN_TYPE_RECORD == nStreamType)
    {
        *pdwReConnectTimes = pHHC->dwRecordReconnectTimes;
        if ( fIsReset )
            pHHC->dwRecordReconnectTimes = 0;
    }
    else if ( CONN_TYPE_IMAGE == nStreamType )
    {
        *pdwReConnectTimes = pHHC->dwImageReconnectTimes;
        if ( fIsReset )
            pHHC->dwImageReconnectTimes = 0;
    }
    else if (CONN_TYPE_VIDEO == nStreamType)
    {
         *pdwReConnectTimes = pHHC->dwVideoReconnectTimes;
        if ( fIsReset )
            pHHC->dwVideoReconnectTimes = 0;
    }
    else
    {
        return E_FAIL;
    }

    return S_OK;
}

 HV_API_EX HRESULT  CDECL HVAPI_ExecCmdEx(HVAPI_HANDLE_EX hHandle, LPCSTR szCmd,LPSTR szRetBuf, INT nBufLen, INT* pnRetLen)
{
    if ( hHandle ==NULL || szCmd == NULL || szRetBuf == NULL || nBufLen <= 0 )
        return E_FAIL;


    HVAPI_HANDLE_CONTEXT_EX *pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
    if ( strcmp(pHHC->szVersion, HVAPI_API_VERSION_EX) != 0 || pHHC->dwOpenType != 1  )
        return E_FAIL;

    const int iBufLen = (1024<<4);
    char *pszXmlBuf = new char[iBufLen];
    if ( pszXmlBuf == NULL )
        return E_FAIL;

	char *szCmdTemp = (char*) szCmd;


    int iXmlBufLen = iBufLen;
    TiXmlDocument cXmlDoc;
	HRESULT hr = S_OK;

    if ( !cXmlDoc.Parse(szCmd) )
    {
    	switch(pHHC->emProtocolVersion)
    	{
    		case PROTOCOL_VERSION_1:
				{
					hr = HvMakeXmlCmdByString1( szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen);
    			}
				break;
			case PROTOCOL_VERSION_2:
			case PROTOCOL_VERSION_EARTH:
				{
					hr = HvMakeXmlCmdByString2( szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen);
				}
				break;
			case PROTOCOL_VERSION_MERCURY:
			default:
				{
					hr = HvMakeXmlCmdByString3( szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen);
				}
				break;
    	}

		szCmdTemp = pszXmlBuf;

    }

	if ( S_OK != hr )
	{
		SAFE_DELETE_ARG(pszXmlBuf);
		return E_FAIL;
	}


	bool fRet = false;
	int sktCmd = INVALID_SOCKET;
	switch(pHHC->emProtocolVersion)
    {

		case PROTOCOL_VERSION_1:
		case PROTOCOL_VERSION_2:
		case PROTOCOL_VERSION_EARTH:
			{
				fRet = ExecXmlExtCmdEx(pHHC->szIP, (char *)szCmdTemp, (char*)szRetBuf,nBufLen, sktCmd);
			}
			break;
		case PROTOCOL_VERSION_MERCURY:
		default:
			{
				fRet = ExecXmlExtCmdMercury(pHHC->szIP, (char*)szCmdTemp, (char*)szRetBuf, nBufLen, sktCmd);
			}
			break;
    }

    if ( pnRetLen != NULL )
    {
        *pnRetLen = nBufLen;
    }
	/*
    char szLog[2048] = {0};
    if ( fRet)
        sprintf(szLog, "[HvDevice] %s  cmd: %s --Success ", pHHC->szIP,  szCmd );
    else
        sprintf(szLog,  "[HvDevice] %s cmd: %s --Fail" , pHHC->szIP,  szCmd);

    WriteLog(szLog);*/

    SAFE_DELETE_ARG(pszXmlBuf);

    return (true== fRet ) ? S_OK : E_FAIL;

}

 HV_API_EX HRESULT  CDECL HVAPI_GetParamEx(HVAPI_HANDLE_EX hHandle,LPSTR szXmlParam, INT nBufLen, INT *pnRetLen)
{
    if ( NULL == hHandle || NULL == szXmlParam )
        return E_FAIL;
    HVAPI_HANDLE_CONTEXT_EX *pHHC = ( HVAPI_HANDLE_CONTEXT_EX*)hHandle;
    if ( 0 != strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion ) || pHHC->dwOpenType != 1  )
        return E_FAIL;

    const int iBufLen = (1024 << 10);
    char * rgchXmlParamBuf = new char[iBufLen];
    if ( rgchXmlParamBuf == NULL )
        return E_FAIL;

    memset(rgchXmlParamBuf, 0,  iBufLen);

    const WORD wPort = CAMERA_CMD_LINK_PORT;
    int hSocketCmd = -1;

    CAMERA_CMD_HEADER cCmdHeader;
    CAMERA_CMD_RESPOND cCmdRespond;

    if (!ConnectCamera(pHHC->szIP, wPort, hSocketCmd ) )
    {
        SAFE_DELETE_ARG(rgchXmlParamBuf);
        return E_FAIL;
    }

    cCmdHeader.dwID = CAMERA_GET_PARAM_CMD;
    cCmdHeader.dwInfoSize = 0;

    if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0  ))
    {
        ForceCloseSocket(hSocketCmd);
        SAFE_DELETE_ARG(rgchXmlParamBuf);
        return E_FAIL;
    }

    if ( sizeof(cCmdRespond)  != RecvAll(hSocketCmd,  (char*)&cCmdRespond, sizeof(cCmdRespond)))
    {
        ForceCloseSocket(hSocketCmd);
        SAFE_DELETE_ARG(rgchXmlParamBuf);
        return E_FAIL;
    }

    if ( CAMERA_GET_PARAM_CMD != cCmdRespond.dwID || 0!=cCmdRespond.dwResult  || 0>=cCmdRespond.dwInfoSize )
    {
         ForceCloseSocket(hSocketCmd);
        SAFE_DELETE_ARG(rgchXmlParamBuf);
        return E_FAIL;
    }

    if (cCmdRespond.dwInfoSize  == RecvAll(hSocketCmd, rgchXmlParamBuf, cCmdRespond.dwInfoSize) )
    {
        if ( nBufLen > (int)cCmdRespond.dwInfoSize )
        {
            if ( pnRetLen )
                *pnRetLen = cCmdRespond.dwInfoSize;
            memcpy(szXmlParam, rgchXmlParamBuf, cCmdRespond.dwInfoSize);
            ForceCloseSocket(hSocketCmd);
            SAFE_DELETE_ARG(rgchXmlParamBuf);
            return S_OK;
        }
        else
        {
            if ( pnRetLen )
                *pnRetLen = cCmdRespond.dwInfoSize;
            ForceCloseSocket(hSocketCmd);
            SAFE_DELETE_ARG(rgchXmlParamBuf);
            return E_FAIL;
        }
    }

    ForceCloseSocket(hSocketCmd);
    SAFE_DELETE_ARG(rgchXmlParamBuf);
    return E_FAIL;
}

 HV_API_EX HRESULT  CDECL HVAPI_SetParamEx(HVAPI_HANDLE_EX hHandle, LPCSTR szXmlParam)
{
     if ( NULL == hHandle || NULL == szXmlParam )
        return E_FAIL;
    HVAPI_HANDLE_CONTEXT_EX *pHHC = ( HVAPI_HANDLE_CONTEXT_EX*)hHandle;
    if ( 0 != strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion ) || pHHC->dwOpenType != 1  )
        return E_FAIL;

     const WORD wPort = CAMERA_CMD_LINK_PORT;
    int hSocketCmd = -1;

    CAMERA_CMD_HEADER cCmdHeader;
    CAMERA_CMD_RESPOND cCmdRespond;

    if (!ConnectCamera(pHHC->szIP, wPort, hSocketCmd ) )
    {
        return E_FAIL;
    }

    cCmdHeader.dwID = CAMERA_SET_PARAM_CMD;
    cCmdHeader.dwInfoSize = (int)strlen(szXmlParam) + 1;

    if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0 ) )
    {
        ForceCloseSocket(hSocketCmd);
        return E_FAIL;
    }

    if (cCmdHeader.dwInfoSize != send(hSocketCmd, (const char*)szXmlParam, cCmdHeader.dwInfoSize, 0 ) )
    {
        ForceCloseSocket(hSocketCmd);
        return E_FAIL;
    }

    if ( sizeof(cCmdRespond) != RecvAll(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond) ) )
    {
         ForceCloseSocket(hSocketCmd);
        return E_FAIL;
    }

    if ( CAMERA_SET_PARAM_CMD != cCmdRespond.dwID || 0 != cCmdRespond.dwResult )
    {
         ForceCloseSocket(hSocketCmd);
        return E_FAIL;
    }
    return S_OK;
}

 HV_API_EX HRESULT  CDECL HVAPI_SendControllPannelUpDataFileEx(HVAPI_HANDLE_EX hHandle, PBYTE pUpDateFileBuffer, DWORD dwFileSize)
{
    if ( NULL == hHandle || NULL == pUpDateFileBuffer )
        return E_FAIL;
    HVAPI_HANDLE_CONTEXT_EX *pHHC = ( HVAPI_HANDLE_CONTEXT_EX*)hHandle;
    if ( 0 != strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion ) || pHHC->dwOpenType != 1  )
        return E_FAIL;

     const WORD wPort = CAMERA_CMD_LINK_PORT;
    int hSocketCmd = -1;

    CAMERA_CMD_HEADER cCmdHeader;
    CAMERA_CMD_RESPOND cCmdRespond;

    if (!ConnectCamera(pHHC->szIP, wPort, hSocketCmd ) )
    {
        return E_FAIL;
    }

    cCmdHeader.dwID = CAMERA_UPDATE_CONTROLL_PANNEL;
    cCmdHeader.dwInfoSize = (int)dwFileSize+1;

    if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0 ) )
    {
        ForceCloseSocket(hSocketCmd);
        return E_FAIL;
    }

    if (cCmdHeader.dwInfoSize != send(hSocketCmd, (const char*)pUpDateFileBuffer, cCmdHeader.dwInfoSize, 0 ) )
    {
        ForceCloseSocket(hSocketCmd);
        return E_FAIL;
    }

    if ( sizeof(cCmdRespond) != RecvAll(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond) ) )
    {
         ForceCloseSocket(hSocketCmd);
        return E_FAIL;
    }

    if ( CAMERA_UPDATE_CONTROLL_PANNEL != cCmdRespond.dwID || 0 != cCmdRespond.dwResult )
    {
         ForceCloseSocket(hSocketCmd);
        return E_FAIL;
    }
    return S_OK;
}

HRESULT SetRecordCallBack(HVAPI_HANDLE_EX hHandle, PVOID pFunc, PVOID pUserData, INT iVideoID,INT iCallBackType, LPCSTR szConnCmd )
{
     if ( hHandle == NULL )
        return E_FAIL;
    HVAPI_HANDLE_CONTEXT_EX *pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;

    if ( strcmp(pHHC->szVersion, HVAPI_API_VERSION_EX) != 0 || pHHC->dwOpenType != 1  )
        return E_FAIL;
    switch( iCallBackType)
    {
        case CALLBACK_TYPE_RECORD_PLATE:
        {
            if ( pFunc == NULL  )
            {
                pHHC->dwRecordConnStatus = CONN_STATUS_DISCONN;
                pHHC->fIsThreadRecvRecordExit = true;
                HvSafeCloseThread(pHHC->pthreadRecvRecord);
                pHHC->pthreadRecvRecord = 0;
                ForceCloseSocket(pHHC->sktRecord);
                pHHC->sktRecord = -1;

                HVAPI_CALLBACK_SET *pTemp = NULL;
                for (pTemp = pHHC->pCallBackSet; pTemp!=NULL; pTemp=pTemp->pNext)
                {
                    if ( pTemp->iVideoID == iVideoID )
                    {
                            pTemp->pOnPlate = NULL;
                            pTemp->pOnPlateParam = NULL;
                            break;
                    }
                }
                return S_OK;
            }


            const int MAXPATH = 260;
            char szFileName[MAXPATH] = {0};
            char szIniName[MAXPATH] = {0};
            GetProcessTruePath(szFileName, MAXPATH);
            strncpy(szIniName, szFileName, MAXPATH);
            strncat(szIniName, "/HvDevice.ini", MAXPATH - 1);

            int iEnable = GetPrivateProfileInt("CARFACEENABLE", "Enable", 8, szIniName);
            if (iEnable == 8)
            {
                iEnable = 0;
                char szEnable[10];
                sprintf(szEnable, "%d", iEnable);
                WritePrivateProfileString("CARFACEENABLE", "Enable", szEnable, szIniName);
                pHHC->fEnableCarFaceCtrl = false;
            }
            else
            {
                if (iEnable == 1)
                    pHHC->fEnableCarFaceCtrl = true;
                else
                    pHHC->fEnableCarFaceCtrl = false;

            }


            //获取是否识别开关

            if (pHHC->fEnableCarFaceCtrl)
            {

                if ( pHHC->pCarFaceRecogCtrl!= NULL )
                {
                    delete pHHC->pCarFaceRecogCtrl;
                    pHHC->pCarFaceRecogCtrl = NULL;
                }

                pHHC->pCarFaceRecogCtrl = new  CCarFaceCtrl();
                if ( pHHC->pCarFaceRecogCtrl->InitCtrl() == S_OK )
                {
                    //sv::utSetTraceCallBack_TXT(svTraceCallback);

                    WriteLog("Init FaceCtrl Success");
                }
                else
                {
                    WriteLog("Inint FaceCtrl falit ");
                    delete pHHC->pCarFaceRecogCtrl;
                    pHHC->pCarFaceRecogCtrl = NULL;
                    pHHC->fEnableCarFaceCtrl = false;

                }
            }

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

            if ( pHHC->dwRecordConnStatus != CONN_STATUS_UNKNOWN &&  pHHC->dwRecordConnStatus != CONN_STATUS_DISCONN )
            {
                 HVAPI_CALLBACK_SET *pTemp = NULL;
                    for (pTemp = pHHC->pCallBackSet; pTemp!=NULL; pTemp=pTemp->pNext)
                    {
                        if ( pTemp->iVideoID == iVideoID )
                        {
                                pTemp->pOnPlate = (HVAPI_CALLBACK_RECORD_PLATE)pFunc;
                                pTemp->pOnPlateParam = (PVOID)pUserData;
                                break;
                        }
                    }

                    if ( pTemp == NULL )
                    {
                        pTemp = new HVAPI_CALLBACK_SET();
                        memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET) );
                        pTemp->iVideoID = iVideoID;
                        pTemp->pOnPlate = (HVAPI_CALLBACK_RECORD_PLATE)pFunc;
                        pTemp->pOnPlateParam = (PVOID)pUserData;
                        pTemp->pNext = pHHC->pCallBackSet;
                        pHHC->pCallBackSet = pTemp;
                    }
                return S_OK;
            }

            if ( !ConnectCamera(pHHC->szIP, CAMERA_RECORD_LINK_PORT, pHHC->sktRecord) )
            {
                HVAPI_CALLBACK_SET *pTemp = NULL;
                for (pTemp = pHHC->pCallBackSet; pTemp!=NULL; pTemp=pTemp->pNext)
                {
                    if ( pTemp->iVideoID == iVideoID )
                    {
                            pTemp->pOnPlate = NULL;
                            pTemp->pOnPlateParam = NULL;
                            break;
                    }
                }
                return E_FAIL;
            } //connect phote failt

            SaveRecordConnCmd(pHHC);

             if ( szConnCmd != NULL && sizeof(pHHC->szRecordConnCmd) > strlen(szConnCmd)  )
             {
                 strcpy(pHHC->szRecordConnCmd, szConnCmd);
                 char szRetBuf[256] = {0};
                  if ( HvSendXmlCmd(pHHC->szIP, pHHC->szRecordConnCmd, szRetBuf, sizeof(szRetBuf), NULL, pHHC->sktRecord) != S_OK )
                 {
                    ForceCloseSocket(pHHC->sktRecord);
                    return E_FAIL;
                  }//hvsendxmlcmd

                   if ( strstr(pHHC->szRecordConnCmd , "DownloadRecord")&& strstr(pHHC->szRecordConnCmd , "Enable[1]"))
                    {
                        pHHC->fIsRecvHistoryRecord = TRUE;
                        char *pTempStr = strstr(pHHC->szRecordConnCmd, "BeginTime");
                        if ( pTempStr )
                        {
                            memcpy(pHHC->szRecordBeginTimeStr, pTempStr+10, 13);
                            pHHC->szRecordBeginTimeStr[13] = '\0';
                        }
                        pTempStr =  strstr(pHHC->szRecordConnCmd, "Index");
                        if ( pTempStr )
                            sscanf(pTempStr, "Index[%d]", &pHHC->dwRecordStartIndex);

                          pTempStr =  strstr(pHHC->szRecordConnCmd, "EndTime");
                          if ( pTempStr )
                          {
                              if ( pTempStr[9] == ']' )
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
                            pTempStr =  strstr(pHHC->szRecordConnCmd, "DataInfo");
                             if ( pTempStr )
                                scanf(pTempStr, "DataInfo[%d]", &pHHC->iRecordDataInfo);

                    }
             }//szConnCMd

              //Set CallBack
           HVAPI_CALLBACK_SET *pTemp = NULL;
            for (pTemp = pHHC->pCallBackSet; pTemp!=NULL; pTemp=pTemp->pNext)
            {
                 if ( pTemp->iVideoID == iVideoID )
                {
                        pTemp->pOnPlate = (HVAPI_CALLBACK_RECORD_PLATE)pFunc;
                        pTemp->pOnPlateParam = (PVOID)pUserData;
                        break;
                }
            }
             if ( pTemp == NULL )
              {
                    pTemp = new HVAPI_CALLBACK_SET();
                    memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET) );
                    pTemp->iVideoID = iVideoID;
                    pTemp->pOnPlate = (HVAPI_CALLBACK_RECORD_PLATE)pFunc;
                    pTemp->pOnPlateParam = (PVOID)pUserData;
                    pTemp->pNext = pHHC->pCallBackSet;
                    pHHC->pCallBackSet = pTemp;
            }
            //genertor thread
            pHHC->fIsThreadRecvRecordExit = FALSE;

           if (  pthread_create(&pHHC->pthreadRecvRecord, NULL,  RecordRecvThreadFuncEx, (void*)pHHC) )
           {
                pHHC->fIsThreadRecvRecordExit = true;
                pHHC->pthreadRecvRecord = 0;
                ForceCloseSocket(pHHC->sktRecord);
                pHHC->sktRecord = -1;

                HVAPI_CALLBACK_SET *pTemp = NULL;
                for (pTemp = pHHC->pCallBackSet; pTemp!=NULL; pTemp=pTemp->pNext)
                {
                    if ( pTemp->iVideoID == iVideoID )
                    {
                            pTemp->pOnPlate = NULL;
                            pTemp->pOnPlateParam = NULL;
                            break;
                    }
                }
                return E_FAIL;
           }

           pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
           return S_OK;

        }
            break ;
        case CALLBACK_TYPE_RECORD_BIGIMAGE:
        {
            HVAPI_CALLBACK_SET *pTemp = NULL;
            for (pTemp = pHHC->pCallBackSet; pTemp!=NULL; pTemp=pTemp->pNext)
            {
                if (pTemp->iVideoID == iVideoID )
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

            if ( pTemp == NULL )
            {
                pTemp = new HVAPI_CALLBACK_SET();
                memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET) );
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
            HVAPI_CALLBACK_SET *pTemp = NULL;
            for (pTemp = pHHC->pCallBackSet; pTemp!=NULL; pTemp=pTemp->pNext)
            {
                    if (pTemp->iVideoID == iVideoID )
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

            if ( pTemp == NULL )
            {
                pTemp = new HVAPI_CALLBACK_SET();
                memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET) );
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
           HVAPI_CALLBACK_SET *pTemp = NULL;
            for (pTemp = pHHC->pCallBackSet; pTemp!=NULL; pTemp=pTemp->pNext)
            {
                    if (pTemp->iVideoID == iVideoID )
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

            if ( pTemp == NULL )
            {
                pTemp = new HVAPI_CALLBACK_SET();
                memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET) );
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
            HVAPI_CALLBACK_SET *pTemp = NULL;
            for (pTemp = pHHC->pCallBackSet; pTemp!=NULL; pTemp=pTemp->pNext)
            {
                if (pTemp->iVideoID == iVideoID )
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

            if ( pTemp == NULL )
            {
                pTemp = new HVAPI_CALLBACK_SET();
                memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET) );
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
            HVAPI_CALLBACK_SET *pTemp = NULL;
            for (pTemp = pHHC->pCallBackSet; pTemp!=NULL; pTemp=pTemp->pNext)
            {
                if (pTemp->iVideoID == iVideoID )
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

            if ( pTemp == NULL )
            {
                pTemp = new HVAPI_CALLBACK_SET();
                memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET) );
                pTemp->iVideoID = iVideoID;
                pTemp->pOnRecordEnd = (HVAPI_CALLBACK_RECORD_INFOEND)pFunc;
                pTemp->pOnRecordEndParam = (PVOID)pUserData;
                pTemp->pNext = pHHC->pCallBackSet;
                pHHC->pCallBackSet = pTemp;
            }
        }
        break ;
        case CALLBACK_TYPE_STRING:
        {
            HVAPI_CALLBACK_SET *pTemp = NULL;
            for (pTemp = pHHC->pCallBackSet; pTemp!=NULL; pTemp=pTemp->pNext)
            {
                    if (pTemp->iVideoID == iVideoID )
                    {
                        pTemp->pOnString = (HVAPI_CALLBACK_STRING)pFunc;
                        pTemp->pOnStringParam = (PVOID)pUserData;
                        break;
                    }
                    else
                    {
                        pTemp->pOnString =NULL;
                        pTemp->pOnStringParam = NULL;
                        return S_OK;
                    }

            }

            if ( pTemp == NULL )
            {
                pTemp = new HVAPI_CALLBACK_SET();
                memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET) );
                pTemp->iVideoID = iVideoID;
               pTemp->pOnString = (HVAPI_CALLBACK_STRING)pFunc;
                pTemp->pOnStringParam = (PVOID)pUserData;
                pTemp->pNext = pHHC->pCallBackSet;
                pHHC->pCallBackSet = pTemp;
            }
        }
        break;
        default:
        return E_FAIL;
    }

    return S_OK;
}

HRESULT SetVideoCallBack(HVAPI_HANDLE_EX hHandle, PVOID pFunc, PVOID pUserData, INT iVideoID,INT iCallBackType, LPCSTR szConnCmd)
{
    if ( hHandle == NULL )
        return E_FAIL;
    HVAPI_HANDLE_CONTEXT_EX *pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
    if ( strcmp(pHHC->szVersion, HVAPI_API_VERSION_EX) != 0 || pHHC->dwOpenType != 1  )
        return E_FAIL;

     if ( pHHC->fIsConnectHistoryVideo)
            return E_FAIL;

    if ( pFunc == NULL )
    {
        pHHC->dwVideoConnStatus = CONN_STATUS_DISCONN;
        pHHC->fIsThreadRecvVideoExit = true;
        HvSafeCloseThread(pHHC->pthreadRecvVideo);
        pHHC->pthreadRecvVideo = 0;
        ForceCloseSocket(pHHC->sktVideo);
        pHHC->sktVideo = -1;

         HVAPI_CALLBACK_SET *pTemp = NULL;
         for ( pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp->pNext)
        {
             if ( pTemp->iVideoID == iVideoID )
              {
                  pTemp->pOnH264 =  NULL;
                  pTemp->pOnH264Param = NULL;
                  break;
            }
        }//FOR
        return S_OK;
    }

    //thread Exist;
    if(  (pHHC->dwVideoConnStatus != CONN_STATUS_UNKNOWN &&  pHHC->dwVideoConnStatus != CONN_STATUS_DISCONN && pHHC->dwVideoConnStatus != CONN_STATUS_RECVDONE) ||  !pHHC->fIsThreadRecvVideoExit )
    {
        HVAPI_CALLBACK_SET *pTemp = NULL;
        for ( pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp->pNext)
        {
            if ( pTemp->iVideoID == iVideoID )
            {
                pTemp->pOnH264 = (HVAPI_CALLBACK_H264)pFunc;
                pTemp->pOnH264Param = (PVOID)pUserData;
                 break;
            }
        }//FOR
        if ( pTemp == NULL )
        {
                pTemp = new HVAPI_CALLBACK_SET();
                memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
                pTemp->iVideoID = iVideoID;
                pTemp->pOnH264 = (HVAPI_CALLBACK_H264)pFunc;
                pTemp->pOnH264Param = (PVOID)pUserData;
                pTemp->pNext = pHHC->pCallBackSet;
                pHHC->pCallBackSet = pTemp;
        }
        return S_OK;
    }

    if (pHHC->fAutoLink)
    {
        return E_NOTIMPL;
    }

    if ( szConnCmd != NULL )
    {
        if ( strstr(szConnCmd, "DownloadVideo") && strstr(szConnCmd, "Enable[1]"))
        return E_FAIL;
    }//szConnCmd

    if (ConnectCamera(pHHC->szIP, CAMERA_VIDEO_LINK_PORT,  pHHC->sktVideo))
    {
        if ( szConnCmd != NULL )
        {
            strcpy(pHHC->szVideoConnCmd, szConnCmd);
            char szRetBuf[256]={0};
            if ( HvSendXmlCmd(pHHC->szIP, pHHC->szVideoConnCmd, szRetBuf, sizeof(szRetBuf), NULL, pHHC->sktVideo) != S_OK )
            {
                ForceCloseSocket(pHHC->sktVideo);
                pHHC->sktVideo = -1;
                return E_FAIL;
            }
            //XML fenxi

        }

         HVAPI_CALLBACK_SET *pTemp = NULL;
         for ( pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp->pNext)
         {
             if ( pTemp->iVideoID == iVideoID )
             {
                  pTemp->pOnH264 = (HVAPI_CALLBACK_H264)pFunc;
                  pTemp->pOnH264Param = (PVOID)pUserData;
                  break;
               }
         }//FOR
          if ( pTemp == NULL )
           {
                pTemp = new HVAPI_CALLBACK_SET();
                memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
                pTemp->iVideoID = iVideoID;
                pTemp->pOnH264 = (HVAPI_CALLBACK_H264)pFunc;
                pTemp->pOnH264Param = (PVOID)pUserData;
                pTemp->pNext = pHHC->pCallBackSet;
                pHHC->pCallBackSet = pTemp;
            }

                        //create thread;
            pHHC->fIsThreadRecvVideoExit = false;

            if (  pthread_create(&(pHHC->pthreadRecvVideo), NULL,  VideoRecvThreadFuncEx,  (void*)pHHC) )
            {
                pHHC->fIsThreadRecvVideoExit = true;
                pHHC->dwVideoConnStatus = CONN_STATUS_DISCONN;
                ForceCloseSocket(pHHC->sktVideo);
                pHHC->sktVideo = -1;

                 HVAPI_CALLBACK_SET *pTemp = NULL;
                 for ( pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp->pNext)
                {
                     if ( pTemp->iVideoID == iVideoID )
                      {
                          pTemp->pOnH264 =  NULL;
                          pTemp->pOnH264Param = NULL;
                          break;
                    }
                }//FOR
                return E_FAIL;
            }
            pHHC->dwVideoConnStatus = CONN_STATUS_NORMAL;
            return S_OK;
    }
    else
    {
        HVAPI_CALLBACK_SET *pTemp = NULL;
        for ( pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp->pNext)
        {
            if ( pTemp->iVideoID == iVideoID )
            {
                pTemp->pOnH264 =  NULL;
                pTemp->pOnH264Param = NULL;
                break;
            }
        }//FOR
    }
    return S_OK;
}

HRESULT SetHistroyVIdeoCallBack(HVAPI_HANDLE_EX hHandle, PVOID pFunc, PVOID pUserData, INT iVideoID,INT iCallBackType, LPCSTR szConnCmd)
{
    if ( hHandle == NULL )
        return E_FAIL;
    HVAPI_HANDLE_CONTEXT_EX *pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
    if ( strcmp(pHHC->szVersion, HVAPI_API_VERSION_EX) != 0 || pHHC->dwOpenType != 1  )
        return E_FAIL;

     if ( pHHC->fIsConnectHistoryVideo)
            return E_FAIL;

    if ( pFunc == NULL )
    {
        pHHC->dwVideoConnStatus = CONN_STATUS_DISCONN;
        pHHC->fIsThreadRecvVideoExit = true;
        HvSafeCloseThread(pHHC->pthreadRecvVideo);
        pHHC->pthreadRecvVideo = 0;
        ForceCloseSocket(pHHC->sktVideo);
        pHHC->sktVideo = -1;

         HVAPI_CALLBACK_SET *pTemp = NULL;
         for ( pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp->pNext)
        {
             if ( pTemp->iVideoID == iVideoID )
              {
                  pTemp->pOnHistoryVideo =  NULL;
                  pTemp->pOnHistoryVideoParam = NULL;
                  break;
            }
        }//FOR
        return S_OK;
    }

     if (pHHC->fAutoLink)
    {
        return E_NOTIMPL;
    }


    if ( szConnCmd == NULL )
        return E_FAIL;

    if ( szConnCmd != NULL )
    {
        if (! strstr(szConnCmd, "DownloadVideo") || ! strstr(szConnCmd, "Enable[1]"))
        return E_FAIL;
    }//szConnCmd


    //thread Exist;
    if(  (pHHC->dwVideoConnStatus != CONN_STATUS_UNKNOWN &&  pHHC->dwVideoConnStatus != CONN_STATUS_DISCONN && pHHC->dwVideoConnStatus != CONN_STATUS_RECVDONE) ||  !pHHC->fIsThreadRecvVideoExit )
    {
        if ( pHHC->fIsConnectHistoryVideo )
        {
            HVAPI_CALLBACK_SET *pTemp = NULL;
            for ( pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp->pNext)
            {
                if ( pTemp->iVideoID == iVideoID )
                {
                    pTemp->pOnHistoryVideo = (HVAPI_CALLBACK_HISTORY_VIDEO)pFunc;
                    pTemp->pOnHistoryVideoParam = (PVOID)pUserData;
                     break;
                }
            }//FOR
            if ( pTemp == NULL )
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
             HVAPI_CALLBACK_SET *pTemp = NULL;
            for ( pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp->pNext)
            {
                if ( pTemp->iVideoID == iVideoID )
                {
                    pTemp->pOnHistoryVideo = (HVAPI_CALLBACK_HISTORY_VIDEO)pFunc;
                    pTemp->pOnHistoryVideoParam = (PVOID)pUserData;
                     break;
                }
            }//FOR
            return E_FAIL;
        }
        return S_OK;
    }

    if (ConnectCamera(pHHC->szIP, CAMERA_VIDEO_LINK_PORT,  pHHC->sktVideo))
    {
        strcpy(pHHC->szVideoConnCmd, szConnCmd);
        char szRetBuf[256]={0};
        if ( HvSendXmlCmd(pHHC->szIP, pHHC->szVideoConnCmd, szRetBuf, sizeof(szRetBuf), NULL, pHHC->sktVideo) != S_OK )
        {
            ForceCloseSocket(pHHC->sktVideo);
            pHHC->sktVideo = -1;
            return E_FAIL;
        }
        //XML fenxi

        char *pTempStr = strstr(pHHC->szVideoConnCmd, "BeginTime");
        if (pTempStr)
        {
            memcpy(pHHC->szVideoBeginTimeStr, pTempStr+10, 19);
            pHHC->szVideoBeginTimeStr[19]='\0';
        }
        pTempStr = strstr(pHHC->szVideoConnCmd, "EndTime");
        if (pTempStr)
        {
            if (pTempStr[9] == ']')
            {
                memset(pHHC->szVideoEndTimeStr, 0, 14);
                pHHC->szVideoEndTimeStr[0] = '\0';
            }
            else
            {
                memcpy(pHHC->szVideoEndTimeStr, pTempStr+8, 19);
                pHHC->szVideoEndTimeStr[19] = '\0';
            }
        }

         HVAPI_CALLBACK_SET *pTemp = NULL;
         for ( pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp->pNext)
         {
             if ( pTemp->iVideoID == iVideoID )
             {
                 pTemp->pOnHistoryVideo = (HVAPI_CALLBACK_HISTORY_VIDEO)pFunc;
                  pTemp->pOnHistoryVideoParam = (PVOID)pUserData;
                  break;
               }
         }//FOR
          if ( pTemp == NULL )
           {
                pTemp = new HVAPI_CALLBACK_SET();
                memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
                pTemp->iVideoID = iVideoID;
                pTemp->pOnHistoryVideo = (HVAPI_CALLBACK_HISTORY_VIDEO)pFunc;
                pTemp->pOnHistoryVideoParam = (PVOID)pUserData;
                pTemp->pNext = pHHC->pCallBackSet;
                pHHC->pCallBackSet = pTemp;
            }

                        //create thread;
            pHHC->fIsThreadRecvVideoExit = false;
            pHHC->fIsConnectHistoryVideo = true;

            if (  pthread_create(&(pHHC->pthreadRecvVideo), NULL,  VideoRecvThreadFuncEx,  (void*)pHHC) )
            {
                pHHC->fIsThreadRecvVideoExit = true;
                pHHC->fIsConnectHistoryVideo = false;
                pHHC->dwVideoConnStatus = CONN_STATUS_DISCONN;
                ForceCloseSocket(pHHC->sktVideo);
                pHHC->sktVideo = -1;

                 HVAPI_CALLBACK_SET *pTemp = NULL;
                 for ( pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp->pNext)
                {
                     if ( pTemp->iVideoID == iVideoID )
                      {
                          pTemp->pOnHistoryVideo =NULL;
                        pTemp->pOnHistoryVideoParam = NULL;
                          break;
                    }
                }//FOR
                return E_FAIL;
            }
            pHHC->dwVideoConnStatus = CONN_STATUS_NORMAL;
            return S_OK;
    }
    else
    {
        pHHC->fIsConnectHistoryVideo = false;
        HVAPI_CALLBACK_SET *pTemp = NULL;
        for ( pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp->pNext)
        {
            if ( pTemp->iVideoID == iVideoID )
            {
                pTemp->pOnHistoryVideo = NULL;
                pTemp->pOnHistoryVideoParam = NULL;
                break;
            }
        }//FOR
    }
    return S_OK;
}

HRESULT SetJpegCallBack(HVAPI_HANDLE_EX hHandle, PVOID pFunc, PVOID pUserData, INT iVideoID,INT iCallBackType, LPCSTR szConnCmd)
{
     if ( hHandle == NULL )
        return E_FAIL;
    HVAPI_HANDLE_CONTEXT_EX *pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
    if ( strcmp(pHHC->szVersion, HVAPI_API_VERSION_EX) != 0 || pHHC->dwOpenType != 1  )
        return E_FAIL;

    if ( pFunc == NULL )
    {
        pHHC->dwImageConnStatus = CONN_STATUS_DISCONN;
        pHHC->fIsThreadRecvImageExit = true;
        HvSafeCloseThread(pHHC->pthreadRecvImage);
        pHHC->pthreadRecvImage = 0;
        ForceCloseSocket(pHHC->sktImage);
        pHHC->sktVideo = -1;

         HVAPI_CALLBACK_SET *pTemp = NULL;
         for ( pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp->pNext)
        {
             if ( pTemp->iVideoID == iVideoID )
              {
                  pTemp->pOnJpegFrame =  NULL;
                  pTemp->pOnJpegFrameParam = NULL;
                  break ;
            }
        }//FOR
        return S_OK;
    }

    if (pHHC->fAutoLink)
    {
        return E_NOTIMPL;
    }

    //i
    if ( pHHC->dwImageConnStatus == CONN_STATUS_UNKNOWN && pHHC->dwImageConnStatus == CONN_STATUS_DISCONN )
    {
        HVAPI_CALLBACK_SET *pTemp = NULL;
        for ( pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
        {
            if ( pTemp->iVideoID == iVideoID )
            {
                pTemp->pOnJpegFrame  = (HVAPI_CALLBACK_JPEG)pFunc;
               pTemp->pOnJpegFrameParam = (PVOID)pUserData;
                 break;
            }
        }//FOR
        if ( pTemp == NULL )
        {
                pTemp = new HVAPI_CALLBACK_SET();
                memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
                pTemp->iVideoID = iVideoID;
                pTemp->pOnJpegFrame  = (HVAPI_CALLBACK_JPEG)pFunc;
               pTemp->pOnJpegFrameParam = (PVOID)pUserData;
                pTemp->pNext = pHHC->pCallBackSet;
                pHHC->pCallBackSet = pTemp;
        }
    }

    if ( !ConnectCamera(pHHC->szIP, CAMERA_IMAGE_LINK_PORT,  pHHC->sktImage) )
    {
        HVAPI_CALLBACK_SET *pTemp = NULL;
        for ( pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
        {
            if ( pTemp->iVideoID == iVideoID )
            {
                pTemp->pOnJpegFrame  = NULL;
                pTemp->pOnJpegFrameParam = NULL;
                break;
            }
        }//FOR
        return E_FAIL;
    }

     if ( szConnCmd != NULL )
    {
        strcpy(pHHC->szImageConnCmd, szConnCmd);
        char szRetBuf[256]={0};
        if ( HvSendXmlCmd(pHHC->szIP, pHHC->szImageConnCmd, szRetBuf, sizeof(szRetBuf), NULL, pHHC->sktImage) != S_OK )
        {
            ForceCloseSocket(pHHC->sktImage);
            pHHC->sktVideo = -1;
            return E_FAIL;
        }
            //XML fenxi

    }

    HVAPI_CALLBACK_SET *pTemp = NULL;
    for ( pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
    {
        if ( pTemp->iVideoID == iVideoID )
        {
            pTemp->pOnJpegFrame  = (HVAPI_CALLBACK_JPEG)pFunc;
            pTemp->pOnJpegFrameParam = (PVOID)pUserData;
            break;
        }
    }//FOR
    if ( pTemp == NULL )
    {
            pTemp = new HVAPI_CALLBACK_SET();
            memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
            pTemp->iVideoID = iVideoID;
            pTemp->pOnJpegFrame  = (HVAPI_CALLBACK_JPEG)pFunc;
            pTemp->pOnJpegFrameParam = (PVOID)pUserData;
            pTemp->pNext = pHHC->pCallBackSet;
            pHHC->pCallBackSet = pTemp;
    }

                        //create thread;
    pHHC->fIsThreadRecvImageExit = false;
    if (  pthread_create(&(pHHC->pthreadRecvImage), NULL,  ImageRecvThreadFuncEx,  (void*)pHHC) )
    {
        pHHC->dwImageConnStatus = CONN_STATUS_DISCONN;
        pHHC->fIsThreadRecvImageExit = true;
        HvSafeCloseThread(pHHC->pthreadRecvImage);
        pHHC->pthreadRecvImage = 0;
        ForceCloseSocket(pHHC->sktImage);
        pHHC->sktVideo = -1;

         HVAPI_CALLBACK_SET *pTemp = NULL;
         for ( pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp->pNext)
        {
             if ( pTemp->iVideoID == iVideoID )
              {
                  pTemp->pOnJpegFrame =  NULL;
                  pTemp->pOnJpegFrameParam = NULL;
                  break ;
            }
        }//FOR
        return E_FAIL;
    }
   pHHC->dwImageConnStatus = CONN_STATUS_NORMAL;
    return S_OK;
}

 HV_API_EX HRESULT  CDECL HVAPI_SetCallBackEx(HVAPI_HANDLE_EX hHandle, PVOID pFunc, PVOID pUserData, INT iVideoID,INT iCallBackType, LPCSTR szConnCmd)
{
    HRESULT hr = E_FAIL;

    switch (iCallBackType)
    {
        case CALLBACK_TYPE_H264_VIDEO:
        {
           hr = SetVideoCallBack(hHandle,  pFunc,  pUserData,  iVideoID, iCallBackType,  szConnCmd);
        }
            break;
        case CALLBACK_TYPE_RECORD_PLATE:
        {
           //

           hr = SetRecordCallBack(hHandle,  pFunc,  pUserData,  iVideoID, iCallBackType,  szConnCmd);
        }
            break ;
        case CALLBACK_TYPE_RECORD_BIGIMAGE:
        {
            hr = SetRecordCallBack(hHandle, pFunc, pUserData, iVideoID, iCallBackType, szConnCmd);
        }
            break;
        case CALLBACK_TYPE_RECORD_SMALLIMAGE:
        {
            hr = SetRecordCallBack(hHandle,  pFunc,  pUserData,  iVideoID, iCallBackType,  szConnCmd);
        }
        break;
        case CALLBACK_TYPE_RECORD_BINARYIMAGE:
        {
           hr = SetRecordCallBack(hHandle,  pFunc,  pUserData,  iVideoID, iCallBackType,  szConnCmd);
        }
        break;
        case CALLBACK_TYPE_RECORD_INFOBEGIN:
        {
           hr = SetRecordCallBack(hHandle,  pFunc,  pUserData,  iVideoID, iCallBackType,  szConnCmd);
        }
        break;
        case CALLBACK_TYPE_RECORD_INFOEND:
        {
            hr = SetRecordCallBack(hHandle,  pFunc,  pUserData,  iVideoID, iCallBackType,  szConnCmd);
        }
        break ;
        case CALLBACK_TYPE_STRING:
        {
           hr = SetRecordCallBack(hHandle,  pFunc,  pUserData,  iVideoID, iCallBackType,  szConnCmd);
        }
        break;
        case CALLBACK_TYPE_JPEG_FRAME:
        {
            hr = SetJpegCallBack(hHandle,  pFunc,  pUserData,  iVideoID, iCallBackType,  szConnCmd);
        }
        break;
        case CALLBACK_TYPE_HISTROY_VIDEO:
        {
            hr =  SetHistroyVIdeoCallBack(hHandle,  pFunc,  pUserData,  iVideoID, iCallBackType,  szConnCmd);

        }

        default:
            hr = E_FAIL;
        break;
    }

    return hr;
}

 HV_API_EX HRESULT  CDECL HVAPI_SetEnhanceRedLightFlagEx(HVAPI_HANDLE_EX hHandle,DWORD dwEnhanceStyle, INT iBrightness,INT iHubThreshold,INT iCompressRate)
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

 HV_API_EX HRESULT  CDECL HVAPI_SetHistoryVideoEnhanceRedLightFlagEx(HVAPI_HANDLE_EX hHandle, DWORD dwEnhanceStyle,INT iBrightness,INT iHubThreshold,INT iCompressRate)
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

typedef struct _tag_Face_Info
{
    int nFaceCount;
    RECT rcFacePos[20];
}FACE_INFO;

 HV_API_EX HRESULT  CDECL HVAPI_GetExtensionInfoEx(HVAPI_HANDLE_EX hHandle, DWORD dwType, LPVOID pRetData, INT* iBufLen)
{
    if (hHandle == NULL || pRetData==NULL  )
    return E_FAIL;

    HVAPI_HANDLE_CONTEXT_EX *pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
    int *pValude = NULL;
    FACE_INFO *pcFaceInfo = NULL;
    HRESULT hr = E_FAIL;

    switch(dwType)
    {
        case PLATE_RECT_BEST_SNAPSHOT:
        {
            if ( *iBufLen != 16 )
            break;
            pValude = (int*)pRetData;
            pValude[0] = pHHC->rcPlate[0].top;
            pValude[1] = pHHC->rcPlate[0].left;
            pValude[2] = pHHC->rcPlate[0].bottom;
            pValude[3] = pHHC->rcPlate[0].right;
            hr = S_OK;
        }
        break;
           case PLATE_RECT_LAST_SNAPSHOT:
           {
               if ( *iBufLen != 16 )
                break;
                pValude = (int*)pRetData;
                pValude[0] = pHHC->rcPlate[1].top;
                pValude[1] = pHHC->rcPlate[1].left;
                pValude[2] = pHHC->rcPlate[1].bottom;
                pValude[3] = pHHC->rcPlate[1].right;
                hr = S_OK;
           }
        break;
           case PLATE_RECT_BEGIN_CAPTURE:
           {
                if ( *iBufLen != 16 )
                break;
                pValude = (int*)pRetData;
                pValude[0] = pHHC->rcPlate[2].top;
                pValude[1] = pHHC->rcPlate[2].left;
                pValude[2] = pHHC->rcPlate[2].bottom;
                pValude[3] = pHHC->rcPlate[2].right;
                hr = S_OK;
           }
        break;
           case PLATE_RECT_BEST_CAPTURE:
           {
                if ( *iBufLen != 16 )
                break;
                pValude = (int*)pRetData;
                pValude[0] = pHHC->rcPlate[3].top;
                pValude[1] = pHHC->rcPlate[3].left;
                pValude[2] = pHHC->rcPlate[3].bottom;
                pValude[3] = pHHC->rcPlate[3].right;
                hr = S_OK;
           }

        break;
           case PLATE_RECT_LAST_CAPTURE:
           {
               if (*iBufLen != 16 )
                break;
                pValude = (int*)pRetData;
                pValude[0] = pHHC->rcPlate[4].top;
                pValude[1] = pHHC->rcPlate[4].left;
                pValude[2] = pHHC->rcPlate[4].bottom;
                pValude[3] = pHHC->rcPlate[4].right;
                hr = S_OK;
           }
        break;
        case FACE_RECT_BEST_SNAPSHOT :
        {
            if ( *iBufLen !=  sizeof(FACE_INFO) )
                    break;
            pcFaceInfo = (FACE_INFO*)pRetData;
            pcFaceInfo->nFaceCount = pHHC->nFaceCount[0];
            memcpy(pcFaceInfo->rcFacePos, pHHC->rcFacePos[0], sizeof(pcFaceInfo->rcFacePos));
            hr = S_OK;
        }
        break;
        case  FACE_RECT_LAST_SNAPSHOT:
        {
            if ( *iBufLen !=  sizeof(FACE_INFO) )
                    break;
            pcFaceInfo = (FACE_INFO*)pRetData;
            pcFaceInfo->nFaceCount = pHHC->nFaceCount[1];
            memcpy(pcFaceInfo->rcFacePos, pHHC->rcFacePos[1], sizeof(pcFaceInfo->rcFacePos));
            hr = S_OK;
        }
        break;
        case  FACE_RECT_BEGIN_CAPTURE:
        {
             if ( *iBufLen !=  sizeof(FACE_INFO) )
                    break;
            pcFaceInfo = (FACE_INFO*)pRetData;
            pcFaceInfo->nFaceCount = pHHC->nFaceCount[2];
            memcpy(pcFaceInfo->rcFacePos, pHHC->rcFacePos[2], sizeof(pcFaceInfo->rcFacePos));
            hr = S_OK;
        }
        break;
        case FACE_RECT_BEST_CAPTURE:
        {
            if ( *iBufLen !=  sizeof(FACE_INFO) )
                    break;
            pcFaceInfo = (FACE_INFO*)pRetData;
            pcFaceInfo->nFaceCount = pHHC->nFaceCount[3];
            memcpy(pcFaceInfo->rcFacePos, pHHC->rcFacePos[3], sizeof(pcFaceInfo->rcFacePos));
            hr = S_OK;
        }
        break;
        case FACE_RECT_LAST_CAPTURE:
        {
             if ( *iBufLen !=  sizeof(FACE_INFO) )
                    break;
            pcFaceInfo = (FACE_INFO*)pRetData;
            pcFaceInfo->nFaceCount = pHHC->nFaceCount[4];
            memcpy(pcFaceInfo->rcFacePos, pHHC->rcFacePos[4], sizeof(pcFaceInfo->rcFacePos));
            hr = S_OK;
        }
        break;

        default:
        break;
    }
    return 0;
}

static unsigned long GetTickCount()
{
    unsigned long currentTime;
    struct timeval current;
    gettimeofday(&current, NULL);
    currentTime = current.tv_sec * 1000 + current.tv_usec/1000;
    return currentTime;
}

static void* StatusMonitorThreadFuncEx(LPVOID lpParam)
{
    if ( lpParam == NULL )
        return NULL;
    HVAPI_HANDLE_CONTEXT_EX *pHHC = (HVAPI_HANDLE_CONTEXT_EX*)lpParam;
    BOOL fIsNeedSleep = TRUE;

    while (!pHHC->fStatusMonotorExit)
    {
        fIsNeedSleep = TRUE;
        if (pHHC->dwRecordConnStatus == CONN_STATUS_RECONN)
        {
            fIsNeedSleep = FALSE;
            if ( ConnectCamera(pHHC->szIP, CAMERA_RECORD_LINK_PORT, pHHC->sktRecord) )
            {
                if  ( strlen( (const char*)pHHC->szRecordConnCmd) > 0 )
                {

                    if ( pHHC->fIsRecvHistoryRecord )
                    {
                        if ( pHHC->iRecordDataInfo == -1 )
                        {
                            sprintf((char*)pHHC->szRecordConnCmd, "DownloadRecord,BeginTime[%s],Index[%d],Enable[1],EndTime[%s]",
                            pHHC->szRecordBeginTimeStr, pHHC->dwRecordStartIndex, pHHC->szRecordEndTimeStr);
                        }
                        else
                        {
                             sprintf((char*)pHHC->szRecordConnCmd, "DownloadRecord,BeginTime[%s],Index[%d],Enable[1],EndTime[%s],DataInfo[%d]",
                            pHHC->szRecordBeginTimeStr, pHHC->dwRecordStartIndex, pHHC->szRecordEndTimeStr, pHHC->iRecordDataInfo);
                        }
                    }

                    char szRetBuf[256] = {0};
                    int hsocketcmd = -1;
                    if(ConnectCamera(pHHC->szIP, CAMERA_CMD_LINK_PORT, hsocketcmd)) // IsNewProtocol()
                    {
                        ForceCloseSocket(hsocketcmd);
                        if ( HvSendXmlCmd(pHHC->szIP, (char*)pHHC->szRecordConnCmd,
                        szRetBuf, sizeof(szRetBuf),NULL, pHHC->sktRecord) == S_OK )
                        {
                            pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
                            pHHC->dwRecordStreamTick = GetTickCount();
                            pHHC->dwRecordReconnectTimes++;
                        }
                    }
                    else
                    {
                        usleep(10000);
                    }
                }
                else
                {
                    pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
                    pHHC->dwRecordStreamTick = GetTickCount();
                    pHHC->dwRecordReconnectTimes++;
                }
            }
        }

        if (pHHC->dwImageConnStatus == CONN_STATUS_RECONN)
        {
            fIsNeedSleep = FALSE;
            if ( ConnectCamera(pHHC->szIP, CAMERA_IMAGE_LINK_PORT,  pHHC->sktImage) )
            {

                if  ( strlen( (const char*)pHHC->szImageConnCmd) > 0 )
                {
                    char szRetBuf[256] = {0};
                    int hsocketcmd = -1;
                    if(ConnectCamera(pHHC->szIP, CAMERA_CMD_LINK_PORT, hsocketcmd)) // IsNewProtocol()
                    {
                        ForceCloseSocket(hsocketcmd);
                        if ( HvSendXmlCmd(pHHC->szIP, (char*)pHHC->szImageConnCmd,
                        szRetBuf, sizeof(szRetBuf),NULL, pHHC->sktImage) == S_OK )
                        {
                            pHHC->dwVideoConnStatus = CONN_STATUS_NORMAL;
                            pHHC->dwVideoStreamTick = GetTickCount();
                            pHHC->dwVideoReconnectTimes++;
                        }
                    }
                    else
                    {
                        usleep(10000);
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

        if (pHHC->dwVideoConnStatus == CONN_STATUS_RECONN)
        {
            fIsNeedSleep = FALSE;
            if (ConnectCamera(pHHC->szIP, CAMERA_VIDEO_LINK_PORT,  pHHC->sktVideo))
            {
                if  ( strlen( (const char*)pHHC->szVideoConnCmd) > 0 )
                {
                    if ( pHHC->fIsConnectHistoryVideo )
                    {
                        sprintf((char*)pHHC->szVideoConnCmd, "DownloadVideo,BeginTime[%s],EndTime[%s],Enable[1]",
                            pHHC->szVideoBeginTimeStr,  pHHC->szVideoEndTimeStr);
                    }

                    char szRetBuf[256] = {0};
                    int hsocketcmd = -1;
                    if(ConnectCamera(pHHC->szIP, CAMERA_CMD_LINK_PORT, hsocketcmd)) // IsNewProtocol()
                    {
                        ForceCloseSocket(hsocketcmd);
                        if ( HvSendXmlCmd(pHHC->szIP, (char*)pHHC->szVideoConnCmd,
                        szRetBuf, sizeof(szRetBuf),NULL, pHHC->sktVideo) == S_OK )
                        {
                             pHHC->dwVideoConnStatus = CONN_STATUS_NORMAL;
                            pHHC->dwVideoStreamTick = GetTickCount();
                            pHHC->dwVideoReconnectTimes++;
                        }
                    }
                    else
                    {
                        usleep(10000);
                    }
                }
                else
                {
                     pHHC->dwVideoConnStatus = CONN_STATUS_NORMAL;
                    pHHC->dwVideoStreamTick = GetTickCount();
                    pHHC->dwVideoReconnectTimes++;
                }
            }// if connect

        }

        if ( fIsNeedSleep)
            usleep(1000000);
    }

     pthread_exit(NULL);
}


void DealOldVersionRecord(HVAPI_HANDLE_CONTEXT_EX* pHHC, unsigned char* pRecordInfo, int nRecordInfoLen,  unsigned char* pRecordData, int nRecordDataLen)
{
    BLOCK_HEADER cBlockHeader;
	DWORD32 dwRecordType = 0;
    DWORD dwAppendInfoBufLen = 1024;
    dwAppendInfoBufLen = (dwAppendInfoBufLen << 5);
	char *szAppendInfo = new char[dwAppendInfoBufLen];
	memset(szAppendInfo, 0, dwAppendInfoBufLen);
	unsigned char* pbTemp = pRecordInfo;
    int iFlag = 0;

    int ipbInfoSize = nRecordInfoLen + 1;

	for ( int i=0; i<(int)nRecordInfoLen; )
	{
		if ( (ipbInfoSize - i) < sizeof(BLOCK_HEADER) )
        {
        	iFlag = 1;
            break ;
        }

		memcpy(&cBlockHeader, pbTemp, sizeof(BLOCK_HEADER));
		pbTemp+=sizeof(BLOCK_HEADER);
		i+=sizeof(BLOCK_HEADER);

		if ( cBlockHeader.dwID == BLOCK_RECORD_TYPE)
		{
			if ( cBlockHeader.dwLen == sizeof(dwRecordType) )
			{

				if ( (ipbInfoSize-1) < (int)cBlockHeader.dwLen )
				{
					iFlag = 1;
					break;
                 }

				memcpy(&dwRecordType, pbTemp,  cBlockHeader.dwLen);
			}
         }
         else if ( cBlockHeader.dwID == BLOCK_XML_TYPE )
         {

		 	if (cBlockHeader.dwLen <= dwAppendInfoBufLen )
			{

				if ( (ipbInfoSize-1) < (int)cBlockHeader.dwLen )
                {
                	iFlag = 1;
                    break;
                }
                memcpy(szAppendInfo, pbTemp,  cBlockHeader.dwLen);
             }
          }
           pbTemp+=cBlockHeader.dwLen;
           i+=cBlockHeader.dwLen;
		}//for

	if ( iFlag == 1  )
    {
            usleep(100000);
            pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
            return ;
    }

     DWORD dwType = RECORD_TYPE_UNKNOWN;
     if (dwRecordType == CAMERA_RECORD_NORMAL )
     {
            dwType = RECORD_TYPE_NORMAL;
     }
     else if(dwRecordType == CAMERA_RECORD_HISTORY )
     {
             dwType = RECORD_TYPE_HISTORY;
     }

     if ( dwRecordType == CAMERA_RECORD_NORMAL || dwRecordType == CAMERA_RECORD_HISTORY )
     {
         ProcRecordDataPacket(pHHC, pRecordData, nRecordDataLen, dwType, szAppendInfo);
     }
     SAFE_DELETE_ARG(szAppendInfo);
	return ;
}




bool GetNodePointerByName(TiXmlElement* pRootEle, const std::string &strNodeName, TiXmlElement* &Node)
{
     // 假如等于根节点名，就退出
     if (strNodeName==pRootEle->Value())
     {
         Node = pRootEle;
         return true;
     }
      TiXmlElement* pEle = pRootEle;
      for (pEle = pRootEle->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())
    {
          //递归处理子节点
          if(GetNodePointerByName(pEle,strNodeName,Node))
              return true;
     }
     return false;
}


int ModifyNodeInfo(TiXmlDocument &doc, std::string szNodeName, std::string szNameValue)
{
	TiXmlElement *pRootEle = doc.RootElement();
	if (pRootEle == NULL )
		return 0;

	TiXmlElement *pNode = NULL;
	GetNodePointerByName(pRootEle, szNodeName, pNode);
	if ( pNode != NULL )
	{
		pNode->Clear();  // 首先清除所有文本
        // 然后插入文本，保存文件
		TiXmlText *pValue = new TiXmlText(szNameValue.c_str());
        pNode->LinkEndChild(pValue);
		return 0;
	}
	else
		return 1;

	return 0;
}

bool AddNode_Attribute(TiXmlDocument &doc,const std::string& strParNodeName,const std::string strNodeName,const std::map<std::string,std::string> &AttMap)
{
    // 定义一个TiXmlDocument类指针
    TiXmlElement *pRootEle = doc.RootElement();
    if (NULL==pRootEle)
        return false;

    TiXmlElement *pNode = NULL;
    GetNodePointerByName(pRootEle,strParNodeName,pNode);

    if (NULL!=pNode)
    {
        // 生成子节点：pNewNode
		/*
		TiXmlElement *pNewNode = new TiXmlElement(strNodeName.c_str());

        if (NULL==pNewNode)
        {
            return false;
        }
		*/

		TiXmlElement xmlNewNode(strNodeName.c_str());
        // 设置节点的属性值，然后插入节点
        //std::map<std::string,std::string>::const_iterator iter;
        std::map<std::string,std::string>::const_reverse_iterator iter;
        for (iter=AttMap.rbegin();iter!=AttMap.rend();iter++)
        {
			//pNewNode->SetAttribute(iter->first.c_str(),iter->second.c_str());
			xmlNewNode.SetAttribute(iter->first.c_str(),iter->second.c_str());
        }

        //pNode->InsertEndChild(*pNewNode);
		pNode->InsertEndChild(xmlNewNode);
        return true;
    }
    else
        return false;
}



void DealNewVersionRecord(HVAPI_HANDLE_CONTEXT_EX* pHHC, unsigned char* pRecordInfo, int nRecordInfoLen,  unsigned char* pRecordData, int nRecordDataLen)
{
	// 从info中解出TransmittingType
	TiXmlDocument cXmlDoc;
	DWORD32 dwRecordType = 0;
	if ( cXmlDoc.Parse((const char*)pRecordInfo) )
	{
		TiXmlElement* pEleRoot = cXmlDoc.RootElement();
		if (NULL == pEleRoot)
		{
			//WrightLogEx(pHHC->szIP, "RecordRecvThreadFuncEx pEleRoot == NULL");
			return ;
		}

		TiXmlElement* pExtInfoEle = pEleRoot->FirstChildElement("ResultExtInfo");
		if (NULL == pExtInfoEle)
		{
			//WrightLogEx(pHHC->szIP, "RecordRecvThreadFuncEx pExtInfoEle == NULL");
			return ;
		}

		TiXmlElement* pTypeEle = pExtInfoEle->FirstChildElement("TransmittingType");
		if (NULL == pTypeEle)
		{
			//WrightLogEx(pHHC->szIP, "RecordRecvThreadFuncEx pTypeEle == NULL");
			return ;
		}

		const char* szType = pTypeEle->GetText();
		if (szType)
		{

			if (0 == strcmp("Realtime", szType))
			{
				dwRecordType = CAMERA_RECORD_NORMAL;
			}
			else if (0 == strcmp("History", szType))
			{
				dwRecordType = CAMERA_RECORD_HISTORY;
			}
		}
		else
		{
			//WrightLogEx(pHHC->szIP, "RecordRecvThreadFuncEx pbInfo contains no TransmittingType");
			return ;
		}
	}
	else
	{
		//WrightLogEx(pHHC->szIP, "RecordRecvThreadFuncEx pbInfo can't be parsed");
		return ;
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
	if(dwRecordType == CAMERA_RECORD_NORMAL || dwRecordType == CAMERA_RECORD_HISTORY)
	{
		ProcRecordDataPacket(pHHC, pRecordData, nRecordDataLen, dwType, (const char*)pRecordInfo);
	}
	return ;
}


HRESULT GetCameraString( const char* pXml ,char* pbString , int* piSting )
{
	if( NULL == pXml || NULL == pbString || NULL == piSting )
	{
		return E_POINTER;
	}

	TiXmlDocument cXmlDoc;
	if (cXmlDoc.Parse( pXml ))
	{
		TiXmlElement* pRootElement = cXmlDoc.RootElement();
		if (NULL == pRootElement)
		{
			//WrightLogEx("GetCameraString Fail1", pXml );
			return E_FAIL;
		}
		const char* pText = pRootElement->GetText();
		if ( pText )
		{
			int iStringLen = strlen( pText );
			if( iStringLen < *piSting )
			{
				//WrightLogEx("GetCameraString OK", pXml );
				strcpy( pbString , pRootElement->GetText() );
				*piSting = iStringLen;
				return S_OK;
			}
		}

	}
	return E_FAIL;
}






const DWORD32 MAX_BUFF_LEN = 10*1024 *1024;
void * RecordRecvThreadFuncEx(LPVOID lpParam)
{
    if ( lpParam == NULL )
        return NULL;
    HVAPI_HANDLE_CONTEXT_EX *pHHC = (HVAPI_HANDLE_CONTEXT_EX*)lpParam;

    INFO_HEADER cInfoHeader;
    INFO_HEADER cInfoHeaderResponse;
    BLOCK_HEADER cBlockHeader;
    unsigned char* pbInfo = NULL;
    unsigned char* pbData = NULL;

    cInfoHeaderResponse.dwType = CAMERA_THROB_RESPONSE;
    cInfoHeaderResponse.dwInfoLen = 0;
    cInfoHeaderResponse.dwDataLen = 0;

    pHHC->dwRecordStreamTick = GetTickCount();
    int iHearLen = sizeof(cInfoHeader);

    while (!pHHC->fIsThreadRecvRecordExit)
    {
        if ( pHHC->dwRecordConnStatus == CONN_STATUS_NORMAL )
        {
            if ( GetTickCount() - pHHC->dwRecordStreamTick > 8000 )
            {
                pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
            }
        }//if

        if ( pHHC->dwRecordConnStatus == CONN_STATUS_RECONN
        || pHHC->dwRecordConnStatus == CONN_STATUS_DISCONN )
        {
            if ( pHHC->fAutoLink )
            {
                ForceCloseSocket(pHHC->sktRecord);
            }
            usleep(1000000);
            continue ;
        }// if
        if ( RecvAll(pHHC->sktRecord, (char*)&cInfoHeader, iHearLen ) != iHearLen )
        {
            pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
            usleep(100000);
            continue ;
        }

        if ( cInfoHeader.dwInfoLen > MAX_BUFF_LEN || cInfoHeader.dwDataLen > MAX_BUFF_LEN )
        {
            if ( pHHC->fAutoLink )
            {
                pHHC->fVailPackResumeCache = FALSE;
                ForceCloseSocket(pHHC->sktRecord);
                pHHC->fIsThreadRecvRecordExit = TRUE;
                pHHC->pthreadRecvRecord = 0;
            }
            pHHC->dwRecordConnStatus = CONN_STATUS_RECVDONE;
            memset(pHHC->szRecordConnCmd, 0,sizeof(pHHC->szRecordConnCmd));

            break;
        }

        SAFE_DELETE_ARG(pbInfo);
        SAFE_DELETE_ARG(pbData);
        pHHC->dwRecordStreamTick = GetTickCount();

        if ( cInfoHeader.dwType == CAMERA_THROB )
        {
            if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
            {
                ;
            }
            else
            {
                if ( send(pHHC->sktRecord, (char*)&cInfoHeaderResponse, sizeof(cInfoHeaderResponse), 0 ) !=sizeof(cInfoHeaderResponse) )
                {
                    pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
                }
            }

            continue ;
        }

        if ( cInfoHeader.dwType == CAMERA_HISTORY_END )
        {
            if ( pHHC->fAutoLink )
            {
                pHHC->fVailPackResumeCache = FALSE;
                ForceCloseSocket(pHHC->sktRecord);
                pHHC->fIsThreadRecvRecordExit = TRUE;
                pHHC->pthreadRecvRecord = 0;
            }
            pHHC->dwRecordConnStatus =  CONN_STATUS_RECVDONE;
            memset(pHHC->szRecordConnCmd, 0, sizeof(pHHC->szRecordConnCmd));
            break ;
        }
        pbInfo = new unsigned char[cInfoHeader.dwInfoLen + 1];
        pbData = new unsigned char[cInfoHeader.dwDataLen + 1];
        int ipbInfoSize = (int)cInfoHeader.dwInfoLen +1;

        if ( pbInfo == NULL || pbData == NULL )
        {
            usleep(100000);
            pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
            continue ;
        }

        memset(pbInfo, 0, cInfoHeader.dwInfoLen+1);
        memset(pbData, 0, cInfoHeader.dwDataLen+1);

        if ( cInfoHeader.dwInfoLen > 0 )
        {
            int nInfoLen = 0;
            int nRet = RecvAll(pHHC->sktRecord, (char*)pbInfo, cInfoHeader.dwInfoLen, nInfoLen);
            bool fRecvFail = cInfoHeader.dwInfoLen != nInfoLen;
            if ( pHHC->fAutoLink && pHHC->fVailPackResumeCache && cInfoHeader.dwType == CAMERA_RECORD)
            {
                UpdatePackCache(pHHC, PACK_TYPE_INFO, (char*)pbInfo, nInfoLen);
            }

            if ( fRecvFail )
            {
                usleep(100000);
                pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
                continue ;
            }
        }//if

        if ( cInfoHeader.dwDataLen > 0 )
        {
            int nDataLen = 0;
            int nRet = RecvAll(pHHC->sktRecord, (char*)pbData, cInfoHeader.dwDataLen, nDataLen);
            bool fRecvFail = cInfoHeader.dwDataLen != nRet;

            if ( pHHC->fAutoLink && pHHC->fVailPackResumeCache && cInfoHeader.dwType == CAMERA_RECORD )
            {
                if ( NULL != pHHC->pPackResumeCache )
                    pHHC->pPackResumeCache->nDataOffset = nDataLen ;

                bool fIsEmptyDataPackCache = true;
                if ( S_OK == IsEmptyPackCache( pHHC, PACK_TYPE_DATA, fIsEmptyDataPackCache ) )
                {
                    if ( fRecvFail || !fIsEmptyDataPackCache )
                        UpdatePackCache(pHHC, PACK_TYPE_DATA, (char*)pbData, nDataLen);
                }
            }

            if ( fRecvFail )
            {
                usleep(100000);
                pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
                continue ;
            }
        }//if

        if  ( cInfoHeader.dwType == CAMERA_STRING  )
        {
            HVAPI_CALLBACK_SET *pTemp = NULL;
            for ( pTemp= pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext )
            {
                if ( pTemp->pOnString ==  NULL )
						continue;


				if ( PROTOCOL_VERSION_1 == pHHC->emProtocolVersion || PROTOCOL_VERSION_2 == pHHC->emProtocolVersion  )
				{
					pTemp->pOnString(pTemp->pOnStringParam, (LPCSTR)pbData, cInfoHeader.dwDataLen );
				}
				else
				{
					char szStrBuf[512] = { 0 };
					int iStrBufLen = 512;

					GetCameraString( (const char*)pbData , szStrBuf , &iStrBufLen );

					pTemp->pOnString(pTemp->pOnStringParam, (LPCSTR)szStrBuf, iStrBufLen);
				}



            }
            continue ;
        }
		else if (cInfoHeader.dwType == CAMERA_TFD_STRING )
		{
			continue;
		}
		else if ( cInfoHeader.dwType != CAMERA_RECORD )
		{
			continue;
		}


        unsigned char* pRecordInfo = pbInfo;
        unsigned char* pRecordData = pbData;
        int nRecordInfoLen = cInfoHeader.dwInfoLen;
        int nRecordDataLen = cInfoHeader.dwDataLen;

        if ( pHHC->fAutoLink && pHHC->fVailPackResumeCache && cInfoHeader.dwType == CAMERA_RECORD )
        {
            bool fEmptyCacheInfoPack = true;
            bool fEmptyCacheDataPack = true;
            if ( S_OK == IsEmptyPackCache(pHHC, PACK_TYPE_INFO, fEmptyCacheInfoPack) )
            {
                if ( !fEmptyCacheInfoPack )
                    GetPackCache(pHHC, PACK_TYPE_INFO, &pRecordInfo, nRecordInfoLen);
            }

            if ( S_OK == IsEmptyPackCache(pHHC, PACK_TYPE_DATA, fEmptyCacheDataPack) )
            {
                if ( !fEmptyCacheInfoPack )
                    GetPackCache(pHHC, PACK_TYPE_DATA, &pRecordData, nRecordDataLen);
            }
        }

        if  ( cInfoHeader.dwType == CAMERA_RECORD )
        {
            if ( PROTOCOL_VERSION_1 == pHHC->emProtocolVersion || PROTOCOL_VERSION_2 == pHHC->emProtocolVersion )
            {
            	DealOldVersionRecord(pHHC, pRecordInfo,nRecordInfoLen,pRecordData, nRecordDataLen);
            }
			else
			{
				DealNewVersionRecord(pHHC, pRecordInfo,nRecordInfoLen,pRecordData, nRecordDataLen);
			}

        }
    }


    SAFE_DELETE_ARG(pbInfo);
    SAFE_DELETE_ARG(pbData);
     pthread_exit(NULL);
    return NULL;
}

HRESULT ProcLprImageFrame(HVAPI_HANDLE_CONTEXT_EX* pHHC, PBYTE pbImgData, DWORD dwImgDataLen, DWORD dwImgDataOffSet, char *pszImageExtInfo)
{
     HVAPI_CALLBACK_SET *pTemp = NULL;
    for ( pTemp =pHHC->pCallBackSet; pTemp != NULL; pTemp=pTemp->pNext )
    {
        if ( pTemp->pOnJpegFrame )
        {
                pTemp->pOnJpegFrame(pTemp->pOnJpegFrameParam, pbImgData+dwImgDataOffSet,
                dwImgDataLen-dwImgDataOffSet, IMAGE_TYPE_JPEG_LPR, pszImageExtInfo);
        }
    }// for

    return S_OK;
}

static void *ImageRecvThreadFuncEx(LPVOID lpParam)
{
    if ( lpParam == NULL )
        return NULL;
    HVAPI_HANDLE_CONTEXT_EX *pHHC = (HVAPI_HANDLE_CONTEXT_EX*)lpParam;

    INFO_HEADER cInfoHeader;
    INFO_HEADER cInfoHeaderResponse;
    BLOCK_HEADER cBlockHeader;
    unsigned char* pbInfo = NULL;
    unsigned char* pbData = NULL;

    cInfoHeaderResponse.dwType = CAMERA_THROB_RESPONSE;
    cInfoHeaderResponse.dwInfoLen = 0;
    cInfoHeaderResponse.dwDataLen = 0;

    pHHC->dwImageStreamTick = GetTickCount();
    int iHeadLen = sizeof(cInfoHeader);


    while (!pHHC->fIsThreadRecvImageExit)
    {
       if ( pHHC->dwImageConnStatus == CONN_STATUS_NORMAL )
       {
           if ( GetTickCount() - pHHC->dwImageStreamTick > 8000 )
                pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
       }// if

       if ( pHHC->dwImageConnStatus == CONN_STATUS_RECONN
       || pHHC->dwImageConnStatus == CONN_STATUS_DISCONN )
       {
           usleep(1000000);
           continue ;
       }// if

      if (  RecvAll(pHHC->sktImage, (char*)&cInfoHeader, iHeadLen) != iHeadLen  )
      {
            pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
            usleep(1000000);
           continue ;
      }

      //
      SAFE_DELETE_ARG(pbInfo);
      SAFE_DELETE_ARG(pbData);

      if ( cInfoHeader.dwType == CAMERA_THROB )
      {
          if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
            continue;

          if ( send(pHHC->sktImage, (char*)&cInfoHeaderResponse, sizeof(cInfoHeaderResponse),0 ) != sizeof(cInfoHeaderResponse) )
          {
              pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
          }
          continue ;
      }

      if ( cInfoHeader.dwType == CAMERA_HISTORY_END )
      {
          pHHC->dwImageConnStatus == CONN_STATUS_RECVDONE;
          memset( pHHC->szImageConnCmd, 0, sizeof(pHHC->szImageConnCmd));
          break ;
      }// if

      pbInfo = new unsigned char[cInfoHeader.dwInfoLen+1];
      pbData = new unsigned char[cInfoHeader.dwDataLen+1];
      int ipbInfoLSize = (int)cInfoHeader.dwInfoLen + 1;

      if ( pbInfo == NULL || pbData == NULL )
      {
          usleep(100000);
          pHHC->dwImageConnStatus == CONN_TYPE_RECORD;
          continue ;
      }
      memset(pbInfo, 0, cInfoHeader.dwInfoLen+1);
      memset(pbData, 0, cInfoHeader.dwDataLen+1);

      //RECV
      if ( cInfoHeader.dwInfoLen > 0 )
      {
          if ( RecvAll(pHHC->sktImage, (char*)pbInfo, cInfoHeader.dwInfoLen ) != cInfoHeader.dwInfoLen )
          {
              usleep(100000);
              pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
              continue ;
          }
      }//if cInfoHeader.dwInfoLen

      if ( cInfoHeader.dwDataLen > 0 )
      {
          if ( RecvAll(pHHC->sktImage, (char*)pbData, cInfoHeader.dwDataLen ) != cInfoHeader.dwDataLen )
          {
              usleep(100000);
              pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
              continue ;
          }
      } // if

      if ( cInfoHeader.dwType == CAMERA_IMAGE )
      {
          DWORD dwImageType = 0;
          DWORD dwImageWidth = 0;
          DWORD dwImageHeight = 0;
          DWORD64 dw64ImageTime = 0;
          DWORD32 dwImageOffset = 0;
          DWORD32 dwDebugInfoLen = 0;
          ImageExtInfo cImageExtInfo = {0};
          ImageExtInfo cImageMERCURYExtInfo = {0};  //水星图片附件信息
          unsigned char*pbTemp = pbInfo;

          char* pszMercuryExtInfo = new char[128*1024];
          memset(pszMercuryExtInfo, 0, 128*1024);
          int iFlag = 0;

          if (PROTOCOL_VERSION_1 !=  pHHC->emProtocolVersion
						&& PROTOCOL_VERSION_2 !=  pHHC->emProtocolVersion)
          {// 水星协议用XML传输info
              DWORD32 dwXmlLen = strlen((const char*)pbInfo);
              unsigned char* pbXml = new unsigned char[dwXmlLen];
              if (pbXml)
                memcpy(pbXml, pbInfo, dwXmlLen);
              else
                continue;
              TiXmlDocument cXmlDoc;
              if (!cXmlDoc.Parse((const char*)pbXml))
              {
                SAFE_DELETE_ARG(pbXml);
                continue;
              }
              else
              {
                SAFE_DELETE_ARG(pbXml);
				const TiXmlElement* pRootElement = cXmlDoc.RootElement();
				if (NULL == pRootElement)
				{
				    continue;
                }
				const TiXmlElement* pImageElement = pRootElement->FirstChildElement("Image");
				if (NULL == pImageElement)
					continue;
				const char* szType = pImageElement->Attribute("Type");
				const char* szWidth = pImageElement->Attribute("Width");
				const char* szHeight = pImageElement->Attribute("Height");
				const char* szTime = pImageElement->Attribute("Time");
				const char* szTimeHigh = pImageElement->Attribute("TimeHigh");
				const char* szTimeLow = pImageElement->Attribute("TimeLow");
				if (NULL == szType ||NULL == szWidth ||NULL == szHeight)
                    continue;
				const char* szShutter = pImageElement->Attribute("Shutter");
				const char* szGain = pImageElement->Attribute("Gain");
				const char* szRGain = pImageElement->Attribute("r_Gain");
                const char* szGGain = pImageElement->Attribute("g_Gain");
				const char* szBGain = pImageElement->Attribute("b_Gain");

				cImageMERCURYExtInfo.iShutter = atoi(szShutter?szShutter:"0");
				cImageMERCURYExtInfo.iGain = atoi(szGain?szGain:"0");
				cImageMERCURYExtInfo.iGainR = atoi(szRGain?szRGain:"0");
				cImageMERCURYExtInfo.iGainG = atoi(szGGain?szGGain:"0");
				cImageMERCURYExtInfo.iGainB = atoi(szBGain?szBGain:"0");

                            // 行人卡口信息参数
				const TiXmlElement* pDebugElement = pImageElement->FirstChildElement("DebugInfo");
				if (pDebugElement)
				{
                    const char* szDebugInfoSize = pDebugElement->Attribute("DebugInfoSize");
					dwDebugInfoLen = atoi(szDebugInfoSize?szDebugInfoSize:"0");
                }


                char* pMercuryExtInfo = pszMercuryExtInfo+4; // 增加4个空字节作为标记位?为什么？
                const TiXmlElement* pRectInfoElement = pImageElement->FirstChildElement("RectInfo");
				if( NULL != pRectInfoElement )
				{
                    const char* szRectCount = pRectInfoElement->Attribute("Count");
                    DWORD32 dwRectCount = atoi(szRectCount?szRectCount:"0");
                    if( dwRectCount > 0 )
                    {
									memcpy( pMercuryExtInfo ,&dwRectCount , sizeof(DWORD32));
									pMercuryExtInfo += sizeof(DWORD32);

                                    RECT cRectTemp;
                                    char szRectName[16];
                                    for (int i = 0; i < dwRectCount; i++)
                                    {
                                        sprintf(szRectName, "Rect%d", i);
                                        const TiXmlElement* pRectElement = pRectInfoElement->FirstChildElement(szRectName);
                                        if (NULL == pRectElement)
                                        {
                                            //char szLog[256];
                                            //sprintf(szLog, "Jpeg_Mercury pRectElement[%d] == NULL", i);
                                            //WrightLogEx("test IP", szLog);
                                            break;
                                        }

                                        const char* pBottom = pRectElement->Attribute("Bottom");
                                        const char* pLeft = pRectElement->Attribute("Left");
                                        const char* pRight = pRectElement->Attribute("Right");
                                        const char* pTop = pRectElement->Attribute("Top");

                                        cRectTemp.bottom = atoi(pBottom?pBottom:"0");
                                        cRectTemp.left = atoi(pLeft?pLeft:"0");
                                        cRectTemp.right = atoi(pRight?pRight:"0");
                                        cRectTemp.top = atoi(pTop?pTop:"0");

                                        memcpy(pMercuryExtInfo ,&cRectTemp ,sizeof(RECT) );
                                        pMercuryExtInfo+= sizeof(RECT);
                                    }
                                }
                    else if (0 == dwRectCount && dwDebugInfoLen > 0) // 行人卡口信息参数
                    {
                        dwRectCount = 12345; // 设置行人卡口标志
                        memcpy( pMercuryExtInfo ,&dwRectCount , sizeof(DWORD32));
                        pMercuryExtInfo += sizeof(DWORD32);
                        memcpy( pMercuryExtInfo ,&dwDebugInfoLen , sizeof(DWORD32));
                        pMercuryExtInfo += sizeof(DWORD32);
                        memcpy( pMercuryExtInfo, pbInfo+dwXmlLen+1, dwDebugInfoLen);
                    }
                }

                if (strcmp(szType, "JPEG") == 0)
                {
								dwImageType = CAMERA_IMAGE_JPEG;
                }
                else if (strcmp(szType, "JPEG_CAPTURE") == 0) // TODO: 与设备约定好
                {
                        dwImageType = CAMERA_IMAGE_JPEG_CAPTURE;
                }
                else if (strcmp(szType, "JPEG_SLAVE") == 0)
                {
                        dwImageType = CAMERA_IMAGE_JPEG_SLAVE;
                }
                else
                {
                        continue;
                }

                dwImageWidth = atoi(szWidth);
                if (0 == dwImageWidth)
                {
                    continue;
                }

                dwImageHeight = atoi(szHeight);
                if (0 == dwImageHeight)
                {
                    continue;
                }

                DWORD dwTimeHigh = atoi(szTimeHigh ? szTimeHigh:"0");
                DWORD dwTimeLow = atoi(szTimeLow ? szTimeLow:"0");
                dw64ImageTime = (((DWORD64)dwTimeHigh)<<32) | dwTimeLow;
              }

          }
          else
          {
              for (int i=0; i<(int)cInfoHeader.dwInfoLen; )
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
          }

          if (iFlag)
          {
             usleep(100000);
			 pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
			 continue;
          }


          char *pszImageExtInfo = new char[128*1024];
          if ( pszImageExtInfo )
          {
              if ( PROTOCOL_VERSION_1 != pHHC->emProtocolVersion
								&& PROTOCOL_VERSION_2 != pHHC->emProtocolVersion)
            {
					sprintf(pszImageExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d, FrameTime:%llu",
                cImageMERCURYExtInfo.iShutter, cImageMERCURYExtInfo.iGain, cImageMERCURYExtInfo.iGainR,
									cImageMERCURYExtInfo.iGainG, cImageMERCURYExtInfo.iGainB, dw64ImageTime);
             }

              else{
                sprintf(pszImageExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d, FrameTime:%llu",
                cImageExtInfo.iShutter, cImageExtInfo.iGain, cImageExtInfo.iGainR, cImageExtInfo.iGainG,
                cImageExtInfo.iGainB, dw64ImageTime);
                }
            }

            DWORD dwType = VIDEO_TYPE_UNKNOWN;
            if ( CAMERA_IMAGE_JPEG == dwImageType )
            {
                dwType = IMAGE_TYPE_JPEG_NORMAL;
            }
            else if (CAMERA_IMAGE_JPEG_CAPTURE == dwImageType )
            {
                dwType = IMAGE_TYPE_JPEG_CAPTURE;
            }
            else if ( CAMERA_IMAGE_JPEG_SLAVE == dwImageType )
            {
                ProcLprImageFrame(pHHC, pbData, cInfoHeader.dwDataLen, dwImageOffset , pszImageExtInfo);
                SAFE_DELETE_ARG(pszImageExtInfo);
                SAFE_DELETE_ARG(pszMercuryExtInfo);
                continue ;
            }

            HVAPI_CALLBACK_SET *pTemp = NULL;
            for ( pTemp =pHHC->pCallBackSet; pTemp != NULL; pTemp=pTemp->pNext )
            {
                if ( pTemp->pOnJpegFrame )
                {
                    pTemp->pOnJpegFrame(pTemp->pOnJpegFrameParam, pbData+dwImageOffset,
                    cInfoHeader.dwDataLen-dwImageOffset, dwType, pszImageExtInfo);
                }
            }// for
            SAFE_DELETE_ARG(pszImageExtInfo);
            SAFE_DELETE_ARG(pszMercuryExtInfo);
      }// if

    }//while

    SAFE_DELETE_ARG(pbInfo);
    SAFE_DELETE_ARG(pbData);
     pthread_exit(NULL);
    return NULL;
}

static void * VideoRecvThreadFuncEx(LPVOID lpParam)
{
    if ( lpParam == NULL )
        return NULL;
    HVAPI_HANDLE_CONTEXT_EX *pHHC = (HVAPI_HANDLE_CONTEXT_EX*)lpParam;

    INFO_HEADER cInfoHeader;
    INFO_HEADER cInfoHeaderResponse;
    BLOCK_HEADER cBlockHeader;
    unsigned char* pbInfo = NULL;
    unsigned char* pbData = NULL;

    cInfoHeaderResponse.dwType = CAMERA_THROB_RESPONSE;
    cInfoHeaderResponse.dwInfoLen = 0;
    cInfoHeaderResponse.dwDataLen = 0;

    pHHC->dwVideoStreamTick = GetTickCount();
    int iHearLen = sizeof(cInfoHeader);
    while (!pHHC->fIsThreadRecvVideoExit)
    {
        if ( pHHC->dwVideoConnStatus  == CONN_STATUS_NORMAL )
        {
            if ( GetTickCount() - pHHC->dwVideoStreamTick > 8000 )
            {
                pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
            }
        }

        if ( pHHC->dwVideoConnStatus == CONN_STATUS_RECONN
        || pHHC->dwVideoConnStatus == CONN_STATUS_DISCONN )
        {
            usleep(1000000);
            continue ;
        }


        if ( RecvAll(pHHC->sktVideo,  (char*)&cInfoHeader,  iHearLen ) != iHearLen )
        {
            pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
            usleep(1000000);
            continue ;
        }

        SAFE_DELETE_ARG(pbInfo);
        SAFE_DELETE_ARG(pbData);

        pHHC->dwVideoStreamTick = GetTickCount();
        if ( cInfoHeader.dwType == CAMERA_THROB )
        {
            if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
            continue;

            if ( send(pHHC->sktVideo,  (char*)&cInfoHeaderResponse, sizeof(cInfoHeaderResponse), 0 )  != sizeof(cInfoHeaderResponse) )
            {
                pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
            }
            continue ;
        }

         if ( cInfoHeader.dwType == CAMERA_HISTORY_END  )
        {
            pHHC->dwVideoConnStatus = CONN_STATUS_RECVDONE;
            pHHC->fIsConnectHistoryVideo = FALSE;
            memset(pHHC->szVideoConnCmd, 0, sizeof(pHHC->szVideoConnCmd));
            ForceCloseSocket(pHHC->sktVideo);
            break;
        }

        pbInfo = new unsigned char[cInfoHeader.dwInfoLen +1];
        pbData = new unsigned char[cInfoHeader.dwDataLen+1];
        int ipbInfoSize = cInfoHeader.dwInfoLen + 1;


        if ( pbInfo == NULL || pbData == NULL )
        {
            pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
            continue ;
        }

        memset(pbInfo, 0, cInfoHeader.dwInfoLen +1 );
        memset(pbData, 0, cInfoHeader.dwDataLen+1);

        if ( cInfoHeader.dwInfoLen > 0 )
        {
            if ( RecvAll(pHHC->sktVideo,  (char*)pbInfo,  cInfoHeader.dwInfoLen ) != cInfoHeader.dwInfoLen )
            {
                usleep(100000);
                pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
                continue ;
            }
        }

        if ( cInfoHeader.dwDataLen > 0 )
        {
            if ( RecvAll(pHHC->sktVideo,  (char*)pbData,  cInfoHeader.dwDataLen ) != cInfoHeader.dwDataLen )
            {
                usleep(100000);
                pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
                continue ;
            }
        }

        if ( cInfoHeader.dwType == CAMERA_VIDEO )
        {
            // 数据流附加信息解析
            DWORD32 dwVideoType = 0;
            DWORD32 dwFrameType = 0;
            DWORD64 dw64VideoTime = 0;
            VideoWidth cVideoWidth = {0};
            VideoHeight cVideoHeight = {0};
            VideoExtInfo cVideoExtInfo = {0};
            unsigned char *pbTemp = pbInfo;
            int iFlag = 0;

            for (int i=0; i<(int)cInfoHeader.dwInfoLen; )
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

            if (PROTOCOL_VERSION_1 != pHHC->emProtocolVersion &&
						PROTOCOL_VERSION_2 != pHHC->emProtocolVersion)
            {
                // 水星协议用XML传输info
                TiXmlDocument cXmlDoc;
                if (!cXmlDoc.Parse((const char*)pbInfo))
                {
                    continue;
                }
                else
                {
                    const TiXmlElement* pRootElement = cXmlDoc.RootElement();

                    if (NULL == pRootElement)
                    {
                        continue;
                    }
                    const TiXmlElement* pImageElement = pRootElement->FirstChildElement("Video");
                    if (NULL == pImageElement)
                    {
                        continue;
                    }

                    const char* szType = pImageElement->Attribute("Type");
                    const char* szFrameType = pImageElement->Attribute("FrameType");
                    const char* szWidth = pImageElement->Attribute("Width");
                    const char* szHeight = pImageElement->Attribute("Height");
                    const char* szTime = pImageElement->Attribute("Time");
                    const char* szTimeHigh = pImageElement->Attribute("TimeHigh");
                    const char* szTimeLow = pImageElement->Attribute("TimeLow");
                    if (NULL == szType ||
                        NULL == szFrameType ||
                        NULL == szWidth ||
                        NULL == szHeight
                        )
                    {
                        continue;
                    }

                    const char* szShutter = pImageElement->Attribute("Shutter");
                    const char* szGain = pImageElement->Attribute("Gain");
                    const char* szRGain = pImageElement->Attribute("r_Gain");
                    const char* szGGain = pImageElement->Attribute("g_Gain");
                    const char* szBGain = pImageElement->Attribute("b_Gain");

                    cVideoExtInfo.iShutter = atoi(szShutter?szShutter:"0");
                    cVideoExtInfo.iGain = atoi(szGain?szGain:"0");
                    cVideoExtInfo.iGainR = atoi(szRGain?szRGain:"0");
                    cVideoExtInfo.iGainG = atoi(szGGain?szGGain:"0");
                    cVideoExtInfo.iGainB = atoi(szBGain?szBGain:"0");

                    if(strcmp("H264",szType ) == 0)
                    {
                        dwVideoType = CAMERA_VIDEO_H264;
                    }
                    if(strcmp("IFrame",szFrameType ) == 0)
                    {
                        dwFrameType = CAMERA_FRAME_I;
                    }
                    else if(strcmp("PFrame",szFrameType ) == 0)
                    {
                        dwFrameType = CAMERA_FRAME_P;
                    }
                    else if(strcmp("IPFrame",szFrameType ) == 0)
                    {
                        dwFrameType = CAMERA_FRAME_IP_ONE_SECOND;
                    }

                    cVideoWidth.iWidth = atoi(szWidth ? szWidth:"0");
                    cVideoHeight.iHight = atoi(szHeight?szHeight:"0");
                                // dw64VideoTime = atoi(szTime ? szTime:"0");
                    DWORD dwTimeHigh = atoi(szTimeHigh ? szTimeHigh:"0");
                    DWORD dwTimeLow = atoi(szTimeLow ? szTimeLow:"0");
                    dw64VideoTime = (((DWORD64)dwTimeHigh)<<32) | dwTimeLow;
                }
            }

            if ( dwVideoType == CAMERA_VIDEO_H264 )
            {

                DWORD dwType = VIDEO_TYPE_UNKNOWN;
                 char *pszVideoExtInfo = new char[128];
                if ( pszVideoExtInfo )
                {
                    sprintf(pszVideoExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d, FrameTime:%llu,Width:%d,Height:%d",      //linux
                    cVideoExtInfo.iShutter, cVideoExtInfo.iGain, cVideoExtInfo.iGainR, cVideoExtInfo.iGainG,
                    cVideoExtInfo.iGainB, dw64VideoTime, cVideoWidth.iWidth, cVideoHeight.iHight);
                }

                if ( CAMERA_FRAME_I == dwFrameType )
                {
                    dwType = VIDEO_TYPE_H264_NORMAL_I;
                }
                else if (CAMERA_FRAME_P == dwFrameType )
                {
                    dwType = VIDEO_TYPE_H264_NORMAL_P;
                }
                else if ( CAMERA_FRAME_IP_ONE_SECOND == dwFrameType )
                {
                    PBYTE pPacketData = pbData;
                    int iTempBufferLen = cInfoHeader.dwDataLen;
                    int iFrameLen = 0;
                    int iFrameType = 0;
                    while ( iTempBufferLen > 0 )
                    {
                        iFrameType = *(int*)pPacketData;
                        pPacketData+=4;
                        iFrameLen = *(int*)pPacketData;
                        pPacketData+=4;
                        if ( iFrameType == 4096 )
                            dwType = VIDEO_TYPE_H264_HISTORY_I;
                        else if ( iFrameType == 4097 )
                            dwType = VIDEO_TYPE_H264_HISTORY_P;
                        if( dwType == VIDEO_TYPE_UNKNOWN || iFrameLen > cInfoHeader.dwDataLen)
                        {
                            break;
                        }

                        HVAPI_CALLBACK_SET *pTemp = NULL;
                        for (pTemp=pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
                        {
                            if ( pTemp->pOnHistoryVideo )
                                pTemp->pOnHistoryVideo(pTemp->pOnHistoryVideoParam, pPacketData, iFrameLen, dwType, pszVideoExtInfo );
                        }
                        pPacketData += iFrameLen;
                        iTempBufferLen = iTempBufferLen - 8 - iFrameLen;
                    }
                    SAFE_DELETE_ARG(pszVideoExtInfo);

                    time_t starttime = dw64VideoTime / 1000;
                    struct tm *pTM = localtime(&starttime);

                    sprintf(pHHC->szVideoBeginTimeStr, "%04d.%02d.%02d_%02d:%02d:%02d",  pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
                    pTM->tm_hour, pTM->tm_min, pTM->tm_sec);
                    pHHC->szVideoBeginTimeStr[19] = '\0';
                    continue ;
                }


                HVAPI_CALLBACK_SET *pTemp = NULL;
                for (pTemp=pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
                {
                    if ( pTemp->pOnH264 )
                        pTemp->pOnH264(pTemp->pOnH264Param, pbData, cInfoHeader.dwDataLen, dwType, pszVideoExtInfo );
                }
                SAFE_DELETE(pszVideoExtInfo);

            }
            else if ( dwVideoType == CAMERA_VIDEO_JPEG )
            {
                char *pszVideoExtInfo = new char[128];
                if ( pszVideoExtInfo )
                {
                    sprintf(pszVideoExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d, FrameTime:%llu,Width:%d,Height:%d",       // linux
                    cVideoExtInfo.iShutter, cVideoExtInfo.iGain, cVideoExtInfo.iGainR, cVideoExtInfo.iGainG,
                    cVideoExtInfo.iGainB, dw64VideoTime, cVideoWidth.iWidth, cVideoHeight.iHight);
                }

                PBYTE pPacketData = pbData;
                int iTempBufferLen = cInfoHeader.dwDataLen;
                int iFrameLen = 0;
                int iReadedCount = 0;
                int iSize = 0;
                int iRedLightCount = 0;
                char szTempInfo[10] = { 0 };
                while (iTempBufferLen >0)
                {
                    iSize = *(int*)pPacketData;
                    pPacketData+= 4;
                    memcpy(szTempInfo, pPacketData, 8 );
                    if ( strcmp( szTempInfo, "redlight") == 0 )
                    {
                        pPacketData += 8;
                        iRedLightCount = *(int*)pPacketData;
                        iFrameLen = 16 + iSize + sizeof(RECT) * iRedLightCount;
                        pPacketData = pPacketData + 4 + iSize + (sizeof(RECT) * iRedLightCount);
                    }
                    else
                    {
                        iFrameLen = 4 + iSize;
                        pPacketData += iSize;
                    }//if

                    //deal
                     ProcHistoryVideoJpegFrame(pHHC,  pbData+iReadedCount, iFrameLen, pszVideoExtInfo );
                     iReadedCount += iFrameLen;
                     iTempBufferLen -= iFrameLen;

                }//while

                SAFE_DELETE_ARG(pszVideoExtInfo);

            }
        }
    }

    SAFE_DELETE_ARG(pbInfo);
    SAFE_DELETE_ARG(pbData);
     pthread_exit(NULL);
    return NULL;
}

HRESULT ProcHistoryVideoJpegFrame(HVAPI_HANDLE_CONTEXT_EX* pHHC,  PBYTE pbFrameData, DWORD dwFrameDataLen, LPCSTR szVideoExtInfo )
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

typedef struct tagAppendInfo
{
    DWORD dwCarID;
    DWORD64 dwTimeHigh;
    DWORD64 dwTimeLow;
    char chPlateBuf[48];
    bool fIsPeccancy;
    tagAppendInfo()
    {
        dwCarID=0;
        fIsPeccancy = false;
        dwTimeHigh = 0;
        dwTimeLow = 0;
        memset(chPlateBuf, 0, 50 );
    }
}AppendInfo;

void ResolveXmlData(LPCSTR szAppendInfo,  AppendInfo *appendResult )
{
    if ( szAppendInfo == NULL || appendResult == NULL )
        return ;

    TiXmlDocument  myDocument;
    if ( ! myDocument.Parse(szAppendInfo) )
        return ;

    TiXmlElement *pRoot = myDocument.RootElement();
    if ( pRoot == NULL )
        return ;

    TiXmlElement *pResultSet = pRoot->FirstChildElement("ResultSet");
    if ( pResultSet == NULL )
        return ;

    const TiXmlElement * pResultElement = pResultSet->FirstChildElement("Result");
    if ( pResultElement == NULL )
        return ;

    const TiXmlElement* pEle = NULL;
    const char *chTemp = NULL;
    for ( pEle= pResultElement->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement() )
    {
        if ( strcmp(pEle->Value(), "PlateName") == 0 )
        {
            chTemp = pEle->GetText();
            if ( chTemp != NULL )
                strcpy(appendResult->chPlateBuf, chTemp);
        }
        else if ( strcmp(pEle->Value(), "EventCheck") == 0 )
        {
            chTemp = pEle->Attribute("value");
            if ( chTemp != NULL )
            {
                if ( strstr(chTemp, "违章:是"))
                    appendResult->fIsPeccancy = true;
            }
        }
        else if ( strcmp(pEle->Value(), "CarID") == 0 )
        {
            chTemp = pEle->Attribute("value");
            if ( chTemp != NULL )
                appendResult->dwCarID = (DWORD)atoi(chTemp);
        }
        else if ( strcmp(pEle->Value(), "TimeLow") == 0 )
        {
            chTemp = pEle->Attribute("value");
            if ( chTemp != NULL )
             {
                 sscanf(chTemp, "%u",  &(appendResult->dwTimeLow) );
             }
        }
        else if (strcmp(pEle->Value(), "TimeHigh") == 0 )
        {
            chTemp = pEle->Attribute("value");
            if ( chTemp != NULL )
            sscanf(chTemp, "%u",  &(appendResult->dwTimeHigh ) );
        }
    }
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

HRESULT ProcBigImageCallBack(HVAPI_CALLBACK_SET* pCallBackSet,DWORD dwType, RECORD_IMAGE_EX* pcImage, BOOL fIsPeccancy,
DWORD dwEnhanceFlag, INT iBrightness, INT iHueThreshold, INT iCompressRate, DWORD dwRecordType, DWORD64 dwTimeMS )
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

	//if(cImgInfo.nRedLightCount <= 0 || dwEnhanceFlag == 0)
	{
		pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
			(WORD)dwType, pcImage->cImgInfo.dwWidth,
			pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
			pcImage->dwImgDataLen, dwRecordType, dwTimeMS);

		return S_OK;
	}

    /*
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


	SAFE_DELETE(pTempPic);
	*/
	return S_OK;

}






HRESULT ProcRecordDataPacket(HVAPI_HANDLE_CONTEXT_EX* pHHC,  unsigned char* pbRecordData,  DWORD dwDataLen,  DWORD dwRecordType, LPCSTR szAppendInfo )
{
    if (dwRecordType != RECORD_TYPE_NORMAL && dwRecordType != RECORD_TYPE_HISTORY )
        return E_FAIL;

    if ( szAppendInfo == NULL )
        return E_FAIL;

    AppendInfo appendResult;
    ResolveXmlData(szAppendInfo,  &appendResult );


    RECORD_IMAGE_GROUP_EX cImgGroup;

    if ( PROTOCOL_VERSION_1 == pHHC->emProtocolVersion ||  PROTOCOL_VERSION_2 == pHHC->emProtocolVersion)
    {
          if ( HvGetRecordImage(pbRecordData, dwDataLen, &cImgGroup) != S_OK  )
                return E_FAIL;
    }
    else
    {
          if ( HvGetRecordImage_Mercury( szAppendInfo, pbRecordData, dwDataLen, &cImgGroup) != S_OK )
            return E_FAIL;
    }

    char  *pszInfo = NULL;

    if (pHHC->fEnableCarFaceCtrl)
    {
        if (pHHC->pCarFaceRecogCtrl == NULL )
        {
            CCarFaceCtrl *pCarFaceRecogCtrl = new  CCarFaceCtrl();

            if ( pCarFaceRecogCtrl->InitCtrl() == S_OK )
            {
                    pHHC->pCarFaceRecogCtrl = pCarFaceRecogCtrl;
            }
            else
            {
                    delete pCarFaceRecogCtrl;
                    pCarFaceRecogCtrl = NULL;
            }//if

        }


        //carFace
        if (pHHC->pCarFaceRecogCtrl != NULL)
        {
            RECORD_IMAGE_EX *pImageRecordEx = NULL;
			if ( cImgGroup.cLastSnapshot.pbImgData != NULL )
				pImageRecordEx = &(cImgGroup.cLastSnapshot);
			else if (cImgGroup.cBestSnapshot.pbImgData != NULL)
				pImageRecordEx = &(cImgGroup.cBestSnapshot);

            sv::CSvImage imageYuv;
            sv::CSvImage imgSamp;
            sv::SV_RESULT hr = sv::RS_S_FALSE;

            if ( pImageRecordEx != NULL  )
            {
                imgSamp.Create(sv::SV_IMAGE_RGB , pImageRecordEx->cImgInfo.dwWidth, pImageRecordEx->cImgInfo.dwHeight);
                if ( JPEG2RGBEx(pImageRecordEx->pbImgData, pImageRecordEx->dwImgDataLen, &imgSamp) == S_OK )
                {
                    imageYuv.Create(sv::SV_IMAGE_YUV422, imgSamp.m_nWidth, imgSamp.m_nHeight);
                    hr = imRGB2YCbCr(&imgSamp, &imageYuv);
                }
                imgSamp.Clear();

                //if ( hr != sv::RS_S_OK)
                 //   imageYuv.Clear();
            }




            //BYTE *pRGB = NULL;
            //int iRGBSize = 0;
           // if ( pImageRecordEx != NULL  && JPEG2RGB(pImageRecordEx->pbImgData, pImageRecordEx->dwImgDataLen, &pRGB, &iRGBSize) == S_OK)   // 有图片进行车脸识别
            if (hr == sv::RS_S_OK )
            {
                //sv::imRGB2YCbCr();

                sv::SV_RECT platePos = { 0 };
				sv::SV_BOOL fYellowPlate = false;
				sv::SV_RECT carFacePos = {0};
				sv::SV_UINT32 regID[1] ={0};

				//delete[] pRGB;
				//pRGB = NULL;


				platePos.m_nLeft = (pImageRecordEx->cImgInfo.rcPlate.left * pImageRecordEx->cImgInfo.dwWidth) / 100;
				platePos.m_nRight = (pImageRecordEx->cImgInfo.rcPlate.right * pImageRecordEx->cImgInfo.dwWidth) / 100;
				platePos.m_nTop = (pImageRecordEx->cImgInfo.rcPlate.top * pImageRecordEx->cImgInfo.dwHeight) / 100;
				platePos.m_nBottom = (pImageRecordEx->cImgInfo.rcPlate.bottom * pImageRecordEx->cImgInfo.dwHeight) / 100;


				char szLog[1024] = { 0 };
				sprintf(szLog, "[%d,%d,%d,%d]\n", platePos.m_nLeft, platePos.m_nRight, platePos.m_nTop, platePos.m_nBottom);
				WriteLog(szLog);

				char szCarFaceName[1024] = { 0 };
				char szCarType[1024] = { 0 };

				//  是否黄牌

                char szPlateColor[32] = { 0 };
                strncpy(szPlateColor, appendResult.chPlateBuf, 2);
				bool fYellow = FALSE;
				if ( strcmp(szPlateColor,"黄") == 0 )
                    fYellow = TRUE;

				//场景判b

				//是否无牌车
				char szPlate[32] = { 0 };
                strncpy(szPlate, appendResult.chPlateBuf, 8);
				bool fNoPlate = FALSE;
				if (  strstr(szPlate, "无车牌") != NULL )
					fNoPlate = TRUE;

					//车脸识别

				if ( pHHC->pCarFaceRecogCtrl->RecogCarFace(&imageYuv,FALSE,fNoPlate,&platePos,fYellow, &carFacePos,1, regID) == S_OK )
				{
					pHHC->pCarFaceRecogCtrl->FindCarFaceString(regID[0], szCarFaceName, szCarType);

					char szLog[1024] = {0};
					sprintf(szLog, "regID:%u  carFaceName:%s, carType:%s  pos:[%d,%d.%d,%d]\r\n", regID[0], szCarFaceName, szCarType,carFacePos.m_nLeft, carFacePos.m_nTop, carFacePos.m_nRight,carFacePos.m_nBottom);

					char chUTFLog[2048] = {0};
					GB2312ToUTF8(szLog, chUTFLog, 2048);
					WriteLog(chUTFLog);

					//printf("%s", chUTFLog);

					//构造附加信息，加入车脸信息
					TiXmlDocument doc;
					doc.Parse(szAppendInfo);

					std::map<std::string,std::string> AttMap;

					char szValue[256] = {0};
					sprintf(szValue, "%s",szCarFaceName);
					AttMap.clear();


                    std::string Name="chnname";
					 std::string value="车标信息";
					AttMap.insert( make_pair(Name, value));


					  Name="value";
					  value=szValue;
					AttMap.insert( make_pair(Name, value));
					//增加车辆类型
					AddNode_Attribute(doc,"Result","ModelsInfo",AttMap);

					//增加车型大小
					std::string szNameValue = szCarType;
					if ( ModifyNodeInfo(doc, "CarType", szNameValue ) != 0 ) //说明， 不存在着此节点， 需修改
					{
						AttMap.clear();

                        std::string Name="value";
						 std::string  value=szNameValue;
						AttMap.insert( make_pair(Name, value));

						 Name="chnname";
						value="车辆类型";
						AttMap.insert( make_pair(Name, value));

						AddNode_Attribute(doc,"Result","CarType",AttMap);



					}

					TiXmlPrinter cPrinter;
					doc.Accept(&cPrinter);

					int ilen = 1024 *16;
					pszInfo = new char[ilen];
					memset(pszInfo, 0, ilen);
					strcpy( pszInfo, cPrinter.CStr());
					WriteLog(pszInfo);
				}
				else
				{
					WriteLog("RecogCarFace failt");
				}

				imageYuv.Clear();



            }//pImageRecordEx != NULL
        }

    }



    DWORD dwCarID = appendResult.dwCarID;

    DWORD64 dw64TimeMs = ((DWORD64)appendResult.dwTimeHigh)<<32 | appendResult.dwTimeLow;

    HVAPI_CALLBACK_SET *pTemp = NULL;
    for ( pTemp= pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext )
    {
        if ( pTemp->pOnRecordBegin )
                pTemp->pOnRecordBegin(pTemp->pOnRecordBeginParam, appendResult.dwCarID);
    }


    //SMALL Image
    if ( cImgGroup.cPlatePicture.pbImgData )
    {
        for ( pTemp= pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext )
        {
            if ( pTemp->pOnSmallImage )
                pTemp->pOnSmallImage(pTemp->pOnSmallImageParam,  cImgGroup.cPlatePicture.cImgInfo.dwCarID,
                 cImgGroup.cPlatePicture.cImgInfo.dwWidth,  cImgGroup.cPlatePicture.cImgInfo.dwHeight,
                 cImgGroup.cPlatePicture.pbImgData, cImgGroup.cPlatePicture.dwImgDataLen, dwRecordType,  cImgGroup.cPlatePicture.cImgInfo.dw64TimeMs);
        }
    }


    //binalry
     if ( cImgGroup.cPlateBinary.pbImgData )
    {
        for ( pTemp= pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext )
        {
            if ( pTemp->pOnBinaryImage)
                pTemp->pOnBinaryImage(pTemp->pOnBinaryImageParam,  cImgGroup.cPlateBinary.cImgInfo.dwCarID,
                 cImgGroup.cPlateBinary.cImgInfo.dwWidth,  cImgGroup.cPlateBinary.cImgInfo.dwHeight,
                 cImgGroup.cPlateBinary.pbImgData, cImgGroup.cPlateBinary.dwImgDataLen, dwRecordType, cImgGroup.cPlateBinary.cImgInfo.dw64TimeMs);
        }//for
    }//

    //BIG IMAGE
    for ( pTemp=pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext )
    {
        if ( pTemp->iVideoID != 0 )
            continue ;
        if ( pTemp->pOnBigImage == NULL )
            continue ;

        if ( cImgGroup.cBestSnapshot.pbImgData )
        {
            pHHC->rcPlate[0] = cImgGroup.cBestSnapshot.cImgInfo.rcPlate;
            pHHC->nFaceCount[0]=cImgGroup.cBestSnapshot.cImgInfo.nFaceCount;
            memcpy(pHHC->rcFacePos[0], cImgGroup.cBestSnapshot.cImgInfo.rcFacePos, sizeof(pHHC->rcFacePos[0]));

            ProcBigImageCallBack(pTemp, RECORD_BIGIMG_BEST_SNAPSHOT, &cImgGroup.cBestSnapshot,  appendResult.fIsPeccancy,
                        pHHC->dwEnhanceRedLightFlag, pHHC->iBigPicBrightness, pHHC->iBigPicHueThrshold,
						pHHC->iBigPicCompressRate, dwRecordType, cImgGroup.cBestSnapshot.cImgInfo.dw64TimeMs);
        }

        if (cImgGroup.cLastSnapshot.pbImgData)
        {
            pHHC->rcPlate[1] = cImgGroup.cLastSnapshot.cImgInfo.rcPlate;
            pHHC->nFaceCount[1]=cImgGroup.cLastSnapshot.cImgInfo.nFaceCount;
            memcpy(pHHC->rcFacePos[1], cImgGroup.cLastSnapshot.cImgInfo.rcFacePos, sizeof(pHHC->rcFacePos[1]));

              ProcBigImageCallBack(pTemp, RECORD_BIGIMG_LAST_SNAPSHOT,  &cImgGroup.cLastSnapshot,  appendResult.fIsPeccancy,
                        pHHC->dwEnhanceRedLightFlag, pHHC->iBigPicBrightness, pHHC->iBigPicHueThrshold,
						pHHC->iBigPicCompressRate, dwRecordType, cImgGroup.cLastSnapshot.cImgInfo.dw64TimeMs);
        }

        if (cImgGroup.cBeginCapture.pbImgData )
        {
            pHHC->rcPlate[2] = cImgGroup.cBeginCapture.cImgInfo.rcPlate;
            pHHC->nFaceCount[2]=cImgGroup.cBeginCapture.cImgInfo.nFaceCount;
            memcpy(pHHC->rcFacePos[2], cImgGroup.cBeginCapture.cImgInfo.rcFacePos, sizeof(pHHC->rcFacePos[2]));

              ProcBigImageCallBack(pTemp, RECORD_BIGIMG_BEGIN_CAPTURE,  &cImgGroup.cBeginCapture,  appendResult.fIsPeccancy,
                        pHHC->dwEnhanceRedLightFlag, pHHC->iBigPicBrightness, pHHC->iBigPicHueThrshold,
						pHHC->iBigPicCompressRate, dwRecordType, cImgGroup.cBeginCapture.cImgInfo.dw64TimeMs);
        }

        if ( cImgGroup.cBestCapture.pbImgData )
        {
            pHHC->rcPlate[3] = cImgGroup.cBestCapture.cImgInfo.rcPlate;
            pHHC->nFaceCount[3]=cImgGroup.cBestCapture.cImgInfo.nFaceCount;
            memcpy(pHHC->rcFacePos[3], cImgGroup.cBestCapture.cImgInfo.rcFacePos, sizeof(pHHC->rcFacePos[3]));

              ProcBigImageCallBack(pTemp, RECORD_BIGIMG_BEST_CAPTURE,  &cImgGroup.cBestCapture,  appendResult.fIsPeccancy,
                        pHHC->dwEnhanceRedLightFlag, pHHC->iBigPicBrightness, pHHC->iBigPicHueThrshold,
						pHHC->iBigPicCompressRate, dwRecordType, cImgGroup.cBestCapture.cImgInfo.dw64TimeMs);
        }

         if ( cImgGroup.cLastCapture.pbImgData )
        {
              pHHC->rcPlate[4] = cImgGroup.cLastCapture.cImgInfo.rcPlate;
            pHHC->nFaceCount[4]=cImgGroup.cLastCapture.cImgInfo.nFaceCount;
            memcpy(pHHC->rcFacePos[4], cImgGroup.cLastCapture.cImgInfo.rcFacePos, sizeof(pHHC->rcFacePos[4]));

              ProcBigImageCallBack(pTemp, RECORD_BIGIMG_LAST_CAPTURE,  &cImgGroup.cLastCapture,  appendResult.fIsPeccancy,
                        pHHC->dwEnhanceRedLightFlag, pHHC->iBigPicBrightness, pHHC->iBigPicHueThrshold,
						pHHC->iBigPicCompressRate, dwRecordType, cImgGroup.cLastCapture.cImgInfo.dw64TimeMs);
        }
    }

     //char *desBuff = new char[48];
    // memset(desBuff, 0, 48);

     //GB2312ToUTF8(appendResult.chPlateBuf, desBuff, 48);
     //memcpy(appendResult.chPlateBuf, desBuff, 48);
     //delete []desBuff;

     for ( pTemp=pHHC->pCallBackSet; pTemp != NULL;  pTemp = pTemp->pNext )
     {
         if ( pTemp->pOnPlate )
         {
             if (pszInfo == NULL)
                pTemp->pOnPlate(pTemp->pOnPlateParam,dwCarID, appendResult.chPlateBuf, szAppendInfo, dwRecordType, dw64TimeMs);
             else
                pTemp->pOnPlate(pTemp->pOnPlateParam,dwCarID, appendResult.chPlateBuf, pszInfo, dwRecordType, dw64TimeMs);
         }
     }

     SAFE_DELETE_ARG(pszInfo);



     for ( pTemp=pHHC->pCallBackSet; pTemp != NULL;  pTemp = pTemp->pNext )
     {
         if ( pTemp->pOnRecordEnd)
         {
             pTemp->pOnRecordEnd(pTemp->pOnRecordEndParam, dwCarID);
         }
     }

     if ( pHHC->fIsRecvHistoryRecord )
     {
         time_t tt = ( dw64TimeMs / 1000 );
         struct tm *pTM = localtime(&tt);
         sprintf(pHHC->szRecordBeginTimeStr,  "%04d.%02d.%02d_%02d", pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
				pTM->tm_hour);
        pHHC->szRecordBeginTimeStr[13] = '\0';
        pHHC->dwRecordStartIndex = dwCarID + 1;
     }

    return S_OK;
}

 HV_API_EX HRESULT  CDECL HVAPIUTILS_GetRecordInfoFromAppendStringEx(LPCSTR szAppened, LPCSTR szInfoName,
LPSTR szRetInfo, int iRetInfoBufLen)
{
    if ( szAppened == NULL || szInfoName == NULL || szRetInfo == NULL || iRetInfoBufLen <=0 )
        return E_FAIL;

    TiXmlDocument cXmlDoc;
    if ( cXmlDoc.Parse(szAppened) == NULL )
        return E_FAIL;

    const TiXmlElement *pRootElement = cXmlDoc.RootElement();
    if ( pRootElement == NULL )
        return E_FAIL;
    const TiXmlElement *pElementResultSet = pRootElement->FirstChildElement("ResultSet");
    if ( pElementResultSet == NULL )
        return E_FAIL;
     const TiXmlElement *pElementResult = pElementResultSet->FirstChildElement("Result");
    if ( pElementResult == NULL )
        return E_FAIL;
    const TiXmlElement *pElementname = pElementResult->FirstChildElement(szInfoName);
    if ( pElementname == NULL )
        return E_FAIL;
    if ( strcmp(szInfoName, "PlateName") == 0 )
    {
        if ( iRetInfoBufLen >= (int)strlen(pElementname->GetText()))
        {
            strcpy(szRetInfo, pElementname->GetText());
            return S_OK;
        }
        else
        {
            strncpy(szRetInfo, pElementname->GetText(), iRetInfoBufLen);
            return S_OK;
        }
    }// IF

    const char* pszChnName = NULL;
    const char* pszValue = NULL;
    pszChnName = pElementname->Attribute("chnname");
    pszValue = pElementname->Attribute("value");
    if ( pszValue == NULL )
        return E_FAIL;
    if ( pszChnName == NULL )
    {
        if ( iRetInfoBufLen < (int)strlen(pszValue))
            return E_FAIL;

        strcpy(szRetInfo, pszValue);
        return S_OK;
    }

    if ( strstr(pszChnName, "事件检测"))
    {

        if ( iRetInfoBufLen < (int)strlen(pszValue))
            return E_FAIL;

        strcpy(szRetInfo, pszValue);
        return S_OK;
    }

    if ( iRetInfoBufLen < (int)(strlen(pszChnName) + strlen(pszValue) + 1) )
        return E_FAIL;
    strcpy(szRetInfo, pszChnName);
    strcat(szRetInfo, ":");
    strcat(szRetInfo, pszValue);
    return S_OK;
}
 HV_API_EX HRESULT  CDECL HVAPIUTILS_ParsePlateXmlStringEx(LPCSTR pszXmlPlateInfo, LPSTR pszPlateInfoBuf, int iPlateInfoBufLen )
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
				//	DWORD dwTimeHigh, dwTimeLow;   // DWORD linux 4字节
                    unsigned long dwTimeHigh, dwTimeLow;
					const TiXmlElement* pElementName = pElementResult->FirstChildElement("TimeHigh");
					{
						if(pElementName)
						{
				//			dwTimeHigh = atol(pElementName->Attribute("value"));
							dwTimeHigh = strtoul(pElementName->Attribute("value"), NULL, 0);
						}
					}

					pElementName = pElementResult->FirstChildElement("TimeLow");
					{
						if(pElementName)
						{
				//			dwTimeLow = atol(pElementName->Attribute("value"));
                            dwTimeLow = strtoul(pElementName->Attribute("value"),NULL, 0);
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
                        sprintf(szTmpTime, "%llu", dw64TimeMs);   //Linux
						strcat(pszPlateInfoBuf, szTmpTime);
					}

				}
			}
		}
	}

    //char *desBuff = new char[iPlateInfoBufLen];
   //memset(desBuff, 0, iPlateInfoBufLen);
   //GB2312ToUTF8(pszPlateInfoBuf, desBuff, (size_t)iPlateInfoBufLen);
    //memcpy(pszPlateInfoBuf, desBuff, iPlateInfoBufLen);
    //delete []desBuff;

	return S_OK;
}
 HV_API_EX HRESULT  CDECL HVAPIUTILS_SmallImageToBitmapEx(PBYTE pbSmallImageData, INT nSmallImageWidth,
 INT nSmallImageHeight, PBYTE pbBitmapData, INT* pnBitmapDataLen )
 {
     HRESULT hr = E_FAIL;

	int iBmpLen = *pnBitmapDataLen;
	hr = Yuv2BMP(pbBitmapData, *pnBitmapDataLen, &iBmpLen, pbSmallImageData, nSmallImageWidth, nSmallImageHeight);
	*pnBitmapDataLen = iBmpLen;

	return hr;
 }
 HV_API_EX HRESULT  CDECL HVAPIUTILS_BinImageToBitmapEx(PBYTE pbBinImageData, PBYTE pbBitmapData,
INT *pnBitmapDataLen)
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
 HV_API_EX HRESULT  CDECL HVAPIUTILS_GetExeCmdRetInfoEx(BOOL fIsNewXmlProtocol, LPCSTR pszRetXmlStr, LPCSTR pszCmdName,
LPCSTR pszInfoName, LPSTR pszInfoValue )
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

// 按视频链路触发抓拍信号
HV_API_EX HRESULT CDECL HVAPI_TriggerSignal(HVAPI_HANDLE_EX hHandle, INT iVideoID)
{
	if(hHandle == NULL )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0 || pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	CHAR szRet [1*1024]={0};
	INT nRetLen = 1*1024;
	CHAR szCmdName[32] = {"SendTriggerOut"};
	CHAR szValue[32] = { 0 };
	CHAR szCmd[32] = {0};

	if ( iVideoID == 0 )
	{
		strcpy(szCmd, "SendTriggerOut");
	}
	else
	{
		strcpy(szCmdName, "SendTriggerOutEx");
		sprintf(szCmd, "SendTriggerOutEx,VideoID[%d]", iVideoID);
	}

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , szCmd , szRet , nRetLen , &nRetLen ) )
	{
		//WrightLogEx(pHHC->szIP, "SendTriggerOut  HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	// 解析命令返回
	INT nRetCode = -1;

	CXmlParseInfo rgRetInfo [1];
	strcpy( rgRetInfo [0].szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , rgRetInfo, 1 ) )
	{
		//WrightLogEx(pHHC->szIP , "SendTriggerOut  HvXmlParse Fail\n");
		return E_FAIL ;
	}

	nRetCode = atoi (rgRetInfo[0]. szKeyValue);

	return nRetCode == 0 ?S_OK: E_FAIL;

}

// 上传黑白名单
HV_API_EX HRESULT CDECL HVAPI_InportNameList(HVAPI_HANDLE_EX hHandle, CHAR* szWhiteNameList, INT iWhiteListLen, CHAR* szBlackNameList, INT iBlackListLen)
{
	if(hHandle == NULL || (NULL == szWhiteNameList && NULL == szBlackNameList) )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0 || pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	if ( pHHC->emProtocolVersion == PROTOCOL_VERSION_1 || pHHC->emProtocolVersion == PROTOCOL_VERSION_2 || pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH )
	{
		return E_FAIL;
	}

	// 创建黑白XML 文档列表
	int iValueLen = iWhiteListLen + iBlackListLen + 10240;
	char *szValueBuf = new char[iValueLen];
	if ( szValueBuf==NULL )
		return E_FAIL;

	int  iXmlLen = iValueLen+20480;
	char *szXmlDocBuf = new char[iXmlLen];
	if ( szXmlDocBuf == NULL )
	{
		delete[] szValueBuf;
		return E_FAIL;
	}

	memset(szValueBuf, 0, iValueLen);
	memset(szXmlDocBuf, 0, iXmlLen);

	if ( szWhiteNameList != NULL  &&  szBlackNameList != NULL )
	{
		sprintf(szValueBuf, "WhiteNameList[%s],BlackNameList[%s]", szWhiteNameList, szBlackNameList);
	}
	else if ( szWhiteNameList != NULL  &&  szBlackNameList == NULL  )
	{
		sprintf(szValueBuf, "WhiteNameList[%s]", szWhiteNameList);
	}
	else if ( szWhiteNameList == NULL  &&  szBlackNameList != NULL)
	{
		sprintf(szValueBuf, "BlackNameList[%s]", szBlackNameList);
	}
	else
		return E_FAIL;



	if ( HvGetXmlForSetNameList( szValueBuf, iValueLen, szXmlDocBuf, iXmlLen) == 0 )
	{
		delete[] szValueBuf;
		szValueBuf = NULL;
		delete[] szXmlDocBuf;
		szXmlDocBuf = NULL;
		return E_FAIL;
	}

	//printf("%s", szXmlDocBuf);

	// 执行命令
	bool fRet = false;
	char szRetBuf[1024] = { 0 };
	int nRetBufLen = 1024;
	fRet = ExecXmlExtCmdMercury(pHHC->szIP, (char*)szXmlDocBuf, (char*)szRetBuf, nRetBufLen, INVALID_SOCKET);

	//printf("%s", szRetBuf);

	// 解析命令返回
	INT nRetCode = -1;

	CXmlParseInfo rgRetInfo [1];
	strcpy( rgRetInfo [0].szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( "SetNameList", szRetBuf, nRetBufLen, rgRetInfo, 1 ) )
	{
		//WrightLogEx(pHHC->szIP , "SetNameList  HvXmlParse Fail\n");
		return E_FAIL ;
	}

	nRetCode = atoi(rgRetInfo[0].szKeyValue);

	return nRetCode ==0 ?S_OK: E_FAIL;

}

HV_API_EX HRESULT CDECL HVAPI_GetNameListXml(HVAPI_HANDLE_EX hHandle, LPSTR szXmlParam, INT nBufLen, INT* pnRetLen)
{
	if ( NULL == hHandle || NULL == szXmlParam )
		return E_FAIL;

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if ( 0 != strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) || pHHC->dwOpenType != 1)
		return E_FAIL;

	const int iBufLen = (1024 << 10);
	char* rgchXmlParamBuf = new char[iBufLen];  // 1MB
	if(rgchXmlParamBuf == NULL)
		return E_FAIL;
	memset(rgchXmlParamBuf, 0, sizeof(rgchXmlParamBuf));

	const WORD wPort = CAMERA_CMD_LINK_PORT;
	SOCKET hSocketCmd;

	CAMERA_CMD_HEADER cCmdHeader;
	CAMERA_CMD_RESPOND cCmdRespond;


	if ( ConnectCamera(pHHC->szIP, wPort, hSocketCmd) )
	{
		cCmdHeader.dwID = CAMERA_GET_NAMELIST_CMD;
		cCmdHeader.dwInfoSize = 0;
		if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0) )
		{
			ForceCloseSocket(hSocketCmd);
			SAFE_DELETE(rgchXmlParamBuf);
			return E_FAIL;
		}

		if ( sizeof(cCmdRespond) == recv(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0) )
		{
			if ( CAMERA_GET_NAMELIST_CMD == cCmdRespond.dwID
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




// 下载黑白名单
HV_API_EX HRESULT CDECL HVAPI_GetNameList(HVAPI_HANDLE_EX hHandle, CHAR* szWhiteNameList, INT *iWhiteListLen, CHAR* szBlackNameList, INT *iBlackListLen)
{
	if(hHandle == NULL)
	{
		return E_FAIL;
	}



	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0 || pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	if ( pHHC->emProtocolVersion == PROTOCOL_VERSION_1 || pHHC->emProtocolVersion == PROTOCOL_VERSION_2 || pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH )
	{
		return E_FAIL;
	}

	//int iLen = 100 * 1024;
	int iLen = 1024 * 1024*2;

	char *szXml = new char[iLen];
	memset(szXml, 0, iLen);

	if ( HVAPI_GetNameListXml(hHandle, szXml, iLen, &iLen) != S_OK )
	{
		delete[] szXml;
		szXml = NULL;
		return E_FAIL;
	}

	//int iTempLen = 100*1024;
	int iTempLen = 1024 *1024;

	char *szTempWhiteList = new char[iTempLen];
	if ( szTempWhiteList == NULL )
	{
		delete[] szXml;
		szXml = NULL;
		return E_FAIL;
	}

	char *szTempBlackList = new char[iTempLen];
	if ( szTempBlackList == NULL )
	{
		delete[] szXml;
		szXml = NULL;
		delete[] szTempWhiteList;
		szTempWhiteList = NULL;
		return E_FAIL;
	}

	memset(szTempWhiteList, 0, iTempLen);
	memset(szTempBlackList, 0, iTempLen);


	char *pWhiteList = strstr(szXml, "WhiteNameList");
	if ( pWhiteList != NULL )
	{
		sscanf(pWhiteList,"WhiteNameList[%[^]]", szTempWhiteList);
	}

	char *pBlackList = strstr(szXml, "BlackNameList");
	if (pBlackList != NULL )
	{
		sscanf(pBlackList,"BlackNameList[%[^]]", szTempBlackList);
	}


	int iWhileLen = (INT)strlen(szTempWhiteList);
	int iBlackLen = (INT)strlen(szTempBlackList);

	if ( szWhiteNameList != NULL && iWhiteListLen != NULL )
	{
		int  iTempLen = *iWhiteListLen > iWhileLen ? iWhileLen: *iWhiteListLen;
		strncpy(szWhiteNameList, szTempWhiteList, iTempLen);
		*iWhiteListLen = iTempLen;

	}

	if ( szBlackNameList != NULL && iBlackListLen != NULL )
	{
		int  iTempLen = *iBlackListLen > iBlackLen ? iBlackLen: *iBlackListLen;
		strncpy(szBlackNameList, szTempBlackList, iTempLen);
		*iBlackListLen = iTempLen;
	}

	delete[] szTempWhiteList;
	szTempWhiteList = NULL;

	delete[] szTempBlackList;
	szTempBlackList = NULL;

	delete[] szXml;
	szXml = NULL;
	return S_OK;
}


HV_API_EX HRESULT CDECL HVAPI_SetOSDEnable(HVAPI_HANDLE_EX hHandle, INT nStreamId ,BOOL fOSDEnable)
{
	if(hHandle == NULL )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		//执行命令
		CHAR szCmd[128]={0};
		sprintf( szCmd , "SetOSDEnable,StreamId[%d],OSDEnable[%d]" ,nStreamId , fOSDEnable );

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			//WrightLogEx(pHHC->szIP , "SetOSD Enable HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		//解析命令返回
		INT nRetCode = -1;

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetOSDEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			//WrightLogEx(pHHC->szIP , "SetOSD Enable HvXmlParse Fail\n");
			return E_FAIL;
		}


		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}
		return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		//执行命令
		CHAR szCmd[128]={0};
		CHAR szCmdName[128]={0};
		if (0 == nStreamId)
		{
			sprintf(szCmdName, "SetOSDh264Enable");
		}
		else if (1 == nStreamId)
		{
			sprintf(szCmdName, "SetOSDjpegEnable");
		}
		else if (2 == nStreamId)
		{
			sprintf(szCmdName, "SetOSDh264SecondEnable");
		}
		else
		{
			return S_FALSE;
		}


			sprintf( szCmd , "%s,OSDEnable[%d]" , szCmdName, fOSDEnable );
			CHAR szRet[1024]={0};
			INT nRetLen = 1024;

			if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
			{
				//WrightLogEx(pHHC->szIP , "SetOSDEnable HVAPI_ExecCmdEx Fail\n");
				return E_FAIL;
			}

			//解析命令返回
			INT nRetCode = -1;

			CXmlParseInfo cRetInfo;
			strcpy( cRetInfo.szKeyName, "RetCode" );

			if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
			{
				//WrightLogEx(pHHC->szIP , "SetOSDEnable HvXmlParse Fail\n");
				return E_FAIL;
			}


			nRetCode = atoi(cRetInfo.szKeyValue);
			return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);



	}

return S_OK;

}

HV_API_EX HRESULT CDECL HVAPI_SetOSDPlateEnable(HVAPI_HANDLE_EX hHandle, INT nStreamId, BOOL fEnable)
{
	if(hHandle == NULL )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}
	//执行命令
	CHAR szCmd[128]={0};
	CHAR szCmdName[128]={0};
	if (0 == nStreamId)
	{
		sprintf(szCmdName, "SetOSDh264PlateEnable");
	}
	else if (1 == nStreamId)
	{
		sprintf(szCmdName, "SetOSDjpegPlateEnable");
	}
	else if (2 == nStreamId)
	{
		sprintf(szCmdName, "SetOSDh264SecondPlateEnable");
	}
	else
	{
		return S_FALSE;
	}


	sprintf( szCmd , "%s,OSDPlateEnable[%d]" , szCmdName,fEnable );



	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		//WrightLogEx(pHHC->szIP , "SetOSDPlateEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		//WrightLogEx(pHHC->szIP , "SetOSDPlateEnable HvXmlParse Fail\n");
		return E_FAIL;
	}


	nRetCode = atoi(cRetInfo.szKeyValue);


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}


HV_API_EX HRESULT CDECL HVAPI_SetOSDTimeEnable(HVAPI_HANDLE_EX hHandle, INT nStreamId, BOOL fEnable)
{
	if(hHandle == NULL )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}


	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		//执行命令
		CHAR szCmd[128]={0};
		sprintf( szCmd , "SetOSDTimeEnable,StreamId[%d],TimeStampEnable[%d]" ,nStreamId , fEnable );

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			//WrightLogEx(pHHC->szIP , "HVAPI_SetOSDTimeEnable HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetOSDTimeEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			//WrightLogEx(pHHC->szIP , "HVAPI_SetOSDTimeEnable HvXmlParse Fail\n");
			return E_FAIL;
		}


		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		//执行命令
		CHAR szCmd[128]={0};
		CHAR szCmdName[128]={0};
		if (0 == nStreamId)
		{
			sprintf(szCmdName, "SetOSDh264TimeEnable");
		}
		else if (1 == nStreamId)
		{
			sprintf(szCmdName, "SetOSDjpegTimeEnable");
		}
		else if (2 == nStreamId)
		{
			sprintf(szCmdName, "SetOSDh264SecondTimeEnable");
		}
		else
		{
			return S_FALSE;
		}


		sprintf( szCmd , "%s,OSDPlateEnable[%d]" ,szCmdName, fEnable );



		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			//WrightLogEx(pHHC->szIP , "HVAPI_SetOSDTimeEnable HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			//WrightLogEx(pHHC->szIP , "HVAPI_SetOSDTimeEnable HvXmlParse Fail\n");
			return E_FAIL;
		}


		nRetCode = atoi(cRetInfo.szKeyValue);

	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}


HV_API_EX HRESULT CDECL HVAPI_SetOSDPos( HVAPI_HANDLE_EX hHandle,INT nStreamId, INT nPosX ,INT nPosY )
{
	if(hHandle == NULL )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}


	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		//执行命令
		CHAR szCmd[128]={0};
		sprintf( szCmd , "SetOSDPos,StreamId[%d],PosX[%d],PosY[%d]" ,nStreamId , nPosX , nPosY );

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			//WrightLogEx(pHHC->szIP , "HVAPI_SetOSDPos HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParse( "SetOSDPos", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			//WrightLogEx(pHHC->szIP , "HVAPI_SetOSDPos HvXmlParse Fail\n");
			return E_FAIL;
		}


		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		//执行命令
		CHAR szCmd[128]={0};
		CHAR szCmdName[128]={0};
		if (0 == nStreamId)
		{
			sprintf(szCmdName, "SetOSDh264Pos");
		}
		else if (1 == nStreamId)
		{
			sprintf(szCmdName, "SetOSDjpegPos");
		}
		else if (2 == nStreamId)
		{
			sprintf(szCmdName, "SetOSDh264SecondPos");
		}
		else
		{
			return S_FALSE;
		}


		sprintf( szCmd , "%s, PosX[%d], PosY[%d]" ,szCmdName, nPosX , nPosY  );



		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			//WrightLogEx(pHHC->szIP , "HVAPI_SetOSDPos HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			//WrightLogEx(pHHC->szIP , "HVAPI_SetOSDPos HvXmlParse Fail\n");
			return E_FAIL;
		}


			nRetCode = atoi(cRetInfo.szKeyValue);

	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}


HV_API_EX HRESULT CDECL HVAPI_SetOSDFontSize(HVAPI_HANDLE_EX hHandle, INT nStreamId ,INT nFontSize)
{
	if(hHandle == NULL )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}
	//执行命令
	CHAR szCmd[128]={0};
	CHAR szCmdName[128]={0};
	if (0 == nStreamId)
	{
		sprintf(szCmdName, "SetOSDh264Font");
	}
	else if (1 == nStreamId)
	{
		sprintf(szCmdName, "SetOSDjpegFont");
	}
	else if (2 == nStreamId)
	{
		sprintf(szCmdName, "SetOSDh264SecondFont");
	}
	else
	{
		return S_FALSE;
	}

	INT nRetCode = -1;


		sprintf( szCmd , "%s,FoneSize[%d]" ,szCmdName, nFontSize );
		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			//WrightLogEx(pHHC->szIP , "SetOSDFont HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		//解析命令返回

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			//WrightLogEx(pHHC->szIP , "SetOSDFont HvXmlParse Fail\n");
			return E_FAIL;
		}


		nRetCode = atoi(cRetInfo.szKeyValue);

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}
HV_API_EX HRESULT CDECL HVAPI_SetOSDFontRGB(HVAPI_HANDLE_EX hHandle, INT nStreamId ,INT nColorR ,INT nColorG,INT nColorB)
{
	if(hHandle == NULL )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}
	//执行命令
	CHAR szCmd[128]={0};
	CHAR szCmdName[128]={0};
	if (0 == nStreamId)
	{
		sprintf(szCmdName, "SetOSDh264FontRGB");
	}
	else if (1 == nStreamId)
	{
		sprintf(szCmdName, "SetOSDjpegFontRGB");
	}
	else if (2 == nStreamId)
	{
		sprintf(szCmdName, "SetOSDh264SecondFontRGB");
	}
	else
	{
		return E_FAIL;
	}

	INT nRetCode = -1;


	sprintf( szCmd , "%s,ColorR[%d],ColorG[%d],ColorB[%d]" ,szCmdName, nColorR, nColorG, nColorB );
	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		//WrightLogEx(pHHC->szIP , "HVAPI_SetOSDFontRGB HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回

	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		//WrightLogEx(pHHC->szIP , "HVAPI_SetOSDFontRGB HvXmlParse Fail\n");
		return E_FAIL;
	}


	nRetCode = atoi(cRetInfo.szKeyValue);

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);

}






HV_API_EX HRESULT CDECL HVAPI_SetOSDFont(HVAPI_HANDLE_EX hHandle, INT nStreamId ,
							   INT nFontSize ,INT nColorR ,INT nColorG,INT nColorB)
{
	if(hHandle == NULL )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		//执行命令
		CHAR szCmd[128]={0};
		sprintf( szCmd , "SetOSDFont,StreamId[%d],FontSize[%d],ColorR[%d],ColorG[%d],ColorB[%d]" ,nStreamId ,
			nFontSize ,nColorR ,nColorG, nColorB );

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			//WrightLogEx(pHHC->szIP , "HVAPI_SetOSDFont HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		//解析命令返回
		INT nRetCode = -1;


		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetOSDFont", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			//WrightLogEx(pHHC->szIP , "HVAPI_SetOSDFont HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
	}
	else
	{

		if (  ( S_OK == HVAPI_SetOSDFontSize(hHandle, nStreamId, nFontSize)  )
			&&( S_OK == HVAPI_SetOSDFontRGB(hHandle, nStreamId, nColorR, nColorG, nColorB) )
			)
		{
			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}
}

BOOL IsVailOSDText( CHAR* szText , INT nTextLen )
{
	if ( NULL == szText || nTextLen <= 0 )
	{
		return FALSE;
	}
	BOOL fIsVail = TRUE;
	for ( INT i = 0 ; i < nTextLen; i++ )
	{
		if ( '[' == szText[i]
				|| ']' == szText[i]
				|| ',' == szText[i]

			)
		{
			fIsVail = FALSE;
			break;
		}
	}
	return fIsVail;
}

HV_API_EX HRESULT CDECL HVAPI_SetOSDText(HVAPI_HANDLE_EX hHandle, INT nStreamId, CHAR* szText )
{
	if( hHandle == NULL || NULL == szText || strlen(szText) > 255 )
	{
		return E_FAIL;
	}
	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
	|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CHAR szCmd[512] = {0};
		INT nRet = -1;

		CHAR* pBuf = szCmd ;
		memcpy( pBuf , &nStreamId , sizeof(INT));
		pBuf += sizeof(INT);

		INT nTextLen = strlen(szText);
		memcpy( pBuf , &nTextLen  , sizeof(INT));
		pBuf += sizeof(INT);

		memcpy( pBuf , szText  , strlen(szText) + 1);
		pBuf += strlen(szText) + 1;

		HRESULT hr = ExecuteCmd( pHHC->szIP ,CAMERA_SET_OSD_TEXT , szCmd ,pBuf-szCmd ,NULL , &nRet );
		if ( S_OK != hr )
		{
			//WrightLogEx(pHHC->szIP , "ExecuteCmd HVAPI_SetOSDText Fail\n" );
		}
		return hr;
	}
	else 	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		//执行命令
		CHAR szCmd[128]={0};
		CHAR szCmdName[128]={0};
		if (0 == nStreamId)
		{
			sprintf(szCmdName, "SetOSDh264Text");
		}
		else if (1 == nStreamId)
		{
			sprintf(szCmdName, "SetOSDjpegText");
		}
		else if (2 == nStreamId)
		{
			sprintf(szCmdName, "SetOSDh264SecondText");
		}
		else
		{
			return S_FALSE;
		}

		sprintf( szCmd , "%s,OSDText[%s]" ,szCmdName, szText );


		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			//WrightLogEx(pHHC->szIP , "HVAPI_SetOSDText HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		INT nRetCode=-1;
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			//WrightLogEx(pHHC->szIP , "HVAPI_SetOSDText HvXmlParse Fail\n");
			return E_FAIL;
		}


		nRetCode = atoi(cRetInfo.szKeyValue);
		return nRetCode;

	}
return S_OK;

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










