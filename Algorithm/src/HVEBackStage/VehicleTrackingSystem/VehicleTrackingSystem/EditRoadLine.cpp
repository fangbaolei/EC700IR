#include "stdafx.h"
#include "EditRoadLine.h"
#include "VehicleTrackingSystemDlg.h"
#include ".\editroadline.h"

IMPLEMENT_DYNAMIC(CEditRoadLineDlg, CDialog)

#define ID_MARK_END_POINT			10086
#define ID_UNMARK_END_POINT			10087
#define ID_MARK_SELECT_END_POINT	10088

CEditRoadLineDlg::CEditRoadLineDlg(CWnd* pParent /* = NULL */)
:CDialog(CEditRoadLineDlg::IDD, pParent)
{
	m_pParent = pParent;
	m_lpBack = NULL;
	m_lpdds = NULL;
	m_iCurrentIndex = -1;
	m_iCurrentEndPos = -1;
	m_iSelectPoint = -1;
}

CEditRoadLineDlg::~CEditRoadLineDlg()
{
	m_pParent = NULL;

	if(m_hMainThread)
	{
		m_fExit = TRUE;
		int iWaitTimes = 0;
		int MAXWAITTIME = 8;
		while(WaitForSingleObject(m_hMainThread, 500) == WAIT_TIMEOUT && iWaitTimes < MAXWAITTIME)
		{
			iWaitTimes++;
		}

		if(iWaitTimes >= MAXWAITTIME)
		{
			TerminateThread(m_hMainThread, 0);
		}
		else
		{
			CloseHandle(m_hMainThread);
		}
	}
	m_hMainThread = NULL;

	if(m_lpBack)
	{
		m_lpBack->Release();
		m_lpBack = NULL;
	}
	if(m_lpdds)
	{
		m_lpdds->Release();
		m_lpdds = NULL;
	}
}

void CEditRoadLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListLoca);
}

BOOL CEditRoadLineDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
			return TRUE;
		case VK_RETURN:
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CEditRoadLineDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_MARK_END_POINT, OnMarkEndPoint)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnNMDblclkList1)
	ON_COMMAND(ID_MARK_SELECT_END_POINT, OnMarkSelectEndPoint)
	ON_COMMAND(ID_UNMARK_END_POINT, OnUnMarkEndPoint)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
END_MESSAGE_MAP()

void CEditRoadLineDlg::UpDateLocaList(void)
{
	m_ListLoca.DeleteAllItems();
	CVehicleTrackingSystemDlg* pDlg = (CVehicleTrackingSystemDlg*)m_pParent;
	EnterCriticalSection(&pDlg->m_csLocaRoadLine);
	POSITION pos = pDlg->m_LocaRoadLineType.GetHeadPosition();
	int iIndex = 0;
	while(pos)
	{
		LocaRoadLineType* pLocaRoadLine = pDlg->m_LocaRoadLineType.GetNext(pos);
		m_ListLoca.InsertItem(iIndex, "", 0);
		m_ListLoca.SetItemText(iIndex, 0, pLocaRoadLine->pLoca->szLocaName);
		m_ListLoca.SetItemText(iIndex, 1, pLocaRoadLine->pLoca->szServerName);
		int iEndPointCount = (int)pLocaRoadLine->pEndPosType.GetCount();
		char szInfo[64];
		sprintf(szInfo, "%d", iEndPointCount);
		m_ListLoca.SetItemText(iIndex, 2, szInfo);
		iIndex++;
	}
	LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
}

BOOL CEditRoadLineDlg::InitDX()
{
	if(FAILED(DirectDrawCreateEx(NULL, (void**)&m_lpdds, IID_IDirectDraw7, NULL)))
	{
		return FALSE;
	}
	if(FAILED(m_lpdds->SetCooperativeLevel(GetDlgItem(IDC_STATIC_MAP)->GetSafeHwnd(), DDSCL_NORMAL)))
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
	if(FAILED(m_lpdds->CreateSurface(&ddsd, &m_lpBack, NULL)))
	{
		return FALSE;
	}
	m_fExit = FALSE;
	m_hMainThread = CreateThread(NULL, 0, MainThread, this, 0, NULL);
	if(m_hMainThread == INVALID_HANDLE_VALUE)
	{
		OnCancel();
		return FALSE;
	}
	return TRUE;
}

