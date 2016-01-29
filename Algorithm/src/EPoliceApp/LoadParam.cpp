// 该文件编码必须为WINDOWS-936格式
#include "LoadParam.h"
#include "HvParamIO.h"
#include "misc.h"
#include "platerecogparam.h"
#include "OuterControlImpl_Linux.h"
#include "hvtarget_ARM.h"
#include "ControlFunc.h"
#include <math.h>

//分型模式列表，名称要和platerecogparam.h中一致
char *g_pszPartWorkModeList[] =
{
    "电子警察"
};
#ifdef SINGLE_BOARD_PLATFORM
extern const char *PSZ_DSP_BUILD_NO;
extern const char *DSP_BUILD_DATE;
extern const char *DSP_BUILD_TIME;
#endif
CEprFileStorage g_cFileStorage(PARAM_START_ADDR, PARAM_SIZE);
CParamStore g_cParamStore;

int g_nSignal[100] = {0};

HRESULT LoadModuleParam(ModuleParams& cModuleParams)
{
    if (FAILED(g_cFileStorage.Initialize("")))
    {
        HV_Trace(5, "Open param file failed, can't save setting!!!\n");
    }
    g_cParamStore.Initialize(&g_cFileStorage, true);
#ifdef SINGLE_BOARD_PLATFORM
    char szTemp[255];
    strcpy(szTemp, g_pszPartWorkModeList[0]);
    g_cParamStore.GetString(
        "\\System\\Build", "WorkMode",
        szTemp, sizeof(szTemp),
        "当前工作模式", "", CUSTOM_LEVEL
    );
    g_cParamStore.SetString("\\System\\Build", "WorkMode", g_pszPartWorkModeList[0]);

    sprintf(szTemp, "%s %s %s", PSZ_DSP_BUILD_NO, DSP_BUILD_DATE, DSP_BUILD_TIME);
    g_cParamStore.GetString(
        "\\System\\Build", "No.",
        szTemp, sizeof(szTemp),
        "软件版本号", "", CUSTOM_LEVEL
    );
    sprintf(szTemp, "%s %s %s", PSZ_DSP_BUILD_NO, DSP_BUILD_DATE, DSP_BUILD_TIME);
    g_cParamStore.SetString("\\System\\Build", "No.", szTemp);

    strcpy(szTemp, DSP_BUILD_DATE);
    g_cParamStore.GetString(
        "\\System\\Build", "Date",
        szTemp, sizeof(szTemp),
        "软件版本日期", "", CUSTOM_LEVEL
    );
    g_cParamStore.SetString("\\System\\Build", "Date", DSP_BUILD_DATE);

    strcpy(szTemp, DSP_BUILD_TIME);
    g_cParamStore.GetString(
        "\\System\\Build", "Time",
        szTemp, sizeof(szTemp),
        "软件版本时间", "", CUSTOM_LEVEL
    );
    g_cParamStore.SetString("\\System\\Build", "Time", DSP_BUILD_TIME);

    // 读取UBOOT下的IP、掩码以及网关
    SetIpFromEEPROM();

#endif

    LoadTcpipParam_1(&g_cParamStore, cModuleParams.cTcpipCfgParam_1);
    LoadTcpipParam_2(&g_cParamStore, cModuleParams.cTcpipCfgParam_2);
    //工作模式一定要最先加载，否则后面的参数有可能加载错
    LoadWorkModeParam(&g_cParamStore, cModuleParams);
    LoadCamCfgParam(&g_cParamStore, cModuleParams.cCamCfgParam);
    LoadTrackerCfgParam(&g_cParamStore, cModuleParams.cTrackerCfgParam);
    LoadResultSenderParam(&g_cParamStore, cModuleParams.cResultSenderParam);
    LoadSignalMatchParam(&g_cParamStore, cModuleParams.cSignalMatchParam);
    LoadEventDetectCfgParam(&g_cParamStore, cModuleParams.cTrackerCfgParam, cModuleParams.cResultSenderParam);

    // 各模块之间参数传递
    cModuleParams.cResultSenderParam.iEddyType = cModuleParams.cTrackerCfgParam.iEddyType;
    cModuleParams.cTrackerCfgParam.iRoadNumberBegin = cModuleParams.cSignalMatchParam.iRoadNumberBegin;
    cModuleParams.cTrackerCfgParam.iStartRoadNum = cModuleParams.cSignalMatchParam.iStartRoadNum;
    cModuleParams.cSignalMatchParam.fEnableFlashLight = cModuleParams.cTrackerCfgParam.fEnableFlashLight;
    cModuleParams.cSignalMatchParam.fEPolice = TRUE;

    //视频检测需要校正坐标
    cModuleParams.cTrackerCfgParam.cVideoDet.iEPOutPutDetectLine = 100 - cModuleParams.cTrackerCfgParam.cVideoDet.iEPOutPutDetectLine;
    cModuleParams.cTrackerCfgParam.cVideoDet.iEPFGDetectLine -= cModuleParams.cTrackerCfgParam.cVideoDet.rcVideoDetArea.top;
    if(cModuleParams.cTrackerCfgParam.cVideoDet.iEPFGDetectLine < 0)
    {
        cModuleParams.cTrackerCfgParam.cVideoDet.iEPFGDetectLine = 0;
    }
    int width = cModuleParams.cTrackerCfgParam.cVideoDet.rcVideoDetArea.bottom - cModuleParams.cTrackerCfgParam.cVideoDet.rcVideoDetArea.top;
    if(width <= 0)
    {
        width = 100;
    }
    cModuleParams.cTrackerCfgParam.cVideoDet.iEPFGDetectLine *= 100.0f / width;
    g_cParamStore.Save(HvCore::PSM_SIMPLE);

    return S_OK;
}

typedef struct _HV_PARAM_TYPE_INT
{
    CHvString strSection;
    CHvString strKey;
    INT* piVal;
    INT iDefault;
    INT iMin;
    INT iMax;
    CHvString strChName;
    CHvString strComment;
    BYTE8 nRank;

} HV_PARAM_TYPE_INT, *PHV_PARAM_TYPE_INT;

//--------------------------------------------------------------------------------------

int LoadWorkModeParam(
    CParamStore* pcParamStore,
    ModuleParams& cModuleParams
)
{
    DWORD32 dwModeCount = 0;

    GetPlateRecogParamCount(&dwModeCount);

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker", "PlateRecogMode",
        &cModuleParams.nWorkModeIndex, cModuleParams.nWorkModeIndex,
        0, dwModeCount,
        "工作模式索引", "", INNER_LEVEL
    );

    char szModeName[128] = {0};
    //由于cModuleParams.nWorkModeIndex存放的是分型前的模式索引
    //因此需将该索引转换为分型后的索引
    GetRecogParamNameOnIndex(cModuleParams.nWorkModeIndex, szModeName);
    //检测是否是有效模式，如果不是则切换到有效模式

    if (GetRecogParamIndexOnNamePart(szModeName) == -1)
    {
        //恢复默认参数
        LoadDefaultParam(pcParamStore, cModuleParams.cTcpipCfgParam_1, cModuleParams.cTcpipCfgParam_2);
        //再重新设置工作模式
        GetRecogParamNameOnIndexPart(0, szModeName);
        cModuleParams.nWorkModeIndex = GetRecogParamIndexOnName(szModeName);
        pcParamStore->SetInt("\\Tracker", "PlateRecogMode", cModuleParams.nWorkModeIndex);
        pcParamStore->Save();
        HV_Trace(5, "%s\n", szModeName);
    }
    else
    {
        HV_Trace(5, "Load Mode OK...\n%s\n", szModeName);
    }
    return 0;
}

int LoadDefaultParam(
    CParamStore* pcParamStore,
    TcpipParam& cTcpipCfgParam1,
    TcpipParam& cTcpipCfgParam2
)
{
#ifdef SINGLE_BOARD_PLATFORM
    pcParamStore->GetString(
        "\\System\\CamLan", "CommIPAddr",
        cTcpipCfgParam1.szIp, 31
    );
    pcParamStore->GetString(
        "\\System\\CamLan", "CommMask",
        cTcpipCfgParam1.szNetmask, 31
    );
    pcParamStore->GetString(
        "\\System\\CamLan", "CommGateway",
        cTcpipCfgParam1.szGateway, 31
    );
    pcParamStore->GetString(
        "\\System\\TcpipCfg", "CommIPAddr",
        cTcpipCfgParam2.szIp, 31
    );
    pcParamStore->GetString(
        "\\System\\TcpipCfg", "CommMask",
        cTcpipCfgParam2.szNetmask, 31
    );
    pcParamStore->GetString(
        "\\System\\TcpipCfg", "CommGateway",
        cTcpipCfgParam2.szGateway, 31
    );
#endif

    pcParamStore->Clear();

#ifdef SINGLE_BOARD_PLATFORM
    LoadTcpipParam_1(pcParamStore, cTcpipCfgParam1);
    LoadTcpipParam_2(pcParamStore, cTcpipCfgParam2);
#endif

    pcParamStore->Save();

    HV_Trace(5, "Param Load Default...\n");
}

static ResultSenderParam g_cResultSenderParam;

static HV_PARAM_TYPE_INT g_rgcResultSenderInt[] =
{
#ifdef SINGLE_BOARD_PLATFORM
    {"\\HvDsp\\Misc", "SafeSaveEnable", (int*)&g_cResultSenderParam.fIsSafeSaver, g_cResultSenderParam.fIsSafeSaver, 0, 1, "是否启动安全存储", "0：不启动； 1：启动", CUSTOM_LEVEL},
#endif
    //{"\\HvDsp\\Misc", "DoReplace", (int*)&g_cResultSenderParam.cProcRule.fReplace, g_cResultSenderParam.cProcRule.fReplace, 0, 1, "是否替换", "默认值：0； 最小值：0； 最大值：1\n0：不替换； 1：替换", PROJECT_LEVEL},
    //{"\\HvDsp\\Misc", "DoLeach", (int*)&g_cResultSenderParam.cProcRule.fLeach, g_cResultSenderParam.cProcRule.fLeach, 0, 1, "是否过滤", "默认值：0； 最小值：0； 最大值：1\n0：不过滤； 1：过滤", PROJECT_LEVEL},
    //{"\\HvDsp\\Misc", "DoCompaty", (int*)&g_cResultSenderParam.cProcRule.fCompaty, g_cResultSenderParam.cProcRule.fCompaty, 0, 1, "是否启用通配符规则", "默认值：0； 最小值：0； 最大值：1\n0：不启用； 1：启用", CUSTOM_LEVEL},
    {"\\HvDsp\\Misc", "LoopReplaceEnable", (int*)&g_cResultSenderParam.cProcRule.fLoopReplace, g_cResultSenderParam.cProcRule.fLoopReplace, 0, 1, "循环替换", "0：不启用； 1：启用", CUSTOM_LEVEL},
};

static int g_iResultSenderIntArraySize = ARRSIZE(g_rgcResultSenderInt);

