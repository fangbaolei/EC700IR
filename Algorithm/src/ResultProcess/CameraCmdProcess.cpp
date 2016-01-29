// 该文件编码格式必须是WIN936

#include "CameraCmdProcess.h"
#include "CameraController.h"
#include "ControlFunc.h"
#include "tinyxml.h"
#include "HvSerialLink.h"
#include "HvParamIO.h"
#include "HvDebugStateInfo.h"
#include "CamTransmit.h"

#define XML_PARAM_FILE_LEN 200*1024  // xml参数文件最大长度

/* LoadParam.cpp */
extern CParamStore g_cParamStore;

/* misc.cpp */
extern void DebugPrintf(const char* szDebugInfo, DWORD dwDumpLen, const char* szID);

/* ParseExecXmlExtCmd.cpp */
extern HRESULT ParseExecXmlExtCmd(char* pbXmlBuf, DWORD32& dwXmlLen);

/* LoadDspFromFlash.c */
extern "C" int SetUbootNetAddr(DWORD32 dwIP, DWORD32 dwNetmask, DWORD32 dwGateway);
extern "C" int ReadResetReport(char* szResetReport, const int iLen);
extern "C" int EmptyResetCount();

/* ParseExecXmlExtCmd.cpp */
extern CmdInfoFirstRetAfterExec g_cCmdInfoFirstRetAfterExec;

#ifndef SINGLE_BOARD_PLATFORM
    #include "HvPciLinkApi.h"
    #include "../CamApp/DataCtrl.h"
#endif

int UnknowCommand( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int ProtocolCommand( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int ResetDevice( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int XmlExtCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int NetComCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int GetDebugStateInfoCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );

int GetParamCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int SetParamCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int GetTimeCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int SetTimeCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int GetResetReport( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );

int SetExposureCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int SetRawShutter( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int SetGainCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int SetRgbGainCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int SetEncodeModeCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int SetAgcLimitCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int SetFpsModeCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int SetRegCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int SoftTriggerCaptureCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int DebugSetSomeValueCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
int UpdateControllPannelCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );
// 字符叠加
int SetCharacterValueCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink );

//==========================单双板共用函数==========================
int UnknowCommand( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    // Comment by Shaorg: todo: 可以考虑更好的处理，对于未知指令发来的数据可以不予已接收。
    if ( pCmdHeader->dwInfoSize != 0 )
    {
        DWORD32 dwAllocSize = pCmdHeader->dwInfoSize > 1024 ? 1024 : pCmdHeader->dwInfoSize;
        PBYTE8 pBuf = new BYTE8[dwAllocSize];
        if (pBuf == NULL ) return E_POINTER;

        DWORD32 dwRecvSize = pCmdHeader->dwInfoSize;
        while ( dwRecvSize > 0 )
        {
            //接收完数据
            DWORD32 dwTmp = 0;
            if (FAILED(pCmdLink->ReceiveData(pBuf, dwRecvSize > dwAllocSize ? dwAllocSize : dwRecvSize, &dwTmp))
                    || dwTmp == 0)
            {
                delete[] pBuf;
                return E_NETFAIL;
            }
            dwRecvSize = (dwTmp <= dwRecvSize) ? (dwRecvSize - dwTmp) : 0;
        }

        if ( pBuf != NULL )
        {
            delete[] pBuf;
            pBuf = NULL;
        }
    }

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_UNKNOW_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = S_OK;

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int ProtocolCommand( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_PROTOCOL_COMMAND;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = S_OK;

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int ResetDevice( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    //复位模式
    int iResetMode = 0;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    if (FAILED(pCmdLink->ReceiveData((PBYTE8)&iResetMode, 4, NULL)))
    {
        return E_NETFAIL;
    }

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_RESET_DEVICE_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = S_OK;

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    // 这个命令比较特殊，要先发送回应包，然后再复位设置。
    ResetHv(iResetMode);
    return S_OK;
}

static HRESULT SetIP(TcpipParam& cTcpipParam);

int XmlExtCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    HRESULT hr = E_FAIL;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    CFastMemAlloc cStack;
    BYTE8* pbXmlBuf;
    DWORD32 dwXmlLen = 32*1024; // 32KB
#ifndef SINGLE_BOARD_PLATFORM
    char szParamMode[32] = {0};
#endif

    //申请内存
    pbXmlBuf = (BYTE8*)cStack.StackAlloc((int)dwXmlLen, FALSE);
    if ( pbXmlBuf == NULL )
    {
        return E_OUTOFMEMORY;
    }

    HV_memset(pbXmlBuf, 0, dwXmlLen);

    if (FAILED(pCmdLink->ReceiveData(pbXmlBuf, pCmdHeader->dwInfoSize, NULL)))
    {
        return E_NETFAIL;
    }

    //打印收到的xml命令
    HV_Trace(5, (char *)pbXmlBuf);

    // 解析xml并执行相应的操作
    hr = ParseExecXmlExtCmd((char*)pbXmlBuf, dwXmlLen);

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_XML_EXT_CMD;
    cRespond.dwInfoSize = dwXmlLen;
    cRespond.dwResult = hr;

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)) )
    {
        return E_NETFAIL;
    }

    if ( 0 == cRespond.dwInfoSize )
    {
        return S_OK;
    }

    //发送回应数据
    if ( FAILED(pCmdLink->SendData((PBYTE8)pbXmlBuf, cRespond.dwInfoSize)) )
    {
        return E_NETFAIL;
    }

    if ( S_OK == hr )
    {
        if ( g_cCmdInfoFirstRetAfterExec.fHvXmlResetDeviceValid )
        {
            HV_Sleep(1000);
            g_cCmdInfoFirstRetAfterExec.fHvXmlResetDeviceValid = false;
            ResetHv(g_cCmdInfoFirstRetAfterExec.iResetMode);
        }
        else if ( g_cCmdInfoFirstRetAfterExec.fHvXmlSetIPValid )
        {
            HV_Sleep(1000);
            g_cCmdInfoFirstRetAfterExec.fHvXmlSetIPValid = false;
            SetIP(g_cCmdInfoFirstRetAfterExec.cTcpipParam);
        }
        else if ( g_cCmdInfoFirstRetAfterExec.iHvXmlRestoreDefaultParamValid )
        {
            HV_Sleep(1000);
#ifndef SINGLE_BOARD_PLATFORM
            if (g_cCmdInfoFirstRetAfterExec.iHvXmlRestoreDefaultParamValid & 0x02)
            {
                strcpy(szParamMode, "RestoreDefaultParam");
                PciSendParamSetData((unsigned char*)szParamMode, strlen(szParamMode) + 1);
            }
#endif
            if (g_cCmdInfoFirstRetAfterExec.iHvXmlRestoreDefaultParamValid & 0x01)
            {
                RestoreDefaultParam();
            }
            g_cCmdInfoFirstRetAfterExec.iHvXmlRestoreDefaultParamValid = 0;
        }
        else if ( g_cCmdInfoFirstRetAfterExec.fHvXmlRestoreFactoryParamValid )
        {
            HV_Sleep(1000);
            g_cCmdInfoFirstRetAfterExec.fHvXmlRestoreFactoryParamValid = false;
#ifndef SINGLE_BOARD_PLATFORM
            PciSendParamSetData(NULL, 0);
#endif
            RestoreFactoryParam();
        }
    }

    return S_OK;
}

