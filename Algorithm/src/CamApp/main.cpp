#include "DataCtrl.h"
#include "hvtarget_ARM.h"
#include "HvDspLinkApi.h"
#include "HvPciLinkApi.h"
#include "ControlFunc.h"
#include "TcpipCfg.h"
#include "IPSearchThread.h"
#include "HvCameraLink.h"
#include "CameraCmdProcess.h"
#include "CameraCmdLink.h"
#include "CameraImageLink.h"
#include "CameraVideoLink.h"
#include "CameraRecordLink.h"
#include "CameraController.h"
#include "DspLinkMemBlocks.h"
#include "HvUtilsClass.h"
#include "HvParamIO.h"
#include "SafeSaverImpl_Net.h"
#include "AutoLinkParam.h"
#include "TrafficLightParam.h"
#include "CharacterOverlap.h"

using namespace HiVideo;

CCamApp* g_pCamApp = NULL;

HV_SEM_HANDLE g_hSemCameraDataCount;                          // 相机协议数据包总个数维护
HV_SEM_HANDLE g_hSemEDMA;                                     // DSP端进行H.264与ARM端使用DMA互斥
pthread_mutex_t g_hMutuxEEPROM = PTHREAD_MUTEX_INITIALIZER;   // ARM端读写EEPROM以及与DSP端的H.264编码互斥
static CSerialBase* g_pSerialBase = NULL;                     // 串口指针，用于互斥

int g_nFpgaVersion = 0; // FPGA版本号
bool g_fCopyrightValid = true;

/* Hvtarget_ARM.c */
extern "C" int GetCpuNearTemp(int* piTemp);

/* HvDspLinkApi.cpp */
extern HRESULT InitDspLink(int argc, char** argv);

/* HvSystemInit.cpp */
extern void LinuxSystemInit(void);

// ---------------------------------------------------------------------
void SetCamEnhance( TRAFFIC_LIGHT_PARAM& cTrafficLightParam )
{
#if !defined( _CAMERA_PIXEL_500W_ )
    const int MAX_LIGHT_COUNT = 8;  // 红灯个数

    char szLightArea[64];
    memset(szLightArea , 0 , sizeof(szLightArea));
    int iLightNum = 0;
    int iLightType = 0;
    char szLightInfor[64];
    memset(szLightInfor , 0 , sizeof(szLightInfor));
    int iLightInterval = 0;

    int iRedLightNum = 0;
    for ( int iEnhanceNum = 0 ; iEnhanceNum<MAX_LIGHT_COUNT ; iEnhanceNum++ )
    {
        if (0 != g_cCameraController.SetTrafficLightEnhanceZone( iEnhanceNum,0 , 0 ,0 , 0 ))
        {
            HV_Trace(5 , "SetTrafficLightEnhanceZone_ error!\n");
        }
    }

    if ( cTrafficLightParam.nLightCount >0 )
    {
        if ( 0 != g_cCameraController.SetTrafficLightEnhanceParam( 0x9000 , 0x999 ,0x2000 , 0x66 ) )
        {
            HV_Trace(5 , "SetTrafficLightEnhanceParam error!\n");
        }
        if ( 0 != g_cCameraController.SetTrafficLightEnhanceLumParam( cTrafficLightParam.fEnhanceRed ? 0x2d00 : 0x3fff, 0x166 ,0x0 ) )
        {
            HV_Trace(5 , "SetTrafficLightEnhanceLumParam error!\n");
        }
        int iLightLeft = 0,iLightTop = 0,iLightRight = 0,iLightBottom = 0;
        int iRLightLeft = 0,iRLightTop = 0, iRLightRight = 0 , iRLightBottom=0;
        int i = 0 , j = 0;

        for ( i = 0; i<cTrafficLightParam.nLightCount; i++ )
        {
            sscanf(cTrafficLightParam.rgszLightPos[i] ,
                   "(%d,%d,%d,%d),%d,%d,%s" ,
                   &iLightLeft , &iLightTop , &iLightRight , &iLightBottom ,
                   &iLightNum , &iLightType , szLightInfor);

            if ( iLightNum <= 0 )
            {
                continue;
            }
            iLightTop*=2;
            iLightBottom*=2;
            if ( iLightType == 0 )
            {
                iLightInterval = (iLightRight - iLightLeft+1)/iLightNum;
            }
            else
            {
                iLightInterval = (iLightBottom - iLightTop+1)/iLightNum;
            }

            for ( j = 1; j <=iLightNum ; j++ )
            {
                if ( szLightInfor[j*2 - 1] != '1' )
                {
                    continue;
                }
                if ( iLightType == 0 )
                {
                    iRLightLeft = iLightLeft + (j-1) * (iLightInterval + 1);
                    iRLightRight = iRLightLeft + iLightInterval;

                    iRLightTop = iLightTop;
                    iRLightBottom = iLightBottom;
                }
                else
                {
                    iRLightLeft = iLightLeft;
                    iRLightRight = iLightRight;

                    iRLightTop = iLightTop + (j-1) * ( iLightInterval + 1);
                    iRLightBottom = iRLightTop + iLightInterval;
                }
                //如果图片旋转，则需要修正坐标
                if(g_cModuleParams.cCamAppParam.fIsSideInstall)
                {
                    int iX1 = iRLightLeft;
                    int iY1 = iRLightTop;
                    int iX2 = iRLightRight;
                    int iY2 = iRLightBottom;

                    iRLightLeft = IMAGE_WIDTH - iY2;
                    iRLightTop = iX1;
                    iRLightRight = IMAGE_WIDTH - iY1;
                    iRLightBottom = iX2;
                }
                if ( 0 != g_cCameraController.SetTrafficLightEnhanceZone( iRedLightNum,
                        iRLightLeft , iRLightTop ,
                        iRLightRight , iRLightBottom ))
                {
                    HV_Trace(5 , "SetTrafficLightEnhanceZone_ error!\n");
                }

                iRedLightNum++;
                if ( iRedLightNum >= MAX_LIGHT_COUNT )
                {
                    break;
                }
            }
            if ( iRedLightNum >= MAX_LIGHT_COUNT )
            {
                break;
            }

        }
        // 针对FPGA A70(或以上)修改
        // 发最后一个ID号，FPGA才认为结束，才会进行加红，否则，不会加红
        iRedLightNum = MAX_LIGHT_COUNT - 1;
        g_cCameraController.SetTrafficLightEnhanceZone(iRedLightNum,
                        iRLightLeft , iRLightTop ,
                        iRLightRight , iRLightBottom);
    }
    else
    {
        HV_Trace(5 , "lightCount < 0\n");
    }
#endif
}