int LoadResultSenderParam(
    CParamStore* pcParamStore,
    ResultSenderParam& cResultSenderParam
)
{
    PHV_PARAM_TYPE_INT pParamPointInt = (PHV_PARAM_TYPE_INT)&g_rgcResultSenderInt;

    for ( int n = 0; n<g_iResultSenderIntArraySize; ++n )
    {
        HvParamReadIntWithWrite(
            pcParamStore,
            pParamPointInt[n].strSection.GetBuffer(),
            pParamPointInt[n].strKey.GetBuffer(),
            pParamPointInt[n].piVal, pParamPointInt[n].iDefault,
            pParamPointInt[n].iMin, pParamPointInt[n].iMax,
            pParamPointInt[n].strChName.GetBuffer(),
            pParamPointInt[n].strComment.GetBuffer(),
            pParamPointInt[n].nRank
        );
    }

    cResultSenderParam = g_cResultSenderParam;

    //attendtion by Shaorg: 以下字符型参数不能初始化为空或一个空格，否则将导致参数无法更改。
    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "OutputAppendInfo",
        &cResultSenderParam.fOutputAppendInfo, cResultSenderParam.fOutputAppendInfo,
        0, 1,
        "输出附加信息开关", "",
        PROJECT_LEVEL);

    /*HvParamReadIntWithWrite(
         pcParamStore, "\\HvDsp\\Misc", "RecordFreeSpace",
         &cResultSenderParam.iRecordFreeSpacePerDisk, cResultSenderParam.iRecordFreeSpacePerDisk,
         0, 100,
         "可靠性存储硬盘空间余量", "",
         PROJECT_LEVEL);

     HvParamReadIntWithWrite(
         pcParamStore, "\\HvDsp\\Misc", "VideoFreeSpace",
         &cResultSenderParam.iVideoFreeSpacePerDisk, cResultSenderParam.iVideoFreeSpacePerDisk,
         0, 100,
         "视频录像存储硬盘空间余量", "",
         PROJECT_LEVEL);*/

    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "OutputObservedFrames",
        &cResultSenderParam.fOutputObservedFrames, cResultSenderParam.fOutputObservedFrames,
        0, 1,
        "输出有效帧数开关", "",
        PROJECT_LEVEL);

    cResultSenderParam.fOutputCarArriveTime = 1;
    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "OutputCarArriveTime",
        &cResultSenderParam.fOutputCarArriveTime, cResultSenderParam.fOutputCarArriveTime,
        0, 1,
        "输出检测时间开关", "",
        PROJECT_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore,
        "HvDsp\\Misc", "OutputFilterInfo",
        &cResultSenderParam.fOutputFilterInfo, cResultSenderParam.fOutputFilterInfo,
        0, 1,
        "输出后处理信息", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "DrawRect",
        &cResultSenderParam.iDrawRect, cResultSenderParam.iDrawRect,
        0, 1,
        "发送图像画红框", "",
        PROJECT_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "VideoDisplayTime",
        &cResultSenderParam.iVideoDisplayTime, cResultSenderParam.iVideoDisplayTime,
        1, 1000000,
        "发送视频时间间隔(MS)", "",
        CUSTOM_LEVEL);
#ifdef SINGLE_BOARD_PLATFORM
    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "HDVideoEnable",
        &cResultSenderParam.iSaveVideo, cResultSenderParam.iSaveVideo,
        0, 1,
        "硬盘录像开关", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "InitHdd",
        &cResultSenderParam.fInitHdd,
        cResultSenderParam.fInitHdd, 0, 2,
        "初始化硬盘", "1-分区格式化;2-坏道检查;注：一次性有效",
        CUSTOM_LEVEL);
#endif
#ifndef SINGLE_BOARD_PLATFORM
    /*HvParamReadFloatWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "CarAGCLinkage",
        &cResultSenderParam.fltCarAGCLinkage, cResultSenderParam.fltCarAGCLinkage,
        0.0, 100,
        "车流量与AGC联动阈值", "0表示AGC不与车流量联动，非0表示5分钟内的车流量少于等于该值时启用AGC，大于时关闭AGC",
        CUSTOM_LEVEL);
    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "AWBLinkage",
        &cResultSenderParam.iAWBLinkage, cResultSenderParam.iAWBLinkage,
        0, 1,
        "环境亮度与AWB联动开关", "",
        CUSTOM_LEVEL);*/
#endif
    HvParamReadStringWithWrite(
        pcParamStore, "\\HvDsp\\FilterRule", "Compaty",
        cResultSenderParam.cProcRule.szCompatyRule,
        sizeof(cResultSenderParam.cProcRule.szCompatyRule),
        "通配符规则", "", CUSTOM_LEVEL);

    HvParamReadStringWithWrite(
        pcParamStore, "\\HvDsp\\FilterRule", "Replace",
        cResultSenderParam.cProcRule.szReplaceRule,
        sizeof(cResultSenderParam.cProcRule.szReplaceRule),
        "替换规则", "", CUSTOM_LEVEL);

    HvParamReadStringWithWrite(
        pcParamStore, "\\HvDsp\\FilterRule", "Leach",
        cResultSenderParam.cProcRule.szLeachRule,
        sizeof(cResultSenderParam.cProcRule.szLeachRule),
        "过滤规则", "", CUSTOM_LEVEL);

    HvParamReadStringWithWrite(
        pcParamStore, "\\HvDsp\\Identify", "StreetName",
        cResultSenderParam.szStreetName,
        sizeof(cResultSenderParam.szStreetName),
        "路口名称", "",
        CUSTOM_LEVEL);

    HvParamReadStringWithWrite(
        pcParamStore, "\\HvDsp\\Identify", "StreetDirection",
        cResultSenderParam.szStreetDirection,
        sizeof(cResultSenderParam.szStreetDirection),
        "路口方向", "",
        CUSTOM_LEVEL);
    //后续需要增加到一体机去
    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "OutputOnlyPeccancy",
        &cResultSenderParam.iOutputOnlyPeccancy, cResultSenderParam.iOutputOnlyPeccancy,
        0, 3,
        "只输出违章结果（1:只输出违章车辆 2:只输出非违章结果 3:接收历史数据时,非违章输出文本,违章输出文本和图片）", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "SendRecordSpace",
        &cResultSenderParam.iSendRecordSpace, cResultSenderParam.iSendRecordSpace,
        0, 100,
        "结果发送间隔(*100ms)", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "SendHisVideoSpace",
        &cResultSenderParam.iSendHisVideoSpace, cResultSenderParam.iSendHisVideoSpace,
        1, 20,
        "历史录像发送间隔(*100ms)", "",
        CUSTOM_LEVEL);

    /*HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\VideoProc", "EnableCrop",
        &cResultSenderParam.iSaveType, cResultSenderParam.iSaveType,
        0, 3,
        "结果保存方式", "",
        CUSTOM_LEVEL);
    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\VideoProc", "Width",
        &cResultSenderParam.iWidth, cResultSenderParam.iWidth,
        180, 8192,
        "截图宽度", "",
        CUSTOM_LEVEL);
    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\VideoProc", "Height",
        &cResultSenderParam.iHeight, cResultSenderParam.iHeight,
        144, 8192,
        "截图高度", "",
        CUSTOM_LEVEL);
    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\VideoProc\\MainVideo", "OutputBestSnap",
        &cResultSenderParam.iBestSnapshotOutput, cResultSenderParam.iBestSnapshotOutput,
        0, 1,
        "主视频最清晰大图输出", "",
        CUSTOM_LEVEL);
    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\VideoProc\\MainVideo", "OutputLastSnap",
        &cResultSenderParam.iLastSnapshotOutput, cResultSenderParam.iLastSnapshotOutput,
        0, 1,
        "主视频最后大图输出", "",
        CUSTOM_LEVEL);
    */
    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\VideoProc\\MainVideo", "OutputCaptureImage",
        &cResultSenderParam.iOutputCaptureImage, cResultSenderParam.iOutputCaptureImage,
        0, 1,
        "非违章车辆输出三张抓拍图", "",
        CUSTOM_LEVEL);

    cResultSenderParam.iFilterUnSurePeccancy = 1;
    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "FilterUnSurePeccancy",
        &cResultSenderParam.iFilterUnSurePeccancy, cResultSenderParam.iFilterUnSurePeccancy,
        0, 1,
        "过滤不合格的违章记录", "",
        PROJECT_LEVEL);
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "OutputPeccancyType",
        &cResultSenderParam.nOutputPeccancyType, cResultSenderParam.nOutputPeccancyType,
        0, 1,
        "输出违章类型方式", "0-按照优先级输出;1-输出所有违章类型", CUSTOM_LEVEL
    );
    HvParamReadStringWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "OutputPeccancyPriority",
        cResultSenderParam.szPeccancyPriority,
        sizeof(cResultSenderParam.szPeccancyPriority),
        "违章类型优先级", "违章优先级排序", CUSTOM_LEVEL
    );

    cResultSenderParam.nRedPosBrightness = 55;
    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "RedPosBrightness",
        &cResultSenderParam.nRedPosBrightness, cResultSenderParam.nRedPosBrightness,
        0, 255,
        "发送红灯位置环境亮度阈值", "",
        PROJECT_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "TraceRank",
        &g_nTraceRank,
        5, 1, 5,
        "调试信息输出控制（1:研发级,3:工程级,5:用户级）", "",
        PROJECT_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Misc", "AutoLinkEnable",
        &cResultSenderParam.cAutoLinkParam.fAutoLinkEnable, cResultSenderParam.cAutoLinkParam.fAutoLinkEnable,0,1,
        "主动连接使能", "", CUSTOM_LEVEL
    );

    HvParamReadStringWithWrite(
        pcParamStore,
        "\\HvDsp\\Misc", "AutoLinkIP",
        cResultSenderParam.cAutoLinkParam.szAutoLinkIP, sizeof(cResultSenderParam.cAutoLinkParam.szAutoLinkIP),
        "主动连接服务器IP", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Misc", "AutoLinkPort",
        &cResultSenderParam.cAutoLinkParam.iAutoLinkPort, 6665,
        0,10000,
        "主动连接服务器端口", "", CUSTOM_LEVEL
    );
#ifdef SINGLE_BOARD_PLATFORM

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Misc", "MTUSetEnable",
        &cResultSenderParam.fMTUSetEnable , cResultSenderParam.fMTUSetEnable,
        0,1,
        "MTU设置使能", "", CUSTOM_LEVEL
    );

#endif
    return 0;
}

//--------------------------------------------------------------------------------------

static SignalMatchParam g_cSignalMatchParam;

static HV_PARAM_TYPE_INT g_rgcSignalMatchInt[] =
{
    {"\\OuterCtrl", "PlateHoldTime", (int*)&g_cSignalMatchParam.dwPlateHoldTime, 2000, 0, 600000, "车牌保持时间", "", CUSTOM_LEVEL},
    {"\\OuterCtrl", "SignalHoldTime", (int*)&g_cSignalMatchParam.dwSignalHoldTime, 2000, 0, 600000, "信号保持时间", "", CUSTOM_LEVEL},
    {"\\OuterCtrl", "OutPutPlateMode", &g_cSignalMatchParam.emOutPutPlateMode, 0, 0, 1, "外总控使能开关", "0=自动;1=信号", CUSTOM_LEVEL},
#ifdef SINGLE_BOARD_PLATFORM
    {"\\OuterCtrl", "CapCommandTime", &g_cSignalMatchParam.iCapCommandTime, 300, 0, 1000, "抓拍图发命令的时间间隔", "", CUSTOM_LEVEL},
    {"\\OuterCtrl", "HardTriggerCap", (int*)&g_cSignalMatchParam.fHardTriggerCap, 0, 0, 1, "硬触发抓拍开关", "", CUSTOM_LEVEL},
#endif
    {"\\OuterCtrl", "ForcePlate", (int*)&g_cSignalMatchParam.fForcePlate, 0, 0, 1, "强制出图", "", PROJECT_LEVEL},
    {"\\OuterCtrl", "SignalTypeNum", &g_cSignalMatchParam.nSignalTypeNum, 0, 0, 10, "信号类型数量", "有效范围：0 - 10", CUSTOM_LEVEL},
    {"\\OuterCtrl", "SignalSourceNum", &g_cSignalMatchParam.nSignalSourceNum, 0, 0, 30, "信号源数量", "有效范围：0 - 30", CUSTOM_LEVEL},
};

