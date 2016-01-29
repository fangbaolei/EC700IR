// 该文件编码格式必须为WINDOWS-936格式

#include "DataCtrl.h"
#include "HvDspLinkApi.h"
#include "hvtarget_ARM.h"
#include "TcpipCfg.h"
#include "ControlFunc.h"
#include "HvPciLinkApi.h"
#include "PciResultSender.h"
#include "DspLinkMemBlocks.h"
#include "OuterControlImpl_Linux.h"
#include "HvUtilsClass.h"
#include "HvParamIO.h"
#include <signal.h>
#include <execinfo.h>

HV_SEM_HANDLE g_hSemEDMA;
#ifdef SINGLE_BOARD_PLATFORM
#include "HvCameraLink.h"
#include "CameraCmdProcess.h"
#include "CameraCmdLink.h"
#include "CameraImageLink.h"
#include "CameraVideoLink.h"
#include "CameraRecordLink.h"
#include "DM6467ResultSender.h"
#include "CamTransmit.h"

HV_SEM_HANDLE g_hSemCameraDataCount;
HV_SEM_HANDLE g_hSemSwDev;
HV_SEM_HANDLE g_hSemEEPROM;
bool g_fCopyrightValid = true;
/* HvUtilsClass.cpp */
extern int g_nHddCheckStatus;

void CreateGlobalResource()
{
    if (CreateSemaphore(&g_hSemCameraDataCount, 1, 1) != 0)
    {
        HV_Trace(5, "create g_hSemCameraDataCount failed!\n");
        HV_Exit(HEC_FAIL, "create g_hSemCameraDataCount failed!");
    }
    if (CreateSemaphore(&g_hSemSwDev, 1, 1) != 0)
    {
        HV_Trace(5, "create g_hSemSwDev failed!\n");
        HV_Exit(HEC_FAIL, "create g_hSemSwDev failed!");
    }
    if (CreateSemaphore(&g_hSemEDMA, 1, 1) != 0)
    {
        HV_Trace(5, "create g_hSemEDMA failed!\n");
        HV_Exit(HEC_FAIL, "create g_hSemEDMA failed!");
    }
    if (CreateSemaphore(&g_hSemEEPROM, 1, 1) != 0)
    {
        HV_Trace(5, "create g_hSemEEPROM failed!\n");
        HV_Exit(HEC_FAIL, "create g_hSemEEPROM failed!");
    }
}

IResultSender * InitDM6467Protocol(
    CCameraImageLinkCtrl& imageLinkCtrl,
    CCameraRecordLinkCtrl& recordLinkCtrl,
    CCameraVideoLinkCtrl & videoLinkCtrl,
    CSafeSaverDm6467Impl* pcSafeSaver,
    CDM6467ResultSender & cDM6467DataSender,
    ResultSenderParam& cResultSenderParam
)
{
    if ( S_OK != imageLinkCtrl.Start(NULL) )
    {
        HV_Trace(5, "CCameraImageLinkCtrl Start failed!\n");
        HV_Exit(HEC_FAIL, "CCameraImageLinkCtrl Start failed!");
    }
    cDM6467DataSender.SetImageSenderCtrl(&imageLinkCtrl);

    SEND_RECORD_PARAM cSendRecordParamTmp;
    cSendRecordParamTmp.pcSafeSaver = cResultSenderParam.fIsSafeSaver ? pcSafeSaver : NULL;
    cSendRecordParamTmp.iOutputOnlyPeccancy = cResultSenderParam.iOutputOnlyPeccancy;
    cSendRecordParamTmp.iSendRecordSpace = cResultSenderParam.iSendRecordSpace;
    recordLinkCtrl.SetParam(&cSendRecordParamTmp);
    if ( S_OK != recordLinkCtrl.Start(NULL) )
    {
        HV_Trace(5, "CCameraRecordLinkCtrl Start failed!\n");
        HV_Exit(HEC_FAIL, "CCameraRecordLinkCtrl Start failed!");
    }

    SEND_VIDEO_PARAM cSendVideoParamTmp;
    cSendVideoParamTmp.pcSafeSaver = pcSafeSaver;
    cSendVideoParamTmp.iSendHisVideoSpace = cResultSenderParam.iSendHisVideoSpace;
    videoLinkCtrl.SetParam(&cSendVideoParamTmp);
    if ( S_OK != videoLinkCtrl.Start(NULL) )
    {
        HV_Trace(5, "CCameraVideoLinkCtrl Start failed!\n");
        HV_Exit(HEC_FAIL, "CCameraVideoLinkCtrl Start failed!");
    }

    cDM6467DataSender.SetRecordSenderCtrl(&recordLinkCtrl);
    cDM6467DataSender.SetSafeSaver(pcSafeSaver);
    cDM6467DataSender.Init(&cResultSenderParam);
    if (pcSafeSaver)
    {
        pcSafeSaver->SetFreeSpacePerDisk(cResultSenderParam.iRecordFreeSpacePerDisk, cResultSenderParam.iVideoFreeSpacePerDisk);
    }
    return &cDM6467DataSender;
}
#endif