static void CreateGlobalResource()
{
    if (CreateSemaphore(&g_hSemCameraDataCount, 1, 1) != 0)
    {
        HV_Trace(5, "create g_hSemCameraDataCount failed!\n");
        HV_Exit(HEC_FAIL, "create g_hSemCameraDataCount failed!");
    }
    if (CreateSemaphore(&g_hSemEDMA, 1, 1) != 0)
    {
        HV_Trace(5, "create g_hSemEDMA failed!\n");
        HV_Exit(HEC_FAIL, "create g_hSemEDMA failed!");
    }
}

static void InitModuleParam()
{
    // 对FPGA进行复位(注：复位必须在设置FPGA、VPIF采集之前完成)
    g_cCameraController.ResetFpga();

    // 载入各模块参数
    HV_Trace(5, "Parameter Loading...\n");
    if ( S_OK != LoadModuleParam(g_cModuleParams) )
    {
        HV_Trace(5, "Parameter Loading Error!!!\n");
        HV_Exit(HEC_FAIL|HEC_RESET_DEV, "Parameter Loading Error!");
    }
    HV_Trace(5, "Parameter Load is finish.\n");

#ifdef _CAMERA_PIXEL_500W_
    WdtHandshake();

    int iADSValueA = 0;
    int iADSValueB = 0;
    if (EprGetADSamplingValue(&iADSValueA, &iADSValueB) == 0)
    {
        if (g_cCameraController.SetADSamplingValue(iADSValueA, iADSValueB) == 0)
        {
            HV_Trace(5, "<InitModuleParam>::SetADSamplingValue ok.\n");
        }
        else
        {
            HV_Trace(5, "<InitModuleParam>::SetADSamplingValue failed.\n");
        }
    }
    else
    {
        HV_Trace(5, "<InitModuleParam>::EprGetADSamplingValue failed.\n");
    }
#endif

    WdtHandshake();

    // 生效相机相关的参数
    g_cCameraController.SetRgbGain_Camyu(
        g_cModuleParams.cCamAppParam.iGainR,
        g_cModuleParams.cCamAppParam.iGainG,
        g_cModuleParams.cCamAppParam.iGainB
    );
    g_cCameraController.SetGain_Camyu(g_cModuleParams.cCamAppParam.iGain);
    g_cCameraController.SetShutter_Camyu(g_cModuleParams.cCamAppParam.iShutter);

    WdtHandshake();

    g_cCameraController.SetCaptureRgbGain_Camyu(
        g_cModuleParams.cCamAppParam.iCaptureGainR,
        g_cModuleParams.cCamAppParam.iCaptureGainG,
        g_cModuleParams.cCamAppParam.iCaptureGainB,
        g_cModuleParams.cCamAppParam.iEnableCaptureGainRGB
    );
    g_cCameraController.SetCaptureGain_Camyu(
        g_cModuleParams.cCamAppParam.iCaptureGain,
        g_cModuleParams.cCamAppParam.iEnableCaptureGain
    );
    g_cCameraController.SetCaptureShutter_Camyu(
        g_cModuleParams.cCamAppParam.iCaptureShutter,
        g_cModuleParams.cCamAppParam.iEnableCaptureShutter
    );

    WdtHandshake();

    g_cCameraController.SetFpsMode(g_cModuleParams.cCamAppParam.iENetSyn);
    if (g_cModuleParams.cFrontPannelParam.iUsedAutoControllMode == 1)
    {
        g_cCameraController.SynSignalEnable(0, 0);
    }
    else
    {
        g_cCameraController.SynSignalEnable(
            g_cModuleParams.cCamAppParam.iFlashRateSynSignalEnable,
            g_cModuleParams.cCamAppParam.iCaptureSynSignalEnable
        );
    }

    g_cCameraController.CaptureEdgeSelect(g_cModuleParams.cCamAppParam.iCaptureEdge);
    g_cCameraController.FlashDifferentLaneExt(g_cModuleParams.cCamAppParam.iFlashDifferentLaneExt);

    WdtHandshake();
    // 分车道抓拍(根据新旧控制板写相应的FPGA寄存器)
    g_cCameraController.SublaneCapture(g_cModuleParams.cFrontPannelParam.iUsedAutoControllMode);

    // RGB增益寄存器已改变，但需要将原来寄存器设置为0
    g_cCameraController.SetOrgRgbGainToZero();

    // 锐化、饱和度、对比度
    g_cCameraController.EnableSharpen(g_cModuleParams.cCamAppParam.iEnableSharpen);
    g_cCameraController.SetSharpenThreshold(g_cModuleParams.cCamAppParam.iSharpenThreshold);
    g_cCameraController.SetSharpenCapture(g_cModuleParams.cCamAppParam.iSharpenCapture);
    g_cCameraController.SetContrast(g_cModuleParams.cCamAppParam.iContrastValue);
    g_cCameraController.SetSaturation(g_cModuleParams.cCamAppParam.iSaturationValue);

    WdtHandshake();
    // 前端像素增益
    g_cCameraController.SetRawRGBGain(
        g_cModuleParams.cCamAppParam.iAWBRawRGain,
        g_cModuleParams.cCamAppParam.iAWBRawGrGain,
        g_cModuleParams.cCamAppParam.iAWBRawGbGain,
        g_cModuleParams.cCamAppParam.iAWBRawBGain);

}

