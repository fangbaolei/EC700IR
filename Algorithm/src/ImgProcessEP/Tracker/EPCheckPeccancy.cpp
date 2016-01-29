#include "EPCheckPeccancy.h"
#include "hvbaseobj.h"
#include "RoadInfo.h"

using namespace HiVideo;

static inline HV_POINT _GetRectCenter(HV_RECT rc)
{
    HV_POINT pt;
    pt.x = (rc.left + rc.right) >> 1;
    pt.y = (rc.top + rc.bottom) >> 1;
    return pt;
}

bool CheckReverseRun(
    CEPTrackInfo* pEPObj
    )
{
    CEPTrackInfo& obj = *pEPObj;

    if(obj.GetPosCount() < 5)
    {
        return FALSE;
    }

    CRect rc0 = obj.GetPos(0);
    int nDistTH = rc0.Height() / 10;
    nDistTH = SV_MAX(nDistTH, 2);

    // 统计大于跳跃阈值的次数
    int nStraightCount = 0;
    int nReverseCount = 0;

    int nLastBottom = obj.GetPos(0).bottom;
    const int JUMP_COUNT = 3;
    int queueLastBottom[JUMP_COUNT] =
    {
        obj.GetPos(0).bottom,
        obj.GetPos(1).bottom,
        obj.GetPos(2).bottom
    };
    int nQueueHead = 0;

    for(int i = JUMP_COUNT; i < obj.GetPosCount(); ++i)
    {
        int nCurBottom = obj.GetPos(i).bottom;
        int nLastBottom = queueLastBottom[nQueueHead];

        int nDist = nCurBottom - nLastBottom;

        if(nDist < -nDistTH)
        {
            ++nStraightCount;
        }
        else if(nDist > nDistTH)
        {
            ++nReverseCount;
        }

        queueLastBottom[nQueueHead] = nCurBottom;
        nQueueHead = (nQueueHead + 1) % JUMP_COUNT;
    }

    if(nReverseCount > nStraightCount
        && obj.GetLastPos().bottom - rc0.bottom > rc0.Height() / 2)
    {
        return TRUE;
    }

    return FALSE;
}

void CheckRunType(
    CEPTrackInfo* pEPObj,
    CRoadInfo* pRoadInfo,
    svEPApi::EP_PARAM* pEPParam
    )
{
    HV_POINT cpBegin, cpEnd;
    cpBegin = _GetRectCenter(pEPObj->GetPos(0));
    cpEnd = _GetRectCenter(pEPObj->GetLastPos());

    RUN_TYPE rtRunType = RT_UNSURE;
    int iRoadBegin = pRoadInfo->GetRoadNum(cpBegin);
    int iRoadEnd = pRoadInfo->GetRoadNum(cpEnd);

    bool fIsOutLeft = false;
    bool fIsOutRight = false;
    bool fIsOutForward = false;

    // 没过停止线的不判断行驶类型。
    int iStopLineY = pEPParam->iStopLinePos * pRoadInfo->GetSceneHeight() / 100;
    // 第三条抓拍线为直行判断线。
    int iForwardPosY = pEPParam->rgiCapturePos[2] * pRoadInfo->GetSceneHeight() / 100;

    // 由于跟踪框到远处后容易变大有时会有点偏，为了避免误判增加一些修正再做判断。
    int iCarWidth = (pEPObj->GetPos(0).right - pEPObj->GetPos(0).left);
    int iLeftX = cpEnd.x + (iCarWidth / 4);
    int iRightX = cpEnd.x - (iCarWidth / 4);


    if(iLeftX < pRoadInfo->GetRoadLineX(0, cpEnd.y))
    {
        fIsOutLeft = true;
    }
    if(iRightX > pRoadInfo->GetRoadLineX(pRoadInfo->GetRoadLineCount()-1, cpEnd.y))
    {
        fIsOutRight = true;
    }
    if(cpEnd.y < iForwardPosY)
    {
        fIsOutForward = true;
    }

    if(fIsOutLeft)
    {
        rtRunType = RT_LEFT;
    }
    else if(fIsOutRight)
    {
        rtRunType = RT_RIGHT;
    }
    else if(fIsOutForward)
    {
        rtRunType = RT_FORWARD;
    }

    if(cpEnd.y > iStopLineY)
    {
        rtRunType = RT_UNSURE;
    }

    pEPObj->m_nRunType = rtRunType;
}

