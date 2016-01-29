// HvDeviceNewTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HvDeviceNewTest.h"
#include "HvDeviceNewTestDlg.h"
#include ".\hvdevicenewtestdlg.h"
#include "GroupTestDlg.h"
#include "IPEditDlg.h"
#include "AutoLink_demoDlg.h"

#include <imagehlp.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_GET_DEV_TYPE		10086
#define ID_SET_IP			10087
#define ID_CAPTURE_IMAGE   10088



// CHvDeviceNewTestDlg 对话框

CListCtrl* g_pList;
DWORD g_dwCurrentConnectIndex;
typedef struct _ipIndex
{
	DWORD32 ip;
	DWORD32 index;
	_ipIndex()
	{
		ip = 0;
		index = 0;
	}
}ipIndex;
ipIndex ipbuf[200];

CHvDeviceNewTestDlg::CHvDeviceNewTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHvDeviceNewTestDlg::IDD, pParent)
	, m_BeforeIndex(0)
	, m_autolinkDlg(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDevice = NULL;
	m_pDlg = NULL;
	m_pOldTestDlg = NULL;
}

void CHvDeviceNewTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListDevice);
}

BEGIN_MESSAGE_MAP(CHvDeviceNewTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON13, OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON14, OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON16, OnBnClickedButton16)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnNMRclickList1)
	ON_COMMAND(ID_GET_DEV_TYPE, OnGetDevType)
	ON_COMMAND(ID_CAPTURE_IMAGE, OnCaptureImage)
	ON_COMMAND(ID_SET_IP, OnSetIP)
	ON_BN_CLICKED(IDC_BUTTON12, OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON18, OnBnClickedButton18)
	ON_BN_CLICKED(IDC_BUTTON15, OnBnClickedButton15)

//	ON_WM_CTLCOLOR()
ON_WM_CLOSE()
ON_WM_KEYDOWN()
ON_BN_CLICKED(IDC_BUTTON_ADD_STATIC_IP, OnBnClickedButtonAddStaticIp)
ON_BN_CLICKED(IDC_BUTTON_CAPTUEIMAGE, OnBnClickedButtonCaptueimage)
END_MESSAGE_MAP()


// CHvDeviceNewTestDlg 消息处理程序

void CHvDeviceNewTestDlg::OnCancel()
{
	if(m_hDevice != NULL)
	{
		HVAPI_CloseEx(m_hDevice);
		m_hDevice = NULL;
	}
	CDialog::OnCancel();
}

BOOL CHvDeviceNewTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	m_ListDevice.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListDevice.InsertColumn(0, "设备IP", LVCFMT_LEFT, 100);
	m_ListDevice.InsertColumn(1, "设备编号", LVCFMT_LEFT, 170);
	m_ListDevice.InsertColumn(2, "设备类型", LVCFMT_LEFT, 90);
	m_ListDevice.InsertColumn(3, "XML版本", LVCFMT_LEFT, 60);
	m_ListDevice.InsertColumn(4, "结果重连次数", LVCFMT_LEFT, 100);
	m_ListDevice.InsertColumn(5, "图像重连次数", LVCFMT_LEFT, 100);
	m_ListDevice.InsertColumn(6, "视频重连次数", LVCFMT_LEFT, 100);


	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON8)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON10)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON18)->EnableWindow(FALSE);



	//CTime cTime(1333072285087/1000);
	//MessageBox(cTime.Format("%Y_%m_%d-%H:%M:%S"));


	CenterWindow(); 

	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CHvDeviceNewTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHvDeviceNewTestDlg::OnPaint() 
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
HCURSOR CHvDeviceNewTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHvDeviceNewTestDlg::OnBnClickedButton1()
{
	//GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	DWORD32 dwDeviceCount = 0;
	m_ListDevice.DeleteAllItems();
	if(HVAPI_SearchDeviceCount(&dwDeviceCount) != S_OK)
	{
		MessageBox("搜索设备失败", "ERROR", MB_OK|MB_ICONERROR);
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
		return;
	}
	if(dwDeviceCount <= 0)
	{
		MessageBox("搜索不到相关设备", "INFO", MB_OK);
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
		return;
	}
	
	DWORD64 dw64Mac;
	DWORD32 dwIP;
	DWORD32 dwMask;
	DWORD32 dwGateway;

	CString strIP;
//	ipIndex ipbuf[200];
	ipIndex iptemp;
	for(DWORD32 dwIndex=0; dwIndex<dwDeviceCount; dwIndex++)
	{
		if(HVAPI_GetDeviceAddr(dwIndex, &dw64Mac, &dwIP, &dwMask, &dwGateway) != S_OK)
		{
			m_ListDevice.DeleteAllItems();
			GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
			return;
		}
		ipbuf[dwIndex].ip = dwIP;
		ipbuf[dwIndex].index = dwIndex;
	}
	for(DWORD32 i=0; i<dwDeviceCount; i++)
		for(DWORD32 j=1; j<dwDeviceCount-i; j++)
		{
			if(ipbuf[j-1].ip > ipbuf[j].ip)
			{
				iptemp = ipbuf[j-1];
				ipbuf[j-1]=ipbuf[j];
				ipbuf[j] = iptemp;
			}
		}
	for(DWORD32 dwIndex=0; dwIndex<dwDeviceCount; dwIndex++)
	{
		strIP.Format("%d.%d.%d.%d", (ipbuf[dwIndex].ip>>24)&0xFF,
			(ipbuf[dwIndex].ip>>16)&0xFF, (ipbuf[dwIndex].ip>>8)&0xFF, (ipbuf[dwIndex].ip)&0xFF);
		m_ListDevice.InsertItem(dwIndex, "", 0);
		m_ListDevice.SetItemText(dwIndex, 0, strIP.GetBuffer());
		char sz[68],sztemp[68];
		HVAPI_GetDeviceInfoEx(ipbuf[dwIndex].index, sz, 68);
		int i = (int)strlen(sz);
		if((strstr(sz, "LS")
			|| strstr(sz, "HVCAM")
			|| strstr(sz, "HLPR"))
			&& !strstr(sz, sztemp))
		{
			m_ListDevice.SetItemText(dwIndex, 1, sz);
			strcpy(sztemp ,sz);
		}
		else
		{
			m_ListDevice.SetItemText(dwIndex, 1, "NULL");
			strcpy(sztemp ,sz);
		}
	}

	m_iTotleDeveci = dwDeviceCount;
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

	GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
}

