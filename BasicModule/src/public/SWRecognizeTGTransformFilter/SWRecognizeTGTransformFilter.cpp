#include <limits.h>
#include "SWFC.h"
#include "SWRecognizeTGTransformFilter.h"
#include "SWGB28181Parameter.h"
#include "AsyncDetResult.h"
using namespace swTgVvdApp;

#define DEF_PULSE_WIDTH (4)
CSWRecognizeTGTransformFilter::CSWRecognizeTGTransformFilter()
	: CSWBaseFilter(1,3)
	, CSWMessage(MSG_RECOGNIZE_CTRL_START, MSG_RECOGNIZE_CTRL_END)
	, m_pTrackerCfg(NULL)
	, m_fSendJPEG(FALSE)
	, m_fSendDebug(FALSE)
	, m_pThread(NULL)
	, m_pProcQueueThread(NULL)
	, m_fInitialized(FALSE)
	, m_iCarArriveTriggerType(0)
	, m_iIOLevel(0)
	, m_fReverseRunFilterFlag(FALSE)
	, m_dwPlateCount(0)
    , m_nEnvLightType(0)
	, m_iPlateLightSum(0)
	, m_iPlateCount(0)
	, m_iPlateLightMin(INT_MAX)
	, m_iPlateLightMax(INT_MIN)
	, m_iPlateLightCheckCount(5)
	, m_iExpectPlateLightMin(80)
	, m_iExpectPlateLightMax(120)
	, m_iUpStep(1)
	, m_iDownStep(1)
	, m_iPulseWidthLevel(DEF_PULSE_WIDTH)
	, m_pMatchThread(NULL)
    , m_nEnvFromM3(3)
    , m_iFrame(0)
{
    m_nCurEnvPeriod = 3; // 默认白天
	GetIn(0)->AddObject(CLASSID(CSWImage));
	GetOut(0)->AddObject(CLASSID(CSWPosImage));
	GetOut(1)->AddObject(CLASSID(CSWCarLeft));
	GetOut(2)->AddObject(CLASSID(CSWCameraDataPDU));
	//GetOut(3)->AddObject(CLASSID(CSWImage));
}

CSWRecognizeTGTransformFilter::~CSWRecognizeTGTransformFilter()
{
	Clear();
}

VOID CSWRecognizeTGTransformFilter::Clear()
{
	SAFE_RELEASE(m_pThread);
	SAFE_RELEASE(m_pProcQueueThread);
	SAFE_RELEASE(m_pMatchThread);
}

HRESULT CSWRecognizeTGTransformFilter::Initialize(
	INT iGlobalParamIndex
	, INT nLastLightType
	, INT nLastPulseLevel
	, INT nLastCplStatus
	, PVOID pvParam
	, INT iMinPlateLight
	, INT iMaxPlateLight
	)
{
	if( m_fInitialized )
	{
		return E_FAIL;
	}
	if( NULL == pvParam )
	{
		return E_INVALIDARG;
	}

	m_pTrackerCfg = (TRACKER_CFG_PARAM *)pvParam;
	m_pTrackerCfg->iPlatform = 2;

	INIT_VIDEO_RECOGER_PARAM cInitParam;
	cInitParam.nPlateRecogParamIndex = iGlobalParamIndex;
	cInitParam.nLastLightType = nLastLightType;
	cInitParam.nLastPulseLevel = nLastPulseLevel;
	cInitParam.nLastCplStatus = nLastCplStatus;
	cInitParam.dwArmTick = CSWDateTime::GetSystemTick();
	swpa_memcpy(&cInitParam.cTrackerCfgParam, m_pTrackerCfg, sizeof(TRACKER_CFG_PARAM));

	SW_TRACE_DEBUG("<RecognizeTransformFilter>work index : %d.", cInitParam.nPlateRecogParamIndex);
	HRESULT hr = CSWBaseLinkCtrl::GetInstance()->InitVideoRecoger(cInitParam);
	if(FAILED(hr))
	{
		SW_TRACE_NORMAL("RecognizeTransformFilter algorithm initialize failed!\n");
		Clear();
		return hr;
	}
	m_iPlateLightCheckCount = m_pTrackerCfg->nPlateLightCheckCount;
	m_iExpectPlateLightMax = iMaxPlateLight;
	m_iExpectPlateLightMin = iMinPlateLight;
	m_iPulseWidthLevel = nLastPulseLevel;
	m_pMatchThread = new CSWThread(this->GetName());
	if (NULL == m_pMatchThread)
	{
		SW_TRACE_NORMAL("CSWRecognizeransformFilter match thread initialize failed!\n");
		Clear();
		return E_OUTOFMEMORY;
	}
	
	m_pProcQueueThread = new CSWThread(this->GetName());
	if (NULL == m_pProcQueueThread)
	{
		SW_TRACE_NORMAL("CSWRecognizeransformFilter proc queue thread initialize failed!\n");
		Clear();
		return E_OUTOFMEMORY;
	}
	
	m_pThread = new CSWThread(this->GetName());
	if( NULL == m_pThread )
	{
		SW_TRACE_NORMAL("RecognizeTransformFilter thread initialize failed!\n");
		Clear();
		return E_OUTOFMEMORY;
	}

	m_lstImage.SetMaxCount(MAX_IMAGE_COUNT);
	m_cSemImage.Create(0, MAX_IMAGE_COUNT);

	m_lstProcQueueList.SetMaxCount(MAX_DECT_COUNT);
	m_cSemProcQueue.Create(0, MAX_DECT_COUNT);

	m_lstMatchImage.SetMaxCount(MAX_MATCH_COUNT);
	m_cSemMatch.Create(0, 1);

	swpa_memset(m_rgiAllCarTrigger, 0, sizeof(m_rgiAllCarTrigger));

	
	SW_TRACE_DEBUG("CSWRecognizeTGTransformFilter initialize finish!\n");

	m_fInitialized = TRUE;
	return S_OK;

}

/*
HRESULT CSWRecognizeTGTransformFilter::OnInitFpgaFlash()
{
	DWORD dwFlashSet1Reg = 0x926;
	DWORD dwFlashSet2Reg = 0x927;
	DWORD dwWriteValue = 0;
	DWORD dwEnable = 0, dwCoupling = 0, dwOutType = 0, dwPolarity = 0, dwPulseWidth = 0;
	DWORD rgFpgaParam[2] = {0};

	dwEnable 	= 1;		//使能
	dwCoupling	= 0; 		//抓拍耦合到频闪设置位 0 不耦合 1 耦合
	dwOutType	= 1;		//输出类型位置位 0 上拉(电平) 1 OC(开关)
	dwPolarity	= 0;		//极性设置 0 负极性 1 正极性
	dwPulseWidth= 50;		//脉宽设置  0-100
	dwWriteValue = (dwEnable << 11) | (dwCoupling << 10) | (dwOutType << 9) | (dwPolarity << 8) | (dwPulseWidth & 0xFF) & 0xFFF;
	rgFpgaParam[0] = 0x926;
	rgFpgaParam[1] = dwWriteValue;
	CSWMessage::SendMessage(MSG_SET_CAM_FPGA_REG, (WPARAM)rgFpgaParam, 0);

	rgFpgaParam[0] = 0x927;
	rgFpgaParam[1] = dwWriteValue;
	CSWMessage::SendMessage(MSG_SET_CAM_FPGA_REG, (WPARAM)rgFpgaParam, 0);

	//以下为提亮图像设置， 不开放寄存器意义  __WRITE_FPGA_START__
	rgFpgaParam[0] = 0xb81;		//gamma 提亮
	rgFpgaParam[1] = 0x02;
	CSWMessage::SendMessage(MSG_SET_CAM_FPGA_REG, (WPARAM)rgFpgaParam, 0);

	rgFpgaParam[0] = 0xb83;		//数字增益提亮
	rgFpgaParam[1] = 0x01;
	CSWMessage::SendMessage(MSG_SET_CAM_FPGA_REG, (WPARAM)rgFpgaParam, 0);

	rgFpgaParam[0] = 0xa3c;
	rgFpgaParam[1] = 0xd0;
	CSWMessage::SendMessage(MSG_SET_CAM_FPGA_REG, (WPARAM)rgFpgaParam, 0);

	rgFpgaParam[0] = 0xa84;
	rgFpgaParam[1] = 0x03;
	CSWMessage::SendMessage(MSG_SET_CAM_FPGA_REG, (WPARAM)rgFpgaParam, 0);
	// __ WRITE_FPGA_END__

	//SW_TRACE_DEBUG("-------------- dwWriteValue = 0x%03x\n", dwWriteValue);

	return S_OK;
}
*/

