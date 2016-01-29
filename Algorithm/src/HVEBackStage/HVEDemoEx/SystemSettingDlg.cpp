#include "stdafx.h"
#include "SystemSettingDlg.h"
#include ".\systemsettingdlg.h"

IMPLEMENT_DYNAMIC(CSystemSettingDlg, CDialog)

CSystemSettingDlg::CSystemSettingDlg(CWnd* pParent /* = NULL */)
: CDialog(CSystemSettingDlg::IDD, pParent)
{
	m_ParentFrame = (CHVEDemoExDlg*)pParent;
}

void CSystemSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPControl);
	DDX_Control(pDX, IDC_EDIT1, m_EditDataBaseName);
	DDX_Control(pDX, IDC_EDIT2, m_EditUserName);
	DDX_Control(pDX, IDC_EDIT3, m_EditPassWord);
//	DDX_Control(pDX, IDC_STATICSTATUE, m_StaticCurrentStatue);
	DDX_Control(pDX, IDC_BUTTON1, m_ButtonConnect);
	DDX_Control(pDX, IDC_BUTTON2, m_ButtonDisConnect);
}

BEGIN_MESSAGE_MAP(CSystemSettingDlg, CDialog)
	
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
//	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
//	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
//	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
END_MESSAGE_MAP()

//void CSystemSettingDlg::SetComBo()
//{
//	if(m_ParentFrame == NULL) return;
//	for(int index=0; index<m_ParentFrame->m_iLocationCount; index++)
//	{
//		m_ComBoxRoadName.InsertString(index, m_ParentFrame->m_rgLocation[index].strLocation_Name.GetBuffer(0));
//	}
//	m_ComBoxRoadName.SetCurSel(m_ParentFrame->m_nCurrentRoadID);
//}

BOOL CSystemSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(m_ParentFrame == NULL)OnCancel();
	DWORD dwSourceIP;
	dwSourceIP = m_ParentFrame->GetSourcePath();
	m_IPControl.SetAddress((BYTE)((dwSourceIP>>24)&0xFF), (BYTE)((dwSourceIP>>16)&0xFF), (BYTE)((dwSourceIP>>8)&0xFF), (BYTE)((dwSourceIP)&0xFF));
	CString	tmpStr;
	tmpStr = m_ParentFrame->GetDataBaseName();
	m_EditDataBaseName.SetWindowText(tmpStr.GetBuffer(0));
	tmpStr = m_ParentFrame->GetUserName();
	m_EditUserName.SetWindowText(tmpStr.GetBuffer(0));
	tmpStr = m_ParentFrame->GetPassWord();
	m_EditPassWord.SetWindowText(tmpStr.GetBuffer(0));

	return TRUE;
}

DWORD CSystemSettingDlg::IPStr2DWord(char* szIP)
{
	DWORD ret;
	if(szIP == NULL || strlen(szIP) < 8) return 0;
	int pTmp[4];
	BYTE* pDst = (BYTE*)&ret;
	sscanf(szIP, "%d.%d.%d.%d", &pTmp[0], &pTmp[1], &pTmp[2], &pTmp[3]);
	pDst[0] = (BYTE)pTmp[3];
	pDst[1] = (BYTE)pTmp[2];
	pDst[2] = (BYTE)pTmp[1];
	pDst[3] = (BYTE)pTmp[0];

	return ret;
}

