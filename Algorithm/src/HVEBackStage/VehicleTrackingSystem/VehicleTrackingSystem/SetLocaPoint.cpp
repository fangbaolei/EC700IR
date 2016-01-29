#include "stdafx.h"
#include "SetLocaPoint.h"
#include "VehicleTrackingSystemDlg.h"
#include ".\setlocapoint.h"

IMPLEMENT_DYNAMIC(CSetLocaPointDlg, CDialog)

CSetLocaPointDlg::CSetLocaPointDlg(LPPOINT pPoint, CWnd* pParent /* = NULL */)
:CDialog(CSetLocaPointDlg::IDD, pParent)
{
	m_Point = pPoint;
	m_pParent = (CVehicleTrackingSystemDlg*)pParent;
	m_lpOffScreen = NULL;
	m_hThreadMain = NULL;
	m_fExit = TRUE;
}

CSetLocaPointDlg::~CSetLocaPointDlg()
{
	m_Point = NULL;
	m_pParent = NULL;

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

void CSetLocaPointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSetLocaPointDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CSetLocaPointDlg::OnCancel()
{
	CDialog::OnCancel();
}

BOOL CSetLocaPointDlg::InitDX()
{
	if(FAILED(DirectDrawCreateEx(NULL, (void**)&m_lpdds, IID_IDirectDraw7, NULL)))
	{
		return FALSE;
	}
	if(FAILED(m_lpdds->SetCooperativeLevel(GetSafeHwnd(), DDSCL_NORMAL)))
	{
		return FALSE;
	}
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.dwWidth = 1024;
	ddsd.dwHeight = 786;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	if(FAILED(m_lpdds->CreateSurface(&ddsd, &m_lpOffScreen, NULL)))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CSetLocaPointDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	RECT cRect;
	cRect.left = (GetSystemMetrics(SM_CXSCREEN) - 1024) >> 1;
	cRect.top = (GetSystemMetrics(SM_CYSCREEN) - 786) >> 1;
	cRect.right = cRect.left + 1024;
	cRect.bottom = cRect.top + 786;

	MoveWindow(&cRect, TRUE);

	if(InitDX() == FALSE)
	{
		OnCancel();
		return FALSE;
	}
	m_fExit = FALSE;
	m_hThreadMain = CreateThread(NULL, 0, MainThread, this, 0, NULL);
	if(m_hThreadMain == INVALID_HANDLE_VALUE)
	{
		OnCancel();
		return FALSE;
	}
	ShowCursor(FALSE);
	return TRUE;
}

BOOL CSetLocaPointDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE
			|| pMsg->wParam == VK_RETURN) return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CSetLocaPointDlg::OnPaint()
{
	CDialog::OnPaint();
	if(m_lpOffScreen)
	{
		RECT cRect;
		::GetClientRect(GetSafeHwnd(), &cRect);
		HDC hSrcDC, hWinDC;
		m_lpOffScreen->GetDC(&hSrcDC);
		hWinDC = ::GetDC(GetSafeHwnd());
		SetStretchBltMode(hWinDC, COLORONCOLOR);
		StretchBlt(hWinDC, 0, 0, cRect.right-cRect.left, cRect.bottom-cRect.top,
			hSrcDC, 0, 0,1024, 786, SRCCOPY);
		::ReleaseDC(GetSafeHwnd(), hWinDC);
		m_lpOffScreen->ReleaseDC(hSrcDC);
	}
}

void CSetLocaPointDlg::PrepareOutPut()
{
	if(m_lpOffScreen == NULL)
	{
		return;
	}

	HDC hOffDC;
	m_lpOffScreen->GetDC(&hOffDC);
	if(m_pParent->m_lpBack)
	{
		HDC hBackDC;
		m_pParent->m_lpBack->GetDC(&hBackDC);
		BitBlt(hOffDC, 0, 0, 1024, 786, hBackDC, 0, 0, SRCCOPY);
		m_pParent->m_lpBack->ReleaseDC(hBackDC);
	}
	m_lpOffScreen->ReleaseDC(hOffDC);
}

void CSetLocaPointDlg::DrawLine(HDC hDC, int x1, int y1, int x2, int y2, int iWidth, COLORREF color)
{
	HPEN hPen = CreatePen(PS_SOLID, iWidth, color);
	SelectObject(hDC, hPen);
	MoveToEx(hDC, x1, y1, NULL);
	LineTo(hDC, x2, y2);
	DeleteObject(hPen);
}

void CSetLocaPointDlg::DrawEllsip(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, color);
	HBRUSH hBrush = CreateSolidBrush(color);
	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);
	Ellipse(hDC, x1, y1, x2, y2);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

void CSetLocaPointDlg::DrawRoundRect(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color, COLORREF color1)
{
	HPEN hPen;
	HBRUSH hBrush;
	hPen = CreatePen(PS_SOLID, 1, color);
	hBrush = CreateSolidBrush(color1);
	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);
	RoundRect(hDC, x1, y1, x2, y2, 10, 10);
	DeleteObject(hBrush);
	DeleteObject(hPen);
}

