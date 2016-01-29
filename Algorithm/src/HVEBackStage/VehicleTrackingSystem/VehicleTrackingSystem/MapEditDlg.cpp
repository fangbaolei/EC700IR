#include "stdafx.h"
#include "MapEditDlg.h"
#include ".\mapeditdlg.h"

#define WM_STARTINTDLG	WM_USER + 1200

#define JSF_SetCotannerSize	"var mydiv = document.getElementById(\"%s\"); mydiv.style.width = %d; mydiv.style.height = %d;"

IMPLEMENT_DYNAMIC(CMapFrameDlg, CDHtmlDialog)

BEGIN_DHTML_EVENT_MAP(CMapFrameDlg)
END_DHTML_EVENT_MAP()

CComPtr<IHTMLWindow2> g_spHtmlWnd = NULL;
CComPtr<IHTMLDocument2> g_spHtmlDoc = NULL;

CMapFrameDlg::CMapFrameDlg(CWnd* pParent /* = NULL */)
: CDHtmlDialog(CMapFrameDlg::IDD, CMapFrameDlg::IDH, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_fIsFirstTime = TRUE;
	m_dwDelay = 0;
	m_fIsInited = FALSE;
}

CMapFrameDlg::~CMapFrameDlg()
{
	g_spHtmlDoc = NULL;
	g_spHtmlWnd = NULL;
}

void CMapFrameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMapFrameDlg, CDHtmlDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CMapFrameDlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	char szCurrentPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szCurrentPath, MAX_PATH);
	PathRemoveFileSpec(szCurrentPath);
	CString strHtmlLoca;
	strHtmlLoca.Format("%s/%s", szCurrentPath, "Frame.html");
	Navigate(strHtmlLoca.GetBuffer(), NULL, NULL, NULL, NULL);
	return TRUE;
}

void CMapFrameDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDHtmlDialog::OnSysCommand(nID, lParam);
}

BOOL CMapFrameDlg::PreTranslateMessage(MSG* pMsg)
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

void CMapFrameDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDHtmlDialog::OnPaint();
	}
}

HCURSOR CMapFrameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HRESULT CMapFrameDlg::ExecScript(CString strJavaScript)
{
	HRESULT hr;
	g_spHtmlDoc = this->m_spHtmlDoc;
	if(g_spHtmlWnd == NULL)
	{
		hr = g_spHtmlDoc->get_parentWindow(&g_spHtmlWnd);
	}
	CComBSTR bstrJavaScript = strJavaScript.AllocSysString();
	CComBSTR bstrLan = SysAllocString(L"javascript");
	VARIANT varRet;
	hr = g_spHtmlWnd->execScript(bstrJavaScript, bstrLan, &varRet);
	g_spHtmlWnd = NULL;
	g_spHtmlDoc = NULL;
	return hr;
}

BOOL CMapFrameDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	switch(message)
	{
	case WM_STARTINTDLG:
		break;
	}
	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

void CMapFrameDlg::OnNavigateComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	CDHtmlDialog::OnNavigateComplete(pDisp, szUrl);
	if(!m_fIsFirstTime)
	{
		SetTimer(1002, 1000, NULL);
	}
	else
	{
		m_fIsFirstTime = FALSE;
	}
}


void CMapFrameDlg::OnTimer(UINT nIDEvent)
{
	CDHtmlDialog::OnTimer(nIDEvent);
	if(nIDEvent == 1002)
	{
		if(m_dwDelay <= 3)
		{
			m_dwDelay++;
		}
		else
		{
			KillTimer(1002);
			m_dwDelay = 0;
			RECT cRect;
			GetClientRect(&cRect);
			CString strSetCotannerSize;
			strSetCotannerSize.Format(JSF_SetCotannerSize, "divMap", cRect.right-cRect.left-20,
				cRect.bottom-cRect.top-20);
			if(FAILED(ExecScript(strSetCotannerSize.GetBuffer())))
			{
				m_fIsInited = FALSE;
				return;
			}
			CString strInitMap = "var map = new BMap.Map(\"divMap\");";
			if(FAILED(ExecScript(strInitMap.GetBuffer())))
			{
				m_fIsInited = FALSE;
				return;
			}
			CString strLoca = "var city = new BMap.LocalSearch(map, {renderOptions:{map:map,autoViewport:true}});var c=\"杭州\";city.search(c)";
			if(FAILED(ExecScript(strLoca.GetBuffer())))
			{
				m_fIsInited = FALSE;
				return;
			}
			strLoca.Format("%s", "var NavigationControl = new BMap.NavigationControl(BMAP_NAVIGATION_CONTROL_LARGE);map.addControl(NavigationControl);  \
map.enableScrollWheelZoom(); map.enableContinuousZoom(); map.enableInertialDragging();");
			if(FAILED(ExecScript(strLoca.GetBuffer())))
			{
				m_fIsInited = FALSE;
				return;
			}
			m_fIsInited = TRUE;
		}
	}
}

void CMapFrameDlg::ResizeContanner(int iWidth, int iHeight)
{
	CString strResetContannerSize;
	strResetContannerSize.Format(JSF_SetCotannerSize, "divMap", iWidth, iHeight);
	ExecScript(strResetContannerSize.GetBuffer());
}

void CMapFrameDlg::ReSetLoca(char* pszLoca)
{
	if(pszLoca == NULL)
	{
		return;
	}
	CString strReLoca;
	strReLoca.Format("var city = new BMap.LocalSearch(map, {renderOptions:{map:map,autoViewport:true}});var c=\"%s\";city.search(c)", pszLoca);
	ExecScript(strReLoca.GetBuffer());
}