bool CheckCrossLine(
    CEPTrackInfo* pEPObj,
    CRoadInfo* pRoadInfo,
    svEPApi::EP_PARAM* pEPParam,
    int* pnRoadNum,
    ACTIONDETECT_PARAM_TRACKER* pParam
    )
{
    HV_POINT ptPos0C;
    HV_RECT rcPos0;

    if(pEPObj->GetPlateRecogCount() > 0)
    {
        svEPApi::EP_PLATE_INFO cPlateInfo;
        rcPos0 = pEPObj->m_rgrcPlateMovePos[0];
    }
    else
    {
        rcPos0 = pEPObj->GetPos(0);
    }

    ptPos0C.x = (rcPos0.right + rcPos0.left) / 2;
    ptPos0C.y = (rcPos0.bottom + rcPos0.top) / 2;

    //红绿灯模式只需要判断停止线前的车牌
    int iPosStop  = pEPParam->iStopLinePos * pRoadInfo->GetSceneHeight() / 100;

    int iPosCount = pEPObj->GetPosCount();
    HV_POINT ptPosStopLineC = ptPos0C;   // 停止线上跟踪位置的中点

    for(int i = 0; i < iPosCount; i++)
    {
        HV_RECT rcPos = pEPObj->GetPos(i);
        HV_POINT ptC = _GetRectCenter(rcPos);

        if(ptC.y > iPosStop && ptC.y < ptPosStopLineC.y)
        {
            ptPosStopLineC = ptC;
        }
    }

    int iRoad0, iRoad1;
    iRoad0 = pRoadInfo->GetRoadNum(ptPos0C);
    iRoad1 = pRoadInfo->GetRoadNum(ptPosStopLineC);
    if (pnRoadNum != NULL)
    {
        *pnRoadNum = iRoad1;
    }

    if( iRoad0 != iRoad1 && iRoad1 >= 0 && iRoad1 < 4 )
    {
    	return true;
    }
    else
    {
    	return false;
    }
}

