// 该文件编码格式必须是WINDOWS-936格式

#include "ImgGatherer.h"
#include "CamyuLinkOpt.h"
#include "HvSockUtils.h"
#include "hvtarget_ARM.h"

using namespace HiVideo;

CVideoGetter_Camyu::CVideoGetter_Camyu()
{
    m_cPlateLightType = LIGHT_TYPE_COUNT;
    m_fIsLightTypeChanged = false;
    m_bActive = false;
    m_dwFrameCount = 0;
    m_pSignalMatch = NULL;
    m_pCapCamParam = NULL;
    m_pCfgCamParam = NULL;
    m_ppCamTransmit = NULL;
    m_fIsCamConnected = FALSE;
}

CVideoGetter_Camyu::~CVideoGetter_Camyu()
{
    Stop(-1);
}

HRESULT CVideoGetter_Camyu::SetCamCfgParam(LPVOID pCfgCamParam)
{
    if (pCfgCamParam == NULL)
    {
        return E_POINTER;
    }

    m_pCfgCamParam = (CAM_CFG_PARAM *)pCfgCamParam;
    return S_OK;
}

HRESULT CVideoGetter_Camyu::SetCapCamCfgParam(LPVOID pCfgCamParam)
{
    if (pCfgCamParam == NULL)
    {
        return E_POINTER;
    }

    m_pCapCamParam = (CAP_CAM_PARAM *)pCfgCamParam;
    return S_OK;
}

HRESULT CVideoGetter_Camyu::Run(void* pvParam)
{
    DWORD32 dwImgSize = 0;
    DWORD32 dwImgTime = 0;
    DWORD32 dwImgWidth = 0;
    DWORD32 dwImgHeight = 0;

    m_dwLastTime = GetSystemTick();
    HRESULT hrCon = m_cCamyuLink.GetImageStart(m_pCfgCamParam->szIP);

    IMG_FRAME frame;

    int iFps = 0;
    DWORD32 dwFPSTime = GetSystemTick();
    while (!m_fExit)
    {
        m_dwLastTime = GetSystemTick();
        m_bActive = true;

        if (m_dwLastTime - dwFPSTime >= 4000)
        {
            HV_Trace(5, "get image fps = %d\n", iFps / 4);
            dwFPSTime = m_dwLastTime;
            iFps = 0;
        }

        if (m_ppCamTransmit && (*m_ppCamTransmit) && (*m_ppCamTransmit)->IsTransmitting())
        {
            if (hrCon == S_OK)
            {
                SetLan1LedOff();
                m_cCamyuLink.GetImageStop();
                hrCon = E_FAIL;
            }
            m_fIsCamConnected = FALSE;
            HV_Trace(5, "相机数据转发功能已开启，识别已停止！");
            HV_Sleep(5000);
            continue;
        }

        // 创建JPEG引用对象
        IReferenceComponentImage *pRefImage;
        HV_COMPONENT_IMAGE imgFrame;
        HRESULT hr = CreateReferenceComponentImage(
                         &pRefImage,
                         HV_IMAGE_JPEG,
                         1600,
                         1200,
                         m_dwFrameCount++,
                         0,
                         0,
                         "NULL",
                         2
                     );
        if (hr != S_OK)
        {
            m_fIsCamConnected = FALSE;
            HV_Trace(5, "VideoGetter_Camyu: Create image failed!\n");
            SetLan1LedOff();
            m_cCamyuLink.GetImageStop();
            HV_Sleep(1000);
            hrCon = m_cCamyuLink.GetImageStart(m_pCfgCamParam->szIP);
            continue;
        }

        pRefImage->GetImage(&imgFrame);
        dwImgSize = (DWORD32)imgFrame.iWidth;

        if (S_OK != m_cCamyuLink.GetImage((char*)GetHvImageData(&imgFrame, 0), &dwImgSize, &dwImgTime))
        {
            m_fIsCamConnected = FALSE;
            SetLan1LedOff();
            pRefImage->Release();
            pRefImage = NULL;
            m_cCamyuLink.GetImageStop();
            HV_Sleep(4000);
            hrCon = m_cCamyuLink.GetImageStart(m_pCfgCamParam->szIP);
            HV_Trace(5, "camyu get image failed,reconnect...\n");
            continue;
        }

        m_fIsCamConnected = TRUE;
        m_cCamyuLink.GetImageWidth(dwImgWidth);
        m_cCamyuLink.GetImageHeight(dwImgHeight);

        if (dwImgWidth == 0 || dwImgHeight == 0)
        {
            HV_Trace(5, "can not get image size, use default size(1600*1200)\n");
            dwImgWidth = 1600;
            dwImgHeight = 1200;
        }

        if (S_OK == hr)
        {
            ++iFps;
            // JPEG图片高宽特殊意义
            imgFrame.iWidth = dwImgSize;
            imgFrame.iHeight = dwImgWidth | (dwImgHeight << 16);
            pRefImage->SetImageSize(imgFrame);
            pRefImage->SetRefTime(dwImgTime);

            Lan1LedLight();
            //动态设置相机参数
            SetCamParameter();

            // 如果是抓拍图
            if (m_cCamyuLink.GetCurFrameType() == 1 && m_pCapCamParam)
            {
                DWORD32 dwTriggerFlag= m_cCamyuLink.GetCurRoadNum();
                pRefImage->SetCaptureFlag(TRUE);
                //////////////////////////////////////////////////////////////////////////
                // 传递到外总控模块
                SIGNAL_INFO tempSignalInfo;

                tempSignalInfo.dwSignalTime = dwImgTime;
                tempSignalInfo.dwInputTime = GetSystemTick();
                tempSignalInfo.dwValue = 0;
                tempSignalInfo.nType = m_pCapCamParam->rgnSignalType[0];
                tempSignalInfo.dwRoad = 0xFF;
                tempSignalInfo.dwFlag = dwTriggerFlag >> 16;
                tempSignalInfo.iModifyRoad = 0;
                tempSignalInfo.pImage = NULL;
                tempSignalInfo.pImageLast = pRefImage;

                // 将信号加入信号队列
                if (m_pSignalMatch == NULL
                        || S_OK != m_pSignalMatch->AppendSignal(&tempSignalInfo))
                {
                    SAFE_RELEASE(tempSignalInfo.pImageLast);
                }
            }
            else
            {
                // 传递到缓冲队列
                frame.pRefImage = pRefImage;
                frame.iVideoID = 0;
                PutOneFrame(frame);
                pRefImage->Release();
            }
        }
    }

    m_cCamyuLink.GetImageStop();
    m_fExit = TRUE;
    SetLan1LedOff();

    return S_OK;
}

