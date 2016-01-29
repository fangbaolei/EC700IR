#include "stdafx.h"
#include "HvDeviceOldTestType.h"
#include <shlwapi.h>

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if(p) {delete[] p; p = NULL;}
#endif

CDeviceType::CDeviceType()
{
	m_strMac = "";
	m_strIP = "";
	m_strMask = "";
	m_strGateway = "";
	m_strDevType = "";
	m_strDevName = "";
	m_hHandle = NULL;
	m_fIsConnected = FALSE;
	m_dwReConnectTimes = 0;
	m_pListShowFrame = NULL;
	m_iListShowFrameIndex = -1;
	m_fIsCanShowInfo = FALSE;
	m_strDate = "";
	m_strTime = "";
	m_strDevInfo = "";
	m_strRetSetCount = "";
	m_strVersion = "";
	m_strModeName = "";
	m_strSavePath = "";
	m_fIsCanSave = FALSE;
	m_ListInfo = NULL;
	m_hProceResultDataThread = NULL;
}

CDeviceType::CDeviceType(LPCSTR szMac, LPCSTR szIP, LPCSTR szMask, LPCSTR szGateway)
{
	m_strMac = szMac;
	m_strIP = szIP;
	m_strMask = szMask;
	m_strGateway = szGateway;
	m_strDevType = "";
	m_strDevName = "";
	m_hHandle = NULL;
	m_fIsConnected = FALSE;
	m_dwReConnectTimes = 0;
	m_pListShowFrame = NULL;
	m_iListShowFrameIndex = -1;
	m_fIsCanShowInfo = FALSE;
	m_strDate = "";
	m_strTime = "";
	m_strDevInfo = "";
	m_strRetSetCount = "";
	m_strVersion = "";
	m_strModeName = "";
	m_ListInfo = NULL;
	char szSavePath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szSavePath, MAX_PATH);
	PathRemoveFileSpec(szSavePath);
	m_strSavePath.Format("%s\\%s", szSavePath, m_strIP.GetBuffer());
	m_fIsCanSave = FALSE;
}

CDeviceType::~CDeviceType()
{
	if(m_hHandle != NULL)
	{
		HVAPI_Close(m_hHandle);
		m_hHandle = NULL;
	}

}

void CDeviceType::SetShowInfoItem(CListCtrl* pListShowFrame, INT iItem)
{
	m_pListShowFrame = pListShowFrame;
	m_iListShowFrameIndex = iItem; 
	char szTmp[20] = {0};
	if(m_fIsConnected)
	{
		memcpy(szTmp, "连接", 4);
		m_pListShowFrame->SetItemText(m_iListShowFrameIndex, 1, m_strDevName.GetBuffer());
	}
	else
	{
		memcpy(szTmp, "断开", 4);
	}
	m_pListShowFrame->SetItemText(m_iListShowFrameIndex, 4, szTmp);
	sprintf(szTmp, "%d", m_dwReConnectTimes);
	m_pListShowFrame->SetItemText(m_iListShowFrameIndex, 5, szTmp);
}

HRESULT CDeviceType::ConnectDevice()
{
	if(m_hHandle != NULL)
	{
		return S_OK;
	}

	if(m_strIP == "")
	{
		return E_FAIL;
	}
	m_hHandle = HVAPI_Open(m_strIP.GetBuffer(), NULL);

	if(m_hHandle == NULL)
	{
		return E_FAIL;
	}

	char szInfoTemp[256] = {0};
	HVAPI_GetInfo(m_hHandle, "OptProductName", szInfoTemp, 256, NULL);
	char szTempInfo[128] = {0};
	HVAPIUTILS_GetExecXmlCmdResString(m_hHandle, 
		szInfoTemp, 
		"OptProductName", 
		"ProductName", 
		szTempInfo);
	m_pListShowFrame->SetItemText(m_iListShowFrameIndex, 1, szTempInfo);
	m_fIsConnected = TRUE;
	m_strDevName = szTempInfo;
	if(!PathFileExists(m_strSavePath.GetBuffer()))
	{
		if(CreateDirectory(m_strSavePath.GetBuffer(), NULL) == TRUE)
		{
			m_fIsCanSave = TRUE;
		}
		else
		{
			m_fIsCanSave = FALSE;
		}
	}
	else
	{
		m_fIsCanSave = TRUE;
	}
	return S_OK;
}

HRESULT CDeviceType::DisConnectDevice()
{
	return S_OK;
}

void CDeviceType::EnableShowInfo(BOOL fIsEnable, CListBox* pListInfo)
{
	m_fIsCanShowInfo = fIsEnable;
	m_ListInfo = pListInfo;
}

