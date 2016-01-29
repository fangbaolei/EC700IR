#ifndef __EP_TRACK_INFO_H__
#define __EP_TRACK_INFO_H__

#include "svEPApi\svEPApi.h" 
#include "swplatetype.h"
#include "swImageObj.h"
#include "trackerdef.h"
#include "VirtualRefImage.h"
#include "RoadInfo.h"

struct EPBestPlateInfo
{
    EPBestPlateInfo()
    {
        pimgPlate = NULL;
        fltConfidence = 0.f;
    }
    ~EPBestPlateInfo()
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
    }
    // 转移到另一变量上 
    HRESULT Detach(EPBestPlateInfo& dst)
    {
        dst = *this;
        pimgPlate = NULL;
        fltConfidence = 0.f;
        return S_OK;
    }
    IReferenceComponentImage *pimgPlate;
    float fltConfidence;
    HiVideo::CRect rcPlatePos;				// 分割后的外围位置
    DWORD32 dwFrameTime;					// 当前帧时间
    HiVideo::CRect rcPos;					// 车牌在整幅图像中的位置
    BYTE8 rgbContent[8];
};

class CEPTrackInfo
{
public:
    enum TRACK_STATE
    {
        TS_FREE = -1,
        TS_NEW = 0,
        TS_TRACKING = 1,
        TS_RESET = 2
    };
    enum TRACK_TYPE
    {
        TT_UNKNOW = -1,
        TT_SMALL_CAR = 0,
        TT_MID_CAR = 1,
        TT_LARGE_CAR = 2,
        TT_BIKE = 3,
        TT_WALK_MAN = 4
    };

    CEPTrackInfo(void);
    ~CEPTrackInfo(void);

    virtual int GetID();
    virtual int GetState();
    virtual int GetType();
    virtual int GetPosCount();
    virtual HV_RECT GetPos(int nIndex);
    virtual HV_RECT GetLastPos();
    virtual void Reset();    // 仅重置检测识别模块的结果，并非释放本对象数据
    virtual CAR_COLOR GetColor();

    virtual BOOL GetPlate(svEPApi::EP_PLATE_INFO* pPlateInfo);
    virtual int GetPlateRecogCount();
    virtual float GetPlateAvgConf(); // 取车牌平均可信度
    virtual float GetPlateAvgFirstConf(); // 取车牌平均首字可信度
    virtual HV_RECT GetLastPlateDetPos();    
    virtual HV_RECT GetLastPlateRecogPos();
    virtual int GetLastPlateRecogPosCount(); // 取上一次识别到牌的物体运动轨迹计数
    virtual int GetLastPlateDetPosCount(); // 取上一次检到牌的物体运动轨迹计数

    virtual float GetLastPlateConf();
    virtual float GetLastPlateFirstConf();

    virtual int GetVotedPlateTypeCount();
    /// 取有效检测数，如果车牌信息不可靠，仍可能为无牌车多检出牌或坏牌
    /// 此时判断有效检测数，如果较高(例如>10)则可判定为无牌车或坏牌车
    virtual int GetValidDetCount(); 

    /// 当前跟踪状态是否为压线，仅在停止线发下判断
    virtual BOOL IsPressRoadLine(int* pnRoadNum = NULL); 
    /// 当前跟踪状态是否为停止
    virtual BOOL IsStop();  
    /// 当前跟踪状态是否逆行
    virtual BOOL IsReverseRun(); 
    /// 当前是否为越线 
    virtual BOOL IsCrossRoadLine(int* pnRoadNum = NULL);

    virtual int GetRoadNum();

    /// 取行驶类型
    virtual RUN_TYPE GetRunType();

    /// 取车牌相似数
    virtual int GetPlateSimilarityCount();

public:
    void Free();   // 释放本对象数据
    virtual BOOL IsUpdated();                    // 是否已经更新检测结果

    // 用算法结果更新所有跟踪
    static HRESULT UpdateAll(
        CEPTrackInfo* rgDjObj,
        int iMaxObj,
        int* piObjCnt,  // 输入输出
        svEPApi::IEPTrack** rgpTracker, 
        int iTrackerCnt
        );