// NetCom消息处理函数
int NetComCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    static unsigned char szSndData[5*1024];
    static unsigned char szRcvData[5*1024];

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

    int nRt = S_OK;
    unsigned int nLen = 0;
    bool bActSucc = false;

    if (FAILED(pCmdLink->ReceiveData((PBYTE8)&szSndData, pCmdHeader->dwInfoSize, &nLen)))
    {
        return E_NETFAIL;
    }

    if (( NULL != s_pSerialLink) && (s_pSerialLink->Lock(2000) == S_OK))
    {
        char buf[1024];
        char* p = buf;
        for(int i=0; i<(int)nLen; ++i)
        {
            p = buf + sprintf(p, "%02X ", szSndData[i]);
        }
        HV_Trace(5, buf);

        if ( nLen == (unsigned int)s_pSerialLink->Send((unsigned char*)szSndData, nLen) )
        {
            // 接收一个完整数据包
            if ( S_OK == s_pSerialLink->RecvPacket((unsigned char*)szRcvData, &nLen, 2000) )
            {
                bActSucc = true;
            }
        }
        s_pSerialLink->UnLock();
    }

    // 填写回应信息头
    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_NETCOM_CMD;
    if ( bActSucc )
    {
        cRespond.dwInfoSize = nLen;
        cRespond.dwResult = S_OK;
    }
    else
    {
        cRespond.dwInfoSize = 0;
        cRespond.dwResult = E_FAIL;
    }

    //发送回应包
    if (FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        nRt = E_NETFAIL;
    }
    else if ( bActSucc
        && (FAILED(pCmdLink->SendData((PBYTE8)szRcvData, nLen))) )
    {
        nRt = E_NETFAIL;
    }
    return nRt;
}

int GetDebugStateInfoCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_GET_DEBUG_INFO_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = S_OK;

    BYTE8* pbDebugInfoData = new BYTE8[128*1024];
    DWORD32 dwDataLen = 0;

    if ((pbDebugInfoData != NULL)
        &&(GetAllDebugStateInfo(pbDebugInfoData, &dwDataLen) == S_OK))
    {
        cRespond.dwInfoSize = dwDataLen;
        cRespond.dwResult = S_OK;
    }
    else
    {
        cRespond.dwInfoSize = 4;
        cRespond.dwResult = E_FAIL;
    }

    int iRt = S_OK;
    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)) )
    {
        iRt = E_NETFAIL;
    }
    else if ( FAILED(pCmdLink->SendData(pbDebugInfoData, cRespond.dwInfoSize)) )
    {
        iRt = E_NETFAIL;
    }

    if (pbDebugInfoData != NULL)
    {
        delete [] pbDebugInfoData;
    }
    return iRt;
}

