// DiskSettingToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <atlstr.h>
#include "DiskSettingTool.h"
#include "DiskSettingToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDiskSettingToolDlg::CDiskSettingToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDiskSettingToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strAppPath = "";
}

void CDiskSettingToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_btOK);
	DDX_Control(pDX, IDC_BUTTON2, m_btCancel);
	DDX_Control(pDX, IDC_COMBO1, m_cbCurrentDisk);
	DDX_Control(pDX, IDC_LIST1, m_lcDiskInfo);
}

BEGIN_MESSAGE_MAP(CDiskSettingToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDClickList)
	ON_NOTIFY(NM_RDBLCLK, IDC_LIST1, OnRDClickList)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
END_MESSAGE_MAP()

void CDiskSettingToolDlg::StartChecking()
{
	char szAppPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szAppPath, MAX_PATH);
	PathRemoveFileSpec(szAppPath);
	m_strAppPath = szAppPath;
	strcat(szAppPath, "\\StartUp.config");
	CFileFind	m_FindFile;
    if(m_FindFile.FindFile(szAppPath))
	{
		CString strWarmmingInfo;
		strWarmmingInfo.Format("    系统检测到本机已经配置过图片保存磁盘\r\n\
信息，如果已经保存有识别结果图片，重新配\r\n\
置很可能会造成原先保存的图片丢失，请确认\r\n\
是否确实需要重新配置?");
		if(MessageBox(strWarmmingInfo.GetBuffer(), "警告", MB_YESNO | MB_ICONWARNING) == IDNO)
		{
			OnCancel();
		}
	}
}

BOOL CDiskSettingToolDlg::GetDiskInfo(CString strDiskChar, DWORD& dwDiskSize)
{
	if(strDiskChar == m_strSystemDisk) return FALSE;
	UINT	uType;
	uType = GetDriveType(strDiskChar.GetBuffer());
	if(uType != DRIVE_FIXED) return FALSE;
	_ULARGE_INTEGER lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes;
	GetDiskFreeSpaceEx(strDiskChar.GetBuffer(), &lpFreeBytesAvailableToCaller, &lpTotalNumberOfBytes, &lpTotalNumberOfFreeBytes);
	dwDiskSize = (DWORD)(lpTotalNumberOfBytes.QuadPart / (1024 * 1024 * 1024));
	return TRUE;
}

void CDiskSettingToolDlg::GetDiskListInfo()
{
	m_lcDiskInfo.DeleteAllItems();
	int		nPos = 0;
	CString	strDrive = "?:";
	CString	strDiskSize;
	DWORD	dwDriveList = ::GetLogicalDrives();
	DWORD	dwDiskSize = 0;
	DWORD	dwIndex = 0;
	while(dwDriveList)
	{
		if(dwDriveList & 1)
		{
			strDrive.SetAt(0, 0x41+nPos);
			if(GetDiskInfo(strDrive, dwDiskSize))
			{
				m_lcDiskInfo.InsertItem(dwIndex, "", 0);
				m_lcDiskInfo.SetItemText(dwIndex, 0, strDrive.GetBuffer());
				strDiskSize.Format("%dGB", dwDiskSize);
				m_lcDiskInfo.SetItemText(dwIndex, 1, strDiskSize.GetBuffer());
				m_lcDiskInfo.SetItemText(dwIndex, 2, " ");
				dwIndex++;
			}
		}
		dwDriveList >>= 1;
		nPos++;
	}
}

BOOL CDiskSettingToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	
	StartChecking();
	m_cbCurrentDisk.ResetContent();
	m_lcDiskInfo.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_lcDiskInfo.SetBkColor(RGB(200, 220, 220));
	m_lcDiskInfo.SetTextBkColor(RGB(200, 220, 220));
	m_lcDiskInfo.InsertColumn(0, "盘符", LVCFMT_LEFT, 84);
	m_lcDiskInfo.InsertColumn(1, "容量", LVCFMT_LEFT, 90);
	m_lcDiskInfo.InsertColumn(2, "选择", LVCFMT_LEFT, 90);

	LPTSTR	szSystemDir = new char[MAX_PATH];
	GetSystemDirectory(szSystemDir, MAX_PATH);
	char szTmp[3];
	szTmp[0] = szSystemDir[0];
	szTmp[1] = szSystemDir[1];
	szTmp[2] = '\0';
	m_strSystemDisk = szTmp;
	GetDiskListInfo();

	return TRUE;
}

void CDiskSettingToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

void CDiskSettingToolDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CDiskSettingToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDiskSettingToolDlg::OnOK()
{

}

void CDiskSettingToolDlg::OnCancel()
{
	CDialog::OnCancel();
}

void CDiskSettingToolDlg::OnDClickList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem == -1)return;
	char szTmp[10];
	m_lcDiskInfo.GetItemText(pNMListView->iItem, 2, szTmp, 10);
	if(strcmp(szTmp, "√") == 0) return;
	m_lcDiskInfo.SetItemText(pNMListView->iItem, 2, "√");
	memset(szTmp, 0, 10);
	m_lcDiskInfo.GetItemText(pNMListView->iItem, 0, szTmp, 10);
	m_cbCurrentDisk.AddString(szTmp);
	m_cbCurrentDisk.SetCurSel(m_cbCurrentDisk.GetCount() - 1);
}

void CDiskSettingToolDlg::OnRDClickList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem == -1)return;
	char szTmp[10];
	m_lcDiskInfo.GetItemText(pNMListView->iItem, 2, szTmp, 10);
	if(strcmp(szTmp, " ") == 0) return;
	m_lcDiskInfo.SetItemText(pNMListView->iItem, 2, " ");
	memset(szTmp, 0, 10);
	m_lcDiskInfo.GetItemText(pNMListView->iItem, 0, szTmp, 10);
	int nCount, nIndex;
	nCount = m_cbCurrentDisk.GetCount();
	char szComboText[10];
	for(nIndex=0; nIndex<nCount; nIndex++)
	{
		memset(szComboText, 0, 10);
		m_cbCurrentDisk.GetLBText(nIndex, szComboText);
		if(strcmp(szComboText, szTmp) == 0)
		{
			m_cbCurrentDisk.DeleteString(nIndex);
			if(m_cbCurrentDisk.GetCount() > 0)
			{
				m_cbCurrentDisk.SetCurSel(m_cbCurrentDisk.GetCount() - 1);
			}
			else
			{
				m_cbCurrentDisk.SetWindowText("");
			}
			return;
		}
	}
}

void CDiskSettingToolDlg::OnBnClickedButton1()
{
	CString strIniFileName;
	if(m_cbCurrentDisk.GetCount() <= 0)
	{
		MessageBox("请先选择要保存的磁盘然后再保存配置!", "提示", MB_OK | MB_ICONWARNING);
		return;
	}
	strIniFileName = m_strAppPath;
	strIniFileName += "\\StartUp.config";
	char szDisk[100];
	memset(szDisk, 0, 100);
	m_cbCurrentDisk.GetWindowText(szDisk, 100);
	WritePrivateProfileStringA("DiskInfo", "CurrentDir", szDisk, strIniFileName.GetBuffer());
	int nCount, nIndex, nPos;
	memset(szDisk, 0, 100);
	nCount = m_cbCurrentDisk.GetCount();
	nPos = 0;
	for(nIndex=0; nIndex<nCount; nIndex++)
	{
		nPos += m_cbCurrentDisk.GetLBText(nIndex, szDisk+nPos);
	}
	WritePrivateProfileStringA("DiskInfo", "DiskList", szDisk, strIniFileName.GetBuffer());
	if(MessageBox("保存设置成功，是否退出配置程序?", "提示", MB_YESNO) == IDYES)
	{
		OnCancel();
	}
}

void CDiskSettingToolDlg::OnBnClickedButton2()
{
	OnCancel();
}

