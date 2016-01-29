#pragma once

#include "resource.h"
#include "ddraw.h"

class CEditRoadLineDlg : public CDialog
{
	DECLARE_DYNAMIC(CEditRoadLineDlg);

public:
	CEditRoadLineDlg(CWnd* pParent = NULL);
	~CEditRoadLineDlg();
	enum {IDD = IDD_EDIT_ROAD_LINE};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	void UpDateLocaList(void);
	BOOL InitDX();
	static DWORD WINAPI MainThread(LPVOID lpParam);
	void GetPos(void);
	void PrepareOutPut(void);
	void DrawLine(HDC hDC, int x1, int y1, int x2, int y2, int iWidth, COLORREF color);
	void DrawEllsip(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color);
	void ShowPosInfo(HDC hDC, int x, int y, char* pszInfo, int iFrameWidth, COLORREF color1, COLORREF color2);
	void ShowStartPos(HDC hDC);
	void ShowEndPosList(HDC hDC);
	void ShowEditLine(HDC hDC);
	BOOL CheckIfCovertInEndPoint(POINT cPoint, int& iCurrentEndPos);
	BOOL CheckIfCoverPoint(POINT cPoint, int& iCurrentPoint);

private:
	CWnd* m_pParent;
	CListCtrl m_ListLoca;
	LPDIRECTDRAW7			m_lpdds;
	LPDIRECTDRAWSURFACE7	m_lpBack;
	HANDLE	m_hMainThread;
	BOOL	m_fExit;
	POINT	m_TmpPoint;
	int m_iCurrentIndex;
	int m_iCurrentEndPos;
	int m_iSelectPoint;
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMarkEndPoint();
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMarkSelectEndPoint();
	afx_msg void OnUnMarkEndPoint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
};
