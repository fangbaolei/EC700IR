// HvDeviceAxCtrl.cpp : Implementation of the CHvDeviceAxCtrl ActiveX Control class.

#include "stdafx.h"
//#include "Dbghelp.h"
#include "HvDeviceAx.h"
#include "HvDeviceAxCtrl.h"
#include "HvDeviceAxPropPage.h"
#include "ToolsFunction.h"
#include "shlwapi.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif






void WriteLog(DWORD len)
{
	//if (szLog == NULL)
	//	return ;

	CString strPath = _T("D:\\AXlog\\");
	CTime tm = CTime::GetCurrentTime();
	MakeSureDirectoryPathExists(strPath.GetBuffer());
	strPath.ReleaseBuffer();
	CString szFileName;
	szFileName.Format("%s%s.log", strPath, tm.Format("%Y%m%d"));

	FILE *myfile = fopen(szFileName, "a+");
	if (myfile)
	{
		CTime tm = CTime::GetCurrentTime();
		fprintf(myfile, "%s : %i\n", tm.Format("%Y-%m-%d %H:%M:%S"), len);
		fclose(myfile);
	}

	return ;
}



extern CHvDeviceAxApp NEAR theApp;




IMPLEMENT_DYNCREATE(CHvDeviceAxCtrl, COleControl)



	// Message map

	BEGIN_MESSAGE_MAP(CHvDeviceAxCtrl, COleControl)
		ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
		ON_WM_CREATE()
		//ON_WM_LBUTTONDBLCLK()
		ON_WM_LBUTTONDBLCLK()
		//ON_MESSAGE(WM_DESTROY_VIDEO,DestroyMaxVideoWnd)
	END_MESSAGE_MAP()



	// Dispatch map

	BEGIN_DISPATCH_MAP(CHvDeviceAxCtrl, COleControl)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
		DISP_PROPERTY_NOTIFY_ID(CHvDeviceAxCtrl, "RecvH264Flag", dispidRecvH264Flag, m_RecvH264Flag, OnRecvH264FlagChanged, VT_I4)
		DISP_PROPERTY_NOTIFY_ID(CHvDeviceAxCtrl, "RecvJpegFlag", dispidRecvJpegFlag, m_RecvJpegFlag, OnRecvJpegFlagChanged, VT_I4)
		DISP_PROPERTY_NOTIFY_ID(CHvDeviceAxCtrl, "RecvPlateBinImageFlag", dispidRecvPlateBinImageFlag, m_RecvPlateBinImageFlag, OnRecvPlateBinImageFlagChanged, VT_I4)
		DISP_PROPERTY_NOTIFY_ID(CHvDeviceAxCtrl, "RecvPlateImageFlag", dispidRecvPlateImageFlag, m_RecvPlateImageFlag, OnRecvPlateImageFlagChanged, VT_I4)
		DISP_PROPERTY_NOTIFY_ID(CHvDeviceAxCtrl, "RecvSnapImageFlag", dispidRecvSnapImageFlag, m_RecvSnapImageFlag, OnRecvSnapImageFlagChanged, VT_I4)
		DISP_PROPERTY_NOTIFY_ID(CHvDeviceAxCtrl, "SaveH264VideoTime", dispidSaveH264VideoTime, m_SaveH264VideoTime, OnSaveH264VideoTimeChanged, VT_I4)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "ConnectTo", dispidConnectTo, ConnectTo, VT_EMPTY, VTS_BSTR)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "GetCarID", dispidGetCarID, GetCarID, VT_I4, VTS_NONE)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "GetPlate", dispidGetPlate, GetPlate, VT_BSTR, VTS_NONE)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "GetPlateColor", dispidGetPlateColor, GetPlateColor, VT_BSTR, VTS_NONE)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "GetPlateInfo", dispidGetPlateInfo, GetPlateInfo, VT_BSTR, VTS_NONE)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "GetStatus", dispidGetStatus, GetStatus, VT_I4, VTS_I2)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "SaveH264Video", dispidSaveH264Video, SaveH264Video, VT_I4, VTS_BSTR)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "SaveJpegVideo", dispidSaveJpegVideo, SaveJpegVideo, VT_I4, VTS_BSTR)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "SavePlateBin2BMP", dispidSavePlateBin2BMP, SavePlateBin2BMP, VT_I4, VTS_BSTR)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "SavePlateBinImage", dispidSavePlateBinImage, SavePlateBinImage, VT_I4, VTS_BSTR)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "SavePlateImage", dispidSavePlateImage, SavePlateImage, VT_I4, VTS_BSTR)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "SaveSnapImage", dispidSaveSnapImage, SaveSnapImage, VT_I4, VTS_BSTR VTS_I2)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "Disconnect", dispidDisconnect, Disconnect, VT_EMPTY, VTS_NONE)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "GetPlateResultTime", dispidGetPlateResultTime, GetPlateResultTime, VT_I8, VTS_NONE)
		DISP_PROPERTY_NOTIFY_ID(CHvDeviceAxCtrl, "RecvRecordFlag", dispidRecvRecordFlag, m_RecvRecordFlag, OnRecvRecordFlagChanged, VT_I4)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "GetFileName", dispidGetFileName, GetFileName, VT_BSTR, VTS_I2)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "SetPathType", dispidSetPathType, SetPathType, VT_I4, VTS_I2)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "SoftTriggerCaptureAndForceGetResult", dispidSoftTriggerCaptureAndForceGetResult, SoftTriggerCaptureAndForceGetResult, VT_I4, VTS_NONE)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "SendTriggerOut", dispidSendTriggerOut, SendTriggerOut, VT_I4, VTS_NONE)
		DISP_PROPERTY_NOTIFY_ID(CHvDeviceAxCtrl, "H264FrameRateNum", dispidH264FrameRateNum, m_H264FrameRateNum, OnH264FrameRateNumChanged, VT_I4)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "GetH264FrameRateNum", dispidGetH264FrameRateNum, GetH264FrameRateNum, VT_I4, VTS_NONE)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "SetH264FrameRateNum", dispidSetH264FrameRateNum, SetH264FrameRateNum, VT_I4, VTS_I4)
		DISP_PROPERTY_NOTIFY_ID(CHvDeviceAxCtrl, "ShowH264VideoFlag", dispidShowH264VideoFlag, m_ShowH264VideoFlag, OnShowH264VideoFlagChanged, VT_I4)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "InportNameListEx", dispidInportNameListEx, InportNameListEx, VT_I4, VTS_BSTR VTS_I4 VTS_BSTR VTS_I4)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "GetNameListEx", dispidGetNameListEx, GetNameListEx, VT_BSTR, VTS_NONE)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "InportNameList", dispidInportNameList, InportNameList, VT_I4, VTS_PI1 VTS_I4 VTS_PI1 VTS_I4)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "GetNameList", dispidGetNameList, GetNameList, VT_I4, VTS_PI1 VTS_PI4 VTS_PI1 VTS_PI4)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "TriggerSignal", dispidTriggerSignal, TriggerSignal, VT_I4, VTS_I4)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "TriggerAlarmSignal", dispidTriggerAlarmSignal, TriggerAlarmSignal, VT_I4, VTS_NONE)
		DISP_PROPERTY_NOTIFY_ID(CHvDeviceAxCtrl, "RecvIllegalVideoFlag", dispidRecvIllegalVideoFlag, m_RecvIllegalVideoFlag, OnRecvIllegalVideoFlagChanged, VT_I4)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "SaveIllegalVideo", dispidSaveIllegalVideo, SaveIllegalVideo, VT_I4, VTS_BSTR)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "GetIllegalReason", dispidGetIllegalReason, GetIllegalReason, VT_BSTR, VTS_NONE)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "GetCaptureImage", dispidGetCaptureImage, GetCaptureImage, VT_I4, VTS_I4 VTS_BSTR)
		DISP_PROPERTY_NOTIFY_ID(CHvDeviceAxCtrl, "HideIcon", dispidHideIcon, m_HideIcon, OnHideIconChanged, VT_I4)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "GetDevSN", dispidGetDevSN, GetDevSN, VT_BSTR, VTS_NONE)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "GetDevMac", dispidGetDevMac, GetDevMac, VT_BSTR, VTS_NONE)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "GetCusTomInfo", dispidGetCusTomInfo, GetCusTomInfo, VT_BSTR, VTS_NONE)
		DISP_FUNCTION_ID(CHvDeviceAxCtrl, "SetCusTomInfo", dispidSetCusTomInfo, SetCusTomInfo, VT_I4, VTS_BSTR)
	END_DISPATCH_MAP()

	//HRESULT CHvDeviceAxCtrl::GetCaptureImage(int iVideoID, LPCTSTR strFileName)


	// Event map

	BEGIN_EVENT_MAP(CHvDeviceAxCtrl, COleControl)
		EVENT_CUSTOM_ID("OnReceivePlate", eventidOnReceivePlate, OnReceivePlate, VTS_NONE)
		EVENT_CUSTOM_ID("OnReceiveJpegVideo", eventidOnReceiveJpegVideo, OnReceiveJpegVideo, VTS_NONE)
		EVENT_CUSTOM_ID("OnReceiveH264Video", eventidOnReceiveH264Video, OnReceiveH264Video, VTS_NONE)
		EVENT_STOCK_DBLCLICK()
		EVENT_STOCK_CLICK()
	END_EVENT_MAP()



	// Property pages

	// TODO: Add more property pages as needed.  Remember to increase the count!
	BEGIN_PROPPAGEIDS(CHvDeviceAxCtrl, 1)
		PROPPAGEID(CHvDeviceAxPropPage::guid)
	END_PROPPAGEIDS(CHvDeviceAxCtrl)

	/*
	//去掉安全警告 BEGIN    xurui
BEGIN_INTERFACE_MAP(CHvDeviceAxCtrl, COleControl)  
    INTERFACE_PART(CHvDeviceAxCtrl, IID_IObjectSafety, ObjectSafety)  
END_INTERFACE_MAP()  */




	// Initialize class factory and guid

	IMPLEMENT_OLECREATE_EX(CHvDeviceAxCtrl, "HVDEVICEAX.HvDeviceAxCtrl.1",
		0x7f3e4c52, 0x224d, 0x415a, 0x95, 0x68, 0xfd, 0x4b, 0xb3, 0x1, 0x67, 0xcf)



		// Type library ID and version

		IMPLEMENT_OLETYPELIB(CHvDeviceAxCtrl, _tlid, _wVerMajor, _wVerMinor)



		// Interface IDs

		const IID BASED_CODE IID_DHvDeviceAx =
	{ 0xFBAD7529, 0x8CC6, 0x49B8, { 0x84, 0x11, 0xB6, 0x5A, 0xC8, 0xD9, 0x85, 0x74 } };
	const IID BASED_CODE IID_DHvDeviceAxEvents =
	{ 0xE6101EF2, 0xEBDE, 0x47C7, { 0xA8, 0xC0, 0x22, 0xD8, 0x12, 0x43, 0xBD, 0x64 } };



	// Control type information

	static const DWORD BASED_CODE _dwHvDeviceAxOleMisc =
		OLEMISC_ACTIVATEWHENVISIBLE |
		OLEMISC_SETCLIENTSITEFIRST |
		OLEMISC_INSIDEOUT |
		OLEMISC_CANTLINKINSIDE |
		OLEMISC_RECOMPOSEONRESIZE;

	IMPLEMENT_OLECTLTYPE(CHvDeviceAxCtrl, IDS_HVDEVICEAX, _dwHvDeviceAxOleMisc)



		// CHvDeviceAxCtrl::CHvDeviceAxCtrlFactory::UpdateRegistry -
		// Adds or removes system registry entries for CHvDeviceAxCtrl

		BOOL CHvDeviceAxCtrl::CHvDeviceAxCtrlFactory::UpdateRegistry(BOOL bRegister)
	{
		// TODO: Verify that your control follows apartment-model threading rules.
		// Refer to MFC TechNote 64 for more information.
		// If your control does not conform to the apartment-model rules, then
		// you must modify the code below, changing the 6th parameter from
		// afxRegApartmentThreading to 0.

		if (bRegister)
			return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_HVDEVICEAX,
			IDB_HVDEVICEAX,
			afxRegApartmentThreading,
			_dwHvDeviceAxOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
		else
			return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
	}



	// CHvDeviceAxCtrl::CHvDeviceAxCtrl - Constructor

	CHvDeviceAxCtrl::CHvDeviceAxCtrl()
		:m_hHandle(NULL)
		,m_iJpegDataLen(0)
		,m_dwCarID(0)
		,m_dwTimes(0)
		,m_strIP("")
		,m_strJpegFileName("")
		,m_strH264SavePath("")
		,m_dwTheadId(0)
		,m_hThread(NULL)
		,m_RecvSnapImageFlag(1)
		,m_RecvPlateImageFlag(1)
		,m_RecvPlateBinImageFlag(1)
		,m_RecvRecordFlag(1)
		,m_H264FrameRateNum(25)
		,m_RecvJpegFlag(0)
		,m_RecvH264Flag(0)
		,m_SaveH264VideoTime(1)
		,m_pbRecvJpegData(NULL)
		,m_fCreated(FALSE)
		,m_fSetRecordCallBack(FALSE)
		,m_JpegBackTime(0)
		,m_jpegnum(0)
		,m_iPathType(0)
		,m_ShowH264VideoFlag(0)
		,m_fIsPlay(false)
		,m_iH264Width(0)
		,m_iH264Heigh(0)
		,m_iBreakCount(0)
		//违章视频添加 add by zengmx
		,m_bIsBeginSaveIllegalVideo(false)
		,m_IllegalReason("")
		,m_dwIllCarIDPrev(0)
		,m_pVideoMemory(NULL)
		,m_strVideoFile("")
		,m_iDeviceType(DEV_TYPE_UNKNOWN)
		,m_RtspClientHandle(NULL)
		,m_HideIcon(0)
		,m_GetImage(NULL)
		,m_hInstHv(NULL)
		,m_GetImageEx(NULL)
	{
		InitializeIIDs(&IID_DHvDeviceAx, &IID_DHvDeviceAxEvents);
		InitializeCriticalSection(&m_csOpt);
		InitializeCriticalSection(&m_csResult);
		InitializeCriticalSection(&m_csJpegData);
		InitializeCriticalSection(&m_csH264Data);
		InitializeCriticalSection(&m_csIllegalVideoList);
		InitializeCriticalSection(&m_csIllegalReason);
		InitializeCriticalSection(&m_csIllegalVideoPath);
		memset(m_szCommand, 0, 1024);

		m_bDshow = false;
		m_chBkH264 = NULL;
		m_iH264Len = 0;
		m_bFullScreen = false;

		CH264FrameDecode::H264Env_Init();
		m_pShowMaxVideoDlg = NULL;

		//xurui 20150324
		m_pCaputureImage = NULL;
		m_iCaptureImgeSize = 0;
		m_fCapureFlag = false;

		//XURUI 20150801 加载抓拍函数
		m_hInstHv =::LoadLibrary("HvDevice.dll");//动态加载Dll
		if ( m_hInstHv != NULL )
		{
			m_GetImage =(GetImage)GetProcAddress(m_hInstHv,"HVAPI_GetCaptureImage");
			m_GetImageEx = (GetCaptureImageEx)GetProcAddress(m_hInstHv,"HVAPI_GetCaptureImageEx");
		}

	}



	// CHvDeviceAxCtrl::~CHvDeviceAxCtrl - Destructor

	CHvDeviceAxCtrl::~CHvDeviceAxCtrl()
	{
		Disconnect();
		if (NULL != m_resultData.resultFile.pastmSmallImg)
		{
			m_resultData.resultFile.pastmSmallImg->Release();
			m_resultData.resultFile.pastmSmallImg = NULL;
		}
		for (int i = 0; i < 5; i++)
		{
			if (NULL != m_resultData.resultFile.pastmBigImg[i])
			{
				m_resultData.resultFile.pastmBigImg[i]->Release();
				m_resultData.resultFile.pastmBigImg[i] = NULL;
			}
		}
		ClearIllegalVideoList();
		DeleteCriticalSection(&m_csOpt);
		DeleteCriticalSection(&m_csResult);
		DeleteCriticalSection(&m_csJpegData);
		DeleteCriticalSection(&m_csH264Data);
		DeleteCriticalSection(&m_csIllegalVideoList);
		DeleteCriticalSection(&m_csIllegalReason);
		DeleteCriticalSection(&m_csIllegalVideoPath);

		m_msgWnd.DestroyWindow();
		if (m_VideoSave.m_fOpenFile)
		{
			m_VideoSave.H264VideoSaverClose();
		}

		if (m_IllegalVideoSave.m_fOpenFile)
		{
			m_IllegalVideoSave.H264VideoSaverClose();
		}

		if ( m_chBkH264 != NULL )
		{
			delete[] m_chBkH264;
			m_chBkH264 = NULL;
		}

		
		if ( m_pCaputureImage != NULL )
		{
			delete[] m_pCaputureImage;
			m_pCaputureImage = NULL;
		}
		/*
		//live555
		if (  m_RtspClientHandle != NULL )
		{
			xlw_RtspClient_stop(m_RtspClientHandle);
			 m_RtspClientHandle = NULL;
		}*/
		CH264FrameDecode::H264Env_Release();

		if (m_hInstHv != NULL )
		{
			::FreeLibrary(m_hInstHv);//释放Dll函数
			m_hInstHv = NULL;
		}
	}


	// CHvDeviceAxCtrl::OnDraw - Drawing function

	void CHvDeviceAxCtrl::OnDraw(
		CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
	{
		if (!pdc)
			return;

		// TODO: Replace the following code with your own drawing code.
		//pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
		//pdc->Ellipse(rcBounds);
		// 限制控件窗口大小 
		if ( rcBounds.Height( ) < 49 || rcBounds.Width( ) < 49 )
		{
			SetControlSize( 49, 49 ); 
		}

		
		if (m_fIsPlay)
		{
			UpdateData(FALSE);
		}
		else
		{
			//显示位图 
			CBitmap bitmap; 
			CBitmap *pSignalWayBitmap; 
			CDC MemDC; 
			MemDC.CreateCompatibleDC( pdc );

			CBrush Brush(RGB(255,255,255));
			pdc->FillRect(rcBounds, &Brush);
			
			//CBitmap bitmapTemp;	
			//bitmapTemp.CreateCompatibleBitmap(&MemDC, rcBounds.Width(), rcBounds.Height());
			
			//pSignalWayBitmap = MemDC.SelectObject( &bitmapTemp );
			//pdc->BitBlt( rcBounds.left, rcBounds.top, rcBounds.Width(), rcBounds.Height(), &MemDC, 0, 0, SRCCOPY );


			//MemDC.SelectObject( pSignalWayBitmap ); 
			//bitmapTemp.DeleteObject();
			
			if (m_HideIcon == 0)
			{
				bitmap.LoadBitmap( IDB_BITMAP_DEVAX ); 
				pSignalWayBitmap = MemDC.SelectObject( &bitmap ); 
				//pdc->BitBlt( rcBounds.left, rcBounds.top, 49, 49, &MemDC, 0, 0, SRCCOPY ); 
				//pdc->BitBlt( rcBounds.left, rcBounds.top, rcBounds.right, rcBounds.bottom, &MemDC, 0, 0, SRCCOPY ); 
				pdc->BitBlt( rcBounds.right/2-24, rcBounds.bottom/2-24, 49, 49, &MemDC, 0, 0, SRCCOPY );
				MemDC.SelectObject( pSignalWayBitmap ); 

			}

			
			bitmap.DeleteObject();
			MemDC.DeleteDC();


		}
	}



	// CHvDeviceAxCtrl::DoPropExchange - Persistence support

	void CHvDeviceAxCtrl::DoPropExchange(CPropExchange* pPX)
	{
		ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
		PX_Long( pPX, "RecvH264Flag", m_RecvH264Flag, 0 );
		PX_Long( pPX, "RecvJpegFlag", m_RecvJpegFlag, 0 );
		PX_Long( pPX, "RecvPlateBinImageFlag", m_RecvPlateBinImageFlag, 1 );
		PX_Long( pPX, "RecvRecordFlag",m_RecvRecordFlag, 1 );
		PX_Long( pPX, "RecvPlateImageFlag", m_RecvPlateImageFlag, 1 );
		PX_Long( pPX, "RecvSnapImageFlag", m_RecvSnapImageFlag, 1 );
		PX_Long( pPX, "SaveH264VideoTime", m_SaveH264VideoTime, 1 );
		PX_Long( pPX, "ShowH264VideoFlag", m_ShowH264VideoFlag, 0 );
		COleControl::DoPropExchange(pPX);

		// TODO: Call PX_ functions for each persistent custom property.
	}



	// CHvDeviceAxCtrl::OnResetState - Reset control to default state

	void CHvDeviceAxCtrl::OnResetState()
	{
		COleControl::OnResetState();  // Resets defaults found in DoPropExchange

		// TODO: Reset any other control state here.
	}



	// CHvDeviceAxCtrl::AboutBox - Display an "About" box to the user

	void CHvDeviceAxCtrl::AboutBox()
	{
		CDialog dlgAbout(IDD_ABOUTBOX_HVDEVICEAX);
		dlgAbout.DoModal();
	}

	void CHvDeviceAxCtrl::WriteIniFile()
	{
		if (m_strIP.GetLength() <= 0)
		{
			return;
		}

		TCHAR fileName[MAX_PATH];
		GetModuleFileName(NULL, fileName, MAX_PATH-1);

		PathRemoveFileSpec(fileName);
		TCHAR iniFileName[MAX_PATH] = { 0 };
		strcpy(iniFileName, fileName);
		strcat(iniFileName, _T("\\SafeModeConfig.ini"));

		TCHAR chTemp[256] = {0};
		sprintf(chTemp, "%d", m_safeModeInfo.iEableSafeMode);
		if (m_safeModeInfo.iEableSafeMode == 0 )
		{
			CTime tm = CTime::GetCurrentTime();
			sprintf(m_safeModeInfo.szBeginTime,"%s", tm.Format("%Y.%m.%d_%H"));
		}

		WritePrivateProfileString(m_strIP, "SafeModeEnable", chTemp, iniFileName);
		WritePrivateProfileString(m_strIP, "BeginTime", m_safeModeInfo.szBeginTime, iniFileName);
		WritePrivateProfileString(m_strIP, "EndTime", m_safeModeInfo.szEndTime, iniFileName);
		sprintf(chTemp, "%d", m_safeModeInfo.index);
		WritePrivateProfileString(m_strIP, "Index", chTemp, iniFileName);
		sprintf(chTemp, "%d", m_safeModeInfo.DataInfo);
		WritePrivateProfileString(m_strIP, "DataType", chTemp, iniFileName);
	}

	void CHvDeviceAxCtrl::ReadIniFile()
	{
		TCHAR fileName[MAX_PATH];
		GetModuleFileName(NULL, fileName, MAX_PATH-1);

		PathRemoveFileSpec(fileName);
		TCHAR iniFileName[MAX_PATH] = { 0 };
		strcpy(iniFileName, fileName);
		strcat(iniFileName, _T("\\SafeModeConfig.ini"));

		SYSTEMTIME systemTime;
		GetSystemTime(&systemTime);
		char szDefaultTime[64];
		ZeroMemory(szDefaultTime, sizeof(szDefaultTime));
		sprintf(szDefaultTime, "%4d.%2d.%2d_%2d", systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour);
		CString strDefaultTime(szDefaultTime);
		strDefaultTime.Replace(' ','0');

		m_safeModeInfo.iEableSafeMode = GetPrivateProfileInt(m_strIP, "SafeModeEnable", 0, iniFileName);
		GetPrivateProfileString(m_strIP, "BeginTime", strDefaultTime, m_safeModeInfo.szBeginTime, 256, iniFileName);
		GetPrivateProfileString(m_strIP, "EndTime", "0", m_safeModeInfo.szEndTime, 256, iniFileName);
		m_safeModeInfo.index = GetPrivateProfileInt(m_strIP, "Index", 0, iniFileName);
		m_safeModeInfo.DataInfo = GetPrivateProfileInt(m_strIP, "DataType", 0, iniFileName);

		sprintf(m_szCommand, "DownloadRecord,BeginTime[%s],Index[%d],Enable[%d],EndTime[%s],DataInfo[%d]",
			m_safeModeInfo.szBeginTime, m_safeModeInfo.index, m_safeModeInfo.iEableSafeMode, m_safeModeInfo.szEndTime, m_safeModeInfo.DataInfo);
	}


	void CHvDeviceAxCtrl::DrawImageInWindow(HWND pWnd, Bitmap* pbmp, int iHeightFactor)
	{
		if (!pWnd || !pbmp) return;

		RECT rect;
		GetClientRect( &rect );
		Graphics grf( pWnd );

		if ( grf.GetLastStatus() == Ok )
		{
			Rect rc( rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top );
			grf.DrawImage(pbmp, rc);
		}
	}


	// CHvDeviceAxCtrl message handlers
	//H264结果标识改变
	void CHvDeviceAxCtrl::OnRecvH264FlagChanged(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		EnterCriticalSection(&m_csOpt);
		if (m_RecvH264Flag)
	{
		if ( m_strH264SavePath != _T("") || m_iDeviceType != DEV_TYPE_HVVENUS )
		{
			int iReturnValue = HVAPI_SetCallBackEx(m_hHandle, CHvDeviceAxCtrl::OnH264FrameProxy, this, 0, CALLBACK_TYPE_H264_VIDEO, NULL);
			if (-1 == iReturnValue)
			{
				HVAPI_CloseEx(m_hHandle);
				m_hHandle = NULL;
				Sleep(1000);
			}
			}
		}
		else
		{
			int iReturnValue = HVAPI_SetCallBackEx(m_hHandle, NULL, NULL,  0, CALLBACK_TYPE_H264_VIDEO, NULL);
			if (m_VideoSave.m_fOpenFile)
			{
				m_VideoSave.H264VideoSaverClose();
			}
			if (-1 == iReturnValue)
			{
				HVAPI_CloseEx(m_hHandle);
				m_hHandle = NULL;
				Sleep(1000);
			}
		}
		LeaveCriticalSection(&m_csOpt);

		SetModifiedFlag();
	}

	//Jpeg接收标识改变
	void CHvDeviceAxCtrl::OnRecvJpegFlagChanged(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		EnterCriticalSection(&m_csOpt);
		if (m_RecvJpegFlag)
		{
			LPSTR lpSzCommond = "SetImgType,EnableRecogVideo[1]";
#ifdef DEBUGOCX
			CString strDebug;
			strDebug.Format("m_hhandle = %d", m_hHandle);
			OutputDebugStringA( strDebug);
#endif
			int iReturnValue = HVAPI_SetCallBackEx(m_hHandle, CHvDeviceAxCtrl::OnJpegFrameProxy, this,  0, CALLBACK_TYPE_JPEG_FRAME, lpSzCommond);
			if (-1 == iReturnValue)
			{
				HVAPI_CloseEx(m_hHandle);
				m_hHandle = NULL;
				Sleep(1000);
			}

#ifdef DEBUGOCX
			//CString strDebug;
			strDebug.Format("开启jpeg close m_hhandle = %d  %d", m_hHandle, iReturnValue);
			OutputDebugStringA( strDebug);
#endif
		}
		else
		{

			int iReturnValue = HVAPI_SetCallBackEx(m_hHandle, NULL, NULL, 0, CALLBACK_TYPE_JPEG_FRAME, NULL);
			if (-1 == iReturnValue)
			{
				HVAPI_CloseEx(m_hHandle);
				m_hHandle = NULL;
				Sleep(1000);
			}
#ifdef DEBUGOCX
			CString strDebug;
			strDebug.Format("关闭jpeg close m_hhandle = %d  %d", m_hHandle, iReturnValue);
			OutputDebugStringA( strDebug);
#endif
		}
		LeaveCriticalSection(&m_csOpt);

		SetModifiedFlag();
	}


	//结果接收标识改动

	void CHvDeviceAxCtrl::SetRecordCallBackflag()
	{

		EnterCriticalSection(&m_csOpt);
		if ((m_RecvRecordFlag)
			&& !m_fSetRecordCallBack)
		{
			ReadIniFile();
			HVAPI_SetCallBackEx(m_hHandle , CHvDeviceAxCtrl::OnIllegalVideoProxy , this , 0, CALLBACK_TYPE_RECORD_ILLEGALVIDEO , /*szConnCmd*/NULL);
			HVAPI_SetCallBackEx(m_hHandle , CHvDeviceAxCtrl::OnRecordBeginProxy , this , 0, CALLBACK_TYPE_RECORD_INFOBEGIN , /*szConnCmd*/NULL);
			HVAPI_SetCallBackEx(m_hHandle , CHvDeviceAxCtrl::OnPlateProxy , this , 0, CALLBACK_TYPE_RECORD_PLATE , m_szCommand);
			HVAPI_SetCallBackEx(m_hHandle , CHvDeviceAxCtrl::OnSmallProxy , this , 0, CALLBACK_TYPE_RECORD_SMALLIMAGE , /*szConnCmd*/NULL);
			HVAPI_SetCallBackEx(m_hHandle , CHvDeviceAxCtrl::OnBinaryProxy , this , 0, CALLBACK_TYPE_RECORD_BINARYIMAGE , /*szConnCmd*/NULL);
			HVAPI_SetCallBackEx(m_hHandle , CHvDeviceAxCtrl::OnBigImageProxy , this ,0, CALLBACK_TYPE_RECORD_BIGIMAGE , /*szConnCmd*/NULL);
			HVAPI_SetCallBackEx(m_hHandle , CHvDeviceAxCtrl::OnRecordEndProxy , this , 0, CALLBACK_TYPE_RECORD_INFOEND , /*szConnCmd*/NULL);

			//int iReturnValue = HVAPI_SetCallBackEx(m_hHandle, CHvDeviceAxCtrl::OnRecordCallBackProxy, this, STREAM_TYPE_RECORD, NULL);
			//if (-1 == iReturnValue)
			//{
			//	HVAPI_CloseEx(m_hHandle);
			//	return;
			//}
			m_fSetRecordCallBack = TRUE;
		}
		else if(!m_RecvRecordFlag)
		{
			HVAPI_SetCallBackEx(m_hHandle , NULL , NULL , 0, CALLBACK_TYPE_RECORD_ILLEGALVIDEO , /*szConnCmd*/NULL);
			HVAPI_SetCallBackEx(m_hHandle, NULL, NULL, 0, CALLBACK_TYPE_RECORD_BIGIMAGE, NULL);
			HVAPI_SetCallBackEx(m_hHandle, NULL, NULL, 0, CALLBACK_TYPE_RECORD_SMALLIMAGE, NULL);
			HVAPI_SetCallBackEx(m_hHandle, NULL, NULL, 0, CALLBACK_TYPE_RECORD_BINARYIMAGE, NULL);
			HVAPI_SetCallBackEx(m_hHandle, NULL, NULL, 0, CALLBACK_TYPE_RECORD_INFOBEGIN, NULL);
			HVAPI_SetCallBackEx(m_hHandle, NULL, NULL, 0, CALLBACK_TYPE_RECORD_INFOEND, NULL);
			//int iReturnValue = HVAPI_SetCallBackEx(m_hHandle, NULL, NULL, STREAM_TYPE_RECORD, NULL);
			//if (-1 == iReturnValue)
			//{
			//	HVAPI_CloseEx(m_hHandle);
			//	return;
			//}
			m_fSetRecordCallBack = FALSE;
		}
		LeaveCriticalSection(&m_csOpt);

	}


	void CHvDeviceAxCtrl::OnRecvPlateBinImageFlagChanged(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		//SetRecordCallBackflag();
		SetModifiedFlag();
	}

	void CHvDeviceAxCtrl::OnRecvPlateImageFlagChanged(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		//SetRecordCallBackflag();
		SetModifiedFlag();
	}

	void CHvDeviceAxCtrl::OnRecvSnapImageFlagChanged(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		//SetRecordCallBackflag();
		SetModifiedFlag();
	}

	void CHvDeviceAxCtrl::OnSaveH264VideoTimeChanged(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		SetModifiedFlag();
	}

	void CHvDeviceAxCtrl::OnRecvRecordFlagChanged(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		SetRecordCallBackflag();
		SetModifiedFlag();
	}




	void CHvDeviceAxCtrl::OnRecvIllegalVideoFlagChanged(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		// TODO: 在此添加属性处理程序代码

		SetModifiedFlag();
	}

	void CHvDeviceAxCtrl::ConnectTo(LPCTSTR strAddr)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CString strNewIP(strAddr);
		int i = m_RecvH264Flag;
		int iLen = strNewIP.GetLength();
		if (iLen < 7)
		{
			return;
		}

		strNewIP.MakeUpper();
		Disconnect();
		m_fClosed = false;

		/*
		if ( m_bDshow )
		{
			//读数据的接收点,构造接收数据的命令
			TCHAR fileName[MAX_PATH];
			GetModuleFileName(theApp.m_hInstance, fileName, MAX_PATH-1);
			PathRemoveFileSpec(fileName);
			char szFile[1024] = { 0 };
			for( int i=0; i<25; i++)
			{
				sprintf(szFile, "%s\\Bk\\bk_%d.H264", fileName,i);
				FILE *myFile = fopen(szFile, "rb");
				if (myFile)
				{
					int iLen = 1024 * 1024;
					if ( m_chBkH264 == NULL )
						 m_chBkH264 = new BYTE[iLen];

					memset(m_chBkH264, 0, iLen);
					m_iH264Len = fread(m_chBkH264, 1, iLen, myFile);
					DSShowVideo(this->m_hWnd, 1280, 720, (char*)m_chBkH264, m_iH264Len);
					fclose(myFile);
					myFile = NULL;
				}
			}

			m_bDshow = false;
		}*/


		m_iH264Width = 0;
		m_iH264Heigh = 0;

		m_strIP = strNewIP;
		ReadIniFile();
		m_hThread = CreateThread(NULL, 0, CHvDeviceAxCtrl::HvConnectThread, this, 0, &m_dwTheadId);
		if (NULL == m_hThread)
		{
			Disconnect();		

		}
		else
		{
			// 初始化要保存的数据
			//for (int i = 0; i < MAX_BIG_IMG_TYPE; i++)
			//{
			//	m_resultData.resultFile.pastmBigImg[i] = NULL;
			//}
			//m_resultData.resultFile.pastmSmallImg = NULL;
			m_resultData.resultFile.pbBinImg = NULL;
			m_resultData.resultFile.dwBinImgSize = 0;
			m_resultData.resultFile.strPlateNo = "";
			m_resultData.resultFile.strAppendInfo = "";
		}

	}

	LONG CHvDeviceAxCtrl::GetCarID(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		EnterCriticalSection(&m_csResult);
		int iCarID = m_resultData.resultInfo.dwCarID;
		LeaveCriticalSection(&m_csResult);
		return iCarID;

	}

	BSTR CHvDeviceAxCtrl::GetPlate(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CString strResult;
		EnterCriticalSection(&m_csResult);
		strResult = m_resultData.resultFile.strPlateNo;
		LeaveCriticalSection(&m_csResult);

		return strResult.AllocSysString();
	}

	BSTR CHvDeviceAxCtrl::GetPlateColor(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CString strResult;

		EnterCriticalSection(&m_csResult);
		strResult = m_resultData.resultFile.strPlateNo.Left(2);
		LeaveCriticalSection(&m_csResult);

		return strResult.AllocSysString();
	}

	BSTR CHvDeviceAxCtrl::GetPlateInfo(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CString strResult;

		EnterCriticalSection(&m_csResult);
		strResult = m_resultData.resultFile.strAppendInfo;
		LeaveCriticalSection(&m_csResult);

		return strResult.AllocSysString();
	}

	LONG CHvDeviceAxCtrl::GetStatus(SHORT iConnType)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		DWORD dwConnStatus = CONN_STATUS_UNKNOWN;
		EnterCriticalSection(&m_csOpt);
		if (NULL == m_hHandle)
		{
			LeaveCriticalSection(&m_csOpt);
			return 2;
		}

		//判断结果连接是否断开
		if (RECORDCONNECT == iConnType)
		{
			HVAPI_GetConnStatusEx(m_hHandle, CONN_TYPE_RECORD, &dwConnStatus);
		}

		//判断Jpeg连接是否断开
		if (JPEGCONNECT == iConnType)
		{
			HVAPI_GetConnStatusEx(m_hHandle, CONN_TYPE_IMAGE, &dwConnStatus);
		}

		//判断H264连接是否断开
		if (H264CONNECT == iConnType)
		{
			//if ( m_iDeviceType == DEV_TYPE_HVVENUS )
			{
				HVAPI_GetConnStatusEx(m_hHandle, CONN_TYPE_VIDEO, &dwConnStatus);
			}
			/*else
			{
				if ( m_rtspState == 1)
					dwConnStatus =  CONN_STATUS_NORMAL;
				else
					dwConnStatus = CONN_STATUS_RECONN;

				if ( m_RtspClientHandle == NULL )
					dwConnStatus = CONN_STATUS_UNKNOWN;

			}*/
			
		}

		LeaveCriticalSection(&m_csOpt);
		int iReturn = 2;
		switch (dwConnStatus)
		{
		case CONN_STATUS_UNKNOWN:
			iReturn = 0;
			break;
		case CONN_STATUS_NORMAL:
			iReturn = 1;
			break;
		case CONN_STATUS_DISCONN:
			iReturn = 2;
			break;
		case CONN_STATUS_RECONN:
			iReturn = 3;
			break;
		}

		return iReturn;
	}

	LONG CHvDeviceAxCtrl::SaveH264Video(LPCTSTR strFilePath)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		EnterCriticalSection(&m_csH264Data);
		m_strH264SavePath.Format("%s", strFilePath);
		if(!DiskStatus(strFilePath))
		{
			m_strH264SavePath = "";
			LeaveCriticalSection(&m_csH264Data);
			return -10;
		}
		LeaveCriticalSection(&m_csH264Data);

		return 0;
	}

	LONG CHvDeviceAxCtrl::SaveJpegVideo(LPCTSTR strFileName)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		if (!m_pbRecvJpegData
			|| m_iJpegDataLen <= 0
			|| m_pbRecvJpegData[0] != 0xff
			|| m_pbRecvJpegData[1] != 0xd8)
		{
			return -1;
		}
		EnterCriticalSection(&m_csJpegData);
		FILE* pFile;
		CTime cTime(m_JpegBackTime);
		CString pathtemp,lasttemp;
		if(cTime.GetYear()>1999)
		{
			int month = cTime.GetMonth();
			int day = cTime.GetDay();
			int hour = cTime.GetHour();
			int minute = cTime.GetMinute();
			pathtemp.Format("%s%04d%02d%02d\\%02d\\%02d\\",strFileName,cTime.GetYear(), month,day,hour,minute);
			if (m_iPathType == 1)
			{
				int index = 0;
				CString strTmp = strFileName;
				int indexTmp = 0;
				while (!strTmp.IsEmpty())
				{
					indexTmp = strTmp.Find("\\", indexTmp);
					if (indexTmp != -1)
					{
						index = indexTmp;
					}
					else
					{
						break;
					}
					indexTmp += 1;
				}
				pathtemp = "";
				pathtemp = strTmp.Left(index + 1);
			}

			if (PathFileExists(pathtemp.GetBuffer()) == FALSE)
			{
				m_jpegnum = 0;
			}
			pathtemp.ReleaseBuffer();
			MakeSureDirectoryPathExists(pathtemp.GetBuffer());
			pathtemp.ReleaseBuffer();
			m_jpegnum++;
			lasttemp.Format("%s%s_%d.jpg",pathtemp,cTime.Format("%Y%m%d%H%M%S"),m_jpegnum);
			if (m_iPathType == 1)
			{
				lasttemp = "";
				lasttemp = strFileName;
			}
			if(!DiskStatus(strFileName))
			{//磁盘满
				LeaveCriticalSection(&m_csJpegData);
				return -10;
			}
			pFile = fopen(lasttemp, "wb");
			if (pFile)
			{
				fwrite(m_pbRecvJpegData, 1, m_iJpegDataLen, pFile);
				fclose(pFile);
				m_strVideoJPEGFile = lasttemp;
			}
			else
			{
				LeaveCriticalSection(&m_csJpegData);
				return -1;
			}
		}
		LeaveCriticalSection(&m_csJpegData);

		return 0;
	}

	LONG CHvDeviceAxCtrl::SavePlateBin2BMP(LPCTSTR strFileName)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		if (!m_resultData.resultFile.dwBinImgSize)
		{
			return -1;
		}
		EnterCriticalSection(&m_csResult);
		CTime m_PlateTime;
		m_PlateTime = CTime(m_dwTimes/1000);
		CString temp_Path;
		temp_Path.Format("%s%s\\",strFileName,m_PlateTime.Format("%Y%m%d"));
		CString file_name;

		if (m_iPathType != 1)
			file_name.Format("%s%s%s%d%s", temp_Path, m_PlateTime.Format("%Y%m%d%H%M%S"), "_",m_resultData.resultInfo.dwCarID, ".BMP");
		else
			file_name = strFileName;


		if (m_iPathType == 1)
		{
			MakeSureDirectoryPathExists(file_name.GetBuffer());
			file_name.ReleaseBuffer();
		}
		else
		{
			MakeSureDirectoryPathExists(temp_Path.GetBuffer());
			temp_Path.ReleaseBuffer();
		}
		if (m_resultData.resultFile.pbBinImg)
		{
			int iBinImageLen = m_resultData.resultFile.dwBinImgSize;

			PBYTE pByte = new BYTE[30 * 1024];
			HVAPIUTILS_BinImageToBitmapEx(m_resultData.resultFile.pbBinImg, pByte, &iBinImageLen);

			CString strLen;
			FILE* pFile;
			if(!DiskStatus(strFileName))
			{
				delete[] pByte;
				LeaveCriticalSection(&m_csResult);
				return -10;
			}
			pFile = fopen(file_name, "wb");
			if (pFile)
			{
				fwrite(pByte, iBinImageLen, 1, pFile);
				fclose(pFile);
				m_strRecordFile = file_name.GetBuffer();
				file_name.ReleaseBuffer();
			}
			else
			{
				delete[] pByte;
				LeaveCriticalSection(&m_csResult);
				return -1;
			}
			delete[] pByte;
		}
		LeaveCriticalSection(&m_csResult);

		return 0;
	}

	LONG CHvDeviceAxCtrl::SavePlateBinImage(LPCTSTR strFileName)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		if (!m_resultData.resultFile.dwBinImgSize)
		{
			return -1;
		}

		CTime m_PlateTime;
		m_PlateTime = CTime(m_dwTimes/1000);
		CString temp_Path;
		temp_Path.Format("%s%s\\",strFileName,m_PlateTime.Format("%Y%m%d"));
		CString file_name;

		if(m_iPathType != 1)
			file_name.Format("%s%s%s%d%s", temp_Path, m_PlateTime.Format("%Y%m%d%H%M%S"), "_",m_resultData.resultInfo.dwCarID,".bin");
		else
			file_name = strFileName;



		if (m_iPathType == 1)
		{
			file_name = "";
			file_name.Format("%s",strFileName);
			MakeSureDirectoryPathExists(file_name.GetBuffer());
			file_name.ReleaseBuffer();
		}
		else
		{
			MakeSureDirectoryPathExists(temp_Path.GetBuffer());
			temp_Path.ReleaseBuffer();
		}
		FILE* pFile;
		if(!DiskStatus(strFileName))
		{

			return -10;
		}
		pFile = fopen(file_name, "wb");
		if (pFile)
		{
			EnterCriticalSection(&m_csResult);
			PBYTE pByte = m_resultData.resultFile.pbBinImg;
			DWORD32 dwSize = m_resultData.resultFile.dwBinImgSize;
			fwrite(pByte, 1, dwSize, pFile);
			fclose(pFile);
			m_strRecordFile = file_name.GetBuffer();
			file_name.ReleaseBuffer();
			LeaveCriticalSection(&m_csResult);
		}
		else
		{
			return -1;
		}

		return 0;
	}

	LONG CHvDeviceAxCtrl::SavePlateImage(LPCTSTR strFileName)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

