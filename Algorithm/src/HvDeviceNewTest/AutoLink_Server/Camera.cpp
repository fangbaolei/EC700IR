#include "StdAfx.h"
#include "Camera.h"

extern CRITICAL_SECTION g_csLog;
extern CRITICAL_SECTION g_csOperation;
void WriteFileName(const char* chFileName);

DWORD WINAPI RecoThread(LPVOID lpParam);

CCamera::CCamera()
{
	m_hThread = NULL;
	m_hHandleThread = NULL;

	m_HvResult = NULL;
	m_iLogEnable = 0;
	m_iImgProcessEnable = 1;

	m_iFontSize = 25;
	m_iImgMode = 0;
	m_dwFontColor = 0;
	m_dwFontEdgeColor = 0;
	m_iBigImgWidth = 1600;
	m_iBigImgHeight = 1200;
	m_iCompressEnable = 0;
	m_iCompressQuality = 100;
	m_iCompressSubQuality = 5;
	m_iCompressSize = 300 * 1024;
	m_iFeatureEnable = 1;
	m_iFeatureWidth = 400;
	m_iFeatureHeigh = 300;

	m_iSyncTimeCount = 0;

	m_hHv = NULL;
	m_dwConnectStatus = 0;
	m_bExit = true;
	m_iDevType = 0;
	m_iPort = 0;

	InitializeCriticalSection( &m_csResult );
	
	m_Common.GetEncoderClsid(L"image/jpeg", &m_jpgClsid);
}

CCamera::~CCamera(void)
{
	DeleteCriticalSection( &m_csResult );
}

// 对外接口::打开
int CCamera::Open(CString strDevSN, CString strDir)
{
	m_strDevSN = strDevSN;
	m_strDir = strDir;

	ReadIniFlie();
	if (-1 == Connect())
		return -1;

	m_bExit = false;
	// 状态监测线程
	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecoThread, this, 0, NULL);
	// 结果处理线程
	m_hHandleThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HandleResultThread, this, 0, NULL);
	return 0;
}

// 对外接口::关闭
int CCamera::Close()
{
	m_bExit = true;
	if (WaitForSingleObject(m_hThread, 2000) == WAIT_OBJECT_0)
	{
	}
	if (m_hThread != NULL)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	DisConnect();
	WriteIniFlie();
	if (WaitForSingleObject(m_hHandleThread, 2000) == WAIT_OBJECT_0)
	{
	}
	if (m_hHandleThread != NULL)
	{
		CloseHandle(m_hHandleThread);
		m_hHandleThread = NULL;
	}
	return 0;
}

// 对外接口::时间同步
int CCamera::SyncTime()
{
	if (300 == m_iSyncTimeCount || 0 == m_iSyncTimeCount)
	{
		time_t t = time(NULL);
		struct tm *Tm = localtime(&t);
		char chTemp[1024] = {0};
		sprintf(chTemp, "SetTime,Date[%d.%02d.%02d],Time[%02d:%02d:%02d]", Tm->tm_year+1900, Tm->tm_mon+1, Tm->tm_mday, Tm->tm_hour, Tm->tm_min, Tm->tm_sec);

		char szRetBuf[1024] = {0};
		int nRetLen = 0;
		if (m_hHv != NULL)
		{
			if (HVAPI_ExecCmdEx(m_hHv, chTemp, szRetBuf, 1024, &nRetLen) != S_OK)
				return -1;
		}
		
		m_iSyncTimeCount = 1;
	}

	m_iSyncTimeCount++;
	if (m_iSyncTimeCount > 300)
	{	
		m_iSyncTimeCount = 1;
	}

	return 0;
}

// 对外接口::获取状态
DWORD CCamera::GetStatus()
{
	return m_dwConnectStatus;
}

// 设备连接
int CCamera::Connect()
{
	if (m_strDevSN.IsEmpty())
	{
		return -1;
	}

	DisConnect();

	// 连接设备
	m_hHv = HVAPI_OpenAutoLink(m_strDevSN, NULL);
	if (NULL == m_hHv)
		return -1;

	// 获取设备IP
	char chDevSN[256] = {0};
	sprintf(chDevSN, "%s", m_strDevSN.GetBuffer(m_strDevSN.GetLength()));
	m_strDevSN.ReleaseBuffer();
	HVAPI_GetDeviceInfoAutoLink(chDevSN, NULL, m_strDevIP.GetBuffer(64), 64, m_iPort);
	m_strDevIP.ReleaseBuffer();

	// 获取模式
	PROTOCOL_VERSION emProtocolVersion;
	HVAPI_GetXmlVersionEx(m_hHv, &emProtocolVersion);

	char szRetBuf[512] = {0};
	int nRetLen = 0;
	if ( HVAPI_ExecCmdEx(m_hHv, "OptWorkMode", szRetBuf, 512, &nRetLen) == S_OK)
	{
		char szInfoValue[256] = {0};
		if ( HVAPIUTILS_GetExeCmdRetInfoEx(true, szRetBuf, "OptWorkMode", "WorkMode", szInfoValue) == S_OK )
			m_iDevType = atoi(szInfoValue);

		char cmd[256] = {0};
		sprintf(cmd, "GetWorkModeIndex,WorkModeIndex[%d]", m_iDevType);
		nRetLen = 0;
		memset(szRetBuf, 0, 512);
		memset(szInfoValue, 0, 256);
		if ( HVAPI_ExecCmdEx(m_hHv, cmd, szRetBuf, 512, &nRetLen) == S_OK)
		{
			HVAPIUTILS_GetExeCmdRetInfoEx( (emProtocolVersion == PROTOCOL_VERSION_1 ? false : true ), szRetBuf, "GetWorkModeIndex", "WorkModeName", szInfoValue);
			if ( strstr(szInfoValue, "电子警察") != NULL )
			{	
				WriteLog("电子警察");
				m_iDevType = 1;
			}
			else
			{
				WriteLog("卡口");
				m_iDevType = 0;
			}
		}
	}
	
	char szCommand[512];
	sprintf(szCommand, "DownloadRecord,Enable[%d],BeginTime[%s],Index[%d],EndTime[%s],DataInfo[%d]",
		m_safeModeInfo.iEanbleSafeMode, m_safeModeInfo.szBeginTime, m_safeModeInfo.index, m_safeModeInfo.szEndTime, m_safeModeInfo.DataInfo);
	WriteLog(szCommand);

	if ((HVAPI_SetCallBackEx(m_hHv, RecordInfoBeginCallBack, this, 0, CALLBACK_TYPE_RECORD_INFOBEGIN, NULL) != S_OK)
		|| (HVAPI_SetCallBackEx(m_hHv, RecordInfoEndCallBack, this, 0, CALLBACK_TYPE_RECORD_INFOEND, NULL) != S_OK)
		|| (HVAPI_SetCallBackEx(m_hHv, GetBigImgCallBack, this, 0, CALLBACK_TYPE_RECORD_BIGIMAGE, NULL) != S_OK)
		|| (HVAPI_SetCallBackEx(m_hHv, GetSmallImgCallBack, this, 0, CALLBACK_TYPE_RECORD_SMALLIMAGE, NULL) != S_OK)
		|| (HVAPI_SetCallBackEx(m_hHv, GetBinImgCallBack, this, 0, CALLBACK_TYPE_RECORD_BINARYIMAGE, NULL) != S_OK)
		|| (HVAPI_SetCallBackEx(m_hHv, GetPlateCallBack, this, 0, CALLBACK_TYPE_RECORD_PLATE, szCommand) != S_OK)
		)
	{
		WriteLog("设置回调失败！\n");
		DisConnect();
		return -1;
	}
	WriteLog("连接成功！");
	
	return 0;
}

