#include "HvParamIO.h"
#include "memstorage.h"
#include "hvfilestorage.h"
#include "HvParamConvert.h"
#include "tinyxml.h"
#include "swdrv.h"
#include "hvsocket.h"
#include "TcpipCfg.h"

//写入int型键值
HRESULT HvParamWriteInt(
    CParamStore* pParam,
    LPCSTR szSection,
    LPCSTR szKey,
    INT iIntVar,
    INT iDefault,
    INT iMin,
    INT iMax,
    LPCSTR szChName,
    LPCSTR szComment,
    BYTE8 nRank,
    BOOL fSaveNow/*=FALSE*/
)
{
    if ( NULL == pParam ) return E_POINTER;
    if ( S_OK != pParam->SetInt(szSection, szKey, iIntVar) ) return E_FAIL;
    if ( S_OK != pParam->GetInt(szSection, szKey, &iIntVar, iDefault, iMin, iMax, szChName, szComment, nRank) ) return E_FAIL;
    if ( fSaveNow )
    {
        if ( S_OK != pParam->Save() ) return E_FAIL;
    }
    return S_OK;
}

//写入float型键值
HRESULT HvParamWriteFloat(
    CParamStore* pParam,
    LPCSTR szSection,
    LPCSTR szKey,
    FLOAT iFloatVar,
    FLOAT fltDefault,
    FLOAT fltMin,
    FLOAT fltMax,
    LPCSTR szChName,
    LPCSTR szComment,
    BYTE8 nRank,
    BOOL fSaveNow/*=FALSE*/
)
{
    if ( NULL == pParam ) return E_POINTER;
    if ( S_OK != pParam->SetFloat(szSection, szKey, iFloatVar) ) return E_FAIL;
    if ( S_OK != pParam->GetFloat(szSection, szKey, &iFloatVar, fltDefault, fltMin, fltMax, szChName, szComment, nRank) ) return E_FAIL;
    if ( fSaveNow )
    {
        if ( S_OK != pParam->Save() ) return E_FAIL;
    }
    return S_OK;
}

//写入sz型键值
HRESULT HvParamWriteString(
    CParamStore* pParam,
    LPCSTR szSection,
    LPCSTR szKey,
    LPCSTR szStr,
    INT iLen,
    LPCSTR szChName,
    LPCSTR szComment,
    BYTE8 nRank,
    BOOL fSaveNow/*=FALSE*/
)
{
    if ( NULL == pParam ) return E_POINTER;
    if ( S_OK != pParam->SetString(szSection, szKey, szStr) ) return E_FAIL;

    char szBufTmp[1024];
    if ( S_OK != pParam->GetString(szSection, szKey, szBufTmp, iLen, szChName, szComment, nRank) ) return E_FAIL;
    if ( fSaveNow )
    {
        if ( S_OK != pParam->Save() ) return E_FAIL;
    }
    return S_OK;
}

//写入二进制型键值
HRESULT HvParamWriteBin(
    CParamStore* pParam,
    LPCSTR szSection,
    LPCSTR szKey,
    LPCVOID pBinData,
    INT iLen,
    LPCSTR szChName,
    LPCSTR szComment,
    BYTE8 nRank,
    BOOL fSaveNow/*=FALSE*/
)
{
    if ( NULL == pParam ) return E_POINTER;
    if ( S_OK != pParam->SetBin(szSection, szKey, pBinData, iLen) ) return E_FAIL;

    int iLenTmp=0;
    if ( S_OK != pParam->GetBin(szSection, szKey, NULL, &iLenTmp, szChName, szComment, nRank) ) return E_FAIL;
    if ( fSaveNow )
    {
        if ( S_OK != pParam->Save() ) return E_FAIL;
    }
    return S_OK;
}

//读取int型键值
HRESULT HvParamReadInt(
    CParamStore* pParam,
    LPCSTR szSection,
    LPCSTR szKey,
    INT* piVal,
    INT iDefault
)
{
    if ( NULL == pParam ) return E_POINTER;
    pParam->EnableAutoAdd(FALSE);
    HRESULT hr = pParam->GetInt(szSection, szKey, piVal, iDefault);
    pParam->EnableAutoAdd(TRUE);
    return hr;
}

