#ifndef _TRACKERCALLBACK_INCLUDED__
#define _TRACKERCALLBACK_INCLUDED__

#include "swObjBase.h"
#include "swPlate.h"
#include "swImage.h"
#include "swImageObj.h"
#include "stdResultInfo.h"
#include "HvBaseObj.h"

// 外总控信号定义
#define MAX_SIGNAL_TYPE		10				// 最大信号类型数量
#define MAX_SIGNAL_SOURCE	30				// 最大信号源数量

//参数结构体定义
#define BINARY_IMAGE_WIDTH 112				// 二值化图的宽度
#define BINARY_IMAGE_HEIGHT 20				// 二值化图的高度

typedef enum {
	BIG_DAY_FRONTLIGHT = 0,
	BIG_DAY_FRONTLIGHT_2,
	DAY_FRONTLIGHT,
	DAY_FRONTLIGHT_2,
	DAY,
	DAY_2,
	DAY_BACKLIGHT,
	DAY_BACKLIGHT_2,
	BIG_DAY_BACKLIGHT,
	BIG_DAY_BACKLIGHT_2,
	NIGHTFALL,
	NIGHTFALL_2,
	NIGHT,
	NIGHT_2,
	LIGHT_TYPE_COUNT
} LIGHT_TYPE;

//车道数据结构, 用车道线来标定
typedef struct 
{
	//边线的坐标与类型
	HV_POINT ptTop;
	HV_POINT ptBottom;
	int iLineType;
	int iRoadType; //车道类型
	char szRoadTypeName[255];
	int iFilterRunType; //要过滤的行驶类型
} ROAD_INFO;

const int MAX_ROADLINE_NUM = 4;
typedef struct _ACTIONDETECT_PARAM_TRACKER
{
	int iDetectCarStopTime;	// 紧急停车判断时间，单位Ｓ
	int iIsYellowLine[MAX_ROADLINE_NUM];		//0-01车道，1-12车道，以此类推
	int iIsCrossLine[MAX_ROADLINE_NUM];		//0-01车道，1-12车道，以此类推
}ACTIONDETECT_PARAM_TRACKER;

//红绿灯状态
typedef enum _TRAFFICLIGHT_LIGHT_STATUS
{
	TLS_UNSURE = 0,
	TLS_RED,
	TLS_YELLOW,
	TLS_GREEN
} LIGHT_STATUS;

//红绿灯方向
typedef enum _TRAFFICLIGHT_POSITION
{
	TP_UNSURE = 0,
	TP_LEFT,
	TP_FORWARD,
	TP_RIGHT,
	TP_TURN,
	MAX_TP_COUNT
} TRAFFICLIGHT_POSITION;

//灯的意义
typedef struct _TRAFFICLIGHT_TYPE
{
	int iTeam;
	int iPos;
	_TRAFFICLIGHT_POSITION tpPos;
	_TRAFFICLIGHT_LIGHT_STATUS tlsStatus;
}TRAFFICLIGHT_TYPE;

//红绿灯最大场景数
#define MAX_SCENE_COUNT 10
typedef struct _TRAFFICLIGHT_SCENE
{
	LIGHT_STATUS lsLeft;
	LIGHT_STATUS lsForward;
	LIGHT_STATUS lsRight;
	LIGHT_STATUS lsTurn;
} TRAFFICLIGHT_SCENE;

typedef struct _SCENE_INFO
{
	char pszInfo[64];
}SCENE_INFO;

//违章类型,黄国超修改，每个违章类型占一位，方便违章类型的叠加;同时注释掉MAX_PT_COUNT;2010-10-12
typedef enum PECCANCY_TYPE
{
	PT_NORMAL      = 0,
	PT_RUSH        =0x01,
	PT_ACROSS      =0x02,
	PT_OVERLINE    =0x04,
	PT_CONVERSE    =0x08,
	PT_ESTOP       =0x10,
	PT_STOPING     =0x20,
	PT_OVERSPEED   =0x40
	//MAX_PT_COUNT
} PECCANCY_TYPE;

//车辆行驶类型
typedef enum RUN_TYPE
{
	RT_UNSURE = 0,
	RT_FORWARD,
	RT_LEFT,
	RT_RIGHT,
	RT_TURN,
	RT_CONVERSE,
	RT_ESTOP,
	MAX_RT_COUNT
} RUN_TYPE;

