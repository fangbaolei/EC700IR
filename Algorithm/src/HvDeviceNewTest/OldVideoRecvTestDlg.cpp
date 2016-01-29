#include "stdafx.h"
#include "OldVideoRecvTestDlg.h"
#include "OldSetVideoEnhanceFlagDlg.h"

extern CListCtrl* g_ListEx;
extern DWORD g_dwCurrentConnectIndexEx;

#define  WM_MESSAGE_GETFINISH	(WM_USER+120)
#define	 WM_PROCE_VIDEODATA		(WM_USER+121)

IMPLEMENT_DYNAMIC(COldVideoRecvTestDlg, CDialog)

COldVideoRecvTestDlg::COldVideoRecvTestDlg(HVAPI_HANDLE* phHandle, CWnd* pParent /* = NULL */)
: CDialog(COldVideoRecvTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDevice = phHandle;
	m_fIsConnectH264Video = FALSE;
	m_fIsRecvHistoryVideo = FALSE;

	m_iRedLightEnhanceFlag = 0;
	m_iBrightness = 0;
	m_iHueThreshold = 120;
	m_CompressRate = 80;

	m_dwLastStatus = CONN_STATUS_UNKNOWN;
	m_dwReconnectTimes = 0;

	InitializeCriticalSection(&m_csList);
	m_fIsQuit = FALSE;
	m_hProceThread = CreateThread(NULL, 0, ProceThread, this, 0, NULL);
}

COldVideoRecvTestDlg::~COldVideoRecvTestDlg()
{
	if(*m_hDevice)
	{
		HVAPI_SetCallBack(*m_hDevice, NULL, NULL, STREAM_TYPE_VIDEO, NULL);
	}

	if(m_hProceThread)
	{
		m_fIsQuit = TRUE;
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

	while (!m_VideoFrameData.IsEmpty())
	{
		VIDEO_PACKET_DATA cTmpInfo = m_VideoFrameData.RemoveHead();
		if (cTmpInfo.pVideoData)
		{
			delete [] cTmpInfo.pVideoData;
			cTmpInfo.pVideoData = NULL;
		}

		if(cTmpInfo.pszVideoExInfo)
		{
			delete[] cTmpInfo.pszVideoExInfo;
			cTmpInfo.pszVideoExInfo = NULL;
		}
	}
}

void COldVideoRecvTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COldVideoRecvTestDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_MESSAGE(WM_MESSAGE_GETFINISH, OnGetFinish)
	ON_MESSAGE(WM_PROCE_VIDEODATA, OnProceVideo)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL COldVideoRecvTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	CTime cTime = CTime::GetCurrentTime();
	char szTemp[20];
	sprintf(szTemp, "%d", cTime.GetYear());
	SetDlgItemText(IDC_EDIT2, szTemp);
	sprintf(szTemp, "%d", cTime.GetMonth());
	SetDlgItemText(IDC_EDIT3, szTemp);
	sprintf(szTemp, "%d", cTime.GetDay());
	SetDlgItemText(IDC_EDIT4, szTemp);
	sprintf(szTemp, "%d", cTime.GetHour());
	SetDlgItemText(IDC_EDIT5, szTemp);
	sprintf(szTemp, "%d", cTime.GetMinute()-1);
	SetDlgItemText(IDC_EDIT6, szTemp);
	sprintf(szTemp, "%d", cTime.GetSecond());
	SetDlgItemText(IDC_EDIT7, szTemp);
	SetDlgItemText(IDC_EDIT8, "60");
	SetTimer(1004, 1000, NULL);

	::GetClientRect(GetDlgItem(IDC_STATIC_VIDEO)->GetSafeHwnd(), &m_ShowFrameRect);
	return TRUE;
}

void COldVideoRecvTestDlg::OnCancel()
{
	if(m_fIsConnectH264Video)
	{
		HVAPI_SetCallBack(*m_hDevice, NULL, NULL, STREAM_TYPE_VIDEO, NULL);
	}
	if(m_fIsRecvHistoryVideo)
	{
		HVAPI_SetCallBack(*m_hDevice, NULL, NULL, STREAM_TYPE_VIDEO, NULL);
	}
	CDialog::OnCancel();
}

