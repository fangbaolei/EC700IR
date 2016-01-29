#include "hvutils.h"
#include "TrackInfoHigh.h"
#include "trackerdef.h"
#include "TrackerImplHigh.h"
#include <math.h>

extern void ImageAddRef(IMG_MEM_OPER_LOG* pLog, IReferenceComponentImage* pRefImage);
extern void ImageRelease(IMG_MEM_OPER_LOG* pLog, IReferenceComponentImage* pRefImage);

using namespace HiVideo;

extern ITracker* g_pTrackerVideo;
extern char g_szDebugInfo[256];
/////////////////////////////////////////////////////////////////////////////////
// Implementation of CTrackInfo

//可配置参数
// 投票数
int CTrackInfo::m_nVoteFrameNum;
// 连续结果相同, Vote
int CTrackInfo::m_nMaxEqualFrameNumForVote;
// 是否输出投票满足条件
int CTrackInfo::m_fOutputVoteCondition;


//投票时，去除得分最低的车牌
int CTrackInfo::m_nRemoveLowConfForVote;

// 识别得分低于阈值(百分比)，用*表示
int CTrackInfo::m_nRecogAsteriskThreshold;

int CTrackInfo::m_iBestLightMode;
int CTrackInfo::m_iBestLightWaitTime;

//红绿灯相关
int CTrackInfo::m_iCaptureOnePos;
int CTrackInfo::m_iCaptureTwoPos;
int CTrackInfo::m_iCaptureThreePos;
int CTrackInfo::m_iStopLinePos;
int CTrackInfo::m_iLeftStopLinePos;
int CTrackInfo::m_iHeight;
int CTrackInfo::m_iWidth;
int CTrackInfo::m_iCheckType;
int CTrackInfo::m_iAutoScanLight;
int CTrackInfo::m_iSceneCheckMode;

int CTrackInfo::m_iMatchScanSpeed;
int CTrackInfo::m_iRealForward;

int CTrackInfo::m_iCarArrivedPos;
int CTrackInfo::m_iRedLightDelay;			//红灯判断违章延迟时间

int CTrackInfo::m_iFlashlightMode;			//抓拍补光灯模式
int CTrackInfo::m_iFlashlightThreshold;		//亮度阈值，当环境亮度小于此值时，抓拍补光灯才闪。

int CTrackInfo::m_iSpecificLeft;
int CTrackInfo::m_iRunTypeEnable;

//车道信息
int CTrackInfo::m_iRoadNumber;								//车道数量
ROAD_INFO CTrackInfo::m_roadInfo[CTrackInfo::s_iMaxRoadNumber];
int CTrackInfo::m_iRoadNumberBegin;								//车道编号起始方向
int CTrackInfo::m_iStartRoadNum;
int CTrackInfo::m_iOutputRoadTypeName = 0;

//事件检测参数
ACTIONDETECT_PARAM_TRACKER CTrackInfo::m_ActionDetectParam;

// 红绿灯情况下模板匹配位置
int CTrackInfo::m_nTrafficLightTempleYPos = 72;

// 厦门项目行人道编号
int CTrackInfo::m_iManRoadNum = -2;
bool CTrackInfo::m_fEnableRecgCarColor = false;
int CTrackInfo::m_nOutputPeccancyType = 0;

//2011-3-9增加红绿灯亮度阀值参数
int CTrackInfo::m_iTrafficLightThread = 3;
int CTrackInfo::m_iTrafficLightBrightValue = 3;
int CTrackInfo::m_iAutoScanThread = 8;

int CTrackInfo::m_fEPUseCarTailModel = 0;
int CTrackInfo::m_iEPNoPlateScoreThreld = 300;
CTrackInfo::CTrackInfo()
{
	m_nID = -1;
	m_State = tsInit;
	m_nMissingTime = m_nObservedFrames = -1;
	m_fltMaxConfidence = -1.0;
	m_fCarArrived = false;
	m_fCarLefted = false;
	m_fVoted = false;
	m_nResultEqualTimes = 0;
	m_pimgBestSnapShot = NULL;
	m_pimgLastSnapShot = NULL;
	m_pimgBeginCapture = NULL;
	m_pimgBestCapture = NULL;
	m_pimgLastCapture = NULL;
	m_dwBestSnapShotRefTime = 0;
	m_dwLastSnapShotRefTime = 0;
	m_dwBeginCaptureRefTime = 0;
	m_dwBestCaptureRefTime = 0;
	m_dwLastCaptureRefTime = 0;
	m_nStartFrameNo = 0;
	m_nEndFrameNo = 0;
	m_fReverseRun = false;
	m_fReverseRunVoted = false;
	m_iVoteSimilarityNum = 0;
	m_cPlateInfo = 1;
	m_nCarArriveTime = 0;
	m_nCarArriveRealTime = 0;
	m_dwCarArriveDopeTime = 0;
	m_dwLastProcessCarArrvieTime = 0;

	m_dwDopeFirstPos = 0;
	m_dwDopeEndPos = 0;
	m_fIsTrigger = false;
	m_dwTriggerIndex = 0;
	m_iCapCount = 0;
	m_iCapSpeed = 0;

	m_fCarIsStop = false;
	m_iCarStopTick = -1;
	m_iCarStopPassTime = 0;

	// 物体检测变量
	m_pObj = NULL;
	m_fCanOutput = false;
	m_fTempCloseEdge = false;		// 模版是否靠近边缘
	m_nCloseEdgeCount = 0;		// 模版靠近边缘帧计数器
	m_nMinYPos = 10000;			// 最小Y坐标
	m_nMaxYPos = 10000;			// 最大的Y坐标偏移
	m_nMaxXPos = 10000;			// 最大的X坐标偏移

	m_iOnePosLightScene = -1;
	m_iTwoPosLightScene = -1;
	m_iThreePosLightScene = -1;
	m_iPassStopLightScene = -1;
	m_iPassLeftStopLightScene = -1;

	m_dwTriggerTime = 0;
	m_iRoad = -1;
	m_fFirstTrigger = false;
	m_fSecondTrigger = false;
	m_fThirdTrigger = false;

	m_iFirstType = -1;

	m_dwFirstTime = 0;
	m_dwSecondTime = 0;
	m_dwThirdTime = 0;

	m_dwFirstTimeMs = 0;
	m_dwSecondTimeMs = 0;
	m_dwThirdTimeMs = 0;

	m_rcBestPos = CRect(0,0,0,0);
	m_rcLastPos = CRect(0,0,0,0);

	m_rectLastImage = CRect(0,0,0,0);

	m_rcPredict = CRect(0, 0, 0, 0);
	m_dwPredictTime = 0;
	for (int i = 0; i < int(PLATE_TYPE_COUNT); i++)
	{
		m_rgBestPlateInfo[i].pimgPlate = NULL;
	}

	m_stCarInfo.rcFG = CRect(0,0,0,0);
	m_stCarInfo.fltCarH = m_stCarInfo.fltCarW = 0.0f;

	for(int i = 0; i < 1000; i++ )
	{
		m_rgfltFrameSpeed[i] = 0;
	}
	m_iFrameSpeedCount = 0;

	m_bestPlatetoBinImage.pimgPlate = NULL;

	m_bPreVote = false;
	m_fHasTrigger = false;
	m_dwTriggerTimeMs = 0;
	m_dwTriggerImageTime = 0;

	m_iCarStopFrame = 0;
}

CTrackInfo::~CTrackInfo()
{
	//Clear();
}

void CTrackInfo::Clear(PROCESS_ONE_FRAME_RESPOND* pProcessRespond)
{
	IMG_MEM_OPER_LOG* pImgMemOperLog = &pProcessRespond->cImgMemOperLog;
	
	// 物体检测变量
	if (m_pObj != NULL)
	{
		delete m_pObj;
		m_pObj = NULL;
	}

	if (m_pimgBestSnapShot)
	{
		ImageRelease(pImgMemOperLog, m_pimgBestSnapShot);
		m_pimgBestSnapShot = NULL;
	}

	if (m_pimgLastSnapShot)
	{
		ImageRelease(pImgMemOperLog, m_pimgLastSnapShot);
		m_pimgLastSnapShot = NULL;
	}

	if (m_pimgBeginCapture)
	{
		ImageRelease(pImgMemOperLog, m_pimgBeginCapture);
		m_pimgBeginCapture = NULL;
	}

	if (m_pimgBestCapture)
	{
		ImageRelease(pImgMemOperLog, m_pimgBestCapture);
		m_pimgBestCapture = NULL;
	}

	if (m_pimgLastCapture)
	{
		ImageRelease(pImgMemOperLog, m_pimgLastCapture);
		m_pimgLastCapture = NULL;
	}

	m_dwBestSnapShotRefTime = 0;
	m_dwLastSnapShotRefTime = 0;
	m_dwBeginCaptureRefTime = 0;
	m_dwBestCaptureRefTime = 0;
	m_dwLastCaptureRefTime = 0;

	m_fHasTrigger = false;
	m_dwTriggerTimeMs = 0;
	m_dwTriggerImageTime = 0;

	m_iCarStopFrame = 0;

	for (int i=0; i<ARRSIZE(m_rgBestPlateInfo); i++)
	{
		m_rgBestPlateInfo[i].Clear();
	}

	m_bestPlatetoBinImage.Clear();

	//恢复初始状态
	m_iVideoID = -1;
	m_nID = -1;
	m_State = tsInit;
	m_nMissingTime = -1;
	m_nObservedFrames = -1;
	m_fltMaxConfidence = -1.0;
	m_fCarArrived = false;
	m_fCarLefted = false;
	m_fVoted = false;
	m_nResultEqualTimes = 0;
	m_nStartFrameNo = 0;
	m_nEndFrameNo = 0;

	m_dwCarArriveDopeTime = 0;
	m_dwLastProcessCarArrvieTime = 0;

	m_dwDopeFirstPos = 0;
	m_dwDopeEndPos = 0;
	m_fIsTrigger = false;
	m_dwTriggerIndex = 0;
	m_iCapCount = 0;
	m_iCapSpeed = 0;

	// 物体检测变量
	m_fCanOutput = false;
	m_nMinYPos = 10000;
	m_nMaxYPos = 10000;			// 最大的Y坐标偏移
	m_nMaxXPos = 10000;			// 最大的X坐标偏移

	m_cItgArea.SetRect(0,0,0,0,FALSE);
	m_fCarIsStop = false;
	m_iCarStopTick = -1;
	m_iCarStopPassTime = 0;

	m_iOnePosLightScene = -1;
	m_iTwoPosLightScene = -1;
	m_iThreePosLightScene = -1;
	m_iPassStopLightScene = -1;
	m_iPassLeftStopLightScene = -1;

	m_dwTriggerTime = 0;
	m_iRoad = -1;

	m_fFirstTrigger = false;
	m_fSecondTrigger = false;
	m_fThirdTrigger = false;

	m_iFirstType = -1;

	m_dwFirstTime = 0;
	m_dwSecondTime = 0;
	m_dwThirdTime = 0;

	m_dwFirstTimeMs = 0;
	m_dwSecondTimeMs = 0;
	m_dwThirdTimeMs = 0;

	m_rcBestPos = CRect(0,0,0,0);
	m_rcLastPos = CRect(0,0,0,0);

	m_rectLastImage = CRect(0,0,0,0);

	m_stCarInfo.rcFG = CRect(0,0,0,0);
	m_stCarInfo.fltCarH = m_stCarInfo.fltCarW = 0.0f;

	for(int i = 0; i < 1000; i++ )
	{
		m_rgfltFrameSpeed[i] = 0;
	}
	m_iFrameSpeedCount = 0;
	m_bPreVote = false;
}

CParamQuanNum CTrackInfo::TrackScore() const
{
	return m_fltMaxConfidence;
}

///////////////////////////////////////////////////////////////////////
// Class: CTrack
// Method: PrepareOutPutFinalResult
// Author: Song Jun
// Written:  2004/11/26
// Reviewed: N/A
// Reviewer: N/A
// Description: 准备输出最终结果, 如果可以输出返回true, 并修改bHasVoted标志
// Returns: 是否输出最终结果
/////////////////////////////////////////////////////////////////////
bool CTrackInfo::PrepareOutPutFinalResult(DWORD32 dwCurTick)
{
	bool fResult(false);

	//判断紧急停车
	if (m_pParam->m_iDetectCarStopEnable)
	{
		if (m_nObservedFrames > 0 && (m_iCarStopTick != -1)
			&& ((int)dwCurTick - m_iCarStopTick >= CTrackInfo::m_ActionDetectParam.iDetectCarStopTime * 1000))
		{
			m_fCarIsStop = true;
			m_iCarStopPassTime += (dwCurTick - m_iCarStopTick) / 1000;
			m_iCarStopTick = dwCurTick;
			fResult = true;
			goto EXIT;
		}
	}

	// zhaopy
	if( m_pParam->g_PlateRcogMode == PRM_CAP_FACE )
	{
		if( !m_fVoted && m_State == tsEnd )
		{
			m_iCarStopTick = dwCurTick;
			fResult = true;
		}
	}
	else
	{
		// 检查黄牌大车停止情况
		bool fYellowCarStopArrive = false;
		if (!m_fCarArrived && m_State == tsMoving && m_nObservedFrames > 5
				&& (m_nObservedFrames >= m_nVoteFrameNum ||
				m_nResultEqualTimes >= m_nMaxEqualFrameNumForVote))
		{
			int nYellowCnt = 0;
			for (int i=0; i<m_cPlateInfo; ++i)
			{
				if ( m_rgPlateInfo[i].color == PC_YELLOW)
				{
					++nYellowCnt;
				}
			}
			// 黄牌占总数60%
			if (nYellowCnt * 100 > m_cPlateInfo * 60)
			{
				CRect rcPlate = LastInfo().rcPos;
				// 在停止线往上20%停的可以给出
				if (rcPlate.bottom > (m_iCarArrivedPos - 20) * CTrackInfo::m_iHeight / 100)
				{
					CRect rcPlatePre = m_rgPlateInfo[m_cPlateInfo-4].rcPos;
					if (rcPlatePre.IntersectRect(&rcPlatePre, &rcPlate))
					{
						if (rcPlatePre.Area() * 100 > rcPlate.Area() * 97)  // 高度相交
						{
							fYellowCarStopArrive = true;
						}
					}
				}
			}
		}

		if (!m_fVoted &&
			(m_State == tsEnd ||
			((m_fCarArrived || fYellowCarStopArrive) &&
			m_State == tsMoving &&
			(m_nObservedFrames >= m_nVoteFrameNum ||
			m_nResultEqualTimes >= m_nMaxEqualFrameNumForVote))))
		{
			m_iCarStopTick = dwCurTick;
			fResult = true;
		}
	}

EXIT:
	if (fResult)
	{
		if (m_nResultEqualTimes >= m_nMaxEqualFrameNumForVote)
		{
			m_nVoteCondition = (int)MAX_EQUAL_FRAME;
		}
		if (m_nObservedFrames >= m_nVoteFrameNum)
		{
			m_nVoteCondition = (int)MAX_VOTE_FRAME;
		}
	}
	return fResult;
}