//车辆行驶车道类型
typedef enum RUN_ROAD_TYPE
{
	RRT_FORWARD = 0x01,
	RRT_LEFT          = 0x02,
	RRT_RIGHT		 = 0x04,
	RRT_TURN	       = 0x08,
	RRT_ESTOP        = 0x10,
	RRT_CONVERSE = 0x20
} RUN_ROAD_TYPE;

//灯亮的绝对时间
typedef struct tag_LightRealTime
{
	DWORD32 dwLeftL;
	DWORD32 dwLeftH;
	DWORD32 dwForwardL;
	DWORD32 dwForwardH;
	DWORD32 dwRightL;
	DWORD32 dwRightH;
	DWORD32 dwTurnL;
	DWORD32 dwTurnH;
	tag_LightRealTime()
	{
		Reset();
	}
	void Reset()
	{
		dwLeftL = dwLeftH = dwForwardL = dwForwardH = dwRightL = dwRightH = dwTurnL = dwTurnH = 0;
	}
}
LIGHT_REALTIME;

//灯亮的相对时间
typedef struct tag_LightTick
{
	DWORD32 dwLeft;
	DWORD32 dwForward;
	DWORD32 dwRight;
	DWORD32 dwTurn;
	tag_LightTick()
	{
		Reset();
	}
	void Reset()
	{
		dwLeft = dwForward = dwRight = dwTurn = 0;
	}
}
LIGHT_TICK;

//压线和越线类型
typedef enum CROSS_OVER_LINE_TYPE
{
	COLT_INVALID = -1,
	COLT_NO = -2,
	COLT_LINE0 = 0,
	COLT_LINE1 = 1,
	COLT_LINE2 = 2,
	COLT_LINE3 = 3,
	COLT_LINE4 = 4
}CROSS_OVER_LINE_TYPE;

// 车辆尺寸信息输出
typedef struct _OUTPUT_CAR_SIZE_INFO {
	int nOutType;				// 输出类型 0:像素 1：米
	float iCarWidth;			// 车辆宽度
	float iCarHeight;			// 车辆高度
} OUTPUT_CAR_SIZE_INFO;

typedef struct tagResultImageStruct {
	IReferenceComponentImage *pimgPlate;
	IReferenceComponentImage *pimgBestSnapShot;
	IReferenceComponentImage *pimgLastSnapShot;
	IReferenceComponentImage *pimgBeginCapture;
	IReferenceComponentImage *pimgBestCapture;
	IReferenceComponentImage *pimgLastCapture;
	IReferenceComponentImage *pimgPlateBin;
	tagResultImageStruct()
	{
		memset(this, 0, sizeof(tagResultImageStruct));
	}
} RESULT_IMAGE_STRUCT;

HRESULT CopyResultImageStruct(RESULT_IMAGE_STRUCT *pDest, const RESULT_IMAGE_STRUCT *pSrc);
HRESULT CopyResultImageStructOnlyJPEG( RESULT_IMAGE_STRUCT *pDest, const RESULT_IMAGE_STRUCT *pSrc );
HRESULT FreeResultImageStruct(RESULT_IMAGE_STRUCT *pObj);

