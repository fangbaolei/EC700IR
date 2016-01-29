#include "stdafx.h"
#include "OldResultRecvTest.h"
#include ".\oldresultrecvtest.h"
#include "OldTestSetSavePathDlg.h"
#include "OldTestSetEnhanceFlagDlg.h"
#include "OldSetGetHistoryRecordDlg.h"
#include "atlimage.h"

using namespace Gdiplus;

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) if(p) {delete[] p; p = NULL;}
#endif

#define WM_PROC_RECORDDATA	(WM_USER + 1200)

IMPLEMENT_DYNAMIC(COldResultRecvTestDlg, CDialog)

extern CListCtrl* g_ListEx;
extern DWORD g_dwCurrentConnectIndexEx;

HFONT g_hFontEx;

extern HRESULT Yuv2Rgb(
				BYTE *pbDest,
				BYTE *pbSrc,
				int iSrcWidth,
				int iSrcHeight,
				int iBGRStride
				);

COldResultRecvTestDlg::COldResultRecvTestDlg(HVAPI_HANDLE* phHandle, CWnd* pParent /* = NULL */)
: CDialog(COldResultRecvTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDevice = phHandle;
	m_dwLastStatus = CONN_STATUS_UNKNOWN;
	m_dwReconnectTimes = 0;
	m_fIsSetRecordCallback = FALSE;

	InitializeCriticalSection(&m_csList);
	m_fIsQuit = FALSE;
	m_hProceThread = CreateThread(NULL, 0, ProceThread, this, 0, NULL);

	m_fIsSaveRecord = FALSE;
	m_strSavePath = "";
	m_iRedLightEnhanceFlag = 0;
	m_iBrightness = 0;
	m_iHueThreshold = 120;
	m_CompressRate = 80;

	m_fIsSetHistoryFlag = FALSE;
	m_fIsSetRecordFlag = FALSE;
}

COldResultRecvTestDlg::~COldResultRecvTestDlg()
{
	HVAPI_SetCallBack(*m_hDevice, NULL, NULL, STREAM_TYPE_RECORD, NULL);
	if(m_hProceThread)
	{
		m_fIsQuit= TRUE;
		int iWaitTimes = 0;
		int MAX_WAIT_TIME = 8;
		while(WaitForSingleObject(m_hProceThread, 500) == WAIT_TIMEOUT
			&& iWaitTimes < MAX_WAIT_TIME)
		{
			iWaitTimes++;
		}
		if(iWaitTimes >= MAX_WAIT_TIME)
		{
			TerminateThread(m_hProceThread, 0);
		}
		CloseHandle(m_hProceThread);
		m_hProceThread = NULL;
	}
	while (!m_RecordList.IsEmpty())
	{
		RECORD_PACKET_TYPE cTmpInfo = m_RecordList.RemoveHead();
		if(cTmpInfo.pData)
		{
			delete[] cTmpInfo.pData;
			cTmpInfo.pData = NULL;
		}
		if(cTmpInfo.pszPlateInfo)
		{
			delete[] cTmpInfo.pszPlateInfo;
			cTmpInfo.pszPlateInfo = NULL;
		}
	}
}

void COldResultRecvTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COldResultRecvTestDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON7, OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON12, OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON11, OnBnClickedButton11)
	ON_MESSAGE(WM_PROC_RECORDDATA, OnProcRecordData)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON15, OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON17, OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON18, OnBnClickedButton18)
	ON_BN_CLICKED(IDC_BTN_PRTSCR, OnBnClickedBtnPrtscr)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_BN_CLICKED(IDC_START_RECORD, OnBnClickedStartRecord)
	ON_BN_CLICKED(IDC_BUTTON_GETIMAGE, OnBnClickedButtonGetimage)
END_MESSAGE_MAP()

BOOL COldResultRecvTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_dwTotleResult = 0;
	g_hFontEx = CreateFont(50, 26, 0, 0, FW_HEAVY, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	MoveWindow((GetSystemMetrics(SM_CXSCREEN)-1100)>>1,
		(GetSystemMetrics(SM_CYSCREEN)-850)>>1, 1100, 850, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_BEST)->GetSafeHwnd(), 5, 5, 640, 400, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_LAST)->GetSafeHwnd(), 5, 410, 640, 400, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_SMALL)->GetSafeHwnd(), 650, 5, 300, 70, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_BINARY)->GetSafeHwnd(), 650, 80, 300, 70, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_PLATE)->GetSafeHwnd(), 650, 155, 300, 70, TRUE);
	::MoveWindow(GetDlgItem(IDC_EDIT1)->GetSafeHwnd(), 650, 230, 300, 430, TRUE);
	::MoveWindow(GetDlgItem(IDC_EDIT2)->GetSafeHwnd(), 650, 665, 300, 150, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON1)->GetSafeHwnd(), 955, 5, 130, 40, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON5)->GetSafeHwnd(), 955, 50, 130, 40, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON6)->GetSafeHwnd(), 955, 95, 130, 40, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON7)->GetSafeHwnd(), 955, 140, 130, 40, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON11)->GetSafeHwnd(), 955, 185, 130, 40, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON12)->GetSafeHwnd(), 955, 230, 130, 40, TRUE);
	::MoveWindow(GetDlgItem(IDC_BUTTON15)->GetSafeHwnd(), 955, 275, 130, 40, TRUE);
	::MoveWindow(GetDlgItem(IDC_BTN_PRTSCR)->GetSafeHwnd(), 955, 320, 130, 40, TRUE);

	GetDlgItem(IDC_BUTTON17)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON7)->SetWindowText("设置结果回调");

	SetTimer(2002, 1000, NULL);
	return TRUE;
}

