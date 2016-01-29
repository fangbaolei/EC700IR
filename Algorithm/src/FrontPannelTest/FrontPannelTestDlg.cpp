// FrontPannelTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FrontPannelTest.h"
#include "FrontPannelTestDlg.h"
#include ".\frontpanneltestdlg.h"
#include "tinyxml.h"
#include "CUpdateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ddraw.lib")

void WrightLog(LPCSTR lpszLogInfo)
{
	FILE* fp = fopen("D:\\ControllPannelLog.txt", "a+");
	if(fp)
	{
		fwrite(lpszLogInfo, strlen(lpszLogInfo), 1, fp);
		fwrite("\n", 1, 1, fp);
		fclose(fp);
	}
}

HRESULT HvParseXmlCmdRespRetcode2(char* szXmlBuf, char* szCmdName, char* szCmdValueName, char* szCmdValueText)
{
	HRESULT rst = E_FAIL;
	TiXmlDocument cXmlDoc;
	if(cXmlDoc.Parse(szXmlBuf))
	{
		const char* pszTextGet = NULL;
		TiXmlElement* pCurElement = cXmlDoc.FirstChildElement("HvCmdRespond");
		if(pCurElement == NULL)
		{
			goto done;
		}

		TiXmlAttribute* pAttrib = pCurElement->FirstAttribute();
		if(pAttrib == NULL)
		{
			goto done;
		}

		if(strcmp(pAttrib->Value(), "2.0") != 0)
		{
			goto done;
		}

		pCurElement = pCurElement->FirstChildElement();
		while(pCurElement)
		{
			if(strcmp(pCurElement->GetText(), szCmdName) == 0)
			{
				pszTextGet = pCurElement->Attribute(szCmdValueName);
				if(pszTextGet)
				{
					memcpy(szCmdValueText, pszTextGet, strlen(pszTextGet) + 1);
					return S_OK;
				}
				else
				{
					goto done;
				}

			}
			pCurElement = pCurElement->NextSiblingElement();
		}

	}

done:
	return rst; 
}

HRESULT ParseXmlInfoRespValue(BOOL fNewProtuol, char* szXmlBuf, char* szInfoName, char* szInfoValueName, char* szInfoValueText)
{
	if(fNewProtuol)
	{
		return HvParseXmlCmdRespRetcode2(szXmlBuf, szInfoName, szInfoValueName, szInfoValueText);
	}
	HRESULT rst = E_FAIL;
	TiXmlDocument cXmlDoc;
	if ( cXmlDoc.Parse(szXmlBuf) )
	{
		const char* szTextGet;
		TiXmlElement* pCurElement = cXmlDoc.FirstChildElement("HvInfoRespond");
		if ( pCurElement == NULL )   goto done;
		TiXmlAttribute* pAttrib = pCurElement->FirstAttribute();
		if ( (pAttrib==NULL) 
			|| (0 != strcmp(pAttrib->Value(), "1.0")) )
			goto done;
		for ( pCurElement = pCurElement->FirstChildElement("Info");
			pCurElement != NULL; 
			pCurElement = pCurElement->NextSiblingElement("Info") )
		{
			pAttrib = pCurElement->FirstAttribute();
			szTextGet = pAttrib->Value();
			if(_strnicmp(szInfoName, szTextGet, strlen(szTextGet)+1) == 0)
			{
				for ( TiXmlElement* pCurValueElement = pCurElement->FirstChildElement("Value");
					pCurValueElement != NULL; 
					pCurValueElement = pCurValueElement->NextSiblingElement("Value") )
				{
					pAttrib = pCurValueElement->FirstAttribute();
					szTextGet = pAttrib->Value();
					if(_strnicmp(szInfoValueName, szTextGet, strlen(szTextGet)+1) == 0)
					{
						szTextGet = pCurValueElement->GetText();
						if(szTextGet == NULL)
						{
							goto done;
						}
						memcpy(szInfoValueText, szTextGet, strlen(szTextGet)+1);
						rst = S_OK;
						break;
					}
				}
			}
		}
	}
done:
	return rst;
}

HRESULT HvParseXmlCmdRespRetcode(char* szXmlBuf, char* szRetcode)
{
	HRESULT rst = E_FAIL;
	TiXmlDocument cXmlDoc;
	if ( cXmlDoc.Parse(szXmlBuf) )
	{
		const char* szTextGet;
		TiXmlElement* pCurElement = cXmlDoc.FirstChildElement("HvCmdRespond");
		if ( pCurElement == NULL ) goto done;
		TiXmlAttribute* pAttrib = pCurElement->FirstAttribute();
		if (pAttrib==NULL)
			goto done;
		if(strcmp(pAttrib->Value(), "1.0") == 0)
		{
			pCurElement = pCurElement->FirstChildElement("RetCode");
			assert(pCurElement != NULL);
			szTextGet = pCurElement->GetText();
		}
		else if(strcmp(pAttrib->Value(), "2.0") == 0)
		{
			for(;pCurElement; pCurElement = pCurElement->NextSiblingElement())
			{
				if(pCurElement->Attribute("RetCode") && pCurElement->GetText())
				{
					szTextGet = pCurElement->Attribute("RetCode");
				}
			}
		}
		//	|| (0 != strcmp(pAttrib->Value(), "1.0")) )   // 默认认为第一个属性就是ver
		//	goto done;
		//// 查找返回值
		//pCurElement = pCurElement->FirstChildElement("RetCode");
		//assert(pCurElement != NULL);
		//szTextGet = pCurElement->GetText();
		assert(szTextGet != NULL);
		memcpy(szRetcode, szTextGet, strlen(szTextGet)+1);
		rst = S_OK;
	}
done:
	return rst;
}

static INT CDECL HvCamCallbackJpeg(PVOID pUserData,
								   PBYTE pbImageData,
								   DWORD dwImageDataLen,
								   PBYTE pbImageInfoData,
								   DWORD dwImageInfoLen,
								   DWORD dwImageType,
								   LPCSTR szImageExtInfo)
{
	if(pUserData == NULL || pbImageData == NULL || szImageExtInfo == NULL) return -1;
	static DWORD dwIndex = 0;
	CFrontPannelTestDlg* pDlg = (CFrontPannelTestDlg*)pUserData;

	if(dwImageType == IMAGE_TYPE_JPEG_NORMAL)
	{
		dwIndex++;
		if(dwIndex > 666666) dwIndex = 0;
		if(dwIndex % 3 != 0) return 0;
		IStream* pStm = NULL;
		CreateStreamOnHGlobal(NULL, TRUE, &pStm);
		IPicture* picholder;
		LARGE_INTEGER liTempStar = {0};
		pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
		ULONG iWritten = NULL;
		pStm->Write(pbImageData, dwImageDataLen, &iWritten);
		pStm->Seek(liTempStar, STREAM_SEEK_SET, NULL);
		if(FAILED(OleLoadPicture(pStm, dwImageDataLen, TRUE, IID_IPicture, (void**)&picholder)))
		{
			pStm->Release();
			return 0;
		}

		RECT cRect;
		::GetWindowRect(pDlg->GetDlgItem(IDC_STATIC_PIC)->GetSafeHwnd(), &cRect);
		HDC TempDC;
		TempDC = ::GetDC(pDlg->GetDlgItem(IDC_STATIC_PIC)->GetSafeHwnd());
		OLE_XSIZE_HIMETRIC hmWidth;
		OLE_YSIZE_HIMETRIC hmHeight;
		picholder->get_Width(&hmWidth);
		picholder->get_Height(&hmHeight);
		int nWidth =MulDiv(hmWidth, GetDeviceCaps(TempDC, LOGPIXELSX), 2540);
		int nHeight = MulDiv(hmHeight, GetDeviceCaps(TempDC, LOGPIXELSY), 2540);
		picholder->Render(TempDC, 0, 0, cRect.right-cRect.left, cRect.bottom-cRect.top, 0, hmHeight, hmWidth, -hmHeight, NULL); 
		::ReleaseDC(pDlg->GetDlgItem(IDC_STATIC_PIC)->GetSafeHwnd(), TempDC);
		picholder->Release();
		pStm->Release();

		if(szImageExtInfo != NULL)
		{
			int iShutter, iGain, R, G, B;
			DWORD64 dwTimes;
			sscanf(szImageExtInfo, "Shutter:%d,Gain:%d,R:%d,G:%d,B:%d,FrameTime:%I64u",
				&iShutter, &iGain, &R, &G, &B, &dwTimes);
			CString strInfo;
			strInfo.Format("快门：%d增益：%d R=%d G=%d B=%d",
				iShutter, iGain, R, G, B);
			pDlg->SetDlgItemText(IDC_STATIC_FRAMINFO, strInfo.GetBuffer());
		}
	}
	return 0;
}

