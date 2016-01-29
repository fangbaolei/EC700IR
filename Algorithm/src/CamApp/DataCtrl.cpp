#include "DataCtrl.h"
#include "math.h"
#include "HvDspLinkApi.h"
#include "hvsysinterface.h"
#include "CameraController.h"
#include "HvSerialLink.h"
#include "CharacterOverlap.h"

/* main.cpp */
extern HV_SEM_HANDLE g_hSemEDMA;
extern int g_nFpgaVersion;
extern bool g_fCopyrightValid;
extern bool g_fResetIPT;
using namespace HvSys;
using namespace HiVideo;

int g_iControllPannelWorkStyle = 0;

HRESULT ForceSend(DWORD32 dwVideoID)
{
    return S_OK;
}

HRESULT GetWorkModeInfo(SYS_INFO* pInfo)
{
    return E_FAIL;
}

HRESULT GetWorkModeList(SYS_INFO* pInfo)
{
    return E_FAIL;
}

HRESULT SetWorkMode(DWORD32 dwWorkMode)
{
    return E_FAIL;
}

// 控制偏光镜
// nMode意义 -1：不使能偏光镜 0：不使用偏光镜；1：使用偏光镜
HRESULT CtrlPL(int nMode)
{
    if ( -1 == nMode )
    {
        return S_OK;
    }

    HRESULT hr = E_FAIL;

    static CHvSerialLink* s_pSerialLink = NULL;
    if (s_pSerialLink == NULL)
    {
        s_pSerialLink = new CHvSerialLink;
        if ((s_pSerialLink != NULL)
                && (s_pSerialLink->Open("/dev/ttyS0") != S_OK))
        {
            delete s_pSerialLink;
            s_pSerialLink = NULL;
        }
    }

    if (0 == s_pSerialLink->Lock(1000))
    {
        unsigned char bData = 0;
        // 切换控制板通道
        if ( S_OK == s_pSerialLink->SendCmdData(0xA1, &bData, 1) )
        {
            // 延时1ms，否则可能切不成功
            HV_Sleep(1);

            // 控制偏光镜
            bData = nMode & 0xFF;
            if ( S_OK == s_pSerialLink->SendCmdData(0x18, &bData, 1) )
            {
                // TODO : 先不接收，改PCI机制后再处理
                /*unsigned char rgbDataRecv[32];
                unsigned int uiRecvLen = 32;
                if ( S_OK == s_pSerialLink->RecvPacket(rgbDataRecv, &uiRecvLen, 1000) )
                {
                    if ( rgbDataRecv[0] == 0x00 )
                    {
                        hr = S_OK;
                    }
                }*/
                hr = S_OK;
            }
        }

        s_pSerialLink->UnLock();
    }

    HV_Trace(1, "CtrlPL:%d, Result:0x%x", nMode, hr);

    return hr;
}

//----------------------H.264相关---------------------------------

H264_FRAME_HEADER g_cH264FrameHeader;  // H.264流的帧头

int GetH264Header(H264_FRAME_HEADER& cH264FrameHeader, const PBYTE8 pbH264BitStream)
{
    memcpy(cH264FrameHeader.bH264Header, pbH264BitStream, H264_HEADER_LEN);
    cH264FrameHeader.iLenH264Header = H264_HEADER_LEN;
    return 0;
}

// H.264编码在各种情况下的规格
const int H264_WIDTH_200M_NORMAL = 1600;
const int H264_HEIGHT_200M_NORMAL = 1088;
const int H264_WIDTH_200M_ROTATE = 1184;
const int H264_HEIGHT_200M_ROTATE = 800;
const int H264_WIDTH_500M_NORMAL = 1216;
const int H264_HEIGHT_500M_NORMAL = 1024;

/* DSPLink.c */
extern "C" int CheckHDVICP01();

// 打开H.264编码器
HRESULT OpenH264Enc(
    CHvDspLinkApi& g_cHvDspLinkApi,
    int dwTargetBitRate,
    BOOL fIsSideInstall,
    int iENetSyn
)
{
    static bool fIsFirstOpen = true;
    if (fIsFirstOpen)
    {
        fIsFirstOpen = false;
        if ( 0 != CheckHDVICP01() )
        {
            HV_Trace(5, "CheckHDVICP01 is failed!\n");
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "CheckHDVICP01 is failed!");
        }
    }

    H264_ENCODE_PARAM cH264EncodeParam;
    H264_ENCODE_DATA cH264EncodeData;
    H264_ENCODE_RESPOND cH264EncodeRespond;

    memset((void*)&g_cH264FrameHeader, 0, sizeof(g_cH264FrameHeader));

    cH264EncodeParam.dwOpType = OPTYPE_OPEN;
    cH264EncodeParam.dwTargetBitRate = dwTargetBitRate;  // 比特率

#ifdef _CAMERA_PIXEL_500W_
    cH264EncodeParam.dwInputWidth = H264_WIDTH_500M_NORMAL;
    cH264EncodeParam.dwInputHeight = H264_HEIGHT_500M_NORMAL;
#else
    if ( 0 == fIsSideInstall )
    {
        cH264EncodeParam.dwInputWidth = H264_WIDTH_200M_NORMAL;
        cH264EncodeParam.dwInputHeight = H264_HEIGHT_200M_NORMAL;
    }
    else
    {
        cH264EncodeParam.dwInputWidth = H264_WIDTH_200M_ROTATE;
        cH264EncodeParam.dwInputHeight = H264_HEIGHT_200M_ROTATE;
    }
#endif

    if ( 1 == iENetSyn )
    {
        cH264EncodeParam.dwIntraFrameInterval = 12;
        cH264EncodeParam.dwFrameRate = 12;
    }
    else
    {
        cH264EncodeParam.dwIntraFrameInterval = 15;
        cH264EncodeParam.dwFrameRate = 15;
    }

    cH264EncodeParam.dwOutputBufSize = 1024*1024;
    memset(&cH264EncodeData, 0, sizeof(cH264EncodeData));

    //SemPend(&g_hSemEDMA);
    HRESULT hr = g_cHvDspLinkApi.SendH264EncodeCmd(&cH264EncodeParam, &cH264EncodeData, &cH264EncodeRespond);
    //SemPost(&g_hSemEDMA);
    if ( S_OK != hr )
    {
        HV_Trace(5, "Open H264Encoder is Failed!\n");
        return E_FAIL;
    }
    else
    {
        HV_Trace(3, "Open H264Encoder is Succeed!\n");
        return S_OK;
    }
}

// 关闭H.264编码器
HRESULT CloseH264Enc(CHvDspLinkApi& g_cHvDspLinkApi)
{
    H264_ENCODE_PARAM cH264EncodeParam;
    H264_ENCODE_DATA cH264EncodeData;
    H264_ENCODE_RESPOND cH264EncodeRespond;

    cH264EncodeParam.dwOpType = OPTYPE_CLOSE;
    memset(&cH264EncodeData, 0, sizeof(cH264EncodeData));

    //SemPend(&g_hSemEDMA);
    HRESULT hr = g_cHvDspLinkApi.SendH264EncodeCmd(&cH264EncodeParam, &cH264EncodeData, &cH264EncodeRespond);
    //SemPost(&g_hSemEDMA);
    if ( S_OK != hr )
    {
        HV_Trace(5, "Close H264Encoder is Failed!\n");
        return E_FAIL;
    }
    else
    {
        HV_Trace(3, "Close H264Encoder is Succeed!\n");
        return S_OK;
    }
}

//-------------------------------------------------------

CCamApp::CCamApp()
        : m_pVideoGetter(NULL)
        , m_pVideoSender(NULL)
        , m_pRecordSender(NULL)
        , m_pFrontController(NULL)
        , m_pSafeSaver(NULL)
{
    m_fSendDebugImage = false;
    m_pImageSender = NULL;
    CreateSemaphore(&m_hSemEncode, 1, 1);
}

CCamApp::~CCamApp()
{
    if (m_pFrontController != NULL)
    {
        m_pFrontController->SetPannelStatus(1, 0);
    }
    DestroySemaphore(&m_hSemEncode);
}

bool CCamApp::ThreadIsOk(int* piErrCode)
{
    bool fRet = true;
    int iErrCode = 0;

    if ( m_pVideoGetter != NULL && S_OK != m_pVideoGetter->GetCurStatus(NULL, 0) )
    {
        iErrCode = 2;
        fRet = false;
    }
    if ( m_pImageSender != NULL && S_OK != m_pImageSender->GetCurStatus(NULL, 0) )
    {
        iErrCode = 3;
        fRet = false;
    }
    if ( m_pVideoSender != NULL && S_OK != m_pVideoSender->GetCurStatus(NULL, 0) )
    {
        iErrCode = 4;
        fRet = false;
    }
    if ( m_pRecordSender != NULL && S_OK != m_pRecordSender->GetCurStatus(NULL, 0) )
    {
        iErrCode = 5;
        fRet = false;
    }

    if ( piErrCode != NULL )
    {
        *piErrCode = iErrCode;
    }

    return fRet;
}

void CCamApp::ResetH264Enc(int iENetSyn)
{
    if ( iENetSyn != -1 )
    {
        m_iENetSyn = iENetSyn;
    }

    // 如果是一体机协议并且H.264编码器已经打开，这时才能复位H.264编码器。
    if ( 0 == m_iCamType && true == m_fH264EncodeIsOpen )
    {
        HRESULT hr = S_OK;

        SemPend(&g_hSemEDMA);

        if ( S_OK != CloseH264Enc(g_cHvDspLinkApi) )
        {
            hr = E_FAIL;
        }

        if ( S_OK != OpenH264Enc(
                    g_cHvDspLinkApi,
                    g_cModuleParams.cCamAppParam.iTargetBitRate,
                    g_cModuleParams.cCamAppParam.fIsSideInstall,
                    m_iENetSyn) )
        {
            hr = E_FAIL;
        }

        SemPost(&g_hSemEDMA);

        if ( S_OK == hr )
        {
            HV_Trace(5, "ResetH264Enc is OK.\n");
        }
        else
        {
            HV_Trace(5, "ResetH264Enc is ERROR!\n");
        }
    }
}

static void GetAGCAWBInfo(
    const IMG_FRAME& frame,
    int *prgiAGCZone,
    int  iCount,
    int& iYArg,
    int& iRSum,
    int& iGSum,
    int& iBSum
)
{
#ifdef _CAMERA_PIXEL_500W_
    iYArg = 0;
    iRSum = frame.cAgcAwbInfo.cAgcAwbInfoA.SumR_ab + frame.cAgcAwbInfo.cAgcAwbInfoB.SumR_ab;
    iGSum = frame.cAgcAwbInfo.cAgcAwbInfoA.SumG_ab + frame.cAgcAwbInfo.cAgcAwbInfoB.SumG_ab;
    iBSum = frame.cAgcAwbInfo.cAgcAwbInfoA.SumB_ab + frame.cAgcAwbInfo.cAgcAwbInfoB.SumB_ab;

    // for 500w AGC
    int iAGCZoneCount = 0;
    for ( int i=0; i<iCount; ++i )
    {
        if ( 1 == prgiAGCZone[i] )
        {
            iAGCZoneCount++;
            iYArg += frame.cAgcAwbInfo.cAgcAwbInfoA.AvgY_ab[i];

            iAGCZoneCount++;
            iYArg += frame.cAgcAwbInfo.cAgcAwbInfoB.AvgY_ab[i];
        }
    }
    iYArg /= iAGCZoneCount;
#else
    iYArg = 0;
    iRSum = frame.cAgcAwbInfo.SumR;
    iGSum = frame.cAgcAwbInfo.SumG;
    iBSum = frame.cAgcAwbInfo.SumB;

    // for 200w AGC
    int iAGCZoneCount = 0;
    for ( int i=0; i<iCount; ++i )
    {
        if ( 1 == prgiAGCZone[i] )
        {
            iAGCZoneCount++;
            iYArg += frame.cAgcAwbInfo.AvgY[i];
        }
    }
    iYArg /= iAGCZoneCount;
#endif // _CAMERA_PIXEL_500W_

    // for AWB
#ifdef _CAMERA_PIXEL_500W_
    static const DWORD32 VALID_POINTCOUNT = (DWORD32)(2448 * 2048 * 0.003);
    if ( (frame.cAgcAwbInfo.cAgcAwbInfoA.PointCount_ab
            + frame.cAgcAwbInfo.cAgcAwbInfoB.PointCount_ab) < VALID_POINTCOUNT )
#else
    static const DWORD32 VALID_POINTCOUNT = (DWORD32)(1600 * 1200 * 0.003);
    if ( frame.cAgcAwbInfo.PointCount < VALID_POINTCOUNT )
#endif
    {
        // 如果统计出来的RGB参考点太少，则该组AWB数据置为0（即：置为无效）
        iRSum = 0;
        iGSum = 0;
        iBSum = 0;
    }
    else
    {
        // Comment by Shaorg: 色彩增益补偿机制（夜间启用）。
        const int Y_DAY_NIGHT_BOUNDARY = 100; // Y值的白天夜间分界线，即：大于为白天（暂定等于也为白天），小于为夜间。
        const int RGB_CORRECT_PERIOD = 150;
        const int RGB_CORRECT_ANTI_WAVE = 10;
        static int iYCount = 0;
        static int iYIsNightCount = 0;
        if ( iYCount > RGB_CORRECT_PERIOD )
        {
            if ( iYIsNightCount > (RGB_CORRECT_PERIOD - RGB_CORRECT_ANTI_WAVE) )
            {
#ifdef _CAMERA_PIXEL_500W_
                iRSum = int( (float)iRSum * (0.98) );
                iGSum = int( (float)iGSum * (1.00) );
                iBSum = int( (float)iBSum * (1.25) );
#else
                iRSum = int( (float)iRSum * (1.00) );
                iGSum = int( (float)iGSum * (0.98) );
                iBSum = int( (float)iBSum * (1.25) );
#endif
            }

            iYCount = 0;
            iYIsNightCount = 0;
        }
        else
        {
            ++iYCount;

            if ( iYArg < Y_DAY_NIGHT_BOUNDARY )
            {
                ++iYIsNightCount;
            }
        }
    }
}

// 将4字节的无符号数x转换成bit位的有符号数。
// 以11位有效位数-7为例，传入FPGA为fffffff9，FPGA传出数据为800003f9，需将其转换成-7
// FPGA规则：将最高符号位放到第31位，取除符号位外的有效位，其余以0填充。
int ChangeValueFromFpga(DWORD32 x, int bit)
{
    int tmp = x;
    if (x&0x80000000)   // 负数
    {
        tmp = (~x & (1<<bit-1) - 1) + 1;    // 取反加1
        tmp = -tmp;
    }
    return tmp;
}