#ifdef SINGLE_BOARD_PLATFORM
//==========================单板命令==========================

CAMERA_CMD_MAP_ITEM g_CameraCmdFuncMap[] =
{
    { CAMERA_UNKNOW_CMD, UnknowCommand },
    { CAMERA_PROTOCOL_COMMAND, ProtocolCommand},
    { CAMERA_GET_PARAM_CMD, GetParamCmd },
    { CAMERA_SET_PARAM_CMD, SetParamCmd },
    { CAMERA_GET_TIME_CMD, GetTimeCmd },
    { CAMERA_SET_TIME_CMD, SetTimeCmd },
    { CAMERA_RESET_DEVICE_CMD, ResetDevice },
    { CAMERA_GET_RESET_REPORT_CMD, GetResetReport },
    { CAMERA_XML_EXT_CMD, XmlExtCmd },
    { CAMERA_NETCOM_CMD, NetComCmd },
    { CAMERA_GET_DEBUG_INFO_CMD, GetDebugStateInfoCmd }
};

int GetParamCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    HRESULT hr = E_FAIL;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    CFastMemAlloc cStack;

    BYTE8* pbXmlBuf;

    DWORD32 dwXmlLen = XML_PARAM_FILE_LEN + 1;

    //申请内存
    pbXmlBuf = (BYTE8*)cStack.StackAlloc((int)dwXmlLen, FALSE);
    if ( pbXmlBuf == NULL )
    {
        return E_OUTOFMEMORY;
    }

    HV_memset(pbXmlBuf, 0, dwXmlLen);

    //读取参数
    hr = GetParamXml(&g_cParamStore, (char*)pbXmlBuf, dwXmlLen);
    if ( S_OK != hr )
    {
        dwXmlLen = 0;
    }
    int ipos = strstr((const char*)pbXmlBuf,"DeviceParam")-(const char*)pbXmlBuf;
    DebugPrintf((const char*)pbXmlBuf+ipos,512, "GetXmlParam");

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_GET_PARAM_CMD;
    cRespond.dwInfoSize = dwXmlLen;
    cRespond.dwResult = hr;

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)) )
    {
        return E_NETFAIL;
    }

    if ( 0 == cRespond.dwInfoSize )
    {
        return S_OK;
    }

    //发送回应数据
    if ( FAILED(pCmdLink->SendData((PBYTE8)pbXmlBuf, cRespond.dwInfoSize)) )
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int SetParamCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    HRESULT hr = E_FAIL;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    CFastMemAlloc cStack;
    BYTE8* pbXmlBuf;
    DWORD32 dwXmlLen = XML_PARAM_FILE_LEN + 1;

    //申请内存
    pbXmlBuf = (BYTE8*)cStack.StackAlloc((int)dwXmlLen, FALSE);
    if ( pbXmlBuf == NULL )
    {
        return E_OUTOFMEMORY;
    }

    HV_memset(pbXmlBuf, 0, dwXmlLen);

    if (FAILED(pCmdLink->ReceiveData(pbXmlBuf, pCmdHeader->dwInfoSize, NULL)))
    {
        return E_NETFAIL;
    }

    if ( 0 == strlen((const char*)pbXmlBuf) )
    {
        // 参数“恢复出厂设置”
        hr = RestoreFactoryParam();
    }
    else
    {
        int ipos = strstr((const char*)pbXmlBuf,"DeviceParam")-(const char*)pbXmlBuf;
        DebugPrintf((const char*)pbXmlBuf+ipos, 512, "SetXmlParam");
        hr = SetIniFile((char*)pbXmlBuf, pCmdHeader->dwInfoSize);
    }

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_SET_PARAM_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = hr;

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int GetTimeCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    DWORD32 dwTimeMsLow = 0;
    DWORD32 dwTimeMsHigh = 0;

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_GET_TIME_CMD;
    cRespond.dwInfoSize = 8;
    cRespond.dwResult = GetSystemTime(&dwTimeMsLow, &dwTimeMsHigh);

    char rgchBufTimeMs[8];
    memcpy(rgchBufTimeMs, &dwTimeMsLow, 4);
    memcpy(rgchBufTimeMs+4, &dwTimeMsHigh, 4);

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)) )
    {
        return E_NETFAIL;
    }

    //发送回应数据
    if ( FAILED(pCmdLink->SendData((PBYTE8)rgchBufTimeMs, cRespond.dwInfoSize)) )
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int SetTimeCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    DWORD32 dwTimeMsLow = 0;
    DWORD32 dwTimeMsHigh = 0;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    if (FAILED(pCmdLink->ReceiveData((PBYTE8)&dwTimeMsLow, 4, NULL)))
    {
        return E_NETFAIL;
    }
    if (FAILED(pCmdLink->ReceiveData((PBYTE8)&dwTimeMsHigh, 4, NULL)))
    {
        return E_NETFAIL;
    }

    HRESULT hr = S_OK;
    if ( S_OK != SetSystemTime(dwTimeMsLow, dwTimeMsHigh) )
    {
        hr = -1;
    }

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_SET_TIME_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = hr;

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int GetResetReport( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_GET_RESET_REPORT_CMD;

    static char szResetReport[4*1024];
    if ( 0 == ReadResetReport((char*)szResetReport, 4 * 1024) )
    {
        cRespond.dwInfoSize = strlen(szResetReport);
        cRespond.dwResult = S_OK;
    }
    else
    {
        cRespond.dwInfoSize = 0;
        cRespond.dwResult = S_FALSE;
    }

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)) )
    {
        return E_NETFAIL;
    }

    //发送回应数据
    if ( FAILED(pCmdLink->SendData((PBYTE8)szResetReport, cRespond.dwInfoSize)) )
    {
        return E_NETFAIL;
    }

    return S_OK;
}

