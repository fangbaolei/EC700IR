#pragma once

#include "HvAutoLinkDeviceNew.h"
#include <afxmt.h>

enum DEVSTATUS{
	CONNECTED,
	DISCONNECT,
};

class CDevice
{
public:
	CDevice(void);
	~CDevice(void);

	static INT  H264CallBackProxy(
												PVOID pUserData,  
												DWORD dwVedioFlag,
												DWORD dwVideoType, 
												DWORD dwWidth,
												DWORD dwHeight,
												DWORD64 dw64TimeMS,
												PBYTE pbVideoData, 
												DWORD dwVideoDataLen,
												LPCSTR szVideoExtInfo
												);


	int OnH264(DWORD dwVedioFlag,
												DWORD dwVideoType, 
												DWORD dwWidth,
												DWORD dwHeight,
												DWORD64 dw64TimeMS,
												PBYTE pbVideoData, 
												DWORD dwVideoDataLen,
												LPCSTR szVideoExtInfo);


	static  INT JPEGCallBackProxy(
											PVOID pUserData,  
											DWORD dwImageFlag,
											DWORD dwImageType, 
											DWORD dwWidth,
											DWORD dwHeight,
											DWORD64 dw64TimeMS,
											PBYTE pbImageData, 
											DWORD dwImageDataLen,
											LPCSTR szImageExtInfo
											);

	int OnJpeg(DWORD dwImageFlag,
											DWORD dwImageType, 
											DWORD dwWidth,
											DWORD dwHeight,
											DWORD64 dw64TimeMS,
											PBYTE pbImageData, 
											DWORD dwImageDataLen,
											LPCSTR szImageExtInfo);



	int SetDeviceInfo(char *szAddr, int iPort, char* szDeviceNo, char *szDeviceInfo, HVAPI_OPERATE_HANDLE handle)
	{
		if ( szAddr != NULL )
			strcpy(m_szAddr, szAddr);
		
		m_iPort = iPort;

		if (szDeviceNo != NULL )
			strcpy(m_szDeviceNo, szDeviceNo);

		if ( szDeviceInfo != NULL )
			strcpy( m_szDeviceInfo, szDeviceInfo);

		m_handle = handle;

		return 0;

	}

	bool SetDeviceHandle(HVAPI_OPERATE_HANDLE handle)
	{
		m_mtHandle.Lock();
		m_handle = handle;
		m_mtHandle.Unlock();

		return true;
	}

	bool SetDeviceAddr(char* szAddr, int Port)
	{
		m_iPort = Port;

		if ( szAddr != NULL )
			strcpy(m_szAddr, szAddr);
		
		return true;
	}

	bool SetDeviceInfo(char* szDeviceInfo)
	{
		if ( szDeviceInfo != NULL )
			strcpy( m_szDeviceInfo, szDeviceInfo);

		return true;
	}

	char* GetDeviceNo()
	{
		return m_szDeviceNo;
	}

	char* GetNetAddr()
	{
		return m_szAddr;
	}

	char* GetDeviceInfo()
	{
		return  m_szDeviceInfo;
	}

	int GetPort()
	{
		return m_iPort;
	}

	bool SetDeviceStatus(DEVSTATUS devStatus)
	{
		m_devStatus = devStatus;
		return true;
	}

	
	DEVSTATUS  GetDeviceStatus()
	{
		return m_devStatus;
	}
	


	HVAPI_OPERATE_HANDLE GetDeviceHandle()
	{
		return m_handle;
	}

	bool SetSaveDir(CString strDir)
	{
		m_strSaveDir = strDir;
		return true;
	}

	bool GetDeviceXml();
	bool UpLoadDeviceXml(CString strFileName);
	bool SyncTime();

	CString GetDeviceTime();

	bool GetPCSHumanTrafficInfo(DWORD64 dw64StartTime, DWORD64 dw64EndTime, CHAR* szRetInfo, INT* iLen);
	bool GetCaptureImage(int nTimeM, BYTE *pImageBuffLen, int iBuffLen, int *iImageLen,DWORD64 *dwTime, DWORD *dwWidth, DWORD *dwHeigh);

	bool StartVideo();
	bool StopVideo();

	bool StartJpegVideo();
	bool StopJpegVideo();

	bool GetHumanTrafficInfo(DWORD64 dw64StartTime, DWORD64 dw64EndTime, char *szRetInfo, int *iLen);

	bool DeviceReset();

	DWORD GetDeviceH264Status();
	DWORD GetDeviceJpegStatus();

	static int  GetDiskFree();

	//HV_API_AUTO HRESULT CDECL HVAUTOAPI_GetPCSHumanTrafficInfo(HVAPI_OPERATE_HANDLE hHandle, DWORD64 dw64StartTime, DWORD64 dw64EndTime, CHAR* szRetInfo, INT* iLen);
private:
	HVAPI_OPERATE_HANDLE m_handle;
	int m_iPort;
	char m_szAddr[32];
	char m_szDeviceNo[64];
	char m_szDeviceInfo[1024];

	CMutex m_mtHandle;
	DEVSTATUS m_devStatus;


	//H264 视频保存
	//视频保存
	CTime m_startTime; 
	void *m_pVideoHandle;
	CString m_strSaveDir;

	
};
