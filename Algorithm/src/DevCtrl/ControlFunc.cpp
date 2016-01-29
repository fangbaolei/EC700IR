//该文件编码格式必须为WINDOS-936格式

#include "ControlFunc.h"
#include "hvsocket.h"
#include "HvParamIO.h"
#include "misc.h"
#include "TcpipCfg.h"
#include "hvtarget_ARM.h"
#include "HvPciLinkApi.h"
#include "hvutils.h"
/* LoadDspFromFlash.c */
extern "C" int SetDeviceState(unsigned long ulStateCode);
extern "C" int GetDeviceState(unsigned long* pulStateCode);
extern "C" int SetUbootNetAddr(DWORD32 dwIP, DWORD32 dwNetmask, DWORD32 dwGateway);
extern "C" int GetUbootNetAddr(DWORD32* pdwIP, DWORD32* pdwNetmask, DWORD32* pdwGateway);

using namespace HvSys;

/* LoadParam.cpp */
extern CParamStore g_cParamStore;

HRESULT SetSysInfo(SYS_INFO* pInfo)
{
    HRESULT hr = S_OK;

    switch (pInfo->nType)
    {
    case INFO_SYS_TIME:
    {
        REAL_TIME_STRUCT tTime;

        ConvertMsToTime(
            pInfo->Info.SysTime.dwTimeLow,
            pInfo->Info.SysTime.dwTimeHigh,
            &tTime
        );

        char szTime[64] = {0};
        sprintf(
            szTime,
            "date %02d%02d%02d%02d%04d.%02d > /dev/null", //月日时分年.秒
            tTime.wMonth, tTime.wDay, tTime.wHour,
            tTime.wMinute, tTime.wYear, tTime.wSecond
        );

        //设置系统时间并将系统时间写入RTC
        if (HV_System(szTime) != 0 || HV_System("hwclock -w") != 0)
        {
            hr = E_FAIL;
        }
        break;
    }

    case INFO_WORKMODE:
        SetWorkMode(pInfo->Info.WorkMode.dwMode);
        break;

    default :
        hr = E_NOTIMPL;
        break;
    }

    return hr;
}

HRESULT GetSysInfo(SYS_INFO* pInfo)
{
    //todo: 获取系统信息需要完成。目前临时这样做。
    HRESULT hr = S_OK;
    switch (pInfo->nType)
    {
    case INFO_PRODUCT_NAME:
        memcpy(pInfo->Info.ProductName.pbNameBuf, "DM6467_Demo", sizeof("DM6467_Demo"));
        pInfo->Info.ProductName.nLen = sizeof("DM6467_Demo");
        break;

    case INFO_SYS_NAME:
        memcpy(pInfo->Info.SysName.pbNameBuf, "DM6467_Linux", sizeof("DM6467_Linux"));
        pInfo->Info.SysName.nLen = sizeof("DM6467_Linux");
        break;

    case INFO_SYS_TIME:
        GetSystemTime(&pInfo->Info.SysTime.dwTimeLow, &pInfo->Info.SysTime.dwTimeHigh);
        break;

    case INFO_WORKMODE:
        hr = GetWorkModeInfo(pInfo);
        break;

    case INFO_WORKMODE_LIST:
        hr = GetWorkModeList(pInfo);
        break;

    default:
        hr = E_FAIL;
        memset(pInfo, 0, sizeof(SYS_INFO));
    }

    return hr;
}

//获取地址信息
HRESULT GetTcpipAddr(
    BYTE8 *pMac,
    DWORD32 *pdwIP,
    DWORD32 *pdwMask,
    DWORD32 *pdwGateway
)
{
    char szIp[32] = {0};
    char szMask[32] = {0};
    char szGateway[32] = {0};
    char szMac[32] = {0};

#ifdef _CAM_APP_
    GetLocalTcpipAddr("eth0", szIp, szMask, szGateway, szMac);
    GetUbootNetAddr(pdwIP, pdwMask, pdwGateway);
    *pdwIP = ntohl(*pdwIP);
    *pdwMask = ntohl(*pdwMask);
    *pdwGateway = ntohl(*pdwGateway);
#else
    if (IsTestCamLanPort())
    {
        GetLocalTcpipAddr("eth0.10", szIp, szMask, szGateway, szMac);
        *pdwIP = ntohl(inet_addr(szIp));
        *pdwMask = ntohl(inet_addr(szMask));
        *pdwGateway = ntohl(inet_addr(szGateway));
    }
    else
    {
        GetLocalTcpipAddr("eth0.20", szIp, szMask, szGateway, szMac);
        GetUbootNetAddr(pdwIP, pdwMask, pdwGateway);
        *pdwIP = ntohl(*pdwIP);
        *pdwMask = ntohl(*pdwMask);
        *pdwGateway = ntohl(*pdwGateway);
    }
#endif

    int iNum=0;
    int i=0;
    char* token = strtok(szMac, ":");
    if ( NULL != token )
    {
        i = 0;
        do
        {
            sscanf(token, "%x", &iNum);
            pMac[i++] = (BYTE8)iNum;

            token = strtok(NULL, ":");
        }
        while ( NULL != token );
    }

    return S_OK;
}

