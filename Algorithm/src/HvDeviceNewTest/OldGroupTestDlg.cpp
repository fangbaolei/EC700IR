#include "stdafx.h"
#include "OldGroupTestDlg.h"
#include "shlwapi.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if(p) {delete[] p; p = NULL;}
#endif

COldTestType::COldTestType(char* szIP, CListCtrl* pListBox, DWORD dwListIndex, CString strSavePath)
{
	m_ListBox = pListBox;
	m_dwIndex = dwListIndex;
	m_ListBox->SetItemText(m_dwIndex, 1, "测试类初始化成功");
	ZeroMemory(m_szIP, sizeof(m_szIP));
	memcpy(m_szIP, szIP, strlen(szIP));
	m_strSavePath = strSavePath;

	if(m_strSavePath != "")
	{
		m_ListBox->SetItemText(m_dwIndex, 1, "正在创建保存文件夹");
		if(!PathFileExists(m_strSavePath.GetBuffer()))
		{
			if(CreateDirectory(m_strSavePath.GetBuffer(), NULL) == TRUE)
			{
				m_strSavePath += "\\";
				m_strSavePath += szIP;
				m_strSavePath += "\\";
				if(!PathFileExists(m_strSavePath.GetBuffer()))
				{
					if(CreateDirectory(m_strSavePath.GetBuffer(), NULL) == TRUE)
					{
						m_fIsCanSave = TRUE;
					}
				}
				else
				{
					m_fIsCanSave = TRUE;
				}
			}
		}
		else
		{
			m_strSavePath += "\\";
			m_strSavePath += szIP;
			m_strSavePath += "\\";
			if(!PathFileExists(m_strSavePath.GetBuffer()))
			{
				if(CreateDirectory(m_strSavePath.GetBuffer(), NULL) == TRUE)
				{
					m_fIsCanSave = TRUE;
				}
			}
			else
			{
				m_fIsCanSave = TRUE;
			}
		}
	}

	m_dwLastStatus = CONN_STATUS_UNKNOWN;
	m_dwReconnectTimes = 0;
	InitializeCriticalSection(&m_csList);
	m_hDevice = NULL;
	m_hProceThread = NULL;
	m_dwStatus = 0;
	m_fIsThreadExit = FALSE;
	m_fIsProceResultThreadExit = FALSE;
	m_hProceResultDataThread = CreateThread(NULL, 0, ProceResultDataThread, this, 0, NULL);
	m_hProceThread = CreateThread(NULL, 0, ProceThread, this, 0, NULL);
}

COldTestType::~COldTestType()
{
	if(m_hProceThread)
	{
		m_fIsThreadExit = TRUE;
		int iWaitTimes = 0;
		int MAX_WAIT_TIME = 8;
		while(WaitForSingleObject(m_hProceThread, 500) == WAIT_TIMEOUT
			&& iWaitTimes < MAX_WAIT_TIME)
		{
			iWaitTimes++;
		}
		if(iWaitTimes >= MAX_WAIT_TIME)
		{
			TerminateThread(m_hProceThread, 0);
		}
		CloseHandle(m_hProceThread);
		m_hProceThread = NULL;
	}

	if(m_hProceThread)
	{
		m_fIsProceResultThreadExit = TRUE;
		int iWaitTimes = 0;
		int MAX_WAIT_TIME = 8;
		while(WaitForSingleObject(m_hProceResultDataThread, 500) == WAIT_TIMEOUT
			&& iWaitTimes < MAX_WAIT_TIME)
		{
			iWaitTimes++;
		}
		if(iWaitTimes >= MAX_WAIT_TIME)
		{
			TerminateThread(m_hProceResultDataThread, 0);
		}
		CloseHandle(m_hProceResultDataThread);
		m_hProceResultDataThread = NULL;
	}

	if(m_hDevice != NULL)
	{
		HVAPI_Close(m_hDevice);
		m_hDevice = NULL;
	}

	while(!m_ResultList.IsEmpty())
	{
		GROUP_RESULT_DATA cTmpInfo = m_ResultList.RemoveHead();
		if (cTmpInfo.pData)
		{
			delete [] cTmpInfo.pData;
			cTmpInfo.pData = NULL;
		}

		if(cTmpInfo.pszPlateInfo)
		{
			delete[] cTmpInfo.pszPlateInfo;
			cTmpInfo.pszPlateInfo = NULL;
		}
	}
}

