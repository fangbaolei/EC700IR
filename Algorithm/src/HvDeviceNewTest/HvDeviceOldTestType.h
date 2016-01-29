#ifndef _HVNAVI_DEVICE_TYPE_H__
#define _HVNAVI_DEVICE_TYPE_H__

#include "HvDevice.h"
#include "HvVarType.h"

using namespace HiVideo;

typedef struct _RECORDE_DATA_TYPE
{
	DWORD	dwRecordType;
	DWORD	dwRecordPacketLen;
	PBYTE	pRevordPackData;
	char*	pszRecordInfo;

	_RECORDE_DATA_TYPE()
	{
		pRevordPackData = NULL;
		pszRecordInfo = NULL;
	}
}RECORDE_DATA_TYPE;

class CDeviceType
{
public:
	CDeviceType();
	CDeviceType(LPCSTR szMac, LPCSTR szIP, LPCSTR szMask, LPCSTR szGateway);
	~CDeviceType();

public:
	const char* GetDevMacAddr(void)
	{
		return m_strMac.GetBuffer();
	}
	const char* GetDevIPAddr(void)
	{
		return m_strIP.GetBuffer();
	}
	BOOL GetDevConnectStatus(void)
	{
		return m_fIsConnected;
	}
	void SetShowInfoItem(CListCtrl* pListShowFrame, INT iItem);
	HRESULT ConnectDevice(void);
	HRESULT DisConnectDevice(void);
	void EnableShowInfo(BOOL fIsEnable, CListBox* pListInfo);
	BOOL UpdateDeviceInfo(void);
	void SetShowPlateFrame(CWnd* pShowPlateFrame);
	void SetShowCatherInfoFrame(CWnd* pShowCatherInfoFrame);
	BOOL SetRecordCallBack(void);
	void SetSavePath(void);

private:
	static INT OnRecord(PVOID pUserData, PBYTE pbResultPacket, DWORD dwPacketLen, DWORD dwRecordType, LPCSTR szResultInfo);
	static INT OnGettherInfo(PVOID pUserData, PBYTE pbInfoData, DWORD dwInfoDataLen);
	static DWORD WINAPI ProceResultDataThread(LPVOID pParam);

private:
	HVAPI_HANDLE	m_hHandle;
	CString	m_strMac;
	CString	m_strIP;
	CString	m_strMask;
	CString m_strGateway;
	CString m_strDevType;
	CString m_strDevName;
	BOOL	m_fIsConnected;
	DWORD	m_dwReConnectTimes;
	CListCtrl* m_pListShowFrame;
	INT	m_iListShowFrameIndex;
	BOOL	m_fIsCanShowInfo;
	CListBox* m_ListInfo;
	CString m_strDate;
	CString m_strTime;
	CString m_strDevInfo;
	CString m_strRetSetCount;
	CString m_strVersion;
	CString m_strModeName;
	CString m_strSavePath;
	BOOL	m_fIsCanSave;
	CHvList<RECORDE_DATA_TYPE>	m_RecordList;
	CRITICAL_SECTION m_csListRecord;
	CWnd* m_pShowPlateFrame;
	CWnd* m_pShowCatherFrame;
	HANDLE	m_hProceResultDataThread;
	BOOL	m_fIsExitProceRecord;
};

#endif