#ifdef DEBUGOCX
		OutputDebugStringA(_T("SavePlateImage 保存车牌图"));
#endif


		IStream* tmpIStream;
		int iSize;
		int iReturnValue = 0;

		EnterCriticalSection(&m_csResult);
		CTime m_PlateTime;
		m_PlateTime = CTime(m_dwTimes/1000);

		CString temp_Path;
		temp_Path.Format("%s%s\\",strFileName,m_PlateTime.Format("%Y%m%d"));

		CString file_name;

		if (m_iPathType != 1)
		{
			file_name.Format("%s%s%s%d%s", temp_Path, m_PlateTime.Format("%Y%m%d%H%M%S"),"_",m_resultData.resultInfo.dwCarID, ".jpg");
		}
		else
		{
			file_name = strFileName;
		}



		tmpIStream = m_resultData.resultFile.pastmSmallImg;
		iSize = m_resultData.resultInfo.iSmallImgSize;
#ifdef DEBUGOCX
		CString  strDebug;
		strDebug.Format("%d %d", m_iPathType, iSize);
		OutputDebugStringA( strDebug );
#endif

		if (m_iPathType == 1)
		{
#ifdef DEBUGOCX
			OutputDebugStringA(_T("  m_iPathType == 1 "));
#endif
			file_name = "";
			file_name.Format("%s",strFileName);
			MakeSureDirectoryPathExists(file_name.GetBuffer());
			file_name.ReleaseBuffer();
		}
		else
		{
			MakeSureDirectoryPathExists(temp_Path.GetBuffer());
			temp_Path.ReleaseBuffer();
		}




		if(!DiskStatus(strFileName))
		{
			iReturnValue = -10;
		}
		else if (iSize)
		{
#ifdef DEBUGOCX
			OutputDebugStringA(_T("iSize不为0 保存车牌图"));
#endif
			SaveStreamFile(tmpIStream, iSize, file_name);
			m_strRecordFile = file_name.GetBuffer();
			file_name.ReleaseBuffer();
		}
		else
		{
			iReturnValue = -1;
		}
		LeaveCriticalSection(&m_csResult);
		return iReturnValue;
	}

	LONG CHvDeviceAxCtrl::SaveSnapImage(LPCTSTR strFileName, SHORT iImageType)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		IStream* tmpIStream = NULL;
		int iSize;
		int iReturnValue = 0;

		EnterCriticalSection(&m_csResult);

		CTime m_PlateTime;
		m_PlateTime = CTime(m_dwTimes/1000);
		CString temp_Path;
		temp_Path.Format("%s%s\\",strFileName,m_PlateTime.Format("%Y%m%d"));
		CString file_name;
		file_name.Format("%s%s%s%d%s%d%s", temp_Path, m_PlateTime.Format("%Y%m%d%H%M%S"),
			"_",m_resultData.resultInfo.dwCarID,"_" ,iImageType,".jpg");

		tmpIStream = m_resultData.resultFile.pastmBigImg[iImageType];
		iSize = m_resultData.resultInfo.rgBigImgSize[iImageType];
		if (m_iPathType == 1)
		{
			file_name = "";
			file_name.Format("%s",strFileName);
			MakeSureDirectoryPathExists(file_name.GetBuffer());
			file_name.ReleaseBuffer();
		}
		else
		{
			MakeSureDirectoryPathExists(temp_Path.GetBuffer());
			temp_Path.ReleaseBuffer();
		}
		if(!DiskStatus(strFileName))
		{
			iReturnValue = -10;
		}
		else if (iSize)
		{
			SaveStreamFile(tmpIStream, iSize, file_name);
			m_strRecordFile = file_name.GetBuffer();
			file_name.ReleaseBuffer();
		}
		else
		{
			iReturnValue = -1;
			m_strRecordFile = "";
		}
		LeaveCriticalSection(&m_csResult);

		return iReturnValue;
	}

	void CHvDeviceAxCtrl::Disconnect(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		EnterCriticalSection(&m_csOpt);
		if (m_hThread != NULL)
		{
			m_fClosed = true;
			WaitForSingleObject(m_hThread, 3000);

			if(!CloseHandle(m_hThread))
			{
				LeaveCriticalSection( &m_csOpt );
				return;
			}
			m_hThread = NULL;
		}

		if (m_hHandle != NULL)
		{
			if(m_hThread != NULL)
			{
				LeaveCriticalSection( &m_csOpt );
				return;
			}
			if(S_OK != HVAPI_CloseEx(m_hHandle))
			{
				LeaveCriticalSection( &m_csOpt );
				return;
			}

			m_hHandle = NULL;
			ClearToSaveData();
		}
		WriteIniFile();
		LeaveCriticalSection( &m_csOpt );
		/*
		//live555
		if (  m_RtspClientHandle != NULL )
		{
			xlw_RtspClient_stop(m_RtspClientHandle);
			 m_RtspClientHandle = NULL;
		}*/

		
		if (m_pShowMaxVideoDlg!= NULL)
		{
			m_pShowMaxVideoDlg->DestroyWindow();
			delete m_pShowMaxVideoDlg;
			m_pShowMaxVideoDlg = NULL;
		}


}

	HRESULT CHvDeviceAxCtrl::ClearToSaveData()
	{
		EnterCriticalSection(&m_csResult);
		if (m_resultData.resultFile.pbBinImg)
		{
			delete[] m_resultData.resultFile.pbBinImg;
			m_resultData.resultFile.pbBinImg = NULL;
		}

		m_resultData.resultFile.dwBinImgSize = 0;
		m_resultData.resultFile.strPlateNo = "";
		m_resultData.resultFile.strAppendInfo = "";
		m_resultData.resultInfo.iSmallImgSize = 0;
		for (int i = 0; i < MAX_BIG_IMG_TYPE; i++)
		{
			if (m_resultData.resultFile.pastmBigImg[i])
			{
				m_resultData.resultInfo.rgBigImgSize[i] = 0;
			}
		}

		LeaveCriticalSection(&m_csResult);

		return S_OK;
	}

	void  CHvDeviceAxCtrl::ShowRtspH264()
	{

		//不是金星设备， 不需要调用RTSP ， 直接使用原协议
		if (  m_iDeviceType != DEV_TYPE_HVVENUS )
		{
			return ;
		}

		static int iTryTimes = 0;
		/*
		if ( m_RecvH264Flag )  //显示视频
		{	
			if ( m_RtspClientHandle == NULL )
			{
				char szUrl[1024] = { 0 };
				sprintf(szUrl, "rtsp://%s/h264ESVideoTest",  m_strIP);   //rtsp://%s/h264ESVideoTest

				m_iH264Heigh = 0;
				m_iH264Width = 0;
				m_RtspClientHandle = xlw_RtspClient_start(szUrl, CHvDeviceAxCtrl::SLWRtspStreamBackProxy, this);
			}
			
			else
			{
				m_rtspState = 0;
				m_rtspState = xlw_RtspClient_queryState(m_RtspClientHandle);
				if ( m_rtspState == 0  )
				{
					if (++iTryTimes > 3 )
					 {
						 xlw_RtspClient_stop(m_RtspClientHandle);
						 m_RtspClientHandle = NULL;
						 iTryTimes = 0;
					}
				}

			}


		}
		else
		{
			if ( m_RtspClientHandle!=NULL )
			{
				xlw_RtspClient_stop(m_RtspClientHandle);
				m_RtspClientHandle = NULL;
			}
		}*/

	}

	bool  CHvDeviceAxCtrl::GetDeviceType()
	{
		int iDeviceType = 0;
		// 获取相机类型
		char szAddr[32] = {0};
		sprintf(szAddr, "%s", m_strIP);
		if ( HVAPI_GetDevTypeEx(szAddr, &iDeviceType) != S_OK )
		{
			return false;
		}
		
		m_iDeviceType = iDeviceType;
		return true;
	}



	DWORD WINAPI CHvDeviceAxCtrl::HvConnectThread(LPVOID lpParameter)
	{
		if (NULL == lpParameter)
		{
			return -1;
		}

#ifdef DEBUGOCX
		OutputDebugStringA(_T("创建连接线程"));
#endif

		CHvDeviceAxCtrl* pAxCtrl = (CHvDeviceAxCtrl*) lpParameter;
		int iOutflg = 0;

		while(!pAxCtrl->m_fClosed)
		{
			if (NULL == pAxCtrl->m_hHandle)
			{
				/*
				if ( !pAxCtrl->GetDeviceType())
				{
					Sleep(1000);
					continue;;
				}
				*/

				//调试使用
				HVAPI_HANDLE_EX hHandle = HVAPI_OpenEx(pAxCtrl->m_strIP.GetBuffer(), NULL);
				pAxCtrl->m_strIP.ReleaseBuffer();
				if (NULL != hHandle)
				{
					int iReturnValue = 0;
					if (pAxCtrl->m_RecvPlateBinImageFlag
						|| pAxCtrl->m_RecvPlateImageFlag
						|| pAxCtrl->m_RecvSnapImageFlag)
					{
						pAxCtrl->ReadIniFile();
						HVAPI_SetCallBackEx(hHandle , CHvDeviceAxCtrl::OnIllegalVideoProxy , pAxCtrl , 0, CALLBACK_TYPE_RECORD_ILLEGALVIDEO , /*szConnCmd*/NULL);
						HVAPI_SetCallBackEx(hHandle , CHvDeviceAxCtrl::OnRecordBeginProxy , pAxCtrl , 0, CALLBACK_TYPE_RECORD_INFOBEGIN , /*szConnCmd*/NULL);
						HVAPI_SetCallBackEx(hHandle , CHvDeviceAxCtrl::OnSmallProxy , pAxCtrl , 0, CALLBACK_TYPE_RECORD_SMALLIMAGE , /*szConnCmd*/NULL);
						HVAPI_SetCallBackEx(hHandle , CHvDeviceAxCtrl::OnBinaryProxy , pAxCtrl , 0, CALLBACK_TYPE_RECORD_BINARYIMAGE , /*szConnCmd*/NULL);
						HVAPI_SetCallBackEx(hHandle , CHvDeviceAxCtrl::OnBigImageProxy , pAxCtrl ,0, CALLBACK_TYPE_RECORD_BIGIMAGE , /*szConnCmd*/NULL);
						HVAPI_SetCallBackEx(hHandle , CHvDeviceAxCtrl::OnRecordEndProxy , pAxCtrl , 0, CALLBACK_TYPE_RECORD_INFOEND , /*szConnCmd*/NULL);
						HVAPI_SetCallBackEx(hHandle , CHvDeviceAxCtrl::OnPlateProxy , pAxCtrl , 0, CALLBACK_TYPE_RECORD_PLATE , pAxCtrl->m_szCommand);
						//TODO(liyh) iReturnValue = HVAPI_SetCallBackEx(hHandle, CHvDeviceAxCtrl::OnRecordCallBackProxy, pAxCtrl, STREAM_TYPE_RECORD, NULL);
						if (-1 == iReturnValue)//怎么确定设置回调失败？ TODO(liyh)
						{
							HVAPI_CloseEx(hHandle);
							Sleep(1000);
							continue;
						}
					}

					if (pAxCtrl->m_RecvJpegFlag)
					{
#ifdef DEBUGOCX
						CString strDebug;
						OutputDebugStringA(_T(" onJpegStreamCallCallBack  strstr前"));
						//strDebug.Format("%s 是不是空？", szImageExtInfo);
						//OutputDebugStringA(szImageExtInfo);
#endif
						LPSTR lpSzCommond = "SetImgType,EnableRecogVideo[1]";
						iReturnValue = HVAPI_SetCallBackEx(hHandle, CHvDeviceAxCtrl::OnJpegFrameProxy, pAxCtrl,  0, CALLBACK_TYPE_JPEG_FRAME, lpSzCommond);
						if (-1 == iReturnValue)
						{
							HVAPI_CloseEx(hHandle);
							Sleep(1000);
							continue;
						}
					}

				if ( pAxCtrl->m_RecvH264Flag && pAxCtrl->m_iDeviceType != DEV_TYPE_HVVENUS )
					{
						iReturnValue = HVAPI_SetCallBackEx(hHandle, CHvDeviceAxCtrl::OnH264FrameProxy, pAxCtrl, 0, CALLBACK_TYPE_H264_VIDEO, NULL);
						if (-1 == iReturnValue)
						{
							HVAPI_CloseEx(hHandle);
							Sleep(1000);
							continue;
						}
					}

					EnterCriticalSection(&pAxCtrl->m_csOpt);
					pAxCtrl->m_hHandle = hHandle;
					LeaveCriticalSection(&pAxCtrl->m_csOpt);

				}
			}
			/*else
			{
				//判断是否发送模拟H264
				pAxCtrl->SendMsgForSetSaveH264();
			}*/
			
			
			iOutflg = 0;
			
			pAxCtrl->ShowRtspH264();
			

			//合并违章视频队列
			pAxCtrl ->DealIllegalVideoList();
			Sleep(1000);
		}

		return 0;
	}

	//违章视频回调
	//add by zengmx at 20140923
	int CHvDeviceAxCtrl::OnIllegalVideo(PVOID pUserData, DWORD dwCarID,
		DWORD dwVideoType, DWORD dwVideoWidth, DWORD dwVideoHeight,
		DWORD64 dw64TimeMS, PBYTE pbVideoData, DWORD dwVideoDataLen, LPCSTR szVideoExtInfo)
	{
		if(!m_RecvIllegalVideoFlag)
		{
			return 0;
		}

		if(m_pVideoMemory == NULL)
		{
			m_pVideoMemory = new CVideoMemory();
			if(m_pVideoMemory == NULL)
			{
				return 0;
			}
		}

		if(pbVideoData == NULL)
		{
			m_bIsBeginSaveIllegalVideo = false;
			//加入违章视频接收完成信息到队列
			AddIllegalVideoList(m_dwIllCarIDPrev , m_pVideoMemory);
			m_pVideoMemory = NULL;
			return 0;
		}
		else
		{
			m_dwIllCarIDPrev = dwCarID;
		}
		//第一帧
		if(!m_bIsBeginSaveIllegalVideo && (VIDEO_TYPE_H264_HISTORY_P  ==  dwVideoType || VIDEO_TYPE_H264_NORMAL_P == dwVideoType))
		{
			return 0;
		}
		m_bIsBeginSaveIllegalVideo = true;
		m_pVideoMemory->AddOneFrame(pbVideoData, dwVideoDataLen, dwVideoType, dwVideoWidth, dwVideoHeight);
		return 0;
	}

	int CHvDeviceAxCtrl::OnRecordBegin(PVOID pUserData, DWORD dwCarID)
	{
		if (m_fClosed)
		{
			return -1;
		}

		//清空数据
		ClearToSaveData();//清空小图 二位图数组

		return 0;
	}

	int CHvDeviceAxCtrl::OnRecordEnd(PVOID pUserData, DWORD dwCarID)
	{

		if (m_fCapureFlag)
		{
			//抓拍图接收完毕， 重新切换回原来接收识别结果的方式
			m_fCapureFlag = false;
			return 0;
		}


		m_msgWnd.SendMessage(MSG_RECEIVE_RESULT, (WPARAM)this, 0);
		if (m_fClosed)
		{
			return -1;
		}

		//记录已经接到的结果断点
		if ( m_safeModeInfo.iEableSafeMode == 1 )
		{
			m_safeModeInfo.index = m_resultData.resultInfo.dwCarID-1;
			CTime tm(m_resultData.resultInfo.dw64Time/1000);
			sprintf(m_safeModeInfo.szBeginTime, "%s", tm.Format("%Y.%m.%d_%H"));

			m_iBreakCount++;

			if (m_iBreakCount > 3)
			{
				//读数据的接收点,构造接收数据的命令
				TCHAR fileName[MAX_PATH];
				GetModuleFileName(NULL, fileName, MAX_PATH-1);
				PathRemoveFileSpec(fileName);
				TCHAR iniFileName[MAX_PATH] = { 0 };
				strcpy(iniFileName, fileName);
				strcat(iniFileName, _T("\\SafeModeConfig.ini"));

				//读取配置文件
				TCHAR chTemp[256] = {0};
				WritePrivateProfileString(m_strIP, "BeginTime", m_safeModeInfo.szBeginTime, iniFileName);
				sprintf(chTemp, "%d", m_safeModeInfo.index);
				WritePrivateProfileString(m_strIP, "Index", chTemp, iniFileName);

				m_iBreakCount = 0;
			}
		}

		return 0;
	}

	int CHvDeviceAxCtrl::OnPlate(PVOID pUserData, DWORD dwCarID,
		LPCSTR pcPlateNo, LPCSTR pcAppendInfo,
		DWORD dwRecordType,
		DWORD64 dw64TimeMS )
	{

		if (m_fClosed)
		{
			return -1;
		}

		EnterCriticalSection(&m_csResult); //注意结果锁	

		//获取车牌字符串
		//CString strTmpPlate(pcPlateNo);
		m_resultData.resultFile.strPlateNo.Format("%s",pcPlateNo);

		//获取车牌信息级车辆ID和时间
		m_resultData.resultInfo.dw64Time =dw64TimeMS;
		m_resultData.resultInfo.dwCarID = dwCarID;

		//车牌追加信息
		int iPlateInfoBuffLen = 2099;
		char* pPlateInfo = new char[iPlateInfoBuffLen];

		DWORD32 dwTimeLowTemp, dwTimeHighTemp, dwCarIDTemp;

		char * pIllegalInfo = new char[255];
		pIllegalInfo[0] = '\0';
		GetPlateNumByXmlExtInfo(pcAppendInfo, pPlateInfo, &dwTimeLowTemp, &dwTimeHighTemp, &dwCarIDTemp, pIllegalInfo);
		m_dwTimes = ((DWORD64)(dwTimeHighTemp) << 32)|dwTimeLowTemp;
		m_resultData.resultFile.strAppendInfo.Format("%s", pPlateInfo);
		LeaveCriticalSection(&m_csResult);//注意结果锁

		EnterCriticalSection(&m_csIllegalReason);
		m_IllegalReason.Format("%s", pIllegalInfo);
		if(m_IllegalReason != "")
		{
			AddIllegalVideoList(dwCarID, dw64TimeMS);
		}
		LeaveCriticalSection(&m_csIllegalReason);
		delete[] pPlateInfo;
		delete[] pIllegalInfo;



		return 0;
	}

	int CHvDeviceAxCtrl::OnBigImage(PVOID pUserData, DWORD dwCarID,  
		WORD  wImgType, WORD  wWidth,
		WORD  wHeight, PBYTE pbPicData,
		DWORD dwImgDataLen,DWORD dwRecordType, 
		DWORD64 dw64TimeMS)
	{
		if (m_fClosed)
		{
			return -1;
		}



		if (m_fCapureFlag && m_iCaptureImgeSize == 0 )
		{
			if ( m_pCaputureImage != NULL)
			{
				delete[] m_pCaputureImage;
				m_pCaputureImage = NULL;
			}

			m_pCaputureImage = new BYTE[dwImgDataLen+1];
			memset(m_pCaputureImage, 0, dwImgDataLen+1);

			memcpy(m_pCaputureImage,  pbPicData, dwImgDataLen);
			m_iCaptureImgeSize = dwImgDataLen;
			return 0;
		}



		if (m_RecvSnapImageFlag)
		{

			EnterCriticalSection(&m_csResult); //注意结果锁

			LARGE_INTEGER  liTemp = {0};

			switch(wImgType)
			{
			case RECORD_BIGIMG_BEST_SNAPSHOT:
				m_resultData.resultInfo.rgBigImgSize[BEST_SNAP] = dwImgDataLen;
				m_resultData.resultFile.pastmBigImg[BEST_SNAP]->Seek(liTemp, SEEK_SET, NULL);
				m_resultData.resultFile.pastmBigImg[BEST_SNAP]->Write(pbPicData, m_resultData.resultInfo.rgBigImgSize[BEST_SNAP], NULL);	
				break;
			case RECORD_BIGIMG_LAST_SNAPSHOT:
				m_resultData.resultInfo.rgBigImgSize[LAST_SNAP] = dwImgDataLen;
				m_resultData.resultFile.pastmBigImg[LAST_SNAP]->Seek(liTemp, SEEK_SET, NULL);
				m_resultData.resultFile.pastmBigImg[LAST_SNAP]->Write(pbPicData, m_resultData.resultInfo.rgBigImgSize[LAST_SNAP], NULL);	
				break;
			case RECORD_BIGIMG_BEGIN_CAPTURE:
				m_resultData.resultInfo.rgBigImgSize[BEGIN_CAP] = dwImgDataLen;
				m_resultData.resultFile.pastmBigImg[BEGIN_CAP]->Seek(liTemp, SEEK_SET, NULL);
				m_resultData.resultFile.pastmBigImg[BEGIN_CAP]->Write(pbPicData, m_resultData.resultInfo.rgBigImgSize[BEGIN_CAP], NULL);	
				break;
			case RECORD_BIGIMG_BEST_CAPTURE:
				m_resultData.resultInfo.rgBigImgSize[BEST_CAP] = dwImgDataLen;
				m_resultData.resultFile.pastmBigImg[BEST_CAP]->Seek(liTemp, SEEK_SET, NULL);
				m_resultData.resultFile.pastmBigImg[BEST_CAP]->Write(pbPicData, m_resultData.resultInfo.rgBigImgSize[BEST_CAP], NULL);	
				break;
			case RECORD_BIGIMG_LAST_CAPTURE:
				m_resultData.resultInfo.rgBigImgSize[LAST_CAP] = dwImgDataLen;
				m_resultData.resultFile.pastmBigImg[LAST_CAP]->Seek(liTemp, SEEK_SET, NULL);
				m_resultData.resultFile.pastmBigImg[LAST_CAP]->Write(pbPicData, m_resultData.resultInfo.rgBigImgSize[LAST_CAP], NULL);	
				break;
			}

			LeaveCriticalSection(&m_csResult);//注意结果锁
		}

		return 0;
	}



	int CHvDeviceAxCtrl::OnSmall(PVOID pUserData, DWORD dwCarID,
		WORD wWidth, WORD wHeight,
		PBYTE pbPicData, DWORD dwImgDataLen,
		DWORD dwRecordType,
		DWORD64 dwTimeMS)
	{

		if (m_fClosed)
		{
			return -1;
		}

		//接收小图
		if (m_RecvPlateImageFlag )
		{
			EnterCriticalSection(&m_csResult); //注意结果锁

			m_resultData.resultInfo.iSmallImgSize=dwImgDataLen;		//m_resultData.resultFile.pastmSmallImg->Write(pbPicData, dwImgDataLen, NULL);

			LARGE_INTEGER  liTemp = {0};
			int iDataLen = 128 * 1024;
			PBYTE pByte = new BYTE[iDataLen];
			m_resultData.resultFile.pastmSmallImg->Seek(liTemp,SEEK_SET,NULL);
			HVAPIUTILS_SmallImageToBitmapEx(pbPicData, 
				wWidth, 
				wHeight,
				pByte,
				&iDataLen);
			m_resultData.resultFile.pastmSmallImg->Write(pByte, iDataLen, NULL);
			delete[] pByte;

			LeaveCriticalSection(&m_csResult);//注意结果锁
		}

		return 0;
	}

	int CHvDeviceAxCtrl::OnBinary(PVOID pUserData, DWORD dwCarID,
		WORD wWidth, WORD wHeight, 
		PBYTE pbPicData, DWORD dwImgDataLen,
		DWORD dwRecordType, 
		DWORD64 dwTimeMS)
	{
		if (m_fClosed)
		{
			return -1;
		}

		if (m_RecvPlateBinImageFlag )
		{
			EnterCriticalSection(&m_csResult); //注意结果锁
			if (m_resultData.resultFile.pbBinImg)
			{
				delete[] m_resultData.resultFile.pbBinImg;
				m_resultData.resultFile.pbBinImg = NULL;
			}

			m_resultData.resultFile.dwBinImgSize = dwImgDataLen;
			m_resultData.resultFile.pbBinImg = new BYTE[m_resultData.resultFile.dwBinImgSize];
			memcpy(m_resultData.resultFile.pbBinImg, pbPicData,  m_resultData.resultFile.dwBinImgSize);

			LeaveCriticalSection(&m_csResult);//注意结果锁
		}

		return 0;
	}

	int CHvDeviceAxCtrl::OnJpegFrame(PVOID pUserData, PBYTE pbImageData, DWORD dwImageDataLen, DWORD dwImageType, LPCSTR szImageExtInfo)
	{
		if(pUserData == NULL || pbImageData == NULL || szImageExtInfo == NULL 
			|| dwImageDataLen <= 0)
		{
			return -1;
		}
		if(dwImageType != IMAGE_TYPE_JPEG_NORMAL &&
			dwImageType != IMAGE_TYPE_JPEG_LPR)
		{
			return 0;
		}

#ifdef DEBUGOCX
		CString strDebug;
		strDebug.Format("%s 是不是空？", szImageExtInfo);
		OutputDebugStringA(szImageExtInfo);
#endif

		char pTime[20] = {0};
		const char *pTmp1, *pTmp2, *pTmp3;
		pTmp1 = strstr(szImageExtInfo, "FrameTime:");
		pTmp2 = strchr(pTmp1, ':');
		pTmp3 = strchr(pTmp2, ',');

		int iTimeLen = 0;
		if (pTmp3)
		{
			iTimeLen = (int)(strlen(pTmp2) - strlen(pTmp3) - 1);
		}
		else
		{
			iTimeLen = (int)strlen(pTmp2) - 1;
		}
		strncpy(pTime, (++pTmp2), iTimeLen);
		m_JpegBackTime = 0;
		sscanf(pTime, "%I64u", &m_JpegBackTime);

		CTime ctCur = CTime::GetCurrentTime();
		m_JpegBackTime = ctCur.GetTime();

		EnterCriticalSection(&m_csJpegData);

		BOOL fSetMessage = FALSE;
		if (m_RecvJpegFlag)
		{
			if (m_pbRecvJpegData != NULL)
			{
				delete[] m_pbRecvJpegData;
				m_pbRecvJpegData = NULL;
			}

			m_pbRecvJpegData = new BYTE[dwImageDataLen + 1];
			if (NULL == m_pbRecvJpegData)
			{
				LeaveCriticalSection(&m_csJpegData);
				return -1;
			}

			memcpy(m_pbRecvJpegData, pbImageData, dwImageDataLen);
			m_iJpegDataLen = dwImageDataLen;
			fSetMessage = TRUE;
		}
		else if (m_pbRecvJpegData != NULL)
		{
			delete[] m_pbRecvJpegData;
			m_pbRecvJpegData = NULL;
		}
		LeaveCriticalSection(&m_csJpegData);

		if (fSetMessage)
		{
			m_msgWnd.SendMessage(MSG_JPEGVIDEO_RESULT, (WPARAM)this, 0);
		}

		return 0;
	}

	int CHvDeviceAxCtrl::OnH264Frame(PVOID pUserData, PBYTE pbVideoData, DWORD dwVideoDataLen, DWORD dwVideoType, LPCSTR szVideoExtInfo)
	{
		if(pUserData == NULL || pbVideoData == NULL || szVideoExtInfo == NULL
			|| dwVideoDataLen <= 0)
		{
			return -1;
		}
		if(dwVideoType != VIDEO_TYPE_H264_NORMAL_I &&
			dwVideoType != VIDEO_TYPE_H264_NORMAL_P)
		{
			return 0;
		}

		if (m_fClosed)
		{
			if (m_VideoSave.m_fOpenFile)
			{
				m_VideoSave.H264VideoSaverClose();
			}
			return -1;
		}

		//获取视频的的长和宽
		if ( m_iH264Heigh ==0 || m_iH264Width == 0 )
		{
			char szTemp[1024] = {0};
			strcpy(szTemp, szVideoExtInfo);

			// 提取视频的长和宽
			char* pTempBuf = strstr(szTemp,"Width:");
			if (pTempBuf != NULL )
			{
				sscanf(pTempBuf, "Width:%d,", &m_iH264Width);
			}

			pTempBuf = strstr(szTemp, "Height:");
			if ( pTempBuf != NULL )
			{
				sscanf(pTempBuf, "Height:%d,", &m_iH264Heigh);
			}
			if (m_iH264Width == 0)
			{
				m_iH264Width = 1600;
			}

			if (m_iH264Heigh == 0)
			{
				m_iH264Heigh = 1200;
			}
		}


	//if (  m_iDeviceType != DEV_TYPE_HVVENUS )
	{
		if ( m_ShowH264VideoFlag )
		{
			try
			{
				//显示视频
				if (m_pShowMaxVideoDlg != NULL )
				{
					bool fVisible = ::IsWindowVisible(m_pShowMaxVideoDlg->m_hWnd);
					if (fVisible)
					{
						//m_fIsPlay = true;
						m_Decode.H264_Decode(pbVideoData, dwVideoDataLen,NULL, NULL,m_pShowMaxVideoDlg->m_hWnd);
					}
					else
					{
						//m_fIsPlay = true;
						m_Decode.H264_Decode(pbVideoData, dwVideoDataLen,NULL, NULL,m_hWnd);
					}
				}
				else
				{
					//m_fIsPlay = true;
					m_Decode.H264_Decode(pbVideoData, dwVideoDataLen,NULL, NULL,m_hWnd);
				}


				/*
#ifndef _DEBUG
				m_fIsPlay = true;
				UpdateData(FALSE);
				DSShowVideo(this->m_hWnd, m_iH264Width, m_iH264Heigh, (char*)pbVideoData, dwVideoDataLen);
				m_bDshow = true;
				//this->Refresh();
				UpdateData(TRUE);
#endif*/
			}
			catch (...)
			{
			}
		}
	}

		bool isH264Empty = false;
		EnterCriticalSection(&m_csH264Data);
		isH264Empty = m_strH264SavePath.IsEmpty();
		if(!DiskStatus(m_strH264SavePath))
		{
			isH264Empty = true;
		}
		LeaveCriticalSection(&m_csH264Data);
		if (m_RecvH264Flag && !isH264Empty)
		{
			char pTime[20] = {0};
			const char *pTmp1, *pTmp2, *pTmp3;

			pTmp1 = strstr(szVideoExtInfo, "FrameTime:");
			pTmp2 = strchr(pTmp1, ':');
			pTmp3 = strchr(pTmp2, ',');
			int iTimeLen = 0;
			if (pTmp3)
			{
				iTimeLen = (int)(strlen(pTmp2) - strlen(pTmp3) - 1);
			}
			else
			{
				iTimeLen = (int)strlen(pTmp2) - 1;
			}
			strncpy(pTime, (++pTmp2), iTimeLen);
			DWORD64 dw64Time = 0;
			sscanf(pTime, "%I64u", &dw64Time);
			DWORD64 dw64CurrentTime = dw64Time / 1000;
			time_t time((time_t)dw64CurrentTime);

			if (!m_VideoSave.m_fOpenFile)
			{
				CTime cTime(dw64CurrentTime);
				CString strVideoFile;
				CString tempTimeFormat = cTime.Format("%Y%m%d%H%M%S");
				EnterCriticalSection(&m_csH264Data);
				strVideoFile.Format("%s\\%s.avi", m_strH264SavePath, tempTimeFormat);
				if (m_iPathType == 1)
				{
					strVideoFile = "";
					strVideoFile = m_strH264SavePath;
				}
				LeaveCriticalSection(&m_csH264Data);
				bool fIFrame = false;
				if (VIDEO_TYPE_H264_NORMAL_I == dwVideoType)
				{
					MakeSureDirectoryPathExists(strVideoFile.GetBuffer());
					strVideoFile.ReleaseBuffer();
					m_VideoSave.H264VideoSaverOpen(strVideoFile.GetBuffer(), m_H264FrameRateNum, m_iH264Width, m_iH264Heigh);//m_VideoSave.H264VideoSaverOpen(strVideoFile.GetBuffer(), 12, 1600, 1200);
					strVideoFile.ReleaseBuffer();
					fIFrame = true;
					m_dwOpenAviTimes = dw64CurrentTime;
					m_strVideoH264File = strVideoFile;
				}

				if (m_VideoSave.m_fOpenFile)
				{
					m_VideoSave.H264VideoSaverWirteOneFrame(pbVideoData, dwVideoDataLen, fIFrame);
				}
			}
			else
			{
				bool fIFrame = false;
				if (VIDEO_TYPE_H264_NORMAL_I == dwVideoType)
				{
					fIFrame = true;
				}
				m_VideoSave.H264VideoSaverWirteOneFrame(pbVideoData, dwVideoDataLen, fIFrame);
			}

			if ((dw64CurrentTime - m_dwOpenAviTimes)  >= (DWORD64)(m_SaveH264VideoTime * 60) && m_VideoSave.m_fOpenFile)
			{
				m_VideoSave.H264VideoSaverClose();
				//LeaveCriticalSection(&m_csH264Data);
				m_msgWnd.SendMessage(MSG_H264VIDEO_RESULT, (WPARAM)this, 0);
			}
			return 0;
		}
		else
		{
			if (m_VideoSave.m_fOpenFile)
			{
				m_VideoSave.H264VideoSaverClose();
			}
		}
		//LeaveCriticalSection(&m_csH264Data);
		m_msgWnd.SendMessage(MSG_H264VIDEO_RESULT, (WPARAM)this, 0);
		return 1;
	}


	bool CHvDeviceAxCtrl::SendMsgForSetSaveH264()
	{
		if ( m_hHandle == NULL )
			return false;

		if ( m_iDeviceType != DEV_TYPE_HVVENUS )
		{
			return true;
		}


		if ( m_RecvH264Flag && m_strH264SavePath == _T("") )
		{


			DWORD dwConnStatus = CONN_STATUS_UNKNOWN;

			EnterCriticalSection(&m_csOpt);
			//判断H264连接是否断开
			HVAPI_GetConnStatusEx(m_hHandle, CONN_TYPE_VIDEO, &dwConnStatus);
			LeaveCriticalSection(&m_csOpt);


			if ( dwConnStatus == CONN_STATUS_DISCONN || dwConnStatus == CONN_STATUS_UNKNOWN )  //如果不存在的， 发送模拟信息
			{
				m_msgWnd.SendMessage(MSG_H264VIDEO_RESULT, (WPARAM)this, 0);
			}
			else  //如果存在着连接， 断开
			{
				HVAPI_SetCallBackEx(m_hHandle, NULL,NULL, 0, CALLBACK_TYPE_H264_VIDEO, NULL);
			}

		
			if ( m_strH264SavePath != _T(""))
			{
				EnterCriticalSection(&m_csOpt);
				HRESULT iReturnValue = HVAPI_SetCallBackEx(m_hHandle, CHvDeviceAxCtrl::OnH264FrameProxy,this, 0, CALLBACK_TYPE_H264_VIDEO, NULL);
				if (-1 == iReturnValue)
				{
					HVAPI_CloseEx(m_hHandle);
					m_hHandle = NULL; 
				}
				LeaveCriticalSection(&m_csOpt);
			}
		}

		return true;

	}

	int CHvDeviceAxCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (COleControl::OnCreate(lpCreateStruct) == -1)
			return -1;

		CreateStreamOnHGlobal(NULL, TRUE, &m_resultData.resultFile.pastmSmallImg);
		for (int i = 0; i < 5 ; i++)
		{
			CreateStreamOnHGlobal(NULL, TRUE, &(m_resultData.resultFile.pastmBigImg[i]));
		}

		if (!m_msgWnd.Create(_T("STATIC"), _T(""), WS_CHILD, CRect(0, 0, 0, 0), this->GetDesktopWindow(), 0))
		{
			MessageBox("m_msgWnd.Create failed");
		}

		m_fClosed = false;
		m_VideoSave.H264VideoSaverInit();
		m_IllegalVideoSave.H264VideoSaverInit();
		CVideoMemory::H264VideoCodeInit();

		return 0;
	}

	LONGLONG CHvDeviceAxCtrl::GetPlateResultTime()
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		DWORD64 dwTime = 0;
		EnterCriticalSection(&m_csResult);
		dwTime = m_dwTimes;
		LeaveCriticalSection(&m_csResult);
		return dwTime;
	}

	BSTR CHvDeviceAxCtrl::GetFileName(SHORT iType)
	{
		static BSTR strFileName = NULL;
		if(strFileName != NULL)
		{
			SysFreeString(strFileName);
		}
		if (iType == 0)
		{
			EnterCriticalSection(&m_csResult);
			strFileName = m_strRecordFile.AllocSysString();
			LeaveCriticalSection(&m_csResult);
		}
		else if (iType == 1)
		{
			EnterCriticalSection(&m_csJpegData);
			strFileName = m_strVideoH264File.AllocSysString();
			LeaveCriticalSection(&m_csJpegData);
		}
		else if (iType == 2)
		{
			EnterCriticalSection(&m_csH264Data);
			strFileName = m_strVideoJPEGFile.AllocSysString();
			LeaveCriticalSection(&m_csH264Data);
		}
		else if(iType == 3)
		{
			EnterCriticalSection(&m_csIllegalVideoPath);
			strFileName = m_IllegalVideoPath.AllocSysString();
			LeaveCriticalSection(&m_csIllegalVideoPath);
		}
		return strFileName;
	}

	LONG CHvDeviceAxCtrl::SetPathType(SHORT iPathtType)
	{
		m_iPathType = iPathtType;
		return 0;
	}


	LONG CHvDeviceAxCtrl::SoftTriggerCaptureAndForceGetResult(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		// TODO: Add your dispatch handler code here
		//MessageBox(_T("SoftTriggerCaptureAndForceGetResult"));

		if (m_fClosed)
		{
			return -1;
		}

		CString strBuf = "SoftTriggerCapture";//CString strBuf = "ForceSend";
		char szRetBuf[128*1024] = {0};
		INT nRetLen = 0;

#ifdef DEBUGOCX
		CString strDebug;
		strDebug.Format("m_hhandle = %d", m_hHandle);
		OutputDebugStringA( strDebug);

		strDebug.Format("strBuf = %s", strBuf);
		OutputDebugStringA( strDebug);

		strDebug.Format("szRetBuf = %d", szRetBuf);
		OutputDebugStringA( strDebug);

		strDebug.Format("m_hhandle = %d", m_hHandle);
		OutputDebugStringA( strDebug);
#endif

		return HVAPI_ExecCmdEx(m_hHandle, strBuf, szRetBuf, sizeof(szRetBuf), &nRetLen);
	}

	LONG CHvDeviceAxCtrl::SendTriggerOut(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		// TODO: Add your dispatch handler code here
		//MessageBox(_T("12V SendTriggerOut "));
		if (m_fClosed)
		{
			return -1;
		}

		CString strBuf = "SendTriggerOut";
		char szRetBuf[128*1024] = {0};
		INT nRetLen = 0;

#ifdef DEBUGOCX
		CString strDebug;
		strDebug.Format("m_hhandle = %d", m_hHandle);
		OutputDebugStringA( strDebug);

		strDebug.Format("strBuf = %s", strBuf);
		OutputDebugStringA( strDebug);

		strDebug.Format("szRetBuf = %d", szRetBuf);
		OutputDebugStringA( strDebug);

		strDebug.Format("m_hhandle = %d", m_hHandle);
		OutputDebugStringA( strDebug);
#endif

		return HVAPI_ExecCmdEx(m_hHandle, strBuf, szRetBuf, sizeof(szRetBuf), &nRetLen);
	}

	void CHvDeviceAxCtrl::OnH264FrameRateNumChanged(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		// TODO: Add your property handler code here

		SetModifiedFlag();
	}

	LONG CHvDeviceAxCtrl::GetH264FrameRateNum(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		// TODO: Add your dispatch handler code here
		return m_H264FrameRateNum;	
	}

	LONG CHvDeviceAxCtrl::SetH264FrameRateNum(LONG longH264FrameRateNum)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		// TODO: Add your dispatch handler code here