static int g_iSignalMatchIntArraySize = ARRSIZE(g_rgcSignalMatchInt);

int LoadSignalMatchParam(
    CParamStore* pcParamStore,
    SignalMatchParam& cSignalMatchParam
)
{
    PHV_PARAM_TYPE_INT pParamPointInt = (PHV_PARAM_TYPE_INT)&g_rgcSignalMatchInt;

    for ( int n = 0; n<g_iSignalMatchIntArraySize; ++n )
    {
        HvParamReadIntWithWrite(
            pcParamStore,
            pParamPointInt[n].strSection.GetBuffer(),
            pParamPointInt[n].strKey.GetBuffer(),
            pParamPointInt[n].piVal, pParamPointInt[n].iDefault,
            pParamPointInt[n].iMin, pParamPointInt[n].iMax,
            pParamPointInt[n].strChName.GetBuffer(),
            pParamPointInt[n].strComment.GetBuffer(),
            pParamPointInt[n].nRank
        );
    }

    cSignalMatchParam = g_cSignalMatchParam;
    HvParamReadStringWithWrite(
        pcParamStore,
        "IPTCtrl", "IPTComStr",
        cSignalMatchParam.szComStr, sizeof(cSignalMatchParam.szComStr),
        "IPT串口", "", PROJECT_LEVEL
    );
    pcParamStore->GetBin("IPTCtrl", "DeviceParam", cSignalMatchParam.bDeviceParam, &cSignalMatchParam.nDeviceLen, "IPT内部参数", "", 3);

    char szSection[200];
    char szAreaNames[200];
    for (int i = 0; i < MAX_IPT_PORT_NUM; i++)
    {
        sprintf(szSection, "IPTCtrl\\Port%02d", i);
        cSignalMatchParam.rgIPTInfo[i].iComSource = -1;
        HvParamReadIntWithWrite(
            pcParamStore,
            szSection, "ComSignalSource",
            &cSignalMatchParam.rgIPTInfo[i].iComSource, cSignalMatchParam.rgIPTInfo[i].iComSource,
            -1, (g_cSignalMatchParam.nSignalSourceNum - 1),
            "信号源", "",
            CUSTOM_LEVEL
        );
        for (int j = 0; j < MAX_IPT_PIN_NUM; j++)
        {
            sprintf(szAreaNames, "SignalSource%01d", j);
            cSignalMatchParam.rgIPTInfo[i].rgSigSource[j] = -1;
            HvParamReadIntWithWrite(
                pcParamStore,
                szSection, szAreaNames,
                &cSignalMatchParam.rgIPTInfo[i].rgSigSource[j], cSignalMatchParam.rgIPTInfo[i].rgSigSource[j],
                -1, (g_cSignalMatchParam.nSignalSourceNum - 1),
                "信号源", "",
                CUSTOM_LEVEL
            );
        }
        cSignalMatchParam.rgIPTInfo[i].iOutSigSource = -1;
        HvParamReadIntWithWrite(
            pcParamStore,
            szSection, "OutSignalSource",
            &cSignalMatchParam.rgIPTInfo[i].iOutSigSource, cSignalMatchParam.rgIPTInfo[i].iOutSigSource,
            -1, (g_cSignalMatchParam.nSignalSourceNum - 1),
            "输出电平信号对应的信号源", "",
            CUSTOM_LEVEL
        );
        HvParamReadIntWithWrite(
            pcParamStore,
            szSection, "OutSignalLevel",
            &cSignalMatchParam.rgIPTInfo[i].iOutSigLevel, cSignalMatchParam.rgIPTInfo[i].iOutSigLevel,
            0, 1,
            "输出电平", "",
            CUSTOM_LEVEL
        );
        HvParamReadIntWithWrite(
            pcParamStore,
            szSection, "OutSignalWidth",
            &cSignalMatchParam.rgIPTInfo[i].iOutSigWidth, cSignalMatchParam.rgIPTInfo[i].iOutSigWidth,
            0, 5000,
            "输出脉宽", "",
            CUSTOM_LEVEL
        );
        cSignalMatchParam.rgIPTInfo[i].fUse =
            (cSignalMatchParam.rgIPTInfo[i].iComSource != -1
             || cSignalMatchParam.rgIPTInfo[i].rgSigSource[0] != -1
             || cSignalMatchParam.rgIPTInfo[i].rgSigSource[1] != -1
             || cSignalMatchParam.rgIPTInfo[i].iOutSigSource != -1) ? true : false;

    }
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\OuterCtrl", "MainSignalType",
        &cSignalMatchParam.iMainSignalType, -1,
        -1, (cSignalMatchParam.nSignalTypeNum - 1),
        "主信号编号", "",
        CUSTOM_LEVEL
    );
    /*
        HvParamReadIntWithWrite(
            pcParamStore,
            "\\OuterCtrl", "CapImgCount",
            &cSignalMatchParam.iCapImgCount,
            cSignalMatchParam.iCapImgCount, 1, 2,
            "抓拍图数量", "",
            CUSTOM_LEVEL
        );
    */
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\OuterCtrl\\OtherSignalSource", "SoftTrigger",
        &cSignalMatchParam.nSoftTrigSignalSource,
        cSignalMatchParam.nSoftTrigSignalSource, -1, 65535,
        "软触发信号源编号", "",
        CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\RoadInfo", "RoadNumberBegin",
        &cSignalMatchParam.iRoadNumberBegin,
        cSignalMatchParam.iRoadNumberBegin, 0, 1,
        "车道编号起始方向", "0:从左开始,1:从右开始",
        CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\RoadInfo", "StartRoadNumber",
        &cSignalMatchParam.iStartRoadNum,
        cSignalMatchParam.iStartRoadNum, 0, 9,
        "车道号起始编号", "0:从0开始,1:从1开始,以此类推...",
        CUSTOM_LEVEL
    );
    for (int i = 0; i < cSignalMatchParam.nSignalTypeNum; i++)
    {
        sprintf(szSection, "\\OuterCtrl\\SignalType%02d", i);
        HvParamReadIntWithWrite(
            pcParamStore,
            szSection, "AttachType",
            (int*)&cSignalMatchParam.rgMatchSigType[i].atType,
            ATTACH_INFO_NOTHING, 0, 255,
            "信号类型(0:触发, 1:速度, 2:抓拍)", "",
            CUSTOM_LEVEL
        );

        cSignalMatchParam.rgMatchSigType[i].fMainSignal \
        = (i == cSignalMatchParam.iMainSignalType ? true : false);

        if (cSignalMatchParam.rgMatchSigType[i].atType == 0
                && cSignalMatchParam.rgMatchSigType[i].fMainSignal
                && cSignalMatchParam.emOutPutPlateMode == PLATE_OUTPUT_SIGNAL)
        {
            cSignalMatchParam.fSignalIsMain = true;
        }

        HvParamReadStringWithWrite(
            pcParamStore,
            szSection, "Name",
            cSignalMatchParam.rgMatchSigType[i].strName, 64,
            "类型描述", "",
            CUSTOM_LEVEL
        );

        cSignalMatchParam.nMainSigTypeNum = 1;
    }

    //---

    for (int i = 0; i < cSignalMatchParam.nSignalSourceNum; i++)
    {
        sprintf(szSection, "\\OuterCtrl\\SignalSource%02d", i);
        HvParamReadIntWithWrite(
            pcParamStore,
            szSection, "MatchType",
            &cSignalMatchParam.rgSigSource[i].nType,
            cSignalMatchParam.rgSigSource[i].nType, 0, 255,
            "信号类型编号", "",
            CUSTOM_LEVEL
        );

        if ((cSignalMatchParam.rgSigSource[i].nType < 0)
                || (cSignalMatchParam.rgSigSource[i].nType >= cSignalMatchParam.nSignalTypeNum))
        {
            cSignalMatchParam.rgSigSource[i].nType = 0;
        }

        HvParamReadIntWithWrite(
            pcParamStore,
            szSection, "PrevTime",
            (int *)(&cSignalMatchParam.rgSigSource[i].dwPrevTime),
            cSignalMatchParam.rgSigSource[i].dwPrevTime, 0, 600000,
            "信号前时间", "",
            CUSTOM_LEVEL
        );

        HvParamReadIntWithWrite(
            pcParamStore,
            szSection, "PostTime",
            (int *)(&cSignalMatchParam.rgSigSource[i].dwPostTime),
            cSignalMatchParam.rgSigSource[i].dwPostTime, 0, 600000,
            "信号后时间", "",
            CUSTOM_LEVEL
        );

        HvParamReadIntWithWrite(
            pcParamStore,
            szSection, "Road",
            &cSignalMatchParam.rgSigSource[i].iRoad,
            cSignalMatchParam.rgSigSource[i].iRoad, 0, 0xff,
            "车道(0xff:全覆盖)", "",
            CUSTOM_LEVEL
        );
        if (cSignalMatchParam.rgMatchSigType[cSignalMatchParam.rgSigSource[i].nType].fMainSignal)
        {
            cSignalMatchParam.nMainSourceNum++;
        }
    }
#ifndef SINGLE_BOARD_PLATFORM
    HvParamReadIntWithWrite(
            pcParamStore,
            "\\OuterCtrl", "SpeedDecide",
            (int *)&cSignalMatchParam.fSpeedDecide,
            cSignalMatchParam.fSpeedDecide, 0, 1,
            "是否启用软件测速校正", "0-否;1-是",
            CUSTOM_LEVEL
        );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\OuterCtrl", "IPTEnable",
        &cSignalMatchParam.fEnableIPT,
        cSignalMatchParam.fEnableIPT, 0, 1,
        "IPT使能开关", "",
        CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\OuterCtrl", "RadarType",
        &cSignalMatchParam.iRadarType,
        cSignalMatchParam.iRadarType, 0, 1,
        "外接雷达类型", "0-不外接雷达,1-川速雷达",
        PROJECT_LEVEL
    );
#endif
    return 0;
}

//--------------------------------------------------------------------------------------

int LoadTcpipParam_1(
    CParamStore* pcParamStore,
    TcpipParam& cTcpipCfgParam
)
{
#ifdef SINGLE_BOARD_PLATFORM
    HvParamReadStringWithWrite(
        pcParamStore,
        "\\System\\CamLan", "CommIPAddr",
        cTcpipCfgParam.szIp, 31,
        "IP地址", "", CUSTOM_LEVEL
    );
    HvParamReadStringWithWrite(
        pcParamStore,
        "\\System\\CamLan", "CommMask",
        cTcpipCfgParam.szNetmask, 31,
        "子网掩码", "", CUSTOM_LEVEL
    );
    HvParamReadStringWithWrite(
        pcParamStore,
        "\\System\\CamLan", "CommGateway",
        cTcpipCfgParam.szGateway, 31,
        "网关", "", CUSTOM_LEVEL
    );
#endif
    return 0;
}

int LoadTcpipParam_2(
    CParamStore* pcParamStore,
    TcpipParam& cTcpipCfgParam
)
{
#ifdef SINGLE_BOARD_PLATFORM
    HvParamReadStringWithWrite(
        pcParamStore,
        "\\System\\TcpipCfg", "CommIPAddr",
        cTcpipCfgParam.szIp, 31,
        "IP地址", "", CUSTOM_LEVEL
    );
    HvParamReadStringWithWrite(
        pcParamStore,
        "\\System\\TcpipCfg", "CommMask",
        cTcpipCfgParam.szNetmask, 31,
        "子网掩码", "", CUSTOM_LEVEL
    );
    HvParamReadStringWithWrite(
        pcParamStore,
        "\\System\\TcpipCfg", "CommGateway",
        cTcpipCfgParam.szGateway, 31,
        "网关", "", CUSTOM_LEVEL
    );

    char szIp[32] = {0};
    char szMask[32] = {0};
    char szGateway[32] = {0};
    char szMac[32] = {0};
    char szSn[128] = {0};

    GetLocalTcpipAddr("eth0.20", szIp, szMask, szGateway, szMac);

    HvParamWriteString(
        pcParamStore,
        "\\System", "CommMac",
        szMac, 31,
        "MAC地址", "", CUSTOM_LEVEL
    );

    GetSN(szSn, sizeof(szSn));
    HvParamWriteString(
        pcParamStore,
        "\\System", "CommDevNo",
        szSn, 127,
        "设备编号", "", CUSTOM_LEVEL
    );
#endif
    return 0;
}
//--------------------------------------------------------------------------------------

