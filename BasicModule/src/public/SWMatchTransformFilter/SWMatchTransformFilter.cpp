#include "swpa.h"
#include "SWFC.h"
#include "SWMessage.h"
#include "SWCarLeft.h"
#include "SWMatchTransformFilter.h"
#include "SWCSRIKRadar.h"
#include "SWJVDCoilDevice.h"
#include "SWKNDRGDevice.h"

#include "SWRAPIERRadar.h"
#include "SWJZRadar.h"
#include "SWCOMTestDevice.h"

#define IMAGE_COUNT   5
#define SIGNAL_COUNT  5
#define CARLEFT_COUNT 5
#define OUTPUT_COUNT 5

	CSWMatchTransformFilter::CSWMatchTransformFilter()
	:CSWBaseFilter(2, 1)
	,CSWMessage(MSG_COM_TEST_BEGIN, MSG_COM_TEST_END)
	,m_pTrackerCfg(NULL)
	 ,m_fInitialize(FALSE)
	 ,m_fRadarTrigger(TRUE)
{

	GetIn(0)->AddObject(CLASSID(CSWImage));
	GetIn(1)->AddObject(CLASSID(CSWCarLeft));
	GetOut(0)->AddObject(CLASSID(CSWCarLeft));
	m_semMatch.Create(0, 1);
	m_semOutput.Create(0, 1);
	for(int i = 0; i < sizeof(m_pDevice)/sizeof(m_pDevice[0]); i++)
	{
		m_pDevice[i] = NULL;
	}

	m_pComTestDevic = NULL;
	m_iTESTINGCOMID = -1;
	//    m_iOutputThreadRestartTime = 0;
}

CSWMatchTransformFilter::~CSWMatchTransformFilter(){

	while(!m_cCarLeftList.IsEmpty() )
	{
		CARLEFT_MATCH_INFO* pCarLeftInfo = m_cCarLeftList.RemoveHead();
		if( pCarLeftInfo != NULL )
		{
			SAFE_RELEASE(pCarLeftInfo->pCarLeft);
			SAFE_DELETE(pCarLeftInfo);
		}
	}

	while(!m_cOutputList.IsEmpty())
	{
		CSWCarLeft* pCarLeft = m_cOutputList.RemoveHead();
		SAFE_RELEASE(pCarLeft);
	}

	for(int i = 0; i < sizeof(m_pDevice)/sizeof(m_pDevice[0]); i++)
	{
		SAFE_RELEASE(m_pDevice[i]);
	}

	SAFE_RELEASE(m_pComTestDevic);
}

HRESULT CSWMatchTransformFilter::Initialize(PVOID pvParam1, PVOID pvParam2,BOOL fRadarTrigger){

	Close();
	CSWAutoLock aLock(&m_cMutex);
	swpa_memcpy(&m_cParam, pvParam1, sizeof(MATCHPARAM));
	swpa_memcpy(&m_cDevParam, pvParam2, 2*sizeof(DEVICEPARAM));
	m_fRadarTrigger=fRadarTrigger;
	for(int i = 0; i < sizeof(m_pDevice)/sizeof(m_pDevice[0]); i++)
	{
		SAFE_RELEASE(m_pDevice[i]);
		switch(m_cDevParam[i].iDeviceType)
		{
			case 1: //川速雷达
				if(!m_fRadarTrigger)
					m_cDevParam[i].iRoadNo=0xFF;
				m_pDevice[i] = new CSWCSRIKRadar(&m_cDevParam[i],m_fRadarTrigger);
				break;
			case 2: //苏江车检器
				m_pDevice[i] = new CSWJVDCoilDevice(&m_cDevParam[i]);
				break;
			case 3 : //外接红绿灯设备
				m_pDevice[i] = new CSWKNDRGDevice(&m_cDevParam[i]);
				break;
			case 4 : //奥利维亚雷达
				m_pDevice[i] = new CSWRAPIERRadar(&m_cDevParam[i]);
				break;
			case 5 : //四川九洲雷达
				m_pDevice[i] = new CSWJZRadar(&m_cDevParam[i]);
				break;
		}

		if(m_pDevice[i])
		{
			if(FAILED(m_pDevice[i]->Initialize(OnEvent, this, i)))
			{
				SAFE_RELEASE(m_pDevice[i]);
			}

			// 在此启动
			m_pDevice[i]->Start();
		}
	}
	m_fInitialize = TRUE;
	return S_OK;
}

VOID CSWMatchTransformFilter::Close(VOID)
{
	for(int i = 0; i < sizeof(m_pDevice)/sizeof(m_pDevice[0]); i++)
	{
		if(m_pDevice[i])
		{
			m_pDevice[i]->Stop();
			m_pDevice[i]->Close();	//关闭串口
		}
		SAFE_RELEASE(m_pDevice[i]);
	}

	m_cMutex.Lock();
	//对于超时信号的处理
	for(int i = 0; i < sizeof(m_cParam.signal)/sizeof(SIGNAL_PARAM); i++)
	{
		for(int j = 0; j < sizeof(m_cParam.signal[i].signal)/sizeof(SIGNAL); j++)
		{
			SAFE_RELEASE(m_cParam.signal[i].signal[j].pImage);
		}	
	}
	swpa_memset(&m_cParam, 0, sizeof(m_cParam));
	m_fInitialize = FALSE;
	m_cMutex.Unlock();
}

HRESULT CSWMatchTransformFilter::Run(){

	HRESULT hr = CSWBaseFilter::Run();
	if(SUCCEEDED(hr))
	{
		m_matchThread.Start(OnMatchSignal, this);
		m_cOutputThread.Start(OnOutputProxy, this);
		/*for(int i = 0; i < sizeof(m_pDevice)/sizeof(m_pDevice[0]); i++)
		{
			if(m_pDevice[i])
			{
				m_pDevice[i]->Start();
			}
		}*/
	}
	return hr;
}

HRESULT CSWMatchTransformFilter::Stop(){

	CSWBaseFilter::Stop();

	m_matchThread.Stop();
	m_cOutputThread.Stop();
	for(int i = 0; i < sizeof(m_pDevice)/sizeof(m_pDevice[0]); i++)
	{
		if(m_pDevice[i])
		{
			m_pDevice[i]->Stop();
			m_pDevice[i]->Close();	//关闭串口
		}
	}
	if (NULL != m_pComTestDevic)
	{
		m_pComTestDevic->Stop();
		m_pComTestDevic->Close();	//关闭串口
	}
	m_iTESTINGCOMID = -1;
	return S_OK;
}

