#pragma once
#include "inc\HvDeviceNew.h"
#include "inc\HvDeviceBaseDef.h"
#include "inc\HvDeviceBaseType.h"
#include "inc\HvDataDefine.h"
#include "FFmpeg.h"
#include "VideoMemory.h"
#include "VideoMaxShowDlg.h"
#include <list>
#include <objsafe.h>   //xurui 去掉安全警告
using namespace std;


// HvDeviceAxCtrl.h : Declaration of the CHvDeviceAxCtrl ActiveX Control class.
#define MSG_RECEIVE_RESULT (WM_USER + 110)//定义车牌到达消息
#define MSG_JPEGVIDEO_RESULT (WM_USER + 111)//Jpeg视频到达消息
#define MSG_H264VIDEO_RESULT (WM_USER + 112)//H264视频到达消息



typedef long (_cdecl *GetImage)(void* , int iVideo, const char *strFileName);//定义函数指针类型, HvDevice.dll抓拍函数


typedef  long (_cdecl *GetCaptureImageEx)(void*,int nTimeM, BYTE *pImageBuff, int iBuffLen, int *iImageLen, DWORD64 *dwTime,  DWORD *dwImageWidth, DWORD *dwImageHeigh);

//#define WM_DESTROY_VIDEO (WM_USER+120)

#define RECORDCONNECT 1
#define JPEGCONNECT   2
#define H264CONNECT   3

typedef struct _SAFE_MODE_INFO
{
	char szDevIP[64];
	int iEableSafeMode;
	char szBeginTime[256];
	char szEndTime[256];
	int index;
	int DataInfo;

	_SAFE_MODE_INFO()
	{
		memset(szDevIP, 0, sizeof(szDevIP));
		memset(szBeginTime, 0, sizeof(szBeginTime));
		memset(szEndTime, 0, sizeof(szEndTime));

		iEableSafeMode = 0;
		index = 0;
		DataInfo = 0;
	}

	~_SAFE_MODE_INFO()
	{
		memset(szDevIP, 0, sizeof(szDevIP));
		memset(szBeginTime, 0, sizeof(szBeginTime));
		memset(szEndTime, 0, sizeof(szEndTime));

		iEableSafeMode = 0;
		index = 0;
		DataInfo = 0;
	}
}SAFE_MODE_INFO;

typedef struct _IllegalVideoInfo
{
	DWORD dwCarID;
	DWORD64 dw64PlateTime;
	CString strFilePath;
	CVideoMemory *pVideoMemory;
	CTime CreatTime;

	_IllegalVideoInfo()
	{
		dw64PlateTime = 0;
		strFilePath = "";
		pVideoMemory = NULL;
		CreatTime = CTime::GetCurrentTime();
	}

	~_IllegalVideoInfo()
	{
		if(pVideoMemory)
		{
			pVideoMemory->ClearVideoFrame();
			delete pVideoMemory;
			pVideoMemory = NULL;
		}
		if(strFilePath != "")
		{
			strFilePath = "";
		}
	}
}IllegalVideoInfo;

typedef list<IllegalVideoInfo*> IllegalVideoList;

//#define DEBUGOCX 


// CHvDeviceAxCtrl : See HvDeviceAxCtrl.cpp for implementation.

class CHvDeviceAxCtrl : public COleControl
{
	DECLARE_DYNCREATE(CHvDeviceAxCtrl)

// Constructor
public:
	CHvDeviceAxCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

// Implementation
protected:
	~CHvDeviceAxCtrl();

	/*
	//去掉安全警告 BEGIN  
    DECLARE_INTERFACE_MAP()  
        BEGIN_INTERFACE_PART(ObjectSafety, IObjectSafety)  
        STDMETHOD(GetInterfaceSafetyOptions)(REFIID riid, DWORD __RPC_FAR *pdwSupportedOptions, DWORD __RPC_FAR *pdwEnabledOptions);  
        STDMETHOD(SetInterfaceSafetyOptions)(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions);  
    END_INTERFACE_PART(ObjectSafety)  */


	DECLARE_OLECREATE_EX(CHvDeviceAxCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CHvDeviceAxCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CHvDeviceAxCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CHvDeviceAxCtrl)		// Type name and misc status

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

