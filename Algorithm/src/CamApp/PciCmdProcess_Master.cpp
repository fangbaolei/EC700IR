// 该文件编码格式必须为WINDOWS-936格式

#include "HvPciLinkApi.h"
#include "sendnetdata.h"
#include "DataCtrl.h"
#include "tinyxml.h"
#include "ControlFunc.h"
#include "DspLinkMemBlocks.h"
#include "HvSerialLink.h"
#include "IPT.h"
#include "HvDebugStateInfo.h"

extern ModuleParams g_cModuleParams;
extern HRESULT GetYPlateByXmlExtInfo(char* szXmlExtInfo, int& iYPlate);
extern HRESULT GetMiddleString(
        char *pszSrc,
        const char *pszBegin,
        const char *pszEnd,
        char *pszOut,
        int nBufSize
    );

CPciCmdProcessMaster::CPciCmdProcessMaster()
{
    m_fSendRecord = FALSE;
    m_dwLastTime = 0;
    m_pRadarBase = NULL;
    CreateSemaphore(&m_hSemQueCount, 0, MAX_CMDINFO_COUNT);
    CreateSemaphore(&m_hSemQueCtrl, 1, 1);
    m_pResultSenderParam = new ResultSenderParam();
    m_nTriggerType = 0;
}

CPciCmdProcessMaster::~CPciCmdProcessMaster()
{
    DestroySemaphore(&m_hSemQueCount);
    DestroySemaphore(&m_hSemQueCtrl);
    if (m_pResultSenderParam)
    {
        delete m_pResultSenderParam;
        m_pResultSenderParam = NULL;
    }
}

HRESULT CPciCmdProcessMaster::PciSendString(PCI_STRING_INFO* pcStringInfo)
{
    HRESULT hResult = E_FAIL;
    if ( NULL != m_cPciParam.pRecordLink && pcStringInfo != NULL )
    {
        char szVideoID[8] = {0};
        char szStreamID[8] = {0};
        char szTimeLow[16] = {0};
        char szTimeHigh[16] = {0};
        sprintf(szVideoID,  "%d", pcStringInfo->wVideoID);
        sprintf(szStreamID, "%d", pcStringInfo->wStreamID);
        sprintf(szTimeLow,  "%u", pcStringInfo->dwTimeLow);
        sprintf(szTimeHigh, "%u", pcStringInfo->dwTimeHigh);

        /*
        <?xml version="1.0" encoding="GB2312" standalone="yes" ?>
        <HvStringInfo VideoID="xxx" StreamID="xxx" TimeLow="xxx" TimeHigh="xxx" />
        */
        TiXmlDocument cStringInfoXmlDoc;
        TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
        TiXmlElement* pRootElement = new TiXmlElement("HvStringInfo");

        pRootElement->SetAttribute("VideoID", szVideoID);
        pRootElement->SetAttribute("StreamID", szStreamID);
        pRootElement->SetAttribute("TimeLow", szTimeLow);
        pRootElement->SetAttribute("TimeHigh", szTimeHigh);

        cStringInfoXmlDoc.LinkEndChild(pDecl);
        cStringInfoXmlDoc.LinkEndChild(pRootElement);

        TiXmlPrinter cPrinter;
        cStringInfoXmlDoc.Accept(&cPrinter);

        IReferenceMemory* pRefMemory = NULL;
        if ( S_OK != CreateReferenceMemory(&pRefMemory, sizeof(PCI_STRING_INFO)) )
        {
            HV_Trace(5, "<PCILINK_SEND_STRING> pRefMemory is NULL..\n");
        }
        else
        {
            PBYTE8 pbStringInfo = NULL;
            pRefMemory->GetData(&pbStringInfo);

            strcpy((char*)pbStringInfo, pcStringInfo->szString);

            // 发送字符串
            SEND_RECORD recordInfo;
            recordInfo.dwRecordType = CAMERA_RECORD_STRING;
            recordInfo.pbXML = (PBYTE8)cPrinter.CStr();
            recordInfo.dwXMLSize = cPrinter.Size();
            recordInfo.pbRecord = pbStringInfo;
            recordInfo.dwRecordSize = strlen((char*)pbStringInfo)+1;
            recordInfo.pRefMemory = pRefMemory;

            DWORD32 dwTimeLow = 0;
            DWORD32 dwTimeHigh = 0;
            ConvertTickToSystemTime(GetSystemTick(), dwTimeLow, dwTimeHigh);
            if ( FAILED(m_cPciParam.pRecordLink->SendRecord(&recordInfo, dwTimeLow, dwTimeHigh)))
            {
                HV_Trace(5, "<link> SendRecord(HvStringInfo) failed!\n");
            }
            else
            {
                hResult = S_OK;
            }

            pRefMemory->Release();
        }
    }
    return hResult;
}

