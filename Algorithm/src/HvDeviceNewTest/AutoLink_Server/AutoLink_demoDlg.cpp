// AutoLink_demoDlg.cpp : 实现文件

#include "stdafx.h"
#include "AutoLink_demo.h"
#include "AutoLink_demoDlg.h"

#include "AutoLinkHistorySet.h"
#include "AutoLinkNameSet.h"
#include "AutoLinkDirSet.h"

#include "Camera.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_DEV_SN_NUM 128
static DWORD WINAPI UpdateClientInfor(LPVOID lpParameter);

CRITICAL_SECTION g_csLog;
CRITICAL_SECTION g_csOperation;
CRITICAL_SECTION g_csDevList;

#define DEV_STATUS_CONN		"连接"
#define DEV_STATUS_RECONN	"重连"
#define DEV_STATUS_DISCONN  "断开"
#define DEV_STATUS_DONE     "完成"
#define DEV_STATUS_UNKNOW   DEV_STATUS_DISCONN

// 循环覆盖
int g_iLoopEnable = 1;  // 循环覆盖开关
int g_iSpace = 10;  // 剩余空间，单位G
int g_iFileCount = 0;  // 保存文件个数
int g_iFileMaxNum = 1000; // 单文件可记录最大数
int g_iFileSaveNum = 0;  // 当前待写入文件记录编号
int g_iFileDeleteNum = 0;  // 当前待删除文件记录编号
char g_chLoopFileDir[MAX_PATH] = {0};  // 循环覆盖文件保存目录

// CAutoLink_demoDlg 对话框

CAutoLink_demoDlg::CAutoLink_demoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoLink_demoDlg::IDD, pParent)
	, m_hShowThread(NULL)
	, m_pIsUsrDiscnn(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoLink_demoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_devListCtrl);
	DDX_Control(pDX, IDC_EDIT1, m_autoLinkPortEdit);
	DDX_Control(pDX, IDC_EDIT2, m_maxListenNumEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_openAutoLinkBut);
	DDX_Control(pDX, IDC_BUTTON2, m_closeAutoLinkBut);
}

BEGIN_MESSAGE_MAP(CAutoLink_demoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_WM_CLOSE()

	ON_BN_CLICKED(IDC_FIND_DIR, OnBnClickedFindDir)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnDBClick)
	ON_NOTIFY(NM_RCLICK, IDC_LIST2, OnRClickDevList)
	ON_COMMAND(ID__RECV_HISTORY_CONFIG, OnSetHistoryConnect)
	ON_BN_CLICKED(IDC_BUTTON_DIR, OnBnClickedButtonDir)
	ON_BN_CLICKED(IDC_BUTTON_FILENAME, OnBnClickedButtonFilename)
END_MESSAGE_MAP()


// CAutoLink_demoDlg 消息处理程序

BOOL CAutoLink_demoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	InitDevListCtrl();
	InitializeCriticalSection (&g_csLog);
	InitializeCriticalSection (&g_csOperation);
	InitializeCriticalSection (&g_csDevList);

	m_hShowThread = NULL;
	m_hDeleteThread = NULL;
	m_iDevMaxNum = 50;
	m_iPort = 6665;

	// 读取界面配置
	ReadDlgIni();
	
	CString strTemp = _T("");
	strTemp.Format("%d", m_iDevMaxNum);
	m_maxListenNumEdit.SetWindowText(strTemp);
	strTemp.Format("%d", m_iPort);
	m_autoLinkPortEdit.SetWindowText(strTemp);
	((CEdit*)GetDlgItem(IDC_DIR))->SetWindowText(m_strDir);

	OnBnClickedButton1();

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAutoLink_demoDlg::OnPaint() 
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
HCURSOR CAutoLink_demoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 判断是否包含数字
bool IsDigitStr( CString str )
{
	bool digitFlag = true;
	int i  = str.GetLength();
	while (i--)
	{
		if (!isdigit(str.GetAt(i)))
		{
			digitFlag = false;
			break;
		}
	}
	return digitFlag;
}