int LoadEventDetectCfgParam(
    CParamStore* pcParamStore,
    TRACKER_CFG_PARAM& cTrackerCfgParam,
    ResultSenderParam& cResultSenderParam
)
{
    char szSection[64];
    char szChName[256];
    //默认一分钟,黄国超修改
    cResultSenderParam.iCheckEventTime = 1;
    HvParamReadIntWithWrite(
        pcParamStore, "HvDsp\\EventChecker", "CheckEventTime",
        &cResultSenderParam.iCheckEventTime, cResultSenderParam.iCheckEventTime,
        0, 5,
        "事件检测周期(单位:分钟)", "",
        PROJECT_LEVEL
    );
    //电子警察，默认逆行监测开关为开
    cTrackerCfgParam.nDetReverseRunEnable = 1;
    HvParamReadIntWithWrite(
        pcParamStore, "\\Tracker\\DetReverseRun", "DetReverseRunEnable",
        &cTrackerCfgParam.nDetReverseRunEnable, cTrackerCfgParam.nDetReverseRunEnable,
        0, 1,
        "逆行检测开关", "",
        PROJECT_LEVEL
    );

    if (cTrackerCfgParam.nDetReverseRunEnable)
    {
        HvParamReadIntWithWrite(
            pcParamStore, "\\Tracker\\DetReverseRun", "Span",
            &cTrackerCfgParam.nSpan, cTrackerCfgParam.nSpan,
            -100, 100,
            "过滤慢速逆行车辆跨度", "",
            CUSTOM_LEVEL
        );
    }

    HvParamReadFloatWithWrite(
        pcParamStore,
        "\\HvDsp\\EventChecker", "OverLineSensitivity",
        &cTrackerCfgParam.fltOverLineSensitivity, cTrackerCfgParam.fltOverLineSensitivity,
        0.5, 5.0,
        "压线检测灵敏度", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\EventChecker", "SpeedLimit",
        &cResultSenderParam.iSpeedLimit, cResultSenderParam.iSpeedLimit,
        0, 1000,
        "速度上限值(km/h)", "",
        CUSTOM_LEVEL
    );

    for (int i = 1; i < MAX_ROADLINE_NUM - 1; i++)
    {
        sprintf(szSection, "IsCrossLine%d%d", i, i + 1);
        sprintf(szChName, "车道线%d检测越线", i + 1);
        HvParamReadIntWithWrite(
            pcParamStore,
            "\\Tracker\\ActionDetect", szSection,
            &cTrackerCfgParam.cActionDetect.iIsCrossLine[i],
            cTrackerCfgParam.cActionDetect.iIsCrossLine[i],
            0, 1,
            szChName, "", CUSTOM_LEVEL
        );
    }

    for (int i = 0; i < MAX_ROADLINE_NUM; i++)
    {
        sprintf(szSection, "IsYellowLine%d%d", i, i + 1);
        sprintf(szChName, "车道线%d检测压线", i + 1);
        HvParamReadIntWithWrite(
            pcParamStore,
            "\\Tracker\\ActionDetect", szSection,
            &cTrackerCfgParam.cActionDetect.iIsYellowLine[i],
            cTrackerCfgParam.cActionDetect.iIsYellowLine[i],
            0, 2,
            szChName, "0:不判断压线,1:压实线,2:压黄线", CUSTOM_LEVEL
        );
    }
}

BOOL CheckArea(int nLeft, int nTop, int nRight, int nBottom)
{
    if (nLeft < 0 || nTop < 0 || nRight < 0 || nBottom < 0
            || nLeft > 100 || nTop > 100 || nRight > 100 || nBottom > 100
            || (nLeft >= nRight) || (nTop >= nBottom))
    {
        return FALSE;
    }
    return TRUE;
}

int LoadTrackerCfgParam(
    CParamStore* pcParamStore,
    TRACKER_CFG_PARAM& cTrackerCfgParam
)
{
//#ifdef SINGLE_BOARD_PLATFORM
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Misc", "EddyType",
        &cTrackerCfgParam.iEddyType, cTrackerCfgParam.iEddyType,
        0, 1,
        "图片旋转(0:不旋转,1:逆时针旋转90度)", "",
#ifdef SINGLE_BOARD_PLATFORM
        PROJECT_LEVEL
#else
        CUSTOM_LEVEL
#endif
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "HvDsp\\Misc", "OutputCarSize",
        &cTrackerCfgParam.fOutputCarSize, cTrackerCfgParam.fOutputCarSize,
        0, 1,
        "输出车辆尺寸", "", PROJECT_LEVEL
    );
//#endif
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\DoublePlate", "DoublePlateTypeEnable",
        &cTrackerCfgParam.nDoublePlateTypeEnable, cTrackerCfgParam.nDoublePlateTypeEnable,
        0, 3,
        "默认双层牌类型开关", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\DoublePlate", "DefaultType",
        &cTrackerCfgParam.nDefaultDBType, cTrackerCfgParam.nDefaultDBType,
        0, 1,
        "默认双层牌类型", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\DoublePlate", "MiddleWidth",
        &cTrackerCfgParam.nMiddleWidth, cTrackerCfgParam.nMiddleWidth,
        0, 300,
        "屏幕中间单层蓝牌后五字宽度", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\DoublePlate", "BottomWidth",
        &cTrackerCfgParam.nBottomWidth, cTrackerCfgParam.nBottomWidth,
        0, 300,
        "屏幕底部单层蓝牌后五字宽度", "", CUSTOM_LEVEL
    );

//==========ProcessPlate==============
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate", "EnableDBGreenSegment",
        &cTrackerCfgParam.nPlateDetect_Green, cTrackerCfgParam.nPlateDetect_Green,
        0, 1,
        "绿牌识别开关", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate", "LightBlueFlag",
        &cTrackerCfgParam.nProcessPlate_LightBlue, cTrackerCfgParam.nProcessPlate_LightBlue,
        0, 1,
        "浅蓝牌开关", "", PROJECT_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\DoublePlate", "DoublePlateEnable",
        &cTrackerCfgParam.fDoublePlateEnable, cTrackerCfgParam.fDoublePlateEnable,
        1, 3,
        "双层牌检测开关", "", PROJECT_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate", "EnableBigPlate",
        &cTrackerCfgParam.nEnableBigPlate, cTrackerCfgParam.nEnableBigPlate,
        0, 1,
        "大牌识别开关", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate", "EnablePlateEnhance",
        &cTrackerCfgParam.nEnablePlateEnhance, cTrackerCfgParam.nEnablePlateEnhance,
        0, 1,
        "分割前是否进行图片增强", "", PROJECT_LEVEL
    );

    cTrackerCfgParam.nEnableProcessBWPlate = 1;
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate", "EnableProcessBWPlate",
        &cTrackerCfgParam.nEnableProcessBWPlate, cTrackerCfgParam.nEnableProcessBWPlate,
        0, 1,
        "是否处理半黑半白牌", "", PROJECT_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate", "PlateResizeThreshold",
        &cTrackerCfgParam.nPlateResizeThreshold, cTrackerCfgParam.nPlateResizeThreshold,
        0, 200,
        "分割前小图拉伸宽度阈值", "", PROJECT_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate\\BlackPlate", "NightThreshold",
        &cTrackerCfgParam.nNightThreshold, cTrackerCfgParam.nNightThreshold,
        0, 240,
        "夜晚模式亮度上限", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate\\BlackPlate", "PlateLightCheckCount",
        &cTrackerCfgParam.nPlateLightCheckCount, cTrackerCfgParam.nPlateLightCheckCount,
        3, 50,
        "亮度调整间隔", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate\\BlackPlate", "MinPlateBrightness",
        &cTrackerCfgParam.nMinPlateBrightness, cTrackerCfgParam.nMinPlateBrightness,
        1, 255,
        "车牌最低亮度", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate\\BlackPlate", "MaxPlateBrightness",
        &cTrackerCfgParam.nMaxPlateBrightness, cTrackerCfgParam.nMaxPlateBrightness,
        1, 255,
        "车牌最高亮度", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate\\BlackPlate", "CtrlCpl",
        &cTrackerCfgParam.nCtrlCpl, cTrackerCfgParam.nCtrlCpl,
        0, 1,
        "控制偏光镜", "",
#ifndef SINGLE_BOARD_PLATFORM
        CUSTOM_LEVEL
#else
        PROJECT_LEVEL
#endif
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate\\BlackPlate", "LightTypeCpl",
        &cTrackerCfgParam.nLightTypeCpl, cTrackerCfgParam.nLightTypeCpl,
        0, 12,
        "控制偏光镜的亮度等级", "",
#ifndef SINGLE_BOARD_PLATFORM
        CUSTOM_LEVEL
#else
        PROJECT_LEVEL