DWORD WINAPI COldTestType::ProceResultDataThread(LPVOID pParam)
{
	if(pParam == NULL)
	{
		return -1;
	}

	COldTestType* pTestType = (COldTestType*)pParam;
	while(!pTestType->m_fIsProceResultThreadExit)
	{
		if(pTestType->m_ResultList.IsEmpty())
		{
			Sleep(500);
			continue;
		}	

		GROUP_RESULT_DATA TmpResultData;
		EnterCriticalSection(&pTestType->m_csList);
		TmpResultData = pTestType->m_ResultList.RemoveHead();
		LeaveCriticalSection(&pTestType->m_csList);

		if(pTestType->m_fIsCanSave && pTestType->m_strSavePath != "")
		{
			if(TmpResultData.dwDataType == RECORD_TYPE_NORMAL ||
				TmpResultData.dwDataType == RECORD_TYPE_HISTORY)
			{
				if(TmpResultData.dwInfoLen <= 0 || TmpResultData.pszPlateInfo == NULL)
				{
					goto done;
				}

				DWORD dwBufLen = 50;
				char* pszPlate = new char[dwBufLen];
				memset(pszPlate, 0, dwBufLen);
				if(HVAPIUTILS_GetRecordInfoFromAppenedString(TmpResultData.pszPlateInfo, 
					"PlateName", pszPlate, 50) != S_OK)
				{
					SAFE_DELETE(pszPlate);
					goto done;
				}

				DWORD dwCarID, dwTimeLow, dwTimeHigh;
				char* pszValue = new char[dwBufLen];
				memset(pszValue, 0, dwBufLen);
				if(HVAPIUTILS_GetRecordInfoFromAppenedString(TmpResultData.pszPlateInfo, "CarID", pszValue, dwBufLen) != S_OK)
				{
					SAFE_DELETE(pszPlate);
					SAFE_DELETE(pszValue);
					goto done;
				}
				dwCarID = atoi(pszValue);

				memset(pszValue, 0, dwBufLen);
				if(HVAPIUTILS_GetRecordInfoFromAppenedString(TmpResultData.pszPlateInfo, "TimeHigh", pszValue, dwBufLen) != S_OK)
				{
					SAFE_DELETE(pszPlate);
					SAFE_DELETE(pszValue);
					goto done;
				}
				dwTimeHigh = atoi(pszValue);

				memset(pszValue, 0, dwBufLen);
				if(HVAPIUTILS_GetRecordInfoFromAppenedString(TmpResultData.pszPlateInfo, "TimeLow", pszValue, dwBufLen) != S_OK)
				{
					SAFE_DELETE(pszPlate);
					SAFE_DELETE(pszValue);
					goto done;
				}
				dwTimeLow = atoi(pszValue);

				SAFE_DELETE(pszValue);
				DWORD64 dw64TimeMS = ((DWORD64)(dwTimeHigh)<<32) | dwTimeLow;

				CString strSaveFileName;
				CTime cTime(dw64TimeMS/1000);
				strSaveFileName.Format("%s%s_%d.xml", pTestType->m_strSavePath,
					cTime.Format("%Y%m%d%H%M%S"), dwCarID);
				FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
				if(fp)
				{
					fwrite(TmpResultData.pszPlateInfo, strlen(TmpResultData.pszPlateInfo), 1, fp);
					fclose(fp);
				}

				strSaveFileName.Format("%sPlateInfo.txt", pTestType->m_strSavePath.GetBuffer());
				fp = fopen(strSaveFileName.GetBuffer(), "a+");
				if(fp)
				{
					fwrite(pszPlate, strlen(pszPlate), 1, fp);
					fwrite("\n", 1, 1, fp);
					fclose(fp);
				}

				SAFE_DELETE(pszPlate);
				RECORD_IMAGE_GROUP cRecordImageGroup;
				if(TmpResultData.pData == NULL)
				{
					goto done;
				}
				if(HVAPIUTILS_GetRecordImageAll(TmpResultData.pData, TmpResultData.dwDataLen, &cRecordImageGroup) != S_OK)
				{
					goto done;
				}
				
				if(cRecordImageGroup.pbImgDataBestSnapShot)
				{
					strSaveFileName.Format("%s%s_%d_%d.jpeg", pTestType->m_strSavePath,
						cTime.Format("%Y%m%d%H%M%S"), dwCarID, 1);
					FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
					if(fp)
					{
						fwrite(cRecordImageGroup.pbImgDataBestSnapShot, cRecordImageGroup.dwImgDataBestSnapShotLen, 1, fp);
						fclose(fp);
					}
				}

				if(cRecordImageGroup.pbImgDataLastSnapShot)
				{
					strSaveFileName.Format("%s%s_%d_%d.jpeg", pTestType->m_strSavePath,
						cTime.Format("%Y%m%d%H%M%S"), dwCarID, 2);
					FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
					if(fp)
					{
						fwrite(cRecordImageGroup.pbImgDataLastSnapShot, cRecordImageGroup.dwImgDataLastSnapShotLen, 1, fp);
						fclose(fp);
					}
				}

				if(cRecordImageGroup.pbImgDataBeginCapture)
				{
					strSaveFileName.Format("%s%s_%d_%d.jpeg", pTestType->m_strSavePath,
						cTime.Format("%Y%m%d%H%M%S"), dwCarID, 3);
					FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
					if(fp)
					{
						fwrite(cRecordImageGroup.pbImgDataBeginCapture, cRecordImageGroup.dwImgDataBeginCaptureLen, 1, fp);
						fclose(fp);
					}
				}

				if(cRecordImageGroup.pbImgDataBestCapture)
				{
					strSaveFileName.Format("%s%s_%d_%d.jpeg", pTestType->m_strSavePath,
						cTime.Format("%Y%m%d%H%M%S"), dwCarID, 4);
					FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
					if(fp)
					{
						fwrite(cRecordImageGroup.pbImgDataBestCapture, cRecordImageGroup.dwImgDataBestCaptureLen, 1, fp);
						fclose(fp);
					}
				}

				if(cRecordImageGroup.pbImgDataLastCapture)
				{
					strSaveFileName.Format("%s%s_%d_%d.jpeg", pTestType->m_strSavePath,
						cTime.Format("%Y%m%d%H%M%S"), dwCarID, 5);
					FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
					if(fp)
					{
						fwrite(cRecordImageGroup.pbImgDataLastCapture, cRecordImageGroup.dwImgDataLastCaptureLen, 1, fp);
						fclose(fp);
					}
				}

				if(cRecordImageGroup.pbImgDataSmaller)
				{
					strSaveFileName.Format("%s%s_%d.bmp", pTestType->m_strSavePath,
						cTime.Format("%Y%m%d%H%M%S"), dwCarID);
					INT iDestBufLen = 1024 << 9;
					PBYTE DestData = new BYTE[iDestBufLen];
					if(DestData != NULL)
					{
						if(HVAPIUTILS_SmallImageToBitmap(cRecordImageGroup.pbImgDataSmaller,
							cRecordImageGroup.cImgInfoSmaller.dwWidth, cRecordImageGroup.cImgInfoSmaller.dwHeight, 
							DestData, &iDestBufLen) == S_OK)
						{
							FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
							if(fp && DestData)
							{
								fwrite(DestData, iDestBufLen, 1, fp);
								fclose(fp);
							}
						}
						if(DestData)
						{
							delete[] DestData;
							DestData = NULL;
						}
					}
				}

				if(cRecordImageGroup.pbImgDataBinary)
				{
					CString strSaveFileName;
					CTime cTime(dw64TimeMS/1000);
					strSaveFileName.Format("%s%s_%d.bin", pTestType->m_strSavePath,
						cTime.Format("%Y%m%d%H%M%S"), dwCarID);
					FILE* fp = fopen(strSaveFileName.GetBuffer(), "wb");
					if(fp)
					{
						fwrite(cRecordImageGroup.pbImgDataBinary, cRecordImageGroup.dwImgDataBinaryLen, 1, fp);
						fclose(fp);
					}
				}
			}
		}

done:
		if(TmpResultData.pData)
		{
			delete[] TmpResultData.pData;
			TmpResultData.pData = NULL;
		}

		if(TmpResultData.pszPlateInfo)
		{
			delete[] TmpResultData.pszPlateInfo;
			TmpResultData.pszPlateInfo = NULL;
		}
	}

	return 0;
}