// 打印FPGA信息作为独立函数
#ifdef _CAMERA_PIXEL_500W_
void PrintFPGAInfo_500w(int nBank, IMG_FRAME& frame, int iYArg, int iRSum, int iGSum, int iBSum)
{
    int n = 0;
    char szBufTmp[1024] = {0};
    strcpy(szBufTmp, "\r\n");
    for ( n=0; n<16; ++n )
    {
        sprintf(szBufTmp+strlen(szBufTmp),
                "AvgY[%02d] = %03u:%03u ",
                n,
                frame.cAgcAwbInfo.cAgcAwbInfoA.AvgY_ab[n],
                frame.cAgcAwbInfo.cAgcAwbInfoB.AvgY_ab[n]);

        if ( 0 == (n+1)%2 )
        {
            strcat(szBufTmp, "\r\n");
        }
    }
    HV_Trace(nBank, szBufTmp);

    int iRGBSumCount = frame.cAgcAwbInfo.cAgcAwbInfoA.PointCount_ab + frame.cAgcAwbInfo.cAgcAwbInfoB.PointCount_ab;
    HV_Trace(nBank, "\niYArg = %d\niRSum = %d\niGSum = %d\niBSum = %d\niRGBSumCount = %d, [%0.3f%%, %d,%d]\n",
             iYArg, iRSum, iGSum, iBSum, iRGBSumCount,
             (float)iRGBSumCount / (IMAGE_WIDTH*IMAGE_HEIGHT),
             IMAGE_WIDTH, IMAGE_HEIGHT);
    HV_Trace(nBank, "\nFPGA regs_A:\n0x%08x, 0x%08x, 0x%08x, 0x%08x\n0x%08x, 0x%08x, 0x%08x, 0x%08x\n0x%08x, 0x%08x, 0x%08x",
             frame.cFpgaRegInfo.cFpgaRegInfoA.reg_video_SHT1,
             frame.cFpgaRegInfo.cFpgaRegInfoA.reg_capture_SHT2,
             frame.cFpgaRegInfo.cFpgaRegInfoA.reg_LED_FLASH_OUT_EN,
             frame.cFpgaRegInfo.cFpgaRegInfoA.reg_capture_mode,
             frame.cFpgaRegInfo.cFpgaRegInfoA.reg_ACSP,
             frame.cFpgaRegInfo.cFpgaRegInfoA.reg_soft_capture_trig,
             frame.cFpgaRegInfo.cFpgaRegInfoA.anti_flicker,
             frame.cFpgaRegInfo.cFpgaRegInfoA.Th_Custom_ab,
             frame.cFpgaRegInfo.cFpgaRegInfoA.Th_Count_ab,
             frame.cFpgaRegInfo.cFpgaRegInfoA.reg_capture_edge_select,
             frame.cFpgaRegInfo.cFpgaRegInfoA.time_cnt_out);
    HV_Trace(nBank, "\nFPGA regs_B:\n0x%08x, 0x%08x, 0x%08x, 0x%08x\n0x%08x, 0x%08x, 0x%08x, 0x%08x\n0x%08x, 0x%08x, 0x%08x",
             frame.cFpgaRegInfo.cFpgaRegInfoB.reg_video_SHT1,
             frame.cFpgaRegInfo.cFpgaRegInfoB.reg_capture_SHT2,
             frame.cFpgaRegInfo.cFpgaRegInfoB.reg_LED_FLASH_OUT_EN,
             frame.cFpgaRegInfo.cFpgaRegInfoB.reg_capture_mode,
             frame.cFpgaRegInfo.cFpgaRegInfoB.reg_ACSP,
             frame.cFpgaRegInfo.cFpgaRegInfoB.reg_soft_capture_trig,
             frame.cFpgaRegInfo.cFpgaRegInfoB.anti_flicker,
             frame.cFpgaRegInfo.cFpgaRegInfoB.Th_Custom_ab,
             frame.cFpgaRegInfo.cFpgaRegInfoB.Th_Count_ab,
             frame.cFpgaRegInfo.cFpgaRegInfoB.reg_capture_edge_select,
             frame.cFpgaRegInfo.cFpgaRegInfoB.time_cnt_out);
    HV_Trace(nBank, "[fpga version = 0x%x:0x%x]\n",
             frame.cFpgaRegInfo.cFpgaRegInfoA.reg_fpga_version,
             frame.cFpgaRegInfo.cFpgaRegInfoB.reg_fpga_version);
    HV_Trace(nBank, "\nb_Gr = 0x%08x:0x%08x, k_Gr = 0x%08x:0x%08x\nb_Gb = 0x%08x:0x%08x, k_Gb = 0x%08x:0x%08x\nb_R  = 0x%08x:0x%08x, k_R  = 0x%08x:0x%08x\nb_B  = 0x%08x:0x%08x, k_B  = 0x%08x:0x%08x\n",
             frame.cFpgaRegInfo.cFpgaRegInfoA.data_b_Gr,
             frame.cFpgaRegInfo.cFpgaRegInfoB.data_b_Gr,
             frame.cFpgaRegInfo.cFpgaRegInfoA.data_k_Gr,
             frame.cFpgaRegInfo.cFpgaRegInfoB.data_k_Gr,
             frame.cFpgaRegInfo.cFpgaRegInfoA.data_b_Gb,
             frame.cFpgaRegInfo.cFpgaRegInfoB.data_b_Gb,
             frame.cFpgaRegInfo.cFpgaRegInfoA.data_k_Gb,
             frame.cFpgaRegInfo.cFpgaRegInfoB.data_k_Gb,
             frame.cFpgaRegInfo.cFpgaRegInfoA.data_b_R,
             frame.cFpgaRegInfo.cFpgaRegInfoB.data_b_R,
             frame.cFpgaRegInfo.cFpgaRegInfoA.data_k_R,
             frame.cFpgaRegInfo.cFpgaRegInfoB.data_k_R,
             frame.cFpgaRegInfo.cFpgaRegInfoA.data_b_B,
             frame.cFpgaRegInfo.cFpgaRegInfoB.data_b_B,
             frame.cFpgaRegInfo.cFpgaRegInfoA.data_k_B,
             frame.cFpgaRegInfo.cFpgaRegInfoB.data_k_B);

    /*
    strcpy(szBufTmp, "\r\n");
    for ( n=0; n<8; ++n )
    {
        sprintf(szBufTmp+strlen(szBufTmp),
                "Gamma_%d = 0x%04x:0x%04x ",
                n,
                frame.cFpgaRegInfoTmp.cFpgaRegInfoA.Gamma_Data_ab[n],
                frame.cFpgaRegInfoTmp.cFpgaRegInfoB.Gamma_Data_ab[n]);

        if ( 0 == (n+1)%2 )
        {
            strcat(szBufTmp, "\r\n");
        }
    }
    HV_Trace(nBank, szBufTmp);
    */
}

#else

void PrintFPGAInfo(int nBank, IMG_FRAME& frame, int iYArg, int iRSum, int iGSum, int iBSum)
{
    int n = 0;
    char szBufTmp[256] = {0};
    strcpy(szBufTmp, "\r\n");
    for ( n=0; n<16; ++n )
    {
        sprintf(szBufTmp+strlen(szBufTmp),
                "AvgY[%02d] = %3u ",
                n, frame.cAgcAwbInfo.AvgY[n]);

        if ( 0 == (n+1)%4 )
        {
            strcat(szBufTmp, "\r\n");
        }
    }
    HV_Trace(nBank, szBufTmp);

    int iRGBSumCount = frame.cAgcAwbInfo.PointCount;
    HV_Trace(nBank, "\niYArg = %d\niRSum = %d\niGSum = %d\niBSum = %d\niRGBSumCount = %d, [%0.3f%%, %d,%d]\n",
             iYArg,
             iRSum,
             iGSum,
             iBSum,
             iRGBSumCount,
             (float)iRGBSumCount / (IMAGE_WIDTH*IMAGE_HEIGHT),
             IMAGE_WIDTH, IMAGE_HEIGHT);
    HV_Trace(nBank, "\nFPGA regs:"
                "\n0x%08x, 0x%08x, 0x%08x, 0x%08x"
                "\n0x%08x, 0x%08x, 0x%08x, 0x%08x"
                "\n0x%08x, 0x%08x, 0x%08x, 0x%08x"
                "\n0x%08x, 0x%08x",
             frame.cFpgaRegInfo.reg_video_SHT1,
             frame.cFpgaRegInfo.reg_capture_SHT2,
             frame.cFpgaRegInfo.reg_LED_FLASH_OUT_EN,
             frame.cFpgaRegInfo.reg_capture_mode,
             frame.cFpgaRegInfo.reg_ACSP,
             frame.cFpgaRegInfo.reg_soft_capture_trig,
             frame.cFpgaRegInfo.anti_flicker,
             frame.cFpgaRegInfo.Th_Custom,
             frame.cFpgaRegInfo.Th_Count,
             frame.cFpgaRegInfo.reg_capture_edge_select,
             frame.cFpgaRegInfo.time_cnt_out,
             frame.cFpgaRegInfo.time_clock_out,
             frame.cFpgaRegInfo.sampling_point,
             frame.cFpgaRegInfo.capture_ID);
    HV_Trace(nBank, "[fpga version = 0x%x]\n",
             frame.cFpgaRegInfo.reg_fpga_version);

//    strcpy(szBufTmp, "\r\n");
//    for ( n=0; n<8; ++n )
//    {
//        sprintf(szBufTmp+strlen(szBufTmp),
//                "Gamma_%d = 0x%04x ",
//                n, frame.cFpgaRegInfo.Gamma_Data[n]);
//
//        if ( 0 == (n+1)%4 )
//        {
//            strcat(szBufTmp, "\r\n");
//        }
//    }
//    HV_Trace(nBank, szBufTmp);
    HV_Trace(nBank, "Gamma: offset: %d value: %d\n", frame.cFpgaRegInfo.Gamma_Data[0], frame.cFpgaRegInfo.Gamma_Data[1]);

    // 红灯加红信息
    HV_Trace(nBank, "[HTh LTh STh CFactor]: [0x%x  0x%x 0x%x 0x%x]\n"
                    "[LTh LFactor HFactor]: [0x%x 0x%x 0x%x]\n",
                    frame.cFpgaRegInfo.th_h_prt,
                    frame.cFpgaRegInfo.th_l_prt,
                    frame.cFpgaRegInfo.th_s_prt,
                    frame.cFpgaRegInfo.colr_prt,
                    frame.cFpgaRegInfo.th_l_reg_new,
                    frame.cFpgaRegInfo.colr_reg_new_l,
                    frame.cFpgaRegInfo.colr_reg_new_h);
    // 锐化
    HV_Trace(nBank, "Sharpen Threshold: %d Enable Sharpen: %d Enable Capture: %d\n",
                        frame.cFpgaRegInfo.sha_th_o,
                        frame.cFpgaRegInfo.sha_ce_o,
                        frame.cFpgaRegInfo.sha_s_or_a_o);

    // 饱和度、对比度
    // 转换成正常值
    frame.cFpgaRegInfo.satu_r_f_max_o = ChangeValueFromFpga(frame.cFpgaRegInfo.satu_r_f_max_o, 15);
    frame.cFpgaRegInfo.satu_r_f_min_o = ChangeValueFromFpga(frame.cFpgaRegInfo.satu_r_f_min_o, 15);
    frame.cFpgaRegInfo.satu_g_f_max_o = ChangeValueFromFpga(frame.cFpgaRegInfo.satu_g_f_max_o, 15);
    frame.cFpgaRegInfo.satu_g_f_min_o = ChangeValueFromFpga(frame.cFpgaRegInfo.satu_g_f_min_o, 15);
    frame.cFpgaRegInfo.satu_b_f_max_o = ChangeValueFromFpga(frame.cFpgaRegInfo.satu_b_f_max_o, 15);
    frame.cFpgaRegInfo.satu_b_f_min_o = ChangeValueFromFpga(frame.cFpgaRegInfo.satu_b_f_min_o, 15);

    frame.cFpgaRegInfo.cont_rgb_f_o = ChangeValueFromFpga(frame.cFpgaRegInfo.cont_rgb_f_o, 17);
    frame.cFpgaRegInfo.cont_add_f_o = ChangeValueFromFpga(frame.cFpgaRegInfo.cont_add_f_o, 11);

    HV_Trace(nBank, "Saturation Value[max min]\n"
                "R: [%d %d]\n"
                "G: [%d %d]\n"
                "B: [%d %d]\n",
            frame.cFpgaRegInfo.satu_r_f_max_o, frame.cFpgaRegInfo.satu_r_f_min_o,
            frame.cFpgaRegInfo.satu_g_f_max_o, frame.cFpgaRegInfo.satu_g_f_min_o,
            frame.cFpgaRegInfo.satu_b_f_max_o, frame.cFpgaRegInfo.satu_b_f_min_o);
    HV_Trace(nBank, "Contrast Value mul_f: %d, add_f: %d\n",
                frame.cFpgaRegInfo.cont_rgb_f_o, frame.cFpgaRegInfo.cont_add_f_o);
}

#endif

void CCamApp::OnImage(void* pContext, IMG_FRAME imgFrame)
{
    CCamApp* pThis = (CCamApp *)pContext;

    if ( imgFrame.pRefImage != NULL )
    {
        imgFrame.pRefImage->AddRef();
    }
    // 如是测试协议，则使用不丢帧方式
    bool fWaitMode = (1 == pThis->m_iCamType) ? true : false;
    if (pThis->m_pImageSender)
    {
        static BOOL fIsRunCaptureOnlyMode = FALSE;
        static DWORD32 dwLastTick = 0;
        DWORD32 dwCurTick = GetSystemTick();
        if (dwCurTick - dwLastTick >= 2000)
        {
            pThis->m_pImageSender->GetIsCaptureMode(&fIsRunCaptureOnlyMode);
            dwLastTick = dwCurTick;
        }
        if (fIsRunCaptureOnlyMode)
        {
            if (imgFrame.pRefImage->IsCaptureImage())
            {
                fWaitMode = true;
            }
            else
            {
                //AGC AWB
                int iYArg = 0;
                int iRSum = 0;
                int iGSum = 0;
                int iBSum = 0;
                GetAGCAWBInfo(imgFrame, pThis->m_rgiAGCZone, sizeof(pThis->m_rgiAGCZone)/sizeof(int), iYArg, iRSum, iGSum, iBSum);
                if ( pThis->m_iEnableAGC )
                {
                    pThis->m_cAgcAwbThread.PutYArg(iYArg);
                }

                if ( pThis->m_iEnableAWB )
                {
                    RGB_Sum cRgbSum;
                    cRgbSum.iRSum = iRSum;
                    cRgbSum.iGSum = iGSum;
                    cRgbSum.iBSum = iBSum;
                    pThis->m_cAgcAwbThread.PutRgbSum(cRgbSum);
                }

                SAFE_RELEASE(imgFrame.pRefImage);
                return;
            }
        }
    }

    if ( false == pThis->m_queImage.AddTail(imgFrame, fWaitMode) )
    {
        SAFE_RELEASE(imgFrame.pRefImage);
    }
}

