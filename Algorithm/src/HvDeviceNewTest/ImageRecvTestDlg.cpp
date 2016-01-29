#include "stdafx.h"
#include "ImageRecvTestDlg.h"
#include ".\imagerecvtestdlg.h"
#include <gdiplus.h>

IMPLEMENT_DYNAMIC(CImageRecvTestDlg, CDialog)

#define WM_PROCE_JPEGDATA	(WM_USER + 500)
#define WM_UPDATE_FPS		(WM_USER + 600)

#define WM_USERDEFMSG_IMAGE (WM_USER + 106) 

extern CListCtrl* g_pList;
extern DWORD g_dwCurrentConnectIndex;

//extern char g_szIniFile[MAX_PATH];
char g_szIniFile[MAX_PATH] = {0};


CImageRecvTestDlg::CImageRecvTestDlg(HVAPI_HANDLE_EX* phHandle, CWnd* pParent /* = NULL */)
: CDialog(CImageRecvTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDevice = phHandle;
	m_fIsShowLpr = FALSE;

//	m_nLprSet = 0;

	//InitializeCriticalSection(&m_csList);
	//m_fIsQuit = FALSE;
	//m_hProceThread = CreateThread(NULL, 0, ProceThread, this, 0, NULL);
}

CImageRecvTestDlg::~CImageRecvTestDlg()
{
	/*if(m_hProceThread)
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
	}*/

	/*while (!m_JpegFrameData.IsEmpty())
	{
		JpegFrameData cTmpInfo = m_JpegFrameData.RemoveHead();
		if (cTmpInfo.pImageData)
		{
			delete [] cTmpInfo.pImageData;
			cTmpInfo.pImageData = NULL;
		}

		if(cTmpInfo.pszImageExInfo)
		{
			delete[] cTmpInfo.pszImageExInfo;
			cTmpInfo.pszImageExInfo = NULL;
		}
	}*/
}

void CImageRecvTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageRecvTestDlg, CDialog)
	ON_MESSAGE(WM_UPDATE_FPS, OnUpdateFps)
	ON_MESSAGE(WM_PROCE_JPEGDATA, OnProceJpegFrame)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_BTN_CAP, OnBnClickedBtnCap)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static DWORD dwFrameCount1 = 0;

BOOL CImageRecvTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	MoveWindow((GetSystemMetrics(SM_CXSCREEN)-1024)>>1,
		(GetSystemMetrics(SM_CYSCREEN)-768)>>1, 1024, 768, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_JPEG)->GetSafeHwnd(), 5, 5, 500, 360, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_LPR)->GetSafeHwnd(), 515, 5, 500, 360, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_CAP)->GetSafeHwnd(), 5, 375, 500, 360, TRUE);
	::MoveWindow(GetDlgItem(IDC_RADIO1)->GetSafeHwnd(), 655, 420, 100, 20, TRUE);
	::MoveWindow(GetDlgItem(IDC_RADIO2)->GetSafeHwnd(), 655, 445, 80, 20, TRUE);
	//::MoveWindow(GetDlgItem(IDC_RADIO2)->GetSafeHwnd(), 655, 420, 80, 20, TRUE);
	//::MoveWindow(GetDlgItem(IDC_RADIO1)->GetSafeHwnd(), 655, 445, 100, 20, TRUE);

	::MoveWindow(GetDlgItem(IDC_BTN_CAP)->GetSafeHwnd(), 750, 445, 80, 20, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC)->GetSafeHwnd(), 655, 480, 120, 20, TRUE);
	::MoveWindow(GetDlgItem(IDC_RADIO3)->GetSafeHwnd(), 780, 480, 80, 20, TRUE);
	::MoveWindow(GetDlgItem(IDC_RADIO4)->GetSafeHwnd(), 860, 480, 80, 20, TRUE);



	//CButton* pBut = (CButton*)GetDlgItem(IDC_RADIO1);
	//pBut->SetCheck(1);
	//OnBnClickedRadio1();

	//CButton* pBut2 = (CButton*)GetDlgItem(IDC_RADIO2);
	//pBut2->SetCheck(1);
	//OnBnClickedRadio2();

	CButton* pBut3 =(CButton*)GetDlgItem(IDC_RADIO3);
	CButton* pBut4 =(CButton*)GetDlgItem(IDC_RADIO4);
	CButton* pButCap =(CButton*)GetDlgItem(IDC_BTN_CAP);
	pBut3->EnableWindow(FALSE);
	pBut4->EnableWindow(FALSE);
	
	GetDlgItem(IDC_STATIC)->ShowWindow(FALSE);
	pBut3->ShowWindow(FALSE);
	pBut4->ShowWindow(FALSE);
	pButCap->ShowWindow(FALSE);
	
	SetTimer(1003, 1000, NULL);
	dwFrameCount1 = 0;