#endif
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "HvDsp\\Misc", "ForceLightOffAtDay",
        &cTrackerCfgParam.iForceLightOffAtDay, cTrackerCfgParam.iForceLightOffAtDay,
        0, 1,
        "白天强制熄灭补光灯", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "HvDsp\\Misc", "ForceLightOffThreshold",
        &cTrackerCfgParam.iForceLightThreshold, cTrackerCfgParam.iForceLightThreshold,
        50, 200,
        "白天强制熄灭补光灯环境亮度阈值", "", PROJECT_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "HvDsp\\Misc", "DisableFlashAtDay",
        &cTrackerCfgParam.iDisableFlashAtDay, cTrackerCfgParam.iDisableFlashAtDay,
        0, 1,
        "白天强制熄灭闪光灯", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate\\BlackPlate", "H0",
        &cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H0, cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H0,
        0, 240,
        "蓝牌色度下限", "", PROJECT_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate\\BlackPlate", "H1",
        &cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H1, cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H1,
        cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H0, 240,
        "蓝牌色度上限", "", PROJECT_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate\\BlackPlate", "S",
        &cTrackerCfgParam.nProcessPlate_BlackPlate_S, cTrackerCfgParam.nProcessPlate_BlackPlate_S,
        0, 240,
        "黑牌饱和度上限", "", PROJECT_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate\\BlackPlate", "L",
        &cTrackerCfgParam.nProcessPlate_BlackPlate_L, cTrackerCfgParam.nProcessPlate_BlackPlate_L,
        0, 240,
        "黑牌亮度上限", "", PROJECT_LEVEL
    );

    //========End ProcessPlate==================

    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Recognition","UseEdgeMethod",
        &cTrackerCfgParam.fUseEdgeMethod, cTrackerCfgParam.fUseEdgeMethod,
        0, 1,
        "加强清晰图识别", "", PROJECT_LEVEL
    );
    cTrackerCfgParam.fEnableT1Model = 1;
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Recognition","EnableT1Model",
        &cTrackerCfgParam.fEnableT1Model, cTrackerCfgParam.fEnableT1Model,
        0, 1,
        "T-1模型开关", "", PROJECT_LEVEL
    );
    cTrackerCfgParam.fEnableDefaultWJ =1;
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Recognition\\DefaultChn", "EnableDefaultWJ",
        &cTrackerCfgParam.fEnableDefaultWJ, cTrackerCfgParam.fEnableDefaultWJ,
        0, 1,
        "本地新武警字符开关", "", CUSTOM_LEVEL
    );
    HvParamReadStringWithWrite(
        pcParamStore,
        "Tracker\\Recognition\\DefaultChn", "DefaultWJChar",
        cTrackerCfgParam.szDefaultWJChar, sizeof(cTrackerCfgParam.szDefaultWJChar) - 1,
        "本地新武警字符", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Misc", "EnableAlphaRecog",
        &cTrackerCfgParam.fEnableAlpha_5, cTrackerCfgParam.fEnableAlpha_5,
        0, 1,
        "黄牌字母识别开关", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Recognition", "RecogAsteriskThreshold",
        &cTrackerCfgParam.nRecogAsteriskThreshold, cTrackerCfgParam.nRecogAsteriskThreshold,
        0, 100,
        "得分低于阈值(百分比)用#表示", "", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Misc", "RecogGXPolice",
        &cTrackerCfgParam.nRecogGxPolice, cTrackerCfgParam.nRecogGxPolice,
        0, 1,
        "地方警牌识别开关", "", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Misc", "CarArrivedPos",
        &cTrackerCfgParam.nCarArrivedPos, cTrackerCfgParam.nCarArrivedPos,
        0, 100,
        "车辆到达位置(除黄牌外所有牌)", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Misc", "CarArrivedPosNoPlate",
        &cTrackerCfgParam.nCarArrivedPosNoPlate, cTrackerCfgParam.nCarArrivedPosNoPlate,
        0, 100,
        "无牌车到达位置", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Misc", "CarArrivedPosYellow",
        &cTrackerCfgParam.nCarArrivedPosYellow, cTrackerCfgParam.nCarArrivedPosYellow,
        0, 100,
        "车辆到达位置(黄牌)", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Misc", "CoreProcessType",
        &cTrackerCfgParam.nProcessType, cTrackerCfgParam.nProcessType,
        0, 2,
        "处理类型 0:只区分大中小型车 1:隧道内区分非机动和行人 2:隧道外区分非机动车和行人", "", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Misc", "CoreOutPutType",
        &cTrackerCfgParam.nOutPutType, cTrackerCfgParam.nOutPutType,
        0, 1,
        "输出类型 0:全类型输出 1:大中小型车输出为机动机", "", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Misc", "NightPlus",
        &cTrackerCfgParam.nNightPlus, cTrackerCfgParam.nNightPlus,
        0, 1,
        "晚上加强非机动车检测,只有场景足够亮时可用,否则会增加多检", "", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Misc", "WalkManSpeed",
        &cTrackerCfgParam.nWalkManSpeed, cTrackerCfgParam.nWalkManSpeed,
        0, 300,
        "行人判断的灵敏度", "", PROJECT_LEVEL
    );
    HvParamReadFloatWithWrite(
        pcParamStore,
        "Tracker\\Misc", "BikeSensitivity",
        &cTrackerCfgParam.fltBikeSensitivity, cTrackerCfgParam.fltBikeSensitivity,
        0.0f, 10.0f,
        "非机动车灵敏度", "", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Misc", "CarArrivedDelay",
        &cTrackerCfgParam.nCarArrivedDelay, cTrackerCfgParam.nCarArrivedDelay,
        0, 100,
        "车辆到达触发延迟距离(米)", "", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Misc", "PreVote",
        &cTrackerCfgParam.fPreVote, cTrackerCfgParam.fPreVote,
        0, 1,
        "是否启用预投票", "0-不使用;1-使用", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrackInfo", "VoteFrameNum",
        &cTrackerCfgParam.nVoteFrameNum, cTrackerCfgParam.nVoteFrameNum,
        0, 1000,
        "投票的结果数", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrackInfo", "MaxEqualFrameNumForVote",
        &cTrackerCfgParam.nMaxEqualFrameNumForVote, cTrackerCfgParam.nMaxEqualFrameNumForVote,
        0, 1000,
        "连续相同结果出牌条件", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrackInfo", "BlockTwinsTimeout",
        &cTrackerCfgParam.nBlockTwinsTimeout, cTrackerCfgParam.nBlockTwinsTimeout,
        0, 3600,
        "相同结果最小时间间隔(S)", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrackInfo", "AverageConfidenceQuan",
        &cTrackerCfgParam.nAverageConfidenceQuan, cTrackerCfgParam.nAverageConfidenceQuan,
        0, 65536,
        "平均得分下限", "", 1
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrackInfo", "FirstConfidenceQuan",
        &cTrackerCfgParam.nFirstConfidenceQuan, cTrackerCfgParam.nFirstConfidenceQuan,
        0, 65536,
        "汉字得分下限", "", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrackInfo", "RemoveLowConfForVote",
        &cTrackerCfgParam.nRemoveLowConfForVote, cTrackerCfgParam.nRemoveLowConfForVote,
        0, 100,
        "投票前去掉低得分车牌百分比", "", PROJECT_LEVEL
    );

    //先初始化扫描区域，每种模式都有自己的扫描区域
    cTrackerCfgParam.cDetectArea.DetectorAreaLeft = 2;
    cTrackerCfgParam.cDetectArea.DetectorAreaTop = 60;
    cTrackerCfgParam.cDetectArea.DetectorAreaRight = 98;
    cTrackerCfgParam.cDetectArea.DetectorAreaBottom = 100;
    cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum = 6;
    cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum = 10;
    //初始化梯形区域
    cTrackerCfgParam.cTrapArea.TopLeftX = cTrackerCfgParam.cDetectArea.DetectorAreaLeft;
    cTrackerCfgParam.cTrapArea.TopRightX = cTrackerCfgParam.cDetectArea.DetectorAreaRight;
    cTrackerCfgParam.cTrapArea.BottomLeftX = cTrackerCfgParam.cDetectArea.DetectorAreaLeft;
    cTrackerCfgParam.cTrapArea.BottomRightX = cTrackerCfgParam.cDetectArea.DetectorAreaRight;

    cTrackerCfgParam.cTrapArea.TopLeftY = cTrackerCfgParam.cDetectArea.DetectorAreaTop;
    cTrackerCfgParam.cTrapArea.TopRightY = cTrackerCfgParam.cDetectArea.DetectorAreaTop;
    cTrackerCfgParam.cTrapArea.BottomLeftY = cTrackerCfgParam.cDetectArea.DetectorAreaBottom;
    cTrackerCfgParam.cTrapArea.BottomRightY = cTrackerCfgParam.cDetectArea.DetectorAreaBottom;

    cTrackerCfgParam.cTrapArea.nDetectorMinScaleNum = cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum;
    cTrackerCfgParam.cTrapArea.nDetectorMaxScaleNum = cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum;

    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\DetAreaCtrl\\Normal","EnableTrap",
        &cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl, cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl,
        0,1,
        "梯形扫描区域控制开关","",5
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\DetAreaCtrl\\Normal","TopLeftX",
        &cTrackerCfgParam.cTrapArea.TopLeftX, cTrackerCfgParam.cTrapArea.TopLeftX,
        0,100,
        "扫描左上区域坐标X","",5
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\DetAreaCtrl\\Normal","TopLeftY",
        &cTrackerCfgParam.cTrapArea.TopLeftY, cTrackerCfgParam.cTrapArea.TopLeftY,
        0,100,
        "扫描左上区域坐标Y","",5
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\DetAreaCtrl\\Normal","TopRightX",
        &cTrackerCfgParam.cTrapArea.TopRightX, cTrackerCfgParam.cTrapArea.TopRightX,
        0,100,
        "扫描右上区域坐标X","",5
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\DetAreaCtrl\\Normal","TopRightY",
        &cTrackerCfgParam.cTrapArea.TopRightY, cTrackerCfgParam.cTrapArea.TopRightY,
        0,100,
        "扫描右上区域坐标Y","",5
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\DetAreaCtrl\\Normal","BottomLeftX",
        &cTrackerCfgParam.cTrapArea.BottomLeftX, cTrackerCfgParam.cTrapArea.BottomLeftX,
        0,100,
        "扫描左下区域坐标X","",5
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\DetAreaCtrl\\Normal","BottomLeftY",
        &cTrackerCfgParam.cTrapArea.BottomLeftY, cTrackerCfgParam.cTrapArea.BottomLeftY,
        0,100,
        "扫描左下区域坐标Y","",5
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\DetAreaCtrl\\Normal","BottomRightX",
        &cTrackerCfgParam.cTrapArea.BottomRightX, cTrackerCfgParam.cTrapArea.BottomRightX,
        0,100,
        "扫描右下区域坐标X","",5
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\DetAreaCtrl\\Normal","BottomRightY",
        &cTrackerCfgParam.cTrapArea.BottomRightY, cTrackerCfgParam.cTrapArea.BottomRightY,
        0,100,
        "扫描右下区域坐标Y","",5
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\DetAreaCtrl\\Normal", "MinScale",
        &cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum,
        cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum,
        0, 16,
        "检测的最小宽度", "56*(1.1^MinScale)", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\DetAreaCtrl\\Normal", "MaxScale",
        &cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum,
        cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum,
        0, 16,
        "检测的最大宽度", "56*(1.1^MaxScale)", CUSTOM_LEVEL
    );
    cTrackerCfgParam.cDetectArea.fEnableDetAreaCtrl = cTrackerCfgParam.cTrapArea.fEnableDetAreaCtrl;
    cTrackerCfgParam.cDetectArea.DetectorAreaLeft = cTrackerCfgParam.cTrapArea.TopLeftX > cTrackerCfgParam.cTrapArea.BottomLeftX?cTrackerCfgParam.cTrapArea.BottomLeftX:cTrackerCfgParam.cTrapArea.TopLeftX;
    cTrackerCfgParam.cDetectArea.DetectorAreaRight = cTrackerCfgParam.cTrapArea.TopRightX < cTrackerCfgParam.cTrapArea.BottomRightX?cTrackerCfgParam.cTrapArea.BottomRightX:cTrackerCfgParam.cTrapArea.TopRightX;
    cTrackerCfgParam.cDetectArea.DetectorAreaTop = cTrackerCfgParam.cTrapArea.TopLeftY > cTrackerCfgParam.cTrapArea.TopRightY?cTrackerCfgParam.cTrapArea.TopRightY:cTrackerCfgParam.cTrapArea.TopLeftY;
    cTrackerCfgParam.cDetectArea.DetectorAreaBottom = cTrackerCfgParam.cTrapArea.BottomLeftY > cTrackerCfgParam.cTrapArea.BottomRightY?cTrackerCfgParam.cTrapArea.BottomLeftY:cTrackerCfgParam.cTrapArea.BottomRightY;

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\RoadInfo", "RoadLineNumber",
        &cTrackerCfgParam.nRoadLineNumber,
        cTrackerCfgParam.nRoadLineNumber, 2, MAX_ROADLINE_NUM,
        "车道线数量", "",
        CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\RoadInfo", "OutputRoadTypeNameFlag",
        &cTrackerCfgParam.nOutputRoadTypeName,
        cTrackerCfgParam.nOutputRoadTypeName, 0, 1,
        "输出车道类型标识", "0-不输出;1-输出",
        CUSTOM_LEVEL
    );

    for (int i = 0; i < MAX_ROADLINE_NUM; i++)
    {
        char szSection[256];
        sprintf(szSection, "\\Tracker\\RoadInfo\\Road%02d", i);

        HvParamReadIntWithWrite(
            pcParamStore,
            szSection, "TopX",
            &cTrackerCfgParam.rgcRoadInfo[i].ptTop.x,
            cTrackerCfgParam.rgcRoadInfo[i].ptTop.x, 0, 5000,
            "顶X坐标", "",
            CUSTOM_LEVEL
        );
        HvParamReadIntWithWrite(
            pcParamStore,
            szSection, "TopY",
            &cTrackerCfgParam.rgcRoadInfo[i].ptTop.y,
            cTrackerCfgParam.rgcRoadInfo[i].ptTop.y, 0, 5000,
            "顶Y坐标", "",
            CUSTOM_LEVEL
        );
        HvParamReadIntWithWrite(
            pcParamStore,
            szSection, "BottomX",
            &cTrackerCfgParam.rgcRoadInfo[i].ptBottom.x,
            cTrackerCfgParam.rgcRoadInfo[i].ptBottom.x, 0, 5000,
            "底X坐标", "",
            CUSTOM_LEVEL
        );
        HvParamReadIntWithWrite(
            pcParamStore,
            szSection, "BottomY",
            &cTrackerCfgParam.rgcRoadInfo[i].ptBottom.y,
            cTrackerCfgParam.rgcRoadInfo[i].ptBottom.y, 0, 5000,
            "底Y坐标", "",
            CUSTOM_LEVEL
        );
        HvParamReadIntWithWrite(
            pcParamStore,
            szSection, "LineType",
            &cTrackerCfgParam.rgcRoadInfo[i].iLineType,
            cTrackerCfgParam.rgcRoadInfo[i].iLineType, 1, 9999,
            "类型", "",
            CUSTOM_LEVEL
        );
        HvParamReadIntWithWrite(
            pcParamStore,
            szSection, "RoadType",
            &cTrackerCfgParam.rgcRoadInfo[i].iRoadType,
            cTrackerCfgParam.rgcRoadInfo[i].iRoadType, 1, 9999,
            "对应的车道类型", "",
            CUSTOM_LEVEL
        );
        HvParamReadStringWithWrite(
            pcParamStore,
            szSection, "RoadTypeName",
            cTrackerCfgParam.rgcRoadInfo[i].szRoadTypeName,
            sizeof(cTrackerCfgParam.rgcRoadInfo[i].szRoadTypeName),
            "车道类型名称", "若为NULL，则取该车道类型的中文名",
            CUSTOM_LEVEL);

        HvParamReadIntWithWrite(
            pcParamStore,
            szSection, "FilterRunType",
            &cTrackerCfgParam.rgcRoadInfo[i].iFilterRunType,
            cTrackerCfgParam.rgcRoadInfo[i].iFilterRunType, 0, 9999,
            "过滤类型(1:直行2:左转4:右转8:调头32:逆行)", "",
            CUSTOM_LEVEL
        );
    }

    cTrackerCfgParam.nCaptureOnePos = 80;
    cTrackerCfgParam.nCaptureTwoPos = 60;
    cTrackerCfgParam.nCaptureThreePos = 45;
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "OnePos",
        &cTrackerCfgParam.nCaptureOnePos,
        cTrackerCfgParam.nCaptureOnePos, 0, 100,
        "抓拍第一张图的位置", "",
        CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "TwoPos",
        &cTrackerCfgParam.nCaptureTwoPos,
        cTrackerCfgParam.nCaptureTwoPos, 0, 100,
        "抓拍第二张图的位置", "",
        CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "ThreePos",
        &cTrackerCfgParam.nCaptureThreePos,
        cTrackerCfgParam.nCaptureThreePos, 0, 100,
        "抓拍第三张图的位置", "",
        CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "FilterRushPeccancy",
        &cTrackerCfgParam.cTrafficLight.nFilterRushPeccancy,
        cTrackerCfgParam.cTrafficLight.nFilterRushPeccancy, 0, 1,
        "过滤掉第三张图为绿灯时的违章", "",
        CUSTOM_LEVEL
    );
    cTrackerCfgParam.cTrafficLight.nFilterMoreReview = 1;
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "FilterMoreReview",
        &cTrackerCfgParam.cTrafficLight.nFilterMoreReview,
        cTrackerCfgParam.cTrafficLight.nFilterMoreReview, 0, 1,
        "过滤大车车身多检开关", "",
        CUSTOM_LEVEL
    );
    cTrackerCfgParam.cScaleSpeed.fEnable = 1;
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ScaleSpeed", "Enable",
        &cTrackerCfgParam.cScaleSpeed.fEnable,
        cTrackerCfgParam.cScaleSpeed.fEnable,
        0, 1,
        "软件测速开关", "", CUSTOM_LEVEL
    );
    //电警默认使用空间标定法，并且不将该参数发送
    cTrackerCfgParam.cScaleSpeed.nMethodsFlag = 1;
    HvParamReadFloatWithWrite(
        pcParamStore,
        "\\Tracker\\ScaleSpeed", "CameraHigh",
        &cTrackerCfgParam.cScaleSpeed.fltCameraHigh,
        cTrackerCfgParam.cScaleSpeed.fltCameraHigh,
        0.01f, 10000.0f,
        "摄像机高度", "", CUSTOM_LEVEL
    );
    HvParamReadFloatWithWrite(
        pcParamStore,
        "\\Tracker\\ScaleSpeed", "AdjustCoef",
        &cTrackerCfgParam.cScaleSpeed.fltAdjustCoef,
        cTrackerCfgParam.cScaleSpeed.fltAdjustCoef,
        0.01f, 10.0f,
        "修正系数", "", CUSTOM_LEVEL
    );
    for (int i = 0; i < 11; i++)
    {
        char szTransMarixText[32];
        char szTransMarixKey[32];
        sprintf(szTransMarixText, "变换矩阵_%d", i);
        sprintf(szTransMarixKey, "TransMarix_%d", i);
        HvParamReadFloatWithWrite(
            pcParamStore,
            "\\Tracker\\ScaleSpeed", szTransMarixKey,
            &cTrackerCfgParam.cScaleSpeed.rgfltTransMarix[i],
            cTrackerCfgParam.cScaleSpeed.rgfltTransMarix[i],
            -3.40282347e+37F, 3.40282347e+37F,
            szTransMarixText, "", CUSTOM_LEVEL
        );
    }

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet", "VideoDetMode",
        &cTrackerCfgParam.cVideoDet.nVideoDetMode,
        cTrackerCfgParam.cVideoDet.nVideoDetMode,
        NO_VIDEODET, VIDEODET_MODE_COUNT,
        "视频检测模式", "0:不使用视频检测, 1:只使用背景检测, 2:使用视频检测", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet\\AreaCtrl", "Left",
        &cTrackerCfgParam.cVideoDet.rcVideoDetArea.left,
        cTrackerCfgParam.cVideoDet.rcVideoDetArea.left,
        0, 100,
        "视频检测左区域", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet\\AreaCtrl", "Right",
        &cTrackerCfgParam.cVideoDet.rcVideoDetArea.right,
        cTrackerCfgParam.cVideoDet.rcVideoDetArea.right,
        0, 100,
        "视频检测右区域", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet\\AreaCtrl", "Top",
        &cTrackerCfgParam.cVideoDet.rcVideoDetArea.top,
        cTrackerCfgParam.cVideoDet.rcVideoDetArea.top,
        0, 100,
        "视频检测上区域", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet\\AreaCtrl", "Bottom",
        &cTrackerCfgParam.cVideoDet.rcVideoDetArea.bottom,
        cTrackerCfgParam.cVideoDet.rcVideoDetArea.bottom,
        0, 100,
        "视频检测下区域", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet", "DayBackgroudThreshold",
        &cTrackerCfgParam.cVideoDet.nDayBackgroudThreshold,
        cTrackerCfgParam.cVideoDet.nDayBackgroudThreshold,
        0, 255,
        "白天背景检测阈值", "", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet", "nBackgroudThreshold",
        &cTrackerCfgParam.cVideoDet.nNightBackgroudThreshold,
        cTrackerCfgParam.cVideoDet.nNightBackgroudThreshold,
        0, 255,
        "晚上背景检测阈值", "", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet", "VoteFrameCount",
        &cTrackerCfgParam.cVideoDet.nVoteFrameCount,
        cTrackerCfgParam.cVideoDet.nVoteFrameCount,
        15, 100,
        "投票有效帧数，当帧数大于此值就投票", "", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet", "CheckAcross",
        &cTrackerCfgParam.cVideoDet.nCheckAcross,
        cTrackerCfgParam.cVideoDet.nCheckAcross,
        0, 1,
        "是否检测横向行驶的物体", "", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet", "ShadowDetMode",
        &cTrackerCfgParam.cVideoDet.nShadowDetMode,
        cTrackerCfgParam.cVideoDet.nShadowDetMode,
        0, 1,
        "阴影检测模式", "0:不使用阴影检测, 1:使用阴影检测", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet", "ShadowDetEffect",
        &cTrackerCfgParam.cVideoDet.nShadowDetEffect,
        cTrackerCfgParam.cVideoDet.nShadowDetEffect,
        0, 100,
        "阴影检测强弱程度值", "0:使用缺省值(不精确), 1:阴影检测强弱程度最小, 100:阴影检测强弱程度最大", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet", "DiffType",
        &cTrackerCfgParam.cVideoDet.nDiffType,
        cTrackerCfgParam.cVideoDet.nDiffType,
        0, 1,
        "前景提取方式", "值为0:帧差提取 值为1:帧差后sobel前景，针对高速路卡口两车道车身多检", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet", "MedFilter",
        &cTrackerCfgParam.cVideoDet.nMedFilter,
        cTrackerCfgParam.cVideoDet.nMedFilter,
        0, 1,
        "抑制抖动开关", "值为0:抑制抖动关闭  值为1:抑制抖动打开", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet", "MedFilterUpLine",
        &cTrackerCfgParam.cVideoDet.nMedFilterUpLine,
        cTrackerCfgParam.cVideoDet.nMedFilterUpLine,
        0, 100,
        "抑制抖动作用区域上区域", "值为0:屏幕最上方 值为100:屏幕最下方", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet", "MedFilterDownLine",
        &cTrackerCfgParam.cVideoDet.nMedFilterDownLine,
        cTrackerCfgParam.cVideoDet.nMedFilterDownLine,
        0, 100,
        "抑制抖动作用区域下区域", "值为0:屏幕最上方 值为100:屏幕最下方", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "EnableFlashLight",
        &cTrackerCfgParam.fEnableFlashLight, cTrackerCfgParam.fEnableFlashLight,
        0, 1,
        "抓拍补光灯模式开关", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "FlashLightThreshold",
        &cTrackerCfgParam.nFlashLightThreshold, cTrackerCfgParam.nFlashLightThreshold,
        0, 255,
        "抓拍补光亮度阈值", "", CUSTOM_LEVEL
    );
    cTrackerCfgParam.cVideoDet.iNoPlateDetMode = 1;
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet", "NoPlateDetMode",
        &cTrackerCfgParam.cVideoDet.iNoPlateDetMode,
        cTrackerCfgParam.cVideoDet.iNoPlateDetMode,
        0, 1,
        "无牌车检测模式 (0:标准模式 1:模板模式)", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet", "EPOutPutDetectLine",
        &cTrackerCfgParam.cVideoDet.iEPOutPutDetectLine,
        cTrackerCfgParam.cVideoDet.iEPOutPutDetectLine,
        0, 100,
        "电子警察检测与跟踪交接线", "", PROJECT_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet", "EPFGDetectLine",
        &cTrackerCfgParam.cVideoDet.iEPFGDetectLine,
        cTrackerCfgParam.cVideoDet.iEPFGDetectLine,
        0, 100,
        "电警无牌车检测屏蔽区域(距离顶部百分比)", "", PROJECT_LEVEL
    );