	//afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);



// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
		dispidSetCusTomInfo = 45L,
		dispidGetCusTomInfo = 44L,
		dispidGetDevMac = 43L,
		dispidGetDevSN = 42L,
		dispidHideIcon = 41L,
		dispidGetCaptureImage = 40L,
		dispidGetIllegalReason = 39L,
		dispidSaveIllegalVideo = 38L,
		dispidRecvIllegalVideoFlag = 37,
		dispidTriggerAlarmSignal = 36L,
		dispidTriggerSignal = 35L,
		dispidGetNameList = 34L,
		dispidInportNameList = 33L,
		dispidGetNameListEx = 32L,		
		dispidInportNameListEx = 31L,
		dispidGetSaveFileStatus = 30L,
		dispidShowH264VideoFlag = 29,
		dispidSetH264FrameRateNum = 28L,
		dispidGetH264FrameRateNum = 27L,
		dispidH264FrameRateNum = 26,
		dispidSendTriggerOut = 25L,
		dispidSoftTriggerCaptureAndForceGetResult = 24L,
		dispidSetPathType = 23L,
		dispidGetFileName = 22L,
		dispidRecvRecordFlag = 21L,
		dispidGetPlateResultTime = 20L,
		eventidOnReceiveH264Video = 3L,
		eventidOnReceiveJpegVideo = 2L,		
		eventidOnReceivePlate = 1L,		
		dispidDisconnect = 19L,
		dispidSaveSnapImage = 18L,
		dispidSavePlateImage = 17L,
		dispidSavePlateBinImage = 16L,
		dispidSavePlateBin2BMP = 15L,
		dispidSaveJpegVideo = 14L,
		dispidSaveH264Video = 13L,
		dispidGetStatus = 12L,
		dispidGetPlateInfo = 11L,
		dispidGetPlateColor = 10L,
		dispidGetPlate = 9L,
		dispidGetCarID = 8L,
		dispidConnectTo = 7L,
		dispidSaveH264VideoTime = 6L,
		dispidRecvSnapImageFlag = 5L,
		dispidRecvPlateImageFlag = 4L,
		dispidRecvPlateBinImageFlag = 3L,
		dispidRecvJpegFlag = 2L,
		dispidRecvH264Flag = 1L
	};
protected:
	void OnRecvH264FlagChanged(void);
	LONG m_RecvH264Flag;
	void OnRecvJpegFlagChanged(void);
	LONG m_RecvJpegFlag;
	void OnRecvPlateBinImageFlagChanged(void);
	LONG m_RecvPlateBinImageFlag;
	void OnRecvPlateImageFlagChanged(void);
	LONG m_RecvPlateImageFlag;
	void OnRecvSnapImageFlagChanged(void);
	LONG m_RecvSnapImageFlag;
	void OnSaveH264VideoTimeChanged(void);
	LONG m_SaveH264VideoTime;
	LONG m_RecvRecordFlag;
	void OnRecvRecordFlagChanged(void);
	void OnH264FrameRateNumChanged(void);
	LONG m_H264FrameRateNum;

	//void OnShowIconFlagChanged(void);
	//LONG m_ShowIconFlag;
	


	void ConnectTo(LPCTSTR strAddr);
	LONG GetCarID(void);
	BSTR GetPlate(void);
	BSTR GetPlateColor(void);
	BSTR GetPlateInfo(void);
	LONG GetStatus(SHORT iConnType);
	LONGLONG GetPlateResultTime();
	LONG SaveH264Video(LPCTSTR strFilePath);
	LONG SaveJpegVideo(LPCTSTR strFileName);
	LONG SavePlateBin2BMP(LPCTSTR strFileName);
	LONG SavePlateBinImage(LPCTSTR strFileName);
	LONG SavePlateImage(LPCTSTR strFileName);
	LONG SaveSnapImage(LPCTSTR strFileName, SHORT iImageType);
	void Disconnect(void);
	HRESULT ClearToSaveData();
	void SetRecordCallBackflag();
	BSTR GetDevSN(void);
	BSTR GetDevMac(void);
