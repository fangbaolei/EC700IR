#pragma once
#include "afxdtctl.h"
#include "afxwin.h"

typedef struct _DEV_CONFIG
{
	bool fEnableRecvHistory;
	char szBeginTime[64];
	char szEndTime[64];
	int nIndex ;
	bool fOnlyViolation;

	_DEV_CONFIG()
		: fEnableRecvHistory(FALSE)
		, nIndex(0)
		, fOnlyViolation(FALSE)
	{
		ZeroMemory(szBeginTime , sizeof(szBeginTime));
		ZeroMemory(szEndTime , sizeof(szEndTime));

	}

}DEV_CONFIG;

HRESULT ReadConfig(char* pSn , DEV_CONFIG* pDevConfig);
HRESULT WriteConfig(char* pSN , DEV_CONFIG* pDevConfig);


// AutoLinkParam dialog


class AutoLinkParam : public CDialog
{
	DECLARE_DYNAMIC(AutoLinkParam)

public:
	AutoLinkParam(CWnd* pParent = NULL);   // standard constructor
	virtual ~AutoLinkParam();

	HRESULT InitParamDialog(char* pSN ,DEV_CONFIG* pDevConfig ,bool * fSetParam);

// Dialog Data
	enum { IDD = IDD_AUTOLINK_SETHISTORY };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	char m_szDevSN[128];
	DEV_CONFIG* m_pDevConfig;
	bool * m_fSetParam;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSaveButton();
	CDateTimeCtrl m_beginTimeCtrl;
	CButton m_enableRecvHistoryBut;
	CEdit m_indexEdit;
	CButton m_enableEndTimeBut;
	CDateTimeCtrl m_endTimeCtrl;
	afx_msg void OnBnClickedCancelButton();
	CButton m_onlyViolationBut;
	afx_msg void OnEnableHistory();
	afx_msg void OnEnableEndTime();
};
