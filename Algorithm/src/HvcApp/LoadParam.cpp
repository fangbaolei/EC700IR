#include "LoadParam.h"
#include "HvParamIO.h"
#include "misc.h"
#include "platerecogparam.h"
#include "OuterControlImpl_Linux.h"
#include "hvtarget_ARM.h"
#include "ControlFunc.h"

//分型模式列表，名称要和platerecogparam.h中一致
char *g_pszPartWorkModeList[] =
{
    "抓拍识别"
};
#ifdef SINGLE_BOARD_PLATFORM
extern const char *PSZ_DSP_BUILD_NO;
extern const char *DSP_BUILD_DATE;
extern const char *DSP_BUILD_TIME;
#endif
CEprFileStorage g_cFileStorage(PARAM_START_ADDR, PARAM_SIZE);
CParamStore g_cParamStore;

HRESULT LoadModuleParam(ModuleParams& cModuleParams)
{
    if (FAILED(g_cFileStorage.Initialize("")))
    {
        HV_Trace(5, "Open param file failed, can't save setting!!!\n");
    }

    g_cParamStore.Initialize(&g_cFileStorage, true);

#ifdef SINGLE_BOARD_PLATFORM
    // 读取UBOOT下的IP、掩码以及网关
    SetIpFromEEPROM();
#endif

    LoadTcpipParam_1(&g_cParamStore, cModuleParams.cTcpipCfgParam_1);
    LoadTcpipParam_2(&g_cParamStore, cModuleParams.cTcpipCfgParam_2);
    //工作模式一定要最先加载，否则后面的参数有可能加载错
    LoadWorkModeParam(&g_cParamStore, cModuleParams);
    LoadBuildNoParam();
    LoadImageFrameParam(&g_cParamStore, cModuleParams.cImgFrameParam, cModuleParams.cCamCfgParam);
    LoadTrackerCfgParam(&g_cParamStore, cModuleParams.cTrackerCfgParam);
    LoadResultSenderParam(&g_cParamStore, cModuleParams.cResultSenderParam);

    //模块间参数传递
#ifndef SINGLE_BOARD_PLATFORM
    cModuleParams.cResultSenderParam.iEddyType = cModuleParams.cTrackerCfgParam.iEddyType;
#endif

    g_cParamStore.Save(HvCore::PSM_SIMPLE);

    return S_OK;
}

int LoadBuildNoParam()
{
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
#endif
}

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

static ResultSenderParam g_cResultSenderParam;

static HV_PARAM_TYPE_INT g_rgcResultSenderInt[] =
{
    {"\\HvDsp\\ResultProc\\MainVideo", "OutputBestSnap", &g_cResultSenderParam.iBestSnapshotOutput, g_cResultSenderParam.iBestSnapshotOutput, 0, 1, "主视频最清晰大图输出", "最小值：0； 最大值：1", PROJECT_LEVEL},
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
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "OutputObservedFrames",
        &cResultSenderParam.fOutputObservedFrames, cResultSenderParam.fOutputObservedFrames,
        0, 1,
        "输出有效帧数开关", "",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore,
        "HvDsp\\Misc", "OutputFilterInfo",
        &cResultSenderParam.fOutputFilterInfo, cResultSenderParam.fOutputFilterInfo,
        0, 1,
        "输出后处理信息", "", CUSTOM_LEVEL
    );
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


//--------------------------------------------------------------------------------------

