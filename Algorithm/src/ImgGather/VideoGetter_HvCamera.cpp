// 该文件编码格式必须是WINDOWS-936格式

#include "ImgGatherer.h"
#include "HvSockUtils.h"
#include "hvtarget_ARM.h"
#include "HvCameraLinkOpt.h"
#include "HvPciLinkApi.h"

using namespace HiVideo;

CVideoGetter_HvCamera::CVideoGetter_HvCamera()
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
    m_fCaptureMode = false;
}

CVideoGetter_HvCamera::~CVideoGetter_HvCamera()
{
    Stop(-1);
}

HRESULT CVideoGetter_HvCamera::SetCamCfgParam(LPVOID pCfgCamParam)
{
    if (pCfgCamParam == NULL)
    {
        return E_POINTER;
    }

    m_pCfgCamParam = (CAM_CFG_PARAM *)pCfgCamParam;
    return S_OK;
}

HRESULT CVideoGetter_HvCamera::SetCapCamCfgParam(LPVOID pCfgCamParam)
{
    if (pCfgCamParam == NULL)
    {
        return E_POINTER;
    }

    m_pCapCamParam = (CAP_CAM_PARAM *)pCfgCamParam;
    return S_OK;
}

HRESULT  CVideoGetter_HvCamera::SetCaptureMode(LPVOID pCfgCamParam)
{
    if (pCfgCamParam == NULL)
    {
        return E_POINTER;
    }

    m_pCfgCamParam = (CAM_CFG_PARAM *)pCfgCamParam;
    m_fCaptureMode = true;
    return S_OK;
}

HRESULT CVideoGetter_HvCamera::RunCommMode()
{
    DWORD32 dwImgSize = 0;
    DWORD32 dwImgTime = 0;
    DWORD32 dwImgWidth = 0;
    DWORD32 dwImgHeight = 0;
    DWORD32 dwImgType = 0;

    m_dwLastTime = GetSystemTick();
    HRESULT hrCon = m_cHvCameraLink.GetImageStart(m_pCfgCamParam->szIP);

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
                m_cHvCameraLink.GetImageStop();
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
            HV_Trace(5, "VideoGetter_HVCamera: Create image failed!\n");
            SetLan1LedOff();
            m_cHvCameraLink.GetImageStop();
            HV_Sleep(1000);
            hrCon = m_cHvCameraLink.GetImageStart(m_pCfgCamParam->szIP);
            continue;
        }

        pRefImage->GetImage(&imgFrame);
        dwImgSize = (DWORD32)imgFrame.iWidth;

        //HV_Trace(5,"GetOneFrame\n");
        if (S_OK != m_cHvCameraLink.GetImage((char*)GetHvImageData(&imgFrame, 0),
                                             &dwImgSize, &dwImgTime, &dwImgType))
        {
            HV_Trace(5,"GetOneFrame Faile\n");
            m_fIsCamConnected = FALSE;
            SetLan1LedOff();
            pRefImage->Release();
            pRefImage = NULL;
            m_cHvCameraLink.GetImageStop();
            HV_Sleep(4000);
            hrCon = m_cHvCameraLink.GetImageStart(m_pCfgCamParam->szIP);
            //重连后需要再次调整参数，避免由于外部设置造成相机不能动态调节参数
            m_fIsLightTypeChanged = true;
            HV_Trace(5, "hvcamera get image failed,reconnect...\n");
            continue;
        }

        m_fIsCamConnected = TRUE;
        m_cHvCameraLink.GetImageWidth(dwImgWidth);
        m_cHvCameraLink.GetImageHeight(dwImgHeight);

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
            if (dwImgType == CAMERA_IMAGE_JPEG_CAPTURE && m_pCapCamParam)
            {
                pRefImage->SetCaptureFlag(TRUE);
                //////////////////////////////////////////////////////////////////////////
                // 传递到外总控模块
                SIGNAL_INFO tempSignalInfo;

                tempSignalInfo.dwSignalTime = dwImgTime;
                tempSignalInfo.dwInputTime = GetSystemTick();
                tempSignalInfo.dwValue = 0;
                tempSignalInfo.nType = m_pCapCamParam->rgnSignalType[0];
                tempSignalInfo.dwRoad = 0; //必须初始化否则为随机值
                tempSignalInfo.dwFlag = 0;
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
            else if(dwImgType == CAMERA_IMAGE_JPEG)
            {
                // 传递到缓冲队列
                frame.pRefImage = pRefImage;
                frame.iVideoID = 0;
                PutOneFrame(frame);
                pRefImage->Release();
            }
            else
            {
                HV_Trace(5,"ImgType:%d\n",dwImgType);
                pRefImage->Release();
            }
        }
    }

    m_cHvCameraLink.GetImageStop();
    m_fExit = TRUE;
    HV_Trace(5,"HvCamera Exit ...\n");
    SetLan1LedOff();

    return S_OK;
}