void CHvDeviceNewTestDlg::OnBnClickedButton2()
{

	if(m_hDevice || m_iTotleDeveci <= 0)
	{
		return;
	}

	int iCurrentIndex = m_ListDevice.GetSelectionMark();
	if(iCurrentIndex<0 || iCurrentIndex>m_iTotleDeveci)
	{
		MessageBox("未选中记录!", "Tip", MB_OK);
		return;
	}
	char szIP[20] = {0};
	m_ListDevice.GetItemText(iCurrentIndex, 0, szIP, 20);
	m_hDevice = HVAPI_OpenEx(szIP, NULL);
	if(m_hDevice == NULL)
	{
		MessageBox("连接设备失败!", "ERROR", MB_OK|MB_ICONERROR);
		return ;
	}
	else
	{
		MessageBox("连接设备成功!");
		//加亮连接成功行
		m_BeforeIndex = iCurrentIndex;
		m_ListDevice.SetItemState(m_BeforeIndex,LVIS_DROPHILITED,LVIS_DROPHILITED);

	}
	int iVersion = 0 ; 
	bool Version = false;
	if(S_OK == (HVAPI_GetXmlVersionEx(m_hDevice, (PROTOCOL_VERSION*)&iVersion)))
	{
		Version = iVersion!=0;
		if(true == Version)
		{
			m_ListDevice.SetItemText(iCurrentIndex, 3, "XML2.0");
		}
		else
		{
			m_ListDevice.SetItemText(iCurrentIndex, 3, "XML1.0");
		}
	}
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);

	GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON8)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON9)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON10)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON18)->EnableWindow(TRUE);


	g_pList = &m_ListDevice;
	g_dwCurrentConnectIndex = iCurrentIndex;
	//int iBufLen = 1024*1024;
	//char* pszRetInfo = new char[iBufLen];
	//HVAPI_ExecCmdEx(m_hDevice, "GetShutter;GetGain;GetRgbGain;GetCaptureShutter;GetCaptureGain;GetCaptureRgbGain;GetENetSyn;GetEncodeMode;GetAgcLightBaseline;GetGammaData;GetAGCZone;GetCaptureEdge;GetAGCEnable;GetAWBEnable;GetAGCParam;GetJpegCompressRate;GetJpegCompressRateCapture;GetFlashRateSynSignalEnable;GetCaptureSynSignalEnable",
	//	pszRetInfo, iBufLen, NULL);
	//MessageBox(pszRetInfo);
	//delete[] pszRetInfo;
	//pszRetInfo = NULL;
}