HRESULT CPciCmdProcessMaster::Start(void *pvParamter)
{
    m_IPT.Start(NULL);
    return CHvThreadBase::Start(pvParamter);
}

HRESULT CPciCmdProcessMaster::Run(void* pvParam)
{
    int nPciHandle;
    while (!m_fExit)
    {
        m_dwLastTime = GetSystemTick();

        if (0 != SemPend(&m_hSemQueCount, 1000))
        {
            continue;
        }

        if (S_OK == GetOneCmd(nPciHandle))
        {
            ProcessCmd(nPciHandle);
        }
    }
    return S_OK;
}

HRESULT CPciCmdProcessMaster::ProcessCmd(int nPciHandle)
{
    HRESULT hResult = S_OK;
    PCI_CMDHEAD_INFO cCmdHeadInfo;
    SLAVE_CPU_STATUS cSlaveCpuStatus;
    int iHvExitCode = 0;
    const int nAppendInfSize = 32 * 1024;  // 32KB
    static char *pszAppendInfo = new char[nAppendInfSize];
    static PCI_STRING_INFO *pcStringInfo = new PCI_STRING_INFO;
    static PCI_DYNPARAM_INFO *pcDynParam = new PCI_DYNPARAM_INFO;

    if (g_cHvPciLinkApi.BeginRecvData(nPciHandle, &cCmdHeadInfo) != S_OK)
    {
        return E_FAIL;
    }

    switch (cCmdHeadInfo.emDataType)
    {
    case PCILINK_SLAVE_CPU_STATE:
    {
        g_cHvPciLinkApi.RecvData(nPciHandle, &cSlaveCpuStatus, sizeof(SLAVE_CPU_STATUS));

        REAL_TIME_STRUCT tTime;
        ConvertMsToTime(cSlaveCpuStatus.dwTimeLow, cSlaveCpuStatus.dwTimeHigh, &tTime);
        if (cSlaveCpuStatus.nStatusID < 0)
        {
            hResult = E_FAIL;
            HV_Trace(
                5,
                "%04d-%02d-%02d %02d:%02d:%02d: %s\n",
                tTime.wYear, tTime.wMonth, tTime.wDay,
                tTime.wHour, tTime.wMinute, tTime.wSecond,
                cSlaveCpuStatus.szErrorText
            );

            HV_Trace(5, "PCILINK_SLAVE_CPU_STATE is Error,ResetHv.\n");
            ResetHv(-1);
        }
        else
        {
            hResult = S_OK;
            HV_Trace(
                5,
                "Slave Status is Normal: %04d-%02d-%02d %02d:%02d:%02d\n",
                tTime.wYear, tTime.wMonth, tTime.wDay,
                tTime.wHour, tTime.wMinute, tTime.wSecond
            );
        }

        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }

    case PCILINK_RECOGNITION_RESULT:
    {
        hResult = E_FAIL;

        if (cCmdHeadInfo.dwDataSize > (DWORD32)PCILINK_DATA_MAX_LEN)
        {
            HV_Trace(5, "<PCILINK_RECOGNITION_RESULT> Data size overflow..\n");
            g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
            break;
        }

        g_cHvPciLinkApi.RecvData(nPciHandle, pszAppendInfo, nAppendInfSize);
        cCmdHeadInfo.dwDataSize -= (DWORD32)nAppendInfSize;
        // todo... 需要精确吗？
        DWORD32 dwPlateTick = GetSystemTick();
        // zhaopy
        IReferenceMemory* pRefMemory = NULL;
        static DWORD32 dwRefDataSize = 2.5 * 1024 * 1024;
        if ( S_OK != CreateReferenceMemory(&pRefMemory, dwRefDataSize) )
        {
            HV_Trace(5, "<PCILINK_RECOGNITION_RESULT> pRefMemory is NULL..\n");
            g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
            break;
        }
        PBYTE8 pbRecord = NULL;
        pRefMemory->GetData(&pbRecord);
        DWORD32 dwRecordSize = cCmdHeadInfo.dwDataSize;
        g_cHvPciLinkApi.RecvData(nPciHandle, pbRecord, dwRecordSize);

        // 处理识别结果
        char szPlateStr[32];
        strcat(pszAppendInfo, "\r\n");
        if ( S_OK == GetPlateNumByXmlExtInfo(pszAppendInfo, szPlateStr) )
        {
            HV_Trace(5, "%s\n", szPlateStr);

            if ( TRUE == m_fSendRecord && NULL != m_cPciParam.pRecordLink )
            {
                // 发送识别结果
                SEND_RECORD recordInfo;
                recordInfo.dwRecordType = CAMERA_RECORD_NORMAL;
                recordInfo.pbXML = (PBYTE8)pszAppendInfo;
                recordInfo.dwXMLSize = nAppendInfSize;
                recordInfo.pbRecord = pbRecord;
                recordInfo.dwRecordSize = dwRecordSize;
                recordInfo.pRefMemory = pRefMemory;

                static char szOutText[256];
                DWORD32 dwTimeLow = 0;
                DWORD32 dwTimeHigh = 0;
                if (GetMiddleString(pszAppendInfo, "<TimeLow", ">", szOutText, 256) == S_OK)
                {
                    sscanf(szOutText, "<TimeLow value=\"%u\" />", &dwTimeLow);
                }
                if (GetMiddleString(pszAppendInfo, "<TimeHigh", ">", szOutText, 256) == S_OK)
                {
                    sscanf(szOutText, "<TimeHigh value=\"%u\" />", &dwTimeHigh);
                }
                if ( FAILED(m_cPciParam.pRecordLink->SendRecord(&recordInfo, dwTimeLow, dwTimeHigh)))
                {
                    HV_Trace(3, "<link> SendRecord failed!\n");
                }
                else
                {
                    hResult = S_OK;
                }
            }
        }

        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        int iYPlate = 0;
        GetYPlateByXmlExtInfo(pszAppendInfo, iYPlate);
        g_pCamApp->SetPlateY(iYPlate, dwPlateTick);
        // zhaopy
        pRefMemory->Release();
        break;
    }

    case PCILINK_SET_AGC_STATE:
    {
        int nAGCValue = 0; //0为关闭AGC，1为开启AGC
        g_cHvPciLinkApi.RecvData(nPciHandle, &nAGCValue, sizeof(int));

        // AGC设置
        if ( NULL != g_pCamApp )
        {
            Trace("<PCILink> EnableAGC\n");
            hResult = g_pCamApp->DynChangeParam(DCP_ENABLE_AGC, nAGCValue);
        }
        else
        {
            hResult = E_FAIL;
        }

        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }

    case PCILINK_SET_AWB_STATE:
    {
        int nAWBValue = 0; //0为自动白平衡，1为手动白平衡
        g_cHvPciLinkApi.RecvData(nPciHandle, &nAWBValue, sizeof(int));

        // AWB设置
        nAWBValue = (0 == nAWBValue) ? 1 : 0;
        if ( NULL != g_pCamApp )
        {
            Trace("<PCILink> EnableAWB\n");
            hResult = g_pCamApp->DynChangeParam(DCP_ENABLE_AWB, nAWBValue);
        }
        else
        {
            hResult = E_FAIL;
        }

        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }

    case PCILINK_SEND_STRING:
    {
        g_cHvPciLinkApi.RecvData(nPciHandle, pcStringInfo, cCmdHeadInfo.dwDataSize);
        hResult = PciSendString(pcStringInfo);
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }

    case PCILINK_DEVSTATUS_STRING:
    {
        hResult = E_FAIL;
        if (m_cPciParam.pRecordLink)
        {
            float fltFrameRate;
            g_cHvPciLinkApi.RecvData(nPciHandle, &fltFrameRate, sizeof(fltFrameRate));
            pcStringInfo->wVideoID = 0;
            pcStringInfo->wStreamID = _TYPE_PERF_STR;
            pcStringInfo->dwTimeLow = 0;
            pcStringInfo->dwTimeHigh = 0;

            if (m_nTriggerType == 0)
            {
                sprintf(
                    pcStringInfo->szString,
                    "客户端状态:%s\n硬盘工作状态:%s\n处理帧率:%0.2f",
                    ((CCameraRecordLinkCtrl*)m_cPciParam.pRecordLink)->GetLinkCount() > 0 ? "连接" : "断开",
                    g_cHddOpThread.GetCurStatusCodeString(),
                    fltFrameRate
                );
            }
            else
            {
                sprintf(
                    pcStringInfo->szString,
                    "客户端状态:%s\n硬盘工作状态:%s\n处理帧率:%0.2f\n触发抓拍类型:%s",
                    ((CCameraRecordLinkCtrl*)m_cPciParam.pRecordLink)->GetLinkCount() > 0 ? "连接" : "断开",
                    g_cHddOpThread.GetCurStatusCodeString(),
                    fltFrameRate,
                    m_nTriggerType == 1 ? "线圈触发" : "视频触发"
                );
            }
            hResult = PciSendString(pcStringInfo);
        }
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }

    case PCILINK_SET_CAM_DYN_PARAM:
    {
        g_cHvPciLinkApi.RecvData(nPciHandle, pcDynParam, sizeof(PCI_DYNPARAM_INFO));
        if (g_pCamApp)
        {
            HV_Trace(5, "<PCILink> DynChangeParam[%d][%d]\n", pcDynParam->eType, pcDynParam->nValue);
            hResult = g_pCamApp->DynChangeParam(pcDynParam->eType, pcDynParam->nValue, true);
            if ( S_OK == hResult )
            {
                switch ( pcDynParam->eType )
                {
                case DCP_ENABLE_AGC:
                    SaveAGCEnable(pcDynParam->nValue);
                    break;
                case DCP_AGC_TH:
                    SaveAGCTh(pcDynParam->nValue);
                    break;
                case DCP_GAIN:
                    SaveGain(pcDynParam->nValue);
                    break;
                case DCP_SHUTTER:
                    SaveShutter(pcDynParam->nValue);
                    break;

                default:
                    break;
                }
            }
            if (pcDynParam->eType == DCP_SETPULSEWIDTH)
            {
                int iPulseLevel = 0, iCplStatus = 0;
                g_pCamApp->GetCurrentPulseLevel(iPulseLevel, iCplStatus);
                g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &iPulseLevel, sizeof(iPulseLevel));
                g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &iCplStatus, sizeof(iCplStatus));
                break;
            }
        }
        else
        {
            hResult = E_FAIL;
        }
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }

    case PCILINK_SET_CAPTURE_SYNSIGNAL:
    {
        int iStatus = -1;
        g_cHvPciLinkApi.RecvData(nPciHandle, &iStatus, sizeof(iStatus));
        if (g_pCamApp)
        {
            if (g_pCamApp->SetCaptureSynSignalStatus(iStatus) == E_FAIL)
            {
                hResult = E_FAIL;
                iStatus = -1;
                g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &iStatus, sizeof(iStatus));
                break;
            }
            else
            {
                g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &iStatus, sizeof(iStatus));
                break;
            }
        }
        hResult = E_FAIL;
        iStatus = -1;
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &iStatus, sizeof(iStatus));
        break;
    }
    case PCILINK_DEBUG_SLAVE_IMAGE:
    {
        hResult = E_FAIL;
        // zhaopy
        IReferenceMemory* pRefMemory = NULL;
        if ( S_OK != CreateReferenceMemory(&pRefMemory, cCmdHeadInfo.dwDataSize) )
        {
            HV_Trace(5, "<PCILINK_DEBUG_SLAVE_IMAGE> pRefMemory is NULL! [size:%d]\n", cCmdHeadInfo.dwDataSize);
            break;
        }
        PBYTE8 pbJpegData = NULL;
        pRefMemory->GetData(&pbJpegData);

        //图片信息
        HV_COMPONENT_IMAGE img;
        g_cHvPciLinkApi.RecvData(nPciHandle, &img, sizeof(HV_COMPONENT_IMAGE));

        //图片偏移大小
        DWORD32 dwImageOffset = 0;
        g_cHvPciLinkApi.RecvData(nPciHandle, &dwImageOffset, sizeof(DWORD32));

        //图片偏移信息+图片数据
        g_cHvPciLinkApi.RecvData(nPciHandle, pbJpegData, dwImageOffset + img.iWidth);

        SEND_CAMERA_IMAGE imageInfo;
        imageInfo.dwImageType = CAMERA_IMAGE_JPEG_SLAVE;  // 从CPU端的图片
        imageInfo.dwWidth = img.iHeight & 0x0000FFFF;
        imageInfo.dwHeight = img.iHeight >> 16;
        imageInfo.dwImageSize = dwImageOffset + img.iWidth;
        imageInfo.dwImageOffset = dwImageOffset;
        imageInfo.pbImage = pbJpegData;
        // zhaopy
        imageInfo.pRefMemory = pRefMemory;

        // 发送图片
        if ( m_cPciParam.pImageLink != NULL )
        {
            if ( FAILED(m_cPciParam.pImageLink->SendCameraImage(&imageInfo)))
            {
                Trace("<link>SendCameraImage failed!\n");
            }
        }

        pRefMemory->Release();
        break;
    }

    case PCILINK_HV_EXIT:
    {
        g_cHvPciLinkApi.RecvData(nPciHandle, &iHvExitCode, sizeof(iHvExitCode));

        time_t iTime = 0;
        tm* pcTM = NULL;
        char pbDateTime[64] = {0};
        char pbBuf[1024] = {0};
        iTime = time(NULL);
        pcTM = gmtime(&iTime);
        sprintf(pbDateTime, "%d/%02d/%02d %02d:%02d:%02d",
                pcTM->tm_year + 1900,
                pcTM->tm_mon + 1,
                pcTM->tm_mday,
                pcTM->tm_hour,
                pcTM->tm_min,
                pcTM->tm_sec);
        sprintf(pbBuf, "ver:[%s %s %s],datetime:[%s],info:[%s].",
                PSZ_DSP_BUILD_NO,
                DSP_BUILD_DATE,
                DSP_BUILD_TIME,
                pbDateTime,
                "PCILINK_HV_EXIT");
        WriteDataToFile("exit.txt", (unsigned char*)pbBuf, strlen(pbBuf));
        break;
    }

    case PCILINK_RESET_HV:
    {
        HV_Trace(5, "PCILINK_RESET_HV,ResetHv.\n");
        ResetHv(-1);
        break;
    }
