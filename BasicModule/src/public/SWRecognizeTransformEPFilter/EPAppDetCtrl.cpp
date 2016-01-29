#include "EPAppDetCtrl.h"
#include "EPDetModelData.h"
#include "hvutils.h"
#include "hvmodelstore.h"
#include "svEPDetResult.h"

#include "SWFC.h"

//using namespace svPlateApi;
const int MAX_MEM_LIST_CNT = 256;
static CSWMemory* g_rgMemList[MAX_MEM_LIST_CNT] = {0};

void* SV_MEM_ALLOC(
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

void* SV_FAST_MEM_ALLOC(
    int nSize  ///< 申请长度
    )
{
    return malloc(nSize);
}

void SV_MEM_FREE(
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
void SV_FAST_MEM_FREE(
    void* pAddr,                    ///< 要释放的地址
    int nSize                       ///< 内存长度
    ) 
{
    if( pAddr != NULL )
    {
        free(pAddr);
    }
}


#if SV_RUN_PLATFORM == SV_PLATFORM_WIN
BYTE8 g_rgbSyncBuf[1024*6];
int g_nSyncBufLen = 0;

#include "ExLib.h"
void ShowROI(
    const sv::SV_IMAGE* pImg,
    svEPDetApi::DET_ROI* rgROI,
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
        if(rgROI[i].nType == 0 || rgROI[i].nType == 2)
        {
            rgrcInfo2[iInfo2] = rgROI[i];
            iInfo2++;
        }
        else
        {
            rgrcInfo[iInfo1] = rgROI[i];
            iInfo1++;
        }
    }

    ShowRectIntx2Img(pImg, rgrcInfo, NULL, iInfo1, rgrcInfo2, NULL, iInfo2, iMs, szName);
    delete [] rgrcInfo;
    delete [] rgrcInfo2;
}
#endif

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
   //SW_TRACE_DEBUG("<EP>:%s.", szInfo);
#endif

    return 0;
}

static unsigned int svGetSysTimeCallBack()
{
    return CSWDateTime::GetSystemTick();
}

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

sv::SV_RESULT svImgCvt_BT1120_DownSampleTo_YUV422(
    const sv::SV_IMAGE *pSrc,
    sv::SV_IMAGE *pDst, 
    int iDownX, 
    int iDownY
    )
{
    using namespace sv;

    if (pDst == NULL 
        || pSrc == NULL
        || (pSrc->m_nType != 110
        && pSrc->m_nType != 115)
        || pDst->m_nType != SV_IMAGE_YUV422)
    {
        return RS_E_INVALIDARG;
    }

    int iDstWidth = (pSrc->m_nWidth / iDownX);
    int iDstHeight = (pSrc->m_nHeight / iDownY);

    if (pDst->m_nWidth != iDstWidth || pDst->m_nHeight != iDstHeight )
    {
        return RS_E_INVALIDARG;
    }
    float fltMultiDownSampleCbCr = (float)iDownX / 2;

    int iSrcYStride, iSrcCbStride, iSrcCrStride;
    const SV_UINT8* pOrgY = NULL;
    const SV_UINT8* pOrgCb = NULL;
    int nScaleY, nScaleCb, nScaleCr;

    iSrcYStride = pSrc->m_rgStrideWidth[0];
    iSrcCbStride = pSrc->m_rgStrideWidth[1];
    iSrcCrStride = pSrc->m_rgStrideWidth[1];
    pOrgY = pSrc->m_pData[0];
    pOrgCb = pSrc->m_pData[1];

    nScaleY = 2;
    nScaleCb = 2;
    nScaleCr = 2;

    int iYOffSet = iDownX * (nScaleY >> 1);
    int iCbOffSet = (int)(fltMultiDownSampleCbCr * nScaleCb);
    int iCrOffSet = (int)(fltMultiDownSampleCbCr * nScaleCr);

    int iCbOffSetx2 = (int)(iDownX * nScaleCb);
    int iCrOffSetx2 = (int)(iDownX * nScaleCr);

    for (int y = 0; y < iDstHeight; y++)
    {
        SV_UINT8* restrict pDstDataY = (SV_UINT8*)(pDst->m_pData[0] + y * pDst->m_rgStrideWidth[0]);
        SV_UINT8* restrict pDstDataU = (SV_UINT8*)(pDst->m_pData[1] + y * pDst->m_rgStrideWidth[1]);
        SV_UINT8* restrict pDstDataV = (SV_UINT8*)(pDst->m_pData[2] + y * pDst->m_rgStrideWidth[2]);

        const SV_UINT8* restrict pSrcY = (SV_UINT8*)(pOrgY + y * iDownY * iSrcYStride);
        const SV_UINT8* restrict pSrcCb = (SV_UINT8*)(pOrgCb + y * iDownY * iSrcCbStride);
        const SV_UINT8* restrict pSrcCr = pSrcCb+1;

        pSrcCb -= iCbOffSet;
        pSrcCr -= iCrOffSet;

        for (int x = 0; x < iDstWidth; x+=2)
        {
            pSrcCb += iCbOffSet;
            pSrcCr += iCrOffSet;

            int iCb = *pSrcCb;
            int iCr = *pSrcCr;

            int iY = *pSrcY;
            int iY2 = *(pSrcY + iYOffSet);

            pDstDataY[x] = (SV_UINT8)iY;
            pDstDataY[x+1] = (SV_UINT8)iY2;

            pDstDataU[x>>1] = (SV_UINT8)iCb;
            pDstDataV[x>>1] = (SV_UINT8)iCr;

            pSrcY += iYOffSet << 1;
            pSrcCb += iCbOffSetx2 - iCbOffSet;
            pSrcCr += iCrOffSetx2 - iCrOffSet;
        }
    }
    return RS_S_OK;
}