int LoadImageFrameParam(
    CParamStore* pcParamStore,
    IMG_FRAME_PARAM& cImgFrameParam,
    CAM_CFG_PARAM& cCamCfgParam
)
{
    //先初始化
    cImgFrameParam.iCamNum = 1;
    strcpy(cImgFrameParam.rgstrCamIP[0], "0.0.0.0");
    //（字符串形式:[扫描区域(左上右下)], 最小车牌宽度,最大车牌宽度,车牌方差阈值）
    strcpy(cImgFrameParam.rgstrHVCParm[0], "[0,0,100,100],200,300,30");

#ifdef SINGLE_BOARD_PLATFORM
    HvParamReadStringWithWrite(
        pcParamStore,
        "\\HvDsp\\VideoProc\\HvcCamCfg", "CamIP_00",
        cImgFrameParam.rgstrCamIP[0], (32-1),
        "摄像机IP", "", CUSTOM_LEVEL
    );
#else
    HvParamReadStringWithWrite(
        pcParamStore,
        "\\HvDsp\\VideoProc\\HvcCamCfg", "CamIP_00",
        cImgFrameParam.rgstrCamIP[0], (32-1),
        "摄像机IP", "", INNER_LEVEL
    );
    cCamCfgParam.iCamType = 0;
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Camera\\Ctrl", "ProtocolType",
        &cCamCfgParam.iCamType, cCamCfgParam.iCamType,
        0, 4,
        "协议类型", "0:一体机协议; 4:测试专用协议4;", INNER_LEVEL
    );

    if (cCamCfgParam.iCamType != 0 && cCamCfgParam.iCamType != 4)
    {
        HV_Trace(5, "Error camera type(%d)! Recover default type(0)\n", cCamCfgParam.iCamType);
        cCamCfgParam.iCamType = 0;
    }

    strcpy(cCamCfgParam.szIP, cImgFrameParam.rgstrCamIP[0]);
#endif

    HvParamReadStringWithWrite(
        pcParamStore,
        "\\HvDsp\\VideoProc\\HvcCamCfg", "HVCParm_00",
        cImgFrameParam.rgstrHVCParm[0], (128-1),
        "抓拍识别参数", "", CUSTOM_LEVEL
    );

   //平行代码 检测扫描区域
   int nLeft = 0;
   int nTop = 0;
   int nRight = 100;
   int nBottom = 100;
   int nMinPlateWidth = 200;
   int nMaxPlateWidth = 300;
   int nVariance = 30;
   char szArea[128] = {0};
   sscanf(cImgFrameParam.rgstrHVCParm[0],"[%d,%d,%d,%d],%d,%d,%d",
          &nLeft, &nTop, &nRight, &nBottom, &nMinPlateWidth, &nMaxPlateWidth, &nVariance);
    if(!CheckArea(nLeft, nTop, nRight, nBottom))
    {
        sprintf(szArea,"[%d,%d,%d,%d],%d,%d,%d",
                0, 0, 100, 100, nMinPlateWidth, nMaxPlateWidth, nVariance);
        memcpy(cImgFrameParam.rgstrHVCParm[0], szArea, strlen(szArea));
    }
    return 0;
}

int LoadTrackerCfgParam(
    CParamStore* pcParamStore,
    TRACKER_CFG_PARAM& cTrackerCfgParam
)
{
#ifndef SINGLE_BOARD_PLATFORM
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\HvDsp\\Misc", "EddyType",
        &cTrackerCfgParam.iEddyType, cTrackerCfgParam.iEddyType,
        0, 1,
        "图片旋转(0:不旋转,1:逆时针旋转90度)", "",
        CUSTOM_LEVEL
    );
