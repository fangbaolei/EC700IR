#ifndef _TRACKERIMPL_INCLUDED__
#define _TRACKERIMPL_INCLUDED__

#include "Tracker.h"
#include "PlateDetector.h"
#include "ProcessPlate.h"
#include "Recognition.h"
#include "TrackInfoHigh.h"
#include "ScaleSpeed.h"			// 测速接口
#include "..\ObjectTracking\ObjectTracking.h"			// 移动物体跟踪接口
#include "..\TrafficLight\trafficlightimpl.h"
#include "ItgArea.h"
#include "ObjectTrackingInfo.h"

#include "CarDet.h"

#define DRAW_RECT_NUM 10
#define DRAW_RECT_LEN 50

using namespace HiVideo;

typedef struct car_left
{
	DWORD32 car_left_time;
	PLATE_COLOR car_plate;
	DWORD32 frame_no;
}CAR_LEFT_STRUCT;

//TODO:	覃宇：除了接口，尽量减少使用继承的方式定义来类，多用复合。
class CFinalResultParam : public CTrackInfo
{
	PlateRecogParam *m_pParam;
	DWORD32 m_dwLastResultTime;		
public:
	void Create(CTrackInfo& TrackInfo, PlateRecogParam *pParam);
	HRESULT UpdateResult(const CTrackInfo& TrackInfo);
	bool IsTwinResult(const CTrackInfo& TrackInfo);
	//更新队列 quanjh 2011-03-06
	void UpdateDetectSaver(DWORD32 dw_TimeRef);
	//队列排序 quanjh 2011-03-06
	void SetOrder();

public:
	static int m_iBlockTwinsTimeout;
	static int m_iSendTwinResult;
	//m_iBlockTwinsTimeout时间内存放的车牌信息 quanjh 2011-02-23
	struct DETECTSAVER
	{
		BYTE8 rgContent[8];
		DWORD32 dw_TrackEndTime;
		CRect rcPos;
	}m_detectsaver[7200];
	//m_iBlockTwinsTimeout时间内队列长度计数 quanjh 2011-02-23
	int m_iTimeinCount;
};

class CTrackerImpl : public ITracker
{
public:
	CTrackerImpl();

// ITracker
	virtual ~CTrackerImpl();
	void Clear();

	STDMETHOD(ProcessOneFrame)(
		PROCESS_ONE_FRAME_PARAM* pParam,
		PROCESS_ONE_FRAME_DATA* pProcessData,
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond
	);
	STDMETHOD(ProcessPhoto)(
		HV_COMPONENT_IMAGE imgCurFrame,
		PVOID pvParam,
		PVOID pvData,
		PROCESS_PHOTO_RESPOND* pProcessRespond
		)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(SetInspector)(
		IInspector *pInspector
		)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(SetHvParam)(
		HvCore::IHvParam2* pHvParam
	)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(SetHvModel)(
		HvCore::IHvModel* pModel
	);

	STDMETHOD(SetHvPerformance)(
		HvCore::IHvPerformance* pHvPerf
		);

	STDMETHOD(SetCharSet)(
		ChnCharSet nCharSet
	);

	STDMETHOD(SetCalibrateParam)(
		BOOL fEnable,
		int nRotate=0,
		int nTilt=0,
		BOOL fOutputImg = FALSE
	);

	STDMETHOD(SetPreSegmentParam)(
		BOOL fEnable
	);
	STDMETHOD(SetLPRMode)(PlateRecogMode nMode);

	STDMETHOD(SetVideoDet)(BOOL fEnabled, BOOL fSkipFrame = TRUE, int nDetLeft = 0, int nDetRight = 100);

	STDMETHOD(SetDetArea)( const DETECTAREA& cDetAreaInfo );

	STDMETHOD(ForceResult)(int iFlag = 0);

	STDMETHOD(GetTrackCount)(PDWORD32 pdwCount);
	STDMETHOD(ClearTrackInfo)(PROCESS_ONE_FRAME_RESPOND* pProcessRespond);

