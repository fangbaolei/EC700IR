#include "TLAppDetCtrl.h"
#include "SWFC.h"
#include "hvutils.h"
#include "TLDetResult.h" 
#include "TLDetModelData.h"

//#define TEST_DET_PLATE
#define TEST_DET_CAR

static int svTraceCallback(
    const char* szInfo,     ///< 字符串
    int nLen                ///< 字符串长度+1
    )
{
#if RUN_PLATFORM == PLATFORM_WINDOWS
    printf(szInfo);

#if 1  // 输出到文件
    static FILE* s_fileSVTrace = NULL;
    if (s_fileSVTrace == NULL)
    {
        s_fileSVTrace = fopen("U:\\DJLog.txt", "w"); 
    }

    if (s_fileSVTrace)
    {
        fwrite(szInfo, 1, strlen(szInfo), s_fileSVTrace);
        fflush(s_fileSVTrace);
    }
#endif
#else
    // TODO打印
//    SW_TRACE_DEBUG("<EP>:%s.", szInfo);
#endif

    return 0;
}

#if 1//SV_RUN_PLATFORM == SV_PLATFORM_LINUX


//using namespace svPlateApi;
const int MAX_MEM_LIST_CNT = 256;
static CSWMemory* g_rgMemList[MAX_MEM_LIST_CNT] = {0};

static void* SV_MEM_ALLOC(
    int nSize  ///< 申请长度
)
{
    for (int i=0; i<MAX_MEM_LIST_CNT; ++i )
    {
       if (g_rgMemList[i] != NULL)  continue;

       g_rgMemList[i] = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY)->Alloc(
           nSize);

       CSWMemory*& pMem = g_rgMemList[i];                                  ;

       if (pMem != NULL)
       {
           //sv::utTrace("alloc Mem addr:%08X phy:%08X\n", pMem->GetBuffer(0), pMem->GetBuffer(1));
           return pMem->GetBuffer(0);
       }
       else
       {
           sv::utTrace("alloc %d failed.\n", nSize);
           return NULL;
       }
    }
    return NULL; 
	//return malloc(nSize);
}

static void* SV_FAST_MEM_ALLOC(
    int nSize  ///< 申请长度
    )
{
    return malloc(nSize);
}

static void SV_MEM_FREE(
    void* pAddr,                    ///< 要释放的地址
    int nSize                       ///< 内存长度
) 
{
    for (int i=0; i<MAX_MEM_LIST_CNT; ++i )
    {
        if (g_rgMemList[i] != NULL)
        {
            CSWMemory*& pMem = g_rgMemList[i]; 
            if (pMem->GetBuffer(0) == pAddr)
            {
               CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY)->Free(pMem);
               pMem = NULL;
               break;
            }
        }
    }
// 	if( pAddr != NULL )
// 	{
// 		free(pAddr);
// 	}
}
static void SV_FAST_MEM_FREE(
    void* pAddr,                    ///< 要释放的地址
    int nSize                       ///< 内存长度
    ) 
{
    if( pAddr != NULL )
    {
        free(pAddr);
    }
}

static unsigned int svGetSysTimeCallBack()
{
    return CSWDateTime::GetSystemTick();
}

#endif

#if SV_RUN_PLATFORM == SV_PLATFORM_WIN

#include "ExLib.h"
static void ShowROI(
    const sv::SV_IMAGE* pImg,
    svTLDetApi::SV_PLATERECT* rgROI,
    int iCount,
    int iMs,
    char* szName
    )
{
    sv::SV_RECT* rgrcInfo = new sv::SV_RECT[iCount];
    sv::SV_RECT* rgrcInfo2 = new sv::SV_RECT[iCount];
    int iInfo1 = 0;
    int iInfo2 = 0;

    for(int i = 0; i < iCount; ++i)
    {
        rgrcInfo[iInfo1++] = rgROI[i].cPlatePos;
    }

    ShowRectIntx2Img(pImg, rgrcInfo, NULL, iInfo1, rgrcInfo2, NULL, iInfo2, iMs, szName);
    delete [] rgrcInfo;
    delete [] rgrcInfo2;
}
#endif