DWORD WINAPI COldTestType::ProceThread(LPVOID pParam)
{
	if(pParam == NULL)
	{
		return -1;
	}
	COldTestType* pTestType = (COldTestType*)pParam;
	while(!pTestType->m_fIsThreadExit)
	{
		switch(pTestType->m_dwStatus)
		{
		case 0:
			pTestType->m_ListBox->SetItemText(pTestType->m_dwIndex, 1, "正在连接识别器...");
			pTestType->m_hDevice = HVAPI_Open(pTestType->m_szIP, NULL);
			if(pTestType->m_hDevice != NULL)
			{
				pTestType->m_dwStatus = 1;
			}
			else
			{
				pTestType->m_ListBox->SetItemText(pTestType->m_dwIndex, 1, "连接识别器失败...");
			}
			break;
		case 1:
			pTestType->m_dwStatus = 5;
			break;
		case 5:
			pTestType->m_ListBox->SetItemText(pTestType->m_dwIndex, 1, "正在设置结果回调...");
			if(HVAPI_SetCallBack(pTestType->m_hDevice, pTestType->OnRecord, 
				pTestType, STREAM_TYPE_RECORD, NULL) == S_OK)
			{
				pTestType->m_dwStatus = 6;
			}
			else
			{
				pTestType->m_ListBox->SetItemText(pTestType->m_dwIndex, 1, "设置结果回调失败...");
			}
			break;
		case 6:
			{
				DWORD dwStatus;
				CString strInfo = "";

				HVAPI_GetConnStatus(pTestType->m_hDevice, STREAM_TYPE_RECORD, &dwStatus);
				switch(dwStatus)
				{
				case CONN_STATUS_UNKNOWN:
					strInfo += "连接状态:未知,";
					break;
				case CONN_STATUS_NORMAL:
					strInfo += "连接状态:正常,";
					break;
				case CONN_STATUS_DISCONN:
					strInfo += "连接状态:停止,";
					break;
				case CONN_STATUS_RECONN:
					strInfo += "连接状态:重连,";
					break;
				case CONN_STATUS_RECVDONE:
					strInfo += "连接状态:结束,";
					break;
				}
				if(pTestType->m_dwLastStatus == CONN_STATUS_RECONN
					&& dwStatus == CONN_STATUS_NORMAL)
				{
					pTestType->m_dwReconnectTimes++;
				}
				pTestType->m_dwLastStatus = dwStatus;

				CString strTemp;
				strTemp.Format("重连次数:%d", pTestType->m_dwReconnectTimes);
				strInfo += strTemp;
				pTestType->m_ListBox->SetItemText(pTestType->m_dwIndex, 1, strInfo.GetBuffer());
				Sleep(2000);
			}
			break;
		}
	}
	return 0;
}

