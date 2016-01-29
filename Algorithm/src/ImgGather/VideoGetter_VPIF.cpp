#include "VideoGetter_VPIF.h"
#include "HvDspLinkApi.h"
#include "HvPciLinkApi.h"
#include "slw_dev.h"
#include "misc.h"
#include "hvtarget_ARM.h"
#include "math.h"

using namespace HiVideo;

extern HV_SEM_HANDLE g_hSemEDMA;
#ifdef _CAM_APP_
/* CamApp的main.cpp */
//extern HV_SEM_HANDLE g_hSemEDMA;
#include "../CamApp/DataCtrl.h"
#endif

/* DataCtrl.cpp */
extern int g_iControllPannelWorkStyle;

#ifdef _CAM_APP_
// 60ms
static int SplitUV(
    int iSwDevFd,
    PBYTE8 pbSrcImgUV,
    PBYTE8 pbDstImgU,
    PBYTE8 pbDstImgV
)
{
    int iRet = 0;
    if ( 1 == g_pCamApp->GetJpegStreamParam() )
    {
        SemPend(&g_hSemEDMA);
        iRet = SwDmaCopyUV(
                   iSwDevFd,
                   pbSrcImgUV,
                   pbDstImgU, pbDstImgV,
                   IMAGE_WIDTH, IMAGE_HEIGHT
               );
        SemPost(&g_hSemEDMA);
    }
    return iRet;
}
#endif // _CAM_APP_

static int BT1120ToBT1120RotateY(
    int iSwDevFd,
    PBYTE8 pbSrcImgY,
    PBYTE8 pbSrcImgUV,
    PBYTE8 pbDstImgY,
    PBYTE8 pbDstImgUV
)
{
#ifdef SINGLE_BOARD_PLATFORM
    return -1;
#else
//    #ifdef _CAM_APP_
    SemPend(&g_hSemEDMA);
//    #endif
    int iRet = SwDmaRotationYUV(
                   iSwDevFd,
                   pbSrcImgY, pbSrcImgUV,
                   pbDstImgY, pbDstImgUV,
                   IMAGE_WIDTH, IMAGE_HEIGHT
               );
//    #ifdef _CAM_APP_
    SemPost(&g_hSemEDMA);
//    #endif
    return iRet;
#endif  // #ifdef SINGLE_BOARD_PLATFORM
}

// --------------------- CVideoGetter_VPIF -------------------------

CVideoGetter_VPIF::CVideoGetter_VPIF()
        : m_dwLastThreadIsOkTime(0)
        , m_eRunMode(SRM_VIDEO)
        , m_pSignalMatch(NULL)
        , m_pCamCfgParam(NULL)
        , m_nCurLightType(LIGHT_TYPE_COUNT)
        , m_iCurCplStatus(-1)
        , m_fNeedToUpdateParam(TRUE)
        , m_dwFrameCount(0)
        , m_iImageWidth(IMAGE_WIDTH)
        , m_iImageHeight(IMAGE_HEIGHT)
        , m_iEddyType(0)
        , m_VpifFd(-1)
        , m_SwDevFd(-1)
        , m_fIsChangeCamParam(false)
        , m_fPCISending(false)
{
}

CVideoGetter_VPIF::~CVideoGetter_VPIF()
{
    Stop(-1);
}

HRESULT CVideoGetter_VPIF::GetCurStatus(char* pszStatus, int nStatusSizes)
{
    DWORD32 dwCurTick = GetSystemTick();

    if ( !m_fPCISending && m_dwLastThreadIsOkTime > 0 && abs(int(dwCurTick - m_dwLastThreadIsOkTime)) > 20000 )
    {
        HV_Trace(5, "CVideoGetter_VPIF::GetCurStatus error [CurTick = %dms, LastOkTime = %dms]\n",
                 dwCurTick, m_dwLastThreadIsOkTime);

        return E_FAIL;
    }

    return S_OK;
}

/*
// 调试输出BT1120的数据行
static void DebugOutputImageLine(PBYTE8 pbStartAddr, int nLine)
{
    printf("\n------------[line:%d]-------------\n", nLine);
    PBYTE8 pbTmp = pbStartAddr + IMAGE_WIDTH * (nLine-1);
    for ( int x = 0; x<IMAGE_WIDTH; ++x )
    {
        printf("0x%02x ", (int)*pbTmp);
        ++pbTmp;
    }
}
*/