#ifndef _HV_CAMERA_PLATFORM_

static void InitPciLink(CPciDataRecvThreadMaster& cPciDataRecv)
{
    int iRetryCount = 0;

    struct mytv
    {
        int iTick;
        int fMaster;
    }tmp;

    // 初始化PCILink
    iRetryCount = 0;
    while ( S_OK != g_cHvPciLinkApi.Init() )
    {
        WdtHandshake();

        if ( iRetryCount++ > 600 )  // 30（600*50）秒后仍然握手不成功，则复位设备。
        {
            HV_Trace(5, "waiting pci timeout,ResetHv.\n");
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "waiting pci timeout,ResetHv.");
        }
        HV_Sleep(50);
        WorkLedLight();  // 快闪工作指示灯，表明正在进行PCI握手。
        if (iRetryCount % 10 == 0)
        {
            HV_Trace(5, "waiting pci...\n");
        }
        if (iRetryCount % 200 == 0)
        {
            tmp.iTick = GetSystemTick();
            tmp.fMaster = 1;
            WriteDataToFile("/.running", (unsigned char *)&tmp, sizeof(tmp));
        }
    }

    SetWorkLedOff();

    // 启动PCILink数据接收线程
    if ( S_OK != cPciDataRecv.Start(NULL) )
    {
        HV_Trace(5, "CPciDataRecvThreadMaster Start is failed!\n");
        HV_Exit(HEC_FAIL, "CPciDataRecvThreadMaster Start failed!");
    }

    // PCILink上层握手
    g_cHvPciLinkApi.SetShakeHandsStatus(true);
    iRetryCount = 0;
    while ( 0 != PciShakeHands() )
    {
        WdtHandshake();

        if ( iRetryCount++ > 6 )
        {
            HV_Trace(5, "PCILINK_SHAKE_HANDS timeout,ResetHv.\n");
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "PCILINK_SHAKE_HANDS timeout,ResetHv.");
        }
        else
        {
            tmp.iTick = GetSystemTick();
            tmp.fMaster = 1;
            WriteDataToFile("/.running", (unsigned char *)&tmp, sizeof(tmp));

            HV_Trace(5, "PCILINK_SHAKE_HANDS...\n");
        }
    }

    SetWorkLedOn();
    HV_Trace(5, "InitPciLink ok!\n");
}

static void GetAutoLinkParam(AUTO_LINK_PARAM& cAutoLinkParam)
{
    SetWorkLedOff();
    HV_Trace(5, "waiting GetAutoLinkParam...\n");
    //获取相机参数
    HRESULT hr = E_FAIL;
    int nDataSize = sizeof(cAutoLinkParam);
    while (hr != S_OK)
    {
        WdtHandshake();

        hr = g_cHvPciLinkApi.SendData(
                 PCILINK_GET_AUTO_LINK_PARAM,
                 NULL, 0, &cAutoLinkParam,
                 &nDataSize, 1000
             );
        if (hr == S_OK)
        {
            HV_Trace(5, "GetAutoLinkParam is OK.[AutoLinkEnable = %d, nDataSize = %d]\n", cAutoLinkParam.fAutoLinkEnable, nDataSize);
            break;
        }

        static int iRetryCount = 0;
        if ( iRetryCount++ > 6 )  // 6次获取主动连接失败则复位设备
        {
            HV_Trace(5, "waiting GetAutoLinkParam timeout,ResetHv.\n");
            HV_Exit(HEC_FAIL, "waiting GetAutoLinkParam timeout,ResetHv.");
        }
        HV_Trace(5, "waiting GetAutoLinkParam...\n");
    }
}

static void GetTrafficLightParam(TRAFFIC_LIGHT_PARAM& cTrafficLightParam)
{
    SetWorkLedOff();
    HV_Trace(5, "waiting GetTrafficLightParam...\n");
    //获取相机参数
    HRESULT hr = E_FAIL;
    int nDataSize = sizeof( cTrafficLightParam );
    while ( hr != S_OK )
    {
        WdtHandshake();

        hr = g_cHvPciLinkApi.SendData(
                 PCILINK_GET_TRAFFIC_LIGHT_PARAM ,
                 NULL, 0, &cTrafficLightParam,
                 &nDataSize, 1000
             );
        if (hr == S_OK)
        {
            HV_Trace(5, "GetTrafficLightParam is OK.[nLightCount = %d, nDataSize = %d]\n", cTrafficLightParam.nLightCount ,  nDataSize);
            break;
        }

        static int iRetryCount = 0;
        if ( iRetryCount++ > 6 )  // 6次获取主动连接失败则复位设备
        {
            HV_Trace(5, "waiting GetTrafficLightParam timeout,ResetHv.\n");
            HV_Exit(HEC_FAIL, "waiting GetTrafficLightParam timeout,ResetHv.");
        }
        HV_Trace(5, "waiting GetTrafficLightParam...\n");
    }

}

