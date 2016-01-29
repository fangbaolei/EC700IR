#ifndef _HVAPI_HANDLE_CONTEXT_H__
#define _HVAPI_HANDLE_CONTEXT_H__

#include "HvDeviceDLL.h"
#include "HvDeviceNew.h"

//#define  _CARFACE

#ifdef _CARFACE
class CCarFaceCtrl;
#endif
/* 回调集定义 */
typedef struct _HVAPI_CALLBACK_SET
{
	struct _HVAPI_CALLBACK_SET* pNext;
	INT iVideoID;	//视频流标号

	HVAPI_CALLBACK_RECORD_INFOBEGIN pOnRecordBegin;	//识别结果开始回调
	PVOID pOnRecordBeginParam; //识别结果开始回调用户参数

	HVAPI_CALLBACK_RECORD_INFOEND pOnRecordEnd;	//识别结果结束回调
	PVOID pOnRecordEndParam; //识别结果结束回调用户参数

	HVAPI_CALLBACK_RECORD_PLATE pOnPlate;	//车牌回调，上传车牌字串和识别结果附加信息
	PVOID pOnPlateParam;	//车牌回调用户参数

	HVAPI_CALLBACK_RECORD_BIGIMAGE pOnBigImage;	//识别结果大图回调
	PVOID pOnBigImageParam;	//识别结果大图回调用户参数

	HVAPI_CALLBACK_RECORD_SMALLIMAGE pOnSmallImage;	//车牌小图回调
	PVOID pOnSmallImageParam;	//车牌小图回调用户参数

	HVAPI_CALLBACK_RECORD_BINARYIMAGE pOnBinaryImage;	//车牌二值图回调
	PVOID pOnBinaryImageParam;	//车牌二值图回调用户参数

	HVAPI_CALLBACK_STRING pOnString;	//字符信息回调
	PVOID pOnStringParam;	//字符信息回调用户参数

	HVAPI_CALLBACK_TFD_STRING pOnTFDString;	//流量检测产品字符信息回调  
	PVOID pOnTFDStringParam;	//流量检测产品字符信息回调用户参数

	HVAPI_CALLBACK_JPEG pOnJpegFrame;	//Image连接图片桢回调
	PVOID pOnJpegFrameParam;	//Image连接图片桢回调用户参数

	HVAPI_CALLBACK_H264 pOnH264; //H264视频流回调
	PVOID pOnH264Param;	//H264视频流回调用户参数

	HVAPI_CALLBACK_HISTORY_VIDEO pOnHistoryVideo; //历史视频回调
	PVOID pOnHistoryVideoParam;	//历史视频回调用户参数

	HVAPI_CALLBACK_RECORD_ILLEGALVideo pOnIllegalVideo;//违法视频回调
	PVOID pOnIllegalVideoParam;//违法视频回调用户参数

	HVAPI_CALLBACK_REDEVELOP_RESULT pOnReDevelopResult;	//二次开发用回调  
	PVOID pOnReDevelopResultParam;	//二次开发用回调用户参数

	HVAPI_CALLBACK_RESULT  pOnRecordResult;// 识别结果扩展回调： 包括大图 小图 二位图 字符信息回调 交通流量信息
	HVAPI_CALLBACK_H264_EX pOnRecoreH264Video;		// H264扩展回调： 
	HVAPI_CALLBACK_H264_EX pOnRecoreH264HistoryVideo;//历史H264扩展回调
	HVAPI_CALLBACK_JPEG_EX pOnMJPEG;				// 图片扩展回调

	_HVAPI_CALLBACK_SET()
		: pNext(NULL)
		, pOnPlate(NULL)
		, pOnPlateParam(NULL)
		, pOnBigImage(NULL)
		, pOnBigImageParam(NULL)
		, pOnSmallImage(NULL)
		, pOnSmallImageParam(NULL)
		, pOnBinaryImage(NULL)
		, pOnBinaryImageParam(NULL)
		, pOnString(NULL)
		, pOnStringParam(NULL)
		, pOnTFDString(NULL)
		, pOnTFDStringParam(NULL)
		, pOnJpegFrame(NULL)
		, pOnJpegFrameParam(NULL)
		, pOnH264(NULL)
		, pOnH264Param(NULL)
		, pOnHistoryVideo(NULL)
		, pOnHistoryVideoParam(NULL)
		, pOnRecordResult(NULL)
		, pOnRecoreH264Video(NULL)
		, pOnRecoreH264HistoryVideo(NULL)
		, pOnMJPEG(NULL)
		, pOnIllegalVideo(NULL)
		, pOnIllegalVideoParam(NULL)
		, pOnReDevelopResult(NULL)
		, pOnReDevelopResultParam(NULL)
		, iVideoID(0)
	{

	}

}HVAPI_CALLBACK_SET;

