#include "StdAfx.h"
#include ".\device.h"

#include "SaveH264Video.h"


CDevice::CDevice(void)
:m_iPort(0)
,m_handle(NULL)
,m_strSaveDir("D:\\Result\\")
,m_pVideoHandle(NULL)
{
	memset(m_szAddr, 0, 32);
	memset(m_szDeviceNo, 0, 64);
	memset(m_szDeviceInfo, 0,1024);

	m_startTime = CTime::GetCurrentTime();

		//初始化FFMPEG
	H264VideoSaverInit();

}

CDevice::~CDevice(void)
{

	//卸载FFMPEG 的环境
	if ( m_pVideoHandle != NULL )
	{
		H264VideoSaverClose(m_pVideoHandle);
		m_pVideoHandle = NULL;
	}	

	H264VideoSaverCoInit();

	m_mtHandle.Lock();
	if ( m_handle != NULL )
	{
		m_handle = NULL;
	}
	m_mtHandle.Unlock();
}

bool CDevice::GetDeviceXml()
{

	//设备已经下线，直接返回失败
	if ( m_devStatus == DISCONNECT)
	{
		return false;
	}

	m_mtHandle.Lock();

	bool fRet = false;
	int iLength = 1024 * 1024;
	char *szXmlBuff = new char[iLength];
	if ( HVAPI_AL_GetDevConfigInfo(m_handle, szXmlBuff, iLength, &iLength) == S_OK )
	{
		fRet = true;
		char szFileName[512] = { 0 };
		sprintf(szFileName, "%s.xml", m_szAddr);
		FILE *myFile = fopen(szFileName, "w");
		if ( myFile )
		{
			fwrite(szXmlBuff, 1,  iLength, myFile);
			fclose(myFile);
			myFile = NULL;
		}
	}

	delete[] szXmlBuff;
	szXmlBuff = NULL;

	m_mtHandle.Unlock();

	return fRet;
	
}

bool CDevice::SyncTime()
{
	//设备已经下线，直接返回失败
	if ( m_devStatus == DISCONNECT)
	{
		return false;
	}

	m_mtHandle.Lock();
	time_t t = time(NULL);
	struct tm *Tm = localtime(&t);
	char chTemp[1024] = {0};
	sprintf(chTemp, "SetTime,Date[%d.%02d.%02d],Time[%02d:%02d:%02d]", Tm->tm_year+1900, Tm->tm_mon+1, Tm->tm_mday, Tm->tm_hour, Tm->tm_min, Tm->tm_sec);

	char szRetBuf[1024] = {0};
	int nRetLen = 0;
	if ( m_handle != NULL )
	{
		if ( HVAPI_AL_ExecCmdOnDev(m_handle, chTemp, szRetBuf, 1024, &nRetLen) == S_OK)
		{
			m_mtHandle.Unlock();
			return true;
		}
	}

	m_mtHandle.Unlock();

	return false;
}

CString CDevice::GetDeviceTime()
{
	//设备已经下线，直接返回失败
	if ( m_devStatus == DISCONNECT)
	{
		return false;
	}

	CString strTime = _T("");
	char chTemp[1024] = {0};
	sprintf(chTemp, "DateTime");

	m_mtHandle.Lock();
	char szRetBuf[1024] = {0};
	int nRetLen = 0;
	if ( m_handle != NULL )
	{
		if ( HVAPI_AL_ExecCmdOnDev(m_handle, chTemp, szRetBuf, 1024, &nRetLen) == S_OK)
		{
			strTime = szRetBuf;
			return strTime;
		}
	}
	m_mtHandle.Unlock();

	return strTime;
}