typedef struct tagCarLeftInfoStruct {
	PBYTE8 pbContent;					// 车牌号码信息
	BYTE8 bType;						// 车牌类型
	BYTE8 bColor;						// 车牌号码
	float fltAverageConfidence;			// 可信度
	float fltFirstAverageConfidence;	// 首字符可信度
	RESULT_IMAGE_STRUCT *pResultImage;	// 结果图片信息
	UINT iFrameNo;						// 结果帧号码
	UINT iRefTime;						// 结果帧时间
	UINT nStartFrameNo;					// 开始帧号码
	UINT nEndFrameNo;					// 结束帧号码
	UINT nFirstFrameTime;				// 开始跟踪时间
	int iObservedFrames;				// 观察帧数
	int iVotedObservedFrames;			// 最后投票车牌类型的有效帧数
	bool fReverseRun;					// 车辆反向运动标志
	int nVoteCondition;					// 投票条件
	int iAvgY;							//环境光亮度
	int iCarAvgY;						//车牌亮度
	int iCarVariance;					//车牌对比度
	int nVideoDetID;					// 视频检测ID号(对于无牌车使用)
	int nCarLeftCount;					// 车辆离开帧数计数器
	float fltCarspeed;					// 车辆信息
	float fltScaleOfDistance;			// 距离计算的误差比例
	int nPlateLightType;				// 当前车牌亮度级别
	UINT nCarArriveTime;				// CarArrive的时间
	HiVideo::CRect rcBestPlatePos;		// 最清晰车牌坐标
	HiVideo::CRect rcLastPlatePos;		// 最后出现车牌坐标
	HiVideo::CRect rcSecondPos;		// 第二个抓拍位置车牌的坐标
	bool fCarIsStop;					// 是否停止车
	int iCarStopPassTime;				// 停止车经过时间
	CAR_TYPE nCarType;					// 车辆类型（大、中、小）
	CAR_COLOR nCarColor;				// 车辆颜色
	bool fOutputCarColor;				//输出车身颜色开关
	OUTPUT_CAR_SIZE_INFO stCarSize;		// 车辆尺寸

	int                 iRoadType;    //车道类型，黄国超添加，2011-03-07
	RUN_TYPE            rtType;       //车辆行驶类型
	/*PECCANCY_TYPE*/ int ptType;	//违章类型
	
	CROSS_OVER_LINE_TYPE coltIsOverYellowLine;		//压黄线
	CROSS_OVER_LINE_TYPE coltIsCrossLine;			//越线
	
	TRAFFICLIGHT_SCENE tsOnePosScene;		//车牌第一个位置的红绿灯场景
	TRAFFICLIGHT_SCENE tsTwoPosScene;		//车牌第二个位置的红绿灯场景

	//红灯开始时间(相对时间,毫秒)
	LIGHT_REALTIME lrtRedStart;

	//这个违章结果是否不确定
	int iUnSurePeccancy;
	// 所处理的视频ID
	int iVideoID;
	// 强制发送抓拍图片开关
	int iSendCapImageFlag;

	//车道编号的方向0:从左开始,1:从右开始
	int iRoadNumberBegin;
	//起始车道号
	int iStartRoadNum;
	//判断是压实线还是双黄线
	bool fIsDoubleYellowLine;

	//抓拍补光抓拍的几个时间
	DWORD32 dwFirstTime;
	DWORD32 dwSecondTime;
	DWORD32 dwThirdTime;

	//当前是否是晚上
	bool fIsNight;

	tagCarLeftInfoStruct()
	{
		memset(this, 0, sizeof(tagCarLeftInfoStruct));
		coltIsOverYellowLine	= COLT_NO;
		coltIsCrossLine		= COLT_NO;
	}
} CARLEFT_INFO_STRUCT;					// 车辆离开信息

typedef struct tagCarArriveInfoStruct {
	int iRoadNumber;
	// 所处理的视频ID
	int iVideoID;
	UINT iPlateLightType;
	UINT dwTriggerOutDelay;
	UINT dwIsOutTrig;
	UINT dwCarArriveTime;
	tagCarArriveInfoStruct()
	{
		memset(this, 0, sizeof(tagCarArriveInfoStruct));
		iRoadNumber = -1;
	}
} CARARRIVE_INFO_STRUCT;


//复制CARLEFT_INFO_STRUCT

HRESULT CopyCarLeftInfoStruct(
							  CARLEFT_INFO_STRUCT *pDest,
							  const CARLEFT_INFO_STRUCT *pSrc, 
							  int iFlag = 0			//是否只保留JPEG图片，0：否，1：是
							  );

//释放CARLEFT_INFO_STRUCT内部分配内存
HRESULT FreeCarLeftInfoStruct(
							  CARLEFT_INFO_STRUCT *pObj
							  );


//帧属性
typedef struct _FRAME_PROPERTY
{
	INT iWidth;
	INT iHeight;
	INT iStride;
}
FRAME_PROPERTY;

typedef struct _VIDEODET_PARAM
{
	BOOL fEnable;
	BOOL fAllowFrameSkip;
	INT nDetLeft;
	INT nDetRight;

	_VIDEODET_PARAM()
	{
		fEnable = FALSE;
		fAllowFrameSkip = TRUE;
		nDetLeft = 0;
		nDetRight = 100;
	}
}
VIDEODET_PARAM;