HRESULT CTrackInfo::UpdatePlateImage(
		HV_COMPONENT_IMAGE imgInputPlate,
		PROCESS_ONE_FRAME_PARAM* pParam,
		int iLastStatus,
		int iCurStatus,
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond
)
{
	IMG_MEM_OPER_LOG* pImgMemOperLog = &pProcessRespond->cImgMemOperLog;
	IReferenceComponentImage* pimgInputSnap = pParam->pCurFrame;
	IReferenceComponentImage* pimgCapture = NULL;

	//电警模式才输出三张抓拍图
	//黄冠榕 2011-08-12
	if(GetCurrentParam()->g_PlateRcogMode == PRM_ELECTRONIC_POLICE)
	{
		pimgCapture = pimgInputSnap;
	}

	bool fNewPlate = (m_nEndFrameNo != pParam->dwFrameNo);
	if (fNewPlate)
	{
		m_nEndFrameNo = pParam->dwFrameNo;
	}

	CRect rcPos = LastInfo().rcPos;
	int iPosOneY = m_iCaptureOnePos * m_iHeight / 100;
	int iPosTwoY = m_iCaptureTwoPos * m_iHeight / 100;
	int iPosThreeY =  m_iCaptureThreePos * m_iHeight / 100;

	//判断是否是逆行
	CRect rcFirstPos = rcPos;
	if( m_cPlateInfo > 1 )
	{
		rcFirstPos = m_rgPlateInfo[0].rcPos;
	}
	int iReverseRunLevel = 2 * m_iHeight / 100;

	bool fReverseRun = false;
	if( m_pParam->g_MovingDirection == MD_TOP2BOTTOM )
	{
		fReverseRun = ( rcFirstPos.CenterPoint().y - rcPos.CenterPoint().y ) > iReverseRunLevel ?  true : false;
	}
	else if( m_pParam->g_MovingDirection == MD_BOTTOM2TOP )
	{
		fReverseRun = (rcPos.CenterPoint().y - rcFirstPos.CenterPoint().y) > iReverseRunLevel ?  true : false;
	}

	//当跟踪丢失或切换到模板跟踪时,不去更新最清晰的那张图。
	bool fIsTemState = (m_State == tsEnd || m_State == tsTemTrack || m_State == tsTemTrackEnd);

	if( !fIsTemState )
	{
		// 为每种类型都保存一个最好的 plate
		PLATE_TYPE nPlateType = LastInfo().nPlateType;

		BOOL fBestCanUpdate = FALSE;
		BOOL fUpdateSmallImage = FALSE;
		if (m_pParam->g_MovingDirection == MD_TOP2BOTTOM)
		{
			fBestCanUpdate = ( (fReverseRun == false && rcPos.CenterPoint().y <= iPosOneY)
					|| (fReverseRun == true && rcPos.CenterPoint().y >= iPosOneY) );
			if (GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE)
			{
				fUpdateSmallImage = true;  // 收费站每帧都更新
			}
		}
		else
		{
			fBestCanUpdate = LastInfo().fltTotalConfidence > m_fltMaxConfidence;
		}

		if (1)//(LastInfo().fltTotalConfidence > m_rgBestPlateInfo[nPlateType].fltConfidence) )
		{
			//韦开拓修改,2011-2-28
			//如果是车尾模式，最清晰那张是第一张。
			//bool fUpdate = true;
			if( m_pParam->g_MovingDirection == MD_BOTTOM2TOP && m_rgBestPlateInfo[nPlateType].fltConfidence.ToFloat() != 0 )
			{
				//fUpdate = false;
				fBestCanUpdate = FALSE;
			}

			if( fBestCanUpdate || fUpdateSmallImage )
			{
				m_rgBestPlateInfo[nPlateType].fltConfidence = LastInfo().fltTotalConfidence;
				m_rgBestPlateInfo[nPlateType].rcPlatePos = LastInfo().rcPlatePos;
				m_rgBestPlateInfo[nPlateType].rcPos = LastInfo().rcPos;
				m_rgBestPlateInfo[nPlateType].dwFrameTime = LastInfo().dwFrameTime;

				//weikt 把处理过的车牌小图保存下来,用于生成二值图 2011-5-3
				m_bestPlatetoBinImage.fltConfidence = LastInfo().fltTotalConfidence;
				m_bestPlatetoBinImage.rcPlatePos = LastInfo().rcPlatePos;
				m_bestPlatetoBinImage.rcPos = LastInfo().rcPos;
				m_bestPlatetoBinImage.dwFrameTime = LastInfo().dwFrameTime;

				/*SAFE_RELEASE(m_bestPlatetoBinImage.pimgPlate);
				RTN_HR_IF_FAILED(CreateReferenceComponentImage(&m_bestPlatetoBinImage.pimgPlate,
					HV_IMAGE_YUV_422,
					imgRectifiedPlate.iWidth,
					imgRectifiedPlate.iHeight
					));
				if(m_bestPlatetoBinImage.pimgPlate != NULL)
				{
					RTN_HR_IF_FAILED(m_bestPlatetoBinImage.pimgPlate->Assign(imgRectifiedPlate));
				}*/
				//weikt end

				// 申请引用对象
				// 用原始的小图，不用处理过的。
				HV_COMPONENT_IMAGE imgCropped;
				imgCropped.nImgType = HV_IMAGE_YUV_422;
				CRect rectPlate = LastInfo().rcPlatePos;

				if( rectPlate.left % 2 != 0 )
				{
					rectPlate.left -= 1;
				}
				rectPlate.bottom += (rectPlate.Height() * 90 / 100);
				rectPlate.right +=  (rectPlate.Width() * 45 / 100);
				if( rectPlate.right % 2 != 0 )
				{
					rectPlate.right -= 1;
				}
			
				if( (rectPlate.left + rectPlate.Width()) <= imgInputPlate.iWidth
					&& (rectPlate.top + rectPlate.Height()) <= imgInputPlate.iHeight )
				{
					CropImage(imgInputPlate, rectPlate, &imgCropped);
				}
				else
				{
					imgCropped = imgInputPlate;
				}

				SAFE_RELEASE(m_rgBestPlateInfo[nPlateType].pimgPlate);
				RTN_HR_IF_FAILED(CreateReferenceComponentImage(&m_rgBestPlateInfo[nPlateType].pimgPlate,
					HV_IMAGE_YUV_422,
					imgCropped.iWidth,
					imgCropped.iHeight,
					0,0,0,"NULL",
					FALSE
					));
				if (imgInputPlate.nImgType != HV_IMAGE_YUV_422)
				{
					RTN_HR_IF_FAILED(m_rgBestPlateInfo[nPlateType].pimgPlate->Convert(imgCropped));
				}
				else
				{
					RTN_HR_IF_FAILED(m_rgBestPlateInfo[nPlateType].pimgPlate->Assign(imgCropped));
				}
			}
		}

		//电警下没有最清晰图和最后大图
		if (fBestCanUpdate)
		{
		    if (LastInfo().fltTotalConfidence > m_fltMaxConfidence)
		    {
		        m_fltMaxConfidence = LastInfo().fltTotalConfidence;
		    }

			//如果是车尾模式，最清晰那张是第一张，最后那张是得分最高这张。
			if( 0 == m_iBestLightMode && !m_pParam->m_fUsedTrafficLight)
			{
				if( m_pParam->g_MovingDirection == MD_BOTTOM2TOP )
				{
					if (m_pimgLastSnapShot)
					{
						ImageRelease(pImgMemOperLog, m_pimgLastSnapShot);
					}
					m_pimgLastSnapShot = pimgInputSnap;
					if (m_pimgLastSnapShot)
					{
						ImageAddRef(pImgMemOperLog, m_pimgLastSnapShot);
						m_rgRect[1] = LastInfo().rcPos;
						m_rcLastPos = LastInfo().rcPos;
						m_dwLastSnapShotRefTime = pParam->dwImageTime;
					}
				}
				else
				{
					if (m_pimgBestSnapShot)
					{
						ImageRelease(pImgMemOperLog, m_pimgBestSnapShot);
					}
					m_pimgBestSnapShot = pimgInputSnap;
					if (m_pimgBestSnapShot)
					{
						ImageAddRef(pImgMemOperLog, m_pimgBestSnapShot);
						m_rgRect[0] = LastInfo().rcPos;
						m_rcBestPos = LastInfo().rcPos;
						m_dwBestSnapShotRefTime = pParam->dwImageTime;
					}
				}
			}
			else if(!m_pParam->m_fUsedTrafficLight)
			{
				if (NULL == m_pimgBestSnapShot)
				{
					m_pimgBestSnapShot = pimgInputSnap;
					if (m_pimgBestSnapShot)
					{
						ImageAddRef(pImgMemOperLog, m_pimgBestSnapShot);
						m_rgRect[0] = LastInfo().rcPos;
						m_rcBestPos = LastInfo().rcPos;
						m_dwBestSnapShotRefTime = pParam->dwImageTime;
					}
				}
			}
		}
	}

	//如果是红绿灯，先判断灯是否正常，不正常不更新图片
	if( m_pParam->m_fUsedTrafficLight )
	{
		int iPosStop  = (m_iStopLinePos * m_iHeight / 100);
		int iPosLeftStop = (m_iLeftStopLinePos * m_iHeight / 100);

		//是否过线,记录过线时的红绿灯状态
		if(m_iPassStopLightScene == -1 && LastInfo().rcPos.CenterPoint().y < iPosStop )
		{
			m_iPassStopLightScene = iLastStatus;
		}
		if( m_iPassLeftStopLightScene == -1 && LastInfo().rcPos.CenterPoint().y < iPosLeftStop )
		{
			m_iPassLeftStopLightScene = iLastStatus;
		}
		//过CarArrive标定线时的时间.
		int iPosCarArrived = m_iCarArrivedPos * m_iHeight / 100;
		if( LastInfo().rcPos.CenterPoint().y < iPosCarArrived
			&& m_dwTriggerTime == 0 )
		{
			m_dwTriggerTime = pParam->dwImageTime;
		}
		//最后一张图的坐标
		m_rectLastImage = LastInfo().rcPos;
	}

	if( m_pParam->m_fUsedTrafficLight && pimgCapture != NULL )
	{
		if( fReverseRun )
		{
			//第一个位置取第一张
			if( m_pimgBestSnapShot != NULL )
			{
				if (m_pimgBeginCapture)
				{
					ImageRelease(pImgMemOperLog, m_pimgBeginCapture);
				}
				m_pimgBeginCapture = m_pimgBestSnapShot;
				ImageAddRef(pImgMemOperLog, m_pimgBeginCapture);
				m_rgRect[2] = m_rgRect[0];
				m_dwBeginCaptureRefTime = m_dwBestSnapShotRefTime;
			}
			//第二个位置用第三个位置来更新
			if( m_rgRect[4].CenterPoint().y < ((iPosTwoY + iPosOneY) / 2)
				|| m_rgRect[2] == m_rgRect[3] )
			{
				if( m_pimgLastCapture != NULL )
				{
					if (m_pimgBestCapture)
					{
						ImageRelease(pImgMemOperLog, m_pimgBestCapture);
					}
					m_pimgBestCapture = m_pimgLastCapture;
					ImageAddRef(pImgMemOperLog, m_pimgBestCapture);
					m_rgRect[3] = m_rgRect[4];
					m_iTwoPosLightScene = m_iThreePosLightScene;
					m_dwBestCaptureRefTime = m_dwLastCaptureRefTime;
				}
			}
			//第三个位置取最后一张
			if( pimgCapture != NULL )
			{
				if (m_pimgLastCapture)
				{
					ImageRelease(pImgMemOperLog, m_pimgLastCapture);
				}
				m_pimgLastCapture = pimgCapture;
				ImageAddRef(pImgMemOperLog, m_pimgLastCapture);
				m_rgRect[4] = rcPos;
				m_iThreePosLightScene = iLastStatus;
				m_dwLastCaptureRefTime = pParam->dwImageTime;
			}
		}
		else
		{
			//第一个位置
			if( rcPos.CenterPoint().y > iPosOneY )
			{
				if (m_pimgBeginCapture)
				{
					ImageRelease(pImgMemOperLog, m_pimgBeginCapture);
				}
				m_pimgBeginCapture = pimgCapture;
				if( m_pimgBeginCapture != NULL )
				{
					ImageAddRef(pImgMemOperLog, m_pimgBeginCapture);	
					m_rgRect[2] = rcPos; 
					m_dwBeginCaptureRefTime = pParam->dwImageTime;
				}
				m_iOnePosLightScene = iLastStatus;
			}

			//第一个位置的灯的状态取第一条抓拍线和停止线中间的位置
			if( rcPos.CenterPoint().y > iPosOneY )
			{
				m_iOnePosLightScene = iLastStatus;
			}

			//第二个位置,用最后一张来更新,避免最后两张图片一样,且要保证不是第一张。
			if( m_rgRect[4].CenterPoint().y > ((iPosTwoY + iPosOneY) / 2)
				|| m_rgRect[2] == m_rgRect[3] )
			{
				if (m_pimgBestCapture)
				{
					ImageRelease(pImgMemOperLog, m_pimgBestCapture);
				}
				m_pimgBestCapture = m_pimgLastCapture;
				if( m_pimgBestCapture != NULL )
				{
					ImageAddRef(pImgMemOperLog, m_pimgBestCapture);
					m_rgRect[3] = m_rgRect[4]; 
					m_dwBestCaptureRefTime = m_dwLastCaptureRefTime;
				}
				m_iTwoPosLightScene = m_iThreePosLightScene;

			}
			else if( m_rgRect[4].CenterPoint().y < ((iPosTwoY + iPosOneY) / 2) && m_rgRect[4].CenterPoint().y > iPosTwoY )
			{
		  		if (m_pimgBestCapture)
				{
					ImageRelease(pImgMemOperLog, m_pimgBestCapture);
				}
				m_pimgBestCapture = m_pimgLastCapture;
				if( m_pimgBestCapture != NULL )
				{
					ImageAddRef(pImgMemOperLog, m_pimgBestCapture);
					m_rgRect[3] = m_rgRect[4]; 
					m_dwBestCaptureRefTime = m_dwLastCaptureRefTime;
				}
				m_iTwoPosLightScene = m_iThreePosLightScene;
			}
			else if( m_rgRect[2].top <= m_rgRect[3].bottom && rcPos.CenterPoint().y > iPosThreeY )
			{
				if (m_pimgBestCapture)
				{
					ImageRelease(pImgMemOperLog, m_pimgBestCapture);
				}
				m_pimgBestCapture = m_pimgLastCapture;
				if( m_pimgBestCapture != NULL )
				{
					ImageAddRef(pImgMemOperLog, m_pimgBestCapture);
					m_rgRect[3] = m_rgRect[4];
					m_dwBestCaptureRefTime = m_dwLastCaptureRefTime;
				}
				m_iTwoPosLightScene = m_iThreePosLightScene;
			}

			//第三个位置
			if( rcPos.CenterPoint().y > iPosThreeY )
			{
				if (m_pimgLastCapture)
				{
					ImageRelease(pImgMemOperLog, m_pimgLastCapture);
				}
				m_pimgLastCapture = pimgCapture;
				if ( m_pimgLastCapture )
				{
					ImageAddRef(pImgMemOperLog, m_pimgLastCapture);
					m_rgRect[4] = LastInfo().rcPos;
					m_dwLastCaptureRefTime = pParam->dwImageTime;
				}
				m_iThreePosLightScene = iLastStatus;
			}
		}
	}
	else if( pimgCapture != NULL )
	{
		bool fUpdateOnePos = false;
		bool fUpdateTwoPos = false;
		bool fUpdateThreePos = false;

		if( m_pParam->g_MovingDirection == MD_TOP2BOTTOM )
		{
			fUpdateOnePos =( rcPos.CenterPoint().y < iPosOneY || fReverseRun );
			fUpdateTwoPos = ( (!fReverseRun && rcPos.CenterPoint().y < iPosTwoY)
				|| (fReverseRun && rcPos.CenterPoint().y > iPosOneY) );
			fUpdateThreePos = ( (!fReverseRun && rcPos.CenterPoint().y < iPosThreeY)
				|| (fReverseRun && rcPos.CenterPoint().y > iPosTwoY) );
		}
		else if( m_pParam->g_MovingDirection == MD_BOTTOM2TOP )
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
			if (m_pimgBeginCapture)
			{
				ImageRelease(pImgMemOperLog, m_pimgBeginCapture);
			}
			m_pimgBeginCapture = pimgCapture;
			if( m_pimgBeginCapture != NULL )
			{
				ImageAddRef(pImgMemOperLog, m_pimgBeginCapture);
				m_rgRect[2] = LastInfo().rcPos; 
				m_dwBeginCaptureRefTime = pParam->dwImageTime;
			}
		}
		//第二个位置
		if( fUpdateTwoPos )
		{
			if (m_pimgBestCapture)
			{
				ImageRelease(pImgMemOperLog, m_pimgBestCapture);
			}
			m_pimgBestCapture = pimgCapture;
			if( m_pimgBestCapture != NULL )
			{
				ImageAddRef(pImgMemOperLog, m_pimgBestCapture);
				m_rgRect[3] = LastInfo().rcPos; 
				m_dwBestCaptureRefTime = pParam->dwImageTime;
			}
		}
		//第三个位置
		if(fUpdateThreePos)
		{
			if (m_pimgLastCapture)
			{
				ImageRelease(pImgMemOperLog, m_pimgLastCapture);
			}
			m_pimgLastCapture = pimgCapture;
			if( m_pimgLastCapture != NULL )
			{
				ImageAddRef(pImgMemOperLog, m_pimgLastCapture);
				m_rgRect[4] = LastInfo().rcPos; 
				m_dwLastCaptureRefTime = pParam->dwImageTime;
			}
		}
	}

	// 如果不是车尾模式
	if( m_pParam->g_MovingDirection != MD_BOTTOM2TOP )
	{
		BOOL fCanUpdate = TRUE;
		if(GetCurrentParam()->g_PlateRcogMode == PRM_CAP_FACE)
		{
			if (fReverseRun)
			{
				if (LastInfo().rcPos.CenterPoint().y < iPosTwoY)
				{
					fCanUpdate = FALSE;
				}
			}
			else
			{
				if (LastInfo().rcPos.CenterPoint().y > iPosTwoY)
				{
					fCanUpdate = FALSE;
				}
			}
		}
		if (fCanUpdate)
		{
			if (m_pimgLastSnapShot)
			{
				ImageRelease(pImgMemOperLog, m_pimgLastSnapShot);
			}
			m_pimgLastSnapShot = pimgInputSnap;
			if ( m_pimgLastSnapShot )
			{
				ImageAddRef(pImgMemOperLog, m_pimgLastSnapShot);
				m_rgRect[1] = LastInfo().rcPos;
				m_rcLastPos = LastInfo().rcPos;
				m_dwLastSnapShotRefTime = pParam->dwImageTime;
			}
		}
	}

	return S_OK;
}