static void GetPanoramicCamaraParam(PANORAMIC_CAPTURER_PARAM& cPanoramicCamaraParam)
{
    HRESULT hr = E_FAIL;
    int nDataSize = sizeof(cPanoramicCamaraParam);
    while(hr != S_OK)
    {
        WdtHandshake();
        hr = g_cHvPciLinkApi.SendData(
                    PCILINK_GET_PANORAMIC_CAMARA_PARAM,
                    NULL, 0, &cPanoramicCamaraParam,
                    &nDataSize, 1000);
        if(hr == S_OK)
        {
            break;
        }
        static int iRetryCount = 0;
        if(iRetryCount++ > 6)
        {
            HV_Trace(5, "Get Panoramic Camara Config Param Failed\n");
            break;
        }
    }
}

static void GetCamCfgParam(CAM_CFG_PARAM& cCamCfgParam)
{
    SetWorkLedOff();
    HV_Trace(5, "waiting GetCamCfgParam...\n");
    //获取相机参数
    HRESULT hr = E_FAIL;
    int nDataSize = sizeof(CAM_CFG_PARAM);
    while (hr != S_OK)
    {
        WdtHandshake();

        hr = g_cHvPciLinkApi.SendData(
                 PCILINK_GET_CAMERA_PARAM,
                 NULL, 0, &cCamCfgParam,
                 &nDataSize, 1000
             );
        if (hr == S_OK)
        {
            HV_Trace(5, "GetCamCfgParam is OK.[iCamType = %d, nDataSize = %d]\n", cCamCfgParam.iCamType, nDataSize);
            break;
        }

        static int iRetryCount = 0;
        if ( iRetryCount++ > 6 )  // 6次获取相机类型都失败则复位设备。
        {
            HV_Trace(5, "waiting Camera Type timeout,ResetHv.\n");
            HV_Exit(HEC_FAIL, "waiting Camera Type timeout,ResetHv.");
        }
        HV_Trace(5, "waiting Camera Type...\n");
    }
}

static void SendControllPannelParam(FrontPannelParam& cFrontPannelParam)
{
    HRESULT hr = E_FAIL;
    HRESULT hrReturn = E_FAIL;
    int iUsedAutoControllMode = cFrontPannelParam.iUsedAutoControllMode;
    int iDataSize = sizeof(int);
    int iReturnSize = sizeof(hrReturn);
    while (hr != S_OK)
    {
        WdtHandshake();

        hr = g_cHvPciLinkApi.SendData(
                 PCILINK_SENE_PANNEL_PARAM,
                 &iUsedAutoControllMode, iDataSize,
                 &hrReturn, &iReturnSize, 1000
                 );
        if (hr == S_OK && iReturnSize == sizeof(HRESULT) && hrReturn == S_OK)
        {
            break;
        }

        static int iRetryCount = 0;
        if ( iRetryCount++ > 6 )  // 6次获取相机类型都失败则复位设备。
        {
            HV_Trace(5, "Send Controll Pannel Param Failed!\n");
            break;
        }
    }
}

static void SetSlaveCompressRate(ModuleParams& cModuleParams)
{
    int nTryCount = 5;
    for (int i = 0; i < nTryCount; i++)
    {
        if (PciSetJpegCompressRate(cModuleParams.cCamAppParam.iJpegCompressRate) == 0)
        {
            break;
        }
        if (i != (nTryCount - 1))
        {
            HV_Trace(5, "Set jpeg compress rate failed! Try again.\n");
        }
        else
        {
            HV_Trace(5, "Set jpeg compress rate failed! ResetHv.\n");
            HV_Exit(HEC_FAIL, "Set jpeg compress rate failed! ResetHv.");
        }
        HV_Sleep(500);
    }

    for (int i = 0; i < nTryCount; i++)
    {
        if (PciSetJpegType(cModuleParams.cCamAppParam.iJpegType) == 0)
        {
            break;
        }
        if (i != (nTryCount - 1))
        {
            HV_Trace(5, "Set jpeg type rate failed! Try again.\n");
        }
        else
        {
            HV_Trace(5, "Set jpeg type rate failed! ResetHv.\n");
            HV_Exit(HEC_FAIL, "Set jpeg type rate failed! ResetHv.");
        }
        HV_Sleep(500);
    }

    for (int i = 0; i < nTryCount; i++)
    {
        if (PciSetCaptureCompressRate(cModuleParams.cCamAppParam.iJpegCompressRateCapture) == 0)
        {
            break;
        }
        if (i != (nTryCount - 1))
        {
            HV_Trace(5, "Set capture compress rate failed! Try again.\n");
        }
        else
        {
            HV_Trace(5, "Set capture compress rate failed! ResetHv.\n");
            HV_Exit(HEC_FAIL, "Set cap compress rate failed! ResetHv.");
        }
        HV_Sleep(500);
    }
}

#endif // _HV_CAMERA_PLATFORM_

static void InitMemBlocksEx(int iCamType)
{
    SHARE_MEM_MODE t;

#ifndef _HV_CAMERA_PLATFORM_
    // 一体机平台模式
    if ( 0 == iCamType )  //根据相机类型初始化共享内存
    {
        // 一体式相机
        #ifdef _CAMERA_PIXEL_500W_
            t = CAMAPP_FULLPLATFORM_HVCAM_500W_PIXEL;  // 500w
        #else
            t = CAMAPP_FULLPLATFORM_HVCAM;  // 200w
        #endif
    }
    else
    {
        // 其它相机：例如创宇相机
        #ifdef _CAMERA_PIXEL_500W_
            t = CAMAPP_FULLPLATFORM_OTHER_500W_PIXEL;  // 500w
        #else
            t = CAMAPP_FULLPLATFORM_OTHER;  // 200w
        #endif
    }
#else
    // 单相机平台模式
    #ifdef _CAMERA_PIXEL_500W_
        t = CAMAPP_CAMERAONLY_500W_PIXEL;  // 500w
    #else
        t = CAMAPP_CAMERAONLY;  // 200w
    #endif
#endif // _HV_CAMERA_PLATFORM_

    if ( S_OK != InitDspLinkMemBlocks(t) )
    {
        HV_Trace(5, "Memory blocks initialize failed!\n");
        HV_Exit(HEC_FAIL|HEC_RESET_DEV, "Memory blocks initialize failed!");
    }
}