void COldResultRecvTestDlg::OnCancel()
{
	HVAPI_SetCallBack(*m_hDevice, NULL, NULL, STREAM_TYPE_RECORD, NULL);
	if(m_hProceThread)
	{
		m_fIsQuit= TRUE;
		int iWaitTimes = 0;
		int MAX_WAIT_TIME = 8;
		while(WaitForSingleObject(m_hProceThread, 500) == WAIT_TIMEOUT
			&& iWaitTimes < MAX_WAIT_TIME)
		{
			iWaitTimes++;
		}
		if(iWaitTimes >= MAX_WAIT_TIME)
		{
			TerminateThread(m_hProceThread, 0);
		}
		CloseHandle(m_hProceThread);
		m_hProceThread = NULL;
	}

	while (!m_RecordList.IsEmpty())
	{
		RECORD_PACKET_TYPE cTmpInfo = m_RecordList.RemoveHead();
		if(cTmpInfo.pData)
		{
			delete[] cTmpInfo.pData;
			cTmpInfo.pData = NULL;
		}
		if(cTmpInfo.pszPlateInfo)
		{
			delete[] cTmpInfo.pszPlateInfo;
			cTmpInfo.pszPlateInfo = NULL;
		}
	}
	DeleteCriticalSection(&m_csList);
	CDialog::OnCancel();
}

DWORD WINAPI COldResultRecvTestDlg::ProceThread(LPVOID pParam)
{
	if(pParam == NULL)
	{
		return 10;
	}
	COldResultRecvTestDlg* pDlg = (COldResultRecvTestDlg*)pParam;
	while(!pDlg->m_fIsQuit)
	{
		if(pDlg->m_RecordList.IsEmpty())
		{
			Sleep(100);
			continue;
		}

		RECORD_PACKET_TYPE TmpRecord;
		EnterCriticalSection(&pDlg->m_csList);
		TmpRecord = pDlg->m_RecordList.RemoveHead();
		LeaveCriticalSection(&pDlg->m_csList);

		pDlg->SendMessage(WM_PROC_RECORDDATA, 0, (LPARAM)&TmpRecord);

		if(TmpRecord.pData)
		{
			delete[] TmpRecord.pData;
			TmpRecord.pData = NULL;
		}
		if(TmpRecord.pszPlateInfo)
		{
			delete[] TmpRecord.pszPlateInfo;
			TmpRecord.pszPlateInfo;
		}
	}
	return 0;
}

void COldResultRecvTestDlg::OnTimer(UINT nIDEvent)
{
	if(m_hDevice && nIDEvent == 2002)
	{
		DWORD dwConnStatus = CONN_STATUS_UNKNOWN;
		if(HVAPI_GetConnStatus(*m_hDevice, STREAM_TYPE_RECORD, &dwConnStatus)
			== S_OK)
		{
			if(m_dwLastStatus == CONN_STATUS_RECONN 
				&& dwConnStatus == CONN_STATUS_NORMAL)
			{
				m_dwReconnectTimes++;
			}
			m_dwLastStatus = dwConnStatus;
		}
		char szTmpInfo[20] = {0};
		sprintf(szTmpInfo, "%d", m_dwReconnectTimes);
		g_ListEx->SetItemText(g_dwCurrentConnectIndexEx, 2, szTmpInfo);
	}
	CDialog::OnTimer(nIDEvent);
}