HRESULT CTrackInfo::UpdateBestImage(IReferenceComponentImage *pimgInputSnap)
{
/*
	if (1 == m_iBestLightMode)
	{
		if (m_fCarArrived
			&& (int)(pimgInputSnap->GetRefTime() - m_nCarArriveRealTime) < m_iBestLightWaitTime)
		{
			if (m_pimgBestSnapShot)
			{
				m_pimgBestSnapShot->Release();
				m_pimgBestSnapShot = NULL;
			}
			m_pimgBestSnapShot = pimgInputSnap;
			if (m_pimgBestSnapShot)
			{
				m_pimgBestSnapShot->AddRef();
				if (m_fObservationExists)
				{
					m_rcBestPos = LastInfo().rcPos;
				}
				else
				{
					m_rcBestPos = PredictPosition(pimgInputSnap->GetRefTime(), FALSE);
				}
				m_rgRect[0] = m_rcBestPos;
			}
			HV_Trace(3,"\n最亮图替换成功");
		}
	}
	return S_OK;
	*/
	return S_OK;
}

CRect CTrackInfo::CurrentPosition()
{
	if (m_cPlateInfo>0)
	{
		return LastInfo().rcPos;
	}
	else
	{
		return CRect(-1, -1, -1, -1);
	}
}

CRect CTrackInfo::PredictPosition(UINT nNowTime, BOOL fEnableDetReverseRun, IScaleSpeed *pScaleSpeed)
{
	if (m_dwPredictTime == nNowTime &&
		m_rcPredict != CRect(0, 0, 0, 0))
	{
		return m_rcPredict;
	}

	const float fltTrackInflate1 = 2.25f;
	const float fltTrackInflate2 = 2.0f;
	const float fltTrackInflate3 = 3.0f;
	const float fltTrackOffset1 = 1.75f;
	const float fltTrackOffset2 = 2.0f;

	CRect rect(-1, -1, -1, -1);
	if (m_cPlateInfo > 0)
	{
		rect = LastInfo().rcPos;
		int iInflateHeight = rect.Height();
		iInflateHeight = MAX(iInflateHeight, 22);

		if (m_cPlateInfo == 1)
		{
			if (fEnableDetReverseRun)
			{
				switch (m_pParam->g_MovingDirection)
				{
				case MD_TOP2BOTTOM:
				case MD_BOTTOM2TOP:
					rect.InflateRect(
						(int)(fltTrackInflate3 * iInflateHeight),
						(int)((fltTrackInflate3 + fltTrackOffset2) * iInflateHeight)
						);
					break;
				case MD_LEFT2RIGHT:
				case MD_RIGHT2LEFT:
					rect.InflateRect(
						(int)((fltTrackInflate3 + fltTrackOffset2) * iInflateHeight),
						(int)(fltTrackInflate3 * iInflateHeight)
						);
					break;
				default:
					rect.InflateRect(
						(int)((fltTrackInflate3 + fltTrackOffset2) * iInflateHeight),
						(int)((fltTrackInflate3 + fltTrackOffset2) * iInflateHeight)
						);
					break;
				}
			}
			else
			{
				switch (m_pParam->g_MovingDirection)
				{
				case MD_TOP2BOTTOM:
					rect.OffsetRect(0, (int)(fltTrackOffset1 * iInflateHeight));
					rect.InflateRect(
						(int)(fltTrackInflate3 * iInflateHeight),
						(int)((fltTrackInflate1 + fltTrackOffset1) * iInflateHeight)
						);
					break;
				case MD_BOTTOM2TOP:
					rect.OffsetRect(0, -(int)(fltTrackOffset1 * iInflateHeight));
					rect.InflateRect(
						(int)(fltTrackInflate3 * iInflateHeight),
						(int)((fltTrackInflate1 + fltTrackOffset1) * iInflateHeight)
						);
					break;
				case MD_LEFT2RIGHT:
					rect.OffsetRect((int)(fltTrackOffset1 * iInflateHeight), 0);
					rect.InflateRect(
						(int)((fltTrackInflate1 + fltTrackOffset1) * iInflateHeight),
						(int)(fltTrackInflate3 * iInflateHeight)
						);
					break;
				case MD_RIGHT2LEFT:
					rect.OffsetRect(-(int)(fltTrackOffset1 * iInflateHeight), 0);
					rect.InflateRect(
						(int)((fltTrackInflate1 + fltTrackOffset1) * iInflateHeight),
						(int)(fltTrackInflate3 * iInflateHeight)
						);
					break;
				default:
					break;
				}
			}
		}
		else
		{
			if( pScaleSpeed != NULL && m_pParam->m_fUsedTrafficLight )
			{
				float fltCarSpeed(0.0f);
				float fltInterval1 = (float)(m_rgPlateInfo[m_cPlateInfo - 1].dwFrameTime
					- m_rgPlateInfo[m_cPlateInfo - 2].dwFrameTime);

				float fltScale(0.0f);
				fltCarSpeed = pScaleSpeed->CalcCarSpeedNewMethod(
					m_rgPlateInfo[m_cPlateInfo - 2].rcPos,
					m_rgPlateInfo[m_cPlateInfo - 1].rcPos,
					(DWORD32)fltInterval1,
					true,
					PLATE_NORMAL,
					fltScale
					);

				float fltInterval = (float)(nNowTime - m_rgPlateInfo[m_cPlateInfo - 1].dwFrameTime);
				float fltDistanceNew = (fltInterval / 1000)   *  (fltCarSpeed * 1000 / 3600) * 3;
				if( fltDistanceNew < 0 ) fltDistanceNew = -fltDistanceNew;
				HV_POINT ptDest;
				HV_POINT ptOrg;
				ptOrg.x = m_rgPlateInfo[m_cPlateInfo - 1].rcPos.CenterPoint().x;
				ptOrg.y = m_rgPlateInfo[m_cPlateInfo - 1].rcPos.CenterPoint().y;

				int iTemp = 1;
				if( LastInfo().nInfoType == PI_LPR && !m_fObservationExists && m_fhasTemple )
				{
					iTemp = 3;
				}
				else if(LastInfo().nInfoType == PI_LPR)
				{
					iTemp = 2;
				}
				if( pScaleSpeed->GetActurePoint(ptOrg, ptDest, fltDistanceNew) )
				{	//计算车辆的运动轨迹
					int idx = m_cPlateInfo - 3;
					if(idx < 0)
					{
						idx = 0;
					}
					int offsetX = m_rgPlateInfo[m_cPlateInfo - 1].rcPos.CenterPoint().x - m_rgPlateInfo[idx].rcPos.CenterPoint().x;
					//左移动
					if(offsetX < -m_rgPlateInfo[m_cPlateInfo - 1].rcPos.Width()/3)
					{
						offsetX = m_rgPlateInfo[m_cPlateInfo - 1].rcPos.CenterPoint().x - m_rgPlateInfo[m_cPlateInfo - 1].rcPos.Width()/3;
					}
					//右移动
					else if(offsetX > m_rgPlateInfo[m_cPlateInfo - 1].rcPos.Width()/3)
					{
						offsetX = m_rgPlateInfo[m_cPlateInfo - 1].rcPos.CenterPoint().x + m_rgPlateInfo[m_cPlateInfo - 1].rcPos.Width()/3;
					}
					else
					{
						offsetX = m_rgPlateInfo[m_cPlateInfo - 1].rcPos.CenterPoint().x;
					}
					//计算预测框
					rect.top = ptDest.y - (m_rgPlateInfo[m_cPlateInfo - 1].rcPos.Height() * iTemp);
					rect.left = offsetX - (m_rgPlateInfo[m_cPlateInfo - 1].rcPos.Width() * 1.2);
					rect.bottom = m_rgPlateInfo[m_cPlateInfo - 1].rcPos.bottom + m_rgPlateInfo[m_cPlateInfo - 1].rcPos.Height() / 2;
					rect.right = offsetX + (m_rgPlateInfo[m_cPlateInfo - 1].rcPos.Width() * 1.2);
				}
			}
			else
			{
			float fltInterval = (float)(nNowTime - m_rgPlateInfo[m_cPlateInfo - 1].dwFrameTime);
			float fltInterval1 = (float)(m_rgPlateInfo[m_cPlateInfo - 1].dwFrameTime
				- m_rgPlateInfo[m_cPlateInfo - 2].dwFrameTime);
			float fltDistanceX1 = (float)((m_rgPlateInfo[m_cPlateInfo - 1].rcPos.left
				+ m_rgPlateInfo[m_cPlateInfo - 1].rcPos.right
				- m_rgPlateInfo[m_cPlateInfo - 2].rcPos.left
				- m_rgPlateInfo[m_cPlateInfo - 2].rcPos.right) >> 1);
			float fltDistanceY1 = (float)((m_rgPlateInfo[m_cPlateInfo - 1].rcPos.top
				+ m_rgPlateInfo[m_cPlateInfo - 1].rcPos.bottom
				- m_rgPlateInfo[m_cPlateInfo - 2].rcPos.top
				- m_rgPlateInfo[m_cPlateInfo - 2].rcPos.bottom) >> 1);
			int iOffsetX, iOffsetY;
			fltInterval1 = MAX(fltInterval1, 1.0f);

			iOffsetX = (int)(fltDistanceX1 * (MAX(0.5f, fltInterval / fltInterval1)));
			iOffsetY = (int)(fltDistanceY1 * (MAX(0.5f, fltInterval / fltInterval1)));

			rect.OffsetRect(iOffsetX, iOffsetY);
			switch (m_pParam->g_MovingDirection)
			{
			case MD_TOP2BOTTOM:
			case MD_BOTTOM2TOP:
				rect.InflateRect(
					(int)(fltTrackInflate3 * iInflateHeight),
					(int)((fltTrackInflate2 + fltTrackOffset2) * iInflateHeight)
					);
				break;
			case MD_LEFT2RIGHT:
			case MD_RIGHT2LEFT:
				rect.InflateRect(
					(int)((fltTrackInflate2 + fltTrackOffset2) * iInflateHeight),
					(int)(fltTrackInflate3 * iInflateHeight)
					);
				break;
			default:
				rect.InflateRect(
					(int)(fltTrackInflate3 * iInflateHeight),
					(int)(fltTrackInflate3 * iInflateHeight)
					);
			}

			//判断车辆是否逆行,调整车牌检测Scale参数
			m_fReverseRun = false;
			switch (m_pParam->g_MovingDirection)
			{
			case MD_TOP2BOTTOM:
				if (m_rgPlateInfo[m_cPlateInfo - 1].rcPos.top - m_rgPlateInfo[0].rcPos.top < 0)
				{
					m_fReverseRun = true;
				}
				break;
			case MD_BOTTOM2TOP:
				if (m_rgPlateInfo[m_cPlateInfo - 1].rcPos.top - m_rgPlateInfo[0].rcPos.top > 0)
				{
					m_fReverseRun = true;
				}
				break;
			case MD_LEFT2RIGHT:
				if (m_rgPlateInfo[m_cPlateInfo - 1].rcPos.left - m_rgPlateInfo[0].rcPos.left < 0)
				{
					m_fReverseRun = true;
				}
				break;
			case MD_RIGHT2LEFT:
				if (m_rgPlateInfo[m_cPlateInfo - 1].rcPos.left - m_rgPlateInfo[0].rcPos.left > 0)
				{
					m_fReverseRun = true;
				}
				break;
			}
		}
	}
	}
	m_rcPredict = rect;
	m_dwPredictTime = nNowTime;
	return rect;
}

HRESULT CTrackInfo::NewTrack(
		PROCESS_ONE_FRAME_PARAM* pParam,
		HV_COMPONENT_IMAGE imgPlate,
		PlateInfo plate,
		int iLastStatus,
		int iCurStatus,
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond
)
{
	static int nIDCounter = 0;

	// filling the field
	m_iVideoID = pParam->iVideoID;
	m_nID = nIDCounter++;
	m_State = tsNew;
	m_nObservedFrames = 1;
	m_nMissingTime = 0; //
	m_fCarArrived = false;
	m_fCarLefted = false;
	m_fVoted = false;
	m_nResultEqualTimes = 0;
	m_nStartFrameNo = pParam->dwFrameNo;
	m_fReverseRun = false;
	m_fReverseRunVoted = false;
	m_iVoteSimilarityNum = 0;
	m_fObservationExists = true;
	m_fPredictCarArrive = false;

	m_dwDopeFirstPos = 0;
	m_dwDopeEndPos = 0;
	m_fIsTrigger = false;
	m_dwTriggerIndex = 0;
	m_iCapCount = 0;
	m_iCapSpeed = 0;

	// put the plateinfo in the list
	m_fltMaxConfidence = -1.0f;

	m_dwFirstFlameTime = pParam->dwImageTime;
	m_rgPlateInfo[0] = plate;
	m_cPlateInfo=1;

	// 聇rack时要重新设置
	if (m_pObj != NULL)
	{
		delete m_pObj;
	}
	m_pObj = NULL;
	m_fCanOutput = false;
	m_fTempTrackExists = false;
	m_fhasTemple = false;
	m_fTempCloseEdge = false;		// 模版是否靠近边缘
	m_nCloseEdgeCount = 0;			// 模版靠近边缘帧计数器
	m_nMinYPos = 10000;
	m_nMaxYPos = 10000;			// 最大的Y坐标偏移
	m_nMaxXPos = 10000;			// 最大的X坐标偏移

	m_bPreVote = false;
	m_rgRect[0].SetRectEmpty();
	m_rgRect[1].SetRectEmpty();
	m_rgRect[2].SetRectEmpty();
	m_rgRect[3].SetRectEmpty();
	m_rgRect[4].SetRectEmpty();
	m_dwBestSnapShotRefTime = 0;
	m_dwLastSnapShotRefTime = 0;
	m_dwBeginCaptureRefTime = 0;
	m_dwBestCaptureRefTime = 0;
	m_dwLastCaptureRefTime = 0;

	m_fHasTrigger = false;
	m_dwTriggerTimeMs = 0;
	m_dwTriggerImageTime = 0;

	m_iCarStopFrame = 0;

    m_nRecogInVaildCount = 0;
    m_fltRecogInVaildRatio = 0.f;

	IMG_MEM_OPER_LOG* pImgMemOperLog = &pProcessRespond->cImgMemOperLog;

	if(!m_pParam->m_fUsedTrafficLight)
	{
		if (m_pimgBestSnapShot)
		{
			ImageRelease(pImgMemOperLog, m_pimgBestSnapShot);
		}
		m_pimgBestSnapShot = pParam->pCurFrame;
		if ( m_pimgBestSnapShot )
		{
			ImageAddRef(pImgMemOperLog, m_pimgBestSnapShot);
			m_rgRect[0] = LastInfo().rcPos;
			m_rcBestPos = LastInfo().rcPos;
			m_dwBestSnapShotRefTime = pParam->dwImageTime;
		}

		if (m_pimgLastSnapShot)
		{
			ImageRelease(pImgMemOperLog, m_pimgLastSnapShot);
		}
		m_pimgLastSnapShot = pParam->pCurFrame;
		if ( m_pimgLastSnapShot )
		{
			ImageAddRef(pImgMemOperLog, m_pimgLastSnapShot);
			m_rgRect[1] = LastInfo().rcPos;
			m_rcLastPos = LastInfo().rcPos;
			m_dwLastSnapShotRefTime = pParam->dwImageTime;
		}
	}

	//电警模式才输出三张抓拍图
	//黄冠榕 2011-08-12
	if(GetCurrentParam()->g_PlateRcogMode == PRM_ELECTRONIC_POLICE)
	{
		if (m_pimgBeginCapture)
		{
			ImageRelease(pImgMemOperLog, m_pimgBeginCapture);
		}
		m_pimgBeginCapture = pParam->pCurFrame;
		if ( m_pimgBeginCapture )
		{
			ImageAddRef(pImgMemOperLog, m_pimgBeginCapture);
			m_rgRect[2] = LastInfo().rcPos;
			m_dwBeginCaptureRefTime = pParam->dwImageTime;
		}

		if (m_pimgBestCapture)
		{
			ImageRelease(pImgMemOperLog, m_pimgBestCapture);
		}
		m_pimgBestCapture = pParam->pCurFrame;
		if ( m_pimgBestCapture )
		{
			ImageAddRef(pImgMemOperLog, m_pimgBestCapture);
			m_rgRect[3] = LastInfo().rcPos;
			m_dwBestCaptureRefTime = pParam->dwImageTime;
		}

		if (m_pimgLastCapture)
		{
			ImageRelease(pImgMemOperLog, m_pimgLastCapture);
		}
		m_pimgLastCapture = pParam->pCurFrame;
		if ( m_pimgLastCapture )
		{
			ImageAddRef(pImgMemOperLog, m_pimgLastCapture);
			m_rgRect[4] = LastInfo().rcPos;
			m_dwLastCaptureRefTime = pParam->dwImageTime;
		}
	}

	m_iOnePosLightScene = m_iTwoPosLightScene = m_iThreePosLightScene = iLastStatus;

	m_stCarInfo.rcFG = CRect(0,0,0,0);
	m_stCarInfo.fltCarH = m_stCarInfo.fltCarW = 0.0f;

	PLATE_TYPE nPlateType = LastInfo().nPlateType;
	if ( (LastInfo().fltTotalConfidence > m_rgBestPlateInfo[nPlateType].fltConfidence) )
	{
		m_rgBestPlateInfo[nPlateType].fltConfidence = LastInfo().fltTotalConfidence;
		m_rgBestPlateInfo[nPlateType].rcPlatePos = LastInfo().rcPlatePos;
		m_rgBestPlateInfo[nPlateType].rcPos = LastInfo().rcPos;
		m_rgBestPlateInfo[nPlateType].dwFrameTime = LastInfo().dwFrameTime;

		//weikt 把处理过的车牌小图保存下来,用于生成二值图 2011-5-3
		m_bestPlatetoBinImage.fltConfidence = LastInfo().fltTotalConfidence;
		m_bestPlatetoBinImage.rcPlatePos = LastInfo().rcPlatePos;
		m_bestPlatetoBinImage.rcPos = LastInfo().rcPos;
		m_bestPlatetoBinImage.dwFrameTime = LastInfo().dwFrameTime;

		//weikt end

		// 申请引用对象
		// 用原始的小图，不用处理过的。
		HV_COMPONENT_IMAGE imgCropped;
		imgCropped.nImgType = HV_IMAGE_YUV_422;
		CRect rectPlate = LastInfo().rcPlatePos;

		if( rectPlate.left % 2 != 0 )
		{
			rectPlate.left -= 1;
		}
		rectPlate.bottom += (rectPlate.Height() * 90 / 100);
		rectPlate.right +=  (rectPlate.Width() * 45 / 100);
		if( rectPlate.right % 2 != 0 )
		{
			rectPlate.right -= 1;
		}

		if( (rectPlate.left + rectPlate.Width()) <= imgPlate.iWidth
			&& (rectPlate.top + rectPlate.Height()) <= imgPlate.iHeight )
		{
			CropImage(imgPlate, rectPlate, &imgCropped);
		}
		else
		{
			imgCropped = imgPlate;
		}

		SAFE_RELEASE(m_rgBestPlateInfo[nPlateType].pimgPlate);
		RTN_HR_IF_FAILED(CreateReferenceComponentImage(&m_rgBestPlateInfo[nPlateType].pimgPlate,
			HV_IMAGE_YUV_422,
			imgCropped.iWidth,
			imgCropped.iHeight,
			0,0,0,"NULL",
			FALSE
			));
		if (imgPlate.nImgType != HV_IMAGE_YUV_422)
		{
			RTN_HR_IF_FAILED(m_rgBestPlateInfo[nPlateType].pimgPlate->Convert(imgCropped));
		}
		else
		{
			RTN_HR_IF_FAILED(m_rgBestPlateInfo[nPlateType].pimgPlate->Assign(imgCropped));
		}
	}

	return UpdatePlateImage(imgPlate, pParam, iLastStatus, iCurStatus, pProcessRespond);
}

