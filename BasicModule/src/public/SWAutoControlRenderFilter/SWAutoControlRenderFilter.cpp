#include "SWFC.h"
#include "SWMessage.h"
#include "SWAutoControlRenderFilter.h"
#include "SWCameraDataPDU.h"

CSWAutoControlRenderFilter::CSWAutoControlRenderFilter()
:CSWBaseFilter(2,0)
,CSWMessage(MSG_AUTO_CONTROL_START, MSG_AUTO_CONTROL_END)
{
	GetIn(0)->AddObject(CLASSID(CSWCameraDataPDU));
	GetIn(1)->AddObject(CLASSID(CSWImage));
	
	m_fEnable = FALSE;
	m_fEnableAGC = FALSE;
	m_iMinPSD = 100;
	m_iMaxPSD = 20000;
	m_iLightType = 0;
	m_iCplStatus = 0;
	m_iPluseLevel = 0;
	swpa_memset(m_irgAGCLimit, 0, sizeof(m_irgAGCLimit)/sizeof(m_irgAGCLimit[0]));
	swpa_memset(m_irgExposureTime, 0, sizeof(m_irgExposureTime)/sizeof(m_irgExposureTime[0]));
	swpa_memset(m_irgGain, 0, sizeof(m_irgGain)/sizeof(m_irgGain[0]));

	m_fEnableAutoCapture = FALSE;
	m_iDayShutterMax = 3800;
	m_iDayGainMax = 120;
	m_iNightShutterMax = 1500;
	m_iNightGainMax = 60;

	m_iGainMin = 10;
	m_iShutterMin = 200;

	// 默认为白天
	m_iIsDayCount = 120;
    m_iIsDuskCount = 120;
	m_fIsDay = FALSE;
	m_fIsDayEx = FALSE;
    m_iEnvType = 1;
	m_iCaptureShutter = 300;
	m_iCaptureGain = 60;
	m_iCaptureImageCount = 0;
	m_iTotalAvgY = 0;
	m_fNeedUpdateCaptureParam = FALSE;

	m_fIsInit = FALSE;
	m_fForceChange = FALSE;
	m_iEnableCpl = 0;

	m_iGammaMode = 2;

	m_nNightShutter = 50000;
	m_nNightThreshold = 0;
    m_nDuskThreshold = 0;

	m_nAvgPlateY = 0;

	m_nMinPlateY = 80;
	m_nMaxPlateY = 120;
	m_nWDRLevel = 0;
	m_fUseMaxAgcShutter = FALSE;

    m_iAGCDayNightShutterControl = 0;
    m_iAGCDayShutterHOri = 3800;
    m_iAGCNightShutterHOri = 1500;
	m_iAGCNightGainHOri = 60;
	m_iAGCGainHOri=120;

	m_iTempCaptureGain=60;
	m_iTempCaptureShutter=1500;

	m_dwAvgY=0;
}

CSWAutoControlRenderFilter::~CSWAutoControlRenderFilter()
{
}

HRESULT CSWAutoControlRenderFilter::Run()
{
	// 独立抓拍参数可独立运行。
	if(m_fEnable || m_fEnableAutoCapture)
	{
		m_thCameraPDU.Start(OnProcessCameraPDU, this);
	}
	return CSWBaseFilter::Run();
}

HRESULT CSWAutoControlRenderFilter::Stop()
{
	m_thCameraPDU.Stop();
	return CSWBaseFilter::Stop();
}

VOID CSWAutoControlRenderFilter::DoInit()
{
	if( !m_fIsInit )
	{
		m_fIsInit = TRUE;
		m_fForceChange = TRUE;

		INT iMode = 0;
		// 获取GAMMAMODE
		if( S_OK == SendMessage(MSG_GET_GAMMA_MODE, 0, (LPARAM)&iMode) )
		{
			SW_TRACE_DEBUG("<AutoControlRenderFilter>gamma mode:%d.", iMode);
			m_iGammaMode = iMode;			
			HRESULT hr = SendMessage(MSG_SET_GAMMA_MODE, m_fIsDay ? 0 : (WPARAM)m_iGammaMode, 0);
		}

        //SendMessage(MSG_SET_LED_SWITCH,m_fIsDay ? 0 : 1, 0);
	}
}