// 初始化列表框
HRESULT CAutoLink_demoDlg::InitDevListCtrl()
{
	m_devListCtrl.DeleteAllItems();

	LVCOLUMN lv;
	lv. mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH ;
	lv. fmt = LVCFMT_CENTER ;
	lv.cx = 120;

	lv.pszText = "设备编号";
	lv.cchTextMax = sizeof(lv.pszText);
	lv.iSubItem = 0;
	lv.iOrder = 0;
	m_devListCtrl.InsertColumn(0, &lv);

	lv.pszText = "状态";
	lv.cchTextMax = sizeof(lv.pszText);
	lv.iSubItem = 1;
	lv.iOrder =1;

	m_devListCtrl.InsertColumn(1, &lv);

 	lv.pszText = "重连次数";
 	lv.cchTextMax = sizeof(lv.pszText);
 	lv.iSubItem = 1;
 	lv.iOrder = 1;
 
 	m_devListCtrl.InsertColumn(2, &lv);

	m_devListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return 0;
}

// 开启监听
void CAutoLink_demoDlg::OnBnClickedButton1()
{
	CString strListenNum;
	m_maxListenNumEdit.GetWindowText(strListenNum);
	int iBufSize = _ttoi(strListenNum);
	m_pIsUsrDiscnn = new int[iBufSize];
	// 初始化设备连接状态
	for (int i = 0; i < iBufSize; ++i)
		m_pIsUsrDiscnn[i] = 0;

	// 判断输入正确性
	CString strMaxListenNum;
	CString strAutoLinkPort;
	CString strError= "Error:";
	HRESULT hr = S_OK;

	m_maxListenNumEdit.GetWindowText(strMaxListenNum);
	if (!IsDigitStr(strMaxListenNum) || strMaxListenNum.IsEmpty())
	{
		strError = "最大监听数：非法值！";
		hr = E_FAIL;
	}
	m_iDevMaxNum = atoi(strMaxListenNum);

	m_autoLinkPortEdit.GetWindowText(strAutoLinkPort);
	if (!IsDigitStr(strAutoLinkPort)|| strAutoLinkPort.IsEmpty())
	{
		strError += "端口号：非法值！";
		hr = E_FAIL;
	}
	m_iPort  = atoi(strAutoLinkPort);

	if (m_iDevMaxNum <= 0 &&  m_iPort<= 0)
	{
		strError = "最大监听数或端口号取值范围：非法值！";
		hr = E_FAIL;
	}

	CString strDir;
	GetDlgItem(IDC_DIR)->GetWindowText(strDir);
	DWORD attr;
	attr = GetFileAttributes(strDir);   
	if( m_Common.IsIllegalDir(strDir) && attr == (DWORD)-1 && attr & FILE_ATTRIBUTE_DIRECTORY)
	{
		strError += "保存目录有误！";
		hr = E_FAIL;
	}
	else
	{
		m_strDir = strDir;
		m_strDir += _T("\\");
		m_strDir.Replace("\\\\", "\\");
	}
	
	if (S_OK != hr)
	{
		AfxMessageBox(strError);
		return ;
	}

	// 写界面配置
	WriteDlgIni();
	
	HVAPI_UnLoadMonitor(); // 卸载监听模块
	// 加载监听模块
	if (S_OK != HVAPI_LoadMonitor(atoi(strMaxListenNum), NULL ))
	{
		AfxMessageBox("LoadMonitor Fail!");
		return ;
	}
	
	// 打开监听服务
	if (S_OK != HVAPI_OpenServer(atoi(strAutoLinkPort), LISTEN_TYPE_RECORD , NULL))
	{
		AfxMessageBox("Open Record Listen Error!");
		return ;
	}

	// 读取设备参数配置
	ReadFileIni();

	// 设备状态监测线程
	if (NULL == m_hShowThread && NULL == m_hDeleteThread)
	{
		m_fExit = FALSE;
		m_hShowThread = CreateThread(NULL, 0, UpdateClientThread, this, 0, NULL);
		m_hDeleteThread = CreateThread(NULL, 0, DeleteThread, this, 0, NULL);
	}

	m_autoLinkPortEdit.EnableWindow(FALSE);
	m_maxListenNumEdit.EnableWindow(FALSE);
	m_openAutoLinkBut.EnableWindow(FALSE);
	m_closeAutoLinkBut.EnableWindow(TRUE);
	GetDlgItem(IDC_DIR)->EnableWindow(FALSE);
	GetDlgItem(IDC_FIND_DIR)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DIR)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_FILENAME)->EnableWindow(FALSE);
}