HRESULT CSWRecognizeTGTransformFilter::RegisterCallBackFunction(PVOID OnDSPAlarm, PVOID pvParam)
{
	typedef VOID (*fnOnDSPAlarm)(PVOID pvParam, INT iInfo);
	HRESULT hr = CSWBaseLinkCtrl::GetInstance()->RegisterCallBack((fnOnDSPAlarm)OnDSPAlarm, pvParam);
	SW_TRACE_DEBUG("RegisterCallBackFunction(0x%08x,0x%08x) return 0x%08x\n", (INT)OnDSPAlarm, (INT)pvParam, hr);
	return hr;
}

VOID CSWRecognizeTGTransformFilter::OnProcessProxy(PVOID pvParam)
{
	if(pvParam != NULL)
	{
		CSWRecognizeTGTransformFilter* pThis = (CSWRecognizeTGTransformFilter*)pvParam;
		pThis->OnProcess();
	}
}

VOID CSWRecognizeTGTransformFilter::OnProcessSyncProxy(PVOID pvParam)
{
	if (pvParam != NULL)
	{
		CSWRecognizeTGTransformFilter* pThis = (CSWRecognizeTGTransformFilter*)pvParam;
		pThis->OnProcessSync();
	}
}

VOID CSWRecognizeTGTransformFilter::OnMatchImageProxy(PVOID pvParam)
{
	if (pvParam != NULL)
	{
		CSWRecognizeTGTransformFilter* pThis = (CSWRecognizeTGTransformFilter*)pvParam;
		pThis->OnMatchImage();
	}
}

HRESULT CSWRecognizeTGTransformFilter::Run()
{
	if( !m_fInitialized )
	{
		SW_TRACE_DEBUG("CSWRecognizeTGTransformFilter does not Initialize");
		return E_FAIL;
	}

//	OnInitFpgaFlash();

	HRESULT hr = S_OK;
	if( GetState() != FILTER_RUNNING )
	{
		CSWBaseFilter::Run();
		hr = m_pMatchThread->Start((START_ROUTINE)&CSWRecognizeTGTransformFilter::OnMatchImageProxy, (PVOID)this);
		
		if (S_OK == hr)
			hr = m_pProcQueueThread->Start((START_ROUTINE)&CSWRecognizeTGTransformFilter::OnProcessSyncProxy, (PVOID)this);

		if (S_OK == hr)
			hr = m_pThread->Start((START_ROUTINE)&CSWRecognizeTGTransformFilter::OnProcessProxy, (PVOID)this);
	}

	return hr;
}

HRESULT CSWRecognizeTGTransformFilter::Stop()
{
	if(NULL != m_pThread)
	{
		m_pThread->Stop();
	}
	if (NULL != m_pProcQueueThread)
	{
		m_pProcQueueThread->Stop();
	}
	if (NULL != m_pMatchThread)
	{
		m_pMatchThread->Stop();
	}
	
	return CSWBaseFilter::Stop();
}

void DrawRect(CSWImage *pImage, HV_RECT rcPos)
{
    SW_COMPONENT_IMAGE cImage;
    pImage->GetImage(&cImage);
    PBYTE pbY = cImage.rgpbData[0];

    rcPos.top *= 2;
    rcPos.bottom *= 2;

    int nRcWidth = rcPos.right - rcPos.left;
    int nRcHeight = rcPos.bottom - rcPos.top;

    PBYTE pbTop = pbY + cImage.rgiStrideWidth[0] * rcPos.top + rcPos.left;
    PBYTE pbLeft = pbTop;
    PBYTE pbRight = pbLeft + nRcWidth;
    PBYTE pbBottom = pbY + cImage.rgiStrideWidth[0] * rcPos.bottom + rcPos.left;

    int nGray = 255;
    // top
    memset(pbTop, nGray, nRcWidth);
    memset(pbTop + cImage.rgiStrideWidth[0], nGray, nRcWidth);

    for (int y = 0; y < nRcHeight; ++y)
    {
        // left
        pbLeft[0] = nGray;
        pbLeft[1] = nGray;
        pbLeft += cImage.rgiStrideWidth[0];

        // right
        pbRight[0] = nGray;
        if (rcPos.right < pImage->GetWidth())
        {
            pbRight[1] = nGray;
        }
        pbRight += cImage.rgiStrideWidth[0];
    }

    // right
    memset(pbBottom, nGray, nRcWidth);
    if (rcPos.bottom < pImage->GetHeight() - 1)
    {
        memset(pbBottom + cImage.rgiStrideWidth[0], nGray, nRcWidth);
    }
}