//	HVAPI_SetCallBackEx(*m_hDevice, OnImage, this, 0, CALLBACK_TYPE_JPEG_FRAME, "SetImgType,EnableRecogVideo[1]");
	return TRUE;
}


int CImageRecvTestDlg::OnImage(PVOID pUserData, PBYTE pbImageData, DWORD dwImageDataLen, DWORD dwImageType, LPCSTR szImageExtInfo)
{

	static DWORD dwStartTine = GetTickCount();
	static DWORD dwFrameCount = 0;
	if(pUserData == NULL)
	{
		return -1;
	}
	CImageRecvTestDlg* pDlg = (CImageRecvTestDlg*)pUserData;

	//CString str; 
	//str.Format("dwImageType=%0X",dwImageType);

	if(dwImageType == IMAGE_TYPE_JPEG_NORMAL)
	{
		OutputDebugString("IMAGE_TYPE_JPEG_NORMAL\n");
	}
	if(dwImageType == IMAGE_TYPE_JPEG_LPR)
	{
		OutputDebugString("IMAGE_TYPE_JPEG_LPR\n");
	}

	// 1y??
	if(pDlg->m_fIsShowLpr)
	{
		if(dwImageType == IMAGE_TYPE_JPEG_NORMAL)
		{
			OutputDebugString("Type: Jpeg\n");
			return 1;
		}
	}
	else
	{
		if(dwImageType == IMAGE_TYPE_JPEG_LPR)
		{
			OutputDebugString("Type: LPR\n");
			return 1;
		}
	}

	IStream* pStm = NULL;
	CreateStreamOnHGlobal(NULL, TRUE, &pStm);
	IPicture* picholder;
	LARGE_INTEGER liTempStar = {0};
	pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
	ULONG iWritten = NULL;
	pStm->Write(pbImageData, dwImageDataLen, &iWritten);
	pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
	if(FAILED(OleLoadPicture(pStm, dwImageDataLen, TRUE, IID_IPicture, (void**)&picholder)))
	{
		pStm->Release();
		return 0;
	}

	if(pDlg->m_fIsShowLpr)
	{
		if(dwImageType == IMAGE_TYPE_JPEG_LPR)
		{
			dwFrameCount1++;
			dwFrameCount++;
		}
		if(dwFrameCount >= 20)
		{
			float fFPS = (dwFrameCount / (float)((GetTickCount() - dwStartTine) / 1000.));
			char szTmp[100];
			sprintf(szTmp, "FPS : %f, %d", fFPS, dwFrameCount1);
			pDlg->SendMessage(WM_UPDATE_FPS, 0, (WPARAM)szTmp);
			dwStartTine = GetTickCount();
			dwFrameCount = 0;
		}
	}
	else 
	{

		if(dwImageType == IMAGE_TYPE_JPEG_NORMAL)
		{
			dwFrameCount++;
		}
		if(dwFrameCount >= 50)
		{
			float fFPS = (dwFrameCount / (float)((GetTickCount() - dwStartTine) / 1000.));
			char szTmp[100];
			sprintf(szTmp, "FPS : %f, %d", fFPS, GetTickCount());
			pDlg->SendMessage(WM_UPDATE_FPS, 0, (WPARAM)szTmp);
			dwStartTine = GetTickCount();
			dwFrameCount = 0;
		}
	}

	HDC TempDC;
	RECT cRectShowPos;
	switch(dwImageType)
	{
	case IMAGE_TYPE_JPEG_NORMAL:
		::GetClientRect(pDlg->GetDlgItem(IDC_STATIC_JPEG)->GetSafeHwnd(), &cRectShowPos);
		TempDC = ::GetDC(pDlg->GetDlgItem(IDC_STATIC_JPEG)->GetSafeHwnd());
		break;
	case IMAGE_TYPE_JPEG_CAPTURE:
		::GetClientRect(pDlg->GetDlgItem(IDC_STATIC_CAP)->GetSafeHwnd(), &cRectShowPos);
		TempDC = ::GetDC(pDlg->GetDlgItem(IDC_STATIC_CAP)->GetSafeHwnd());
		break;
	case IMAGE_TYPE_JPEG_LPR:
		::GetClientRect(pDlg->GetDlgItem(IDC_STATIC_LPR)->GetSafeHwnd(), &cRectShowPos);
		TempDC = ::GetDC(pDlg->GetDlgItem(IDC_STATIC_LPR)->GetSafeHwnd());
		break;
	}
	OLE_XSIZE_HIMETRIC hmWidth;
	OLE_YSIZE_HIMETRIC hmHeight;
	picholder->get_Width(&hmWidth);
	picholder->get_Height(&hmHeight);
	int nWidth =MulDiv(hmWidth, GetDeviceCaps(TempDC, LOGPIXELSX), 2540);
	int nHeight = MulDiv(hmHeight, GetDeviceCaps(TempDC, LOGPIXELSY), 2540);
//	picholder->Render(TempDC, 0, 0, 640, 400, 0, hmHeight, hmWidth, -hmHeight, NULL);
	picholder->Render(TempDC, 0, 0, 500, 360, 0, hmHeight, hmWidth, -hmHeight, NULL);

	char* pPlatePosInfo = strstr(szImageExtInfo, "PlatePosInfo:");
	if(pPlatePosInfo)
	{
		int iWidth = cRectShowPos.right - cRectShowPos.left;
		int iHeight = cRectShowPos.bottom - cRectShowPos.top;
		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
		SelectObject(TempDC, hPen);
		pPlatePosInfo += 13;
		DWORD dwPlateCount;
		memcpy(&dwPlateCount, pPlatePosInfo, sizeof(DWORD));
		pPlatePosInfo += 4;
		for(int iIndex=0; iIndex<(int)dwPlateCount; iIndex++)
		{
			RECT cRect;
			memcpy(&cRect, pPlatePosInfo, sizeof(cRect));
			pPlatePosInfo += sizeof(cRect);
			
			cRect.left = (int)(cRect.left * iWidth / (float)nWidth);
			cRect.right = (int)(cRect.right * iWidth / (float)nWidth);
			cRect.top = (int)(cRect.top * iHeight / (float)nHeight);
			cRect.bottom = (int)(cRect.bottom * iHeight / (float)nHeight);
			MoveToEx(TempDC, cRect.left, cRect.top, NULL);
			LineTo(TempDC, cRect.right, cRect.top);
			LineTo(TempDC, cRect.right, cRect.bottom);
			LineTo(TempDC, cRect.left, cRect.bottom);
			LineTo(TempDC, cRect.left, cRect.top);
		}
		DeleteObject(hPen);
	}

	switch(dwImageType)
	{
	case IMAGE_TYPE_JPEG_NORMAL:
		::ReleaseDC(pDlg->GetDlgItem(IDC_STATIC_JPEG)->GetSafeHwnd(), TempDC);
		break;
	case IMAGE_TYPE_JPEG_CAPTURE:
		::ReleaseDC(pDlg->GetDlgItem(IDC_STATIC_CAP)->GetSafeHwnd(), TempDC);
		break;
	case IMAGE_TYPE_JPEG_LPR:
		::ReleaseDC(pDlg->GetDlgItem(IDC_STATIC_LPR)->GetSafeHwnd(), TempDC);
		break;
	}
	picholder->Release();
	pStm->Release();
	
	return 0;
}







