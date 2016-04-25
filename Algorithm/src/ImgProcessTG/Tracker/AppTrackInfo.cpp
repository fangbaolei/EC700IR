#include "AppTrackInfo.h"
#include "AppUtils.h"

using namespace sv;

namespace swTgApp 
{

static bool IsOverLap(HV_RECT& rc0, HV_RECT& rc1, int p)
{
  int l = SV_MAX(rc0.left,rc1.left);
  int t = SV_MAX(rc0.top,rc1.top);
  int r = SV_MIN(rc0.right,rc1.right);
  int b = SV_MIN(rc0.bottom,rc1.bottom);

  if ( l>=r || t>=b)
  {
	  return false;
  }

  int nArea0 = (rc0.right - rc0.left + 1)*(rc0.bottom - rc0.top + 1);
  int nArea1 = (rc1.right - rc1.left + 1)*(rc1.bottom - rc1.top + 1);
  int nArea2 = (b-t+1) * (r-l+1);

  return (100 * nArea2) >= p * SV_MIN(nArea0,nArea1);
}
CAppTrackInfo::TRACK_STATE TrackState2InfoState(int nTrackState)
{
    using namespace svTgVvdApi;

    CAppTrackInfo::TRACK_STATE tsRet;

    switch(nTrackState)
    {
    case ITgTrack::TS_NEW:
        tsRet = CAppTrackInfo::TS_NEW;
        break;

    case ITgTrack::TS_TRACKING:
        tsRet = CAppTrackInfo::TS_TRACKING;
        break;

    case ITgTrack::TS_END:
        tsRet = CAppTrackInfo::TS_END;
        break;

    default:
        tsRet = CAppTrackInfo::TS_END;
    }

    return tsRet;
}

CAppTrackInfo::TRACK_TYPE TrackType2InfoType(svTgVvdApi::ITgTrack::TRACK_TYPE nTrackType)
{
    using namespace svTgVvdApi;

    CAppTrackInfo::TRACK_TYPE ttRet;

    // 更新物体类型
    switch(nTrackType)
    {
    case ITgTrack::TT_LARGE_CAR:
        ttRet = CAppTrackInfo::TT_LARGE_CAR;
        break;

    case ITgTrack::TT_MID_CAR:
        ttRet = CAppTrackInfo::TT_MID_CAR;
        break;

    case ITgTrack::TT_SMALL_CAR:
        ttRet = CAppTrackInfo::TT_SMALL_CAR;
        break;

    case ITgTrack::TT_BIKE:
        ttRet = CAppTrackInfo::TT_BIKE;
        break;

    case ITgTrack::TT_WALK_MAN:
        ttRet = CAppTrackInfo::TT_WAKL_MAN;
        break;

    default:
        ttRet = CAppTrackInfo::TT_UNKNOW;
    }

    return ttRet;
}

CAR_COLOR TgCarColor2CarColor(svTgVvdApi::CAR_COLOR nTgColor)
{
    CAR_COLOR color;

    switch(nTgColor)
    {
    case svTgVvdApi::CC_GREY:
        color = CC_GREY;
        break;

    case svTgVvdApi::CC_WHITE:
        color = CC_WHITE;
        break;

    case svTgVvdApi::CC_BLACK:
        color = CC_BLACK;
        break;

    case svTgVvdApi::CC_RED:
        color = CC_RED;
        break;

    case svTgVvdApi::CC_GREEN:
        color = CC_GREEN;
        break;

    case svTgVvdApi::CC_BLUE:
        color = CC_BLUE;
        break;

    case svTgVvdApi::CC_YELLOW:
        color = CC_YELLOW;
        break;

    default:
        color = CC_UNKNOWN;
    }

    return color;
}

CRoadInfo* CAppTrackInfo::m_pRoadInfo = NULL;
sv::SV_UINT32 CAppTrackInfo::s_iCurImageTick = 0;

CAppTrackInfo::CAppTrackInfo(void)
    :   m_pimgBestSnapShot(NULL)
    ,   m_pimgLastSnapShot(NULL)
{
    Free();
}


CAppTrackInfo::~CAppTrackInfo(void)
{
    Free();
}

unsigned int CAppTrackInfo::GetID()
{
    return m_dwID;
}

CAppTrackInfo::TRACK_STATE CAppTrackInfo::GetState()
{
    return m_nTrackState;
}

CAppTrackInfo::TRACK_TYPE CAppTrackInfo::GetType()
{
    return m_nTrackType;
}

int CAppTrackInfo::GetPosCount()
{
    return m_pTrack->GetLocusCount();
}

CSvRect CAppTrackInfo::GetPos(int nIndex)
{
    return m_pTrack->GetLocus(nIndex).m_rcPos;
}

CSvRect CAppTrackInfo::GetLastPos()
{
    return *(CSvRect*)&m_rcCurPos;
}

const svTgVvdApi::TG_TRACK_LOCUS& CAppTrackInfo::GetLocus(int nIndex)
{
    return m_pTrack->GetLocus(nIndex);
}

const svTgVvdApi::TG_TRACK_LOCUS& CAppTrackInfo::GetLastLocus()
{
    return m_pTrack->GetLocus(m_pTrack->GetLocusCount() - 1);
}

const svTgVvdApi::TG_PLATE_INFO& CAppTrackInfo::GetPlate(int nIndex)  // 取过程中识别到的车牌
{
    return m_pTrack->GetPlate(nIndex);
}

int CAppTrackInfo::GetPlateCount()
{
    return m_pTrack->GetPlateCount();
}

void CAppTrackInfo::End()
{
    // 注意：跟踪结束后的清空等操作应该在Free()中进行，请勿在此进行。
    // 不是每个跟踪结束都进入此函数
    m_pTrack->End();
    Update(m_pTrack); // End之后算法库会进行一些处理如投票等，需要重新Update
}

sv::SV_RESULT CAppTrackInfo::GetResult(svTgVvdApi::ITgTrack::TG_RESULT_INFO* pResInfo)
{
    return m_pTrack->GetResult(pResInfo);
}

sv::SV_RESULT CAppTrackInfo::GetExInfo(svTgVvdApi::TG_TRACK_EX_INFO* pExInfo)
{
    return m_pTrack->GetExInfo(pExInfo);
}


int CAppTrackInfo::GetRoadNum()
{
    svTgVvdApi::TG_TRACK_EX_INFO exInfo;
    m_pTrack->GetExInfo(&exInfo);

    return exInfo.nRoadNum;
}

// int CAppTrackInfo::GetPlateSimilarityCount()
// {
//     return m_cLastPlate.nSimilaryCount;
// }

void CAppTrackInfo::Free()
{
    m_pTrack = NULL;
    m_dwID = 0xFFFFFFFF;
    m_nTrackState = TS_FREE;

    ReleaseIReferenceComponentImage(m_pimgBestSnapShot);
    ReleaseIReferenceComponentImage(m_pimgLastSnapShot);

    m_nReverseRunCount = 0;

    for (int i=0; i<svTgVvdApi::PLATE_TYPE_COUNT; ++i)
    {
        m_rgBestPlateInfo[i].Clear();
    }

    m_nCarArriveTime = 0;

    m_fCanOutput = false;

    m_rcBestPlatePos.SetRect(-1, -1, -1, -1);
    m_rcLastPlatePos.SetRect(-1, -1, -1, -1);

    m_fCarArrived =  FALSE;
    m_fIsTrigger  =  FALSE;
    m_nCarArriveRealTime = 0;

	m_fCaptureFrameHavePlate = false;
	m_nBestPlateSimilar = 0;
	m_nLastPlateSimilar = 0;

    m_nLastPlateY = 0;  
    m_iTriggerRoadNum = -1;
    m_fLastFrameHasPlate = FALSE;
	
	m_dwTriggerCameraTimes = 0;

	m_nPlateMovePosCount = 0;
	memset(m_rgrcPlateMovePos, 0, sizeof(HV_RECT) * m_nMaxPlateMovePos);

	//m_nCenterPointMovePosCount = 0;
	//memset(m_CenterPointMovPos, 0, sizeof(HV_POINT) * m_nMaxCenterPointMovePos);
}

sv::SV_BOOL CAppTrackInfo::IsUpdated()
{
    return ((m_pTrack != NULL) && (m_pTrack->GetID() == m_dwID));
}

void CAppTrackInfo::SetRoadInfo(CRoadInfo* pRoadInfo)
{
    m_pRoadInfo = pRoadInfo;
}

inline int GetSVRectCenterY(sv::SV_RECT cRect)
{
    return cRect.m_nTop + (cRect.m_nBottom - cRect.m_nTop) >> 1;
}

/// 当前跟踪状态是否逆行
BOOL CAppTrackInfo::IsReverseRun()
{
	BOOL fReverseRun = FALSE;
    int nPosCount = m_pTrack->GetLocusCount();
	if (nPosCount > 3)
	{
		int iReverseCount = 0;
        int nPosY0 = GetSVRectCenterY(m_pTrack->GetLocus(0).m_rcPos);
		for (int i = 1; i < nPosCount; i++)
		{
            if (nPosY0 > GetSVRectCenterY(m_pTrack->GetLocus(i).m_rcPos))
			{
				iReverseCount += 2;
			}
			else
			{
				iReverseCount -= 2;
			}
		}
		if (iReverseCount > 0)
		{
			fReverseRun = TRUE;
		}
		m_nReverseRunCount = iReverseCount;
	}
    return fReverseRun;
}
sv::SV_BOOL CAppTrackInfo::IsLeftToRight(int iImgWidth)
{
	sv::SV_BOOL fFlag = FALSE;
	int iPosCount = GetPosCount();
	if (iPosCount > 3)
	{
		sv::CSvRect rcFirst = GetPos(0);
		sv::CSvRect rcLast = GetPos(iPosCount - 1);
		//if (HV_ABS(rcLast.CenterPoint().m_nX - rcFirst.CenterPoint().m_nX) > HV_ABS(rcLast.CenterPoint().m_nY - rcFirst.CenterPoint().m_nY) * 4)
		if (rcLast.CenterPoint().m_nX > rcFirst.CenterPoint().m_nX && (rcLast.CenterPoint().m_nX - rcFirst.CenterPoint().m_nX) > iImgWidth / 2)
		{
			fFlag = TRUE;
		}
	}
	return fFlag;

}

void CAppTrackInfo::Update(svTgVvdApi::ITgTrack* pITgTrack)
{
    m_pTrack = pITgTrack;
    // ID一定要拷贝下来，因为指针内容可能会变，下一次匹配还用指针取ID会不对
    m_dwID = pITgTrack->GetID();

    // 更新物体类型
    m_nTrackType = TrackType2InfoType(pITgTrack->GetType());

    // 更新跟踪状态
    m_nTrackState = TrackState2InfoState(pITgTrack->GetState());

    pITgTrack->GetExInfo(&m_cExInfo);
    m_nCarColor = TgCarColor2CarColor(m_cExInfo.nCarColor);

    m_rcCurPos = pITgTrack->GetLastPos();

    if (m_nTrackState == TS_END)
    {
        svTgVvdApi::ITgTrack::TG_RESULT_INFO tkRes;
        pITgTrack->GetResult(&tkRes);
        m_nCarColor = TgCarColor2CarColor(tkRes.nCarColor);
        m_nTrackType = TrackType2InfoType(tkRes.nTrackType);
    }
	
	// 无牌时会返回失败，但不重要
    const svTgVvdApi::TG_TRACK_LOCUS& cLastLocus = pITgTrack->GetLocus(pITgTrack->GetLocusCount() - 1);  // 最近的轨迹
    bool fGetPlate = cLastLocus.m_pPlateInfo != NULL;
    //svEPApi::TG_PLATE_INFO nCurPlate;
    //bool fHavePlate = pITgTrack->GetPlate(&nCurPlate);

    //nRecogInValidCount = m_cLastPlate.nRecogInValidCount;

	// 首次更新记录车牌位置
	if (fGetPlate)
	{
		HV_RECT rcCurPlate;
		rcCurPlate.left = cLastLocus.m_rcPos.m_nLeft;
		rcCurPlate.right = cLastLocus.m_rcPos.m_nRight;
		rcCurPlate.top = cLastLocus.m_rcPos.m_nTop;
		rcCurPlate.bottom = cLastLocus.m_rcPos.m_nBottom;
		if (m_nPlateMovePosCount == 0
			|| !IsOverLap(rcCurPlate, m_rgrcPlateMovePos[m_nPlateMovePosCount-1], 95)
			&& m_nPlateMovePosCount < m_nMaxPlateMovePos)
		{
			m_rgrcPlateMovePos[m_nPlateMovePosCount] = rcCurPlate;
			m_rgdwPlateTick[m_nPlateMovePosCount] = CAppTrackInfo::s_iCurImageTick;
			++m_nPlateMovePosCount;
		}
		m_fLastFrameHasPlate = TRUE;
		if (!m_fIsTrigger && m_pRoadInfo != NULL)
		{
			m_iTriggerRoadNum = m_pRoadInfo->GetRoadNum(m_rcCurPos.CenterPoint());
		}
	}
	else
	{
		m_fLastFrameHasPlate = FALSE;
		if (m_pRoadInfo->GetRoadLineCount() == 3)//只处理双车道
		{
			int iSecondLinex = m_pRoadInfo->GetRoadLineX(1,m_rcCurPos.CenterPoint().m_nY);
			if (m_rcCurPos.m_nLeft > iSecondLinex && m_rcCurPos.m_nRight > iSecondLinex)
			{
				m_iTriggerRoadNum = 1;
			}
			else if(m_rcCurPos.m_nLeft < iSecondLinex && m_rcCurPos.m_nRight < iSecondLinex)
			{
				m_iTriggerRoadNum = 0;
			}
			else
			{
				m_iTriggerRoadNum = -1;
			}
		}
	}

	/*const svTgVvdApi::TG_TRACK_LOCUS& cLastLocus = pITgTrack->GetLocus(pITgTrack->GetLocusCount() - 1);;  // 最近的轨迹
	 bool fGetPlate = cLastLocus.m_pPlateInfo != NULL;

	HV_RECT rcCurCenterPointPos={0};
	rcCurCenterPointPos.left=cLastLocus.m_rcPos.m_nLeft;
	rcCurCenterPointPos.right=cLastLocus.m_rcPos.m_nRight;
	rcCurCenterPointPos.top=cLastLocus.m_rcPos.m_nTop;
	rcCurCenterPointPos.bottom=cLastLocus.m_rcPos.m_nBottom;
	if (m_nCenterPointMovePosCount == 0 || !IsOverLap(rcCurCenterPointPos, m_CenterPointMovPos[m_nCenterPointMovePosCount-1], 95) && m_nCenterPointMovePosCount < m_nMaxCenterPointMovePos)
	{
		m_CenterPointMovPos[m_nCenterPointMovePosCount] = rcCurCenterPointPos;
		m_rgdwPlateTick[m_nCenterPointMovePosCount] = CAppTrackInfo::s_iCurImageTick;
		++m_nCenterPointMovePosCount;
	}*/

    //     // 无牌时会返回失败，但不重要
    //     bool fHavePlate = pITgTrack->GetPlate(&m_cLastPlate);
    //
    //     nRecogInValidCount = m_cLastPlate.nRecogInValidCount;
    //
    //     // 首次更新记录车牌位置
    //     if (fHavePlate && GetLastPlateDetPosCount() == GetPosCount())
    //     {
    //         SV_RECT rcCurPlate;
    //         rcCurPlate.left = m_cLastPlate.rcDetPos.m_nLeft;
    //         rcCurPlate.right = m_cLastPlate.rcDetPos.m_nRight;
    //         rcCurPlate.top = m_cLastPlate.rcDetPos.m_nTop;
    //         rcCurPlate.bottom = m_cLastPlate.rcDetPos.m_nBottom;
    //         if (m_nPlateMovePosCount == 0
    //             || !IsOverLap(rcCurPlate, m_rgrcPlateMovePos[m_nPlateMovePosCount-1], 95)
    //             && m_nPlateMovePosCount < m_nMaxPlateMovePos)
    //         {
    //             m_rgrcPlateMovePos[m_nPlateMovePosCount] = rcCurPlate;
    //             m_rgdwPlateTick[m_nPlateMovePosCount] = CAppTrackInfo::s_iCurImageTick;
    //             ++m_nPlateMovePosCount;
    //         }
    //     }
}


sv::SV_RESULT CAppTrackInfo::UpdateAll(
    CAppTrackInfo* rgDjObj,
    int iMaxObj,
    int* piObjCnt,  // 输入输出
    svTgVvdApi::ITgTrack* rgpTracker[],
    int iTrackerCnt
)
{
    // ITgTrack to CDjAppObj
    // 注意，ITgTrack为新时，CDjAppObj中的所有结果属性都不可靠（因为直接用ITgTrack指针），
    // 只有ID可靠，须最先用ID更新CDjAppObj中的ITgTrack指针
    svTgVvdApi::ITgTrack** rgResult = new svTgVvdApi::ITgTrack*[iTrackerCnt];

    if(rgResult == NULL)
    {
        return RS_E_OUTOFMEMORY;
    }

    for(int i = 0; i < iTrackerCnt; ++i)
    {
        rgResult[i] = rgpTracker[i];
    }

    int iObjCnt = *piObjCnt;

    //     //dbg
    //     printf("STEP1[OBJ] ");
    //     for (int i=0; i<iMaxObj; ++i)
    //     { if (!rgDjObj[i].IsGetResult())
    //     printf("R%d ", rgDjObj[i].GetID());
    //     else
    //         printf("%d ", rgDjObj[i].GetID());
    //     }printf("\n");
    //sv::utTrace("ITgTrack cnt:%d\n", iTrackerCnt);
    // 对每个Obj找对应的Result
    for(int i = 0; i < iObjCnt; ++i)
    {
        if(rgDjObj[i].GetState() == CAppTrackInfo::TS_END)  // 上次退出的，本次重置
        {
            rgDjObj[i].Free();
            continue;
        }

        int j = 0;

        for(; j < iTrackerCnt; ++j)
        {
            if(rgResult[j] != NULL
                    && rgDjObj[i].GetID() == rgResult[j]->GetID())
            {
                rgDjObj[i].Update(rgResult[j]);
#if RUN_PLATFORM == PLATFORM_WINDOWS

                //sv::utTrace("Tracker obj Update [%d]\n", rgDjObj[i].GetID());
                //                 if(rgDjObj[i].GetPosCount() == 1
                //                         && rgDjObj[i].GetState() == CAppTrackInfo::TS_END
                //                   )
                //                 {
                //                     printf("TrackInfo Update Err0 [%d], p%d s%d\n", rgDjObj[i].GetID(),
                //                            rgDjObj[i].GetPosCount(), rgDjObj[i].GetState());
                //                     system("pause");
                //                 }

#endif

                rgResult[j] = NULL; // 使用过的置空，为后面检查是否被使用过
                break;
            }
        }

        if(j == iTrackerCnt)    // 没匹配到
        {
            rgDjObj[i].Free();
        }
    }

    // 对每一个 rgDjObj 遍历，如果为空则找新的结果放入。同时将空的移出列队
    int iRemainResIndex = 0;
    int iEmptyObjIndex = -1;

    for(int i = 0; i < iMaxObj; ++i)
    {

        if(rgDjObj[i].IsUpdated()     // 取到结果
                || rgDjObj[i].GetState() == CAppTrackInfo::TS_END)      // 被标为退出
        {
            // Obj非空，将其移动到前面空的位置
            if(iEmptyObjIndex != -1)
            {
                // 交换时注意只能纯数据拷贝，用类直接赋值临时类会调用析构函数导致不正确
                char rgchbuf[sizeof(CAppTrackInfo)];
                memcpy(rgchbuf, &rgDjObj[iEmptyObjIndex], sizeof(CAppTrackInfo));
                memcpy(&rgDjObj[iEmptyObjIndex], &rgDjObj[i], sizeof(CAppTrackInfo));
                memcpy(&rgDjObj[i], rgchbuf, sizeof(CAppTrackInfo));
                ++iEmptyObjIndex;
                //printf("SWT %d-%d MPT+[%d]\n", iEmptyObjIndex, i, iEmptyObjIndex);
            }

            continue;
        }

        // Obj空闲可赋新值

        {
            int iObjIndex = (iEmptyObjIndex == -1) ? i : iEmptyObjIndex;

            if(rgDjObj[iObjIndex].m_pTrack != NULL)
            {
                // 异常情况
                rgDjObj[iObjIndex].Free();
            }
        }

        // 找未处理的结果
        for(; iRemainResIndex < iTrackerCnt; ++iRemainResIndex)
        {
            if(rgResult[iRemainResIndex] != NULL)
            {
                break;
            }
        }

        if(iRemainResIndex != iTrackerCnt)
        {
            // 找到新的
            CAppTrackInfo* pNewObj = NULL;

            if(iEmptyObjIndex != -1)
            {
                pNewObj = &rgDjObj[iEmptyObjIndex];
                ++iEmptyObjIndex;
            }
            else
            {
                pNewObj = &rgDjObj[i];
            }

            pNewObj->Update(rgResult[iRemainResIndex]);

#if SV_RUN_PLATFORM == SV_PLATFORM_WIN

            //sv::utTrace("Tracker obj new [%d]\n", pNewObj->GetID());
            if(rgResult[iRemainResIndex]->GetLocusCount() != 1
              )
            {
                printf("TrackInfo Update Err [%d], p%d s%d, 第一次检到应该轨迹数为1\n", rgResult[iRemainResIndex]->GetID(),
                       rgResult[iRemainResIndex]->GetLocusCount(), rgResult[iRemainResIndex]->GetState());
                system("pause");
            }

#endif
            //             if (pNewObj->GetState() == ITgTrack::TS_END)
            //             {
            //                 pNewObj->m_fExit = true;
            //                 sv::SV_Trace("EXIT[OBJ] %d\n", pNewObj->GetID());
            //             }
            rgResult[iRemainResIndex] = NULL;
        }
        else
        {
            // 已找完，没有了
            if(iEmptyObjIndex == -1)
            {
                iEmptyObjIndex = i;
                //printf("SET MPT [%d]\n",  iEmptyObjIndex);
            }

        }
    }

    iObjCnt = (iEmptyObjIndex != -1) ? iEmptyObjIndex : iMaxObj;

    *piObjCnt = iObjCnt;
    delete [] rgResult;

    return RS_S_OK;
}

}