HRESULT CTrackInfo::Detach(CTrackInfo &tiDest, PROCESS_ONE_FRAME_RESPOND* pProcessRespond)
{
	tiDest.Clear(pProcessRespond);

	*((CTrackInfoHeader*)&tiDest)=*((CTrackInfoHeader*)this);

	tiDest.m_pimgBestSnapShot = m_pimgBestSnapShot;
	m_pimgBestSnapShot = NULL;
	tiDest.m_pimgLastSnapShot = m_pimgLastSnapShot;
	m_pimgLastSnapShot = NULL;
	tiDest.m_pimgBeginCapture = m_pimgBeginCapture;
	m_pimgBeginCapture = NULL;
	tiDest.m_pimgBestCapture = m_pimgBestCapture;
	m_pimgBestCapture = NULL;
	tiDest.m_pimgLastCapture = m_pimgLastCapture;
	m_pimgLastCapture = NULL;

	// 物体检测
	tiDest.m_pObj = m_pObj;
	m_pObj = NULL;

	for (int i=0; i<ARRSIZE(m_rgBestPlateInfo); i++)
	{
		RTN_HR_IF_FAILED(m_rgBestPlateInfo[i].Detach(tiDest.m_rgBestPlateInfo[i]));
	}

	m_bestPlatetoBinImage.Detach(tiDest.m_bestPlatetoBinImage);

	Clear(pProcessRespond);

	return S_OK;
}

HRESULT CTrackInfo::Vote(PLATE_TYPE &nVotedType, PLATE_COLOR &nVotedColor, PBYTE8 pbVotedContent)
{
	HV_DebugInfo( DEBUG_STR_TRACK_FILE, "CTrackInfo::Vote()\n");

	for (int i=0; i<m_cPlateInfo; i++)
	{
		WORD16 wszPlateName[100];
		RTN_HR_IF_FAILED(m_rgPlateInfo[i].GetPlateName(wszPlateName));
		HV_DebugInfo( DEBUG_STR_VOTE_FILE,
						"[%03d] Color=%d, Type=%d, Name=[%S], Conf=%f\n",
						i,
						m_rgPlateInfo[i].color,
						m_rgPlateInfo[i].nPlateType,
						wszPlateName,
						m_rgPlateInfo[i].fltTotalConfidence.ToFloat()
					);
	}

	RemoveInvalidVotes();

	// 对车牌类型进行投票
	CParamQuanNum rgfltTypeScore[PLATE_TYPE_COUNT] = {0};
	int nWJCount = 0;
	int nDoubleWJCount = 0;
	for (int i = 0; i < m_cPlateInfo; i++)
	{
		if (m_rgPlateInfo[i].fValid && m_rgPlateInfo[i].nInfoType == PI_LPR)
		{
			rgfltTypeScore[m_rgPlateInfo[i].nPlateType] += m_rgPlateInfo[i].fltTotalConfidence;
		}
		if(m_rgPlateInfo[i].nPlateType == PLATE_WJ && m_rgPlateInfo[i].fValid)
		{
			nWJCount++;
		}
		else if(m_rgPlateInfo[i].nPlateType == PLATE_DOUBLE_WJ && m_rgPlateInfo[i].fValid)
		{
			nDoubleWJCount++;
		}
	}
	nVotedType = PLATE_UNKNOWN;

	for (int i = 1; i < PLATE_TYPE_COUNT; i++)
	{
		if (rgfltTypeScore[i] > rgfltTypeScore[nVotedType])
		{
			nVotedType = (PLATE_TYPE)i;
		}
	}

	if(nWJCount > 5) 
	{
		nVotedType = PLATE_WJ;
	}
	else if(nDoubleWJCount > 5)
	{
		nVotedType = PLATE_DOUBLE_WJ;
	}

	int iCharNum(7);
	if (PLATE_DOUBLE_GREEN == m_nVotedType)
	{
		iCharNum = 8;
	}

	// TODO: Magic Number
	// 统计所有满足“类型”条件的车牌颜色 和各个字符的得分
	CParamQuanNum rgfltCharScore[8][g_cnChars]={0};
	int rgiCharCount[8][g_cnChars] = {0};
	unsigned int rgnColorScore[PC_COUNT]={0};
	for (int i = 0; i < m_cPlateInfo; i++)
	{
		if (m_rgPlateInfo[i].fValid &&
			m_rgPlateInfo[i].nPlateType == nVotedType
			&& m_rgPlateInfo[i].nInfoType == PI_LPR )
		{
			for (int j = 0; j < iCharNum; j++)
			{
				rgfltCharScore[j][m_rgPlateInfo[i].rgbContent[j]] +=
					m_rgPlateInfo[i].rgfltConfidence[j];
				rgiCharCount[j][m_rgPlateInfo[i].rgbContent[j]]++;
			}
			rgnColorScore[m_rgPlateInfo[i].color]++;
		}
	}

	//车身颜色投票
	if(m_fEnableRecgCarColor)
	{
		unsigned int rgnCarColorScore[CC_COUNT] = {0};
		if(GetCurrentParam()->g_PlateRcogMode == PRM_TOLLGATE)  //收费站模式下，只取后面的结果作为有效结果
		{
			int StartVote = MAX_INT(m_cPlateInfo-5,0);
			for (int i = StartVote; i < m_cPlateInfo; i++)
			{
				rgnCarColorScore[m_rgPlateInfo[i].nCarColour]++;
			}
		}
		else
		{
			for (int i = 0; i < m_cPlateInfo; i++)
			{
				rgnCarColorScore[m_rgPlateInfo[i].nCarColour]++;
			}
		}

		m_nVotedCarColor = (CAR_COLOR)1;
		for (int i = 2; i < CC_COUNT; i++)
		{
			if (rgnCarColorScore[m_nVotedCarColor] < rgnCarColorScore[i])
			{
				m_nVotedCarColor = (CAR_COLOR)i;
			}
		}
		if (0 == rgnCarColorScore[m_nVotedCarColor])
		{
			m_nVotedCarColor = CC_UNKNOWN;
		}
	}

	// 输出调试信息
	for (int i = 0; i < iCharNum; i++)
	{
		HV_DebugInfo(DEBUG_STR_VOTE_FILE, "Char[%d] :", i);
		for (int j = 1; j < g_cnChars; j++)
		{
			if (rgfltCharScore[i][j] > 0)
			{
				HV_DebugInfo(	DEBUG_STR_VOTE_FILE,
									"[%C](%03d)=%f, ",
									MapId2Char(j),
									j,
									rgfltCharScore[i][j].ToFloat()
								);
			}
		}

		HV_DebugInfo(DEBUG_STR_VOTE_FILE, "\n");
	}

	// 对7位字符进行预投票
	HV_Trace(3, "\nVotedScore:");
	CParamQuanNum rgfltVotedCharScore[8];
	for (int i = 0; i < iCharNum; i++)
	{
		pbVotedContent[i] = 0;
		rgfltVotedCharScore[i] = 0.0f;
		for (int j = 1; j < g_cnChars; j++)
		{
			//如果是港澳牌但是投票结果的第0位和第1位不是粤Z则不对港澳字符进行投票
			//2010-11-11 韦开拓修改
			if(j == 114 || j == 115)
			{
				if(pbVotedContent[0] != 57 || pbVotedContent[1] != 36)
				{
					continue;;
				}
			}
			//end
			if (rgfltCharScore[i][pbVotedContent[i]] < rgfltCharScore[i][j])
			{
				pbVotedContent[i] = j;
				rgfltVotedCharScore[i] = rgfltCharScore[i][j].ToFloat() / rgiCharCount[i][j];
			}
		}
		HV_Trace(3, "%.02f  ", rgfltVotedCharScore[i].ToFloat());
	}
	HV_Trace(3, "\n");

	for (int i = 0; i < iCharNum; i++)
	{
		//计算平均得分
		float fltTotalScore = 0.0f;
		for (int j = 0; j < iCharNum; j++)
		{
			if (j == i) continue;
			fltTotalScore += rgfltVotedCharScore[j].ToFloat();
		}
		float fltAvgScore = fltTotalScore / 6;
		// 识别率低于阈值，用#代替
		if (rgfltVotedCharScore[i].ToFloat() < (fltAvgScore * (float)m_nRecogAsteriskThreshold / 100.0f))
		{
			if (i == 0)
			{
				pbVotedContent[i] = 1;
			}
			else
			{
				pbVotedContent[i] = 0;
			}
		}
	}

	//added by liujie 卡口车头条件下将相似度判断放宽松
	int iSimT = 4;
	if (GetCurrentParam()->g_PlateRcogMode == PRM_HIGHWAY_HEAD
		|| GetCurrentParam()->g_PlateRcogMode == PRM_CAP_FACE)
	{
		iSimT = 3;
	}//end added 

    int nDetCount = 0;
	for (int i = 0; i < m_cPlateInfo; i++)
	{
		int iCount(0);
		if( m_rgPlateInfo[i].nInfoType != PI_LPR ) continue;

		for (int j = 1; j < iCharNum; j++)
		{
			if ( pbVotedContent[j] == 0)
			{
				iCount++;
			}
			else if (pbVotedContent[j] == m_rgPlateInfo[i].rgbContent[j])
			{
				iCount++;
			}
		}
		if (iCount > iSimT)
		{
			m_iVoteSimilarityNum++;
		}
        if (m_rgPlateInfo[i].nInValidCount >= 4)
        {
            ++m_nRecogInVaildCount;
        }
        nDetCount++;
	}

    m_fltRecogInVaildRatio = ((float)m_nRecogInVaildCount)/nDetCount;

	// 对车牌颜色进行预投票
	nVotedColor = PC_UNKNOWN;
	for (int i = 1; i < PC_COUNT; i++)
	{
		if (rgnColorScore[nVotedColor] < rgnColorScore[i])
		{
			nVotedColor = (PLATE_COLOR)i;
		}
	}
	if (((CTrackerImpl*)g_pTrackerVideo)->m_LightType == NIGHT)		// 晚上 黑牌少
	{
		if (nVotedColor == PC_BLACK && rgnColorScore[PC_BLUE] * 5 > rgnColorScore[PC_BLACK] * 3)
		{
			nVotedColor = PC_BLUE;
		}
	}

	//---------------------------------------------------------------------------------------------
	// 车牌类型验证
	// 针对各种类型的车牌进行规则检查，修正预投票结果
	//---------------------------------------------------------------------------------------------
	switch(nVotedType)
	{
	case PLATE_NORMAL:				// 蓝牌或黑牌
		if ((pbVotedContent[0] == 57) && (pbVotedContent[1] == 36))
		{
			// 前两位为“粤Z”
			if ((pbVotedContent[6] != 114) && (pbVotedContent[6] != 115))		// 港澳字尾
			{
				// 最后一位不是“港澳”，则改成港澳的字尾(有可能票数为0)
				if (rgfltCharScore[6][114] < rgfltCharScore[6][115])
				{
					pbVotedContent[6] = 115;
				}
				else
				{
					pbVotedContent[6] = 114;
				}
			}
			nVotedColor = PC_BLACK;			// 强制设为黑色
		}
		break;
	case PLATE_POLICE:				// 黄牌、军牌或广西警牌
		if (IsMilitaryAlpha(pbVotedContent[0]) || IsMilitaryChn(pbVotedContent[0]) || ((pbVotedContent[0] == 38) && (pbVotedContent[1] == 32)))
		{
			// 对于军牌   后5位只使用数字投票
			for (int i = 2; i < 7; i++)
			{
				if (!IsDigitChar(pbVotedContent[i]))
				{
					// 如果不是数字，则修改投票
					pbVotedContent[i] = 1;
					for (int j = 2; j <= 10; j++)
					{
						if (rgfltCharScore[i][pbVotedContent[i]] < rgfltCharScore[i][j])
						{
							pbVotedContent[i] = j;
						}
					}
				}
			}
			nVotedColor = PC_WHITE;			// 强制设为白色
			break;
		}

		// (字头中已经没有军牌了，只能是黄牌和广西警牌)
		if (pbVotedContent[6] == 37)		// 最后一位为“警”
		{
			if (pbVotedContent[0] != 58)	// 但第一位非“桂”
			{
				// 重新在数字、“学”中投票
				pbVotedContent[6] = 1;
				for (int j = 2; j <= 10; j++)
				{
					if (rgfltCharScore[6][pbVotedContent[6]] < rgfltCharScore[6][j])
					{
						pbVotedContent[6] = j;
					}
				}
				if (rgfltCharScore[6][pbVotedContent[6]] < rgfltCharScore[6][101])		// 和“学”比较
				{
					pbVotedContent[6] = 101;
				}
				// 标准黄牌或浅蓝牌
				if (nVotedColor == PC_WHITE)
				{
					nVotedColor = PC_YELLOW;			// 强制设为黄色,也可能是浅蓝牌
				}
			}
			else
			{
				// 广西警牌
				nVotedColor = PC_WHITE;			// 强制设为白色
			}
		}
		else
		{
			// 标准黄牌或浅杜?
			if (nVotedColor == PC_WHITE)
			{
				nVotedColor = PC_YELLOW;			// 强制设为黄色,也可能是浅蓝牌
			}
		}
		break;
	case PLATE_POLICE2:							// 标准警牌
		pbVotedContent[6] = 37;					// 强制设为警字
		nVotedColor = PC_WHITE;					// 强制设为白色
		break;
	case PLATE_DOUBLE_YELLOW:					// 双层牌
		if (IsMilitaryAlpha(pbVotedContent[0]) || IsMilitaryChn(pbVotedContent[0]) || ((pbVotedContent[0] == 38) && (pbVotedContent[1] == 32)))
		{
			// 双层军牌, 对于军牌   后5位只使用数字投票
			for (int i = 2; i < 7; i++)
			{
				if (!IsDigitChar(pbVotedContent[i]))
				{
					// 如果不是数字，则修改投票
					pbVotedContent[i] = 1;
					for (int j = 2; j <= 10; j++)
					{
						if (rgfltCharScore[i][pbVotedContent[i]] < rgfltCharScore[i][j])
						{
							pbVotedContent[i] = j;
						}
					}
				}
			}
			nVotedColor = PC_WHITE;				// 强制设为白色
		}
		else
		{
			// 双黄牌
			nVotedColor = PC_YELLOW;			// 强制设为黄色
		}
		break;
	default:
		break;
	}

	if (pbVotedContent[6] == 101)//如果最后一位为'学'字,则设为黄牌
	{
		nVotedColor = PC_YELLOW;
	}
	//---------------------------------------------------------------------------------------------
	// 车牌类型验证结束
	//---------------------------------------------------------------------------------------------

	// 车辆宽、高的统计计算
	int nTempCar(0);
	float fltTempW(0.0f);
	float fltTempH(0.0f);

	for (int i = 0; i < m_cPlateInfo; i++)
	{
		if ((m_rgPlateInfo[i].stCarInfo.rcFG != CRect(0, 0, 0, 0)) && (m_rgPlateInfo[i].stCarInfo.fltCarW > 0.8))
		{
			nTempCar++;
			fltTempW += m_rgPlateInfo[i].stCarInfo.fltCarW;
			fltTempH += m_rgPlateInfo[i].stCarInfo.fltCarH;
		}

	}
	if (nTempCar > 0)
	{
		m_stCarInfo.fltCarW = fltTempW / nTempCar;
		m_stCarInfo.fltCarH = fltTempH / nTempCar;
	}

	// 对车型进行投票
	unsigned int rgnCarTypeScore[CT_COUNT]={0};				// 车型
	for (int i = 0; i < m_cPlateInfo; i++)
	{
		//if( m_rgPlateInfo[i].nInfoType != PI_LPR ) continue;

		rgnCarTypeScore[m_rgPlateInfo[i].nCarType]++;	// 车型统计
	}
	m_nVotedCarType = CAR_TYPE(CT_UNKNOWN + 1);
	for (int i = m_nVotedCarType + 1; i < CT_COUNT; i++)
	{
		if (rgnCarTypeScore[m_nVotedCarType] < rgnCarTypeScore[i])
		{
			m_nVotedCarType = (CAR_TYPE)i;
		}
	}

	// 车型的修正
	if (m_nVotedCarType == CT_WALKMAN || m_nVotedCarType == CT_BIKE)
	{
		m_nVotedCarType = CT_SMALL;
	}

	switch(nVotedType)
	{
	case PLATE_NORMAL:						// 蓝牌或黑牌
		if (m_nVotedCarType == CT_LARGE || m_nVotedCarType == CT_UNKNOWN)
		{
			m_nVotedCarType = CT_SMALL;
		}
		break;
	case PLATE_POLICE:						// 黄牌、军牌或广西警牌
		m_nVotedCarType = CT_LARGE;
		if (nVotedColor == PC_WHITE || nVotedColor == PC_BLUE || nVotedColor == PC_LIGHTBLUE)
		{
			m_nVotedCarType = CT_SMALL;
		}
		break;
	case PLATE_POLICE2:						// 标准警牌
		m_nVotedCarType = CT_SMALL;
		break;
	case PLATE_DOUBLE_YELLOW:				// 双层牌
		m_nVotedCarType = CT_LARGE;
		break;
	case PLATE_DOUBLE_MOTO:
		m_nVotedCarType = CT_BIKE;
		break;
	default:
		m_nVotedCarType = CT_SMALL;
		break;
	}

	// 输出车牌信息
	WORD16 wszPlateName[100];
	RTN_HR_IF_FAILED(GetPlateNameUnicode(
		wszPlateName,
		nVotedType,
		nVotedColor,
		pbVotedContent
	));

	HV_DebugInfo(	DEBUG_STR_VOTE_FILE,
						"VotedResult: Color=%d, Real=%d, Type=%d, Name=[%S]\n",
						m_rgPlateInfo[0].color,
						nVotedColor,
						nVotedType,
						wszPlateName
					);

	//还原车牌有效标志
	for (int i= 0; i < m_cPlateInfo; i++)
		m_rgPlateInfo[i].fValid = true;

	return S_OK;
}