HRESULT CSWAutoControlRenderFilter::Receive(CSWObject* obj)
{    
	if(IsDecendant(CSWCameraDataPDU, obj) && TRUE == m_fEnable)
	{
		CSWCameraDataPDU* pdu = (CSWCameraDataPDU *)obj;
		
		// 只有做过白天晚上的判断后才进行频闪脉宽等级的修改。
		if( m_fIsInit )
		{
			// 晚上补光灯一定要亮，白天一定不亮。
			/*if(pdu->GetPluseLevel() <= 0 && !m_fIsDay)
			{
				pdu->SetPluseLevel(4);
			}
			else if(m_fIsDay)
			{
				pdu->SetPluseLevel(0);
			}*/
			pdu->SetPluseLevel(0);

			if (!m_fIsDay)
			{
				pdu->SetLightType(13);	//晚上强制13级
				pdu->SetWDRLevel(0);    //晚上强制关闭WDR
			}

			// 偏振镜目前也要保证晚上不使能。
			// todo.
			if( m_iEnableCpl == 1 )
			{

				if( m_fIsDay )
				{
					pdu->SetCplStatus(1);
				}
				else
				{
					pdu->SetCplStatus(0);
				}
			}
/*
				if( pdu->GetCplStatus() == 1 && !m_fIsDay )
				{
					pdu->SetCplStatus(0);
				}
				// 是否设置为只要白天偏振镜就要打开
				if( m_iEnableCpl == 1 && m_fIsDay && pdu->GetCplStatus() == 0 )
				{
					pdu->SetCplStatus(1);
				}
				*/
		}


		obj->AddRef();
		//if (pdu->GetAvgPlateY() != 0)
		//{
		//    m_nAvgPlateY = pdu->GetAvgPlateY();
		//}

		if((m_iLightType != pdu->GetLightType() 
			|| m_iCplStatus != pdu->GetCplStatus() 
			|| m_iPluseLevel != pdu->GetPluseLevel()
			|| m_nWDRLevel != pdu->GetWDRLevel())
			&& S_OK == m_lstPDU.Push(obj))
		{
			m_iLightType = pdu->GetLightType();
			m_iCplStatus = pdu->GetCplStatus();
			m_iPluseLevel = pdu->GetPluseLevel();
			m_nWDRLevel = pdu->GetWDRLevel();
			SW_TRACE_NORMAL("场景状态改变:[LightType:%d,CPL:%d, PluseLevel:%d, WDRLevel:%d]",
				m_iLightType, m_iCplStatus, m_iPluseLevel, m_nWDRLevel);
		}
		else
		{
			obj->Release();
		}

		static int s_nFrameCnt = 0;
		// 白天情况下5分钟强制刷新一次WDR
		if (m_fIsDay && s_nFrameCnt++ >= 3750)
		{
		    CSWCameraDataPDU *pCamPDU = new CSWCameraDataPDU();
            pCamPDU->SetLightType(m_iLightType);   //相机等级
            //pCamPDU->SetCplStatus(m_iCplStatus);   //偏光镜状态
            //pCamPDU->SetPluseLevel(m_iPluseLevel);  //频闪补光脉宽
            pCamPDU->SetWDRLevel(-1); //-1表示强制刷新
            if (S_OK != m_lstPDU.Push(pCamPDU))
            {
                pCamPDU->Release();
            }
            else
            {
                s_nFrameCnt = 0;
            }
		}
		else if (m_fIsDay == FALSE)
		{
		    s_nFrameCnt = 0;
		}
	}
	else if(IsDecendant(CSWImage, obj) && (m_fEnable || m_fEnableAutoCapture))
	{
		CSWImage* pImage = (CSWImage*)obj;

		if( pImage != NULL
		    && pImage->GetType() != SW_IMAGE_JPEG
		    && !(swpa_strcmp("VPIF", pImage->GetFrameName()) != 0))
		{
			IMAGE_EXT_INFO cInfo;
			pImage->GetImageExtInfo(&cInfo);
			// 判断白天黑夜
            if(!pImage->IsCaptureImage())
			{
                if ( FALSE )        // EC700IR 不需要再ARM端判断白天晚上
                {
                    int nNightThreshold = 0;
                    int nDuskThreshold = 0;
                    int nNightShutter = 0;
                    //if (m_fIsDay)
                    {
                        nNightThreshold = m_nNightThreshold;        // 夜晚判断环境亮度阈值
                        nDuskThreshold = m_nDuskThreshold;      // 傍晚判断环境亮度阈值
                        nNightShutter = m_nNightShutter;        // 夜晚判断快门阈值
                    }
    //			    else
    //			    {
    //			        nNightThreshold = m_nNightThreshold + 30;	//晚上亮度阈值调高，相当于晚上切白天要求亮度高一些
    //			        nNightThreshold = nNightThreshold > 255 ? 255 : nNightThreshold;
    //			        nNightShutter = m_nNightShutter - 200;
    //			        nNightShutter = nNightShutter < 0 ? 10 : nNightShutter;
    //                    nDuskThreshold = m_nDuskThreshold;      // 傍晚判断环境亮度阈值
    //			    }

                    //当晚上出现白色的车的时候，亮度小于的条件不容易满足，易误判为白天
                    if (cInfo.iShutter > nNightShutter && cInfo.iAvgY <= nDuskThreshold)
                    {
                        if( m_iIsDayCount > 0 )
                        {
                            m_iIsDayCount--;
                        }
                        else if( !m_fIsInit )
                        {
                            SW_TRACE_DEBUG("<AutoControlRenderFilter>day night init.\n");
                            DoInit();
                        }

                        if(3 == m_iEnvType && m_iIsDayCount < 200 )
                        {
                            m_fForceChange = TRUE;
                            m_fIsDay = FALSE;
                            m_fIsDayEx = TRUE;

                            if( !m_fIsInit )
                            {
                                SW_TRACE_DEBUG("<AutoControlRenderFilter>day night init.\n");
                                DoInit();
                            }
                        }

                        // 傍晚、晚上相互转换逻辑
                        {
                            if (cInfo.iAvgY <= nNightThreshold)
                            {
                                if( m_iIsDuskCount > 0 )
                                {
                                    m_iIsDuskCount --;
                                    SW_TRACE_DEBUG("AutoControlRenderFilter : cInfo.iAvgY(%d) <= nNightThreshold(%d), m_iIsDuskCount(%d).\n",
                                                   cInfo.iAvgY, nNightThreshold, m_iIsDuskCount);
                                }

                                if (1 != m_iEnvType && m_iIsDuskCount < 50)
                                {

                                    m_fIsDay = FALSE;
                                    m_fIsDayEx = FALSE;
                                }
                            }
                            else
                            {
                                SW_TRACE_DEBUG("AutoControlRenderFilter : cInfo.iAvgY(%d) > nNightThreshold(%d), m_iIsDuskCount(%d).\n",
                                               cInfo.iAvgY, nNightThreshold, m_iIsDuskCount);
                                if( m_iIsDuskCount < 500 )
                                {
                                    m_iIsDuskCount ++;
                                }
                                if (1 == m_iEnvType && m_iIsDuskCount > 300)
                                {
                                    m_fIsDay = FALSE;
                                    m_fIsDayEx = TRUE;
                                }
                            }
                        }
                    }
                    else
                    {
                        //从晚上切换到白天需要一定的帧数，以减小路面反光及白车对环境亮度的影响，晚上误判为白天
                        if( m_iIsDayCount < 1200 )
                        {
                            m_iIsDayCount++;
                        }
                        else if( !m_fIsInit )
                        {
                            SW_TRACE_DEBUG("<AutoControlRenderFilter>day night init.\n");
                            DoInit();
                        }
                        if( m_iIsDayCount > 1000 )
                        {
                            if( !m_fIsDay )
                            {
                                m_fForceChange = TRUE;
                                m_fIsDay = TRUE;

                                if( !m_fIsInit )
                                {
                                    SW_TRACE_DEBUG("<AutoControlRenderFilter>day night init.\n");
                                    DoInit();
                                }

                            }
						//	else
						//	{
						//		if( cInfo.iShutter > 3000 && !m_fIsDayEx)
						//		{
						//			m_fIsDayEx = TRUE;
						//		}
						//		else if( cInfo.iShutter < 2500 )
						//		{
						//			m_fIsDayEx = FALSE;
						//		}
						//	}
                        }
                    }
                }


				static int s_nFrameCount = 0;
				static DWORD s_dwTotalY = 0;
				s_dwTotalY += cInfo.iAvgY;
				if (s_nFrameCount++ >= 125)
				{
				    static char *s_prgM3Status[3] = {"晚上","傍晚","白天"};
				    static DWORD s_dwAvgY = 0;
				    int nStatus = -1;

				    s_dwAvgY = s_dwTotalY / s_nFrameCount;

                    SendMessage(MSG_GET_M3_DAYNIGHT_STATUS, 0, (LPARAM)&nStatus);

					nStatus = nStatus >= 1 ? nStatus - 1 : 0;
					nStatus = nStatus > 2 ? 2 : nStatus;
                    SW_TRACE_NORMAL("当前状态：%d-%d, M3状态：%s, 环境亮度：%d[%d],"
                        "LightType:%d, PluseLevel:%d, WDRLevel[%d], %d ,%d\n",
                       // m_fIsDay ? "白天" : m_fIsDayEx ? "傍晚" : "晚上",
                        m_fIsDay, m_fIsDayEx,
				        s_prgM3Status[nStatus],
				        s_dwAvgY, (m_irgAGCLimit[m_iLightType] - 10),
				        m_iLightType, m_iPluseLevel,
                        m_nWDRLevel, cInfo.iShutter,cInfo.iGain);

					m_dwAvgY=s_dwAvgY;	

				    s_nFrameCount = 0;
				    s_dwTotalY = 0;
                    OnSetCaptureAutoParam((WPARAM)0,(LPARAM)0);
					//INT nStatus = -1;
					SendMessage(MSG_RECOGNIZE_GET_DSP_ENV_LIGHT_TYPE, NULL, (LPARAM)&nStatus);

					switch(nStatus)
					{
						case 2:
							m_fIsDay=FALSE;
							m_fIsDayEx=FALSE;
							break;
						case 1:
							m_fIsDay=FALSE;
							m_fIsDayEx=TRUE;
							break;
						case 0:
							m_fIsDay=TRUE;
							m_fIsDayEx=(cInfo.iShutter > 3000);
							break;
					}
					static INT s_nLastStatus = -1;
					SendMessage(MSG_SET_DSP_FLAG,nStatus,NULL);

                    if ( m_iAGCDayNightShutterControl )
                    {
                        // 晚上需要较小的快门                   
                        //INT nStatus = -1;
                        //SendMessage(MSG_GET_M3_DAYNIGHT_STATUS, 0, (LPARAM)&nStatus);       // (3:day, 2:dusk, 1:night)
                        // 0白天，1傍晚，2晚上
                        if (nStatus != s_nLastStatus)
                        {
                            if (2 == nStatus)
                            {
                                //设置AGC快门范围
                                DWORD nValue= 0;
                                nValue = (m_iAGCNightShutterHOri << 16);
                                nValue |= (m_iAGCShutterLOri & 0x0000FFFF);
                                PostMessage(MSG_SET_SHU_RANGE, (WPARAM)nValue, NULL);
								nValue=0;
								nValue=(m_iAGCNightGainHOri<<16);
								nValue |= (10 & 0x0000FFFF);
								PostMessage(MSG_SET_GAIN_RANGE, (WPARAM)nValue, NULL);
								PostMessage(MSG_CAPTURERGB_ENABLE, (WPARAM)1, NULL);
                                SW_TRACE_DEBUG("Set Night ShutterHOri = %d GainHOri=%d ################\n", m_iAGCNightShutterHOri,m_iAGCNightGainHOri);
                            }
                            else
                            {
                                //设置AGC快门范围
                                DWORD nValue= 0;
                                nValue = (m_iAGCDayShutterHOri << 16);
                                nValue |= (m_iAGCShutterLOri & 0x0000FFFF);						
                                PostMessage(MSG_SET_SHU_RANGE, (WPARAM)nValue, NULL);
								nValue=0;
								nValue=(m_iAGCGainHOri<<16);
								nValue |= (10 & 0x0000FFFF);
								PostMessage(MSG_SET_GAIN_RANGE, (WPARAM)nValue, NULL);
								PostMessage(MSG_CAPTURERGB_ENABLE, (WPARAM)0, NULL);
                                SW_TRACE_DEBUG("Set Day ShutterHOri = %d GainHOri=%d ################\n", m_iAGCDayShutterHOri,m_iAGCGainHOri);
                            }
                        }
                        s_nLastStatus = nStatus;
                    }
				}
			}
			else if( pImage->IsCaptureImage()  && TRUE == m_fEnableAutoCapture) // 只有使能了抓拍自动控制才去调节。
			{
				UpdateCaptureParam(cInfo);
			}
		}
	}

	return S_OK;
}