#if defined(_CAMERA_PIXEL_500W_) && !defined(_HV_CAMERA_PLATFORM_)
HRESULT CCamApp::Run(void* pvParam)
{
    IMG_FRAME frame;

    // 源图像数据
    HV_COMPONENT_IMAGE imgCbYCrY;

    HV_COMPONENT_IMAGE imgJPEG;
    IReferenceComponentImage* pRefImageJpeg = NULL;

    // 随机加密认证相关
    int iFrameCount = 0;
    int iVerifyCrypt = 0;
    const int VERIFY_CRYPT_PARAM = 518400;
    srand((int)time(0));

    bool fIsCapture = false;

    // 进入相机平台的处理主循环
    while (!m_fExit)
    {
        if (m_pVideoGetter == NULL)
        {
            HV_Trace(5, "m_pVideoGetter is NULL!\n");
            HV_Exit(HEC_FAIL, "m_pVideoGetter is NULL!");
        }

        // 从相机端通过VPIF获取源图像
        frame = m_queImage.RemoveHead(500);
        if ( NULL == frame.pRefImage && FALSE == frame.fUseImgVPIF )
        {
            HV_Trace(5, "get video frame error.\n");
            continue;
        }

        iFrameCount++;
        if ( iFrameCount >= iVerifyCrypt )
        {
            if ( 0 != verify_crypt() )
            {
                // 加密认证失败
                Trace("\n:-(\n");
                g_fCopyrightValid = false;
            }
            else
            {
                // 加密认证成功
                Trace("\n:-)\n");
                g_fCopyrightValid = true;
            }

            iFrameCount = 0;
            iVerifyCrypt = ( rand() * VERIFY_CRYPT_PARAM / RAND_MAX );
            iVerifyCrypt += VERIFY_CRYPT_PARAM;
        }

        fIsCapture = (1 == frame.cFpgaRegInfo.cFpgaRegInfoA.reg_soft_capture_trig) ? (true) : (false);

        CReferenceFrame_VPIF* pRefFrame_VPIF = NULL;
        if (FAILED(CreateReferenceFrame_VPIF(
                       &pRefFrame_VPIF,
                       (CVideoGetter_VPIF*)m_pVideoGetter,
                       frame.imgVPIF,
                       frame.dwTimeTick,
                       (fIsCapture) ? (TRUE) : (FALSE)
                   )))
        {
            ((CVideoGetter_VPIF*)m_pVideoGetter)->PutCaptureBuffer(frame.imgVPIF.rgImageData[0].addr);
            HV_Trace(5, "CreateReferenceFrame_VPIF Failed!\n");
            continue;
        }

        imgCbYCrY = *(pRefFrame_VPIF->GetImage());

        //发送到数据调制器
        m_cDataMux.PutFrame(
            pRefFrame_VPIF,
            m_iH264Stream,
            m_iJpegStream != 1, //不开JPEG流时才压结果图
            m_iJpegCompressRateCapture,
            m_iJpegCompressRate,
            m_iJpegExpectSize
        );

        // AGC AWB
        int iYArg = 0;
        int iRSum = 0;
        int iGSum = 0;
        int iBSum = 0;
        GetAGCAWBInfo(frame, m_rgiAGCZone, sizeof(m_rgiAGCZone)/sizeof(int), iYArg, iRSum, iGSum, iBSum);

        // 非抓拍图才进行AGC及AWB
        if ( !fIsCapture )
        {
            if ( m_iEnableAGC )
            {
                m_cAgcAwbThread.PutYArg(iYArg);
            }

            if ( m_iEnableAWB )
            {
                RGB_Sum cRgbSum;
                cRgbSum.iRSum = iRSum;
                cRgbSum.iGSum = iGSum;
                cRgbSum.iBSum = iBSum;
                m_cAgcAwbThread.PutRgbSum(cRgbSum);
            }
        }

        //发送JPEG流
        if (m_iJpegStream)
        {
            //创建Jpeg引用对象
            if ( S_OK != CreateReferenceComponentImage(
                        &pRefImageJpeg,
                        HV_IMAGE_JPEG,
                        imgCbYCrY.iWidth,
                        imgCbYCrY.iHeight*2,
                        m_dwFrameCount++,
                        GetSystemTick(),
                        0,
                        "NULL",
                        2 ) )
            {
                break;
            }

            pRefImageJpeg->GetImage(&imgJPEG);

            //设置Jpeg的真实宽高
            imgJPEG.iHeight = imgCbYCrY.iWidth | ((imgCbYCrY.iHeight*2) << 16);

            pRefImageJpeg->SetImageSize(imgJPEG);

            HRESULT hrJpegEnc = E_FAIL;

            CAM_DSP_PARAM cCamDspParamFastJpeg;
            CAM_DSP_DATA cCamDspDataFastJpeg;
            CAM_DSP_RESPOND cCamDspRespondFastJpeg;

            cCamDspParamFastJpeg.dwProcType = PROC_TYPE_JPEGENC;
            cCamDspParamFastJpeg.cJpegEncodeParam.dwCompressRate =
                (fIsCapture) ? (m_iJpegCompressRateCapture) : (m_iJpegCompressRate);
            cCamDspParamFastJpeg.cJpegEncodeParam.iEddy = (g_cModuleParams.cCamAppParam.fIsSideInstall) ? 1 : 0;
            cCamDspDataFastJpeg.cJpegEncodeData.hvImageYuv = imgCbYCrY;
            cCamDspDataFastJpeg.cJpegEncodeData.hvImageJpg = imgJPEG;

            //tS(SendCamDspCmd_jpegenc);
            SemPend(&g_hSemEDMA);
            hrJpegEnc = g_cHvDspLinkApi.SendCamDspCmd(&cCamDspParamFastJpeg, &cCamDspDataFastJpeg, &cCamDspRespondFastJpeg);
            SemPost(&g_hSemEDMA);
            //tE(SendCamDspCmd_jpegenc);

            if ( S_OK == hrJpegEnc )
            {
                // 根据Jpeg图片期望大小，自动调节Jpeg压缩率。
                if ( !fIsCapture && m_iJpegExpectSize != 0 )
                {
                    if ( cCamDspRespondFastJpeg.cJpegEncodeRespond.dwJpegLen+10240 < (DWORD32)m_iJpegExpectSize )
                    {
                        if ( m_iJpegCompressRate < m_iJpegCompressRateH )
                        {
                            m_iJpegCompressRate++;
                        }
                    }
                    else if ( cCamDspRespondFastJpeg.cJpegEncodeRespond.dwJpegLen > (DWORD32)m_iJpegExpectSize )
                    {
                        if ( m_iJpegCompressRate > m_iJpegCompressRateL )
                        {
                            m_iJpegCompressRate--;
                        }
                    }
                }

                //设置Jpeg图片数据的实际长度
                imgJPEG.iWidth = cCamDspRespondFastJpeg.cJpegEncodeRespond.dwJpegLen;
                pRefImageJpeg->SetImageSize(imgJPEG);

                // Jpeg图片流传输
                if ( 0 == m_iSendType )
                {
                    DWORD32 dwTimeLow=0, dwTimeHigh=0;
                    GetSystemTime(&dwTimeLow, &dwTimeHigh);

                    //==================------zhaopy 发放--------=========
                    SEND_CAMERA_IMAGE imageInfo;
                    imageInfo.dwImageType =
                        (fIsCapture) ? (CAMERA_IMAGE_JPEG_CAPTURE) : (CAMERA_IMAGE_JPEG);
                    imageInfo.dwWidth = imgCbYCrY.iWidth;
                    imageInfo.dwHeight = imgCbYCrY.iHeight*2;
                    imageInfo.dwTimeLow = dwTimeLow;
                    imageInfo.dwTimeHigh = dwTimeHigh;

                    if ( AGCIsEnable() )
                    {
                        m_cAgcAwbThread.GetAGCShutterGain(
                            imageInfo.cImageExtInfo.iShutter,
                            imageInfo.cImageExtInfo.iGain
                        );
                    }
                    else
                    {
                        imageInfo.cImageExtInfo.iShutter = g_cModuleParams.cCamAppParam.iShutter;
                        imageInfo.cImageExtInfo.iGain = g_cModuleParams.cCamAppParam.iGain;
                    }

                    if ( AWBIsEnable() )
                    {
                        m_cAgcAwbThread.GetAWBGain(
                            imageInfo.cImageExtInfo.iGainR,
                            imageInfo.cImageExtInfo.iGainG,
                            imageInfo.cImageExtInfo.iGainB
                        );
                    }
                    else
                    {
                        imageInfo.cImageExtInfo.iGainR = g_cModuleParams.cCamAppParam.iGainR;
                        imageInfo.cImageExtInfo.iGainG = g_cModuleParams.cCamAppParam.iGainG;
                        imageInfo.cImageExtInfo.iGainB = g_cModuleParams.cCamAppParam.iGainB;
                    }

                    imageInfo.cFpgaExtInfo.cFpgaRegInfo = frame.cFpgaRegInfo;
                    imageInfo.cFpgaExtInfo.cAgcAwbInfo = frame.cAgcAwbInfo;
                    imageInfo.dwImageSize = imgJPEG.iWidth;
                    imageInfo.pbImage = GetHvImageData(&imgJPEG, 0);
                    // zhaopy
                    imageInfo.pRefImage = pRefImageJpeg;
                    // 发送图片
                    if ( m_pImageSender != NULL )
                    {
                        if ( FAILED(m_pImageSender->SendCameraImage(&imageInfo)))
                        {
                            Trace("<link>SendCameraImage failed!\n");
                        }
                    }
                }
            }
            else
            {
                Trace("<Jpeg> SendCamDspCmd is failed!!![0x%08x][%d Byte]\n",
                      cCamDspRespondFastJpeg.cJpegEncodeRespond.dwExtErrCode,
                      cCamDspRespondFastJpeg.cJpegEncodeRespond.dwJpegLen);

                if ( cCamDspRespondFastJpeg.cJpegEncodeRespond.dwJpegLen > 1024*1024 )
                {
                    // 图片大小超过预定的1MB时，强行开启Jpeg大小动态调节功能。
                    m_iJpegExpectSize = 900*1024;
                }
            }

            SAFE_RELEASE(pRefImageJpeg);
        }

        pRefFrame_VPIF->Release();

        /* 取JPEG测试代码
        static int t = 0;
        if( rgTimeTick[0] != 0 && (t++)&1)
        {
            if(S_OK != m_cDataMux.FetchJpeg(rgTimeTick[0], &pRefImageJpeg))
            {
                printf("FetchJpeg[%d] failed\n", rgTimeTick[0]);
            }

            if( pRefImageJpeg)
            {
                pRefImageJpeg->GetImage(&imgJPEG);

                SEND_CAMERA_IMAGE imageInfo;
                imageInfo.dwImageType = CAMERA_IMAGE_JPEG;
                imageInfo.dwWidth = imgCbYCrY.iWidth;
                imageInfo.dwHeight = imgCbYCrY.iHeight*2;
                imageInfo.dwImageSize = imgJPEG.iWidth;
                imageInfo.pbImage = GetHvImageData(&imgJPEG, 0);
                // zhaopy
                imageInfo.pRefImage = pRefImageJpeg;
                // 发送图片
                if ( m_pImageSender != NULL )
                {
                    if ( FAILED(hr = m_pImageSender->SendCameraImage(&imageInfo)))
                    {
                        HV_Trace(5, "<link>SendCameraImage failed!ret=%08x\n", hr);
                    }
                }

                pRefImageJpeg->Release();
            }
        }
        */

        // 统计并显示实际处理帧率
        static int iFps = 0;
        static int iLastTick = GetSystemTick();
        int iCurTick;
        if ( iFps++ >= 100 )
        {
            iCurTick = GetSystemTick();

            HV_Trace(5, "--- process fps: %.1f Jpeg:%d,H264:%d,JpegCR:%d ---\n\n",
                     float(100*1000) / (iCurTick - iLastTick),
                     m_iJpegStream, m_iH264Stream, m_iJpegCompressRate);

            iLastTick = iCurTick;
            iFps = 0;

            if ( 0 == g_nFpgaVersion )
            {
                g_nFpgaVersion = frame.cFpgaRegInfo.cFpgaRegInfoA.reg_fpga_version;
            }
        }
    }  // end while (!m_fExit)

    HV_Trace(5, "CCamApp::Run EXIT!!!\n");
    return S_OK;
}

#else