/*
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet", "EPUseCarTailModel",
        &cTrackerCfgParam.cVideoDet.iEPUseCarTailModel,
        cTrackerCfgParam.cVideoDet.iEPUseCarTailModel,
        0, 1,
        "电警无牌车检测是否使用模型过滤多检", "", PROJECT_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\VideoDet", "EPNoPlateScoreThreld",
        &cTrackerCfgParam.cVideoDet.iEPNoPlateScoreThreld,
        cTrackerCfgParam.cVideoDet.iEPNoPlateScoreThreld,
        0, 1000,
        "无牌车过滤多检的分数阈值", "", PROJECT_LEVEL
    );

    //扫描区域有效性判断
    if (!CheckArea(
                cTrackerCfgParam.cDetectArea.DetectorAreaLeft,
                cTrackerCfgParam.cDetectArea.DetectorAreaTop,
                cTrackerCfgParam.cDetectArea.DetectorAreaRight,
                cTrackerCfgParam.cDetectArea.DetectorAreaBottom
            ))
    {
        cTrackerCfgParam.cDetectArea.DetectorAreaLeft = 2;
        cTrackerCfgParam.cDetectArea.DetectorAreaTop = 60;
        cTrackerCfgParam.cDetectArea.DetectorAreaRight = 98;
        cTrackerCfgParam.cDetectArea.DetectorAreaBottom = 100;
        pcParamStore->SetInt(
            "\\Tracker\\DetAreaCtrl\\Normal", "Left",
            cTrackerCfgParam.cDetectArea.DetectorAreaLeft
        );
        pcParamStore->SetInt(
            "\\Tracker\\DetAreaCtrl\\Normal", "Top",
            cTrackerCfgParam.cDetectArea.DetectorAreaTop
        );
        pcParamStore->SetInt(
            "\\Tracker\\DetAreaCtrl\\Normal", "Right",
            cTrackerCfgParam.cDetectArea.DetectorAreaRight
        );
        pcParamStore->SetInt(
            "\\Tracker\\DetAreaCtrl\\Normal", "Bottom",
            cTrackerCfgParam.cDetectArea.DetectorAreaBottom
        );
    }
*/
    //Scale有效性判断
    if (cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum >
            cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum)
    {
        pcParamStore->SetInt("\\Tracker\\DetAreaCtrl\\Normal", "MinScale", 6);
        pcParamStore->SetInt("\\Tracker\\DetAreaCtrl\\Normal", "MaxScale", 10);
        cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum = 5;
        cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum = 9;
    }

    //视频检测区域有效性判断
    if (!CheckArea(
                cTrackerCfgParam.cVideoDet.rcVideoDetArea.left,
                cTrackerCfgParam.cVideoDet.rcVideoDetArea.top,
                cTrackerCfgParam.cVideoDet.rcVideoDetArea.right,
                cTrackerCfgParam.cVideoDet.rcVideoDetArea.bottom
            ))
    {
        pcParamStore->SetInt("\\Tracker\\VideoDet\\AreaCtrl", "Left", 0);
        pcParamStore->SetInt("\\Tracker\\VideoDet\\AreaCtrl", "Top", 0);
        pcParamStore->SetInt("\\Tracker\\VideoDet\\AreaCtrl", "Right", 100);
        pcParamStore->SetInt("\\Tracker\\VideoDet\\AreaCtrl", "Bottom", 100);

        cTrackerCfgParam.cVideoDet.rcVideoDetArea.left = 0;
        cTrackerCfgParam.cVideoDet.rcVideoDetArea.top =0;
        cTrackerCfgParam.cVideoDet.rcVideoDetArea.right = 100;
        cTrackerCfgParam.cVideoDet.rcVideoDetArea.bottom = 100;
    }
    LoadTrafficLightParam(pcParamStore, cTrackerCfgParam.cTrafficLight);
    return 0;
}