HRESULT CSWRecognizeTGTransformFilter::OnRecognizePhoto(WPARAM wParam, LPARAM lParam)
{
	if( wParam == 0 && lParam == 0 )
	{
		return E_INVALIDARG;
	}
    CSWCarLeft* pCarLeft = (CSWCarLeft*)wParam;

    CSWImage* pImage = NULL;
    for(int i = 0; i < pCarLeft->GetImageCount() - 2; i++)
    {
        if( pCarLeft->GetImage(i) == NULL)
        {
            continue;
        }
        pImage = pCarLeft->GetImage(i);
        break;
    }
    if (NULL == pImage)
    {
        SW_TRACE_DEBUG("<RecognizeTransformFilter OnRecognizePhoto>No image, can't recognize!\n");
		return E_INVALIDARG;
    }

	CSWCarLeftArray *carLeft = (CSWCarLeftArray *)lParam;
	
	PROCESS_EVENT_STRUCT cEvent;

	// 判断是不是抓拍图。
	// 目前DSP端需要通过此标志来做处理。
	if( !pImage->IsCaptureImage() )
	{
		SW_TRACE_DEBUG("<RecognizeTransformFilter>Not captrue image, can't recognize!\n");
		return E_INVALIDARG;
	}

    // 提供抓拍识别区域
    INT iImageWidth = pImage->GetWidth();
    INT iImageHeight = pImage->GetHeight();
    FRAME_RECOGNIZE_PARAM CapRecognizeParam;
    HV_RECT rectVideoLastPalte = pCarLeft->GetLastPlateRect();
    CapRecognizeParam.rectCaptureRegArea.left = rectVideoLastPalte.left - 300 < 0 ? 0 : rectVideoLastPalte.left - 300;
    CapRecognizeParam.rectCaptureRegArea.top = rectVideoLastPalte.top - 300 < 0 ? 0 : rectVideoLastPalte.top - 300;
    CapRecognizeParam.rectCaptureRegArea.right = rectVideoLastPalte.right + 300 > iImageWidth ? iImageWidth : rectVideoLastPalte.right + 300;
    CapRecognizeParam.rectCaptureRegArea.bottom = iImageHeight / 2;//rectVideoLastPalte.bottom + 300 > iImageHeight / 2 ? iImageHeight / 2 : rectVideoLastPalte.bottom + 300;

    if (CapRecognizeParam.rectCaptureRegArea.top < (iImageHeight >> 2))
    {
        CapRecognizeParam.rectCaptureRegArea.top = (iImageHeight >> 2);
    }

    if(swpa_utils_file_exist("/tmp/draw.txt"))
    {
        DrawRect(pImage, CapRecognizeParam.rectCaptureRegArea);
    }

    if(swpa_utils_file_exist("/tmp/drawbest.txt"))
    {
        SW_RECT rcCapPos;
        pCarLeft->GetResultRect(2, &rcCapPos);
        rectVideoLastPalte.left = rcCapPos.left;
        rectVideoLastPalte.top = rcCapPos.top;
        rectVideoLastPalte.right = rcCapPos.right;
        rectVideoLastPalte.bottom = rcCapPos.bottom;
        DrawRect(pImage, rectVideoLastPalte);
    }

	DWORD dwNow = CSWDateTime::GetSystemTick();
    HRESULT hr = CSWBaseLinkCtrl::GetInstance()->ProcessOneFrame(pImage, &cEvent, &CapRecognizeParam);
	SW_TRACE_DEBUG("CSWBaseLinkCtrl::GetInstance()->ProcessOneFrame %d ms", CSWDateTime::GetSystemTick() - dwNow);
	if(swpa_utils_file_exist("./test.txt"))
	{
		cEvent.dwEventId = EVENT_CARLEFT;
		cEvent.iCarLeftInfoCount = 2;
	}
	
	if(S_OK == hr && (cEvent.dwEventId & EVENT_CARLEFT))
	{
		carLeft->SetCount(cEvent.iCarLeftInfoCount);
		for(int i = 0; i < cEvent.iCarLeftInfoCount; i++)
		{
			carLeft->Get(i)->SetParameter(m_pTrackerCfg, &cEvent.rgCarLeftInfo[i]);
			if(swpa_utils_file_exist("./test.txt"))
			{
				carLeft->Get(i)->SetPlateNo("Test");
				carLeft->Get(i)->SetCarColor(CC_WHITE);
			}
		}
	}
	return hr;
}

/**
*@brief 设置识别开关
*/
HRESULT CSWRecognizeTGTransformFilter::OnRecognizeGetJPEG(WPARAM wParam, LPARAM lParam)
{
	m_fSendJPEG = (BOOL)wParam;
	if(!m_fSendJPEG)
	{
		m_fSendDebug = FALSE;
	}
	SW_TRACE_DEBUG("<RecognizeTransformFilter>Set RecognizeEnable %d,JPEG:%d.\n", (UINT)wParam, m_fSendJPEG);
	return S_OK;
}

/**
*@brief 输出调试码流
*/
HRESULT CSWRecognizeTGTransformFilter::OnRecognizeOutPutDebug(WPARAM wParam, LPARAM lParam)
{
	m_fSendDebug = (BOOL)wParam;
	SW_TRACE_DEBUG("<RecognizeTransformFilter>Set RecognizeOutPutDebug %d.,Debug:%d\n", (UINT)wParam, m_fSendDebug);
	return S_OK;
}

HRESULT CSWRecognizeTGTransformFilter::OnTriggerEnable(WPARAM wParam, LPARAM lParam)
{
	SW_TRACE_DEBUG("OnTriggerEnable(0x%08x,0x%08x)", wParam, lParam);
	m_iCarArriveTriggerType = (INT)wParam;
	return S_OK;
}

HRESULT CSWRecognizeTGTransformFilter::OnIOEvent(WPARAM wParam, LPARAM lParam)
{
	m_iIOLevel = (INT)wParam;
	SW_TRACE_DEBUG("OnIOEvent(0x%08x,0x%08x)", wParam, lParam);
	return S_OK;
}

/**
 *@brief 动态修改识别参数
 */
HRESULT CSWRecognizeTGTransformFilter::OnModifyParam(WPARAM wParam, LPARAM lParam)
{
	TRACKER_CFG_PARAM* pCfgParam = (TRACKER_CFG_PARAM*)wParam;
	HRESULT hr = CSWBaseLinkCtrl::GetInstance()->ModifyVideoRecogerParam((*pCfgParam));
	if(FAILED(hr))
	{
        SW_TRACE_DEBUG("ModifyVideoRecogerParam failed, ret = 0x%X!\n", hr);
		return S_FALSE;
	}
	m_fModifyParam = TRUE;
	m_iPlateLightCheckCount = pCfgParam->nPlateLightCheckCount;
	//不支持动态修改
	//m_iExpectPlateLightMax = pCfgParam->nMaxPlateBrightness;
	//m_iExpectPlateLightMin = pCfgParam->nMinPlateBrightness;
    SW_TRACE_DEBUG("ModifyVideoRecogerParam OK!\n");
	return hr;
}

HRESULT CSWRecognizeTGTransformFilter::OnGetCarLeftCount(WPARAM wParam, LPARAM lParam)
{
	DWORD *pdwCarCount = (DWORD *)lParam;
	if (NULL == pdwCarCount)
	{
		return E_INVALIDARG;
	}
	*pdwCarCount = m_dwPlateCount;
	return S_OK;
}