#ifdef DEBUGOCX
		CString strDebug;
		strDebug.Format("m_RecvH264Flag = %d", m_RecvH264Flag);
		OutputDebugStringA( strDebug);
#endif

		if (longH264FrameRateNum >=5  &&  longH264FrameRateNum  <61  )
		{
			if ( 0 == m_RecvH264Flag )
			{
				m_H264FrameRateNum=longH264FrameRateNum;
				return 0;
			}
			else
			{
				m_H264FrameRateNum=25;
				return -1;
			}		
		}
		else
		{
			m_H264FrameRateNum=25;
			return -1;
		}	
	}

	void CHvDeviceAxCtrl::OnShowH264VideoFlagChanged(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		SetModifiedFlag();
	}

	LONG CHvDeviceAxCtrl::GetSaveFileStatus(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		// TODO: Add your dispatch handler code here

		return 0;
	}

	LONG CHvDeviceAxCtrl::InportNameListEx(LPCTSTR szWhiteNameList, LONG iWhiteListLen, LPCTSTR szBlackNameList, LONG iBlackListLen)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		// TODO: Add your dispatch handler code here
		if (m_fClosed)
		{
			return -1;
		}

		return HVAPI_InportNameList(m_hHandle, (char*)szWhiteNameList, iWhiteListLen, (char*)szBlackNameList, iBlackListLen);

		return 0;
	}

	BSTR CHvDeviceAxCtrl::GetNameListEx(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CString strResult;

		// TODO: Add your dispatch handler code here
		if (m_fClosed)
		{
			return strResult.AllocSysString();
		}

		if ( m_hHandle == NULL )
			return strResult.AllocSysString();

		int bufferLen = 1024 * 1024;
		char *szWhileName = new char[bufferLen];
		char *szBlackName = new char[bufferLen];
		memset(szWhileName,0, bufferLen);
		memset(szBlackName, 0, bufferLen);
		int iWhiteListLen = bufferLen;
		int iBlackListLen = bufferLen;

		if ( HVAPI_GetNameList(m_hHandle, szWhileName, &iWhiteListLen, szBlackName, &iBlackListLen) == S_OK)
		{
			strResult.Format("WhileNameList[%s],BlackNameList[%s]", szWhileName, szBlackName);
		}

		if (szWhileName != NULL)
		{
			delete[] szWhileName;
			szWhileName = NULL;
		}

		if (szBlackName != NULL)
		{
			delete[] szBlackName;
			szBlackName = NULL;
		}

		return strResult.AllocSysString();
	}

	LONG CHvDeviceAxCtrl::InportNameList(CHAR* szWhiteNameList, LONG iWhiteListLen, CHAR* szBlackNameList, LONG iBlackListLen)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		// TODO: Add your dispatch handler code here
		if (m_fClosed)
		{
			return -1;
		}

		return HVAPI_InportNameList(m_hHandle, szWhiteNameList, iWhiteListLen, szBlackNameList, iBlackListLen);
	}

	LONG CHvDeviceAxCtrl::GetNameList(CHAR* szWhiteNameList, LONG* iWhiteListLen, CHAR* szBlackNameList, LONG* iBlackListLen)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		// TODO: Add your dispatch handler code here
		if (m_fClosed)
		{
			return -1;
		}

		return HVAPI_GetNameList(m_hHandle, szWhiteNameList, (int*)iWhiteListLen, szBlackNameList, (int*)iBlackListLen);
	}

	LONG CHvDeviceAxCtrl::TriggerSignal(LONG VideoID)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		// TODO: Add your dispatch handler code here
		if (m_fClosed)
		{
			return -1;
		}

		if (VideoID > 1)
		{
			VideoID = 1;
		}
		else if (VideoID < 0)
		{
			VideoID = 0;
		}

		return HVAPI_TriggerSignal(m_hHandle, VideoID);
	}

	LONG CHvDeviceAxCtrl::TriggerAlarmSignal(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		// TODO: Add your dispatch handler code here
		if (m_fClosed)
		{
			return -1;
		}

		return HVAPI_TriggerAlarmSignal(m_hHandle);
	}

	LONG CHvDeviceAxCtrl::SaveIllegalVideo(LPCTSTR strFilePath)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		// TODO: 在此添加调度处理程序代码
		if(!DiskStatus(strFilePath))
		{
			return -10;
		}
		CTime PlateTime;
		EnterCriticalSection(&m_csResult);
		DWORD CarID = m_resultData.resultInfo.dwCarID;
		PlateTime = CTime(m_resultData.resultInfo.dw64Time / 1000);
		LeaveCriticalSection(&m_csResult);

		CString filePath;
		filePath.Format("%s", strFilePath);
		DWORD64 time = AddIllegalVideoList(CarID, filePath);

		//生成最终保存的文件名，该位置命令应该与DealIllegalVideoList()中的一致
		CString temp_Path;
		temp_Path.Format("%s%s\\",strFilePath ,PlateTime.Format("%Y%m%d"));
		EnterCriticalSection(&m_csIllegalVideoPath);
		if(m_iPathType == 1)
		{
			m_IllegalVideoPath = "";
			m_IllegalVideoPath.Format("%s",strFilePath);
		}
		else
		{
			m_IllegalVideoPath.Format("%s%s%s%d%s", temp_Path, PlateTime.Format("%Y%m%d%H%M%S"),"_", CarID, ".avi");
		}
		MakeSureDirectoryPathExists(m_IllegalVideoPath.GetBuffer());
		m_IllegalVideoPath.ReleaseBuffer();
		LeaveCriticalSection(&m_csIllegalVideoPath);
		return 0;
	}

	BSTR CHvDeviceAxCtrl::GetIllegalReason(void)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CString strResult;

		// TODO: 在此添加调度处理程序代码
		EnterCriticalSection(&m_csIllegalReason);
		strResult = m_IllegalReason;
		LeaveCriticalSection(&m_csIllegalReason);

		return strResult.AllocSysString();
	}

	int CHvDeviceAxCtrl::AddIllegalVideoList(DWORD dwCarID, DWORD64 dw64PlateTime)
	{
		IllegalVideoList::iterator i;
		bool isFind = false;
		EnterCriticalSection(&m_csIllegalVideoList);
		for(i = m_IllegalVideoList.begin(); i != m_IllegalVideoList.end(); ++i)
		{
			if((*i)->dwCarID == dwCarID)
			{
				isFind = true;
				(*i)->dw64PlateTime = dw64PlateTime;
				break;
			}
		}
		if(!isFind)
		{
			IllegalVideoInfo *pRet = new IllegalVideoInfo();
			pRet->dwCarID = dwCarID;
			pRet->dw64PlateTime = dw64PlateTime;
			m_IllegalVideoList.push_back(pRet);
		}
		LeaveCriticalSection(&m_csIllegalVideoList);
		return 0;
	}

	DWORD64 CHvDeviceAxCtrl::AddIllegalVideoList(DWORD dwCarID, CString filePath)
	{
		DWORD64 ret = 0;
		IllegalVideoList::iterator i;
		bool isFind = false;
		EnterCriticalSection(&m_csIllegalVideoList);
		for(i = m_IllegalVideoList.begin(); i != m_IllegalVideoList.end(); ++i)
		{
			if((*i)->dwCarID == dwCarID)
			{
				isFind = true;
				(*i)->strFilePath = filePath;
				ret = (*i)->dw64PlateTime;
				break;
			}
		}
		if(!isFind)
		{
			IllegalVideoInfo *pRet = new IllegalVideoInfo();
			pRet->dwCarID = dwCarID;
			pRet->strFilePath = filePath;
			m_IllegalVideoList.push_back(pRet);
		}
		LeaveCriticalSection(&m_csIllegalVideoList);
		return ret;
	}

	int CHvDeviceAxCtrl::AddIllegalVideoList(DWORD dwCarID, CVideoMemory* pVideo)
	{
		IllegalVideoList::iterator i;
		bool isFind = false;
		EnterCriticalSection(&m_csIllegalVideoList);
		for(i = m_IllegalVideoList.begin(); i != m_IllegalVideoList.end(); ++i)
		{
			if((*i)->dwCarID == dwCarID)
			{
				isFind = true;
				(*i)->pVideoMemory = pVideo;
				break;
			}
		}
		if(!isFind)
		{
			IllegalVideoInfo *pRet = new IllegalVideoInfo();
			pRet->dwCarID = dwCarID;
			pRet->pVideoMemory = pVideo;
			m_IllegalVideoList.push_back(pRet);
		}
		LeaveCriticalSection(&m_csIllegalVideoList);
		return 0;
	}

	int CHvDeviceAxCtrl::DealIllegalVideoList()
	{
		IllegalVideoList::iterator i;
		CTime PlateTime;
		CString temp_Path;
		CString file_name;
		CTimeSpan timeSpan;
		CTime nowTime(CTime::GetCurrentTime());
		if(m_IllegalVideoList.empty())
		{
			return 0;
		}
		EnterCriticalSection(&m_csIllegalVideoList);
		for(i = m_IllegalVideoList.begin(); i != m_IllegalVideoList.end();)
		{
			if((*i)->dw64PlateTime != 0 && (*i)->strFilePath != "" && (*i)->pVideoMemory != NULL)
			{
				m_IllegalVideoListSave.push_back(*i);
				i = m_IllegalVideoList.erase(i);
			}
			else  
			{
				timeSpan = nowTime - (*i)->CreatTime;
				if(timeSpan.GetSeconds() > 15)
				{
					if((*i)->pVideoMemory != NULL)
					{
						(*i)->pVideoMemory->ClearVideoFrame();
						delete (*i)->pVideoMemory;
						(*i)->pVideoMemory = NULL;
					}
					(*i)->strFilePath.Empty();
					delete *i;
					i = m_IllegalVideoList.erase(i);
				}
				else
				{
					++i;
				}
			}
		}
		LeaveCriticalSection(&m_csIllegalVideoList);
		//保存处理
		IllegalVideoInfo *pRet = NULL;
		while(!m_IllegalVideoListSave.empty())
		{
			pRet = m_IllegalVideoListSave.front();
			m_IllegalVideoListSave.pop_front();

			if(m_iPathType == 1)
			{
				file_name.Format("%s",pRet->strFilePath);
			}
			else
			{
				//此处生成的文件名，应与SaveIllegalVideo()一致
				PlateTime = CTime((pRet->dw64PlateTime) / 1000);
				temp_Path.Format("%s%s\\",pRet->strFilePath,PlateTime.Format("%Y%m%d"));
				file_name.Format("%s%s%s%d%s", temp_Path, PlateTime.Format("%Y%m%d%H%M%S"),"_",pRet->dwCarID, ".avi");
			}
			pRet->pVideoMemory->SaveToDisk(file_name.GetBuffer(), 25);
			file_name.ReleaseBuffer();
			
			if(pRet->pVideoMemory != NULL)
			{
				pRet->pVideoMemory->ClearVideoFrame();
				delete pRet->pVideoMemory;
				pRet->pVideoMemory = NULL;
			}
			pRet->strFilePath.Empty();
			delete pRet;
			pRet = NULL;
		}
		return 0;
	}

	int CHvDeviceAxCtrl::ClearIllegalVideoList()
	{
		IllegalVideoInfo *pRet = NULL;
		EnterCriticalSection(&m_csIllegalVideoList);
		while(!m_IllegalVideoList.empty())
		{
			pRet = m_IllegalVideoList.front();
			m_IllegalVideoList.pop_front();

			pRet->pVideoMemory->ClearVideoFrame();
			if(pRet->pVideoMemory)
			{
				delete pRet->pVideoMemory;
				pRet->pVideoMemory = NULL;
			}
			pRet->strFilePath.Empty();
			delete pRet;
		}
		LeaveCriticalSection(&m_csIllegalVideoList);
		return 0;
	}


	/*
	void CHvDeviceAxCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
	{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

		//	CWnd *pWnd = FromHandle(m_hWnd);
		if (m_bFullScreen)
		{
			LockWindowUpdate();
			::SetParent(m_hWnd, m_hWndParent);
			SetWindowPlacement(&m_temppl);
			::SetForegroundWindow(m_hWndParent);
			::SetFocus(m_hWndParent);
			::SetFocus(m_hWnd);
			UnlockWindowUpdate();
		}
		else
		{  
			GetWindowPlacement(&m_temppl);
			int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
			int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
			m_hWndParent = ::GetParent(m_hWnd);
			::SetParent(m_hWnd, GetDesktopWindow()->m_hWnd);
			WINDOWPLACEMENT wp1;
			ZeroMemory(&wp1, sizeof(WINDOWPLACEMENT));
			wp1.length = sizeof(WINDOWPLACEMENT);
			wp1.showCmd = SW_SHOWNORMAL;
			wp1.rcNormalPosition.left = 0;
			wp1.rcNormalPosition.top = 0;
			wp1.rcNormalPosition.right = nScreenWidth;
			wp1.rcNormalPosition.bottom = nScreenHeight;
			SetWindowPlacement(&wp1);
			::SetForegroundWindow(GetDesktopWindow()->m_hWnd);
			::SetForegroundWindow(m_hWnd);
		}
		m_bFullScreen = !m_bFullScreen;

		COleControl::OnLButtonDblClk(nFlags, point);
}*/

