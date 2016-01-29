#include "stdafx.h"
#include "CUpdateDlg.h"
#include ".\cupdatedlg.h"
#include "FrontPannelTestDlg.h"
#include "WarnningDlg.h"

const unsigned char rgszUpDateFileFlag[66] = 
{
	0x55, 0xAA,
	0x48, 0x57, 0x3A, 0x20, 0x50, 0x53, 0x44, 0x30,  0x35, 0x43, 0x20, 0x76, 0x31, 0x2E, 0x30, 0x20,
	0x53, 0x57, 0x3A, 0x20, 0x4D, 0x61, 0x69, 0x6E,  0x20, 0x50, 0x72, 0x6F, 0x67, 0x72, 0x61, 0x6D,
	0x20, 0x31, 0x2E, 0x30, 0x32, 0x2E, 0x30, 0x33,  0x20, 0x62, 0x75, 0x69, 0x6C, 0x64, 0x20, 0x31,
	0x32, 0x30, 0x31, 0x30, 0x36, 0x20, 0x20, 0x20,  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
};

static const UINT8 crc8_tab[256] = 
{
	0x00,0x07,0x0E,0x09,0x1C,0x1B,0x12,0x15,0x38,0x3F,0x36,0x31,0x24,0x23,0x2A,0x2D,   
	0x70,0x77,0x7E,0x79,0x6C,0x6B,0x62,0x65,0x48,0x4F,0x46,0x41,0x54,0x53,0x5A,0x5D,   
	0xE0,0xE7,0xEE,0xE9,0xFC,0xFB,0xF2,0xF5,0xD8,0xDF,0xD6,0xD1,0xC4,0xC3,0xCA,0xCD,   
	0x90,0x97,0x9E,0x99,0x8C,0x8B,0x82,0x85,0xA8,0xAF,0xA6,0xA1,0xB4,0xB3,0xBA,0xBD,   
	0xC7,0xC0,0xC9,0xCE,0xDB,0xDC,0xD5,0xD2,0xFF,0xF8,0xF1,0xF6,0xE3,0xE4,0xED,0xEA,   
	0xB7,0xB0,0xB9,0xBE,0xAB,0xAC,0xA5,0xA2,0x8F,0x88,0x81,0x86,0x93,0x94,0x9D,0x9A,   
	0x27,0x20,0x29,0x2E,0x3B,0x3C,0x35,0x32,0x1F,0x18,0x11,0x16,0x03,0x04,0x0D,0x0A,   
	0x57,0x50,0x59,0x5E,0x4B,0x4C,0x45,0x42,0x6F,0x68,0x61,0x66,0x73,0x74,0x7D,0x7A,   
	0x89,0x8E,0x87,0x80,0x95,0x92,0x9B,0x9C,0xB1,0xB6,0xBF,0xB8,0xAD,0xAA,0xA3,0xA4,   
	0xF9,0xFE,0xF7,0xF0,0xE5,0xE2,0xEB,0xEC,0xC1,0xC6,0xCF,0xC8,0xDD,0xDA,0xD3,0xD4,   
	0x69,0x6E,0x67,0x60,0x75,0x72,0x7B,0x7C,0x51,0x56,0x5F,0x58,0x4D,0x4A,0x43,0x44,   
	0x19,0x1E,0x17,0x10,0x05,0x02,0x0B,0x0C,0x21,0x26,0x2F,0x28,0x3D,0x3A,0x33,0x34,   
	0x4E,0x49,0x40,0x47,0x52,0x55,0x5C,0x5B,0x76,0x71,0x78,0x7F,0x6A,0x6D,0x64,0x63,   
	0x3E,0x39,0x30,0x37,0x22,0x25,0x2C,0x2B,0x06,0x01,0x08,0x0F,0x1A,0x1D,0x14,0x13,   
	0xAE,0xA9,0xA0,0xA7,0xB2,0xB5,0xBC,0xBB,0x96,0x91,0x98,0x9F,0x8A,0x8D,0x84,0x83,   
	0xDE,0xD9,0xD0,0xD7,0xC2,0xC5,0xCC,0xCB,0xE6,0xE1,0xE8,0xEF,0xFA,0xFD,0xF4,0xF3   
};