int CImageRecvTestDlg::OnImageEx(
								 PVOID pUserData,  
								 DWORD dwImageFlag,
								 DWORD dwImageType, 
								 DWORD dwWidth,
								 DWORD dwHeight,
								 DWORD64 dw64TimeMS,
								 PBYTE pbImageData, 
								 DWORD dwImageDataLen,
								 LPCSTR szImageExtInfo)
{
	static DWORD dwStartTine = GetTickCount();
	static DWORD dwFrameCount = 0;
	if(pUserData == NULL)
	{
		return -1;
	}
	CImageRecvTestDlg* pDlg = (CImageRecvTestDlg*)pUserData;

	//CString str; 
	//str.Format("dwImageType=%0X",dwImageType);

	if(dwImageType == IMAGE_TYPE_JPEG_NORMAL)
	{
		OutputDebugString("IMAGE_TYPE_JPEG_NORMAL\n");
	}
	if(dwImageType == IMAGE_TYPE_JPEG_LPR)
	{
		OutputDebugString("IMAGE_TYPE_JPEG_LPR\n");
	}

	// 1y??
	if(pDlg->m_fIsShowLpr)
	{
		if(dwImageType == IMAGE_TYPE_JPEG_NORMAL)
		{
			OutputDebugString("Type: Jpeg\n");
			return 1;
		}
	}
	else
	{
		if(dwImageType == IMAGE_TYPE_JPEG_LPR)
		{
			OutputDebugString("Type: LPR\n");
			return 1;
		}
	}

	IStream* pStm = NULL;
	CreateStreamOnHGlobal(NULL, TRUE, &pStm);
	IPicture* picholder;
	LARGE_INTEGER liTempStar = {0};
	pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
	ULONG iWritten = NULL;
	pStm->Write(pbImageData, dwImageDataLen, &iWritten);
	pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
	if(FAILED(OleLoadPicture(pStm, dwImageDataLen, TRUE, IID_IPicture, (void**)&picholder)))
	{
		pStm->Release();
		return 0;
	}

	if(pDlg->m_fIsShowLpr)
	{
		if(dwImageType == IMAGE_TYPE_JPEG_LPR)
		{
			dwFrameCount1++;
			dwFrameCount++;
		}
		if(dwFrameCount >= 20)
		{
			float fFPS = (dwFrameCount / (float)((GetTickCount() - dwStartTine) / 1000.));
			char szTmp[100];
			sprintf(szTmp, "FPS : %f, %d", fFPS, dwFrameCount1);
			pDlg->SendMessage(WM_UPDATE_FPS, 0, (WPARAM)szTmp);
			dwStartTine = GetTickCount();
			dwFrameCount = 0;
		}
	}
	else 
	{

		if(dwImageType == IMAGE_TYPE_JPEG_NORMAL)
		{
			dwFrameCount++;
		}
		if(dwFrameCount >= 50)
		{
			float fFPS = (dwFrameCount / (float)((GetTickCount() - dwStartTine) / 1000.));
			char szTmp[100];
			sprintf(szTmp, "FPS : %f, %d", fFPS, GetTickCount());
			pDlg->SendMessage(WM_UPDATE_FPS, 0, (WPARAM)szTmp);
			dwStartTine = GetTickCount();
			dwFrameCount = 0;
		}
	}

	HDC TempDC;
	RECT cRectShowPos;
	switch(dwImageType)
	{
	case IMAGE_TYPE_JPEG_NORMAL:
		::GetClientRect(pDlg->GetDlgItem(IDC_STATIC_JPEG)->GetSafeHwnd(), &cRectShowPos);
		TempDC = ::GetDC(pDlg->GetDlgItem(IDC_STATIC_JPEG)->GetSafeHwnd());
		break;
	case IMAGE_TYPE_JPEG_CAPTURE:
		::GetClientRect(pDlg->GetDlgItem(IDC_STATIC_CAP)->GetSafeHwnd(), &cRectShowPos);
		TempDC = ::GetDC(pDlg->GetDlgItem(IDC_STATIC_CAP)->GetSafeHwnd());
		break;
	case IMAGE_TYPE_JPEG_LPR:
		::GetClientRect(pDlg->GetDlgItem(IDC_STATIC_LPR)->GetSafeHwnd(), &cRectShowPos);
		TempDC = ::GetDC(pDlg->GetDlgItem(IDC_STATIC_LPR)->GetSafeHwnd());
		break;
	}
	OLE_XSIZE_HIMETRIC hmWidth;
	OLE_YSIZE_HIMETRIC hmHeight;
	picholder->get_Width(&hmWidth);
	picholder->get_Height(&hmHeight);
	int nWidth =MulDiv(hmWidth, GetDeviceCaps(TempDC, LOGPIXELSX), 2540);
	int nHeight = MulDiv(hmHeight, GetDeviceCaps(TempDC, LOGPIXELSY), 2540);
	//	picholder->Render(TempDC, 0, 0, 640, 400, 0, hmHeight, hmWidth, -hmHeight, NULL);
	picholder->Render(TempDC, 0, 0, 500, 360, 0, hmHeight, hmWidth, -hmHeight, NULL);

	char* pPlatePosInfo = strstr(szImageExtInfo, "PlatePosInfo:");
	if(pPlatePosInfo)
	{
		int iWidth = cRectShowPos.right - cRectShowPos.left;
		int iHeight = cRectShowPos.bottom - cRectShowPos.top;
		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
		SelectObject(TempDC, hPen);
		pPlatePosInfo += 13;
		DWORD dwPlateCount;
		memcpy(&dwPlateCount, pPlatePosInfo, sizeof(DWORD));
		pPlatePosInfo += 4;
		for(int iIndex=0; iIndex<(int)dwPlateCount; iIndex++)
		{
			RECT cRect;
			memcpy(&cRect, pPlatePosInfo, sizeof(cRect));
			pPlatePosInfo += sizeof(cRect);

			cRect.left = (int)(cRect.left * iWidth / (float)nWidth);
			cRect.right = (int)(cRect.right * iWidth / (float)nWidth);
			cRect.top = (int)(cRect.top * iHeight / (float)nHeight);
			cRect.bottom = (int)(cRect.bottom * iHeight / (float)nHeight);
			MoveToEx(TempDC, cRect.left, cRect.top, NULL);
			LineTo(TempDC, cRect.right, cRect.top);
			LineTo(TempDC, cRect.right, cRect.bottom);
			LineTo(TempDC, cRect.left, cRect.bottom);
			LineTo(TempDC, cRect.left, cRect.top);
		}
		DeleteObject(hPen);
	}

	switch(dwImageType)
	{
	case IMAGE_TYPE_JPEG_NORMAL:
		::ReleaseDC(pDlg->GetDlgItem(IDC_STATIC_JPEG)->GetSafeHwnd(), TempDC);
		break;
	case IMAGE_TYPE_JPEG_CAPTURE:
		::ReleaseDC(pDlg->GetDlgItem(IDC_STATIC_CAP)->GetSafeHwnd(), TempDC);
		break;
	case IMAGE_TYPE_JPEG_LPR:
		::ReleaseDC(pDlg->GetDlgItem(IDC_STATIC_LPR)->GetSafeHwnd(), TempDC);
		break;
	}
	picholder->Release();
	pStm->Release();

	return 0;
}




