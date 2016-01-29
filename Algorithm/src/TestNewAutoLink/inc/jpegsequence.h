#pragma once

#include "HvUtils.h"
#include "PlayControl.h"
#include "hvthread.h"
#include "HvJpegProcesser.h"
#include "CamyuSeter.h"
#include "TestServer.h"

#if (RUN_PLATFORM == PLATFORM_WINDOWS)
#include "AviFromJpg.h"
#include <time.h>

// AVI 控制器
typedef struct
{
	__time64_t lFirstTime;				// 第一帧的绝对时间(秒)
	DWORD32 dwBeginTick;				// 启动后第一帧的Tick(毫秒)
	int nTickOffset;					// 时间偏差(毫秒)

	DWORD32 dwCurAviFirstTick;			// 当前Avi文件的第一帧时标（毫秒）
	DWORD32 dwCurAviLastTick;			// 当前Avi文件上一帧的时标(毫秒)

	int nMsecPerFrame;				// 每帧间隔（毫秒）
	int nCurSecFrames;				// 当期秒内帧计数器
	DWORD32 dwCurSecTick;			// 当前秒的时标
	DWORD32 dwLastSendTick;			// 上次到压缩队列的时标
	DWORD32 dwNextSendTick;			// 下次到压缩队列的时间
} 
AVI_FRAME_CTRL;

#endif

const DWORD32 JPEG_LIST_COUNT = 10;
const DWORD32 FRAME_LIST_COUNT = 1;

// JPEG处理线程
class CProcessJpgThread : public HiVideo::IRunable
{
public:
	enum {THREAD_GET_JPEG = 0, THREAD_DECODE_JPEG, THREAD_RECOG_JPEG, THREAD_AVI_COMPRESS};
	CProcessJpgThread(void);
	~CProcessJpgThread(void);
	HRESULT Create(int iThreadType, int iPriority, void *pvParamter);
	HRESULT Close();
	bool IsConnected();
	virtual HRESULT Run(void* pvParamter);
protected:
	int m_iThreadType;
	HiVideo::IThread* m_pThread;
};

class CJpegSequence: public IPlayControl
{
public:
	CJpegSequence(void);
	virtual ~CJpegSequence(void);

	// IPlayControl接口的方法定义
	STDMETHOD(Play)();
	STDMETHOD(Next)();
	STDMETHOD(Prev)();
	STDMETHOD(Seek)(long long rt);
	STDMETHOD(Stop)();
	STDMETHOD(Pause)();
	STDMETHOD(SetCallback)(CFrameSinkCallback pfnCallback, LPVOID pParam=NULL);
	STDMETHOD(SetParam)(const PLAYCONTROL_PARAM &param);
	STDMETHOD(GetParam)(PLAYCONTROL_PARAM &param);
	STDMETHOD(GetStatus)(int &iStatus);
	STDMETHOD(GetCameraStatus)(int &iStatus);

	virtual bool ThreadIsOk();
	//前端采集线程实现
	STDMETHOD(Process)() = 0;
	//JPG解压线程实现
	STDMETHOD(ProcessJpeg)();
	//识别主线程实现
	STDMETHOD(ProcessFrame)();
#if (RUN_PLATFORM == PLATFORM_WINDOWS)

	//AVI 压缩线程
	STDMETHOD(ProcessAviCompress)();
#endif

protected:
	CProcessJpgThread m_threadGetJpg; // 前端采集线程
	CProcessJpgThread m_threadDecodeJpg; // Jpeg解压线程
	CProcessJpgThread m_threadRecogJpg; // 识别主线程
	CProcessJpgThread m_threadAviCompress; // AVI压缩线程

#ifndef _NML_DEVICE
	CTestServer m_testServer; //测试协议服务器
#endif

	//视频帧数据结构
	struct JPG_FRAME {
		IReferenceComponentImage *pRefImage;
		HVPOSITION pCurrent;			//当前帧所在的位置，用来做视频前后导航
		int iVideoID;
		JPG_FRAME()
			:pRefImage(NULL)
			,pCurrent(0)
			,iVideoID(0)
		{
		}
	};
	//解压JPG帧
	BOOL JpgToBmp(struct JPG_FRAME &jpgframe);
	//调用回调
	BOOL SendBmp(struct JPG_FRAME &jpgframe);

	HVPOSITION m_pNavigator;
	virtual BOOL GetNextJpgFrame(struct JPG_FRAME &jpgframe, HVPOSITION &pCurrent) = 0;
	virtual BOOL GetPrevJpgFrame(struct JPG_FRAME &jpgframe, HVPOSITION &pCurrent) = 0;

	BOOL m_fOpened;
	BOOL m_fPaused;
	CFrameSinkCallback m_pfnCallback;
	LPVOID m_pCallbackParam;

	//视频帧保存设置
	BOOL m_fEnableSave;
	CHvString m_strSavePath;
	bool m_fUseStorageSave;			// 使用结构化存储采集文件
	bool m_fTermOfHour;			// 分小时保存
	HiVideo::IJpegSaver* m_pJpegSaver;

	//JPG数据缓冲队列
	CHvList<struct JPG_FRAME, JPEG_LIST_COUNT + 3> m_queJpeg;
	HiVideo::ISemaphore* m_psemJpegQue;

	//帧缓冲队列
	CHvList<struct JPG_FRAME, JPEG_LIST_COUNT + 3> m_queFrame;
	HiVideo::ISemaphore* m_psemFrameQue;

	// YUV 图像队列
	CHvList<IReferenceComponentImage *, 10> m_queYUV;

	bool m_fExitThread;			// 线程退出标志

	HiVideo::IJpegProcesser* m_pJpegProcesser;

	// 前端采集状态
	int m_iGetImageStatus;

#if (RUN_PLATFORM == PLATFORM_WINDOWS)
	// 抓拍相机设置
	CCamyuSeter* m_pCamyuSeter;

	CAviFromJpg *m_pAvi;					// 使用AVI压缩的类
	bool m_fAviCompress;					// 使用AVI压缩
	bool m_fStopAviCompress;				// 停止AVI压缩线程
	bool m_fAviTimeInit;					// AVI 压缩时间初始化标志

	int m_nFrameRate;						// 帧率(缺省2)
	int m_nCompressRate;					// 压缩率(缺省2500 25%)
	int m_nAviMinutes;						// AVI压缩的时间（分钟）

	AVI_FRAME_CTRL m_stAVI;					// AVI帧控制器

#endif

};