CEPolice *g_pEPolice = NULL;

using namespace HiVideo;

#ifdef _USE_SEED_DEBUG_
/* DSPLink.c */
extern int g_fDebugStart;
#endif

/* HvDspLinkApi.cpp */
extern HRESULT InitDspLink(int argc, char** argv);

void PrintfErrorInfo(SLAVE_CPU_STATUS *pSlaveCapStatus, char *pszErrInfo);

HRESULT InitMemBlocksEx(int iCamType, int iEddyType);

HRESULT SetDspLinkEddyType(int nEddyType);

HRESULT InitCaptureThread(
    CImgCapturer **ppImgCapturer,
    ICamTrigger **ppCamTrigger,
    COuterControlImpl *pOuterControl,
    ModuleParams *pModuleParams,
    int iCameraType
);

/* HvSystemInit.cpp */
extern void LinuxSystemInit(void);

int main(int argc, char** argv)
{
    HV_Trace(5, "Begin...\n");

    LinuxSystemInit();

    CUserExitThread e;
    e.Start(NULL);

#ifdef SINGLE_BOARD_PLATFORM
    CreateGlobalResource();
    EnableWatchDog();
#else
    if (CreateSemaphore(&g_hSemEDMA, 1, 1) != 0)
    {
        HV_Trace(5, "create g_hSemEDMA failed!\n");
        HV_Exit(HEC_FAIL, "create g_hSemEDMA failed!");
    }
#endif

    WdtHandshake();

    // 载入各模块参数
    ModuleParams cModuleParams;
    if ( S_OK != LoadModuleParam(cModuleParams) )
    {
        HV_Trace(5, "LoadModuleParam failed!\n");
        HV_Exit(HEC_FAIL|HEC_RESET_DEV, "LoadModuleParam failed!");
    }

    WdtHandshake();

#ifdef SINGLE_BOARD_PLATFORM
    // 判断是否恢复出厂设置
    if (IsRecoverFactorySettings())
    {
        RecoverFactorySettings();
        HV_Exit(HEC_SUCC, "RecoverFactorySettings!");
    }

    WdtHandshake();
#endif

    // 初始化共享内存
    if (S_OK != InitMemBlocksEx(cModuleParams.cCamCfgParam.iCamType,
                                cModuleParams.cTrackerCfgParam.iEddyType))
    {
        HV_Trace(5, "Memory blocks initialize failed!\n");
        HV_Exit(HEC_RESET_DEV|HEC_FAIL, "Memory blocks initialize failed!");
    }

    // 初始化DSPLink
    if ( S_OK != InitDspLink(argc, argv) )
    {
        HV_Trace(5, "InitDspLink is failed!\n");
        HV_Exit(HEC_RESET_DEV|HEC_FAIL, "InitDspLink is failed!");
    }

    // 设置DSP端图片旋转标识
    if (S_OK != SetDspLinkEddyType(cModuleParams.cTrackerCfgParam.iEddyType))
    {
        HV_Trace(5, "Set dsp eddy type failed!\n");
        HV_Exit(HEC_FAIL, "Set dsp eddy type failed!");
    }

    int nLastLightType = (int)LIGHT_TYPE_COUNT;
    int nLastPulseLevel = 0;
    int nLastCplStatus = 0;
    CCamLightTypeSaver cCamLightTypeSaver;
    cCamLightTypeSaver.GetLastLightType(nLastLightType , nLastPulseLevel , nLastCplStatus);
    if (cModuleParams.cTrackerCfgParam.nCtrlCpl == 0)
    {
        nLastCplStatus = -1;
    }
    else
    {
        if (nLastCplStatus == -1)
        {
            nLastCplStatus = 0;
        }
    }

    // 创建视频流识别对象
    CVideoRecoger* pVideoRecoger = new CVideoRecoger();

    // 初始化视频流识别对象
    if ( S_OK != pVideoRecoger->Init(cModuleParams.nWorkModeIndex,
                                     nLastLightType,
                                     nLastPulseLevel,
                                     nLastCplStatus,
                                     cModuleParams.cTrackerCfgParam) )
    {
#ifndef SINGLE_BOARD_PLATFORM
        g_cHvPciLinkApi.SendData(PCILINK_RESET_HV, NULL, 0);
#endif
        HV_Trace(5, "VideoRecoger init failed!\n");
        HV_Exit(HEC_FAIL, "VideoRecoger init failed!");
    }
    pVideoRecoger->SetEncodeType(1 + cModuleParams.cResultSenderParam.iOutputCaptureImage);

    // 结果发送对象
    IResultSender *pResultSender = NULL;
    // 视频流获取对象
    IImgGatherer* pVideoGetter = NULL;
    // 事件检测对象
    CEventChecker_EP *pEventChecker = NULL;
    // 创建信号匹配对象
    COuterControlImpl *pOuterControl = NULL;
    // 是否允许操作蜂鸣器
    BOOL fCanSetBuzzer = FALSE;

    // 初始化事件检测
    pEventChecker = new CEventChecker_EP();
    pEventChecker->SetResultParam(&cModuleParams.cResultSenderParam);
    pEventChecker->SetTrackerParam(&cModuleParams.cTrackerCfgParam);

    // 初始化信号匹配
    pOuterControl = new COuterControlImpl(cModuleParams.cSignalMatchParam);

#ifdef SINGLE_BOARD_PLATFORM

    WdtHandshake();

    // 设备搜索响应线程对象
    CIPSearchThread cDevSearch;
    // 网口1IP合法性标识
    BOOL fIsIP1Correct = FALSE;

    // 判断是否测试相机口（生产测试时使用）
    if (!IsTestCamLanPort())
    {
        // 正常工作，非测试，设置LAN1和LAN2口
        // 确保LAN1和LAN2不在同一网段内，如果在同一网段内，LAN1将会被强行关闭
        if (CheckIPAddr(cModuleParams.cTcpipCfgParam_1, cModuleParams.cTcpipCfgParam_2) == S_OK)
        {
            fIsIP1Correct = TRUE;
            SetTcpipAddr_1(&cModuleParams.cTcpipCfgParam_1);
        }
    }
    else
    {
        // 进行测试相机口，只设置LAN1口为通信息端口
        fIsIP1Correct = TRUE;
        SetTcpipAddr_1(&cModuleParams.cTcpipCfgParam_2);
    }

    SetAllLightOff();
    CSafeSaverDm6467Impl* pcSafeSaver = NULL;

    int iDiskCount = 0;
    if ( cModuleParams.cResultSenderParam.fIsSafeSaver )
    {
        ++iDiskCount;
    }
    if ( cModuleParams.cResultSenderParam.iSaveVideo )
    {
        ++iDiskCount;
    }

    if (0 < cModuleParams.cResultSenderParam.fInitHdd)
    {
        SetAllLightOn();
        g_cHddOpThread.SetHddOpType(cModuleParams.cResultSenderParam.fInitHdd == 1 ? HOT_FULL_INIT : HOT_CHECKFACTOR, iDiskCount);
        g_cHddOpThread.Start(NULL);
        HV_Trace(5, "Hdd init...\n");
    }
    if (cModuleParams.cResultSenderParam.fIsSafeSaver || cModuleParams.cResultSenderParam.iSaveVideo)
    {
        if (!cModuleParams.cResultSenderParam.fInitHdd)
        {
            SetAllLightOn();
            g_cHddOpThread.SetHddOpType(HOT_INIT, iDiskCount);
            g_cHddOpThread.Start(NULL);
            HV_Trace(5, "Hdd check...\n");
        }
        pcSafeSaver = new CSafeSaverDm6467Impl();
    }
    else if (cModuleParams.cResultSenderParam.fInitHdd <= 0)
    {
        fCanSetBuzzer = TRUE;
    }

    if (pcSafeSaver)
    {
        SSafeSaveParam sParam;
        sParam.fSaveRecord = cModuleParams.cResultSenderParam.fIsSafeSaver;
        sParam.fSaveVideo = cModuleParams.cResultSenderParam.iSaveVideo;
        pcSafeSaver->Init(&sParam);
    }

    CDM6467ResultSender  cResultSender;
    CCameraImageLinkCtrl imageLinkCtrl;
    CCameraRecordLinkCtrl recordLinkCtrl;

    AUTO_LINK_PARAM* pAutoLinkParam = &cModuleParams.cResultSenderParam.cAutoLinkParam;

    recordLinkCtrl.SetAutoLink(
        pAutoLinkParam->fAutoLinkEnable,
        pAutoLinkParam->szAutoLinkIP,
        pAutoLinkParam->iAutoLinkPort
    );

    recordLinkCtrl.EnableMTUSet(&cModuleParams.cResultSenderParam.fMTUSetEnable);

    CCameraVideoLinkCtrl  videoLinkCtrl;
    pResultSender = InitDM6467Protocol(
                        imageLinkCtrl,
                        recordLinkCtrl,
                        videoLinkCtrl,
                        pcSafeSaver,
                        cResultSender,
                        cModuleParams.cResultSenderParam
                    );

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

    WdtHandshake();

    switch (cModuleParams.cCamCfgParam.iCamType)
    {
    case 0://测试专用协议
    {
        pVideoGetter = new CVideoGetter_Test();
        HV_Trace(5, "测试专用协议\n");
        break;
    }
    case 1: //NVC协议
    {
        pVideoGetter = new CVideoGetter_Nvc();
        HV_Trace(5, "NVC协议\n");
        break;
    }
    case 3: //广播协议
    {
        pVideoGetter = new CVideoGetter_Broadcast();
        HV_Trace(5, "广播协议\n");
        break;
    }
    case 4: //HV摄像机协议
    {
        pVideoGetter = new CVideoGetter_HvCamera();
        HV_Trace(5, "HV摄像机协议\n");
        break;
    }
    case 2://创宇协议
    default:
    {
        pVideoGetter = new CVideoGetter_Camyu();
        HV_Trace(5, "创宇协议\n");
        break;
    }
    }

    pVideoGetter->MountTransmiter(&g_pCamTransmit);

    WdtHandshake();

    // 启动设备搜索响应线程
    if ( S_OK != cDevSearch.Create() )
    {
        HV_Trace(5, "IP search thread create failed!\n");
        HV_Exit(HEC_FAIL, "IP search thread create failed!");
    }

    // 初始化HvTelnet服务
    if ( -1 == TelnetInit() )
    {
        HV_Trace(5, "TelnetInit is Failed!\n");
        HV_Exit(HEC_FAIL, "TelnetInit is Failed!");
    }
#else
    // PCI接收线程对象
    CPciDataRecvThreadSlave cPciDataRecv;

    //根据相机类型初始化相关采集对象
    if (cModuleParams.cCamCfgParam.iCamType == 0)
    {
        pVideoGetter = new CVideoGetter_VPIF();
        ((CVideoGetter_VPIF*)pVideoGetter)->MountSignalMatch(pOuterControl);
    }
    else
    {
        pVideoGetter = new CVideoGetter_Pci();
    }

    pResultSender = new CPciResultSender();
    ((CPciResultSender*)pResultSender)->Init(&cModuleParams.cResultSenderParam, cModuleParams.cCamCfgParam.iCamType);

    WdtHandshake();

    HV_Trace(5, "start pci link.\n");

    PCI_PARAM_SLAVE cPciParam;
    cPciParam.pCfgCamParam = &cModuleParams.cCamCfgParam;
    cPciParam.pVideoGetter = pVideoGetter;
    cPciParam.pOuterControler = pOuterControl;
    cPciParam.pAutoLinkParam = &cModuleParams.cResultSenderParam.cAutoLinkParam;
    cPciParam.pTrafficCfgParam = &cModuleParams.cTrackerCfgParam.cTrafficLight;
    // 启动PCILink
    if (S_OK != StartPciLink(&cPciDataRecv, cPciParam))
    {
        HV_Trace(5, "CPciDataRecvThreadSlave Start is failed!\n");
        HV_Exit(HEC_FAIL, "CPciDataRecvThreadSlave Start is failed!");
    }

#endif
    // 传递相机参数
    pVideoGetter->SetCamCfgParam(&cModuleParams.cCamCfgParam);

    // 设置图片旋转标识
    pVideoGetter->SetImageEddyType(cModuleParams.cTrackerCfgParam.iEddyType);

    pEventChecker->SetResultSender(pResultSender);

    // 创建卡口数据流控制对象
    g_pEPolice = new CEPolice();
    g_pEPolice->SetModuleParams(cModuleParams);
    g_pEPolice->SetFlashLampDiff(cModuleParams.cSignalMatchParam.iFlashLampDiff);

    // 挂载功能组件
    g_pEPolice->MountVideoRecoger(pVideoRecoger);
    g_pEPolice->MountVideoGetter(pVideoGetter);
    g_pEPolice->MountResultSender(pResultSender);
    g_pEPolice->MountEventChecker(pEventChecker);
    g_pEPolice->MountSignalMatch(pOuterControl);
    g_pEPolice->MountLightTypeSaver(&cCamLightTypeSaver);
#ifdef SINGLE_BOARD_PLATFORM
    g_pEPolice->MountVideoSender(&videoLinkCtrl);
#else
    // DataCtrl握手 注：需要在DataCtrl启动前
    if (S_OK != DataCtrlHandshake(&cPciDataRecv))
    {
        HV_Trace(5, "DataCtrlHandshake failed!\n");
        HV_Exit(HEC_FAIL|HEC_RESET_DEV, "DataCtrlHandshake failed!");
    }
#endif

    WdtHandshake();

    // 运行各功能组件
    if (pOuterControl != NULL)
    {
        if (S_OK != pOuterControl->InitOuterControler())
        {
            HV_Trace(5, "SignalMatch init failed!\n");
            HV_Exit(HEC_FAIL, "SignalMatch init failed!");
        }
    }
    if (S_OK != pVideoGetter->Play(NULL))
    {
        HV_Trace(5, "VideoGetter play failed!\n");
        HV_Exit(HEC_FAIL, "VideoGetter play failed!");
    }

    // 注意：总控线程一定要最后再启动
    if (S_OK != g_pEPolice->Start(NULL))
    {
        HV_Trace(5, "DataCtrl start failed!\n");
        HV_Exit(HEC_FAIL, "DataCtrl start failed!");
    }

#ifdef SINGLE_BOARD_PLATFORM
    // 定时对网关进行ARPPing
    CPingThread cPingThread;
    cPingThread.SetIPAddress(cModuleParams.cTcpipCfgParam_1.szIp, cModuleParams.cTcpipCfgParam_1.szNetmask, cModuleParams.cTcpipCfgParam_1.szGateway);
    cPingThread.Start(NULL);

    // 启动相机数据转发线程
    if (S_OK == StartCamTransmit(
                cModuleParams.cCamCfgParam.iCamType,
                cModuleParams.cCamCfgParam.szIP,
                cModuleParams.cTcpipCfgParam_1,
                cModuleParams.cTcpipCfgParam_2)
       )
    {
        HV_Trace(5, "StartCamTransmit...\n");
    }
#endif

    WdtHandshake();

#if !defined(SINGLE_BOARD_PLATFORM) && defined(IPT_IN_MASTER)
    HRESULT hr = E_FAIL;
    if (cModuleParams.cSignalMatchParam.fEnableIPT)
    {
        HV_Trace(5, "start send pci data.\n");
        while (true)
        {
            HRESULT hrIpt = E_FAIL;
            int len = sizeof(hrIpt);
            HV_Trace(5, "start send pci data %d.\n", cModuleParams.cSignalMatchParam.nDeviceLen);
            hr = g_cHvPciLinkApi.SendData(
                     PCILINK_PCIIPTPARAM,
                     cModuleParams.cSignalMatchParam.bDeviceParam,
                     cModuleParams.cSignalMatchParam.nDeviceLen,
                     &hrIpt,
                     &len
                 );

            if ((hr != S_OK) || (hrIpt != S_OK))
            {
                HV_Trace(5, "init IPT failed, retry.\n");
                HV_Sleep(100);
            }
            else
            {
                break;
            }
        }
        HV_Trace(5, "init IPT ok.\n");
    }
#endif

#ifndef SINGLE_BOARD_PLATFORM
    //发送结果参数配置给主
    g_cHvPciLinkApi.SendData(PCILINK_RESULT_PARAM, &cModuleParams.cResultSenderParam, sizeof(cModuleParams.cResultSenderParam));
    //一体机直接外接雷达
    g_cHvPciLinkApi.SendData(PCILINK_OUTERDEVICE_TYPE, &cModuleParams.cSignalMatchParam.iRadarType, sizeof(int));
#endif

    WdtHandshake();
    if (fCanSetBuzzer)
    {
        SetBuzzerOn();
        HV_Sleep(200);
        SetBuzzerOff();
    }
    SetLan1LedOff();
    SetLan2LedOff();
    SetHddLedOff();
    SetStatLedOff();

    HV_Trace(5, "Running...\n");

    DWORD dwOutputFlag = 0;
    DWORD32 dwCurTick = 0;
    DWORD32 dwLastTick = GetSystemTick();
    SLAVE_CPU_STATUS cSlaveCpuStatus;

    while (true)
    {
#ifdef SINGLE_BOARD_PLATFORM
        if (!fIsIP1Correct && (GetSystemTick() - dwCurTick >= 5000))
        {
            HV_Trace(5, "LAN1和LAN2的IP不能设置为同一网段，请更改！\n");
            dwCurTick = GetSystemTick();
        }
#else
        dwCurTick = GetSystemTick();
        if (dwCurTick - g_pEPolice->GetLastResultTime() >= CPU_THROB_PERIOD)
        {
            GetSystemTime(&cSlaveCpuStatus.dwTimeLow, &cSlaveCpuStatus.dwTimeHigh);
            if (cSlaveCpuStatus.nStatusID == 0)
            {
                memset(cSlaveCpuStatus.szErrorText, 0, 32);
            }

            //向主CPU发送状态
            if ( 0 == PciSendSlaveCpuState(&cSlaveCpuStatus) )
            {
                //更新时标
                g_pEPolice->SetLastResultTime(dwCurTick);
            }
            else
            {
                HV_Exit(HEC_FAIL|HEC_RESET_DEV, "Send CPU throb is failed!");
            }
        }
#endif

        if (GetSystemTick() - dwLastTick > 10000)
        {
            pResultSender->SendDevStateString(
                pVideoGetter->IsCamConnected(),
                g_pEPolice->GetFrameRate(),
                g_cHddOpThread.GetCurStatusCodeString()
            );
            dwLastTick = GetSystemTick();
        }

        HV_Sleep(500);

        // 监控各模块运行状态
#ifdef SINGLE_BOARD_PLATFORM
        if ( S_OK != imageLinkCtrl.GetCurStatus(NULL, 0) )
        {
            PrintfErrorInfo(&cSlaveCpuStatus, "imageLinkCtrl is error!!!\n");
            HV_Exit(HEC_FAIL, "imageLinkCtrl is error!!!");
        }

        if (S_OK != recordLinkCtrl.GetCurStatus(NULL, 0) )
        {
            PrintfErrorInfo(&cSlaveCpuStatus, "recordLinkCtrl is error!!!\n");
            HV_Exit(HEC_FAIL, "recordLinkCtrl is error!!!");
        }

        if (S_OK != videoLinkCtrl.GetCurStatus(NULL, 0) )
        {
            PrintfErrorInfo(&cSlaveCpuStatus, "videoLinkCtrl is error!!!\n");
            HV_Exit(HEC_FAIL, "videoLinkCtrl is error!!!");
        }

        if (!g_fCopyrightValid)
        {
            cResultSender.DisableRecordSend();
        }
        else
        {
            cResultSender.EnableRecordSend();
        }

        if (!g_nHddCheckStatus)
        {
            SetAllLightFlash();
        }
        else if (g_nHddCheckStatus == 2)
        {
            SetAllLightOn();
        }
        else
        {
            SetAllLightOff();
        }
#endif
        int iErrCode=0;
        if ( !g_pEPolice->ThreadIsOk(&iErrCode) )
        {
            if (1 == iErrCode)
            {
                PrintfErrorInfo(&cSlaveCpuStatus, "VideoRecoger is error!!!\n");
                HV_Exit(HEC_FAIL, "VideoRecoger is error!!!");
            }
            else if (2 == iErrCode)
            {
                PrintfErrorInfo(&cSlaveCpuStatus, "VideoGetter is error!!!\n");
                HV_Exit(HEC_FAIL, "VideoGetter is error!!!");
            }
            else if (3 == iErrCode)
            {
                PrintfErrorInfo(&cSlaveCpuStatus, "SignalMatch is error!!!\n");
                HV_Exit(HEC_FAIL, "SignalMatch is error!!!");
            }
            else if (6 == iErrCode)
            {
                PrintfErrorInfo(&cSlaveCpuStatus, "Carleft thread is error!!!\n");
                HV_Exit(HEC_FAIL, "Carleft thread is error!!!");
            }
            continue;
        }

        if ( S_OK != pResultSender->GetCurStatus(NULL,0) )
        {
            PrintfErrorInfo(&cSlaveCpuStatus, "ResultSender is error!!!\n");
            HV_Exit(HEC_FAIL, "ResultSender is error!!!");
            continue;
        }
#ifndef SINGLE_BOARD_PLATFORM
        if ( S_OK != cPciDataRecv.GetCurStatus(NULL,0) )
        {
            HV_Exit(HEC_FAIL, "PciDataRecvThread is error!!!");
            continue;
        }
#endif
        WdtHandshake();
        WorkLedLight();
        //通过写文件通知守护进程当前进程依旧活着
        struct mytv
        {
            int iTick;
            int fMaster;
        }tmp;
        tmp.iTick = GetSystemTick();
#ifndef SINGLE_BOARD_PLATFORM
        tmp.fMaster = 0;
#else
        tmp.fMaster = 1;
#endif
        WriteDataToFile("/.running", (unsigned char *)&tmp, sizeof(tmp));

        if (((dwOutputFlag++) & 3) == 0)
        {
            static int s_nMemStatusShowFlag = 0;
            if ( s_nMemStatusShowFlag++ > 10 )
            {
                s_nMemStatusShowFlag = 0;
                ShareMemPoolStatusShow();
            }

            printf(".");
            // Linux标准输出是行缓存，故需要强制刷新
            fflush(stdout);
        }

        cSlaveCpuStatus.nStatusID = 0;
    }
    g_cHvDspLinkApi.Uninit();
#ifndef SINGLE_BOARD_PLATFORM
    g_cHvPciLinkApi.Uninit();
#endif

    HV_Exit(HEC_FAIL, "MainExit");
}