#define UPDATE_FILE_SIZE 122880
#define WM_UPDATE_FINISH	(WM_USER + 120)
#define WM_SHOWMESSAGE		(WM_USER + 130)
#define WM_UPDATE_PROGRESS	(WM_USER + 140)

IMPLEMENT_DYNAMIC(CUpdateDlg, CDialog)

extern HRESULT HvParseXmlCmdRespRetcode(char* szXmlBuf, char* szRetcode);
extern HRESULT HvParseXmlCmdRespRetcode2(char* szXmlBuf, char* szCmdName, char* szCmdValueName, char* szCmdValueText);
extern HRESULT ParseXmlInfoRespValue(BOOL fNewProtuol, char* szXmlBuf, char* szInfoName, char* szInfoValueName, char* szInfoValueText);

UINT8 CRCCount(UINT8 iStartValue, UINT8* pBuffer, UINT32 iLen)
{
	UINT8 iValue = iStartValue;
	UINT8* pTempBuffer = pBuffer;
	while(iLen--)
	{
		iValue = crc8_tab[iValue ^ (*pTempBuffer++)];
	}
	return iValue;
}

CUpdateDlg::CUpdateDlg(HVAPI_HANDLE& hHandle, CWnd* pParent /* = NULL */)
: CDialog(CUpdateDlg::IDD, pParent)
{
	m_HvDevice = hHandle;
	m_rgbUpdateFileBuffer = NULL;
	m_iCRCValue = 0;
	m_pProcessUpDateThread = NULL;
}

CUpdateDlg::~CUpdateDlg()
{
	m_pProcessUpDateThread = NULL;
	m_iUpdateStatus = 0;
}

void CUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
}

BEGIN_MESSAGE_MAP(CUpdateDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, OnBnClickedButton9)
	ON_MESSAGE(WM_UPDATE_FINISH, OnShearFinsh)
	ON_MESSAGE(WM_SHOWMESSAGE, OnShowMessage)
	ON_MESSAGE(WM_UPDATE_PROGRESS, OnUpdateProgress)
END_MESSAGE_MAP()

void CUpdateDlg::OnCancel()
{
	if(m_pProcessUpDateThread)
	{
		m_fIsRunningUpdata = FALSE;
	}
	INT iWaitTimes = 0;
	while(WaitForSingleObject(m_pProcessUpDateThread, 5000) == WAIT_TIMEOUT && iWaitTimes < 8)
	{
		iWaitTimes++;
	}
	if(iWaitTimes >= 8)
	{
		TerminateProcess(m_pProcessUpDateThread, 0);
	}

	if(m_rgbUpdateFileBuffer)
	{
		delete[] m_rgbUpdateFileBuffer;
		m_rgbUpdateFileBuffer = NULL;
	}
	CDialog::OnCancel();
}

BOOL CUpdateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int iWorkMode, iWorkStatus;
	GetControllPannelStatus(iWorkMode, iWorkStatus);
	if(iWorkMode == 2)
	{
		char szRetBuf[256] = {0};
		char szValue[20] = {0};
		int iStatus = 0;
		int iPageIndex = 0;
		int iRetLen = 0;
		if(HVAPI_GetInfo(m_HvDevice, "GetControllPannelUpdatingStatus", 
			szRetBuf, 256, &iRetLen) == S_OK)
		{
#ifdef OLD_XML_FORMAT
			if(ParseXmlInfoRespValue(FALSE, szRetBuf, "GetControllPannelUpdatingStatus", "Status", szValue) == S_OK)
#else
			if(ParseXmlInfoRespValue(TRUE, szRetBuf, "GetControllPannelUpdatingStatus", "Status", szValue) == S_OK)
#endif
			
			{
				iStatus = atoi(szValue);
				if(iStatus != -1)
				{
					GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
					GetDlgItem(IDC_BUTTON8)->EnableWindow(FALSE);
					GetDlgItem(IDC_BUTTON9)->EnableWindow(FALSE);
					GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
					m_Progress.SetRange(0, 480);
					m_Progress.SetPos(0);
					m_fIsRunningUpdata = TRUE;
					m_iUpdateStatus = 0;
					m_pProcessUpDateThread = AfxBeginThread(ProcessUpDateThread2, this);
					return TRUE;
				}
			}
		}

	}

	SetDlgItemText(IDC_STATIC_STATUS, "当前状态：未升级");
	m_Progress.SetRange(0, 480);
	m_Progress.SetPos(0);
	return TRUE;
}