void COldResultRecvTestDlg::OnBnClickedButton7()
{
	if(m_hDevice == NULL)
	{
		MessageBox("设置结果接收回调失败", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);
	if(m_fIsSetRecordCallback)
	{
		if(HVAPI_SetCallBack(*m_hDevice, NULL, NULL, STREAM_TYPE_RECORD, NULL) != S_OK)
		{
			MessageBox("取消结果接收回调失败", "ERROR", MB_OK|MB_ICONERROR);
			GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
			return;
		}
		m_fIsSetRecordCallback = FALSE;
		SetDlgItemText(IDC_BUTTON7, "设置结果回调");
		if(HVAPI_SetCallBack(*m_hDevice, NULL, NULL, STREAM_TYPE_GATHER_INFO, NULL) != S_OK)
		{
			MessageBox("取消交通信息采集回调失败...");
		}
	}
	else
	{
		if(HVAPI_SetCallBack(*m_hDevice, OnRecord, this, STREAM_TYPE_RECORD, NULL) != S_OK)
		{
			MessageBox("设置结果接收回调失败", "ERROR", MB_OK|MB_ICONERROR);
			GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
			return;
		}
		m_fIsSetRecordCallback = TRUE;
		SetDlgItemText(IDC_BUTTON7, "取消结果回调");
		if(HVAPI_SetCallBack(*m_hDevice, OnGetherInfo, this, STREAM_TYPE_GATHER_INFO, NULL) != S_OK)
		{
			MessageBox("设置交通信息采集回调失败...");
		}
	}
	GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
}

void OldTestWrightLog(LPCSTR lpcInfo)
{
	FILE* fp = fopen("D:\\HvDevice_ReviewOldLog.txt", "a+");
	if(fp)
	{
		fwrite(lpcInfo, strlen(lpcInfo), 1, fp);
		fwrite("\n", 1, 1, fp);
		fclose(fp);
	}
}

INT COldResultRecvTestDlg::OnRecord(PVOID pUserData, PBYTE pbResultPacket, DWORD dwPacketLen, DWORD dwRecordType, LPCSTR szResultInfo)
{
	if(pUserData == NULL)
	{
		return -1;
	}
	
	COldResultRecvTestDlg* pDlg = (COldResultRecvTestDlg*)pUserData;
	if(pDlg->m_RecordList.IsFull())
	{
		return -1;
	}
	if(szResultInfo == NULL)
	{
		return -1;
	}
	RECORD_PACKET_TYPE NewRecordData;
	NewRecordData.dwDataType = dwRecordType;
	NewRecordData.dwDataLen = dwPacketLen;
	INT iInfoLen = (INT)strlen(szResultInfo);
	NewRecordData.pszPlateInfo = new char[iInfoLen];
	if(NewRecordData.pszPlateInfo == NULL)
	{
		return -1;
	}
	NewRecordData.pData = new BYTE[NewRecordData.dwDataLen];
	if(NewRecordData.pData == NULL)
	{
		SAFE_DELETE_ARRAY(NewRecordData.pszPlateInfo);
		return -1;
	}
	memcpy(NewRecordData.pszPlateInfo, szResultInfo, iInfoLen);
	memcpy(NewRecordData.pData, pbResultPacket, dwPacketLen);

	EnterCriticalSection(&pDlg->m_csList);
	pDlg->m_RecordList.AddTail(NewRecordData);
	LeaveCriticalSection(&pDlg->m_csList);

	if(dwRecordType == RECORD_TYPE_HISTORY)
	{
		OldTestWrightLog("**********历史结果");
	}
	else
	{
		OldTestWrightLog("&&&&&&&&&&实时结果");
	}

	return 0;
}

LRESULT COldResultRecvTestDlg::OnProcRecordData(WPARAM wParam, LPARAM lParam)
{
	if(lParam == NULL)
	{
		return E_FAIL;
	}
	RECORD_PACKET_TYPE* pTmpResult = (RECORD_PACKET_TYPE*)lParam;
	if(pTmpResult->dwDataType == RECORD_TYPE_NORMAL
		|| pTmpResult->dwDataType == RECORD_TYPE_HISTORY)
	{
		if(pTmpResult->pszPlateInfo == NULL)
		{
			return E_FAIL;
		}

		DWORD dwBufLen = 50;
		char* pszPlate = new char[dwBufLen];
		memset(pszPlate, 0, dwBufLen);
		if(HVAPIUTILS_GetRecordInfoFromAppenedString(pTmpResult->pszPlateInfo, "PlateName", pszPlate, 50)
			!= S_OK)
		{
			SAFE_DELETE_ARRAY(pszPlate);
			return E_FAIL;
		}

		DWORD dwCarID, dwTimeLow, dwTimeHigh;
		char* pszValue = new char[dwBufLen];
		memset(pszValue, 0, dwBufLen);

		if(HVAPIUTILS_GetRecordInfoFromAppenedString(pTmpResult->pszPlateInfo, "CarID", pszValue, dwBufLen) != S_OK)
		{
			SAFE_DELETE_ARRAY(pszPlate);
			SAFE_DELETE_ARRAY(pszValue);
			return E_FAIL;
		}
		dwCarID = atoi(pszValue);

		memset(pszValue, 0, dwBufLen);
		if(HVAPIUTILS_GetRecordInfoFromAppenedString(pTmpResult->pszPlateInfo, "TimeHigh", pszValue, dwBufLen) != S_OK)
		{
			SAFE_DELETE_ARRAY(pszPlate);
			SAFE_DELETE_ARRAY(pszValue);
			return E_FAIL;
		}
		dwTimeHigh = atoi(pszValue);

		memset(pszValue, 0, dwBufLen);
		if(HVAPIUTILS_GetRecordInfoFromAppenedString(pTmpResult->pszPlateInfo, "TimeLow", pszValue, dwBufLen) != S_OK)
		{
			SAFE_DELETE_ARRAY(pszPlate);
			SAFE_DELETE_ARRAY(pszValue);
			return E_FAIL;
		}
		dwTimeLow = atoi(pszValue);

		SAFE_DELETE_ARRAY(pszValue);
		DWORD64 dw64TimeMS = ((DWORD64)(dwTimeHigh)<<32) | dwTimeLow;
		m_dwTotleResult++;

		DrawPlate(pszPlate);
		
		int iPlateInfoBufLen = 1024;
		iPlateInfoBufLen = (iPlateInfoBufLen << 5);
		char* pszPlateInfo = new char [iPlateInfoBufLen];
		memset(pszPlateInfo, 0, iPlateInfoBufLen);
		HVAPIUTILS_ParsePlateXmlString(pTmpResult->pszPlateInfo, pszPlateInfo, (1024<<5));
		
		CString strAppend;
		int i=0;
		while(pszPlateInfo[i] != '\0')
		{
			if(pszPlateInfo[i] == '\n')
			{
				strAppend += "\r\n";
				i++;
			}
			else
			{
				strAppend += pszPlateInfo[i];
				i++;
			}
		}
		char* pstrTime = strstr(pszPlateInfo, "车辆检测时间:");
		if(pstrTime)
		{
			DWORD64 dw64TimeMS = 0;
			sscanf(pstrTime, "车辆检测时间:%I64u", &dw64TimeMS);
			CTime cTime(dw64TimeMS / 1000);
			CString strTime;
			strTime.Format("\r\n\r\n\r\n车辆检测时间转换后：%s", cTime.Format("%Y-%m-%d %H:%M:%S"));
			strAppend += strTime;
		}
		if(pTmpResult->dwDataType == RECORD_TYPE_HISTORY)
		{
			strAppend += "\r\n结果类型:历史";
		}
		else
		{
			strAppend += "\r\n结果类型:实时";
		}
		SetDlgItemText(IDC_EDIT1, strAppend.GetBuffer());

		if(m_fIsSaveRecord && m_strSavePath != "")
		{
			CString strSaveFileName;
			CTime cTime(dw64TimeMS/1000);
			strSaveFileName.Format("%s%s_%d.xml", m_strSavePath.GetBuffer(),
				cTime.Format("%Y%m%d%H%M%S"), dwCarID);
			FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
			if(fp)
			{
				fwrite(pTmpResult->pszPlateInfo, strlen(pTmpResult->pszPlateInfo), 1, fp);
				fclose(fp);
			}

			strSaveFileName.Format("%sPlateInfo.txt", m_strSavePath.GetBuffer());
			fp = fopen(strSaveFileName.GetBuffer(), "a+");
			if(fp)
			{
				fwrite(pszPlate, strlen(pszPlate), 1, fp);
				fwrite("\n", 1, 1, fp);
				fclose(fp);
			}
		}
		
		BOOL fIsPeccancy = FALSE;
		if(strstr(pszPlateInfo, "违章:是"))
		{
			fIsPeccancy = TRUE;
		}
		SAFE_DELETE_ARRAY(pszPlate);
		SAFE_DELETE_ARRAY(pszPlateInfo);
		RECORD_IMAGE_GROUP cRecordImageGroup;
		if(pTmpResult->pData == NULL)
		{
			return S_OK;
		}
		if(HVAPIUTILS_GetRecordImageAll(pTmpResult->pData, pTmpResult->dwDataLen, &cRecordImageGroup) != S_OK)
		{
			return E_FAIL;
		}
        DrawRecordImage(&cRecordImageGroup, fIsPeccancy, dwCarID, dw64TimeMS);

	}
	return S_OK;
}

void COldResultRecvTestDlg::DrawPlate(char* pszPlate)
{
	if(pszPlate == NULL)
	{
		return;
	}
	COLORREF bgColor, wordColor;
	if(strstr(pszPlate, "无车牌"))
	{
		bgColor = RGB(255, 0, 0);
		wordColor = RGB(255, 255, 255);
	}
	else if(strstr(pszPlate, "蓝"))
	{
		bgColor = RGB(0, 0, 255);
		wordColor = RGB(255, 255, 255);
	}
	else if(strstr(pszPlate, "黄"))
	{
		bgColor = RGB(255, 255, 0);
		wordColor = RGB(0, 0, 0);
	}
	else if(strstr(pszPlate, "黑"))
	{
		bgColor = RGB(0, 0, 0);
		wordColor = RGB(255, 255, 255);
	}
	else if(strstr(pszPlate, "白"))
	{
		bgColor = RGB(255, 255, 255);
		wordColor = RGB(0, 0, 0);
	}
	else
	{
		bgColor = RGB(0, 0, 255);
		wordColor = RGB(255, 255, 255);
	}

	HWND hTmpHwnd = GetDlgItem(IDC_STATIC_BINARY)->GetSafeHwnd();
	HDC hDC = ::GetDC(hTmpHwnd);
	HPEN hPen;
	HBRUSH hBrush;
	hPen = CreatePen(PS_SOLID, 1, bgColor);
	hBrush = CreateSolidBrush(bgColor);
	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);
	SelectObject(hDC, g_hFontEx);
	Rectangle(hDC, 0, 0, 300, 70);
	DeleteObject(hPen);
	DeleteObject(hBrush);
	SetBkMode(hDC, 3);
	SetTextColor(hDC, wordColor);
	TextOut(hDC, 5, 10, pszPlate, (int)strlen(pszPlate));
	::ReleaseDC(hTmpHwnd, hDC);

	hTmpHwnd = GetDlgItem(IDC_STATIC_PLATE)->GetSafeHwnd();
	hDC = ::GetDC(hTmpHwnd);
	hPen = CreatePen(PS_SOLID, 1, RGB(5, 5, 5));
	hBrush = CreateSolidBrush(RGB(5, 5, 5));
	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);
	SelectObject(hDC, g_hFontEx);
	Rectangle(hDC, 0, 0, 300, 70);
	DeleteObject(hPen);
	DeleteObject(hBrush);
	SetBkMode(hDC, 3);
	SetTextColor(hDC, RGB(0, 192, 0));
	char szTmpeInfo[20] = {0};
	sprintf(szTmpeInfo, "%d", m_dwTotleResult);
	TextOut(hDC, 5, 10, szTmpeInfo, (int)strlen(szTmpeInfo));
	::ReleaseDC(hTmpHwnd, hDC);
}

