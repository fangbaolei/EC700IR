#include "ImgProcesser.h"
#include "HvDspLinkApi.h"
#include "HvPciLinkApi.h"

/* PciCmdProcess_Slave.cpp */
extern int g_iSendSlaveImage;

HRESULT AllImageAddRef(RESULT_IMAGE_STRUCT *pResultImage)
{
    if (pResultImage == NULL)
    {
        return E_POINTER;
    }

    if (pResultImage->pimgPlate)
    {
        pResultImage->pimgPlate->AddRef();
    }
    if (pResultImage->pimgBestSnapShot)
    {
        pResultImage->pimgBestSnapShot->AddRef();
    }
    if (pResultImage->pimgLastSnapShot)
    {
        pResultImage->pimgLastSnapShot->AddRef();
    }
    if (pResultImage->pimgBeginCapture)
    {
        pResultImage->pimgBeginCapture->AddRef();
    }
    if (pResultImage->pimgBestCapture)
    {
        pResultImage->pimgBestCapture->AddRef();
    }
    if (pResultImage->pimgLastCapture)
    {
        pResultImage->pimgLastCapture->AddRef();
    }
    if (pResultImage->pimgPlateBin)
    {
        pResultImage->pimgPlateBin->AddRef();
    }

    return S_OK;
}

HRESULT AllImageRelease(RESULT_IMAGE_STRUCT *pResultImage)
{
    if (pResultImage == NULL)
    {
        return E_POINTER;
    }

    SAFE_RELEASE(pResultImage->pimgPlate);
    SAFE_RELEASE(pResultImage->pimgBestSnapShot);
    SAFE_RELEASE(pResultImage->pimgLastSnapShot);
    SAFE_RELEASE(pResultImage->pimgBeginCapture);
    SAFE_RELEASE(pResultImage->pimgBestCapture);
    SAFE_RELEASE(pResultImage->pimgLastCapture);
    SAFE_RELEASE(pResultImage->pimgPlateBin);

    return S_OK;
}

HRESULT AllImageReleaseNotNull(RESULT_IMAGE_STRUCT *pResultImage)
{
    if (pResultImage == NULL)
    {
        return E_POINTER;
    }

    if (pResultImage->pimgPlate)
    {
        pResultImage->pimgPlate->Release();
    }
    if (pResultImage->pimgBestSnapShot)
    {
        pResultImage->pimgBestSnapShot->Release();
    }
    if (pResultImage->pimgLastSnapShot)
    {
        pResultImage->pimgLastSnapShot->Release();
    }
    if (pResultImage->pimgBeginCapture)
    {
        pResultImage->pimgBeginCapture->Release();
    }
    if (pResultImage->pimgBestCapture)
    {
        pResultImage->pimgBestCapture->Release();
    }
    if (pResultImage->pimgLastCapture)
    {
        pResultImage->pimgLastCapture->Release();
    }
    if (pResultImage->pimgPlateBin)
    {
        pResultImage->pimgPlateBin->Release();
    }
    return S_OK;
}

HRESULT EncodeYuv2Jpeg(
    IReferenceComponentImage *pRefImageYuv,
    IReferenceComponentImage *pRefImageJpeg,
    bool bReplace = false
)
{
    if (pRefImageYuv == NULL || pRefImageJpeg == NULL)
    {
        return E_FAIL;
    }

    JPEG_ENCODE_PARAM jpegParam;
    JPEG_ENCODE_DATA jpegData;
    JPEG_ENCODE_RESPOND jpegRespond;

    HRESULT hrTemp = E_FAIL;

    if (pRefImageYuv->IsCaptureImage())
    {
        jpegParam.dwCompressRate = g_nCaptureCompressRate;
    }
    else
    {
        jpegParam.dwCompressRate = g_nJpegCompressRate;
    }
    jpegParam.dwJpegDataType = g_nJpegType;
    pRefImageYuv->GetImage(&jpegData.hvImageYuv);
    pRefImageJpeg->GetImage(&jpegData.hvImageJpg);
    hrTemp = g_cHvDspLinkApi.SendJpegEncodeCmd(&jpegParam, &jpegData, &jpegRespond);
    if (hrTemp == S_OK)
    {
        //设置JPEG宽高
        int nWidth = jpegData.hvImageYuv.iWidth;
        int nHeight = jpegData.hvImageYuv.iHeight;
        if ( HV_IMAGE_BT1120_FIELD == jpegData.hvImageYuv.nImgType )
        {
            nHeight <<= 1;
        }
        jpegData.hvImageJpg.iHeight = (nWidth & 0xFFFF) | (nHeight << 16);
        jpegData.hvImageJpg.iWidth = jpegRespond.dwJpegLen;
        pRefImageJpeg->SetImageSize(jpegData.hvImageJpg);
        pRefImageJpeg->SetCaptureFlag(pRefImageYuv->IsCaptureImage());
        pRefImageJpeg->SetRefTime(pRefImageYuv->GetRefTime());
        //如果启用图片替换，则替换图片
        if (bReplace)
        {
            pRefImageYuv->Attach(pRefImageJpeg);
        }
    }
    else
    {
        HV_Trace(5, "Encode Jpeg Failed...\n");
    }

    return hrTemp;
}