	STDMETHOD(GetPlatePos)(DWORD32 dwIndex, CRect *prcPlate, CRect* prcRect);

	STDMETHOD(GetDetectCount)(PDWORD32 pdwCount);
	STDMETHOD(GetDetectPos)(DWORD32 dwIndex, CRect *prcDetected);

	STDMETHOD(GetObjCount)(PDWORD32 pdwCount);
	STDMETHOD(GetObjPos)(DWORD32 dwIndex, CRect *prcObj);
	STDMETHOD(GetEPObjCount)(PDWORD32 pdwCount);
	STDMETHOD(GetEPObjPos)(DWORD32 dwIndex, CRect *prcObj);

	STDMETHOD(IsVoted)(DWORD32 dwIndex, bool *pfVoted);

	//为HVCORE实现的初始化函数
	STDMETHOD(InitTracker)(TRACKER_CFG_PARAM* pCfgParam);

	//为HVCORE实现的动态修改参数
	STDMETHOD(ModifyTracker)(TRACKER_CFG_PARAM* pCfgParam);

	// 车辆检测模型的更新接口
	STDMETHOD(UpdateModel)(unsigned int uModelId, unsigned char* pbData, unsigned int cbDataSize);

	// 动态更新算法参数接口
	STDMETHOD(UpdateAlgParam)(unsigned char* pbData, unsigned int cbDataSize);

	//测试检测模块的性能，测试的模块类型和测试参数通过全局参数列表设置,
	//全局参数列表通过INI文件加载.
	STDMETHOD(TestPerformance)(
		HV_COMPONENT_IMAGE *phciTest,	//测试数据
		char *pszReport,				//测试报告
		DWORD32 dwReportBufSize,		//测试报告缓冲区长度
		DWORD32 dwMaxTestCount,			//最大测试次数
		DWORD32 dwMaxTestDuration		//最大测试时间(毫秒)
	);

	STDMETHOD(ModuleTest)( 	
		MODULE_ID nModuleID,
		DWORD32 dwFlag,
		HV_COMPONENT_IMAGE *pImg,
		STD_RESULT* pStdResult,
		PVOID pvArgDat = NULL
		)
	{
		return E_NOTIMPL;
	}

	// 得到环境亮度
	STDMETHOD(GetLightType)(LIGHT_TYPE* pLightType);

	STDMETHOD(SetFirstLightType)(LIGHT_TYPE nLightType);

	STDMETHOD(SetFirstPulseLevel)(int nPulseLevel);

	STDMETHOD(SetFirstCplStatus)(int nCplStatus);
	//取得当前积分区域
	STDMETHOD(GetItgArea)(CItgArea** ppItgArea, DWORD32 dwRefTime);

	// zhaopy
	STDMETHOD(CheckEvent)();

	//检测当前所有的跟踪是否需要Cararrive
	HRESULT CheckCarArrive(
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond,
		int iDiffTime = 0
		);
	HRESULT ProcessCarArrive(
		CTrackInfo* pTrack, 
		DWORD32 dwImageTime, 
		float fltDistance, 
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond
		);

	HRESULT CheckCarArriveTrigger();
	// zhaopy
	HRESULT ProcessNoPlateCarArrive(
		CObjTrackInfo* pObjTrack, 
		DWORD32 dwImageTime, 
		float fltDistance, 
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond
		);

	void CalcTrapDetArea(
		TRAP_DETAREA* pTrapDetArea,
		int nCarArriveLineY,  // 百分比
		int nFrameWidth,
		int nFrameHeight
		);

	HRESULT DetectionParkingBox(
		PROCESS_ONE_FRAME_PARAM* pParam,
		PROCESS_ONE_FRAME_DATA* pProcessData,
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond
	);

private:
	BOOL m_fEnableCarArriveTrigger;	// CarArrive是否触发相机抓拍
	int  m_iTriggerPlateType; //触发抓拍的车牌类型,0-所有车牌;1-黄牌
	// zhaopy
	static const int MIN_TRIGGER_TIME = 100;	// 两次触发最小间隔时间
	DWORD32 m_dwLastTriggerTick;	// 最后一次触发的时间
	DWORD32 m_dwTriggerCameraTimes;		// 当前触发次数

public:
	//创建功能组件
	HRESULT OnCreate();