HRESULT CCamApp::Run(void* pvParam)
{
    IMG_FRAME frame;

    // 源图像数据
    IReferenceComponentImage *pRefImageCbYCrY=NULL;
    HV_COMPONENT_IMAGE imgCbYCrY;

    // H264编码相关
    H264_ENCODE_PARAM cH264EncodeParam;
    H264_ENCODE_DATA cH264EncodeData;
    H264_ENCODE_RESPOND cH264EncodeRespond;
    IReferenceComponentImage *pRefImageH264Frame=NULL;
    HV_COMPONENT_IMAGE imgH264Frame;

    // Jpeg编码相关
    IReferenceComponentImage *pRefImageJpeg=NULL;
    HV_COMPONENT_IMAGE imgJPEG;

    // 相机DSP处理相关
    CAM_DSP_PARAM cCamDspParam;
    CAM_DSP_DATA cCamDspData;
    CAM_DSP_RESPOND cCamDspRespond;

    // 其它协议相关
    ONE_FRAME_INFO cOneFrameInfo;
    HRESULT hr;
    HRESULT hrReturn;
    int nReturnSize;
    bool fIsCapture = false;
    bool fProcessSucceed = false;
    unsigned long ulH264FrameCount = 0;
    unsigned char *pTempData = new unsigned char[1024*1024];

    // 随机加密认证相关
    int iFrameCount = 0;
    int iVerifyCrypt = 0;
    const int VERIFY_CRYPT_PARAM = 518400;
    srand((int)time(0));

    int iControllPannelWorkMode = 0;
    int iControllPannelWorkStatus = 0;
    int iControllPannelRunStatus = 0;

    ENV_TYPE etLast = ENV_COUNT;
    bool fEnableAutoCapture = (g_cModuleParams.cCamAppParam.iEnableAutoCapture == 1);
    bool fUsedCaptureParam = false;
    int iShutterCapture = 500;
    int iGainCapture = 110;

    float fltFPS = 0;

    // 图像/视频时间
    DWORD32 dwTimeLow=0, dwTimeHigh=0;
    //字符叠加
    CCharacterOverlap cCharacterOverlap;

    // 进入相机平台的处理主循环
    while (!m_fExit)
    {
        if (g_cModuleParams.cFrontPannelParam.iUsedAutoControllMode == 1)
        {
            if (g_cModuleParams.cFrontPannelParam.iAutoRunFlag == 1)
            {
                GetPannelStatus(iControllPannelWorkMode, iControllPannelWorkStatus);
                GetControllPannelRunStatus(iControllPannelRunStatus);
                if (iControllPannelWorkMode == 1 && iControllPannelWorkStatus == 0
                        && iControllPannelRunStatus == 1)
                {
                    ResetControllPannel();
                }
            }
        }
        if(!m_queDCP.IsEmpty())
        {
            DCP dcpValue = m_queDCP.RemoveHead();
            DynChangeParam(dcpValue.eType, dcpValue.iValue, false);
        }
        if (m_pVideoGetter)
        {
            // 从相机端通过VPIF获取源图像
            frame = m_queImage.RemoveHead(500);
            if ( NULL == frame.pRefImage && FALSE == frame.fUseImgVPIF )
            {
                HV_Trace(3, "get video frame error.\n");
                continue;
            }

            iFrameCount++;
            if ( iFrameCount >= iVerifyCrypt )
            {
                if ( 0 != verify_crypt() )
                {
                    // 加密认证失败
                    HV_Trace(5, "\n:-(\n");
                    g_fCopyrightValid = false;
                }
                else
                {
                    // 加密认证成功
                    Trace("\n:-)\n");
                    g_fCopyrightValid = true;
                }

                iFrameCount = 0;
                iVerifyCrypt = ( rand() * VERIFY_CRYPT_PARAM / RAND_MAX );
                iVerifyCrypt += VERIFY_CRYPT_PARAM;
            }

#ifdef _CAMERA_PIXEL_500W_
            m_iCamType = 0;
#endif
            // 如果是非一体化相机协议，则通过PCI将图片发给从端进行识别
            if (m_iCamType != 0)
            {
                nReturnSize = sizeof(HRESULT);
                if (S_OK != frame.pRefImage->GetImage(&cOneFrameInfo.imgJpeg))
                {
                    SAFE_RELEASE(frame.pRefImage);
                    continue;
                }

                cOneFrameInfo.dwRefTime = frame.pRefImage->GetRefTime();
                strcpy(cOneFrameInfo.szFrameName, frame.pRefImage->GetFrameName());

                //图片信息
                memcpy(pTempData, &cOneFrameInfo, sizeof(cOneFrameInfo));
                //图片数据
                memcpy(
                    pTempData + sizeof(cOneFrameInfo),
                    GetHvImageData(&cOneFrameInfo.imgJpeg, 0),
                    cOneFrameInfo.imgJpeg.iWidth
                );
                hr = E_FAIL;
                hrReturn = E_FAIL;
                do
                {
                    hr = g_cHvPciLinkApi.SendData(
                             PCILINK_ONE_FRAME_DATA,
                             pTempData,
                             sizeof(cOneFrameInfo) + cOneFrameInfo.imgJpeg.iWidth,
                             &hrReturn, &nReturnSize
                         );
                    if (hr != S_OK || hrReturn != S_OK)
                    {
                        HV_Trace(5, "Send image data to slave failed! Try again!\n");
                    }
                }
                while (hr != S_OK || hrReturn != S_OK);

                SAFE_RELEASE(frame.pRefImage);
                continue;
            }

#ifdef _CAMERA_PIXEL_500W_
            imgCbYCrY = frame.imgVPIF;
#else
            pRefImageCbYCrY = frame.pRefImage;

            if ( S_OK != pRefImageCbYCrY->GetImage(&imgCbYCrY) )
            {
                SAFE_RELEASE(pRefImageCbYCrY);

                HV_Trace(5, "<CCamApp::Run> pRefImageCbYCrY GetImage is Error!\n");
                continue;
            }
#endif

#ifdef _CAMERA_PIXEL_500W_
            fIsCapture = (1 == frame.cFpgaRegInfo.cFpgaRegInfoA.reg_soft_capture_trig) ? (true) : (false);
#else
            fIsCapture = (1 == frame.cFpgaRegInfo.reg_soft_capture_trig) ? (true) : (false);
#endif
        }
        else
        {
            HV_Trace(5, "m_pVideoGetter is NULL!\n");
            HV_Sleep(3000);
            continue;
        }

        // AGC AWB
        int iYArg = 0;
        int iRSum = 0;
        int iGSum = 0;
        int iBSum = 0;
        GetAGCAWBInfo(frame, m_rgiAGCZone, sizeof(m_rgiAGCZone)/sizeof(int), iYArg, iRSum, iGSum, iBSum);

        // 非抓拍图才进行AGC及AWB
        if ( !fIsCapture )
        {
            //当计算处理的亮度值低于设置的值，则进行gamma校正
            if (g_cModuleParams.cCamAppParam.iAvgY > iYArg)
            {
                g_cCameraController.SetGammaData(g_cModuleParams.cCamAppParam.rgiGamma);
            }
            //采用默认的gamma校正
            else if (g_cModuleParams.cCamAppParam.iAvgY + 15 < iYArg)
            {
                g_cCameraController.SetGammaData(g_cModuleParams.cCamAppParam.rgiDefGamma);
            }
            // zhaopy auto...
            if ( fEnableAutoCapture )
            {
                ENV_TYPE etNow = m_cAgcAwbThread.GetEnvType();
                if ( etNow != etLast )
                {
                    switch (etNow)
                    {
                    case ENV_CLOUDY:
                        iShutterCapture = g_cModuleParams.cCamAppParam.iShutterCloudy;
                        iGainCapture = g_cModuleParams.cCamAppParam.iGainCloudy;
                        fUsedCaptureParam = true;
                        break;
                    case ENV_DAY:
                        iShutterCapture = g_cModuleParams.cCamAppParam.iShutterDay;
                        iGainCapture = g_cModuleParams.cCamAppParam.iGainDay;
                        fUsedCaptureParam = true;
                        break;
                    case ENV_BLIGHT:
                        iShutterCapture = g_cModuleParams.cCamAppParam.iShutterBLight;
                        iGainCapture = g_cModuleParams.cCamAppParam.iGainBLight;
                        fUsedCaptureParam = true;
                        break;
                    case ENV_FLIGHT:
                        iShutterCapture = g_cModuleParams.cCamAppParam.iShutterFLight;
                        iGainCapture = g_cModuleParams.cCamAppParam.iGainFLight;
                        fUsedCaptureParam = true;
                        break;
                    case ENV_NEIGHT:
                        iShutterCapture = g_cModuleParams.cCamAppParam.iShutterNeight;
                        iGainCapture = g_cModuleParams.cCamAppParam.iGainNeight;
                        fUsedCaptureParam = true;
                        break;
                    case ENV_UNKNOW:
                        fUsedCaptureParam = false;
                        break;
                    default:
                        fUsedCaptureParam = false;
                        break;
                    }

                    g_cCameraController.SetCaptureShutter_Camyu(iShutterCapture, fUsedCaptureParam);
                    g_cCameraController.SetCaptureGain_Camyu(iGainCapture, fUsedCaptureParam);

                    etLast = etNow;
                }

                static int siTestCount = 0;
                siTestCount++;
                if ( siTestCount > 100 )
                {
                    char szEnvType[64] = {0};
                    switch (etLast)
                    {
                    case ENV_UNKNOW:
                        strcpy(szEnvType, "未知");
                        break;
                    case ENV_CLOUDY:
                        strcpy(szEnvType, "阴天");
                        break;
                    case ENV_DAY:
                        strcpy(szEnvType, "晴天");
                        break;
                    case ENV_BLIGHT:
                        strcpy(szEnvType, "逆光");
                        break;
                    case ENV_FLIGHT:
                        strcpy(szEnvType, "顺光");
                        break;
                    case ENV_NEIGHT:
                        strcpy(szEnvType, "晚上");
                        break;
                    default:
                        strcpy(szEnvType, "未知");
                        break;
                    }
                    HV_Trace(5, "<EnvType:%s shtter:%d, gain:%d, %s>\n", szEnvType, iShutterCapture, iGainCapture, fUsedCaptureParam ? "yes" : "no");
                    siTestCount = 0;
                }

                static int siPutCount = 0;
                siPutCount++;
                if ( siPutCount > 15 )
                {
                    ENV_INFO envInfo;
                    envInfo.iYSum = iYArg;
                    DWORD32 dwCurTick = GetSystemTick();
                    if ( dwCurTick > m_dwPlateTick && (dwCurTick - m_dwPlateTick) < 60000 )
                    {
                        envInfo.iYPlate = m_iYPlate;
                    }
                    else
                    {
                        envInfo.iYPlate = 0;
                    }

                    if ( AGCIsEnable() )
                    {
                        m_cAgcAwbThread.GetAGCShutterGain(
                            envInfo.iShutter,
                            envInfo.iGain
                        );
                    }
                    else
                    {
                        envInfo.iShutter = g_cModuleParams.cCamAppParam.iShutter;
                        envInfo.iGain = g_cModuleParams.cCamAppParam.iGain;
                    }

                    if ( (dwCurTick - m_dwPlateTick) < 2000 && envInfo.iYPlate > 0)
                    {
                        HV_Trace(5, "<envinfo>plate:%d, env:%d, shutter:%d, gain:%d.\n",
                                 envInfo.iYPlate, envInfo.iYSum, envInfo.iShutter, envInfo.iGain);
                    }
                    m_cAgcAwbThread.PutEnvInfo(envInfo);
                    siPutCount = 0;
                }
            }
            if ( m_iEnableAGC )
            {
                m_cAgcAwbThread.PutYArg(iYArg);
            }

            if ( m_iEnableAWB )
            {
                RGB_Sum cRgbSum;
                cRgbSum.iRSum = iRSum;
                cRgbSum.iGSum = iGSum;
                cRgbSum.iBSum = iBSum;
                m_cAgcAwbThread.PutRgbSum(cRgbSum);
            }
        }

        else
        {
            HV_Trace(5, "receive a capture image");
        }
        fProcessSucceed = false;

        if ( 1 == m_iH264Stream && false == m_fH264EncodeIsOpen )
        {
            if ( S_OK == OpenH264Enc(
                        g_cHvDspLinkApi,
                        g_cModuleParams.cCamAppParam.iTargetBitRate,
                        g_cModuleParams.cCamAppParam.fIsSideInstall,
                        g_cModuleParams.cCamAppParam.iENetSyn) )
            {
                m_fH264EncodeIsOpen = true;
            }
        }
        CSemLock slock(&m_hSemEncode);
        // --------------- 双码流 ---------------
#ifdef _CAMERA_PIXEL_500W_
        if ( 1 == m_iH264Stream
                && 1 == m_iJpegStream
                && m_fH264EncodeIsOpen )
#else
        if (0) // 非500w模式下暂时屏蔽双码流
#endif
        {
            //创建H.264编码后的帧对象
            if ( S_OK != CreateReferenceComponentImage(
                        &pRefImageH264Frame,
                        HV_IMAGE_H264,
                        imgCbYCrY.iWidth,
                        imgCbYCrY.iHeight,
                        0,
                        GetSystemTick(),
                        0,
                        "NULL",
                        2 ) )
            {
                HV_Trace(5, "<CCamApp::Run> pRefImageH264Frame Create isn't S_OK.\n");
                break;
            }

            //创建Jpeg引用对象
            if ( S_OK != CreateReferenceComponentImage(
                        &pRefImageJpeg,
                        HV_IMAGE_JPEG,
                        imgCbYCrY.iWidth,
#ifdef _CAMERA_PIXEL_500W_
                        imgCbYCrY.iHeight*2,
#else
                        imgCbYCrY.iHeight,
#endif
                        m_dwFrameCount++,
                        GetSystemTick(),
                        0,
                        "NULL",
                        2 ) )
            {
                HV_Trace(5, "<CCamApp::Run> pRefImageJpeg Create isn't S_OK.\n");
                break;
            }

            pRefImageH264Frame->GetImage(&imgH264Frame);
            pRefImageJpeg->GetImage(&imgJPEG);

            //设置Jpeg的实际宽高
#ifdef _CAMERA_PIXEL_500W_
            imgJPEG.iHeight = imgCbYCrY.iWidth | ((imgCbYCrY.iHeight*2) << 16);
#else
            imgJPEG.iHeight = imgCbYCrY.iWidth | (imgCbYCrY.iHeight << 16);
#endif
            pRefImageJpeg->SetImageSize(imgJPEG);

            HRESULT hrJpegEnc = E_FAIL;
            HRESULT hrH264Enc = E_FAIL;

            DWORD32 dwJpegLen = 0;
            DWORD32 dwH264FrameLen = 0;
            DWORD32 dwH264FrameType = (DWORD32)-1;

            // Jpeg编码
            cCamDspParam.dwProcType = PROC_TYPE_JPEGENC;
            cCamDspParam.cJpegEncodeParam.dwCompressRate = m_iJpegCompressRate;
            cCamDspData.cJpegEncodeData.hvImageYuv = imgCbYCrY;
            cCamDspData.cJpegEncodeData.hvImageJpg = imgJPEG;

            // 获取时间
            GetSystemTime(&dwTimeLow, &dwTimeHigh);
            // 组装时间信息进行叠加
            cCharacterOverlap.MakeDateTimeString(cCamDspData.cJpegEncodeData.szDateTimeStrings, dwTimeLow, dwTimeHigh);

            SemPend(&g_hSemEDMA);
            hrJpegEnc = g_cHvDspLinkApi.SendCamDspCmd(&cCamDspParam, &cCamDspData, &cCamDspRespond);
            SemPost(&g_hSemEDMA);
            dwJpegLen = cCamDspRespond.cJpegEncodeRespond.dwJpegLen;
            if ( dwJpegLen > 1024*1024 )
            {
                HV_Trace(5, "dwJpegLen > 1024*1024 [%d]\n", dwJpegLen);
            }

            // H.264编码
            cCamDspParam.dwProcType = PROC_TYPE_H264ENC;
            cCamDspParam.cH264EncodeParam.dwOpType = OPTYPE_ENCODE;
            cCamDspData.cH264EncodeData.hvImageYuv = imgCbYCrY;
            cCamDspData.cH264EncodeData.hvImageFrame = imgH264Frame;

            // 叠加时间
            cCamDspData.cH264EncodeData.fDoubleStream = TRUE;
            cCharacterOverlap.MakeDateTimeString(cCamDspData.cH264EncodeData.szDateTimeStrings, dwTimeLow, dwTimeHigh);

            EEPROM_Lock();
            SemPend(&g_hSemEDMA);
            hrH264Enc = g_cHvDspLinkApi.SendCamDspCmd(&cCamDspParam, &cCamDspData, &cCamDspRespond);
            SemPost(&g_hSemEDMA);
            EEPROM_UnLock();
            dwH264FrameLen = cCamDspRespond.cH264EncodeRespond.dwFrameLen;
            dwH264FrameType = cCamDspRespond.cH264EncodeRespond.dwFrameType;
            if ( dwH264FrameLen > 1024*1024 )
            {
                HV_Trace(5, "dwH264FrameLen > 1024*1024 [%d]\n", dwH264FrameLen);
            }

            if ( S_OK == hrJpegEnc && S_OK == hrH264Enc )
            {
                fProcessSucceed = true;
                ++ulH264FrameCount;

                // H.264视频流传输
                if ( !fIsCapture && FRAME_TYPE_H264_SKIP != dwH264FrameType )
                {
                    if ( 0 == m_iSendType )
                    {
                        //==================------zhaopy 发放--------=========
                        SEND_CAMERA_VIDEO videoInfo;
                        videoInfo.dwVideoType = CAMERA_VIDEO_H264;
                        videoInfo.dwFrameType =
                            dwH264FrameType == FRAME_TYPE_H264_I ? CAMERA_FRAME_I : CAMERA_FRAME_P;
                        videoInfo.dwTimeLow = dwTimeLow;
                        videoInfo.dwTimeHigh = dwTimeHigh;

                        if ( AGCIsEnable() )
                        {
                            m_cAgcAwbThread.GetAGCShutterGain(
                                videoInfo.cVideoExtInfo.iShutter,
                                videoInfo.cVideoExtInfo.iGain
                            );
                        }
                        else
                        {
                            videoInfo.cVideoExtInfo.iShutter = g_cModuleParams.cCamAppParam.iShutter;
                            videoInfo.cVideoExtInfo.iGain = g_cModuleParams.cCamAppParam.iGain;
                        }

                        if ( AWBIsEnable() )
                        {
                            m_cAgcAwbThread.GetAWBGain(
                                videoInfo.cVideoExtInfo.iGainR,
                                videoInfo.cVideoExtInfo.iGainG,
                                videoInfo.cVideoExtInfo.iGainB
                            );
                        }
                        else
                        {
                            videoInfo.cVideoExtInfo.iGainR = g_cModuleParams.cCamAppParam.iGainR;
                            videoInfo.cVideoExtInfo.iGainG = g_cModuleParams.cCamAppParam.iGainG;
                            videoInfo.cVideoExtInfo.iGainB = g_cModuleParams.cCamAppParam.iGainB;
                        }

                        videoInfo.dwY = iYArg;
                        videoInfo.dwWidth = m_dwH264InputWidth;
                        videoInfo.dwHeight = m_dwH264InputHeight;
                        DWORD32 dwRealOutFR = 0;
                        if ( 0 == m_iENetSyn )
                        {
                            dwRealOutFR = MIN(g_cModuleParams.cCamAppParam.iOutputFrameRate, 15);
                        }
                        else
                        {
                            dwRealOutFR = MIN(g_cModuleParams.cCamAppParam.iOutputFrameRate, 12);
                        }
                        videoInfo.dwOutputFrameRate = dwRealOutFR;
                        videoInfo.dwVideoSize = dwH264FrameLen;
                        videoInfo.pbVideo = GetHvImageData(&imgH264Frame, 0);
                        // zhaopy
                        videoInfo.pRefImage = pRefImageH264Frame;
                        videoInfo.fltFPS = fltFPS;
                        // 发送视频
                        if ( m_pVideoSender != NULL )
                        {
                            if ( FAILED(m_pVideoSender->SendCameraVideo(&videoInfo)) )
                            {
                                Trace("<link>SendCameraVideo failed!\n");
                            }
                        }
                    }
                }

                // Comment by Shaorg: 通过关开编码器的方式获取IDR帧
                if ( 0 == ulH264FrameCount%m_dwIDRFrameInterval )
                {
                    if ( S_OK != CloseH264Enc(g_cHvDspLinkApi) )
                    {
                        HV_Trace(5, "CloseH264Enc is Error! [FrameCount: %d]\n", ulH264FrameCount);
                        HV_Exit(HEC_FAIL, "CloseH264Enc is Error!");
                    }
                    if ( S_OK != OpenH264Enc(
                                g_cHvDspLinkApi,
                                g_cModuleParams.cCamAppParam.iTargetBitRate,
                                g_cModuleParams.cCamAppParam.fIsSideInstall,
                                g_cModuleParams.cCamAppParam.iENetSyn) )
                    {
                        HV_Trace(5, "OpenH264Enc is Error! [FrameCount: %d]\n", ulH264FrameCount);
                        HV_Exit(HEC_FAIL, "OpenH264Enc is Error!");
                    }
                }

                // Jpeg图片流传输

                //设置Jpeg图片数据的实际长度
                imgJPEG.iWidth = dwJpegLen;
                pRefImageJpeg->SetImageSize(imgJPEG);

                if ( 0 == m_iSendType )
                {
#ifdef _CAMERA_PIXEL_500W_
                    GetSystemTime(&dwTimeLow, &dwTimeHigh);
#else
                    // 使能字符叠加使用叠加的时间，否则使用图片时间
                    if (!g_cModuleParams.cCamAppParam.iEnableCharacterOverlap)
                    {
                        ConvertTickToSystemTime(pRefImageCbYCrY->GetRefTime(), dwTimeLow, dwTimeHigh);
                    }
#endif
                    //==================------zhaopy 发放--------=========
                    SEND_CAMERA_IMAGE imageInfo;
                    imageInfo.dwImageType = CAMERA_IMAGE_JPEG;
                    imageInfo.dwWidth = imgCbYCrY.iWidth;
#ifdef _CAMERA_PIXEL_500W_
                    imageInfo.dwHeight = imgCbYCrY.iHeight*2;
#else
                    imageInfo.dwHeight = imgCbYCrY.iHeight;
#endif
                    imageInfo.dwTimeLow = dwTimeLow;
                    imageInfo.dwTimeHigh = dwTimeHigh;

                    if ( AGCIsEnable() )
                    {
                        m_cAgcAwbThread.GetAGCShutterGain(
                            imageInfo.cImageExtInfo.iShutter,
                            imageInfo.cImageExtInfo.iGain
                        );
                    }
                    else
                    {
                        imageInfo.cImageExtInfo.iShutter = g_cModuleParams.cCamAppParam.iShutter;
                        imageInfo.cImageExtInfo.iGain = g_cModuleParams.cCamAppParam.iGain;
                    }

                    if ( AWBIsEnable() )
                    {
                        m_cAgcAwbThread.GetAWBGain(
                            imageInfo.cImageExtInfo.iGainR,
                            imageInfo.cImageExtInfo.iGainG,
                            imageInfo.cImageExtInfo.iGainB
                        );
                    }
                    else
                    {
                        imageInfo.cImageExtInfo.iGainR = g_cModuleParams.cCamAppParam.iGainR;
                        imageInfo.cImageExtInfo.iGainG = g_cModuleParams.cCamAppParam.iGainG;
                        imageInfo.cImageExtInfo.iGainB = g_cModuleParams.cCamAppParam.iGainB;
                    }

                    imageInfo.cFpgaExtInfo.cFpgaRegInfo = frame.cFpgaRegInfo;
                    imageInfo.cFpgaExtInfo.cAgcAwbInfo = frame.cAgcAwbInfo;
                    imageInfo.dwJpegCompressRate = m_iJpegCompressRate;
                    imageInfo.dwY = iYArg;
                    imageInfo.dwImageSize = imgJPEG.iWidth;
                    imageInfo.pbImage = GetHvImageData(&imgJPEG, 0);
                    // zhaopy
                    imageInfo.pRefImage = pRefImageJpeg;
                    // 发送图片
                    if ( m_pImageSender != NULL )
                    {
                        if ( FAILED(m_pImageSender->SendCameraImage(&imageInfo)))
                        {
                            HV_Trace(5, "<link>SendCameraImage failed!\n");
                        }
                    }
                }
            }
            else
            {
                HV_Trace(5, "<Jpeg&H.264> SendCamDspCmd is failed!!!\n");
            }

            SAFE_RELEASE(pRefImageH264Frame);
            SAFE_RELEASE(pRefImageJpeg);
        }

        // --------------- H.264流 ---------------
        if ( !fIsCapture
                && 1 == m_iH264Stream
                && 0 == m_iJpegStream
                && m_fH264EncodeIsOpen )
        {
            //创建H.264编码后的帧对象
            if ( S_OK != CreateReferenceComponentImage(
                        &pRefImageH264Frame,
                        HV_IMAGE_H264,
                        imgCbYCrY.iWidth,
                        imgCbYCrY.iHeight,
                        0,
                        GetSystemTick(),
                        0,
                        "NULL",
                        2 ) )
            {
                HV_Trace(5, "<CCamApp::Run> pRefImageH264Frame Create isn't S_OK.\n");
                break;
            }
            else
            {
                pRefImageH264Frame->GetImage(&imgH264Frame);

                cH264EncodeParam.dwOpType = OPTYPE_ENCODE;
                cH264EncodeParam.fEnableH264BrightnessAdjust = g_cModuleParams.cCamAppParam.fEnableH264BrightnessAdjust;
                cH264EncodeParam.iAdjustPointX = g_cModuleParams.cCamAppParam.iAdjustPointX;
                cH264EncodeParam.iAdjustPointY = g_cModuleParams.cCamAppParam.iAdjustPointY;

                cH264EncodeData.hvImageYuv = imgCbYCrY;
                cH264EncodeData.hvImageFrame = imgH264Frame;

                HRESULT hrH264Enc = E_FAIL;

                // 叠加时间
                GetSystemTime(&dwTimeLow, &dwTimeHigh);
                cCamDspData.cH264EncodeData.fDoubleStream = FALSE;
                cCharacterOverlap.MakeDateTimeString(cH264EncodeData.szDateTimeStrings, dwTimeLow, dwTimeHigh);

                //tS(SendH264EncodeCmd);
                EEPROM_Lock();
                SemPend(&g_hSemEDMA);
                hrH264Enc = g_cHvDspLinkApi.SendH264EncodeCmd(&cH264EncodeParam, &cH264EncodeData, &cH264EncodeRespond);
                SemPost(&g_hSemEDMA);
                EEPROM_UnLock();
                //tE(SendH264EncodeCmd);

                if ( S_OK == hrH264Enc )
                {
                    fProcessSucceed = true;
                    ++ulH264FrameCount;

                    if ( FRAME_TYPE_H264_SKIP != cH264EncodeRespond.dwFrameType )
                    {
                        // H.264视频流传输
                        if ( 0 == m_iSendType )
                        {
                            //==================------zhaopy 发放--------=========
                            SEND_CAMERA_VIDEO videoInfo;
                            videoInfo.dwVideoType = CAMERA_VIDEO_H264;
                            videoInfo.dwFrameType =
                                cH264EncodeRespond.dwFrameType == FRAME_TYPE_H264_I ? CAMERA_FRAME_I : CAMERA_FRAME_P;

                            videoInfo.dwTimeLow = dwTimeLow;
                            videoInfo.dwTimeHigh = dwTimeHigh;

                            if ( AGCIsEnable() )
                            {
                                m_cAgcAwbThread.GetAGCShutterGain(
                                    videoInfo.cVideoExtInfo.iShutter,
                                    videoInfo.cVideoExtInfo.iGain
                                );
                            }
                            else
                            {
                                videoInfo.cVideoExtInfo.iShutter = g_cModuleParams.cCamAppParam.iShutter;
                                videoInfo.cVideoExtInfo.iGain = g_cModuleParams.cCamAppParam.iGain;
                            }

                            if ( AWBIsEnable() )
                            {
                                m_cAgcAwbThread.GetAWBGain(
                                    videoInfo.cVideoExtInfo.iGainR,
                                    videoInfo.cVideoExtInfo.iGainG,
                                    videoInfo.cVideoExtInfo.iGainB
                                );
                            }
                            else
                            {
                                videoInfo.cVideoExtInfo.iGainR = g_cModuleParams.cCamAppParam.iGainR;
                                videoInfo.cVideoExtInfo.iGainG = g_cModuleParams.cCamAppParam.iGainG;
                                videoInfo.cVideoExtInfo.iGainB = g_cModuleParams.cCamAppParam.iGainB;
                            }
                            videoInfo.cVideoExtInfo.dwFrameID = ulH264FrameCount;

                            videoInfo.dwY = iYArg;
                            videoInfo.dwWidth = m_dwH264InputWidth;
                            videoInfo.dwHeight = m_dwH264InputHeight;
                            DWORD32 dwRealOutFR = 0;
                            if ( 0 == m_iENetSyn )
                            {
                                dwRealOutFR = MIN(g_cModuleParams.cCamAppParam.iOutputFrameRate, 15);
                            }
                            else
                            {
                                dwRealOutFR = MIN(g_cModuleParams.cCamAppParam.iOutputFrameRate, 12);
                            }
                            videoInfo.dwOutputFrameRate = dwRealOutFR;
                            videoInfo.dwVideoSize = cH264EncodeRespond.dwFrameLen;
                            videoInfo.pbVideo = GetHvImageData(&imgH264Frame, 0);
                            // zhaopy
                            videoInfo.pRefImage = pRefImageH264Frame;
                            // 发送视频
                            if ( m_pVideoSender != NULL )
                            {
                                if ( FAILED(m_pVideoSender->SendCameraVideo(&videoInfo)) )
                                {
                                    Trace("<link>SendCameraVideo failed!\n");
                                }
                            }
                        }
                    }

                    // Comment by Shaorg: 通过关开编码器的方式获取IDR帧
                    if ( 0 == ulH264FrameCount%m_dwIDRFrameInterval )
                    {
                        if ( S_OK != CloseH264Enc(g_cHvDspLinkApi) )
                        {
                            HV_Trace(5, "CloseH264Enc is Error! [FrameCount: %d]\n", ulH264FrameCount);
                            HV_Exit(HEC_FAIL, "CloseH264Enc is Error!");
                        }
                        if ( S_OK != OpenH264Enc(
                                    g_cHvDspLinkApi,
                                    g_cModuleParams.cCamAppParam.iTargetBitRate,
                                    g_cModuleParams.cCamAppParam.fIsSideInstall,
                                    g_cModuleParams.cCamAppParam.iENetSyn) )
                        {
                            HV_Trace(5, "OpenH264Enc is Error! [FrameCount: %d]\n", ulH264FrameCount);
                            HV_Exit(HEC_FAIL, "OpenH264Enc is Error!");
                        }
                    }
                }
                else
                {
                    Trace("<H.264> SendH264EncodeCmd is failed!!!\n");
                }

                SAFE_RELEASE(pRefImageH264Frame);
            }
        }

        // --------------- Jpeg流 ---------------
        if ( (TRUE == frame.fSplitUV || TRUE == frame.fRotateY || HV_IMAGE_BT1120 == imgCbYCrY.nImgType)
                && (1 == m_iJpegStream && !m_fSendDebugImage)
                && 0 == m_iH264Stream )
        {
            //创建Jpeg引用对象
            if ( S_OK != CreateReferenceComponentImage(
                        &pRefImageJpeg,
                        HV_IMAGE_JPEG,
                        imgCbYCrY.iWidth,
#ifdef _CAMERA_PIXEL_500W_
                        imgCbYCrY.iHeight*2,
#else
                        imgCbYCrY.iHeight,
#endif
                        m_dwFrameCount++,
                        GetSystemTick(),
                        0,
                        "NULL",
                        2 ) )
            {
                HV_Trace(5, "<CCamApp::Run> pRefImageJpeg Create isn't S_OK.\n");
                break;
            }

            pRefImageJpeg->GetImage(&imgJPEG);

            //设置Jpeg的真实宽高
#ifdef _CAMERA_PIXEL_500W_
            imgJPEG.iHeight = imgCbYCrY.iWidth | ((imgCbYCrY.iHeight*2) << 16);
#else
            imgJPEG.iHeight = imgCbYCrY.iWidth | (imgCbYCrY.iHeight << 16);
#endif
            pRefImageJpeg->SetImageSize(imgJPEG);

            HRESULT hrJpegEnc = E_FAIL;

            CAM_DSP_PARAM cCamDspParamFastJpeg;
            CAM_DSP_DATA cCamDspDataFastJpeg;
            CAM_DSP_RESPOND cCamDspRespondFastJpeg;

            cCamDspParamFastJpeg.dwProcType = PROC_TYPE_JPEGENC;
            cCamDspParamFastJpeg.cJpegEncodeParam.dwCompressRate =
                (fIsCapture) ? (m_iJpegCompressRateCapture) : (m_iJpegCompressRate);
            cCamDspParamFastJpeg.cJpegEncodeParam.iEddy = (g_cModuleParams.cCamAppParam.fIsSideInstall) ? 1 : 0;
            cCamDspParamFastJpeg.cJpegEncodeParam.dwJpegDataType = g_cModuleParams.cCamAppParam.iJpegType;
            cCamDspDataFastJpeg.cJpegEncodeData.hvImageYuv = imgCbYCrY;
            cCamDspDataFastJpeg.cJpegEncodeData.hvImageJpg = imgJPEG;

            // 获取图片时间并叠加到图像中
            ConvertTickToSystemTime(pRefImageCbYCrY->GetRefTime(), dwTimeLow, dwTimeHigh);
            cCharacterOverlap.MakeDateTimeString(cCamDspDataFastJpeg.cJpegEncodeData.szDateTimeStrings, dwTimeLow, dwTimeHigh);
            //tS(SendCamDspCmd_jpegenc);
            SemPend(&g_hSemEDMA);
            hrJpegEnc = g_cHvDspLinkApi.SendCamDspCmd(&cCamDspParamFastJpeg, &cCamDspDataFastJpeg, &cCamDspRespondFastJpeg);
            SemPost(&g_hSemEDMA);
            //tE(SendCamDspCmd_jpegenc);

            if ( S_OK == hrJpegEnc )
            {
                fProcessSucceed = true;

                // 根据Jpeg图片期望大小，自动调节Jpeg压缩率。
                if ( !fIsCapture && m_iJpegExpectSize != 0 )
                {
                    if ( cCamDspRespondFastJpeg.cJpegEncodeRespond.dwJpegLen+10240 < (DWORD32)m_iJpegExpectSize )
                    {
                        if ( m_iJpegCompressRate < m_iJpegCompressRateH )
                        {
                            m_iJpegCompressRate++;
                        }
                    }
                    else if ( cCamDspRespondFastJpeg.cJpegEncodeRespond.dwJpegLen > (DWORD32)m_iJpegExpectSize )
                    {
                        if ( m_iJpegCompressRate > m_iJpegCompressRateL )
                        {
                            m_iJpegCompressRate--;
                        }
                    }
                }

                //设置Jpeg图片数据的实际长度
                imgJPEG.iWidth = cCamDspRespondFastJpeg.cJpegEncodeRespond.dwJpegLen;
                pRefImageJpeg->SetImageSize(imgJPEG);

                // Jpeg图片流传输
                if ( 0 == m_iSendType )
                {
                    DWORD32 dwTimeLow=0, dwTimeHigh=0;
#ifdef _CAMERA_PIXEL_500W_
                    GetSystemTime(&dwTimeLow, &dwTimeHigh);
#endif
                    //==================------zhaopy 发放--------=========
                    SEND_CAMERA_IMAGE imageInfo;
                    imageInfo.dwImageType =
                        (fIsCapture) ? (CAMERA_IMAGE_JPEG_CAPTURE) : (CAMERA_IMAGE_JPEG);
                    imageInfo.dwWidth = imgCbYCrY.iWidth;
#ifdef _CAMERA_PIXEL_500W_
                    imageInfo.dwHeight = imgCbYCrY.iHeight*2;
#else
                    imageInfo.dwHeight = imgCbYCrY.iHeight;
#endif
                    imageInfo.dwTimeLow = dwTimeLow;
                    imageInfo.dwTimeHigh = dwTimeHigh;

                    if ( AGCIsEnable() )
                    {
                        m_cAgcAwbThread.GetAGCShutterGain(
                            imageInfo.cImageExtInfo.iShutter,
                            imageInfo.cImageExtInfo.iGain
                        );
                    }
                    else
                    {
                        imageInfo.cImageExtInfo.iShutter = g_cModuleParams.cCamAppParam.iShutter;
                        imageInfo.cImageExtInfo.iGain = g_cModuleParams.cCamAppParam.iGain;
                    }

                    if ( AWBIsEnable() )
                    {
                        m_cAgcAwbThread.GetAWBGain(
                            imageInfo.cImageExtInfo.iGainR,
                            imageInfo.cImageExtInfo.iGainG,
                            imageInfo.cImageExtInfo.iGainB
                        );
                    }
                    else
                    {
                        imageInfo.cImageExtInfo.iGainR = g_cModuleParams.cCamAppParam.iGainR;
                        imageInfo.cImageExtInfo.iGainG = g_cModuleParams.cCamAppParam.iGainG;
                        imageInfo.cImageExtInfo.iGainB = g_cModuleParams.cCamAppParam.iGainB;
                    }

                    imageInfo.cFpgaExtInfo.cFpgaRegInfo = frame.cFpgaRegInfo;
                    imageInfo.cFpgaExtInfo.cAgcAwbInfo = frame.cAgcAwbInfo;
                    imageInfo.dwJpegCompressRate = m_iJpegCompressRate;
                    imageInfo.dwY = iYArg;
                    imageInfo.dwImageSize = imgJPEG.iWidth;
                    imageInfo.pbImage = GetHvImageData(&imgJPEG, 0);
                    // zhaopy
                    imageInfo.pRefImage = pRefImageJpeg;
                    // 发送图片
                    if ( m_pImageSender != NULL )
                    {
                        if ( FAILED(m_pImageSender->SendCameraImage(&imageInfo)))
                        {
                            Trace("<link>SendCameraImage failed!\n");
                        }
                    }
                }
            }
            else
            {
                HV_Trace(5, "<Jpeg> SendCamDspCmd is failed!!![0x%08x][%d Byte]\n",
                         cCamDspRespondFastJpeg.cJpegEncodeRespond.dwExtErrCode,
                         cCamDspRespondFastJpeg.cJpegEncodeRespond.dwJpegLen);

                if ( cCamDspRespondFastJpeg.cJpegEncodeRespond.dwJpegLen > 1024*1024 )
                {
                    // 图片大小超过预定的1MB时，强行开启Jpeg大小动态调节功能。
                    m_iJpegExpectSize = 900*1024;
                }
            }

            SAFE_RELEASE(pRefImageJpeg);
        }

        // --------------- 无码流 ---------------
        if ( 0 == m_iJpegStream && 0 == m_iH264Stream )
        {
            fProcessSucceed = true;
        }

#ifdef _CAMERA_PIXEL_500W_
        ((CVideoGetter_VPIF*)m_pVideoGetter)->PutCaptureBuffer(frame.imgVPIF.rgImageData[0].addr);
#else
        SAFE_RELEASE(pRefImageCbYCrY);
#endif

        // 统计并显示实际处理帧率
        static int iFps = 0;
        static int iLastTick = GetSystemTick();
        int iCurTick;
        if ( iFps >= 100 )
        {
            iCurTick = GetSystemTick();
            fltFPS = float(100*1000) / (iCurTick - iLastTick);

            HV_Trace(5, "--- process fps: %.1f Jpeg:%d,H264:%d,JpegCR:%d ---\n\n",
                     fltFPS,
                     m_iJpegStream, m_iH264Stream, m_iJpegCompressRate);

            iLastTick = iCurTick;
            iFps = 0;

            // --- DebugOutput FPAG output value --- [start]

#ifdef _CAMERA_PIXEL_500W_
            PrintFPGAInfo_500w(1, frame, iYArg, iRSum, iGSum, iBSum);
#else
            PrintFPGAInfo(1, frame, iYArg, iRSum, iGSum, iBSum);
#endif // _CAMERA_PIXEL_500W_

            // --- DebugOutput FPAG output value --- [end]

            if ( 0 == g_nFpgaVersion )
            {
#ifdef _CAMERA_PIXEL_500W_
                g_nFpgaVersion = frame.cFpgaRegInfo.cFpgaRegInfoA.reg_fpga_version;
#else
                g_nFpgaVersion = frame.cFpgaRegInfo.reg_fpga_version;
#endif
            }
        }
        else if ( fProcessSucceed )
        {
            iFps++;
        }
    }  // end while (!m_fExit)

    HV_Trace(5, "CCamApp::Run EXIT!!!\n");
    return S_OK;
}
#endif // #if defined(_CAMERA_PIXEL_500W_) && !defined(_HV_CAMERA_PLATFORM_)