bool CTrackInfo::PreVote(void)
{
	if(m_bPreVote)
		return true;

	if(m_cPlateInfo < 4)
		return false;

	// 对车牌类型进行投票
	CParamQuanNum rgfltTypeScore[PLATE_TYPE_COUNT] = {0};
	int nWJCount = 0;
	int nDoubleWJCount = 0;
	for (int i = 0; i < m_cPlateInfo; i++)
	{
		if (m_rgPlateInfo[i].fValid && m_rgPlateInfo[i].nInfoType == PI_LPR)
		{
			rgfltTypeScore[m_rgPlateInfo[i].nPlateType] += m_rgPlateInfo[i].fltTotalConfidence;
		}
		if(m_rgPlateInfo[i].nPlateType == PLATE_WJ)
		{
			nWJCount++;
		}
		else if(m_rgPlateInfo[i].nPlateType == PLATE_DOUBLE_WJ)
		{
			nDoubleWJCount++;
		}
	}

	PLATE_TYPE nVotedType = PLATE_UNKNOWN;
	for (int i = 1; i < PLATE_TYPE_COUNT; i++)
	{
		if (rgfltTypeScore[i] > rgfltTypeScore[nVotedType])
		{
			nVotedType = (PLATE_TYPE)i;
		}
	}

	if(nWJCount > 3) 
	{
		nVotedType = PLATE_WJ;
	}
	else if(nDoubleWJCount > 3)
	{
		nVotedType = PLATE_DOUBLE_WJ;
	}

	int iCharNum(7);
	if (PLATE_DOUBLE_GREEN == m_nVotedType)
	{
		iCharNum = 8;
	}

	// 统计所有满足“类型”条件的车牌各个字符的得分
	CParamQuanNum rgfltCharScore[8][g_cnChars]={0};
	int rgiCharCount[8][g_cnChars] = {0};
		for (int i = 0; i < m_cPlateInfo; i++)
	{
		if (m_rgPlateInfo[i].fValid &&
			m_rgPlateInfo[i].nPlateType == nVotedType
			&& m_rgPlateInfo[i].nInfoType == PI_LPR )
		{
			for (int j = 0; j < iCharNum; j++)
			{
				rgfltCharScore[j][m_rgPlateInfo[i].rgbContent[j]] += m_rgPlateInfo[i].rgfltConfidence[j];
				rgiCharCount[j][m_rgPlateInfo[i].rgbContent[j]]++;
			}
		}
	}
	
	// 对7位字符进行预投票
	BYTE8 pbVotedContent[8];
	CParamQuanNum rgfltVotedCharScore[8];
	for (int i = 0; i < iCharNum; i++)
	{
		pbVotedContent[i] = 0;
		rgfltVotedCharScore[i] = 0.0f;
		for (int j = 1; j < g_cnChars; j++)
		{
			//如果是港澳牌但是投票结果的第0位和第1位不是粤Z则不对港澳字符进行投票
			//2010-11-11 韦开拓修改
			if(j == 114 || j == 115)
			{
				if(pbVotedContent[0] != 57 || pbVotedContent[1] != 36)
				{
					continue;;
				}
			}
			//end
			if (rgfltCharScore[i][pbVotedContent[i]] < rgfltCharScore[i][j])
			{
				pbVotedContent[i] = j;
				rgfltVotedCharScore[i] = rgfltCharScore[i][j].ToFloat() / rgiCharCount[i][j];
			}
		}
	}


	for (int i = 0; i < iCharNum; i++)
	{
		//计算平均得分
		float fltTotalScore = 0.0f;
		for (int j = 0; j < iCharNum; j++)
		{
			if (j == i) continue;
			fltTotalScore += rgfltVotedCharScore[j].ToFloat();
		}
		float fltAvgScore = fltTotalScore / 6;
		// 识别率低于阈值，用#代替
		if (rgfltVotedCharScore[i].ToFloat() < (fltAvgScore * (float)m_nRecogAsteriskThreshold / 100.0f))
		{
			if (i == 0)
			{
				pbVotedContent[i] = 1;
			}
			else
			{
				pbVotedContent[i] = 0;
			}
		}
	}

	int iVoteSimilarityNum = 0;
	for (int i = 0; i < m_cPlateInfo; i++)
	{
		int iCount(0);
		if( m_rgPlateInfo[i].nInfoType != PI_LPR ) 
			continue;

		for (int j = 1; j < iCharNum; j++)
		{
			if ( pbVotedContent[j] == 0)
			{
				iCount++;
			}
			else if (pbVotedContent[j] == m_rgPlateInfo[i].rgbContent[j])
			{
				iCount++;
			}
		}
		if (iCount > 4)
		{
			iVoteSimilarityNum++;
		}
	}
	m_bPreVote = iVoteSimilarityNum >= 4;
	return	m_bPreVote;
}

// 去掉多余的无效票
HRESULT CTrackInfo::RemoveInvalidVotes()
{
	//若有个性车牌，则只保留个性车牌的结果
	int i;
	for (i = 0; i < m_cPlateInfo; i++)
	{
		if (m_rgPlateInfo[i].nPlateType == PLATE_INDIVIDUAL)
		{
			break;
		}
	}
	if (i < m_cPlateInfo)
	{
		for (int i = 0; i < m_cPlateInfo; i++)
		{
			m_rgPlateInfo[i].fValid = (m_rgPlateInfo[i].nPlateType == PLATE_INDIVIDUAL);
		}
		return S_OK;
	}
	//剔除太大、太小、得分太低的车牌

	//实际车牌数
	int iPlateCount = 0;
	for(int i = 0; i < m_cPlateInfo; ++i)
	{
		if( m_rgPlateInfo[i].nInfoType == PI_LPR ) iPlateCount++;
	}

	int iInvalidRate = 100;
	if (GetCurrentParam()->g_PlateRcogMode == PRM_HIGHWAY_HEAD
		|| GetCurrentParam()->g_PlateRcogMode == PRM_CAP_FACE)
	{
		iInvalidRate = 200;	// 车头模式时不可用车牌比例除以2
	}
	int nLowConfDeleteCount = iPlateCount * m_nRemoveLowConfForVote / iInvalidRate;
	if (iPlateCount - nLowConfDeleteCount > 0)
	{
		while (nLowConfDeleteCount > 0)
		{
			CParamQuanNum fltMinConfidence = 1.1f;
			int iDelete = -1;
			for (int i = 0; i < m_cPlateInfo; i++)
			{
				if( m_rgPlateInfo[i].nInfoType != PI_LPR ) continue;

				if (m_rgPlateInfo[i].fValid &&
					m_rgPlateInfo[i].fltTotalConfidence < fltMinConfidence)
				{
					iDelete = i;
					fltMinConfidence = m_rgPlateInfo[i].fltTotalConfidence;
				}
			}
			if (iDelete >= 0)
			{
				m_rgPlateInfo[iDelete].fValid = false;
				nLowConfDeleteCount--;
			}
			else
			{
				nLowConfDeleteCount = 0;
			}
		}
	}
	return S_OK;
}

float CTrackInfo::AverageConfidence()
{
	CParamQuanNum fltAverage;
	int iDeleteCount = 0;
	for (int i=0; i<m_cPlateInfo; i++)
	{
		if( m_rgPlateInfo[i].nInfoType != PI_LPR )
		{
			iDeleteCount++;
			continue;
		}
		fltAverage+=m_rgPlateInfo[i].fltTotalConfidence;
	}

	if( m_cPlateInfo == iDeleteCount ) return 0;
	return fltAverage.ToFloat() / (m_cPlateInfo - iDeleteCount);
}

float CTrackInfo::AverageFirstConfidence()
{
	CParamQuanNum fltAverage;
	int iDeleteCount = 0;
	for (int i=0; i<m_cPlateInfo; i++)
	{
		if( m_rgPlateInfo[i].nInfoType != PI_LPR )
		{
			iDeleteCount++;
			continue;
		}
		fltAverage+=m_rgPlateInfo[i].rgfltConfidence[ 0 ];
	}

	if( m_cPlateInfo == iDeleteCount ) return 0;
	return fltAverage.ToFloat() / (m_cPlateInfo - iDeleteCount);
}

CParamQuanNum CTrackInfo::AveragePosConfidence(int iPos)
{
	CParamQuanNum fltAverage;
	if ((iPos < 0) || (iPos >= 7))
	{
		return 0;
	}

	int iDeleteCount = 0;
	for (int i = 0; i<m_cPlateInfo; i++)
	{
		if( m_rgPlateInfo[i].nInfoType != PI_LPR )
		{
			iDeleteCount++;
			continue;
		}
		fltAverage += m_rgPlateInfo[i].rgfltConfidence[ iPos ];
	}

	if( m_cPlateInfo == iDeleteCount ) return 0;
	return fltAverage.ToFloat() / (m_cPlateInfo - iDeleteCount);
}

int CTrackInfo::Similarity( const PlateInfo &plateinfo )
{
	int iMaxSimilarity = 0;
	for (int i = 0; i < m_cPlateInfo; i++)
	{
		int iSimilairty = m_rgPlateInfo[i].Similarity(plateinfo);
		if (iSimilairty > iMaxSimilarity)
			iMaxSimilarity = iSimilairty;
	}
	return iMaxSimilarity;
}

bool CTrackInfo::IsTwins( CTrackInfo &trackInfo )
{
	PLATE_TYPE plateType;
	PLATE_COLOR plateColor;
	BYTE8 rgbContent[7];
	trackInfo.Vote(plateType, plateColor, rgbContent);
	return (plateType == m_nVotedType &&
		plateColor == m_nVotedColor &&
		memcmp(rgbContent, m_rgbVotedResult, sizeof(rgbContent)) == 0);
}

float CTrackInfo::AverageStdVar()
{
	int iSum = 0;
	int iDeleteCount = 0;
	for (int i = 0; i < m_cPlateInfo; i++)
	{
		if( m_rgPlateInfo[i].nInfoType != PI_LPR )
		{
			iDeleteCount++;
			continue;
		}
		iSum += m_rgPlateInfo[i].nVariance;
	}

	if( m_cPlateInfo == iDeleteCount ) return 0;
	return (float)iSum / (m_cPlateInfo - iDeleteCount);
}

float CTrackInfo::AveragePlateBrightness()
{
	int iSum = 0;
	int iDeleteCount = 0;
	for (int i = 0; i < m_cPlateInfo; i++)
	{
		if( m_rgPlateInfo[i].nInfoType != PI_LPR )
		{
			iDeleteCount++;
			continue;
		}
		iSum += m_rgPlateInfo[i].nAvgY;
	}

	if( m_cPlateInfo == iDeleteCount ) return 0;
	return (float)iSum / (m_cPlateInfo - iDeleteCount);
}

// 更新模版匹配需要的模版
HRESULT CTrackInfo::UpdateObjTemple(
									HV_COMPONENT_IMAGE& img,
									CRect &rcObj)
{
	if (GetHvImageData(&img, 0) == NULL)
	{
		return E_POINTER;			// 空指针错误
	}

	if( CTrackInfo::m_iMatchScanSpeed == 0 )
	{
		CObjMatch::X_SCAN_STEP = 8;
		CObjMatch::Y_SCAN_STEP = 4;
	}
	else if( CTrackInfo::m_iMatchScanSpeed == 1 )
	{
		CObjMatch::X_SCAN_STEP = 4;
		CObjMatch::Y_SCAN_STEP = 2;
	}

	if ((rcObj.top <0 ) || (rcObj.left < 0)
		|| (rcObj.bottom <= rcObj.top) || (rcObj.right <= rcObj.left))
	{
		return E_INVALIDARG;
	}

	if (m_pObj == NULL)
	{
		// 建立模版对象
		m_pObj = new CObjMatch;
		if (m_pObj == NULL)
		{
			return E_OUTOFMEMORY;
		}
	}


//	HV_COMPONENT_IMAGE imgCur;
//	RTN_HR_IF_FAILED(pImg->GetImage(&imgCur));			// 取谜枷?

	m_pObj->UpdateTemple(&img, rcObj);

	return S_OK;
}