char *g_rgszLightType[] =
{
    "BigSuitlight",
    "BigSuitlight2",
    "Suitlight",
    "Suitlight2",
    "Day",
    "Day2",
    "Backlight",
    "Backlight2",
    "BigBacklight",
    "BigBacklight2",
    "Nightfall",
    "Nightfall2",
    "Night",
    "Night2"
};

HRESULT InitCamCfgParam(CParamStore* pcParamStore, CAM_CFG_PARAM *pCfgCamParam)
{
    if (pCfgCamParam->iMinExposureTime == 0)
    {
        float fTmp = pCfgCamParam->fltDB / 20;
        fTmp = pow((float)10.0, fTmp);
        pCfgCamParam->irgExposureTime[13] = pCfgCamParam->iMaxExposureTime;
        pCfgCamParam->irgGain[13] = pCfgCamParam->iMaxGain;

        for (int i = 12; i >= 0; i--)
        {
            int iNewExposureTime, iNewGain;
            int iLastExposureTime, iLastGain;
            iLastGain = pCfgCamParam->irgGain[i + 1];
            iNewGain = iLastGain - (int)(pCfgCamParam->fltDB * 10);
            iLastExposureTime = pCfgCamParam->irgExposureTime[i + 1];
            if ( iNewGain >= pCfgCamParam->iMinGain )
            {
                pCfgCamParam->irgExposureTime[i] = iLastExposureTime;
                pCfgCamParam->irgGain[i] = iNewGain;
            }
            else
            {
                iNewExposureTime = (int)(((float)iLastExposureTime / fTmp) + 0.5);
                pCfgCamParam->irgExposureTime[i] = iNewExposureTime;
                pCfgCamParam->irgGain[i] = iLastGain;
            }
        }
    }
    else
    {
        float fTmp = pCfgCamParam->fltDB / 20;
        fTmp = pow((float)10.0, fTmp);
        int iGainLevel = (int)(pCfgCamParam->fltDB * 10);

        pCfgCamParam->irgExposureTime[0] = pCfgCamParam->iMinExposureTime;
        pCfgCamParam->irgGain[0] = pCfgCamParam->iMinGain;

        for (int i = 1; i < 14; i++)
        {
            int iNewExposureTime, iNewGain;
            int iLastExposureTime, iLastGain;
            iLastExposureTime = pCfgCamParam->irgExposureTime[i - 1];
            iLastGain = pCfgCamParam->irgGain[i - 1];
            iNewExposureTime = (int)(((float)iLastExposureTime * fTmp) + 0.5);
            if ( iNewExposureTime <= pCfgCamParam->iMaxExposureTime )
            {
                pCfgCamParam->irgExposureTime[i] = iNewExposureTime;
                pCfgCamParam->irgGain[i] = iLastGain;
            }
            else
            {
                iNewGain = iLastGain + iGainLevel;
                if ( iNewGain > pCfgCamParam->iMaxGain )
                {
                    pCfgCamParam->irgExposureTime[i] = iLastExposureTime;
                    pCfgCamParam->irgGain[i] = iLastGain;
                }
                else
                {
                    pCfgCamParam->irgExposureTime[i] = iLastExposureTime;
                    pCfgCamParam->irgGain[i] = iNewGain;
                }
            }
        }
    }

    char szText[255];
    int iAGCDB = (pCfgCamParam->iMaxAGCLimit - pCfgCamParam->iMinAGCLimit) / 13;
    if (iAGCDB > 0)
    {
        for (int i = 0; i < 14; i++)
        {
            sprintf(szText, "\\HvDsp\\Camera\\%s", g_rgszLightType[i]);
            pCfgCamParam->irgAGCLimit[i] = pCfgCamParam->iMinAGCLimit + (iAGCDB * i);
            pcParamStore->SetInt(szText, "AGCLimit", pCfgCamParam->irgAGCLimit[i]);
        }
    }

    if (pCfgCamParam->iEnableAGC)
    {
        for (int i = 0; i < 14; i++)
        {
            pCfgCamParam->irgExposureTime[i] = -1;
            pCfgCamParam->irgGain[i] = -1;
        }
    }

    for (int i = 0; i < 14; i++)
    {
        sprintf(szText, "\\HvDsp\\Camera\\%s", g_rgszLightType[i]);
        pcParamStore->SetInt(szText, "ExposureTime", pCfgCamParam->irgExposureTime[i]);
        pcParamStore->SetInt(szText, "Plus", pCfgCamParam->irgGain[i]);
    }

    return S_OK;
}

int LoadCamCfgParam(
    CParamStore* pcParamStore,
    CAM_CFG_PARAM& cCamCfgParam
)
{
#ifdef SINGLE_BOARD_PLATFORM
    HvParamReadStringWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "Addr",
        cCamCfgParam.szIP, (100-1),
        "识别相机IP", "", CUSTOM_LEVEL
    );
    cCamCfgParam.iCamType = 2;
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "ProtocolType",
        &cCamCfgParam.iCamType, cCamCfgParam.iCamType,
        0, 4,
        "协议类型", "0:测试专用协议; 1:NVC相机; 2:创宇相机; 3:广播协议; 4:HV摄像机协议", CUSTOM_LEVEL
    );
#else
    HvParamReadStringWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "Addr",
        cCamCfgParam.szIP, (100-1),
        "识别相机IP", "", INNER_LEVEL
    );
    cCamCfgParam.iCamType = 0;
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "ProtocolType",
        &cCamCfgParam.iCamType, cCamCfgParam.iCamType,
        0, 3,
        "协议类型", "0:一体机协议; 1:测试专用协议1; 2:测试专用协议2; 3:测试专用协议3;", INNER_LEVEL
    );
#endif
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "DynamicCfgEnable",
        &cCamCfgParam.iDynamicCfgEnable, cCamCfgParam.iDynamicCfgEnable,
        0, 1,
        "动态设置参数", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "DynamicTriggerEnable",
        &cCamCfgParam.iDynamicTriggerEnable, cCamCfgParam.iDynamicTriggerEnable,
        0, 1,
        "视频触发抓拍相机", "对创宇J系列相机及HV摄像机有效", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "TriggerDelay",
        &cCamCfgParam.iTriggerDelay, cCamCfgParam.iTriggerDelay,
        0, 65535,
        "触发延时(ms)", "", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "AGCEnable",
        &cCamCfgParam.iEnableAGC, cCamCfgParam.iEnableAGC,
        0, 1,
        "使能AGC", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "AutoParamEnable",
        &cCamCfgParam.iAutoParamEnable, cCamCfgParam.iAutoParamEnable,
        0, 1,
        "自动填充相机参数", "", CUSTOM_LEVEL
    );
    if (cCamCfgParam.iAutoParamEnable == 1)
    {
        pcParamStore->SetInt("\\HvDsp\\Camera\\Ctrl", "AutoParamEnable", 0);
    }

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "MaxAGCLimit",
        &cCamCfgParam.iMaxAGCLimit, cCamCfgParam.iMaxAGCLimit,
        0, 255,
        "最大AGC门限值", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "MinAGCLimit",
        &cCamCfgParam.iMinAGCLimit, cCamCfgParam.iMinAGCLimit,
        0, 255,
        "最小AGC门限值", "", CUSTOM_LEVEL
    );
    HvParamReadFloatWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "DB",
        &cCamCfgParam.fltDB, cCamCfgParam.fltDB,
        1.0f, 10.0f,
        "步长", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "MinExposureTime",
        &cCamCfgParam.iMinExposureTime, cCamCfgParam.iMinExposureTime,
        0, 54000,
        "最小曝光时间", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "MinPlus",
        &cCamCfgParam.iMinGain, cCamCfgParam.iMinGain,
        0, 360,
        "最小增益", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "MaxExposureTime",
        &cCamCfgParam.iMaxExposureTime, cCamCfgParam.iMaxExposureTime,
        0, 54000,
        "最大曝光时间", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "MaxPlus",
        &cCamCfgParam.iMaxGain, cCamCfgParam.iMaxGain,
        0, 360,
        "最大增益", "", CUSTOM_LEVEL
    );

    char szText[255] = {0};
    char szChnText[255] = {0};
    for (int i = 0; i < MAX_LEVEL_COUNT; i++)
    {
        sprintf(szText, "\\HvDsp\\Camera\\%s", g_rgszLightType[i]);
        sprintf(szChnText, "%d级曝光时间", i);
        HvParamReadIntWithWrite(
            pcParamStore,
            szText, "ExposureTime",
            &cCamCfgParam.irgExposureTime[i], cCamCfgParam.irgExposureTime[i],
            -1, 54000,
            szChnText, "", CUSTOM_LEVEL
        );

        sprintf(szChnText, "%d级增益", i);
        HvParamReadIntWithWrite(
            pcParamStore,
            szText, "Plus",
            &cCamCfgParam.irgGain[i], cCamCfgParam.irgGain[i],
            -1, 360,
            szChnText, "", CUSTOM_LEVEL
        );

        sprintf(szChnText, "%d级AGC增益门限", i);
        HvParamReadIntWithWrite(
            pcParamStore,
            szText, "AGCLimit",
            &cCamCfgParam.irgAGCLimit[i], cCamCfgParam.irgAGCLimit[i],
            -1, 255,
            szChnText, "", CUSTOM_LEVEL
        );
#ifdef SINGLE_BOARD_PLATFORM
        sprintf(szChnText, "%d级亮度", i);
        HvParamReadIntWithWrite(
            pcParamStore,
            szText, "Brightness",
            &cCamCfgParam.irgBrightness[i], cCamCfgParam.irgBrightness[i],
            -75, 75,
            szChnText, "", CUSTOM_LEVEL
        );

        sprintf(szChnText, "%d级模式", i);
        HvParamReadStringWithWrite(
            pcParamStore,
            szText, "Mode",
            cCamCfgParam.rgszMode[i], 10,
            szChnText, "速度, 画质, 平衡, 快速", PROJECT_LEVEL
        );
#endif
    }

    if (cCamCfgParam.iAutoParamEnable == 1)
    {
        InitCamCfgParam(pcParamStore, &cCamCfgParam);
    }

    HvParamReadIntWithWrite(
        pcParamStore,
        "HvDsp\\Camera\\Ctrl", "AddrPort",
        &cCamCfgParam.nTestProtocolPort, cCamCfgParam.nTestProtocolPort,
        8100, 8999,
        "测试协议端口", "",
#ifdef SINGLE_BOARD_PLATFORM
        CUSTOM_LEVEL
#else
        INNER_LEVEL
#endif
    );

    return 0;
}