HRESULT CVideoGetter_Camyu::GetCurStatus(char* pszStatus, int nStatusSizes)
{
    bool bActive = m_bActive;
    DWORD32 dwCurTime = GetSystemTick();
    m_bActive = false;
    if (dwCurTime < m_dwLastTime)
    {
        return S_OK;
    }

    if (dwCurTime - m_dwLastTime > 20000 && m_dwLastTime > 0)
    {
        HV_Trace(5, "current time = %d, last time = %d, escape = %d\n", dwCurTime, m_dwLastTime, dwCurTime - m_dwLastTime);
        return bActive ? S_OK : E_FAIL;
    }
    return S_OK;
}

HRESULT CVideoGetter_Camyu::SetLightType(LIGHT_TYPE cLightType, int iCplStatus)
{
    //黄国超修改，只有m_fIsLightTypeChanged为false的情况下才能更新场景变化,2011-09-26
    if (m_cPlateLightType != cLightType && !m_fIsLightTypeChanged)
    {
        m_cPlateLightType = cLightType;
        m_fIsLightTypeChanged = true;
        HV_Trace(5, "场景状态改变:%d\n", m_cPlateLightType);
    }
    return S_OK;
}

bool CVideoGetter_Camyu::SetCamParameter()
{
    HRESULT hr = S_OK;

    if (!m_pCfgCamParam)
    {
        return false;
    }

    if (!m_fIsLightTypeChanged
            || !m_pCfgCamParam->iDynamicCfgEnable)
    {
        return true;
    }

    if (S_OK != m_cCamyuLink.CtrtCamStart(m_pCfgCamParam->szIP))
    {
        return false;
    }

    int nTypeIndex = (int)m_cPlateLightType;
    if (-1 != m_pCfgCamParam->irgExposureTime[nTypeIndex])
    {
        hr = m_cCamyuLink.SendCommand(CY_TIME, m_pCfgCamParam->irgExposureTime[nTypeIndex], 0);
    }
    if (S_OK == hr && -1 != m_pCfgCamParam->irgAGCLimit[nTypeIndex])
    {
        hr = m_cCamyuLink.SendCommand(CY_AGCLIMIT, m_pCfgCamParam->irgAGCLimit[nTypeIndex], 0);
    }
    if (S_OK == hr && -1 != m_pCfgCamParam->irgGain[nTypeIndex])
    {
        hr = m_cCamyuLink.SendCommand(CY_GAIN, m_pCfgCamParam->irgGain[nTypeIndex], 0);
    }
    if (S_OK == hr && -1 != m_pCfgCamParam->iEnableAGC)
    {
        hr = m_cCamyuLink.SendCommand(CY_AGCMODE, m_pCfgCamParam->iEnableAGC, 0);
    }

    m_cCamyuLink.CtrtCamStop();

    if (S_OK == hr)
    {
        HV_Trace(5, "摄像机参数改变:%d\n", nTypeIndex);
        m_fIsLightTypeChanged = false;
    }

    return hr == S_OK ? true : false;
}
