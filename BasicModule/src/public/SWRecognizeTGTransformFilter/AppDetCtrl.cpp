#include "AppDetCtrl.h"
#include "DetModelData.h"
#include "hvutils.h"
#include "AsyncDetResult.h"
#include "SWLog.h"

#if SV_RUN_PLATFORM == SV_PLATFORM_LINUX
#include "SWFC.h"
#endif

#if SV_RUN_PLATFORM == SV_PLATFORM_WIN
#include "ExLib.h"
void ShowROI(
    const sv::SV_IMAGE* pImg,
    svTgVvdDetApi::DET_ROI* rgROI,
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

    //ShowRectIntx2Img(pImg, rgrcInfo, NULL, iInfo1, rgrcInfo2, NULL, iInfo2, iMs, szName);
    delete [] rgrcInfo;
    delete [] rgrcInfo2;
}
#endif

static char g_szBufLog[1024];

namespace swTgVvdApp
{

#if SV_RUN_PLATFORM == SV_PLATFORM_LINUX

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

    static unsigned int svGetSysTimeCallBack()
    {
        return CSWDateTime::GetSystemTick();
    }

#endif

    static int svTraceCallback(
        const char* szInfo,     ///< 字符串
        int nLen                ///< 字符串长度+1
        )
    {
#if RUN_PLATFORM == PLATFORM_WINDOWS
        printf(szInfo);

#if 0  // 输出到文件
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
        //SW_TRACE_DSP("ARM:%s", szInfo);
        if (strlen(g_szBufLog) + strlen(szInfo) < 1024*1)
        {
        	strcat(g_szBufLog, szInfo);
        }
#endif

        return 0;
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
        else if (nHvType == HV_IMAGE_GRAY)
        {
            svType = sv::SV_IMAGE_GRAY;
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

    CAppDetCtrl::CAppDetCtrl(void)
        : m_pDetCtrl(NULL)
        , m_nEnvLightType(0)
    {
        svTgVvdDetApi::MOD_DET_INFO* pModel = m_rgModDetInfo;

        // 白天小车
        pModel->nDetType = svTgVvdDetApi::MOD_DET_INFO::DAY_SMALL_CAR;
        pModel->pbData = swTgVvdApp::g_cTgDetMode_DaySmall.pData;
        pModel->nDataLen = swTgVvdApp::g_cTgDetMode_DaySmall.nDataLen;
        pModel->fltRoadRatio = 0.6f;
        pModel->nStepDivX = 8;
        pModel->nStepDivY = 6;
        pModel->nScaleNum = 3;
        pModel->nMergeNum = 2;
        pModel->nMaxROI = 500;
        ++pModel;

        // 白天大车
        pModel->nDetType = svTgVvdDetApi::MOD_DET_INFO::DAY_LARGE_CAR;
        pModel->pbData = swTgVvdApp::g_cTgDetMode_DayLarge.pData;
        pModel->nDataLen = swTgVvdApp::g_cTgDetMode_DayLarge.nDataLen;
        pModel->fltRoadRatio = 0.9f;
        pModel->nStepDivX = 8;
        pModel->nStepDivY = 6;
        pModel->nScaleNum = 3;
        pModel->nMergeNum = 2;
        pModel->nMaxROI = 500;
        ++pModel;

        // 傍晚小车
        pModel->nDetType = svTgVvdDetApi::MOD_DET_INFO::DUSK_SMALL_CAR;
        pModel->pbData = swTgVvdApp::g_cTgDetMode_DuskSmall.pData;         //傍晚使用白天模型
        pModel->nDataLen = swTgVvdApp::g_cTgDetMode_DuskSmall.nDataLen;
        pModel->fltRoadRatio = 0.6f;
        pModel->nStepDivX = 8;
        pModel->nStepDivY = 6;
        pModel->nScaleNum = 3;
        pModel->nMergeNum = 2;
        pModel->nMaxROI = 500;
        ++pModel;

        // 傍晚小车增强模型，模型参数需要和傍晚小车一致
        //*pModel = *(pModel - 1);
        //pModel->nDetType = svTgVvdDetApi::MOD_DET_INFO::DUSK_SMALL_CAR_EX;
        //pModel->pbData = swTgVvdApp::g_cTgDetMode_DuskSmallEx.pData;
        //pModel->nDataLen = swTgVvdApp::g_cTgDetMode_DuskSmallEx.nDataLen;
        //++pModel;

        // 傍晚大车
        pModel->nDetType = svTgVvdDetApi::MOD_DET_INFO::DUSK_LARGE_CAR;
        pModel->pbData = swTgVvdApp::g_cTgDetMode_DayLarge.pData;         //傍晚使用白天模型
        pModel->nDataLen = swTgVvdApp::g_cTgDetMode_DayLarge.nDataLen;
        pModel->fltRoadRatio = 0.9f;
        pModel->nStepDivX = 8;
        pModel->nStepDivY = 6;
        pModel->nScaleNum = 3;
        pModel->nMergeNum = 2;
        pModel->nMaxROI = 500;
        ++pModel;

        // 傍晚大车增强模型，模型参数需要和傍晚小车一致
        *pModel = *(pModel - 1);
        pModel->nDetType = svTgVvdDetApi::MOD_DET_INFO::DUSK_LARGE_CAR_EX;
        pModel->pbData = swTgVvdApp::g_cTgDetMode_DuskLargeEx.pData;
        pModel->nDataLen = swTgVvdApp::g_cTgDetMode_DuskLargeEx.nDataLen;
        ++pModel;

        // 晚上小车
        pModel->nDetType = svTgVvdDetApi::MOD_DET_INFO::NIGHT_SMALL_CAR;
        pModel->pbData = swTgVvdApp::g_cTgDetMode_NightSmall.pData;
        pModel->nDataLen = swTgVvdApp::g_cTgDetMode_NightSmall.nDataLen;
        pModel->fltRoadRatio = 0.6f;
        pModel->nStepDivX = 8;
        pModel->nStepDivY = 8;
        pModel->nScaleNum = 3;
        pModel->nMergeNum = 2;
        pModel->nMaxROI = 500;
        ++pModel;

        // 晚上大车
        pModel->nDetType = svTgVvdDetApi::MOD_DET_INFO::NIGHT_LARGE_CAR;
        pModel->pbData = swTgVvdApp::g_cTgDetMode_NightLarge.pData;
        pModel->nDataLen = swTgVvdApp::g_cTgDetMode_NightLarge.nDataLen;
        pModel->fltRoadRatio = 0.9f;
        pModel->nStepDivX = 8;
        pModel->nStepDivY = 8;
        pModel->nScaleNum = 3;
        pModel->nMergeNum = 2;
        pModel->nMaxROI = 500;
        ++pModel;
    }


    CAppDetCtrl::~CAppDetCtrl(void)
    {
        if (m_pDetCtrl != NULL)
        {
            svTgVvdDetApi::FreeTgDetCtrl(m_pDetCtrl);
            m_pDetCtrl = NULL;
        }
    }

    HRESULT CAppDetCtrl::Init(
        TRACKER_CFG_PARAM* pCfgParam,
        int iFrameWidth, 
        int iFrameHeight
        )
    {
#if SV_RUN_PLATFORM != SV_PLATFORM_WIN     // Win平台由AppTrackCtrl控制
        sv::utSetTraceCallBack_TXT(svTraceCallback);
#endif

#if SV_RUN_PLATFORM == SV_PLATFORM_LINUX
        sv::SetCallBack_GetSysTime(svGetSysTimeCallBack);
        sv::SetCallBack_MemAlloc(SV_MEM_ALLOC);
        sv::SetCallBack_MemFree(SV_MEM_FREE);
        sv::SetCallBack_FastMemAlloc(SV_FAST_MEM_ALLOC);
        sv::SetCallBack_FastMemFree(SV_FAST_MEM_FREE);
#endif

        m_nSmallCarRoadRatio = 0;

        // create ctrl
        if (m_pDetCtrl == NULL)
        {
            m_pDetCtrl = svTgVvdDetApi::CreateTgDetCtrl();
        }

        if (m_pDetCtrl == NULL)
        {
            return E_OUTOFMEMORY;
        }

        if( S_OK != SetDetCfgParam(pCfgParam, iFrameWidth, iFrameHeight) )
        {
            return E_FAIL;
        }

        // load det model
        for (int i=0; i<MAX_MOD_DET_INFO; ++i)
        {
            RTN_HR_IF_SVFAILED(m_pDetCtrl->LoadDetModel(&m_rgModDetInfo[i]));
        }
#ifndef BIKE_DET_MODE
        m_cDetParam.nDetTopLine = 20;
        m_cDetParam.nDetBottomLine = 80;
#else
        m_cDetParam.nDetBottomLine = 99;
        m_cDetParam.nMergeOverlapRatio = 65;
#endif
        RTN_HR_IF_SVFAILED(m_pDetCtrl->Init(&m_cDetParam));
        return S_OK;
    }

    HRESULT CAppDetCtrl::Uninit()
    {
        if (NULL != m_pDetCtrl)
        {
            m_pDetCtrl->ReleaseDetModel();
            svTgVvdDetApi::FreeTgDetCtrl(m_pDetCtrl);
            m_pDetCtrl = NULL;
        }
        return S_OK;
    }
#define NEW_DET_FLAG
    HRESULT CAppDetCtrl::Process(
        const HV_COMPONENT_IMAGE& hvImgFrame,
        void* pvData,
        int nMaxAsycDataSize,
        int* pAsycDataSize
        )
    {
        if (m_pDetCtrl == NULL)   return E_FAIL;

        //   SW_TRACE_DEBUG("EP image type:%d,w:%d,h:%d, str:%d,%d,%d.",
        //   		hvImgFrame.nImgType, hvImgFrame.iWidth, hvImgFrame.iHeight,
        //   		hvImgFrame.iStrideWidth[0], hvImgFrame.iStrideWidth[1], hvImgFrame.iStrideWidth[2]);

        // convert to svimage
        sv::SV_IMAGE svImgFrame;
#ifdef NEW_DET_FLAG
        svImgFrame.m_nWidth = hvImgFrame.iWidth;
        svImgFrame.m_nHeight = hvImgFrame.iHeight;
        svImgFrame.m_nType = HvimgType2SvimgType(hvImgFrame.nImgType);

        svImgFrame.m_pData[0] = (sv::SV_UINT8*)GetHvImageData(&hvImgFrame, 0);
		svImgFrame.m_pData[1] = (sv::SV_UINT8*)GetHvImageData(&hvImgFrame, 1);
		svImgFrame.m_pData[2] = (sv::SV_UINT8*)GetHvImageData(&hvImgFrame, 2);
		svImgFrame.m_rgStrideWidth[0] = hvImgFrame.iStrideWidth[0] / 2;
		svImgFrame.m_rgStrideWidth[1] = hvImgFrame.iStrideWidth[1] / 2;
		svImgFrame.m_rgStrideWidth[2] = hvImgFrame.iStrideWidth[2] / 2;
		svImgFrame.m_nHeight *= 2;
#else
        svImgFrame.m_nWidth = hvImgFrame.iWidth;
		svImgFrame.m_nHeight = hvImgFrame.iHeight;
		svImgFrame.m_nType = HvimgType2SvimgType(hvImgFrame.nImgType);
		svImgFrame.m_pData[0] = (sv::SV_UINT8*)GetHvImageData(&hvImgFrame, 0);
		svImgFrame.m_pData[1] = (sv::SV_UINT8*)GetHvImageData(&hvImgFrame, 1);
		svImgFrame.m_pData[2] = (sv::SV_UINT8*)GetHvImageData(&hvImgFrame, 2);
		svImgFrame.m_rgStrideWidth[0] = hvImgFrame.iStrideWidth[0];
		svImgFrame.m_rgStrideWidth[1] = hvImgFrame.iStrideWidth[1];
		svImgFrame.m_rgStrideWidth[2] = hvImgFrame.iStrideWidth[2];
#endif

        HRESULT hr = S_OK;
        m_nObjROICnt = 0;
		svTgVvdDetApi::CTgDetCtrl::LIGHT_TYPE nLightType = svTgVvdDetApi::CTgDetCtrl::LT_DAY;
		switch (m_nEnvLightType)
		{
		case svTgVvdDetApi::CTgDetCtrl::LT_DAY:
			nLightType = svTgVvdDetApi::CTgDetCtrl::LT_DAY;
			break;
		case svTgVvdDetApi::CTgDetCtrl::LT_DUSK:
			nLightType = svTgVvdDetApi::CTgDetCtrl::LT_DUSK;
			break;
		case svTgVvdDetApi::CTgDetCtrl::LT_NIGHT:
			nLightType = svTgVvdDetApi::CTgDetCtrl::LT_NIGHT;
			break;
		default:
			break;
		}

		g_szBufLog[0] = 0;
        // 检测识别
        sv::SV_RESULT svRet = m_pDetCtrl->Process(
            svImgFrame,
            nLightType,
            m_rgObjROI,
            MAX_OBJ_ROI,
            &m_nObjROICnt
            ); 
#ifdef NEW_DET_FLAG
        for (int i=0; i<m_nObjROICnt; ++i)
		{
			m_rgObjROI[i].m_nTop /= 2;
			m_rgObjROI[i].m_nBottom /= 2;
		}
#endif
        if (svRet != sv::RS_S_OK)  sv::utTrace("Tg Det Err:%08X. imgtype %d\n", svRet, hvImgFrame.nImgType);
        RTN_HR_IF_SVFAILED( svRet );

        swTgApp::MakeResultBuf(0,
            (swTgApp::DET_ROI*)m_rgObjROI, m_nObjROICnt, pvData, nMaxAsycDataSize, pAsycDataSize);

        sv::utTrace("m_pDetCtrl Get ROI:%d. Data:%d\n", m_nObjROICnt, *pAsycDataSize);

#if SV_RUN_PLATFORM == SV_PLATFORM_LINUX

        SW_TRACE_DSP(g_szBufLog);

#endif

        // zhaopy
        // printf("m_pDetCtrl Get ROI:%d. Plate:%d. Data:%d\n", m_nObjROICnt, 0, iDataSize);
        // TODO 打印时间和输出数
        // m_rgObjROI

//#if SV_RUN_PLATFORM == SV_PLATFORM_WIN
//        ShowROI(&svImgFrame, m_rgObjROI, m_nObjROICnt, 1, "DR");
//#endif
//
        return hr;
    }

    HRESULT CAppDetCtrl::SetLightType(int nEnvLightType, int iEnvStatus)
    {
        m_nEnvLightType = nEnvLightType;
        if (iEnvStatus == 2
			&& (m_nEnvLightType == svTgVvdDetApi::CTgDetCtrl::LT_DAY
			|| m_nEnvLightType == svTgVvdDetApi::CTgDetCtrl::LT_DUSK))
		{
        	if (m_pDetCtrl)
        	{
        		m_pDetCtrl->ForceDuskEnvLightType();
        	}
		}
        return S_OK;
    }

    HRESULT CAppDetCtrl::SetSmallCarRoadRatio(int nSmallCarRoadRatio)
    {
        m_nSmallCarRoadRatio = nSmallCarRoadRatio;
        return S_OK;
    }

    HRESULT CAppDetCtrl::SetDetCfgParam(
        TRACKER_CFG_PARAM* pCfgParam,
        int iFrameWidth, 
        int iFrameHeight
        )
    {
        // set roadinfo
        svTgVvdDetApi::TG_DET_PARAM& cParam = m_cDetParam;

        cParam.nRoadLineCount = pCfgParam->nRoadLineNumber; //车道线数

        for (int i=0; i<cParam.nRoadLineCount; ++i)
        {
#if SV_RUN_PLATFORM == SV_PLATFORM_WIN  
            cParam.rgRoadLine[i].ptTop.m_nX = pCfgParam->rgcRoadInfo[i].ptTop.x ; 
            cParam.rgRoadLine[i].ptTop.m_nY = pCfgParam->rgcRoadInfo[i].ptTop.y ; 
            cParam.rgRoadLine[i].ptBottom.m_nX = pCfgParam->rgcRoadInfo[i].ptBottom.x; 
            cParam.rgRoadLine[i].ptBottom.m_nY = pCfgParam->rgcRoadInfo[i].ptBottom.y;  
#else
            // 设备上的坐标还是帧图的实坐标！还要除以2
            cParam.rgRoadLine[i].ptTop.m_nX = pCfgParam->rgcRoadInfo[i].ptTop.x * 100 / iFrameWidth; 
            cParam.rgRoadLine[i].ptTop.m_nY = pCfgParam->rgcRoadInfo[i].ptTop.y * 100 / iFrameHeight / 2; 
            cParam.rgRoadLine[i].ptBottom.m_nX = pCfgParam->rgcRoadInfo[i].ptBottom.x * 100 / iFrameWidth; 
            cParam.rgRoadLine[i].ptBottom.m_nY = pCfgParam->rgcRoadInfo[i].ptBottom.y * 100 / iFrameHeight / 2; 
#endif
            sv::utTrace("Road%d, [%d %d] [%d %d]\n", i, cParam.rgRoadLine[i].ptTop.m_nX, cParam.rgRoadLine[i].ptTop.m_nY,
                cParam.rgRoadLine[i].ptBottom.m_nX, cParam.rgRoadLine[i].ptBottom.m_nY);
        }
        //     cParam.rgRoadLine[0].ptTop.m_nX = 44; 
        //     cParam.rgRoadLine[0].ptTop.m_nY = 0;
        //     cParam.rgRoadLine[0].ptBottom.m_nX = 5;////pCfgParam->rgcRoadInfo[i].ptBottom.x * 100 / iFrameWidth; 
        //     cParam.rgRoadLine[0].ptBottom.m_nY = 100;//pCfgParam->rgcRoadInfo[i].ptBottom.y * 100 / iFrameHeight; 
        //     cParam.rgRoadLine[1].ptTop.m_nX = 39;//pCfgParam->rgcRoadInfo[i].ptTop.x * 100 / iFrameWidth; 
        //     cParam.rgRoadLine[1].ptTop.m_nY = 0;//pCfgParam->rgcRoadInfo[i].ptTop.y * 100 / iFrameHeight; 
        //     cParam.rgRoadLine[1].ptBottom.m_nX = 37;//pCfgParam->rgcRoadInfo[i].ptBottom.x * 100 / iFrameWidth; 
        //     cParam.rgRoadLine[1].ptBottom.m_nY = 100;//pCfgParam->rgcRoadInfo[i].ptBottom.y * 100 / iFrameHeight; 
        //     cParam.rgRoadLine[2].ptTop.m_nX = 39;//pCfgParam->rgcRoadInfo[i].ptTop.x * 100 / iFrameWidth; 
        //     cParam.rgRoadLine[2].ptTop.m_nY = 0;//pCfgParam->rgcRoadInfo[i].ptTop.y * 100 / iFrameHeight; 
        //     cParam.rgRoadLine[2].ptBottom.m_nX = 65;//pCfgParam->rgcRoadInfo[i].ptBottom.x * 100 / iFrameWidth; 
        //     cParam.rgRoadLine[2].ptBottom.m_nY = 100;//pCfgParam->rgcRoadInfo[i].ptBottom.y * 100 / iFrameHeight; 
        //     cParam.rgRoadLine[3].ptTop.m_nX = 38;//pCfgParam->rgcRoadInfo[i].ptTop.x * 100 / iFrameWidth; 
        //     cParam.rgRoadLine[3].ptTop.m_nY = 0;//pCfgParam->rgcRoadInfo[i].ptTop.y * 100 / iFrameHeight; 
        //     cParam.rgRoadLine[3].ptBottom.m_nX = 95;//pCfgParam->rgcRoadInfo[i].ptBottom.x * 100 / iFrameWidth; 
        //     cParam.rgRoadLine[3].ptBottom.m_nY = 100;//pCfgParam->rgcRoadInfo[i].ptBottom.y * 100 / iFrameHeight; 


        if (iFrameWidth >= 3300)
        {
            cParam.fltXScale = 0.2f;
            cParam.fltYScale = 0.2f;
        }
        else if (iFrameWidth >= 1920)
        {
            cParam.fltXScale = 0.25f;
            cParam.fltYScale = 0.5f;
        }

        //cParam.nDetTopLine = 20;

        return S_OK;
    }

}