#ifndef RTN_HR_IF_SVFAILED
#define RTN_HR_IF_SVFAILED(fun)\
{ HRESULT hr = Svresult2Hresult(fun); if(FAILED(hr)) return hr;}
#endif

CEPAppDetCtrl::CEPAppDetCtrl(void)
    : m_pEPDetCtrl(NULL)
    , m_fIsNight(FALSE)
{
}


CEPAppDetCtrl::~CEPAppDetCtrl(void)
{
}

HRESULT CEPAppDetCtrl::Init(
    TRACKER_CFG_PARAM* pCfgParam,
    PlateRecogParam* pRecogParam,
    int iFrameWidth, 
    int iFrameHeight
    )
{
    sv::SetCallBack_GetSysTime(svGetSysTimeCallBack);
    sv::utSetTraceCallBack_TXT(svTraceCallback);

    sv::SetCallBack_MemAlloc(SV_MEM_ALLOC);
    sv::SetCallBack_MemFree(SV_MEM_FREE);
    sv::SetCallBack_FastMemAlloc(SV_FAST_MEM_ALLOC);
    sv::SetCallBack_FastMemFree(SV_FAST_MEM_FREE);


    // create ctrl
    m_pEPDetCtrl = svEPDetApi::CreateEPDetCtrl();

    if (m_pEPDetCtrl == NULL)
    {
        return E_OUTOFMEMORY;
    }

    if( S_OK != SetObjDetCfgParam(pCfgParam, iFrameWidth, iFrameHeight) )
    {
    	return E_FAIL;
    }

    // load det model
    for (int i=0; i<MAX_MOD_DET_INFO; ++i)
    {
        RTN_HR_IF_SVFAILED(m_pEPDetCtrl->LoadDetModel(&m_rgModelParam[i]));
    }

    RTN_HR_IF_SVFAILED(m_pEPDetCtrl->Init(&m_cEPParam));
    return S_OK;
}

HRESULT CEPAppDetCtrl::Uninit()
{
    if (NULL != m_pEPDetCtrl)
    {
        m_pEPDetCtrl->ReleaseDetModel();
        svEPDetApi::FreeEPDetCtrl(m_pEPDetCtrl);
        m_pEPDetCtrl = NULL;
    }
    return S_OK;
}

