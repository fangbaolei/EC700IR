#pragma once
#include "HvCamera.h"
#include "afxwin.h"

// CNewSDKTestDialog dialog

class CNewSDKTestDialog : public CDialog
{
	DECLARE_DYNAMIC(CNewSDKTestDialog)

public:
	CNewSDKTestDialog(HVAPI_HANDLE_EX hHandle, CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewSDKTestDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG14 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL CNewSDKTestDialog::OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	static UINT  MuilThreadFunc(LPVOID lpvoid);
		static UINT  MuliThreadFunc2(LPVOID lpvoid);
			static UINT  MuilThreadFunc3(LPVOID lpvoid);
			 afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
			 afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

private:
	HVAPI_HANDLE_EX m_hHandle;
public:
	CEdit m_Param1Edit;
	CEdit m_Param2Edit;
	CEdit m_Param3Edit;
	CEdit m_Param4Edit;
	CEdit m_Param5Edit;
	CEdit m_Param6Edit;
	CEdit m_Param7Edit;
	CEdit m_Param8Edit;
	CEdit m_Param9Edit;
	CEdit m_Param10Edit;
	afx_msg void OnBnClickedSeth264bitrate();
	afx_msg void OnBnClickedSetjpegcompressrate();
	afx_msg void OnBnClickedSetosd();
	afx_msg void OnBnClickedSetosdenable();
	afx_msg void OnBnClickedSetosdtimeenable();
	afx_msg void OnBnClickedSetosdpos();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedSetosdfont();
	afx_msg void OnBnClickedSetosdtext();
	afx_msg void OnBnClickedSetcvbs();
	afx_msg void OnBnClickedSetmanualshutter();
	afx_msg void OnBnClickedSetmanualgain();
	afx_msg void OnBnClickedSetawbenable();
	afx_msg void OnBnClickedSetawbmode();
	afx_msg void OnBnClickedSetmanualrgb();
	afx_msg void OnBnClickedSetagcenable();
	afx_msg void OnBnClickedSetagclightbaseline();
	afx_msg void OnBnClickedSetagcparam();
	afx_msg void OnBnClickedSetlut();
	afx_msg void OnBnClickedSetbrightness();
	afx_msg void OnBnClickedSetcontrast();
	afx_msg void OnBnClickedSetsharpness();
	afx_msg void OnBnClickedSetsaturation();
	afx_msg void OnBnClickedSetwdrenable();
	afx_msg void OnBnClickedSetwdrlevel();
	afx_msg void OnBnClickedSetblcenable();
	afx_msg void OnBnClickedSetblclevel();
	afx_msg void OnBnClickedSetdremode();
	afx_msg void OnBnClickedSetdrenable();
	afx_msg void OnBnClickedSetdrelevel();
	afx_msg void OnBnClickedSetdenoiseenable();
	afx_msg void OnBnClickedSetdenoisemode();
	afx_msg void OnBnClickedSetdenoiselevel();
	afx_msg void OnBnClickedSetfiltermode();
	afx_msg void OnBnClickedSetdciris();
	afx_msg void OnBnClickedSetipinfo();
	afx_msg void OnBnClickedSetrtspmulticastenable();
	afx_msg void OnBnClickedSettime();
	afx_msg void OnBnClickedSettimezone();
	afx_msg void OnBnClickedSetntpenable();
	afx_msg void OnBnClickedSetntpserverip();
	afx_msg void OnBnClickedResetdevice();
	afx_msg void OnBnClickedRestoredefaultparam();
	afx_msg void OnBnClickedRestorefactoryparam();
	afx_msg void OnBnClickedSetparamex();
	afx_msg void OnBnClickedGetparamex();
	afx_msg void OnBnClickedGetdevbasicinfo();
	afx_msg void OnBnClickedGetrunmode();
	afx_msg void OnBnClickedGetdevstate();
	afx_msg void OnBnClickedGetresetreport();
	afx_msg void OnBnClickedGetresetcount();
	afx_msg void OnBnClickedGetlog();
	afx_msg void OnBnClickedGetcamerastate();
	afx_msg void OnBnClickedGetcamerabasicinfo();
	afx_msg void OnBnClickedGetosd();
	afx_msg void OnBnClickedTriggerimage();
	afx_msg void OnBnClickedGetlut();
	afx_msg void OnBnClickedDologin();
	afx_msg void OnBnClickedAdduser();
	afx_msg void OnBnClickedDeluser();
	afx_msg void OnBnClickedModuser();
	afx_msg void OnBnClickedGetusers();
	afx_msg void OnBnClickedGetvideostate();
	afx_msg void OnBnClickedSetsensewdr();
	afx_msg void OnBnClickedSetocgate();
	afx_msg void OnBnClickedSetocgate1();
	afx_msg void OnBnClickedExeccmdex();
	afx_msg void OnBnClickedButtonGetdevrunmode();
	afx_msg void OnBnClickedButtonSetntpserverupdateinterval();
	afx_msg void OnBnClickedButtonHvapiEnablecomcheck();
	afx_msg void OnBnClickedButtonHvapiSetsharpnessenable();
	afx_msg void OnBnClickedButtonhvapiShrinkdciris();
	afx_msg void OnBnClickedButtonHvapiZoomdciris();
	afx_msg void OnBnClickedButtonhvapiEnablegrayimage();
	afx_msg void OnBnClickedButtonhvapiAutotestcamera();
	afx_msg void OnBnClickedButtonhvapiSetenredlightenable();
	afx_msg void OnBnClickedButtonhvapiSetredlightrect();
	afx_msg void OnBnClickedButtonhvapiSetdebugjpegstatus();


	afx_msg void OnBnClickedButtonhvapiGetenredlightenable();


	afx_msg void OnBnClickedButtonhvapiSetosdplateenable();
	afx_msg void OnBnClickedButtonhvapiGethddcheckreport();

	afx_msg void OnBnClickedButtonhvapiSettracerank();
	afx_msg void OnBnClickedButtonhvapiSetautojpegcompressenable();

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonhvapiGetblackboxmessage();
	afx_msg void OnBnClickedButtonWriteFpga();
	afx_msg void OnBnClickedButtonReadFpga();
 
 
	afx_msg void OnBnClickedButtonCamsettest();
	afx_msg void OnBnClickedButtonhvapiSetacsync();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButtonhvapiSetimageenhancementenable();

	afx_msg void OnBnClickedButtonhvapiSetenredlightthreshold();
	afx_msg void OnBnClickedButtonGetdevbasicinfovenus();
	afx_msg void OnBnClickedButtonhvapiGetcamerabasicinfo();
	afx_msg void OnBnClickedButtonhvapiGetcamerastatevenus();
	afx_msg void OnBnClickedButtonhvapiSetctrlcplenable();
	afx_msg void OnBnClickedButtonGetdevstatevenus();
	afx_msg void OnBnClickedButtonhvapiGettracerank();
	afx_msg void OnBnClickedButtonhvapiGetvideostatevenus();
	afx_msg void OnBnClickedButtongetconnstatusex();

	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButtonsetipbymacaddr();
	afx_msg void OnBnClickedButtonSettimezone();
	afx_msg void OnBnClickedButtonhvapiSetdenoisesnfenable();
	afx_msg void OnBnClickedButtonsetedgeenable();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedSetColor();
	afx_msg void OnBnClickedSet2ddenoise();
	afx_msg void OnBnClickedSet2ddenoisevalue();
	afx_msg void OnBnClickedButtonhvapiSetgammaenable();
	afx_msg void OnBnClickedButtonhvapiSetgammavalue();
	afx_msg void OnBnClickedButtonsetagczone();
	afx_msg void OnBnClickedButtonhvapiGetagczone();
	afx_msg void OnBnClickedButtongetdevtypeex();
	afx_msg void OnBnClickedButtonhvapiSetdenoisetnfenable();
	afx_msg void OnBnClickedButtonSetautojpegcompressparam();
	afx_msg void OnBnClickedButtongethddstatus();
	afx_msg void OnBnClickedButtongetrunstatusstring();
	afx_msg void OnBnClickedButtonHvapiSetledswitch();
	afx_msg void OnBnClickedButtonsearchex();
	afx_msg void OnBnClickedButtonGetrunstatus();
	afx_msg void OnBnClickedButtongetcameraworkstate();
	afx_msg void OnBnClickedButtonsettgio();
	afx_msg void OnBnClickedButtonsetf1io();
	afx_msg void OnBnClickedButtonsetexpio();
	afx_msg void OnBnClickedButtonsetalmio();
	afx_msg void OnBnClickedButtongettgio();
	afx_msg void OnBnClickedButtongetf1io();
	afx_msg void OnBnClickedButtongetexpio();
	afx_msg void OnBnClickedButtongetalmio();
	afx_msg void OnBnClickedButtonsetcolorgradation();
	afx_msg void OnBnClickedButtonsetvediorequestcontrol();
	afx_msg void OnBnClickedButtonhvapiSetaescene();
	afx_msg void OnBnClickedButtonhvapiGetaescene();

	afx_msg void OnBnClickedButtonsetdevname();
	afx_msg void OnBnClickedButtongetdevname();
	afx_msg void OnBnClickedButtonseth264secondbitrate();
	afx_msg void OnBnClickedButtongeth264secondbitrate();
	afx_msg void OnBnClickedButtonsetdenoiseswitch();
	afx_msg void OnBnClickedButtonhvapiSofttriggercapture();
	afx_msg void OnBnClickedSetsharpnessenable();
	afx_msg void OnBnClickedSeth264bitratecontrol();
	afx_msg void OnBnClickedSetstreamfps();
	afx_msg void OnBnClickedButtonsetfacedataadv();
};