// 动态改变参数
HRESULT CCamApp::DynChangeParam(DCP_TYPE eType, int iValue, bool fFromPCI)
{
    CSemLock slock(&m_hSemEncode);

    DCP dcpValue;
    dcpValue.eType = eType;
    dcpValue.iValue = iValue;

    HV_Trace(1, "<DynChangeParam> [0x%02x][%d]\n", eType,iValue);

    switch ( eType )
    {
    case DCP_SEND_TYPE:
    {
        m_iSendType = iValue;
        g_cModuleParams.cCamAppParam.iSendType = m_iSendType;
        Trace("<DynChangeParam> DCP_SEND_TYPE[%d]\n", iValue);
    }
    break;

    case DCP_CAM_OUTPUT:
    {
        switch ( iValue )
        {
        case CAM_OUTPUT_NONE:
            m_iJpegStream = 0;
            m_iH264Stream = 0;
            break;

        case CAM_OUTPUT_JPEG_ONLY:
            m_iJpegStream = 1;
            m_iH264Stream = 0;
            break;

        case CAM_OUTPUT_H264_ONLY:
            m_iJpegStream = 0;
            m_iH264Stream = 1;
            break;

        case CAM_OUTPUT_JPEG_H264:
            m_iJpegStream = 1;
            m_iH264Stream = 1;
            break;

        case CAM_OUTPUT_DEBUG:
            m_iJpegStream = 1;
            m_iH264Stream = 0;
            break;
        }

        g_cModuleParams.cCamAppParam.iJpegStream = m_iJpegStream;
        g_cModuleParams.cCamAppParam.iH264Stream = m_iH264Stream;

        if ( 0 == m_iCamType && iValue != CAM_OUTPUT_DEBUG )
        {
            m_fSendDebugImage = false;
            PciSendSlaveImage(0);
        }
        else
        {
            m_fSendDebugImage = true;
            PciSendSlaveImage(1);
        }

        Trace("<DynChangeParam> DCP_CAM_OUTPUT[%d]\n", iValue);
    }
    break;

    case DCP_ENABLE_AGC:
    {
        m_iEnableAGC = iValue;
        if ( 1 == iValue )
        {
            if (g_cModuleParams.cCamAppParam.iAGCEnable != 1)
            {
                g_cModuleParams.cCamAppParam.iAGCEnable = m_iEnableAGC;
                m_cAgcAwbThread.SetAGCTh(g_cModuleParams.cCamAppParam.iAGCTh);

                m_cAgcAwbThread.SetAGCShutterGain(
                    g_cModuleParams.cCamAppParam.iShutter,
                    g_cModuleParams.cCamAppParam.iGain
                );

                m_cAgcAwbThread.SetAGCParam(
                    g_cModuleParams.cCamAppParam.iAGCShutterLOri,
                    g_cModuleParams.cCamAppParam.iAGCShutterHOri,
                    g_cModuleParams.cCamAppParam.iAGCGainLOri,
                    g_cModuleParams.cCamAppParam.iAGCGainHOri
                );
            }
        }
        else
        {
            g_cModuleParams.cCamAppParam.iAGCEnable = iValue;

            int iShutterTmp = 0;
            int iGainTmp = 0;

            m_cAgcAwbThread.GetAGCShutterGain(
                iShutterTmp,
                iGainTmp
            );

            if ( iShutterTmp != 0 || iGainTmp != 0 )
            {
                g_cModuleParams.cCamAppParam.iShutter = iShutterTmp;
                g_cModuleParams.cCamAppParam.iGain = iGainTmp;
            }
        }
        Trace("<DynChangeParam> EnableAGC = [%d]\n", m_iEnableAGC);
    }
    break;

    case DCP_ENABLE_AWB:
    {
        m_iEnableAWB = iValue;
        g_cModuleParams.cCamAppParam.iAWBEnable= m_iEnableAWB;
        if ( 1 == m_iEnableAWB )
        {
            // 初始化AWB起调基准
            m_cAgcAwbThread.SetAWBGain(0, 0, 0);
        }
        else
        {
            int iGainRTmp = 0;
            int iGainGTmp = 0;
            int iGainBTmp = 0;

            m_cAgcAwbThread.GetAWBGain(
                iGainRTmp,
                iGainGTmp,
                iGainBTmp
            );

            if ( iGainRTmp != 0 || iGainGTmp != 0 || iGainBTmp != 0 )
            {
                g_cModuleParams.cCamAppParam.iGainR = iGainRTmp;
                g_cModuleParams.cCamAppParam.iGainG = iGainGTmp;
                g_cModuleParams.cCamAppParam.iGainB = iGainBTmp;
            }
        }
        Trace("<DynChangeParam> EnableAWB = [%d]\n", m_iEnableAWB);
    }
    break;

    case DCP_AGC_TH:
    {
        g_cModuleParams.cCamAppParam.iAGCTh = iValue;
        m_cAgcAwbThread.ResetAGC();
        m_cAgcAwbThread.SetAGCTh(iValue);
        Trace("<DynChangeParam> DCP_AGC_TH[%d]\n", iValue);
    }
    break;

    case DCP_SHUTTER:
    {
        g_cCameraController.SetShutter_Camyu(iValue);
        Trace("<DynChangeParam> DCP_SHUTTER[%d]\n", iValue);
    }
    break;

    case DCP_GAIN:
    {
        g_cCameraController.SetGain_Camyu(iValue);
        Trace("<DynChangeParam> DCP_GAIN[%d]\n", iValue);
    }
    break;

    case DCP_JPEG_EXPECT_SIZE:
    {
        m_iJpegExpectSize = iValue;
        g_cModuleParams.cCamAppParam.iJpegExpectSize = m_iJpegExpectSize;
        Trace("<DynChangeParam> DCP_JPEG_EXPECT_SIZE[%d]\n", iValue);
    }
    break;

    case DCP_JPEG_COMPRESS_RATE:
    {
        if (PciSetJpegCompressRate(iValue) == 0)
        {
            m_iJpegCompressRate = iValue;
            g_cModuleParams.cCamAppParam.iJpegCompressRate = m_iJpegCompressRate;
            Trace("<DynChangeParam> DCP_JPEG_COMPRESS_RATE[%d]\n", iValue);
        }
        else
        {
            Trace("<DynChangeParam> DCP_JPEG_COMPRESS_RATE failed!\n");
        }
    }
    break;

    case DCP_JPEG_COMPRESS_RATE_CAPTURE:
    {
        if (PciSetCaptureCompressRate(iValue) == 0)
        {
            m_iJpegCompressRateCapture = iValue;
            g_cModuleParams.cCamAppParam.iJpegCompressRateCapture = m_iJpegCompressRateCapture;
            Trace("<DynChangeParam> DCP_JPEG_COMPRESS_RATE_CAPTURE[%d]\n", iValue);
        }
        else
        {
            Trace("<DynChangeParam> DCP_JPEG_COMPRESS_RATE_CAPTURE failed!\n");
        }
    }
    break;

    case DCP_FLASH_RATE_SYN_SIGNAL_ENABLE:
    {
        m_iFlashRateSynSignalEnable = iValue;
        g_cCameraController.SynSignalEnable(m_iFlashRateSynSignalEnable, m_iCaptureSynSignalEnable);
        Trace("<DynChangeParam> DCP_FLASH_RATE_SYN_SIGNAL_ENABLE[%d]\n", iValue);
    }
    break;

    case DCP_CAPTURE_SYN_SIGNAL_ENABLE:
    {
        m_iCaptureSynSignalEnable = iValue;
        g_cCameraController.SynSignalEnable(m_iFlashRateSynSignalEnable, m_iCaptureSynSignalEnable);
        Trace("<DynChangeParam> DCP_CAPTURE_SYN_SIGNAL_ENABLE[%d]\n", iValue);
    }
    break;

    case DCP_CPL:
    {
        if (fFromPCI)
        {
            m_queDCP.AddTail(dcpValue);
            break;
        }
        else
        {
            if (g_cModuleParams.cFrontPannelParam.iUsedAutoControllMode == 0)
            {
                //CtrlPL(iValue);
                break;
            }
            if (g_cModuleParams.cFrontPannelParam.iPolarizingPrismMode == iValue
                    || g_cModuleParams.cFrontPannelParam.iAutoRunFlag == 0
                    || m_pFrontController->GetControllPannelRunStatus() != 1)
            {
                break;
            }
            if (iValue == -1)
            {
                g_cModuleParams.cFrontPannelParam.iPolarizingPrismMode = iValue;
                break;
            }
            if (m_pFrontController->SetPolarizingPrismMode(iValue) != true)
            {
                break;
            }
            g_cModuleParams.cFrontPannelParam.iPolarizingPrismMode = iValue;
        }
    }
    break;
    case DCP_SETPULSEWIDTH:
    {
        if (fFromPCI)
        {
            m_queDCP.AddTail(dcpValue);
            break;
        }
        else
        {
            if (g_cModuleParams.cFrontPannelParam.iUsedAutoControllMode == 0
                    || g_cModuleParams.cFrontPannelParam.iAutoRunFlag == 0
                    || m_pFrontController->GetControllPannelRunStatus() != 1)
            {
                break;
            }
            int iPulseWidth = 0;
            if (g_cModuleParams.cFrontPannelParam.iPulseLevel == iValue)
            {
                if (iValue > 0)
                {
                    if (m_iFlashRateSynSignalEnable != 1)
                    {
                        m_iFlashRateSynSignalEnable = 1;
                        g_cCameraController.SynSignalEnable(m_iFlashRateSynSignalEnable, m_iCaptureSynSignalEnable);
                    }
                }
                else
                {
                    if (m_iFlashRateSynSignalEnable != 0)
                    {
                        m_iFlashRateSynSignalEnable = 0;
                        g_cCameraController.SynSignalEnable(m_iFlashRateSynSignalEnable, m_iCaptureSynSignalEnable);
                    }
                }
                break;
            }
            if (iValue == 0)
            {
                iPulseWidth = 0;
            }
            else
            {
                iPulseWidth = (iValue - 1) * g_cModuleParams.cFrontPannelParam.iPulseStep
                              + g_cModuleParams.cFrontPannelParam.iPulseWidthMin;
                if (iPulseWidth < 0) iPulseWidth = 0;
                else if (iPulseWidth > 80) iPulseWidth = 80;
            }
            if (m_pFrontController)
            {
                if (m_pFrontController->SetPulseWidth(0, iPulseWidth, 4) != true) break;
                if (m_pFrontController->SetPulseWidth(1, iPulseWidth, 4) != true) break;
                if (m_pFrontController->SetPulseWidth(2, iPulseWidth, 4) != true) break;
                if (m_pFrontController->SetPulseWidth(3, iPulseWidth, 4) != true) break;
            }
            g_cModuleParams.cFrontPannelParam.iPulseLevel = iValue;
            if (g_cModuleParams.cFrontPannelParam.iPulseLevel > 0)
            {
                if (g_cModuleParams.cFrontPannelParam.iPalanceLightStatus != 1)
                {
                    g_cModuleParams.cFrontPannelParam.iPalanceLightStatus = 1;
                    if (m_pFrontController)
                    {
                        m_pFrontController->SetPalaceLightMode(1);
                    }
                }
                if (m_iFlashRateSynSignalEnable != 1)
                {
                    m_iFlashRateSynSignalEnable = 1;
                    g_cCameraController.SynSignalEnable(m_iFlashRateSynSignalEnable, m_iCaptureSynSignalEnable);
                }
            }
            else
            {
                if (g_cModuleParams.cFrontPannelParam.iPalanceLightStatus != 0)
                {
                    g_cModuleParams.cFrontPannelParam.iPalanceLightStatus = 0;
                    if (m_pFrontController)
                    {
                        m_pFrontController->SetPalaceLightMode(0);
                    }
                }
                if (m_iFlashRateSynSignalEnable != 0)
                {
                    m_iFlashRateSynSignalEnable = 0;
                    g_cCameraController.SynSignalEnable(m_iFlashRateSynSignalEnable, m_iCaptureSynSignalEnable);
                }
            }
        }
        break;
    }

    default:
        break;
    }

    return S_OK;
}