//读取float型键值
HRESULT HvParamReadFloat(
    CParamStore* pParam,
    LPCSTR szSection,
    LPCSTR szKey,
    FLOAT* pfltVal,
    FLOAT fltDefault
)
{
    if ( NULL == pParam ) return E_POINTER;
    pParam->EnableAutoAdd(FALSE);
    HRESULT hr = pParam->GetFloat(szSection, szKey, pfltVal, fltDefault);
    pParam->EnableAutoAdd(TRUE);
    return hr;
}

//读取sz型键值
HRESULT HvParamReadString(
    CParamStore* pParam,
    LPCSTR szSection,
    LPCSTR szKey,
    LPSTR szString,
    INT iLen
)
{
    if ( NULL == pParam ) return E_POINTER;
    pParam->EnableAutoAdd(FALSE);
    HRESULT hr = pParam->GetString(szSection, szKey, szString, iLen);
    pParam->EnableAutoAdd(TRUE);
    return hr;
}

//读取二进制型键值
HRESULT HvParamReadBin(
    CParamStore* pParam,
    LPCSTR szSection,
    LPCSTR szKey,
    LPVOID pBuf,
    INT* piBufLen
)
{
    if ( NULL == pParam ) return E_POINTER;
    pParam->EnableAutoAdd(FALSE);
    HRESULT hr = pParam->GetBin(szSection, szKey, pBuf, piBufLen);
    pParam->EnableAutoAdd(TRUE);
    return hr;
}

#define PARAM_BUF_LEN PARAM_SIZE /**< 总参数的最大容量（字节） */
extern char* PARAM_FILE_PATH;

HRESULT GetParamXml(
    CParamStore* pParamStore,
    char* szBuf,
    int nBufLen,
    DWORD32 dwFlag /*=0*/
)
{
    if (!szBuf) return E_INVALIDARG;
    szBuf[nBufLen-1] = '\0';

    CFastMemAlloc cStack;
    BYTE8* pBuf = (BYTE8*)cStack.StackAlloc(PARAM_BUF_LEN, FALSE);
    if (pBuf == NULL) return E_OUTOFMEMORY;

    CMemStorage cMemStg;
    cMemStg.Initialize(pBuf, PARAM_BUF_LEN);

    pParamStore->SaveTo(&cMemStg, FALSE);

    int nExportParamRank = 1; //= g_HvDspParam.nExportParamRank;
    bool nExportRankInfo = true; //= g_HvDspParam.nExportRankInfo;

    TiXmlDocument cDoc;
    ConvertParam2Xml(pBuf, PARAM_BUF_LEN, &cDoc, nExportParamRank, TRUE, nExportRankInfo);

    TiXmlPrinter cPrinter;
    cDoc.Accept(&cPrinter);

    strncpy(szBuf, cPrinter.CStr(), nBufLen - 1);

    return S_OK;
}

HRESULT SetParamXml(
    CParamStore* pParamStore,
    char* szBuf,
    DWORD32 dwFlag /*=0*/
)
{
    if (!szBuf) return E_INVALIDARG;

    if (strlen(szBuf) == 0)
    {
        HV_Trace(5, "Clear Param!\n");

        // 清空参数
        pParamStore->Clear();
        pParamStore->Save();

        return S_OK;
    }

    CFastMemAlloc cStack;
    BYTE8* pBuf = (BYTE8*)cStack.StackAlloc(PARAM_BUF_LEN, FALSE);
    if (pBuf == NULL) return E_OUTOFMEMORY;

    //Comment by Shaorg:
    //将将以下这个静态变量变为普通变量。
    //目的是为了修复Bug：“通过HvSetup修改参数，只有第一次修改能生效。”
    //static TiXmlDocument cDoc;
    TiXmlDocument cDoc;
    cDoc.Parse(szBuf);

    DWORD32 dwAppendMode = CParamStore::MODE_REPLACE_KEY;
    TiXmlHandle docHandle(&cDoc);
    TiXmlElement* pAppend = docHandle.FirstChild("HvParamDoc").FirstChild("AppendMode").ToElement();
    if (pAppend)
    {
        LPCSTR szMode = pAppend->GetText();
        if (szMode)
        {
            if (strcmp(szMode,"Replace") == 0)
            {
                dwAppendMode = CParamStore::MODE_REPLACE_KEY;
            }
            else if (strcmp(szMode,"Add") == 0)
            {
                dwAppendMode = CParamStore::MODE_ADD_NEW;
            }
            else if (strcmp(szMode,"OverWrite") == 0)
            {
                dwAppendMode = CParamStore::MODE_REPLACE_SECTION;
            }
        }
    }

    UINT nBufLen = PARAM_BUF_LEN;
    if ( S_OK != ConvertXml2Param(&cDoc, pBuf, nBufLen, TRUE) )
    {
        HV_Trace(1, "ConvertXml2Param() Failed!\n");
        return E_FAIL;
    }

    HRESULT hr = E_FAIL;

    CMemStorage cMemStg;
    cMemStg.Initialize(pBuf, PARAM_BUF_LEN);

    //static CParamStore cNewStore;  //Comment by Shaorg：原来这里为什么要用静态变量呢？
    CParamStore cNewStore;
    if (SUCCEEDED(cNewStore.LoadFrom(&cMemStg)))
    {
        pParamStore->Inject("", "", &cNewStore, dwAppendMode);
        hr = pParamStore->Save();
    }

    if (FAILED(hr))
    {
        HV_Trace(1, "CParamStore::Save() Failed!\n");
    }

    return hr;
}

