#include "hvutils.h"
#include "tinyxml.h"
#include "HvTime.h"
#include "ControlFunc.h"
#include "CameraLinkBase.h"
#include "hvsysinterface.h"
#include "HvParamIO.h"
#include "HvUtilsClass.h"
#include "CameraController.h"
#include "CharacterOverlap.h"

#ifndef SINGLE_BOARD_PLATFORM
#include "../CamApp/DataCtrl.h"
#include "../CamApp/LoadParam.h"
#else
#include "CamTransmit.h"
#endif // SINGLE_BOARD_PLATFORM

/* LoadParam.cpp */
extern CParamStore g_cParamStore;

/* Hvtarget_ARM.c */
extern "C" int SetUbootNetAddr(DWORD32 dwIP, DWORD32 dwNetmask, DWORD32 dwGateway);
extern "C" int GetResetCount(int* piResetCount);
extern "C" int ReadResetReport(char* szResetReport, const int iLen);
extern "C" int GetSN(char* pbSerialNo, int iLen);
extern "C" int GetCpuNearTemp(int* piTemp);

CmdInfoFirstRetAfterExec g_cCmdInfoFirstRetAfterExec;

using namespace HvSys;

// HvXml协议实现

// -------------------- HvXml协议简易示悿------------------------

/* HvCmd 例子ﺿ<<xml version="1.0" encoding="GB2312" standalone="yes" ?>
<HvCmd ver="1.0">
	<CmdName>SetTime</CmdName>
	<CmdArg>
		<Value id="Date">2011.06.01</Value>
		<Value id="Time">00:00:00</Value>
	</CmdArg>
</HvCmd>

<?xml version="1.0" encoding="GB2312" standalone="yes" ?>
<HvCmdRespond ver="1.0">
	<RetCode>0</RetCode>
</HvCmdRespond>
*/

/* HvCmd 例子2ﺿ<?xml version="1.0" encoding="GB2312" standalone="yes" ?>
<HvCmd ver="1.0">
	<CmdName>GetWorkModeIndex</CmdName>
	<CmdArg>
		<Value id="WorkModeIndex">1</Value>
	</CmdArg>
</HvCmd>

<?xml version="1.0" encoding="GB2312" standalone="yes" ?>
<HvCmdRespond ver="1.0">
    <RetCode>0</RetCode>
    <RetMsg>
        <Value id="WorkModeName">TrafficGate</Value>
    </RetMsg>
</HvCmdRespond>
*/

/* HvInfo 例子ﺿ<?xml version="1.0" encoding="GB2312" standalone="yes" ?>
<HvInfo ver="1.0">
	<Info name="DateTime" />
	<Info name="HvName" />
</HvInfo>

<?xml version="1.0" encoding="GB2312" standalone="yes" ?>
<HvInfoRespond ver="1.0">
	<Info name="DateTime">
		<Value name="Date">2011.06.01</Value>
		<Value name="Time">00:00:00</Value>
	</Info>
	<Info name="HvName">
		<Value name="Name">Video</Value>
	</Info>
</HvInfoRespond>
*/

// --------------------------------------------------------------

/*
发送：

<?xml version="1.0" encoding="GB2312" standalone="yes" ?>
<HvCmd ver="2.0">
       <CmdName date="2011.06.01" time="00:00:00">SetTime</CmdName>                ----//TiXmlElement* pCmdArgElement
       <CmdName>GetTime</CmdName>
</HvCmd>

返回ﺿ<?xml version="1.0" encoding="GB2312" standalone="yes" ?>
<HvCmdRespond ver="2.0">
       <CmdName RetCode="0">SetTime</CmdName>
       <CmdName RetCode="0" date="2011.06.01" time="00:00:00">GetTime</CmdName>
</HvCmdRespond>
*/

#define HV_XML_VERSION "2.0"  // DM6467一体机设备

// HvXml协议结点宏定义
#define HX_VER "ver"
#define HX_HVCMD "HvCmd"
#define HX_CMDNAME "CmdName"
#define HX_CMDARG "CmdArg"
#define HX_HVCMDRESPOND "HvCmdRespond"
#define HX_RETCODE "RetCode"
#define HX_RETMSG "RetMsg"
#define HX_HVINFO "HvInfo"
#define HX_HVINFORESPOND "HvInfoRespond"
#define HX_VALUE "Value"
#define HX_ID "id"
#define HX_INFO "Info"
#define HX_NAME "name"
#define CONNECTED_IP_MAX_COUNT 16

// 解析执行HvCmd
HRESULT HvXml_Cmd(const TiXmlDocument cXmlDoc, TiXmlElement* pRootElementRet);