	HRESULT RecognizePlate(
		HV_COMPONENT_IMAGE imgPlate,
		PlateInfo &Info,
		CPersistentComponentImage& imgRectified
		);

	//识别车牌颜色，供外部调用者使用
	HRESULT RecognizeCarColour(
		HV_COMPONENT_IMAGE imgPlateOri,
		PlateInfo &Info,
		int iAvgGray                                  //环境亮度
		);

	LIGHT_TYPE m_LightType; //环境光类型

	HRESULT RecogSnapImg(
		PROCESS_ONE_FRAME_PARAM* pParam,
		PROCESS_ONE_FRAME_DATA* pProcessData,
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond,
		HV_COMPONENT_IMAGE& imgFrame,
		CRect& rcRecogArae,
		int nDetMinScale,
		int nDetMaxScale
		);


protected:
	static const int s_knMaxTrackInfo = 30;
	static const int s_knMaxPlateCandidate = 200;
	static const int s_knMaxObjCandidate = 30;
	static const int s_knMaxVideoChannel = 4;

	BOOL m_fEnablePreciseRectify;

	CFinalResultParam m_LastResult;

	RECTA m_rgTempPlatePos[s_knMaxPlateCandidate];

	BOOL m_fInitialized;
	INT m_nWidth, m_nHeight, m_nStride;

	CTrackInfo m_rgTrackInfo[s_knMaxTrackInfo];
	int m_cTrackInfo;
	int m_rgiFiveCharWidth[40];
	int m_nDefaultDBType;

	PlateRecogParam *m_pParam;
	CFinalResultParam* m_pLastResult;

	//车牌信息结构数组是否已经初始化
	bool m_fLastResultInited;

	IInspector *m_pInspector;
	IPlateDetector *m_pPlateDetector;
	IProcessPlate *m_pProcessPlate;
	IRecognition *m_pRecognition;
	IScaleSpeed *m_pScaleSpeed;							// 测速接口

	IObjectDetector *m_rgpObjectDetector[s_knMaxVideoChannel];	//移动物体检测接口
	CRect m_rgrcDetected[s_knMaxObjCandidate];
	int m_cDetected;
	//车牌扫描区域,用原有的检测区域
    CRect m_rgrcScanEP[s_knMaxObjCandidate];
    int m_cScanEP;
    //车尾模型扫描
    CRect m_rgrcCarTail[s_knMaxObjCandidate];
    int m_cCarTail;
	CObjTrackInfo m_rgObjTrackInfo[s_knMaxTrackInfo];	//物体跟踪
	int m_cObjTrackInfo;

	// zhaopy 0624
	CRect m_rcLightDetected[MAX_LIGHT_NIGHT_COUNT];
	int m_iLightDetected;

	//增加电警的无牌车跟踪
    CObjTrackInfo m_rgEPObjTrackInfo[s_knMaxTrackInfo];	//电警下增加的物体跟踪
    int m_cEPObjTrackInfo;
    OBJDETECT_INFO m_objDetectInfo;
	HvCore::IHvModel* m_pHvModel;
	HvCore::IHvPerformance* m_pHvPerf;

	int m_iAvgY;				//环境光亮度
	LIGHT_TYPE m_PlateLightType;	//车牌亮度级别类型

	CItgArea m_cItgArea; //全检积分图区域

	// 是否使用补光灯
	int m_iUsedLight;
	//交通灯相关
	CTrafficLightImpl m_cTrafficLight;

	//交通灯相关
	CTrafficLightInfo m_rgLightInfo[MAX_TRAFFICLIGHT_COUNT];

	//用于回调输出
	HV_RECT m_rgLightRect[MAX_TRAFFICLIGHT_COUNT];	//灯组位置
	DWORD32 m_rgLightStatus[MAX_TRAFFICLIGHT_COUNT]; //灯组状态
	CItgArea m_rgLightArea[MAX_TRAFFICLIGHT_COUNT];  //需要JPEG解压的灯组位置

