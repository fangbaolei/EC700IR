#include "ObjectTrackingInfo.h"
#include "TrackInfoHigh.h"

CObjTrackInfo::CObjTrackInfo()
:m_objTrackState(OBJ_TRACK_INIT)
,m_fTrack(FALSE)
,m_cPlateTrackID(0)
,m_fCarArrived(false)
,m_fVoted(false)
,m_iObjInfoCount(0)
,m_iMissingCount(0)
,m_pFirstRefImage(NULL)
,m_pBestReftImage(NULL)
,m_pLastRefImage(NULL)
,m_pOneRefImage(NULL)
,m_pTwoRefImage(NULL)
,m_pThreeRefImage(NULL)
,m_iBestDistance(0)
,m_nVotedCarType(CT_UNKNOWN)
,m_nCarArriveTime(0)
,m_nCarArrivedCount(0)
,m_iRoad(-1)
,m_rcFirstCarPos(0, 0, 0, 0)
,m_rcBestCarPos(0, 0, 0, 0)
,m_rcLastCarPos(0, 0, 0, 0)
,m_iOnePosLightScene(-1)
,m_iTwoPosLightScene(-1)
,m_iThreePosLightScene(-1)
,m_iPassStopLightScene(-1)
,m_iPassLeftStopLightScene(-1)
,m_fIsMan(false)
,m_iObjFrameCount(0)
,m_fIsTrigger(false)
,m_dwTriggerIndex(0)
,m_iCapCount(0)
,m_fEPOutput(true)
,m_pObj(NULL)
,m_fNewTem(false)
,m_iObjStopFrame(0)
,m_iTemYBegin(0)
,m_fFirstPredict(false)
,m_iTrackID(0)
,m_iEPDetSkipFrames(0)
,m_iEPNoPlateScore(0)
{
	m_stCarInfo.rcFG = CRect(0,0,0,0);
	m_stCarInfo.fltCarH = 0.0f;
	m_stCarInfo.fltCarW = 0.0f;
	m_bClearImage = false;
	m_dwFirstFlameTime = 0;

	m_dwBestSnapShotRefTime = 0;
	m_dwLastSnapShotRefTime = 0;
	m_dwBeginCaptureRefTime = 0;
	m_dwBestCaptureRefTime = 0;
	m_dwLastCaptureRefTime = 0;

	//zhaopy
	m_fHasTrigger = false;
	m_dwTriggerTimeMs = 0;
	m_dwTriggerImageTime = 0;
}

CObjTrackInfo::~CObjTrackInfo()
{
	//Clear();
}

HRESULT CObjTrackInfo::New(
	PROCESS_ONE_FRAME_PARAM* pParam,
	CObjInfo &objInfo, 
	HV_COMPONENT_IMAGE &imgCurFrame, 
	int iLightScene,
	int iCurLight, 
	bool fIsMan, 
	PROCESS_ONE_FRAME_RESPOND* pProcessRespond 
	)
{
	HRESULT hr = S_OK;
	if (m_objTrackState != OBJ_TRACK_INIT || GetHvImageData(&imgCurFrame, 0) == NULL)
	{
		return E_UNEXPECTED;
	}

	// 内存操作日志
	IMG_MEM_OPER_LOG* pImgMemOperLog = &pProcessRespond->cImgMemOperLog;

	m_fIsMan = fIsMan;				// 行人标志
	m_iObjFrameCount++;
	for(int i = 0; i < 200; i++)
	{
		m_rgfltFrameSpeed[i] = 0.0;
	}

	m_rgObjInfo[m_iObjInfoCount] = objInfo;
	m_iObjInfoCount++;
	m_dwBestSnapShotRefTime = 0;
	m_dwLastSnapShotRefTime = 0;
	m_dwBeginCaptureRefTime = 0;
	m_dwBestCaptureRefTime = 0;
	m_dwLastCaptureRefTime = 0;

	//zhaopy
	m_fHasTrigger = false;
	m_dwTriggerTimeMs = 0;
	m_dwTriggerImageTime = 0;
	
	if( GetCurrentParam()->m_fUsedTrafficLight != 1 )
	{
//	m_pFirstRefImage = pParam->pCurFrame;
//	ImageAddRef(pImgMemOperLog, m_pFirstRefImage);
		m_pBestReftImage = pParam->pCurFrame;
		ImageAddRef(pImgMemOperLog, m_pBestReftImage);
		
		m_pLastRefImage = pParam->pCurFrame;
		ImageAddRef(pImgMemOperLog, m_pLastRefImage);
	
		m_rcFirstCarPos = objInfo.m_rcFG;
		m_rcBestCarPos = objInfo.m_rcFG;
		m_rcLastCarPos = objInfo.m_rcFG;

		m_dwBestSnapShotRefTime = pParam->dwImageTime;
		m_dwLastSnapShotRefTime = pParam->dwImageTime;
	}
	else
	{
		m_pOneRefImage = pParam->pCurFrame;
		ImageAddRef(pImgMemOperLog, m_pOneRefImage);
		m_pTwoRefImage = pParam->pCurFrame;
		ImageAddRef(pImgMemOperLog, m_pTwoRefImage);
		m_pThreeRefImage = pParam->pCurFrame;
		ImageAddRef(pImgMemOperLog, m_pThreeRefImage);
		m_rgRect[0] = m_rgRect[1] = m_rgRect[2] = objInfo.m_rcFG;
		m_iOnePosLightScene = m_iTwoPosLightScene = m_iThreePosLightScene = iLightScene;
		m_dwBeginCaptureRefTime = pParam->dwImageTime;
		m_dwBestCaptureRefTime = pParam->dwImageTime;
		m_dwLastCaptureRefTime = pParam->dwImageTime;
	}

	int dx = (imgCurFrame.iWidth >> 1) - (objInfo.m_rcFG.right - objInfo.m_rcFG.left);
	int dy = (imgCurFrame.iHeight >> 1) - (objInfo.m_rcFG.bottom - objInfo.m_rcFG.top);
	m_iBestDistance = dx * dx + dy * dy;
	m_objTrackState = OBJ_TRACK_NEW;
	m_fTrack = TRUE;
	m_iVideoID = pParam->iVideoID;

	m_stCarInfo.rcFG = CRect(0,0,0,0);
	m_stCarInfo.fltCarH = 0.0f;
	m_stCarInfo.fltCarW = 0.0f;
	
	m_fFirstTrigger = false;
	m_fSecondTrigger = false;
	m_fThirdTrigger = false;

	m_dwFirstTime = 0;
	m_dwSecondTime = 0;
	m_dwThirdTime = 0;

	m_dwFirstTimeMs = 0;
	m_dwSecondTimeMs = 0;
	m_dwThirdTimeMs = 0;
	m_bClearImage = false;

	m_dwFirstFlameTime = pParam->dwImageTime;

	m_fIsTrigger = false;
	m_dwTriggerIndex = 0;
	m_iCapCount = 0;
	//跟踪标识id
	static UINT iTrackIdCount = 1;
	m_iTrackID = iTrackIdCount++;

	return hr;
}

