// VideoMaxShowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HvDeviceAx.h"
#include "VideoMaxShowDlg.h"
#include "afxdialogex.h"
#include "HvDeviceAxCtrl.h"


// CVideoMaxShowDlg 对话框

IMPLEMENT_DYNAMIC(CVideoMaxShowDlg, CDialogEx)

CVideoMaxShowDlg::CVideoMaxShowDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVideoMaxShowDlg::IDD, pParent)
{

}

CVideoMaxShowDlg::~CVideoMaxShowDlg()
{
}

void CVideoMaxShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVideoMaxShowDlg, CDialogEx)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CVideoMaxShowDlg 消息处理程序


void CVideoMaxShowDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//向父窗口发送双击操作
	//::PostMessage(::GetParent(m_hWnd),WM_LBUTTONDBLCLK, 0, 0);
	//::SetForegroundWindow(::GetParent(m_hWnd));
	((CHvDeviceAxCtrl*)GetParent())->ShowVideoNomal(); //OnLButtonDblClk(nFlags, point);
	CDialogEx::OnLButtonDblClk(nFlags, point);
	this->OnOK();
	::PostMessage(::GetParent(m_hWnd),WM_LBUTTONDBLCLK, 0, 0);
}