LRESULT COldVideoRecvTestDlg::OnGetFinish(WPARAM wParam, LPARAM lParam)
{
	HVAPI_SetCallBack(*m_hDevice, NULL, NULL, STREAM_TYPE_VIDEO, NULL);
	CTime cStartTime(m_dwStartTime);
	CTime cEndTime(m_dwEndTime);
	CString strInfo;
	strInfo.Format("成功获取从%s到%s时间片内的录像数据", 
		cStartTime.Format("%Y-%m-%d %H:%M:%S"), cEndTime.Format("%Y-%m-%d %H:%M:%S"));
	MessageBox(strInfo.GetBuffer());
	m_fIsRecvHistoryVideo = FALSE;
	SetDlgItemText(IDC_BUTTON2, "接收历史录像");
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	return S_OK;
}

INT COldVideoRecvTestDlg::OnH264Video(PVOID pUserData, PBYTE pbVideoData, DWORD dwVideoDataLen, DWORD dwVideoType, LPCSTR szVideoExtInfo)
{
	static DWORD dwStartTine = GetTickCount();
	static DWORD dwFrameCount = 0;
	if(pUserData == NULL || pbVideoData == NULL || dwVideoDataLen <= 0)
	{
		return -1;
	}
	COldVideoRecvTestDlg* pDlg = (COldVideoRecvTestDlg*)pUserData;
	dwFrameCount++;
	if(dwFrameCount >= 50)
	{
		float fFPS = (dwFrameCount / (float)(GetTickCount() - dwStartTine) * 1000);
		char szTmp[100];
		sprintf(szTmp, "FPS : %f", fFPS);
		pDlg->SetWindowText(szTmp);
		dwStartTine = GetTickCount();
		dwFrameCount = 0;
	}
	if(pDlg->m_VideoFrameData.IsFull())
	{
		return -1;
	}
	VIDEO_PACKET_DATA NewFrameData;
	NewFrameData.dwVideoType = dwVideoType;
	NewFrameData.dwVideoDataLen = dwVideoDataLen;
	NewFrameData.dwVideoExInfoLen = (DWORD)strlen(szVideoExtInfo);
	NewFrameData.pszVideoExInfo = new char[NewFrameData.dwVideoExInfoLen+1];
	if(NewFrameData.pszVideoExInfo == NULL)
	{
		return -1;
	}
	NewFrameData.pVideoData = new BYTE[dwVideoDataLen];
	if(NewFrameData.pVideoData == NULL)
	{
		delete[] NewFrameData.pszVideoExInfo;
		NewFrameData.pszVideoExInfo = NULL;
		return -1;
	}
	memcpy(NewFrameData.pszVideoExInfo, szVideoExtInfo, NewFrameData.dwVideoExInfoLen);
	NewFrameData.pszVideoExInfo[NewFrameData.dwVideoExInfoLen] = '\0';
	memcpy(NewFrameData.pVideoData, pbVideoData, dwVideoDataLen);

	EnterCriticalSection(&pDlg->m_csList);
	pDlg->m_VideoFrameData.AddTail(NewFrameData);
	LeaveCriticalSection(&pDlg->m_csList);
	return 0;
}

void COldVideoRecvTestDlg::OnBnClickedButton1()
{
	if(m_fIsRecvHistoryVideo)
	{
		return;
	}
	if(m_fIsConnectH264Video)
	{
		if(HVAPI_SetCallBack(*m_hDevice, NULL, NULL, STREAM_TYPE_VIDEO, NULL)
			!= S_OK)
		{
			return;
		}
		m_fIsConnectH264Video = FALSE;
		SetDlgItemText(IDC_BUTTON1, "接收H264视频");
		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
		m_fIsConnectH264Video = TRUE;
		if(HVAPI_SetCallBack(*m_hDevice, OnH264Video, this, STREAM_TYPE_VIDEO, NULL)
			!= S_OK)
		{
			GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
			m_fIsConnectH264Video = FALSE;
			return;
		}
		SetDlgItemText(IDC_BUTTON1, "断开H264连接");
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	}
}

void COldVideoRecvTestDlg::OnBnClickedButton3()
{
	COldSetVideoEnahnceFlagDlg pDlg(this);
	pDlg.DoModal();
}

