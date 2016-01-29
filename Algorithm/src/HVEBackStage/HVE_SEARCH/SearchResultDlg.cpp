// SearchResultDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HVE_SEARCH.h"
#include "SearchResultDlg.h"
#include ".\searchresultdlg.h"
#include <shlwapi.h>
#include <Winnetwk.h>

#pragma comment(lib, "Mpr.lib")
#pragma comment(lib, "shlwapi.lib")

#define WM_LOADEND WM_USER+1100

// CSearchResultDlg dialog

IMPLEMENT_DYNAMIC(CSearchResultDlg, CDialog)
CSearchResultDlg::CSearchResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchResultDlg::IDD, pParent)
{
	m_hFontComm = CreateFont(50, 26, 0, 0, FW_HEAVY, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");
	m_hFontMin = CreateFont(14, 6, 0, 0, FW_LIGHT, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "Microsoft Sans Serif");

	m_lpDDS = NULL;
	m_lpRPic = NULL;
	m_lpCPic = NULL;

 	m_pShowLoading = new CShowLoading(this);
 	m_pShowLoading->CreateOwnerEx();


}

CSearchResultDlg::~CSearchResultDlg()
{
	DeleteObject(m_hFontComm);
	DeleteObject(m_hFontMin);

	if(m_lpCPic)
	{
		m_lpCPic->Release();
		m_lpCPic = NULL;
	}
	if(m_lpRPic)
	{
		m_lpRPic->Release();
		m_lpRPic = NULL;
	}
	if(m_lpDDS)
	{
		m_lpDDS->Release();
		m_lpDDS = NULL;
	}

 	delete m_pShowLoading;

	if (m_hWnd)
	{
		DestroyWindow();
	}

}

void CSearchResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTRESULT, m_listResult);
}


BEGIN_MESSAGE_MAP(CSearchResultDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_SIZE()
	ON_WM_PAINT()
//	ON_NOTIFY(NM_DBLCLK, IDC_LISTRESULT, OnNMDblclkListresult)
	ON_MESSAGE(WM_LOADEND, OnLoadEnd)
//	ON_STN_DBLCLK(IDC_STATIC_C_PIC, OnStnDblclickStaticCPic)
//	ON_STN_DBLCLK(IDC_STATIC_R_PIC, OnStnDblclickStaticRPic)
	ON_NOTIFY(NM_CLICK, IDC_LISTRESULT, OnNMClickListresult)
//	ON_STN_CLICKED(IDC_STATIC_R_PIC, OnStnClickedStaticRPic)
ON_WM_LBUTTONDBLCLK()
//ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTRESULT, OnLvnItemchangedListresult)
ON_NOTIFY(LVN_KEYDOWN, IDC_LISTRESULT, OnLvnKeydownListresult)
//ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTRESULT, OnLvnItemchangedListresult)
END_MESSAGE_MAP()


// CSearchResultDlg message handlers

void CSearchResultDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CSearchResultDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();

	DestroyWindow();
}

BOOL CSearchResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	// get server username, password, by zhut 20121112
	CString strIniFilePath;
	char szAppPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szAppPath, MAX_PATH);
	PathRemoveFileSpec(szAppPath);
	strIniFilePath.Format("%s\\Config.ini", szAppPath);

	GetPrivateProfileString("DBInfo", "ServerIP", "",
		m_strServerIP.GetBuffer(MAX_PATH), MAX_PATH, strIniFilePath);
	GetPrivateProfileString("ServerInfo", "ServerUserName", "administrator",
		m_strServerUserName.GetBuffer(MAX_PATH), MAX_PATH, strIniFilePath);
	GetPrivateProfileString("ServerInfo", "ServerPassword", "123456", 
		m_strServerPassword.GetBuffer(MAX_PATH), MAX_PATH, strIniFilePath);

	if (FALSE == InitDx())
	{
		AfxMessageBox("初始化失败");
		return FALSE;
	}

	SetDefaultPicBuf();

	InitList();

	AutoSize();

	m_iSelItem = -1;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CSearchResultDlg::InitList(void)
{
	m_listResult.InsertColumn(0, "序号", LVCFMT_LEFT, 40);
	m_listResult.InsertColumn(1, "车牌号", LVCFMT_LEFT, 80);
	m_listResult.InsertColumn(2, "经过时刻", LVCFMT_LEFT, 120);
	m_listResult.InsertColumn(3, "断面", LVCFMT_LEFT, 80);
	m_listResult.InsertColumn(4, "车道", LVCFMT_LEFT, 40);
	m_listResult.InsertColumn(5, "方向", LVCFMT_LEFT, 40);
	m_listResult.InsertColumn(6, "车速", LVCFMT_LEFT, 40);
	m_listResult.InsertColumn(7, "车辆类型", LVCFMT_LEFT, 60);
	

	if (m_bRoadNameIsExist)
	{
		m_listResult.InsertColumn(8, "桩号", LVCFMT_LEFT, 80);
		m_listResult.InsertColumn(9, "big_image_1_path", LVCFMT_LEFT, 0);
		m_listResult.InsertColumn(10, "big_image_2_path", LVCFMT_LEFT, 0);
		m_iColCount = 11;
	} 
	else
	{
		m_listResult.InsertColumn(8, "big_image_1_path", LVCFMT_LEFT, 0);
		m_listResult.InsertColumn(9, "big_image_2_path", LVCFMT_LEFT, 0);
		m_iColCount = 10;
	}
	

	m_listResult.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	return 0;
}