VOID CSWAutoControlRenderFilter::UpdateCaptureParam(const IMAGE_EXT_INFO& cInfo)
{
	// 上次的调整还没处理
	if( m_fNeedUpdateCaptureParam ) return;

	INT iDayMaxAvgY = 180;
	INT iDayMinAvgY = 120;
	INT iDayExMaxAvgY = 140;
	INT iDayExMinAvgY = 90;

	INT iMaxAvgY = iDayMaxAvgY;
	INT iMinAvgY = iDayMinAvgY;

	// 如果是场景转变或是第一次初始化需要立即设置。
	if( m_fForceChange )
	{
		m_fForceChange = FALSE;
		//if( m_fIsDay || m_fIsDayEx )
		//{
		//	m_iCaptureShutter = m_iDayShutterMax;
		//	m_iCaptureGain = m_iDayGainMax; 
		//}
		//else
		//{
			//晚上取上次手动设定的最大值
			m_iCaptureGain=m_iTempCaptureGain;
			m_iCaptureShutter=m_iTempCaptureShutter;
			//m_iCaptureGain = m_iNightGainMax;
			//m_iCaptureShutter = m_iNightShutterMax;
		//}

		SW_TRACE_DEBUG("<AutoControlRenderFilter>ForceChange. shutter:%d,gain:%d.", m_iCaptureShutter, m_iCaptureGain);
		m_fNeedUpdateCaptureParam = TRUE;
		return;
	}

	if( m_fIsDay || m_fIsDayEx)
	{
		// 阴天情况下，图片亮度不能太高。
		if( m_fIsDayEx )
		{
			iMaxAvgY = iDayExMaxAvgY;
			iMinAvgY = iDayExMinAvgY;
		}
		else
		{
			iMaxAvgY = iDayMaxAvgY;
			iMinAvgY = iDayMinAvgY;
		}
		// 每两张抓拍图调整一次
		m_iCaptureImageCount++;
		m_iTotalAvgY += cInfo.iAvgY;

		if( m_iCaptureImageCount == 2 )
		{
			m_iTotalAvgY /= 2;
			SW_TRACE_DEBUG("m_iTotalAvgY=%d\n",m_iTotalAvgY);
			DWORD m_iCaptureGainT=m_iCaptureGain;
			DWORD m_iCaptureShutterT=m_iCaptureShutter;
			if( m_iTotalAvgY < iMinAvgY )
			{
				m_fNeedUpdateCaptureParam = FALSE;
				if( m_iCaptureGain < m_iDayGainMax )
				{
					m_iCaptureGain += 10;
					m_iCaptureGain = m_iCaptureGain > m_iDayGainMax ? m_iDayGainMax : m_iCaptureGain;
					m_fNeedUpdateCaptureParam = TRUE;
				}
				else
				{
					if(m_iCaptureGain>m_iDayGainMax)
					{
						m_iCaptureGain=m_iDayGainMax;
						m_fNeedUpdateCaptureParam = TRUE;
					}
				}
				if( m_iCaptureShutter < m_iDayShutterMax )
				{
					m_iCaptureShutter += 100;
					m_iCaptureShutter = m_iCaptureShutter > m_iDayShutterMax ? m_iDayShutterMax : m_iCaptureShutter;
					m_fNeedUpdateCaptureParam = TRUE;
				}
				else
				{
					m_iCaptureShutter=m_iDayShutterMax;
					m_fNeedUpdateCaptureParam = TRUE;
				}
			}
			else if( m_iTotalAvgY > iMaxAvgY )
			{
				m_fNeedUpdateCaptureParam = FALSE;
				if( m_iCaptureShutter > m_iShutterMin )
				{
					m_iCaptureShutter -= 100;
					m_iCaptureShutter = m_iCaptureShutter < m_iShutterMin ? m_iShutterMin : m_iCaptureShutter;
					m_fNeedUpdateCaptureParam = TRUE;
				}
				else
				{
					m_iCaptureShutter=m_iShutterMin;
					m_fNeedUpdateCaptureParam = TRUE;
				}
				if( m_iCaptureGain > m_iGainMin )
				{
					m_iCaptureGain -= 10;
					m_iCaptureGain = m_iCaptureGain < m_iGainMin ? m_iGainMin : m_iCaptureGain;
					m_fNeedUpdateCaptureParam = TRUE;
				}
				else
				{
					m_iCaptureGain=m_iGainMin;
					m_fNeedUpdateCaptureParam = TRUE;
				}
			}
			if(m_iCaptureShutterT==m_iCaptureShutter||m_iCaptureGainT==m_iCaptureGain)
			{
				m_fNeedUpdateCaptureParam=FALSE;
			}
			m_iCaptureImageCount = 0;
			m_iTotalAvgY = 0;
		}
	}
	else
	{
		// 晚上的调整方式需要抓拍图重识别后的车牌亮度来处理。如果无此值晚上用默认的一套参数。
		if( m_iCaptureShutter != m_iTempCaptureShutter || m_iCaptureGain != m_iTempCaptureGain )
		{
		//	m_iCaptureGain = m_iNightGainMax;
		//	m_iCaptureShutter = m_iNightShutterMax;
			//晚上取上次手动设定的最大值
			m_iCaptureGain=m_iTempCaptureGain;
			m_iCaptureShutter=m_iTempCaptureShutter;
			m_fNeedUpdateCaptureParam = TRUE;
		}
	}

	SW_TRACE_DEBUG("抓拍增益：%d,抓拍快门：%d\n",m_iCaptureGain,m_iCaptureShutter);
}