IReferenceComponentImage *CreateJpegRefImg(int nMemHeap)
{
    IReferenceComponentImage *pImgJpeg = NULL;
    HRESULT hr = CreateReferenceComponentImage(
                     &pImgJpeg,
                     HV_IMAGE_JPEG,
#ifdef _CAMERA_PIXEL_500W_
                     2448, 2048,
#else
                     1600, 1200,
#endif
                     0, GetSystemTick(), 0,
                     "NULL", nMemHeap
                 );

    if (hr != S_OK)
    {
        HV_Trace(5, "CreateJpegRefImg return failed\n");
        pImgJpeg = NULL;
    }

    return pImgJpeg;
}

//将YUV数据转换为JPEG，目前支持的YUV数据为BT1120,RAW12,CbYCrY
//如果数据源为JPEG数据，则直接返回S_OK
HRESULT ConvertToJpeg(IReferenceComponentImage **ppRefImage, bool bReplace)
{
    HRESULT hr;
    HV_COMPONENT_IMAGE img;

    if ((*ppRefImage) == NULL || (*ppRefImage)->GetImage(&img) != S_OK)
    {
        return E_FAIL;
    }

    if (img.nImgType == HV_IMAGE_JPEG)
    {
        return S_OK;
    }

    IReferenceComponentImage *pJpeg = CreateJpegRefImg(2);
    hr = EncodeYuv2Jpeg((*ppRefImage), pJpeg, bReplace);
    if (hr == S_OK)
    {
        if (!bReplace)
        {
            (*ppRefImage)->Release();
            (*ppRefImage) = pJpeg;
        }
        else
        {
            SAFE_RELEASE(pJpeg);
        }
    }
    else
    {
        SAFE_RELEASE(pJpeg);
    }
    return hr;
}

CPhotoRecoger::CPhotoRecoger()
{
    m_iEddyType = 0;
}

CPhotoRecoger::~CPhotoRecoger()
{
}

HRESULT CPhotoRecoger::Init(TRACKER_CFG_PARAM& cTrackerCfgParam)
{
    INIT_PHOTO_RECOGER_PARAM cParam;
    cParam.nPlateRecogParamIndex = 9;
    LoadPlateRecogParam(cParam.nPlateRecogParamIndex);
    memcpy(&cParam.cTrackerCfgParam, &cTrackerCfgParam, sizeof(TRACKER_CFG_PARAM));
    //设置图片旋转标志
    m_iEddyType = cTrackerCfgParam.iEddyType;
    return g_cHvDspLinkApi.SendInitPhotoRecogerCmd(&cParam);
}