HRESULT CObjTrackInfo::Process(
	CObjInfo &objInfo, 
	HV_COMPONENT_IMAGE &imgCurFrame, 
	HiVideo::CRect rcVideoDet, 
	int iLightScene, 
	int iCurLight, 
	PROCESS_ONE_FRAME_PARAM* pParam,
	PROCESS_ONE_FRAME_RESPOND* pProcessRespond
	)
{
	if (m_objTrackState == OBJ_TRACK_INIT ||
		m_objTrackState == OBJ_TRACK_END)
	{
		return E_OBJ_NO_INIT;
	}

	// 内存操作日志
	IMG_MEM_OPER_LOG* pImgMemOperLog = &pProcessRespond->cImgMemOperLog;
	bool fReplace(false);
	bool fNew(false);
	if (objInfo.m_dwFrameTime == LastInfo().m_dwFrameTime)
	{
		HiVideo::CRect rcIntersectNew;
		if (rcIntersectNew.IntersectRect(&(m_rgObjInfo[m_iObjInfoCount - 2].m_rcFG), &(objInfo.m_rcFG)))
		{
			HiVideo::CRect rcIntersectOld;
			rcIntersectOld.IntersectRect(&(m_rgObjInfo[m_iObjInfoCount - 2].m_rcFG), &(LastInfo().m_rcFG));
			fReplace = (rcIntersectNew.Area() > rcIntersectOld.Area());
		}
	}
	else
	{
		m_fTrack = objInfo.m_rcFG.IntersectsWith(LastInfo().m_rcFG);
		fNew = (m_fTrack == TRUE);
	}

	if (fReplace || fNew)
	{
		if (fReplace) m_iObjInfoCount--;
		m_rgObjInfo[m_iObjInfoCount] = objInfo;

		//判断移动物体是否停止
		if (m_iObjInfoCount > 1 && abs(m_rgObjInfo[m_iObjInfoCount].m_rcFG.CenterPoint().y - m_rgObjInfo[m_iObjInfoCount - 1].m_rcFG.CenterPoint().y) > 2 && m_iObjFrameCount < 200)
		{			
			DWORD32 dwDiffTime = m_rgObjInfo[m_iObjInfoCount].m_dwFrameTime - m_rgObjInfo[0].m_dwFrameTime;
			int iDiffY = abs(m_rgObjInfo[m_iObjInfoCount].m_rcFG.CenterPoint().y - m_rgObjInfo[0].m_rcFG.CenterPoint().y);
			m_rgfltFrameSpeed[m_iObjFrameCount - 1] = float(iDiffY * 1.0 / (dwDiffTime / 1000.0));
			m_iObjFrameCount++;
		}

		if (m_iObjInfoCount < MAX_OBJINFO_COUNT - 1)
		{
			m_iObjInfoCount++;
		}
		else
		{
			m_rgObjInfo[m_iObjInfoCount - 1] = m_rgObjInfo[m_iObjInfoCount];
		}
		//黄国超增加限制,电警下仅需要轨迹图
#ifndef VERSION_IR
		if( GetCurrentParam()->m_fUsedTrafficLight != 1 )
#endif // #ifndef VERSION_IR
		{
			bool fUpdate = false;
			if (LastInfo().m_rcFG.CenterPoint().y > m_rgObjInfo[0].m_rcFG.CenterPoint().y)
			{
				if (LastInfo().m_rcFG.bottom > m_rcLastCarPos.bottom) fUpdate = true;
			}
			else
			{
				if (LastInfo().m_rcFG.top < m_rcLastCarPos.top) fUpdate = true;
			}
			if (fUpdate)
			{
				if (m_pLastRefImage)
				{
					ImageRelease(pImgMemOperLog, m_pLastRefImage);
					m_pLastRefImage = NULL;
				}
				if(!m_bClearImage)
				{
					m_pLastRefImage = pParam->pCurFrame;
					ImageAddRef(pImgMemOperLog, m_pLastRefImage);
					m_dwLastSnapShotRefTime = pParam->dwImageTime;
				}
				m_rcLastCarPos = LastInfo().m_rcFG;
			}
		}
		CRect rcPos = LastInfo().m_rcFG;
		int iPosOneY = CTrackInfo::m_iCaptureOnePos * CTrackInfo::m_iHeight / 100;
		int iPosTwoY = CTrackInfo::m_iCaptureTwoPos * CTrackInfo::m_iHeight / 100;
		int iPosThreeY = CTrackInfo::m_iCaptureThreePos * CTrackInfo::m_iHeight / 100;
		int iPosStop  = CTrackInfo::m_iStopLinePos * CTrackInfo::m_iHeight / 100;
		int iPosLeftStop  = CTrackInfo::m_iLeftStopLinePos * CTrackInfo::m_iHeight / 100;
		int iLastImgPos = m_rgRect[2].CenterPoint().y;
		CRect rcFirstPos = rcPos;
		if( m_iObjInfoCount > 1 )
		{
			rcFirstPos = m_rgObjInfo[0].m_rcFG;
		}
		//判断是否是逆行
		int iReverseRunLevel = 2 * CTrackInfo::m_iHeight / 100;
		bool fReverseRun = false;
		if( GetCurrentParam()->g_MovingDirection == MD_TOP2BOTTOM )
		{
			fReverseRun = (rcFirstPos.CenterPoint().y - rcPos.CenterPoint().y) > iReverseRunLevel ?  true : false;
		}
		else if( GetCurrentParam()->g_MovingDirection == MD_BOTTOM2TOP )
		{
			fReverseRun = (rcPos.CenterPoint().y - rcFirstPos.CenterPoint().y) > iReverseRunLevel ?  true : false;
		}
		if( GetCurrentParam()->m_fUsedTrafficLight == 1 )
		{
#ifdef VERSION_IR
			//第一个位置
			if( rcPos.CenterPoint().y > iPosOneY  
				|| ((rcPos.bottom > (CTrackInfo::m_iHeight * 95 / 100)) && rcPos.CenterPoint().y > iPosStop) )
			{
				if( m_pOneRefImage != NULL )
				{
					ImageRelease(pImgMemOperLog, m_pOneRefImage);
					m_pOneRefImage = NULL;
				}
				if(!m_bClearImage)
				{
					m_pOneRefImage = pParam->pCurFrame;
					if( m_pOneRefImage != NULL)
						ImageAddRef(pImgMemOperLog, m_pOneRefImage);
				}
				m_rgRect[0] = LastInfo().m_rcFG;
				m_iOnePosLightScene = iLightScene;
				m_dwBeginCaptureRefTime = pParam->dwImageTime;
			}

			//第二个位置(不是逆行时,用最后一张图片来更新)
			if( (!fReverseRun && iLastImgPos > iPosTwoY) 
				|| (fReverseRun && rcPos.CenterPoint().y < iPosOneY) )
			{
				if (m_pTwoRefImage)
				{
					ImageRelease(pImgMemOperLog, m_pTwoRefImage);
					m_pTwoRefImage = NULL;
				}
				if( fReverseRun ) 
				{
					if(!m_bClearImage)
					{
						m_pTwoRefImage = pParam->pCurFrame;
					}
					m_iTwoPosLightScene = iLightScene;
					m_rgRect[1] = LastInfo().m_rcFG;
				}
				else 
				{
					if(!m_bClearImage)
						m_pTwoRefImage = m_pThreeRefImage;
					m_iTwoPosLightScene = m_iThreePosLightScene;
					m_rgRect[1] = m_rgRect[2];
				}
				m_dwBestCaptureRefTime = pParam->dwImageTime;

				if( m_pTwoRefImage != NULL )
				{
					ImageAddRef(pImgMemOperLog, m_pTwoRefImage);
				}
			}
			//是否过线
			if( m_iPassStopLightScene == -1 && rcPos.CenterPoint().y < iPosStop )
			{
				m_iPassStopLightScene = iLightScene;
			}
			if( m_iPassLeftStopLightScene == -1 && rcPos.CenterPoint().y < iPosLeftStop )
			{
				m_iPassLeftStopLightScene = iLightScene;
			}

			if( (fReverseRun && rcPos.CenterPoint().y < iPosTwoY) 
				|| !fReverseRun)
			{
				if (m_pThreeRefImage)
				{
					ImageRelease(pImgMemOperLog, m_pThreeRefImage);
					m_pThreeRefImage = NULL;
				}
				if(!m_bClearImage)
				{
					m_pThreeRefImage = pParam->pCurFrame;
					if ( m_pThreeRefImage )
					{
						ImageAddRef(pImgMemOperLog, m_pThreeRefImage);
					}
				}
				m_iThreePosLightScene = iLightScene;
				m_dwLastCaptureRefTime = pParam->dwImageTime;
			}

			m_rgRect[2] = LastInfo().m_rcFG;
			
				//是否过线
				if( m_iPassStopLightScene == -1 && rcPos.CenterPoint().y < iPosStop )
				{
					m_iPassStopLightScene = iLightScene;
				}
				if( m_iPassLeftStopLightScene == -1 && rcPos.CenterPoint().y < iPosLeftStop )
				{
					m_iPassLeftStopLightScene = iLightScene;
				}
				
#else // #ifdef VERSION_IR
			if (fReverseRun)
			{
				//TODO
			}
			else
			{
				//第一个位置, 第一个位置以底坐标来判断
				if( rcPos.CenterPoint().y > iPosOneY )
				{
					if( m_pOneRefImage != NULL )
					{
						ImageRelease(pImgMemOperLog, m_pOneRefImage);
						m_pOneRefImage = NULL;
					}
					if(!m_bClearImage)
					{
						m_pOneRefImage = pParam->pCurFrame;
						if( m_pOneRefImage != NULL)
						{
							ImageAddRef(pImgMemOperLog, m_pOneRefImage);
							m_rgRect[0] = LastInfo().m_rcFG;
							m_iOnePosLightScene = iLightScene;
							m_dwBeginCaptureRefTime = pParam->dwImageTime;
						}
					}

				}
				//第二个位置(不是逆行时,用最后一张图片来更新)
				if( rcPos.CenterPoint().y > iPosTwoY  )
				{
					if (m_pTwoRefImage)
					{
						ImageRelease(pImgMemOperLog, m_pTwoRefImage);
						m_pTwoRefImage = NULL;
					}
					if(!m_bClearImage)
					{
						m_pTwoRefImage = pParam->pCurFrame;
						if( m_pTwoRefImage != NULL )
						{
							ImageAddRef(pImgMemOperLog, m_pTwoRefImage);
							m_rgRect[1] = LastInfo().m_rcFG;
							m_iTwoPosLightScene = iLightScene;
							m_dwBestCaptureRefTime = pParam->dwImageTime;
						}

					}
				}

				//第三个位置 (总是更新)
				if (m_pThreeRefImage)
				{
					ImageRelease(pImgMemOperLog, m_pThreeRefImage);
					m_pThreeRefImage = NULL;
				}
				if(!m_bClearImage)
				{
					m_pThreeRefImage = pParam->pCurFrame;
					if ( m_pThreeRefImage )
					{
						ImageAddRef(pImgMemOperLog, m_pThreeRefImage);
						m_iThreePosLightScene = iLightScene;
						m_dwLastCaptureRefTime = pParam->dwImageTime;
						m_iThreePosLightScene = iLightScene;
						m_rgRect[2] = LastInfo().m_rcFG;
					}
				}

				//是否过线
				if( m_iPassStopLightScene == -1 && rcPos.CenterPoint().y < iPosStop )
				{
					m_iPassStopLightScene = iLightScene;
				}
				if( m_iPassLeftStopLightScene == -1 && rcPos.CenterPoint().y < iPosLeftStop )
				{
					m_iPassLeftStopLightScene = iLightScene;
				}
			}
#endif // #ifdef VERSION_IR
		}		
		else
		{
		// zhaopy
			bool fUpdateOnePos = false;
			bool fUpdateTwoPos = false;
			bool fUpdateThreePos = false;

			if( GetCurrentParam()->g_MovingDirection == MD_TOP2BOTTOM )
			{
				fUpdateOnePos =( rcPos.CenterPoint().y < iPosOneY || fReverseRun );
				fUpdateTwoPos = ( (!fReverseRun && rcPos.CenterPoint().y < iPosTwoY)
					|| (fReverseRun && rcPos.CenterPoint().y > iPosOneY) );
				fUpdateThreePos = ( (!fReverseRun && rcPos.CenterPoint().y < iPosThreeY)
					|| (fReverseRun && rcPos.CenterPoint().y > iPosTwoY) );
			}
			else if( GetCurrentParam()->g_MovingDirection == MD_BOTTOM2TOP )
			{
				fUpdateOnePos =( rcPos.CenterPoint().y > iPosOneY || fReverseRun );
				fUpdateTwoPos = ( (!fReverseRun && rcPos.CenterPoint().y > iPosTwoY)
					|| (fReverseRun && rcPos.CenterPoint().y < iPosOneY) );
				fUpdateThreePos = ( (!fReverseRun && rcPos.CenterPoint().y > iPosThreeY)
					|| (fReverseRun && rcPos.CenterPoint().y < iPosTwoY) );
			}

			//第一个位置
			if( fUpdateOnePos )
			{
				if (m_pOneRefImage)
				{
					ImageRelease(pImgMemOperLog, m_pOneRefImage);
					m_pOneRefImage = NULL;
				}
				m_pOneRefImage = pParam->pCurFrame;
				if( m_pOneRefImage != NULL )
				{
					ImageAddRef(pImgMemOperLog, m_pOneRefImage);
					m_rgRect[0] = LastInfo().m_rcFG; 
				}
			}
			//第二个位置
			if( fUpdateTwoPos )
			{
				if(m_pTwoRefImage)
				{
					ImageRelease(pImgMemOperLog, m_pTwoRefImage);
					m_pTwoRefImage = NULL;
				}
				m_pTwoRefImage = pParam->pCurFrame;
				if( m_pTwoRefImage != NULL )
				{
					ImageAddRef(pImgMemOperLog, m_pTwoRefImage);
					m_rgRect[1] = LastInfo().m_rcFG; 
				}
			}
			//第三个位置
			if(fUpdateThreePos)
			{
				if(m_pThreeRefImage)
				{
					ImageRelease(pImgMemOperLog, m_pThreeRefImage);
					m_pThreeRefImage = NULL;
				}
				m_pThreeRefImage = pParam->pCurFrame;
				if( m_pThreeRefImage != NULL )
				{
					ImageAddRef(pImgMemOperLog, m_pThreeRefImage);
					m_rgRect[2] = LastInfo().m_rcFG; 
				}
			}
		}
		if( GetCurrentParam()->m_fUsedTrafficLight != 1 )
		{
			int dx = (LastInfo().m_rcFG.right + LastInfo().m_rcFG.left - imgCurFrame.iWidth) / 2;
			int dy = LastInfo().m_rcFG.bottom - (imgCurFrame.iHeight / 2);
			int iDistance = dx * dx + dy * dy;
			if (0 == CTrackInfo::m_iBestLightMode)
			{
				if (iDistance < m_iBestDistance)
				{
					m_iBestDistance = iDistance;
					if(m_pBestReftImage)
					{
						ImageRelease(pImgMemOperLog, m_pBestReftImage);
						m_pBestReftImage = NULL;
					}
					if(!m_bClearImage)
					{
						m_pBestReftImage = pParam->pCurFrame;
						ImageAddRef(pImgMemOperLog, m_pBestReftImage);
						m_dwBestSnapShotRefTime = pParam->dwImageTime;
					}
					m_rcBestCarPos = LastInfo().m_rcFG;
				}
			}
			else
			{
				if (iDistance < m_iBestDistance)
				{
					m_iBestDistance = iDistance;
				}

				if (m_fCarArrived
					&& (int)(pParam->dwImageTime - m_nCarArriveRealTime) < CTrackInfo::m_iBestLightWaitTime)
				{
					if(m_pBestReftImage)
					{
						ImageRelease(pImgMemOperLog, m_pBestReftImage);
						m_pBestReftImage = NULL;
					}
					if(!m_bClearImage)
					{
						m_pBestReftImage = pParam->pCurFrame;
						ImageAddRef(pImgMemOperLog, m_pBestReftImage);
						m_dwBestSnapShotRefTime = pParam->dwImageTime;
					}
					m_rcBestCarPos = LastInfo().m_rcFG;
				}
			}
		}
	}
	return S_OK;
}