#ifndef IPT_IN_MASTER
    case PCILINK_PCICOM:
    {
        bool bAct = false;
        unsigned int nRetLen = 0;

        // by ganzz
        static CHvSerialLink* s_pSerialLink = NULL;
        if (s_pSerialLink == NULL)
        {
            s_pSerialLink = new CHvSerialLink();
            if (s_pSerialLink->Open("/dev/ttyS0") != S_OK)
            {
                HV_Trace(5, "<PCILINK_PCICOM> open com failed. \n");
                delete s_pSerialLink;
                s_pSerialLink = NULL;
            }
        }

        g_cHvPciLinkApi.RecvData(nPciHandle, pszAppendInfo, cCmdHeadInfo.dwDataSize);

        if (s_pSerialLink != NULL)
        {
            unsigned char b = 1;
            if (S_OK == s_pSerialLink->SendCmdData(0xA1, &b, 1))
            {
                HV_Sleep(1);
                // 使用pszAppendInfo作缓存
                if (dwDataSize == (DWORD32)s_pSerialLink->Send((unsigned char*)pszAppendInfo, cCmdHeadInfo.dwDataSize))
                {
                    bAct = true;
                }
            }
            else
            {
                HV_Trace(5, "SendCmdData failed.\n");
            }
        }

        //如果需要返回值则读串口
        // TODO:新PCI机制通信都需要返回值，此处是否要做处理？
        if (1)
        {
            if (bAct)
            {
                // 假定pszAppendInfo的32k缓存还足够装,从dwDataSize偏移处接收
                if ( 0 == s_pSerialLink->RecvPacket(
                            (unsigned char*)pszAppendInfo + cCmdHeadInfo.dwDataSize,
                            &nRetLen,
                            1000) )
                {
                    // 接收成功，回传接收的数据
                    g_cHvPciLinkApi.WriteReturnValue(nPciHandle, pszAppendInfo + cCmdHeadInfo.dwDataSize, nRetLen);
                }
                else
                {
                    HV_Trace(5, "PCILINK_PCICOM recv failed. %d\n", nRetLen);
                    // 接收不到，认为执行失败，回传执行失败信息
                    bAct = false;
                }
            }

            if (!bAct)
            {
                // 接收失败，回传执行失败信息
                unsigned char bCmdID;
                unsigned char bCmdSN;
                unsigned char bResult = 0;
                // 解包，取CmdID和SN
                s_pSerialLink->PickupDataFromPacket(
                    (unsigned char*)pszAppendInfo,
                    cCmdHeadInfo.dwDataSize,
                    &bCmdID,
                    &bCmdSN,
                    NULL,
                    NULL
                );
                // 组执行失败包
                s_pSerialLink->MakePacket(
                    bCmdID,
                    bCmdSN,
                    (unsigned char*)&bResult,
                    1,
                    (unsigned char*)pszAppendInfo,
                    &nRetLen
                );

                g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &pszAppendInfo, nRetLen);
            }
        }
        //s_cSerialLink.Close();
        break;
    }