static HRESULT Svresult2Hresult(sv::SV_RESULT svrt)
{
    HRESULT hr;
    switch(svrt)
    {
    case sv::RS_S_OK:
        hr = S_OK;
        break;
    case sv::RS_S_FALSE:
        hr = S_FALSE;
        break;
    case sv::RS_E_FAIL:
        hr = E_FAIL;
        break;
    case sv::RS_E_OUTOFMEMORY:
        hr = E_OUTOFMEMORY;
        break;
    case sv::RS_E_INVALIDARG:
        hr = E_INVALIDARG;
        break;
    case sv::RS_E_UNEXPECTED:
        hr = E_UNEXPECTED;
        break;
    case sv::RS_E_OBJNOINIT:
        hr = E_OBJ_NO_INIT;
        break;
    case sv::RS_E_NOTIMPL:
        hr = E_NOTIMPL;
        break;
    default :
        hr = E_UNEXPECTED;
        break;
    }

    return hr;
}

static inline sv::SV_IMAGE_TYPE HvimgType2SvimgType(int nHvType)
{
    sv::SV_IMAGE_TYPE svType = sv::SV_IMAGE_UNKNOWN;
    if (nHvType == HV_IMAGE_YUV_422)
    {
        svType = sv::SV_IMAGE_YUV422;
    }
    else if (nHvType == HV_IMAGE_YCbYCr 
        || nHvType == HV_IMAGE_CbYCrY 
        || nHvType == HV_IMAGE_BT1120
        || nHvType == HV_IMAGE_BT1120_FIELD
        || nHvType == HV_IMAGE_YUV_420)
    {
        svType = (sv::SV_IMAGE_TYPE)(nHvType + 100);
    }
    return svType;
}

#ifndef RTN_HR_IF_SVFAILED
#define RTN_HR_IF_SVFAILED(fun)\
{ HRESULT hr = Svresult2Hresult(fun); if(FAILED(hr)) return hr;}
#endif

CTLAppDetCtrl::CTLAppDetCtrl(void)
    : m_pTLDetCtrl(NULL)
    , m_pTLCarDetCtrl(NULL)
{
}


CTLAppDetCtrl::~CTLAppDetCtrl(void)
{
}

