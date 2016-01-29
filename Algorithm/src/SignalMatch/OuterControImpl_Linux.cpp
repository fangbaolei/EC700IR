#include "HvPciLinkApi.h"
#include "OuterControlImpl_Linux.h"
#include "platerecogparam.h"
#include "ImgProcesser.h"
#if (RUN_PLATFORM == PLATFORM_DSP_BIOS)
#include "hv_io.h"
#endif
#include "HvParamIO.h"
#include "IPTControlImpl.h"
extern CParamStore g_cParamStore;

#define SAFE_DELETE_OBJ(pObj)						\
if (pObj)										    \
{													\
	delete pObj;									\
	pObj = NULL;									\
}

// 外总控信号定义
#define MAX_SIGNAL_TYPE		10				// 最大信号类型数量
#define MAX_SIGNAL_SOURCE	30				// 最大信号源数量

#define PLATE_HOLD_TIME		2000			// 车牌保留5秒
#define SIGNAL_HOLD_TIME	2000			// 信号保留5秒
#define SPEED_SCORE_PARAM	0.12			// 速度范围参数， 决定参与匹配的数度上限和下限

#define IPT_CLOCK_SYNC_TIME	43200000		// IPT时钟同步时间(12个小时)

/* ImgProcesser.cpp */
extern HRESULT AllImageAddRef(RESULT_IMAGE_STRUCT *pResultImage);
extern HRESULT AllImageRelease(RESULT_IMAGE_STRUCT *pResultImage);
extern HRESULT AllImageReleaseNotNull(RESULT_IMAGE_STRUCT *pResultImage);

bool g_fOuterOK(false);

int g_nSignalTypeNum(0);					// 信号类型总数
int g_nSignalSourceNum(0);					// 信号源总数

SIGNAL_TYPE *g_prgMatchSigType;				// 动态匹配信号类型数组指针
SIGNAL_SOURCE *g_prgSigSource;				// 动态信号源数组指针

CHvList<SIGNAL_INFO*> *g_prgQueSignal;		// 信号数据队列数组

// 控制信号队列读写同步临界区数组指针
HV_SEM_HANDLE g_rgSemSignal[MAX_SIGNAL_SOURCE];

CHvList<PLATE_INFO_hxl*> g_quePlate;		// 车牌数据队列
MATCH_CAP_ROAD g_rgmatchCapRoad[CAPROAD_LIST_COUNT];

HV_SEM_HANDLE g_SemPlate;			        // 控制读写同步
HV_SEM_HANDLE g_SemHvCore;			        // 控制访问HvCore同步
HV_SEM_HANDLE g_SemImage;			        // 控制每一帧图像

DWORD32 g_dwSignalHoldTime;					// 信号保持时间
DWORD32 g_dwPlateHoldTime;					// 信号保持时间
DWORD32 g_dwCarArriveTimes;					// CarArrive到达的次数
DWORD32 g_dwRoadFlag;

CHvList<OUTPUT_SIGNAL> g_queOutSignal;		// 输出脉冲队列

CHvList<SIGNAL_INFO*>  g_PanormicSignal;     //
HV_SEM_HANDLE g_SemPanormicSignal;

int g_iCapImgCount = 1;				        // 抓拍图片数量

static int g_nTestCount(0);

DWORD32 g_dwSoftSignalTime;			//软触发时标
DWORD32 g_dwSignalTime;				//硬触发时标
bool g_fSignalIsMain;	            //硬触发是否主信号

// 锁定车牌队列
inline void LockPlate()
{
    SemPend(&g_SemPlate);
}
// 释放车牌队列
inline void UnlockPlate()
{
    SemPost(&g_SemPlate);
}
// 锁定信号队列
inline void LockSignal(int nIndex)
{
    if ( nIndex < 0 || nIndex >= MAX_SIGNAL_SOURCE ) return;
    SemPend(&g_rgSemSignal[nIndex]);
}
// 释放信号队列
inline void UnlockSignal(int nIndex)
{
    if ( nIndex < 0 || nIndex >= MAX_SIGNAL_SOURCE ) return;
    SemPost(&g_rgSemSignal[nIndex]);
}
// 锁定核心
inline void LockCore()
{
    SemPend(&g_SemHvCore);
}
// 释放核心
inline void UnlockCore()
{
    SemPost(&g_SemHvCore);
}
// 锁定图片
inline void LockImage()
{
    SemPend(&g_SemImage);
}
// 释放图片
inline void UnlockImage()
{
    SemPost(&g_SemImage);
}

HRESULT ReleaseProcessEvent(PROCESS_EVENT_STRUCT *pProcessEvent)
{
    if (pProcessEvent == NULL)
    {
        return E_POINTER;
    }

    for (int i = 0; i < pProcessEvent->iCarLeftInfoCount; i++)
    {
        AllImageRelease(&(pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg));
    }

    return S_OK;
}

HRESULT AddRefProcessEvent(PROCESS_EVENT_STRUCT *pProcessEvent)
{
    if (pProcessEvent == NULL)
    {
        return E_POINTER;
    }

    for (int i = 0; i < pProcessEvent->iCarLeftInfoCount; i++)
    {
        AllImageAddRef(&(pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg));
    }

    return S_OK;
}

HRESULT CopyResultImageStruct( RESULT_IMAGE_STRUCT *pDest, const RESULT_IMAGE_STRUCT *pSrc )
{
    if (pDest == NULL || pSrc == NULL) return E_INVALIDARG;

    HV_memcpy(pDest, pSrc, sizeof(RESULT_IMAGE_STRUCT));

    if (pSrc->pimgPlate != NULL)
    {
        pSrc->pimgPlate->AddRef();
    }
    if (pSrc->pimgBestSnapShot != NULL)
    {
        pSrc->pimgBestSnapShot->AddRef();
    }
    if (pSrc->pimgLastSnapShot != NULL)
    {
        pSrc->pimgLastSnapShot->AddRef();
    }
    if (pSrc->pimgBeginCapture != NULL)
    {
        pSrc->pimgBeginCapture->AddRef();
    }
    if (pSrc->pimgBestCapture != NULL)
    {
        pSrc->pimgBestCapture->AddRef();
    }
    if (pSrc->pimgLastCapture != NULL)
    {
        pSrc->pimgLastCapture->AddRef();
    }
    if (pSrc->pimgPlateBin != NULL)
    {
        pSrc->pimgPlateBin->AddRef();
    }
    return S_OK;
}

HRESULT FreeResultImageStruct( RESULT_IMAGE_STRUCT *pObj )
{
    if (pObj == NULL) return E_INVALIDARG;
    if (pObj->pimgPlate != NULL)
    {
        pObj->pimgPlate->Release();
        pObj->pimgPlate = NULL;
    }
    if (pObj->pimgBestSnapShot != NULL)
    {
        pObj->pimgBestSnapShot->Release();
        pObj->pimgBestSnapShot = NULL;
    }
    if (pObj->pimgLastSnapShot != NULL)
    {
        pObj->pimgLastSnapShot->Release();
        pObj->pimgLastSnapShot = NULL;
    }
    if (pObj->pimgBeginCapture != NULL)
    {
        pObj->pimgBeginCapture->Release();
        pObj->pimgBeginCapture = NULL;
    }
    if (pObj->pimgBestCapture != NULL)
    {
        pObj->pimgBestCapture->Release();
        pObj->pimgBestCapture = NULL;
    }
    if (pObj->pimgLastCapture != NULL)
    {
        pObj->pimgLastCapture->Release();
        pObj->pimgLastCapture = NULL;
    }
    if (pObj->pimgPlateBin != NULL)
    {
        pObj->pimgPlateBin->Release();
        pObj->pimgPlateBin = NULL;
    }
    return S_OK;
}

HRESULT CopyCarLeftInfoStruct( CARLEFT_INFO_STRUCT *pDest, const CARLEFT_INFO_STRUCT *pSrc)
{
    if (pDest == NULL || pSrc == NULL) return E_INVALIDARG;

    HV_memcpy(pDest, pSrc, sizeof(CARLEFT_INFO_STRUCT));
    CopyResultImageStruct(&pDest->cCoreResult.cResultImg, &pSrc->cCoreResult.cResultImg);

    return S_OK;
}

HRESULT FreeCarLeftInfoStruct( CARLEFT_INFO_STRUCT *pObj )
{
    if (pObj == NULL) return E_INVALIDARG;

    FreeResultImageStruct(&pObj->cCoreResult.cResultImg);

    return S_OK;
}

//-----------
// 外总控线程
//-----------

HRESULT CProcessQueueProxy::Run( void* pvParam )
{
    COuterControlImpl* pOuter = (COuterControlImpl*)pvParam;
    if ( pOuter == NULL )
    {
        return E_INVALIDARG;
    }
    return pOuter->ProcessQueue();
}

HRESULT CProcessIPTProxy::Run( void* pvParam )
{
    COuterControlImpl* pOuter = (COuterControlImpl*)pvParam;
    if ( pOuter == NULL )
    {
        return E_INVALIDARG;
    }
    HV_Trace(5, "ProcessIPTSignal.....\n");
#if defined(SINGLE_BOARD_PLATFORM) || !defined(IPT_IN_MASTER)
    pOuter->ProcessIPTSignal();
#endif
    return S_OK;
}

HRESULT CProcessOneFrameProxy::Run( void* pvParam )
{
    COuterControlImpl* pOuter = (COuterControlImpl*)pvParam;
    if ( pOuter == NULL )
    {
        return E_INVALIDARG;
    }
    return pOuter->ProcessOneFrameProxy();
}

//------------
//外总控类函数
//------------

COuterControlImpl::COuterControlImpl(const SignalMatchParam& cSignalMatchParam)
        : m_iDeviceParamLen(0)
        , m_bIOLevel(0)
        , m_iLightCount(0)
        , m_iRedLightCount(0)
        , m_fInitialized(false)
        , m_fRunFlag(true)
        , m_pImage(NULL)
        , m_fIsProcessImage(true)
        , m_fForcePlate(false)
        , m_fBeginIPT(false)
        , m_nLastLightType(DAY)
        , m_iCapCommandTime(300)
        , m_fUsedFlashLight(FALSE)
        , m_iCplStatus(0)
        , m_iPulseLevel(0)
        , m_pVideoRecoger(NULL)
        , m_dwCaptureImageCount(0)
        , m_dwCarLeftCount(0)
        , m_fSwitchToVideoDetect(FALSE)
{
    if (!m_fInitialized)
    {
        g_fOuterOK = false;									// 外总控未工作

        m_nMainSigTypeNum = 0;									// 主信号类型个数
        m_prgMainSigType = 0;									// 保存主信号类型的数组
        m_nMainSourceNum = 0;									// 主信号源数量
        m_prgMainSource = NULL;									// 保存主信号的数组

        m_pComm = NULL;
        m_pIPTControl = NULL;
        m_pIPTDevice = NULL;

        // 设置IPT口配置信息
        for (int i = 0; i < MAX_IPT_PORT_NUM; i++)
        {
            m_rgIPTInfo[i].fUse = false;
            m_rgIPTInfo[i].iComSource = -1;
            m_rgIPTInfo[i].iOutSigSource = -1;
            m_rgIPTInfo[i].iOutSigLevel = 0;
            m_rgIPTInfo[i].iOutSigWidth = 500;
            for (int j = 0; j < MAX_IPT_PIN_NUM; j++)
            {
                m_rgIPTInfo[i].rgSigSource[j] = -1;
            }
        }
    }

    m_dwOuterThreadIsOkTime = GetSystemTick();
    m_dwIPTThreadIsOkTime = GetSystemTick();
    m_dwFramThreadIsOkTime = GetSystemTick();

    for (int i = 0; i < (int)(ARRSIZE(m_rgdwCarArriveTime)); i++)
    {
        m_rgdwCarArriveTime[i] = 0;
    }

    m_cSignalMatchParam = cSignalMatchParam;
}

COuterControlImpl::~COuterControlImpl()
{
    Clear();
}

void COuterControlImpl::Clear()
{
    g_fOuterOK = false;  // 外总控停止
    Stop();

    // 清空车牌队列和信号队列
    ClearAllQueue();

    SAFE_DELETE_OBJ(m_pIPTDevice);
    SAFE_DELETE_OBJ(m_pIPTControl);
    SAFE_DELETE_OBJ(m_pComm);

    // 释放动态匹配信号类型数组
    if (g_prgMatchSigType != NULL)
    {
        delete []g_prgMatchSigType;
        g_prgMatchSigType = NULL;
    }

    // 释放动态信号源数组
    if (g_prgSigSource != NULL)
    {
        delete []g_prgSigSource;
        g_prgSigSource = NULL;
    }

    // 释放动态信号队列数组
    if (g_prgQueSignal != NULL)
    {
        delete []g_prgQueSignal;
        g_prgQueSignal = NULL;
    }

    // 删除保存主信号类型的数组
    if (m_prgMainSigType != NULL)
    {
        delete []m_prgMainSigType;
        m_prgMainSigType = NULL;
    }

    // 删除保存主信号信号源的数组
    if (m_prgMainSource != NULL)
    {
        delete []m_prgMainSource;
        m_prgMainSource = NULL;
    }

    m_iLightCount = 0;
    m_iRedLightCount = 0;
}

void COuterControlImpl::ClearAllQueue()
{
    // 清空车牌队列
    while (g_quePlate.GetSize() > 0)
    {
        PLATE_INFO_hxl *pPlateInfo;

        LockPlate();
        pPlateInfo = g_quePlate.RemoveHead();
        UnlockPlate();
        // 结果结构内存
        ReleasePlateInfo(pPlateInfo);
    }

    // 清空匹配结果队列
    while (m_queCarLeft.GetSize() > 0)
    {
        PLATE_INFO_hxl *pCarLeftInfo;

        LockCore();
        pCarLeftInfo = m_queCarLeft.RemoveHead();
        UnlockCore();
        // 结果结构内存
        ReleasePlateInfo(pCarLeftInfo);
    }

    // 清空信号队列
    ClearSignalQueue();
}

HRESULT COuterControlImpl::ClearSignalQueue()
{
    for (int i = 0; i < g_nSignalSourceNum; i++)
    {
        LockSignal(i);
        while (g_prgQueSignal[i].GetSize() > 0)
        {
            SIGNAL_INFO *pTempSignalInfo;
            pTempSignalInfo = g_prgQueSignal[i].RemoveHead();

            // 结果结构内存
            ReleaseSignalInfo(pTempSignalInfo);
            g_nTestCount--;
        }
        UnlockSignal(i);
    }
    return S_OK;
}

// 运行程序Run，启动外总控队列处理线程
HRESULT COuterControlImpl::Run()
{
#if defined(SINGLE_BOARD_PLATFORM) || !defined(IPT_IN_MASTER)
    m_fBeginIPT = false;  // IPT线程暂时不运行，等待外总控线程完成运行
#endif
    if (m_emOutPutPlateMode == PLATE_OUTPUT_SIGNAL)
    {
        m_cProcessQueueProxy.Start(this);

        if (GetCurrentMode() != PRM_HVC)  //HVC不需要此线程
        {
            m_cProcessOneFramProxy.Start(this);
        }
    }
    m_cProcessIPTProxy.Start(this);
    return S_OK;
}

// 停止队列处理线程
HRESULT COuterControlImpl::Stop()
{
    m_fRunFlag = false;			// 设置线程停止标志
    HV_Sleep(1000);

    if (m_emOutPutPlateMode == PLATE_OUTPUT_SIGNAL)
    {
        m_cProcessQueueProxy.Stop();
        m_cProcessOneFramProxy.Stop();
    }
    m_cProcessIPTProxy.Stop();

    return S_OK;
}

HRESULT COuterControlImpl::OutputSignal(OUTPUT_SIGNAL output_signal)
{
    g_queOutSignal.AddTail(output_signal);
    return S_OK;
}