bool CDevice::GetCaptureImage(int nTimeM, BYTE *pImageBuffLen, int iBuffLen, int *iImageLen, DWORD64 *dwTime, DWORD *dwWidth, DWORD *dwHeigh)
{
	//设备已经下线，直接返回失败
	if ( m_devStatus == DISCONNECT)
	{
		return false;
	}


	m_mtHandle.Lock();
	bool bRet = false;
	if ( m_handle != NULL )
	{

		if ( HVAPI_AL_GetCaptureImage(m_handle,nTimeM, pImageBuffLen, iBuffLen,iImageLen,dwTime, dwWidth, dwHeigh) == S_OK )
		{
			bRet = true;
		}
	}
	m_mtHandle.Unlock();

	return bRet;
}
/*
bool CDevice::GetPCSHumanTrafficInfo(DWORD64 dw64StartTime, DWORD64 dw64EndTime, CHAR* szRetInfo, INT* iLen)
{
	return true;
}*/


bool CDevice::StartVideo()
{
	//设备已经下线，直接返回失败
	if ( m_devStatus == DISCONNECT)
	{
		return false;
	}

	bool bRet = false;

	m_mtHandle.Lock();
	if ( m_handle != NULL )
	{

		if ( HVAPI_AL_StartRecvH264Video(m_handle, CDevice::H264CallBackProxy, this, 0,0,0,0) == S_OK)
		{
			bRet = true;
		}
		else
		{
			bRet =false;
		}
	}
	m_mtHandle.Unlock();

	return bRet;
}


INT  CDevice::H264CallBackProxy(
												PVOID pUserData,  
												DWORD dwVedioFlag,
												DWORD dwVideoType, 
												DWORD dwWidth,
												DWORD dwHeight,
												DWORD64 dw64TimeMS,
												PBYTE pbVideoData, 
												DWORD dwVideoDataLen,
												LPCSTR szVideoExtInfo
												)
{
	
	//OutputDebugString(szVideoExtInfo);
	//OutputDebugString("\n");

	if ( pUserData == NULL )
		return 0;

	CDevice *pThis = (CDevice*) pUserData;

	return pThis->OnH264(dwVedioFlag, dwVideoType, dwWidth,dwHeight,dw64TimeMS,pbVideoData, dwVideoDataLen, szVideoExtInfo);

}


int CDevice::OnH264(DWORD dwVedioFlag,
												DWORD dwVideoType, 
												DWORD dwWidth,
												DWORD dwHeight,
												DWORD64 dw64TimeMS,
												PBYTE pbVideoData, 
												DWORD dwVideoDataLen,
												LPCSTR szVideoExtInfo)
{

	return 0;

	//对H264视频进行保存
	CTime tm = CTime::GetCurrentTime();


	if (  H264CheckDistSpace("D:\\") < 1024 )
		return 0;

	
	if (pbVideoData == NULL || dwVideoDataLen == 0 )
		return 0;

	CTimeSpan ts = tm - m_startTime;

	if ( ts.GetTotalMinutes() >=5 ) //大于指定的间隔,换文件保存
	{
		if ( m_pVideoHandle != NULL )
		{
			H264VideoSaverClose(m_pVideoHandle);
			m_pVideoHandle = NULL;
		}	
	}
	
	// 提取视频的长和宽
	int iWidth = dwWidth;
	int iHeight = dwHeight;
	
	//生成新的视频文件
	if ( m_pVideoHandle == NULL )
	{
		CString strDir = _T("");
		strDir.Format("%s\\H264\\%s\\%s\\%s\\",m_strSaveDir, m_szAddr, tm.Format("%Y%m%d"), tm.Format("%H"));
		if ( !PathFileExists(strDir) )
		{
			MakeSureDirectoryPathExists(strDir);
			//如果尚不存在，则保存到相应的目录中
		}

		CString strFileName;
		strFileName.Format("%s%s.avi",strDir, tm.Format("%Y%m%d%H%M%S"));
		m_pVideoHandle = H264VideoSaverOpen(strFileName, 14, iWidth, iHeight);
		if ( m_pVideoHandle != NULL )
		{
			m_startTime = tm;
		}
	}


	if ( m_pVideoHandle != NULL )
	{
		//保存视频
		if ( dwVideoType ==VIDEO_TYPE_H264_NORMAL_I ||  dwVideoType == VIDEO_TYPE_H264_HISTORY_I )
			H264VideoSaverWirteOneFrame(m_pVideoHandle, pbVideoData, dwVideoDataLen, true);
		else
			H264VideoSaverWirteOneFrame(m_pVideoHandle, pbVideoData, dwVideoDataLen, false);

	}
	
	return 0;
}




