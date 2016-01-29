#ifndef _HV_AUTOLINK_NEW_H_
#define _HV_AUTOLINK_NEW_H_

#include "HvDeviceDLL.h"
//#include "HvDeviceNew.h"

#include "HvAutoLinkDeviceNew.h"



typedef struct _HVAPI_AL_CALLBACK_SET
{
	HVAPI_AL_CALLBACK_H264 pOnRecoreH264Video;		// H264扩展回调： 
	PVOID pOnH264Param;
	HVAPI_AL_CALLBACK_H264 pOnRecoreH264HistoryVideo;//历史H264扩展回调
	PVOID pOnHistroyH264Param;

	 HVAPI_AL_CALLBACK_JPEG pOnMJPEG;				// 图片扩展回调
	PVOID pOnJpegParam;

	_HVAPI_AL_CALLBACK_SET()
	{
		pOnRecoreH264Video = NULL;
		pOnH264Param = NULL;
		pOnRecoreH264HistoryVideo = NULL;
		pOnMJPEG = NULL;
		pOnJpegParam = NULL;
	}

}_HVAPI_AL_CALLBACK_SET;


//视频服务
typedef struct _HVAPI_AL_VIDEO_SERVER
{
	SOCKET sktVideo;
	HANDLE	hThreadServer;
	bool fExit;

	bool fHistoryEnable;
	DWORD64 dwBeginTime;
	DWORD64 dwEndTime;

	DWORD dwRecvType;

	char szAddr[32];
	char szDeviceNo[32]; 

	_HVAPI_AL_VIDEO_SERVER()
		:sktVideo(INVALID_SOCKET)
		,hThreadServer(NULL)
		,dwBeginTime(0)
		,dwEndTime(0)
		,fHistoryEnable(false)
		,dwRecvType(0)
	{
	}
}_HVAPI_AL_VIDEO_SERVER;



enum NOTICESTATUS{
	NONOTICE=0,    //未通知
	ONLINE,		   //上线通知
	OFFLINE,	   //下线通知
};

typedef struct _HVAPI_AL_CONTEXT
{
	_HVAPI_AL_CALLBACK_SET CallBackSet;
	CRITICAL_SECTION csH264Call;
	CRITICAL_SECTION csJpegCall;

	//CRITICAL_SECTION csCallBack;

	//命令链路
	SOCKET	sktCmd;
	HANDLE	hThreadCmd;
	bool fCmdThExit;   //用于标明命令线程是否已退出，同时标明设备是否在线，如果为真， 表现设备不在线， 句柄不可用了， 如果为假， 表示设备可用。
	bool fBusySktCmd;
	CRITICAL_SECTION csSktCmd;
	DWORD dwCmdTick;

	char szAddr[32];
	char szDeviceNo[32];
	char szDeviceInfo[1024];
	int iPort;

	_HVAPI_AL_VIDEO_SERVER videoH264;
	_HVAPI_AL_VIDEO_SERVER videoJpeg;


	NOTICESTATUS eNotieType;

	DWORD dwRef;  // 用于计数。 上线时自动加1， 当应用计数为0 时， 且句柄已不可用了， 动态库自动删除此句柄。   上位机接到下线通知， 请调用API 函数关掉句柄，关掉时， 应用计数为0,。
	CRITICAL_SECTION csRef;

	
	_HVAPI_AL_CONTEXT()
		:sktCmd(INVALID_SOCKET)
		,hThreadCmd(NULL)
		//,pCallBackSet(NULL)
		,fCmdThExit(false)
		,fBusySktCmd(false)
		,dwCmdTick(0)
		,eNotieType(NONOTICE)
		,dwRef(0)
	{
		InitializeCriticalSection(&csSktCmd);
		InitializeCriticalSection(&csH264Call);
		InitializeCriticalSection(&csJpegCall);
		InitializeCriticalSection(&csRef);
	}


	~_HVAPI_AL_CONTEXT()
	{
		DeleteCriticalSection(&csSktCmd);
		DeleteCriticalSection(&csH264Call);
		DeleteCriticalSection(&csJpegCall);
		DeleteCriticalSection(&csRef);
	}

}_HVAPI_AL_CONTEXT;






typedef struct _tagNoticeFunc
{
	HVAPI_AL_ONLINE_NOTICE pOnOnLine;	//识别结果开始回调
	PVOID pOnOnLineParam; //识别结果开始回调用户参数

	HVAPI_AL_OFFLINE_NOTICE pOnOffLine;	//识别结果结束回调
	PVOID pOnOffLineParam; //识别结果结束回调用户参数

	_tagNoticeFunc()
		:pOnOnLine(NULL)
		,pOnOnLineParam(NULL)
		,pOnOffLine(NULL)
		,pOnOffLineParam(NULL)
	{
	}

}NoticeFunc;




typedef struct _tagnNetServer
{
	CHAR szVersion[8];

	_HVAPI_AL_CONTEXT **pAutoContext;
	NoticeFunc noticeFunc;

	CRITICAL_SECTION csContext;

	//命令服务
	INT iCmdPort;
	int iMaxCnt;
	int iLinkCnt;
	SOCKET	sktCmdSvr;
	HANDLE	hThreadCmdSvr;
	bool fCmdSvrStop;

	//数据服务
	INT iDatePort;
	int iMaxDateLinkCnt;
	int iDateLinkCnt;
	SOCKET	sktDateSvr;
	HANDLE	hThreadDatedSvr;
	bool fDateSvrStop;

	//处理设备上线和下线的线程(通过回调通知上层函数)
	HANDLE	hNteThread; 

	_tagnNetServer()
		:sktCmdSvr(INVALID_SOCKET)
		,hThreadCmdSvr(NULL)
		,iCmdPort(0)
		,iMaxCnt(0)
		,iLinkCnt(0)
		,fCmdSvrStop(false)
		,pAutoContext(NULL)
		,iDatePort(0)
		,iMaxDateLinkCnt(0)
		,iDateLinkCnt(0)
		,sktDateSvr(INVALID_SOCKET)
		,hThreadDatedSvr(NULL)
		,fDateSvrStop(false)
		,hNteThread(NULL)
	{
		memset(szVersion, 0, 8);
		InitializeCriticalSection(&csContext);
	}
	~_tagnNetServer()
	{
		DeleteCriticalSection(&csContext);
	}


}NetServer;

#endif