// 关闭监听
void CAutoLink_demoDlg::OnBnClickedButton2()
{
	m_closeAutoLinkBut.EnableWindow(FALSE);
	// 关闭监测线程
	m_fExit = TRUE;
	if (NULL != m_hShowThread)
	{
		WaitForSingleObject(m_hShowThread, 1200);
		CloseHandle(m_hShowThread);
		m_hShowThread = NULL;
	}
	if (NULL != m_hDeleteThread)
	{
		WaitForSingleObject(m_hDeleteThread, 1200);
		CloseHandle(m_hDeleteThread);
		m_hDeleteThread = NULL;
	}

	// 关闭设备连接
	CCamera* pCameraTemp = NULL;
	POSITION pos = m_connDevList.GetHeadPosition();
	while(pos)
	{
		pCameraTemp = m_connDevList.GetAt(pos);

		if (NULL != pCameraTemp)
		{
			pCameraTemp->Close();
			delete pCameraTemp;
			pCameraTemp = NULL;
		}
		m_connDevList.GetNext(pos);
	}
	m_connDevList.RemoveAll();

	// 关闭监听(该操作会主动去关闭主动连接句柄，必须要放在HVAPI_CloseEx之后，否则会报错)
	HVAPI_CloseServer(LISTEN_TYPE_RECORD);
	HVAPI_UnLoadMonitor();

	if (m_pIsUsrDiscnn != NULL)
	{
		delete [] m_pIsUsrDiscnn;
		m_pIsUsrDiscnn = NULL;
	}

	// 更新设备参数配置
	WriteFileIni();

	m_devListCtrl.DeleteAllItems();
	m_devListCtrl.UpdateWindow();

	m_autoLinkPortEdit.EnableWindow(TRUE);
	m_maxListenNumEdit.EnableWindow(TRUE);
	m_openAutoLinkBut.EnableWindow(TRUE);
	GetDlgItem(IDC_DIR)->EnableWindow(TRUE);
	GetDlgItem(IDC_FIND_DIR)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DIR)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_FILENAME)->EnableWindow(TRUE);
}

// 监测设备状态线程
int CAutoLink_demoDlg::UpdateClient()
{
	char szRecordConStatus[128];
	ZeroMemory(szRecordConStatus, sizeof(szRecordConStatus));

	int nDevListLen = 0;
	int nDevListBufLen = 128 * 100;
	char* pDevListBuf = new char[nDevListBufLen];
	
	char* pszSN = NULL;
	char* pszTemp = NULL;

	while(!m_fExit)
	{
		Sleep(1000);
		
		// 搜索主动连接设备，获取设备列表的长度
		if (S_OK != HVAPI_GetDeviceListSize(nDevListLen, NULL))
		{
			continue;
		}
		if (nDevListBufLen < nDevListLen)
		{
			if (NULL != pDevListBuf)
			{
				delete pDevListBuf;
				pDevListBuf = NULL;
			}
			nDevListBufLen = nDevListLen;
			pDevListBuf = new char[nDevListBufLen];
		}

		// 搜索主动连接设备，获取设备列表的缓冲区（设备间以 ; 分隔）
		if (S_OK == HVAPI_GetDeviceList(pDevListBuf, nDevListBufLen, NULL))
		{
			if (NULL == pDevListBuf)
			{
				continue;
			}
			pszSN = pDevListBuf;
			while(pszTemp = strstr(pszSN, ";"))
			{
				*pszTemp = '\0';

				// 连接设备并更新列表上的连接状态
				ShowClientInfor(pszSN);
				pszSN = pszTemp+1;
			}
		}
	}

	if (NULL != pDevListBuf)
	{
		delete pDevListBuf;
		pDevListBuf = NULL;
	}

	return 0;
}