CItgArea* CTrackInfo::GetPredictArea(DWORD32 dwRefTime, const HiVideo::CRect& rcFullSearch, const HiVideo::CRect& rcTrackArea, BOOL fEnableDetReverseRun, IScaleSpeed *pScaleSpeed)
{
	CRect rcPredict = PredictPosition(dwRefTime, fEnableDetReverseRun, pScaleSpeed);

	//如果预测位置包含在全检区域或者不在跟踪区域内则不用输出
	if( rcFullSearch.Contains(&rcPredict) ||
		!rcPredict.IntersectsWith(rcTrackArea) )
	{
		return NULL;
	}

	return SUCCEEDED(m_cItgArea.SetRect(rcPredict,FALSE))?&m_cItgArea:NULL;
}


//==========================================================
//红绿灯场景
/*
TRAFFICLIGHT_SCENE g_rgLightScene[MAX_SCENE_COUNT] = {{TLS_UNSURE, TLS_UNSURE, TLS_UNSURE, TLS_UNSURE}};
void ClearLightScene()
{
	for(int i = 0; i < MAX_SCENE_COUNT; ++i)
	{
		g_rgLightScene[i].lsForward = g_rgLightScene[i].lsLeft = g_rgLightScene[i].lsRight = g_rgLightScene[i].lsTurn = TLS_UNSURE;
	}
}
void SetLightScene(int iPos, TRAFFICLIGHT_SCENE ts)
{
	if( iPos >= 0 && iPos <MAX_SCENE_COUNT )
	{
		g_rgLightScene[iPos] = ts;
	}
}
bool GetLightScene(int iPos, TRAFFICLIGHT_SCENE* pts)
{
	bool fRet = false;
	if( pts == NULL ) return fRet;
	if( iPos >= 0 && iPos <MAX_SCENE_COUNT)
	{
		*pts = g_rgLightScene[iPos];
		fRet = true;
	}
	else if(iPos == -1)
	{
		(*pts).lsForward = TLS_UNSURE;
		(*pts).lsRight = TLS_UNSURE;
		(*pts).lsLeft = TLS_UNSURE;
		(*pts).lsTurn = TLS_UNSURE;
		fRet = true;
	}
	return fRet;
}

SCENE_INFO g_rgSceneInfo[MAX_SCENE_COUNT];
void ClearSceneInfo()
{
	for(int i = 0; i < MAX_SCENE_COUNT; ++i)
	{
		memset(g_rgSceneInfo[i].pszInfo, 0, 64);
	}
}
void SetSceneInfo(int iPos, const SCENE_INFO& ts)
{
	if( iPos >= 0 && iPos <MAX_SCENE_COUNT )
	{
		memcpy(g_rgSceneInfo[iPos].pszInfo, ts.pszInfo, 64);
	}
}
bool GetSceneInfo(int iPos, SCENE_INFO* pts)
{
	bool fRet = false;
	if( pts == NULL ) return fRet;
	if( iPos >= 0 && iPos <MAX_SCENE_COUNT)
	{
		memcpy((*pts).pszInfo, g_rgSceneInfo[iPos].pszInfo, 64);
		fRet = true;
	}
	return fRet;
}

TRAFFICLIGHT_TYPE g_rgLightType[MAX_LIGHT_TYPE_COUNT] = {{-1,-1,TP_LEFT,TLS_UNSURE}};
int g_iLightTypeCount = 0;

void ClearTrafficLightType()
{
	g_iLightTypeCount = 0;
	for(int i = 0; i < MAX_LIGHT_TYPE_COUNT; ++i)
	{
		g_rgLightType[i].iTeam = -1;
	}
}
void SetTrafficLightType(int iTeam, int iPos, _TRAFFICLIGHT_POSITION tpPos, _TRAFFICLIGHT_LIGHT_STATUS tlsStatush)
{
	if(g_iLightTypeCount >= MAX_LIGHT_TYPE_COUNT ) return;
	g_rgLightType[g_iLightTypeCount].iTeam = iTeam;
	g_rgLightType[g_iLightTypeCount].iPos = iPos;
	g_rgLightType[g_iLightTypeCount].tpPos = tpPos;
	g_rgLightType[g_iLightTypeCount].tlsStatus = tlsStatush;
	g_iLightTypeCount++;
}

bool GetTrafficLightType(int iTeam, int iPos, TRAFFICLIGHT_TYPE* ltLightType)
{
	if( iTeam < 0 || iPos < 0 || ltLightType == NULL ) return false;
	for(int i = 0; i < MAX_LIGHT_TYPE_COUNT; ++i)
	{
		if( g_rgLightType[i].iTeam == iTeam
			&& g_rgLightType[i].iPos == iPos )
		{
			*ltLightType = g_rgLightType[i];
			return true;
		}
	}
	return false;
}

int g_rgSceneStatus[STATUS_BUF_LEN] = {0};

void SetSceneStatus(int nPos, int nStatus)
{
	g_rgSceneStatus[nPos%STATUS_BUF_LEN] = nStatus;
}

int GetSceneStatus(int nPos)
{
	return g_rgSceneStatus[nPos%STATUS_BUF_LEN];
}

//如果是灯组状态与场景是邦定的
int GetSceneNumber(const LIGHT_TEAM_STATUS& ltsInfo, const int& iSceneCount)
{
	int iRet = -1;
	char szSceneInfo[64];
	memset(szSceneInfo, 0, 64);
	int iPos = 0;
	DWORD32 dwStatus = 0;
	for(DWORD32 i = 0; i < ltsInfo.nTeamCount; ++i)
	{
		dwStatus = ltsInfo.pdwStatus[i];
		for(int k = ltsInfo.pnLightCount[i] - 1; k >= 0 ; --k)
		{
			switch (dwStatus & 0x0F)
			{
			case TS_RED:
				szSceneInfo[iPos + k] = '1';
				break;
			case TS_YELLOW:
				szSceneInfo[iPos + k] = '2';
				break;
			case TS_GREEN:
				szSceneInfo[iPos + k] = '3';
				break;
			case TS_UNSURE:
				szSceneInfo[iPos + k] = '0';
				break;
			default:
				szSceneInfo[iPos + k] = '?';
				break;
			}
			dwStatus >>= 4;
		}
		iPos += ltsInfo.pnLightCount[i];
		if( DWORD32(i+1) < ltsInfo.nTeamCount )
		{
			szSceneInfo[iPos++] = '_';
		}
	}

	SCENE_INFO stInfo;
	int iLen = 0;
	for( int i = 0; i < iSceneCount; ++i )
	{
		if( GetSceneInfo(i, &stInfo) )
		{
			iLen = (int)strlen(stInfo.pszInfo);
			if( strlen(szSceneInfo) != iLen ) continue;

			int k = 0;
			for(k = 0; k < iLen; ++k)
			{
				if( stInfo.pszInfo[k] == '?' ) continue;
				if( stInfo.pszInfo[k] == '*' &&  szSceneInfo[k] > '0' ) continue;
				if( stInfo.pszInfo[k] != szSceneInfo[k] ) break;
			}
			if( k >= iLen )
			{
				iRet = i;
				break;
			}
		}
	}
	return iRet;
}

//传入各灯组的状态，返回场景状态编号
int TransitionScene(const LIGHT_TEAM_STATUS& ltsInfo, const int& iSceneCount, const int& iLastOkLightStatus,  bool fFlag)
{
	int iRet = -1;

	if( !fFlag ) return GetSceneNumber(ltsInfo, iSceneCount);

	if( ltsInfo.nTeamCount == 0 || ltsInfo.nTeamCount > MAX_TRAFFICLIGHT_COUNT ) return iRet;
	//场景
	TRAFFICLIGHT_SCENE* pts = new TRAFFICLIGHT_SCENE[ltsInfo.nTeamCount];
	if( pts == NULL ) return iRet;

	int* piUsed = new int[ltsInfo.nTeamCount * 4];
	if( piUsed == NULL )
	{
		delete[] pts;
		return iRet;
	}
	memset(piUsed, 0, sizeof(int) *(ltsInfo.nTeamCount * 4));
	memset(pts, 0, sizeof(TRAFFICLIGHT_SCENE) * ltsInfo.nTeamCount);
	TRAFFICLIGHT_TYPE lt;
	int iCount = 0;
	DWORD32 dwTmp = 0;

	for( DWORD32 i = 0; i < ltsInfo.nTeamCount; ++i )
	{
		dwTmp = ltsInfo.pdwStatus[i];
		iCount = ltsInfo.pnLightCount[i];
		for(int k = iCount - 1; k >= 0; --k)
		{
			if( (dwTmp & 0x01) && GetTrafficLightType(i, k, &lt) )
			{
				LIGHT_STATUS tlsStatus = lt.tlsStatus;
				if (lt.tlsStatus == TLS_UNSURE)
				{
					switch (dwTmp & 0x0F)
					{
					case TS_RED:
						tlsStatus = TLS_RED;
						break;
					case TS_YELLOW:
						tlsStatus = TLS_YELLOW;
						break;
					case TS_GREEN:
						tlsStatus = TLS_GREEN;
						break;
					}
				}

				switch(lt.tpPos)
				{
				case TP_LEFT:
					pts[i].lsLeft = tlsStatus;
					piUsed[i * 4 + 0] = 1;
					break;
				case TP_RIGHT:
					pts[i].lsRight = tlsStatus;
					piUsed[i * 4 + 1] = 1;
					break;
				case TP_FORWARD:
					pts[i].lsForward = tlsStatus;
					piUsed[i * 4 + 2] = 1;
					break;
				case TP_TURN:
					pts[i].lsTurn = tlsStatus;
					piUsed[i * 4 + 3] = 1;
					break;
				case TP_UNSURE:
					{
						//未定义只能用在黄灯
						if( tlsStatus == TLS_YELLOW && iLastOkLightStatus != -1 )
						{
							int iNextStatus = (iLastOkLightStatus + 1) % iSceneCount;
							TRAFFICLIGHT_SCENE tsLast, tsNext;
							GetLightScene(iLastOkLightStatus, &tsLast);
							GetLightScene(iNextStatus, &tsNext);
							if( (tsLast.lsForward == TLS_GREEN && tsNext.lsForward == TLS_YELLOW)
								|| tsLast.lsForward == TLS_YELLOW )
							{
								pts[i].lsForward = TLS_YELLOW;
								piUsed[i * 4 + 2] = 1;
							}
							else if( (tsLast.lsLeft == TLS_GREEN && tsNext.lsLeft == TLS_YELLOW)
								|| tsLast.lsLeft == TLS_YELLOW )
							{
								pts[i].lsLeft = TLS_YELLOW;
								piUsed[i * 4 + 0] = 1;
							}
							else if( (tsLast.lsRight == TLS_GREEN && tsNext.lsRight == TLS_YELLOW)
								|| tsLast.lsRight == TLS_YELLOW)
							{
								pts[i].lsRight = TLS_YELLOW;
								piUsed[i * 4 + 1] = 1;
							}
							else if( (tsLast.lsTurn == TLS_GREEN && tsNext.lsTurn == TLS_YELLOW)
								|| tsLast.lsTurn == TLS_YELLOW )
							{
								pts[i].lsTurn = TLS_YELLOW;
								piUsed[i * 4 + 3] = 1;
							}
						}
					}
					break;

				default:
					break;
				}
			}
			dwTmp >>= 4;
		}
	}

	TRAFFICLIGHT_SCENE tsDest = {TLS_UNSURE, TLS_UNSURE, TLS_UNSURE, TLS_UNSURE};
	bool fL, fR, fF, fT;
	fL = fR = fF = fT = false;
	for(DWORD32 i = 0; i < ltsInfo.nTeamCount; ++i)
	{
		if( !fL && pts[i].lsLeft != TLS_UNSURE && piUsed[i * 4 + 0] == 1)
		{
			tsDest.lsLeft = pts[i].lsLeft;
			fL = true;
		}
		else if( fL && piUsed[i * 4 + 0] == 1 && pts[i].lsLeft != tsDest.lsLeft )
		{
			tsDest.lsLeft = TLS_UNSURE;
		}

		if( !fR && pts[i].lsRight != TLS_UNSURE && piUsed[i * 4 + 1] == 1)
		{
			tsDest.lsRight = pts[i].lsRight;
			fR = true;
		}
		else if( fR && piUsed[i * 4 + 1] == 1 && pts[i].lsRight != tsDest.lsRight )
		{
			tsDest.lsRight = TLS_UNSURE;
		}

		if( !fF && pts[i].lsForward != TLS_UNSURE && piUsed[i * 4 + 2] == 1)
		{
			tsDest.lsForward = pts[i].lsForward;
			fF = true;
		}
		else if( fF && piUsed[i * 4 + 2] == 1 && pts[i].lsForward != tsDest.lsForward )
		{
			tsDest.lsForward = TLS_UNSURE;
		}

		if( !fT && pts[i].lsTurn != TLS_UNSURE && piUsed[i * 4 + 3] == 1)
		{
			tsDest.lsTurn = pts[i].lsTurn;
			fT = true;
		}
		else if( fT && piUsed[i * 4 + 3] == 1 && pts[i].lsTurn != tsDest.lsTurn )
		{
			tsDest.lsTurn = TLS_UNSURE;
		}
	}

	TRAFFICLIGHT_SCENE tsTmp;
	for(int i = 0; i < iSceneCount; ++i)
	{
		if( GetLightScene(i, &tsTmp) )
		{
			if( tsTmp.lsLeft == tsDest.lsLeft && tsTmp.lsRight == tsDest.lsRight
				&& tsTmp.lsForward == tsDest.lsForward && tsTmp.lsTurn == tsDest.lsTurn )
			{
				iRet = i;
				break;
			}
		}
	}

	delete[] pts;
	delete[] piUsed;

	return iRet;
}

__inline int GetPixelValue(BYTE8 r, BYTE8 g, BYTE8 b)
{
	BYTE8 max = MAX_INT(MAX_INT(r,g),b);

	return max;
}

__inline int GetPixelValue2(int r, int g, int b)
{
	int max = MAX_INT((r * 2 - g - b), (g + b - r * 2));
	if (max > 255) max = 255;
	if (max < 0) max = 0;
	return max;
}
__inline int GetPixelRedValue(int r, int g, int b)
{
	int max = (r * 2 - g - b) / 2 ;
	if (max < 0) max = 0;
	return max;
}
__inline int GetPixelGreenValue(int r, int g, int b)
{
	int max = (g + b - r * 2) / 2;
	if (max < 0) max = 0;
	return max;
}

void CropGrayImg(HV_COMPONENT_IMAGE& imgSrc, HV_RECT rcCrop, HV_COMPONENT_IMAGE& imgCrop)
{
	imgCrop.nImgType = HV_IMAGE_GRAY;
	imgCrop.iStrideWidth[0] = imgSrc.iStrideWidth[0];
	imgCrop.iStrideWidth[1] = imgSrc.iStrideWidth[1];
	imgCrop.iStrideWidth[2] = imgSrc.iStrideWidth[2];
	imgCrop.iWidth = rcCrop.right - rcCrop.left;
	imgCrop.iHeight = rcCrop.bottom - rcCrop.top;

	SetHvImageData(&imgCrop, 0, GetHvImageData(&imgSrc, 0) + rcCrop.top * imgSrc.iStrideWidth[0] + rcCrop.left);
	SetHvImageData(&imgCrop, 1, NULL);
	SetHvImageData(&imgCrop, 2, NULL);
}

int CalcLightValue(HV_COMPONENT_IMAGE& imgLight)
{
	int nCount = 0;
	BYTE8* pBufLine = GetHvImageData(&imgLight, 0);
	for(int i = 0; i < imgLight.iHeight; i++, pBufLine += imgLight.iStrideWidth[0])
	{
		for(int j = 0; j < imgLight.iWidth; j++)
		{
			if(pBufLine[j]  == 255) nCount++;
		}
	}

	return nCount*255/(imgLight.iWidth * imgLight.iHeight);
}

//计算区域偏绿或偏红
HRESULT LightIsGreen(
					const HV_COMPONENT_IMAGE& imgR,
					const HV_COMPONENT_IMAGE &imgG,
					const HV_COMPONENT_IMAGE &imgB,
					const HiVideo::CRect rect,
					bool &fIsGreen)
{
	if (imgR.nImgType != HV_IMAGE_GRAY ||
		imgG.nImgType != HV_IMAGE_GRAY ||
		imgB.nImgType != HV_IMAGE_GRAY ||
		GetHvImageData(&imgR, 0) == NULL ||
		GetHvImageData(&imgG, 0) == NULL ||
		GetHvImageData(&imgB, 0) == NULL)
	{
		return E_INVALIDARG;
	}

	int iSum = 0;
	for(int y = rect.top; y < rect.bottom; y++)
	{
		for(int x = rect.left; x < rect.right; x++)
		{
			iSum += *(GetHvImageData(&imgG, 0) + y * imgG.iStrideWidth[0] + x);
			iSum += *(GetHvImageData(&imgB, 0) + y * imgB.iStrideWidth[0] + x);
			iSum -= *(GetHvImageData(&imgR, 0) + y * imgR.iStrideWidth[0] + x) * 2;
		}
	}
	fIsGreen = (iSum > 0);
	return S_OK;
}

DWORD32 CTrafficLightInfo::GetLightStatus(HV_COMPONENT_IMAGE imgLight, int nPosCount, int nType)
{
	if(nType != 0 && nType != 1) return TS_UNSURE;

	//转为YUV_422
	CPersistentComponentImage imgTrans;
	if(imgLight.nImgType != HV_IMAGE_YUV_422)
	{
		imgTrans.Create(HV_IMAGE_YUV_422, imgLight.iWidth, imgLight.iHeight);
		imgTrans.Convert(imgLight);
		imgLight = imgTrans;
	}

	//转换成RGB
	CPersistentComponentImage imgR, imgG, imgB, imgLR, imgLG;
	imgR.Create(HV_IMAGE_GRAY, imgLight.iWidth, imgLight.iHeight);
	imgG.Create(HV_IMAGE_GRAY, imgLight.iWidth, imgLight.iHeight);
	imgB.Create(HV_IMAGE_GRAY, imgLight.iWidth, imgLight.iHeight);
	imgLR.Create(HV_IMAGE_GRAY, imgLight.iWidth, imgLight.iHeight);
	imgLG.Create(HV_IMAGE_GRAY, imgLight.iWidth, imgLight.iHeight);
	IMAGE_ConvertYCbCr2BGR(&imgLight, GetHvImageData(&imgR, 0), GetHvImageData(&imgG, 0), GetHvImageData(&imgB, 0), imgR.iStrideWidth[0]);


	//计算亮度值
	RESTRICT_PBYTE8 pRLine = GetHvImageData(&imgR, 0);
	RESTRICT_PBYTE8 pGLine = GetHvImageData(&imgG, 0);
	RESTRICT_PBYTE8 pBLine = GetHvImageData(&imgB, 0);
	RESTRICT_PBYTE8 pLRLine = GetHvImageData(&imgLR, 0);
	RESTRICT_PBYTE8 pLGLine = GetHvImageData(&imgLG, 0);
	for(int i = 0; i < imgR.iHeight; i++)
	{
		for(int j = 0; j < imgR.iWidth; j++)
		{
			pLRLine[j] = (BYTE8)(GetPixelRedValue(pRLine[j], pGLine[j], pBLine[j]));
			pLGLine[j] = (BYTE8)(GetPixelGreenValue(pRLine[j], pGLine[j], pBLine[j]));
		}
		pRLine += imgR.iStrideWidth[0];
		pGLine += imgG.iStrideWidth[0];
		pBLine += imgB.iStrideWidth[0];
		pLRLine += imgLR.iStrideWidth[0];
		pLGLine += imgLG.iStrideWidth[0];
	}

	//二值化
	BYTE8 bThreshold;
	BYTE8 bThresholdStat = 25 + CTrackInfo::m_iTrafficLightThread * 5;
	IMAGE_CalcBinaryThreshold(imgLR, 1, 1, &bThreshold);
	if (bThreshold < bThresholdStat) bThreshold = bThresholdStat;
	hvThreshold(&imgLR, &imgLR, bThreshold, 255, HV_THRESH_BINARY);

	IMAGE_CalcBinaryThreshold(imgLG, 1, 1, &bThreshold);
	if (bThreshold < bThresholdStat) bThreshold = bThresholdStat;
	hvThreshold(&imgLG, &imgLG, bThreshold, 255, HV_THRESH_BINARY);
	hvDilateNew(&imgLG, &imgG, m_pElement);
	hvErodeNew(&imgG, &imgLG, m_pElement);

	//HvImageDebugShow(&imgLG, cvSize(500, 200));

	//生成分割框
	CRect rcPos[CTrafficLightInfo::MAX_POS_COUNT];

	if( nType == 0)
	{
		for (int i = 0; i < nPosCount; i++)
		{
			rcPos[i].top = 0;
			rcPos[i].bottom = imgLight.iHeight;
			rcPos[i].right = (i + 1) * imgLight.iWidth / nPosCount;

			if(i == 0)
			{
				rcPos[i].left = 0;
			}
			else
			{
				rcPos[i].left = rcPos[i-1].right;
			}
		}
	}
	else if( nType == 1 )
	{
		for (int i = 0; i < nPosCount; i++)
		{
			rcPos[i].left = 0;
			rcPos[i].right = imgLight.iWidth;
			rcPos[i].bottom = (i + 1) * imgLight.iHeight / nPosCount;

			if(i == 0)
			{
				rcPos[i].top = 0;
			}
			else
			{
				rcPos[i].top = rcPos[i-1].bottom;
			}
		}
	}
	else
	{
		return TS_UNSURE;
	}

	//计算每个灯的亮度值
	HV_COMPONENT_IMAGE imgCrop;
	CRect rcCrop;
	int r[CTrafficLightInfo::MAX_POS_COUNT] = {0};
	int g[CTrafficLightInfo::MAX_POS_COUNT] = {0};

	for(int i = 0; i < nPosCount; i++)
	{
		rcCrop = rcPos[i];
		//去除边缘影响
		rcCrop.DeflateRect(rcCrop.Width() /6, rcCrop.Height()/6);
		CropGrayImg(imgLR, rcCrop, imgCrop);
		r[i] = CalcLightValue(imgCrop);
		CropGrayImg(imgLG, rcCrop, imgCrop);
		g[i] = CalcLightValue(imgCrop);
	}

	DWORD32 nStatus = 0;
	int iBrighValue = 30 + CTrackInfo::m_iTrafficLightBrightValue * 15;
	for( int i = 0; i < nPosCount; i++)
	{	
#if 0
		//黄国超调试
		char szTemp[255];
		sprintf(szTemp, "r[%d]=%d,g[%d]=%d,iBrighValue=%d\n", i, r[i], i, g[i], iBrighValue);
		strcat(g_szDebugInfo, szTemp);
#endif
		nStatus <<=4;
		if (r[i] > g[i])
		{
			if (r[i] > iBrighValue)
			{
				nStatus |= TS_RED;
			}
		}
		else
		{
			if (g[i] > iBrighValue)
			{
				nStatus |= TS_GREEN;
			}
		}
	}
	return nStatus;
}

HRESULT CTrafficLightInfo::UpdateStatus(HV_COMPONENT_IMAGE* pSceneImage)
{
	if( !pSceneImage ) return E_INVALIDARG;

	CRect rcLight = GetRect();
	CRect rcMax(0, 0, pSceneImage->iWidth, pSceneImage->iHeight);

	if(rcLight.bottom <= rcLight.top ||
		rcLight.right <= rcLight.left ||
		!rcLight.IntersectRect(&rcMax, &rcLight))
	{
		m_nLastStatus = TS_UNSURE;
		return S_FALSE;
	}

	CPersistentComponentImage imgTemp;
	if (HV_IMAGE_BT1120_ROTATE_Y == pSceneImage->nImgType)
	{
		if(FAILED(imgTemp.Create(HV_IMAGE_YUV_422, rcLight.Width(), rcLight.Height())))
		{
			return E_OUTOFMEMORY;
		}
		imgTemp.CropAssign(*pSceneImage, rcLight); 
	}
	else
	{
		HV_COMPONENT_IMAGE imgCrop;
		CropImage(*pSceneImage, rcLight, &imgCrop);
		//必须在CropImage之后才能Create图片，否则会导致宽度不一致，图像转换失效,黄国超修改,2011-10-12
		if(FAILED(imgTemp.Create(HV_IMAGE_YUV_422, rcLight.Width(), rcLight.Height())))
		{
			return E_OUTOFMEMORY;
		}
		imgTemp.Convert(imgCrop);
	}

	CPersistentComponentImage imgLight;
	if(FAILED(imgLight.Create(HV_IMAGE_YUV_422, imgTemp.iWidth, imgTemp.iHeight*2)))
	{
		return E_OUTOFMEMORY;
	}
	PlateResize(imgTemp, imgLight);

	m_nLastStatus = GetLightStatus(imgLight, m_nLightCount, m_nType);

	return S_OK;
}

HRESULT CTrafficLightInfo::UpdateStatus2(HV_COMPONENT_IMAGE* pSceneImage)
{
	if( !pSceneImage ) return E_INVALIDARG;

	HiVideo::CRect rcLight = GetRect();
	HiVideo::CRect rcMax(0, 0, pSceneImage->iWidth, pSceneImage->iHeight);

	if(!m_rcLightInflated.IntersectRect(m_rcLightInflated, rcMax))
	{
		m_nLastStatus = TS_UNSURE;
		return S_FALSE;
	}

	CPersistentComponentImage imgLight;
	if(FAILED(imgLight.Create(HV_IMAGE_YUV_422, m_rcLightInflated.Width(), m_rcLightInflated.Height())))
	{
		return E_OUTOFMEMORY;
	}

	if (HV_IMAGE_BT1120_ROTATE_Y == pSceneImage->nImgType)
	{
		imgLight.CropAssign(*pSceneImage, m_rcLightInflated);	
	}
	else
	{
		HV_COMPONENT_IMAGE imgCrop;
		CropImage(*pSceneImage, m_rcLightInflated, &imgCrop);
		imgLight.Convert(imgCrop);
	}

	m_nLastStatus = GetLightStatus2(imgLight, rcLight);

	return S_OK;
}

HRESULT CTrafficLightInfo::SetRect(const HV_RECT& cRect, int nLightCount, int nType)
{
	m_rcLight = cRect;
	m_nLightCount = nLightCount;
	m_nType = nType;

	//扩大灯组范围
	int iInflateWidth = m_rcLight.Width() > m_rcLight.Height() ? m_rcLight.Height() : m_rcLight.Width();
	iInflateWidth = (int)(iInflateWidth * CTrackInfo::m_iAutoScanThread / 10.0);
	m_rcLightInflated = m_rcLight;
	m_rcLightInflated.InflateRect(iInflateWidth, iInflateWidth);

	return S_OK;
}

HRESULT CTrafficLightInfo::SetRect(int nLeft, int nTop, int nRight, int nBottom, int nLightCount, int nType)
{
	HV_RECT rcRect = { nLeft, nTop, nRight, nBottom };
	return SetRect(rcRect, nLightCount, nType);
}

#include "hv2ipl.h"

DWORD32 CTrafficLightInfo::GetLightStatus2( HV_COMPONENT_IMAGE imgLight, HiVideo::CRect rcLight )
{
	if(m_nType != 0 && m_nType != 1) return TS_UNSURE;

	//转为YUV_422
	CPersistentComponentImage imgTrans;
	if(imgLight.nImgType != HV_IMAGE_YUV_422)
	{
		imgTrans.Create(HV_IMAGE_YUV_422, imgLight.iWidth, imgLight.iHeight);
		imgTrans.Convert(imgLight);
		imgLight = imgTrans;
	}

	//转换成RGB
	CPersistentComponentImage imgR, imgG, imgB, imgLR, imgLG;
	imgR.Create(HV_IMAGE_GRAY, imgLight.iWidth, imgLight.iHeight);
	imgG.Create(HV_IMAGE_GRAY, imgLight.iWidth, imgLight.iHeight);
	imgB.Create(HV_IMAGE_GRAY, imgLight.iWidth, imgLight.iHeight);
	imgLR.Create(HV_IMAGE_GRAY, imgLight.iWidth, imgLight.iHeight);
	imgLG.Create(HV_IMAGE_GRAY, imgLight.iWidth, imgLight.iHeight);
	IMAGE_ConvertYCbCr2BGR(&imgLight, GetHvImageData(&imgR, 0), GetHvImageData(&imgG, 0), GetHvImageData(&imgB, 0), imgR.iStrideWidth[0]);


	//计算亮度值
	RESTRICT_PBYTE8 pRLine = GetHvImageData(&imgR, 0);
	RESTRICT_PBYTE8 pGLine = GetHvImageData(&imgG, 0);
	RESTRICT_PBYTE8 pBLine = GetHvImageData(&imgB, 0);
	RESTRICT_PBYTE8 pLRLine = GetHvImageData(&imgLR, 0);
	RESTRICT_PBYTE8 pLGLine = GetHvImageData(&imgLG, 0);
	for(int i = 0; i < imgR.iHeight; i++)
	{
		for(int j = 0; j < imgR.iWidth; j++)
		{
			pLRLine[j] = (BYTE8)(GetPixelRedValue(pRLine[j], pGLine[j], pBLine[j]));
			pLGLine[j] = (BYTE8)(GetPixelGreenValue(pRLine[j], pGLine[j], pBLine[j]));
		}
		pRLine += imgR.iStrideWidth[0];
		pGLine += imgG.iStrideWidth[0];
		pBLine += imgB.iStrideWidth[0];
		pLRLine += imgLR.iStrideWidth[0];
		pLGLine += imgLG.iStrideWidth[0];
	}

	//二值化
	BYTE8 bThreshold;
	BYTE8 bThresholdStat = 25 + CTrackInfo::m_iTrafficLightThread * 5;
	IMAGE_CalcBinaryThreshold(imgLR, 1, 1, &bThreshold);
	if (bThreshold < bThresholdStat) bThreshold = bThresholdStat;
	hvThreshold(&imgLR, &imgLR, bThreshold, 255, HV_THRESH_BINARY);

	IMAGE_CalcBinaryThreshold(imgLG, 1, 1, &bThreshold);
	if (bThreshold < bThresholdStat) bThreshold = bThresholdStat;
	hvThreshold(&imgLG, &imgLG, bThreshold, 255, HV_THRESH_BINARY);
	hvDilateNew(&imgLG, &imgG, m_pElement);
	hvErodeNew(&imgG, &imgLG, m_pElement);

	//HvImageDebugShow(&imgLR, cvSize(500, 200));

	//找到符合高宽的灯
	const int iMaxBox = 20;
	int cBox = 0;
	CRect rgrcBox[iMaxBox];
	bool rgfBoxsIsGreen[iMaxBox] = {false};
	int nRects;
	nRects = hvFindContoursImg(
		&imgLR,
		HV_RETR_EXTERNAL,
		HV_CHAIN_APPROX_SIMPLE
		);
	for (int i = 0; i < nRects; i++)
	{
		if (cBox == iMaxBox) break;
		rgrcBox[cBox] = g_rgContourRect[i];
		rgfBoxsIsGreen[cBox] = false;
		cBox++;
	}
	nRects = hvFindContoursImg(
		&imgLG,
		HV_RETR_EXTERNAL,
		HV_CHAIN_APPROX_SIMPLE
		);
	for (int i = 0; i < nRects; i++)
	{
		if (cBox == iMaxBox) break;
		rgrcBox[cBox] = g_rgContourRect[i];
		rgfBoxsIsGreen[cBox] = true;
		cBox++;
	}

	DWORD32 dwLightStatus = TS_UNSURE;
	int cLightItems = 0;
	CRect rgrcLightItems[CTrafficLightInfo::MAX_POS_COUNT] = {CRect(0, 0, 0, 0)};
	bool rgfLightItemsIsGreen[CTrafficLightInfo::MAX_POS_COUNT] = {false};

	if (m_nType == 0)
	{
		float fltLightWidth = (float)rcLight.Width() / m_nLightCount;
		float fltLightHeight = (float)rcLight.Height();
		for(int nIndex = 0; nIndex < cBox; nIndex++)
		{
			CRect rcTemp(rgrcBox[nIndex]);
			int iLightItemConnected(0);
			if (rcTemp.Height() > (fltLightHeight * 0.25f) &&
				rcTemp.Height() < (fltLightHeight * 1.5f))
			{
				//判断连在一起的灯
				if (rcTemp.Width() > (fltLightWidth * 0.5f) &&
					rcTemp.Width() < (fltLightWidth * 1.5f))
				{
					iLightItemConnected = 1;
				}
				else if (rcTemp.Width() > (fltLightWidth * 1.5f) &&
					rcTemp.Width() < (fltLightWidth * 2.5f))
				{
					iLightItemConnected = 2;
				}
				else if (rcTemp.Width() > (fltLightWidth * 2.5f) &&
					rcTemp.Width() < (fltLightWidth * 3.5f))
				{
					iLightItemConnected = 3;
				}
			}
			if (iLightItemConnected > 0)
			{
				//判断新的灯是否在一条线上
				if (cLightItems > 0 &&
					HV_ABS(rgrcLightItems[cLightItems - 1].CenterPoint().y - rcTemp.CenterPoint().y) > (fltLightHeight / 2))
				{
					if (HV_ABS(rgrcLightItems[cLightItems - 1].CenterPoint().y - (imgLight.iHeight / 2)) >
						HV_ABS(rcTemp.CenterPoint().y - (imgLight.iHeight / 2)))
					{
						cLightItems = 0;
					}
					else
					{
						iLightItemConnected = 0;
					}
				}
			}
			if (iLightItemConnected > 0 && (cLightItems + iLightItemConnected) < CTrafficLightInfo::MAX_POS_COUNT)
			{
				//按照从左到右的顺序排列灯
				int i;
				for (i = cLightItems; i > 0; i--)
				{
					if (rgrcLightItems[i - 1].CenterPoint().x > rcTemp.CenterPoint().x)
					{
						rgrcLightItems[i - 1 + iLightItemConnected] = rgrcLightItems[i - 1];
						rgfLightItemsIsGreen[i - 1 + iLightItemConnected] = rgfLightItemsIsGreen[1 - 1];
					}
					else
					{
						break;
					}
				}
				for (int j = 0; j < iLightItemConnected; j++)
				{
					CRect rcLightItem;
					rcLightItem.top = rcTemp.top;
					rcLightItem.bottom = rcTemp.bottom;
					rcLightItem.left = (int)(rcTemp.left + j * fltLightWidth + 0.5);
					rcLightItem.right = (int)(rcLightItem.left + fltLightWidth + 0.5);
					rgrcLightItems[i + j] = rcLightItem;
					rgfLightItemsIsGreen[i + j] = rgfBoxsIsGreen[nIndex];
				}
				cLightItems += iLightItemConnected;
			}
		}
		if (cLightItems > 0)
		{
			//初始化灯组位置之前需要定位
			if (!m_fInit)
			{
				if (m_rcLastLight.Width() == 0)
				{
					m_rcLastLight.left = rgrcLightItems[0].left;
					m_rcLastLight.right = rgrcLightItems[cLightItems - 1].right;
					m_rcLastLight.top = rgrcLightItems[0].top;
					m_rcLastLight.bottom = (int)(m_rcLastLight.top + fltLightHeight + 0.5);
				}
				else
				{
					if (HV_ABS(rgrcLightItems[0].CenterPoint().y - m_rcLastLight.CenterPoint().y) > (fltLightHeight * 0.5f))
					{
						m_rcLastLight = CRect(0, 0, 0, 0);
					}
					else
					{
						if (rgrcLightItems[0].left < m_rcLastLight.left)
							m_rcLastLight.left = rgrcLightItems[0].left;
						if (rgrcLightItems[cLightItems - 1].right > m_rcLastLight.right)
							m_rcLastLight.right = rgrcLightItems[cLightItems - 1].right;
						m_rcLastLight.top = rgrcLightItems[0].top;
						m_rcLastLight.bottom = (int)(m_rcLastLight.top + fltLightHeight + 0.5);
					}
				}
				if (m_rcLastLight.Width() > (fltLightWidth * m_nLightCount + fltLightWidth))
					m_rcLastLight = CRect(0, 0, 0, 0);
				else if (m_rcLastLight.Width() > (fltLightWidth * m_nLightCount - fltLightWidth / 2))
				{
					m_fInit = true;
					if (rgrcLightItems[0].left == m_rcLastLight.left)
						m_rcLastLight.right = m_rcLastLight.left + m_rcLight.Width();
					else
						m_rcLastLight.left = m_rcLastLight.right - m_rcLight.Width();
				}
			}
			else
			{
				float fltOffsetX = 0.0f, fltOffsetY = 0.0f, fltMinOffsetX = fltLightWidth / 2, fltMinOffsetY = fltLightHeight / 2;
				float fltLightPosX = m_rcLastLight.left + fltLightWidth / 2;
				int iLightIndex = 0;
				for (int i = 0; i < m_nLightCount; i++)
				{
					dwLightStatus <<= 4;
					if (iLightIndex < cLightItems)
					{
						fltOffsetX = rgrcLightItems[iLightIndex].CenterPoint().x - fltLightPosX;
						fltOffsetY = rgrcLightItems[iLightIndex].CenterPoint().y - (m_rcLastLight.top + fltLightHeight / 2);
						if (HV_ABS(fltOffsetX) < (fltLightWidth * 0.5f) &&
							HV_ABS(fltOffsetY) < (fltLightHeight * 0.5f))
						{
							if (rgfLightItemsIsGreen[iLightIndex])
							{
								dwLightStatus |= TS_GREEN;
							}
							else
							{
								dwLightStatus |= TS_RED;
							}
							iLightIndex++;
							if ((fltOffsetX * fltOffsetX + fltOffsetY * fltOffsetY) < (fltMinOffsetX * fltMinOffsetX + fltMinOffsetY * fltMinOffsetY))
							{
								fltMinOffsetX = fltOffsetX;
								fltMinOffsetY = fltOffsetY;
							}
						}
					}
					fltLightPosX += fltLightWidth;
				}
				if (HV_ABS(fltMinOffsetX) < (fltLightWidth / 2) &&
					HV_ABS(fltMinOffsetY) < (fltLightHeight / 2))
				{
					m_rcLastLight.OffsetRect((int)(fltMinOffsetX + 0.5), (int)(fltMinOffsetY + 0.5));
				}
			}
		}
	}
	else
	{
		float fltLightWidth = (float)rcLight.Width();
		float fltLightHeight = (float)rcLight.Height() / m_nLightCount;
		for(int nIndex = 0; nIndex < cBox; nIndex++)
		{
			CRect rcTemp(rgrcBox[nIndex]);
			int iLightItemConnected(0);
			if (rcTemp.Width() > (fltLightWidth * 0.25f) &&
				rcTemp.Width() < (fltLightWidth * 1.5f)	)
			{
				//判断连在一起的灯
				if (rcTemp.Height() > (fltLightHeight * 0.5f) &&
					rcTemp.Height() < (fltLightHeight * 1.5f))
				{
					iLightItemConnected = 1;
				}
				else if (rcTemp.Height() > (fltLightHeight * 1.5f) &&
					rcTemp.Height() < (fltLightHeight * 2.5f))
				{
					iLightItemConnected = 2;
				}
				else if (rcTemp.Height() > (fltLightHeight * 2.5f) &&
					rcTemp.Height() < (fltLightHeight * 3.5f))
				{
					iLightItemConnected = 3;
				}
			}
			if (iLightItemConnected > 0)
			{
				//判断新的灯是否在一条线上
				if (cLightItems > 0 &&
					HV_ABS(rgrcLightItems[cLightItems -1].CenterPoint().x - rcTemp.CenterPoint().x) > (fltLightWidth / 2))
				{
					if (HV_ABS(rgrcLightItems[cLightItems - 1].CenterPoint().x - (imgLight.iWidth / 2)) >
						HV_ABS(rcTemp.CenterPoint().x - (imgLight.iWidth / 2)))
					{
						cLightItems = 0;
					}
					else
					{
						iLightItemConnected = 0;
					}
				}
			}
			if (iLightItemConnected > 0 && (cLightItems + iLightItemConnected) < CTrafficLightInfo::MAX_POS_COUNT)
			{
				//按照从左到右的顺序排列灯
				int i = 0;
				if (cLightItems > 0)
				{
					for (i = cLightItems; i > 0; i--)
					{
						if (rgrcLightItems[i - 1].CenterPoint().y > rcTemp.CenterPoint().y)
						{
							rgrcLightItems[i - 1 + iLightItemConnected] = rgrcLightItems[i - 1];
							rgfLightItemsIsGreen[i - 1 + iLightItemConnected] = rgfLightItemsIsGreen[i - 1];
						}
						else
						{
							break;
						}
					}
				}
				for (int j = 0; j < iLightItemConnected; j++)
				{
					CRect rcLightItem;
					rcLightItem.top = (int)(rcTemp.top + j * fltLightHeight + 0.5);
					rcLightItem.bottom = (int)(rcLightItem.top + fltLightHeight + 0.5);
					rcLightItem.left = rcTemp.left;
					rcLightItem.right = rcTemp.right;
					rgrcLightItems[i + j] = rcLightItem;
					rgfLightItemsIsGreen[i + j] = rgfBoxsIsGreen[nIndex];
				}
				cLightItems += iLightItemConnected;
			}
		}
		if (cLightItems > 0)
		{
			//初始化灯组位置之前需要定位
			if (!m_fInit)
			{
				if (m_rcLastLight.Height() == 0)
				{
					m_rcLastLight.top = rgrcLightItems[0].top;
					m_rcLastLight.bottom = rgrcLightItems[cLightItems - 1].bottom;
					m_rcLastLight.left = rgrcLightItems[0].left;
					m_rcLastLight.right = (int)(m_rcLastLight.left + fltLightWidth + 0.5);
				}
				else
				{
					if (abs(rgrcLightItems[0].CenterPoint().x - m_rcLastLight.CenterPoint().x) > (fltLightWidth * 0.5f))
					{
						m_rcLastLight = CRect(0, 0, 0, 0);
					}
					else
					{
						if (rgrcLightItems[0].top < m_rcLastLight.top)
							m_rcLastLight.top = rgrcLightItems[0].top;
						if (rgrcLightItems[cLightItems - 1].bottom > m_rcLastLight.bottom)
							m_rcLastLight.bottom = rgrcLightItems[cLightItems - 1].bottom;
						m_rcLastLight.left = rgrcLightItems[0].left;
						m_rcLastLight.right = (int)(m_rcLastLight.left + fltLightWidth + 0.5);
					}
				}
				if (m_rcLastLight.Height() > (fltLightHeight * m_nLightCount + fltLightHeight))
					m_rcLastLight = CRect(0, 0, 0, 0);
				else if (m_rcLastLight.Height() > (fltLightHeight * m_nLightCount - fltLightHeight / 2))
				{
					m_fInit = true;
					if (rgrcLightItems[0].top == m_rcLastLight.top)
						m_rcLastLight.bottom = m_rcLastLight.top + m_rcLight.Height();
					else
						m_rcLastLight.top = m_rcLastLight.bottom - m_rcLight.Height();
				}
			}
			else
			{
				float fltOffsetX = 0, fltOffsetY = 0, fltMinOffsetX = fltLightWidth / 2, fltMinOffsetY = fltLightHeight / 2;
				float fltLightPosY = m_rcLastLight.top + fltLightHeight / 2;
				int iLightIndex = 0;
				for (int i = 0; i < m_nLightCount; i++)
				{
					dwLightStatus <<= 4;
					if (iLightIndex < cLightItems)
					{
						fltOffsetX = rgrcLightItems[iLightIndex].CenterPoint().x - (m_rcLastLight.left + fltLightWidth / 2);
						fltOffsetY = rgrcLightItems[iLightIndex].CenterPoint().y - fltLightPosY;
						if (HV_ABS(fltOffsetX) < (fltLightWidth * 0.5f) &&
							HV_ABS(fltOffsetY) < (fltLightHeight * 0.5f))
						{
							if (rgfLightItemsIsGreen[iLightIndex])
							{
								dwLightStatus |= TS_GREEN;
							}
							else
							{
								dwLightStatus |= TS_RED;
							}
							iLightIndex++;
							if ((fltOffsetX * fltOffsetX + fltOffsetY * fltOffsetY) < (fltMinOffsetX * fltMinOffsetX + fltMinOffsetY * fltMinOffsetY))
							{
								fltMinOffsetX = fltOffsetX;
								fltMinOffsetY = fltOffsetY;
							}
						}
					}
					fltLightPosY += fltLightHeight;
				}
				if (HV_ABS(fltMinOffsetX) < (fltLightWidth / 2) &&
					HV_ABS(fltMinOffsetY) < (fltLightHeight / 2))
				{
					m_rcLastLight.OffsetRect((int)(fltMinOffsetX + 0.5), (int)(fltMinOffsetY + 0.5));
				}
			}
		}
	}
	return dwLightStatus;
}

*/
int GetLineX(int iY, int iLineNum)
{
	int iX = -1;
	if( iLineNum < 0 || iLineNum >= CTrackInfo::m_iRoadNumber ) return iX;
	float fltX1 = (float)CTrackInfo::m_roadInfo[iLineNum].ptBottom.x;
	float fltY1 = (float)CTrackInfo::m_roadInfo[iLineNum].ptBottom.y;
	float fltX0 = (float)CTrackInfo::m_roadInfo[iLineNum].ptTop.x;
	float fltY0 = (float)CTrackInfo::m_roadInfo[iLineNum].ptTop.y;

	float fltDX1 = fltX1 - fltX0;
	float fltDY1 = fltY1 - fltY0;
	// zhaopy 会有误差
	if(fltDX1 == 0.0) fltDX1 = 1.0;
	if(fltDY1 == 0.0) fltDY1 = 1.0;

	float fltDY = (float)iY - fltY0;
	iX = (int)((fltDX1 / fltDY1) * fltDY + fltX0 + 0.5);
	return iX;
}

