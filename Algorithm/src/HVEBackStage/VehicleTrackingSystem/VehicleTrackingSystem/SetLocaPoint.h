#pragma once

#include "resource.h"
#include "ddraw.h"

class CVehicleTrackingSystemDlg;

class CSetLocaPointDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetLocaPointDlg);

public:
	CSetLocaPointDlg(LPPOINT pPoint, CWnd* pParent = NULL);
	~CSetLocaPointDlg();
	enum {IDD = IDD_SET_LOCA_POINT_DLG};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	BOOL InitDX();
	static DWORD WINAPI MainThread(LPVOID pParam);
	void PrepareOutPut(void);
	void DrawLine(HDC hDC, int x1, int y1, int x2, int y2, int iWidth, COLORREF color);
	void DrawEllsip(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color);
	void DrawRoundRect(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color, COLORREF color1);

private:
	LPDIRECTDRAW7			m_lpdds;
	LPDIRECTDRAWSURFACE7	m_lpOffScreen;
	HANDLE					m_hThreadMain;
	BOOL					m_fExit;

public:
	LPPOINT m_Point;
	POINT	m_TmpPoint;
	CVehicleTrackingSystemDlg* m_pParent;
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCancel();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
