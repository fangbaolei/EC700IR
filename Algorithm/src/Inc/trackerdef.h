// 该文件编码格式必须为WINDOWS-936格式

/**
* @file	trackerdef.h
* @version	1.0
* @brief Traker相关接口定义
*/

#ifndef _TRACKERCALLBACK_INCLUDED__
#define _TRACKERCALLBACK_INCLUDED__

#include "swobjbase.h"
#include "swimage.h"
#include "swimageobj.h"
#include "hvbaseobj.h"
#include "stdresultinfo.h"

#include "hvcoretype.h"
#include "swplate.h"

//参数结构体定义
#define BINARY_IMAGE_WIDTH 112				/**< 二值化图的宽度 */
#define BINARY_IMAGE_HEIGHT 20				/**< 二值化图的高度 */

#ifndef MAX_TRAFFICLIGHT_COUNT
#define MAX_TRAFFICLIGHT_COUNT 8           /**< 最大灯组数*/
#endif
#ifndef MAX_SCENE_COUNT
#define MAX_SCENE_COUNT 10                  /**< 最大场景数*/
#endif

namespace HiVideo
{
    //红绿灯状态
    typedef enum _TRAFFICLIGHT_LIGHT_STATUS
    {
        TLS_UNSURE = 0,
        TLS_RED,
        TLS_YELLOW,
        TLS_GREEN
    } LIGHT_STATUS;

    typedef struct _TRAFFICLIGHT_SCENE
    {
        LIGHT_STATUS lsLeft;
        LIGHT_STATUS lsForward;
        LIGHT_STATUS lsRight;
        LIGHT_STATUS lsTurn;
    } TRAFFICLIGHT_SCENE;
}

using namespace HiVideo;

/**
*  车道数据结构, 用车道线来标定
*/
typedef struct
{
    //边线的坐标与类型
    HV_POINT ptTop;
    HV_POINT ptBottom;
    int iLineType;
    int iRoadType;      //车道类型
    char szRoadTypeName[255];//车道类型名称
    int iFilterRunType; //要过滤的行驶类型
} ROAD_INFO;

/**
*  事件检测类型
*/
const int MAX_ROADLINE_NUM = 4;
typedef struct _ACTIONDETECT_PARAM_TRACKER
{
    int iDetectCarStopTime;                 	// 紧急停车判断时间，单位Ｓ
    int iIsYellowLine[MAX_ROADLINE_NUM];		// 0-01车道，1-12车道，以此类推
    int iIsCrossLine[MAX_ROADLINE_NUM];		    // 0-01车道，1-12车道，以此类推
} ACTIONDETECT_PARAM_TRACKER;

typedef enum _TRAFFICLIGHT_POSITION
{
    TP_UNSURE = 0,
    TP_LEFT,
    TP_FORWARD,
    TP_RIGHT,
    TP_TURN,
    MAX_TP_COUNT
} TRAFFICLIGHT_POSITION;

