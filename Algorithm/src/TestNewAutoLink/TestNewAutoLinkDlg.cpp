// TestNewAutoLinkDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestNewAutoLink.h"
#include "TestNewAutoLinkDlg.h"
#include ".\testnewautolinkdlg.h"


#define TE_TIME 1
#define TI_TIME 5000

#pragma comment(lib, "HvDevice.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//得到程序当前目录
CString GetCurrentDir(void)
{
	CString strPath;

	TCHAR szFileName[ MAX_PATH ] = {0};
	TCHAR szIniName[MAX_PATH] = {0};
	TCHAR szBakName[MAX_PATH] = {0};
	GetModuleFileName( NULL, szFileName, MAX_PATH );	//取得包括程序名的全路径
	PathRemoveFileSpec( szFileName );				//去掉程序名

	strPath = szFileName;
	strPath += "\\";
	return strPath;
}




static int HVAPI_AL_OnLine_Notice(PVOID pUserData, HVAPI_OPERATE_HANDLE handle, char*szDeviceNo, char*szAddr,int iPort,char *szOtherXmlInfo)
{
	if ( pUserData== NULL )
		return 0;

	CTestNewAutoLinkDlg *pThis = (CTestNewAutoLinkDlg*)pUserData;
	return pThis->OnLineNotice(handle, szDeviceNo, szAddr,iPort,szOtherXmlInfo);

	return 0;
}


static int HVAPI_AL_OffLine_Notice(PVOID pUserData, HVAPI_OPERATE_HANDLE handle, char* szDeviceNo)
{
	if ( pUserData== NULL )
		return 0;

	CTestNewAutoLinkDlg *pThis = (CTestNewAutoLinkDlg*)pUserData;
	return pThis->OffLineNotice(handle, szDeviceNo);

	return 0;
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTestNewAutoLinkDlg 对话框



CTestNewAutoLinkDlg::CTestNewAutoLinkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestNewAutoLinkDlg::IDD, pParent)
	, m_uiTimeM(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


CTestNewAutoLinkDlg::~CTestNewAutoLinkDlg()
{
	

	m_mtDevcieList.Lock();

	DWORD dwCount = m_DeviceArray.GetCount();

	for( int i=0; i<dwCount; i++)
	{
		POSITION p = m_DeviceArray.FindIndex(i);
		CDevice* pTemppDevice = m_DeviceArray.GetAt(p);

		delete  pTemppDevice;
		pTemppDevice = NULL;
	}

	m_mtDevcieList.Unlock();

	HVAPI_AL_UnInitActiveModule();

}


void CTestNewAutoLinkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEVICELIST, m_deviceList);
	DDX_Text(pDX, IDC_EDIT_TIME, m_uiTimeM);
	DDX_Control(pDX, IDC_DATETIMEPICKER_STARTY, m_dtStartY);
	DDX_Control(pDX, IDC_DATETIMEPICKER_ENDY, m_dtEndY);
}

BEGIN_MESSAGE_MAP(CTestNewAutoLinkDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_STARTSERVER, OnBnClickedButtonStartserver)
	ON_BN_CLICKED(IDC_BUTTON_CLOSESERVER, OnBnClickedButtonCloseserver)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DEVICELIST, OnNMClickListDevicelist)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DEVICELIST, OnNMRclickListDevicelist)
	ON_COMMAND(ID_GETXML, OnGetxml)
	ON_COMMAND(ID_SYNCTIME, OnSynctime)
	ON_COMMAND(ID_GETDEVICETIME, OnGetdevicetime)
	ON_COMMAND(ID_GETTRAFICCINFO, OnGettraficcinfo)
	ON_COMMAND(ID_CAPTUREIMAGE, OnCaptureimage)
	ON_COMMAND(ID_GETVIDEO, OnGetvideo)
	ON_COMMAND(ID_STOPVIDEO, OnStopvideo)
	ON_COMMAND(ID_GETJPEG, OnGetjpeg)
	ON_COMMAND(ID_STIPJPEG, OnStipjpeg)
	ON_COMMAND(ID_UPLOADXML, OnUploadxml)
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CTestNewAutoLinkDlg 消息处理程序

