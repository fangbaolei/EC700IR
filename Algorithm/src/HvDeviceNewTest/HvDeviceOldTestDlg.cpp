#include "stdafx.h"
#include "HvDeviceOldTestDlg.h"
#include "IPEditDlg.h"

IMPLEMENT_DYNAMIC(COldHvDeviceTestDlg, CDialog)

#define ID_GET_DEV_TYPE		10086
#define ID_SET_IP			10087

CListCtrl* g_ListEx;
DWORD g_dwCurrentConnectIndexEx;

COldHvDeviceTestDlg::COldHvDeviceTestDlg(CWnd* pParent /* = NULL */)
: CDialog(COldHvDeviceTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDevice = NULL;
	m_pDlg = NULL;
	m_pImageDlg = NULL;
	InitializeCriticalSection(&m_csDevIPList);
}

COldHvDeviceTestDlg::~COldHvDeviceTestDlg()
{
	EnterCriticalSection(&m_csDevIPList);
	while(m_DevIPList.GetCount() > 0)
	{
		DevIPInfo* pTmp = m_DevIPList.RemoveHead();
		delete pTmp;
		pTmp = NULL;
	}
	LeaveCriticalSection(&m_csDevIPList);
	DeleteCriticalSection(&m_csDevIPList);
}

void COldHvDeviceTestDlg::EmptyList()
{
	EnterCriticalSection(&m_csDevIPList);
	while(m_DevIPList.GetCount() > 0)
	{
		DevIPInfo* pTmp = m_DevIPList.RemoveHead();
		delete pTmp;
		pTmp = NULL;
	}
	LeaveCriticalSection(&m_csDevIPList);
}

void COldHvDeviceTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListDevice);
}

BEGIN_MESSAGE_MAP(COldHvDeviceTestDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON13, OnBnClickedButton13)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnNMRclickList1)
	ON_COMMAND(ID_GET_DEV_TYPE, OnGetDevType)
	ON_COMMAND(ID_SET_IP, OnSetIP)
END_MESSAGE_MAP()

BOOL COldHvDeviceTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	SetWindowText("HvDevice旧接口测试");
	m_ListDevice.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListDevice.InsertColumn(0, "设备IP", LVCFMT_LEFT, 100);
	m_ListDevice.InsertColumn(1, "设备类型", LVCFMT_LEFT, 80);
	m_ListDevice.InsertColumn(2, "结果重连次数", LVCFMT_LEFT, 70);
	m_ListDevice.InsertColumn(3, "图像重连次数", LVCFMT_LEFT, 70);
	m_ListDevice.InsertColumn(4, "视频重连次数", LVCFMT_LEFT, 70);
	
	GetDlgItem(IDC_BUTTON1)->SetWindowText("搜索设备");
	GetDlgItem(IDC_BUTTON2)->SetWindowText("连接设备");
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON13)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON14)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON16)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON18)->ShowWindow(SW_HIDE);
	return TRUE;
}

