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
#include "svEPDetApi/svEPDetApi.h"
#include "hvinterface.h"
#include "trackerdef.h"
#include "DspLinkCmd.h"
#include "platedetector.h"

class CEPAppDetCtrl
{
public:
    CEPAppDetCtrl(void);
    ~CEPAppDetCtrl(void);

    HRESULT Init(
        TRACKER_CFG_PARAM* pCfgParam,
        PlateRecogParam* pRecogParam,
        int iFrameWidth, 
        int iFrameHeight
        );

    HRESULT Uninit();

    HRESULT Process(
        const HV_COMPONENT_IMAGE& hvImgFrame,
        void* pvData,
        int& iDataSize
        );
    HRESULT SetLightType(BOOL fIsNight);
private:
    static const int MAX_MOD_DET_INFO = svEPDetApi::MOD_DET_INFO::TYPE_COUNT;
    svEPDetApi::MOD_DET_INFO m_rgModelParam[MAX_MOD_DET_INFO];
    svEPDetApi::EP_DET_PARAM m_cEPParam;
    svEPDetApi::CEPDetCtrl* m_pEPDetCtrl;

    static const int MAX_OBJ_ROI = 512;
    svEPDetApi::DET_ROI m_rgObjROI[MAX_OBJ_ROI];
    int m_nObjROICnt;

    HRESULT SetObjDetCfgParam(
        TRACKER_CFG_PARAM* pCfgParam,
        int iFrameWidth, 
        int iFrameHeight
        );

    // for test
    PlateRecogParam m_cRecogParam;
    //IPlateDetector* m_pPlateDetector;
    //svPlateApi::CPlateDetector* m_pPlateDetector;

    //CHvModelStore m_cModelStore;
   
    BOOL m_fIsNight;
};