int CSearchResultDlg::AutoSize(void)
{
	CRect rc;
	GetClientRect(rc);

	CRect rcPicR;
	CRect rcPicC;
	GetDlgItem(IDC_STATIC_R_PIC)->GetWindowRect(rcPicR);
	ScreenToClient(rcPicR);
	GetDlgItem(IDC_STATIC_C_PIC)->GetWindowRect(rcPicC);
	ScreenToClient(rcPicC);

	int iPicHight = (rc.Height()-4)/2;
	int iPicWidth = iPicHight*4/3;
	

	GetDlgItem(IDC_STATIC_R_PIC)->SetWindowPos(NULL, rc.Width()-iPicWidth, 0, iPicWidth, iPicHight, SWP_SHOWWINDOW);
	GetDlgItem(IDC_STATIC_C_PIC)->SetWindowPos(NULL, rc.Width()-iPicWidth, iPicHight+4, iPicWidth, iPicHight, SWP_SHOWWINDOW);

	m_listResult.SetWindowPos(NULL, 0, 0, rc.Width()-iPicWidth, rc.Height(), SWP_SHOWWINDOW);

	return 0;
}

void CSearchResultDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_listResult.m_hWnd)
	{

		AutoSize();
	}
	

	// TODO: Add your message handler code here
}




int CSearchResultDlg::SetDefaultPicBuf(void)
{
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	m_lpCPic->Lock(NULL, &ddsd, 0, NULL);
	BYTE* pBuf = (BYTE*)ddsd.lpSurface;
	memset(pBuf, 250, ddsd.dwHeight*ddsd.lPitch);
	m_lpCPic->Unlock(NULL);

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	m_lpRPic->Lock(NULL, &ddsd, 0, NULL);
	pBuf = (BYTE*)ddsd.lpSurface;
	memset(pBuf, 250, ddsd.dwHeight*ddsd.lPitch);
	m_lpRPic->Unlock(NULL);

	HDC hDC;
	m_lpRPic->GetDC(&hDC);
	SelectObject(hDC, m_hFontComm);
	SetBkMode(hDC, 3);
	SetTextColor(hDC, RGB(128, 128, 128));
	TextOut(hDC, 80, 80, "识别图", 6);
	m_lpRPic->ReleaseDC(hDC);

	m_lpCPic->GetDC(&hDC);
	SelectObject(hDC, m_hFontComm);
	SetBkMode(hDC, 3);
	SetTextColor(hDC, RGB(128, 128, 128));
	TextOut(hDC, 80, 80, "抓拍图", 6);
	m_lpCPic->ReleaseDC(hDC);

	return 0;
}

void CSearchResultDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	if(m_lpRPic)
	{
		RECT cRect;
		::GetClientRect(GetDlgItem(IDC_STATIC_R_PIC)->GetSafeHwnd(), &cRect);
		HDC hSrcDC, hWinDC;
		m_lpRPic->GetDC(&hSrcDC);
		hWinDC = ::GetDC(GetDlgItem(IDC_STATIC_R_PIC)->GetSafeHwnd());
		SetStretchBltMode(hWinDC, COLORONCOLOR);
		StretchBlt(hWinDC, 0, 0, cRect.right-cRect.left, cRect.bottom-cRect.top,
			hSrcDC, 0, 0, 315, 220, SRCCOPY);
		::ReleaseDC(GetDlgItem(IDC_STATIC_R_PIC)->GetSafeHwnd(), hWinDC);
		m_lpRPic->ReleaseDC(hSrcDC);
	}

	if(m_lpCPic)
	{
		RECT cRect;
		::GetClientRect(GetDlgItem(IDC_STATIC_C_PIC)->GetSafeHwnd(), &cRect);
		HDC hSrcDC, hWinDC;
		m_lpCPic->GetDC(&hSrcDC);
		hWinDC = ::GetDC(GetDlgItem(IDC_STATIC_C_PIC)->GetSafeHwnd());
		SetStretchBltMode(hWinDC, COLORONCOLOR);
		StretchBlt(hWinDC, 0, 0, cRect.right-cRect.left, cRect.bottom-cRect.top,
			hSrcDC, 0, 0, 315, 220, SRCCOPY);
		::ReleaseDC(GetDlgItem(IDC_STATIC_C_PIC)->GetSafeHwnd(), hWinDC);
		m_lpCPic->ReleaseDC(hSrcDC);
	}

}