BOOL CTestNewAutoLinkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码


	GetDlgItem(IDC_EDIT_PORT)->SetWindowText("8888");
	InitDevListCtrl();

	/*
	SYSTEMTIME st;
    GetSystemTime(&st);
    SystemTimeToTzSpecificLocalTime(NULL, &st, &st);  //时间转换 */

	m_dtStartY.SetFormat("yyyy-MM-dd   HH:mm"); 
	m_dtEndY.SetFormat("yyyy-MM-dd   HH:mm");  


	CTime tm = CTime::GetCurrentTime();
	
	DWORD64 dwTime = tm.GetTime();
	
	int iTemp = 60 * 60;
	dwTime-= iTemp;

	CTime tStart(dwTime);

	int i = CDevice::GetDiskFree();




	m_dtStartY.SetTime(&tStart);
	m_dtEndY.SetTime(&tm);

	SetTimer(1, 5000, NULL);

	HVAPI_AL_InitActiveModule();
	HVAPI_AL_RegDevOnLineNotice(HVAPI_AL_OnLine_Notice, this);
	HVAPI_AL_RegDevOffLineNotice(HVAPI_AL_OffLine_Notice, this);

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}


void CTestNewAutoLinkDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


// 初始化列表框
HRESULT CTestNewAutoLinkDlg::InitDevListCtrl()
{
	m_deviceList.DeleteAllItems();

	LVCOLUMN lv;
	lv. mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH ;
	lv. fmt = LVCFMT_CENTER ;
	lv.cx = 120;

	lv.pszText = "设备编号";
	lv.cchTextMax = sizeof(lv.pszText);
	lv.iSubItem = 0;
	lv.iOrder = 0;
	m_deviceList.InsertColumn(0, &lv);

	lv.pszText = "设置IP";
	lv.cchTextMax = sizeof(lv.pszText);
	lv.iSubItem = 1;
	lv.iOrder =1;

	m_deviceList.InsertColumn(1, &lv);

 	lv.pszText = "端口";
 	lv.cchTextMax = sizeof(lv.pszText);
 	lv.iSubItem = 2;
 	lv.iOrder = 2;
 
 	m_deviceList.InsertColumn(2, &lv);

	lv.pszText = "状态";
 	lv.cchTextMax = sizeof(lv.pszText);
 	lv.iSubItem = 3;
 	lv.iOrder = 3;
 
 	m_deviceList.InsertColumn(3, &lv);

	lv.pszText = "H264状态";
 	lv.cchTextMax = sizeof(lv.pszText);
 	lv.iSubItem = 4;
 	lv.iOrder = 4;
 
 	m_deviceList.InsertColumn(4, &lv);

	lv.pszText = "JPEG状态";
 	lv.cchTextMax = sizeof(lv.pszText);
 	lv.iSubItem = 5;
 	lv.iOrder = 5;
 
 	m_deviceList.InsertColumn(5, &lv);



	m_deviceList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return 0;
}


// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestNewAutoLinkDlg::OnPaint() 
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
HCURSOR CTestNewAutoLinkDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestNewAutoLinkDlg::OnBnClickedButtonStartserver()
{
	// TODO: Add your control notification handler code here
	
	CString strText="";
	GetDlgItem(IDC_EDIT_PORT)->GetWindowText(strText);

	int iPort = atoi(strText);
	
	if ( HVAPI_AL_OpenDevCtrlServer(iPort, 50) == S_OK )
	{
		AfxMessageBox("服务器启动成功");
		this->SetWindowText("服务器已启动");
	}
	else
	{
		AfxMessageBox("服务器启动失败");
		
	}
	
}


void CTestNewAutoLinkDlg::OnBnClickedButtonCloseserver()
{
	// TODO: Add your control notification handler code here

	if ( HVAPI_AL_CloseDevCtrlServer() == S_OK )
	{
		AfxMessageBox("服务器关闭成功");
		this->SetWindowText("服务器关闭");
	}
	else
	{
		AfxMessageBox("服务器关闭失败");
	}


	/*
	m_mtDevcieList.Lock();
	DWORD dwCount = m_DeviceArray.GetCount();


	for( int i=0; i<dwCount; i++)
	{
		POSITION p = m_DeviceArray.FindIndex(i);
		CDevice*pDevice = m_DeviceArray.GetAt(p);
		if ( pDevice != NULL )
		{
			delete pDevice;
			pDevice = NULL;
		}

		
	}

	m_DeviceArray.RemoveAll();

	m_mtDevcieList.Unlock();

	m_deviceList.DeleteAllItems();*/
	
}