bool CObjTrackInfo::IsTerminateTrack(HiVideo::CRect& rcVideoDet, LIGHT_TYPE lightType)
{
    bool fTerminate = false;
    //if (IsPassLine())
    //{
    //    fTerminate =  true;
    //}

    //如果obj已经转成了一个模板的跟踪
    if(m_objTrackState == OBJ_TRACK_TEM)
    {
        //查看是否位于视频检测区域外若是则删除
        if (!rcVideoDet.Contains(LastInfo().m_rcFG))
        {
            fTerminate = true;
        }
        //CRect roi = LastInfo().m_rcFG;
        //if (IsOutSideRoad(roi.CenterPoint().x, roi.CenterPoint().y))
        //{
        //    return true;
        //}
        //如果达到一定的帧数，
        if (IsObjStop() )
        {
            CRect roi = LastInfo().m_rcFG;
            //if(m_iEPNoPlateScore < 300)m_fEPOutput = false;
            fTerminate = true;
        }
        //如果到达屏幕的左右边缘
        CRect r = LastInfo().m_rcFG;
        if(r.left < 5 || (r.right > (CTrackInfo::m_iWidth - 5)))
        {
            fTerminate =  true;
        }
    }
    if(fTerminate)
    {
        //如果使用了车尾检测模型
        //那么使用它来限制多检的情况
        //目前只能用白天，黑夜没有进行车尾模型训练
        if (lightType == DAY && CTrackInfo::m_fEPUseCarTailModel == 1)
        {
            if(m_iEPNoPlateScore < CTrackInfo::m_iEPNoPlateScoreThreld)m_fEPOutput = false;
        }                 
    }
    return fTerminate;
}

