#pragma once

#include "resource.h"

typedef struct _IPInfo
{
	DWORD32 dwIP;
	DWORD32 dwMask;
	DWORD32 dwGateWay;
	DWORD64	dw64Mac;
}IPInfo;

class CIPEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CIPEditDlg)
public:
	CIPEditDlg(IPInfo* pIpInfp, CWnd* pParent = NULL);
	virtual ~CIPEditDlg();
	enum {IDD = IDD_DIALOG_SETIP};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	IPInfo* m_pIPInfo;
	CIPAddressCtrl m_IP;
	CIPAddressCtrl m_Mask;
	CIPAddressCtrl m_GateWay;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
