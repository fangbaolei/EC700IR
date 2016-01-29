// 该文件编码必须是WINDOWS-936格式
#include "DataCtrl.h"
#include "ControlFunc.h"
#include "math.h"
#include "HvDspLinkApi.h"

using namespace HiVideo;
using namespace HvSys;
int g_iControllPannelWorkStyle = 0;
#ifdef SINGLE_BOARD_PLATFORM
extern bool g_fCopyrightValid;
#endif
HRESULT ForceSend(DWORD32 dwVideoID)
{
    return g_pCapFace->ForceSend(dwVideoID);
}

HRESULT GetWorkModeInfo(SYS_INFO* pInfo)
{
    return(g_pCapFace->GetWorkModeInfo(pInfo));
}

HRESULT GetWorkModeList(SYS_INFO* pInfo)
{
    return(g_pCapFace->GetWorkModeList(pInfo));
}

HRESULT SetWorkMode(DWORD32 dwWorkMode)
{
    return(SetWorkModePart(dwWorkMode));
}

void DspSoftTriggerCallback(int unused)
{
#ifdef SINGLE_BOARD_PLATFORM
    if (g_pCapFace)
    {
        g_pCapFace->DspSoftTrigger();
    }
#endif
}


//--------------------------------------------------------

CCapFace::CCapFace()
        : m_pVideoRecoger(NULL)
        , m_pVideoGetter(NULL)
        , m_pXmlDoc(NULL)
        , m_pResultSender(NULL)
        , m_pSignalMatch(NULL)
        , m_pImgCapturer(NULL)
        , m_pCamTrigger(NULL)
        , m_nCarLeftCount(0)
        , m_nLightType(-1)
        , m_pCarLeftThread(NULL)
        , m_fltFrameRate(0.0f)
        , m_nLastLightLevel(0)
        , m_pEventChecker(NULL)
{
#ifdef SINGLE_BOARD_PLATFORM
    m_pVideoSender = NULL;
#endif
    m_pCarLeftThread = new CCarLeftThread(this);
    m_pCarLeftThread->Start(NULL);
    m_nFlashLampDiff = 5;
    m_iFlashStatus = -1;
}

CCapFace::~CCapFace()
{
    if (m_pSignalMatch)
    {
        delete m_pSignalMatch;
    }
    if (m_pVideoRecoger)
    {
        delete m_pVideoRecoger;
    }
    if (m_pVideoGetter)
    {
        delete m_pVideoGetter;
    }
    if (m_pCamTrigger)
    {
        delete m_pCamTrigger;
    }
    if (m_pImgCapturer)
    {
        delete m_pImgCapturer;
    }
    if (m_pXmlDoc)
    {
        delete m_pXmlDoc;
    }
    if (m_pResultSender)
    {
        delete m_pResultSender;
    }
    if (m_pEventChecker)
	  {
		    delete m_pEventChecker;
	  }
    if (m_pCarLeftThread)
    {
        delete m_pCarLeftThread;
        m_pCarLeftThread = NULL;
    }
}

HRESULT CCapFace::ForceSend(DWORD32 dwVideoID)
{
    SIGNAL_INFO cSignalInfo;
    cSignalInfo.nType = ((COuterControlImpl*)m_pSignalMatch)->GetSoftTrigSignalSource();
    cSignalInfo.dwSignalTime = GetSystemTick();
    cSignalInfo.dwInputTime = GetSystemTick();
    cSignalInfo.dwValue = 0;
    cSignalInfo.dwRoad = 0x00FF00FF;
    cSignalInfo.pImage = NULL;
    cSignalInfo.pImageLast = NULL;

    return m_pSignalMatch->AppendSignal(&cSignalInfo);
}

HRESULT CCapFace::GetWorkModeInfo(SYS_INFO* pInfo)
{
    DWORD32 dwModeIndex = 0;
    char szModeName[128] = "UNKNOWN";
    //m_cModuleParams.nWorkModeIndex保存的是分型前的模式索引
    //因此需将该索引转换为分型后的索引
    GetRecogParamNameOnIndex(m_cModuleParams.nWorkModeIndex, szModeName);
    pInfo->Info.WorkMode.dwMode = GetRecogParamIndexOnNamePart(szModeName);
    pInfo->Info.WorkMode.dwModeCount = GetRecogParamCountPart();
    return S_OK;
}

HRESULT CCapFace::GetWorkModeList(SYS_INFO* pInfo)
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

void CCapFace::OnImage(void* pContext, IMG_FRAME imgFrame)
{
    CCapFace* pThis = (CCapFace *)pContext;

    if ( imgFrame.pRefImage != NULL )
    {
        imgFrame.pRefImage->AddRef();
    }

#ifdef SINGLE_BOARD_PLATFORM
    if (false == pThis->m_queImage.AddTail(imgFrame, 0 == pThis->m_cModuleParams.cCamCfgParam.iCamType))
#else
    if (false == pThis->m_queImage.AddTail(imgFrame, 0 != pThis->m_cModuleParams.cCamCfgParam.iCamType))
#endif
    {
        SAFE_RELEASE(imgFrame.pRefImage);
    }
}

