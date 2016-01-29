// 该文件编码必须为WINDOWS-936格式
#include "LoadParam.h"
#include "HvParamIO.h"
#include "misc.h"
#include "platerecogparam.h"
#include "hvtarget_ARM.h"
#include <math.h>
#include "HvParamIO.h"
#include "hvsysinterface.h"
using namespace HvSys;

static ModuleParams g_cModuleParams;

//分型模式列表，名称要和platerecogparam.h中一致
char *g_pszPartWorkModeList[] =
{
    "断面控制器"
};

CEprFileStorage g_cFileStorage(PARAM_START_ADDR, PARAM_SIZE);
CParamStore g_cParamStore;

HRESULT LoadModuleParam(ModuleParams& cModuleParams)
{
    if (FAILED(g_cFileStorage.Initialize("")))
    {
        HV_Trace(5, "Open param file failed, can't save setting!!!\n");
    }

    g_cParamStore.Initialize(&g_cFileStorage, true);
    LoadTcpipParam_1(&g_cParamStore, cModuleParams.cTcpipCfgParam_1);
    LoadTcpipParam_2(&g_cParamStore, cModuleParams.cTcpipCfgParam_2);
    //工作模式一定要最先加载，否则后面的参数有可能加载错
    LoadWorkModeParam(&g_cParamStore, cModuleParams);

    LoadResultSenderParam(&g_cParamStore, cModuleParams.cResultSenderParam);

    LoadDevParam(&g_cParamStore, cModuleParams.cDevParam);

    g_cParamStore.Save(HvCore::PSM_SIMPLE);

    //char szBuffer[64 * 1024] = {0};
    //GetParamXml(&g_cParamStore, (char*)szBuffer, sizeof(szBuffer));
    //printf(szBuffer);

    g_cModuleParams = cModuleParams;

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
        HvParamReadIntWithWrite(
            pcParamStore,
            "\\Tracker", "PlateRecogMode",
            &cModuleParams.nWorkModeIndex, cModuleParams.nWorkModeIndex,
            0, dwModeCount,
            "工作模式索引", "", INNER_LEVEL
        );
        pcParamStore->Save();
        HV_Trace(5, "Load Default Param OK...%s\n", szModeName);
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

    pcParamStore->Clear();

    LoadTcpipParam_1(pcParamStore, cTcpipCfgParam1);
    LoadTcpipParam_2(pcParamStore, cTcpipCfgParam2);

    pcParamStore->Save();

    HV_Trace(5, "Param Load Default...\n");
}

static ResultSenderParam g_cResultSenderParam;

static HV_PARAM_TYPE_INT g_rgcResultSenderInt[] =
{
    {"\\HvDsp\\Misc", "SafeSaveEnable", (int*)&g_cResultSenderParam.fIsSafeSaver, 1, 0, 1, "是否启动安全存储", "默认值：1； 最小值：0； 最大值：1\n0：不启动； 1：启动", CUSTOM_LEVEL},
    {"\\HvDsp\\Misc", "DoReplace", (int*)&g_cResultSenderParam.cProcRule.fReplace, g_cResultSenderParam.cProcRule.fReplace, 0, 1, "是否替换", "默认值：0； 最小值：0； 最大值：1\n0：不替换； 1：替换", PROJECT_LEVEL},
    {"\\HvDsp\\Misc", "DoLeach", (int*)&g_cResultSenderParam.cProcRule.fLeach, g_cResultSenderParam.cProcRule.fLeach, 0, 1, "是否过滤", "默认值：0； 最小值：0； 最大值：1\n0：不过滤； 1：过滤", PROJECT_LEVEL},
    {"\\HvDsp\\Misc", "DoCompaty", (int*)&g_cResultSenderParam.cProcRule.fCompaty, g_cResultSenderParam.cProcRule.fCompaty, 0, 1, "是否启用通配符规则", "默认值：0； 最小值：0； 最大值：1\n0：不启用； 1：启用", CUSTOM_LEVEL},
    {"\\HvDsp\\Misc", "LoopReplace", (int*)&g_cResultSenderParam.cProcRule.fLoopReplace, g_cResultSenderParam.cProcRule.fLoopReplace, 0, 1, "是否启用循环替换", "默认值：0； 最小值：0； 最大值：1\n0：不启用； 1：启用", CUSTOM_LEVEL},
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

    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "InitHdd",
        &cResultSenderParam.fInitHdd,
        cResultSenderParam.fInitHdd, 0, 1,
        "初始化硬盘", "对硬盘进行分区以及格式化。注：一次性有效",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "TraceRank",
        &g_nTraceRank,
        5, 1, 5,
        "调试信息输出控制（1:研发级,3:工程级,5:用户级）", "",
        PROJECT_LEVEL);

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