BOOL CEditRoadLineDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	RECT cRect;
	cRect.left = (GetSystemMetrics(SM_CXSCREEN) - 1324) >> 1;
	cRect.right = cRect.left + 1324;
	cRect.top = (GetSystemMetrics(SM_CYSCREEN) - 836 - 10) >> 1;
	cRect.bottom = cRect.top + 836;
	MoveWindow(&cRect, TRUE);
	cRect.left = 5;
	cRect.right = 1026;
	cRect.top = 5;
	cRect.bottom = 791;
	GetDlgItem(IDC_STATIC_MAP)->MoveWindow(&cRect, TRUE);
	cRect.left = 1031;
	cRect.right = 1319;
	cRect.top = 2;
	cRect.bottom = 791;
	m_ListLoca.MoveWindow(&cRect, TRUE);
	m_ListLoca.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListLoca.InsertColumn(0, "断面名称", LVCFMT_LEFT, 100);
	m_ListLoca.InsertColumn(1, "所属数据站名", LVCFMT_LEFT, 100);
	m_ListLoca.InsertColumn(2, "终点数", LVCFMT_LEFT, 100);

	if(InitDX() != TRUE)
	{
		MessageBox("路径编辑界面初始化失败!", "ERROR", MB_OK|MB_ICONERROR);
		OnCancel();
		return FALSE;
	}

	UpDateLocaList();
	if(m_ListLoca.GetItemCount() > 0)
	{
		m_iCurrentIndex = 0;
	}

	return TRUE;
}

void CEditRoadLineDlg::OnPaint()
{
	CDialog::OnPaint();
	if(m_lpBack)
	{
		RECT cRect;
		::GetClientRect(GetDlgItem(IDC_STATIC_MAP)->GetSafeHwnd(), &cRect);
		HDC hSrcDC, hWinDC;
		m_lpBack->GetDC(&hSrcDC);
		hWinDC = ::GetDC(GetDlgItem(IDC_STATIC_MAP)->GetSafeHwnd());
		SetStretchBltMode(hWinDC, COLORONCOLOR);
		StretchBlt(hWinDC, 0, 0, cRect.right-cRect.left, cRect.bottom-cRect.top,
			hSrcDC, 0, 0,1024, 786, SRCCOPY);
		::ReleaseDC(GetDlgItem(IDC_STATIC_MAP)->GetSafeHwnd(), hWinDC);
		m_lpBack->ReleaseDC(hSrcDC);
	}
}

void CEditRoadLineDlg::GetPos()
{
	RECT cRect;
	GetDlgItem(IDC_STATIC_MAP)->GetClientRect(&cRect);
	GetDlgItem(IDC_STATIC_MAP)->ClientToScreen((LPPOINT)&cRect);
	GetDlgItem(IDC_STATIC_MAP)->ClientToScreen((LPPOINT)&cRect+1);
	POINT cPoint;
	GetCursorPos(&cPoint);
	cPoint.x -= cRect.left;
	cPoint.y -= cRect.top;
	m_TmpPoint.x = cPoint.x;
	m_TmpPoint.y = cPoint.y;
}

