#pragma once

#include "svTgIrApi/svTgIrApi.h"
#include "swplatetype.h"
#include "swImageObj.h"
#include "trackerdef.h"
#include "VirtualRefImage.h"
#include "RoadInfo.h"

namespace swTgApp 
{

struct BestPlateInfo
{
    BestPlateInfo()
    {
        pimgPlate = NULL;
        fltConfidence = 0.f;
		nPlateSimilar = 0;
    }
    ~BestPlateInfo()
    {
    }
    void Clear()
    {
        fltConfidence = 0.f;
        if (pimgPlate != NULL)
        {
            pimgPlate->Release();
            pimgPlate = NULL;
        }
        rgbContent[0] = 0;
		nPlateSimilar = 0;
    }
    // 转移到另一变量上 
    HRESULT Detach(BestPlateInfo& dst)
    {
        dst = *this;
        pimgPlate = NULL;
        fltConfidence = 0.f;
		nPlateSimilar = 0;
        return S_OK;
    }
    IReferenceComponentImage *pimgPlate;
    float fltConfidence;
	int nPlateSimilar;						// 当前牌的相似度
    HiVideo::CRect rcPlatePos;				// 分割后的外围位置
    DWORD32 dwFrameTime;					// 当前帧时间
    HiVideo::CRect rcPos;					// 车牌在整幅图像中的位置
    BYTE8 rgbContent[8];
};

class CAppTrackInfo
{
public:
    enum TRACK_STATE
    {
        TS_FREE = -1,
        TS_NEW = 0,
        TS_TRACKING = 1,
        TS_END = 2
    };
    enum TRACK_TYPE
    {
        TT_UNKNOW = -1,
        TT_SMALL_CAR = 0,
        TT_MID_CAR = 1,
        TT_LARGE_CAR = 2,
        TT_BIKE = 3,
        TT_WAKL_MAN = 4
    };

    CAppTrackInfo(void);
    ~CAppTrackInfo(void);

    virtual unsigned int GetID();
    virtual TRACK_STATE GetState();
    virtual TRACK_TYPE GetType();
    virtual int GetPosCount();
    virtual sv::CSvRect GetPos(int nIndex);
    virtual sv::CSvRect GetLastPos();
    virtual const svTgIrApi::TG_TRACK_LOCUS& GetLocus(int nIndex);
    virtual const svTgIrApi::TG_TRACK_LOCUS& GetLastLocus();
    virtual const svTgIrApi::TG_PLATE_INFO& GetPlate(int nIndex);  // 取过程中识别到的车牌
    virtual int GetPlateCount();
    virtual void End();    // 仅结束检测识别模块的结果，并非释放本对象数据

    /// 取结果信息，包括车牌识别结果。仅跟踪为TS_END状态数据才有效。
    virtual sv::SV_RESULT GetResult(svTgIrApi::ITgTrack::TG_RESULT_INFO* pResInfo);

    /// 取跟踪扩展信息
    virtual sv::SV_RESULT GetExInfo(svTgIrApi::TG_TRACK_EX_INFO* pExInfo);   

//     /// 当前跟踪状态是否为压线，仅在停止线发下判断
//     virtual sv::SV_BOOL IsPressRoadLine(int* pnRoadNum = NULL);
//     /// 当前跟踪状态是否为停止
//     virtual sv::SV_BOOL IsStop();
//     /// 当前跟踪状态是否逆行
//     virtual sv::SV_BOOL IsReverseRun();
//     /// 当前是否为越线
//     virtual sv::SV_BOOL IsCrossRoadLine(int* pnRoadNum = NULL);

    virtual int GetRoadNum();

public:
    void Free();   // 释放本对象数据
    virtual sv::SV_BOOL IsUpdated();                    // 是否已经更新检测结果

    // 用算法结果更新所有跟踪
    static sv::SV_RESULT UpdateAll(
        CAppTrackInfo* rgDjObj,
        int iMaxObj,
        int* piObjCnt,  // 输入输出
        svTgIrApi::ITgTrack** rgpTracker,
        int iTrackerCnt
    );

    static void SetRoadInfo(swTgApp::CRoadInfo* pRoadInfo);   // 设置车道信息，必须设置

    // 当前帧的时间。
    static sv::SV_UINT32 s_iCurImageTick;

    svTgIrApi::ITgTrack* m_pTrack;

    // 从算法接口获取的跟踪扩展信息
    svTgIrApi::TG_TRACK_EX_INFO m_cExInfo;

    BOOL m_fCarArrived;
    BOOL m_fIsTrigger;
    DWORD32 m_nCarArriveRealTime;
    CRect m_rcCarArrivePos;
private:
    virtual void Update(svTgIrApi::ITgTrack* pITgTrack);  // 使用检测结果更新状态信息等
    // < 与DjTrack模块对应变量
    sv::SV_UINT32 m_dwID;

    // >
    TRACK_STATE m_nTrackState;
    TRACK_TYPE m_nTrackType;

    // 公共车道信息，由其调用者调定
    static swTgApp::CRoadInfo* m_pRoadInfo;

public:

    sv::CSvRect m_rcCurPos;

    // 有牌车的图
    BestPlateInfo m_rgBestPlateInfo[svTgIrApi::PLATE_TYPE_COUNT]; // 为每种类型都保存一个得分最高的车牌图像
    IVirtualRefImage *m_pimgBestSnapShot;    // 最清晰大图
    IVirtualRefImage *m_pimgLastSnapShot;

    sv::SV_UINT32 m_nStartFrameNo;
    sv::SV_UINT32 m_nEndFrameNo;
    sv::SV_UINT32 m_dwFirstFrameTime;

    sv::SV_UINT32 m_nCarArriveTime;  // TODO 实现赋值,暂时用过挺止线时间

    int m_nReverseRunCount;     // 每隔几帧检一次逆行 

    sv::CSvRect m_rcBestPos;   // 对应m_pimgBestSnapShot
    sv::CSvRect m_rcLastPos;   // 对应m_pimgLastSnapShot
    sv::CSvRect m_rcBestPlatePos;   // 对应m_pimgBestSnapShot，该坐标不一定有，以-1为无效值。
    sv::CSvRect m_rcLastPlatePos;   // 对应m_pimgLastSnapShot，该坐标不一定有，以-1为无效值。
    
    int m_nBestPlateSimilar;		// 最佳大图车牌相似度
    int m_nLastPlateSimilar;		// 最后大图车牌相似度

    RUN_TYPE m_nRunType; // 行驶类型，跟踪结束后有效

    // 车牌位置轨迹。Update时自动更新。
    // 注意：并不是每帧都记录，只有当车牌在移动时才会记录。当车牌在同一个位置时不记录
    static const int m_nMaxPlateMovePos = 64;
    int m_nPlateMovePosCount;
    HV_RECT m_rgrcPlateMovePos[m_nMaxPlateMovePos];
    // zhaopy 记录时间
    DWORD32 m_rgdwPlateTick[m_nMaxPlateMovePos];

    bool m_fCanOutput;

    CAR_COLOR m_nCarColor;   // 跟踪结束时，为投票结果

    // 输出车道号，注意为应用相关，有起始、左右起之分
    int m_nOutRoadNum;

    // 抓拍图是否有牌，目前只针对傍晚有效
    bool m_fCaptureFrameHavePlate;
    int m_nLastPlateY;    
	
    // 要记录抓拍计数，出结果时使用
    DWORD32 m_dwTriggerCameraTimes;
};

}
