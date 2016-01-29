// 该文件编码必须是WINDOWS-936格式
#include "hvthreadbase.h"
#include "LoadParam.h"
#include "hvtarget_ARM.h"
#include "IPSearchThread.h"
#include "HvUtilsClass.h"
#include "CameraCmdProcess.h"
#include "CameraCmdLink.h"
#include "CameraRecordLink.h"
#include "DataCtrl.h"
#include "DspLinkMemBlocks.h"
#include "HvParamIO.h"
#include "CamTransmit.h"

HV_SEM_HANDLE g_hSemCameraDataCount;
HV_SEM_HANDLE g_hSemSwDev;
HV_SEM_HANDLE g_hSemEDMA;
HV_SEM_HANDLE g_hSemEEPROM;

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

#ifdef PC_TEST

int main(int argc, char** argv)
{
    CUserExitThread e;
    e.Start(NULL);

    CSafeSaverDm6467Impl* pcSafeSaver = new CSafeSaverDm6467Impl();

    CHvBoxHolder cHvBoxHolder;
    cHvBoxHolder.Init(NULL, pcSafeSaver);

    HV_Sleep(200);
    HV_Trace(5, "Running...\n");

    while (true)
    {
        HV_Sleep(5000);
        cHvBoxHolder.ShowStatus();
        printf(".");
        fflush(stdout);
    }

    return 0;
}

#else

/* HvSystemInit.cpp */
extern void LinuxSystemInit(void);

