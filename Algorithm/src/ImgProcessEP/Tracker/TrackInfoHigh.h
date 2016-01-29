#ifndef _TRACKINFO_INCLUDED__
#define _TRACKINFO_INCLUDED__

#include <string.h>

#include "swimage.h"
#include "hvbaseobj.h"
#include "swplate.h"
#include "swimageobj.h"
#include "platerecogparam.h"

#include "DspLinkCmd.h"

#include "ItgArea.h"

#include "ObjMatch.h"
#include "HvImageNew.h"
#include "ScaleSpeed.h"


extern int GetAllPoints(HV_POINT* pAll, int iY, int *iPointCount);
extern BOOL IsOutSideRoad(int iX, int iY);


/**
*  Tracker state define
*/
enum TRACK_STATE
{
	tsInit = 0,
	tsNew,
	tsMoving,
	tsEnd,
	tsWaitRemove,
	tsTemTrack,				// 模版跟踪状态
	tsTemTrackEnd			// 模版跟踪结束状态
};

// 新增加车牌信息类型
enum PLATEINFO_TYPE
{
	PI_LPR = 0,				// 车牌识别类型
	PI_TEM					// 模版匹配类型
};

struct CarSizeInfo
{
	HiVideo::CRect rcFG;		// 车辆外围轮廓(来自视频检测)
	float fltCarW;				// 车宽
	float fltCarH;				// 车长
};

typedef struct _CARCOLOR_INFO
{
	DWORD32 dwCarColorType;
	char szName[24];
	DWORD32 dwCarColorID;
} CARCOLOR_INFO;

extern _CARCOLOR_INFO g_rgCarColorInfo[];
#define CARCOLOR_NUM		ARRSIZE(g_rgCarColorInfo)

struct BestPlateInfo
{
	BestPlateInfo()
	{
		pimgPlate = NULL;
		fltConfidence = 0;
	}
	~BestPlateInfo()
	{
	}
	void Clear()
	{
		fltConfidence = 0;
		if (pimgPlate != NULL)
		{
			pimgPlate->Release();
			pimgPlate = NULL;
		}
	}
	HRESULT Detach(BestPlateInfo& dst)
	{
		dst = *this;
		pimgPlate = NULL;
		fltConfidence = 0;
		return S_OK;
	}
	IReferenceComponentImage *pimgPlate;
	CParamQuanNum fltConfidence;
	HiVideo::CRect rcPlatePos;				// 分割后的外围位置
	DWORD32 dwFrameTime;					// 当前帧时间
	HiVideo::CRect rcPos;					// 车牌在整幅图像中的位置
};

struct PlateInfo
{
	PlateInfo()
		: color(PC_UNKNOWN)
		, nPlateType(PLATE_UNKNOWN)
		, iLastCharHeight(-1)
		, iLastVscale(-1)
		, fltTotalConfidence(0)
		, nVariance(0)
		, nAvgY(0)
		, fValid(true)
		, nInfoType(PI_LPR)			// 默认是车牌识别类型
		, nCarType(CT_UNKNOWN)		// 车辆类型未知
		, nCarColour(CC_UNKNOWN)
		, fReRecog(true)
		, iCarColor(0)
	{
		memset(rgfltConfidence, 0, sizeof rgfltConfidence);
		memset(rgbContent, 0, sizeof rgbContent);
		memset(&rcPos, 0, sizeof rcPos);
	};
	PlateInfo(HiVideo::CRect& rc)
		: color(PC_UNKNOWN)
		, nPlateType(PLATE_UNKNOWN)
		, iLastCharHeight(-1)
		, iLastVscale(-1)
		, fltTotalConfidence(0)
		, rcPos(rc)
		, nVariance(0)
		, nAvgY(0)
		, fValid(true)
		, nInfoType(PI_LPR)			// 默认是车牌识别类型
		, nCarType(CT_UNKNOWN)		// 车辆类型未知
		, nCarColour(CC_UNKNOWN)
		, fReRecog(true)
		, iCarColor(0)
	{
		memset(rgfltConfidence, 0, sizeof rgfltConfidence);
		memset(rgbContent, 0, sizeof rgbContent);
		memset(&stCarInfo, 0, sizeof stCarInfo);
	};