// 功能简述：将strInfo中的strsrc全部替换为strdst
static void string_replace(string& strInfo, const string& strsrc, const string& strdst)
{
    string::size_type pos=0;
    string::size_type srclen=strsrc.size();
    string::size_type dstlen=strdst.size();
    while ( (pos=strInfo.find(strsrc, pos)) != string::npos)
    {
        strInfo.replace(pos, srclen, strdst);
        pos += dstlen;
    }
}

HRESULT CCamApp::GetHddStateInfo(char* szInfo, int& nLen)
{
    //硬盘状态信息获取
    char szShellCmd[128] = {0};
    sprintf(szShellCmd, "df -m|awk '{if($6 ~ \"/usr/sbin/%s/*\") print $0}'", ROOT_DIR);
    FILE *pFile = popen(szShellCmd, "r");
    if (pFile)
    {
        memset(szInfo, 0, nLen);
        strcpy(szInfo, "\r\n");
        fread(szInfo+2, 1, nLen, pFile);
        pclose(pFile);

        string strInfo = szInfo;
        string_replace(strInfo, "\n", "\r\n");
        strcpy(szInfo, strInfo.c_str());

        nLen = strlen(szInfo);
        if ( nLen <= 3 )
        {
            strcpy(szInfo, "\r\nCan't Found HardDisk!\r\n");
            nLen = strlen(szInfo);
        }
        return S_OK;
    }
    else
    {
        strcpy(szInfo, "\r\nGetHddStateInfo is Failed!\r\n");
        nLen = strlen(szInfo);
        return E_FAIL;
    }
}