BOOL CObjTrackInfo::IsObjStop()
{
    bool fStop = true;
    if(m_iObjInfoCount < 6)return false;
    if(m_objTrackState != OBJ_TRACK_TEM)return false;
    // 计算3帧的平均中心Y 坐标
    int nMeanY(0);
    int rgnY[4];
    for (int i = 0; i < 4; i++)
    {
        rgnY[i] = (m_rgObjInfo[m_iObjInfoCount - 4 + i].m_rcFG.top
            + m_rgObjInfo[m_iObjInfoCount - 4 + i].m_rcFG.bottom) / 2;
        nMeanY += rgnY[i];
    }
    nMeanY /= 4;
    // 进行停止判断
    for (int i = 0; i < 4; i++)
    {
        if (abs(rgnY[i] - nMeanY) > 2)
        {
            // 偏差超过阈值,则车辆移动
            m_iObjStopFrame =0;
            fStop = false;
            break;
        }
    }

    if(LastInfo().m_rcFG.CenterPoint().y ==  m_rgObjInfo[m_iObjInfoCount - 2].m_rcFG.CenterPoint().y
        && LastInfo().m_rcFG.CenterPoint().x == m_rgObjInfo[m_iObjInfoCount - 2].m_rcFG.CenterPoint().x)
    {
        m_iObjStopFrame++;
    }

    int iFrameCount = 12;
    if (m_iObjStopFrame > iFrameCount)
    {
        fStop = true;
    }
    else
    {
        fStop = false;
    }

    return fStop;
}

bool CObjTrackInfo::IsPassLine()
{
    int iForwardPosY = CTrackInfo::m_iRealForward * CTrackInfo::m_iHeight / 100;
    CRect r = LastInfo().m_rcFG;
    if(r.bottom <= iForwardPosY)
    {
        //HV_Trace("\nid:[%d] is over", m_Feature.iObjId);
        return true; 
    }
    return false;
}