DWORD WINAPI CSetLocaPointDlg::MainThread(LPVOID pParam)
{
	CSetLocaPointDlg* pDlg = (CSetLocaPointDlg*)pParam;
	while(!pDlg->m_fExit)
	{
		pDlg->PrepareOutPut();
		if(pDlg->m_fExit) break;
		RECT cRect;
		pDlg->GetClientRect(&cRect);
		pDlg->ClientToScreen((LPPOINT)&cRect);
		pDlg->ClientToScreen((LPPOINT)&cRect+1);
		POINT cPoint;
		GetCursorPos(&cPoint);
		cPoint.x -= cRect.left;
		cPoint.y -= cRect.top;
		pDlg->m_TmpPoint.x = cPoint.x;
		pDlg->m_TmpPoint.y = cPoint.y;
		HDC hDC;
		if(pDlg->m_fExit) break;
		pDlg->m_lpOffScreen->GetDC(&hDC);
		if(pDlg->m_TmpPoint.x >= 989 && pDlg->m_TmpPoint.x <= 1019
			&& pDlg->m_TmpPoint.y >= 13 && pDlg->m_TmpPoint.y <= 28)
		{
			pDlg->DrawRoundRect(hDC, 989, 5, 1019, 35, RGB(32, 32, 32), RGB(220, 32, 32));
			pDlg->DrawLine(hDC, 996, 13, 1012, 28, 3, RGB(255, 255, 255));
			pDlg->DrawLine(hDC, 996, 28, 1012, 13, 3, RGB(255, 255, 255));
		}
		else
		{
			pDlg->DrawRoundRect(hDC, 989, 5, 1019, 35, RGB(32, 32, 32), RGB(190, 190, 190));
			pDlg->DrawLine(hDC, 996, 13, 1012, 28, 3, RGB(255, 255, 255));
			pDlg->DrawLine(hDC, 996, 28, 1012, 13, 3, RGB(255, 255, 255));
		}

		int iIndex = 0;
		EnterCriticalSection(&pDlg->m_pParent->m_csLocaList);
		POSITION posLoca = pDlg->m_pParent->m_LocaList.GetHeadPosition();
		while(posLoca)
		{
			LocaType* pTmpLoca = pDlg->m_pParent->m_LocaList.GetNext(posLoca);
			if(iIndex == pDlg->m_pParent->m_iSelectIndex)
			{
				pTmpLoca->iPositionX = pDlg->m_Point->x;
				pTmpLoca->iPositionY = pDlg->m_Point->y;
				if(pTmpLoca->iPositionX >= 0 && pTmpLoca->iPositionX <= 1024
					&& pTmpLoca->iPositionY >= 0 && pTmpLoca->iPositionY <= 786)
				{
					pDlg->DrawEllsip(hDC, pTmpLoca->iPositionX-10, pTmpLoca->iPositionY-10,
						pTmpLoca->iPositionX+10, pTmpLoca->iPositionY+10, RGB(255, 0, 0));
				}
			}
			else
			{
				if(pTmpLoca->iPositionX >= 0 && pTmpLoca->iPositionX <= 1024
					&& pTmpLoca->iPositionY >= 0 && pTmpLoca->iPositionY <= 786)
				{
					pDlg->DrawEllsip(hDC, pTmpLoca->iPositionX-10, pTmpLoca->iPositionY-10,
						pTmpLoca->iPositionX+10, pTmpLoca->iPositionY+10, RGB(0, 255, 0));
				}
			}
			iIndex++;
		}
		LeaveCriticalSection(&pDlg->m_pParent->m_csLocaList);

		pDlg->DrawLine(hDC, cPoint.x-15, cPoint.y,
			cPoint.x+15, cPoint.y, 2, RGB(2, 2, 2));
		pDlg->DrawLine(hDC, cPoint.x, cPoint.y-15,
			cPoint.x, cPoint.y+15, 2, RGB(2, 2, 2));

		pDlg->m_lpOffScreen->ReleaseDC(hDC);
		if(pDlg->m_fExit) break;
		pDlg->SendMessage(WM_PAINT, 0, 0);
		Sleep(20);
	}
	return 0;
}

void CSetLocaPointDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_Point->x = point.x;
	m_Point->y = point.y;
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CSetLocaPointDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	ShowCursor(TRUE);
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

	CDialog::OnCancel();
}

void CSetLocaPointDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	POINT cPoint;
	cPoint.x = point.x;
	cPoint.y = point.y;
	if(cPoint.x >= 989 && cPoint.x <= 1019
		&& cPoint.y >= 13 && cPoint.y <= 28)
	{
		ShowCursor(TRUE);
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

		CDialog::OnCancel();
		return;
	}
	CDialog::OnLButtonUp(nFlags, point);
}