HRESULT CCamApp::GetModStateInfo(char* szInfo, int& nLen)
{
    //模块状态信息获取
    int iErrCode=0;
    if ( true == g_pCamApp->ThreadIsOk(&iErrCode) )
    {
        strcpy(szInfo, "\r\nEvery module is OK!\r\n");
    }
    else
    {
        if (2 == iErrCode)
        {
            strcat(szInfo, "\r\nIImgGatherer is Error!!!\r\n");
        }
        else if (3 == iErrCode)
        {
            strcat(szInfo, "\r\nISendCameraImage is Error!!!\r\n");
        }
        else if (4 == iErrCode)
        {
            strcat(szInfo, "\r\nISendCameraVideo is Error!!!\r\n");
        }
        else if (5 == iErrCode)
        {
            strcat(szInfo, "\r\nISendRecord is Error!!!\r\n");
        }
        else if (6 == iErrCode)
        {
            strcat(szInfo, "\r\nISafeSaver is Error!!!\r\n");
        }
    }

    nLen = strlen(szInfo);
    return S_OK;
}

HRESULT CCamApp::OpenPL()
{
    if (g_cModuleParams.cFrontPannelParam.iUsedAutoControllMode == 1)
        return S_OK;
    return CtrlPL(1);
}

HRESULT CCamApp::ClosePL()
{
    if (g_cModuleParams.cFrontPannelParam.iUsedAutoControllMode == 1)
        return S_OK;
    return CtrlPL(0);
}