/*
	LRESULT CHvDeviceAxCtrl::DestroyMaxVideoWnd(WPARAM a,LPARAM b)
	{
		Sleep(500);
		return 0;
	}*/


	void CHvDeviceAxCtrl::ShowVideoNomal()
	{
		if (m_bFullScreen)
		{
			m_bFullScreen = false;
		}
	}

	void CHvDeviceAxCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
	{
		// TODO: 在此添加消息处理程序代码和/或调用默认值
		/*
		if (m_bFullScreen)
		{
			m_bFullScreen = !m_bFullScreen;
			//LockWindowUpdate();
			//::SetParent(m_hWnd, m_hWndParent);
			//SetWindowPlacement(&m_temppl);
			//::SetForegroundWindow(m_hWndParent);
			//::SetFocus(m_hWndParent);
			//::SetFocus(m_hWnd);

			if (m_pShowMaxVideoDlg!= NULL)
			{
				//m_pShowMaxVideoDlg->ShowWindow(SW_HIDE);
				
			}
			//UnlockWindowUpdate();	
		}
		else*/
		{  

			//GetWindowPlacement(&m_temppl);
			//int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
			//int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);


			
			/*
			if(m_pShowMaxVideoDlg!= NULL )
			{
				m_pShowMaxVideoDlg->DestroyWindow();
				delete m_pShowMaxVideoDlg;
				m_pShowMaxVideoDlg = NULL;
			}
			*/

			if (m_pShowMaxVideoDlg== NULL )
			{
				m_pShowMaxVideoDlg = new CVideoMaxShowDlg();
				m_pShowMaxVideoDlg->Create(IDD_DIALOG_MAXSHOWVIDEO, this);
			}

			m_bFullScreen = true;
			m_pShowMaxVideoDlg->ShowWindow(SW_MAXIMIZE);
			
			

			//m_hWndParent = ::GetParent(m_hWnd);
			//::SetParent(m_hWnd, GetDesktopWindow()->m_hWnd);
			//::SetParent(m_hWnd, m_pShowMaxVideoDlg->m_hWnd);
			//::SetWindowPos(m_hWnd,NULL,0, 0,nScreenWidth,nScreenHeight,SWP_NOZORDER);
			//SetControlSize(nScreenWidth,nScreenHeight);
			//Invalidate();

			//::MoveWindow( m_hWnd, 0, 0, nScreenWidth,nScreenHeight, true);


			//WINDOWPLACEMENT wp1;
			//ZeroMemory(&wp1, sizeof(WINDOWPLACEMENT));
			//wp1.length = sizeof(WINDOWPLACEMENT);
			//wp1.showCmd = SW_SHOWNORMAL;
			//wp1.rcNormalPosition.left = 0;
			//wp1.rcNormalPosition.top = 0;
			//wp1.rcNormalPosition.right =  nScreenWidth;
			//wp1.rcNormalPosition.bottom =  nScreenHeight;
			//SetWindowPlacement(&wp1);
			//::SetForegroundWindow(GetDesktopWindow()->m_hWnd);
			//::SetForegroundWindow(m_pShowMaxVideoDlg->m_hWnd);
			//::SetForegroundWindow(m_hWnd);
			
			//m_pShowMaxVideoDlg->ShowWindow(SW_MAXIMIZE);

			//m_pShowMaxVideoDlg->ShowWindow(SW_SHOWNORMAL);
		}
		//m_bFullScreen = !m_bFullScreen;

		COleControl::OnLButtonDblClk(nFlags, point);
	}

	/*
	//Live55 接收RTSP流视频回调函数
	void CHvDeviceAxCtrl::SLWRtspStreamBack(unsigned char* frame,int frameSize,FRAME_HEAD_S frame_head)
	{
		int iWidth = 0;
		int iHeigh = 0;

		if (m_ShowH264VideoFlag )
		{
			//显示视频
			if (m_pShowMaxVideoDlg != NULL )
			{
				bool fVisible = ::IsWindowVisible(m_pShowMaxVideoDlg->m_hWnd);
				if (fVisible)
				{
					m_Decode.H264_Decode(frame, frameSize,&iWidth, &iHeigh,m_pShowMaxVideoDlg->m_hWnd);
				}
				else
				{
					m_Decode.H264_Decode(frame, frameSize,&iWidth, &iHeigh,m_hWnd);
				}
			}
			else
			{
				m_Decode.H264_Decode(frame, frameSize,&iWidth, &iHeigh,m_hWnd);
			}

			m_iH264Width = iWidth;
			m_iH264Heigh = iHeigh;
		}
		else
		{
			if (m_iH264Width==0 || m_iH264Heigh  == 0 )
			{
				m_Decode.H264_Decode(frame, frameSize,&iWidth, &iHeigh,NULL);
				m_iH264Width = iWidth;
				m_iH264Heigh = iHeigh;
			}
		}


		
		

		//保存视频
		bool isH264Empty = false;
		EnterCriticalSection(&m_csH264Data);
		isH264Empty = m_strH264SavePath.IsEmpty();
		if(!DiskStatus(m_strH264SavePath))
		{
			isH264Empty = true;
		}
		LeaveCriticalSection(&m_csH264Data);
		if (m_RecvH264Flag && !isH264Empty)
		{
			DWORD64 dw64CurrentTime = (DWORD64)frame_head.TimeStamp /1000;
			time_t time((time_t)dw64CurrentTime);

			if (!m_VideoSave.m_fOpenFile)
			{
				CTime cTime(time);
				CString strVideoFile;
				CString tempTimeFormat = cTime.Format("%Y%m%d%H%M%S");
				EnterCriticalSection(&m_csH264Data);
				strVideoFile.Format("%s\\%s.avi", m_strH264SavePath, tempTimeFormat);
				if (m_iPathType == 1)
				{
					strVideoFile = "";
					strVideoFile = m_strH264SavePath;
				}
				LeaveCriticalSection(&m_csH264Data);
				bool fIFrame = false;
				 
				if ( frame_head.u16FrameType == 1 )  //I 帧
				{
					MakeSureDirectoryPathExists(strVideoFile.GetBuffer());
					strVideoFile.ReleaseBuffer();
					m_VideoSave.H264VideoSaverOpen(strVideoFile.GetBuffer(), m_H264FrameRateNum, m_iH264Width, m_iH264Heigh);//m_VideoSave.H264VideoSaverOpen(strVideoFile.GetBuffer(), 12, 1600, 1200);
					strVideoFile.ReleaseBuffer();
					fIFrame = true;
					m_dwOpenAviTimes = dw64CurrentTime;
					m_strVideoH264File = strVideoFile;
				}

				if (m_VideoSave.m_fOpenFile)
				{
					m_VideoSave.H264VideoSaverWirteOneFrame(frame, frameSize, fIFrame);
				}
			}
			else
			{
				bool fIFrame = false;
				if (frame_head.u16FrameType == 1)
				{
					fIFrame = true;
				}
				m_VideoSave.H264VideoSaverWirteOneFrame(frame, frameSize, fIFrame);
			}

			if ((dw64CurrentTime - m_dwOpenAviTimes)  >= (DWORD64)(m_SaveH264VideoTime * 60) && m_VideoSave.m_fOpenFile)
			{
				m_VideoSave.H264VideoSaverClose();
				
				m_msgWnd.SendMessage(MSG_H264VIDEO_RESULT, (WPARAM)this, 0);
			}
			return ;
		}
		else
		{
			if (m_VideoSave.m_fOpenFile)
			{
				m_VideoSave.H264VideoSaverClose();
			}
		}
		
		m_msgWnd.SendMessage(MSG_H264VIDEO_RESULT, (WPARAM)this, 0);

		return ;
	}*/



	HRESULT CHvDeviceAxCtrl::GetCaptureImage(int iVideoID, LPCTSTR strFileName)
	{
		if (m_fClosed)
		{
			return -1;
		}



		EnterCriticalSection(&m_csOpt);
		if ( m_hHandle == NULL )
		{
			LeaveCriticalSection(&m_csOpt);
			return S_FALSE;
		}

		
		if (iVideoID > 1)
		{
			iVideoID = 1;
		}
		else if (iVideoID < 0)
		{
			iVideoID = 0;
		}


		if (m_GetImage != NULL )
		{
			long iRet = m_GetImage( m_hHandle, iVideoID, strFileName);
			
			if ( iRet != S_OK )
				iRet = S_FALSE;

			LeaveCriticalSection(&m_csOpt);

			return iRet;

		}

		HRESULT hr = SoftTriggerCaptureAndForceGetResult();
		m_fCapureFlag = true;

		LeaveCriticalSection(&m_csOpt);

		if (hr != S_OK)
		{
			return S_FALSE;
		}
		else
		{
			//等待结果到来
			int iTryCount = 0;
			while( m_fCapureFlag && iTryCount<10 )
			{
				Sleep(300);
				iTryCount++;
			}
			m_fCapureFlag = false;

			HRESULT  hr = S_OK;

			if (m_iCaptureImgeSize != 0 && m_pCaputureImage != NULL )
			{
				//保存图片到硬盘
				CString file_name = "";
				file_name.Format("%s",strFileName);
				MakeSureDirectoryPathExists(file_name.GetBuffer());
				file_name.ReleaseBuffer();

				FILE *myFile = fopen(strFileName, "wb");
				if ( myFile != NULL )
				{
					fwrite(m_pCaputureImage, 1, m_iCaptureImgeSize, myFile);
					fclose(myFile);
					myFile = NULL;
				}
				else
				{
					 hr = S_FALSE;
				}
			}
			else
			{
				 hr = S_FALSE;
			}

			m_iCaptureImgeSize = 0;

			if (m_pCaputureImage!=NULL)
			{
				delete[] m_pCaputureImage;
				m_pCaputureImage = NULL;
			}

			return hr;
		}

		return S_OK;
	}


