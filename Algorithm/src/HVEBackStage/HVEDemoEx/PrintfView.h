#pragma once
#include "afxwin.h"
#include "QueryDlg.h"
#include "ddraw.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ddraw.lib")

class CPrintfView :
	public CFrameWnd
{
public:
	CPrintfView(CQuery* pParent);
	~CPrintfView(void);
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
private:
	CQuery* m_ParentFrame;
	LPDIRECTDRAW7			m_lpdds;
	LPDIRECTDRAWSURFACE7	m_lpback;

	CDC m_dcChart;
	int m_iWidth;
	int m_iHeight;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	BOOL GetPrintfInfo(void);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	void OnPrint();
	void OnPrintSetting();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
