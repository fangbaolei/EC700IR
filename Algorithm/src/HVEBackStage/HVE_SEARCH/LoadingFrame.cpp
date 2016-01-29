#include "stdafx.h"
#include "LoadingFrame.h"

const int Loca1[] = 
{
	75,38, 69,39, 64,43, 63,49,
	64,55, 68,60, 74,61, 80,60,
	85,56, 86,50, 85,44, 81,39
};

const int Loca2[] = 
{
	75,30,65,32,57,39,55,49,
	57,59,64,67,74,69,84,67,
	92,60,94,50,92,40,85,32
};

HFONT g_hFontInfo;

IMPLEMENT_DYNAMIC(CShowLoading, CDialog)

CShowLoading::CShowLoading(CWnd* pParent)
: CDialog(CShowLoading::IDD, NULL)
{
	m_pParent = pParent;
	m_hThreadMain = NULL;
	m_strInfo = "";
	InitializeCriticalSection(&m_csString);
	g_hFontInfo = CreateFont(20, 7, 0, 0, FW_HEAVY, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "Microsoft Sans Serif");
}

CShowLoading::~CShowLoading()
{
	SafeStopThread();
	DeleteCriticalSection(&m_csString);
	if(m_lpParent)
	{
		m_lpParent->Release();
		m_lpParent = NULL;
	}
	if(m_lpBack)
	{
		m_lpBack->Release();
		m_lpBack = NULL;
	}
	if(m_lpDDS)
	{
		m_lpDDS->Release();
		m_lpDDS = NULL;
	}

	if (m_hWnd)
	{
		DestroyWindow();

	}
}

void CShowLoading::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CShowLoading, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CShowLoading::InitDX()
{
	if(FAILED(DirectDrawCreateEx(NULL, (void**)&m_lpDDS, IID_IDirectDraw7, NULL)))
	{
		return;
	}
	if(FAILED(m_lpDDS->SetCooperativeLevel(GetSafeHwnd(), DDSCL_NORMAL)))
	{
		return;
	}
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.dwWidth = 150;
	ddsd.dwHeight = 150;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	if(FAILED(m_lpDDS->CreateSurface(&ddsd, &m_lpBack, NULL)))
	{
		return;
	}
	if(FAILED(m_lpDDS->CreateSurface(&ddsd, &m_lpParent, NULL)))
	{
		return;
	}
}

void CShowLoading::DrawLine(HDC hDC, int x1, int y1, int x2, int y2, int iWidth, COLORREF color)
{
	HPEN hPen = CreatePen(PS_SOLID, iWidth, color);
	SelectObject(hDC, hPen);
	MoveToEx(hDC, x1, y1, NULL);
	LineTo(hDC, x2, y2);
	DeleteObject(hPen);
}

BOOL CShowLoading::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitDX();
	return TRUE;
}

void CShowLoading::SafeStopThread()
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

void CShowLoading::CreateOwnerEx()
{
	Create(IDD_SET_LOCA_POINT_DLG, m_pParent);
}