HRESULT CVideoGetter_VPIF::OpenVPIF()
{
    m_VpifFd = open_capture(0);  //开启0号视频设备
    if (m_VpifFd < 0)
    {
        HV_Trace(5, "<CVideoGetter_VPIF::OpenVPIF> open_capture failed, m_VpifFd:0x%x.\n", m_VpifFd);
        HV_Exit(HEC_FAIL|HEC_RESET_DEV, "open_capture failed");
        return E_FAIL;
    }

    int ret = init_capture(m_VpifFd);
    if (ret < 0)
    {
        close_capture(m_VpifFd);
        HV_Trace(5, "<CVideoGetter_VPIF::OpenVPIF> init_capture failed, ret:0x%x.\n", ret);
        HV_Exit(HEC_FAIL|HEC_RESET_DEV, "init_capture failed");
        return E_FAIL;
    }

    ret = start_capture(m_VpifFd);
    if (ret < 0)
    {
        close_capture(m_VpifFd);
        HV_Trace(5, "<CVideoGetter_VPIF::OpenVPIF> start_capture failed, ret:0x%x.\n", ret);
        HV_Exit(HEC_FAIL|HEC_RESET_DEV, "start_capture failed");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CVideoGetter_VPIF::CloseVPIF()
{
    stop_capture(m_VpifFd);
    close_capture(m_VpifFd);
    return S_OK;
}

#ifdef _CAMERA_PIXEL_500W_
// 运行在“500w视频流模式”下
HRESULT CVideoGetter_VPIF::RunVideoMode(void*pvParam)
{
    m_dwLastThreadIsOkTime = GetSystemTick();

    if ( S_OK != OpenVPIF() )
    {
        return E_FAIL;
    }

    IMG_DATA cImgData;

    while (!m_fExit)  // 视频帧处理主循环
    {
        m_dwLastThreadIsOkTime = GetSystemTick();

        int iRet = get_capture_buffer(m_VpifFd, &cImgData);
        if ( 0 != iRet )
        {
            if ( 1 == iRet )
            {
                HV_Sleep(50);
                continue;
            }
            else
            {
                HV_Trace(5, "get_capture_buffer is error!\n");
                HV_Exit(HEC_FAIL|HEC_RESET_DEV, "get_capture_buffer is error!");
            }
        }

        if ( cImgData.addr != NULL && IMAGE_DATA_SIZE == cImgData.iDataLen )
        {
            HV_COMPONENT_IMAGE imgVPIF;
            imgVPIF.rgImageData[0].addr = (PBYTE8)cImgData.addr;
            imgVPIF.rgImageData[1].addr = (PBYTE8)cImgData.addr + IMAGE_WIDTH*IMAGE_HEIGHT;
            imgVPIF.rgImageData[2].addr = NULL;
            imgVPIF.rgImageData[0].phys = (unsigned int)cImgData.offset;
            imgVPIF.rgImageData[1].phys = (unsigned int)((PBYTE8)cImgData.offset + IMAGE_WIDTH*IMAGE_HEIGHT);
            imgVPIF.rgImageData[2].phys = (unsigned int)NULL;
            imgVPIF.iStrideWidth[0] = IMAGE_WIDTH;
            imgVPIF.iStrideWidth[1] = IMAGE_WIDTH;
            imgVPIF.iStrideWidth[2] = 0;
            imgVPIF.iHeight = IMAGE_HEIGHT;
            imgVPIF.iWidth = IMAGE_WIDTH;
            imgVPIF.nImgType = HV_IMAGE_BT1120;
            imgVPIF.iMemPos = 2;

            IMG_FRAME imgFrame;

            // 提取BT1120流中的特殊值
            g_cCameraController.GetExtInfoByImage(
                (unsigned char*)imgVPIF.rgImageData[0].addr,
                imgFrame.cFpgaRegInfo,
                imgFrame.cAgcAwbInfo
            );
            imgFrame.imgVPIF = imgVPIF;
            imgFrame.fUseImgVPIF = TRUE;

            //imgFrame.dwTimeTick = GetSystemTick(); //FPGA读出时间不对，临时用系统时间
            imgFrame.dwTimeTick = imgFrame.cFpgaRegInfo.cFpgaRegInfoA.time_cnt_out;

            PutOneFrame(imgFrame);
        }
    }  // end while (!m_fExit)

    CloseVPIF();

    return S_OK;
}
#else
// 运行在“200w视频流模式”下
HRESULT CVideoGetter_VPIF::RunVideoMode(void*pvParam)
{
#ifdef _CAM_APP_
    m_dwLastThreadIsOkTime = GetSystemTick();

    if ( S_OK != OpenVPIF() )
    {
        return E_FAIL;
    }

    IMG_DATA cImgData;

    // 通过丢帧机制进行输出帧率控制
    int iCurFrameIndex = 1;
    DWORD32 dwNowTick = GetSystemTick();
    double fltActuralFrameIndex = 0;
    if (m_cImgFrameParam.iOutputFrameRate > m_cImgFrameParam.fltCaptureFrameRate)
    {
        m_cImgFrameParam.iOutputFrameRate = (int)m_cImgFrameParam.fltCaptureFrameRate;
    }
    float fltStep = m_cImgFrameParam.fltCaptureFrameRate / m_cImgFrameParam.iOutputFrameRate;

    while (!m_fExit)  // 视频帧处理主循环
    {
        m_dwLastThreadIsOkTime = GetSystemTick();

        int iRet = get_capture_buffer(m_VpifFd, &cImgData);
        if ( 0 != iRet )
        {
            HV_Trace(5, "get_capture_buffer is error!\n");
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "get_capture_buffer is error!");
        }
#ifdef _CAMERA_PIXEL_200W_25FPS_
        if ( true ) // 200w的25帧模式暂不控制输出帧率
#else
        if ( iCurFrameIndex++ == int(fltActuralFrameIndex + fltStep + 0.5))
#endif
        {
            fltActuralFrameIndex = fltActuralFrameIndex + fltStep;
            if ( cImgData.addr != NULL && IMAGE_DATA_SIZE == cImgData.iDataLen )
            {
                PBYTE8 pbSrcY = (PBYTE8)cImgData.addr;
                PBYTE8 pbSrcUV = ((PBYTE8)cImgData.addr) + IMAGE_WIDTH*IMAGE_HEIGHT;

                IMG_FRAME imgFrame;
                HV_IMAGE_TYPE nImgType;

                if (1 == m_iEddyType)
                {
                    nImgType = HV_IMAGE_BT1120_ROTATE_Y;
                }
                else
                {
#ifdef _CAMERA_PIXEL_200W_25FPS_
                    nImgType = HV_IMAGE_BT1120;
#else
                    nImgType = HV_IMAGE_BT1120_UV;
#endif
                }

                g_cCameraController.GetExtInfoByImage((unsigned char*)cImgData.addr, imgFrame.cFpgaRegInfo, imgFrame.cAgcAwbInfo);
                if ( S_OK == CreateReferenceComponentImage(
                            &(imgFrame.pRefImage),
                            nImgType,
                            m_iImageWidth,
                            m_iImageHeight,
                            m_dwFrameCount++,
                            GetSystemTick(),
                            0,
                            "NULL",
                            2 ) )
                {
                    imgFrame.pRefImage->SetCaptureFlag(imgFrame.cFpgaRegInfo.reg_soft_capture_trig == 1);
                    HV_COMPONENT_IMAGE imgVPIF;

                    if ( S_OK != imgFrame.pRefImage->GetImage(&imgVPIF) )
                    {
                        HV_Trace(5, "imgFrame.pRefImage GetImage is Failed!\n");
                        HV_Exit(HEC_FAIL, "imgFrame.pRefImage GetImage is Failed!");
                    }

                    if ( HV_IMAGE_BT1120 == nImgType )
                    {
                        memcpy((void*)GetHvImageData(&imgVPIF, 0), (void*)pbSrcY, IMAGE_WIDTH*IMAGE_HEIGHT);
                        memcpy((void*)GetHvImageData(&imgVPIF, 1), (void*)pbSrcUV, IMAGE_WIDTH*IMAGE_HEIGHT);
                        PutOneFrame(imgFrame);
                    }
                    else if ( HV_IMAGE_BT1120_UV == nImgType )
                    {
                        memcpy((void*)GetHvImageData(&imgVPIF, 0), (void*)pbSrcY, IMAGE_WIDTH*IMAGE_HEIGHT);
                        memcpy((void*)GetHvImageData(&imgVPIF, 1), (void*)pbSrcUV, IMAGE_WIDTH*IMAGE_HEIGHT);
                        if ( 0 == SplitUV(
                                    m_SwDevFd,
                                    (PBYTE8)(imgVPIF.rgImageData[1].phys),
                                    (PBYTE8)(imgVPIF.rgImageData[2].phys),
                                    (PBYTE8)(imgVPIF.rgImageData[2].phys) + ((IMAGE_HEIGHT*IMAGE_WIDTH) >> 1)
                                ) )
                        {
                            //是否分离UV的标志，确保压缩JPEG是图片正常，没有分离UV情况下不能压缩JPEG，黄国超备注,2011-12-27
                            if (1 == g_pCamApp->GetJpegStreamParam())
                            {
                                imgFrame.fSplitUV = TRUE;
                            }
                            PutOneFrame(imgFrame);
                        }
                        else
                        {
                            HV_Trace(5, "SplitUV is failed.\n");
                        }
                    }
                    else if ( HV_IMAGE_BT1120_ROTATE_Y == nImgType )
                    {
                        if ( 0 == BT1120ToBT1120RotateY(
                                    m_SwDevFd,
                                    (PBYTE8)cImgData.offset,
                                    (PBYTE8)(((PBYTE8)cImgData.offset) + (IMAGE_HEIGHT*IMAGE_WIDTH)),
                                    (PBYTE8)imgVPIF.rgImageData[0].phys,
                                    (PBYTE8)imgVPIF.rgImageData[1].phys
                                ) )
                        {
                            imgFrame.fRotateY = TRUE;
                            PutOneFrame(imgFrame);
                        }
                        else
                        {
                            HV_Trace(5, "BT1120ToBT1120RotateY is failed.\n");
                        }
                    }

                    SAFE_RELEASE(imgFrame.pRefImage);
                }
                else
                {
                    HV_Trace(5, "imgFrame.pRefImage Create is Failed!\n");
                }
            }
        }

        if (iCurFrameIndex > 100000)
        {
            HV_Trace(5, "CVideoGetter_VPIF cycle.");
            iCurFrameIndex = 0;
            fltActuralFrameIndex = 0.0f;
        }
        // 统计帧率
        static int iFps = 0;
        static int iLastTick = GetSystemTick();
        int iCurTick;
        if ( iFps >= 100 )
        {
            iCurTick = GetSystemTick();
            HV_Trace(5, "vpif fps: %.1f\n", float(100*1000) / (iCurTick - iLastTick));
            iLastTick = iCurTick;
            iFps = 0;
        }
        else
        {
            iFps++;
        }

        if ( put_capture_buffer(m_VpifFd, &cImgData) < 0 )
        {
            HV_Trace(5, "put_capture_buffer is error!\n");
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "put_capture_buffer is error!");
        }
    }  // end while (!m_fExit)

    CloseVPIF();

    return S_OK;
#else
return S_FALSE;
#endif // _CAM_APP_
}
#endif // _CAMERA_PIXEL_500W_

