#include "SWFC.h"
#include "SWRecognizeTransformPTFilter.h"

CSWRecognizeTransformPTFilter::CSWRecognizeTransformPTFilter() : CSWBaseFilter(1, 4)
    , CSWMessage(MSG_RECOGNIZE_CTRL_START, MSG_RECOGNIZE_CTRL_END)
    , m_pTrackerCfg(NULL)
    , m_fSendJPEG(FALSE)
    , m_fSendDebug(FALSE)
    , m_pThread(NULL)
    , m_fInitialized(FALSE)
    , m_iCarArriveTriggerType(0)
    , m_iIOLevel(0)
    , m_fDetectingTollEvasion(FALSE)
    , m_iTailgatingTimeThreshold(0)
    , m_iBarrierNormalMode(0)
{
    GetIn(0)->AddObject(CLASSID(CSWImage));
    GetOut(0)->AddObject(CLASSID(CSWPosImage));
    GetOut(1)->AddObject(CLASSID(CSWCarLeft));
    GetOut(2)->AddObject(CLASSID(CSWCameraDataPDU));
    GetOut(3)->AddObject(CLASSID(CSWImage));
}

CSWRecognizeTransformPTFilter::~CSWRecognizeTransformPTFilter()
{
    Clear();
}

VOID CSWRecognizeTransformPTFilter::Clear()
{
}

HRESULT CSWRecognizeTransformPTFilter::Initialize(
    INT iGlobalParamIndex,
    PVOID pvFrameParam,
    PVOID pvCfgParam
    )
{
    if (m_fInitialized)
    {
        return E_FAIL ;
    }
    if (NULL == pvFrameParam || NULL == pvCfgParam)
    {
        return E_INVALIDARG ;
    }

    char *pszFrameParam = (char*)pvFrameParam;

    m_pTrackerCfg = (TRACKER_CFG_PARAM *)pvCfgParam;
    m_pTrackerCfg->iPlatform = 2;

    swpa_sscanf(
        pszFrameParam,
        "[%d,%d,%d,%d],%d,%d,%d",
        &m_cPlateRecognitionParam.rgDetArea[0].left,
        &m_cPlateRecognitionParam.rgDetArea[0].top,
        &m_cPlateRecognitionParam.rgDetArea[0].right,
        &m_cPlateRecognitionParam.rgDetArea[0].bottom,
        &m_cPlateRecognitionParam.nMinPlateWidth,
        &m_cPlateRecognitionParam.nMaxPlateWidth,
        &m_cPlateRecognitionParam.nVariance
        );

    INIT_PHOTO_RECOGER_PARAM cInitParam;
    cInitParam.nPlateRecogParamIndex = iGlobalParamIndex;
    swpa_memcpy(&cInitParam.cTrackerCfgParam, m_pTrackerCfg, sizeof(TRACKER_CFG_PARAM));

    SW_TRACE_NORMAL("<RecognizeTransformPTFilter>work index : %d.", cInitParam.nPlateRecogParamIndex);
    HRESULT hr = CSWDspLinkCtrl::GetInstance()->InitPhotoRecoger(cInitParam);
    if (FAILED(hr))
    {
        SW_TRACE_NORMAL("RecognizeTransformPTFilter algorithm initialize failed!\n");
        Clear();
        return hr;
    }

    m_pThread = new CSWThread(this->GetName());
    if (NULL == m_pThread)
    {
        SW_TRACE_NORMAL("RecognizeTransformPTFilter thread initialize failed!\n");
        Clear();
        return E_OUTOFMEMORY ;
    }

    m_cSemImage.Create(0, MAX_IMAGE_COUNT);

    m_fInitialized = TRUE;
    return S_OK ;
}

HRESULT CSWRecognizeTransformPTFilter::RegisterCallBackFunction(PVOID OnDSPAlarm, PVOID pvParam)
{
    typedef VOID (*fnOnDSPAlarm)(PVOID pvParam, INT iInfo);
    HRESULT hr = CSWDspLinkCtrl::GetInstance()->RegisterCallBack((fnOnDSPAlarm)OnDSPAlarm, pvParam);
    SW_TRACE_NORMAL("RegisterCallBackFunction(0x%08x,0x%08x) return 0x%08x\n", (INT )OnDSPAlarm, (INT )pvParam, hr);
    return hr;
}

