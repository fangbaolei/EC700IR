#pragma once


// CVideoMaxShowDlg 对话框

class CVideoMaxShowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVideoMaxShowDlg)

public:
	CVideoMaxShowDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CVideoMaxShowDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_MAXSHOWVIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
