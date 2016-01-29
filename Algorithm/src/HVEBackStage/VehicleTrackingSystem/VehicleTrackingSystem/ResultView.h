#pragma once
#include "afxtempl.h"
#include "resource.h"
#include "ddraw.h"

typedef struct _FinalResultType
{
	char szPlate[30];
	char szPassTime[50];
	char szRPicPath[256];
	char szCPicPath[256];
	char szLocaName[256];
	char szServerIP[30];
	POINT Loca;
	_FinalResultType()
	{
		memset(szPlate, 0, 30);
		memset(szPassTime, 0, 50);
		memset(szRPicPath, 0, 256);
		memset(szCPicPath, 0, 256);
		memset(szLocaName, 0, 256);
		memset(szServerIP, 0, 30);
		Loca.x = 0;
		Loca.y = 0;
	}
}FinalResultType;

typedef struct _LocaType
{
	int		iLocaID;
	char	szLocaName[256];
	char    szServerName[256];
	char	szServerIP[30];
	char	szDataBaseName[32];
	char	szUserName[32];
	char	szPassWork[16];
	int		iPositionX;
	int		iPositionY;
	_LocaType()
	{
		iLocaID = -1;
		memset(szLocaName, 0, 256);
		memset(szServerName, 0, 256);
		memset(szServerIP, 0, 30);
		memset(szDataBaseName, 0, 32);
		memset(szUserName, 0, 32);
		memset(szPassWork, 0, 16);
		iPositionX = -100;
		iPositionY = -100;
	}
}LocaType;

class CResultViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CResultViewDlg);

public:
	CResultViewDlg(CWnd* pParent = NULL);
	CResultViewDlg(LPCSTR lpszResultFilePath, CWnd* pParent = NULL);
	~CResultViewDlg();
	enum {IDD = IDD_DIALOG_RESULT_VIEW};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	BOOL InitDX();
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	static DWORD WINAPI MainThread(LPVOID pParam);
	void GetPos(void);
	void PrepareOutPut(void);
	void CopyFinalResultList(void);
	void LoadFinalResultList(void);
	void UpDateResultList(void);
	void DrawPlate(void);
	void DrawRect(HDC hDC, int x, int y, int x1, int y1, COLORREF color);
	void DrawText(HDC hDC, int x, int y, HFONT hFont, 
		char* pszText, int iLen, COLORREF corlor);
	void DrawRectangle(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color1, COLORREF color2);
	void DrawTextThis(HDC hDC, int x, int y, LPCSTR pszText, int iTextLen, COLORREF color);
	void DrawEllsip(HDC hDC, int x, int y, int x1, int y1, COLORREF color1, COLORREF corlor2);
	void DrawLine(HDC hDC, int x1, int y1, int x2, int y2, int iWidth, COLORREF color);
	void ShowCurrentResultInfo(HDC hDC);
	void ShowPosInfo(HDC hDC, int x, int y, char* pszInfo, int iFrameWidth, COLORREF color1, COLORREF color2);
	void UpDateResultPic(void);
	void OnPrevResult(void);
	void OnNextResult(void);

private:
	LPDIRECTDRAW7			m_lpDDS;
	LPDIRECTDRAWSURFACE7	m_lpOffScreen;
	LPDIRECTDRAWSURFACE7	m_lpBack;
	LPDIRECTDRAWSURFACE7	m_lpRPic;
	LPDIRECTDRAWSURFACE7	m_lpCPic;
	LPDIRECTDRAWSURFACE7	m_lpPlate;

	CWnd* m_pParent;
	CList<FinalResultType*>	m_FinalResultList;
	CRITICAL_SECTION m_csFinalResult;
	CList<LocaType*> m_LocaList;
	CRITICAL_SECTION m_csLocaList;
	char m_szFinalResultFilePath[256];
	BOOL m_fLoadFromFile;
	CListCtrl m_ListFinalResult;
	int m_iCurrentSelect;
	BOOL m_fExit;
	HANDLE m_hThreadMain;
	HFONT m_MainFont;
	int m_iDelay;
	POINT	m_TmpPoint;
	BOOL	m_fIsNeedUpDatePicBuffer;

	int m_iResultCount;
	int m_iResultDelay;
	CString m_strRPicPath;
	CString m_strCPicPath;
public:
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpdatePicBuffer(void);
	afx_msg void OnLvnKeydownList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
