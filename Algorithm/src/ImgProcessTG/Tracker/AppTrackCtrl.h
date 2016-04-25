#pragma once

#include "swbasetype.h"
#include "swimage.h"

#include "svTgVvdApi/svTgVvdApi.h"
#include "hvinterface.h"
//#include "DspLinkCmd.h"
#include "AppTrackInfo.h"
#include "platerecogparam.h"
#include "RoadInfo.h"
#include "trackerdef.h"
#include "DspLinkCmd.h"
#include "VirtualRefImage.h"

class ITrackerCallback; // trackercallback.h

namespace swTgApp 
{

    class CPlateLightCtrl
    {
    public:
        CPlateLightCtrl();
        ~CPlateLightCtrl();
        void Init(int nMaxPlateY, int nMinPlateY, int nCheckOut, int nNightTH, int nMaxAgcTh);
        void SetFirstLightType(LIGHT_TYPE nLightType);

        HRESULT CheckLight(int nEnvAvgY);
        HRESULT UpdatePlateLight(int iY);

        LIGHT_TYPE GetPlateLightType();
        int GetWDRLevel();
        void SetArmEnvPeriodInfo(bool fIsArmNight){m_fIsARMNight = fIsArmNight;}	//传入ARM端的时段判断结果	

        void SetMaxAgcTh(int nTh) {m_nMaxAGCTH = nTh;}
    private:
        HRESULT UpdateLightType(int iY, bool fIsAvgBrightness);

        // zhaopy
        LIGHT_TYPE m_nFirstLightType;
        unsigned int m_dwLastCarLeftTime;
        LIGHT_TYPE m_LightType;

        int m_iMaxPlateBrightness;
        int m_iMinPlateBrightness;
        int m_iPlateLightCheckCount;
        int m_iNightThreshold;

        LIGHT_TYPE m_nPlateLightType;
        int m_nWDRLevel;
        BOOL m_fIsARMNight;			//ARM端判断为晚上
        int m_nMaxAGCTH;
        int m_nEnvAvgY;
    };

    struct LAST_RESULT_INFO
    {
        char szPlate[32];
        unsigned int nArriveTime;
    };


    struct TRIGGER_INFO
    {
        sv::CSvRect rcTriggerRecog;   // 抓拍认别区域
        DWORD32 nTrackId;             // 跟踪ID
        DWORD32 nTriggerTimesCnt;     // 触发次数计数
    };

    class CAppTrackCtrl
    {
    public:
        CAppTrackCtrl(void);
        virtual ~CAppTrackCtrl(void);

        HRESULT Init(
            TRACKER_CFG_PARAM* pCfgParam,
            PlateRecogParam* pRecogParam,
            int iFrameWidth, 
            int iFrameHeight
            );
        HRESULT Uninit();

        HRESULT Process(
            PROCESS_ONE_FRAME_PARAM* pProcParam,
            PROCESS_ONE_FRAME_DATA* pProcessData,
            PROCESS_ONE_FRAME_RESPOND* pProcessRespond
            );

        // 设置初始车牌亮度等级
        void SetFirstLightType(LIGHT_TYPE nLightType);

        bool TriggerCallBack(
            sv::SV_RECT rcPos,      /// 回调的位置
            int nTrackID
            );
    private:

        /*HRESULT RecogOneFrame(
            PROCESS_ONE_FRAME_PARAM* pProcParam,
            PROCESS_ONE_FRAME_DATA* pProcessData,
            PROCESS_ONE_FRAME_RESPOND* pProcessRespond,
            HV_COMPONENT_IMAGE hvImgFrame,
            HiVideo::CRect& rcRecogArae
            );*/

        HRESULT PreProcess(
            PROCESS_ONE_FRAME_PARAM* pProcParam,
            PROCESS_ONE_FRAME_DATA* pProcessData,
            PROCESS_ONE_FRAME_RESPOND* pProcessRespond
            );
        HRESULT PostProcess();


        HRESULT CheckLightType(int iCarY, bool fIsAvgBrightness);

        HRESULT ProcessTrackState(
            HV_COMPONENT_IMAGE *pImage,
            PROCESS_ONE_FRAME_RESPOND* pProcessRespond
            );
        HRESULT OnTrackerFirstDet(
            CAppTrackInfo* pTrackInfo
            );
        HRESULT OnTrackerEnd(
            CAppTrackInfo* pTrackInfo
            );

        bool IsTrackerCanOutput(CAppTrackInfo* pTrackInfo);

        HRESULT FireCarLeftEvent(CAppTrackInfo* pTrackInfo);