int CCamera::DisConnect()
{
	if (NULL == m_hHv)
		return -1;

	Result* pHvResult = NULL;
	EnterCriticalSection(&m_csResult);
	while(true)
	{
		if (!m_HvResultList.IsEmpty())
		{
			pHvResult = m_HvResultList.RemoveHead();
			if (pHvResult != NULL)
			{
				delete pHvResult;
				pHvResult = NULL;
			}
		}

		if (m_HvResultList.IsEmpty())
			break;
	}
	LeaveCriticalSection(&m_csResult);

	HVAPI_CloseEx(m_hHv);
	m_hHv = NULL;
	WriteLog("断开成功！");
	return 0;
}

// 状态监测线程
int CCamera::RecoConnect()
{
	// 主动连接无法使用被动的其他接口
	//DWORD dwStatus = -1;
	//while(!m_bExit)
	//{
	//	if ( HVAPI_GetConnStatusEx(m_hHv, CONN_TYPE_RECORD, &dwStatus) == S_OK )
	//	{
	//		if (dwStatus == CONN_STATUS_NORMAL 
	//			|| dwStatus == CONN_STATUS_RECVDONE)
	//		{
	//			SyncTime(); // 时间同步
	//			m_dwConnectStatus = dwStatus;
	//		}
	//		else if (dwStatus == CONN_STATUS_RECONN)
	//		{
	//			m_dwConnectStatus = dwStatus;
	//		}
	//		else
	//		{
	//			m_dwConnectStatus = dwStatus;
	//			//Connect();
	//		}
	//	}
	//	else
	//	{
	//		m_dwConnectStatus = CONN_STATUS_UNKNOWN;
	//		WriteLog("获取状态失败！");
	//		//Connect();
	//	}

	//	Sleep(1000);
	//}
	return 0;
}

// 结果保存线程
int CCamera::HandleResult()
{
	Result* pHvResult = NULL;
	while(!m_bExit)
	{
		EnterCriticalSection(&m_csResult);
		if (!m_HvResultList.IsEmpty())
		{
			pHvResult = m_HvResultList.RemoveHead();
		}
		else
		{
			Sleep(500);
		}
		LeaveCriticalSection(&m_csResult);
		
		if (pHvResult == NULL)
			continue;

		SaveResult(pHvResult);
		if (pHvResult != NULL)
		{
			delete pHvResult;
			pHvResult = NULL;
		}
	}
	return 0;
}

// 读取ini文件
int CCamera::ReadIniFlie()
{
	TCHAR fileName[MAX_PATH];
	GetModuleFileName(NULL, fileName, MAX_PATH-1);
	PathRemoveFileSpec(fileName);
	TCHAR chIniFileName[MAX_PATH] = { 0 };
	strcpy(chIniFileName, fileName);
	strcat(chIniFileName, _T("\\Config.ini"));

	ReadHistoryIniFile();

	EnterCriticalSection(&g_csLog);

	// 保存路径
	GetPrivateProfileString("DIR_SET", "IllegalDir", "$(DeviceIP)\\$(IllegalInfo)违法\\$(StreetName)\\$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)\\", m_chIllegalDir, 1024, chIniFileName);
	GetPrivateProfileString("DIR_SET", "NormalDir", "$(DeviceIP)\\卡口\\$(StreetName)\\$(Year)$(Month)$(Day)\\$(Hour)\\", m_chNormalDir, 1024, chIniFileName);

	// 文件命名
	m_FileNameInfo.iBigImgEnable = GetPrivateProfileInt("FILENAME_SET", "BigImgEnable", 1, chIniFileName);
	m_FileNameInfo.iSmallImgEnable = GetPrivateProfileInt("FILENAME_SET", "SmallImgEnable", 1, chIniFileName);
	m_FileNameInfo.iBinEnable = GetPrivateProfileInt("FILENAME_SET", "BinEnable", 1, chIniFileName);
	m_FileNameInfo.iInfoEnable = GetPrivateProfileInt("FILENAME_SET", "InfoEnable", 1, chIniFileName);
	m_FileNameInfo.iPlateEnable = GetPrivateProfileInt("FILENAME_SET", "PlateEnable", 1, chIniFileName);
	m_FileNameInfo.iPlateNoEnable = GetPrivateProfileInt("FILENAME_SET", "PlateNoEnable", 0, chIniFileName);

	GetPrivateProfileString("FILENAME_SET", "BigImgFileName", "$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)$(MilliSecond)_$(BigImageID_Letter).jpg", m_FileNameInfo.chBigImgFileName, 1024, chIniFileName);
	GetPrivateProfileString("FILENAME_SET", "SmallImgName", "$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)$(MilliSecond).bmp", m_FileNameInfo.chSmallImgFileName, 1024, chIniFileName);
	GetPrivateProfileString("FILENAME_SET", "BinFileName", "$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)$(MilliSecond).bin", m_FileNameInfo.chBinFileName, 1024, chIniFileName);
	GetPrivateProfileString("FILENAME_SET", "InfoFileName", "$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)$(MilliSecond).inf", m_FileNameInfo.chInfoFileName, 1024, chIniFileName);
	GetPrivateProfileString("FILENAME_SET", "PlateFileName", "$(Year)$(Month)$(Day)$(Hour)$(Minute)$(Second)$(MilliSecond).txt", m_FileNameInfo.chPlateFileName, 1024, chIniFileName);
	GetPrivateProfileString("FILENAME_SET", "PlateNoFileName", "PlateNo.txt", m_FileNameInfo.chPlateNoFileName, 1024, chIniFileName);

	// 日志开关
	m_iLogEnable = GetPrivateProfileInt("LOG_SET", "LogEnable", 0, chIniFileName);

	// 压缩选项
	m_iCompressEnable = GetPrivateProfileInt("COMPRESS_SET", "CompressEnable", 0, chIniFileName);
	m_iCompressQuality = GetPrivateProfileInt("COMPRESS_SET", "CompressQuality", 100, chIniFileName);
	m_iCompressSubQuality = GetPrivateProfileInt("COMPRESS_SET", "CompressSubQuality", 5, chIniFileName);
	m_iCompressSize = GetPrivateProfileInt("COMPRESS_SET", "CompressSize", 300, chIniFileName);
	if (m_iCompressSize <= 0)
		m_iCompressSize = 300 * 1024;
	else
		m_iCompressSize *= 1024;

	// 图像处理开关
	m_iImgProcessEnable = GetPrivateProfileInt("JPG_SET", "ImgProcessEnable", 1, chIniFileName);

	// 图片保存模式 0为按原图分辨率保存  1为按配置分辨率保存
	m_iImgMode = GetPrivateProfileInt("JPG_SET", "ImgMode", 0, chIniFileName);

	// 图片大小
	m_iBigImgWidth = GetPrivateProfileInt("JPG_SET", "BigImgWidth", 1600, chIniFileName);
	if (m_iBigImgWidth <= 0)
		m_iBigImgWidth = 1600;
	m_iBigImgHeight = GetPrivateProfileInt("JPG_SET", "BigImgHeight", 1200, chIniFileName);
	if (m_iBigImgWidth <= 0)
		m_iBigImgWidth = 1200;

	// 字体大小
	m_iFontSize = GetPrivateProfileInt("JPG_SET", "FontSize", 25, chIniFileName);

	char chTemp[256] = {0};
	int r, g, b;
	// 字体颜色
	GetPrivateProfileString("JPG_SET", "FontColor", "255,0,0", chTemp, 256, chIniFileName);
	sscanf(chTemp, "%d,%d,%d", &r, &g, &b);
	m_dwFontColor = RGB(r, g, b);

	// 水印颜色
	GetPrivateProfileString("JPG_SET", "FontEdgeColor", "0,0,0", chTemp, 256, chIniFileName);
	sscanf(chTemp, "%d,%d,%d", &r, &g, &b);
	m_dwFontEdgeColor = RGB(r, g, b);

	// 车身截图选项
	m_iFeatureEnable = GetPrivateProfileInt("FeatureInfo", "FeatureEnable", 1, chIniFileName);
	m_iFeatureWidth = GetPrivateProfileInt("FeatureInfo", "FeatureWidth", 600, chIniFileName);
	m_iFeatureHeigh = GetPrivateProfileInt("FeatureInfo", "FeatureHeigh", 450, chIniFileName);

	LeaveCriticalSection(&g_csLog);

	return 0;
}

