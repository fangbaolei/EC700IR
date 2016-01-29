#ifndef _LOADING_FRAME_H__
#define _LOADING_FRAME_H__

#include "resource.h"
#include "ddraw.h"

class CShowLoading : public CDialog
{
	DECLARE_DYNAMIC(CShowLoading);
public:
	CShowLoading(CWnd* pParent);
	~CShowLoading();
	enum {IDD = IDD_SET_LOCA_POINT_DLG};
	void CreateOwnerEx(void);
	void Start();
	void Stop();
	void SetInfoWord(LPCSTR pszInfo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	void InitDX(void);
	DECLARE_MESSAGE_MAP()
	void SafeStopThread(void);
	static DWORD WINAPI MainThread(LPVOID pParam);
	void PrepareShow(void);
	void DrawLine(HDC hDC, int x1, int y1, int x2, int y2, int iWidth, COLORREF color);
	void TransparentProce(void);
	afx_msg void OnCancel();
	afx_msg void OnOK();

private:
	LPDIRECTDRAW7				m_lpDDS;
	LPDIRECTDRAWSURFACE7		m_lpBack;
	LPDIRECTDRAWSURFACE7		m_lpParent;
	HANDLE						m_hThreadMain;
	BOOL						m_fExit;
	int							m_iPointIndex;
	CWnd*						m_pParent;
	CString						m_strInfo;
	CRITICAL_SECTION			m_csString;
};


#endif