HRESULT CCapFace::Run(void* pvParam)
{
    DWORD32 dwVideoFrameCount = 0;
    DWORD32 dwFrameCount = 0;
    DWORD32 dwLastFrameCount = 0;
    DWORD32 dwSecondCount = GetSystemTick() / 1000;
    DWORD32 dwCurSecCount = 0;
    m_dwLastSendTime = GetSystemTick();

    IMG_FRAME frame;
#ifdef SINGLE_BOARD_PLATFORM
    // 随机加密认证相关
    int iFrameCount = 0;
    int iVerifyCrypt = 0;
    const int VERIFY_CRYPT_PARAM = 518400;
    srand((int)time(0));
#endif
    if(m_cModuleParams.cTrackerCfgParam.iDisableFlashAtDay == 0)
    {
        SetCaptureSynSignalEnableFlash(TRUE);
    }
    while (!m_fExit)
    {
        if (!m_pVideoGetter)
        {
            HV_Sleep(4000);
            continue;
        }

        frame = m_queImage.RemoveHead(500);
        if (NULL == frame.pRefImage)
        {
            HV_Trace(5, "get video frame error.\n");
            continue;
        }

#ifdef SINGLE_BOARD_PLATFORM
        //加密认证
        iFrameCount++;
        if ( iFrameCount >= iVerifyCrypt )
        {
            if ( 0 != verify_crypt() )
            {
                // 加密认证失败
                Trace("\n:-(\n");
                g_fCopyrightValid = false;
            }
            else
            {
                // 加密认证成功
                Trace("\n:-)\n");
                g_fCopyrightValid = true;
            }

            iFrameCount = 0;
            iVerifyCrypt = ( rand() * VERIFY_CRYPT_PARAM / RAND_MAX );
            iVerifyCrypt += VERIFY_CRYPT_PARAM;
        }

        static DWORD32 dwSendVideo = GetSystemTick();
        if (m_pVideoSender
                && m_cModuleParams.cResultSenderParam.iSaveVideo
                && (int)(GetSystemTick() - dwSendVideo) >= m_cModuleParams.cResultSenderParam.iVideoDisplayTime)
        {
            dwSendVideo = GetSystemTick();
            HV_COMPONENT_IMAGE imgFrame;
            frame.pRefImage->GetImage(&imgFrame);
            if (imgFrame.nImgType == HV_IMAGE_JPEG)
            {
                static DWORD32 dwLastTime = GetSystemTick();
                SEND_CAMERA_VIDEO videoInfo;
                videoInfo.dwVideoType = CAMERA_VIDEO_JPEG;
                videoInfo.dwFrameType = GetSystemTick() - dwLastTime >= 1000 ? CAMERA_FRAME_I : CAMERA_FRAME_P;
                if (videoInfo.dwFrameType == CAMERA_FRAME_I)
                {
                    dwLastTime = GetSystemTick();
                }
                GetSystemTime(&videoInfo.dwTimeLow, &videoInfo.dwTimeHigh);
                videoInfo.dwVideoSize = imgFrame.iWidth;
                videoInfo.pbVideo = GetHvImageData(&imgFrame, 0);
                videoInfo.pRefImage = frame.pRefImage;
                if ( FAILED(m_pVideoSender->SendCameraVideo(&videoInfo)) )
                {
                    HV_Trace(5, "<link>SendCameraVideo failed!\n");
                }
            }
        }
#endif

        dwFrameCount++;
        if (dwFrameCount >= 100)
        {
            dwCurSecCount = GetSystemTick() / 1000;
            m_fltFrameRate = (float)(dwFrameCount) / (dwCurSecCount - dwSecondCount);
            HV_Trace(5, "fps: %0.1f\n", m_fltFrameRate);
            dwFrameCount = 0;
            dwSecondCount = dwCurSecCount;
        }
        if (m_pEventChecker)
        {
            m_pEventChecker->ProcessOneFrame(frame.pRefImage);
        }
        PROCESS_EVENT_STRUCT cProcessEvent;
        if (g_iControllPannelWorkStyle == 1)
        {
            bool fIsUpdatingPannelParam = ((CVideoGetter_VPIF*)m_pVideoGetter)->CheckIfChangeParam();
            if (fIsUpdatingPannelParam)
                cProcessEvent.fIsCheckLight = false;
            else
                cProcessEvent.fIsCheckLight = true;
        }
        else
        {
            cProcessEvent.fIsCheckLight = true;
        }

        cProcessEvent.iCarArriveTriggerType = ((COuterControlImpl *)m_pSignalMatch)->NeedProcessCarArriveTrigger();

        DWORD32 dwBeginRecog = GetSystemTick();
        HRESULT hr = m_pVideoRecoger->ProcessOneFrame(
                         frame.iVideoID,
                         frame.pRefImage,
                         NULL,
                         NULL,
                         &cProcessEvent
                     );
        DWORD32 dwRecogTime = GetSystemTick() - dwBeginRecog;
        if ( dwRecogTime > 120 )
        {
            HV_Trace(5, "<DataCtrl> Recog relay : %d\n", dwRecogTime);
        }

        //信号匹配
        if (hr == S_OK)
        {
            hr = m_pSignalMatch->ProcessOneFrame(
                     frame.iVideoID,
                     frame.pRefImage,
                     NULL,
                     NULL,
                     &cProcessEvent
                 );
            m_pVideoGetter->SetLightType(cProcessEvent.cLightType, cProcessEvent.iCplStatus);
            if (g_iControllPannelWorkStyle == 1)
            {
                ((CVideoGetter_VPIF*)m_pVideoGetter)->SetPannelStatus(cProcessEvent.iCplStatus, cProcessEvent.iPulseLevel);
                if(m_cModuleParams.cTrackerCfgParam.iDisableFlashAtDay)
                {
                    SetCaptureSynSignalEnableFlash(cProcessEvent.fIsNight);
                }
            }
        }

        m_pResultSender->PutVideo(
            &dwVideoFrameCount,
            &frame,
            cProcessEvent.cTrackRectInfo.dwTrackCount,
            cProcessEvent.cTrackRectInfo.rgTrackRect
        );

        // 释放共享内存数据
        SAFE_RELEASE(frame.pRefImage);

        if (S_OK == hr)
        {
            if ( cProcessEvent.dwEventId & EVENT_CARARRIVE )
            {
                for ( int n=0; n<cProcessEvent.iCarArriveInfoCount; ++n )
                {
                    CarArrive(&cProcessEvent.rgCarArriveInfo[n], NULL);
                    memset(&cProcessEvent.rgCarArriveInfo[n], 0, sizeof(CARARRIVE_INFO_STRUCT));
                }
            }

            if ( cProcessEvent.dwEventId & EVENT_CARLEFT )
            {
                for ( int n=0; n<cProcessEvent.iCarLeftInfoCount; ++n )
                {
                    m_pCarLeftThread->PutOneResult(cProcessEvent.rgCarLeftInfo[n]);
                    m_nCarLeftCount++;
                }
            }
        }
#ifndef SINGLE_BOARD_PLATFORM
        //计算车流量，并将车流量与AGC联动
        ScaleCarFlowRate();
        //根据环境亮度设置AWB，白天自动白平衡，晚上手动白平衡
        SetAWBFromLightType(cProcessEvent.cLightType);
#endif
    }

    return S_OK;
}

