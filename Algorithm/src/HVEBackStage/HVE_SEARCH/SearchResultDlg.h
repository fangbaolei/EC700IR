#pragma once
#include "afxcmn.h"
#include "LoadingFrame.h"
#include <ddraw.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ddraw.lib")

// CSearchResultDlg dialog

class CSearchResultDlg : public CDialog
{
	DECLARE_DYNAMIC(CSearchResultDlg)

public:
	CSearchResultDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSearchResultDlg();

// Dialog Data
	enum { IDD = IDD_DIALOGSEACHRESULT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	CListCtrl m_listResult;
	int InitList(void);
	int AutoSize(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	bool m_bListInited;
 	LPDIRECTDRAW7			m_lpDDS;
	LPDIRECTDRAWSURFACE7	m_lpRPic;
	LPDIRECTDRAWSURFACE7	m_lpCPic;
	HFONT m_hFontComm;
	HFONT m_hFontMin;
	CString m_strRPicPath;
	CString m_strCPicPath;
	CString m_strServerIP;
	CString m_strServerUserName;
	CString m_strServerPassword;
	int m_iSelItem;
	BOOL m_bRoadNameIsExist;
	int m_iColCount;

 	CShowLoading* m_pShowLoading;

	int SetDefaultPicBuf(void);
	afx_msg void OnPaint();
	BOOL InitDx(void);
	int UpdatePicBuf(void);
	void DrawRectangle(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color1, COLORREF color2);
	void DrawTextThis(HDC hDC, int x, int y, LPCSTR pszText, int iTextLen, COLORREF color);

	afx_msg LRESULT OnLoadEnd(WPARAM wParam, LPARAM lParam);

	void LoadResultPic(int iIndex);
	
	static UINT LoadPicThread(LPVOID pParam);
	afx_msg void OnNMClickListresult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLvnKeydownListresult(NMHDR *pNMHDR, LRESULT *pResult);
	int GetPreItem(void);
	int GetNextItem(void);
	int ShowPicture(int iItem);
	int DrawNomalPicture(LPDIRECTDRAWSURFACE7 lpPic, CString strFilePath);
	int DrawErrorPicture(LPDIRECTDRAWSURFACE7 lpPic, CString strFilePath, CString strName);
};