int LeftOrRight(int iX, int iY, int iLineNum, int* piDistance)
{
	int iRet = 0;
	if( iLineNum < 0 || iLineNum >= CTrackInfo::m_iRoadNumber ) return iRet;
	if( piDistance != NULL ) *piDistance = 0;

	int iTX = GetLineX(iY, iLineNum);

	if( iTX >= iX ) iRet = -1;
	else iRet = 1;

	int iA, iB, iC;
	if( piDistance != NULL )
	{
		int a, b, c, d;
		a = CTrackInfo::m_roadInfo[iLineNum].ptBottom.x;
		b = CTrackInfo::m_roadInfo[iLineNum].ptBottom.y;
		c = CTrackInfo::m_roadInfo[iLineNum].ptTop.x;
		d = CTrackInfo::m_roadInfo[iLineNum].ptTop.y;

		iA = d - b;
		iB = a - c;
		iC = c * b - a * d;

		if( (iA * iA + iB * iB) != 0 )
		{
			*piDistance = (int)(abs(iA * iX + iB * iY + iC) / sqrt((float)(iA * iA + iB * iB)));
		}
	}

	return iRet;
}


int MatchRoad(int iX, int iY, int *piRoadWidth)
{
	int iRet = -1;
	if( CTrackInfo::m_iRoadNumber <= 0 ) return iRet;

	int i = 0;
	for( i = 0; i < (CTrackInfo::m_iRoadNumber - 1) ; ++i )
	{
		if(CTrackInfo::m_iRoadNumberBegin == 0)
		{
			if( -1 == LeftOrRight(iX, iY, i) ) break;
		}
		else
		{
			if( 1 == LeftOrRight(iX, iY, i) ) break;
		}
	}

	//车道编号从0开始
	iRet = (i == 0)? 0 : i -1 ;

	//计算车道宽度
	if (piRoadWidth != NULL)
	{
		*piRoadWidth = GetLineX(iY, iRet + 1) - GetLineX(iY, iRet);
	}

	return iRet;
}