// 连接设备并更新列表上的连接状态
HRESULT CAutoLink_demoDlg::ShowClientInfor(char* pSN)
{
	if (m_fExit || NULL == pSN)
	{
		return S_FALSE;
	}
	if (0 == strcmp(pSN, ""))
	{
		return S_FALSE;
	}

	INT nRecordConStatus = CONN_STATUS_UNKNOWN;
	int nReConnectCount = 0;
	CString szRecordConStatus;
	// 设备状态
	HVAPI_GetDeviceStatus(pSN, nRecordConStatus, nReConnectCount, NULL);
	switch (nRecordConStatus)
	{
		case CONN_STATUS_UNKNOWN:
			szRecordConStatus.Format(DEV_STATUS_UNKNOW);
			break;
		case CONN_STATUS_NORMAL:
			szRecordConStatus.Format(DEV_STATUS_CONN);
			break;
		case CONN_STATUS_DISCONN:
			szRecordConStatus.Format(DEV_STATUS_DISCONN);
			break;
		case CONN_STATUS_RECONN:
			szRecordConStatus.Format(DEV_STATUS_RECONN);
			break;
		case CONN_STATUS_RECVDONE:
			szRecordConStatus.Format(DEV_STATUS_DONE);
			break;
		default:
			szRecordConStatus.Format(DEV_STATUS_UNKNOW);
			break;
	}

	// 更新列表状态
	int iNetDiskCount = m_devListCtrl.GetItemCount();
	CString strDevSN;
	CString strReConnectCount;
	strReConnectCount.Format("%d", nReConnectCount);
	for (int i = 0; i<iNetDiskCount; i++)
	{
		strDevSN = m_devListCtrl.GetItemText(i, 0);

		if (0 == strcmp(pSN, strDevSN))
		{
			if (m_pIsUsrDiscnn[i] == 0)
			{
				if (0 == strcmp(szRecordConStatus, DEV_STATUS_UNKNOW) 
					|| 0 == strcmp(szRecordConStatus, DEV_STATUS_DISCONN))
				{
					CreatConnect(pSN); // 连接设备
				}
			}
			m_devListCtrl.SetItemText(i, 1, szRecordConStatus);
			m_devListCtrl.SetItemText(i, 2, strReConnectCount);
			return S_OK;
		}
	}

	int iItemRow = m_devListCtrl.InsertItem(0, pSN);
	m_devListCtrl.SetItem(iItemRow, 1, LVIF_TEXT, szRecordConStatus, 0, 0, 0 ,0 );
	m_devListCtrl.SetItemText(i, 2, strReConnectCount);

	return S_OK;
}

// 连接设备
HRESULT CAutoLink_demoDlg::CreatConnect(char *pSN)
{
	if (NULL == pSN)
		return S_FALSE;

	CString strDevSN(pSN);

	CCamera* pCameraTemp = NULL;
	EnterCriticalSection(&g_csDevList);
	POSITION pos = m_connDevList.GetHeadPosition();
	while(pos)
	{
		pCameraTemp = m_connDevList.GetAt(pos);

		if (NULL != pCameraTemp)
		{
			if (pCameraTemp->m_strDevSN == strDevSN)
			{
				LeaveCriticalSection(&g_csDevList);
				return S_OK;
			}
		}
		m_connDevList.GetNext(pos);
	}

	char chTemp[256] = {0};
	CCamera* pCamera = new CCamera();
	if (pCamera->Open(strDevSN, m_strDir) != 0)
	{
		delete pCamera;
		pCamera = NULL;
		sprintf(chTemp, "创建设备失败:[%s]", strDevSN);
		WriteLog(chTemp);
	}
	else
	{
		m_connDevList.AddHead(pCamera);
		sprintf(chTemp, "创建设备成功[%s] 设备数量:[%d]", strDevSN, m_connDevList.GetCount());
		WriteLog(chTemp);
	}
	LeaveCriticalSection(&g_csDevList);
	return S_OK;
}

// 断开连接
HRESULT CAutoLink_demoDlg::DisConnectDev(char* pSN)
{
	if (NULL == pSN)
		return S_FALSE;

	CString strDevSN(pSN);

	CCamera* pCameraTemp = NULL;
	EnterCriticalSection(&g_csDevList);
	POSITION pos = m_connDevList.GetHeadPosition();
	while(pos)
	{
		pCameraTemp = m_connDevList.GetAt(pos);
		if (NULL != pCameraTemp)
		{
			if (pCameraTemp->m_strDevSN == strDevSN)
			{
				pCameraTemp->Close();
				m_connDevList.RemoveAt(pos);
				delete pCameraTemp;
				pCameraTemp = NULL;
				break;
			}
		}
		m_connDevList.GetNext(pos);
	}
	LeaveCriticalSection(&g_csDevList);
	return S_OK;
}

