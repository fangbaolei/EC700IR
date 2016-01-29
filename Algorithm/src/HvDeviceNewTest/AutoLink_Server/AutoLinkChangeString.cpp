// AutoLinkChangeString.cpp : implementation file
//

#include "stdafx.h"
#include "AutoLink_demo.h"
#include "AutoLinkChangeString.h"
#include ".\autolinkchangestring.h"


// AutoLinkChangeString dialog

IMPLEMENT_DYNAMIC(AutoLinkChangeString, CDialog)
AutoLinkChangeString::AutoLinkChangeString(CWnd* pParent /*=NULL*/)
	: CDialog(AutoLinkChangeString::IDD, pParent)
{
}

AutoLinkChangeString::~AutoLinkChangeString()
{
}

void AutoLinkChangeString::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MATCH1, m_ExampleEdit);
	DDX_Control(pDX, IDC_EDIT_MATCH2, m_InfoEdit);
	DDX_Control(pDX, IDC_EDIT_MATCH3, m_ChangeEdit);
	DDX_Control(pDX, IDC_COMBO_MATCHTIME, m_TimeDropList);
	DDX_Control(pDX, IDC_COMBO_MATCHPLATE, m_PlateDropList);
	DDX_Control(pDX, IDC_COMBO_MATCHDEV, m_DevInfoDropList);
	DDX_Control(pDX, IDC_COMBO_MATCHOTHER, m_OtherDropList);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_TypeDropList);
}

BOOL AutoLinkChangeString::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bFirstUseDropLise = true; // 是否初次使用下拉列表框
	
	InitChangeDlg();

	return TRUE;
}

BEGIN_MESSAGE_MAP(AutoLinkChangeString, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_MATCHOK, OnBnClickedButtonMatchok)
	ON_BN_CLICKED(IDC_BUTTON_MATCHCANCEL, OnBnClickedButtonMatchcancel)
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_COMBO_MATCHTIME, OnCbnSelchangeComboMatchtime)
	ON_CBN_SELCHANGE(IDC_COMBO_MATCHPLATE, OnCbnSelchangeComboMatchplate)
	ON_CBN_SELCHANGE(IDC_COMBO_MATCHDEV, OnCbnSelchangeComboMatchdev)
	ON_CBN_SELCHANGE(IDC_COMBO_MATCHOTHER, OnCbnSelchangeComboMatchother)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelchangeComboType)
END_MESSAGE_MAP()

// AutoLinkChangeString message handlers

void AutoLinkChangeString::InitChangeDlg()
{
	ShowText(m_chInitText);

	m_TimeDropList.ResetContent();  // 清空所有内容
	m_TimeDropList.SetCurSel(1);  // 设置当前选项
	m_TimeDropList.AddString("");
	m_TimeDropList.AddString("年");
	m_TimeDropList.AddString("月");
	m_TimeDropList.AddString("日");
	m_TimeDropList.AddString("时");
	m_TimeDropList.AddString("分");
	m_TimeDropList.AddString("秒");
	m_TimeDropList.AddString("毫秒");

	m_PlateDropList.ResetContent();  // 清空所有内容
	m_PlateDropList.SetCurSel(1);  // 设置当前选项
	m_PlateDropList.AddString("");
	m_PlateDropList.AddString("车牌号");
	m_PlateDropList.AddString("车牌颜色");
	m_PlateDropList.AddString("违法信息");
	m_PlateDropList.AddString("车辆类型");
	m_PlateDropList.AddString("车速");
	m_PlateDropList.AddString("限速");
	m_PlateDropList.AddString("图片字母ID");
	m_PlateDropList.AddString("图片ID");
	m_PlateDropList.AddString("图片数量");
	m_PlateDropList.AddString("车道号");

	m_DevInfoDropList.ResetContent();  // 清空所有内容
	m_DevInfoDropList.SetCurSel(1);  // 设置当前选项
	m_DevInfoDropList.AddString("");
	m_DevInfoDropList.AddString("设备IP");
	m_DevInfoDropList.AddString("设备编号");

	m_OtherDropList.ResetContent();  // 清空所有内容
	m_OtherDropList.SetCurSel(1);  // 设置当前选项
	m_OtherDropList.AddString("");
	m_OtherDropList.AddString("路口名称");
	m_OtherDropList.AddString("路口方向");

	m_TypeDropList.ResetContent();  // 清空所有内容
	m_TypeDropList.SetCurSel(1);  // 设置当前选项
	m_TypeDropList.AddString("");
	m_TypeDropList.AddString("jpg");
	m_TypeDropList.AddString("bmp");
	m_TypeDropList.AddString("txt");
	m_TypeDropList.AddString("inf");
	m_TypeDropList.AddString("bin");
}

// 对外接口::使用前设置基本参数
void AutoLinkChangeString::SetInitText(char* chText)
{
	sprintf(m_chInitText, "%s", chText);
}

// 对外接口::使用后返回结果
void AutoLinkChangeString::GetReturnText(char* chText)
{
	sprintf(chText, "%s", m_strReturnText);
}


void AutoLinkChangeString::OnBnClickedButtonMatchok()
{
	m_ChangeEdit.GetWindowText(m_strReturnText);
	OnOK();
}

void AutoLinkChangeString::OnBnClickedButtonMatchcancel()
{
	sprintf(m_strReturnText.GetBuffer(1024), "%s", m_chInitText);
	m_strReturnText.ReleaseBuffer();
	OnCancel();
}

void AutoLinkChangeString::OnClose()
{
	sprintf(m_strReturnText.GetBuffer(1024), "%s", m_chInitText);
	m_strReturnText.ReleaseBuffer();

	CDialog::OnClose();
}

