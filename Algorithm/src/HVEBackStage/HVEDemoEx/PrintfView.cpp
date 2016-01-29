#include "StdAfx.h"
#include ".\printfview.h"

CPrintfView::CPrintfView(CQuery* pParent)
{
	m_ParentFrame = pParent;
	m_lpback = NULL;
	m_lpdds = NULL;
}

CPrintfView::~CPrintfView(void)
{
}
BEGIN_MESSAGE_MAP(CPrintfView, CFrameWnd)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SYSCOMMAND()
//	ON_WM_LBUTTONDBLCLK()
//	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

BOOL CPrintfView::OnEraseBkgnd(CDC* pDC)
{
	CBrush backBrush(RGB(180, 180, 50));
	pDC->SelectObject(&backBrush);
	CRect rect;
	pDC->GetClipBox(&rect);
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);

	return TRUE;
}

int CPrintfView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	if(FAILED(DirectDrawCreateEx(NULL, (void**)&m_lpdds, IID_IDirectDraw7, NULL)))
	{
		MessageBox("获取打印数据失败,打印失败!", "错误", MB_OK | MB_ICONERROR);
		OnDestroy();
	}
	if(FAILED(m_lpdds->SetCooperativeLevel(GetSafeHwnd(), DDSCL_NORMAL)))
	{
		MessageBox("获取打印数据失败,打印失败!", "错误", MB_OK | MB_ICONERROR);
		OnDestroy();
	}

	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.dwWidth = 794;
	ddsd.dwHeight = 1123;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	if(FAILED(m_lpdds->CreateSurface(&ddsd, &m_lpback, NULL)))
	{
		MessageBox("获取打印数据失败,打印失败!", "错误", MB_OK | MB_ICONERROR);
		OnDestroy();
	}

	GetPrintfInfo();

	// add by zhut 20121207
	m_ParentFrame->GetChartDC(&m_dcChart, m_iWidth, m_iHeight);

	return 0;
}

void CPrintfView::OnDestroy()
{
	CFrameWnd::OnDestroy();

	if(m_lpback != NULL)
	{
		m_lpback->Release();
		m_lpback = NULL;
	}
	if(m_lpdds != NULL)
	{
		m_lpdds->Release();
		m_lpdds = NULL;
	}
	
	if (m_dcChart.m_hDC != NULL)
	{
		m_dcChart.DeleteDC();
	}
	

	::PostMessage(m_ParentFrame->m_hWnd, WM_VIEWQUIT, 0, 0);
}

void CPrintfView::OnPaint()
{
	CPaintDC dc(this); 
	if(m_lpback == NULL) return;
	RECT rect;
	::GetClientRect(this->GetSafeHwnd(), &rect);
	ClientToScreen(&rect);
	HDC BuffDC;
	int lenght = (int)(794 * ((rect.bottom - rect.top) - 40) / 1123.);
	m_lpback->GetDC(&BuffDC);

	::StretchBlt(dc.m_hDC, ((rect.right - rect.left) - 2*lenght - 20)>>1, 
		20, lenght, (rect.bottom - rect.top) - 40, BuffDC, 0, 0, 794, 1123, SRCCOPY);

	m_lpback->ReleaseDC(BuffDC);

	// draw chart, by zhut 20121108
	::StretchBlt(dc.m_hDC, (((rect.right - rect.left) - 2*lenght - 20)>>1) + lenght + 20, 
		 		20, lenght,(rect.bottom - rect.top) - 40 , m_dcChart.m_hDC, 0, 0, m_iWidth, m_iHeight, SRCCOPY);

}