HRESULT CObjTrackInfo::Clear(PROCESS_ONE_FRAME_RESPOND* pProcessRespond)
{
    //释放模板
    if (m_pObj != NULL)
    {
        delete m_pObj;
        m_pObj = NULL;
    }
	if (m_objTrackState == OBJ_TRACK_INIT)
	{
		return S_OK;
	}
	m_objTrackState = OBJ_TRACK_INIT;
	m_iVideoID = -1;
	for (int i = 0; i < m_iObjInfoCount; i++)
	{
		m_rgObjInfo[i].Clear();
	}
	m_fTrack = FALSE;
	m_cPlateTrackID = 0;
	m_fCarArrived = false;
	m_fVoted = false;
	m_iObjInfoCount = 0;
	m_iMissingCount = 0;
	m_nVotedCarType = CT_UNKNOWN;
	m_nCarArrivedCount = 0;
	m_bClearImage = false;

	// 内存操作日志
	IMG_MEM_OPER_LOG* pImgMemOperLog = &pProcessRespond->cImgMemOperLog;

	if (m_pFirstRefImage)
	{
		ImageRelease(pImgMemOperLog, m_pFirstRefImage);
		m_pFirstRefImage = NULL;	
	}
	if (m_pBestReftImage)
	{
		ImageRelease(pImgMemOperLog, m_pBestReftImage);
		m_pBestReftImage = NULL;	
	}
	if (m_pLastRefImage)
	{
		ImageRelease(pImgMemOperLog, m_pLastRefImage);
		m_pLastRefImage = NULL;	
	}

	if (m_pOneRefImage)
	{
		ImageRelease(pImgMemOperLog, m_pOneRefImage);
		m_pOneRefImage = NULL;	
	}
	if (m_pTwoRefImage)
	{
		ImageRelease(pImgMemOperLog, m_pTwoRefImage);
		m_pTwoRefImage = NULL;	
	}
	if (m_pThreeRefImage)
	{
		ImageRelease(pImgMemOperLog, m_pThreeRefImage);
		m_pThreeRefImage = NULL;	
	}

	m_iBestDistance = 0;

	m_iRoad = -1;

	m_iOnePosLightScene = -1;
	m_iTwoPosLightScene = -1;
	m_iThreePosLightScene = -1;
	m_iPassStopLightScene = -1;
	m_iPassLeftStopLightScene = -1;

	// TODO
	m_fIsMan = false;				// 行人标志缺省false;	
	m_iObjFrameCount = 0;

	m_fIsTrigger = false;
	m_dwTriggerIndex = 0;
	m_iCapCount = 0;
	m_dwBestSnapShotRefTime = 0;
	m_dwLastSnapShotRefTime = 0;
	m_dwBeginCaptureRefTime = 0;
	m_dwBestCaptureRefTime = 0;
	m_dwLastCaptureRefTime = 0;

	//zhaopy
	m_fHasTrigger = false;
	m_dwTriggerTimeMs = 0;
	m_dwTriggerImageTime = 0;
	m_fEPOutput = true;
    m_fNewTem = false;
    m_iObjStopFrame = 0;
    m_iTemYBegin = 0;
	m_iEPDetSkipFrames = 0;
    m_iEPNoPlateScore = 0;

	return S_OK;
}

HRESULT CObjTrackInfo::ClearImage(PROCESS_ONE_FRAME_RESPOND* pProcessRespond)
{
	if(m_bClearImage)
		return S_OK;

	m_bClearImage = true;
	// 内存操作日志
	IMG_MEM_OPER_LOG* pImgMemOperLog = &pProcessRespond->cImgMemOperLog;

	if (m_pFirstRefImage)
	{
		ImageRelease(pImgMemOperLog, m_pFirstRefImage);
		m_pFirstRefImage = NULL;	
	}
	if (m_pBestReftImage)
	{
		ImageRelease(pImgMemOperLog, m_pBestReftImage);
		m_pBestReftImage = NULL;	
	}
	if (m_pLastRefImage)
	{
		ImageRelease(pImgMemOperLog, m_pLastRefImage);
		m_pLastRefImage = NULL;	
	}
	if (m_pOneRefImage)
	{
		ImageRelease(pImgMemOperLog, m_pOneRefImage);
		m_pOneRefImage = NULL;	
	}
	if (m_pTwoRefImage)
	{
		ImageRelease(pImgMemOperLog, m_pTwoRefImage);
		m_pTwoRefImage = NULL;	
	}
	if (m_pThreeRefImage)
	{
		ImageRelease(pImgMemOperLog, m_pThreeRefImage);
		m_pThreeRefImage = NULL;	
	}
	return S_OK;
}

bool CObjTrackInfo::IsClearImage(void)
{
	return   !m_pFirstRefImage
		   &&!m_pBestReftImage
		   &&!m_pLastRefImage
		   &&!m_pOneRefImage
	       &&!m_pTwoRefImage
		   &&!m_pThreeRefImage;
}

CObjInfo & CObjTrackInfo::LastInfo()
{
	return m_rgObjInfo[m_iObjInfoCount - 1];
}

HRESULT CObjTrackInfo::Assign(CObjTrackInfo &src, PROCESS_ONE_FRAME_RESPOND* pProcessRespond)
{
	Clear(pProcessRespond);

	// 内存操作日志
	IMG_MEM_OPER_LOG* pImgMemOperLog = &pProcessRespond->cImgMemOperLog;

	m_fIsMan = src.m_fIsMan;		// 行人标志
	m_iObjFrameCount = src.m_iObjFrameCount;

	m_iVideoID = src.m_iVideoID;
	m_objTrackState = src.m_objTrackState;
	m_fTrack = src.m_fTrack;
	m_cPlateTrackID = src.m_cPlateTrackID;
	memcpy(m_rgiPlateTrackID, src.m_rgiPlateTrackID, sizeof(m_rgiPlateTrackID));
	m_fCarArrived = src.m_fCarArrived;
	m_nCarArriveTime = src.m_nCarArriveTime;
	m_nCarArriveRealTime = src.m_nCarArriveRealTime;
	m_fVoted = src.m_fVoted;
	m_iObjInfoCount = src.m_iObjInfoCount;
	m_iMissingCount = src.m_iMissingCount;
	m_iBestDistance = src.m_iBestDistance;
	m_rcCarArrivePos = src.m_rcCarArrivePos;
	m_nCarArrivedCount = src.m_nCarArrivedCount;
	m_rcFirstCarPos = src.m_rcFirstCarPos;
	m_rcBestCarPos = src.m_rcBestCarPos;
	m_rcLastCarPos = src.m_rcLastCarPos;
	m_iRoad = src.m_iRoad;
	m_stCarInfo = src.m_stCarInfo;
	m_bClearImage = src.m_bClearImage;
	
	m_fEPOutput = src.m_fEPOutput;
    m_pObj = src.m_pObj;
    src.m_pObj = NULL;//
    m_fNewTem = src.m_fNewTem;
    m_iObjStopFrame = src.m_iObjStopFrame;
    m_iTemYBegin = src.m_iTemYBegin;
	m_iEPDetSkipFrames = src.m_iEPDetSkipFrames;
    m_iEPNoPlateScore = src.m_iEPNoPlateScore;

	for (int i = 0; i < m_iObjInfoCount; i++)
	{
		m_rgObjInfo[i] = src.m_rgObjInfo[i];
	}

	m_pFirstRefImage = src.m_pFirstRefImage;
	if (m_pFirstRefImage)
	{
		ImageAddRef(pImgMemOperLog, m_pFirstRefImage);
	}
	m_pBestReftImage = src.m_pBestReftImage;
	if (m_pBestReftImage)
	{
		ImageAddRef(pImgMemOperLog, m_pBestReftImage);
	}
	m_pLastRefImage = src.m_pLastRefImage;
	if (m_pLastRefImage)
	{
		ImageAddRef(pImgMemOperLog, m_pLastRefImage);
	}

	m_pOneRefImage = src.m_pOneRefImage;
	if (m_pOneRefImage)
	{
		ImageAddRef(pImgMemOperLog, m_pOneRefImage);
	}
	m_pTwoRefImage = src.m_pTwoRefImage;
	if (m_pTwoRefImage)
	{
		ImageAddRef(pImgMemOperLog, m_pTwoRefImage);
	}
	m_pThreeRefImage = src.m_pThreeRefImage;
	if (m_pThreeRefImage)
	{
		ImageAddRef(pImgMemOperLog, m_pThreeRefImage);
	}

	m_iOnePosLightScene = src.m_iOnePosLightScene;
	m_iTwoPosLightScene = src.m_iTwoPosLightScene;
	m_iThreePosLightScene = src.m_iThreePosLightScene;
	m_iPassStopLightScene = src.m_iPassStopLightScene;
	m_iPassLeftStopLightScene = src.m_iPassLeftStopLightScene;
	m_rgRect[0] = src.m_rgRect[0];
	m_rgRect[1] = src.m_rgRect[1];
	m_rgRect[2] = src.m_rgRect[2];

	m_fIsTrigger = src.m_fIsTrigger;
	m_dwTriggerIndex = src.m_dwTriggerIndex;
	m_iCapCount = src.m_iCapCount;

	m_dwBestSnapShotRefTime = src.m_dwBestSnapShotRefTime;
	m_dwLastSnapShotRefTime = src.m_dwLastSnapShotRefTime;
	m_dwBeginCaptureRefTime = src.m_dwBeginCaptureRefTime;
	m_dwBestCaptureRefTime  = src.m_dwBestCaptureRefTime;
	m_dwLastCaptureRefTime  = src.m_dwLastCaptureRefTime;

	//zhaopy
	m_fHasTrigger = src.m_fHasTrigger;
	m_dwTriggerTimeMs = src.m_dwTriggerTimeMs;
	m_dwTriggerImageTime = src.m_dwTriggerImageTime;

	return S_OK;
}

