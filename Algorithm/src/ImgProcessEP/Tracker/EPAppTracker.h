#ifndef _DJ_APP_TRACKER_H_
#define _DJ_APP_TRACKER_H_

#include "swbasetype.h"
#include "swimage.h"

#include "svEPApi/svEPApi.h"
#include "hvinterface.h"
//#include "DspLinkCmd.h"
#include "EPTrackInfo.h"
#include "../TrafficLight/TrafficLightImpl.h"
#include "platerecogparam.h"
#include "RoadInfo.h"
#include "trackerdef.h"
#include "DspLinkCmd.h"
#include "VirtualRefImage.h"
#include "ScaleSpeed.h"

class ITrackerCallback; // trackercallback.h

// 临时保存的车牌信息结构，用于过滤相同车牌
typedef struct tag_CarInfo
{
	char szPlate[32];
	DWORD32 iPlateTick;
	int iPlateRoad;
}
PLATE_INFO;

class CEPAppTracker
{
public:
    CEPAppTracker(void);
    ~CEPAppTracker(void);

    HRESULT Init(
        TRACKER_CFG_PARAM* pCfgParam,
        PlateRecogParam* pRecogParam,
        HvCore::IHvModel* pHvParam, 
        int iFrameWidth, 
        int iFrameHeight,
        IScaleSpeed *pScaleSpeed
        );

    // todo.
    // 红绿灯相关回调当前还是回调到老的TRACKER接口，
    // 所以需增加以下接口
    HRESULT SetTrackerCallBack(ITracker* pCallBack);
    void UpdatemRedLightRect(HV_RECT* prgRect, const int& iCount);


    HRESULT Uninit();

    HRESULT Process(
        PROCESS_ONE_FRAME_PARAM* pProcParam,
        PROCESS_ONE_FRAME_DATA* pProcessData,
        PROCESS_ONE_FRAME_RESPOND* pProcessRespond
        );
    HRESULT PreProcess(
        PROCESS_ONE_FRAME_PARAM* pProcParam,
        PROCESS_ONE_FRAME_DATA* pProcessData,
        PROCESS_ONE_FRAME_RESPOND* pProcessRespond
        );
    HRESULT PostProcess();

    /// 设置回调，必须在Init之前
    HRESULT SetCallBackFunc(ITrackerCallback* pCallback);

    HRESULT SetFirstLightType(LIGHT_TYPE nLightType);

    HRESULT CalcCarSpeed(float &fltCarSpeed, float &fltScaleOfDistance, CEPTrackInfo* pTrackInfo, IScaleSpeed *m_pScaleSpeed);
private:

    HRESULT ProcessTrackState(
        HV_COMPONENT_IMAGE *pImage,
        PROCESS_ONE_FRAME_RESPOND* pProcessRespond
        );
    HRESULT OnTrackerFirstDet(
        CEPTrackInfo* pTrackInfo
        );
    HRESULT OnTrackerEnd(
        CEPTrackInfo* pTrackInfo
        );

    bool IsTrackerCanOutput(CEPTrackInfo* pTrackInfo);

    HRESULT FireCarLeftEvent(CEPTrackInfo* pTrackInfo);

    ///是否按车道行驶
    bool CheckRoadAndRun(RUN_TYPE runtype, int runroadtype);

    ///是否闯红灯
    bool CheckRush(int iPreLight, int iAcrossLight, int iThreeLight, int runroadtype, int runtype, DWORD32 dwCaptureTime);

    ///检测抓拍
    HRESULT CheckCapture(
        HV_COMPONENT_IMAGE* pImage
        );

    ///处理违章
    HRESULT CheckPeccancy(
        HV_COMPONENT_IMAGE* pImage
        );

    ///处理每帧单个跟踪的状态
    HRESULT CheckOneState(CEPTrackInfo* pTrackInfo, HV_COMPONENT_IMAGE* pImage);

    HRESULT CheckPeccancyType(
        int iPreLight,			//压线前的红绿灯场景
        int iAcrossLight,		//过线后的红绿灯场景
        int iThreeLight,		//第三张抓拍位置的红绿灯场景
        RUN_TYPE runtype,	//行驶类型
        int runroadtype,      //行驶车道类型
        DWORD32 dwCaptureTime,	//抓拍时间
        /*PECCANCY_TYPE*/int * pPeccancyType //违章类型,黄国超修改为int类型
        );
    HRESULT CheckTrafficLight(HV_COMPONENT_IMAGE* pSceneImage );

