/// @file
/// @brief 
/// @author ganzz
/// @date 1/2/2014 15:05:32
/// 
/// 修改说明:
/// [1/2/2014 15:05:32 ganzz] 最初版本

#pragma once
#include "swbasetype.h"
#include "swimage.h"
#include "trackerdef.h"
#include "svTLDetApi/svTLDetApi.h"

class CTLAppDetCtrl
{
public:
    CTLAppDetCtrl(void);
    ~CTLAppDetCtrl(void);

    HRESULT Init(
        TRACKER_CFG_PARAM* pCfgParam
        );

    HRESULT Uninit();

    HRESULT Process(
        const HV_COMPONENT_IMAGE& hvImgFrame,
        void* pvSyncData,
        int nMaxSyncDataSize,
        int* pnSyncDateSize
        );

private:

    svTLDetApi::TL_DET_PARAM m_cTLParam;
    svTLDetApi::CTLDetCtrl* m_pTLDetCtrl;

    static const int MAX_MOD_DET_INFO = svTLDetApi::MOD_DET_INFO::TYPE_COUNT;
    svTLDetApi::MOD_DET_INFO m_rgModelParam[MAX_MOD_DET_INFO];

    svTLDetApi::TL_CAR_DET_PARAM m_cTLCarDetParam;
    svTLDetApi::CTLCarDetCtrl* m_pTLCarDetCtrl;

    static const int MAX_PLATE_RECT = 32;
    svTLDetApi::SV_PLATERECT m_rgPlateRect[MAX_PLATE_RECT];
    int m_nPlateRectCnt;

    static const int MAX_ROI_RECT = 32;
    svTLDetApi::DET_ROI m_rgRoi[MAX_ROI_RECT];
    int m_nRoiCnt;

    int m_nFrameWidth;
    int m_nFrameHeight;
};