/* HVAPI 句柄内容 */
typedef struct _HVAPI_HANDLE_CONTEXT_EX
{
	DWORD	dwOpenType;
	CHAR	szVersion[8]; //版本			
	CHAR	szIP[16];	//设备IP
	CHAR	szDevSN[256];  //设备编号

	SOCKET	sktRecord;						//结果接收SOCKET
	LPSTR	szRecordConnCmd[128];			//结果接收命令
	DWORD	dwRecordConnStatus;				//结果接收连接状态
	BOOL	fIsRecvHistoryRecord;			//是否为接收历史结果
	SOCKET	sktImage;						//图片接收SOCKET
	LPSTR	szImageConnCmd[128];			//图片接收命令
	DWORD	dwImageConnStatus;				//图片接收连接状态
	SOCKET	sktVideo;						//视频接收SOCKET
	LPSTR	szVideoConnCmd[128];			//视频接收命令
	DWORD	dwVideoConnStatus;				//视频接收连接状态
	BOOL	fIsConnectHistoryVideo;			//是否为接收历史录像的连接

	HANDLE	hThreadRecvRecord;				//结果接收线程
	BOOL	fIsThreadRecvRecordExit;		//结果接收线程结束标记
	DWORD	dwEnhanceRedLightFlag;			//识别结果红灯加红标志
	INT		iBigPicBrightness;				//识别结果大图增亮
	INT		iBigPicHueThrshold;				//识别结果大图红灯阀值
	INT		iBigPicCompressRate;			//识别结果大图红灯加红图片压缩品质
	HANDLE	hThreadRecvImage;				//图片接收线程
	BOOL	fIsThreadRecvImageExit;			//图片接收线程结束标记
	HANDLE	hThreadRecvVideo;				//视频接收线程
	BOOL	fIsThreadRecvVideoExit;			//视频接收线程结束标记

	HANDLE	hThreadSocketStatusMonitor;		//连接状态检测线程
	BOOL	fIsThreadSocketStatusMonitorExit;	//连接状态检测线程结束标记

	HVAPI_CALLBACK_SET* pCallBackSet;		//回调集

	DWORD	dwRecordStreamTick;				//结果接收线程计数器
	DWORD	dwImageStreamTick;				//图片接收线程计数器
	DWORD	dwVideoStreamTick;

	DWORD	dwVideoEnhanceRedLightFlag;		//历史录像红灯加红处理
	INT		iBrightness;					//历史录像增亮
	INT		iHueThrshold;					//历史录像红灯色度阀值
	INT		iCompressRate;					//历史录像红灯加红图片压缩品质

	char szRecordBeginTimeStr[14];			//接收历史结果起始时间
	char szRecordEndTimeStr[14];			//接收历史结果结束时间
	DWORD dwRecordStartIndex;				//接收历史结果起始车辆号
	DWORD dwHistoryRecordFlag;				//是否为接收历史结果标志
	INT iRecordDataInfo;					//结果过滤标志
	INT iRealTimeRecordSendIllegalVideo;	//实时结果接收违章视频标志

	char szVideoBeginTimeStr[20];			//接收历史视频起始时间
	char szVideoEndTimeStr[20];				//接收历史视频结束时间

	DWORD dwRecordReconectTimes;			//结果接收重连次数
	DWORD dwImageReconnectTimes;			//图像接收重连次数
	DWORD dwVideoReconnectTimes;			//视频接收重连次数

	RECT rcPlate[5];                        //车牌坐标
	RECT rcFacePos[5][20];                     //人脸坐标
	int nFaceCount[5];                         //人脸数

	bool  fNewProtocol;
	PROTOCOL_VERSION emProtocolVersion;     // 协议版本号
	DWORD CropImageEnable;
	DWORD CropImageHeight;
	DWORD CropImageWeight;

	DWORD NoneImageEnable;
	DWORD NoneImageUpx;
	DWORD NoneImageUpy;


	BOOL fIsRecordPlateExit;                //结果回调关闭标记
	BOOL fIsRecordStringExit;                //结果状态信息关闭标记

	//主动连接相关
	BOOL fAutoLink;							//主动连接标志
	BOOL fVailPackResumeCache;				//结果续传缓存 使能标志
	PACK_RESUME_CACHE* pPackResumeCache;	//结果续传缓存
	CRITICAL_SECTION csCallbackJpeg;
	CRITICAL_SECTION csCallbackRecord;
	CRITICAL_SECTION csCallbackH264;
	DWORD64 AutoLinkSetCallBackTime;
	DWORD64 AutiLinkOverTime;
	//用户验证相关
	CHAR szUserName[128];
	CHAR szPassword[128];
	INT nAuthority;

	
//车脸识别
	bool fEnableCarFaceCtrl;
#ifdef  _CARFACE
	
	CCarFaceCtrl *pCarFaceRecogCtrl;
#endif

	
	//20150521 徐瑞 为抓拍图准备
	BYTE *pCaputureImage;
	int iCaptureImgeSize;
	bool fCapureFlag;
	CRITICAL_SECTION csLockGetImage;


	_HVAPI_HANDLE_CONTEXT_EX()
		: hThreadSocketStatusMonitor(NULL)
		, hThreadRecvRecord(NULL)
		, hThreadRecvImage(NULL)
		, hThreadRecvVideo(NULL)
		, pCallBackSet(NULL)
		, fIsThreadSocketStatusMonitorExit(TRUE)
		, sktRecord(INVALID_SOCKET)
		, dwRecordConnStatus(CONN_STATUS_CONNFIRST)
		, dwRecordStreamTick(0)
		, fIsRecvHistoryRecord(FALSE)
		, sktImage(INVALID_SOCKET)
		, dwImageConnStatus(CONN_STATUS_UNKNOWN)
		, dwImageStreamTick(0)
		, sktVideo(INVALID_SOCKET)
		, dwVideoConnStatus(CONN_STATUS_UNKNOWN)
		, dwVideoStreamTick(0)
		, fIsConnectHistoryVideo(0)
		, fIsThreadRecvRecordExit(TRUE)
		, fIsThreadRecvImageExit(TRUE)
		, fIsThreadRecvVideoExit(TRUE)
		, dwEnhanceRedLightFlag(0)
		, dwVideoEnhanceRedLightFlag(0)
		, dwRecordStartIndex(0)
		, dwHistoryRecordFlag(0)
		, iRecordDataInfo(-1)
		, iRealTimeRecordSendIllegalVideo(-1)
		, dwRecordReconectTimes(0)
		, dwImageReconnectTimes(0)
		, dwVideoReconnectTimes(0)
		, dwOpenType(0)
		, fNewProtocol(false)
		, emProtocolVersion(PROTOCOL_UNKNOWN)
		, fAutoLink(FALSE)
		, pPackResumeCache(NULL)
		, fVailPackResumeCache(FALSE)
		, AutoLinkSetCallBackTime(0)
		, AutiLinkOverTime(0)
		, CropImageEnable(0)
		, CropImageHeight(600)
		, CropImageWeight(800)
		, NoneImageEnable(0)
		, NoneImageUpy(300)
		, NoneImageUpx(400)
		, fIsRecordPlateExit(TRUE)
		, fIsRecordStringExit(TRUE)
		, nAuthority(-1)
		, pCaputureImage(NULL)
		, iCaptureImgeSize(0)
		, fCapureFlag(false)
		
		
	{
		InitializeCriticalSection(&csCallbackRecord);
		InitializeCriticalSection(&csCallbackH264);
		InitializeCriticalSection(&csCallbackJpeg);
		InitializeCriticalSection(&csLockGetImage);

		ZeroMemory(szVersion, sizeof(szVersion));
		ZeroMemory(szIP, sizeof(szIP));
		ZeroMemory(rcFacePos, sizeof(rcFacePos));
		szUserName[0] = '\0';
		szPassword[0] = '\0';

		#ifdef  _CARFACE
		fEnableCarFaceCtrl = false;
		pCarFaceRecogCtrl = NULL;
		#endif

	}
	~_HVAPI_HANDLE_CONTEXT_EX()
	{
		if ( pCaputureImage != NULL )
		{
			delete[] pCaputureImage;
			pCaputureImage = NULL;
		}

		DeleteCriticalSection(&csCallbackRecord);
		DeleteCriticalSection(&csCallbackH264);
		DeleteCriticalSection(&csCallbackJpeg);
		DeleteCriticalSection(&csLockGetImage);
	}
}HVAPI_HANDLE_CONTEXT_EX;