	int m_nRedLightCount;
	HV_RECT m_rgRedLightRect[MAX_TRAFFICLIGHT_COUNT * 2];	//红灯灯组位置

	//外接IO红绿灯信息
	//static const int MAX_IO_LIGHT_COUNT = 8;
	TRAFFICLIGHT_TYPE m_rgIOLight[MAX_IO_LIGHT_COUNT];

	//初始化时进行清理
	HRESULT OnInit();

	HRESULT SetFrameProperty(
		const FRAME_PROPERTY& FrameProperty,
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond
	);

	HRESULT GetInflateRect(
		CRect &rect,		// in, out
		PLATE_COLOR color, 
		PLATE_TYPE &type,
		INT maxRectHeight	// 判断是否用inflate rate2
	);

	HRESULT RecognizeCarColour(
		HV_COMPONENT_IMAGE imgPlateOri,
		PlateInfo &Info,
		int iAvgGray ,                                 //环境亮度
		BOOL fIsNight
	);

	// 2011.3 新增车身颜色识别
	HRESULT RecognizeColorArea(
		const HV_COMPONENT_IMAGE imgInput, 
		HV_RECT rcCrop, 
		BYTE8 *cTempAdd, 
		BYTE8 *piResult,
		int iCutLeng,
		LIGHT_TYPE m_PlateLightType
	);
	HRESULT CalcPlateColor(HV_COMPONENT_IMAGE imgFrame,PBYTE8 pBW,int &nColor);
	HRESULT GetPlateColor(PBYTE8 RBG1,PBYTE8 RBG2, int &nColor);
	HRESULT GetColorResult(PBYTE8 RBG1,PBYTE8 RBG2,bool *fDiffThr, int &nColor);

	HRESULT ProcessTrackState(
		CTrackInfo *pTrack,									//当前处理的跟踪
		CRect &rcTrackArea,									//跟踪区域
		PlateInfo &plateObserved,							//当前帧检测到的车牌信息
		HV_COMPONENT_IMAGE &imgCalibratedFrame,				//经过矫正的视频帧
		CPersistentComponentImage &imgBestRectified,		//经过矫正的车牌小图
		HV_COMPONENT_IMAGE &imgOriSnapFrame,				//原始的视频帧
		HV_COMPONENT_IMAGE &imgOriCapFrame, 
		PROCESS_ONE_FRAME_PARAM* pParam,
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond	
	);

	HRESULT ProcessTemTrackState(							// 处理模版匹配状某盗?
		CTrackInfo *pTrack,									//当前处理的跟踪
		CRect &rcTrackArea
		);

	HRESULT TrackingObj(PROCESS_ONE_FRAME_PARAM* pParam, HV_COMPONENT_IMAGE &imgCurImage, PROCESS_ONE_FRAME_RESPOND* pProcessRespond);
	HRESULT ShowObjTracks(PROCESS_ONE_FRAME_PARAM* pParam, int iVideoID, PROCESS_ONE_FRAME_RESPOND* pProcessRespond);
	HRESULT ShowTracks(	
		PROCESS_ONE_FRAME_PARAM* pParam, 
		PROCESS_ONE_FRAME_DATA* pProcessData, 
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond
		);
	HRESULT RemoveInvalidTracks(PROCESS_ONE_FRAME_RESPOND* pProcessRespond);
	HRESULT FireCarLeftEvent(
		CTrackInfo& TrackInfo, 
		PROCESS_ONE_FRAME_PARAM* pParam,
		PROCESS_ONE_FRAME_DATA* pProcessData, 
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond
		);
	HRESULT FireNoPlateCarLeftEvent(PROCESS_ONE_FRAME_PARAM* pParam, CObjTrackInfo& ObjTrackInfo, PROCESS_ONE_FRAME_RESPOND* pProcessRespond);
	HRESULT TrackingObjByTem(PROCESS_ONE_FRAME_PARAM* pParam, HV_COMPONENT_IMAGE &imgCurImage, PROCESS_ONE_FRAME_RESPOND* pProcessRespond);
    CRect GetEPNoPlatePredictRect(UINT nNowTime, CObjTrackInfo* pObjTrack);
	
