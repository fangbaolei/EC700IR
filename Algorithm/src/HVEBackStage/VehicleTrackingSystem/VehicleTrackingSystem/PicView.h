#pragma once

#include "resource.h"
#include "ddraw.h"

class CPicView : public CDialog
{
	DECLARE_DYNAMIC(CPicView);
public:
	CPicView(LPDIRECTDRAWSURFACE7 lpSrcPic, CWnd* pParent = NULL);
	~CPicView();
	enum {IDD = IDD_DIALOG_PICVIEW};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	void InitDX();
	DECLARE_MESSAGE_MAP()
	void SafeStopThread(void);
	static DWORD WINAPI MainThread(LPVOID lpParam);
	void PrepareShow(void);

private:
	LPDIRECTDRAW7			m_lpdds;
	LPDIRECTDRAWSURFACE7	m_lpSrcPic;
	LPDIRECTDRAWSURFACE7	m_lpOffScreen;
	HANDLE					m_hThreadMain;
	BOOL					m_fExit;
	int						m_iWidth;
	int						m_iHeight;
	int						m_iRealWidth;
	int						m_iRealHeight;
public:
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
};