BOOL CSearchResultDlg::InitDx(void)
{
	if(FAILED(DirectDrawCreateEx(NULL, (void**)&m_lpDDS, IID_IDirectDraw7, NULL)))
	{
		return FALSE;
	}
	if(FAILED(m_lpDDS->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL)))
	{
		return FALSE;
	}

	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.dwWidth = 315;
	ddsd.dwHeight = 220;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	
	if(FAILED(m_lpDDS->CreateSurface(&ddsd, &m_lpRPic, NULL)))
	{
		return FALSE;
	}
	if(FAILED(m_lpDDS->CreateSurface(&ddsd, &m_lpCPic, NULL)))
	{
		return FALSE;
	}

	return TRUE;
}

int CSearchResultDlg::UpdatePicBuf(void)
{
	if (-1 == DrawNomalPicture(m_lpRPic, m_strRPicPath))
	{
		DrawErrorPicture(m_lpRPic, m_strRPicPath, "识别图：");
	}

	if (-1 == DrawNomalPicture(m_lpCPic, m_strCPicPath))
	{
		DrawErrorPicture(m_lpCPic, m_strCPicPath, "抓拍图：");
	}

	SendMessage(WM_PAINT, 0, 0);

	return 0;

}

void CSearchResultDlg::DrawRectangle(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color1, COLORREF color2)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, color1);
	HBRUSH hBrush = CreateSolidBrush(color2);
	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);
	Rectangle(hDC, x1, y1, x2, y2);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

void CSearchResultDlg::DrawTextThis(HDC hDC, int x, int y, LPCSTR pszText, int iTextLen, COLORREF color)
{
	SetBkMode(hDC, 3);
	SetTextColor(hDC, color);
	TextOut(hDC, x, y, pszText, iTextLen);
}

void CSearchResultDlg::LoadResultPic(int iIndex)
{
	EnableWindow(FALSE);

	//delete previous  file
	if (PathFileExists(m_strRPicPath))
	{
		DeleteFile(m_strRPicPath);
		m_strRPicPath.Empty();
	}
	if (PathFileExists(m_strCPicPath))
	{
		DeleteFile(m_strCPicPath);
		m_strCPicPath.Empty();
	}

	char szCurrentPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szCurrentPath, MAX_PATH);
	PathRemoveFileSpec(szCurrentPath);
	CString strSavePath = szCurrentPath;
	strSavePath += "\\TmpResultTmp\\";
	if(!PathFileExists(strSavePath.GetBuffer()))
	{
		if(CreateDirectory(strSavePath.GetBuffer(), NULL) == TRUE)
		{
			SetFileAttributes(strSavePath.GetBuffer(), FILE_ATTRIBUTE_HIDDEN);
		}
	}

	NETRESOURCE cNetRes;
	memset(&cNetRes, 0, sizeof(cNetRes));
	CString strServerIP;
	strServerIP.Format("\\\\%s", m_strServerIP);
	cNetRes.lpRemoteName = strServerIP.GetBuffer(30);		
	DWORD dwRet = WNetAddConnection2(&cNetRes,  m_strServerPassword, m_strServerUserName,0); 
	strServerIP.ReleaseBuffer();

	char szFileName[MAX_PATH] = {0};
	m_listResult.GetItemText(iIndex, m_iColCount-2, szFileName, MAX_PATH);

	CString strUrl;
	strUrl.Format("\\\\%s\\", m_strServerIP);
	strUrl += szFileName[0];
	strUrl += szFileName+2;
	CString strFileName = szFileName;
	int iIndexChar = strFileName.ReverseFind('\\');
	strFileName = strFileName.Mid(iIndexChar+1, strFileName.GetLength()-iIndexChar);
	CString strSaveName = strSavePath;
	strSaveName += strFileName;

	HRESULT hr = URLDownloadToFile(NULL, _T(strUrl.GetBuffer()),
		_T(strSaveName.GetBuffer()), 0, NULL);

	if(SUCCEEDED(hr))
	{
		m_strRPicPath = strSaveName;
	}
	else
	{
		m_strRPicPath = "";
	}


	m_listResult.GetItemText(iIndex, m_iColCount-1, szFileName, MAX_PATH);

	strUrl.Format("\\\\%s\\", m_strServerIP);
	strUrl += szFileName[0];
	strUrl += szFileName + 2;
	strFileName = szFileName;
	iIndexChar = strFileName.ReverseFind('\\');
	strFileName = strFileName.Mid(iIndexChar+1, strFileName.GetLength()-iIndexChar);
	strSaveName = strSavePath;
	strSaveName += strFileName;
	hr = URLDownloadToFile(NULL, _T(strUrl.GetBuffer()),
		_T(strSaveName.GetBuffer()), 0, NULL);
	if(SUCCEEDED(hr))
	{
		m_strCPicPath = strSaveName;
	}
	else
	{
		m_strCPicPath = "";
	}

	EnableWindow(TRUE);

}