// 无牌车投票:目前只对车型进行投票
HRESULT CObjTrackInfo::Vote(CAR_TYPE &nVotedCarType)
{
	// 对车型进行投票
	unsigned int rgnCarTypeScore[CT_COUNT]={0};				// 车型
	for (int i = 0; i < m_iObjInfoCount; i++)
	{
		rgnCarTypeScore[m_rgObjInfo[i].m_nCarType]++;	// 车型统计
	}
	nVotedCarType = CAR_TYPE(CT_UNKNOWN + 1);
	for (int i = nVotedCarType + 1; i < CT_COUNT; i++)
	{
		if (rgnCarTypeScore[nVotedCarType] < rgnCarTypeScore[i])
		{
			nVotedCarType = (CAR_TYPE)i;
		}
	}
	if (rgnCarTypeScore[nVotedCarType] == 0)
	{
		nVotedCarType = CT_UNKNOWN;
	}
	if (nVotedCarType == CT_WALKMAN && !m_fIsMan)
	{
		nVotedCarType = CT_BIKE;
	}

	// 设置默认的车型
	//if ((nVotedCarType == CT_UNKNOWN) || (nVotedCarType == CT_MID))
	//{
	//	nVotedCarType = CT_SMALL;
	//}

	// 车辆宽、高的统计计算
	int nTempCar(0);
	float fltTempW(0.0f);
	float fltTempH(0.0f);

	for (int i = 0; i < m_iObjInfoCount; i++)
	{
		if ((m_rgObjInfo[i].m_rcFG != CRect(0, 0, 0, 0)) && (m_rgObjInfo[i].m_nCarW  > 10))
		{
			nTempCar++;
			fltTempW += m_rgObjInfo[i].m_nCarW;
			fltTempH += m_rgObjInfo[i].m_nCarH;
		}

	}
	if (nTempCar > 0)
	{
		m_stCarInfo.fltCarW = fltTempW / nTempCar;
		m_stCarInfo.fltCarH = fltTempH / nTempCar;
	}

	return S_OK;
}

bool CObjTrackInfo::MatchPlateTrackID( int iPlateTrackID )
{
	for (int i = 0; i < m_cPlateTrackID; i++)
	{
		if (m_rgiPlateTrackID[i] == iPlateTrackID) return true;
	}
	return false;
}

HRESULT CObjTrackInfo::AddPlateTrackID( int iPlateTrackID )
{
	if (m_cPlateTrackID >= MAX_PLATETRACK_COUNT) return S_FALSE;
	m_rgiPlateTrackID[m_cPlateTrackID++] = iPlateTrackID;
	return S_OK;
}

//旧的做法，该方法暂时没有使用
CRect CObjTrackInfo::PredictPosition(UINT nNowTime, BOOL fEnableDetReverseRun)
{
    if (m_dwPredictTime == nNowTime && m_rcPredict != CRect(0, 0, 0, 0))
    {
        return m_rcPredict;
    }
    const float fltTrackInflate1 = 0.25f;
    const float fltTrackInflate2 = 0.0f;
    const float fltTrackInflate3 = 0.0f;
    const float fltTrackOffset1 = 0.75f;
    const float fltTrackOffset2 = 0.0f;

    CRect rect(-1, -1, -1, -1);
    if (m_iObjInfoCount > 0)
    {
        rect = LastInfo().m_rcFG;
        int iInflateHeight = rect.Height();
        iInflateHeight = MAX(iInflateHeight, 22);
        //int iWidth  = rect.right  - rect.left;
        //int iHeight = rect.bottom - rect.top;
        
        if (m_iObjInfoCount == 1)
        {
            if (fEnableDetReverseRun)
            {
                //switch (m_pParam->g_MovingDirection)
                //{
                //case MD_TOP2BOTTOM:
                //case MD_BOTTOM2TOP:
                rect.InflateRect(
                    (int)(fltTrackInflate3 * iInflateHeight),
                    (int)((fltTrackInflate3 + fltTrackOffset2) * iInflateHeight)
                    );
                //rect.top -= (iHeight >> 1);
                //if(rect.top < 0)rect.top = 0; 

                //    break;
                //case MD_LEFT2RIGHT:
                //case MD_RIGHT2LEFT:
                //    rect.InflateRect(
                //        (int)((fltTrackInflate3 + fltTrackOffset2) * iInflateHeight),
                //        (int)(fltTrackInflate3 * iInflateHeight)
                //        );
                //    break;
                //default:
                //    rect.InflateRect(
                //        (int)((fltTrackInflate3 + fltTrackOffset2) * iInflateHeight),
                //        (int)((fltTrackInflate3 + fltTrackOffset2) * iInflateHeight)
                //        );
                //    break;
                // }
            }else{
                rect.OffsetRect(0, -(int)(fltTrackOffset1 * iInflateHeight));
                rect.InflateRect(
                    (int)(fltTrackInflate3 * iInflateHeight),
                    (int)((fltTrackInflate1 + fltTrackOffset1) * iInflateHeight)
                    );
            }
        }else {
            CObjInfo cInfo1 = m_rgObjInfo[m_iObjInfoCount - 1];
            CObjInfo cInfo2 = m_rgObjInfo[m_iObjInfoCount - 2];
            
            float fltInterval = (float)(nNowTime - cInfo1.m_dwFrameTime);
            float fltInterval1 = (float)(cInfo1.m_dwFrameTime - cInfo2.m_dwFrameTime);
            float fltDistanceX1 = (float)((cInfo1.m_rcFG.left
                                           + cInfo1.m_rcFG.right
                                           - cInfo2.m_rcFG.left
                                           - cInfo2.m_rcFG.right) >> 1);
            float fltDistanceY1 = (float)((cInfo1.m_rcFG.top
                                           + cInfo1.m_rcFG.bottom
                                           - cInfo2.m_rcFG.top
                                           - cInfo2.m_rcFG.bottom) >> 1);
            int iOffsetX, iOffsetY;
            fltInterval1 = MAX(fltInterval1, 1.0f);

            iOffsetX = (int)(fltDistanceX1 * (MAX(0.5f, fltInterval / fltInterval1)));
            iOffsetY = (int)(fltDistanceY1 * (MAX(0.5f, fltInterval / fltInterval1)));

            rect.OffsetRect(iOffsetX, iOffsetY);
            rect.InflateRect(
                (int)(fltTrackInflate3 * iInflateHeight),
                (int)((fltTrackInflate2 + fltTrackOffset2) * iInflateHeight)
                );
            m_rcPredict = rect;
            m_dwPredictTime = nNowTime;
            //return rect;
        }
    }
    return rect;
}