HRESULT CTLAppDetCtrl::Init(
    TRACKER_CFG_PARAM* pCfgParam
    )
{
    sv::utSetTraceCallBack_TXT(svTraceCallback);
#if 1
    sv::SetCallBack_GetSysTime(svGetSysTimeCallBack);

    sv::SetCallBack_MemAlloc(SV_MEM_ALLOC);
    sv::SetCallBack_MemFree(SV_MEM_FREE);
    sv::SetCallBack_FastMemAlloc(SV_FAST_MEM_ALLOC);
    sv::SetCallBack_FastMemFree(SV_FAST_MEM_FREE);
#endif

    m_nFrameWidth = 0;
    m_nFrameHeight = 0;

#ifdef TEST_DET_PLATE
    // 绝对坐标，临时存到 m_cTLParam.nRoadXX 上
    m_cTLParam.nRoadX00 = pCfgParam->rgcRoadInfo[0].ptTop.x;
    m_cTLParam.nRoadY00 = pCfgParam->rgcRoadInfo[0].ptTop.y;
    m_cTLParam.nRoadX01 = pCfgParam->rgcRoadInfo[0].ptBottom.x;
    m_cTLParam.nRoadY01 = pCfgParam->rgcRoadInfo[0].ptBottom.y;
    m_cTLParam.nRoadX10 = pCfgParam->rgcRoadInfo[1].ptTop.x;
    m_cTLParam.nRoadY10 = pCfgParam->rgcRoadInfo[1].ptTop.y;
    m_cTLParam.nRoadX11 = pCfgParam->rgcRoadInfo[1].ptBottom.x;
    m_cTLParam.nRoadY11 = pCfgParam->rgcRoadInfo[1].ptBottom.y;

#if SV_PLATFORM_LINUX == SV_RUN_PLATFORM
    m_cTLCarDetParam.nRoadY00 >>= 1;
    m_cTLCarDetParam.nRoadY01 >>= 1;
    m_cTLCarDetParam.nRoadY10 >>= 1;
    m_cTLCarDetParam.nRoadY11 >>= 1;
#endif

    m_cTLParam.nStartDetLine = pCfgParam->nCarArrivedPos - 40;
    m_cTLParam.nEndDetLine = pCfgParam->nCarArrivedPos;

    m_cTLParam.nDetMinScaleNum = pCfgParam->cDetectArea.nDetectorMinScaleNum;
    m_cTLParam.nDetMaxScaleNum = pCfgParam->cDetectArea.nDetectorMaxScaleNum;

    // create ctrl
    if (m_pTLDetCtrl != NULL)
    {
        FreeTLDetCtrl(m_pTLDetCtrl);
    }
    m_pTLDetCtrl = svTLDetApi::CreateTLDetCtrl();

    if (m_pTLDetCtrl == NULL)
    {
        return E_OUTOFMEMORY;
    }
#endif

#ifdef TEST_DET_CAR
    m_cTLCarDetParam.nRoadX00 = pCfgParam->rgcRoadInfo[0].ptTop.x;
    m_cTLCarDetParam.nRoadY00 = pCfgParam->rgcRoadInfo[0].ptTop.y;
    m_cTLCarDetParam.nRoadX01 = pCfgParam->rgcRoadInfo[0].ptBottom.x;
    m_cTLCarDetParam.nRoadY01 = pCfgParam->rgcRoadInfo[0].ptBottom.y;
    m_cTLCarDetParam.nRoadX10 = pCfgParam->rgcRoadInfo[1].ptTop.x;
    m_cTLCarDetParam.nRoadY10 = pCfgParam->rgcRoadInfo[1].ptTop.y;
    m_cTLCarDetParam.nRoadX11 = pCfgParam->rgcRoadInfo[1].ptBottom.x;
    m_cTLCarDetParam.nRoadY11 = pCfgParam->rgcRoadInfo[1].ptBottom.y;

    // 不使用夜晚模型
    m_cTLCarDetParam.nNightEnvLightTh = 1;
      
#if SV_PLATFORM_LINUX == SV_RUN_PLATFORM
    m_cTLCarDetParam.nRoadY00 >>= 1;
    m_cTLCarDetParam.nRoadY01 >>= 1;
    m_cTLCarDetParam.nRoadY10 >>= 1;
    m_cTLCarDetParam.nRoadY11 >>= 1;
#endif
    sv::utTrace("TLCtrl [%d %d] [%d %d] [%d %d] [%d %d]\n", m_cTLCarDetParam.nRoadX00, m_cTLCarDetParam.nRoadY00,
            m_cTLCarDetParam.nRoadX01, m_cTLCarDetParam.nRoadY01,
            m_cTLCarDetParam.nRoadX10, m_cTLCarDetParam.nRoadY10,
            m_cTLCarDetParam.nRoadX11, m_cTLCarDetParam.nRoadY11
            );

    // 无牌检查区必须包含在有牌检测区内，防止无牌先出
    m_cTLCarDetParam.nStartDetLine = pCfgParam->nCarArrivedPos - pCfgParam->iScanAreaTopOffset + 10; //pCfgParam->nCarArrivedPos - 22;
    m_cTLCarDetParam.nEndDetLine = pCfgParam->nCarArrivedPos + pCfgParam->iScanAreaBottomOffset + 8;//pCfgParam->nCarArrivedPos + 20;

    m_cTLCarDetParam.fltXScale = 0.25f;
    m_cTLCarDetParam.fltYScale = 0.5f;

    // create ctrl
    if (m_pTLCarDetCtrl != NULL)
    {
        FreeTLCarDetCtrl(m_pTLCarDetCtrl);
    }
    m_pTLCarDetCtrl = svTLDetApi::CreateTLCarDetCtrl();
    if (m_pTLCarDetCtrl == NULL)
    {
        return E_OUTOFMEMORY;
    }

    //  model param , load
#define SV_INIT_MOD_DET_PARAM(model, pbdata, datelen, dettype, divx, dixy, scalenum, scalecoef, mergenum, maxroi, roadratio)\
    model.pbData = pbdata;\
    model.nDataLen = datelen;\
    model.nDetType = dettype;\
    model.nStepDivX = divx;\
    model.nStepDivY = dixy;\
    model.nScaleNum = scalenum;\
    model.fltScaleCoef = scalecoef;\
    model.nMergeNum = mergenum;\
    model.nMaxROI = maxroi;\
    model.fltRoadRatio = roadratio

    // 白天小车参数
    SV_INIT_MOD_DET_PARAM(
        m_rgModelParam[0],
        svTLDetData::g_cTLDetModDaySmall.pbData, 
        svTLDetData::g_cTLDetModDaySmall.nDataLen,
        svTLDetApi::MOD_DET_INFO::DAY_SMALL_CAR,
        16, 8, 4, 0.94f, 2, 512, 0.7f
        );
    // 晚上小车参数
    SV_INIT_MOD_DET_PARAM(
        m_rgModelParam[1], 
        svTLDetData::g_cTLDetModNightSmall.pbData, 
        svTLDetData::g_cTLDetModNightSmall.nDataLen,
        svTLDetApi::MOD_DET_INFO::NIGHT_SMALL_CAR,
        16, 8, 2, 0.9f,  2, 512, 0.7f
        );

    for (int i=0; i<MAX_MOD_DET_INFO; ++i)
    {
        RTN_HR_IF_SVFAILED(m_pTLCarDetCtrl->LoadDetModel(&m_rgModelParam[i]));
    }


#endif

    return S_OK;
}