void CImageRecvTestDlg::OnTimer(UINT nIDEvent)
{
	if(*m_hDevice && nIDEvent == 1003)
	{
		DWORD dwReConnectTimes = 0;
		HVAPI_GetReConnectTimesEx(*m_hDevice, CONN_TYPE_IMAGE, &dwReConnectTimes, FALSE);
		char szTmpInfo[20] = {0};
		sprintf(szTmpInfo, "%d", dwReConnectTimes);
		g_pList->SetItemText(g_dwCurrentConnectIndex, 5, szTmpInfo);
	}

	CDialog::OnTimer(nIDEvent);
}

/*DWORD WINAPI CImageRecvTestDlg::ProceThread(LPVOID lpParam)
{
	if(lpParam == NULL)
	{
		return -1;
	}

	CImageRecvTestDlg* pDlg = (CImageRecvTestDlg*)lpParam;
	while(!pDlg->m_fIsQuit)
	{
		if(pDlg->m_JpegFrameData.IsEmpty())
		{
			Sleep(100);
			continue;
		}

		JpegFrameData TmpFrame;
		EnterCriticalSection(&pDlg->m_csList);
		TmpFrame = pDlg->m_JpegFrameData.RemoveHead();
		LeaveCriticalSection(&pDlg->m_csList);

		pDlg->SendMessage(WM_PROCE_JPEGDATA, 0, (LPARAM)&TmpFrame);

		if(TmpFrame.pImageData)
		{
			delete[] TmpFrame.pImageData;
			TmpFrame.pImageData = NULL;
		}

		if(TmpFrame.pszImageExInfo)
		{
			delete[] TmpFrame.pszImageExInfo;
			TmpFrame.pszImageExInfo = NULL;
		}
	}
	return 0;
}*/