//设备上线
int CTestNewAutoLinkDlg::OnLineNotice(HVAPI_OPERATE_HANDLE handle, char*szDeviceNo, char*szAddr,int iPort,char *szOtherXmlInfo)
{
	//查找是否已经有保存此设备编号的操作句柄（一般为）

	CString str;
	str.Format("0x%0x", handle);

	CString strText="";
	GetDlgItem(IDC_EDIT_MSG)->GetWindowText(strText);

	if ( strText.GetLength() > 1024 * 5)
	{
		strText = "";
		
	}

	strText = strText +  "\r\n设备上线:" +  szDeviceNo + "_" +  szAddr+ "_" + str;
	GetDlgItem(IDC_EDIT_MSG)->SetWindowText(strText);


	CDevice*pDevice = NULL;

	m_mtDevcieList.Lock();

	DWORD dwCount = m_DeviceArray.GetCount();
	for( int i=0; i<dwCount; i++)
	{
		POSITION p = m_DeviceArray.FindIndex(i);
		CDevice* pTemppDevice = m_DeviceArray.GetAt(p);

		if ( strcmp(pTemppDevice->GetDeviceNo(), szDeviceNo) == 0  )
		{
			pDevice = pTemppDevice;
			break;
		}
	}

	m_mtDevcieList.Unlock();

	if ( pDevice != NULL )  //已存在为此设备建立服务的对象， 故重用
	{
		pDevice->SetDeviceAddr(szAddr, iPort);
		pDevice->SetDeviceInfo(szOtherXmlInfo);

		if ( pDevice->GetDeviceHandle() != handle)
		{
			HVAPI_AL_AddRef(handle);  //需应用此句柄， 表现上层操作要保存和使用此句柄
			pDevice->SetDeviceHandle(handle);
		}

	}
	else  //如果找不到曾经建立的对象，则说明从没有上线过， 或者说已删除掉了， 重新建立
	{

		pDevice = new CDevice() ;
		HVAPI_AL_AddRef(handle);  //需应用此句柄， 表现上层操作要保存和使用此句柄
		pDevice->SetDeviceInfo(szAddr, iPort, szDeviceNo,szOtherXmlInfo, handle);
		m_DeviceArray.AddTail(pDevice);
	}

	pDevice->SetDeviceStatus(CONNECTED);

	//刷新状态列表框

	m_mtListCtrl.Lock();
	int index = -1;
	int iNetDiskCount = m_deviceList.GetItemCount();
	for (int i = 0; i<iNetDiskCount; i++)
	{
		CString strDevSN = m_deviceList.GetItemText(i, 0);
		if (0 == strcmp(szDeviceNo, strDevSN))
		{
			index = i;
			m_deviceList.SetItem(index, 1, LVIF_TEXT, szAddr, 0, 0, 0 ,0 );
			CString strTemp;
			strTemp.Format("%d", iPort);
			m_deviceList.SetItemText(index, 2, strTemp);
			//
			 strTemp = "连接";
			m_deviceList.SetItemText(index, 3, strTemp);
			break;
		}
	}

	if ( index == -1 )  // 找不到， 说明是新的设备， 加入到列表里面
	{
		iNetDiskCount = m_deviceList.GetItemCount();
		LVITEM  lvitem;

		int iItemRow = m_deviceList.InsertItem(0, szDeviceNo);
		m_deviceList.SetItem(iItemRow, 1, LVIF_TEXT, szAddr, 0, 0, 0 ,0 );
		CString strTemp;
		strTemp.Format("%d", iPort);
		m_deviceList.SetItemText(iItemRow, 2, strTemp);
		strTemp = "连接";
		m_deviceList.SetItemText(iItemRow, 3, strTemp);
	}
	m_mtListCtrl.Unlock();


	return 0;
}