// CFrontPannelTestDlg 对话框

CFrontPannelTestDlg::CFrontPannelTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFrontPannelTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDevice = NULL;
	m_fIsConnected = FALSE;
	m_fIsShowVideo = FALSE;
	m_pUpDateParamThread = NULL;
	m_fIsUpDataConfigInfo = FALSE;
	m_iAutoRunMode = -1;
	m_iUpdateDelay = -1;
	m_iPannelStyle = -1;
}

void CFrontPannelTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_DeviceIP);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressPulse1);
	DDX_Control(pDX, IDC_PROGRESS2, m_ProgressPulse2);
	DDX_Control(pDX, IDC_PROGRESS3, m_ProgressPulse3);
	DDX_Control(pDX, IDC_PROGRESS4, m_ProgressPulse4);
	DDX_Control(pDX, IDC_COMBO1, m_ComBoxWorkMode);
	DDX_Control(pDX, IDC_COMBO2, m_ComBoxWorkStatus);
	DDX_Control(pDX, IDC_COMBO3, m_ComBoxPLMode);
	DDX_Control(pDX, IDC_SLIDER1, m_SliderMin);
	DDX_Control(pDX, IDC_SLIDER2, m_SliderMax);
	DDX_Control(pDX, IDC_COMBO4, m_ComBoxFlash1Polarity);
	DDX_Control(pDX, IDC_COMBO5, m_ComBoxFlash1Coupling);
	DDX_Control(pDX, IDC_COMBO6, m_ComBoxFlash1ResistorMode);
	DDX_Control(pDX, IDC_COMBO7, m_ComBoxFlash2Polarity);
	DDX_Control(pDX, IDC_COMBO8, m_ComBoxFlash2Coupling);
	DDX_Control(pDX, IDC_COMBO9, m_ComBoxFlash2ResistorMode);
	DDX_Control(pDX, IDC_COMBO10, m_ComBoxFlash3Polarity);
	DDX_Control(pDX, IDC_COMBO11, m_ComBoxFlash3Coupling);
	DDX_Control(pDX, IDC_COMBO12, m_ComBoxFlash3ResistorMode);
}

BEGIN_MESSAGE_MAP(CFrontPannelTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_COMBO2, OnCbnSelchangeCombo2)
	ON_CBN_SELCHANGE(IDC_COMBO3, OnCbnSelchangeCombo3)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_CHECK2, OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_BUTTON11, OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, OnBnClickedButton12)
END_MESSAGE_MAP()


// CFrontPannelTestDlg 消息处理程序

BOOL CFrontPannelTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	m_DeviceIP.SetAddress(172, 18, 111, 222);
	SetDlgItemText(IDC_STATIC_MV, "控制板主程序版本: 未知");
	SetDlgItemText(IDC_STATIC_SV, "控制板升级程序版本: 未知");
	SetDlgItemText(IDC_STATIC_INFO, "工作模式：未知 工作状态：未知 温度：未知");
	SetDlgItemText(IDC_STATIC_FRAMINFO, "快门时间：0 增益：0 R=0 G=0 B=0");
	m_ProgressPulse1.SetRange(0, 80);
	m_ProgressPulse2.SetRange(0, 80);
	m_ProgressPulse3.SetRange(0, 80);
	m_ProgressPulse4.SetRange(0, 80);
	m_ProgressPulse1.SetPos(0);
	m_ProgressPulse2.SetPos(0);
	m_ProgressPulse3.SetPos(0);
	m_ProgressPulse4.SetPos(0);
	SetDlgItemText(IDC_EDIT5, "0ms");
	SetDlgItemText(IDC_EDIT6, "未知");
	SetDlgItemText(IDC_EDIT7, "未知");
	SetDlgItemText(IDC_EDIT8, "未知");

	SetDlgItemText(IDC_EDIT9, "0ms");
	SetDlgItemText(IDC_EDIT10, "未知");
	SetDlgItemText(IDC_EDIT11, "未知");
	SetDlgItemText(IDC_EDIT12, "未知");

	SetDlgItemText(IDC_EDIT13, "0ms");
	SetDlgItemText(IDC_EDIT14, "未知");
	SetDlgItemText(IDC_EDIT15, "未知");
	SetDlgItemText(IDC_EDIT16, "未知");

	m_ComBoxWorkMode.AddString("正常");
	m_ComBoxWorkMode.AddString("升级");
	m_ComBoxWorkMode.AddString("测试");
	m_ComBoxWorkMode.SetCurSel(0);

	m_ComBoxWorkStatus.AddString("停止");
	m_ComBoxWorkStatus.AddString("运行");
	m_ComBoxWorkStatus.SetCurSel(0);

	m_ComBoxPLMode.AddString("关闭");
	m_ComBoxPLMode.AddString("打开");
	m_ComBoxPLMode.SetCurSel(0);

	m_ComBoxWorkMode.EnableWindow(FALSE);
	m_ComBoxWorkStatus.EnableWindow(FALSE);

	m_SliderMin.SetRange(0, 80);
	m_SliderMin.SetPos(0);

	m_SliderMax.SetRange(0, 80);
	m_SliderMax.SetPos(0);

	SetDlgItemText(IDC_EDIT17, "0ms");
	SetDlgItemText(IDC_EDIT18, "0ms");

	m_ComBoxFlash1Polarity.AddString("负极");
	m_ComBoxFlash1Polarity.AddString("正极");
	m_ComBoxFlash1Polarity.SetCurSel(0);
	m_ComBoxFlash1Coupling.AddString("不耦合");
	m_ComBoxFlash1Coupling.AddString("耦合");
	m_ComBoxFlash1Coupling.SetCurSel(0);
	m_ComBoxFlash1ResistorMode.AddString("短路");
	m_ComBoxFlash1ResistorMode.AddString("电平");
	m_ComBoxFlash1ResistorMode.SetCurSel(0);

	m_ComBoxFlash2Polarity.AddString("负极");
	m_ComBoxFlash2Polarity.AddString("正极");
	m_ComBoxFlash2Polarity.SetCurSel(0);
	m_ComBoxFlash2Coupling.AddString("不耦合");
	m_ComBoxFlash2Coupling.AddString("耦合");
	m_ComBoxFlash2Coupling.SetCurSel(0);
	m_ComBoxFlash2ResistorMode.AddString("短路");
	m_ComBoxFlash2ResistorMode.AddString("电平");
	m_ComBoxFlash2ResistorMode.SetCurSel(0);

	m_ComBoxFlash3Polarity.AddString("负极");
	m_ComBoxFlash3Polarity.AddString("正极");
	m_ComBoxFlash3Polarity.SetCurSel(0);
	m_ComBoxFlash3Coupling.AddString("不耦合");
	m_ComBoxFlash3Coupling.AddString("耦合");
	m_ComBoxFlash3Coupling.SetCurSel(0);
	m_ComBoxFlash3ResistorMode.AddString("短路");
	m_ComBoxFlash3ResistorMode.AddString("电平");
	m_ComBoxFlash3ResistorMode.SetCurSel(0);
	SetDlgItemText(IDC_EDIT19, "0.0ms");
	SetDlgItemText(IDC_EDIT20, "0.0ms");
	SetDlgItemText(IDC_EDIT21, "0.0ms");

	SetDlgItemText(IDC_EDIT22, "0");
	GetDlgItem(IDC_EDIT22)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON11)->EnableWindow(FALSE);

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CFrontPannelTestDlg::OnOK()
{

}