HRESULT CPhotoRecoger::ProcessPhoto(
    int iVideoId,
    IReferenceComponentImage *pImage,
    PVOID pvParam,
    PROCESS_EVENT_STRUCT* pProcessEvent
)
{
    if (pImage == NULL || pvParam == NULL)
    {
        return E_POINTER;
    }
    BOOL fIsJpegImage = FALSE;
    DWORD32 dwImageTime;

    PROCESS_PHOTO_PARAM cProcessParam;
    PROCESS_PHOTO_DATA cProcessData;
    PROCESS_PHOTO_RESPOND cProcessRespond;

    memcpy(&cProcessParam.prPram, pvParam, sizeof(cProcessParam.prPram));
    pImage->GetImage(&cProcessData.hvImage);
    if (cProcessData.hvImage.nImgType == HV_IMAGE_JPEG)
    {
        fIsJpegImage = TRUE;
    }

    dwImageTime = pImage->GetRefTime();

    // 创建YUV数据
    IReferenceComponentImage *pRefImageYuv = NULL;
    HRESULT hr = E_FAIL;
    int iImageWidth = 0;
    int iImageHeight = 0;
    HV_IMAGE_TYPE emImageType = (HV_IMAGE_TYPE)(-1);

    if (fIsJpegImage)
    {
        iImageWidth = cProcessData.hvImage.iHeight & 0x0000FFFF;
        iImageHeight = cProcessData.hvImage.iHeight >> 16;
        emImageType = HV_IMAGE_CbYCrY;
    }
    else if (cProcessData.hvImage.nImgType == HV_IMAGE_BT1120_FIELD
             || cProcessData.hvImage.nImgType == HV_IMAGE_BT1120
             || cProcessData.hvImage.nImgType == HV_IMAGE_BT1120_ROTATE_Y)
    {
        iImageWidth = cProcessData.hvImage.iWidth;
        iImageHeight = cProcessData.hvImage.iHeight;
        emImageType = HV_IMAGE_YUV_422;
    }

    hr = CreateReferenceComponentImage(
             &pRefImageYuv, emImageType,
             m_iEddyType ? iImageHeight:iImageWidth,
             m_iEddyType ? iImageWidth:iImageHeight,
             0, dwImageTime, 0,
             "NULL", 2
         );

    if (S_OK == hr)
    {
        pRefImageYuv->GetImage(&cProcessData.hvImageYuv);
    }
    else
    {
        HV_Trace(1, "Create yuv data failed!!!\n");
        return E_OUTOFMEMORY;
    }

    // 创建小图数据
    for (int i = 0; i < MAX_EVENT_COUNT; i++)
    {
        hr = CreateReferenceComponentImage(
                 &cProcessParam.rgpSmallImage[i],
                 HV_IMAGE_YUV_422,
                 400,   // 小图最大宽
                 80,   // 小图最大高
                 0,
                 GetSystemTick(),
                 0,
                 "NULL",
                 2
             );
        if (S_OK == hr)
        {
            cProcessParam.rgpSmallImage[i]->GetImage(&cProcessData.rghvImageSmall[i]);
        }
        else
        {
            HV_Trace(5, "Create small image data failed!!!\n");
        }

        hr = CreateReferenceComponentImage(
                 &cProcessParam.rgpBinImage[i],
                 HV_IMAGE_BIN,
                 BINARY_IMAGE_WIDTH,
                 BINARY_IMAGE_HEIGHT,
                 0,
                 GetSystemTick(),
                 0,
                 "NULL",
                 2
             );
        if (S_OK == hr)
        {
            cProcessParam.rgpBinImage[i]->GetImage(&cProcessData.rghvImageBin[i]);
        }
        else
        {
            HV_Trace(5, "Create Bin image data failed!!!\n");
        }
    }

    hr = g_cHvDspLinkApi.SendProcessPhotoCmd(&cProcessParam, &cProcessData, &cProcessRespond);

    if (S_OK == hr)
    {
        pProcessEvent->dwEventId |= EVENT_CARLEFT;
        pProcessEvent->iCarLeftInfoCount = cProcessRespond.iResultCount;

        IReferenceComponentImage *pJpeg = NULL;
        IReferenceComponentImage *pYuv = NULL;

        if (pProcessEvent->iCarLeftInfoCount > 0 && pProcessEvent->iCarLeftInfoCount < MAX_EVENT_COUNT)
        {
            for (int i = 0; i < pProcessEvent->iCarLeftInfoCount; i++)
            {
                memcpy(
                    &pProcessEvent->rgCarLeftInfo[i].cCoreResult,
                    &cProcessRespond.rgProcessPhotoResult[i],
                    sizeof(PROCESS_IMAGE_CORE_RESULT)
                );

                pProcessEvent->rgCarLeftInfo[i].cCoreResult.nCarArriveTime = dwImageTime;

                if (!fIsJpegImage)
                {
                    if (cProcessData.hvImage.nImgType == HV_IMAGE_BT1120_FIELD
                            || cProcessData.hvImage.nImgType == HV_IMAGE_BT1120
                            || cProcessData.hvImage.nImgType == HV_IMAGE_BT1120_ROTATE_Y)
                    {
                        pYuv = pImage;
                    }
                    else
                    {
                        pYuv = pRefImageYuv;
                    }
                    pJpeg = CreateJpegRefImg(2);
                    EncodeYuv2Jpeg(pYuv, pJpeg);
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBestSnapShot = pJpeg;
                }
                else
                {
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBestSnapShot = pImage;
                    pImage->AddRef();
                }

                if (cProcessRespond.rgProcessPhotoResult[i].rcBestPlatePos.right - cProcessRespond.rgProcessPhotoResult[i].rcBestPlatePos.left == 0
                        || cProcessRespond.rgProcessPhotoResult[i].rcBestPlatePos.bottom - cProcessRespond.rgProcessPhotoResult[i].rcBestPlatePos.top == 0)
                {
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlate = NULL;
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlateBin = NULL;
                }
                else
                {
                    cProcessData.rghvImageSmall[i].iWidth =
                        cProcessRespond.rgProcessPhotoResult[i].rcBestPlatePos.right - cProcessRespond.rgProcessPhotoResult[i].rcBestPlatePos.left;
                    cProcessData.rghvImageSmall[i].iHeight =
                        cProcessRespond.rgProcessPhotoResult[i].rcBestPlatePos.bottom - cProcessRespond.rgProcessPhotoResult[i].rcBestPlatePos.top;
                    cProcessData.rghvImageSmall[i].iStrideWidth[0] =
                        (cProcessData.rghvImageSmall[i].iWidth + 3) & (~3);
                    cProcessData.rghvImageSmall[i].iStrideWidth[1] =
                        cProcessData.rghvImageSmall[i].iStrideWidth[0] >> 1;
                    cProcessData.rghvImageSmall[i].iStrideWidth[2] =
                        cProcessData.rghvImageSmall[i].iStrideWidth[0] >> 1;

                    cProcessParam.rgpSmallImage[i]->SetImageSize(cProcessData.rghvImageSmall[i]);
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlate = cProcessParam.rgpSmallImage[i];
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlate->AddRef();

                    cProcessParam.rgpBinImage[i]->SetImageSize(cProcessData.rghvImageBin[i]);
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlateBin = cProcessParam.rgpBinImage[i];
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlateBin->AddRef();
                }
            }
        }
        // 无车牌情况
        else if (pProcessEvent->iCarLeftInfoCount == 0)
        {
            pProcessEvent->iCarLeftInfoCount = 1;
            pProcessEvent->rgCarLeftInfo[0].cCoreResult.nCarArriveTime = dwImageTime;
            if (!fIsJpegImage)
            {
                if (cProcessData.hvImage.nImgType == HV_IMAGE_BT1120_FIELD)
                {
                    pYuv = pImage;
                }
                else
                {
                    pYuv = pRefImageYuv;
                }
                pJpeg = CreateJpegRefImg(2);
                EncodeYuv2Jpeg(pYuv, pJpeg);
                pProcessEvent->rgCarLeftInfo[0].cCoreResult.cResultImg.pimgBestSnapShot = pJpeg;
            }
            else
            {
                pProcessEvent->rgCarLeftInfo[0].cCoreResult.cResultImg.pimgBestSnapShot = pImage;
                pImage->AddRef();
            }
        }
    }

    // 释放YUV数据
    SAFE_RELEASE(pRefImageYuv);

    for (int i = 0; i < MAX_EVENT_COUNT; i++)
    {
        SAFE_RELEASE(cProcessParam.rgpSmallImage[i]);
        SAFE_RELEASE(cProcessParam.rgpBinImage[i]);
    }

    return hr;
}