        ///检测抓拍
        HRESULT CheckCapture(
            HV_COMPONENT_IMAGE* pImage
            );

        ///处理违章
        HRESULT CheckPeccancy(
            HV_COMPONENT_IMAGE* pImage
            );

        ///处理每帧单个跟踪的状态
        HRESULT CheckOneState(CAppTrackInfo* pTrackInfo, HV_COMPONENT_IMAGE* pImage);
        ///检查是否车辆到达，如果到达，满足条件时触发抓拍
        HRESULT CheckCarArrive(CAppTrackInfo* pTrackInfo, HV_COMPONENT_IMAGE* pImage);

        HRESULT SetTgApiParam(
            TRACKER_CFG_PARAM* pCfgParam,
            PlateRecogParam* pRecogParam,
            int iFrameWidth, 
            int iFrameHeight
            );

        HRESULT SetCtrlParam(
            TRACKER_CFG_PARAM* pCfgParam
            );


        // 返回是否成功抓拍
        bool ProcessCarArrive(
            CAppTrackInfo* pTrack, 
            DWORD32 dwImageTime, 
            PROCESS_ONE_FRAME_RESPOND* pProcessRespond
            );

		void pwm_writeinfo_in_dsp(DWORD32 temp_data);
		void pwm_int_in_dsp(void);
        void TriggerCamera(const int iRoadNum);
		CROSS_OVER_LINE_TYPE IsOverYellowLine(CAppTrackInfo* pTrack);
        /// 还原到原始车道号，自动恢复右起、起始号不为0的情况，用于输出
        //int RecoverRoadNum(int iRoadNum);
		bool IsNeedCaptureAll();
    protected:
        static const int MAX_MOD_DET_INFO = svTgVvdApi::MOD_DET_INFO::TYPE_COUNT;
        svTgVvdApi::MOD_DET_INFO m_rgModDetInfo[MAX_MOD_DET_INFO];
        svTgVvdApi::TG_PARAM m_cApiParam;
        svTgVvdApi::CTgCtrl* m_pTgCtrl;
        svTgVvdApi::CTgCtrl::LIGHT_TYPE m_nEnvLightType;  // 环境亮度类型

        TRACKER_CFG_PARAM m_cTrackerCfgParam;

        int m_nEnvLight;  // 环境亮度

        int m_iFrameWidth;
        int m_iFrameHeight;

        swTgApp::CRoadInfo m_cRoadInfo;

        static const int MAX_TRACK_INFO = 30;
        CAppTrackInfo m_rgTrackInfo[MAX_TRACK_INFO];
        int m_iTrackInfoCnt;

        ITrackerCallback* m_pCallback;

        PlateRecogParam m_cRecogParam;
        PROCESS_ONE_FRAME_DATA* m_pProcessData;
        PROCESS_ONE_FRAME_RESPOND* m_pProcessRespond;

        IVirtualRefImage* m_pCurIRefImage;

        // 用于过滤相同车牌
        static const int MAX_LAST_RES_PLATE = 50;
        LAST_RESULT_INFO m_rgLastResultPlate[MAX_LAST_RES_PLATE];
        int m_nLastResultPlateCount;


        int m_iAverageConfidenceQuan; // 平均得分下限
        int m_iFirstConfidenceQuan;  // 汉字得分下限
        int m_iBlockTwinsTimeout; // 相同牌过滤时间，单位秒
        int m_iArriveOnePos;        // 取第一张大图位置
        int m_iArriveTwoPos;		// 取第二张大图位置
        int m_iDetectReverseEnable;

        BOOL m_fIsCheckLightType;
        CPlateLightCtrl m_cPlateLightCtrl;
        // 是否使能车辆到达抓拍，在本类中动态改变
        BOOL m_fEnableCarArriveTrigger;

        static const int MIN_TRIGGER_TIME = 150;	// 两次触发最小间隔时间
        DWORD32 m_dwLastTriggerTick;	// 最后一次触发的时间
		DWORD32 m_dwLastTriggerRoadNum;
        DWORD32 m_dwTriggerCameraTimes;		// 当前触发次数

        DWORD32 m_dwRecvTriggerImageTimes;  // 接收到的抓拍图次数
//
//        // 抓拍识别位置队列
//        static const int MAX_TRIGGER_INFO = 10;
//        TRIGGER_INFO m_rgTriggerInfo[MAX_TRIGGER_INFO];
//        int m_nTriggerInfoCnt;

        // 帧延迟毫秒，根据延迟时间调整触发时机
        DWORD32 m_dwFrameDelay;
        // 开始处理时的系统时间
        DWORD32 m_dwProcessStartSystemTick;    
    };

}