HRESULT HvParamReadIntWithWrite(
    CParamStore* pParam,
    const char* szSection,
    const char* szKey,
    int* pVal,
    int nDefault,
    int nMin,
    int nMax,
    const char* szChName,
    const char* szComment,
    BYTE8 nRank,
    BOOL fSaveNow/*=FALSE*/
)
{
    pParam->EnableAutoAdd(TRUE);
    HRESULT hr = pParam->GetInt(szSection, szKey, pVal, nDefault, nMin, nMax, szChName, szComment, nRank);
    if ( fSaveNow )
    {
        if ( S_OK != pParam->Save() ) return E_FAIL;
    }
    return hr;
}

HRESULT HvParamReadFloatWithWrite(
    CParamStore* pParam,
    const char* szSection,
    const char* szKey,
    float* pVal,
    float fltDefault,
    float fltMin,
    float fltMax,
    const char* szChName,
    const char* szComment,
    BYTE8 nRank,
    BOOL fSaveNow/*=FALSE*/
)
{
    pParam->EnableAutoAdd(TRUE);
    HRESULT hr = pParam->GetFloat(szSection, szKey, pVal, fltDefault, fltMin, fltMax, szChName, szComment, nRank);
    if ( fSaveNow )
    {
        if ( S_OK != pParam->Save() ) return E_FAIL;
    }
    return hr;
}

HRESULT HvParamReadStringWithWrite(
    CParamStore* pParam,
    const char* szSection,
    const char* szKey,
    char* szRet,
    int nLen,
    const char* szChName,
    const char* szComment,
    BYTE8 nRank,
    BOOL fSaveNow/*=FALSE*/
)
{
    pParam->EnableAutoAdd(TRUE);
    HRESULT hr = pParam->GetString(szSection, szKey, szRet, nLen, szChName, szComment, nRank);
    if ( fSaveNow )
    {
        if ( S_OK != pParam->Save() ) return E_FAIL;
    }
    return hr;
}

HRESULT CombineMasterSlaveXmlParam(
    const char* szXmlParamMaster,
    const char* szXmlParamSlave,
    char* szXmlParamAll,
    DWORD32& dwLen
)
{
    TiXmlElement* pMasterRootElement = NULL;
    TiXmlDocument cXmlDocMaster;
    if ( szXmlParamMaster && cXmlDocMaster.Parse(szXmlParamMaster) )
    {
        pMasterRootElement = cXmlDocMaster.RootElement();
    }

    TiXmlElement* pSlaveRootElement = NULL;
    TiXmlDocument cXmlDocSlave;
    if ( szXmlParamSlave && cXmlDocSlave.Parse(szXmlParamSlave) )
    {
        pSlaveRootElement = cXmlDocSlave.RootElement();
    }

    if ( NULL == pMasterRootElement || NULL == pSlaveRootElement )
    {
        dwLen = 0;
        return E_FAIL;
    }
    else
    {
        // 将主CPU参数内的所有Section结点插入到从CPU参数内的HvParam结点下。
        TiXmlElement* pMasterHvParamElement = NULL;
        TiXmlElement* pSlaveHvParamElement = NULL;
        pMasterHvParamElement = pMasterRootElement->FirstChildElement("HvParam");
        pSlaveHvParamElement = pSlaveRootElement->FirstChildElement("HvParam");
        if ( pMasterHvParamElement && pSlaveHvParamElement)
        {
            TiXmlElement* pMasterSectionElement = NULL;
            pMasterSectionElement = pMasterHvParamElement->FirstChildElement();
            while (pMasterSectionElement)
            {
                pSlaveHvParamElement->LinkEndChild(pMasterSectionElement->Clone());
                pMasterSectionElement = pMasterSectionElement->NextSiblingElement();
            }
        }

        TiXmlPrinter cTxPr;
        cXmlDocSlave.Accept(&cTxPr);
        DWORD32 dwXmlLen = (DWORD32)cTxPr.Size();
        if ( dwLen > dwXmlLen )
        {
            dwLen = dwXmlLen;
            memcpy(szXmlParamAll, cTxPr.CStr(), dwXmlLen);
            return S_OK;
        }
        else
        {
            dwLen = 0;
            return E_FAIL;
        }
    }
}