public:
	HVAPI_HANDLE_EX m_hHandle;
	HANDLE m_hThread;
	CString m_strIP;
	CString m_strJpegFileName;
	CString m_strH264SavePath;
	BOOL m_fClosed;
	BOOL m_fCreated;
	BOOL m_fSetRecordCallBack;
	DWORD m_dwTheadId;
	DWORD32 m_dwCarID;
	DWORD64 m_dwTimes;
	DWORD64 m_dwOpenAviTimes;
	int m_iJpegDataLen;
	CFFmpeg m_VideoSave;

	//违章视频添加
	CFFmpeg m_IllegalVideoSave;
	CVideoMemory *m_pVideoMemory;
	bool m_bIsBeginSaveIllegalVideo;
	DWORD m_dwIllCarIDPrev;
	CString m_IllegalReason;
	CRITICAL_SECTION m_csIllegalReason;

	IllegalVideoList m_IllegalVideoList;
	IllegalVideoList m_IllegalVideoListSave;
	CRITICAL_SECTION m_csIllegalVideoList;
	CString m_strVideoFile;

	CRITICAL_SECTION m_csIllegalVideoPath;
	CString m_IllegalVideoPath;

	RESULT_DATA m_resultData;
	PBYTE m_pbRecvJpegData;

	//结果数据的临界区
	CRITICAL_SECTION m_csResult;
	//Jpeg数据的临界区
	CRITICAL_SECTION m_csJpegData;
	//H264数据的临界区
	CRITICAL_SECTION m_csH264Data;
	//操作的临界区
	CRITICAL_SECTION m_csOpt;