void CUpdateDlg::OnBnClickedButton1()
{
	CFileDialog dlg(TRUE);
	if(dlg.DoModal() == IDOK)
	{
		CString strFileName = dlg.GetPathName();
		SetDlgItemText(IDC_EDIT1, strFileName.GetBuffer());
	}
}

void CUpdateDlg::OnBnClickedButton8()
{
	OnCancel();
}

UINT CUpdateDlg::ProcessUpDateThread(LPVOID pUserData)
{
	if(pUserData == NULL)
	{
		return -1;
	}
	CUpdateDlg* pDlg = (CUpdateDlg*)pUserData;
	::EnableWindow((pDlg->GetDlgItem(IDC_BUTTON1))->GetSafeHwnd(), FALSE);
	::EnableWindow((pDlg->GetDlgItem(IDC_BUTTON8))->GetSafeHwnd(), FALSE);
	::EnableWindow((pDlg->GetDlgItem(IDC_BUTTON9))->GetSafeHwnd(), FALSE);
	::EnableWindow((pDlg->GetDlgItem(IDC_EDIT1))->GetSafeHwnd(), FALSE);
	while(pDlg->m_fIsRunningUpdata)
	{
		switch(pDlg->m_iUpdateStatus)
		{
		case 0:
			//pDlg->SetDlgItemText(IDC_STATIC_STATUS, "当前状态：正在检测升级文件合法性");
			pDlg->SendMessage(WM_SHOWMESSAGE, 0, (LPARAM)"当前状态：正在检测升级文件合法性");
			if(pDlg->CheckUpdateFile() != S_OK)
			{
				pDlg->MessageBox("错误的升级文件类型，无法升级", "ERROR", MB_OK|MB_ICONERROR);
				goto EXIT;
			}
			Sleep(1000);
			pDlg->m_iUpdateStatus = 11;
			break;
		case 1:
			{
				//pDlg->SetDlgItemText(IDC_STATIC_STATUS, "当前状态：设置控制板进入升级模式");
				pDlg->SendMessage(WM_SHOWMESSAGE, 0, (LPARAM)"当前状态：设置控制板进入升级模式");
				char szRet[256] = {0};
				int iRetLen = 0;
				if(HVAPI_ExecCmd(pDlg->m_HvDevice, "SetControllPannelStatus,WorkMode[2],WorkStatus[0]", szRet, 256, &iRetLen) != S_OK)
				{
					pDlg->MessageBox("切换控制板进入升级模式失败,升级失败", "ERROR", MB_OK|MB_ICONERROR);
					goto EXIT;
				}
				char szRetcode[10] = {0};
#ifdef OLD_XML_FORMAT
				if(FAILED(HvParseXmlCmdRespRetcode(szRet, szRetcode)))
#else
				if(FAILED(HvParseXmlCmdRespRetcode2(szRet, "SetControllPannelStatus", "RetCode", szRetcode)))
#endif
				
				{
					pDlg->MessageBox("切换控制板进入升级模式失败,升级失败", "ERROR", MB_OK|MB_ICONERROR);
					goto EXIT;
				}
				if(atoi(szRetcode) != 0)
				{
					pDlg->MessageBox("切换控制板进入升级模式失败,升级失败", "ERROR", MB_OK|MB_ICONERROR);
					goto EXIT;
				}
				pDlg->m_iUpdateStatus = 2;
				Sleep(1000);
			}
			break;
		case 2:
			//pDlg->SetDlgItemText(IDC_STATIC_STATUS, "当前状态：上传升级文件");
			pDlg->SendMessage(WM_SHOWMESSAGE, 0, (LPARAM)"当前状态：上传升级文件");
			if(HVAPI_SendControllPannelUpdateFile(pDlg->m_HvDevice, (BYTE*)pDlg->m_rgbUpdateFileBuffer, UPDATE_FILE_SIZE) != S_OK)
			{
				pDlg->MessageBox("上传升级文件失败，升级失败", "ERROR", MB_OK|MB_ICONERROR);
				goto EXIT;
			}
			pDlg->m_iUpdateStatus = 3;
			pDlg->m_Progress.SetRange(0, 480);
			break;
		case 3:
			{
				char szRetBuf[256] = {0};
				char szValue[20] = {0};
				int iStatus = 0;
				int iPageIndex = 0;
				int iRetLen = 0;
				if(HVAPI_GetInfo(pDlg->m_HvDevice, "GetControllPannelUpdatingStatus", 
					szRetBuf, 256, &iRetLen) != S_OK)
				{
					Sleep(100);
					continue;
				}
#ifdef OLD_XML_FORMAT
				if(ParseXmlInfoRespValue(FALSE, szRetBuf, "GetControllPannelUpdatingStatus", "Status", szValue) != S_OK)
#else
				if(ParseXmlInfoRespValue(TRUE, szRetBuf, "GetControllPannelUpdatingStatus", "Status", szValue) != S_OK)
#endif
				
				{
					Sleep(100);
					continue;
				}
				iStatus = atoi(szValue);
				memset(szValue, 0, 20);
#ifdef OLD_XML_FORMAT
				if(ParseXmlInfoRespValue(FALSE, szRetBuf, "GetControllPannelUpdatingStatus", "PageIndex", szValue) != S_OK)
#else
				if(ParseXmlInfoRespValue(TRUE, szRetBuf, "GetControllPannelUpdatingStatus", "PageIndex", szValue) != S_OK)
#endif
				{
					Sleep(100);
					continue;
				}
				iPageIndex = atoi(szValue);
				char szTemp[50] = {0};
				sprintf(szTemp, "正在烧写Flash，第%d页...", iPageIndex);
				//pDlg->SetDlgItemText(IDC_STATIC_STATUS, szTemp);
				pDlg->SendMessage(WM_SHOWMESSAGE, 0, (LPARAM)szTemp);
				//pDlg->m_Progress.SetPos(iPageIndex);
				pDlg->SendMessage(WM_UPDATE_PROGRESS, (WPARAM)iPageIndex, 0);
				if(iPageIndex == 480 && iStatus == -1)
				{
					//pDlg->SetDlgItemText(IDC_STATIC_STATUS, "当前状态：数据更新完成，检测数据完整性");
					pDlg->SendMessage(WM_SHOWMESSAGE, 0, (LPARAM)"当前状态：数据更新完成，检测数据完整性");
					pDlg->m_iUpdateStatus = 4;
				}
			}
			break;
		case 4:
			{
				char szRetBuf[256] = {0};
				char szValue[20] = {0};
				int iCRCValue = -1;
				int iRetLen = 0;
				if(HVAPI_GetInfo(pDlg->m_HvDevice, "GetControllPannelCRCValue", 
					szRetBuf, 256, &iRetLen) != S_OK)
				{
					Sleep(100);
					continue;
				}
#ifdef OLD_XML_FORMAT
				if(ParseXmlInfoRespValue(FALSE, szRetBuf, "GetControllPannelCRCValue", "Value", szValue) != S_OK)
#else
				if(ParseXmlInfoRespValue(TRUE, szRetBuf, "GetControllPannelCRCValue", "Value", szValue) != S_OK)
#endif
				{
					Sleep(100);
					continue;
				}
				iCRCValue = atoi(szValue);
				if(pDlg->m_iCRCValue != iCRCValue)
				{
					pDlg->MessageBox("控制板主程序校验失败，升级失败", "ERROR", MB_OK|MB_ICONERROR);
					goto EXIT;
				}
				pDlg->m_iUpdateStatus = 5;
			}
			break;
		case 5:
			{
				//pDlg->SetDlgItemText(IDC_STATIC_STATUS, "当前状态：切换控制板进入正常模式");
				pDlg->SendMessage(WM_SHOWMESSAGE, 0, (LPARAM)"当前状态：切换控制板进入正常模式");
				char szRet[256] = {0};
				int iRetLen = 0;
				if(HVAPI_ExecCmd(pDlg->m_HvDevice, "SetControllPannelStatus,WorkMode[1],WorkStatus[0]", szRet, 256, &iRetLen) != S_OK)
				{
					pDlg->MessageBox("切换控制板进入正常模式失败", "ERROR", MB_OK|MB_ICONERROR);
					goto EXIT;
				}
				char szRetcode[10] = {0};
#ifdef OLD_XML_FORMAT
				if(FAILED(HvParseXmlCmdRespRetcode(szRet, szRetcode)))
#else
				if(FAILED(HvParseXmlCmdRespRetcode2(szRet, "SetControllPannelStatus", "RetCode", szRetcode)))
#endif
				
				{
					pDlg->MessageBox("切换控制板进入正常模式失败", "ERROR", MB_OK|MB_ICONERROR);
					goto EXIT;
				}
				if(atoi(szRetcode) != 0)
				{
					pDlg->MessageBox("切换控制板进入正常模式失败", "ERROR", MB_OK|MB_ICONERROR);
					goto EXIT;
				}
				pDlg->MessageBox("升级控制完成", "INFO", MB_OK);
				goto EXIT;
			}
			break;
		case 11:
			{
				int iWorkMode, iWorkStatus;
				pDlg->GetControllPannelStatus(iWorkMode, iWorkStatus);
				if(iWorkMode == 2)
				{
					pDlg->m_iUpdateStatus = 2;
				}
				else
				{
					pDlg->m_iUpdateStatus = 1;
				}
			}
			break;
		}
	}

EXIT:
	::SendMessage(pDlg->GetSafeHwnd(), WM_UPDATE_FINISH, NULL, NULL);
	return 0;
}