/*
	// Implementation of IObjectSafety  
STDMETHODIMP CHvDeviceAxCtrl::XObjectSafety::GetInterfaceSafetyOptions(  
    REFIID riid,  
    DWORD __RPC_FAR *pdwSupportedOptions,  
    DWORD __RPC_FAR *pdwEnabledOptions)  
{  
    METHOD_PROLOGUE_EX(CHvDeviceAxCtrl, ObjectSafety);  
    if (!pdwSupportedOptions || !pdwEnabledOptions)  
    {  
        return E_POINTER;  
    }  
    *pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;  
    *pdwEnabledOptions = 0;  
    if (NULL == pThis->GetInterface(&riid))  
    {  
        TRACE("Requested interface is not supported.\n");  
        return E_NOINTERFACE;  
    }  
    // What interface is being checked out anyhow?  
    OLECHAR szGUID[39];  
    int i = StringFromGUID2(riid, szGUID, 39);  
    if (riid == IID_IDispatch)  
    {  
        // Client wants to know if object is safe for scripting  
        *pdwEnabledOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER;  
        return S_OK;  
    }  
    else if (riid == IID_IPersistPropertyBag  
            || riid == IID_IPersistStreamInit  
            || riid == IID_IPersistStorage  
            || riid == IID_IPersistMemory)  
    {  
        // Those are the persistence interfaces COleControl derived controls support  
        // as indicated in AFXCTL.H  
        // Client wants to know if object is safe for initializing from persistent data  
        *pdwEnabledOptions = INTERFACESAFE_FOR_UNTRUSTED_DATA;  
        return S_OK;  
    }  
    else  
    {  
        // Find out what interface this is, and decide what options to enable  
        TRACE("We didn't account for the safety of this interface, and it's one we support\n");  
        return E_NOINTERFACE;  
    }  
}  
  
STDMETHODIMP CHvDeviceAxCtrl::XObjectSafety::SetInterfaceSafetyOptions(  
    REFIID riid,  
    DWORD dwOptionSetMask,  
    DWORD dwEnabledOptions)  
{  
    METHOD_PROLOGUE_EX(CHvDeviceAxCtrl, ObjectSafety);  
    OLECHAR szGUID[39];  
    // What is this interface anyway?  
    // We can do a quick lookup in the registry under HKEY_CLASSES_ROOT\Interface  
    int i = StringFromGUID2(riid, szGUID, 39);  
    if (0 == dwOptionSetMask && 0 == dwEnabledOptions)  
    {  
        // the control certainly supports NO requests through the specified interface  
        // so it"s safe to return S_OK even if the interface isn"t supported.  
        return S_OK;  
    }  
  
    // Do we support the specified interface?  
    if (NULL == pThis->GetInterface(&riid))  
    {  
        TRACE1("%s is not support.\n", szGUID);  
        return E_FAIL;  
    }  
  
    if (riid == IID_IDispatch)  
    {  
        TRACE("Client asking if it's safe to call through IDispatch.\n");  
            TRACE("In other words, is the control safe for scripting?\n");  
        if (INTERFACESAFE_FOR_UNTRUSTED_CALLER == dwOptionSetMask && INTERFACESAFE_FOR_UNTRUSTED_CALLER == dwEnabledOptions)  
        {  
            return S_OK;  
        }  
        else  
        {  
            return E_FAIL;  
        }  
    }  
    else if (riid == IID_IPersistPropertyBag  
        || riid == IID_IPersistStreamInit  
        || riid == IID_IPersistStorage  
        || riid == IID_IPersistMemory)  
    {  
        TRACE("Client asking if it's safe to call through IPersist*.\n");  
            TRACE("In other words, is the control safe for initializing from persistent data?\n");  
        if (INTERFACESAFE_FOR_UNTRUSTED_DATA == dwOptionSetMask && INTERFACESAFE_FOR_UNTRUSTED_DATA == dwEnabledOptions)  
        {  
            return NOERROR;  
        }  
        else  
        {  
            return E_FAIL;  
        }  
    }  
    else  
    {  
        TRACE1("We didn\"t account for the safety of %s, and it\"s one we support\n", szGUID);  
        return E_FAIL;  
    }  
}  
  
STDMETHODIMP_(ULONG) CHvDeviceAxCtrl::XObjectSafety::AddRef()  
{  
    METHOD_PROLOGUE_EX_(CHvDeviceAxCtrl, ObjectSafety)  
    return (ULONG)pThis->ExternalAddRef();  
}  
  
STDMETHODIMP_(ULONG) CHvDeviceAxCtrl::XObjectSafety::Release()  
{  
    METHOD_PROLOGUE_EX_(CHvDeviceAxCtrl, ObjectSafety)  
    return (ULONG)pThis->ExternalRelease();  
}  
  
STDMETHODIMP CHvDeviceAxCtrl::XObjectSafety::QueryInterface(REFIID iid, LPVOID* ppvObj)  
{  
    METHOD_PROLOGUE_EX_(CHvDeviceAxCtrl, ObjectSafety)  
    return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);  
}  
*/