	HRESULT GetPlateName(PWORD16 pwszName) const
	{
		return ::GetPlateNameUnicode(pwszName, nPlateType, color, rgbContent);
	};
	HRESULT GetPlateNameAlpha(char* szName) const
	{
		return ::GetPlateNameAlpha(szName, nPlateType, color, rgbContent);
	};
	int Similarity(const PlateInfo &plateinfo)
	{
		if (nInfoType == PI_TEM)
		{
			return 0;
		}
		int iScore = 0;
		int iCharNum(7);
		if (PC_GREEN == plateinfo.color)
		{
			iCharNum = 8;
		}
		for (int i = 0; i < iCharNum; i++)
		{
			if (rgbContent[i] == plateinfo.rgbContent[i])
			{
				iScore++;
			}
		}
		return iScore;
	};
	HiVideo::CRect rcPos;			// 车牌在整幅图像中的位置
	CarSizeInfo stCarInfo;			// 车辆外围信息
	PLATE_COLOR color;
	PLATE_TYPE nPlateType;
	int iLastDetWidth;
	int iLastCharHeight;
	int iLastVscale;
	BYTE8 rgbContent[8];			// TODO: Magic Number
	CParamQuanNum rgfltConfidence[8];		// TODO: Magic Number
	CParamQuanNum fltTotalConfidence;
	HiVideo::CRect rcPlatePos;				// 分割后的外围位置
	UINT nFrameNo;					// 当前帧号
	DWORD32 dwFrameTime;			// 当前帧时间
	int iHL;						// 车牌位置亮度
	bool fValid;					// 是否参与投票
	CAR_TYPE nCarType;				// 车辆类型（大、中、小）

	DWORD32 nVariance;		//车牌对比度
	DWORD32 nAvgY;				//车牌亮度

	HiVideo::CRect rgrcChar[8];

	PLATEINFO_TYPE nInfoType;		// 车牌信息类型
	CAR_COLOR nCarColour;
	bool fReRecog;
	int iCarColor;                  //1浅,2深,0默认不分深浅
};

class CTrackInfoHeader
{
public:
	static const int s_knMaxPlateInfo = 200;

	PlateRecogParam *m_pParam;

	TRACK_STATE m_State;		// one of the four states

	bool m_fObservationExists;	//当前帧是否存在车牌
	UINT m_dwPredictTime;
	HiVideo::CRect m_rcPredict;

	bool m_fCarArrived;	//是否已经调用CarArrive回调
	bool m_fCarLefted; // 是否已经调用过CarLeft;
	UINT m_nCarArriveTime; //调用CarArrive的时间
	UINT m_nCarArriveRealTime; // CarArrive实际时间
	DWORD32 m_dwCarArriveDopeTime;//CarArrive 预测时间

	DWORD32 m_dwLastProcessCarArrvieTime;
	// zhaopy
	DWORD32 m_dwDopeFirstPos;
	DWORD32 m_dwDopeEndPos;
	bool m_fIsTrigger;
	DWORD32 m_dwTriggerIndex;   //抓拍索引
	int    m_iCapCount;
	int    m_iCapSpeed;

	HiVideo::CRect m_rcCarArrivePos; //CarArrive时的车牌位置
	//最清晰和最后一张图的坐标
	HiVideo::CRect m_rcBestPos;
	HiVideo::CRect m_rcLastPos;
	bool m_fVoted;		//强制投票，是否已经投票

	int m_iVideoID;		//当前跟踪所属的视频编号
	int m_nID;
	int m_nMissingTime;			// the number of frames recently missing
	int m_nObservedFrames;		// the total number of observed frames
	UINT m_nStartFrameNo;
	UINT m_nEndFrameNo;
	int m_nResultEqualTimes;	//结果连续相同的次数

	int m_nVoteCondition;