HRESULT CSWMatchTransformFilter::Receive(CSWObject* obj){

	if(m_fInitialize)
	{
		//图片
		if(IsDecendant(CSWImage, obj) && m_cParam.fEnable)
		{			
			CSWImage* pImage = (CSWImage *)obj;	
			if(pImage->IsCaptureImage())
			{
				static DWORD dwLastCapImageTime = 0;
				DWORD dwCapTimeDiff = pImage->GetRefTime() - dwLastCapImageTime;
				SW_TRACE_DEBUG("receive a capture image: refTime=%d, trigger id=%d, lastCapTime: %d, diff=%d timelen=%d\n",
						pImage->GetRefTime(), pImage->GetFlag(), dwLastCapImageTime, dwCapTimeDiff,CSWDateTime::GetSystemTick());
				dwLastCapImageTime = pImage->GetRefTime();

				if(!m_fRadarTrigger)
				{
					CARLEFT_MATCH_INFO* pCarLeftInfo = new CARLEFT_MATCH_INFO;
					CSWCarLeft* pCarLeft = NULL;
					if(S_OK != SendCommand(1, (WPARAM)&pCarLeft, 0) || !pCarLeft)
					{
						SW_TRACE_DEBUG("pCarLeft == NULL or is not CSWCarLeft object, used default CSWCarLeft\n");
						pCarLeft = new CSWCarLeft();
						pCarLeft->SetParameter(m_pTrackerCfg, NULL);
					}			
					pCarLeftInfo->pCarLeft = pCarLeft;
					pCarLeftInfo->pCarLeft->SetRoadNo(0xFF);
					pCarLeftInfo->dwInputTime = CSWDateTime::GetSystemTick();
					pCarLeftInfo->pCarLeft->SetImage(0, pImage);

					m_cCarLeftMutex.Lock();
					BOOL bFull=FALSE;
					if(!(bFull=m_cCarLeftList.IsFull()))
					{  
						CARLEFT_MATCH_INFO* pCarLeftMatchInfo = new CARLEFT_MATCH_INFO;
						pCarLeftMatchInfo->pCarLeft = pCarLeft;
						SW_TRACE_DEBUG("m_cCarLeftList add");
						m_cCarLeftList.AddTail(pCarLeftMatchInfo);
					}
					else
					{
						SW_TRACE_DEBUG("m_cCarLeftList is full:%d", m_cCarLeftList.GetCount());
					}
					m_cCarLeftMutex.Unlock();
					
					if( m_pTrackerCfg == NULL )
					{
						m_pTrackerCfg = pCarLeft->GetTrackerParam();
					}
					if(!bFull)	
						m_semMatch.Post();
					return S_OK;
				}
			}	
			//信号匹配上，则通知匹配
			if(true == AppendImage(pImage))
			{
				SW_TRACE_DEBUG("image %d match a signal, post to match thread", pImage->GetRefTime());
				m_semMatch.Post();
			}
		}
		//识别结果
		else if(IsDecendant(CSWCarLeft, obj))
		{
			CSWCarLeft *pCarLeft = (CSWCarLeft *)obj;

			BOOL bFull=FALSE;
			m_cCarLeftMutex.Lock();
			if(!(bFull=m_cCarLeftList.IsFull()))
			{
				SW_TRACE_DEBUG("receive a CarLeft: CarArriveTime=%d, trigger id=%d, PlateNo=%s\n",
						pCarLeft->GetCarArriveTime(), pCarLeft->GetTriggerIndex(), (LPCSTR)pCarLeft->GetPlateNo());
				pCarLeft->AddRef();
				CARLEFT_MATCH_INFO* pCarLeftMatchInfo = new CARLEFT_MATCH_INFO;
				pCarLeftMatchInfo->pCarLeft = pCarLeft;
				m_cCarLeftList.AddTail(pCarLeftMatchInfo);
			}
			else
			{
				SW_TRACE_DEBUG("m_cCarLeftList is full:%d", m_cCarLeftList.GetCount());
			}		
			m_cCarLeftMutex.Unlock();

			SW_TRACE_DEBUG("m_cCarLeftList size:%d", m_cCarLeftList.GetCount());
			if( m_pTrackerCfg == NULL )
			{
				m_pTrackerCfg = pCarLeft->GetTrackerParam();
			}
			if(!bFull)	
				m_semMatch.Post();
		}
	}
	return S_OK;
}

bool CSWMatchTransformFilter::SignalIncludeImage(SIGNAL_PARAM* pSignal, bool fCapture)
{
	bool fHasImage = false;
	if(NULL != pSignal)
	{
		if(pSignal->dwType == CSWBaseDevice::IMAGE && TRUE == fCapture)
		{
			SW_TRACE_DEBUG("image match IMAGE signal type");
			fHasImage = true;
		}
		else if(pSignal->dwType == CSWBaseDevice::TRIGGER && TRUE == pSignal->fTrigger && TRUE == fCapture)
		{
			SW_TRACE_DEBUG("image match TRIGGER capture signal type");
			fHasImage = true;
		}
		else if(pSignal->dwType == CSWBaseDevice::TRIGGER && FALSE == pSignal->fTrigger && FALSE == fCapture)
		{
			SW_TRACE_DEBUG("image match TRIGGER normal signal type");
			fHasImage = true;
		}
		else if(pSignal->dwCondition == 2 && FALSE == fCapture)
		{
			SW_TRACE_DEBUG("image match dwCondition == 2 normal signal type");
			fHasImage = true;
		}
	}
	return fHasImage;
}

bool CSWMatchTransformFilter::AppendImage(CSWImage *pImage)
{
	CSWAutoLock aLock(&m_cMutex);
	bool fFound = false;
	bool fCapture = pImage->IsCaptureImage();
	//先搜索空闲的空间保存图片
	for(int i = 0; i < sizeof(m_cParam.signal)/sizeof(SIGNAL_PARAM); i++)
	{	
		SIGNAL_PARAM* pSignal = &m_cParam.signal[i];
		if( pSignal->dwType == CSWBaseDevice::IMAGE && TRUE == fCapture )
		{
			for(int j = 0; j < sizeof(pSignal->signal)/sizeof(SIGNAL); j++)
			{	//图片指针为0
				if(NULL == pSignal->signal[j].pImage)
				{
					pSignal->signal[j].dwTime = pImage->GetRefTime();
					SW_TRACE_DEBUG("<Match>append image signal[%d],signal[%d] time:%d,RoadID:%d.\n", i,j, pSignal->signal[j].dwTime,pSignal->dwRoadID);
					pSignal->signal[j].pImage = pImage;
					pSignal->signal[j].fIsMatch = FALSE;
					pImage->AddRef();
					fFound = true;
					break;
				}
			}
		}
		else if( (pSignal->dwType == CSWBaseDevice::TRIGGER && TRUE == pSignal->fTrigger && TRUE == fCapture)
				|| (pSignal->dwType == CSWBaseDevice::TRIGGER && FALSE == pSignal->fTrigger && FALSE == fCapture)
				|| (pSignal->dwCondition == 2 && FALSE == fCapture)
		       )
		{
			for(int j = 0; j < sizeof(pSignal->signal)/sizeof(SIGNAL); j++)
			{
				if(NULL == pSignal->signal[j].pImage && pSignal->signal[j].dwTime > 0)
				{
					SW_TRACE_DEBUG("<Match>append image to signal[%d],signal[%d][type:%d] time:%d.\n",i, j, pSignal->dwType, pSignal->signal[j].dwTime);
					pSignal->signal[j].pImage = pImage;
					pImage->AddRef();
					fFound = true;
					break;
				}
			}
		}

	}
	return fFound;
}