// 屏蔽回车和ESC
BOOL AutoLinkChangeString::PreTranslateMessage(MSG* pMsg)
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

void AutoLinkChangeString::ShowText(CString strText)
{
	CString strTemp;
	m_ChangeEdit.GetWindowText(strTemp);

	strTemp = strTemp + strText;
	m_ChangeEdit.SetWindowText(strTemp);

	CString strExampleText = m_Common.GetStringExample(strTemp); // 示例
	CString strInfoText = m_Common.GetStringInfo(strTemp); // 说明

	m_ExampleEdit.SetWindowText(strExampleText);
	m_InfoEdit.SetWindowText(strInfoText);
}

void AutoLinkChangeString::OnCbnSelchangeComboMatchtime()
{
	CString strListTemp;
	int iIndex = m_TimeDropList.GetCurSel();
	m_TimeDropList.GetLBText(iIndex, strListTemp);

	// 第一次打开
	if (m_bFirstUseDropLise && !strListTemp.IsEmpty())
	{
		m_ChangeEdit.SetWindowText("");
		m_bFirstUseDropLise = false;
	}

	if ("年" == strListTemp)
	{
		ShowText("$(Year)");
	}
	else if ("月" == strListTemp)
	{
		ShowText("$(Month)");
	}
	else if ("日" == strListTemp)
	{
		ShowText("$(Day)");
	}
	else if ("时" == strListTemp)
	{
		ShowText("$(Hour)");
	}
	else if ("分" == strListTemp)
	{
		ShowText("$(Minute)");
	}
	else if ("秒" == strListTemp)
	{
		ShowText("$(Second)");
	}
	else if ("毫秒" == strListTemp)
	{
		ShowText("$(MilliSecond)");
	}
	else
	{
		ShowText("");
	}
}

void AutoLinkChangeString::OnCbnSelchangeComboMatchplate()
{
	CString strListTemp;
	int iIndex = m_PlateDropList.GetCurSel();
	m_PlateDropList.GetLBText(iIndex, strListTemp);
	
	// 第一次打开
	if (m_bFirstUseDropLise && !strListTemp.IsEmpty())
	{
		m_ChangeEdit.SetWindowText("");
		m_bFirstUseDropLise = false;
	}

	if ("车牌号" == strListTemp)
	{
		ShowText("$(PlateNo)");
	}
	else if ("车牌颜色" == strListTemp)
	{
		ShowText("$(PlateColor)");
	}
	else if ("违法信息" == strListTemp)
	{
		ShowText("$(IllegalInfo)");
	}
	else if ("车辆类型" == strListTemp)
	{
		ShowText("$(CarType)");
	}
	else if ("车速" == strListTemp)
	{
		ShowText("$(Speed)");
	}
	else if ("限速" == strListTemp)
	{
		ShowText("$(LimitSpeed)");
	}
	else if ("图片字母ID" == strListTemp)
	{
		ShowText("$(BigImageID_Letter)");
	}
	else if ("图片ID" == strListTemp)
	{
		ShowText("$(BigImageID)");
	}
	else if ("图片数量" == strListTemp)
	{
		ShowText("$(BigImageCount)");
	}
	else if ("车道号" == strListTemp)
	{
		ShowText("$(RoadNum)");
	}
	else
	{
		ShowText("");
	}
}

void AutoLinkChangeString::OnCbnSelchangeComboMatchdev()
{
	CString strListTemp;
	int iIndex = m_DevInfoDropList.GetCurSel();
	m_DevInfoDropList.GetLBText(iIndex, strListTemp);
	
	// 第一次打开
	if (m_bFirstUseDropLise && !strListTemp.IsEmpty())
	{
		m_ChangeEdit.SetWindowText("");
		m_bFirstUseDropLise = false;
	}

	if ("设备IP" == strListTemp)
	{
		ShowText("$(DeviceIP)");
	}
	else if ("设备编号" == strListTemp)
	{
		ShowText("$(DeviceNo)");
	}
	else
	{
		ShowText("");
	}
}

void AutoLinkChangeString::OnCbnSelchangeComboMatchother()
{
	CString strListTemp;
	int iIndex = m_OtherDropList.GetCurSel();
	m_OtherDropList.GetLBText(iIndex, strListTemp);

	// 第一次打开
	if (m_bFirstUseDropLise && !strListTemp.IsEmpty())
	{
		m_ChangeEdit.SetWindowText("");
		m_bFirstUseDropLise = false;
	}

	if ("路口名称" == strListTemp)
	{
		ShowText("$(StreetName)");
	}
	else if ("路口方向" == strListTemp)
	{
		ShowText("$(StreetDirection)");
	}
	else
	{
		ShowText("");
	}
}

void AutoLinkChangeString::OnCbnSelchangeComboType()
{
	// TODO: Add your control notification handler code here
	CString strListTemp;
	int iIndex = m_TypeDropList.GetCurSel();
	m_TypeDropList.GetLBText(iIndex, strListTemp);

	// 第一次打开
	if (m_bFirstUseDropLise && !strListTemp.IsEmpty())
	{
		m_ChangeEdit.SetWindowText("");
		m_bFirstUseDropLise = false;
	}

	if ("jpg" == strListTemp)
	{
		ShowText(".jpg");
	}
	else if ("bmp" == strListTemp)
	{
		ShowText(".bmp");
	}
	else if ("bin" == strListTemp)
	{
		ShowText(".bin");
	}
	else if ("inf" == strListTemp)
	{
		ShowText(".inf");
	}
	else if ("txt" == strListTemp)
	{
		ShowText(".txt");
	}
	else
	{
		ShowText("");
	}
}
