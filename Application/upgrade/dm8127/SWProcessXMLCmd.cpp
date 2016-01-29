#include "SWProcessXMLCmd.h"
#include "arch.h"
#include <unistd.h>

#define HV_XML_CMD_VERSION_NO "3.0"

#define HVXML_VER 					"Ver"
#define HVXML_HVCMD					"HvCmd"
#define HVXML_HVCMDRESPOND 			"HvCmdRespond"
#define HVXML_RETCODE 				"RetCode"
#define HVXML_RETMSG 				"RetMsg"
#define HVXML_CMDNAME				"CmdName"
#define HVXML_TYPE 					"Type"
#define HVXML_VALUE 				"Value"
#define HVXML_CLASS 				"Class"
#define HVXML_GETTER 				"GETTER"
#define HVXML_SETTER 				"SETTER"
#define HVXML_RETURNLEN				"RetLen"

// 命令回应包
typedef struct tag_CameraCmdRespond
{
    DWORD dwID;
    DWORD dwInfoSize;
    INT iResult;
}
CAMERA_CMD_RESPOND;

typedef struct __NETCMDMAPPINGTABLE
{
    const CHAR * 	szCmdName;          //XML命令字内容
    const DWORD		dwCmdID;            //XML命令字
    const BOOL      fOnlyBackup;        //只在备份模式执行
}NET_CMD_MAPPING_TABLE;

//命令名字与ID映射表
static struct __NETCMDMAPPINGTABLE g_XmlCmdMappingTable[] =
{
    {"ResetDevice",             CMD_ResetDevice,            FALSE},
    {"OptResetMode",            CMD_OptResetMode,           FALSE},
    {"RestoreFactorySetting",   CMD_RestoreFactorySetting,  TRUE},
    {"UnlockFactorySetting",    CMD_UnlockFactorySetting,   TRUE},
    {"WriteSN",                 CMD_WriteSN,                TRUE},
    {"SetTime",                 CMD_SetTime,                TRUE},
    {"DateTime",                CMD_GetTime,                TRUE},
    {"HDDStatus",               CMD_HDDStatus,              TRUE},
    {"GetDevType",              CMD_GetDevType,             TRUE},

    {"SetDCAperture",           CMD_SetDCAperture,          TRUE},
    {"GetDCAperture",           CMD_GetDCAperture,          TRUE},
    {"ZoomDCIRIS",              CMD_ZoomDCIRIS,             TRUE},
    {"ShrinkDCIRIS",            CMD_ShrinkDCIRIS,           TRUE},
    {"SetF1IO",                 CMD_SetF1IO,                TRUE},
    {"GetF1IO",                 CMD_GetF1IO,                TRUE},
    {"SetALMIO",                CMD_SetALMIO,               TRUE},
    {"GetALMIO",                CMD_GetALMIO,               TRUE},
    {"SetEXPIO",                CMD_SetEXPIO,               TRUE},
    {"GetEXPIO",                CMD_GetEXPIO,               TRUE},
    {"SetTGIO",                 CMD_SetTGIO,                TRUE},
    {"GetTGIO",                 CMD_GetTGIO,                TRUE},
    {"GetCoilTiggerCount",      CMD_GetCoilTiggerCount,     TRUE},
    {"ResetCoilTiggerCount",    CMD_ResetCoilTiggerCount,   TRUE},
    {"SetComTestEnable",        CMD_SetComTestEnable,       TRUE},
    {"SetComTestdisable",       CMD_SetComTestdisable,      TRUE},
    {"SetCtrlCpl",              CMD_SetCtrlCpl,             TRUE},

    {"SetAGCEnable",            CMD_SetAGCEnable,           TRUE},
    {"GetAGCEnable",            CMD_GetAGCEnable,           TRUE},
    {"SetGain",                 CMD_SetGain,                TRUE},
    {"GetGain",                 CMD_GetGain,                TRUE},
    {"SetShutter",              CMD_SetShutter,             TRUE},
    {"GetShutter",              CMD_GetShutter,             TRUE},
    {"SetAWBEnable",            CMD_SetAWBEnable,           TRUE},
    {"GetAWBEnable",            CMD_GetAWBEnable,           TRUE},
    {"SetRGBGain",              CMD_SetRGBGain,             TRUE},
    {"GetRGBGain",              CMD_GetRGBGain,             TRUE},
    {"AutoTestCamera",          CMD_AutoTestCamera,         TRUE},
    {"SwitchFPGA",              CMD_SwitchFPGA,             TRUE},
    {"GetFPGAMode",             CMD_GetFPGAMode,            TRUE},
    {"SetAF",                   CMD_SetAF,                  TRUE},
    {"GetAF",                   CMD_GetAF,                  TRUE},

    {"TuneFocus",               CMD_TuneFocus,              TRUE},
    {"GetFocus",                CMD_GetFocus,               TRUE},
    {"TuneZoom",                CMD_TuneZoom,                TRUE},
    {"GetZoom",                 CMD_GetZoom,                TRUE},

    {"StartPan",                CMD_StartPan,               TRUE},
    {"StopPan",                 CMD_StopPan,                TRUE},
    {"StartTilt",               CMD_StartTilt,              TRUE},
    {"StopTilt",                CMD_StopTilt,               TRUE},
    {"StartWiper",              CMD_StartWiper,             TRUE},
    {"SetLEDModeEx",            CMD_SetLEDModeEx,           TRUE},
    {"SetDefog",                CMD_SetDefog,               TRUE}

};

enum FPGA_IO_TYPE
{
    E_NORMAL_LED,	//类型：0-补光灯（地灯）1-告警输出 2-抓拍输出
    E_ALARM_OUT,
    E_TRIGGEROUT,
};

typedef struct _tFPGA_IO_ARG{
    DWORD type;
    DWORD enable;
    DWORD freq_num;
    DWORD polarity;
    DWORD pulse_width;
    DWORD output_type;
    DWORD coupling;
    _tFPGA_IO_ARG()
        : type(0)
        , enable(0)
        , freq_num(0)
        , polarity(0)
        , pulse_width(0)
        , output_type(0)
        , coupling(0)
    {
    }
}tFPGA_IO_ARG;

enum {
    IPC_CONTROL_CMD_GET_MAGIC_CODE,
    IPC_CONTROL_CMD_GET_CAPTURE_COUNT,
    IPC_CONTROL_CMD_SET_CAPTURE_COUNT
};

typedef struct COMMAND_HEAD_T
{
    unsigned int uCmdId;
    int iCmdInfoLength;
} COMMAND_HEAD;

typedef struct COMMAND_ACK_HEAD_T
{
    unsigned int uCmdId;
    int iCmdAck;
    int iCmdAckInfoLength;
} COMMAND_ACK_HEAD;

CSWProcessXMLCmd::CSWProcessXMLCmd()
{
    m_IsBackupMode = FALSE;
    m_fSupportNoBreakUpgrade = FALSE;
    m_fSupportCommand = FALSE;
    swpa_memset(m_szLastXMLCmd,0,MAX_COMMAND_LENGTH);
    m_dwDevType = DEVTYPE_VENUS;
    m_pComTestDevic = NULL;

    swpa_strcpy(m_szXMLCommandSockFile,"/tmp/ipc_client.sock");
}

HRESULT CSWProcessXMLCmd::Initialize(BOOL fBackupMode,INT nDevType)
{
    m_IsBackupMode = fBackupMode;
    m_dwDevType = nDevType;

    if(TRUE == m_IsBackupMode)
    {
        if (FAILED(m_cLocalTcpSock.Create(TRUE)))
        {
            SW_TRACE_NORMAL("Info: CSWProcessXMLCmd Create %s failed.", m_szXMLCommandSockFile);
            return E_FAIL;
        }

        if(IsVenus())
        {
            //跟进FPGA设置相机工作模式
            INT nKind = TYPE_FAKEIMG;
            OnGetFPGAMode(0,(LPARAM)&nKind,NULL,NULL,NULL);

            INT MODE = 0;
            if(TYPE_178 == nKind)
            {
                MODE = 6;
            }
            else if(TYPE_249 == nKind)
            {
                MODE = 16;
            }
            ipnc_cient_control(0, IPC_CONTROL_CMD_SET_WORKMODE + 100,(void*)&MODE , sizeof(INT), 0);

            //设置AGC基准值
            INT Value = 100;
            ipnc_cient_control(0, IPC_CONTROL_CMD_SET_AE_THRESHOLD +100,(void*)&Value , sizeof(INT), 0);

            INT iEnable = 1;
            //启动AGC
            OnSetAGCEnable((WPARAM)&iEnable,0,NULL,NULL,NULL);
            //启动AWB
            OnSetAWBEnable((WPARAM)&iEnable,0,NULL,NULL,NULL);

            //补光灯控制 EXP IO
            INT dwParam[2];	//polarity output_type
            dwParam[0] = 0; //补光灯输出极性
            dwParam[1] = 0;	//补光灯输出类型
            OnSetEXPIO((WPARAM)dwParam, 0,NULL,NULL,NULL);

            //触发输入
            INT iCaptureEdgeOne = 1; //上升沿触发
            OnSetTGIO((WPARAM)iCaptureEdgeOne,NULL,NULL,NULL,NULL);

            //触发输入
            OnSetF1IO((WPARAM)dwParam, 0,NULL,NULL,NULL);

            //报警输出 ALM IO
            OnSetALMIO((WPARAM)dwParam, 0,NULL,NULL,NULL);
        }

        if(IsJupiter() && FAILED(InitDomeCamera()))
        {
            SW_TRACE_NORMAL("Info: CSWProcessXMLCmd InitDomeCamera.", m_szXMLCommandSockFile);
            return E_FAIL;
        }
    }

    return S_OK;
}

CSWProcessXMLCmd::~CSWProcessXMLCmd()
{
    if(NULL != m_pComTestDevic)
    {
        m_pComTestDevic->Stop();
        m_pComTestDevic->Close();
        SAFE_RELEASE(m_pComTestDevic);
    }

    if(TRUE == m_IsBackupMode)
    {
        m_cLocalTcpSock.Close();
    }
}

