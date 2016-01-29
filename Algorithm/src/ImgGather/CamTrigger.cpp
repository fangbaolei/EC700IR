#include "ImgGatherer.h"
#include "platerecogparam.h"

using namespace HiVideo;

CCamTrigger::CCamTrigger()
        : m_iCurRoadNum(0)
        , m_nCurLightType(DAY)
        , m_pCfgCamParam(NULL)
{
    CreateSemaphore(&m_hSemNeedTrigger, 0, 10);
    m_dwLastThreadIsOkTime = GetSystemTick();
}

CCamTrigger::~CCamTrigger(void)
{
    Stop(-1);
    DestroySemaphore(&m_hSemNeedTrigger);
}

HRESULT CCamTrigger::ProcessOnTrigger()
{
    while (!m_fExit)
    {
        m_dwLastThreadIsOkTime = GetSystemTick();
        if (!m_pCfgCamParam->iDynamicTriggerEnable)
        {
            HV_Sleep(4000);
            continue;
        }

        int iCurExposureTime = -1;
        int iCurGain = -1;

        if (SemPend(&m_hSemNeedTrigger, 1000) == 0)
        {
            iCurExposureTime = m_pCfgCamParam->irgExposureTime[m_nCurLightType];
            iCurGain = m_pCfgCamParam->irgGain[m_nCurLightType];

            // 触发抓拍相机
            HRESULT hr = m_cCamyuLink.CtrtCamStart(m_pCfgCamParam->szIP);
            if (hr != S_OK)
            {
                HV_Trace(3, "CamTrigger: CtrtCamStart Error..\n");
                continue;
            }

            hr = m_cCamyuLink.SendCommand(CY_SOFT_TRIGGER, 1, m_iCurRoadNum);
            if (hr != S_OK)
            {
                HV_Trace(3, "CamTrigger: SendCommand Error..\n");
                m_cCamyuLink.CtrtCamStop();
                continue;
            }

            m_cCamyuLink.CtrtCamStop();
        }
    }
    m_fExit = TRUE;

    return S_OK;
}

HRESULT CCamTrigger::Run(void* pvParam)
{
    HV_Sleep(1000);
    // 有车时触发摄像机
    return ProcessOnTrigger();
}

HRESULT CCamTrigger::Trigger(LIGHT_TYPE nLightType, int iRoadNum)
{
    if (!m_pCfgCamParam->iDynamicTriggerEnable) return S_FALSE;

    m_nCurLightType = nLightType;
    m_iCurRoadNum = iRoadNum;
    SemPost(&m_hSemNeedTrigger, 0);
    return S_OK;
}

HRESULT CCamTrigger::HardTrigger(int iRoadNum)
{
    m_iCurRoadNum = iRoadNum;
    SemPost(&m_hSemNeedTrigger, 0);
    return S_OK;
}

bool CCamTrigger::ThreadIsOk()
{
    bool fRtn = false;
    if (GetSystemTick() - m_dwLastThreadIsOkTime < 100000)
    {
        fRtn = true;
    }
    return fRtn;
}

HRESULT CCamTrigger::SetCamTriggerParam(CAM_CFG_PARAM *pCfgCamParam)
{
    if (pCfgCamParam == NULL)
    {
        return E_FAIL;
    }

    m_pCfgCamParam = pCfgCamParam;

    if (GetCurrentMode() == PRM_HVC)
    {
        if (strcmp(m_pCfgCamParam->szIP, "0.0.0.0") == 0)
        {
            m_pCfgCamParam->iDynamicTriggerEnable = false;
        }
        else
        {
            m_pCfgCamParam->iDynamicTriggerEnable = true;
        }
    }
    return S_OK;
}