bool CSWMatchTransformFilter::AppendSpeed(DWORD dwRoadID, DWORD dwTime, DWORD dwSpeed, DWORD dwSpeedType, DWORD dwDirection)
{
	CSWAutoLock aLock(&m_cMutex);
	bool fFound = false;
	for(int i = 0; false == fFound && i < sizeof(m_cParam.signal)/sizeof(SIGNAL_PARAM); i++)
	{	
		//侧装不区分车道
		if(!m_fRadarTrigger)
		{
			if(m_cParam.signal[i].dwType == CSWBaseDevice::SPEED)
			{
				m_cParam.signal[i].dwRoadID = 0xFF;
			}
		}

		if(m_cParam.signal[i].dwType == CSWBaseDevice::SPEED && (m_cParam.signal[i].dwRoadID == 0xFF || m_cParam.signal[i].dwRoadID == dwRoadID))
		{
			for(int j = 0; false == fFound && j < sizeof(m_cParam.signal[i].signal)/sizeof(SIGNAL); j++)
			{
				if(0 == m_cParam.signal[i].signal[j].dwTime)
				{
					m_cParam.signal[i].signal[j].dwTime = dwTime;
					m_cParam.signal[i].signal[j].dwSpeed = dwSpeed;
					m_cParam.signal[i].signal[j].dwSpeedType = dwSpeedType;
					m_cParam.signal[i].signal[j].dwDirection = dwDirection;
					m_cParam.signal[i].signal[j].fIsMatch = FALSE;
					m_cParam.signal[i].signal[j].pImage = NULL;
					SW_TRACE_DEBUG("<Match>append speed signal[%d] time:%d.\n", j, m_cParam.signal[i].signal[j].dwTime);
					fFound = true;
					break;
				}
			}
		}
	}
	return fFound;
}

void* CSWMatchTransformFilter::OnOutputProxy(void* pvParam)
{
	if( pvParam != NULL )
	{
		CSWMatchTransformFilter* pFilter = (CSWMatchTransformFilter*)pvParam;
		pFilter->OnOutput();
	}
}