    HRESULT CheckEPNoPlateConfidence(HV_COMPONENT_IMAGE* pImgCalibratedFrame, int iVideID);
    HRESULT CheckEPNoPlateConfidence2(HV_COMPONENT_IMAGE* pImgCalibratedFrame, int iVideID);

	HRESULT TestDetPerformance(
		HV_COMPONENT_IMAGE *phciTest,	//测试图像
		char *pszReport,				//测试报告
		DWORD32 dwReportBufSize,		//测试报告缓冲区长度
		DWORD32 dwMaxTestCount,			//最大测试次数
		DWORD32 dwMaxTestDuration		//最大测试时间(毫秒)
	);
	HRESULT TestSegPerformance(
		HV_COMPONENT_IMAGE *phciTest,	//测试图像
		char *pszReport,				//测试报告
		DWORD32 dwReportBufSize,		//测试报告缓冲区长度
		DWORD32 dwMaxTestCount,			//最大测试次数
		DWORD32 dwMaxTestDuration		//最大测试时间(毫秒)
	);
	STDMETHOD(GetTrafficLightStatus)(
		unsigned char* pbStatus
		)
	{
		*pbStatus = m_nIOLevel;
		return S_OK;
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
		m_nRedLightCount = nRedLightCount;
		for(int i = 0; i < nRedLightCount; i++)
		{
			m_rgRedLightRect[i] = rgRedLightPos[i];
		}
		return S_OK;
	}

	//判断白天黑夜;
	HRESULT CheckLight();
	//判断顺光逆光
	HRESULT CheckLightType(int iCarY, bool fIsAvgBrightness);
	// 车型判断参数设置
	HRESULT SetCarTypeParam(int nWidth, int nHeight);

	HRESULT CheckTrafficLight(HV_COMPONENT_IMAGE* pSceneImage );

	//计算车速
	HRESULT CalcCarSpeed(float &fltCarSpeed, float &fltScaleOfDistance, CTrackInfo &TrackInfo);
	HRESULT CalcObjSpeed(float &fltCarSpeed, float &fltScaleOfDistance, CObjTrackInfo &ObjTrackInfo);

	// 是否可以创建模板(匹配)
	bool IsCanFoundTemple(CTrackInfo *pTrack);
	// 车辆是否停止
	bool IsCarStop(CTrackInfo *pTrack);
	// 更新模板跟踪最大位移
	void UpdateOffset(CTrackInfo *pTrack);

	void RoadInfoParamInit();
	void InitRoadInfoParam();
	//通过IO取当前红绿灯状态
	DWORD32 GetIOStatus(int iTeam, BYTE8 bLevel);
	//判断是否压线
	bool IsOverLine(int iX, int iY);
	//盗拘惺还旒?
	RUN_TYPE CheckRunType(HiVideo::CRect rect0, HiVideo::CRect rect1);
	RUN_TYPE CheckRunTypeEx(HiVideo::CRect rect0, HiVideo::CRect rect1);

	//卸鲜欠裼锌赡芪フ?
	bool CheckPeccancy(CTrackInfo *pTrack);
	bool CheckObjectPeccancy(CObjTrackInfo* pObjTrackInfo);

	// zhaopy
	bool CheckPeccancyTrigger(CTrackInfo* pTrack);
	bool CheckObjectPeccancyTrigger(CObjTrackInfo* pObjTrackInfo, int iObjIndex);
	HRESULT MatchTrackInfoAndObjectInfo(PROCESS_ONE_FRAME_PARAM *pParam, PROCESS_ONE_FRAME_RESPOND* pProcessRespond);
	//计算每一帧与第一帧的帧差比时间差的一个帧速
	HRESULT CalcFrameSpeed(CTrackInfo *pTrack);
	//事件检测
	//压黄线
	CROSS_OVER_LINE_TYPE IsOverYellowLine(CTrackInfo& TrackInfo);
	//越线
	CROSS_OVER_LINE_TYPE IsCrossLine(CTrackInfo& TrackInfo);