HRESULT CTLAppDetCtrl::Uninit()
{

#ifdef TEST_DET_PLATE
    if (NULL != m_pTLDetCtrl)
    {
        svTLDetApi::FreeTLDetCtrl(m_pTLDetCtrl);
        m_pTLDetCtrl = NULL;
    }
#endif

#ifdef TEST_DET_CAR
    if (NULL != m_pTLCarDetCtrl)
    {
        svTLDetApi::FreeTLCarDetCtrl(m_pTLCarDetCtrl);
        m_pTLCarDetCtrl = NULL;
    }
#endif

    return S_OK;
}

HRESULT CTLAppDetCtrl::Process(
    const HV_COMPONENT_IMAGE& hvImgFrame,
    void* pvSyncData,
    int nMaxSyncDataSize,
    int* pnSyncDateSize
    )
{
#ifdef TEST_DET_PLATE
    if (m_pTLDetCtrl == NULL)   return E_FAIL;
#endif
#ifdef TEST_DET_CAR
    if (m_pTLCarDetCtrl == NULL)   return E_FAIL;
#endif
  //   SW_TRACE_DEBUG("EP image type:%d,w:%d,h:%d, str:%d,%d,%d.",
  //   		hvImgFrame.nImgType, hvImgFrame.iWidth, hvImgFrame.iHeight,
  //   		hvImgFrame.iStrideWidth[0], hvImgFrame.iStrideWidth[1], hvImgFrame.iStrideWidth[2]);

    // convert to svimage
    sv::SV_IMAGE svImgFrame;
    svImgFrame.m_nWidth = hvImgFrame.iWidth;
    svImgFrame.m_nHeight = hvImgFrame.iHeight;
    svImgFrame.m_nType = HvimgType2SvimgType(hvImgFrame.nImgType);
    svImgFrame.m_pData[0] = (sv::SV_UINT8*)GetHvImageData(&hvImgFrame, 0);
    svImgFrame.m_pData[1] = (sv::SV_UINT8*)GetHvImageData(&hvImgFrame, 1);
    svImgFrame.m_pData[2] = (sv::SV_UINT8*)GetHvImageData(&hvImgFrame, 2);
    svImgFrame.m_rgStrideWidth[0] = hvImgFrame.iStrideWidth[0];
    svImgFrame.m_rgStrideWidth[1] = hvImgFrame.iStrideWidth[1];
    svImgFrame.m_rgStrideWidth[2] = hvImgFrame.iStrideWidth[2];

    if(m_nFrameWidth != svImgFrame.m_nWidth || m_nFrameHeight != svImgFrame.m_nHeight)
    {

        m_nFrameWidth = svImgFrame.m_nWidth;
        m_nFrameHeight = svImgFrame.m_nHeight;



#ifdef TEST_DET_PLATE
        // 绝对坐标转回相对坐标
        m_cTLParam.nRoadX00 = m_cTLParam.nRoadX00 * 100 / m_nFrameWidth;
        m_cTLParam.nRoadY00 = m_cTLParam.nRoadY00 * 100 / m_nFrameHeight;
        m_cTLParam.nRoadX01 = m_cTLParam.nRoadX01 * 100 / m_nFrameWidth;
        m_cTLParam.nRoadY01 = m_cTLParam.nRoadY01 * 100 / m_nFrameHeight;
        m_cTLParam.nRoadX10 = m_cTLParam.nRoadX10 * 100 / m_nFrameWidth;
        m_cTLParam.nRoadY10 = m_cTLParam.nRoadY10 * 100 / m_nFrameHeight;
        m_cTLParam.nRoadX11 = m_cTLParam.nRoadX11 * 100 / m_nFrameWidth;
        m_cTLParam.nRoadY11 = m_cTLParam.nRoadY11 * 100 / m_nFrameHeight;
        RTN_HR_IF_SVFAILED(m_pTLDetCtrl->Init(&m_cTLParam));
#endif
#ifdef TEST_DET_CAR
        // 绝对坐标转回相对坐标
        m_cTLCarDetParam.nRoadX00 = m_cTLCarDetParam.nRoadX00 * 100 / m_nFrameWidth;
        m_cTLCarDetParam.nRoadY00 = m_cTLCarDetParam.nRoadY00 * 100 / m_nFrameHeight;
        m_cTLCarDetParam.nRoadX01 = m_cTLCarDetParam.nRoadX01 * 100 / m_nFrameWidth;
        m_cTLCarDetParam.nRoadY01 = m_cTLCarDetParam.nRoadY01 * 100 / m_nFrameHeight;
        m_cTLCarDetParam.nRoadX10 = m_cTLCarDetParam.nRoadX10 * 100 / m_nFrameWidth;
        m_cTLCarDetParam.nRoadY10 = m_cTLCarDetParam.nRoadY10 * 100 / m_nFrameHeight;
        m_cTLCarDetParam.nRoadX11 = m_cTLCarDetParam.nRoadX11 * 100 / m_nFrameWidth;
        m_cTLCarDetParam.nRoadY11 = m_cTLCarDetParam.nRoadY11 * 100 / m_nFrameHeight;
        RTN_HR_IF_SVFAILED(m_pTLCarDetCtrl->Init(&m_cTLCarDetParam));

        sv::utTrace("TLCtrlReal Road [%d %d] [%d %d] [%d %d] [%d %d]\n", m_cTLCarDetParam.nRoadX00, m_cTLCarDetParam.nRoadY00,
            m_cTLCarDetParam.nRoadX01, m_cTLCarDetParam.nRoadY01,
            m_cTLCarDetParam.nRoadX10, m_cTLCarDetParam.nRoadY10,
            m_cTLCarDetParam.nRoadX11, m_cTLCarDetParam.nRoadY11
            );
#endif
    }

    HRESULT hr = S_OK;
    m_nPlateRectCnt = 0;
    m_nRoiCnt = 0;
    //sv::SV_RESULT svRet = sv::RS_S_OK;
    int nAvgY = -1;

#ifdef TEST_DET_PLATE
    sv::SV_RESULT svRet = m_pTLDetCtrl->Process(
        svImgFrame,
        &nAvgY,
        m_rgPlateRect,
        MAX_PLATE_RECT,
        &m_nPlateRectCnt
        ); 
#endif
                              
#ifdef TEST_DET_CAR
    sv::SV_RESULT svRet = m_pTLCarDetCtrl->Process(
        svImgFrame,
        m_rgRoi,
        MAX_ROI_RECT,
        &m_nRoiCnt
        ); 
#endif

    if (svRet != sv::RS_S_OK)  sv::utTrace("TL Det Err:%08X. imgtype %d\n", svRet, hvImgFrame.nImgType);
    RTN_HR_IF_SVFAILED(svRet);

    // 将数据转为 Sync buf

    int iRetDataSize = 0;
     svTLDetApi::MakeResultBuf(nAvgY, m_rgPlateRect, m_nPlateRectCnt, 
         m_rgRoi, m_nRoiCnt,
         pvSyncData, nMaxSyncDataSize, &iRetDataSize);
    *pnSyncDateSize = iRetDataSize;

#ifdef TEST_DET_PLATE
    sv::utTrace("m_pTLDetCtrl Get Plate:%d. Data:%d\n", m_nPlateRectCnt, iRetDataSize);
#if SV_RUN_PLATFORM == SV_PLATFORM_WIN
    ShowPlate(&svImgFrame, m_rgPlateRect, m_nPlateRectCnt, 1, "DP");
#endif
#endif

#ifdef TEST_DET_CAR
    sv::utTrace("m_pTLDetCtrl Get ROI:%d. Data:%d\n", m_nRoiCnt, iRetDataSize);
#if SV_RUN_PLATFORM == SV_PLATFORM_WIN
    ShowROI(&svImgFrame, m_rgRoi, m_nRoiCnt, 1, "DP");
#endif
#endif

    return hr;
}