void CHvDeviceNewTestDlg::OnBnClickedButton3()
{
	if(m_hDevice == NULL)
	{
		return;
	}
	HVAPI_CloseEx(m_hDevice);
	//断开设备后加亮恢复正常
	UINT   nState=m_ListDevice.GetItemState(m_BeforeIndex,LVIF_STATE);  
	nState&=!nState;  
	m_ListDevice.SetItemState(m_BeforeIndex,nState,LVIF_STATE); 

	m_hDevice = NULL;
	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);

	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON8)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON10)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON18)->EnableWindow(FALSE);

	MessageBox("断开设备成功!");
	
}

void CHvDeviceNewTestDlg::OnBnClickedButton4()
{
	if(m_hDevice == NULL)
	{
		OnBnClickedButton2();
		if(m_hDevice == NULL)
		{
			return;
		}
	}

	//if(m_pDlg)
	//{
	//	delete m_pDlg;
	//	m_pDlg = NULL;
	//}

	m_pDlg = new CResultRecvTestDlg(&m_hDevice, this);
	m_pDlg->Create(IDD_DIALOG1, this);
	m_pDlg->ShowWindow(SW_SHOW);
}

void CHvDeviceNewTestDlg::OnBnClickedButton8()
{
	
	int iCurrentIndex = m_ListDevice.GetSelectionMark();
	char szIP[20] = {0};
	m_ListDevice.GetItemText(iCurrentIndex, 0, szIP, 20);
	CString str = szIP;

	m_pImageDlg = new CImageRecvTestDlg(&m_hDevice, NULL);
	m_pImageDlg->Create(IDD_DIALOG2, this);
	m_pImageDlg->GetIP(str);
	m_pImageDlg->ShowWindow(SW_SHOW);
}

void CHvDeviceNewTestDlg::OnBnClickedButton9()
{
	m_pVideoDlg = new CVideoRecvTestDlg(&m_hDevice, NULL);
	m_pVideoDlg->Create(IDD_DIALOG3, this);
	m_pVideoDlg->ShowWindow(SW_SHOW);
}


void CHvDeviceNewTestDlg::OnBnClickedButton10()
{
	
	CDevBasicInfo cBasicInfo;
	memset( &cBasicInfo , 0 , sizeof(cBasicInfo ));
	if ( S_OK == HVAPI_GetDevBasicInfo( m_hDevice , &cBasicInfo ) )
	{
		if ( strlen(cBasicInfo.szDevType) > 0)
		{
			m_pNewSDKTestDlg = new CNewSDKTestDialog(m_hDevice, NULL);
			m_pNewSDKTestDlg->Create(IDD_DIALOG14, this);
			m_pNewSDKTestDlg->ShowWindow(SW_SHOW);
			return ;
		}
	}
	

	m_pCmdDlg = new CCmdTestDlg(&m_hDevice, NULL);
	m_pCmdDlg->Create(IDD_DIALOG5, this);
	m_pCmdDlg->ShowWindow(SW_SHOW);
}


void CHvDeviceNewTestDlg::OnBnClickedButton5()
{
	GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);
	DWORD dwCount = m_ListDevice.GetItemCount();
	if(dwCount <= 0)
	{
	//	OnBnClickedButton1();
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
			m_ListDevice.SetItemText(dwIndex, 2, "未知");
			continue;
		}
		INT iDevType = 0;
		if ( strstr(szIP, "111.113") )
		{
			int ii = 0;
		}
		HVAPI_GetDevTypeEx(szIP, &iDevType);
		switch(iDevType)
		{
		case DEV_TYPE_HVCAM_200W:
			m_ListDevice.SetItemText(dwIndex, 2, "一体机200W");
			break;
		case DEV_TYPE_HVCAM_500W:
			m_ListDevice.SetItemText(dwIndex, 2, "一体机500W");
			break;
		case DEV_TYPE_HVSIGLE:
			m_ListDevice.SetItemText(dwIndex, 2, "单板");
			break;
		case DEV_TYPE_HVCAM_SINGLE:
			m_ListDevice.SetItemText(dwIndex, 2, "简化版一体机");
			break;
		case DEV_TYPE_HVCAMERA:
			m_ListDevice.SetItemText(dwIndex, 2, "纯相机");
			break;
	    case DEV_TYPE_HVMERCURY:
		    m_ListDevice.SetItemText(dwIndex, 2, "水星");
		    break;
		case DEV_TYPE_HVEARTH:
			m_ListDevice.SetItemText(dwIndex, 2, "地球");
			break;
		case DEV_TYPE_HVVENUS:
			m_ListDevice.SetItemText(dwIndex, 2, "金星");
			break;
		default:
			m_ListDevice.SetItemText(dwIndex, 2, "未知");
			break;
		}
	}
	GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);
}