HRESULT SetTcpipAddr(
    const BYTE8* rgMac,
    DWORD32 dwIP,
    DWORD32 dwMask,
    DWORD32 dwGateway
)
{
    DWORD32 dwIPTmp;
    DWORD32 dwMaskTmp;
    DWORD32 dwGatewayTmp;
    BYTE8 rgMacTmp[8] = {0};

    if ( S_OK != GetTcpipAddr(rgMacTmp, &dwIPTmp, &dwMaskTmp, &dwGatewayTmp))
    {
        return E_FAIL;
    }
    if (memcmp(rgMac, rgMacTmp, 6) != 0)
    {
        return E_FAIL;
    }

    TcpipParam cTcpipParam;

    MyGetIpString(dwIP, cTcpipParam.szIp);
    MyGetIpString(dwMask, cTcpipParam.szNetmask);
    MyGetIpString(dwGateway, cTcpipParam.szGateway);

    dwIP = htonl(dwIP);
    dwMask = htonl(dwMask);
    dwGateway = htonl(dwGateway);

    if ( S_OK == SetUbootNetAddr(dwIP, dwMask, dwGateway) )
    {
        g_cParamStore.SetString(
            "\\System\\TcpipCfg", "CommIPAddr",
            cTcpipParam.szIp
        );
        g_cParamStore.SetString(
            "\\System\\TcpipCfg", "CommMask",
            cTcpipParam.szNetmask
        );
        g_cParamStore.SetString(
            "\\System\\TcpipCfg", "CommGateway",
            cTcpipParam.szGateway
        );
        if ( S_OK == g_cParamStore.Save() )
        {
            HRESULT hr = E_FAIL;
#ifdef SINGLE_BOARD_PLATFORM
            if (IsTestCamLanPort()) // 判断是否测试相机口（生产测试时使用）
            {
                hr = SetTcpipAddr_1(&cTcpipParam);  // 单板平台左边
            }
            else
            {
                hr = SetTcpipAddr_2(&cTcpipParam);  // 单板平台右边（即：靠近复位按钮）那个网口
            }
#else
            hr = SetTcpipAddr_0(&cTcpipParam);
#endif
            return hr;
        }
    }

    return E_FAIL;
}

HRESULT GetConnectedIP(
    DWORD32* pdwCount,
    DWORD32* pdwIP,
    DWORD32* pdwType
)
{
    return E_NOTIMPL;
}

HRESULT GetIniFile(
    const char* szBuffer,
    UINT* pnBufLen
)
{
    HRESULT hr = GetParamXml(&g_cParamStore, (char*)szBuffer, *pnBufLen);
    *pnBufLen = strlen((char*)szBuffer) + 1;
    return hr;
}

HRESULT SetIniFile(
    const char* szBuffer,
    UINT nBufLen
)
{
    return SetParamXml(&g_cParamStore, (char*)szBuffer);
}

/* Hvtarget_ARM.c */
extern "C" int g_iDisableResetWDT;

#ifndef SINGLE_BOARD_PLATFORM
static HRESULT SynSlaveDeviceState(int iResetMode)
{
    HRESULT hr = E_FAIL;

    int iRetSetState = 0;
    // 同步“从CPU”的启动状态标志，最多尝试3次。
    for (int i=0; i<3; i++)
    {
        iRetSetState = PciSetDeviceState(iResetMode);
        if ( -2 == iRetSetState )   // PCI连接失败
        {
            HV_Trace(5, "ResetHv::PciSetDeviceState PCI connect Failed! retry...\n");
            HV_Sleep(10);
        }
        else if ( -1 == iRetSetState )   // 命令执行失败
        {
            HV_Trace(5, "ResetHv::PciSetDeviceState is Failed! retry...\n");
            HV_Sleep(10);
        }
        else if ( 0 == iRetSetState )
        {
            HV_Trace(5, "ResetHv::PciSetDeviceState is Succ.\n");
            break;
        }
    }
    if ( 0 == iRetSetState )
    {
        hr = S_OK;
    }
    else
    {
        HV_Trace(5, "ResetHv::PciSetDeviceState is Error!\n");
    }

    return hr;
}
#endif