void CEditRoadLineDlg::PrepareOutPut()
{
	if(m_lpBack == NULL)
	{
		return;
	}
	HDC hOffDC;
	m_lpBack->GetDC(&hOffDC);
	if(m_pParent)
	{
		CVehicleTrackingSystemDlg* pTmp = (CVehicleTrackingSystemDlg*)m_pParent;
		EnterCriticalSection(&pTmp->m_csLocaRoadLine);
		if(pTmp->m_lpBack)
		{
			HDC hBackDC;
			pTmp->m_lpBack->GetDC(&hBackDC);
			BitBlt(hOffDC, 0, 0, 1024, 786, hBackDC, 0, 0, SRCCOPY);

			int iIndex = 0;
			POSITION pos = pTmp->m_LocaRoadLineType.GetHeadPosition();
			while(pos)
			{
				LocaRoadLineType* pLocaRoadLine = pTmp->m_LocaRoadLineType.GetNext(pos);
				if(iIndex == m_iCurrentIndex)
				DrawEllsip(hOffDC, pLocaRoadLine->pLoca->iPositionX-10,
					pLocaRoadLine->pLoca->iPositionY-10, pLocaRoadLine->pLoca->iPositionX+10,
					pLocaRoadLine->pLoca->iPositionY + 10, RGB(255, 0, 0));
				else
				DrawEllsip(hOffDC, pLocaRoadLine->pLoca->iPositionX-10,
					pLocaRoadLine->pLoca->iPositionY-10, pLocaRoadLine->pLoca->iPositionX+10,
					pLocaRoadLine->pLoca->iPositionY + 10, RGB(0, 255, 0));
				if(m_TmpPoint.x >= pLocaRoadLine->pLoca->iPositionX-10 && m_TmpPoint.x <= pLocaRoadLine->pLoca->iPositionX+10
					&& m_TmpPoint.y >= pLocaRoadLine->pLoca->iPositionY-10 && m_TmpPoint.y <= pLocaRoadLine->pLoca->iPositionY+10)
				{
					if(iIndex == m_iCurrentIndex)
					ShowPosInfo(hOffDC, pLocaRoadLine->pLoca->iPositionX, pLocaRoadLine->pLoca->iPositionY,
						pLocaRoadLine->pLoca->szLocaName, 3, RGB(255, 0, 0), RGB(255, 255, 255));
					else
						ShowPosInfo(hOffDC, pLocaRoadLine->pLoca->iPositionX, pLocaRoadLine->pLoca->iPositionY,
						pLocaRoadLine->pLoca->szLocaName, 3, RGB(0, 255, 0), RGB(255, 255, 255));
				}
				iIndex++;
			}

			pTmp->m_lpBack->ReleaseDC(hBackDC);
		}
		LeaveCriticalSection(&pTmp->m_csLocaRoadLine);
	}
	m_lpBack->ReleaseDC(hOffDC);
}

void CEditRoadLineDlg::DrawLine(HDC hDC, int x1, int y1, int x2, int y2, int iWidth, COLORREF color)
{
	HPEN hPen = CreatePen(PS_SOLID, iWidth, color);
	SelectObject(hDC, hPen);
	MoveToEx(hDC, x1, y1, NULL);
	LineTo(hDC, x2, y2);
	DeleteObject(hPen);
}

void CEditRoadLineDlg::DrawEllsip(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, color);
	HBRUSH hBrush = CreateSolidBrush(color);
	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);
	Ellipse(hDC, x1, y1, x2, y2);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

void CEditRoadLineDlg::ShowStartPos(HDC hDC)
{

}