BOOL CPrintfView::GetPrintfInfo()
{

	INT nInfoCount = m_ParentFrame->m_ListReport.GetItemCount();
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	m_lpback->Lock(NULL, &ddsd, 0, NULL);
	UCHAR* buffer = (UCHAR*)ddsd.lpSurface;
	memset(buffer, 255, ddsd.lPitch * ddsd.dwHeight);
	m_lpback->Unlock(NULL);
	HDC tmpDC;	
	m_lpback->GetDC(&tmpDC);

	HFONT g_hFontPlate = CreateFont(30, 16, 0, 0, FW_HEAVY, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");
	HFONT hFontOld;
	hFontOld = (HFONT)SelectObject(tmpDC, g_hFontPlate);
	CString strTmp = "HVE视频车检器——";
	char szReportType[30];
	m_ParentFrame->m_StaticReportType.GetWindowText(szReportType, 30);
	if(strlen(szReportType) > 10)
	strTmp += (szReportType+10);
	TextOut(tmpDC, 200, 50, strTmp.GetBuffer(0), strTmp.GetLength());
	::DeleteObject(g_hFontPlate);
	g_hFontPlate = CreateFont(19, 9, 0, 0, FW_THIN, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");
	SelectObject(tmpDC, g_hFontPlate);
	strTmp.Format("%s%s", "断面名称：", m_ParentFrame->m_ParentFarm->m_rgLocation[m_ParentFrame->m_ParentFarm->m_nCurrentRoadID].strLocation_Name);
	TextOut(tmpDC, 50, 100, strTmp.GetBuffer(0), strTmp.GetLength());
	m_ParentFrame->m_StaticReportTimes.GetWindowText(szReportType, 30);
	strTmp.Format("日期：%s", szReportType);
	TextOut(tmpDC, 50, 125, strTmp.GetBuffer(0), strTmp.GetLength());
	char szSunCount[10];
	char szSpeed[10];
	char szCross[10];
	m_ParentFrame->m_EditSun.GetWindowText(szSunCount, 10);
	m_ParentFrame->m_EditSpeed.GetWindowText(szSpeed, 10);
	m_ParentFrame->m_EditCross.GetWindowText(szCross, 10);
	strTmp.Format("总车流量：%s辆    平均占有率：%s%%    平均车速：%s公里/小时", szSunCount, szCross, szSpeed);
	TextOut(tmpDC, 50, 150, strTmp.GetBuffer(0), strTmp.GetLength());
	strTmp.Format("车流量单位：辆    占有率单位：%%    车速单位：公里/小时");
	TextOut(tmpDC, 50, 175, strTmp.GetBuffer(0), strTmp.GetLength());
	int Index;
	for(Index=0; Index<35; Index++)
	{
		if(Index == 1)
		{
			MoveToEx(tmpDC, 60, 210+Index*26, NULL);
			LineTo(tmpDC, 720, 210+Index*26);
		}
		else
		{
			MoveToEx(tmpDC, 10, 210+Index*26, NULL);
			LineTo(tmpDC, 784, 210+Index*26);
		}

	}

	MoveToEx(tmpDC, 10, 210, NULL);
	LineTo(tmpDC, 10, 1094);
	MoveToEx(tmpDC, 784, 210, NULL);
	LineTo(tmpDC, 784, 1094);

	for(Index=0; Index<13; Index++)
	{
		if((Index%3) == 0)
		MoveToEx(tmpDC, 60+Index*55, 210, NULL);
		else
		MoveToEx(tmpDC, 60+Index*55, 236, NULL);
		LineTo(tmpDC, 60+Index*55, 1094);
	}

	::DeleteObject(g_hFontPlate);
	g_hFontPlate = CreateFont(16, 7, 0, 0, FW_THIN, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");
	SelectObject(tmpDC, g_hFontPlate);

	TextOut(tmpDC, 30, 228, "时", 2);
	TextOut(tmpDC, 132, 215, "车道1", 5);
	TextOut(tmpDC, 297, 215, "车道2", 5);
	TextOut(tmpDC, 462, 215, "车道3", 5);
	TextOut(tmpDC, 627, 215, "车道4", 5);

	LVCOLUMN    col;     
	col.fmt    =    LVIF_TEXT;   
	col.cchTextMax    =    10;
	col.pszText = new char[10];
	m_ParentFrame->m_ListReport.GetColumn(13, &col);	
	TextOut(tmpDC, 725, 228, col.pszText, 8);
	delete[] col.pszText;

	DeleteObject(g_hFontPlate);
	g_hFontPlate = CreateFont(16, 6, 0, 0, FW_THIN, false, false, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");
	SelectObject(tmpDC, g_hFontPlate);

	for(Index=0; Index<4; Index++)
	{
		TextOut(tmpDC, 65+Index*165, 241, "总车流量", 8);
		TextOut(tmpDC, 126+Index*165, 241, "占有率", 6);
		TextOut(tmpDC, 175+Index*165, 241, "平均车速", 8);
	}	

	for(Index=0; Index<nInfoCount; Index++)
	{
		char szInfo[10];
		for(int IntemCount=0; IntemCount<14; IntemCount++)
		{
			m_ParentFrame->m_ListReport.GetItemText(Index, IntemCount, szInfo, 10);
			if(IntemCount == 0)
				TextOut(tmpDC, 15, 265+Index*26, szInfo, (int)strlen(szInfo));
			else
				TextOut(tmpDC, 65+(IntemCount-1)*55, 265+Index*26, szInfo, (int)strlen(szInfo));
		}
	}

	DeleteObject(g_hFontPlate);
	SelectObject(tmpDC, hFontOld);
	m_lpback->ReleaseDC(tmpDC);
	return TRUE;
}

void CPrintfView::OnPrint()
{
	CDC dc;
	CPrintDialog printDlg(FALSE);
	if(printDlg.DoModal() != IDOK) return;
	if(!dc.Attach(printDlg.GetPrinterDC()))
	{
		MessageBox("找不到有效的打印设备，请检查与打印机设备连接是否正常!", "错误", MB_OK | MB_ICONERROR);
		return;
	}
	dc.m_bPrinting = TRUE;
	DOCINFO di;
	ZeroMemory(&di, sizeof(DOCINFO));
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = "打印报表";
	BOOL bPrintingOK = dc.StartDoc(&di);
	CPrintInfo Info;
	Info.SetMaxPage(1);
	int maxw = dc.GetDeviceCaps(HORZRES);
	int maxh = dc.GetDeviceCaps(VERTRES);
	Info.m_rectDraw.SetRect(0, 0, maxw, maxh);
	RECT tmpRect;
	GetClientRect(&tmpRect);
	HDC tmpDC;
	m_lpback->GetDC(&tmpDC);
	for(UINT page = Info.GetMinPage(); page<=Info.GetMaxPage()&&bPrintingOK; page++)
	{
		dc.StartPage();
		dc.SetStretchBltMode(COLORONCOLOR);
		::StretchBlt(dc.m_hDC, 0, 0, maxw, maxh, tmpDC, 0, 0, 794, 1123, SRCCOPY);
		bPrintingOK = (dc.EndPage() > 0);
	}
	m_lpback->ReleaseDC(tmpDC);
	if(bPrintingOK)dc.EndDoc();
	else dc.AbortDoc();
}

void CPrintfView::OnPrintSetting()
{
	CPrintDialog printDlg(TRUE);
	printDlg.DoModal();
}

void CPrintfView::OnSysCommand(UINT nID, LPARAM lParam)
{
	CFrameWnd::OnSysCommand(nID, lParam);
}

void CPrintfView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CFrameWnd::OnLButtonDblClk(nFlags, point);
}

void CPrintfView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(point.x > 10 && point.x < 120 && point.y > 10 && point.y < 40)
	OnPrint();
	else if(point.x > 10 && point.x < 120 && point.y > 50 && point.y < 80)
	OnPrintSetting();

	CFrameWnd::OnLButtonUp(nFlags, point);
}
