// NewSDKTestDialog.cpp : implementation file
//

#include "stdafx.h"
#include <cstdlib>
#include "HvCamera.h"
#include "HvDeviceNewTest.h"
#include "NewSDKTestDialog.h"
#include ".\newsdktestdialog.h"
#include "HvDeviceNew.h"


// CNewSDKTestDialog dialog

IMPLEMENT_DYNAMIC(CNewSDKTestDialog, CDialog)
CNewSDKTestDialog::CNewSDKTestDialog(HVAPI_HANDLE_EX hHandle, CWnd* pParent /*=NULL*/)
	: CDialog(CNewSDKTestDialog::IDD, pParent)
{
	m_hHandle = hHandle;
}

CNewSDKTestDialog::~CNewSDKTestDialog()
{
}

 BOOL CNewSDKTestDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	RECT rect;
	GetClientRect(&rect);
	SetScrollRange(SB_HORZ,   0,  rect.right-rect.left,   TRUE);
	SetScrollRange(SB_VERT,   0,  rect.bottom-rect.top,   TRUE);	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CNewSDKTestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PARAM1, m_Param1Edit);
	DDX_Control(pDX, IDC_PARAM2, m_Param2Edit);
	DDX_Control(pDX, IDC_PARAM3, m_Param3Edit);
	DDX_Control(pDX, IDC_PARAM4, m_Param4Edit);
	DDX_Control(pDX, IDC_PARAM5, m_Param5Edit);
	DDX_Control(pDX, IDC_PARAM6, m_Param6Edit);
	DDX_Control(pDX, IDC_EDIT1, m_Param7Edit);
	DDX_Control(pDX, IDC_EDIT2, m_Param8Edit);
	DDX_Control(pDX, IDC_EDIT3, m_Param9Edit);
	DDX_Control(pDX, IDC_EDIT4, m_Param10Edit);
}


BEGIN_MESSAGE_MAP(CNewSDKTestDialog, CDialog)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_SetH264BitRate, OnBnClickedSeth264bitrate)
	ON_BN_CLICKED(IDC_SetJpegCompressRate, OnBnClickedSetjpegcompressrate)
	ON_BN_CLICKED(IDC_SetOSDEnable, OnBnClickedSetosdenable)
	ON_BN_CLICKED(IDC_SetOSDTimeEnable, OnBnClickedSetosdtimeenable)
	ON_BN_CLICKED(IDC_SetOSDPos, OnBnClickedSetosdpos)
	ON_BN_CLICKED(IDC_BUTTON7, OnBnClickedButton7)
	ON_BN_CLICKED(IDC_SetOSDFont, OnBnClickedSetosdfont)
	ON_BN_CLICKED(IDC_SetOSDText, OnBnClickedSetosdtext)
	ON_BN_CLICKED(IDC_SetCVBS, OnBnClickedSetcvbs)
	ON_BN_CLICKED(IDC_SetManualShutter, OnBnClickedSetmanualshutter)
	ON_BN_CLICKED(IDC_SetManualGain, OnBnClickedSetmanualgain)
	ON_BN_CLICKED(IDC_SetAWBEnable, OnBnClickedSetawbenable)
	ON_BN_CLICKED(IDC_SetManualRGB, OnBnClickedSetmanualrgb)
	ON_BN_CLICKED(IDC_SetAGCEnable, OnBnClickedSetagcenable)
	ON_BN_CLICKED(IDC_SetAGCLightBaseLine, OnBnClickedSetagclightbaseline)
	ON_BN_CLICKED(IDC_SetAGCParam, OnBnClickedSetagcparam)
	ON_BN_CLICKED(IDC_SetLUT, OnBnClickedSetlut)
	ON_BN_CLICKED(IDC_SetBrightness, OnBnClickedSetbrightness)
	ON_BN_CLICKED(IDC_SetContrast, OnBnClickedSetcontrast)
	ON_BN_CLICKED(IDC_SetSharpness, OnBnClickedSetsharpness)
	ON_BN_CLICKED(IDC_SetSaturation, OnBnClickedSetsaturation)
	ON_BN_CLICKED(IDC_SetWDREnable, OnBnClickedSetwdrenable)
	ON_BN_CLICKED(IDC_SetWDRLevel, OnBnClickedSetwdrlevel)
	ON_BN_CLICKED(IDC_SetDeNoiseEnable, OnBnClickedSetdenoiseenable)
	ON_BN_CLICKED(IDC_SetDeNoiseMode, OnBnClickedSetdenoisemode)
	ON_BN_CLICKED(IDC_SetDeNoiseLevel, OnBnClickedSetdenoiselevel)
	ON_BN_CLICKED(IDC_SetFilterMode, OnBnClickedSetfiltermode)
	ON_BN_CLICKED(IDC_SetDCIRIS, OnBnClickedSetdciris)
	ON_BN_CLICKED(IDC_SetIPInfo, OnBnClickedSetipinfo)
	ON_BN_CLICKED(IDC_SetTime, OnBnClickedSettime)
	ON_BN_CLICKED(IDC_SetTimeZone, OnBnClickedSettimezone)
	ON_BN_CLICKED(IDC_SetNTPEnable, OnBnClickedSetntpenable)
	ON_BN_CLICKED(IDC_SetNTPServerIP, OnBnClickedSetntpserverip)
	ON_BN_CLICKED(IDC_ResetDevice, OnBnClickedResetdevice)
	ON_BN_CLICKED(IDC_RestoreDefaultParam, OnBnClickedRestoredefaultparam)
	ON_BN_CLICKED(IDC_RestoreFactoryParam, OnBnClickedRestorefactoryparam)
	ON_BN_CLICKED(IDC_SetParamEx, OnBnClickedSetparamex)
	ON_BN_CLICKED(IDC_GetParamEx, OnBnClickedGetparamex)
	ON_BN_CLICKED(IDC_GetDevBasicInfo, OnBnClickedGetdevbasicinfo)
	ON_BN_CLICKED(IDC_GetRunMode, OnBnClickedGetrunmode)
	ON_BN_CLICKED(IDC_GetDevState, OnBnClickedGetdevstate)
	ON_BN_CLICKED(IDC_GetResetReport, OnBnClickedGetresetreport)
	ON_BN_CLICKED(IDC_GetResetCount, OnBnClickedGetresetcount)
	ON_BN_CLICKED(IDC_GetLog, OnBnClickedGetlog)
	ON_BN_CLICKED(IDC_GetCameraState, OnBnClickedGetcamerastate)
	ON_BN_CLICKED(IDC_GetCameraBasicInfo, OnBnClickedGetcamerabasicinfo)
	ON_BN_CLICKED(IDC_GetOSD, OnBnClickedGetosd)
	ON_BN_CLICKED(IDC_TriggerImage, OnBnClickedTriggerimage)
	ON_BN_CLICKED(IDC_GetLUT, OnBnClickedGetlut)
	ON_BN_CLICKED(IDC_DoLogin, OnBnClickedDologin)
	ON_BN_CLICKED(IDC_AddUser, OnBnClickedAdduser)
	ON_BN_CLICKED(IDC_DelUser, OnBnClickedDeluser)
	ON_BN_CLICKED(IDC_ModUser, OnBnClickedModuser)
	ON_BN_CLICKED(IDC_GetUsers, OnBnClickedGetusers)
	ON_BN_CLICKED(IDC_GetVideoState, OnBnClickedGetvideostate)
	ON_BN_CLICKED(IDC_SetOCGate1, OnBnClickedSetocgate1)
	ON_BN_CLICKED(IDC_ExecCmdEx, OnBnClickedExeccmdex)
	ON_BN_CLICKED(IDC_BUTTON_SetNTPServerUpdateInterval, OnBnClickedButtonSetntpserverupdateinterval)
	ON_BN_CLICKED(IDC_BUTTON_HVAPI_StartCOMCheck, OnBnClickedButtonHvapiEnablecomcheck)
	ON_BN_CLICKED(IDC_BUTTON_HVAPI_SetSharpnessEnable, OnBnClickedButtonHvapiSetsharpnessenable)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_ShrinkDCIRIS, OnBnClickedButtonhvapiShrinkdciris)
	ON_BN_CLICKED(IDC_BUTTON_HVAPI_ZoomDCIRIS, OnBnClickedButtonHvapiZoomdciris)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SetGrayImageEnable, OnBnClickedButtonhvapiEnablegrayimage)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_StartCameraTest, OnBnClickedButtonhvapiAutotestcamera)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SetEnRedLightEnable, OnBnClickedButtonhvapiSetenredlightenable)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SetRedLightRect, OnBnClickedButtonhvapiSetredlightrect)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SetDebugJpegStatus, OnBnClickedButtonhvapiSetdebugjpegstatus)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_GetAGCZone, OnBnClickedButtonhvapiGetagczone)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_GetEnRedLightEnable, OnBnClickedButtonhvapiGetenredlightenable)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SetOSDPlateEnable, OnBnClickedButtonhvapiSetosdplateenable)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_GetHddCheckReport, OnBnClickedButtonhvapiGethddcheckreport)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SetTraceRank, OnBnClickedButtonhvapiSettracerank)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SetAutoJpegCompressEnable, OnBnClickedButtonhvapiSetautojpegcompressenable)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_GetBlackBoxMessage, OnBnClickedButtonhvapiGetblackboxmessage)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_FPGA, OnBnClickedButtonWriteFpga)
	ON_BN_CLICKED(IDC_BUTTON_READ_FPGA, OnBnClickedButtonReadFpga)
 
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SetACSync, OnBnClickedButtonhvapiSetacsync)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SetImageEnhancementEnable, OnBnClickedButtonhvapiSetimageenhancementenable)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SetGAMMAValue, OnBnClickedButtonhvapiSetgammavalue)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SetGAMMAEnable, OnBnClickedButtonhvapiSetgammaenable)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SetEnRedLightThreshold, OnBnClickedButtonhvapiSetenredlightthreshold)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_GetCameraBasicInfo, OnBnClickedButtonhvapiGetcamerabasicinfo)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SetCtrlCplEnable, OnBnClickedButtonhvapiSetctrlcplenable)
	ON_BN_CLICKED(IDC_BUTTONsetacgzone, OnBnClickedButtonsetagczone)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_GetTraceRank, OnBnClickedButtonhvapiGettracerank)
	ON_BN_CLICKED(IDC_BUTTONGetConnStatusEx, OnBnClickedButtongetconnstatusex)
	ON_BN_CLICKED(IDC_BUTTONGetDevtypeEx, OnBnClickedButtongetdevtypeex)
	ON_BN_CLICKED(IDC_BUTTONSetIPByMacAddr, OnBnClickedButtonsetipbymacaddr)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SetDeNoiseSNFEnable, OnBnClickedButtonhvapiSetdenoisesnfenable)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SetDeNoiseTNFEnable, OnBnClickedButtonhvapiSetdenoisetnfenable)
	ON_BN_CLICKED(IDC_BUTTONSetEdgeEnable, OnBnClickedButtonsetedgeenable)
	ON_BN_CLICKED(IDC_SET_2DDENOISE, OnBnClickedSet2ddenoise)
	ON_BN_CLICKED(IDC_SET_COLOR_, OnBnClickedSetColor)
	ON_BN_CLICKED(IDC_SET_2DDeNoiseValue, OnBnClickedSet2ddenoisevalue)
	ON_BN_CLICKED(IDC_BUTTON_SetAutoJpegCompressParam, OnBnClickedButtonSetautojpegcompressparam)
	ON_BN_CLICKED(IDC_BUTTONGetHDDStatus, OnBnClickedButtongethddstatus)
	ON_BN_CLICKED(IDC_BUTTONGetRunStatusString, OnBnClickedButtongetrunstatusstring)
	ON_BN_CLICKED(IDC_BUTTONGetCameraWorkState, OnBnClickedButtongetcameraworkstate)
	ON_BN_CLICKED(IDC_BUTTONSetTGIO, OnBnClickedButtonsettgio)
	ON_BN_CLICKED(IDC_BUTTONSetF1IO, OnBnClickedButtonsetf1io)
	ON_BN_CLICKED(IDC_BUTTONSetEXPIO, OnBnClickedButtonsetexpio)
	ON_BN_CLICKED(IDC_BUTTONSetALMIO, OnBnClickedButtonsetalmio)
	ON_BN_CLICKED(IDC_BUTTONGetTGIO, OnBnClickedButtongettgio)
	ON_BN_CLICKED(IDC_BUTTONGetF1IO, OnBnClickedButtongetf1io)
	ON_BN_CLICKED(IDC_BUTTONGetEXPIO, OnBnClickedButtongetexpio)
	ON_BN_CLICKED(IDC_BUTTONGetALMIO, OnBnClickedButtongetalmio)
	ON_BN_CLICKED(IDC_BUTTONSetColorGradation, OnBnClickedButtonsetcolorgradation)
	ON_BN_CLICKED(IDC_BUTTONSetVedioRequestControl, OnBnClickedButtonsetvediorequestcontrol)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SetAEScene, OnBnClickedButtonhvapiSetaescene)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_GetAEScene, OnBnClickedButtonhvapiGetaescene)
	ON_BN_CLICKED(IDC_BUTTONSetDevName, OnBnClickedButtonsetdevname)
	ON_BN_CLICKED(IDC_BUTTONGetDevName, OnBnClickedButtongetdevname)
	ON_BN_CLICKED(IDC_BUTTONSetH264SecondBitRate, OnBnClickedButtonseth264secondbitrate)
	ON_BN_CLICKED(IDC_BUTTONGetH264SecondBitRate, OnBnClickedButtongeth264secondbitrate)
	ON_BN_CLICKED(IDC_BUTTONSetDeNoiseSwitch, OnBnClickedButtonsetdenoiseswitch)
	ON_BN_CLICKED(IDC_BUTTONHVAPI_SoftTriggerCapture, OnBnClickedButtonhvapiSofttriggercapture)
	ON_BN_CLICKED(IDC_SetSharpnessEnable, OnBnClickedSetsharpnessenable)
	ON_BN_CLICKED(IDC_SetH264BitRateControl, OnBnClickedSeth264bitratecontrol)
	ON_BN_CLICKED(IDC_SetStreamFps, OnBnClickedSetstreamfps)
	ON_BN_CLICKED(IDC_BUTTONSetFaceDataAdv, OnBnClickedButtonsetfacedataadv)
	END_MESSAGE_MAP()


