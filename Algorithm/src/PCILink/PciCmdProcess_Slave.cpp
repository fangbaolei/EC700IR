// 该文件编码格式必须为WINDOWS-936格式

#include "HvPciLinkApi.h"
#include "HvParamStore.h"
#include "HvParamIO.h"
#include "swimageobj.h"
#include "ControlFunc.h"
#include "tinyxml.h"
#include "OuterControlImpl_Linux.h"
#include "misc.h"
#include "VideoGetter_VPIF.h"

#define MAX_PCI_RECV_BUF_SIZE   (1024 * 1024)
int g_iSendSlaveImage = 0;  // 发送“调试从端图片” 0:不发送；1：发送

/* misc.cpp */
extern void DebugPrintf(const char* szDebugInfo, DWORD dwDumpLen, const char* szID);
extern "C" int GetDeviceState(unsigned long* pulStateCode);
extern "C" int SetDeviceState(unsigned long ulStateCode);

using namespace HiVideo;
using namespace HvSys;

extern CParamStore g_cParamStore;
extern int g_iControllPannelWorkStyle;

static HRESULT PciClockSyncData(int nPciHandle)
{
    HRESULT hr = E_FAIL;

    DWORD32 dwTimeLow, dwTimeHigh;
    g_cHvPciLinkApi.RecvData(nPciHandle, &dwTimeLow, 4);
    g_cHvPciLinkApi.RecvData(nPciHandle, &dwTimeHigh, 4);

    if ( SetSystemTime(dwTimeLow, dwTimeHigh) != 0 )
    {
        HV_Trace(5, "Sync Time Failed...\n");
    }
    else
    {
        // 确保设置的系统时间已生效
        DWORD32 dwTimeLowNow, dwTimeHighNow;
        DWORD32 dwWaitCount = 10;
        while (dwWaitCount--)
        {
            GetSystemTime(&dwTimeLowNow, &dwTimeHighNow);

            REAL_TIME_STRUCT tTimeNow;
            ConvertMsToTime(dwTimeLowNow, dwTimeHighNow, &tTimeNow);
            char szTimeNow[64] = {0};
            sprintf(
                szTimeNow,
                "%04d/%02d/%02d %02d:%02d:%02d",
                tTimeNow.wYear, tTimeNow.wMonth, tTimeNow.wDay,
                tTimeNow.wHour, tTimeNow.wMinute,tTimeNow.wSecond
            );

            if ( dwTimeHighNow > dwTimeHigh
                    || (dwTimeHighNow == dwTimeHigh && (dwTimeLowNow+1000) >= dwTimeLow) )
            {
                HV_Trace(5, "Sync Time OK. [Now Time: %s]\n", szTimeNow);
                hr = S_OK;
                break;
            }
            else
            {
                HV_Trace(5, "waiting Sync Time... [Now Time: %s]\n", szTimeNow);
                HV_Sleep(50);
            }
        }
    }

    return hr;
}

CPciCmdProcessSlave::CPciCmdProcessSlave()
{
    m_fShakeHandsSucceed = FALSE;
    m_fDataCtrlHandsSucceed = FALSE;    
    m_pbRecvData = NULL;
    m_dwLastTime = 0;
    CreateSemaphore(&m_hSemQueCount, 0, MAX_CMDINFO_COUNT);
    CreateSemaphore(&m_hSemQueCtrl, 1, 1);
}

CPciCmdProcessSlave::~CPciCmdProcessSlave()
{
    DestroySemaphore(&m_hSemQueCount);
    DestroySemaphore(&m_hSemQueCtrl);
    SAFE_DELETE_ARRAY(m_pbRecvData);
}

HRESULT CPciCmdProcessSlave::Run(void* pvParam)
{
    int nPciHandle;
    m_pbRecvData = new BYTE8[MAX_PCI_RECV_BUF_SIZE];
    if (!m_pbRecvData)
    {
        HV_Trace(5, "CPciCmdProcessSlave: Alloc memory failed!");
        return E_FAIL;
    }

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

    SAFE_DELETE_ARRAY(m_pbRecvData);

    return S_OK;
}