HRESULT CSWProcessXMLCmd::ProcessXMLCmd(const PBYTE pbBuf, const DWORD dwSize, PVOID *ppvOutBuf, DWORD *pdwOutSize, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    HRESULT hr = S_OK;

    TiXmlDocument xmlDoc;

    if (NULL == pbBuf || 0 == dwSize || NULL == ppvOutBuf || NULL == pdwOutSize || NULL == iExitCode || NULL == fGoingToExit || NULL == fSwitchingSystem)
    {
        SW_TRACE_DEBUG("Err: NULL == pbBuf || 0 == dwSize || NULL == ppvOutBuf || NULL == pdwOutSize || NULL == iExitCode || NULL == fGoingToExit || NULL == fSwitchingSystem\n");
        return E_INVALIDARG;
    }

    TiXmlDocument  XmlDoc;

    *ppvOutBuf = NULL;
    *pdwOutSize = 0;

    TiXmlElement * pEleRoot = NULL;
    TiXmlDocument * pXmlOutputDoc = NULL;

    hr = GenerateReplyXMLHeader(&pXmlOutputDoc);
    if (FAILED(hr) || NULL == pXmlOutputDoc)
    {
        SW_TRACE_NORMAL("Err: failed to Generate Reply XML Header!!\n");

        SAFE_DELETE(pXmlOutputDoc);
        return E_FAIL;
    }

    m_fSupportCommand = TRUE;
    if (0 != XmlDoc.Parse((const CHAR*)pbBuf)
        && NULL != XmlDoc.RootElement()
        && NULL != XmlDoc.RootElement()->Value()
        && 0 == swpa_strcmp(XmlDoc.RootElement()->Value(), HVXML_HVCMD))
    {
        pEleRoot = XmlDoc.RootElement();
        for (TiXmlElement *Ele = pEleRoot->FirstChildElement(); Ele; Ele = Ele->NextSiblingElement())
        {
            if(NULL != Ele->GetText())
            {
                swpa_memset(m_szLastXMLCmd,0,MAX_COMMAND_LENGTH);
                swpa_strncpy(m_szLastXMLCmd,Ele->GetText(),swpa_strlen(Ele->GetText()));
            }

            BOOL fCmdProcessed = FALSE;
            for (INT i = 0; i < sizeof(g_XmlCmdMappingTable)/sizeof(g_XmlCmdMappingTable[0]); i++)
            {
                if (NULL != Ele->GetText()
                    && 0 == swpa_stricmp(Ele->GetText(), g_XmlCmdMappingTable[i].szCmdName))
                {
                    fCmdProcessed = TRUE;
                    //如果在正式系统，过滤某些命令
                    if(FALSE == m_IsBackupMode && TRUE == g_XmlCmdMappingTable[i].fOnlyBackup)
                    {
                        SW_TRACE_NORMAL("CSWProcessXMLCmd:cmd=%s Can't excute in Normal Mode.\n", Ele->GetText());
                        GenerateCMDProcessFailedXml(pXmlOutputDoc, Ele->GetText());
                        break;
                    }

                    /*为了保留以前命令的兼容，采用原实现方式处理以下命令*/
                    if(0 == swpa_stricmp(Ele->GetText(),"WriteSN"))
                    {
                        if (NULL == Ele->Attribute("SN"))
                        {
                            SW_TRACE_NORMAL("CSWProcessXMLCmd:CMD=WriteSN no SN info\n");
                            GenerateNoValueErrorXml(pXmlOutputDoc, Ele->GetText());
                        }
                        else
                        {
                            SW_TRACE_NORMAL("CSWProcessXMLCmd:Got WriteSN Cmd SN=%s.\n",Ele->Attribute("SN"));
                            if (FAILED(OnWriteSN(Ele->Attribute("SN"))))
                            {
                                SW_TRACE_NORMAL("CSWProcessXMLCmd:failed to Process WriteSN cmd.\n");
                                GenerateCMDProcessFailedXml(pXmlOutputDoc, Ele->GetText());
                            }
                            else if (FAILED(GenerateCMDReplyXml(pXmlOutputDoc, g_XmlCmdMappingTable[i].szCmdName, "INT", "Setter", NULL)))
                            {
                                GenerateCMDProcessFailedXml(pXmlOutputDoc, Ele->GetText());
                            }
                        }
                        break;
                    }
                    else
                    {
                        const CHAR * pszType = Ele->Attribute(HVXML_TYPE);
                        if (NULL == pszType)
                        {
                            SW_TRACE_NORMAL("CSWProcessXMLCmd:cmd=%s  No Type\n",  Ele->GetText());
                            GenerateNoTypeErrorXml(pXmlOutputDoc, Ele->GetText());
                            break;
                        }

                        const CHAR * pszClass = Ele->Attribute(HVXML_CLASS);
                        if (NULL == pszClass)
                        {
                            SW_TRACE_NORMAL("CSWProcessXMLCmd:cmd=%s  No Class\n",  Ele->GetText());
                            GenerateNoClassErrorXml(pXmlOutputDoc, Ele->GetText());
                            break;
                        }

                        const CHAR * pszValue = Ele->Attribute(HVXML_VALUE);
                        if (NULL == pszValue)
                        {
                            if (0 == swpa_strcmp(HVXML_SETTER, pszClass))
                            {
                                SW_TRACE_NORMAL("Err:cmd=%s  No Value",  Ele->GetText());
                                GenerateNoValueErrorXml(pXmlOutputDoc, Ele->GetText());
                                break;
                            }
                        }

                        SW_TRACE_NORMAL("CSWProcessXMLCmd: got %s Cmd Type=%s Class=%s Value=%s.\n",Ele->GetText(),pszType,pszClass,pszValue);

                        PVOID pvRetBuf = NULL;
                        hr = SendCMD(g_XmlCmdMappingTable[i].dwCmdID, pszValue, pszType, pszClass, &pvRetBuf,iExitCode,fGoingToExit,fSwitchingSystem);
                        if (FAILED(hr))
                        {
                            SW_TRACE_NORMAL("CSWProcessXMLCmd:failed to Process cmd=%s\n", Ele->GetText());
                            GenerateCMDProcessFailedXml(pXmlOutputDoc, Ele->GetText());
                            SAFE_MEM_FREE(pvRetBuf);
                            break;
                        }

                        if (FAILED(GenerateCMDReplyXml(pXmlOutputDoc, g_XmlCmdMappingTable[i].szCmdName, pszType, pszClass, pvRetBuf)))
                        {
                            SAFE_MEM_FREE(pvRetBuf);
                            GenerateCMDProcessFailedXml(pXmlOutputDoc, Ele->GetText());
                            break;
                        }
                        SAFE_MEM_FREE(pvRetBuf);
                    }
                }
            }

            if (!fCmdProcessed && NULL != Ele->GetText())
            {
                SW_TRACE_NORMAL("CSWProcessXMLCmd:cmd=%s not found.\n", Ele->GetText());
                m_fSupportCommand = FALSE;
                GenerateCMDNotFoundXml(pXmlOutputDoc, Ele->GetText());
                continue;
            }
        }
    }
    else
    {
        SW_TRACE_NORMAL("CSWProcessXMLCmd:Parse Xml Failed.\n");
        m_fSupportCommand = FALSE;
        GenerateCMDParseFailedXml(pXmlOutputDoc);
    }

    PackXml(pXmlOutputDoc, ppvOutBuf,  pdwOutSize);

    SAFE_DELETE(pXmlOutputDoc);

    return m_fSupportCommand==TRUE?S_OK:E_FAIL;
}