BOOL CFrontPannelTestDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return true;
		else if(pMsg->wParam == VK_RETURN)
		{
			char szTempText[20] = {0};
			float fValue = 0.;
			int iValue = 0;
			CWnd* pw = CWnd::GetFocus();
			if(pw == GetDlgItem(IDC_EDIT17))
			{
				if(m_hDevice == NULL)
				{
					MessageBox("未连接一体机设备，无法设置");
					return CDialog::PreTranslateMessage(pMsg);
				}
				if(m_iPannelStyle != 1)
				{
					MessageBox("非数控板无法设置运行模式", "INFO", MB_OK);
					return CDialog::PreTranslateMessage(pMsg);
				}
				GetDlgItem(IDC_EDIT18)->SetFocus();
				GetDlgItemText(IDC_EDIT17, szTempText, 20);
				sscanf(szTempText, "%fms", &fValue);
				iValue = (int)(fValue * 10 + 0.5);
				if(iValue < 1)
				{
					MessageBox("脉宽最小值不能小于0.1毫秒", "ERROR", MB_OK|MB_ICONERROR);
					m_iUpdateDelay = 2;
					return CDialog::PreTranslateMessage(pMsg);
				}
				if(iValue > 72)
				{
					MessageBox("脉宽最小值不能大于7.2毫秒", "ERROR", MB_OK|MB_ICONERROR);
					m_iUpdateDelay = 2;
					return CDialog::PreTranslateMessage(pMsg);
				}
				m_SliderMin.SetPos(iValue);
				ChangePulseWidth();
			}
			else if(pw == GetDlgItem(IDC_EDIT18))
			{
				if(m_hDevice == NULL)
				{
					MessageBox("未连接一体机设备，无法设置");
					return CDialog::PreTranslateMessage(pMsg);
				}
				if(m_iPannelStyle != 1)
				{
					MessageBox("非数控板无法设置运行模式", "INFO", MB_OK);
					return CDialog::PreTranslateMessage(pMsg);
				}
				GetDlgItem(IDC_EDIT17)->SetFocus();
				GetDlgItemText(IDC_EDIT18, szTempText, 20);
				sscanf(szTempText, "%fms", &fValue);
				iValue = (int)(fValue * 10 + 0.5);
				if(iValue < 9)
				{
					MessageBox("脉宽最大值不能小于0.9毫秒", "ERROR", MB_OK|MB_ICONERROR);
					m_iUpdateDelay = 2;
					return CDialog::PreTranslateMessage(pMsg);
				}
				if(iValue > 80)
				{
					MessageBox("脉宽最大值不能大于8.0毫秒", "ERROR", MB_OK|MB_ICONERROR);
					m_iUpdateDelay = 2;
					return CDialog::PreTranslateMessage(pMsg);
				}
				m_SliderMax.SetPos(iValue);
				ChangePulseWidth();
			}
			else if(pw == GetDlgItem(IDC_EDIT22))
			{
				if(m_hDevice == NULL)
				{
					MessageBox("未连接一体机设备，无法设置");
					return CDialog::PreTranslateMessage(pMsg);
				}
				if(m_iPannelStyle != 1)
				{
					MessageBox("非数控板无法设置运行模式", "INFO", MB_OK);
					return CDialog::PreTranslateMessage(pMsg);
				}
				GetDlgItem(IDC_EDIT17)->SetFocus();
				GetDlgItemText(IDC_EDIT22, szTempText, 20);
				iValue = atoi(szTempText);
				if(iValue < 0 || iValue > 8)
				{
					MessageBox("等级只能为0到8级之间", "ERROR", MB_OK|MB_ICONERROR);
					m_iUpdateDelay = 2;
					return CDialog::PreTranslateMessage(pMsg);
				}
				CString strCmd;
				strCmd.Format("SetControllPannelPulseLevel,Level[%d]", iValue);
				char szRet[256] = {0};
				int iRetLen = 0;
				HVAPI_ExecCmd(m_hDevice, strCmd.GetBuffer(), szRet, 256, &iRetLen);
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CFrontPannelTestDlg::OnCancel()
{
	if(MessageBox("确定退出?", "警告", MB_YESNO) == IDNO)
	return;
	m_fIsConnected = false;
	if(WaitForSingleObject(m_pUpDateParamThread, 5000) == WAIT_TIMEOUT)
	{
		TerminateProcess(m_pUpDateParamThread, 0);
	}
	m_pUpDateParamThread = NULL;
	if(m_hDevice != NULL)
	{
		HVAPI_Close(m_hDevice);
		m_hDevice = NULL;
	}
	CDialog::OnCancel();
}

void CFrontPannelTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFrontPannelTestDlg::OnPaint() 
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
HCURSOR CFrontPannelTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFrontPannelTestDlg::OnBnClickedButton1()
{
	if(m_fIsConnected)
	{
		::EnableWindow(GetDlgItem(IDC_IPADDRESS1)->GetSafeHwnd(), false);
		::EnableWindow(GetDlgItem(IDC_BUTTON1)->GetSafeHwnd(), false);
		m_fIsConnected = FALSE;
		WaitForSingleObject(m_pUpDateParamThread, 5000);
		m_pUpDateParamThread = NULL;
		//HVAPI_SetCallBack(m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_JPEG_FRAME, NULL);
		HVAPI_SetCallBack(m_hDevice, NULL, NULL, STREAM_TYPE_IMAGE, NULL);
		HVAPI_Close(m_hDevice);
		m_hDevice = NULL;
		m_fIsConnected = FALSE;
		SetDlgItemText(IDC_BUTTON1, "连接设备");
		::EnableWindow(GetDlgItem(IDC_BUTTON1)->GetSafeHwnd(), true);
		::EnableWindow(GetDlgItem(IDC_IPADDRESS1)->GetSafeHwnd(), true);
	}
	else
	{
		DWORD dwDeviceIP = 0;
		m_DeviceIP.GetAddress(dwDeviceIP);
		if(dwDeviceIP == 0 || dwDeviceIP == 0xFFFFFFFF)
		{
			MessageBox("非法IP", "ERROR", MB_OK|MB_ICONERROR);
			return;
		}
		char szIP[20];
		sprintf(szIP, "%d.%d.%d.%d", (dwDeviceIP>>24)&0xFF, 
			(dwDeviceIP>>16)&0xFF, (dwDeviceIP>>8)&0xFF, (dwDeviceIP)&0xFF);
		::EnableWindow(GetDlgItem(IDC_IPADDRESS1)->GetSafeHwnd(), false);
		::EnableWindow(GetDlgItem(IDC_BUTTON1)->GetSafeHwnd(), false);
		m_hDevice = HVAPI_Open(szIP, NULL);
#ifdef OLD_XML_FORMAT
		m_fIsNewProtuls = FALSE;
#else
		m_fIsNewProtuls = TRUE;
#endif
		
		if(m_hDevice == NULL)
		{
			MessageBox("连接设备失败", "ERROR", MB_OK|MB_ICONERROR);
			::EnableWindow(GetDlgItem(IDC_BUTTON1)->GetSafeHwnd(), true);
			::EnableWindow(GetDlgItem(IDC_IPADDRESS1)->GetSafeHwnd(), true);
			return;
		}
		ShowPannelVersion();
		m_fIsConnected = TRUE;
		m_fIsUpDataConfigInfo = TRUE;
		m_pUpDateParamThread = AfxBeginThread(UpDateParamThread, this);
		SetDlgItemText(IDC_BUTTON1, "断开连接");
		CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
		if(pBtn->GetCheck() == 1)
		{
			//if(HVAPI_SetCallBack(m_hDevice, HvCamCallbackJpeg, this, 0, CALLBACK_TYPE_JPEG_FRAME, "SetImgType,EnableRecogVideo[1]") != S_OK)
			if(HVAPI_SetCallBack(m_hDevice, HvCamCallbackJpeg, this, STREAM_TYPE_IMAGE, "SetImgType,EnableRecogVideo[1]") != S_OK)
			{
				MessageBox("设置图像回调失败，无法显示视频", "ERROR", MB_OK|MB_ICONERROR);
				::EnableWindow(GetDlgItem(IDC_BUTTON1)->GetSafeHwnd(), true);
				::EnableWindow(GetDlgItem(IDC_IPADDRESS1)->GetSafeHwnd(), true);
				return;
			}
			m_fIsShowVideo = TRUE;
		}
		::EnableWindow(GetDlgItem(IDC_BUTTON1)->GetSafeHwnd(), true);
	}
}

void CFrontPannelTestDlg::ShowPannelVersion()
{
	if(m_hDevice == NULL) return;
	char szRetBuf[1024] = {0};
	int iRetLen = 0;
	if(HVAPI_GetInfo(m_hDevice, "GetControllPannelVersion", 
		szRetBuf, 1024, &iRetLen) != S_OK)
	{
		SetDlgItemText(IDC_STATIC_MV, "控制板主程序版本: 未知");
		SetDlgItemText(IDC_STATIC_SV, "控制板升级程序版本: 未知");
		return;
	}
	char szVersionStr[256] = {0};
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelVersion", "MainProgram", szVersionStr) == S_OK)
	{
		char szInfo[512] = {0};
		sprintf(szInfo, "控制板主程序版本:%s", szVersionStr);
		SetDlgItemText(IDC_STATIC_MV, szInfo);
	}
	memset(szVersionStr, 0, 256);
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelVersion", "UpdataProgram", szVersionStr) == S_OK)
	{
		char szInfo[512] = {0};
		sprintf(szInfo, "控制板升级程序版本:%s", szVersionStr);
		SetDlgItemText(IDC_STATIC_SV, szInfo);
	}
}

void CFrontPannelTestDlg::OnBnClickedCheck1()
{
	int iMode;
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
	iMode = pBtn->GetCheck();
	if(iMode == 0)
	{
		if(m_fIsShowVideo)
		{
			m_fIsShowVideo = FALSE;
			//HVAPI_SetCallBack(m_hDevice, NULL, NULL, 0, CALLBACK_TYPE_JPEG_FRAME, NULL);
			HVAPI_SetCallBack(m_hDevice, NULL, NULL, STREAM_TYPE_IMAGE, NULL);
			SetDlgItemText(IDC_STATIC_FRAMINFO, "快门时间：0 增益：0 R=0 G=0 B=0");
		}
	}
	else
	{
		if(m_fIsShowVideo)return;
		//if(HVAPI_SetCallBack(m_hDevice, HvCamCallbackJpeg, this, 0, CALLBACK_TYPE_JPEG_FRAME, "SetImgType,EnableRecogVideo[1]") != S_OK)
		if(HVAPI_SetCallBack(m_hDevice, HvCamCallbackJpeg, this, STREAM_TYPE_IMAGE, "SetImgType,EnableRecogVideo[1]") != S_OK)
		{
			MessageBox("设置图像回调失败，无法显示视频", "ERROR", MB_OK|MB_ICONERROR);
			::EnableWindow(GetDlgItem(IDC_BUTTON1)->GetSafeHwnd(), true);
			pBtn->SetCheck(0);
			return;
		}
		m_fIsShowVideo = TRUE;
	}
}

UINT CFrontPannelTestDlg::UpDateParamThread(LPVOID pData)
{
	if(pData == NULL)
	{
		return -1;
	}
	CFrontPannelTestDlg* pDlg = (CFrontPannelTestDlg*)pData;
	while(pDlg->m_fIsConnected)
	{
		if(pDlg->m_hDevice == NULL)
		{
			Sleep(1000);
			continue;
		}
		if(pDlg->m_iUpdateDelay > 0)
		{
			pDlg->m_iUpdateDelay--;
			if(pDlg->m_iUpdateDelay <= 0)
			{
				pDlg->m_iUpdateDelay = -1;
				pDlg->m_fIsUpDataConfigInfo = TRUE;
			}
		}
		pDlg->ShowPannelStyle();
		if(pDlg->m_iPannelStyle != 1) continue;
		pDlg->GetControllPannelAutoRunMode();
		pDlg->ShowPannelStatus();
		pDlg->ShowDeviceStatus();
		pDlg->ShowPulseInfo();
		pDlg->ShowFlashInfo();
		pDlg->m_fIsUpDataConfigInfo = FALSE;
		Sleep(500);
	}
	return 0;
}

void CFrontPannelTestDlg::ShowPannelStatus()
{
	if(m_hDevice == NULL) return;
	char szTemperature[20] = {0};
	char szWorkMode[20] = {0};
	char szWorkStatus[20] = {0};
	int iWorkMode = 0;
	int iWorkStatus = 0;
	char szRetBuf[256] = {0};
	int iRetLen = 0;
	if(HVAPI_GetInfo(m_hDevice, "GetControllPannelTemperature", 
		szRetBuf, 256, &iRetLen) != S_OK)
	{
		sprintf(szTemperature, "0.0℃");
	}
	else
	{
		if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelTemperature", "PannelTemperature", szTemperature) != S_OK)
		{
			sprintf(szTemperature, "0.0℃");
		}
	}

	if(HVAPI_GetInfo(m_hDevice, "GetControllPannelStatus", 
		szRetBuf, 256, &iRetLen) != S_OK)
	{
		sprintf(szWorkMode, "未知");
		sprintf(szWorkStatus, "停止");
	}
	else
	{
		if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelStatus", "WorkMode", szWorkMode) != S_OK)
		{
			iWorkMode = -1;
		}
		if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelStatus", "WorkStatus", szWorkStatus) != S_OK)
		{
			iWorkStatus = -1;
		}
	}
	iWorkMode = atoi(szWorkMode);
	switch(iWorkMode)
	{
	case 1: sprintf(szWorkMode, "正常"); break;
	case 2: sprintf(szWorkMode, "升级"); break;
	case 3: sprintf(szWorkMode, "测试"); break;
	default: sprintf(szWorkMode, "未知"); break;
	}
	iWorkStatus = atoi(szWorkStatus);
	if(iWorkStatus == 1)
	{
		sprintf(szWorkStatus, "运行");
	}
	else
	{
		sprintf(szWorkStatus, "停止");
	}

	sprintf(szRetBuf, "工作模式:%s 工作状态:%s 温度:%s",
		szWorkMode, szWorkStatus, szTemperature);
	SetDlgItemText(IDC_STATIC_INFO, szRetBuf);
	if(m_fIsUpDataConfigInfo)
	{
		if(iWorkMode <= 1 || iWorkMode > 3)
		m_ComBoxWorkMode.SetCurSel(0);
		else
		m_ComBoxWorkMode.SetCurSel(iWorkMode-1);
		if(iWorkStatus == 1)
		m_ComBoxWorkStatus.SetCurSel(1);
		else
		m_ComBoxWorkStatus.SetCurSel(0);
	}
}

void CFrontPannelTestDlg::ShowDeviceStatus()
{
	if(m_hDevice == NULL) return;
	char szRetBuf[512] = {0};
	int iRetLen = 0;
	if(HVAPI_GetInfo(m_hDevice, "GetControllPannelDeviceStatus", 
		szRetBuf, 512, &iRetLen) != S_OK)
	{
		SetDlgItemText(IDC_STATIC_PL, "偏光镜状态:未知");
		SetDlgItemText(IDC_STATIC_PA, "地灯状态:未知");
		return;
	}
	char szValue[20] = {0};
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelDeviceStatus", "PolarizingPrismStatus", szValue) != S_OK)
	{
		SetDlgItemText(IDC_STATIC_PL, "偏光镜状态:未知");
		if(m_fIsUpDataConfigInfo)
		{
			m_ComBoxPLMode.SetCurSel(0);
		}
	}
	else
	{
		int iPolarizingPrismStatus = atoi(szValue);
		if(iPolarizingPrismStatus == -1)
		{
			SetDlgItemText(IDC_STATIC_PL, "偏光镜状态:未启用");
			if(m_fIsUpDataConfigInfo)
			m_ComBoxPLMode.SetCurSel(0);
		}
		else if(iPolarizingPrismStatus == 1)
		{
			SetDlgItemText(IDC_STATIC_PL, "偏光镜状态:打开");
			if(m_fIsUpDataConfigInfo)
			m_ComBoxPLMode.SetCurSel(1);
		}
		else
		{
			SetDlgItemText(IDC_STATIC_PL, "偏光镜状态:关闭");
			if(m_fIsUpDataConfigInfo)
			m_ComBoxPLMode.SetCurSel(0);
		}
	}
	memset(szValue, 0, 20);
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelDeviceStatus", "PalanceLightStatu", szValue) != S_OK)
	{
		SetDlgItemText(IDC_STATIC_PA, "地灯状态:未知");
	}
	else
	{
		int iPalanceLightStatus = atoi(szValue);
		if(iPalanceLightStatus == 0)
		{
			SetDlgItemText(IDC_STATIC_PA, "地灯状态:熄灭");
		}
		else
		{
			SetDlgItemText(IDC_STATIC_PA, "地灯状态:打开");
		}
	}
}

void CFrontPannelTestDlg::ShowPulseInfo()
{
	if(m_hDevice == NULL) return;
	char szRetBuf[1024] = {0};
	int iRetLen = 0;
	if(HVAPI_GetInfo(m_hDevice, "GetControllPannelPulseInfo", 
		szRetBuf, 1024, &iRetLen) != S_OK)
	{
		m_ProgressPulse1.SetPos(0);
		m_ProgressPulse2.SetPos(0);
		m_ProgressPulse3.SetPos(0);
		m_ProgressPulse4.SetPos(0);
		SetDlgItemText(IDC_STATIC_PUL, "补光脉宽等级：未知");
		return;
	}
	int iPulseLevel = -1;
	int iPulseStep = -1;
	int iPulseWidthMin = -1;
	int iPulseWidthMax = -1;
	char szValue[20] = {0};
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelPulseInfo", "PulseLevel", szValue) != S_OK)
	{
		iPulseLevel = -1;
	}
	else
	{
		iPulseLevel = atoi(szValue);
	}
	memset(szValue, 0, 20);
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelPulseInfo", "PulseStep", szValue) != S_OK)
	{
		iPulseStep = -1;
	}
	else
	{
		iPulseStep = atoi(szValue);
	}
	memset(szValue, 0, 20);
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelPulseInfo", "PulseWidthMin", szValue) != S_OK)
	{
		iPulseWidthMin = -1;
	}
	else
	{
		iPulseWidthMin = atoi(szValue);
	}
	memset(szValue, 0, 20);
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelPulseInfo", "PulseWidthMax", szValue) != S_OK)
	{
		iPulseWidthMax = -1;
	}
	else
	{
		iPulseWidthMax = atoi(szValue);
	}
	int iPulseWidth;
	if(iPulseLevel > 0)
	iPulseWidth = iPulseWidthMin + (int)((iPulseLevel - 1) * iPulseStep);
	else
	iPulseWidth = 0;
	if(iPulseWidth < 0) iPulseWidth = 0;
	else if(iPulseWidth > 80) iPulseWidth = 80;
	if(iPulseLevel > 0 && iPulseWidth < 1 ) iPulseWidth = 1;
	m_ProgressPulse1.SetPos(iPulseWidth);
	m_ProgressPulse2.SetPos(iPulseWidth);
	m_ProgressPulse3.SetPos(iPulseWidth);
	m_ProgressPulse4.SetPos(iPulseWidth);
	sprintf(szRetBuf, "补光脉宽等级:%d级", iPulseLevel);
	SetDlgItemText(IDC_STATIC_PUL, szRetBuf);
	sprintf(szRetBuf, "%2.1fms", iPulseWidth/10.);
	SetDlgItemText(IDC_EDIT1, szRetBuf);
	SetDlgItemText(IDC_EDIT2, szRetBuf);
	SetDlgItemText(IDC_EDIT3, szRetBuf);
	SetDlgItemText(IDC_EDIT4, szRetBuf);
	if(m_fIsUpDataConfigInfo)
	{
		m_SliderMin.SetPos(iPulseWidthMin);
		m_SliderMax.SetPos(iPulseWidthMax);
		sprintf(szRetBuf, "%2.1fms", iPulseWidthMin/10.);
		SetDlgItemText(IDC_EDIT17, szRetBuf);
		sprintf(szRetBuf, "%2.1fms", iPulseWidthMax/10.);
		SetDlgItemText(IDC_EDIT18, szRetBuf);
	}
}

void CFrontPannelTestDlg::ShowFlashInfo()
{
	if(m_hDevice == NULL) return;
	char szRetBuf[1024] = {0};
	int iRetLen = 0;
	if(HVAPI_GetInfo(m_hDevice, "GetControllPannelFlashInfo", 
		szRetBuf, 1024, &iRetLen) != S_OK)
	{
		SetDlgItemText(IDC_EDIT5, "0ms");
		SetDlgItemText(IDC_EDIT6, "未知");
		SetDlgItemText(IDC_EDIT7, "未知");
		SetDlgItemText(IDC_EDIT8, "未知");

		SetDlgItemText(IDC_EDIT9, "0ms");
		SetDlgItemText(IDC_EDIT10, "未知");
		SetDlgItemText(IDC_EDIT11, "未知");
		SetDlgItemText(IDC_EDIT12, "未知");

		SetDlgItemText(IDC_EDIT13, "0ms");
		SetDlgItemText(IDC_EDIT14, "未知");
		SetDlgItemText(IDC_EDIT15, "未知");
		SetDlgItemText(IDC_EDIT16, "未知");
		return;
	}
	char szValue[20] = {0};
	int iValue = 0;
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelFlashInfo", "Flash1PulseWidth", szValue) != S_OK)
	{
		SetDlgItemText(IDC_EDIT5, "0ms");
	}
	else
	{
		iValue = atoi(szValue);
		sprintf(szValue, "%2.1fms", iValue/10.);
		SetDlgItemText(IDC_EDIT5, szValue);
		if(m_fIsUpDataConfigInfo)
		{
			sprintf(szValue, "%2.1fms", iValue/10.);
			SetDlgItemText(IDC_EDIT19, szValue);
		}
	}
	memset(szValue, 0, 20);
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelFlashInfo", "Flash1Polarity", szValue) != S_OK)
	{
		SetDlgItemText(IDC_EDIT6, "未知");
	}
	else
	{
		iValue = atoi(szValue);
		if(iValue == 0)
		SetDlgItemText(IDC_EDIT6, "负极");
		else
		SetDlgItemText(IDC_EDIT6, "正极");
		if(m_fIsUpDataConfigInfo)
		{
			m_ComBoxFlash1Polarity.SetCurSel(iValue);
		}
	}
	memset(szValue, 0, 20);
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelFlashInfo", "Flash1Coupling", szValue) != S_OK)
	{
		SetDlgItemText(IDC_EDIT7, "未知");
	}
	else
	{
		iValue = atoi(szValue);
		if(iValue == 0)
			SetDlgItemText(IDC_EDIT7, "不耦合");
		else
			SetDlgItemText(IDC_EDIT7, "耦合");
		if(m_fIsUpDataConfigInfo)
		{
			m_ComBoxFlash1Coupling.SetCurSel(iValue);
		}
	}
	memset(szValue, 0, 20);
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelFlashInfo", "Flash1ResistorMode", szValue) != S_OK)
	{
		SetDlgItemText(IDC_EDIT8, "未知");
	}
	else
	{
		iValue = atoi(szValue);
		if(iValue == 0)
			SetDlgItemText(IDC_EDIT8, "短路");
		else
			SetDlgItemText(IDC_EDIT8, "电平");
		if(m_fIsUpDataConfigInfo)
		{
			m_ComBoxFlash1ResistorMode.SetCurSel(iValue);
		}
	}

	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelFlashInfo", "Flash2PulseWidth", szValue) != S_OK)
	{
		SetDlgItemText(IDC_EDIT9, "0ms");
	}
	else
	{
		iValue = atoi(szValue);
		sprintf(szValue, "%2.1fms", iValue/10.);
		SetDlgItemText(IDC_EDIT9, szValue);
		if(m_fIsUpDataConfigInfo)
		{
			sprintf(szValue, "%2.1fms", iValue/10.);
			SetDlgItemText(IDC_EDIT20, szValue);
		}
	}
	memset(szValue, 0, 20);
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelFlashInfo", "Flash2Polarity", szValue) != S_OK)
	{
		SetDlgItemText(IDC_EDIT10, "未知");
	}
	else
	{
		iValue = atoi(szValue);
		if(iValue == 0)
			SetDlgItemText(IDC_EDIT10, "负极");
		else
			SetDlgItemText(IDC_EDIT10, "正极");
		if(m_fIsUpDataConfigInfo)
		{
			m_ComBoxFlash2Polarity.SetCurSel(iValue);
		}
	}
	memset(szValue, 0, 20);
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelFlashInfo", "Flash2Coupling", szValue) != S_OK)
	{
		SetDlgItemText(IDC_EDIT11, "未知");
	}
	else
	{
		iValue = atoi(szValue);
		if(iValue == 0)
			SetDlgItemText(IDC_EDIT11, "不耦合");
		else
			SetDlgItemText(IDC_EDIT11, "耦合");
		if(m_fIsUpDataConfigInfo)
		{
			m_ComBoxFlash2Coupling.SetCurSel(iValue);
		}
	}
	memset(szValue, 0, 20);
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelFlashInfo", "Flash2ResistorMode", szValue) != S_OK)
	{
		SetDlgItemText(IDC_EDIT12, "未知");
	}
	else
	{
		iValue = atoi(szValue);
		if(iValue == 0)
			SetDlgItemText(IDC_EDIT12, "短路");
		else
			SetDlgItemText(IDC_EDIT12, "电平");
		if(m_fIsUpDataConfigInfo)
		{
			m_ComBoxFlash2ResistorMode.SetCurSel(iValue);
		}
	}

	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelFlashInfo", "Flash3PulseWidth", szValue) != S_OK)
	{
		SetDlgItemText(IDC_EDIT13, "0ms");
	}
	else
	{
		iValue = atoi(szValue);
		sprintf(szValue, "%2.1fms", iValue/10.);
		SetDlgItemText(IDC_EDIT13, szValue);
		if(m_fIsUpDataConfigInfo)
		{
			sprintf(szValue, "%2.1fms", iValue/10.);
			SetDlgItemText(IDC_EDIT21, szValue);
		}
	}
	memset(szValue, 0, 20);
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelFlashInfo", "Flash3Polarity", szValue) != S_OK)
	{
		SetDlgItemText(IDC_EDIT14, "未知");
	}
	else
	{
		iValue = atoi(szValue);
		if(iValue == 0)
			SetDlgItemText(IDC_EDIT14, "负极");
		else
			SetDlgItemText(IDC_EDIT14, "正极");
		if(m_fIsUpDataConfigInfo)
		{
			m_ComBoxFlash3Polarity.SetCurSel(iValue);
		}
	}
	memset(szValue, 0, 20);
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelFlashInfo", "Flash3Coupling", szValue) != S_OK)
	{
		SetDlgItemText(IDC_EDIT15, "未知");
	}
	else
	{
		iValue = atoi(szValue);
		if(iValue == 0)
			SetDlgItemText(IDC_EDIT15, "不耦合");
		else
			SetDlgItemText(IDC_EDIT15, "耦合");
		if(m_fIsUpDataConfigInfo)
		{
			m_ComBoxFlash3Coupling.SetCurSel(iValue);
		}
	}
	memset(szValue, 0, 20);
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelFlashInfo", "Flash3ResistorMode", szValue) != S_OK)
	{
		SetDlgItemText(IDC_EDIT16, "未知");
	}
	else
	{
		iValue = atoi(szValue);
		if(iValue == 0)
			SetDlgItemText(IDC_EDIT16, "短路");
		else
			SetDlgItemText(IDC_EDIT16, "电平");
		if(m_fIsUpDataConfigInfo)
		{
			m_ComBoxFlash3ResistorMode.SetCurSel(iValue);
		}
	}
}

void CFrontPannelTestDlg::ShowPannelStyle()
{
	if(m_hDevice == NULL) return;
	if(!m_fIsConnected)
	{
		m_iPannelStyle = -1;
		SetDlgItemText(IDC_STATIC_PST, "控制板类型：未知");
		return;
	}
	char szRetBuf[1024] = {0};
	int iRetLen = 0;
	if(HVAPI_GetInfo(m_hDevice, "GetControllPannelAutoRunStle", 
		szRetBuf, 1024, &iRetLen) != S_OK)
	{
		m_iPannelStyle = -1;
		SetDlgItemText(IDC_STATIC_PST, "控制板类型：未知");
		return;
	}
	char szValue[20] = {0};
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelAutoRunStle", "Style", szValue) != S_OK)
	{
		m_iPannelStyle = -1;
		SetDlgItemText(IDC_STATIC_PST, "控制板类型：未知");
		return;
	}
	m_iPannelStyle = atoi(szValue);
	if(m_iPannelStyle == 0)
	{
		SetDlgItemText(IDC_STATIC_PST, "控制板类型：拨码板");
	}
	else
	{
		SetDlgItemText(IDC_STATIC_PST, "控制板类型：数控板");
	}
}

void CFrontPannelTestDlg::OnCbnSelchangeCombo1()
{
	if(m_iPannelStyle != 1)
	{
		MessageBox("非数控板无法设置运行模式", "INFO", MB_OK);
		return;
	}
	int iModeValue = m_ComBoxWorkMode.GetCurSel();
	if(iModeValue == 2)
	{
		MessageBox("当前版本禁止进入测试模式");
		m_fIsUpDataConfigInfo = true;
		ShowPannelStatus();
		ShowDeviceStatus();
		ShowPulseInfo();
		ShowFlashInfo();
		m_fIsUpDataConfigInfo = false;
		return;
	}
	int iStatusValue = m_ComBoxWorkStatus.GetCurSel();
	CString strCmd;
	strCmd.Format("SetControllPannelStatus,WorkMode[%d],WorkStatus[%d]",
		iModeValue+1, iStatusValue);
	char szRet[256] = {0};
	int iRetLen = 0;
	HVAPI_ExecCmd(m_hDevice, strCmd.GetBuffer(), szRet, 256, &iRetLen);
}

void CFrontPannelTestDlg::OnCbnSelchangeCombo2()
{
	if(m_iPannelStyle != 1)
	{
		MessageBox("非数控板无法设置运行模式", "INFO", MB_OK);
		return;
	}
	int iModeValue = m_ComBoxWorkMode.GetCurSel();
	if(iModeValue == 2)
	{
		MessageBox("当前版本禁止进入测试模式");
		m_fIsUpDataConfigInfo = true;
		ShowPannelStatus();
		ShowDeviceStatus();
		ShowPulseInfo();
		ShowFlashInfo();
		m_fIsUpDataConfigInfo = false;
		return;
	}
	int iStatusValue = m_ComBoxWorkStatus.GetCurSel();
	CString strCmd;
	strCmd.Format("SetControllPannelStatus,WorkMode[%d],WorkStatus[%d]",
		iModeValue+1, iStatusValue);
	char szRet[256] = {0};
	int iRetLen = 0;
	HVAPI_ExecCmd(m_hDevice, strCmd.GetBuffer(), szRet, 256, &iRetLen);
}

void CFrontPannelTestDlg::ChangePulseWidth()
{
	int iValueMin = m_SliderMin.GetPos();
	int iValueMax = m_SliderMax.GetPos();
	if(iValueMin > 72)
	{
		m_fIsUpDataConfigInfo = true;
		ShowPannelStatus();
		ShowDeviceStatus();
		ShowPulseInfo();
		ShowFlashInfo();
		m_fIsUpDataConfigInfo = false;
		MessageBox("脉宽最小值不能大于7.2毫秒");
		return;
	}
	else if(iValueMin < 1)
	{
		m_fIsUpDataConfigInfo = true;
		ShowPannelStatus();
		ShowDeviceStatus();
		ShowPulseInfo();
		ShowFlashInfo();
		MessageBox("脉宽最小值不能小于0.1毫秒");
		m_fIsUpDataConfigInfo = false;
		return;
	}
	if(iValueMax < 9)
	{
		m_fIsUpDataConfigInfo = true;
		ShowPannelStatus();
		ShowDeviceStatus();
		ShowPulseInfo();
		ShowFlashInfo();
		m_fIsUpDataConfigInfo = false;
		MessageBox("脉宽最大值不能小于0.9毫秒");
		return;
	}
	if(iValueMin > iValueMax)
	{
		m_fIsUpDataConfigInfo = true;
		ShowPannelStatus();
		ShowDeviceStatus();
		ShowPulseInfo();
		ShowFlashInfo();
		m_fIsUpDataConfigInfo = false;
		MessageBox("脉宽最小值不能大于最大值");
		return;
	}
	CString strCmd;
	strCmd.Format("SetPulseWidthRange,PulseWidthMin[%d],PulseWidthMax[%d]",
		iValueMin, iValueMax);
	char szRet[256] = {0};
	int iRetLen = 0;
	HVAPI_ExecCmd(m_hDevice, strCmd.GetBuffer(), szRet, 256, &iRetLen);
	sprintf(szRet, "%2.1fms", iValueMin/10.);
	SetDlgItemText(IDC_EDIT17, szRet);
	sprintf(szRet, "%2.1fms", iValueMax/10.);
	SetDlgItemText(IDC_EDIT18, szRet);
}

void CFrontPannelTestDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int iID = ((CWnd*)pScrollBar)->GetDlgCtrlID();
	if(iID == IDC_SLIDER1 || iID == IDC_SLIDER2)
	{
		if(m_iPannelStyle != 1)
		{
			MessageBox("非数控板无法设置运行模式", "INFO", MB_OK);
			CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
			return;
		}
		ChangePulseWidth();
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CFrontPannelTestDlg::OnCbnSelchangeCombo3()
{
	if(m_iPannelStyle != 1)
	{
		MessageBox("非数控板无法设置运行模式", "INFO", MB_OK);
		return;
	}
	int iPLMode = m_ComBoxPLMode.GetCurSel();
	CString strCmd;
	strCmd.Format("TestPolarizingPrismMode,Mode[%d]", iPLMode);
	char szRet[256] = {0};
	int iRetLen = 0;
	HVAPI_ExecCmd(m_hDevice, strCmd.GetBuffer(), szRet, 256, &iRetLen);
}

void CFrontPannelTestDlg::OnBnClickedButton2()
{
	if(m_iPannelStyle != 1)
	{
		MessageBox("非数控板无法设置运行模式", "INFO", MB_OK);
		return;
	}
	int iPolarity = 0;
	int iTriggerType = 0;
	int iPulseWidth = 10;
	int iCoupling = 0;
	float fPulseWidth;
	char szPulseWidth[40] = {0};
	GetDlgItemText(IDC_EDIT19, szPulseWidth, 40);
	sscanf(szPulseWidth, "%fms", &fPulseWidth);
	iPulseWidth = (int)(fPulseWidth * 10 + 0.5);
	if(iPulseWidth < 100)
	{
		MessageBox("抓拍补光脉宽不能小于10毫秒");
		m_fIsUpDataConfigInfo = true;
		ShowPannelStatus();
		ShowDeviceStatus();
		ShowPulseInfo();
		ShowFlashInfo();
		m_fIsUpDataConfigInfo = false;
		return;
	}
	else if(iPulseWidth > 4000)
	{
		MessageBox("抓拍补光脉宽不能大于400毫秒");
		m_fIsUpDataConfigInfo = true;
		ShowPannelStatus();
		ShowDeviceStatus();
		ShowPulseInfo();
		ShowFlashInfo();
		m_fIsUpDataConfigInfo = false;
		return;
	}
	iPolarity = m_ComBoxFlash1Polarity.GetCurSel();
	iTriggerType = m_ComBoxFlash1ResistorMode.GetCurSel();
	iCoupling = m_ComBoxFlash1Coupling.GetCurSel();
	CString strCmd;
	strCmd.Format("SetFlashSingle,Channel[%d],Polarity[%d],TriggerType[%d],PulseWidth[%d],Coupling[%d]",
		0, iPolarity, iTriggerType, iPulseWidth, iCoupling);
	char szRet[256] = {0};
	int iRetLen = 0;
	HVAPI_ExecCmd(m_hDevice, strCmd.GetBuffer(), szRet, 256, &iRetLen);
}

void CFrontPannelTestDlg::OnBnClickedButton3()
{
	if(m_iPannelStyle != 1)
	{
		MessageBox("非数控板无法设置运行模式", "INFO", MB_OK);
		return;
	}
	int iPolarity = 0;
	int iTriggerType = 0;
	int iPulseWidth = 10;
	int iCoupling = 0;
	float fPulseWidth;
	char szPulseWidth[40] = {0};
	GetDlgItemText(IDC_EDIT20, szPulseWidth, 40);
	sscanf(szPulseWidth, "%fms", &fPulseWidth);
	iPulseWidth = (int)(fPulseWidth * 10 + 0.5);
	if(iPulseWidth < 100)
	{
		MessageBox("抓拍补光脉宽不能小于10毫秒");
		m_fIsUpDataConfigInfo = true;
		ShowPannelStatus();
		ShowDeviceStatus();
		ShowPulseInfo();
		ShowFlashInfo();
		m_fIsUpDataConfigInfo = false;
		return;
	}
	else if(iPulseWidth > 4000)
	{
		MessageBox("抓拍补光脉宽不能大于400毫秒");
		m_fIsUpDataConfigInfo = true;
		ShowPannelStatus();
		ShowDeviceStatus();
		ShowPulseInfo();
		ShowFlashInfo();
		m_fIsUpDataConfigInfo = false;
		return;
	}
	iPolarity = m_ComBoxFlash2Polarity.GetCurSel();
	iTriggerType = m_ComBoxFlash2ResistorMode.GetCurSel();
	iCoupling = m_ComBoxFlash2Coupling.GetCurSel();
	CString strCmd;
	strCmd.Format("SetFlashSingle,Channel[%d],Polarity[%d],TriggerType[%d],PulseWidth[%d],Coupling[%d]",
		1, iPolarity, iTriggerType, iPulseWidth, iCoupling);
	char szRet[256] = {0};
	int iRetLen = 0;
	HVAPI_ExecCmd(m_hDevice, strCmd.GetBuffer(), szRet, 256, &iRetLen);
}



void CFrontPannelTestDlg::OnBnClickedButton4()
{
	if(m_iPannelStyle != 1)
	{
		MessageBox("非数控板无法设置运行模式", "INFO", MB_OK);
		return;
	}
	int iPolarity = 0;
	int iTriggerType = 0;
	int iPulseWidth = 10;
	int iCoupling = 0;
	float fPulseWidth;
	char szPulseWidth[40] = {0};
	GetDlgItemText(IDC_EDIT21, szPulseWidth, 40);
	sscanf(szPulseWidth, "%fms", &fPulseWidth);
	iPulseWidth = (int)(fPulseWidth * 10 + 0.5);
	if(iPulseWidth < 100)
	{
		MessageBox("抓拍补光脉宽不能小于10毫秒");
		m_fIsUpDataConfigInfo = true;
		ShowPannelStatus();
		ShowDeviceStatus();
		ShowPulseInfo();
		ShowFlashInfo();
		m_fIsUpDataConfigInfo = false;
		return;
	}
	else if(iPulseWidth > 4000)
	{
		MessageBox("抓拍补光脉宽不能大于400毫秒");
		m_fIsUpDataConfigInfo = true;
		ShowPannelStatus();
		ShowDeviceStatus();
		ShowPulseInfo();
		ShowFlashInfo();
		m_fIsUpDataConfigInfo = false;
		return;
	}
	iPolarity = m_ComBoxFlash3Polarity.GetCurSel();
	iTriggerType = m_ComBoxFlash3ResistorMode.GetCurSel();
	iCoupling = m_ComBoxFlash3Coupling.GetCurSel();
	CString strCmd;
	strCmd.Format("SetFlashSingle,Channel[%d],Polarity[%d],TriggerType[%d],PulseWidth[%d],Coupling[%d]",
		2, iPolarity, iTriggerType, iPulseWidth, iCoupling);
	char szRet[256] = {0};
	int iRetLen = 0;
	HVAPI_ExecCmd(m_hDevice, strCmd.GetBuffer(), szRet, 256, &iRetLen);
}

void CFrontPannelTestDlg::OnBnClickedButton6()
{
	if(m_iPannelStyle != 1)
	{
		MessageBox("非数控板无法设置运行模式", "INFO", MB_OK);
		return;
	}
	m_fIsUpDataConfigInfo = true;
	ShowPannelStatus();
	ShowDeviceStatus();
	ShowPulseInfo();
	ShowFlashInfo();
	m_fIsUpDataConfigInfo = false;

}

void CFrontPannelTestDlg::OnBnClickedButton7()
{
	if(m_hDevice == NULL)
	{
		MessageBox("未连接到一体机，无法保存配置参数", "错误", MB_OK|MB_ICONERROR);
		return;
	}
	if(m_iPannelStyle != 1)
	{
		MessageBox("非数控板无法设置运行模式", "INFO", MB_OK);
		return;
	}
	if(MessageBox("确定保存配置参数?", "警告", MB_YESNO) == IDNO)
	return;
	GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);
	int iAutoRunStatus = 0;
	int iPulseWidthMin = 0;
	int iPulseWidthMax = 0;
	int iFlash1PulseWidth = 100;
	int iFlash1Polarity = 0;
	int iFlash1TriggerType = 0;
	int iFlash1Coupling = 0;
	int iFlash2PulseWidth = 100;
	int iFlash2Polarity = 0;
	int iFlash2TriggerType = 0;
	int iFlash2Coupling = 0;
	int iFlash3PulseWidth = 100;
	int iFlash3Polarity = 0;
	int iFlash3TriggerType = 0;
	int iFlash3Coupling = 0;
	iPulseWidthMin = m_SliderMin.GetPos();
	iPulseWidthMax = m_SliderMax.GetPos();
	if(iPulseWidthMin < 1) iPulseWidthMin = 1;
	else if(iPulseWidthMin > 72) iPulseWidthMin = 72;
	if(iPulseWidthMax < 9) iPulseWidthMax = 9;
	else if(iPulseWidthMax > 80) iPulseWidthMax = 80;
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK2);
	iAutoRunStatus = pBtn->GetCheck();

	float fPulseWidth;
	char szPulseWidth[40] = {0};
	GetDlgItemText(IDC_EDIT19, szPulseWidth, 40);
	sscanf(szPulseWidth, "%fms", &fPulseWidth);
	iFlash1PulseWidth = (int)(fPulseWidth * 10);
	if(iFlash1PulseWidth < 100) iFlash1PulseWidth = 100;
	else if(iFlash1PulseWidth > 4000) iFlash1PulseWidth = 4000;
	memset(szPulseWidth, 0, 40);
	GetDlgItemText(IDC_EDIT20, szPulseWidth, 40);
	sscanf(szPulseWidth, "%fms", &fPulseWidth);
	iFlash2PulseWidth = (int)(fPulseWidth * 10);
	if(iFlash2PulseWidth < 100) iFlash2PulseWidth = 100;
	else if(iFlash2PulseWidth > 4000) iFlash2PulseWidth = 4000;
	memset(szPulseWidth, 0, 40);
	GetDlgItemText(IDC_EDIT21, szPulseWidth, 40);
	sscanf(szPulseWidth, "%fms", &fPulseWidth);
	iFlash3PulseWidth = (int)(fPulseWidth * 10);
	if(iFlash3PulseWidth < 100) iFlash3PulseWidth = 100;
	else if(iFlash3PulseWidth > 4000) iFlash3PulseWidth = 4000;
	iFlash1Polarity = m_ComBoxFlash1Polarity.GetCurSel();
	iFlash1TriggerType = m_ComBoxFlash1ResistorMode.GetCurSel();
	iFlash1Coupling = m_ComBoxFlash1Coupling.GetCurSel();
	iFlash2Polarity = m_ComBoxFlash2Polarity.GetCurSel();
	iFlash2TriggerType = m_ComBoxFlash2ResistorMode.GetCurSel();
	iFlash2Coupling = m_ComBoxFlash2Coupling.GetCurSel();
	iFlash3Polarity = m_ComBoxFlash3Polarity.GetCurSel();
	iFlash3TriggerType = m_ComBoxFlash3ResistorMode.GetCurSel();
	iFlash3Coupling = m_ComBoxFlash3Coupling.GetCurSel();

	CString strCmd;
	strCmd.Format("SaveControllPannelConfig,PulseWidthMin[%d],PulseWidthMax[%d],\
Flash1Polarity[%d],Flash1TriggerType[%d],Flash1PulseWidth[%d],Flash1Coupling[%d],\
Flash2Polarity[%d],Flash2TriggerType[%d],Flash2PulseWidth[%d],Flash2Coupling[%d],\
Flash3Polarity[%d],Flash3TriggerType[%d],Flash3PulseWidth[%d],Flash3Coupling[%d],\
AutoRun[%d]",
iPulseWidthMin, iPulseWidthMax,
iFlash1Polarity, iFlash1TriggerType, iFlash1PulseWidth, iFlash1Coupling,
iFlash2Polarity, iFlash2TriggerType, iFlash2PulseWidth, iFlash2Coupling,
iFlash3Polarity, iFlash3TriggerType, iFlash3PulseWidth, iFlash3Coupling,
iAutoRunStatus);
	char szRet[256] = {0};
	int iRetLen = 0;
	if(HVAPI_ExecCmd(m_hDevice, strCmd.GetBuffer(), szRet, 256, &iRetLen) != S_OK)
	{
		MessageBox("保存配置参数失败,发送命令失败！", "ERROR", MB_OK|MB_ICONERROR);
		GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
		return;
	}

	char szRetcode[10] = {0};
#ifdef OLD_XML_FORMAT
	if(FAILED(HvParseXmlCmdRespRetcode(szRet, szRetcode)))
#else
	if(FAILED(HvParseXmlCmdRespRetcode2(szRet, "SaveControllPannelConfig", "RetCode", szRetcode)))
#endif
	
	{
		MessageBox("保存配置参数失败，未知的响应信息！", "ERROR", MB_OK|MB_ICONERROR);
		GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
		return;
	}
	if(atoi(szRetcode) != 0)
	{
		MessageBox("保存配置参数失败，命令执行失败！", "ERROR", MB_OK|MB_ICONERROR);
		GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
		return;
	}
	MessageBox("保存配置参数成功,设备正在复位中，请稍后...", "INFO", MB_OK);
	//iRetLen = 0;
	//HVAPI_ExecCmd(m_hDevice, "ResetDevice", szRet, 128, &iRetLen);
	GetDlgItem(IDC_BUTTON7)->EnableWindow(TRUE);
}

void CFrontPannelTestDlg::OnBnClickedButton5()
{
	//MessageBox("危险，回到公司再测试升级功能", "警告", MB_OK|MB_ICONWARNING);
	//return;
	if(m_hDevice == NULL) 
	{
		MessageBox("未连接识别器，无法升级");
		return;
	}
	if(m_iPannelStyle != 1)
	{
		MessageBox("非数控板无法设置运行模式", "INFO", MB_OK);
		return;
	}
	CUpdateDlg* pDlg = new CUpdateDlg(m_hDevice, this);
	pDlg->DoModal();
	delete pDlg;
	pDlg = NULL;
}

BOOL CFrontPannelTestDlg::GetControllPannelAutoRunMode(void)
{
	if(m_hDevice == NULL)
	{
		return FALSE;
	}
	char szRetBuf[256] = {0};
	int iRetLen = 0;
	if(HVAPI_GetInfo(m_hDevice, "GetControllPannelAutoRunStatus", 
		szRetBuf, 256, &iRetLen) != S_OK)
	{
		return FALSE;
	}
	char szValue[20] = {0};
	int iValue = 0;
	if(ParseXmlInfoRespValue(m_fIsNewProtuls, szRetBuf, "GetControllPannelAutoRunStatus", "Value", szValue) != S_OK)
	{
		return FALSE;
	}
	int iStatus = atoi(szValue);
	if(iStatus != 0 && iStatus != 1)
	{
		return FALSE;
	}
	if(m_fIsConnected == FALSE)return FALSE;
	m_iAutoRunMode = iStatus;
	CButton* pBnt = ((CButton*)GetDlgItem(IDC_CHECK2));
	pBnt->SetCheck(iStatus);
	if(m_iAutoRunMode == 0) //手动
	{
		GetDlgItem(IDC_EDIT22)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO1)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO2)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO3)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON11)->EnableWindow(TRUE);
	}
	else	//自动
	{
		GetDlgItem(IDC_EDIT22)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO2)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO3)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON11)->EnableWindow(FALSE);
	}
	return TRUE;
}