/* LoadParam.cpp */
extern CParamStore g_cParamStore;

static int SaveTcpipParam(
    const TcpipParam& cTcpipCfgParam1,
    const TcpipParam& cTcpipCfgParam2
)
{
#ifdef SINGLE_BOARD_PLATFORM
    HvParamWriteString(
        &g_cParamStore,
        "\\System\\CamLan", "CommIPAddr",
        cTcpipCfgParam1.szIp, 31,
        "IP地址", "", 5
    );
    HvParamWriteString(
        &g_cParamStore,
        "\\System\\CamLan", "CommMask",
        cTcpipCfgParam1.szNetmask, 31,
        "子网掩码", "", 5
    );
    HvParamWriteString(
        &g_cParamStore,
        "\\System\\CamLan", "CommGateway",
        cTcpipCfgParam1.szGateway, 31,
        "网关", "", 5
    );
    HvParamWriteString(
        &g_cParamStore,
        "\\System\\TcpipCfg", "CommIPAddr",
        cTcpipCfgParam2.szIp, 31,
        "IP地址", "", 5
    );
    HvParamWriteString(
        &g_cParamStore,
        "\\System\\TcpipCfg", "CommMask",
        cTcpipCfgParam2.szNetmask, 31,
        "子网掩码", "", 5
    );
    HvParamWriteString(
        &g_cParamStore,
        "\\System\\TcpipCfg", "CommGateway",
        cTcpipCfgParam2.szGateway, 31,
        "网关", "", 5
    );
#else
    #ifdef _CAM_APP_
        HvParamWriteString(
            &g_cParamStore,
            "\\System\\TcpipCfg", "CommIPAddr",
            cTcpipCfgParam2.szIp, 31,
            "IP地址", "", 5
        );
        HvParamWriteString(
            &g_cParamStore,
            "\\System\\TcpipCfg", "CommMask",
            cTcpipCfgParam2.szNetmask, 31,
            "子网掩码", "", 5
        );
        HvParamWriteString(
            &g_cParamStore,
            "\\System\\TcpipCfg", "CommGateway",
            cTcpipCfgParam2.szGateway, 31,
            "网关", "", 5
        );
    #endif
#endif

    g_cParamStore.Save();

    return 0;
}