HRESULT CPhotoRecoger::GetCurStatus(char* pszStatus, int nStatusSizes)
{
    // 由于并没有继承线程类，因此直接返回S——OK
    return S_OK;
}


////////////////////////////////////////////////////////////////////////
unsigned char *g_pData;
CVideoRecoger::CVideoRecoger()
{
    CreateSemaphore(&m_hLockProcess, 1, 1);
    m_nEddyType = 0;
    m_iEncodeType = 0;
}

CVideoRecoger::~CVideoRecoger()
{
    DestroySemaphore(&m_hLockProcess);
}

HRESULT CVideoRecoger::Init(int iGlobalParamIndex, int nLastLightType,int nLastPulseLevel, int nLastCplStatus, TRACKER_CFG_PARAM& cTrackerCfgParam)
{
    INIT_VIDEO_RECOGER_PARAM cInitParam;
    cInitParam.nPlateRecogParamIndex = iGlobalParamIndex;
    cInitParam.nLastLightType = nLastLightType;
    cInitParam.nLastPulseLevel = nLastPulseLevel;
    cInitParam.nLastCplStatus = nLastCplStatus;
    cInitParam.dwArmTick = GetSystemTick();
#ifdef SINGLE_BOARD_PLATFORM
    cTrackerCfgParam.iPlatform = 1;
#else
    cTrackerCfgParam.iPlatform = 0;
#endif
    LoadPlateRecogParam(iGlobalParamIndex);
    memcpy(&(cInitParam.cTrackerCfgParam), &cTrackerCfgParam, sizeof(cInitParam.cTrackerCfgParam));
    m_nEddyType = cTrackerCfgParam.iEddyType;
    return g_cHvDspLinkApi.SendInitVideoRecogerCmd(&cInitParam);
}