int LoadTrafficLightParam(
    CParamStore* pcParamStore,
    TRAFFICLIGHT_CFG_PARAM& cTrafficLightCfgParam
)
{
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "CheckType",
        &cTrafficLightCfgParam.nCheckType, cTrafficLightCfgParam.nCheckType,
        0, 1,
        "红绿灯检测方式(0:视频识别, 1:外接红绿灯信息)", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "AutoScanLight",
        &cTrafficLightCfgParam.nAutoScanLight, cTrafficLightCfgParam.nAutoScanLight,
        0, 1,
        "动态检测红绿灯位置开关", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrafficLight","DeleteLightEdge",
        &cTrafficLightCfgParam.fDeleteLightEdge,cTrafficLightCfgParam.fDeleteLightEdge,
        0,1,
        "去除灯框边缘开关","",PROJECT_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "SceneCheckMode",
        &cTrafficLightCfgParam.nSenceCheckMode, cTrafficLightCfgParam.nSenceCheckMode,
        0, 1,
        "场景检测模式", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "StopLinePos",
        &cTrafficLightCfgParam.nStopLinePos, cTrafficLightCfgParam.nStopLinePos,
        0, 100,
        "停止线位置(直行)", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "LeftStopLinePos",
        &cTrafficLightCfgParam.nLeftStopLinePos, cTrafficLightCfgParam.nLeftStopLinePos,
        0, 100,
        "停止线位置(左转)", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\Templet", "ScanSpeed",
        &cTrafficLightCfgParam.nMatchScanSpeed, cTrafficLightCfgParam.nMatchScanSpeed,
        0, 100,
        "扫描速度(0:快, 1:慢)", "", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "LightCount",
        &cTrafficLightCfgParam.nLightCount, cTrafficLightCfgParam.nLightCount,
        0, MAX_TRAFFICLIGHT_COUNT,
        "灯组数量", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "ValidSceneStatus",
        &cTrafficLightCfgParam.nValidSceneStatusCount, cTrafficLightCfgParam.nValidSceneStatusCount,
        1, 100,
        "有效场景状态计数", "相同状态要达到此值才输出", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "OutputInLine",
        &cTrafficLightCfgParam.nOutputInLine, cTrafficLightCfgParam.nOutputInLine,
        0, 1,
        "输出未过停止线的车辆", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "RedLightDelay",
        &cTrafficLightCfgParam.nRedLightDelay, cTrafficLightCfgParam.nRedLightDelay,
        0, 300,
        "多色灯黄灯变红灯间隔(秒)", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "SpecificLeft",
        &cTrafficLightCfgParam.nSpecificLeft, cTrafficLightCfgParam.nSpecificLeft,
        0, 1,
        "左转道相对", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "RunTypeEnable",
        &cTrafficLightCfgParam.nRunTypeEnable, cTrafficLightCfgParam.nRunTypeEnable,
        0, 1,
        "行驶类型强制输出", "", CUSTOM_LEVEL
    );
    for (int i = 0; i < MAX_TRAFFICLIGHT_COUNT; i++)
    {
        char szLightPosKey[32];
        char szLightPosInfo[32];
        sprintf(szLightPosKey, "LightPos_%d", i + 1);
        sprintf(szLightPosInfo, "灯组信息_%d", i + 1);
        HvParamReadStringWithWrite(
            pcParamStore,
            "\\Tracker\\TrafficLight", szLightPosKey,
            cTrafficLightCfgParam.rgszLightPos[i], (64-1),
            szLightPosInfo, "", CUSTOM_LEVEL
        );
    }
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight\\Scene", "SceneCount",
        &cTrafficLightCfgParam.nSceneCount, cTrafficLightCfgParam.nSceneCount,
        0, MAX_SCENE_COUNT,
        "红绿灯场景数", "", CUSTOM_LEVEL
    );
    for (int i = 0; i < MAX_SCENE_COUNT; i++)
    {
        char szSceneKey[32];
        char szSceneInfo[32];
        sprintf(szSceneKey, "Scene_%d", i + 1);
        sprintf(szSceneInfo, "场景信息_%d", i + 1);
        HvParamReadStringWithWrite(
            pcParamStore,
            "\\Tracker\\TrafficLight\\Scene", szSceneKey,
            cTrafficLightCfgParam.rgszScene[i], (32-1),
            szSceneInfo, "", CUSTOM_LEVEL
        );
    }

    HvParamReadStringWithWrite(
        pcParamStore,
        "Tracker\\TrafficLight", "IOConfig",
        cTrafficLightCfgParam.szIOConfig, sizeof(cTrafficLightCfgParam.szIOConfig),
        "外接IO红绿灯参数", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrafficLight", "EnhanceRedLight",
        &cTrafficLightCfgParam.fEnhanceRedLight, cTrafficLightCfgParam.fEnhanceRedLight,
        0,1,
        "红灯发白区域是否加红", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrafficLight","TrafficLightThread",
        &cTrafficLightCfgParam.nTrafficLightThread, cTrafficLightCfgParam.nTrafficLightThread,
        0,5,
        "红绿灯灵敏度","",CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrafficLight","TrafficLightBrightValue",
        &cTrafficLightCfgParam.nTrafficLightBrightValue, cTrafficLightCfgParam.nTrafficLightBrightValue,
        0,5,
        "红绿灯亮度等级","",CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrafficLight","AutoScanThread",
        &cTrafficLightCfgParam.nAutoScanThread, cTrafficLightCfgParam.nAutoScanThread,
        0,10,
        "红绿灯动态检测阈值","",CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrafficLight","SkipFrameNO",
        &cTrafficLightCfgParam.nSkipFrameNO, cTrafficLightCfgParam.nSkipFrameNO,
        0,50,
        "动态检测找灯间隔帧","",CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrafficLight","LightAdhesion",
        &cTrafficLightCfgParam.fIsLightAdhesion, cTrafficLightCfgParam.fIsLightAdhesion,
        0,1,
        "红绿灯是否粘连在一起","",CUSTOM_LEVEL
    );
    // zhaopy
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrafficLight","HMThreld",
        &cTrafficLightCfgParam.fltHMThreld, cTrafficLightCfgParam.fltHMThreld,
        0,100,
        "白天阈值","",PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrafficLight","HMThreldNight",
        &cTrafficLightCfgParam.fltHMThreldNight, cTrafficLightCfgParam.fltHMThreldNight,
        0,100,
        "夜景阈值","",PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrafficLight","HMDelay",
        &cTrafficLightCfgParam.iHMDelay, cTrafficLightCfgParam.iHMDelay,
        1,100,
        "白天延迟帧数","",PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrafficLight","HMDelayNight",
        &cTrafficLightCfgParam.iHMDelayNight, cTrafficLightCfgParam.iHMDelayNight,
        1,100,
        "夜景延迟帧数","",PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrafficLight","MaxMatchDiff",
        &cTrafficLightCfgParam.iMaxMatchDiff, cTrafficLightCfgParam.iMaxMatchDiff,
        1,100,
        "差异阈值","",PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\TrafficLight","RealForward",
        &cTrafficLightCfgParam.iRealForward, cTrafficLightCfgParam.iRealForward,
        0,100,
        "直行判断位置","",CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\TrafficLight", "FilterNoPlatePeccancy",
        &cTrafficLightCfgParam.fFilterNoPlatePeccancy, cTrafficLightCfgParam.fFilterNoPlatePeccancy,
        0, 1,
        "过滤无车牌违章结果", "", CUSTOM_LEVEL
    );

    return 0;
}

int GetRecogParamCountPart()
{
    return ARRSIZE(g_pszPartWorkModeList);
}

int GetRecogParamIndexOnNamePart(char *pszModeName)
{
    int nIndex = -1, nCount = GetRecogParamCountPart();
    for (int i = 0; i < nCount; i++)
    {
        if (strcmp(g_pszPartWorkModeList[i], pszModeName) == 0)
        {
            nIndex = i;
            break;
        }
    }

    return nIndex;
}

HRESULT GetRecogParamNameOnIndexPart(DWORD32 dwIndex, char *pszModeName)
{
    if ((int)dwIndex >= GetRecogParamCountPart())
    {
        return E_FAIL;
    }

    if (pszModeName == NULL)
    {
        return E_POINTER;
    }

    strcpy(pszModeName, g_pszPartWorkModeList[dwIndex]);

    return S_OK;
}

HRESULT SetWorkModePart(DWORD32 dwWorkMode)
{
    char szModeName[128] = {0};
    DWORD32 dwWorkModeCount = 0;

    if ((int)dwWorkMode >= GetRecogParamCountPart())
    {
        return E_FAIL;
    }

    GetPlateRecogParamCount(&dwWorkModeCount);

    //通过分型后模式索引获取模式名
    if (S_OK != GetRecogParamNameOnIndexPart(dwWorkMode, szModeName))
    {
        return E_FAIL;
    }

    //通过模式名获取分型前的模式索引
    dwWorkMode = GetRecogParamIndexOnName(szModeName);

    HRESULT hr = HvParamWriteInt(
                     &g_cParamStore,
                     "\\Tracker", "PlateRecogMode",
                     dwWorkMode, 0, 0, dwWorkModeCount - 1,
                     "工作模式索引", "",
                     INNER_LEVEL, TRUE
                 );

    return hr;
}