CGroupTestDlg*			g_dlgGroup;
void CHvDeviceNewTestDlg::OnBnClickedButton13()
{
	//CGroupTestDlg* dlg;
	//dlg = new CGroupTestDlg(NULL);
	//dlg->Create(IDD_DIALOG8, this);
 //   dlg->ShowWindow(SW_SHOW);

	g_dlgGroup = new CGroupTestDlg(NULL);
	g_dlgGroup->Create(IDD_DIALOG8, this);
    g_dlgGroup->ShowWindow(SW_SHOW);
}

void CHvDeviceNewTestDlg::OnBnClickedButton14()
{
	if(m_pOldTestDlg)
	{
		delete m_pOldTestDlg;
		m_pOldTestDlg = NULL;
	}
	m_pOldTestDlg = new COldHvDeviceTestDlg(NULL);
	m_pOldTestDlg->Create(IDD_HVDEVICENEWTEST_DIALOG, this);
	m_pOldTestDlg->ShowWindow(SW_SHOW);
}


void CHvDeviceNewTestDlg::OnBnClickedButton16()
{
	COldGroupTestDlg Dlg(this);
	Dlg.DoModal();
}

void CHvDeviceNewTestDlg::OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult)
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
	cMenu.AppendMenu(MF_BYCOMMAND | MF_STRING, ID_CAPTURE_IMAGE, "抓拍");

	
	CPoint cPt;
	GetCursorPos(&cPt);
	cMenu.TrackPopupMenu(TPM_LEFTALIGN, cPt.x, cPt.y, this);
	*pResult = 0;
}

