// FrontPannelTestDlg.h : 头文件
//

#pragma once

#include "HvDevice.h"
#include "ddraw.h"

// CFrontPannelTestDlg 对话框
class CFrontPannelTestDlg : public CDialog
{
// 构造
public:
	CFrontPannelTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FRONTPANNELTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//HRESULT ParseXmlInfoRespValue(char* szXmlBuf, char* szInfoName, char* szInfoValueName, char* szInfoValueText);
	DECLARE_MESSAGE_MAP()
public:
	CIPAddressCtrl	m_DeviceIP;
	CProgressCtrl	m_ProgressPulse1;
	CProgressCtrl	m_ProgressPulse2;
	CProgressCtrl	m_ProgressPulse3;
	CProgressCtrl	m_ProgressPulse4;
	CComboBox		m_ComBoxWorkMode;
	CComboBox		m_ComBoxWorkStatus;
	CComboBox		m_ComBoxPLMode;
	CSliderCtrl		m_SliderMin;
	CSliderCtrl		m_SliderMax;
	CComboBox		m_ComBoxFlash1Polarity;
	CComboBox		m_ComBoxFlash1Coupling;
	CComboBox		m_ComBoxFlash1ResistorMode;
	CComboBox		m_ComBoxFlash2Polarity;
	CComboBox		m_ComBoxFlash2Coupling;
	CComboBox		m_ComBoxFlash2ResistorMode;
	CComboBox		m_ComBoxFlash3Polarity;
	CComboBox		m_ComBoxFlash3Coupling;
	CComboBox		m_ComBoxFlash3ResistorMode;
	afx_msg void OnBnClickedButton1();
	void ShowPannelVersion(void);
	void ChangePulseWidth(void);
	BOOL GetControllPannelAutoRunMode(void);

protected:
	HVAPI_HANDLE m_hDevice;
	BOOL m_fIsConnected;
	BOOL m_fIsShowVideo;
	BOOL m_fIsUpDataConfigInfo;
	CWinThread* m_pUpDateParamThread;
	int	 m_iAutoRunMode;
	int	 m_iUpdateDelay;
	int  m_iPannelStyle;
	BOOL m_fIsNewProtuls;

public:
	afx_msg void OnBnClickedCheck1();
	static UINT UpDateParamThread(LPVOID);
	void ShowPannelStatus(void);
	void ShowDeviceStatus(void);
	void ShowPulseInfo(void);
	void ShowFlashInfo(void);
	void ShowPannelStyle(void);
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnSelchangeCombo2();
	afx_msg void OnCbnSelchangeCombo3();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton12();
};