static void InitStorage(ISafeSaver** ppcSafeSaver, ModuleParams& cModuleParams)
{
	//固态硬盘
	if (2 == cModuleParams.cCamAppParam.iDiskType)
	{
	    int iDiskCount = 0;
	    if( cModuleParams.cCamAppParam.fIsSafeSaver )
	    {
	        ++iDiskCount;
	    }
	    if( cModuleParams.cCamAppParam.fSaveVideo )
	    {
	        ++iDiskCount;
	    }
	    HV_Trace(5, "InitStorage iDiskCount = %d", iDiskCount);
		if (0 < cModuleParams.cCamAppParam.fInitHdd)
		{
			g_cHddOpThread.SetHddOpType(cModuleParams.cCamAppParam.fInitHdd == 1 ? HOT_FULL_INIT : HOT_CHECKFACTOR, iDiskCount);
			g_cHddOpThread.Start(NULL);
			HV_Trace(5, "Hdd init...\n");
		}
		if (cModuleParams.cCamAppParam.fIsSafeSaver || cModuleParams.cCamAppParam.fSaveVideo)
		{
			if (!cModuleParams.cCamAppParam.fInitHdd)
			{
				g_cHddOpThread.SetHddOpType(HOT_INIT, iDiskCount);
				g_cHddOpThread.Start(NULL);
				HV_Trace(5, "Hdd check...\n");
			}
			*ppcSafeSaver = new CSafeSaverDm6467Impl();
			SSafeSaveParam sParam;
			sParam.fSaveRecord = cModuleParams.cCamAppParam.fIsSafeSaver;
			sParam.fSaveVideo = cModuleParams.cCamAppParam.fSaveVideo;
			sParam.iFileSize = cModuleParams.cCamAppParam.iFileSize;
			strcpy(sParam.szNFSParam, cModuleParams.cCamAppParam.szNFSParam);
			(*ppcSafeSaver)->Init(&sParam);
		}
	}
	else if ( 0 == cModuleParams.cCamAppParam.iDiskType
       && (TRUE == cModuleParams.cCamAppParam.fIsSafeSaver
       || TRUE == cModuleParams.cCamAppParam.fSaveVideo))
    {
        *ppcSafeSaver = new CSafeSaverNetImpl();
        SSafeSaveParam sParam;
        sParam.fFormat = (1 == cModuleParams.cCamAppParam.fInitHdd);
        sParam.iNetDiskType = cModuleParams.cCamAppParam.iDiskType;
        strcpy(sParam.szNetDiskIP, cModuleParams.cCamAppParam.szNetDiskIP);
        strcpy(sParam.szFtpRoot, cModuleParams.cCamAppParam.szFtpRoot);
        strcpy(sParam.szNetDiskUser, cModuleParams.cCamAppParam.szNetDiskUser);
        strcpy(sParam.szNetDiskPwd, cModuleParams.cCamAppParam.szNetDiskPwd);
        strcpy(sParam.szNFS, cModuleParams.cCamAppParam.szNFS);
        sParam.fSaveRecord = cModuleParams.cCamAppParam.fIsSafeSaver;
        sParam.fSaveVideo = cModuleParams.cCamAppParam.fSaveVideo;
        sParam.iNetDiskSpace = cModuleParams.cCamAppParam.iNetDiskSpace;
        sParam.iFileSize = cModuleParams.cCamAppParam.iFileSize;
        strcpy(sParam.szNFSParam, cModuleParams.cCamAppParam.szNFSParam);
        (*ppcSafeSaver)->Init(&sParam);
    }
}

static void InitImgGatherer(
    IImgGatherer** ppVideoGetter,
    ModuleParams& cModuleParams,
    CAM_CFG_PARAM& cCamCfgParam,
    IMG_FRAME_PARAM& cImgFrameParam
)
{
    switch (cCamCfgParam.iCamType)
    {
    case 1: //测试专用协议
    {
        *ppVideoGetter = new CVideoGetter_Test();
        (*ppVideoGetter)->SetCamCfgParam(&cCamCfgParam);
        HV_Trace(5, "测试专用协议1\n");
        break;
    }
    case 2: //创宇协议
    {
        *ppVideoGetter = new CVideoGetter_Camyu();
        (*ppVideoGetter)->SetCamCfgParam(&cCamCfgParam);
        HV_Trace(5, "测试专用协议2\n");
        break;
    }
    case 3: //广播协议
    {
        *ppVideoGetter = new CVideoGetter_Broadcast();
        HV_Trace(5, "测试专用协议3\n");
        break;
    }
    case 4: //创宇抓拍协议
    {
        *ppVideoGetter = new CImgCapturer();
        strcpy(cImgFrameParam.rgstrCamIP[0], cCamCfgParam.szIP);
        (*ppVideoGetter)->SetImgFrameParam(cImgFrameParam);
        HV_Trace(5, "测试专用协议4\n");
        break;
    }
    case 0: //一体式相机协议
    default:
    {
        *ppVideoGetter = new CVideoGetter_VPIF();
        IMG_FRAME_PARAM cImgFrameParamTmp;
        cImgFrameParamTmp.fltCaptureFrameRate = cModuleParams.cCamAppParam.iENetSyn == 1 ? 12.5f : 15.0f;
        cImgFrameParamTmp.iOutputFrameRate = cModuleParams.cCamAppParam.iOutputFrameRate;
        (*ppVideoGetter)->SetImgFrameParam(cImgFrameParamTmp);
        ((CVideoGetter_VPIF*)(*ppVideoGetter))->SetImageEddyType((cModuleParams.cCamAppParam.fIsSideInstall) ? 1 : 0);

#ifdef _CAMERA_PIXEL_500W_
        HV_Trace(5, "一体式相机协议 - 500w\n");
#elif defined(_CAMERA_PIXEL_200W_25FPS_)
        HV_Trace(5, "一体式相机协议 - 200w 25fps\n");
#else
        HV_Trace(5, "一体式相机协议 - 200w 15fps\n");
#endif
    }
    }
}