	//过滤大车在停止线前停下的多检 weikt 2011-4-19
	HRESULT CheckNoPlate(CObjTrackInfo* pObj);
	HRESULT RemoveMoreReview(CObjTrackInfo* pObj);

	// zhaopy 0624
	//added by liujie 雨夜多闪条件限制
	BOOL RainyFlashControl(HiVideo::CRect* pPos, int iVideoID);
	BOOL RainyObjOutPutControl(HiVideo::CRect* pPos, int iTrackID);

	/////////////////////////////////////////////////////////////////
	// 违章判定

	bool CheckRoadAndRun(RUN_TYPE runtype, int runroadtype);
	bool CheckRush(int iPreLight, int iAcrossLight, int runroadtype, int runtype, DWORD32 dwCaptureTime);

	HRESULT CheckPeccancyType(
		int iPreLight,			//压线前的红绿灯场景
		int iAcrossLight,		//过线后的红绿灯场景
		RUN_TYPE runtype,	//行驶类型
		int runroadtype,      //行驶车道类型
		DWORD32 dwCaptureTime,	//抓拍时间
		/*PECCANCY_TYPE*/int * pPeccancyType //违章类型,黄国超修改为int类型
		);

	HRESULT DownSampleYUV422(HV_COMPONENT_IMAGE imgSrc, HV_COMPONENT_IMAGE imgDst);

	// 计算螺钉位置
	HRESULT GetPlateNail(
		PLATE_COLOR color,			
		HV_COMPONENT_IMAGE imgPlate, 
		CRect* pCharPos
		);

	// zhaopy 0624
	bool IsCanOutputReverseRun(CObjTrackInfo* pObj);

// zhaopy 一体机抓拍
private:
	void TriggerCamera(const int iRoadNum);
	//记录下抓拍信息
	void SetTriggerInfo(const DWORD32 dwLastImageTime
					   ,const DWORD32 dwCarArriveTime
	                   ,const DWORD32 dwRoadId
	                   ,const DWORD32 dwTriggerIndex
	                   );

	//抓拍跟踪(有车牌)
	void CapTrack(CTrackInfo* pTrack);
	
	//抓拍物体(无车牌)
	void CapObjTrack(CObjTrackInfo* pObjTrack);

	//判断车牌坐标是否包含在四边形区域内 weikt 2011-7-20
	bool IsContainOfTrap(TRAP_DETAREA trapDetArea,CRect rectPlate);

    //< TrackerImplHigh_PhotoRecog.cpp
    HRESULT CalcPlateColor(HV_COMPONENT_IMAGE imgFrame,PBYTE8 pBW, PLATE_TYPE nPlateType,PLATE_COLOR &nColor,int &nAvgY1, int &nAvgY2);

    HRESULT PreTreatment(
        HV_COMPONENT_IMAGE& imgPlate,
        PLATE_TYPE nPlateType,
        PLATE_COLOR nPlateColor,
        BOOL fEnhance,
        CPersistentComponentImage* rgImgRectified,
        int nCount,	 //指定预处理生成的图数量,通过改变垂直校正角度
        CBinaryInfo &BinInfo
        );

    HRESULT CropPlateImage(
        const HV_COMPONENT_IMAGE& imgPhoto,
        CRect& rcOriPos,
        PLATE_TYPE nPlateType,
        int nDetConf,
        HV_COMPONENT_IMAGE* rgPlateImage,
        int nCount	//指定截取图的结果数
        );

    HRESULT CTrackerImpl::Vote(
        PlateInfo** rgInfo,
        int nCount,
        PlateInfo* pResult,
        CParamQuanNum* pfltConf
        );

    HRESULT RecogSnapImg(
        HV_COMPONENT_IMAGE& imgFrame,
        CRect& rcRecogArae,
        int nDetMinScale,
        int nDetMaxScale
        );
    //>

private:
	//可配置参数
	DETECTAREA m_cDetAreaInfo;