void PrintfErrorInfo(SLAVE_CPU_STATUS *pSlaveCapStatus, char *pszErrInfo)
{
    pSlaveCapStatus->nStatusID = -1;
    strcpy(pSlaveCapStatus->szErrorText, pszErrInfo);
    HV_Trace(5, pSlaveCapStatus->szErrorText);
}

HRESULT InitMemBlocksEx(int iCamType, int iEddyType)
{
    SHARE_MEM_MODE t;

#ifdef SINGLE_BOARD_PLATFORM
    t = LPRAPP_EPOLICE_S;
#else
    if ( 0 == iCamType ) // 一体化相机
    {
        if ( 0 == iEddyType )
        {
#ifndef _CAMERA_PIXEL_500W_
            t = LPRAPP_NORMAL;
#else
    t = LPRAPP_EPOLICE_500W;
#endif
        }
        else
        {
            t = LPRAPP_EDDY;
        }
    }
    else
    {
        t = LPRAPP_OTHER;
    }
#endif

    return InitDspLinkMemBlocks(t);
}

HRESULT SetDspLinkEddyType(int nEddyType)
{
    IMAGE_EDDYTYPE_PARAM cEddyType;
    cEddyType.nEddyType = nEddyType;
    return g_cHvDspLinkApi.SendImageEddyTypeCmd(&cEddyType);
}

