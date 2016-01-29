// 接口实现

#include "HvDeviceNew.h"
#include "HvCamera.h"
#include "HvDevice.h"
#include "HVAPI_HANDLE_CONTEXT_EX.h"
#include "HvDeviceDLL.h"
#include <gdiplus.h>
#include <atltime.h>
#include <atlstr.h>
#include "HvAutoLink.h"
#include "HvListenCMD.h"
#include "HvResultFilter.h"


#include <map> 
#include<string>

#include <imagehlp.h>
#pragma comment(lib, "imagehlp.lib")


#ifdef _CARFACE
#include "CarFaceCtrl.h"
#endif

//后处理
CHvResultFilter g_ResultFilter;
char* g_szFilterFileName = "ResultTransact.txt";
WORD g_fFilterValid = 1;

CRITICAL_SECTION g_csFilterSync;		// 全局同步
#pragma warning( disable : 4996 )

//#define OUT_DEBUG_LOG	 1

using namespace HiVideo;
using namespace Gdiplus;

CLSID	g_jpgClsid = {0};
ULONG_PTR g_gdiplusToken = 0;
BOOL g_fGdiInitialized = FALSE;
BOOL g_fIsDrawPlateFrame = TRUE;
//HVDEVICE.ini
char g_szIniFile[MAX_PATH] = {0};

#define DEFAULT_CONNCMD  "DownloadRecord,Enable[0],BeginTime[2012.01.01_01],Index[0],EndTime[0]"

extern HRESULT IsEmptyPackCache( HVAPI_HANDLE_CONTEXT_EX* pHandle , PACK_TYPE eType , bool& fIsEmptyCache);
extern HRESULT GetPackCache( HVAPI_HANDLE_CONTEXT_EX* pHandle , PACK_TYPE eType ,unsigned char** ppBuffer , int& iBufferLen );
extern HRESULT UpdatePackCache(HVAPI_HANDLE_CONTEXT_EX* pHandle, PACK_TYPE eType , char* pBuffer , int nBufferLen );

extern HRESULT CloseAutoLinkHHC(LPSTR szDevSN);
extern HRESULT OutPutDebugInfor(char* pDebugInfo);
extern HRESULT SetConnAutoLinkHHC(LPSTR szDevSN);

// 描述:	设置车牌后处理(车牌替换),当规则文件更换或内容改变后都要调用这个函数才能生效
// 参数:	pszFilePath					后处理规则文件,如果为NULL,则用默认的文件
//			fValid				        是否启用后处理: 1启用 0不启用
// 返回值: 返回S_OK, 表示成功;
//         返回E_FAIL, 表示未知的错误导致操作失败;



// 描述:	设置车牌后处理(车牌替换),当规则文件更换或内容改变后都要调用这个函数,更改的规则才能生效
// 参数:	pszFilePath					后处理规则文件,如果为NULL,则用默认的文件
//			fValid				        是否启用后处理: 1启用 0不启用
// 返回值: 返回S_OK, 表示成功;
//         返回E_FAIL, 表示未知的错误导致操作失败;
HRESULT SetResultTransact(char* pszFilePath /*= NULL*/, WORD  fValid /*= 1*/)
{
	CString strModulePath;
	// 取得包括程序名的全路径
	GetModuleFileName(NULL, strModulePath.GetBuffer(MAX_PATH),MAX_PATH);

	//去掉程序名
	PathRemoveFileSpec(strModulePath.GetBuffer());
	strModulePath.ReleaseBuffer();
	if(strModulePath.Right(0) != '\\')
		strModulePath.Append("\\");
	CString strFilePath = strModulePath + g_szFilterFileName;

	if(pszFilePath != NULL)
	{
		if(strchr(pszFilePath, '\\') == NULL)
		{
			strFilePath = strModulePath + pszFilePath;
		}
		else
		{
			strFilePath = pszFilePath;
		}
	}

	if(fValid == 1)
	{
		g_ResultFilter.InitRule(NULL, strFilePath);
		g_fFilterValid = 1;
	}
	else
	{
		g_fFilterValid = 0;
	}

	return S_OK;
}



void WrightLogEx(LPCSTR lpszIP, LPCSTR lpszLogInfo)
{
#ifdef OUT_DEBUG_LOG
	CTime cTime = CTime::GetCurrentTime();
	char szTime[256] = {0};
	FILE* fp = fopen("E:\\HvDeviceNewLog.txt", "a+");
	if(fp)
	{
		sprintf(szTime, "%s  <%s>  ", cTime.Format("%Y-%m-%d %H:%M:%S"), lpszIP);
		fwrite(szTime, strlen(szTime), 1, fp);
		fwrite(lpszLogInfo, strlen(lpszLogInfo), 1, fp);
		fwrite("\n", 1, 1, fp);
		fclose(fp);
	}
#endif
}
#define _TRANSBUF_LEN 4 * 1024 * 1024
CString FileReadLine( FILE* pFilePlateNo )
{
	char chFileLine[1024] = {0};
	char chCharacter;
	int  iIndex = 0;

	//ASSERT( pFilePlateNo != NULL );

	chCharacter = fgetc( pFilePlateNo );
	if( chCharacter == EOF )
		return "NO";
	while( chCharacter != EOF )
	{
		chFileLine[iIndex] = chCharacter;
		if ((chCharacter == '\n') || (chCharacter == EOF))
			break;
		chCharacter = fgetc( pFilePlateNo );
		if (iIndex++ >= 1023) 
		{
			chFileLine[1023] = 0;
			break;
		}
	}

	CString strLine = chFileLine;
	return strLine.Mid(0,iIndex);
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

BOOL WritePrivateProfileInt(const PSTR pszSection, const PSTR pszKey, const int& iValue, const PSTR pszFilePath)
{
	char szTemp[50] = {0};
	sprintf(szTemp, "%d", iValue );
	return WritePrivateProfileStringA(pszSection, pszKey, szTemp, pszFilePath);
}

void LoadIniConfig(void)
{
	char szAppPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szAppPath, MAX_PATH);
	PathRemoveFileSpec(szAppPath);
	strcat(szAppPath, "\\HvDevice.ini");
	if(!PathFileExists(szAppPath))
	{
		int iShowPlateFrameValue = (g_fIsDrawPlateFrame == TRUE) ? 1 : 0;
		WritePrivateProfileInt("MainConfig", "DrawLprPlateFrame", iShowPlateFrameValue,szAppPath);

		WritePrivateProfileInt("CARFACEENABLE", "Enable", 0, szAppPath);

	}
	else
	{
		int iShowPlateFrameValue = GetPrivateProfileInt("MainConfig", "DrawLprPlateFrame",0,szAppPath);
		g_fIsDrawPlateFrame = (iShowPlateFrameValue == 1) ? TRUE : FALSE;
	}
}


////水星协议set get执行快捷函数，TODO
//适用条件  xml关键字如 { "SetGain", DEFAULT_SETTER },
HRESULT static Default_Setter_Int_ForMercuryProtocol(HVAPI_HANDLE_EX hHandle, const CHAR* szCmdName, INT nValue )
{
	if(NULL ==  hHandle|| NULL == szCmdName )
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

	//执行命令
	CHAR szCmd[128]={0};
	sprintf( szCmd , "%s,Value[%d]" ,szCmdName, nValue);

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		CHAR szTemp[128]={0};
		sprintf(szTemp, "%s HVAPI_ExecCmdEx Fail\n", szCmdName);
		WrightLogEx(pHHC->szIP , szTemp);
		return E_FAIL;
	}

	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		CHAR szTemp[128]={0};
		sprintf(szTemp, "%s HvXmlParse Fail\n", szCmdName);
		WrightLogEx(pHHC->szIP , szTemp);
		return E_FAIL;
	}


	nRetCode = atoi(cRetInfo.szKeyValue);


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

//适用条件  xml关键字如 {  "GetGain", DEFAULT_GETTER },  
HRESULT static Default_Getter_Int_ForMercuryProtocol(HVAPI_HANDLE_EX hHandle, const CHAR* szCmdName, INT* pnValue )
{
	if(hHandle == NULL || NULL == pnValue )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}

	CHAR szRet [1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , szCmdName, szRet , nRetLen , &nRetLen ) )
	{
		CHAR szTemp[128]={0};
		sprintf(szTemp, "%s HVAPI_ExecCmdEx Fail\n", szCmdName);
		WrightLogEx(pHHC->szIP , szTemp);
		return E_FAIL ;
	}

	//解析命令返回
	INT nRetCode = -1;


	CXmlParseInfo rgRetInfo [2];
	strcpy( rgRetInfo [0].szKeyName, "RetCode" );
	CHAR szFirstParamName[128];
	if (S_OK != HvGetFirstParamNameFromXmlCmdAppendInfoMap(szCmdName, szFirstParamName, 128))
	{
		return E_FAIL;
	}
	strcpy( rgRetInfo [1].szKeyName, szFirstParamName );//TODO(liyh) 比较好的做法，可以查询全局命令map中，对应命令的回传参数，可以通用

	if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , rgRetInfo, 2 ) )
	{
		CHAR szTemp[128]={0};
		sprintf(szTemp, "%s HvXmlParse Fail\n", szCmdName);
		WrightLogEx(pHHC->szIP , szTemp);

		return E_FAIL ;
	}


	nRetCode = atoi (rgRetInfo[0]. szKeyValue);

	* pnValue = atoi (rgRetInfo[1]. szKeyValue);


	return nRetCode ==0 ?S_OK: E_FAIL;
}

HRESULT static Default_Setter_Bool_ForMercuryProtocol(HVAPI_HANDLE_EX hHandle, const CHAR* szCmdName, BOOL fValue )
{
	if(NULL ==  hHandle|| NULL == szCmdName )
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

	//执行命令
	CHAR szCmd[128]={0};
	sprintf( szCmd , "%s,Value[%d]" ,szCmdName, fValue);

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		CHAR szTemp[128]={0};
		sprintf(szTemp, "%s HVAPI_ExecCmdEx Fail\n", szCmdName);
		WrightLogEx(pHHC->szIP , szTemp);
		return E_FAIL;
	}

	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		CHAR szTemp[128]={0};
		sprintf(szTemp, "%s HvXmlParse Fail\n", szCmdName);
		WrightLogEx(pHHC->szIP , szTemp);
		return E_FAIL;
	}


	nRetCode = atoi(cRetInfo.szKeyValue);


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}
HRESULT static Default_Getter_Bool_ForMercuryProtocol(HVAPI_HANDLE_EX hHandle, const CHAR* szCmdName, BOOL* pfValue )
{
	if(hHandle == NULL || NULL == pfValue )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}

	CHAR szRet [1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , szCmdName, szRet , nRetLen , &nRetLen ) )
	{
		CHAR szTemp[128]={0};
		sprintf(szTemp, "%s HVAPI_ExecCmdEx Fail\n", szCmdName);
		WrightLogEx(pHHC->szIP , szTemp);
		return E_FAIL ;
	}

	//解析命令返回
	INT nRetCode = -1;


	CXmlParseInfo rgRetInfo [2];
	strcpy( rgRetInfo [0].szKeyName, "RetCode" );
	CHAR szFirstParamName[128];
	if (S_OK != HvGetFirstParamNameFromXmlCmdAppendInfoMap(szCmdName, szFirstParamName, 128))
	{
		return E_FAIL;
	}
	strcpy( rgRetInfo [1].szKeyName, szFirstParamName );//TODO(liyh) 比较好的做法，可以查询全局命令map中，对应命令的回传参数，可以通用

	if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , rgRetInfo, 2 ) )
	{
		CHAR szTemp[128]={0};
		sprintf(szTemp, "%s HvXmlParse Fail\n", szCmdName);
		WrightLogEx(pHHC->szIP , szTemp);

		return E_FAIL ;
	}


	nRetCode = atoi (rgRetInfo[0]. szKeyValue);

	* pfValue = atoi (rgRetInfo[1]. szKeyValue);


	return nRetCode ==0 ?S_OK: E_FAIL;
}

HRESULT static Default_Setter_INTARRAY1D_ForMercuryProtocol(HVAPI_HANDLE_EX hHandle, const CHAR* szCmdName,INT* prgValue, INT nNum )
{
	if(NULL ==  hHandle|| NULL == szCmdName || NULL == prgValue  )
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
	//执行命令
	CHAR szCmd[256]={0};
	strcat(szCmd, szCmdName);

	CHAR szParam[128];
	for(int i=0; i<nNum; i++)
	{
		strcat(szCmd, ",");
		sprintf(szParam, "Value%d[%d]", i, prgValue[i]);
		strcat(szCmd, szParam);		
	}	

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		CHAR szTemp[128]={0};
		sprintf(szTemp, "%s HVAPI_ExecCmdEx Fail\n", szCmdName);
		WrightLogEx(pHHC->szIP , szTemp);
		return E_FAIL;
	}

	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		CHAR szTemp[128]={0};
		sprintf(szTemp, "%s HvXmlParse Fail\n", szCmdName);
		WrightLogEx(pHHC->szIP , szTemp);
		return E_FAIL;
	}

	nRetCode = atoi(cRetInfo.szKeyValue);

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HRESULT static Default_Getter_INTARRAY1D_ForMercuryProtocol(HVAPI_HANDLE_EX hHandle, const CHAR* szCmdName,INT* prgValue, INT nNum )
{
	if(NULL ==  hHandle|| NULL == szCmdName || NULL == prgValue )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	CHAR szRet [5*1024]={0};
	INT nRetLen = 5*1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , szCmdName , szRet , nRetLen , &nRetLen) )
	{
		CHAR szTemp[128]={0};
		sprintf(szTemp, "%s HVAPI_ExecCmdEx Fail\n", szCmdName);
		WrightLogEx(pHHC->szIP , szTemp);
		return E_FAIL;
	}

	INT nRetCode = -1;

	CXmlParseInfo* pcRetInfo = new CXmlParseInfo[nNum+1];
	if(NULL == pcRetInfo)
	{
		return E_FAIL;
	}
	strcpy( pcRetInfo[0].szKeyName, "RetCode" );
	

	HvGetParamNameFromXmlCmdAppendInfoMap(szCmdName, pcRetInfo, nNum+1);


	if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , pcRetInfo, nNum + 1 ) )
	{
		CHAR szTemp[128]={0};
		sprintf(szTemp, "%s HvXmlParse Fail\n", szCmdName);
		WrightLogEx(pHHC->szIP , szTemp);
		return E_FAIL;
	}


	nRetCode = atoi(pcRetInfo[0].szKeyValue);
	for (int i=0; i<nNum; i++)
	{
		prgValue[i] = atoi(pcRetInfo[i+1].szKeyValue);
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HRESULT static Default_Getter_String_ForMercuryProtocol(HVAPI_HANDLE_EX hHandle, const CHAR* szCmdName, CHAR* pszValue, INT* pnLen )
{

}

HRESULT static Default_Setter_String_ForMercuryProtocol(HVAPI_HANDLE_EX hHandle, const CHAR* szCmdName,CHAR* pszValue)
{

}


// ===========================================对外接口实现=======================================

/**
* @brief			数据上传接口
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		dwCMD_TYPE_ID		上传类型命令ID
* @param[in]		szUploadDate		要保存到设备的数据
* @param[in]		dwDataLen		要保存到设备的数据长度
* @return			成功：S_OK；失败：E_FAIL
*/
HRESULT  HVAPI_UploadData(HVAPI_HANDLE_EX hHandle, DWORD32 dwCMD_TYPE_ID, CHAR* szUploadDate, DWORD32 dwDataLen)
{
	if ( NULL == hHandle || NULL == szUploadDate  )
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
		cCmdHeader.dwID = dwCMD_TYPE_ID;
		cCmdHeader.dwInfoSize = dwDataLen;
		int iReciveTimeOutMS = 60000;
		setsockopt(hSocketCmd, SOL_SOCKET, SO_RCVTIMEO, (char*)&iReciveTimeOutMS, sizeof(iReciveTimeOutMS));
		if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0) )
		{
			ForceCloseSocket(hSocketCmd);
			CHAR szTemp[128]= {0};
			sprintf(szTemp, "HVAPI_UploadData Fail, send cmdheader fail \n");
			WrightLogEx(pHHC->szIP , szTemp);
			return E_FAIL;
		}
		if ( cCmdHeader.dwInfoSize != send(hSocketCmd, (const char*)szUploadDate, cCmdHeader.dwInfoSize, 0) )
		{
			ForceCloseSocket(hSocketCmd);
			CHAR szTemp[128]= {0};
			sprintf(szTemp, "HVAPI_UploadData Fail, send upload date fail \n");
			WrightLogEx(pHHC->szIP , szTemp);
			return E_FAIL;
		}

		if ( sizeof(cCmdRespond) == recv(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0) )
		{
			if ( dwCMD_TYPE_ID == cCmdRespond.dwID
			        && 0 == cCmdRespond.dwResult )
			{
				ForceCloseSocket(hSocketCmd);
				return S_OK;
			}
		}

		ForceCloseSocket(hSocketCmd);
	}

	CHAR szTemp[128]= {0};
	sprintf(szTemp, "HVAPI_UploadData Fail, can not connect device \n");
	WrightLogEx(pHHC->szIP , szTemp);

	return E_FAIL;
}


HV_API_EX HRESULT CDECL HVAPI_SearchDeviceEx( CDevBasicInfo* rgDevInfo , INT* pnDevCount )
{
	return SearchDeviceEx(rgDevInfo , pnDevCount );
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
	if ( NULL == pcIP || NULL == iDeviceType )
	{
		return E_POINTER;
	}
	
	if (strcmp(pcIP, "") == 0)
	{
		return E_FAIL;
	}
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

	PROTOCOL_VERSION eXmlVersion = GetProtocolVersion(pcIP);
	if(HvMakeXmlInfoByString( eXmlVersion , "GetDevType", 10, pszXmlBuf, iXmlBufLen) == E_FAIL)
	{
		return E_FAIL;
	}
	char* pszRetBuf = new char[1024];
	if(!pszRetBuf)
	{
		SAFE_DELETE_ARG(pszXmlBuf);
		return E_FAIL;
	}
	memset(pszRetBuf, 0, 1024);
	int iRetBufLen = 1024;
	SOCKET sktCmd = INVALID_SOCKET;

	if( PROTOCOL_VERSION_1 == eXmlVersion 
		|| PROTOCOL_VERSION_2 == eXmlVersion 
		||  PROTOCOL_VERSION_EARTH == eXmlVersion )
	{
		if(ExecXmlExtCmdEx(pcIP, (char*)pszXmlBuf, (char*)pszRetBuf, iRetBufLen, sktCmd) == false)
		{
			SAFE_DELETE_ARG(pszRetBuf);
			SAFE_DELETE_ARG(pszXmlBuf);
			return E_FAIL;
		}
	}
	else
	{
		if(ExecXmlExtCmdMercury(pcIP, (char*)pszXmlBuf, (char*)pszRetBuf, iRetBufLen, sktCmd) == false)
		{
			SAFE_DELETE_ARG(pszRetBuf);
			SAFE_DELETE_ARG(pszXmlBuf);
			return E_FAIL;
		}
	}

	char szRetcode[256] = {0};
	if( eXmlVersion  == PROTOCOL_VERSION_1 )
	{
		if(FAILED(HvParseXmlInfoRespValue(pszRetBuf, "GetDevType", "DevType", szRetcode)))
		{
			SAFE_DELETE_ARG(pszRetBuf);
			SAFE_DELETE_ARG(pszXmlBuf);
			return S_OK;
		}
	}
	else if( eXmlVersion  == PROTOCOL_VERSION_2 )
	{
		if(FAILED(HvParseXmlCmdRespRetcode2(pszRetBuf, "GetDevType", "DevType", szRetcode)))
		{
			SAFE_DELETE_ARG(pszRetBuf);
			SAFE_DELETE_ARG(pszXmlBuf);
			return S_OK;
		}
	}
	else if( eXmlVersion  == PROTOCOL_VERSION_EARTH )
	{
		//解析命令返回
		CXmlParseInfo rgRetInfo[2];
		strcpy( rgRetInfo[0].szKeyName, "RetCode" );
		strcpy( rgRetInfo[1].szKeyName, "DevType" );

		if ( S_OK != HvXmlParse( "GetDevType", pszRetBuf ,strlen(pszRetBuf) ,rgRetInfo , 2 ) )
		{
			WrightLogEx("GetDevType" , "GetDevType HvXmlParse Fail\n");
			return E_FAIL;
		}
		INT nRetCode = -1;
		if ( XML_CMD_TYPE_INT == rgRetInfo[0].eKeyType )
		{
			nRetCode = atoi(rgRetInfo[0].szKeyValue);
		}
		if ( 0 == nRetCode && XML_CMD_TYPE_STRING == rgRetInfo[1].eKeyType )
		{
			strcpy( szRetcode , rgRetInfo[1].szKeyValue ) ;
		}

	}
	else // 水星设备端 ,HvDevice 强制改成 2.0, 因为NAVI上面有太多判断版本号的做法，此为临时做法
	{
		if(FAILED(HvParseXmlCmdRespRetcode3(pszRetBuf, "GetDevType", "DevType", szRetcode)))
		{
			SAFE_DELETE_ARG(pszRetBuf);
			SAFE_DELETE_ARG(pszXmlBuf);
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
	else if( strstr(szRetcode, "Mercury_300W") )
	{
		*iDeviceType = DEV_TYPE_HVMERCURY;
	}
	else if ( strcmp(szRetcode, "PCC200A") == 0 )
	{
		*iDeviceType = DEV_TYPE_HVEARTH;
	}
	else if (   (strcmp(szRetcode, "PCC200") == 0)     ||    (strcmp(szRetcode, "PCC600") == 0)      )
	{
		*iDeviceType = DEV_TYPE_HVVENUS;
	}
	else if (strcmp(szRetcode, "SDC200") == 0)
	{
		*iDeviceType = DEV_TYPE_HVJUPITER;
	}
	else
	{
		*iDeviceType = DEV_TYPE_UNKNOWN;
	}
	SAFE_DELETE_ARG(pszRetBuf);
	SAFE_DELETE_ARG(pszXmlBuf);
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
		CString BugValue;
		if(pHHC->dwRecordConnStatus == CONN_STATUS_RECONN)
		{	
			BugValue.Empty();
		//	BugValue.Format("%d-%d-%d", BUGTIME.GetHour(), BUGTIME.GetMinute(), BUGTIME.GetSecond());
			BugValue += "结果回调尝试重连中..." ;
			WrightLogEx(pHHC->szIP, BugValue);

			fIsNeedSleep = FALSE;
			if(ConnectCamera(pHHC->szIP, CAMERA_RECORD_LINK_PORT, pHHC->sktRecord))
			{	
				if(strlen((const char*)pHHC->szRecordConnCmd) > 0)
				{
					BugValue.Empty();
					BugValue.Format("***%s", pHHC->szRecordConnCmd);
					WrightLogEx(pHHC->szIP, BugValue);

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
					char szRetConnCmd[128] = {0};
					memcpy(szRetConnCmd, (char*)pHHC->szRecordConnCmd, 128);
					if(HvSendXmlCmd(pHHC->szIP, szRetConnCmd, 
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
		else
		{
			BugValue.Empty();
			BugValue += "设备结果重连失败..." ;
		}
		if(pHHC->dwImageConnStatus == CONN_STATUS_RECONN)
		{
			BugValue.Empty();
	//		BugValue.Format("%d-%d-%d", BUGTIME.GetHour(), BUGTIME.GetMinute(), BUGTIME.GetSecond());
			BugValue += "图片回调尝试重连中..." ;
			WrightLogEx(pHHC->szIP, BugValue);

			fIsNeedSleep = FALSE;
			if(ConnectCamera(pHHC->szIP, CAMERA_IMAGE_LINK_PORT, pHHC->sktImage))
			{
				if(strlen((const char*)pHHC->szImageConnCmd) > 0)
				{
					BugValue.Empty();
					BugValue.Format("%s", pHHC->szImageConnCmd);
					WrightLogEx(pHHC->szIP, BugValue);

					char szRetBuf[256] = {0};
					char szRetConnCmd[128] = {0};
					memcpy(szRetConnCmd, (char*)pHHC->szImageConnCmd, 128);

					if(HvSendXmlCmd(pHHC->szIP, szRetConnCmd, 
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
			else
			{
			BugValue.Empty();
			BugValue += "设备图片重连失败..." ;
			}
		}

		if(pHHC->dwVideoConnStatus == CONN_STATUS_RECONN)
		{
			BugValue.Empty();
	//		BugValue.Format("%d-%d-%d", BUGTIME.GetHour(), BUGTIME.GetMinute(), BUGTIME.GetSecond());
			BugValue += "视频回调尝试重连中..." ;
			WrightLogEx(pHHC->szIP, BugValue);

			fIsNeedSleep = FALSE;
			if(ConnectCamera(pHHC->szIP, CAMERA_VIDEO_LINK_PORT, pHHC->sktVideo))
			{
				if(strlen((const char*)pHHC->szVideoConnCmd) > 0)
				{
					BugValue.Empty();
					BugValue.Format("%s", pHHC->szVideoConnCmd);
					WrightLogEx(pHHC->szIP, BugValue);

					if(pHHC->fIsConnectHistoryVideo)
					{
						sprintf((char*)pHHC->szVideoConnCmd, "DownloadVideo,BeginTime[%s],EndTime[%s],Enable[1]",
							pHHC->szVideoBeginTimeStr, pHHC->szVideoEndTimeStr);
					}
					char szRetBuf[256] = {0};
					char szRetConnCmd[128] = {0};
					memcpy(szRetConnCmd, (char*)pHHC->szVideoConnCmd, 128);
					if(HvSendXmlCmd(pHHC->szIP, szRetConnCmd, 
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
			else
			{
                BugValue.Empty();
                BugValue += "设备视频重连失败..." ;
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
		WrightLogEx("testIP", "HVAPI_OpenEx EOR: the input ip is NULL");
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

			hContextHandle->emProtocolVersion = GetProtocolVersion(hContextHandle->szIP);

			if ( PROTOCOL_UNKNOWN == hContextHandle->emProtocolVersion ) //未知， 命令连接没有连接上，无法获取， 返回打开失败
			{
				WrightLogEx(hContextHandle->szIP, "无法获取协议");
				delete hContextHandle;
				return NULL;
			}

			if ( PROTOCOL_VERSION_1 != hContextHandle->emProtocolVersion
				&& PROTOCOL_VERSION_2 != hContextHandle->emProtocolVersion
				|| PROTOCOL_VERSION_EARTH == hContextHandle->emProtocolVersion )
			{
				WrightLogEx(hContextHandle->szIP, "水星协议");
			}
			else
			{
				WrightLogEx(hContextHandle->szIP, "水星之前的协议");
				char szRetBuf[512] = {0};
				if(HVAPI_ExecCmdEx(hRetHandle, "GetVersionString", szRetBuf, sizeof(szRetBuf), NULL) != S_OK)
				{
					WrightLogEx(hContextHandle->szIP, "GetVersionString NULL");
					delete hContextHandle;
					return NULL;
				}
			}

			hContextHandle->fIsThreadSocketStatusMonitorExit = FALSE;
			hContextHandle->hThreadSocketStatusMonitor = CreateThread(NULL, STACK_SIZE, HvSocketStatusMonitorThreadFuncEx, hContextHandle, 0, NULL);
			if(hContextHandle->hThreadSocketStatusMonitor == NULL)
			{
				WrightLogEx(hContextHandle->szIP, "CreateThread NULL");
				delete hContextHandle;
				return NULL;
			}
			//zhanghz,写HvDevice.ini配置
			GetModuleFileName(NULL, g_szIniFile, MAX_PATH);	//取得包括程序名的全路径
			PathRemoveFileSpec(g_szIniFile);				//去掉程序名
			_tcsncat(g_szIniFile, _T("\\HvDevice.ini"), MAX_PATH - 1);

			if(!PathFileExists(g_szIniFile))
			{
				WritePrivateProfileInt(hContextHandle->szIP, "CropImageEnable", 0, g_szIniFile);
				WritePrivateProfileInt(hContextHandle->szIP, "CropImageHeight", 400, g_szIniFile);		
				WritePrivateProfileInt(hContextHandle->szIP, "CropImageWeight", 400, g_szIniFile);

				WritePrivateProfileInt(hContextHandle->szIP, "NoneImageEnable", 0, g_szIniFile);
				WritePrivateProfileInt(hContextHandle->szIP, "NoneImageUpx", 400, g_szIniFile);		
				WritePrivateProfileInt(hContextHandle->szIP, "NoneImageUpy", 300, g_szIniFile);

				WritePrivateProfileInt("CARFACEENABLE", "Enable", 0, g_szIniFile);

			}
			else
			{
				bool fFoundIP = false;
				FILE *pfileInfo = fopen(g_szIniFile, "rb");

				if (pfileInfo)
				{
					CString strLineInfo;
					do
					{
						strLineInfo = FileReadLine(pfileInfo);
						if (strLineInfo.Find(szIp)>=0)
						{
							fFoundIP = true;
						}
						if(fFoundIP)
						{
							break;
						}
					}
					while( strLineInfo != "NO" );
					fclose( pfileInfo );
				}

				//初始化车脸识别标志

				if ( GetPrivateProfileInt("CARFACEENABLE", "Enable", 0, g_szIniFile) == 1)
						hContextHandle->fEnableCarFaceCtrl = true;


				if(fFoundIP == false)
				{
					WritePrivateProfileInt(hContextHandle->szIP, "CropImageEnable", 0, g_szIniFile);
					WritePrivateProfileInt(hContextHandle->szIP, "CropImageHeight", 600, g_szIniFile);		
					WritePrivateProfileInt(hContextHandle->szIP, "CropImageWeight", 800, g_szIniFile);
					hContextHandle->CropImageEnable = 0;
					hContextHandle->CropImageHeight = 600;
					hContextHandle->CropImageWeight = 800;

					WritePrivateProfileInt(hContextHandle->szIP, "NoneImageEnable", 0, g_szIniFile);
					WritePrivateProfileInt(hContextHandle->szIP, "NoneImageUpy", 300, g_szIniFile);		
					WritePrivateProfileInt(hContextHandle->szIP, "NoneImageUpx", 400, g_szIniFile);
					hContextHandle->NoneImageEnable = 0;
					hContextHandle->NoneImageUpy = 300;
					hContextHandle->NoneImageUpx = 400;
				}
				else
				{
					hContextHandle->CropImageEnable = GetPrivateProfileInt(hContextHandle->szIP, "CropImageEnable", 0, g_szIniFile);
					hContextHandle->CropImageHeight = GetPrivateProfileInt(hContextHandle->szIP, "CropImageHeight", 600, g_szIniFile);
					hContextHandle->CropImageWeight = GetPrivateProfileInt(hContextHandle->szIP, "CropImageWeight", 800, g_szIniFile);

					hContextHandle->NoneImageEnable = GetPrivateProfileInt(hContextHandle->szIP, "NoneImageEnable", 0, g_szIniFile);
					hContextHandle->NoneImageUpy = GetPrivateProfileInt(hContextHandle->szIP, "NoneImageUpy", 300, g_szIniFile);
					hContextHandle->NoneImageUpx = GetPrivateProfileInt(hContextHandle->szIP, "NoneImageUpx", 400, g_szIniFile);

					
				}
			}

		}


		#ifdef _CARFACE

		if (hContextHandle->fEnableCarFaceCtrl)
		{
			if ( hContextHandle->pCarFaceRecogCtrl != NULL )
			{
				delete hContextHandle->pCarFaceRecogCtrl;
				hContextHandle->pCarFaceRecogCtrl = NULL;
			}

			hContextHandle->pCarFaceRecogCtrl = new  CCarFaceCtrl();
			if ( hContextHandle->pCarFaceRecogCtrl->InitCtrl() == S_OK )
			{
				WrightLogEx(hContextHandle->szIP, "Init FaceCtrl Success");
			}
			else
			{
				WrightLogEx(hContextHandle->szIP, "Inint FaceCtrl falit ");
			}
		}
		#endif

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

		if (NULL != pHHC->pPackResumeCache)
		{
			delete pHHC->pPackResumeCache;
			pHHC->pPackResumeCache = NULL;
		}
	}

	#ifdef _CARFACE

	if ( pHHC->pCarFaceRecogCtrl != NULL )
	{
		delete pHHC->pCarFaceRecogCtrl;
		 pHHC->pCarFaceRecogCtrl = NULL;
	}

	#endif	

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

HV_API_EX HRESULT CDECL HVAPI_GetXmlVersionEx(HVAPI_HANDLE_EX hHandle, PROTOCOL_VERSION* pemProtocolVersion)
{
	if(hHandle == NULL || pemProtocolVersion == NULL)
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}
	*pemProtocolVersion = pHHC->emProtocolVersion;
	return S_OK;
}

HV_API_EX HRESULT CDECL HVAPI_GetProtocolVersionEx(HVAPI_HANDLE_EX hHandle, PROTOCOL_VERSION* pemProtocolVersion)
{
	if(hHandle == NULL || pemProtocolVersion == NULL)
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}
	*pemProtocolVersion = pHHC->emProtocolVersion;
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
	memset(pszXmlBuf, 0, iBufLen);

	int iXmlBufLen = iBufLen;

	// 从szCmd组织XML命令
	TiXmlDocument cXmlDoc;
	HRESULT hr = S_OK;
	bool fIsXml = cXmlDoc.Parse(szCmd);

	


	switch ( pHHC->emProtocolVersion )
	{
		case PROTOCOL_VERSION_1:
			if (!fIsXml)
			{
				hr = HvMakeXmlCmdByString1( szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen);
				WrightLogEx( pHHC->szIP, pszXmlBuf  );
			}
		case PROTOCOL_VERSION_2:
			if(!fIsXml)
			{
				hr = HvMakeXmlCmdByString2( szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen);
				WrightLogEx( pHHC->szIP, pszXmlBuf  );
			}
			break;
			//地球
		case PROTOCOL_VERSION_EARTH:
			if(!fIsXml)
			{
				WrightLogEx(pHHC->szIP , "PROTOCOL_VERSION_EARTH\n");
				hr = HvMakeXmlCmdByString2( szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen);
				WrightLogEx( pHHC->szIP, pszXmlBuf  );
			}
			break;
		case PROTOCOL_VERSION_MERCURY: 
		default:
			if (!fIsXml)
			{
				WrightLogEx(pHHC->szIP , "PROTOCOL_VERSION_MERCURY\n");
				hr = HvMakeXmlCmdByString3( szCmd, (int)strlen(szCmd), pszXmlBuf, iXmlBufLen);
				WrightLogEx( pHHC->szIP, pszXmlBuf  );
			}
			else
			{
				//（水星）最新版设备不支持 上位机设置XML
				hr = E_FAIL;
			}
			
			break;
	}

	if ( S_OK != hr )
	{
		WrightLogEx( pHHC->szIP, "HVAPI_ExecCmdEx Make XML Fail\n "  );
		SAFE_DELETE_ARG(pszXmlBuf);
		return E_FAIL;
	}
	if (!fIsXml)
	{
		szCmd = (char*)pszXmlBuf;
	}
	
	// 执行命令
	bool fRet = false;
	switch( pHHC->emProtocolVersion )
	{
		case PROTOCOL_VERSION_1:
		case PROTOCOL_VERSION_2:
		case PROTOCOL_VERSION_EARTH:
			fRet = ExecXmlExtCmdEx(pHHC->szIP, (char*)szCmd, (char*)szRetBuf, nBufLen, INVALID_SOCKET);
			break;
		case PROTOCOL_VERSION_MERCURY:
		default:
			fRet = ExecXmlExtCmdMercury(pHHC->szIP, (char*)szCmd, (char*)szRetBuf, nBufLen, INVALID_SOCKET);
			break;
	}

	if ( fRet)
	{
		WrightLogEx( pHHC->szIP, szRetBuf);
	}


	if ( NULL != pnRetLen )
	{
		*pnRetLen = nBufLen;
	}

	SAFE_DELETE_ARG(pszXmlBuf);
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
			SAFE_DELETE_ARG(rgchXmlParamBuf);
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
						SAFE_DELETE_ARG(rgchXmlParamBuf);
						return S_OK;
					}
					else
					{
						if ( pnRetLen )
						{
							*pnRetLen = cCmdRespond.dwInfoSize;
						}
						ForceCloseSocket(hSocketCmd);
						SAFE_DELETE_ARG(rgchXmlParamBuf);
						return E_FAIL;
					}
				}
			}
		}

		ForceCloseSocket(hSocketCmd);
	}
	SAFE_DELETE_ARG(rgchXmlParamBuf);
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

//计算截图位置大小
static HRESULT InflateCropRect(
			RECT& rectDst,
			RECT  cPlatePos,
			const int& iImgWidth,
			const int& iImgHeight,
			const int& iCropWidth,
			const int& iCropHeight,
			DWORD dwimgType = 0,
			DWORD dwNoneImage = 0
			)
{   if(cPlatePos.top < 101 && cPlatePos.bottom < 101 && cPlatePos.left < 101 && cPlatePos.right < 101)
	{
			cPlatePos.top = ((DWORD)(cPlatePos.top * iImgHeight))/100;
			cPlatePos.bottom = ((DWORD)(cPlatePos.bottom * iImgHeight))/100;
			cPlatePos.left = ((DWORD)(cPlatePos.left * iImgWidth))/100;
			cPlatePos.right = ((DWORD)(cPlatePos.right * iImgWidth))/100;
	}
	int iPosWidth = cPlatePos.right - cPlatePos.left;
	int iPosHeigh  = cPlatePos.bottom - cPlatePos.top;

	int iTemp = iCropWidth - iPosWidth;
	iTemp >>= 1;

	int iLeftDst = cPlatePos.left - iTemp;
	int iRightDst = cPlatePos.right + iTemp;
	iRightDst = iRightDst + (iCropWidth - (iRightDst - iLeftDst) );

	if( iCropWidth >= iImgWidth )
	{
		rectDst.left = 0;
		rectDst.right = iImgWidth;
	}
	else
	{
		if( iLeftDst < 0 )
		{
			iRightDst -= iLeftDst;
			iLeftDst = 0;
		}
		if( iRightDst > iImgWidth )
		{
			iLeftDst -= (iRightDst - iImgWidth);
			iRightDst = iImgWidth;
		}
		if( (iLeftDst & 1) == 1 )
		{
			iLeftDst -= 1;
			iRightDst -= 1;
		}
		rectDst.left = iLeftDst;
		rectDst.right = iRightDst;
	}
	int iTopDst;
	int iBottomDst;
	iTemp = iCropHeight - iPosHeigh;
	iTemp >>= 2;

	if(((DWORD)(cPlatePos.bottom - cPlatePos.top)*100) / iImgHeight >= 5)
	{	
		if(dwNoneImage == 1)
		{
			iTopDst = cPlatePos.top - (iTemp * 2);
			iBottomDst = cPlatePos.bottom + (iTemp * 2);
			iBottomDst =  iBottomDst + (iCropHeight - (iBottomDst - iTopDst));
		}
		else if(dwimgType == RECORD_BIGIMG_LAST_SNAPSHOT)
		{	
			if(iTemp > 0)
			{
				iTopDst = cPlatePos.top - iTemp + (int)(iImgHeight*0.4) ;
				iBottomDst = cPlatePos.bottom + (iTemp * 3) + (int)(iImgHeight*0.4);
				iBottomDst =  iBottomDst + (iCropHeight - (iBottomDst - iTopDst));
			}
			else
			{
				iTopDst = cPlatePos.top  + (int)(iImgHeight*0.4) ;
				iBottomDst = cPlatePos.bottom  + (int)(iImgHeight*0.4);
				iTopDst =  iTopDst - (iCropHeight - (iBottomDst - iTopDst));
			}
		}
		else
		{
			if(iTemp > 0)
			{
				if(iImgHeight > 1200)
				{
					iTopDst = cPlatePos.top - (iTemp * 2) + (int)(iImgHeight*0.15);
					iBottomDst = cPlatePos.bottom + (iTemp * 2) + (int)(iImgHeight*0.15);
				}
				else
				{
					iTopDst = cPlatePos.top - (iTemp * 2);// + (int)(iImgHeight*0.1);
					iBottomDst = cPlatePos.bottom + (iTemp * 2);// + (int)(iImgHeight*0.1);
				}
					iBottomDst =  iBottomDst + (iCropHeight - (iBottomDst - iTopDst));
			}
			else
			{
				if(iImgHeight > 1200)
				{
					iTopDst = cPlatePos.top + (int)(iImgHeight*0.15);
					iBottomDst = cPlatePos.bottom + (int)(iImgHeight*0.15);
				}
				else
				{
					iTopDst = cPlatePos.top;// + (int)(iImgHeight*0.1);
					iBottomDst = cPlatePos.bottom;// + (int)(iImgHeight*0.1);
				}
				iTopDst =  iTopDst - (iCropHeight - (iBottomDst - iTopDst));
			}
		}
	}
	else
	{
		iTopDst = cPlatePos.top - (iTemp * 3);
		iBottomDst = cPlatePos.bottom + iTemp;
		iBottomDst =  iBottomDst + (iCropHeight - (iBottomDst - iTopDst));
	}

	if( iCropHeight >= iImgHeight )
	{
		rectDst.top = 0;
		rectDst.bottom = iImgHeight;
	}
	else
	{
		if( iTopDst < 0 )
		{
			iBottomDst -= iTopDst;
			iTopDst = 0;
		}
		if( iBottomDst > iImgHeight )
		{
			iTopDst -= (iBottomDst - iImgHeight);
			iBottomDst = iImgHeight;
		}
		rectDst.top = iTopDst;
		rectDst.bottom = iBottomDst;
	}
	return S_OK;
}

HRESULT ProcBigImageCallBack(HVAPI_CALLBACK_SET* pCallBackSet, DWORD dwType, RECORD_IMAGE_EX* pcImage, BOOL fIsPeccancy,
							 DWORD dwEnhanceFlag, INT iBrightness, INT iHueThreshold,
							 INT iCompressRate, DWORD dwRecordType, DWORD64 dwTimeMS, HVAPI_HANDLE_CONTEXT_EX* pHHC)
{
	if(pCallBackSet == NULL || pcImage == NULL)
	{
		return E_FAIL;
	}

	RECT rcPlate = pcImage->cImgInfo.rcPlate;
	int nRedLightCount = 0;//pcImage->cImgInfo.
	RECT rcRedLightPos[20];
	memset( rcRedLightPos , 0 , sizeof(rcRedLightPos) );

	if( PROTOCOL_VERSION_1 == pHHC->emProtocolVersion
		|| PROTOCOL_VERSION_2 == pHHC->emProtocolVersion )
	{
		PCI_IMAGE_INFO cImgInfo;
		memset(&cImgInfo, 0, sizeof(cImgInfo));
		DWORD iBuffLen;
		iBuffLen = (sizeof(PCI_IMAGE_INFO) < pcImage->dwImgInfoLen) ? sizeof(PCI_IMAGE_INFO) : pcImage->dwImgInfoLen;
		memcpy(&cImgInfo, pcImage->pbImgInfo, iBuffLen);

		nRedLightCount = cImgInfo.nRedLightCount;
		memcpy( rcRedLightPos ,cImgInfo.rcRedLightPos , sizeof(rcRedLightPos) ); 
	}

	if((pHHC->CropImageEnable == 1) 
		&& (pHHC->CropImageHeight < pcImage->cImgInfo.dwHeight) 
		&& (pHHC->CropImageWeight < pcImage->cImgInfo.dwWidth)
		&& (pHHC->CropImageHeight > 0)
		&& (pHHC->CropImageWeight > 0))
	{
		WORD wNoneImage = 0;
		if(rcPlate.bottom == 0 && rcPlate.top == 0 && rcPlate.left == 0 && rcPlate.right == 0 && pHHC->NoneImageEnable == 1)
		{
			rcPlate.top = pHHC->NoneImageUpy;
			rcPlate.bottom = pHHC->NoneImageUpy + pHHC->CropImageHeight;
			rcPlate.left = pHHC->NoneImageUpx;
			rcPlate.right = pHHC->NoneImageUpx + pHHC->CropImageWeight;
			wNoneImage = 1;
		}

		IStream* pstm = NULL;
		CreateStreamOnHGlobal(NULL, TRUE, &pstm);
		LARGE_INTEGER liTemp = { 0 };
		ULARGE_INTEGER uliZero = { 0 };
		pstm->Seek(liTemp, STREAM_SEEK_SET, NULL );
		pstm->SetSize(uliZero);
		ULONG ulRealSize = 0;
		pstm->Write(pcImage->pbImgData, pcImage->dwImgDataLen, &ulRealSize);
		Bitmap* pbmp = Bitmap::FromStream(pstm);

		pcImage->cImgInfo.dwWidth = pbmp->GetWidth();
		pcImage->cImgInfo.dwHeight = pbmp->GetHeight();
		RECT rectImg;
		InflateCropRect( rectImg, rcPlate, (int)pcImage->cImgInfo.dwWidth, (int)pcImage->cImgInfo.dwHeight, (int)pHHC->CropImageWeight, (int)pHHC->CropImageHeight, dwType, wNoneImage);
		Bitmap bmpTmp((rectImg.right - rectImg.left), (rectImg.bottom - rectImg.top));
		Graphics grfTmp( &bmpTmp );

		grfTmp.DrawImage( pbmp, 0, 0, rectImg.left, rectImg.top, (rectImg.right - rectImg.left), (rectImg.bottom - rectImg.top), UnitPixel );
		BYTE* pTransBuf = new BYTE[_TRANSBUF_LEN];

		delete pbmp;
		pbmp = bmpTmp.Clone(0, 0, bmpTmp.GetWidth(), bmpTmp.GetHeight(), bmpTmp.GetPixelFormat());
		pcImage->cImgInfo.dwWidth = pbmp->GetWidth();
		pcImage->cImgInfo.dwHeight = pbmp->GetHeight();

		pstm->Seek(liTemp,STREAM_SEEK_SET,NULL);
		pstm->SetSize(uliZero);
		pbmp->Save(pstm, &g_jpgClsid);

		pstm->Seek(liTemp,STREAM_SEEK_SET,NULL);
		ULONG TmpLen = 0;
		pstm->Read(pTransBuf, _TRANSBUF_LEN, &TmpLen);
		memcpy(pcImage->pbImgData,pTransBuf,TmpLen);
		pcImage->dwImgDataLen = (DWORD)TmpLen;
		if(pstm != NULL)
		{
			pstm->Release();
			pstm = NULL;
		}
		if(pbmp !=NULL)
		{
			delete pbmp;
			pbmp = NULL;
		}
		delete[] pTransBuf;
	}
	
	if( nRedLightCount <=0 || dwEnhanceFlag == 0)
	{
			if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
				(WORD)dwType, pcImage->cImgInfo.dwWidth,
				pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
				pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
				}

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
 
					if( pCallBackSet->pOnBigImage )
					{
						pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
					}
				}
				HRESULT hr = HvEnhanceTrafficLight(pcImage->pbImgData, pcImage->dwImgDataLen,
					nRedLightCount, (PBYTE)&rcRedLightPos, pTempPic,
					dwRetBufLen, iBrightness, iHueThreshold, iCompressRate);
				if(hr == S_OK)
				{
					if ( pCallBackSet->pOnBigImage )
					{
						pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
							RECORD_BIGIMG_BEST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
							pcImage->cImgInfo.dwHeight, pTempPic,
							dwRetBufLen, dwRecordType, dwTimeMS);
					}

				}
				else
				{
 
					if(pCallBackSet->pOnBigImage)
					{
						pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
					}
					 

				}
				delete[] pTempPic;
				pTempPic = NULL;
			}
			else
			{
 
				if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
					RECORD_BIGIMG_BEST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
					pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
					pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
					}
			 

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
 
					if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
						}
					 
				}
				HRESULT hr = HvEnhanceTrafficLight(pcImage->pbImgData, pcImage->dwImgDataLen,
					nRedLightCount, (PBYTE)&rcRedLightPos, pTempPic,
					dwRetBufLen, iBrightness, iHueThreshold, iCompressRate);
				if(hr == S_OK)
				{
	 
					if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pTempPic,
						dwRetBufLen, dwRecordType, dwTimeMS);
						}
					 
				}
				else
				{
 
					if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
						}
			 

				}
				delete[] pTempPic;
				pTempPic = NULL;
			}
			else
			{
 
				if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
					RECORD_BIGIMG_LAST_SNAPSHOT, pcImage->cImgInfo.dwWidth,
					pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
					pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
					}
			 

			}
		}
		break;
	case RECORD_BIGIMG_BEGIN_CAPTURE:
		{
			if((dwEnhanceFlag == 1 && fIsPeccancy)
				|| dwEnhanceFlag == 2 || dwEnhanceFlag == 3
				||(dwEnhanceFlag == 4 && fIsPeccancy))
			{
				dwRetBufLen = dwRetBufLen << 10;
				pTempPic = new BYTE[dwRetBufLen];
				if(pTempPic == NULL)
				{
	 
					if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEGIN_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
						}
				 

				}
				HRESULT hr = HvEnhanceTrafficLight(pcImage->pbImgData, pcImage->dwImgDataLen,
					nRedLightCount, (PBYTE)&rcRedLightPos, pTempPic,
					dwRetBufLen, iBrightness, iHueThreshold, iCompressRate);
				if(hr == S_OK)
				{
 
					if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEGIN_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pTempPic,
						dwRetBufLen, dwRecordType, dwTimeMS);
						}
				 
				}
				else
				{
	 
					if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEGIN_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
						}
				 

				}
				delete[] pTempPic;
				pTempPic = NULL;
			}
			else
			{
 
				if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
					RECORD_BIGIMG_BEGIN_CAPTURE, pcImage->cImgInfo.dwWidth,
					pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
					pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
					}
				 

			}
		}
		break;
	case RECORD_BIGIMG_BEST_CAPTURE:
		{
			if((dwEnhanceFlag == 1 && fIsPeccancy)
				|| dwEnhanceFlag == 2 || dwEnhanceFlag == 3
				||(dwEnhanceFlag == 4 && fIsPeccancy))
			{
				dwRetBufLen = dwRetBufLen << 10;
				pTempPic = new BYTE[dwRetBufLen];
				if(pTempPic == NULL)
				{
	 
					if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
						}
				 

				}
				HRESULT hr = HvEnhanceTrafficLight(pcImage->pbImgData, pcImage->dwImgDataLen,
					nRedLightCount, (PBYTE)&rcRedLightPos, pTempPic,
					dwRetBufLen, iBrightness, iHueThreshold, iCompressRate);
				if(hr == S_OK)
				{
 
					if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pTempPic,
						dwRetBufLen, dwRecordType, dwTimeMS);
						}
				 

				}
				else
				{
	 
					if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_BEST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
						}
				 

				}
				delete[] pTempPic;
				pTempPic = NULL;
			}
			else
			{
				if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
					RECORD_BIGIMG_BEST_CAPTURE, pcImage->cImgInfo.dwWidth,
					pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
					pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
					}

			}
		}
		break;
	case RECORD_BIGIMG_LAST_CAPTURE:
		{
			if((dwEnhanceFlag == 1 && fIsPeccancy)
				|| dwEnhanceFlag == 2 || dwEnhanceFlag == 3
				||(dwEnhanceFlag == 4 && fIsPeccancy))
			{
				dwRetBufLen = dwRetBufLen << 10;
				pTempPic = new BYTE[dwRetBufLen];
				if(pTempPic == NULL)
				{
					if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
						}

				}
				HRESULT hr = HvEnhanceTrafficLight(pcImage->pbImgData, pcImage->dwImgDataLen,
					nRedLightCount, (PBYTE)&rcRedLightPos, pTempPic,
					dwRetBufLen, iBrightness, iHueThreshold, iCompressRate);
				if(hr == S_OK)
				{
					if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pTempPic,
						dwRetBufLen, dwRecordType, dwTimeMS);
						}

				}
				else
				{
					if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
						RECORD_BIGIMG_LAST_CAPTURE, pcImage->cImgInfo.dwWidth,
						pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
						pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
						}

				}
				delete[] pTempPic;
				pTempPic = NULL;
			}
			else
			{
				if(pCallBackSet->pOnBigImage){pCallBackSet->pOnBigImage(pCallBackSet->pOnBigImageParam, pcImage->cImgInfo.dwCarID,
					RECORD_BIGIMG_LAST_CAPTURE, pcImage->cImgInfo.dwWidth,
					pcImage->cImgInfo.dwHeight, pcImage->pbImgData,
					pcImage->dwImgDataLen, dwRecordType, dwTimeMS);
					}


			}
		}
		break;
	}


	SAFE_DELETE_ARG(pTempPic);
	return S_OK;
}

HRESULT GetResultImage(	RECORD_IMAGE_GROUP_EX* pImgGroup,
					   DWORD64 dw64TimeMS,
					   CImageInfo* pPlate,
					   CImageInfo* pPlateBin,
					   CImageInfo* pBestSnapshot,
					   CImageInfo* pLastSnapshot,
					   CImageInfo* pBeginCapture,
					   CImageInfo* pBestCapture,
					   CImageInfo* pLastCapture)
{
	if ( NULL == pImgGroup )
	{
		return E_POINTER;
	}
	if( NULL != pPlate )
	{
		pPlate->wSize = sizeof(CImageInfo);
		pPlate->wImgType = RECORD_BIGIMG_PLATE;
		pPlate->wWidth = pImgGroup->cPlatePicture.cImgInfo.dwWidth;
		pPlate->wHeight = pImgGroup->cPlatePicture.cImgInfo.dwHeight;
		pPlate->pbData = pImgGroup->cPlatePicture.pbImgData;
		pPlate->dwDataLen = pImgGroup->cPlatePicture.dwImgDataLen;
		pPlate->dw64TimeMS = pImgGroup->cPlatePicture.cImgInfo.dw64TimeMS;

	}
	if( NULL != pPlateBin )
	{
		pPlateBin->wSize = sizeof(CImageInfo);
		pPlateBin->wImgType = RECORD_BIGIMG_PLATE_BIN;
		pPlateBin->wWidth = pImgGroup->cPlateBinary.cImgInfo.dwWidth;
		pPlateBin->wHeight = pImgGroup->cPlateBinary.cImgInfo.dwHeight;
		pPlateBin->pbData = pImgGroup->cPlateBinary.pbImgData;
		pPlateBin->dwDataLen = pImgGroup->cPlateBinary.dwImgDataLen;
		pPlateBin->dw64TimeMS =  pImgGroup->cPlateBinary.cImgInfo.dw64TimeMS;
	}
	if( NULL != pBestSnapshot )
	{
		pBestSnapshot->wSize = sizeof(CImageInfo);
		pBestSnapshot->wImgType = RECORD_BIGIMG_BEST_SNAPSHOT;
		pBestSnapshot->wWidth = pImgGroup->cBestSnapshot.cImgInfo.dwWidth;
		pBestSnapshot->wHeight = pImgGroup->cBestSnapshot.cImgInfo.dwHeight;
		pBestSnapshot->pbData = pImgGroup->cBestSnapshot.pbImgData;
		pBestSnapshot->dwDataLen = pImgGroup->cBestSnapshot.dwImgDataLen;
		pBestSnapshot->dw64TimeMS =  pImgGroup->cBestSnapshot.cImgInfo.dw64TimeMS;
	}
	if( NULL != pLastSnapshot )
	{
		pLastSnapshot->wSize = sizeof(CImageInfo);
		pLastSnapshot->wImgType = RECORD_BIGIMG_LAST_SNAPSHOT;
		pLastSnapshot->wWidth = pImgGroup->cLastSnapshot.cImgInfo.dwWidth;
		pLastSnapshot->wHeight = pImgGroup->cLastSnapshot.cImgInfo.dwHeight;
		pLastSnapshot->pbData = pImgGroup->cLastSnapshot.pbImgData;
		pLastSnapshot->dwDataLen = pImgGroup->cLastSnapshot.dwImgDataLen;
		pLastSnapshot->dw64TimeMS =  pImgGroup->cLastSnapshot.cImgInfo.dw64TimeMS;
	}
	if( NULL != pBeginCapture )
	{
		pBeginCapture->wSize = sizeof(CImageInfo);
		pBeginCapture->wImgType = RECORD_BIGIMG_BEGIN_CAPTURE;
		pBeginCapture->wWidth = pImgGroup->cBeginCapture.cImgInfo.dwWidth;
		pBeginCapture->wHeight = pImgGroup->cBeginCapture.cImgInfo.dwHeight;
		pBeginCapture->pbData = pImgGroup->cBeginCapture.pbImgData;
		pBeginCapture->dwDataLen = pImgGroup->cBeginCapture.dwImgDataLen;
		pBeginCapture->dw64TimeMS =  pImgGroup->cBeginCapture.cImgInfo.dw64TimeMS;
	}
	if( NULL != pBestCapture )
	{
		pBestCapture->wSize = sizeof(CImageInfo);
		pBestCapture->wImgType = RECORD_BIGIMG_BEST_CAPTURE;
		pBestCapture->wWidth = pImgGroup->cBestCapture.cImgInfo.dwWidth;
		pBestCapture->wHeight = pImgGroup->cBestCapture.cImgInfo.dwHeight;
		pBestCapture->pbData = pImgGroup->cBestCapture.pbImgData;
		pBestCapture->dwDataLen = pImgGroup->cBestCapture.dwImgDataLen;
		pBestCapture->dw64TimeMS =  pImgGroup->cBestCapture.cImgInfo.dw64TimeMS;
	}
	if( NULL != pLastCapture )
	{
		pLastCapture->wSize = sizeof(CImageInfo);
		pLastCapture->wImgType = RECORD_BIGIMG_LAST_CAPTURE;
		pLastCapture->wWidth = pImgGroup->cLastCapture.cImgInfo.dwWidth;
		pLastCapture->wHeight = pImgGroup->cLastCapture.cImgInfo.dwHeight;
		pLastCapture->pbData = pImgGroup->cLastCapture.pbImgData;
		pLastCapture->dwDataLen = pImgGroup->cLastCapture.dwImgDataLen;
		pLastCapture->dw64TimeMS =  pImgGroup->cLastCapture.cImgInfo.dw64TimeMS;
	}

	return S_OK;
}



static HRESULT ModifyNoideInfo(LPCSTR pszXmlAppendInfo,  LPSTR pszNodeName, LPSTR pszNodeValue, LPSTR pszTempXmlBuff, INT iTempXmlSize )
{

	if( pszXmlAppendInfo == NULL ||pszNodeName == NULL || pszTempXmlBuff==NULL ||  iTempXmlSize  == 0 )
		return E_FAIL;

	TiXmlDocument cXmlDoc;

	if( cXmlDoc.Parse( pszXmlAppendInfo) == NULL )
		return E_FAIL;
	
	TiXmlElement* pRootElement = cXmlDoc.RootElement();
	if ( pRootElement == NULL )
		return E_FAIL;

	TiXmlElement* pElementResultSet = pRootElement->FirstChildElement("ResultSet");
	if (  pElementResultSet == NULL )
		return E_FAIL;

	TiXmlElement* pElementResult = pElementResultSet->FirstChildElement("Result");
	if ( pElementResult == NULL )
		return E_FAIL;

	TiXmlElement* pNewElement = pElementResult->FirstChildElement(pszNodeName);

	if ( pNewElement == NULL )
		return E_FAIL;


	TiXmlText *pValue = new TiXmlText(pszNodeValue);
	pNewElement->Clear();
	pNewElement->LinkEndChild(pValue);

	TiXmlPrinter cPrinter;
	cXmlDoc.Accept(&cPrinter);

	strncpy( pszTempXmlBuff, cPrinter.CStr(),  iTempXmlSize);
	return S_OK;

}


bool GetNodePointerByName(TiXmlElement* pRootEle,const std::string &strNodeName,TiXmlElement* &Node)
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
        std::map<std::string,std::string>::const_iterator iter;
        for (iter=AttMap.begin();iter!=AttMap.end();iter++)
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


HRESULT ResolveCaptureImage(HVAPI_HANDLE_CONTEXT_EX* pHHC, unsigned char* pbRecordData, 
	
							DWORD dwDataLen, DWORD dwRecordType, LPCSTR szAppendInfo)
{
	if ( pHHC->pCaputureImage != NULL)
	{
		delete[] pHHC->pCaputureImage;
		pHHC->pCaputureImage = NULL;
	}

	pHHC->iCaptureImgeSize = 0;

	RECORD_IMAGE_GROUP_EX cImgGroup;

	HRESULT hr =S_FALSE;

	if ( PROTOCOL_VERSION_1 != pHHC->emProtocolVersion 
		&& PROTOCOL_VERSION_2 != pHHC->emProtocolVersion )
	{
		// 解图片
		hr = HvGetRecordImage_Mercury(szAppendInfo, pbRecordData, dwDataLen, &cImgGroup);

	}
	else
	{
		hr = HvGetRecordImage(pbRecordData, dwDataLen, &cImgGroup);
	}

	if ( hr == S_OK )
	{
		BYTE *pImage = NULL;
		int iSize = 0;
		if(cImgGroup.cLastSnapshot.pbImgData)
		{
			pImage = cImgGroup.cLastSnapshot.pbImgData;
			iSize =  cImgGroup.cLastSnapshot.dwImgDataLen;
		}
		else if(cImgGroup.cBestSnapshot.pbImgData)
		{
			pImage = cImgGroup.cBestSnapshot.pbImgData;
			iSize =  cImgGroup.cBestSnapshot.dwImgDataLen;
		}

		if ( pImage != NULL )
		{
			pHHC->pCaputureImage = new BYTE[iSize+1];
			memset(pHHC->pCaputureImage, 0, iSize+1);

			memcpy(pHHC->pCaputureImage,  pImage, iSize);
			pHHC->iCaptureImgeSize = iSize;
			
		}
		else
			hr = S_FALSE;

		pHHC->fCapureFlag = false;

		return hr;
	}

	return hr;

}


// 结果数据包解析并抛送回调
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

	//如果调用了获取一张图片的函数(进入获取一张图片的 处理流程)
	if ( pHHC->fCapureFlag && pHHC->iCaptureImgeSize == 0 )
	{
		return ResolveCaptureImage(pHHC, pbRecordData, dwDataLen, dwRecordType, szAppendInfo);
	}


	DWORD dwCarID = 0, dwTimeHigh= 0, dwTimeLow = 0;
	DWORD dwBufLen = 50;
	char* pszPlateBuf = new char[dwBufLen];
	memset(pszPlateBuf, 0, dwBufLen);
	if(HVAPIUTILS_GetRecordInfoFromAppenedStringEx(szAppendInfo, "PlateName", pszPlateBuf, dwBufLen) != S_OK)
	{
		SAFE_DELETE_ARG(pszPlateBuf);
		return E_FAIL;
	}

	char* pszValue = new char[dwBufLen];
	memset(pszValue, 0, dwBufLen);
	if(HVAPIUTILS_GetRecordInfoFromAppenedStringEx(szAppendInfo, "CarID", pszValue, dwBufLen) != S_OK)
	{
		SAFE_DELETE_ARG(pszPlateBuf);
		SAFE_DELETE_ARG(pszValue);
		return E_FAIL;
	}
	dwCarID = atoi(pszValue);

	memset(pszValue, 0, dwBufLen);
	if(HVAPIUTILS_GetRecordInfoFromAppenedStringEx(szAppendInfo, "TimeHigh", pszValue, dwBufLen) != S_OK)
	{
		SAFE_DELETE_ARG(pszPlateBuf);
		SAFE_DELETE_ARG(pszValue);
		return E_FAIL;
	}
	dwTimeHigh = atoi(pszValue);
	
	memset(pszValue, 0, dwBufLen);
	if(HVAPIUTILS_GetRecordInfoFromAppenedStringEx(szAppendInfo, "TimeLow", pszValue, dwBufLen) != S_OK)
	{
		SAFE_DELETE_ARG(pszPlateBuf);
		SAFE_DELETE_ARG(pszValue);
		return E_FAIL;
	}
	dwTimeLow = atoi(pszValue);

	SAFE_DELETE_ARG(pszValue);

	HRESULT hr = S_OK;
	DWORD64 dw64TimeMS = 0;  //((DWORD64)(dwTimeHigh)<<32) | dwTimeLow;
	dw64TimeMS = dwTimeHigh;
	dw64TimeMS <<= 32;
	dw64TimeMS += dwTimeLow;
	CTime ctime(dw64TimeMS/1000);
	CString strTime;
	strTime.Format("%4d.%2d.%2d %2d:%2d:%2d", ctime.GetYear(), ctime.GetMonth(), ctime.GetDay(),
											ctime.GetHour(), ctime.GetMinute(), ctime.GetSecond());

	char chAppedInftoTemp[10240] = { 0 };
	strcpy(chAppedInftoTemp, szAppendInfo);

	//后处理车牌号码 xurui
	//进行车牌替换
	EnterCriticalSection(&g_csFilterSync);
	if(g_fFilterValid == 1)
	{
		RESULT_INFO resultInfo;
		resultInfo.strPlate =  pszPlateBuf;
		int iInfoLen = 4096;
		char *szInfo = new char[iInfoLen];

		if ( szInfo != NULL )
		{
			memset(szInfo, 0, iInfoLen);
			HVAPIUTILS_ParsePlateXmlStringEx(szAppendInfo, szInfo, iInfoLen);
			
			resultInfo.strOther = szInfo;
		}

		if(g_ResultFilter.FilterProcess(&resultInfo, TRUE, FALSE, FALSE))
		{
			if ( strcmp(pszPlateBuf, resultInfo.strPlate) != 0 )
			{
				memset(pszPlateBuf, 0, dwBufLen);
				strncpy(pszPlateBuf, resultInfo.strPlate.GetBuffer(), dwBufLen);
				ModifyNoideInfo(szAppendInfo, "PlateName", pszPlateBuf, chAppedInftoTemp, 10240);
			}
		}
		delete[] szInfo;
	}
	LeaveCriticalSection(&g_csFilterSync);

	HVAPI_CALLBACK_SET* pTemp;
	// 首先抛送结果开始回调和车牌信息回调
	for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
	{
		if(pTemp->pOnRecordBegin)
		{
			pTemp->pOnRecordBegin(pTemp->pOnRecordBeginParam, dwCarID);
		}

		//if(pTemp->pOnPlate)
		//{
		//	pTemp->pOnPlate(pTemp->pOnPlateParam, dwCarID, pszPlateBuf, chAppedInftoTemp, dwRecordType, dw64TimeMS);
		//}
	}

	// 无图片等数据则抛送结束回调
	if(pbRecordData == NULL || dwDataLen == 0)
	{
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnRecordEnd)
			{
				pTemp->pOnRecordEnd(pTemp->pOnRecordEndParam, dwCarID);
			}
			//发送数据异常标志
			if (pTemp->pOnRecordResult)
			{
				CImageInfo tPlate;
				CImageInfo tPlateBin;
				CImageInfo tBestSnapshot;
				CImageInfo tLastSnapshot;
				CImageInfo tBeginCapture;
				CImageInfo tBestCapture;
				CImageInfo tLastCapture;

				tPlate.fHasData  = false;
				tPlateBin.fHasData  = false;
				tBestSnapshot.fHasData  = false;
				tLastSnapshot.fHasData  = false;
				tBeginCapture.fHasData  = false;
				tBestCapture.fHasData  = false;
				tLastCapture.fHasData  = false;

				pTemp->pOnRecordResult(
					pTemp->pOnPlateParam, 
					RESULT_FLAG_VAIL, 
					dwRecordType, 
					dwCarID,
					pszPlateBuf, chAppedInftoTemp, dw64TimeMS ,
					tPlate,
					tPlateBin,
					tBestSnapshot,
					tLastSnapshot,
					tBeginCapture,
					tBestCapture,
					tLastCapture
					);
				/*

				pTemp->pOnRecordResult(
					pTemp->pOnPlateParam, 
					RESULT_FLAG_INVAIL, 
					0, 
					0,
					NULL, NULL, 0 ,
					tPlate,
					tPlateBin,
					tBestSnapshot,
					tLastSnapshot,
					tBeginCapture,
					tBestCapture,
					tLastCapture);
				*/
			}
		}
		SAFE_DELETE_ARG(pszPlateBuf);
		//return S_FALSE;
		return S_OK;
	}
	
	RECORD_IMAGE_GROUP_EX cImgGroup;
	if ( PROTOCOL_VERSION_1 != pHHC->emProtocolVersion 
		&& PROTOCOL_VERSION_2 != pHHC->emProtocolVersion )
	{
		// 解图片
		if(HvGetRecordImage_Mercury(chAppedInftoTemp, pbRecordData, dwDataLen, &cImgGroup) != S_OK)
		{

			//车牌信息回调
			for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
			{
				if(pTemp->pOnPlate)
				{
					pTemp->pOnPlate(pTemp->pOnPlateParam, dwCarID, pszPlateBuf, chAppedInftoTemp, dwRecordType, dw64TimeMS);
				}
			}



			WrightLogEx(pHHC->szIP, "Get Picture From Result Failed...");
			for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
			{
				if(pTemp->pOnRecordEnd)
				{
					pTemp->pOnRecordEnd(pTemp->pOnRecordEndParam, dwCarID);
				}
				//发送数据异常标志
				if (pTemp->pOnRecordResult)
				{
					CImageInfo tPlate;
					CImageInfo tPlateBin;
					CImageInfo tBestSnapshot;
					CImageInfo tLastSnapshot;
					CImageInfo tBeginCapture;
					CImageInfo tBestCapture;
					CImageInfo tLastCapture;

					tPlate.fHasData  = false;
					tPlateBin.fHasData  = false;
					tBestSnapshot.fHasData  = false;
					tLastSnapshot.fHasData  = false;
					tBeginCapture.fHasData  = false;
					tBestCapture.fHasData  = false;
					tLastCapture.fHasData  = false;

					pTemp->pOnRecordResult(
						pTemp->pOnPlateParam, 
						RESULT_FLAG_INVAIL, 
						0, 
						0,
						NULL, NULL, 0 ,
						tPlate,
						tPlateBin,
						tBestSnapshot,
						tLastSnapshot,
						tBeginCapture,
						tBestCapture,
						tLastCapture);
				}
			}
			SAFE_DELETE_ARG(pszPlateBuf);
			return E_FAIL;
		}

		// TODO: 从szAppendInfo中删除ResultExtInfo结点，避免NAVI无法解析

	}
	else
	{
		if(HvGetRecordImage(pbRecordData, dwDataLen, &cImgGroup) != S_OK)
		{
			//车牌信息回调
			for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
			{
				if(pTemp->pOnPlate)
				{
					pTemp->pOnPlate(pTemp->pOnPlateParam, dwCarID, pszPlateBuf, chAppedInftoTemp, dwRecordType, dw64TimeMS);
				}
			}

			WrightLogEx(pHHC->szIP, "Get Picture Frome Result Faile...");
			for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
			{
				if(pTemp->pOnRecordEnd)
				{
					pTemp->pOnRecordEnd(pTemp->pOnRecordEndParam, dwCarID);
				}
				//发送数据异常标志
				if (pTemp->pOnRecordResult)
				{
					CImageInfo tPlate;
					CImageInfo tPlateBin;
					CImageInfo tBestSnapshot;
					CImageInfo tLastSnapshot;
					CImageInfo tBeginCapture;
					CImageInfo tBestCapture;
					CImageInfo tLastCapture;

					tPlate.fHasData  = false;
					tPlateBin.fHasData  = false;
					tBestSnapshot.fHasData  = false;
					tLastSnapshot.fHasData  = false;
					tBeginCapture.fHasData  = false;
					tBestCapture.fHasData  = false;
					tLastCapture.fHasData  = false;

					pTemp->pOnRecordResult(
						pTemp->pOnPlateParam, 
						RESULT_FLAG_INVAIL, 
						0, 
						0,
						NULL, NULL, 0 ,
						tPlate,
						tPlateBin,
						tBestSnapshot,
						tLastSnapshot,
						tBeginCapture,
						tBestCapture,
						tLastCapture);
				}
			}
			SAFE_DELETE_ARG(pszPlateBuf);
			return E_FAIL;
		}
	}
	
	BOOL fIsPeccancy = FALSE;
	if(strstr(chAppedInftoTemp, "违章:是"))
	{
		fIsPeccancy = TRUE;
	}



	#ifdef _CARFACE

	//对车脸进行识别
	if (pHHC->fEnableCarFaceCtrl)
	{
		if (pHHC->pCarFaceRecogCtrl == NULL )
		{
			CCarFaceCtrl *pCarFaceRecogCtrl = new  CCarFaceCtrl();

			if ( pCarFaceRecogCtrl->InitCtrl() == S_OK )
			{
				pHHC->pCarFaceRecogCtrl = pCarFaceRecogCtrl;
				//WrightLogEx(hContextHandle->szIP, "Init FaceCtrl Success");
			}
			else
			{
				delete pCarFaceRecogCtrl;
				pCarFaceRecogCtrl = NULL;
				//WrightLogEx(hContextHandle->szIP, "Inint FaceCtrl falit ");
			}
		}


		if ( pHHC->pCarFaceRecogCtrl != NULL )
		{
			RECORD_IMAGE_EX *pImageRecordEx = NULL;
			if ( cImgGroup.cLastSnapshot.pbImgData != NULL )
				pImageRecordEx = &(cImgGroup.cLastSnapshot);
			else if (cImgGroup.cBestSnapshot.pbImgData != NULL)
				pImageRecordEx = &(cImgGroup.cBestSnapshot);


		    if ( pImageRecordEx != NULL )   // 有图片进行车脸识别
			{
				sv::CSvImage *imgSamp = new sv::CSvImage();

				imgSamp->Create(sv::SV_IMAGE_YUV422, pImageRecordEx->cImgInfo.dwWidth, pImageRecordEx->cImgInfo.dwHeight);
				sv::utReadImage_Jpeg(pImageRecordEx->pbImgData, pImageRecordEx->dwImgDataLen, imgSamp, NULL, NULL);


				sv::SV_RECT platePos = { 0 };
				sv::SV_BOOL fYellowPlate = false;
				sv::SV_RECT carFacePos = {0};
				sv::SV_UINT32 regID[10] ={0};


				platePos.m_nLeft = (pImageRecordEx->cImgInfo.rcPlate.left * pImageRecordEx->cImgInfo.dwWidth) / 100;
				platePos.m_nRight = (pImageRecordEx->cImgInfo.rcPlate.right * pImageRecordEx->cImgInfo.dwWidth) / 100;
				platePos.m_nTop = (pImageRecordEx->cImgInfo.rcPlate.top * pImageRecordEx->cImgInfo.dwHeight) / 100;
				platePos.m_nBottom = (pImageRecordEx->cImgInfo.rcPlate.bottom * pImageRecordEx->cImgInfo.dwHeight) / 100;


				char szLog[1024] = { 0 };
				sprintf(szLog, "[%d,%d,%d,%d]", platePos.m_nLeft, platePos.m_nRight, platePos.m_nTop, platePos.m_nBottom);
				WrightLogEx(pHHC->szIP, szLog);

				char szCarFaceName[1024] = { 0 };
				char szCarType[1024] = { 0 };

				//  是否黄牌

				bool fYellow = FALSE;
				if ( strncmp(pszPlateBuf,"黄", 2) == 0 )
					fYellow = TRUE;

				//场景判断


				//是否无牌车
				bool fNoPlate = FALSE;
				if (  strstr(pszPlateBuf, "无牌车") != NULL )
					fNoPlate = TRUE;


				//车脸识别
				if ( pHHC->pCarFaceRecogCtrl->RecogCarFace(imgSamp,FALSE,fNoPlate,&platePos,fYellow, &carFacePos,10, regID) == S_OK )
				{
					pHHC->pCarFaceRecogCtrl->FindCarFaceString(regID[0], szCarFaceName, szCarType);
					WrightLogEx(pHHC->szIP, szCarFaceName);
					WrightLogEx(pHHC->szIP, szCarType);

					//构造附加信息，加入车脸信息
					TiXmlDocument doc;
					doc.Parse(chAppedInftoTemp); 

					std::map<std::string,std::string> AttMap;
				
					char szValue[256] = {0};
					sprintf(szValue, "%s",szCarFaceName);
					AttMap.clear();

					std::string Name="value";
					std::string value=szValue;
					AttMap.insert( make_pair(Name, value));

					Name="chnname";
					value="车标信息";
					AttMap.insert( make_pair(Name, value));

					//增加车辆类型
					AddNode_Attribute(doc,"Result","ModelsInfo",AttMap);

					//增加车型大小
					std::string szNameValue = szCarType;
					if ( ModifyNodeInfo(doc, "CarType", szNameValue ) != 0 ) //说明， 不存在着此节点， 需修改
					{
						AttMap.clear();
						std::string Name="value";
						std::string value=szNameValue;
						AttMap.insert( make_pair(Name, value));

						Name="chnname";
						value="车辆类型";
						AttMap.insert( make_pair(Name, value));
						AddNode_Attribute(doc,"Result","CarType",AttMap);

					}


					TiXmlPrinter cPrinter;
					doc.Accept(&cPrinter);

					strcpy( chAppedInftoTemp, cPrinter.CStr());

					WrightLogEx(pHHC->szIP, chAppedInftoTemp);


					

				}
				else
				{
					WrightLogEx(pHHC->szIP, "RecogCarFace failt");
				}
				delete imgSamp;
				imgSamp = NULL;

			}
		}
	}

#endif


	//车牌信息回调
	for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
	{
		if(pTemp->pOnPlate)
		{
			WrightLogEx(pHHC->szIP, chAppedInftoTemp);

			pTemp->pOnPlate(pTemp->pOnPlateParam, dwCarID, pszPlateBuf, chAppedInftoTemp, dwRecordType, dw64TimeMS);
		}
	}


	// 抛送大图回调
	for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
	{
		if(pTemp->iVideoID == 0)
		{
			if(pTemp->pOnBigImage != NULL  ||  pTemp->pOnRecordResult != NULL )
			{
				if(cImgGroup.cBestSnapshot.pbImgData)
				{
					pHHC->rcPlate[0] = cImgGroup.cBestSnapshot.cImgInfo.rcPlate;
					pHHC->nFaceCount[0] = cImgGroup.cBestSnapshot.cImgInfo.nFaceCount;
					memcpy(pHHC->rcFacePos[0], cImgGroup.cBestSnapshot.cImgInfo.rcFacePos, sizeof(pHHC->rcFacePos[0]));
					
					cImgGroup.cBestSnapshot.cImgInfo.dwCarID = dwCarID;
					ProcBigImageCallBack(pTemp, RECORD_BIGIMG_BEST_SNAPSHOT, &cImgGroup.cBestSnapshot, fIsPeccancy,
						pHHC->dwEnhanceRedLightFlag, pHHC->iBigPicBrightness, pHHC->iBigPicHueThrshold,
						pHHC->iBigPicCompressRate, dwRecordType, cImgGroup.cBestSnapshot.cImgInfo.dw64TimeMS, pHHC);
				}

				if(cImgGroup.cLastSnapshot.pbImgData)
				{
					pHHC->rcPlate[1] = cImgGroup.cLastSnapshot.cImgInfo.rcPlate;
					pHHC->nFaceCount[1] = cImgGroup.cLastSnapshot.cImgInfo.nFaceCount;
					memcpy(pHHC->rcFacePos[1], cImgGroup.cLastSnapshot.cImgInfo.rcFacePos, sizeof(pHHC->rcFacePos[1]));

					cImgGroup.cLastSnapshot.cImgInfo.dwCarID = dwCarID;
					ProcBigImageCallBack(pTemp, RECORD_BIGIMG_LAST_SNAPSHOT, &cImgGroup.cLastSnapshot, fIsPeccancy,
						pHHC->dwEnhanceRedLightFlag, pHHC->iBigPicBrightness, pHHC->iBigPicHueThrshold,
						pHHC->iBigPicCompressRate, dwRecordType, cImgGroup.cLastSnapshot.cImgInfo.dw64TimeMS, pHHC);
				}

				if(cImgGroup.cBeginCapture.pbImgData)
				{
					pHHC->rcPlate[2] = cImgGroup.cBeginCapture.cImgInfo.rcPlate;
					pHHC->nFaceCount[2] = cImgGroup.cBeginCapture.cImgInfo.nFaceCount;
					memcpy(pHHC->rcFacePos[2], cImgGroup.cBeginCapture.cImgInfo.rcFacePos, sizeof(pHHC->rcFacePos[2]));
					
					cImgGroup.cBeginCapture.cImgInfo.dwCarID = dwCarID;
					ProcBigImageCallBack(pTemp, RECORD_BIGIMG_BEGIN_CAPTURE, &cImgGroup.cBeginCapture, fIsPeccancy,
						pHHC->dwEnhanceRedLightFlag, pHHC->iBigPicBrightness, pHHC->iBigPicHueThrshold,
						pHHC->iBigPicCompressRate, dwRecordType, cImgGroup.cBeginCapture.cImgInfo.dw64TimeMS, pHHC);
				}

				if(cImgGroup.cBestCapture.pbImgData)
				{
					pHHC->rcPlate[3] = cImgGroup.cBestCapture.cImgInfo.rcPlate;
					pHHC->nFaceCount[3] = cImgGroup.cBestCapture.cImgInfo.nFaceCount;
					memcpy(pHHC->rcFacePos[3], cImgGroup.cBestCapture.cImgInfo.rcFacePos, sizeof(pHHC->rcFacePos[3]));
					
					cImgGroup.cBestCapture.cImgInfo.dwCarID = dwCarID;
					ProcBigImageCallBack(pTemp, RECORD_BIGIMG_BEST_CAPTURE, &cImgGroup.cBestCapture, fIsPeccancy,
						pHHC->dwEnhanceRedLightFlag, pHHC->iBigPicBrightness, pHHC->iBigPicHueThrshold,
						pHHC->iBigPicCompressRate, dwRecordType, cImgGroup.cBestCapture.cImgInfo.dw64TimeMS, pHHC);
				}

				if(cImgGroup.cLastCapture.pbImgData)
				{
					pHHC->rcPlate[4] = cImgGroup.cLastCapture.cImgInfo.rcPlate;
					pHHC->nFaceCount[4] = cImgGroup.cLastCapture.cImgInfo.nFaceCount;
					memcpy(pHHC->rcFacePos[4], cImgGroup.cLastCapture.cImgInfo.rcFacePos, sizeof(pHHC->rcFacePos[4]));
					
					cImgGroup.cLastCapture.cImgInfo.dwCarID = dwCarID;
					ProcBigImageCallBack(pTemp, RECORD_BIGIMG_LAST_CAPTURE, &cImgGroup.cLastCapture, fIsPeccancy,
						pHHC->dwEnhanceRedLightFlag, pHHC->iBigPicBrightness, pHHC->iBigPicHueThrshold,
						pHHC->iBigPicCompressRate, dwRecordType, cImgGroup.cLastCapture.cImgInfo.dw64TimeMS, pHHC);
				}
			}
		}
	}

	// 抛送车牌小图回调
	if(cImgGroup.cPlatePicture.pbImgData)
	{
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnSmallImage)
			{
				cImgGroup.cPlatePicture.cImgInfo.dwCarID = dwCarID;

				pTemp->pOnSmallImage(pTemp->pOnSmallImageParam, cImgGroup.cPlatePicture.cImgInfo.dwCarID,
					cImgGroup.cPlatePicture.cImgInfo.dwWidth, cImgGroup.cPlatePicture.cImgInfo.dwHeight,
					cImgGroup.cPlatePicture.pbImgData, cImgGroup.cPlatePicture.dwImgDataLen, dwRecordType, dw64TimeMS);
			}
		}
	}

	// 抛送车牌二值图回调
	if(cImgGroup.cPlateBinary.pbImgData)
	{
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnBinaryImage)
			{
				cImgGroup.cPlateBinary.cImgInfo.dwCarID = dwCarID;

				pTemp->pOnBinaryImage(pTemp->pOnBinaryImageParam, cImgGroup.cPlateBinary.cImgInfo.dwCarID,
					cImgGroup.cPlateBinary.cImgInfo.dwWidth, cImgGroup.cPlateBinary.cImgInfo.dwHeight,
					cImgGroup.cPlateBinary.pbImgData, cImgGroup.cPlateBinary.dwImgDataLen, dwRecordType, dw64TimeMS);
			}
		}
	}

	for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
	{
		WrightLogEx( "发送结果数据","" );
		//发送结果数据
		if (pTemp->pOnRecordResult)
		{
			WrightLogEx( "new Result\n","" );
			CImageInfo tPlate;
			CImageInfo tPlateBin;
			CImageInfo tBestSnapshot;
			CImageInfo tLastSnapshot;
			CImageInfo tBeginCapture;
			CImageInfo tBestCapture;
			CImageInfo tLastCapture;
			
			//获取结果图片数据
			GetResultImage( &cImgGroup , dw64TimeMS 
							,&tPlate
							,&tPlateBin
							,&tBestSnapshot
							,&tLastSnapshot
							,&tBeginCapture
							,&tBestCapture
							,&tLastCapture);
		
			tPlate.fHasData  =  (NULL != tPlate.pbData)? true : false;
			tPlateBin.fHasData  =  (NULL != tPlateBin.pbData)? true : false;
			tBestSnapshot.fHasData  =  (NULL != tBestSnapshot.pbData)? true : false;
			tLastSnapshot.fHasData  =  (NULL != tLastSnapshot.pbData)? true : false;
			tBeginCapture.fHasData  =  (NULL != tBeginCapture.pbData)? true : false;
			tBestCapture.fHasData  =  (NULL != tBestCapture.pbData)? true : false;
			tLastCapture.fHasData  =  (NULL != tLastCapture.pbData)? true : false;

			WrightLogEx(pHHC->szIP, "HHHHH结果回调开始");
			DWORD	dwRecordStreamTickBegin = GetTickCount();
			pTemp->pOnRecordResult(
				pTemp->pOnPlateParam, 
				RESULT_FLAG_VAIL, 
				dwRecordType, 
				dwCarID,
				pszPlateBuf, chAppedInftoTemp, dw64TimeMS ,
				tPlate,
				tPlateBin,
				tBestSnapshot,
				tLastSnapshot,
				tBeginCapture,
				tBestCapture,
				tLastCapture
				);
			DWORD	dwRecordStreamTickEnd = GetTickCount();
			pHHC->dwRecordStreamTick += (dwRecordStreamTickEnd - dwRecordStreamTickBegin);
			WrightLogEx(pHHC->szIP, "HHHHH结果回调结束");
		}

		//解析违法视频并发送,20140806
		if (pTemp->pOnIllegalVideo 
			&& cImgGroup.cIllegalVideo.dwVideoDataLen > 0 
			&& cImgGroup.cIllegalVideo.pbVideoData != NULL)
		{
			int iTempBufferLen = cImgGroup.cIllegalVideo.dwVideoDataLen;
			PBYTE pPacketData = cImgGroup.cIllegalVideo.pbVideoData;
			DWORD iFrameType = VIDEO_TYPE_UNKNOWN;
			DWORD iFrameLen = 0;
			DWORD32 dwType = VIDEO_TYPE_UNKNOWN;

			while(iTempBufferLen > 0)
			{
				iFrameType = *(int*)pPacketData;
				pPacketData += 4;
				iFrameLen = *(int*)pPacketData;
				pPacketData += 4;
				if(iFrameType == 4096) dwType = VIDEO_TYPE_H264_HISTORY_I;
				else if(iFrameType == 4097) dwType = VIDEO_TYPE_H264_HISTORY_P;
				
				if (pTemp->pOnIllegalVideo)
				{
					pTemp->pOnIllegalVideo(pTemp->pOnIllegalVideoParam
						, cImgGroup.cIllegalVideo.dwCarID
						, dwType
						, cImgGroup.cIllegalVideo.dwWidth
						, cImgGroup.cIllegalVideo.dwHeight
						, cImgGroup.cIllegalVideo.dw64TimeMS
						, pPacketData
						, iFrameLen
						, NULL
						);
				}
				pPacketData += iFrameLen;
				iTempBufferLen = iTempBufferLen - 8 - iFrameLen;
			}
			if (pTemp->pOnIllegalVideo)
			{
				pTemp->pOnIllegalVideo(pTemp->pOnIllegalVideoParam
					, H264_FLAG_HISTROY_END
					, VIDEO_TYPE_UNKNOWN
					, 0
					, 0
					, 0
					, NULL
					, 0
					, NULL
					);
			}
		}
		//结束解析违章录像
		

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
	SAFE_DELETE_ARG(pszPlateBuf);
	return S_OK;
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
			WrightLogEx("GetCameraString Fail1", pXml );
			return E_FAIL; 
		}
		const char* pText = pRootElement->GetText();
		if ( pText )
		{
			int iStringLen = strlen( pText );
			if( iStringLen < *piSting )
			{
				WrightLogEx("GetCameraString OK", pXml );
				strcpy( pbString , pRootElement->GetText() );
				*piSting = iStringLen;
				return S_OK;
			}
		}

	}
	return E_FAIL;
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
	pHHC->AutoLinkSetCallBackTime = 0;
	while(!pHHC->fIsThreadRecvRecordExit)
	{
		if(pHHC->dwRecordConnStatus == CONN_STATUS_NORMAL)
		{
			if(GetTickCount() - pHHC->dwRecordStreamTick > 8000)
			{
				WrightLogEx(pHHC->szIP, "[RecordLink] Reconnect,Last Result Proces TimeOut...");
				pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
				if (pHHC->fAutoLink)
				{
					pHHC->AutiLinkOverTime = time(NULL);
				}
			}
		}

		if(pHHC->dwRecordConnStatus == CONN_STATUS_RECONN
			|| pHHC->dwRecordConnStatus == CONN_STATUS_DISCONN)
		{
			if (pHHC->fAutoLink)
			{
				ForceCloseSocket(pHHC->sktRecord);
				if(pHHC->AutiLinkOverTime != 0
					&& pHHC->AutiLinkOverTime + 300 < time(NULL))
				{
					pHHC->dwRecordConnStatus = CONN_STATUS_CONNOVERTIME;
				}
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
				if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY) //TODO(liyh)  当前是判断水星 金星 的情况下，不发心跳包回馈；   不过当前用协议判断不太好
				{
					//
				}
				else
				{
					if(send(pHHC->sktRecord, (char*)&cInfoHeaderResponse, 
						sizeof(cInfoHeaderResponse), 0) != sizeof(cInfoHeaderResponse))
					{
						WrightLogEx(pHHC->szIP, "[RecordLink] Reconnect,Throb Response Send Failed...");
						if (pHHC->fAutoLink)
						{
							if(pHHC->dwRecordConnStatus != CONN_STATUS_RECONN
								&& pHHC->AutiLinkOverTime == 0)
							{
								pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
								pHHC->AutiLinkOverTime = time(NULL);
							}
						}
						else
						{
							pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
						}
					}
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

				
				HVAPI_CALLBACK_SET* pTemp;
				for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
				{
					//发送历史结果结束标志
					if (pTemp->pOnRecordResult)
					{
						WrightLogEx("发送历史结果结束标志 OK \n" , "");

						CImageInfo tPlate;
						CImageInfo tPlateBin;
						CImageInfo tBestSnapshot;
						CImageInfo tLastSnapshot;
						CImageInfo tBeginCapture;
						CImageInfo tBestCapture;
						CImageInfo tLastCapture;

						tPlate.fHasData  = false;
						tPlateBin.fHasData  = false;
						tBestSnapshot.fHasData  = false;
						tLastSnapshot.fHasData  = false;
						tBeginCapture.fHasData  = false;
						tBestCapture.fHasData  = false;
						tLastCapture.fHasData  = false;

						pTemp->pOnRecordResult(
							pTemp->pOnPlateParam, 
							RESULT_FLAG_HISTROY_END, 
							0, 
							0,
							NULL, NULL, 0 ,
							tPlate,
							tPlateBin,
							tBestSnapshot,
							tLastSnapshot,
							tBeginCapture,
							tBestCapture,
							tLastCapture);
					}
				}

				break;
			}

			DWORD32 dwBuffLen = 20 * 1024 * 1024;
			if (cInfoHeader.dwInfoLen > dwBuffLen || cInfoHeader.dwDataLen > dwBuffLen)
			{
				char szDebug[256];
				sprintf( szDebug , "===[RecordLink] Quit,Recive Data Error...Info:%d,Data: %d, Type: %d ===\n ",cInfoHeader.dwInfoLen , cInfoHeader.dwDataLen ,  cInfoHeader.dwType );
				WrightLogEx( pHHC->szIP , szDebug  );

				if (pHHC->fAutoLink)
				{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
					pHHC->fVailPackResumeCache = FALSE;
					ForceCloseSocket(pHHC->sktRecord);
					pHHC->fIsThreadRecvRecordExit = TRUE;
					pHHC->hThreadRecvRecord = NULL;
					break;
				}
				else
				{
					pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
				}
				continue;
			}

			pbInfo = new unsigned char[cInfoHeader.dwInfoLen + 1];
			pbData = new unsigned char[cInfoHeader.dwDataLen + 1];
			int ipbInfoSize = (int)cInfoHeader.dwInfoLen + 1;

			if(pbInfo == NULL || pbData == NULL)
			{
				Sleep(100);
				WrightLogEx(pHHC->szIP, "[RecordLink] Reconnect,Malloc Buffer Failed...");
				if (pHHC->fAutoLink)
				{
					if(pHHC->dwRecordConnStatus != CONN_STATUS_RECONN
						&& pHHC->AutiLinkOverTime == 0)
					{
						pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
						pHHC->AutiLinkOverTime = time(NULL);
					}
				}
				else
				{
					pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
				}
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
					if (pHHC->fAutoLink)
					{
						if(pHHC->dwRecordConnStatus != CONN_STATUS_RECONN
							&& pHHC->AutiLinkOverTime == 0)
						{
							pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
							pHHC->AutiLinkOverTime = time(NULL);
						}
					}
					else
					{
						pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
					}
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
					if (pHHC->fAutoLink)
					{
						if(pHHC->dwRecordConnStatus != CONN_STATUS_RECONN
							&& pHHC->AutiLinkOverTime == 0)
						{
							pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
							pHHC->AutiLinkOverTime = time(NULL);
						}
					}
					else
					{
						pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
					}
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

					if ( PROTOCOL_VERSION_1 != pHHC->emProtocolVersion  
						&& PROTOCOL_VERSION_2 != pHHC->emProtocolVersion )
					{
						// 从info中解出TransmittingType
						TiXmlDocument cXmlDoc;
						if (cXmlDoc.Parse((const char*)pbInfo))
						{
							TiXmlElement* pEleRoot = cXmlDoc.RootElement();
							if (NULL == pEleRoot)
							{
								WrightLogEx(pHHC->szIP, "RecordRecvThreadFuncEx pEleRoot == NULL");
								continue; 
							}

							TiXmlElement* pExtInfoEle = pEleRoot->FirstChildElement("ResultExtInfo");
							if (NULL == pExtInfoEle)
							{
								WrightLogEx(pHHC->szIP, "RecordRecvThreadFuncEx pExtInfoEle == NULL");
								continue; 
							}

							TiXmlElement* pTypeEle = pExtInfoEle->FirstChildElement("TransmittingType");
							if (NULL == pTypeEle)
							{
								WrightLogEx(pHHC->szIP, "RecordRecvThreadFuncEx pTypeEle == NULL");
								continue; 
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
								WrightLogEx(pHHC->szIP, "RecordRecvThreadFuncEx pbInfo contains no TransmittingType");
								continue; 
							}
						}
						else
						{
							WrightLogEx(pHHC->szIP, "RecordRecvThreadFuncEx pbInfo can't be parsed");
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
							ProcRecordDataPackt(pHHC, pRecordData, nRecordDataLen, dwType, (const char*)pbInfo);					
						}
					}
					else
					{
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
							if (pHHC->fAutoLink)
							{
								if(pHHC->dwRecordConnStatus != CONN_STATUS_RECONN
									&& pHHC->AutiLinkOverTime == 0)
								{
									pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
									pHHC->AutiLinkOverTime = time(NULL);
								}
							}
							else
							{
								pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
							}
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
						SAFE_DELETE_ARG(szAppendInfo);
					}
				}
				else if(cInfoHeader.dwType == CAMERA_STRING)
				{
					HVAPI_CALLBACK_SET* pTemp;
					for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
					{
						if(pTemp->pOnString)
						{
							//新协议为XML组包
							char szStrBuf[512];
							int iStrBufLen = 512;
							if ( PROTOCOL_VERSION_1 != pHHC->emProtocolVersion  
								&& PROTOCOL_VERSION_2 != pHHC->emProtocolVersion )
							{
								GetCameraString( (const char*)pbData , szStrBuf , &iStrBufLen );
							}
							else 
							{
								if( cInfoHeader.dwDataLen < iStrBufLen )
								{
									strcpy( szStrBuf ,  (const char*)pbData );
									iStrBufLen = cInfoHeader.dwDataLen;
								}
								else
								{
									WrightLogEx( pHHC->szIP , "==STRING TOO LONG==");
								}
							}
							pTemp->pOnString(pTemp->pOnStringParam, (LPCSTR)szStrBuf, iStrBufLen);
						}
					}
				}
				else if(cInfoHeader.dwType == CAMERA_TFD_STRING)
				{
					DWORD32 dwRecordType = 0;

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
								break;  //只获取BLOCK_RECORD_TYPE 信息
							}
						}
						pbTemp += cBlockHeader.dwLen;
						i += cBlockHeader.dwLen;
					}

					if (iFlag)
					{
						Sleep(100);
						WrightLogEx(pHHC->szIP, "[RecordLink] Reconnect,Copy InfoData Failed...");
						if (pHHC->fAutoLink)
						{
							if(pHHC->dwRecordConnStatus != CONN_STATUS_RECONN
								&& pHHC->AutiLinkOverTime == 0)
							{
								pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
								pHHC->AutiLinkOverTime = time(NULL);
							}
						}
						else
						{
							pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
						}
						continue;
					}

					DWORD dwType = RECORD_TYPE_UNKNOWN;
					if(dwRecordType == CAMERA_RECORD_TFDSTR || dwRecordType == CAMERA_RECORD_STFDSTR)
					{
						dwType = RECORD_TYPE_NORMAL;
					}
					else if(dwRecordType == CAMERA_RECORD_HISTORY)
					{
						dwType = RECORD_TYPE_HISTORY;
					}
					
					HVAPI_CALLBACK_SET* pTemp;
					for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
					{
						if(pTemp->pOnTFDString)
						{
							pTemp->pOnTFDString(pTemp->pOnTFDStringParam, (LPCSTR)pbData, cInfoHeader.dwDataLen, dwType);
						}
					}
				}
			}
			else
			{
				unsigned char* pRecordInfo = pbInfo;
				unsigned char* pRecordData = pbData;
				int nRecordInfoLen = cInfoHeader.dwInfoLen;
				int nRecordDataLen = cInfoHeader.dwDataLen;

				//
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
				//

				if(cInfoHeader.dwType == CAMERA_RECORD)
				{
					DWORD32 dwRecordType = 0;
					DWORD dwAppendInfoBufLen = 1024;
					dwAppendInfoBufLen = (dwAppendInfoBufLen << 5);

					int nAppendInfoLen = 0;

					if ( PROTOCOL_VERSION_1 != pHHC->emProtocolVersion  
						&& PROTOCOL_VERSION_2 != pHHC->emProtocolVersion )
					{
						// 从info中解出TransmittingType
						TiXmlDocument cXmlDoc;
						if (cXmlDoc.Parse((const char*)pbInfo))
						{
							TiXmlElement* pEleRoot = cXmlDoc.RootElement();
							if (NULL == pEleRoot)
							{
								WrightLogEx(pHHC->szIP, "RecordRecvThreadFuncEx pEleRoot == NULL");
								continue; 
							}

							TiXmlElement* pExtInfoEle = pEleRoot->FirstChildElement("ResultExtInfo");
							if (NULL == pExtInfoEle)
							{
								WrightLogEx(pHHC->szIP, "RecordRecvThreadFuncEx pExtInfoEle == NULL");
								continue; 
							}

							TiXmlElement* pTypeEle = pExtInfoEle->FirstChildElement("TransmittingType");
							if (NULL == pTypeEle)
							{
								WrightLogEx(pHHC->szIP, "RecordRecvThreadFuncEx pTypeEle == NULL");
								continue; 
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
								WrightLogEx(pHHC->szIP, "RecordRecvThreadFuncEx pbInfo contains no TransmittingType");
								continue; 
							}
						}
						else
						{
							WrightLogEx(pHHC->szIP, "RecordRecvThreadFuncEx pbInfo can't be parsed");
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
							//ProcRecordDataPackt(pHHC, pRecordData, nRecordDataLen, dwType, (const char*)pbInfo);	
							ProcRecordDataPackt(pHHC, NULL, 0, dwType, (const char*)pbInfo);
						}
						//ProcRecordDataPackt(pHHC, NULL, 0, dwType, (const char*)pbInfo);
					}
					else
					{

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
							if (pHHC->fAutoLink)
							{
								if(pHHC->dwRecordConnStatus != CONN_STATUS_RECONN
									&& pHHC->AutiLinkOverTime == 0)
								{
									pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
									pHHC->AutiLinkOverTime = time(NULL);
								}
							}
							else
							{
								pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
							}
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
						SAFE_DELETE_ARG(szAppendInfo);
					}
				}
				//------------------------------------------------------------------------------
				
				else if(cInfoHeader.dwType == CAMERA_STRING)
				{
					HVAPI_CALLBACK_SET* pTemp;
					for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
					{
						if(pTemp->pOnString)
						{
							//新协议为XML组包
							char szStrBuf[512];
							int iStrBufLen = 512;
							if ( PROTOCOL_VERSION_1 != pHHC->emProtocolVersion  
								&& PROTOCOL_VERSION_2 != pHHC->emProtocolVersion )
							{
								GetCameraString( (const char*)pbData , szStrBuf , &iStrBufLen );
							}
							else 
							{
								if( cInfoHeader.dwDataLen < iStrBufLen )
								{
									strcpy( szStrBuf ,  (const char*)pbData );
									iStrBufLen = cInfoHeader.dwDataLen;
								}
								else
								{
									WrightLogEx( pHHC->szIP , "==STRING TOO LONG==");
								}
							}
							pTemp->pOnString(pTemp->pOnStringParam, (LPCSTR)szStrBuf, iStrBufLen);
						}
					}
				}
				else if(cInfoHeader.dwType == CAMERA_TFD_STRING)
				{
					DWORD32 dwRecordType = 0;

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
								break;  //只获取BLOCK_RECORD_TYPE 信息
							}
						}
						pbTemp += cBlockHeader.dwLen;
						i += cBlockHeader.dwLen;
					}

					if (iFlag)
					{
						Sleep(100);
						WrightLogEx(pHHC->szIP, "[RecordLink] Reconnect,Copy InfoData Failed...");
						if (pHHC->fAutoLink)
						{
							if(pHHC->dwRecordConnStatus != CONN_STATUS_RECONN
								&& pHHC->AutiLinkOverTime == 0)
							{
								pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
								pHHC->AutiLinkOverTime = time(NULL);
							}
						}
						else
						{
							pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
						}
						continue;
					}

					DWORD dwType = RECORD_TYPE_UNKNOWN;
					if(dwRecordType == CAMERA_RECORD_TFDSTR || dwRecordType == CAMERA_RECORD_STFDSTR)
					{
						dwType = RECORD_TYPE_NORMAL;
					}
					else if(dwRecordType == CAMERA_RECORD_HISTORY)
					{
						dwType = RECORD_TYPE_HISTORY;
					}
					
					HVAPI_CALLBACK_SET* pTemp;
					for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
					{
						if(pTemp->pOnTFDString)
						{
							pTemp->pOnTFDString(pTemp->pOnTFDStringParam, (LPCSTR)pbData, cInfoHeader.dwDataLen, dwType);
						}
					}
				}
				//-------------------------------------------------------------------------------
			}

			if(cInfoHeader.dwType == CAMERA_REDEVELOP_RESULT)
			{
				HVAPI_CALLBACK_SET* pTemp;					
				DWORD nWidth = 0;
				DWORD nHeight = 0;
				unsigned char* pbNewData = pbData;
				DWORD32 dwNewDataLen = 0;

				//pbData的前12个字节是图片的宽 高 图片长度，每个4字节
				if ( cInfoHeader.dwDataLen > 0)
				{
					memcpy(&nWidth, pbData, 4);
					memcpy(&nHeight, pbData+4, 4);
					memcpy(&dwNewDataLen, pbData+8, 4);
					pbNewData = pbData + 12;

				}

				for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
				{
					if(pTemp->pOnReDevelopResult)
					{
						pTemp->pOnReDevelopResult(pTemp->pOnReDevelopResultParam, pbInfo, cInfoHeader.dwInfoLen, pbNewData, dwNewDataLen, nWidth, nHeight);
					}
				}



				
			}
		}
		else
		{
			
			char szDebug[256];
			sprintf( szDebug , "===3092[RecordLink] Reconnect,Recv Data EOR...Info:%d,Data: %d, Type: %d ===\n ",cInfoHeader.dwInfoLen , cInfoHeader.dwDataLen , cInfoHeader.dwType );
		    WrightLogEx( pHHC->szIP , szDebug  );

			if (pHHC->fAutoLink)
			{
				if(pHHC->dwRecordConnStatus != CONN_STATUS_RECONN 
					&& pHHC->dwRecordConnStatus != CONN_STATUS_CONNOVERTIME
					&& pHHC->AutiLinkOverTime == 0)
				{
					pHHC->AutiLinkOverTime = time(NULL);
					pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
				}
			}
			else
			{
				pHHC->dwRecordConnStatus = CONN_STATUS_RECONN;
			}
			Sleep(100);
		}
	}

	SAFE_DELETE_ARG(pbInfo);
	SAFE_DELETE_ARG(pbData);
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
				memcpy(pHHC->szRecordEndTimeStr, pTempStr+8, 13);
				pHHC->szRecordEndTimeStr[13] = '\0';
			}
		}

		pTempStr = strstr((char*)pHHC->szRecordConnCmd, "DataInfo");
		if(pTempStr)
		{
			sscanf(pTempStr, "DataInfo[%d]", &pHHC->iRecordDataInfo);
		}
	}

	char* pTempStr = strstr((char*)pHHC->szRecordConnCmd, "SendIllegalVideo");
	if(pTempStr)
	{
		sscanf(pTempStr, "SendIllegalVideo[%d]", &pHHC->iRealTimeRecordSendIllegalVideo);
	}
	if(strstr( (char*)pHHC->szRecordConnCmd, "Enable[0]"))
	{
		pHHC->fIsRecvHistoryRecord = false;
	}

	return S_OK;;
}


HRESULT ProcLprImageFrame(HVAPI_HANDLE_CONTEXT_EX* pHHC, PBYTE pbImgData, DWORD dwImgDataLen,
						  DWORD dwImgDataOffSet, char* pMeruyInfo ,char* pszImageExtInfo, DWORD64 dw64ImageTime, DWORD32 dwImageWidth,  DWORD32  dwImageHeight )
{
	if( NULL == pHHC )
	{
		return E_POINTER;
	}

	HVAPI_CALLBACK_SET* pTemp = NULL;
	int iPlateCount = 0;
	char* pszTemp = NULL;

	int iCurVideoID = 0;
	if(   PROTOCOL_VERSION_1 !=  pHHC->emProtocolVersion
		&& PROTOCOL_VERSION_2 !=  pHHC->emProtocolVersion )
	{
		pszTemp = pMeruyInfo;
	}
	else
	{
		pszTemp = strstr((char*)pbImgData, "rect");
		
		if ( pszTemp == NULL) //解析视频编号
		{
			char* pVideoid = strstr((char*)pbImgData + dwImgDataOffSet - 8, "videoid");
			if (pVideoid)
			{
				memcpy(&iCurVideoID, pVideoid+7, 1);
				if (iCurVideoID < 0 || iCurVideoID > 2)
				{
					iCurVideoID = 0;
				}
			}
		}
	}

	if(pszTemp == NULL)
	{
		char szExtInfo[1024] = {0};
		sprintf(szExtInfo, "%s,VideoID:%d", pszImageExtInfo, iCurVideoID);
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnJpegFrame)
			{
				pTemp->pOnJpegFrame(pTemp->pOnJpegFrameParam, pbImgData+dwImgDataOffSet, 
					dwImgDataLen-dwImgDataOffSet, IMAGE_TYPE_JPEG_LPR, szExtInfo);
			}
			if (pTemp->pOnMJPEG)
			{
				pTemp->pOnMJPEG(pTemp->pOnJpegFrameParam
					,0
					,IMAGE_TYPE_JPEG_LPR
					,dwImageWidth
					,dwImageHeight
					,dw64ImageTime
					,pbImgData+dwImgDataOffSet
					,dwImgDataLen-dwImgDataOffSet
					,szExtInfo);
			}
		}
		return S_OK;
	}
    pszTemp = pszTemp + 4; // 去除前面用于标记的4个空字节
	memcpy(&iPlateCount, pszTemp, sizeof(DWORD32));

    // 若为行人卡口
    if (12345 == iPlateCount)
    {
        pszTemp = pszTemp + 4; // 去除坐标数量(或标记)
        DWORD32 dwDebugInfoLen = 0;
        memcpy(&dwDebugInfoLen, pszTemp, 4);

        strcat(pszImageExtInfo, ",DebugInfo:");
        char* pTmpInfo = pszImageExtInfo + strlen(pszImageExtInfo);
        memcpy(pTmpInfo, pszTemp, dwDebugInfoLen+sizeof(DWORD32));

        for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
        {
            if(pTemp->pOnJpegFrame)
            {
                pTemp->pOnJpegFrame(pTemp->pOnJpegFrameParam, pbImgData+dwImgDataOffSet,
                    dwImgDataLen-dwImgDataOffSet, IMAGE_TYPE_JPEG_LPR, pszImageExtInfo);
            }
            if (pTemp->pOnMJPEG)
            {
                pTemp->pOnMJPEG(pTemp->pOnJpegFrameParam
                    ,0
                    ,IMAGE_TYPE_JPEG_LPR
                    ,dwImageWidth
                    ,dwImageHeight
                    ,dw64ImageTime
                    ,pbImgData+dwImgDataOffSet
                    ,dwImgDataLen-dwImgDataOffSet
                    ,pszImageExtInfo);
            }
        }
        return S_OK;
    }

	int iTempSet = 4+iPlateCount*sizeof(RECT)+4;

	//查找视频编号
	char* pVideoid = strstr((char*)pbImgData+iTempSet, "videoid");
	if (pVideoid)
	{
		memcpy(&iCurVideoID, pVideoid+7, 1);
		if (iCurVideoID < 0 || iCurVideoID > 2)
		{
			iCurVideoID = 0;
		}
	}

	if(iPlateCount <= 0 || iPlateCount > 30)  //原数组为20  导致若红绿灯数+车牌跟踪框大于20时 接收不到图片
	{
		char szExtInfo[1024] = {0};
		sprintf(szExtInfo, "%s,VideoID:%d", pszImageExtInfo, iCurVideoID);
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnJpegFrame)
			{
				pTemp->pOnJpegFrame(pTemp->pOnJpegFrameParam, pbImgData+dwImgDataOffSet, 
					dwImgDataLen-dwImgDataOffSet, IMAGE_TYPE_JPEG_LPR, szExtInfo);
			}
			if (pTemp->pOnMJPEG)
			{
				pTemp->pOnMJPEG(pTemp->pOnJpegFrameParam
					,0
					,IMAGE_TYPE_JPEG_LPR
					,dwImageWidth
					,dwImageHeight
					,dw64ImageTime
					,pbImgData+dwImgDataOffSet
					,dwImgDataLen-dwImgDataOffSet
					,szExtInfo);
			}
		}
		return S_OK;
	}
	
	if(!g_fIsDrawPlateFrame)
	{
        strcat(pszImageExtInfo, "PlatePosInfo:");
		char* pTmpInfo = pszImageExtInfo + strlen(pszImageExtInfo);
		memcpy(pTmpInfo, pszTemp, iPlateCount*sizeof(RECT)+4);

		char szExtInfo[1024] = {0};
		sprintf(szExtInfo, "%s,VideoID:%d", pszImageExtInfo, iCurVideoID);



		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnJpegFrame)
			{
				pTemp->pOnJpegFrame(pTemp->pOnJpegFrameParam, pbImgData+dwImgDataOffSet, 
					dwImgDataLen-dwImgDataOffSet, IMAGE_TYPE_JPEG_LPR, szExtInfo);
			}
			if (pTemp->pOnMJPEG)
			{
				pTemp->pOnMJPEG(pTemp->pOnJpegFrameParam
					,0
					,IMAGE_TYPE_JPEG_LPR
					,dwImageWidth
					,dwImageHeight
					,dw64ImageTime
					,pbImgData+dwImgDataOffSet
					,dwImgDataLen-dwImgDataOffSet
					,szExtInfo);
			}
		}
		return S_OK;
	}
	
    pszTemp = pszTemp + 4; // 这4个字节为车牌数量
	IStream* pStm = NULL;
	CreateStreamOnHGlobal(NULL, TRUE, &pStm);
	if(pStm == NULL)
	{
		char szExtInfo[1024] = {0};
		sprintf(szExtInfo, "%s,VideoID:%d", pszImageExtInfo, iCurVideoID);
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnJpegFrame)
			{
				pTemp->pOnJpegFrame(pTemp->pOnJpegFrameParam, pbImgData+dwImgDataOffSet, 
					dwImgDataLen-dwImgDataOffSet, IMAGE_TYPE_JPEG_LPR, szExtInfo);
			}
			if (pTemp->pOnMJPEG)
			{
				pTemp->pOnMJPEG(pTemp->pOnJpegFrameParam
					,0
					,IMAGE_TYPE_JPEG_LPR
					,dwImageWidth
					,dwImageHeight
					,dw64ImageTime
					,pbImgData+dwImgDataOffSet
					,dwImgDataLen-dwImgDataOffSet
					,szExtInfo);
			}
		}
		return S_OK;
	}

	IStream* pStmDest = NULL;
	CreateStreamOnHGlobal(NULL, TRUE, &pStmDest);
	if(pStmDest == NULL)
	{
		char szExtInfo[1024] = {0};
		sprintf(szExtInfo, "%s,VideoID:%d", pszImageExtInfo, iCurVideoID);
		pStm->Release();
		for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
		{
			if(pTemp->pOnJpegFrame)
			{
				pTemp->pOnJpegFrame(pTemp->pOnJpegFrameParam, pbImgData+dwImgDataOffSet, 
					dwImgDataLen-dwImgDataOffSet, IMAGE_TYPE_JPEG_LPR, szExtInfo);
			}
			if (pTemp->pOnMJPEG)
			{
				pTemp->pOnMJPEG(pTemp->pOnJpegFrameParam
					,0
					,IMAGE_TYPE_JPEG_LPR
					,dwImageWidth
					,dwImageHeight
					,dw64ImageTime
					,pbImgData+dwImgDataOffSet
					,dwImgDataLen-dwImgDataOffSet
					,pszImageExtInfo);
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

	char szExtInfo[1024] = {0};
	sprintf(szExtInfo, "%s,VideoID:%d", pszImageExtInfo, iCurVideoID);
	for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
	{
		if(pTemp->pOnJpegFrame)
		{
			pTemp->pOnJpegFrame(pTemp->pOnJpegFrameParam, pbImageData, 
				(DWORD)ulTmp, IMAGE_TYPE_JPEG_LPR, szExtInfo);
		}
		if (pTemp->pOnMJPEG)
		{
			pTemp->pOnMJPEG(pTemp->pOnJpegFrameParam
				,0
				,IMAGE_TYPE_JPEG_LPR
				,dwImageWidth
				,dwImageHeight
				,dw64ImageTime
				,pbImageData
				,(DWORD)ulTmp
				,szExtInfo);
		}
	}

	SAFE_DELETE_ARG(pbImageData);
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
			SAFE_DELETE_ARG(pbInfo);
			SAFE_DELETE_ARG(pbData);
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
				pHHC->dwImageConnStatus = CONN_STATUS_RECONN;
				//ZeroMemory(pHHC->szImageConnCmd, sizeof(pHHC->szImageConnCmd));
				continue;;
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
				//ZeroMemory(pbInfo, cInfoHeader.dwInfoLen+1);
				//ZeroMemory(pbData, cInfoHeader.dwDataLen+1);
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
                    DWORD32 dwDebugInfoLen = 0;
					ImageExtInfo cImageExtInfo = {0};
					ImageExtInfo cImageMERCURYExtInfo = {0};  //水星图片附件信息
					unsigned char* pbTemp = pbInfo;
					char* pszMercuryExtInfo = new char[128*1024];
					memset(pszMercuryExtInfo, 0, 128*1024);
					int iFlag = 0;
					if ( PROTOCOL_VERSION_1 !=  pHHC->emProtocolVersion
						&& PROTOCOL_VERSION_2 !=  pHHC->emProtocolVersion )
					{ // 水星协议用XML传输info

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
							{
								continue;
							}
							const char* szType = pImageElement->Attribute("Type");
							const char* szWidth = pImageElement->Attribute("Width");
							const char* szHeight = pImageElement->Attribute("Height");
							const char* szTime = pImageElement->Attribute("Time");
							const char* szTimeHigh = pImageElement->Attribute("TimeHigh");
							const char* szTimeLow = pImageElement->Attribute("TimeLow");
							if (NULL == szType ||
								NULL == szWidth ||
								NULL == szHeight)
							{
								continue;
							}

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
                                            char szLog[256];
                                            sprintf(szLog, "Jpeg_Mercury pRectElement[%d] == NULL", i);
                                            WrightLogEx("test IP", szLog);
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
							
							CTime ctime(dw64ImageTime/1000);
							CString strImageTime;
							strImageTime.Format("%4d-%2d-%2d %2d:%2d:%2d", 
								ctime.GetYear(),
								ctime.GetMonth(),
								ctime.GetDay(),
								ctime.GetHour(),
								ctime.GetMinute(),
								ctime.GetSecond());
						}
					}
					else   ////
					{
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
						char* pszImageExtInfo = new char[128*1024];
						if(pszImageExtInfo)
						{
							if ( PROTOCOL_VERSION_1 != pHHC->emProtocolVersion  
								&& PROTOCOL_VERSION_2 != pHHC->emProtocolVersion)
							{
								sprintf(pszImageExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d,FrameTime:%I64u",
									cImageMERCURYExtInfo.iShutter, cImageMERCURYExtInfo.iGain, cImageMERCURYExtInfo.iGainR,
									cImageMERCURYExtInfo.iGainG, cImageMERCURYExtInfo.iGainB, dw64ImageTime);
							}
							else
							{
								sprintf(pszImageExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d,FrameTime:%I64u",
									cImageExtInfo.iShutter, cImageExtInfo.iGain, cImageExtInfo.iGainR,
									cImageExtInfo.iGainG, cImageExtInfo.iGainB, dw64ImageTime);
							}
						}

                        ProcLprImageFrame(pHHC, pbData, cInfoHeader.dwDataLen, dwImageOffset, pszMercuryExtInfo, pszImageExtInfo,  dw64ImageTime, dwImageWidth,  dwImageHeight);
						SAFE_DELETE_ARG(pszImageExtInfo);
						SAFE_DELETE_ARG(pszMercuryExtInfo);
						continue;
					}

					char* pszImageExtInfo = new char[128*1024];
					if(pszImageExtInfo)
					{
						if ( PROTOCOL_VERSION_1 != pHHC->emProtocolVersion 
							&& PROTOCOL_VERSION_2 != pHHC->emProtocolVersion )
						{
							sprintf(pszImageExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d,FrameTime:%I64u",
								cImageMERCURYExtInfo.iShutter, cImageMERCURYExtInfo.iGain, cImageMERCURYExtInfo.iGainR,
								cImageMERCURYExtInfo.iGainG, cImageMERCURYExtInfo.iGainB, dw64ImageTime);
						}
						else
						{
							sprintf(pszImageExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d,FrameTime:%I64u",
								cImageExtInfo.iShutter, cImageExtInfo.iGain, cImageExtInfo.iGainR,
								cImageExtInfo.iGainG, cImageExtInfo.iGainB, dw64ImageTime);
						}
					}
					HVAPI_CALLBACK_SET* pTemp = NULL;
					for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
					{
						if(pTemp->pOnJpegFrame)
						{
							pTemp->pOnJpegFrame(pTemp->pOnJpegFrameParam, pbData+dwImageOffset, 
								cInfoHeader.dwDataLen-dwImageOffset, dwType, pszImageExtInfo);
						}
						if (pTemp->pOnMJPEG)
						{
							pTemp->pOnMJPEG(pTemp->pOnJpegFrameParam
								,0 
								,dwType
								,dwImageWidth
								,dwImageHeight
								,dw64ImageTime
								,pbData+dwImageOffset
								,cInfoHeader.dwDataLen-dwImageOffset
								,pszImageExtInfo);
						}
					}
					SAFE_DELETE_ARG(pszImageExtInfo);
                    SAFE_DELETE_ARG(pszMercuryExtInfo);
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
	SAFE_DELETE_ARG(pbInfo);
	SAFE_DELETE_ARG(pbData);
	return 0;
}

HRESULT ProcHistoryVideoJpegFrame(HVAPI_HANDLE_CONTEXT_EX* pHHC, PBYTE pbFrameData, DWORD dwFrameDataLen, LPCSTR szVideoExtInfo,  DWORD64 dw64VideoTime, DWORD32 dwVideoWidth,  DWORD32  dwVideoHeight )
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
			if (pTemp->pOnRecoreH264HistoryVideo)
			{
				pTemp->pOnRecoreH264HistoryVideo(pTemp->pOnHistoryVideoParam
					, 0
					, VIDEO_TYPE_JPEG_HISTORY
					, dwVideoWidth
					, dwVideoHeight
					, dw64VideoTime
					, pbTemp
					, iSize
					,  szVideoExtInfo
					);
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
			if (pTemp->pOnRecoreH264HistoryVideo)
			{
				pTemp->pOnRecoreH264HistoryVideo(pTemp->pOnHistoryVideoParam
					, 0
					, VIDEO_TYPE_JPEG_HISTORY
					, dwVideoWidth
					, dwVideoHeight
					, dw64VideoTime
					, pbTemp
					, iSize
					,  szVideoExtInfo
					);
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
			if (pTemp->pOnRecoreH264HistoryVideo)
			{
				pTemp->pOnRecoreH264HistoryVideo(pTemp->pOnHistoryVideoParam
					, 0
					, VIDEO_TYPE_JPEG_HISTORY
					, dwVideoWidth
					, dwVideoHeight
					, dw64VideoTime
					, pbTemp
					, iSize
					,  szVideoExtInfo
					);
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
			if (pTemp->pOnRecoreH264HistoryVideo)
			{
				pTemp->pOnRecoreH264HistoryVideo(pTemp->pOnHistoryVideoParam
					, 0
					, VIDEO_TYPE_JPEG_HISTORY
					, dwVideoWidth
					, dwVideoHeight
					, dw64VideoTime
					, pbTemp
					, iSize
					,  szVideoExtInfo
					);
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
		if (pTemp->pOnRecoreH264HistoryVideo)
		{
			pTemp->pOnRecoreH264HistoryVideo(pTemp->pOnHistoryVideoParam
				, 0
				, VIDEO_TYPE_JPEG_HISTORY
				, dwVideoWidth
				, dwVideoHeight
				, dw64VideoTime
				, pbDestImgBuf
				, dwDestImgBufLen
				,  szVideoExtInfo
				);
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
			SAFE_DELETE_ARG(pbInfo);
			SAFE_DELETE_ARG(pbData);

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

				HVAPI_CALLBACK_SET* pTemp = NULL;
				for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
				{
					if (pTemp->pOnRecoreH264HistoryVideo)
					{
						pTemp->pOnRecoreH264HistoryVideo(pTemp->pOnHistoryVideoParam
							, H264_FLAG_HISTROY_END
							, VIDEO_TYPE_UNKNOWN
							, 0
							, 0
							, 0
							, 0
							, 0
							,  NULL
							);
					}
				}

				break;
			}

			DWORD32 dwBuffLen = 10 * 1024 * 1024;
			if (cInfoHeader.dwInfoLen > dwBuffLen || cInfoHeader.dwDataLen > dwBuffLen)
			{
				WrightLogEx(pHHC->szIP, "[VideoLink] Quit,Recive Data Error...");
				pHHC->dwVideoConnStatus = CONN_STATUS_RECONN;
				//ZeroMemory(pHHC->szVideoConnCmd, sizeof(pHHC->szVideoConnCmd));
				continue;
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
					
					if ( PROTOCOL_VERSION_1 != pHHC->emProtocolVersion && 
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
								if( dwType == VIDEO_TYPE_UNKNOWN || iFrameLen > cInfoHeader.dwDataLen)
								{
									FILE *fp = fopen("VideoFrame.log","a+");
									if(fp)
									{
										fprintf(fp,"Type = %0x  iFrameLen = %10d\tcInfoHeader.dwDataLen = %7d  pszVideoExtInfo = %s\n", 
											dwType, iFrameLen, cInfoHeader.dwDataLen, pszVideoExtInfo);
										fclose(fp);
									}
									break;
								}
								HVAPI_CALLBACK_SET* pTemp = NULL;
								for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
								{
									if(pTemp->pOnHistoryVideo)
									{
										pTemp->pOnHistoryVideo(pTemp->pOnHistoryVideoParam, pPacketData, iFrameLen, dwType, pszVideoExtInfo);
									}
									if (pTemp->pOnRecoreH264HistoryVideo)
									{
										pTemp->pOnRecoreH264HistoryVideo(pTemp->pOnHistoryVideoParam
											, 0
											, dwType
											, cVideoWidth.iWidth
											, cVideoHeight.iHight
											, dw64VideoTime
											, pPacketData
											, iFrameLen
											,  pszVideoExtInfo
											);
									}
								}
								pPacketData += iFrameLen;
								iTempBufferLem = iTempBufferLem - 8 - iFrameLen;
							}
							SAFE_DELETE_ARG(pszVideoExtInfo);
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
							if (pTemp->pOnRecoreH264Video)
							{
								WrightLogEx("pOnRecoreH264Video== coming" , "");
								pTemp->pOnRecoreH264Video(pTemp->pOnH264Param
									, 0
									, dwType
									, cVideoWidth.iWidth
									, cVideoHeight.iHight
									, dw64VideoTime
									, pbData
									, cInfoHeader.dwDataLen
									,  pszVideoExtInfo
									);
							}
						}
						SAFE_DELETE_ARG(pszVideoExtInfo);
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
							ProcHistoryVideoJpegFrame(pHHC, pbData+iReadedCount, iFrameLen, pszVideoExtInfo, dw64VideoTime, (DWORD32)cVideoWidth.iWidth, (DWORD32)cVideoHeight.iHight);
							iReadedCount += iFrameLen;
							iTempBufferLem -= iFrameLen;
						}
						SAFE_DELETE_ARG(pszVideoExtInfo);
						CTime cTime(dw64VideoTime/1000);
						CString cStr = cTime.Format("%Y.%m.%d_%H:%M:%S");
						memcpy(pHHC->szVideoBeginTimeStr, cStr.GetBuffer(), 19);
						pHHC->szVideoBeginTimeStr[19] = '\0';
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
	SAFE_DELETE_ARG(pbInfo);
	SAFE_DELETE_ARG(pbData);
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

	if(szConnCmd != NULL)
	{
		WrightLogEx(pHHC->szIP, szConnCmd);
	}

	// 非2.0版本则不走这里
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
				char szConnCmdReplace[128];
				char* pszConnCmdReplace = NULL;
				CCSLockEx sLock(&pHHC->csCallbackRecord);
				WrightLogEx(pHHC->szIP, "设置车牌回调");
				
				bool HasIllegalVideo = false;
				for(HVAPI_CALLBACK_SET* pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
				{
					if(pTemp != NULL  && pTemp->pOnIllegalVideo != NULL)
					{						
						HasIllegalVideo = true;
						break;
					}
					
				}
						
				if (NULL == szConnCmd)
				{
					if(HasIllegalVideo)
					{
						strcpy(szConnCmdReplace, "DownloadRecord,BeginTime[0],Index[0],Enable[0],EndTime[0],DataInfo[0],SendIllegalVideo[1]");
						pszConnCmdReplace = szConnCmdReplace;
					}
					//pszConnCmdReplace = szConnCmdReplace;
				}
				else
				{
					if (sizeof(szConnCmdReplace) > strlen(szConnCmd))
					{
						char* pIllegalVideo = strstr(szConnCmd,"SendIllegalVideo[");
						if (pIllegalVideo == NULL)
						{
							//追加SendIllegalVideo属性
							if (sizeof(szConnCmdReplace)  > strlen(szConnCmd) + strlen(",SendIllegalVideo[1]"))
							{
								strcpy(szConnCmdReplace, szConnCmd);
								strcat(szConnCmdReplace, ",SendIllegalVideo[1]");
							}
							else
							{
								strcpy(szConnCmdReplace, szConnCmd);
							}
						}
						else
						{
							//强制覆盖设置SendIllegalVideo属性为1														
							strcpy(szConnCmdReplace, szConnCmd);
							char* pIllegalVideoReplace = strstr(szConnCmdReplace,"SendIllegalVideo[");
							if(strlen(pIllegalVideoReplace)  >=  strlen("SendIllegalVideo[1]"))//pIllegalVideoReplace能进行覆盖操作，不会溢出
							{
								memcpy(pIllegalVideoReplace, "SendIllegalVideo[1]", strlen("SendIllegalVideo[1]"));
							}
						}	
						pszConnCmdReplace = szConnCmdReplace;						
						
					}
					else
					{
						memset(szConnCmdReplace, 0, sizeof(szConnCmdReplace));
						pszConnCmdReplace = NULL;
					}
				}
				
				if(pHHC->fAutoLink)
				{
					if (INVALID_SOCKET != pHHC->sktRecord)
					{
						ForceCloseSocket(pHHC->sktRecord);
					}

					if ( pszConnCmdReplace != NULL && sizeof(pHHC->szRecordConnCmd) > strlen(pszConnCmdReplace) )
					{
						strcpy((char*)pHHC->szRecordConnCmd, pszConnCmdReplace);
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
				//	pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
					pHHC->fVailPackResumeCache = FALSE;
					pHHC->AutoLinkSetCallBackTime  = time(NULL);
					SetConnAutoLinkHHC(pHHC->szDevSN);
					pHHC->fIsRecordPlateExit = FALSE;
					WrightLogEx(pHHC->szIP, "主动连接设置车牌回调成功");
					return S_OK;
				}

				if ((pHHC->dwRecordConnStatus == CONN_STATUS_UNKNOWN
					|| pHHC->dwRecordConnStatus == CONN_STATUS_DISCONN
					|| pHHC->dwRecordConnStatus == CONN_STATUS_CONNFIRST)
					&& pHHC->fIsRecordStringExit == TRUE)
				{
					if (ConnectCamera(pHHC->szIP, CAMERA_RECORD_LINK_PORT, pHHC->sktRecord))
					{
						if ( pszConnCmdReplace != NULL && sizeof(pHHC->szRecordConnCmd) > strlen(pszConnCmdReplace))
						{
							strcpy((char*)pHHC->szRecordConnCmd, pszConnCmdReplace);
							char szRetBuf[256] = {0};
							char szTmpConnCmd[128] = {0};
							memcpy(szTmpConnCmd, (char*)pHHC->szRecordConnCmd, 128);
							if(HvSendXmlCmd(pHHC->szIP, szTmpConnCmd, szRetBuf, sizeof(szRetBuf),
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
										memcpy(pHHC->szRecordEndTimeStr, pTempStr+8, 13);
										pHHC->szRecordEndTimeStr[13] = '\0';
									}
								}

								pTempStr = strstr((char*)pHHC->szRecordConnCmd, "DataInfo");
								if(pTempStr)
								{
									sscanf(pTempStr, "DataInfo[%d]", &pHHC->iRecordDataInfo);
								}

								
								pTempStr = strstr((char*)pHHC->szRecordConnCmd, "SendIllegalVideo");
								if(pTempStr)
								{
									sscanf(pTempStr, "SendIllegalVideo[%d]", &pHHC->iRealTimeRecordSendIllegalVideo);
								}
							}
						}
						else
						{
							memset(pHHC->szRecordConnCmd, 0, 128);
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
						pHHC->hThreadRecvRecord = CreateThread(NULL, STACK_SIZE, RecordRecvThreadFuncEx, pHHC, 0, NULL);
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
					WrightLogEx(pHHC->szIP, "设置车牌回调成功");
					pHHC->fIsRecordPlateExit = FALSE;
					return S_OK;
				}
				pHHC->fIsRecordPlateExit = FALSE;
				WrightLogEx(pHHC->szIP, "设置车牌回调成功");
			}
			else
			{	
				WrightLogEx(pHHC->szIP, "取消车牌回调");

				pHHC->fIsThreadRecvRecordExit = TRUE;
				HvSafeCloseThread(pHHC->hThreadRecvRecord);
				ForceCloseSocket(pHHC->sktRecord);
				pHHC->hThreadRecvRecord = NULL;	

				pHHC->fIsRecordStringExit = TRUE;
				pHHC->fIsRecordPlateExit = TRUE;

				pHHC->dwRecordConnStatus = CONN_STATUS_DISCONN;

				memset(pHHC->szRecordConnCmd, 0, 128);

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
				WrightLogEx(pHHC->szIP, "取消车牌回调成功");
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
						WrightLogEx(pHHC->szIP, "设置大图回调");
						CCSLockEx sLock(&pHHC->csCallbackRecord);
						pTemp->pOnBigImage = (HVAPI_CALLBACK_RECORD_BIGIMAGE)pFunc;
						pTemp->pOnBigImageParam = (PVOID)pUserData;
						break;
					}
					else
					{	
						WrightLogEx(pHHC->szIP, "取消大图回调");
						pTemp->pOnBigImage = NULL;
						pTemp->pOnBigImageParam = NULL;
						WrightLogEx(pHHC->szIP, "取消大图回调成功");
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
			WrightLogEx(pHHC->szIP, "设置大图回调成功");
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
						WrightLogEx(pHHC->szIP, "设置小图回调");
						CCSLockEx sLock(&pHHC->csCallbackRecord);
						pTemp->pOnSmallImage = (HVAPI_CALLBACK_RECORD_SMALLIMAGE)pFunc;
						pTemp->pOnSmallImageParam = (PVOID)pUserData;
						break;
					}
					else
					{
						WrightLogEx(pHHC->szIP, "取消小图回调");
						pTemp->pOnSmallImage = NULL;
						pTemp->pOnSmallImageParam = NULL;
						WrightLogEx(pHHC->szIP, "取消小图回调成功");
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
			WrightLogEx(pHHC->szIP, "设置小图回调成功");
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
						WrightLogEx(pHHC->szIP, "设置BIN图回调");
						CCSLockEx sLock(&pHHC->csCallbackRecord);
						pTemp->pOnBinaryImage = (HVAPI_CALLBACK_RECORD_BINARYIMAGE)pFunc;
						pTemp->pOnBinaryImageParam = (PVOID)pUserData;
						break;
					}
					else
					{
						WrightLogEx(pHHC->szIP, "取消BIN图回调");
						pTemp->pOnBinaryImage = NULL;
						pTemp->pOnBinaryImageParam = NULL;
						WrightLogEx(pHHC->szIP, "取消BIN图回调成功");

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
			WrightLogEx(pHHC->szIP, "设置BIN图回调成功");
		}
		break;
	case CALLBACK_TYPE_RECORD_ILLEGALVIDEO:
		{
			HVAPI_CALLBACK_SET* pTemp = NULL;
			for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
			{
				if(pTemp->iVideoID == iVideoID)
				{
					if(pFunc)
					{
						WrightLogEx(pHHC->szIP, "设置违法视频回调");
						CCSLockEx sLock(&pHHC->csCallbackRecord);
						pTemp->pOnIllegalVideo = (HVAPI_CALLBACK_RECORD_ILLEGALVideo)pFunc;
						pTemp->pOnIllegalVideoParam = (PVOID)pUserData;
						break;
					}
					else
					{
						WrightLogEx(pHHC->szIP, "取消违法视频回调");
						pTemp->pOnIllegalVideo = NULL;
						pTemp->pOnIllegalVideoParam = NULL;
						WrightLogEx(pHHC->szIP, "取消违法视频回调成功");

						return S_OK;
					}
				}
			}
			if(pTemp == NULL)
			{
				pTemp = new HVAPI_CALLBACK_SET();
				memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
				pTemp->iVideoID = iVideoID;
				pTemp->pOnIllegalVideo = (HVAPI_CALLBACK_RECORD_ILLEGALVideo)pFunc;
				pTemp->pOnIllegalVideoParam = (PVOID)pUserData;
				pTemp->pNext = pHHC->pCallBackSet;
				pHHC->pCallBackSet = pTemp;
			}
			WrightLogEx(pHHC->szIP, "设置违法视频回调成功");
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
			if(pFunc)
			{
				if ((pHHC->dwRecordConnStatus == CONN_STATUS_UNKNOWN
					|| pHHC->dwRecordConnStatus == CONN_STATUS_DISCONN
					|| pHHC->dwRecordConnStatus == CONN_STATUS_CONNFIRST)
					&& pHHC->fIsRecordPlateExit == TRUE)
				{
					if (ConnectCamera(pHHC->szIP, CAMERA_RECORD_LINK_PORT, pHHC->sktRecord))
					{
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnString = (HVAPI_CALLBACK_STRING)pFunc;
								pTemp->pOnStringParam = (PVOID)pUserData;
								break;
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

						pHHC->fIsThreadRecvRecordExit = FALSE;
						pHHC->hThreadRecvRecord = CreateThread(NULL, STACK_SIZE, RecordRecvThreadFuncEx, pHHC, 0, NULL);
						pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
					}
					else
					{
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnString = NULL;
								pTemp->pOnStringParam = NULL;
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
							pTemp->pOnString = (HVAPI_CALLBACK_STRING)pFunc;
							pTemp->pOnStringParam = (PVOID)pUserData;
							break;
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
					pHHC->fIsRecordStringExit = FALSE;
				}
				pHHC->fIsRecordStringExit = FALSE;
			}
			else
			{
				if(pHHC->fIsRecordPlateExit == TRUE && pHHC->fIsRecordStringExit == FALSE)
				{
					pHHC->fIsThreadRecvRecordExit = TRUE;
					HvSafeCloseThread(pHHC->hThreadRecvRecord);
					ForceCloseSocket(pHHC->sktRecord);
					pHHC->hThreadRecvRecord = NULL;
				}
				pHHC->fIsRecordStringExit = TRUE;
				if(pHHC->fIsRecordPlateExit == TRUE)
				{
					pHHC->dwRecordConnStatus = CONN_STATUS_DISCONN;
				}

				HVAPI_CALLBACK_SET* pTemp = NULL;
				for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
				{
					if(pTemp->iVideoID == iVideoID)
					{
						pTemp->pOnString = NULL;
						pTemp->pOnStringParam = NULL;
						break;
					}
				}
			}
		}
		break;
	case CALLBACK_TYPE_JPEG_FRAME:
		{
			if(pFunc)
			{
				CCSLockEx sLock(&pHHC->csCallbackJpeg);
				if (pHHC->fAutoLink)
				{
					WrightLogEx(pHHC->szIP, "设置JPEG回调失败，存在主动连接");
					return E_NOTIMPL;
				}
				WrightLogEx(pHHC->szIP, "设置JPEG回调");
				if(pHHC->dwImageConnStatus == CONN_STATUS_UNKNOWN
					|| pHHC->dwImageConnStatus == CONN_STATUS_DISCONN)
				{
					if(ConnectCamera(pHHC->szIP, CAMERA_IMAGE_LINK_PORT, pHHC->sktImage))
					{
						if(szConnCmd != NULL && sizeof(pHHC->szImageConnCmd) > strlen(szConnCmd))
						{
							strcpy((char*)pHHC->szImageConnCmd, szConnCmd);
							char szRetBuf[256] = {0};
							char szTmpConnCmd[128] = {0};
							memcpy(szTmpConnCmd, (char*)pHHC->szImageConnCmd, 128);

							if(HvSendXmlCmd(pHHC->szIP, szTmpConnCmd, szRetBuf, sizeof(szRetBuf),
								NULL, pHHC->sktImage) != S_OK)
							{
								ForceCloseSocket(pHHC->sktImage);
								WrightLogEx(pHHC->szIP, "设置JPEG回调失败，发送XML命令失败");
								return E_FAIL;
							}
						}
						else
						{
							memset(pHHC->szImageConnCmd, 0, 128);
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
						pHHC->hThreadRecvImage = CreateThread(NULL, STACK_SIZE, ImageRecvThreadFuncEx, pHHC, 0, NULL);
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
						WrightLogEx(pHHC->szIP, "设置JPEG回调失败，连接相机失败");
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
					WrightLogEx(pHHC->szIP, "设置JPEG回调成功");
					return S_OK;
				}
				WrightLogEx(pHHC->szIP, "设置JPEG回调成功");
			}
			else
			{	
				WrightLogEx(pHHC->szIP, "取消JPEG回调");
				pHHC->dwImageConnStatus = CONN_STATUS_DISCONN;
				pHHC->fIsThreadRecvImageExit = TRUE;
				HvSafeCloseThread(pHHC->hThreadRecvImage);
				ForceCloseSocket(pHHC->sktImage);
				memset(pHHC->szImageConnCmd, 0, 128);

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
				WrightLogEx(pHHC->szIP, "取消JPEG回调成功");
			}
		}
		break;
	case CALLBACK_TYPE_H264_VIDEO:
		{
			if(pHHC->fIsConnectHistoryVideo)
			{
				WrightLogEx(pHHC->szIP, "设置历史录像回调失败，存在历史录像连接");
				return E_FAIL;
			}
			if(pFunc)
			{
				CCSLockEx sLock(&pHHC->csCallbackH264);
				if (pHHC->fAutoLink)
				{
					WrightLogEx(pHHC->szIP, "设置录像回调失败，存在主动连接");
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
							WrightLogEx(pHHC->szIP, "设置录像回调失败，连接命令有误");
							return E_FAIL;
						}
					}
					WrightLogEx(pHHC->szIP, "设置录像回调");
					if(ConnectCamera(pHHC->szIP, CAMERA_VIDEO_LINK_PORT, pHHC->sktVideo))
					{
						if(szConnCmd != NULL && sizeof(pHHC->szVideoConnCmd) > strlen(szConnCmd))
						{
							strcpy((char*)pHHC->szVideoConnCmd, szConnCmd);
							char szRetBuf[256] = {0};
							char szTmpConnCmd[128] = {0};
							memcpy(szTmpConnCmd, (char*)pHHC->szVideoConnCmd, 128);
							if(HvSendXmlCmd(pHHC->szIP, szTmpConnCmd, szRetBuf, sizeof(szRetBuf),
								NULL, pHHC->sktVideo) != S_OK)
							{
								ForceCloseSocket(pHHC->sktVideo);
								WrightLogEx(pHHC->szIP, "设置录像回调失败，发送XML命令失败");
								return E_FAIL;
							}
						}
						else
						{
							memset(pHHC->szVideoConnCmd, 0, 128);
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
						pHHC->hThreadRecvVideo = CreateThread(NULL, STACK_SIZE, VideoRecvThreadFuncEx, pHHC, 0, NULL);
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
						WrightLogEx(pHHC->szIP, "设置录像回调失败，连接一体机失败");
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
				WrightLogEx(pHHC->szIP, "设置录像回调成功");
			}
			else
			{
				WrightLogEx(pHHC->szIP, "取消录像回调");
				pHHC->dwVideoConnStatus = CONN_STATUS_DISCONN;
				pHHC->fIsThreadRecvVideoExit = TRUE;
				HvSafeCloseThread(pHHC->hThreadRecvVideo);
				ForceCloseSocket(pHHC->sktVideo);
				memset(pHHC->szVideoConnCmd, 0, 128);

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
				WrightLogEx(pHHC->szIP, "取消录像回调成功");
			}
		}
		break;
	case CALLBACK_TYPE_HISTORY_VIDEO:
		{
			if(pFunc)
			{
				CCSLockEx sLock(&pHHC->csCallbackH264);
				if (pHHC->fAutoLink)
				{
					WrightLogEx(pHHC->szIP, "设置历史录像回调失败，存在主动连接");
					return E_NOTIMPL;
				}
				if(szConnCmd == NULL)
				{
					WrightLogEx(pHHC->szIP, "设置历史录像回调失败，连接命令为空");
					return E_FAIL;
				}
				if(!strstr(szConnCmd, "DownloadVideo") || 
					!strstr(szConnCmd, "Enable[1]"))
				{
					WrightLogEx(pHHC->szIP, "设置历史录像回调失败，连接命令有误");
					return E_FAIL;
				}
				WrightLogEx(pHHC->szIP, "设置历史录像回调");
				if(pHHC->dwVideoConnStatus == CONN_STATUS_UNKNOWN
					|| pHHC->dwVideoConnStatus == CONN_STATUS_DISCONN
					|| pHHC->dwVideoConnStatus == CONN_STATUS_RECVDONE)
				{
					if(ConnectCamera(pHHC->szIP, CAMERA_VIDEO_LINK_PORT, pHHC->sktVideo))
					{
						strcpy((char*)pHHC->szVideoConnCmd, szConnCmd);
						char szRetBuf[256] = {0};
						char szTmpConnCmd[128] = {0};
						memcpy(szTmpConnCmd, (char*)pHHC->szVideoConnCmd, 128);
						WrightLogEx("设置历史录像回调" , szTmpConnCmd);
						if(HvSendXmlCmd(pHHC->szIP, szTmpConnCmd, szRetBuf, sizeof(szRetBuf),
							NULL, pHHC->sktVideo) != S_OK)
						{
							ForceCloseSocket(pHHC->sktVideo);
							pHHC->fIsConnectHistoryVideo = FALSE;
							WrightLogEx(pHHC->szIP, "设置历史录像回调失败，发送XML命令失败");
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
						pHHC->hThreadRecvVideo = CreateThread(NULL, STACK_SIZE, VideoRecvThreadFuncEx, pHHC, 0, NULL);
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
						WrightLogEx(pHHC->szIP, "设置历史录像回调失败，连接一体机失败");
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
						WrightLogEx(pHHC->szIP, "设置历史录像回调失败，历史录像连接依然存在");
						return E_FAIL;
					}
				}
				WrightLogEx(pHHC->szIP, "设置历史录像回调成功");
			}
			else
			{
				WrightLogEx(pHHC->szIP, "取消历史录像回调");
				if(pHHC->sktVideo != INVALID_SOCKET)
				{
					ForceCloseSocket(pHHC->sktVideo);
				}
				pHHC->fIsThreadRecvVideoExit = TRUE;
				HvSafeCloseThread(pHHC->hThreadRecvVideo);
				ForceCloseSocket(pHHC->sktVideo);
				pHHC->dwVideoConnStatus = CONN_STATUS_DISCONN;
				memset(pHHC->szVideoConnCmd, 0, 128);

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
				pHHC->fIsConnectHistoryVideo = FALSE;
				WrightLogEx(pHHC->szIP, "取消历史录像回调成功");
			}
		}
		break;
	case CALLBACK_TYPE_TFD_STRING:
		{
			if(pFunc)
			{
				if ((pHHC->dwRecordConnStatus == CONN_STATUS_UNKNOWN
					|| pHHC->dwRecordConnStatus == CONN_STATUS_DISCONN
					|| pHHC->dwRecordConnStatus == CONN_STATUS_CONNFIRST)
					&& pHHC->fIsRecordPlateExit == TRUE)
				{
					if (ConnectCamera(pHHC->szIP, CAMERA_RECORD_LINK_PORT, pHHC->sktRecord))
					{

						if ( szConnCmd != NULL && sizeof(pHHC->szRecordConnCmd) > strlen(szConnCmd))
						{
							strcpy((char*)pHHC->szRecordConnCmd, szConnCmd);
							char szRetBuf[256] = {0};
							char szTmpConnCmd[128] = {0};
							memcpy(szTmpConnCmd, (char*)pHHC->szRecordConnCmd, 128);
							if(HvSendXmlCmd(pHHC->szIP, szTmpConnCmd, szRetBuf, sizeof(szRetBuf),
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
										memcpy(pHHC->szRecordEndTimeStr, pTempStr+8, 13);
										pHHC->szRecordEndTimeStr[13] = '\0';
									}
								}

								pTempStr = strstr((char*)pHHC->szRecordConnCmd, "DataInfo");
								if(pTempStr)
								{
									sscanf(pTempStr, "DataInfo[%d]", &pHHC->iRecordDataInfo);
								}

								pTempStr = strstr((char*)pHHC->szRecordConnCmd, "SendIllegalVideo");
								if(pTempStr)
								{
									sscanf(pTempStr, "SendIllegalVideo[%d]", &pHHC->iRealTimeRecordSendIllegalVideo);
								}
							}
						}
						else
						{
							memset(pHHC->szRecordConnCmd, 0, 128);
						}

						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnTFDString = (HVAPI_CALLBACK_TFD_STRING)pFunc;
								pTemp->pOnTFDStringParam = (PVOID)pUserData;
								break;
							}
						}
						if(pTemp == NULL)
						{
							pTemp = new HVAPI_CALLBACK_SET();
							memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
							pTemp->iVideoID = iVideoID;
							pTemp->pOnTFDString = (HVAPI_CALLBACK_TFD_STRING)pFunc;
							pTemp->pOnTFDStringParam = (PVOID)pUserData;
							pTemp->pNext = pHHC->pCallBackSet;
							pHHC->pCallBackSet = pTemp;
						}

						pHHC->fIsThreadRecvRecordExit = FALSE;
						pHHC->hThreadRecvRecord = CreateThread(NULL, STACK_SIZE, RecordRecvThreadFuncEx, pHHC, 0, NULL);
						pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
					}
					else
					{
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnTFDString = NULL;
								pTemp->pOnTFDStringParam = NULL;
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
							pTemp->pOnTFDString = (HVAPI_CALLBACK_TFD_STRING)pFunc;
							pTemp->pOnTFDStringParam = (PVOID)pUserData;
							break;
						}
					}
					if(pTemp == NULL)
					{
						pTemp = new HVAPI_CALLBACK_SET();
						memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
						pTemp->iVideoID = iVideoID;
						pTemp->pOnTFDString = (HVAPI_CALLBACK_TFD_STRING)pFunc;
						pTemp->pOnTFDStringParam = (PVOID)pUserData;
						pTemp->pNext = pHHC->pCallBackSet;
						pHHC->pCallBackSet = pTemp;
					}
					pHHC->fIsRecordStringExit = FALSE;
				}
				pHHC->fIsRecordStringExit = FALSE;
			}
			else
			{
				if(pHHC->fIsRecordPlateExit == TRUE && pHHC->fIsRecordStringExit == FALSE)
				{
					pHHC->fIsThreadRecvRecordExit = TRUE;
					HvSafeCloseThread(pHHC->hThreadRecvRecord);
					ForceCloseSocket(pHHC->sktRecord);
					pHHC->hThreadRecvRecord = NULL;
				}
				pHHC->fIsRecordStringExit = TRUE;
				if(pHHC->fIsRecordPlateExit == TRUE)
				{
					pHHC->dwRecordConnStatus = CONN_STATUS_DISCONN;
				}

				HVAPI_CALLBACK_SET* pTemp = NULL;
				for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
				{
					if(pTemp->iVideoID == iVideoID)
					{
						pTemp->pOnTFDString = NULL;
						pTemp->pOnTFDStringParam = NULL;
						break;
					}
				}
			}
		}
		break;
	case CALLBACK_TYPE_ADV_VIDEO:
		{
			if(pHHC->fIsConnectHistoryVideo)
			{
				WrightLogEx(pHHC->szIP, "设置历史录像回调失败，存在历史录像连接");
				return E_FAIL;
			}
			if(pFunc)
			{
				CCSLockEx sLock(&pHHC->csCallbackH264);
				if (pHHC->fAutoLink)
				{
					WrightLogEx(pHHC->szIP, "设置录像回调失败，存在主动连接");
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
							WrightLogEx(pHHC->szIP, "设置录像回调失败，连接命令有误");
							return E_FAIL;
						}
					}
					WrightLogEx(pHHC->szIP, "设置录像回调");
					if(ConnectCamera(pHHC->szIP, CAMERA_VIDEO_LINK_PORT, pHHC->sktVideo))
					{
						if(szConnCmd != NULL && sizeof(pHHC->szVideoConnCmd) > strlen(szConnCmd))
						{
							strcpy((char*)pHHC->szVideoConnCmd, szConnCmd);
							char szRetBuf[256] = {0};
							char szTmpConnCmd[128] = {0};
							memcpy(szTmpConnCmd, (char*)pHHC->szVideoConnCmd, 128);
							if(HvSendXmlCmd(pHHC->szIP, szTmpConnCmd, szRetBuf, sizeof(szRetBuf),
								NULL, pHHC->sktVideo) != S_OK)
							{
								ForceCloseSocket(pHHC->sktVideo);
								WrightLogEx(pHHC->szIP, "设置录像回调失败，发送XML命令失败");
								return E_FAIL;
							}
						}
						else
						{
							memset(pHHC->szVideoConnCmd, 0, 128);
						}
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp !=NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnRecoreH264Video = (HVAPI_CALLBACK_H264_EX)pFunc;
								pTemp->pOnH264Param = (PVOID)pUserData;
								break;
							}
						}
						if(pTemp == NULL)
						{
							pTemp = new HVAPI_CALLBACK_SET();
							memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
							pTemp->iVideoID = iVideoID;
							pTemp->pOnRecoreH264Video = (HVAPI_CALLBACK_H264_EX)pFunc;
							pTemp->pOnH264Param = (PVOID)pUserData;
							pTemp->pNext = pHHC->pCallBackSet;
							pHHC->pCallBackSet = pTemp;
						}
						pHHC->fIsThreadRecvVideoExit = FALSE;
						pHHC->hThreadRecvVideo = CreateThread(NULL, STACK_SIZE, VideoRecvThreadFuncEx, pHHC, 0, NULL);
						pHHC->dwVideoConnStatus = CONN_STATUS_NORMAL;
					}
					else
					{
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp!= NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnRecoreH264Video = NULL;
								pTemp->pOnH264Param = NULL;
								break;
							}
						}
						WrightLogEx(pHHC->szIP, "设置录像回调失败，连接一体机失败");
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
							pTemp->pOnRecoreH264Video = (HVAPI_CALLBACK_H264_EX)pFunc;
							pTemp->pOnH264Param = (PVOID)pUserData;
							break;
						}
					}
					if(pTemp == NULL)
					{
						pTemp = new HVAPI_CALLBACK_SET();
						memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
						pTemp->iVideoID = iVideoID;
						pTemp->pOnRecoreH264Video = (HVAPI_CALLBACK_H264_EX)pFunc;
						pTemp->pOnH264Param = (PVOID)pUserData;
						pTemp->pNext = pHHC->pCallBackSet;
						pHHC->pCallBackSet = pTemp;
					}
				}
				WrightLogEx(pHHC->szIP, "设置录像回调成功");
			}
			else
			{
				WrightLogEx(pHHC->szIP, "取消录像回调");
				pHHC->dwVideoConnStatus = CONN_STATUS_DISCONN;
				pHHC->fIsThreadRecvVideoExit = TRUE;
				HvSafeCloseThread(pHHC->hThreadRecvVideo);
				ForceCloseSocket(pHHC->sktVideo);
				memset(pHHC->szVideoConnCmd, 0, 128);

				HVAPI_CALLBACK_SET* pTemp = NULL;
				for(pTemp = pHHC->pCallBackSet; pTemp!= NULL; pTemp = pTemp->pNext)
				{
					if(pTemp->iVideoID == iVideoID)
					{
						pTemp->pOnRecoreH264Video = NULL;
						pTemp->pOnH264Param = NULL;
						break;
					}
				}
				WrightLogEx(pHHC->szIP, "取消录像回调成功");
			}


		}
		break;
	case CALLBACK_TYPE_ADV_HISTORY_VIDEO:
		{
			if(pFunc)
			{
				CCSLockEx sLock(&pHHC->csCallbackH264);
				if (pHHC->fAutoLink)
				{
					WrightLogEx(pHHC->szIP, "设置历史录像回调失败，存在主动连接");
					return E_NOTIMPL;
				}
				if(szConnCmd == NULL)
				{
					WrightLogEx(pHHC->szIP, "设置历史录像回调失败，连接命令为空");
					return E_FAIL;
				}
				if(!strstr(szConnCmd, "DownloadVideo") || 
					!strstr(szConnCmd, "Enable[1]"))
				{
					WrightLogEx(pHHC->szIP, "设置历史录像回调失败，连接命令有误");
					return E_FAIL;
				}
				WrightLogEx(pHHC->szIP, "设置历史录像回调");
				if(pHHC->dwVideoConnStatus == CONN_STATUS_UNKNOWN
					|| pHHC->dwVideoConnStatus == CONN_STATUS_DISCONN
					|| pHHC->dwVideoConnStatus == CONN_STATUS_RECVDONE)
				{
					if(ConnectCamera(pHHC->szIP, CAMERA_VIDEO_LINK_PORT, pHHC->sktVideo))
					{
						strcpy((char*)pHHC->szVideoConnCmd, szConnCmd);
						char szRetBuf[256] = {0};
						char szTmpConnCmd[128] = {0};
						memcpy(szTmpConnCmd, (char*)pHHC->szVideoConnCmd, 128);
						if(HvSendXmlCmd(pHHC->szIP, szTmpConnCmd, szRetBuf, sizeof(szRetBuf),
							NULL, pHHC->sktVideo) != S_OK)
						{
							ForceCloseSocket(pHHC->sktVideo);
							pHHC->fIsConnectHistoryVideo = FALSE;
							WrightLogEx(pHHC->szIP, "设置历史录像回调失败，发送XML命令失败");
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
								pTemp->pOnRecoreH264HistoryVideo = (HVAPI_CALLBACK_H264_EX)pFunc;
								pTemp->pOnHistoryVideoParam = (PVOID)pUserData;
								break;
							}
						}
						if(pTemp == NULL)
						{
							pTemp = new HVAPI_CALLBACK_SET();
							memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
							pTemp->iVideoID = iVideoID;
							pTemp->pOnRecoreH264HistoryVideo = (HVAPI_CALLBACK_H264_EX)pFunc;
							pTemp->pOnHistoryVideoParam = (PVOID)pUserData;
							pTemp->pNext = pHHC->pCallBackSet;
							pHHC->pCallBackSet = pTemp;
						}
						pHHC->fIsConnectHistoryVideo = TRUE;
						pHHC->fIsThreadRecvVideoExit = FALSE;
						pHHC->hThreadRecvVideo = CreateThread(NULL, STACK_SIZE, VideoRecvThreadFuncEx, pHHC, 0, NULL);
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
								pTemp->pOnRecoreH264HistoryVideo = NULL;
								pTemp->pOnHistoryVideoParam = NULL;
								break;
							}
						}
						WrightLogEx(pHHC->szIP, "设置历史录像回调失败，连接一体机失败");
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
								pTemp->pOnRecoreH264HistoryVideo = (HVAPI_CALLBACK_H264_EX)pFunc;
								pTemp->pOnHistoryVideoParam = (PVOID)pUserData;
								break;
							}
						}
						if(pTemp == NULL)
						{
							pTemp = new HVAPI_CALLBACK_SET();
							memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
							pTemp->iVideoID = iVideoID;
							pTemp->pOnRecoreH264HistoryVideo = (HVAPI_CALLBACK_H264_EX)pFunc;
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
								pTemp->pOnRecoreH264HistoryVideo = NULL;
								pTemp->pOnHistoryVideoParam = NULL;
								break;
							}
						}
						WrightLogEx(pHHC->szIP, "设置历史录像回调失败，历史录像连接依然存在");
						return E_FAIL;
					}
				}
				WrightLogEx(pHHC->szIP, "设置历史录像回调成功");
			}
			else
			{
				WrightLogEx(pHHC->szIP, "取消历史录像回调");
				if(pHHC->sktVideo != INVALID_SOCKET)
				{
					ForceCloseSocket(pHHC->sktVideo);
				}
				pHHC->fIsThreadRecvVideoExit = TRUE;
				HvSafeCloseThread(pHHC->hThreadRecvVideo);
				ForceCloseSocket(pHHC->sktVideo);
				pHHC->dwVideoConnStatus = CONN_STATUS_DISCONN;
				memset(pHHC->szVideoConnCmd, 0, 128);

				HVAPI_CALLBACK_SET* pTemp = NULL;
				for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
				{
					if(pTemp->iVideoID == iVideoID)
					{
						pTemp->pOnRecoreH264HistoryVideo = NULL;
						pTemp->pOnHistoryVideoParam = NULL;
						break;
					}
				}
				pHHC->fIsConnectHistoryVideo = FALSE;
				WrightLogEx(pHHC->szIP, "取消历史录像回调成功");
			}
		}
		break;
	case CALLBACK_TYPE_ADV_MJPEG:
		{
			if(pFunc)
			{
				CCSLockEx sLock(&pHHC->csCallbackJpeg);
				if (pHHC->fAutoLink)
				{
					WrightLogEx(pHHC->szIP, "设置JPEG回调失败，存在主动连接");
					return E_NOTIMPL;
				}
				WrightLogEx(pHHC->szIP, "设置JPEG回调");
				if(pHHC->dwImageConnStatus == CONN_STATUS_UNKNOWN
					|| pHHC->dwImageConnStatus == CONN_STATUS_DISCONN)
				{
					if(ConnectCamera(pHHC->szIP, CAMERA_IMAGE_LINK_PORT, pHHC->sktImage))
					{
						if(szConnCmd != NULL && sizeof(pHHC->szImageConnCmd) > strlen(szConnCmd))
						{
							strcpy((char*)pHHC->szImageConnCmd, szConnCmd);
							char szRetBuf[256] = {0};
							char szTmpConnCmd[128] = {0};
							memcpy(szTmpConnCmd, (char*)pHHC->szImageConnCmd, 128);

							if(HvSendXmlCmd(pHHC->szIP, szTmpConnCmd, szRetBuf, sizeof(szRetBuf),
								NULL, pHHC->sktImage) != S_OK)
							{
								ForceCloseSocket(pHHC->sktImage);
								WrightLogEx(pHHC->szIP, "设置JPEG回调失败，发送XML命令失败");
								return E_FAIL;
							}
						}
						else
						{
							memset(pHHC->szImageConnCmd, 0, 128);
						}
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnMJPEG = (HVAPI_CALLBACK_JPEG_EX  )pFunc;
								pTemp->pOnJpegFrameParam = (PVOID)pUserData;
								break;
							}
						}
						if(pTemp == NULL)
						{
							pTemp = new HVAPI_CALLBACK_SET();
							memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
							pTemp->iVideoID = iVideoID;
							pTemp->pOnMJPEG = (HVAPI_CALLBACK_JPEG_EX  )pFunc;
							pTemp->pOnJpegFrameParam = (PVOID)pUserData;
							pTemp->pNext = pHHC->pCallBackSet;
							pHHC->pCallBackSet = pTemp;
						}
						pHHC->fIsThreadRecvImageExit = FALSE;
						pHHC->hThreadRecvImage = CreateThread(NULL, STACK_SIZE, ImageRecvThreadFuncEx, pHHC, 0, NULL);
						pHHC->dwImageConnStatus = CONN_STATUS_NORMAL;
					}
					else
					{
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnMJPEG = NULL;
								pTemp->pOnJpegFrameParam = NULL;
								break;
							}
						}
						WrightLogEx(pHHC->szIP, "设置JPEG回调失败，连接相机失败");
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
							pTemp->pOnMJPEG = (HVAPI_CALLBACK_JPEG_EX  )pFunc;
							pTemp->pOnJpegFrameParam = (PVOID)pUserData;
							break;
						}
					}
					if(pTemp == NULL)
					{
						pTemp = new HVAPI_CALLBACK_SET();
						memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
						pTemp->iVideoID = iVideoID;
						pTemp->pOnMJPEG = (HVAPI_CALLBACK_JPEG_EX  )pFunc;
						pTemp->pOnJpegFrameParam = (PVOID)pUserData;
						pTemp->pNext = pHHC->pCallBackSet;
						pHHC->pCallBackSet = pTemp;
					}
					WrightLogEx(pHHC->szIP, "设置JPEG回调成功");
					return S_OK;
				}
				WrightLogEx(pHHC->szIP, "设置JPEG回调成功");
			}
			else
			{	
				WrightLogEx(pHHC->szIP, "取消JPEG回调");
				pHHC->dwImageConnStatus = CONN_STATUS_DISCONN;
				pHHC->fIsThreadRecvImageExit = TRUE;
				HvSafeCloseThread(pHHC->hThreadRecvImage);
							
				ForceCloseSocket(pHHC->sktImage);
				memset(pHHC->szImageConnCmd, 0, 128);

				HVAPI_CALLBACK_SET* pTemp = NULL;
				for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
				{
					if(pTemp->iVideoID == iVideoID)
					{
						pTemp->pOnMJPEG = NULL;
						pTemp->pOnJpegFrameParam = NULL;
						break;
					}
				}
				WrightLogEx(pHHC->szIP, "取消JPEG回调成功");
			}
		}
		break;
	case CALLBACK_TYPE_ADV_RECORD_RESULT:
		{	
			if (pFunc)
			{
				char szConnCmdReplace[128];
				char* pszConnCmdReplace = NULL;
				CCSLockEx sLock(&pHHC->csCallbackRecord);
				WrightLogEx(pHHC->szIP, "设置识别结果回调");

				bool HasIllegalVideo = false;
				for(HVAPI_CALLBACK_SET* pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
				{
					if(pTemp != NULL  && pTemp->pOnIllegalVideo != NULL)
					{						
						HasIllegalVideo = true;
						break;
					}
					
				}
						
				if (NULL == szConnCmd)
				{
					if(HasIllegalVideo)
					{
						strcpy(szConnCmdReplace, "DownloadRecord,BeginTime[0],Index[0],Enable[0],EndTime[0],DataInfo[0],SendIllegalVideo[1]");
					}
					pszConnCmdReplace = szConnCmdReplace;
				}
				else
				{
					if (sizeof(szConnCmdReplace) > strlen(szConnCmd))
					{
						char* pIllegalVideo = strstr(szConnCmd,"SendIllegalVideo[");
						if (pIllegalVideo == NULL)
						{
							//追加SendIllegalVideo属性
							if (sizeof(szConnCmdReplace)  > strlen(szConnCmd) + strlen(",SendIllegalVideo[1]"))
							{
								strcpy(szConnCmdReplace, szConnCmd);
								strcat(szConnCmdReplace, ",SendIllegalVideo[1]");
							}
							else
							{
								strcpy(szConnCmdReplace, szConnCmd);
							}
						}
						else
						{
							//强制覆盖设置SendIllegalVideo属性为1														
							strcpy(szConnCmdReplace, szConnCmd);
							char* pIllegalVideoReplace = strstr(szConnCmdReplace,"SendIllegalVideo[");
							if(strlen(pIllegalVideoReplace)  >=  strlen("SendIllegalVideo[1]"))//pIllegalVideoReplace能进行覆盖操作，不会溢出
							{
								memcpy(pIllegalVideoReplace, "SendIllegalVideo[1]", strlen("SendIllegalVideo[1]"));
							}
						}	
						pszConnCmdReplace = szConnCmdReplace;						
						
					}
					else
					{
						memset(szConnCmdReplace, 0, sizeof(szConnCmdReplace));
						pszConnCmdReplace = NULL;
					}
				}
				
				if(pHHC->fAutoLink)
				{
					if (INVALID_SOCKET != pHHC->sktRecord)
					{
						ForceCloseSocket(pHHC->sktRecord);
					}

					if ( pszConnCmdReplace != NULL && sizeof(pHHC->szRecordConnCmd) > strlen(pszConnCmdReplace) )
					{
						strcpy((char*)pHHC->szRecordConnCmd, pszConnCmdReplace);
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
							pTemp->pOnRecordResult = (HVAPI_CALLBACK_RESULT )pFunc;
							pTemp->pOnPlateParam = (PVOID)pUserData;
							break;
						}
					}
					if(pTemp == NULL)
					{
						pTemp = new HVAPI_CALLBACK_SET();
						memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
						pTemp->iVideoID = iVideoID;
						pTemp->pOnRecordResult = (HVAPI_CALLBACK_RESULT )pFunc;
						pTemp->pOnPlateParam = (PVOID)pUserData;
						pTemp->pNext = pHHC->pCallBackSet;
						pHHC->pCallBackSet = pTemp;
					}
					//	pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
					pHHC->fVailPackResumeCache = FALSE;
					pHHC->AutoLinkSetCallBackTime  = time(NULL);
					SetConnAutoLinkHHC(pHHC->szDevSN);
					pHHC->fIsRecordPlateExit = FALSE;
					WrightLogEx(pHHC->szIP, "主动连接设置识别结果回调成功");
					return S_OK;
				}

				if ((pHHC->dwRecordConnStatus == CONN_STATUS_UNKNOWN
					|| pHHC->dwRecordConnStatus == CONN_STATUS_DISCONN
					|| pHHC->dwRecordConnStatus == CONN_STATUS_CONNFIRST)
					&& pHHC->fIsRecordStringExit == TRUE)
				{
					if (ConnectCamera(pHHC->szIP, CAMERA_RECORD_LINK_PORT, pHHC->sktRecord))
					{
						if ( pszConnCmdReplace != NULL && sizeof(pHHC->szRecordConnCmd) > strlen(pszConnCmdReplace))
						{
							strcpy((char*)pHHC->szRecordConnCmd, pszConnCmdReplace);
							char szRetBuf[256] = {0};
							char szTmpConnCmd[128] = {0};
							memcpy(szTmpConnCmd, (char*)pHHC->szRecordConnCmd, 128);
							if(HvSendXmlCmd(pHHC->szIP, szTmpConnCmd, szRetBuf, sizeof(szRetBuf),
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
										memcpy(pHHC->szRecordEndTimeStr, pTempStr+8, 13);
										pHHC->szRecordEndTimeStr[13] = '\0';
									}
								}

								pTempStr = strstr((char*)pHHC->szRecordConnCmd, "DataInfo");
								if(pTempStr)
								{
									sscanf(pTempStr, "DataInfo[%d]", &pHHC->iRecordDataInfo);
								}

								pTempStr = strstr((char*)pHHC->szRecordConnCmd, "SendIllegalVideo");
								if(pTempStr)
								{
									sscanf(pTempStr, "SendIllegalVideo[%d]", &pHHC->iRealTimeRecordSendIllegalVideo);
								}
							}
						}
						else
						{
							memset(pHHC->szRecordConnCmd, 0, 128);
						}
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnRecordResult = (HVAPI_CALLBACK_RESULT )pFunc;
								pTemp->pOnPlateParam = (PVOID)pUserData;
								break;
							}
						}
						if(pTemp == NULL)
						{
							pTemp = new HVAPI_CALLBACK_SET();
							memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
							pTemp->iVideoID = iVideoID;
							pTemp->pOnRecordResult = (HVAPI_CALLBACK_RESULT )pFunc;
							pTemp->pOnPlateParam = (PVOID)pUserData;
							pTemp->pNext = pHHC->pCallBackSet;
							pHHC->pCallBackSet = pTemp;
						}

						pHHC->fIsThreadRecvRecordExit = FALSE;
						pHHC->hThreadRecvRecord = CreateThread(NULL, STACK_SIZE, RecordRecvThreadFuncEx, pHHC, 0, NULL);
						pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
					}
					else
					{
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnRecordResult = NULL;
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
							pTemp->pOnRecordResult = (HVAPI_CALLBACK_RESULT )pFunc;
							pTemp->pOnPlateParam = (PVOID)pUserData;
							break;
						}
					}
					if(pTemp == NULL)
					{
						pTemp = new HVAPI_CALLBACK_SET();
						memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
						pTemp->iVideoID = iVideoID;
						pTemp->pOnRecordResult = (HVAPI_CALLBACK_RESULT )pFunc;
						pTemp->pOnPlateParam = (PVOID)pUserData;
						pTemp->pNext = pHHC->pCallBackSet;
						pHHC->pCallBackSet = pTemp;
					}
					WrightLogEx(pHHC->szIP, "设置识别结果回调成功");
					pHHC->fIsRecordPlateExit = FALSE;
					return S_OK;
				}
				pHHC->fIsRecordPlateExit = FALSE;
				WrightLogEx(pHHC->szIP, "设置识别结果回调成功");
			}
			else
			{	
				WrightLogEx(pHHC->szIP, "取消识别结果回调");

				pHHC->fIsThreadRecvRecordExit = TRUE;
				HvSafeCloseThread(pHHC->hThreadRecvRecord);
				ForceCloseSocket(pHHC->sktRecord);
				pHHC->hThreadRecvRecord = NULL;	

				pHHC->fIsRecordStringExit = TRUE;
				pHHC->fIsRecordPlateExit = TRUE;

				pHHC->dwRecordConnStatus = CONN_STATUS_DISCONN;

				memset(pHHC->szRecordConnCmd, 0, 128);

				HVAPI_CALLBACK_SET* pTemp = NULL;
				for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
				{
					if(pTemp->iVideoID == iVideoID)
					{
						pTemp->pOnRecordResult = NULL;
						pTemp->pOnPlateParam = NULL;
						break;
					}
				}
				pHHC->fVailPackResumeCache = FALSE;
				WrightLogEx(pHHC->szIP, "取消识别结果回调成功");
			}
		}

		break;
		/*
	case CALLBACK_TYPE_ADV_HISTORY_RECORD_RESULT:
		{	
			if (pFunc)
			{
				CCSLockEx sLock(&pHHC->csCallbackRecord);
				WrightLogEx(pHHC->szIP, "设置识别结果回调");
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
							pTemp->pOnRecordHistoryResult = (HVAPI_CALLBACK_RESULT )pFunc;
							pTemp->pOnRecordHistoryResultParam = (PVOID)pUserData;
							break;
						}
					}
					if(pTemp == NULL)
					{
						pTemp = new HVAPI_CALLBACK_SET();
						memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
						pTemp->iVideoID = iVideoID;
						pTemp->pOnRecordHistoryResult = (HVAPI_CALLBACK_RESULT )pFunc;
						pTemp->pOnRecordHistoryResultParam = (PVOID)pUserData;
						pTemp->pNext = pHHC->pCallBackSet;
						pHHC->pCallBackSet = pTemp;
					}
					//	pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
					pHHC->fVailPackResumeCache = FALSE;
					pHHC->AutoLinkSetCallBackTime  = time(NULL);
					SetConnAutoLinkHHC(pHHC->szDevSN);
					pHHC->fIsRecordPlateExit = FALSE;
					WrightLogEx(pHHC->szIP, "主动连接设置识别结果回调成功");
					return S_OK;
				}

				if ((pHHC->dwRecordConnStatus == CONN_STATUS_UNKNOWN
					|| pHHC->dwRecordConnStatus == CONN_STATUS_DISCONN
					|| pHHC->dwRecordConnStatus == CONN_STATUS_CONNFIRST)
					&& pHHC->fIsRecordStringExit == TRUE)
				{
					if (ConnectCamera(pHHC->szIP, CAMERA_RECORD_LINK_PORT, pHHC->sktRecord))
					{
						if ( szConnCmd != NULL && sizeof(pHHC->szRecordConnCmd) > strlen(szConnCmd))
						{
							strcpy((char*)pHHC->szRecordConnCmd, szConnCmd);
							char szRetBuf[256] = {0};
							char szTmpConnCmd[128] = {0};
							memcpy(szTmpConnCmd, (char*)pHHC->szRecordConnCmd, 128);
							if(HvSendXmlCmd(pHHC->szIP, szTmpConnCmd, szRetBuf, sizeof(szRetBuf),
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
										memcpy(pHHC->szRecordEndTimeStr, pTempStr+8, 13);
										pHHC->szRecordEndTimeStr[13] = '\0';
									}
								}

								pTempStr = strstr((char*)pHHC->szRecordConnCmd, "DataInfo");
								if(pTempStr)
								{
									sscanf(pTempStr, "DataInfo[%d]", &pHHC->iRecordDataInfo);
								}

								pTempStr = strstr((char*)pHHC->szRecordConnCmd, "SendIllegalVideo");
								if(pTempStr)
								{
									sscanf(pTempStr, "SendIllegalVideo[%d]", &pHHC->iRealTimeRecordSendIllegalVideo);
								}
							}
						}
						else
						{
							memset(pHHC->szRecordConnCmd, 0, 128);
						}
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnRecordHistoryResult = (HVAPI_CALLBACK_RESULT )pFunc;
								pTemp->pOnRecordHistoryResultParam = (PVOID)pUserData;
								break;
							}
						}
						if(pTemp == NULL)
						{
							pTemp = new HVAPI_CALLBACK_SET();
							memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
							pTemp->iVideoID = iVideoID;
							pTemp->pOnRecordHistoryResult = (HVAPI_CALLBACK_RESULT )pFunc;
							pTemp->pOnRecordHistoryResultParam = (PVOID)pUserData;
							pTemp->pNext = pHHC->pCallBackSet;
							pHHC->pCallBackSet = pTemp;
						}

						pHHC->fIsThreadRecvRecordExit = FALSE;
						pHHC->hThreadRecvRecord = CreateThread(NULL, STACK_SIZE, RecordRecvThreadFuncEx, pHHC, 0, NULL);
						pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
					}
					else
					{
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnRecordHistoryResult = NULL;
								pTemp->pOnRecordHistoryResultParam = NULL;
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
							pTemp->pOnRecordHistoryResult = (HVAPI_CALLBACK_RESULT )pFunc;
							pTemp->pOnRecordHistoryResultParam = (PVOID)pUserData;
							break;
						}
					}
					if(pTemp == NULL)
					{
						pTemp = new HVAPI_CALLBACK_SET();
						memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
						pTemp->iVideoID = iVideoID;
						pTemp->pOnRecordHistoryResult = (HVAPI_CALLBACK_RESULT )pFunc;
						pTemp->pOnRecordHistoryResultParam = (PVOID)pUserData;
						pTemp->pNext = pHHC->pCallBackSet;
						pHHC->pCallBackSet = pTemp;
					}
					WrightLogEx(pHHC->szIP, "设置识别结果回调成功");
					pHHC->fIsRecordPlateExit = FALSE;
					return S_OK;
				}
				pHHC->fIsRecordPlateExit = FALSE;
				WrightLogEx(pHHC->szIP, "设置识别结果回调成功");
			}
			else
			{	
				WrightLogEx(pHHC->szIP, "取消识别结果回调");

				pHHC->fIsThreadRecvRecordExit = TRUE;
				HvSafeCloseThread(pHHC->hThreadRecvRecord);
				ForceCloseSocket(pHHC->sktRecord);
				pHHC->hThreadRecvRecord = NULL;	

				pHHC->fIsRecordStringExit = TRUE;
				pHHC->fIsRecordPlateExit = TRUE;

				pHHC->dwRecordConnStatus = CONN_STATUS_DISCONN;

				memset(pHHC->szRecordConnCmd, 0, 128);

				HVAPI_CALLBACK_SET* pTemp = NULL;
				for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
				{
					if(pTemp->iVideoID == iVideoID)
					{
						pTemp->pOnRecordHistoryResult = NULL;
						pTemp->pOnRecordHistoryResultParam = NULL;
						break;
					}
				}
				pHHC->fVailPackResumeCache = FALSE;
				WrightLogEx(pHHC->szIP, "取消识别结果回调成功");
			}
		}

		break;
	*/
	case CALLBACK_TYPE_REDEVELOP_RESULT:
		{	
			if (pFunc)
			{
				CCSLockEx sLock(&pHHC->csCallbackRecord);
				WrightLogEx(pHHC->szIP, "设置二次开发结果回调");
	 
				if ((pHHC->dwRecordConnStatus == CONN_STATUS_UNKNOWN
					|| pHHC->dwRecordConnStatus == CONN_STATUS_DISCONN
					|| pHHC->dwRecordConnStatus == CONN_STATUS_CONNFIRST)
					&& pHHC->fIsRecordStringExit == TRUE)
				{
					if (ConnectCamera(pHHC->szIP, CAMERA_RECORD_LINK_PORT, pHHC->sktRecord))
					{
						if ( szConnCmd != NULL && sizeof(pHHC->szRecordConnCmd) > strlen(szConnCmd))
						{
							strcpy((char*)pHHC->szRecordConnCmd, szConnCmd);
							char szRetBuf[256] = {0};
							char szTmpConnCmd[128] = {0};
							memcpy(szTmpConnCmd, (char*)pHHC->szRecordConnCmd, 128);
							if(HvSendXmlCmd(pHHC->szIP, szTmpConnCmd, szRetBuf, sizeof(szRetBuf),
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
										memcpy(pHHC->szRecordEndTimeStr, pTempStr+8, 13);
										pHHC->szRecordEndTimeStr[13] = '\0';
									}
								}

								pTempStr = strstr((char*)pHHC->szRecordConnCmd, "DataInfo");
								if(pTempStr)
								{
									sscanf(pTempStr, "DataInfo[%d]", &pHHC->iRecordDataInfo);
								}

								pTempStr = strstr((char*)pHHC->szRecordConnCmd, "SendIllegalVideo");
								if(pTempStr)
								{
									sscanf(pTempStr, "SendIllegalVideo[%d]", &pHHC->iRealTimeRecordSendIllegalVideo);
								}
							}
						}
						else
						{
							memset(pHHC->szRecordConnCmd, 0, 128);
						}
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnReDevelopResult = (HVAPI_CALLBACK_REDEVELOP_RESULT )pFunc;
								pTemp->pOnReDevelopResultParam = (PVOID)pUserData;
								break;
							}
						}
						if(pTemp == NULL)
						{
							pTemp = new HVAPI_CALLBACK_SET();
							memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
							pTemp->iVideoID = iVideoID;
							pTemp->pOnReDevelopResult = (HVAPI_CALLBACK_REDEVELOP_RESULT )pFunc;
							pTemp->pOnReDevelopResultParam = (PVOID)pUserData;
							pTemp->pNext = pHHC->pCallBackSet;
							pHHC->pCallBackSet = pTemp;
						}

						pHHC->fIsThreadRecvRecordExit = FALSE;
						pHHC->hThreadRecvRecord = CreateThread(NULL, STACK_SIZE, RecordRecvThreadFuncEx, pHHC, 0, NULL);
						pHHC->dwRecordConnStatus = CONN_STATUS_NORMAL;
					}
					else
					{
						HVAPI_CALLBACK_SET* pTemp = NULL;
						for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
						{
							if(pTemp->iVideoID == iVideoID)
							{
								pTemp->pOnReDevelopResult = NULL;
								pTemp->pOnReDevelopResultParam = NULL;
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
							pTemp->pOnReDevelopResult = (HVAPI_CALLBACK_REDEVELOP_RESULT )pFunc;
							pTemp->pOnReDevelopResultParam = (PVOID)pUserData;
							break;
						}
					}
					if(pTemp == NULL)
					{
						pTemp = new HVAPI_CALLBACK_SET();
						memset(pTemp, 0, sizeof(HVAPI_CALLBACK_SET));
						pTemp->iVideoID = iVideoID;
						pTemp->pOnReDevelopResult = (HVAPI_CALLBACK_REDEVELOP_RESULT )pFunc;
						pTemp->pOnReDevelopResultParam = (PVOID)pUserData;
						pTemp->pNext = pHHC->pCallBackSet;
						pHHC->pCallBackSet = pTemp;
					}
					WrightLogEx(pHHC->szIP, "设置二次开发结果回调成功");
					pHHC->fIsRecordPlateExit = FALSE;
					return S_OK;
				}
				pHHC->fIsRecordPlateExit = FALSE;
				WrightLogEx(pHHC->szIP, "设置二次开发结果回调成功");
			}
			else
			{	
				WrightLogEx(pHHC->szIP, "取消二次开发结果回调");

				pHHC->fIsThreadRecvRecordExit = TRUE;
				HvSafeCloseThread(pHHC->hThreadRecvRecord);
				ForceCloseSocket(pHHC->sktRecord);
				pHHC->hThreadRecvRecord = NULL;	

				pHHC->fIsRecordStringExit = TRUE;
				pHHC->fIsRecordPlateExit = TRUE;

				pHHC->dwRecordConnStatus = CONN_STATUS_DISCONN;

				memset(pHHC->szRecordConnCmd, 0, 128);

				HVAPI_CALLBACK_SET* pTemp = NULL;
				for(pTemp = pHHC->pCallBackSet; pTemp != NULL; pTemp = pTemp->pNext)
				{
					if(pTemp->iVideoID == iVideoID)
					{
						pTemp->pOnReDevelopResult = NULL;
						pTemp->pOnReDevelopResultParam = NULL;
						break;
					}
				}
				pHHC->fVailPackResumeCache = FALSE;
				WrightLogEx(pHHC->szIP, "取消二次开发结果回调成功");
			}
		}

		break;
	}

	return S_OK;
}



HV_API_EX HRESULT CDECL HVAPI_StartRecvH264Video(
	HVAPI_HANDLE_EX hHandle,
	PVOID pFunc,
	PVOID pUserData,
	INT iVideoID ,    
	DWORD64 dw64BeginTimeMS,
	DWORD64 dw64EndTimeMS,
	DWORD dwRecvFlag)
{
	CTime cBeginTime(dw64BeginTimeMS);
	CTime cEndTime(dw64EndTimeMS);
	CHAR szCmd[256];

	HRESULT hr = S_OK;
	DWORD dwCallBackType = 0; 

	if ( H264_RECV_FLAG_REALTIME ==  dwRecvFlag )
	{
		sprintf( szCmd ,"DownloadVideo,BeginTime[0],EndTime[0],Enable[0]");
		dwCallBackType = CALLBACK_TYPE_ADV_VIDEO;
	}
	else if ( H264_RECV_FLAG_HISTORY == dwRecvFlag )
	{
		if ( 0 != dw64BeginTimeMS && 0 == dw64EndTimeMS )
		{
			sprintf( szCmd ,"DownloadVideo,BeginTime[%s],EndTime[0],Enable[1]",
				cBeginTime.Format("%Y.%m.%d_%H:%M:%S") );
		}
		else if ( 0 != dw64BeginTimeMS && 0 != dw64EndTimeMS && dw64BeginTimeMS < dw64EndTimeMS )
		{
			sprintf( szCmd ,"DownloadVideo,BeginTime[%s],EndTime[%s],Enable[1]",
				cBeginTime.Format("%Y.%m.%d_%H:%M:%S") , cEndTime.Format("%Y.%m.%d_%H:%M:%S") );
		}
		else
		{
			WrightLogEx( ((HVAPI_HANDLE_CONTEXT_EX*)(hHandle))->szIP, "HVAPI_StartRecvH264Video Parameter error:  the dw64BeginTimeMS and dw64EndTimeMS value relations is EOR" );
			return E_FAIL;
		}
		dwCallBackType = CALLBACK_TYPE_ADV_HISTORY_VIDEO;
	}
	else
	{
		WrightLogEx( ((HVAPI_HANDLE_CONTEXT_EX*)(hHandle))->szIP, "HVAPI_StartRecvH264Video  Parameter error: dwRecvFlag value is EOR" );
		return E_FAIL;
	}
	WrightLogEx( szCmd , "");
	return HVAPI_SetCallBackEx(hHandle, pFunc, pUserData,iVideoID, dwCallBackType, szCmd );
}


/**
* @brief              启动MJPEG接收
* @param[in]          hHandle				 设备句柄
* @param[in]          pFunc					 回调函数指针
* @param[in]          pUserData				 用户数据
* @param[in]          iVideoID				 视频通道，目前只使用
* @param[in]          dwRecvFlag			 接收标志
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_StartRecvMJPEG(
	HVAPI_HANDLE_EX hHandle,
	PVOID pFunc,
	PVOID pUserData,
	INT iVideoID ,
	DWORD dwRecvFlag
	)
{
    BOOL fEnableRecogVideo = (dwRecvFlag == MJPEG_RECV_FLAG_DEBUG);
	CHAR szConnCmd[128];

	sprintf(szConnCmd , "SetImgType,EnableRecogVideo[%d]", fEnableRecogVideo );
	WrightLogEx( szConnCmd , "");
	return HVAPI_SetCallBackEx(hHandle, pFunc, pUserData, iVideoID, CALLBACK_TYPE_ADV_MJPEG, szConnCmd  ); 
}


HV_API_EX HRESULT CDECL  HVAPI_StartRecvResult
(
	HVAPI_HANDLE_EX hHandle,
	PVOID pFunc,
	PVOID pUserData,
	INT iVideoID,
	DWORD64 dw64BeginTimeMS,
	DWORD64 dw64EndTimeMS,
	DWORD  dwStartIndex,
	DWORD  dwRecvFlag
 )
{
	CTime cBeginTime(dw64BeginTimeMS);
	CTime cEndTime(dw64EndTimeMS);
	CHAR szCmd[256];
	

	if ( RESULT_RECV_FLAG_REALTIME == dwRecvFlag )
	{
		sprintf(szCmd , "DownloadRecord,BeginTime[0],Index[0],Enable[0],EndTime[0],DataInfo[0]");
	}
	else if ( RESULT_RECV_FLAG_HISTORY == dwRecvFlag 
		   ||  RESULT_RECV_FLAG_HISTROY_ONLY_PECCANCY == dwRecvFlag )
	{
		BOOL fOnlyRecvPeccancy = dwRecvFlag == RESULT_RECV_FLAG_HISTROY_ONLY_PECCANCY;
		if( 0 != dw64BeginTimeMS && 0 == dw64EndTimeMS )
		{
			sprintf(szCmd ,"DownloadRecord,BeginTime[%s],Index[%d],Enable[1],EndTime[0],DataInfo[%d]",
				cBeginTime.Format("%Y.%m.%d_%H"), dwStartIndex,  fOnlyRecvPeccancy );
		}
		else if ( 0 != dw64BeginTimeMS && 0 != dw64EndTimeMS && dw64BeginTimeMS <= dw64EndTimeMS )
		{
			sprintf(szCmd ,"DownloadRecord,BeginTime[%s],Index[%d],Enable[1],EndTime[%s],DataInfo[%d]",
				cBeginTime.Format("%Y.%m.%d_%H"), dwStartIndex, cEndTime.Format("%Y.%m.%d_%H"), fOnlyRecvPeccancy );
		}
		else
		{
			WrightLogEx( ((HVAPI_HANDLE_CONTEXT_EX*)(hHandle))->szIP, "HVAPI_StartRecvResult Parameter error:  the dw64BeginTimeMS and dw64EndTimeMS value relations is EOR" );
			return E_FAIL;
		}
	}
	else
	{
		WrightLogEx( ((HVAPI_HANDLE_CONTEXT_EX*)(hHandle))->szIP, "HVAPI_StartRecvResult Parameter error:  dwRecvFlag value  is EOR" );
		return E_NOTIMPL;
	}
	WrightLogEx(szCmd , "");
	return  HVAPI_SetCallBackEx(hHandle, pFunc, pUserData, iVideoID, CALLBACK_TYPE_ADV_RECORD_RESULT, szCmd ); 

}




HV_API_EX HRESULT CDECL  HVAPI_StartRecvRedevelopResult
(
	HVAPI_HANDLE_EX hHandle,
	PVOID pFunc,
	PVOID pUserData
 )
{ 
	char szCmd[256];
	strcpy(szCmd , "DownloadRecord,BeginTime[0],Index[0],Enable[0],EndTime[0],DataInfo[0]");
	return  HVAPI_SetCallBackEx(hHandle, pFunc, pUserData, 0, CALLBACK_TYPE_REDEVELOP_RESULT, szCmd ); 
}

HV_API_EX HRESULT CDECL HVAPI_StopRecvH264Video(HVAPI_HANDLE_EX hHandle)
{
	if(hHandle == NULL) 
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;

	if(strcmp(pHHC->szVersion, HVAPI_API_VERSION_EX) != 0
		|| pHHC->dwOpenType != 1)
	{
		WrightLogEx("HVAPI_StopRecvH264Video err\n" ,"" );
		return E_FAIL;
	}
	
	HRESULT hr = S_OK ;
	if ( pHHC->fIsConnectHistoryVideo )
	{
		hr = HVAPI_SetCallBackEx( hHandle , NULL , NULL , 0 ,CALLBACK_TYPE_ADV_HISTORY_VIDEO ,NULL );
	}
	else
	{
		hr = HVAPI_SetCallBackEx( hHandle , NULL , NULL , 0 ,CALLBACK_TYPE_ADV_VIDEO ,NULL );
	}
	WrightLogEx("HVAPI_StopRecvH264Video Ok\n" ,"" );
	return hr;
}

HV_API_EX HRESULT CDECL  HVAPI_StopRecvMJPEG(HVAPI_HANDLE_EX hHandle)
{
	return HVAPI_SetCallBackEx( hHandle , NULL , NULL , 0 ,CALLBACK_TYPE_ADV_MJPEG  ,NULL );
}


HV_API_EX HRESULT CDECL  HVAPI_StopRecvResult(HVAPI_HANDLE_EX hHandle)
{
	return HVAPI_SetCallBackEx( hHandle , NULL , NULL , 0 ,CALLBACK_TYPE_ADV_RECORD_RESULT ,NULL );
}

HV_API_EX HRESULT CDECL  HVAPI_StopRecvRedevelopResult
(
	HVAPI_HANDLE_EX hHandle
 )
{ 
	return HVAPI_SetCallBackEx( hHandle , NULL , NULL , 0 ,CALLBACK_TYPE_REDEVELOP_RESULT ,NULL );
}





/**
* @brief              启动状态信息接收
* @param[in]          hHandle				 设备句柄
* @param[in]          pFunc					 回调函数指针
* @param[in]          pUserData				 用户数据
* @param[in]          iVideoID				 视频通道，目前只使用
* @param[in]          dwRecvFlag			 接收标志
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_StartRecvMsg(
	HVAPI_HANDLE_EX hHandle,
	PVOID pFunc,
	PVOID pUserData,
	INT iVideoID ,
	DWORD dwRecvFlag
	)
{
	return HVAPI_SetCallBackEx(hHandle, pFunc, pUserData, iVideoID, CALLBACK_TYPE_STRING, NULL  ); 
}
/**
* @brief              停止状态信息接收
* @param[in]          hHandle          设备句柄
* @return             成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL  HVAPI_StopRecvMsg(HVAPI_HANDLE_EX hHandle)
{
	return HVAPI_SetCallBackEx(hHandle, NULL, NULL, 0, CALLBACK_TYPE_STRING, NULL  ); 
}

HV_API_EX HRESULT CDECL HVAPI_SetEnhanceRedLightFlagEx(HVAPI_HANDLE_EX hHandle, DWORD dwEnhanceStyle, INT iBrightness, INT iHubThreshold, INT iCompressRate)
{
	if(hHandle == NULL)
	{
		return E_FAIL;
	}
	if(dwEnhanceStyle<0 || dwEnhanceStyle>4) 
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
					/*
					if(!strstr(pszPlateInfoBuf, "车辆检测时间:"))
					{
						strcat(pszPlateInfoBuf, "\n车辆检测时间:");
						DWORD64 dw64TimeMs = ((DWORD64)dwTimeHigh<<32) | (DWORD64)dwTimeLow;
						char szTmpTime[20] = {0};
						sprintf(szTmpTime, "%I64u", dw64TimeMs);
						strcat(pszPlateInfoBuf, szTmpTime);
					}*/
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

//地球新接口 注释待整理
//
HV_API_EX HV_API_EX HRESULT CDECL HVAPI_SetStreamFps(HVAPI_HANDLE_EX hHandle, INT nStreamId, INT nFps)
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
	sprintf( szCmd , "SetStreamFps,StreamId[%d],Fps[%d]" ,nStreamId , nFps );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetStreamFps HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}
	
	//解析命令返回
	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );


	if ( S_OK != HvXmlParse("SetStreamFps", szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetStreamFps HvXmlParse Fail\n");
		return E_FAIL;
	}

	INT nRetCode = -1;
	if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
	{
		nRetCode = atoi(cRetInfo.szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}


HV_API_EX HRESULT CDECL HVAPI_SetH264BitRateControl(HVAPI_HANDLE_EX hHandle, INT nType )
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
	sprintf( szCmd , "SetH264BitRateControl,H264ControlMode[%d]" ,nType );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "SetH264BitRateControl HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetH264BitRateControl", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "SetH264BitRateControl HvXmlParse Fail\n");
			return E_FAIL;
		}

 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetH264BitRateControl", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "SetH264BitRateControl HvXmlParse Fail\n");
			return E_FAIL;
		}
		nRetCode = atoi(cRetInfo.szKeyValue);
	}
	
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetH264BitRate(HVAPI_HANDLE_EX hHandle, INT nBitRate )
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
	sprintf( szCmd , "SetH264BitRate,H264BitRate[%d]" ,nBitRate );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "SetH264BitRate HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}


	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		//解析命令返回
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetH264BitRate", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "SetH264BitRate HvXmlParse Fail\n");
			return E_FAIL;
		}
		INT nRetCode = -1;
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

		return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
	}
	else
	{
		//解析命令返回
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetH264BitRate", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "SetH264BitRate HvXmlParse Fail\n");
			return E_FAIL;
		}
		INT nRetCode = -1;
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
		return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
	}

}

HV_API_EX HRESULT CDECL HVAPI_SetJpegCompressRate(HVAPI_HANDLE_EX hHandle, INT nJpegCompressRate )
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
	sprintf( szCmd , "SetJpegCompressRate,JpegQuality[%d]" ,nJpegCompressRate );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "SetJpegCompressRate HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetJpegCompressRate", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "SetJpegCompressRate HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetJpegCompressRate", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "SetJpegCompressRate HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}



	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
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
			WrightLogEx(pHHC->szIP , "SetOSD Enable HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		//解析命令返回
		INT nRetCode = -1;

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetOSDEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "SetOSD Enable HvXmlParse Fail\n");
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
				WrightLogEx(pHHC->szIP , "SetOSDEnable HVAPI_ExecCmdEx Fail\n");
				return E_FAIL;
			}

			//解析命令返回
			INT nRetCode = -1;

			CXmlParseInfo cRetInfo;
			strcpy( cRetInfo.szKeyName, "RetCode" );

			if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
			{
				WrightLogEx(pHHC->szIP , "SetOSDEnable HvXmlParse Fail\n");
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
		WrightLogEx(pHHC->szIP , "SetOSDPlateEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "SetOSDPlateEnable HvXmlParse Fail\n");
		return E_FAIL;
	}


	nRetCode = atoi(cRetInfo.szKeyValue);


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
			WrightLogEx(pHHC->szIP , "SetOSDFont HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		//解析命令返回
 
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "SetOSDFont HvXmlParse Fail\n");
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
		WrightLogEx(pHHC->szIP , "HVAPI_SetOSDFontRGB HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回

	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetOSDFontRGB HvXmlParse Fail\n");
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
			WrightLogEx(pHHC->szIP , "HVAPI_SetOSDTimeEnable HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetOSDTimeEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetOSDTimeEnable HvXmlParse Fail\n");
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
			WrightLogEx(pHHC->szIP , "HVAPI_SetOSDTimeEnable HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetOSDTimeEnable HvXmlParse Fail\n");
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
			WrightLogEx(pHHC->szIP , "HVAPI_SetOSDPos HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParse( "SetOSDPos", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetOSDPos HvXmlParse Fail\n");
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
			WrightLogEx(pHHC->szIP , "HVAPI_SetOSDPos HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetOSDPos HvXmlParse Fail\n");
			return E_FAIL;
		}

 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}

	
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
			WrightLogEx(pHHC->szIP , "HVAPI_SetOSDFont HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		//解析命令返回
		INT nRetCode = -1;


		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetOSDFont", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetOSDFont HvXmlParse Fail\n");
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
			WrightLogEx(pHHC->szIP , "ExecuteCmd HVAPI_SetOSDText Fail\n" );
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
			WrightLogEx(pHHC->szIP , "HVAPI_SetOSDText HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		INT nRetCode=-1;
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetOSDText HvXmlParse Fail\n");
			return E_FAIL;
		}


		nRetCode = atoi(cRetInfo.szKeyValue);
		return nRetCode;
	
	}
return S_OK;
	
}

HV_API_EX HRESULT CDECL HVAPI_SetCVBDisPlayMode(HVAPI_HANDLE_EX hHandle, INT nMode )
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
	sprintf( szCmd , "SetCVBSDisplayMode,DisplayMode[%d]" ,nMode );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetCVBDisPlayMode HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParse( "SetCVBSDisplayMode", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetCVBDisPlayMode HvXmlParse Fail\n");
			return E_FAIL;
		}
	 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParseMercury( "SetCVBSDisplayMode", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetCVBDisPlayMode HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}



	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetManualShutter(HVAPI_HANDLE_EX hHandle, INT nShutter )
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
	sprintf( szCmd , "SetShutter,Shutter[%d]" ,nShutter );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetManualShutter HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );
		WrightLogEx("HVAPI_SetManualShutter" , szRet);
		if ( S_OK != HvXmlParse( "SetShutter", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetManualShutter HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );
		WrightLogEx("HVAPI_SetManualShutter" , szRet);
		if ( S_OK != HvXmlParseMercury( "SetShutter", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetManualShutter HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}



	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetManualGain(HVAPI_HANDLE_EX hHandle, INT nGain )
{
	if(hHandle == NULL )
	{
		return E_FAIL;
	}
	
	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	WrightLogEx("SetGainxx" , pHHC->szVersion);
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}
	//执行命令
	CHAR szCmd[128]={0};
	sprintf( szCmd , "SetGain,Gain[%d]" ,nGain );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetManualGain HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );
		WrightLogEx("SetGain" , szRet);
		if ( S_OK != HvXmlParse( "SetGain", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetManualGain HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );
		WrightLogEx("SetGain" , szRet);
		if ( S_OK != HvXmlParseMercury( "SetGain", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetManualGain HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}



	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetAWBEnable(HVAPI_HANDLE_EX hHandle, INT nEnable )
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
	sprintf( szCmd , "SetAWBEnable,Enable[%d]" ,nEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetAWBEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetAWBEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAWBEnable HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetAWBEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAWBEnable HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}

	

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetAWBMode(HVAPI_HANDLE_EX hHandle, INT nMode )
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
	sprintf( szCmd , "SetAWBMode,Mode[%d]" ,nMode );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetAWBMode HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetAWBMode", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAWBMode HvXmlParse Fail\n");
			return E_FAIL;
		}
	 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetAWBMode", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAWBMode HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}

	

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetManualRGB(HVAPI_HANDLE_EX hHandle, INT nGainR, INT nGainG, INT nGainB  )
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
	sprintf( szCmd , "SetRgbGain,GainR[%d],GainG[%d],GainB[%d]" , nGainR, nGainG,nGainB  );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetManualRGB HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParse( "SetRgbGain", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetManualRGB HvXmlParse Fail\n");
			return E_FAIL;
		}
	 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParseMercury( "SetRgbGain", szRet ,nRetLen , &cRetInfo, 1 ) ) //TODO为什么返回关键字是SetRGBGain，sdk在map表找不到
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetManualRGB HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}



HV_API_EX HRESULT CDECL HVAPI_SetAGCEnable(HVAPI_HANDLE_EX hHandle, INT nEnable )
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
	sprintf( szCmd , "SetAGCEnable,Enable[%d]" ,nEnable);

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetAGCEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParse( "SetAGCEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAGCEnable HvXmlParse Fail\n");
			return E_FAIL;
		}

		 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParseMercury( "SetAGCEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAGCEnable HvXmlParse Fail\n");
			return E_FAIL;
		}

 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetAGCLightBaseLine(HVAPI_HANDLE_EX hHandle, INT nLightBaseLine )
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
		sprintf( szCmd , "SetAGCLightBaseLine,LightBaseline[%d]" , nLightBaseLine  );

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAgcLightBaseLine HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}


		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParse( "SetAGCLightBaseLine", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAgcLightBaseLine HvXmlParse Fail\n");
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
		sprintf( szCmd , "SetAgcLightBaseline,LightBaseline[%d]" , nLightBaseLine  );

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAgcLightBaseLine HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}


		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParseMercury( "SetAgcLightBaseline", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAgcLightBaseLine HvXmlParse Fail\n");
			return E_FAIL;
		}

 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}

	
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}


HV_API_EX HRESULT CDECL HVAPI_SetAGCZone(HVAPI_HANDLE_EX hHandle, INT rgZone[16] )
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
	
	HRESULT hr = S_OK;
	INT nAGCZoneValue = 0;
	for ( INT i = 0 ; i < 16 ; ++i )
	{
		if ( 0 != rgZone[i] && 1 != rgZone[i] )
		{
			hr = S_FALSE;
			break;
		}
		nAGCZoneValue |= (rgZone[i] & 1) <<i;
	}
	
	if ( S_OK != hr )
	{
		return hr ;
	}


	//执行命令
	CHAR szCmd[256]= {0};
	sprintf( szCmd , "SetAGCZone,Count[%d],Value[%d]",16, nAGCZoneValue);

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetAGCZone HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}
	
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetAGCZone", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAGCZone HvXmlParse Fail\n");
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
		CHAR szCmd[256]= {0};
		sprintf( szCmd , "SetAGCZone,ZoneNum:%02d",16);
		char szTemp[128] = {0};
		for ( INT i = 0 ; i < 16 ; ++i )
		{
			sprintf( szTemp , ",AGCZone%02d[%d]",i , rgZone[i] );
			strcat( szCmd , szTemp);
		}

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;
	
		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAGCZone HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetAGCZone", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAGCZone HvXmlParse Fail\n");
			return E_FAIL;
		}
		nRetCode = atoi(cRetInfo.szKeyValue);
	}
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetAGCParam(HVAPI_HANDLE_EX hHandle,INT nShutterMin , INT nShutterMax ,INT nGainMin , INT nGainMax )
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
	sprintf( szCmd , "SetAGCParam,ShutterMin[%d],ShutterMax[%d],GainMin[%d],GainMax[%d]" ,nShutterMin , nShutterMax ,nGainMin ,  nGainMax );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetAGCParam HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}
	
	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetAGCParam", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAGCParam HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetAGCParam", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAGCParam HvXmlParse Fail\n");
			return E_FAIL;
		}


		nRetCode = atoi(cRetInfo.szKeyValue);
		
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}


HV_API_EX HRESULT CDECL HVAPI_SetLUT(HVAPI_HANDLE_EX hHandle, CHvPoint rgLUT[] , INT nLUTPointCount )
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
	
	const INT  MAX_LUT_POINT_COUNT = 8;
	if ( nLUTPointCount > MAX_LUT_POINT_COUNT ||  nLUTPointCount <= 0 )
	{
		return S_FALSE;
	}

	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		//执行命令
		const INT MAX_CMD_LEN = 1024;
		CHAR szCmd[MAX_CMD_LEN]= {0};
		sprintf( szCmd ,"SetLUT,PointCount[%d]",nLUTPointCount );
		CHAR szTemp[512];
		for ( int i = 0 ; i < nLUTPointCount ; i++ )
		{
			sprintf( szTemp , ",Point%02d_X[%d],Point%02d_Y[%d]" , i , rgLUT[i].nX ,i, rgLUT[i].nY);

			if ( strlen( szCmd ) + strlen(szTemp ) < MAX_CMD_LEN + 1 )
			{
				strcat( szCmd , szTemp );
			}

		}

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetLUT HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		//解析命令返回
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetLUT", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetLUT HvXmlParse Fail\n");
			return E_FAIL;
		}
		INT nRetCode = -1;
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

		return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		//执行命令
		const INT MAX_CMD_LEN = 1024;
		CHAR szCmd[MAX_CMD_LEN]= {0};
		sprintf( szCmd ,"SetGammaData,PointCount[%d]",nLUTPointCount );
		CHAR szTemp[512];
		for ( int i = 0 ; i < nLUTPointCount ; i++ )
		{
			sprintf( szTemp , ",Point%02d_X[%d],Point%02d_Y[%d]" , i , rgLUT[i].nX ,i, rgLUT[i].nY);

			if ( strlen( szCmd ) + strlen(szTemp ) < MAX_CMD_LEN + 1 )
			{
				strcat( szCmd , szTemp );
			}

		}

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetLUT HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		//解析命令返回
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetGammaData", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetLUT HvXmlParse Fail\n");
			return E_FAIL;
		}
		INT nRetCode = -1;
 
		nRetCode = atoi(cRetInfo.szKeyValue);
		 

		return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
	}
	return E_FAIL;
}


HV_API_EX HRESULT CDECL HVAPI_SetBrightness(HVAPI_HANDLE_EX hHandle, INT nBrightness )
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
	sprintf( szCmd , "SetBrightness,Brightness[%d]" ,nBrightness );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetBrightness HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetBrightness", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetBrightness HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetBrightness", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetBrightness HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}



HV_API_EX HRESULT CDECL HVAPI_GetBrightness(HVAPI_HANDLE_EX hHandle, INT* pnBrightness )
{
	return Default_Getter_Int_ForMercuryProtocol(hHandle, "GetBrightness", pnBrightness);	
}

HV_API_EX HRESULT CDECL HVAPI_SetContrast(HVAPI_HANDLE_EX hHandle, INT nContrast )
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
	sprintf( szCmd , "SetContrast,Contrast[%d]" ,nContrast );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetContrast HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetContrast", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetContrast HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetContrast", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetContrast HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetSharpness(HVAPI_HANDLE_EX hHandle, INT nSharpness )
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
	sprintf( szCmd , "SetSharpness,Sharpness[%d]" ,nSharpness );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetSharpness HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetSharpness", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetSharpness HvXmlParse Fail\n");
			return E_FAIL;
		}

 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetSharpness", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetSharpness HvXmlParse Fail\n");
			return E_FAIL;
		}

 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetSaturation(HVAPI_HANDLE_EX hHandle, INT nSaturation )
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
	sprintf( szCmd , "SetSaturation,Saturation[%d]" ,nSaturation );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetSaturation HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParse( "SetSaturation", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetSaturation HvXmlParse Fail\n");
			return E_FAIL;
		}

 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParseMercury( "SetSaturation", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetSaturation HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}

	
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetWDREnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable  )
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
	sprintf( szCmd , "SetWDREnable,Enable[%d]" ,fEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetWDREnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetWDREnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetWDREnable HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetWDREnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetWDREnable HvXmlParse Fail\n");
			return E_FAIL;
		}

 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}

	
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetWDRLevel(HVAPI_HANDLE_EX hHandle, INT nLevel )
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
	sprintf( szCmd , "SetWDRLevel,Level[%d]" ,nLevel );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetWDRLevel HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetWDRLevel", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetWDRLevel HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetWDRLevel", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetWDRLevel HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}

	

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetBLCEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable  )
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
	sprintf( szCmd , "SetBLCEnable,Enable[%d]" ,fEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetBLCEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetBLCEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetBLCEnable HvXmlParse Fail\n");
			return E_FAIL;
		}
	 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetBLCEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetBLCEnable HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}

	

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetBLCLevel(HVAPI_HANDLE_EX hHandle, INT nLevel )
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
	sprintf( szCmd , "SetBLCLevel,Level[%d]" ,nLevel);

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetBLCLevel HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetBLCLevel", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetBLCLevel HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetBLCLevel", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetBLCLevel HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}



	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetDREEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable )
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
	sprintf( szCmd , "SetDREEnable,Enable[%d]" ,fEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetDREnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetDREEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDREnable HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetDREEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDREnable HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}



	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetDRELevel(HVAPI_HANDLE_EX hHandle, INT nLevel )
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
	sprintf( szCmd , "SetDRELevel,Level[%d]" ,nLevel );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetDRELevel HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetDRELevel", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDRELevel HvXmlParse Fail\n");
			return E_FAIL;
		}

 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetDRELevel", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDRELevel HvXmlParse Fail\n");
			return E_FAIL;
		}

 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetDREMode(HVAPI_HANDLE_EX hHandle, INT nMode )
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
	sprintf( szCmd , "SetDREMode,Mode[%d]" ,nMode );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetDREMode HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{


		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetDREMode", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDREMode HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetDREMode", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDREMode HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}
HV_API_EX HRESULT CDECL HVAPI_SetDeNoiseSNFEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable )
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
	sprintf( szCmd , "SetDeNoiseSNFEnable,Enable[%d]" ,fEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetDeNoiseSNFEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetDeNoiseSNFEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDeNoiseSNFEnable HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetDeNoiseSNFEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDeNoiseSNFEnable HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}



HV_API_EX HRESULT CDECL HVAPI_SetDeNoiseTNFEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable )
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
	sprintf( szCmd , "SetDeNoiseTNFEnable,Enable[%d]" ,fEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetDeNoiseTNFEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetDeNoiseTNFEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDeNoiseTNFEnable HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetDeNoiseTNFEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDeNoiseTNFEnable HvXmlParse Fail\n");
			return E_FAIL;
		}

		nRetCode = atoi(cRetInfo.szKeyValue);

	}



	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetDeNoiseMode(HVAPI_HANDLE_EX hHandle, INT nMode )
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
	sprintf( szCmd , "SetDeNoiseMode,Mode[%d]" ,nMode);

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetDeNoiseMode HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetDeNoiseMode", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDeNoiseMode HvXmlParse Fail\n");
			return E_FAIL;
		}

	 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetDeNoiseMode", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDeNoiseMode HvXmlParse Fail\n");
			return E_FAIL;
		}

 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetDeNoiseLevel(HVAPI_HANDLE_EX hHandle,INT nLevel )
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
	sprintf( szCmd , "SetDeNoiseLevel,Level[%d]" , nLevel );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetDeNoiseLevel HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetDeNoiseLevel", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDeNoiseLevel HvXmlParse Fail\n");
			return E_FAIL;
		}

 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetDeNoiseLevel", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDeNoiseLevel HvXmlParse Fail\n");
			return E_FAIL;
		}

 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetFilterMode(HVAPI_HANDLE_EX hHandle, INT nMode )
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
		sprintf( szCmd , "SetFilterMode,Mode[%d]" ,nMode );

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetFilterMode HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetFilterMode", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetFilterMode HvXmlParse Fail\n");
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
		sprintf( szCmd , "SetCtrlCpl,Mode[%d]" ,nMode );

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetFilterMode HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetCtrlCpl", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetFilterMode HvXmlParse Fail\n");
			return E_FAIL;
		}

 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetDCIRIS(HVAPI_HANDLE_EX hHandle, BOOL fEnable )
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
		sprintf( szCmd , "SetDCIRIS,Enable[%d]" ,fEnable );

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDCIRIS HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetDCIRIS", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDCIRIS HvXmlParse Fail\n");
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
		sprintf( szCmd , "SetDCAperture,Enable[%d]" ,fEnable );

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDCIRIS HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetDCAperture", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetDCIRIS HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}



	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetIPInfo(HVAPI_HANDLE_EX hHandle, CHAR* szIP
							  ,CHAR* szMask 
							  , CHAR* szGateWay
							  , CHAR* szDNS )
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
	CHAR szCmd[256]={0};
	//to do ---- 对IP等信息进行有效性检测
	sprintf( szCmd , "SetIP,IP[%s],Mask[%s],Gateway[%s],DNS[%s]" ,
		szIP , szMask , szGateWay ,szDNS );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetIPInfo HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回

	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetIP", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetIPInfo HvXmlParse Fail\n");
			return E_FAIL;
		}
		 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetIP", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetIPInfo HvXmlParse Fail\n");
			return E_FAIL;
		}
		 
 
		nRetCode = atoi(cRetInfo.szKeyValue);
 
	}
	
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}



HV_API_EX HRESULT CDECL HVAPI_SetFTPServerIP(HVAPI_HANDLE_EX hHandle, CHAR* szIP )
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
	sprintf( szCmd , "SetFTPServerIP,ServerIP[%s]" ,szIP);

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetFTPServerIP HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParse( "SetFTPServerIP", szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetFTPServerIP HvXmlParse Fail\n");
		return E_FAIL;
	}

	INT nRetCode = -1;
	if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
	{
		nRetCode = atoi(cRetInfo.szKeyValue);
	}
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetFTPPort(HVAPI_HANDLE_EX hHandle, INT nPort )
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
	sprintf( szCmd , "SetFTPPort,Port[%d]" , nPort );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetFTPPort HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParse( "SetFTPPort", szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetFTPPort HvXmlParse Fail\n");
		return E_FAIL;
	}
	INT nRetCode = -1;
	if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
	{
		nRetCode = atoi(cRetInfo.szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetFTPUserInfo(HVAPI_HANDLE_EX hHandle, CHAR* szUserName , CHAR* szPassword )
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
	sprintf( szCmd , "SetFTPUserInfo,UserName[%s],Password[%s]" ,szUserName , szPassword );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetFTPUserInfo HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParse( "SetFTPUserInfo", szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetFTPUserInfo HvXmlParse Fail\n");
		return E_FAIL;
	}
	INT nRetCode = -1;
	if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
	{
		nRetCode = atoi(cRetInfo.szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}


HV_API_EX HRESULT CDECL HVAPI_SetFTPUpLoadPath(HVAPI_HANDLE_EX hHandle, CHAR* szPath  )
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
	sprintf( szCmd , "SetFTPUpLoadPath,FileUploadPath[%s]" ,szPath );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetFTPUpLoadPath HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParse( "SetFTPUpLoadPath", szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetFTPUpLoadPath HvXmlParse Fail\n");
		return E_FAIL;
	}
	INT nRetCode = -1;
	if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
	{
		nRetCode = atoi(cRetInfo.szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetRTSPMulticastEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable )
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
	sprintf( szCmd , "SetRTSPMulticastEnable,Enable[%d]" ,fEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetRTSPMulticastEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParse( "SetRTSPMulticastEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetRTSPMulticastEnable HvXmlParse Fail\n");
		return E_FAIL;
	}
	INT nRetCode = -1;
	if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
	{
		nRetCode = atoi(cRetInfo.szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetHttpPort(HVAPI_HANDLE_EX hHandle, INT nPort )
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
	sprintf( szCmd , "SetHttpPort,HttpPort[%d]" ,nPort );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetHttpPort HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParse( "SetHttpPort", szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetHttpPort HvXmlParse Fail\n");
		return E_FAIL;
	}
	INT nRetCode = -1;
	if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
	{
		nRetCode = atoi(cRetInfo.szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetHttpsPort(HVAPI_HANDLE_EX hHandle, INT nPort )
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
	sprintf( szCmd , "SetHttpsPort,HttpSPort[N]" ,nPort );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetHttpsPort HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParse( "SetHttpsPort", szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetHttpsPort HvXmlParse Fail\n");
		return E_FAIL;
	}
	INT nRetCode = -1;
	if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
	{
		nRetCode = atoi(cRetInfo.szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetSMTPAuthenticationEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable )
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
	sprintf( szCmd , "SetSMTPAuthenticationEnable,AuthenticationEnable[%d]" , fEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetSMTPAuthenticationEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParse( "SetSMTPAuthenticationEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetSMTPAuthenticationEnable HvXmlParse Fail\n");
		return E_FAIL;
	}
	INT nRetCode = -1;
	if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
	{
		nRetCode = atoi(cRetInfo.szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetSMTPUserInfo(HVAPI_HANDLE_EX hHandle, CHAR* szUserName , CHAR* szPassword )
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
	sprintf( szCmd , "SetSMTPUserInfo,UserName[%s],Password[%s]" ,szUserName , szPassword );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetSMTPUserInfo HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParse( "SetSMTPUserInfo", szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetSMTPUserInfo HvXmlParse Fail\n");
		return E_FAIL;
	}
	INT nRetCode = -1;
	if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
	{
		nRetCode = atoi(cRetInfo.szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}


HV_API_EX HRESULT CDECL HVAPI_SetSMTPSender(HVAPI_HANDLE_EX hHandle, CHAR* szSenderMail )
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
	sprintf( szCmd , "SetSMTPSender,Sender[%s]" ,szSenderMail );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetSMTPSender HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParse( "SetSMTPSender", szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetSMTPSender HvXmlParse Fail\n");
		return E_FAIL;
	}
	INT nRetCode = -1;
	if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
	{
		nRetCode = atoi(cRetInfo.szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetSMTPServerIP(HVAPI_HANDLE_EX hHandle, CHAR* szIP  )
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
	sprintf( szCmd , "SetSMTPServerIP,ServerIP[%s]" ,szIP );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetSMTPServerIP HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParse( "SetSMTPServerIP", szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetSMTPServerIP HvXmlParse Fail\n");
		return E_FAIL;
	}
	INT nRetCode = -1;
	if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
	{
		nRetCode = atoi(cRetInfo.szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetSMTPPort(HVAPI_HANDLE_EX hHandle, INT nPort )
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
	sprintf( szCmd , "SetSMTPPort,Port[%d]" ,nPort );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetSMTPPort HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParse( "SetSMTPPort", szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetSMTPPort HvXmlParse Fail\n");
		return E_FAIL;
	}
	INT nRetCode = -1;
	if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
	{
		nRetCode = atoi(cRetInfo.szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetSMTPEmail(HVAPI_HANDLE_EX hHandle, CHAR* szEmail ,INT nEmailLen )
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
	sprintf( szCmd , "SetSMTPEmail,Email[%s]" ,szEmail );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetSMTPEmail HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	CXmlParseInfo cRetInfo;
	strcpy( cRetInfo.szKeyName, "RetCode" );

	if ( S_OK != HvXmlParse( "SetSMTPEmail", szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetSMTPEmail HvXmlParse Fail\n");
		return E_FAIL;
	}
	INT nRetCode = -1;
	if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
	{
		nRetCode = atoi(cRetInfo.szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetTime(HVAPI_HANDLE_EX hHandle, INT nYear , INT nMon , INT nDay , INT nHour ,INT nMin , INT nSec , INT nMSec )
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
	CHAR szCmd[256]={0};
	
	CHAR szDate[128] = {0};
	CHAR szTime[128] ={0};
	sprintf( szDate , "%04d.%02d.%02d" ,nYear , nMon,nDay );
	sprintf( szTime , "%02d:%02d:%02d %03d" , nHour , nMin , nSec ,nMSec);
	sprintf( szCmd , "SetTime,Date[%s],Time[%s]" ,szDate , szTime );
	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetTime HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetTime", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetTime HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetTime", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetTime HvXmlParse Fail\n");
			return E_FAIL;
		}
 
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}

	

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetTimeZone(HVAPI_HANDLE_EX hHandle, INT nZoneNum )
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
	sprintf( szCmd , "SetTimeZone,TimeZone[%d]" ,nZoneNum );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetTimeZone HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetTimeZone", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetTimeZone HvXmlParse Fail\n");
			return E_FAIL;
		}

		
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetTimeZone", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetTimeZone HvXmlParse Fail\n");
			return E_FAIL;
		}
	 
 
		nRetCode = atoi(cRetInfo.szKeyValue);
	 
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetNTPEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable )
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
	sprintf( szCmd , "SetNTPEnable,Enable[%d]" ,fEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetNTPEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetNTPEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetNTPEnable HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetNTPEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetNTPEnable HvXmlParse Fail\n");
			return E_FAIL;
		}
 
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}



	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetNTPServerIP(HVAPI_HANDLE_EX hHandle, CHAR* szIP )
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
	sprintf( szCmd , "SetNTPServerIP,ServerIP[%s]" ,szIP );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetNTPServerIP HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetNTPServerIP", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetNTPServerIP HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetNTPServerIP", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetNTPServerIP HvXmlParse Fail\n");
			return E_FAIL;
		}
	
		nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

 

HV_API_EX HRESULT CDECL HVAPI_SetNTPServerUpdateInterval(HVAPI_HANDLE_EX hHandle,DWORD32 dw32UpdateIntervalMS )
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
	sprintf( szCmd , "SetNTPServerUpdateInterval,UpdateInterval[%d]" ,dw32UpdateIntervalMS );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetNTPServerUpdateInterval HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetNTPServerUpdateInterval", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetNTPServerUpdateInterval HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetNTPServerUpdateInterval", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetNTPServerUpdateInterval HvXmlParse Fail\n");
			return E_FAIL;
		}
 

		nRetCode = atoi(cRetInfo.szKeyValue);

	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}


HV_API_EX HRESULT CDECL HVAPI_SetSensorWDREnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable )
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
	sprintf( szCmd , "SetSensorWDR,Enable[%d]" ,fEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetSensorWDREnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetSensorWDR", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetSensorWDREnable HvXmlParse Fail\n");
			return E_FAIL;
		}
	 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetSensorWDR", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetSensorWDREnable HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}
	

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetOCGate(HVAPI_HANDLE_EX hHandle, BOOL fEnable )
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
		sprintf( szCmd , "SetOCGate,Enable[%d]" ,fEnable );

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetOCGate HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetOCGate", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetOCGate HvXmlParse Fail\n");
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
		sprintf( szCmd , "SendTriggerOut,Enable[%d]" ,fEnable );

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetOCGate HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SendTriggerOut", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetOCGate HvXmlParse Fail\n");
			return E_FAIL;
		}
		 
 
			nRetCode = atoi(cRetInfo.szKeyValue);
	}
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}


/*
* @brief		设置色温
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	nValue		 范围：
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetColor(HVAPI_HANDLE_EX hHandle, INT nValue )
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
	sprintf( szCmd , "SetColor,Value[%d]" ,nValue );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetColor HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetColor", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetColor HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		
	}
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

/*
* @brief		设置2D降噪开关
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	fEnable		 范围：0：关闭，1：打开
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_Set2DDeNoiseEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable )
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
	sprintf( szCmd , "Set2DDenoiseEnable,Enable[%d]" ,fEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_Set2DDeNoiseEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "Set2DDenoiseEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_Set2DDeNoiseEnable HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{

	}
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

/*
* @brief		设置2D降噪强度
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	nValue		 范围：0～255
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_Set2DDeNoiseStrength(HVAPI_HANDLE_EX hHandle, INT nValue )
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
	sprintf( szCmd , "Set2DDenoiseValue,Value[%d]" ,nValue );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_Set2DDeNoiseStrength HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "Set2DDenoiseValue", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_Set2DDeNoiseStrength HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{

	}
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

/*
* @brief		设置GAMMA开关
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	fEnable		 范围：0： 关闭， 1：打开
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetGammaEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable )
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
	sprintf( szCmd , "SetGammaEnable,Enable[%d]" ,fEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetGammaEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetGammaEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetGammaEnable HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetGammaEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetGammaEnable HvXmlParse Fail\n");
			return E_FAIL;
		}

 
		nRetCode = atoi(cRetInfo.szKeyValue);
	 
	}
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

/*
* @brief		设置GAMMA强度
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	fValue		 范围：1～5
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetGammaStrength(HVAPI_HANDLE_EX hHandle, INT nValue )
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
	sprintf( szCmd , "SetGammaValue,Value[%d]" ,nValue );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetGammaStrength HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetGammaValue", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetGammaStrength HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetGammaValue", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetGammaStrength HvXmlParse Fail\n");
			return E_FAIL;
		}

 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}


HV_API_EX HRESULT CDECL HVAPI_ResetDevice(HVAPI_HANDLE_EX hHandle  , INT nRetsetMode )
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
	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		if ( S_OK != HVAPI_ExecCmdEx(hHandle , "ResetDevice" , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_ResetDevice HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		//解析命令返回


		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "ResetDevice", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_ResetDevice HvXmlParse Fail\n");
			return E_FAIL;
		}

	 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CHAR szCmd[128];
		sprintf(szCmd, "ResetDevice,ResetMode[%d]",nRetsetMode);
		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_ResetDevice HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "ResetDevice", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_ResetDevice HvXmlParse Fail\n");
			return E_FAIL;
		}

		nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}

	
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_RestoreDefaultParam(HVAPI_HANDLE_EX hHandle)
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
	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , "RestoreDefaultParam" , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_RestoreDefaultParam HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "RestoreDefaultParam" , szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_RestoreDefaultParam HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "RestoreDefaultParam" , szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_RestoreDefaultParam HvXmlParse Fail\n");
			return E_FAIL;
		}
		 
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}



	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_RestoreFactoryParam(HVAPI_HANDLE_EX hHandle)
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
	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , "RestoreFactoryParam" , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_RestoreFactoryParam HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "RestoreFactoryParam", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_RestoreFactoryParam HvXmlParse Fail\n");
			return E_FAIL;
		}

		 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "RestoreFactoryParam", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_RestoreFactoryParam HvXmlParse Fail\n");
			return E_FAIL;
		}

		 
 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}


HV_API_EX HRESULT CDECL HVAPI_GetDevBasicInfo(HVAPI_HANDLE_EX hHandle, CDevBasicInfo* pBasicInfo  )
{
	if( hHandle == NULL || NULL == pBasicInfo )
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
	CHAR szRet[5*1024]={0};
	INT nRetLen = 5*1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , "GetDevBasicInfo" , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_GetDevBasicInfo HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	CXmlParseInfo rgRetInfo[17];
	strcpy( rgRetInfo[0].szKeyName, "RetCode" );
	strcpy( rgRetInfo[1].szKeyName, "Mode" );
	strcpy( rgRetInfo[2].szKeyName, "SN" );
	strcpy( rgRetInfo[3].szKeyName, "DevType" );
	strcpy( rgRetInfo[4].szKeyName, "WorkMode" );
	strcpy( rgRetInfo[5].szKeyName, "DevVersion" );
	strcpy( rgRetInfo[6].szKeyName, "IP" );
	strcpy( rgRetInfo[7].szKeyName, "Mac" );
	strcpy( rgRetInfo[8].szKeyName, "Mask" );
	strcpy( rgRetInfo[9].szKeyName, "Gateway" );
	strcpy( rgRetInfo[10].szKeyName, "BackupVersion" );
	strcpy( rgRetInfo[11].szKeyName, "FPGAVersion" );
	strcpy( rgRetInfo[12].szKeyName, "KernelVersion" );
	strcpy( rgRetInfo[13].szKeyName, "UbootVersion" );
	strcpy( rgRetInfo[14].szKeyName, "UBLVersion" );
	strcpy( rgRetInfo[15].szKeyName, "FirmwareVersion" );
	strcpy( rgRetInfo[16].szKeyName, "NetPackageVersion" );
	
	INT nRetCode = -1;
	if( pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH )
	{
		if ( S_OK != HvXmlParse( "GetDevBasicInfo", szRet ,nRetLen , rgRetInfo, 11 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetDevBasicInfo HvXmlParse Fail\n");
			return E_FAIL;
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[0].eKeyType )
		{
			nRetCode = atoi(rgRetInfo[0].szKeyValue);
		}
		if ( sizeof(pBasicInfo->szModelVersion) >= strlen(rgRetInfo[1].szKeyValue) )
		{
			strcpy( pBasicInfo->szModelVersion ,rgRetInfo[1].szKeyValue );
		}
		if ( sizeof(pBasicInfo->szSN) >= strlen(rgRetInfo[2].szKeyValue) )
		{
			strcpy( pBasicInfo->szSN ,rgRetInfo[2].szKeyValue );
		}
		if ( sizeof(pBasicInfo->szDevType) >= strlen(rgRetInfo[3].szKeyValue) )
		{
			strcpy( pBasicInfo->szDevType ,rgRetInfo[3].szKeyValue );
		}
		if ( sizeof(pBasicInfo->szWorkMode) >= strlen(rgRetInfo[4].szKeyValue) )
		{
			strcpy( pBasicInfo->szWorkMode ,rgRetInfo[4].szKeyValue );
		}
		if ( sizeof(pBasicInfo->szDevVersion) >= strlen(rgRetInfo[5].szKeyValue) )
		{
			strcpy( pBasicInfo->szDevVersion ,rgRetInfo[5].szKeyValue );
		}
		if ( sizeof(pBasicInfo->szIP) >= strlen(rgRetInfo[6].szKeyValue) )
		{
			strcpy( pBasicInfo->szIP ,rgRetInfo[6].szKeyValue );
		}
		if ( sizeof(pBasicInfo->szMac) >= strlen(rgRetInfo[7].szKeyValue) )
		{
			strcpy( pBasicInfo->szMac ,rgRetInfo[7].szKeyValue );
		}
		if ( sizeof(pBasicInfo->szMask) >= strlen(rgRetInfo[8].szKeyValue) )
		{
			strcpy( pBasicInfo->szMask ,rgRetInfo[8].szKeyValue );
		}
		if ( sizeof(pBasicInfo->szGateway) >= strlen(rgRetInfo[9].szKeyValue) )
		{
			strcpy( pBasicInfo->szGateway ,rgRetInfo[9].szKeyValue );
		}
		if ( sizeof(pBasicInfo->szMode) >= strlen(rgRetInfo[10].szKeyValue) )
		{
			strcpy( pBasicInfo->szMode ,rgRetInfo[10].szKeyValue );
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		if ( S_OK != HvXmlParseMercury( "GetDevBasicInfo", szRet ,nRetLen , rgRetInfo, 17 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetDevBasicInfo HvXmlParse Fail\n");
			return E_FAIL;
		}

		nRetCode = atoi(rgRetInfo[0].szKeyValue);
		strcpy( pBasicInfo->szMode,rgRetInfo[1].szKeyValue );
		strcpy( pBasicInfo->szSN ,rgRetInfo[2].szKeyValue );
		strcpy( pBasicInfo->szDevType ,rgRetInfo[3].szKeyValue );
		strcpy( pBasicInfo->szWorkMode ,rgRetInfo[4].szKeyValue );
		strcpy( pBasicInfo->szDevVersion ,rgRetInfo[5].szKeyValue );
		strcpy( pBasicInfo->szIP ,rgRetInfo[6].szKeyValue );
		strcpy( pBasicInfo->szMac ,rgRetInfo[7].szKeyValue );
		strcpy( pBasicInfo->szMask ,rgRetInfo[8].szKeyValue );
		strcpy( pBasicInfo->szGateway ,rgRetInfo[9].szKeyValue );
		strcpy( pBasicInfo->szBackupVersion ,rgRetInfo[10].szKeyValue );
		strcpy( pBasicInfo->szFPGAVersion ,rgRetInfo[11].szKeyValue );
		strcpy( pBasicInfo->szKernelVersion ,rgRetInfo[12].szKeyValue );
		strcpy( pBasicInfo->szUbootVersion ,rgRetInfo[13].szKeyValue );
		strcpy( pBasicInfo->szUBLVersion ,rgRetInfo[14].szKeyValue );
		strcpy( pBasicInfo->szFirmwareVersion ,rgRetInfo[15].szKeyValue );
		strcpy( pBasicInfo->szNetPackageVersion ,rgRetInfo[16].szKeyValue );
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}


HV_API_EX HRESULT CDECL HVAPI_GetRunMode(HVAPI_HANDLE_EX hHandle, INT* pnRunMode )
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
	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	INT nRetCode = -1;
	if ( pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH )
	{
		if ( S_OK != HVAPI_ExecCmdEx(hHandle , "GetRunMode" , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetRunMode HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		//解析命令返回
		CXmlParseInfo rgRetInfo[2];
		strcpy( rgRetInfo[0].szKeyName, "RetCode" );
		strcpy( rgRetInfo[1].szKeyName, "RunMode" );

		if ( S_OK != HvXmlParse( "GetRunMode", szRet ,nRetLen , rgRetInfo, 2 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetRunMode HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == rgRetInfo[0].eKeyType )
		{
			nRetCode = atoi(rgRetInfo[0].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[1].eKeyType && NULL != pnRunMode )
		{
			*pnRunMode = atoi(rgRetInfo[1].szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		const int iBufLen = (1024 << 4);
		char* pszXmlBuf = new char[iBufLen];
		int iXmlBufLen = iBufLen;

		if(!pszXmlBuf)
		{
			return E_FAIL;
		}
		memset(pszXmlBuf, 0, iBufLen);
		DWORD dwXmlVersion = 0;


		if(HvMakeXmlInfoByString( pHHC->emProtocolVersion , "OptResetMode", sizeof("OptResetMode"), pszXmlBuf, iXmlBufLen) == E_FAIL)
		{
			return E_FAIL;
		}
		char* pszRetBuf = new char[1024];
		if(!pszRetBuf)
		{
			SAFE_DELETE_ARG(pszXmlBuf);
			return E_FAIL;
		}
		memset(pszRetBuf, 0, 1024);
		int iRetBufLen = 1024;
		SOCKET sktCmd = INVALID_SOCKET;


			if(ExecXmlExtCmdMercury((char*)pHHC->szIP, (char*)pszXmlBuf, (char*)pszRetBuf, iRetBufLen, sktCmd) == false)
			{
				SAFE_DELETE_ARG(pszRetBuf);
				SAFE_DELETE_ARG(pszXmlBuf);
				return E_FAIL;
			}
 

		CXmlParseInfo rgRetInfo [2];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );
		strcpy( rgRetInfo [1].szKeyName, "ResetMode" );

		if ( S_OK != HvXmlParseMercury( "OptResetMode", pszRetBuf ,strlen(pszRetBuf) ,rgRetInfo , 2 ) )
		{
			WrightLogEx("OptResetMode" , "OptResetMode HvXmlParse Fail\n");
			return E_FAIL;
		}
		nRetCode = atoi (rgRetInfo[0]. szKeyValue);
		*pnRunMode = atoi (rgRetInfo[1]. szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_GetDevState(HVAPI_HANDLE_EX hHandle, CDevState* pState  )
{
	if(hHandle == NULL || NULL == pState )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)	
	{
		return E_FAIL;
	}
	CHAR szRet[5*1024]={0};
	INT nRetLen = 5*1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , "GetDevState" , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_GetDevState HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	CXmlParseInfo rgRetInfo[17];
	strcpy( rgRetInfo[0].szKeyName, "RetCode" );
	strcpy( rgRetInfo[1].szKeyName, "CpuTemperature" );//
	strcpy( rgRetInfo[2].szKeyName, "Date" );//
	strcpy( rgRetInfo[3].szKeyName, "Time" );//
	strcpy( rgRetInfo[4].szKeyName, "CpuUsage" );//
	strcpy( rgRetInfo[5].szKeyName, "MemUsage" );//
	strcpy( rgRetInfo[6].szKeyName, "HddOpStatus" );//
	strcpy( rgRetInfo[7].szKeyName, "RecordLinkIP" );//
	strcpy( rgRetInfo[8].szKeyName, "ImageLinkIP" );//
	strcpy( rgRetInfo[9].szKeyName, "VideoLinkIP" );//
	strcpy( rgRetInfo[10].szKeyName,"OCGateEnable" );
	strcpy( rgRetInfo[11].szKeyName ,"RTSPMulticastEnable" );
	strcpy( rgRetInfo[12].szKeyName ,"TimeZone" );//
	strcpy( rgRetInfo[13].szKeyName ,"NTPEnable" );//
	strcpy( rgRetInfo[14].szKeyName ,"NTPServerIP" );//
	strcpy( rgRetInfo[15].szKeyName ,"NTPServerUpdateInterval" );//
	strcpy( rgRetInfo[16].szKeyName ,"TraceRank" );//


	INT nRetCode = -1;
	if ( pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH )
	{
		if ( S_OK != HvXmlParse( "GetDevState", szRet ,nRetLen , rgRetInfo, 15 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetDevState HvXmlParse Fail\n");
			return E_FAIL;
		}
		
		if ( XML_CMD_TYPE_INT == rgRetInfo[0].eKeyType )
		{
			nRetCode = atoi(rgRetInfo[0].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[1].eKeyType )
		{
			pState->nTemperature = atoi(rgRetInfo[1].szKeyValue);
		}
		if ( XML_CMD_TYPE_STRING == rgRetInfo[2].eKeyType && 
			XML_CMD_TYPE_STRING == rgRetInfo[3].eKeyType )
		{
			sscanf( rgRetInfo[2].szKeyValue , "%04d.%02d.%02d" ,
				&(pState->nYear), &(pState->nMon) , &(pState->nDay) );
			sscanf( rgRetInfo[3].szKeyValue , "%02d:%02d:%02d %03d" ,
				&(pState->nHour) , &(pState->nMin) , &(pState->nSec) ,&(pState->nMSec) );
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[4].eKeyType )
		{
			pState->nCpuUsage = atoi(rgRetInfo[4].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[5].eKeyType )
		{
			pState->nMemUsage = atoi(rgRetInfo[5].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[6].eKeyType )
		{
			pState->nHddState = atoi(rgRetInfo[6].szKeyValue);
		}
		if ( XML_CMD_TYPE_STRING == rgRetInfo[7].eKeyType && 
			sizeof(pState->szRecordLinkIP) > strlen(rgRetInfo[7].szKeyValue) )
		{
			strcpy( pState->szRecordLinkIP ,rgRetInfo[7].szKeyValue);
		}
		if ( XML_CMD_TYPE_STRING == rgRetInfo[8].eKeyType && 
			sizeof(pState->szImageLinkIP) > strlen(rgRetInfo[8].szKeyValue) )
		{
			strcpy( pState->szImageLinkIP ,rgRetInfo[8].szKeyValue);
		}
		if ( XML_CMD_TYPE_STRING == rgRetInfo[9].eKeyType && 
			sizeof(pState->szVideoLinkIP) > strlen(rgRetInfo[9].szKeyValue) )
		{
			strcpy( pState->szVideoLinkIP ,rgRetInfo[9].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[10].eKeyType )
		{
			pState->fOCGateEnable = atoi(rgRetInfo[10].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[11].eKeyType )
		{
			pState->fRTSPMulticastEnable = atoi(rgRetInfo[11].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[12].eKeyType )
		{
			pState->nTimeZone = atoi(rgRetInfo[12].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[13].eKeyType )
		{
			pState->fNTPEnable = atoi(rgRetInfo[13].szKeyValue);
		}
		if ( XML_CMD_TYPE_STRING == rgRetInfo[14].eKeyType )
		{
			strcpy( pState->szNTPServerIP ,rgRetInfo[14].szKeyValue  );
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		if ( S_OK != HvXmlParseMercury( "GetDevState", szRet ,nRetLen , rgRetInfo, 17 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetDevState HvXmlParse Fail\n");
			return E_FAIL;
		}
		nRetCode = atoi(rgRetInfo[0].szKeyValue);
		pState->nTemperature = atoi(rgRetInfo[1].szKeyValue);
		sscanf( rgRetInfo[2].szKeyValue , "%04d.%02d.%02d" ,
			&(pState->nYear), &(pState->nMon) , &(pState->nDay) );
		sscanf( rgRetInfo[3].szKeyValue , "%02d:%02d:%02d %03d" ,
			&(pState->nHour) , &(pState->nMin) , &(pState->nSec) ,&(pState->nMSec) );
		pState->nCpuUsage = atoi(rgRetInfo[4].szKeyValue);
		pState->nMemUsage = atoi(rgRetInfo[5].szKeyValue);
		if (strcmp(rgRetInfo[6].szKeyValue, "正常") == 0)
		{
			pState->nHddState = 0;
		}
		else
		{
			pState->nHddState = 1;
		}
		


		INT nConnectIPLen=512;
		ParseConnectIP(rgRetInfo[7].szKeyValue, "结果连接", pState->szRecordLinkIP, &nConnectIPLen );
		nConnectIPLen=512;
		ParseConnectIP(rgRetInfo[8].szKeyValue, "图片连接", pState->szImageLinkIP, &nConnectIPLen);
		nConnectIPLen=512;
		ParseConnectIP(rgRetInfo[9].szKeyValue, "视频连接", pState->szVideoLinkIP, &nConnectIPLen);

		//strcpy( pState->szRecordLinkIP,rgRetInfo[7].szKeyValue); //"RecordLinkIP"  
		//strcpy( pState->szImageLinkIP, rgRetInfo[8].szKeyValue); //"ImageLinkIP"  
		//strcpy( pState->szVideoLinkIP,rgRetInfo[9].szKeyValue); //"VideoLinkIP"  
		pState->nTimeZone = atoi( rgRetInfo[12].szKeyValue); //"TimeZone"  
		pState->fNTPEnable = atoi( rgRetInfo[13].szKeyValue); //"NTPEnable"  
		strcpy( pState->szNTPServerIP, rgRetInfo[14].szKeyValue); //"NTPServerIP"  
		pState->nNTPServerUpdateInterval = atoi( rgRetInfo[15].szKeyValue); //"NTPServerUpdateInterval"  
		pState->nTraceRank = atoi( rgRetInfo[16].szKeyValue); //"TraceRank"  
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_GetResetReport( HVAPI_HANDLE_EX hHandle,CHAR* pReportBuf, INT* pnBufLen  )
{

	if(hHandle == NULL || NULL == pReportBuf || NULL == pnBufLen)
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	if ( pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		if ( S_OK != ExecuteCmd( pHHC->szIP ,CAMERA_GET_RESET_REPORT_CMD , NULL ,0 ,pReportBuf , pnBufLen ) )
		{
			WrightLogEx(pHHC->szIP , "ExecuteCmd CAMERA_TRIGGER_IMAGE Fail\n" );
			return E_FAIL;
		}

		return S_OK;
	}
	else if( pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CHAR szRet [17*1024]={0};
		INT nRetLen = 17*1024;

		if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetResetReport" , szRet , nRetLen , &nRetLen ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetResetReport  HVAPI_ExecCmdEx Fail\n");
			return E_FAIL ;
		}

		//解析命令返回


		INT nRetCode = -1;

		CXmlParseInfo rgRetInfo [2];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "GetResetReport", szRet ,nRetLen , rgRetInfo, 1 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetResetReport  HvXmlParse Fail\n");
			return E_FAIL ;
		}	

		nRetCode = atoi (rgRetInfo[0]. szKeyValue);

		if ( S_OK != HvParseXmlCmdRespRetcode2Adv(szRet,"GetResetReport", "ResetReport", pReportBuf, pnBufLen) )
		{
			nRetCode =-1;
		}
		
		return nRetCode ==0 ?S_OK: E_FAIL;
	}
return S_OK;
	
}

HV_API_EX HRESULT CDECL HVAPI_GetResetCount(HVAPI_HANDLE_EX hHandle, INT* pnResetCount )
{
	if(hHandle == NULL || NULL == pnResetCount )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , "GetResetCount" , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetResetCount HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		//解析命令返回
		INT nRetCode = -1;
		CXmlParseInfo rgRetInfo[2];
		strcpy( rgRetInfo[0].szKeyName, "RetCode" );
		strcpy( rgRetInfo[1].szKeyName, "ResetCount" );
		if ( S_OK != HvXmlParse( "GetResetCount", szRet ,nRetLen , rgRetInfo, 2 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetResetCount HvXmlParse Fail\n");
			return E_FAIL;
		}

		
		if ( XML_CMD_TYPE_INT == rgRetInfo[0].eKeyType )
		{
			nRetCode = atoi(rgRetInfo[0].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[1].eKeyType )
		{
			*pnResetCount = atoi(rgRetInfo[1].szKeyValue);
		}
		return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , "OptResetCount" , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetResetCount HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		//解析命令返回
		INT nRetCode = -1;
		CXmlParseInfo rgRetInfo [2];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );
		strcpy( rgRetInfo [1].szKeyName, "ResetCount" );

		if ( S_OK != HvXmlParseMercury( "OptResetCount", szRet ,nRetLen ,rgRetInfo , 2 ) )
		{
			WrightLogEx("GetResetCount" , "GetResetCount HvXmlParse Fail\n");
			return E_FAIL;
		}
		

		nRetCode = atoi (rgRetInfo[0]. szKeyValue);
		*pnResetCount = atoi (rgRetInfo[1]. szKeyValue);
		return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
	}



	return E_FAIL;
}

HV_API_EX HRESULT CDECL HVAPI_GetLog(HVAPI_HANDLE_EX hHandle, CHAR* szLog , INT* pnLogLen )
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

	if ( S_OK != ExecuteCmd( pHHC->szIP ,CAMERA_GET_LOG , NULL ,0 ,szLog , pnLogLen ) )
	{
		WrightLogEx( pHHC->szIP , "ExecuteCmd CAMERA_GET_LOG Fail\n" );
		return E_FAIL;
	}

	return S_OK;

}

HV_API_EX HRESULT CDECL HVAPI_GetCameraState(HVAPI_HANDLE_EX hHandle, CCameraState* pCameraState )
{
	if(hHandle == NULL || NULL == pCameraState )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}
	CHAR szRet[5*1024]={0};
	INT nRetLen = 5*1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , "GetCameraState" , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetCameraState HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		//解析命令返回
		CXmlParseInfo rgRetInfo[11];
		strcpy( rgRetInfo[0].szKeyName, "RetCode" );
		strcpy( rgRetInfo[1].szKeyName, "AGCEnable" );
		strcpy( rgRetInfo[2].szKeyName, "Shutter" );
		strcpy( rgRetInfo[3].szKeyName, "Gain" );
		strcpy( rgRetInfo[4].szKeyName, "AWBEnable" );
		strcpy( rgRetInfo[5].szKeyName, "GainR" );
		strcpy( rgRetInfo[6].szKeyName, "GainG" );
		strcpy( rgRetInfo[7].szKeyName, "GainB" );

		
		INT nRetCode = -1;
		if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
		{
			if ( S_OK != HvXmlParse( "GetCameraState", szRet ,nRetLen , rgRetInfo, 8 ) )
			{
				WrightLogEx(pHHC->szIP , "HVAPI_GetCameraState HvXmlParse Fail\n");
				return E_FAIL;
			}
			if ( XML_CMD_TYPE_INT == rgRetInfo[0].eKeyType )
			{
				nRetCode = atoi(rgRetInfo[0].szKeyValue);
			}
			if ( XML_CMD_TYPE_INT == rgRetInfo[1].eKeyType )
			{
				pCameraState->fAGCEnable = atoi(rgRetInfo[1].szKeyValue);
			}
			if ( XML_CMD_TYPE_INT == rgRetInfo[2].eKeyType )
			{
				pCameraState->nShutter = atoi(rgRetInfo[2].szKeyValue);
			}
			if ( XML_CMD_TYPE_INT == rgRetInfo[3].eKeyType )
			{
				pCameraState->nGain = atoi(rgRetInfo[3].szKeyValue);
			}
			if ( XML_CMD_TYPE_INT == rgRetInfo[4].eKeyType )
			{
				pCameraState->fAWBEnable = atoi(rgRetInfo[4].szKeyValue);
			}
			if ( XML_CMD_TYPE_INT == rgRetInfo[5].eKeyType )
			{
				pCameraState->nGainR = atoi(rgRetInfo[5].szKeyValue);
			}
			if ( XML_CMD_TYPE_INT == rgRetInfo[6].eKeyType )
			{
				pCameraState->nGainG = atoi(rgRetInfo[6].szKeyValue);
			}
			if ( XML_CMD_TYPE_INT == rgRetInfo[7].eKeyType )
			{
				pCameraState->nGainB = atoi(rgRetInfo[7].szKeyValue);
			}
		}
		else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
		{
			if ( S_OK != HvXmlParseMercury( "GetCameraState", szRet ,nRetLen , rgRetInfo, 8 ) )
			{
				WrightLogEx(pHHC->szIP , "HVAPI_GetCameraState HvXmlParse Fail\n");
				return E_FAIL;
			}
			nRetCode = atoi(rgRetInfo[0].szKeyValue);
			pCameraState->fAGCEnable = atoi(rgRetInfo[1].szKeyValue);
			pCameraState->nShutter = atoi(rgRetInfo[2].szKeyValue);
			pCameraState->nGain = atoi(rgRetInfo[3].szKeyValue);
			pCameraState->fAWBEnable = atoi(rgRetInfo[4].szKeyValue);
			pCameraState->nGainR = atoi(rgRetInfo[5].szKeyValue);
			pCameraState->nGainG = atoi(rgRetInfo[6].szKeyValue);
			pCameraState->nGainB = atoi(rgRetInfo[7].szKeyValue);

		}

		return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}


HV_API_EX HRESULT CDECL HVAPI_GetCameraBasicInfo(HVAPI_HANDLE_EX hHandle, CCameraBasicInfo* pCameraBasicInfo )
{
	if(hHandle == NULL || NULL == pCameraBasicInfo )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	CHAR szRet[5*1024]={0};
	INT nRetLen = 5*1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , "GetCameraBasicInfo" , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_GetCameraBasicInfo HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回--- 命令太多
	CXmlParseInfo rgRetInfo[40];
	strcpy( rgRetInfo[0].szKeyName, "RetCode" );
	strcpy( rgRetInfo[1].szKeyName, "Brightness" ); 
	strcpy( rgRetInfo[2].szKeyName, "Contrast" ); //
	strcpy( rgRetInfo[3].szKeyName, "Sharpness" );//
	strcpy( rgRetInfo[4].szKeyName, "Saturation" );//
	strcpy( rgRetInfo[5].szKeyName, "WDREnable" );//
	strcpy( rgRetInfo[6].szKeyName, "WDRLevel" );//
	strcpy( rgRetInfo[7].szKeyName, "Color" );
	strcpy( rgRetInfo[8].szKeyName, "n2DDeNoiseEnable" );
	strcpy( rgRetInfo[9].szKeyName, "n2DDeNoiseValue" );
	strcpy( rgRetInfo[10].szKeyName, "DeNoiseEnable" );
	strcpy( rgRetInfo[11].szKeyName, "DeNoiseMode" );//
	strcpy( rgRetInfo[12].szKeyName, "DeNoiseLevel" );//
	strcpy( rgRetInfo[13].szKeyName, "ManualShutter" );//
	strcpy( rgRetInfo[14].szKeyName, "ManualGain" );//
	strcpy( rgRetInfo[15].szKeyName, "ManualGainR" );//
	strcpy( rgRetInfo[16].szKeyName, "ManualGainG" );//
	strcpy( rgRetInfo[17].szKeyName, "ManualGainB" );//
	strcpy( rgRetInfo[18].szKeyName, "AWBEnable" );//
	strcpy( rgRetInfo[19].szKeyName, "AWBMode" );     
	strcpy( rgRetInfo[20].szKeyName, "AGCEnable" );//
	strcpy( rgRetInfo[21].szKeyName, "AGCLightBaseLine" );//
	strcpy( rgRetInfo[22].szKeyName, "AGCShutterMin" );//
	strcpy( rgRetInfo[23].szKeyName, "AGCShutterMax" );//
	strcpy( rgRetInfo[24].szKeyName, "AGCGainMin" );//
	strcpy( rgRetInfo[25].szKeyName, "AGCGainMax" );//
	strcpy( rgRetInfo[26].szKeyName, "AGCZone" );
	strcpy( rgRetInfo[27].szKeyName, "FilterMode" );//
	strcpy( rgRetInfo[28].szKeyName, "DCEnable" );//
	strcpy( rgRetInfo[29].szKeyName, "GammaEnable" );//
	strcpy( rgRetInfo[30].szKeyName, "GammaValue" );//
	strcpy( rgRetInfo[31].szKeyName, "SharpnessEnable" );//
	strcpy( rgRetInfo[32].szKeyName, "ACSyncMode" );//
	strcpy( rgRetInfo[33].szKeyName, "ACSyncDelay" );//
	strcpy( rgRetInfo[34].szKeyName, "GrayImageEnable" );//
	strcpy( rgRetInfo[35].szKeyName, "ImageEnhancementEnable" );//
	strcpy( rgRetInfo[36].szKeyName, "EnRedLightThreshold" );//
	strcpy( rgRetInfo[37].szKeyName, "DeNoiseSNFEnable" );//
	strcpy( rgRetInfo[38].szKeyName, "DeNoiseTNFEnable" );//
	strcpy( rgRetInfo[39].szKeyName, "EdgeEnhance" );//

	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
	if ( S_OK != HvXmlParse( "GetCameraBasicInfo", szRet ,nRetLen , rgRetInfo, 40 ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_GetCameraBasicInfo HvXmlParse Fail\n");
		return E_FAIL;
	}
	
 
	if ( XML_CMD_TYPE_INT == rgRetInfo[0].eKeyType )
	{
		nRetCode = atoi(rgRetInfo[0].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[1].eKeyType )
	{
		pCameraBasicInfo->nBrightness = atoi(rgRetInfo[1].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[2].eKeyType )
	{
		pCameraBasicInfo->nContrast = atoi(rgRetInfo[2].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[3].eKeyType )
	{
		pCameraBasicInfo->nSharpness = atoi(rgRetInfo[3].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[4].eKeyType )
	{
		pCameraBasicInfo->nSaturation = atoi(rgRetInfo[4].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[5].eKeyType )
	{
		pCameraBasicInfo->fWDREnable = atoi(rgRetInfo[5].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[6].eKeyType )
	{
		pCameraBasicInfo->nWDRLevel = atoi(rgRetInfo[6].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[7].eKeyType )
	{
		pCameraBasicInfo->nColor = atoi(rgRetInfo[7].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[8].eKeyType )
	{
		pCameraBasicInfo->f2DDeNoiseEnable = atoi(rgRetInfo[8].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[9].eKeyType )
	{
		pCameraBasicInfo->n2DeNoiseStrength = atoi(rgRetInfo[9].szKeyValue);
	}

	if ( XML_CMD_TYPE_INT == rgRetInfo[11].eKeyType )
	{
		pCameraBasicInfo->nDeNoiseMode = atoi(rgRetInfo[11].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[12].eKeyType )
	{
		pCameraBasicInfo->nDeNoiseLevel = atoi(rgRetInfo[12].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[13].eKeyType )
	{
		pCameraBasicInfo->nManualShutter = atoi(rgRetInfo[13].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[14].eKeyType )
	{
		pCameraBasicInfo->nManualGain = atoi(rgRetInfo[14].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[15].eKeyType )
	{
		pCameraBasicInfo->nManualGainR = atoi(rgRetInfo[15].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[16].eKeyType )
	{
		pCameraBasicInfo->nManualGainG = atoi(rgRetInfo[16].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[17].eKeyType )
	{
		pCameraBasicInfo->nManualGainB = atoi(rgRetInfo[17].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[18].eKeyType )
	{
		pCameraBasicInfo->fAWBEnable = atoi(rgRetInfo[18].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[20].eKeyType )
	{
		pCameraBasicInfo->fAGCEnable = atoi(rgRetInfo[20].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[21].eKeyType )
	{
		pCameraBasicInfo->nAGCLightBaseLine = atoi(rgRetInfo[21].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[22].eKeyType )
	{
		pCameraBasicInfo->nAGCShutterMin = atoi(rgRetInfo[22].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[23].eKeyType )
	{
		pCameraBasicInfo->nAGCShutterMax = atoi(rgRetInfo[23].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[24].eKeyType )
	{
		pCameraBasicInfo->nAGCGainMin = atoi(rgRetInfo[24].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[25].eKeyType )
	{
		pCameraBasicInfo->nAGCGainMax = atoi(rgRetInfo[25].szKeyValue);
	}

	if ( XML_CMD_TYPE_INT == rgRetInfo[27].eKeyType )
	{
		pCameraBasicInfo->nFilterMode = atoi(rgRetInfo[27].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[28].eKeyType )
	{
		pCameraBasicInfo->fDCEnable = atoi(rgRetInfo[28].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[29].eKeyType )
	{
		pCameraBasicInfo->fGammaEnable = atoi(rgRetInfo[29].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[30].eKeyType )
	{
		pCameraBasicInfo->nGammaStrength = atoi(rgRetInfo[30].szKeyValue);
	}

	if ( XML_CMD_TYPE_INT == rgRetInfo[37].eKeyType )
	{
		pCameraBasicInfo->fDeNoiseSNFEnable = atoi(rgRetInfo[37].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[38].eKeyType )
	{
		pCameraBasicInfo->fDeNoiseTNFEnable = atoi(rgRetInfo[38].szKeyValue);
	}
	
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		if ( S_OK != HvXmlParseMercury( "GetCameraBasicInfo", szRet ,nRetLen , rgRetInfo, 40) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetCameraBasicInfo HvXmlParse Fail\n");
			return E_FAIL;
		}
		    nRetCode = atoi(rgRetInfo[0].szKeyValue);
		pCameraBasicInfo->nContrast=atoi(rgRetInfo[2].szKeyValue);//"Contrast"  //
		pCameraBasicInfo->nSharpness=atoi(rgRetInfo[3].szKeyValue);//"Sharpness" //
		pCameraBasicInfo->nSaturation=atoi(rgRetInfo[4].szKeyValue);//"Saturation" //
		pCameraBasicInfo->fWDREnable=atoi(rgRetInfo[5].szKeyValue);//"WDREnable" //
		pCameraBasicInfo->nWDRLevel=atoi(rgRetInfo[6].szKeyValue);//"WDRLevel" //
		pCameraBasicInfo->nDeNoiseMode=atoi(rgRetInfo[11].szKeyValue);//"DeNoiseMode" //
		pCameraBasicInfo->nDeNoiseLevel=atoi(rgRetInfo[12].szKeyValue);//"DeNoiseLevel" //
		pCameraBasicInfo->nManualShutter = atoi( rgRetInfo[13].szKeyValue);// "ManualShutter" //
		pCameraBasicInfo->nManualGain = atoi( rgRetInfo[14].szKeyValue);// "ManualGain" //
		pCameraBasicInfo->nManualGainR = atoi( rgRetInfo[15].szKeyValue);// "ManualGainR" //
		pCameraBasicInfo->nManualGainG = atoi( rgRetInfo[16].szKeyValue);// "ManualGainG" //
		pCameraBasicInfo->nManualGainB = atoi( rgRetInfo[17].szKeyValue);// "ManualGainB" //
		pCameraBasicInfo->fAWBEnable=atoi(rgRetInfo[18].szKeyValue);//"AWBEnable" // 
		pCameraBasicInfo->fAGCEnable=atoi(rgRetInfo[20].szKeyValue);//"AGCEnable" //
		pCameraBasicInfo->nAGCLightBaseLine=atoi(rgRetInfo[21].szKeyValue);//"AGCLightBaseLine" //
		pCameraBasicInfo->nAGCShutterMin=atoi(rgRetInfo[22].szKeyValue);//"AGCShutterMin" //
		pCameraBasicInfo->nAGCShutterMax=atoi(rgRetInfo[23].szKeyValue);//"AGCShutterMax" //
		pCameraBasicInfo->nAGCGainMin=atoi(rgRetInfo[24].szKeyValue);//"AGCGainMin" //
		pCameraBasicInfo->nAGCGainMax=atoi(rgRetInfo[25].szKeyValue);//"AGCGainMax" //
		pCameraBasicInfo->nFilterMode=atoi(rgRetInfo[27].szKeyValue);//"FilterMode"//
		pCameraBasicInfo->fDCEnable=atoi(rgRetInfo[28].szKeyValue);//"DCEnable" //
		pCameraBasicInfo->fGammaEnable=atoi(rgRetInfo[29].szKeyValue);//"GammaEnable" //
		pCameraBasicInfo->nGammaStrength=atoi(rgRetInfo[30].szKeyValue);//"GammaValue" //
		pCameraBasicInfo->fSharpnessEnable=atoi(rgRetInfo[31].szKeyValue);//"SharpnessEnable" //
		pCameraBasicInfo->nACSyncMode=atoi(rgRetInfo[32].szKeyValue);//"ACSyncMode" //
		pCameraBasicInfo->nACSyncDelay=atoi(rgRetInfo[33].szKeyValue);//"ACSyncDelay" //
		pCameraBasicInfo->fGrayImageEnable=atoi(rgRetInfo[34].szKeyValue);//"GrayImageEnable" //
		pCameraBasicInfo->fImageEnhancementEnable=atoi(rgRetInfo[35].szKeyValue);//"ImageEnhancementEnable" //
		pCameraBasicInfo->nEnRedLightThreshold=atoi(rgRetInfo[36].szKeyValue);//"EnRedLightThreshold" //
		pCameraBasicInfo->fDeNoiseSNFEnable=atoi(rgRetInfo[37].szKeyValue);//"DeNoiseSNFEnable"//
		pCameraBasicInfo->fDeNoiseTNFEnable=atoi(rgRetInfo[38].szKeyValue);//"DeNoiseTNFEnable"//
		pCameraBasicInfo->nEdgeEnhance=atoi(rgRetInfo[39].szKeyValue);//"EdgeEnhance"//
	}
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}


HV_API_EX HRESULT CDECL HVAPI_GetVideoState(HVAPI_HANDLE_EX hHandle, CVideoState* pVideoState )
{

	if(hHandle == NULL || NULL == pVideoState )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	CHAR szRet[5*1024]={0};
	INT nRetLen = 5*1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , "GetVideoState" , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_GetVedioState HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	CXmlParseInfo rgRetInfo[12];
	strcpy( rgRetInfo[0].szKeyName, "RetCode" );
	strcpy( rgRetInfo[1].szKeyName, "CVBSDisplayMode" );
	strcpy( rgRetInfo[2].szKeyName, "H264FPS" );
	strcpy( rgRetInfo[3].szKeyName, "H264BitRateControl" );
	strcpy( rgRetInfo[4].szKeyName, "H264BitRate" );
	strcpy( rgRetInfo[5].szKeyName, "JpegFPS" );
	strcpy( rgRetInfo[6].szKeyName, "JpegCompressRate" );
	strcpy( rgRetInfo[7].szKeyName, "AutoJpegCompressEnable" );//
	strcpy( rgRetInfo[8].szKeyName, "JpegFileSize" );//
	strcpy( rgRetInfo[9].szKeyName, "JpegCompressMaxRate" );//
	strcpy( rgRetInfo[10].szKeyName, "JpegCompressMinRate" );//
	strcpy( rgRetInfo[11].szKeyName, "DebugJpegStatus" );//

	INT nRetCode = -1;
	if ( pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH )
	{
		if ( S_OK != HvXmlParse( "GetVideoState", szRet ,nRetLen , rgRetInfo, 7 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetVedioState HvXmlParse Fail\n");
			return E_FAIL;
		}
		
		if ( XML_CMD_TYPE_INT == rgRetInfo[0].eKeyType )
		{
			nRetCode = atoi(rgRetInfo[0].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[1].eKeyType )
		{
			pVideoState->nCVBSDisplayMode = atoi(rgRetInfo[1].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[2].eKeyType )
		{
			pVideoState->nH264FPS = atoi(rgRetInfo[2].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[3].eKeyType )
		{
			pVideoState->nH264BitRateControl = atoi(rgRetInfo[3].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[4].eKeyType )
		{
			pVideoState->nH264BitRate = atoi(rgRetInfo[4].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[5].eKeyType )
		{
			pVideoState->nJpegFPS = atoi(rgRetInfo[5].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[6].eKeyType )
		{
			pVideoState->nJpegCompressRate = atoi(rgRetInfo[6].szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		if ( S_OK != HvXmlParseMercury( "GetVideoState", szRet ,nRetLen , rgRetInfo, 12 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetVedioState HvXmlParse Fail\n");
			return E_FAIL;
		}


		nRetCode = atoi(rgRetInfo[0].szKeyValue);
		pVideoState->nCVBSDisplayMode = atoi(rgRetInfo[1].szKeyValue);
		pVideoState->nH264BitRate = atoi(rgRetInfo[4].szKeyValue);
		pVideoState->nJpegCompressRate = atoi(rgRetInfo[6].szKeyValue);
		pVideoState->fAutoJpegCompressEnable = atoi(rgRetInfo[7].szKeyValue);//"AutoJpegCompressEnable" );//
		pVideoState->nJpegFileSize = atoi(rgRetInfo[8].szKeyValue);//"JpegFileSize" );//
		pVideoState->nJpegCompressMaxRate = atoi(rgRetInfo[9].szKeyValue);//"JpegCompressMaxRate" );//
		pVideoState->nJpegCompressMinRate = atoi(rgRetInfo[10].szKeyValue);//"JpegCompressMinRate" );//
		pVideoState->nDebugJpegStatus = atoi(rgRetInfo[11].szKeyValue);//"DebugJpegStatus" );//

	}
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_GetHddCheckReport(HVAPI_HANDLE_EX hHandle, CHAR* szReport , INT *piReportLen )
{

	if(hHandle == NULL || NULL == szReport || NULL == piReportLen)
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
		CHAR szRet [1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetHddCheckReport" , szRet , nRetLen , &nRetLen ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetHddCheckReport HVAPI_ExecCmdEx Fail\n");
			return E_FAIL ;
		}

		//解析命令返回


		INT nRetCode = -1;

		CXmlParseInfo rgRetInfo [2];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );
		strcpy( rgRetInfo [1].szKeyName, "RetMsg" );

		if ( S_OK != HvXmlParse( "GetHddCheckReport", szRet ,nRetLen , rgRetInfo, 2 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetHddCheckReport HvXmlParse Fail\n");
			return E_FAIL ;
		}
 
		if ( XML_CMD_TYPE_INT == rgRetInfo[0]. eKeyType )
		{
			nRetCode = atoi (rgRetInfo[0]. szKeyValue);
		}
		if ( XML_CMD_TYPE_STRING == rgRetInfo[1]. eKeyType )
		{
			strcpy(szReport, rgRetInfo[1].szKeyValue);
			*piReportLen=strlen(rgRetInfo[1].szKeyValue);
		}
		return nRetCode ==0 ?S_OK: E_FAIL;

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		if ( S_OK != ExecuteCmd( pHHC->szIP ,CAMERA_GET_HDD_REPORT_LOG , NULL ,0 ,szReport , piReportLen ) )
		{
			WrightLogEx(pHHC->szIP , "ExecuteCmd CAMERA_TRIGGER_IMAGE Fail\n" );
			return E_FAIL;
		}

		return S_OK;
 
	}
	return E_FAIL;
	
}

HV_API_EX HRESULT CDECL HVAPI_GetOSDInfo(HVAPI_HANDLE_EX hHandle,  INT nStreamId , COSDInfo* pOSDInfo  )
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
		CHAR szCmd[128]={0};
		sprintf( szCmd , "GetOSD,nStreamId[%d]" ,nStreamId );

		CHAR szRet[5*1024]={0};
		INT nRetLen = 5*1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetOSDInfo HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		//解析命令返回
		CXmlParseInfo rgRetInfo[10];
		strcpy( rgRetInfo[0].szKeyName, "RetCode" );
		strcpy( rgRetInfo[1].szKeyName, "TimeStampEnable" );
		strcpy( rgRetInfo[2].szKeyName, "OSDEnable" );
		strcpy( rgRetInfo[3].szKeyName, "PosX" );
		strcpy( rgRetInfo[4].szKeyName, "PosY" );
		strcpy( rgRetInfo[5].szKeyName, "FontSize" );
		strcpy( rgRetInfo[6].szKeyName, "ColorR" );
		strcpy( rgRetInfo[7].szKeyName, "ColorG" );
		strcpy( rgRetInfo[8].szKeyName, "ColorB" );
		strcpy( rgRetInfo[9].szKeyName, "Text" );

		if ( S_OK != HvXmlParse( "GetOSD", szRet ,nRetLen , rgRetInfo, 10 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetOSDInfo HvXmlParse Fail\n");
			return E_FAIL;
		}
		INT nRetCode = -1;
		if ( XML_CMD_TYPE_INT == rgRetInfo[0].eKeyType )
		{
			nRetCode = atoi(rgRetInfo[0].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[1].eKeyType )
		{
			pOSDInfo->fTimeStampEnable = atoi(rgRetInfo[1].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[2].eKeyType )
		{
			pOSDInfo->fEnable = atoi(rgRetInfo[2].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[3].eKeyType )
		{
			pOSDInfo->nPosX = atoi(rgRetInfo[3].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[4].eKeyType )
		{
			pOSDInfo->nPosY = atoi(rgRetInfo[4].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[5].eKeyType )
		{
			pOSDInfo->nFontSize = atoi(rgRetInfo[5].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[6].eKeyType )
		{
			pOSDInfo->nFontColorR = atoi(rgRetInfo[6].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[7].eKeyType )
		{
			pOSDInfo->nFontColorG = atoi(rgRetInfo[7].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[8].eKeyType )
		{
			pOSDInfo->nFontColorB = atoi(rgRetInfo[8].szKeyValue);
		}
		if ( XML_CMD_TYPE_STRING == rgRetInfo[9].eKeyType )
		{
			if ( sizeof(pOSDInfo->szText) > strlen(rgRetInfo[9].szKeyValue ))
			{
				strcpy(pOSDInfo->szText , rgRetInfo[9].szKeyValue);
			}

		}
		return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CHAR szCmd[128]={0};
		CHAR szCmdName[128]={0};
		if (0 == nStreamId)
		{
			 sprintf(szCmdName, "GetH264Caption");
		}
		else if (1 == nStreamId)
		{
			sprintf(szCmdName, "GetJPEGCaption");
		}
		else if (2 == nStreamId)
		{
			sprintf(szCmdName, "GetH264SecondCaption");
		}
		else
		{
			return S_FALSE;
		}
		sprintf( szCmd , "%s" ,szCmdName );

		CHAR szRet[5*1024]={0};
		INT nRetLen = 5*1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetOSDInfo HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}

		//解析命令返回
		CXmlParseInfo rgRetInfo[15];
		strcpy( rgRetInfo[0].szKeyName, "RetCode" );
		strcpy( rgRetInfo[1].szKeyName, "TimeStampEnable" );//
		strcpy( rgRetInfo[2].szKeyName, "OSDEnable" );//
		strcpy( rgRetInfo[3].szKeyName, "PosX" );//
		strcpy( rgRetInfo[4].szKeyName, "PosY" );//
		strcpy( rgRetInfo[5].szKeyName, "FontSize" );//
		strcpy( rgRetInfo[6].szKeyName, "ColorR" );//
		strcpy( rgRetInfo[7].szKeyName, "ColorG" );//
		strcpy( rgRetInfo[8].szKeyName, "ColorB" );//
		strcpy( rgRetInfo[9].szKeyName, "Text" );//
		strcpy( rgRetInfo[10].szKeyName,"PlateEnable" );//

		if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , rgRetInfo, 11 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetOSDInfo HvXmlParse Fail\n");
			return E_FAIL;
		}
		INT nRetCode = -1;

		pOSDInfo->fTimeStampEnable = atoi(rgRetInfo[1].szKeyValue);//"TimeStampEnable" );//
		pOSDInfo->fEnable = atoi(rgRetInfo[2].szKeyValue);//"OSDEnable" );//
		pOSDInfo->nPosX = atoi(rgRetInfo[3].szKeyValue);//"PosX" );//
		pOSDInfo->nPosY = atoi(rgRetInfo[4].szKeyValue);//"PosY" );//
		pOSDInfo->nFontSize = atoi(rgRetInfo[5].szKeyValue);//"FontSize" );//
		pOSDInfo->nFontColorR = atoi(rgRetInfo[6].szKeyValue);//"ColorR" );//
		pOSDInfo->nFontColorG = atoi(rgRetInfo[7].szKeyValue);//"ColorG" );//
		pOSDInfo->nFontColorB = atoi(rgRetInfo[8].szKeyValue);//"ColorB" );//
		strcpy(pOSDInfo->szText, rgRetInfo[9].szKeyValue);//"Text" );//
		pOSDInfo->fPlateEnable = atoi(rgRetInfo[10].szKeyValue);//"PlateEnable" );//


	}
 return S_OK;
}

HV_API_EX HRESULT CDECL HVAPI_GetLUT(HVAPI_HANDLE_EX hHandle, CHvPoint rgLUT[] , INT* pnLUTPointCount )
{
	if(hHandle == NULL || NULL == pnLUTPointCount )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	CHAR szRet[5*1024]={0};
	INT nRetLen = 5*1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , "GetLUT" , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_GetLUT HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	const int MAX_POINT_NUM = 15;
	CXmlParseInfo rgRetInfo[MAX_POINT_NUM*2+2];
	strcpy( rgRetInfo[0].szKeyName, "RetCode" );
	strcpy( rgRetInfo[1].szKeyName, "PointCount" );
	
	INT nLUTPointCount = MAX_POINT_NUM-1>=*pnLUTPointCount ?*pnLUTPointCount:MAX_POINT_NUM-1;
	INT nRetInfotNum = 2;
	for ( INT i = 0 ; i< nLUTPointCount ; i++ )
	{
		sprintf( rgRetInfo[nRetInfotNum].szKeyName  , "Point%02d_X" , i);
		nRetInfotNum++;
		sprintf( rgRetInfo[nRetInfotNum].szKeyName  , "Point%02d_Y" , i);
		nRetInfotNum++;
	}
	if ( S_OK != HvXmlParse( "GetLUT", szRet ,nRetLen , rgRetInfo, nRetInfotNum ) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_GetLUT HvXmlParse Fail\n");
		return E_FAIL;
	}
	INT nRetCode = -1;
	INT nRetPointCount = 0;
	if ( XML_CMD_TYPE_INT == rgRetInfo[0].eKeyType )
	{
		nRetCode = atoi(rgRetInfo[0].szKeyValue);
	}
	if ( XML_CMD_TYPE_INT == rgRetInfo[1].eKeyType )
	{
		nRetPointCount = atoi(rgRetInfo[1].szKeyValue);
	}
	nRetInfotNum = 2;
	*pnLUTPointCount = 0;
	nLUTPointCount = nLUTPointCount>nRetPointCount?nRetPointCount:nLUTPointCount;
	for ( INT i = 0 ; i< nLUTPointCount ; ++i  )
	{
		if ( XML_CMD_TYPE_INT == rgRetInfo[nRetInfotNum].eKeyType &&
			XML_CMD_TYPE_INT == rgRetInfo[nRetInfotNum+1].eKeyType )
		{
			rgLUT[i].nX = atoi( rgRetInfo[nRetInfotNum].szKeyValue );
			rgLUT[i].nY = atoi( rgRetInfo[nRetInfotNum+1].szKeyValue );
			(*pnLUTPointCount)++;
		}
		nRetInfotNum+=2;
	}
	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

BOOL IsVailUserName( CHAR* szUserName , INT nUserNameLen )
{
	if ( NULL == szUserName || nUserNameLen <= 0 )
	{
		return FALSE;
	}
	BOOL fIsVail = TRUE;
	for ( INT i = 0 ; i < nUserNameLen; i++ )
	{
		if ( '-' != szUserName[i]
			&& '_' != szUserName[i]
			&& ( szUserName[i] < '0' || szUserName[i] > '9' )
			&& ( szUserName[i] < 'a' || szUserName[i] > 'z' )
			&& ( szUserName[i] < 'A' || szUserName[i] > 'Z' )
			)
		{
			fIsVail = FALSE;
			break;
		}
	}
	return fIsVail;
}


BOOL IsVailPassword( CHAR* szPassword , INT nPasswordLen )
{
	if ( NULL == szPassword || nPasswordLen <= 0 )
	{
		return FALSE;
	}
	BOOL fIsVail = TRUE;
	for ( INT i = 0 ; i < nPasswordLen; i++ )
	{
		if ( '-' != szPassword[i]
		&& '_' != szPassword[i]
		&& ( szPassword[i] < '0' || szPassword[i] > '9' )
			&& ( szPassword[i] < 'a' || szPassword[i] > 'z' )
			&& ( szPassword[i] < 'A' || szPassword[i] > 'Z' )
			)
		{
			fIsVail = FALSE;
			break;
		}
	}
	return fIsVail;
}

HV_API_EX HRESULT CDECL HVAPI_Login(HVAPI_HANDLE_EX hHandle, CHAR* szUserName , CHAR* szPassword  )
{
	if(hHandle == NULL || NULL == szUserName || NULL == szPassword )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}
	const INT MAX_USERNAME_LEN = 64;
	const INT MAX_PASSWORD_LEN = 64;
	if ( strlen( szUserName ) > MAX_USERNAME_LEN || 
		strlen( szPassword ) > MAX_PASSWORD_LEN  )
	{
		return S_FALSE;
	}
		
	if ( FALSE == IsVailUserName( szUserName , strlen( szUserName ) ) || 
		 FALSE == IsVailPassword( szPassword , strlen( szPassword ) ) 
		 )
	{
		return S_FALSE;
	}
	//执行命令
	CHAR szCmd[256]={0};
	sprintf( szCmd , "DoLogin,UserName[%s],Password[%s]" ,szUserName , szPassword );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_Login HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}
	
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		//解析命令返回
		CXmlParseInfo rgRetInfo[2];
		strcpy( rgRetInfo[0].szKeyName, "RetCode" );
		strcpy( rgRetInfo[1].szKeyName, "Authority" );

		if ( S_OK != HvXmlParse( "DoLogin", szRet ,nRetLen ,rgRetInfo , 2 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_Login HvXmlParse Fail\n");
			return E_FAIL;
		}
		
		if ( XML_CMD_TYPE_INT == rgRetInfo[0].eKeyType )
		{
			nRetCode = atoi(rgRetInfo[0].szKeyValue);
		}
		INT nAuthority = -1;
		if ( XML_CMD_TYPE_INT == rgRetInfo[1].eKeyType )
		{
			nAuthority = atoi(rgRetInfo[1].szKeyValue);
		}

		if (0 == nRetCode)
		{
			strcpy( pHHC->szUserName ,szUserName);
			strcpy( pHHC->szPassword ,szPassword);
			pHHC->nAuthority = nAuthority;
		}

		
	}
	else if ( pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		//解析命令返回
		CXmlParseInfo rgRetInfo[2];
		strcpy( rgRetInfo[0].szKeyName, "RetCode" );
		strcpy( rgRetInfo[1].szKeyName, "Authority" );

		if ( S_OK != HvXmlParseMercury( "DoLogin", szRet ,nRetLen ,rgRetInfo , 2 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_Login HvXmlParse Fail\n");
			return E_FAIL;
		}

		INT nAuthority = -1;
		nRetCode = atoi(rgRetInfo[0].szKeyValue);
		nAuthority = atoi(rgRetInfo[1].szKeyValue);

		if (0 == nRetCode)
		{
			strcpy( pHHC->szUserName ,szUserName);
			strcpy( pHHC->szPassword ,szPassword);
			pHHC->nAuthority = nAuthority;
		}
	}
 
	return nRetCode==0 ?S_OK: ( nRetCode == -2?S_FALSE: E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_AddUser( HVAPI_HANDLE_EX hHandle, CHAR* szUserName , CHAR* szPassword, INT nAuthority )
{
	if(hHandle == NULL || NULL == szUserName || NULL == szPassword )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	const INT MAX_USERNAME_LEN = 64;
	const INT MAX_PASSWORD_LEN = 64;
	if ( strlen( szUserName ) > MAX_USERNAME_LEN ||
		strlen( szPassword ) > MAX_PASSWORD_LEN )
	{
		return S_FALSE;
	}

	if ( FALSE == IsVailUserName( szUserName , strlen( szUserName ) ) || 
		FALSE == IsVailPassword( szPassword , strlen( szPassword ) ) 
		)
	{
		return S_FALSE;
	}

	//执行命令
	CHAR szCmd[512]={0};
	sprintf( szCmd , "AddUser,UserName[%s],Password[%s],AddUserName[%s],AddUserPassword[%s],AddUserAuthority[%d]" ,
				pHHC->szUserName, pHHC->szPassword, szUserName , szPassword , nAuthority );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_AddUser HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "AddUser", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_AddUser HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "AddUser", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_AddUser HvXmlParse Fail\n");
			return E_FAIL;
		}
 
			nRetCode = atoi(cRetInfo.szKeyValue);
	 
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}



HV_API_EX HRESULT CDECL HVAPI_DelUser(HVAPI_HANDLE_EX hHandle, CHAR* szUserName  )
{
	if(hHandle == NULL || NULL == szUserName )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	const INT MAX_USERNAME_LEN = 64;
	if ( strlen( szUserName ) > MAX_USERNAME_LEN )
	{
		return S_FALSE;
	}

	if ( FALSE == IsVailUserName( szUserName , strlen( szUserName ) )) 
	{
		return S_FALSE;
	}

	//执行命令
	CHAR szCmd[512]={0};
	sprintf( szCmd , "DelUser,UserName[%s],Password[%s],DelUserName[%s]" ,
			pHHC->szUserName, pHHC->szPassword,szUserName );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_DelUser HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "DelUser", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_DelUser HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "DelUser", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_DelUser HvXmlParse Fail\n");
			return E_FAIL;
		}
        nRetCode = atoi(cRetInfo.szKeyValue);
	}

	

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_GetUsersList(HVAPI_HANDLE_EX hHandle, CUserInfo rgUserInfo[] , INT* pnUserCount )
{
	if(hHandle == NULL || NULL == pnUserCount )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}
	CHAR szRet[16*1024]={0};
	INT nRetLen = 16*1024;

	//执行命令
	CHAR szCmd[512]={0};
	sprintf( szCmd , "GetUsers,UserName[%s],Password[%s]" 
		,strlen(pHHC->szUserName)<=0?"NULL":pHHC->szUserName
		,strlen(pHHC->szPassword)<=0?"NULL":pHHC->szPassword );

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_GetUsersList HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}


	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		//解析命令返回
		const INT MAX_USER_NUM = 32;
		CXmlParseInfo rgRetInfo[MAX_USER_NUM*2+2];
		strcpy( rgRetInfo[0].szKeyName, "RetCode" );
		strcpy( rgRetInfo[1].szKeyName, "UserCount" );

		INT nMaxUserNum = MAX_USER_NUM>=*pnUserCount ?*pnUserCount:MAX_USER_NUM;


		INT nRetInfotNum = 2;
		for ( INT i = 0 ; i< nMaxUserNum ;i++  )
		{
			sprintf( rgRetInfo[nRetInfotNum].szKeyName  , "User%02d_Name" , i);
			nRetInfotNum++;
			sprintf( rgRetInfo[nRetInfotNum].szKeyName  , "User%02d_Authority" , i);
			nRetInfotNum++;
		}

		if ( S_OK != HvXmlParse( "GetUsers", szRet ,nRetLen , rgRetInfo, nRetInfotNum ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetUsersList HvXmlParse Fail\n");
			return E_FAIL;
		}
		INT nRetCode = -1;
		if ( XML_CMD_TYPE_INT == rgRetInfo[0].eKeyType )
		{
			nRetCode = atoi(rgRetInfo[0].szKeyValue);
		}
		nRetInfotNum = 2;
		*pnUserCount = 0;
		for ( INT i = 0 ; i< nMaxUserNum ; ++i  )
		{
			if ( XML_CMD_TYPE_STRING == rgRetInfo[nRetInfotNum].eKeyType &&
				XML_CMD_TYPE_INT == rgRetInfo[nRetInfotNum+1].eKeyType )
			{
				if ( sizeof(rgUserInfo[i].szUserName) >= strlen(rgRetInfo[nRetInfotNum].szKeyValue) )
				{
					strcpy( rgUserInfo[i].szUserName , rgRetInfo[nRetInfotNum].szKeyValue );
				}
				rgUserInfo[i].nAuthority = atoi( rgRetInfo[nRetInfotNum+1].szKeyValue );
				(*pnUserCount)++;
			}
			nRetInfotNum+=2;
		}

		return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		//解析命令返回
		const INT MAX_USER_NUM = 32;
		CXmlParseInfo rgRetInfo[MAX_USER_NUM*2+2];
		strcpy( rgRetInfo[0].szKeyName, "RetCode" );
		strcpy( rgRetInfo[1].szKeyName, "UserCount" );

		INT nMaxUserNum = MAX_USER_NUM>=*pnUserCount ?*pnUserCount:MAX_USER_NUM;


		INT nRetInfotNum = 2;
		for ( INT i = 0 ; i< nMaxUserNum ;i++  )
		{
			sprintf( rgRetInfo[nRetInfotNum].szKeyName  , "UserName%02d" , i);
			nRetInfotNum++;
			sprintf( rgRetInfo[nRetInfotNum].szKeyName  , "UserAuthority%02d" , i);
			nRetInfotNum++;
		}

		if ( S_OK != HvXmlParseMercury( "GetUsers", szRet ,nRetLen , rgRetInfo, nRetInfotNum ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_GetUsersList HvXmlParse Fail\n");
			return E_FAIL;
		}
		INT nRetCode = -1;
 
		nRetCode = atoi(rgRetInfo[0].szKeyValue);
		*pnUserCount = atoi(rgRetInfo[1].szKeyValue);

		nRetInfotNum = 2;
		for ( INT i = 0 ; i< *pnUserCount ; ++i  )
		{
 
				if ( sizeof(rgUserInfo[i].szUserName) >= strlen(rgRetInfo[nRetInfotNum].szKeyValue) )
				{
					strcpy( rgUserInfo[i].szUserName , rgRetInfo[nRetInfotNum].szKeyValue );
				}
				rgUserInfo[i].nAuthority = atoi( rgRetInfo[nRetInfotNum+1].szKeyValue );
				nRetInfotNum+=2;
			 
		}

		return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
	}
	return E_FAIL;
}

HV_API_EX HRESULT CDECL HVAPI_ModUser(HVAPI_HANDLE_EX hHandle,  CHAR* szUserName , CHAR* szOldPassword,  CHAR* szNewPassword)
{
	if(hHandle == NULL || NULL == szUserName  
		|| NULL == szNewPassword || NULL == szOldPassword )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	const INT MAX_USERNAME_LEN = 64;
	const INT MAX_PASSWORD_LEN = 64;
	if ( strlen( szUserName ) > MAX_USERNAME_LEN 
		|| strlen( szNewPassword ) > MAX_PASSWORD_LEN 
		|| strlen( szOldPassword ) > MAX_PASSWORD_LEN )
	{
		return S_FALSE;
	}

	if ( FALSE == IsVailUserName( szUserName , strlen( szUserName ) ) || 
		 FALSE == IsVailPassword( szOldPassword , strlen( szOldPassword ) ) || 
		 FALSE == IsVailPassword( szNewPassword , strlen( szNewPassword ) ) 
		 )
	{
		return S_FALSE;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		//若未登录
		if ( 0 != strcmp( pHHC->szUserName , szUserName ) 
			|| 0 != strcmp( pHHC->szPassword , szOldPassword ) )
		{
			return E_FAIL;
		}

		//执行命令
		CHAR szCmd[512]={0};
		sprintf( szCmd , "ModUser,UserName[%s],Password[%s],ModUserName[%s],ModUserPassword[%s],ModUserAuthority[%d]" ,
				pHHC->szUserName, pHHC->szPassword ,szUserName , szNewPassword , pHHC->nAuthority );

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_ModUser HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}


		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "ModUser", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_ModUser HvXmlParse Fail\n");
			return E_FAIL;
		}
 
		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{

		BOOL fForceModUser = FALSE;
		INT nAuthority =  -1;
		if ( 0 != strcmp(pHHC->szUserName, "admin"))
		{
			//若未登录
			if ( 0 != strcmp( pHHC->szUserName , szUserName ) 
				|| 0 != strcmp( pHHC->szPassword , szOldPassword ) )
			{
				return E_FAIL;
			}
		}
		else
		{
			if ( 0  != strcmp(szUserName, "admin"))
			{
				fForceModUser = TRUE;
				CUserInfo rgUserInfo[20];
				INT nUserCount=20;
				HRESULT result=HVAPI_GetUsersList( hHandle, rgUserInfo, &nUserCount );

				if (S_OK == result)
				{
					BOOL fFind=FALSE;
					for (INT i=0; i<nUserCount; i++)
					{
						if(0 == strcmp(rgUserInfo[i].szUserName, szUserName ) )
						{
							fFind = TRUE;
							break;
						}
					}

					if ( !fFind)
					{
						return S_FALSE;
					}
					else
					{
						nAuthority = rgUserInfo[i].nAuthority;
					}
				}
				else
				{
					return E_FAIL;
				}
				
			}

			else
			{
				//如果是管理员改自己的密码
				if ( 0 != strcmp( pHHC->szUserName , szUserName ) 
					|| 0 != strcmp( pHHC->szPassword , szOldPassword ) )
				{
					return E_FAIL;
				}
			}
		}
		

		if (! fForceModUser)
		{
			nAuthority = pHHC->nAuthority;
		}

		//执行命令
		CHAR szCmd[512]={0};
		sprintf( szCmd , "ModUser,UserName[%s],Password[%s],ModUserName[%s],ModUserPassword[%s],ModUserAuthority[%d]" ,
			pHHC->szUserName, pHHC->szPassword ,szUserName , szNewPassword , nAuthority );

		CHAR szRet[1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_ModUser HVAPI_ExecCmdEx Fail\n");
			return E_FAIL;
		}


		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "ModUser", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_ModUser HvXmlParse Fail\n");
			return E_FAIL;
		}
 
 
		nRetCode = atoi(cRetInfo.szKeyValue);

		if (0 == strcmp( pHHC->szUserName , szUserName ) )//自己改自己的密码
		{
			if (0 == nRetCode)
			{
				strcpy( pHHC->szPassword ,szNewPassword);
			}
		}

		 
	}



	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_TriggerImage(HVAPI_HANDLE_EX hHandle, PBYTE pCapImage , INT* pnImageSize )
{
	return E_FAIL;

	/*
	if(hHandle == NULL || NULL == pCapImage || NULL == pnImageSize )
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	if ( S_OK != ExecuteCmd( pHHC->szIP ,CAMERA_TRIGGER_IMAGE , NULL ,0 ,(CHAR*)pCapImage , pnImageSize ) )
	{
		WrightLogEx(pHHC->szIP , "ExecuteCmd CAMERA_TRIGGER_IMAGE Fail\n" );
		return E_FAIL;
	}

	return S_OK;*/
}

//TODO（liyh） 以ip来连接设备，获取工作模式
HV_API_EX HRESULT CDECL HVAPI_GetDevRunMode(LPCSTR szIp, 	INT* pnDeviceRunMode )// 获取设备当前运行模式
{
	if(NULL == pnDeviceRunMode )
	{
		return E_FAIL ;
	}

	const int iBufLen = (1024 << 4);
	char* pszXmlBuf = new char[iBufLen];
	int iXmlBufLen = iBufLen;

	if(!pszXmlBuf)
	{
		return E_FAIL;
	}
	memset(pszXmlBuf, 0, iBufLen);
	DWORD dwXmlVersion = 0;

	PROTOCOL_VERSION eXmlVersion = GetProtocolVersion((char*)szIp);
	if(HvMakeXmlInfoByString( eXmlVersion , "OptResetMode", sizeof("OptResetMode"), pszXmlBuf, iXmlBufLen) == E_FAIL)
	{
		return E_FAIL;
	}
	char* pszRetBuf = new char[1024];
	if(!pszRetBuf)
	{
		SAFE_DELETE_ARG(pszXmlBuf);
		return E_FAIL;
	}
	memset(pszRetBuf, 0, 1024);
	int iRetBufLen = 1024;
	SOCKET sktCmd = INVALID_SOCKET;

	if( PROTOCOL_VERSION_MERCURY == eXmlVersion  )
	{
		if(ExecXmlExtCmdMercury((char*)szIp, (char*)pszXmlBuf, (char*)pszRetBuf, iRetBufLen, sktCmd) == false)
		{
			SAFE_DELETE_ARG(pszRetBuf);
			SAFE_DELETE_ARG(pszXmlBuf);
			return E_FAIL;
		}
	}
	else
	{
		return E_FAIL;
	}	 

	CXmlParseInfo rgRetInfo [2];
	strcpy( rgRetInfo [0].szKeyName, "RetCode" );
	strcpy( rgRetInfo [1].szKeyName, "ResetMode" );

	if ( S_OK != HvXmlParseMercury( "OptResetMode", pszRetBuf ,strlen(pszRetBuf) ,rgRetInfo , 2 ) )
	{
		WrightLogEx("OptResetMode" , "OptResetMode HvXmlParse Fail\n");
		return E_FAIL;
	}
	INT nRetCode = -1;

	nRetCode = atoi (rgRetInfo[0]. szKeyValue);
	*pnDeviceRunMode = atoi (rgRetInfo[1]. szKeyValue);


	 return nRetCode ==0 ?S_OK: E_FAIL; 
	
}

HV_API_EX HRESULT CDECL HVAPI_GetAGCZone(HVAPI_HANDLE_EX hHandle, INT rgZone[16])//获取AGC检测区域,16个值
{
	if(hHandle == NULL || rgZone == NULL )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}

	CHAR szRet [5*1024]={0};
	INT nRetLen = 5*1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetAGCZone" , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetAGCZone HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo rgRetInfo [3];
		strcpy( rgRetInfo[0].szKeyName, "RetCode" );
		strcpy( rgRetInfo[1].szKeyName, "AGCZoneCount" );
		strcpy( rgRetInfo[2].szKeyName, "AGCZone" );

		if ( S_OK != HvXmlParse( "GetAGCZone", szRet ,nRetLen , rgRetInfo, 3 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetAGCZone HvXmlParse Fail\n");
			return E_FAIL ;
		}
		
		INT nAGCZoneCount = 0;
		INT nAGCZoneValue = 0;
		if ( XML_CMD_TYPE_INT == rgRetInfo[0]. eKeyType )
		{
			nRetCode = atoi(rgRetInfo[0].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[1]. eKeyType )
		{
			nAGCZoneCount = atoi(rgRetInfo[1].szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[2]. eKeyType )
		{
			nAGCZoneValue = atoi(rgRetInfo[2].szKeyValue);
		}

		for ( INT i = 0 ; i< nAGCZoneCount ; i++  )
		{
			rgZone[i] = ((nAGCZoneValue >>i) & 0x1);
		}

	} 
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		const int MAX_POINT_NUM = 16;
		CXmlParseInfo rgRetInfo [MAX_POINT_NUM+1];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );
		//返回的RetMsg RetMsg="1,2,3,4,..."属性已被拆分为 AGCZone00="1" AGCZone01="2" AGCZone02="3"... 

		int nStartIndexNum=1;
		INT nZoneNum = 16;
		INT nRetInfotNum = nStartIndexNum ;
		for ( INT i = 0 ; i< nZoneNum ; i++ )
		{
			sprintf( rgRetInfo [nRetInfotNum]. szKeyName  , "AGCZone%02d" , i);
			nRetInfotNum++;
		}
		if ( S_OK != HvXmlParseMercury( "GetAGCZone", szRet ,nRetLen , rgRetInfo, nRetInfotNum ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetAGCZone HvXmlParse Fail\n");
			return E_FAIL ;
		}
		INT nRetPointCount = 0;

		nRetCode = atoi (rgRetInfo[0]. szKeyValue);	

		nRetInfotNum = 1;
		nZoneNum=16;
		for ( INT i = 0 ; i< nZoneNum ; ++i  )
		{
			rgZone[i]=atoi(rgRetInfo[i+nStartIndexNum].szKeyValue); 
			nRetInfotNum++;
		}
		
	}
	
	return nRetCode ==0 ?S_OK: E_FAIL;
}

HV_API_EX HRESULT CDECL HVAPI_SetGrayImageEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable)//    黑白图模式
{
	if(hHandle == NULL )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}
	//执行命令
	CHAR szCmd [128]={0};
	sprintf( szCmd , "SetEnableGrayImage,Value[%d]" , fEnable );

	CHAR szRet [1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_EnableGrayImage HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{


		CXmlParseInfo cRetInfo ;
		strcpy( cRetInfo .szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetEnableGrayImage", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_EnableGrayImage HvXmlParse Fail\n");
			return E_FAIL ;
		}

 
		if ( XML_CMD_TYPE_INT == cRetInfo. eKeyType )
		{
			nRetCode = atoi (cRetInfo. szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{

		CXmlParseInfo cRetInfo ;
		strcpy( cRetInfo .szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetEnableGrayImage", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_EnableGrayImage HvXmlParse Fail\n");
			return E_FAIL ;
		}

 
			nRetCode = atoi (cRetInfo. szKeyValue);
		 
	}

	return nRetCode ==0 ?S_OK: E_FAIL;

}
HV_API_EX HRESULT CDECL HVAPI_SetACSync(HVAPI_HANDLE_EX hHandle, INT nLevel, DWORD32 dw32DelayMS) //    电网同步
{
	if(hHandle == NULL )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}
	//执行命令
	CHAR szCmd [128]={0};
	sprintf( szCmd , "SetSyncPower,Mode[%d],DelayMS[%d]" , nLevel, dw32DelayMS );

	CHAR szRet [1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_SyncPower HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		CXmlParseInfo cRetInfo ;
		strcpy( cRetInfo .szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetSyncPower", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_SyncPower HvXmlParse Fail\n");
			return E_FAIL ;
		}

	 
		if ( XML_CMD_TYPE_INT == cRetInfo. eKeyType )
		{
			nRetCode = atoi (cRetInfo. szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{

		CXmlParseInfo cRetInfo ;
		strcpy( cRetInfo .szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetSyncPower", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_SyncPower HvXmlParse Fail\n");
			return E_FAIL ;
		}

 
			nRetCode = atoi (cRetInfo. szKeyValue);
		 
	}

	return nRetCode ==0 ?S_OK: E_FAIL;

}
HV_API_EX HRESULT CDECL HVAPI_GetDebugJpegStatus(HVAPI_HANDLE_EX hHandle, BOOL* pfEnable)//  获取调试码流开关
{
	if(hHandle == NULL || NULL == pfEnable )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}

	CHAR szRet [1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetDebugJpegStatus" , szRet , nRetLen , &nRetLen ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetDebugJpegStatus HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		CXmlParseInfo rgRetInfo [2];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );
		strcpy( rgRetInfo [1].szKeyName, "RetMsg" );

		if ( S_OK != HvXmlParse( "GetDebugJpegStatus", szRet ,nRetLen , rgRetInfo, 2 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetDebugJpegStatus HvXmlParse Fail\n");
			return E_FAIL ;
		}

 
		if ( XML_CMD_TYPE_INT == rgRetInfo[0]. eKeyType )
		{
			nRetCode = atoi (rgRetInfo[0]. szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[1]. eKeyType )
		{
			* pfEnable = atoi (rgRetInfo[1]. szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{

		CXmlParseInfo rgRetInfo [2];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );
		strcpy( rgRetInfo [1].szKeyName, "value" );

		if ( S_OK != HvXmlParseMercury( "GetDebugJpegStatus", szRet ,nRetLen , rgRetInfo, 2 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetDebugJpegStatus HvXmlParse Fail\n");
			return E_FAIL ;
		}

 
			nRetCode = atoi (rgRetInfo[0]. szKeyValue);
 
			* pfEnable = atoi (rgRetInfo[1]. szKeyValue);
		 
	}



	return nRetCode ==0 ?S_OK: E_FAIL;
}

HV_API_EX HRESULT CDECL HVAPI_SetDebugJpegStatus(HVAPI_HANDLE_EX hHandle, BOOL fEnable) //  设置调试码流开关
{
	if(hHandle == NULL )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}
	//执行命令
	CHAR szCmd [128]={0};
	sprintf( szCmd , "SetDebugJpegStatus,Value[%d]" , fEnable );

	CHAR szRet [1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_SetDebugJpegStatus HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo ;
		strcpy( cRetInfo .szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetDebugJpegStatus", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_SetDebugJpegStatus HvXmlParse Fail\n");
			return E_FAIL ;
		}

 
		if ( XML_CMD_TYPE_INT == cRetInfo. eKeyType )
		{
			nRetCode = atoi (cRetInfo. szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo ;
		strcpy( cRetInfo .szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetDebugJpegStatus", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_SetDebugJpegStatus HvXmlParse Fail\n");
			return E_FAIL ;
		}
 
			nRetCode = atoi (cRetInfo. szKeyValue);
		 
	}


	return nRetCode ==0 ?S_OK: E_FAIL;
}
HV_API_EX HRESULT CDECL HVAPI_StartCOMCheck(HVAPI_HANDLE_EX hHandle, INT nCOMNum, BOOL fEnable )// 启动设备串口检测功能
{
	if(hHandle == NULL )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}


	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		//执行命令
		CHAR szCmd [128]={0};
		sprintf( szCmd , "EnableCOMCheck,COMNum[%d],Enable[%d]", nCOMNum, fEnable );

		CHAR szRet [1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx( hHandle , szCmd , szRet , nRetLen , &nRetLen) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_EnableCOMCheck HVAPI_ExecCmdEx Fail\n");
			return E_FAIL ;
		}
		CXmlParseInfo cRetInfo ;
		strcpy( cRetInfo .szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "EnableCOMCheck", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_EnableCOMCheck HvXmlParse Fail\n");
			return E_FAIL ;
		}

 
		if ( XML_CMD_TYPE_INT == cRetInfo. eKeyType )
		{
			nRetCode = atoi (cRetInfo. szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		//执行命令
		CHAR szCmd [128]={0};
		if (fEnable)
		{
			sprintf( szCmd , "SetComTestEnable,COMNum[%d]", nCOMNum, fEnable );
			CHAR szRet [1024]={0};
			INT nRetLen = 1024;

			if ( S_OK != HVAPI_ExecCmdEx( hHandle , szCmd , szRet , nRetLen , &nRetLen) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_EnableCOMCheck HVAPI_ExecCmdEx Fail\n");
				return E_FAIL ;
			}
			CXmlParseInfo cRetInfo ;
			strcpy( cRetInfo .szKeyName, "RetCode" );

			if ( S_OK != HvXmlParseMercury( "SetComTestEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_EnableCOMCheck HvXmlParse Fail\n");
				return E_FAIL ;
			}
			nRetCode = atoi (cRetInfo. szKeyValue);
		}
		else
		{
			sprintf( szCmd , "SetComTestDisable,COMNum[%d]", nCOMNum, fEnable );
			CHAR szRet [1024]={0};
			INT nRetLen = 1024;

			if ( S_OK != HVAPI_ExecCmdEx( hHandle , szCmd , szRet , nRetLen , &nRetLen) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_EnableCOMCheck HVAPI_ExecCmdEx Fail\n");
				return E_FAIL ;
			}
			CXmlParseInfo cRetInfo ;
			strcpy( cRetInfo .szKeyName, "RetCode" );

			if ( S_OK != HvXmlParseMercury( "SetComTestDisable", szRet ,nRetLen , &cRetInfo, 1 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_EnableCOMCheck HvXmlParse Fail\n");
				return E_FAIL ;
			}
			nRetCode = atoi (cRetInfo. szKeyValue);
		}
			
		 
	}

	
	return nRetCode ==0 ?S_OK: E_FAIL;

}
HV_API_EX HRESULT CDECL HVAPI_StartCameraTest(HVAPI_HANDLE_EX hHandle) //   启动自动测试相机功能
{
	if(hHandle == NULL )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}
	//执行命令
	CHAR szCmd [128]={0};
	sprintf( szCmd , "AutoTestCamera");

	CHAR szRet [1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_AutoTestCamera HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		CXmlParseInfo cRetInfo ;
		strcpy( cRetInfo .szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "AutoTestCamera", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_AutoTestCamera HvXmlParse Fail\n");
			return E_FAIL ;
		}

 
		if ( XML_CMD_TYPE_INT == cRetInfo. eKeyType )
		{
			nRetCode = atoi (cRetInfo. szKeyValue);
		}
	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo ;
		strcpy( cRetInfo .szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "AutoTestCamera", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_AutoTestCamera HvXmlParse Fail\n");
			return E_FAIL ;
		}
 
			nRetCode = atoi (cRetInfo. szKeyValue);
		 
	}

	
	return nRetCode ==0 ?S_OK: E_FAIL;

}


HV_API_EX HRESULT CDECL HVAPI_SetSharpnessEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable)
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
	sprintf( szCmd , "SetSharpnessEnable,Enable[%d]" ,fEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetNTPEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetSharpnessEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetSharpnessEnable HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetSharpnessEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetSharpnessEnable HvXmlParse Fail\n");
			return E_FAIL;
		}


		nRetCode = atoi(cRetInfo.szKeyValue);

	}



	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}



HV_API_EX HRESULT CDECL HVAPI_ZoomDCIRIS(HVAPI_HANDLE_EX hHandle)
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

	sprintf( szCmd , "ZoomDCIRIS");

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_ZoomDCIRIS HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParse( "ZoomDCIRIS", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_ZoomDCIRIS HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParseMercury( "ZoomDCIRIS", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_ZoomDCIRIS HvXmlParse Fail\n");
			return E_FAIL;
		}

		nRetCode = atoi(cRetInfo.szKeyValue);

	}



	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}



HV_API_EX HRESULT CDECL HVAPI_ShrinkDCIRIS(HVAPI_HANDLE_EX hHandle)
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

	sprintf( szCmd , "ShrinkDCIRIS");

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_ShrinkDCIRIS HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParse( "ShrinkDCIRIS", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_ShrinkDCIRIS HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParseMercury( "ShrinkDCIRIS", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_ShrinkDCIRIS HvXmlParse Fail\n");
			return E_FAIL;
		}

		nRetCode = atoi(cRetInfo.szKeyValue);

	}



	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

 
HV_API_EX HRESULT CDECL HVAPI_SetEnRedLightEnable (HVAPI_HANDLE_EX hHandle, BOOL fEnable )
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
	sprintf( szCmd , "SetEnRedLightEnable,Enable[%d]" ,fEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetNTPEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetEnRedLightEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetEnRedLightEnable HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetEnRedLightEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetEnRedLightEnable HvXmlParse Fail\n");
			return E_FAIL;
		}


		nRetCode = atoi(cRetInfo.szKeyValue);

	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}


HV_API_EX HRESULT CDECL HVAPI_GetEnRedLightEnable (HVAPI_HANDLE_EX hHandle, BOOL* pfEnable )
{
	if(hHandle == NULL || NULL == pfEnable )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}

	CHAR szRet [1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetEnRedLightEnable" , szRet , nRetLen , &nRetLen ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetEnRedLightEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		CXmlParseInfo rgRetInfo [2];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );
		strcpy( rgRetInfo [1].szKeyName, "RetMsg" );

		if ( S_OK != HvXmlParse( "GetEnRedLightEnable", szRet ,nRetLen , rgRetInfo, 2 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetEnRedLightEnable HvXmlParse Fail\n");
			return E_FAIL ;
		}


		if ( XML_CMD_TYPE_INT == rgRetInfo[0]. eKeyType )
		{
			nRetCode = atoi (rgRetInfo[0]. szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[1]. eKeyType )
		{
			* pfEnable = atoi (rgRetInfo[1]. szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{

		CXmlParseInfo rgRetInfo [2];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );
		strcpy( rgRetInfo [1].szKeyName, "value" );

		if ( S_OK != HvXmlParseMercury( "GetEnRedLightEnable", szRet ,nRetLen , rgRetInfo, 2 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetEnRedLightEnable HvXmlParse Fail\n");
			return E_FAIL ;
		}


		nRetCode = atoi (rgRetInfo[0]. szKeyValue);

		* pfEnable = atoi (rgRetInfo[1]. szKeyValue);

	}



	return nRetCode ==0 ?S_OK: E_FAIL;
}



HV_API_EX HRESULT CDECL HVAPI_SetRedLightRect (HVAPI_HANDLE_EX hHandle, CHvPoint rgRedLightRect [],  INT nRedLightRectPointCount)
{

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

		const INT  MAX_REDLIGHTRECT_POINT_COUNT = 16;
		if ( nRedLightRectPointCount > MAX_REDLIGHTRECT_POINT_COUNT ||  nRedLightRectPointCount <= 0 )
		{
			return S_FALSE;
		}

		if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
		{
			//执行命令
			const INT MAX_CMD_LEN = 1024;
			CHAR szCmd[MAX_CMD_LEN]= {0};
			sprintf( szCmd ,"SetRedLightRect,PointCount[%d]",nRedLightRectPointCount );
			CHAR szTemp[512];
			for ( int i = 0 ; i < nRedLightRectPointCount ; i++ )
			{
				sprintf( szTemp , ",Point%02d_X[%d],Point%02d_Y[%d]" , i , rgRedLightRect[i].nX ,i, rgRedLightRect[i].nY);

				if ( strlen( szCmd ) + strlen(szTemp ) < MAX_CMD_LEN + 1 )
				{
					strcat( szCmd , szTemp );
				}

			}

			CHAR szRet[1024]={0};
			INT nRetLen = 1024;

			if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
			{
				WrightLogEx(pHHC->szIP , "HVAPI_SetRedLightRect HVAPI_ExecCmdEx Fail\n");
				return E_FAIL;
			}

			//解析命令返回
			CXmlParseInfo cRetInfo;
			strcpy( cRetInfo.szKeyName, "RetCode" );

			if ( S_OK != HvXmlParse( "SetLUT", szRet ,nRetLen , &cRetInfo, 1 ) )
			{
				WrightLogEx(pHHC->szIP , "HVAPI_SetRedLightRect HvXmlParse Fail\n");
				return E_FAIL;
			}
			INT nRetCode = -1;
			if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
			{
				nRetCode = atoi(cRetInfo.szKeyValue);
			}

			return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);

		}
		else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
		{
			//执行命令
			const INT MAX_CMD_LEN = 1024;
			CHAR szCmd[MAX_CMD_LEN]= {0};
			sprintf( szCmd ,"SetRedLightRect,PointCount[%d]",nRedLightRectPointCount );
			CHAR szTemp[512];
			for ( int i = 0 ; i < nRedLightRectPointCount ; i++ )
			{
				sprintf( szTemp , ",Point%02d_X[%d],Point%02d_Y[%d]" , i , rgRedLightRect[i].nX ,i, rgRedLightRect[i].nY);

				if ( strlen( szCmd ) + strlen(szTemp ) < MAX_CMD_LEN + 1 )
				{
					strcat( szCmd , szTemp );
				}

			}

			CHAR szRet[1024]={0};
			INT nRetLen = 1024;

			if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
			{
				WrightLogEx(pHHC->szIP , "HVAPI_SetRedLightRect HVAPI_ExecCmdEx Fail\n");
				return E_FAIL;
			}

			//解析命令返回
			CXmlParseInfo cRetInfo;
			strcpy( cRetInfo.szKeyName, "RetCode" );

			if ( S_OK != HvXmlParseMercury( "SetRedLightRect", szRet ,nRetLen , &cRetInfo, 1 ) )
			{
				WrightLogEx(pHHC->szIP , "HVAPI_SetRedLightRect HvXmlParse Fail\n");
				return E_FAIL;
			}
			INT nRetCode = -1;

			nRetCode = atoi(cRetInfo.szKeyValue);


			return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
		}

	}
	return E_FAIL;

}




HV_API_EX HRESULT CDECL HVAPI_GetContrast(HVAPI_HANDLE_EX hHandle, INT* pnContrast)
{
	{
		if(hHandle == NULL || NULL == pnContrast )
		{
			return E_FAIL ;
		}

		HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
		if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
			|| pHHC->dwOpenType != 1)
		{
			return E_FAIL ;
		}

		CHAR szRet [1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetContrast" , szRet , nRetLen , &nRetLen ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetContrast HVAPI_ExecCmdEx Fail\n");
			return E_FAIL ;
		}

		//解析命令返回
		INT nRetCode = -1;
		if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "RetMsg" );

			if ( S_OK != HvXmlParse( "GetContrast", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetContrast HvXmlParse Fail\n");
				return E_FAIL ;
			}


			if ( XML_CMD_TYPE_INT == rgRetInfo[0]. eKeyType )
			{
				nRetCode = atoi (rgRetInfo[0]. szKeyValue);
			}
			if ( XML_CMD_TYPE_INT == rgRetInfo[1]. eKeyType )
			{
				* pnContrast = atoi (rgRetInfo[1]. szKeyValue);
			}

		}
		else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "value" );

			if ( S_OK != HvXmlParseMercury( "GetContrast", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetContrast HvXmlParse Fail\n");
				return E_FAIL ;
			}


			nRetCode = atoi (rgRetInfo[0].szKeyValue);

			* pnContrast = atoi (rgRetInfo[1]. szKeyValue);

		}



		return nRetCode ==0 ?S_OK: E_FAIL;
	}
}
HV_API_EX HRESULT CDECL HVAPI_GetSaturation(HVAPI_HANDLE_EX hHandle,  INT* pnSaturation)
{
	{
		if(hHandle == NULL || NULL == pnSaturation )
		{
			return E_FAIL ;
		}

		HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
		if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
			|| pHHC->dwOpenType != 1)
		{
			return E_FAIL ;
		}

		CHAR szRet [1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetSaturation" , szRet , nRetLen , &nRetLen ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetSaturation HVAPI_ExecCmdEx Fail\n");
			return E_FAIL ;
		}

		//解析命令返回
		INT nRetCode = -1;
		if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "RetMsg" );

			if ( S_OK != HvXmlParse( "GetSaturation", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetSaturation HvXmlParse Fail\n");
				return E_FAIL ;
			}


			if ( XML_CMD_TYPE_INT == rgRetInfo[0]. eKeyType )
			{
				nRetCode = atoi (rgRetInfo[0]. szKeyValue);
			}
			if ( XML_CMD_TYPE_INT == rgRetInfo[1]. eKeyType )
			{
				* pnSaturation = atoi (rgRetInfo[1]. szKeyValue);
			}

		}
		else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "value" );

			if ( S_OK != HvXmlParseMercury( "GetSaturation", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetSaturation HvXmlParse Fail\n");
				return E_FAIL ;
			}


			nRetCode = atoi (rgRetInfo[0]. szKeyValue);

			* pnSaturation = atoi (rgRetInfo[1]. szKeyValue);

		}



		return nRetCode ==0 ?S_OK: E_FAIL;
	}
}
HV_API_EX HRESULT CDECL HVAPI_GetSharpness(HVAPI_HANDLE_EX hHandle,  INT* pnSharpness)
{
	{
		if(hHandle == NULL || NULL == pnSharpness )
		{
			return E_FAIL ;
		}

		HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
		if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
			|| pHHC->dwOpenType != 1)
		{
			return E_FAIL ;
		}

		CHAR szRet [1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetSharpness" , szRet , nRetLen , &nRetLen ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetSharpness HVAPI_ExecCmdEx Fail\n");
			return E_FAIL ;
		}

		//解析命令返回
		INT nRetCode = -1;
		if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "RetMsg" );

			if ( S_OK != HvXmlParse( "GetSharpness", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetSharpness HvXmlParse Fail\n");
				return E_FAIL ;
			}


			if ( XML_CMD_TYPE_INT == rgRetInfo[0]. eKeyType )
			{
				nRetCode = atoi (rgRetInfo[0]. szKeyValue);
			}
			if ( XML_CMD_TYPE_INT == rgRetInfo[1]. eKeyType )
			{
				* pnSharpness = atoi (rgRetInfo[1]. szKeyValue);
			}

		}
		else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "value" );

			if ( S_OK != HvXmlParseMercury( "GetSharpness", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetSharpness HvXmlParse Fail\n");
				return E_FAIL ;
			}


			nRetCode = atoi (rgRetInfo[0]. szKeyValue);

			* pnSharpness = atoi (rgRetInfo[1]. szKeyValue);

		}



		return nRetCode ==0 ?S_OK: E_FAIL;
	}
}
HV_API_EX HRESULT CDECL HVAPI_GetDCAperture(HVAPI_HANDLE_EX hHandle,  BOOL* pfEnable)
{
	{
		if(hHandle == NULL || NULL == pfEnable )
		{
			return E_FAIL ;
		}

		HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
		if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
			|| pHHC->dwOpenType != 1)
		{
			return E_FAIL ;
		}

		CHAR szRet [1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetDCAperture" , szRet , nRetLen , &nRetLen ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetDCAperture HVAPI_ExecCmdEx Fail\n");
			return E_FAIL ;
		}

		//解析命令返回
		INT nRetCode = -1;
		if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "RetMsg" );

			if ( S_OK != HvXmlParse( "GetDCAperture", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetDCAperture HvXmlParse Fail\n");
				return E_FAIL ;
			}


			if ( XML_CMD_TYPE_INT == rgRetInfo[0]. eKeyType )
			{
				nRetCode = atoi (rgRetInfo[0]. szKeyValue);
			}
			if ( XML_CMD_TYPE_INT == rgRetInfo[1]. eKeyType )
			{
				* pfEnable = atoi (rgRetInfo[1]. szKeyValue);
			}

		}
		else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "value" );

			if ( S_OK != HvXmlParseMercury( "GetDCAperture", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetDCAperture HvXmlParse Fail\n");
				return E_FAIL ;
			}


			nRetCode = atoi (rgRetInfo[0]. szKeyValue);

			* pfEnable = atoi (rgRetInfo[1]. szKeyValue);

		}



		return nRetCode ==0 ?S_OK: E_FAIL;
	}
}

HV_API_EX HRESULT CDECL HVAPI_GetWDRLevel(HVAPI_HANDLE_EX hHandle,  INT* pnWDRLevel)
{
	if(hHandle == NULL || NULL == pnWDRLevel )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}

	CHAR szRet [1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetWDRLevel" , szRet , nRetLen , &nRetLen ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetWDRLevel HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		CXmlParseInfo rgRetInfo [2];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );
		strcpy( rgRetInfo [1].szKeyName, "RetMsg" );

		if ( S_OK != HvXmlParse( "GetWDRLevel", szRet ,nRetLen , rgRetInfo, 2 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetWDRLevel HvXmlParse Fail\n");
			return E_FAIL ;
		}


		if ( XML_CMD_TYPE_INT == rgRetInfo[0]. eKeyType )
		{
			nRetCode = atoi (rgRetInfo[0]. szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[1]. eKeyType )
		{
			* pnWDRLevel = atoi (rgRetInfo[1]. szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{

		CXmlParseInfo rgRetInfo [2];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );
		strcpy( rgRetInfo [1].szKeyName, "value" );

		if ( S_OK != HvXmlParseMercury( "GetWDRLevel", szRet ,nRetLen , rgRetInfo, 2 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetWDRLevel HvXmlParse Fail\n");
			return E_FAIL ;
		}


		nRetCode = atoi (rgRetInfo[0]. szKeyValue);

		* pnWDRLevel = atoi (rgRetInfo[1]. szKeyValue);

	}



	return nRetCode ==0 ?S_OK: E_FAIL;
}


HV_API_EX HRESULT CDECL HVAPI_GetAWBEnable(HVAPI_HANDLE_EX hHandle,  BOOL* pfEnable)
{
	{
		if(hHandle == NULL || NULL == pfEnable )
		{
			return E_FAIL ;
		}

		HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
		if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
			|| pHHC->dwOpenType != 1)
		{
			return E_FAIL ;
		}

		CHAR szRet [1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetAWBEnable" , szRet , nRetLen , &nRetLen ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetAWBEnable HVAPI_ExecCmdEx Fail\n");
			return E_FAIL ;
		}

		//解析命令返回
		INT nRetCode = -1;
		if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "RetMsg" );

			if ( S_OK != HvXmlParse( "GetAWBEnable", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetAWBEnable HvXmlParse Fail\n");
				return E_FAIL ;
			}


			if ( XML_CMD_TYPE_INT == rgRetInfo[0]. eKeyType )
			{
				nRetCode = atoi (rgRetInfo[0]. szKeyValue);
			}
			if ( XML_CMD_TYPE_INT == rgRetInfo[1]. eKeyType )
			{
				* pfEnable = atoi (rgRetInfo[1]. szKeyValue);
			}

		}
		else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "value" );

			if ( S_OK != HvXmlParseMercury( "GetAWBEnable", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetAWBEnable HvXmlParse Fail\n");
				return E_FAIL ;
			}


			nRetCode = atoi (rgRetInfo[0]. szKeyValue);

			* pfEnable = atoi (rgRetInfo[1]. szKeyValue);

		}



		return nRetCode ==0 ?S_OK: E_FAIL;
	}
}
HV_API_EX HRESULT CDECL HVAPI_GetAGCEnable(HVAPI_HANDLE_EX hHandle,  BOOL* pfEnable)
{
	{
		if(hHandle == NULL || NULL == pfEnable )
		{
			return E_FAIL ;
		}

		HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
		if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
			|| pHHC->dwOpenType != 1)
		{
			return E_FAIL ;
		}

		CHAR szRet [1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetAGCEnable" , szRet , nRetLen , &nRetLen ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetAGCEnable HVAPI_ExecCmdEx Fail\n");
			return E_FAIL ;
		}

		//解析命令返回
		INT nRetCode = -1;
		if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "RetMsg" );

			if ( S_OK != HvXmlParse( "GetAGCEnable", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetAGCEnable HvXmlParse Fail\n");
				return E_FAIL ;
			}


			if ( XML_CMD_TYPE_INT == rgRetInfo[0]. eKeyType )
			{
				nRetCode = atoi (rgRetInfo[0]. szKeyValue);
			}
			if ( XML_CMD_TYPE_INT == rgRetInfo[1]. eKeyType )
			{
				* pfEnable = atoi (rgRetInfo[1]. szKeyValue);
			}

		}
		else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "value" );

			if ( S_OK != HvXmlParseMercury( "GetAGCEnable", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetAGCEnable HvXmlParse Fail\n");
				return E_FAIL ;
			}


			nRetCode = atoi (rgRetInfo[0]. szKeyValue);

			* pfEnable = atoi (rgRetInfo[1]. szKeyValue);

		}



		return nRetCode ==0 ?S_OK: E_FAIL;
	}
}

HV_API_EX HRESULT CDECL HVAPI_GetEnableGrayImage(HVAPI_HANDLE_EX hHandle,   BOOL* pfEnable)
{
	{
		if(hHandle == NULL || NULL == pfEnable )
		{
			return E_FAIL ;
		}

		HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
		if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
			|| pHHC->dwOpenType != 1)
		{
			return E_FAIL ;
		}

		CHAR szRet [1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetEnableGrayImage" , szRet , nRetLen , &nRetLen ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetEnableGrayImage HVAPI_ExecCmdEx Fail\n");
			return E_FAIL ;
		}

		//解析命令返回
		INT nRetCode = -1;
		if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "RetMsg" );

			if ( S_OK != HvXmlParse( "GetEnableGrayImage", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetEnableGrayImage HvXmlParse Fail\n");
				return E_FAIL ;
			}


			if ( XML_CMD_TYPE_INT == rgRetInfo[0]. eKeyType )
			{
				nRetCode = atoi (rgRetInfo[0]. szKeyValue);
			}
			if ( XML_CMD_TYPE_INT == rgRetInfo[1]. eKeyType )
			{
				* pfEnable = atoi (rgRetInfo[1]. szKeyValue);
			}

		}
		else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "value" );

			if ( S_OK != HvXmlParseMercury( "GetEnableGrayImage", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetEnableGrayImage HvXmlParse Fail\n");
				return E_FAIL ;
			}


			nRetCode = atoi (rgRetInfo[0]. szKeyValue);

			* pfEnable = atoi (rgRetInfo[1]. szKeyValue);

		}



		return nRetCode ==0 ?S_OK: E_FAIL;
	}
}
HV_API_EX HRESULT CDECL HVAPI_GetSyncPower(HVAPI_HANDLE_EX hHandle,  INT* pnMode, DWORD32* pdw32Delay)
{
	if(hHandle == NULL || NULL == pnMode || NULL == pdw32Delay )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}

	CHAR szRet [1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetSyncPower" , szRet , nRetLen , &nRetLen ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetSyncPower HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{

		CXmlParseInfo rgRetInfo [3];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );
		//一维数组，RetMsg被拆分为"Mode", "DelayMS"
		strcpy( rgRetInfo [1].szKeyName, "Mode" );
		strcpy( rgRetInfo [2].szKeyName, "DelayMS" );

		if ( S_OK != HvXmlParseMercury( "GetSyncPower", szRet ,nRetLen , rgRetInfo, 3 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetSyncPower HvXmlParse Fail\n");
			return E_FAIL ;
		}


		nRetCode = atoi (rgRetInfo[0]. szKeyValue);

		* pnMode = atoi (rgRetInfo[1]. szKeyValue);
		* pdw32Delay = atoi (rgRetInfo[2]. szKeyValue);

	}



	return nRetCode ==0 ?S_OK: E_FAIL;
}

HV_API_EX HRESULT CDECL HVAPI_GetDeNoiseMode(HVAPI_HANDLE_EX hHandle, INT* pnMode)
{
	if(hHandle == NULL || NULL == pnMode )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}

	CHAR szRet [1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetDeNoiseMode" , szRet , nRetLen , &nRetLen ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetDeNoiseMode HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		CXmlParseInfo rgRetInfo [2];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );
		strcpy( rgRetInfo [1].szKeyName, "RetMsg" );

		if ( S_OK != HvXmlParse( "GetDeNoiseMode", szRet ,nRetLen , rgRetInfo, 2 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetDeNoiseMode HvXmlParse Fail\n");
			return E_FAIL ;
		}


		if ( XML_CMD_TYPE_INT == rgRetInfo[0]. eKeyType )
		{
			nRetCode = atoi (rgRetInfo[0]. szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[1]. eKeyType )
		{
			* pnMode = atoi (rgRetInfo[1]. szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{

		CXmlParseInfo rgRetInfo [2];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );
		strcpy( rgRetInfo [1].szKeyName, "value" );

		if ( S_OK != HvXmlParseMercury( "GetDeNoiseMode", szRet ,nRetLen , rgRetInfo, 2 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetDeNoiseMode HvXmlParse Fail\n");
			return E_FAIL ;
		}


		nRetCode = atoi (rgRetInfo[0]. szKeyValue);

		* pnMode = atoi (rgRetInfo[1]. szKeyValue);

	}



	return nRetCode ==0 ?S_OK: E_FAIL;
}


HV_API_EX HRESULT CDECL HVAPI_GetDeNoiseLevel(HVAPI_HANDLE_EX hHandle, INT* pnlevel)
{
	if(hHandle == NULL || NULL == pnlevel )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}

	CHAR szRet [1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetDeNoiseLevel" , szRet , nRetLen , &nRetLen ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetDeNoiseLevel HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		CXmlParseInfo rgRetInfo [2];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );
		strcpy( rgRetInfo [1].szKeyName, "RetMsg" );

		if ( S_OK != HvXmlParse( "GetDeNoiseLevel", szRet ,nRetLen , rgRetInfo, 2 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetDeNoiseLevel HvXmlParse Fail\n");
			return E_FAIL ;
		}


		if ( XML_CMD_TYPE_INT == rgRetInfo[0]. eKeyType )
		{
			nRetCode = atoi (rgRetInfo[0]. szKeyValue);
		}
		if ( XML_CMD_TYPE_INT == rgRetInfo[1]. eKeyType )
		{
			* pnlevel = atoi (rgRetInfo[1]. szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{

		CXmlParseInfo rgRetInfo [2];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );
		strcpy( rgRetInfo [1].szKeyName, "value" );

		if ( S_OK != HvXmlParseMercury( "GetDeNoiseLevel", szRet ,nRetLen , rgRetInfo, 2 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetDeNoiseLevel HvXmlParse Fail\n");
			return E_FAIL ;
		}


		nRetCode = atoi (rgRetInfo[0]. szKeyValue);

		* pnlevel = atoi (rgRetInfo[1]. szKeyValue);

	}



	return nRetCode ==0 ?S_OK: E_FAIL;
}

HV_API_EX HRESULT CDECL HVAPI_GetManualRGB(HVAPI_HANDLE_EX hHandle, INT* pnGainR, INT* pnGainG, INT* pnGainB  )
{
 

		if(hHandle == NULL || NULL == pnGainR || NULL == pnGainG || NULL == pnGainB   )
		{
			return E_FAIL ;
		}

		HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
		if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
			|| pHHC->dwOpenType != 1)
		{
			return E_FAIL ;
		}

		CHAR szRet [1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetRgbGain" , szRet , nRetLen , &nRetLen ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetManualRGB HVAPI_ExecCmdEx Fail\n");
			return E_FAIL ;
		}

		//解析命令返回
		INT nRetCode = -1;
		if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
		{

			CXmlParseInfo rgRetInfo [4];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "GainR" );
			strcpy( rgRetInfo [2].szKeyName, "GainG" );
			strcpy( rgRetInfo [3].szKeyName, "GainB" );

			if ( S_OK != HvXmlParseMercury( "GetRgbGain", szRet ,nRetLen , rgRetInfo, 4 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetManualRGB HvXmlParse Fail\n");
				return E_FAIL ;
			}


			nRetCode = atoi (rgRetInfo[0]. szKeyValue);
			* pnGainR = atoi (rgRetInfo[1]. szKeyValue);
			* pnGainG = atoi (rgRetInfo[2]. szKeyValue);
			* pnGainB = atoi (rgRetInfo[3]. szKeyValue);

		}

		return nRetCode ==0 ?S_OK: E_FAIL;
}

HV_API_EX HRESULT CDECL HVAPI_GetAGCParam(HVAPI_HANDLE_EX hHandle,INT* pnShutterMin , INT* pnShutterMax ,
										  INT* pnGainMin , INT* pnGainMax )
{
	if(hHandle == NULL || NULL == pnShutterMin || NULL == pnShutterMax || NULL == pnGainMin || NULL == pnGainMax  )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}

	CHAR szRet [1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetAGCParam" , szRet , nRetLen , &nRetLen ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetAGCParam HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{

		CXmlParseInfo rgRetInfo [5];
		strcpy( rgRetInfo [0].szKeyName, "RetCode" );
		strcpy( rgRetInfo [1].szKeyName, "ShutterMin" );
		strcpy( rgRetInfo [2].szKeyName, "ShutterMax" );
		strcpy( rgRetInfo [3].szKeyName, "GainMin" );
		strcpy( rgRetInfo [4].szKeyName, "GainMax" );

		if ( S_OK != HvXmlParseMercury( "GetAGCParam", szRet ,nRetLen , rgRetInfo, 5 ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetAGCParam HvXmlParse Fail\n");
			return E_FAIL ;
		}


		nRetCode = atoi (rgRetInfo[0]. szKeyValue);

		* pnShutterMin = atoi (rgRetInfo[1]. szKeyValue);
		* pnShutterMax = atoi (rgRetInfo[2]. szKeyValue);
		* pnGainMin = atoi (rgRetInfo[3]. szKeyValue);
		* pnGainMax = atoi (rgRetInfo[4]. szKeyValue);
	}

	return nRetCode ==0 ?S_OK: E_FAIL;
}

HV_API_EX HRESULT CDECL HVAPI_SetAutoJpegCompressEnable(HVAPI_HANDLE_EX hHandle,  BOOL fEnable)
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
	sprintf( szCmd , "SetAutoJpegCompressEnable,Enable[%d]" ,fEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetAutoJpegCompressEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParse( "SetAutoJpegCompressEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAutoJpegCompressEnable HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );

		if ( S_OK != HvXmlParseMercury( "SetAutoJpegCompressEnable", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAutoJpegCompressEnable HvXmlParse Fail\n");
			return E_FAIL;
		}


		nRetCode = atoi(cRetInfo.szKeyValue);

	}



	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetTraceRank(HVAPI_HANDLE_EX hHandle, INT nlevel)
{
	return Default_Setter_Int_ForMercuryProtocol(hHandle, "SetTraceRank", nlevel);
}

HV_API_EX HRESULT CDECL HVAPI_GetTraceRank(HVAPI_HANDLE_EX hHandle, INT* pnlevel)
{
	return Default_Getter_Int_ForMercuryProtocol(hHandle, "GetTraceRank", pnlevel);
}

HV_API_EX HRESULT CDECL HVAPI_GetWDREnable(HVAPI_HANDLE_EX hHandle, BOOL* pfEnable  )
{
	return Default_Getter_Bool_ForMercuryProtocol(hHandle, "GetWDREnable", pfEnable);
}
HV_API_EX HRESULT CDECL HVAPI_GetBLCEnable(HVAPI_HANDLE_EX hHandle, BOOL* pfEnable  )
{
	return Default_Getter_Bool_ForMercuryProtocol(hHandle, "GetBLCEnable", pfEnable);
}
HV_API_EX HRESULT CDECL HVAPI_GetBLCLevel(HVAPI_HANDLE_EX hHandle, INT* pnLevel )
{
	return Default_Getter_Int_ForMercuryProtocol(hHandle, "GetBLCLevel", pnLevel);
}
HV_API_EX HRESULT CDECL HVAPI_GetDREEnable(HVAPI_HANDLE_EX hHandle, BOOL* pfEnable )
{
	return Default_Getter_Bool_ForMercuryProtocol(hHandle, "GetDREEnable", pfEnable);
}
HV_API_EX HRESULT CDECL HVAPI_GetDREMode(HVAPI_HANDLE_EX hHandle, INT* pnMode )
{
	return Default_Getter_Int_ForMercuryProtocol(hHandle, "GetDREMode", pnMode);
}
HV_API_EX HRESULT CDECL HVAPI_GetDRELevel(HVAPI_HANDLE_EX hHandle, INT* pnLevel )
{
	return Default_Getter_Int_ForMercuryProtocol(hHandle, "GetDRELevel", pnLevel);
}
HV_API_EX HRESULT CDECL HVAPI_GetDeNoiseEnable(HVAPI_HANDLE_EX hHandle, BOOL* pfEnable )
{
	return Default_Getter_Bool_ForMercuryProtocol(hHandle, "GetDeNoiseEnable", pfEnable);
}
HV_API_EX HRESULT CDECL HVAPI_GetSensorWDREnable(HVAPI_HANDLE_EX hHandle, BOOL* pfEnable )
{
	return Default_Getter_Bool_ForMercuryProtocol(hHandle, "GetSensorWDR", pfEnable);
}
HV_API_EX HRESULT CDECL HVAPI_GetSharpnessEnable(HVAPI_HANDLE_EX hHandle, BOOL* pfEnable)
{
	return Default_Getter_Bool_ForMercuryProtocol(hHandle, "GetSharpnessEnable", pfEnable);
}
HV_API_EX HRESULT CDECL HVAPI_GetAutoJpegCompressEnable(HVAPI_HANDLE_EX hHandle,  BOOL* pfEnable)
{
	return Default_Getter_Bool_ForMercuryProtocol(hHandle, "GetAutoJpegCompressEnable", pfEnable);
}
HV_API_EX HRESULT CDECL HVAPI_GetManualShutter(HVAPI_HANDLE_EX hHandle, INT* pnShutter )
{
	return Default_Getter_Int_ForMercuryProtocol(hHandle, "GetShutter", pnShutter);
}
HV_API_EX HRESULT CDECL HVAPI_GetManualGain(HVAPI_HANDLE_EX hHandle, INT* pnGain )
{
	return Default_Getter_Int_ForMercuryProtocol(hHandle, "GetGain", pnGain);
}
HV_API_EX HRESULT CDECL HVAPI_GetAWBMode(HVAPI_HANDLE_EX hHandle, INT* pnMode )
{
	return Default_Getter_Int_ForMercuryProtocol(hHandle, "GetAWBMode", pnMode);
}
HV_API_EX HRESULT CDECL HVAPI_GetAGCLightBaseLine(HVAPI_HANDLE_EX hHandle, INT* pnLightBaseLine )
{
	return Default_Getter_Int_ForMercuryProtocol(hHandle, "GetAgcLightBaseline", pnLightBaseLine);
}

HV_API_EX HRESULT CDECL HVAPI_SetCtrlCplEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable )
{
	return Default_Setter_Bool_ForMercuryProtocol(hHandle, "SetCtrlCpl", fEnable);
}

HV_API_EX HRESULT CDECL HVAPI_SetAutoJpegCompressParam (HVAPI_HANDLE_EX hHandle, INT nMaxCompressRate, INT nMinCompressRate, INT nImageSize)
{
	if(hHandle == NULL )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}
	//执行命令
	CHAR szCmd [128]={0};
	sprintf( szCmd , "SetAutoJpegCompressParam,MaxCompressRate[%d],MinCompressRate[%d],ImageSize[%d]" , nMaxCompressRate, nMinCompressRate , nImageSize );

	CHAR szRet [1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_SetAutoJpegCompressParam HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回
	INT nRetCode = -1;


	CXmlParseInfo cRetInfo ;
	strcpy( cRetInfo .szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( "SetAutoJpegCompressParam", szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_SetAutoJpegCompressParam HvXmlParse Fail\n");
		return E_FAIL ;
	}


	nRetCode = atoi (cRetInfo. szKeyValue);
	return nRetCode ==0 ?S_OK: E_FAIL;

}

HV_API_EX HRESULT CDECL HVAPI_GetAutoJpegCompressParam (HVAPI_HANDLE_EX hHandle, INT* pnMaxCompressRate, INT* pnMinCompressRate, INT* pnImageSize)
{
		if(hHandle == NULL || NULL == pnMaxCompressRate || NULL == pnMinCompressRate || NULL == pnImageSize   )
		{
			return E_FAIL ;
		}

		HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
		if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
			|| pHHC->dwOpenType != 1)
		{
			return E_FAIL ;
		}

		CHAR szRet [1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetAutoJpegCompressParam" , szRet , nRetLen , &nRetLen ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetAutoJpegCompressParam HVAPI_ExecCmdEx Fail\n");
			return E_FAIL ;
		}

		//解析命令返回
		INT nRetCode = -1;
		if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
		{

			CXmlParseInfo rgRetInfo [4];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "MaxCompressRate" );
			strcpy( rgRetInfo [2].szKeyName, "MinCompressRate" );
			strcpy( rgRetInfo [3].szKeyName, "ImageSize" );

			if ( S_OK != HvXmlParseMercury( "GetAutoJpegCompressParam", szRet ,nRetLen , rgRetInfo, 4 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetAutoJpegCompressParam HvXmlParse Fail\n");
				return E_FAIL ;
			}


			nRetCode = atoi (rgRetInfo[0]. szKeyValue);
			* pnMaxCompressRate = atoi (rgRetInfo[1]. szKeyValue);
			* pnMinCompressRate = atoi (rgRetInfo[2]. szKeyValue);
			* pnImageSize = atoi (rgRetInfo[3]. szKeyValue);

		}

		return nRetCode ==0 ?S_OK: E_FAIL;
}


HV_API_EX HRESULT CDECL HVAPI_SetImageEnhancementEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable)
{
	return Default_Setter_Bool_ForMercuryProtocol(hHandle, "SetImageEnhancementEnable", fEnable);
}
HV_API_EX HRESULT CDECL HVAPI_GetImageEnhancementEnable(HVAPI_HANDLE_EX hHandle, BOOL* pfEnable)
{
	return Default_Getter_Bool_ForMercuryProtocol(hHandle, "GetImageEnhancementEnable", pfEnable);
}

HV_API_EX HRESULT CDECL HVAPI_GetBlackBoxMessage(HVAPI_HANDLE_EX hHandle, CHAR* szBlackBoxMessage, INT* pnMessageLen)
{
	if(hHandle == NULL || NULL == szBlackBoxMessage || NULL == pnMessageLen)
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	CHAR szRet [17*1024]={0};
	INT nRetLen = 17*1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetBlackBoxMessage" , szRet , nRetLen , &nRetLen ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetBlackBoxMessage  HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回


	INT nRetCode = -1;

	CXmlParseInfo rgRetInfo [1];
	strcpy( rgRetInfo [0].szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( "GetBlackBoxMessage", szRet ,nRetLen , rgRetInfo, 1 ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetBlackBoxMessage  HvXmlParse Fail\n");
		return E_FAIL ;
	}


	nRetCode = atoi (rgRetInfo[0]. szKeyValue);


	if ( S_OK != HvParseXmlCmdRespRetcode2Adv(szRet,"GetBlackBoxMessage", "RetMsg", szBlackBoxMessage, pnMessageLen))
	{
		nRetCode = -1;
	}
 

	return nRetCode ==0 ?S_OK: E_FAIL;
}

HV_API_EX HRESULT CDECL HVAPI_ReadFPGA(HVAPI_HANDLE_EX hHandle, INT nFPGAAddress, INT* pnFPGAValue)
{
	if(hHandle == NULL || NULL == pnFPGAValue )
	{
		return E_FAIL ;
	}


	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}

	CHAR szRet [1024]={0};
	INT nRetLen = 1024;

	CHAR szCmdName[128]="ReadFPGA";
	CHAR szCmd[128];
	sprintf( szCmd , "%s,FPGAAddress[%d]" ,szCmdName, nFPGAAddress );
	if ( S_OK != HVAPI_ExecCmdEx( hHandle , szCmd, szRet , nRetLen , &nRetLen ) )
	{
		CHAR szTemp[128]={0};
		sprintf(szTemp, "%s HVAPI_ExecCmdEx Fail\n", szCmdName);
		WrightLogEx(pHHC->szIP , szTemp);
		return E_FAIL ;
	}

	//解析命令返回
	INT nRetCode = -1;


	CXmlParseInfo rgRetInfo [2];
	strcpy( rgRetInfo [0].szKeyName, "RetCode" );
	CHAR szFirstParamName[128];
	if (S_OK != HvGetFirstParamNameFromXmlCmdAppendInfoMap(szCmdName, szFirstParamName, 128))
	{
		return E_FAIL;
	}
	strcpy( rgRetInfo [1].szKeyName, szFirstParamName );//TODO(liyh) 比较好的做法，可以查询全局命令map中，对应命令的回传参数，可以通用


	if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , rgRetInfo, 2 ) )
	{
		CHAR szTemp[128]={0};
		sprintf(szTemp, "%s HvXmlParse Fail\n", szCmdName);
		WrightLogEx(pHHC->szIP , szTemp);

		return E_FAIL ;
	}


	nRetCode = atoi (rgRetInfo[0]. szKeyValue);
	* pnFPGAValue = atoi (rgRetInfo[1]. szKeyValue);


	return nRetCode ==0 ?S_OK: E_FAIL;
	 
}

HV_API_EX HRESULT CDECL HVAPI_WriteFPGA(HVAPI_HANDLE_EX hHandle, INT nFPGAAddress, INT nFPGAValue)
{
	if(hHandle == NULL )
	{
		return E_FAIL ;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
	if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL ;
	}
	//执行命令
	CHAR szCmd [128]={0};
	sprintf( szCmd , "WriteFPGA,FPGAAddress[%d],FPGAValue[%d]" , nFPGAAddress, nFPGAValue );

	CHAR szRet [1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_WriteFPGA HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回
	INT nRetCode = -1;


	CXmlParseInfo cRetInfo ;
	strcpy( cRetInfo .szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( "WriteFPGA", szRet ,nRetLen , &cRetInfo, 1 ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_WriteFPGA HvXmlParse Fail\n");
		return E_FAIL ;
	}


	nRetCode = atoi (cRetInfo. szKeyValue);

	return nRetCode ==0 ?S_OK: E_FAIL;

}
HV_API_EX HRESULT CDECL HVAPI_SetEnRedLightThreshold(HVAPI_HANDLE_EX hHandle, INT nValue)
{
	return Default_Setter_Int_ForMercuryProtocol(hHandle, "SetEnRedLightThreshold", nValue);
}

HV_API_EX HRESULT CDECL HVAPI_SetEdgeEnhance(HVAPI_HANDLE_EX hHandle, INT nValue)
{
	return Default_Setter_Int_ForMercuryProtocol(hHandle, "SetEdgeEnhance", nValue);
}

HV_API_EX HRESULT CDECL HVAPI_GetRunStatusString(HVAPI_HANDLE_EX hHandle, CHAR* szRunStatusString, INT* pnRunStatusStringLen  )
{
	if(hHandle == NULL || NULL == szRunStatusString || NULL == pnRunStatusStringLen)
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	CHAR szRet [1*1024]={0};
	INT nRetLen = 1*1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetRunStatus" , szRet , nRetLen , &nRetLen ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetRunStatus  HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回


	INT nRetCode = -1;

	CXmlParseInfo rgRetInfo [1];
	strcpy( rgRetInfo [0].szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( "GetRunStatus", szRet ,nRetLen , rgRetInfo, 1 ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetRunStatus  HvXmlParse Fail\n");
		return E_FAIL ;
	}


	nRetCode = atoi (rgRetInfo[0]. szKeyValue);


	if ( S_OK != HvParseXmlCmdRespRetcode2Adv(szRet,"GetRunStatus", "RunStatus", szRunStatusString, pnRunStatusStringLen))
	{
		nRetCode = -1;
	}


	return nRetCode ==0 ?S_OK: E_FAIL;
}




HV_API_EX HRESULT CDECL HVAPI_GetCameraWorkState(HVAPI_HANDLE_EX hHandle, CHAR* szStatus, INT* pnStatusStringLen  )
{
	if(hHandle == NULL || NULL == szStatus || NULL == pnStatusStringLen)
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	CHAR szRet [1*1024]={0};
	INT nRetLen = 1*1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetCameraWorkState" , szRet , nRetLen , &nRetLen ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetCameraWorkState  HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回


	INT nRetCode = -1;

	CXmlParseInfo rgRetInfo [1];
	strcpy( rgRetInfo [0].szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( "GetCameraWorkState", szRet ,nRetLen , rgRetInfo, 1 ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetCameraWorkState  HvXmlParse Fail\n");
		return E_FAIL ;
	}


	nRetCode = atoi (rgRetInfo[0]. szKeyValue);


	if ( S_OK != HvParseXmlCmdRespRetcode2Adv(szRet,"GetCameraWorkState", "RunStatus", szStatus, pnStatusStringLen))
	{
		nRetCode = -1;
	}


	return nRetCode ==0 ?S_OK: E_FAIL;
}



HV_API_EX HRESULT CDECL HVAPI_GetHDDStatus(HVAPI_HANDLE_EX hHandle, CHAR* szStatus, INT* pnStatusStringLen  )
{
	if(hHandle == NULL || NULL == szStatus || NULL == pnStatusStringLen)
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	CHAR szRet [17*1024]={0};
	INT nRetLen = 17*1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , "HDDStatus" , szRet , nRetLen , &nRetLen ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_HDDStatus  HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回


	INT nRetCode = -1;

	CXmlParseInfo rgRetInfo [1];
	strcpy( rgRetInfo [0].szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( "HDDStatus", szRet ,nRetLen , rgRetInfo, 1 ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_HDDStatus  HvXmlParse Fail\n");
		return E_FAIL ;
	}


	nRetCode = atoi (rgRetInfo[0]. szKeyValue);


	if ( S_OK != HvParseXmlCmdRespRetcode2Adv(szRet,"HDDStatus", "RetMsg", szStatus, pnStatusStringLen))
	{
		nRetCode = -1;
	}


	return nRetCode ==0 ?S_OK: E_FAIL;
}

HV_API_EX HRESULT CDECL HVAPI_SetTGIO(HVAPI_HANDLE_EX hHandle, INT nValue  )
{
	return Default_Setter_Int_ForMercuryProtocol(hHandle, "SetTGIO", nValue);
}


HV_API_EX HRESULT CDECL HVAPI_GetTGIO(HVAPI_HANDLE_EX hHandle, INT* pnValue  )
{
	return Default_Getter_Int_ForMercuryProtocol(hHandle, "GetTGIO", pnValue);
}


HV_API_EX HRESULT CDECL HVAPI_SetF1IO(HVAPI_HANDLE_EX hHandle, INT nPolarity, INT nType)
{
	INT rgValue[2];
	rgValue[0]=nPolarity;
	rgValue[1]=nType;

	return Default_Setter_INTARRAY1D_ForMercuryProtocol(hHandle, "SetF1IO" ,rgValue, 2);
}


HV_API_EX HRESULT CDECL HVAPI_GetF1IO(HVAPI_HANDLE_EX hHandle, INT* pnPolarity, INT* pnType)
{
	if (NULL == pnPolarity || NULL == pnType)
	{
		return E_FAIL;
	}
	INT rgValue[2];
	HRESULT ret=Default_Getter_INTARRAY1D_ForMercuryProtocol(hHandle, "GetF1IO", rgValue, 2);
	*pnPolarity = rgValue[0];
	*pnType = rgValue[1];
	return ret;
}


HV_API_EX HRESULT CDECL HVAPI_SetEXPIO(HVAPI_HANDLE_EX hHandle, INT nPolarity, INT nType)
{
	INT rgValue[2];
	rgValue[0]=nPolarity;
	rgValue[1]=nType;

	return Default_Setter_INTARRAY1D_ForMercuryProtocol(hHandle, "SetEXPIO" ,rgValue, 2);

}

HV_API_EX HRESULT CDECL HVAPI_GetEXPIO(HVAPI_HANDLE_EX hHandle, INT* pnPolarity, INT* pnType)
{
	if (NULL == pnPolarity || NULL == pnType)
	{
		return E_FAIL;
	}
	INT rgValue[2];
	HRESULT ret=Default_Getter_INTARRAY1D_ForMercuryProtocol(hHandle, "GetEXPIO", rgValue, 2);
	*pnPolarity = rgValue[0];
	*pnType = rgValue[1];
	return ret;
}


HV_API_EX HRESULT CDECL HVAPI_SetALMIO(HVAPI_HANDLE_EX hHandle, INT nPolarity, INT nType)
{
	INT rgValue[2];
	rgValue[0]=nPolarity;
	rgValue[1]=nType;

	return Default_Setter_INTARRAY1D_ForMercuryProtocol(hHandle, "SetALMIO" ,rgValue, 2);
}

HV_API_EX HRESULT CDECL HVAPI_GetALMIO(HVAPI_HANDLE_EX hHandle, INT* pnPolarity, INT* pnType)
{
	if (NULL == pnPolarity || NULL == pnType)
	{
		return E_FAIL;
	}
	INT rgValue[2];
	HRESULT ret=Default_Getter_INTARRAY1D_ForMercuryProtocol(hHandle, "GetALMIO", rgValue, 2);
	*pnPolarity = rgValue[0];
	*pnType = rgValue[1];
	return ret;
}

HV_API_EX HRESULT CDECL HVAPI_SetMJPEGRect(HVAPI_HANDLE_EX hHandle, CHvPoint PosTopLeft, CHvPoint PosLowerRight)
{
	INT rgValue[4];
	rgValue[0] = PosTopLeft.nX;
	rgValue[1] = PosTopLeft.nY;
	rgValue[2] = PosLowerRight.nX;
	rgValue[3] = PosLowerRight.nY;
	return Default_Setter_INTARRAY1D_ForMercuryProtocol(hHandle, "SetMJPEGRect" ,rgValue, 4);
}

HV_API_EX HRESULT CDECL HVAPI_SetColorGradation(HVAPI_HANDLE_EX hHandle, INT nValue)
{
	return Default_Setter_Int_ForMercuryProtocol(hHandle, "SetColorGradation", nValue);
}

HV_API_EX HRESULT CDECL HVAPI_SetVedioRequestControl(HVAPI_HANDLE_EX hHandle, BOOL fEnable, DWORD32 dwIP, INT nPort)
{
	INT rgValue[3];
	rgValue[0] = fEnable;
	rgValue[1] = dwIP;
	rgValue[2] = nPort;

	return Default_Setter_INTARRAY1D_ForMercuryProtocol(hHandle, "SetVedioRequestControl" ,rgValue, 3); 

}


HV_API_EX HRESULT CDECL HVAPI_SetAEScene(HVAPI_HANDLE_EX hHandle,  INT nMode)
{
	return Default_Setter_Int_ForMercuryProtocol(hHandle, "SetAEScene", nMode);
}


HV_API_EX HRESULT CDECL HVAPI_GetAEScene(HVAPI_HANDLE_EX hHandle,  INT* pnMode)
{
	return Default_Getter_Int_ForMercuryProtocol(hHandle, "GetAEScene", pnMode);
}

HV_API_EX HRESULT CDECL HVAPI_GetCustomizedDevName(HVAPI_HANDLE_EX hHandle, CHAR* pszDevName, INT* pnDevNameLen)
{
	if(hHandle == NULL || NULL == pszDevName || NULL == pnDevNameLen)
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	CHAR szRet [1*1024]={0};
	INT nRetLen = 1*1024;

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetCustomizedDevName" , szRet , nRetLen , &nRetLen ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetCustomizedDevName  HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回


	INT nRetCode = -1;

	CXmlParseInfo rgRetInfo [1];
	strcpy( rgRetInfo [0].szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( "GetCustomizedDevName", szRet ,nRetLen , rgRetInfo, 1 ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetCustomizedDevName  HvXmlParse Fail\n");
		return E_FAIL ;
	}


	nRetCode = atoi (rgRetInfo[0]. szKeyValue);


	if ( S_OK != HvParseXmlCmdRespRetcode2Adv(szRet,"GetCustomizedDevName", "DevName", pszDevName, pnDevNameLen))
	{
		nRetCode = -1;
	}


	return nRetCode ==0 ?S_OK: E_FAIL;
}



HV_API_EX HRESULT CDECL HVAPI_SetCustomizedDevName(HVAPI_HANDLE_EX hHandle, CHAR* pszDevName)
{
	if(hHandle == NULL || NULL == pszDevName)
	{
		return E_FAIL;
	}

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
	if(strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0
		|| pHHC->dwOpenType != 1)
	{
		return E_FAIL;
	}

	CHAR szRet [1*1024]={0};
	INT nRetLen = 1*1024;

	char szCmd[512];
	sprintf(szCmd,"SetCustomizedDevName,devName[%s]", pszDevName);

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , szCmd , szRet , nRetLen , &nRetLen ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_SetCustomizedDevName  HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	//解析命令返回


	INT nRetCode = -1;

	CXmlParseInfo rgRetInfo [1];
	strcpy( rgRetInfo [0].szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( "SetCustomizedDevName", szRet ,nRetLen , rgRetInfo, 1 ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_SetCustomizedDevName  HvXmlParse Fail\n");
		return E_FAIL ;
	}
	nRetCode = atoi (rgRetInfo[0]. szKeyValue);

	return nRetCode ==0 ?S_OK: E_FAIL;
}

HV_API_EX HRESULT CDECL HVAPI_SetDeNoiseSwitch(HVAPI_HANDLE_EX hHandle, INT nSwitch )
{
	return HVAPI_SetDeNoiseMode(hHandle, nSwitch);
}

HV_API_EX HRESULT CDECL HVAPI_SetH264SecondBitRate(HVAPI_HANDLE_EX hHandle, INT nBitRate )
{
	return Default_Setter_Int_ForMercuryProtocol(hHandle, "SetH264SecondBitRate", nBitRate);
}


HV_API_EX HRESULT CDECL HVAPI_GetH264SecondBitRate(HVAPI_HANDLE_EX hHandle, INT* pnBitRate )
{
	return Default_Getter_Int_ForMercuryProtocol(hHandle, "GetH264SecondBitRate", pnBitRate);
}/////////////////////////////////////

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
		WrightLogEx(pHHC->szIP, "SendTriggerOut  HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	// 解析命令返回
	INT nRetCode = -1;

	CXmlParseInfo rgRetInfo [1];
	strcpy( rgRetInfo [0].szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( szCmdName, szRet ,nRetLen , rgRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "SendTriggerOut  HvXmlParse Fail\n");
		return E_FAIL ;
	}

	nRetCode = atoi (rgRetInfo[0]. szKeyValue);

	return nRetCode == 0 ?S_OK: E_FAIL;

}

// 触发报警信号
HV_API_EX HRESULT CDECL HVAPI_TriggerAlarmSignal(HVAPI_HANDLE_EX hHandle)
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

	if ( S_OK != HVAPI_ExecCmdEx( hHandle , "SendAlarmOut" , szRet , nRetLen , &nRetLen ) )
	{
		WrightLogEx(pHHC ->szIP , "HVAPI_GetRunStatus  HVAPI_ExecCmdEx Fail\n");
		return E_FAIL ;
	}

	// 解析命令返回
	INT nRetCode = -1;

	CXmlParseInfo rgRetInfo [1];
	strcpy( rgRetInfo [0].szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury("SendAlarmOut", szRet ,nRetLen , rgRetInfo, 1) )
	{
		WrightLogEx(pHHC->szIP , "SendAlarmOut  HvXmlParse Fail\n");
		return E_FAIL ;
	}
	nRetCode = atoi(rgRetInfo[0].szKeyValue);

	return nRetCode == 0?S_OK:E_FAIL;
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

	// 执行命令
	bool fRet = false;
	char szRetBuf[1024] = { 0 };
	int nRetBufLen = 1024;
	fRet = ExecXmlExtCmdMercury(pHHC->szIP, (char*)szXmlDocBuf, (char*)szRetBuf, nRetBufLen, INVALID_SOCKET);

	// 解析命令返回
	INT nRetCode = -1;

	CXmlParseInfo rgRetInfo [1];
	strcpy( rgRetInfo [0].szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( "SetNameList", szRetBuf, nRetBufLen, rgRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "SetNameList  HvXmlParse Fail\n");
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



	/*
	char szCmd[32] = {"GetNameList"};
	// 执行命令
	bool fRet = false;
	char szCmdXml[1024] = { 0 };
	int nRetBufLen = 1024;
	
	// 生成对应设备版本号的XML格式的命令
	HRESULT hr = HvMakeXmlCmdByString3( szCmd, (int)strlen(szCmd), szCmdXml, nRetBufLen);
	if ( hr != S_OK )
		return E_FAIL;

	int iRetXmlLen = 512 * 1024;
	char *szRetXml = new char[iRetXmlLen];
	if (szRetXml == NULL)
	{
		return E_FAIL;
	}
	fRet = ExecXmlExtCmdMercury(pHHC->szIP, szCmdXml, szRetXml, iRetXmlLen, INVALID_SOCKET);

	// 解析命令返回
	INT nRetCode = -1;

	CXmlParseInfo rgRetInfo [1];
	strcpy( rgRetInfo[0].szKeyName, "RetCode");

	if ( S_OK != HvXmlParseMercury( "GetNameList", szRetXml, iRetXmlLen, rgRetInfo, 1 ) )
	{
		WrightLogEx(pHHC ->szIP , "GetNameList  HvXmlParse Fail\n");
		return E_FAIL ;
	}

	nRetCode = atoi (rgRetInfo[0]. szKeyValue);

	hr = E_FAIL;

	if (nRetCode != 0)
	{
		delete[] szRetXml;
		szRetXml = NULL;
		return E_FAIL;
	}
	else
	{
		int iTempLen = 1024 * 1024;
		char *szTempWhiteList = new char[iTempLen];
		if ( szTempWhiteList == NULL )
			return E_FAIL;

		char *szTempBlackList = new char[iTempLen];
		if ( szTempBlackList == NULL )
		{
			delete[] szTempWhiteList;
			szTempWhiteList = NULL;
			return E_FAIL;
		}

		memset(szTempWhiteList, 0, iTempLen);
		memset(szTempBlackList, 0, iTempLen);

		if (HvGetNamListFromXML(szRetXml, szTempWhiteList, szTempBlackList) ) //  获取成功
		{
			
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
			hr = S_OK;
		}

		delete[] szTempWhiteList;
		szTempWhiteList = NULL;

		delete[] szTempBlackList;
		szTempBlackList = NULL;	
	}

	if (szRetXml != NULL)
	{
		delete[] szRetXml;
		szRetXml = NULL;
	}
	
	return hr;*/
}
HV_API_EX HRESULT CDECL HVAPI_SoftTriggerCapture(HVAPI_HANDLE_EX hHandle)
{
	return Default_Setter_Int_ForMercuryProtocol(hHandle, "SoftTriggerCapture", 0);
}

HV_API_EX HRESULT CDECL HVAPI_SetManualCaptureRGB(HVAPI_HANDLE_EX hHandle, INT nEnable, INT nGainR, INT nGainG, INT nGainB)
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
	sprintf( szCmd , "SetCaptureRgbGain,GainR[%d],GainG[%d],GainB[%d],Enable[%d]" , nGainR, nGainG,nGainB, nEnable);

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetManualCaptureRGB HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParse( "SetCaptureRgbGain", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetManualCaptureRGB HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{

		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParseMercury( "SetCaptureRgbGain", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetManualRGB HvXmlParse Fail\n");
			return E_FAIL;
		}

		nRetCode = atoi(cRetInfo.szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetManualCaptureShutter(HVAPI_HANDLE_EX hHandle, INT nEnable, INT nShutter )
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
	sprintf( szCmd , "SetCaptureShutter,Shutter[%d],Enable[%d]" ,nShutter, nEnable );

	//MessageBox(NULL, szCmd, "", 0);

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetManualCaptureShutter HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );
		WrightLogEx("HVAPI_SetManualCaptureShutter" , szRet);
		if ( S_OK != HvXmlParse( "SetCaptureShutter", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetManualCaptureShutter HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );
		WrightLogEx("HVAPI_SetManualCaptureShutter" , szRet);
		if ( S_OK != HvXmlParseMercury( "SetCaptureShutter", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetManualCaptureShutter HvXmlParse Fail\n");
			return E_FAIL;
		}

		nRetCode = atoi(cRetInfo.szKeyValue);

	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetManualCaptureGain(HVAPI_HANDLE_EX hHandle, INT nEnable, INT nGain )
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
	sprintf( szCmd , "SetCaptureGain,Gain[%d],Enable[%d]" ,nGain, nEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetManualCaptureGain HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );
		WrightLogEx("SetCaptureGain" , szRet);
		if ( S_OK != HvXmlParse( "SetCaptureGain", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetManualCaptureGain HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );
		WrightLogEx("SetCaptureGain" , szRet);
		if ( S_OK != HvXmlParseMercury( "SetCaptureGain", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetManualCaptureGain HvXmlParse Fail\n");
			return E_FAIL;
		}

		nRetCode = atoi(cRetInfo.szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetManualCaptureSharpen(HVAPI_HANDLE_EX hHandle, INT nEnable, INT nSharpen )
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
	sprintf( szCmd , "SetCaptureSharpen,Sharpen[%d],Enable[%d]" ,nSharpen, nEnable );

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetManualCaptureSharpen HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );
		WrightLogEx("HVAPI_SetManualCaptureSharpen" , szRet);
		if ( S_OK != HvXmlParse( "SetCaptureSharpen", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetManualCaptureSharpen HvXmlParse Fail\n");
			return E_FAIL;
		}

		if ( XML_CMD_TYPE_INT == cRetInfo.eKeyType )
		{
			nRetCode = atoi(cRetInfo.szKeyValue);
		}

	}
	else if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );
		WrightLogEx("HVAPI_SetManualCaptureSharpen" , szRet);
		if ( S_OK != HvXmlParseMercury( "SetCaptureSharpen", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetManualCaptureSharpen HvXmlParse Fail\n");
			return E_FAIL;
		}

		nRetCode = atoi(cRetInfo.szKeyValue);

	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_SetExpPluseWidth(HVAPI_HANDLE_EX hHandle, INT nValue)
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
	sprintf( szCmd , "SetExpPluseWidth,PluseWidth[%d]", nValue);

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetExpPluseWidth HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );
		WrightLogEx("HVAPI_SetExpPluseWidth" , szRet);
		if ( S_OK != HvXmlParseMercury( "SetExpPluseWidth", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetExpPluseWidth HvXmlParse Fail\n");
			return E_FAIL;
		}

		nRetCode = atoi(cRetInfo.szKeyValue);
	}

	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

HV_API_EX HRESULT CDECL HVAPI_Capture(HVAPI_HANDLE_EX hHandle, char* szValue)
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

	if ( pHHC->emProtocolVersion == PROTOCOL_VERSION_1 || pHHC->emProtocolVersion == PROTOCOL_VERSION_2 || pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH ) 
	{
		return E_FAIL;
	}

	char szXmlDocBuf[1024] = {0};
	int iXmlLen = 1024;
	int iValueLen = strlen(szValue);
	if ( HvGetXmlForTriggerPlate( szValue, iValueLen, szXmlDocBuf, iXmlLen) == 0 )
	{
		return E_FAIL;
	}

	// 执行命令
	bool fRet = false;
	char szRetBuf[1024] = { 0 };
	int nRetBufLen = 1024;
	fRet = ExecXmlExtCmdMercury(pHHC->szIP, (char*)szXmlDocBuf, (char*)szRetBuf, nRetBufLen, INVALID_SOCKET);

	// 解析命令返回
	INT nRetCode = -1;

	CXmlParseInfo rgRetInfo [1];
	strcpy( rgRetInfo [0].szKeyName, "RetCode" );

	if ( S_OK != HvXmlParseMercury( "TriggerPlateRecog", szRetBuf, nRetBufLen, rgRetInfo, 1 ) )
	{
		WrightLogEx(pHHC->szIP , "TriggerPlateRecog  HvXmlParse Fail\n");
		return E_FAIL ;
	}

	nRetCode = atoi(rgRetInfo[0].szKeyValue);

	return nRetCode ==0 ?S_OK: E_FAIL;
}

//随机产生一个数
int Random(int istart, int iend)
{
	srand(time(NULL));  /*初始化随机数种子*/
    int a=rand()%(iend-istart)+istart;  /*生成一个[istart,iend)区间内的整数*/
	a%=1000;
	return a;
}



HV_API_EX HRESULT CDECL HVAPI_SendCaptureCmd(HVAPI_HANDLE_EX hHandle, DWORD64 dw64CaptureTime)
{
	if (NULL == hHandle)
	{
		return E_FAIL;
	}
	if ( dw64CaptureTime < 0 )
	{
		return E_FAIL;
	}
	DWORD64 dw64time = dw64CaptureTime;
	int iTemp = 0;

	iTemp = Random(0, 9999);

	SYSTEMTIME sys; 
	GetLocalTime( &sys );

	char chTemp[1024] = {0};
	if ( NULL == dw64time || 0 == dw64time)
	{
		sprintf(chTemp, "[TrackID:%d;TIME:%d-%d-%d %d:%d:%d %03d;RESERVED:reserved]", iTemp, sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute,sys.wSecond, sys.wMilliseconds);
	}
	else
	{

		CTime ctm(dw64time / 1000);
		int Minisecond = dw64time % 1000;
		sprintf(chTemp, "[TrackID:%d;TIME:%d-%d-%d %d:%d:%d %03d;RESERVED:reserved]", iTemp, ctm.GetYear(), ctm.GetMonth(), ctm.GetDay(), ctm.GetHour(), ctm.GetMinute(), ctm.GetSecond(), Minisecond);

		//time_t dwTime = (time_t)(dw64CaptureTime/1000);
		//struct tm *tblock = localtime(&dwTime);
		//sprintf(chTemp, "[TrackID:%d;TIME:%d-%d-%d %d:%d:%d %03d;RESERVED:reserved]", iTemp, tblock->tm_year + 1970, tblock->tm_mon, tblock->tm_mday, tblock->tm_hour, tblock->tm_min, tblock->tm_sec,dw64CaptureTime % 1000);
	}

	if (HVAPI_Capture(hHandle, chTemp))
	{
		return E_FAIL;
	}
	else
	{
		return S_OK;
	}
}

HV_API_EX HRESULT CDECL HVAPI_OpenListenCMDSever(int nNum)
{
	IHvLitenCMD *pListenCMD = IHvLitenCMD::CreateInstance(CAMERA_CMD_LINK_PORT,nNum);
	if (!pListenCMD)
	{
		return E_FAIL;
	}
	pListenCMD->Stop();
	pListenCMD->Start();

	return S_OK;
}

HV_API_EX HRESULT CDECL HVAPI_CloseListenCMDSever()
{
	IHvLitenCMD *pListenCMD = IHvLitenCMD::GetInstance();
	if (!pListenCMD)
	{
		return E_FAIL;
	}
	pListenCMD->Close();
	SAFE_DELETE(pListenCMD);
	return S_OK;
}

HV_API_EX HRESULT CDECL HVAPI_SetCallBackListen(HVAPI_HANDLE_EX hHandle,PVOID pFunc, PVOID pUserData)
{
	IHvLitenCMD *pListenCMD = IHvLitenCMD::GetInstance();
	if (!pListenCMD)
	{
		return E_FAIL;
	}
	
	pListenCMD->SetCallBack(hHandle,pFunc,pUserData);
	//pListenCMD->Stop();
	//pListenCMD->Start();
	return S_OK;
}


HV_API_EX HRESULT CDECL HVAPI_SetCallBackListenEx(HVAPI_HANDLE_EX hHandle,INT iCallBackType,PVOID pFunc, PVOID pUserData)
{
	IHvLitenCMD *pListenCMD = IHvLitenCMD::GetInstance();
	if (!pListenCMD)
	{
		return E_FAIL;
	}

	pListenCMD->SetCallBackEx(hHandle,iCallBackType,pFunc,pUserData);
	//pListenCMD->Stop();
	//pListenCMD->Start();
	return S_OK;
}

HV_API_EX HVAPI_HANDLE_EX CDECL HVAPI_OpenListenClientEx(LPCSTR szIp)
{
	IHvLitenCMD *pListenCMD = IHvLitenCMD::GetInstance();
	if (!pListenCMD)
	{
		return NULL;
	}

	return pListenCMD->OpenListenHanle(szIp);
}


HV_API_EX HRESULT CDECL HVAPI_CloseListenClientEx(HVAPI_HANDLE_EX hHandle)
{
	IHvLitenCMD *pListenCMD = IHvLitenCMD::GetInstance();
	if (!pListenCMD)
	{
		return E_FAIL;
	}
	
	return pListenCMD->CloseListenHanle(hHandle);

}

// 获取客流量信息
HV_API_EX HRESULT CDECL HVAPI_GetPCSFlow(HVAPI_HANDLE_EX hHandle, DWORD64 dw64StartTime, DWORD64 dw64EndTime, CHAR* szRetInfo, INT* iLen)
{
    if (hHandle == NULL || NULL == szRetInfo)
    {
        return E_FAIL;
    }

    HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
    if (strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0 || pHHC->dwOpenType != 1)
    {
        return E_FAIL;
    }

    if ( pHHC->emProtocolVersion == PROTOCOL_VERSION_1
         || pHHC->emProtocolVersion == PROTOCOL_VERSION_2
         || pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH )
    {
        return E_FAIL;
    }

	DWORD64 dw64TimeTemp = dw64EndTime - dw64StartTime;
	if (dw64StartTime > dw64EndTime || dw64TimeTemp < 0 || dw64TimeTemp > (1000 * 60 * 60 *24 * 31))
	{
		*iLen = 0; 
		return S_FALSE;
	}

    time_t dwStartTime = (time_t)(dw64StartTime / 1000);
    struct tm *tmStart = localtime(&dwStartTime);
	// 两次调用localtime，编译器会优化成只有一个内存空间，第一次的会被后一次给刷新
	struct tm tmTemp;
	tmTemp.tm_year = tmStart->tm_year;
	tmTemp.tm_mon = tmStart->tm_mon;
	tmTemp.tm_mday = tmStart->tm_mday;
	tmTemp.tm_hour = tmStart->tm_hour;
	tmTemp.tm_min = tmStart->tm_min;

    time_t dwEndTime = (time_t)(dw64EndTime / 1000);
    struct tm *tmEnd = localtime(&dwEndTime);

	char chTime[512] = {0};
	sprintf(chTime, "StartTime:[%04d-%02d-%02d %02d:%02d];EndTime:[%04d-%02d-%02d %02d:%02d]",
		tmTemp.tm_year + 1900, tmTemp.tm_mon + 1, tmTemp.tm_mday, tmTemp.tm_hour, tmTemp.tm_min,
		tmEnd->tm_year + 1900, tmEnd->tm_mon + 1, tmEnd->tm_mday, tmEnd->tm_hour, tmEnd->tm_min);

    char szCmd[32] = {"GetPCSFlow"};
    char szCmdXml[1024] = { 0 };
    if (HvGetXmlOfStringType(szCmd, "GETTER", chTime, szCmdXml) == 0)
    {
        return E_FAIL;
    }

    int iRetXmlLen = (1024<<10)*2 + 1024;
    char *szRetXml = new char[iRetXmlLen];
    if (szRetXml == NULL)
    {
        return E_FAIL;
    }
    ExecXmlExtCmdMercury(pHHC->szIP, szCmdXml, szRetXml, iRetXmlLen, INVALID_SOCKET);

    // 解析命令返回
    INT nRetCode = -1;
    CXmlParseInfo rgRetInfo[1];
    strcpy( rgRetInfo[0].szKeyName, "RetCode");

    if ( S_OK != HvXmlParseMercury(szCmd, szRetXml, iRetXmlLen, rgRetInfo, 1 ) )
    {
        WrightLogEx(pHHC ->szIP , "GetPCSFlow  HvXmlParse Fail !\n");
        delete[] szRetXml;
        szRetXml = NULL;
        return E_FAIL ;
    }
    nRetCode = atoi(rgRetInfo[0].szKeyValue);

	int iRet = -1;
    if (nRetCode != 0)
    {
        delete[] szRetXml;
        szRetXml = NULL;
        return E_FAIL;
    }
    else
    {
        int iTempLen = (1024<<10) * 2;
        char *szRetTemp = new char[iTempLen];
        if ( szRetTemp == NULL )
        {
            delete[] szRetXml;
            szRetXml = NULL;
            return E_FAIL;
        }
        HvGetRetMsgFromXml(szRetXml, szRetTemp);

        int iRetLen = (int)strlen(szRetTemp);
        if (iRetLen <= *iLen)
        {
            strncpy(szRetInfo, szRetTemp, iRetLen);
            *iLen = iRetLen;
			iRet = 0;
        }
		else 
		{
			*iLen = iRetLen;
			iRet = -1;
		}

		if (szRetTemp != NULL)
		{
			delete[] szRetTemp;
			szRetTemp = NULL;
		}
    }

    if (szRetXml != NULL)
    {
        delete[] szRetXml;
        szRetXml = NULL;
    }

	if (-1 == iRet)
	{
		return S_FALSE;
	}
	else
	{
		return S_OK;
	}
}

HV_API_EX HRESULT CDECL HVAPI_SetFaceDataAdv(HVAPI_HANDLE_EX hHandle, INT nNum, CHAR* szUploadDate, DWORD32 dwDataLen)
{
	switch(nNum)
	{
	case 0:
		return HVAPI_UploadData(hHandle, CAMERA_SET_CAR_FACE_NORMAL_CMD, szUploadDate, dwDataLen);
		break;
	case 1:
		return HVAPI_UploadData(hHandle, CAMERA_SET_CAR_FACE_EX_CMD, szUploadDate, dwDataLen);
		break;
	case 2:
		return HVAPI_UploadData(hHandle, CAMERA_SET_CAR_FACE_INFO_CMD, szUploadDate, dwDataLen);
		break;
	default:
		break;
	}
	return E_FAIL;
}

HV_API_EX HRESULT CDECL HVAPI_SetDSPParam(HVAPI_HANDLE_EX hHandle, CHAR* szUploadDate, DWORD32 dwDataLen)
{
	return HVAPI_UploadData(hHandle, CAMERA_SET_DSP_PARAM_CMD, szUploadDate, dwDataLen);
}

//----------------------- add by cxr --- begin

HV_API_EX HRESULT CDECL HVAPI_SetJpegOutType(HVAPI_HANDLE_EX hHandle,  INT nMode)
{
	return Default_Setter_Int_ForMercuryProtocol(hHandle, "SetJpegOutType", nMode);
}

HV_API_EX HRESULT CDECL HVAPI_GetJpegOutType(HVAPI_HANDLE_EX hHandle,  INT* pnMode)
{
	return Default_Getter_Int_ForMercuryProtocol(hHandle, "GetJpegOutType", pnMode);
}

HV_API_EX HRESULT CDECL HVAPI_SetCoilEnable(HVAPI_HANDLE_EX hHandle,  INT nMode)
{
	return Default_Setter_Int_ForMercuryProtocol(hHandle, "SetCoilEnable", nMode);
}

HV_API_EX HRESULT CDECL HVAPI_GetCoilEnable(HVAPI_HANDLE_EX hHandle,  INT* pnMode)
{
	return Default_Getter_Int_ForMercuryProtocol(hHandle, "GetCoilEnable", pnMode);
}

//------------------------------------------------------end

//获取AWB工作模式
HV_API_EX HRESULT CDECL HVAPI_GetAWBWorkMode(HVAPI_HANDLE_EX hHandle,  BOOL* pfEnable)
{
	{
		if(hHandle == NULL || NULL == pfEnable )
		{
			return E_FAIL ;
		}

		HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
		if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
			|| pHHC->dwOpenType != 1)
		{
			return E_FAIL ;
		}

		CHAR szRet [1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetAWBWorkMode" , szRet , nRetLen , &nRetLen ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetAWBWorkMode HVAPI_ExecCmdEx Fail\n");
			return E_FAIL ;
		}

		//解析命令返回
		INT nRetCode = -1;
		if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "value" );

			if ( S_OK != HvXmlParseMercury( "GetAWBWorkMode", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetAWBWorkMode HvXmlParse Fail\n");
				return E_FAIL ;
			}


			nRetCode = atoi (rgRetInfo[0]. szKeyValue);

			* pfEnable = atoi (rgRetInfo[1]. szKeyValue);

		}



		return nRetCode ==0 ?S_OK: E_FAIL;
	}
}
//设置AWB工作模式
HV_API_EX HRESULT CDECL HVAPI_SetAWBWorkMode(HVAPI_HANDLE_EX hHandle, INT nEnable )
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
	sprintf( szCmd , "SetAWBWorkMode,Enable[%d]" ,nEnable);

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetAWBWorkMode HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParseMercury( "SetAWBWorkMode", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAWBWorkMode HvXmlParse Fail\n");
			return E_FAIL;
		}

 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}
//获取相机全控制
HV_API_EX HRESULT CDECL HVAPI_GetAutoControlCammeraEnable(HVAPI_HANDLE_EX hHandle,  BOOL* pfEnable)
{
	{
		if(hHandle == NULL || NULL == pfEnable )
		{
			return E_FAIL ;
		}

		HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
		if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
			|| pHHC->dwOpenType != 1)
		{
			return E_FAIL ;
		}

		CHAR szRet [1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetAutoControlCammeraAll" , szRet , nRetLen , &nRetLen ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetAutoControlCammeraEnable HVAPI_ExecCmdEx Fail\n");
			return E_FAIL ;
		}

		//解析命令返回
		INT nRetCode = -1;
		if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "value" );

			if ( S_OK != HvXmlParseMercury( "GetAutoControlCammeraAll", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetAutoControlCammeraEnable HvXmlParse Fail\n");
				return E_FAIL ;
			}


			nRetCode = atoi (rgRetInfo[0]. szKeyValue);

			* pfEnable = atoi (rgRetInfo[1]. szKeyValue);

		}



		return nRetCode ==0 ?S_OK: E_FAIL;
	}
}
//设置相机全控制
HV_API_EX HRESULT CDECL HVAPI_SetAutoControlCammeraEnable(HVAPI_HANDLE_EX hHandle, INT nEnable )
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
	sprintf( szCmd , "SetAutoControlCammeraAll,Enable[%d]" ,nEnable);

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetAutoControlCammeraEnable HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParseMercury( "SetAutoControlCammeraAll", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetAutoControlCammeraEnable HvXmlParse Fail\n");
			return E_FAIL;
		}

 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}
//获取H264分辨率
HV_API_EX HRESULT CDECL HVAPI_GetH264Resolution(HVAPI_HANDLE_EX hHandle,  INT* piValue)
{
	{
		if(hHandle == NULL || NULL == piValue )
		{
			return E_FAIL ;
		}

		HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) hHandle;
		if(strcmp (HVAPI_API_VERSION_EX, pHHC->szVersion ) != 0
			|| pHHC->dwOpenType != 1)
		{
			return E_FAIL ;
		}

		CHAR szRet [1024]={0};
		INT nRetLen = 1024;

		if ( S_OK != HVAPI_ExecCmdEx( hHandle , "GetH264Resolution" , szRet , nRetLen , &nRetLen ) )
		{
			WrightLogEx(pHHC ->szIP , "HVAPI_GetH264Resolution HVAPI_ExecCmdEx Fail\n");
			return E_FAIL ;
		}

		//解析命令返回
		INT nRetCode = -1;
		if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
		{

			CXmlParseInfo rgRetInfo [2];
			strcpy( rgRetInfo [0].szKeyName, "RetCode" );
			strcpy( rgRetInfo [1].szKeyName, "value" );

			if ( S_OK != HvXmlParseMercury( "GetH264Resolution", szRet ,nRetLen , rgRetInfo, 2 ) )
			{
				WrightLogEx(pHHC ->szIP , "HVAPI_GetH264Resolution HvXmlParse Fail\n");
				return E_FAIL ;
			}


			nRetCode = atoi (rgRetInfo[0]. szKeyValue);

			* piValue = atoi (rgRetInfo[1]. szKeyValue);

		}



		return nRetCode ==0 ?S_OK: E_FAIL;
	}
}
//设置H264分辨率
HV_API_EX HRESULT CDECL HVAPI_SetH264Resolution(HVAPI_HANDLE_EX hHandle, INT nEnable )
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
	sprintf( szCmd , "SetH264Resolution,Enable[%d]" ,nEnable);

	CHAR szRet[1024]={0};
	INT nRetLen = 1024;

	if ( S_OK != HVAPI_ExecCmdEx(hHandle , szCmd , szRet , nRetLen , &nRetLen) )
	{
		WrightLogEx(pHHC->szIP , "HVAPI_SetH264Resolution HVAPI_ExecCmdEx Fail\n");
		return E_FAIL;
	}

	//解析命令返回
	INT nRetCode = -1;
	if (pHHC->emProtocolVersion == PROTOCOL_VERSION_MERCURY)
	{
		CXmlParseInfo cRetInfo;
		strcpy( cRetInfo.szKeyName, "RetCode" );


		if ( S_OK != HvXmlParseMercury( "SetH264Resolution", szRet ,nRetLen , &cRetInfo, 1 ) )
		{
			WrightLogEx(pHHC->szIP , "HVAPI_SetH264Resolution HvXmlParse Fail\n");
			return E_FAIL;
		}

 
			nRetCode = atoi(cRetInfo.szKeyValue);
		 
	}


	return nRetCode==0 ?S_OK: (nRetCode == -2 ? S_FALSE:E_FAIL);
}

// 抓拍视频
HV_API_EX HRESULT CDECL HVAPI_CaptureVideo(HVAPI_HANDLE_EX hHandle, INT iFlag)
{
    if (hHandle == NULL)
    {
        return E_FAIL;
    }

    HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
    if (strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0 || pHHC->dwOpenType != 1)
    {
        return E_FAIL;
    }

    if ( pHHC->emProtocolVersion == PROTOCOL_VERSION_1
         || pHHC->emProtocolVersion == PROTOCOL_VERSION_2
         || pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH )
    {
        return E_FAIL;
    }

	char szCmd[32] = {"CaptureVideo"};
    char szCmdXml[1024] = { 0 };

	char chValue[24] = {0};
	sprintf(chValue, "%d", iFlag);

	// 封装命令
    if (HvGetXmlOfStringType(szCmd, "SETTER", chValue, szCmdXml) == 0)
    {
        return E_FAIL;
    }

    int iRetXmlLen = 512;
	char szRetXml[512] = {0};
    ExecXmlExtCmdMercury(pHHC->szIP, szCmdXml, szRetXml, iRetXmlLen, INVALID_SOCKET);

	// 解析命令返回信息
    INT nRetCode = -1;
    CXmlParseInfo rgRetInfo[1];
    strcpy( rgRetInfo[0].szKeyName, "RetCode");

    if ( S_OK != HvXmlParseMercury(szCmd, szRetXml, iRetXmlLen, rgRetInfo, 1 ) )
    {
        WrightLogEx(pHHC ->szIP , "CaptureVideo  HvXmlParse Fail\n");
        return E_FAIL ;
    }
    nRetCode = atoi(rgRetInfo[0].szKeyValue);

	if (nRetCode != 0)
    {
        return E_FAIL;
    }

	return S_OK;
}

// OBC补光灯开关
HV_API_EX HRESULT CDECL HVAPI_OBCLightSwitch(HVAPI_HANDLE_EX hHandle, INT iFlag)
{
    if (hHandle == NULL)
    {
        return E_FAIL;
    }

    HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
    if (strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0 || pHHC->dwOpenType != 1)
    {
        return E_FAIL;
    }

    if ( pHHC->emProtocolVersion == PROTOCOL_VERSION_1
         || pHHC->emProtocolVersion == PROTOCOL_VERSION_2
         || pHHC->emProtocolVersion == PROTOCOL_VERSION_EARTH )
    {
        return E_FAIL;
    }

	char szCmd[32] = {"OBCLightSwitch"};
    char szCmdXml[1024] = { 0 };

	char chValue[24] = {0};
	sprintf(chValue, "%d", iFlag);

	// 封装命令
    if (HvGetXmlOfStringType(szCmd, "SETTER", chValue, szCmdXml) == 0)
    {
        return E_FAIL;
    }

    int iRetXmlLen = 512;
	char szRetXml[512] = {0};
    ExecXmlExtCmdMercury(pHHC->szIP, szCmdXml, szRetXml, iRetXmlLen, INVALID_SOCKET);

	// 解析命令返回信息
    INT nRetCode = -1;
    CXmlParseInfo rgRetInfo[1];
    strcpy( rgRetInfo[0].szKeyName, "RetCode");

    if ( S_OK != HvXmlParseMercury(szCmd, szRetXml, iRetXmlLen, rgRetInfo, 1 ) )
    {
        WrightLogEx(pHHC ->szIP , "OBCLightSwitch  HvXmlParse Fail\n");
        return E_FAIL ;
    }
    nRetCode = atoi(rgRetInfo[0].szKeyValue);

	if (nRetCode != 0)
    {
        return E_FAIL;
    }

	return S_OK;
}


HV_API_EX HRESULT CDECL HVAPI_GetCaptureImageEx(HVAPI_HANDLE_EX hHandle,int nTimeM, BYTE *pImageBuff, int iBuffLen, int *iImageLen, DWORD64 *dwTime,  DWORD *dwImageWidth, DWORD *dwImageHeigh)
{
	if ( NULL == hHandle || NULL ==pImageBuff )
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
	SOCKET hSocketCmd = INVALID_SOCKET;

	CAMERA_CMD_HEADER cCmdHeader;
	CAMERA_CMD_RESPOND cCmdRespond;

	if ( ConnectCamera(pHHC->szIP, wPort, hSocketCmd, 1, 2000) )
	{
		// 协议格式
		//协议ID（4）+内容长度（4）+内容（N : N=0 表示当然图片， N 为前N 分钟有结果的最后一张图片， 在行人卡口中有效，其他无效果）
		cCmdHeader.dwID = CAMERA_CAPTURE_IMAGE;
		cCmdHeader.dwInfoSize = 4;
		if ( sizeof(cCmdHeader) != send(hSocketCmd, (const char*)&cCmdHeader, sizeof(cCmdHeader), 0) )
		{
			ForceCloseSocket(hSocketCmd);
			return E_FAIL;
		}

		if ( sizeof(int) != send( hSocketCmd, (const char*)&nTimeM, sizeof(int), 0) )
		{
			ForceCloseSocket(hSocketCmd);
			return E_FAIL;
		}


		struct ImgeInfo
		{
			int iSize;
			DWORD dwHighTime;
			DWORD dwLowTime;
			DWORD dwWidth;
			DWORD dwHeigh;
		}srtImageInfo;


		if ( sizeof(cCmdRespond) == recv(hSocketCmd, (char*)&cCmdRespond, sizeof(cCmdRespond), 0) )
		{
			if ( CAMERA_CAPTURE_IMAGE == cCmdRespond.dwID && 0 == cCmdRespond.dwResult  && 0 < cCmdRespond.dwInfoSize - sizeof(struct ImgeInfo) )
			{
				BYTE *pBuffTemp = new BYTE[cCmdRespond.dwInfoSize+1];
				memset(pBuffTemp, 0, cCmdRespond.dwInfoSize+1);

				if ( cCmdRespond.dwInfoSize == RecvAll(hSocketCmd, (char*)pBuffTemp, cCmdRespond.dwInfoSize) )
				{
					if ( iImageLen )
						*iImageLen = cCmdRespond.dwInfoSize - sizeof(struct ImgeInfo);


					memcpy(&srtImageInfo, pBuffTemp, sizeof(srtImageInfo));

					if ( dwTime != NULL  )
					{
						DWORD64 dwTempTime = srtImageInfo.dwHighTime;
						dwTempTime = (dwTempTime <<32) | srtImageInfo.dwLowTime;
						*dwTime = dwTempTime;
					}

					if ( dwImageWidth != NULL )
						*dwImageWidth = srtImageInfo.dwWidth;

					if ( dwImageHeigh != NULL )
						*dwImageHeigh = srtImageInfo.dwHeigh;

					if (iBuffLen >= cCmdRespond.dwInfoSize-sizeof(srtImageInfo) )
					{
						memcpy(pImageBuff, pBuffTemp+sizeof(srtImageInfo), cCmdRespond.dwInfoSize-sizeof(srtImageInfo));
					}
					else
					{
						SAFE_DELETE_ARG(pBuffTemp);
						ForceCloseSocket(hSocketCmd);
						return S_FALSE;
					}
				}//

				SAFE_DELETE_ARG(pBuffTemp);
				ForceCloseSocket(hSocketCmd);
			}
			else
			{
				ForceCloseSocket(hSocketCmd);
			}
		}
		else
		{
			ForceCloseSocket(hSocketCmd);
			return S_FALSE;
		}
	
	}
	else
	{
		return S_FALSE;
	}

	return S_OK;

}



HV_API_EX HRESULT CDECL HVAPI_GetCaptureImage(HVAPI_HANDLE_EX hHandle, INT iVideoID, const CHAR *strFileName)
{
	if ( hHandle == NULL )
		return E_FAIL;

	HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*)hHandle;
    if (strcmp(HVAPI_API_VERSION_EX, pHHC->szVersion) != 0 || pHHC->dwOpenType != 1)
    {
        return E_FAIL;
    }

	


	HRESULT hr = E_FAIL;

	//增加了不使用软触发命令也能抓拍的方法， 在这里直接掉用该方法， 不再使用软触发了。如果不成功，再使用软触发。

	INT iBuffLen = 1024 * 1024;
	INT iImageLen = 0;
	BYTE *pBuffImage = new BYTE[iBuffLen];
	memset(pBuffImage, 0, iBuffLen);

	DWORD64 dwTime64 = 0;
	DWORD dwWidth = 0, dwHeigh = 0;

	if ( HVAPI_GetCaptureImageEx(hHandle,0, pBuffImage, iBuffLen, &iImageLen, &dwTime64,  &dwWidth, &dwHeigh) == S_OK  && iImageLen > 0  )
	{
		//保存图片
		//保存图片到硬盘
		CString file_name = "";
		file_name.Format("%s",strFileName);
		MakeSureDirectoryPathExists(file_name.GetBuffer());
		file_name.ReleaseBuffer();
		FILE *myFile = fopen(strFileName, "wb");
		if ( myFile != NULL )
		{
			fwrite(pBuffImage, 1, iImageLen, myFile);
			fclose(myFile);
			myFile = NULL;
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}

		delete[] pBuffImage;
		pBuffImage = NULL;
		return hr;

	}

	if ( pBuffImage != NULL )
	{
		delete[] pBuffImage;
		pBuffImage = NULL;
	}

	

	EnterCriticalSection(&(pHHC->csLockGetImage));
	hr = HVAPI_SoftTriggerCapture(pHHC);
	pHHC->fCapureFlag = true;
	
	if ( hr != S_OK )
	{
		pHHC->fCapureFlag = false;
		LeaveCriticalSection(&(pHHC->csLockGetImage));
		return E_FAIL;
	}
	else
	{
		//等待结果到来
		int iTryCount = 0;
		while( pHHC->fCapureFlag && iTryCount<10 )
		{
			Sleep(300);
			iTryCount++;
		}

		pHHC->fCapureFlag = false;
		hr = S_OK;
		
		if (pHHC->iCaptureImgeSize != 0 && pHHC->pCaputureImage != NULL )
		{
				//保存图片到硬盘
				CString file_name = "";
				file_name.Format("%s",strFileName);
				MakeSureDirectoryPathExists(file_name.GetBuffer());
				file_name.ReleaseBuffer();

				FILE *myFile = fopen(strFileName, "wb");
				if ( myFile != NULL )
				{
					fwrite(pHHC->pCaputureImage, 1, pHHC->iCaptureImgeSize, myFile);
					fclose(myFile);
					myFile = NULL;
				}
				else
				{
					 hr = E_FAIL;
				}
		}
		else
		{
				 hr = E_FAIL;
		}

		pHHC->iCaptureImgeSize = 0;
		if (pHHC->pCaputureImage!=NULL)
		{
			delete[] pHHC->pCaputureImage;
			pHHC->pCaputureImage = NULL;
		}

	}
	LeaveCriticalSection(&(pHHC->csLockGetImage));

	return hr;
}