HRESULT CSWMatchTransformFilter::OnOutput()
{
	SW_TRACE_DEBUG("<MatchTransformFilter> output run...\n");
	INT iCount = 0, iTotalCount = 0;
	while( S_OK == m_cOutputThread.IsValid() && FILTER_RUNNING == GetState() )
	{
		m_semOutput.Pend(500);
		//swpa_thread_sleep_ms(500);

		//发送
		m_cOutputMutex.Lock();
		CSWCarLeft* pCarLeft = NULL;
		if( !m_cOutputList.IsEmpty() )
		{
			iTotalCount = m_cOutputList.GetCount();
			pCarLeft = m_cOutputList.RemoveHead();
			SW_TRACE_DEBUG("OnOutput iTotalCount = %d.\n", iTotalCount);
		}
		if( pCarLeft == NULL )
		{
			m_cOutputMutex.Unlock();
			continue;
		}
		m_cOutputMutex.Unlock();

		//SW_TRACE_DEBUG("OnOutput <1111>\n");
		++iCount;
		SW_TRACE_DEBUG("<MatchTransformFilter>[%d] out put find %s. %d.\n", iTotalCount, (const CHAR*)pCarLeft->GetPlateNo(), iCount);

		for(int i = 0; i < pCarLeft->GetImageCount() - 2; i++)
		{
			// TODO: 只对抓拍图识别，目前外总控匹配到 最清晰大图中。
			if (0 != i)
			{
				break;
			}

			if( pCarLeft->GetImage(i) == NULL)
			{
				continue;
			}
			// todo:有车身颜色有抓拍图就要有重识别，这个对性能会有一此小影响。
			BOOL fIsNoPlateRecong = m_cParam.fCaptureRecong; //(-1 != pCarLeft->GetPlateNo().Find("无车牌") && m_cParam.fCaptureRecong);
			BOOL fIsNeedRecogColor = (pCarLeft->GetTrackerParam() != NULL && pCarLeft->GetTrackerParam()->fEnableRecgCarColor);			
			if( pCarLeft->GetImage(i)->IsCaptureImage() && (fIsNeedRecogColor || fIsNoPlateRecong) && pCarLeft->GetImage(i)->GetType() != SW_IMAGE_JPEG )
			{
				CSWCarLeftArray carLeft;
				DWORD dwBegin = CSWDateTime::GetSystemTick();
				//HRESULT hr = CSWMessage::SendMessage(MSG_RECOGNIZE_PHOTO, (WPARAM)pCarLeft, (LPARAM)&carLeft);
				SW_TRACE_DEBUG("<MatchTransformFilter>recognize photo %d ms.\n", CSWDateTime::GetSystemTick() - dwBegin);
				HRESULT hr=S_OK;
				if( S_OK == hr)
				{
					int index = -1;
					for(int k = 0; k < carLeft.GetCount(); k++)
					{
						// 多结果才需要根据车道匹配结果
						if((carLeft.GetCount() <= 1) ||
								(carLeft[k]->GetRoadNo() == pCarLeft->GetRoadNo() + 1 || carLeft[k]->GetRoadNo() == 0xFF || pCarLeft->GetRoadNo() == 0xFF))
						{
							index = k;
							break;
						}
						else
						{
							SW_TRACE_DEBUG("<MatchTransformFilter>recognize index:%d. roadno:%d. match roadno:%d.\n", k, carLeft[k]->GetRoadNo(), pCarLeft->GetRoadNo()+1);
						}
					}

					//                    SW_RECT rcLast;
					//                    pCarLeft->GetResultRect(1, &rcLast);
					//                    SW_TRACE_DEBUG("Output CarLeft : PlateNo : %s, LastRect[%d, %d, %d, %d] \n",
					//                                   (LPCSTR)pCarLeft->GetPlateNo(), rcLast.left, rcLast.top, rcLast.right, rcLast.bottom);
					if( index == -1 )
					{
						SW_TRACE_DEBUG("<MatchTransformFilter>recognize count:%d.none match.\n", carLeft.GetCount());
					}
					else if ((swpa_strstr(carLeft[index]->GetPlateNo(), "无") == NULL) || (swpa_strstr(pCarLeft->GetPlateNo(), "无")  != NULL) ) // 抓拍识别有结果或者视频流识别是无车牌就替换大图
					{	
						//替换车牌号码
						if( fIsNoPlateRecong )
						{
							//                            SW_TRACE_DEBUG("<MatchTransformFilter>replace Capture Result : %s.\n", (LPCSTR)carLeft[index]->GetPlateNo());
							pCarLeft->m_fCaptureRecogResult = TRUE;
							pCarLeft->SetPlateNo(carLeft[index]->GetPlateNo());
							pCarLeft->SetFirstAverageConfidence(carLeft[index]->GetAverageConfidence());
							pCarLeft->SetFirstAverageConfidence(carLeft[index]->GetFirstAverageConfidence());
							pCarLeft->SetObservedFrames(carLeft[index]->GetObservedFrames());
							pCarLeft->SetPlateType(carLeft[index]->GetPlateType());
							pCarLeft->SetPlateColor(carLeft[index]->GetPlateColor());

							// 设置车型
							if(pCarLeft->GetCarType() == CT_WALKMAN
									|| pCarLeft->GetCarType() == CT_BIKE )
							{
								pCarLeft->SetCarType(CT_SMALL);
							}

							// 重识别的车牌坐标放在了 BestPlatePos，这里放在输出结果的 LastPlatePos
							SW_RECT rcBest;
							carLeft[index]->GetResultRect(0, &rcBest);
							pCarLeft->SetResultRect(1, &rcBest);
							//                            SW_TRACE_DEBUG("CaptureImage Recognize result : %s, BestRect[%d, %d, %d, %d]",
							//                                           (LPCSTR)carLeft[index]->GetPlateNo(), rcBest.left, rcBest.top, rcBest.right, rcBest.bottom);

							for(int k = 0; k < carLeft[index]->GetImageCount(); k++)
							{
								SW_RECT rc;
								CSWImage *pImage = carLeft[index]->GetImage(k, &rc);
								if(pImage)
								{
									pCarLeft->SetImage(k, pImage, &rc);
								}
							}
						}
						//替换车身颜色
						if( fIsNeedRecogColor)
						{
							switch(carLeft[index]->GetCarColor())
							{
								case CC_WHITE:
									SW_TRACE_DEBUG("%s:CC_WHITE, IsNight=%d", (LPCSTR)carLeft[index]->GetPlateNo(), carLeft[index]->GetIsNight());
									break;
								case CC_GREY:
									SW_TRACE_DEBUG("%s:CC_GREY, IsNight=%d", (LPCSTR)carLeft[index]->GetPlateNo(), carLeft[index]->GetIsNight());
									break;
								case CC_BLACK:
									SW_TRACE_DEBUG("%s:CC_BLACK, IsNight=%d", (LPCSTR)carLeft[index]->GetPlateNo(), carLeft[index]->GetIsNight());
									break;
								case CC_RED:
									SW_TRACE_DEBUG("%s:CC_RED, IsNight=%d", (LPCSTR)carLeft[index]->GetPlateNo(), carLeft[index]->GetIsNight());
									break;
								case CC_YELLOW:
									SW_TRACE_DEBUG("%s:CC_YELLOW, IsNight=%d", (LPCSTR)carLeft[index]->GetPlateNo(), carLeft[index]->GetIsNight());
									break;
								case CC_GREEN:
									SW_TRACE_DEBUG("%s:CC_GREEN, IsNight=%d", (LPCSTR)carLeft[index]->GetPlateNo(), carLeft[index]->GetIsNight());
									break;
								case CC_BLUE:
									SW_TRACE_DEBUG("%s:CC_BLUE, IsNight=%d", (LPCSTR)carLeft[index]->GetPlateNo(), carLeft[index]->GetIsNight());
									break;
								case CC_PURPLE:
									SW_TRACE_DEBUG("%s:CC_PURPLE, IsNight=%d", (LPCSTR)carLeft[index]->GetPlateNo(), carLeft[index]->GetIsNight());
									break;
								case CC_PINK:
									SW_TRACE_DEBUG("%s:CC_PINK, IsNight=%d", (LPCSTR)carLeft[index]->GetPlateNo(), carLeft[index]->GetIsNight());
									break;
								case CC_BROWN:
									SW_TRACE_DEBUG("%s:CC_BROWN, IsNight=%d", (LPCSTR)carLeft[index]->GetPlateNo(), carLeft[index]->GetIsNight());
									break;
								default:
									SW_TRACE_DEBUG("%s:CC_UNKNOWN, IsNight=%d", (LPCSTR)carLeft[index]->GetPlateNo(), carLeft[index]->GetIsNight());
									break;
							}
							if(carLeft[index]->GetCarColor() != CC_UNKNOWN)
							{
								SW_TRACE_DEBUG("<MatchTransformFilter>replace Color.%s=>%s. %d=>%d.\n", (const CHAR*)pCarLeft->GetPlateNo(), (const CHAR*)carLeft[index]->GetPlateNo(), pCarLeft->GetCarColor(), carLeft[index]->GetCarColor());
								pCarLeft->SetCarColor((CAR_COLOR)carLeft[index]->GetCarColor());
							}
						}
						//人脸坐标
						pCarLeft->SetFaceRect(carLeft[index]);
					}
					// 视频流无车牌  或 抓拍识别有牌车 都替换抓拍图
					if( (swpa_strstr(pCarLeft->GetPlateNo(), "无")  != NULL) ||
							(index != -1 && swpa_strstr(carLeft[index]->GetPlateNo(), "无") == NULL) )
					{
						// 替换抓拍图到最后大图
						CSWImage* pImage = pCarLeft->GetImage(0);
						if (NULL != pImage)
						{
							pCarLeft->SetImage(1, pImage);
							pCarLeft->SetImage(0, NULL);
						}
					}
				}
			}
		}
		// 去掉最清晰大图，以免可能引起复位，测试需要的时候在打开
		if ( 0 )
		{
			pCarLeft->SetImage(0, NULL);
		}

		GetOut(0)->Deliver(pCarLeft);
		pCarLeft->Release();
		pCarLeft = NULL;
		//        SW_TRACE_DEBUG("OnOutput <4444>\n");
	}
	SW_TRACE_DEBUG("<MatchTransformFilter> output exit...\n");
	return S_OK;
}

