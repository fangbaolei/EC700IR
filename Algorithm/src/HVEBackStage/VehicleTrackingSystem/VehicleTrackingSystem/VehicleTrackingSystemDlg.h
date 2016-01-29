#pragma once
#include "ddraw.h"
#include "afxtempl.h"
#include "AddNewSever.h"
#include "ResultView.h"
#include "afxwin.h"

#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename ("EOF", "adoEOF")

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ddraw.lib")

typedef struct _ResultInfoType
{
	char szPlate[30];
	char szPassTime[50];
	char szRPicPath[256];
	char szCPicPath[256];
	int iLocaIndex;
	char szServerIP[30];
	char szDataBaseName[32];
	_ResultInfoType()
	{
		memset(szPlate, 0, 30);
		memset(szPassTime, 0, 50);
		memset(szRPicPath, 0, 256);
		memset(szCPicPath, 0, 256);
		iLocaIndex = -1;
		memset(szServerIP, 0, 30);
		memset(szDataBaseName, 0, 32);
	}

}ResultInfoType;

class CSearchCarPassLocaType
{
public:
	CSearchCarPassLocaType(NewSeverType* pServer, char* pszSQL, BOOL fIsSetTime, CTime cStartTime, CTime cEndTime);
	~CSearchCarPassLocaType();
	void Start();
	int GetStatus();

protected:
	static DWORD WINAPI ProcessThread(LPVOID lpParam);
	void ClearLastResult(void);

private:
	NewSeverType m_Server;
	int m_Status;
	HANDLE m_hProcess;
	char* m_pszSQL;
	BOOL m_fIsSetTime;
	CTime m_cStartTime;
	CTime m_cEndTime;

public:
	CList<ResultInfoType*> m_ResultList;
	CRITICAL_SECTION m_csResultList;
};

typedef struct _EndLocaType
{
	LocaType* pLoca;
	CList<POINT*> pRoadLinePointList;
	CRITICAL_SECTION csPointList;
	_EndLocaType()
	{
		pLoca = NULL;
		pRoadLinePointList.RemoveAll();
		InitializeCriticalSection(&csPointList);
	}
	~_EndLocaType()
	{
		pLoca = NULL;
		EnterCriticalSection(&csPointList);
		while(pRoadLinePointList.GetCount() > 0)
		{
			POINT* pTmpPoint = pRoadLinePointList.RemoveHead();
			delete pTmpPoint;
			pTmpPoint = NULL;
		}
		LeaveCriticalSection(&csPointList);
	}
}EndLocaType;

typedef struct _LocaRoadLineType
{
	LocaType* pLoca;
	CList<EndLocaType*>	pEndPosType;
	CRITICAL_SECTION csEndList;
	_LocaRoadLineType()
	{
		pLoca = NULL;
		pEndPosType.RemoveAll();
		InitializeCriticalSection(&csEndList);
	}
	~_LocaRoadLineType()
	{
		pLoca = NULL;
		EnterCriticalSection(&csEndList);
		while(pEndPosType.GetCount() > 0)
		{
			EndLocaType* pTmpEndLoca = pEndPosType.RemoveHead();
			delete pTmpEndLoca;
			pTmpEndLoca = NULL;
		}
		LeaveCriticalSection(&csEndList);
	}
}LocaRoadLineType;

class CVehicleTrackingSystemDlg : public CDialog
{
public:
	CVehicleTrackingSystemDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_VEHICLETRACKINGSYSTEM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	BOOL OnInitDX(void);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void GetLocaInfo(INT iSeverIndex);
	BOOL AddLocaInfo(LocaType* pAddLoca);
	void UpDataListLoca(void);
	BOOL AddServerInfo(NewSeverType* pAddServer);
	void UpDataListServer(void);
	void ClearSencer(void);
	void PrepareOutPut(void);
	void DrawLine(HDC hDC, int x1, int y1, int x2, int y2, int iWidth, COLORREF color);
	void DrawEllsip(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color);
	void DrawRectangle(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color1, COLORREF color2);
	void ShowPosInfo(HDC hDC, int x, int y, char* pszInfo, int iFrameWidth, COLORREF color1, COLORREF color2);
	void DrawTextThis(HDC hDC, int x, int y, LPCSTR pszText, int iTextLen, COLORREF color);
	void WindowsPointToBack(int x1, int y1, CRect* pcRect, int& iRx, int& iRy);
	void UpdateSearchInfo(void);
	BOOL UpDataCarType(LocaType* pLocaType);
	void SetDefualtPicBuffer(void);
	void UpDatePicBuffer(void);
	static DWORD WINAPI SearchPassCarThread(LPVOID lpParam);
	static DWORD WINAPI GetLocaInfoThread(LPVOID lpParam);
	int GetLocaIndex(LocaType* pLoca);
	void ClearSearchClass(void);
	void ProceSearchDone(void);
	void CleanFinalResult(void);
	int GetResultLoca(ResultInfoType* pResultInfo);
	void SortFinalResult(void);
	void UpDataFinalResultList(void);
	void ShowResultInfo(HDC hDC);
	void ProceSearch1Done(int iRetType);
	void ClearTmpResultPic(char* sDirName);
	void LoadResultPic(int iIndex);
	void LoadFinalResultPic(FinalResultType* pTmpFinalResult);
	static DWORD WINAPI LoadResultPicThread(LPVOID lParam);
	void UpDateFinalResultPic(void);
	void CopyResultPicToSavePath(char* pszFilePathName, char* pszSavePath, CString& strSavePathName);