BOOL CDeviceType::UpdateDeviceInfo()
{
	char szRetInfo[2048] = {0};
	if(HVAPI_GetInfo(m_hHandle, "DateTime,HvName,OptResetCount,GetVersionString, OptWorkMode",
		szRetInfo, 2048, NULL) != S_OK)
	{
		return E_FAIL;
	}
	
	char szInfo[128] = {0};
	if(HVAPIUTILS_GetExecXmlCmdResString(m_hHandle, szRetInfo, "DateTime", "Date", szInfo)
		!= S_OK)
	{
		return E_FAIL;
	}
	m_strDate = szInfo;
	if(HVAPIUTILS_GetExecXmlCmdResString(m_hHandle, szRetInfo, "DateTime", "Time", szInfo)
		!= S_OK)
	{
		return E_FAIL;
	}
	m_strTime = szInfo;

	m_ListInfo->ResetContent();
	if(m_fIsCanShowInfo)
	{
		CString strShowInfo;
		strShowInfo.Format("设备系统时间:%s %s", m_strDate, m_strTime);
		m_ListInfo->AddString(strShowInfo.GetBuffer());
	}

	if(HVAPIUTILS_GetExecXmlCmdResString(m_hHandle, szRetInfo, "HvName", "Name", szInfo)
		!= S_OK)
	{
		return E_FAIL;
	}
	m_strDevInfo = szInfo;

	if(m_fIsCanShowInfo)
	{
		CString strShowInfo;
		strShowInfo.Format("系统名称:%s", m_strDevInfo);
		m_ListInfo->AddString(strShowInfo.GetBuffer());
	}

	if(HVAPIUTILS_GetExecXmlCmdResString(m_hHandle, szRetInfo, "OptResetCount", "ResetCount", szInfo)
		!= S_OK)
	{
		return E_FAIL;
	}
	m_strRetSetCount = szInfo;

	if(m_fIsCanShowInfo)
	{
		CString strShowInfo;
		strShowInfo.Format("复位次数:%s", m_strRetSetCount);
		m_ListInfo->AddString(strShowInfo.GetBuffer());
	}

	if(HVAPIUTILS_GetExecXmlCmdResString(m_hHandle, szRetInfo, "GetVersionString", "SoftVersionString", szInfo)
		!= S_OK)
	{
		return E_FAIL;
	}
	m_strVersion = szInfo;

	if(m_fIsCanShowInfo)
	{
		CString strShowInfo;
		strShowInfo.Format("软件版本号:%s", m_strVersion);
		m_ListInfo->AddString(strShowInfo.GetBuffer());
	}

	if(HVAPIUTILS_GetExecXmlCmdResString(m_hHandle, szRetInfo, "OptWorkMode", "WorkMode", szInfo)
		!= S_OK)
	{
		return E_FAIL;
	}

	char szCmd[128] = {0};
	sprintf(szCmd, "GetWorkModeIndex,WorkModeIndex[%s]", szInfo);
	if(HVAPI_ExecCmd(m_hHandle, szCmd, szRetInfo, 2048, NULL) != S_OK)
	{
		return E_FAIL;
	}

	if(HVAPIUTILS_GetExecXmlCmdResString(m_hHandle, szRetInfo, "GetWorkModeIndex", "WorkModeName", szInfo)
		!= S_OK)
	{
		return E_FAIL;
	}
	m_strModeName = szInfo;
	if(m_fIsCanShowInfo)
	{
		CString strShowInfo;
		strShowInfo.Format("工作模式:%s", m_strModeName);
		m_ListInfo->AddString(strShowInfo.GetBuffer());
	}

	return TRUE;
}

void CDeviceType::SetShowPlateFrame(CWnd* pShowPlateFrame)
{
	m_pShowPlateFrame = pShowPlateFrame;
}

void CDeviceType::SetShowCatherInfoFrame(CWnd* pShowCatherInfoFrame)
{
	m_pShowCatherFrame = pShowCatherInfoFrame;
}

BOOL CDeviceType::SetRecordCallBack()
{
	InitializeCriticalSection(&m_csListRecord);
	m_fIsExitProceRecord = FALSE;
	m_hProceResultDataThread = CreateThread(NULL, 0, ProceResultDataThread, this, 0, NULL);
	HVAPI_SetCallBack(m_hHandle, OnRecord, this, STREAM_TYPE_RECORD, NULL);
	return TRUE;
}