//该法太过于简单估测有问题，暂时不使用
CRect CObjTrackInfo::PredictPositionSimple(UINT nNowTime, BOOL fEnableDetReverseRun, HvSize imgSize)
{
    //only use last obj info
    CRect rect(-1, -1, -1, -1);
    if (m_iObjInfoCount > 0)
    {
        CRect roi = LastInfo().m_rcFG;
        //扩大
        int iH = roi.bottom - roi.top;
        int iW = roi.right - roi.left;
        //HV_Trace("\niH:[%d], iW:[%d]", iH, iW);
        roi.left -= (iW>>3);
        roi.right += (iW>>3);
        roi.top -= (iH>>1);
        roi.bottom += (iH>>1);

        if(roi.left < 0)roi.left = 0;
        if(roi.right >= imgSize.width)roi.right = imgSize.width - 1;
        if(roi.top < 0)roi.top = 0;
        if(roi.bottom >= imgSize.height)roi.bottom = imgSize.height - 1;
        rect = roi;
        //HV_Trace("\nroi.Width:[%d], roi.Height:[%d]", roi.Width(), roi.Height());
        m_rcPredict = roi;
    }
    return rect;
}

HRESULT CObjTrackInfo::UpdateObjTemple(HV_COMPONENT_IMAGE &imgCurImage, CRect &rcObj)
{
    if ((rcObj.top <0 ) || (rcObj.left < 0)
        || (rcObj.bottom <= rcObj.top) || (rcObj.right <= rcObj.left))
    {
        return E_INVALIDARG;
    }

     if (m_pObj == NULL)
     {
         //建立模版对象
         m_pObj = new CObjMatch;
         if (m_pObj == NULL)
         {
             return E_OUTOFMEMORY;
         }
     }

    m_pObj->UpdateTemple(&imgCurImage, rcObj);
    return S_OK;
}

//由MOVING的跟踪状态转到模板的跟踪状态
HRESULT CObjTrackInfo::NewTemEx2(int iVideoID, 
			                      CObjInfo &objInfo, 
			                      PROCESS_ONE_FRAME_PARAM* pParam, 
			                  	  HV_COMPONENT_IMAGE &imgCurImage, 
			                  	  PROCESS_ONE_FRAME_RESPOND* pProcessRespond,
			                      int iLightScene, 
			                      int iCurLight,
			                      CPoint offset,
			                      HvSize EPSize, 
                                bool fIsMan)
{
    if (m_objTrackState != OBJ_TRACK_TEM)
    {
        return E_UNEXPECTED;
    }

    m_fIsMan = fIsMan;				// 行人标志
    //m_iObjFrameCount++;

    //CRect newRect = objInfo.m_rcFG;
    CRect newRect = LastInfo().m_rcFG;
    //缩放矩形框到某个指定的尺寸，用这个尺寸去进行模宓母?
    //先简单的从中心取出一块可以作为跟踪的区域
    //目前是采取大约车道宽度1/6的水平作为跟踪模板的大小
    //假如以后可以从跟踪总控拿到5字车宽的估计宽度，那么模板的跟踪区域就采用5字车宽的两倍来进行跟踪
    int iSH = newRect.bottom - newRect.top;
    CPoint center = newRect.CenterPoint();
    int iBottom = newRect.bottom - (iSH>>1);
    int iTop = iBottom - EPSize.height;
    int iLeft = center.x - (EPSize.width>>1);
    int iRight = center.x + (EPSize.width>>1);
    CRect rShrink(iLeft, iTop, iRight, iBottom);
    rShrink.IntersectRect(&rShrink, &newRect);
    m_iTemYBegin = rShrink.bottom;
    //objInfo.m_rcFG = newRect;
    objInfo.m_rcFG = rShrink;
    //HV_Trace("\nnewRect width:[%d], newRect height:[%d]", newRect.Width(), newRect.Height());
    //HV_Trace("\nnewRect top:[%d], newRect bottom:[%d]", newRect.top, newRect.bottom);
    //HV_Trace("\n rShrink top:[%d], rShrink bottom:[%d]", rShrink.top, rShrink.bottom);
    
    m_rgObjInfo[m_iObjInfoCount-1] = objInfo;
    //m_iObjInfoCount++;
  
    HRESULT hr = UpdateObjTemple(imgCurImage, rShrink);
    if (SUCCEEDED(hr))
    {
        //HV_Trace("\n模板建立成功");
        m_fNewTem = true;
        m_fFirstPredict = true;
    }
    
    return S_OK;
}

BOOL CObjTrackInfo::IsTransToTemState(HV_COMPONENT_IMAGE& imgSrc, 
			                  	  		int iLightScene, 
			                            int iCurLight, 
			                            CPoint offset, 
			                            int iLineY)
{
    BOOL fOutput = false;
    if (m_objTrackState != OBJ_TRACK_MOVING)
    {
        return false;
    }

    CRect rLastRect = LastInfo().m_rcFG;
    //转移条件
    if (rLastRect.top < iLineY && rLastRect.bottom < imgSrc.iHeight*9/10)
    {
        fOutput = true;
    }
    
    //车道外物体不转移到模板跟踪状态
    if (IsOutSideRoad(rLastRect.CenterPoint().x, rLastRect.CenterPoint().y))
    {
        fOutput = false;
    }
    //符合一定的车道宽度比例和宽高比例才进行转移
    if (fOutput)
    {
        int iLineW;
        MatchRoad(rLastRect.left, rLastRect.bottom, &iLineW);
        if(iLineW > 0)
        {
            if((rLastRect.Width()*100/iLineW) < 35)fOutput = false;
            if (rLastRect.Height()*100/rLastRect.Width() < 35)fOutput = false;            
        }
    }
    return fOutput;
}