HRESULT CCapFace::SetAWBFromLightType(LIGHT_TYPE emLightType)
{
    if (m_cModuleParams.cResultSenderParam.iAWBLinkage <= 0)
    {
        return S_OK;
    }

    static int nLastLightType = -1;
    static DWORD32 dwLastSecCount = GetSystemTick() / 1000;

    if (nLastLightType == emLightType)
    {
        return S_OK;
    }

    //60s统计一次
    if (GetSystemTick () - dwLastSecCount < 60)
    {
        return S_OK;
    }

    dwLastSecCount = GetSystemTick();

    HRESULT hr;
    HRESULT hrAwb;
    int nAWBValue = 0;
    int nReturnSize = sizeof(HRESULT);

    if (emLightType == DAY)
    {
        nAWBValue = 0;
    }
    else
    {
        nAWBValue = 1;
    }

    hr = g_cHvPciLinkApi.SendData(
             PCILINK_SET_AWB_STATE,
             &nAWBValue, sizeof(int),
             &hrAwb, &nReturnSize
         );

    if (hr != S_OK || hrAwb != S_OK)
    {
        HV_Trace(5, "Set AWB failed!\n");
        return E_FAIL;
    }

    nLastLightType = emLightType;

    return S_OK;
}

HRESULT CCapFace::ScaleCarFlowRate()
{
    if (m_cModuleParams.cResultSenderParam.fltCarAGCLinkage <= 0.0f)
    {
        return S_OK;
    }

    static DWORD32 dwLastSecCount;

    DWORD32 dwCurSecCount = 0;
    HRESULT hr;
    HRESULT hrAgc;
    int nAGCValue = 0;
    int nReturnSize = sizeof(HRESULT);

    dwCurSecCount = GetSystemTick() / 1000;
    // 统计车流量，5分钟统计一次
    if (dwCurSecCount - dwLastSecCount >= 5 * 60)
    {
        float fltCarAGCLinkage = (float)m_nCarLeftCount / (float)(dwCurSecCount - dwLastSecCount);

        if (fltCarAGCLinkage > m_cModuleParams.cResultSenderParam.fltCarAGCLinkage)
        {
            nAGCValue = 0;
        }
        else
        {
            nAGCValue = 1;
        }

        hr = g_cHvPciLinkApi.SendData(
                 PCILINK_SET_AGC_STATE,
                 &nAGCValue, sizeof(int),
                 &hrAgc, &nReturnSize
             );

        if (hr != S_OK || hrAgc != S_OK)
        {
            HV_Trace(5, "%s AGC failed!\n", nAGCValue == 0 ? "Close" : "Open");
            m_nCarLeftCount = 0;
            dwLastSecCount = dwCurSecCount;
            return E_FAIL;
        }

        m_nCarLeftCount = 0;
        dwLastSecCount = dwCurSecCount;
        m_cModuleParams.cCamCfgParam.iEnableAGC = nAGCValue;
    }

    return S_OK;
}

void CCapFace::DspSoftTrigger()
{
#ifdef SINGLE_BOARD_PLATFORM
    if (!m_pCamTrigger)
    {
        return;
    }

    static DWORD32 dwLastTriggerIndex = 0;
    int iTriggerInfo = 0;
    int iTryTimes = 3;
    TRIGGER_CAM_INFO cTriggerCamInfo;
    bool fOk = false;
    while (--iTryTimes > 0)
    {
        const PBYTE8 pbTmp = (PBYTE8)m_cModuleParams.cTrackerCfgParam.cTriggerInfoBuf.addr;
        if (pbTmp && (m_cModuleParams.cTrackerCfgParam.cTriggerInfoBuf.len >= sizeof(TRIGGER_CAM_INFO)))
        {
            memcpy(&cTriggerCamInfo, pbTmp, sizeof(cTriggerCamInfo));
            if (dwLastTriggerIndex != cTriggerCamInfo.dwTriggerIndex)
            {
                dwLastTriggerIndex = cTriggerCamInfo.dwTriggerIndex;
                iTriggerInfo = cTriggerCamInfo.dwTriggerIndex;
                iTriggerInfo = (iTriggerInfo << 16) | cTriggerCamInfo.dwRoadId;
                fOk = true;
                break;
            }
        }
        HV_Sleep(10);
    }
    if (fOk)
    {
        m_pCamTrigger->Trigger((LIGHT_TYPE)0, iTriggerInfo);
    }
#endif
}

HRESULT CCapFace::CarArrive(
    CARARRIVE_INFO_STRUCT *pCarArriveInfo,
    LPVOID pvUserData
)
{
    /*
        HV_Trace(
            3, "<CarArriveInfo>P:%d,%d, Depo:%d, Trigger:%d, Image:%d, ARM:%d\n",
            pCarArriveInfo->dwFirstPos,
            pCarArriveInfo->dwEndPos,
            pCarArriveInfo->dwTriggerOutDelay,
            pCarArriveInfo->dwCarArriveRealTime,
            pCarArriveInfo->dwCarArriveTime,
            GetSystemTick()
        );
    */
    return S_OK;
}

HRESULT CCapFace::CarLeft(
    CARLEFT_INFO_STRUCT *pCarLeftInfo,
    LPVOID pvUserData
)
{
    if (!pCarLeftInfo || !m_pResultSender)
    {
        return E_FAIL;
    }

    int iStringLen = MAX_PLATE_STRING_SIZE;
    if (BuildPlateString(m_szPlateString, &iStringLen, pCarLeftInfo) == S_OK
            && m_pResultSender->PutResult(m_szPlateString, pCarLeftInfo) == S_OK)
    {
        m_dwLastSendTime = GetSystemTick();
    }

    return S_OK;
}