void CVideoGetter_VPIF::PutCaptureBuffer(void* addr)
{
#ifdef _CAMERA_PIXEL_500W_
    IMG_DATA cImgData;
    cImgData.addr = addr;

    int ret = put_capture_buffer(m_VpifFd, &cImgData);
    if (ret < 0)
    {
        HV_Trace(5, "put_capture_buffer is error!\n");
        HV_Exit(HEC_FAIL|HEC_RESET_DEV, "put_capture_buffer is error!");
    }
#endif
}

// 运行在“带视频流的触发抓拍模式”下
HRESULT CVideoGetter_VPIF::RunCaptureMode(void*pvParam)
{
    m_dwLastThreadIsOkTime = GetSystemTick();

    if ( S_OK != OpenVPIF() )
    {
        return E_FAIL;
    }

    IMG_DATA cImgData;
    HV_COMPONENT_IMAGE imgBayer12;
    IReferenceComponentImage *pRefImageBayer12=NULL;
    while (!m_fExit)  // 视频帧处理主循环
    {
        m_dwLastThreadIsOkTime = GetSystemTick();

        if ( 0 != get_capture_buffer(m_VpifFd, &cImgData) )
        {
            HV_Trace(5, "get_capture_buffer is error!\n");
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "get_capture_buffer is error!");
        }

        if ( cImgData.addr != NULL && IMAGE_DATA_SIZE == cImgData.iDataLen )
        {
            bool fIsCapture = false;
            IMG_FRAME imgFrame;
            g_cCameraController.GetExtInfoByImage((unsigned char*)cImgData.addr, imgFrame.cFpgaRegInfo, imgFrame.cAgcAwbInfo);

#ifdef _CAMERA_PIXEL_500W_
            fIsCapture = (1 == imgFrame.cFpgaRegInfo.cFpgaRegInfoA.reg_soft_capture_trig) ? (true) : (false);
#else
            fIsCapture = (1 == imgFrame.cFpgaRegInfo.reg_soft_capture_trig) ? (true) : (false);
#endif
            //动态更新相机参数
            UpdateCamParam();
            if (g_iControllPannelWorkStyle == 1)
            {
                UpdatePannelStatus();
            }
            HV_IMAGE_TYPE nImgType;
            int iImageHeight = 0;
#ifndef _CAMERA_PIXEL_500W_
            if ( 1 == m_iEddyType )
            {
                nImgType = HV_IMAGE_BT1120_ROTATE_Y;
                iImageHeight = m_iImageHeight;
            }
            else
            {
                // 如果是抓拍触发图，则用帧格式，否则用场格式。
                if ( 1 == fIsCapture )
                {
                    nImgType = HV_IMAGE_BT1120;
                    iImageHeight = m_iImageHeight;
                }
                else
                {
                    nImgType = HV_IMAGE_BT1120_FIELD;
                    iImageHeight = m_iImageHeight >> 1;
                }
            }
#else
            m_iEddyType = 0;
            nImgType = HV_IMAGE_BT1120;
            iImageHeight = m_iImageHeight;
#endif
            if ( S_OK != CreateReferenceComponentImage(
                        &pRefImageBayer12,
                        nImgType,
                        m_iImageWidth,
                        iImageHeight,
                        m_dwFrameCount++,
                        GetSystemTick(),
                        0,
                        "NULL",
                        2 ) )
            {
                HV_Trace(5, "pRefImageBayer12 GetImage is Failed!create a normal image\n");
                //在这里填入空白的图片给识别进程，让其释放出占用共享内存的图片
                if ( S_OK == CreateReferenceComponentImage(
                            &pRefImageBayer12,
                            nImgType,
                            m_iImageWidth,
                            m_iImageHeight,
                            0,
                            GetSystemTick(),
                            0,
                            "NULL",
                            0 ) )
                {
                    if ( S_OK == pRefImageBayer12->GetImage(&imgBayer12) )
                    {
                        IMG_FRAME imgFrame;
                        imgFrame.pRefImage = pRefImageBayer12;
                        PutOneFrame(imgFrame);
                    }

                    SAFE_RELEASE(pRefImageBayer12);
                }
            }
            else
            {
                if ( S_OK == pRefImageBayer12->GetImage(&imgBayer12) )
                {
                    HRESULT hr = E_FAIL;
                    if ( 1 == m_iEddyType )  // 逆时针旋转90度
                    {
                        if ( 0 == BT1120ToBT1120RotateY(
                                    m_SwDevFd,
                                    (PBYTE8)cImgData.offset,
                                    (PBYTE8)(((PBYTE8)cImgData.offset) + (IMAGE_HEIGHT*IMAGE_WIDTH)),
                                    (PBYTE8)imgBayer12.rgImageData[0].phys,
                                    (PBYTE8)imgBayer12.rgImageData[1].phys
                                ) )
                        {
                            hr = S_OK;
                        }
                        else
                        {
                            HV_Trace(5, "BT1120ToBT1120RotateY is failed.\n");
                        }
                    }
                    else
                    {
                        if ( HV_IMAGE_BT1120 == nImgType )
                        {
                            PBYTE8 pbSrcY = (PBYTE8)cImgData.addr;
                            PBYTE8 pbSrcUV = ((PBYTE8)cImgData.addr) + IMAGE_WIDTH*IMAGE_HEIGHT;
                            memcpy((void*)GetHvImageData(&imgBayer12, 0), (void*)pbSrcY, IMAGE_WIDTH*IMAGE_HEIGHT);
                            memcpy((void*)GetHvImageData(&imgBayer12, 1), (void*)pbSrcUV, IMAGE_WIDTH*IMAGE_HEIGHT);
                            hr = S_OK;
                        }
                        else if ( HV_IMAGE_BT1120_FIELD == nImgType )
                        {
                            PBYTE8 pbSrcY = (PBYTE8)(cImgData.addr);
                            PBYTE8 pbSrcUV = (PBYTE8)(cImgData.addr) + 1600*1200;

                            PBYTE8 pbDstY = (PBYTE8)imgBayer12.rgImageData[0].addr;
                            PBYTE8 pbDstUV = (PBYTE8)imgBayer12.rgImageData[1].addr;

                            for ( int i = 0; i < 600; ++i )
                            {
                                memcpy(pbDstY, pbSrcY, 1600);
                                memcpy(pbDstUV, pbSrcUV, 1600);

                                pbSrcY += 3200;
                                pbSrcUV += 3200;

                                pbDstY += 1600;
                                pbDstUV += 1600;
                            }

                            hr = S_OK;
                        }
                    }

                    if ( S_OK == hr )
                    {
                        imgFrame.pRefImage = pRefImageBayer12;

                        if ( 1 == fIsCapture )
                        {
                            HV_Trace(5, "<Recv trigger image> Image time: %d\n", pRefImageBayer12->GetRefTime());
                            imgFrame.pRefImage->SetCaptureFlag(TRUE);
                            if (GetCurrentMode() == PRM_TOLLGATE)
                            {
                                PutOneFrame(imgFrame);
                            }
                            OnRecvCaptureImage(imgFrame);
                        }
                        else
                        {
                            PutOneFrame(imgFrame);
                        }
                        SAFE_RELEASE(pRefImageBayer12);
                    }
                    else
                    {
                        SAFE_RELEASE(pRefImageBayer12);
                    }
                }
                else
                {
                    SAFE_RELEASE(pRefImageBayer12);
                    HV_Trace(5, "pRefImageBayer12 GetImage is Failed!create a normal image\n");
                    PciSendSlaveDebugInfo("pRefImageBayer12 GetImage is Failed!create a normal image");
                    //在这里填入空白的图片给识别进程，让其释放出占用共享内存的图片
                    if ( S_OK == CreateReferenceComponentImage(
                                &pRefImageBayer12,
                                nImgType,
                                m_iImageWidth,
                                m_iImageHeight,
                                0,
                                GetSystemTick(),
                                0,
                                "NULL",
                                0 ) )
                    {
                        if ( S_OK == pRefImageBayer12->GetImage(&imgBayer12) )
                        {
                            IMG_FRAME imgFrame;
                            imgFrame.pRefImage = pRefImageBayer12;
                            PutOneFrame(imgFrame);
                        }

                        SAFE_RELEASE(pRefImageBayer12);
                    }
                }
            }
        }

        int ret = put_capture_buffer(m_VpifFd, &cImgData);
        if (ret < 0)
        {
            HV_Trace(5, "put_capture_buffer is error!\n");
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "put_capture_buffer is error!");
        }
    }  // end while (!m_fExit)

    CloseVPIF();

    return S_OK;
}