	PlateInfo m_rgPlateInfo[s_knMaxPlateInfo];
	int m_cPlateInfo;
	DWORD32 m_dwFirstFlameTime;			// 第一帧的低32位时标
	bool m_fReverseRun;					// 车辆逆行标志
	bool m_fReverseRunVoted;
	int m_iVoteSimilarityNum;

	BYTE8 m_rgbVotedResult[8];
	PLATE_TYPE m_nVotedType;
	PLATE_COLOR m_nVotedColor;

	CParamQuanNum m_fltMaxConfidence;	// track中plate的最高confidence

	//根据预测位置计算得到的积分图
	CItgArea m_cItgArea;
	bool m_fCarIsStop; // 是否停止车标志
	int m_iCarStopTick;
	int m_iCarStopPassTime;

	CAR_TYPE m_nVotedCarType;	// 车型(大中小)
	CAR_COLOR m_nVotedCarColor;

	CObjMatch *m_pObj;			// 模版匹配
	bool m_fCanOutput;			// 是否输出车牌(已经投票但可信度不够的，不输出车牌)
	bool m_fhasTemple;			// 当前是否有模板
	bool m_fTempTrackExists;	// 模版匹配存在
	bool m_fTempCloseEdge;		// 模版是否靠近边缘
	int m_nCloseEdgeCount;		// 模版靠近边缘帧计数器
	int m_nMinYPos;				// 红绿灯状态下，模版跟踪到达的最小Y坐标
	int m_nMaxYPos;				// 红绿灯状态下，模版跟踪的最大Y坐标偏移
	int m_nMaxXPos;				// 红绿灯状态下，模版跟踪的最大X坐标偏移

	//红绿灯相关
	int m_iOnePosLightScene;
	int m_iTwoPosLightScene;
	int m_iThreePosLightScene;
	int m_iPassStopLightScene;
	int m_iPassLeftStopLightScene;
	bool m_fPassStopLine;
	//是否要抓拍
	bool m_fFirstTrigger;
	bool m_fSecondTrigger;
	bool m_fThirdTrigger;

	//抓拍的时间
	DWORD32 m_dwFirstTime;
	DWORD32 m_dwSecondTime;
	DWORD32 m_dwThirdTime;

	DWORD32 m_dwFirstTimeMs;
	DWORD32 m_dwSecondTimeMs;
	DWORD32 m_dwThirdTimeMs;

	float m_rgfltFrameSpeed[1000];
	int m_iFrameSpeedCount;

	int m_iFirstType;

	DWORD32 m_dwTriggerTime;	//到线圈位置的时间

	int m_iRoad;
	//每张图片的坐标
	HiVideo::CRect m_rgRect[5];
	//最后一张图片的坐标
	HiVideo::CRect m_rectLastImage;
	CarSizeInfo m_stCarInfo;		// 车辆外围信息(来自视频检测)	

	bool m_fPredictCarArrive;		//当前帧是否做过CarArrive预测
	// zhaopy
	int m_iCarStopFrame;
	DWORD32 m_dwBestSnapShotRefTime;
	DWORD32 m_dwLastSnapShotRefTime;
	DWORD32 m_dwBeginCaptureRefTime;
	DWORD32 m_dwBestCaptureRefTime;
	DWORD32 m_dwLastCaptureRefTime;

	//zhaopy
	// 第二张抓拍
	bool m_fHasTrigger;
	DWORD32 m_dwTriggerTimeMs;
	DWORD32 m_dwTriggerImageTime;
};

class CTrackInfo : public CTrackInfoHeader
{
public:
	CTrackInfo();
	~CTrackInfo();

	HRESULT Detach(CTrackInfo &tiDest, PROCESS_ONE_FRAME_RESPOND* pProcessRespond);

	IReferenceComponentImage *m_pimgBestSnapShot;
	IReferenceComponentImage *m_pimgLastSnapShot;
	IReferenceComponentImage *m_pimgBeginCapture;
	IReferenceComponentImage *m_pimgBestCapture;
	IReferenceComponentImage *m_pimgLastCapture;

