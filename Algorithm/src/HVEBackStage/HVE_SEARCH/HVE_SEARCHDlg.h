// HVE_SEARCHDlg.h : 头文件
//

#pragma once
#include "afxdtctl.h"
#include "afxwin.h"

#include "SearchResultDlg.h"
#include "LoadingFrame.h"

#include <map>

// CHVE_SEARCHDlg 对话框
class CHVE_SEARCHDlg : public CDialog
{
// 构造
public:
	CHVE_SEARCHDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HVE_SEARCH_DIALOG };

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
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	CString m_strIniFilePath;
	CString m_strServerIP;
	CString m_strDBName;
	CString m_strLoginName;
	CString m_strPassword;

	_ConnectionPtr m_pConnection;
	CString m_strViewName;
	CTime m_SearchStartTime;
	CTime m_SearchEndTime;
	CString m_strSearchCarNum;
	CString m_strSearchRoadName;
	CString m_strSearchLocationInfo;
	CString m_strSearchCarTypeInfo;
	CString m_strDirection;;
	std::map<CString, CString> m_mapLocation;
	std::map<CString, CString> m_mapCarType;

	CSearchResultDlg m_dlgResult;
	CShowLoading* m_pShowLoading;
	

	int InitConfig(void);
	int SaveConfig(void);
	afx_msg void OnBnClickedButtonconnect();
	afx_msg void OnBnClickedButtonsearch();
	int InitSearchCondition(void);
	CDateTimeCtrl m_StartTimeCtrl;
	CDateTimeCtrl m_EndTimeCtrl;
	CComboBox m_ComboLocation;
	CComboBox m_ComboCarType;
	CComboBox m_ComboRoadName;
	CComboBox m_ComboDirection;
	CString m_strAppPath;
	BOOL m_bRoadNameIsFind;

	int CreateDBView(void);
	int DropDBView(void);
	int QueryView(void);
	int ConnectDB(void);
	int DisConnectDB(void);
	BOOL IsTableExists(_ConnectionPtr& pConnect, CString strTableName);
	int GetLocationInfoFromDB(void);
	int GetCarTypeInfoFromDB(void);
	afx_msg void OnBnClickedButtondisconnect();
	int SearchCar(void);
	static UINT SearchThread(LPVOID pParam);
	afx_msg LRESULT OnSearchEnd(WPARAM wParam, LPARAM lParam);
	bool m_bIsSearching;
	CString LocationInfo2ID(CString strInfo);
	CString LocationID2Info(CString strID);
	CString CarTypeInfo2ID(CString strInfo);
	CString CarTypeID2Info(CString strID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int GetRoadNameFromDB(void);		
	BOOL DeleteDirectory(CString strDirPath);
	BOOL IsRoadNameExists(_ConnectionPtr& pConnect, CString strTableName);
	
};