static void InitCameraProtocol(
    CCameraImageLinkCtrl& imageLinkCtrl,
    CCameraVideoLinkCtrl& videoLinkCtrl,
    CCameraRecordLinkCtrl& recordLinkCtrl,
    ISafeSaver* pcSafeSaver,
    CPciDataRecvThreadMaster& cPciDataRecv
)
{
    if ( S_OK != imageLinkCtrl.Start(NULL) )
    {
        HV_Trace(5, "CCameraImageLinkCtrl Start failed!\n");
        HV_Exit(HEC_FAIL, "CCameraImageLinkCtrl Start failed!");
    }

    SEND_VIDEO_PARAM cSendVideoParamTmp;
    cSendVideoParamTmp.pcSafeSaver = pcSafeSaver;
    videoLinkCtrl.SetParam(&cSendVideoParamTmp);
    if ( S_OK != videoLinkCtrl.Start(NULL) )
    {
        HV_Trace(5, "CCameraVideoLinkCtrl Start failed!\n");
        HV_Exit(HEC_FAIL, "CCameraVideoLinkCtrl Start failed!");
    }

#ifndef _HV_CAMERA_PLATFORM_
    SEND_RECORD_PARAM cSendRecordParamTmp;
    cSendRecordParamTmp.pcSafeSaver = pcSafeSaver;
    recordLinkCtrl.SetParam(&cSendRecordParamTmp);
    if ( S_OK != recordLinkCtrl.Start(NULL) )
    {
        HV_Trace(5, "CCameraRecordLinkCtrl Start failed!\n");
        HV_Exit(HEC_FAIL, "CCameraRecordLinkCtrl Start failed!");
    }
    PCI_PARAM_MASTER cPciParam;
    cPciParam.pImageLink = &imageLinkCtrl;
    cPciParam.pRecordLink = &recordLinkCtrl;
    cPciParam.pSafeSaver = pcSafeSaver;
    cPciParam.pVideoLink = &videoLinkCtrl;
    cPciDataRecv.SetPciParam(cPciParam);

	//固态硬盘一直存储还是断开时才存储
    if (g_cModuleParams.cCamAppParam.iDiskType == 2)
    {
    	recordLinkCtrl.SetSaveSafeType(g_cModuleParams.cCamAppParam.iSaveSafeType);
    	videoLinkCtrl.SetSaveSafeType(g_cModuleParams.cCamAppParam.iSaveSafeType);
    }
#endif
}

// ---------------------------------------------------------------------

int main(int argc, char** argv)
{
    HV_Trace(5, "CamApp::main Begin...\n");

    //通过写文件通知守护进程当前进程依旧活着
    struct mytv
    {
        int iTick;
        int fMaster;
    }tmp;
    tmp.iTick = 0;
    tmp.fMaster = 1;
    WriteDataToFile("/.running", (unsigned char *)&tmp, sizeof(tmp));

    LinuxSystemInit();

#ifndef _HVCAM_PLATFORM_RTM_
    // 仅研发调试时使用。
    SetWorkLedOff();
    CUserExitThread cUserExitThread;
    cUserExitThread.Start(NULL);
    SetWorkLedOn();
#endif

    // 使能看门狗
    EnableWatchDog();

    CreateGlobalResource();

    WdtHandshake();

    InitModuleParam();

    WdtHandshake();

    CPciDataRecvThreadMaster cPciDataRecv;
    CAM_CFG_PARAM cCamCfgParam;
    IMG_FRAME_PARAM cImgFrameParam;
    AUTO_LINK_PARAM cAutoLinkParam;
    TRAFFIC_LIGHT_PARAM cTrafficLightParam;
    PANORAMIC_CAPTURER_PARAM cPanoramicCamaraParam;

#ifndef _HV_CAMERA_PLATFORM_
    InitPciLink(cPciDataRecv);
    GetCamCfgParam(cCamCfgParam);
    GetPanoramicCamaraParam(cPanoramicCamaraParam);
    //全景抓拍相机图片接收类
    IImgGatherer* pHvcPanormicCapturer = NULL;
    CAM_CFG_PARAM cCaptureParam;
    if(cPanoramicCamaraParam.nEnableFlag == 1)
    {
        sprintf(cCaptureParam.szIP, "%s", cPanoramicCamaraParam.szCamaraIP);
        cCaptureParam.iCamType = 4;
        //启动全景抓拍相机线程
        pHvcPanormicCapturer = new CVideoGetter_HvCamera();
        ((CVideoGetter_HvCamera*)pHvcPanormicCapturer)->SetCaptureMode(&cCaptureParam);
        pHvcPanormicCapturer->Play(NULL);
    }
#endif

    // 初始化共享内存，必须在PCI握手成功后立即执行
    InitMemBlocksEx(cCamCfgParam.iCamType);

#ifndef _HV_CAMERA_PLATFORM_
    SetSlaveCompressRate(g_cModuleParams);
    SendControllPannelParam(g_cModuleParams.cFrontPannelParam);

    GetAutoLinkParam(cAutoLinkParam);
    GetTrafficLightParam( cTrafficLightParam );

    WdtHandshake();
    //设置红灯加红
    SetCamEnhance(cTrafficLightParam);

    // 判断是否恢复出厂设置
    // 因一体机需要通过PCI通知从恢复出厂设置,故初始化PCI后才执行
    if (IsRecoverFactorySettings())
    {
        if (S_OK == PciSendParamSetData(NULL, 0))
        {
            RecoverFactorySettings();
            HV_Exit(HEC_SUCC|HEC_RESET_DEV, "RecoverFactorySettings!");
        }
        else
        {
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "RecoverFactorySettings have some wrong!");
        }
    }