	void OnEditServerComm(void);
	void OnDeleteServerComm(void);
	void ProcGetLocaInfoDone(int iRetType);
	void SetDefaultSize(void);
	void ConventerRect(RECT& srcRect, RECT& desRect, float fRatioX, float fRatioY);
	void SetMaxSize(void);
	void ReFreshFinalREsultLocaPoint(LocaType* pLoca);

public:
	LPDIRECTDRAW7			m_lpDDS;
	LPDIRECTDRAWSURFACE7	m_lpBack;
	LPDIRECTDRAWSURFACE7	m_lpOffScreen;
	LPDIRECTDRAWSURFACE7	m_lpRPic;
	LPDIRECTDRAWSURFACE7	m_lpCPic;
	CListCtrl				m_ListSever;
	CListCtrl				m_ListLoca;
	CList<LocaRoadLineType*>	m_LocaRoadLineType;
	CRITICAL_SECTION			m_csLocaRoadLine;
	CList<FinalResultType*>	m_FinalResultList;
	CRITICAL_SECTION		m_csFinalResultList;
	CList<LocaType*>		m_LocaList;
	CRITICAL_SECTION		m_csLocaList;
	int						m_iSelectIndex;
	int						m_iStartResultIndex;
	int						m_iEndResultIndex;	
	BOOL					m_fIsUpdateInternetMap;

protected:
	_ConnectionPtr			m_pConnection;
	_RecordsetPtr			m_pRecordSet;
	CString					m_LastConnectServerIP;
	CList<NewSeverType*>	m_ServerList;
	CRITICAL_SECTION		m_csServerList;
	CDateTimeCtrl			m_DateStart;
	CDateTimeCtrl			m_DateEnd;
	CComboBox				m_ComBoxCarType;
	CDateTimeCtrl			m_SearchCarStartTime;
	CDateTimeCtrl			m_SearchCarEndTime;
	CListBox				m_SelectServer;
	CComboBox				m_ComBoxServer;
	CListCtrl				m_ListResult;
	HANDLE					m_hSearchPassCarThread;
	BOOL					m_fIsBreak;
	HANDLE					m_hSearchLocaThread;
	BOOL					m_fIsCancelGetLocaInfo;
	int						m_iTempServerIndex;
	HANDLE					m_hLoadResultPic;
	BOOL					m_fIsLoadPicBreak;
	int						m_iResultType;
	int						m_iSelectResultIndex;
	int						m_iTmpSelectIndex;
	BOOL					m_fIsInit;
	BOOL					m_fIsUpDataLoca;

	CList<CSearchCarPassLocaType*> m_SearchClassList;
	CRITICAL_SECTION		m_csSearchClassList;

	CString	m_strRPicPath;
	CString m_strCPicPath;
	CToolTipCtrl m_Message;

	CString m_strServerName;
	CString m_strServerPassword;

public:
	afx_msg void OnLoadFile();
	afx_msg void OnLoadMapFromInterNet();
	afx_msg void OnLoadMap();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSave();
	afx_msg void OnClearSencer();
	afx_msg void OnNMRdblclkList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnEditRoadLine();
	afx_msg void OnNMDblclkList4(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheck7();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedCheck6();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnViewResult();
	afx_msg void OnNMRclickList4(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSetStart();
	afx_msg void OnSetEnd();
	afx_msg void OnUnSetStart();
	afx_msg void OnUnSetEnd();
	afx_msg void OnSaveResult();
	afx_msg void OnViewSavedResult();
	afx_msg void OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOpenDemo();
	afx_msg void OnBnClickedCheck8();
	afx_msg void On32782();
	afx_msg void OnSyncSearch(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPrevResult(void);
	afx_msg void OnNextResult(void);
	afx_msg void OnLvnKeydownList4(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickList4(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_comboDir1;
	CComboBox m_comboDir2;
//	afx_msg void OnNMKillfocusList2(NMHDR *pNMHDR, LRESULT *pResult);
};