HRESULT CSWRecognizeTGTransformFilter::Receive(CSWObject* obj)
{
	if(IsDecendant(CSWImage, obj))
	{
		CSWImage* pImage = (CSWImage*)obj;
		// 识别的时候不接收抓拍图
		if(pImage->IsCaptureImage())
		{
#if 0
			if(m_fSendJPEG)
			{
				CSWPosImage* pPosImage = new CSWPosImage(pImage, 0, m_fSendDebug);
				GetOut(0)->Deliver(pPosImage);
				pPosImage->Release();
			}
#else
			if (pImage->GetFlag() == 0 && !m_fSendJPEG) return S_OK;//H264模式下抓拍标记为0不需要等降噪帧
			//抓拍图也要先放入匹配队列进行匹配，然后再压缩JPEG
			PROC_QUEUE_ELEM OneElem;
            OneElem.pImage = pImage;
            OneElem.pData = NULL;
			m_cMutexMatch.Lock();
			if( !m_lstMatchImage.IsFull() )
			{
				pImage->AddRef();
                m_lstMatchImage.AddTail(OneElem);
				if( S_OK != m_cSemMatch.Post() )
				{
					SW_TRACE_DEBUG("<RecognizeTransformFilter>SemMatch post failed. list count:%d.\n", m_lstMatchImage.GetCount());
				}
			}
			else
			{
                PROC_QUEUE_ELEM DelElem = m_lstMatchImage.RemoveHead();
                SAFE_RELEASE(DelElem.pImage);
				pImage->AddRef();
                m_lstMatchImage.AddTail(OneElem);
			}
			m_cMutexMatch.Unlock();
			
#endif
			return S_OK;
		}
		else
		{
			static DWORD m_LastTime=0;
			m_iFrame++;
			DWORD m_NowTime=CSWDateTime::GetSystemTick();
			DWORD m_DiffTime=m_NowTime-m_LastTime;
			if(m_DiffTime>10000)
			{
				m_LastTime=m_NowTime;
				SW_TRACE_DEBUG("帧率:%d\n",m_iFrame);
				m_iFrame=0;
			}
		}
		
		// 测试协议等待队列有空闲空间
		// 默认VPIF采集的帧名都是“VPIF”。
		if(swpa_strcmp("VPIF", pImage->GetFrameName()) != 0 )
		{
			while( TRUE )
			{
				m_cMutexImage.Lock();
				if( !m_lstImage.IsFull())
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
		}
		//放入队列
		m_cMutexImage.Lock();
		if( !m_lstImage.IsFull() )
		{
			pImage->AddRef();
			m_lstImage.AddTail(pImage);
			if( S_OK != m_cSemImage.Post() )
			{
				SW_TRACE_DEBUG("<RecognizeTransformFilter>SemImage post failed. list count:%d.\n", m_lstImage.GetCount());
			}
		}
		else
		{
			CSWImage* pDelImage = m_lstImage.RemoveHead();
            SAFE_RELEASE(pDelImage);
			pImage->AddRef();
			m_lstImage.AddTail(pImage);
			// if( S_OK != m_cSemImage.Post() )
			// {
			// 	SW_TRACE_DEBUG("<RecognizeTransformFilter>SemImage post failed. list count:%d.\n", m_lstImage.GetCount());
			// }
		}
		m_cMutexImage.Unlock();
	}

	return S_OK;
}

HRESULT CSWRecognizeTGTransformFilter::OnProcess()
{
	SW_TRACE_DEBUG("CSWRecognizeTGTransformFilter OnProcess run.\n");

	DWORD dwFps = 0;
	DWORD dwBeginTick = CSWDateTime::GetSystemTick();
	m_fModifyParam = TRUE;
    INT nLastEnvPeriod = -1;

	while(S_OK == m_pThread->IsValid() && GetState() == FILTER_RUNNING)
	{
		if( S_OK != m_cSemImage.Pend(1000) )
		{
			SW_TRACE_DEBUG("Image Semaphore Pend failed.");
			continue;
		}
		m_cMutexImage.Lock();
		if( m_lstImage.IsEmpty() )
		{
			SW_TRACE_DEBUG("ImageList is empty.");
			m_cMutexImage.Unlock();
			continue;
		}
		
		//SW_TRACE_DEBUG("m_fEnable=%d,m_fOutPutDebug=%d", m_fEnable, m_fOutPutDebug);
		CSWImage* pImage = m_lstImage.RemoveHead();
		m_cMutexImage.Unlock();

		int iRectCount = 0;
		int iDataSize = 1024 * 32;
		unsigned char* pbData = new unsigned char[iDataSize];
		if (NULL == pbData)
        {
            SAFE_RELEASE(pImage);
            continue;
        }

        //< ARM端算法处理调用
        if(NULL != pImage && m_fModifyParam )
        {
            // 需换成场图
            m_cAlgDetCtrl.Init(m_pTrackerCfg, pImage->GetWidth(), pImage->GetHeight() / 2);
            m_fModifyParam = FALSE;
            SAFE_RELEASE(pImage);
            delete [] pbData;
            continue;
        }
        if( !m_fSendJPEG || m_fSendDebug )
        {
            // arm alg proc

            DWORD dwArmProcess = CSWDateTime::GetSystemTick();

            SW_COMPONENT_IMAGE cImageCop;
            pImage->GetImage(&cImageCop);

            HV_COMPONENT_IMAGE hvImageYuv;

            hvImageYuv.nImgType = HV_IMAGE_YUV_420;

            // 需换成场图
            hvImageYuv.iHeight = (pImage->GetHeight() / 2);
            hvImageYuv.iWidth = pImage->GetWidth();

            hvImageYuv.iStrideWidth[0] = (cImageCop.rgiStrideWidth[0] << 1);
            hvImageYuv.iStrideWidth[1] = (cImageCop.rgiStrideWidth[1] << 1);
            hvImageYuv.iStrideWidth[2] = 0;

            hvImageYuv.rgImageData[0].addr = cImageCop.rgpbData[0];
            hvImageYuv.rgImageData[1].addr = cImageCop.rgpbData[1];
            hvImageYuv.rgImageData[2].addr = 0;

            // todo huanggr 第二个参数需要传递ARM端的场景状态，即原EC500IR结构体PROCESS_ONE_FRAME_PARAM中的iEnvStatus变量
            if ( 0 )
            {
                static INT s_iFrameCount = 0;
                if (s_iFrameCount++ >= 50)
                {
                    s_iFrameCount = 0;
                    m_cAlgDetCtrl.SetLightType(m_nEnvLightType, 2);
                    SendMessage(MSG_RECOGNIZE_ENVPERIOD_CHANGED,(WPARAM)2,0);
                    SW_TRACE_DEBUG("CSWRecognizeTGTransformFilter::OnProcess<Just for test SetLightType: %d(3:day, 2:dusk, 1:night)******************* >.\n", 2);

                    m_nEnvFromM3 = 2;
                }
            }
            else
            {
                static INT s_iFrameCount = 0;
                if (s_iFrameCount++ >= 50)
                {
                    s_iFrameCount = 0;
                    INT nStatus = -1;
                    SendMessage(MSG_GET_M3_DAYNIGHT_STATUS, 0, (LPARAM)&nStatus);
                    SW_TRACE_DEBUG("Get M3 EnvLightType: %d(3:day, 2:dusk, 1:night)******************* >.\n", nStatus);
                    //if (nLastEnvPeriod != nStatus)
                    {
                        m_nEnvFromM3 = nStatus;
                        m_cAlgDetCtrl.SetLightType(m_nEnvLightType, nStatus);
                        //SendMessage(MSG_RECOGNIZE_ENVPERIOD_CHANGED,(WPARAM)nStatus,0);
                        SW_TRACE_DEBUG("CSWRecognizeTGTransformFilter::OnProcess< SetLightType: %d(3:day, 2:dusk, 1:night)******************* >.\n", nStatus);
                        nLastEnvPeriod = nStatus;
                    }
                }
            }


		static int nn = 0;
            HRESULT hh = m_cAlgDetCtrl.Process(hvImageYuv,
                pbData, iDataSize, &iDataSize);

            DWORD dwArmProcessEnd = CSWDateTime::GetSystemTick();
            if( dwArmProcessEnd - dwArmProcess > 100 )
            {
                SW_TRACE(7, "*****************  ms:%d.   %d", dwArmProcessEnd - dwArmProcess, nn++);
            }
        }  
        //> ARM端算法处理调用

        // 测试协议不丢帧。
        if( swpa_strcmp("VPIF", pImage->GetFrameName()) != 0 )
        {
            while(TRUE)
            {
				m_cMutexProcQueue.Lock();
				if( !m_lstProcQueueList.IsFull() )
				{
					m_cMutexProcQueue.Unlock();
					break;
				}
				else
				{
					m_cMutexProcQueue.Unlock();
					CSWApplication::Sleep(200);
				}
			}
            //SW_TRACE_NORMAL("== PROC %s =========\n", pImage->GetFrameName());
            //SW_TRACE_DSP("== PROC %s =========\n", pImage->GetFrameName());
		}

		m_cMutexProcQueue.Lock();
		if (m_lstProcQueueList.IsFull())	//DSP处理不过来，选择性丢掉部分ARM端的检测结果和帧
		{
			//SW_TRACE_NORMAL("==================== ProcQueueList is full!!!");
			SW_POSITION pos = m_lstProcQueueList.GetHeadPosition();
			SW_POSITION lastpos = pos;
			INT index = 0;
			int iDeleteCount = 0;
			while(m_lstProcQueueList.IsValid(pos))
			{
				lastpos = pos;
				PROC_QUEUE_ELEM cElem = m_lstProcQueueList.GetNext(pos);
				//		if(  index % 3 == 2 )
				{
					m_lstProcQueueList.RemoveAt(lastpos);
					SAFE_RELEASE(cElem.pImage);
					PROCESS_EVENT_STRUCT* pProcessEvent =
						(PROCESS_EVENT_STRUCT*)cElem.pData;
					if( pProcessEvent != NULL
							&& NULL != pProcessEvent->cSyncDetData.pbData )
					{
						delete[] pProcessEvent->cSyncDetData.pbData;
						pProcessEvent->cSyncDetData.pbData = NULL;
					}
					SAFE_DELETE(pProcessEvent);
					++iDeleteCount;
				}
				++index;
			}

			//SW_TRACE_DEBUG("Arm process delete count:%d.", iDeleteCount);

			if( iDeleteCount == 0 )
			{
				if (pbData != NULL)
				{
					delete[] pbData;
					pbData = NULL;
				}
				//释放资源
				SAFE_RELEASE(pImage);
				m_cMutexProcQueue.Unlock();
				continue;
			}
		}

		 // 创建元素空间
		PROCESS_EVENT_STRUCT* pProcessEvent = new PROCESS_EVENT_STRUCT;
		if (pProcessEvent == NULL)
		{
			SW_TRACE_DEBUG("Sync queue out of mem.");
			delete[] pbData;
			SAFE_RELEASE(pImage);
			m_cMutexProcQueue.Unlock();
			continue;
		}

		swpa_memset(pProcessEvent, 0, sizeof(PROCESS_EVENT_STRUCT));
		PROC_QUEUE_ELEM cNewElem;
		pImage->AddRef();
		cNewElem.pImage = pImage;
		cNewElem.pData = pProcessEvent;
		pProcessEvent->cSyncDetData.pbData = pbData;
		pProcessEvent->cSyncDetData.nLen = iDataSize;
		//pProcessEvent->cSyncDetData.nEleCnt = iRectCount;
		m_lstProcQueueList.AddTail(cNewElem);
		m_cSemProcQueue.Post();
		m_cMutexProcQueue.Unlock();

		//释放资源
		SAFE_RELEASE(pImage);
		
		// print fps
		if( dwFps++ >= 50 )
		{
			DWORD dwCurTick = CSWDateTime::GetSystemTick();
			CHAR szInfo[256] = {0};
			CHAR szMsg[256] = {0};
			swpa_sprintf(szInfo, "ARM recognize fps: %.1f JPEG:%d,Debug:%d.", float(50*1000) / (dwCurTick - dwBeginTick), m_fSendJPEG, m_fSendDebug);
			SW_TRACE_NORMAL("--- %s---\n", szInfo);
			//swpa_sprintf(szMsg, "识别:%s", szInfo);
			//CSWMessage::SendMessage(MSG_APP_UPDATE_STATUS, (WPARAM)szMsg, 0);
			dwBeginTick = dwCurTick;
			dwFps = 0;
		}

	}

	SW_TRACE_DEBUG("RecognizeTransformFilter OnProcess exit.\n");
	return S_OK;
}

HRESULT CSWRecognizeTGTransformFilter::OnProcessSync()
{
	SW_TRACE_DEBUG("CSWRecognizeTGTransformFilter OnProcessSync run.\n");

	CSWPosImage* pPosImage = NULL;
	CSWCameraDataPDU* pCamPDU = NULL;

	DWORD dwFps = 0;
	DWORD dwBeginTick = CSWDateTime::GetSystemTick();
	INT iPulseLevel = m_iPulseWidthLevel;
	BOOL fIsARMCheckNight = FALSE;
	DWORD dwFrameCount = 0;
	INT nLastEnvPeriod = -1;
	BOOL fUpdate=FALSE;

	while(S_OK == m_pProcQueueThread->IsValid() && GetState() == FILTER_RUNNING)
	{
		if (S_OK != m_cSemProcQueue.Pend(1000))
		{
			SW_TRACE_DEBUG("ProcQueue Semaphore Pend failed!");
			continue;
		}

		m_cMutexProcQueue.Lock();
		if (m_lstProcQueueList.IsEmpty())
		{
			m_cMutexProcQueue.Unlock();
			SW_TRACE_DEBUG("ProcQueueList is empty!");
			continue;
		}

		PROC_QUEUE_ELEM cQueueElem;
		cQueueElem = m_lstProcQueueList.RemoveHead();
		m_cMutexProcQueue.Unlock();

		CSWImage* pImage = cQueueElem.pImage;
		PROCESS_EVENT_STRUCT* pProcessEvent = (PROCESS_EVENT_STRUCT*)(cQueueElem.pData);

		if(!m_fSendJPEG || m_fSendDebug)
		{
//			if (nLastEnvPeriod != m_nCurEnvPeriod)
//			{
//			    if (m_nCurEnvPeriod == 1) //白天切换到晚上时，强制让脉宽等级从4开始调
//			    {
//			        iPulseLevel = DEF_PULSE_WIDTH;
//			        ResetAdjustPulseWidthInfo(iPulseLevel);
//			        fIsARMCheckNight = TRUE;
//			    }
//			    else
//			    {
//			        iPulseLevel = 0;
//			        fIsARMCheckNight = FALSE;
//			    }
//			    nLastEnvPeriod = m_nCurEnvPeriod;
//			}

			pProcessEvent->fIsCheckLight = true;
			pProcessEvent->iCarArriveTriggerType = m_iCarArriveTriggerType;
			pProcessEvent->nIOLevel = m_iIOLevel;
			pProcessEvent->nEnvLightType = (fIsARMCheckNight ? 2 : 0);

            FRAME_RECOGNIZE_PARAM recogParam;
            recogParam.iEnvStatus = m_nEnvFromM3;

            // 日志输出
            static INT s_iFrameCount = 0;
            if (s_iFrameCount++ >= 50)
            {
                s_iFrameCount = 0;
                SW_TRACE_DEBUG("Before processOneFrame, EnvStatus Frome Main : %d(3:day, 2:dusk, 1:night)", recogParam.iEnvStatus);
            }

			DWORD dwTmpCurTick = CSWDateTime::GetSystemTick();
			static DWORD s_dwTotalTick = 0;
			static DWORD s_dwMaxTick = 0;
			static char s_szTemp[256] = {0};

            HRESULT hr = CSWBaseLinkCtrl::GetInstance()->ProcessOneFrame(pImage, pProcessEvent, &recogParam);

            dwTmpCurTick = CSWDateTime::GetSystemTick() - dwTmpCurTick;
            s_dwTotalTick += dwTmpCurTick;
            if (dwTmpCurTick > s_dwMaxTick)
            {
            	s_dwMaxTick = dwTmpCurTick;
            	strcpy(s_szTemp, pImage->GetFrameName());
            }
            if (strlen(s_szTemp) <= 0)
            {
            	strcpy(s_szTemp, "NULL");
            }
			SW_TRACE(7, "xxxxxxxxxxxxxxx    %d_%u, max = %d_%s", dwTmpCurTick,
					s_dwTotalTick,
					s_dwMaxTick,
					s_szTemp
					);

			if(S_OK == hr)
			{
				if ( pProcessEvent->dwEventId & EVENT_CARARRIVE )
				{
					SW_TRACE_DEBUG("EVENT_CARARRIVE %d\n",pProcessEvent->iCarArriveInfoCount);
					for( int i = 0; i < pProcessEvent->iCarArriveInfoCount; ++i )
					{	 
						CarArriveEvent(&pProcessEvent->rgCarArriveInfo[i]);
					}
				}
				if( pProcessEvent->dwEventId & EVENT_CARLEFT )
				{
					SW_TRACE_DEBUG("EVENT_CARLEFT %d\n",pProcessEvent->iCarArriveInfoCount);
					for( int i = 0; i < pProcessEvent->iCarLeftInfoCount; ++i )
					{
						//有车牌,而且是ARM判断为晚上才计入统计
						//if (pProcessEvent->rgCarLeftInfo[i].cCoreResult.rgbContent[0] != 0)
						//	&& fIsARMCheckNight)
						//{
							iPulseLevel = AdjustPulseWidth(pProcessEvent->rgCarLeftInfo[i].cCoreResult.iCarAvgY);
						//}
	
						pProcessEvent->rgCarLeftInfo[i].cCoreResult.iPulseLevel = iPulseLevel;//生成结果附件信息XML时用到
						CarLeftEvent(&pProcessEvent->rgCarLeftInfo[i]);
					}
				}

				for (int i = 0; i < MAX_EVENT_COUNT;i++)
				{
					if (m_rgiAllCarTrigger[i] != pProcessEvent->rgiAllCarTrigger[i])
					{
						fUpdate = TRUE;
					}
					m_rgiAllCarTrigger[i] = pProcessEvent->rgiAllCarTrigger[i];
				}
				if(fUpdate)
				{
					//通知更新当前还需要保留还些抓拍图
					CSWMessage::SendMessage(MSG_UPDATE_ALL_TRIGGER, (WPARAM)m_rgiAllCarTrigger, 0);
				}
				fUpdate=FALSE;

#if 0
                TG_DET_API_RESULT_HEAD* pHead = (TG_DET_API_RESULT_HEAD*)pProcessEvent->cSyncDetData.pbData;
                DET_ROI *pRect = (DET_ROI*)(pProcessEvent->cSyncDetData.pbData + sizeof(TG_DET_API_RESULT_HEAD));
                if (pHead->nDetROICnt > 20) pHead->nDetROICnt = 20;
                for(int i = 0; i < pHead->nDetROICnt; i++)
                {
                    pProcessEvent->cTrackRectInfo.rgTrackRect[i] = CRect(
                                pRect[i].m_nLeft,
                                pRect[i].m_nTop,
                                pRect[i].m_nRight,
                                pRect[i].m_nBottom);
                }
                pProcessEvent->cTrackRectInfo.dwTrackCount = pHead->nDetROICnt;
#endif

				//摄像机参数
				if (!(swpa_strcmp("VPIF", pImage->GetFrameName()) != 0))
				{
                    pCamPDU = new CSWCameraDataPDU();
                    pCamPDU->SetLightType(pProcessEvent->cLightType);	//相机等级
                    pCamPDU->SetCplStatus(pProcessEvent->iCplStatus);	//偏光镜状态
                    pCamPDU->SetPluseLevel(iPulseLevel/*pProcessEvent->iPulseLevel*/);	//频闪补光脉宽
                    pCamPDU->SetWDRLevel(pProcessEvent->nWDRLevel);
                    GetOut(2)->Deliver(pCamPDU);
                    SAFE_RELEASE(pCamPDU);
				}

                static INT s_iLastEnvLightType = -1;
                // DSP信息反馈
                m_nEnvLightType = pProcessEvent->nEnvLightType; // 0白天，1傍晚，2晚上
                static INT s_iCountTemp = 0;
                if (++ s_iCountTemp > 50)
                {
                    s_iCountTemp = 0;
                    SW_TRACE_DEBUG("EnvLightType from dsp : %d (0:day, 1:dusk, 2:night)\n", m_nEnvLightType);
                }
                if (s_iLastEnvLightType != m_nEnvLightType)
                {
                    INT nStatus = -1;
                    nStatus = m_nEnvLightType == 0 ? 3 : 1;
                    //SendMessage(MSG_RECOGNIZE_ENVPERIOD_CHANGED,(WPARAM)nStatus,0);
                    SW_TRACE_DEBUG("EnvLightType changed from dsp : %d (0:day, 1:dusk, 2:night)\n", m_nEnvLightType);
                }
                s_iLastEnvLightType = m_nEnvLightType;
			}
			else
			{
				SW_TRACE_DEBUG("<RecognizeTransformFilter>ProceeOneFrame failed. 0x%08x.\n", hr);
				// 如果是DSP超时,则复位设备.
				if( hr == E_UNEXPECTED )
				{
					CSWMessage::SendMessage(MSG_APP_RESETDEVICE, 0, 0);
				}
			}
		}
		else if (!fUpdate)
		{
			//切换为JPEG码流时，发送一次更新标记
			swpa_memset(m_rgiAllCarTrigger, 0, sizeof(m_rgiAllCarTrigger));
			CSWMessage::SendMessage(MSG_UPDATE_ALL_TRIGGER, (WPARAM)m_rgiAllCarTrigger, 0);
			fUpdate = TRUE;
		}

		// print fps
		if( dwFps++ >= 50 )
		{
			DWORD dwCurTick = CSWDateTime::GetSystemTick();
			CHAR szInfo[256] = {0};
			CHAR szMsg[256] = {0};
            swpa_sprintf(szInfo, "DSP recognize fps: %.1f JPEG:%d,Debug:%d, EnvLightType for dsp : %d (0:day, 1:dusk, 2:night).",
                         float(50*1000) / (dwCurTick - dwBeginTick), m_fSendJPEG, m_fSendDebug, m_nEnvLightType);

			SW_TRACE_NORMAL("--- %s---\n", szInfo);

			swpa_sprintf(szMsg, "识别:%s", szInfo);
			CSWMessage::SendMessage(MSG_APP_UPDATE_STATUS, (WPARAM)szMsg, 0);

			dwBeginTick = dwCurTick;
			dwFps = 0;
		}

		//放入匹配队列
		//放入队列
		m_cMutexMatch.Lock();
		if( !m_lstMatchImage.IsFull() )
		{
            cQueueElem.pImage->AddRef();
            m_lstMatchImage.AddTail(cQueueElem);
			if( S_OK != m_cSemMatch.Post() )
			{
				SW_TRACE_DEBUG("<RecognizeTransformFilter>SemMatch post failed. list count:%d.\n", m_lstMatchImage.GetCount());
			}
		}
		else
		{
            PROC_QUEUE_ELEM DelElem = m_lstMatchImage.RemoveHead();
            SAFE_RELEASE(DelElem.pImage);
			PROCESS_EVENT_STRUCT* pDelProcessEvent = (PROCESS_EVENT_STRUCT*)(DelElem.pData);
			if(NULL != pDelProcessEvent && NULL != pDelProcessEvent->cSyncDetData.pbData )
        	{
            	delete[] pDelProcessEvent->cSyncDetData.pbData;
            	pDelProcessEvent->cSyncDetData.pbData = NULL;
        	}
        	SAFE_DELETE(pDelProcessEvent);
            cQueueElem.pImage->AddRef();
            m_lstMatchImage.AddTail(cQueueElem);
		}
		m_cMutexMatch.Unlock();

		//释放资源
		SAFE_RELEASE(pImage);
		
//		if(NULL != pProcessEvent && NULL != pProcessEvent->cSyncDetData.pbData )
//        {
//        	delete[] pProcessEvent->cSyncDetData.pbData;
//        	pProcessEvent->cSyncDetData.pbData = NULL;
//        }
//        SAFE_DELETE(pProcessEvent);
		
	}
	
	SW_TRACE_DEBUG("CSWRecognizeTGTransformFilter OnProcessSync exit.\n");
	return S_OK;
}

HRESULT CSWRecognizeTGTransformFilter::OnMatchImage()
{
	SW_TRACE_DEBUG("CSWRecognizeTGTransformFilter OnMatchImage run.\n");

	CSWPosImage* pPosImage = NULL;

	DWORD dwFps = 0;
	DWORD dwBeginTick = CSWDateTime::GetSystemTick();

	while(S_OK == m_pMatchThread->IsValid() && GetState() == FILTER_RUNNING)
	{
		//debug 
		//swpa_thread_sleep_ms(1000);
		//continue;
		
		if (S_OK != m_cSemMatch.Pend(1000))
		{
			SW_TRACE_DEBUG("OnMatchImage Semaphore Pend failed!");
			//continue;
		}

		m_cMutexMatch.Lock();
		if (m_lstMatchImage.IsEmpty())
		{
			m_cMutexMatch.Unlock();
			SW_TRACE_DEBUG("OnMatchImage list is empty!");
			continue;
		}

        PROC_QUEUE_ELEM OneElem;
        OneElem = m_lstMatchImage.RemoveHead();
		m_cMutexMatch.Unlock();
        CSWImage* pImage;
        pImage = OneElem.pImage;
        PROCESS_EVENT_STRUCT* pProcessEvent = (PROCESS_EVENT_STRUCT*)(OneElem.pData);


		//816
		if (SENSOR_ICX816 == swpa_get_sensor_type())
		{	
			if (S_OK != pImage->ReplaceFrameBuffer(CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY)))
			{
				SW_TRACE_NORMAL("Replace frame buffer failed! release the image\n");

				//释放资源
				if(pImage->GetLastImage())
				{
					pImage->Clear();
				}

		        if(NULL != pProcessEvent && NULL != pProcessEvent->cSyncDetData.pbData )
		        {
		            delete[] pProcessEvent->cSyncDetData.pbData;
		            pProcessEvent->cSyncDetData.pbData = NULL;
		        }
		        SAFE_DELETE(pProcessEvent);

		        SAFE_RELEASE(pImage);
		
				continue;
			}
			
		}

        if (m_fSendDebug || m_fSendJPEG)
        {
            if(NULL != pProcessEvent && pProcessEvent->cTrackRectInfo.dwTrackCount > 0)
            {
                pPosImage = new CSWPosImage(pImage, pProcessEvent->cTrackRectInfo.dwTrackCount, m_fSendDebug);
                for(int i = 0; i < pPosImage->GetCount(); i++)
                {
                    pProcessEvent->cTrackRectInfo.rgTrackRect[i].top *= 2;
                    pProcessEvent->cTrackRectInfo.rgTrackRect[i].bottom *= 2;
                    pPosImage->GetRect(i) = *(SW_RECT *)&pProcessEvent->cTrackRectInfo.rgTrackRect[i];
                    SW_TRACE_DEBUG("SendDebug <%d><%d,%d,%d,%d>.\n", pProcessEvent->cTrackRectInfo.dwTrackCount,
                       pProcessEvent->cTrackRectInfo.rgTrackRect[i].left,pProcessEvent->cTrackRectInfo.rgTrackRect[i].right,
                        pProcessEvent->cTrackRectInfo.rgTrackRect[i].top,pProcessEvent->cTrackRectInfo.rgTrackRect[i].bottom);
                }
            }
            else
            {
                if(NULL == pPosImage)
                {
                    pPosImage = new CSWPosImage(pImage, 0, m_fSendDebug);
                }
            }
            GetOut(0)->Deliver(pPosImage);
        }
		
		SAFE_RELEASE(pPosImage);
		//释放资源
		if(pImage->GetLastImage())
		{
			pImage->Clear();
		}

        if(NULL != pProcessEvent && NULL != pProcessEvent->cSyncDetData.pbData )
        {
            delete[] pProcessEvent->cSyncDetData.pbData;
            pProcessEvent->cSyncDetData.pbData = NULL;
        }
        SAFE_DELETE(pProcessEvent);

		
        SAFE_RELEASE(pImage);
		
	}
	

	SW_TRACE_DEBUG("CSWRecognizeTGTransformFilter OnMatchImage exit.\n");
	return S_OK;
}