//复位操作 (包括复位程序和复位设备)
bool g_fHvReset = false;

HRESULT ResetHv(int iResetMode)
{
    g_fHvReset = true;
    HV_Trace(5, "ResetHv...[mode:0x%x]\n", iResetMode);

    HV_Exit(HEC_SUCC, "ResetHv");
    return S_OK;
}

HRESULT HvResetDevice(int iResetMode)
{
    g_fHvReset = true;
    HV_Trace(5, "HvResetDevice...[mode:0x%x]\n", iResetMode);

    //TODO:需要去除MagicNum
    if ( -1 != iResetMode
            && 0 != iResetMode
            && 1 != iResetMode
            && 2 != iResetMode
            && 3 != iResetMode
            && 4 != iResetMode )
    {
        return E_FAIL;
    }

    HRESULT hr = E_FAIL;

    if ( -1 != iResetMode )
    {
        if ( 0 == SetDeviceState(iResetMode) )
        {
#ifndef SINGLE_BOARD_PLATFORM
            hr = SynSlaveDeviceState(iResetMode);
#else
            hr = S_OK;
#endif
        }
    }

    // 卸载硬盘
    if (HV_System("dm 6 0 Result_disk"))
    {
        HV_Trace(5, "[dm 6 0 Result_disk] exec error.\n");
    }
    HV_Sleep(1000);

    if ( S_OK == hr )
    {
        HV_Exit(HEC_SUCC|HEC_RESET_DEV, "HvResetDevice");
    }
    else
    {
        char szInfo[64];
        sprintf(szInfo, "HvResetDevice is Error.[iResetMode = %d]", iResetMode);
        HV_Exit(HEC_FAIL|HEC_RESET_DEV, szInfo);
    }

    return S_OK;
}

HRESULT SetRunState(DWORD32 dwStateCode)
{
    if ( SetDeviceState(dwStateCode) >= 0 )
    {
        HV_Trace(5, "SetStateCmd: [%d]\n", dwStateCode);
        return S_OK;
    }
    else
    {
        HV_Trace(5, "SetStateCmd is failed.\n");
        return E_FAIL;
    }
}

HRESULT GetRunState(DWORD32* pdwStateCode)
{
    if ( GetDeviceState((unsigned long*)pdwStateCode) >= 0 )
    {
        HV_Trace(5, "GetStateCmd: [%d]\n", *pdwStateCode);
        return S_OK;
    }
    else
    {
        HV_Trace(5, "GetStateCmd is failed.\n");
        return E_FAIL;
    }
}

void SetIpFromEEPROM()
{
    DWORD32 dwIP=0,dwMask=0,dwGateway=0;
    GetUbootNetAddr(&dwIP, &dwMask, &dwGateway);
    dwIP = ntohl(dwIP);
    dwMask = ntohl(dwMask);
    dwGateway = ntohl(dwGateway);

    TcpipParam cTcpipParam;
    MyGetIpString(dwIP, cTcpipParam.szIp);
    MyGetIpString(dwMask, cTcpipParam.szNetmask);
    MyGetIpString(dwGateway, cTcpipParam.szGateway);

    HV_Trace(5, "Uboot IP: %s\n", cTcpipParam.szIp);
    HV_Trace(5, "Uboot Netmask: %s\n", cTcpipParam.szNetmask);
    HV_Trace(5, "Uboot Gateway: %s\n", cTcpipParam.szGateway);

    g_cParamStore.SetString(
        "\\System\\TcpipCfg", "CommIPAddr",
        cTcpipParam.szIp
    );
    g_cParamStore.SetString(
        "\\System\\TcpipCfg", "CommMask",
        cTcpipParam.szNetmask
    );
    g_cParamStore.SetString(
        "\\System\\TcpipCfg", "CommGateway",
        cTcpipParam.szGateway
    );
    g_cParamStore.Save();

#ifdef SINGLE_BOARD_PLATFORM
    if (!IsTestCamLanPort())
    {
        SetTcpipAddr_2(&cTcpipParam);
    }
#else
    SetTcpipAddr_0(&cTcpipParam);
#endif
}