	DETECTAREA m_cRecogSnapArea;

	BOOL m_fEnableScaleSpeed;						// 软件测速使能

	// 是否输出调试信息
	int m_fOutputDebugInfo;

	// 是否判断广西警标志
	int m_iRecogGxPolice;
	//新军牌识别开关
	int m_iRecogNewMilitary;

	//第5位是否开字母识别
	int m_fEnableAlpha_5;

	// 是否限制相同的车牌不能出两次
	int m_iSendTwinResult;

	// 限制相同车牌出现的时间间隔(秒)
	int m_iBlockTwinsTimeout;

	// 平均得分下限
	int m_iAverageConfidenceQuan;

	// 汉字得分下限
	int m_iFirstConfidenceQuan;

	// 本地化汉字，38~68, 58为“桂”
	int m_nDefaultChnId;				

	//双层牌开关
	int m_iEnableDefaultDBType;
	int m_iMiddleWidth;
	int m_iBottomWidth;
	
	//车牌检测方差控制
	BOOL m_fMinVarianceUserCtrl;
	int m_iMinVariance;

	//新WJ处理开关
	BOOL m_fEnableNewWJSegment;
	//新WJ字符替换
	BOOL  m_fEnableDefaultWJ;
	char  m_strDefaultWJChar[3];

	//大牌识别开关
	BOOL  m_fEnableBigPlate;
	//分割前是否进行图片增强
	BOOL m_fEnablePlateEnhance;
	//是否处理半黑白牌
	BOOL m_fEnableProcessBWPlate;
	//分割前小图拉伸宽度阈值
	int m_iPlateResizeThreshold;
	// 处理清晰图未识别
	BOOL m_fUseEdgeMethod;

	//灯组数量
	int m_nLightCount;	
	//当前场景
	int m_iLastLightStatus;
	//实时红绿灯状态
	int m_iCurLightStatus;
	//上一个有效的红绿灯状态
	int m_iLastOkLightStatus;

	int m_iLastTriggerTimeMs;
	static int m_sMinTriggerTimeMs;			//两次触发的时间间隔
	DWORD32 m_dwLastTriggerInfo;								//最后一次触发的信息
	static const int s_iMinTriggerPosTime = 200;		//两个位置触发的最小时间间隔		
	int m_iLastFirstTime;
	int m_iLastSecondTime;
	int m_iLastThirdTime;
	int m_iRushSpeed;									//闯红灯的速度

	int m_iSceneCount;
	//交通灯场景状态输出间隔
	int m_nValidSceneStatusCount;
	//快速转换场景
	bool m_fCheckSpeed;
	//连续无效场景数
	int m_nErrorSceneCount;

	int m_iSpan;

	//白天黑夜视频亮度阈值
	int m_iNightThreshold;
	int m_iPlateLightCheckCount;		//车牌亮度检测间隔
	int m_iMinPlateBrightness;
	int m_iMaxPlateBrightness;
	//是否控制偏光镜
	int m_iCtrlCpl;
	//当前偏光镜状态 1:使能 0:不使能 -1：未启用
	int m_iCplStatus;
	//是否强制白天灭补光灯
	int m_iForceLightOffAtDay;
	int m_iForceLightOffThreshold;
	int m_iForceLightOffFlag;
	//是否需要重新根据环境亮度调整
	int m_iAdjustForAvgBrigtness;
	//需要调整的时标
	DWORD32 m_dwAdjustTick;
	// zhaopy 调节的亮度等级限制
	LIGHT_TYPE m_nLightTypeCpl;
	int m_iPulseLevel;

	//车辆到达位置
	int m_iCarArrivedPos;
	//无牌车到达位置
	int m_iCarArrivedPosNoPlate;

	//车辆到达位置(黄牌)
	int m_iCarArrivedPosYellow;
	//车辆到达延迟距离
	int m_iCarArrivedDelay;