public:
	//结果数据回调
	//int OnRecordCallBack(PBYTE pbResultPacket, DWORD dwPacketLen, DWORD dwRecordType, LPCSTR szResultInfo);
	int OnIllegalVideo(PVOID pUserData, DWORD dwCarID,
			DWORD dwVideoType, DWORD dwVideoWidth, DWORD dwVideoHeight,
			DWORD64 dw64TimeMS, PBYTE pbVideoData, DWORD dwVideoDataLen, LPCSTR szVideoExtInfo);
	int OnRecordBegin(PVOID pUserData, DWORD dwCarID);
	int OnRecordEnd(PVOID pUserData, DWORD dwCarID);
	int OnPlate(PVOID pUserData, DWORD dwCarID,
			LPCSTR pcPlateNo, LPCSTR pcAppendInfo,
			DWORD dwRecordType,
			DWORD64 dw64TimeMS );
	int OnBigImage(PVOID pUserData, DWORD dwCarID,  
			   WORD  wImgType, WORD  wWidth,
			   WORD  wHeight, PBYTE pbPicData,
			   DWORD dwImgDataLen,DWORD dwRecordType, 
			   DWORD64 dw64TimeMS);
	int OnSmall(PVOID pUserData, DWORD dwCarID,
			WORD wWidth, WORD wHeight,
			PBYTE pbPicData, DWORD dwImgDataLen,
			DWORD dwRecordType,
			DWORD64 dwTimeMS);
	int OnBinary(PVOID pUserData, DWORD dwCarID,
			 WORD wWidth, WORD wHeight, 
			 PBYTE pbPicData, DWORD dwImgDataLen,
			 DWORD dwRecordType, 
			 DWORD64 dwTimeMS);
	int OnJpegFrame(PVOID pUserData, PBYTE pbImageData, DWORD dwImageDataLen, DWORD dwImageType, LPCSTR szImageExtInfo);
	int OnH264Frame(PVOID pUserData, PBYTE pbVideoData, DWORD dwVideoDataLen, DWORD dwVideoType, LPCSTR szVideoExtInfo);

	int AddIllegalVideoList(DWORD dwCarID, DWORD64 dw64PlateTime);
	DWORD64 AddIllegalVideoList(DWORD dwCarID, CString filePath);
	int AddIllegalVideoList(DWORD dwCarID, CVideoMemory* pVideo);
	int DealIllegalVideoList();
	int ClearIllegalVideoList();


	//Live55 接收RTSP流视频回调函数
	//void SLWRtspStreamBack(unsigned char* frame,int frameSize,FRAME_HEAD_S frame_head);



	//Jpeg视频流回调
	//int OnJpegStreamCallBack(PBYTE pbImageData, DWORD dwImageDataLen,PBYTE pbImageInfoData, DWORD dwImageInfoLen, DWORD dwImageType, LPCSTR szImageExtInfo);

	////H264视频流回调
	//int OnH264StreamCallBack(PBYTE pbVideoData, DWORD dwVideoDataLen, DWORD dwVideoType, LPCSTR szVideoExtInfo);

	//连接线程
	static DWORD WINAPI HvConnectThread(LPVOID lpParameter);

	//识别结果回调函数代理
	/*
	static int OnRecordCallBackProxy(PVOID lpFirstParameter,
		PBYTE pbResultPacket,
		DWORD dwPacketLen,
		DWORD dwRecordType,
		LPCSTR szResultInfo)
	{
		if (NULL == lpFirstParameter)
		{
			return -1;
		}
		return ((CHvDeviceAxCtrl*) lpFirstParameter)->OnRecordCallBack(pbResultPacket, dwPacketLen, dwRecordType, szResultInfo);
	}
	*/

	static int OnIllegalVideoProxy(PVOID lpFirstParameter, DWORD dwCarID,
				DWORD dwVideoType, DWORD dwVideoWidth, DWORD dwVideoHeight,
				DWORD64 dw64TimeMS, PBYTE pbVideoData, DWORD dwVideoDataLen, LPCSTR szVideoExtInfo)
	{
			if ( NULL == lpFirstParameter )
			{
				return -1;
			}
			return ((CHvDeviceAxCtrl*) lpFirstParameter)->OnIllegalVideo(lpFirstParameter, dwCarID,
				dwVideoType, dwVideoWidth, dwVideoHeight,
				dw64TimeMS, pbVideoData, dwVideoDataLen, szVideoExtInfo);
 
	}

	static int OnRecordBeginProxy(PVOID lpFirstParameter, DWORD dwCarID)
	{
			if ( NULL == lpFirstParameter )
			{
				return -1;
			}
			return ((CHvDeviceAxCtrl*) lpFirstParameter)->OnRecordBegin( lpFirstParameter,  dwCarID);
 
	}

	static int OnRecordEndProxy(PVOID lpFirstParameter, DWORD dwCarID)
	{
			if ( NULL == lpFirstParameter )
			{
				return -1;
			}

		    return ((CHvDeviceAxCtrl*) lpFirstParameter)->OnRecordEnd( lpFirstParameter,  dwCarID);
	}

	static int OnPlateProxy(PVOID lpFirstParameter, DWORD dwCarID,
			LPCSTR pcPlateNo, LPCSTR pcAppendInfo,
			DWORD dwRecordType,
			DWORD64 dw64TimeMS )
	{
			if ( NULL == lpFirstParameter )
			{
				return -1;
			}

		    return ((CHvDeviceAxCtrl*) lpFirstParameter)->OnPlate( lpFirstParameter,  dwCarID,
			 pcPlateNo,  pcAppendInfo,
			 dwRecordType,
			 dw64TimeMS );
	}

	static int OnBigImageProxy(PVOID lpFirstParameter, DWORD dwCarID,  
			   WORD  wImgType, WORD  wWidth,
			   WORD  wHeight, PBYTE pbPicData,
			   DWORD dwImgDataLen,DWORD dwRecordType, 
			   DWORD64 dw64TimeMS)
	{
			if ( NULL == lpFirstParameter )
			{
				return -1;
			}

		    return ((CHvDeviceAxCtrl*) lpFirstParameter)->OnBigImage( lpFirstParameter,  dwCarID,  
			     wImgType,   wWidth,
			     wHeight,  pbPicData,
			    dwImgDataLen, dwRecordType, 
			    dw64TimeMS);
	}

	static int OnSmallProxy(PVOID lpFirstParameter, DWORD dwCarID,
			WORD wWidth, WORD wHeight,
			PBYTE pbPicData, DWORD dwImgDataLen,
			DWORD dwRecordType,
			DWORD64 dwTimeMS)
	{
			if ( NULL == lpFirstParameter )
			{
				return -1;
			}

		    return ((CHvDeviceAxCtrl*) lpFirstParameter)->OnSmall( lpFirstParameter,  dwCarID,
			 wWidth,  wHeight,
			 pbPicData,  dwImgDataLen,
			 dwRecordType,
			 dwTimeMS);
	}

	static int OnBinaryProxy(PVOID lpFirstParameter, DWORD dwCarID,
			 WORD wWidth, WORD wHeight, 
			 PBYTE pbPicData, DWORD dwImgDataLen,
			 DWORD dwRecordType, 
			 DWORD64 dwTimeMS)
	{
			if ( NULL == lpFirstParameter )
			{
				return -1;
			}

		    return ((CHvDeviceAxCtrl*) lpFirstParameter)->OnBinary( lpFirstParameter,  dwCarID,
			  wWidth,  wHeight, 
			  pbPicData,  dwImgDataLen,
			  dwRecordType, 
			  dwTimeMS);
	}


	//Jpeg视频流回调函数代理
	//static int OnJpegStreamCallBackProxy(LPVOID lpFirstParameter,
	//	PBYTE pbImageData,
	//	DWORD dwImageDataLen, 
	//	PBYTE pbImageInfoData,
	//	DWORD dwImageInfoLen,
	//	DWORD dwImageType,
	//	LPCSTR szImageExtInfo)
	//{
	//	if (NULL == lpFirstParameter)
	//	{
	//		return -1;
	//	}

	//	return ((CHvDeviceAxCtrl*) lpFirstParameter)->OnJpegStreamCallBack(pbImageData, dwImageDataLen, pbImageInfoData,
	//									 dwImageInfoLen, dwImageType, szImageExtInfo);
	//}
	static int OnJpegFrameProxy(PVOID lpFirstParameter, PBYTE pbImageData, DWORD dwImageDataLen, DWORD dwImageType, LPCSTR szImageExtInfo)
	{
		
		if (NULL == lpFirstParameter)
		{
			return -1;
		}

		return ((CHvDeviceAxCtrl*) lpFirstParameter)->OnJpegFrame( lpFirstParameter,  pbImageData,  dwImageDataLen,  dwImageType,  szImageExtInfo);
	}

	static int OnH264FrameProxy(PVOID lpFirstParameter, PBYTE pbVideoData, DWORD dwVideoDataLen, DWORD dwVideoType, LPCSTR szVideoExtInfo)
	{
		if (NULL == lpFirstParameter)
		{
			return -1;
		}

		return ((CHvDeviceAxCtrl*) lpFirstParameter)->OnH264Frame( lpFirstParameter,  pbVideoData,  dwVideoDataLen,  dwVideoType,  szVideoExtInfo);

	}

	//H264视频流回调函数代理
	//static int OnH264StreamCallBackProxy(LPVOID lpFirstParameter,
	//	PBYTE pbVideoData, 
	//	DWORD dwVideoDataLen, 
	//	DWORD dwVideoType,
	//	LPCSTR szVideoExtInfo)
	//{
	//	if (NULL == lpFirstParameter)
	//	{
	//		return -1;
	//	}

	//	return ((CHvDeviceAxCtrl*) lpFirstParameter)->OnH264StreamCallBack(pbVideoData, dwVideoDataLen, dwVideoType, szVideoExtInfo);
	//}
	//virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	/*
	static void WINAPI SLWRtspStreamBackProxy(void* pFirstParameter, unsigned char* frame,int frameSize,FRAME_HEAD_S frame_head)
	{
		if (NULL == pFirstParameter)
		{
			return ;
		}

		((CHvDeviceAxCtrl*)pFirstParameter)->SLWRtspStreamBack(frame,frameSize,frame_head);
		return ;
	}
	*/