VOID CSWRecognizeTransformPTFilter::OnProcessProxy(PVOID pvParam)
{
    if (pvParam != NULL)
    {
        CSWRecognizeTransformPTFilter* pThis = (CSWRecognizeTransformPTFilter*)pvParam;
        pThis->OnProcess();
    }
}

HRESULT CSWRecognizeTransformPTFilter::Run()
{
    if (!m_fInitialized)
    {
        return E_FAIL ;
    }

    HRESULT hr = S_OK;
    if (GetState() != FILTER_RUNNING)
    {
        CSWBaseFilter::Run();
        hr = m_pThread->Start((START_ROUTINE) & CSWRecognizeTransformPTFilter::OnProcessProxy, (PVOID)this);
    }

    return hr;
}

HRESULT CSWRecognizeTransformPTFilter::Stop()
{
    if (NULL != m_pThread)
    {
        m_pThread->Stop();
    }
    return CSWBaseFilter::Stop();
}

HRESULT CSWRecognizeTransformPTFilter::Receive(CSWObject* obj)
{
    if (IsDecendant(CSWImage, obj))
    {
        CSWImage* pImage = (CSWImage*)obj;
        CSWImage *pCapImage = NULL;

        BOOL fEvasion = FALSE;
        if (GetTollEvasionDetectingFlag())
        {
            if (FAILED(CheckTollEvasion(fEvasion)))
            {
                SW_TRACE_DEBUG("Err: failed to check toll evasion\n");
            }
        }

        if (swpa_strcmp("VPIF", pImage->GetFrameName()) == 0)
        {
            if (pImage->IsCaptureImage() == TRUE)
            {
                SW_COMPONENT_IMAGE cImageSrc;
                if (S_OK != CSWImage::CreateSWImage(&pCapImage,
                        SW_IMAGE_BT1120,
                        pImage->GetWidth(),
                        pImage->GetHeight(),
                        CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY),
                        0, 0,
                        FALSE, "VPIF"))
                {
                    SW_TRACE_NORMAL("<RecognizeTransformPTFilter>Create image failed!");
                    return E_OUTOFMEMORY ;
                }

                pImage->GetImage(&cImageSrc);

                if (S_OK != pCapImage->Assign(cImageSrc, CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY)))
                {
                    pCapImage->Release();
                    SW_TRACE_NORMAL("<RecognizeTransformPTFilter>Assign failed!");
                    return E_OUTOFMEMORY ;
                }

                pCapImage->SetRefTime(pImage->GetRefTime());
            }
        }
        else
        {
            pCapImage = pImage;
            pCapImage->AddRef();
        }

        if (m_fSendJPEG == TRUE
            && swpa_strcmp("VPIF", pImage->GetFrameName()) == 0)
        {
            CSWPosImage *pPosImage = new CSWPosImage(pImage);
            GetOut(0)->Deliver(pPosImage);
            pPosImage->Release();
        }

        if (pCapImage == NULL)
        {
            return S_OK ;
        }

        pCapImage->SetCaptureFlag(FALSE);

        while (TRUE)
        {
            m_cMutexImage.Lock();
            if (m_lstImage.GetCount() < MAX_IMAGE_COUNT)
            {
                m_cMutexImage.Unlock();
                break;
            }
            else
            {
                m_cMutexImage.Unlock();
                CSWApplication::Sleep(100);
            }
        }

        m_cMutexTollEvasion.Lock();
        m_lstTollEvasionFlag.AddTail(fEvasion);
        m_cMutexTollEvasion.Unlock();

        //放入队列
        m_cMutexImage.Lock();
        m_lstImage.AddTail(pCapImage);
        if (S_OK != m_cSemImage.Post())
        {
            SW_TRACE_DEBUG("<RecognizeTransformPTFilter>SemImage post failed. list count:%d.\n", m_lstImage.GetCount());
        }
        m_cMutexImage.Unlock();
    }

    return S_OK ;
}