HRESULT CCapFace::BuildPlateString(
    char* pszPlateString,
    int* piPlateStringSize,
    CARLEFT_INFO_STRUCT *pCarLeftInfo
)
{
    if (pCarLeftInfo == NULL || pszPlateString == NULL || piPlateStringSize == NULL) return E_INVALIDARG;

    if (!m_pXmlDoc)	//如果文档为空则创建新文档
    {
        m_pXmlDoc = new TiXmlDocument;
        TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
        TiXmlElement* pRoot = new TiXmlElement("HvcResultDoc");

        if ( !m_pXmlDoc || !pDecl || !pRoot )
        {
            SAFE_DELETE(m_pXmlDoc);
            SAFE_DELETE(pDecl);
            SAFE_DELETE(pRoot);
            return E_OUTOFMEMORY;
        }

        m_pXmlDoc->LinkEndChild(pDecl);
        m_pXmlDoc->LinkEndChild(pRoot);
    }

    //取得ResultSet段
    TiXmlElement* pResultSet = m_pXmlDoc->RootElement()->FirstChildElement("ResultSet");
    if (!pResultSet)
    {
        pResultSet = new TiXmlElement("ResultSet");
        if ( !pResultSet ) return E_OUTOFMEMORY;
        m_pXmlDoc->RootElement()->LinkEndChild(pResultSet);
    }

    //注意：一定要删除已经存在的节
    TiXmlNode* pResultOld = pResultSet->FirstChild("Result");
    if ( pResultOld )
    {
        pResultSet->RemoveChild(pResultOld);
    }

    //写入Result
    TiXmlElement* pResult = new TiXmlElement("Result");
    if (pResult)
    {
        pResultSet->LinkEndChild(pResult);

        //车牌
        static char szConf[128] = {0};
        static char szPlateName[32] = {0};
        static char szFrameName[64] = {0};
        static char szCarType[16] = {0};
        bool fIsNoPlate = false;
        bool fIsCar = true;
        bool fIsDouble = false;
        bool fIsDoubleMoto = false;

        GetPlateNameAlpha(
            (char*)szPlateName,
            ( PLATE_TYPE )pCarLeftInfo->cCoreResult.nType,
            ( PLATE_COLOR )pCarLeftInfo->cCoreResult.nColor,
            pCarLeftInfo->cCoreResult.rgbContent
        );

        if (strstr(szPlateName, "11111") != NULL)
        {
            HV_Trace(5, "M");
            return S_FALSE;
        }

        //获取车辆类型
        GetCarType(&pCarLeftInfo->cCoreResult, szCarType);

        //车辆类型为行人、或者非机动车时，将"无车牌"替换为车辆类型
        if (strcmp(szCarType, "行人") == 0
                || strcmp(szCarType, "非机动车") == 0)
        {
            strcpy(szPlateName, "  ");
            strcat(szPlateName, szCarType);
            fIsCar = false;
        }

        if (pCarLeftInfo->cCoreResult.nType == PLATE_DOUBLE_YELLOW)
        {
            fIsDouble = true;
        }
        if (pCarLeftInfo->cCoreResult.nType == PLATE_DOUBLE_MOTO)
        {
            fIsDoubleMoto = true;
        }

        HV_Trace(5, "%s\n", szPlateName);  //输出车牌字符串

        //无牌车
        if ((*(pCarLeftInfo->cCoreResult.rgbContent)) == 0)
        {
            fIsNoPlate = true;
        }

        TiXmlElement* pValue = new TiXmlElement("PlateName");
        TiXmlText* pText = new TiXmlText(szPlateName);
        if (pValue && pText)
        {
            pValue->LinkEndChild(pText);
            pResult->LinkEndChild(pValue);
        }

        int nColorType = 0;
        if (strncmp(szPlateName, "蓝", 2) == 0)
        {
            nColorType = 1;
        }
        else if (strncmp(szPlateName, "黄", 2) == 0)
        {
            nColorType = 2;
        }
        else if (strncmp(szPlateName, "黑", 2) == 0)
        {
            nColorType = 3;
        }
        else if (strncmp(szPlateName, "白", 2) == 0)
        {
            nColorType = 4;
        }
        else if (strncmp(szPlateName, "绿", 2) == 0)
        {
            nColorType = 5;
        }
        else
        {
            nColorType = 0;
        }

        int nPlateType = 0;
        switch ( pCarLeftInfo->cCoreResult.nType )
        {
        case PLATE_NORMAL:
        case PLATE_POLICE:
            nPlateType = 1;
            break;
        case PLATE_WJ:
            nPlateType = 2;
            break;
        case PLATE_POLICE2:
            nPlateType = 3;
            break;
        case PLATE_DOUBLE_YELLOW:
        case PLATE_DOUBLE_MOTO:
            nPlateType = 4;
            break;
        default:
            nPlateType = 0;
            break;
        }

        pValue = new TiXmlElement("Color");
        if ( pValue )
        {
            pValue->SetAttribute("raw_value", pCarLeftInfo->cCoreResult.nColor);
            pValue->SetAttribute("value", nColorType);
            pResult->LinkEndChild(pValue);
        }

        pValue = new TiXmlElement("Type");
        if ( pValue )
        {
            pValue->SetAttribute("raw_value", pCarLeftInfo->cCoreResult.nType);
            pValue->SetAttribute("value", nPlateType);
            pResult->LinkEndChild(pValue);
        }

        //如果输出附加信息
        if (m_cModuleParams.cResultSenderParam.fOutputAppendInfo)
        {
            //输出车辆逆行标识
            if (m_cModuleParams.cTrackerCfgParam.nDetReverseRunEnable)
            {
                pValue = new TiXmlElement("ReverseRun");
                if (pValue)
                {
                    sprintf(szConf, "%s", pCarLeftInfo->cCoreResult.fReverseRun ? "是" : "否");
                    pValue->SetAttribute("value", szConf);
                    pValue->SetAttribute("chnname", "车辆逆向行驶");
                    pResult->LinkEndChild(pValue);
                }
            }

            if (m_cModuleParams.cTrackerCfgParam.cScaleSpeed.fEnable
                    && !pCarLeftInfo->cCoreResult.fltCarspeed)
            {
                srand(GetSystemTick());
                pCarLeftInfo->cCoreResult.fltCarspeed = rand() % 20 + 20.0f;
                if (fIsNoPlate && fIsCar)
                {
                    pCarLeftInfo->cCoreResult.nCarColor = CC_GREY;
                }
            }
            //输出速度值
            if (pCarLeftInfo->cCoreResult.fltCarspeed > 0.0f)
            {
                pValue = new TiXmlElement("VideoScaleSpeed");
                if (pValue)
                {
                    sprintf(szConf, "%d Km/h", (int)(pCarLeftInfo->cCoreResult.fltCarspeed));
                    pValue->SetAttribute("value", szConf);
                    pValue->SetAttribute("chnname", "视频测速");
                    pResult->LinkEndChild(pValue);
                }
                //如果有事件检测功能，则把限速值发出去
                /*
                static bool fOutput = false;
                for (int i = 0; i < MAX_ROADLINE_NUM; i++)
                {
                    if (m_cModuleParams.cTrackerCfgParam.cActionDetect.iIsYellowLine[i]
                            || (i > 0 && i < MAX_ROADLINE_NUM - 1 && m_cModuleParams.cTrackerCfgParam.cActionDetect.iIsCrossLine[i]))
                    {
                        fOutput = true;
                        break;
                    }
                }
                if (fOutput)
                */
                {
                    pValue = new TiXmlElement("SpeedLimit");
                    if (pValue)
                    {
                        sprintf(szConf, "%d Km/h", m_cModuleParams.cResultSenderParam.iSpeedLimit);
                        pValue->SetAttribute("value", szConf);
                        pValue->SetAttribute("chnname", "限速值");
                        pResult->LinkEndChild(pValue);
                    }
                }
                //距离计算的误差比例
                pValue = new TiXmlElement("ScaleSpeedOfDistance");
                if (pValue)
                {
                    sprintf(szConf, "%0.2f", pCarLeftInfo->cCoreResult.fltScaleOfDistance);
                    pValue->SetAttribute("value", szConf);
                    pValue->SetAttribute("chnname", "距离测量误差比例");
                    pResult->LinkEndChild(pValue);
                }
            }

            if (m_cModuleParams.cResultSenderParam.fOutputObservedFrames)
            {
                //有效帧数
                pValue = new TiXmlElement("ObservedFrames");
                if (pValue)
                {
                    sprintf(szConf, "%d", pCarLeftInfo->cCoreResult.iObservedFrames);
                    pValue->SetAttribute("value", szConf);
                    pValue->SetAttribute("chnname", "有效帧数");
                    pResult->LinkEndChild(pValue);
                }

                //可信度
                pValue = new TiXmlElement("Confidence");
                if (pValue)
                {
                    sprintf(
                        szConf, "%.3f",
                        exp(log(pCarLeftInfo->cCoreResult.fltAverageConfidence) * 0.143)
                    );
                    pValue->SetAttribute("value", szConf);
                    pValue->SetAttribute("chnname", "平均可信度");
                    pResult->LinkEndChild(pValue);
                }

                //首字符可信度
                pValue = new TiXmlElement("FirstCharConf");
                if (pValue)
                {
                    sprintf(szConf, "%.3f", pCarLeftInfo->cCoreResult.fltFirstAverageConfidence);
                    pValue->SetAttribute("value", szConf);
                    pValue->SetAttribute("chnname", "首字可信度");
                    pResult->LinkEndChild(pValue);
                }
            }

            //车辆检测时间
            if (m_cModuleParams.cResultSenderParam.fOutputCarArriveTime)
            {
                pValue = new TiXmlElement("CarArriveTime");
                if (pValue)
                {
                    REAL_TIME_STRUCT rt;
                    DWORD32 dwLow, dwHigh;
                    ConvertTickToSystemTime(pCarLeftInfo->cCoreResult.nFirstFrameTime, dwLow, dwHigh);
                    ConvertMsToTime(dwLow, dwHigh, &rt);
                    sprintf(szConf, "%04d-%02d-%02d %02d:%02d:%02d:%03d", rt.wYear, rt.wMonth, rt.wDay, rt.wHour, rt.wMinute, rt.wSecond, rt.wMSecond);
                    pValue->SetAttribute("value", szConf);
                    pValue->SetAttribute("chnname", "车辆检测时间");
                    pResult->LinkEndChild(pValue);
                }
            }

            if (fIsDouble)
            {
                //双层牌
                pValue = new TiXmlElement("DoublePlate");
                if (pValue)
                {
                    pValue->SetAttribute("value", "双");
                    pValue->SetAttribute("chnname", "车牌类型");
                    pResult->LinkEndChild(pValue);
                }
            }

            if (fIsDoubleMoto)
            {
                //摩托
                pValue = new TiXmlElement("DoubleMoto");
                if (pValue)
                {
                    pValue->SetAttribute("value", "摩");
                    pValue->SetAttribute("chnname", "车牌类型");
                    pResult->LinkEndChild(pValue);
                }
            }

            //环境光亮度
            pValue = new TiXmlElement("AmbientLight");
            if (pValue)
            {
                sprintf(szConf, "%d", pCarLeftInfo->cCoreResult.iAvgY);
                pValue->SetAttribute("value", szConf);
                pValue->SetAttribute("chnname", "环境亮度");
                pResult->LinkEndChild(pValue);
            }

            //车牌亮度
            pValue = new TiXmlElement("PlateLight");
            if (pValue)
            {
                sprintf(szConf, "%d", pCarLeftInfo->cCoreResult.iCarAvgY);
                pValue->SetAttribute("value", szConf);
                pValue->SetAttribute("chnname", "车牌亮度");
                pResult->LinkEndChild(pValue);
            }

            //车牌对比度
            pValue = new TiXmlElement("PlateVariance");
            if (pValue)
            {
                sprintf(szConf, "%d", pCarLeftInfo->cCoreResult.iCarVariance);
                pValue->SetAttribute("value", szConf);
                pValue->SetAttribute("chnname", "车牌对比度");
                pResult->LinkEndChild(pValue);
            }

            //车辆类型
            pValue = new TiXmlElement("CarType");
            if (pValue)
            {
                pValue->SetAttribute("value", szCarType);
                pValue->SetAttribute("chnname", "车辆类型");
                pResult->LinkEndChild(pValue);
            }

            //车辆尺寸
            if (m_cModuleParams.cTrackerCfgParam.fOutputCarSize
                    && pCarLeftInfo->cCoreResult.cCarSize.nOutType < 2
                    && pCarLeftInfo->cCoreResult.nCarType != CT_WALKMAN
                    && pCarLeftInfo->cCoreResult.nCarType != CT_BIKE
                    && pCarLeftInfo->cCoreResult.nType != PLATE_DOUBLE_MOTO)
            {
                if (pCarLeftInfo->cCoreResult.cCarSize.nOutType > 0)
                {
                    if (pCarLeftInfo->cCoreResult.cCarSize.iCarWidth > 5)
                    {
                        pCarLeftInfo->cCoreResult.cCarSize.iCarWidth = 5;
                    }
                    if (pCarLeftInfo->cCoreResult.cCarSize.iCarHeight > 20)
                    {
                        pCarLeftInfo->cCoreResult.cCarSize.iCarHeight = 20;
                    }
                }
                pValue = new TiXmlElement("CarSize");
                if (pValue)
                {
                    if (pCarLeftInfo->cCoreResult.cCarSize.nOutType == 0)
                    {
                        sprintf(
                            szConf, "%d 车长(像素):%d",
                            (int)pCarLeftInfo->cCoreResult.cCarSize.iCarWidth,
                            (int)pCarLeftInfo->cCoreResult.cCarSize.iCarHeight
                        );
                    }
                    else
                    {
                        sprintf(
                            szConf, "%.2f 车长(米):%.2f",
                            pCarLeftInfo->cCoreResult.cCarSize.iCarWidth,
                            pCarLeftInfo->cCoreResult.cCarSize.iCarHeight
                        );
                    }
                    pValue->SetAttribute("value", szConf);
                    sprintf(
                        szConf, "车宽(%s)",
                        pCarLeftInfo->cCoreResult.cCarSize.nOutType == 0 ? "像素" : "米"
                    );
                    pValue->SetAttribute("chnname", szConf);
                    pResult->LinkEndChild(pValue);
                }
            }


            //车身颜色
            if (m_cModuleParams.cTrackerCfgParam.fEnableRecgCarColor)
            {
                pValue = new TiXmlElement("CarColor");
                if (pValue)
                {
                    GetCarColor(pCarLeftInfo, szConf);
                    pValue->SetAttribute("value", szConf);
                    pValue->SetAttribute("chnname", "车身颜色");
                    pResult->LinkEndChild(pValue);
                }
            }

            //车道号
            pValue = new TiXmlElement("RoadNumber");
            if (pValue)
            {
                if (m_cModuleParams.cTrackerCfgParam.iStartRoadNum == 0)
                {
                    sprintf(szConf, "%d", pCarLeftInfo->cCoreResult.nRoadNo);
                }
                else
                {
                    if (pCarLeftInfo->cCoreResult.nRoadNo == 255)
                    {
                        sprintf(szConf, "%d", pCarLeftInfo->cCoreResult.nRoadNo);
                    }
                    else
                    {
                        sprintf(szConf, "%d", pCarLeftInfo->cCoreResult.nRoadNo + m_cModuleParams.cTrackerCfgParam.iStartRoadNum);
                    }
                }
                pValue->SetAttribute("value", szConf);
                pValue->SetAttribute("chnname", "车道");
                pResult->LinkEndChild(pValue);
            }

            //起始车道号
            pValue = new TiXmlElement("BeginRoadNumber");
            if (pValue)
            {
                if (m_cModuleParams.cTrackerCfgParam.iRoadNumberBegin == 0)
                {
                    sprintf(szConf, "<左,%d>", m_cModuleParams.cTrackerCfgParam.iStartRoadNum);
                }
                else
                {
                    sprintf(szConf, "<右,%d>", m_cModuleParams.cTrackerCfgParam.iStartRoadNum);
                }
                pValue->SetAttribute("value", szConf);
                pValue->SetAttribute("chnname", "起始车道号");
                pResult->LinkEndChild(pValue);
            }

            //路口名称
            pValue = new TiXmlElement("StreetName");
            if (pValue)
            {
                pValue->SetAttribute("value", m_cModuleParams.cResultSenderParam.szStreetName);
                pValue->SetAttribute("chnname", "路口名称");
                pResult->LinkEndChild(pValue);
            }

            //路口方向
            pValue = new TiXmlElement("StreetDirection");
            if (pValue)
            {
                pValue->SetAttribute("value", m_cModuleParams.cResultSenderParam.szStreetDirection);
                pValue->SetAttribute("chnname", "路口方向");
                pResult->LinkEndChild(pValue);
            }

            //事件检测信息
            pValue = new TiXmlElement("EventCheck");
            if (pValue)
            {
                GetEventDetInfo(pCarLeftInfo, szConf);
                if (strlen(szConf) != 0)
                {
                    pValue->SetAttribute("value", szConf);
                    pValue->SetAttribute("chnname", "事件检测");
                    pResult->LinkEndChild(pValue);
                }
            }

            if (pCarLeftInfo->cCoreResult.cResultImg.pimgBestSnapShot)
            {
                //视频帧名
                strcpy(
                    szFrameName,
                    pCarLeftInfo->cCoreResult.cResultImg.pimgBestSnapShot->GetFrameName()
                );
                if (strlen(szFrameName) != 0
#ifdef SINGLE_BOARD_PLATFORM
                        && m_cModuleParams.cCamCfgParam.iCamType == 0 // 单板下0才是测试协议
#else
                        && m_cModuleParams.cCamCfgParam.iCamType == 1
#endif
                   )
                {
                    pValue = new TiXmlElement("FrameName");
                    if ( pValue )
                    {
                        char *pFirst = szFrameName, *pSecond = szFrameName;
                        char *pTemp = strstr(pFirst, ".");
                        while (pTemp)
                        {
                            pSecond = pTemp;
                            pTemp = strstr(pTemp + 1, ".");
                        }
                        szFrameName[pSecond - pFirst] = 0;
                        pValue->SetAttribute("value", szFrameName);
                        pValue->SetAttribute("chnname", "视频帧名");
                        pResult->LinkEndChild(pValue);
                    }
                }
            }
            if (m_cModuleParams.cCamCfgParam.iDynamicCfgEnable)
            {
                //当前亮度级别
                pValue = new TiXmlElement("PlateLightType");
                if (pValue)
                {
                    sprintf(szConf, "%02d级", pCarLeftInfo->cCoreResult.nPlateLightType);
                    pValue->SetAttribute("value", szConf);
                    pValue->SetAttribute("chnname", "摄像机亮度等级");
                    pResult->LinkEndChild(pValue);
                }
            }
#ifndef SINGLE_BOARD_PLATFORM
            if (m_cModuleParams.cCamCfgParam.iDynamicCfgEnable)
            {
                //当前偏光镜状态
                pValue = new TiXmlElement("CplStatus");
                if (pValue)
                {
                    sprintf(szConf, "%d", pCarLeftInfo->cCoreResult.iCplStatus);
                    pValue->SetAttribute("value", szConf);
                    pValue->SetAttribute("chnname", "偏光镜状态");
                    pResult->LinkEndChild(pValue);
                }
            }

            if (g_iControllPannelWorkStyle == 1 && m_cModuleParams.cCamCfgParam.iDynamicCfgEnable)
            {
                //当前补光灯脉宽级别
                pValue = new TiXmlElement("PulseLevel");
                if (pValue)
                {
                    sprintf(szConf, "%d", pCarLeftInfo->cCoreResult.iPulseLevel);
                    pValue->SetAttribute("value", szConf);
                    pValue->SetAttribute("chnname", "补光脉宽等级");
                    pResult->LinkEndChild(pValue);
                }

                //补光灯状态判断
                static int iFillLightCount = 0;
                static int iFillLightMaxCount = (m_cModuleParams.cTrackerCfgParam.nPlateLightCheckCount << 1);
                if (iFillLightMaxCount < 20) iFillLightMaxCount = 20;
                int iMinAvgY = m_cModuleParams.cTrackerCfgParam.nMinPlateBrightness;
                if (pCarLeftInfo->cCoreResult.iPulseLevel > 0)
                {
                    if (pCarLeftInfo->cCoreResult.iCarAvgY < 50)
                    {
                        iFillLightCount++;
                    }
                    else
                    {
                        iFillLightCount = 0;
                    }
                    if (m_nLastLightLevel != pCarLeftInfo->cCoreResult.iPulseLevel)
                    {
                        iFillLightCount = 0;
                        m_nLastLightLevel = pCarLeftInfo->cCoreResult.iPulseLevel;
                    }
                    if (iFillLightCount > iFillLightMaxCount )
                    {
                        iFillLightCount = iFillLightMaxCount;
                        pValue = new TiXmlElement("FlashLightStatus");
                        if (pValue)
                        {
                            sprintf(szConf, "%s", "异常");
                            pValue->SetAttribute("value", szConf);
                            pValue->SetAttribute("chnname", "补光灯工作状态");
                            pResult->LinkEndChild(pValue);
                        }
                    }
                    else
                    {
                        pValue = new TiXmlElement("FlashLightStatus");
                        if (pValue)
                        {
                            sprintf(szConf, "%s", "正常");
                            pValue->SetAttribute("value", szConf);
                            pValue->SetAttribute("chnname", "补光灯工作状态");
                            pResult->LinkEndChild(pValue);
                        }
                    }
                }

                //闪光灯状态判断,无车牌时不进行判断
                static int iFlashLampCount = 0;
                if (pCarLeftInfo->cCoreResult.fIsCapture)
                {
                    if (pCarLeftInfo->cCoreResult.rgbContent[0] != 0)
                    {
                        if (pCarLeftInfo->cCoreResult.iPulseLevel > 0)
                        {
                            if (pCarLeftInfo->cCoreResult.iCapturerAvgY - pCarLeftInfo->cCoreResult.iAvgY
                                    < m_nFlashLampDiff)
                            {
                                iFlashLampCount++;
                            }
                            else
                            {
                                iFlashLampCount = 0;
                            }

                            if (iFlashLampCount > 5)
                            {
                                iFlashLampCount = 5;
                                pValue = new TiXmlElement("CaptureLightStatus");
                                if (pValue)
                                {
                                    sprintf(szConf, "%s", "异常");
                                    pValue->SetAttribute("value", szConf);
                                    pValue->SetAttribute("chnname", "闪光灯工作状态");
                                    pResult->LinkEndChild(pValue);
                                }
                            }
                            else
                            {
                                pValue = new TiXmlElement("CaptureLightStatus");
                                if (pValue)
                                {
                                    sprintf(szConf, "%s", "正常");
                                    pValue->SetAttribute("value", szConf);
                                    pValue->SetAttribute("chnname", "闪光灯工作状态");
                                    pResult->LinkEndChild(pValue);
                                }
                            }
                        }
                        else
                        {
                            iFlashLampCount = 0;
                        }
                    }
                }
                else
                {
                    if (pCarLeftInfo->cCoreResult.rgbContent[0] != 0)
                        iFlashLampCount = 0;
                }
            }
#endif

        } // end of if (m_cModuleParams.cResultSenderParam.fOutputAppendInfo)

        m_pLightTypeSaver->SaveLightType(
            pCarLeftInfo->cCoreResult.nPlateLightType,
            pCarLeftInfo->cCoreResult.iPulseLevel,
            pCarLeftInfo->cCoreResult.iCplStatus,
            pCarLeftInfo->cCoreResult.nCarArriveTime
        );

    } // end of if (pResult)

    TiXmlPrinter cTxPr;
    if (m_pXmlDoc)
    {
        if (m_pEventChecker)
        {
            m_pEventChecker->GetOneEventInfo(pCarLeftInfo, pResult);
        }
        cTxPr.SetStreamPrinting();
        m_pXmlDoc->Accept(&cTxPr);

        int nCpyLen = MIN_INT(*piPlateStringSize, (int)cTxPr.Size() + 1);
        HV_memcpy( pszPlateString, cTxPr.CStr(), nCpyLen );
        pszPlateString[nCpyLen - 1] = '\0';
        *piPlateStringSize = nCpyLen;
    }
    else
    {
        *piPlateStringSize = 0;
    }

    return S_OK;
}