#else
//==========================双板命令==========================

CAMERA_CMD_MAP_ITEM g_CameraCmdFuncMap[] =
{
    { CAMERA_UNKNOW_CMD, UnknowCommand },
    { CAMERA_PROTOCOL_COMMAND, ProtocolCommand},
    { CAMERA_GET_PARAM_CMD, GetParamCmd },
    { CAMERA_SET_PARAM_CMD, SetParamCmd },
    { CAMERA_XML_EXT_CMD, XmlExtCmd },
    { CAMERA_RESET_DEVICE_CMD, ResetDevice },
    { CAMERA_GET_RESET_REPORT_CMD, GetResetReport },
    { CAMERA_SET_REG_CMD, SetRegCmd },
    { CAMERA_DEBUG_SET_SOME_VALUE_CMD, DebugSetSomeValueCmd },
    { CAMERA_NETCOM_CMD, NetComCmd },
    { CAMERA_GET_DEBUG_INFO_CMD, GetDebugStateInfoCmd },
    { CAMERA_UPDATE_CONTROLL_PANNEL, UpdateControllPannelCmd },
    { CAMERA_SET_CHARACTER, SetCharacterValueCmd },

    // 暂时保留的一些命令
    { CAMERA_GET_TIME_CMD, GetTimeCmd },
    { CAMERA_SET_TIME_CMD, SetTimeCmd },
    { CAMERA_SET_EXPOSURE_CMD, SetExposureCmd },
    { CAMERA_SET_RAW_SHUTTER_CMD, SetRawShutter },
    { CAMERA_SET_GAIN_CMD, SetGainCmd },
    { CAMERA_SET_RGB_GAIN_CMD, SetRgbGainCmd },
    { CAMERA_SET_ENCODE_MODE_CMD, SetEncodeModeCmd },
    { CAMERA_SET_FPS_MODE_CMD, SetFpsModeCmd },
    { CAMERA_SET_AGCLIMIT_CMD, SetAgcLimitCmd },
    { CAMERA_SOFT_TRIGGER_CAPTURE_CMD, SoftTriggerCaptureCmd },
};

int SetExposureCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    //相机快门时间
    int iShutter = 0;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    if (FAILED(pCmdLink->ReceiveData((PBYTE8)&iShutter, 4, NULL)))
    {
        return E_NETFAIL;
    }

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_SET_EXPOSURE_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = g_cCameraController.SetShutter_Camyu(iShutter);

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int SetRawShutter( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    //相机快门时间
    int iShutterRaw = 0;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    if (FAILED(pCmdLink->ReceiveData((PBYTE8)&iShutterRaw, 4, NULL)))
    {
        return E_NETFAIL;
    }

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_SET_RAW_SHUTTER_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = g_cCameraController.SetShutter(iShutterRaw);

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int SetGainCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    //相机增益
    int iGain = 0;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    if (FAILED(pCmdLink->ReceiveData((PBYTE8)&iGain, 4, NULL)))
    {
        return E_NETFAIL;
    }

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_SET_GAIN_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = g_cCameraController.SetGain_Camyu(iGain);

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int SetRgbGainCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    //相机RGB增益
    struct
    {
        int iGainR;
        int iGainG;
        int iGainB;
    } cRgbGain;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    if (FAILED(pCmdLink->ReceiveData((PBYTE8)&cRgbGain, sizeof(cRgbGain), NULL)))
    {
        return E_NETFAIL;
    }

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_SET_RGB_GAIN_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = g_cCameraController.SetRgbGain_Camyu(cRgbGain.iGainR, cRgbGain.iGainG, cRgbGain.iGainB);

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int SetEncodeModeCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    //编码模式（-1：关闭；0：Jpeg流；1：H.264流；2：双码流）
    int iEncodeMode = 0;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    if (FAILED(pCmdLink->ReceiveData((PBYTE8)&iEncodeMode, 4, NULL)))
    {
        return E_NETFAIL;
    }

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_SET_ENCODE_MODE_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = g_pCamApp->DynChangeParam(DCP_CAM_OUTPUT, iEncodeMode);

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int SetAgcLimitCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    //AGC期望亮度
    int iAgcLimit = 0;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    if (FAILED(pCmdLink->ReceiveData((PBYTE8)&iAgcLimit, 4, NULL)))
    {
        return E_NETFAIL;
    }

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_SET_AGCLIMIT_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = g_pCamApp->DynChangeParam(DCP_AGC_TH, iAgcLimit);

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int GetTimeCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    DWORD32 dwTimeMsLow = 0;
    DWORD32 dwTimeMsHigh = 0;

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_GET_TIME_CMD;
    cRespond.dwInfoSize = 8;
    cRespond.dwResult = GetSystemTime(&dwTimeMsLow, &dwTimeMsHigh);

    char rgchBufTimeMs[8];
    memcpy(rgchBufTimeMs, &dwTimeMsLow, 4);
    memcpy(rgchBufTimeMs+4, &dwTimeMsHigh, 4);

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)) )
    {
        return E_NETFAIL;
    }

    //发送回应数据
    if ( FAILED(pCmdLink->SendData((PBYTE8)rgchBufTimeMs, cRespond.dwInfoSize)) )
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int SetTimeCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    DWORD32 dwTimeMsLow = 0;
    DWORD32 dwTimeMsHigh = 0;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    if (FAILED(pCmdLink->ReceiveData((PBYTE8)&dwTimeMsLow, 4, NULL)))
    {
        return E_NETFAIL;
    }
    if (FAILED(pCmdLink->ReceiveData((PBYTE8)&dwTimeMsHigh, 4, NULL)))
    {
        return E_NETFAIL;
    }

    HRESULT hr = S_OK;
    if ( S_OK != SetSystemTime(dwTimeMsLow, dwTimeMsHigh) )
    {
        hr = -1;
    }
    if ( 0 != PciSendClockSyncData(dwTimeMsLow, dwTimeMsHigh) )
    {
        hr = (S_OK == hr) ? (-2) : (-3);
    }

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_SET_TIME_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = hr;

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int GetParamCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    HRESULT hr = E_FAIL;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    CFastMemAlloc cMasterStack;
    CFastMemAlloc cSlaveStack;
    CFastMemAlloc cStack;
    BYTE8* pbMasterXmlBuf;
    BYTE8* pbSlaveXmlBuf;
    BYTE8* pbXmlBuf;
    DWORD32 dwMasterXmlLen = XML_PARAM_FILE_LEN + 1;
    DWORD32 dwSlaveXmlLen = XML_PARAM_FILE_LEN + 1;
    DWORD32 dwXmlLen = XML_PARAM_FILE_LEN * 2 + 1;

    //申请内存
    pbMasterXmlBuf = (BYTE8*)cMasterStack.StackAlloc((int)dwMasterXmlLen, FALSE);
    pbSlaveXmlBuf = (BYTE8*)cSlaveStack.StackAlloc((int)dwSlaveXmlLen, FALSE);
    pbXmlBuf = (BYTE8*)cStack.StackAlloc((int)dwXmlLen, FALSE);
    if ( pbMasterXmlBuf == NULL || pbSlaveXmlBuf == NULL || pbXmlBuf == NULL )
    {
        return E_OUTOFMEMORY;
    }

    HV_memset(pbMasterXmlBuf, 0, dwMasterXmlLen);
    HV_memset(pbSlaveXmlBuf, 0, dwSlaveXmlLen);
    HV_memset(pbXmlBuf, 0, dwXmlLen);

    //读取主参数
    hr = GetParamXml(&g_cParamStore, (char*)pbMasterXmlBuf, dwMasterXmlLen);
    if ( S_OK == hr )
    {
        //读取从参数
        hr = g_cHvPciLinkApi.SendData(
                 PCILINK_PARAM_GET_DATA,
                 NULL,
                 0,
                 pbSlaveXmlBuf,
                 (int*)&dwSlaveXmlLen,
                 6000
             );

        if ( S_OK == hr )
        {
            //将主从参数合并
            dwXmlLen = XML_PARAM_FILE_LEN * 2 + 1;
            hr = CombineMasterSlaveXmlParam(
                     (const char*)pbMasterXmlBuf,
                     (const char*)pbSlaveXmlBuf,
                     (char*)pbXmlBuf,
                     dwXmlLen
                 );
        }
    }

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_GET_PARAM_CMD;
    cRespond.dwInfoSize = dwXmlLen;
    cRespond.dwResult = hr;

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)) )
    {
        return E_NETFAIL;
    }

    if ( 0 == cRespond.dwInfoSize )
    {
        return S_OK;
    }

    //发送回应数据
    if ( FAILED(pCmdLink->SendData((PBYTE8)pbXmlBuf, cRespond.dwInfoSize)) )
    {
        return E_NETFAIL;
    }

    return S_OK;
}

