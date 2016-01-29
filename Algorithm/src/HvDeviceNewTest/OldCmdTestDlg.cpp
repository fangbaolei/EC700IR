#include "stdafx.h"
#include "OldCmdTestDlg.h"

IMPLEMENT_DYNAMIC(COldCmdTestDlg, CDialog)

COldCmdTestDlg::COldCmdTestDlg(HVAPI_HANDLE* phHandle, CWnd* pParent /* = NULL */)
: CDialog(COldCmdTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDevice = phHandle;
}

COldCmdTestDlg::~COldCmdTestDlg()
{

}

void COldCmdTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COldCmdTestDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
END_MESSAGE_MAP()

BOOL COldCmdTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	return TRUE;
}

void COldCmdTestDlg::OnBnClickedButton1()
{
	if(*m_hDevice == NULL)
	{
		MessageBox("未连接到识别器");
		return;
	}
	CString strBuf;
	GetDlgItemText(IDC_EDIT1, strBuf);
	if(strBuf == "")
	{
		MessageBox("输入不能为空");
		return;
	}

	char szRetBuf[128*1024] = {0};
	INT nRetLen = 0;
	if ( S_OK == HVAPI_ExecCmd(*m_hDevice, strBuf.GetBuffer(0), szRetBuf, sizeof(szRetBuf), &nRetLen) )
	{
		CString strMsg;
		strMsg.Format("HVAPI_ExecCmd is OK. \nRecvStringLen = %d \n RecvBitsNum = %d ?\n\n%s", strlen(szRetBuf), nRetLen, szRetBuf);
		AfxMessageBox(strMsg);
	}
	else
	{
		AfxMessageBox("ERROR!");
	}
}

void COldCmdTestDlg::OnBnClickedButton4()
{
	if(*m_hDevice == NULL)
	{
	MessageBox("未连接到识别器");
	return;
	}
	CString strBuf;
	GetDlgItemText(IDC_EDIT9, strBuf);
	if ( "" == strBuf )
	{
		AfxMessageBox("输入不能为空");
		return ;
	}

	char szRetBuf[512*1024] = {0};
	INT nRetLen = 0;
	if ( S_OK == HVAPI_GetInfo(*m_hDevice, strBuf.GetBuffer(0), szRetBuf, sizeof(szRetBuf), &nRetLen) )
	{
		CString strMsg;
		strMsg.Format("HVAPI_GetInfo is OK. %d == %d ?\n\n%s", strlen(szRetBuf), nRetLen, szRetBuf);
		AfxMessageBox(strMsg);
	}
	else
	{
		AfxMessageBox("ERROR!");
	}
}

void COldCmdTestDlg::OnBnClickedButton5()
{
	static char szXmlParam[512*1024] = {0};
	INT nRetLen = 0;
	GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);
	if ( S_OK == HVAPI_GetParam(*m_hDevice, szXmlParam, sizeof(szXmlParam), &nRetLen) )
	{
		CFileDialog cFileDlg(FALSE);
		if(cFileDlg.DoModal() == IDOK)
		{
			FILE* fp = fopen(cFileDlg.GetPathName(), "wb");
			if(fp)
			{
				fwrite(szXmlParam, strlen(szXmlParam), 1, fp);
				fclose(fp);
			}
		}

		CString strMsg;
		strMsg.Format("HVAPI_GetXmlParam is OK. %d == %d ?\n\nAlready save to %s'", strlen(szXmlParam), nRetLen,
			cFileDlg.GetPathName());
		AfxMessageBox(strMsg);
	}
	else
	{
		AfxMessageBox("ERROR!");
	}
	GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
}

void COldCmdTestDlg::OnBnClickedButton6()
{
	GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
	char szFileFilter[] = "XML文件|*.xml|所有文件|*.*|";
	CString strSelectedFile;
	CFileDialog FileDialogBox(
		TRUE, NULL, "",
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		szFileFilter, this
		);
	if ( FileDialogBox.DoModal() == IDOK )
	{
		strSelectedFile = FileDialogBox.GetPathName();
		char* pszXmlParam;
		unsigned long ulFileSize = 0; 
		unsigned long ulReadedSize = 0;
		FILE *fp = fopen(strSelectedFile.GetBuffer(0), "rb");  
		if ( fp )  
		{  
			fseek(fp, 0, SEEK_END);  
			ulFileSize = ftell(fp);
			pszXmlParam = new char[ulFileSize];
			fseek(fp, 0, SEEK_SET);
			ulReadedSize = (unsigned long)fread(pszXmlParam, 1, ulFileSize, fp);  
			fclose(fp); 
			if(ulReadedSize != ulFileSize)
			{
				GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);
				return; 
			}
		}  

		if ( pszXmlParam )
		{
			if ( S_OK == HVAPI_SetParam(*m_hDevice, pszXmlParam) )
			{
				AfxMessageBox("HVAPI_SetXmlParam is OK");
			}
			else
			{
				AfxMessageBox("ERROR!");
			}
		}
		delete[] pszXmlParam;
	}
	GetDlgItem(IDC_BUTTON6)->EnableWindow(TRUE);
}