bool CheckPressRoadLine(
    CEPTrackInfo* pEPObj,
    CRoadInfo* pRoadInfo,
    float fltOverLineSensitivity,
    int nStopLinePos,  // 绝对坐标
    int* pnRoadNum,
    ACTIONDETECT_PARAM_TRACKER* pParam
    )
{

//          if (0 == m_pParam->m_iDetectOverYellowLineEnable)
//          {
//              return COLT_INVALID;
//          }
    
         //CROSS_OVER_LINE_TYPE coltLines[5] = {COLT_LINE0, COLT_LINE1, COLT_LINE2, COLT_LINE3, COLT_LINE4};
    
	if( NULL == pParam )
	{
		 sv::utTrace("<123dspex>CheckPressRoadLine NULL == pParam.\n");
		return false;
	}
         int iCount[5] = {0, 0, 0, 0, 0};
         int nRoadLineCnt = pRoadInfo->GetRoadLineCount(); 
         for (int i = 0; i < nRoadLineCnt; i++)
         {


             for (int t = 0; t < pEPObj->m_nPlateMovePosCount; t++)
             {
                 //求车牌宽度的一半
                 int iHalfWidth = (pEPObj->m_rgrcPlateMovePos[t].right - pEPObj->m_rgrcPlateMovePos[t].left) / 2;
                 // 根据灵敏度修正
                 iHalfWidth = (int)(iHalfWidth * fltOverLineSensitivity);
                 // 根据车型，向两边括相应大小
                 int nCarType = pEPObj->GetType();
                 switch (nCarType)
                 {
                 case CEPTrackInfo::TT_SMALL_CAR:
                     break;
                 case CEPTrackInfo::TT_MID_CAR:
                     iHalfWidth = (int)(iHalfWidth * 1.2f);
                     break;
                 case CEPTrackInfo::TT_LARGE_CAR:
                     iHalfWidth = (int)(iHalfWidth * 1.5f);
                     break;
                 }
                 //求中心点
                 int iCenterX = (pEPObj->m_rgrcPlateMovePos[t].right + pEPObj->m_rgrcPlateMovePos[t].left) / 2;
                 int iCenterY = (pEPObj->m_rgrcPlateMovePos[t].bottom + pEPObj->m_rgrcPlateMovePos[t].top) / 2;
                 //红绿灯模式只需要判断停止线前的车牌
                 if (iCenterY < nStopLinePos)
                 {
                     continue;
                 }
                 //求黄线对应Y上的X
                 int iXCenter = pRoadInfo->GetRoadLineX(i, iCenterY);
                 if (SV_ABS(iCenterX - iXCenter) < iHalfWidth)
                 {
                     iCount[i]++;
                 }
             }
         }
    
         int iMax = 0;
         int iIndex(0);
         for (int i = 0; i < 5; i++)
         {
             if (iCount[i] > iMax)
             {
                 iMax = iCount[i];
                 iIndex = i;
             }
         }
         if (0 >= iMax)
         {
        	 sv::utTrace("<123dspex>CheckPressRoadLine 0 >= iMax.\n");
             return false;
         }
         else
         {
        	 sv::utTrace("<123dspex>CheckPressRoadLine 0 < iMax. index:%d\n", iIndex);
             *pnRoadNum = iIndex;
             return true;
         }
}
// 
// bool CheckStop()
// {
//     return false;  //TODO
// 
// }
// 
// bool CheckCrossRoadLine()
// {
//     return false;  //TODO
//     //     if (0 == m_pParam->m_iDetectCrossLineEnable)
//     //     {
//     //         return COLT_INVALID;
//     //     }
//     //     if (CTrackInfo::m_iRoadNumber <= 2)
//     //     {
//     //         return COLT_NO;
//     //     }
//     //     CROSS_OVER_LINE_TYPE coltLines[4] = {COLT_NO, COLT_LINE1, COLT_LINE2, COLT_LINE3};
//     //
//     //     int iCenterX0, iCenterX1, iCenterY0, iCenterY1;
//     //     iCenterX0 = (TrackInfo.m_rgPlateInfo[0].rcPos.m_nRight + TrackInfo.m_rgPlateInfo[0].rcPos.m_nLeft) / 2;
//     //     iCenterY0 = (TrackInfo.m_rgPlateInfo[0].rcPos.m_nBottom + TrackInfo.m_rgPlateInfo[0].rcPos.m_nTop) / 2;
//     //
//     //     //红绿灯模式只需要判断停止线前的车牌
//     //     if (m_pParam->m_fUsedTrafficLight)
//     //     {
//     //         int iPosStop  = CTrackInfo::m_iStopLinePos * m_nHeight / 100;
//     //         iCenterX1 = iCenterX0;
//     //         iCenterY1 = iCenterY0;
//     //         for (int i = 0; i < TrackInfo.m_cPlateInfo; i++)
//     //         {
//     //             if( TrackInfo.m_rgPlateInfo[i].nInfoType != PI_LPR )
//     //             {
//     //                 continue;
//     //             }
//     //             int iTmpX = (TrackInfo.m_rgPlateInfo[i].rcPos.m_nRight + TrackInfo.m_rgPlateInfo[i].rcPos.m_nLeft) / 2;
//     //             int iTmpY = (TrackInfo.m_rgPlateInfo[i].rcPos.m_nBottom + TrackInfo.m_rgPlateInfo[i].rcPos.m_nTop) / 2;
//     //             if (iTmpY > iPosStop && iTmpY < iCenterY1)
//     //             {
//     //                 iCenterX1 = iTmpX;
//     //                 iCenterY1 = iTmpY;
//     //             }
//     //         }
//     //     }
//     //     else
//     //     {
//     //         iCenterX1 = (TrackInfo.LastInfo().rcPos.m_nRight + TrackInfo.LastInfo().rcPos.m_nLeft) / 2;
//     //         iCenterY1 = (TrackInfo.LastInfo().rcPos.m_nBottom + TrackInfo.LastInfo().rcPos.m_nTop) / 2;
//     //     }
//     //
//     //     int iRoad0, iRoad1;
//     //     iRoad0 = MatchRoad(iCenterX0, iCenterY0);
//     //     iRoad1 = MatchRoad(iCenterX1, iCenterY1);
//     //
//     //     if (iRoad0 == iRoad1)
//     //     {
//     //         return COLT_NO;
//     //     }
//     //     if (iRoad0 > iRoad1)
//     //     {
//     //         int iTmp = iRoad1;
//     //         iRoad1 = iRoad0;
//     //         iRoad0 = iTmp;
//     //     }
//     //
//     //     int index = iRoad1;
//     //     if( index >= 0 && index < 4 && CTrackInfo::m_ActionDetectParam.iIsCrossLine[index] == 1)
//     //     {
//     //         return coltLines[index];
//     //     }
//     //
//     //     return COLT_NO;
// }
