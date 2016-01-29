#include "stdafx.h"
#include "GetInternetMapDlg.h"
#include ".\getinternetmapdlg.h"
#include "VehicleTrackingSystemDlg.h"

HRESULT WirteToJpgFile(LPCSTR szFileName, unsigned char* pbData, int iWidth, int iHeight, int iQuality);

IMPLEMENT_DYNAMIC(CGetInternetMapDlg, CDialog)

CGetInternetMapDlg::CGetInternetMapDlg(CWnd* pParent)
:CDialog(CGetInternetMapDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_MapDlg = NULL;
	m_pParent = pParent;
}

CGetInternetMapDlg::~CGetInternetMapDlg()
{
	if(m_MapDlg != NULL)
	{
		delete m_MapDlg;
		m_MapDlg = NULL;
	}
}

void CGetInternetMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGetInternetMapDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_WM_TIMER()
	ON_WM_NCLBUTTONDBLCLK()
END_MESSAGE_MAP()

BOOL CGetInternetMapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	//ShowWindow(SW_SHOWMAXIMIZED);

	RECT cRectInit;
	cRectInit.left = 0;
	cRectInit.top = 0;
	cRectInit.right = GetSystemMetrics(SM_CXSCREEN);
	cRectInit.bottom = GetSystemMetrics(SM_CYSCREEN);
	MoveWindow(&cRectInit, TRUE);
	cRectInit.left = 218;
	cRectInit.top = 50;
	cRectInit.right = 1242;
	cRectInit.bottom = 836;
	GetDlgItem(IDC_STATIC)->MoveWindow(&cRectInit, TRUE);
	cRectInit.left = 218;
	cRectInit.right = 278;
	cRectInit.top = 10;
	cRectInit.bottom = 35;
	GetDlgItem(IDC_STATIC111)->MoveWindow(&cRectInit, TRUE);
	
	cRectInit.left = 288;
	cRectInit.right = 450;
	GetDlgItem(IDC_EDIT1)->MoveWindow(&cRectInit, TRUE);
	SetDlgItemText(IDC_EDIT1, "杭州");

	cRectInit.left = 460;
	cRectInit.right = 560;
	GetDlgItem(IDC_BUTTON1)->MoveWindow(&cRectInit, TRUE);

	cRectInit.left = 570;
	cRectInit.right = 670;
	GetDlgItem(IDC_BUTTON2)->MoveWindow(&cRectInit, TRUE);

	RECT cRect;
	m_MapDlg = new CMapFrameDlg;
	m_MapDlg->Create(IDD_HTML_DLG);
	m_MapDlg->SetParent(GetDlgItem(IDC_STATIC));
	GetDlgItem(IDC_STATIC)->GetWindowRect(&cRect);
	m_MapDlg->MoveWindow(0, 0, cRect.right-cRect.left, cRect.bottom-cRect.top);
	m_MapDlg->ShowWindow(SW_SHOW);

	GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	m_iWaitInitTimes = 0;
	SetTimer(1300, 1000, NULL);

	return TRUE;
}

BOOL CGetInternetMapDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
			return TRUE;
		case VK_RETURN:
			{
				char szLoca[MAX_PATH] = {0};
				GetDlgItemText(IDC_EDIT1, szLoca, MAX_PATH);
				if(strlen(szLoca) <= 0)
				{
					MessageBox("搜索地名不能为空");
					return TRUE;
				}
				m_MapDlg->ReSetLoca(szLoca);
			}
			return TRUE;
		}
	}
	else if(pMsg->message == WM_SYSCOMMAND)
	{
		if(pMsg->wParam == SC_RESTORE) return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CGetInternetMapDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
	//if(nID == SC_MAXIMIZE || nID == SC_RESTORE)
	//{
	//	RECT cRect;
	//	GetWindowRect(&cRect);
	//	ClientToScreen((LPPOINT)&cRect);
	//	ClientToScreen((LPPOINT)&cRect+1);
	//	RECT cRectNew;
	//	cRectNew.left = 5;
	//	cRectNew.right = cRect.right-cRect.left+5 - 18;
	//	cRectNew.top = 50;
	//	cRectNew.bottom = cRect.bottom-cRect.top+50 - 98;
	//	GetDlgItem(IDC_STATIC)->MoveWindow(&cRectNew, TRUE);

	//	GetDlgItem(IDC_STATIC)->GetWindowRect(&cRect);
	//	m_MapDlg->MoveWindow(0, 0, cRect.right-cRect.left, cRect.bottom-cRect.top);
	//	m_MapDlg->UpdateWindow();

	//	m_MapDlg->ResizeContanner(cRect.right-cRect.left-20, cRect.bottom-cRect.top-20);
	//}
}

void CGetInternetMapDlg::OnSize(UINT nType, int cx, int cy)
{
	if(nType == SIZE_MAXIMIZED)
	{

	}
	else if(nType == SIZE_RESTORED)
	{

	}
	CDialog::OnSize(nType, cx, cy);
}

void CGetInternetMapDlg::OnBnClickedButton1()
{
	char szLoca[MAX_PATH] = {0};
	GetDlgItemText(IDC_EDIT1, szLoca, MAX_PATH);
	if(strlen(szLoca) <= 0)
	{
		MessageBox("搜索地名不能为空");
		return;
	}
	m_MapDlg->ReSetLoca(szLoca);
}

