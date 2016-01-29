#pragma once
#include "resource.h"
#include "atlbase.h"
#include "atlwin.h"
#include <afxdhtml.h> 

class CMapFrameDlg : public CDHtmlDialog
{
	DECLARE_DYNAMIC(CMapFrameDlg)
public:
	CMapFrameDlg(CWnd* pParent = NULL);
	~CMapFrameDlg();
	HRESULT ExecScript(CString strJavaScript);
	void ResizeContanner(int iWidth, int iHeight);
	void ReSetLoca(char* pszLoca);
	BOOL IsInited(void){ return m_fIsInited; }

	enum {IDD = IDD_HTML_DLG, IDH = IDR_HTML};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnNavigateComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()

protected:
	HICON m_hIcon;
	BOOL m_fIsFirstTime;
	DWORD m_dwDelay;
	BOOL m_fIsInited;

public:
	afx_msg void OnTimer(UINT nIDEvent);
};