	//临时图片及坐标
	IReferenceComponentImage *m_pimgBestCaptureTemp;
	HiVideo::CRect m_rectBestCaptureTemp;
	DWORD32 m_dwBestCaptureRefTimeTemp;

	// 为每种类型都保存一个得分最高的车牌图像
	BestPlateInfo m_rgBestPlateInfo[PLATE_TYPE_COUNT];
	//保存处理后的车牌小图,用于生成二值化图 weikt 2011-5-3
	BestPlateInfo m_bestPlatetoBinImage;

	IReferenceComponentImage *GetLastPlateImage()
	{
		if (m_cPlateInfo <= 0) return NULL;
		return m_rgBestPlateInfo[LastInfo().nPlateType].pimgPlate;
	}

	void Clear(PROCESS_ONE_FRAME_RESPOND* pProcessRespond);

	inline const PlateInfo& LastInfo() const
	{
		return m_rgPlateInfo[m_cPlateInfo-1];
	}

	inline PlateInfo& LastInfo()
	{
		return m_rgPlateInfo[m_cPlateInfo-1];
	}
	HiVideo::CRect CurrentPosition();
	HiVideo::CRect PredictPosition(UINT nNowTime , BOOL fEnableDetReverseRun, IScaleSpeed *pScaleSpeed);
	CParamQuanNum TrackScore() const;

	// 判断该Track是否投票, 输出结果
	bool PrepareOutPutFinalResult(DWORD32 dwCurTick);
	HRESULT NewTrack(
		PROCESS_ONE_FRAME_PARAM* pParam,
		HV_COMPONENT_IMAGE imgPlate,
		PlateInfo plate,
		int iLastStatus,
		int iCurStatus,
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond
	);
	HRESULT UpdatePlateImage(
		HV_COMPONENT_IMAGE imgInputPlate,
		PROCESS_ONE_FRAME_PARAM* pParam,
		int iLastStatus,
		int iCurStatus,
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond
	);
	HRESULT UpdateBestImage(
		IReferenceComponentImage *
		);
	HRESULT Vote(PLATE_TYPE &nVotedType, PLATE_COLOR &nVotedColor, PBYTE8 pbVotedContent);
	HRESULT RemoveInvalidVotes();

	//增加对跟踪进行预投票的机制,返回true表明可以出牌，false表明还没达到出牌的条件
	bool  PreVote(void);

	HRESULT UpdateObjTemple(HV_COMPONENT_IMAGE& img, HiVideo::CRect &rcObj);	// 更新物体倌０?

	float AverageConfidence();
	float AverageFirstConfidence();
	CParamQuanNum AveragePosConfidence(int iPos);

	int Similarity(const PlateInfo &plateinfo);
	bool IsTwins(CTrackInfo &trackInfo);
	float AverageStdVar();
	float AveragePlateBrightness();

	//更新预测区域,在解压时使用
	CItgArea* GetPredictArea(
		DWORD32 dwRefTime,
		const HiVideo::CRect& rcFullSearch,
		const HiVideo::CRect& rcTrackArea,
		BOOL fEnableDetReverseRun,
		IScaleSpeed *pScaleSpeed
		);

public:
	//可配置参数,由上一级Tracker进行设置
	// 投票数
	static int m_nVoteFrameNum;
	// 连续结果相同, Vote
	static int m_nMaxEqualFrameNumForVote;
	// 是否输出投票满足条件
	static int m_fOutputVoteCondition;

	//投票时，去除得分最低的车牌
	static int m_nRemoveLowConfForVote;

	// 识别得分低于阈值(百分比)，用*表示
	static int m_nRecogAsteriskThreshold;

	static int m_iBestLightMode; // 是否使用最亮图模式，该值为1时用跟踪里的最亮图代替最清晰图
	static int m_iBestLightWaitTime; // 最亮图的等待时间