HRESULT CObjTrackInfo::ProcessEx(PROCESS_ONE_FRAME_PARAM* pParam, 
			                  	  HV_COMPONENT_IMAGE& imgSrc, 
								  HiVideo::CRect rcVideoDet,
			                      CRect& rSearch,
			                      BOOL fEnableDetReverseRun,
			                      int iLightScene, 
			                      LIGHT_TYPE lightType,
			                      PROCESS_ONE_FRAME_RESPOND* pProcessRespond
			                      )
{
    UINT nNowTime   = pParam->dwImageTime;
    //CRect rPredict  = PredictPosition(nNowTime, fEnableDetReverseRun);
    //上一个roi
    CRect rOld      = LastInfo().m_rcFG;
    if(IsTerminateTrack(rcVideoDet, lightType))return S_OK;
    CRect rNew(-1, -1, -1, -1);
    HV_COMPONENT_IMAGE imgCrop;
    HvSize S = hvSize(imgSrc.iWidth, imgSrc.iHeight);
    CRect rPredict(0,0,0,0);
    if (rSearch.Height() == 0 || rSearch.Width() == 0)
    {
        rPredict  = PredictPositionSimple(nNowTime, fEnableDetReverseRun, S);
        //rPredict = rOld;
    } else 
    {
        rPredict = rSearch;
    }
    CRect rSrc(0,0, imgSrc.iWidth, imgSrc.iHeight);
    rPredict.IntersectRect(rPredict, rSrc);
    RTN_HR_IF_FAILED(CropImage(imgSrc, rPredict, &imgCrop));
    m_pObj->GetBestMatch(&imgCrop, &rNew, false, 0.7f, false ,8, 20, false);
    if (rNew.top == -1)
    {
        //匹配失败的情况,认为丢失
        m_iMissingCount++;
    } else 
    {
        //匹配成功，调整匹配位置到全图位置
        rNew.left += rPredict.left;
        rNew.right += rPredict.left;
        rNew.top += rPredict.top;
        rNew.bottom += rPredict.top;
        rNew.IntersectRect(rNew, rPredict);
        //将新的匹配矩形框组合成new objinfo 放到新的队列去
        CRect rLast = LastInfo().m_rcFG;
        CObjInfo objInfo;
        objInfo.m_dwFrameTime = nNowTime;
        objInfo.m_nFrameNo = pParam->dwFrameNo;
        objInfo.m_rcFG = rNew;
        m_rgObjInfo[m_iObjInfoCount] = objInfo;
        
        if (m_iObjInfoCount < MAX_OBJINFO_COUNT - 1)
        {
            m_iObjInfoCount++;
        }
        else
        {
            m_rgObjInfo[m_iObjInfoCount - 1] = m_rgObjInfo[m_iObjInfoCount];
        }
        //更新模板
        m_pObj->UpdateTemple(&imgSrc, rNew);
        m_fTrack = true;
        
		// 内存操作日志
		IMG_MEM_OPER_LOG* pImgMemOperLog = &pProcessRespond->cImgMemOperLog;

		CRect rcPos = LastInfo().m_rcFG;
		int iPosOneY = CTrackInfo::m_iCaptureOnePos * CTrackInfo::m_iHeight / 100;
		int iPosTwoY = CTrackInfo::m_iCaptureTwoPos * CTrackInfo::m_iHeight / 100;
		//int iPosThreeY = CTrackInfo::m_iCaptureThreePos * CTrackInfo::m_iHeight / 100;
		int iPosStop  = CTrackInfo::m_iStopLinePos * CTrackInfo::m_iHeight / 100;
		int iPosLeftStop  = CTrackInfo::m_iLeftStopLinePos * CTrackInfo::m_iHeight / 100;
		CRect rcFirstPos = rcPos;
		if( m_iObjInfoCount > 1 )
		{
			rcFirstPos = m_rgObjInfo[0].m_rcFG;
		}
		//判断是否是逆行
		int iReverseRunLevel = 2 * CTrackInfo::m_iHeight / 100;
		bool fReverseRun = false;
		if( GetCurrentParam()->g_MovingDirection == MD_TOP2BOTTOM )
		{
			fReverseRun = (rcFirstPos.CenterPoint().y - rcPos.CenterPoint().y) > iReverseRunLevel ?  true : false;
		}
		else if( GetCurrentParam()->g_MovingDirection == MD_BOTTOM2TOP )
		{
			fReverseRun = (rcPos.CenterPoint().y - rcFirstPos.CenterPoint().y) > iReverseRunLevel ?  true : false;
		}
		if( GetCurrentParam()->m_fUsedTrafficLight == 1 )
		{
			if (fReverseRun)
			{
				//TODO
			}
			else
			{
				//第一个位置, 第一个位置以底坐标来判断
				if( rcPos.CenterPoint().y > iPosOneY )
				{
					if( m_pOneRefImage != NULL )
					{
						ImageRelease(pImgMemOperLog, m_pOneRefImage);
						m_pOneRefImage = NULL;
					}
					if(!m_bClearImage)
					{
						m_pOneRefImage = pParam->pCurFrame;
						if( m_pOneRefImage != NULL)
						{
							ImageAddRef(pImgMemOperLog, m_pOneRefImage);
							m_rgRect[0] = LastInfo().m_rcFG;
							m_iOnePosLightScene = iLightScene;
							m_dwBeginCaptureRefTime = pParam->dwImageTime;
						}
					}

				}

				//第二个位置(不是逆行时,用最后一张图片来更新)
				if( rcPos.CenterPoint().y > iPosTwoY )
				{
					if (m_pTwoRefImage)
					{
						ImageRelease(pImgMemOperLog, m_pTwoRefImage);
						m_pTwoRefImage = NULL;
					}
					if(!m_bClearImage)
					{
						m_pTwoRefImage = pParam->pCurFrame;
						if( m_pTwoRefImage != NULL )
						{
							ImageAddRef(pImgMemOperLog, m_pTwoRefImage);
							m_rgRect[1] = LastInfo().m_rcFG;
							m_iTwoPosLightScene = iLightScene;
							m_dwBestCaptureRefTime = pParam->dwImageTime;
						}

					}
				}

				//第三个位置 (总是更新)
				if (m_pThreeRefImage)
				{
					ImageRelease(pImgMemOperLog, m_pThreeRefImage);
					m_pThreeRefImage = NULL;
				}
				if(!m_bClearImage)
				{
					m_pThreeRefImage = pParam->pCurFrame;
					if ( m_pThreeRefImage )
					{
						ImageAddRef(pImgMemOperLog, m_pThreeRefImage);
						m_iThreePosLightScene = iLightScene;
						m_dwLastCaptureRefTime = pParam->dwImageTime;
						m_iThreePosLightScene = iLightScene;
						m_rgRect[2] = LastInfo().m_rcFG;
					}
				}

				//是否过线
				if( m_iPassStopLightScene == -1 && rcPos.CenterPoint().y < iPosStop )
				{
					m_iPassStopLightScene = iLightScene;
				}
				if( m_iPassLeftStopLightScene == -1 && rcPos.CenterPoint().y < iPosLeftStop )
				{
					m_iPassLeftStopLightScene = iLightScene;
				}
			}
		}					
    }
    return S_OK;
}

HRESULT CObjTrackInfo::FlashLightImage(DWORD32* pdwTriggerTime, int* piMinTriggerTimeMS, DWORD32* pdwLastTriggerInfo,int iLightType)
{
	return S_FALSE;
}