// 读取历史结果ini文件
int CCamera::ReadHistoryIniFile()
{
	TCHAR fileName[MAX_PATH];
	GetModuleFileName(NULL, fileName, MAX_PATH-1);
	PathRemoveFileSpec(fileName);
	TCHAR chIniFileName[MAX_PATH] = { 0 };
	strcpy(chIniFileName, fileName);
	strcat(chIniFileName, _T("\\SafeModeConfig.ini"));

	SYSTEMTIME systemTime;
	GetSystemTime(& systemTime);
	char szDefaultTime[64];
	ZeroMemory(szDefaultTime, sizeof(szDefaultTime));
	sprintf(szDefaultTime , "%4d.%2d.%2d_%2d", systemTime.wYear ,systemTime.wMonth , systemTime.wDay ,systemTime.wHour );
	CString strDefaultTime(szDefaultTime);
	strDefaultTime.Replace(' ','0');

	EnterCriticalSection(&g_csLog);

	m_safeModeInfo.iEanbleSafeMode = GetPrivateProfileInt(m_strDevSN, "SafeModeEnable", 0, chIniFileName);
	GetPrivateProfileString(m_strDevSN, "BeginTime", strDefaultTime, m_safeModeInfo.szBeginTime, 256, chIniFileName);
	GetPrivateProfileString(m_strDevSN, "EndTime", "0", m_safeModeInfo.szEndTime, 256, chIniFileName);
	m_safeModeInfo.index = GetPrivateProfileInt(m_strDevSN, "Index", 0, chIniFileName);
	m_safeModeInfo.DataInfo = GetPrivateProfileInt(m_strDevSN, "DataType", 0, chIniFileName);

	LeaveCriticalSection(&g_csLog);

	return 0;
}