LRESULT CImageRecvTestDlg::OnUpdateFps(WPARAM wParam, LPARAM lParam)
{
	if(lParam == NULL)
	{
		return E_FAIL;
	}
	SetWindowText((char*)lParam);
	return S_OK;
}

LRESULT CImageRecvTestDlg::OnProceJpegFrame(WPARAM wParam, LPARAM lParam)
{
	if(lParam == NULL)
	{
		return E_FAIL;
	}
	//JpegFrameData* TmpJpegFrame = (JpegFrameData*)lParam;
	//CString strTmp;
	//strTmp.Format("FrameLen = %d", TmpJpegFrame->dwImageDataLen);
	//SetWindowText(strTmp.GetBuffer());
	//IStream* pStm = NULL;
	//CreateStreamOnHGlobal(NULL, TRUE, &pStm);
	//IPicture* picholder;
	//LARGE_INTEGER liTempStar = {0};
	//pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
	//ULONG iWritten = NULL;
	//pStm->Write(TmpJpegFrame->pImageData, TmpJpegFrame->dwImageDataLen, &iWritten);
	//pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
	//if(FAILED(OleLoadPicture(pStm, TmpJpegFrame->dwImageDataLen, TRUE, IID_IPicture, (void**)&picholder)))
	//{
	//	pStm->Release();
	//	return E_FAIL;
	//}

	//HDC TempDC;
	//switch(TmpJpegFrame->dwImageType)
	//{
	//case IMAGE_TYPE_JPEG_NORMAL:
	//	TempDC = ::GetDC(GetDlgItem(IDC_STATIC_JPEG)->GetSafeHwnd());
	//	break;
	//case IMAGE_TYPE_JPEG_CAPTURE:
	//	TempDC = ::GetDC(GetDlgItem(IDC_STATIC_CAP)->GetSafeHwnd());
	//	break;
	//case IMAGE_TYPE_JPEG_LPR:
	//	TempDC = ::GetDC(GetDlgItem(IDC_STATIC_LPR)->GetSafeHwnd());
	//	break;
	//}
	//OLE_XSIZE_HIMETRIC hmWidth;
	//OLE_YSIZE_HIMETRIC hmHeight;
	//picholder->get_Width(&hmWidth);
	//picholder->get_Height(&hmHeight);
	//int nWidth =MulDiv(hmWidth, GetDeviceCaps(TempDC, LOGPIXELSX), 2540);
	//int nHeight = MulDiv(hmHeight, GetDeviceCaps(TempDC, LOGPIXELSY), 2540);
	//picholder->Render(TempDC, 0, 0, 640, 400, 0, hmHeight, hmWidth, -hmHeight, NULL);
	//switch(TmpJpegFrame->dwImageType)
	//{
	//case IMAGE_TYPE_JPEG_NORMAL:
	//	::ReleaseDC(GetDlgItem(IDC_STATIC_JPEG)->GetSafeHwnd(), TempDC);
	//	break;
	//case IMAGE_TYPE_JPEG_CAPTURE:
	//	::ReleaseDC(GetDlgItem(IDC_STATIC_CAP)->GetSafeHwnd(), TempDC);
	//	break;
	//case IMAGE_TYPE_JPEG_LPR:
	//	::ReleaseDC(GetDlgItem(IDC_STATIC_LPR)->GetSafeHwnd(), TempDC);
	//	break;
	//}
	//picholder->Release();
	//pStm->Release();
	return S_OK;
}