UINT CSearchResultDlg::LoadPicThread(LPVOID pParam)
{
	CSearchResultDlg* pDlg = (CSearchResultDlg*) pParam;

	pDlg->LoadResultPic(pDlg->m_iSelItem);

	::PostMessage(pDlg->m_hWnd,WM_LOADEND, 0, 0);

	return 0;
}

LRESULT CSearchResultDlg::OnLoadEnd(WPARAM wParam, LPARAM lParam)
{

 	m_pShowLoading->Stop();

	UpdatePicBuf();

	return 0;
}


void CSearchResultDlg::OnNMClickListresult(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	NMLISTVIEW* pList = (NMLISTVIEW*)pNMHDR;

	if (pList->iItem == -1)
	{
		return;
	}

	ShowPicture(pList->iItem);

}

void CSearchResultDlg::OnLvnKeydownListresult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	NMLISTVIEW* pList = (NMLISTVIEW*)pNMHDR;

	if (pLVKeyDow->wVKey == VK_DOWN)
	{
		GetNextItem();
	}
	else if (pLVKeyDow->wVKey == VK_UP)
	{
		GetPreItem();
	}

}

void CSearchResultDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CRect rcRpic;
	CRect rcCpic;

	GetDlgItem(IDC_STATIC_R_PIC)->GetWindowRect(rcRpic);
	ScreenToClient(rcRpic);
	GetDlgItem(IDC_STATIC_C_PIC)->GetWindowRect(rcCpic);
	ScreenToClient(rcCpic);

	if ((point.x > rcRpic.left) && 
		(point.x < rcRpic.right) &&
		(point.y > rcRpic.top) &&
		(point.y < rcRpic.bottom)
		)
	{
		if (PathFileExists(m_strRPicPath))
		{
			ShellExecute(m_hWnd, "open", m_strRPicPath, NULL, NULL, SW_SHOWMAXIMIZED);
		}
	}

	if ((point.x > rcCpic.left) && 
		(point.x < rcCpic.right) &&
		(point.y > rcCpic.top) &&
		(point.y < rcCpic.bottom)
		)
	{
		if (PathFileExists(m_strCPicPath))
		{
			ShellExecute(m_hWnd, "open", m_strCPicPath, NULL, NULL, SW_SHOWMAXIMIZED);
		}
	}


	CDialog::OnLButtonDblClk(nFlags, point);
}



int CSearchResultDlg::GetPreItem(void)
{

	int iTotol = m_listResult.GetItemCount();

	if(iTotol <= 0) 
		return -1;

// 	m_listResult.EnableWindow(FALSE);
	int iSelectIndex = m_listResult.GetSelectionMark();
	if(iSelectIndex < 0 || iSelectIndex >= iTotol)
	{
		iSelectIndex = iTotol;
		m_listResult.SetSelectionMark(iSelectIndex);
	}
	else
	{
		iSelectIndex--;
		if(iSelectIndex < 0)
		{
			m_listResult.EnableWindow(TRUE);
			m_listResult.SetFocus();
			AfxMessageBox("已到达第一条记录");
			return -1;
		}
 		m_listResult.SetSelectionMark(iSelectIndex);
	}

// 	m_listResult.EnsureVisible(iSelectIndex, TRUE);
	
	ShowPicture(iSelectIndex);

// 	m_listResult.EnableWindow(TRUE);
// 	m_listResult.SetFocus();

	return 0;
}