HRESULT CVideoRecoger::ProcessOneFrame(
    int iVideoID,
    IReferenceComponentImage *pImage,
    IReferenceComponentImage *pCapImage,
    PVOID pvParam,
    PROCESS_EVENT_STRUCT* pProcessEvent
)
{
    if (NULL == pImage || NULL == pProcessEvent)
    {
        return E_POINTER;
    }

#ifdef SINGLE_BOARD_PLATFORM
    if (!m_cTSServer.IsValid())
    {
        m_cTSServer.Create(9876);
    }
#endif
    int iDiffTime = 0;
    if ( NULL != pvParam )
    {
        iDiffTime = (int)(*(int*)pvParam);
    }
    SemPend(&m_hLockProcess);

    BOOL fIsJpegImage = FALSE;

    PROCESS_ONE_FRAME_PARAM cProcessParam;
    PROCESS_ONE_FRAME_DATA cProcessData;
    PROCESS_ONE_FRAME_RESPOND cProcessRespond;

    cProcessParam.dwIOLevel = pProcessEvent->nIOLevel;
    cProcessParam.iCarArriveTriggerType = pProcessEvent->iCarArriveTriggerType;
    cProcessParam.fIsCaptureImage = pImage->IsCaptureImage();
    cProcessParam.iVideoID = iVideoID;
    cProcessParam.dwFrameNo = pImage->GetFrameNo();
    cProcessParam.dwImageTime = pImage->GetRefTime();
    cProcessParam.iDiffTick = iDiffTime;
    cProcessParam.fIsCheckLightType = pProcessEvent->fIsCheckLight;
    pImage->GetImage(&cProcessData.hvImage);

    cProcessParam.pCurFrame = pImage;
    if (cProcessData.hvImage.nImgType == HV_IMAGE_JPEG)
    {
        fIsJpegImage = TRUE;
    }
    // 创建YUV数据
    IReferenceComponentImage *pRefImageYuv = NULL;
    HRESULT hr = S_OK;
    if (fIsJpegImage)
    {
        int width = cProcessData.hvImage.iHeight & 0x0000FFFF;
        int height = cProcessData.hvImage.iHeight >> 16;
        //需要创建静态共享内存,能如此做的原因是因为识别完毕之后，该内存不在被引用，避免网络发送线程占用太多共享内存导致无法识别
        static IReferenceComponentImage * yuvImage = NULL;
        if (!yuvImage)
        {
            // 由于GBA解码库解码300W图片快，而对200W图片解码慢
            // 因此300W图片用GBA解码库，其它用TI解码库
            // huanggr 2011-12-01
            if (width > 2000 && width < 2100)
            {
                hr = CreateReferenceComponentImage(
                         &yuvImage,
                         HV_IMAGE_YUV_422,
                         m_nEddyType ? height : width,
                         m_nEddyType ? (width + 1) / 2 : (height + 1) / 2,
                         0,
                         cProcessParam.dwImageTime,
                         0,
                         "NULL",
                         2
                     );
            }
            else
            {
                hr = CreateReferenceComponentImage(
                         &yuvImage,
                         HV_IMAGE_CbYCrY,
                         m_nEddyType ? height : width,
                         m_nEddyType ? width : height,
                         0,
                         cProcessParam.dwImageTime,
                         0,
                         "NULL",
                         2
                     );
            }
        }
        if (yuvImage)
        {
            pRefImageYuv = yuvImage;
            pRefImageYuv->SetRefTime(cProcessParam.dwImageTime);
            pRefImageYuv->AddRef();
            hr = S_OK;
#ifdef SINGLE_BOARD_PLATFORM
            static DWORD32 dwNowTime = GetSystemTick();
            if (GetSystemTick() - dwNowTime > 1000 && m_cTSServer.HasClient())
            {
                dwNowTime = GetSystemTick();
                //发送调试信息到客户端
                HV_COMPONENT_IMAGE imgFrame;
                yuvImage->GetImage(&imgFrame);

                m_cTSServer.Send(&imgFrame.iWidth, sizeof(int))
                && m_cTSServer.Send(&imgFrame.iHeight, sizeof(int))
                && m_cTSServer.Send(GetHvImageData(&imgFrame, 0), 2 * imgFrame.iWidth * imgFrame.iHeight);
            }
#endif
        }
        else
        {
            HV_Trace(5, "create yuvimage failed.\n");
            SemPost(&m_hLockProcess);
            return E_FAIL;
        }
    }
    else
    {
        pRefImageYuv = pImage;
        pImage->AddRef();
    }
    if (S_OK == hr)
    {
        pRefImageYuv->GetImage(&cProcessData.hvImageYuv);
    }
    else
    {
        HV_Trace(5, "Create yuv data failed!!!\n");
        SemPost(&m_hLockProcess);
        return E_OUTOFMEMORY;
    }
    BOOL fIsAllocOk = TRUE;
    // 创建小图数据
    for (int i = 0; i < MAX_EVENT_COUNT; i++)
    {
        HRESULT hr = CreateReferenceComponentImage(
                         &cProcessParam.rgpSmallImage[i],
                         HV_IMAGE_YUV_422,
                         400,   // 小图最大宽
                         80,   // 小图最大高
                         0,
                         GetSystemTick(),
                         0,
                         "NULL",
                         2
                     );
        if (S_OK == hr)
        {
            cProcessParam.rgpSmallImage[i]->GetImage(&cProcessData.rghvImageSmall[i]);
        }
        else
        {
            HV_Trace(5, "Create small image data failed!!!\n");
            fIsAllocOk = FALSE;
            break;
        }

        hr = CreateReferenceComponentImage(
                 &cProcessParam.rgpBinImage[i],
                 HV_IMAGE_BIN,
                 BINARY_IMAGE_WIDTH,
                 BINARY_IMAGE_HEIGHT,
                 0,
                 GetSystemTick(),
                 0,
                 "NULL",
                 2
             );
        if (S_OK == hr)
        {
            cProcessParam.rgpBinImage[i]->GetImage(&cProcessData.rghvImageBin[i]);
        }
        else
        {
            HV_Trace(5, "Create Bin image data failed!!!\n");
            fIsAllocOk = FALSE;
            break;
        }
    }
    if (fIsAllocOk)
    {
        hr = g_cHvDspLinkApi.SendProcessOneFrameCmd(&cProcessParam, &cProcessData, &cProcessRespond);
    }
    else
    {
        hr = E_FAIL;
    }
    if (S_OK == hr)
    {
        pProcessEvent->dwEventId = cProcessRespond.cTrigEvent.dwEventId;
        pProcessEvent->iCarArriveInfoCount = cProcessRespond.cTrigEvent.iCarArriveCount;
        pProcessEvent->iCarLeftInfoCount = cProcessRespond.cTrigEvent.iCarLeftCount;
        pProcessEvent->cLightType = cProcessRespond.cLightType;
        pProcessEvent->iCplStatus = cProcessRespond.iCplStatus;
        pProcessEvent->iPulseLevel = cProcessRespond.iPulseLevel;
        pProcessEvent->cTrackRectInfo = cProcessRespond.cTrackRectInfo;
        pProcessEvent->iFrameAvgY = cProcessRespond.iFrameAvgY;
        pProcessEvent->fIsNight = (cProcessRespond.cTrigEvent.rgCarLeftCoreInfo[0].nVoteCondition == 1) ? true : false;
        //拷贝红灯坐标，要求这2个数组的大小一致
        memcpy(pProcessEvent->rcRedLight, cProcessRespond.rcRedLight, sizeof(pProcessEvent->rcRedLight));
        //
        if (pProcessEvent->cTrackRectInfo.dwTrackCount)
        {
            //    HV_Trace(5, "pProcessEvent->cTrackRectInfo.dwTrackCount=%d\n", pProcessEvent->cTrackRectInfo.dwTrackCount);
        }
        // CarArrive
        if (pProcessEvent->dwEventId & EVENT_CARARRIVE)
        {
            for (int i = 0; i < pProcessEvent->iCarArriveInfoCount; i++)
            {
                memcpy(&pProcessEvent->rgCarArriveInfo[i], &cProcessRespond.cTrigEvent.rgCarArriveInfo[i], sizeof(CARARRIVE_INFO_STRUCT));
            }
        }
        // CarLeft
        // zhaopy
        DWORD32 dwBegin = GetSystemTick();
        if (pProcessEvent->dwEventId & EVENT_CARLEFT)
        {
            HV_Trace(5, "pProcessEvent->iCarLeftInfoCount=%d\n", pProcessEvent->iCarLeftInfoCount);
            static char szPlateName[32];
            static int iCarLeftCount = 0;
            for (int i = 0; i < pProcessEvent->iCarLeftInfoCount; i++)
            {
                GetPlateNameAlpha(
                    (char*)szPlateName,
                    ( PLATE_TYPE )cProcessRespond.cTrigEvent.rgCarLeftCoreInfo[i].nType,
                    ( PLATE_COLOR )cProcessRespond.cTrigEvent.rgCarLeftCoreInfo[i].nColor,
                    cProcessRespond.cTrigEvent.rgCarLeftCoreInfo[i].rgbContent
                );
                HV_Trace(5, "PlateCount = %d, Tracker == %s\n", ++iCarLeftCount, szPlateName);

                memcpy(
                    &pProcessEvent->rgCarLeftInfo[i].cCoreResult,
                    &cProcessRespond.cTrigEvent.rgCarLeftCoreInfo[i],
                    sizeof(PROCESS_IMAGE_CORE_RESULT)
                );

                if (m_iEncodeType == 0 && S_OK == ConvertToJpeg(&pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBestSnapShot, true))
                {
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBestSnapShot->AddRef();
                }
                else
                {
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBestSnapShot = NULL;
                }

                if (m_iEncodeType == 0 && S_OK == ConvertToJpeg(&pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgLastSnapShot, true))
                {
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgLastSnapShot->AddRef();
                }
                else
                {
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgLastSnapShot = NULL;
                }

                if (S_OK == ConvertToJpeg(&pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBeginCapture, true))
                {
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBeginCapture->AddRef();
                }
                else
                {
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBeginCapture = NULL;
                }

                if ((m_iEncodeType == 2 || pProcessEvent->rgCarLeftInfo[i].cCoreResult.ptType != PT_NORMAL) && S_OK == ConvertToJpeg(&pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBestCapture, true))
                {
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBestCapture->AddRef();
                }
                else
                {
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBestCapture = NULL;
                }

                if ((m_iEncodeType == 2 || pProcessEvent->rgCarLeftInfo[i].cCoreResult.ptType != PT_NORMAL) && S_OK == ConvertToJpeg(&pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgLastCapture, true))
                {
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgLastCapture->AddRef();
                }
                else
                {
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgLastCapture = NULL;
                }

                if (cProcessRespond.cTrigEvent.rgCarLeftCoreInfo[i].nPlateWidth == 0
                        || cProcessRespond.cTrigEvent.rgCarLeftCoreInfo[i].nPlateHeight == 0)
                {
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlate = NULL;
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlateBin = NULL;
                }
                else
                {
                    cProcessData.rghvImageSmall[i].iWidth =
                        cProcessRespond.cTrigEvent.rgCarLeftCoreInfo[i].nPlateWidth;
                    cProcessData.rghvImageSmall[i].iHeight =
                        cProcessRespond.cTrigEvent.rgCarLeftCoreInfo[i].nPlateHeight;
                    cProcessData.rghvImageSmall[i].iStrideWidth[0] =
                        (cProcessData.rghvImageSmall[i].iWidth + 3) & (~3);
                    cProcessData.rghvImageSmall[i].iStrideWidth[1] =
                        cProcessData.rghvImageSmall[i].iStrideWidth[0] >> 1;
                    cProcessData.rghvImageSmall[i].iStrideWidth[2] =
                        cProcessData.rghvImageSmall[i].iStrideWidth[0] >> 1;

                    cProcessParam.rgpSmallImage[i]->SetImageSize(cProcessData.rghvImageSmall[i]);
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlate = cProcessParam.rgpSmallImage[i];
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlate->AddRef();

                    cProcessParam.rgpBinImage[i]->SetImageSize(cProcessData.rghvImageBin[i]);
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlateBin = cProcessParam.rgpBinImage[i];
                    pProcessEvent->rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlateBin->AddRef();
                }
            }
        }
        // zhaopy
        if ( pProcessEvent->iCarLeftInfoCount > 0 )
        {
            HV_Trace(5, "<Processer> Process carleft relay : %d. plate:%d\n",
                     GetSystemTick() - dwBegin, pProcessEvent->iCarLeftInfoCount);
        }
        //处理图片内存LOG
        HV_Trace(cProcessRespond.cImgMemOperLog.nCount > 100 ? 5 : 3, "cImgMemOperLog = %d, max = 1024\n", cProcessRespond.cImgMemOperLog.nCount);
        //先对图片++
        for (int i = 0; i < cProcessRespond.cImgMemOperLog.nCount; i++)
        {
            if (cProcessRespond.cImgMemOperLog.rgOperInfo[i].nOperFlag == IOF_ADD_REF)
            {
                cProcessRespond.cImgMemOperLog.rgOperInfo[i].pMemAddr->AddRef();
            }
        }
        //然后在对图片--
        for (int i = 0; i < cProcessRespond.cImgMemOperLog.nCount; i++)
        {
            if (cProcessRespond.cImgMemOperLog.rgOperInfo[i].nOperFlag == IOF_RELEASE)
            {
                cProcessRespond.cImgMemOperLog.rgOperInfo[i].pMemAddr->Release();
            }
        }
    }
    else
    {
        HV_Trace(5, "g_cHvDspLinkApi.SendProcessOneFrameCmd error, hr = %d, framename=%s\n", hr, pImage->GetFrameName() ? pImage->GetFrameName() : "");
    }

    for (int i = 0; i < MAX_EVENT_COUNT; i++)
    {
        SAFE_RELEASE(cProcessParam.rgpSmallImage[i]);
        SAFE_RELEASE(cProcessParam.rgpBinImage[i]);
    }

    SAFE_RELEASE(pRefImageYuv);

    SemPost(&m_hLockProcess);
    return hr;
}