void* CSWAutoControlRenderFilter::OnProcessCameraPDU(void* pvParam)
{
	CSWAutoControlRenderFilter* pThis = (CSWAutoControlRenderFilter*)pvParam;
	INT iLightType = -1, iCplStatus = -1, iPluseLevel = -1, iWDRLevel = 0;
	while(S_OK == pThis->m_thCameraPDU.IsValid())
	{
		CSWCameraDataPDU* pdu = (CSWCameraDataPDU *)pThis->m_lstPDU.Pop(100);
		if(pdu)
		{	
			//摄像机等级
			if(iLightType != pdu->GetLightType() && pdu->GetLightType() < 14)
			{
				iLightType = pdu->GetLightType();				
				//AGC调整
				if(TRUE == pThis->m_fEnableAGC)
				{
					SW_TRACE_DEBUG("摄像机参数改变:%d[AGCLimit:%d]", iLightType, pThis->m_irgAGCLimit[iLightType]);
					CSWMessage::PostMessage(MSG_SET_AGCTH, pThis->m_irgAGCLimit[iLightType]);
				}
				//曝光时间、增益调整
				else
				{
					SW_TRACE_DEBUG("摄像机参数改变:%d[ExporeTime:%d, Gain:%d]", iLightType, pThis->m_irgExposureTime[iLightType], pThis->m_irgGain[iLightType]);
					CSWMessage::PostMessage(MSG_SET_AGCGAIN, pThis->m_irgGain[iLightType]);
					CSWMessage::PostMessage(MSG_SET_SHUTTER, pThis->m_irgExposureTime[iLightType]);
				}
			}
			
			//偏光镜状态
			if(iCplStatus != pdu->GetCplStatus())
			{
				iCplStatus = pdu->GetCplStatus();
				int iSwitch = (iCplStatus == 0) ? 1 : 2;
				SW_TRACE_DEBUG("偏振镜状态:[%s]", iCplStatus == 0 ? "不使能" : "使能");
				CSWMessage::PostMessage(MSG_SET_FILTERSWITCH, iSwitch, 0);
				
			}
			
			//频闪灯脉宽等级
			/*if(iPluseLevel != pdu->GetPluseLevel())
			{
				iPluseLevel = pdu->GetPluseLevel();
				if(0 == iPluseLevel)
				{
					SendMessage(MSG_SET_LED_SWITCH,(WPARAM)0,0);	//LED off
				}
				else
				{
					INT iStep = (pThis->m_iMaxPSD - pThis->m_iMinPSD)/7;
					INT iPulseWidth = (pThis->m_iMinPSD + (iPluseLevel -1)* iStep)/100;//us转为1/10ms
					SW_TRACE_DEBUG("频闪灯脉宽等级:[%d,%d],PSD(%d~%d),step:%d", 
						iPluseLevel, iPulseWidth,pThis->m_iMinPSD,pThis->m_iMaxPSD,iStep);
					//CSWMessage::SendMessage(MSG_SET_FLASHRATE_ENABLE, TRUE);
					//CSWMessage::SendMessage(MSG_SET_FLASHRATE_PULSE, iPulseWidth);
					CSWMessage::SendMessage(MSG_SET_EXP_PLUSEWIDTH, iPulseWidth);
				}
			}*/
						//WDR
			if (iWDRLevel != pdu->GetWDRLevel())
			{
			    if (pdu->GetWDRLevel() >= 0 && pdu->GetWDRLevel() <= 4)
			    {
                    iWDRLevel = pdu->GetWDRLevel();
                    SendMessage(MSG_SET_WDR_LEVEL, (WPARAM)iWDRLevel, 0);
                    // 关闭WDR后需要重新设置AGC基准值
                    if (iWDRLevel == 0 && pThis->m_fEnableAGC)
                    {
                        PostMessage(MSG_SET_AGCTH, pThis->m_irgAGCLimit[13]);
                    }
			    }
			    else if (pdu->GetWDRLevel() < 0) //强制刷新WDR等级
			    {
			        PostMessage(MSG_SET_WDR_LEVEL, (WPARAM)iWDRLevel, 0);
			    }
			}		
			pdu->Release();
			//保存参数
			pThis->SaveParam(iLightType, iCplStatus, iPluseLevel);
		}
		else if( pThis->m_fNeedUpdateCaptureParam ) // 调整抓拍参数
		{
			// 只要抓拍参数不为-1， 会默认使能抓拍独立参数。
			CSWMessage::PostMessage(MSG_SET_CAPTURESHUTTER, (WPARAM)&pThis->m_iCaptureShutter, 0);
			CSWMessage::PostMessage(MSG_SET_CAPTRUEGAIN, (WPARAM)&pThis->m_iCaptureGain, 0);
			SW_TRACE_DEBUG("<AutoControlRenderFilter>Capture change shutter:%d,gain:%d.\n", pThis->m_iCaptureShutter, pThis->m_iCaptureGain);
			pThis->m_fNeedUpdateCaptureParam = FALSE;
		}
	}
	return 0;
}