void CGetInternetMapDlg::OnBnClickedButton2()
{
	CString strClearCover = "map.removeControl(NavigationControl); map.clearOverlays();";
	m_MapDlg->ExecScript(strClearCover.GetBuffer());
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	m_dwDelay = 0;
	SetWindowText("正在获取地图数据，请稍侯...");
	SetTimer(1003, 500, NULL);
}

void CGetInternetMapDlg::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == 1003)
	{
		if(m_dwDelay >= 3)
		{
			KillTimer(1003);
			HWND hWndSrc;
			hWndSrc = m_MapDlg->GetSafeHwnd();
			RECT cRectSrc;
			::GetClientRect(hWndSrc, &cRectSrc);
			HDC hDcSrc, hDcDec;
			hDcSrc = ::GetDC(hWndSrc);
			((CVehicleTrackingSystemDlg*)m_pParent)->m_lpBack->GetDC(&hDcDec);
			SetStretchBltMode(hDcDec, COLORONCOLOR);
			StretchBlt(hDcDec, 0, 0, 1024, 786, hDcSrc, 0, 0, 
				cRectSrc.right-cRectSrc.left, cRectSrc.bottom-cRectSrc.top, SRCCOPY);
			((CVehicleTrackingSystemDlg*)m_pParent)->m_lpBack->ReleaseDC(hDcDec);
			::ReleaseDC(hWndSrc, hDcSrc);
			((CVehicleTrackingSystemDlg*)m_pParent)->m_fIsUpdateInternetMap = TRUE;
			if(MessageBox("保存地图数据？", "INFO", MB_YESNO) == IDYES)
			{
				CFileDialog cGetSavePath(FALSE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "JPG文件(*.jpg)|*.jpg||");
				if(cGetSavePath.DoModal() == IDOK)
				{
					CString strSaveFilePath;
					strSaveFilePath = cGetSavePath.GetPathName();
					if(strSaveFilePath.GetLength() <= 3)
					{
						MessageBox("文件保存路径错误，保存失败!", "ERROR", MB_OK|MB_ICONERROR);
					}
					else
					{
						if(strSaveFilePath[strSaveFilePath.GetLength()-1] != 'g'
							&& strSaveFilePath[strSaveFilePath.GetLength()-1] != 'G')
						{
							strSaveFilePath += ".jpg";
						}
						DDSURFACEDESC2 ddsd;
						ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
						ddsd.dwSize = sizeof(DDSURFACEDESC2);
						((CVehicleTrackingSystemDlg*)m_pParent)->m_lpBack->Lock(NULL, &ddsd, 0, NULL);

						BYTE* pImage = (BYTE*)ddsd.lpSurface;
						PBYTE pYuv = new BYTE[1024 * 786 * 3];
						PBYTE tmpImage = pYuv;
						INT indexX, indexY;

				if(ddsd.ddpfPixelFormat.dwRGBBitCount == 32)
				{
					for(indexY=0; indexY<786; indexY++)
					{
						for(indexX=0; indexX<1024; indexX++)
						{
							*tmpImage++ = *(pImage + 2);
							*tmpImage++ = *(pImage + 1);
							*tmpImage++ = *(pImage);
							*tmpImage++;
							pImage += 4;
						}
					}
				}
				else if(ddsd.ddpfPixelFormat.dwRGBBitCount == 16)
				{
					for(indexY=0; indexY<786; indexY++)
					{
						for(indexX=0; indexX<1024; indexX++)
						{
							BYTE b = *(pImage + 2);
							BYTE g = *(pImage + 1);
							BYTE r = *pImage;
							r >>= 3;
							g >>= 2;
							b >>= 3;
							short dwColor = (short)(((short)r<<11) + ((short)g<<5) + (short)b);
							memcpy(tmpImage, &dwColor, 2);
							tmpImage += 2;
							pImage += 2;
						}
					}
				}
				else
				{
					MessageBox("不是32位或16位色模式，无法载入地图");
				}
						((CVehicleTrackingSystemDlg*)m_pParent)->m_lpBack->Unlock(NULL);
						if(WirteToJpgFile(strSaveFilePath.GetBuffer(), pYuv, 1024, 786, 80) != S_OK)
						{
							MessageBox("JPEG编码错误，保存失败!", "ERROR", MB_OK|MB_ICONERROR);
						}
						SAFE_DELETE_ARRAY(pYuv);
					}
				}
			}
			OnCancel();
		}
		SetWindowText("正在获取地图数据，请稍侯...");
		m_dwDelay++;
	}
	else if(nIDEvent == 1300)
	{
		if(m_MapDlg->IsInited())
		{
			KillTimer(1300);
			m_iWaitInitTimes = 0;
			GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
		}
		else
		{
			m_iWaitInitTimes++;
			if(m_iWaitInitTimes >= 30)
			{
				KillTimer(1300);
				MessageBox("地图环境初始化失败，无法启动网络地图功能!");
				OnCancel();
				return;
			}
		}
	}
	CDialog::OnTimer(nIDEvent);
}



void CGetInternetMapDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	return;
	CDialog::OnNcLButtonDblClk(nHitTest, point);
}