int GetAllPoints(HV_POINT* pAll, int iY, int *iPointCount)
{
    if(CTrackInfo::m_iRoadNumber<=0)return -1;
    int iMaxCount = (*iPointCount < CTrackInfo::m_iRoadNumber) ? *iPointCount : CTrackInfo::m_iRoadNumber;

    for (int i = 0; i < iMaxCount; i++)
    {
        pAll[i].x = GetLineX(iY, i);
        pAll[i].y = iY;
    }
    *iPointCount = iMaxCount;
    return 0;
}

BOOL IsOutSideRoad(int iX, int iY)
{
	if ( CTrackInfo::m_iRoadNumber <= 0 )
	{
		return false;
	}
    int iRet = 0;
    //if( iLineNum < 0 || iLineNum >= CTrackInfo::m_iRoadNumber ) return iRet;
    //if( piDistance != NULL ) *piDistance = 0;
    int iLineNum1 = 0;
    int iLineNum2 = CTrackInfo::m_iRoadNumber - 1;

    int iTX1 = GetLineX(iY, iLineNum1);
    int iTX2 = GetLineX(iY, iLineNum2);
    
    //同时大于或同时小于视为车道外物体
    if(((iTX1 > iX) && (iTX2 > iX)) || ((iTX1 < iX) && (iTX2 < iX)))
    {
        return true;
    } else {
        return false;
    }
}