void CEditRoadLineDlg::ShowEndPosList(HDC hDC)
{
	if(m_iCurrentIndex < 0)
	{
		return;
	}
	CVehicleTrackingSystemDlg* pDlg = (CVehicleTrackingSystemDlg*)m_pParent;
	EnterCriticalSection(&pDlg->m_csLocaRoadLine);
	POSITION pos = pDlg->m_LocaRoadLineType.FindIndex(m_iCurrentIndex);
	if(pos)
	{
		LocaRoadLineType* pTmpLocaRoadLine = pDlg->m_LocaRoadLineType.GetAt(pos);
		if(pTmpLocaRoadLine->pEndPosType.GetCount() > 0)//pEndLocaList.GetCount() > 0)
		{
			int iIndex = 0;
			EnterCriticalSection(&pTmpLocaRoadLine->csEndList);
			POSITION posEndPoint = pTmpLocaRoadLine->pEndPosType.GetHeadPosition();//pEndLocaList.GetHeadPosition();
			while(posEndPoint)
			{
				EndLocaType* pTmpLoca = pTmpLocaRoadLine->pEndPosType.GetNext(posEndPoint);//pEndLocaList.GetNext(posEndPoint);
				if(iIndex == m_iCurrentEndPos)
				{
					DrawLine(hDC, pTmpLoca->pLoca->iPositionX - 12, pTmpLoca->pLoca->iPositionY, 
						pTmpLoca->pLoca->iPositionX, pTmpLoca->pLoca->iPositionY - 20, 3, RGB(255, 0, 0));
					DrawLine(hDC, pTmpLoca->pLoca->iPositionX - 12, pTmpLoca->pLoca->iPositionY, 
						pTmpLoca->pLoca->iPositionX, pTmpLoca->pLoca->iPositionY + 20, 3, RGB(255, 0, 0));
					DrawLine(hDC, pTmpLoca->pLoca->iPositionX + 12, pTmpLoca->pLoca->iPositionY, 
						pTmpLoca->pLoca->iPositionX, pTmpLoca->pLoca->iPositionY - 20, 3, RGB(255, 0, 0));
					DrawLine(hDC, pTmpLoca->pLoca->iPositionX + 12, pTmpLoca->pLoca->iPositionY, 
						pTmpLoca->pLoca->iPositionX, pTmpLoca->pLoca->iPositionY + 20, 3, RGB(255, 0, 0));
				}
				else
				{
					DrawLine(hDC, pTmpLoca->pLoca->iPositionX - 12, pTmpLoca->pLoca->iPositionY, 
						pTmpLoca->pLoca->iPositionX, pTmpLoca->pLoca->iPositionY - 20, 3, RGB(0, 0, 255));
					DrawLine(hDC, pTmpLoca->pLoca->iPositionX - 12, pTmpLoca->pLoca->iPositionY, 
						pTmpLoca->pLoca->iPositionX, pTmpLoca->pLoca->iPositionY + 20, 3, RGB(0, 0, 255));
					DrawLine(hDC, pTmpLoca->pLoca->iPositionX + 12, pTmpLoca->pLoca->iPositionY, 
						pTmpLoca->pLoca->iPositionX, pTmpLoca->pLoca->iPositionY - 20, 3, RGB(0, 0, 255));
					DrawLine(hDC, pTmpLoca->pLoca->iPositionX + 12, pTmpLoca->pLoca->iPositionY, 
						pTmpLoca->pLoca->iPositionX, pTmpLoca->pLoca->iPositionY + 20, 3, RGB(0, 0, 255));
				}
				iIndex++;
			}
			LeaveCriticalSection(&pTmpLocaRoadLine->csEndList);
		}
	}
	LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
}

void CEditRoadLineDlg::ShowPosInfo(HDC hDC, int x, int y, char* pszInfo, int iFrameWidth, COLORREF color1, COLORREF color2)
{
	HPEN hPen;
	HBRUSH hBrush;
	hPen = CreatePen(PS_SOLID, iFrameWidth, color1);
	hBrush = CreateSolidBrush(color2);
	SelectObject(hDC, hPen);
	SelectObject(hDC, hBrush);
	BOOL fIsLeft = FALSE;
	BOOL fIsButton = FALSE;

	if(x > 764) fIsLeft = TRUE;
	if(y > 726) fIsButton = TRUE;

	int StartX;
	int StartY;
	if(fIsLeft) StartX = x - 230;
	else StartX = x + 30;
	if(fIsButton) StartY = y + 20;
	else StartY = y - 50;
	int iWidth = 0;
	if(fIsLeft) iWidth = 200;
	else iWidth = 200;

	RoundRect(hDC, StartX, StartY, StartX+iWidth, StartY+30, 30, 15);
	POINT cPoint1, cPoint2;
	if(fIsLeft)
	{
		cPoint1.x = StartX+200;
		cPoint2.x = StartX+180;
	}
	else
	{
		cPoint1.x = StartX;
		cPoint2.x = StartX+10;
	}
	if(fIsButton)
	{
		cPoint1.y = StartY;
		cPoint2.y = StartY+5;
	}
	else
	{
		cPoint1.y = StartY+25;
		cPoint2.y = StartY+30;
	}

	DrawLine(hDC, x, y, cPoint1.x, cPoint1.y, iFrameWidth, color1);
	DrawLine(hDC, x, y, cPoint2.x, cPoint2.y, iFrameWidth, color1);

	SetTextColor(hDC, RGB(0, 0, 0));
	SetBkMode(hDC, 3);
	int iTextLen = (int)strlen(pszInfo);
	if(iTextLen > 20) iTextLen = 20;
	TextOut(hDC, StartX+5, StartY+5, pszInfo, iTextLen);

	DeleteObject(hBrush);
	DeleteObject(hPen);
}