protected:
	//触发事件
	void OnReceivePlate(void)
	{
		FireEvent(eventidOnReceivePlate, EVENT_PARAM(VTS_NONE));
	}

	void OnReceiveJpegVideo(void)
	{
		FireEvent(eventidOnReceiveJpegVideo, EVENT_PARAM(VTS_NONE));
	}

	void OnReceiveH264Video(void)
	{
		FireEvent(eventidOnReceiveH264Video, EVENT_PARAM(VTS_NONE));
	}
	class CMessageWindow: public CWnd
	{
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
		{
			CHvDeviceAxCtrl *pCtrl;
			switch (message)
			{
			case MSG_RECEIVE_RESULT:
				pCtrl = (CHvDeviceAxCtrl*)wParam;
				if (pCtrl != NULL)
				{
					pCtrl->OnReceivePlate();
				}
				break;
			case MSG_JPEGVIDEO_RESULT:
				pCtrl = (CHvDeviceAxCtrl*)wParam;
				if (pCtrl != NULL)
				{
					pCtrl->OnReceiveJpegVideo();
				}
				break;
			case MSG_H264VIDEO_RESULT:
				pCtrl = (CHvDeviceAxCtrl*)wParam;
				if (pCtrl != NULL)
				{
					pCtrl->OnReceiveH264Video();
				}
				break;
			default:
				break;
			}
			return CWnd::WindowProc(message, wParam, lParam);
		}
	} m_msgWnd;