    HRESULT SetObjDetCfgParam(
        TRACKER_CFG_PARAM* pCfgParam,
        int iFrameWidth, 
        int iFrameHeight
        );

    HRESULT SetPlateRecogCfgParam(
        TRACKER_CFG_PARAM* pCfgParam,
        PlateRecogParam* pRecogParam
        );
    HRESULT InitTrafficLight(
        TRACKER_CFG_PARAM* pCfgParam
        );
    HRESULT SetMiscParam(
        TRACKER_CFG_PARAM* pCfgParam
        );
    
    // 抓拍回调
    static int Capture_CallBack(
        svEPApi::IEPTrack* pTrack,     ///< 跟踪指针
        int nCapPosIndex      ///< 抓拍位置下标:0，1，2，从近到远
        );
    static CEPAppTracker* m_pInstance;  // 用于调用DoCapture
    // 真正处理抓拍函数
    HRESULT DoCapture(svEPApi::IEPTrack* pITracker, int nCapPosIndex);

    HRESULT CheckLight();
    HRESULT CheckLightType(int iCarY, bool fIsAvgBrightness);

protected:
    static const int MAX_MOD_DET_INFO = svEPApi::MOD_DET_INFO::TYPE_COUNT;
    svEPApi::MOD_DET_INFO m_rgModelParam[MAX_MOD_DET_INFO];
    svEPApi::EP_PARAM m_cEPParam;
	svEPApi::CEPCtrl* m_pEPCtrl;
    svEPApi::EP_RESULT_INFO::LIGHT_TYPE m_nEnvLightType;  // 环境亮度类型
    LIGHT_TYPE m_nPlateLightType;

    // zhaopy
    LIGHT_TYPE m_nFirstLightType;
    unsigned int m_dwLastCarLeftTime;
    LIGHT_TYPE m_LightType;
    int m_iMaxPlateBrightness;
    int m_iMinPlateBrightness;
    int m_iPlateLightCheckCount;
    int m_iNightThreshold;
    bool m_fIsCheckLightType;

    int m_nEnvLight;  // 环境亮度

    int m_iFrameWidth;
    int m_iFrameHeight;

    HiVideo::CRoadInfo m_cRoadInfo;

    static const int MAX_DJ_TRACK_INFO = 30;
    CEPTrackInfo m_rgTrackInfo[MAX_DJ_TRACK_INFO];
    int m_iTrackInfoCnt;

    ITrackerCallback* m_pCallback;

    //过滤掉第三张抓拍图为绿灯的违章
    int m_iFilterRushPeccancy;

    //<红绿灯专用参数
	CTrafficLightImpl m_cTrafficLight;
    //灯组数量
    int m_nLightCount;	
    //当前场景
    int m_iLastLightStatus;
    //实时红绿灯状态
    int m_iCurLightStatus;
    int m_iOutputInLine; //输出未过停止线的车辆

    int m_nRedLightCount; 
    HV_RECT m_rgRedLightRect[MAX_TRAFFICLIGHT_COUNT * 2];	//红灯灯组位置
    //>

    int m_iBestLightMode;  // 是否使用最亮图模式，该值为1时用跟踪里的最亮图代替最清晰图

    // 平均得分下限
    int m_iAverageConfidenceQuan;

    // 汉字得分下限
    int m_iFirstConfidenceQuan;

    //事件检测参数
    ACTIONDETECT_PARAM_TRACKER m_ActionDetectParam;
    float m_fltOverLineSensitivity;   // 压线检测灵敏度
    PlateRecogParam m_cRecogParam;
    PROCESS_ONE_FRAME_DATA* m_pProcessData;
    PROCESS_ONE_FRAME_RESPOND* m_pProcessRespond;

    IVirtualRefImage* m_pCurIRefImage;

    // todo
    ITracker* m_pTrackerCallback;

	int m_iRoadNumberBegin;							//车道编号起始方向
	int m_iStartRoadNum;								//起始车道号

	IScaleSpeed *m_pScaleSpeed;
	bool m_fEnableScaleSpeed;

	bool m_fFilterNoPlatePeccancy;
	int m_iBlockTwinsTimeout;

	// 用于过滤相同车牌
	static const int MAX_PLATE_INFO_COUNT = 50;
	PLATE_INFO m_rgPlateInfo[MAX_PLATE_INFO_COUNT];
	int m_iPlateInfoCount;

	// 最后一个蓝牌车的车牌宽度
	int m_iLastPlateWidth;
	// 超速车速
	int m_iLimitSpeed;
};

#endif //_DJ_APP_TRACKER_H_

