// HvOcxTestDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "atltime.h"
#include "hvdeviceaxctrl1.h"

// CHvOcxTestDlg 对话框
class CHvOcxTestDlg : public CDialog
{
// 构造
public:
	CHvOcxTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HVOCXTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedPatch();
	DECLARE_EVENTSINK_MAP()
	void OnReceivePlateHvdeviceaxctrl1();
	void OnReceiveJpegVideoHvdeviceaxctrl1();
	void OnReceiveH264VideoHvdeviceaxctrl1();
public:
	int m_Jpegnum;
	CString m_strFullName;
	bool m_bExit;
	void WrightLogEx(LPCSTR lpszLogInfo);
	bool m_record;
	CIPAddressCtrl m_IP;//ip	
private:
	CButton m_GetBigImage;
	CButton m_GetSmallImage;
	CButton m_GetBinImage;
	CButton m_Getjpeg;
	CButton m_Geth264;
	CButton m_isShowVedio;
	BOOL m_SaveBigImage; //大图
	BOOL m_SaveSmallImge;//小图
	BOOL m_SaveBin; //二位图
	BOOL m_SaveBinBMP; //二位图转bmp
	BOOL m_Savejpeg;//jpeg流
	BOOL m_Saveh264;//264流
	BOOL m_isSaveIllegalVideo;
	bool m_fisconnect;//是否连接
	DWORD32 m_jpegnum;
	int m_connecttime;
	bool m_SetSaveTime;//设置时间
	CString m_Patch;
	int m_SaveTime;
	int m_H264FrameRateNum;
	int m_GetH264FramRateNum;
	int m_DevStatue;
	HANDLE m_hThread;
public:
	afx_msg void OnBnClickedConnect();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedCheck4();
	afx_msg void OnBnClickedCheck5();
	afx_msg void OnBnClickedCheck6();
	afx_msg void OnBnClickedCheck7();
	afx_msg void OnBnClickedCheck8();
	afx_msg void OnBnClickedCheck9();
	afx_msg void OnBnClickedCheck10();
	afx_msg void OnBnClickedCheck11();
	afx_msg void OnBnClickedSetTime();
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedSetTime2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedCheckShowVedio();
	afx_msg void OnBnClickedButtonInputlist();
	afx_msg void OnBnClickedButtonGetlist();
	CHvdeviceaxctrl1 m_HAC;
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedCkillegalvideo();
	CButton m_RecvIllegalVideo;
	afx_msg void OnBnClickedCksaveillegalvideo();
	CButton m_ButtonSaveIllegalVideo;
	afx_msg void OnBnClickedCkillegalvideo2();
	CButton m_ckSaveFileMode;
	afx_msg void OnBnClickedButtonGetimage();
	afx_msg void OnBnClickedCheckIconhide();
	CButton m_cbHideIcon;
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButtonGetinfo();
	afx_msg void OnBnClickedButtonSetinfo();
	afx_msg void OnBnClickedButtonStart();
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
};
