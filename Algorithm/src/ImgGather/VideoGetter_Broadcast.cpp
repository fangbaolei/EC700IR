// 该文件编码格式必须是WINDOWS-936格式

#include "ImgGatherer.h"
#include "HvSockUtils.h"
#include "hvtarget_ARM.h"

using namespace HiVideo;

CVideoGetter_Broadcast::CVideoGetter_Broadcast()
{
    m_dwFrameCount = 0;
}

CVideoGetter_Broadcast::~CVideoGetter_Broadcast()
{
    Stop(-1);
}

HRESULT CVideoGetter_Broadcast::Run(void* pvParam)
{
    IMG_FRAME frame;
    DWORD32 dwImgWidth = 1600;
    DWORD32 dwImgHeight = 1200;

    if (S_OK != m_cBroadcastLink.Initialize())
    {
        HV_Trace(5, "BroadcastLink initialize failed!\n");
        return E_FAIL;
    }

    m_dwLastTime = GetSystemTick();

    while (!m_fExit)
    {
        m_dwLastTime = GetSystemTick();

        // 创建JPEG引用对象
        IReferenceComponentImage *pRefImage;
        HRESULT hr = CreateReferenceComponentImage(
                         &pRefImage, HV_IMAGE_JPEG,
                         dwImgWidth, dwImgHeight,
                         m_dwFrameCount++, 0, 0,
                         "NULL", 2
                     );
        if (S_OK != hr)
        {
            HV_Trace(5, "CVideoGetter_Broadcast::Run(): CreateReferenceComponentImage() failed!\n");
            continue;
        }

        HV_COMPONENT_IMAGE imgFrame;
        pRefImage->GetImage(&imgFrame);

        DWORD32 dwRefTime = 0;
        char szFrameName[260];

        hr =  m_cBroadcastLink.GetImage(
                  (char*)GetHvImageData(&imgFrame, 0),
                  (PDWORD32)&imgFrame.iWidth,
                  &dwRefTime,
                  szFrameName
              );

        if (hr != S_OK)
        {
            SetLan1LedOff();
            HV_Trace(5, "CVideoGetter_Broadcast::Run(): GetImage() failed!\n");
            pRefImage->Release();
            m_cBroadcastLink.Initialize();
            continue;
        }

        imgFrame.iHeight = dwImgWidth | (dwImgHeight << 16);
        pRefImage->SetImageSize(imgFrame);
        pRefImage->SetRefTime(dwRefTime);
        pRefImage->SetFrameName(szFrameName);

        Lan1LedLight();

        // 传递到缓冲队列
        frame.pRefImage = pRefImage;
        frame.iVideoID = 0;
        PutOneFrame(frame);
        pRefImage->Release();
    }

    m_fExit = TRUE;
    SetLan1LedOff();

    return S_OK;
}

HRESULT CVideoGetter_Broadcast::GetCurStatus(char* pszStatus, int nStatusSizes)
{
    /*if (GetSystemTick() - m_dwLastTime > 20000)
    {
        return E_FAIL;
    }*/
    return S_OK;
}