void CHvDeviceNewTestDlg::OnGetDevType()
{
	int iCurrentSelect = m_ListDevice.GetSelectionMark();
	if(iCurrentSelect < 0 || iCurrentSelect > m_iTotleDeveci)
	{
		MessageBox("未选择设备，获取失败", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}

	char szIP[20] = {0};
	m_ListDevice.GetItemText(iCurrentSelect, 0, szIP, 20);

	INT iDevType = DEV_TYPE_UNKNOWN;
	if(HVAPI_GetDevTypeEx(szIP, &iDevType) != S_OK)
	{
		MessageBox("获取设备类型失败", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	switch(iDevType)
	{
	case DEV_TYPE_HVCAM_200W:
		m_ListDevice.SetItemText(iCurrentSelect, 2, "一体机200W");
		break;
	case DEV_TYPE_HVCAM_500W:
		m_ListDevice.SetItemText(iCurrentSelect, 2, "一体机500W");
		break;
	case DEV_TYPE_HVSIGLE:
		m_ListDevice.SetItemText(iCurrentSelect, 2, "单板");
		break;
	case DEV_TYPE_HVCAM_SINGLE:
		m_ListDevice.SetItemText(iCurrentSelect, 2, "简化版一体机");
		break;
	case DEV_TYPE_HVCAMERA:
		m_ListDevice.SetItemText(iCurrentSelect, 2, "纯相机");
		break;
	case DEV_TYPE_HVMERCURY:
		m_ListDevice.SetItemText(iCurrentSelect, 2, "水星");
		break;
	case DEV_TYPE_HVEARTH:
		m_ListDevice.SetItemText(iCurrentSelect, 2, "地球");
		break;
	case DEV_TYPE_HVVENUS:
		m_ListDevice.SetItemText(iCurrentSelect, 2, "金星");
		break;
	default:
		m_ListDevice.SetItemText(iCurrentSelect, 2, "未知");
		break;
	}
}

BOOL IsCharacter(const char ch)
{
	int i = ch;
	if ((i >= 97 && i <= 122) 
		|| (i >= 48 && i <= 57) 
		|| (i >= 65 && i <= 90))
	{
		return TRUE;
	}
	else if (ch == ' ')
	{
		return TRUE;
	}

	return FALSE;
}

static unsigned char font_test[] = {
		0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x03,0x00,0x08,0x01,0x00,0x08,0x01,0x00,0x08,
		0x01,0x00,0x08,0x01,0x00,0x08,0x01,0x00,0xFF,0xFF,0x00,0x08,0x01,0x00,0x08,0x01,
		0x00,0x08,0x01,0x00,0x08,0x01,0x00,0x08,0x01,0x00,0xFC,0x03,0x00,0x08,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00//"中",0*

		//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x7F,0x00,0x00,0x20,0x00,0x00,
		//0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0xFF,0x3F,0x00,0x00,0x20,0x00,0x00,0x20,
		//0x00,0x00,0x20,0x00,0x00,0x20,0x00,0x00,0x20,0x00,0xF0,0x7F,0x00,0x00,0x00,0x00,
		//0x00,0x00,0x00,0x00,0x00,0x00,//"山",1

		//0x00,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0xE4,0xFF,0x00,0x24,0x00,0x00,0x24,
		//0x09,0x00,0x64,0x09,0x00,0xA4,0x09,0x00,0x3F,0x7F,0x00,0x24,0x09,0x00,0xA4,0x09,
		//0x00,0x64,0x09,0x00,0x24,0x49,0x00,0x24,0x80,0x00,0xE4,0x7F,0x00,0x06,0x00,0x00,
		//0x04,0x00,0x00,0x00,0x00,0x00,//"南",2

		//0x00,0x00,0x00,0x42,0x10,0x00,0x42,0x10,0x00,0x42,0x08,0x00,0xFE,0x0F,0x00,0x42,
		//0x04,0x00,0x42,0x04,0x00,0x00,0x02,0x00,0x02,0x01,0x00,0x82,0x00,0x00,0x42,0x00,
		//0x00,0xF2,0xFF,0x00,0x0E,0x00,0x00,0x82,0x00,0x00,0x02,0x01,0x00,0x03,0x03,0x00,
		//0x02,0x06,0x00,0x00,0x00,0x00,//"环",3

		//0x00,0x00,0x00,0x00,0x40,0x00,0x7E,0xFF,0x00,0x22,0x40,0x00,0xE2,0x7F,0x00,0x22,
		//0x21,0x00,0x3E,0x21,0x00,0x00,0x04,0x00,0x20,0x04,0x00,0x18,0xFE,0x00,0x17,0x45,
		//0x00,0xA4,0x44,0x00,0x44,0x44,0x00,0xA4,0x44,0x00,0x1C,0xFD,0x00,0x04,0x03,0x00,
		//0x00,0x01,0x00,0x00,0x00,0x00,//"路",4

		//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x3F,0x00,0x04,
		//0x10,0x00,0x04,0x10,0x00,0x04,0x10,0x00,0x04,0x10,0x00,0x04,0x10,0x00,0x04,0x10,
		//0x00,0x04,0x10,0x00,0x04,0x10,0x00,0x04,0x10,0x00,0xFE,0x3F,0x00,0x04,0x00,0x00,
		//0x00,0x00,0x00,0x00,0x00,0x00,//"口",5
};


void CHvDeviceNewTestDlg::OnBnClickedButton12()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CDC cTmpDC;
	cTmpDC.CreateCompatibleDC(NULL);
	int nCharacterNum, nSize;
	CString strEdit;
	GetDlgItemText(IDC_EDIT5, strEdit);
	nSize = atoi(strEdit);
	GetDlgItemText(IDC_EDIT_CHAR, strEdit);
	//strEdit = "国木1A";
	char* psz = strEdit.GetBuffer();
	nCharacterNum = (int)strlen(psz);

	int nCharacterLen = (int)(nCharacterNum * nSize);

	CBitmap MemBitmap;
	MemBitmap.CreateCompatibleBitmap(&cTmpDC, nCharacterLen, nSize);
	cTmpDC.SelectObject(&MemBitmap);
	cTmpDC.FillSolidRect(0, 0,nCharacterLen, nSize, cTmpDC.GetBkColor());

	CFont cFont;
	cFont.CreateFont(nSize, 0, 0, 0, /*FW_BOLD*/FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("宋体"));

	cTmpDC.SelectObject(&cFont);
	CString str = "国";
	CSize size = cTmpDC.GetTextExtent(str);

	CPaintDC dc(this);
	dc.SelectObject(&cFont);
	int iCharDataLen = nSize % 8;
	if (iCharDataLen)
	{
		iCharDataLen = nSize / 8 + 1;
	}
	else
	{
		iCharDataLen = nSize / 8;
	}

	int iTmp = iCharDataLen * (nCharacterLen / 2);
	PBYTE pszDate= new BYTE[iTmp];
	PBYTE pszTmp;
	pszTmp = pszDate;
	memset(pszDate, 0, iTmp);
	unsigned char bChar[72] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x10,0x30,0x00,0x10,0x3F,0x00,0x10,
		0x21,0x00,0x10,0x22,0x10,0x10,0x22,0x20,0x10,0x22,0x60,0x10,0x22,0x60,0x10,0x22,
		0x38,0xF8,0xE3,0x0F,0x08,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0xFF,0x7F,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	//int iCharFlag = -1;
	//int nw = 18, nh = 18, nIndex, nRemain, nByteC = 0;
	BYTE rgb[8] = {1, 2, 4, 8, 16, 32, 64, 128};
	COLORREF color = 0;

	int nRow, nColumn;
	nRow = nColumn = nSize;
	CString strTextOut;
	CString strValue;
	strValue.Format("%s", psz);
	int iIndex = 0;
	pszTmp = pszDate;

	HFONT hf = CreateFont(nSize, 0, 0, 0, /*FW_BOLD*/FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("宋体"));
	//HDC hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL); 
	HDC hdcCompatible = CreateCompatibleDC(NULL);
	SelectObject(hdcCompatible, hf);

	HBITMAP hbmScreen = CreateCompatibleBitmap(hdcCompatible, nSize, nSize); 

	//if (hbmScreen == 0) 
	//	MessageBox(NULL, "123", "Warning", MB_OKCANCEL);

	// Select the bitmaps into the compatible DC. 
	//SetBkColor(hdcCompatible, GetBkColor(hdcCompatible));

	if (!SelectObject(hdcCompatible, hbmScreen)) 
		MessageBox("123", "Warning", MB_OKCANCEL);

	int ii = 0;
	for (int i = 0; i < nCharacterNum; i++)
	{
		BOOL bChar = IsCharacter(psz[i]);
		if (bChar)
		{
			nColumn = nSize / 2;
			strTextOut.Format("%c", psz[i]);
		}
		else
		{
			nColumn = nSize;
			strTextOut = strValue.Mid(i, 2);
			i++;
		}

		//cTmpDC.TextOut(0, 0, strTextOut);
		TextOut(hdcCompatible, 0, 0, "国", 2);

		for (int ic = 0; ic < nColumn; ic++)
		{
			for (int ir = 0; ir < nRow; ir++)
			{
				int iFlag = (ir + 1) % 8;
				if (!iFlag && ir != (nRow - 1))
				{
					pszTmp++;
					ii++;
				}

				//COLORREF colorTmp = cTmpDC.GetPixel(ic, ir);
				COLORREF colorTmp = GetPixel(hdcCompatible, ic, ir);
				if (colorTmp == color)
				{
					*pszTmp = *pszTmp | rgb[iFlag];
				}
			}
			pszTmp++;
			ii++;
		}
	}

	int R,G,B;
	R = G = B = 255;
	CString str22;
	GetDlgItemText(IDC_EDIT2, str22);
	R = atoi(str22);
	GetDlgItemText(IDC_EDIT3, str22);
	G = atoi(str22);
	GetDlgItemText(IDC_EDIT4, str22);
	B = atoi(str22);

	//int y = (int)( 0.299   * R + 0.587   * G + 0.114   * B);
	//int cb = (int)(-0.16874 * R - 0.33126 * G + 0.50000 * B + 128);
	//if (cb < 0)
	//	cb = 0;
	//int cr = (int)( 0.50000 * R - 0.41869 * G - 0.08131 * B + 128);
	//if (cr < 0)
	//	cr = 0;
	/*
	FILE* pfile;
	pfile = fopen("F:\\Src_main6467\\LPR\\src\\HvDeviceNewTest\\Debug\\123.txt", "ab");
	if (pfile)
	{
	for (int i = 0; i < iTmp; i++)
	{
	int ii = pszDate[i];
	CString str11;

	str11.Format("0x%x,", ii);
	ii = str11.GetLength();
	fwrite(str11.GetBuffer(), 1, ii, pfile);
	pszDate ++;
	}

	fclose(pfile);
	}*/

	int is = sizeof(font_test);
	HRESULT rs;
//	nSize = 32;
	//rs = HVAPIUTILS_SetCharacterValue(m_hDevice, (char*)pszDate, iTmp, 90, 90, 0, nSize, R, G, B);
	rs = HVAPIUTILS_SetCharacterValue(m_hDevice, psz, 90, 90, 0, nSize, R, G, B);
	if(rs == S_OK)
	{
		MessageBox("字符叠加设置成功");
	}
	else
	{
		MessageBox("字符叠加设置失败");
	}
	//rs = HVAPIUTILS_SetCharacterValue(m_hDevice, psz, iTmp, 90, 90, 0, nSize, R, G, B);
	delete[] pszDate;
	//rs = HVAPIUTILS_SetCharacterValue(m_hDevice, bChar, sizeof(bChar), 90, 90, 0, 24, R, G, B);
	//rs = HVAPIUTILS_SetCharacterValue(m_hDevice, font_test, sizeof(font_test), 90, 90, 0, 18, R, G, B);
	//HVAPI_ExecCmd()
}


void CHvDeviceNewTestDlg::OnBnClickedButton18()
{
	// TODO: Add your control notification handler code here
	if(m_hDevice == NULL) 
	{
		MessageBox("未连接识别器，无法升级");
		return;
	}

	char szWorkStatus[20] = {0};
	char szRetBuf[128*1024] = {0};
	int iRetLen = 0;
	int m_dvetype = -1;
	if(HVAPI_ExecCmdEx(m_hDevice, "GetControllPannelAutoRunStle", 
		szRetBuf, sizeof(szRetBuf), &iRetLen) != S_OK)
	{
		m_dvetype = -1;
		::AfxMessageBox("控制板类型：未知");
		return;
	}
	int iVersion = 0;
	bool fIsNewProtol = false;
	HVAPI_GetXmlVersionEx(m_hDevice, (PROTOCOL_VERSION*)&iVersion);
	fIsNewProtol = iVersion!=0;
	if(HVAPIUTILS_GetExeCmdRetInfoEx(fIsNewProtol, szRetBuf, "GetControllPannelAutoRunStle", "Style", szWorkStatus) != S_OK)
	{
		m_dvetype = -1;
		::AfxMessageBox("控制板类型：未知");
		return;
	}

	m_dvetype = atoi(szWorkStatus);
	if(m_dvetype == 0)
	{
		::AfxMessageBox("控制板类型：拨码板,不能操作");
		return ;
	}
//	//if(m_dvetype != 1)
//	//{
//	//	MessageBox("非数控板无法设置运行模式", "INFO", MB_OK);
//	//	return;
//	//}
	m_pupdataDlg = new CUpdateDlg(&m_hDevice, NULL);
	m_pupdataDlg->Create(IDD_DIALOG13, this);
	m_pupdataDlg->ShowWindow(SW_SHOW);
}

void CHvDeviceNewTestDlg::OnSetIP()
{
	int iCurrentSelect = m_ListDevice.GetSelectionMark();
	if(iCurrentSelect < 0 || iCurrentSelect > m_iTotleDeveci)
	{
		MessageBox("未选择设备，获取失败", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	IPInfo cTmpIPInfo;

	if(FAILED(HVAPI_GetDeviceAddr(ipbuf[iCurrentSelect].index, &cTmpIPInfo.dw64Mac, &cTmpIPInfo.dwIP, 
		&cTmpIPInfo.dwMask, &cTmpIPInfo.dwGateWay)))
	{
		MessageBox("获取设备MAC地址失败，设置失败!");
		return;
	}

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
		if(HVAPI_SetIPByMacAddr(cSetIPInfo.dw64Mac, cSetIPInfo.dwIP, cSetIPInfo.dwMask, cSetIPInfo.dwGateWay) != S_OK)
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


//CAutoLink_demoDlg*		g_dlgLink;
void CHvDeviceNewTestDlg::OnBnClickedButton15()
{
	// TODO: Add your control notification handler code here
	CAutoLink_demoDlg* dlg;
	dlg = new CAutoLink_demoDlg(NULL);
	dlg->Create(IDD_AUTOLINK_DEMO_DIALOG,this);
	dlg->ShowWindow(SW_SHOW);
	m_autolinkDlg = dlg;

	//g_dlgLink = new CAutoLink_demoDlg(NULL);
	//g_dlgLink->Create(IDD_AUTOLINK_DEMO_DIALOG,this);
	//g_dlgLink->ShowWindow(SW_SHOW);
	//m_autolinkDlg = g_dlgLink;

}


void CHvDeviceNewTestDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
//	if(m_autolinkDlg != NULL)
//		m_autolinkDlg->OnClose();
	CDialog::OnClose();
}


//////////////////
void CHvDeviceNewTestDlg::OnOK()
{
    // 防止按回车退出
}


BOOL CHvDeviceNewTestDlg::PreTranslateMessage(MSG* pMsg) 
 {

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		 return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
 } 



 void CHvDeviceNewTestDlg::OnBnClickedButtonAddStaticIp()
 {
	 // TODO: Add your control notification handler code here
	 CString strParam2 ;
	 GetDlgItemText(IDC_EDIT_CHAR, strParam2);	 
	 if ("" == strParam2)
	 {
		 //strParam2="172.18.88.118";
		 strParam2="172.18.10.188";
	 }

	 m_hDevice = HVAPI_OpenEx(strParam2.GetBuffer(), NULL);
	 if(m_hDevice == NULL)
	 {
		 MessageBox("连接设备失败!", "ERROR", MB_OK|MB_ICONERROR);
		 return ;
	 }
	 else
	 {
		 MessageBox("连接设备成功!");
		 //加亮连接成功行



	 }
	 strParam2.ReleaseBuffer();
	 int iVersion = 0 ; 
	 bool Version = false;
	 if(S_OK == (HVAPI_GetXmlVersionEx(m_hDevice, (PROTOCOL_VERSION*)&iVersion)))
	 {
		 Version = iVersion!=0;

	 }
	 GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	 GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);

	 GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);
	 GetDlgItem(IDC_BUTTON8)->EnableWindow(TRUE);
	 GetDlgItem(IDC_BUTTON9)->EnableWindow(TRUE);
	 GetDlgItem(IDC_BUTTON10)->EnableWindow(TRUE);
	 GetDlgItem(IDC_BUTTON18)->EnableWindow(TRUE);


	 g_pList = &m_ListDevice;
 }

void CHvDeviceNewTestDlg::OnCaptureImage()
 {

	 if ( m_hDevice != NULL )
	 {
		 HRESULT hr = E_FAIL;

	//增加了不使用软触发命令也能抓拍的方法， 在这里直接掉用该方法， 不再使用软触发了。如果不成功，再使用软触发。

		INT iBuffLen = 1024 * 1024;
		INT iImageLen = 0;
		BYTE *pBuffImage = new BYTE[iBuffLen];
		memset(pBuffImage, 0, iBuffLen);

		DWORD64 dwTime64 = 0;
		DWORD dwWidth = 0, dwHeigh = 0;

		CString file_name = "";

		if ( HVAPI_GetCaptureImageEx(m_hDevice,0, pBuffImage, iBuffLen, &iImageLen, &dwTime64,  &dwWidth, &dwHeigh) == S_OK  && iImageLen > 0  )
		{
			//保存图片
			//保存图片到硬盘
			CTime tm(dwTime64/1000);
			DWORD dwTemp = dwTime64%1000;

			
			file_name.Format("D:\\Result\\%s\\%s.jpeg", tm.Format("%Y%m%d"), tm.Format("%H%M%S"), dwTemp );

			MakeSureDirectoryPathExists(file_name.GetBuffer());

			file_name.ReleaseBuffer();
			FILE *myFile = fopen(file_name, "wb");
			if ( myFile != NULL )
			{
				fwrite(pBuffImage, 1, iImageLen, myFile);
				fclose(myFile);
				myFile = NULL;
				hr = S_OK;
			}
			else
			{
				hr = E_FAIL;
			}
		}

		if ( pBuffImage != NULL )
		{
			delete[] pBuffImage;
			pBuffImage = NULL;
		}

		if ( hr == S_OK )
		{
			AfxMessageBox(file_name);
		}
		else
		{
			 AfxMessageBox("未保存");
		}
	 }
	 else
	 {
		 AfxMessageBox("设备未连接");
	 }

	 return ;
 }


 void CHvDeviceNewTestDlg::OnBnClickedButtonCaptueimage()
 {
	 // TODO: Add your control notification handler code here

	if ( m_hDevice != NULL )
	 {
		 HRESULT hr = E_FAIL;

	//增加了不使用软触发命令也能抓拍的方法， 在这里直接掉用该方法， 不再使用软触发了。如果不成功，再使用软触发。

		INT iBuffLen = 1024 * 1024;
		INT iImageLen = 0;
		BYTE *pBuffImage = new BYTE[iBuffLen];
		memset(pBuffImage, 0, iBuffLen);

		DWORD64 dwTime64 = 0;
		DWORD dwWidth = 0, dwHeigh = 0;

		CString file_name = "";

		if ( HVAPI_GetCaptureImageEx(m_hDevice,0, pBuffImage, iBuffLen, &iImageLen, &dwTime64,  &dwWidth, &dwHeigh) == S_OK  && iImageLen > 0  )
		{
			//保存图片
			//保存图片到硬盘
			CTime tm(dwTime64/1000);
			DWORD dwTemp = dwTime64%1000;

			
			file_name.Format("D:\\Result\\%s\\%s.jpeg", tm.Format("%Y%m%d"), tm.Format("%H%M%S"), dwTemp );

			MakeSureDirectoryPathExists(file_name.GetBuffer());

			file_name.ReleaseBuffer();
			FILE *myFile = fopen(file_name, "wb");
			if ( myFile != NULL )
			{
				fwrite(pBuffImage, 1, iImageLen, myFile);
				fclose(myFile);
				myFile = NULL;
				hr = S_OK;
			}
			else
			{
				hr = E_FAIL;
			}
		}

		if ( pBuffImage != NULL )
		{
			delete[] pBuffImage;
			pBuffImage = NULL;
		}
	 }

 }