//设备下线通知函数
int CTestNewAutoLinkDlg::OffLineNotice(HVAPI_OPERATE_HANDLE handle, char* szDeviceNo)
{
	CString strText="";
	GetDlgItem(IDC_EDIT_MSG)->GetWindowText(strText);
	if ( strText.GetLength() > 1024 * 5)
	{
		strText = "";
		
	}

	strText = strText + "\r\n设备下线" + szDeviceNo;



	GetDlgItem(IDC_EDIT_MSG)->SetWindowText(strText);

	CDevice*pDevice = NULL;

	m_mtDevcieList.Lock();

	DWORD dwCount = m_DeviceArray.GetCount();
	for( int i=0; i<dwCount; i++)
	{
		POSITION p = m_DeviceArray.FindIndex(i);
		CDevice* pTempDevice = m_DeviceArray.GetAt(p);
		
		if ( pTempDevice->GetDeviceHandle() ==  handle )
		{
			pDevice = pTempDevice;
			//此设备连接已断开，析放其操作句柄， 并设置与此相关联的对象句柄为空和状态
			//pDevice->SetDeviceHandle(NULL);
			pDevice->SetDeviceStatus(DISCONNECT);  //设置状态为断开
			break;
		}
	}

	m_mtDevcieList.Unlock();


	if ( pDevice == NULL )  //说明找不到与此句柄相关联的对象和引用
	{
		HVAPI_AL_ReleaseRef(handle);   //删除此句柄
		return E_FAIL;  
	}


	m_mtListCtrl.Lock();

	int iNetDiskCount = m_deviceList.GetItemCount();

	for (int i = 0; i<iNetDiskCount; i++)
	{
		CString strDevSN = m_deviceList.GetItemText(i, 0);

		if (0 == strcmp(szDeviceNo, strDevSN))
		{
			CString strTemp = "断开";
			m_deviceList.SetItemText(i, 3, strTemp);
			break;
		}
	}

	m_mtListCtrl.Unlock();


	return 0;
}


void CTestNewAutoLinkDlg::OnNMClickListDevicelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	//获取单击所在的行号   
	//找出鼠标位置   
	DWORD dwPos = GetMessagePos();  
	CPoint point( LOWORD(dwPos), HIWORD(dwPos) );  
	m_deviceList.ScreenToClient(&point);  
	  
	//定义结构体   
	LVHITTESTINFO lvinfo;  
	lvinfo.pt = point;  
	  
	//获取行号信息   
	int nItem = m_deviceList.HitTest(&lvinfo);  
	if(nItem != -1)  
		m_itemSel = lvinfo.iItem;   //当前行号  

}

void CTestNewAutoLinkDlg::OnNMRclickListDevicelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

	NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (-1 != pNMListView->iItem)
	{
		CString strDevSN = m_deviceList.GetItemText(pNMListView->iItem , 0);

		int iSubMenuNum = 0;
		CMenu menu, *pSubMenu;
		menu.LoadMenu(IDR_MENU1);
		pSubMenu = menu.GetSubMenu(iSubMenuNum);

		POINT oPoint;
		GetCursorPos(&oPoint);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, oPoint.x, oPoint.y, this);
	}


	*pResult = 0;
}

CDevice* CTestNewAutoLinkDlg::GetSelDevice()
{
	CDevice*pDevice = NULL;
	if ( m_itemSel != -1)
	{
		CString strDevSN = m_deviceList.GetItemText( m_itemSel , 0);

		DWORD dwCount = m_DeviceArray.GetCount();
		
		for( int i=0; i<dwCount; i++)
		{
			POSITION p = m_DeviceArray.FindIndex(i);
			CDevice *pTemppDevice = m_DeviceArray.GetAt(p);
			
			if ( strcmp(pTemppDevice->GetDeviceNo(), strDevSN) == 0  )
			{
				pDevice  = pTemppDevice;
				break;
			}
		}//for

	}

	return pDevice;
}

void CTestNewAutoLinkDlg::OnGetxml()
{
	// TODO: Add your command handler code here

	CDevice*pDevice = GetSelDevice();
	if ( pDevice != NULL )
	{
		if ( pDevice->GetDeviceXml() )
		{
			AfxMessageBox("下载成功");
		}
		else
		{
			AfxMessageBox("下载失败");
		}
	}

}

void CTestNewAutoLinkDlg::OnSynctime()
{
	// TODO: Add your command handler code here

	CDevice*pDevice = GetSelDevice();
	if ( pDevice != NULL )
	{
		if ( pDevice->SyncTime() )
		{
			AfxMessageBox("同步时间成功");
		}
		else
		{
			AfxMessageBox("同步时间失败");
		}
	}

}


void CTestNewAutoLinkDlg::OnGetdevicetime()
{
	// TODO: Add your command handler code here

	CDevice*pDevice = GetSelDevice();
	if ( pDevice != NULL )
	{
		CString strTime = pDevice->GetDeviceTime();
		AfxMessageBox(strTime);
	}
}