HRESULT CPciCmdProcessSlave::ProcessCmd(int nPciHandle)
{
    HRESULT hResult = S_OK;
    IReferenceComponentImage *pRefImage = NULL;
    ONE_FRAME_INFO cOneFrameInfo;
    PANORAMIC_IMAGE cPanormicImage;
    HV_COMPONENT_IMAGE imgJpeg;
    SYS_INFO cInfo;
    PCI_CMDHEAD_INFO cCmdHeadInfo;
    int iHvExitCode = 0;
    cPanormicImage.pImageData = NULL;

    if (g_cHvPciLinkApi.BeginRecvData(nPciHandle, &cCmdHeadInfo) != S_OK)
    {
        return E_FAIL;
    }

    switch (cCmdHeadInfo.emDataType)
    {
    case PCILINK_GET_CAMERA_PARAM:
    {
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, m_cPciParam.pCfgCamParam, sizeof(CAM_CFG_PARAM));
        break;
    }

    case PCILINK_GET_PANORAMIC_CAMARA_PARAM:
    {
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &m_cPciParam.cPanoramicCamaraParam
            , sizeof(PANORAMIC_CAPTURER_PARAM));
        break;
     }

    case PCILINK_SENE_PANNEL_PARAM:
    {
        hResult = g_cHvPciLinkApi.RecvData(nPciHandle, &g_iControllPannelWorkStyle, cCmdHeadInfo.dwDataSize);
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }

    case PCILINK_CLOCK_SYNC_DATA:
    {
        hResult = PciClockSyncData(nPciHandle);
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }

    case PCILINK_GET_AUTO_LINK_PARAM:
    {
        if(NULL != m_cPciParam.pAutoLinkParam )
        {
            g_cHvPciLinkApi.WriteReturnValue(nPciHandle,m_cPciParam.pAutoLinkParam , sizeof(AUTO_LINK_PARAM));
        }
        break;
    }

    case PCILINK_GET_TRAFFIC_LIGHT_PARAM:
    {
        if(NULL != m_cPciParam.pTrafficCfgParam)
        {
            TRAFFIC_LIGHT_PARAM cTrafficLightParam;
            cTrafficLightParam.fEnhanceRed = m_cPciParam.pTrafficCfgParam->fEnhanceRedLight;
            cTrafficLightParam.nLightCount = m_cPciParam.pTrafficCfgParam->nLightCount;
            if( sizeof( cTrafficLightParam.rgszLightPos ) >= sizeof( m_cPciParam.pTrafficCfgParam->rgszLightPos ))
            {
                memcpy(cTrafficLightParam.rgszLightPos ,  m_cPciParam.pTrafficCfgParam->rgszLightPos , sizeof( m_cPciParam.pTrafficCfgParam->rgszLightPos ) );
            }
            g_cHvPciLinkApi.WriteReturnValue(nPciHandle , &cTrafficLightParam , sizeof(TRAFFIC_LIGHT_PARAM));
        }
        break;
    }

    case PCILINK_PARAM_SET_DATA:
    {
        g_cHvPciLinkApi.RecvData(nPciHandle, m_pbRecvData, cCmdHeadInfo.dwDataSize);
        m_pbRecvData[cCmdHeadInfo.dwDataSize] = 0;

        if ( 0 == cCmdHeadInfo.dwDataSize )
        {
            hResult = SetParamXml(&g_cParamStore, "");
        }
        else if (strcmp((char*)m_pbRecvData, "RestoreDefaultParam") == 0)
        {
            hResult = RestoreDefaultParam();
        }
        else
        {
            TiXmlDocument cDoc;
            cDoc.Parse((char*)m_pbRecvData);
            TiXmlElement* pHvParam = cDoc.RootElement()->FirstChildElement("HvParam");
            TiXmlElement* pParamCam;

            if (pHvParam)
            {
                pParamCam = pHvParam->FirstChildElement();
                while (pParamCam)
                {
                    TiXmlElement* pTempElement = pParamCam;
                    pParamCam = pParamCam->NextSiblingElement();
                    //删除主CPU端的参数
                    if (strcmp(pTempElement->Attribute("name"), "System") == 0
                            || strcmp(pTempElement->Attribute("name"), "CamApp") == 0)
                    {
                        pHvParam->RemoveChild(pTempElement);
                    }
                }
            }

            TiXmlPrinter cPrint;
            cDoc.Accept(&cPrint);

            strncpy((char*)m_pbRecvData, cPrint.CStr(), 1024 * 1024);
            hResult = SetParamXml(&g_cParamStore, (char*)m_pbRecvData);
            if (hResult == S_OK)
            {
                DebugPrintf((const char*)m_pbRecvData, 512, "SlaveXmlParam");
                HV_Trace(5, "SlaveXmlParam dwDataSize = [%d]\n", cCmdHeadInfo.dwDataSize);
            }
        }

        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }

    case PCILINK_PARAM_GET_DATA:
    {
        if ( S_OK == GetParamXml(
                    &g_cParamStore,
                    (char*)m_pbRecvData,
                    MAX_PCI_RECV_BUF_SIZE)
            )
        {
            g_cHvPciLinkApi.WriteReturnValue(nPciHandle, m_pbRecvData, strlen((char*)m_pbRecvData));
        }
        break;
    }

    case PCILINK_ONE_FRAME_DATA:
    {
        g_cHvPciLinkApi.RecvData(nPciHandle, &cOneFrameInfo, sizeof(cOneFrameInfo));
        imgJpeg = cOneFrameInfo.imgJpeg;
        IMG_FRAME frame;
        static DWORD32 dwFrameNo = 0;
        hResult = CreateReferenceComponentImage(
                 &pRefImage,
                 cOneFrameInfo.imgJpeg.nImgType,
                 cOneFrameInfo.imgJpeg.iHeight & 0x0000FFFF,
                 cOneFrameInfo.imgJpeg.iHeight >> 16,
                 dwFrameNo++, cOneFrameInfo.dwRefTime,
                 0, cOneFrameInfo.szFrameName, 2
             );
        if (S_OK == hResult)
        {
            if (S_OK == pRefImage->GetImage(&cOneFrameInfo.imgJpeg))
            {
                cOneFrameInfo.imgJpeg.iWidth = imgJpeg.iWidth;
                cOneFrameInfo.imgJpeg.iHeight = imgJpeg.iHeight;
                cOneFrameInfo.imgJpeg.iStrideWidth[0] = imgJpeg.iStrideWidth[0];
                cOneFrameInfo.imgJpeg.iStrideWidth[1] = imgJpeg.iStrideWidth[1];
                cOneFrameInfo.imgJpeg.iStrideWidth[2] = imgJpeg.iStrideWidth[2];
                g_cHvPciLinkApi.RecvData(nPciHandle,
                    GetHvImageData(&cOneFrameInfo.imgJpeg, 0),
                    imgJpeg.iWidth
                );
                pRefImage->SetImageSize(cOneFrameInfo.imgJpeg);
                frame.pRefImage = pRefImage;
                m_cPciParam.pVideoGetter->PutOneFrame(frame);
            }
            pRefImage->Release();
        }
        else if (pRefImage)
        {
            HV_Trace(5, "GetImage failed..\n");
            SAFE_RELEASE(pRefImage);
            hResult = E_FAIL;
        }
        else
        {
            HV_Trace(5, "Jpeg image create failed..\n");
            hResult = E_FAIL;
        }
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }

    case PCILINK_GET_WORKMODEINDEX:
    {
        int nWorkModeIndex = -1;
        g_cHvPciLinkApi.RecvData(nPciHandle, &nWorkModeIndex, cCmdHeadInfo.dwDataSize);

        cInfo.Info.WorkModeList.nMode = nWorkModeIndex;
        cInfo.Info.WorkModeList.pbListBuf = (char*)m_pbRecvData;

        GetWorkModeList(&cInfo);
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, m_pbRecvData, cInfo.Info.WorkModeList.nLen);
        break;
    }

    case PCILINK_GET_WORKMODE_COUNT:
    case PCILINK_GET_WORKMODE:
    {
        cInfo.Info.WorkMode.dwMode = (DWORD32)-1;
        cInfo.Info.WorkMode.dwModeCount = 0;
        GetWorkModeInfo(&cInfo);
        if (cCmdHeadInfo.emDataType == PCILINK_GET_WORKMODE)
        {
            g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &cInfo.Info.WorkMode.dwMode, 4);
        }
        else
        {
            g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &cInfo.Info.WorkMode.dwModeCount, 4);
        }
        break;
    }

    case PCILINK_SET_WORKMODE:
    {
        DWORD32 dwWorkMode = 0;
        g_cHvPciLinkApi.RecvData(nPciHandle, &dwWorkMode, cCmdHeadInfo.dwDataSize);
        hResult = SetWorkMode(dwWorkMode);
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }

    case PCILINK_SHAKE_HANDS:
    {
        m_fShakeHandsSucceed = TRUE;
        g_cHvPciLinkApi.SetShakeHandsStatus(true);
        hResult = PciClockSyncData(nPciHandle);
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }

    case PCILINK_DATACTRL_SHAKE_HANDS:
    {
        m_fDataCtrlHandsSucceed = TRUE;
        hResult = S_OK;
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }
    
    case PCILINK_SEND_SLAVE_IMAGE:
    {
        if ( 4 == cCmdHeadInfo.dwDataSize )
        {
            g_cHvPciLinkApi.RecvData(nPciHandle, &g_iSendSlaveImage, cCmdHeadInfo.dwDataSize);
            HV_Trace(5, "PCILINK_SEND_SLAVE_IMAGE [%d]\n", g_iSendSlaveImage);
        }
        break;
    }

    case PCILINK_FORCESEND:
    {
        DWORD32 dwVideoID;
        g_cHvPciLinkApi.RecvData(nPciHandle, &dwVideoID, sizeof(DWORD32));
        hResult = ForceSend(dwVideoID);
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }

    case PCILINK_SET_JPEG_CR:
    {
        g_cHvPciLinkApi.RecvData(nPciHandle, &g_nJpegCompressRate, sizeof(int));
        hResult = S_OK;
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }
    
    case PCILINK_SET_JPEG_TYPE:
    {
        g_cHvPciLinkApi.RecvData(nPciHandle, &g_nJpegType, sizeof(int));
        hResult = S_OK;
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }

    case PCILINK_SET_CAPTURE_CR:
    {
        g_cHvPciLinkApi.RecvData(nPciHandle, &g_nCaptureCompressRate, sizeof(int));
        hResult = S_OK;
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        break;
    }

    case PCILINK_SET_DEVICE_STATE:
    {
        DWORD32 dwResetModeWrite;
        DWORD32 dwResetModeRead;
        g_cHvPciLinkApi.RecvData(nPciHandle, &dwResetModeWrite, sizeof(DWORD32));

        hResult = E_FAIL;
        // 先取状态，比较不同后再设状态，再取状态进行比较，
        // 若取和设不一致再设，不成功三次失败返回
        for (int i=0; i<3; i++)
        {
            void* pVoidTmp = &dwResetModeRead;
            GetDeviceState((unsigned long*)pVoidTmp);

            if (dwResetModeRead != dwResetModeWrite)
            {
                SetDeviceState(dwResetModeWrite);
                HV_Trace(5, "Try SetDeviceState [%d]\n", dwResetModeWrite);
            }
            else
            {
                HV_Trace(5, "SetDeviceState [%d] OK\n", dwResetModeWrite);
                hResult = S_OK;
                break;
            }
        }

        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
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

    case PCILINK_PCIIPTPARAM:
    {
        g_cHvPciLinkApi.RecvData(nPciHandle, m_pbRecvData, cCmdHeadInfo.dwDataSize);
        if (m_cPciParam.pOuterControler)
        {
            ((COuterControlImpl *)(m_cPciParam.pOuterControler))->SetIPTParam(m_pbRecvData, cCmdHeadInfo.dwDataSize);
        }
        break;
    }

    case PCILINK_PCIIPT:
    {
        g_cHvPciLinkApi.RecvData(nPciHandle, m_pbRecvData, cCmdHeadInfo.dwDataSize);
        HV_Trace(5, "receive a signal from master, data size = %d\n", cCmdHeadInfo.dwDataSize);
        if (m_cPciParam.pOuterControler)
        {
            ((COuterControlImpl *)(m_cPciParam.pOuterControler))->SetEventData(m_pbRecvData, cCmdHeadInfo.dwDataSize);
        }
        else
        {
            HV_Trace(5, "m_cPciParam.pOuterControler is null, ignore the signal.\n");
        }
        break;
    }

    case PCILINK_GET_SLAVE_DEBUG_INFO:     // 读从黑盒子信息
    {
        // 如果正确，将返回所有读到的记录。否则返回E_FAIL
        int nStateSize = 1024 * 1024;
        hResult = HvDebugStateInfoReadAll((char*)m_pbRecvData, &nStateSize);
        if (hResult != S_OK)
        {
            g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        }
        else
        {
            g_cHvPciLinkApi.WriteReturnValue(nPciHandle, m_pbRecvData, (DWORD32)nStateSize);
        }
        break;
    }

    case PCILINK_GET_SLAVE_RESET_LOG:  // 取从复位记录
    {
        DWORD32 dwStateSize = 1024 * 1024;
        hResult = GetResetLog(m_pbRecvData, &dwStateSize);
        if (hResult != S_OK)
        {
            g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hResult, sizeof(hResult));
        }
        else
        {
            g_cHvPciLinkApi.WriteReturnValue(nPciHandle, m_pbRecvData, dwStateSize);
        }
        break;
    }

    case PCILINK_SEND_PANORAMIC_IMAGE:
    {
        HRESULT hr = E_FAIL;
        if(cPanormicImage.pImageData != NULL)
        {
            delete[] cPanormicImage.pImageData;
            cPanormicImage.pImageData = NULL;
        }
        hr = g_cHvPciLinkApi.RecvData(nPciHandle, &cPanormicImage.cImageHeader, sizeof(PANORAMIC_CAPTURER_FRAME_INFO));
        if(hr == S_OK && cPanormicImage.cImageHeader.dwLen > 0)
        {
            cPanormicImage.pImageData = new BYTE8[cPanormicImage.cImageHeader.dwLen];
            if(cPanormicImage.pImageData)
            {
                hr = g_cHvPciLinkApi.RecvData(nPciHandle, cPanormicImage.pImageData, cPanormicImage.cImageHeader.dwLen);
                if(hr == S_OK && m_cPciParam.pOuterControler)
                {
                    SIGNAL_INFO tempSignalInfo;
                    tempSignalInfo.dwSignalTime = GetSystemTick();
                    tempSignalInfo.dwValue = 0;
                    tempSignalInfo.nType = 1200;
                    tempSignalInfo.dwFlag = 0;
                    tempSignalInfo.iModifyRoad = 0;
                    IReferenceComponentImage* pRefImage;
                    hr = CreateReferenceComponentImage(&pRefImage,
                                HV_IMAGE_JPEG, cPanormicImage.cImageHeader.wWidth,
                                cPanormicImage.cImageHeader.wHeight, 0,
                                tempSignalInfo.dwSignalTime, 0, "NULL", 2);
                    if(hr == S_OK)
                    {
                        pRefImage->SetCaptureFlag(TRUE);
                        HV_COMPONENT_IMAGE imgFrame;
                        pRefImage->GetImage(&imgFrame);
                        memcpy(GetHvImageData(&imgFrame, 0), cPanormicImage.pImageData, cPanormicImage.cImageHeader.dwLen);
                        imgFrame.iWidth = cPanormicImage.cImageHeader.dwLen;
                        imgFrame.iHeight = cPanormicImage.cImageHeader.wWidth | (cPanormicImage.cImageHeader.wHeight << 16);
                        pRefImage->SetImageSize(imgFrame);
                        tempSignalInfo.pImage = NULL;
                        tempSignalInfo.pImageLast = pRefImage;
                        tempSignalInfo.dwInputTime = GetSystemTick();
                        if(((COuterControlImpl*)m_cPciParam.pOuterControler)->AddPanoramicSignal(&tempSignalInfo) && tempSignalInfo.pImageLast != NULL)
                        {
                            tempSignalInfo.pImageLast->Release();
                            tempSignalInfo.pImageLast = NULL;
                        }
                    }
                }
                delete[] cPanormicImage.pImageData;
                cPanormicImage.pImageData = NULL;
            }
        }
        g_cHvPciLinkApi.WriteReturnValue(nPciHandle, &hr, sizeof(hr));
        break;
     }

    case PCILINK_MASTER_CPU_STATE:
    {
        MASTER_CPU_STATUS cMasterCpuStatus;
        g_cHvPciLinkApi.RecvData(nPciHandle, &cMasterCpuStatus, sizeof(MASTER_CPU_STATUS));
        break;
    }

    default:
    {
        HV_Trace(1, "Unknowed command..\n");
    }
    }

    // 结束接收
    g_cHvPciLinkApi.EndRecvData(nPciHandle, cCmdHeadInfo);

    if (cCmdHeadInfo.emDataType == PCILINK_HV_EXIT)
    {
        HV_Sleep(1000);
        exit(iHvExitCode);
    }

    return hResult;
}

HRESULT CPciCmdProcessSlave::PutOneCmd(int nPciHandle)
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

HRESULT CPciCmdProcessSlave::GetOneCmd(int& nPciHandle)
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

HRESULT CPciCmdProcessSlave::IsThreadOk()
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