// 运行在“单触发抓拍模式”下
HRESULT CVideoGetter_VPIF::RunHvcMode(void*pvParam)
{
    m_dwLastThreadIsOkTime = GetSystemTick();

    if ( S_OK != OpenVPIF() )
    {
        return E_FAIL;
    }

    IMG_DATA cImgData;
    HV_COMPONENT_IMAGE imgBayer12;
    IReferenceComponentImage *pRefImageBayer12=NULL;

    while (!m_fExit)  // 视频帧处理主循环
    {
        m_dwLastThreadIsOkTime = GetSystemTick();

        if ( 0 != get_capture_buffer(m_VpifFd, &cImgData) )
        {
            HV_Trace(5, "get_capture_buffer is error!\n");
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "get_capture_buffer is error!");
        }

        if ( cImgData.addr != NULL && IMAGE_DATA_SIZE == cImgData.iDataLen )
        {
            bool fIsCapture = false;
            IMG_FRAME imgFrame;

            g_cCameraController.GetExtInfoByImage((unsigned char*)cImgData.addr, imgFrame.cFpgaRegInfo, imgFrame.cAgcAwbInfo);

#ifdef _CAMERA_PIXEL_500W_
            fIsCapture = (1 == imgFrame.cFpgaRegInfo.cFpgaRegInfoA.reg_soft_capture_trig) ? (true) : (false);
#else
            fIsCapture = (1 == imgFrame.cFpgaRegInfo.reg_soft_capture_trig) ? (true) : (false);
#endif

            if ( 1 == fIsCapture )
            {
                HV_IMAGE_TYPE nImgType;
                int iImageHeight = 0;
#ifndef _CAMERA_PIXEL_500W_
                if ( 1 == m_iEddyType )
                {
                    nImgType = HV_IMAGE_BT1120_ROTATE_Y;
                    iImageHeight = m_iImageHeight;
                }
                else
                {
                    nImgType = HV_IMAGE_BT1120;
                    iImageHeight = m_iImageHeight;
                }
#else
                m_iEddyType = 0;
                nImgType = HV_IMAGE_BT1120;
                iImageHeight = m_iImageHeight;
#endif
                if ( S_OK != CreateReferenceComponentImage(
                            &pRefImageBayer12,
                            nImgType,
                            m_iImageWidth,
                            iImageHeight,
                            m_dwFrameCount++,
                            GetSystemTick(),
                            0,
                            "NULL",
                            2 ) )
                {
                    HV_Trace(5, "pRefImageBayer12 Create is Failed!\n");
                    HV_Exit(HEC_FAIL, "pRefImageBayer12 Create is Failed!");
                }

                if ( S_OK == pRefImageBayer12->GetImage(&imgBayer12) )
                {
                    HRESULT hr = E_FAIL;

                    if ( 1 == m_iEddyType )  // 逆时针旋转90度
                    {
                        if ( 0 == BT1120ToBT1120RotateY(
                                    m_SwDevFd,
                                    (PBYTE8)cImgData.offset,
                                    (PBYTE8)(((PBYTE8)cImgData.offset) + (IMAGE_HEIGHT*IMAGE_WIDTH)),
                                    (PBYTE8)imgBayer12.rgImageData[0].phys,
                                    (PBYTE8)imgBayer12.rgImageData[1].phys
                                ) )
                        {
                            hr = S_OK;
                        }
                        else
                        {
                            HV_Trace(5, "BT1120ToBT1120RotateY is failed.\n");
                        }
                    }
                    else
                    {
                        if ( HV_IMAGE_BT1120 == nImgType )
                        {
                            PBYTE8 pbSrcY = (PBYTE8)cImgData.addr;
                            PBYTE8 pbSrcUV = ((PBYTE8)cImgData.addr) + IMAGE_WIDTH*IMAGE_HEIGHT;
                            memcpy((void*)GetHvImageData(&imgBayer12, 0), (void*)pbSrcY, IMAGE_WIDTH*IMAGE_HEIGHT);
                            memcpy((void*)GetHvImageData(&imgBayer12, 1), (void*)pbSrcUV, IMAGE_WIDTH*IMAGE_HEIGHT);
                            hr = S_OK;
                        }
                        else if ( HV_IMAGE_BT1120_FIELD == nImgType )
                        {
                            PBYTE8 pbSrcY = (PBYTE8)(cImgData.addr);
                            PBYTE8 pbSrcUV = (PBYTE8)(cImgData.addr) + 1600*1200;

                            PBYTE8 pbDstY = (PBYTE8)imgBayer12.rgImageData[0].addr;
                            PBYTE8 pbDstUV = (PBYTE8)imgBayer12.rgImageData[1].addr;

                            for ( int i = 0; i < 600; ++i )
                            {
                                memcpy(pbDstY, pbSrcY, 1600);
                                memcpy(pbDstUV, pbSrcUV, 1600);

                                pbSrcY += 3200;
                                pbSrcUV += 3200;

                                pbDstY += 1600;
                                pbDstUV += 1600;
                            }

                            hr = S_OK;
                        }
                    }

                    if ( S_OK == hr )
                    {
                        imgFrame.pRefImage = pRefImageBayer12;
                        imgFrame.pRefImage->SetCaptureFlag(TRUE);
                        PutOneFrame(imgFrame);
                    }

                    SAFE_RELEASE(pRefImageBayer12);
                }
                else
                {
                    HV_Trace(5, "pRefImageBayer12 GetImage is Failed!\n");
                    HV_Exit(HEC_FAIL, "pRefImageBayer12 GetImage is Failed!");
                }
            }
        }

        int ret = put_capture_buffer(m_VpifFd, &cImgData);
        if (ret < 0)
        {
            HV_Trace(5, "put_capture_buffer is error!\n");
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "put_capture_buffer is error!");
        }
    }  // end while (!m_fExit)

    CloseVPIF();

    return S_OK;
}