void CShowLoading::TransparentProce()
{
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(&ddsd));
	ddsd.dwSize = sizeof(ddsd);
	m_lpParent->Lock(NULL, &ddsd, 0, NULL);
	PBYTE pBuf = (PBYTE)ddsd.lpSurface;

	if(ddsd.ddpfPixelFormat.dwRGBBitCount == 32)
	{
		int iLen = ddsd.dwHeight * ddsd.lPitch;
		for(int iIndex=0; iIndex<iLen; iIndex++)
		{
			int iValue = (int)(*pBuf * 0.3);
			if(iValue < 0)iValue = 0;
			if(iValue > 255) iValue = 255;
			*pBuf++ = iValue;
		}
	}
	else if(ddsd.ddpfPixelFormat.dwRGBBitCount == 16)
	{
		for(int iIndexY=0; iIndexY<(int)ddsd.dwHeight; iIndexY++)
		{
			for(int iIndexX=0; iIndexX<(int)(ddsd.dwWidth); iIndexX++)
			{
				short color;
				memcpy(&color, pBuf, 2);
				int r = (int)((color >> 11) & 0x1F);
				int g = (int)((color>> 5) & 0x3F);
				int b = (int)((color) & 0x1F);
				r <<= 3;
				g <<= 2;
				b <<= 3;
				r = (int)(r * 0.3);
				g = (int)(g * 0.3);
				b = (int)(b * 0.3);
				r >>= 3;
				g >>= 2;
				b >>= 3;
				if(r<0) r=0;
				else if(r>255)r=255;
				if(g<0) g=0;
				else if(g>255)g=255;
				if(b<0) b=0;
				else if(b>255)b=255;
				color = (unsigned short)(((unsigned short)r<<11) + ((unsigned short)g<<5) + (unsigned short)b);
				memcpy(pBuf, &color, 2);
				pBuf += 2;
			}
		}
	}

	m_lpParent->Unlock(NULL);
}

void CShowLoading::Start()
{
	RECT cRect;
	::GetClientRect(m_pParent->GetSafeHwnd(), &cRect);
	::ClientToScreen(m_pParent->GetSafeHwnd(), (LPPOINT)&cRect);
	::ClientToScreen(m_pParent->GetSafeHwnd(), (LPPOINT)&cRect+1);
	RECT cRectMoveTo;
	cRectMoveTo.left = cRect.left+((cRect.right-cRect.left-150)>>1);
	cRectMoveTo.top = cRect.top + ((cRect.bottom-cRect.top-150)>>1);
	cRectMoveTo.right = cRectMoveTo.left+150;
	cRectMoveTo.bottom = cRectMoveTo.top+150;
	MoveWindow(&cRectMoveTo, TRUE);

	::ScreenToClient(m_pParent->GetSafeHwnd(), (LPPOINT)&cRectMoveTo);
	::ScreenToClient(m_pParent->GetSafeHwnd(), (LPPOINT)&cRectMoveTo+1);

	HDC hParentDC, hBufDC;
	m_lpParent->GetDC(&hBufDC);
	hParentDC = ::GetDC(m_pParent->GetSafeHwnd());
	BitBlt(hBufDC, 0, 0, 150, 150, hParentDC, cRectMoveTo.left, cRectMoveTo.top, SRCCOPY);
	::ReleaseDC(m_pParent->GetSafeHwnd(), hParentDC);
	m_lpParent->ReleaseDC(hBufDC);

	TransparentProce();

	ShowWindow(SW_SHOW);
	SafeStopThread();
	m_iPointIndex = 0;
	m_fExit = FALSE;
	m_hThreadMain = CreateThread(NULL, 0, MainThread, this, 0, NULL);
}

void CShowLoading::Stop()
{
	SafeStopThread();
	this->ShowWindow(SW_HIDE);
}