HRESULT CEPAppDetCtrl::Process(
    const HV_COMPONENT_IMAGE& hvImgFrame,
    void* pvData,
    int& iDataSize
    )
{
    if (m_pEPDetCtrl == NULL)   return E_FAIL;

 //   SW_TRACE_DEBUG("EP image type:%d,w:%d,h:%d, str:%d,%d,%d.",
 //   		hvImgFrame.nImgType, hvImgFrame.iWidth, hvImgFrame.iHeight,
 //   		hvImgFrame.iStrideWidth[0], hvImgFrame.iStrideWidth[1], hvImgFrame.iStrideWidth[2]);

    // convert to svimage
    sv::SV_IMAGE svImgFrame;
    svImgFrame.m_nWidth = hvImgFrame.iWidth;
    svImgFrame.m_nHeight = hvImgFrame.iHeight;
    svImgFrame.m_nType = HvimgType2SvimgType(hvImgFrame.nImgType);
    svImgFrame.m_pData[0] = (sv::SV_UINT8*)hvImgFrame.rgImageData[0].addr;
    svImgFrame.m_pData[1] = (sv::SV_UINT8*)hvImgFrame.rgImageData[1].addr;
    svImgFrame.m_pData[2] = (sv::SV_UINT8*)hvImgFrame.rgImageData[2].addr;
    svImgFrame.m_rgStrideWidth[0] = hvImgFrame.iStrideWidth[0];
    svImgFrame.m_rgStrideWidth[1] = hvImgFrame.iStrideWidth[1];
    svImgFrame.m_rgStrideWidth[2] = hvImgFrame.iStrideWidth[2];

    HRESULT hr = S_OK;
    m_nObjROICnt = 0;
    svEPDetApi::CEPDetCtrl::LIGHT_TYPE nLightType = (m_fIsNight) 
        ? svEPDetApi::CEPDetCtrl::LT_NIGHT : svEPDetApi::CEPDetCtrl::LT_DAY;
    // 检测识别

//     sv::CSvImage imgDst;
//     imgDst.m_nType = sv::SV_IMAGE_YUV422;
//     const int IM_WIDTH = 640;
//     const int IM_HEIGHT = 180;
//     imgDst.m_nWidth = IM_WIDTH;
//     imgDst.m_nHeight = IM_HEIGHT;
//     static sv::SV_UINT8 s_buf[IM_WIDTH*IM_HEIGHT];
//     static sv::SV_UINT8 s_buf2[IM_WIDTH*IM_HEIGHT/2];
//     static sv::SV_UINT8 s_buf3[IM_WIDTH*IM_HEIGHT/2];
//     imgDst.m_rgStrideWidth[0] = IM_WIDTH;
//     imgDst.m_rgStrideWidth[1] = IM_WIDTH/2;
//     imgDst.m_rgStrideWidth[2] = IM_WIDTH/2;
//     imgDst.m_pData[0] = s_buf;
//     imgDst.m_pData[1] = s_buf2;//((SV_UINT8*)s_buf) + 640*180;
//     imgDst.m_pData[2] = s_buf3;//imgDst.m_pData[1] + 640*180/2;
//     sv::utTrace("FRAME Static out 5\n");
//     unsigned char* pDstDataY = imgDst.m_pData[0];
//     unsigned char* pDstDataU = imgDst.m_pData[1];
//     unsigned char* pDstDataV = imgDst.m_pData[2];
// //     for(int x = 0; x < imgDst.m_nWidth * imgDst.m_nHeight / 2; ++x)
// //     {
// //         pDstDataY[2*x] = 0;//(SV_UINT8)iY;
// //         pDstDataY[2*x+1] = 0;//(SV_UINT8)iY;
// //     }
// 
//     int nSumY = 0;
//     int nSumU = 0;
//     int nSumV = 0;
//     int nErrCount = 0;
//     int nErrKeep = 0;
//     for(int x = 0; x < imgDst.m_nWidth * imgDst.m_nHeight / 2; ++x)
//     {
// 
//         nSumY += pDstDataY[2*x];
//         nSumY += pDstDataY[2*x+1];
//         nSumU += pDstDataU[x];
//         nSumV += pDstDataV[x];
// 
//         if ((nErrCount < 10) && (pDstDataY[2*x] != 1 || pDstDataY[2*x+1] != 1))
//         {
//             ++nErrCount;
//             sv::utTrace("Err Y: 0ffset %d \n", x); 
//         }
//         if ((nErrCount < 10) && (pDstDataU[x] != 4))
//         {
//             ++nErrCount;
//             sv::utTrace("Err U: 0ffset %d \n", x); 
//         }
//         if ((nErrCount < 10) && (pDstDataV[x] != 8))
//         {
//             ++nErrCount;
//             sv::utTrace("Err V: 0ffset %d \n", x); 
//         }
//     }
//     sv::utTrace("CHK SUM: %d %d %d\n", nSumY, nSumU, nSumV); 
//     sv::utTrace("addr: %X %X %X\n", s_buf, s_buf2, s_buf3); 
// 
//      for(int x = 0; x < imgDst.m_nWidth * imgDst.m_nHeight / 2; ++x)
//      {
// 
//         pDstDataY[2*x] = 1;//(SV_UINT8)iY;
//         pDstDataY[2*x+1] = 1;//(SV_UINT8)iY;
//         pDstDataU[x] = 4;//(SV_UINT8)iY;
//         pDstDataV[x] = 8;//(SV_UINT8)iY;
//     }

    //sv::SV_RESULT svRet = sv::RS_S_OK;
    sv::SV_RESULT svRet = m_pEPDetCtrl->Process(
        svImgFrame,
        nLightType,
        m_rgObjROI,
        MAX_OBJ_ROI,
        &m_nObjROICnt,
        NULL,
        0,
        NULL
        ); 

    if (svRet != sv::RS_S_OK)  sv::utTrace("EP Det Err:%08X. imgtype %d\n", svRet, hvImgFrame.nImgType);
    RTN_HR_IF_SVFAILED(svRet);

    int iRetDataSize = 0;
     svEPDetApi::MakeResultBuf(0,
         m_rgObjROI, m_nObjROICnt, NULL, 0, pvData, iDataSize, &iRetDataSize);
    iDataSize = iRetDataSize;

    sv::utTrace("m_pEPDetCtrl Get ROI:%d. Plate:%d. Data:%d\n", m_nObjROICnt, 0, iDataSize);

    // zhaopy
   // printf("m_pEPDetCtrl Get ROI:%d. Plate:%d. Data:%d\n", m_nObjROICnt, 0, iDataSize);

    // TODO 打印时间和输出数
    // m_rgObjROI              

#if SV_RUN_PLATFORM == SV_PLATFORM_WIN
    ShowROI(&svImgFrame, m_rgObjROI, m_nObjROICnt, 1, "DR");
#endif

    return hr;
}