//车辆到达，返回S_FALSE表示忽略此次CarArrive
HRESULT STDMETHODCALLTYPE COuterControlImpl::CarArrive(
    CARARRIVE_INFO_STRUCT *pCarArriveInfo,
    LPVOID pvUserData
)
{
    static DWORD32 dwArriveTimes = 0;
    if (pCarArriveInfo == NULL) return E_POINTER;

//   if (pCarArriveInfo->dwCarArriveTime - m_rgdwCarArriveTime[pCarArriveInfo->iRoadNumber + 1] < m_iCapCommandTime)
    //  {
    //      return S_FALSE;
//   }

    //  m_rgdwCarArriveTime[pCarArriveInfo->iRoadNumber + 1] = pCarArriveInfo->dwCarArriveTime;

    // zhaopy
    pCarArriveInfo->dwCarArriveTime = pCarArriveInfo->dwCarArriveRealTime;
#if defined(SINGLE_BOARD_PLATFORM) || !defined(IPT_IN_MASTER)
    if (m_fBeginIPT)
    {
        OUTPUT_SIGNAL output_signal;
        output_signal.iRoadNumber = pCarArriveInfo->iRoadNumber;
        output_signal.dwTime = GetSystemTick();
        output_signal.dwDelay = pCarArriveInfo->dwTriggerOutDelay;
        OutputSignal(output_signal);
    }
#endif
    if (m_fUsedFlashLight)
    {
        return S_OK;
    }

    dwArriveTimes++;
    if (dwArriveTimes >= 0xffff)
    {
        dwArriveTimes = 1;
    }

    DWORD32 dwCurTime = GetSystemTick();
    if (dwCurTime >= m_dwLastCarArriveTime )
    {
        LockCore();
        if (!m_fHardTriggerCap)
        {
            int iRoadNumber = pCarArriveInfo->iRoadNumber;
            pCarArriveInfo->iRoadNumber = dwArriveTimes;
            pCarArriveInfo->iRoadNumber = (pCarArriveInfo->iRoadNumber << 16) | iRoadNumber;
        }
        UnlockCore();

        for (int i = 0; i < CAPROAD_LIST_COUNT; i++)
        {
            if (g_rgmatchCapRoad[i].dwCarArriveTime == 0)
            {
                g_rgmatchCapRoad[i].dwCarArriveTime = (pCarArriveInfo->iRoadNumber & 0xffff0000) >> 16;
                g_rgmatchCapRoad[i].dwOutTime = GetSystemTick() + 3000;
                g_rgmatchCapRoad[i].iRoadNum = pCarArriveInfo->iRoadNumber & 0x0000ffff;
                break;
            }
        }

        for (int i = 0; i < CAPROAD_LIST_COUNT; i++)
        {
            if (g_rgmatchCapRoad[i].dwOutTime < GetSystemTick() && g_rgmatchCapRoad[i].dwOutTime > 0)
            {
                g_rgmatchCapRoad[i].dwCarArriveTime = 0;
                g_rgmatchCapRoad[i].dwOutTime = 0;
                g_rgmatchCapRoad[i].iRoadNum = -1;
            }
        }
    }
    else
    {
        if (dwCurTime < m_dwLastCarArriveTime && !m_fHardTriggerCap)
        {
            if (dwArriveTimes == 1)
            {
                g_dwCarArriveTimes = 0xfffe;
            }
            else
            {
                g_dwCarArriveTimes = dwArriveTimes - 1;
            }
            g_dwRoadFlag = pCarArriveInfo->iRoadNumber;
        }
    }
    if (!m_fHardTriggerCap)
    {
        m_dwLastCarArriveTime = GetSystemTick() + 160;
    }

    //HV_Trace(5, "*I*==%d\n", pCarArriveInfo->dwCarArriveTime);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE COuterControlImpl::CarLeft(
    CARLEFT_INFO_STRUCT *pCarLeftInfo,
    LPVOID pvUserData
)
{
    if (!m_fRunFlag)
    {
        return S_OK;
    }

    m_nLastLightType = (LIGHT_TYPE)pCarLeftInfo->cCoreResult.nPlateLightType;
    m_iCplStatus = pCarLeftInfo->cCoreResult.iCplStatus;
    m_iPulseLevel = pCarLeftInfo->cCoreResult.iPulseLevel;

    CARLEFT_INFO_STRUCT *pTempCarlefInfo = new CARLEFT_INFO_STRUCT();
    if (pTempCarlefInfo == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // 复制用户数据
    LPVOID pvTempUserData(NULL);
    if (pvUserData != NULL)
    {
        pvTempUserData = new char[256];
        if (NULL == pvTempUserData)
        {
            delete pTempCarlefInfo;
            return E_OUTOFMEMORY;
        }
        strncpy((char *)pvTempUserData, (char *)pvUserData, 256);
    }

    PLATE_INFO_hxl *pTempPlateInfo = new PLATE_INFO_hxl();
    if (pTempPlateInfo == NULL)
    {
        if (pTempCarlefInfo != NULL)
        {
            delete pTempCarlefInfo;
        }
        if (pvTempUserData != NULL)
        {
            delete[] (PBYTE8)pvTempUserData;
        }
        return E_OUTOFMEMORY;
    }

    // 复制结构数据，复制的时候，图片已经加了引用计数
    CopyCarLeftInfoStruct(pTempCarlefInfo, pCarLeftInfo);

    pTempPlateInfo->pCarleftInfo = pTempCarlefInfo;
    pTempPlateInfo->pvUserData = pvTempUserData;

    if (m_fSpeedDecide)
    {
        // 速度匹配
        pTempPlateInfo->dwValue = (DWORD32)pTempCarlefInfo->cCoreResult.fltCarspeed;  // 取得软件测速时间
    }
    else
    {
        pTempPlateInfo->dwValue = 0;
    }

    // 设置匹配信息
    pTempPlateInfo->nStatus = STATUS_INIT;

    // TODO 出牌时间,采用CarArrive的时间.
    pTempPlateInfo->dwPlateTime = pTempCarlefInfo->cCoreResult.nCarArriveTime;
    pTempPlateInfo->dwInputTime = GetSystemTick();

    // 将车牌数据加入车牌队列,如果满则丢弃
    LockPlate();
    if ( g_quePlate.GetSize() >= PLATE_LIST_COUNT )
    {
        ReleasePlateInfo(pTempPlateInfo);
        HV_Trace(1, "Plate Full..\n");
    }
    else
    {
        g_quePlate.AddTail(pTempPlateInfo);
        HV_Trace(5, "Plate: %d, %d, %d\n"
                 ,pTempPlateInfo->pCarleftInfo->cCoreResult.dwTriggerIndex
                 ,pTempPlateInfo->dwPlateTime
                 ,pTempPlateInfo->dwInputTime
                );

        char szTmp[255];
        sprintf(szTmp, "Plate: %d, %d, %d\n"
                ,pTempPlateInfo->pCarleftInfo->cCoreResult.dwTriggerIndex
                ,pTempPlateInfo->dwPlateTime
                ,pTempPlateInfo->dwInputTime
               );
        PciSendSlaveDebugInfo(szTmp);
    }
    UnlockPlate();

    return S_OK;
}

// 初始化外总控
HRESULT STDMETHODCALLTYPE COuterControlImpl::InitOuterControler()
{
    if (m_fInitialized)
    {
        Clear();							                    // 清除数据

        m_fRunFlag = true;
        m_fBeginIPT = false;
        m_fInitialized = false;

        g_fOuterOK = false;									// 外总控未工作

        m_nMainSigTypeNum = 0;									// 主信号类型个数
        m_prgMainSigType = 0;									// 保存主信号类型的数组
        m_nMainSourceNum = 0;									// 主信号源数量
        m_prgMainSource = NULL;									// 保存主信号氖?

        m_pComm = NULL;
        m_pIPTControl = NULL;
        m_pIPTDevice = NULL;

        // 设置IPT口配置信息
        for (int i = 0; i < MAX_IPT_PORT_NUM; i++)
        {
            m_rgIPTInfo[i].fUse = false;
            m_rgIPTInfo[i].iComSource = -1;
            m_rgIPTInfo[i].iOutSigSource = -1;
            m_rgIPTInfo[i].iOutSigLevel = 0;
            m_rgIPTInfo[i].iOutSigWidth = 500;
            for (int j = 0; j < MAX_IPT_PIN_NUM; j++)
            {
                m_rgIPTInfo[i].rgSigSource[j] = -1;
            }
        }

    }

    for (int i = 0; i < CAPROAD_LIST_COUNT; i++)
    {
        g_rgmatchCapRoad[i].dwCarArriveTime = 0;
        g_rgmatchCapRoad[i].dwOutTime = 0;
        g_rgmatchCapRoad[i].iRoadNum = -1;
    }
    // 初始化指针
    g_prgMatchSigType = NULL;							// 匹配信号类型数组指针
    g_prgSigSource = NULL;								// 信号源数组指针
    g_prgQueSignal = NULL;								// 信号队列数组指针

    // 缺省的外总控配置
    m_fSpeedDecide = true;								// 默认使用软件测速校正(暂时不开放配置)

    g_dwPlateHoldTime = PLATE_HOLD_TIME;				// 车牌保持时间
    g_dwSignalHoldTime = SIGNAL_HOLD_TIME;				// 信号保持时间

    m_emOutPutPlateMode = PLATE_OUTPUT_AUTO;			// 自动出牌作为缺省的出牌模式
    m_emTrackType = WORK_MODE_SINGLE;					// 缺省使用单车道方式(不考虑车牌位置信息)

    g_nSignalTypeNum = 0;								// 信号类型总数
    g_nSignalSourceNum = 0;								// 信号源总数

    m_iCapCommandTime = 300; 							//抓拍图的时间间隔
    m_fHardTriggerCap = 0;								//硬触发抓拍开关

    g_dwCarArriveTimes = 0;								//Car Arrive 次数
    g_dwRoadFlag = 0;									//相隔时间很短不发抓拍命令时,记录下车道号
    m_dwLastCarArriveTime = 0;

    int iMainSignalType = -1;

    m_iStartRoadNum = 0;
    m_iRoadNumberBegin = 0;

    //获取全局的外总控配置
    g_dwPlateHoldTime = m_cSignalMatchParam.dwPlateHoldTime;
    g_dwSignalHoldTime = m_cSignalMatchParam.dwSignalHoldTime;
    m_emOutPutPlateMode = (OUTPUT_PLATE_MODE)m_cSignalMatchParam.emOutPutPlateMode;
    m_iCapCommandTime = m_cSignalMatchParam.iCapCommandTime;
    m_fHardTriggerCap = m_cSignalMatchParam.fHardTriggerCap;

    // zhaopy
    g_iCapImgCount = m_cSignalMatchParam.iCapImgCount;

    m_iRoadNumberBegin = m_cSignalMatchParam.iRoadNumberBegin;
    m_iStartRoadNum = m_cSignalMatchParam.iStartRoadNum;
    m_fForcePlate = m_cSignalMatchParam.fForcePlate;
    g_nSignalTypeNum = m_cSignalMatchParam.nSignalTypeNum;
    g_nSignalSourceNum = m_cSignalMatchParam.nSignalSourceNum;
    iMainSignalType = m_cSignalMatchParam.iMainSignalType;
    m_fUsedFlashLight = m_cSignalMatchParam.fEnableFlashLight;
    m_fSpeedDecide = m_cSignalMatchParam.fSpeedDecide;

    g_nSignalTypeNum = (g_nSignalTypeNum < 0) ? 0 : g_nSignalTypeNum;
    g_nSignalTypeNum = (g_nSignalTypeNum > MAX_SIGNAL_TYPE) ? MAX_SIGNAL_TYPE : g_nSignalTypeNum;
    g_nSignalSourceNum = (g_nSignalSourceNum < 0) ? 0 : g_nSignalSourceNum;
    g_nSignalSourceNum = (g_nSignalSourceNum > MAX_SIGNAL_SOURCE) ? MAX_SIGNAL_SOURCE : g_nSignalSourceNum;

    if (g_nSignalTypeNum == 0)
    {
        g_nSignalSourceNum = 0;
        m_emOutPutPlateMode = PLATE_OUTPUT_AUTO;
    }

    // 动态申请匹配信号类型数组
    if (g_nSignalTypeNum > 0)
    {
        g_prgMatchSigType = new SIGNAL_TYPE[g_nSignalTypeNum];
        if (g_nSignalTypeNum == 0)
        {
            return E_OUTOFMEMORY;			// 内存分配失败
        }
    }
    // 动态申请信号源数组
    if (g_nSignalSourceNum > 0)
    {
        g_prgSigSource = new SIGNAL_SOURCE[g_nSignalSourceNum];
        if (g_prgSigSource == NULL)
        {
            return E_OUTOFMEMORY;
        }

        // 为每一个信号源创建信号队列
        g_prgQueSignal = new CHvList<SIGNAL_INFO*>[g_nSignalSourceNum];
        if (g_prgQueSignal == NULL)
        {
            return E_OUTOFMEMORY;
        }
    }
    else
    {
        m_emOutPutPlateMode = PLATE_OUTPUT_AUTO;
    }

    // 初始化创建的匹配信号类型数组
    for (int i = 0; i < g_nSignalTypeNum; i++)
    {
        g_prgMatchSigType[i].atType = ATTACH_INFO_NOTHING;			// 不附加任何信号
        g_prgMatchSigType[i].fMainSignal = false;					// 非主信号
        sprintf(g_prgMatchSigType[i].strName, "匹配信号类型%02d", i);
    }

    // 初始化创建的信号源数组
    for (int i = 0; i < g_nSignalSourceNum; i++)
    {
        g_prgSigSource[i].nType = 0;								// 对应匹配信号类型0
        g_prgSigSource[i].dwPrevTime = g_dwPlateHoldTime;
        g_prgSigSource[i].dwPostTime = g_dwSignalHoldTime;
        g_prgSigSource[i].iRoad = -1;
    }

    // 读取二级外总控参数
    m_nMainSigTypeNum = 0;											// 主信号的类型数量
    m_nMainSourceNum = 0;											// 主信号的信号源数量

    //获取匹配信号类型信息
    for (int i = 0; i < g_nSignalTypeNum; i++)
    {
        g_prgMatchSigType[i].atType = m_cSignalMatchParam.rgMatchSigType[i].atType;
        g_prgMatchSigType[i].fMainSignal = (i == iMainSignalType ? true : false);
        strcpy(g_prgMatchSigType[i].strName, m_cSignalMatchParam.rgMatchSigType[i].strName);
    }
    g_fSignalIsMain = m_cSignalMatchParam.fSignalIsMain;
    m_nMainSigTypeNum = m_cSignalMatchParam.nMainSigTypeNum;

    //获取信号源信息
    for (int i = 0; i < g_nSignalSourceNum; i++)
    {
        g_prgSigSource[i].nType = m_cSignalMatchParam.rgSigSource[i].nType;
        g_prgSigSource[i].dwPrevTime = m_cSignalMatchParam.rgSigSource[i].dwPrevTime;
        g_prgSigSource[i].dwPostTime = m_cSignalMatchParam.rgSigSource[i].dwPostTime;
        g_prgSigSource[i].iRoad = m_cSignalMatchParam.rgSigSource[i].iRoad;
        m_nMainSourceNum = m_cSignalMatchParam.nMainSourceNum;
    }

    // 申请保存主信号类型的数组
    if (m_nMainSigTypeNum > 0)
    {
        m_prgMainSigType = new int[m_nMainSigTypeNum];
        if (m_prgMainSigType == NULL)
        {
            return E_OUTOFMEMORY;
        }

        // 记录所有为主信号的信号类型
        int nTypeCount(0);
        for (int i = 0; i < g_nSignalTypeNum; i++)
        {
            if (g_prgMatchSigType[i].fMainSignal)
            {
                m_prgMainSigType[nTypeCount++] = i;
            }
        }
    }

    if (m_nMainSourceNum > 0)
    {
        //申请保存主信号信号源的数组
        m_prgMainSource = new int[m_nMainSourceNum];
        if (m_nMainSourceNum == 0)
        {
            return E_OUTOFMEMORY;
        }

        // 记录所有为主信号的信号源
        int nSourceCount(0);
        for (int i = 0; i < g_nSignalSourceNum; i++)
        {
            if (g_prgMatchSigType[g_prgSigSource[i].nType].fMainSignal)
            {
                m_prgMainSource[nSourceCount++] = i;
            }
        }
    }

    // 设置图片的尺寸
    SetFrameProperty(2048, 768);

    // 初始化IPT
    strcpy(m_szComStr, "COM1");  // IPT使用的串口
    for (int i = 0; i < MAX_IPT_PORT_NUM; i++)
    {
        m_rgIPTInfo[i].fUse = false;
        m_rgIPTInfo[i].iComSource = -1;
        m_rgIPTInfo[i].iOutSigSource = -1;
        m_rgIPTInfo[i].iOutSigLevel = 0;
        m_rgIPTInfo[i].iOutSigWidth = 500;
        for (int j = 0; j < MAX_IPT_PIN_NUM; j++)
        {
            m_rgIPTInfo[i].rgSigSource[j] = -1;
        }
    }

//todo
//        // 读取IPT配置参数
//        if (m_pHvParam)
//        {
//            char szSection[200];
//            char szAreaNames[200];
//
//            // 读取IPT串口编号
//            m_pHvParam->GetString(
//                "IPTCtrl", "IPTComStr",
//                m_szComStr, 200,
//                "IPT串口","",3
//            );   // 读取IPT 串口编号
//
//            // 读取4个ipt端口信息
//            for (int i = 0; i < MAX_IPT_PORT_NUM; i++)
//            {
//                sprintf(szSection, "IPTCtrl\\Port%02d", i);
//
//                m_pHvParam->GetInt(
//                    szSection, "ComSignalSource",
//                    &m_rgIPTInfo[i].iComSource, m_rgIPTInfo[i].iComSource,
//                    -1,g_nSignalSourceNum - 1,
//                    "信号源","",5
//                );
//
//                for (int j = 0; j < MAX_IPT_PIN_NUM; j++)
//                {
//                    sprintf(szAreaNames, "SignalSource%01d", j);
//                    m_pHvParam->GetInt(
//                        szSection, szAreaNames,
//                        &m_rgIPTInfo[i].rgSigSource[j], m_rgIPTInfo[i].rgSigSource[j],
//                        -1,g_nSignalSourceNum - 1,
//                        "信号源","",5
//                    );
//                }
//
//                m_pHvParam->GetInt(
//                    szSection, "OutSignalSource",
//                    &m_rgIPTInfo[i].iOutSigSource, m_rgIPTInfo[i].iOutSigSource,
//                    0, g_nSignalSourceNum - 1,
//                    "输出电平信号对应的信号源", "", 5
//                );
//                m_pHvParam->GetInt(szSection, "OutSignalLevel",
//                                   &m_rgIPTInfo[i].iOutSigLevel, m_rgIPTInfo[i].iOutSigLevel,
//                                   0, 1,
//                                   "输出电平", "", 5
//                                  );
//                m_pHvParam->GetInt(szSection, "OutSignalWidth",
//                                   &m_rgIPTInfo[i].iOutSigWidth, m_rgIPTInfo[i].iOutSigWidth,
//                                   0, 5000,
//                                   "输出脉宽", "", 5);
//
//                m_rgIPTInfo[i].fUse =
//                    (m_rgIPTInfo[i].iComSource != -1 || m_rgIPTInfo[i].rgSigSource[0] != -1 || m_rgIPTInfo[i].rgSigSource[1] != -1 || m_rgIPTInfo[i].iOutSigSource != -1) ? true : false;
//            }
//        }

    //获取IPT配置参数
    strcpy(m_szComStr, m_cSignalMatchParam.szComStr);
    if (m_cSignalMatchParam.nDeviceLen <= (int)sizeof(m_rgbDeviceParam))
    {
        memcpy(m_rgbDeviceParam, m_cSignalMatchParam.bDeviceParam, m_cSignalMatchParam.nDeviceLen);
        m_iDeviceParamLen = m_cSignalMatchParam.nDeviceLen;
    }
    else
    {
        memset(m_rgbDeviceParam, 0, sizeof(m_rgbDeviceParam));
        m_iDeviceParamLen = 0;
    }
    for (int i = 0; i < MAX_IPT_PORT_NUM; i++)
    {
        m_rgIPTInfo[i].iComSource = m_cSignalMatchParam.rgIPTInfo[i].iComSource;
        for (int j = 0; j < MAX_IPT_PIN_NUM; j++)
        {
            m_rgIPTInfo[i].rgSigSource[j] = m_cSignalMatchParam.rgIPTInfo[i].rgSigSource[j];
        }

        m_rgIPTInfo[i].iOutSigSource = m_cSignalMatchParam.rgIPTInfo[i].iOutSigSource;
        m_rgIPTInfo[i].iOutSigLevel = m_cSignalMatchParam.rgIPTInfo[i].iOutSigLevel;
        m_rgIPTInfo[i].iOutSigWidth = m_cSignalMatchParam.rgIPTInfo[i].iOutSigWidth;
        m_rgIPTInfo[i].fUse = m_cSignalMatchParam.rgIPTInfo[i].fUse;
    }

    CreateSemaphore(&g_SemPlate, 1, 1);
    CreateSemaphore(&g_SemHvCore, 1, 1);
    CreateSemaphore(&g_SemImage, 1, 1);
    CreateSemaphore(&g_SemPanormicSignal, 1, 1);

    for ( int i = 0; i < MAX_SIGNAL_SOURCE; ++i )
    {
        CreateSemaphore(&g_rgSemSignal[i], 1, 1);
    }

#if defined(SINGLE_BOARD_PLATFORM) || !defined(IPT_IN_MASTER)
    // 创建IPT设备对象
    if (m_pComm == NULL)
    {
        if (!strstr(m_szComStr, "/dev/ttyS"))
        {
            strcpy(m_szComStr, "/dev/ttyS0");
            g_cParamStore.SetString("IPTCtrl", "IPTComStr", m_szComStr);
        }
        RTN_HR_IF_FAILED(CreateIComm(&m_pComm, m_szComStr));
    }
    if (m_pIPTControl == NULL)
    {
        RTN_HR_IF_FAILED(CreateIIPTControl(&m_pIPTControl, m_pComm));
    }
    if (m_pIPTDevice == NULL)
    {
        RTN_HR_IF_FAILED(CreateIPTDevice(&m_pIPTDevice, m_pIPTControl));
    }
#endif

    // 开始运行
    Run();

    m_fInitialized = true;
    g_fOuterOK = true;  // 外总控已正常工作,可以接收信号

    return S_OK;
}

HRESULT COuterControlImpl::ProcessQueue()
{
#if defined(SINGLE_BOARD_PLATFORM) || !defined(IPT_IN_MASTER)
    m_fBeginIPT = true;					                // 通知IPT线程开始运行
#endif
    while (m_fRunFlag)
    {
        m_dwOuterThreadIsOkTime = GetSystemTick();		// 线程运行正常时间

        int nPlateNum = g_quePlate.GetSize();			// 车牌数量
        int nSignalNum = GetSignalNum();				// 信号数量
        if ((nPlateNum == 0) && (nSignalNum == 0))
        {
            HV_Sleep(100);
            continue;
        }

        switch (m_emOutPutPlateMode)
        {
        case PLATE_OUTPUT_SIGNAL:		// 出牌必须考虑信号
            if (PreProcessQueue())
            {
                // 需要匹配（有匹配信号或者车牌、信号超期）
                if (Match() > 0)
                {
                    // 输出匹配车牌(有牌车和无牌车)
                    OutputMatchPlate();
                }
            }
            // 释放所有匹配和过期的车牌和信号
            ReleaseOldElement();
            break;
        case PLATE_OUTPUT_AUTO:						// 自动出牌模式，不考虑信号
        default:
            if (nPlateNum > 0)
            {
                // 有数据，才等待队列
                PLATE_INFO_hxl *pTempPlateInfo;

                LockPlate();
                pTempPlateInfo = g_quePlate.RemoveHead();
                UnlockPlate();

                pTempPlateInfo->pCarleftInfo->cOtherInfo.iRoadNumberBegin = m_iRoadNumberBegin;
                pTempPlateInfo->pCarleftInfo->cOtherInfo.iStartRoadNum = m_iStartRoadNum;

                SendMatchInfo(pTempPlateInfo->pCarleftInfo, pTempPlateInfo->pvUserData);

                // 结果结构内存
                ReleasePlateInfo(pTempPlateInfo);
            }
            if (nSignalNum > 0)
            {
                // 清除信号
                ClearSignalQueue();
            }
            break;
        }
        HV_Sleep(100);			// 每一循环休眠100毫秒
    } //while (m_fRunFlag)

    // 清空所有队列
    ClearAllQueue();
    return S_OK;
}

// 判断信号和车牌是否匹配
// sdwTimeDiff:时间差异。。0表示完全匹配， 负数表示信号在车牌前， 正数表示信号在车牌后
// dwPosDiff:车牌位置差异
bool COuterControlImpl::IsMatchPlate(PLATE_INFO_hxl *pPlateInfo, SIGNAL_INFO *pSignalInfo, long &lTimeDiff, DWORD32 &dwPosDiff)
{
    // 匹配车牌和信号
    DWORD32 dwPrevTime = pSignalInfo->dwSignalTime - g_prgSigSource[pSignalInfo->nType].dwPrevTime;
    DWORD32 dwPostTime = pSignalInfo->dwSignalTime + g_prgSigSource[pSignalInfo->nType].dwPostTime;

    if ((pPlateInfo->dwPlateTime < dwPrevTime) || (pPlateInfo->dwPlateTime > dwPostTime))
    {
        return false;		// 时间不匹配
    }
    lTimeDiff = (long)pPlateInfo->dwPlateTime - (long)pSignalInfo->dwSignalTime;		// 计算时间差异

    return true;
}

// 判断两个信号是否匹配
// sdwTimeDiff:时间差异。。0表示完全匹配， 负数表示信号在车牌前， 正数表示信号在车牌后
// dwPosDiff:车牌位置差异
bool COuterControlImpl::IsMatchSignal(SIGNAL_INFO *pSignalInfoMain, SIGNAL_INFO *pSignalInfo, long &lTimeDiff)
{
    // 匹配车牌和信号
    DWORD32 dwPrevTime = pSignalInfoMain->dwSignalTime - g_prgSigSource[pSignalInfoMain->nType].dwPrevTime;
    DWORD32 dwPostTime = pSignalInfoMain->dwSignalTime + g_prgSigSource[pSignalInfoMain->nType].dwPostTime;

    if ((pSignalInfo->dwSignalTime < dwPrevTime) || (pSignalInfo->dwSignalTime > dwPostTime))
    {
        return false;		// 时间不匹配
    }
    // 计算时间差异
    lTimeDiff = (long)pSignalInfo->dwSignalTime - (long)pSignalInfoMain->dwSignalTime;

    return true;
}

bool COuterControlImpl::IsPlateOutTime(PLATE_INFO_hxl *pPlateInfo)
{
    DWORD32 dwTime = GetSystemTick();		// 取当前时间

    if ((dwTime - pPlateInfo->dwInputTime) > g_dwPlateHoldTime)
    {
        // 车牌超期
        return true;
    }
    return false;
}

bool COuterControlImpl::IsSignalOutTime(SIGNAL_INFO *pSignalInfo)
{
    DWORD32 dwTime = GetSystemTick();		// 取当前时间

    if ((dwTime - pSignalInfo->dwInputTime) > g_dwSignalHoldTime)
    {
        // 信号超期
        return true;
    }
    return false;
}

HRESULT COuterControlImpl::SetFrameProperty(
    int iWidth,
    int iHeight
)
{
    m_nWidth = iWidth;
    m_nHeight = iHeight;

    // TODO 计算所有信号源的实际尺寸(配置是百分比)
    for (int i = 0; i < g_nSignalSourceNum; i++)
    {
    }

    return S_OK;
}

// 图形处理接口,为每个主信号缓冲图片,将CarLeft信息放入队列，并处理CarArrive事件
HRESULT STDMETHODCALLTYPE COuterControlImpl::ProcessOneFrame(
    int iVideoID,
    IReferenceComponentImage *pImage,
    IReferenceComponentImage *pCapImage,
    PVOID pvParam,
    PROCESS_EVENT_STRUCT* pProcessEvent
)
{
    if (m_emOutPutPlateMode != PLATE_OUTPUT_SIGNAL)
    {
        return S_OK;
    }

    if (pProcessEvent->dwEventId & EVENT_CARARRIVE)
    {
        //处理CarArrive事件
        int nCarArriveCount = 0;
        for (int i = 0; i < pProcessEvent->iCarArriveInfoCount; i++)
        {
            if (S_OK == CarArrive(&pProcessEvent->rgCarArriveInfo[i], pvParam))
            {
                pProcessEvent->rgCarArriveInfo[nCarArriveCount++] = pProcessEvent->rgCarArriveInfo[i];
            }
        }

        if (nCarArriveCount == 0)
        {
            pProcessEvent->dwEventId &= ~(EVENT_CARARRIVE);
        }
        pProcessEvent->iCarArriveInfoCount = nCarArriveCount;
    }
    int nCarLeft = 0;
    if (pProcessEvent->dwEventId & EVENT_CARLEFT)
    {
        //将CarLeft信息放入待匹配队列
        for (int i = 0; i < pProcessEvent->iCarLeftInfoCount; i++)
        {
            //电子警察模式下如果没有使能IPT，且采用了闪光抓拍又非违章的，直接返回
            if (!m_cSignalMatchParam.fEPolice
                    || m_cSignalMatchParam.fEnableIPT
                    || pProcessEvent->rgCarLeftInfo[i].cCoreResult.dwTriggerTime != 0
                    || pProcessEvent->rgCarLeftInfo[i].cCoreResult.ptType != PT_NORMAL)
            {
                if(!pProcessEvent->rgCarLeftInfo[i].cCoreResult.dwTriggerTime)
                {
                    HV_Trace(5, "trigger time is zero, maybe not match capture image");
                    PciSendSlaveDebugInfo("trigger time is zero, maybe not match capture image");
                }
                m_dwCarLeftCount++;
                if (m_cSignalMatchParam.nSwitchFlashLightCount > 0
                        && !m_fSwitchToVideoDetect
                        && m_dwCarLeftCount > m_dwCaptureImageCount + m_cSignalMatchParam.nSwitchFlashLightCount)
                {
                    m_fSwitchToVideoDetect = TRUE;
                    int nType = 2;
                    g_cHvPciLinkApi.SendData(PCILINK_FLASHLIGHT_TYPE, &nType, sizeof(nType));
                }
                CarLeft(&pProcessEvent->rgCarLeftInfo[i], pvParam);
                FreeResultImageStruct(&pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg);
            }
            else
            {
                char szTmp[255];
                sprintf(szTmp, "ignore the result, fEPolice=%d,fEnableIPT=%d,TriggerTime=%d", m_cSignalMatchParam.fEPolice, m_cSignalMatchParam.fEnableIPT, pProcessEvent->rgCarLeftInfo[i].cCoreResult.dwTriggerTime);
                HV_Trace(5, szTmp);
                PciSendSlaveDebugInfo(szTmp);
                pProcessEvent->rgCarLeftInfo[nCarLeft++] = pProcessEvent->rgCarLeftInfo[i];
            }
        }
    }

    //将已经匹配好的CarLeft传回去
    LockCore();
    int nCarLeftCount = m_queCarLeft.GetSize();
    pProcessEvent->iCarLeftInfoCount = nCarLeft + nCarLeftCount;
    if (pProcessEvent->iCarLeftInfoCount > 0)
    {
        pProcessEvent->dwEventId |= EVENT_CARLEFT;
    }
    else
    {
        pProcessEvent->dwEventId &= ~(EVENT_CARLEFT);
    }

    for (int i = 0; i < nCarLeftCount; i++)
    {
        PLATE_INFO_hxl *pPlateInfo = m_queCarLeft.RemoveHead();
        // 复制结构数据，复制的时候，图片已经加了引用计数
        CopyCarLeftInfoStruct(&pProcessEvent->rgCarLeftInfo[nCarLeft + i], pPlateInfo->pCarleftInfo);
        ReleasePlateInfo(pPlateInfo);
    }
    UnlockCore();

    if ( !m_fIsProcessImage || pImage == NULL )
    {
        return S_OK;
    }

    LockImage();

    if ( m_pImage != NULL )
    {
        m_pImage->Release();
        m_pImage = NULL;
    }

    m_pImage = pImage;

    if ( m_pImage != NULL ) m_pImage->AddRef();

    m_fIsProcessImage = false;
    UnlockImage();

    return S_OK;
}

HRESULT COuterControlImpl::ProcessOneFrameProxy()
{
    HVPOSITION nPos;
    IReferenceComponentImage *pimgTemp = NULL;
    HVPOSITION rgPos[100];						// 临时保留位置信息
    int rgnSrcType[100];						// 信号源类型
    int nPosNum(0);

    while (m_fRunFlag)
    {
        HV_Sleep(10);
        m_dwFramThreadIsOkTime = GetSystemTick();

        if ( !m_fIsProcessImage && m_pImage != NULL )
        {
            LockImage();
            HV_COMPONENT_IMAGE imgFrame;
            m_pImage->GetImage(&imgFrame);
            if ((imgFrame.iWidth != m_nWidth) || (imgFrame.iHeight != m_nHeight))
            {
                SetFrameProperty(imgFrame.iWidth, imgFrame.iHeight);
            }

            if (GetSignalNum() == 0)
            {
                m_pImage->Release();
                m_pImage = NULL;

                m_fIsProcessImage = true;
                UnlockImage();
                continue;
            }

            nPosNum = 0;
            pimgTemp = NULL;
            // 处理所有主信号
            for (int i = 0; i < m_nMainSourceNum; i++)
            {
                if (nPosNum >= 100)
                {
                    break;
                }
                int nSrcType = m_prgMainSource[i];
                LockSignal(nSrcType);
                nPos = g_prgQueSignal[nSrcType].GetHeadPosition();

                // 处理所有信号
                while ((g_prgQueSignal[nSrcType].IsValidPos(nPos)) && (nPosNum < 100))
                {
                    HVPOSITION oldPos = nPos;

                    // 取得信号
                    SIGNAL_INFO *pSignalInfo = g_prgQueSignal[nSrcType].GetNext(nPos);

                    // 主信号并且没有信号还没有挂接图片
                    if (pSignalInfo->nStatus == STATUS_INIT)
                    {
                        rgPos[nPosNum] = oldPos;
                        rgnSrcType[nPosNum] = nSrcType;
                        nPosNum++;
                    }
                }
                UnlockSignal(nSrcType);
            }
            if (nPosNum > 0)
            {
                pimgTemp = m_pImage;

                if (pimgTemp != NULL)
                {
                    for (int i = 0; i < nPosNum; i++)
                    {
                        // 为每个位置保存图象
                        LockSignal(rgnSrcType[i]);
                        if ( g_prgQueSignal[rgnSrcType[i]].IsValidPos(rgPos[i]) )
                        {
                            SIGNAL_INFO *pSignalInfo = g_prgQueSignal[rgnSrcType[i]].GetNext(rgPos[i]);
                            if (pSignalInfo != NULL)
                            {
                                ConvertToJpeg(&m_pImage);
                                pSignalInfo->nStatus = STATUS_ADD_IMAGE;
                                pSignalInfo->pImage = m_pImage;
                                m_pImage->AddRef();
                            }
                        }
                        UnlockSignal(rgnSrcType[i]);
                    }
                }
            }

            m_pImage->Release();
            m_pImage = NULL;

            m_fIsProcessImage = true;
            UnlockImage();
        }
        else
        {
            HV_Sleep(100);
        }
    }

    return S_OK;
}

// 队列预处理
bool COuterControlImpl::PreProcessQueue()
{
    bool fPlateOutTime(false);						// 是否有超期车牌
    bool fMainSigOuttime(false);					// 是否有超期信号
    bool fIsCanMatch(false);						// 是否可以匹配
    HVPOSITION nPos;

    int nPlateNum = g_quePlate.GetSize();			// 车牌数量
    int nSignalNum = GetSignalNum();				// 信号数量

    if (nPlateNum > 0)
    {
        // 先处理信号，看是否有过期的信号
        LockPlate();
        nPos = g_quePlate.GetHeadPosition();
        while (g_quePlate.IsValidPos(nPos))
        {
            // 分析车牌是否过期
            PLATE_INFO_hxl *pPlateInfo = g_quePlate.GetNext(nPos);

            if (IsPlateOutTime(pPlateInfo))
            {
                // 车牌过期
                pPlateInfo->nStatus = STATUS_OUTTIME;
                fPlateOutTime = true;
            }
        }
        UnlockPlate();
    }

    if (nSignalNum > 0)
    {
        bool rgfHasSignalType[MAX_SIGNAL_TYPE];  // 按类型统计信号个数
        for (int i = 0; i < MAX_SIGNAL_TYPE; i++)
        {
            rgfHasSignalType[i] = false;
        }

        // 处理过期的信号
        for (int i = 0; i < g_nSignalSourceNum; i++)
        {
            if (g_prgQueSignal[i].GetSize() > 0)
            {
                rgfHasSignalType[g_prgSigSource[i].nType] = true;  // 有该类型的信号

                LockSignal(i);  // 锁定对应的信号队列
                nPos = g_prgQueSignal[i].GetHeadPosition();
                while (g_prgQueSignal[i].IsValidPos(nPos))
                {
                    // 分析车牌是否过期
                    SIGNAL_INFO *pSignalInfo = g_prgQueSignal[i].GetNext(nPos);

                    if (IsSignalOutTime(pSignalInfo))
                    {
                        // 信号过期
                        pSignalInfo->nStatus = STATUS_OUTTIME;
                        if (g_prgMatchSigType[g_prgSigSource[i].nType].fMainSignal)
                        {
                            // 该信号是主信号
                            {
                                fMainSigOuttime = true;
                            }
                        }
                    }
                }
                UnlockSignal(i);  // 释放对应的信号队列
            }
        } // for

        fIsCanMatch = true;
        for (int i = 0; i < g_nSignalTypeNum; i++)
        {
            if (!rgfHasSignalType[i])
            {
                fIsCanMatch = false;
                break;
            }
        }
    }

    SemPend(&g_SemPanormicSignal);
    if (g_PanormicSignal.GetSize() > 0)
    {
        nPos = g_PanormicSignal.GetHeadPosition();
        while (g_PanormicSignal.IsValidPos(nPos))
        {
            SIGNAL_INFO* pSignalInfo = g_PanormicSignal.GetNext(nPos);
            if (GetSystemTick() - pSignalInfo->dwInputTime > g_dwSignalHoldTime)
            {
                pSignalInfo->nStatus = STATUS_OUTTIME;
            }
        }
    }
    SemPost(&g_SemPanormicSignal);

    // 只要有超期车牌、信号，或者类型足够，就需要进行匹配操作
    return (fPlateOutTime | fMainSigOuttime | fIsCanMatch | 0 != m_cSignalMatchParam.nPlateType);
}

// 按车牌匹配所有信号
bool COuterControlImpl::MatchPlate(PLATE_INFO_hxl *pPlateInfo)
{
    HVPOSITION nPos;
    pPlateInfo->pCarleftInfo->cCoreResult.iMatchCount = 0;

    // 匹配统计结构
    bool rgbSignalType[MAX_SIGNAL_TYPE];  // 按类型统计匹配
    MATCH_ANALYSIS rgMatchInfo[MAX_SIGNAL_TYPE];  // 当前车牌的匹配信息

    // 初始化统计结构
    for (int i = 0; i < g_nSignalTypeNum; i++)
    {
        rgbSignalType[i] = false;
        rgMatchInfo[i].nSignalPos = -1;
        rgMatchInfo[i].nSignalPos_2 = -1;
    }

    // 锁定所有信号队列
    for (int i = 0; i < g_nSignalSourceNum; i++)
    {
        LockSignal(i);
    }
    bool fTriggerMath = false;
    DWORD32 dwTriggerIndex = 0;
#ifdef SINGLE_BOARD_PLATFORM
    dwTriggerIndex = pPlateInfo->pCarleftInfo->cCoreResult.dwTriggerIndex;
#endif
    //绗竴杞尮閰嶏紝鏍规嵁鎶撴媿鏍囧織鍖归厤
    if (dwTriggerIndex > 0)
    {
        for (int i = 0; i < g_nSignalSourceNum; i++)
        {
            int nSrgType = g_prgSigSource[i].nType;
            if (g_prgMatchSigType[nSrgType].atType != ATTACH_INFO_LAST_IMAGE)
            {
                continue;
            }

            const std::list<SIGNAL_INFO*>& listSignal = g_prgQueSignal[i].GetList();
            std::list<SIGNAL_INFO*>::const_iterator iter = listSignal.begin();

            DWORD32 dwSignalTime = 0;
            int iPos = 0;
            while (iter != listSignal.end())
            {
                if (((*iter)->nStatus != STATUS_MATCH) && ((*iter)->dwFlag == dwTriggerIndex))//鍖归厤涓婁簡
                {
                    fTriggerMath = true;
                    rgMatchInfo[nSrgType].nSigSourceType = i;				// 淇″彿婧愮被鍨
                    rgMatchInfo[nSrgType].nSignalPos = iPos;				// 褰撳墠浣嶇疆
                    rgbSignalType[nSrgType] = true;							// 杩欑绫诲瀷宸茬粡鍖归厤鏈溅鐗
                    dwSignalTime = (*iter)->dwSignalTime;

                    // 濡傛灉闇€瑕佸寮犳姄鎷嶅浘鐗
                    if (g_iCapImgCount > 1)
                    {
                        int iPos2 = iPos;
                        std::list<SIGNAL_INFO*>::const_iterator iter2 = iter;
                        ++iPos2;
                        ++iter2;
                        while (iter2 != listSignal.end())
                        {
                            if ((*iter2)->dwFlag == dwTriggerIndex)//鍖归厤涓婁簡
                            {
                                rgMatchInfo[nSrgType].nSignalPos_2 = iPos2;
                                break;
                            }
                            ++iPos2;
                            ++iter2;
                        }
                        if (rgMatchInfo[nSrgType].nSignalPos_2 < 0)
                        {
                            rgbSignalType[nSrgType] = false;
                        }
                    }
                    break;
                }
                ++iPos;
                ++iter;
            }

            if (rgbSignalType[nSrgType])
            {
                int iDiff = dwSignalTime - pPlateInfo->dwPlateTime;
                HV_Trace(5, "Trigger flag:%d match type:%d diff:%d\n", dwTriggerIndex, nSrgType, iDiff);
            }
        }
    }

    // 匹配所有信号源的信号
    for (int i = 0; i < g_nSignalSourceNum; i++)
    {
        int nSrgType = g_prgSigSource[i].nType;
        if (rgbSignalType[nSrgType])
        {
            continue;
        }
        nPos = g_prgQueSignal[i].GetHeadPosition();
        while (g_prgQueSignal[i].IsValidPos(nPos))
        {
            HVPOSITION nCurSigPos = nPos;  // 保存当前位置
            SIGNAL_INFO *pSignalInfo = g_prgQueSignal[i].GetNext(nPos);  // 取得该信号源的信号
            if (g_prgMatchSigType[nSrgType].atType == ATTACH_INFO_LAST_IMAGE)
            {
                if ((dwTriggerIndex > 0)
                        && (pSignalInfo->dwFlag > 0)
                        && (pPlateInfo->nStatus != STATUS_OUTTIME)
                   )
                {
                    continue;
                }
            }
            if (pSignalInfo->nStatus != STATUS_MATCH)
            {
                long lTimeDiff(0);
                DWORD32 dwPosDiff(0);
                int iSignalRoad = g_prgSigSource[pSignalInfo->nType].iRoad;

                int iPlateRoad = pPlateInfo->pCarleftInfo->cCoreResult.nRoadNo;

                //如果信号是有车道信号,则用此车道信息替换设置的车道信息
                if ( pSignalInfo->dwRoad != 0 && (pSignalInfo->dwRoad & 0x00FF0000) == 0x00FF0000 )
                {
                    if ( (BYTE8)pSignalInfo->dwRoad == 0xFF )
                    {
                        iSignalRoad = 0xFF;
                    }
                    else
                    {
                        iSignalRoad = (BYTE8)pSignalInfo->dwRoad;
                    }
                }

                if ( IsMatchPlate(pPlateInfo, pSignalInfo, lTimeDiff, dwPosDiff)
                        && (iSignalRoad == 0xFF || iPlateRoad == 0xFF || iSignalRoad == iPlateRoad) )
                {
                    // 如果信号带有速度，还应当判断速度是否差别不大
                    if ((m_fSpeedDecide)
                            && (g_prgMatchSigType[nSrgType].atType == ATTACH_INFO_SPEED))
                    {
                        // 计算速度差异
                        if ((pPlateInfo->dwValue > 0) && (pSignalInfo->dwValue > 0))
                        {
                            // 使用软件测速来校正
                            int nSlowSpeed = int((1.0 - SPEED_SCORE_PARAM) * pPlateInfo->dwValue);
                            int nFastSpeed = int((1.0 + SPEED_SCORE_PARAM) * pPlateInfo->dwValue);

                            if (((int)pSignalInfo->dwValue >= nFastSpeed) || ((int)pSignalInfo->dwValue <= nSlowSpeed))
                            {
                                // 速度差异太大，则本次匹配失败,放弃
                                HV_Trace(5, "plate speed =%d, radar speed = %d, nSlowSpeed = %d, nFastSpeed = %d, ignore radar speed\n",
                                         pPlateInfo->dwValue, pSignalInfo->dwValue, nSlowSpeed, nFastSpeed);
                                continue;
                            }
                        }
                    }

                    // 车牌和信号已经匹配，则记录匹配结果

                    // 优先信号向前匹配
                    bool fIsUpdate = false;
                    if ( rgbSignalType[nSrgType] && lTimeDiff < 0 && rgMatchInfo[nSrgType].lTimeDiff > 0 && abs(lTimeDiff) < 180)
                    {
                        fIsUpdate = true;
                    }
                    bool fUnUpdate = false;
                    if ( rgbSignalType[nSrgType] && lTimeDiff > 0 && rgMatchInfo[nSrgType].lTimeDiff < 0 && abs(rgMatchInfo[nSrgType].lTimeDiff) < 180)
                    {
                        fUnUpdate = true;
                    }

                    if ((!rgbSignalType[nSrgType])
                            || ( (abs(lTimeDiff) < abs(rgMatchInfo[nSrgType].lTimeDiff)) && (abs(lTimeDiff) > 80) && !fUnUpdate )
                            || fIsUpdate )
                    {
                        HV_Trace(5, "<Outer>Match: S:%d, P:%d I:%d\n"
                                 ,pSignalInfo->dwSignalTime
                                 ,pPlateInfo->dwPlateTime
                                 ,pPlateInfo->pCarleftInfo->cCoreResult.dwTriggerIndex
                                );
                        // 原来未有匹配信息或者当前时间差异比较小
                        rgMatchInfo[nSrgType].nSigSourceType = i;				// 信号源类型
                        rgMatchInfo[nSrgType].nSignalPos = nCurSigPos;			// 当前位置
                        rgMatchInfo[nSrgType].lTimeDiff = lTimeDiff;		// 时间差异
                        rgMatchInfo[nSrgType].dwPosDiff = dwPosDiff;			// 位置差异

                        // 更新类型匹配
                        rgbSignalType[nSrgType] = true;						// 这种类型已经匹配本车牌

                        if ( GetCurrentMode() == PRM_HVC &&
                                iPlateRoad == 0xFF &&
                                iSignalRoad != 0xFF ) //无牌车设置主信号车道
                        {
                            pPlateInfo->pCarleftInfo->cCoreResult.nRoadNo = iSignalRoad;
                        }

                        pPlateInfo->pCarleftInfo->cCoreResult.iMatchCount = 1;

                        //是否需要匹配第2张抓拍图
                        bool fNeedSecond = false;
                        if (g_prgMatchSigType[nSrgType].atType == ATTACH_INFO_LAST_IMAGE)
                        {
                            if (g_iCapImgCount > 1)
                            {
                                fNeedSecond = true;
                            }
                            else if ((pPlateInfo->pCarleftInfo->cCoreResult.iCapCount > 1) && m_cSignalMatchParam.iCapMode)
                            {
                                if (m_cSignalMatchParam.iCapMode == 1)//超速违章
                                {
                                    if ((int)(pPlateInfo->pCarleftInfo->cCoreResult.fltCarspeed + 0.5f) > m_cSignalMatchParam.iSpeedLimit)
                                    {
                                        fNeedSecond = true;
                                    }
                                }
                                else if (m_cSignalMatchParam.iCapMode == 2)
                                {
                                    fNeedSecond = true;
                                }
                            }
                        }

                        if (fNeedSecond)
                        {
                            rgMatchInfo[nSrgType].nSignalPos_2 = -1;
                            if (g_prgQueSignal[i].IsValidPos(nPos))
                            {
                                // 获取下一张抓拍图片
                                HVPOSITION nPosTmp = nPos;
                                SIGNAL_INFO *pSignalInfo_2 = g_prgQueSignal[i].GetNext(nPosTmp);
                                if (pSignalInfo_2->dwSignalTime > pSignalInfo->dwSignalTime)
                                {
                                    rgMatchInfo[nSrgType].nSignalPos_2 = nPos;
                                    pPlateInfo->pCarleftInfo->cCoreResult.iMatchCount = 2;
                                }
                            }

                            if (rgMatchInfo[nSrgType].nSignalPos_2 == -1)
                            {
                                if (pPlateInfo->nStatus != STATUS_OUTTIME)
                                {
                                    rgbSignalType[nSrgType] = false;
                                }
                            }
                        }
                    }
                } // if match
            } // if (pSignalInfo->nStatus != STATUS_MATCH)
        } // while(g_prgQueSignal[i].IsValidPos(nPos))
    } // for

    // 判断车牌是否匹配
    bool fSuccessMatch(true);
    for (int i = 0; i < g_nSignalTypeNum; i++)
    {
        //仅双层黄牌才进行匹配
        if ((g_prgMatchSigType[g_prgSigSource[i].nType].atType == ATTACH_INFO_LAST_IMAGE) //图片信号
                && (m_cSignalMatchParam.nPlateType == 1)
                && (pPlateInfo->pCarleftInfo->cCoreResult.nColor != PC_YELLOW)
           )
        {
            rgbSignalType[i] = false;
            continue;
        }
        if (!rgbSignalType[i])
        {
            // 该匹配类型没有匹配，则判断车牌是否超期了
            if ((pPlateInfo->nStatus != STATUS_OUTTIME)
                    || (g_prgMatchSigType[i].fMainSignal))
            {
                // 没有超期，本次匹配失败，继续等待信号
                // 没有匹配主信号，则匹配失败，车牌将删除
                fSuccessMatch = false;
                break;
            }

        }
    }

    //如果是车牌超时而且是强制出结果的模式
    if ( (pPlateInfo->nStatus == STATUS_OUTTIME) && m_fForcePlate )
    {
        fSuccessMatch = true;
    }

    IReferenceComponentImage* pPanormicImage = NULL;
    if (m_cSignalMatchParam.fEnablePanoramicCamera)
    {
        MatchPanoramicSignal(pPlateInfo->dwInputTime, &pPanormicImage);
        if (pPanormicImage == NULL)
        {
            if (pPlateInfo->nStatus == STATUS_OUTTIME)
            {
                fSuccessMatch = true;
            }
            else
            {
                fSuccessMatch = false;
            }
        }
        else
        {
            fSuccessMatch = true;
        }
    }

    if (fSuccessMatch)
    {
        // 初始化当前匹配信息结构
        m_rgMatchInfo[m_nMatchInfoNum].nMatchType = MATCH_PLATE;  // 有牌车匹配
        m_rgMatchInfo[m_nMatchInfoNum].dwSpeed = 0;
        m_rgMatchInfo[m_nMatchInfoNum].dwWeight = 0;
        m_rgMatchInfo[m_nMatchInfoNum].pLastImage = NULL;
        m_rgMatchInfo[m_nMatchInfoNum].pLastImage_2 = NULL;
        m_rgMatchInfo[m_nMatchInfoNum].pPanormicImage = NULL;

        // 所有类型信号都匹配成功，设置车牌队列和信号的匹配信息
        pPlateInfo->nStatus = STATUS_MATCH;  // 本车牌已经匹配

        // 依次设置所有匹配成功的信号
        for (int i = 0; i < g_nSignalTypeNum; i++)
        {
            if (rgbSignalType[i])
            {
                SIGNAL_INFO *pSignalInfo =
                    g_prgQueSignal[rgMatchInfo[i].nSigSourceType].GetNext(rgMatchInfo[i].nSignalPos);

                SIGNAL_INFO *pSignalInfo_2 = NULL;
                if ((g_prgMatchSigType[i].atType == ATTACH_INFO_LAST_IMAGE)
                        && (rgMatchInfo[i].nSignalPos_2 != -1)
                   )
                {
                    pSignalInfo_2 = g_prgQueSignal[rgMatchInfo[i].nSigSourceType].GetNext(rgMatchInfo[i].nSignalPos_2);
                }

                int iSourceNum = pSignalInfo->nType;
                int iSourceTypeNum = g_prgSigSource[iSourceNum].nType;

                //如果是全覆盖的抓拍方式(即全景),图片匹配上了也不删除,只有超时后才删除.
                if ( g_prgMatchSigType[iSourceTypeNum].atType == ATTACH_INFO_LAST_IMAGE
                        && g_prgSigSource[iSourceNum].iRoad == 0xFF )
                {
                    //不设置匹配标志
                }
                else
                {
                    pSignalInfo->nStatus = STATUS_MATCH;			// 本信号已经匹配
                }

                // 加入信号附带的信息
                GetSignalAttchInfo(&m_rgMatchInfo[m_nMatchInfoNum], pSignalInfo, pSignalInfo_2);
            }
        }
        if (pPanormicImage)
        {
            m_rgMatchInfo[m_nMatchInfoNum].pPanormicImage = pPanormicImage;
            m_rgMatchInfo[m_nMatchInfoNum].pPanormicImage->AddRef();
        }
    } // if (fSuccessMatch)

    // 释放所有信号队列
    for (int i = 0; i < g_nSignalSourceNum; i++)
    {
        UnlockSignal(i);
    }
    return(fSuccessMatch);
}

// 把信号的附加信息加入匹配信息结构
void COuterControlImpl::GetSignalAttchInfo(
    MATCH_INFO  *pMatchInfo,
    SIGNAL_INFO *pSignalInfo,
    SIGNAL_INFO *pSignalInfo_2/* = NULL*/)
{
    int nSigType = g_prgSigSource[ pSignalInfo->nType].nType;
    switch (g_prgMatchSigType[nSigType].atType)
    {
    case ATTACH_INFO_SPEED:
        pMatchInfo->dwSpeed = pSignalInfo->dwValue;
        HV_Trace(5, "match speed,value=%d.\n", pSignalInfo->dwValue);
        break;
    case ATTACH_INFO_WEIGHT:
        pMatchInfo->dwWeight = pSignalInfo->dwValue;
        break;
    case ATTACH_INFO_LAST_IMAGE:
        if (pMatchInfo->pLastImage != NULL)
        {
            pMatchInfo->pLastImage->Release();
            pMatchInfo->pLastImage = NULL;
        }
        if (pSignalInfo->pImageLast != NULL)
        {
            pMatchInfo->pLastImage = pSignalInfo->pImageLast;
            pMatchInfo->pLastImage->AddRef();
            // zhaopy
            ReleaseProcessEvent(&pMatchInfo->cProcessEvent);
            memcpy(
                &(pMatchInfo->cProcessEvent),
                &(pSignalInfo->cProcessEvent),
                sizeof(PROCESS_EVENT_STRUCT)
            );
            AddRefProcessEvent(&pMatchInfo->cProcessEvent);
        }
        if (pSignalInfo_2)
        {
            if (pMatchInfo->pLastImage_2 != NULL)
            {
                pMatchInfo->pLastImage_2->Release();
                pMatchInfo->pLastImage_2 = NULL;
            }
            if (pSignalInfo_2->pImageLast != NULL)
            {
                pMatchInfo->pLastImage_2 = pSignalInfo_2->pImageLast;
                pMatchInfo->pLastImage_2->AddRef();
            }
        }
        break;
    case ATTACH_INFO_NOTHING:
    default:
        break;
    }
}

bool COuterControlImpl::MatchNoPlate(SIGNAL_INFO *pSignalInfoMain)
{
    HVPOSITION nPos;
    int nSigType = g_prgSigSource[pSignalInfoMain->nType].nType;  // 匹配信号类型
    //信号的车道编号
    int iMainSignalRoad = g_prgSigSource[pSignalInfoMain->nType].iRoad;

    // 匹配统计结构
    bool rgbSignalType[MAX_SIGNAL_TYPE];  // 按类型统计匹配
    MATCH_ANALYSIS rgMatchInfo[MAX_SIGNAL_TYPE];  // 当前车牌的匹配信息

    // 初始化统计结构
    for (int i = 0; i < g_nSignalTypeNum; i++)
    {
        rgbSignalType[i] = false;
        rgMatchInfo[i].nSignalPos = -1;
        rgMatchInfo[i].nSignalPos_2 = -1;
    }

    //锁定所有信号队列
    for (int i = 0; i < g_nSignalSourceNum; i++)
    {
        if (g_prgSigSource[i].nType != nSigType)
        {
            // 只匹配和主信号类型不同的信号
            LockSignal(i);
        }
    }

    // 匹配所有信号源的信号
    for (int i = 0; i < g_nSignalSourceNum; i++)
    {
        if (g_prgSigSource[i].nType == nSigType)
        {
            continue;  // 类型和主信号相同，则跳过
        }

        int nSrgType = g_prgSigSource[i].nType;
        nPos = g_prgQueSignal[i].GetHeadPosition();
        while (g_prgQueSignal[i].IsValidPos(nPos))
        {
            HVPOSITION nCurSigPos = nPos;  // 保存当前位置
            SIGNAL_INFO *pSignalInfo = g_prgQueSignal[i].GetNext(nPos);  // 取得该信号源的信号
            //信号的车道编号
            int iRoad = g_prgSigSource[pSignalInfo->nType].iRoad;

            if (pSignalInfo->nStatus != STATUS_MATCH)
            {
                long lTimeDiff(0);
                if (	(iRoad == iMainSignalRoad || iMainSignalRoad == 0xFF || iRoad == 0xFF)
                        && IsMatchSignal(pSignalInfoMain, pSignalInfo, lTimeDiff) )
                {
                    // 主信号和信号已经匹配，则记录匹配结果
                    if ((!rgbSignalType[nSrgType])
                            || (abs(lTimeDiff) < abs(rgMatchInfo[nSrgType].lTimeDiff)))
                    {
                        // 原来未有匹配信息或者当前时间差异比较小
                        rgMatchInfo[nSrgType].nSigSourceType = i;				// 信号源类型
                        rgMatchInfo[nSrgType].nSignalPos = nCurSigPos;			// 当前位置
                        rgMatchInfo[nSrgType].lTimeDiff = lTimeDiff;		// 时间差异

                        // 更新类型匹配
                        rgbSignalType[nSrgType] = true;  // 这种类型已经匹配本车牌

                        // 如果需要多张抓拍图片
                        if (g_iCapImgCount != 1
                                && (g_prgMatchSigType[nSrgType].atType == ATTACH_INFO_LAST_IMAGE))
                        {
                            if (g_prgQueSignal[i].IsValidPos(nPos))
                            {
                                // 获取下一张抓拍图片
                                HVPOSITION nPosTmp = nPos;
                                SIGNAL_INFO *pSignalInfo_2 = g_prgQueSignal[i].GetNext(nPosTmp);
                                if (pSignalInfo_2->dwSignalTime - pSignalInfo->dwSignalTime > 0)
                                {
                                    rgMatchInfo[nSrgType].nSignalPos_2 = nPos;
                                }
                                else
                                {
                                    rgbSignalType[nSrgType] = false;
                                }
                            }
                            else
                            {
                                rgbSignalType[nSrgType] = false;
                            }
                        }
                    }
                } // if match
            } // if (pSignalInfo->nStatus != STATUS_MATCH)
        } // while(g_prgQueSignal[i].IsValidPos(nPos))
    } // for

    // 初始化当前匹配信息结构
    m_rgMatchInfo[m_nMatchInfoNum].nMatchType = MATCH_NO_PLATE;
    m_rgMatchInfo[m_nMatchInfoNum].dwSpeed = 0;
    m_rgMatchInfo[m_nMatchInfoNum].dwWeight = 0;
    m_rgMatchInfo[m_nMatchInfoNum].pLastImage =NULL;
    m_rgMatchInfo[m_nMatchInfoNum].pLastImage_2 = NULL;

    // 所有类型信号都匹配成功，设置主信号和其他信号的匹配信息
    pSignalInfoMain->nStatus = STATUS_MATCH;
    GetSignalAttchInfo(&m_rgMatchInfo[m_nMatchInfoNum], pSignalInfoMain);

    // 依次设置所有匹配成功的信号
    for (int i = 0; i < g_nSignalTypeNum; i++)
    {
        if (rgbSignalType[i])
        {
            SIGNAL_INFO *pSignalInfo =
                g_prgQueSignal[rgMatchInfo[i].nSigSourceType].GetNext(rgMatchInfo[i].nSignalPos);

            SIGNAL_INFO *pSignalInfo_2 = NULL;
            if (g_iCapImgCount != 1
                    && (g_prgMatchSigType[i].atType == ATTACH_INFO_LAST_IMAGE)
                    && rgMatchInfo[i].nSignalPos_2 != -1)
            {
                pSignalInfo_2 = g_prgQueSignal[rgMatchInfo[i].nSigSourceType].GetNext(rgMatchInfo[i].nSignalPos_2);
            }

            int iSourceNum = pSignalInfo->nType;
            int iSourceTypeNum = g_prgSigSource[iSourceNum].nType;

            //如果是全覆盖的抓拍方式(即全景),图片匹配上了也不删除,只有超时后才删除.
            if ( g_prgMatchSigType[iSourceTypeNum].atType == ATTACH_INFO_LAST_IMAGE
                    && g_prgSigSource[iSourceNum].iRoad == 0xFF )
            {
                //不设置匹配标志
            }
            else
            {
                pSignalInfo->nStatus = STATUS_MATCH;			// 本信号已经匹配
            }

            // 加入信号附带的信息
            GetSignalAttchInfo(&m_rgMatchInfo[m_nMatchInfoNum], pSignalInfo, pSignalInfo_2);
        }
    }

    // 释放所有信号队列
    for (int i = 0; i < g_nSignalSourceNum; i++)
    {
        if (g_prgSigSource[i].nType != nSigType)
        {
            UnlockSignal(i);
        }
    }
    return true;						// 主信号永远匹配的，输出无牌车
}

int COuterControlImpl::Match()
{
    HVPOSITION nPos;
    m_nMatchInfoNum = 0;			// 匹配数量清0

    // 匹配所有车牌
    LockPlate();
    nPos = g_quePlate.GetHeadPosition();
    while (g_quePlate.IsValidPos(nPos)  && (m_nMatchInfoNum < SIGNAL_LIST_COUNT))
    {
        PLATE_INFO_hxl *pPlateInfo = g_quePlate.GetNext(nPos);  // 取得一个车牌记录
        if (pPlateInfo->nStatus != STATUS_MATCH)
        {
            if (MatchPlate(pPlateInfo))  // 匹配当前车牌
            {
                m_rgMatchInfo[m_nMatchInfoNum++].pInfo = pPlateInfo;  // 设置车牌指针
            }
        }
    }
    UnlockPlate();

    // 匹配所有超期的主信号
    for (int i = 0; i < m_nMainSourceNum; i++)
    {
        int nSrcType = m_prgMainSource[i];  // 主信号源编号

        LockSignal(nSrcType);
        nPos = g_prgQueSignal[nSrcType].GetHeadPosition();
        while (g_prgQueSignal[nSrcType].IsValidPos(nPos)
                && (m_nMatchInfoNum < SIGNAL_LIST_COUNT))
        {
            // 取得该信号源的信号
            SIGNAL_INFO *pSignalInfo = g_prgQueSignal[nSrcType].GetNext(nPos);

            if (pSignalInfo->nStatus == STATUS_OUTTIME)
            {
                // 主信号超期了,进行无牌车匹配
                if (MatchNoPlate(pSignalInfo))  // 匹配当前主信号
                {
                    if (m_cSignalMatchParam.fEnablePanoramicCamera)
                    {
                        IReferenceComponentImage* pPanoramicImage = NULL;
                        MatchPanoramicSignal(pSignalInfo->dwSignalTime, &pPanoramicImage);
                        if (pPanoramicImage)
                        {
                            m_rgMatchInfo[m_nMatchInfoNum].pPanormicImage = pPanoramicImage;
                            m_rgMatchInfo[m_nMatchInfoNum].pPanormicImage->AddRef();
                        }
                    }
                    // 匹配信息结构带入主信号的指针
                    m_rgMatchInfo[m_nMatchInfoNum++].pInfo = pSignalInfo;
                }
            }
        }
        UnlockSignal(nSrcType);
    }
    return m_nMatchInfoNum;  // 返回本次匹配的数量
}

// 输出匹配的车牌
void COuterControlImpl::OutputMatchPlate()
{
    for (int i = 0; i < m_nMatchInfoNum; i++)
    {
        if (!m_fRunFlag)
        {
            m_nMatchInfoNum = 0;
            return;
        }
        if (m_rgMatchInfo[i].nMatchType == MATCH_PLATE)
        {
            // 有牌车匹配
            HV_Trace(5, "<Outer>OutPutPlate: P:%d\n", ((PLATE_INFO_hxl*)m_rgMatchInfo[i].pInfo)->dwPlateTime);
            OutPutPlate(&m_rgMatchInfo[i]);
            HV_Sleep(20);
            m_dwOuterThreadIsOkTime = GetSystemTick();
        }
        else
        {
            // 超时匹配
            HV_Trace(5, "<Outer>OutPutNoPlate: P:%d\n", ((PLATE_INFO_hxl*)m_rgMatchInfo[i].pInfo)->dwPlateTime);
            OutPutNoPlate(&m_rgMatchInfo[i]);
            HV_Sleep(20);
            m_dwOuterThreadIsOkTime = GetSystemTick();
        }
    }

    m_nMatchInfoNum = 0;					// 匹配数量清0
}

int ReplaceResult(CARLEFT_INFO_STRUCT *pCarLeftInfo, PROCESS_EVENT_STRUCT *pProcessEvent, bool fMainSignal = false)
{
    if (pCarLeftInfo == NULL || pProcessEvent == NULL || pProcessEvent->iCarLeftInfoCount <= 0)
    {
        return -1;
    }

    int nBestPlate = -1;
    int iMinDiffX = 1600, iImgWidth = 1600;
    HV_COMPONENT_IMAGE img;

    if (pCarLeftInfo->cCoreResult.cResultImg.pimgLastSnapShot
            && pCarLeftInfo->cCoreResult.cResultImg.pimgLastSnapShot->GetImage(&img))
    {
        if (img.nImgType == HV_IMAGE_JPEG)
        {
            iImgWidth = img.iHeight & 0x0000FFFF;
        }
        else
        {
            iImgWidth = img.iWidth;
        }
    }

    iMinDiffX = iImgWidth;

    for (int i = 0; i < pProcessEvent->iCarLeftInfoCount; ++i)
    {
        CRect rcCurPlate = pProcessEvent->rgCarLeftInfo[i].cCoreResult.rcBestPlatePos;
        CRect rcCarLeftPlate = pCarLeftInfo->cCoreResult.rcBestPlatePos;
        if (rcCurPlate.CenterPoint().y < rcCarLeftPlate.CenterPoint().y)
        {
            continue;
        }
        int iDiff = abs(rcCurPlate.CenterPoint().x - rcCarLeftPlate.CenterPoint().x);
        if (iDiff < iMinDiffX)
        {
            iMinDiffX = iDiff;
            nBestPlate = i;
        }
    }
    if (iMinDiffX == iImgWidth)
    {
        for (int i = 0; i < pProcessEvent->iCarLeftInfoCount; ++i)
        {
            CRect rcCurPlate = pProcessEvent->rgCarLeftInfo[i].cCoreResult.rcBestPlatePos;
            CRect rcCarLeftPlate = pCarLeftInfo->cCoreResult.rcBestPlatePos;
            int iDiff = abs(rcCurPlate.CenterPoint().x - rcCarLeftPlate.CenterPoint().x);
            if (iDiff < iMinDiffX)
            {
                iMinDiffX = iDiff;
                nBestPlate = i;
            }
        }
    }

    if (nBestPlate != -1)
    {
        CRect rcCurPlate = pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.rcBestPlatePos;
        if (iMinDiffX > (rcCurPlate.right - rcCurPlate.left) * 4)
        {
            nBestPlate = -1;
        }
    }
    if(nBestPlate == -1)
    {
        if(false == fMainSignal)
        {
            HV_Trace(5, "nBestPlate == -1");
            return -1;
        }
        else
        {
            nBestPlate = 0;
        }
    }
    pCarLeftInfo->cCoreResult.fIsCapture = true;
    pCarLeftInfo->cCoreResult.iCapturerAvgY = pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.iAvgY;
    static char szPlateName[32] = {0};
    GetPlateNameAlpha(
        (char*)szPlateName,
        ( PLATE_TYPE )pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.nType,
        ( PLATE_COLOR )pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.nColor,
        pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.rgbContent
    );
    switch (pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.nCarColor)
    {
    case CC_WHITE:
        HV_Trace(5, "[%d:%d]%s:CC_WHITE, IsNight=%d", pProcessEvent->iCarLeftInfoCount, nBestPlate, szPlateName, pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.fIsNight);
        break;
    case CC_GREY:
        HV_Trace(5, "[%d:%d]%s:CC_GREY, IsNight=%d", pProcessEvent->iCarLeftInfoCount, nBestPlate, szPlateName, pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.fIsNight);
        break;
    case CC_BLACK:
        HV_Trace(5, "[%d:%d]%s:CC_BLACK, IsNight=%d", pProcessEvent->iCarLeftInfoCount, nBestPlate, szPlateName, pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.fIsNight);
        break;
    case CC_RED:
        HV_Trace(5, "[%d:%d]%s:CC_RED, IsNight=%d", pProcessEvent->iCarLeftInfoCount, nBestPlate, szPlateName, pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.fIsNight);
        break;
    case CC_YELLOW:
        HV_Trace(5, "[%d:%d]%s:CC_YELLOW, IsNight=%d", pProcessEvent->iCarLeftInfoCount, nBestPlate, szPlateName, pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.fIsNight);
        break;
    case CC_GREEN:
        HV_Trace(5, "[%d:%d]%s:CC_GREEN, IsNight=%d", pProcessEvent->iCarLeftInfoCount, nBestPlate, szPlateName, pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.fIsNight);
        break;
    case CC_BLUE:
        HV_Trace(5, "[%d:%d]%s:CC_BLUE, IsNight=%d", pProcessEvent->iCarLeftInfoCount, nBestPlate, szPlateName, pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.fIsNight);
        break;
    case CC_PURPLE:
        HV_Trace(5, "[%d:%d]%s:CC_PURPLE, IsNight=%d", pProcessEvent->iCarLeftInfoCount, nBestPlate, szPlateName, pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.fIsNight);
        break;
    case CC_PINK:
        HV_Trace(5, "[%d:%d]%s:CC_PINK, IsNight=%d", pProcessEvent->iCarLeftInfoCount, nBestPlate, szPlateName, pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.fIsNight);
        break;
    case CC_BROWN:
        HV_Trace(5, "[%d:%d]%s:CC_BROWN, IsNight=%d", pProcessEvent->iCarLeftInfoCount, nBestPlate, szPlateName, pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.fIsNight);
        break;
    default:
        HV_Trace(5, "[%d:%d]%s:CC_UNKNOWN, IsNight=%d", pProcessEvent->iCarLeftInfoCount, nBestPlate, szPlateName, pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.fIsNight);
        break;
    }
    // 无车牌则进行识别结果替换，有车牌则替换车身颜色
    if (pCarLeftInfo->cCoreResult.rgbContent[0] == 0)
    {
        if (nBestPlate == -1)
        {
            HV_Trace(5, "Not found the match result!\n");
        }
        else
        {
            memcpy(
                pCarLeftInfo->cCoreResult.rgbContent,
                pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.rgbContent,
                8
            );

            pCarLeftInfo->cCoreResult.nType = pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.nType;
            pCarLeftInfo->cCoreResult.nColor = pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.nColor;
            pCarLeftInfo->cCoreResult.fltAverageConfidence = pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.fltAverageConfidence;
            pCarLeftInfo->cCoreResult.fltFirstAverageConfidence = pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.fltFirstAverageConfidence;
            pCarLeftInfo->cCoreResult.iObservedFrames = 1;
            pCarLeftInfo->cCoreResult.nColor = pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.nColor;
            pCarLeftInfo->cCoreResult.nCarColor = pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.nCarColor;

            pCarLeftInfo->cCoreResult.cResultImg.pimgPlate = pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.cResultImg.pimgPlate;
            pCarLeftInfo->cCoreResult.cResultImg.pimgPlateBin = pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.cResultImg.pimgPlateBin;

            AllImageAddRef(&(pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.cResultImg));
        }
    }
    else
    {
        if (nBestPlate >= 0
                && pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.nCarColor != CC_UNKNOWN)
        {
            pCarLeftInfo->cCoreResult.nCarColor = pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.nCarColor;
        }
    }
    pCarLeftInfo->cCoreResult.nFaceCount = pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.nFaceCount;
    memcpy(pCarLeftInfo->cCoreResult.rgFaceRect , pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.rgFaceRect ,
           sizeof(pProcessEvent->rgCarLeftInfo[nBestPlate].cCoreResult.rgFaceRect));

    if (pCarLeftInfo->cCoreResult.nFaceCount > 0)
    {
        static char szFace[256];
        strcpy(szFace, "");
        for (int i = 0; i < pCarLeftInfo->cCoreResult.nFaceCount; i++)
        {
            sprintf(szFace + strlen(szFace), "(%d,%d,%d,%d)", pCarLeftInfo->cCoreResult.rgFaceRect[i].left, pCarLeftInfo->cCoreResult.rgFaceRect[i].top, pCarLeftInfo->cCoreResult.rgFaceRect[i].right, pCarLeftInfo->cCoreResult.rgFaceRect[i].bottom);
        }
        HV_Trace(5, "<Match> face pos=%s", szFace);
    }
    return nBestPlate;
}

// 输出有牌车信息
void COuterControlImpl::OutPutPlate(MATCH_INFO  *pMatchInfo)
{
    PLATE_INFO_hxl *pPlateInfo = (PLATE_INFO_hxl *)pMatchInfo->pInfo;

    if (pPlateInfo == NULL)
    {
        return;
    }
    /*
        //为了方便测试，输出2个速度值
        int nSpeed = pPlateInfo->pCarleftInfo->cCoreResult.fltCarspeed;
        nSpeed *= 1000;
        nSpeed += pMatchInfo->dwSpeed;
        pPlateInfo->pCarleftInfo->cCoreResult.fltCarspeed = nSpeed;
    */
    // 附上车牌数据
    if (pMatchInfo->dwSpeed != 0)
    {
        pPlateInfo->pCarleftInfo->cCoreResult.fltCarspeed = (float)pMatchInfo->dwSpeed;
    }

    if(m_cSignalMatchParam.fEnablePanoramicCamera)
    {
           IReferenceComponentImage *pimgTmp;
           pimgTmp = pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastSnapShot;
           pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastSnapShot = pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgBestSnapShot;
           pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgBestSnapShot = pimgTmp;
    }

    if ((!m_cSignalMatchParam.nPlateType) //所有车牌
            || (m_cSignalMatchParam.nPlateType == 1 && pPlateInfo->pCarleftInfo->cCoreResult.nColor == PC_YELLOW)//仅黄牌
       )
    {
        // 附加的替换图片
        if (pMatchInfo->pLastImage != NULL)
        {
            if (m_cSignalMatchParam.fEPolice)
            {
                //电警的抓拍图替换第2张轨迹图
                if (pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgBestCapture != NULL)
                {
                    pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgBestCapture->Release();
                    pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgBestCapture = NULL;
                }
                pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgBestCapture = pMatchInfo->pLastImage;
            }
            else
            {
                // 使用图片替换last图片
                if (pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastSnapShot != NULL)
                {
                    pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastSnapShot->Release();
                    pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastSnapShot = NULL;
                }
                pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastSnapShot = pMatchInfo->pLastImage;
            }
        }
        if (pMatchInfo->pLastImage_2 != NULL)
        {
            // 附加第二张抓拍图
            // 使用图片替换last图片
            if (pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgBeginCapture != NULL)
            {
                pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgBeginCapture->Release();
                pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgBeginCapture = NULL;
            }
            pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgBeginCapture = pMatchInfo->pLastImage_2;
        }
        if (g_iCapImgCount == 2)
        {
            pPlateInfo->pCarleftInfo->cCoreResult.iSendCapImageFlag = 1;
            // 去掉后面两张抓拍图
            if (pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgBestCapture != NULL)
            {
                pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgBestCapture->Release();
                pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgBestCapture = NULL;
            }
            if (pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastCapture != NULL)
            {
                pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastCapture->Release();
                pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastCapture = NULL;
            }
        }
    }
    pPlateInfo->pCarleftInfo->cOtherInfo.iRoadNumberBegin = m_iRoadNumberBegin;
    pPlateInfo->pCarleftInfo->cOtherInfo.iStartRoadNum = m_iStartRoadNum;

    if (pMatchInfo->pLastImage)
    {
        ReplaceResult(pPlateInfo->pCarleftInfo, &(pMatchInfo->cProcessEvent));
        if (m_cSignalMatchParam.fEnableRecogSnapImage)
        {
            pPlateInfo->pCarleftInfo->cCoreResult.fIsCapture = true;
        }
        ReleaseProcessEvent(&(pMatchInfo->cProcessEvent));
    }

    if (pMatchInfo->pPanormicImage)
    {
        if (!pPlateInfo->pCarleftInfo->cCoreResult.fIsNight)
        {
            if (pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastSnapShot != NULL)
            {
                pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastSnapShot->Release();
                pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastSnapShot = NULL;
            }
            pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastSnapShot = pMatchInfo->pPanormicImage;
        }
        else
        {
            IReferenceComponentImage *pimgTmp;
            pimgTmp = pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastSnapShot;
            pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastSnapShot = pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgBestSnapShot;
            pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgBestSnapShot = pimgTmp;

            if (pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastSnapShot != NULL)
            {
                pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastSnapShot->Release();
                pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastSnapShot = NULL;
            }
            pPlateInfo->pCarleftInfo->cCoreResult.cResultImg.pimgLastSnapShot = pMatchInfo->pPanormicImage;

            pPlateInfo->pCarleftInfo->cCoreResult.fltCarspeed = (int)(pPlateInfo->pCarleftInfo->cCoreResult.fltCarspeed * 1.1);
            if(pPlateInfo->pCarleftInfo->cCoreResult.fltCarspeed > 140)
            {
                srand(GetSystemTick());
                int iRand = rand() % 20;
                pPlateInfo->pCarleftInfo->cCoreResult.fltCarspeed = 120 + iRand;
            }
        }
    }

    // 车牌已经匹配，则输出车牌
    SendMatchInfo(pPlateInfo->pCarleftInfo, pPlateInfo->pvUserData);
}

// 输出无牌车信息
void COuterControlImpl::OutPutNoPlate(MATCH_INFO  *pMatchInfo)
{
    SIGNAL_INFO *pSignalInfo = (SIGNAL_INFO *)pMatchInfo->pInfo;

    if (pSignalInfo == NULL)
    {
        return;
    }

    IReferenceComponentImage *pImage = pSignalInfo->pImage;

    CARLEFT_INFO_STRUCT stCarLeftInfo;

    RESULT_IMAGE_STRUCT reImage;

    reImage.pimgBestSnapShot = pImage;
    if (pMatchInfo->pLastImage != NULL)
    {
        reImage.pimgLastSnapShot = pMatchInfo->pLastImage;
    }
    else
    {
        reImage.pimgLastSnapShot = pImage;
    }

    //电警才输出5张图 黄冠榕 2011-09-01
    if (GetCurrentMode() == PRM_ELECTRONIC_POLICE)
    {
        reImage.pimgBeginCapture = pImage;
        reImage.pimgBestCapture = pImage;
        reImage.pimgLastCapture = pImage;
    }

    if (pMatchInfo->pLastImage_2 != NULL)
    {
        reImage.pimgBeginCapture = pMatchInfo->pLastImage_2;
    }

    // 如果有两张抓拍图，统一成只出三张大图
    if (g_iCapImgCount == 2)
    {
        stCarLeftInfo.cCoreResult.iSendCapImageFlag = 1;
        reImage.pimgBestCapture = NULL;
        reImage.pimgLastCapture = NULL;
    }

    reImage.pimgPlateBin = NULL;
    reImage.pimgPlate = NULL;

    // 结果结构赋值
    stCarLeftInfo.cCoreResult.rgbContent[0] = 0;

    stCarLeftInfo.cCoreResult.nType = PLATE_UNKNOWN;
    stCarLeftInfo.cCoreResult.nColor = PC_UNKNOWN;
    stCarLeftInfo.cCoreResult.fltAverageConfidence = 0;
    stCarLeftInfo.cCoreResult.fltFirstAverageConfidence = 0;
    stCarLeftInfo.cCoreResult.cResultImg = reImage;
    stCarLeftInfo.cOtherInfo.nPlateLightType = m_nLastLightType;
    stCarLeftInfo.cCoreResult.nPlateLightType = m_nLastLightType;
    stCarLeftInfo.cCoreResult.iPulseLevel = m_iPulseLevel;
    stCarLeftInfo.cCoreResult.iCplStatus = m_iCplStatus;
    if (pImage != NULL)
    {
        stCarLeftInfo.cOtherInfo.iFrameNo = pImage->GetFrameNo();
        stCarLeftInfo.cOtherInfo.iRefTime = pImage->GetRefTime();
        stCarLeftInfo.cOtherInfo.nCarArriveTime = pImage->GetRefTime();
    }
    else
    {
        stCarLeftInfo.cOtherInfo.iFrameNo = 0;
        stCarLeftInfo.cOtherInfo.iRefTime = 0;
        stCarLeftInfo.cOtherInfo.nCarArriveTime = GetSystemTick();
        HV_Trace(5, "<OutPutNoPlate> pImage = NULL");
    }
    stCarLeftInfo.cOtherInfo.nStartFrameNo = 0;
    stCarLeftInfo.cOtherInfo.nEndFrameNo = 0;
    stCarLeftInfo.cOtherInfo.nFirstFrameTime = 0;
    stCarLeftInfo.cOtherInfo.iObservedFrames = 0;
    stCarLeftInfo.cCoreResult.fltAverageConfidence = 0;
    stCarLeftInfo.cCoreResult.fltFirstAverageConfidence = 0;
    stCarLeftInfo.cCoreResult.fReverseRun = false;
    if ((pSignalInfo->dwRoad & 0x00ff0000) == 0x00ff0000)
    {
        stCarLeftInfo.cCoreResult.nRoadNo = pSignalInfo->dwRoad & 0x0000ffff;
    }
    else
    {
        // 输出无牌车车道号
        stCarLeftInfo.cCoreResult.nRoadNo = g_prgSigSource[pSignalInfo->nType].iRoad;
    }

    stCarLeftInfo.cOtherInfo.nCarLeftCount = 0;			            // 取得车辆离开帧数计数器
    stCarLeftInfo.cCoreResult.iAvgY = 0;							// 环境亮度
    stCarLeftInfo.cCoreResult.fltCarspeed = 0;						// 无牌车速度为0
    if (pMatchInfo->dwSpeed != 0)
    {
        stCarLeftInfo.cCoreResult.fltCarspeed = pMatchInfo->dwSpeed;  // 无牌车速度
    }

    stCarLeftInfo.cOtherInfo.iRoadNumberBegin = m_iRoadNumberBegin;
    stCarLeftInfo.cOtherInfo.iStartRoadNum = m_iStartRoadNum;

    int nResultIndex = -1;
    if (pMatchInfo->pLastImage)
    {
        nResultIndex = ReplaceResult(&stCarLeftInfo, &(pSignalInfo->cProcessEvent), true);
        if (m_cSignalMatchParam.fEnableRecogSnapImage)
        {
            stCarLeftInfo.cCoreResult.fIsCapture = true;
        }
    }

    if (pMatchInfo->pPanormicImage)
    {
        pMatchInfo->pPanormicImage->AddRef();
        if (!stCarLeftInfo.cCoreResult.fIsNight)
        {
            if (stCarLeftInfo.cCoreResult.cResultImg.pimgLastSnapShot != NULL)
            {
                stCarLeftInfo.cCoreResult.cResultImg.pimgLastSnapShot->Release();
                stCarLeftInfo.cCoreResult.cResultImg.pimgLastSnapShot = NULL;
            }
            stCarLeftInfo.cCoreResult.cResultImg.pimgLastSnapShot = pMatchInfo->pPanormicImage;
        }
        else
        {
            IReferenceComponentImage *pimgTmp;
            pimgTmp = stCarLeftInfo.cCoreResult.cResultImg.pimgLastSnapShot;
            stCarLeftInfo.cCoreResult.cResultImg.pimgLastSnapShot = stCarLeftInfo.cCoreResult.cResultImg.pimgBestSnapShot;
            stCarLeftInfo.cCoreResult.cResultImg.pimgBestSnapShot = pimgTmp;

            if (stCarLeftInfo.cCoreResult.cResultImg.pimgLastSnapShot != NULL)
            {
                stCarLeftInfo.cCoreResult.cResultImg.pimgLastSnapShot->Release();
                stCarLeftInfo.cCoreResult.cResultImg.pimgLastSnapShot = NULL;
            }
            stCarLeftInfo.cCoreResult.cResultImg.pimgLastSnapShot = pMatchInfo->pPanormicImage;

            stCarLeftInfo.cCoreResult.fltCarspeed = (int)(stCarLeftInfo.cCoreResult.fltCarspeed * 1.1);
            if(stCarLeftInfo.cCoreResult.fltCarspeed > 140)
            {
                srand(GetSystemTick());
                int iRand = rand() % 20;
                stCarLeftInfo.cCoreResult.fltCarspeed = 120 + iRand;
            }
        }
    }

    if (GetCurrentMode() != PRM_HVC) //HVC不输出主信号超时的未检测
    {
        SendMatchInfo(&stCarLeftInfo, NULL);
        if (nResultIndex >= 0)
        {
            // 由于信号有可能被重复使用，因此释放信号时不能将指针设置为NULL
            AllImageReleaseNotNull(&(pSignalInfo->cProcessEvent.rgCarLeftInfo[nResultIndex].cCoreResult.cResultImg));
        }
    }

    if (pMatchInfo->pLastImage != NULL)
    {
        pMatchInfo->pLastImage->Release();
    }
    if (pMatchInfo->pLastImage_2 != NULL)
    {
        pMatchInfo->pLastImage_2->Release();
    }
    if (pMatchInfo->pPanormicImage != NULL)
    {
        pMatchInfo->pPanormicImage->Release();
    }
}

void COuterControlImpl::ReleaseOldElement()
{
    HVPOSITION nPos;

    // 先清理车牌队列
    LockPlate();
    nPos = g_quePlate.GetHeadPosition();

    while (g_quePlate.IsValidPos(nPos))
    {
        HVPOSITION oldPos = nPos;
        PLATE_INFO_hxl *pPlateInfo = g_quePlate.GetNext(nPos);
        if ((pPlateInfo->nStatus == STATUS_OUTTIME)
                || pPlateInfo->nStatus == STATUS_MATCH)
        {
            // 匹配或者超期，都进行删除
            ReleasePlateInfo(pPlateInfo);
            g_quePlate.RemoveAt(oldPos);
            // 删除后nPos要自减1，因为队列索引已改变
            nPos--;
        }
    }

    UnlockPlate();

    // 清理所有信号队列
    int nSignalNum(0);

    for (int i = 0; i < g_nSignalSourceNum; i++)
    {
        nSignalNum = 0;
        LockSignal(i);
        nPos = g_prgQueSignal[i].GetHeadPosition();
        while ( g_prgQueSignal[i].IsValidPos(nPos))
        {
            HVPOSITION oldPos = nPos;
            SIGNAL_INFO *pSignalInfo = g_prgQueSignal[i].GetNext(nPos);
            if ((pSignalInfo->nStatus == STATUS_OUTTIME)
                    || pSignalInfo->nStatus == STATUS_MATCH)
            {
                // 匹配或者超期，都进行删除
                ReleaseSignalInfo(pSignalInfo);

                // 删除当前位置节点
                g_prgQueSignal[i].RemoveAt(oldPos);
                g_nTestCount--;
                // 删除后nPos要自减1，因为队列索引已改变
                nPos--;
            }
        }

        UnlockSignal(i);
    } // for

    SemPend(&g_SemPanormicSignal);
    nPos = g_PanormicSignal.GetHeadPosition();
    while (g_PanormicSignal.IsValidPos(nPos))
    {
        HVPOSITION oldPos = nPos;
        SIGNAL_INFO* pSignalInfo = g_PanormicSignal.GetNext(nPos);
        if (pSignalInfo->nStatus == STATUS_OUTTIME)
        {
            if (pSignalInfo->pImageLast)
            {
                pSignalInfo->pImageLast->Release();
                pSignalInfo->pImageLast = NULL;
            }
            delete pSignalInfo;
            g_PanormicSignal.RemoveAt(oldPos);
            nPos--;
        }
    }
    SemPost(&g_SemPanormicSignal);
}

//------------
// IPT接口函数
//------------
HRESULT COuterControlImpl::ProcessIPTSignal()
{
    // 如果在这不去读设备版本号信息，在狂硬触发下会复位，原因不明。
    // 读取IPT版本号
    char szVersion[128] = {0};
    HV_memset(szVersion, 0, 128);
    if ( S_OK == IPT_ReadSoftwareVersion((PBYTE8)szVersion, m_pIPTControl))
    {
        HV_Trace(5, "IPT version : %s \n", szVersion);
    }
    else
    {
        HV_Trace(5, "IPT Version : Unknown \n");
    }
    //IPT设备参数的初始化
    int i = 0;
    for (i = 0; i < m_iDeviceParamLen; ++i)
    {
        if (m_rgbDeviceParam[i] != 0x00) break;
    }
    //判断当前XML里面的参数是否有效
    if ( i >= m_iDeviceParamLen || m_iDeviceParamLen < 256 )
    {
        m_iDeviceParamLen = 0;
    }
    //如果XML的数据有效,则写入设备.
    if ( m_iDeviceParamLen > 256 )
    {
        if ( S_OK != IPT_WriteDeviceWorkParam(m_rgbDeviceParam, m_rgbDeviceParam + 4, m_iDeviceParamLen - 4, m_pIPTControl) )
        {
            HV_Trace(5, "Initialized IPT WriteDeviceWorkParam error! SetParam error!\n");
        }
        else
        {
            HV_Trace(5, "IPT WriteDeviceWordParam ok ...\n");
        }
    }
    else
    {
        //先恢复默认再读取IPT参数保存到XML.
        if ( S_OK != IPT_ResetDefault(m_pIPTControl) )
        {
            HV_Trace(5, "Initialized IPT ResetDefault error! \n");
        }
        else
        {
            HV_Trace(5, "Initialized IPT ResetDefault ok... \n");
        }
        HV_Sleep(2000);

        //最长等待5秒
        int iTryTimes = 3;
        while ( true )
        {
            if ( iTryTimes <= 0 )
            {
                HV_Trace(5, "Initialized IPT ReadDeviceWorkParam error! \n");
                break;
            }
            if ( S_OK == IPT_ReadDeviceWorkParam(m_rgbDeviceParam, m_rgbDeviceParam + 4,  m_pIPTControl) )
            {
                m_iDeviceParamLen = 256 + 4;
                break;
            }
            iTryTimes--;
            HV_Sleep(1000);
        }

        if ( m_iDeviceParamLen > 256 )
        {
            g_cParamStore.SetBin(
                "IPTCtrl", "DeviceParam",
                m_rgbDeviceParam, m_iDeviceParamLen);
            g_cParamStore.Save(0);				// 保存配置信息
            HV_Trace(5, "IPT DeviceWordParam write to XML...\n");
        }
    }

    HV_Sleep(100);

    HRESULT hr(S_OK);
    BYTE8 bEventId;
    BYTE8 bType;
    BYTE8 bEventDeviceType;
    DWORD32 dwTime;
    BYTE8 rgbData[64];
    DWORD32 dwSize;
#if defined(SINGLE_BOARD_PLATFORM) || !defined(IPT_IN_MASTER)
    if (m_emOutPutPlateMode != PLATE_OUTPUT_SIGNAL)
    {
        m_fBeginIPT = true;
    }
#endif
    while (!m_fBeginIPT)
    {
        m_dwIPTThreadIsOkTime = GetSystemTick();
        HV_Sleep(100);
    }

    // 同步信号
    DWORD32 dwCurTime;
    while (true)
    {
        m_dwIPTThreadIsOkTime = GetSystemTick();
        dwCurTime = GetSystemTick();
        if (m_pIPTDevice != NULL)
        {
            hr = m_pIPTDevice->SyncTime(dwCurTime);
            if (SUCCEEDED(hr))
            {
                m_dwLastSyncTime = dwCurTime;		// 上次同步时间
                break;								// 同步成功才进入正式循环
            }
            HV_Trace(5, "sync IPT time.\n");
        }
        if (!m_fRunFlag)
        {
            break;
        }
        HV_Sleep(1000);
    }

    HV_Trace(5, "start IPT...\n");

    //IPT是否连接正常
    bool fIsConnected = true;
    int iSpaceTimeMs = 500;
    BYTE8 rgbOutPut[16];
    int iOutPutLen = 0;
    // 正式开始IPT线程
    while (m_fRunFlag)
    {
        HV_Sleep(iSpaceTimeMs);			// 每次休眠10MS
        m_dwIPTThreadIsOkTime = GetSystemTick();

        dwCurTime = m_dwIPTThreadIsOkTime;
        if ((dwCurTime - m_dwLastSyncTime) > IPT_CLOCK_SYNC_TIME
                || (!fIsConnected) )
        {
            // 同步时间
            HV_Trace(5, "sync IPT time.\n");
            hr = m_pIPTDevice->SyncTime(dwCurTime);
            if (SUCCEEDED(hr))
            {
                m_dwLastSyncTime = dwCurTime;		// 上次同步时间
                fIsConnected = true;
                HV_Sleep(10);
            }
            else
            {
                continue;
            }
        }

        memset(rgbOutPut, 0, 16);
        iOutPutLen = 1;
        //外总控开关是否打开
        if (m_emOutPutPlateMode == PLATE_OUTPUT_SIGNAL)
        {
            //IO输出
            int iOutSignalCount = g_queOutSignal.GetSize();
            for (int i = 0; i < iOutSignalCount; i++)
            {
                OUTPUT_SIGNAL output_signal = g_queOutSignal.RemoveHead();
                if (GetSystemTick() - output_signal.dwTime >= output_signal.dwDelay)
                {
                    int iRoad = output_signal.iRoadNumber;
                    bool fPutBack = false;
                    //找到车牌最近的信号源
                    for (int i = 0; i < g_nSignalSourceNum; i++)
                    {
                        for (int k = 0; k < MAX_IPT_PORT_NUM; k++)
                        {
                            bool fPortIsUsed = (m_rgIPTInfo[k].fUse && m_rgIPTInfo[k].iOutSigSource == i);
                            bool fMatchRoad = (g_prgSigSource[i].iRoad == iRoad || (g_prgSigSource[i].iRoad == 0xFF));
                            bool fMatchType = (g_prgMatchSigType[g_prgSigSource[i].nType].atType == ATTACH_INFO_LAST_IMAGE);
                            if ( fPortIsUsed && fMatchType && fMatchRoad )
                            {
                                BYTE8 bPort = 0x10;
                                bPort <<= k;
                                BYTE8 bLevel = 0x01;
                                bLevel <<= k;
                                //一次一个端口只能有一个输出
                                if ( (rgbOutPut[0] & bPort) == 0 )
                                {
                                    rgbOutPut[0] |= bPort;
                                    if ( m_rgIPTInfo[k].iOutSigLevel == 1 ) rgbOutPut[0] |= bLevel;
                                    iOutPutLen += 2;
                                }
                                else
                                {
                                    fPutBack = true;
                                    HV_Trace(1, "\n==TWIN==\n");
                                }
                                break;
                            }
                        }
                    }
                    //如果此信号未处理，则放回信号队列。
                    if ( fPutBack )
                    {
                        HV_Trace(1, "\n==RePutBack==\n");
                        g_queOutSignal.AddTail(output_signal);
                    }
                }
                else
                {
                    g_queOutSignal.AddTail(output_signal);
                }
            }
            if ( iOutPutLen > 1 )
            {
                int index = 1;
                BYTE8 bPort = 0x10;
                for (int i = 0; i < MAX_IPT_PORT_NUM; ++i)
                {
                    if (rgbOutPut[0] & bPort)
                    {
                        memcpy(rgbOutPut + index, (const void*)(&m_rgIPTInfo[i].iOutSigWidth), 2);
                        index += 2;
                    }
                    bPort <<= 1;
                }
            }
        }
        // 读取IPT事件队列
        dwSize = 64;
        hr =  m_pIPTDevice->ReadEventEx( rgbOutPut, iOutPutLen, &bEventId, &bType, &bEventDeviceType, &dwTime, rgbData, &dwSize);

        if (FAILED(hr))
        {
            HV_Trace(5, "read event failed.\n");
            // 读取事件失败则退出重连
            fIsConnected = false;
            continue;
        }

        if ( dwSize > 4 )
        {
            //更新IO输入状态
            m_bIOLevel = ~rgbData[4];
        }

        if ( (bEventId >= MAX_IPT_PORT_NUM) || m_rgIPTInfo[bEventId].fUse == false )
        {
            continue;  // 本信号没有使用，则继续下一个信号
        }
        HV_Trace(5, "bType=0x%02x, dwTime = %d\n", bType, dwTime);

        // 分析本次信号并处理
        SIGNAL_INFO temSignal[MAX_IPT_PIN_NUM];			// 可能两个针脚都会有跳变
        memset(&temSignal, 0, sizeof(SIGNAL_INFO) * MAX_IPT_PIN_NUM);

        int nSignalNum(0);								// 信号数量
        BYTE8 bTemp = 0x1;

        switch (bType)
        {
        case IPT_EVENT_IO_IN:			//IO口输入
            HV_Trace(5, "receive a io in signal.\n");
            g_dwSignalTime = GetSystemTick();

            for (int i = 0; i < MAX_IPT_PIN_NUM; i++)
            {
                // 0对应左边，1 对应右边
                if (rgbData[0] & bTemp)
                {
                    if ( m_rgIPTInfo[bEventId].rgSigSource[i] != -1 )
                    {
                        temSignal[nSignalNum].nType = m_rgIPTInfo[bEventId].rgSigSource[i];
                        temSignal[nSignalNum].dwSignalTime = dwTime;
                        temSignal[nSignalNum].dwInputTime = GetSystemTick();

                        temSignal[nSignalNum].dwValue = 0;
                        temSignal[nSignalNum].pImage = NULL;
                        temSignal[nSignalNum].pImageLast = NULL;

                        if (m_fHardTriggerCap)
                        {
                            static DWORD32 dwArriveTimes = 0;
                            dwArriveTimes++;
                            if (dwArriveTimes >= 0xffff)
                            {
                                dwArriveTimes = 1;
                            }
                            int iRoadNumber = dwArriveTimes;
                            iRoadNumber = (iRoadNumber << 16) | g_prgSigSource[temSignal[nSignalNum].nType].iRoad;

                            if (GetSystemTick() >= m_dwLastCarArriveTime)
                            {
                                //todo:
                                //g_cCamTrigger.HardTrigger(iRoadNumber);
                            }
                            else
                            {
                                if (dwArriveTimes == 1)
                                {
                                    g_dwCarArriveTimes = 0xfffe;
                                }
                                else
                                {
                                    g_dwCarArriveTimes = dwArriveTimes - 1;
                                }
                                g_dwRoadFlag = iRoadNumber;
                            }

                            m_dwLastCarArriveTime = GetSystemTick() + 100;

                            HV_Trace(3, "hard trigger!%d:%d\r\n",i,iRoadNumber);
                        }

                        nSignalNum++;
                    }
                }
                bTemp = bTemp << 1;			// 左移检测下一位
            }
            break;
        case IPT_EVENT_CIRCLE:			//线圈测速
            HV_Trace(5, "receive a circle signal, direction=%d, calc speed=%d.\n", rgbData[1], rgbData[0]);
            if (rgbData[1] == 0)
            {
                if ( m_rgIPTInfo[bEventId].iComSource != -1 )
                {
                    // 正向行驶才会出速度
                    temSignal[nSignalNum].nType = m_rgIPTInfo[bEventId].iComSource;
                    temSignal[nSignalNum].dwSignalTime = dwTime;
                    temSignal[nSignalNum].dwInputTime = GetSystemTick();

                    temSignal[nSignalNum].dwValue = rgbData[0];
                    temSignal[nSignalNum].pImage = NULL;
                    temSignal[nSignalNum].pImageLast = NULL;

                    nSignalNum++;
                }
            }
            break;
        case IPT_EVENT_IO_OUT:			//IO口输出
            break;
        case IPT_EVENT_RADAR:			//雷达测速
            if ( m_rgIPTInfo[bEventId].iComSource != -1 )
            {
                temSignal[nSignalNum].nType = m_rgIPTInfo[bEventId].iComSource;
                temSignal[nSignalNum].dwSignalTime = dwTime;
                temSignal[nSignalNum].dwInputTime = GetSystemTick();

                temSignal[nSignalNum].dwValue = rgbData[0];
                temSignal[nSignalNum].pImage = NULL;
                temSignal[nSignalNum].pImageLast = NULL;

                if ( dwSize >= 3 && bEventDeviceType == 0x03)
                {
                    temSignal[nSignalNum].dwRoad |= rgbData[2];
                    temSignal[nSignalNum].dwRoad |= 0x00FF0000;
                }
                nSignalNum++;
            }
            break;
        default:
            break;
        }

        if (nSignalNum == 0)
        {
            continue;						// 没有建立信号，则继续读取下一事件
        }

        // 发送信号
        for (int i = 0; i < nSignalNum; i++)
        {
            AppendSignal(&temSignal[i]);
        }
    } // while (m_fRunFlag)

    // 释放所有IPT使用的信号源队列
    ClearSignalQueue();

    return S_OK;
}

void COuterControlImpl::SetIPTParam(BYTE8 * pbData, int size)
{
    g_cParamStore.SetBin(
        "IPTCtrl", "DeviceParam",
        pbData, size);
    g_cParamStore.Save(0);				// 保存配置信息
    HV_Trace(5, "IPT DeviceWordParam write to XML...\n");
}

void COuterControlImpl::SetEventData(BYTE8 * pbData, int size)
{
    BYTE8 * buf = pbData;
    BYTE8 * bEventId = buf;
    buf++;
    BYTE8 * bType = buf;
    buf++;
    BYTE8 * bEventDeviceType = buf;
    buf++;
    DWORD32 dwTime;
    memcpy(&dwTime, buf, sizeof(DWORD32));
    buf += sizeof(DWORD32);
    BYTE8 * rgbData = buf;
    DWORD32 dwSize = size - 7;
    //因为主从的时间没有实时同步，因此需要替换主的时间为从的时间，确保信号和车牌的时间误差不能太大。
    dwTime = GetSystemTick();
    HV_Trace(5, "receive a signal,EventId=%d, Type=%d, EventDeviceType=%d, Time=%d\n", *bEventId, *bType, *bEventDeviceType, dwTime);
    if (*bType == 0x08 && *bEventDeviceType == 0x0b)
    {
        HV_Trace(5, "receive an io event from radar signal,data=%d\n", rgbData[0]);
        m_bIOLevel = rgbData[0];
    }
    //只要有线圈触发信号，则认为线圈触发正常
    if (*bType == IPT_EVENT_IO_IN && m_fSwitchToVideoDetect && m_cSignalMatchParam.nSwitchFlashLightCount > 0)
    {
        m_dwCarLeftCount = 0;
        m_dwCaptureImageCount = 0;
        m_fSwitchToVideoDetect = FALSE;
        int nType = 1;
        g_cHvPciLinkApi.SendData(PCILINK_FLASHLIGHT_TYPE, &nType, sizeof(nType));
    }

    if (!m_rgIPTInfo[*bEventId].fUse)
    {
        return;
    }

    // 分析本次信号并处理
    SIGNAL_INFO temSignal[MAX_IPT_PIN_NUM];			// 可能两个针脚都会有跳变
    memset(&temSignal, 0, sizeof(SIGNAL_INFO) * MAX_IPT_PIN_NUM);
    int nSignalNum(0);								// 信号数量
    BYTE8 bTemp = 0x1;
    switch (*bType)
    {
    case IPT_EVENT_IO_IN:			//IO口输入
        HV_Trace(5, "receive a io in signal,rgbData[0]=%d, rgbData[1]=%d.\n", rgbData[0], rgbData[1]);
        g_dwSignalTime = GetSystemTick();

        for (int i = 0; i < MAX_IPT_PIN_NUM; i++)
        {
            // 0对应左边，1 对应右边
            if (rgbData[0] & bTemp)
            {
                if ( m_rgIPTInfo[*bEventId].rgSigSource[i] != -1 )
                {
                    temSignal[nSignalNum].nType = m_rgIPTInfo[*bEventId].rgSigSource[i];
                    temSignal[nSignalNum].dwSignalTime = dwTime;
                    temSignal[nSignalNum].dwInputTime = GetSystemTick();

                    temSignal[nSignalNum].dwValue = 0;
                    temSignal[nSignalNum].pImage = NULL;
                    temSignal[nSignalNum].pImageLast = NULL;

                    if (m_fHardTriggerCap)
                    {
                        static DWORD32 dwArriveTimes = 0;
                        dwArriveTimes++;
                        if (dwArriveTimes >= 0xffff)
                        {
                            dwArriveTimes = 1;
                        }
                        int iRoadNumber = dwArriveTimes;
                        iRoadNumber = (iRoadNumber << 16) | g_prgSigSource[temSignal[nSignalNum].nType].iRoad;

                        if (GetSystemTick() >= m_dwLastCarArriveTime)
                        {
                            //todo:
                            //g_cCamTrigger.HardTrigger(iRoadNumber);
                        }
                        else
                        {
                            if (dwArriveTimes == 1)
                            {
                                g_dwCarArriveTimes = 0xfffe;
                            }
                            else
                            {
                                g_dwCarArriveTimes = dwArriveTimes - 1;
                            }
                            g_dwRoadFlag = iRoadNumber;
                        }

                        m_dwLastCarArriveTime = GetSystemTick() + 100;

                        HV_Trace(3, "hard trigger!%d:%d\r\n",i,iRoadNumber);
                    }

                    nSignalNum++;
                }
            }
            bTemp = bTemp << 1;			// 左移检测下一位
        }
        break;
    case IPT_EVENT_CIRCLE:			//线圈测速
        HV_Trace(5, "receive a circle signal, direction=%d, calc speed=%d.\n", rgbData[1], rgbData[0]);
        if (rgbData[1] == 0)
        {
            if ( m_rgIPTInfo[*bEventId].iComSource != -1 )
            {
                // 正向行驶才会出速度
                temSignal[nSignalNum].nType = m_rgIPTInfo[*bEventId].iComSource;
                temSignal[nSignalNum].dwSignalTime = dwTime;
                temSignal[nSignalNum].dwInputTime = GetSystemTick();

                temSignal[nSignalNum].dwValue = rgbData[0];
                temSignal[nSignalNum].pImage = NULL;
                temSignal[nSignalNum].pImageLast = NULL;

                nSignalNum++;
            }
        }
        break;
    case IPT_EVENT_RADAR:			//雷达测速
        HV_Trace(5, "receive a radar signal,speed=%d\n", rgbData[0]);
        if ( m_rgIPTInfo[*bEventId].iComSource != -1 )
        {
            temSignal[nSignalNum].nType = m_rgIPTInfo[*bEventId].iComSource;
            temSignal[nSignalNum].dwSignalTime = dwTime;
            temSignal[nSignalNum].dwInputTime = GetSystemTick();

            temSignal[nSignalNum].dwValue = rgbData[0];
            temSignal[nSignalNum].pImage = NULL;
            temSignal[nSignalNum].pImageLast = NULL;

            if ( dwSize >= 3 && *bEventDeviceType == 0x03)
            {
                temSignal[nSignalNum].dwRoad |= rgbData[2];
                temSignal[nSignalNum].dwRoad |= 0x00FF0000;
            }
            nSignalNum++;
        }
        break;
    }
    // 发送信号
    for (int i = 0; i < nSignalNum; i++)
    {
        AppendSignal(&temSignal[i]);
    }
}

// 线程是否正常
bool COuterControlImpl::ThreadIsOk()
{
    bool fRtn = false;
    //如果IPT线程没有运行
    if (m_emOutPutPlateMode != PLATE_OUTPUT_SIGNAL)
    {
        fRtn = true;
        return fRtn;
    }

    DWORD32 dwCurTick = GetSystemTick();
    if ( GetCurrentMode() == PRM_HVC )
    {
        if ( (dwCurTick < m_dwOuterThreadIsOkTime)
                || ((dwCurTick - m_dwOuterThreadIsOkTime) < 20000) )
        {
            fRtn = true;
        }
    }
    else
    {
        if ( (dwCurTick < m_dwOuterThreadIsOkTime)
                || ((dwCurTick - m_dwOuterThreadIsOkTime) < 20000) )
        {
            if ( (dwCurTick < m_dwFramThreadIsOkTime)
                    || ((dwCurTick - m_dwFramThreadIsOkTime) < 20000) )
            {
                fRtn = true;
            }
        }
        if (!m_dwOuterThreadIsOkTime)
        {
            fRtn = true;
        }
    }

    return fRtn;
}

// 将信号加入信号队列
HRESULT COuterControlImpl::AppendSignal(SIGNAL_INFO *pSignalInfo)
{
    if (m_emOutPutPlateMode != PLATE_OUTPUT_SIGNAL)
    {
        return S_FALSE;
    }
    if (!g_fOuterOK)
    {
        return E_NOTIMPL;
    }
    if (pSignalInfo == NULL)
    {
        return E_POINTER;
    }
    int nSourceType = pSignalInfo->nType;
    if ((nSourceType < 0) || (nSourceType >= g_nSignalSourceNum))
    {
        HV_Trace(5, "nSourceType error\n");
        return E_INVALIDARG;
    }
    // 拷贝数据
    SIGNAL_INFO *pTempSignalInfo = new SIGNAL_INFO;
    if (pTempSignalInfo == NULL)
    {
        HV_Trace(5, "pTempSignalInfo == null\n");
        return E_OUTOFMEMORY;
    }
    *pTempSignalInfo = *pSignalInfo;  // 拷贝数据
    pTempSignalInfo->nStatus = STATUS_INIT;

    bool fIsFull = false;
    LockSignal(nSourceType);
    if (g_prgQueSignal[nSourceType].GetSize() >= SIGNAL_LIST_COUNT)
    {
        fIsFull = true;
    }
    if (!fIsFull)
    {
        if (pTempSignalInfo->pImageLast)
        {
            m_dwCaptureImageCount++;
        }

        //识别抓拍图
        if (IsNeedToRecogSnapImage() && m_pVideoRecoger && pTempSignalInfo->pImageLast)
        {
            // todo... 为什么是要大于100.
            static DWORD32 dwLastImageTime = 0;
            if (g_iCapImgCount != 2 || pTempSignalInfo->pImageLast->GetRefTime() - dwLastImageTime > 100)
            {
                dwLastImageTime = pTempSignalInfo->pImageLast->GetRefTime();
                DWORD32 tNow = GetSystemTick();
                m_pVideoRecoger->ProcessOneFrame(
                    0,
                    pTempSignalInfo->pImageLast,
                    NULL,
                    NULL,
                    &(pTempSignalInfo->cProcessEvent)
                );
                if (pTempSignalInfo->cProcessEvent.dwEventId & EVENT_CARLEFT)
                {
                    for (int i = 0; i < pTempSignalInfo->cProcessEvent.iCarLeftInfoCount; i++)
                    {
                        CRect *rc = pTempSignalInfo->cProcessEvent.rgCarLeftInfo[i].cCoreResult.rgFaceRect;
                        for (int j = 0; j < pTempSignalInfo->cProcessEvent.rgCarLeftInfo[i].cCoreResult.nFaceCount; j++)
                        {
                            HV_Trace(5, "face rect:(%d,%d,%d,%d)", rc[j].left, rc[j].top, rc[j].right, rc[j].bottom);
                        }
                    }
                }
            }
        }
        // 将非JPEG图片编码为JPEG
        ConvertToJpeg(&pTempSignalInfo->pImage);
        ConvertToJpeg(&pTempSignalInfo->pImageLast);
        // 将信号加入信号队列
        g_prgQueSignal[nSourceType].AddTail(pTempSignalInfo);
        char szTemp[255];
        sprintf(szTemp, "Signal: %d, %d, %d \n"
                 ,pTempSignalInfo->dwFlag
                 ,pTempSignalInfo->dwSignalTime
                 ,pTempSignalInfo->dwInputTime);
        HV_Trace(5, szTemp);
        PciSendSlaveDebugInfo(szTemp);

        g_nTestCount++;
    }
    else
    {
        HV_Trace(5, "Signal full: %d, %d \n", pTempSignalInfo->dwSignalTime, pTempSignalInfo->dwInputTime);
        ReleaseSignalInfo(pTempSignalInfo);
    }
    UnlockSignal(nSourceType);

    return S_OK;
}

// 取得所有信号的数量
int COuterControlImpl::GetSignalNum()
{
    int nCount(0);
    for (int i = 0; i < g_nSignalSourceNum; i++)
    {
        nCount += g_prgQueSignal[i].GetSize();
    }
    return nCount;
}

// 释放车牌数据
void COuterControlImpl::ReleasePlateInfo(PLATE_INFO_hxl *pPlateInfo)
{
    if (pPlateInfo != NULL)
    {
        FreeCarLeftInfoStruct(pPlateInfo->pCarleftInfo);
        if (pPlateInfo->pCarleftInfo != NULL)
        {
            delete pPlateInfo->pCarleftInfo;
        }
        if (pPlateInfo->pvUserData != NULL)
        {
            delete[] (PBYTE8)pPlateInfo->pvUserData;
        }
        delete pPlateInfo;
    }
}

// 释放信号数据
void COuterControlImpl::ReleaseSignalInfo(SIGNAL_INFO *pSignalInfo)
{
    if (pSignalInfo != NULL)
    {
        if (pSignalInfo->pImage != NULL)
        {
            pSignalInfo->pImage->Release();
            pSignalInfo->pImage = NULL;
        }

        if ((pSignalInfo->pImageLast != NULL)
                && (g_prgMatchSigType[g_prgSigSource[pSignalInfo->nType].nType].atType
                    == ATTACH_INFO_LAST_IMAGE))
        {
            pSignalInfo->pImageLast->Release();
            pSignalInfo->pImageLast = NULL;
        }

        ReleaseProcessEvent(&(pSignalInfo->cProcessEvent));

        delete pSignalInfo;
    }
}

//判断车牌是哪个车道的
int COuterControlImpl::MatchRoad(CARLEFT_INFO_STRUCT *pPlateInfo)
{
    int iRet = -1;
    return iRet;
}

//判断车牌是哪个车道的
int COuterControlImpl::MatchRoad(int iX1, int iY1, int iX2, int iY2)
{
    int iRet = -1;
    return iRet;
}

//将CarLeft信息放入发送队列
HRESULT COuterControlImpl::SendMatchInfo(
    CARLEFT_INFO_STRUCT *pCarLeftInfo,
    LPVOID pvUserData
)
{
    LockCore();
    if (m_queCarLeft.GetSize() >= MAX_CARLEFT_COUNT)
    {
        HV_Trace(5, "CarLeft SendQueue Is Full...\n");
        UnlockCore();
        return E_FAIL;
    }

    CARLEFT_INFO_STRUCT *pTempCarLeftInfo = new CARLEFT_INFO_STRUCT();
    if (pTempCarLeftInfo == NULL)
    {
        UnlockCore();
        return E_FAIL;
    }

    PLATE_INFO_hxl *pTempPlateInfo = new PLATE_INFO_hxl();
    if (pTempPlateInfo == NULL)
    {
        UnlockCore();
        delete pTempCarLeftInfo;
        return E_FAIL;
    }

    // 复制结构数据，复制的时候，图片已经加了引用计数
    CopyCarLeftInfoStruct(pTempCarLeftInfo, pCarLeftInfo);

    pTempPlateInfo->pCarleftInfo = pTempCarLeftInfo;
    pTempPlateInfo->pvUserData = pvUserData;

    m_queCarLeft.AddTail(pTempPlateInfo);
    UnlockCore();

    return S_OK;
}
//因为图片的信号源没有IP地址对应，因此无法得到多个图片信号源的下标
//目前只能暂时只能添加1个图片信号源
int COuterControlImpl::GetImageSourceIndex(void)
{
    static int iImageSouceIndex = -1;
    if (iImageSouceIndex < 0)
    {
        int *iIndex = new int[g_nSignalSourceNum];
        memset(iIndex, 0, sizeof(int)*g_nSignalSourceNum);
        //标志已经占用的iSourceInex;
        for (int i = 0; i < 4; i++)
        {
            if (m_rgIPTInfo[i].rgSigSource[0] >= 0)
            {
                iIndex[m_rgIPTInfo[i].rgSigSource[0]] = 1;
            }
            if (m_rgIPTInfo[i].rgSigSource[1] >= 0)
            {
                iIndex[m_rgIPTInfo[i].rgSigSource[1]] = 1;
            }
            if (m_rgIPTInfo[i].iComSource >= 0)
            {
                iIndex[m_rgIPTInfo[i].iComSource] = 1;
            }
        }
        //查找未被占用的SourceIndex
        for (int i = 0; i < g_nSignalSourceNum; i++)
        {
            if (!iIndex[i])
            {
                iImageSouceIndex = i;
                break;
            }
        }
        delete []iIndex;
    }
    return iImageSouceIndex;
}

int COuterControlImpl::GetSoftTrigSignalSource()
{
    return m_cSignalMatchParam.nSoftTrigSignalSource;
}

HRESULT COuterControlImpl::AddPanoramicSignal(SIGNAL_INFO *pSignalInfo)
{
    if (!g_fOuterOK)
    {
        return E_NOTIMPL;
    }
    if (pSignalInfo == NULL)
    {
        return E_POINTER;
    }
    if (pSignalInfo->nType != 1200)
    {
        return E_FAIL;
    }

    SIGNAL_INFO* pTempSignalInfo = new SIGNAL_INFO;
    if (pTempSignalInfo == NULL)
    {
        HV_Trace(5, "pTempSignalInfo == NULL\n");
        return E_OUTOFMEMORY;
    }

    *pTempSignalInfo = *pSignalInfo;
    pTempSignalInfo->nStatus = STATUS_INIT;

    bool fIsFull = false;
    SemPend(&g_SemPanormicSignal);
    if (g_PanormicSignal.GetSize() >= SIGNAL_LIST_COUNT)
    {
        fIsFull = true;
    }


    if (!fIsFull)
    {
        g_PanormicSignal.AddTail(pTempSignalInfo);
    }
    else
    {
        if (pTempSignalInfo->pImageLast)
        {
            pTempSignalInfo->pImageLast->Release();
            pTempSignalInfo->pImageLast = NULL;
        }
        delete pTempSignalInfo;
    }

    SemPost(&g_SemPanormicSignal);
    return S_OK;
}

HRESULT COuterControlImpl::MatchPanoramicSignal(DWORD32 dwPlateTime, IReferenceComponentImage** pRetImageFRame)
{
    HVPOSITION nPos;
    bool bIsMacth = false;
    MATCH_ANALYSIS cMatchInfo;
    SIGNAL_INFO* pMatchSignalInfo = NULL;
    cMatchInfo.nSignalPos = -1;
    cMatchInfo.nSignalPos_2 = -1;

    SemPend(&g_SemPanormicSignal);
    nPos = g_PanormicSignal.GetHeadPosition();
    while (g_PanormicSignal.IsValidPos(nPos))
    {
        HVPOSITION nCurSigPos = nPos;
        SIGNAL_INFO* pSignalInfo = g_PanormicSignal.GetNext(nPos);
        long lTimeDiff(0);
        DWORD32 dwPrevTime = pSignalInfo->dwInputTime - 200;
        DWORD32 dwPostTime = pSignalInfo->dwInputTime + 4000;

        if (dwPlateTime >= dwPrevTime && dwPlateTime <= dwPostTime)
        {
            lTimeDiff = (long)dwPlateTime - (long)pSignalInfo->dwSignalTime;
            bool fIsUpdate = false;
            if (bIsMacth && lTimeDiff < 0 && cMatchInfo.lTimeDiff > 0 && abs(lTimeDiff) < 180)
            {
                fIsUpdate = true;
            }
            bool fUnUpDate = false;
            if (bIsMacth && lTimeDiff > 0 && cMatchInfo.lTimeDiff < 0 && abs(cMatchInfo.lTimeDiff) < 150)
            {
                fUnUpDate = true;
            }

            if (!bIsMacth
                    || ( (abs(lTimeDiff) < abs(cMatchInfo.lTimeDiff))
                         && (abs(lTimeDiff) > 80) && !fUnUpDate )
                    || fIsUpdate )
            {
                cMatchInfo.nSignalPos = nCurSigPos;
                cMatchInfo.lTimeDiff = lTimeDiff;
                pMatchSignalInfo = pSignalInfo;
                bIsMacth = true;
            }
        }
    }
    SemPost(&g_SemPanormicSignal);

    if (pMatchSignalInfo)
    {
        *pRetImageFRame = pMatchSignalInfo->pImageLast;
        return S_OK;
    }
    return E_FAIL;
}