static bool ExistCamAppSection(TiXmlElement* pRootElementAll)
{
    TiXmlElement* pHvParamElement = pRootElementAll->FirstChildElement();

    if ( pHvParamElement )
    {
        TiXmlElement* pSectionElement = pHvParamElement->FirstChildElement();
        while (pSectionElement)
        {
            if ( 0 == strcmp("CamApp", pSectionElement->Attribute("name")) )
            {
                return true;
            }

            pSectionElement = pSectionElement->NextSiblingElement();
        }
    }

    return false;
}

static HRESULT SaveMasterParam(TiXmlDocument& cXmlDocAll)
{
    HRESULT hr = E_FAIL;

    TiXmlElement* pRootElementAll = cXmlDocAll.RootElement();
    if (pRootElementAll)
    {
        TiXmlElement* pHvParamElement = pRootElementAll->FirstChildElement();
        if (pHvParamElement)
        {
            TiXmlElement* pSectionElementTmp = NULL;
            TiXmlElement* pSectionElement = pHvParamElement->FirstChildElement();
            while (pSectionElement)
            {
                pSectionElementTmp = pSectionElement;
                pSectionElement = pSectionElement->NextSiblingElement();

                if ( 0 != strcmp("CamApp", pSectionElementTmp->Attribute("name")) )
                {
                    // 主CPU端只保存CamApp节的参数，因此其余的都要去掉。
                    pHvParamElement->RemoveChild(pSectionElementTmp);
                }
            }

            TiXmlPrinter cPrinter;
            cXmlDocAll.Accept(&cPrinter);
            if ( S_OK == SetParamXml(&g_cParamStore, (char*)cPrinter.CStr()) )
            {
                hr = S_OK;
                DebugPrintf((const char*)cPrinter.CStr(), 512, "MasterXmlParam");
            }
        }
    }

    return hr;
}

int SetParamCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    HRESULT hr = E_FAIL;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    CFastMemAlloc cStack;
    BYTE8* pbXmlBuf;
    DWORD32 dwXmlLen = XML_PARAM_FILE_LEN * 2 + 1;

    //申请内存
    pbXmlBuf = (BYTE8*)cStack.StackAlloc((int)dwXmlLen, FALSE);
    if ( pbXmlBuf == NULL )
    {
        return E_OUTOFMEMORY;
    }

    HV_memset(pbXmlBuf, 0, dwXmlLen);

    if (FAILED(pCmdLink->ReceiveData(pbXmlBuf, pCmdHeader->dwInfoSize, NULL)))
    {
        return E_NETFAIL;
    }

    if ( 0 == strlen((const char*)pbXmlBuf) )
    {
        // 参数“恢复出厂设置”
        if ( 0 == PciSendParamSetData(NULL, 0) )
        {
            hr = RestoreFactoryParam();
        }
        else
        {
            hr = E_FAIL;
        }
    }
    else
    {
        TiXmlDocument cXmlDocAll;
        if ( cXmlDocAll.Parse((const char*)pbXmlBuf) )
        {
            TiXmlElement* pRootElementAll = cXmlDocAll.RootElement();
            if ( pRootElementAll )
            {
                if ( NULL != pRootElementAll->FirstChildElement("CameraOnly") )
                {
                    // 存在CameraOnly结点，表示：只保存主CPU端的参数
                    hr = SaveMasterParam(cXmlDocAll);
                    HV_Trace(5, "Only Save Camera Param.\n");
                }
                else if ( false == ExistCamAppSection(pRootElementAll) )
                {
                    // 不存在属性名为CamApp的Section，表示：参数“恢复默认设置”
                    char szParamMode[32] = {0};
                    strcpy(szParamMode, "RestoreDefaultParam");
                    if ( 0 == PciSendParamSetData((unsigned char*)szParamMode, strlen(szParamMode) + 1) )
                    {
                        hr = RestoreDefaultParam();
                    }
                }
                else  // 正常保存所有参数
                {
                    if ( 0 == PciSendParamSetData(pbXmlBuf, strlen((char*)pbXmlBuf)) )
                    {
                        hr = SaveMasterParam(cXmlDocAll);
                        HV_Trace(5, "Normal Save Param.\n");
                    }
                }
            }
        }
    }

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_SET_PARAM_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = hr;

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int UpdateControllPannelCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    CFastMemAlloc cStack;
    BYTE8* pbUpdateFileBuffer;
    DWORD32 dwUpdateFileLen = pCmdHeader->dwInfoSize;

    //申请内存
    pbUpdateFileBuffer = (BYTE8*)cStack.StackAlloc((int)dwUpdateFileLen, FALSE);
    if ( pbUpdateFileBuffer == NULL )
    {
        return E_OUTOFMEMORY;
    }

    HV_memset(pbUpdateFileBuffer, 0, dwUpdateFileLen);

    if (FAILED(pCmdLink->ReceiveData(pbUpdateFileBuffer, pCmdHeader->dwInfoSize, NULL)))
    {
        return E_NETFAIL;
    }

    if(g_pCamApp->UpdateControllPannel(pbUpdateFileBuffer) != S_OK)
    {
       return E_FAIL;
    }

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_UPDATE_CONTROLL_PANNEL;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = 0;

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }
    return S_OK;
}

int SetCharacterValueCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    HRESULT hr = E_FAIL;
    if ( pCmdHeader == NULL || pCmdLink == NULL )
    {
        return E_INVALIDARG;
    }

    CFastMemAlloc cStack;
    BYTE8* pbBuffer;
    DWORD32 dwBufferLen = pCmdHeader->dwInfoSize;

    //申请内存
    pbBuffer = (BYTE8*)cStack.StackAlloc((int)dwBufferLen, FALSE);
    if ( pbBuffer == NULL )
    {
        return E_OUTOFMEMORY;
    }

    HV_memset(pbBuffer, 0, dwBufferLen);

    if (FAILED(pCmdLink->ReceiveData(pbBuffer, dwBufferLen, NULL)))
    {
        return E_NETFAIL;
    }

    typedef struct CharacterInfo_t {
        int iX;
        int iY;
        int iFontSize;
        int iDateFormat;
        int iFontColor;
    }CharacterInfo;

    CharacterInfo cInfo;
    memcpy(&cInfo, pbBuffer, sizeof(CharacterInfo));

    pbBuffer += sizeof(CharacterInfo);
    char* pLattice = (char *)pbBuffer;
    g_cModuleParams.cCamAppParam.iX = cInfo.iX;
    g_cModuleParams.cCamAppParam.iY = cInfo.iY;
    g_cModuleParams.cCamAppParam.iFontSize = cInfo.iFontSize;
    g_cModuleParams.cCamAppParam.iDateFormat = cInfo.iDateFormat;
    g_cModuleParams.cCamAppParam.iFontColor = cInfo.iFontColor;
    g_cModuleParams.cCamAppParam.iLatticeLen = pCmdHeader->dwInfoSize - sizeof(CharacterInfo);

    hr = g_cCameraController.SetCharacterOverlap(pLattice);

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_SET_CHARACTER;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = hr;

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    HV_Trace(5, "SetCharacterValueCmd OK!\n");

    return 0;
}

int SetFpsModeCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    int iFpsMode = 0;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    if (FAILED(pCmdLink->ReceiveData((PBYTE8)&iFpsMode, 4, NULL)))
    {
        return E_NETFAIL;
    }

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_SET_FPS_MODE_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = g_cCameraController.SetFpsMode(iFpsMode, TRUE);

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int SetRegCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    char pbRegInfo[8] = {0};
    DWORD32 addr = 0;
    DWORD32 data = 0;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    if (FAILED(pCmdLink->ReceiveData((PBYTE8)pbRegInfo, 8, NULL)))
    {
        return E_NETFAIL;
    }

    memcpy(&addr, pbRegInfo, 4);
    memcpy(&data, pbRegInfo+4, 4);

    Trace("SetRegCmd:[addr=0x%x, data=0x%x]\n", addr,data);

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_SET_REG_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = g_cCameraController.SetRegDirect(addr, data);

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int SoftTriggerCaptureCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_SOFT_TRIGGER_CAPTURE_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = g_cCameraController.SoftTriggerCapture();

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int DebugSetSomeValueCmd( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    struct
    {
        DWORD32 dwType;
        DWORD32 dwValue;
    } cDebugParam;

    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    if ( FAILED(pCmdLink->ReceiveData((PBYTE8)&cDebugParam, sizeof(cDebugParam), NULL)) )
    {
        return E_NETFAIL;
    }

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_DEBUG_SET_SOME_VALUE_CMD;
    cRespond.dwInfoSize = 0;
    cRespond.dwResult = g_pCamApp->DynChangeParam((DCP_TYPE)cDebugParam.dwType, cDebugParam.dwValue);

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)))
    {
        return E_NETFAIL;
    }

    return S_OK;
}