#else
    case PCILINK_PCIIPTPARAM:
    {
        hResult = S_OK;
        g_cHvPciLinkApi.RecvData(nPciHandle, pszAppendInfo, cCmdHeadInfo.dwDataSize);
        if (!m_IPT.IsValid())
        {
            if (!m_IPT.Initialize((BYTE8 *)pszAppendInfo, cCmdHeadInfo.dwDataSize))
            {
                hResult = E_FAIL;
            }
        }
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }
#endif // IPT_IN_MASTER
    case PCILINK_OUTERDEVICE_TYPE:
    {
        g_cHvPciLinkApi.RecvData(nPciHandle, pszAppendInfo, cCmdHeadInfo.dwDataSize);
        HV_Trace(5, "PCILINK_OUTERDEVICE_TYPE=====dwDataSize=%d, Radar Type = %d---\n", cCmdHeadInfo.dwDataSize, *(int *)pszAppendInfo);
        switch (*(int *)pszAppendInfo)
        {
        case 1 :
            m_pRadarBase = new CCSRIRadar();
            break;
        }
        if (m_pRadarBase)
        {
            m_pRadarBase->Initialize();
        }
        break;
    }
    case PCILINK_SLAVE_LOG:
    {
        g_cHvPciLinkApi.RecvData(nPciHandle, pszAppendInfo, cCmdHeadInfo.dwDataSize);
        HV_Trace(5, "slave log ==== %s", pszAppendInfo);
        break;
    }
    case PCILINK_RESULT_PARAM:
    {
        g_cHvPciLinkApi.RecvData(nPciHandle, m_pResultSenderParam, cCmdHeadInfo.dwDataSize);
        HV_Trace(5, "<CPciCmdProcessMaster>TYPE:PCILINK_RESULT_PARAM,"
                 " DataSize:%u, iOutputOnlyPeccancy = %d, iFlashDifferentLane=%d\n",
                 cCmdHeadInfo.dwDataSize,
                 m_pResultSenderParam->iOutputOnlyPeccancy,
                 m_pResultSenderParam->iFlashDifferentLane);

        if (m_cPciParam.pRecordLink)
        {
            SEND_RECORD_PARAM cParam;
            cParam.iOutputOnlyPeccancy = m_pResultSenderParam->iOutputOnlyPeccancy;
            cParam.iSendRecordSpace = m_pResultSenderParam->iSendRecordSpace;
            m_cPciParam.pRecordLink->SetParam(&cParam);
        }
        if (m_cPciParam.pVideoLink)
        {
            SEND_VIDEO_PARAM cParam;
            cParam.iSendHisVideoSpace = m_pResultSenderParam->iSendHisVideoSpace;
            m_cPciParam.pVideoLink->SetParam(&cParam);
        }
        //闪光灯分车道闪
        g_cModuleParams.cCamAppParam.iFlashDifferentLane = m_pResultSenderParam->iFlashDifferentLane;
        g_cCameraController.FlashDifferentLane(g_cModuleParams.cCamAppParam.iFlashDifferentLane);
        break;
    }
    case PCILINK_FLASHLIGHT_TYPE:
    {
        int nType = 0;
        g_cHvPciLinkApi.RecvData(nPciHandle, &nType, sizeof(nType));
        switch (nType)
        {
        case 0 : //不闪
            m_nTriggerType = 0;
            g_cCameraController.FlashDifferentLaneExt(0);
            g_cCameraController.FlashDifferentLane(0);
            break;
        case 1 : //硬触发
            HV_Trace(5, "硬触发抓拍");
            m_nTriggerType = 1;
            g_cCameraController.FlashDifferentLaneExt(1);
            g_cCameraController.FlashDifferentLane(0);
            break;
        case 2 : //软触发
            HV_Trace(5, "软触发抓拍");
            m_nTriggerType = 2;
            g_cCameraController.FlashDifferentLaneExt(0);
            g_cCameraController.FlashDifferentLane(1);
            break;
        }
        break;
    }
    default:
    {
        HV_Trace(5, "Unknowed command..\n");
    }
    }

    // 结束接收
    g_cHvPciLinkApi.EndRecvData(nPciHandle, cCmdHeadInfo);

    if (cCmdHeadInfo.emDataType == PCILINK_HV_EXIT)
    {
        HV_Trace(5, "ResetHv[%d]...", iHvExitCode);
        HV_Sleep(1000);
        exit(iHvExitCode);
    }

    return S_OK;
}