// 关闭程序
void CAutoLink_demoDlg::OnClose()
{
	if (IDYES == MessageBox("是否退出程序？", "退出提示", MB_YESNO))
	{
		// 禁用关闭按钮，避免重复关闭
		CMenu *menu = GetSystemMenu(FALSE);
		if (menu != NULL)
		{
			menu->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND | MF_GRAYED);
		}
		m_closeAutoLinkBut.EnableWindow(FALSE);

		m_fExit = TRUE;
		if (NULL != m_hShowThread)
		{
			WaitForSingleObject(m_hShowThread, 2000);
			CloseHandle(m_hShowThread);
			m_hShowThread = NULL;
		}
		if (NULL != m_hDeleteThread)
		{
			WaitForSingleObject(m_hDeleteThread, 1200);
			CloseHandle(m_hDeleteThread);
			m_hDeleteThread = NULL;
		}

		// 关闭设备连接
		CCamera* pCameraTemp = NULL;
		POSITION pos = m_connDevList.GetHeadPosition();
		while(pos)
		{
			pCameraTemp = m_connDevList.GetAt(pos);

			if (NULL != pCameraTemp)
			{
				pCameraTemp->Close();
				delete pCameraTemp;
				pCameraTemp = NULL;
			}
			m_connDevList.GetNext(pos);
		}
		m_connDevList.RemoveAll();

		// (该操作会主动去关闭主动连接句柄，必须要放在HVAPI_CloseEx之后，否则会报错)
		HVAPI_CloseServer(LISTEN_TYPE_RECORD); // 关闭结果监听服务
		HVAPI_UnLoadMonitor(); // 卸载监听模块
		
		m_devListCtrl.DeleteAllItems();

		if (m_pIsUsrDiscnn != NULL)
		{
			delete [] m_pIsUsrDiscnn;
			m_pIsUsrDiscnn = NULL;
		}

		// 更新配置
		WriteFileIni();

		DeleteCriticalSection( &g_csLog );
		DeleteCriticalSection(&g_csOperation);
		DeleteCriticalSection(&g_csDevList);

		CDialog::OnClose();
	}
}

// 双击连接设备
void CAutoLink_demoDlg::OnDBClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (-1 == pNMListView->iItem)
		return;

	CString strDevSN = m_devListCtrl.GetItemText(pNMListView->iItem , 0);
	CString strConnStatus = m_devListCtrl.GetItemText(pNMListView->iItem , 1);

	char chTempSN[128] = {0};
	sprintf(chTempSN, "%s", strDevSN.GetBuffer(strDevSN.GetLength()));
	strDevSN.ReleaseBuffer();

	if (0 == strcmp(strConnStatus, DEV_STATUS_UNKNOW) || 
		0 == strcmp(strConnStatus, DEV_STATUS_DISCONN))
	{
		m_pIsUsrDiscnn[pNMListView->iItem] = 0;
		AfxMessageBox("开始连接设备！");
	}
	else if (0 == strcmp(strConnStatus , DEV_STATUS_DONE))
	{
		m_pIsUsrDiscnn[pNMListView->iItem] = 0;
		AfxMessageBox("若需要继续接实时结果，请右键设置关闭历史连接！");
	}
	else
	{
		m_pIsUsrDiscnn[pNMListView->iItem] = 1;
		if (S_OK == DisConnectDev(chTempSN))
		{
			AfxMessageBox("断开设备！");
		}
	}
}

// 设置历史连接
void CAutoLink_demoDlg::OnSetHistoryConnect()
{
	POSITION pos = m_devListCtrl.GetFirstSelectedItemPosition();
	int iRow = m_devListCtrl.GetNextSelectedItem(pos);
	CString strDevSN = m_devListCtrl.GetItemText(iRow, 0);

	char chDevSN[128] = {0};
	sprintf(chDevSN, "%s", strDevSN.GetBuffer(strDevSN.GetLength()));
	strDevSN.ReleaseBuffer();

	bool fSetParam = false;
	AutoLinkHistorySet dlg;
	dlg.InitHistorySet(chDevSN, &fSetParam);
	dlg.DoModal();

	if (fSetParam)
	{
		if (S_OK == DisConnectDev(chDevSN))
			WriteLog("历史结果设置成功！");
	}
}

