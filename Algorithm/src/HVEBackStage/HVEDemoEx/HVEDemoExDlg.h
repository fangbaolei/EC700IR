// HVEDemoExDlg.h : 头文件
//

#pragma once
//#include "AxLib.h"
#include "afxtempl.h"
#include "atlstr.h"

#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename ("EOF", "adoEOF")
//using namespace dbAx;

class CQuery;
class CSystemSettingDlg;
#define  WM_QURE_QUIT	WM_USER+130
#define  WM_SET_ONOK	WM_USER+131
#define	 WM_SET_ONCACAL	WM_USER+132

typedef struct _ROAD_INFO
{
	int nRoad_No;				//车道号
	CString strHve_Addr;		//覆盖该车道HVE设备IP地址
	int nLimit_Speed;			//该车道的限速值

	_ROAD_INFO()
	{
		nRoad_No = 0;
		strHve_Addr.Empty();
		nLimit_Speed = 0;
	};

	_ROAD_INFO& operator=(const _ROAD_INFO& src)
	{
		nRoad_No = src.nRoad_No;
		strHve_Addr = src.strHve_Addr;
		nLimit_Speed = src.nLimit_Speed;

		return *this;
	}
}
ROAD_INFO;

typedef struct _LOCATION_INFO
{
	int nLocation_Id;				//路段ID
	CString strLocation_Name;		//路段描述

	int nSite_Distance;				//该路段路程，单位km 
	int nRoad_Able_Adapt;			//该路段适应交通量

	CArray<ROAD_INFO>	rgRoadInfo;
}LOCATION_INFO;

typedef struct QureStr_Ptr
{
	int Count_1, Count_2, Count_3, Count_4;
	int Speed_1, Speed_2, Speed_3, Speed_4;
	double Cross_1, Cross_2, Cross_3, Cross_4;
}QureStr;

typedef struct SettingDataStr_Ptr
{
	DWORD dwIP;
	CString strDataBaseName;
	CString	strUserName;
	CString strPassWord;
}SettingDataStr;

// CHVEDemoExDlg 对话框
class CHVEDemoExDlg : public CDialog
{
// 构造
public:
	CHVEDemoExDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HVEDEMOEX_DIALOG };
	enum { MAX_LOCATION = 32};

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCancel();
	afx_msg void OnOK();
	DECLARE_MESSAGE_MAP()

protected:
	CListCtrl			m_listRoadInfo;
	CListCtrl			m_listReport;

	CStatic				m_staticMinRoad1CarNum;
	CStatic				m_staticMinRoad1AvrSpeed;
	CStatic				m_staticMinRoad1Avr;
	CStatic				m_staticMinRoad1Statu;

	CStatic				m_staticMinRoad2CarNum;
	CStatic				m_staticMinRoad2AvrSpeed;
	CStatic				m_staticMinRoad2Avr;
	CStatic				m_staticMinRoad2Statu;

	CStatic				m_staticMinRoad3CarNum;
	CStatic				m_staticMinRoad3AvrSpeed;
	CStatic				m_staticMinRoad3Avr;
	CStatic				m_staticMinRoad3Statu;

	CStatic				m_staticMinRoad4CarNum;
	CStatic				m_staticMinRoad4AvrSpeed;
	CStatic				m_staticMinRoad4Avr;
	CStatic				m_staticMinRoad4Statu;

	CStatic				m_staticHourRoad1CarNum;
	CStatic				m_staticHourRoad1AvrSpeed;
	CStatic				m_staticHourRoad1Avr;
	CStatic				m_staticHourRoad1Statu;

	CStatic				m_staticHourRoad2CarNum;
	CStatic				m_staticHourRoad2AvrSpeed;
	CStatic				m_staticHourRoad2Avr;
	CStatic				m_staticHourRoad2Statu;

	CStatic				m_staticHourRoad3CarNum;
	CStatic				m_staticHourRoad3AvrSpeed;
	CStatic				m_staticHourRoad3Avr;
	CStatic				m_staticHourRoad3Statu;

	CStatic				m_staticHourRoad4CarNum;
	CStatic				m_staticHourRoad4AvrSpeed;
	CStatic				m_staticHourRoad4Avr;
	CStatic				m_staticHourRoad4Statu;

	CStatic				m_staticCurrentStatue;
	CComboBox			m_ComBoxRoad;

	BOOL				m_fIsConnect;
	DWORD				m_strSourcePath;
	CWinThread*			m_updataThread;

	_ConnectionPtr		m_pConnection;
	_RecordsetPtr		pRenRecordset;
	BOOL				m_fIsADOConnected;
	BOOL				m_fIsNewInfo;
	BOOL				m_fIsReverse;

public:
	afx_msg void OnStnClickedStatic1();
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	BOOL DoConnectDataBase(CString strConnect);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL		GetConnectStatus(void){ return m_fIsADOConnected; }
	DWORD		GetSourcePath(void){ return m_strSourcePath; }
	void		SetSourcePath(DWORD dwNewPath) {m_strSourcePath = dwNewPath;}
	CString		GetDataBaseName(void){ return m_strDataBaseName; }
	CString		GetUserName(void){ return m_strUserName; }
	CString		GetPassWord(void){ return m_strPassWord; }
	BOOL		QueryLocation(void);
	LOCATION_INFO* GetLocationInfoByID(int ID);
	BOOL		Starting(void);
	BOOL		Stop(void);
	static UINT	UpDataThread(LPVOID userData);
	void		UpdataMinInfo(COleDateTime& startTime, COleDateTime& endTime);
	void		UpdataHourInfo(COleDateTime& startTime, COleDateTime& endTime);
	void		ShowRoadInfo(void);
	void		InitReportList(void);
	void		ShowCurrentCarNum(void);
	BOOL		DisConnect(void);
	BOOL		GetCountByTime(CString strStartTime, CString strEndTime, QureStr& stResult);
	BOOL		GetCarCountByTime(CString strStartTime, CString strEndTime, QureStr& stResult);
	afx_msg LRESULT OnQueryQuit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetOk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetCancel(WPARAM wParam, LPARAM lParam);
	void GetIniFileInfo(void);
	void SaveIniInfo(void);
	DWORD	IPStr2DWord(char* szIP);
	void	AddLocaTionToCombox(void);
	BOOL PreTranslateMessage(MSG* pMsg);
	BOOL CreateView(COleDateTime& startTime, COleDateTime& endTime);
	void DropView(void);
	BOOL IsTableExists(CString strTableName);

public:
	LOCATION_INFO	m_rgLocation[MAX_LOCATION];
	int				m_iLocationCount;
	int				m_nCurrentRoadID;
	BOOL			m_fIsRunning;
	int				m_nCurrentHour;
	int				m_ShowCurrentHourDelay;
	CString			m_strDataBaseName;
	CString			m_strUserName;
	CString			m_strPassWord;
	SettingDataStr	m_rgSettingInfo;
//	CAxConnection	m_cAxConn;
//	CAxRecordset	m_cRec;

	CQuery*			m_QueryDlg;
	BOOL			m_fIsShowQuery;
	CSystemSettingDlg* m_SetDlg;
	BOOL			m_fIsShowSetDlg;
	CString			m_strIniFilePath;
	BOOL			m_fIsGetMiniDataFaile;
	BOOL			m_fIsGetHourDataFaile;
	BOOL			m_fIsGetReportFaile;
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedCheck1();
};