// CNewSDKTestDialog message handlers

void CNewSDKTestDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//OnOK();
}
void CNewSDKTestDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)        
{
 SCROLLINFO scrollInfo;          
 GetScrollInfo(SB_VERT,   &scrollInfo,   SIF_ALL);          
 switch   (nSBCode)          
 {          
  case   SB_LINEUP:              
   scrollInfo.nPos -= 1;              
   if   (scrollInfo.nPos   <   scrollInfo.nMin)              
   {                  
    scrollInfo.nPos =   scrollInfo.nMin;                  
    break;              
   }              
   SetScrollInfo(SB_VERT,   &scrollInfo,   SIF_ALL);              
   ScrollWindow(0,   1);              
   break;          
  case   SB_LINEDOWN:             
   scrollInfo.nPos += 1;              
   if   (scrollInfo.nPos   >   scrollInfo.nMax)              
   {                  
    scrollInfo.nPos =   scrollInfo.nMax;                  
    break;              
   }              
   SetScrollInfo(SB_VERT,   &scrollInfo,   SIF_ALL);              
   ScrollWindow(0,   -1);              
   break;          
  case   SB_TOP:              
   ScrollWindow(0,   (scrollInfo.nPos   -   scrollInfo.nMin)   *   1);              
   scrollInfo.nPos =   scrollInfo.nMin;              
   SetScrollInfo(SB_VERT,   &scrollInfo,   SIF_ALL);              
   break;          
  case   SB_BOTTOM:              
   ScrollWindow(0,   -(scrollInfo.nMax   -   scrollInfo.nPos)   *   1);              
   scrollInfo.nPos =   scrollInfo.nMax;              
   SetScrollInfo(SB_VERT,   &scrollInfo,   SIF_ALL);             
   break;          
  case   SB_PAGEUP:              
   scrollInfo.nPos -= 1;              
   if   (scrollInfo.nPos   <   scrollInfo.nMin)              
   {                 
    scrollInfo.nPos =   scrollInfo.nMin;                  
    break;              
   }              
   SetScrollInfo(SB_VERT,&scrollInfo,SIF_ALL);              
   ScrollWindow(0,   1   *   1);              
   break;          
  case   SB_PAGEDOWN:              
   scrollInfo.nPos += 1;              
   if   (scrollInfo.nPos   >   scrollInfo.nMax)              
   {                  
    scrollInfo.nPos =   scrollInfo.nMax;                  
    break;              
   }              
   SetScrollInfo(SB_VERT,   &scrollInfo,   SIF_ALL);              
   ScrollWindow(0,   -1   *   1);              
   break;          
  case   SB_ENDSCROLL:              
   break;          
  case   SB_THUMBPOSITION:              
   break;          
  case   SB_THUMBTRACK:
   
   char mstr[64]={0};
   sprintf(mstr,"nPos1:%d nPos1:%d/n",scrollInfo.nPos,nPos);
   OutputDebugString(mstr);  


   ScrollWindow(0,   (scrollInfo.nPos   -   nPos) );              
   scrollInfo.nPos =  nPos;
   SetScrollInfo(SB_VERT,   &scrollInfo,   SIF_ALL);              
   break;          
 }         
}


void CNewSDKTestDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)        
{
 SCROLLINFO scrollInfo;          
 GetScrollInfo(SB_HORZ,   &scrollInfo,   SIF_ALL);          
 switch   (nSBCode)          
 {          
  case   SB_LINEUP:              
   scrollInfo.nPos -= 1;              
   if   (scrollInfo.nPos   <   scrollInfo.nMin)              
   {                  
    scrollInfo.nPos =   scrollInfo.nMin;                  
    break;              
   }              
   SetScrollInfo(SB_HORZ,   &scrollInfo,   SIF_ALL);              
   ScrollWindow(0,   10);              
   break;          
  case   SB_LINEDOWN:             
   scrollInfo.nPos += 1;              
   if   (scrollInfo.nPos   >   scrollInfo.nMax)              
   {                  
    scrollInfo.nPos =   scrollInfo.nMax;                  
    break;              
   }              
   SetScrollInfo(SB_HORZ,   &scrollInfo,   SIF_ALL);              
   ScrollWindow(0,   -10);              
   break;          
  case   SB_TOP:              
   ScrollWindow(0,   (scrollInfo.nPos   -   scrollInfo.nMin)   *   10);              
   scrollInfo.nPos =   scrollInfo.nMin;              
   SetScrollInfo(SB_HORZ,   &scrollInfo,   SIF_ALL);              
   break;          
  case   SB_BOTTOM:              
   ScrollWindow(0,   -(scrollInfo.nMax   -   scrollInfo.nPos)   *   10);              
   scrollInfo.nPos =   scrollInfo.nMax;              
   SetScrollInfo(SB_HORZ,   &scrollInfo,   SIF_ALL);             
   break;          
  case   SB_PAGEUP:              
   scrollInfo.nPos -= 5;              
   if   (scrollInfo.nPos   <   scrollInfo.nMin)              
   {                 
    scrollInfo.nPos =   scrollInfo.nMin;                  
    break;              
   }              
   SetScrollInfo(SB_HORZ,&scrollInfo,SIF_ALL);              
   ScrollWindow(0,   10   *   5);              
   break;          
  case   SB_PAGEDOWN:              
   scrollInfo.nPos += 5;              
   if   (scrollInfo.nPos   >   scrollInfo.nMax)              
   {                  
    scrollInfo.nPos =   scrollInfo.nMax;                  
    break;              
   }              
   SetScrollInfo(SB_HORZ,   &scrollInfo,   SIF_ALL);              
   ScrollWindow(0,   -10   *   5);              
   break;          
  case   SB_ENDSCROLL:              
   break;          
  case   SB_THUMBPOSITION:              
   break;          
  case   SB_THUMBTRACK:              
   ScrollWindow((scrollInfo.nPos   -   nPos)   *   10, 0  );              
   scrollInfo.nPos =   nPos;              
   SetScrollInfo(SB_HORZ,   &scrollInfo,   SIF_ALL);              
   break;          
 }
}
void CNewSDKTestDialog::OnBnClickedSeth264bitrate()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetH264BitRate( m_hHandle ,  atoi( strParam2 ) );
		if ( S_OK == hr )
		{
			MessageBox("HVAPI_SetH264BitRate OK\n ");
			
		}
		else if ( S_FALSE == hr )
		{
			MessageBox("HVAPI_SetH264BitRate InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox("HVAPI_SetH264BitRate Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetjpegcompressrate()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetJpegCompressRate( m_hHandle , atoi( strParam2 ));
		if ( S_OK == hr )
		{
			MessageBox("HVAPI_SetJpegCompressRate OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox("HVAPI_SetJpegCompressRate InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox("HVAPI_SetJpegCompressRate Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetosd()
{
	// TODO: Add your control notification handler code here
}

void CNewSDKTestDialog::OnBnClickedSetosdenable()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);

		HRESULT hr = HVAPI_SetOSDEnable( m_hHandle , atoi(strParam2) , atoi(strParam3)  );
		if ( S_OK == hr )
		{
			MessageBox("HVAPI_SetOSDEnable OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox("HVAPI_SetOSDEnable InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox("HVAPI_SetOSDEnable Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetosdtimeenable()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);

		HRESULT hr = HVAPI_SetOSDTimeEnable( m_hHandle , atoi(strParam2) , atoi(strParam3)  );
		if ( S_OK == hr )
		{
			MessageBox("HVAPI_SetOSDTimeEnable OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox("HVAPI_SetOSDTimeEnable InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox("HVAPI_SetOSDTimeEnable Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetosdpos()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);
		CString strParam4 ;
		m_Param4Edit.GetWindowText(strParam4);

		HRESULT hr = HVAPI_SetOSDPos( m_hHandle , atoi(strParam2) , atoi(strParam3) , atoi(strParam4) );
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButton7()
{
	// TODO: Add your control notification handler code here
}

void CNewSDKTestDialog::OnBnClickedSetosdfont()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);
		CString strParam4 ;
		m_Param4Edit.GetWindowText(strParam4);
		CString strParam5 ;
		m_Param5Edit.GetWindowText(strParam5);
		CString strParam6 ;
		m_Param6Edit.GetWindowText(strParam6);

		HRESULT hr = HVAPI_SetOSDFont( m_hHandle , atoi(strParam2) , atoi(strParam3) , atoi(strParam4), atoi(strParam5), atoi(strParam6));
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetosdtext()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);

		HRESULT hr = HVAPI_SetOSDText( m_hHandle , atoi(strParam2) , strParam3.GetBuffer()  );
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}

		strParam3.ReleaseBuffer();
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetcvbs()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetCVBDisPlayMode( m_hHandle , atoi(strParam2)  ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetmanualshutter()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr =  HVAPI_SetManualShutter(m_hHandle , atoi(strParam2) );
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetmanualgain()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetManualGain(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetawbenable()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetAWBEnable(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetawbmode()
{

}

void CNewSDKTestDialog::OnBnClickedSetmanualrgb()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);
		CString strParam4 ;
		m_Param4Edit.GetWindowText(strParam4);

		HRESULT hr =  HVAPI_SetManualRGB(m_hHandle , atoi(strParam2) , atoi(strParam3), atoi(strParam4));
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetagcenable()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetAGCEnable(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetagclightbaseline()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetAGCLightBaseLine(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetagcparam()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);
		CString strParam4 ;
		m_Param4Edit.GetWindowText(strParam4);
		CString strParam5 ;
		m_Param5Edit.GetWindowText(strParam5);

		HRESULT hr = HVAPI_SetAGCParam(m_hHandle , atoi(strParam2) , atoi(strParam3), atoi(strParam4) , atoi(strParam5)) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

HRESULT GetAGCZone( CString strAGCZone , INT rgAgcZone[16] , INT* pZoneCount )
{
	if (  NULL == pZoneCount )
	{
		return E_POINTER;
	}
	INT nLeft = 0;
	INT nRight = 0;
	CString strTempRight ;
	CString strTempLeft;

	nLeft = strAGCZone.Find( '(' , 0 );
	strAGCZone = strAGCZone.Right( strAGCZone.GetLength() - nLeft );
	INT nPointIndex = 0;

	while ( (nRight = strAGCZone.Find( ')' , 0 )) >0 && 
		(nLeft = strAGCZone.Find( '(' , 0 )) >=0  &&
		nLeft < nRight
		)

	{
		strTempRight = strAGCZone.Mid( nLeft, nRight);
		strAGCZone = strAGCZone.Right( strAGCZone.GetLength() - nRight -1 );

		if ( nPointIndex < *pZoneCount )
		{
			sscanf(strTempRight.GetBuffer() , "(%d)" ,&rgAgcZone[nPointIndex]  );
			strTempRight.ReleaseBuffer();

			nPointIndex++;

		}
		else
		{
			break;
		}
	}

	*pZoneCount = nPointIndex;
	return S_OK;
}

HRESULT GetLutPoint( CString strPoint,  CHvPoint* prgPoint , INT* pnPointCount )
{
	if ( NULL == prgPoint || NULL == pnPointCount )
	{
		return E_POINTER;
	}
	INT nLeft = 0;
	INT nRight = 0;
	CString strTempRight ;
	CString strTempLeft;
	
	nLeft = strPoint.Find( '(' , 0 );
	strPoint = strPoint.Right( strPoint.GetLength() - nLeft );
	INT nPointIndex = 0;
	
	while ( (nRight = strPoint.Find( ')' , 0 )) >0 && 
			(nLeft = strPoint.Find( '(' , 0 )) >=0  &&
			nLeft < nRight
		)
		
	{
		strTempRight = strPoint.Left( nRight + 1 );
		strPoint = strPoint.Right( strPoint.GetLength() - nRight -1 );

		if ( nPointIndex < *pnPointCount )
		{
			sscanf(strTempRight.GetBuffer() , "(%d,%d)" ,&prgPoint[nPointIndex].nX , &prgPoint[nPointIndex].nY  );
			strTempRight.ReleaseBuffer();
		
			nPointIndex++;

		}
		else
		{
			break;
		}
	}

	*pnPointCount = nPointIndex;
	return S_OK;

}


void CNewSDKTestDialog::OnBnClickedSetlut()
{
	// x,y; x,y
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);

		CHvPoint rgPoint[8];
		memset( rgPoint , 0 , sizeof(CHvPoint)*8 );
		INT nPointCount = 8;

		GetLutPoint( strParam2 , rgPoint , &nPointCount );

		HRESULT hr = HVAPI_SetLUT( m_hHandle , rgPoint , atoi(strParam3)  ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetbrightness()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);


		HRESULT hr = HVAPI_SetBrightness(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetcontrast()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetContrast(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetsharpness()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetSharpness(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetsaturation()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetSaturation(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetwdrenable()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetWDREnable(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetwdrlevel()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetWDRLevel(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetblcenable()
{

}

void CNewSDKTestDialog::OnBnClickedSetblclevel()
{

}

void CNewSDKTestDialog::OnBnClickedSetdremode()
{

}

void CNewSDKTestDialog::OnBnClickedSetdrenable()
{

}

void CNewSDKTestDialog::OnBnClickedSetdrelevel()
{

}

void CNewSDKTestDialog::OnBnClickedSetdenoiseenable()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);


		HRESULT hr = HVAPI_SetDeNoiseSNFEnable(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetdenoisemode()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetDeNoiseMode(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetdenoiselevel()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetDeNoiseLevel(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetfiltermode()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr =  HVAPI_SetFilterMode(m_hHandle , atoi(strParam2) );
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetdciris()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetDCIRIS(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetipinfo()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);
		CString strParam4 ;
		m_Param4Edit.GetWindowText(strParam4);
		CString strParam5 ;
		m_Param5Edit.GetWindowText(strParam5);

		HRESULT hr = HVAPI_SetIPInfo(m_hHandle , strParam2.GetBuffer() 
			, strParam3.GetBuffer() 
			, strParam4.GetBuffer() 
			, strParam5.GetBuffer()
			) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}

		strParam2.ReleaseBuffer();
		strParam3.ReleaseBuffer();
		strParam4.ReleaseBuffer();
		strParam5.ReleaseBuffer();
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetrtspmulticastenable()
{
	/*
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		if ( S_OK != HVAPI_SetRTSPMulticastEnable(m_hHandle , atoi(strParam2)) )
		{
			MessageBox("HVAPI_SetRTSPMulticastEnable Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetRTSPMulticastEnable OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
	*/
}

void CNewSDKTestDialog::OnBnClickedSettime()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);
		CString strParam4 ;
		m_Param4Edit.GetWindowText(strParam4);
		CString strParam5 ;
		m_Param5Edit.GetWindowText(strParam5);
		CString strParam6 ;
		m_Param6Edit.GetWindowText(strParam6);
		CString strParam7 ;
		m_Param7Edit.GetWindowText(strParam7);
		CString strParam8 ;
		m_Param8Edit.GetWindowText(strParam8);

		HRESULT hr = HVAPI_SetTime(m_hHandle , atoi(strParam2)
			, atoi(strParam3) , atoi(strParam4)
			, atoi(strParam5) , atoi(strParam6)
			, atoi(strParam7) , atoi(strParam8)
			) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSettimezone()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetTimeZone(m_hHandle , atoi(strParam2)) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetntpenable()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr =  HVAPI_SetNTPEnable(m_hHandle , atoi(strParam2));
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetntpserverip()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetNTPServerIP(m_hHandle , strParam2.GetBuffer()  ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}

		strParam2.ReleaseBuffer();
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedResetdevice()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		HRESULT hr = HVAPI_ResetDevice(m_hHandle , atoi( strParam2.GetBuffer() )  ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedRestoredefaultparam()
{
	if( NULL != m_hHandle )
	{
		if ( S_OK != HVAPI_RestoreDefaultParam(m_hHandle) )
		{
			MessageBox("HVAPI_RestoreDefaultParam Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_RestoreDefaultParam OK\n ");
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedRestorefactoryparam()
{
	if( NULL != m_hHandle )
	{
		if ( S_OK != HVAPI_RestoreFactoryParam(m_hHandle) )
		{
			MessageBox("HVAPI_RestoreFactoryParam Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_RestoreFactoryParam OK\n ");
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetparamex()
{

	char szFileFilter[] = "XML文件|*.xml|所有文件|*.*|";
	CString strSelectedFile;
	CFileDialog FileDialogBox(
		TRUE, NULL, "",
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		szFileFilter, this
		);
	if ( FileDialogBox.DoModal() == IDOK )
	{
		strSelectedFile = FileDialogBox.GetPathName();
		char* pszXmlParam;
		unsigned long ulFileSize = 0; 
		unsigned long ulReadedSize = 0;
		FILE *fp = fopen(strSelectedFile.GetBuffer(0), "rb");  
		if ( fp )  
		{  
			fseek(fp, 0, SEEK_END);  
			ulFileSize = ftell(fp);
			pszXmlParam = new char[ulFileSize];
			fseek(fp, 0, SEEK_SET);
			ulReadedSize = (unsigned long)fread(pszXmlParam, 1, ulFileSize, fp);  
			fclose(fp); 
			if(ulReadedSize != ulFileSize)
			{
				return; 
			}
		}  

		if ( pszXmlParam )
		{
			if ( S_OK == HVAPI_SetParamEx(m_hHandle, pszXmlParam) )
			{
				AfxMessageBox("HVAPI_SetXmlParam is OK");
			}
			else
			{
				AfxMessageBox("ERROR!");
			}
		}
		delete[] pszXmlParam;
	}

}

void CNewSDKTestDialog::OnBnClickedGetparamex()
{
	static char szXmlParam[512*1024] = {0};
	INT nRetLen = 0;

	if ( S_OK == HVAPI_GetParamEx(m_hHandle, szXmlParam, sizeof(szXmlParam), &nRetLen) )
	{
		CFileDialog cFileDlg(FALSE);
		if(cFileDlg.DoModal() == IDOK)
		{
			FILE* fp = fopen(cFileDlg.GetPathName(), "wb");
			if(fp)
			{
				fwrite(szXmlParam, strlen(szXmlParam), 1, fp);
				fclose(fp);
			}
		}

		CString strMsg;
		strMsg.Format("HVAPI_GetXmlParam is OK. %d == %d ?\n\nAlready save to %s'", strlen(szXmlParam), nRetLen,
			cFileDlg.GetPathName());
		AfxMessageBox(strMsg);
	}
	else
	{
		AfxMessageBox("ERROR!");
	}
}

void CNewSDKTestDialog::OnBnClickedGetdevbasicinfo()
{
	if( NULL != m_hHandle )
	{
		int nXmlBufLen = 128*1024;
		char* pXmlBuf = new char[nXmlBufLen];
		if ( NULL != pXmlBuf )
		{
			CDevBasicInfo cInfo;
			if ( S_OK != HVAPI_GetDevBasicInfo( m_hHandle , &cInfo  ) )
			{
				MessageBox("HVAPI_GetDevBasicInfo Fail\n ");
			}
			else
			{
				char szRetInfo[512];
				sprintf( szRetInfo , 
					"DevType:%s , Version:%s, ModeVersion:%s , SN:%s, WorkMode:%s\n" 
					"IP:%s\n"
					"Mac:%s\n"
					"Mask:%s\n"
					"Gateway:%s\n"
					"Mode:%s\n"
					"BackupVersion:%s\n"
					"FPGAVersion:%s\n"
					"KernelVersion:%s\n"
					"UbootVersion:%s\n"
					"UBLVersion:%s\n"
					,cInfo.szDevType , cInfo.szDevVersion,
					cInfo.szModelVersion , cInfo.szSN , cInfo.szWorkMode
					,cInfo.szIP
					,cInfo.szMac
					,cInfo.szMask
					,cInfo.szGateway
					,cInfo.szMode
					,cInfo.szBackupVersion
					,cInfo.szFPGAVersion
					,cInfo.szKernelVersion
					,cInfo.szUbootVersion
					,cInfo.szUBLVersion
					
					);
				MessageBox( szRetInfo  );
			}
			delete pXmlBuf;
			pXmlBuf = NULL;
		}


	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedGetrunmode()
{
	/*if( NULL != m_hHandle )
	{
		INT nRunMode = -1;
		if ( S_OK != HVAPI_GetRunMode( m_hHandle ,&nRunMode ) )
		{
			char szRetInfo[128]={0};
			sprintf( szRetInfo , "GetRunMode:%d\n" , nRunMode );
			MessageBox(szRetInfo);
		}
		else
		{
			MessageBox("HVAPI_RestoreFactoryParam OK\n ");
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}*/
}

void CNewSDKTestDialog::OnBnClickedGetdevstate()
{
	if( NULL != m_hHandle )
	{
		CDevState cDevState;
		if ( S_OK != HVAPI_GetDevState(m_hHandle , &cDevState ) )
		{
			MessageBox("HVAPI_GetDevState Fail\n ");
		}
		else
		{
			
			char szDebug[1024];
			sprintf( szDebug 
				, "DevState: Cpu:%d: , DateTime:%4d:%2d:%2d %2d:%2d:%2d:%2d \n,"
				"Mem:%d,Temp:%d,  HddState:%d, OCGate:%d,"
				"\n NTPEnable:%d\nNTPServer:%s \n NTPServerUpdateInterval%d\n"
				"TimeZone:%d\n"
				"RecordLinkIP:%s \n"
				"VideoLinkIP:%s \n"
				"ImageLinkIP:%s \n"
				"TraceRank:%d\n"
				,cDevState.nCpuUsage
				,cDevState.nYear, cDevState.nMon, cDevState.nDay
				,cDevState.nHour, cDevState.nMin, cDevState.nSec,cDevState.nMSec
				,cDevState.nMemUsage
				,cDevState.nTemperature
				,cDevState.nHddState
				,cDevState.fOCGateEnable
				,cDevState.fNTPEnable
				,cDevState.szNTPServerIP
				,cDevState.nNTPServerUpdateInterval
				,cDevState.nTimeZone
				,cDevState.szRecordLinkIP
				,cDevState.szVideoLinkIP
				,cDevState.szImageLinkIP
				,cDevState.nTraceRank
				);
			MessageBox( szDebug );
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedGetresetreport()
{
	if( NULL != m_hHandle )
	{
		int nReportLen = 17*1024;
		CHAR szReport[17*1024] = {0};
		if ( S_OK != HVAPI_GetResetReport( m_hHandle , szReport , &nReportLen ))
		{
			MessageBox("HVAPI_GetResetReport Fail\n ");
		}
		else
		{
			MessageBox(szReport);
			MessageBox("HVAPI_GetResetReport OK\n ");
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedGetresetcount()
{
	if( NULL != m_hHandle )
	{
		int nRetCount = 0;
		if ( S_OK != HVAPI_GetResetCount(m_hHandle , &nRetCount ) )
		{
			MessageBox("HVAPI_GetResetCount Fail\n ");
		}
		else
		{
			char szDebug[256];
			sprintf( szDebug , "ResetCount: %d\n"
				,nRetCount
				);
			MessageBox( szDebug );
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedGetlog()
{
	if( NULL != m_hHandle )
	{
		int nLogByte = 8*1024; 
		CHAR* pLog = new CHAR[nLogByte];

		if ( NULL != pLog )
		{
			if ( S_OK != HVAPI_GetLog( m_hHandle , pLog , &nLogByte  ) )
			{
				MessageBox("HVAPI_GetLog Fail\n ");
			}
			else
			{
				MessageBox(pLog , NULL);
			}
			delete pLog;
			pLog = NULL;
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedGetcamerastate()
{
	if( NULL != m_hHandle )
	{
		CCameraState cCamState;
		if ( S_OK != HVAPI_GetCameraState(m_hHandle , &cCamState ) )
		{
			MessageBox("HVAPI_GetCameraState Fail\n ");
		}
		else
		{
			char szDebug[256];
			sprintf( szDebug , "cCamState: AGC:%d: ,AWB: %d ,Shutter:%d ,Gain:%d, R:%d,G:%d,B:%d\n"
							
				,cCamState.fAGCEnable
				,cCamState.fAWBEnable
				,cCamState.nShutter,cCamState.nGain
				,cCamState.nGainR
				,cCamState.nGainG
				,cCamState.nGainB

				);
			MessageBox( szDebug );
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedGetcamerabasicinfo()
{
	if( NULL != m_hHandle )
	{
		CCameraBasicInfo cBasicInfo;
		if ( S_OK != HVAPI_GetCameraBasicInfo( m_hHandle , &cBasicInfo ) )
		{
			MessageBox("HVAPI_GetCameraBasicInfo Fail\n ");
		}
		else
		{
			char szDebug[1024];
			sprintf( szDebug , 
				"cBasicInfo: \n"
				"AGC:%d: GainMax:%d ,Min:%d ,BaseLine:%d ,ShuterMax:%d,Min:%d\n"
				"AWB:%d \n"
				"Color:%d, 2DDeNoiseEnable:%d,2DDeNoiseValue:%d\n"
				"WDR:%d, %d\n"
				"Brightness:%d.nContrast:%d, nSharpness:%d,Saturation:%d\n"
				"DeNoise: SNF:%d,TNF:%d , Level:%d,Mode:%d\n"
				"Manual :shutter:%d,gain:%d \nR:%d,G:%d,B:%d\n"
				"FilterMode:%d,DCEnable:%d \n"
				"Gamma:Enable:%d,Strength:%d\n"
				"ImageEnhancementEnable:%d\n"
				"ACSyncMode:%d\n"
				"ACSyncDelay:%d\n"
				"GrayImageEnable:%d\n"
				"EdgeEnhance:%d\n"
				
				,cBasicInfo.fAGCEnable//
				,cBasicInfo.nAGCGainMax//
				,cBasicInfo.nAGCGainMin//
				,cBasicInfo.nAGCLightBaseLine////
				,cBasicInfo.nAGCShutterMax//
				,cBasicInfo.nAGCShutterMin//
				
				,cBasicInfo.fAWBEnable//
			
				,cBasicInfo.nColor//
				,cBasicInfo.f2DDeNoiseEnable//
				,cBasicInfo.n2DeNoiseStrength//

				,cBasicInfo.fWDREnable//
				,cBasicInfo.nWDRLevel//

				,cBasicInfo.nBrightness//
				,cBasicInfo.nContrast//
				,cBasicInfo.nSharpness//
				,cBasicInfo.nSaturation//

				,cBasicInfo.fDeNoiseSNFEnable//
				,cBasicInfo.fDeNoiseTNFEnable//
				,cBasicInfo.nDeNoiseLevel//
				,cBasicInfo.nDeNoiseMode//

				,cBasicInfo.nManualShutter//
				,cBasicInfo.nManualGain//
				,cBasicInfo.nManualGainR//
				,cBasicInfo.nManualGainG//
				,cBasicInfo.nManualGainB//
				

				,cBasicInfo.nFilterMode//
				,cBasicInfo.fDCEnable//
				,cBasicInfo.fGammaEnable//
				,cBasicInfo.nGammaStrength//
				,cBasicInfo.fImageEnhancementEnable//////
				,cBasicInfo.nACSyncMode//////
				,cBasicInfo.nACSyncDelay//////
				,cBasicInfo.fGrayImageEnable//////
				,cBasicInfo.nEdgeEnhance////
				);
			MessageBox( szDebug );
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedGetosd()
{
	if( NULL != m_hHandle )
	{
		CString strParam2;
		m_Param2Edit.GetWindowText(strParam2);

		COSDInfo cOSDInfo;
		if ( S_OK != HVAPI_GetOSDInfo( m_hHandle , atoi(strParam2) , &cOSDInfo  ) )
		{
			MessageBox("HVAPI_GetOSDInfo Fail\n ");
		}
		else
		{
			char szDebug[256];
			sprintf( szDebug , "OSDEnable:%d: ,PlateEnable:%d, TimeStampEnable: %d ,R:%d ,G:%d, B:%d,FontSize:%d,X:%d,Y:%d, Text:%s\n"
				,cOSDInfo.fEnable
				,cOSDInfo.fPlateEnable
				,cOSDInfo.fTimeStampEnable
				,cOSDInfo.nFontColorR,cOSDInfo.nFontColorG,cOSDInfo.nFontColorB
				,cOSDInfo.nFontSize
				,cOSDInfo.nPosX,cOSDInfo.nPosY
				,cOSDInfo.szText
				);
			MessageBox( szDebug );
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedTriggerimage()
{
	if( NULL != m_hHandle )
	{
		int nImageByte = 2*1024*1024; 
		PBYTE pImage = new BYTE[nImageByte];

		if ( NULL != pImage )
		{
			if ( S_OK != HVAPI_TriggerImage( m_hHandle , pImage , &nImageByte  ) )
			{
				MessageBox("HVAPI_TriggerImage Fail\n ");
			}
			else if ( NULL != pImage && nImageByte > 0 )
			{
				FILE* fp = fopen("Trigger.jpg" , "wb+");
				if(fp)
				{
					fwrite(pImage, nImageByte, 1, fp);
					fclose(fp);		
				}
			}
			delete pImage;
			pImage = NULL;
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedGetlut()
{
	if( NULL != m_hHandle )
	{
		CHvPoint rgPoint[32];
		int nPointCount = 32;
		if ( S_OK != HVAPI_GetLUT( m_hHandle , rgPoint , &nPointCount  ) )
		{
			MessageBox("HVAPI_GetLUT Fail\n ");
		}
		else
		{
			char szDebug[256]="LUT:";
			char szTemp[128]={0};
			for (int i = 0; i< nPointCount ; i++ )
			{
				sprintf( szTemp , " %d,%d, \n " , rgPoint[i].nX , rgPoint[i].nY );
				strcat( szDebug , szTemp );
				if ( (i+1) %4 == 0 )
				{
					strcat( szDebug , "\n" );
				}
			}

			MessageBox( szDebug );
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedDologin()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);

		HRESULT hr = HVAPI_Login( m_hHandle , strParam2.GetBuffer() 
			, strParam3.GetBuffer() ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}

		strParam2.ReleaseBuffer();
		strParam3.ReleaseBuffer();
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedAdduser()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);
		CString strParam4 ;
		m_Param4Edit.GetWindowText(strParam4);

		HRESULT hr =  HVAPI_AddUser( m_hHandle , strParam2.GetBuffer() 
			, strParam3.GetBuffer(), atoi(strParam4));
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}

		strParam2.ReleaseBuffer();
		strParam3.ReleaseBuffer();

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedDeluser()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr =  HVAPI_DelUser( m_hHandle , strParam2.GetBuffer());
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}

		strParam2.ReleaseBuffer();
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedModuser()
{
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);
		CString strParam4 ;
		m_Param4Edit.GetWindowText(strParam4);
		CString strParam5 ;
		m_Param5Edit.GetWindowText(strParam5);

		HRESULT hr = HVAPI_ModUser( m_hHandle , strParam2.GetBuffer() 
			, strParam3.GetBuffer()
			, strParam4.GetBuffer()) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}

		strParam2.ReleaseBuffer();
		strParam3.ReleaseBuffer();
		strParam4.ReleaseBuffer();

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedGetusers()
{
	if( NULL != m_hHandle )
	{
		CUserInfo cUserInfo[26];
		int nUserInfoNum =26;

		if ( S_OK != HVAPI_GetUsersList(m_hHandle , cUserInfo , &nUserInfoNum ) )
		{
			MessageBox("HVAPI_GetUsersList Fail\n ");
		}
		else
		{
			char szAllUser[1024]={0};
			char szDebug[128];
			for (int i = 0 ; i < nUserInfoNum ; i++ )
			{
				sprintf( szDebug , "User_%d: %s , %d\n" ,i,
					cUserInfo[i].szUserName, cUserInfo[i].nAuthority );
				strcat( szAllUser , szDebug );
				
			}
			MessageBox( szAllUser );
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedGetvideostate()
{
	if( NULL != m_hHandle )
	{
		CVideoState cVideoState;
		if ( S_OK != HVAPI_GetVideoState( m_hHandle ,&cVideoState  ))
		{
			MessageBox("HVAPI_GetVideoState Fail\n ");
		}
		else
		{
			char szDebug[1024] ={0};
			sprintf( szDebug , "CVB:%d , H264BitRate:%d,H264Ctrl:%d,H264Fps:%d , JpegFPS:%d , JpegCompressRate:%d\n" 
				"AutoJpegCompressEnable:%d\n"
				"JpegFileSize:%d\n"
				"JpegCompressMaxRate:%d\n"
				"JpegCompressMinRate:%d\n"
				"DebugJpegStatus:%d\n"
				, cVideoState.nCVBSDisplayMode 
				, cVideoState.nH264BitRate
				, cVideoState.nH264BitRateControl
				, cVideoState.nH264FPS
				, cVideoState.nJpegFPS
				, cVideoState.nJpegCompressRate
				, cVideoState.fAutoJpegCompressEnable
				, cVideoState.nJpegFileSize
				, cVideoState.nJpegCompressMaxRate
				, cVideoState.nJpegCompressMinRate
				, cVideoState.nDebugJpegStatus
				
				);
			MessageBox(szDebug);

		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetsensewdr()
{

}

void CNewSDKTestDialog::OnBnClickedSetocgate()
{

}

void CNewSDKTestDialog::OnBnClickedSetocgate1()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr =  HVAPI_SetOCGate(m_hHandle , atoi(strParam2) );
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}

}

void CNewSDKTestDialog::OnBnClickedExeccmdex()
{
	// TODO: Add your control notification handler code here
	if(m_hHandle == NULL)
	{
		MessageBox("未连接到识别器");
		return;
	}
	CString strBuf;
	GetDlgItemText(IDC_EDIT_ExecCmdEx, strBuf);
	if(strBuf == "")
	{
		MessageBox("命令字符串不能为空");
		return;
	}

	char szRetBuf[128*1024] = {0};
	INT nRetLen = 0;
	if ( S_OK == HVAPI_ExecCmdEx(m_hHandle, strBuf.GetBuffer(0), szRetBuf, sizeof(szRetBuf), &nRetLen) ) 
	{
		CString strMsg;
		strMsg.Format("HVAPI_ExecCmd is OK. \nRecvStringLen = %d \n RecvBitsNum = %d ?\n\n%s", strlen(szRetBuf), nRetLen, szRetBuf);
		AfxMessageBox(strMsg);
	}
	else
	{
		AfxMessageBox("ERROR!");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonSetntpserverupdateinterval()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		if ( S_OK != HVAPI_SetNTPServerUpdateInterval(m_hHandle , atoi(strParam2) ) )
		{
			MessageBox("HVAPI_SetNTPServerUpdateInterval Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetNTPServerUpdateInterval OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonHvapiEnablecomcheck()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);

		if ( S_OK != HVAPI_StartCOMCheck(m_hHandle , atoi(strParam2), atoi(strParam3) ) )
		{
			MessageBox("HVAPI_StartCOMCheck Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_StartCOMCheck OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonHvapiSetsharpnessenable()
{
	// TODO: Add your control notification handler code here

	/*if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		if ( S_OK != HVAPI_SetSharpnessEnable(m_hHandle , atoi(strParam2) ) )
		{
			MessageBox("HVAPI_SetSharpnessEnable Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetSharpnessEnable OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}*/
}

void CNewSDKTestDialog::OnBnClickedButtonhvapiShrinkdciris()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{

		if ( S_OK != HVAPI_ShrinkDCIRIS(m_hHandle   ) )
		{
			MessageBox("HVAPI_ShrinkDCIRIS Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_ShrinkDCIRIS OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonHvapiZoomdciris()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{

		if ( S_OK != HVAPI_ZoomDCIRIS(m_hHandle   ) )
		{
			MessageBox("HVAPI_ZoomDCIRIS Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_ZoomDCIRIS OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonhvapiEnablegrayimage()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		if ( S_OK != HVAPI_SetGrayImageEnable(m_hHandle , atoi(strParam2) ) )
		{
			MessageBox("HVAPI_SetGrayImageEnable Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetGrayImageEnable OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}


void CNewSDKTestDialog::OnBnClickedButtonhvapiAutotestcamera()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{

		if ( S_OK != HVAPI_StartCameraTest(m_hHandle   ) )
		{
			MessageBox("HVAPI_StartCameraTest Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_StartCameraTest OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonhvapiSetenredlightenable()
{
	// TODO: Add your control notification handler code here
	///*if( NULL != m_hHandle )
	//{
	//	CString strParam2 ;
	//	m_Param2Edit.GetWindowText(strParam2);

	//	if ( S_OK != HVAPI_SetEnRedLightEnable(m_hHandle , atoi(strParam2) ) )
	//	{
	//		MessageBox("HVAPI_SetEnRedLightEnable Fail\n ");
	//	}
	//	else
	//	{
	//		MessageBox("HVAPI_SetEnRedLightEnable OK\n ");
	//	}
	//}
	//else
	//{
	//	MessageBox("NULL == m_hHandle \n ");
	//}*/
}

void CNewSDKTestDialog::OnBnClickedButtonhvapiSetredlightrect()
{
	//// TODO: Add your control notification handler code here
	//if( NULL != m_hHandle )
	//{
	//	//CString strParam2 ;
	//	//m_Param2Edit.GetWindowText(strParam2);

	//	CHvPoint rgPoint[16];
	//	for (int i=0; i<16; i++)
	//	{
	//		rgPoint[i].nX=i;
	//		rgPoint[i].nY=i*2;
	//	}

	//	if ( S_OK != HVAPI_SetRedLightRect(m_hHandle , rgPoint, 16 ) )
	//	{
	//		MessageBox("HVAPI_SetRedLightRect Fail\n ");
	//	}
	//	else
	//	{
	//		MessageBox("HVAPI_SetRedLightRect OK\n ");
	//	}
	//}
	//else
	//{
	//	MessageBox("NULL == m_hHandle \n ");
	//}
}

void CNewSDKTestDialog::OnBnClickedButtonhvapiSetdebugjpegstatus()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		if ( S_OK != HVAPI_SetDebugJpegStatus(m_hHandle , atoi(strParam2) ) )
		{
			MessageBox("HVAPI_SetDebugJpegStatus Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetDebugJpegStatus OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}




void CNewSDKTestDialog::OnBnClickedButtonhvapiGetenredlightenable()
{
	// TODO: Add your control notification handler code here


	/*INT nmode=-1;
	if ( S_OK != HVAPI_GetEnRedLightEnable(m_hHandle, &nmode ) )
	{
		MessageBox("HVAPI_GetEnRedLightEnable Fail\n ");
	}
	else
	{
		MessageBox("HVAPI_GetEnRedLightEnable OK\n ");
	}*/
}


 
void CNewSDKTestDialog::OnBnClickedButtonhvapiSetosdplateenable()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2;
		m_Param2Edit.GetWindowText(strParam2);

		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);

		if ( S_OK != HVAPI_SetOSDPlateEnable(m_hHandle ,atoi(strParam2), atoi(strParam3) ) )
		{
			MessageBox("HVAPI_SetOSDPlateEnable Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetOSDPlateEnable OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonhvapiGethddcheckreport()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		int nReportLen = 7 * 1024 ;
		CHAR szReport[ 7 * 1024] = {0};
		if ( S_OK != HVAPI_GetHddCheckReport( m_hHandle , szReport , &nReportLen ))
		{
			MessageBox("HVAPI_GetHddCheckReport Fail\n ");
		}
		else
		{
			MessageBox(szReport);
			MessageBox("HVAPI_GetHddCheckReport OK\n ");
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

 
void CNewSDKTestDialog::OnBnClickedButtonhvapiSettracerank()
{
	// TODO: Add your control notification handler code here
	/*if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		if ( S_OK != HVAPI_SetTraceRank(m_hHandle , atoi(strParam2) ) )
		{
			MessageBox("HVAPI_SetTraceRank Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetTraceRank OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}*/
}

void CNewSDKTestDialog::OnBnClickedButtonhvapiSetautojpegcompressenable()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		if ( S_OK != HVAPI_SetAutoJpegCompressEnable(m_hHandle , atoi(strParam2) ) )
		{
			MessageBox("HVAPI_SetAutoJpegCompressEnable Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetAutoJpegCompressEnable OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

 
UINT CNewSDKTestDialog::MuilThreadFunc(LPVOID lpvoid)
{
	
	CHAR szRet[1024]={0};
	INT nRetLen = 1024;
	
	CHAR szRet1[1024]={0};
	INT nRetLen1 = 1024;
	while(true)
	{

		HVAPI_HANDLE_EX handle1=HVAPI_OpenEx("172.18.88.88", NULL);

		if ( S_OK != HVAPI_ExecCmdEx(handle1 , "GetVersion;GetDevType;" , szRet1 , nRetLen1 , &nRetLen1) )
		{
			//WrightLogEx(pHHC->szIP , "SetOSD Enable HVAPI_ExecCmdEx Fail\n");
			//return E_FAIL;
		}

		HVAPI_CloseEx(handle1);

		handle1=HVAPI_OpenEx("172.18.6.17", NULL);

		if ( S_OK != HVAPI_ExecCmdEx(handle1 , "GetVersion;GetDevType;" , szRet1 , nRetLen1 , &nRetLen1) )
		{
			//WrightLogEx(pHHC->szIP , "SetOSD Enable HVAPI_ExecCmdEx Fail\n");
			//return E_FAIL;
		}

		HVAPI_CloseEx(handle1);
	} 
 

}


UINT CNewSDKTestDialog::MuliThreadFunc2(LPVOID lpvoid)
{

 

	CHAR szRet2[1024]={0};
	INT nRetLen2 = 1024;
	while(true)
	{

		HVAPI_HANDLE_EX handle2=HVAPI_OpenEx("172.18.88.59", NULL);

		if ( S_OK != HVAPI_ExecCmdEx(handle2 , "GetVersion;GetDevType;" , szRet2 , nRetLen2 , &nRetLen2) )
		{
			//WrightLogEx(pHHC->szIP , "SetOSD Enable HVAPI_ExecCmdEx Fail\n");
			//return E_FAIL;
		}

		HVAPI_CloseEx(handle2);
	}


 
 
 
}


UINT CNewSDKTestDialog::MuilThreadFunc3(LPVOID lpvoid)
{

 
	CHAR szRet3[1024]={0};
	INT nRetLen3 = 1024;
	while(true)
	{
	
		HVAPI_HANDLE_EX handle3=HVAPI_OpenEx("172.18.105.9", NULL);

		if ( S_OK != HVAPI_ExecCmdEx(handle3 , "GetVersion;GetDevType;" , szRet3 , nRetLen3 , &nRetLen3) )
		{
			//WrightLogEx(pHHC->szIP , "SetOSD Enable HVAPI_ExecCmdEx Fail\n");
			//return E_FAIL;
		}

		HVAPI_CloseEx(handle3);


		  handle3=HVAPI_OpenEx("172.18.106.2", NULL);

		if ( S_OK != HVAPI_ExecCmdEx(handle3 , "GetVersion;GetDevType;" , szRet3 , nRetLen3 , &nRetLen3) )
		{
			//WrightLogEx(pHHC->szIP , "SetOSD Enable HVAPI_ExecCmdEx Fail\n");
			//return E_FAIL;
		}

		HVAPI_CloseEx(handle3);

		  handle3=HVAPI_OpenEx("172.18.106.12", NULL);

		if ( S_OK != HVAPI_ExecCmdEx(handle3 , "GetVersion;GetDevType;" , szRet3 , nRetLen3 , &nRetLen3) )
		{
			//WrightLogEx(pHHC->szIP , "SetOSD Enable HVAPI_ExecCmdEx Fail\n");
			//return E_FAIL;
		}

		HVAPI_CloseEx(handle3);

		  handle3=HVAPI_OpenEx("172.18.106.18", NULL);

		if ( S_OK != HVAPI_ExecCmdEx(handle3 , "GetVersion;GetDevType;" , szRet3 , nRetLen3 , &nRetLen3) )
		{
			//WrightLogEx(pHHC->szIP , "SetOSD Enable HVAPI_ExecCmdEx Fail\n");
			//return E_FAIL;
		}

		HVAPI_CloseEx(handle3);
	}
}



void CNewSDKTestDialog::OnBnClickedSetColor()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetColor(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	for (int i=0; i<2; i++)
	{
		HANDLE h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MuilThreadFunc, this, 0, NULL);
		if (NULL == h)
		{
			//return false;
		}
		CloseHandle(h);

		HANDLE h2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MuliThreadFunc2, this, 0, NULL);
		if (NULL == h2)
		{
			//return false;
		}
		CloseHandle(h2);

		HANDLE h3 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MuilThreadFunc3, this, 0, NULL);
		if (NULL == h3)
		{
			//return false;
		}
		CloseHandle(h3);
	}
}

void CNewSDKTestDialog::OnBnClickedButtonhvapiGetblackboxmessage()
{
	 //TODO: Add your control notification handler code here
	//黑匣子内部用接口测试
	//////////////if( NULL != m_hHandle )
	//////////////{
	//////////////	int nReportLen = 17*1024;
	//////////////	CHAR szReport[17*1024] = {0};
	//////////////	if ( S_OK != HVAPI_GetBlackBoxMessage( m_hHandle , szReport , &nReportLen ))
	//////////////	{
	//////////////		MessageBox("Getblackboxmessage Fail\n ");
	//////////////	}
	//////////////	else
	//////////////	{
	//////////////		/*MessageBox(szReport);
	//////////////		MessageBox("Getblackboxmessage OK\n ");*/
	//////////////		CFileDialog cFileDlg(FALSE);
	//////////////		if(cFileDlg.DoModal() == IDOK)
	//////////////	 {
	//////////////		 FILE* fp = fopen(cFileDlg.GetPathName(), "wb");
	//////////////		 if(fp)
	//////////////		 {
	//////////////			 fwrite(szReport, strlen(szReport), 1, fp);
	//////////////			 fclose(fp);
	//////////////		 }
	//////////////	 }

	//////////////	 CString strMsg;
	//////////////	 strMsg.Format("HVAPI_Getblackbox is OK. %d == %d ?\n\nAlready save to %s'", strlen(szReport), nReportLen,
	//////////////		 cFileDlg.GetPathName());
	//////////////	}

	//////////////}
	//////////////else
	//////////////{
	//////////////	MessageBox("NULL == m_hHandle \n ");
	//////////////}
}

////TODO(liyh) 写FPGA裸数据，相机部测试用 屏蔽
void CNewSDKTestDialog::OnBnClickedButtonWriteFpga()
{
//	// TODO: Add your control notification handler code here
//	GetDlgItem(IDC_STATIC_WRITE_RESULT1)->SetWindowText("");
//	GetDlgItem(IDC_STATIC_WRITE_RESULT2)->SetWindowText("");
//	GetDlgItem(IDC_STATIC_WRITE_RESULT3)->SetWindowText("");
//	GetDlgItem(IDC_STATIC_WRITE_RESULT4)->SetWindowText("");
//	GetDlgItem(IDC_STATIC_WRITE_RESULT5)->SetWindowText("");
//	GetDlgItem(IDC_STATIC_WRITE_RESULT6)->SetWindowText("");
//	GetDlgItem(IDC_STATIC_WRITE_RESULT7)->SetWindowText("");
//	if( NULL != m_hHandle )
//	{
//		char *stopstring, *stopstringValue;
//		int iRadixNum=16;
//
//		if (((CButton*)(GetDlgItem(IDC_CHECK_WRITE1)))->GetCheck())
//		{
//			CString strParamADD1;
//			GetDlgItem(IDC_EDIT_WRITE_ADD1)->GetWindowText(strParamADD1); 
//			CString strParamVALUE1 ;
//			GetDlgItem(IDC_EDIT_WRITE_VALUE1)->GetWindowText(strParamVALUE1);
//			if (S_OK != HVAPI_WriteFPGA(m_hHandle, strtol(strParamADD1, &stopstring, iRadixNum), strtol(strParamVALUE1, &stopstringValue, iRadixNum) ))
//			{
//				GetDlgItem(IDC_STATIC_WRITE_RESULT1)->SetWindowText("×");
//			}
//			else
//			{
//				GetDlgItem(IDC_STATIC_WRITE_RESULT1)->SetWindowText("√");
//			}
//		}
//
//		if (((CButton*)(GetDlgItem(IDC_CHECK_WRITE2)))->GetCheck())
//		{
//			CString strParamADD2;
//			GetDlgItem(IDC_EDIT_WRITE_ADD2)->GetWindowText(strParamADD2); 
//			CString strParamVALUE2 ;
//			GetDlgItem(IDC_EDIT_WRITE_VALUE2)->GetWindowText(strParamVALUE2);
//			if (S_OK != HVAPI_WriteFPGA(m_hHandle, strtol(strParamADD2, &stopstring, iRadixNum), strtol(strParamVALUE2, &stopstringValue, iRadixNum) ))
//			{
//				GetDlgItem(IDC_STATIC_WRITE_RESULT2)->SetWindowText("×");
//			}
//			else
//			{
//				GetDlgItem(IDC_STATIC_WRITE_RESULT2)->SetWindowText("√");
//			}
//		}
//
//		if (((CButton*)(GetDlgItem(IDC_CHECK_WRITE3)))->GetCheck())
//		{
//			CString strParamADD3;
//			GetDlgItem(IDC_EDIT_WRITE_ADD3)->GetWindowText(strParamADD3); 
//			CString strParamVALUE3 ;
//			GetDlgItem(IDC_EDIT_WRITE_VALUE3)->GetWindowText(strParamVALUE3);
//			if (S_OK != HVAPI_WriteFPGA(m_hHandle, strtol(strParamADD3, &stopstring, iRadixNum), strtol(strParamVALUE3, &stopstringValue, iRadixNum) ))
//			{
//				GetDlgItem(IDC_STATIC_WRITE_RESULT3)->SetWindowText("×");
//			}
//			else
//			{
//				GetDlgItem(IDC_STATIC_WRITE_RESULT3)->SetWindowText("√");
//			}
//		}
//
//		if (((CButton*)(GetDlgItem(IDC_CHECK_WRITE4)))->GetCheck())
//		{
//			CString strParamADD4;
//			GetDlgItem(IDC_EDIT_WRITE_ADD4)->GetWindowText(strParamADD4); 
//			CString strParamVALUE4 ;
//			GetDlgItem(IDC_EDIT_WRITE_VALUE4)->GetWindowText(strParamVALUE4);
//			if (S_OK != HVAPI_WriteFPGA(m_hHandle, strtol(strParamADD4, &stopstring, iRadixNum), strtol(strParamVALUE4, &stopstringValue, iRadixNum) ))
//			{
//				GetDlgItem(IDC_STATIC_WRITE_RESULT4)->SetWindowText("×");
//			}
//			else
//			{
//				GetDlgItem(IDC_STATIC_WRITE_RESULT4)->SetWindowText("√");
//			}
//		}
//
//		if (((CButton*)(GetDlgItem(IDC_CHECK_WRITE5)))->GetCheck())
//		{
//			CString strParamADD5;
//			GetDlgItem(IDC_EDIT_WRITE_ADD5)->GetWindowText(strParamADD5); 
//			CString strParamVALUE5 ;
//			GetDlgItem(IDC_EDIT_WRITE_VALUE5)->GetWindowText(strParamVALUE5);
//			if (S_OK != HVAPI_WriteFPGA(m_hHandle, strtol(strParamADD5, &stopstring, iRadixNum), strtol(strParamVALUE5, &stopstringValue, iRadixNum) ))
//			{
//				GetDlgItem(IDC_STATIC_WRITE_RESULT5)->SetWindowText("×");
//			}
//			else
//			{
//				GetDlgItem(IDC_STATIC_WRITE_RESULT5)->SetWindowText("√");
//			}
//		}
//
//		if (((CButton*)(GetDlgItem(IDC_CHECK_WRITE6)))->GetCheck())
//		{
//			CString strParamADD6;
//			GetDlgItem(IDC_EDIT_WRITE_ADD6)->GetWindowText(strParamADD6); 
//			CString strParamVALUE6 ;
//			GetDlgItem(IDC_EDIT_WRITE_VALUE6)->GetWindowText(strParamVALUE6);
//			if (S_OK != HVAPI_WriteFPGA(m_hHandle, strtol(strParamADD6, &stopstring, iRadixNum), strtol(strParamVALUE6, &stopstringValue, iRadixNum) ))
//			{
//				GetDlgItem(IDC_STATIC_WRITE_RESULT6)->SetWindowText("×");
//			}
//			else
//			{
//				GetDlgItem(IDC_STATIC_WRITE_RESULT6)->SetWindowText("√");
//			}
//		}
//
//		if (((CButton*)(GetDlgItem(IDC_CHECK_WRITE7)))->GetCheck())
//		{
//			CString strParamADD7;
//			GetDlgItem(IDC_EDIT_WRITE_ADD7)->GetWindowText(strParamADD7); 
//			CString strParamVALUE7 ;
//			GetDlgItem(IDC_EDIT_WRITE_VALUE7)->GetWindowText(strParamVALUE7);
//			if (S_OK != HVAPI_WriteFPGA(m_hHandle, strtol(strParamADD7, &stopstring, iRadixNum), strtol(strParamVALUE7, &stopstringValue, iRadixNum) ))
//			{
//				GetDlgItem(IDC_STATIC_WRITE_RESULT7)->SetWindowText("×");
//			}
//			else
//			{
//				GetDlgItem(IDC_STATIC_WRITE_RESULT7)->SetWindowText("√");
//			}
//		}
//
//
//	}
//	else
//	{
//		MessageBox("NULL == m_hHandle \n ");
//	}
//
}
//
////TODO(liyh) 读FPGA裸数据，相机部测试用  屏蔽
void CNewSDKTestDialog::OnBnClickedButtonReadFpga()
{
//	// TODO: Add your control notification handler code here
//	GetDlgItem(IDC_STATIC_READ_RESULT1)->SetWindowText("");
//	GetDlgItem(IDC_STATIC_READ_RESULT2)->SetWindowText("");
//	GetDlgItem(IDC_STATIC_READ_RESULT3)->SetWindowText("");
//	GetDlgItem(IDC_STATIC_READ_RESULT4)->SetWindowText("");
//	GetDlgItem(IDC_STATIC_READ_RESULT5)->SetWindowText("");
//	GetDlgItem(IDC_STATIC_READ_RESULT6)->SetWindowText("");
//	GetDlgItem(IDC_STATIC_READ_RESULT7)->SetWindowText("");
//
//	if( NULL != m_hHandle )
//	{
//		char *stopstring;
//		int iRadixNum=16;
//
//		if (((CButton*)(GetDlgItem(IDC_CHECK_READ1)))->GetCheck())
//		{
//			CString strParamADD1;
//			GetDlgItem(IDC_EDIT_READ_ADD1)->GetWindowText(strParamADD1); 
//			INT nValue1;
//			if (S_OK != HVAPI_ReadFPGA(m_hHandle, strtol(strParamADD1, &stopstring, iRadixNum), &nValue1 ))
//			{
//				GetDlgItem(IDC_STATIC_READ_RESULT1)->SetWindowText("×");
//			}
//			else
//			{
//
//				CString strParamVALUE1;
//				strParamVALUE1.Format("%0x",nValue1) ;
//				GetDlgItem(IDC_EDIT_READ_VALUE1)->SetWindowText(strParamVALUE1);
//				GetDlgItem(IDC_STATIC_READ_RESULT1)->SetWindowText("√");
//			}
//		}
//
//		if (((CButton*)(GetDlgItem(IDC_CHECK_READ2)))->GetCheck())
//		{
//			CString strParamADD2;
//			GetDlgItem(IDC_EDIT_READ_ADD2)->GetWindowText(strParamADD2); 
//			INT nValue2;
//			if (S_OK != HVAPI_ReadFPGA(m_hHandle, strtol(strParamADD2, &stopstring, iRadixNum), &nValue2 ))
//			{
//				GetDlgItem(IDC_STATIC_READ_RESULT2)->SetWindowText("×");
//			}
//			else
//			{
//
//				CString strParamVALUE2;
//				strParamVALUE2.Format("%0x",nValue2) ;
//				GetDlgItem(IDC_EDIT_READ_VALUE2)->SetWindowText(strParamVALUE2);
//				GetDlgItem(IDC_STATIC_READ_RESULT2)->SetWindowText("√");
//			}
//		}
//
//
//		if (((CButton*)(GetDlgItem(IDC_CHECK_READ3)))->GetCheck())
//		{
//			CString strParamADD3;
//			GetDlgItem(IDC_EDIT_READ_ADD3)->GetWindowText(strParamADD3); 
//			INT nValue3;
//			if (S_OK != HVAPI_ReadFPGA(m_hHandle, strtol(strParamADD3, &stopstring, iRadixNum), &nValue3 ))
//			{
//				GetDlgItem(IDC_STATIC_READ_RESULT3)->SetWindowText("×");
//			}
//			else
//			{
//	
//				CString strParamVALUE3;
//				strParamVALUE3.Format("%0x",nValue3) ;
//				GetDlgItem(IDC_EDIT_READ_VALUE3)->SetWindowText(strParamVALUE3);
//				GetDlgItem(IDC_STATIC_READ_RESULT3)->SetWindowText("√");
//			}
//		}
//
//		if (((CButton*)(GetDlgItem(IDC_CHECK_READ4)))->GetCheck())
//		{
//			CString strParamADD4;
//			GetDlgItem(IDC_EDIT_READ_ADD4)->GetWindowText(strParamADD4); 
//			INT nValue4;
//			if (S_OK != HVAPI_ReadFPGA(m_hHandle, strtol(strParamADD4, &stopstring, iRadixNum), &nValue4 ))
//			{
//				GetDlgItem(IDC_STATIC_READ_RESULT4)->SetWindowText("×");
//			}
//			else
//			{
//	
//				CString strParamVALUE4;
//				strParamVALUE4.Format("%0x",nValue4) ;
//				GetDlgItem(IDC_EDIT_READ_VALUE4)->SetWindowText(strParamVALUE4);
//				GetDlgItem(IDC_STATIC_READ_RESULT4)->SetWindowText("√");
//			}
//		}
//
//		if (((CButton*)(GetDlgItem(IDC_CHECK_READ5)))->GetCheck())
//		{
//			CString strParamADD5;
//			GetDlgItem(IDC_EDIT_READ_ADD5)->GetWindowText(strParamADD5); 
//			INT nValue5;
//			if (S_OK != HVAPI_ReadFPGA(m_hHandle, strtol(strParamADD5, &stopstring, iRadixNum), &nValue5 ))
//			{
//				GetDlgItem(IDC_STATIC_READ_RESULT5)->SetWindowText("×");
//			}
//			else
//			{
//			
//				CString strParamVALUE5;
//				strParamVALUE5.Format("%0x",nValue5) ;
//				GetDlgItem(IDC_EDIT_READ_VALUE5)->SetWindowText(strParamVALUE5);
//				GetDlgItem(IDC_STATIC_READ_RESULT5)->SetWindowText("√");
//			}
//		}
//
//		if (((CButton*)(GetDlgItem(IDC_CHECK_READ6)))->GetCheck())
//		{
//			CString strParamADD6;
//			GetDlgItem(IDC_EDIT_READ_ADD6)->GetWindowText(strParamADD6); 
//			INT nValue6;
//			if (S_OK != HVAPI_ReadFPGA(m_hHandle, strtol(strParamADD6, &stopstring, iRadixNum), &nValue6 ))
//			{
//				GetDlgItem(IDC_STATIC_READ_RESULT6)->SetWindowText("×");
//			}
//			else
//			{
//		
//				CString strParamVALUE6;
//				strParamVALUE6.Format("%0x",nValue6) ;
//				GetDlgItem(IDC_EDIT_READ_VALUE6)->SetWindowText(strParamVALUE6);
//				GetDlgItem(IDC_STATIC_READ_RESULT6)->SetWindowText("√");
//			}
//		}
//
//		if (((CButton*)(GetDlgItem(IDC_CHECK_READ7)))->GetCheck())
//		{
//			CString strParamADD7;
//			GetDlgItem(IDC_EDIT_READ_ADD7)->GetWindowText(strParamADD7); 
//			INT nValue7;
//			if (S_OK != HVAPI_ReadFPGA(m_hHandle, strtol(strParamADD7, &stopstring, iRadixNum), &nValue7 ))
//			{
//				GetDlgItem(IDC_STATIC_READ_RESULT7)->SetWindowText("×");
//			}
//			else
//			{
//			
//				CString strParamVALUE7;
//				strParamVALUE7.Format("%0x",nValue7) ;
//				GetDlgItem(IDC_EDIT_READ_VALUE7)->SetWindowText(strParamVALUE7);
//				GetDlgItem(IDC_STATIC_READ_RESULT7)->SetWindowText("√");
//			}
//		}
//
//
//
//
//
//	}
//	else
//	{
//		MessageBox("NULL == m_hHandle \n ");
//	}
}


void CNewSDKTestDialog::OnBnClickedButtonhvapiSetacsync()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);

		if ( S_OK != HVAPI_SetACSync(m_hHandle , atoi(strParam2), atoi(strParam3) ) )
		{
			MessageBox("HVAPI_SetACSync Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetACSync OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	 CDevBasicInfo basicInfo[1];
	 int icount=1;
	 HVAPI_SearchDeviceEx(basicInfo, &icount);

}

void CNewSDKTestDialog::OnBnClickedButtonhvapiSetimageenhancementenable()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		if ( S_OK != HVAPI_SetImageEnhancementEnable(m_hHandle , atoi(strParam2) ) )
		{
			MessageBox("HVAPI_SetImageEnhancementEnable Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetImageEnhancementEnable OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}

}




void CNewSDKTestDialog::OnBnClickedSet2ddenoise()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_Set2DDeNoiseEnable(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSet2ddenoisevalue()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_Set2DDeNoiseStrength(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}
void CNewSDKTestDialog::OnBnClickedButtonhvapiSetgammaenable()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetGammaEnable(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonhvapiSetgammavalue()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetGammaStrength(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonsetagczone()
{
// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		
		INT rgZone[16];
		INT nZoneCount = 16;
		
		if ( S_OK != GetAGCZone(strParam2 , rgZone , &nZoneCount))
		{
			MessageBox("GetAGCZone Fail\n ");
			return ;
		}
		HRESULT hr = HVAPI_SetAGCZone(m_hHandle , rgZone ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}

}

void CNewSDKTestDialog::OnBnClickedButtonhvapiGetagczone()
{
		// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		INT rgZone[16];
		INT nZoneCount = 16;
		char szAGCZone[512] = {0};
		if ( S_OK != HVAPI_GetAGCZone(m_hHandle , rgZone ) )
		{
			MessageBox("HVAPI_SetAGCZone Fail\n ");
		}
		else
		{
			char szTemp[128] = {0};
			for (int i = 0 ; i < 16 ; i++ )
			{
				sprintf( szTemp , " %d ",rgZone[i]);
				strcat( szAGCZone ,  szTemp);
			}
			MessageBox(szAGCZone);
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonhvapiSetenredlightthreshold()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		if ( S_OK != HVAPI_SetEnRedLightThreshold(m_hHandle , atoi(strParam2) ) )
		{
			MessageBox("HVAPI_SetEnRedLightThreshold Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetEnRedLightThreshold OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}

}




void CNewSDKTestDialog::OnBnClickedButtonhvapiGetcamerabasicinfo()
{
	// TODO: Add your control notification handler code here
	//if( NULL != m_hHandle )
	//{
	//	CCameraBasicInfo cBasicInfo;
	//	if ( S_OK != HVAPI_GetCameraBasicInfo( m_hHandle , &cBasicInfo ) )
	//	{
	//		MessageBox("HVAPI_GetCameraBasicInfo Fail\n ");
	//	}
	//	else
	//	{
	//		char szDebug[1024];
	//		sprintf( szDebug , 
	//			"cBasicInfo: \n"
	//			"AGC:%d: GainMax:%d ,Min:%d ,BaseLine:%d ,ShuterMax:%d,Min:%d\n"
	//			"AWB:%d \n"
	//			"Color:%d, 2DDeNoiseEnable:%d,2DDeNoiseValue:%d\n"
	//			"WDR:%d, %d\n"
	//			"Brightness:%d.nContrast:%d, nSharpness:%d,Saturation:%d\n"
	//			"DeNoise: SNF:%d,TNF:%d , Level:%d,Mode:%d\n"
	//			"Manual :shutter:%d,gain:%d \nR:%d,G:%d,B:%d\n"
	//			"FilterMode:%d,DCEnable:%d \n"
	//			"Gamma:Enable:%d,Strength:%d\n"

}



void CNewSDKTestDialog::OnBnClickedButtonhvapiSetctrlcplenable()
{
	// TODO: Add your control notification handler code here
	
		/*if( NULL != m_hHandle )
		{
			CString strParam2 ;
			m_Param2Edit.GetWindowText(strParam2);

			if ( S_OK != HVAPI_SetCtrlCplEnable(m_hHandle , atoi(strParam2) ) )
			{
				MessageBox("HVAPI_SetCtrlCplEnable Fail\n ");
			}
			else
			{
				MessageBox("HVAPI_SetCtrlCplEnable OK\n ");
			}
		}
		else
		{
			MessageBox("NULL == m_hHandle \n ");
		}*/
}




void CNewSDKTestDialog::OnBnClickedButtonhvapiGettracerank()
{

}


void CNewSDKTestDialog::OnBnClickedButtongetconnstatusex()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		char *stopstring;
		m_Param2Edit.GetWindowText(strParam2);
		DWORD dwState;

		if ( S_OK != HVAPI_GetConnStatusEx(m_hHandle , strtoul(strParam2, &stopstring, 16)  ,&dwState  ) )
		{
			MessageBox("HVAPI_GetConnStatusEx Fail\n ");
		}
		else
		{
			char szShowMsg[128];
			sprintf(szShowMsg, "HVAPI_GetConnStatusEx OK\n %x", dwState);
			MessageBox(szShowMsg);
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}

}

void CNewSDKTestDialog::OnBnClickedButtongetdevtypeex()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		int nDevType = 0;
		//HVAPI_HANDLE_CONTEXT_EX* pHHC = (HVAPI_HANDLE_CONTEXT_EX*) m_hHandle;
		if ( S_OK != HVAPI_GetDevTypeEx(strParam2.GetBuffer() , &nDevType ) )
		{
			MessageBox("HVAPI_GetDevTypeEx Fail\n ");
		}
		else
		{
			char szTemp[128] = {0};
			sprintf( szTemp , "Devtype: %d ",nDevType);
			MessageBox(szTemp);
		}
		strParam2.ReleaseBuffer();
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}




unsigned long long   htonll(unsigned long long   val)  
{  

	return (((((unsigned long long)htonl((int)((val << 32) >> 32))) << 32) | (unsigned int)htonl(((int)(val >> 32)))) >> 16);
}

void CNewSDKTestDialog::OnBnClickedButtonsetipbymacaddr()
{
	// TODO: Add your control notification handler code here


		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);
		CString strParam4 ;
		m_Param4Edit.GetWindowText(strParam4);
		CString strParam5 ;
		m_Param5Edit.GetWindowText(strParam5);


		DWORD64 dw64MAC=0;
		strParam2.Replace(":", "");
		strParam2.Replace("-", "");
		sscanf( strParam2.GetBuffer(), "%I64X", &dw64MAC );
		if ( S_OK != HVAPI_SetIPByMacAddr(htonll(dw64MAC) 
			, htonl( inet_addr(strParam3.GetBuffer()) )
			, htonl(  inet_addr(strParam4.GetBuffer()) )
			, htonl(  inet_addr(strParam5.GetBuffer()) )
			) )
		{
			MessageBox("HVAPI_SetIPByMacAddr Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetIPByMacAddr OK\n ");
		}

		strParam2.ReleaseBuffer();
		strParam3.ReleaseBuffer();
		strParam4.ReleaseBuffer();
		strParam5.ReleaseBuffer();

}


void CNewSDKTestDialog::OnBnClickedButtonhvapiSetdenoisetnfenable()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		if ( S_OK != HVAPI_SetDeNoiseTNFEnable(m_hHandle , atoi(strParam2)) )
		{
			MessageBox("HVAPI_SetDeNoiseTNFEnable Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetDeNoiseTNFEnable OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonhvapiSetdenoisesnfenable()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		if ( S_OK != HVAPI_SetDeNoiseSNFEnable(m_hHandle , atoi(strParam2)) )
		{
			MessageBox("HVAPI_SetDeNoiseSNFEnable Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetDeNoiseSNFEnable OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}


void CNewSDKTestDialog::OnBnClickedButtonsetedgeenable()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		if ( S_OK != HVAPI_SetEdgeEnhance(m_hHandle , atoi(strParam2)) )
		{
			MessageBox("HVAPI_SetEdgeEnhance Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetEdgeEnhance OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}






void CNewSDKTestDialog::OnBnClickedButtonSetautojpegcompressparam()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);
		CString strParam4 ;
		m_Param4Edit.GetWindowText(strParam4);


		HRESULT hr = HVAPI_SetAutoJpegCompressParam(m_hHandle , atoi(strParam2) , atoi(strParam3), atoi(strParam4) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtongethddstatus()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		int nReportLen = 1*1024;
		CHAR szReport[1*1024] = {0};
		if ( S_OK != HVAPI_GetHDDStatus( m_hHandle , szReport , &nReportLen ))
		{
			MessageBox("HDDStatus Fail\n ");
		}
		else
		{
			MessageBox(szReport);
			MessageBox("HDDStatus OK\n ");
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}


void CNewSDKTestDialog::OnBnClickedButtongetrunstatusstring()
{
	if( NULL != m_hHandle )
	{
		int nReportLen = 1*1024;
		CHAR szReport[1*1024] = {0};
		if ( S_OK != HVAPI_GetRunStatusString( m_hHandle , szReport , &nReportLen ))
		{
			MessageBox("GetRunStatus Fail\n ");
		}
		else
		{
			MessageBox(szReport);
			MessageBox("GetRunStatus OK\n ");
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}




void CNewSDKTestDialog::OnBnClickedButtongetcameraworkstate()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		int nReportLen = 1*1024;
		CHAR szReport[1*1024] = {0};
		if ( S_OK != HVAPI_GetCameraWorkState( m_hHandle , szReport , &nReportLen ))
		{
			MessageBox("GetRunStatus Fail\n ");
		}
		else
		{
			MessageBox(szReport);
			MessageBox("HVAPI_GetCameraWorkState OK\n ");
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonsettgio()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		if ( S_OK != HVAPI_SetTGIO(m_hHandle , atoi(strParam2)) )
		{
			MessageBox("HVAPI_SetTGIO Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetTGIO OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonsetf1io()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);

		if ( S_OK != HVAPI_SetF1IO(m_hHandle , atoi(strParam2) , atoi(strParam3)) )
		{
			MessageBox("HVAPI_SetF1IO Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetF1IO OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonsetexpio()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);

		if ( S_OK != HVAPI_SetEXPIO(m_hHandle , atoi(strParam2) , atoi(strParam3)) )
		{
			MessageBox("HVAPI_SetEXPIO Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetEXPIO OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonsetalmio()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);

		if ( S_OK != HVAPI_SetALMIO(m_hHandle , atoi(strParam2) , atoi(strParam3)) )
		{
			MessageBox("HVAPI_SetALMIO Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetALMIO OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtongettgio()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		int nValue = 0;
		if ( S_OK != HVAPI_GetTGIO(m_hHandle , &nValue ) )
		{
			MessageBox("HVAPI_GetResetCount Fail\n ");
		}
		else
		{
			char szDebug[256];
			sprintf( szDebug , "tgio: %d\n"
				,nValue
				);
			MessageBox( szDebug );
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtongetf1io()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		int nValue = 0;
		int nValueType=0;
		if ( S_OK != HVAPI_GetF1IO(m_hHandle , &nValue, &nValueType ) )
		{
			MessageBox("HVAPI_GetF1IO Fail\n ");
		}
		else
		{
			char szDebug[256];
			sprintf( szDebug , "F1IO: %d   %d\n"
				,nValue, nValueType
				);
			MessageBox( szDebug );
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtongetexpio()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		int nValue = 0;
		int nValueType=0;
		if ( S_OK != HVAPI_GetEXPIO(m_hHandle , &nValue, &nValueType ) )
		{
			MessageBox("HVAPI_GetEXPIO Fail\n ");
		}
		else
		{
			char szDebug[256];
			sprintf( szDebug , "EXPIO: %d   %d\n"
				,nValue, nValueType
				);
			MessageBox( szDebug );
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtongetalmio()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		int nValue = 0;
		int nValueType=0;
		if ( S_OK != HVAPI_GetALMIO(m_hHandle , &nValue, &nValueType ) )
		{
			MessageBox("HVAPI_GetALMIO Fail\n ");
		}
		else
		{
			char szDebug[256];
			sprintf( szDebug , "ALMIO: %d   %d\n"
				,nValue, nValueType
				);
			MessageBox( szDebug );
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}

}

void CNewSDKTestDialog::OnBnClickedButtonsetcolorgradation()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		if ( S_OK != HVAPI_SetColorGradation(m_hHandle , atoi(strParam2)) )
		{
			MessageBox("HVAPI_SetColorGradation Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetColorGradation OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}

}



		CString IPToString( const unsigned long ulIP )
		{
			in_addr ia;
			ia.s_addr = ulIP;
			return CString(inet_ntoa(ia));
		}



void CNewSDKTestDialog::OnBnClickedButtonsetvediorequestcontrol()
{
	// TODO: Add your control notification handler code here
	//if( NULL != m_hHandle )
	//{
	//	CString strParam2 ;
	//	m_Param2Edit.GetWindowText(strParam2);

	//	CString strParam3 ;
	//	m_Param3Edit.GetWindowText(strParam3);

	//	CString strParam4 ;
	//	m_Param4Edit.GetWindowText(strParam4);

	//	if ( S_OK != HVAPI_SetVedioRequestControl(m_hHandle , atoi(strParam2),  ( inet_addr(strParam3.GetBuffer()) ), atoi(strParam4) )             )
	//	{
	//		MessageBox("HVAPI_SetVedioRequestControl Fail\n ");
	//	}
	//	else
	//	{
	//		MessageBox("HVAPI_SetVedioRequestControl OK\n ");
	//	}
	//	

	//	CString str =  IPToString( ( inet_addr(strParam3.GetBuffer()) ));

	//	int i = inet_addr(strParam3.GetBuffer()) ;
	//	i=0;

	//}
	//else
	//{
	//	MessageBox("NULL == m_hHandle \n ");
	//}
}

void CNewSDKTestDialog::OnBnClickedButtonhvapiSetaescene()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		if ( S_OK != HVAPI_SetAEScene(m_hHandle , atoi(strParam2)) )
		{
			MessageBox("HVAPI_SetAEScene Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetAEScene OK\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonhvapiGetaescene()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		int iValue=0;

		if ( S_OK != HVAPI_GetAEScene(m_hHandle , &iValue) )
		{
			MessageBox("HVAPI_GetAEScene Fail\n ");
		}
		else
		{
			char sz[64];
			sprintf(sz, "%d", iValue);
			MessageBox(sz);
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonsetdevname()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetCustomizedDevName( m_hHandle , strParam2.GetBuffer()  );
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}

		strParam2.ReleaseBuffer();
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}

}

void CNewSDKTestDialog::OnBnClickedButtongetdevname()
{
	// TODO: Add your control notification handler code here
		if( NULL != m_hHandle )
	{
		int nReportLen = 1*1024;
		CHAR szReport[1*1024] = {0};
		if ( S_OK != HVAPI_GetCustomizedDevName( m_hHandle , szReport , &nReportLen ))
		{
			MessageBox("GetCustomizedDevName Fail\n ");
		}
		else
		{
			MessageBox(szReport);
			MessageBox("HVAPI_GetCustomizedDevName OK\n ");
		}

	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonseth264secondbitrate()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);
/*
		if ( S_OK != HVAPI_SetH264SecondBitRate(m_hHandle , atoi(strParam2)) )
		{
			MessageBox("HVAPI_SetH264SecondBitRate Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SetH264SecondBitRate OK\n ");
		}*/
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtongeth264secondbitrate()
{
	// TODO: Add your control notification handler code here
		if( NULL != m_hHandle )
	{
		int iValue=0;

		if (true /*S_OK != HVAPI_GetH264SecondBitRate(m_hHandle , &iValue)*/ )
		{
			MessageBox("HVAPI_GetH264SecondBitRate Fail\n ");
		}
		else
		{
			char sz[64];
			sprintf(sz, "%d", iValue);
			MessageBox(sz);
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonsetdenoiseswitch()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetDeNoiseMode(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedButtonhvapiSofttriggercapture()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
/*
		if ( S_OK != HVAPI_SoftTriggerCapture(m_hHandle   ) )
		{
			MessageBox("HVAPI_SoftTriggerCapture Fail\n ");
		}
		else
		{
			MessageBox("HVAPI_SoftTriggerCapture OK\n ");
		}*/
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetsharpnessenable()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetSharpnessEnable(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSeth264bitratecontrol()
{
	// TODO: Add your control notification handler code here
	if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		HRESULT hr = HVAPI_SetH264BitRateControl(m_hHandle , atoi(strParam2) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}

void CNewSDKTestDialog::OnBnClickedSetstreamfps()
{
	// TODO: Add your control notification handler code here
		if( NULL != m_hHandle )
	{
		CString strParam2 ;
		m_Param2Edit.GetWindowText(strParam2);

		CString strParam3 ;
		m_Param3Edit.GetWindowText(strParam3);

		HRESULT hr = HVAPI_SetStreamFps(m_hHandle , atoi(strParam2), atoi(strParam3) ) ;
		if ( S_OK == hr )
		{
			MessageBox(" OK\n ");

		}
		else if ( S_FALSE == hr )
		{
			MessageBox(" InPUT INVAIL PARAM \n ");
		}
		else 
		{
			MessageBox(" Fail\n ");
		}
	}
	else
	{
		MessageBox("NULL == m_hHandle \n ");
	}
}


void CNewSDKTestDialog::OnBnClickedButtonsetfacedataadv()
{
	// TODO: Add your control notification handler code here
	CString strParam2 ;
	m_Param2Edit.GetWindowText(strParam2);
		char szFileFilter[] = "XML文件|*.xml|所有文件|*.*|";
	CString strSelectedFile;
	CFileDialog FileDialogBox(
		TRUE, NULL, "",
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		szFileFilter, this
		);
	if ( FileDialogBox.DoModal() == IDOK )
	{
		strSelectedFile = FileDialogBox.GetPathName();
		char* pszXmlParam;
		unsigned long ulFileSize = 0; 
		unsigned long ulReadedSize = 0;
		FILE *fp = fopen(strSelectedFile.GetBuffer(0), "rb");  
		if ( fp )  
		{  
			fseek(fp, 0, SEEK_END);  
			ulFileSize = ftell(fp);
			pszXmlParam = new char[ulFileSize];
			fseek(fp, 0, SEEK_SET);
			ulReadedSize = (unsigned long)fread(pszXmlParam, 1, ulFileSize, fp);  
			fclose(fp); 
			if(ulReadedSize != ulFileSize)
			{
				return; 
			}
		}  

		if ( pszXmlParam )
		{
			if ( S_OK == HVAPI_SetFaceDataAdv(m_hHandle, atoi(strParam2),pszXmlParam, ulFileSize) )
			{
				AfxMessageBox("HVAPI_SetXmlParam is OK");
			}
			else
			{
				AfxMessageBox("ERROR!");
			}
		}
		delete[] pszXmlParam;
	}
}