HRESULT CSWRecognizeTransformPTFilter::OnProcess()
{
    SW_TRACE_DEBUG("CSWRecognizeTransformPTFilter run.\n");

    DWORD dwFps = 0;
    DWORD dwBeginTick = CSWDateTime::GetSystemTick();

    PROCESS_EVENT_STRUCT cProcessEvent;

    while (S_OK == m_pThread->IsValid() && GetState() == FILTER_RUNNING)
    {
        if (S_OK != m_cSemImage.Pend(1000))
        {
            continue;
        }
        m_cMutexImage.Lock();
        if (!m_lstImage.IsEmpty())
        {
            CSWImage* pImage = m_lstImage.RemoveHead();
            m_cMutexImage.Unlock();

            m_cMutexTollEvasion.Lock();
            BOOL fEvasion = m_lstTollEvasionFlag.RemoveHead();
            m_cMutexTollEvasion.Unlock();

            swpa_memset(&cProcessEvent, 0, sizeof(PROCESS_EVENT_STRUCT));
            HRESULT hr = CSWDspLinkCtrl::GetInstance()->ProcessPhoto(pImage,
                &m_cPlateRecognitionParam,
                &cProcessEvent
                );
            if (S_OK == hr)
            {
                if (cProcessEvent.dwEventId & EVENT_CARLEFT)
                {
                    for (int i = 0; i < cProcessEvent.iCarLeftInfoCount; ++i)
                    {
                        CarLeftEvent(&cProcessEvent.rgCarLeftInfo[i], fEvasion, pImage->GetRefTime());
                    }
                }
            }
            else
            {
                SW_TRACE_NORMAL("<RecognizeTransformPTFilter>ProcessPhoto failed. 0x%08x.\n", hr);
            }

            //释放资源
            SAFE_RELEASE(pImage);
        }
        else
        {
            m_cMutexImage.Unlock();
        }
    }

    SW_TRACE_DEBUG("RecognizeTransformPTFilter exit.\n");
    return S_OK ;
}

HRESULT CSWRecognizeTransformPTFilter::CarArriveEvent(CARARRIVE_INFO_STRUCT *pCarArriveInfo)
{
    //todo..
    return S_OK ;
}

HRESULT CSWRecognizeTransformPTFilter::SetOutputImage(INT iBest, INT iLast)
{
    m_iOutPutBestImage = iBest;
    m_iOutPutLastImage = iLast;
    return S_OK ;
}

HRESULT CSWRecognizeTransformPTFilter::CarLeftEvent(CARLEFT_INFO_STRUCT *pCarLeftInfo, const BOOL fEvasion, const DWORD dwEvasionTick)
{
    static int siPlateCount = 0;
    siPlateCount++;
    SW_TRACE_DEBUG("<RecognizeTransformPTFilter>car:%d.\n", siPlateCount);

    if (NULL == pCarLeftInfo)
    {
        return E_INVALIDARG ;
    }

    //通知Graph构造CarLeft对象
    CSWCarLeft* pCarLeft = NULL;
    SendCommand(1, (WPARAM)&pCarLeft, (LPARAM)pCarLeftInfo);
    if (pCarLeft == NULL || !IsDecendant(CSWCarLeft, pCarLeft))
    {
        SAFE_RELEASE(pCarLeft);
        SW_TRACE_NORMAL("pCarLeft == NULL or is not CSWCarLeft object, used default CSWCarLeft\n");
        pCarLeft = new CSWCarLeft();
    }
    pCarLeft->SetParameter(m_pTrackerCfg, pCarLeftInfo);

    if (NULL != pCarLeft)
    {
        SW_TRACE_NORMAL("object carleft name:%s,PlateNo:%s[0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x]",
            pCarLeft->Name(),
            (LPCSTR )pCarLeft->GetPlateNo(),
            pCarLeft->GetImage(0),
            pCarLeft->GetImage(1),
            pCarLeft->GetImage(2),
            pCarLeft->GetImage(3),
            pCarLeft->GetImage(4),
            pCarLeft->GetImage(5),
            pCarLeft->GetImage(6));

        SW_TRACE_DEBUG("Info: fEvasion = %d\n", fEvasion);

        if (FAILED(pCarLeft->Invoke("SetTollEvasionFlag", fEvasion )))
        {
              SW_TRACE_DEBUG("Err: failed to send toll evasion flag: %d\n", fEvasion);
        }

        if (FAILED(pCarLeft->Invoke("SetTollEvasionTick", dwEvasionTick )))
        {
              SW_TRACE_DEBUG("Err: failed to send toll evasion tick: %d\n", dwEvasionTick);
        }    

        GetOut(1)->Deliver(pCarLeft);
        pCarLeft->Release();
        SW_TRACE_DEBUG("object carleft done.\n");
    }
    return S_OK ;
}