void COldHvDeviceTestDlg::OnBnClickedButton1()
{
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	DWORD32 dwDeviceCount = 0;
	EmptyList();
	m_ListDevice.DeleteAllItems();
	char szIPBuf[1024<<8] = {0};
	dwDeviceCount = HVAPI_SearchDevice(DEV_TYPE_ALL, szIPBuf, 1024<<8);
	if(dwDeviceCount <= 0)
	{
		MessageBox("搜索不到相关设备", "INFO", MB_OK);
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
		return;
	}

	int iIPBufLoca = 0;
	int iIndex = 0;
	for(iIndex = 0; iIndex < (INT)dwDeviceCount; iIndex++)
	{
		int iTempLoca = 0;
		char szTempMac[32] = {0};
		char szTempIP[20] = {0};
		char szTempMask[20] = {0};
		char szTempGateway[20] = {0};
		while(szIPBuf[iIPBufLoca] != ',')
		{
			szTempMac[iTempLoca++] = szIPBuf[iIPBufLoca++];
		}
		iIPBufLoca++;
		iTempLoca = 0;
		while(szIPBuf[iIPBufLoca] != ',')
		{
			szTempIP[iTempLoca++] = szIPBuf[iIPBufLoca++];
		}
		iIPBufLoca++;
		iTempLoca = 0;
		while(szIPBuf[iIPBufLoca] != ',')
		{
			szTempMask[iTempLoca++] = szIPBuf[iIPBufLoca++];
		}
		iIPBufLoca++;
		iTempLoca = 0;
		while(szIPBuf[iIPBufLoca] != ';')
		{
			szTempGateway[iTempLoca++] = szIPBuf[iIPBufLoca++];
		}
		iIPBufLoca++;
		m_ListDevice.InsertItem(iIndex, "", 0);
		m_ListDevice.SetItemText(iIndex, 0, szTempIP);
		DevIPInfo* pNewInfo = new DevIPInfo;
		memcpy(pNewInfo->szIP, szTempIP, strlen(szTempIP));
		memcpy(pNewInfo->szMask, szTempMask, strlen(szTempMask));
		memcpy(pNewInfo->szGateWay, szTempGateway, strlen(szTempGateway));
		memcpy(pNewInfo->szMac, szTempMac, strlen(szTempMac));
		m_DevIPList.AddTail(pNewInfo);
	}
	if(m_hDevice == NULL)
	{
		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
	}
	m_iTotleDevice = dwDeviceCount;
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
}

void COldHvDeviceTestDlg::OnBnClickedButton2()
{
	if(m_hDevice || m_iTotleDevice <= 0)
	{
		return;
	}
	int iCurrentIndex = m_ListDevice.GetSelectionMark();
	if(iCurrentIndex<0 || iCurrentIndex>m_iTotleDevice)
	{
		return;
	}
	char szIP[20] = {0};
	m_ListDevice.GetItemText(iCurrentIndex, 0, szIP, 20);
	m_hDevice = HVAPI_Open(szIP, NULL);
	if(m_hDevice == NULL)
	{
		MessageBox("连接设备失败!", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
	g_ListEx = &m_ListDevice;
	g_dwCurrentConnectIndexEx = iCurrentIndex;
}

void COldHvDeviceTestDlg::OnBnClickedButton3()
{
	if(m_hDevice == NULL)
	{
		return;
	}
	HVAPI_Close(m_hDevice);
	m_hDevice = NULL;
	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
}

void COldHvDeviceTestDlg::OnBnClickedButton4()
{
	if(m_hDevice == NULL)
	{
		OnBnClickedButton2();
		if(m_hDevice == NULL)
		{
			return;
		}
	}

	if(m_pDlg)
	{
		delete m_pDlg;
		m_pDlg = NULL;
	}

	m_pDlg = new COldResultRecvTestDlg(&m_hDevice, this);
	m_pDlg->Create(IDD_DIALOG1, this);
	m_pDlg->ShowWindow(SW_SHOW);
}

void COldHvDeviceTestDlg::OnBnClickedButton8()
{
	m_pImageDlg = new COldImageRecvTestDlg(&m_hDevice, NULL);
	m_pImageDlg->Create(IDD_DIALOG2, this);
	m_pImageDlg->ShowWindow(SW_SHOW);
}

void COldHvDeviceTestDlg::OnBnClickedButton9()
{
	m_pVideoDlg = new COldVideoRecvTestDlg(&m_hDevice, NULL);
	m_pVideoDlg->Create(IDD_DIALOG3, this);
	m_pVideoDlg->ShowWindow(SW_SHOW);
}

void COldHvDeviceTestDlg::OnBnClickedButton10()
{
	m_pCmdDlg = new COldCmdTestDlg(&m_hDevice, NULL);
	m_pCmdDlg->Create(IDD_DIALOG5, this);
	m_pCmdDlg->ShowWindow(SW_SHOW);
}

void COldHvDeviceTestDlg::OnBnClickedButton13()
{
	COldGroupTestDlg Dlg(this);
	Dlg.DoModal();
}

void COldHvDeviceTestDlg::OnBnClickedButton5()
{
	GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);
	DWORD dwCount = m_ListDevice.GetItemCount();
	if(dwCount <= 0)
	{
		OnBnClickedButton1();
		dwCount = m_ListDevice.GetItemCount();
		if(dwCount <= 0)
		{
			MessageBox("未搜索到设备,请检查网络!", "ERROR", MB_OK|MB_ICONERROR);
			GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
			return;
		}
	}
	DWORD dwIndex = 0;
	for(dwIndex = 0; dwIndex < dwCount; dwIndex++)
	{
		char szIP[20] = {0};
		m_ListDevice.GetItemText(dwIndex, 0, szIP, 20);
		if(strlen(szIP) <= 0)
		{
			m_ListDevice.SetItemText(dwIndex, 1, "未知");
			continue;
		}
		INT iDevType = 0;
		HVAPI_GetDevType(szIP, &iDevType);
		switch(iDevType)
		{
		case DEV_TYPE_HVCAM_200W:
			m_ListDevice.SetItemText(dwIndex, 1, "一体机200W");
			break;
		case DEV_TYPE_HVCAM_500W:
			m_ListDevice.SetItemText(dwIndex, 1, "一体机500W");
			break;
		case DEV_TYPE_HVSIGLE:
			m_ListDevice.SetItemText(dwIndex, 1, "单板");
			break;
		case DEV_TYPE_HVCAM_SINGLE:
			m_ListDevice.SetItemText(dwIndex, 1, "简化版一体机");
			break;
		case DEV_TYPE_HVCAMERA:
		    m_ListDevice.SetItemText(dwIndex, 1, "纯相机");
			break;
		default:
			m_ListDevice.SetItemText(dwIndex, 1, "未知");
			break;
		}
	}
	GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
}

void COldHvDeviceTestDlg::OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int iColumn = pNMListView->iItem;
	if(iColumn < 0 || iColumn > m_ListDevice.GetItemCount())
	{
		return;
	}
	CMenu cMenu;
	cMenu.CreatePopupMenu();
	cMenu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_GET_DEV_TYPE, "获取设备类型");
	cMenu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_SET_IP, "修改设备IP");
	CPoint cPt;
	GetCursorPos(&cPt);
	cMenu.TrackPopupMenu(TPM_LEFTALIGN, cPt.x, cPt.y, this);
	*pResult = 0;
}