#endif

    WdtHandshake();

    // 初始化DSPLink
    InitDspLink(argc, argv);

    WdtHandshake();

    CPingThread cPingThread;
    cPingThread.SetIPAddress(
        g_cModuleParams.cTcpipCfgParam.szIp,
        g_cModuleParams.cTcpipCfgParam.szNetmask,
        g_cModuleParams.cTcpipCfgParam.szGateway
    );
    cPingThread.Start(NULL);

    WdtHandshake();

    ISafeSaver* pcSafeSaver = NULL;
    InitStorage(&pcSafeSaver, g_cModuleParams);

    WdtHandshake();

    // 创建抓拍图像获取对象
    IImgGatherer* pVideoGetter = NULL;
    InitImgGatherer(&pVideoGetter, g_cModuleParams, cCamCfgParam, cImgFrameParam);

    // 创建命令控制协议对象以及结果发送对象
    ICameraCmdProcess* pICameraCmdProcess = NULL;
    if ( S_OK != CCameraCmdProcess::CreateICameraCmdProcess(&pICameraCmdProcess) )
    {
        HV_Trace(5, "CreateICameraCmdProcess is failed!\n");
        HV_Exit(HEC_FAIL, "CreateICameraCmdProcess is failed!");
    }
    CCameraCmdLinkCtrl cCameraCmdLinkCtrl(pICameraCmdProcess);
    if ( S_OK != cCameraCmdLinkCtrl.Start(NULL) )
    {
        HV_Trace(5, "CCameraCmdLinkCtrl Start failed!\n");
        HV_Exit(HEC_FAIL, "CCameraCmdLinkCtrl Start failed!");
    }

    CCameraImageLinkCtrl imageLinkCtrl;
    CCameraVideoLinkCtrl videoLinkCtrl;
    CCameraRecordLinkCtrl recordLinkCtrl;

    recordLinkCtrl.SetAutoLink(
        cAutoLinkParam.fAutoLinkEnable,
        cAutoLinkParam.szAutoLinkIP,
        cAutoLinkParam.iAutoLinkPort
    );

    InitCameraProtocol(
        imageLinkCtrl,
        videoLinkCtrl,
        recordLinkCtrl,
        pcSafeSaver,
        cPciDataRecv
    );

    CFrontController FrontController;
    if (g_cModuleParams.cFrontPannelParam.iUsedAutoControllMode == 1)
    {
        FrontController.OpenDevice();
        FrontController.Start(NULL);
    }
    // 创建相机数据流控制对象
    g_pCamApp = new CCamApp();
    g_pCamApp->SetCameraType(cCamCfgParam.iCamType);

    // 挂载功能组件
    g_pCamApp->MountVideoGetter(pVideoGetter);
    g_pCamApp->MountImageSender(&imageLinkCtrl);
    g_pCamApp->MountVideoSender(&videoLinkCtrl);

    // 相机操作接口
    g_cCameraController.MountCameraUtils((ICameraUtils*)g_pCamApp);

    // 字符叠加
    CCharacterOverlap cCharacterOverlap;
    g_cCameraController.MoutCharacterUtils((ICharacterOverlap*)&cCharacterOverlap);
    cCharacterOverlap.EnableCharacterOverlap(g_cModuleParams.cCamAppParam.iEnableCharacterOverlap);

    if (g_cModuleParams.cFrontPannelParam.iUsedAutoControllMode == 1)
    {
        g_pCamApp->MountFrontController(&FrontController);
    }
    else
    {
        g_pCamApp->MountFrontController(NULL);
    }
#ifndef _HV_CAMERA_PLATFORM_
    g_pCamApp->MountRecordSender(&recordLinkCtrl);
    g_pCamApp->MountSafeSaver(pcSafeSaver);
