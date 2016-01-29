#include "stdafx.h"
#include "PicView.h"
#include ".\picview.h"

IMPLEMENT_DYNAMIC(CPicView, CDialog)

CPicView::CPicView(LPDIRECTDRAWSURFACE7 lpSrcPic, CWnd* pParent /* = NULL */)
: CDialog(CPicView::IDD, pParent)
{
	m_hThreadMain = NULL;
	m_fExit = TRUE;
	m_lpSrcPic = lpSrcPic;
	m_lpdds = NULL;
	m_lpOffScreen = NULL;
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	lpSrcPic->Lock(NULL, &ddsd, 0, NULL);
	m_iWidth = (int)ddsd.dwWidth;
	m_iHeight = (int)ddsd.dwHeight;
	lpSrcPic->Unlock(NULL);
}

CPicView::~CPicView()
{
	SafeStopThread();
	if(m_lpOffScreen)
	{
		m_lpOffScreen->Release();
		m_lpOffScreen = NULL;
	}
	if(m_lpdds)
	{
		m_lpdds->Release();
		m_lpdds = NULL;
	}
}

void CPicView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPicView, CDialog)
	ON_WM_PAINT()
	ON_WM_RBUTTONDBLCLK()
END_MESSAGE_MAP()

void CPicView::InitDX()
{
	if(FAILED(DirectDrawCreateEx(NULL, (void**)&m_lpdds, IID_IDirectDraw7, NULL)))
	{
		return;
	}
	if(FAILED(m_lpdds->SetCooperativeLevel(GetSafeHwnd(), DDSCL_NORMAL)))
	{
		return;
	}
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.dwWidth = m_iWidth;
	ddsd.dwHeight = m_iHeight;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	if(FAILED(m_lpdds->CreateSurface(&ddsd, &m_lpOffScreen, NULL)))
	{
		return;
	}
}

BOOL CPicView::OnInitDialog()
{
	CDialog::OnInitDialog();
	int iWidth = 0;
	int iHeight = 0;
	if(m_iWidth > GetSystemMetrics(SM_CXSCREEN)) iWidth = GetSystemMetrics(SM_CXSCREEN);
	else iWidth = m_iWidth;
	if(m_iHeight > GetSystemMetrics(SM_CYSCREEN)) iHeight = GetSystemMetrics(SM_CYSCREEN);
	else iHeight = m_iHeight;
	m_iRealWidth = iWidth;
	m_iRealHeight = iHeight;
	RECT cRect;
	cRect.left = (GetSystemMetrics(SM_CXSCREEN)-iWidth) >> 1;
	cRect.right = cRect.left + iWidth;
	cRect.top = (GetSystemMetrics(SM_CYSCREEN)-iHeight) >> 1;
	cRect.bottom = cRect.top + iHeight;
	MoveWindow(&cRect, TRUE);
	InitDX();
	return TRUE;
}

void CPicView::SafeStopThread()
{
	if(m_hThreadMain)
	{
		m_fExit = TRUE;
		int iWaitTimes = 0;
		int MAXWAITTIME = 8;
		while(WaitForSingleObject(m_hThreadMain, 500) == WAIT_TIMEOUT && iWaitTimes < MAXWAITTIME)
		{
			iWaitTimes++;
		}

		if(iWaitTimes >= MAXWAITTIME)
		{
			TerminateThread(m_hThreadMain, 0);
		}
		else
		{
			CloseHandle(m_hThreadMain);
		}
	}
	m_hThreadMain = NULL;
}

void CPicView::PrepareShow()
{
	if(m_lpSrcPic && m_lpOffScreen)
	{
		HDC hDCSrc, hDCOff;
		m_lpOffScreen->GetDC(&hDCOff);
		m_lpSrcPic->GetDC(&hDCSrc);
		BitBlt(hDCOff, 0, 0, m_iWidth, m_iHeight, hDCSrc, 0, 0, SRCCOPY);
		m_lpSrcPic->ReleaseDC(hDCSrc);
		m_lpOffScreen->ReleaseDC(hDCOff);
	}
}

void CPicView::OnPaint()
{
	CDialog::OnPaint();
	if(m_lpOffScreen)
	{
		PrepareShow();
		RECT cRect;
		::GetClientRect(GetSafeHwnd(), &cRect);
		HDC hSrcDC, hWinDC;
		m_lpOffScreen->GetDC(&hSrcDC);
		hWinDC = ::GetDC(GetSafeHwnd());
		SetStretchBltMode(hWinDC, COLORONCOLOR);
		StretchBlt(hWinDC, 0, 0, cRect.right-cRect.left, cRect.bottom-cRect.top,
			hSrcDC, 0, 0,m_iWidth, m_iHeight, SRCCOPY);
		::ReleaseDC(GetSafeHwnd(), hWinDC);
		m_lpOffScreen->ReleaseDC(hSrcDC);
	}
}

void CPicView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	CDialog::OnCancel();
}