HRESULT CSWRecognizeTGTransformFilter::CarArriveEvent(CARARRIVE_INFO_STRUCT *pCarArriveInfo)
{
	//todo..
	return S_OK;
}

HRESULT CSWRecognizeTGTransformFilter::CarLeftEvent(CARLEFT_INFO_STRUCT *pCarLeftInfo)
{
	m_dwPlateCount++;
	//SW_TRACE_DEBUG("<RecognizeTransformFilter>car:%d.\n", m_dwPlateCount);


	if(NULL == pCarLeftInfo)
	{
		return E_INVALIDARG;
	}
	//通知Graph构造CarLeft对象
	CSWCarLeft* pCarLeft = NULL;
	SendCommand(1, (WPARAM)&pCarLeft, (LPARAM)pCarLeftInfo);
	if( pCarLeft == NULL || !IsDecendant(CSWCarLeft, pCarLeft))
	{
		SAFE_RELEASE(pCarLeft);
		SW_TRACE_DEBUG("pCarLeft == NULL or is not CSWCarLeft object, used default CSWCarLeft\n");
		pCarLeft = new CSWCarLeft();
	}
	pCarLeft->SetParameter(m_pTrackerCfg, pCarLeftInfo);

	if(NULL != pCarLeft)
	{
        GB28181Alarm((LPCSTR)pCarLeft->GetPlateNo());
		CSWString s;
		switch(pCarLeftInfo->cCoreResult.nVoteCondition)
		{
		case NEW2END : s = "NEW状态下跟踪丢失";break;
		case LEAVE_TRACK_RECT: s = "高速模式下车牌离开跟踪区域";break;
		case TRACK_MISS: s = "在跟踪区域内跟踪丢失";break;
		case ENDRECT_MISS: s = "在跟踪区域外跟踪丢失";break;
		case MAX_EQUAL_FRAME: s = "连续识别结果相同";break;
		case MAX_VOTE_FRAME: s = "有效结果超过阈值";break;
		}
		SW_TRACE_NORMAL("object carleft name:%s,PlateNo:%s"
			"[0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x][%s]", 
			pCarLeft->Name(), (LPCSTR)pCarLeft->GetPlateNo(), 
			pCarLeft->GetImage(0), pCarLeft->GetImage(1), pCarLeft->GetImage(2), 
			pCarLeft->GetImage(3), pCarLeft->GetImage(4), pCarLeft->GetImage(5), 
			pCarLeft->GetImage(6), (LPCSTR)s);

        //HV_RECT pLastRect = pCarLeft->GetLastPlateRect();
        /*SW_TRACE_DEBUG("PlateNo:%s, LastRect <left = %d, top = %d, right = %d, bottom = %d> .\n",
                       (LPCSTR)pCarLeft->GetPlateNo(), pLastRect.left, pLastRect.top, pLastRect.right, pLastRect.bottom);*/

		if (pCarLeft->GetReverseRun()
			&& GetReverseRunFilterFlag())
		{
			SW_TRACE_DEBUG("Info: ReverseRun. Discard this carleft obj.\n");
		}
		else
		{
		    if (pCarLeft->GetRoadNo() < 0)
            {
                pCarLeft->SetRoadNo(0);
            }
		    GetOut(1)->Deliver(pCarLeft);
		}
		
		//GetOut(3)->Deliver(pCarLeft->GetImage(PLATE_IMAGE));
		pCarLeft->Release();
		SW_TRACE_DEBUG("object carleft done.\n");
	}
	return S_OK;
}
HRESULT CSWRecognizeTGTransformFilter::GB28181Alarm(LPCSTR szMsg)
{
    //printf("%s %d\n",__FUNCTION__,__LINE__);
    GB28181_AlarmInfo info;
    swpa_strcpy(info.szAlarmMsg,szMsg);
    SWPA_DATETIME_TM tm;
    CSWDateTime dt;
    dt.GetTime(&tm);
    swpa_sprintf(info.szAlarmTime,"%d-%02d-%02dT%02d:%02d:%02d.%03d",tm.year,tm.month,tm.day,tm.hour,tm.min,tm.sec,tm.msec);
    //SW_TRACE_NORMAL("CSWRecognizeTGTransformFilter::OnSendAlarm %s %s %d ",info.szAlarmTime,info.szAlarmMsg,MSG_GB28181_SEND_ALARM);
    SendRemoteMessage(MSG_GB28181_SEND_ALARM,&info,sizeof(info));
    return S_OK;
}