void CShowLoading::PrepareShow()
{
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	m_lpBack->Lock(NULL, &ddsd, 0, NULL);
	BYTE* pSurface = (BYTE*)ddsd.lpSurface;
	memset(pSurface, 0, ddsd.lPitch * ddsd.dwHeight);
	m_lpBack->Unlock(NULL);

	HDC hTmpDC, hParentDC;
	m_lpBack->GetDC(&hTmpDC);
	m_lpParent->GetDC(&hParentDC);
	BitBlt(hTmpDC, 0, 0, 150, 150, hParentDC, 0, 0, SRCCOPY);
	m_lpParent->ReleaseDC(hParentDC);
	for(int iIndex=0; iIndex<12; iIndex++)
	{
		DrawLine(hTmpDC, Loca1[iIndex<<1], Loca1[(iIndex<<1)+1],
			Loca2[iIndex<<1], Loca2[(iIndex<<1)+1], 3, RGB(128, 128, 128));
	}

	DrawLine(hTmpDC, Loca1[m_iPointIndex<<1], Loca1[(m_iPointIndex<<1)+1],
		Loca2[m_iPointIndex<<1], Loca2[(m_iPointIndex<<1)+1], 3, RGB(131, 131, 131));

	int iTmpIndex = m_iPointIndex - 1;
	if(iTmpIndex < 0) iTmpIndex = iTmpIndex + 12;
	DrawLine(hTmpDC, Loca1[iTmpIndex<<1], Loca1[(iTmpIndex<<1)+1],
		Loca2[iTmpIndex<<1], Loca2[(iTmpIndex<<1)+1], 3, RGB(162, 162, 162));

	iTmpIndex = m_iPointIndex - 2;
	if(iTmpIndex < 0) iTmpIndex = iTmpIndex + 12;
	DrawLine(hTmpDC, Loca1[iTmpIndex<<1], Loca1[(iTmpIndex<<1)+1],
		Loca2[iTmpIndex<<1], Loca2[(iTmpIndex<<1)+1], 3, RGB(192, 192, 192));

	iTmpIndex = m_iPointIndex - 3;
	if(iTmpIndex < 0) iTmpIndex = iTmpIndex + 12;
	DrawLine(hTmpDC, Loca1[iTmpIndex<<1], Loca1[(iTmpIndex<<1)+1],
		Loca2[iTmpIndex<<1], Loca2[(iTmpIndex<<1)+1], 3, RGB(224, 224, 224));

	iTmpIndex = m_iPointIndex - 4;
	if(iTmpIndex < 0) iTmpIndex = iTmpIndex + 12;
	DrawLine(hTmpDC, Loca1[iTmpIndex<<1], Loca1[(iTmpIndex<<1)+1],
		Loca2[iTmpIndex<<1], Loca2[(iTmpIndex<<1)+1], 3, RGB(255, 255, 255));

	EnterCriticalSection(&m_csString);
	if(m_strInfo.GetLength() > 0)
	{
		SetBkMode(hTmpDC, 3);
		SetTextColor(hTmpDC, RGB(255, 255, 255));
		SelectObject(hTmpDC, g_hFontInfo);
		TextOut(hTmpDC, 15, 100, m_strInfo.GetBuffer(), m_strInfo.GetLength());
	}
	m_lpBack->ReleaseDC(hTmpDC);
	LeaveCriticalSection(&m_csString);
}

void CShowLoading::OnPaint()
{
	CDialog::OnPaint();
	if(m_lpBack)
	{
		PrepareShow();
		RECT cRect;
		::GetClientRect(GetSafeHwnd(), &cRect);
		HDC hSrcDC, hWinDC;
		m_lpBack->GetDC(&hSrcDC);
		hWinDC = ::GetDC(GetSafeHwnd());
		SetStretchBltMode(hWinDC, COLORONCOLOR);
		StretchBlt(hWinDC, 0, 0, cRect.right-cRect.left, cRect.bottom-cRect.top,
			hSrcDC, 0, 0,150, 150, SRCCOPY);
		::ReleaseDC(GetSafeHwnd(), hWinDC);
		m_lpBack->ReleaseDC(hSrcDC);
	}
}

DWORD WINAPI CShowLoading::MainThread(LPVOID pParam)
{
	CShowLoading* pDlg = (CShowLoading*)pParam;
	while(!pDlg->m_fExit)
	{
		pDlg->m_iPointIndex--;
		if(pDlg->m_iPointIndex <= -1) pDlg->m_iPointIndex = 11;
		pDlg->SendMessage(WM_PAINT, 0, 0);
		Sleep(100);
	}
	return 0;
}

void CShowLoading::SetInfoWord(LPCSTR pszInfo)
{
	EnterCriticalSection(&m_csString);
	m_strInfo = pszInfo;
	LeaveCriticalSection(&m_csString);
}

void CShowLoading::OnCancel()
{

}

void CShowLoading::OnOK()
{

}