// 右键选中列表
void CAutoLink_demoDlg::OnRClickDevList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (-1 != pNMListView->iItem)
	{
		CString strDevSN = m_devListCtrl.GetItemText(pNMListView->iItem , 0);

		int iSubMenuNum = 0;
		CMenu menu, *pSubMenu;
		menu.LoadMenu(IDR_MENU1);
		pSubMenu = menu.GetSubMenu(iSubMenuNum);

		POINT oPoint;
		GetCursorPos(&oPoint);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, oPoint.x, oPoint.y, this);
	}
}

// 目录选择框
void CAutoLink_demoDlg::OnBnClickedFindDir()
{
	BROWSEINFO   bBinfo; 
	memset(&bBinfo, 0, sizeof(BROWSEINFO));     // 定义结构并初始化 
	bBinfo.hwndOwner=m_hWnd; // 设置对话框所有者句柄 

	bBinfo.lpszTitle= "请选择结果保存目录： "; 
	bBinfo.ulFlags   =   BIF_RETURNONLYFSDIRS;   // 设置标志只允许选择目录         
	LPITEMIDLIST   lpDlist; 

	CString dir;
	//用来保存返回信息的IDList，使用SHGetPathFromIDList函数转换为字符串 
	lpDlist=SHBrowseForFolder(&bBinfo);   // 显示选择对话框 
	if(lpDlist!=NULL) 
	{ 
		SHGetPathFromIDList(lpDlist, dir.GetBuffer(255)); // 把项目标识列表转化成目录 
		dir.ReleaseBuffer();
		GetDlgItem(IDC_DIR)->SetWindowText(dir);
	}
}

// 屏蔽回车和ESC
BOOL CAutoLink_demoDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN
			|| pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

// 路径设置
void CAutoLink_demoDlg::OnBnClickedButtonDir()
{
	AutoLinkDirSet dlg;
	dlg.DoModal();
}

// 文件名设置
void CAutoLink_demoDlg::OnBnClickedButtonFilename()
{
	AutoLinkNameSet dlg;
	dlg.DoModal();
}

// 日志
void CAutoLink_demoDlg::WriteLog(char* chText)
{
	if (chText == NULL)
		return;

	FILE *file = NULL;
	CTime tm = CTime::GetCurrentTime();
	CString strText;
	strText.Format("%s [主程]:%s\n", tm.Format("%Y-%m-%d %H:%M:%S"), chText);

	EnterCriticalSection(&g_csLog);
	if (m_iLogEnable != 0)
	{
		file = fopen("Log.txt", "a+");
		fprintf(file, "%s", strText.GetBuffer(strText.GetLength()));
		strText.ReleaseBuffer();
		fclose(file);
		file = NULL;
	}
	LeaveCriticalSection(&g_csLog);
}

// 读取配置
void CAutoLink_demoDlg::ReadFileIni()
{
	TCHAR fileName[MAX_PATH];
	GetModuleFileName(NULL, fileName, MAX_PATH-1);
	PathRemoveFileSpec(fileName);
	TCHAR chIniFileName[MAX_PATH] = { 0 };
	strcpy(chIniFileName, fileName);
	sprintf(g_chLoopFileDir, "%s\\LoopFileTemp\\", chIniFileName);
	strcat(chIniFileName, _T("\\Config.ini"));

	// 日志开关
	m_iLogEnable = GetPrivateProfileInt("LOG_SET", "LogEnable", 0, chIniFileName);

	// 循环覆盖
	g_iLoopEnable = GetPrivateProfileInt("LOOP_SET", "LoopEnable", 1, chIniFileName);
	g_iSpace = GetPrivateProfileInt("LOOP_SET", "Space", 10, chIniFileName);
	g_iFileMaxNum = GetPrivateProfileInt("LOOP_SET", "FileMaxNum", 1000, chIniFileName);
	g_iFileSaveNum = GetPrivateProfileInt("LOOP_SET", "FileSaveNum", 0, chIniFileName);
	g_iFileDeleteNum = GetPrivateProfileInt("LOOP_SET", "FileDeleteNum", 0, chIniFileName);
}