void COldVideoRecvTestDlg::OnBnClickedButton2()
{
	if(m_fIsConnectH264Video)
	{
		return;
	}
	if(m_fIsRecvHistoryVideo)
	{
		if(HVAPI_SetCallBack(*m_hDevice, NULL, NULL, STREAM_TYPE_VIDEO, NULL)
			!= S_OK)
		{
			return;
		}
		m_fIsRecvHistoryVideo = FALSE;
		SetDlgItemText(IDC_BUTTON2, "接收历史录像");
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	}
	else
	{
		int iYear;
		int iMoth;
		int iDay;
		int iHour;
		int iMinit;
		int iSec;
		int iLen;
		char szTemp[20];
		GetDlgItemText(IDC_EDIT2, szTemp, 20);
		iYear = atoi(szTemp);
		GetDlgItemText(IDC_EDIT3, szTemp, 20);
		iMoth = atoi(szTemp);
		GetDlgItemText(IDC_EDIT4, szTemp, 20);
		iDay = atoi(szTemp);
		GetDlgItemText(IDC_EDIT5, szTemp, 20);
		iHour = atoi(szTemp);
		GetDlgItemText(IDC_EDIT6, szTemp, 20);
		iMinit = atoi(szTemp);
		GetDlgItemText(IDC_EDIT7, szTemp, 20);
		iSec = atoi(szTemp);
		GetDlgItemText(IDC_EDIT8, szTemp, 20);
		iLen = atoi(szTemp);
		CTime cStratTime(iYear, iMoth, iDay, iHour, iMinit, iSec);
		__time64_t dwStartTime = cStratTime.GetTime();
		__time64_t dwEndTime = dwStartTime + iLen;
		if(dwEndTime <= dwStartTime)
		{
			MessageBox("设置接收时间片区错误", "ERROR", MB_OK|MB_ICONERROR);
			return;
		}
		CTime cEndTime(dwEndTime);
		CString strCmd;
		strCmd.Format("DownloadVideo,BeginTime[%s],EndTime[%s],Enable[1]",
			cStratTime.Format("%Y.%m.%d_%H:%M:%S"), cEndTime.Format("%Y.%m.%d_%H:%M:%S"));

		m_dwStartTime = (DWORD)dwStartTime;
		m_dwEndTime = (DWORD)dwEndTime;
		GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
		m_fIsRecvHistoryVideo = TRUE;
		if(HVAPI_SetCallBack(*m_hDevice, OnH264Video, this, STREAM_TYPE_VIDEO, strCmd.GetBuffer())
			!= S_OK)
		{
			GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
			m_fIsRecvHistoryVideo = FALSE;
			return;
		}
		SetDlgItemText(IDC_BUTTON2, "停止接收");
		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	}
}