void CImageRecvTestDlg::GetIP(CString strIP)
{
	m_strIP = strIP;
}

void CImageRecvTestDlg::OnBnClickedRadio1()
{
	CButton* pBut = (CButton*)GetDlgItem(IDC_RADIO1);
	CButton* pBut3 =(CButton*)GetDlgItem(IDC_RADIO3);
	CButton* pBut4 =(CButton*)GetDlgItem(IDC_RADIO4);

	if(pBut->GetCheck() == 1)
	{
		m_fIsShowLpr = FALSE;
		pBut3->EnableWindow(FALSE);
		pBut4->EnableWindow(FALSE);
	}
	else
	{
		m_fIsShowLpr = TRUE;
		pBut3->EnableWindow(TRUE);
		pBut4->EnableWindow(TRUE);
	}

	CString strBuf = "SetEncodeMode,EncodeMode[0]";
	char szRetBuf[128*1024] = {0};
	INT nRetLen = 0;
		

	int iDeviceType =0;
	HVAPI_GetDevTypeEx( (LPSTR)(LPCTSTR)m_strIP, &iDeviceType);

	if ( S_OK == HVAPI_ExecCmdEx(*m_hDevice, strBuf, szRetBuf, sizeof(szRetBuf), &nRetLen))
	{	
		if(iDeviceType == DEV_TYPE_HVCAM_SINGLE)
		{
			HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_JPEG_FRAME,NULL);
			HVAPI_SetCallBackEx(*m_hDevice, OnImage, this, 0, CALLBACK_TYPE_JPEG_FRAME,NULL);
		}
		else if(iDeviceType == DEV_TYPE_HVCAM_200W || iDeviceType == DEV_TYPE_HVCAM_500W || iDeviceType == DEV_TYPE_HVMERCURY)
		{
			//HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_JPEG_FRAME,NULL);
			//HVAPI_SetCallBackEx(*m_hDevice, OnImage, this, 0, CALLBACK_TYPE_JPEG_FRAME,"SetImgType,EnableRecogVideo[0]");
			
			//ÐÂ½Ó¿Ú
			HVAPI_StopRecvMJPEG( *m_hDevice );
			HVAPI_StartRecvMJPEG( *m_hDevice , OnImageEx ,this , 0 , MJPEG_RECV_FLAG_REALTIME );
			
			//AfxMessageBox("HVAPI_StartRecvMJPEG  Ok\n");
		}
	}
	else
	{
		char szDebugInfo[256];
		sprintf( szDebugInfo , "HVAPI_ExecCmdEx :%s fail" ,strBuf  );
		AfxMessageBox(szDebugInfo); 
	}
}