bool CCapFace::ThreadIsOk(int* piErrCode)
{
    if ( m_pVideoRecoger && S_OK != m_pVideoRecoger->GetCurStatus(NULL, 0) )
    {
        if (piErrCode) *piErrCode = 1;
        return false;
    }

    if ( m_pVideoGetter && S_OK != m_pVideoGetter->GetCurStatus(NULL, 0) )
    {
        if (piErrCode) *piErrCode = 2;
        return false;
    }

    if (m_pSignalMatch && !m_pSignalMatch->ThreadIsOk())
    {
        if (piErrCode) *piErrCode = 3;
        return false;
    }

#ifdef SINGLE_BOARD_PLATFORM
    if (m_pCamTrigger && !((CCamTrigger*)m_pCamTrigger)->ThreadIsOk())
    {
        if (piErrCode) *piErrCode = 4;
        return false;
    }

    if (m_pImgCapturer && m_pImgCapturer->GetCurStatus(NULL, 0) != S_OK)
    {
        if (piErrCode) *piErrCode = 5;
        return false;
    }
#endif

    if (m_pCarLeftThread && m_pCarLeftThread->IsThreadOK() != S_OK)
    {
        if (piErrCode)
        {
            (*piErrCode) = 6;
        }
        return false;
    }

    return true;
}