HRESULT CVideoGetter_VPIF::Run(void*pvParam)
{
    HRESULT hr = E_FAIL;

    m_SwDevFd = SwDevOpen(O_RDWR);

    if ( SRM_VIDEO == m_eRunMode )
    {
        hr = RunVideoMode(pvParam);
    }
    else if ( SRM_CAPTURE == m_eRunMode )
    {
        hr = RunCaptureMode(pvParam);
    }
    else if ( SRM_HVC == m_eRunMode )
    {
        hr = RunHvcMode(pvParam);
    }
    else
    {
        hr = E_FAIL;
    }

    if ( m_SwDevFd >= 0 )
    {
        SwDevClose(m_SwDevFd);
        m_SwDevFd = -1;
    }

    return hr;
}

HRESULT CVideoGetter_VPIF::SetLightType(LIGHT_TYPE cLightType, int iCplStatus)
{
    if (m_pCamCfgParam == NULL)
    {
        return E_POINTER;
    }
    if (!m_pCamCfgParam->iDynamicCfgEnable)
    {
        return S_OK;
    }
    if (cLightType < BIG_DAY_FRONTLIGHT || cLightType >= LIGHT_TYPE_COUNT)
    {
        return E_FAIL;
    }

    if (g_iControllPannelWorkStyle == 1)
    {
        if (m_nCurLightType == cLightType)
        {
            return S_OK;
        }
        m_fNeedToUpdateParam = TRUE;
        m_nCurLightType = cLightType;
        HV_Trace(5, "场景状态改变：%d\n", m_nCurLightType);
    }
    else
    {
        if (m_nCurLightType == cLightType && m_iCurCplStatus == iCplStatus)
        {
            return S_OK;
        }
        m_fNeedToUpdateParam = TRUE;
        m_nCurLightType = cLightType;
        m_iCurCplStatus = iCplStatus;
        HV_Trace(5, "场景状态改变：%d_%d\n", m_nCurLightType, m_iCurCplStatus);
    }

    return S_OK;
}