HRESULT CEPAppDetCtrl::SetLightType(BOOL fIsNight)
{
    m_fIsNight = fIsNight;
    return S_OK;
}

HRESULT CEPAppDetCtrl::SetObjDetCfgParam(
    TRACKER_CFG_PARAM* pCfgParam,
    int iFrameWidth, 
    int iFrameHeight
    )
{
    // set roadinfo
    svEPDetApi::EP_DET_PARAM& cParam = m_cEPParam;

    cParam.iRoadInfoCount = pCfgParam->nRoadLineNumber; //车道线数


    SW_TRACE_DEBUG("<123arm>road begin:%d. count:%d.", pCfgParam->iRoadNumberBegin, cParam.iRoadInfoCount);
    // zhaopy
    if( pCfgParam->iRoadNumberBegin == 0 )
    {
		// 参数转化为百分比
		for (int i=0; i<cParam.iRoadInfoCount; ++i)
		{
			cParam.rgRoadLine[i].ptTop.m_nX = pCfgParam->rgcRoadInfo[i].ptTop.x * 100 / iFrameWidth;
			cParam.rgRoadLine[i].ptTop.m_nY = pCfgParam->rgcRoadInfo[i].ptTop.y * 100 / iFrameHeight;
			cParam.rgRoadLine[i].ptBottom.m_nX = pCfgParam->rgcRoadInfo[i].ptBottom.x * 100 / iFrameWidth;
			cParam.rgRoadLine[i].ptBottom.m_nY = pCfgParam->rgcRoadInfo[i].ptBottom.y * 100 / iFrameHeight;

			printf("Road%d, [%d %d] [%d %d]\n", i, cParam.rgRoadLine[i].ptTop.m_nX, cParam.rgRoadLine[i].ptTop.m_nY,
				cParam.rgRoadLine[i].ptBottom.m_nX, cParam.rgRoadLine[i].ptBottom.m_nY);
		}
    }
    else
    {
    	int index = 0;
		// 参数转化为百分比
		for (int i=cParam.iRoadInfoCount - 1; i >= 0 ; --i, ++index)
		{
			cParam.rgRoadLine[index].ptTop.m_nX = pCfgParam->rgcRoadInfo[i].ptTop.x * 100 / iFrameWidth;
			cParam.rgRoadLine[index].ptTop.m_nY = pCfgParam->rgcRoadInfo[i].ptTop.y * 100 / iFrameHeight;
			cParam.rgRoadLine[index].ptBottom.m_nX = pCfgParam->rgcRoadInfo[i].ptBottom.x * 100 / iFrameWidth;
			cParam.rgRoadLine[index].ptBottom.m_nY = pCfgParam->rgcRoadInfo[i].ptBottom.y * 100 / iFrameHeight;

			printf("exRoad%d, [%d %d] [%d %d]\n", index, cParam.rgRoadLine[index].ptTop.m_nX, cParam.rgRoadLine[index].ptTop.m_nY,
				cParam.rgRoadLine[index].ptBottom.m_nX, cParam.rgRoadLine[index].ptBottom.m_nY);
		}
    }

    // DetArea
    //cParam.rcDetArea = *(sv::SV_RECT*)&(pCfgParam->cVideoDet.rcVideoDetArea);
    //cParam.rcDetArea.m_nTop = pCfgParam->nCaptureTwoPos;
    cParam.nTrackPos = pCfgParam->nCaptureTwoPos;

    cParam.nTh = 4;

    if (iFrameWidth >= 2800)
    {
        cParam.fltXScale = 0.20f;
        cParam.fltYScale = 0.20f;
    }       
    else if (iFrameWidth >= 2440)
    {
        cParam.fltXScale = 0.25f;
        cParam.fltYScale = 0.25f;
    }        
    else if (iFrameWidth >= 1920)
    {
        cParam.fltXScale = 0.33f;
        cParam.fltYScale = 0.33f;
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
        svEPData::g_cEPDetModDaySmall.pbData, 
        svEPData::g_cEPDetModDaySmall.nDataLen,
        svEPDetApi::MOD_DET_INFO::DAY_SMALL_CAR,
        16, 8, 1, 0.9f, 2, 512, 0.6f
        );
    // 白天大车参数
    SV_INIT_MOD_DET_PARAM(  
        m_rgModelParam[1],
        svEPData::g_cEPDetModDayBig.pbData, 
        svEPData::g_cEPDetModDayBig.nDataLen,
        svEPDetApi::MOD_DET_INFO::DAY_BIG_CAR,
        16, 8, 2, 0.7f, 2, 512, 1.1f
        );
    // 晚上小车参数
    SV_INIT_MOD_DET_PARAM(
        m_rgModelParam[2], 
        svEPData::g_cEPDetModNightSmall.pbData, 
        svEPData::g_cEPDetModNightSmall.nDataLen,
        svEPDetApi::MOD_DET_INFO::NIGHT_SMALL_CAR,
        16, 8, 1, 0.75f,  2, 512, 0.6f
        );
    // 晚上大车参数
    SV_INIT_MOD_DET_PARAM(
        m_rgModelParam[3], 
        svEPData::g_cEPDetModNightBig.pbData, 
        svEPData::g_cEPDetModNightBig.nDataLen,
        svEPDetApi::MOD_DET_INFO::NIGHT_BIG_CAR,
        16, 8, 2, 0.7f,   2, 512, 1.1f
        );

    return S_OK;
}