//预校正参数
typedef struct _PRECALIBRATE_PARAM
{
	BOOL fEnable;
	INT nRotate;
	INT	nTilt;
	BOOL fOutputImg;

	_PRECALIBRATE_PARAM()
	{
		fEnable = FALSE;
		nRotate = 0;
		nTilt = 0;
		fOutputImg = FALSE;
	}
}
PRECALIBRATE_PARAM;

typedef struct _VOTEINFO
{
	DWORD32 dwIndex;
	BOOL fVoted;

	_VOTEINFO()
	{
		dwIndex = 0;
		fVoted = FALSE;
	}
}
VOTEINFO;

typedef struct _DETECTAREA
{
	BOOL fEnableDetAreaCtrl;
	int DetectorAreaLeft;
	int DetectorAreaRight;
	int DetectorAreaTop;
	int DetectorAreaBottom;
	int nDetectorMinScaleNum;
	int nDetectorMaxScaleNum;

	_DETECTAREA()
	{
		fEnableDetAreaCtrl = FALSE;
		DetectorAreaLeft = 0;
		DetectorAreaRight = 100;
		DetectorAreaTop = 0;
		DetectorAreaBottom = 40;
		nDetectorMinScaleNum = 6;
		nDetectorMaxScaleNum = 12;
	}
}
DETECTAREA;

typedef struct _DBL_DETECTAREA
{
	BOOL fEnableDetAreaCtrl;
	int DetectorAreaLeft;
	int DetectorAreaRight;
	int DetectorAreaTop;
	int DetectorAreaBottom;
	int nDetectorMinScaleNum;
	int nDetectorMaxScaleNum;

	_DBL_DETECTAREA()
	{
		fEnableDetAreaCtrl = FALSE;
		DetectorAreaLeft = 0;
		DetectorAreaRight = 100;
		DetectorAreaTop = 0;
		DetectorAreaBottom = 75;
		nDetectorMinScaleNum = 4;
		nDetectorMaxScaleNum = 10;
	}
}
DBL_DETECTAREA;

typedef struct _PR_PARAM
{
	enum{MAX_DET_AREA = 16}; //最大可设检测区域数
	UINT nSize;		//参数结构体长度
	INT nMinPlateWidth;			//最小车牌检测宽度,默认为100
	INT nMaxPlateWidth;		//最大车牌检测宽度,默认为200
	INT	nVariance;					//车牌方差阈值,默认为30
	INT nDetAreaCount;	//检测区域数
	HV_RECT rgDetArea[MAX_DET_AREA];		//以百分数表示的检测区域,默认为0~100

	_PR_PARAM()
	{
		nSize = sizeof(_PR_PARAM);
		nMinPlateWidth = 60;
		nMaxPlateWidth = 250;
		nVariance = 30;
		nDetAreaCount = 1;
		for(int i = 0; i < MAX_DET_AREA; i++)
		{
			rgDetArea[i].left = 0;
			rgDetArea[i].top = 0;
			rgDetArea[i].right = 100;
			rgDetArea[i].bottom = 100;
		}	
	}
}
PR_PARAM;

typedef struct _PLATE_POS
{
	DWORD32 dwIndex;
	DWORD32 dwLeft;
	DWORD32 dwTop;
	DWORD32 dwRight;
	DWORD32 dwBottom;

	_PLATE_POS()
	{
		dwIndex = 0;
		dwLeft = 0;
		dwTop = 0;
		dwRight = 0;
		dwBottom = 0;
	}
} PLATE_POS;

typedef struct _TESTPERFORMANCE_INFO
{
	HV_COMPONENT_IMAGE* pImgDat;
	char* pszPerformanceReport;
	DWORD32 dwReportSize;
	DWORD32 dwMaxTestCount;
	DWORD32 dwMaxTestDuration;
}
TESTPERFORMANCE_INFO;

