#pragma once

#include "MapEditDlg.h"

#define SAFE_DELETE(p)	if(p){delete p; p = NULL;}
#define SAFE_DELETE_ARRAY(p) if(p){delete[] p; p = NULL;}

#include "ddraw.h"

class CVehicleTrackingSystemDlg;

class CGetInternetMapDlg : public CDialog
{
	DECLARE_DYNAMIC(CGetInternetMapDlg)
public:
	CGetInternetMapDlg(CWnd* pParent = NULL);
	~CGetInternetMapDlg();
	enum {IDD = IDD_MAP_EDIT_DIALOG};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

protected:
	HICON m_hIcon;
	int m_iWaitInitTimes;

private:
	CMapFrameDlg* m_MapDlg;
	CWnd* m_pParent;
	DWORD m_dwDelay;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
};