    static void SetRoadInfo(CRoadInfo* pRoadInfo);   // 设置车道信息，必须设置

    // 当前帧的时间。
    static DWORD32 s_iCurImageTick;

private:
    virtual void Update(svEPApi::IEPTrack* pIEPTrack);  // 使用检测结果更新状态信息等
    // < 与DjTrack模块对应变量
    DWORD32 m_dwID;
    svEPApi::IEPTrack* m_pEPTrack;
    // >
    TRACK_STATE m_nTrackState;
    TRACK_TYPE m_nTrackType;

    // 避免多次调算法接口取车牌，这里保存上一帧车牌信息
    svEPApi::EP_PLATE_INFO m_cLastPlate;  // 该信息在更新结果列队时自动更新  

    // 公共车道信息，由其调用者调定
    static CRoadInfo* m_pRoadInfo;

public:

    // 有牌车的图
    EPBestPlateInfo m_rgBestPlateInfo[PLATE_TYPE_COUNT]; // 为每种类型都保存一个得分最高的车牌图像
    IVirtualRefImage *m_pimgBestSnapShot;    // 最清晰大图
    IVirtualRefImage *m_pimgLastSnapShot;
    IVirtualRefImage *m_pimgBeginCapture;
    IVirtualRefImage *m_pimgBestCapture;
    IVirtualRefImage *m_pimgLastCapture;
    DWORD32 m_nStartFrameNo;
    DWORD32 m_nEndFrameNo;
	DWORD32 m_dwFirstFrameTime;

    DWORD32 m_nCarArriveTime;  // TODO 实现赋值,暂时用过挺止线时间

    int m_nPressRoadLineNum;    // -1为没有该事件
    int m_nCrossRoadLineNum;    // -1为没有该事件
    int m_nReverseRunCount;     // 每隔几帧检一次逆行 

    int m_iOnePosLightScene;
    int m_iTwoPosLightScene;
    int m_iThreePosLightScene;
    int m_iPassStopLightScene;
    int m_iPassLeftStopLightScene;


    HV_RECT m_rcBestPos;  // 这个变量好像没用，逆行时等价于m_rgRect[0]，顺行等价于m_rgRect[3]
    HV_RECT m_rcLastPos;  // 这个变量好像没用，逆行时等价于m_rgRect[1]，顺行等价于m_rgRect[4]
    ///每张图片的坐标
    ///2，3，4分别为三个抓拍位置，本方案严格对应从近到远三条线的位置，与顺逆行无关 
    ///0，1原来表示逆行位置，本方案已经弃用，目前仅为了代码兼容
    HiVideo::CRect m_rgRect[5]; 

    // 最大车牌可信度， 用于抓拍
    float m_fltMaxPlateConf; 

    bool m_fTrackReliable; //跟踪是否可靠，只有可靠的才是出结果条件之一
    
    bool m_rgfIsCapTime[3]; //是否本帧抓排 

    //上一次检到牌的车牌信息，由于算法可能采用隔帧检测识别，需要保存数据
    //仅采用了隔帧检测识别，且上一帧为检测、检测有可能需要更新最佳车牌时，该数据有效
    EPBestPlateInfo m_cLastDetPlateInfo;  

    RUN_TYPE m_nRunType; // 行驶类型，跟踪结束后有效

    // 车牌位置轨迹。Update时自动更新。
    // 注意：并不是每帧都记录，只有当车牌在移动时才会记录。当车牌在同一个位置时不记录
    static const int m_nMaxPlateMovePos = 64;
    int m_nPlateMovePosCount;
    HV_RECT m_rgrcPlateMovePos[m_nMaxPlateMovePos];
    // zhaopy 记录时间
    DWORD32 m_rgdwPlateTick[m_nMaxPlateMovePos];

    // 识别无效帧数
    int nRecogInValidCount;
};

#endif // __EP_TRACK_INFO_H__