#endif

    WdtHandshake();

    // 运行各功能组件
    if ( S_OK != pVideoGetter->Play(NULL) )
    {
        HV_Trace(5, "VideoGetter play failed!\n");
        HV_Exit(HEC_FAIL, "VideoGetter play failed!");
    }

    g_pCamApp->Initialize();

    if ( S_OK != g_pCamApp->Start(NULL) )
    {
        HV_Trace(5, "g_pCamApp Start failed!\n");
        HV_Exit(HEC_FAIL, "g_pCamApp Start failed!");
    }

    WdtHandshake();

    // 启动设备搜索响应线程
    CIPSearchThread cDevSearch;
    if ( S_OK != cDevSearch.Create() )
    {
        HV_Trace(5, "IP Search Thread Failed!\n");
        HV_Exit(HEC_FAIL, "IP Search Thread Failed!");
    }

    WdtHandshake();

    if ( -1 == TelnetInit() )  // 初始化HvTelnet服务
    {
        HV_Trace(5, "TelnetInit is Failed!\n");
        HV_Exit(HEC_FAIL, "TelnetInit is Failed!");
    }

    WdtHandshake();
    // DataCtrl握手 注：需要在DataCtrl启动前
    if (SendDataCtrlHandshake() != S_OK)
    {
        HV_Trace(5, "Datactrl handshake failed!");
        HV_Exit(HEC_FAIL, "Datactrl handshake failed!");
    }


    HV_Trace(5, "Running...\n");

    DWORD dwOutputFlag = 0;
    int iErrCode = 0;

    while (true)
    {
        HV_Sleep(500);
        // 监控各模块运行状态
        if (!g_pCamApp->ThreadIsOk(&iErrCode))
        {
            if (2 == iErrCode)
            {
                HV_Trace(5, "IImgGatherer is Error!!!\n");
                HV_Exit(HEC_FAIL, "IImgGatherer is Error!!!");
            }
            else if (3 == iErrCode)
            {
                HV_Trace(5, "ISendCameraImage is Error!!!\n");
                HV_Exit(HEC_FAIL, "ISendCameraImage is Error!!!");
            }
            else if (4 == iErrCode)
            {
                HV_Trace(5, "ISendCameraVideo is Error!!!\n");
                HV_Exit(HEC_FAIL, "ISendCameraVideo is Error!!!");
            }
            else if (5 == iErrCode)
            {
                HV_Trace(5, "ISendRecord is Error!!!\n");
                HV_Exit(HEC_FAIL, "ISendRecord is Error!!!");
            }
            else
            {
                HV_Trace(5, "Unknown Error!!![iErrCode = %d]\n", iErrCode);
                char szInfoTmp[64];
                sprintf(szInfoTmp, "Unknown Error!!![iErrCode = %d]", iErrCode);
                HV_Exit(HEC_FAIL, szInfoTmp);
            }
        }

#ifndef _HV_CAMERA_PLATFORM_
        if ( S_OK != cPciDataRecv.GetCurStatus(NULL,0) )
        {
            HV_Trace(5, "Slave CPU status is abnormal!!!\n");
            HV_Exit(HEC_FAIL, "Slave CPU status is abnormal!!!");
        }
#endif

        WdtHandshake();
        WorkLedLight();  // 闪烁工作指示灯，表示一切正常。

        //通过写文件通知守护进程当前进程依旧活着
        struct mytv
        {
            int iTick;
            int fMaster;
        }tmp;
        tmp.iTick = GetSystemTick();
        tmp.fMaster = 1;
        WriteDataToFile("/.running", (unsigned char *)&tmp, sizeof(tmp));

        if (!g_fCopyrightValid)
        {
            cPciDataRecv.DisableRecordSend();
        }
        else
        {
            cPciDataRecv.EnableRecordSend();
        }

#ifndef _HVCAM_PLATFORM_RTM_
        static int s_iShowCpuNearFlag = 0;
        if (200 < s_iShowCpuNearFlag++)
        {
            s_iShowCpuNearFlag = 0;

            int iCpuNearTemp = 0;
            if (0 == GetCpuNearTemp(&iCpuNearTemp))
            {
                HV_Trace(5, "CPU temperature: [%d]\n", iCpuNearTemp);
            }
        }

        if (((dwOutputFlag++) & 3) == 0)
        {
            static int s_iMemStatusShowFlag = 0;
            if (10 < s_iMemStatusShowFlag++)
            {
                s_iMemStatusShowFlag = 0;
                ShareMemPoolStatusShow();
            }

            Trace(".");
        }
#endif

        /* Comment by Shaorg: 临时解决PCI由主至从的通道断开问题。
        static MASTER_CPU_STATUS cMasterCpuStatus = {0};
        static DWORD32 dwLastTick = GetSystemTick();
        DWORD32 dwCurTick = GetSystemTick();
        if (dwCurTick - dwLastTick >= CPU_THROB_PERIOD)
        {
            if (0 == PciSendMasterCpuState(&cMasterCpuStatus))
            {
                dwLastTick = dwCurTick;
                HV_Trace(5, "Send CPU throb is succ. dwCurTick = [%d].\n", dwCurTick);
            }
            else
            {
                HV_Exit(HEC_FAIL|HEC_RESET_DEV, "Send CPU throb is failed!");
            }
        }
        */
    }

    HV_Trace(5, "CCamApp::main EXIT!!![%d]\n", dwOutputFlag);

    // 资源释放
    DestroySemaphore(&g_hSemCameraDataCount);
    DestroySemaphore(&g_hSemEDMA);

    SAFE_DELETE(pVideoGetter);
    SAFE_DELETE(pICameraCmdProcess);
    SAFE_DELETE(pcSafeSaver);
    SAFE_DELETE(g_pCamApp);

    g_cHvDspLinkApi.Uninit();
#ifndef _HV_CAMERA_PLATFORM_
    g_cHvPciLinkApi.Uninit();
#endif

    HV_Exit(HEC_FAIL, "MainExit");
}

// ---------------------------------------------------------------------

int Com_Lock(int iTimeOutMs)
{
    int iRet = -1;

    if (g_pSerialBase == NULL)
    {
        g_pSerialBase = new CSerialBase;
        if (g_pSerialBase != NULL)
        {
            if (S_OK != g_pSerialBase->Open("/dev/ttyS0"))
            {
                delete g_pSerialBase;
                g_pSerialBase = NULL;
            }
        }
    }

    // 取不到锁时直接丢弃
    if (g_pSerialBase != NULL)
    {
        if (S_OK == g_pSerialBase->Lock(iTimeOutMs))
        {
            iRet = S_OK;
        }
    }

    return iRet;
}

int Com_UnLock()
{
    if (g_pSerialBase != NULL)
    {
        g_pSerialBase->UnLock();
    }
    return 0;
}