HRESULT CSWMatchTransformFilter::OnStartCOMTest(WPARAM wParam, LPARAM lParam)
{
	INT iComId = (INT)wParam;
	if (iComId < 0 || iComId > 1)
	{
		SW_TRACE_NORMAL("Err: OnStartCOMTest COM ID %d err\n",iComId);
		return E_INVALIDARG;
	}

	if (NULL != m_pComTestDevic)	//只能开始一次
	{
		SW_TRACE_NORMAL("Err: Must stop last COM %d test server first\n",m_iTESTINGCOMID);
		return E_FAIL;
	}

	if (-1 != m_iTESTINGCOMID)
	{
		SW_TRACE_NORMAL("Err: Last Test COM %d already on server!\n",m_iTESTINGCOMID);
		return E_INVALIDARG;
	}

	//关闭正常外设接入
	if(m_pDevice[iComId])
	{
		m_pDevice[iComId]->Stop();
		m_pDevice[iComId]->Close();
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

	m_iTESTINGCOMID = iComId;
	SW_TRACE_NORMAL("<OnStartCOMTest> Start test COM %d server!\n",iComId);

	return S_OK;
}

HRESULT CSWMatchTransformFilter::OnStopCOMTest(WPARAM wParam, LPARAM lParam)
{
	INT iComId = (INT)wParam;
	if (iComId < 0 || iComId > 1)
	{
		SW_TRACE_NORMAL("Err: OnStopCOMTest COM ID %d err\n",iComId);
		return E_INVALIDARG;
	}

	if (m_iTESTINGCOMID != iComId)
	{
		SW_TRACE_NORMAL("Err: COM ID Err last %d cur %d\n",m_iTESTINGCOMID,iComId);
		return E_FAIL;
	}

	if (m_pComTestDevic)
	{
		m_pComTestDevic->Stop();
		m_pComTestDevic->Close();
		SAFE_RELEASE(m_pComTestDevic);
	}

	//恢复其他的外设接入
	if(m_pDevice[iComId])
	{
		if(S_OK == m_pDevice[iComId]->Initialize(OnEvent, this, iComId))
		{
			m_pDevice[iComId]->Start();
		}
		else
		{
			SW_TRACE_NORMAL("init device <id:%d> failed!\n",iComId);
			SAFE_RELEASE(m_pDevice[iComId]);
		}
	}

	m_iTESTINGCOMID = -1;

	SW_TRACE_NORMAL("<OnStopCOMTest> Stop test COM %d server!\n",iComId);

	return S_OK;
}

VOID CSWMatchTransformFilter::OnEvent(PVOID pvParam, CSWBaseDevice *pDevice, CSWBaseDevice::DEVICE_TYPE type, DWORD dwTime, PDWORD pdwValue)
{
	CSWMatchTransformFilter *pThis = (CSWMatchTransformFilter *)pvParam;
	if(type == CSWBaseDevice::SPEED)
	{
		SW_TRACE_DEBUG("receive speed event! time: %d[speed:%d, roadid:%d].", dwTime, pdwValue[0], pdwValue[1]);
		if(pThis->AppendSpeed(pdwValue[1], dwTime, pdwValue[0], IsDecendant(CSWJVDCoilDevice, pDevice) ? E_SIG_COIL_SPEED : E_SIG_RADAR_SPEED,pdwValue[2]))
		{
			SW_TRACE_DEBUG("speed %d[%d, %d, %d] match a signal, post to match thread", dwTime, pdwValue[0], pdwValue[1], pdwValue[2]);
			pThis->m_semMatch.Post();
		}
	}
}

void* CSWMatchTransformFilter::OnMatchSignal(void* pvParam)
{
	CSWMatchTransformFilter* pThis = (CSWMatchTransformFilter *)pvParam;
	CARLEFT_MATCH_INFO* pCarLeftMatchInfo = NULL;

	SW_TRACE_DEBUG("<MatchTransformFilter> Match run...\n");
	while(S_OK == pThis->m_matchThread.IsValid() && FILTER_RUNNING == pThis->GetState() && pThis->m_fInitialize)
	{
		pThis->m_semMatch.Pend(400);
		// 临时处理：可能会出现 output 线程卡死的情况，重启设备
		{
			if (pThis->m_cOutputList.GetCount() >= 12)
			{
				//                pThis->m_cOutputThread.Stop();
				//                swpa_thread_sleep_ms(500);
				//                pThis->m_iOutputThreadRestartTime ++;
				//                pThis->m_cOutputThread.Start(OnOutputProxy, pThis);
				//                SW_TRACE_DEBUG("<MatchTransformFilter> Match <OutputListCount = %d, m_cOutputThread is dead, restart...restarttimes = %d>\n",
				//                               pThis->m_cOutputList.GetCount(), pThis->m_iOutputThreadRestartTime);

				//                if (pThis->m_iOutputThreadRestartTime >= 3)
				{
					SW_TRACE_DEBUG("<MatchTransformFilter> Match <m_cOutputThread is dead, RESETDEVICE>\n");
					CSWMessage::SendMessage(MSG_APP_RESETDEVICE, 0, 0);
				}
			}
		}

		//外总控不使能的情况下
		//if(!pThis->m_cParam.fEnable)
		if(0)
		{
			pThis->m_cCarLeftMutex.Lock();
			while( !pThis->m_cCarLeftList.IsEmpty() )
			{
				pCarLeftMatchInfo = pThis->m_cCarLeftList.RemoveHead();
				if( pCarLeftMatchInfo != NULL )
				{
					CSWCarLeft *pCarLeft = pCarLeftMatchInfo->pCarLeft;
					if( pCarLeft != NULL )
					{
						pThis->m_cOutputMutex.Lock();
						if(!pThis->m_cOutputList.IsFull())
						{
							pCarLeft->AddRef();
							pThis->m_cOutputList.AddTail(pCarLeft);
							SW_TRACE_DEBUG("pThis->m_cOutputList.GetCount():%d", pThis->m_cOutputList.GetCount());
							pThis->m_semOutput.Post();
						}
						else
						{
							SW_TRACE_DEBUG("m_cOutputList is full, egnore the result, total size:%d", pThis->m_cOutputList.GetCount());
						}
						pThis->m_cOutputMutex.Unlock();
					}
					SAFE_RELEASE(pCarLeftMatchInfo->pCarLeft);
					delete pCarLeftMatchInfo;
					pCarLeftMatchInfo = NULL;
				}
			}
			pThis->m_cCarLeftMutex.Unlock();
		}
		//使能外总控，进行信号匹配
		else
		{
			pThis->m_cMutex.Lock();
			SW_TRACE_DEBUG("OutCtrl run\n");
			//对于超时信号的处理
			for(int i = 0; i < sizeof(pThis->m_cParam.signal)/sizeof(SIGNAL_PARAM); i++)
			{
				if(CSWBaseDevice::NONE < pThis->m_cParam.signal[i].dwType && pThis->m_cParam.signal[i].dwType < CSWBaseDevice::MAX_COUNT)
				{
					DWORD dwRefTime=CSWDateTime::GetSystemTick();
					for(int j = 0; j < sizeof(pThis->m_cParam.signal[i].signal)/sizeof(SIGNAL); j++)
					{		
						if((pThis->m_cParam.signal[i].signal[j].dwTime > 0) &&
								(dwRefTime - pThis->m_cParam.signal[i].signal[j].dwTime > pThis->m_cParam.dwSignalKeepTime) &&
								(dwRefTime > pThis->m_cParam.signal[i].signal[j].dwTime))
						{
							switch(pThis->m_cParam.signal[i].dwCondition)
							{
								case 0 : //此信号可有可无，直接干掉
									if(pThis->m_cParam.signal[i].signal[j].pImage!=NULL)
									{
										SW_TRACE_DEBUG("signal[%d][%d][%d - %d > %d] time out, ignore  the signal TriggerIndex:%d",
												j, pThis->m_cParam.signal[i].dwType, dwRefTime,
												pThis->m_cParam.signal[i].signal[j].dwTime, pThis->m_cParam.dwSignalKeepTime,
												pThis->m_cParam.signal[i].signal[j].pImage->GetFlag());
									}
									pThis->m_cParam.signal[i].signal[j].dwTime = 0;
									SAFE_RELEASE(pThis->m_cParam.signal[i].signal[j].pImage);
									break;

								case 1 : //此信号必须等待车牌才能出结果,超时直接干掉
									SW_TRACE_DEBUG("signal[%d][%d - %d > %d] time out, ignore  the signal",
											pThis->m_cParam.signal[i].dwType, dwRefTime,
											pThis->m_cParam.signal[i].signal[j].dwTime, pThis->m_cParam.dwSignalKeepTime);
									pThis->m_cParam.signal[i].signal[j].dwTime = 0;
									SAFE_RELEASE(pThis->m_cParam.signal[i].signal[j].pImage);
									break;

								case 2 : //强制出牌
									// 如果已经匹配过就不出结果
									{
										if( !pThis->m_cParam.signal[i].signal[j].fIsMatch )
										{
											pThis->m_cCarLeftMutex.Lock();
											if(!pThis->m_cCarLeftList.IsFull())
											{
												SW_TRACE_DEBUG("signal[%d][%d - %d > %d] time out, fire to carleft",
														pThis->m_cParam.signal[i].dwType, dwRefTime,
														pThis->m_cParam.signal[i].signal[j].dwTime, pThis->m_cParam.dwSignalKeepTime);
												CARLEFT_MATCH_INFO* pCarLeftInfo = new CARLEFT_MATCH_INFO;
												CSWCarLeft* pCarLeft = NULL;
												if(S_OK != pThis->SendCommand(1, (WPARAM)&pCarLeft, 0) || !pCarLeft)
												{
													SW_TRACE_DEBUG("create carleft failed, use default CSWCarLeft.");
													pCarLeft = new CSWCarLeft();
													pCarLeft->SetParameter(pThis->m_pTrackerCfg, NULL);
												}
												SW_TRACE_DEBUG("create carleft name:%s, image:0x%08x", pCarLeft->Name(), pThis->m_cParam.signal[i].signal[j].pImage);
												pCarLeftInfo->pCarLeft = pCarLeft;
												pCarLeftInfo->pCarLeft->SetCarArriveTime(pThis->m_cParam.signal[i].signal[j].dwTime);
												pCarLeftInfo->pCarLeft->SetRoadNo(pThis->m_cParam.signal[i].dwRoadID);
												pCarLeftInfo->dwOutputCondition = 1;
												pCarLeftInfo->dwInputTime = dwRefTime;
												pCarLeftInfo->pCarLeft->SetImage(1, pThis->m_cParam.signal[i].signal[j].pImage);
												pThis->m_cCarLeftList.AddTail(pCarLeftInfo);
												pThis->m_cParam.signal[i].signal[j].fIsMatch = TRUE;
												SW_TRACE_DEBUG("m_cCarLeftList size:%d", pThis->m_cCarLeftList.GetCount());
											}
											else
											{
												SW_TRACE_DEBUG("m_cCarLeftList is full:%d", pThis->m_cCarLeftList.GetCount());
											}
											pThis->m_cCarLeftMutex.Unlock();
										}
										else
										{
											if(pThis->m_cParam.signal[i].signal[j].pImage!=NULL)
											{
												SW_TRACE_DEBUG("signal[%d][%d - %d > %d] time out and it has match, ignore the signal,pImage=0x%08x",
														pThis->m_cParam.signal[i].dwType, dwRefTime,
														pThis->m_cParam.signal[i].signal[j].dwTime, pThis->m_cParam.dwSignalKeepTime,
														pThis->m_cParam.signal[i].signal[j].pImage);
											}
											pThis->m_cParam.signal[i].signal[j].dwTime = 0;
											SAFE_RELEASE(pThis->m_cParam.signal[i].signal[j].pImage);
										}
									}
									break;
							}
						}
					}
				}
			}
			pThis->m_cMutex.Unlock();

			//遍历车牌链表
			pThis->m_cCarLeftMutex.Lock();
			SW_POSITION pos = pThis->m_cCarLeftList.GetHeadPosition();
			SW_POSITION posLast = pos;
			while(pThis->m_cCarLeftList.IsValid(pos))
			{
				posLast = pos;
				pCarLeftMatchInfo = pThis->m_cCarLeftList.GetNext(pos);
				//遍历信号依次匹配车牌
				//bool fOk = true;
				bool fOk = false;
				pThis->m_cMutex.Lock();
				for(int i = 0; i < sizeof(pThis->m_cParam.signal)/sizeof(SIGNAL_PARAM); i++)
				{
					if(CSWBaseDevice::NONE < pThis->m_cParam.signal[i].dwType
							&& pThis->m_cParam.signal[i].dwType < CSWBaseDevice::MAX_COUNT
							&& (pCarLeftMatchInfo->pCarLeft->GetRoadNo() == pThis->m_cParam.signal[i].dwRoadID || 0xFF == pThis->m_cParam.signal[i].dwRoadID))
					{
						pFunMatch pMatch = pThis->GetFunction(pThis->m_cParam.signal[i].dwType);
						if(!pMatch)
						{
							SW_TRACE_DEBUG("dwType = %d, not found match function\n", pThis->m_cParam.signal[i].dwType);
							continue;
						}
						if((pThis->*pMatch)(&pThis->m_cParam.signal[i], pCarLeftMatchInfo))
						{
							fOk=true;
						}
						else
						{
							SW_TRACE_DEBUG("match error type:%d,RoadID=%d\n", pThis->m_cParam.signal[i].dwType,pThis->m_cParam.signal[i].dwRoadID);
						}
						/*if(!pMatch)
						{
							fOk = false;
							SW_TRACE_DEBUG("dwType = %d, not found match function", pThis->m_cParam.signal[i].dwType);
						}
						else if(false == (pThis->*pMatch)(&pThis->m_cParam.signal[i], pCarLeftMatchInfo))
						{
							// 即使有一类型匹配不上，也要继续匹配其它类型。
							// 只记录最终是否已完全匹配
							fOk = false;
							//break;
						}
						else
						{
							fOk = true;
						}*/
					}
				}
				pThis->m_cMutex.Unlock();
				//匹配完毕，放到结果队列。
				if(fOk)
				{
					SW_TRACE_DEBUG("match carleft:%s\n", (LPCSTR)pCarLeftMatchInfo->pCarLeft->GetPlateNo());
					pThis->m_cOutputMutex.Lock();
					if(!pThis->m_cOutputList.IsFull())
					{
						SAFE_ADDREF(pCarLeftMatchInfo->pCarLeft);
						pThis->m_cOutputList.AddTail(pCarLeftMatchInfo->pCarLeft);
						SW_TRACE_DEBUG("m_cOutputList size:%d", pThis->m_cOutputList.GetCount());
						pThis->m_semOutput.Post();
					}
					else
					{
						SW_TRACE_DEBUG("m_cOutputList is full, egnore the result, total size:%d", pThis->m_cOutputList.GetCount());
					}
					pThis->m_cOutputMutex.Unlock();

					SAFE_RELEASE(pCarLeftMatchInfo->pCarLeft);
					delete pCarLeftMatchInfo;
					pThis->m_cCarLeftList.RemoveAt(posLast);
				}
				//判断车牌是否超时，如果超时，则直接删除
				else if(CSWDateTime::GetSystemTick() - pCarLeftMatchInfo->dwInputTime > pThis->m_cParam.dwPlateKeepTime)
				{
					//判断该车牌所在的车道号对应的信号是否无关紧要，是则强制出牌，否则丢弃
					fOk = true;
					for(int i = 0; i < sizeof(pThis->m_cParam.signal)/sizeof(SIGNAL_PARAM); i++)
					{
						if(pThis->m_cParam.signal[i].dwRoadID == 0xFF || pThis->m_cParam.signal[i].dwRoadID == pCarLeftMatchInfo->pCarLeft->GetRoadNo())
						{
							if(pThis->m_cParam.signal[i].dwCondition == 1)
							{
								fOk = false;
							}
						}
					}

					SW_TRACE_DEBUG("carleft ID:%d time out, %s the plate:%s, current time:%d inputTime:%d diffTime:%d\n",
							pCarLeftMatchInfo->pCarLeft->GetTriggerIndex(),
							fOk ? "fire output" : "delete",
							(LPCSTR)pCarLeftMatchInfo->pCarLeft->GetPlateNo(),
							CSWDateTime::GetSystemTick(),
							pCarLeftMatchInfo->dwInputTime,
							CSWDateTime::GetSystemTick() - pCarLeftMatchInfo->dwInputTime);
					if(fOk)
					{
						CSWCarLeft *pCarLeft = pCarLeftMatchInfo->pCarLeft;

						pThis->m_cOutputMutex.Lock();
						if(!pThis->m_cOutputList.IsFull())
						{
							pCarLeft->AddRef();
							pThis->m_cOutputList.AddTail(pCarLeft);
							SW_TRACE_DEBUG("m_cOutputList size:%d", pThis->m_cOutputList.GetCount());
							pThis->m_semOutput.Post();
						}
						else
						{
							SW_TRACE_DEBUG("m_cOutputList is full, egnore the result, total size:%d", pThis->m_cOutputList.GetCount());
						}
						pThis->m_cOutputMutex.Unlock();
					}
					pCarLeftMatchInfo->pCarLeft->Release();
					delete pCarLeftMatchInfo;
					pThis->m_cCarLeftList.RemoveAt(posLast);
				}
			}
			pThis->m_cCarLeftMutex.Unlock();
		}
	}

	SW_TRACE_DEBUG("<MatchTransformFilter> Match exit...\n");
	return 0;
}

bool CSWMatchTransformFilter::MatchSpeed(SIGNAL_PARAM *pSignal, CARLEFT_MATCH_INFO *pMatchInfo)
{
	DWORD dwDiffTime = pMatchInfo->dwMatchSpeedTime;
	DWORD dwMatchIndex = 0xFFFFFFFF;
	if(m_fRadarTrigger)	//正装模式匹配机制
	{
		for(int i = 0; i < sizeof(pSignal->signal)/sizeof(pSignal->signal[0]); i++)
		{	//信号存在
			if(pSignal->signal[i].dwTime > 0)
			{	//信号和车牌的差值
				if(pSignal->signal[i].dwTime >= pMatchInfo->dwInputTime
						&& pSignal->signal[i].dwTime - pMatchInfo->dwInputTime <= m_cParam.dwMatchMaxTime)
				{
					dwDiffTime = pSignal->signal[i].dwTime - pMatchInfo->dwInputTime;
					dwMatchIndex = i;
				}
				//车牌和信号的差值
				else if(pSignal->signal[i].dwTime < pMatchInfo->dwInputTime 
						&& pMatchInfo->dwInputTime - pSignal->signal[i].dwTime <= m_cParam.dwMatchMinTime
						&& pMatchInfo->dwInputTime - pSignal->signal[i].dwTime < dwDiffTime)
				{
					dwDiffTime = pMatchInfo->dwInputTime - pSignal->signal[i].dwTime;
					dwMatchIndex = i;
				}
			}		
		}

		if(dwMatchIndex != 0xFFFFFFFF)
		{
			SW_TRACE_DEBUG("match speed time:%d, carleft time:%d, interval:%d", pSignal->signal[dwMatchIndex].dwTime, pMatchInfo->pCarLeft->GetCarArriveTime(), (INT)pSignal->signal[dwMatchIndex].dwTime - (INT)pMatchInfo->pCarLeft->GetCarArriveTime());

			pMatchInfo->pCarLeft->SetCarspeed(pSignal->signal[dwMatchIndex].dwSpeed);
			pMatchInfo->pCarLeft->SetSpeedType((E_SIG_RADAR_SPEED == pSignal->signal[dwMatchIndex].dwSpeedType)?"雷达测速":"线圈测速");
			pMatchInfo->dwMatchSpeedTime = dwDiffTime;	


			// 如果是主信号超时出的结果。
			if(pMatchInfo->dwOutputCondition == 1)
			{
				SW_TRACE_DEBUG("雷达主信号超时");
				pMatchInfo->pCarLeft->SetImage(0, pSignal->signal[dwMatchIndex].pImage);
				pMatchInfo->pCarLeft->SetImage(1, pSignal->signal[dwMatchIndex].pImage);
			}
			// 设置匹配标志
			pSignal->signal[dwMatchIndex].fIsMatch = TRUE;
			//信号超时则删除
			//if(CSWDateTime::GetSystemTick() - pSignal->signal[dwMatchIndex].dwTime > m_cParam.dwSignalKeepTime)
			{
				SAFE_RELEASE(pSignal->signal[dwMatchIndex].pImage);
				pSignal->signal[dwMatchIndex].dwTime = 0;
				pSignal->signal[dwMatchIndex].dwSpeed = 0;
			}
		}
	}
	else	//侧装模式不做匹配，以雷达时间作为过车时间
	{
		for(int i = 0; i < sizeof(pSignal->signal)/sizeof(pSignal->signal[0]); i++)
		{
			if(pSignal->signal[i].dwTime > 0)
			{
				dwMatchIndex = i;
			}
		}

		if(dwMatchIndex != 0xFFFFFFFF)
		{
			SW_TRACE_DEBUG("match speed time:%d, dwMatchIndex:%d, interval:%d speed value:%d", pSignal->signal[dwMatchIndex].dwTime, dwMatchIndex, pSignal->signal[dwMatchIndex].dwSpeed);
			pMatchInfo->pCarLeft->SetCarspeed(pSignal->signal[dwMatchIndex].dwSpeed);
			pMatchInfo->pCarLeft->SetCarArriveTime(pSignal->signal[dwMatchIndex].dwTime);
			pMatchInfo->pCarLeft->SetSpeedType("雷达测速");
			pMatchInfo->pCarLeft->SetReverseRun(pSignal->signal[dwMatchIndex].dwDirection==2?TRUE:FALSE);

			// 设置匹配标志
			pSignal->signal[dwMatchIndex].fIsMatch = TRUE;
			//信号超时则删除
			if(CSWDateTime::GetSystemTick() - pSignal->signal[dwMatchIndex].dwTime > m_cParam.dwSignalKeepTime)
			{
				SAFE_RELEASE(pSignal->signal[dwMatchIndex].pImage);
				pSignal->signal[dwMatchIndex].dwTime = 0;
				pSignal->signal[dwMatchIndex].dwSpeed = 0;
			}
		}

	}
	return dwDiffTime != NOT_MATCH_TIME;
}

bool CSWMatchTransformFilter::MatchImage(SIGNAL_PARAM *pSignal, CARLEFT_MATCH_INFO *pMatchInfo)
{	
	SW_TRACE_DEBUG("MatchImage:%d",pMatchInfo->dwInputTime);
	DWORD dwDiffTime = pMatchInfo->dwMatchImageTime[1];
	DWORD dwMatchIndex = 0xFFFFFFFF;

	//先判断图片的标志位
	for(int i = 0; i < sizeof(pSignal->signal)/sizeof(pSignal->signal[0]); i++)
	{
		if(pSignal->signal[i].dwTime > 0 && pSignal->signal[i].pImage)
		{
			SW_TRACE_DEBUG("CSWMatchTransformFilter::MatchImage< CarLeft TriggerIndex = %d, CaptureImage trigger id = %d >.\n",
					pMatchInfo->pCarLeft->GetTriggerIndex(), pSignal->signal[i].pImage->GetFlag());
			if(0 < pSignal->signal[i].pImage->GetFlag() && 
					pSignal->signal[i].pImage->GetFlag() == pMatchInfo->pCarLeft->GetTriggerIndex())
			{
				SW_TRACE_DEBUG("match car left by trigger id=%d", pSignal->signal[i].pImage->GetFlag());
				dwMatchIndex = i;
				dwDiffTime = 0;
				break;
			}
		}
	}

	// 尽可能使用 trigger id 做匹配，刚开始匹配不上也需要等待一定时间
	if (dwMatchIndex == 0xFFFFFFFF)
	{
		DWORD dwPlatePassTime = CSWDateTime::GetSystemTick() - pMatchInfo->dwInputTime;
		if (dwPlatePassTime < m_cParam.dwPlateKeepTime / 2)
		{
			SW_TRACE_DEBUG("no match by trigger id , continue PlatePassTime = %d, LimitTime = %d.\n", dwPlatePassTime, m_cParam.dwPlateKeepTime / 2);
			return FALSE;
		}
	}


	//在判断临近时间点
	if(dwMatchIndex == 0xFFFFFFFF)
	{
		for(int i = 0; i < sizeof(pSignal->signal)/sizeof(pSignal->signal[0]); i++)
		{
			if(pSignal->signal[i].dwTime > 0)
			{
				if(pSignal->signal[i].dwTime >= pMatchInfo->dwInputTime
						&& pSignal->signal[i].dwTime - pMatchInfo->dwInputTime <= m_cParam.dwMatchMaxTime)
				{
					dwDiffTime = pSignal->signal[i].dwTime - pMatchInfo->dwInputTime;
					dwMatchIndex = i;
				}
				else if(pSignal->signal[i].dwTime < pMatchInfo->dwInputTime 
						&& pMatchInfo->dwInputTime - pSignal->signal[i].dwTime <= m_cParam.dwMatchMinTime)
				{
					dwDiffTime = pMatchInfo->dwInputTime - pSignal->signal[i].dwTime;
					dwMatchIndex = i;
				}
			}
		}
	}

	if(dwMatchIndex != 0xFFFFFFFF)
	{	
		//压缩图片
		//最后发送时再压缩图片。
		//CSWMessage::SendMessage(MSG_JPEGENCODE, (WPARAM)pSignal->signal[dwMatchIndex].pImage);
		//替换结果
		SW_RECT rc;
		swpa_memset(&rc, 0, sizeof(SW_RECT));
		if(pMatchInfo->pCarLeft->GetOutputLastImage())
			pMatchInfo->pCarLeft->SetImage(0, pSignal->signal[dwMatchIndex].pImage, NULL);
		pMatchInfo->dwMatchImageTime[1] = dwDiffTime;
		SW_TRACE_DEBUG("match image time:%d, carleft time:%d, interval:%d, carleft Trigger:%d", 
				pSignal->signal[dwMatchIndex].dwTime, 
				pMatchInfo->pCarLeft->GetCarArriveTime(), 
				(INT)pSignal->signal[dwMatchIndex].dwTime - (INT)pMatchInfo->pCarLeft->GetCarArriveTime(),
				pMatchInfo->pCarLeft->GetTriggerIndex());
		// 设置匹配标志
		pSignal->signal[dwMatchIndex].fIsMatch = TRUE;
		// 如果是主信号超时出的结果。
		if(pMatchInfo->dwOutputCondition == 1)
		{
			SW_TRACE_DEBUG("抓拍图主信号超时");
			pMatchInfo->pCarLeft->SetImage(0, pSignal->signal[dwMatchIndex].pImage, &rc);
			pMatchInfo->pCarLeft->SetImage(1, pSignal->signal[dwMatchIndex].pImage, &rc);
		}
		//信号超时则删除
		if(CSWDateTime::GetSystemTick() - pSignal->signal[dwMatchIndex].dwTime > m_cParam.dwSignalKeepTime)
		{
			SAFE_RELEASE(pSignal->signal[dwMatchIndex].pImage);
			pSignal->signal[dwMatchIndex].dwTime = 0;
			pSignal->signal[dwMatchIndex].dwSpeed = 0;
		}
	}	
	return dwDiffTime != NOT_MATCH_TIME;
}

bool CSWMatchTransformFilter::MatchTrigger(SIGNAL_PARAM *pSignal, CARLEFT_MATCH_INFO *pMatchInfo)
{
	return true;
}

/**
  @brief 外总控使能开关
  @param [in] 1使能 0不使能
 */
HRESULT CSWMatchTransformFilter::OnOutputCtrlEnable(WPARAM wParam, LPARAM lParam)
{
	INT iEnable = (INT)wParam;

	m_cParam.fEnable = (iEnable == 1) ? TRUE : FALSE;

	return S_OK;
}