UINT CUpdateDlg::ProcessUpDateThread2(LPVOID pUserData)
{
	if(pUserData == NULL)
	{
		return -1;
	}
	CUpdateDlg* pDlg = (CUpdateDlg*)pUserData;
	while(pDlg->m_fIsRunningUpdata)
	{
		switch(pDlg->m_iUpdateStatus)
		{
		case 0:
			{
				char szRetBuf[256] = {0};
				char szValue[20] = {0};
				int iStatus = 0;
				int iPageIndex = 0;
				int iRetLen = 0;
				if(HVAPI_GetInfo(pDlg->m_HvDevice, "GetControllPannelUpdatingStatus", 
					szRetBuf, 256, &iRetLen) != S_OK)
				{
					Sleep(100);
					continue;
				}
#ifdef OLD_XML_FORMAT
				if(ParseXmlInfoRespValue(FALSE, szRetBuf, "GetControllPannelUpdatingStatus", "Status", szValue) != S_OK)
#else
				if(ParseXmlInfoRespValue(TRUE, szRetBuf, "GetControllPannelUpdatingStatus", "Status", szValue) != S_OK)
#endif
				
				{
					Sleep(100);
					continue;
				}
				iStatus = atoi(szValue);
				memset(szValue, 0, 20);
#ifdef OLD_XML_FORMAT
				if(ParseXmlInfoRespValue(FALSE, szRetBuf, "GetControllPannelUpdatingStatus", "PageIndex", szValue) != S_OK)
#else
				if(ParseXmlInfoRespValue(TRUE, szRetBuf, "GetControllPannelUpdatingStatus", "PageIndex", szValue) != S_OK)
#endif
				
				{
					Sleep(100);
					continue;
				}
				iPageIndex = atoi(szValue);
				char szTemp[50] = {0};
				sprintf(szTemp, "正在烧写Flash，第%d页...", iPageIndex);
				//pDlg->SetDlgItemText(IDC_STATIC_STATUS, szTemp);
				pDlg->SendMessage(WM_SHOWMESSAGE, 0, (LPARAM)szTemp);
				//pDlg->m_Progress.SetPos(iPageIndex);
				pDlg->SendMessage(WM_UPDATE_PROGRESS, (WPARAM)iPageIndex, 0);
				if(iPageIndex == 480 && iStatus == -1)
				{
					//pDlg->SetDlgItemText(IDC_STATIC_STATUS, "当前状态：数据更新完成");
					pDlg->SendMessage(WM_SHOWMESSAGE, 0, (LPARAM)"当前状态：数据更新完成");
					pDlg->m_iUpdateStatus = 1;
				}
			}
			break;
		case 1:
			{
				char szRetBuf[256] = {0};
				char szValue[20] = {0};
				int iCRCValue = -1;
				int iRetLen = 0;

				if(HVAPI_GetInfo(pDlg->m_HvDevice, "GetControllPannelCRCValue", 
					szRetBuf, 256, &iRetLen) != S_OK)
				{
#ifdef OLD_XML_FORMAT
					ParseXmlInfoRespValue(FALSE, szRetBuf, "GetControllPannelCRCValue", "Value", szValue);
#else
					ParseXmlInfoRespValue(TRUE, szRetBuf, "GetControllPannelCRCValue", "Value", szValue);
#endif
					
					iCRCValue = atoi(szValue);
				}
				DWORD dwProcMode = 0;
				CWarnningDlg pDlgWarnning(&dwProcMode, iCRCValue, NULL);
				pDlgWarnning.DoModal();
				if(dwProcMode == 2)
				{
					pDlg->m_iUpdateStatus = 3;
				}
				else
				{
					pDlg->m_iUpdateStatus = 2;
				}
			}
			break;
		case 2:
			goto EXIT1;
			break;
		case 3:
			{
				//pDlg->SetDlgItemText(IDC_STATIC_STATUS, "当前状态：切换控制板进入正常模式");
				pDlg->SendMessage(WM_SHOWMESSAGE, 0, (LPARAM)"当前状态：切换控制板进入正常模式");
				char szRet[256] = {0};
				int iRetLen = 0;
				if(HVAPI_ExecCmd(pDlg->m_HvDevice, "SetControllPannelStatus,WorkMode[1],WorkStatus[0]", szRet, 256, &iRetLen) != S_OK)
				{
					pDlg->MessageBox("切换控制板进入正常模式失败", "ERROR", MB_OK|MB_ICONERROR);
					goto EXIT1;
				}
				char szRetcode[10] = {0};
#ifdef OLD_XML_FORMAT
				if(FAILED(HvParseXmlCmdRespRetcode(szRet, szRetcode)))//HvParseXmlCmdRespRetcode(szRet, "SetControllPannelStatus", "RetCode", szRetcode)
#else
				if(FAILED(HvParseXmlCmdRespRetcode2(szRet, "SetControllPannelStatus", "RetCode", szRetcode)))//
#endif
				{
					pDlg->MessageBox("切换控制板进入正常模式失败", "ERROR", MB_OK|MB_ICONERROR);
					goto EXIT1;
				}
				if(atoi(szRetcode) != 0)
				{
					pDlg->MessageBox("切换控制板进入正常模式失败", "ERROR", MB_OK|MB_ICONERROR);
					goto EXIT1;
				}
				pDlg->MessageBox("升级控制完成", "INFO", MB_OK);
				goto EXIT1;
			}
			break;
		}
	}

EXIT1:
	pDlg->SendMessage(WM_UPDATE_FINISH, NULL, NULL);
	return 0;
}

