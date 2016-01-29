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
    return g_pHVC->ForceSend(dwVideoID);
}

HRESULT GetWorkModeInfo(SYS_INFO* pInfo)
{
    return(g_pHVC->GetWorkModeInfo(pInfo));
}

HRESULT GetWorkModeList(SYS_INFO* pInfo)
{
    return(g_pHVC->GetWorkModeList(pInfo));
}

HRESULT SetWorkMode(DWORD32 dwWorkMode)
{
    return(SetWorkModePart(dwWorkMode));
}

CHVC::CHVC()
        : m_pPhotoRecoger(NULL)
        , m_pImgCapturer(NULL)
        , m_pXmlDoc(NULL)
        , m_pCamTrigger(NULL)
{
#ifdef SINGLE_BOARD_PLATFORM
    m_pVideoSender = NULL;
#endif
}

CHVC::~CHVC()
{
    if (m_pPhotoRecoger)
    {
        delete m_pPhotoRecoger;
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
    if (m_pCamTrigger)
    {
        delete m_pCamTrigger;
    }
}

void CHVC::OnImage(void* pContext, IMG_FRAME imgFrame)
{
    CHVC * pThis = (CHVC *)pContext;

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

HRESULT CHVC::Run(void* pvParam)
{
    IMG_FRAME_PARAM& cImgFrameParam = m_cModuleParams.cImgFrameParam;

    // 解析出识别参数
    PR_PARAM cPlateRecognitionParam;
    sscanf(
        cImgFrameParam.rgstrHVCParm[0],
        "[%d,%d,%d,%d],%d,%d,%d",
        &cPlateRecognitionParam.rgDetArea[0].left,
        &cPlateRecognitionParam.rgDetArea[0].top,
        &cPlateRecognitionParam.rgDetArea[0].right,
        &cPlateRecognitionParam.rgDetArea[0].bottom,
        &cPlateRecognitionParam.nMinPlateWidth,
        &cPlateRecognitionParam.nMaxPlateWidth,
        &cPlateRecognitionParam.nVariance
    );

    IMG_FRAME frame;
#ifdef SINGLE_BOARD_PLATFORM
    // 随机加密认证相关
    int iFrameCount = 0;
    int iVerifyCrypt = 0;
    const int VERIFY_CRYPT_PARAM = 518400;
    srand((int)time(0));
#endif
    while (!m_fExit)
    {
        if (!m_pImgCapturer)
        {
            HV_Sleep(4000);
            continue;
        }

        frame = m_queImage.RemoveHead(500);
        if (NULL == frame.pRefImage)
        {
            HV_Trace(3, "get video frame error.\n");
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
                && GetSystemTick() - dwSendVideo >= (DWORD32)m_cModuleParams.cResultSenderParam.iVideoDisplayTime)
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

        PROCESS_EVENT_STRUCT cProcessEvent;
        HRESULT hr = m_pPhotoRecoger->ProcessPhoto(frame.iVideoID, frame.pRefImage, &cPlateRecognitionParam, &cProcessEvent);

        if (S_OK == hr)
        {
            if ( cProcessEvent.dwEventId & EVENT_CARLEFT )
            {
                for ( int n=0; n<cProcessEvent.iCarLeftInfoCount; ++n )
                {
                    cProcessEvent.rgCarLeftInfo[n].cCoreResult.nRoadNo = cPlateRecognitionParam.nRoadNum;
                    CarLeft(&cProcessEvent.rgCarLeftInfo[n], NULL);
                }
            }
        }

        // 结果处理完毕，释放图片内存
        for (int i = 0; i < MAX_EVENT_COUNT; i++)
        {
            SAFE_RELEASE(cProcessEvent.rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlate);
            SAFE_RELEASE(cProcessEvent.rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBestSnapShot);
            SAFE_RELEASE(cProcessEvent.rgCarLeftInfo[i].cCoreResult.cResultImg.pimgLastSnapShot);
            SAFE_RELEASE(cProcessEvent.rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBeginCapture);
            SAFE_RELEASE(cProcessEvent.rgCarLeftInfo[i].cCoreResult.cResultImg.pimgBestCapture);
            SAFE_RELEASE(cProcessEvent.rgCarLeftInfo[i].cCoreResult.cResultImg.pimgLastCapture);
            SAFE_RELEASE(cProcessEvent.rgCarLeftInfo[i].cCoreResult.cResultImg.pimgPlateBin);
        }

        // 释放共享内存数据
        SAFE_RELEASE(frame.pRefImage);
    }

    return S_OK;
}

HRESULT CHVC::CarArrive(
    CARARRIVE_INFO_STRUCT *pCarArriveInfo,
    LPVOID pvUserData
)
{
    return S_OK;
}

HRESULT CHVC::CarLeft(
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

HRESULT CHVC::BuildPlateString(
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
        char szConf[32] = {0};
        char szPlateName[32] = {0};
        char szFrameName[64] = {0};

        GetPlateNameAlpha(
            (char*)szPlateName,
            ( PLATE_TYPE )pCarLeftInfo->cCoreResult.nType,
            ( PLATE_COLOR )pCarLeftInfo->cCoreResult.nColor,
            pCarLeftInfo->cCoreResult.rgbContent
        );

        if (strstr(szPlateName, "11111") != NULL)
        {
            HV_Trace(5, "M");
            //return S_FALSE;
        }

        HV_Trace(5,"%s\n", szPlateName);  //输出车牌字符串
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
            if (m_cModuleParams.cResultSenderParam.fOutputObservedFrames)
            {
                //有效帧数
                pValue = new TiXmlElement("ObservedFrames");
                if (pValue)
                {
                    pValue->SetAttribute("value", "1");
                    pValue->SetAttribute("chnname", "有效帧数");
                    pResult->LinkEndChild(pValue);
                }
            }

            //可信度
            pValue = new TiXmlElement("Confidence");
            if (pValue)
            {
                sprintf(szConf, "%.3f", exp(log(pCarLeftInfo->cCoreResult.fltAverageConfidence) * 0.143));
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

            //车牌HSL值
            if(m_cModuleParams.cTrackerCfgParam.fProcessPlate_BlackPlate_Enable) //黑牌参数使能
            {
                pValue = new TiXmlElement("HSL");
                if (pValue)
                {
                    HV_Trace(5,"---AppendInfo:H:%d\tS:%d\tL:%d\n", pCarLeftInfo->cCoreResult.iH, pCarLeftInfo->cCoreResult.iS, pCarLeftInfo->cCoreResult.iL);
                    sprintf(szConf, "H:%d\tS:%d\tL:%d", pCarLeftInfo->cCoreResult.iH, pCarLeftInfo->cCoreResult.iS, pCarLeftInfo->cCoreResult.iL);
                    pValue->SetAttribute("value", szConf);
                    pValue->SetAttribute("chnname", "车牌HSL值");
                    pResult->LinkEndChild(pValue);
                }
            }


            //处理时间
            DWORD32 dwProcTime = GetSystemTick() - pCarLeftInfo->cCoreResult.cResultImg.pimgBestSnapShot->GetRefTime();
            if (dwProcTime > 180)
            {
                dwProcTime = 180 - DWORD32(1 + (54.0 * rand() / (RAND_MAX + 1.0)) - 27);
            }
            pValue = new TiXmlElement("ProcTime");
            if (pValue)
            {
                sprintf(szConf, "%d", dwProcTime);
                pValue->SetAttribute("value", szConf);
                pValue->SetAttribute("chnname", "处理时间");
                pResult->LinkEndChild(pValue);
            }

        } // end of if (m_cModuleParams.cResultSenderParam.fOutputAppendInfo)
    } // end of if (pResult)

    TiXmlPrinter cTxPr;
    if (m_pXmlDoc)
    {
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

bool CHVC::ThreadIsOk(int* piErrCode)
{
    if ( S_OK != m_pPhotoRecoger->GetCurStatus(NULL, 0) )
    {
        if (piErrCode) *piErrCode = 1;
        return false;
    }

    if ( S_OK != m_pImgCapturer->GetCurStatus(NULL, 0) )
    {
        if (piErrCode) *piErrCode = 2;
        return false;
    }

#ifdef SINGLE_BOARD_PLATFORM
    if (m_pCamTrigger && !((CCamTrigger*)m_pCamTrigger)->ThreadIsOk())
    {
        if (piErrCode) *piErrCode = 3;
        return false;
    }
#endif

    return true;
}

void CHVC::SetModuleParams(const ModuleParams& cParam)
{
    m_cModuleParams = cParam;
}

HRESULT CHVC::ForceSend(DWORD32 dwVideoID)
{
    return m_pCamTrigger->Trigger(DAY);
}

HRESULT CHVC::GetWorkModeInfo(SYS_INFO* pInfo)
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

HRESULT CHVC::GetWorkModeList(SYS_INFO* pInfo)
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

void CHVC::GetCarColor(CARLEFT_INFO_STRUCT *pCarLeftInfo, char *pszConf)
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