void CTestNewAutoLinkDlg::OnGettraficcinfo()
{
	// TODO: Add your command handler code here

	CTime timeStart;
	m_dtStartY. GetTime(timeStart);

	CTime timeEnd;
	m_dtEndY.GetTime(timeEnd);

	if ( timeStart >=timeEnd )
	{
		AfxMessageBox("设置的时间不合适");
		return ;
	}

	CDevice*pDevice = GetSelDevice();

	//m_dtStartY.
	
	
	

	if ( pDevice != NULL )
	{
		DWORD64 dwBeginTime = ((DWORD64)timeStart.GetTime()) * 1000;
		DWORD64 dwEndTime = ((DWORD64)timeEnd.GetTime()) * 1000;
		
		CTime tm = CTime::GetCurrentTime();
		

		//dwEndTime = ((DWORD64)tm.GetTime()) * 1000;
		//一个小时的毫秒
		//dwBeginTime = dwEndTime - 60 * 60 * 1000 * 24  * 6;
		int iRetLen = 1024 * 1024;
		char *szRetInfo = new char[iRetLen];

		memset(szRetInfo, 0, iRetLen);

		
		if ( pDevice->GetHumanTrafficInfo(dwBeginTime, dwEndTime, szRetInfo,  &iRetLen) ) 
		{
			CString strFileName;
			CTime tm = CTime::GetCurrentTime();
			strFileName.Format("%s_info.txt", tm.Format("%Y%m%d%H%M%S"));

			CString strDir = GetCurrentDir()+"TrifficInfo\\";

			CString  *strTemp = new CString(szRetInfo); 

			strTemp->Replace(";", ";\r\n");

			if ( !PathFileExists(strDir) )
			{
				MakeSureDirectoryPathExists(strDir);
				//如果尚不存在，则保存到相应的目录中
			}

			strFileName =  strDir + strFileName;
			FILE *myFile = fopen(strFileName, "w");
			
			if ( myFile )
			{
				int iLen = strTemp->GetLength();

				fwrite(strTemp->GetBuffer(), 1, iLen, myFile);
				fclose(myFile);
				myFile = NULL;
				strTemp->ReleaseBuffer();
			}

			delete strTemp;

			AfxMessageBox("获取流量信息OK"+strFileName);
		}
		else
		{
			AfxMessageBox("获取流量信息失败");
		}

		delete[] szRetInfo;
		szRetInfo = NULL;

		//CString strTime = pDevice->GetDeviceTime(, );
		//AfxMessageBox(strTime);
	}

}

void CTestNewAutoLinkDlg::OnCaptureimage()
{
	// TODO: Add your command handler code here

	UpdateData(TRUE);

	CDevice*pDevice = GetSelDevice();
	if ( pDevice != NULL )
	{
		int iBuffLen = 1024 * 1024;
		int iImageLen = 0;
		BYTE *pTempBuff = new BYTE[iBuffLen];
		memset(pTempBuff, 0, iBuffLen);

		DWORD64 dwTime = 0; 
		DWORD dwWidth = 0;
		DWORD dwHeigh = 0;

		if ( pDevice->GetCaptureImage(m_uiTimeM, pTempBuff, iBuffLen, &iImageLen, &dwTime,&dwWidth, &dwHeigh ))
		{
			CString strFileName;
			CTime tm = CTime::GetCurrentTime();
			strFileName.Format("%s.jpeg", tm.Format("%Y%m%d%H%M%S"));

			CString strDir = GetCurrentDir()+"Capture\\";

			if ( !PathFileExists(strDir) )
			{
				MakeSureDirectoryPathExists(strDir);
				//如果尚不存在，则保存到相应的目录中
			}

			strFileName =  strDir + strFileName;

			FILE *myFile =fopen(strFileName, "wb");
			if (myFile)
			{
				fwrite(pTempBuff, 1, iImageLen, myFile);
				fclose(myFile);
			}

			CTime tImgaeTime(dwTime/1000);
			CString strTemp;
			strTemp.Format("ImgeTime:%s, Width:%d, Heigh:%d", tImgaeTime.Format("%Y%m%d%H%M%S"),dwWidth, dwHeigh);

			AfxMessageBox(strTemp);
		}
		else
		{
			AfxMessageBox("获取图片失败");
		}

		delete[] pTempBuff;
		pTempBuff = NULL;
	}

}