HRESULT CVideoGetter_VPIF::SetPannelStatus(int iCplStatus, int iPulseLevel)
{
    if (m_pCamCfgParam == NULL)
    {
        return E_POINTER;
    }
    if (!m_pCamCfgParam->iDynamicCfgEnable)
    {
        return S_OK;
    }
    if (m_iCurCplStatus == iCplStatus && m_iPulseLevel == iPulseLevel)
    {
        return S_OK;
    }
    m_fIsNeedToUpdatePannelStatus = true;
    m_fIsChangeCamParam = true;
    m_iCurCplStatus = iCplStatus;
    m_iPulseLevel = iPulseLevel;
    HV_Trace(5, "控制板状态改变,%d_%d", m_iCurCplStatus, m_iPulseLevel);
    return S_OK;
}

HRESULT CVideoGetter_VPIF::SetCamCfgParam(LPVOID pCfgCamParam)
{
    m_pCamCfgParam = (CAM_CFG_PARAM *)pCfgCamParam;
    if (m_pCamCfgParam == NULL)
    {
        return E_POINTER;
    }

    int iAGCDB = (m_pCamCfgParam->iMaxAGCLimit - m_pCamCfgParam->iMinAGCLimit) / 13;
    if (iAGCDB > 0)
    {
        for (int i = 0; i < 14; i++)
        {
            m_irgAGCLimit[i] = m_pCamCfgParam->iMinAGCLimit + (iAGCDB * i);
        }
    }
    else
    {
        for (int i = 0; i < 14; i++)
        {
            m_irgAGCLimit[i] = m_pCamCfgParam->irgAGCLimit[i];
        }
    }
    return S_OK;
}