HRESULT CSWAutoControlRenderFilter::SaveParam(INT iLightType, INT iCplStatus, INT iPluseLevel)
{
	CSWFile file;
	if(S_OK == file.Open("BLOCK//tmp/.light.dat", "w+"))
	{
		file.Write(&iLightType, sizeof(INT));		
		file.Write(&iPluseLevel, sizeof(INT));
		file.Write(&iCplStatus, sizeof(INT));
		SW_TRACE_DEBUG("<writeparam>lighttype:%d,iPluse:%d,iCpl:%d.\n", iLightType, iPluseLevel, iCplStatus);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CSWAutoControlRenderFilter::OnReadParam(WPARAM wParam, LPARAM lParam)
{
	CSWFile file;
	INT *iParam = (INT *)wParam;
	if(S_OK == file.Open("BLOCK//tmp/.light.dat", "r"))
	{
		file.Read(iParam, 3*sizeof(INT));
		SW_TRACE_DEBUG("<readparam>lighttype:%d,iPluse:%d,iCpl:%d.\n", iParam[0], iParam[1], iParam[2]);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CSWAutoControlRenderFilter::OnGetRealParam(WPARAM wParam, LPARAM lParam)
{
	INT* piBuffer = (INT*)lParam;
	if( piBuffer != NULL )
	{
		piBuffer[0] = m_iLightType;
		piBuffer[1] = m_iCplStatus;
		piBuffer[2] = m_iPluseLevel;
		return S_OK;
	}
	return E_INVALIDARG;
}

HRESULT CSWAutoControlRenderFilter::OnGetEnvPeriod(WPARAM wParam, LPARAM lParam)
{
	INT *piIsNight = (INT *)lParam;
	if (NULL == piIsNight)
	{
		return E_INVALIDARG;
	}
	*piIsNight = 0;
	if (FALSE == m_fIsDay && FALSE == m_fIsDayEx)
	{
		*piIsNight = 1;
	}
	
	return S_OK;
}

HRESULT CSWAutoControlRenderFilter::Enable(BOOL fEnable,
        BOOL fEnableAGC,
        INT iMinPSD,
        INT iMaxPSD,
        INT iEnableCpl,
        INT iMinPlateY,
        INT iMaxPlateY)
{
	m_fEnable = fEnable;
	m_fEnableAGC = fEnableAGC;
	m_iMinPSD = iMinPSD;
	m_iMaxPSD = iMaxPSD;
	m_iEnableCpl = iEnableCpl;
	m_nMinPlateY = iMinPlateY;
	m_nMaxPlateY = iMaxPlateY;

	// AGC不自动使能，由前端相机参数确定。
	
	/*if( m_fEnable )
	{
		CSWMessage::SendMessage(MSG_SET_AGCENABLE, m_fEnableAGC);
	}*/
	
	return S_OK;
}

HRESULT CSWAutoControlRenderFilter::SetLevel(INT iLevel, INT iAGCLimit, INT iExposureTime, INT iGain)
{
	SW_TRACE_NORMAL("level:%d, AGCLimit:%d, ExporeTime:%d,Gain:%d\n", iLevel, iAGCLimit, iExposureTime, iGain);
	if(iLevel < 14)
	{
		m_irgAGCLimit[iLevel] = iAGCLimit;
		m_irgExposureTime[iLevel] = iExposureTime;
		m_irgGain[iLevel] = iGain;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CSWAutoControlRenderFilter::SetAutoCaptureParam(BOOL fEnable, INT iDayShutter, INT iDayGain, INT iNightShutter, INT iNightGain)
{
	m_fEnableAutoCapture = fEnable;
	m_iDayShutterMax = iDayShutter;
	m_iDayGainMax = iDayGain;
	m_iNightShutterMax = iNightShutter;
	m_iNightGainMax = iNightGain;

    return S_OK;
}

HRESULT CSWAutoControlRenderFilter::SetCaptureShutterGain(INT iCaptureShutter, INT iCaptureGain)
{
    m_iCaptureShutter = m_iTempCaptureShutter = iCaptureShutter;
    m_iCaptureGain = m_iTempCaptureGain = iCaptureGain;
    return S_OK;
}

HRESULT CSWAutoControlRenderFilter::OnSetMaxAgcShutter(WPARAM wParam, LPARAM lParam)
{
    if (m_fUseMaxAgcShutter)
    {
        m_nNightShutter = ((int)wParam - 200) >= 0 ? ((int)wParam - 200) : 0;
    }
    return S_OK;
}

HRESULT CSWAutoControlRenderFilter::SetNightThresholdArg(INT nNightShutter, INT nNightThreshold, INT nDuskThreshold, INT nMaxAgcShutter)
{
	if (nNightThreshold < 0)
	{
		SW_TRACE_DEBUG("Err: invalid arg: %d,%d\n", nNightShutter, nNightThreshold);
		return E_INVALIDARG;
	}

	if (nNightShutter == -1)
	{
	    m_fUseMaxAgcShutter = TRUE;
	    nNightShutter = (nMaxAgcShutter - 200) >= 0 ? (nMaxAgcShutter - 200) : 0;
	}
	else
	{
	    m_fUseMaxAgcShutter = FALSE;
	}

	m_nNightShutter = nNightShutter;
	m_nNightThreshold = nNightThreshold;
    m_nDuskThreshold = nDuskThreshold;

    return S_OK;
}

HRESULT CSWAutoControlRenderFilter::SetDayNightShutterHOri(INT iDayNightShutterEnable, INT iDayShutterHOri, INT iNightShutterHOri, INT iGainHOri, INT iNightGainHOri, INT iShutterLOri)
{
    m_iAGCDayNightShutterControl = iDayNightShutterEnable;
    m_iAGCDayShutterHOri = iDayShutterHOri;
    m_iAGCNightShutterHOri = iNightShutterHOri;
	m_iAGCNightGainHOri= iNightGainHOri;
    m_iAGCShutterLOri = iShutterLOri;
	m_iAGCGainHOri=iGainHOri;
    return S_OK;
}

HRESULT CSWAutoControlRenderFilter::SetRealTimeDayNightShutterHOri(WPARAM wParam, LPARAM lParam)
{
	m_iAGCNightShutterHOri = (DWORD)wParam;
	m_iAGCNightGainHOri= (DWORD)lParam;
    return S_OK;
}

HRESULT CSWAutoControlRenderFilter::OnSetCaptureAutoParam(WPARAM wParam, LPARAM lParam)
{
//	INT iAGCEnable = 0;
//	static INT iLastGain = 0;
//	static INT iLastShutter = 0;

//	const INT cniMaxCaptureShutter = 300;
//	const INT cniMinCaptureGainShutter = 300;
//	INT iGetShutter = 0, iSetShutter = 0;
//	INT iSetGain = 0;
	
    DWORD dwRegValue[2] = {0};
	dwRegValue[0] = 0x914;
	dwRegValue[1] = 0;
	CSWMessage::SendMessage(MSG_GET_CAM_FPGA_REG, NULL, (LPARAM)dwRegValue);

	if((dwRegValue[1] & 0x3) != 0x3)
	{
		dwRegValue[1] |= 0x3;	//强制使能抓拍快门和增益
		CSWMessage::PostMessage(MSG_SET_CAM_FPGA_REG, (WPARAM)dwRegValue, NULL);

        CSWMessage::PostMessage(MSG_SET_CAPTURESHUTTER, (WPARAM)(DWORD*)&m_iCaptureShutter, NULL);
        CSWMessage::PostMessage(MSG_SET_CAPTRUEGAIN, (WPARAM)(DWORD*)&m_iCaptureGain, NULL);
	}



    // EC700IR  手动设置抓拍快门、增益，不必动态控制
//	CSWMessage::SendMessage(MSG_GET_SHUTTER, NULL, (LPARAM)&iGetShutter);	//视频流快门

//	//大于最大值则用最大值，小于则拥视频流快门
//	iSetShutter = iGetShutter > cniMaxCaptureShutter ? cniMaxCaptureShutter : iGetShutter;
//	//从快门300到1500，每快门增加100增益+5，快门1500以上则保持70的增益值
//	if(iGetShutter > cniMinCaptureGainShutter)
//	{
//		INT iTmpShutterDiff = iGetShutter - cniMinCaptureGainShutter;
//		INT iGainIndex = iTmpShutterDiff / 100;

//		iGainIndex = iGainIndex > 12 ? 12 : iGainIndex;

//		iSetGain = 10 + (5 * iGainIndex);
//	}
//	else
//	{
//		iSetGain = 10;
//	}


//	//设置抓拍增益
//	dwRegValue[0] = 0x919;
//	dwRegValue[1] = iSetGain;
//	CSWMessage::SendMessage(MSG_SET_CAM_FPGA_REG, (WPARAM)dwRegValue, NULL);
//	iLastGain = iSetGain;


//	//设置抓拍快门
//	dwRegValue[0] = 0x915;
//	dwRegValue[1] = iSetShutter / (INT)29.6;
//	CSWMessage::SendMessage(MSG_SET_CAM_FPGA_REG, (WPARAM)dwRegValue, NULL);
//	iLastShutter = iSetShutter;

	return S_OK;
}


