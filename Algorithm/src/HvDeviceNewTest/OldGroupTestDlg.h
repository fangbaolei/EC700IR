#ifndef _OLD_GROUP_TEST_H__
#define _OLD_GROUP_TEST_H__

#include "resource.h"
#include "HvDevice.h"
#include "HvVarType.h"

using namespace HiVideo;

typedef struct _GROUP_RESULT_DATA
{
	DWORD	dwDataType;
	DWORD	dwDataLen;
	DWORD	dwInfoLen;
	PBYTE	pData;
	char*	pszPlateInfo;

	_GROUP_RESULT_DATA()
	{
		pData = NULL;
		pszPlateInfo = NULL;
	}

}GROUP_RESULT_DATA;

class COldTestType
{
public:
	COldTestType(char* szIP, CListCtrl* pListBox, DWORD dwListIndex, CString strSavePath);
	~COldTestType();

private:
	static DWORD WINAPI ProceThread(LPVOID pParam);
	static DWORD WINAPI ProceResultDataThread(LPVOID pParam);
	static int OnRecord(PVOID pUserData,
		PBYTE pbResultPacket,
		DWORD dwPacketLen,
		DWORD dwRecordType,
		LPCSTR szResultInfo);

private:
	HVAPI_HANDLE	m_hDevice;
	HANDLE	m_hProceThread;
	HANDLE	m_hProceResultDataThread;
	DWORD   m_dwStatus;
	char	m_szIP[20];
	BOOL	m_fIsThreadExit;
	BOOL	m_fIsProceResultThreadExit;
	CListCtrl* m_ListBox;
	DWORD m_dwIndex;
	CString m_strSavePath;
	BOOL m_fIsCanSave;
	CHvList<GROUP_RESULT_DATA>	m_ResultList;
	CRITICAL_SECTION	m_csList;
	DWORD m_dwLastStatus;
	DWORD m_dwReconnectTimes;
};

class COldGroupTestDlg : public CDialog
{
	DECLARE_DYNAMIC(COldGroupTestDlg)
public:
	COldGroupTestDlg(CWnd* pParent = NULL);
	virtual ~COldGroupTestDlg();
	enum {IDD = IDD_DIALOG8};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	DECLARE_MESSAGE_MAP()

private:
	HICON	m_hIcon;
	CWnd* m_pParentDlg;
	CIPAddressCtrl m_IPControll;
	CListBox m_ListBox;
	CListCtrl m_ListControll;
	CString m_SaveFilePath;
	CToolTipCtrl m_Messge;
	COldTestType* m_TestType[20];
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton8();

protected:
	virtual void OnOK();

};

#endif