// HvCmd类型详细实现
HRESULT HvXml_SetTime(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetWorkModeIndex(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetIP(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetWorkMode(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_ForceSend(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetShutter(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetShutter(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetGain(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetGain(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetRgbGain(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetRgbGain(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetCaptureShutter(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetCaptureShutter(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetCaptureGain(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetCaptureGain(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetCaptureRgbGain(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetCaptureRgbGain(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SoftTriggerCapture(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetEncodeMode(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetENetSyn(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetENetSyn(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetAgcLightBaseline(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetAgcLightBaseline(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetGammaData(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetGammaData(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetAGCZone(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetAGCZone(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetCaptureEdge(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetCaptureEdge(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetAGCEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetAGCEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetAWBEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetAWBEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetAGCParam(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetAGCParam(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetJpegCompressRate(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetJpegCompressRate(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetJpegCompressRateCapture(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetJpegCompressRateCapture(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetFlashRateSynSignalEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetFlashRateSynSignalEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetCaptureSynSignalEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetCaptureSynSignalEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_ResetDevice(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_RestoreDefaultParam(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_RestoreFactoryParam(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SaveADSamplingValue(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetControllPannelWorkStatus(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetControllPannelPulseWidthRange(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_TestControllPannelPLMode(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetFlashSingle(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SaveControllPannelConfig(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetControllPannelAutoRunStatus(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetControllPannelPulseLevel(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetControllPannelDefParam(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SaveParam(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetCharacterEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetCharacterEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetCharacterInfo(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SaveCharacterInfo(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_SetCharacterFixedLight(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_DoPartition(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_DoCheckDisk(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);

// HvInfo类型详细实现
HRESULT HvXml_GetDateTime(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetHvName(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetHvID(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetResetCount(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetWorkMode(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetWorkModeCount(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetResetMode(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetProductName(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetVideoCount(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetVersion(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetVersionString(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetConnectedIP(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetHddOpStatus(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetHddCheckReport(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetResetReport(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetDevType(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetCpuTemperature(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetEncodeMode(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetControllPannelVersion(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetControllPannelTemperature(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetControllPannelStatus(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetControllPannelPulseInfo(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetControllPannelDeviceStatus(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetControllPannelFlashInfo(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetControllPannelUpdatingStatus(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetControllPannelCRCValue(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetControllPannelAutoRunStatus(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetControllPannelAutoRunStyle(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);
HRESULT HvXml_GetCheckDisk(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);

//--------------------------------------------------------------------

// 限制：单个Id，Value或Name的长度不能超轿28字节⾿#define IVN_MAX_SIZE 128

// 工具函数：快速建立命令xml
int BuildHvCmdXml(
    char* pXmlBuf,
    char* pCmdName,
    int iArgCount,
    const char rgszName[][IVN_MAX_SIZE],
    const char rgszValue[][IVN_MAX_SIZE]
);

// 工具函数：快速建立命令返回xml
static HRESULT BuildHvCmdRetXml(
    TiXmlElement* pRootElementRet,
    const TiXmlElement* pCmdArgElement,
    const char* szRetCode,
    int iRetCount = 0,
    const char rgszId[][IVN_MAX_SIZE] = NULL,
    const char rgszValue[][IVN_MAX_SIZE] = NULL
);

// 工具函数
static HRESULT GetParamStringFromXml(
    const TiXmlElement* pCmdArgElement,
    const char *pszParamName,
    char *pszParamValue,
    int nParamValueSize
);

// 工具函数
static HRESULT GetParamIntFromXml(
    const TiXmlElement* pCmdArgElement,
    const char *pszParamName,
    int *pnParamValue
);

//-----------------------------------------------------------------

typedef HRESULT ( *HVXML_CMD_FUNC )(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet);

typedef struct tag_HvXmlCmdFunc
{
    char szCmdName[32];
    HVXML_CMD_FUNC fpCmdFunc;
} HVXML_CMD_MAP_ITEM;

#ifdef SINGLE_BOARD_PLATFORM
//single
//--------------------------------------------------------------------
HVXML_CMD_MAP_ITEM g_HvXmlCmdFuncMap[] =
{
    { "SetTime", HvXml_SetTime },
    { "GetWorkModeIndex", HvXml_GetWorkModeIndex },
    { "SetIP", HvXml_SetIP },
    { "SetOptWorkMode", HvXml_SetWorkMode },
    { "ForceSend", HvXml_ForceSend },
    { "ResetDevice", HvXml_ResetDevice },
    { "RestoreDefaultParam", HvXml_RestoreDefaultParam },
    { "RestoreFactoryParam", HvXml_RestoreFactoryParam },

    //原Info
    { "DateTime", HvXml_GetDateTime },
    { "HvName", HvXml_GetHvName },
    { "HvID", HvXml_GetHvID },
    { "OptResetCount", HvXml_GetResetCount },
    { "OptWorkMode", HvXml_GetWorkMode },
    { "OptWorkModeCount", HvXml_GetWorkModeCount },
    { "OptResetMode", HvXml_GetResetMode },
    { "OptProductName", HvXml_GetProductName },
    { "GetVideoCount", HvXml_GetVideoCount },
    { "GetVersion", HvXml_GetVersion },
    { "GetVersionString", HvXml_GetVersionString },
    { "GetConnectedIP", HvXml_GetConnectedIP },
    { "GetHddOpStatus", HvXml_GetHddOpStatus },
    { "GetHddCheckReport", HvXml_GetHddCheckReport },
    { "GetResetReport", HvXml_GetResetReport },
    { "GetDevType", HvXml_GetDevType },
    { "DoPartition", HvXml_DoPartition },
    { "DoCheckDisk", HvXml_DoCheckDisk },
    { "GetCheckDisk", HvXml_GetCheckDisk },
};
static int g_HvXmlCmdMapItemCount = sizeof(g_HvXmlCmdFuncMap)/sizeof(g_HvXmlCmdFuncMap[0]);

HRESULT HvXml_GetWorkModeIndex(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;
    int nWorkMode = -1;
    char rgszId[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];

    strcpy(rgszId[0], "WorkModeName");
    strcpy(rgszValue[0], "UNKNOWN");
    if (GetParamIntFromXml(pCmdArgElement, "WorkModeIndex", &nWorkMode) == S_OK)
    {
        SYS_INFO cInfo;
        cInfo.Info.WorkModeList.nMode = nWorkMode;
        cInfo.Info.WorkModeList.pbListBuf = rgszValue[0];

        hr = GetWorkModeList(&cInfo);

        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, ((hr == S_OK) ? "0" : "-1"), 1, rgszId, rgszValue);
    }

    return E_FAIL;
}

HRESULT HvXml_SetWorkMode(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iWorkMode = -1;

    if ( S_OK == GetParamIntFromXml(pCmdArgElement, "WorkMode", &iWorkMode) )
    {
        if ( S_OK == SetWorkMode((DWORD32)iWorkMode) )
        {
            g_cCmdInfoFirstRetAfterExec.iResetMode = 3;
            g_cCmdInfoFirstRetAfterExec.fHvXmlResetDeviceValid = true;
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_ForceSend(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    DWORD32 dwVideoID = 0;

    if ( S_OK == ForceSend(dwVideoID) )
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
    }
    else
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
}

HRESULT HvXml_GetWorkMode(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    int nValue = -1;
    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];
    strcpy(rgszName[0], "WorkMode");

    SYS_INFO cInfo;
    cInfo.Info.WorkMode.dwMode = (DWORD32)-1;
    cInfo.Info.WorkMode.dwModeCount = 0;
    GetWorkModeInfo(&cInfo);
    nValue = cInfo.Info.WorkMode.dwMode;

    sprintf(rgszValue[0], "%d", nValue);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             1,
             rgszName,
             rgszValue
         );
    return hr;
}

HRESULT HvXml_GetWorkModeCount(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    int nValue = 0;
    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];
    strcpy(rgszName[0], "WorkModeCount");

    SYS_INFO cInfo;
    cInfo.Info.WorkMode.dwMode = (DWORD32)-1;
    cInfo.Info.WorkMode.dwModeCount = 0;
    GetWorkModeInfo(&cInfo);
    nValue = cInfo.Info.WorkMode.dwModeCount;

    sprintf(rgszValue[0], "%d", nValue);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             1,
             rgszName,
             rgszValue
         );
    return hr;
}

#else
//double
//--------------------------------------------------------------------
HVXML_CMD_MAP_ITEM g_HvXmlCmdFuncMap[] =
{
    { "SetTime", HvXml_SetTime },
    { "GetWorkModeIndex", HvXml_GetWorkModeIndex },
    { "SetIP", HvXml_SetIP },
    { "SetOptWorkMode", HvXml_SetWorkMode },
    { "ForceSend", HvXml_ForceSend },
    { "SetShutter", HvXml_SetShutter },
    { "GetShutter", HvXml_GetShutter },
    { "SetGain", HvXml_SetGain },
    { "GetGain", HvXml_GetGain },
    { "SetRgbGain", HvXml_SetRgbGain },
    { "GetRgbGain", HvXml_GetRgbGain },
    { "SetCaptureShutter", HvXml_SetCaptureShutter },
    { "GetCaptureShutter", HvXml_GetCaptureShutter },
    { "SetCaptureGain", HvXml_SetCaptureGain },
    { "GetCaptureGain", HvXml_GetCaptureGain },
    { "SetCaptureRgbGain", HvXml_SetCaptureRgbGain },
    { "GetCaptureRgbGain", HvXml_GetCaptureRgbGain },
    { "SoftTriggerCapture", HvXml_SoftTriggerCapture },
    { "SetEncodeMode", HvXml_SetEncodeMode },
    { "SetENetSyn", HvXml_SetENetSyn },
    { "GetENetSyn", HvXml_GetENetSyn },
    { "SetAgcLightBaseline", HvXml_SetAgcLightBaseline },
    { "GetAgcLightBaseline", HvXml_GetAgcLightBaseline },
    { "SetGammaData", HvXml_SetGammaData },
    { "GetGammaData", HvXml_GetGammaData },
    { "SetAGCZone", HvXml_SetAGCZone },
    { "GetAGCZone", HvXml_GetAGCZone },
    { "SetCaptureEdge", HvXml_SetCaptureEdge },
    { "GetCaptureEdge", HvXml_GetCaptureEdge },
    { "SetAGCEnable", HvXml_SetAGCEnable },
    { "GetAGCEnable", HvXml_GetAGCEnable },
    { "SetAWBEnable", HvXml_SetAWBEnable },
    { "GetAWBEnable", HvXml_GetAWBEnable },
    { "SetAGCParam", HvXml_SetAGCParam },
    { "GetAGCParam", HvXml_GetAGCParam },
    { "SetJpegCompressRate", HvXml_SetJpegCompressRate },
    { "GetJpegCompressRate", HvXml_GetJpegCompressRate },
    { "SetJpegCompressRateCapture", HvXml_SetJpegCompressRateCapture },
    { "GetJpegCompressRateCapture", HvXml_GetJpegCompressRateCapture },
    { "SetFlashRateSynSignalEnable", HvXml_SetFlashRateSynSignalEnable },
    { "GetFlashRateSynSignalEnable", HvXml_GetFlashRateSynSignalEnable },
    { "SetCaptureSynSignalEnable", HvXml_SetCaptureSynSignalEnable },
    { "GetCaptureSynSignalEnable", HvXml_GetCaptureSynSignalEnable },
    { "ResetDevice", HvXml_ResetDevice },
    { "RestoreDefaultParam", HvXml_RestoreDefaultParam },
    { "RestoreFactoryParam", HvXml_RestoreFactoryParam },
    { "SaveADSamplingValue", HvXml_SaveADSamplingValue },
    { "SetControllPannelStatus", HvXml_SetControllPannelWorkStatus },
    { "SetPulseWidthRange", HvXml_SetControllPannelPulseWidthRange },
    { "TestPolarizingPrismMode", HvXml_TestControllPannelPLMode },
    { "SetFlashSingle", HvXml_SetFlashSingle },
    { "SaveControllPannelConfig", HvXml_SaveControllPannelConfig },
    { "SetControllPannelAutoRunStatus", HvXml_SetControllPannelAutoRunStatus },
    { "SetControllPannelPulseLevel", HvXml_SetControllPannelPulseLevel },
    { "SetControllPannelDefParam", HvXml_SetControllPannelDefParam },
    { "SaveParam", HvXml_SaveParam },
    { "SetCharacterEnable", HvXml_SetCharacterEnable },
    { "GetCharacterEnable", HvXml_GetCharacterEnable },
    { "GetCharacterInfo", HvXml_GetCharacterInfo },
    { "SaveCharacterInfo", HvXml_SaveCharacterInfo },
    { "SetCharacterFixedLight", HvXml_SetCharacterFixedLight},

    //原Info
    { "DateTime", HvXml_GetDateTime },
    { "HvName", HvXml_GetHvName },
    { "HvID", HvXml_GetHvID },
    { "OptResetCount", HvXml_GetResetCount },
    { "OptWorkMode", HvXml_GetWorkMode },
    { "OptWorkModeCount", HvXml_GetWorkModeCount },
    { "OptResetMode", HvXml_GetResetMode },
    { "OptProductName", HvXml_GetProductName },
    { "GetVideoCount", HvXml_GetVideoCount },
    { "GetVersion", HvXml_GetVersion },
    { "GetVersionString", HvXml_GetVersionString },
    { "GetConnectedIP", HvXml_GetConnectedIP },
    { "GetHddOpStatus", HvXml_GetHddOpStatus },
    { "GetHddCheckReport", HvXml_GetHddCheckReport },
    { "GetResetReport", HvXml_GetResetReport },
    { "GetDevType", HvXml_GetDevType },
    { "GetCpuTemperature", HvXml_GetCpuTemperature },
    { "GetEncodeMode", HvXml_GetEncodeMode },
    { "GetControllPannelVersion", HvXml_GetControllPannelVersion },
    { "GetControllPannelTemperature", HvXml_GetControllPannelTemperature },
    { "GetControllPannelStatus", HvXml_GetControllPannelStatus },
    { "GetControllPannelPulseInfo", HvXml_GetControllPannelPulseInfo },
    { "GetControllPannelDeviceStatus", HvXml_GetControllPannelDeviceStatus },
    { "GetControllPannelFlashInfo", HvXml_GetControllPannelFlashInfo },
    { "GetControllPannelUpdatingStatus", HvXml_GetControllPannelUpdatingStatus },
    { "GetControllPannelCRCValue", HvXml_GetControllPannelCRCValue },
    { "GetControllPannelAutoRunStatus", HvXml_GetControllPannelAutoRunStatus },
    { "GetControllPannelAutoRunStle", HvXml_GetControllPannelAutoRunStyle },
    { "DoPartition", HvXml_DoPartition },
    { "DoCheckDisk", HvXml_DoCheckDisk },
    { "GetCheckDisk", HvXml_GetCheckDisk },
};
static int g_HvXmlCmdMapItemCount = sizeof(g_HvXmlCmdFuncMap)/sizeof(g_HvXmlCmdFuncMap[0]);


HRESULT HvXml_GetWorkModeIndex(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;
    int nWorkMode = -1, nReturnSize = IVN_MAX_SIZE;
    char rgszId[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];

    strcpy(rgszId[0], "WorkModeName");
    strcpy(rgszValue[0], "UNKNOWN");
    if (GetParamIntFromXml(pCmdArgElement, "WorkModeIndex", &nWorkMode) == S_OK)
    {
        hr = g_cHvPciLinkApi.SendData(
                 PCILINK_GET_WORKMODEINDEX,
                 &nWorkMode,
                 sizeof(int),
                 rgszValue[0],
                 &nReturnSize
             );
        if (S_OK == hr)
        {
            rgszValue[0][nReturnSize] = '\0';
        }

        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, ((hr == S_OK) ? "0" : "-1"), 1, rgszId, rgszValue);
    }

    return E_FAIL;
}

HRESULT HvXml_SetWorkMode(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iWorkMode = -1;

    if ( S_OK == GetParamIntFromXml(pCmdArgElement, "WorkMode", &iWorkMode) )
    {
        HRESULT hr = E_FAIL;
        int nSize = sizeof(hr);
        if ( S_OK == g_cHvPciLinkApi.SendData(PCILINK_SET_WORKMODE, &iWorkMode, 4, &hr, &nSize, 20000)
                && hr == S_OK )
        {
            g_cCmdInfoFirstRetAfterExec.iResetMode = 3;
            g_cCmdInfoFirstRetAfterExec.fHvXmlResetDeviceValid = true;
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_ForceSend(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    DWORD32 dwVideoID = 0;

    HRESULT hr = E_FAIL;
    int nSize = sizeof(hr);
    if ( S_OK == g_cHvPciLinkApi.SendData(PCILINK_FORCESEND, &dwVideoID, 4, &hr, &nSize)
            && hr == S_OK )
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
    }
    else
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
}

HRESULT HvXml_SetShutter(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = 0;

    if (GetParamIntFromXml(pCmdArgElement, "Shutter", &iValue) == S_OK)
    {
        if ( 0 == g_cCameraController.SetShutter_Camyu(iValue) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_GetShutter(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = 0;

    if (0 == g_cCameraController.GetShutter_Camyu(iValue))
    {
        char rgszName[1][IVN_MAX_SIZE];
        char rgszValue[1][IVN_MAX_SIZE];

        strcpy(rgszName[0], "value");
        sprintf(rgszValue[0], "%d", iValue);

        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 1, rgszName, rgszValue);
    }
    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");;
}

HRESULT HvXml_SetGain(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = 0;

    if (GetParamIntFromXml(pCmdArgElement, "Gain", &iValue) == S_OK)
    {
        if ( 0 == g_cCameraController.SetGain_Camyu(iValue) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_GetGain(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = 0;

    if (0 == g_cCameraController.GetGain_Camyu(iValue))
    {
        char rgszName[1][IVN_MAX_SIZE];
        char rgszValue[1][IVN_MAX_SIZE];

        strcpy(rgszName[0], "value");
        sprintf(rgszValue[0], "%d", iValue);

        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 1, rgszName, rgszValue);
    }
    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
}

HRESULT HvXml_SetRgbGain(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iGainR = 0;
    int iGainG = 0;
    int iGainB = 0;

    if ( GetParamIntFromXml(pCmdArgElement, "GainR", &iGainR) == S_OK
            && GetParamIntFromXml(pCmdArgElement, "GainG", &iGainG) == S_OK
            && GetParamIntFromXml(pCmdArgElement, "GainB", &iGainB) == S_OK )
    {
        //由于使能AWB时,当R或者B为0时,AWB不进行调节,所以当R或B为0时，强制设置为1
        if (iGainR == 0)
        {
            iGainR = 1;
        }
        if (iGainB == 0)
        {
            iGainB = 1;
        }
        if ( 0 == g_cCameraController.SetRgbGain_Camyu(iGainR, iGainG, iGainB) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_GetRgbGain(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iGainR, iGainG, iGainB;

    if (0 == g_cCameraController.GetRgbGain_Camyu(iGainR, iGainG, iGainB))
    {
        char rgszName[3][IVN_MAX_SIZE];
        char rgszValue[3][IVN_MAX_SIZE];

        strcpy(rgszName[0], "GainR");
        sprintf(rgszValue[0], "%d", iGainR);
        strcpy(rgszName[1], "GainG");
        sprintf(rgszValue[1], "%d", iGainG);
        strcpy(rgszName[2], "GainB");
        sprintf(rgszValue[2], "%d", iGainB);

        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 3, rgszName, rgszValue);
    }
    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
}

HRESULT HvXml_SetCaptureShutter(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = 0;
    int iEnable = 0;

    if ( GetParamIntFromXml(pCmdArgElement, "Shutter", &iValue) == S_OK
            && GetParamIntFromXml(pCmdArgElement, "Enable", &iEnable) == S_OK )
    {
        if ( 0 == g_cCameraController.SetCaptureShutter_Camyu(iValue, iEnable) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_GetCaptureShutter(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iShutter, fEnable;

    if (0 == g_cCameraController.GetCaptureShutter_Camyu(iShutter, fEnable))
    {
        char rgszName[2][IVN_MAX_SIZE];
        char rgszValue[2][IVN_MAX_SIZE];

        strcpy(rgszName[0], "Shutter");
        sprintf(rgszValue[0], "%d", iShutter);
        strcpy(rgszName[1], "Enable");
        sprintf(rgszValue[1], "%d", fEnable);

        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 2, rgszName, rgszValue);
    }
    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
}

HRESULT HvXml_SetCaptureGain(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = 0;
    int iEnable = 0;

    if ( GetParamIntFromXml(pCmdArgElement, "Gain", &iValue) == S_OK
            && GetParamIntFromXml(pCmdArgElement, "Enable", &iEnable) == S_OK )
    {
        if ( 0 == g_cCameraController.SetCaptureGain_Camyu(iValue, iEnable) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_GetCaptureGain(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iGain, fEnable;

    if (0 == g_cCameraController.GetCaptureGain_Camyu(iGain, fEnable))
    {
        char rgszName[2][IVN_MAX_SIZE];
        char rgszValue[2][IVN_MAX_SIZE];

        strcpy(rgszName[0], "Gain");
        sprintf(rgszValue[0], "%d", iGain);
        strcpy(rgszName[1], "Enable");
        sprintf(rgszValue[1], "%d", fEnable);

        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 2, rgszName, rgszValue);
    }
    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
}

HRESULT HvXml_SetCaptureRgbGain(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iGainR = 0;
    int iGainG = 0;
    int iGainB = 0;
    int iEnable = 0;

    if ( GetParamIntFromXml(pCmdArgElement, "GainR", &iGainR) == S_OK
            && GetParamIntFromXml(pCmdArgElement, "GainG", &iGainG) == S_OK
            && GetParamIntFromXml(pCmdArgElement, "GainB", &iGainB) == S_OK
            && GetParamIntFromXml(pCmdArgElement, "Enable", &iEnable) == S_OK )
    {
        if ( 0 == g_cCameraController.SetCaptureRgbGain_Camyu(iGainR, iGainG, iGainB, iEnable) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_GetCaptureRgbGain(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iGainR, iGainG, iGainB, fEnable;

    if (0 == g_cCameraController.GetCaptureRgbGain_Camyu(iGainR, iGainG, iGainB, fEnable))
    {
        char rgszName[4][IVN_MAX_SIZE];
        char rgszValue[4][IVN_MAX_SIZE];

        strcpy(rgszName[0], "GainR");
        sprintf(rgszValue[0], "%d", iGainR);
        strcpy(rgszName[1], "GainG");
        sprintf(rgszValue[1], "%d", iGainG);
        strcpy(rgszName[2], "GainB");
        sprintf(rgszValue[2], "%d", iGainB);
        strcpy(rgszName[3], "Enable");
        sprintf(rgszValue[3], "%d", fEnable);

        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 4, rgszName, rgszValue);
    }
    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");;
}


HRESULT HvXml_SoftTriggerCapture(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    if ( 0 == g_cCameraController.SoftTriggerCapture() )
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
    }
    else
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
}

HRESULT HvXml_SetEncodeMode(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
#ifdef _CAM_APP_
    int iEncodeMode = 0;

    if ( GetParamIntFromXml(pCmdArgElement, "EncodeMode", &iEncodeMode) == S_OK )
    {
        if (iEncodeMode < -1 && iEncodeMode > 2)
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
        if ( S_OK == g_cCameraController.DynChangeParam(DCP_CAM_OUTPUT, iEncodeMode) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }
#endif
    return E_FAIL;
}

HRESULT HvXml_SetENetSyn(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iEnable = 0;

    if ( GetParamIntFromXml(pCmdArgElement, "Enable", &iEnable) == S_OK )
    {
        if ( 0 == g_cCameraController.SetFpsMode(iEnable, TRUE) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_GetENetSyn(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = 0;

    if (0 == g_cCameraController.GetFpsMode(iValue))
    {
        char rgszName[1][IVN_MAX_SIZE];
        char rgszValue[1][IVN_MAX_SIZE];

        strcpy(rgszName[0], "value");
        sprintf(rgszValue[0], "%d", iValue);

        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 1, rgszName, rgszValue);
    }
    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");

}

HRESULT HvXml_SetAgcLightBaseline(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
#ifdef _CAM_APP_
    int iValue = 0;

    if ( GetParamIntFromXml(pCmdArgElement, "Value", &iValue) == S_OK )
    {
        if ( S_OK == g_cCameraController.DynChangeParam(DCP_AGC_TH, iValue) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }
#endif
    return E_FAIL;
}

HRESULT HvXml_GetAgcLightBaseline(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = g_cModuleParams.cCamAppParam.iAGCTh;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];

    strcpy(rgszName[0], "value");
    sprintf(rgszValue[0], "%d", iValue);

    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 1, rgszName, rgszValue);
}

HRESULT HvXml_SetGammaData(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int rgiDataXY[8][2];

    char szValueNameX[32];
    char szValueNameY[32];

    bool fValidParam = true;

    for ( int i = 0; i < 8; ++i )
    {
        sprintf(szValueNameX, "Point%d_X", i);
        sprintf(szValueNameY, "Point%d_Y", i);

        if ( GetParamIntFromXml(pCmdArgElement, szValueNameX, &rgiDataXY[i][0]) != S_OK
                || GetParamIntFromXml(pCmdArgElement, szValueNameY, &rgiDataXY[i][1]) != S_OK )
        {
            fValidParam = false;
            break;
        }
    }

    if ( fValidParam )
    {
        //保存gamma值
        memcpy(g_cModuleParams.cCamAppParam.rgiGamma, rgiDataXY, 64);
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        /*
        if ( 0 == g_cCameraController.SetGammaData(rgiDataXY) )
        {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }*/
    }

    return E_FAIL;
}

HRESULT HvXml_GetGammaData(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    char rgszName[16][IVN_MAX_SIZE];
    char rgszValue[16][IVN_MAX_SIZE];
    for ( int i = 0; i < 16; i+=2 )
    {
        sprintf(rgszName[i], "Point%d_X", i);
        sprintf(rgszValue[i], "%d", g_cModuleParams.cCamAppParam.rgiGamma[i/2][0]);
        sprintf(rgszName[i+1], "Point%d_Y", i);
        sprintf(rgszValue[i+1], "%d", g_cModuleParams.cCamAppParam.rgiGamma[i/2][1]);
    }
    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 16, rgszName, rgszValue);
}

HRESULT HvXml_SetAGCZone(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int rgiAGCZone[16];
    char szValueName[32];
    bool fValidParam = true;

    for ( int i = 0; i < 16; ++i )
    {
        sprintf(szValueName, "AGCZone%02d", i);
        if ( GetParamIntFromXml(pCmdArgElement, szValueName, &rgiAGCZone[i]) != S_OK )
        {
            fValidParam = false;
            break;
        }
    }

    if ( fValidParam )
    {
#ifdef _CAM_APP_
        if ( 0 == g_cCameraController.SetAGCZone(rgiAGCZone) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
#endif
    }

    return E_FAIL;
}

HRESULT HvXml_GetAGCZone(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
#ifdef _CAM_APP_
    int rgiAGCZone[16];
    if ( 0 == g_cCameraController.GetAGCZone(rgiAGCZone) )
    {
        char rgszName[16][IVN_MAX_SIZE];
        char rgszValue[16][IVN_MAX_SIZE];

        for (int i = 0; i < 16; i++)
        {
            sprintf(rgszName[i], "AGCZone%02d", i);
            sprintf(rgszValue[i], "%d", rgiAGCZone[i]);
        }
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 16, rgszName, rgszValue);
    }
#endif
    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
}

HRESULT HvXml_SetCaptureEdge(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = 0;

    if ( GetParamIntFromXml(pCmdArgElement, "Value", &iValue) == S_OK )
    {
        if ( S_OK == g_cCameraController.CaptureEdgeSelect(iValue) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_GetCaptureEdge(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    return E_FAIL;
}

HRESULT HvXml_SetAGCEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
#ifdef _CAM_APP_
    int iValue = 0;

    if ( GetParamIntFromXml(pCmdArgElement, "Value", &iValue) == S_OK )
    {
        if ( S_OK == g_cCameraController.DynChangeParam(DCP_ENABLE_AGC, iValue) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }
#endif
    return E_FAIL;
}

HRESULT HvXml_GetAGCEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = g_cModuleParams.cCamAppParam.iAGCEnable;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];

    strcpy(rgszName[0], "value");
    sprintf(rgszValue[0], "%d", iValue);

    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 1, rgszName, rgszValue);
}

HRESULT HvXml_SetAWBEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
#ifdef _CAM_APP_
    int iValue = 0;

    if ( GetParamIntFromXml(pCmdArgElement, "Value", &iValue) == S_OK )
    {
        if ( S_OK == g_cCameraController.DynChangeParam(DCP_ENABLE_AWB, iValue) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }
#endif
    return E_FAIL;
}

HRESULT HvXml_GetAWBEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = g_cModuleParams.cCamAppParam.iAWBEnable;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];

    strcpy(rgszName[0], "value");
    sprintf(rgszValue[0], "%d", iValue);

    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 1, rgszName, rgszValue);
}

HRESULT HvXml_SetAGCParam(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iShutterMin = 0;
    int iShutterMax = 0;
    int iGainMin = 0;
    int iGainMax = 0;

    if ( GetParamIntFromXml(pCmdArgElement, "ShutterMin", &iShutterMin) == S_OK
            && GetParamIntFromXml(pCmdArgElement, "ShutterMax", &iShutterMax) == S_OK
            && GetParamIntFromXml(pCmdArgElement, "GainMin", &iGainMin) == S_OK
            && GetParamIntFromXml(pCmdArgElement, "GainMax", &iGainMax) == S_OK )
    {
#ifdef _CAM_APP_
        if ( 0 == g_cCameraController.SetAGCParam(iShutterMin, iShutterMax, iGainMin, iGainMax) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
#endif
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_GetAGCParam(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iShutterMin = g_cModuleParams.cCamAppParam.iAGCShutterLOri;
    int iShutterMax = g_cModuleParams.cCamAppParam.iAGCShutterHOri;
    int iGainMin = g_cModuleParams.cCamAppParam.iAGCGainLOri;
    int iGainMax = g_cModuleParams.cCamAppParam.iAGCGainHOri;

    char rgszName[4][IVN_MAX_SIZE];
    char rgszValue[4][IVN_MAX_SIZE];

    strcpy(rgszName[0], "ShutterMin");
    sprintf(rgszValue[0], "%d", iShutterMin);
    strcpy(rgszName[1], "ShutterMax");
    sprintf(rgszValue[1], "%d", iShutterMax);
    strcpy(rgszName[2], "GainMin");
    sprintf(rgszValue[2], "%d", iGainMin);
    strcpy(rgszName[3], "GainMax");
    sprintf(rgszValue[3], "%d", iGainMax);

    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 4, rgszName, rgszValue);

}

HRESULT HvXml_SetJpegCompressRate(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = 0;

    if ( GetParamIntFromXml(pCmdArgElement, "Value", &iValue) == S_OK )
    {
#ifdef _CAM_APP_
        if ( S_OK == g_cCameraController.DynChangeParam(DCP_JPEG_COMPRESS_RATE, iValue) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
#endif
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_GetJpegCompressRate(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = g_cModuleParams.cCamAppParam.iJpegCompressRate;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];

    strcpy(rgszName[0], "value");
    sprintf(rgszValue[0], "%d", iValue);

    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 1, rgszName, rgszValue);
}

HRESULT HvXml_SetJpegCompressRateCapture(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = 0;

    if ( GetParamIntFromXml(pCmdArgElement, "Value", &iValue) == S_OK )
    {
#ifdef _CAM_APP_
        if ( S_OK == g_cCameraController.DynChangeParam(DCP_JPEG_COMPRESS_RATE_CAPTURE, iValue) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
#endif
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_GetJpegCompressRateCapture(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = g_cModuleParams.cCamAppParam.iJpegCompressRateCapture;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];

    strcpy(rgszName[0], "value");
    sprintf(rgszValue[0], "%d", iValue);

    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 1, rgszName, rgszValue);
}

HRESULT HvXml_SetFlashRateSynSignalEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = 0;

    if ( GetParamIntFromXml(pCmdArgElement, "Value", &iValue) == S_OK )
    {
#ifdef _CAM_APP_
        if ( S_OK == g_cCameraController.DynChangeParam(DCP_FLASH_RATE_SYN_SIGNAL_ENABLE, iValue) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
#endif
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_GetFlashRateSynSignalEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = g_cModuleParams.cCamAppParam.iFlashRateSynSignalEnable;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];

    strcpy(rgszName[0], "value");
    sprintf(rgszValue[0], "%d", iValue);

    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 1, rgszName, rgszValue);
}

HRESULT HvXml_SetCaptureSynSignalEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = 0;

    if ( GetParamIntFromXml(pCmdArgElement, "Value", &iValue) == S_OK )
    {
#ifdef _CAM_APP_
        if ( S_OK == g_cCameraController.DynChangeParam(DCP_CAPTURE_SYN_SIGNAL_ENABLE, iValue) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
#endif
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_GetCaptureSynSignalEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = g_cModuleParams.cCamAppParam.iCaptureSynSignalEnable;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];

    strcpy(rgszName[0], "value");
    sprintf(rgszValue[0], "%d", iValue);

    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 1, rgszName, rgszValue);
}

HRESULT HvXml_GetWorkMode(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    int nValue = -1, nReturnSize = 4;
    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];
    strcpy(rgszName[0], "WorkMode");

    g_cHvPciLinkApi.SendData(
        PCILINK_GET_WORKMODE,
        NULL, 0,
        &nValue,
        &nReturnSize
    );

    sprintf(rgszValue[0], "%d", nValue);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             1,
             rgszName,
             rgszValue
         );
    return hr;
}

HRESULT HvXml_GetWorkModeCount(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    int nValue = 0, nReturnSize = 4;
    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];
    strcpy(rgszName[0], "WorkModeCount");

    g_cHvPciLinkApi.SendData(
        PCILINK_GET_WORKMODE_COUNT,
        NULL, 0,
        &nValue,
        &nReturnSize
    );

    sprintf(rgszValue[0], "%d", nValue);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             1,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_GetCpuTemperature(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];
    strcpy(rgszName[0], "Value");
    strcpy(rgszValue[0], "-1");

    int iTemp = 0;
    GetCpuNearTemp(&iTemp);
    sprintf(rgszValue[0], "%d", iTemp);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             1,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_GetEncodeMode(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;
#ifdef _CAM_APP_
    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];
    strcpy(rgszName[0], "EncodeMode");
    strcpy(rgszValue[0], "-1");

    sprintf(rgszValue[0], "%d", g_cCameraController.GetEncodeMode());

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             1,
             rgszName,
             rgszValue
         );
#endif
    return hr;
}

HRESULT HvXml_SaveADSamplingValue(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValueA = 0;
    int iValueB = 0;

    if ( (GetParamIntFromXml(pCmdArgElement, "ValueA", &iValueA) == S_OK)
            && ( GetParamIntFromXml(pCmdArgElement, "ValueB", &iValueB) == S_OK) )
    {
        if ( S_OK == EprSetADSamplingValue(iValueA, iValueB) )
        {
            HV_Trace(5, "EprSetADSamplingValue ok\n");
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            HV_Trace(5, "HvXml_SaveADSamplingValue failed\n");
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_SaveParam(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
#ifdef _CAM_APP_
    if ( S_OK == g_cCameraController.SaveCurrentParam() )
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
    }
    else
#endif
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
}

HRESULT HvXml_SetCharacterEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = 0;

    if ( GetParamIntFromXml(pCmdArgElement, "EnableCharacter", &iValue) == S_OK )
    {
#ifdef _CAM_APP_
        if ( S_OK == g_cCameraController.EnableCharacterOverlap(iValue) )
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
#endif
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }

    return E_FAIL;
}

HRESULT HvXml_GetCharacterEnable(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = g_cModuleParams.cCamAppParam.iEnableCharacterOverlap;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];

    strcpy(rgszName[0], "value");
    sprintf(rgszValue[0], "%d", iValue);

    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 1, rgszName, rgszValue);
}

HRESULT HvXml_GetCharacterInfo(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
// 时间显示字符长度为strlen("2012-04-19 09:02:10:999  ") = 25
#define TIMEINFO_LEN  25

    char rgszName[6][IVN_MAX_SIZE];
    char rgszValue[6][IVN_MAX_SIZE];
    int iX = g_cModuleParams.cCamAppParam.iX;
    int iY = g_cModuleParams.cCamAppParam.iY;
    int iDateFormat = g_cModuleParams.cCamAppParam.iDateFormat;
    int iFontColor = g_cModuleParams.cCamAppParam.iFontColor;
    int iHalfWidthSize = GetFontHalfWidthSize(g_cModuleParams.cCamAppParam.iFontSize);
    int iHalfWidth = GetFontHalfWidth(g_cModuleParams.cCamAppParam.iFontSize);
    int iFontHeight = GetFontHeight(g_cModuleParams.cCamAppParam.iFontSize);
    int iCharacterWidth = (TIMEINFO_LEN+g_cModuleParams.cCamAppParam.iLatticeLen/iHalfWidthSize)*iHalfWidth;
    int iFixedLight = g_cModuleParams.cCamAppParam.iEnableFixedLight;
    strcpy(rgszName[0], "x");
    sprintf(rgszValue[0], "%d", iX);
    strcpy(rgszName[1], "y");
    sprintf(rgszValue[1], "%d", iY);
    strcpy(rgszName[2], "DateFormat");
    sprintf(rgszValue[2], "%d", iDateFormat);
    strcpy(rgszName[3], "FontColor");
    sprintf(rgszValue[3], "%d", iFontColor);
    strcpy(rgszName[4], "CharacterWidth");
    sprintf(rgszValue[4], "%d", iCharacterWidth);
    strcpy(rgszName[5], "CharacterHeight");
    sprintf(rgszValue[5], "%d", iFontHeight);
    strcpy(rgszName[6], "FixedLight");
    sprintf(rgszValue[6], "%d", iFixedLight);

    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0", 7, rgszName, rgszValue);
}

HRESULT HvXml_SaveCharacterInfo(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
#ifdef _CAM_APP_
    g_cCameraController.SaveCharacterInfo();
#endif
    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
}
#endif // SINGLE_BOARD_PLATFORM

HRESULT HvXml_SetCharacterFixedLight(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = 0;
    if ( GetParamIntFromXml(pCmdArgElement, "value", &iValue) == S_OK )
    {
#ifdef _CAM_APP_
        g_cCameraController.SetCharacterLightness(iValue);
#endif
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
    }
    return E_FAIL;
}
// ------------------------ 工具函数实现 -------------------------------

// 工具函数：快速建立命令xml
int BuildHvCmdXml(
    char* pXmlBuf,
    char* pCmdName,
    int iArgCount,
    const char rgszName[][IVN_MAX_SIZE],
    const char rgszValue[][IVN_MAX_SIZE]
)
{
    TiXmlDocument doc;
    TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
    doc.LinkEndChild(pDecl);

    TiXmlElement* pXmlRootElement = new TiXmlElement(HX_HVCMD);
    pXmlRootElement->SetAttribute(HX_VER, "1.0");

    doc.LinkEndChild(pXmlRootElement);

    TiXmlElement *pXmlElementCmd = new TiXmlElement(HX_CMDNAME);
    pXmlRootElement->LinkEndChild(pXmlElementCmd);

    TiXmlText *pXmlCmdText = new TiXmlText(pCmdName);
    pXmlElementCmd->LinkEndChild(pXmlCmdText);

    if (iArgCount != 0)
    {
        TiXmlElement *pXmlElementCmd = new TiXmlElement(HX_CMDARG);
        pXmlRootElement->LinkEndChild(pXmlElementCmd);

        for (int i=0; i<iArgCount; i++)
        {
            TiXmlElement *pXmlElementID = new TiXmlElement(HX_VALUE);
            pXmlElementCmd->LinkEndChild(pXmlElementID);

            pXmlElementID->SetAttribute(HX_ID, rgszName[i]);

            if (0 == strcmp(rgszValue[i] ,""))
                continue;

            TiXmlText *pXmlIDText = new TiXmlText(rgszValue[i]);
            pXmlElementID->LinkEndChild(pXmlIDText);
        }
    }


    TiXmlPrinter printer;
    doc.Accept(&printer);

    int len = (int)printer.Size();
    memcpy(pXmlBuf, printer.CStr(), len);
    pXmlBuf[len] = '\0';

    return len;
}


static HRESULT BuildHvCmdRetXml(
    TiXmlElement* pRootElementRet,
    const TiXmlElement* pCmdArgElement,
    const char* szRetCode,
    int iRetCount/* = 0*/,
    const char rgszId[][IVN_MAX_SIZE]/* = NULL*/,
    const char rgszValue[][IVN_MAX_SIZE]/* = NULL*/
)
{
    if (NULL == pRootElementRet ||NULL == pCmdArgElement)
    {
        return E_FAIL;
    }

    TiXmlElement* pElementRet = new TiXmlElement(HX_CMDNAME);
    if ( pElementRet )
    {
        //命令名称
        TiXmlText *pXmlCmdText = new TiXmlText(pCmdArgElement->GetText());
        pElementRet->LinkEndChild(pXmlCmdText);

        if (szRetCode)
        {
            pElementRet->SetAttribute(HX_RETCODE, szRetCode);
        }

        //参数返回便
        for ( int i=0; i<iRetCount; ++i )
        {
            if (rgszId[i] && rgszValue[i])
            {
                pElementRet->SetAttribute(rgszId[i], rgszValue[i]);
            }
        }
        pRootElementRet->LinkEndChild(pElementRet);
    }

    return S_OK;
}

static HRESULT GetParamStringFromXml(
    const TiXmlElement* pCmdArgElement,
    const char *pszParamName,
    char *pszParamValue,
    int nParamValueSize
)
{
    if ( pCmdArgElement == NULL
            || pszParamName == NULL
            || pszParamValue == NULL
            || nParamValueSize <= 0 )
    {
        return E_POINTER;
    }

    const char* pszAttr = pCmdArgElement->Attribute(pszParamName);
    if (pszAttr)
    {
        strcpy(pszParamValue, pszAttr);
        return S_OK;
    }

    return E_FAIL;
}

static HRESULT GetParamIntFromXml(
    const TiXmlElement* pCmdArgElement,
    const char *pszParamName,
    int *pnParamValue
)
{
    if (NULL == pCmdArgElement || NULL == pszParamName || NULL == pnParamValue)
    {
        return E_POINTER;
    }

    const char* pszAttr = pCmdArgElement->Attribute(pszParamName);
    if (pszAttr)
    {
        (*pnParamValue) = atoi(pszAttr);
        return S_OK;
    }

    return E_FAIL;
}

//-------------------------单双板共用函擿------------------------------

// 执行命令
HRESULT HvXml_Cmd(const TiXmlDocument cXmlDoc, TiXmlDocument& cResultXmlDoc)
{
    const TiXmlElement* pRootElement = cXmlDoc.RootElement();
    if ( NULL == pRootElement )
    {
        return E_FAIL;
    }

    TiXmlDeclaration* pResultDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
    TiXmlElement* pResultRootElement = new TiXmlElement(HX_HVCMDRESPOND);
    if ( NULL == pResultDecl || NULL == pResultRootElement)
    {
        return E_FAIL;
    }
    pResultRootElement->SetAttribute(HX_VER, HV_XML_VERSION);

    int iSuccessCount = 0;
    const TiXmlElement* pCmdArgElement = pRootElement->FirstChildElement();
    while (pCmdArgElement)
    {
        HVXML_CMD_FUNC fp = NULL;

        for ( int i = 0; i < g_HvXmlCmdMapItemCount; ++i )
        {
            if ( 0 == strcmp(g_HvXmlCmdFuncMap[i].szCmdName, pCmdArgElement->GetText()) )
            {
                fp = g_HvXmlCmdFuncMap[i].fpCmdFunc;
                break;
            }
        }
        if ( fp )
        {
            HRESULT hr = (*fp)(pCmdArgElement, pResultRootElement);

            if (SUCCEEDED(hr))
            {
                ++iSuccessCount;
                HV_Trace(5, "HvXml_Cmd[%s] OK\n", pCmdArgElement->GetText());
            }
            else
            {
                HV_Trace(5, "HvXml_Cmd[%s] FAILED, ret=%08x\n", pCmdArgElement->GetText(), hr);
            }
        }
        pCmdArgElement = pCmdArgElement->NextSiblingElement();
    }

    if (iSuccessCount <= 0)
    {
        return E_FAIL;
    }

    cResultXmlDoc.LinkEndChild(pResultDecl);
    cResultXmlDoc.LinkEndChild(pResultRootElement);
    return S_OK;
}

// 解析执行HvXml协议
HRESULT ParseExecXmlExtCmd(char* pbXmlBuf, DWORD32& dwXmlLen)
{
    if ( NULL == pbXmlBuf || dwXmlLen <= 0 )
    {
        return E_INVALIDARG;
    }

    TiXmlDocument cXmlDoc;
    if ( cXmlDoc.Parse(pbXmlBuf) )
    {
        const TiXmlElement* pRootElement = cXmlDoc.RootElement();
        if ( pRootElement != NULL
                && 0 == strcmp(HV_XML_VERSION, pRootElement->Attribute(HX_VER)) )
        {
            HRESULT hr = E_FAIL;
            TiXmlDocument cResultXmlDoc;

            if ( 0 == strcmp(HX_HVCMD, pRootElement->Value()) )
            {
                hr = HvXml_Cmd(cXmlDoc, cResultXmlDoc);
            }
            else
            {
                hr = E_NOTIMPL;
            }

            if ( S_OK == hr )
            {
                TiXmlPrinter cPrinter;
                cResultXmlDoc.Accept(&cPrinter);
                if ( cPrinter.Size() > 0 && cPrinter.Size() < dwXmlLen )
                {
                    dwXmlLen = (DWORD32)cPrinter.Size();
                    strcpy(pbXmlBuf, cPrinter.CStr());
                    return S_OK;
                }
                else
                {
                    dwXmlLen = (DWORD32)cPrinter.Size();
                    return S_FALSE;
                }
            }
        }
    }

    return E_FAIL;
}

HRESULT HvXml_SetTime(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    char szDate[32] = {0};
    char szTime[32] = {0};

    if ( NULL == pCmdArgElement )
    {
        return E_INVALIDARG;
    }

    const TiXmlAttribute* pAttr = pCmdArgElement->FirstAttribute();
    while ( pAttr )
    {
        if ( 0 == strcmp("Date", pAttr->Name()) )
        {
            if ( strlen(pAttr->Value()) < 32 )
            {
                strcpy(szDate, pAttr->Value());
            }
        }
        else if ( 0 == strcmp("Time", pAttr->Name()) )
        {
            if ( strlen(pAttr->Value()) < 32 )
            {
                strcpy(szTime, pAttr->Value());
            }
        }

        pAttr = pAttr->Next();
    }
    int iLen = strlen(szTime);
    if ( 10 == strlen(szDate) && (8 == iLen || 12 == iLen) )
    {
        //设置日期时间
        REAL_TIME_STRUCT cRealTime;
        DWORD32 dwYear,dwMonth,dwDay,dwHour,dwMinute,dwSecond, dwMs = 0;

        sscanf(szDate, "%04d.%02d.%02d",
               &dwYear,
               &dwMonth,
               &dwDay);

        if (iLen == 8)
        {
            sscanf(szTime, "%02d:%02d:%02d",
                   &dwHour,
                   &dwMinute,
                   &dwSecond);
        }
        else if (iLen == 12)
        {
            sscanf(szTime, "%02d:%02d:%02d %03d",
                   &dwHour,
                   &dwMinute,
                   &dwSecond,
                   &dwMs);
        }
        HV_Trace(5, "%s %s\n", szDate, szTime);

        cRealTime.wYear = dwYear;
        cRealTime.wMonth = dwMonth;
        cRealTime.wDay = dwDay;
        cRealTime.wHour = dwHour;
        cRealTime.wMinute = dwMinute;
        cRealTime.wSecond = dwSecond;
        cRealTime.wMSecond = dwMs;

        DWORD32 dwTimeMsLow = 0;
        DWORD32 dwTimeMsHigh = 0;
        ConvertTimeToMs(&cRealTime, &dwTimeMsLow, &dwTimeMsHigh);

        if ( 0 == SetSystemTime(cRealTime)
#ifndef SINGLE_BOARD_PLATFORM
                && 0 == PciSendClockSyncData(dwTimeMsLow, dwTimeMsHigh)
#endif
           )
        {
            GetSystemTime(&dwTimeMsLow, &dwTimeMsHigh);
            ConvertMsToTime(dwTimeMsLow, dwTimeMsHigh, &cRealTime);
            HV_Trace(5, "%04d-%02d-%02d %02d:%02d:%02d %03d", cRealTime.wYear, cRealTime.wMonth, cRealTime.wDay, cRealTime.wHour, cRealTime.wMinute, cRealTime.wSecond, cRealTime.wMSecond);
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
        }
        else
        {
            return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
        }
    }
    else
    {
        return E_FAIL;
    }
}

HRESULT HvXml_SetIP(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    char szIP[32] = {0};
    char szMask[32] = {0};
    char szGateway[32] = {0};

    if ( GetParamStringFromXml(pCmdArgElement, "IP", szIP, sizeof(szIP)) == S_OK
            && GetParamStringFromXml(pCmdArgElement, "Mask", szMask, sizeof(szMask)) == S_OK
            && GetParamStringFromXml(pCmdArgElement, "Gateway", szGateway, sizeof(szGateway)) == S_OK )
    {
        TcpipParam cTcpipParam;
        strcpy(cTcpipParam.szIp, szIP);
        strcpy(cTcpipParam.szNetmask, szMask);
        strcpy(cTcpipParam.szGateway, szGateway);

        g_cCmdInfoFirstRetAfterExec.cTcpipParam = cTcpipParam;
        g_cCmdInfoFirstRetAfterExec.fHvXmlSetIPValid = true;

        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
    }

    return E_FAIL;
}

HRESULT HvXml_ResetDevice(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    if ( S_OK != GetParamIntFromXml(pCmdArgElement, "ResetMode", &g_cCmdInfoFirstRetAfterExec.iResetMode) )
    {
        g_cCmdInfoFirstRetAfterExec.iResetMode = -1;
    }

    g_cCmdInfoFirstRetAfterExec.fHvXmlResetDeviceValid = true;
    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
}

HRESULT HvXml_RestoreDefaultParam(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iValue = 3;
    if ( S_OK != GetParamIntFromXml(pCmdArgElement, "Value", &iValue) )
    {
    }
    HV_Trace(5, "HvXml_RestoreDefaultParam, value=%d\n", iValue);
    g_cCmdInfoFirstRetAfterExec.iHvXmlRestoreDefaultParamValid = iValue;
    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
}

HRESULT HvXml_RestoreFactoryParam(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    g_cCmdInfoFirstRetAfterExec.fHvXmlRestoreFactoryParamValid = true;
    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
}

HRESULT HvXml_GetDateTime(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    char szDate[32] = {0};
    char szTime[32] = {0};

    // 获取日期时间
    REAL_TIME_STRUCT cRealTime;
    DWORD32 dwTimeMsLow = 0;
    DWORD32 dwTimeMsHigh = 0;

    GetSystemTime(&dwTimeMsLow, &dwTimeMsHigh);
    ConvertMsToTime(dwTimeMsLow, dwTimeMsHigh, &cRealTime);

    sprintf(szDate, "%04d.%02d.%02d",
            cRealTime.wYear,
            cRealTime.wMonth,
            cRealTime.wDay);

    sprintf(szTime, "%02d:%02d:%02d %03d",
            cRealTime.wHour,
            cRealTime.wMinute,
            cRealTime.wSecond,
            cRealTime.wMSecond);

    HRESULT hr = E_FAIL;

    char rgszName[2][IVN_MAX_SIZE];
    char rgszValue[2][IVN_MAX_SIZE];
    strcpy(rgszName[0], "Date");
    strcpy(rgszValue[0], szDate);
    strcpy(rgszName[1], "Time");
    strcpy(rgszValue[1], szTime);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             2,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_GetHvName(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];
    strcpy(rgszName[0], "Name");
    strcpy(rgszValue[0], "DM6467-Video-Beta");

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             1,
             rgszName,
             rgszValue
         );
    return hr;
}

HRESULT HvXml_GetHvID(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    char rgszName[2][IVN_MAX_SIZE];
    char rgszValue[2][IVN_MAX_SIZE];
    strcpy(rgszName[0], "Id");
    strcpy(rgszValue[0], "0x0000");
    strcpy(rgszName[1], "Name");
    strcpy(rgszValue[1], "Unknown");

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             2,
             rgszName,
             rgszValue
         );
    return hr;
}

HRESULT HvXml_GetResetCount(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];
    strcpy(rgszName[0], "ResetCount");
    strcpy(rgszValue[0], "0");

    int iResetCount = 0;
    if ( GetResetCount(&iResetCount) >= 0 )
    {
        sprintf(rgszValue[0], "%d", iResetCount);

        hr = BuildHvCmdRetXml(
                 pRootElementRet,
                 pCmdArgElement,
                 NULL,
                 1,
                 rgszName,
                 rgszValue
             );
    }

    return hr;
}

HRESULT HvXml_GetResetMode(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];
    strcpy(rgszName[0], "ResetMode");
    strcpy(rgszValue[0], "0");

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             1,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_GetProductName(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];
    strcpy(rgszName[0], "ProductName");

    // 注：这里rgszValue[0]的长度必须为128字节时才能这样写
    if ( GetSN(rgszValue[0], IVN_MAX_SIZE) >= 0 )
    {
        hr = BuildHvCmdRetXml(
                 pRootElementRet,
                 pCmdArgElement,
                 NULL,
                 1,
                 rgszName,
                 rgszValue
             );
    }

    return hr;
}

HRESULT HvXml_GetVideoCount(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];
    strcpy(rgszName[0], "VideoCount");
    strcpy(rgszValue[0], "1");

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             1,
             rgszName,
             rgszValue
         );
    return hr;
}

HRESULT HvXml_GetVersion(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    char rgszName[2][IVN_MAX_SIZE];
    char rgszValue[2][IVN_MAX_SIZE];

    strcpy(rgszName[0], "SoftVersion");
    sprintf(rgszValue[0], "%s %s %s",
            PSZ_DSP_BUILD_NO,
            DSP_BUILD_DATE,
            DSP_BUILD_TIME
           );

    strcpy(rgszName[1], "ModelVersion");
    strcpy(rgszValue[1], "Unknown");

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             2,
             rgszName,
             rgszValue
         );
    return hr;
}

HRESULT HvXml_GetVersionString(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];
    strcpy(rgszName[0], "SoftVersionString");
    sprintf(rgszValue[0], "%s %s %s",
            PSZ_DSP_BUILD_NO,
            DSP_BUILD_DATE,
            DSP_BUILD_TIME
           );

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             1,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_GetConnectedIP(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    char rgszName[CONNECTED_IP_MAX_COUNT][IVN_MAX_SIZE];
    char rgszValue[CONNECTED_IP_MAX_COUNT][IVN_MAX_SIZE];
    int iCount = g_cCameraConnectedLog.GetConnectedInfo(rgszName, rgszValue, CONNECTED_IP_MAX_COUNT);
    if ( iCount != -1 )
    {
        hr = BuildHvCmdRetXml(
                 pRootElementRet,
                 pCmdArgElement,
                 NULL,
                 iCount,
                 rgszName,
                 rgszValue
             );
    }

    return hr;
}

HRESULT HvXml_GetResetReport(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    static char szResetReport[4*1024] = {0};  // 4KB
    if ( 0 == ReadResetReport((char*)szResetReport, 4*1024) )
    {
        TiXmlElement* pElementRet = new TiXmlElement(HX_CMDNAME);

        //命令名称
        TiXmlText *pXmlCmdText = new TiXmlText(pCmdArgElement->GetText());
        pElementRet->LinkEndChild(pXmlCmdText);

        //参数返回便
        pElementRet->SetAttribute("ResetReport", szResetReport);

        pRootElementRet->LinkEndChild(pElementRet);
        hr = S_OK;
    }

    return hr;
}

HRESULT HvXml_GetHddOpStatus(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];
    strcpy(rgszName[0], "HddOpStatus");
    strcpy(rgszValue[0], "-1");

    sprintf(rgszValue[0], "%d", g_cHddOpThread.GetCurStatusCode());

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             1,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_GetHddCheckReport(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];
    strcpy(rgszName[0], "HddCheckReport");
    strncpy(rgszValue[0], g_cHddOpThread.GetCurStatusCodeString(), IVN_MAX_SIZE);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             1,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_GetDevType(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    char rgszName[2][IVN_MAX_SIZE];
    char rgszValue[2][IVN_MAX_SIZE];
    strcpy(rgszName[0], "DevType");
    strcpy(rgszName[1], "BuildNo");
#ifdef SINGLE_BOARD_PLATFORM
    strcpy(rgszValue[0], "DM6467_S");
#else
#ifdef _CAMERA_PIXEL_500W_
    strcpy(rgszValue[0], "DM6467_500W");
#else
    strcpy(rgszValue[0], "DM6467_200W");
#endif
#endif
    strcpy(rgszValue[1], PSZ_DSP_BUILD_NO);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             2,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_DoPartition(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iRet = g_cCheckDisk.DoPartition();
    char szRet[8] = {0};
    sprintf(szRet, "%d", iRet);
    HV_Trace(5, "HvXml_DoPartition:%s\n", szRet);

    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, szRet);
}

HRESULT HvXml_DoCheckDisk(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iRet = g_cCheckDisk.DoCheckDisk();
    char szRet[8] = {0};
    sprintf(szRet, "%d", iRet);
    HV_Trace(5, "HvXml_DoCheckDisk:%s\n", szRet);

    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, szRet);
}

HRESULT HvXml_GetCheckDisk(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    SCheckDiskReport sCheckDiskReport;
    g_cCheckDisk.GetCheckDisk(sCheckDiskReport);
    char szRet[8] = {0};
    sprintf(szRet, "%d", sCheckDiskReport.iResult);
    HV_Trace(5, "HvXml_GetCheckDisk:%s\n", szRet);

    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];
    strcpy(rgszName[0], "RetMsg");
    if (strlen(sCheckDiskReport.szMsg) > 0)
    {
        strncpy(rgszValue[0], sCheckDiskReport.szMsg, IVN_MAX_SIZE - 1);
    }
    else
    {
        strcpy(rgszValue[0], "");
    }

    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, szRet, 1, rgszName, rgszValue);
}

#ifndef SINGLE_BOARD_PLATFORM
HRESULT HvXml_SetControllPannelWorkStatus(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iWorkMode = 0;
    int iWorkStatus = 0;

    if (GetParamIntFromXml(pCmdArgElement, "WorkMode", &iWorkMode) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "WorkStatus", &iWorkStatus) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (g_pCamApp->SetControllPannelStatus(iWorkMode, iWorkStatus) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    else
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
    }
    return E_FAIL;
}

HRESULT HvXml_SetControllPannelPulseWidthRange(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iPulseWidthMin = 0;
    int iPulseWidthMax = 0;
    if (GetParamIntFromXml(pCmdArgElement, "PulseWidthMin", &iPulseWidthMin) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "PulseWidthMax", &iPulseWidthMax) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (g_pCamApp->SetPulseWidthRange(iPulseWidthMin, iPulseWidthMax) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    else
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
    }
    return E_FAIL;
}

HRESULT HvXml_TestControllPannelPLMode(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iPolarizingPrismMode = -1;
    if (GetParamIntFromXml(pCmdArgElement, "Mode", &iPolarizingPrismMode) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (g_pCamApp->TestPLMode(iPolarizingPrismMode) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    else
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
    }
    return E_FAIL;
}

HRESULT HvXml_SetFlashSingle(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iChannel = 0;
    int iPolarity = 0;
    int iTriggerType = 0;
    int iPulseWidth = 10;
    int iCoupling = 0;
    if (GetParamIntFromXml(pCmdArgElement, "Channel", &iChannel) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "Polarity", &iPolarity) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "TriggerType", &iTriggerType) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "PulseWidth", &iPulseWidth) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "Coupling", &iCoupling) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (g_pCamApp->SetFlashSingle(iChannel, iPolarity, iTriggerType, iPulseWidth, iCoupling) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    else
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
    }
    return E_FAIL;
}

HRESULT HvXml_SaveControllPannelConfig(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iFlash1Polarity = 0;
    int iFlash1TriggerType = 0;
    int iFlash1PulseWidth = 10;
    int iFlash1Coupling = 0;
    int iFlash2Polarity = 0;
    int iFlash2TriggerType = 0;
    int iFlash2PulseWidth = 10;
    int iFlash2Coupling = 0;
    int iFlash3Polarity = 0;
    int iFlash3TriggerType = 0;
    int iFlash3PulseWidth = 10;
    int iFlash3Coupling = 0;
    int iPulseWidthMin = 0;
    int iPulseWidthMax = 0;
    int iAutoRunStatus = 0;
    if (GetParamIntFromXml(pCmdArgElement, "AutoRun", &iAutoRunStatus) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "PulseWidthMin", &iPulseWidthMin) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "PulseWidthMax", &iPulseWidthMax) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }

    if (GetParamIntFromXml(pCmdArgElement, "Flash1Polarity", &iFlash1Polarity) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "Flash1TriggerType", &iFlash1TriggerType) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "Flash1PulseWidth", &iFlash1PulseWidth) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "Flash1Coupling", &iFlash1Coupling) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }

    if (GetParamIntFromXml(pCmdArgElement, "Flash2Polarity", &iFlash2Polarity) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "Flash2TriggerType", &iFlash2TriggerType) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "Flash2PulseWidth", &iFlash2PulseWidth) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "Flash2Coupling", &iFlash2Coupling) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }

    if (GetParamIntFromXml(pCmdArgElement, "Flash3Polarity", &iFlash3Polarity) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "Flash3TriggerType", &iFlash3TriggerType) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "Flash3PulseWidth", &iFlash3PulseWidth) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (GetParamIntFromXml(pCmdArgElement, "Flash3Coupling", &iFlash3Coupling) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    SaveControllPannelAutoRunStatus(iAutoRunStatus, TRUE);
    SaveControllPannelPulseWidthRange(iPulseWidthMin, iPulseWidthMax, TRUE);
    SaveControllPannelFlashConfig(0, iFlash1Polarity, iFlash1TriggerType, iFlash1PulseWidth, iFlash1Coupling, TRUE);
    SaveControllPannelFlashConfig(1, iFlash2Polarity, iFlash2TriggerType, iFlash2PulseWidth, iFlash2Coupling, TRUE);
    SaveControllPannelFlashConfig(2, iFlash3Polarity, iFlash3TriggerType, iFlash3PulseWidth, iFlash3Coupling, TRUE);
    HRESULT hr = BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
    if (hr == S_OK)
    {
        g_cCmdInfoFirstRetAfterExec.fHvXmlResetDeviceValid = true;
    }
    return hr;
}

HRESULT HvXml_SetControllPannelAutoRunStatus(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iStatus = 0;
    if (GetParamIntFromXml(pCmdArgElement, "Status", &iStatus) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (g_pCamApp->SetControllPannelAutoRunStatus(iStatus) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
}

HRESULT HvXml_SetControllPannelPulseLevel(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    int iPulseLevel = 0;
    if (GetParamIntFromXml(pCmdArgElement, "Level", &iPulseLevel) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    if (g_pCamApp->SetTestModePulseLevel(iPulseLevel) != S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
    return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
}

HRESULT HvXml_SetControllPannelDefParam(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    if (g_pCamApp->SetControllPannelDefaultParam() == S_OK)
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "0");
    }
    else
    {
        return BuildHvCmdRetXml(pRootElementRet, pCmdArgElement, "-1");
    }
}

HRESULT HvXml_GetControllPannelVersion(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;
    char rgszName[2][IVN_MAX_SIZE] = {0};
    char rgszValue[2][IVN_MAX_SIZE] = {0};
    strcpy(rgszName[0], "UpdataProgram");
    strcpy(rgszName[1], "MainProgram");
    char rgszVersionString[256] = {0};
    int iBufLen = 256;
    if (g_pCamApp == NULL) return E_FAIL;
    if (g_pCamApp->GetPannelVersion((unsigned char*)rgszVersionString, iBufLen) != S_OK)
    {
        return E_FAIL;
    }
    memcpy(rgszValue[0], rgszVersionString+16, 64);
    memcpy(rgszValue[1], rgszVersionString+100, 64);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             2,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_GetControllPannelTemperature(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;
    char rgszName[1][IVN_MAX_SIZE];
    char rgszValue[1][IVN_MAX_SIZE];
    strcpy(rgszName[0], "PannelTemperature");
    float fPannelTemperature = 0.0f;
    if (g_pCamApp == NULL) return E_FAIL;
    if (g_pCamApp->GetPannelTemperature(fPannelTemperature) != S_OK)
    {
        return E_FAIL;
    }
    sprintf(rgszValue[0], "%3.2f", fPannelTemperature);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             1,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_GetControllPannelStatus(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    int iWorkMode, iWorkStatus;
    if (g_pCamApp == NULL) return E_FAIL;
    if (g_pCamApp->GetPannelStatus(iWorkMode, iWorkStatus) != S_OK)
    {
        return E_FAIL;
    }
    char rgszName[2][IVN_MAX_SIZE] = {0};
    char rgszValue[2][IVN_MAX_SIZE] = {0};
    strcpy(rgszName[0], "WorkMode");
    strcpy(rgszName[1], "WorkStatus");
    sprintf(rgszValue[0], "%d", iWorkMode);
    sprintf(rgszValue[1], "%d", iWorkStatus);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             2,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_GetControllPannelAutoRunStyle(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;

    int iControllPannelStyle;
    if (g_pCamApp == NULL) return E_FAIL;
    if (g_pCamApp->GetControllPannelStyle(iControllPannelStyle) != S_OK)
    {
        return E_FAIL;
    }
    char rgszName[1][IVN_MAX_SIZE] = {0};
    char rgszValue[1][IVN_MAX_SIZE] = {0};
    strcpy(rgszName[0], "Style");
    sprintf(rgszValue[0], "%d", iControllPannelStyle);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             1,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_GetControllPannelPulseInfo(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;
    int iPulseLevel;
    int iPulseStep;
    int iPulseWidthMin;
    int iPulseWidthMax;
    if (g_pCamApp == NULL) return E_FAIL;
    if (g_pCamApp->GetPulseInfo(iPulseLevel, iPulseStep, iPulseWidthMin, iPulseWidthMax) != S_OK)
    {
        return hr;
    }

    char rgszName[4][IVN_MAX_SIZE] = {0};
    char rgszValue[4][IVN_MAX_SIZE] = {0};
    strcpy(rgszName[0], "PulseLevel");
    strcpy(rgszName[1], "PulseStep");
    strcpy(rgszName[2], "PulseWidthMin");
    strcpy(rgszName[3], "PulseWidthMax");
    sprintf(rgszValue[0], "%d", iPulseLevel);
    sprintf(rgszValue[1], "%d", iPulseStep);
    sprintf(rgszValue[2], "%d", iPulseWidthMin);
    sprintf(rgszValue[3], "%d", iPulseWidthMax);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             4,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_GetControllPannelDeviceStatus(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;
    int iPolarizingPrismStatus;
    int iPalanceLightStatus;
    if (g_pCamApp == NULL) return E_FAIL;
    if (g_pCamApp->GetOutDeviceStatus(iPolarizingPrismStatus, iPalanceLightStatus) != S_OK)
    {
        return hr;
    }

    char rgszName[2][IVN_MAX_SIZE] = {0};
    char rgszValue[2][IVN_MAX_SIZE] = {0};
    strcpy(rgszName[0], "PolarizingPrismStatus");
    strcpy(rgszName[1], "PalanceLightStatu");
    sprintf(rgszValue[0], "%d", iPolarizingPrismStatus);
    sprintf(rgszValue[1], "%d", iPalanceLightStatus);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             2,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_GetControllPannelFlashInfo(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;
    unsigned char rgbBuf[48] = {0};
    if (g_pCamApp == NULL) return E_FAIL;
    if (g_pCamApp->GetFlashInfo(rgbBuf, 48) != S_OK)
    {
        return hr;
    }
    char rgszName[12][IVN_MAX_SIZE] = {0};
    char rgszValue[12][IVN_MAX_SIZE] = {0};
    strcpy(rgszName[0], "Flash1PulseWidth");
    strcpy(rgszName[1], "Flash1Polarity");
    strcpy(rgszName[2], "Flash1Coupling");
    strcpy(rgszName[3], "Flash1ResistorMode");
    strcpy(rgszName[4], "Flash2PulseWidth");
    strcpy(rgszName[5], "Flash2Polarity");
    strcpy(rgszName[6], "Flash2Coupling");
    strcpy(rgszName[7], "Flash2ResistorMode");
    strcpy(rgszName[8], "Flash3PulseWidth");
    strcpy(rgszName[9], "Flash3Polarity");
    strcpy(rgszName[10], "Flash3Coupling");
    strcpy(rgszName[11], "Flash3ResistorMode");
    unsigned char* pTempbuf = rgbBuf;
    int iValue = 0;
    for (int index=0; index<12; index++)
    {
        memcpy(&iValue, pTempbuf, 4);
        pTempbuf += 4;
        sprintf(rgszValue[index], "%d", iValue);
    }

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             12,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_GetControllPannelUpdatingStatus(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;
    int iUpdatingStatus = 0;
    int iUpdatePageIndex = 0;
    if (g_pCamApp == NULL) return E_FAIL;
    if (g_pCamApp->GetControllPannelUpdatingStatus(iUpdatingStatus, iUpdatePageIndex) != S_OK)
    {
        return hr;
    }
    char rgszName[2][IVN_MAX_SIZE] = {0};
    char rgszValue[2][IVN_MAX_SIZE] = {0};
    strcpy(rgszName[0], "Status");
    strcpy(rgszName[1], "PageIndex");
    sprintf(rgszValue[0], "%d", iUpdatingStatus);
    sprintf(rgszValue[1], "%d", iUpdatePageIndex);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             2,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_GetControllPannelCRCValue(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;
    int iCRCValue = -1;
    if (g_pCamApp == NULL) return E_FAIL;
    if (g_pCamApp->GetControllPannelCRCValue(iCRCValue) != S_OK)
    {
        return hr;
    }
    char rgszName[1][IVN_MAX_SIZE] = {0};
    char rgszValue[1][IVN_MAX_SIZE] = {0};
    strcpy(rgszName[0], "Value");
    sprintf(rgszValue[0], "%d", iCRCValue);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             1,
             rgszName,
             rgszValue
         );

    return hr;
}

HRESULT HvXml_GetControllPannelAutoRunStatus(const TiXmlElement* pCmdArgElement, TiXmlElement* pRootElementRet)
{
    HRESULT hr = E_FAIL;
    int iStatus = 0;
    if (g_pCamApp == NULL) return E_FAIL;
    if (g_pCamApp->GetControllPannelAutoRunStatus(iStatus) != S_OK)
    {
        return hr;
    }
    char rgszName[1][IVN_MAX_SIZE] = {0};
    char rgszValue[1][IVN_MAX_SIZE] = {0};
    strcpy(rgszName[0], "Value");
    sprintf(rgszValue[0], "%d", iStatus);

    hr = BuildHvCmdRetXml(
             pRootElementRet,
             pCmdArgElement,
             NULL,
             1,
             rgszName,
             rgszValue
         );
    return hr;
}



#endif
