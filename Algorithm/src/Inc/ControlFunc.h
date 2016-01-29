#ifndef _CONTROLFUNC_H_
#define _CONTROLFUNC_H_

#include "hvutils.h"
#include "hvsysinterface.h"
#include "TcpipCfg.h"

HRESULT GetSysInfo(HvSys::SYS_INFO* pInfo);

HRESULT SetSysInfo(HvSys::SYS_INFO* pInfo);

HRESULT GetTcpipAddr(
    BYTE8 *pMac,
    DWORD32 *pdwIP,
    DWORD32 *pdwMask,
    DWORD32 *pdwGateway
);

HRESULT SetTcpipAddr(
    const BYTE8* rgMac,
    DWORD32 dwIP,
    DWORD32 dwMask,
    DWORD32 dwGateway
);

HRESULT GetConnectedIP(
    DWORD32* pdwCount,
    DWORD32* pdwIP,
    DWORD32* pdwType
);

HRESULT GetIniFile(
    const char* szBuffer,
    UINT* pnBufLen
);

HRESULT SetIniFile(
    const char* szBuffer,
    UINT nBufLen
);

// 硬复位设备
// iResetMode: -1表示RESET到当前状态，其余的参见hvtarget_ARM.h的70行左右的GET_STATUS_ID_XXX系列宏定义。
HRESULT HvResetDevice(int iResetMode);

// 软复位设备
// Comment by Shaorg: 因为使用了守护进程机制，所以这里的iResetMode将被忽略。
HRESULT ResetHv(int iResetMode);

// 以下函数声明在此，实现体在DataCtrl.cpp
HRESULT ForceSend(DWORD32 dwVideoID);
HRESULT GetWorkModeInfo(HvSys::SYS_INFO* pInfo);
HRESULT GetWorkModeList(HvSys::SYS_INFO* pInfo);
HRESULT SetWorkMode(DWORD32 dwWorkMode);

// 设置设备运行状态
HRESULT SetRunState(DWORD32 dwStateCode);

// 获取设备运行状态
HRESULT GetRunState(DWORD32* pdwStateCode);

void SetIpFromEEPROM();

// 取黑匣子信息，若为一体机取主并通过PCI取从记录，长度包括\0结尾。
// 只返回 S_OK ，失败时pbInfo会带有失败信息。长度包括\0结尾。
HRESULT GetAllDebugStateInfo(BYTE8* pbInfo, DWORD32* pdwInfoLen);

// 取Resetlog, 长度包括\0结尾。
// 成功返回 S_OK, 失败返回 E_FAIL。长度包括\0结尾。
HRESULT GetResetLog(BYTE8* pbLog, DWORD32* pdwLogLen);

// 取Resetlog，若为一体机取主并通过PCI取从记录。
// 只返回 S_OK ，失败时pbLog会带有失败信息。长度包括\0结尾。
HRESULT GetAllResetLog(BYTE8* pbLog, DWORD32* pdwLogLen);

// Comment by Shaorg:
// 某些命令由于其特殊原因，必须先给上位机回应，
// 然后才能执行，否则上位机无法收到回应。
// 例如：复位命令，设置IP命令等等。
// 以下这个结构体就是用来记录这类命令的相关信息。
typedef struct tagCmdInfoFirstRetAfterExec
{
    bool fHvXmlSetIPValid;
    TcpipParam cTcpipParam;

    bool fHvXmlResetDeviceValid;
    int iResetMode;

    int  iHvXmlRestoreDefaultParamValid;
    bool fHvXmlRestoreFactoryParamValid;

    tagCmdInfoFirstRetAfterExec()
    {
        fHvXmlSetIPValid = false;
        memset(&cTcpipParam, 0, sizeof(cTcpipParam));

        fHvXmlResetDeviceValid = false;
        iResetMode = -1;

        iHvXmlRestoreDefaultParamValid = 0;
        fHvXmlRestoreFactoryParamValid = false;
    };

} CmdInfoFirstRetAfterExec;

#endif