int CDevice::OnJpeg(DWORD dwImageFlag,
											DWORD dwImageType, 
											DWORD dwWidth,
											DWORD dwHeight,
											DWORD64 dw64TimeMS,
											PBYTE pbImageData, 
											DWORD dwImageDataLen,
											LPCSTR szImageExtInfo)
{


	return 0;

	if (  H264CheckDistSpace("D:\\") < 1024 )
		return 0;


		if ( pbImageData == NULL )
		{
			return 0;
		}

	
		CTime tm(dw64TimeMS/1000);
		DWORD ms =  dw64TimeMS % 1000;
	
		CString strDir = _T("");
		strDir.Format("%s\\Jpeg\\%s\\%s\\%s\\",m_strSaveDir, m_szAddr, tm.Format("%Y%m%d"), tm.Format("%H"));
		if ( !PathFileExists(strDir) )
		{
			MakeSureDirectoryPathExists(strDir);
			//如果尚不存在，则保存到相应的目录中
		}



		CString strFileName;
		strFileName.Format("%s%s%3d.jpeg.",strDir, tm.Format("%Y%m%d%H%M%S"),  ms);

		FILE *myFile = fopen(strFileName, "wb");
		if ( myFile )
		{
			fwrite(pbImageData, 1, dwImageDataLen, myFile);
			fclose(myFile);
			myFile = NULL;
		}

	return 0;
}




bool CDevice::StopVideo()
{
	//设备已经下线，直接返回失败
	if ( m_devStatus == DISCONNECT)
	{
		return false;
	}

	bool bRet = false;

	m_mtHandle.Lock();
	if ( m_handle != NULL )
	{

		if ( HVAPI_AL_StopRecvH264Video(m_handle) == S_OK )
		{
			bRet = true;
		}
		else
		{
			bRet =false;
		}
	}

	m_mtHandle.Unlock();

	return bRet;
}


INT CDevice::JPEGCallBackProxy(
											PVOID pUserData,  
											DWORD dwImageFlag,
											DWORD dwImageType, 
											DWORD dwWidth,
											DWORD dwHeight,
											DWORD64 dw64TimeMS,
											PBYTE pbImageData, 
											DWORD dwImageDataLen,
											LPCSTR szImageExtInfo
											)
 {

	 /*
	 OutputDebugString(szImageExtInfo);
	 OutputDebugString("\n");
	 CString strFileName ;
	 strFileName.Format("D:\\result\\%ld.jpeg", dw64TimeMS);;
	FILE *myFile =fopen(strFileName, "wb");
	if (myFile)
	{
		fwrite(pbImageData, 1, dwImageDataLen, myFile);
		fclose(myFile);
		myFile = NULL;
	}*/

	 if ( pUserData == NULL )
		return 0;
	 
	 CDevice *pThis = (CDevice*) pUserData;
	 return pThis->OnJpeg(dwImageFlag, dwImageType, dwWidth, dwHeight,dw64TimeMS,pbImageData, dwImageDataLen,szImageExtInfo);

 }

 bool CDevice::StartJpegVideo()
 {
	 //设备已经下线，直接返回失败
	if ( m_devStatus == DISCONNECT)
	{
		return false;
	}

	m_mtHandle.Lock();
	
	 bool bRet = false;

	if ( m_handle != NULL )
	{

		if ( HVAPI_AL_StartRecvMJPEG(m_handle, CDevice::JPEGCallBackProxy, this, 0,0) == S_OK)
		{
			bRet = true;
		}
		else
		{
			bRet =false;
		}
	}

	m_mtHandle.Unlock();

	return bRet;

 }

bool CDevice::StopJpegVideo()
{
	//设备已经下线，直接返回失败
	if ( m_devStatus == DISCONNECT)
	{
		return false;
	}

	bool bRet = false;

	m_mtHandle.Lock();
	if ( m_handle != NULL )
	{

		if ( HVAPI_AL_StopRecvMJPEG(m_handle) == S_OK )
		{
			bRet = true;
		}
		else
		{
			bRet =false;
		}
	}

	m_mtHandle.Unlock();

	return bRet;
}