INT COldTestType::OnRecord(PVOID pUserData, PBYTE pbResultPacket, DWORD dwPacketLen, DWORD dwRecordType, LPCSTR szResultInfo)
{
	if(pUserData == NULL)
	{
		return -1;
	}
	COldTestType* pTestType = (COldTestType*)pUserData;
	if(pTestType->m_ResultList.IsFull())
	{
		return -1;
	}
	GROUP_RESULT_DATA NewResultData;
	NewResultData.dwDataType = dwRecordType;
	NewResultData.dwDataLen = dwPacketLen;
	NewResultData.dwInfoLen = (DWORD)strlen(szResultInfo);
	NewResultData.pData = new BYTE[NewResultData.dwDataLen];
	NewResultData.pszPlateInfo = new char[NewResultData.dwInfoLen];
	if(NewResultData.pData == NULL || NewResultData.pszPlateInfo == NULL)
	{
		SAFE_DELETE(NewResultData.pData);
		SAFE_DELETE(NewResultData.pszPlateInfo);
		return -1;
	}
	memcpy(NewResultData.pData, pbResultPacket, NewResultData.dwDataLen);
	memcpy(NewResultData.pszPlateInfo, szResultInfo, NewResultData.dwInfoLen);
	EnterCriticalSection(&pTestType->m_csList);
	pTestType->m_ResultList.AddTail(NewResultData);
	LeaveCriticalSection(&pTestType->m_csList);
	return 0;
}