// 写配置
void CAutoLink_demoDlg::WriteFileIni()
{
	TCHAR fileName[MAX_PATH];
	GetModuleFileName(NULL, fileName, MAX_PATH-1);
	PathRemoveFileSpec(fileName);
	TCHAR chIniFileName[MAX_PATH] = { 0 };
	strcpy(chIniFileName, fileName);
	strcat(chIniFileName, _T("\\Config.ini"));

	char chTemp[256] = {0};
	
	// 循环覆盖
	sprintf(chTemp, "%d", g_iLoopEnable);
	WritePrivateProfileString("LOOP_SET", "LoopEnable", chTemp, chIniFileName);
	sprintf(chTemp, "%d", g_iSpace);
	WritePrivateProfileString("LOOP_SET", "Space", chTemp, chIniFileName);
	//sprintf(chTemp, "%d", g_iFileMaxNum);
	//WritePrivateProfileString("LOOP_SET", "FileMaxNum", chTemp, chIniFileName);
	sprintf(chTemp, "%d", g_iFileSaveNum);
	WritePrivateProfileString("LOOP_SET", "FileSaveNum", chTemp, chIniFileName);
	sprintf(chTemp, "%d", g_iFileDeleteNum);
	WritePrivateProfileString("LOOP_SET", "FileDeleteNum", chTemp, chIniFileName);
}

// 读取界面配置
void CAutoLink_demoDlg::ReadDlgIni()
{
	TCHAR fileName[MAX_PATH];
	GetModuleFileName(NULL, fileName, MAX_PATH-1);
	PathRemoveFileSpec(fileName);
	TCHAR chIniFileName[MAX_PATH] = { 0 };
	strcpy(chIniFileName, fileName);
	strcat(chIniFileName, _T("\\Config.ini"));

	// 盘符保存
	GetPrivateProfileString("ALL_SET", "DiskDir", "D:\\", m_strDir.GetBuffer(512), 512, chIniFileName);
	m_strDir.ReleaseBuffer();
	// 监听端口
	m_iPort = GetPrivateProfileInt("ALL_SET", "Port", 6665, chIniFileName);
	// 监听设备数量
	m_iDevMaxNum = GetPrivateProfileInt("ALL_SET", "DeviceNum", 50, chIniFileName);
}

// 写界面配置
void CAutoLink_demoDlg::WriteDlgIni()
{
	TCHAR fileName[MAX_PATH];
	GetModuleFileName(NULL, fileName, MAX_PATH-1);
	PathRemoveFileSpec(fileName);
	TCHAR chIniFileName[MAX_PATH] = { 0 };
	strcpy(chIniFileName, fileName);
	strcat(chIniFileName, _T("\\Config.ini"));

	char chTemp[256] = {0};

	// 盘符保存
	WritePrivateProfileString("ALL_SET", "DiskDir", m_strDir.GetBuffer(m_strDir.GetLength()), chIniFileName);
	m_strDir.ReleaseBuffer();
	// 监听端口
	sprintf(chTemp, "%d", m_iPort);
	WritePrivateProfileString("ALL_SET", "Port", chTemp, chIniFileName);
	// 监听设备数量
	sprintf(chTemp, "%d", m_iDevMaxNum);
	WritePrivateProfileString("ALL_SET", "DeviceNum", chTemp, chIniFileName);
}

// 循环覆盖::记录文件名
void WriteFileName(const char* chFileName)
{
	if (NULL == chFileName || g_iLoopEnable != 1)
		return;

	FILE* pFile = NULL;
	char chLoopFileName[512] = {0};
	EnterCriticalSection(&g_csOperation);
	if (g_iFileCount >= g_iFileMaxNum)
	{
		g_iFileSaveNum++;
		g_iFileCount = 0;
	}
	if ( !PathFileExists(g_chLoopFileDir) )
	{
		MakeSureDirectoryPathExists(g_chLoopFileDir);
	}

	sprintf(chLoopFileName, "%s%d.bns", g_chLoopFileDir, g_iFileSaveNum);
	pFile = fopen(chLoopFileName, "a+");
	if (pFile != NULL)
	{
		fprintf(pFile, "%s\n", chFileName);
		fclose(pFile);
		pFile = NULL; 
	}

	g_iFileCount++;
	LeaveCriticalSection(&g_csOperation);
}

