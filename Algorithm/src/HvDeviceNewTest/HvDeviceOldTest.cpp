#include "stdafx.h"
#include "HvDeviceOldTest.h"
#include ".\hvdeviceoldtest.h"

#define WM_SEARCH_DEV	(WM_USER + 100)

IMPLEMENT_DYNAMIC(CHvDeviceOldTestDlg, CDialog)

CHvDeviceOldTestDlg::CHvDeviceOldTestDlg(CWnd* pParent /* = NULL */)
: CDialog(CHvDeviceOldTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_iTotolDevice = 0;
	m_iCurrentSelectDevice = -1;
}

CHvDeviceOldTestDlg::~CHvDeviceOldTestDlg()
{

}

BEGIN_MESSAGE_MAP(CHvDeviceOldTestDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_MESSAGE(WM_SEARCH_DEV, OnSearchDev)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnNMDblclkList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnNMRclickList1)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
END_MESSAGE_MAP()

void CHvDeviceOldTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListShowFrame);
	DDX_Control(pDX, IDC_LIST2, m_ListInfo);
}

BOOL CHvDeviceOldTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_ListShowFrame.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListShowFrame.InsertColumn(0, "设备IP", LVCFMT_LEFT, 110);
	m_ListShowFrame.InsertColumn(1, "设备编号", LVCFMT_LEFT, 140);
	m_ListShowFrame.InsertColumn(2, "类型", LVCFMT_LEFT, 100);
	m_ListShowFrame.InsertColumn(3, "MAC地址", LVCFMT_LEFT, 120);
	m_ListShowFrame.InsertColumn(4, "状态", LVCFMT_LEFT, 60);
	m_ListShowFrame.InsertColumn(5, "重连次数", LVCFMT_LEFT, 80);

	m_ListInfo.ResetContent();
	return TRUE;
}

void CHvDeviceOldTestDlg::OnCancel()
{
	CDialog::OnCancel();
}

void CHvDeviceOldTestDlg::OnBnClickedButton1()
{
	SendMessage(WM_SEARCH_DEV, 0, 0);
}

LRESULT CHvDeviceOldTestDlg::OnSearchDev(WPARAM wParam, LPARAM lParam)
{
	char szIPBufe[1024 << 8] = {0};
	DWORD dwDevCount = 0;
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON8)->EnableWindow(FALSE);

	dwDevCount = HVAPI_SearchDevice(DEV_TYPE_ALL, szIPBufe, 1024 << 8);
	if(dwDevCount <= 0)
	{
		MessageBox("搜索不到任何设备，证确认网络中已接入识别器设备");
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON8)->EnableWindow(TRUE);
		return S_OK;
	}

	m_ListShowFrame.DeleteAllItems();
	m_ListInfo.ResetContent();
	CDeviceType* cTmpDev;
	INT iLastDevCount = (INT)m_DeviceList.GetSize();
	INT iIndex = 0;
	for(iIndex=0; iIndex<iLastDevCount; iIndex++)
	{
		cTmpDev = m_DeviceList.RemoveHead();
		if(cTmpDev->GetDevConnectStatus() == TRUE)
		{
			m_DeviceList.AddTail(cTmpDev);
		}
	}

	int iIPBuferLoca = 0;
	for(iIndex = 0; iIndex < (INT)dwDevCount; iIndex++)
	{
		int iTempLoca = 0;
		char szTempMac[32] = {0};
		char szTempIP[20] = {0};
		char szTempMask[20] = {0};
		char szTempGateway[20] = {0};
		while(szIPBufe[iIPBuferLoca] != ',')
		{
			szTempMac[iTempLoca++] = szIPBufe[iIPBuferLoca++];
		}
		iIPBuferLoca++;
		iTempLoca = 0;
		while(szIPBufe[iIPBuferLoca] != ',')
		{
			szTempIP[iTempLoca++] = szIPBufe[iIPBuferLoca++];
		}
		iIPBuferLoca++;
		iTempLoca = 0;
		while(szIPBufe[iIPBuferLoca] != ',')
		{
			szTempMask[iTempLoca++] = szIPBufe[iIPBuferLoca++];
		}
		iIPBuferLoca++;
		iTempLoca = 0;
		while(szIPBufe[iIPBuferLoca] != ';')
		{
			szTempGateway[iTempLoca++] = szIPBufe[iIPBuferLoca++];
		}
		iIPBuferLoca++;
		CDeviceType *pNewDevice = new CDeviceType(szTempMac, szTempIP, szTempMask, szTempGateway);
		m_DeviceList.AddTail(pNewDevice);
	}

	iLastDevCount = (INT)m_DeviceList.GetSize();
	for(iIndex=0; iIndex<iLastDevCount; iIndex++)
	{
		POSITION pos = m_DeviceList.FindIndex(iIndex);
		const char* pTempMac;
		cTmpDev = m_DeviceList.GetAt(pos);
		m_ListShowFrame.InsertItem(iIndex, "", 0);
		pTempMac = cTmpDev->GetDevMacAddr();
		m_ListShowFrame.SetItemText(iIndex, 0, cTmpDev->GetDevIPAddr());
		m_ListShowFrame.SetItemText(iIndex, 3, pTempMac);
		char szTemp[3] = {0};
		szTemp[0] = pTempMac[15];
		szTemp[1] = pTempMac[16];
		if(strcmp(szTemp, "B6") == 0)
		{
			m_ListShowFrame.SetItemText(iIndex, 2, "一体机200W");
		}
		else if(strcmp(szTemp, "B7") == 0)
		{
			m_ListShowFrame.SetItemText(iIndex, 2, "单板");
		}
		else if(strcmp(szTemp, "B8") == 0)
		{
			m_ListShowFrame.SetItemText(iIndex, 2, "一体机500W");
		}
		else if(strcmp(szTemp, "B9") == 0)
		{
			m_ListShowFrame.SetItemText(iIndex, 2, "简化版一体机");
		}
		else
		{
			m_ListShowFrame.SetItemText(iIndex, 2, "未知");
		}
		cTmpDev->SetShowInfoItem(&m_ListShowFrame, iIndex);
	}
	m_iTotolDevice = iLastDevCount;
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON8)->EnableWindow(TRUE);
	return S_OK;
}

void CHvDeviceOldTestDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iCurrentIntem = m_ListShowFrame.GetSelectionMark();
	if(iCurrentIntem <= 0 || iCurrentIntem > m_iTotolDevice)
	{
		return;
	}

	m_hCursor = ::LoadCursor(AfxGetInstanceHandle(), IDC_WAIT);
	::SetCursor(m_hCursor);
	POSITION pos = m_DeviceList.FindIndex(iCurrentIntem);
	CDeviceType* pTempDev = m_DeviceList.GetAt(pos);
	if(!pTempDev->GetDevConnectStatus())
	{
		pTempDev->ConnectDevice();
		if(m_iCurrentSelectDevice != -1)
		{
			pos = m_DeviceList.FindIndex(m_iCurrentSelectDevice);
			CDeviceType* pDevice = m_DeviceList.GetAt(pos);
			pDevice->EnableShowInfo(FALSE, NULL);
			pDevice->SetShowPlateFrame(NULL);
		}
		m_iCurrentSelectDevice = iCurrentIntem;
		pTempDev->EnableShowInfo(TRUE, &m_ListInfo);
		pTempDev->UpdateDeviceInfo();
		pTempDev->SetShowPlateFrame(GetDlgItem(IDC_EDIT2));
		pTempDev->ConnectDevice();
		pTempDev->SetRecordCallBack();
	}
	else
	{
		pTempDev->DisConnectDevice();
	}
	m_hCursor = ::LoadCursor(AfxGetInstanceHandle(), IDC_ARROW);
	::SetCursor(m_hCursor);
	*pResult = 0;
}

#define ID_CONNECT_DEV		10086
#define ID_DISCONNECT_DEV	10087
#define ID_REFRESH_DEV_INFO	10088
#define ID_RESEARCH_DEV		10089
#define ID_SET_SAVE_PATH	10090
#define ID_SET_HISTORY		10091
#define ID_SET_PICCOVENT	10092
#define ID_SET_GET_HISTORY_RECORD	20086
#define ID_UNSET_GET_HISTORY_RECORD	20087
#define ID_SET_GET_HISTORY_VIDEO	20088
#define ID_UNSET_GET_HISTORY_VIDEO	20089
#define ID_SET_ENHACE_PIC			20090
#define ID_SET_COMBIM_PIC			20091

void CHvDeviceOldTestDlg::OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu cMenu;
	cMenu.CreatePopupMenu();
	cMenu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_CONNECT_DEV, "连接设备");
	cMenu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_DISCONNECT_DEV, "断开连接");
	cMenu.AppendMenu(MF_SEPARATOR);
	cMenu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_REFRESH_DEV_INFO, "刷新设备信息");
	cMenu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_RESEARCH_DEV, "重新搜索设备");
	cMenu.AppendMenu(MF_SEPARATOR);
	cMenu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_SET_SAVE_PATH, "设置保存路径");
	CMenu menuTmp;
	menuTmp.CreatePopupMenu();
	menuTmp.AppendMenu(MF_BYCOMMAND, ID_SET_GET_HISTORY_RECORD, "接收历史结果");
	menuTmp.AppendMenu(MF_BYCOMMAND, ID_UNSET_GET_HISTORY_RECORD, "取消接收历史结果");
	menuTmp.AppendMenu(MF_SEPARATOR);
	menuTmp.AppendMenu(MF_BYCOMMAND, ID_SET_GET_HISTORY_VIDEO, "接收历史录像");
	menuTmp.AppendMenu(MF_BYCOMMAND, ID_UNSET_GET_HISTORY_VIDEO, "取消接收历史录像");
	cMenu.InsertMenu(6, MF_BYCOMMAND | MF_POPUP, (UINT_PTR)menuTmp.m_hMenu, "设置接收历史数据");
	menuTmp.Detach();
	cMenu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_SET_PICCOVENT, "图片处理");
	CPoint cPt;
	GetCursorPos(&cPt);
	cMenu.TrackPopupMenu(TPM_LEFTALIGN, cPt.x, cPt.y, this);
	*pResult = 0;
}


void CHvDeviceOldTestDlg::OnOK()
{
    // 防止按回车退出
}


BOOL CHvDeviceOldTestDlg::PreTranslateMessage(MSG* pMsg) 
{

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		 return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
} 
void CHvDeviceOldTestDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
}