protected:
	bool m_fIsPlay;
	int m_iH264Heigh;
	int m_iH264Width;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
private:
	DWORD64 m_JpegBackTime;
	SHORT m_iPathType;
	CString m_strRecordFile;
	CString m_strVideoJPEGFile;
	CString m_strVideoH264File;
	char m_szCommand[1024]; // 历史结果命令
	SAFE_MODE_INFO m_safeModeInfo;
	int m_iBreakCount; // 历史结果标记间隔记数


	//live555 接收句柄
	HANDLE  m_RtspClientHandle;
	int m_rtspState;

	//解码类
	CH264FrameDecode m_Decode;

	//双击最大化时的显示窗口
	CVideoMaxShowDlg *m_pShowMaxVideoDlg;

	//设备类型
	int m_iDeviceType;

	bool m_bDshow;

	BYTE  *m_chBkH264;
	int m_iH264Len;

	bool m_bFullScreen;
	HWND m_hWndParent;
	CRect m_FullScreenRect;
	WINDOWPLACEMENT m_temppl;


	//20150324 徐瑞 为抓拍图准备
	BYTE *m_pCaputureImage;
	int m_iCaptureImgeSize;
	bool m_fCapureFlag;

	HINSTANCE m_hInstHv;
	GetImage m_GetImage;
	GetCaptureImageEx m_GetImageEx;

public:
	int m_jpegnum;
	void WriteIniFile();
	void ReadIniFile();
	void ShowRtspH264();
	bool GetDeviceType();
	

	//模拟发送H264到来的消息，让客户设置保存目录(保存目录的是否为空，关系到是否需要连接自定义H264连接) 
	bool SendMsgForSetSaveH264();

	void CHvDeviceAxCtrl::DrawImageInWindow(HWND pWnd, Bitmap* pbmp, int iHeightFactor=1);

protected:
	BSTR GetFileName(SHORT iType);	
	LONG SetPathType(SHORT iPathtType);
	LONG SoftTriggerCaptureAndForceGetResult(void);
	LONG SendTriggerOut(void);

	LONG GetH264FrameRateNum(void);
	LONG SetH264FrameRateNum(LONG longH264FrameRateNum);
	void OnShowH264VideoFlagChanged(void);
	LONG m_ShowH264VideoFlag;
	LONG GetSaveFileStatus(void);
	LONG InportNameListEx(LPCTSTR szWhiteNameList, LONG iWhiteListLen, LPCTSTR szBlackNameList, LONG iBlackListLen);
	BSTR GetNameListEx(void);
	LONG InportNameList(CHAR* szWhiteNameList, LONG iWhiteListLen, CHAR* szBlackNameList, LONG iBlackListLen);
	LONG GetNameList(CHAR* szWhiteNameList, LONG* iWhiteListLen, CHAR* szBlackNameList, LONG* iBlackListLen);
	LONG TriggerSignal(LONG VideoID);
	LONG TriggerAlarmSignal(void);
	void OnRecvIllegalVideoFlagChanged(void);
	LONG m_RecvIllegalVideoFlag;
	LONG SaveIllegalVideo(LPCTSTR strFilePath);
	BSTR GetIllegalReason(void);
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//afx_msg LRESULT DestroyMaxVideoWnd(WPARAM a,LPARAM b);
	void ShowVideoNomal();
	HRESULT GetCaptureImage(int iVideoID, LPCTSTR strFileName);

	
protected:
	void OnHideIconChanged(void);
	LONG m_HideIcon;
	BSTR GetCusTomInfo(void);
	LONG SetCusTomInfo(LPCTSTR strCustomInfo);
};