void COldResultRecvTestDlg::DrawRecordImage(RECORD_IMAGE_GROUP* pcRecordImage, BOOL fIsPeccancy, DWORD dwCarID, DWORD64 dw64TimeMS)
{
	if(pcRecordImage == NULL)
	{
		return;
	}

	if(pcRecordImage->pbImgDataBestSnapShot)
	{
		PBYTE pEnhanncePicBuf = NULL;
		int iEnhanncePicBufLen = 0;
		if(m_iRedLightEnhanceFlag == 1 && fIsPeccancy
			|| m_iRedLightEnhanceFlag == 2)
		{
			int iRedLightCount = 0;
			int iRedLightPosBufLen = 20*sizeof(RECT);
			PBYTE pRedLightPos = new BYTE[iRedLightPosBufLen];
			memset(pRedLightPos, 0, iRedLightPosBufLen);
			RECORD_IMAGE cTmpImage;
			cTmpImage.cImgInfo = pcRecordImage->cImgInfoBestSnapshot;
			cTmpImage.dwImgInfoLen = pcRecordImage->dwImgInfoBestSnapShotLen;
			cTmpImage.dwImgDataLen = pcRecordImage->dwImgDataBestSnapShotLen;
			cTmpImage.pbImgData = pcRecordImage->pbImgDataBestSnapShot;
			cTmpImage.pbImgInfoEx = pcRecordImage->pbImgInfoBestSnapShot;
			if(HVAPIUTILS_GetRedLightPosFromeRecordImage(&cTmpImage, iRedLightPosBufLen, pRedLightPos, iRedLightCount) == S_OK)
			{
				if(iRedLightCount > 0 && iRedLightCount <=20)
				{
					iEnhanncePicBufLen = (1024 << 10);
					pEnhanncePicBuf = new BYTE[iEnhanncePicBufLen];
					if(pEnhanncePicBuf)
					{
						if(HVAPIUTILS_TrafficLightEnhance(pcRecordImage->pbImgDataBestSnapShot,
							pcRecordImage->dwImgDataBestSnapShotLen, iRedLightCount, pRedLightPos,
							pEnhanncePicBuf, iEnhanncePicBufLen, m_iBrightness,
							m_iHueThreshold, m_CompressRate) != S_OK)
						{
							delete pEnhanncePicBuf;
							pEnhanncePicBuf = NULL;
						}
					}
				}
			}
			SAFE_DELETE_ARRAY(pRedLightPos);
		}
		IStream* pStm = NULL;
		CreateStreamOnHGlobal(NULL, TRUE, &pStm);
		IPicture* picholder;
		LARGE_INTEGER liTempStar = {0};
		pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
		ULONG iWritten = NULL;
		if(pEnhanncePicBuf)
		pStm->Write(pEnhanncePicBuf, iEnhanncePicBufLen, &iWritten);
		else
		pStm->Write(pcRecordImage->pbImgDataBestSnapShot, pcRecordImage->dwImgDataBestSnapShotLen, &iWritten);
		pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
		if(pEnhanncePicBuf)
		{
			if(FAILED(OleLoadPicture(pStm, iEnhanncePicBufLen, TRUE, IID_IPicture, (void**)&picholder)))
			{
				pStm->Release();
				return;
			}
		}
		else
		{
			if(FAILED(OleLoadPicture(pStm, pcRecordImage->dwImgDataBestSnapShotLen, TRUE, IID_IPicture, (void**)&picholder)))
			{
				pStm->Release();
				return;
			}
		}

		HDC TempDC;
		TempDC = ::GetDC(GetDlgItem(IDC_STATIC_BEST)->GetSafeHwnd());
		OLE_XSIZE_HIMETRIC hmWidth;
		OLE_YSIZE_HIMETRIC hmHeight;
		picholder->get_Width(&hmWidth);
		picholder->get_Height(&hmHeight);
		int nWidth =MulDiv(hmWidth, GetDeviceCaps(TempDC, LOGPIXELSX), 2540);
		int nHeight = MulDiv(hmHeight, GetDeviceCaps(TempDC, LOGPIXELSY), 2540);
		picholder->Render(TempDC, 0, 0, 640, 400, 0, hmHeight, hmWidth, -hmHeight, NULL); 
		::ReleaseDC(GetDlgItem(IDC_STATIC_BEST)->GetSafeHwnd(), TempDC);
		picholder->Release();
		pStm->Release();

		if(m_fIsSaveRecord && m_strSavePath != "")
		{
			CString strSaveFileName;
			CTime cTime(dw64TimeMS/1000);
			strSaveFileName.Format("%s%s_%d_01.jpg", m_strSavePath.GetBuffer(),
				cTime.Format("%Y%m%d%H%M%S"), dwCarID);
			FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
			if(fp)
			{
				if(pEnhanncePicBuf)
				fwrite(pEnhanncePicBuf, iEnhanncePicBufLen, 1, fp);
				else
				fwrite(pcRecordImage->pbImgDataBestSnapShot, pcRecordImage->dwImgDataBestSnapShotLen, 1, fp);
				fclose(fp);
			}
		}
		SAFE_DELETE_ARRAY(pEnhanncePicBuf);
	}

	if(pcRecordImage->pbImgDataLastSnapShot)
	{
		PBYTE pEnhanncePicBuf = NULL;
		int iEnhanncePicBufLen = 0;
		if(m_iRedLightEnhanceFlag == 1 && fIsPeccancy
			|| m_iRedLightEnhanceFlag == 2)
		{
			int iRedLightCount = 0;
			int iRedLightPosBufLen = 20*sizeof(RECT);
			PBYTE pRedLightPos = new BYTE[iRedLightPosBufLen];
			memset(pRedLightPos, 0, iRedLightPosBufLen);
			RECORD_IMAGE cTmpImage;
			cTmpImage.cImgInfo = pcRecordImage->cImgInfoLastSnapshot;
			cTmpImage.dwImgInfoLen = pcRecordImage->dwImgInfoLastSnapShotLen;
			cTmpImage.dwImgDataLen = pcRecordImage->dwImgDataLastSnapShotLen;
			cTmpImage.pbImgData = pcRecordImage->pbImgDataLastSnapShot;
			cTmpImage.pbImgInfoEx = pcRecordImage->pbImgInfoLastSnapShot;
			if(HVAPIUTILS_GetRedLightPosFromeRecordImage(&cTmpImage, iRedLightPosBufLen, pRedLightPos, iRedLightCount) == S_OK)
			{
				if(iRedLightCount > 0 && iRedLightCount <=20)
				{
					iEnhanncePicBufLen = (1024 << 10);
					pEnhanncePicBuf = new BYTE[iEnhanncePicBufLen];
					if(pEnhanncePicBuf)
					{
						if(HVAPIUTILS_TrafficLightEnhance(pcRecordImage->pbImgDataLastSnapShot,
							pcRecordImage->dwImgDataLastSnapShotLen, iRedLightCount, pRedLightPos,
							pEnhanncePicBuf, iEnhanncePicBufLen, m_iBrightness,
							m_iHueThreshold, m_CompressRate) != S_OK)
						{
							delete pEnhanncePicBuf;
							pEnhanncePicBuf = NULL;
						}
					}
				}
			}
			SAFE_DELETE_ARRAY(pRedLightPos);
		}
		IStream* pStm = NULL;
		CreateStreamOnHGlobal(NULL, TRUE, &pStm);
		IPicture* picholder;
		LARGE_INTEGER liTempStar = {0};
		pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
		ULONG iWritten = NULL;
		if(pEnhanncePicBuf)
		pStm->Write(pEnhanncePicBuf, iEnhanncePicBufLen, &iWritten);
		else
		pStm->Write(pcRecordImage->pbImgDataLastSnapShot, pcRecordImage->dwImgDataLastSnapShotLen, &iWritten);
		pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
		if(pEnhanncePicBuf)
		{
			if(FAILED(OleLoadPicture(pStm, iEnhanncePicBufLen, TRUE, IID_IPicture, (void**)&picholder)))
			{
				pStm->Release();
				return;
			}
		}
		else
		{
			if(FAILED(OleLoadPicture(pStm, pcRecordImage->dwImgDataLastSnapShotLen, TRUE, IID_IPicture, (void**)&picholder)))
			{
				pStm->Release();
				return;
			}
		}

		HDC TempDC;
		TempDC = ::GetDC(GetDlgItem(IDC_STATIC_LAST)->GetSafeHwnd());
		OLE_XSIZE_HIMETRIC hmWidth;
		OLE_YSIZE_HIMETRIC hmHeight;
		picholder->get_Width(&hmWidth);
		picholder->get_Height(&hmHeight);
		int nWidth =MulDiv(hmWidth, GetDeviceCaps(TempDC, LOGPIXELSX), 2540);
		int nHeight = MulDiv(hmHeight, GetDeviceCaps(TempDC, LOGPIXELSY), 2540);
		picholder->Render(TempDC, 0, 0, 640, 400, 0, hmHeight, hmWidth, -hmHeight, NULL); 
		::ReleaseDC(GetDlgItem(IDC_STATIC_LAST)->GetSafeHwnd(), TempDC);
		picholder->Release();
		pStm->Release();

		if(m_fIsSaveRecord && m_strSavePath != "")
		{
			CString strSaveFileName;
			CTime cTime(dw64TimeMS/1000);
			strSaveFileName.Format("%s%s_%d_02.jpg", m_strSavePath.GetBuffer(),
				cTime.Format("%Y%m%d%H%M%S"), dwCarID);
			FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
			if(fp)
			{
				if(pEnhanncePicBuf)
					fwrite(pEnhanncePicBuf, iEnhanncePicBufLen, 1, fp);
				else
					fwrite(pcRecordImage->pbImgDataBestSnapShot, pcRecordImage->dwImgDataBestSnapShotLen, 1, fp);
				fclose(fp);
			}
		}
		SAFE_DELETE_ARRAY(pEnhanncePicBuf);
	}



	if(m_fIsSaveRecord && m_strSavePath != "")
	{
		if(pcRecordImage->pbImgDataBeginCapture)
		{
			PBYTE pEnhanncePicBuf = NULL;
			int iEnhanncePicBufLen = 0;
			if(m_iRedLightEnhanceFlag == 1 && fIsPeccancy
				|| m_iRedLightEnhanceFlag == 2 || m_iRedLightEnhanceFlag == 3)
			{
				int iRedLightCount = 0;
				int iRedLightPosBufLen = 20*sizeof(RECT);
				PBYTE pRedLightPos = new BYTE[iRedLightPosBufLen];
				memset(pRedLightPos, 0, iRedLightPosBufLen);
				RECORD_IMAGE cTmpImage;
				cTmpImage.cImgInfo = pcRecordImage->cImgInfoBeginCapture;
				cTmpImage.dwImgInfoLen = pcRecordImage->dwImgInfoBeginCaptureLen;
				cTmpImage.dwImgDataLen = pcRecordImage->dwImgDataBeginCaptureLen;
				cTmpImage.pbImgData = pcRecordImage->pbImgDataBeginCapture;
				cTmpImage.pbImgInfoEx = pcRecordImage->pbImgInfoBeginCapture;
				if(HVAPIUTILS_GetRedLightPosFromeRecordImage(&cTmpImage, iRedLightPosBufLen, pRedLightPos, iRedLightCount) == S_OK)
				{
					if(iRedLightCount > 0 && iRedLightCount <=20)
					{
						iEnhanncePicBufLen = (1024 << 10);
						pEnhanncePicBuf = new BYTE[iEnhanncePicBufLen];
						if(pEnhanncePicBuf)
						{
							if(HVAPIUTILS_TrafficLightEnhance(pcRecordImage->pbImgDataBeginCapture,
								pcRecordImage->dwImgDataBeginCaptureLen, iRedLightCount, pRedLightPos,
								pEnhanncePicBuf, iEnhanncePicBufLen, m_iBrightness,
								m_iHueThreshold, m_CompressRate) != S_OK)
							{
								delete pEnhanncePicBuf;
								pEnhanncePicBuf = NULL;
							}
						}
					}
				}
				SAFE_DELETE_ARRAY(pRedLightPos);
			}
			CString strSaveFileName;
			CTime cTime(dw64TimeMS/1000);
			strSaveFileName.Format("%s%s_%d_03.jpg", m_strSavePath.GetBuffer(),
				cTime.Format("%Y%m%d%H%M%S"), dwCarID);
			FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
			if(fp)
			{
				if(pEnhanncePicBuf)
					fwrite(pEnhanncePicBuf, iEnhanncePicBufLen, 1, fp);
				else
					fwrite(pcRecordImage->pbImgDataBeginCapture, pcRecordImage->dwImgDataBeginCaptureLen, 1, fp);
				fclose(fp);
			}
			SAFE_DELETE_ARRAY(pEnhanncePicBuf);
		}

		if(pcRecordImage->pbImgDataBestCapture)
		{
			PBYTE pEnhanncePicBuf = NULL;
			int iEnhanncePicBufLen = 0;
			if(m_iRedLightEnhanceFlag == 1 && fIsPeccancy
				|| m_iRedLightEnhanceFlag == 2 || m_iRedLightEnhanceFlag == 3)
			{
				int iRedLightCount = 0;
				int iRedLightPosBufLen = 20*sizeof(RECT);
				PBYTE pRedLightPos = new BYTE[iRedLightPosBufLen];
				memset(pRedLightPos, 0, iRedLightPosBufLen);
				RECORD_IMAGE cTmpImage;
				cTmpImage.cImgInfo = pcRecordImage->cImgInfoBestCapture;
				cTmpImage.dwImgInfoLen = pcRecordImage->dwImgInfoBestCaptureLen;
				cTmpImage.dwImgDataLen = pcRecordImage->dwImgDataBestCaptureLen;
				cTmpImage.pbImgData = pcRecordImage->pbImgDataBestCapture;
				cTmpImage.pbImgInfoEx = pcRecordImage->pbImgInfoBestCapture;
				if(HVAPIUTILS_GetRedLightPosFromeRecordImage(&cTmpImage, iRedLightPosBufLen, pRedLightPos, iRedLightCount) == S_OK)
				{
					if(iRedLightCount > 0 && iRedLightCount <=20)
					{
						iEnhanncePicBufLen = (1024 << 10);
						pEnhanncePicBuf = new BYTE[iEnhanncePicBufLen];
						if(pEnhanncePicBuf)
						{
							if(HVAPIUTILS_TrafficLightEnhance(pcRecordImage->pbImgDataBestCapture,
								pcRecordImage->dwImgDataBestCaptureLen, iRedLightCount, pRedLightPos,
								pEnhanncePicBuf, iEnhanncePicBufLen, m_iBrightness,
								m_iHueThreshold, m_CompressRate) != S_OK)
							{
								delete pEnhanncePicBuf;
								pEnhanncePicBuf = NULL;
							}
						}
					}
				}
				SAFE_DELETE_ARRAY(pRedLightPos);
			}
			CString strSaveFileName;
			CTime cTime(dw64TimeMS/1000);
			strSaveFileName.Format("%s%s_%d_04.jpg", m_strSavePath.GetBuffer(),
				cTime.Format("%Y%m%d%H%M%S"), dwCarID);
			FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
			if(fp)
			{
				if(pEnhanncePicBuf)
					fwrite(pEnhanncePicBuf, iEnhanncePicBufLen, 1, fp);
				else
					fwrite(pcRecordImage->pbImgDataBestCapture, pcRecordImage->dwImgDataBestCaptureLen, 1, fp);
				fclose(fp);
			}
			SAFE_DELETE_ARRAY(pEnhanncePicBuf);
		}
		if(pcRecordImage->pbImgDataLastCapture)
		{
			PBYTE pEnhanncePicBuf = NULL;
			int iEnhanncePicBufLen = 0;
			if(m_iRedLightEnhanceFlag == 1 && fIsPeccancy
				|| m_iRedLightEnhanceFlag == 2 || m_iRedLightEnhanceFlag == 3)
			{
				int iRedLightCount = 0;
				int iRedLightPosBufLen = 20*sizeof(RECT);
				PBYTE pRedLightPos = new BYTE[iRedLightPosBufLen];
				memset(pRedLightPos, 0, iRedLightPosBufLen);
				RECORD_IMAGE cTmpImage;
				cTmpImage.cImgInfo = pcRecordImage->cImgInfoLastCapture;
				cTmpImage.dwImgInfoLen = pcRecordImage->dwImgInfoLastCaptureLen;
				cTmpImage.dwImgDataLen = pcRecordImage->dwImgDataLastCaptureLen;
				cTmpImage.pbImgData = pcRecordImage->pbImgDataLastCapture;
				cTmpImage.pbImgInfoEx = pcRecordImage->pbImgInfoLastCapture;
				if(HVAPIUTILS_GetRedLightPosFromeRecordImage(&cTmpImage, iRedLightPosBufLen, pRedLightPos, iRedLightCount) == S_OK)
				{
					if(iRedLightCount > 0 && iRedLightCount <=20)
					{
						iEnhanncePicBufLen = (1024 << 10);
						pEnhanncePicBuf = new BYTE[iEnhanncePicBufLen];
						if(pEnhanncePicBuf)
						{
							if(HVAPIUTILS_TrafficLightEnhance(pcRecordImage->pbImgDataLastCapture,
								pcRecordImage->dwImgDataLastCaptureLen, iRedLightCount, pRedLightPos,
								pEnhanncePicBuf, iEnhanncePicBufLen, m_iBrightness,
								m_iHueThreshold, m_CompressRate) != S_OK)
							{
								delete pEnhanncePicBuf;
								pEnhanncePicBuf = NULL;
							}
						}
					}
				}
				SAFE_DELETE_ARRAY(pRedLightPos);
			}
			CString strSaveFileName;
			CTime cTime(dw64TimeMS/1000);
			strSaveFileName.Format("%s%s_%d_05.jpg", m_strSavePath.GetBuffer(),
				cTime.Format("%Y%m%d%H%M%S"), dwCarID);
			FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
			if(fp)
			{
				if(pEnhanncePicBuf)
					fwrite(pEnhanncePicBuf, iEnhanncePicBufLen, 1, fp);
				else
					fwrite(pcRecordImage->pbImgDataLastCapture, pcRecordImage->dwImgDataLastCaptureLen, 1, fp);
				fclose(fp);
			}
			SAFE_DELETE_ARRAY(pEnhanncePicBuf);
		}
	}
	
	if(pcRecordImage->pbImgDataSmaller)
	{
		CImage image;
		if(image.Create(pcRecordImage->cImgInfoSmaller.dwWidth, pcRecordImage->cImgInfoSmaller.dwHeight, 24))
		{
			Yuv2Rgb((BYTE*)image.GetBits(), pcRecordImage->pbImgDataSmaller, (int)pcRecordImage->cImgInfoSmaller.dwWidth,
				-(int)pcRecordImage->cImgInfoSmaller.dwHeight, image.GetPitch());
			IStream* pStream = NULL;
			IPicture* picholder;
			CreateStreamOnHGlobal(NULL, TRUE, &pStream);
			image.Save(pStream, ImageFormatJPEG);
			LARGE_INTEGER liTemp = {0};
			pStream->Seek(liTemp, STREAM_SEEK_SET, NULL);
			if(FAILED(OleLoadPicture(pStream, pcRecordImage->dwImgDataSmallerLen, TRUE, IID_IPicture, (void**)&picholder)))
			{
				pStream->Release();
				return;
			}
			HDC TempDC;
			TempDC = ::GetDC(GetDlgItem(IDC_STATIC_SMALL)->GetSafeHwnd());
			OLE_XSIZE_HIMETRIC hmWidth;
			OLE_YSIZE_HIMETRIC hmHeight;
			picholder->get_Width(&hmWidth);
			picholder->get_Height(&hmHeight);
			int nWidth =MulDiv(hmWidth, GetDeviceCaps(TempDC, LOGPIXELSX), 2540);
			int nHeight = MulDiv(hmHeight, GetDeviceCaps(TempDC, LOGPIXELSY), 2540);
			picholder->Render(TempDC, 0, 0, 300, 70, 0, hmHeight, hmWidth, -hmHeight, NULL); 
			::ReleaseDC(GetDlgItem(IDC_STATIC_SMALL)->GetSafeHwnd(), TempDC);
			picholder->Release();
			pStream->Release();

			if(m_fIsSaveRecord && m_strSavePath != "")
			{
				int iBitmapLen = (1024<<10);
				PBYTE pBitmapBuf = new BYTE[iBitmapLen];
				if(pBitmapBuf)
				{
					if(HVAPIUTILS_SmallImageToBitmap(pcRecordImage->pbImgDataSmaller, pcRecordImage->cImgInfoSmaller.dwWidth,
						pcRecordImage->cImgInfoSmaller.dwHeight, pBitmapBuf, &iBitmapLen) == S_OK)
					{
						CString strSaveFileName;
						CTime cTime(dw64TimeMS/1000);
						strSaveFileName.Format("%s%s_%d.bmp", m_strSavePath.GetBuffer(),
							cTime.Format("%Y%m%d%H%M%S"), dwCarID);
						FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
						if(fp)
						{
							fwrite(pBitmapBuf, iBitmapLen, 1, fp);
							fclose(fp);
						}
					}
				}
				SAFE_DELETE_ARRAY(pBitmapBuf);
			}
		}
	}

	if(pcRecordImage->pbImgDataBinary)
	{
		if(m_fIsSaveRecord && m_strSavePath != "")
		{
			CString strSaveFileName;
			CTime cTime(dw64TimeMS/1000);
			strSaveFileName.Format("%s%s_%d.bin", m_strSavePath.GetBuffer(),
				cTime.Format("%Y%m%d%H%M%S"), dwCarID);
			FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
			if(fp)
			{
				fwrite(pcRecordImage->pbImgDataBinary, pcRecordImage->dwImgDataBinaryLen, 1, fp);
				fclose(fp);
			}
		}
	}
}