	// 车型判断参数
	int m_nCarLenTop;				// 小车在图象顶部的高度
	int m_nCarLenBottom;			// 小车在图象底部的高度
	int m_nCarWTop;					// 小车在图象顶部的宽度
	int m_nCarWBottom;				// 小车在图象底部的宽度
	float m_fltSlopeH;
	float m_fltSlopeW;

	BOOL m_fOutputCarSize;			// 输出车辆尺寸

	int	m_nVideoDetMode;			// 0:无视频检测 1:只是背景检测 2:完整视频检测
	CRect m_rcVideoDetArea;		//视频检测区域

	DWORD32 m_dwLastCarLeftTime; // 最近一次出牌时间

	//每个方向红绿灯开始时间(左转,直行,右转,掉头）
	LIGHT_REALTIME m_redrealtime;
	//绿灯开始相对时间
	LIGHT_TICK m_greentick;
	//红灯延迟时间
	LIGHT_TICK m_ltRedRelay;

	LIGHT_TYPE m_nFirstLightType;
	int m_iOutputInLine;
	// 压线检测灵敏度参数
	float m_fltOverLineSensitivity;

	int m_iOutPutType;					// 输出方式
	float m_fltBikeSensitivity;			//非机动车判断的灵敏度
	int m_iWalkManSpeed;				//判断行人的灵敏度

	DWORD32 m_dwTriggerTimes;			//触发次数

	//视频检测投票帧数.
	int m_iVoteFrameCount;
	//视频检测是否检测横向行驶的物体
	int m_iCheckAcross;
	//过滤掉第三张抓拍图为绿灯的违章
	int m_iFilterRushPeccancy;

	//韦开拓 过滤大车车身多检参数 2011-4-6
	BOOL m_fFilterMoreReview;
	float m_fltHMThreld;       //白天阈值
	float m_fltHMThreldNight;  //夜景阈值
	//模板匹配直方图延迟更新帧数
	int m_HMDelay;             //白天延迟帧数
	int m_HMDelayNight;        //夜景延迟帧数
	int m_iMaxMatchDiff;

	TRACKER_CFG_PARAM m_cfgParam;
	//added by liujie 控制大车多检
	CAR_LEFT_STRUCT m_leftCar[16];
	int m_iBigCarDelayFrameNo;

	int m_iCarArriveTrig; //收费站模式下是否进行抓拍

	int m_nIOLevel;
	
	//added by liujie MFC前端演示矩形框
	//int m_RectNum[DRAW_RECT_NUM];
	//HiVideo::CRect m_DrawRect[DRAW_RECT_NUM][DRAW_RECT_LEN];

	int m_iNoPlateDetMode; //无牌车检测模式 0:标准模式 1:模板模式

	//模型检测框
	RECTA m_rgEPCarPos[s_knMaxPlateCandidate];

	// zhaopy 0624
	//控制雨夜场景右边外车道车辆多检
	int m_iEdgeFilterThreld;
	
	// zhaopy
	// 过滤无牌车违章
	BOOL m_fFilterNoPlatePeccancy;
    BOOL m_fEnableDetFace;
	int m_nMinFaceScale;
	int m_nMaxFaceScale;

	// 车辆行驶方向 -- 用于判断逆行
	MovingDirection	m_TravelDirection;	

	//车牌颜色识别控制
	int m_nEnableColorReRecog;  //小车是否取车牌两边区域重识别结果
	int m_nWGColorReThr;        //灰白重识别——偏向于识别黑
	int m_nWColorReThr;        //白重识别——灰白调整，偏向于识别灰
	int m_nBColorReThr;        //黑重识别——灰黑调整，偏向于识别灰
	bool m_fEnableCorlorThr;  //环境亮度与车身亮度比较开关
	int m_nBGColorThr;        //车身亮度比环境亮度大BGColorThr的黑色车，设置成灰色车
	int m_nWGColorThr;        //晚上车身亮度比环境亮度大WGColorThr的灰色车，设置成白色车

	// 是否有修改参数的操作,有则需要重置算法的SetImageSize接口
	bool m_fIsModifyParam;
	CCarDetector m_cCarDetector;
};

#endif // _TRACKERIMPL_INCLUDED__