void CImageRecvTestDlg::OnBnClickedRadio2()
{
	CButton* pBut = (CButton*)GetDlgItem(IDC_RADIO2);
	CButton* pBut3 =(CButton*)GetDlgItem(IDC_RADIO3);
	CButton* pBut4 =(CButton*)GetDlgItem(IDC_RADIO4);

	if(pBut->GetCheck() == 1)
	{
		m_fIsShowLpr = TRUE;
		pBut3->EnableWindow(TRUE);
		pBut4->EnableWindow(TRUE);
		pBut3->SetCheck(1);	
	}
	else
	{
		m_fIsShowLpr = FALSE;
		pBut3->EnableWindow(FALSE);
		pBut4->EnableWindow(FALSE);
	}

	CString strBuf = "SetEncodeMode,EncodeMode[3]";
	char szRetBuf[128*1024] = {0};
	INT nRetLen = 0;

	if ( S_OK == HVAPI_ExecCmdEx(*m_hDevice, strBuf, szRetBuf, sizeof(szRetBuf), &nRetLen))
	{
		//HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_JPEG_FRAME,NULL);
		//HVAPI_SetCallBackEx(*m_hDevice, OnImage, this, 0, CALLBACK_TYPE_JPEG_FRAME,"SetImgType,EnableRecogVideo[1]");		
		HVAPI_StopRecvMJPEG(*m_hDevice);
		
		HVAPI_StartRecvMJPEG(*m_hDevice , OnImageEx , this , 0 , MJPEG_RECV_FLAG_DEBUG);
		AfxMessageBox("HVAPI_StartRecvMJPEG Debug Ok\n");
	}
	else
	{
		char szDebugInfo[256];
		sprintf( szDebugInfo , "HVAPI_ExecCmdEx :%s fail" ,strBuf  );
		AfxMessageBox(szDebugInfo); 
	}


}


