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

#include "svTgDetApi/svTgDetApi.h"

#include "trackerdef.h"
#include "DspLinkCmd.h"

namespace swTgApp
{
    class CAppDetCtrl
    {
    public:
        CAppDetCtrl(void);
        ~CAppDetCtrl(void);

        HRESULT Init(
            TRACKER_CFG_PARAM* pCfgParam,
            int iFrameWidth, 
            int iFrameHeight
            );

        HRESULT Uninit();

        HRESULT Process(
            const HV_COMPONENT_IMAGE& hvImgFrame,
            void* pvData,
            int nMaxAsycDataSize,
            int* pAsycDataSize
            );
        HRESULT SetLightType(int nEnvLightType, int iEnvStatus);
        HRESULT SetSmallCarRoadRatio(int nSmallCarRoadRatio);
    private:
        static const int MAX_MOD_DET_INFO = svTgIrDetApi::MOD_DET_INFO::TYPE_COUNT;
        svTgIrDetApi::MOD_DET_INFO m_rgModDetInfo[MAX_MOD_DET_INFO];
        svTgIrDetApi::TG_DET_PARAM m_cDetParam;
        svTgIrDetApi::CTgDetCtrl* m_pDetCtrl;

        static const int MAX_OBJ_ROI = 512;
        svTgIrDetApi::DET_ROI m_rgObjROI[MAX_OBJ_ROI];
        int m_nObjROICnt;

        HRESULT SetDetCfgParam(
            TRACKER_CFG_PARAM* pCfgParam,
            int iFrameWidth, 
            int iFrameHeight
            );

        int m_nEnvLightType;
        int m_nSmallCarRoadRatio;
    };

}
