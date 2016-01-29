#include "stdafx.h"
#include "OldImageRecvTestDlg.h"

IMPLEMENT_DYNAMIC(COldImageRecvTestDlg, CDialog)

extern CListCtrl* g_ListEx;
extern DWORD g_dwCurrentConnectIndexEx;

COldImageRecvTestDlg::COldImageRecvTestDlg(HVAPI_HANDLE* phHandle, CWnd* pParent /* = NULL */)
: CDialog(COldImageRecvTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDevice = phHandle;
	m_dwLastStatus = CONN_STATUS_UNKNOWN;
	m_dwReconnectTimes = 0;
}

COldImageRecvTestDlg::~COldImageRecvTestDlg()
{

}

void COldImageRecvTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COldImageRecvTestDlg, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL COldImageRecvTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	MoveWindow((GetSystemMetrics(SM_CXSCREEN)-1300)>>1,
		(GetSystemMetrics(SM_CYSCREEN)-900)>>1, 1300, 900, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_JPEG)->GetSafeHwnd(), 5, 5, 640, 400, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_LPR)->GetSafeHwnd(), 650, 5, 640, 400, TRUE);
	::MoveWindow(GetDlgItem(IDC_STATIC_CAP)->GetSafeHwnd(), 5, 410, 640, 400, TRUE);
	SetTimer(1003, 1000, NULL);
	HVAPI_SetCallBack(*m_hDevice, OnImage, this, STREAM_TYPE_IMAGE, "SetImgType,EnableRecogVideo[1]");
	return TRUE;
}

void COldImageRecvTestDlg::OnCancel()
{
	HVAPI_SetCallBack(*m_hDevice, NULL, NULL, STREAM_TYPE_IMAGE, NULL);
	CDialog::OnCancel();
}

int COldImageRecvTestDlg::OnImage(PVOID pUserData,
								  PBYTE pbImageData,
								  DWORD dwImageDataLen,
								  PBYTE pbImageInfoData,
								  DWORD dwImageInfoLen,
								  DWORD dwImageType,
								  LPCSTR szImageExtInfo)
{
	static DWORD dwStartTine = GetTickCount();
	static DWORD dwFrameCount = 0;
	if(pUserData == NULL)
	{
		return -1;
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

	COldImageRecvTestDlg* pDlg = (COldImageRecvTestDlg*)pUserData;
	if(dwImageType == IMAGE_TYPE_JPEG_NORMAL)
		dwFrameCount++;
	if(dwFrameCount >= 50)
	{
		float fFPS = (dwFrameCount / (float)((GetTickCount() - dwStartTine) / 1000.));
		char szTmp[100];
		sprintf(szTmp, "FPS : %f", fFPS);
		pDlg->SetWindowText(szTmp);
		dwStartTine = GetTickCount();
		dwFrameCount = 0;
	}
	HDC TempDC;
	switch(dwImageType)
	{
	case IMAGE_TYPE_JPEG_NORMAL:
		TempDC = ::GetDC(pDlg->GetDlgItem(IDC_STATIC_JPEG)->GetSafeHwnd());
		break;
	case IMAGE_TYPE_JPEG_CAPTURE:
		TempDC = ::GetDC(pDlg->GetDlgItem(IDC_STATIC_CAP)->GetSafeHwnd());
		break;
	case IMAGE_TYPE_JPEG_LPR:
		TempDC = ::GetDC(pDlg->GetDlgItem(IDC_STATIC_LPR)->GetSafeHwnd());
		break;
	}
	OLE_XSIZE_HIMETRIC hmWidth;
	OLE_YSIZE_HIMETRIC hmHeight;
	picholder->get_Width(&hmWidth);
	picholder->get_Height(&hmHeight);
	int nWidth =MulDiv(hmWidth, GetDeviceCaps(TempDC, LOGPIXELSX), 2540);
	int nHeight = MulDiv(hmHeight, GetDeviceCaps(TempDC, LOGPIXELSY), 2540);
	picholder->Render(TempDC, 0, 0, 640, 400, 0, hmHeight, hmWidth, -hmHeight, NULL);
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

void COldImageRecvTestDlg::OnTimer(UINT nIDEvent)
{
	if(*m_hDevice && nIDEvent == 1003)
	{
		DWORD dwConnStatus = CONN_STATUS_UNKNOWN;
		if(HVAPI_GetConnStatus(*m_hDevice, STREAM_TYPE_IMAGE, &dwConnStatus)
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
		g_ListEx->SetItemText(g_dwCurrentConnectIndexEx, 3, szTmpInfo);
	}

	CDialog::OnTimer(nIDEvent);
}