void CTestNewAutoLinkDlg::OnGetvideo()
{
	// TODO: Add your command handler code here

	CDevice*pDevice = GetSelDevice();
	if ( pDevice != NULL )
	{
		

		if ( pDevice->StartVideo())
		{
			AfxMessageBox("请求命令已发送");
		}
		else
		{
			AfxMessageBox("请求命令发送失败");
		}
	}


}

void CTestNewAutoLinkDlg::OnStopvideo()
{
	// TODO: Add your command handler code here

	CDevice*pDevice = GetSelDevice();
	if ( pDevice != NULL )
	{
		

		if ( pDevice->StopVideo())
		{
			AfxMessageBox("停止视频命令已发送");
		}
		else
		{
			AfxMessageBox("停止视频命令发送失败");
		}
	}

}

void CTestNewAutoLinkDlg::OnGetjpeg()
{
	// TODO: Add your command handler code here
	CDevice*pDevice = GetSelDevice();
	if ( pDevice != NULL )
	{
		

		if ( pDevice->StartJpegVideo())
		{
			AfxMessageBox("请求JPEG命令已发送");
		}
		else
		{
			AfxMessageBox("请求JPEG命令发送失败");
		}
	}

}

void CTestNewAutoLinkDlg::OnStipjpeg()
{
	// TODO: Add your command handler code here
	CDevice*pDevice = GetSelDevice();
	if ( pDevice != NULL )
	{
		

		if ( pDevice->StopJpegVideo())
		{
			AfxMessageBox("停止JPEG视频命令已发送");
		}
		else
		{
			AfxMessageBox("停止JPEG视频命令发送失败");
		}
	}

}


//上传配置文件
void CTestNewAutoLinkDlg::OnUploadxml()
{
	// TODO: Add your command handler code here
	CString FilePathName;
    CFileDialog dlg(TRUE, //TRUE为OPEN对话框，FALSE为SAVE AS对话框
        NULL, 
        NULL,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        (LPCTSTR)_TEXT("Xml Files (*.xml)|*.xml|All Files (*.*)|*.*||"),
        NULL);

    if(dlg.DoModal()==IDOK)
    {
        FilePathName=dlg.GetPathName(); //文件名保存在了FilePathName里
    }
    else
    {
         return;
    }



	CDevice*pDevice = GetSelDevice();
	if ( pDevice != NULL )
	{
		

		if ( pDevice->UpLoadDeviceXml(FilePathName))
		{
			AfxMessageBox("上传文件成功, 设备重启");
			pDevice->DeviceReset();
		}
		else
		{
			AfxMessageBox("上传文件失败");
		}
	}

}

void CTestNewAutoLinkDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if ( nIDEvent == 1 )  //更改H264 状态
	{
		//m_mtDevcieList.Lock();

		DWORD dwCount = m_DeviceArray.GetCount();
		for( int i=0; i<dwCount; i++)
		{
			POSITION p = m_DeviceArray.FindIndex(i);
			CDevice* pTemppDevice = m_DeviceArray.GetAt(p);

			char *szDeviceNo = pTemppDevice->GetDeviceNo();

			////
			//m_mtListCtrl.Lock();
			int iNetDiskCount = m_deviceList.GetItemCount();
			for (int j = 0; i<iNetDiskCount; j++)
			{
				CString strDevSN = m_deviceList.GetItemText(j, 0);

				if (0 == strcmp(szDeviceNo, strDevSN))
				{
					DWORD dwStatus  = pTemppDevice->GetDeviceH264Status();
					if ( dwStatus == AL_LINK_STATE_NORMAL)
						m_deviceList.SetItemText(j, 4, "连接");
					else if (dwStatus == AL_LINK_STATE_RECONN )
						m_deviceList.SetItemText(j, 4, "重连");
					else
						m_deviceList.SetItemText(j, 4, "断开");

					dwStatus = pTemppDevice->GetDeviceJpegStatus();
					if ( dwStatus == AL_LINK_STATE_NORMAL)
						m_deviceList.SetItemText(j, 5, "连接");
					else if (dwStatus == AL_LINK_STATE_RECONN )
						m_deviceList.SetItemText(j, 5, "重连");
					else
						m_deviceList.SetItemText(j, 5, "断开");

					
					break;
				}
			}
		//	m_mtListCtrl.Unlock();
			/////	
		}

	//m_mtDevcieList.Unlock();

	}

	CDialog::OnTimer(nIDEvent);
}

void CTestNewAutoLinkDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	KillTimer(1);
	

	CDialog::OnClose();
}