IMPLEMENT_DYNAMIC(COldGroupTestDlg, CDialog)

COldGroupTestDlg::COldGroupTestDlg(CWnd* pParent /* = NULL */)
: CDialog(COldGroupTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	if(pParent)
		m_pParentDlg = pParent;
	for(INT iIndex = 0; iIndex < 20; iIndex++)
		m_TestType[iIndex] = NULL;
}

COldGroupTestDlg::~COldGroupTestDlg()
{

}

void COldGroupTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPControll);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_LIST2, m_ListControll);
}

BEGIN_MESSAGE_MAP(COldGroupTestDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
END_MESSAGE_MAP()

BOOL COldGroupTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	m_IPControll.SetAddress(0, 0, 0, 0);
	m_ListBox.ResetContent();
	m_Messge.Create(this);
	m_Messge.AddTool(GetDlgItem(IDC_BUTTON2), "TEST");
	m_Messge.SetDelayTime(0);
	m_Messge.SetTipBkColor(RGB(255, 255, 255));
	m_Messge.SetTipTextColor(RGB(255, 0, 0));
	m_Messge.Activate(TRUE);

	m_ListControll.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListControll.InsertColumn(0, "设备IP", LVCFMT_LEFT, 100);
	m_ListControll.InsertColumn(1, "当前状态", LVCFMT_LEFT, 160);
	
	SetWindowText("旧接口峰值测试");

	return TRUE;
}

void COldGroupTestDlg::OnCancel()
{
	WORD wIndex;
	for(wIndex=0; wIndex<20; wIndex++)
	{
		if(m_TestType[wIndex] != NULL)
		{
			delete m_TestType[wIndex];
			m_TestType[wIndex] = NULL;
		}
	}

	CDialog::OnCancel();
}

void COldGroupTestDlg::OnBnClickedButton1()
{
	DWORD dwIP = 0;
	m_IPControll.GetAddress(dwIP);
	if(dwIP == 0 || dwIP == 0xFFFFFFFF)
	{
		MessageBox("非法IP", "ERROR", MB_OK|MB_ICONERROR);
		return;
	}
	int iTotle = m_ListBox.GetCount();
	if(iTotle > 20)
	{
		MessageBox("测试工具最多只能支持同时连接20台设备");
		return;
	}
	for(int iIndex = 0; iIndex < iTotle; iIndex++)
	{
		if(m_ListBox.GetItemData(iIndex) == dwIP)
		{
			char szMsg[100] = {0};
			char szIP[20] = {0};
			m_ListBox.GetText(iIndex, szIP);
			sprintf(szMsg, "已添加%s的设备到列表，无需重复添加", szIP);
			MessageBox(szMsg);
			return;
		}
	}

	char szIP[20] = {0};
	sprintf(szIP, "%d.%d.%d.%d", (dwIP>>24)&0xFF,
		(dwIP>>16)&0xFF, (dwIP>>8)&0xFF, (dwIP)&0xFF);
	m_ListBox.AddString(szIP);
	m_ListBox.SetItemData(iTotle, dwIP);
}