// 检测驱动盘的大小，以G为单位
int CAutoLink_demoDlg::CheckDriveSpaceSize(char *szDriveName)
{
	if(szDriveName == NULL)
		return -1;

	ULARGE_INTEGER uliUserFree;
	ULARGE_INTEGER uliTotal;
	ULARGE_INTEGER uliRealFree;
	::GetDiskFreeSpaceEx(szDriveName, &uliUserFree, &uliTotal, &uliRealFree);
	DWORD64 dwfree = (uliRealFree.QuadPart >> 20) / 1024;
	return (int)dwfree;
}

// 删除线程
int CAutoLink_demoDlg::DeleteFileRun()
{
	int iFreeSpace = 0;
	char chDir[256] = {0};
	sprintf(chDir, "%s", m_strDir.GetBuffer(m_strDir.GetLength()));
	m_strDir.ReleaseBuffer();

	while(!m_fExit)
	{
		Sleep(4000);
		iFreeSpace = CheckDriveSpaceSize(chDir);
		if (iFreeSpace == -1 || iFreeSpace >= g_iSpace || g_iLoopEnable != 1 || g_iFileDeleteNum >= g_iFileSaveNum)
			continue;

		char chTemp[256] = {0};
		sprintf(chTemp, "剩余磁盘空间:%dG", iFreeSpace);
		WriteLog(chTemp);

		char chPosFile[512] = {0};
		sprintf(chPosFile, "%s%d.bns", g_chLoopFileDir, g_iFileDeleteNum);

		FILE* pFile = NULL;
		EnterCriticalSection(&g_csOperation);
		pFile = fopen(chPosFile, "r");
		if (NULL == pFile)
		{
			sprintf(chTemp, "打开记录文件失败:%s", chPosFile);
			WriteLog(chTemp);
			if (g_iFileDeleteNum < g_iFileSaveNum)
			{
				g_iFileDeleteNum++;
			}
			LeaveCriticalSection(&g_csOperation);
			continue;
		}

		char chDeleteFileName[512] = {0};
		char chDeleteFileNameTemp[512] = {0};
		while(!feof(pFile))
		{
			memset(chDeleteFileName, 0, 512);
			memset(chDeleteFileNameTemp, 0, 512);
			if (fgets(chDeleteFileNameTemp, 512, pFile))
			{
				strncpy(chDeleteFileName, chDeleteFileNameTemp, strlen(chDeleteFileNameTemp)-1);
				DeleteFileByName(chDeleteFileName);
			}
		}
		if (feof(pFile))
		{
			fclose(pFile);
			pFile = NULL;
			DeleteFile(chPosFile);

			sprintf(chTemp, "删除了记录文件:%s", chPosFile);
			WriteLog(chTemp);

			if (g_iFileDeleteNum < g_iFileSaveNum)
			{
				g_iFileDeleteNum++;
			}
		}
		LeaveCriticalSection(&g_csOperation);

		if (pFile != NULL)
		{
			fclose(pFile);
			pFile = NULL;
		}
	}

	return 0;
}

// 删除文件
int CAutoLink_demoDlg::DeleteFileByName(const char* chFileName)
{
	if (NULL == chFileName || g_iFileDeleteNum == g_iFileSaveNum)
		return -1;

	BOOL iRet = TRUE;
	iRet = DeleteFile(chFileName);
	if (iRet)
	{
		DeleteDirectory(chFileName); // 递归删除空目录
	}

	return iRet;
}

// 递归删除空目录
int CAutoLink_demoDlg::DeleteDirectory(const char* chDir)
{
	CString strDeleteDirTemp(chDir);
	int iPos = -1;
	iPos = strDeleteDirTemp.ReverseFind('\\');
	if (-1 == iPos)
	{
		return -1;
	}

	strDeleteDirTemp.Delete(iPos, 100);
	CString strDeleteDir = strDeleteDirTemp;

	if (RemoveDirectory(strDeleteDir.GetBuffer(strDeleteDir.GetLength())))
	{
		DeleteDirectory(strDeleteDir);
		strDeleteDir.ReleaseBuffer();
	}
	else
	{
		strDeleteDir.ReleaseBuffer();
		return -1;
	}

	return 0;
}