int LoadDevParam(
    CParamStore* pcParamStore,
    DevParam& cDevParam
)
{
    cDevParam.iFilterTime = 10;
    strcpy(cDevParam.szDevIP1, "172.18.10.10");
    strcpy(cDevParam.szDevIP2, "172.18.10.10");
    strcpy(cDevParam.szDevIP3, "172.18.10.10");
    strcpy(cDevParam.szDevIP4, "172.18.10.10");
    strcpy(cDevParam.szDevIP5, "172.18.10.10");
    strcpy(cDevParam.szDevIP6, "172.18.10.10");
    strcpy(cDevParam.szDevIP7, "172.18.10.10");
    strcpy(cDevParam.szDevIP8, "172.18.10.10");

    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "FilterTime",
        &cDevParam.iFilterTime,
        cDevParam.iFilterTime, 1, 30,
        "过滤不同一体机一段时间内的相同结果(单位:秒)","",
        CUSTOM_LEVEL);

    HvParamReadIntWithWrite(
        pcParamStore, "\\HvDsp\\Misc", "DevCount",
        &cDevParam.iDevCount,
        cDevParam.iDevCount, 0, 8,
        "一体机数量","",
        CUSTOM_LEVEL);

    HvParamReadStringWithWrite(
        pcParamStore,
        "\\HvDsp\\Misc", "DevIPAddr1",
        cDevParam.szDevIP1, 31,
        "第1个一体机IP地址", "", CUSTOM_LEVEL
    );

    HvParamReadStringWithWrite(
        pcParamStore,
        "\\HvDsp\\Misc", "DevIPAddr2",
        cDevParam.szDevIP2, 31,
        "第2个一体机IP地址", "", CUSTOM_LEVEL
    );

    HvParamReadStringWithWrite(
        pcParamStore,
        "\\HvDsp\\Misc", "DevIPAddr3",
        cDevParam.szDevIP3, 31,
        "第3个一体机IP地址", "", CUSTOM_LEVEL
    );

    HvParamReadStringWithWrite(
        pcParamStore,
        "\\HvDsp\\Misc", "DevIPAddr4",
        cDevParam.szDevIP4, 31,
        "第4个一体机IP地址", "", CUSTOM_LEVEL
    );

    HvParamReadStringWithWrite(
        pcParamStore,
        "\\HvDsp\\Misc", "DevIPAddr5",
        cDevParam.szDevIP5, 31,
        "第5个一体机IP地址", "", CUSTOM_LEVEL
    );

    HvParamReadStringWithWrite(
        pcParamStore,
        "\\HvDsp\\Misc", "DevIPAddr6",
        cDevParam.szDevIP6, 31,
        "第6个一体机IP地址", "", CUSTOM_LEVEL
    );

    HvParamReadStringWithWrite(
        pcParamStore,
        "\\HvDsp\\Misc", "DevIPAddr7",
        cDevParam.szDevIP7, 31,
        "第7个一体机IP地址", "", CUSTOM_LEVEL
    );

    HvParamReadStringWithWrite(
        pcParamStore,
        "\\HvDsp\\Misc", "DevIPAddr8",
        cDevParam.szDevIP8, 31,
        "第8个一体机IP地址", "", CUSTOM_LEVEL
    );
}

//--------------------------------------------------------------------------------------
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

//ParseExecXmlExtCmd.cpp
//--------------------------------------------------------------------------------------
HRESULT GetWorkModeInfo(SYS_INFO* pInfo)
{
    DWORD32 dwModeIndex = 0;
    char szModeName[128] = "UNKNOWN";
    //m_cModuleParams.nWorkModeIndex保存的是分型前的模式索引
    //因此需将该索引转换为分型后的索引
    GetRecogParamNameOnIndex(g_cModuleParams.nWorkModeIndex, szModeName);
    pInfo->Info.WorkMode.dwMode = GetRecogParamIndexOnNamePart(szModeName);
    pInfo->Info.WorkMode.dwModeCount = GetRecogParamCountPart();
    return S_OK;
}

HRESULT GetWorkModeList(SYS_INFO* pInfo)
{
    if (pInfo->Info.WorkModeList.nMode >= GetRecogParamCountPart())
    {
        strcpy((char*)pInfo->Info.WorkModeList.pbListBuf, "UNKNOWN");
        pInfo->Info.WorkModeList.nLen = 8;
        return E_FAIL;
    }

    GetRecogParamNameOnIndexPart(
        pInfo->Info.WorkModeList.nMode,
        (char*)pInfo->Info.WorkModeList.pbListBuf
    );
    pInfo->Info.WorkModeList.nLen = strlen((char*)(pInfo->Info.WorkModeList.pbListBuf)) + 1;

    return S_OK;
}

HRESULT SetWorkMode(DWORD32 dwWorkMode)
{
    return(SetWorkModePart(dwWorkMode));
}