	//红绿灯相关
	static int m_iCheckType;
	static int m_iAutoScanLight;		//动态检查红绿灯位置开关
	static int m_iSceneCheckMode	;	//场景的检测模式
	static int m_iCaptureOnePos;
	static int m_iCaptureTwoPos;
	static int m_iCaptureThreePos;
	static int m_iStopLinePos;
	static int m_iLeftStopLinePos;			//左转道停车线位置与直行停车线的差值.
	static int m_iHeight;
	static int m_iWidth;
	static int m_iRedLightDelay;			//红灯判断违章延迟时间
	// zhaopy
	static int m_iRealForward;
	static int m_iFlashlightMode;
	static int m_iFlashlightThreshold;


	static int m_iSpecificLeft;			//加强左转判断
	static int m_iRunTypeEnable;		//强制输出行驶类型，即如行驶类型未能判断，则按车辆所在的车道类型输出。

	//CarArrive的位置(即线圈的位置)
	static int m_iCarArrivedPos;

	static int m_iMatchScanSpeed;		// 模版匹配扫描速度

	//车道信息
	static int m_iRoadNumber;								//车道线数量
	static int m_iRoadNumberBegin;							//车道编号起始方向
	static int m_iStartRoadNum;								//起始车道号
	static const int s_iMaxRoadNumber = 5;				//车道线最多可以为5
	static int m_iOutputRoadTypeName;					//是否输出车道名称
	static ROAD_INFO m_roadInfo[s_iMaxRoadNumber];

	//事件检测参数
	static ACTIONDETECT_PARAM_TRACKER m_ActionDetectParam;

	// 红绿灯情况下模板匹配位置
	static int m_nTrafficLightTempleYPos;

	// 厦门项目行人道编号
	static int m_iManRoadNum;

	//颜色识别开关
	static bool m_fEnableRecgCarColor;

	static int  m_nOutputPeccancyType;

	//黄国超增加优先级排序顺序,2010-12-07
	static char szPeccancyPriority[255];

	//韦开拓 2011-3-9增加红绿灯识别参数	
	static int m_iTrafficLightBrightValue;
	static int m_iTrafficLightThread;
	static int m_iAutoScanThread;

	bool   m_bPreVote;
	//added by liujie 
	//电警无牌车车尾模型扫描开关
	static int m_fEPUseCarTailModel;
	static int m_iEPNoPlateScoreThreld;

};

//计算车道
extern int MatchRoad(int iX, int iY, int *piRoadWidth = NULL);
extern int LeftOrRight(int iX, int iY, int iLineNum, int* piDistance = NULL);
//取车身颜色结果
extern int GetCarColorType( DWORD32 dwCarColorID );