HRESULT GetAllDebugStateInfo(BYTE8* pbInfo, DWORD32* pdwInfoLen)
{
    if ((pbInfo == NULL) || (pdwInfoLen == NULL))
    {
        return E_FAIL;
    }

    int iDataLen = 0;
    int iTemp;
    char* szTemp;

    szTemp = "\r\n-------Info------\r\n";
    iTemp = strlen(szTemp);
    memcpy(pbInfo, szTemp, iTemp);
    pbInfo += iTemp;
    iDataLen += iTemp;

    if (HvDebugStateInfoReadAll((char*)pbInfo, &iTemp) == S_OK)
    {
        HV_Trace(5, "GetDebugStateInfo ok");
    }
    else
    {
        HV_Trace(5, "GetDebugStateInfo Failed");

        szTemp = "\r\nRead Info Failed !\r\n";
        iTemp = strlen(szTemp);
        memcpy(pbInfo, szTemp, iTemp);
    }
    pbInfo += iTemp;
    iDataLen += iTemp;

#ifndef SINGLE_BOARD_PLATFORM
    szTemp = "\r\n-------Slave Info------\r\n";
    iTemp = strlen(szTemp);
    memcpy(pbInfo, szTemp, iTemp);
    pbInfo += iTemp;
    iDataLen += iTemp;

    iTemp = 16 * 1024;
    if (PciGetSlaveDebugInfo(pbInfo, &iTemp) == S_OK)
    {
        HV_Trace(5, "Slave GetDebugStateInfo ok");
    }
    else
    {
        HV_Trace(5, "Slave GetDebugStateInfo failed");

        szTemp = "\r\nPCI Read Slave Info Failed !\r\n";
        iTemp = strlen(szTemp);
        memcpy(pbInfo, szTemp, iTemp);
    }

    pbInfo += iTemp;
    iDataLen += iTemp;
#endif

    pbInfo[0] = '\0';	    // 使最后一位\0
    ++pbInfo;
    ++iDataLen;

    *pdwInfoLen = iDataLen;
    return S_OK;
}

HRESULT GetResetLog(BYTE8* pbLog, DWORD32* pdwLogLen)
{
    if ((pbLog == NULL) || (pdwLogLen == NULL))
    {
        return E_FAIL;
    }

    HRESULT hr = E_FAIL;

    FILE* fp = fopen("/log/reset.log", "r");
    if (fp)
    {
        BYTE8* pbPtr = pbLog;
        int iLineLen = 0;
        while (fgets((char*)pbPtr, 1024, fp) != NULL)
        {
            iLineLen = strlen((char*)pbPtr);
            pbPtr += iLineLen;
            *pdwLogLen += iLineLen;
        }
        fclose(fp);
        hr = S_OK;
    }

    return hr;
}

HRESULT GetAllResetLog(BYTE8* pbLog, DWORD32* pdwLogLen)
{
    if ((pbLog == NULL) || (pdwLogLen == NULL))
    {
        return E_FAIL;
    }

    int iDataLen = 0;
    int iTemp;
    char* szTemp;

    szTemp = "\r\n-------Log-------\r\n";
    iTemp = strlen(szTemp);
    memcpy(pbLog, szTemp, iTemp);
    pbLog += iTemp;
    iDataLen += iTemp;

    if (GetResetLog(pbLog, (DWORD32*)&iTemp) == S_OK)
    {
        HV_Trace(5, "GetResetLog ok");
    }
    else
    {
        HV_Trace(5, "GetResetLog Failed");

        szTemp = "\r\nRead Reset Log Failed !\r\n";
        iTemp = strlen(szTemp);
        memcpy(pbLog, szTemp, iTemp);
    }
    pbLog += iTemp;
    iDataLen += iTemp;

#ifndef SINGLE_BOARD_PLATFORM
    szTemp = "\r\n-------Slave  Log-------\r\n";
    iTemp = strlen(szTemp);
    memcpy(pbLog, szTemp, iTemp);
    pbLog += iTemp;
    iDataLen += iTemp;

    iTemp = 16 * 1024;
    if (PciGetResetLog(pbLog, &iTemp) == S_OK)
    {
        HV_Trace(5, "Slave GetResetLog ok");
    }
    else
    {
        HV_Trace(5, "Slave GetResetLog failed");

        szTemp = "\r\nPCI Read Slave Reset Log Failed !\r\n";
        iTemp = strlen(szTemp);
        memcpy(pbLog, szTemp, iTemp);
    }

    pbLog += iTemp;
    iDataLen += iTemp;
#endif

    pbLog[0] = '\0';	    // 使最后一位\0
    ++pbLog;
    ++iDataLen;

    *pdwLogLen = iDataLen;
    return S_OK;
}

int ResetInitHddParam()
{
    HvParamWriteInt(
#ifdef _CAM_APP_
        &g_cParamStore, "\\CamApp", "InitHdd",
#else
        &g_cParamStore, "\\HvDsp\\Misc", "InitHdd",
#endif
        0, 0, 0, 1,
        "初始化硬盘", "对硬盘进行分区以及格式化",
        5);

    g_cParamStore.Save();

    return 0;
}