int CSearchResultDlg::GetNextItem(void)
{
	int iTotol = m_listResult.GetItemCount();

	if(iTotol <= 0) 
		return -1;

// 	m_listResult.EnableWindow(FALSE);
	int iSelectIndex = m_listResult.GetSelectionMark();
	if(iSelectIndex < 0 || iSelectIndex >= iTotol)
	{
		iSelectIndex = 0;
		m_listResult.SetSelectionMark(iSelectIndex);
	}
	else
	{
		iSelectIndex++;
		if(iSelectIndex >= iTotol)
		{
			m_listResult.EnableWindow(TRUE);
			m_listResult.SetFocus();
			AfxMessageBox("已到达最后一条记录");
			return -1;
		}
		m_listResult.SetSelectionMark(iSelectIndex);
	}

// 	m_listResult.EnsureVisible(iSelectIndex, TRUE);
	
	ShowPicture(iSelectIndex);

// 	m_listResult.EnableWindow(TRUE);
// 	m_listResult.SetFocus();

	return 0;
}

int CSearchResultDlg::ShowPicture(int iItem)
{
	m_iSelItem = iItem;

 	m_pShowLoading->Start();
 	m_pShowLoading->SetInfoWord("正在下载图片，请稍后");

	AfxBeginThread(LoadPicThread, this);

	return 0;
}

int CSearchResultDlg::DrawNomalPicture(LPDIRECTDRAWSURFACE7 lpPic, CString strFilePath)
{
	if(!PathFileExists(strFilePath))
	{
		return -1;
	}

	IPicture* pPic;
	IStream* pStm;
	HANDLE hFile = NULL;
	DWORD dwFileSize, dwByteRead;

	hFile = CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	dwFileSize = GetFileSize(hFile, NULL);
	if(dwFileSize == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
		return -1;
	}

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
	LPVOID pvData = NULL;
	if(hGlobal == NULL)
	{
		CloseHandle(hFile);
		return -1;
	}
	if((pvData = GlobalLock(hGlobal)) == NULL)
	{
		CloseHandle(hFile);
		GlobalFree(hGlobal);
		return -1;
	}
	ReadFile(hFile, pvData, dwFileSize, &dwByteRead, NULL);
	GlobalUnlock(hGlobal);
	CloseHandle(hFile);

	CDC* pThisDC = GetDC();

	CreateStreamOnHGlobal(hGlobal, TRUE, &pStm);
	if(pStm == NULL)
	{
		GlobalFree(hGlobal);
		return -1;
	}
	HRESULT hResult = OleLoadPicture(pStm, dwFileSize, TRUE, IID_IPicture, (LPVOID*)&pPic);
	if(FAILED(hResult))
	{
		pStm->Release();
		GlobalFree(hGlobal);
		return -1;
	}

	OLE_XSIZE_HIMETRIC hmWidth;
	OLE_YSIZE_HIMETRIC hmHeight;
	pPic->get_Width(&hmWidth);
	pPic->get_Height(&hmHeight);

	int iWidth = MulDiv(hmWidth, pThisDC->GetDeviceCaps(LOGPIXELSX), 2540);
	int iHeight = MulDiv(hmHeight, pThisDC->GetDeviceCaps(LOGPIXELSY), 2540);

	HDC hTmpDC1;
	lpPic->GetDC(&hTmpDC1);
	pPic->Render(hTmpDC1, 0, 0, 315, 220, 0, hmHeight, hmWidth, -hmHeight, NULL);
	lpPic->ReleaseDC(hTmpDC1);
	pPic->Release();
	pStm->Release();
	GlobalFree(hGlobal);

	return 0;
}

int CSearchResultDlg::DrawErrorPicture(LPDIRECTDRAWSURFACE7 lpPic, CString strFilePath, CString strName)
{
	HDC hTmpDC1;
	lpPic->GetDC(&hTmpDC1);
	SelectObject(hTmpDC1, m_hFontMin);
	DrawRectangle(hTmpDC1, 0, 0, 315, 220, RGB(250, 250, 250), RGB(250, 250, 250));

	CString strFileName = strFilePath;
	int iIndexChar = strFileName.ReverseFind('\\');
	strFileName = strFileName.Mid(iIndexChar+1, strFileName.GetLength()-iIndexChar);
	DrawTextThis(hTmpDC1, 10, 10, strName, 7, RGB(32, 32, 32));
	iIndexChar = strFileName.GetLength();

	if(iIndexChar > 50)
	{
		iIndexChar = 50;
	}
	DrawTextThis(hTmpDC1, 10, 30, strFileName.GetBuffer(), iIndexChar, RGB(255, 0, 0));
	DrawTextThis(hTmpDC1, 10, 50, "获取失败!", 9, RGB(32, 32, 32));
	lpPic->ReleaseDC(hTmpDC1);

	return 0;
}