typedef struct _DEBUG_INSPECTOR_INFO
{
	UINT nFrameNo;	// 帧号
	int iHL;		// 亮度

	int nPlatePosLeft;
	int nPlatePosTop;
	int nPlatePosRight;
	int nPlatePosBottom;

	BYTE8 rgbContent[7];
	int rgfltConfidence[7];
	int AveragePosConfidence[7];

	UINT color;
	UINT nPlateType;
	int iLastDetWidth;
	int iLastCharHeight;
	int iLastVscale;
	BOOL fIndividualPlate;
	BOOL fYellowPlate;
	_DEBUG_INSPECTOR_INFO()
	{
		memset(this, 0, sizeof(_DEBUG_INSPECTOR_INFO));
	}
}
DEBUG_INSPECTOR_INFO;

enum VOTE_CONDITION {
	NEW2END,			//NEW状态下跟踪丢失
	LEAVE_TRACK_RECT,	//高速模式下车牌离开跟踪区域
	TRACK_MISS,			//在跟踪区域内跟踪丢失
	ENDRECT_MISS,		//在跟踪区域外跟踪丢失
	MAX_EQUAL_FRAME,	//连续识别结果相同
	MAX_VOTE_FRAME		//有效结果超过阈值
};

typedef struct _CORE_PERFORMANCE_TEST_DATA {
	BOOL m_fTestPerformance;			//性懿馐钥?
	char *m_pszPerformanceReport;		//测试报告缓冲区
	DWORD32 m_dwReportSize;				//测试报告缓冲区大小
	HV_COMPONENT_IMAGE	m_imgTestData;	//测试图片
	DWORD32 m_dwMaxTestCount;			//最大测试次数
	DWORD32 m_dwMaxTestDuration;		//最大测试时间(毫秒)
	HRESULT m_hrTestFunc;				//测试函数返回值
} CORE_PERFORMANCE_TEST_DATA;

extern CORE_PERFORMANCE_TEST_DATA g_CorePerformanceTestData;

class ITrackerCallback
{
public:
	STDMETHOD(CarArrive)(
		CARARRIVE_INFO_STRUCT *pCarArriveInfo,
		LPVOID pvUserData
	) = 0;

	STDMETHOD(CarLeft)(
		struct tagCarLeftInfoStruct *pCarLeftInfo,
		LPVOID pvUserData
	)=0;

	STDMETHOD(DisposeUserData)(
		UINT iFrameNo,
		UINT iRefTime,
		LPVOID pvUserData
	)
	{
		return S_OK;
	}

	//三个触发位置
	STDMETHOD(FirstTrigger)(
			LIGHT_TYPE lighttype,
			DWORD32 dwRoad,
			LPVOID pvOtherInfo
		)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(SecondTrigger)(
		LIGHT_TYPE lighttype,
		DWORD32 dwRoad,
		LPVOID pvOtherInfo
		)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(ThirdTrigger)(
		LIGHT_TYPE lighttype,
		DWORD32 dwRoad,
		LPVOID pvOtherInfo
		)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(ReaddSignal)(
	DWORD32 dwTriggerFlag
	)
	{
		return E_NOTIMPL;	
	}
	STDMETHOD(TrafficLightStatus)(
		UINT nLightCount,		//灯组数量
		DWORD32*	rgLightStatus,	//灯组状态
		INT*	pSceneStatus,	//用户根据灯组状态自定义的场景状态
		HV_RECT* rgLightPos,		//灯组坐标位置
		UINT nRedLightCount,   //红灯的数量
		HV_RECT* rgRedLightPos,	//红灯的坐标(用于红灯的加红)
		HV_COMPONENT_IMAGE* pSceneImage//全景图
		)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(TrafficSceneStatus)(
		TRAFFICLIGHT_SCENE tsSceneStatus	//用户自定义的场景状态
		)
	{
		return E_NOTIMPL;
	}

	//取当前红绿灯的状态,通过IPT
	STDMETHOD(GetTrafficLightStatus)(
		unsigned char* pbStatus
		)
	{
		return E_NOTIMPL;
	}

	//取当前红绿灯框位置信息
	STDMETHOD(GetTrafficLightPos)(
		PUINT cbLightCount,		//灯组数量
		HV_RECT* pLightPos,		//灯组坐标位置
		PUINT cbRedLightCount,   //红灯的数量
		HV_RECT* pRedLightPos	//红灯的坐标(用于红灯的加红)
		)
	{
		return E_NOTIMPL;
	}
};

#endif // _TRACKERCALLBACK_INCLUDED__