void CEditRoadLineDlg::ShowEditLine(HDC hDC)
{
	CVehicleTrackingSystemDlg* pDlg = (CVehicleTrackingSystemDlg*)m_pParent;
	EnterCriticalSection(&pDlg->m_csLocaRoadLine);
	if(m_iCurrentIndex < 0 || m_iCurrentIndex >= pDlg->m_LocaRoadLineType.GetCount())
	{
		LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
		return;
	}
	POSITION pos = pDlg->m_LocaRoadLineType.FindIndex(m_iCurrentIndex);
	if(pos)
	{
		LocaRoadLineType* pTmpLoca = pDlg->m_LocaRoadLineType.GetAt(pos);
		EnterCriticalSection(&pTmpLoca->csEndList);
		if(m_iCurrentEndPos < 0 || m_iCurrentEndPos >= pTmpLoca->pEndPosType.GetCount())
		{
			LeaveCriticalSection(&pTmpLoca->csEndList);
			LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
			return;
		}

		POINT PointPre,PointNext;
		POSITION posEnd = pTmpLoca->pEndPosType.FindIndex(m_iCurrentEndPos);
		if(posEnd)
		{
			EndLocaType* pTmpEnd = pTmpLoca->pEndPosType.GetAt(posEnd);
			PointPre.x = pTmpLoca->pLoca->iPositionX;
			PointPre.y = pTmpLoca->pLoca->iPositionY;

			EnterCriticalSection(&pTmpEnd->csPointList);
			POSITION posPoint = pTmpEnd->pRoadLinePointList.GetHeadPosition();
			while(posPoint)
			{
				POINT* pTmpPoint = pTmpEnd->pRoadLinePointList.GetNext(posPoint);
				PointNext.x = pTmpPoint->x;
				PointNext.y = pTmpPoint->y;

				DrawLine(hDC, pTmpPoint->x-5, pTmpPoint->y-5, pTmpPoint->x+5, pTmpPoint->y+5, 1, RGB(255, 0, 0));
				DrawLine(hDC, pTmpPoint->x-5, pTmpPoint->y+5, pTmpPoint->x+5, pTmpPoint->y-5, 1, RGB(255, 0, 0));
				DrawLine(hDC, PointPre.x, PointPre.y, PointNext.x, PointNext.y, 1, RGB(255, 0, 0));
				PointPre.x = PointNext.x;
				PointPre.y = PointNext.y;
			}
			
			PointNext.x = pTmpEnd->pLoca->iPositionX;
			PointNext.y = pTmpEnd->pLoca->iPositionY;
			DrawLine(hDC, PointPre.x, PointPre.y, PointNext.x, PointNext.y, 1, RGB(255, 0, 0));

			LeaveCriticalSection(&pTmpEnd->csPointList);
		}

		LeaveCriticalSection(&pTmpLoca->csEndList);
	}
	LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
}

DWORD WINAPI CEditRoadLineDlg::MainThread(LPVOID lpParam)
{
	if(lpParam == NULL)
	{
		return 0xFFFFFFFF;
	}
	CEditRoadLineDlg* pDlg = (CEditRoadLineDlg*)lpParam;
	while(!pDlg->m_fExit)
	{
		pDlg->GetPos();
		pDlg->PrepareOutPut();
		HDC hDC;
		pDlg->m_lpBack->GetDC(&hDC);

		pDlg->ShowStartPos(hDC);
		pDlg->ShowEndPosList(hDC);
		pDlg->ShowEditLine(hDC);

		pDlg->m_lpBack->ReleaseDC(hDC);
		pDlg->SendMessage(WM_PAINT, 0, 0);
		Sleep(20);
	}
	return 0;
}