void COldVideoRecvTestDlg::OnTimer(UINT nIDEvent)
{
	if(*m_hDevice && nIDEvent == 1004)
	{
		DWORD dwConnStatus = CONN_STATUS_UNKNOWN;
		if(HVAPI_GetConnStatus(*m_hDevice, STREAM_TYPE_VIDEO, &dwConnStatus)
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
		g_ListEx->SetItemText(g_dwCurrentConnectIndexEx, 4, szTmpInfo);
	}
	CDialog::OnTimer(nIDEvent);
}

DWORD WINAPI COldVideoRecvTestDlg::ProceThread(LPVOID lParam)
{
	if(lParam == NULL)
	{
		return -1;
	}

	COldVideoRecvTestDlg* pDlg = (COldVideoRecvTestDlg*)lParam;
	while(!pDlg->m_fIsQuit)
	{
		if(pDlg->m_VideoFrameData.IsEmpty())
		{
			Sleep(100);
			continue;
		}

		VIDEO_PACKET_DATA TmpFrame;
		EnterCriticalSection(&pDlg->m_csList);
		TmpFrame = pDlg->m_VideoFrameData.RemoveHead();
		LeaveCriticalSection(&pDlg->m_csList);

		pDlg->SendMessage(WM_PROCE_VIDEODATA, 0, (LPARAM)&TmpFrame);

		if(TmpFrame.pVideoData)
		{
			delete[] TmpFrame.pVideoData;
			TmpFrame.pVideoData = NULL;
		}

		if(TmpFrame.pszVideoExInfo)
		{
			delete[] TmpFrame.pszVideoExInfo;
			TmpFrame.pszVideoExInfo = NULL;
		}
	}
	return 0;
}

LRESULT COldVideoRecvTestDlg::OnProceVideo(WPARAM wParam, LPARAM lParam)
{
	if(lParam == NULL) 
	{
		return E_FAIL;
	}
	VIDEO_PACKET_DATA* TmpVideo = (VIDEO_PACKET_DATA*)lParam;
	switch(TmpVideo->dwVideoType)
	{
	case VIDEO_TYPE_H264_NORMAL_I:
	case VIDEO_TYPE_H264_NORMAL_P:
		{
			CString strMsg;
			strMsg.Format("DataLen = %d, ExtInfo = %s",
				TmpVideo->dwVideoDataLen, TmpVideo->pszVideoExInfo);
			HDC TempDC = ::GetDC(GetDlgItem(IDC_STATIC_VIDEO)->GetSafeHwnd());
			SetBkMode(TempDC, 3);
			SetTextColor(TempDC, RGB(0, 128, 0));
			Rectangle(TempDC, 10, 20, 700, 80);
			TextOut(TempDC, 20, 20, "H264CallBack FrameInfo:", 23);
			int ilen = strMsg.GetLength();
			CString str1 = strMsg.Left(60);
			CString str2 = strMsg.Right(ilen - 60);
			TextOut(TempDC, 20, 40, str1.GetBuffer(), str1.GetLength());
			TextOut(TempDC, 20, 60, str2.GetBuffer(), str2.GetLength());
			//TextOut(TempDC, 20, 40, strMsg.GetBuffer(), strMsg.GetLength());
			::ReleaseDC(GetDlgItem(IDC_STATIC_VIDEO)->GetSafeHwnd(), TempDC);
		}
		break;
	case VIDEO_TYPE_H264_HISTORY_I:
	case VIDEO_TYPE_H264_HISTORY_P:
		{
			CString strMsg;
			strMsg.Format("DataLen = %d, ExtInfo = %s",
				TmpVideo->dwVideoDataLen, TmpVideo->pszVideoExInfo);
			HDC TempDC = ::GetDC(GetDlgItem(IDC_STATIC_VIDEO)->GetSafeHwnd());
			SetBkMode(TempDC, 3);
			SetTextColor(TempDC, RGB(0, 128, 0));
			Rectangle(TempDC, 10, 20, 700, 100);
			TextOut(TempDC, 20, 20, "H264CallBack FrameInfo:", 23);
			int ilen = strMsg.GetLength();
			CString str1 = strMsg.Left(60);
			CString str2 = strMsg.Right(ilen - 60);
			TextOut(TempDC, 20, 40, str1.GetBuffer(), str1.GetLength());
			TextOut(TempDC, 20, 60, str2.GetBuffer(), str2.GetLength());
			//TextOut(TempDC, 20, 40, strMsg.GetBuffer(), strMsg.GetLength());

			char* pszTime = strstr(TmpVideo->pszVideoExInfo, "FrameTime:");
			DWORD64 dw64TimeMS = 0;
			if(pszTime)
			{
				sscanf(pszTime, "FrameTime:%I64u", &dw64TimeMS);
				CTime cFrameTime(dw64TimeMS/1000);
				strMsg = cFrameTime.Format("%Y.%m.%d_%H:%M:%S");
				TextOut(TempDC, 20, 60, strMsg.GetBuffer(), strMsg.GetLength());
			}
			::ReleaseDC(GetDlgItem(IDC_STATIC_VIDEO)->GetSafeHwnd(), TempDC);
			if((dw64TimeMS/1000) >= m_dwEndTime)
			{
				::SendMessage(GetSafeHwnd(), WM_MESSAGE_GETFINISH, 0, 0);
			}
		}
		break;
	case VIDEO_TYPE_JPEG_HISTORY:
		{
			PBYTE pJPEGPos = NULL;
			DWORD dwJPEGDataLen = 0;
			int iRedLightCount = 0;
			int iRedLightBufPosLen = sizeof(RECT) * 20;
			PBYTE pRedLightPosBuf = new BYTE[iRedLightBufPosLen];
			if(HVAPIUTILS_ParseHistoryVideoFrame(TmpVideo->pVideoData, TmpVideo->dwVideoDataLen,
				iRedLightCount, iRedLightBufPosLen, pRedLightPosBuf, pJPEGPos, dwJPEGDataLen) != S_OK)
			{
				return E_FAIL;
			}
			if(pJPEGPos == NULL || dwJPEGDataLen <= 0)
			{
				return E_FAIL;
			}

			int iEnhanceBufLen = 0;
			PBYTE pEnhanceBuf = NULL;
			if(m_iRedLightEnhanceFlag == 1)
			{
				iEnhanceBufLen = (1024 << 10);
				pEnhanceBuf = new BYTE[iEnhanceBufLen];
				if(pEnhanceBuf)
				{
					if(HVAPIUTILS_TrafficLightEnhance(pJPEGPos, dwJPEGDataLen, iRedLightCount,
						pRedLightPosBuf, pEnhanceBuf, iEnhanceBufLen, m_iBrightness,
						m_iHueThreshold, m_CompressRate) != S_OK)
					{
						delete[] pEnhanceBuf;
						pEnhanceBuf = NULL;
						iEnhanceBufLen = 0;
					}
				}
			}

			IStream* pStm = NULL;
			CreateStreamOnHGlobal(NULL, TRUE, &pStm);
			IPicture* picholder;
			LARGE_INTEGER liTempStar = {0};
			pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
			ULONG iWritten = NULL;
			if(pEnhanceBuf)
			pStm->Write(pEnhanceBuf, iEnhanceBufLen, &iWritten);
			else
			pStm->Write(pJPEGPos, dwJPEGDataLen, &iWritten);
			pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
			if(pEnhanceBuf)
			{
				if(FAILED(OleLoadPicture(pStm, iEnhanceBufLen, TRUE, IID_IPicture, (void**)&picholder)))
				{
					pStm->Release();
					return E_FAIL;
				}
			}
			else
			{
				if(FAILED(OleLoadPicture(pStm, dwJPEGDataLen, TRUE, IID_IPicture, (void**)&picholder)))
				{
					pStm->Release();
					return E_FAIL;
				}
			}

			HDC TempDC;
			TempDC = ::GetDC(GetDlgItem(IDC_STATIC_VIDEO)->GetSafeHwnd());
			OLE_XSIZE_HIMETRIC hmWidth;
			OLE_YSIZE_HIMETRIC hmHeight;
			picholder->get_Width(&hmWidth);
			picholder->get_Height(&hmHeight);
			int nWidth =MulDiv(hmWidth, GetDeviceCaps(TempDC, LOGPIXELSX), 2540);
			int nHeight = MulDiv(hmHeight, GetDeviceCaps(TempDC, LOGPIXELSY), 2540);
			picholder->Render(TempDC, 0, 0, m_ShowFrameRect.right-m_ShowFrameRect.left,
				m_ShowFrameRect.bottom-m_ShowFrameRect.top,
				0, hmHeight, hmWidth, -hmHeight, NULL);
			::ReleaseDC(GetDlgItem(IDC_STATIC_VIDEO)->GetSafeHwnd(), TempDC);
			picholder->Release();
			pStm->Release();

			CString strMsg;
			strMsg.Format("DataLen = %d, ExtInfo = %s",
				TmpVideo->dwVideoDataLen, TmpVideo->pszVideoExInfo);
			TempDC = ::GetDC(GetDlgItem(IDC_STATIC_VIDEO)->GetSafeHwnd());
			SetBkMode(TempDC, 3);
			SetTextColor(TempDC, RGB(0, 128, 0));
			Rectangle(TempDC, 10, 220, 700, 280);
			TextOut(TempDC, 20, 220, "HistoryVideoCallback JPEG FrameInfo:", 23);
			TextOut(TempDC, 20, 240, strMsg.GetBuffer(), strMsg.GetLength());
			char* pszTime = strstr(TmpVideo->pszVideoExInfo, "FrameTime:");
			DWORD64 dw64TimeMS = 0;
			if(pszTime)
			{
				sscanf(pszTime, "FrameTime:%I64u", &dw64TimeMS);
			}
			CTime cFrameTime(dw64TimeMS/1000);
			strMsg = cFrameTime.Format("%Y.%m.%d_%H:%M:%S");
			TextOut(TempDC, 20, 260, strMsg.GetBuffer(), strMsg.GetLength());
			::ReleaseDC(GetDlgItem(IDC_STATIC_VIDEO)->GetSafeHwnd(), TempDC);
			SetWindowText(strMsg.GetBuffer());
			if((dw64TimeMS/1000) >= m_dwEndTime)
			{
				::SendMessage(GetSafeHwnd(), WM_MESSAGE_GETFINISH, 0, 0);
			}
		}
		break;
	}
	return S_OK;
}