// 写ini文件
int CCamera::WriteIniFlie()
{
	TCHAR chFileName[MAX_PATH];
	GetModuleFileName(NULL, chFileName, MAX_PATH-1);
	PathRemoveFileSpec(chFileName);
	TCHAR chIniFileName[MAX_PATH] = { 0 };
	strcpy(chIniFileName, chFileName);
	strcat(chIniFileName, "\\Config.ini");

	EnterCriticalSection(&g_csLog);

	char chTemp[256] = {0};

	// 路径信息
	WritePrivateProfileString("DIR_SET", "IllegalDir", m_chIllegalDir, chIniFileName);
	WritePrivateProfileString("DIR_SET", "NormalDir", m_chNormalDir, chIniFileName);

	// 文件名信息
	sprintf(chTemp, "%d", m_FileNameInfo.iBigImgEnable);
	WritePrivateProfileString("FILENAME_SET", "BigImgEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_FileNameInfo.iSmallImgEnable);
	WritePrivateProfileString("FILENAME_SET", "SmallImgEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_FileNameInfo.iBinEnable);
	WritePrivateProfileString("FILENAME_SET", "BinEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_FileNameInfo.iInfoEnable);
	WritePrivateProfileString("FILENAME_SET", "InfoEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_FileNameInfo.iPlateEnable);
	WritePrivateProfileString("FILENAME_SET", "PlateEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_FileNameInfo.iPlateNoEnable);
	WritePrivateProfileString("FILENAME_SET", "PlateNoEnable", chTemp, chIniFileName);

	WritePrivateProfileString("FILENAME_SET", "BigImgFileName", m_FileNameInfo.chBigImgFileName, chIniFileName);
	WritePrivateProfileString("FILENAME_SET", "SmallImgName", m_FileNameInfo.chSmallImgFileName, chIniFileName);
	WritePrivateProfileString("FILENAME_SET", "BinFileName", m_FileNameInfo.chBinFileName, chIniFileName);
	WritePrivateProfileString("FILENAME_SET", "InfoFileName", m_FileNameInfo.chInfoFileName, chIniFileName);
	WritePrivateProfileString("FILENAME_SET", "PlateFileName", m_FileNameInfo.chPlateFileName, chIniFileName);
	WritePrivateProfileString("FILENAME_SET", "PlateNoFileName", m_FileNameInfo.chPlateNoFileName, chIniFileName);

	// 日志开关
	sprintf(chTemp, "%d", m_iLogEnable);
	WritePrivateProfileString("LOG_SET", "LogEnable", chTemp, chIniFileName);

	// 压缩选项
	sprintf(chTemp, "%d", m_iCompressEnable);
	WritePrivateProfileString("COMPRESS_SET", "CompressEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_iCompressQuality);
	WritePrivateProfileString("COMPRESS_SET", "CompressQuality", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_iCompressSubQuality);
	WritePrivateProfileString("COMPRESS_SET", "CompressSubQuality", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_iCompressSize/1024);
	WritePrivateProfileString("COMPRESS_SET", "CompressSize", chTemp, chIniFileName);

	// 图像处理开关
	sprintf(chTemp, "%d", m_iImgProcessEnable);
	WritePrivateProfileString("JPG_SET", "ImgProcessEnable", chTemp, chIniFileName);
	
	// 图片保存模式 0为按原图分辨率保存  1为按配置分辨率保存
	sprintf(chTemp, "%d", m_iImgMode);
	WritePrivateProfileString("JPG_SET", "ImgMode", chTemp, chIniFileName);

	// 图片大小
	sprintf(chTemp, "%d", m_iBigImgWidth);
	WritePrivateProfileString("JPG_SET", "BigImgWidth", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_iBigImgHeight);
	WritePrivateProfileString("JPG_SET", "BigImgHeight", chTemp, chIniFileName);

	// 字体大小
	sprintf(chTemp, "%d", m_iFontSize);
	WritePrivateProfileString("JPG_SET", "FontSize", chTemp, chIniFileName);

	int r, g, b;
	// 字体颜色
	r = (BYTE)(m_dwFontColor);
	g = (BYTE)(m_dwFontColor>> 8 & 0xFF);
	b = (BYTE)(m_dwFontColor>> 16 & 0xFF);
	sprintf(chTemp, "%d,%d,%d", (int)r, (int)g, (int)b);
	WritePrivateProfileString("JPG_SET", "FontColor", chTemp, chIniFileName);
	// 背景颜色
	r = (BYTE)(m_dwFontEdgeColor);
	g = (BYTE)(m_dwFontEdgeColor>> 8 & 0xFF);
	b = (BYTE)(m_dwFontEdgeColor>> 16 & 0xFF);
	sprintf(chTemp, "%d,%d,%d", (int)r, (int)g, (int)b);
	WritePrivateProfileString("JPG_SET", "FontEdgeColor", chTemp, chIniFileName);

	// 车身截图选项
	sprintf(chTemp, "%d", m_iFeatureEnable);
	WritePrivateProfileString("FeatureInfo", "FeatureEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_iFeatureWidth);
	WritePrivateProfileString("FeatureInfo", "FeatureWidth", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_iFeatureHeigh);
	WritePrivateProfileString("FeatureInfo", "FeatureHeigh", chTemp, chIniFileName);

	LeaveCriticalSection(&g_csLog);

	return 0;
}

// 写历史结果ini文件
int CCamera::WriteHistoryIniFile()
{
	TCHAR fileName[MAX_PATH];
	GetModuleFileName(NULL, fileName, MAX_PATH-1);
	PathRemoveFileSpec(fileName);
	TCHAR chIniFileName[MAX_PATH] = { 0 };
	strcpy(chIniFileName, fileName);
	strcat(chIniFileName, _T("\\SafeModeConfig.ini"));

	TCHAR chTemp[256] = {0};
	sprintf(chTemp, "%d", m_safeModeInfo.iEanbleSafeMode);
	if (m_safeModeInfo.iEanbleSafeMode == 0 )
	{
		CTime tm = CTime::GetCurrentTime();
		sprintf(m_safeModeInfo.szBeginTime, "%s", tm.Format("%Y.%m.%d_%H"));
	}

	EnterCriticalSection(&g_csLog);

	WritePrivateProfileString(m_strDevSN, "SafeModeEnable", chTemp, chIniFileName);
	WritePrivateProfileString(m_strDevSN, "BeginTime", m_safeModeInfo.szBeginTime, chIniFileName);
	WritePrivateProfileString(m_strDevSN, "EndTime", m_safeModeInfo.szEndTime, chIniFileName);
	sprintf(chTemp, "%d", m_safeModeInfo.index);
	WritePrivateProfileString(m_strDevSN, "Index", chTemp, chIniFileName);
	sprintf(chTemp, "%d", m_safeModeInfo.DataInfo);
	WritePrivateProfileString(m_strDevSN, "DataType", chTemp, chIniFileName);

	LeaveCriticalSection(&g_csLog);

	return 0;
}

// 识别结果开始回调函数
int CCamera::RecordInfoBegin(DWORD dwCarID)
{
	if ( m_HvResult != NULL )
	{
		delete m_HvResult;
		m_HvResult = NULL;
	}

	m_HvResult = new Result();

	return 0;
}

// 识别结果结束回调函数
int CCamera::RecordInfoEnd(DWORD dwCarID)
{
	if (m_HvResult == NULL)
		return -1;

	if ( m_safeModeInfo.iEanbleSafeMode == 1 )
	{
		m_safeModeInfo.index = m_HvResult->dwCarID;
		CTime tm(m_HvResult->dwTime/1000);
		sprintf(m_safeModeInfo.szBeginTime, "%s", tm.Format("%Y.%m.%d_%H"));

		m_iBreakCount++;

		if (m_iBreakCount > 3)
		{
			TCHAR chFileName[MAX_PATH];
			GetModuleFileName(NULL, chFileName, MAX_PATH-1);
			PathRemoveFileSpec(chFileName);
			TCHAR chIniFileName[MAX_PATH] = { 0 };
			strcpy(chIniFileName, chFileName);
			strcat(chIniFileName,  _T("\\SafeModeConfig.ini"));

			// 更新历史信息
			EnterCriticalSection(&g_csLog);

			char chTemp[256] = {0};
			WritePrivateProfileString(m_strDevSN, "BeginTime", m_safeModeInfo.szBeginTime, chIniFileName);
			sprintf(chTemp, "%d", m_safeModeInfo.index);
			WritePrivateProfileString(m_strDevSN, "Index", chTemp, chIniFileName);

			LeaveCriticalSection(&g_csLog);

			m_iBreakCount = 0;
		}
	}


	// 添加结果入结果队列
	EnterCriticalSection(&m_csResult);
	if (m_HvResultList.GetCount() < MAX_LIST_COUNT)
	{
		m_HvResultList.AddTail(m_HvResult);
		m_HvResult = NULL;
	}
	LeaveCriticalSection(&m_csResult);

	return 0;
}

// 车牌信息回调
int CCamera::GetPlate(DWORD dwCarID, 
						LPCSTR pcPlateNo, 
						LPCSTR pcAppendInfo,
						DWORD dwRecordType,
						DWORD64 dw64TimeMS)
{
	if (  m_HvResult == NULL )
		return -1;

	CString szPlate = pcPlateNo;
	if ( szPlate.Find("无车牌") == -1 )
	{
		strcpy(m_HvResult->chPlate, szPlate.Mid(2));
		strcpy(m_HvResult->chPlateColor, szPlate.Left(2));
	}
	else
	{
		strcpy(m_HvResult->chPlate, "无车牌");
		strcpy(m_HvResult->chPlateColor, "其他");
	}

	if ( strstr(pcPlateNo, "无车牌") != NULL)
		strcpy(m_HvResult->chPlate, "拒识无牌");

	m_HvResult->dwCarID = dwCarID;
	m_HvResult->dwTime = dw64TimeMS;

	// 解析附加信息
	ResolveXmlData(pcAppendInfo);

	// 解析成普通inf文件
	if (pcAppendInfo != NULL)
	{
		HVAPIUTILS_ParsePlateXmlStringEx(pcAppendInfo, m_HvResult->chAppendInfo, 2048);
	}

	return 0;
}

// 大图回调
int CCamera::GetBigImg(DWORD dwCarID, 
						WORD  wImgType,
						WORD  wWidth,
						WORD  wHeight,
						PBYTE pbPicData,
						DWORD dwImgDataLen,
						DWORD dwRecordType,
						DWORD64 dw64TimeMS)
{
	if (  m_HvResult == NULL )
		return -1;

	if (wImgType <= 0)
		wImgType = 1;
	if (wImgType > MAX_BIG_COUNT)
		wImgType = MAX_BIG_COUNT;

	LARGE_INTEGER liTemp = { 0 };
	ULARGE_INTEGER uliTemp = { 0 };

	int index = wImgType -1;

	if (m_HvResult->BigImgStream[index] == NULL)
	{
		CreateStreamOnHGlobal(NULL, TRUE, &m_HvResult->BigImgStream[index]);
	}

	if ( m_HvResult->BigImgStream[index] != NULL )
	{
		m_HvResult->BigImgStream[index]->SetSize(uliTemp);
		m_HvResult->BigImgStream[index]->Seek(liTemp, STREAM_SEEK_SET, NULL);
		m_HvResult->BigImgStream[index]->Write(pbPicData, dwImgDataLen, NULL);
		m_HvResult->dwBigImgSize[index] = dwImgDataLen;
		m_HvResult->dwImageTimeMs[index] = dw64TimeMS;
		m_HvResult->iImageCount++;
	}

	m_HvResult->BigCarID = dwCarID;

	// 获取车牌坐标
	// 数组a[0]为左上角y坐标a[1]为左上角x坐标
	// 数组a[2]为右下角y坐标a[3]为右下角x坐标
	int a[4] = {0};
	int iBuffLen = sizeof(a);
	HVAPI_GetExtensionInfoEx(m_hHv, wImgType, a, &iBuffLen);

	// 当返回的坐标是错的时取自定义坐标
	if (a[0]>100 || a[1]>100 || a[2]>100 || a[3]>100
		|| 0 == a[0] || 0 == a[1] || 0 == a[2] || 0 == a[3])
	{
		a[0] = 50;
		a[1] = 50;
		a[2] = 50;
		a[3] = 50;
	}

	m_HvResult->rcPlatePos[index].left  = (long)(a[1] * wWidth) / 100;  //x
	m_HvResult->rcPlatePos[index].right = (long)(a[3] * wWidth) / 100; //x

	m_HvResult->rcPlatePos[index].top = (long)(a[0] * wHeight) / 100; //y
	m_HvResult->rcPlatePos[index].bottom = (long)(a[2] * wHeight) / 100; //y

	return 0;
}

// 车牌小图回调
int CCamera::GetSmallImg(DWORD dwCarID,
							WORD wWidth,
							WORD wHeight,
							PBYTE pbPicData,
							DWORD dwImgDataLen,
							DWORD dwRecordType,
							DWORD64 dw64TimeMS)
{
	if ( m_HvResult == NULL )
		return -1;

	if ( m_HvResult->pbSmallImg != NULL )
	{
		delete[] m_HvResult->pbSmallImg;
		m_HvResult->pbSmallImg = NULL;
	}

	int iSmallLen = 100 * 1024;
	m_HvResult->pbSmallImg = new BYTE[iSmallLen];

	if ( m_HvResult->pbSmallImg != NULL  )
	{
		int iSmallBmpLen = iSmallLen;
		memset(m_HvResult->pbSmallImg, 0, iSmallLen);
		if ( HVAPIUTILS_SmallImageToBitmapEx(pbPicData, wWidth, wHeight, m_HvResult->pbSmallImg, &iSmallBmpLen) != S_OK )
		{
			WriteLog("转化小图失败！");
		}
		m_HvResult->dwSmallImgSize = iSmallBmpLen;
		m_HvResult->dwSmallImgTime = dw64TimeMS;
	}
	return 0;
}

// 车牌二值图回调
int CCamera::GetBinImg(DWORD dwCarID,
						WORD wWidth,
						WORD wHeight,
						PBYTE pbPicData,
						DWORD dwImgDataLen,
						DWORD dwRecordType,
						DWORD64 dw64TimeMS)
{
	if (m_HvResult->pbBinImage != NULL)
	{
		delete[] m_HvResult->pbBinImage;
		m_HvResult->pbBinImage = NULL;
	}
	m_HvResult->pbBinImage = new BYTE[dwImgDataLen];
	m_HvResult->dwBinSize = 0;
	if (m_HvResult->pbBinImage != NULL)
	{
		memcpy(m_HvResult->pbBinImage, pbPicData, dwImgDataLen);
		m_HvResult->dwBinSize = dwImgDataLen;
	}

	return 0;
}

// 写日志
int CCamera::WriteLog(char* chText)
{
	if (chText == NULL)
		return -1;

	FILE *file = NULL;
	CTime tm = CTime::GetCurrentTime();
	CString strText;
	strText.Format("%s [%s]:%s\n", tm.Format("%Y-%m-%d %H:%M:%S"), m_strDevSN, chText);

	EnterCriticalSection(&g_csLog);

	if (m_iLogEnable != 0)
	{
		file = fopen("Log.txt", "a+");
		if (file != NULL)
		{
			fprintf(file, "%s", strText.GetBuffer(strText.GetLength()));
			strText.ReleaseBuffer();
			fclose(file);
		}
		file = NULL;
	}

	LeaveCriticalSection(&g_csLog);
	
	return 0;
}

//对XML的附加信息进行解析
int CCamera::ResolveXmlData(LPCSTR szResultInfo)
{
	if (szResultInfo == NULL )
		return -1;

	// 创建解析文档
	TiXmlDocument myDocument;
	if ( !myDocument.Parse(szResultInfo))
	{
		OutputDebugString( " !myDocument.Parse(szResultInfo) \n");
		return -1;
	}

	// 文档节点
	TiXmlElement* pRoot = myDocument.RootElement();
	if (pRoot ==  NULL)
		return -1;

	// 结果集
	TiXmlElement* pResultSet = pRoot->FirstChildElement();
	if ( pResultSet == NULL)
		return -1;

	// 结果
	TiXmlElement* pResultElement = pResultSet->FirstChildElement();
	if ( pResultElement == NULL )
		return -1;

	// 解析结果
	TiXmlElement* pEle = NULL;
	CString strTemp = _T("");
	const char *chElement = NULL;

	for (pEle = pResultElement->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())
	{
		if ( strcmp(pEle->Value(), "VideoScaleSpeed") == 0
			|| strcmp(pEle->Value(), "RadarScaleSpeed") == 0
			|| strcmp(pEle->Value(), "CoilScaleSpeed") == 0) // 视频、雷达、线圈测速
		{
			chElement = pEle->Attribute("value");
			strTemp = chElement;
			strTemp.Replace("Km/h", "");
			m_HvResult->iSpeed = atoi(strTemp);
		}
		else if ( strcmp(pEle->Value(), "SpeedLimit") == 0 ) // 限速值
		{
			chElement = pEle->Attribute("value");
			strTemp = chElement;
			strTemp.Replace("Km/h", "");
			m_HvResult->iLimitSpeed = atoi(strTemp);
		}
		else if ( strcmp(pEle->Value(), "StreetName") == 0 )  // 路口名称
		{
			chElement = pEle->Attribute("value");
			strTemp = chElement;
			strcpy(m_HvResult->chRoadName, strTemp);
		}
		else if ( strcmp(pEle->Value(), "StreetDirection") == 0 ) // 路口方向
		{
			chElement = pEle->Attribute("value");
			strTemp = chElement;
			strcpy(m_HvResult->chRoadDirection, strTemp);
		}
		else if ( strcmp(pEle->Value(), "RoadNumber") == 0 ) // 车道号
		{
			chElement = pEle->Attribute("value");
			strTemp = chElement;
			m_HvResult->iLand = atoi(strTemp);
		}
		else if ( strcmp(pEle->Value(), "CarType")==0 ) // 车辆类型
		{
			chElement = pEle->Attribute("value");
			strTemp = chElement;
			strcpy(m_HvResult->chCarType, strTemp);
		}
		else if ( strcmp(pEle->Value(), "EventCheck") == 0 ) // 事件检测
		{
			chElement = pEle->Attribute("value");
			strTemp = chElement;

			// 对信息进行解析
			BOOL fIsPeccancy = FALSE;
			//BOOL m_fIsRetrograde = FALSE;
			const char *pPeccancyType = strstr(strTemp, "违章:是");
			char szValue[256] = { 0 };
			CString strPeccancyType = _T("");

			if(pPeccancyType != NULL)
			{
				fIsPeccancy = TRUE;
				sscanf(pPeccancyType, "违章:是<%s>\n", szValue);
				strPeccancyType.Format("%s", szValue);
				// 判断最后一个字符是否是">"，若是便去掉
				CString strTemp = strPeccancyType.Mid(strPeccancyType.GetLength() - 1, 1);
				if (strcmp(strTemp, ">") == 0)
				{
					strTemp = strPeccancyType.Mid(0, strPeccancyType.GetLength() - 1);
					strPeccancyType = strTemp;
				}
				// 判断最后一个字符是否是","，若是便去掉
				strTemp = strPeccancyType.Mid(strPeccancyType.GetLength() - 1, 1);
				if (strcmp(strTemp, ",") == 0)
				{
					strTemp = strPeccancyType.Mid(0, strPeccancyType.GetLength() - 1);
					strPeccancyType = strTemp;
				}
				//strPeccancyType.Replace(">","");
			}
			
			// 非违章
			m_HvResult->iIllegalType = 0;
			strcpy(m_HvResult->chIllegalInfo, "正常");

			// 违章
			if (fIsPeccancy)
			{
				if (strPeccancyType.Find("闯红灯") != -1)
				{
					strcpy(m_HvResult->chIllegalInfo, "闯红灯");
					m_HvResult->iIllegalType = 3;
					// 红灯开始时间
					char szReadTemp[256] = { 0 };
					char *pRedTime = (char*)strstr((const char*)strTemp, "红灯时间:");
					if (pRedTime)
					{
						sscanf(pRedTime, "红灯时间:%d:%d:%d:%d\n", &m_HvResult->iH, &m_HvResult->iM, &m_HvResult->iS, &m_HvResult->iSS);
						m_HvResult->iRedTimeMS = m_HvResult->iH*3600000 + m_HvResult->iM*60000 + m_HvResult->iS*1000 + m_HvResult->iSS;
					}
				}
				else if (strPeccancyType.Find("逆行") != -1)
				{
					strcpy(m_HvResult->chIllegalInfo, "逆行");
					m_HvResult->iIllegalType = 5;
				}
				else if (strPeccancyType.Find("超速") != -1)
				{
					strcpy(m_HvResult->chIllegalInfo, "超速");
					m_HvResult->iIllegalType = 2;
				}
				else if (strPeccancyType.Find("不按车道行驶") != -1)
				{
					strcpy(m_HvResult->chIllegalInfo, "不按车道行驶");
					m_HvResult->iIllegalType = 4;

				}
				else if ( strPeccancyType.Find("压线") != -1 )
				{
					strcpy(m_HvResult->chIllegalInfo, "压线");
					m_HvResult->iIllegalType = 6;
				}
				else if (strPeccancyType.Find("禁行") != -1)  //黄牌违章
				{
					strcpy(m_HvResult->chIllegalInfo, "禁行");
					m_HvResult->iIllegalType = 7;
				}
				else if (strPeccancyType.Find("非机动车道") != -1)
				{
					strcpy(m_HvResult->chIllegalInfo, "非机动车道");
					m_HvResult->iIllegalType = 8;
				}
				else
				{
					strcpy(m_HvResult->chIllegalInfo, "其它");
					m_HvResult->iIllegalType = 9;
				}
			}

			// 计算超速比
			if (m_HvResult->iSpeed > m_HvResult->iLimitSpeed)
			{
				m_HvResult->fOverSpeedRate = (float)(m_HvResult->iSpeed - m_HvResult->iLimitSpeed) * 100 / m_HvResult->iLimitSpeed;
			}
			else
			{
				m_HvResult->fOverSpeedRate = 0;
			}

		}
	}
	return 0;
}

float CCamera::GetRedTime(int iRedTime, DWORD64 dwCarTime)
{

	CTime tm(dwCarTime/1000);
	int iMS = (int)dwCarTime%1000;
	int iH, iM, iS;
	sscanf(tm.Format("%H:%M:%S"), "%d:%d:%d", &iH, &iM, &iS);

	int iCarTimeTemp = iH*3600000 + iM*60000 + iS*1000 + iMS;

	if (iCarTimeTemp < iRedTime)
		return 0;

	int iTime = iCarTimeTemp - iRedTime;

	return (float)iTime / 1000;
}

// 计算截图位置大小
int CCamera::InflateCropRect(RECT& rectDst,
	const RECT &cPlatePos,
	const int& iImgWidth,
	const int& iImgHeight,
	const int& iCropWidth,
	const int& iCropHeight
	)
{
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

	iTemp = iCropHeight - iPosHeigh;
	iTemp >>= 2;
	int iTopDst = cPlatePos.top - (iTemp * 3);
	int iBottomDst = cPlatePos.bottom + iTemp;
	iBottomDst =  iBottomDst + (iCropHeight - (iBottomDst - iTopDst));

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
	return 0;
}

// 保存车辆信息
int CCamera::SaveResult(Result* pHvResult)
{
	if (NULL == pHvResult)
	{
		return -1;
	}
	if (m_safeModeInfo.iEanbleSafeMode == 1 && m_safeModeInfo.DataInfo == 1)
	{
		if (pHvResult->iIllegalType == 0)
			return 0;
	}

	// 字符替换预置
	ReplaceInfo replaceInfo;
	replaceInfo.dwCarIndex = pHvResult->dwCarID;
	replaceInfo.dwTimeMs = pHvResult->dwTime;
	replaceInfo.iLimitSpeed = pHvResult->iLimitSpeed;
	replaceInfo.iSpeed = pHvResult->iSpeed;
	replaceInfo.iRoadNumber = pHvResult->iLand;
	replaceInfo.strPlateNo = pHvResult->chPlate;
	replaceInfo.strDeviceIP = m_strDevIP;
	replaceInfo.strDeviceNo = m_strDevSN;
	replaceInfo.strIllegalInfo = pHvResult->chIllegalInfo;
	replaceInfo.strPlateColor = pHvResult->chPlateColor;
	replaceInfo.strCarType = pHvResult->chCarType;
	replaceInfo.strStreetName = pHvResult->chRoadName;
	replaceInfo.strStreetDirection = pHvResult->chRoadDirection;

	// 路径替换
	CString strPath = _T("");
	CString strTempPath = _T("");
	if (pHvResult->iIllegalType != 0)
	{
		strTempPath.Format("%s%s", m_strDir, m_chIllegalDir);
	}
	else
	{
		strTempPath.Format("%s%s", m_strDir, m_chNormalDir);
	}
	strPath = m_Common.GetTrueString(strTempPath, replaceInfo);

	// 保存大图
	if (m_FileNameInfo.iBigImgEnable != 0)
	{
		CString strFile = _T("");
		CString strTempFile = _T("");
		strTempFile.Format("%s%s", strPath, m_FileNameInfo.chBigImgFileName);
		
		int iImageIndex[MAX_BIG_COUNT] = {-1, -1, -1, -1, -1};
		int iPos = 2;
		int iImageCount = 0;

		for (int i=MAX_BIG_COUNT-1; i>=0; i--)
		{
			if ( iImageCount == 3 )
				break ;

			if (pHvResult->dwBigImgSize[i] != 0)
			{
				iImageCount++;
				iImageIndex[iPos] = i;
				if ( iPos == 0 )
					break ;
				iPos--;
			}
		}

		if (pHvResult->iIllegalType != 0 )
		{
			if (1 == m_iDevType && iImageCount < 3)
			{
				WriteLog("电警：图片数量不足三张！");
				return -1;
			}
			else if (0 == m_iDevType && iImageCount < 2)
			{
				WriteLog("卡口：图片数量不足两张！");
				return -1;
			}
		}

		replaceInfo.iBigImageCount = iImageCount;

		// 循环每张图
		for (int i = 0; i < MAX_BIG_COUNT; i++)
		{
			if (iImageIndex[i] == -1)
			{
				continue;
			}
			int iIndex = 0;
			iIndex = iImageIndex[i];
			if (pHvResult->BigImgStream[iIndex] == NULL)
			{
				continue;
			}

			IStream* pStreamOut = NULL;
			if (m_iImgProcessEnable != 0)
			{
				if (CreateStreamOnHGlobal(NULL, TRUE, &pStreamOut) != S_OK)
				{
					WriteLog("申请内存失败！");
					break;
				}
				CTime tm(pHvResult->dwImageTimeMs[iIndex]/1000);
				int iTemp = (int)(pHvResult->dwImageTimeMs[iIndex] % 1000);
				CString strText = _T("");
				if (pHvResult->iIllegalType == 3) // 闯红灯
				{
					strText.Format("时间:%s:%03d  地点:%s  拍摄方向:%s  红灯后%.3f秒", tm.Format("%Y-%m-%d %H:%M:%S"), iTemp,
						pHvResult->chRoadName, pHvResult->chRoadDirection, GetRedTime(pHvResult->iRedTimeMS, pHvResult->dwImageTimeMs[iIndex]));
				}
				else if (pHvResult->iIllegalType == 2) // 超速
				{
					strText.Format("时间:%s:%03d  地点:%s  拍摄方向:%s  限速:%d Km/h  速度:%d Km/h 超速比:%.2f%%", tm.Format("%Y-%m-%d %H:%M:%S"), iTemp,
						pHvResult->chRoadName, pHvResult->chRoadDirection, pHvResult->iLimitSpeed, pHvResult->iSpeed, pHvResult->fOverSpeedRate);
				}
				else
				{
					strText.Format("时间:%s:%03d  地点:%s  拍摄方向:%s", tm.Format("%Y-%m-%d %H:%M:%S"), iTemp,
						pHvResult->chRoadName, pHvResult->chRoadDirection);
				}
				ImgProcess(pHvResult, pHvResult->BigImgStream[iIndex], iIndex, strText, pStreamOut); // 图片处理
			}
			else
			{
				pStreamOut = pHvResult->BigImgStream[iIndex];
				pHvResult->BigImgStream[iIndex] = NULL; // 避免被多次释放
			}

			int iLastSize = 0;
			ULARGE_INTEGER uiLength;
			if (m_Common.GetStreamLength(pStreamOut, &uiLength))
			{
				iLastSize = (int)uiLength.QuadPart;
			}

			BYTE* pByte = new BYTE[iLastSize+1];
			if (NULL == pByte)
			{
				WriteLog("内存申请失败！");
				return -1;
			}
			m_Common.IStreamToByteArray(pStreamOut, pByte, iLastSize);

			if (m_iFeatureEnable != 0) // 沈阳中通命名格式，第二张抓拍图为首张
			{
				switch(iIndex)
				{
				case 0:
					replaceInfo.strBigImageID = "a";
					replaceInfo.iBigImageID = 1;
					break;
				case 1:
					replaceInfo.strBigImageID = "b";
					replaceInfo.iBigImageID = 2;
					break;
				case 2:
					replaceInfo.strBigImageID = "b";
					replaceInfo.iBigImageID = 2;
					break;
				case 3:
					replaceInfo.strBigImageID = "a";
					replaceInfo.iBigImageID = 1;
					break;
				case 4:
					replaceInfo.strBigImageID = "c";
					replaceInfo.iBigImageID = 3;
					break;
				default:
					replaceInfo.strBigImageID = "a";
					replaceInfo.iBigImageID = 1;
					break;
				}
			}
			else // 普通模式
			{
				switch(i)
				{
				case 0:
					replaceInfo.strBigImageID = "a";
					replaceInfo.iBigImageID = 1;
					break;
				case 1:
					replaceInfo.strBigImageID = "b";
					replaceInfo.iBigImageID = 2;
					break;
				case 2:
					replaceInfo.strBigImageID = "a";
					replaceInfo.iBigImageID = 1;
					break;
				case 3:
					replaceInfo.strBigImageID = "b";
					replaceInfo.iBigImageID = 2;
					break;
				case 4:
					replaceInfo.strBigImageID = "c";
					replaceInfo.iBigImageID = 3;
					break;
				default:
					replaceInfo.strBigImageID = "a";
					replaceInfo.iBigImageID = 1;
					break;
				}
			}
			strFile = m_Common.GetTrueString(strTempFile, replaceInfo);

			if( !strFile.IsEmpty() && pByte != NULL)
			{
				// 创建目录
				if ( !PathFileExists(strPath) )
				{
					MakeSureDirectoryPathExists(strPath);
				}
				FILE* pfile = fopen(strFile, "wb");
				if( pfile != NULL )
				{
					fwrite(pByte, 1, iLastSize, pfile);
					fclose(pfile);
					pfile = NULL;
					WriteFileName(strFile);
				}
			}

			if (pByte != NULL)
			{
				delete[] pByte;
				pByte = NULL;
			}
			if (pStreamOut != NULL)
			{
				pStreamOut->Release();
				pStreamOut = NULL;
			}
		}
	}

	// 保存小图
	if (m_FileNameInfo.iSmallImgEnable != 0)
	{
		if ( !PathFileExists(strPath) )
		{
			MakeSureDirectoryPathExists(strPath);
		}

		CString strFile = _T("");
		CString strTempFile = _T("");
		strTempFile.Format("%s%s", strPath, m_FileNameInfo.chSmallImgFileName);
		strFile = m_Common.GetTrueString(strTempFile, replaceInfo);

		if( !strFile.IsEmpty() && pHvResult->pbSmallImg != NULL )
		{
			FILE* pfile = fopen(strFile, "wb");
			if( pfile != NULL )
			{
				fwrite(pHvResult->pbSmallImg, 1, pHvResult->dwSmallImgSize, pfile);
				fclose(pfile);
				pfile = NULL;
				WriteFileName(strFile);
			}
		}
	}

	// 保存二值图
	if (m_FileNameInfo.iBinEnable != 0)
	{
		if ( !PathFileExists(strPath) )
		{
			MakeSureDirectoryPathExists(strPath);
		}

		CString strFile = _T("");
		CString strTempFile = _T("");
		strTempFile.Format("%s%s", strPath, m_FileNameInfo.chBinFileName);
		strFile = m_Common.GetTrueString(strTempFile, replaceInfo);

		if( !strFile.IsEmpty() && pHvResult->pbBinImage != NULL )
		{
			FILE* pfile = fopen(strFile, "wb");
			if( pfile != NULL )
			{
				fwrite(pHvResult->pbBinImage, 1, pHvResult->dwBinSize, pfile);
				fclose(pfile);
				pfile = NULL;
				WriteFileName(strFile);
			}
		}
	}

	// 保存附加信息
	if (m_FileNameInfo.iInfoEnable != 0)
	{
		if ( !PathFileExists(strPath) )
		{
			MakeSureDirectoryPathExists(strPath);
		}

		CString strFile = _T("");
		CString strTempFile = _T("");
		strTempFile.Format("%s%s", strPath, m_FileNameInfo.chInfoFileName);
		strFile = m_Common.GetTrueString(strTempFile, replaceInfo);

		if( !strFile.IsEmpty() )
		{
			FILE* pfile = fopen(strFile, "a+");
			if( pfile != NULL )
			{
				fprintf(pfile, "%s\n", pHvResult->chAppendInfo);
				fclose(pfile);
				pfile = NULL;
				WriteFileName(strFile);
			}
		}
	}

	// 保存车牌信息
	if (m_FileNameInfo.iPlateEnable != 0)
	{
		if ( !PathFileExists(strPath) )
		{
			MakeSureDirectoryPathExists(strPath);
		}

		CString strFile = _T("");
		CString strTempFile = _T("");
		strTempFile.Format("%s%s", strPath, m_FileNameInfo.chPlateFileName);
		strFile = m_Common.GetTrueString(strTempFile, replaceInfo);

		if( !strFile.IsEmpty() )
		{
			FILE* pfile = fopen(strFile, "a+");
			if( pfile != NULL )
			{
				fprintf(pfile, "%s\n", pHvResult->chPlate);
				fclose(pfile);
				pfile = NULL;
				WriteFileName(strFile);
			}
		}
	}

	// 保存车牌汇总
	if (m_FileNameInfo.iPlateNoEnable != 0)
	{
		if ( !PathFileExists(strPath) )
		{
			MakeSureDirectoryPathExists(strPath);
		}

		CString strFile = _T("");
		CString strTempFile = _T("");
		strTempFile.Format("%s%s", strPath, m_FileNameInfo.chPlateNoFileName);
		strFile = m_Common.GetTrueString(strTempFile, replaceInfo);

		if( !strFile.IsEmpty() )
		{
			FILE* pfile = fopen(strFile, "a+");
			if( pfile != NULL )
			{
				fprintf(pfile, "%s\n", pHvResult->chPlate);
				fclose(pfile);
				pfile = NULL;
				WriteFileName(strFile);
			}
		}
	}

	return 0;
}

// 图片处理
int CCamera::ImgProcess(Result* pHvResult, IStream* pStream, int iImgIndex, CString strText, IStream* pStreamOut)
{
	int iRet = -1;
	if (pStream == NULL)
	{
		return iRet;
	}
	Bitmap bmp(pStream);
	///==============================字符叠加设置*开始===========================================
	StringFormat fmtString;
	fmtString.SetAlignment(StringAlignmentNear);
	fmtString.SetLineAlignment(StringAlignmentNear); 

	FontFamily fntFamily(L"宋体");
	PointF point(0, 0);
	int iSingleFontSize = m_iFontSize;
	
	// 字体颜色
	BYTE r, g, b;
	r = (BYTE)(m_dwFontColor);
	g = (BYTE)(m_dwFontColor>> 8 & 0xFF);
	b = (BYTE)(m_dwFontColor>> 16 & 0xFF);
	SolidBrush solidBrush(Color(255, r, g, b));

	// 背景颜色
	r = (BYTE)(m_dwFontEdgeColor);
	g = (BYTE)(m_dwFontEdgeColor>> 8 & 0xFF);
	b = (BYTE)(m_dwFontEdgeColor>> 16 & 0xFF);
	SolidBrush solidBrushBack(Color(255, r, g, b));

	int iImageWidth = bmp.GetWidth();
	int iImageHeight = bmp.GetHeight();
	if (m_iImgMode != 0)
	{
		iImageWidth = m_iBigImgWidth;
		iImageHeight = m_iBigImgHeight;
	}

	// 截图计算
	RECT rectFeature = {0};
	if (1 == m_iFeatureEnable && 3 == iImgIndex)  // 沈阳中通第二张抓拍图截取成车身图
	{
        if (0 == pHvResult->rcPlatePos[iImgIndex].left) // 未获取到坐标
        {
			rectFeature.left = 0;
            rectFeature.top = 0;
            rectFeature.right = bmp.GetWidth();
            rectFeature.bottom = bmp.GetHeight();
        }
        else
        {
			InflateCropRect(rectFeature, pHvResult->rcPlatePos[iImgIndex], bmp.GetWidth(), bmp.GetHeight(),
                    m_iFeatureWidth, m_iFeatureHeigh);
        }
		iImageWidth = rectFeature.right - rectFeature.left;
		iImageHeight = rectFeature.bottom - rectFeature.top;
	}
	RectF rfFeature(0, 0, (REAL)iImageWidth, (REAL)iImageHeight);

	// 根据图片大小自动改变字体大小防溢出并获取背景框大小
	CStringW wStr(strText);
	Graphics gr1(&bmp);
	int iFontWidth = 0;
	int iFontHeight = 0;
	while (true)
	{
		Gdiplus::Font fontTmp(&fntFamily, (REAL)iSingleFontSize, FontStyleBold);
		RectF outrect;
		gr1.MeasureString(wStr, -1, &fontTmp, point, &outrect); // 测量字体宽高，得出背景框
		if (outrect.Width > iImageWidth-1 && iSingleFontSize > 8 )
		{
			iSingleFontSize = iSingleFontSize - 2;
			continue;
		}

		iFontWidth = (int)outrect.Width;
		iFontHeight = (int)outrect.Height;
		break;
	}
	// 刷新字体框架
	Gdiplus::Font fontTmp(&fntFamily, (REAL)iSingleFontSize, FontStyleBold);
	// 设置背景框架位置大小
	RectF rfBack(0, (REAL)iImageHeight, (REAL)iImageWidth, (REAL)iFontHeight+10);
	// 设置字体位置矩形大小
	RectF rfDestMString(0, (REAL)iImageHeight+5, (REAL)iImageWidth, (REAL)iFontHeight+10);
	///==============================字符叠加设置*结束===========================================

	///===============================叠加字符*开始======================================
	Bitmap *pDest = NULL;
	pDest = new Bitmap(iImageWidth, iImageHeight + iFontHeight+ 10, bmp.GetPixelFormat());
	if (pDest == NULL)
	{
		return iRet ;
	}
	Graphics grMain(pDest);

	if (1 == m_iFeatureEnable && 3 == iImgIndex)  // 沈阳中通第二张抓拍图截取成车身图
    {
	    grMain.DrawImage(&bmp, rfFeature, (REAL)rectFeature.left, (REAL)rectFeature.top,
               (REAL)(rectFeature.right - rectFeature.left), (REAL)(rectFeature.bottom - rectFeature.top), UnitPixel);
	}
	else
	{
		grMain.DrawImage(&bmp, 0, 0, iImageWidth, iImageHeight);
	}
	// 粉刷背景
	grMain.FillRectangle(&solidBrushBack, rfBack);
	// 叠加字体
	grMain.DrawString(wStr, -1, &fontTmp, rfDestMString, &fmtString, &solidBrush);
	///==============================字符叠加设置*结束===========================================

	LARGE_INTEGER liTemp = { 0 };
	ULARGE_INTEGER uliZero = { 0 };
	pStreamOut->SetSize(uliZero);
	pStreamOut->Seek(liTemp, STREAM_SEEK_SET, NULL );

	int iLastSize = pHvResult->dwBigImgSize[iImgIndex];

	pDest->Save(pStreamOut, &m_jpgClsid, 0);
	ULARGE_INTEGER uiLength;
	if (m_Common.GetStreamLength(pStreamOut, &uiLength))
	{
		iLastSize = (int)uiLength.QuadPart;
	}

	// 压缩
	if (1 == m_iCompressEnable)
	{
		// 构造编码参数列表
		EncoderParameters encoderParameters;
		// 数组中只包含一个EncoderParameter对象    
		encoderParameters.Count = 1;    
		encoderParameters.Parameter[0].Guid = EncoderQuality;    
		// 参数类型为LONG
		encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;    
		// 只设置一个参数
		encoderParameters.Parameter[0].NumberOfValues = 1;  
		// 压缩质量 
		ULONG quality = m_iCompressQuality;  
		encoderParameters.Parameter[0].Value = &quality;

		while (iLastSize >= m_iCompressSize && quality > 0)
		{
			int iSub  = (int)(quality - m_iCompressSubQuality);
			if (iSub <= 0)
				quality = 0;
			else
				quality = iSub;

			encoderParameters.Parameter[0].Value = &quality;
			pStreamOut->Seek(liTemp, STREAM_SEEK_SET, NULL);
			pStreamOut->SetSize(uliZero);

			if (pDest->Save(pStreamOut, &m_jpgClsid, &encoderParameters) == Ok )
			{
				ULARGE_INTEGER uiLength;
				if (m_Common.GetStreamLength(pStreamOut, &uiLength))
				{
					iLastSize = (int)uiLength.QuadPart;
				}
			}
		}
	}

	pStreamOut->Seek(liTemp, STREAM_SEEK_SET, NULL);

	if (pDest != NULL)
	{
		delete pDest;
		pDest = NULL;
	}

	return 0;
}