void COldResultRecvTestDlg::OnBnClickedButton12()
{
	COldTestSetSavePathDlg Dlg(m_hDevice, this);
	Dlg.DoModal();
}

void COldResultRecvTestDlg::OnBnClickedButton11()
{
	COldTestSetEnahnceFlagDlg Dlg(m_hDevice, this);
	Dlg.DoModal();
}

int COldResultRecvTestDlg::OnGetherInfo(PVOID pUserData, LPCSTR pString, DWORD dwStrLen)
{
	if(pUserData == NULL || pString == NULL || dwStrLen <= 0)
	{
		return -1;
	}
	COldResultRecvTestDlg* pDlg = (COldResultRecvTestDlg*)pUserData;
	if(strstr(pString, "统计日期:"))
	{
		CString strTmp;
		DWORD dwIndex = 0;
		while(dwIndex < dwStrLen)
		{
			if(pString[dwIndex] == '\n')
			{
				strTmp += "\r\n";
				dwIndex++;
			}
			else
			{
				strTmp += pString[dwIndex];
				dwIndex++;
			}
		}
		pDlg->GetDlgItem(IDC_EDIT2)->SetWindowText(strTmp);
	}
	return 0;
}

void COldResultRecvTestDlg::OnBnClickedButton15()
{
	if(m_fIsSetHistoryFlag)
	{
		GetDlgItem(IDC_BUTTON15)->EnableWindow(FALSE);
		HVAPI_SetCallBack(*m_hDevice, NULL, NULL, STREAM_TYPE_RECORD, NULL);

		if(m_fIsSetRecordCallback)
		{
			//DWORD dwRetTryTimes = 0;
			//while(dwRetTryTimes < 3)
			//{
				if(HVAPI_SetCallBack(*m_hDevice, OnRecord, this, STREAM_TYPE_RECORD, NULL)
					 != S_OK)
				//{
				//	m_dwTotleResult = 0;
				//	break;
				//}
				//dwRetTryTimes++;
				//Sleep(100);
			//}
			//if(dwRetTryTimes >= 3)
			{
				m_fIsSetRecordCallback = FALSE;
				SetDlgItemText(IDC_BUTTON7, "设置车牌回调");
				HVAPI_SetCallBack(*m_hDevice, NULL, NULL, STREAM_TYPE_GATHER_INFO, NULL);
			}
			else
			{
				HVAPI_SetCallBack(*m_hDevice, OnGetherInfo, this, STREAM_TYPE_GATHER_INFO, NULL);
			}
		}
		GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON15)->EnableWindow(TRUE);
		SetDlgItemText(IDC_BUTTON15, "接收历史结果");
		m_fIsSetHistoryFlag = FALSE;
	}
	else
	{
		dw64StartTime = 0;
		dw64EndTime = 0;
		dwIndex = 0;
		m_fIsUsedEndTime = FALSE;
		m_fIsGetHistoryRecord = FALSE;
		COldSetGetHistoryDlg Dlg(this);
		Dlg.DoModal();
		if(m_fIsGetHistoryRecord)
		{
			GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON15)->EnableWindow(FALSE);
			if(m_fIsSetRecordCallback)
			{
				HVAPI_SetCallBack(*m_hDevice, NULL, NULL, STREAM_TYPE_RECORD, NULL);
				HVAPI_SetCallBack(*m_hDevice, NULL, NULL, STREAM_TYPE_GATHER_INFO, NULL);
			}
			CString strCmd;
			if(m_fIsUsedEndTime)
			{
				if(dw64EndTime <= dw64StartTime)
				{
					MessageBox("设置错误的时间片，下载失败", "ERROR", MB_OK|MB_ICONERROR);
					return;
				}
				CTime cTimeStart(dw64StartTime);
				CTime cTimeEnd(dw64EndTime);
				strCmd.Format("DownloadRecord,BeginTime[%s],EndTime[%s],Index[%d],Enable[1]",
					cTimeStart.Format("%Y.%m.%d_%H"), cTimeEnd.Format("%Y.%m.%d_%H"), dwIndex);
			}
			else
			{
				CTime cTimeStart(dw64StartTime);
				strCmd.Format("DownloadRecord,BeginTime[%s],EndTime[0],Index[%d],Enable[1]",
					cTimeStart.Format("%Y.%m.%d_%H"), dwIndex);
			}
			//DWORD dwRetTryTimes = 0;
			//while(dwRetTryTimes < 3)
			//{
				if(HVAPI_SetCallBack(*m_hDevice, OnRecord, this, STREAM_TYPE_RECORD, strCmd.GetBuffer())
					!= S_OK)
				//{
				//	m_dwTotleResult = 0;
				//	break;
				//}
				//dwRetTryTimes++;
				//Sleep(100);
			//}
			//if(dwRetTryTimes >= 3)
			{
				MessageBox("设置获取历史结果失败", "ERROR", MB_OK|MB_ICONERROR);
				GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
				GetDlgItem(IDC_BUTTON15)->EnableWindow(TRUE);
				return;
			}
			else
			{
				GetDlgItem(IDC_BUTTON15)->EnableWindow(TRUE);
				SetDlgItemText(IDC_BUTTON15, "停止接收历史结果");
				m_fIsSetHistoryFlag = TRUE;
			}
		}
	}
}



