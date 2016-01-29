#include "stdafx.h"
#include "VideoRecvTestDlg.h"
#include ".\videorecvtestdlg.h"
#include "SetVideoEnhanceFlagDlg.h"

extern CListCtrl* g_pList;
extern DWORD g_dwCurrentConnectIndex;

#define  WM_MESSAGE_GETFINISH	(WM_USER+120)
#define	 WM_PROCE_VIDEODATA		(WM_USER+121)


static bool m_realfinish = false;
IMPLEMENT_DYNAMIC(CVideoRecvTestDlg, CDialog)

CVideoRecvTestDlg::CVideoRecvTestDlg(HVAPI_HANDLE_EX* phHandle, CWnd* pParent /* = NULL */)
: CDialog(CVideoRecvTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDevice = phHandle;
	m_fIsConnectH264Video = FALSE;
	m_fIsRecvHistoryVideo = FALSE;

	InitializeCriticalSection(&m_csList);
	m_fIsQuit = FALSE;
	m_hProceThread = CreateThread(NULL, 0, ProceThread, this, 0, NULL);
}

CVideoRecvTestDlg::~CVideoRecvTestDlg()
{
	KillTimer(1004);
	if(*m_hDevice)
	{
		HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_H264_VIDEO, NULL);
		HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_HISTORY_VIDEO, NULL);
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
		VideoFrameData cTmpInfo = m_VideoFrameData.RemoveHead();
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

void CVideoRecvTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVideoRecvTestDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_MESSAGE(WM_MESSAGE_GETFINISH, OnGetFinish)
	ON_MESSAGE(WM_PROCE_VIDEODATA, OnProceVideo)
	ON_WM_TIMER()
//	ON_WM_CTLCOLOR()
ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CVideoRecvTestDlg::OnInitDialog()
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

void CVideoRecvTestDlg::OnCancel()
{
	//if(m_fIsConnectH264Video)
	//{
	//	HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_H264_VIDEO, NULL);
	//}
	//if(m_fIsRecvHistoryVideo)
	//{
	//	HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_HISTORY_VIDEO, NULL);
	//}
	   CDialog::OnCancel();
//		DestroyWindow();
}

void CVideoRecvTestDlg::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
//	CDialog::PostNcDestroy();
//    delete this;
}


LRESULT CVideoRecvTestDlg::OnGetFinish(WPARAM wParam, LPARAM lParam)
{
	HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_HISTORY_VIDEO, NULL);
	CTime cStartTime(m_dwStartTime);
	CTime cEndTime(m_dwEndTime);
	CString strInfo;
	strInfo.Format("成功获取从%s到%s时间片内的录像数据", 
		cStartTime.Format("%Y-%m-%d %H:%M:%S"), cEndTime.Format("%Y-%m-%d %H:%M:%S"));
	MessageBox(strInfo.GetBuffer());
	m_fIsRecvHistoryVideo = FALSE;
	m_realfinish = true;
	SetDlgItemText(IDC_BUTTON2, "接收历史录像");
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	return S_OK;
}