HRESULT SetCamParamFromPci(DCP_TYPE eType, int nValue)
{
    HRESULT hr = E_FAIL;

    if (nValue == -1)
    {
        return S_OK;
    }

    int nSize = sizeof(HRESULT);
    PCI_DYNPARAM_INFO cDynParamInfo;
    cDynParamInfo.eType = eType;
    cDynParamInfo.nValue = nValue;
    g_cHvPciLinkApi.SendData(
        PCILINK_SET_CAM_DYN_PARAM,
        &cDynParamInfo,
        sizeof(PCI_DYNPARAM_INFO),
        &hr,
        &nSize
    );

    return hr;
}

HRESULT SetLightTypeFromPci(DCP_TYPE eType, int nValue, unsigned char* rgbResultData)
{
    if (eType == DCP_SETPULSEWIDTH)
    {
        int iSize = 8;
        PCI_DYNPARAM_INFO cDynParamInfo;
        cDynParamInfo.eType = eType;
        cDynParamInfo.nValue = nValue;
        g_cHvPciLinkApi.SendData(
            PCILINK_SET_CAM_DYN_PARAM,
            &cDynParamInfo,
            sizeof(PCI_DYNPARAM_INFO),
            rgbResultData,
            &iSize,
            20000
        );
        return S_OK;
    }
    else
    {
        HRESULT hr = E_FAIL;
        int nSize = sizeof(HRESULT);
        PCI_DYNPARAM_INFO cDynParamInfo;
        cDynParamInfo.eType = eType;
        cDynParamInfo.nValue = nValue;
        g_cHvPciLinkApi.SendData(
            PCILINK_SET_CAM_DYN_PARAM,
            &cDynParamInfo,
            sizeof(PCI_DYNPARAM_INFO),
            &hr,
            &nSize
        );

        return hr;
    }
}