INT CSWRecognizeTGTransformFilter::AdjustPulseWidth(INT iPlateLight)
{
	m_iPlateLightSum += iPlateLight;
	m_iPlateCount++;
	if (iPlateLight < m_iPlateLightMin)
	{
		m_iPlateLightMin = iPlateLight;
	}
	else if (iPlateLight > m_iPlateLightMax)
	{
		m_iPlateLightMax = iPlateLight;
	}
	
#define MIN_VALUE(x, y) ((x) > (y) ? (y) : (x))
#define MAX_VALUE(x, y) ((x) > (y) ? (x) : (y))

	//统计一定车牌个数后计算亮度平均值，再判断
	if (m_iPlateCount >= m_iPlateLightCheckCount
		&& m_iPlateCount > 2)
	{
		INT iPlateLightAvg = (m_iPlateLightSum - m_iPlateLightMin - m_iPlateLightMax)/(m_iPlateCount -2);
		
		if (iPlateLightAvg > m_iExpectPlateLightMax 
			&& m_iPulseWidthLevel > PULSE_WIDTH_LEVEL_MIN)
		{
			m_iPulseWidthLevel = MAX_VALUE((m_iPulseWidthLevel - m_iDownStep),PULSE_WIDTH_LEVEL_MIN);

			
			SW_TRACE_NORMAL("==Plate Light Avg:%d,count:%d,Exp(%d~%d),Level:%d,Step:%d==",
				iPlateLightAvg,m_iPlateCount,m_iExpectPlateLightMin,m_iExpectPlateLightMax,
				m_iPulseWidthLevel,m_iDownStep);
			
			//m_iDownStep++;//容易调得太低
			m_iUpStep = 1;
		}
		else if (iPlateLightAvg < m_iExpectPlateLightMin 
			&& m_iPulseWidthLevel < PULSE_WIDTH_LEVEL_MAX)
		{			
			m_iPulseWidthLevel = MIN_VALUE((m_iPulseWidthLevel + m_iUpStep),PULSE_WIDTH_LEVEL_MAX);

			SW_TRACE_NORMAL("==Plate Light Avg:%d,count:%d,Exp(%d~%d),Level:%d,Step:%d==",
				iPlateLightAvg,m_iPlateCount,m_iExpectPlateLightMin,m_iExpectPlateLightMax,
				m_iPulseWidthLevel,m_iUpStep);

			//m_iUpStep++;//一步一步调节
			m_iDownStep = 1;
		}
		else
		{
			m_iUpStep = 1;
			m_iDownStep = 1;
			//debug info
			SW_TRACE_NORMAL("==Plate Light Avg:%d,count:%d,Exp(%d~%d),Level:%d ==",
				iPlateLightAvg,m_iPlateCount,m_iExpectPlateLightMin,m_iExpectPlateLightMax,
				m_iPulseWidthLevel);
		}
		//clear
		m_iPlateLightSum = 0;
		m_iPlateCount = 0;
		m_iPlateLightMax = INT_MIN;
		m_iPlateLightMin = INT_MAX;
		
	}

	// 晚上强制脉宽最等级为1
	if (m_nCurEnvPeriod == 1 && m_iPulseWidthLevel == 0)
	{
	    m_iPulseWidthLevel = 1;
	}

	return m_iPulseWidthLevel;
}