// 参数“恢复默认设置”
HRESULT RestoreDefaultParam()
{
    TcpipParam cTcpipParam1;
    TcpipParam cTcpipParam2;

#ifndef _CAM_APP_
    int nWorkMode = 1;
#endif

    // 读出当前的网络配置信息
#ifdef SINGLE_BOARD_PLATFORM
    HvParamReadString(&g_cParamStore, "\\System\\CamLan", "CommIPAddr", cTcpipParam1.szIp, 32);
    HvParamReadString(&g_cParamStore, "\\System\\CamLan", "CommMask", cTcpipParam1.szNetmask, 32);
    HvParamReadString(&g_cParamStore, "\\System\\CamLan", "CommGateway", cTcpipParam1.szGateway, 32);
    HvParamReadString(&g_cParamStore, "\\System\\TcpipCfg", "CommIPAddr", cTcpipParam2.szIp, 32);
    HvParamReadString(&g_cParamStore, "\\System\\TcpipCfg", "CommMask", cTcpipParam2.szNetmask, 32);
    HvParamReadString(&g_cParamStore, "\\System\\TcpipCfg", "CommGateway", cTcpipParam2.szGateway, 32);
    HvParamReadInt(&g_cParamStore, "\\Tracker", "PlateRecogMode", &nWorkMode, nWorkMode);
#else
    #ifdef _CAM_APP_
        HvParamReadString(&g_cParamStore, "\\System\\TcpipCfg", "CommIPAddr", cTcpipParam2.szIp, 32);
        HvParamReadString(&g_cParamStore, "\\System\\TcpipCfg", "CommMask", cTcpipParam2.szNetmask, 32);
        HvParamReadString(&g_cParamStore, "\\System\\TcpipCfg", "CommGateway", cTcpipParam2.szGateway, 32);
    #else
        HvParamReadInt(&g_cParamStore, "\\Tracker", "PlateRecogMode", &nWorkMode, nWorkMode);
    #endif
#endif

    // 将参数清空
    SetParamXml(&g_cParamStore, "");

#ifndef _CAM_APP_
    HvParamReadIntWithWrite(
        &g_cParamStore,
        "\\Tracker", "PlateRecogMode",
        &nWorkMode, nWorkMode,
        0, 14,
        "工作模式索引", "", 1
    );
#endif

    // 恢复清空之前的网络配置信息
    SaveTcpipParam(cTcpipParam1, cTcpipParam2);

    HV_Trace(5, "RestoreDefaultParam is Succ.\n");
    return S_OK;
}

// 参数“恢复出厂设置”
HRESULT RestoreFactoryParam()
{
    // 将参数清空
    SetParamXml(&g_cParamStore, "");

    TcpipParam cTcpipParamTmp;
    DWORD32 dwIP = 0;
    DWORD32 dwMask = 0;
    DWORD32 dwGateway = 0;
    MyGetIpDWord(cTcpipParamTmp.szIp, dwIP);
    MyGetIpDWord(cTcpipParamTmp.szNetmask, dwMask);
    MyGetIpDWord(cTcpipParamTmp.szGateway, dwGateway);
    dwIP = htonl(dwIP);
    dwMask = htonl(dwMask);
    dwGateway = htonl(dwGateway);

    // 同步Uboot的网络地址
    SetUbootNetAddr(dwIP, dwMask, dwGateway);

    // 清空复位次数
    EmptyResetCount();

    HV_Trace(5, "RestoreFactoryParam is Succ.\n");
    return S_OK;
}

bool IsRecoverFactorySettings()
{
    // CPLD读取出厂默认设置寄存器
    unsigned char bReg = 0;
    bool fReadyRead = false;
    // 试读 CPLD 5次
    int i;
    for (i=0; i<5; ++i)
    {
        bReg = 0;

        if (0 > SwCpldRead(0x11, &bReg))
        {
            // cpld 读失败
            usleep(5000);
            continue;
        }

        // 第2位为1时表示CPLD内部EEPROM忙;如果读出值为1，则延时5ms后再读一次。
        if ((bReg & 0x4) == 0x4)
        {
            // cpld 忙
            usleep(5000);
            continue;
        }

        // 第1位为1时表示这次读出的位0内容有效
        if ((bReg & 0x2) == 0x2)
        {
            fReadyRead = 1;
            break;
        }

        // 读出值为0，延时5ms后再读一次。
        usleep(5000);
    }

    bool fRet = false;

    // 如果0位有效，读0位
    if (fReadyRead && ((bReg & 0x1) == 0x1))
    {
        fRet = true;
    }

    return fRet;
}

int RecoverFactorySettings()
{
    int hr = E_FAIL;

    // 有发现写入返回成功但写入失败的情况
    // 尝试写入并读出验证，最多判断3次

    unsigned char bReg = 0xF8;   // CPLD后3位清0
    int iTryTimes = 3;

    while (iTryTimes != 0)
    {
        if (0 <= SwCpldWrite(0x11, bReg))
        {
            usleep(5000);
            if (!IsRecoverFactorySettings())
            {
                break;
            }
        }
        --iTryTimes;
    }

    if (iTryTimes > 0)
    {
        // 参数“恢复出厂设置”
        hr = RestoreFactoryParam();
    }

    return hr;
}