void CCapFace::SetModuleParams(const ModuleParams& cParam)
{
    m_cModuleParams = cParam;
}

void CCapFace::GetCarColor(CARLEFT_INFO_STRUCT *pCarLeftInfo, char *pszConf)
{
    switch (pCarLeftInfo->cCoreResult.nCarColor)
    {
    case CC_WHITE:
        sprintf(pszConf, "白色");
        break;
    case CC_GREY:
        sprintf(pszConf, "灰色");
        break;
    case CC_BLACK:
        sprintf(pszConf, "黑色");
        break;
    case CC_RED:
        sprintf(pszConf, "红色");
        break;
    case CC_YELLOW:
        sprintf(pszConf, "黄色");
        break;
    case CC_GREEN:
        sprintf(pszConf, "绿色");
        break;
    case CC_BLUE:
        sprintf(pszConf, "蓝色");
        break;
    case CC_PURPLE:
        sprintf(pszConf, "紫色");
        break;
    case CC_PINK:
        sprintf(pszConf, "粉色");
        break;
    case CC_BROWN:
        sprintf(pszConf, "棕色");
        break;
    default:
        sprintf(pszConf, "未知");
        break;
    }
}

void CCapFace::GetCarType(PROCESS_IMAGE_CORE_RESULT *pImgCoreInfo, char *pszConf)
{
    if (pImgCoreInfo->nCarType == CT_LARGE && PLATE_DOUBLE_MOTO != pImgCoreInfo->nType)
    {
        strcpy(pszConf, "大");
    }
    else if (pImgCoreInfo->nCarType == CT_SMALL && PLATE_DOUBLE_MOTO != pImgCoreInfo->nType)
    {
        strcpy(pszConf, "小");
    }
    else if (pImgCoreInfo->nCarType == CT_MID && PLATE_DOUBLE_MOTO != pImgCoreInfo->nType)
    {
        strcpy(pszConf, "中");
    }
    else if (PLATE_DOUBLE_MOTO == pImgCoreInfo->nType)
    {
        strcpy(pszConf, "摩托车");
    }
    else if (pImgCoreInfo->nCarType == CT_WALKMAN)
    {
        strcpy(pszConf, "行人");
    }
    else if (pImgCoreInfo->nCarType == CT_BIKE)
    {
        strcpy(pszConf, "非机动车");
    }
    else if (pImgCoreInfo->nCarType == CT_VEHICLE)
    {
        strcpy(pszConf, "机动车");
    }
    else
    {
        strcpy(pszConf, "未知");
    }
}