HRESULT CCamApp::GetPannelVersion(unsigned char* rgbVersionBuf, int& iBufLen)
{
    if (m_pFrontController == NULL)
    {
        return E_FAIL;
    }
    if (m_pFrontController->GetPannelVersion(rgbVersionBuf, iBufLen) == false)
    {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT CCamApp::GetPannelTemperature(float& fTemperature)
{
    if (m_pFrontController == NULL) return E_FAIL;
    unsigned short usPannelTemperature = 0;
    if (m_pFrontController->GetPannelTemperature(usPannelTemperature) == false)
    {
        fTemperature = 0.0;
        return E_FAIL;
    }
    fTemperature = (float)((usPannelTemperature >> 7) * 0.5);
    return S_OK;
}

HRESULT CCamApp::GetPannelStatus(int& iWorkMode, int& iWorkStatus)
{
    if (m_pFrontController == NULL) return E_FAIL;
    iWorkMode = m_pFrontController->GetWorkMode();
    iWorkStatus = (m_pFrontController->GetWorkStatus() == true) ? 1 : 0;
    return S_OK;
}

HRESULT CCamApp::GetPulseInfo(int& iPulseLevel, int& iPulseStep, int& iPulseWidthMin, int& iPulseWidthMax)
{
    if (m_pFrontController == NULL) return E_FAIL;
    if (g_cModuleParams.cFrontPannelParam.iAutoRunFlag == 1)
        iPulseLevel = g_cModuleParams.cFrontPannelParam.iPulseLevel;
    else
        iPulseLevel = m_iTestModePulseLevel;
    iPulseStep = g_cModuleParams.cFrontPannelParam.iPulseStep;
    iPulseWidthMin = g_cModuleParams.cFrontPannelParam.iPulseWidthMin;
    iPulseWidthMax = g_cModuleParams.cFrontPannelParam.iPulseWidthMax;
    return S_OK;
}

HRESULT CCamApp::GetOutDeviceStatus(int& iPolarizingPrismStatus, int& iPalanceLightStatus)
{
    if (m_pFrontController == NULL) return E_FAIL;
    iPolarizingPrismStatus = g_cModuleParams.cFrontPannelParam.iPolarizingPrismMode;
    iPalanceLightStatus = g_cModuleParams.cFrontPannelParam.iPalanceLightStatus;
    return S_OK;
}

HRESULT CCamApp::GetFlashInfo(void* pBuffer, int iBufLen)
{
    if (m_pFrontController == NULL) return E_FAIL;
    if (iBufLen < (int)(sizeof(int) * 12)) return E_FAIL;
    unsigned char* pTempBuf = (unsigned char*)pBuffer;
    memcpy(pTempBuf, &g_cModuleParams.cFrontPannelParam.iFlash1PulseWidth, 4);
    pTempBuf += 4;
    memcpy(pTempBuf, &g_cModuleParams.cFrontPannelParam.iFlash1Polarity, 4);
    pTempBuf += 4;
    memcpy(pTempBuf, &g_cModuleParams.cFrontPannelParam.iFlash1Coupling, 4);
    pTempBuf += 4;
    memcpy(pTempBuf, &g_cModuleParams.cFrontPannelParam.iFlash1ResistorMode, 4);
    pTempBuf += 4;

    memcpy(pTempBuf, &g_cModuleParams.cFrontPannelParam.iFlash2PulseWidth, 4);
    pTempBuf += 4;
    memcpy(pTempBuf, &g_cModuleParams.cFrontPannelParam.iFlash2Polarity, 4);
    pTempBuf += 4;
    memcpy(pTempBuf, &g_cModuleParams.cFrontPannelParam.iFlash2Coupling, 4);
    pTempBuf += 4;
    memcpy(pTempBuf, &g_cModuleParams.cFrontPannelParam.iFlash2ResistorMode, 4);
    pTempBuf += 4;

    memcpy(pTempBuf, &g_cModuleParams.cFrontPannelParam.iFlash3PulseWidth, 4);
    pTempBuf += 4;
    memcpy(pTempBuf, &g_cModuleParams.cFrontPannelParam.iFlash3Polarity, 4);
    pTempBuf += 4;
    memcpy(pTempBuf, &g_cModuleParams.cFrontPannelParam.iFlash3Coupling, 4);
    pTempBuf += 4;
    memcpy(pTempBuf, &g_cModuleParams.cFrontPannelParam.iFlash3ResistorMode, 4);
    return S_OK;
}

HRESULT CCamApp::InitControllPannel(void)
{
    if (m_pFrontController == NULL) return E_FAIL;
    if (m_pFrontController->GetControllPannelRunStatus() != 1)
    {
        return E_FAIL;
    }
    m_pFrontController->SetGridSync(g_cModuleParams.cFrontPannelParam.iDivFrequency, g_cModuleParams.cFrontPannelParam.iOutPutDelay);
    m_pFrontController->SetCaptureFlash(0,
                                        g_cModuleParams.cFrontPannelParam.iFlash1Polarity,
                                        g_cModuleParams.cFrontPannelParam.iFlash1ResistorMode,
                                        g_cModuleParams.cFrontPannelParam.iFlash1PulseWidth,
                                        g_cModuleParams.cFrontPannelParam.iFlash1Coupling);
    m_pFrontController->SetCaptureFlash(1,
                                        g_cModuleParams.cFrontPannelParam.iFlash2Polarity,
                                        g_cModuleParams.cFrontPannelParam.iFlash2ResistorMode,
                                        g_cModuleParams.cFrontPannelParam.iFlash2PulseWidth,
                                        g_cModuleParams.cFrontPannelParam.iFlash2Coupling);
    m_pFrontController->SetCaptureFlash(2,
                                        g_cModuleParams.cFrontPannelParam.iFlash3Polarity,
                                        g_cModuleParams.cFrontPannelParam.iFlash3ResistorMode,
                                        g_cModuleParams.cFrontPannelParam.iFlash3PulseWidth,
                                        g_cModuleParams.cFrontPannelParam.iFlash3Coupling);
    m_iFlashRateSynSignalEnable = 0;
    m_iCaptureSynSignalEnable = 0;//g_cModuleParams.cCamAppParam.iCaptureSynSignalEnable;
    g_cCameraController.SynSignalEnable(m_iFlashRateSynSignalEnable, m_iCaptureSynSignalEnable);
    m_pFrontController->SetPolarizingPrismMode(0);
    m_pFrontController->SetPalaceLightMode(0);
    return S_OK;
}

HRESULT CCamApp::SetControllPannelStatus(int iWorkMode, int iWorkStatus)
{
    if (m_pFrontController == NULL) return E_FAIL;
    if (m_pFrontController->GetControllPannelRunStatus() != 1) return E_FAIL;
    if (m_pFrontController->SetPannelStatus(iWorkMode, iWorkStatus) != true)
        return E_FAIL;
    if (g_cModuleParams.cFrontPannelParam.iAutoRunFlag == 0)
    {
        if (iWorkMode == 1 && iWorkStatus == 1)
        {
            SetTestModePulseLevel(m_iTestModePulseLevel);
            if (m_iTestModePLMode == 0 || m_iTestModePLMode == 1)
            {
                m_pFrontController->SetPolarizingPrismMode(m_iTestModePLMode);
            }
            m_pFrontController->SetCaptureFlash(0,
                                                g_cModuleParams.cFrontPannelParam.iFlash1Polarity,
                                                g_cModuleParams.cFrontPannelParam.iFlash1ResistorMode,
                                                g_cModuleParams.cFrontPannelParam.iFlash1PulseWidth,
                                                g_cModuleParams.cFrontPannelParam.iFlash1Coupling);
            m_pFrontController->SetCaptureFlash(1,
                                                g_cModuleParams.cFrontPannelParam.iFlash2Polarity,
                                                g_cModuleParams.cFrontPannelParam.iFlash2ResistorMode,
                                                g_cModuleParams.cFrontPannelParam.iFlash2PulseWidth,
                                                g_cModuleParams.cFrontPannelParam.iFlash2Coupling);
            m_pFrontController->SetCaptureFlash(2,
                                                g_cModuleParams.cFrontPannelParam.iFlash3Polarity,
                                                g_cModuleParams.cFrontPannelParam.iFlash3ResistorMode,
                                                g_cModuleParams.cFrontPannelParam.iFlash3PulseWidth,
                                                g_cModuleParams.cFrontPannelParam.iFlash3Coupling);
        }
    }
    return S_OK;
}

HRESULT CCamApp::SetPulseWidthRange(int iPulseMin, int iPulseMax)
{
    if (m_pFrontController == NULL) return E_FAIL;
    if (m_pFrontController->GetControllPannelRunStatus() != 1) return E_FAIL;
    if (iPulseMin < 0 || iPulseMin > 72) return E_FAIL;
    if (iPulseMax < 9 || iPulseMax > 80) return E_FAIL;
    g_cModuleParams.cFrontPannelParam.iPulseWidthMin = iPulseMin;
    g_cModuleParams.cFrontPannelParam.iPulseWidthMax = iPulseMax;
    g_cModuleParams.cFrontPannelParam.iPulseStep =
        (int)((g_cModuleParams.cFrontPannelParam.iPulseWidthMax -
               g_cModuleParams.cFrontPannelParam.iPulseWidthMin) / 7.);
    int iPulseWidth = g_cModuleParams.cFrontPannelParam.iPulseWidthMin +
                      g_cModuleParams.cFrontPannelParam.iPulseStep * (g_cModuleParams.cFrontPannelParam.iPulseLevel - 1);
    if (iPulseWidth < 0 ) iPulseWidth = 0;
    else if (iPulseWidth > 80) iPulseWidth = 80;
    m_pFrontController->SetPulseWidth(0, iPulseWidth, 4);
    m_pFrontController->SetPulseWidth(1, iPulseWidth, 4);
    m_pFrontController->SetPulseWidth(2, iPulseWidth, 4);
    m_pFrontController->SetPulseWidth(3, iPulseWidth, 4);
    return S_OK;
}

HRESULT CCamApp::TestPLMode(int iMode)
{
    if (m_pFrontController == NULL) return E_FAIL;
    if (m_pFrontController->GetControllPannelRunStatus() != 1) return E_FAIL;
    if (m_pFrontController->SetPolarizingPrismMode(iMode) != true)
    {
        return E_FAIL;
    }
    m_iTestModePLMode = iMode;
    return S_OK;
}

HRESULT CCamApp::SetFlashSingle(int iChannel, int iPolarity, int iTriggerType, int iPulseWidth, int iCoupling)
{
    if (m_pFrontController == NULL) return E_FAIL;
    if (m_pFrontController->GetControllPannelRunStatus() != 1) return E_FAIL;
    bool fIsCoupling = false;
    if (iCoupling == 0) fIsCoupling = false;
    else fIsCoupling = true;
    if (iPulseWidth < 100) return false;
    if (iPulseWidth > 4000) return false;
    if (m_pFrontController->SetCaptureFlash(iChannel, iPolarity, iTriggerType, iPulseWidth, fIsCoupling) != true)
        return E_FAIL;
    switch (iChannel)
    {
    case 0:
        g_cModuleParams.cFrontPannelParam.iFlash1PulseWidth = iPulseWidth;
        g_cModuleParams.cFrontPannelParam.iFlash1Polarity = iPolarity;
        g_cModuleParams.cFrontPannelParam.iFlash1ResistorMode = iTriggerType;
        g_cModuleParams.cFrontPannelParam.iFlash1Coupling = iCoupling;
        break;
    case 1:
        g_cModuleParams.cFrontPannelParam.iFlash2PulseWidth = iPulseWidth;
        g_cModuleParams.cFrontPannelParam.iFlash2Polarity = iPolarity;
        g_cModuleParams.cFrontPannelParam.iFlash2ResistorMode = iTriggerType;
        g_cModuleParams.cFrontPannelParam.iFlash2Coupling = iCoupling;
        break;
    case 2:
        g_cModuleParams.cFrontPannelParam.iFlash3PulseWidth = iPulseWidth;
        g_cModuleParams.cFrontPannelParam.iFlash3Polarity = iPolarity;
        g_cModuleParams.cFrontPannelParam.iFlash3ResistorMode = iTriggerType;
        g_cModuleParams.cFrontPannelParam.iFlash3Coupling = iCoupling;
        break;
    }
    return S_OK;
}

HRESULT CCamApp::GetCurrentPulseLevel(int& iPulseLevel, int& iCplStatus)
{
    if (g_cModuleParams.cFrontPannelParam.iAutoRunFlag == 1)
    {
        iPulseLevel = g_cModuleParams.cFrontPannelParam.iPulseLevel;
        iCplStatus = g_cModuleParams.cFrontPannelParam.iPolarizingPrismMode;
    }
    else
    {
        iPulseLevel = m_iTestModePulseLevel;
        iCplStatus = m_iTestModePLMode;
    }
    return S_OK;
}

HRESULT CCamApp::UpdateControllPannel(unsigned char* pUpdateFile)
{
    if (m_pFrontController == NULL) return E_FAIL;
    if (m_pFrontController->UpdatePannel(pUpdateFile) != TRUE)
        return E_FAIL;
    return S_OK;
}

HRESULT CCamApp::GetControllPannelUpdatingStatus(int& iUpdatingStatus, int& iUpdatePageIndex)
{
    if (m_pFrontController == NULL) return E_FAIL;
    if (m_pFrontController->GetUpdatingInfo(iUpdatingStatus, iUpdatePageIndex) != TRUE)
        return E_FAIL;
    return S_OK;
}

HRESULT CCamApp::GetControllPannelCRCValue(int& iCRCValue)
{
    if (m_pFrontController == NULL) return E_FAIL;
    if (m_pFrontController->GetCRCValue(iCRCValue) != TRUE)
        return E_FAIL;
    return S_OK;
}

HRESULT CCamApp::ResetControllPannel(void)
{
    if (m_pFrontController == NULL) return E_FAIL;
    if (m_pFrontController->GetControllPannelRunStatus() != 1) return E_FAIL;
    m_pFrontController->SetGridSync(g_cModuleParams.cFrontPannelParam.iDivFrequency, g_cModuleParams.cFrontPannelParam.iOutPutDelay);
    if (g_cModuleParams.cFrontPannelParam.iPolarizingPrismMode == 0 ||
            g_cModuleParams.cFrontPannelParam.iPolarizingPrismMode == 1)
    {
        if (m_pFrontController->SetPolarizingPrismMode(g_cModuleParams.cFrontPannelParam.iPolarizingPrismMode) != TRUE)
        {
            return E_FAIL;
        }
    }
    int iPulseWidth = 0;
    if (g_cModuleParams.cFrontPannelParam.iPulseLevel == 0)
    {
        iPulseWidth = 0;
    }
    else
    {
        iPulseWidth = (g_cModuleParams.cFrontPannelParam.iPulseLevel - 1)
                      * g_cModuleParams.cFrontPannelParam.iPulseStep
                      + g_cModuleParams.cFrontPannelParam.iPulseWidthMin;
        if (iPulseWidth < 0) iPulseWidth = 0;
        else if (iPulseWidth > 80) iPulseWidth = 80;
    }
    if (m_pFrontController->SetPulseWidth(0, iPulseWidth, 4) != TRUE) return E_FAIL;
    if (m_pFrontController->SetPulseWidth(1, iPulseWidth, 4) != TRUE) return E_FAIL;
    if (m_pFrontController->SetPulseWidth(2, iPulseWidth, 4) != TRUE) return E_FAIL;
    if (m_pFrontController->SetPulseWidth(3, iPulseWidth, 4) != TRUE) return E_FAIL;

    if (g_cModuleParams.cFrontPannelParam.iPulseLevel > 0)
    {
        if (g_cModuleParams.cFrontPannelParam.iPalanceLightStatus != 1)
        {
            g_cModuleParams.cFrontPannelParam.iPalanceLightStatus = 1;
            m_pFrontController->SetPalaceLightMode(1);
        }
        if (m_iFlashRateSynSignalEnable != 1)
        {
            m_iFlashRateSynSignalEnable = 1;
            g_cCameraController.SynSignalEnable(m_iFlashRateSynSignalEnable, m_iCaptureSynSignalEnable);
        }
    }
    else
    {
        if (g_cModuleParams.cFrontPannelParam.iPalanceLightStatus != 0)
        {
            g_cModuleParams.cFrontPannelParam.iPalanceLightStatus = 0;
            m_pFrontController->SetPalaceLightMode(0);
        }
        if (m_iFlashRateSynSignalEnable != 0)
        {
            m_iFlashRateSynSignalEnable = 0;
            g_cCameraController.SynSignalEnable(m_iFlashRateSynSignalEnable, m_iCaptureSynSignalEnable);
        }
    }

    if (m_pFrontController->SetCaptureFlash(0,
                                            g_cModuleParams.cFrontPannelParam.iFlash1Polarity,
                                            g_cModuleParams.cFrontPannelParam.iFlash1ResistorMode,
                                            g_cModuleParams.cFrontPannelParam.iFlash1PulseWidth,
                                            g_cModuleParams.cFrontPannelParam.iFlash1Coupling) != TRUE) return E_FAIL;
    if (m_pFrontController->SetCaptureFlash(1,
                                            g_cModuleParams.cFrontPannelParam.iFlash2Polarity,
                                            g_cModuleParams.cFrontPannelParam.iFlash2ResistorMode,
                                            g_cModuleParams.cFrontPannelParam.iFlash2PulseWidth,
                                            g_cModuleParams.cFrontPannelParam.iFlash2Coupling) != TRUE) return E_FAIL;
    if (m_pFrontController->SetCaptureFlash(2,
                                            g_cModuleParams.cFrontPannelParam.iFlash3Polarity,
                                            g_cModuleParams.cFrontPannelParam.iFlash3ResistorMode,
                                            g_cModuleParams.cFrontPannelParam.iFlash3PulseWidth,
                                            g_cModuleParams.cFrontPannelParam.iFlash3Coupling) != TRUE) return E_FAIL;
    m_pFrontController->SetPannelStatus(1, true);
    g_fResetIPT = true;
    return S_OK;
}

HRESULT CCamApp::GetControllPannelAutoRunStatus(int& iStatus)
{
    iStatus = g_cModuleParams.cFrontPannelParam.iAutoRunFlag;
    return S_OK;
}

HRESULT CCamApp::SetControllPannelAutoRunStatus(int iMode)
{
    if (m_pFrontController == NULL) return E_FAIL;
    if (m_pFrontController->GetControllPannelRunStatus() != 1) return E_FAIL;
    if (iMode == g_cModuleParams.cFrontPannelParam.iAutoRunFlag)
    {
        return S_OK;
    }
    g_cModuleParams.cFrontPannelParam.iAutoRunFlag = iMode;
    if (g_cModuleParams.cFrontPannelParam.iAutoRunFlag == 0)
    {
        m_pFrontController->SetPannelStatus(1, true);
        m_iTestModePLMode = g_cModuleParams.cFrontPannelParam.iPolarizingPrismMode;
        m_iTestModePulseLevel = g_cModuleParams.cFrontPannelParam.iPulseLevel;
    }
    else
    {
        m_pFrontController->SetPannelStatus(1, false);
    }
    return S_OK;
}

HRESULT CCamApp::SetTestModePulseLevel(int iPulseLevel)
{
    if (m_pFrontController == NULL) return E_FAIL;
    if (m_pFrontController->GetControllPannelRunStatus() != 1) return E_FAIL;
    if (iPulseLevel < 0 || iPulseLevel > 8)
    {
        return E_FAIL;
    }
    m_iTestModePulseLevel = iPulseLevel;
    int iPulseWidth = g_cModuleParams.cFrontPannelParam.iPulseWidthMin +
                      g_cModuleParams.cFrontPannelParam.iPulseStep * (m_iTestModePulseLevel - 1);
    if (iPulseWidth < 0) iPulseWidth = 0;
    else if (iPulseWidth > 80) iPulseWidth = 80;
    m_pFrontController->SetPulseWidth(0, iPulseWidth, 4);
    m_pFrontController->SetPulseWidth(1, iPulseWidth, 4);
    m_pFrontController->SetPulseWidth(2, iPulseWidth, 4);
    m_pFrontController->SetPulseWidth(3, iPulseWidth, 4);
    if (m_iTestModePulseLevel > 0)
    {
        if (g_cModuleParams.cFrontPannelParam.iPalanceLightStatus != 1)
        {
            g_cModuleParams.cFrontPannelParam.iPalanceLightStatus = 1;
            m_pFrontController->SetPalaceLightMode(1);
        }
        if (m_iFlashRateSynSignalEnable != 1)
        {
            m_iFlashRateSynSignalEnable = 1;
            g_cCameraController.SynSignalEnable(m_iFlashRateSynSignalEnable, m_iCaptureSynSignalEnable);
        }
    }
    else
    {
        if (g_cModuleParams.cFrontPannelParam.iPalanceLightStatus != 0)
        {
            g_cModuleParams.cFrontPannelParam.iPalanceLightStatus = 0;
            m_pFrontController->SetPalaceLightMode(0);
        }
        if (m_iFlashRateSynSignalEnable != 0)
        {
            m_iFlashRateSynSignalEnable = 0;
            g_cCameraController.SynSignalEnable(m_iFlashRateSynSignalEnable, m_iCaptureSynSignalEnable);
        }
    }
    return S_OK;
}

HRESULT CCamApp::GetControllPannelStyle(int& iStyle)
{
    iStyle = g_cModuleParams.cFrontPannelParam.iUsedAutoControllMode;
    return S_OK;
}

void CCamApp::InitH264WidthHeight(BOOL fIsSideInstall)
{
#ifdef _CAMERA_PIXEL_500W_
    m_dwH264InputWidth = H264_WIDTH_500M_NORMAL;
    m_dwH264InputHeight = H264_HEIGHT_500M_NORMAL;
#else
    if ( 0 == fIsSideInstall )
    {
        m_dwH264InputWidth = H264_WIDTH_200M_NORMAL;
        m_dwH264InputHeight = H264_HEIGHT_200M_NORMAL;
    }
    else
    {
        m_dwH264InputWidth = H264_WIDTH_200M_ROTATE;
        m_dwH264InputHeight = H264_HEIGHT_200M_ROTATE;
    }
#endif
}

HRESULT CCamApp::SetControllPannelDefaultParam(void)
{
    if (m_pFrontController == NULL) return E_FAIL;
    if (m_pFrontController->GetControllPannelRunStatus() != 1) return E_FAIL;
    g_cModuleParams.cFrontPannelParam.iPulseWidthMin = 5;
    g_cModuleParams.cFrontPannelParam.iPulseWidthMax = 42;
    g_cModuleParams.cFrontPannelParam.iPulseStep = 5;

    g_cModuleParams.cFrontPannelParam.iFlash1PulseWidth = 100;
    g_cModuleParams.cFrontPannelParam.iFlash1Polarity = 0;
    g_cModuleParams.cFrontPannelParam.iFlash1Coupling = 0;
    g_cModuleParams.cFrontPannelParam.iFlash1ResistorMode = 0;

    g_cModuleParams.cFrontPannelParam.iFlash2PulseWidth = 100;
    g_cModuleParams.cFrontPannelParam.iFlash2Polarity = 0;
    g_cModuleParams.cFrontPannelParam.iFlash2Coupling = 0;
    g_cModuleParams.cFrontPannelParam.iFlash2ResistorMode = 0;

    g_cModuleParams.cFrontPannelParam.iFlash3PulseWidth = 100;
    g_cModuleParams.cFrontPannelParam.iFlash3Polarity = 0;
    g_cModuleParams.cFrontPannelParam.iFlash3Coupling = 0;
    g_cModuleParams.cFrontPannelParam.iFlash3ResistorMode = 0;

    int iPulseWidth = g_cModuleParams.cFrontPannelParam.iPulseWidthMin
                      + g_cModuleParams.cFrontPannelParam.iPulseStep
                      * (g_cModuleParams.cFrontPannelParam.iPulseLevel - 1);
    if (iPulseWidth < 0)
    {
        iPulseWidth = 0;
    }
    else if (iPulseWidth > 80)
    {
        iPulseWidth = 80;
    }
    if (m_pFrontController->SetPulseWidth(0, iPulseWidth, 4) != true)
    {
        return E_FAIL;
    }
    if (m_pFrontController->SetPulseWidth(1, iPulseWidth, 4) != true)
    {
        return E_FAIL;
    }
    if (m_pFrontController->SetPulseWidth(2, iPulseWidth, 4) != true)
    {
        return E_FAIL;
    }
    if (m_pFrontController->SetPulseWidth(3, iPulseWidth, 4) != true)
    {
        return E_FAIL;
    }

    if (SetFlashSingle(0, g_cModuleParams.cFrontPannelParam.iFlash1PulseWidth,
                       g_cModuleParams.cFrontPannelParam.iFlash1Polarity,
                       g_cModuleParams.cFrontPannelParam.iFlash1Coupling,
                       g_cModuleParams.cFrontPannelParam.iFlash1ResistorMode) != S_OK)
    {
        return E_FAIL;
    }

    if (SetFlashSingle(1, g_cModuleParams.cFrontPannelParam.iFlash2PulseWidth,
                       g_cModuleParams.cFrontPannelParam.iFlash2Polarity,
                       g_cModuleParams.cFrontPannelParam.iFlash2Coupling,
                       g_cModuleParams.cFrontPannelParam.iFlash2ResistorMode) != S_OK)
    {
        return E_FAIL;
    }

    if (SetFlashSingle(2, g_cModuleParams.cFrontPannelParam.iFlash3PulseWidth,
                       g_cModuleParams.cFrontPannelParam.iFlash3Polarity,
                       g_cModuleParams.cFrontPannelParam.iFlash3Coupling,
                       g_cModuleParams.cFrontPannelParam.iFlash3ResistorMode) != S_OK)
    {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT CCamApp::Initialize()
{
#if defined(_CAMERA_PIXEL_500W_) && !defined(_HV_CAMERA_PLATFORM_)
    m_cDataMux.Initalize(
        m_pVideoSender,
        g_cModuleParams.cCamAppParam.iTargetBitRate,
        g_cModuleParams.cCamAppParam.fIsSideInstall,
        g_cModuleParams.cCamAppParam.iENetSyn,
        g_cModuleParams.cCamAppParam.iJpegCompressRateCapture,
        g_cModuleParams.cCamAppParam.iJpegCompressRate,
        g_cModuleParams.cCamAppParam.iJpegCompressRateL,
        g_cModuleParams.cCamAppParam.iJpegCompressRateH,
        g_cModuleParams.cCamAppParam.iJpegExpectSize,
        g_cModuleParams.cCamAppParam.iIFrameInterval + 1
    );
    m_iCamType = 0;
#endif

    // 参数初始化
    m_iSendType = g_cModuleParams.cCamAppParam.iSendType;
    m_iJpegStream = g_cModuleParams.cCamAppParam.iJpegStream;
    m_iH264Stream = g_cModuleParams.cCamAppParam.iH264Stream;
    m_iEnableAGC = g_cModuleParams.cCamAppParam.iAGCEnable;
    m_iEnableAWB = g_cModuleParams.cCamAppParam.iAWBEnable;
    m_iJpegCompressRateL = g_cModuleParams.cCamAppParam.iJpegCompressRateL;
    m_iJpegCompressRateH = g_cModuleParams.cCamAppParam.iJpegCompressRateH;
    m_iJpegExpectSize = g_cModuleParams.cCamAppParam.iJpegExpectSize;
    m_iJpegCompressRate = g_cModuleParams.cCamAppParam.iJpegCompressRate;
    //如果是限制图像大小的，则将压缩率限制再上下限范围内
    if (m_iJpegExpectSize > 0)
    {
        if (m_iJpegCompressRate < m_iJpegCompressRateL)
        {
            m_iJpegCompressRate = m_iJpegCompressRateL;
        }
        if (m_iJpegCompressRate > m_iJpegCompressRateH)
        {
            m_iJpegCompressRate = m_iJpegCompressRateH;
        }
    }
    m_iJpegCompressRateCapture = g_cModuleParams.cCamAppParam.iJpegCompressRateCapture;
    m_iFlashRateSynSignalEnable = g_cModuleParams.cCamAppParam.iFlashRateSynSignalEnable;
    m_iCaptureSynSignalEnable = g_cModuleParams.cCamAppParam.iCaptureSynSignalEnable;
    m_iENetSyn = g_cModuleParams.cCamAppParam.iENetSyn;

    for ( int i = 0; i < 16; ++i )
    {
        m_rgiAGCZone[i] = 1;
        m_rgiRawAGCZone[i] = 1;
    }

    m_fH264EncodeIsOpen = false;
    m_dwFrameCount = 0;
    m_dwIDRFrameInterval = g_cModuleParams.cCamAppParam.iIFrameInterval + 1;

    InitH264WidthHeight(g_cModuleParams.cCamAppParam.fIsSideInstall);

    m_iTestModePLMode = 0;
    m_iTestModePulseLevel = 0;

    if ( 1 == m_iEnableAGC )
    {
        m_cAgcAwbThread.SetAGCTh(g_cModuleParams.cCamAppParam.iAGCTh);

        m_cAgcAwbThread.SetAGCShutterGain(
            g_cModuleParams.cCamAppParam.iShutter,
            g_cModuleParams.cCamAppParam.iGain
        );

        m_cAgcAwbThread.SetAGCParam(
            g_cModuleParams.cCamAppParam.iAGCShutterLOri,
            g_cModuleParams.cCamAppParam.iAGCShutterHOri,
            g_cModuleParams.cCamAppParam.iAGCGainLOri,
            g_cModuleParams.cCamAppParam.iAGCGainHOri
        );
    }

    if ( 1 == m_iEnableAWB )
    {
        // 初始化AWB起调基准
        m_cAgcAwbThread.SetAWBGain(0, 0, 0);
    }

    if ( g_cModuleParams.cFrontPannelParam.iUsedAutoControllMode == 1 )
    {
        m_iFlashRateSynSignalEnable = 0;
        m_iCaptureSynSignalEnable = 0;//g_cModuleParams.cCamAppParam.iCaptureSynSignalEnable;
    }
    else
    {
        m_iFlashRateSynSignalEnable = g_cModuleParams.cCamAppParam.iFlashRateSynSignalEnable;
        m_iCaptureSynSignalEnable = g_cModuleParams.cCamAppParam.iCaptureSynSignalEnable;
    }

    if ( g_cModuleParams.cFrontPannelParam.iUsedAutoControllMode == 1 )
    {
        InitControllPannel();
    }

    return S_OK;
}

HRESULT CCamApp::GetControllPannelRunStatus(int& iRunStatus)
{
    if (m_pFrontController == NULL) return E_FAIL;
    iRunStatus = m_pFrontController->GetControllPannelRunStatus();
    return S_OK;
}

HRESULT CCamApp::SetCaptureSynSignalStatus(int iStatus)
{
    if (iStatus != 0 && iStatus != 1) return E_FAIL;
    if (iStatus == m_iCaptureSynSignalEnable) return S_OK;
    m_iCaptureSynSignalEnable = iStatus;
    g_cCameraController.SynSignalEnable(m_iFlashRateSynSignalEnable, m_iCaptureSynSignalEnable);
    return S_OK;
}