INT CVideoRecvTestDlg::OnH264Video(PVOID pUserData, PBYTE pbVideoData, DWORD dwVideoDataLen, DWORD dwVideoType, LPCSTR szVideoExtInfo)
{
	static DWORD dwStartTine = GetTickCount();
	static DWORD dwFrameCount = 0;
	if(pUserData == NULL || pbVideoData == NULL || dwVideoDataLen <= 0)
	{
		return -1;
	}

	CVideoRecvTestDlg* pDlg = (CVideoRecvTestDlg*)pUserData;
	//dwFrameCount++;
	//if(dwFrameCount >= 50)
	//{
	//	float fFPS = (dwFrameCount / (float)(GetTickCount() - dwStartTine) * 1000);
	//	char szTmp[100];
	//	sprintf(szTmp, "FPS : %f", fFPS);
	//	pDlg->SetWindowText(szTmp);
	//	dwStartTine = GetTickCount();
	//	dwFrameCount = 0;
	//}
	if(pDlg->m_VideoFrameData.IsFull())
	{
		return -1;
	}
	VideoFrameData NewFrameData;
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

INT CVideoRecvTestDlg::OnH264VideoEx(
									 PVOID pUserData,  
									 DWORD dwVedioFlag,
									 DWORD dwVideoType, 
									 DWORD dwWidth,
									 DWORD dwHeight,
									 DWORD64 dw64TimeMS,
									 PBYTE pbVideoData, 
									 DWORD dwVideoDataLen,
									 LPCSTR szVideoExtInfo)
{
	static DWORD dwStartTine = GetTickCount();
	static DWORD dwFrameCount = 0;
	if(pUserData == NULL || pbVideoData == NULL || dwVideoDataLen <= 0)
	{
		return -1;
	}

	CVideoRecvTestDlg* pDlg = (CVideoRecvTestDlg*)pUserData;
	//dwFrameCount++;
	//if(dwFrameCount >= 50)
	//{
	//	float fFPS = (dwFrameCount / (float)(GetTickCount() - dwStartTine) * 1000);
	//	char szTmp[100];
	//	sprintf(szTmp, "FPS : %f", fFPS);
	//	pDlg->SetWindowText(szTmp);
	//	dwStartTine = GetTickCount();
	//	dwFrameCount = 0;
	//}
	if(pDlg->m_VideoFrameData.IsFull())
	{
		return -1;
	}
	VideoFrameData NewFrameData;
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

INT CVideoRecvTestDlg::OnHistoryVideo(PVOID pUserData, PBYTE pbVideoData, DWORD dwVideoDataLen, DWORD dwVideoType, LPCSTR szVideoExtInfo)
{
	if(pUserData == NULL || pbVideoData == NULL || dwVideoDataLen <= 0)
	{
		return -1;
	}
	CVideoRecvTestDlg* pDlg = (CVideoRecvTestDlg*)pUserData;
	if(pDlg->m_VideoFrameData.IsFull())
	{
		return -1;
	}
	VideoFrameData NewFrameData;
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

INT CVideoRecvTestDlg::OnHistoryVideoEx(
										PVOID pUserData,  
										DWORD dwVedioFlag,
										DWORD dwVideoType, 
										DWORD dwWidth,
										DWORD dwHeight,
										DWORD64 dw64TimeMS,
										PBYTE pbVideoData, 
										DWORD dwVideoDataLen,
										LPCSTR szVideoExtInfo)
{
	if(pUserData == NULL || pbVideoData == NULL || dwVideoDataLen <= 0)
	{
		return -1;
	}
	CVideoRecvTestDlg* pDlg = (CVideoRecvTestDlg*)pUserData;
	if(pDlg->m_VideoFrameData.IsFull())
	{
		return -1;
	}
	VideoFrameData NewFrameData;
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

void CVideoRecvTestDlg::OnBnClickedButton1()
{
	if(m_fIsRecvHistoryVideo)
	{
		return;
	}
	if(m_fIsConnectH264Video)
	{
		//if(HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_H264_VIDEO, NULL)
		//	!= S_OK)
		if ( S_OK != HVAPI_StopRecvH264Video(*m_hDevice ) )
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
		//if(HVAPI_SetCallBackEx(*m_hDevice, OnH264Video, this, 0, CALLBACK_TYPE_H264_VIDEO, NULL)
		//	!= S_OK)

		if ( S_OK != HVAPI_StartRecvH264Video(*m_hDevice ,OnH264VideoEx , this , 0
					, 0 , 0 , H264_RECV_FLAG_REALTIME ) )
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

void CVideoRecvTestDlg::OnBnClickedButton3()
{
	if(*m_hDevice == NULL)
	{
		return;
	}
	CSetVideoEnahnceFlagDlg pDlg(m_hDevice, this);
	pDlg.DoModal();
}

void CVideoRecvTestDlg::OnBnClickedButton2()
{
	if(m_fIsConnectH264Video)
	{
		return;
	}
	if(m_fIsRecvHistoryVideo)
	{
		//if(HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_HISTORY_VIDEO, NULL)
		//	!= S_OK)
		if ( S_OK != HVAPI_StopRecvH264Video(*m_hDevice ) )
		{
			return;
		}
		m_fIsRecvHistoryVideo = FALSE;
		m_realfinish = true;
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
		//if(HVAPI_SetCallBackEx(*m_hDevice, OnHistoryVideo, this, 0, CALLBACK_TYPE_HISTORY_VIDEO, strCmd.GetBuffer())
		//	!= S_OK)
		if ( S_OK != HVAPI_StartRecvH264Video(*m_hDevice ,OnHistoryVideoEx , this , 0
			, dwStartTime , dwEndTime , H264_RECV_FLAG_HISTORY ) )
		{
			GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
			m_fIsRecvHistoryVideo = FALSE;
			return;
		}
		m_realfinish = false;
		SetDlgItemText(IDC_BUTTON2, "停止接收");
		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	}
}

void CVideoRecvTestDlg::OnTimer(UINT nIDEvent)
{
	if(m_hDevice && nIDEvent == 1004)
	{
		DWORD dwReConnectTimes = 0;
		if(HVAPI_GetReConnectTimesEx(*m_hDevice, CONN_TYPE_VIDEO, &dwReConnectTimes, FALSE) == S_OK)
		{
			char szTmpInfo[20] = {0};
			sprintf(szTmpInfo, "%d", dwReConnectTimes);
			g_pList->SetItemText(g_dwCurrentConnectIndex, 6, szTmpInfo);
		}

		DWORD dwConnectStatus = CONN_STATUS_UNKNOWN;
		if(HVAPI_GetConnStatusEx(*m_hDevice, CONN_TYPE_VIDEO, &dwConnectStatus) == S_OK)
		{
			if(dwConnectStatus == CONN_STATUS_RECVDONE)
			{
				if(m_fIsRecvHistoryVideo)
				{
					m_fIsRecvHistoryVideo = FALSE;
					HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_HISTORY_VIDEO, NULL);
					MessageBox("识别器已主动断开连接,接收完成,如未收到任\n何数据可能是设置的时间片内不存在历史录像数据");
					m_realfinish = true;
					SetDlgItemText(IDC_BUTTON2, "接收历史录像");
					GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
				}
			}
		}
	}
	CDialog::OnTimer(nIDEvent);
}

DWORD WINAPI CVideoRecvTestDlg::ProceThread(LPVOID lParam)
{
	if(lParam == NULL)
	{
		return -1;
	}

	CVideoRecvTestDlg* pDlg = (CVideoRecvTestDlg*)lParam;
	while(!pDlg->m_fIsQuit)
	{
		if(pDlg->m_VideoFrameData.IsEmpty())
		{
			Sleep(100);
			continue;
		}

		VideoFrameData TmpFrame;
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

LRESULT CVideoRecvTestDlg::OnProceVideo(WPARAM wParam, LPARAM lParam)
{
	if(lParam == NULL) 
	{
		return E_FAIL;
	}
	VideoFrameData* TmpVideo = (VideoFrameData*)lParam;
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
				TextOut(TempDC, 400, 60, strMsg.GetBuffer(), strMsg.GetLength());
			}
			::ReleaseDC(GetDlgItem(IDC_STATIC_VIDEO)->GetSafeHwnd(), TempDC);
			if(((dw64TimeMS/1000) >= m_dwEndTime)&&m_realfinish == false)
			{
				::SendMessage(GetSafeHwnd(), WM_MESSAGE_GETFINISH, 0, 0);
			}
		}
		break;
	case VIDEO_TYPE_JPEG_HISTORY:
		{
			IStream* pStm = NULL;
			CreateStreamOnHGlobal(NULL, TRUE, &pStm);
			IPicture* picholder;
			LARGE_INTEGER liTempStar = {0};
			pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
			ULONG iWritten = NULL;
			pStm->Write(TmpVideo->pVideoData, TmpVideo->dwVideoDataLen, &iWritten);
			pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
			if(FAILED(OleLoadPicture(pStm, TmpVideo->dwVideoDataLen, TRUE, IID_IPicture, (void**)&picholder)))
			{
				pStm->Release();
				return 0;
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
			if(((dw64TimeMS/1000) >= m_dwEndTime)&&m_realfinish == false)
			{
				::SendMessage(GetSafeHwnd(), WM_MESSAGE_GETFINISH, 0, 0);
			}
		}
		break;
	}
	
	return S_OK;
}


void CVideoRecvTestDlg::OnClose()
{

	if (IDYES != ::MessageBox(m_hWnd, "是否退出？", "",
		MB_ICONINFORMATION | MB_YESNO))
	{
		return;
	}

	CMenu* pMenu = this->GetSystemMenu(FALSE);
	pMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);

	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
    

	KillTimer(1004);
	if(*m_hDevice)
	{
		HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_H264_VIDEO, NULL);
		HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_HISTORY_VIDEO, NULL);
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
		VideoFrameData cTmpInfo = m_VideoFrameData.RemoveHead();
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

	CDialog::OnClose();
}