HRESULT CPciCmdProcessMaster::PutOneCmd(int nPciHandle)
{
    HRESULT hResult = S_OK;
    SemPend(&m_hSemQueCtrl);
    if (m_queCmdHandle.GetSize() < MAX_CMDINFO_COUNT)
    {
        m_queCmdHandle.AddTail(nPciHandle);
        if (0 != SemPost(&m_hSemQueCount))
        {
            HV_Trace(5, "Put one cmd failed!");
            m_queCmdHandle.RemoveTail();
            hResult = E_FAIL;
        }
    }
    else
    {
        HV_Trace(5, "Pci cmd list is full, put one cmd failed!");
        hResult = E_FAIL;
    }
    SemPost(&m_hSemQueCtrl);
    return hResult;
}

HRESULT CPciCmdProcessMaster::GetOneCmd(int& nPciHandle)
{
    HRESULT hResult = E_FAIL;
    SemPend(&m_hSemQueCtrl);
    if (m_queCmdHandle.GetSize() > 0)
    {
        hResult = S_OK;
        nPciHandle = m_queCmdHandle.RemoveHead();
    }
    SemPost(&m_hSemQueCtrl);
    return hResult;
}

HRESULT CPciCmdProcessMaster::IsThreadOk()
{
    DWORD32 dwCurTick = GetSystemTick();
    if (dwCurTick < m_dwLastTime || m_dwLastTime == 0)
    {
        return S_OK;
    }
    if (dwCurTick - m_dwLastTime > 20000)
    {
        HV_Trace(5, "PciCmdProcess timeout! CurTick = %d, LastTick = %d.", dwCurTick, m_dwLastTime);
        return E_FAIL;
    }
    return S_OK;
}