void COldResultRecvTestDlg::OnBnClickedButton17()
{
	// TODO: Add your control notification handler code here
}

void COldResultRecvTestDlg::OnBnClickedButton18()
{
	// TODO: Add your control notification handler code here
}




void COldResultRecvTestDlg::OnBnClickedBtnPrtscr()
{
	MessageBox("str","",MB_OK);
}




void COldResultRecvTestDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
}

void COldResultRecvTestDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
}


void COldResultRecvTestDlg::OnBnClickedStartRecord()
{
	// TODO: Add your control notification handler code here
}

void COldResultRecvTestDlg::OnBnClickedButtonGetimage()
{
	// TODO: Add your control notification handler code here
	static int s_index = 0;
	/*
	if ( *m_hDevice != NULL )
	{
		char szFileName[1024] = { 0 };
		CTime tm =CTime::GetCurrentTime();

		sprintf(szFileName, "D:\\result\\%s_%d.jpg", tm.Format("%Y%m%d%H%M%S"), s_index++);

		if ( HVAPI_GetCaptureImage(*m_hDevice, 0, szFileName) == S_OK )
		{
			//MessageBox("str","",MB_OK);
			CString str;
			str.Format("保存图片成功:%s", szFileName);
			AfxMessageBox(str);
		}
		else
		{
			AfxMessageBox("保存图片失败");
		}

	}
	else
	{
		AfxMessageBox("设备未连接");
	}*/
	
}