int GetResetReport( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_INVALIDARG;

    CAMERA_CMD_RESPOND cRespond;
    cRespond.dwID = CAMERA_GET_RESET_REPORT_CMD;

    static char szResetReport[4*1024];
    if ( 0 == ReadResetReport((char*)szResetReport, 4 * 1024) )
    {
        cRespond.dwInfoSize = strlen(szResetReport);
        cRespond.dwResult = S_OK;
    }
    else
    {
        cRespond.dwInfoSize = 0;
        cRespond.dwResult = S_FALSE;
    }

    //发送回应包
    if ( FAILED(pCmdLink->SendRespond(&cRespond)) )
    {
        return E_NETFAIL;
    }

    //发送回应数据
    if ( FAILED(pCmdLink->SendData((PBYTE8)szResetReport, cRespond.dwInfoSize)) )
    {
        return E_NETFAIL;
    }

    return S_OK;
}

#endif // SINGLE_BOARD_PLATFORM

//---------------------============== CCameraCmdProcess ==================-------------------------

CCameraCmdProcess::CCameraCmdProcess()
        : m_pCmdMap(NULL)
        , m_iCmdCount(0)
{
}

CCameraCmdProcess::~CCameraCmdProcess()
{
}

HRESULT CCameraCmdProcess::Initialize(CAMERA_CMD_MAP_ITEM* pCmdMap, const int& iItemCount)
{
    m_pCmdMap = pCmdMap;
    m_iCmdCount = iItemCount;

    return S_OK;
}

HRESULT CCameraCmdProcess::Process( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink )
{
    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_POINTER;

    return ExecuteCommand(pCmdHeader, pCmdLink);
}

HRESULT CCameraCmdProcess::ExecuteCommand(CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink)
{
    if ( pCmdHeader == NULL || pCmdLink == NULL ) return E_POINTER;

    //初始为 UNKNOW 命令
    CAMERA_CMD_FUNC fp = m_pCmdMap[0].fpCmdFunc;
    for ( int i = 0; i < m_iCmdCount; i++)
    {
        if ( m_pCmdMap[i].dwCmdID == pCmdHeader->dwID )
        {
            fp = m_pCmdMap[i].fpCmdFunc;
            break;
        }
    }

    if (fp == NULL) return E_NOTIMPL;

    return (*fp)(pCmdHeader, pCmdLink);
}

HRESULT CCameraCmdProcess::CreateICameraCmdProcess(ICameraCmdProcess** ppICameraCmdProcess)
{
    if ( ppICameraCmdProcess == NULL ) return E_POINTER;

    CCameraCmdProcess* pProcess = new CCameraCmdProcess;
    if ( pProcess == NULL ) return E_INVALIDARG;
    pProcess->Initialize(g_CameraCmdFuncMap, sizeof(g_CameraCmdFuncMap)/sizeof(g_CameraCmdFuncMap[0]));
    *ppICameraCmdProcess = (ICameraCmdProcess*)pProcess;

    return S_OK;
}

// --------------------------------------------------------------------

static HRESULT SetIP(TcpipParam& cTcpipParam)
{
    DWORD32 dwIP = 0;
    DWORD32 dwMask = 0;
    DWORD32 dwGateway = 0;

    MyGetIpDWord(cTcpipParam.szIp, dwIP);
    MyGetIpDWord(cTcpipParam.szNetmask, dwMask);
    MyGetIpDWord(cTcpipParam.szGateway, dwGateway);
    dwIP = htonl(dwIP);
    dwMask = htonl(dwMask);
    dwGateway = htonl(dwGateway);

    HRESULT hr = E_FAIL;
#ifdef SINGLE_BOARD_PLATFORM
    if (IsTestCamLanPort()) // 判断是否测试相机口（生产测试时使用）
    {
        hr = SetTcpipAddr_1(&cTcpipParam);  // 单板平台左边
    }
    else
    {
        hr = SetTcpipAddr_2(&cTcpipParam);  // 单板平台右边（即：靠近复位按钮）那个网口
        dwIP = ntohl(dwIP);
        dwMask = ntohl(dwMask);
        HRESULT hrOK = ChangeCamTransmit(dwIP, dwMask);
        if (S_OK != hrOK && S_FALSE != hrOK)
        {
            HV_Trace(5, "ChangeCamTransmit Failed !\n");
        }
        dwIP = htonl(dwIP);
        dwMask = htonl(dwMask);
    }
#else
    hr = SetTcpipAddr_0(&cTcpipParam);  // 设置IP
#endif

    if ( hr == S_OK )
    {
        if ( S_OK == SetUbootNetAddr(dwIP, dwMask, dwGateway) ) // 设置Uboot网络地址
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

            if ( S_OK == g_cParamStore.Save() ) // 保存到EEPROM参数集
            {
                return hr;
            }
        }
    }

    return E_FAIL;
}