void COldGroupTestDlg::OnBnClickedButton2()
{
	BROWSEINFOA bi;
	ZeroMemory(&bi, sizeof(BROWSEINFOA));
	bi.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpszTitle = "保存路径";
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	TCHAR szFolder[MAX_PATH<<1];
	szFolder[0] = _T('\0');
	if(pidl)
	{
		if(SHGetPathFromIDList(pidl, szFolder))
		{
			m_SaveFilePath = szFolder;
		}
	}
}

BOOL COldGroupTestDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_MOUSEMOVE 
		&& pMsg->hwnd == GetDlgItem(IDC_BUTTON2)->GetSafeHwnd())
	{
		char szNewMessge[MAX_PATH] = {0};
		sprintf(szNewMessge, "当前保存路径\"%s\"", m_SaveFilePath);
		m_Messge.AddTool(GetDlgItem(IDC_BUTTON2), szNewMessge);
		m_Messge.RelayEvent(pMsg);
	}
	if(pMsg->message == WM_RBUTTONDBLCLK
		&& pMsg->hwnd == GetDlgItem(IDC_LIST1)->GetSafeHwnd())
	{
		DWORD dwIndex = m_ListBox.GetCurSel();
		m_ListBox.DeleteString(dwIndex);
	}

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		 return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void COldGroupTestDlg::OnBnClickedButton3()
{
	DWORD dwTotle = m_ListBox.GetCount();
	if(dwTotle <= 0 || dwTotle > 20)
	{
		MessageBox("设备列表为NULL");
		return;
	}
	if(m_SaveFilePath == "")
	{
		if(MessageBox("文件保存路径未设置，接收到的结果将不保存，是否确定启动测试？", "WARMMING", MB_YESNO) == IDNO)
		{
			return;
		}
	}
	GetDlgItem(IDC_IPADDRESS1)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIST1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);
	m_ListControll.DeleteAllItems();
	for(DWORD dwIndex=0; dwIndex<dwTotle; dwIndex++)
	{
		if(m_TestType[dwIndex] != NULL)
		{
			delete m_TestType[dwIndex];
			m_TestType[dwIndex] = NULL;
		}
		char szIP[20] = {0};
		m_ListBox.GetText(dwIndex, szIP);
		m_ListControll.InsertItem(dwIndex, "", 0);
		m_ListControll.SetItemText(dwIndex, 0, szIP);
		m_TestType[dwIndex] = new COldTestType(szIP, &m_ListControll, dwIndex, m_SaveFilePath);
	}
}

void COldGroupTestDlg::OnBnClickedButton4()
{
	WORD wIndex;
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	for(wIndex=0; wIndex<20; wIndex++)
	{
		if(m_TestType[wIndex] != NULL)
		{
			delete m_TestType[wIndex];
			m_TestType[wIndex] = NULL;
		}
	}
	GetDlgItem(IDC_IPADDRESS1)->EnableWindow(TRUE);
	GetDlgItem(IDC_LIST1)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
	m_ListControll.DeleteAllItems();
	GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);
}

void COldGroupTestDlg::OnBnClickedButton8()
{
	int iTotleSel = m_ListBox.GetCount();
	int iIndex = m_ListBox.GetCurSel();
	if(iIndex >= 0 && iIndex < iTotleSel)
	{
		m_ListBox.DeleteString(iIndex);
	}
}

void COldGroupTestDlg::OnOK()
{
    // 防止按回车退出
}