// 注：仅支持单板模式
// 必须打开单板模式编译宏“SINGLE_BOARD_PLATFORM”才能编译通过!!!
int main(int argc, char** argv)
{
    HV_Trace(5, "Begin...\n");
    HV_Trace(5, "%s %s\n", __TIME__, __DATE__);

    LinuxSystemInit();

    CUserExitThread e;
    e.Start(NULL);

    // 使能看门狗。
    // 注：使能之前必须喂狗，防止遇到“打开看门狗的同时发生喂狗超时而导致设备复位”这样的情况。
    WdtHandshake();
    CPLD_Write(CPLD_WTD_ENABLE, 1);
    WdtHandshake();

    // 载入各模块参数
    ModuleParams cModuleParams;
    if ( S_OK != LoadModuleParam(cModuleParams) )
    {
        HV_Trace(5, "LoadModuleParam failed!\n");
        HV_Exit(HEC_FAIL|HEC_RESET_DEV, "LoadModuleParam failed!");
    }

    WdtHandshake();

    // 判断是否恢复出厂设置
    if (IsRecoverFactorySettings())
    {
        RecoverFactorySettings();
        HV_Exit(HEC_SUCC, "RecoverFactorySettings!");
    }

    WdtHandshake();

    // 初始化共享内存
    if (S_OK != InitDspLinkMemBlocks(LPRAPP_SINGLE_BOARD_PLATFORM))
    {
        HV_Trace(5, "Memory blocks initialize failed!\n");
        HV_Exit(HEC_RESET_DEV|HEC_FAIL, "Memory blocks initialize failed!");
    }

    WdtHandshake();

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

        // 设置设备IP
        SetTcpipAddr_2(&cModuleParams.cTcpipCfgParam_2);
    }
    else
    {
        // 进行测试相机口，只设置LAN1口为通信息端口
        fIsIP1Correct = TRUE;
        SetTcpipAddr_1(&cModuleParams.cTcpipCfgParam_2);
    }

    if (S_OK == StartCamTransmit(0,
                                 "",
                                 cModuleParams.cTcpipCfgParam_1,
                                 cModuleParams.cTcpipCfgParam_2))
    {
        HV_Trace(5, "StartCamTransmit...\n");
    }

    WdtHandshake();
    CSafeSaverDm6467Impl* pcSafeSaver = NULL;
    if (1 == cModuleParams.cResultSenderParam.fInitHdd)
    {
        SetAllLightOn();
        g_cHddOpThread.SetHddOpType(HOT_FULL_INIT);
        g_cHddOpThread.Start(NULL);
        HV_Trace(5, "Hdd init...\n");
    }
    if (cModuleParams.cResultSenderParam.fIsSafeSaver)
    {
        if (!cModuleParams.cResultSenderParam.fInitHdd)
        {
            SetAllLightOn();
            g_cHddOpThread.SetHddOpType(HOT_INIT);
            g_cHddOpThread.Start(NULL);
            HV_Trace(5, "Hdd check...\n");
        }
        WdtHandshake();
        while (1 != g_nHddCheckStatus)
        {
            HV_Trace(5, "等待硬盘初始化完成...\n");
            HV_Sleep(1000);
            WdtHandshake();
        }
        pcSafeSaver = new CSafeSaverDm6467Impl();
        SSafeSaveParam sParam;
        sParam.fCoverCtrl = true;
        sParam.fSaveRecord = true;
        pcSafeSaver->Init(&sParam);
    }

    SEND_RECORD_PARAM cParam;
    cParam.pcSafeSaver = pcSafeSaver;
    CCameraRecordLinkCtrl cCameraRecordLinkCtrl;
    cCameraRecordLinkCtrl.SetParam(&cParam);
    if ( S_OK != cCameraRecordLinkCtrl.Start(NULL) )
    {
        HV_Trace(5, "CCameraRecordLinkCtrl Start failed!\n");
        HV_Exit(HEC_FAIL, "CCameraRecordLinkCtrl Start failed!");
    }

    WdtHandshake();
    ICameraCmdProcess* pICameraCmdProcess = NULL;
    if ( S_OK != CCameraCmdProcess::CreateICameraCmdProcess(&pICameraCmdProcess) )
    {
        HV_Trace(5, "CreateICameraCmdProcess failed!\n");
        HV_Exit(HEC_FAIL, "CreateICameraCmdProcess failed!");
    }

    CCameraCmdLinkCtrl cCameraCmdLinkCtrl(pICameraCmdProcess);
    if ( S_OK != cCameraCmdLinkCtrl.Start(NULL) )
    {
        HV_Trace(5, "CCameraCmdLinkCtrl Start failed!\n");
        HV_Exit(HEC_FAIL, "CCameraCmdLinkCtrl Start failed!");
    }

    WdtHandshake();
    CHvBoxHolder cHvBoxHolder;
    cHvBoxHolder.Init(&cModuleParams, pcSafeSaver);

    HV_Sleep(300);

    // 启动设备搜索响应线程
    CIPSearchThread cDevSearch;
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

    WdtHandshake();

    /*
    SetBuzzerOn();
    HV_Sleep(200);
    SetBuzzerOff();
    */

    SetLan1LedOff();
    SetLan2LedOff();
    SetHddLedOff();
    SetStatLedOff();

    HV_Trace(5, "Running...\n");

    // 定时对网关进行ARPPing
    CPingThread cPingThread;
    cPingThread.SetIPAddress(cModuleParams.cTcpipCfgParam_1.szIp, cModuleParams.cTcpipCfgParam_1.szNetmask, cModuleParams.cTcpipCfgParam_1.szGateway);
    cPingThread.Start(NULL);

    WdtHandshake();

    DWORD32 dwIPTick = 0;
    DWORD32 dwBoxTick = 0;
    DWORD32 dwDotTick = 0;
    DWORD32 dwMemTick = GetSystemTick();
    while (1)
    {
        if (!fIsIP1Correct && (GetSystemTick() - dwIPTick >= 5000))
        {
            HV_Trace(5, "LAN1和LAN2的IP不能设置为同一网段，请更改！\n");
            dwIPTick = GetSystemTick();
        }

        HV_Sleep(500);

        if (GetSystemTick() - dwBoxTick >= 10000)
        {
            cHvBoxHolder.ShowStatus();
            dwBoxTick = GetSystemTick();
        }

        if (GetSystemTick() - dwMemTick >= 20000)
        {
            ShareMemPoolStatusShow();
            dwMemTick = GetSystemTick();
        }

        WdtHandshake();
        WorkLedLight();

        if (GetSystemTick() - dwDotTick >= 1000)
        {
            HV_Trace(5,".");
            fflush(stdout);
            dwDotTick = GetSystemTick();
        }
    }

    HV_Exit(HEC_FAIL, "MainExit");
}
#endif