HRESULT CVideoRecoger::GetCurStatus(char* pszStatus, int nStatusSizes)
{
    // 由于并没有继承线程类，因此直接返回S_OK
    return S_OK;
}

HRESULT CCamLightTypeSaver::GetLastLightType(int& nLightType, int& nPulseLevel , int& nCplStatus)
{
    FILE* pFile = fopen(CAM_LIGHTTYPE_FILENAME, "rb");
    if (pFile == NULL)
    {
        HV_Trace(5, "Can not found light type file, use default light type!");
        return E_FAIL;
    }

    SAVELIGHTTYPE_INFO cLightTypeInfo;
    size_t nSize = sizeof(cLightTypeInfo);
    if (nSize != fread(&cLightTypeInfo, 1, nSize, pFile))
    {
        fclose(pFile);
        HV_Trace(5, "Read light type failed!");
        return E_FAIL;
    }

    fclose(pFile);

    DWORD32 dwCurTick = GetSystemTick(), dwTimeHigh = 0, dwTimeLow = 0;
    ConvertTickToSystemTime(dwCurTick, dwTimeLow, dwTimeHigh);

    DWORD64 dw64CurTime = dwTimeHigh;
    dw64CurTime = (dw64CurTime << 32) | dwTimeLow;

    // 如果最后一次保存的相机亮度等级时间距离当前时间超过10分钟则使用默认等级
    if (dw64CurTime < cLightTypeInfo.dw64LastTime
            || (dw64CurTime - cLightTypeInfo.dw64LastTime) > 600000)
    {
        HV_Trace(5, "The time of last result is too old, use default light type!");
        return S_FALSE;
    }

    nLightType = cLightTypeInfo.nLightType;
    nPulseLevel = cLightTypeInfo.nPulseLevel;
    nCplStatus = cLightTypeInfo.nCplStatus;
    HV_Trace(5, "Get light type succ! Current light type = %d , pulseLevel:%d\n", nLightType , nPulseLevel);
    return S_OK;
}