void CUpdateDlg::OnBnClickedButton9()
{
	//MessageBox("升级功能未完成，暂时无法使用", "INFO", MB_OK);
	//return;
	CString strFilePath;
	GetDlgItemText(IDC_EDIT1, strFilePath);
	if(strFilePath.GetLength() <= 0)
	{
		MessageBox("升级文件路径不能为空", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	m_fIsRunningUpdata = TRUE;
	m_iUpdateStatus = 0;
	m_Progress.SetRange(0, 480);
	m_Progress.SetPos(0);
	m_pProcessUpDateThread = AfxBeginThread(ProcessUpDateThread, this);
}

LRESULT CUpdateDlg::OnShearFinsh(WPARAM wParam, LPARAM lParam)
{
	if(m_rgbUpdateFileBuffer)
	{
		delete[] m_rgbUpdateFileBuffer;
		m_rgbUpdateFileBuffer = NULL;
	}
	::EnableWindow((GetDlgItem(IDC_BUTTON1))->GetSafeHwnd(), TRUE);
	::EnableWindow((GetDlgItem(IDC_BUTTON8))->GetSafeHwnd(), TRUE);
	::EnableWindow((GetDlgItem(IDC_BUTTON9))->GetSafeHwnd(), TRUE);
	::EnableWindow((GetDlgItem(IDC_EDIT1))->GetSafeHwnd(), TRUE);
	return S_OK;
}

BOOL CUpdateDlg::CheckUpdateFile()
{
	CFile File;
	CString strFileName;
	GetDlgItemText(IDC_EDIT1, strFileName);
	if(File.Open(strFileName.GetBuffer(), CFile::modeRead) == FALSE)
	return FALSE;
	ULONGLONG wFileLen = File.GetLength();
	if(wFileLen != UPDATE_FILE_SIZE)
	{
		File.Close();
		return FALSE;
	}
	m_rgbUpdateFileBuffer = new unsigned char[UPDATE_FILE_SIZE];
	if(!m_rgbUpdateFileBuffer)
	{
		File.Close();
		return FALSE;
	}
	File.Read(m_rgbUpdateFileBuffer, UPDATE_FILE_SIZE);
	File.Close();
	int iIndex = 0;
	for(iIndex=0; iIndex<66; iIndex++)
	{
		if(m_rgbUpdateFileBuffer[0x1DFBE + iIndex] != rgszUpDateFileFlag[iIndex])
		return FALSE;
	}
	SetDlgItemText(IDC_STATIC_STATUS, "当前状态:计算升级文件校验值");
	m_iCRCValue = 0;
	m_iCRCValue = CRCCount(m_iCRCValue, m_rgbUpdateFileBuffer, UPDATE_FILE_SIZE);
	return TRUE;
}

void CUpdateDlg::GetControllPannelStatus(int& iWorkMode, int& iWorkStatus)
{
	char szWorkMode[20] = {0};
	char szWorkStatus[20] = {0};
	char szRetBuf[256] = {0};
	int iRetLen = 0;

	if(HVAPI_GetInfo(m_HvDevice, "GetControllPannelStatus", 
		szRetBuf, 256, &iRetLen) != S_OK)
	{
		sprintf(szWorkMode, "未知");
		sprintf(szWorkStatus, "停止");
	}
	else
	{
#ifdef OLD_XML_FORMAT
		if(ParseXmlInfoRespValue(FALSE, szRetBuf, "GetControllPannelStatus", "WorkMode", szWorkMode) != S_OK)
#else
		if(ParseXmlInfoRespValue(TRUE, szRetBuf, "GetControllPannelStatus", "WorkMode", szWorkMode) != S_OK)
#endif
		{
			iWorkMode = -1;
		}
#ifdef OLD_XML_FORMAT
		if(ParseXmlInfoRespValue(FALSE, szRetBuf, "GetControllPannelStatus", "WorkStatus", szWorkStatus) != S_OK)
#else
		if(ParseXmlInfoRespValue(TRUE, szRetBuf, "GetControllPannelStatus", "WorkStatus", szWorkStatus) != S_OK)
#endif
		{
			iWorkStatus = -1;
		}
	}
	iWorkMode = atoi(szWorkMode);
	iWorkStatus = atoi(szWorkStatus);
}

LRESULT CUpdateDlg::OnShowMessage(WPARAM wParam, LPARAM lParam)
{
	if(lParam == NULL)
	{
		return E_FAIL;
	}

	SetDlgItemText(IDC_STATIC_STATUS, (char*)lParam);

	return S_OK;
}

LRESULT CUpdateDlg::OnUpdateProgress(WPARAM wParam, LPARAM lParam)
{
	if(wParam == NULL)
	{
		return E_FAIL;
	}

	m_Progress.SetPos((int)wParam);
	return S_OK;
}