#endif
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\Misc", "VoteCount",
        &cTrackerCfgParam.nVoteCount, cTrackerCfgParam.nVoteCount,
        0, 5,
        "投票数", "", PROJECT_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\Misc", "Alpha5",
        &cTrackerCfgParam.fAlpha5, cTrackerCfgParam.fAlpha5,
        0, 1,
        "第五位字母识别", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\Misc", "Alpha6",
        &cTrackerCfgParam.fAlpha6, cTrackerCfgParam.fAlpha6,
        0, 1,
        "第六位字母识别", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\Misc", "Alpha7",
        &cTrackerCfgParam.fAlpha7, cTrackerCfgParam.fAlpha7,
        0, 1,
        "第七位字母识别", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\Misc", "PlateEnhanceThreshold",
        &cTrackerCfgParam.nPlateEnhanceThreshold, cTrackerCfgParam.nPlateEnhanceThreshold,
        0, 255,
        "车牌增强阈值", "", CUSTOM_LEVEL
    );
    cTrackerCfgParam.fltMinConfForOutput = 1.0f;
    HvParamReadFloatWithWrite(
        pcParamStore,
        "\\Tracker\\Misc", "MinConfForOutput",
        &cTrackerCfgParam.fltMinConfForOutput, cTrackerCfgParam.fltMinConfForOutput,
        0, 1,
        "可信度阈值", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\Misc", "FirstPlatePos",
        &cTrackerCfgParam.nFirstPlatePos, cTrackerCfgParam.nFirstPlatePos,
        0, 1,
        "优先出牌方位", "", CUSTOM_LEVEL
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
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\Recognition", "EnableRecongCarColor",
        &cTrackerCfgParam.fEnableRecgCarColor, cTrackerCfgParam.fEnableRecgCarColor,
        0, 1,
        "车身颜色识别开关", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Recognition","EnableT1Model",
        &cTrackerCfgParam.fEnableT1Model, cTrackerCfgParam.fEnableT1Model,
        0, 1,
        "T-1模型开关", "", PROJECT_LEVEL
    );
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
        "Tracker\\Misc", "RecogGXPolice",
        &cTrackerCfgParam.nRecogGxPolice, cTrackerCfgParam.nRecogGxPolice,
        0, 1,
        "地方警牌识别开关", "", CUSTOM_LEVEL
    );
    HvParamReadFloatWithWrite(
        pcParamStore,
        "\\Tracker\\Misc", "DetSetpAdj",
        &cTrackerCfgParam.fltPlateDetect_StepAdj, cTrackerCfgParam.fltPlateDetect_StepAdj,
        1.0, 5.0,
        "步长调整系数", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Misc", "OnePlateMode",
        &cTrackerCfgParam.fOnePlateMode, cTrackerCfgParam.fOnePlateMode,
        0, 1,
        "检测单车牌", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "Tracker\\Misc", "SegHeight",
        &cTrackerCfgParam.nSegHeight, cTrackerCfgParam.nSegHeight,
        10, 100,
        "分割参考高度", "", CUSTOM_LEVEL
    );
    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate", "EnableDBGreenSegment",
        &cTrackerCfgParam.nPlateDetect_Green, cTrackerCfgParam.nPlateDetect_Green,
        0, 1,
        "绿牌识别开关", "", CUSTOM_LEVEL
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
        "\\Tracker\\ProcessPlate", "LightBlueFlag",
        &cTrackerCfgParam.nProcessPlate_LightBlue, cTrackerCfgParam.nProcessPlate_LightBlue,
        0, 1,
        "浅蓝牌开关", "", PROJECT_LEVEL
    );

        HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate\\BlackPlate", "Enable",
        &cTrackerCfgParam.fProcessPlate_BlackPlate_Enable, cTrackerCfgParam.fProcessPlate_BlackPlate_Enable,
        0, 1,
        "使能黑牌判断规则", "", CUSTOM_LEVEL
    );

        HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate\\BlackPlate", "H0",
        &cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H0, cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H0,
        0, 240,
        "蓝牌色度下限", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate\\BlackPlate", "H1",
        &cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H1, cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H1,
        cTrackerCfgParam.nProcessPlate_BlackPlateThreshold_H0, 240,
        "蓝牌色度上限", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate\\BlackPlate", "S",
        &cTrackerCfgParam.nProcessPlate_BlackPlate_S, cTrackerCfgParam.nProcessPlate_BlackPlate_S,
        0, 240,
        "黑牌饱和度上限", "", CUSTOM_LEVEL
    );

    HvParamReadIntWithWrite(
        pcParamStore,
        "\\Tracker\\ProcessPlate\\BlackPlate", "L",
        &cTrackerCfgParam.nProcessPlate_BlackPlate_L, cTrackerCfgParam.nProcessPlate_BlackPlate_L,
        0, 240,
        "黑牌亮度上限", "", CUSTOM_LEVEL
    );
    return 0;
}