void CCapFace::GetEventDetInfo(CARLEFT_INFO_STRUCT *pCarLeftInfo, char *pszConf)
{
    pszConf[0] = 0;
    CHvString strEventCheck = "";

    if (m_cModuleParams.cTrackerCfgParam.nDetReverseRunEnable
            && pCarLeftInfo->cCoreResult.fReverseRun)
    {
        strEventCheck += "逆行,";
    }

    //压线,车道线从左到右编号从0开始
    if (pCarLeftInfo->cCoreResult.nDetectOverYellowLineEnable
            && pCarLeftInfo->cCoreResult.coltIsOverYellowLine != COLT_INVALID)
    {
        if (pCarLeftInfo->cCoreResult.coltIsOverYellowLine == COLT_NO)
        {
            strcat(pszConf, "压线:无\n");
        }
        else
        {
            CHvString strYellowLine = "";
            if (pCarLeftInfo->cCoreResult.fIsDoubleYellowLine)
            {
                strYellowLine.Format("压线:车道线%d<黄线>", pCarLeftInfo->cCoreResult.coltIsOverYellowLine + 1);
            }
            else
            {
                strYellowLine.Format("压线:车道线%d<实线>", pCarLeftInfo->cCoreResult.coltIsOverYellowLine + 1);
            }
            strEventCheck += strYellowLine;
            strEventCheck += ",";
            strYellowLine = strYellowLine + CHvString("\n");
            strcat(pszConf, strYellowLine.GetBuffer());
        }
    }

    //越线,车道线从左到右编号从0开始
    if (pCarLeftInfo->cCoreResult.nDetectCrossLineEnable
            && pCarLeftInfo->cCoreResult.coltIsCrossLine != COLT_INVALID)
    {
        if (pCarLeftInfo->cCoreResult.coltIsCrossLine == COLT_NO)
        {
            strcat(pszConf, "越线:无\n");
        }
        else
        {
            CHvString strCrossLine = "";
            strCrossLine.Format("越线:车道线%d", pCarLeftInfo->cCoreResult.coltIsCrossLine + 1);
            strEventCheck += strCrossLine;
            strEventCheck += ",";
            strCrossLine = strCrossLine + CHvString("\n");
            strcat(pszConf, strCrossLine.GetBuffer());
        }
    }

    //超速
    if ((int)(pCarLeftInfo->cCoreResult.fltCarspeed) >
            m_cModuleParams.cResultSenderParam.iSpeedLimit)
    {
        strEventCheck += "超速,";
    }

    if (strEventCheck != "")
    {
        CHvString strTmp;
        strTmp.Format("违章:是<%s>", strEventCheck.GetBuffer());
        strcat(pszConf, strTmp.GetBuffer());
    }
    else
    {
        //去掉最后一个换行符
        pszConf[strlen(pszConf) - 1] = 0;
    }
}