bool CDevice::UpLoadDeviceXml(CString strFileName)
{
	//设备已经下线，直接返回失败
	if ( m_devStatus == DISCONNECT)
	{
		return false;
	}


	bool bRet = false;

	m_mtHandle.Lock();

	if ( m_handle != NULL )
	{
		int xmlLen = 1024 * 1024;
		char *szXml = new char[xmlLen];
		memset(szXml, 0,  xmlLen);

		FILE *myFile  =fopen(strFileName, "r");
		if ( myFile != NULL )
		{
			fread(szXml,1,xmlLen, myFile);
			

			if ( HVAPI_AL_UploadDevConfigInfo(m_handle, szXml) == S_OK)
			{
				bRet = true;

			}
			else
			{
				bRet =false;
			}
			fclose(myFile);
			myFile = NULL;
			
		}

		
		delete[] szXml;
		szXml = NULL;
	}
	m_mtHandle.Unlock();

	return bRet;
}


bool CDevice::GetHumanTrafficInfo(DWORD64 dw64StartTime, DWORD64 dw64EndTime, char *szRetInfo, int *iLen)
{

	//设备已经下线，直接返回失败
	if ( m_devStatus == DISCONNECT)
	{
		return false;
	}

	bool bRet = false;

	if ( szRetInfo == NULL || iLen == NULL )
		return false;

	//HVAPI_AL_GetPCSHumanTrafficInfo(HVAPI_OPERATE_HANDLE hHandle, DWORD64 dw64StartTime, DWORD64 dw64EndTime, CHAR* szRetInfo, INT* iLen);

	m_mtHandle.Lock();
	if ( m_handle != NULL )
	{
		
		if ( HVAPI_AL_GetPCSHumanTrafficInfo( m_handle , dw64StartTime, dw64EndTime, szRetInfo, iLen) == S_OK )
		{
			bRet = true;
		}
		else
		{
			bRet = false;
		}
		
	}
	m_mtHandle.Unlock();

	return bRet;
}


DWORD CDevice::GetDeviceH264Status()
{
	DWORD dwStatus = AL_LINK_STATE_DISCONN;
	if ( m_handle == NULL )
	{
		return dwStatus;
	}
	m_mtHandle.Lock();


	if ( HVAPI_AL_GetDevLinkStatus(m_handle, AL_LINK_TYPE_H264_VIDEO, &dwStatus) != S_OK )
	{

		dwStatus =  AL_LINK_STATE_DISCONN;
	}
	

	m_mtHandle.Unlock();

	return dwStatus;


}

DWORD CDevice::GetDeviceJpegStatus()
{
	DWORD dwStatus = AL_LINK_STATE_DISCONN;
	if ( m_handle == NULL )
	{
		return dwStatus;
	}
	m_mtHandle.Lock();


	if ( HVAPI_AL_GetDevLinkStatus(m_handle, AL_LINK_TYPE_JPEG_VIDEO, &dwStatus) != S_OK )
	{

		dwStatus =  AL_LINK_STATE_DISCONN;
	}
	

	m_mtHandle.Unlock();

	return dwStatus;
}

bool CDevice::DeviceReset()
{
	bool fReset = false;

	if ( m_handle == NULL )
	{
		return fReset;
	}


	m_mtHandle.Lock();


	char chTemp[1024] = {0};
	sprintf(chTemp, "ResetDevice");

	char szRetBuf[1024] = {0};
	int nRetLen = 0;
	if ( m_handle != NULL )
	{
		if ( HVAPI_AL_ExecCmdOnDev(m_handle, chTemp, szRetBuf, 1024, &nRetLen) == S_OK)
		{
			
			fReset = true;
		}
	}
	

	m_mtHandle.Unlock();

	return fReset;
}


 int  CDevice::GetDiskFree()
{
	return H264CheckDistSpace("D:\\");
}












