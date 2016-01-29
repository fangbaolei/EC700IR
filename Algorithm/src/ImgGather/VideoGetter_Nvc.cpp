// 该文件编码格式必须是WINDOWS-936格式

#include "ImgGatherer.h"
#include "HvSockUtils.h"
#include "hvtarget_ARM.h"

using namespace HiVideo;

CVideoGetter_Nvc::CVideoGetter_Nvc()
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

CVideoGetter_Nvc::~CVideoGetter_Nvc()
{
    Stop(-1);
}

HRESULT CVideoGetter_Nvc::SetCamCfgParam(LPVOID pCfgCamParam)
{
    if (pCfgCamParam == NULL)
    {
        return E_POINTER;
    }

    m_pCfgCamParam = (CAM_CFG_PARAM *)pCfgCamParam;
    return S_OK;
}

HRESULT CVideoGetter_Nvc::SetCapCamCfgParam(LPVOID pCfgCamParam)
{
    if (pCfgCamParam == NULL)
    {
        return E_POINTER;
    }

    m_pCapCamParam = (CAP_CAM_PARAM *)pCfgCamParam;
    return S_OK;
}

HRESULT CVideoGetter_Nvc::Run(void* pvParam)
{
    DWORD32 dwImgSize = 0;
    DWORD32 dwImgTime = 0;
    DWORD32 dwImgWidth = 0;
    DWORD32 dwImgHeight = 0;

    m_dwLastTime = GetSystemTick();
    HRESULT hrCon = m_cNvcLink.Connect(m_pCfgCamParam->szIP);

    IMG_FRAME frame;

    while (!m_fExit)
    {
        m_dwLastTime = GetSystemTick();
        m_bActive = true;

        if (m_ppCamTransmit && (*m_ppCamTransmit) && (*m_ppCamTransmit)->IsTransmitting())
        {
            if (hrCon == S_OK)
            {
                SetLan1LedOff();
                m_cNvcLink.DisConnect();
                hrCon = E_FAIL;
            }
            m_fIsCamConnected = FALSE;
            HV_Trace(5, "相机数据转发功能已开启，识别已停止！");
            HV_Sleep(5000);
            continue;
        }

        if (hrCon != S_OK)
        {
            m_cNvcLink.DisConnect();
            HV_Sleep(1000);
            m_fIsCamConnected = FALSE;
            hrCon = m_cNvcLink.Connect(m_pCfgCamParam->szIP);
            HV_Trace(5, "Connect camera failed,reconnect...\n");
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
            HV_Trace(5, "VideoGetter_Nvc: Create image failed!\n");
            SetLan1LedOff();
            continue;
        }

        pRefImage->GetImage(&imgFrame);
        dwImgSize = (DWORD32)imgFrame.iWidth;

        if (S_OK != m_cNvcLink.GetJpgImge((char*)GetHvImageData(&imgFrame, 0), &dwImgSize, &dwImgTime))
        {
            SetLan1LedOff();
            pRefImage->Release();
            pRefImage = NULL;
            m_fIsCamConnected = FALSE;
            m_cNvcLink.DisConnect();
            hrCon = m_cNvcLink.Connect(m_pCfgCamParam->szIP);
            HV_Trace(5, "Nvc get image failed,reconnect...\n");
            continue;
        }

        m_fIsCamConnected = TRUE;
        m_cNvcLink.GetImageWidth(dwImgWidth);
        m_cNvcLink.GetImageHeight(dwImgHeight);

        if (dwImgWidth == 0 || dwImgHeight == 0)
        {
            dwImgWidth = 2048;
            dwImgHeight = 1536;
        }

        if (S_OK == hr)
        {
            // JPEG图片高宽特殊意义
            imgFrame.iWidth = dwImgSize;
            imgFrame.iHeight = dwImgWidth | (dwImgHeight << 16);
            pRefImage->SetImageSize(imgFrame);
            pRefImage->SetRefTime(dwImgTime);

            Lan1LedLight();

            //动态设置相机参数
            SetCamParameter();

            // 传递到缓冲队列
            frame.pRefImage = pRefImage;
            frame.iVideoID = 0;
            PutOneFrame(frame);
            pRefImage->Release();
        }
    }

    m_cNvcLink.DisConnect();
    m_fExit = TRUE;
    SetLan1LedOff();

    return S_OK;
}

HRESULT CVideoGetter_Nvc::GetCurStatus(char* pszStatus, int nStatusSizes)
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

HRESULT CVideoGetter_Nvc::SetLightType(LIGHT_TYPE cLightType, int iCplStatus)
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

int CalcMode(char* pszMode)
{
    int iRtn;
    if (strcmp(pszMode, "速度") == 0)
    {
        iRtn = 0;
    }
    else if (strcmp(pszMode, "画质") == 0)
    {
        iRtn = 1;
    }
    else if (strcmp(pszMode, "平衡") == 0)
    {
        iRtn = 2;
    }
    else if (strcmp(pszMode, "快速") == 0)
    {
        iRtn = 3;
    }
    else
    {
        iRtn = 2;
    }
    return iRtn;
}

HRESULT CVideoGetter_Nvc::CamSetMode(WORD16 wValue, WORD16 wTime)       //设置低照度模式
{
    HRESULT hr(S_OK);
    if (wValue == 0)
    {
        m_cNvcLink.WriteRegValue(7, 8);
        m_cNvcLink.WriteRegValue(11, 1);
        m_cNvcLink.WriteRegValue(13, 20);
    }
    else if (wValue == 1)
    {
        m_cNvcLink.WriteRegValue(7, 20);
        m_cNvcLink.WriteRegValue(11, 4);
        m_cNvcLink.WriteRegValue(13, 5);
    }
    else if (wValue == 2)
    {
        m_cNvcLink.WriteRegValue(7, 8);
        m_cNvcLink.WriteRegValue(11, 2);
        m_cNvcLink.WriteRegValue(13, 20);
    }
    else if (wValue == 3)
    {
        m_cNvcLink.WriteRegValue(7, 1);
        m_cNvcLink.WriteRegValue(11, 1);
    }
    else
    {
        hr = E_FAIL;
    }
    if (hr != E_FAIL)
    {
        m_cNvcLink.WriteRegValue(0x04, wTime);
    }
    return hr;
}

HRESULT CVideoGetter_Nvc::CamSetLight(WORD16 wValue)
{
    return m_cNvcLink.WriteRegValue(0x02, wValue);
}

bool CVideoGetter_Nvc::SetCamParameter()
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

    int nTypeIndex = (int)m_cPlateLightType;

    hr = CamSetLight(m_pCfgCamParam->irgBrightness[nTypeIndex] + 100);

    if (S_OK == hr)
    {
        hr = CamSetMode(CalcMode(m_pCfgCamParam->rgszMode[nTypeIndex]), m_pCfgCamParam->irgExposureTime[nTypeIndex]);
    }

    if (S_OK == hr)
    {
        HV_Trace(5, "摄像机参数改变:%d\n", nTypeIndex);
        m_fIsLightTypeChanged = false;
    }

    return hr == S_OK ? true : false;
}
