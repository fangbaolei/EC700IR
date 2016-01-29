#pragma once
#include "afxwin.h"
#include "resource.h"
#include "HVEDemoExDlg.h"

#define  WM_SEARCHFINISH	WM_USER + 120
#define	 WM_DLGENDING		WM_USER + 121
#define  WM_VIEWQUIT		WM_USER + 122

class CPrintfView;
#include "ddraw.h"
#include ".\mschart\mschart.h"

typedef struct QueryStruc_Str
{
	int CarCount_1, CarCount_2, CarCount_3, CarCount_4;
	int CarSpeed_1, CarSpeed_2, CarSpeed_3, CarSpeed_4;
	double Cross_1, Cross_2, Cross_3, Cross_4;
}QueryStruc;

class CQuery : public CDialog
{
	DECLARE_DYNAMIC(CQuery)
public:
	CQuery(CWnd* pParent = NULL);
	enum {IDD = IDD_DIALOGQUERY};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	
public:
	CHVEDemoExDlg*	m_ParentFarm;
	CListCtrl		m_ListReport;
	CDateTimeCtrl	m_Data;
	CComboBox		m_MothYear;
	CComboBox		m_MothMoth;
	CComboBox		m_Year;
	CProgressCtrl	m_Process;
	CButton			m_ButtonQuery;
	CButton			m_ButtonPrint;
	CStatic			m_StaticReportType;
	CStatic			m_StaticReportTimes;
	CButton			m_ButtonReset;
	CButton			m_ButtonPrintView;
	CButton			m_ButtomPrintSet;
	CEdit			m_EditSun;
	CEdit			m_EditSpeed;
	CEdit			m_EditCross;
	CWinThread*		m_GetDateThread;
	CWinThread*		m_GetDateProcessThread;
	BOOL			m_fIsGetDate;
	BOOL			m_fIsProcsee;
	CPrintfView*	m_PrintfViewDlg;
	int				m_nReportType;
	_ConnectionPtr	m_pConnect;
	_RecordsetPtr	pRenRecordset;
	BOOL			m_fIsADOConnected;
	BOOL			m_fIsQuit;
	BOOL			m_fIsReverse;

	LPDIRECTDRAW7			m_lpdds;
	LPDIRECTDRAWSURFACE7	m_lpback;

	CBrush* m_pBrushChart;
	CFont m_fontChart;

public:
	afx_msg void OnBnClickedButton1();
	static UINT GetDayDataThread(LPVOID userData);
	static UINT	GetDayProcessThread(LPVOID userData);
	static UINT GetMothDataThread(LPVOID userData);
	static UINT GetYearDataThread(LPVOID userData);
	int GetMothDayCount(int year, int moth);
	afx_msg void OnCancel();
	afx_msg void OnOK();
	afx_msg LRESULT OnSearchDone(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnViewQuit(WPARAM wParam, LPARAM lParam);
	BOOL GetCountByTime(CString StartTime, CString EndTime, QueryStruc& res);
	BOOL GetTotleCountByTime(CString StartTime, CString EndTime, int& CarCount, int& CarSpeed, double& Cross);
	BOOL ConnectDataBase(void);
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton3();
	BOOL GetPrintInfo(void);
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnNMRclickList3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOutPutResultToExecl(void);
	BOOL CreateView(COleDateTime& startTime, COleDateTime& endTime);
	void DropView(void);
	BOOL IsTableExists(CString strTableName);
	BOOL IsTableLikeThisExists(CString strTableName);
	int ShowCarCountPerDay(bool bShow);
	CMSChart m_Chart;
	int InitChart(void);
	int SetChartData(short nRow, double dblCarNum1, double dblCarNum2, double dblCarNum3, double dblCarNum4, double dblCarCount);
	DECLARE_EVENTSINK_MAP()
//	void DblClickMschart1();
	int InitChartData(short nRowCount);

public:
	int GetChartDC(CDC* pDc, int& iWidth, int& iHeight);
	BOOL m_bShowChart;
	afx_msg void OnBnClickedCheckshowchart();
};