HRESULT CSWProcessXMLCmd::GenerateReplyXMLHeader(TiXmlDocument **ppXmlOutputDoc)
{
    HRESULT hr = S_OK;

    TiXmlDocument * pXmlOutputDoc = NULL;
    TiXmlDeclaration *pDeclaration = NULL;
    TiXmlElement *pRootEle = NULL;

    if (NULL == ppXmlOutputDoc)
    {
        hr = E_INVALIDARG;
        SW_TRACE_NORMAL("Err: NULL == ppXmlOutputDoc\n");
        goto OUT;
    }

    pXmlOutputDoc = new TiXmlDocument();
    if (NULL == pXmlOutputDoc)
    {
        SW_TRACE_NORMAL("Err: no memory for pXmlOutputDoc\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }

    pDeclaration = new TiXmlDeclaration("1.0","GB2312","yes");
    if (NULL == pDeclaration)
    {
        SW_TRACE_NORMAL("Err: no memory for pDeclaration\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }
    pXmlOutputDoc->LinkEndChild(pDeclaration);

    pRootEle = new TiXmlElement(HVXML_HVCMDRESPOND);
    if (NULL == pRootEle)
    {
        SW_TRACE_NORMAL("Err: no memory for pRootEle\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }

    pRootEle->SetAttribute(HVXML_VER, HV_XML_CMD_VERSION_NO);

    pXmlOutputDoc->LinkEndChild(pRootEle);

    *ppXmlOutputDoc = pXmlOutputDoc;

    return S_OK;

OUT:

    SAFE_DELETE(pXmlOutputDoc);

    return hr;
}

HRESULT CSWProcessXMLCmd::GenerateCMDProcessFailedXml(TiXmlDocument *pXmlOutputDoc, const CHAR *szCmdName)
{
    HRESULT hr = S_OK;

    if (NULL == pXmlOutputDoc || NULL == szCmdName)
    {
        SW_TRACE_NORMAL("Err: NULL == pXmlOutputDoc || NULL == szCmdName\n");
        return E_INVALIDARG;
    }


    TiXmlElement *pRootEle = NULL;
    TiXmlElement *pReplyEle = NULL;
    CHAR szVal[256] = {0};


    pRootEle = pXmlOutputDoc->RootElement();
    if (NULL == pRootEle)
    {
        SW_TRACE_NORMAL("Err: NULL == pRootEle\n");
        return E_INVALIDARG;
    }

    pReplyEle = new TiXmlElement(HVXML_CMDNAME);
    TiXmlText* pTxtCmd = new TiXmlText(szCmdName);
    pReplyEle->LinkEndChild(pTxtCmd);
    if (NULL == pReplyEle)
    {
        SW_TRACE_NORMAL("Err: no memory for pReplyEle\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }

    pReplyEle->SetAttribute(HVXML_RETCODE, -1);
    pReplyEle->SetAttribute(HVXML_RETMSG, "Failed to Process");

    pRootEle->LinkEndChild(pReplyEle);

    return S_OK;

OUT:

    SAFE_DELETE(pReplyEle);

    return hr;
}

HRESULT CSWProcessXMLCmd::GenerateNoTypeErrorXml(TiXmlDocument *pXmlOutputDoc, const CHAR *szCmdName)
{
    HRESULT hr = S_OK;

    if (NULL == pXmlOutputDoc || NULL == szCmdName)
    {
        SW_TRACE_NORMAL("Err: NULL == pXmlOutputDoc || NULL == szCmdName\n");
        return E_INVALIDARG;
    }


    TiXmlElement *pRootEle = NULL;
    TiXmlElement *pReplyEle = NULL;
    CHAR szVal[256] = {0};


    pRootEle = pXmlOutputDoc->RootElement();
    if (NULL == pRootEle)
    {
        SW_TRACE_NORMAL("Err: NULL == pRootEle\n");
        return E_INVALIDARG;
    }

    pReplyEle = new TiXmlElement(HVXML_CMDNAME);
    TiXmlText* pTxtCmd = new TiXmlText(szCmdName);
    pReplyEle->LinkEndChild(pTxtCmd);
    if (NULL == pReplyEle)
    {
        SW_TRACE_NORMAL("Err: no memory for pReplyEle\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }

    pReplyEle->SetAttribute(HVXML_RETCODE, -1);
    pReplyEle->SetAttribute(HVXML_RETMSG, "No \"Type\" Attribute");

    pRootEle->LinkEndChild(pReplyEle);

    return S_OK;

OUT:

    SAFE_DELETE(pReplyEle);

    return hr;
}

HRESULT CSWProcessXMLCmd::GenerateNoClassErrorXml(TiXmlDocument *pXmlOutputDoc, const CHAR *szCmdName)
{
    HRESULT hr = S_OK;

    if (NULL == pXmlOutputDoc || NULL == szCmdName)
    {
        SW_TRACE_NORMAL("Err: NULL == pXmlOutputDoc || NULL == szCmdName\n");
        return E_INVALIDARG;
    }


    TiXmlElement *pRootEle = NULL;
    TiXmlElement *pReplyEle = NULL;
    CHAR szVal[256] = {0};


    pRootEle = pXmlOutputDoc->RootElement();
    if (NULL == pRootEle)
    {
        SW_TRACE_NORMAL("Err: NULL == pRootEle\n");
        return E_INVALIDARG;
    }

    pReplyEle = new TiXmlElement(HVXML_CMDNAME);
    TiXmlText* pTxtCmd = new TiXmlText(szCmdName);
    pReplyEle->LinkEndChild(pTxtCmd);
    if (NULL == pReplyEle)
    {
        SW_TRACE_NORMAL("Err: no memory for pReplyEle\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }

    pReplyEle->SetAttribute(HVXML_RETCODE, -1);
    pReplyEle->SetAttribute(HVXML_RETMSG, "No \"Class\" Attribute");

    pRootEle->LinkEndChild(pReplyEle);

    return S_OK;

OUT:

    SAFE_DELETE(pReplyEle);

    return hr;
}

HRESULT CSWProcessXMLCmd::GenerateNoValueErrorXml(TiXmlDocument *pXmlOutputDoc, const CHAR *szCmdName)
{
    HRESULT hr = S_OK;

    if (NULL == pXmlOutputDoc || NULL == szCmdName)
    {
        SW_TRACE_NORMAL("Err: NULL == pXmlOutputDoc || NULL == szCmdName\n");
        return E_INVALIDARG;
    }


    TiXmlElement *pRootEle = NULL;
    TiXmlElement *pReplyEle = NULL;
    CHAR szVal[256] = {0};


    pRootEle = pXmlOutputDoc->RootElement();
    if (NULL == pRootEle)
    {
        SW_TRACE_NORMAL("Err: NULL == pRootEle\n");
        return E_INVALIDARG;
    }

    pReplyEle = new TiXmlElement(HVXML_CMDNAME);
    TiXmlText* pTxtCmd = new TiXmlText(szCmdName);
    pReplyEle->LinkEndChild(pTxtCmd);
    if (NULL == pReplyEle)
    {
        SW_TRACE_NORMAL("Err: no memory for pReplyEle\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }

    pReplyEle->SetAttribute(HVXML_RETCODE, -1);
    pReplyEle->SetAttribute(HVXML_RETMSG, "No \"Value\" Attribute for SETTER");

    pRootEle->LinkEndChild(pReplyEle);

    return S_OK;

OUT:

    SAFE_DELETE(pReplyEle);

    return hr;
}

HRESULT CSWProcessXMLCmd::GenerateCMDParseFailedXml(TiXmlDocument *pXmlOutputDoc)
{
    HRESULT hr = S_OK;

    if (NULL == pXmlOutputDoc)
    {
        SW_TRACE_NORMAL("Err: NULL == pXmlOutputDoc\n");
        return E_INVALIDARG;
    }

    TiXmlElement *pRootEle = NULL;
    pRootEle = pXmlOutputDoc->RootElement();
    if (NULL == pRootEle)
    {
        SW_TRACE_NORMAL("Err: NULL == pRootEle\n");
        return E_INVALIDARG;
    }

    pRootEle->SetAttribute(HVXML_RETCODE, -1);
    pRootEle->SetAttribute(HVXML_RETMSG, "Failed to Parse the XML");

    return S_OK;
}

HRESULT CSWProcessXMLCmd::GenerateCMDReplyXml(TiXmlDocument *pXmlOutputDoc, const CHAR *szCmdName, const CHAR *pszType, const CHAR *pszClass, const PVOID pvData)
{
    HRESULT hr = S_OK;

    if (NULL == pXmlOutputDoc || NULL == szCmdName || NULL == pszType || NULL == pszClass)//todo : NULL == pvData ||
    {
        SW_TRACE_NORMAL("Err: pXmlOutputDoc = %p, szCmdName = %p, pvData = %p, pszType = %p, pszClass = %p\n",
            pXmlOutputDoc, szCmdName, pvData, pszType, pszClass);
        return E_INVALIDARG;
    }

    TiXmlElement *pRootEle = NULL;
    TiXmlElement *pReplyEle = NULL;
    TiXmlText* pEleText = NULL;

    pRootEle = pXmlOutputDoc->RootElement();
    if (NULL == pRootEle)
    {
        SW_TRACE_NORMAL("Err: NULL == pRootEle\n");
        return E_INVALIDARG;
    }

    pReplyEle = new TiXmlElement(HVXML_CMDNAME);
    if (NULL == pReplyEle)
    {
        SW_TRACE_NORMAL("Err: no memory for pReplyEle\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }


    pEleText = new TiXmlText(szCmdName);
    if (NULL == pEleText)
    {
        SW_TRACE_NORMAL("Err: no memory for pEleText\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }
    pReplyEle->LinkEndChild(pEleText);

    pReplyEle->SetAttribute(HVXML_RETCODE, 0);


    if (0 == swpa_stricmp(pszClass, "Setter"))
    {
        pReplyEle->SetAttribute(HVXML_RETMSG, "OK");
    }
    else if (0 == swpa_stricmp(pszClass, "Getter"))
    {
        if (0 == swpa_strcmp(pszType, "INT"))
        {
            CHAR szVal[32] = {0};
            swpa_snprintf(szVal, sizeof(szVal)-1, "%d", *(INT*)pvData);
            pReplyEle->SetAttribute(HVXML_RETMSG, szVal);
        }
        else if (0 == swpa_strcmp(pszType, "DOUBLE"))
        {
            CHAR szVal[64] = {0};
            swpa_snprintf(szVal, sizeof(szVal)-1, "%lf", *(DOUBLE*)pvData);
            pReplyEle->SetAttribute(HVXML_RETMSG, szVal);
        }
        else if (0 == swpa_strcmp(pszType, "FLOAT"))
        {
            CHAR szVal[64] = {0};
            swpa_snprintf(szVal, sizeof(szVal)-1, "%f",  *(FLOAT*)pvData);
            pReplyEle->SetAttribute(HVXML_RETMSG, szVal);
        }
        else if (0 == swpa_strcmp(pszType, "BOOL"))
        {
            CHAR szVal[8] = {0};
            swpa_snprintf(szVal, sizeof(szVal)-1, "%d", *(BOOL*)pvData);
            pReplyEle->SetAttribute(HVXML_RETMSG, szVal);
        }
        else if (0 == swpa_strcmp(pszType, "DWORD"))
        {
            CHAR szVal[32] = {0};
            swpa_snprintf(szVal, sizeof(szVal)-1, "%u", *(DWORD*)pvData);
            pReplyEle->SetAttribute(HVXML_RETMSG, szVal);
        }
        else if (0 == swpa_strcmp(pszType, "STRING"))
        {
            CHAR* szVal = (CHAR*)pvData;
            //swpa_snprintf(szVal, sizeof(szVal)-1, "%s", (CHAR*)pvData);
            pReplyEle->SetAttribute(HVXML_RETMSG, szVal);
        }
        else if (0 == swpa_strcmp(pszType, "NULL"))
        {
            pReplyEle->SetAttribute(HVXML_RETMSG, "OK");
        }
        else if (0 == swpa_strcmp(pszType, "CUSTOM"))
        {

            if (0 == swpa_strcmp("DateTime", szCmdName))
            {
                pReplyEle->SetAttribute(HVXML_RETMSG, (CHAR*)pvData);
            }
            else if (0 == swpa_strcmp("SetTime", szCmdName))
            {
                pReplyEle->SetAttribute(HVXML_RETMSG, "OK");
            }
            else if (0 == swpa_strcmp("GetIP", szCmdName))
            {
                pReplyEle->SetAttribute(HVXML_RETMSG, (CHAR*)pvData);
            }
            else if (0 == swpa_strcmp("SetIP", szCmdName))
            {
                pReplyEle->SetAttribute(HVXML_RETMSG, "OK");
            }
            else if (0 == swpa_strcmp("GetVersion", szCmdName))
            {
                pReplyEle->SetAttribute(HVXML_RETMSG, (CHAR*)pvData);
            }
            else if (0 == swpa_strcmp("GetDevType", szCmdName))
            {
                pReplyEle->SetAttribute(HVXML_RETMSG, (CHAR*)pvData);
            }
            else if( 0 == swpa_strcmp("GetConnectedIP", szCmdName) )
            {
                pReplyEle->SetAttribute(HVXML_RETMSG, (CHAR*)pvData);
            }
            else if( 0 == swpa_strcmp("ReadFPGA", szCmdName) )
            {
                CHAR szVal[32] = {0};
                swpa_snprintf(szVal, sizeof(szVal)-1, "%d", *(INT*)pvData);
                pReplyEle->SetAttribute(HVXML_RETMSG, (CHAR*)szVal);
            }
            else
                pReplyEle->SetAttribute(HVXML_RETMSG, (CHAR*)pvData);


        }
        else if (NULL != swpa_strstr(pszType, "INTARRAY1D"))
        {
            INT iArraySize = *((INT*)pvData);
            //SW_TRACE_NORMAL("%s %d INTARRAY1D %d \n",__FUNCTION__,__LINE__,iArraySize);
            DWORD dwLen = (iArraySize) * 32 + (iArraySize)*sizeof(',') + 1;//todo
            CHAR* szVal = (CHAR*)swpa_mem_alloc(dwLen);
            if (NULL == szVal)
            {
                SW_TRACE_NORMAL("Err: no memory for szVal\n");
                return E_OUTOFMEMORY;
            }
            swpa_memset(szVal, 0, dwLen);
            //SW_TRACE_NORMAL("%s %d INTARRAY1D size :%d \n",__FUNCTION__,__LINE__,iArraySize);
            for (INT i=0; i<iArraySize;  i++)
            {
                CHAR szValInt[16] = {0};
                swpa_memset(szValInt, 0, sizeof(szValInt));
                swpa_snprintf(szValInt, sizeof(szValInt),"%d,", *((INT*)pvData+1+i));
                swpa_strcat(szVal, szValInt);
            }
            //SW_TRACE_NORMAL("%s %d INTARRAY1D szVal :%s \n",__FUNCTION__,__LINE__,szVal);
            pReplyEle->SetAttribute(HVXML_RETMSG, szVal);
            SAFE_MEM_FREE(szVal);
        }
        else if (NULL != swpa_strstr(pszType, "INTARRAY2D"))
        {
            //todo
        }

    }


    pRootEle->LinkEndChild(pReplyEle);


    return S_OK;

OUT:

    SAFE_DELETE(pReplyEle);

    return hr;
}

HRESULT CSWProcessXMLCmd::GenerateCMDNotFoundXml(TiXmlDocument *pXmlOutputDoc, const CHAR *szCmdName)
{
    HRESULT hr = S_OK;

    if (NULL == pXmlOutputDoc || NULL == szCmdName)
    {
        SW_TRACE_NORMAL("Err: NULL == pXmlOutputDoc || NULL == szCmdName\n");
        return E_INVALIDARG;
    }


    TiXmlElement *pRootEle = NULL;
    TiXmlElement *pReplyEle = NULL;
    CHAR szVal[256] = {0};


    pRootEle = pXmlOutputDoc->RootElement();
    if (NULL == pRootEle)
    {
        SW_TRACE_NORMAL("Err: NULL == pRootEle\n");
        return E_INVALIDARG;
    }

    pReplyEle = new TiXmlElement(HVXML_CMDNAME);
    TiXmlText* pTxtCmd = new TiXmlText(szCmdName);
    pReplyEle->LinkEndChild(pTxtCmd);
    if (NULL == pReplyEle)
    {
        SW_TRACE_NORMAL("Err: no memory for pReplyEle\n");
        hr = E_OUTOFMEMORY;
        goto OUT;
    }

    pReplyEle->SetAttribute(HVXML_RETCODE, -1);
    pReplyEle->SetAttribute(HVXML_RETMSG, "Command Not Found");

    pRootEle->LinkEndChild(pReplyEle);

    return S_OK;

OUT:

    SAFE_DELETE(pReplyEle);

    return hr;
}

HRESULT CSWProcessXMLCmd::PackXml(TiXmlDocument *pXmlOutputDoc, PVOID *ppvXMLOutBuf, DWORD *pdwXMLOutSize)
{
    if (NULL == pXmlOutputDoc || NULL == ppvXMLOutBuf || NULL == pdwXMLOutSize)
    {
        SW_TRACE_NORMAL("Err: NULL == pXmlOutputDoc || NULL == ppvXMLOutBuf || NULL == pdwXMLOutSize\n");
        return E_INVALIDARG;
    }

    TiXmlPrinter XmlPrinter;
    pXmlOutputDoc->Accept(&XmlPrinter);

    INT nInfoSize = XmlPrinter.Size() + 1;
    CHAR* pszXmlData = (CHAR*)swpa_mem_alloc(nInfoSize);
    if (NULL == pszXmlData)
    {
        SW_TRACE_NORMAL("Err: no memory for pszXmlData\n");
        return E_OUTOFMEMORY;
    }
    swpa_memset(pszXmlData, 0x0, nInfoSize);
    swpa_memcpy(pszXmlData, XmlPrinter.CStr(), XmlPrinter.Size());

    *ppvXMLOutBuf = pszXmlData;
    *pdwXMLOutSize = nInfoSize;

    return S_OK;
}

HRESULT CSWProcessXMLCmd::SendCMD(const DWORD dwCMDID, const CHAR *szValue, const CHAR *szType, const CHAR *szClass, PVOID *ppvRetBuf,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem)
{
    HRESULT hr = S_OK;
    CHAR * pszType = (CHAR*)szType;
    CHAR * pszValue = (CHAR*)szValue;

    SAFE_MEM_FREE(*ppvRetBuf); //make sure "NULL == *ppvRetBuf"

    if (0 == swpa_strcmp(pszType, "INT"))
    {
        INT iVal = 0;

        if (0 == swpa_stricmp(szClass, "Getter"))
        {
            *ppvRetBuf = swpa_mem_alloc(sizeof(INT));
            if (NULL == *ppvRetBuf)
            {
                SW_TRACE_NORMAL("Err: no memory for *ppvRetBuf\n");
                return E_FAIL;
            }
        }
        else if (0 == swpa_stricmp(szClass, "Setter"))
        {
            if (NULL != pszValue)
            {
                iVal = swpa_atoi(pszValue);
            }
            else
            {
                SW_TRACE_NORMAL("Err: NULL == pszValue for Setter\n");
                return E_INVALIDARG;
            }
        }

        hr = OnCommand(dwCMDID,
            (WPARAM)iVal,
            (LPARAM)*ppvRetBuf,
            iExitCode,
            fGoingToExit,
            fSwitchingSystem);

    }
    else if (0 == swpa_strcmp(pszType, "DOUBLE"))
    {
        DOUBLE dblVal = 0;
        if (0 == swpa_stricmp(szClass, "Getter"))
        {
            *ppvRetBuf = swpa_mem_alloc(sizeof(DOUBLE));
            if (NULL == *ppvRetBuf)
            {
                SW_TRACE_NORMAL("Err: no memory for *ppvRetBuf\n");
                return E_FAIL;
            }
        }
        else if (0 == swpa_stricmp(szClass, "Setter"))
        {
            if (NULL != pszValue)
            {
                dblVal = swpa_atof(pszValue);
            }
            else
            {
                SW_TRACE_NORMAL("Err: NULL == pszValue for Setter\n");
                return E_INVALIDARG;
            }
        }
        hr = OnCommand(dwCMDID,
                       (WPARAM)dblVal,
                       (LPARAM)*ppvRetBuf,
                       iExitCode,
                       fGoingToExit,
                       fSwitchingSystem);
    }
    else if (0 == swpa_strcmp(pszType, "FLOAT"))
    {
        FLOAT fltVal = 0;

        if (0 == swpa_stricmp(szClass, "Getter"))
        {
            *ppvRetBuf = swpa_mem_alloc(sizeof(FLOAT));
            if (NULL == *ppvRetBuf)
            {
                SW_TRACE_NORMAL("Err: no memory for *ppvRetBuf\n");
                return E_FAIL;
            }
        }
        else if (0 == swpa_stricmp(szClass, "Setter"))
        {
            if (NULL != pszValue)
            {
                fltVal = swpa_atof(pszValue);
            }
            else
            {
                SW_TRACE_NORMAL("Err: NULL == pszValue for Setter\n");
                return E_INVALIDARG;
            }
        }
        hr = OnCommand(dwCMDID,
                       (WPARAM)fltVal,
                       (LPARAM)*ppvRetBuf,
                       iExitCode,
                       fGoingToExit,
                       fSwitchingSystem);
    }
    else if (0 == swpa_strcmp(pszType, "BOOL"))
    {
        BOOL boolVal = 0;

        if (0 == swpa_stricmp(szClass, "Getter"))
        {
            *ppvRetBuf = swpa_mem_alloc(sizeof(BOOL));
            if (NULL == *ppvRetBuf)
            {
                SW_TRACE_NORMAL("Err: no memory for *ppvRetBuf\n");
                return E_FAIL;
            }
        }
        else if (0 == swpa_stricmp(szClass, "Setter"))
        {
            if (NULL != pszValue)
            {
                boolVal = swpa_atoi(pszValue);
            }
            else
            {
                SW_TRACE_NORMAL("Err: NULL == pszValue for Setter\n");
                return E_INVALIDARG;
            }
        }
        hr = OnCommand(dwCMDID,
                       (WPARAM)boolVal,
                       (LPARAM)*ppvRetBuf,
                       iExitCode,
                       fGoingToExit,
                       fSwitchingSystem);
    }
    else if (0 == swpa_strcmp(pszType, "DWORD"))
    {
        DWORD dwVal = 0;

        if (0 == swpa_stricmp(szClass, "Getter"))
        {
            *ppvRetBuf = swpa_mem_alloc(sizeof(DWORD));
            if (NULL == *ppvRetBuf)
            {
                SW_TRACE_NORMAL("Err: no memory for *ppvRetBuf\n");
                return E_FAIL;
            }
        }
        else if (0 == swpa_stricmp(szClass, "Setter"))
        {
            if (NULL != pszValue)
            {
                dwVal = swpa_atoi(pszValue);
            }
            else
            {
                SW_TRACE_NORMAL("Err: NULL == pszValue for Setter\n");
                return E_INVALIDARG;
            }
        }
        hr = OnCommand(dwCMDID,
                       (WPARAM)dwVal,
                       (LPARAM)*ppvRetBuf,
                       iExitCode,
                       fGoingToExit,
                       fSwitchingSystem);
    }
    else if (0 == swpa_strcmp(pszType, "STRING"))
    {
        if (0 == swpa_stricmp(szClass, "Getter"))
        {
            *ppvRetBuf = swpa_mem_alloc(16*1024); //16KB by default
            if (NULL == *ppvRetBuf)
            {
                SW_TRACE_NORMAL("Err: no memory for *ppvRetBuf\n");
                return E_FAIL;
            }
            swpa_memset(*ppvRetBuf, 0, 16*1024);
        }
        else if (0 == swpa_stricmp(szClass, "Setter"))
        {
            if (NULL != pszValue)
            {
                //szVal = pszValue;
            }
            else
            {
                SW_TRACE_NORMAL("Err: NULL == pszValue for Setter\n");
                return E_INVALIDARG;
            }
        }
        hr = OnCommand(dwCMDID,
                       (WPARAM)pszValue,
                       (LPARAM)*ppvRetBuf,
                       iExitCode,
                       fGoingToExit,
                       fSwitchingSystem);
    }
    else if (0 == swpa_strcmp(pszType, "NULL"))
    {
        hr = OnCommand(dwCMDID,
                       (WPARAM)NULL,
                       (LPARAM)NULL,
                       iExitCode,
                       fGoingToExit,
                       fSwitchingSystem);
    }
    else if (0 == swpa_strcmp(pszType, "CUSTOM"))
    {
        //SW_TRACE_NORMAL("Info: got CUSTOM type...\n");
        if (FAILED(SendCustomCMD(dwCMDID, szValue, ppvRetBuf,iExitCode,fGoingToExit,fSwitchingSystem)))
        {
            SW_TRACE_DEBUG("Err: failed to send Custom CMD %d.\n", dwCMDID);
            return E_FAIL;
        }
    }
    else if (NULL != swpa_strstr(pszType, "INTARRAY1D"))
    {
        INT iArraySize = 0;

        if (NULL == pszValue)
        {
            SW_TRACE_NORMAL("Err: NULL == pszValue\n");
            return E_INVALIDARG;
        }

        swpa_sscanf(pszType, "INTARRAY1D,SIZE:%d", &iArraySize);

        INT * piInput = NULL;
        INT* piBuf = NULL;
        if (0 == swpa_stricmp(szClass, "Getter"))
        {
            piBuf = (INT*)swpa_mem_alloc(iArraySize * sizeof(INT) + sizeof(INT));
            if (NULL == piBuf)
            {
                SW_TRACE_NORMAL("Err: no memory for piBuf\n");
                return E_FAIL;
            }
            piBuf[0] = iArraySize;
            *ppvRetBuf = piBuf;

            hr = OnCommand(dwCMDID,
                           (WPARAM)piInput,
                           (LPARAM)(piBuf+1),
                           iExitCode,
                           fGoingToExit,
                           fSwitchingSystem);
        }
        else if (0 == swpa_stricmp(szClass, "Setter"))
        {
            if (NULL != pszValue)
            {
                piInput = (INT*)swpa_mem_alloc(iArraySize * sizeof(INT));
                swpa_memset(piInput, 0, sizeof(iArraySize * sizeof(INT)));
                if (NULL == piInput)
                {
                    SW_TRACE_NORMAL("Err: no memory for piInput\n");
                    hr = E_OUTOFMEMORY;
                    return hr;
                }

                for (INT i = 0; i < iArraySize; i++)
                {
                    if (NULL == pszValue)
                    {
                        hr = E_FAIL;
                        SAFE_MEM_FREE(piInput);
                        return hr;
                    }

                    swpa_sscanf(pszValue, "%d", piInput+i);
                    if (i+1 < iArraySize)
                    {
                        pszValue = swpa_strstr(pszValue, ",") + 1;
                    }
                }

                hr = OnCommand(dwCMDID,
                               (WPARAM)piInput,
                               0,
                               iExitCode,
                               fGoingToExit,
                               fSwitchingSystem);
            }
            else
            {
                SW_TRACE_NORMAL("Err: NULL == pszValue for Setter\n");
                return E_INVALIDARG;
            }
        }

       // SW_TRACE_NORMAL("%s %d  %d %d %d \n",__FUNCTION__,__LINE__,piBuf[1],piBuf[2],piBuf[3]);
        SAFE_MEM_FREE(piInput);

    }
    else if (0 == swpa_strcmp(pszType, "INTARRAY2D"))
    {
        INT iWidth = 0, iHeight = 0;

        if (NULL == pszValue)
        {
            SW_TRACE_NORMAL("Err: NULL == pszValue\n");
            return E_INVALIDARG;
        }

        swpa_sscanf(pszValue, "SIZE:%d*%d", &iHeight, &iWidth);
        pszValue = swpa_strstr(pszValue, "DATA:") + 5;
        INT * piInput = (INT*)swpa_mem_alloc(iHeight* iWidth * sizeof(INT));
        if (NULL == piInput)
        {
            SW_TRACE_NORMAL("Err: no memory for piInput\n");
            hr = E_OUTOFMEMORY;
            return hr;
        }
        swpa_memset(piInput, 0, sizeof(iHeight* iWidth	* sizeof(INT)));

        for (INT i = 0; i < iHeight; i++)
        {
            for (INT j = 0; j < iWidth; j++)
            {
                if (NULL == pszValue)
                {
                    hr = E_FAIL;
                    SW_TRACE_NORMAL("Err: NULL == pszValue\n");
                    SAFE_MEM_FREE(piInput);
                    return hr;
                }

                swpa_sscanf(pszValue, "%d", piInput+i);
                pszValue = swpa_strstr(pszValue+1, ",");
            }
        }

        hr = OnCommand(dwCMDID,
                       (WPARAM)piInput,
                       (LPARAM)*ppvRetBuf,
                       iExitCode,
                       fGoingToExit,
                       fSwitchingSystem);

        SAFE_MEM_FREE(piInput);
    }

    return hr;
}

HRESULT CSWProcessXMLCmd::SendCustomCMD(const DWORD dwCMDID, const CHAR * szValue, PVOID* ppvRetBuf,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem)
{
    HRESULT hr = S_OK;

    switch (dwCMDID)
    {
        case CMD_GetTime:
        {
            SWPA_DATETIME_TM sRealTime;
            swpa_memset(&sRealTime, 0, sizeof(sRealTime));
            hr = OnCommand(dwCMDID,
                           (WPARAM)NULL,
                           (LPARAM)&sRealTime,
                           iExitCode,
                           fGoingToExit,
                           fSwitchingSystem);

            if (SUCCEEDED(hr))
            {
                CHAR szStr[256] = {0};
                swpa_snprintf(szStr, sizeof(szStr)-1, "Date[%4d.%02d.%02d],Time[%02d:%02d:%02d %03d]",
                    sRealTime.year, sRealTime.month, sRealTime.day, sRealTime.hour, sRealTime.min, sRealTime.sec, sRealTime.msec);
                *ppvRetBuf = swpa_mem_alloc(swpa_strlen(szStr)+1);
                if (NULL == *ppvRetBuf)
                {
                    SW_TRACE_NORMAL("Err: no memory for *ppvRetBuf\n");
                    hr = E_OUTOFMEMORY;
                    break;
                }
                swpa_memset(*ppvRetBuf, 0, swpa_strlen(szStr)+1);
                swpa_strncpy((CHAR*)*ppvRetBuf, szStr, swpa_strlen(szStr));
            }
        }
        break;
        case CMD_SetTime:
        {
            SWPA_DATETIME_TM sRealTime;
            swpa_memset(&sRealTime, 0, sizeof(sRealTime));

            SW_TRACE_DEBUG("<settime>%s.\n", szValue);
            INT iY, iMon, iD, iH, iM, iS, iMS, iTZ=0;//水星iTZ=8 金星=0
            swpa_sscanf(szValue, "Date[%4d.%2d.%2d],Time[%2d:%2d:%2d %03d]",// UTC%d",
                    &iY, &iMon, &iD, &iH, &iM, &iS, &iMS);//, &iTZ);
            sRealTime.year = (short)iY;
            sRealTime.month = (short)iMon;
            sRealTime.day = (short)iD;
            sRealTime.hour = (short)iH;
            sRealTime.min = (short)iM;
            sRealTime.sec = (short)iS;
            sRealTime.msec = (short)iMS;
            SW_TRACE_DEBUG("<settime>%d-%d-%d %d:%d:%d %d.\n",iY, iMon, iD, iH, iM, iS, iMS);
            if (iY < 2000/*1970*/)
            {
                SW_TRACE_NORMAL("<settime> cannot set Year to %d.\n",iY);
                hr = E_INVALIDARG;
            }
            else
            {
                SWPA_TIME sTime, sCurTime;
                CSWDateTime cCurTime;

                cCurTime.GetTime(&sCurTime);

                swpa_datetime_tm2time(sRealTime, &sTime);

                if (sCurTime.sec <= sTime.sec + 3
                    && sCurTime.sec >= sTime.sec - 3)
                {
                    SW_TRACE_DEBUG("Info: no need to sync time.\n");
                    hr = S_OK;
                }
                else
                {
                    hr = OnCommand(dwCMDID,
                                   (WPARAM)&sRealTime,
                                   (LPARAM)NULL,
                                   iExitCode,
                                   fGoingToExit,
                                   fSwitchingSystem);
                }
            }
        }
        break;
        default:
        {
            hr = E_FAIL;
        }
        break;
    }

    return hr;
}

HRESULT CSWProcessXMLCmd::GetVersion(const CHAR *TAG_STR, CHAR *szVersion)
{
    HRESULT hr=S_OK;
    swpa_strcpy(szVersion, "unknown");

    CHAR szVerInfo[512] = {0};
    INT iLen = sizeof(szVerInfo);
    if (SWPAR_OK == swpa_info_get_firmware_version(szVerInfo, &iLen))
    {
        SW_TRACE_DEBUG("Info: szVerInfo = %s\n", szVerInfo);
        CHAR * szSub = swpa_strstr(szVerInfo, TAG_STR);
        if (NULL != szSub)
        {
            CHAR * szEnd = swpa_strstr(szSub, ";");
            CHAR szVer[256] = {0};
            swpa_strncpy(szVer, szSub+swpa_strlen(TAG_STR)+1, (DWORD)szEnd - (DWORD)szSub  - swpa_strlen(TAG_STR) - 1);
            SW_TRACE_DEBUG("Info: Ver = %s\n", szVer);
            swpa_strcpy(szVersion, szVer);
            hr = S_OK;
        }
    }
    else
    {
        SW_TRACE_DEBUG("Err: failed to get firmware version info\n");
        hr = E_FAIL;
    }

    return hr;
}

HRESULT CSWProcessXMLCmd::ipnc_cient_control(int linkId, int cmd, void *pPrm, int prmSize, int timeout)
{
    HRESULT hr = E_FAIL;

    if(!m_cLocalTcpSock.IsConnect() && FAILED(m_cLocalTcpSock.Connect(m_szXMLCommandSockFile)))
    {
        SW_TRACE_NORMAL("Info: CSWProcessXMLCmd Connect %s failed.", m_szXMLCommandSockFile);
        return E_FAIL;
    }

    m_cLocalTcpSock.SetRecvTimeout(3000);
    m_cLocalTcpSock.SetSendTimeout(3000);

    COMMAND_HEAD cHead;
    cHead.uCmdId = cmd;
    cHead.iCmdInfoLength = prmSize;
    DWORD dwDataLength = 0;
    hr = m_cLocalTcpSock.Send((void*)&cHead, sizeof(cHead),&dwDataLength);
    if (FAILED(hr) || dwDataLength != sizeof(cHead))
    {
        SW_TRACE_DEBUG("[CSWProcessXMLCmd]: failed to send Command head cmd:%d,hr:0x%08x,Len:%d Sent:%d\n",
            cmd,hr,sizeof(cHead),dwDataLength);
        return E_FAIL;
    }

    dwDataLength = 0;
    hr = m_cLocalTcpSock.Send(pPrm, prmSize,&dwDataLength);
    if (FAILED(hr) || dwDataLength != prmSize)
    {
        SW_TRACE_DEBUG("[CSWProcessXMLCmd]: failed to send Command Info cmd:%d,hr:0x%08x,Len:%d Sent:%d\n",
            cmd,hr,prmSize,dwDataLength);
        return E_FAIL;
    }

    dwDataLength = 0;
    COMMAND_ACK_HEAD ack;
    hr = m_cLocalTcpSock.Read((void*)&ack,sizeof(ack),&dwDataLength);
    if (FAILED(hr) || dwDataLength != sizeof(ack))
    {
        SW_TRACE_DEBUG("[CSWProcessXMLCmd]: failed to read Command respond head cmd:%d,hr:0x%08x,Len:%d Read:%d\n",
            cmd,hr,sizeof(ack),dwDataLength);
        return E_FAIL;
    }

    if(0 != ack.iCmdAck)
    {
        SW_TRACE_DEBUG("[CSWProcessXMLCmd]: Command M3 Process Fail uCmdId:%d,ack:%d,iCmdAckInfoLength:%d\n",
            ack.uCmdId,ack.iCmdAck,ack.iCmdAckInfoLength);
        return E_FAIL;
    }

    if(ack.iCmdAckInfoLength > 0 && ack.iCmdAckInfoLength <= prmSize)
    {
        hr = m_cLocalTcpSock.Read(pPrm,prmSize,&dwDataLength);
        if (FAILED(hr))
        {
            SW_TRACE_DEBUG("[CSWProcessXMLCmd]: failed to read Command respond info cmd%d,hr:0x%08x,Len:%d Read:%d\n",
                           cmd,hr,prmSize,dwDataLength);
            return E_FAIL;
        }
    }

    hr = S_OK;
}

HRESULT CSWProcessXMLCmd::InitDomeCamera()
{
    if (SWPAR_OK != swpa_camera_init())
    {
        SW_TRACE_DEBUG("Err: failed to init camera\n");
        return E_FAIL;
    }

    if (SWPAR_OK != swpa_camera_set_power(1))
    {
        SW_TRACE_DEBUG("Err: failed to set camera power to 1\n");
        return E_FAIL;
    }

    // 读取机芯初始参数，判断是否需要设置，一般在生产时已设置。
    // 需断电上电才能生效。
    MONITOR_MODE  mMode;
    int iSyncMode;
    if( 0 != swpa_camera_get_monitoring_mode(&mMode)
            || mMode != MODE_1080P_25 )
    {
        SW_TRACE_DEBUG("Err: failed to get monitoring_mode %d.\n", MODE_1080P_25);
    }
    swpa_camera_set_monitoring_mode(MODE_1080P_25);

    if( 0 != swpa_camera_get_sync_mode(&iSyncMode)
            || iSyncMode != 0)
    {
        SW_TRACE_DEBUG("Err: failed to get sync_mode 0.\n");
    }
    swpa_camera_set_sync_mode(0);

    // 禁止数字变焦
    swpa_camera_basicparam_dzoom_enable(0);

    // 默认使能High Resulotion模式
    if (SWPAR_OK != swpa_camera_basicparam_set_hrmode(1))
    {
        SW_TRACE_DEBUG("Err: failed to set HR mode to %d!\n", 1);
        return E_FAIL;
    }

    // 默认使能曝光补偿模式
    if (SWPAR_OK != swpa_camera_basicparam_set_expcomp_mode(1))
    {
        SW_TRACE_DEBUG("Err: failed to set exposure compensation mode to %d!\n", 1);
        return E_FAIL;
    }

    DWORD dwSNFEnable = FALSE, dwTNFEnable=FALSE;
    if( 0 != swpa_ipnc_control(0, CMD_SET_TNF_STATE ,(void*)&dwTNFEnable , sizeof(DWORD), 0)
        || 0 != swpa_ipnc_control(0, CMD_SET_SNF_STATE ,(void*)&dwSNFEnable , sizeof(DWORD), 0)
        )
    {
        return E_FAIL;
    }

    //默认 不使能防抖功能
    if (SWPAR_OK != swpa_camera_basicparam_set_stabilizer(0))
    {
        SW_TRACE_DEBUG("err: failed to enable stabilizer!\n");
    }

    // 同步焦距
    if (SWPAR_OK != swpa_camera_ball_sync_zoom())
    {
        swpa_thread_sleep_ms(200);
        if (SWPAR_OK != swpa_camera_ball_sync_zoom())
        {
            SW_TRACE_DEBUG("err: failed to sync zoom!\n");
        }
    }

    return S_OK;
}

HRESULT CSWProcessXMLCmd::OnWriteSN(const CHAR *szSN)
{
    if (NULL == szSN)
    {
        SW_TRACE_DEBUG("Err: NULL == szSN\n");
        return E_INVALIDARG;
    }

    if (strlen(szSN) > 255)
    {
        SW_TRACE_DEBUG("Err: sn too long!\n");
        return E_ABORT;
    }

    CHAR szSNtoSave[256] = {0};
    swpa_strncpy(szSNtoSave, szSN, strlen(szSN)+1);
    if (SWPAR_OK == swpa_device_write_sn((CHAR*)szSNtoSave))
    {
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
}

HRESULT CSWProcessXMLCmd::OnResetDevice(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem)
{
    INT nExitCode = (INT)wParam;
    SW_TRACE_NORMAL("Info: iExitCode = %d\n", iExitCode);

    if (1 == nExitCode) //switch to Normal Mode(used only after Upgrade being finished), need to clear the boot count
    {
        swpa_device_set_bootsystem(0);
        swpa_device_set_normal_bootcount(0);

        if (SWPAR_OK != swpa_device_set_resetmode(0))
        {
            SW_TRACE_DEBUG("Err: failed to set working_mode to normal!\n");
            return E_FAIL;
        }

        // 设置启动计数标志
        if( SWPAR_OK == swpa_device_set_bootcount_flag(1) )
        {
            SW_TRACE_DEBUG("info: set boot flag 1 ok.\n");
            *fSwitchingSystem = TRUE;
        }
        else
        {
            SW_TRACE_DEBUG("Err: set boot flag 1 failed.\n");
        }

        *iExitCode = 2; //force to reboot after Upgrade
        *fGoingToExit = TRUE;
    }
    else if (2 == nExitCode) //Reboot
    {
        *iExitCode = nExitCode;
        *fGoingToExit = TRUE;
    }
    else if (3 == nExitCode) //switch to Normal Mode(response to switch Factory-Mode to Normal-Mode), need to clear the boot count
    {
        if (m_IsBackupMode)
        {
            swpa_device_set_bootsystem(0);
            swpa_device_set_normal_bootcount(0);

            if (SWPAR_OK != swpa_device_set_resetmode(0))
            {
                SW_TRACE_DEBUG("Err: failed to set working_mode to normal!\n");
                return E_FAIL;
            }

            // 设置启动计数标志
            if( SWPAR_OK == swpa_device_set_bootcount_flag(1) )
            {
                SW_TRACE_DEBUG("info: set boot flag 1 ok.\n");
                *fSwitchingSystem = TRUE;
            }
            else
            {
                SW_TRACE_DEBUG("Err: set boot flag 1 failed.\n");
            }

            *iExitCode = 2; //force to reboot
            *fGoingToExit = TRUE;
        }
    }
    else if (4 == nExitCode) //switch to Factory Mode, need to clear the boot count
    {
        if (!m_IsBackupMode)
        {
            swpa_device_set_bootsystem(1);
            swpa_device_set_backup_bootcount(0);

            swpa_device_set_resetmode(0);

            // 设置启动计数标志
            if( SWPAR_OK == swpa_device_set_bootcount_flag(1) )
            {
                SW_TRACE_DEBUG("info: set boot flag 1 ok.\n");
                *fSwitchingSystem = TRUE;
            }
            else
            {
                SW_TRACE_DEBUG("Err: set boot flag 1 failed.\n");
            }

            *iExitCode = 2; //force to reboot
            *fGoingToExit = TRUE;
        }
    }

    return S_OK;
}

HRESULT CSWProcessXMLCmd::OnOptResetMode(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT iMode = 0;
    INT iFlag = 1;
    if(m_fSupportNoBreakUpgrade /*支持不复位设备连续升级*/
        || (SWPAR_OK == swpa_device_get_bootcount_flag(&iFlag) && iFlag == 0) /*已正常运行一定时间*/
        )
    {
        if (m_IsBackupMode)
        {
            iMode = 1;
        }
        else if (SWPAR_OK != swpa_device_get_resetmode(&iMode))
        {
            SW_TRACE_NORMAL("Err: failed to get reset mode value!\n");
            iMode = -1;
        }
    }
    else
    {
        iMode = -1;
    }

    if (-1 == iMode)
    {
        return E_FAIL;
    }
    else
    {
        INT* tmp = (INT*)lParam;
        *tmp = iMode;
         return S_OK;
    }
}

HRESULT CSWProcessXMLCmd::OnRestoreFactorySetting(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (SWPAR_OK == swpa_device_lock_uboot()
        && SWPAR_OK == swpa_device_lock_linuxinfo()
        && SWPAR_OK == swpa_device_set_bootdelay(0)
        && SWPAR_OK == swpa_device_restore_defaultinfo()
        )
    {
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
}

HRESULT CSWProcessXMLCmd::OnUnlockFactorySetting(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (SWPAR_OK == swpa_device_unlock_uboot()
        && SWPAR_OK == swpa_device_unlock_linuxinfo()
        //&& SWPAR_OK == swpa_device_set_bootdelay(3)
        )
    {
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
}

HRESULT CSWProcessXMLCmd::OnSetTime(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    HRESULT hr = E_FAIL;
    SWPA_DATETIME_TM *tm = (SWPA_DATETIME_TM *)wParam;
    if(NULL != tm)
    {
        CSWDateTime dt;
        hr = dt.SetTime(*tm);
        //金星有时区文件，已经是GMT8
        swpa_utils_shell("hwclock -w  --utc", NULL);
    }
    SW_TRACE_DEBUG("CSWProcessXMLCmd::OnSetTime(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
    return hr;
}

HRESULT CSWProcessXMLCmd::OnGetTime(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    HRESULT hr = E_FAIL;

    SWPA_DATETIME_TM* pTime = (SWPA_DATETIME_TM*)lParam;


    if(NULL != pTime)
    {
        SWPA_DATETIME_TM tm;
        CSWDateTime dt;
        dt.GetTime(&tm);

        pTime->year = tm.year;
        pTime->month = tm.month;
        pTime->day = tm.day;
        pTime->hour = tm.hour;
        pTime->min = tm.min;
        pTime->sec = tm.sec;
        pTime->msec = tm.msec;

        hr = S_OK;
    }
    SW_TRACE_DEBUG("CSWProcessXMLCmd::OnGetTime(0x%08x, 0x%08x) return 0x%08x\n", wParam, lParam, hr);
    return hr;
}

HRESULT CSWProcessXMLCmd::OnHDDStatus(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    HRESULT hr = E_FAIL;
    CHAR *strStatus = (CHAR*)lParam;
    if (NULL == strStatus)
    {
        hr = E_INVALIDARG;
        SW_TRACE_DEBUG("CSWProcessXMLCmd::OnHDDStatus lParam is null.\n");
        return hr;
    }
    swpa_strcpy(strStatus, "未找到SD卡.");

    LONGLONG llSize = -1;
    for(int i = 0; i < 10;i++)
    {
        CHAR szDev[256] = {0};
        swpa_snprintf(szDev, sizeof(szDev)-1, "/dev/mmcblk%d",i);
        if (SWPAR_OK == swpa_hdd_get_totalsize(szDev, &llSize))
        {
            swpa_sprintf(strStatus, "总大小:%lldMB = %lldGB.", llSize>>20, llSize>>30);
            break;
        }
    }

    return S_OK;
}

HRESULT CSWProcessXMLCmd::OnGetDevType(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    CHAR *strName = (CHAR*)lParam;
    if(NULL != strName)
    {
        if (IsVenus())
        {
            swpa_strcpy(strName, "VENUS");
            INT nFPGAMode = 0;
            OnGetFPGAMode(NULL,(LPARAM)&nFPGAMode,NULL,NULL,NULL);
            char szVersion[16] = {0};
            if(TYPE_FAKEIMG != nFPGAMode && S_OK == GetVersion("camera_fpga",szVersion))
            {
                if(NULL != swpa_strstr(szVersion,"VB"))
                    swpa_strcpy(strName, "PCC200");
                else if(NULL != swpa_strstr(szVersion,"VC"))
                    swpa_strcpy(strName, "PCC600");
                else if(NULL != swpa_strstr(szVersion,"VD"))
                    swpa_strcpy(strName, "PCC200B");
            }
        }
        else if (IsJupiter())
        {
            swpa_strcpy(strName, "SDC200");
        }
        else if(IsSaturn())
        {
            swpa_strcpy(strName, "PCC200C");
        }
        else
        {
            //for others
            swpa_strcpy(strName, "UNKNOW");
        }
    }

    return hr;
}

HRESULT CSWProcessXMLCmd::OnSetDCAperture(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT Value = (INT)wParam;
    return ipnc_cient_control(0, IPC_CONTROL_CMD_SET_DCIRIS_AUTO_STATE +100,(void*)&Value , sizeof(INT), 0);
}

HRESULT CSWProcessXMLCmd::OnGetDCAperture(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    return ipnc_cient_control(0, IPC_CONTROL_CMD_GET_DCIRIS_AUTO_STATE +100,(void*)lParam, sizeof(INT), 0);
}

HRESULT CSWProcessXMLCmd::OnZoomDCIRIS(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    //检查AGC状态
    INT fEnable = 0;
    ipnc_cient_control(0, IPC_CONTROL_CMD_GET_DCIRIS_AUTO_STATE +100,(void*)&fEnable, sizeof(INT), 0);

    //DC光圈只能在AGC=FALSE时可以手动控制
    if(0 == fEnable)
    {
        INT Value = 1;//任意值
        return ipnc_cient_control(0, IPC_CONTROL_CMD_ZOOM_DCIRIS+100, (void *)&Value, sizeof(INT), 0);
    }

    return E_FAIL;
}

HRESULT CSWProcessXMLCmd::OnShrinkDCIRIS(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    //检查AGC状态
    INT fEnable = 0;
    ipnc_cient_control(0, IPC_CONTROL_CMD_GET_DCIRIS_AUTO_STATE +100,(void*)&fEnable, sizeof(INT), 0);
    //DC光圈只能在AGC=FALSE时可以手动控制
    if(0 == fEnable)
    {
        INT Value = 1;//任意值
        return ipnc_cient_control(0, IPC_CONTROL_CMD_SHRINK_DCIRIS+100, (void *)&Value, sizeof(INT), 0);
    }

    return E_FAIL;
}

HRESULT CSWProcessXMLCmd::OnSetF1IO(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    //polarity output_type
    INT* pParam = (INT*)wParam;
    INT dwPolarity = pParam[0];
    INT dwOutputType = pParam[1];
    tFPGA_IO_ARG tIOArg;
    swpa_memset(&tIOArg,0,sizeof(tIOArg));
    tIOArg.type = E_TRIGGEROUT;
    tIOArg.polarity = dwPolarity;
    tIOArg.output_type = dwOutputType;

    return ipnc_cient_control(0, IPC_CONTROL_CMD_SET_IO_ARG +100,(void*)&tIOArg , sizeof(tIOArg), 0);
}

HRESULT CSWProcessXMLCmd::OnGetF1IO(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    INT* pdwParam = (INT*)lParam;

    tFPGA_IO_ARG tIOArg;
    tIOArg.type = E_TRIGGEROUT;

    HRESULT hr = ipnc_cient_control(0, IPC_CONTROL_CMD_GET_IO_ARG +100,(void*)&tIOArg , sizeof(tIOArg), 0);
    if(S_OK == hr)
    {
        pdwParam[0] = tIOArg.polarity;
        pdwParam[1] = tIOArg.output_type;
    }
    return hr;
}

HRESULT CSWProcessXMLCmd::OnSetALMIO(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    //polarity output_type
    INT *pParam = (INT *)wParam;
    INT dwPolarity = pParam[0];
    INT dwOutputType = pParam[1];
    tFPGA_IO_ARG tIOArg;
    swpa_memset(&tIOArg,0,sizeof(tIOArg));
    tIOArg.type = E_ALARM_OUT;
    tIOArg.polarity = dwPolarity;
    tIOArg.output_type = dwOutputType;

    return ipnc_cient_control(0, IPC_CONTROL_CMD_SET_IO_ARG +100,(void*)&tIOArg , sizeof(tIOArg), 0);
}

HRESULT CSWProcessXMLCmd::OnGetALMIO(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    INT* pdwParam = (INT*)lParam;

    tFPGA_IO_ARG tIOArg;
    tIOArg.type = E_ALARM_OUT;

    HRESULT hr = ipnc_cient_control(0, IPC_CONTROL_CMD_GET_IO_ARG +100,(void*)&tIOArg , sizeof(tIOArg), 0);
    if(S_OK == hr)
    {
        pdwParam[0] = tIOArg.polarity;
        pdwParam[1] = tIOArg.output_type;
    }
    return hr;
}

HRESULT CSWProcessXMLCmd::OnSetEXPIO(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    //polarity output_type
    INT *pParam = (INT *)wParam;
    INT dwPolarity = pParam[0];
    INT dwOutputType = pParam[1];
    tFPGA_IO_ARG tIOArg;
    swpa_memset(&tIOArg,0,sizeof(tIOArg));
    tIOArg.type = E_NORMAL_LED;
    tIOArg.polarity = dwPolarity;
    tIOArg.output_type = dwOutputType;
    tIOArg.pulse_width = 0;

    return ipnc_cient_control(0, IPC_CONTROL_CMD_SET_IO_ARG +100,(void*)&tIOArg , sizeof(tIOArg), 0);
}

HRESULT CSWProcessXMLCmd::OnGetEXPIO(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    INT* pdwParam = (INT*)lParam;

    tFPGA_IO_ARG tIOArg;
    tIOArg.type = E_NORMAL_LED;

    HRESULT hr = ipnc_cient_control(0, IPC_CONTROL_CMD_GET_IO_ARG +100,(void*)&tIOArg , sizeof(tIOArg), 0);
    if(S_OK == hr)
    {
        pdwParam[0] = tIOArg.polarity;
        pdwParam[1] = tIOArg.output_type;
    }
    return hr;
}

HRESULT CSWProcessXMLCmd::OnSetTGIO(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT dwEgdeType = (INT)wParam;
    return ipnc_cient_control(0, IPC_CONTROL_CMD_SET_EDGE_MOD +100,(void*)&dwEgdeType , sizeof(INT), 0);
}

HRESULT CSWProcessXMLCmd::OnGetTGIO(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    INT* pdwParam = (INT*)lParam;
    INT dwEgdeType = 0;

    if(S_OK != ipnc_cient_control(0, IPC_CONTROL_CMD_GET_EDGE_MOD +100,(void*)&dwEgdeType , sizeof(INT), 0))
    {
        return E_FAIL;
    }

    pdwParam[0] = dwEgdeType;

    return S_OK;
}

HRESULT CSWProcessXMLCmd::OnSetCtrlCpl(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT Value = (INT)wParam;
    INT nFilterStatus = (2 == Value) ? 1 : 0;
    return ipnc_cient_control(0, IPC_CONTROL_CMD_SET_FILTER_STATE+100, (void*)&nFilterStatus , sizeof(INT), 0);
}

HRESULT CSWProcessXMLCmd::OnGetCoilTiggerCount(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    if(IsVenus())
    {
        //假图模式计数=0
        INT iFPGAMode = TYPE_FAKEIMG;
        if( S_OK == OnGetFPGAMode(NULL,(LPARAM)&iFPGAMode,NULL,NULL,NULL) && TYPE_FAKEIMG == iFPGAMode)
        {
            INT* iCount = (INT*)lParam;
            *iCount = 0;
            return S_OK;
        }

        return ipnc_cient_control(0,IPC_CONTROL_CMD_GET_CAPTURE_COUNT,(void*)lParam,sizeof(INT),0);
    }

    return E_FAIL;
}

HRESULT CSWProcessXMLCmd::OnResetCoilTiggerCount(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if(IsVenus())
    {
        INT iCount = 0;
        return ipnc_cient_control(0,IPC_CONTROL_CMD_SET_CAPTURE_COUNT,(void*)&iCount,sizeof(INT),0);
    }

    return E_FAIL;
}

HRESULT CSWProcessXMLCmd::OnSetComTestEnable(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT iComId = (INT)wParam;
    if (iComId < 0 || iComId > 1)
    {
        SW_TRACE_NORMAL("Err: OnStartCOMTest COM ID %d err\n",iComId);
        return E_INVALIDARG;
    }

    if (NULL != m_pComTestDevic)	//只能开始一次
    {
        SW_TRACE_NORMAL("Err: Must stop last COM test server first\n");
        return E_FAIL;
    }

    DEVICEPARAM tParam;
    tParam.iBaudrate = 9600;
    tParam.iCommType = 1;		//485 ，金星只有485串口，水星既有485又有232

    m_pComTestDevic = new CSWCOMTestDevice(&tParam);
    if (NULL == m_pComTestDevic)
    {
        SW_TRACE_NORMAL("Create new Com test device failed!\n");
        return E_FAIL;
    }

    if (FAILED(m_pComTestDevic->Initialize(NULL,NULL,iComId)))
    {
        SW_TRACE_NORMAL("init Com test device <id:%d> failed!\n",iComId);
        SAFE_RELEASE(m_pComTestDevic);
        return E_FAIL;
    }
    else
    {
        m_pComTestDevic->Start();
    }

    SW_TRACE_NORMAL("<OnStartCOMTest> Start test COM %d server!\n",iComId);

    return S_OK;
}

HRESULT CSWProcessXMLCmd::OnSetComTestdisable(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT iComId = (INT)wParam;
    if (iComId < 0 || iComId > 1)
    {
        SW_TRACE_NORMAL("Err: OnStopCOMTest COM ID %d err\n",iComId);
        return E_INVALIDARG;
    }

    if (m_pComTestDevic)
    {
        m_pComTestDevic->Stop();
        m_pComTestDevic->Close();
        SAFE_RELEASE(m_pComTestDevic);
    }

    SW_TRACE_NORMAL("<OnStopCOMTest> Stop test COM %d server!\n",iComId);

    return S_OK;
}

HRESULT CSWProcessXMLCmd::OnSetAGCEnable(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT Value = (INT)wParam;
    if(IsJupiter())
    {
        AE_MODE mode = 1 == Value? AE_FULL_AUTO : AE_MANUAL;
        return swpa_camera_basicparam_set_AE(mode);
    }
    else
    {
        //同步设置光圈
        ipnc_cient_control(0, IPC_CONTROL_CMD_SET_DCIRIS_AUTO_STATE +100,(void*)&Value , sizeof(INT), 0);

        return ipnc_cient_control(0, IPC_CONTROL_CMD_SET_AE_STATE + 100 ,(void*)&Value , sizeof(INT), 0);
    }
}

HRESULT CSWProcessXMLCmd::OnGetAGCEnable(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    if(IsJupiter())
    {
        AE_MODE eAEMode = AE_FULL_AUTO;
        if (SWPAR_OK != swpa_camera_basicparam_get_AE(&eAEMode))
        {
            SW_TRACE_DEBUG("Err: failed to get AE mode!\n");
            return E_FAIL;
        }
        INT* iValue = (INT*)lParam;

        *iValue = AE_FULL_AUTO == eAEMode? 1:0;
        return S_OK;
    }
    else
    {
        return ipnc_cient_control(0, IPC_CONTROL_CMD_GET_AE_STATE + 100,(void*)lParam , sizeof(BOOL), 0);
    }
}

HRESULT CSWProcessXMLCmd::OnSetGain(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT iValue = (INT)wParam;
    if(IsJupiter())
    {
        if (iValue < 0 || iValue > 0xf)
        {
            SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
            return E_INVALIDARG;
        }

        if (SWPAR_OK != swpa_camera_basicparam_set_gain(iValue))
        {
            SW_TRACE_DEBUG("Err: failed to set gain to %d!\n", iValue);
            return E_FAIL;
        }

        return S_OK;
    }
    else
    {
        return ipnc_cient_control(0, IPC_CONTROL_CMD_SET_GAIN +100,(void*)&iValue , sizeof(INT), 0);
    }
}

HRESULT CSWProcessXMLCmd::OnGetGain(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    if(IsJupiter())
    {
        INT* piValue = (INT*)lParam;
        if (SWPAR_OK != swpa_camera_basicparam_get_gain(piValue))
        {
            SW_TRACE_DEBUG("Err: failed to get shutter!\n");
            return E_FAIL;
        }

        return S_OK;
    }
    else
    {
        return ipnc_cient_control(0, IPC_CONTROL_CMD_GET_GAIN +100,(void*)lParam , sizeof(INT), 0);
    }
}

HRESULT CSWProcessXMLCmd::OnSetShutter(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT iValue = (INT)wParam;
    if(IsJupiter())
    {
        if (iValue < 0 || iValue > 0x15)
        {
            SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
            return E_INVALIDARG;
        }

        if (SWPAR_OK != swpa_camera_basicparam_set_shutter(iValue))
        {
            SW_TRACE_DEBUG("Err: failed to set shutter to %d!\n", iValue);
            return E_FAIL;
        }

        return S_OK;
    }
    else
    {
        return ipnc_cient_control(0, IPC_CONTROL_CMD_SET_EXP +100,(void*)&iValue , sizeof(INT), 0);
    }
}

HRESULT CSWProcessXMLCmd::OnGetShutter(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    if(IsJupiter())
    {
        INT* piValue = (INT*)lParam;
        if (SWPAR_OK != swpa_camera_basicparam_get_shutter(piValue))
        {
            SW_TRACE_DEBUG("Err: failed to get shutter!\n");
            return E_FAIL;
        }

        return S_OK;
    }
    else
    {
        return ipnc_cient_control(0, IPC_CONTROL_CMD_GET_EXP + 100,(void*)lParam , sizeof(INT), 0);
    }
}

HRESULT CSWProcessXMLCmd::OnSetAWBEnable(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT iValue = (INT)wParam;
    if(IsJupiter())
    {
        if (iValue < 0 || iValue > 1)
        {
            SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
            return E_INVALIDARG;
        }
        AWB_MODE mode = 1 == iValue? AWB_AUTO : AWB_MANUAL;

        if (SWPAR_OK != swpa_camera_basicparam_set_AWB(mode))
        {
            SW_TRACE_DEBUG("Err: failed to set awb mode to %d!\n", iValue);
            return E_FAIL;
        }

        return S_OK;
    }
    else
    {
        return ipnc_cient_control(0, IPC_CONTROL_CMD_SET_AWB_STATE +100,(void*)&iValue , sizeof(INT), 0);
    }
}

HRESULT CSWProcessXMLCmd::OnGetAWBEnable(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    if(IsJupiter())
    {
        INT* piValue = (INT*)lParam;
        AWB_MODE mode = AWB_MANUAL;
        if (SWPAR_OK != swpa_camera_basicparam_get_AWB(&mode))
        {
            SW_TRACE_DEBUG("Err: failed to get awb mode!\n");
            return E_FAIL;
        }

        *piValue = AWB_MANUAL == mode? 0 : 1;
        return S_OK;
    }
    else
    {
        return ipnc_cient_control(0, IPC_CONTROL_CMD_GET_AWB_STATE +100,(void*)lParam , sizeof(INT), 0);
    }
}

HRESULT CSWProcessXMLCmd::OnSetRGBGain(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT *tmp = (INT *)wParam;
    if(tmp == NULL || tmp+1 == NULL || tmp+2== NULL)
    {
        SW_TRACE_DEBUG("%s %d Value error \n",__FUNCTION__,__LINE__);
        return E_FAIL;
    }

    if(IsJupiter())
    {
        INT iGainR = tmp[0];
        INT iGainG = tmp[1];
        INT iGainB = tmp[2];

        if (SWPAR_OK != swpa_camera_basicparam_set_rgb_gain(iGainR, iGainG, iGainB))
        {
            SW_TRACE_DEBUG("Err: failed to set rgb to %d,%d,%d!\n",
                           iGainR, iGainG, iGainB);
            return E_FAIL;
        }

        return S_OK;
    }
    else
    {
        DWORD Value;
        Value = tmp[0]  | tmp[1] << 8 | tmp[2] << 16;
        return ipnc_cient_control(0, IPC_CONTROL_CMD_SET_RGB +100,(void*)&Value , sizeof(DWORD), 0);
    }
}

HRESULT CSWProcessXMLCmd::OnGetRGBGain(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    INT *temp = (INT *)lParam;
    if(temp == NULL || temp+1 == NULL || temp +2 == NULL) return E_FAIL;

    if(IsJupiter())
    {
        if (SWPAR_OK != swpa_camera_basicparam_get_rgb_gain(&temp[0], &temp[1], &temp[2]))
        {
            SW_TRACE_DEBUG("Err: failed to get rgb!\n");
            return E_FAIL;
        }

        return S_OK;
    }
    else
    {
        int iValue;
        HRESULT hr = ipnc_cient_control(0, IPC_CONTROL_CMD_GET_RGB +100,(void*)&iValue , sizeof(INT), 0);
        if(S_OK == hr)
        {
            temp[0] = (iValue >> 0 ) & 0xff;
            temp[1] = (iValue >> 8 ) & 0xff;
            temp[2] = (iValue >> 16) & 0xff;
        }
        return hr;
    }
}

HRESULT CSWProcessXMLCmd::OnAutoTestCamera(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    //获取AGC、AWB状态
    INT fAGCEnable = 0;
    INT fAWBEnable = 0;
    INT fAGCGain =0;
    INT fShutter =0;
    INT rgiValue[3] = {0};

    if(S_OK != OnGetAGCEnable(NULL,(LPARAM)&fAGCEnable,NULL,NULL,NULL))
    {
        SW_TRACE_DEBUG("[OnAutoTestCamera] OnGetAGCEnable Fail.\n");
    }

    if(S_OK != OnGetAWBEnable(NULL, (LPARAM)&fAWBEnable,NULL,NULL,NULL))
    {
        SW_TRACE_DEBUG("[OnAutoTestCamera] OnGetAWBEnable Fail.\n");
    }

    //获取增益
    if( S_OK != OnGetGain(NULL,(LPARAM)&fAGCGain,NULL,NULL,NULL))
    {
        SW_TRACE_DEBUG("[OnAutoTestCamera] OnGetGain Fail.\n");
    }

    //获取快门
    if(S_OK != OnGetShutter(NULL,(LPARAM)&fShutter,NULL,NULL,NULL))
    {
        SW_TRACE_DEBUG("[OnAutoTestCamera] OnGetShutter Fail.\n");
    }

    //获取R增益,G增益,B增益
    if(S_OK != OnGetRGBGain(NULL,(LPARAM)rgiValue,NULL,NULL,NULL))
    {
        SW_TRACE_DEBUG("[OnAutoTestCamera] OnGetRGBGain Fail.\n");
    }

    INT fEnable = 0;
    if(S_OK !=OnSetAGCEnable( fEnable , 0 ,NULL,NULL,NULL))
    {
        SW_TRACE_DEBUG("[OnAutoTestCamera] OnSetAGCEnable 0 Fail.\n");
    }
    if(S_OK != OnSetAWBEnable( fEnable , 0 ,NULL,NULL,NULL))
    {
        SW_TRACE_DEBUG("[OnAutoTestCamera] OnSetAWBEnable 0 Fail.\n");
    }

    //调整快门最小到最大，步长：1/10 *（最大快门-最小快门），耗时：2秒
    if(IsJupiter())
    {
        for (int i = 0x15 ; i >= 0x01; i--)
        {
            OnSetShutter( i , 0,NULL,NULL,NULL);
            swpa_thread_sleep_ms(20);
        }
        OnSetShutter( 0x3 , 0,NULL,NULL,NULL);
    }
    else
    {
        for (int i = 1 ; i <= 50; i++)
        {
            OnSetShutter( 30000 *i/50 , 0,NULL,NULL,NULL);
            swpa_thread_sleep_ms(20);
        }
        OnSetShutter( 3000 , 0,NULL,NULL,NULL);
    }

    //调整增益最小到最大；步长：1/10 *（最大增益-最小增益），耗时：2秒
    if(IsJupiter())
    {
        for (int i = 1 ; i <= 0xf; i++)
        {
            OnSetGain( i , 0,NULL,NULL,NULL);
            swpa_thread_sleep_ms(20);
        }
        OnSetGain( 0x3 , 0,NULL,NULL,NULL);
    }
    else
    {
        for (int i = 1 ; i <= 50; i++)
        {
            OnSetGain( 420 *i/50 , 0,NULL,NULL,NULL);
            swpa_thread_sleep_ms(20);
        }
        OnSetGain( 60 , 0,NULL,NULL,NULL);
    }

    //调整R最小到最大; 步长：1/10 *（最大R值-最小R值），耗时：2秒
    INT nValue[3];
    swpa_memset(&nValue,0,sizeof(nValue));
    for (int i = 1 ; i <= 50; i++)
    {
        nValue[0] = 255 *i/50;
        OnSetRGBGain((WPARAM)nValue, 0,NULL,NULL,NULL);
        swpa_thread_sleep_ms(20);
    }
/*
    swpa_memset(&nValue,0,sizeof(nValue));
    //调整G最小到最大; 步长：1/10 *（最大G值-最小G值），耗时：2秒
    for (int i = 1 ; i <= 50; i++)
    {
        nValue[1] = 255 *i/50;
        OnSetRGBGain((WPARAM)nValue , 0,NULL,NULL,NULL);
        swpa_thread_sleep_ms(20);
    }
*/
    swpa_memset(&nValue,0,sizeof(nValue));
    //调整B最小到最大; 步长：1/10 *（最大B值-最小B值），耗时：2秒
    for (int i = 1 ; i <= 50; i++)
    {
        nValue[2] = 255 *i/50;
        OnSetRGBGain((WPARAM)nValue , 0,NULL,NULL,NULL);
        swpa_thread_sleep_ms(20);
    }

    //恢复增益
    if(S_OK != OnSetGain((WPARAM)fAGCGain,NULL,NULL,NULL,NULL))
    {
        SW_TRACE_DEBUG("[OnAutoTestCamera] OnSetGain restore Fail.\n");
    }

    //恢复快门
    if(S_OK != OnSetShutter((WPARAM)fShutter,NULL,NULL,NULL,NULL))
    {
        SW_TRACE_DEBUG("[OnAutoTestCamera] OnSetShutter restore Fail.\n");
    }

    //恢复 R增益,G增益,B增益
    if(S_OK != OnSetRGBGain((WPARAM)rgiValue,NULL,NULL,NULL,NULL))
    {
        SW_TRACE_DEBUG("[OnAutoTestCamera] OnSetRGBGain restore Fail.\n");
    }

    //恢复AGC\AWB状态
    if(S_OK != OnSetAGCEnable(fAGCEnable , 0,NULL,NULL,NULL))
    {
        SW_TRACE_DEBUG("[OnAutoTestCamera] OnSetAGCEnable Fail.\n");
    }

    if(S_OK != OnSetAWBEnable(fAWBEnable , 0,NULL,NULL,NULL))
    {
        SW_TRACE_DEBUG("[OnAutoTestCamera] OnSetAWBEnable Fail.\n");
    }

    return S_OK;
}

HRESULT CSWProcessXMLCmd::OnSwitchFPGA(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    //load the corresponding fpga file according to the existence of flag file
    INT iDir = (INT)wParam;

    char* sz185FlagFile = "/mnt/nand/realimagefpga185";
    if (swpa_utils_file_exist(sz185FlagFile) && TYPE_185 != iDir)
    {
        swpa_utils_file_delete(sz185FlagFile);
    }

    char* sz178FlagFile = "/mnt/nand/realimagefpga178";
    if (swpa_utils_file_exist(sz178FlagFile) && TYPE_178 != iDir)
    {
        swpa_utils_file_delete(sz178FlagFile);
    }

    char* sz249FlagFile = "/mnt/nand/realimagefpga249";
    if (swpa_utils_file_exist(sz249FlagFile) && TYPE_249 != iDir)
    {
        swpa_utils_file_delete(sz249FlagFile);
    }

    if (TYPE_FAKEIMG == iDir)
    {
        SW_TRACE_DEBUG("Info: switching fpga to fake-image mode.\n");
        BOOL fError = FALSE;
        if (swpa_utils_file_exist(sz185FlagFile)
                && SWPAR_OK != swpa_utils_file_delete(sz185FlagFile))
        {
            fError = TRUE;
        }
        if (swpa_utils_file_exist(sz178FlagFile)
                && SWPAR_OK != swpa_utils_file_delete(sz178FlagFile))
        {
            fError = TRUE;
        }
        if (swpa_utils_file_exist(sz249FlagFile)
                && SWPAR_OK != swpa_utils_file_delete(sz249FlagFile))
        {
            fError = TRUE;
        }

        if(TRUE == fError)
        {
            SW_TRACE_NORMAL("Err: failed to set fpga to fake-image mode.\n");
            return E_FAIL;
        }
    }
    else if(TYPE_185 == iDir)
    {
        SW_TRACE_DEBUG("Info: switching fpga to 185 real-image mode.\n");
        if(!swpa_utils_file_exist(sz185FlagFile))
        {
            SW_TRACE_DEBUG("Info:Create 185 real-image mode flag file.\n");
            if(SWPAR_OK != swpa_utils_file_create(sz185FlagFile))
            {
                SW_TRACE_NORMAL("Err: failed to set 185 fpga mode.\n");
                return E_FAIL;
            }
        }
        else
        {
            SW_TRACE_DEBUG("Info:Exist 185 real-image mode flag file.\n");
        }
    }
    else if(TYPE_178 == iDir)
    {
        SW_TRACE_DEBUG("Info: switching fpga to 178 real-image mode.\n");
        if(!swpa_utils_file_exist(sz178FlagFile))
        {
            SW_TRACE_DEBUG("Info:Create 178 real-image mode flag file.\n");
            if(SWPAR_OK != swpa_utils_file_create(sz178FlagFile))
            {
                SW_TRACE_NORMAL("Err: failed to set 178 fpga mode.\n");
                return E_FAIL;
            }
        }
        else
        {
            SW_TRACE_DEBUG("Info:Exist 178 real-image mode flag file.\n");
        }
    }
    else if(TYPE_249 == iDir)
    {
        SW_TRACE_DEBUG("Info: switching fpga to 249 real-image mode.\n");
        if(!swpa_utils_file_exist(sz249FlagFile))
        {
            SW_TRACE_DEBUG("Info:Create 249 real-image mode flag file.\n");
            if(SWPAR_OK != swpa_utils_file_create(sz249FlagFile))
            {
                SW_TRACE_NORMAL("Err: failed to set 249 fpga mode.\n");
                return E_FAIL;
            }
        }
        else
        {
            SW_TRACE_DEBUG("Info:Exist 249 real-image mode flag file.\n");
        }
    }
    else
    {
        SW_TRACE_NORMAL("Err: Unknow fpga mode:%d.\n",iDir);
        return E_FAIL;
    }

    sync();
    *iExitCode = 2;
    *fGoingToExit = TRUE;
    *fSwitchingSystem = FALSE;

    return S_OK; //reboot
}

HRESULT CSWProcessXMLCmd::OnGetFPGAMode(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    INT* pMode = (INT*)lParam;

    *pMode = TYPE_FAKEIMG;

    char* sz185FlagFile = "/mnt/nand/realimagefpga185";
    char* sz178FlagFile = "/mnt/nand/realimagefpga178";
    char* sz249FlagFile = "/mnt/nand/realimagefpga249";

    if (swpa_utils_file_exist(sz185FlagFile))
    {
        *pMode = TYPE_185;
    }
    else if (swpa_utils_file_exist(sz178FlagFile))
    {
        *pMode = TYPE_178;
    }
    else if (swpa_utils_file_exist(sz249FlagFile))
    {
        *pMode = TYPE_249;
    }

    return S_OK;
}

HRESULT CSWProcessXMLCmd::OnSetAF(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT iValue = (INT)wParam;
    if(IsJupiter())
    {
        if (iValue < 0 || iValue > 1)
        {
            SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
            return E_INVALIDARG;
        }
        AF_MODE eAFMode = 1 == iValue ? AF_AUTO : AF_MANUAL;
        if (SWPAR_OK != swpa_camera_basicparam_set_AF(eAFMode))
        {
            SW_TRACE_DEBUG("Err: failed to set AF to %d!\n", iValue);
            return E_FAIL;
        }

        return S_OK;
    }

    return E_FAIL;
}

HRESULT CSWProcessXMLCmd::OnGetAF(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    if(IsJupiter())
    {
        INT* piValue = (INT*)lParam;
        AF_MODE eAFMode;
        if (SWPAR_OK != swpa_camera_basicparam_get_AF(&eAFMode))
        {
            SW_TRACE_DEBUG("Err: failed to get AF!\n");
            return E_FAIL;
        }

        *piValue = AF_AUTO == eAFMode ? 1 : 0;

        return S_OK;
    }

    return E_FAIL;
}

HRESULT CSWProcessXMLCmd::OnTuneFocus(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT iTune = (INT)wParam;

    if(IsJupiter())
    {
        FOCUS_MODE iMode = (0 == iTune) ? FOCUS_NEAR : FOCUS_FAR;
        if (SWPAR_OK != swpa_camera_basicparam_set_focus_speed(iMode, 5))
        {
            SW_TRACE_DEBUG("Err: failed to set focus speed\n");
            return E_FAIL;
        }

        CSWApplication::Sleep(30);

        if (SWPAR_OK != swpa_camera_basicparam_set_focus(FOCUS_STOP))
        {
            SW_TRACE_DEBUG("Err: failed to stop focus\n");
            return E_FAIL;
        }

        return S_OK;
    }

    return E_FAIL;
}

HRESULT CSWProcessXMLCmd::OnGetFocus(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    if(IsJupiter())
    {
        INT * piValue = (INT*)lParam;

        if (SWPAR_OK != swpa_camera_basicparam_get_focus_value(piValue))
        {
            SW_TRACE_DEBUG("Err: failed to get focus!\n");
            return E_FAIL;
        }

        return S_OK;
    }

    return E_FAIL;
}

HRESULT CSWProcessXMLCmd::OnTuneZoom(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT iValue = (INT)wParam;

    if(IsJupiter())
    {
        ZOOM_MODE eMode = (0 == iValue)? ZOOM_TELE : ZOOM_WIDE;
        INT iZoomSpeed = 0x5;//default speed, equals to the value in domerocker

        if (SWPAR_OK != swpa_camera_basicparam_set_zoom_speed(eMode, iZoomSpeed))
        {
            SW_TRACE_DEBUG("Err: failed to start zoom %d!\n", iValue);
            return E_FAIL;
        }

        CSWApplication::Sleep(30);

        if (SWPAR_OK != swpa_camera_basicparam_set_zoom_mode(ZOOM_STOP))
        {
            SW_TRACE_DEBUG("Err: failed to stop zoom!\n");
            return E_FAIL;
        }

        for (INT iCount=0; iCount<3; iCount++)
        {
            swpa_thread_sleep_ms(200);
            swpa_camera_ball_sync_zoom();
        }

        return S_OK;
    }

    return E_FAIL;
}

HRESULT CSWProcessXMLCmd::OnGetZoom(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (NULL == lParam)
    {
        SW_TRACE_DEBUG("Err: invalid arg lParam!\n");
        return E_INVALIDARG;
    }

    if(IsJupiter())
    {
        INT * piValue = (INT*)lParam;
        INT iZoom = 0;

        if (SWPAR_OK != swpa_camera_basicparam_get_zoom_value(&iZoom))
        {
            SW_TRACE_DEBUG("Err: failed to get zoom!\n");
            return E_FAIL;
        }

        return S_OK;
    }

    return E_FAIL;
}

HRESULT CSWProcessXMLCmd::OnStartPan(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT iValue = (INT)wParam;

    if (iValue < 0 || iValue > 1)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    PAN_TYPE eType = (PAN_TYPE)iValue;
    if (SWPAR_OK != swpa_camera_ball_pan(eType,30))
    {
        SW_TRACE_DEBUG("Err: failed to start pan %d!\n", eType);
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSWProcessXMLCmd::OnStopPan(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (SWPAR_OK != swpa_camera_ball_stop())
    {
        SW_TRACE_DEBUG("Err: failed to stop pan!\n");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSWProcessXMLCmd::OnStartTilt(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT iValue = (INT)wParam;

    if (iValue < 0 || iValue > 1)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", iValue);
        return E_INVALIDARG;
    }

    TILT_TYPE eType = (TILT_TYPE)iValue;
    if (SWPAR_OK != swpa_camera_ball_tilt(eType, 30))
    {
        SW_TRACE_DEBUG("Err: failed to start tilt %d!\n", eType);
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSWProcessXMLCmd::OnStopTilt(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (SWPAR_OK != swpa_camera_ball_stop())
    {
        SW_TRACE_DEBUG("Err: failed to stop tilt!\n");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSWProcessXMLCmd::OnStartWiper(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    if (SWPAR_OK != swpa_camera_ball_set_wiper())
    {
        SW_TRACE_DEBUG("Err: failed to start wiper!\n");
        return E_FAIL;
    }


    return S_OK;
}

HRESULT CSWProcessXMLCmd::OnSetLEDModeEx(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT iMode = (INT)wParam;
    if (iMode < 0 || iMode > 4)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam = %d!\n", iMode);
        return E_INVALIDARG;
    }

    INT iLEDMode = 0;
    INT iNearPower = 0;
    INT iMediumPower = 0;
    INT iFarPower = 0;
    switch (iMode)
    {
        case 0:   //全灭
            iLEDMode = 3; /*off*/
            break;
        case 1:  //近光灯
            iLEDMode = 2; /*manual*/
            iNearPower = 10;
            break;
        case 2: //中等亮度
            iLEDMode = 2; /*manual*/
            iMediumPower = 10;
            break;
        case 3: //远光灯
            iLEDMode = 2; /*manual*/
            iFarPower = 10;
            break;
        case 4: //全亮
            iLEDMode = 2; /*manual*/
            iNearPower = 10;
            iMediumPower = 10;
            iFarPower = 10;
            break;
        default:
            break;
    }

    if (SWPAR_OK != swpa_camera_ball_set_led_mode(iLEDMode, 0x2))
    {
        SW_TRACE_DEBUG("Err: failed to set LED Mode:%d!\n", iMode);
        return E_FAIL;
    }

    if (SWPAR_OK != swpa_camera_ball_set_led_power(iNearPower, iMediumPower, iFarPower))
    {
        SW_TRACE_DEBUG("Err: failed to set LED power to NearPower:%d MediumPower:%d FarPower:%d!\n", iNearPower,iMediumPower,iFarPower);
        return E_FAIL;
    }


    return S_OK;
}

HRESULT CSWProcessXMLCmd::OnSetDefog(WPARAM wParam, LPARAM lParam, INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem)
{
    INT iValue = (INT)wParam;

    if (iValue != 0 && iValue != 1)
    {
        SW_TRACE_DEBUG("Err: invalid arg wParam(=%d)!\n", (INT)wParam);
        return E_INVALIDARG;
    }

    if (SWPAR_OK != swpa_camera_ball_set_defog_fan(iValue))
    {
        SW_TRACE_DEBUG("Err: failed to set defog to %d!\n", iValue);
        return E_FAIL;
    }

    return S_OK;
}