HRESULT InitCaptureThread(
    CImgCapturer **ppImgCapturer,
    ICamTrigger **ppCamTrigger,
    COuterControlImpl *pOuterControl,
    ModuleParams *pModuleParams,
    int iCameraType
)
{
    static CAM_CFG_PARAM cCamTriggerParam;
    if (iCameraType != 4)
    {
        (*ppImgCapturer) = new CImgCapturer();
        (*ppImgCapturer)->SetCapCamParam(
            pModuleParams->cResultSenderParam.iEddyType,
            &pModuleParams->cResultSenderParam.cCapCamParam,
            pOuterControl
        );
        (*ppImgCapturer)->Play(NULL);
    }

    if (iCameraType == 4)
    {
        (*ppCamTrigger) = new CHVTrigger();
    }
    else
    {
        (*ppCamTrigger) = new CCamTrigger();
    }
    memcpy(&cCamTriggerParam, &(pModuleParams->cCamCfgParam), sizeof(pModuleParams->cCamCfgParam));
    strcpy(cCamTriggerParam.szIP, pModuleParams->cResultSenderParam.cCapCamParam.rgszRoadIP[0]);
    ((CCamTrigger*)(*ppCamTrigger))->SetCamTriggerParam(&cCamTriggerParam);
    ((CCamTrigger*)(*ppCamTrigger))->Start(NULL);

    return S_OK;
}