HRESULT CCamLightTypeSaver::SaveLightType(int nLightType, int nPulseLevel ,int nCplStatus , UINT nCarArriveTime)
{
    static DWORD32 dwLastTick = 0;
    DWORD32 dwCurTick = GetSystemTick();

    // 1分钟保存一次相机亮度等级
    if ((dwCurTick - dwLastTick) < 60000 && dwLastTick != 0)
    {
        return S_FALSE;
    }

    FILE* pFile = fopen(CAM_LIGHTTYPE_FILENAME, "wb");
    if (pFile == NULL)
    {
        HV_Trace(5, "Open light type file failed!");
        return E_FAIL;
    }

    DWORD32 dwTimeHigh = 0, dwTimeLow = 0;
    ConvertTickToSystemTime(nCarArriveTime, dwTimeLow, dwTimeHigh);

    SAVELIGHTTYPE_INFO cLightTypeInfo;
    cLightTypeInfo.nLightType = nLightType;
    cLightTypeInfo.nPulseLevel = nPulseLevel;
    cLightTypeInfo.dw64LastTime = dwTimeHigh;
    cLightTypeInfo.dw64LastTime = (cLightTypeInfo.dw64LastTime << 32) | dwTimeLow;
    cLightTypeInfo.nCplStatus = nCplStatus;
    size_t nSize =  sizeof(cLightTypeInfo);

    if (nSize != fwrite(&cLightTypeInfo, 1, nSize, pFile))
    {
        fclose(pFile);
        HV_Trace(5, "Save light type failed! Current light type = %d", nLightType);
        return E_FAIL;
    }

    dwLastTick = dwCurTick;

    fclose(pFile);

    HV_Trace(5, "Save light type succ! Current light type:%d , pulseLevel:%d, CplStatus:%d,nCarArriveTime:%d, nowTime:%d\n", nLightType , nPulseLevel, nCplStatus,nCarArriveTime, dwLastTick);
    return S_OK;
}