HRESULT CVideoGetter_VPIF::UpdateCamParam()
{
    if (m_pCamCfgParam == NULL)
    {
        return E_POINTER;
    }

    if (!m_pCamCfgParam->iDynamicCfgEnable)
    {
        return S_OK;
    }

    if (!m_fNeedToUpdateParam)
    {
        return S_OK;
    }

    static DWORD32 dwLastTime = GetSystemTick();

    if (GetSystemTick() - dwLastTime < 5 * 1000)
    {
        return S_OK;
    }

    HRESULT hr = S_OK;

    m_fPCISending = true;
    hr |= SetCamParamFromPci(DCP_ENABLE_AGC, m_pCamCfgParam->iEnableAGC);
    if (m_pCamCfgParam->iEnableAGC == 0)
    {
        hr |= SetCamParamFromPci(DCP_AGC_TH, m_pCamCfgParam->irgAGCLimit[m_nCurLightType]);
        hr |= SetCamParamFromPci(DCP_GAIN, m_pCamCfgParam->irgGain[m_nCurLightType]);
        hr |= SetCamParamFromPci(DCP_SHUTTER, m_pCamCfgParam->irgExposureTime[m_nCurLightType]);
        /*
        if (g_iControllPannelWorkStyle != 1)
        {
            hr |= SetCamParamFromPci(DCP_CPL, m_iCurCplStatus);
        }
        */
    }
    else
    {
        hr |= SetCamParamFromPci(DCP_AGC_TH, m_irgAGCLimit[m_nCurLightType]);
        /*
        if (g_iControllPannelWorkStyle != 1)
        {
            hr |= SetCamParamFromPci(DCP_CPL, m_iCurCplStatus);
        }
        */
    }
    m_fPCISending = false;
    if (hr == S_OK)
    {
        if (g_iControllPannelWorkStyle == 1)
        {
            HV_Trace(5, "相机参数改变：%d\n", m_nCurLightType);
        }
        else
        {
            HV_Trace(5, "相机参数改变：%d_%d\n", m_nCurLightType, m_iCurCplStatus);
        }
        m_fNeedToUpdateParam = FALSE;
        return S_OK;
    }
    else
    {
        if (g_iControllPannelWorkStyle == 1)
        {
            HV_Trace(5, "相机参数改变失败：%d\n", m_nCurLightType);
        }
        else
        {
            HV_Trace(5, "相机参数改变失败：%d_%d\n", m_nCurLightType, m_iCurCplStatus);
        }
    }

    return E_FAIL;
}

HRESULT CVideoGetter_VPIF::UpdatePannelStatus()
{
    if (m_pCamCfgParam == NULL)
    {
        return E_POINTER;
    }

    if (!m_pCamCfgParam->iDynamicCfgEnable)
    {
        return S_OK;
    }
    if (!m_fIsNeedToUpdatePannelStatus)
    {
        return S_OK;
    }
    static DWORD32 dwLastTime = GetSystemTick();
    if (GetSystemTick() - dwLastTime < 5 * 1000)
    {
        return S_OK;
    }
    m_fPCISending = true;
    HRESULT hr = S_OK;
    hr |= SetLightTypeFromPci(DCP_CPL, m_iCurCplStatus, NULL);
    unsigned char rgbResult[8] = {0};
    hr |= SetLightTypeFromPci(DCP_SETPULSEWIDTH, m_iPulseLevel, rgbResult);
    /*if (hr == S_OK)
    {
        m_fIsNeedToUpdatePannelStatus = FALSE;
        dwLastTime = GetSystemTick();
        return S_OK;
    }*/
    m_fPCISending = false;
    int iPulseLevel, iCplStatus;
    memcpy(&iPulseLevel, rgbResult, 4);
    memcpy(&iCplStatus, rgbResult+4, 4);
    /*
    if (iPulseLevel != m_iPulseLevel || iCplStatus != m_iCurCplStatus)
    {
        HV_Trace(5, "修改控制板参数失败, %d_%d", m_iPulseLevel, m_iCurCplStatus);
        m_fIsChangeCamParam = true;
        m_fIsNeedToUpdatePannelStatus = true;
        return S_OK;
    }
    */
    m_fIsChangeCamParam = false;
    m_fIsNeedToUpdatePannelStatus = false;
    HV_Trace(5, "修改控制板参数成功, %d_%d", m_iPulseLevel, m_iCurCplStatus);
    return S_OK;
}

void CVideoGetter_VPIF::OnRecvCaptureImage(const IMG_FRAME& imgFrame)
{
#ifndef _CAM_APP_
    if (m_pSignalMatch == NULL)
    {
        return;
    }

    SIGNAL_INFO tempSignalInfo;
    memset(&tempSignalInfo, 0, sizeof(SIGNAL_INFO));

    tempSignalInfo.dwSignalTime = imgFrame.pRefImage->GetRefTime();
    tempSignalInfo.dwInputTime = GetSystemTick();
    tempSignalInfo.dwValue = 0;
    tempSignalInfo.nType = ((COuterControlImpl*)m_pSignalMatch)->GetImageSourceIndex();
    tempSignalInfo.dwRoad = 0; //必须初始化否则为随机值
    tempSignalInfo.dwFlag = 0;
    tempSignalInfo.iModifyRoad = 0;
    tempSignalInfo.pImage = NULL;
    tempSignalInfo.pImageLast = imgFrame.pRefImage;

    if (tempSignalInfo.pImageLast)
    {
        tempSignalInfo.pImageLast->AddRef();
    }
    // 将信号加入信号队列
    if ( S_OK != m_pSignalMatch->AppendSignal(&tempSignalInfo)
            && tempSignalInfo.pImageLast != NULL )
    {
        tempSignalInfo.pImageLast->Release();
        tempSignalInfo.pImageLast = NULL;
    }
#endif
}

HRESULT CreateReferenceFrame_VPIF(
    CReferenceFrame_VPIF** ppRefFrame_VPIF,
    CVideoGetter_VPIF* pVPIF,
    HV_COMPONENT_IMAGE imgVPIF,
    DWORD32 dwTimeTick,
    BOOL fIsCapture
)
{
    if (ppRefFrame_VPIF == NULL)
    {
        return E_INVALIDARG;
    }

    *ppRefFrame_VPIF = new CReferenceFrame_VPIF(pVPIF, imgVPIF, dwTimeTick, fIsCapture);
    return (*ppRefFrame_VPIF == NULL)?E_OUTOFMEMORY:S_OK;
}