void CFrontPannelTestDlg::OnBnClickedCheck2()
{
	if(m_hDevice == NULL) return;
	if(m_iPannelStyle != 1)
	{
		MessageBox("非数控板无法设置运行模式", "INFO", MB_OK);
		return;
	}
	int iMode;
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK2);
	iMode = pBtn->GetCheck();
	CString strCmd;
	strCmd.Format("SetControllPannelAutoRunStatus,Status[%d]", iMode);
	char szRet[256] = {0};
	int iRetLen = 0;
	HVAPI_ExecCmd(m_hDevice, strCmd.GetBuffer(), szRet, 256, &iRetLen);
	m_iUpdateDelay = 2;
}

void CFrontPannelTestDlg::OnBnClickedButton11()
{
	if(m_hDevice == NULL)
	{
		MessageBox("未连接一体机设备，无法设置");
		return;
	}
	char szRet[256] = {0};
	int iRetLen = 0;
	HVAPI_ExecCmd(m_hDevice, "SoftTriggerCapture", szRet, 256, &iRetLen);
}

void CFrontPannelTestDlg::OnBnClickedButton12()
{
	if(m_hDevice == NULL) return;
	if(m_iPannelStyle != 1)
	{
		MessageBox("非数控板无法恢复控制板默认配置", "INFO", MB_OK);
		return;
	}
	CString strCmd;
	strCmd.Format("SetControllPannelDefParam");
	char szRet[256] = {0};
	int iRetLen = 0;
	if(HVAPI_ExecCmd(m_hDevice, strCmd.GetBuffer(), szRet, 256, &iRetLen) != S_OK)
	{
		MessageBox("恢复默认参数失败", "ERROR", MB_OK|MB_ICONERROR);
		m_iUpdateDelay = 2;
		return;
	}
	char szRetcode[10] = {0};
#ifdef OLD_XML_FORMAT
	if(FAILED(HvParseXmlCmdRespRetcode(szRet, szRetcode)))
#else
	if(FAILED(HvParseXmlCmdRespRetcode2(szRet, "SetControllPannelDefParam", "RetCode", szRetcode)))
#endif
	{
		MessageBox("恢复默认参数失败", "ERROR", MB_OK|MB_ICONERROR);
		m_iUpdateDelay = 2;
		return;
	}
	if(atoi(szRetcode) != 0)
	{
		MessageBox("恢复默认参数失败", "ERROR", MB_OK|MB_ICONERROR);
		m_iUpdateDelay = 2;
		return;
	}
	m_iUpdateDelay = 2;
}