void COldHvDeviceTestDlg::OnGetDevType()
{
	int iCurrentSelect = m_ListDevice.GetSelectionMark();
	if(iCurrentSelect < 0 || iCurrentSelect > m_iTotleDevice)
	{
		MessageBox("未选择设备，获取失败", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}

	char szIP[20] = {0};
	m_ListDevice.GetItemText(iCurrentSelect, 0, szIP, 20);

	INT iDevType = DEV_TYPE_UNKNOWN;
	if(HVAPI_GetDevType(szIP, &iDevType) != S_OK)
	{
		MessageBox("获取设备类型失败", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	switch(iDevType)
	{
	case DEV_TYPE_HVCAM_200W:
		m_ListDevice.SetItemText(iCurrentSelect, 1, "一体机200W");
		break;
	case DEV_TYPE_HVCAM_500W:
		m_ListDevice.SetItemText(iCurrentSelect, 1, "一体机500W");
		break;
	case DEV_TYPE_HVSIGLE:
		m_ListDevice.SetItemText(iCurrentSelect, 1, "单板");
		break;
	case DEV_TYPE_HVCAM_SINGLE:
		m_ListDevice.SetItemText(iCurrentSelect, 1, "简化版一体机");
		break;
	default:
		m_ListDevice.SetItemText(iCurrentSelect, 1, "未知");
		break;
	}
}

void COldHvDeviceTestDlg::OnSetIP()
{
	EnterCriticalSection(&m_csDevIPList);
	int iCurrentSelect = m_ListDevice.GetSelectionMark();
	if(iCurrentSelect < 0 || iCurrentSelect >= m_DevIPList.GetCount())
	{
		LeaveCriticalSection(&m_csDevIPList);
		MessageBox("未选择设备，获取失败", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	IPInfo cTmpIPInfo;

	POSITION pos = m_DevIPList.FindIndex(iCurrentSelect);
	if(!pos)
	{
		MessageBox("获取设备MAC失败，设置失败!", "ERROR", MB_OK|MB_ICONERROR);
		LeaveCriticalSection(&m_csDevIPList);
		return;
	}

	DevIPInfo* pTmp = m_DevIPList.GetAt(pos);
	DWORD32 TmpByte[6];
	sscanf(pTmp->szIP, "%d.%d.%d.%d", &TmpByte[0], &TmpByte[1], &TmpByte[2], &TmpByte[3]);
	cTmpIPInfo.dwIP = (TmpByte[0]<<24) | (TmpByte[1]<<16) | (TmpByte[2]<<8) | TmpByte[3];
	sscanf(pTmp->szMask, "%d.%d.%d.%d", &TmpByte[0], &TmpByte[1], &TmpByte[2], &TmpByte[3]);
	cTmpIPInfo.dwMask = (TmpByte[0]<<24) | (TmpByte[1]<<16) | (TmpByte[2]<<8) | TmpByte[3];
	sscanf(pTmp->szGateWay, "%d.%d.%d.%d", &TmpByte[0], &TmpByte[1], &TmpByte[2], &TmpByte[3]);
	cTmpIPInfo.dwGateWay = (TmpByte[0]<<24) | (TmpByte[1]<<16) | (TmpByte[2]<<8) | TmpByte[3];
	sscanf(pTmp->szMac, "%02X-%02X-%02X-%02X-%02X-%02X", &TmpByte[0], &TmpByte[1], &TmpByte[2], &TmpByte[3], &TmpByte[4], &TmpByte[5]);
	cTmpIPInfo.dw64Mac = 0;
	cTmpIPInfo.dw64Mac = (((DWORD64)TmpByte[0]))|(((DWORD64)TmpByte[1])<<8) |(((DWORD64)TmpByte[2])<<16)
		|(((DWORD64)TmpByte[3])<<24) |(((DWORD64)TmpByte[4])<<32) |(((DWORD64)TmpByte[5])<<40);
	pTmp = NULL;
	LeaveCriticalSection(&m_csDevIPList);

	IPInfo cSetIPInfo;
	cSetIPInfo.dw64Mac = cTmpIPInfo.dw64Mac;
	cSetIPInfo.dwIP = cTmpIPInfo.dwIP;
	cSetIPInfo.dwMask = cTmpIPInfo.dwMask;
	cSetIPInfo.dwGateWay = cTmpIPInfo.dwGateWay;
	CIPEditDlg* pDlg = new CIPEditDlg(&cSetIPInfo, this);
	if(pDlg->DoModal() == IDOK)
	{
		if(cSetIPInfo.dwIP == cTmpIPInfo.dwIP &&
			cSetIPInfo.dwMask == cTmpIPInfo.dwMask &&
			cSetIPInfo.dwGateWay == cTmpIPInfo.dwGateWay && 
			cSetIPInfo.dw64Mac == cTmpIPInfo.dw64Mac)
		{
			MessageBox("IP信息没有任何修改，无需设置");
			return;
		}

		m_ListDevice.EnableWindow(FALSE);
		SetWindowText("正在设置IP，请稍候...");
		if(FAILED(HVAPI_SetIPFromMac(cSetIPInfo.dw64Mac, cSetIPInfo.dwIP, cSetIPInfo.dwMask, cSetIPInfo.dwGateWay)))
		{
			MessageBox("设置设备IP失败!", "ERROR", MB_OK|MB_ICONERROR);
			m_ListDevice.EnableWindow(TRUE);
			SetWindowText("HvDevice新接口测试");
			return;
		}
		else
		{
			MessageBox("设置设备IP成功!", "INFO", MB_OK);
			m_ListDevice.EnableWindow(TRUE);
			SetWindowText("HvDevice新接口测试");
			OnBnClickedButton1();
		}
	}
}