void CHvDeviceAxCtrl::OnHideIconChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	InvalidateControl();
	SetModifiedFlag();

}

BSTR CHvDeviceAxCtrl::GetDevSN()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	//MessageBox(_T("SoftTriggerCaptureAndForceGetResult"));
	CString strResult= "";
	if (!m_fClosed)
	{
		CString strBuf = "GetDevBasicInfo";//CString strBuf = "ForceSend";
		char szRetBuf[128*1024] = {0};
		INT nRetLen = 0;
		if(HVAPI_ExecCmdEx(m_hHandle, strBuf, szRetBuf, sizeof(szRetBuf), &nRetLen)==S_OK)
		{
			char szSN[64] = {0};
			HVAPIUTILS_GetExeCmdRetInfoEx(TRUE, szRetBuf, "GetDevBasicInfo", "SN", szSN);
			strResult = szSN;
		}
	}
	
	return strResult.AllocSysString();
}


BSTR CHvDeviceAxCtrl::GetDevMac(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult= "";
	if (!m_fClosed)
	{
		CString strBuf = "GetDevBasicInfo";
		char szRetBuf[128*1024] = {0};
		INT nRetLen = 0;
		if(HVAPI_ExecCmdEx(m_hHandle, strBuf, szRetBuf, sizeof(szRetBuf), &nRetLen)==S_OK)
		{
			char szMac[64] = {0};
			HVAPIUTILS_GetExeCmdRetInfoEx(TRUE, szRetBuf, "GetDevBasicInfo", "Mac", szMac);
			strResult = szMac;
		}
	}
	return strResult.AllocSysString();
}