/* HVAPI 句柄内容 */
typedef struct _HVAPI_LISTEN_HANDLE_CONTEXT
{			
	CHAR	szIP[16];	//设备IP
	PVOID	pUserData;
	HANDLE  hThread;
	CRITICAL_SECTION csCallback;
	HVAPI_CALLBACK_LISTEN_CMD pCallBackFun;		//回函数
	HVAPI_CALLBACK_LISTEN_CMDEX pCallBackFunEx;		//回函数
	DWORD		historyTrigerID;
	_HVAPI_LISTEN_HANDLE_CONTEXT()
		: hThread(NULL)
		, pCallBackFun(NULL)
		, pUserData(NULL)
		, historyTrigerID(-1)
		, pCallBackFunEx(NULL)
		
	{
		memset(szIP,0,sizeof(szIP));
		//memset(historyTrigerID,-1,sizeof(historyTrigerID));
		InitializeCriticalSection(&csCallback);
	}
	~_HVAPI_LISTEN_HANDLE_CONTEXT()
	{
		DeleteCriticalSection(&csCallback);
	}
}HVAPI_LISTEN_HANDLE_CONTEXT;

/*
class CCSLockEx
{
public:
	CCSLockEx(CRITICAL_SECTION * pCS, bool fLock = true)
	{
		m_pCS = pCS;
		m_fLock = false;
		if (fLock)
		{
			Lock();
		}
	}
	virtual ~CCSLockEx()
	{
		Unlock();
	}
	void Lock(void)
	{
		if (!m_fLock)
		{
			m_fLock = true;
			EnterCriticalSection(m_pCS);
		}
	}
	void Unlock(void)
	{
		if (m_fLock)
		{
			LeaveCriticalSection(m_pCS);
			m_fLock = false;
		}
	}
private:
	CRITICAL_SECTION *m_pCS;
	bool m_fLock;
};*/

#endif