//====================================
/*
extern TRAFFICLIGHT_SCENE g_rgLightScene[MAX_SCENE_COUNT];
extern void ClearLightScene();
extern void SetLightScene(int iPos, TRAFFICLIGHT_SCENE ts);
extern bool GetLightScene(int iPos, TRAFFICLIGHT_SCENE* pts);

extern SCENE_INFO g_rgSceneInfo[MAX_SCENE_COUNT];
extern void ClearSceneInfo();
extern void SetSceneInfo(int iPos, const SCENE_INFO& ts);
extern bool GetSceneInfo(int iPos, SCENE_INFO* pts);

#define MAX_LIGHT_TYPE_COUNT 32
extern TRAFFICLIGHT_TYPE g_rgLightType[MAX_LIGHT_TYPE_COUNT];
extern int g_iLightTypeCount;
extern void ClearTrafficLightType();
extern void SetTrafficLightType(int iTeam, int iPos, _TRAFFICLIGHT_POSITION tpPos, _TRAFFICLIGHT_LIGHT_STATUS tlsStatush);
extern bool GetTrafficLightType(int iTeam, int iPos, TRAFFICLIGHT_TYPE* ltLightType);

#define STATUS_BUF_LEN 100
extern int g_rgSceneStatus[STATUS_BUF_LEN];
extern void SetSceneStatus(int nPos, int nStatus);
extern int GetSceneStatus(int nPos);

#define TS_UNSURE 0
#define TS_GREEN 3
#define TS_YELLOW 5
#define TS_RED 9

#define MAX_TRAFFICLIGHT_COUNT 10	//最大灯组数

//所有灯组当前状态
typedef struct tag_LightTeamStatus
{
	DWORD32 nTeamCount;
	DWORD32 pdwStatus[MAX_TRAFFICLIGHT_COUNT];
	DWORD32 pnLightCount[MAX_TRAFFICLIGHT_COUNT];
	tag_LightTeamStatus()
	{
		memset(pdwStatus, 0, sizeof(DWORD32) * MAX_TRAFFICLIGHT_COUNT);
		memset(pnLightCount, 0, sizeof(DWORD32) * MAX_TRAFFICLIGHT_COUNT);
		nTeamCount = 0;
	}
}
LIGHT_TEAM_STATUS;

//传入各灯组的状态，返回场景状态编号
extern int TransitionScene(const LIGHT_TEAM_STATUS& ltsInfo, const int& iSceneCount, const int& iLastOkLightStatus, bool fFlag);

class CTrafficLightInfo
{
public:
	enum {MAX_POS_COUNT = 8};	//灯组位置划分

public:
	HRESULT UpdateStatus(HV_COMPONENT_IMAGE* pSceneImage);
	HRESULT UpdateStatus2(HV_COMPONENT_IMAGE* pSceneImage);
	
	HRESULT SetRect(const HV_RECT& cRect, int nLightCount, int nType);
	HRESULT SetRect(int nLeft, int nTop, int nRight, int nBottom, int nLightCount, int nType);

public:
	DWORD32 GetLastStatus()
	{
		return m_nLastStatus;
	}

	int GetPosCount()
	{
		return m_nLightCount;
	}

	HV_RECT GetRect()
	{
		return m_rcLight;
	}

	HV_RECT GetLastRect()
	{
		HiVideo::CRect rcTemp = m_rcLastLight;
		rcTemp += m_rcLightInflated.TopLeft();
		return rcTemp;
	}

	HV_RECT GetInflatedRect()
	{
		return m_rcLightInflated;
	}

	HRESULT Reset()
	{
		m_fInit = false;
		HV_RECT rcInit = {0,0,0,0};
		m_rcLight = rcInit;
		m_rcLastLight = rcInit;
		m_nID = 0;
		m_nType = 0;
		m_nLightCount = 0;
		m_nLastStatus = TS_UNSURE;

		return S_OK;
	}

	HRESULT ReScan()
	{
		//如果还没初始化
		if( !m_fInit ) return S_FALSE;

		HV_RECT rcInit = {0,0,0,0};
		m_rcLastLight = rcInit;
		m_nLastStatus = TS_UNSURE;
		m_fInit = false;
		return S_OK;
	}

public:
	CTrafficLightInfo()
	{
		m_fInit = false;
		HV_RECT rcInit = {0,0,0,0};
		m_rcLight = rcInit;
		m_rcLastLight = rcInit;
		m_nID = 0;
		m_nType = 0;
		m_nLightCount = 0;
		m_nLastStatus = TS_UNSURE;

		m_pElement = hvCreateStructuringElementEx(
			5,
			5,
			2,
			2,
			HV_SHAPE_RECT);
	}
	~CTrafficLightInfo()
	{
		hvReleaseStructuringElement(&m_pElement);
	}

protected:
	DWORD32 GetLightStatus(HV_COMPONENT_IMAGE imgLight, int nPosCount, int nType);
	DWORD32 GetLightStatus2(HV_COMPONENT_IMAGE imgLight, HiVideo::CRect rcLight);

protected:
	INT m_nID;
	HiVideo::CRect m_rcLight;	//初始位置框
	HiVideo::CRect m_rcLastLight; //当前位置
	HiVideo::CRect m_rcLightInflated;
	INT m_nType;		//0 = 横向, 1 = 竖向
	INT m_nLightCount;	//包含灯数量
	DWORD32 m_nLastStatus;

	HvConvKernel* m_pElement;

	bool m_fInit;

public:
	//连续多少帧是中间状况，则重置红绿灯框位置.
	static const int MAX_ERROR_COUNT = 500;
};
*/
#endif // _TRACKINFO_INCLUDED__