BSTR CHvDeviceAxCtrl::GetCusTomInfo(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult = "";

	// TODO: 在此添加调度处理程序代码

	if (m_fClosed)
	{
		return strResult.AllocSysString();
	}
	
	EnterCriticalSection(&m_csOpt);
	if ( m_hHandle == NULL )
	{
		LeaveCriticalSection(&m_csOpt);
		return strResult.AllocSysString();
	}

	CString strCmd = "GetCusTomInfo";
	char szRetBuf[128*1024] = {0};
	INT nRetLen = 0;
	
	if(HVAPI_ExecCmdEx(m_hHandle, strCmd, szRetBuf, sizeof(szRetBuf), &nRetLen)==S_OK)
	{
		char szInfo[1025] = {0};
		HVAPIUTILS_GetExeCmdRetInfoEx(TRUE, szRetBuf, strCmd, "RetMsg", szInfo);
		strResult = szInfo;
	}

	LeaveCriticalSection(&m_csOpt);

	return strResult.AllocSysString();
}


LONG CHvDeviceAxCtrl::SetCusTomInfo(LPCTSTR strCustomInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码

	if (m_fClosed)
	{
		return 1;
	}
	
	EnterCriticalSection(&m_csOpt);
	if ( m_hHandle == NULL )
	{
		LeaveCriticalSection(&m_csOpt);
		return 1;
	}

	//SetCusTomInfo,Value[1111111111111111111111111111]
	CString strCmd;
	strCmd.Format("SetCusTomInfo,Value[%s]", strCustomInfo);
	char szRetBuf[128*1024] = {0};
	INT nRetLen = 0;
	
	int iRet = 1;
	if(HVAPI_ExecCmdEx(m_hHandle, strCmd, szRetBuf, sizeof(szRetBuf), &nRetLen)==S_OK)
	{
		char szInfo[1025] = {0};
		HVAPIUTILS_GetExeCmdRetInfoEx(TRUE, szRetBuf, strCmd, "RetCode", szInfo);
		iRet = atoi(szInfo);
	}
	LeaveCriticalSection(&m_csOpt);

	return iRet;
}