typedef enum
{
    NO_VIDEODET = 0,			// 不使用视频检测
    USE_BACK_DET_ONLY,			// 只使用背景检测
    USE_VIDEODET,				// 使用视频检测
    VIDEODET_MODE_COUNT
} HV_VIDEODET_MODE;


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
typedef enum _RUN_TYPE
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
typedef enum _RUN_ROAD_TYPE
{
    RRT_FORWARD = 0x01,
    RRT_LEFT    = 0x02,
    RRT_RIGHT	= 0x04,
    RRT_TURN	= 0x08,
    RRT_ESTOP   = 0x10,
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
typedef struct _OUTPUT_CAR_SIZE_INFO
{
    int nOutType;				// 输出类型 0:像素 1：米
    float iCarWidth;			// 车辆宽度
    float iCarHeight;			// 车辆高度
} OUTPUT_CAR_SIZE_INFO;

//  识别结果图片数据结构
typedef struct tagResultImageStruct
{
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

/**
* 车辆尺寸信息输出
*/
typedef struct _CAR_SIZE_INFO
{
    int nOutType;				// 输出类型 0:像素 1：米
    float iCarWidth;			// 车辆宽度
    float iCarHeight;			// 车辆高度
} CAR_SIZE_INFO;

/**
*  识别核心结果数据结构，由Tracker提供的
*/
typedef struct _PROCESS_IMAGE_CORE_RESULT
{
    // 所有元素4字节对齐
    BYTE8 rgbContent[8];	            // 车牌号码信息
    char szPlate[24];					// 车牌字符串
    int nType;						    // 车牌类型
    int nColor;						    // 车牌颜色
    float fltAverageConfidence;		// 可信度
    float fltFirstAverageConfidence;	// 首字符可信度
    int iAvgY;							// 环境光亮度
    int iCarAvgY;						// 车牌亮度
    int iCarVariance;					// 车牌对比度
    CAR_TYPE nCarType;				    // 车辆类型（大、中、小）
    CAR_COLOR nCarColor;				// 车辆颜色
    int nRoadNo;						// 车道号
    CAR_SIZE_INFO cCarSize;		        // 车辆尺寸
    DWORD32 nFrameNo;						// 结果帧号码
    DWORD32 nRefTime;						// 结果帧时间
    DWORD32 nStartFrameNo;					// 开始帧号码
    DWORD32 nEndFrameNo;					// 结束帧号码
    DWORD32 nFirstFrameTime;				// 开始跟踪时间
    int iObservedFrames;				// 观察帧数
    int iVotedObservedFrames;			// 最后投票车牌类型的有效帧数
    BOOL fReverseRun;					// 车辆反向运动标志
    int nVoteCondition;					// 投票条件
    //判断是压实线还是双黄线
    BOOL fIsDoubleYellowLine;

    //闪光抓拍的几个时间
    DWORD32 dwFirstTime;
    DWORD32 dwSecondTime;
    DWORD32 dwThirdTime;

    CROSS_OVER_LINE_TYPE coltIsOverYellowLine;		//压黄线
    CROSS_OVER_LINE_TYPE coltIsCrossLine;			//越线

    int nCarLeftCount;					// 车辆离开帧数计数器
    float fltCarspeed;					// 车辆速度
    float fltScaleOfDistance;			// 距离计算的误差比例
    int nPlateLightType;				// 当前车牌亮度级别
    int iCplStatus;						// 偏光镜状态
    int iPulseLevel;                     //补光脉宽等级
    UINT nCarArriveTime;				// CarArrive的时间
    HV_RECT        rcBestPlatePos;		// 最清晰车牌坐标
    HV_RECT        rcLastPlatePos;		// 最后出现车牌坐标
    HV_RECT        rcFirstPos;		    // 第一个抓拍位置车牌的坐标
    HV_RECT        rcSecondPos;		    // 第二个抓拍位置车牌的坐标
    HV_RECT        rcThirdPos;		    // 第三个抓拍位置车牌的坐标
    int nPlateWidth;                    // 车牌小图宽度
    int nPlateHeight;                   // 车牌小图高度
    BOOL fCarIsStop;					// 是否停止车
    int iCarStopPassTime;				// 停止车经过时间

    TRAFFICLIGHT_SCENE tsOnePosScene;		//车牌第一个位置的红绿灯场景
    TRAFFICLIGHT_SCENE tsTwoPosScene;		//车牌第二个位置的红绿灯场景

    HV_RECT         rcRedLightPos[20];		//红灯位置坐标
    int             rcRedLightCount;			//红灯数量
    CRect rgFaceRect[20];
    int nFaceCount;

    int iCapturerAvgY;
    bool fIsCapture;
	bool fIsNight;						//夜晚场景标志

    //红灯开始时间(相对时间,毫秒)
    LIGHT_REALTIME lrtRedStart;

    //这个违章结果是否不确定
    int iUnSurePeccancy;
    // 所处理的视频ID
    int iVideoID;
    // 强制发送抓拍图片开关
    int iSendCapImageFlag;
    //车辆行驶类型
    RUN_TYPE rtType;
    //违章类型
    int ptType;
    //越线检测开关
    int nDetectCrossLineEnable;
    //压黄线检测开关
    int nDetectOverYellowLineEnable;
    //识别结果图片集
    RESULT_IMAGE_STRUCT cResultImg;
    DWORD32 dwBestSnapShotRefTime;
    DWORD32 dwLastSnapShotRefTime;
    DWORD32 dwBeginCaptureRefTime;
    DWORD32 dwBestCaptureRefTime;
    DWORD32 dwLastCaptureRefTime;

    DWORD32 dwTriggerTime;
	//抓拍标志
	DWORD32 dwTriggerIndex;
	//匹配类型 0:匹配失败 1:根据抓拍标志匹配 2:根据时间匹配
	DWORD32 dwMatchResult;

	//实际触发抓拍的数量
	int    iCapCount;
	//抓拍时的速度
	int    iCapSpeed;
	int    iMatchCount;

	//颜色HSL分量
    int iH;
    int iS;
    int iL;

    int nWdrLevel;

    _PROCESS_IMAGE_CORE_RESULT()
    {
        memset(this, 0, sizeof(_PROCESS_IMAGE_CORE_RESULT));
        coltIsOverYellowLine	= COLT_NO;
        coltIsCrossLine		= COLT_NO;
    }
} PROCESS_IMAGE_CORE_RESULT;

/**
*  识别外部结果数据结构
*/
typedef struct _PROCESS_IMAGE_OTHER_INFO
{
    // 所有元素4字节对齐
    UINT iFrameNo;						// 结果帧号码
    UINT iRefTime;						// 结果帧时间
    UINT nStartFrameNo;					// 开始帧号码
    UINT nEndFrameNo;					// 结束帧号码
    UINT nFirstFrameTime;				// 开始跟踪时间
    int iCarAvgY;						// 车牌亮度
    int iCarVariance;					// 车牌对比度

    //new add(todo: 新加入的内容，之后需要考虑是否要再分类)
    int nPlateLightType;                // 当前车牌亮度级别
    int iRoadNumberBegin;               // 车道编号的方向 0:从左开始, 1:从右开始
    int iStartRoadNum;                  // 起始车道号
    int iObservedFrames;				// 观察帧数
    int nCarLeftCount;					// 车辆离开帧数计数器
    UINT nCarArriveTime;                // CarArrive的时间

    _PROCESS_IMAGE_OTHER_INFO()
    {
        memset(this, 0, sizeof(_PROCESS_IMAGE_OTHER_INFO));
    }
} PROCESS_IMAGE_OTHER_INFO;

/**
*  CarLeft参数数据结构
*/
typedef struct tagCarLeftInfoStruct
{
    PROCESS_IMAGE_CORE_RESULT cCoreResult;
    PROCESS_IMAGE_OTHER_INFO cOtherInfo;
} CARLEFT_INFO_STRUCT;					// 车辆离开信息

/**
*  CarArrive参数数据结构
*/
typedef struct tagCarArriveInfoStruct
{
    int iRoadNumber;
    UINT iPlateLightType;
    UINT dwTriggerOutDelay;
    UINT dwIsOutTrig;
    UINT dwCarArriveTime;
    UINT dwCarArriveRealTime;
    DWORD32 dwFirstPos;
    DWORD32 dwEndPos;
    tagCarArriveInfoStruct()
    {
        memset(this, 0, sizeof(tagCarArriveInfoStruct));
        iRoadNumber = -1;
    }
} CARARRIVE_INFO_STRUCT;

const DWORD32 EVENT_NONE = 0;
const DWORD32 EVENT_CARARRIVE = 0x0001;
const DWORD32 EVENT_CARLEFT = 0x0002;
const DWORD32 EVENT_FRAME_RECOED = 0x0004;
const int MAX_EVENT_COUNT = 8;

/**
*  跟踪框信息
*/
typedef struct _TRACK_RECT_INFO
{
    DWORD32 dwTrackCount;
    CRect rgTrackRect[30];

    _TRACK_RECT_INFO()
    {
        memset(this, 0, sizeof(_TRACK_RECT_INFO));
    }
} TRACK_RECT_INFO;

typedef struct _SYNC_DET_DATA
{
    void* pbData;
    int nLen;
    _SYNC_DET_DATA()
    :   pbData(NULL)
    ,   nLen(0)
    {
    }
} SYNC_DET_DATA;
/**
*  帧处理事件数据结构
*/
typedef struct tagPrcoessEventInfoStruct
{
    DWORD32 dwEventId;
    LIGHT_TYPE cLightType; // 车牌亮度等级
    int iCplStatus;		   // 偏光镜状态
    int nIOLevel;		  //IO状态，红绿灯信息
    int iCarArriveTriggerType; //0-不关心;1-不触发抓拍;2-触发抓拍
    BOOL fIsCheckLight;
    int nEnvLightType; // 0白天，1傍晚，2晚上
    int iPulseLevel;       //补光脉宽等级
    int iFrameAvgY;   //帧平均亮度
    HV_RECT rcRedLight[20];
    TRACK_RECT_INFO cTrackRectInfo;
    int iCarArriveInfoCount;
    int iCarLeftInfoCount;
    CARARRIVE_INFO_STRUCT rgCarArriveInfo[MAX_EVENT_COUNT];
    CARLEFT_INFO_STRUCT rgCarLeftInfo[MAX_EVENT_COUNT];
    SYNC_DET_DATA cSyncDetData;  //异步检测数据
    int nWDRLevel;	// WDR等级
	int rgiAllCarTrigger[MAX_EVENT_COUNT];
    tagPrcoessEventInfoStruct()
    {
        dwEventId = EVENT_NONE;
        iCarArriveInfoCount = 0;
        iCarLeftInfoCount = 0;
        nIOLevel = 0;
        fIsCheckLight = true;
        nEnvLightType = 0;
        iPulseLevel = 0;
        nWDRLevel = 0;
        memset(rcRedLight, 0, sizeof(rcRedLight));
		memset(rgiAllCarTrigger, 0, sizeof(rgiAllCarTrigger));
    }
} PROCESS_EVENT_STRUCT;

/**
*  车辆检测帧处理事件数据结构
*/
typedef struct tagPrcoessDetectEventInfoStruct
{
    HV_RECT rgcDetect[20];
    int rgiDetectType[20];
    int iDetectCount;
    tagPrcoessDetectEventInfoStruct()
    {
        memset(this, 0, sizeof(tagPrcoessDetectEventInfoStruct));
    }
} PROCESS_DETECT_INFO;


//复制CARLEFT_INFO_STRUCT
HRESULT CopyCarLeftInfoStruct(
    CARLEFT_INFO_STRUCT *pDest,
    const CARLEFT_INFO_STRUCT *pSrc
);

//释放CARLEFT_INFO_STRUCT内部分配内存
HRESULT FreeCarLeftInfoStruct(
    CARLEFT_INFO_STRUCT *pObj
);


/**
*  帧属性数据结构
*/
typedef struct _FRAME_PROPERTY
{
    INT iWidth;
    INT iHeight;
    INT iStride;
}
FRAME_PROPERTY;

/**
* 视频检测参数
*/
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

/**
* 预校正参数
*/
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

/**
* 扫描区域参数
*/
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

//四边形扫描区域四个点的坐标
typedef struct TRAP_DETAREA
{
	BOOL fEnableDetAreaCtrl;
	int TopLeftX;
	int TopLeftY;
	int TopRightX;
	int TopRightY;
	int BottomLeftX;
	int BottomLeftY;
	int BottomRightX;
	int BottomRightY;
	int nDetectorMinScaleNum;
	int nDetectorMaxScaleNum;
	TRAP_DETAREA()
	{
		fEnableDetAreaCtrl = FALSE;
		TopLeftX = 0;
		TopLeftY = 0;
		TopRightX = 100;
		TopRightY = 100;
		BottomLeftX = 0;
		BottomLeftY = 0;
		BottomRightX = 100;
		BottomRightY = 100;
		nDetectorMinScaleNum = 6;
		nDetectorMaxScaleNum = 12;
	}
}TRAP_DETAREA;

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

//todo: 需要处理——注释中的nMinPlateWidth，nMaxPlateWidth默认值与构造方法中的初始化值不同。
//attendtion by Shaorg： MAX_DET_AREA不可更改，否则产生Bug，因为在其它地方用了16这个与MAX_DET_AREA同义的量。
typedef struct _PR_PARAM
{
    enum{MAX_DET_AREA = 16};           //最大可设检测区域数
    UINT nSize;                         //参数结构体长度
    INT nMinPlateWidth;			        //最小车牌检测宽度,默认为100
    INT nMaxPlateWidth;		            //最大车牌检测宽度,默认为200
    INT	nVariance;				        //车牌方差阈值,默认为30
    INT nDetAreaCount;	                //检测区域数
    INT nRoadNum;                       //车道号
    HV_RECT rgDetArea[MAX_DET_AREA];    //以百分数表示的检测区域,默认为0~100

    _PR_PARAM()
    {
        nSize = sizeof(_PR_PARAM);
        nMinPlateWidth = 60;
        nMaxPlateWidth = 250;
        nVariance = 30;
        nDetAreaCount = 1;
        nRoadNum = -1;
        for (int i = 0; i < MAX_DET_AREA; i++)
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

enum VOTE_CONDITION
{
    NEW2END,			//NEW状态下跟踪丢失
    LEAVE_TRACK_RECT,	//高速模式下车牌离开跟踪区域
    TRACK_MISS,			//在跟踪区域内跟踪丢失
    ENDRECT_MISS,		//在跟踪区域外跟踪丢失
    MAX_EQUAL_FRAME,	//连续识别结果相同
    MAX_VOTE_FRAME		//有效结果超过阈值
};

typedef struct _CORE_PERFORMANCE_TEST_DATA
{
    BOOL m_fTestPerformance;			//性能测试标记
    char *m_pszPerformanceReport;		//测试报告缓冲区
    DWORD32 m_dwReportSize;				//测试报告缓冲区大小
    HV_COMPONENT_IMAGE	m_imgTestData;	//测试图片
    DWORD32 m_dwMaxTestCount;			//最大测试次数
    DWORD32 m_dwMaxTestDuration;		//最大测试时间(毫秒)
    HRESULT m_hrTestFunc;				//测试函数返回值
} CORE_PERFORMANCE_TEST_DATA;

typedef struct _TRAFFICLIGHT_CFG_PARAM
{
    int nCheckType;         //红绿灯检测方式
    int nAutoScanLight;     //动态检测红绿灯位置开关
    int nSenceCheckMode;    //场景检测模式
    int nStopLinePos;       //直行停止线位置
    int nLeftStopLinePos;   //左转停止线位置
    int nMatchScanSpeed;    //扫描速度
    int nLightCount;        //灯组数量
    int nValidSceneStatusCount;  //有效场景状态计数
    int nOutputInLine;      //输出未过停止线的车辆
    int nRedLightDelay;     //多色灯黄灯变红灯间隔(秒)
    int nSpecificLeft;      //左转道相对
    int nRunTypeEnable;     //行驶类型强制输出
    int nSceneCount;        //红绿灯场景数
    char rgszLightPos[MAX_TRAFFICLIGHT_COUNT][64];  //灯组信息
    char rgszScene[MAX_SCENE_COUNT][32];  //场景信息

    int  nTrafficLightThread;
    int  nTrafficLightBrightValue;
    int  nAutoScanThread;
    int  nSkipFrameNO;
    BOOL fIsLightAdhesion;
    BOOL fDeleteLightEdge;
    int fltHMThreld;       //白天阈值
    int fltHMThreldNight;  //夜景阈值
    //模板匹配直方图延迟更新帧数
    int iHMDelay;             //白天延迟帧数
    int iHMDelayNight;        //夜景延迟帧数
    int	iMaxMatchDiff;
    int iRealForward;
    int  nFilterRushPeccancy;//过滤第3张图为绿灯的情况
    int  nFilterMoreReview;  //过滤大车车身多检开关
    char szIOConfig[255];	 //外接红绿灯信号
    BOOL fEnhanceRedLight;   //是否进行白色区域加红
    BOOL fFilterNoPlatePeccancy;

    _TRAFFICLIGHT_CFG_PARAM()
    {
        nCheckType = 0;
        nAutoScanLight = 1;
        nSenceCheckMode = 0;
        nStopLinePos = 65;
        nLeftStopLinePos = 65;
        nMatchScanSpeed = 0;
        nLightCount = 0;
        nValidSceneStatusCount = 5;
        nOutputInLine = 1;
        nRedLightDelay = 0;
        nSpecificLeft = 0;
        nRunTypeEnable = 0;
        nSceneCount = 0;

        nTrafficLightThread = 3;
        nTrafficLightBrightValue = 3;
        nAutoScanThread = 10;
        nSkipFrameNO = 30;
        fIsLightAdhesion = FALSE;
        fDeleteLightEdge = FALSE;
        fFilterNoPlatePeccancy = FALSE;

        for (int i = 0; i < MAX_TRAFFICLIGHT_COUNT; i++)
        {
            strcpy(rgszLightPos[i], "(0,0,0,0),0,0,00");
        }
        for (int i = 0; i < MAX_SCENE_COUNT; i++)
        {
            strcpy(rgszScene[i], "(0,0,0,0),0");
        }

        nFilterRushPeccancy = 0;
        nFilterMoreReview = 0;
        fltHMThreld = 48;       //白天阈值
        fltHMThreldNight = 48;  //夜景阈值
        //模板匹配直方图延迟更新帧数
        iHMDelay = 7;             //白天延迟帧数
        iHMDelayNight = 7;        //夜景延迟帧数
        iMaxMatchDiff = 18;
        iRealForward = 40;

        strcpy(szIOConfig, "00,00,00,00,00,00,00,00");

        fEnhanceRedLight = TRUE;
    }
} TRAFFICLIGHT_CFG_PARAM;

typedef struct _SCALE_SPEED_PARAM
{
    BOOL fEnable;    //软件测速开关
    int nMethodsFlag;   //计算模式，0:平面标定法, 1:空间标定法
    float fltLeftDistance;  //屏幕下沿到龙门架距离
    float fltFullDistance;  //屏幕上沿到龙门架距离
    float fltCameraHigh;    //摄像机高度
    float fltAdjustCoef;    //修正系数
    float rgfltTransMarix[11];  //计算图像坐标到空间坐标的变换矩阵

    float fltDistance;      // 屏幕上沿到下沿的距离(米)
    float fltRoadWidth;     // 单个车道宽度(米)

    _SCALE_SPEED_PARAM()
    {
        fEnable = FALSE;
        nMethodsFlag = 0;
        fltLeftDistance = 25.0f;
        fltFullDistance = 75.0f;
        fltCameraHigh = 6.5f;
        fltAdjustCoef = 1.050f;
        fltDistance = 90;
        fltRoadWidth = 3.75;
        rgfltTransMarix[0] = 165.472f;
        rgfltTransMarix[1] = -38.447f;
        rgfltTransMarix[2] = -4.968f;
        rgfltTransMarix[3] = 606.269f;
        rgfltTransMarix[4] = -2.743f;
        rgfltTransMarix[5] = 1.813f;
        rgfltTransMarix[6] = -177.545f;
        rgfltTransMarix[7] = 1089.211f;
        rgfltTransMarix[8] = 0.000f;
        rgfltTransMarix[9] = -0.035f;
        rgfltTransMarix[10] = -0.005f;
    }
}SCALE_SPEED_PARAM;

typedef struct _VIDEODET_CFG_PARAM
{
    int nVideoDetMode;  //视频检测模式
    int nDayBackgroudThreshold;     //白天背景检测阈值
    int nNightBackgroudThreshold;   //晚上背景检测阈值
    int nVoteFrameCount;    //投票有效帧数，当帧数大于此值就投票
    int nCheckAcross;       //是否检测横向行驶的物体
    int nShadowDetMode;     //阴影检测模式
    int nShadowDetEffect;   //阴影检测强弱程度值
    int nDiffType; // 前景提取方式
    int nMedFilter; // 抑制抖动开关
    int nMedFilterUpLine; // 抑制抖动作用区域上区域
    int nMedFilterDownLine; // 抑制抖动作用区域下区域
    int nBigCarDelayFrameNo;
    CRect rcVideoDetArea;   //视频检测区域
	int iEPOutPutDetectLine;         //电警检测到跟踪模块交接线
	int iEPFGDetectLine;
	int iNoPlateDetMode;     //无牌车检测模式 0:标准模式 1:模板模式
	int iEPUseCarTailModel; //电警无牌车检测是否使用模型过滤多检
	int iEPNoPlateScoreThreld; //无牌车过滤多检的分数阈值

    _VIDEODET_CFG_PARAM()
    {
        nVideoDetMode = USE_VIDEODET;
        nDayBackgroudThreshold = 20;
        nNightBackgroudThreshold = 50;
        nVoteFrameCount = 30;
        nCheckAcross = 0;
        nShadowDetMode = 0;
        nShadowDetEffect = 0;
        nDiffType = 0;
        nMedFilter = 0;
        nMedFilterUpLine = 0;
        nMedFilterDownLine = 0;
        rcVideoDetArea.SetRect(0, 0, 100, 100);
        nBigCarDelayFrameNo = 32;
		iEPOutPutDetectLine = 63;         //电警检测到跟踪模块交接线
		iEPFGDetectLine = 45;             //无牌车检测屏蔽区域(距离顶部百分比)
		iNoPlateDetMode = 0;
		iEPUseCarTailModel = 0;
		iEPNoPlateScoreThreld = 300;
    }
}VIDEODET_CFG_PARAM;

// Tracker可配置参数
typedef struct _TRACKER_CFG_PARAM
{
    BOOL fEnableVote;	// 投票开关
    BOOL fAlpha5;		// 第5位字母识别开关
    BOOL fAlpha6;		// 第6位字母识别开关
    BOOL fAlpha7;		// 第7位字母识别开关
    BOOL fEnableRecgCarColor;       // 车身颜色识别开关
    BOOL fEnableFlashLight; //抓拍补光灯模式开关
    BOOL fDoublePlateEnable;//双层牌检测开关
    BOOL fUseEdgeMethod; // 加强清晰图识别
    BOOL fEnableT1Model; // T-1模型开关
    BOOL fEnableDefaultWJ; // 本地新武警字符开关
    BOOL fEnableAlpha_5; // 黄牌字母识别开关
    BOOL fOutputCarSize; // 输出车辆尺寸
    BOOL fOnePlateMode; // 检测单车牌
    BOOL fEnableDetFace; // 人脸检测开关

    float fltMinConfForOutput;      // 可信度阈值
    float fltPlateDetect_StepAdj;   // 步长调整系数
    float fltOverLineSensitivity;   // 压线检测灵敏度

    int nFirstPlatePos;             // 优先出牌方位
    int nPlateEnhanceThreshold;     // 车牌增强阈值
    int nMiddleWidth;   // 屏幕中间单层蓝牌后五字宽度
    int nBottomWidth;   // 屏幕底部单层蓝牌后五字宽度
    BOOL fProcessPlate_BlackPlate_Enable; //使能黑牌判断规则(黑牌可变蓝牌)
    int nProcessPlate_BlackPlate_S; // 黑牌的饱和度上限
    int nProcessPlate_BlackPlate_L; // 黑牌亮度上限
    int nProcessPlate_BlackPlateThreshold_H0; // 蓝牌色度下限
    int nProcessPlate_BlackPlateThreshold_H1; // 蓝牌色度上限
    int nProcessPlate_LightBlue;    // 浅蓝牌开关
    int nPlateDetect_Green; // 绿牌识别开关
    int nNightThreshold; // 夜晚模式亮度上限
    int nPlateLightCheckCount; // 亮度调整间隔
    int nMinPlateBrightness; // 车牌最低亮度
    int nMaxPlateBrightness; // 车牌最高亮度
    int nCtrlCpl; // 控制偏光镜
    int nLightTypeCpl; // 偏光镜控制亮度等级
    int nEnablePlateEnhance; // 分割前是否进行图片增强
    int nPlateResizeThreshold; // 分割前小图拉伸宽度阈值
    int nEnableBigPlate; // 大牌识别开关
    int nRecogAsteriskThreshold; //
    int iEddyType;          // 图片旋转(0:不旋转,1:逆时针旋转90度)
    int nRoadLineNumber;    // 车道线数量
    int nOutputRoadTypeName;//车道类型输出开关
    int nCaptureOnePos;     //抓拍第一张图的位置
    int nCaptureTwoPos;     //抓拍第二张图的位置
    int nCaptureThreePos;   //抓拍第三张图的位置
    int nDetReverseRunEnable;   // 检测逆行开关
    int nSpan;  //过滤慢速逆行车辆跨度
    int iRoadNumberBegin;   //车道编号的方向0:从左开始,1:从右开始
    int iStartRoadNum;  //起始车道号
    int nFlashLightThreshold;   //抓拍补光亮度阈值
    int nRecogGxPolice; // 地方武警牌识别开关
	int nRecogNewMilitary;//新军牌识别开关
    int nCarArrivedPos; // 车辆到达位置（除黄牌外）
    int nCarArrivedPosNoPlate; // 无牌车到达位置
    int nCarArrivedPosYellow; //车辆到达位置（黄牌）
    int nProcessType; // 处理类型
    int nOutPutType; // 输出类型
    int nNightPlus; // 晚上加强非机动车检测
    int nWalkManSpeed; // 行人判断灵敏度
    int nCarArrivedDelay; // 车辆到达触发延迟距离(米)
    int nVoteFrameNum; // 投票结果数
    int nMaxEqualFrameNumForVote; // 连续相同结果出牌条件
    int nBlockTwinsTimeout; // 相同结果最小时间间隔(S)
    int nAverageConfidenceQuan; // 平均得分下限
    int nFirstConfidenceQuan; // 汉字得分下限
    int nRemoveLowConfForVote; // 投票前去掉低得分车牌百分比
    int nVoteCount; // 投票数
    int nSegHeight; // 分割参考高度
    int nDoublePlateTypeEnable; // 默认双层牌类型开关
    int nDefaultDBType; // 默认双层牌类型
    int nMinFaceScale; // 最小人脸宽度
    int nMaxFaceScale; // 最大人脸宽度
    float fltBikeSensitivity; // 非机动车灵敏度
    char szDefaultWJChar[4]; // 本地新武警字符

    int nSegWhitePlate; //白牌强制分割

    int nCarArriveTrig; //是否触发抓拍
	int nTriggerPlateType;//触发抓拍的车牌类型
    int nEnableProcessBWPlate;//是否处理半黑半白牌

    ROAD_INFO rgcRoadInfo[MAX_ROADLINE_NUM];    //车道线信息
    ACTIONDETECT_PARAM_TRACKER cActionDetect;   //事件检测参数
    DETECTAREA cDetectArea;	// 扫描区域
	TRAP_DETAREA cTrapArea; //梯形扫描区域
    DETECTAREA cRecogSnapArea; // 抓拍图识别参数
    TRAFFICLIGHT_CFG_PARAM cTrafficLight;   //红绿灯参数
    SCALE_SPEED_PARAM cScaleSpeed;  //软件测速参数
    VIDEODET_CFG_PARAM cVideoDet;   //视频检测参数

	DSPLinkBuffer cTriggerInfoBuf;    //保存触发抓拍时的信息
	int iPlatform;        				//平台 0:一体机 1:单板
	int iSpeedLimit;      				//限速值
	int iCapMode;    				//抓拍模式 (0:抓拍一次; 1:超速违章抓拍2次; 2:全部抓拍2次)
	BOOL fPreVote;
	//车身颜色识别控制
	int nEnableColorReRecog;
	int nWGColorReThr;
	int nWColorReThr;
	int nBColorReThr;
	BOOL fEnableCorlorThr;
	int nBGColorThr;
	int nWGColorThr;

    //以下参数为视频流收费站专用
    int iOutPutNoPlate; //是否输出无牌车
    int iScanAreaTopOffset;  // 扫描区顶部离触发线百分比
    int iScanAreaBottomOffset; // 扫描区底部离触发线百分比
    int iBuleTriggerToYellowOffset; // 蓝牌触发线到黄牌触发线的百分比
    int iPrevTime; //向前匹配时间
    int iPostTime; //向后匹配时间
    int iForceLightOffAtDay;
	int iForceLightOffAtNight;
	int iDisableFlashAtDay;
	int iForceLightThreshold;

	int iMovingDirection; //行驶方向
    BOOL fFilterReverseEnable;        //过滤逆行车辆标志
    int nRecogInVaildRatioTH;         // 识别无效次数比率阈值，百分比*100，0~100，大于该百分比则不出结果

    int iUsedLight; // 是否使用频闪补光灯
    
    //收费站停止触发线偏移，停止后若在非黄牌触发线往上偏移m_nStopArriveLineOffset以内
    //可直接出牌，黄牌自动再往上偏移10。
    int nStopArriveLineOffset;

    //收费站与投票结果最小相似帧数，达不到则不出结果
    int nTollOutResultMinSimiFrame;

    int nPlateTrackerOutLine;

    int nMaxAGCTH;
	int iImageConstDelay;		//图像延时到达时间

	int nDuskMaxLightTH;
	int nNightMaxLightTH;

    _TRACKER_CFG_PARAM()
    {
        fEnableVote = TRUE;
        fAlpha5 = FALSE;
        fAlpha6 = FALSE;
        fAlpha7 = FALSE;
        fEnableRecgCarColor = FALSE;
        fEnableFlashLight = FALSE;
        fltMinConfForOutput = 0.7f;
        nPlateEnhanceThreshold = 40;
        fProcessPlate_BlackPlate_Enable = FALSE;
        nProcessPlate_BlackPlate_S = 0;
        nProcessPlate_BlackPlate_L = 5;
        nProcessPlate_BlackPlateThreshold_H0 = 5;
        nProcessPlate_BlackPlateThreshold_H1 = 230;
        nProcessPlate_LightBlue = 0;
        nPlateDetect_Green = 0;
        nNightThreshold = 55;
        nPlateLightCheckCount = 5;
        nMinPlateBrightness = 80;
        nMaxPlateBrightness = 120;
        nCtrlCpl = 0;
        nLightTypeCpl = ((int)LIGHT_TYPE_COUNT / 2);
        nEnablePlateEnhance = 0;
        nPlateResizeThreshold = 120;
        nEnableBigPlate = 0;
        nFirstPlatePos = 0;
        fltPlateDetect_StepAdj = 1.5f;
        nMiddleWidth = 70;
        nBottomWidth = 90;
        iEddyType = 0;
        fltOverLineSensitivity = 2.75f;
        nRoadLineNumber = 4;
        nOutputRoadTypeName = 0;
        nCaptureOnePos = 0;
        nCaptureTwoPos = 0;
        nCaptureThreePos = 0;
        cActionDetect.iDetectCarStopTime = 10;
        nDetReverseRunEnable = 1;
        nSpan = 5;
        iRoadNumberBegin = 0;
        iStartRoadNum = 1;
        nFlashLightThreshold = 25;
        fDoublePlateEnable = 1;
        nRecogAsteriskThreshold = 0;
        fUseEdgeMethod = FALSE;
        fEnableT1Model = FALSE;
        fEnableDefaultWJ = FALSE;
        nRecogGxPolice = 0;
		nRecogNewMilitary = 0;
        fEnableAlpha_5 = FALSE;
        nCarArrivedPos = 50;
        nCarArrivedPosNoPlate = 50;
        nCarArrivedPosYellow = 40;
        nProcessType = 0;
        nOutPutType = 0;
        nNightPlus = 0;
        nWalkManSpeed = 100;
        fltBikeSensitivity = 4.0f;
        nCarArrivedDelay = 0;
        nVoteFrameNum = 35;
        nMaxEqualFrameNumForVote = 15;
        nBlockTwinsTimeout = 30;
        nFirstConfidenceQuan = 0;
        nRemoveLowConfForVote = 40;
        nAverageConfidenceQuan = 0;
        fOutputCarSize = FALSE;
        memset(szDefaultWJChar, 0, 4);
        memcpy(szDefaultWJChar, "16", 2);
        nVoteCount = 3;
        fOnePlateMode = TRUE;
        nSegHeight = 18;
        nDoublePlateTypeEnable = 0;
        nDefaultDBType = 0;

        nSegWhitePlate = 0;

        nCarArriveTrig = 1;
        nTriggerPlateType = 0;
        nEnableProcessBWPlate = 0;

		iPlatform = 0;
		iSpeedLimit = 0;
		iCapMode = 0;

		iOutPutNoPlate = 0;
		iScanAreaTopOffset = 28;
		iScanAreaBottomOffset = 12;
		iBuleTriggerToYellowOffset = 8;
        iPrevTime = 5000;
        iPostTime = 0;

		fPreVote = TRUE;
        fEnableDetFace = FALSE;
        nMinFaceScale = 2;
        nMaxFaceScale = 10;
		//车身颜色识别控制
		nEnableColorReRecog = 0;
		nWGColorReThr = 60;
		nWColorReThr = 150;
		nBColorReThr = 20;
		fEnableCorlorThr = false;
		nBGColorThr = 25;
		nWGColorThr = 150;

        nRecogInVaildRatioTH = 40;
        for (int i = 0; i < MAX_ROADLINE_NUM; i++)
        {
            memset(&rgcRoadInfo[i], 0, sizeof(ROAD_INFO));
            strcpy(rgcRoadInfo[i].szRoadTypeName, "NULL");
            rgcRoadInfo[i].iLineType = 1;
            rgcRoadInfo[i].iRoadType = (int)(RRT_FORWARD | RRT_LEFT | RRT_RIGHT | RRT_TURN);
            cActionDetect.iIsYellowLine[i] = 0;
            cActionDetect.iIsCrossLine[i] = 0;
        }
        iForceLightOffAtDay = 0;
		iForceLightOffAtNight = 0;
		iDisableFlashAtDay = 0;
		iForceLightThreshold = 100;

		iMovingDirection = 4;
        fFilterReverseEnable = false;

        iUsedLight = 1; // 是否使用频闪补光灯

        nStopArriveLineOffset = 10;

        nTollOutResultMinSimiFrame = 5;

        nPlateTrackerOutLine = 20;

        nMaxAGCTH = 150;
		iImageConstDelay = 0;

		nDuskMaxLightTH=60;
		nNightMaxLightTH=13;
    }
} TRACKER_CFG_PARAM;

extern CORE_PERFORMANCE_TEST_DATA g_CorePerformanceTestData;

typedef struct _TRIGGER_CAM_INFO
{
    DWORD32     dwCarArriveTime;	//触发时的Tick
	DWORD32     dwRoadId;          //车道
	DWORD32     dwTriggerIndex;   //触发序号

	_TRIGGER_CAM_INFO()
	{
		memset(this, 0, sizeof(*this));
	}
} TRIGGER_CAM_INFO;


#endif // _TRACKERCALLBACK_INCLUDED__