void CSystemSettingDlg::OnBnClickedButton1()
{
	//CString		strConnection;
	char szIP[20];
	char szDataBaseName[30];
	char szUserName[30];
	char szPassWord[20];
	m_IPControl.GetWindowText(szIP, 20);
	if(strstr(szIP, "0.0.0.0"))
	{
		memset(szIP, 0, 20);
		szIP[0] = '.';
	}

	m_EditDataBaseName.GetWindowText(szDataBaseName, 30);
	if(strlen(szDataBaseName) <= 0)
	{
		MessageBox("修改失败，数据库名不能为空");
		OnCancel();
		return;
	}
	m_EditUserName.GetWindowText(szUserName, 30);
	if(strlen(szUserName) <= 0)
	{
		MessageBox("修改失败，用户名不能为空");
		OnCancel();
		return;
	}
	m_EditPassWord.GetWindowText(szPassWord, 20);
	//strConnection.Format("Provider=SQLOLEDB.1;Password=%s;User ID=%s;Initial Catalog=%s;Data Source=%s",
	//	szPassWord, szUserName, szDataBaseName, szIP);
	//if(m_ParentFrame->DoConnectDataBase(strConnection))
	//{
	//	m_StaticCurrentStatue.SetWindowText("已连接到数据库");
	//	m_ButtonConnect.EnableWindow(FALSE);
	//	m_ButtonDisConnect.EnableWindow(TRUE);
	//	m_ButtonStart.EnableWindow(TRUE);
	//	m_ButtonStop.EnableWindow(FALSE);
	//	m_IPControl.EnableWindow(FALSE);
	//	m_EditDataBaseName.EnableWindow(FALSE);
	//	m_EditUserName.EnableWindow(FALSE);
	//	m_EditPassWord.EnableWindow(FALSE);
	//	SetComBo();
	//	DWORD dwIP;
	//	dwIP = IPStr2DWord(szIP);
	//	m_ParentFrame->SetSourcePath(dwIP);
	//	m_ParentFrame->m_strDataBaseName = szDataBaseName;
	//	m_ParentFrame->m_strUserName = szUserName;
	//	m_ParentFrame->m_strPassWord = szPassWord;
	//}
	m_ParentFrame->m_rgSettingInfo.dwIP = IPStr2DWord(szIP);
	m_ParentFrame->m_rgSettingInfo.strDataBaseName = szDataBaseName;
	m_ParentFrame->m_rgSettingInfo.strUserName = szUserName;
	m_ParentFrame->m_rgSettingInfo.strPassWord = szPassWord;
	::PostMessage(AfxGetMainWnd()->m_hWnd, WM_SET_ONOK, 0, 0);
	DestroyWindow();
}

//void CSystemSettingDlg::OnBnClickedButton3()
//{
//	if(!m_ParentFrame->GetConnectStatus())return;
//	if(m_ParentFrame->Starting())
//	{
//		m_ButtonStart.EnableWindow(FALSE);
//		m_ButtonStop.EnableWindow(TRUE);
//		m_ComBoxRoadName.EnableWindow(FALSE);
//	}
//}

//void CSystemSettingDlg::OnCbnSelchangeCombo1()
//{
//	m_ParentFrame->m_nCurrentRoadID = m_ComBoxRoadName.GetCurSel();
//	m_ParentFrame->ShowRoadInfo();
//}

//void CSystemSettingDlg::OnBnClickedButton4()
//{
//	m_ParentFrame->Stop();
//	m_ButtonStart.EnableWindow(TRUE);
//	m_ButtonStop.EnableWindow(FALSE);
//	m_ComBoxRoadName.EnableWindow(TRUE);
//}

void CSystemSettingDlg::OnBnClickedButton2()
{
	//if(!m_ParentFrame->GetConnectStatus()) return;
	//if(m_ParentFrame->m_fIsRunning)
	//{
	//	m_ParentFrame->Stop();
	//}
	//m_ParentFrame->DisConnect();
	//m_ButtonConnect.EnableWindow(TRUE);
	//m_ButtonDisConnect.EnableWindow(FALSE);
	//m_ButtonStart.EnableWindow(FALSE);
	//m_ButtonStop.EnableWindow(FALSE);
	//m_ComBoxRoadName.ResetContent();
	//m_ComBoxRoadName.EnableWindow(TRUE);
	//m_StaticCurrentStatue.SetWindowText("未连接到数据库");
	//m_IPControl.EnableWindow(TRUE);
	//m_EditDataBaseName.EnableWindow(TRUE);
	//m_EditUserName.EnableWindow(TRUE);
	//m_EditPassWord.EnableWindow(TRUE);
	OnCancel();
}

void CSystemSettingDlg::OnOK()
{

}

void CSystemSettingDlg::OnCancel()
{
	::PostMessage(AfxGetMainWnd()->m_hWnd, WM_SET_ONCACAL, NULL, NULL);
	CDialog::OnCancel();
}

BOOL CSystemSettingDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}