HRESULT CVideoGetter_HvCamera::RunCaptureMode()
{
    DWORD32 dwImgSize = 0;
    DWORD32 dwImgTime = 0;
    DWORD32 dwImgWidth = 0;
    DWORD32 dwImgHeight = 0;
    DWORD32 dwImgType = 0;
    PANORAMIC_CAPTURER_FRAME_INFO cFrameInfo;
    const int IMAGE_FRAME_LEN = 1024 * 1024;
    char* pbFrameBuf = new char[IMAGE_FRAME_LEN];
    int iFrameInfoLen = sizeof(PANORAMIC_CAPTURER_FRAME_INFO);
    if(pbFrameBuf == NULL)
    {
        HV_Trace(5, "<PanormicCapture> Image Buffer Alloc Failed!\n");
        return E_FAIL;
    }

    m_dwLastTime = GetSystemTick();
    HRESULT hrCon = m_cHvCameraLink.GetImageStart(m_pCfgCamParam->szIP);
    m_cHvCameraLink.SendImageCmd("PanoramicMode");

    IMG_FRAME frame;

    while (!m_fExit)
    {
        m_dwLastTime = GetSystemTick();
        m_bActive = true;

        if (m_ppCamTransmit && (*m_ppCamTransmit) && (*m_ppCamTransmit)->IsTransmitting())
        {
            if (hrCon == S_OK)
            {
                m_cHvCameraLink.GetImageStop();
                hrCon = E_FAIL;
            }
            m_fIsCamConnected = FALSE;
            HV_Sleep(5000);
            continue;
        }

        dwImgSize = IMAGE_FRAME_LEN - iFrameInfoLen;
        if (S_OK != m_cHvCameraLink.GetImage(pbFrameBuf + iFrameInfoLen,
                                             &dwImgSize, &dwImgTime, &dwImgType))
        {
            m_fIsCamConnected = FALSE;
            m_cHvCameraLink.GetImageStop();
            HV_Sleep(4000);
            hrCon = m_cHvCameraLink.GetImageStart(m_pCfgCamParam->szIP);
            m_cHvCameraLink.SendImageCmd("PanoramicMode");
            continue;
        }

        m_fIsCamConnected = TRUE;
        m_cHvCameraLink.GetImageWidth(dwImgWidth);
        m_cHvCameraLink.GetImageHeight(dwImgHeight);

        if (dwImgWidth == 0 || dwImgHeight == 0)
        {
            dwImgWidth = 1600;
            dwImgHeight = 1200;
        }

        // 如果是抓拍图
        if (dwImgType == CAMERA_IMAGE_JPEG_CAPTURE)
        {
            cFrameInfo.dwLen = dwImgSize;
            cFrameInfo.wWidth = dwImgWidth;
            cFrameInfo.wHeight = dwImgHeight;
            memcpy(pbFrameBuf, &cFrameInfo, iFrameInfoLen);
            HRESULT hr = E_FAIL;
            int iTimes = 0;
            while(hr == E_FAIL && iTimes < 3)
            {
                hr = g_cHvPciLinkApi.SendData( PCILINK_SEND_PANORAMIC_IMAGE,
                    pbFrameBuf, iFrameInfoLen+dwImgSize, NULL, NULL, 2000);
                iTimes++;
            }
        }
    }

    SAFE_DELETE_ARRAY(pbFrameBuf);
    pbFrameBuf = NULL;
    m_cHvCameraLink.GetImageStop();
    m_fExit = TRUE;

    return S_OK;
}

HRESULT CVideoGetter_HvCamera::Run(void* pvParam)
{
    if(m_fCaptureMode)
    {
        return RunCaptureMode();
    }
    else
    {
        return RunCommMode();
    }
}

HRESULT CVideoGetter_HvCamera::GetCurStatus(char* pszStatus, int nStatusSizes)
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

HRESULT CVideoGetter_HvCamera::SetLightType(LIGHT_TYPE cLightType, int iCplStatus)
{
    if (m_cPlateLightType != cLightType && !m_fIsLightTypeChanged)
    {
        m_cPlateLightType = cLightType;
        m_fIsLightTypeChanged = true;
        HV_Trace(5, "场景状态改变:%d\n", m_cPlateLightType);
    }
    return S_OK;
}

bool CVideoGetter_HvCamera::SetCamParameter()
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

    if (S_OK != m_cHvCameraLink.CtrtCamStart(m_pCfgCamParam->szIP))
    {
        return false;
    }

    char szCmd[255];
    HV_memset(szCmd,0,255);

    int nTypeIndex = (int)m_cPlateLightType;
    if (-1 != m_pCfgCamParam->irgExposureTime[nTypeIndex])
    {
        // 快门
        sprintf(szCmd,"SetShutter,Shutter[%d]",m_pCfgCamParam->irgExposureTime[nTypeIndex]);
        hr = m_cHvCameraLink.SendCommand(szCmd);

    }
    if (S_OK == hr && -1 != m_pCfgCamParam->irgAGCLimit[nTypeIndex])
    {
        // AGC基准
        sprintf(szCmd,"SetAgcLightBaseline,Value[%d]",m_pCfgCamParam->irgAGCLimit[nTypeIndex]);
        hr = m_cHvCameraLink.SendCommand(szCmd);
    }
    if (S_OK == hr && -1 != m_pCfgCamParam->irgGain[nTypeIndex])
    {
        // 增益
        sprintf(szCmd,"SetGain,Gain[%d]",m_pCfgCamParam->irgGain[nTypeIndex]);
        hr = m_cHvCameraLink.SendCommand(szCmd);
    }
    if (S_OK == hr && -1 != m_pCfgCamParam->iEnableAGC)
    {
        // AGC使能
        sprintf(szCmd,"SetAGCEnable,Value[%d]",m_pCfgCamParam->iEnableAGC);
        hr = m_cHvCameraLink.SendCommand(szCmd);
    }

    m_cHvCameraLink.CtrtCamStop();

    if (S_OK == hr)
    {
        HV_Trace(5, "摄像机参数改变:%d\n", nTypeIndex);
        m_fIsLightTypeChanged = false;
    }

    return hr == S_OK ? true : false;
}