DWORD WINAPI CDeviceType::ProceResultDataThread(LPVOID pParam)
{
	if(pParam == NULL)
	{
		return 10;
	}
	CDeviceType* pDev = (CDeviceType*)pParam;
	while(!pDev->m_fIsExitProceRecord)
	{
		if(pDev->m_RecordList.IsEmpty())
		{
			Sleep(500);
			continue;
		}	

		RECORDE_DATA_TYPE TmpRecordData;
		EnterCriticalSection(&pDev->m_csListRecord);
		TmpRecordData = pDev->m_RecordList.RemoveHead();
		LeaveCriticalSection(&pDev->m_csListRecord);

		if(TmpRecordData.dwRecordType == RECORD_TYPE_NORMAL 
			|| TmpRecordData.dwRecordType == RECORD_TYPE_HISTORY)
		{
			if(TmpRecordData.pszRecordInfo == NULL)
			{
				SAFE_DELETE(TmpRecordData.pRevordPackData);
				SAFE_DELETE(TmpRecordData.pszRecordInfo);
				continue;
			}

			DWORD dwBufLen = 50;
			char* pszPlateBuf = new char[dwBufLen];
			memset(pszPlateBuf, 0, dwBufLen);
			if(HVAPIUTILS_GetRecordInfoFromAppenedString(TmpRecordData.pszRecordInfo, "PlateName", pszPlateBuf, 50) != S_OK)
			{
				SAFE_DELETE(TmpRecordData.pRevordPackData);
				SAFE_DELETE(TmpRecordData.pszRecordInfo);
				SAFE_DELETE(pszPlateBuf);
				continue;
			}
			if(pDev->m_pShowPlateFrame)
			{
				pDev->m_pShowPlateFrame->SetWindowText(pszPlateBuf);
			}

			DWORD dwCarID, dwTimeLow, dwTimeHigh;
			char* pszValue = new char[dwBufLen];
			memset(pszValue, 0, dwBufLen);

			if(HVAPIUTILS_GetRecordInfoFromAppenedString(TmpRecordData.pszRecordInfo, "CarID", pszValue, dwBufLen) != S_OK)
			{
				SAFE_DELETE(pszPlateBuf);
				SAFE_DELETE(pszValue);
				return E_FAIL;
			}
			dwCarID = atoi(pszValue);

			memset(pszValue, 0, dwBufLen);
			if(HVAPIUTILS_GetRecordInfoFromAppenedString(TmpRecordData.pszRecordInfo, "TimeHigh", pszValue, dwBufLen) != S_OK)
			{
				SAFE_DELETE(pszPlateBuf);
				SAFE_DELETE(pszValue);
				return E_FAIL;
			}
			dwTimeHigh = atoi(pszValue);

			memset(pszValue, 0, dwBufLen);
			if(HVAPIUTILS_GetRecordInfoFromAppenedString(TmpRecordData.pszRecordInfo, "TimeLow", pszValue, dwBufLen) != S_OK)
			{
				SAFE_DELETE(pszPlateBuf);
				SAFE_DELETE(pszValue);
				return E_FAIL;
			}
			dwTimeLow = atoi(pszValue);

			SAFE_DELETE(pszValue);
			DWORD64 dw64TimeMS = ((DWORD64)(dwTimeHigh)<<32) | dwTimeLow;
		}
	}
	return 0;
}

INT CDeviceType::OnRecord(PVOID pUserData, PBYTE pbResultPacket, DWORD dwPacketLen, DWORD dwRecordType, LPCSTR szResultInfo)
{
	if(pUserData == NULL || szResultInfo == NULL)
	{
		return -1;
	}
	CDeviceType* pDev = (CDeviceType*)pUserData;
	if(pDev->m_RecordList.IsFull())
	{
		return -1;
	}
	RECORDE_DATA_TYPE NewRecord;
	NewRecord.dwRecordType = dwRecordType;
	NewRecord.dwRecordPacketLen = dwPacketLen;
	if(dwPacketLen > 0)
	{
		NewRecord.pRevordPackData = new BYTE[dwPacketLen];
		if(NewRecord.pRevordPackData == NULL)
		{
			return -1;
		}
		memcpy(NewRecord.pRevordPackData, pbResultPacket, dwPacketLen);
	}

	DWORD dwInfoLen = (DWORD)strlen(szResultInfo);
	NewRecord.pszRecordInfo = new char[dwInfoLen];
	if(NewRecord.pszRecordInfo == NULL)
	{
		SAFE_DELETE(NewRecord.pRevordPackData);
		return -1;
	}
	memcpy(NewRecord.pszRecordInfo, szResultInfo, dwInfoLen);
	EnterCriticalSection(&pDev->m_csListRecord);
	pDev->m_RecordList.AddTail(NewRecord);
	LeaveCriticalSection(&pDev->m_csListRecord);

	return 0;
}

INT CDeviceType::OnGettherInfo(PVOID pUserData, PBYTE pbInfoData, DWORD dwInfoDataLen)
{
	if(pUserData == NULL)
	{
		return 10;
	}
	CDeviceType* pDev = (CDeviceType*)pUserData;
	if(pDev->m_pShowCatherFrame)
	{
		pDev->m_pShowCatherFrame->SetWindowText((char*)pbInfoData);
	}
	return 0;
}