/**
 *@brief 设置识别开关
 */
HRESULT CSWRecognizeTransformPTFilter::OnRecognizeGetJPEG(WPARAM wParam, LPARAM lParam)
{
    m_fSendJPEG = (BOOL)wParam;
    if (!m_fSendJPEG)
    {
        m_fSendDebug = FALSE;
    }
    SW_TRACE_DEBUG("<RecognizeTransformPTFilter>Set RecognizeEnable %d.\n", (UINT )wParam);
    return S_OK ;
}

/**
 *@brief 输出调试码流
 */
HRESULT CSWRecognizeTransformPTFilter::OnRecognizeOutPutDebug(WPARAM wParam, LPARAM lParam)
{
    m_fSendDebug = (BOOL)wParam;
    SW_TRACE_DEBUG("<RecognizeTransformPTFilter>Set RecognizeOutPutDebug %d.\n", (UINT )wParam);
    return S_OK ;
}


/**
 *@brief 读取栏杆机状态
 */
HRESULT CSWRecognizeTransformPTFilter::GetBarrierStatus(INT& iStatus)
{
    if (FAILED(CSWMessage::SendMessage(MSG_GET_BARRIER_STATE, 0, (LPARAM)&iStatus)))
    {
        SW_TRACE_DEBUG("Err: failed to get barrier state\n");
        return E_FAIL;
    }

    static INT iPrevState = GetBarrierNormalState();

    //SW_TRACE_DEBUG("Info: barrier state: %d\n", iStatus);

    if (iPrevState != iStatus)
    {
        SW_TRACE_DEBUG("Info: barrier state: %d\n", iStatus);
        iPrevState = iStatus;
    }
    
    return S_OK;
}


/**
 *@brief 判断是否有冲卡
 */
HRESULT CSWRecognizeTransformPTFilter::CheckTollEvasion(BOOL& fEvasion)
{
    INT iStatus = 0;
    static INT iPrevStatus = GetBarrierNormalState();
    static DWORD dwUpToDownTick = 0;

    fEvasion = FALSE;
    
    if (FAILED(GetBarrierStatus(iStatus)))
    {
        SW_TRACE_DEBUG("Err: failed to read barrier status\n");
        return E_FAIL;
    }

    if (0 == GetBarrierNormalState()) 
    {
        if (0 == iStatus)
        {
            fEvasion = TRUE; //
            
            if (iPrevStatus)
            {
                dwUpToDownTick = CSWDateTime::GetSystemTick();
            }
        }
        else
        {
            DWORD dwTick = CSWDateTime::GetSystemTick();
            if (dwTick - dwUpToDownTick < GetTailgatingTimeThreshold())
            {
                fEvasion = TRUE;
            }
        }
    }
    else
    {    
        if (1 == iStatus)
        {
            fEvasion = TRUE; //
            
            if (!iPrevStatus)
            {
                dwUpToDownTick = CSWDateTime::GetSystemTick();
            }
        }
        else
        {
            DWORD dwTick = CSWDateTime::GetSystemTick();
            if (dwTick - dwUpToDownTick < GetTailgatingTimeThreshold())
            {
                fEvasion = TRUE;
            }
        }
    }

    if (iPrevStatus != iStatus)
    {
        iPrevStatus = iStatus;
    }
    
    return S_OK;
}