void CImageRecvTestDlg::OnBnClickedRadio3()
{
	//CButton* pBut = (CButton*)GetDlgItem(IDC_RADIO2);
	//pBut->SetCheck(1);
	//m_nLprSet = 0;
	//SetIni();
}

void CImageRecvTestDlg::OnBnClickedRadio4()
{
	//CButton* pBut = (CButton*)GetDlgItem(IDC_RADIO2);
	//pBut->SetCheck(1);
	//m_nLprSet = 1;
	//SetIni();

}


void CImageRecvTestDlg::SetIni()
{

	GetModuleFileName(NULL, g_szIniFile, MAX_PATH);	//¨¨?¦Ì?¡ã¨¹¨¤¡§3¨¬D¨°??¦Ì?¨¨??¡¤??
	PathRemoveFileSpec(g_szIniFile);				//¨¨£¤¦Ì?3¨¬D¨°??
	_tcsncat(g_szIniFile, _T("\\HvDevice.ini"), MAX_PATH - 1);

	if(m_nLprSet == 0)
	{
		::WritePrivateProfileString("MainConfig", "DrawLprPlateFrame","0", g_szIniFile);
	}
	else
	{
		::WritePrivateProfileString("MainConfig", "DrawLprPlateFrame","1", g_szIniFile);
	}
	CString str = "";
	GetPrivateProfileString("MainConfig", "DrawLprPlateFrame", "", str.GetBuffer(MAX_PATH), MAX_PATH, g_szIniFile);

}


void CImageRecvTestDlg::OnOK()
{
    // ¡¤¨¤?1¡ã¡ä??3¦Ì¨ª?3?
}


BOOL CImageRecvTestDlg::PreTranslateMessage(MSG* pMsg) 
{

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		 return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
 } 



void CImageRecvTestDlg::OnBnClickedBtnCap()
{

	CString strBuf = "SoftTriggerCapture";
	char szRetBuf[128*1024] = {0};
	INT nRetLen = 0;

	if ( S_OK == HVAPI_ExecCmdEx(*m_hDevice, strBuf, szRetBuf, sizeof(szRetBuf), &nRetLen))
	{
		//AfxMessageBox("¨¨¨ª¡ä£¤¡¤¡é3¨¦1|¡ê?");
	    //HVAPI_SetCallBackEx(*m_hDevice, OnImage, this, 0, CALLBACK_TYPE_JPEG_FRAME, "SetImgType,EnableRecogVideo[1]");
	}
	else
	{
		char szDebugInfo[256];
		sprintf( szDebugInfo , "HVAPI_ExecCmdEx :%s fail" ,strBuf  );
		AfxMessageBox(szDebugInfo); 
	}
}


void CImageRecvTestDlg::OnClose()
{
	HVAPI_SetCallBackEx(*m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_JPEG_FRAME, NULL);

	KillTimer(1003);
	m_hDevice = NULL;

	CDialog::OnClose();
}

void CImageRecvTestDlg::OnCancel()
{

	DestroyWindow();
	//CDialog::OnCancel();
}

void CImageRecvTestDlg::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
//	CDialog::PostNcDestroy();
    delete this;
}



