VOID CSWRecognizeTGTransformFilter::ResetAdjustPulseWidthInfo(INT iDefPulseWidth)
{
	m_iPlateCount = 0;
	m_iPlateLightSum = 0;
	m_iPlateLightMax = INT_MIN;
	m_iPlateLightMin = INT_MAX;
	m_iPulseWidthLevel = iDefPulseWidth;
}

/**
 *@brief 白天晚上状态变换
 *@brief wParam: 3表示白天，2表示傍晚，3表示晚上
 */
HRESULT CSWRecognizeTGTransformFilter::OnEnvPeriodChanged(WPARAM wParam, LPARAM lParam)
{
    m_nCurEnvPeriod = (INT)wParam;

    /*INT iOutputCtrlEnable = (wParam == 3) ? 0 : 1;
	if(FAILED(CSWMessage::SendMessage(MSG_COM_OUTPUT_CTRL_ENABLE, (WPARAM)iOutputCtrlEnable, 0)))
	{
		CSWMessage::SendMessage(MSG_COM_OUTPUT_CTRL_ENABLE, (WPARAM)iOutputCtrlEnable, 0);
	}*/
    return S_OK;
}

HRESULT CSWRecognizeTGTransformFilter::OnGetDspEnvLightType(WPARAM wParam, LPARAM lParam)
{
    INT *piEnvLightType = (INT *)lParam;
    *piEnvLightType = m_nEnvLightType;

    return S_OK;
}