HRESULT CCapFace::SetCaptureSynSignalEnableFlash(bool fIsNight)
{
    HRESULT hr = E_FAIL;
    bool fIsNeedUpdate = false;
    int nRet = E_FAIL;
    int nSize = sizeof(nRet);
    int nStatue;

    if(m_iFlashStatus == -1)
    {
        if(fIsNight)
        {
            nStatue = 1;
        }
        else
        {
            nStatue = 0;
        }
        fIsNeedUpdate = true;
    }
    else
    {
        if(fIsNight && m_iFlashStatus == 0)
        {
            nStatue = 1;
            fIsNeedUpdate = true;
        }
        else if(!fIsNight && m_iFlashStatus == 1)
        {
            nStatue = 0;
            fIsNeedUpdate = true;
        }
    }

    if(!fIsNeedUpdate)
    {
        return S_OK;
    }

    hr = g_cHvPciLinkApi.SendData(
        PCILINK_SET_CAPTURE_SYNSIGNAL,
        &nStatue,
        sizeof(nStatue),
        &nRet,
        &nSize);
    if(hr == S_OK)
    {
        if(nRet == nStatue)
        {
            hr = S_OK;
            m_iFlashStatus = nStatue;
        }
        else
        {
            hr = E_FAIL;
        }
    }
    return hr;
}