BOOL CEditRoadLineDlg::CheckIfCovertInEndPoint(POINT cPoint, int& iCurrentEndPos)
{
	CVehicleTrackingSystemDlg* pDlg = (CVehicleTrackingSystemDlg*)m_pParent;
	EnterCriticalSection(&pDlg->m_csLocaRoadLine);
	if(m_iCurrentIndex < 0 || m_iCurrentIndex >= pDlg->m_LocaRoadLineType.GetCount())
	{
		LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
		return FALSE;
	}
	
	int iIndex = 0;
	POSITION pos = pDlg->m_LocaRoadLineType.FindIndex(m_iCurrentIndex);
	LocaRoadLineType* pTmp = (LocaRoadLineType*)pDlg->m_LocaRoadLineType.GetNext(pos);
	EnterCriticalSection(&pTmp->csEndList);
	POSITION posEndPoint = pTmp->pEndPosType.GetHeadPosition();
	while(posEndPoint)
	{
		EndLocaType* pLoca = pTmp->pEndPosType.GetNext(posEndPoint);
		if(cPoint.x >= pLoca->pLoca->iPositionX-11 && cPoint.x <= pLoca->pLoca->iPositionX+11
			&& cPoint.y >= pLoca->pLoca->iPositionY-11 && cPoint.y <= pLoca->pLoca->iPositionY+11)
		{
			iCurrentEndPos = iIndex;
			LeaveCriticalSection(&pTmp->csEndList);
			LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
			return TRUE;
		}
		iIndex++;
	}
	LeaveCriticalSection(&pTmp->csEndList);
	LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
	return FALSE;
}

BOOL CEditRoadLineDlg::CheckIfCoverPoint(POINT cPoint, int& iCurrentPoint)
{
	CVehicleTrackingSystemDlg* pDlg = (CVehicleTrackingSystemDlg*)m_pParent;
	EnterCriticalSection(&pDlg->m_csLocaRoadLine);
	if(pDlg->m_LocaRoadLineType.GetCount() <= 0)
	{
		LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
		return FALSE;
	}

	int iIndex = 0;
	POSITION pos = pDlg->m_LocaRoadLineType.GetHeadPosition();
	while(pos)
	{
		LocaRoadLineType* pTmpLoca = pDlg->m_LocaRoadLineType.GetNext(pos);
		if(cPoint.x >= pTmpLoca->pLoca->iPositionX-11 && cPoint.x <= pTmpLoca->pLoca->iPositionX+11
			&& cPoint.y >= pTmpLoca->pLoca->iPositionY-11 && cPoint.y <= pTmpLoca->pLoca->iPositionY+11)
		{
			iCurrentPoint = iIndex;
			LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
			return TRUE;
		}
		iIndex++;
	}
	LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
	return FALSE;
}

void CEditRoadLineDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	CRect cTmpRect;
	POINT cPoint;
	cPoint.x = point.x;
	cPoint.y = point.y;
	GetDlgItem(IDC_STATIC_MAP)->GetWindowRect(&cTmpRect);
	ScreenToClient(&cTmpRect);
	if(cTmpRect.PtInRect(point))
	{
		int iTmp = -1;
		if(CheckIfCovertInEndPoint(m_TmpPoint, iTmp) == TRUE)
		{
			if(iTmp >= 0)
			{
				if(iTmp == m_iCurrentEndPos)
				{
					m_iSelectPoint = iTmp;
					CMenu cMecu;
					cMecu.CreatePopupMenu();
					cMecu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_UNMARK_END_POINT, "取消终点标记");
					CPoint cTmpPoint;
					GetCursorPos(&cTmpPoint);
					cMecu.TrackPopupMenu(TPM_LEFTALIGN, cTmpPoint.x, cTmpPoint.y, this);
				}
				else
				{
					m_iSelectPoint = iTmp;
					CMenu cMecu;
					cMecu.CreatePopupMenu();
					cMecu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_UNMARK_END_POINT, "取消终点标记");
					cMecu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_MARK_SELECT_END_POINT, "标记为选中终点");
					CPoint cTmpPoint;
					GetCursorPos(&cTmpPoint);
					cMecu.TrackPopupMenu(TPM_LEFTALIGN, cTmpPoint.x, cTmpPoint.y, this);
				}
			}
			return;
		}

		iTmp = -1;
		if(CheckIfCoverPoint(m_TmpPoint, iTmp) == TRUE)
		{
			if(iTmp == m_iCurrentIndex)
			{
				
			}
			else
			{
				m_iSelectPoint = iTmp;
				CMenu cMecu;
				cMecu.CreatePopupMenu();
				cMecu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_MARK_END_POINT, "标记为终点");
				CPoint cTmpPoint;
				GetCursorPos(&cTmpPoint);
				cMecu.TrackPopupMenu(TPM_LEFTALIGN, cTmpPoint.x, cTmpPoint.y, this);
			}
		}
	}

	CDialog::OnRButtonDown(nFlags, point);
}

void CEditRoadLineDlg::OnMarkEndPoint()
{
	CVehicleTrackingSystemDlg* pDlg = (CVehicleTrackingSystemDlg*)m_pParent;
	EnterCriticalSection(&pDlg->m_csLocaRoadLine);
	if(m_iCurrentIndex < 0 || m_iCurrentIndex >= pDlg->m_LocaRoadLineType.GetCount())
	{
		LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
		return;
	}

	POSITION posCurrent = pDlg->m_LocaRoadLineType.FindIndex(m_iCurrentIndex);
	POSITION pos = pDlg->m_LocaRoadLineType.FindIndex(m_iSelectPoint);
	if(pos && posCurrent)
	{
		LocaRoadLineType* pStartPoint = pDlg->m_LocaRoadLineType.GetAt(posCurrent);
		LocaRoadLineType* pTmpLoca = pDlg->m_LocaRoadLineType.GetAt(pos);
		EndLocaType* pNewEndPoint = new EndLocaType();
		pNewEndPoint->pLoca = pTmpLoca->pLoca;
		EnterCriticalSection(&pStartPoint->csEndList);
		pStartPoint->pEndPosType.AddTail(pNewEndPoint);
		LeaveCriticalSection(&pStartPoint->csEndList);

	}
	LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
	int iTmpSelectPos = m_iSelectPoint;
	UpDateLocaList();
	m_iSelectPoint = iTmpSelectPos;
}

void CEditRoadLineDlg::OnMarkSelectEndPoint()
{
	m_iCurrentEndPos = m_iSelectPoint;
	m_iSelectPoint = -1;
}

void CEditRoadLineDlg::OnUnMarkEndPoint()
{
	CVehicleTrackingSystemDlg* pDlg = (CVehicleTrackingSystemDlg*)m_pParent;
	EnterCriticalSection(&pDlg->m_csLocaRoadLine);
	POSITION pos = pDlg->m_LocaRoadLineType.FindIndex(m_iCurrentIndex);
	if(pos)
	{
		LocaRoadLineType* pTmpLoca = pDlg->m_LocaRoadLineType.GetAt(pos);
		EnterCriticalSection(&pTmpLoca->csEndList);
		POSITION posEndPoint = pTmpLoca->pEndPosType.FindIndex(m_iSelectPoint);
		if(posEndPoint)
		{
			EndLocaType* pTmpEndLoca = pTmpLoca->pEndPosType.GetAt(posEndPoint);
			delete pTmpEndLoca;
			pTmpEndLoca = NULL;
			pTmpLoca->pEndPosType.RemoveAt(posEndPoint);
		}
		LeaveCriticalSection(&pTmpLoca->csEndList);
	}
	LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
	m_iCurrentEndPos = -1;
	int iTmpSelectPos = m_iSelectPoint;
	UpDateLocaList();
	m_iSelectPoint = iTmpSelectPos;
}

void CEditRoadLineDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	CVehicleTrackingSystemDlg* pDlg = (CVehicleTrackingSystemDlg*)m_pParent;
	EnterCriticalSection(&pDlg->m_csLocaRoadLine);
	int iSelectPoint = m_ListLoca.GetSelectionMark();
	if(iSelectPoint < 0 || iSelectPoint >= pDlg->m_LocaRoadLineType.GetCount())
	{
		LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
		return;
	}
	m_iCurrentIndex = iSelectPoint;
	LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
	*pResult = 0;
}

void CEditRoadLineDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CVehicleTrackingSystemDlg* pDlg = (CVehicleTrackingSystemDlg*)m_pParent;
	EnterCriticalSection(&pDlg->m_csLocaRoadLine);
	if(m_iCurrentIndex < 0 || m_iCurrentIndex >= pDlg->m_LocaRoadLineType.GetCount())
	{
		LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
		return;
	}
	POSITION pos = pDlg->m_LocaRoadLineType.FindIndex(m_iCurrentIndex);
	if(pos)
	{
		LocaRoadLineType* pTmpLoca = pDlg->m_LocaRoadLineType.GetAt(pos);
		EnterCriticalSection(&pTmpLoca->csEndList);
		if(m_iCurrentEndPos < 0 || m_iCurrentEndPos >= pTmpLoca->pEndPosType.GetCount())
		{
			LeaveCriticalSection(&pTmpLoca->csEndList);
			LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
			return;
		}

		POSITION posEnd = pTmpLoca->pEndPosType.FindIndex(m_iCurrentEndPos);
		if(posEnd)
		{
			EndLocaType* pTmpEnd = pTmpLoca->pEndPosType.GetAt(posEnd);
			POINT* pNewPoint = new POINT();
			pNewPoint->x = m_TmpPoint.x;
			pNewPoint->y = m_TmpPoint.y;
			EnterCriticalSection(&pTmpEnd->csPointList);
			pTmpEnd->pRoadLinePointList.AddTail(pNewPoint);
			LeaveCriticalSection(&pTmpEnd->csPointList);
		}

		LeaveCriticalSection(&pTmpLoca->csEndList);
	}
	LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CEditRoadLineDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	CVehicleTrackingSystemDlg* pDlg = (CVehicleTrackingSystemDlg*)m_pParent;
	EnterCriticalSection(&pDlg->m_csLocaRoadLine);
	if(m_iCurrentIndex < 0 || m_iCurrentIndex >= pDlg->m_LocaRoadLineType.GetCount())
	{
		LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
		return;
	}
	POSITION pos = pDlg->m_LocaRoadLineType.FindIndex(m_iCurrentIndex);
	if(pos)
	{
		LocaRoadLineType* pTmpLoca = pDlg->m_LocaRoadLineType.GetAt(pos);
		EnterCriticalSection(&pTmpLoca->csEndList);
		if(m_iCurrentEndPos < 0 || m_iCurrentEndPos >= pTmpLoca->pEndPosType.GetCount())
		{
			LeaveCriticalSection(&pTmpLoca->csEndList);
			LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
			return;
		}

		POSITION posEnd = pTmpLoca->pEndPosType.FindIndex(m_iCurrentEndPos);
		if(posEnd)
		{
			EndLocaType* pTmpEnd = pTmpLoca->pEndPosType.GetAt(posEnd);
			EnterCriticalSection(&pTmpEnd->csPointList);
			if(pTmpEnd->pRoadLinePointList.GetCount() > 0)
			{
				POINT* pDelPoint = pTmpEnd->pRoadLinePointList.RemoveTail();
				delete pDelPoint;
				pDelPoint = NULL;
			}
			LeaveCriticalSection(&pTmpEnd->csPointList);
		}

		LeaveCriticalSection(&pTmpLoca->csEndList);
	}
	LeaveCriticalSection(&pDlg->m_csLocaRoadLine);
	CDialog::OnRButtonDblClk(nFlags, point);
}
