#include "EventChecker.h"
#include "hv_opt.h"
#include "resultsend.h"
#include <math.h>

using namespace HiVideo;

#define min(a,b) (((a) < (b)) ? (a) : (b))

CEventChecker::CEventChecker()
{
    memset(m_szEventInfo, 0, sizeof(m_szEventInfo));
    memset(m_szEventInfoTmp, 0, sizeof(m_szEventInfoTmp));
    memset(m_rgEventInfoSum, 0, sizeof(m_rgEventInfoSum));
    m_dwLastOutputTime = GetSystemTick();
    memset(m_rgdwLastCarArriveTime, 0, sizeof(m_rgdwLastCarArriveTime));
    m_iCurTurnAroundBufIndex = 0;
    m_pResultSender = NULL;
    m_pTrackerCfgParam = NULL;
    m_pRsltSenderParam = NULL;
}

CEventChecker::~CEventChecker()
{

}

HRESULT CEventChecker::GetInfoInt(TiXmlElement* pDoc, char* pszKey, DWORD32* pdwDes)
{
    TiXmlElement* pNode = pDoc->FirstChildElement(pszKey);
    if (!pNode)
    {
        return E_FAIL;
    }

    (*pdwDes) = (DWORD32)atol(pNode->Attribute("value"));
    return S_OK;
}

HRESULT CEventChecker::GetInfoStr(TiXmlElement* pDoc, char* pszKey, char* pszDes)
{
    return E_NOTIMPL;
}

bool CEventChecker::IsTurnAround(CARLEFT_INFO_STRUCT *pCarLeftInfo)
{
    if (NULL == pCarLeftInfo)
    {
        return false;
    }
    bool fIsTurnArround = false;
    // 判断调头
    for (int i = 0; i < MAX_TURN_AROUND_BUF; i++)
    {
        // 未检测不判断掉头
        if (*(pCarLeftInfo->cCoreResult.rgbContent) != 0
                && memcmp(pCarLeftInfo->cCoreResult.rgbContent, m_rgTurnArround[i].rgbPlateNo, 7) == 0
                && pCarLeftInfo->cCoreResult.fReverseRun != m_rgTurnArround[i].fIsReverseRun
                && pCarLeftInfo->cCoreResult.nFirstFrameTime - m_rgTurnArround[i].dwTick < (DWORD32)TURN_AROUND_TIME)
        {
            fIsTurnArround = true;
            pCarLeftInfo->cCoreResult.rtType = RT_TURN;
        }
    }

    if (++m_iCurTurnAroundBufIndex >= MAX_TURN_AROUND_BUF)
    {
        m_iCurTurnAroundBufIndex = 0;
    }
    memcpy(m_rgTurnArround[m_iCurTurnAroundBufIndex].rgbPlateNo, pCarLeftInfo->cCoreResult.rgbContent, 7);
    m_rgTurnArround[m_iCurTurnAroundBufIndex].fIsReverseRun = pCarLeftInfo->cCoreResult.fReverseRun;
    m_rgTurnArround[m_iCurTurnAroundBufIndex].dwTick = pCarLeftInfo->cCoreResult.nFirstFrameTime;
    return fIsTurnArround;
}

HRESULT CEventChecker::GetOneEventInfo(
    CARLEFT_INFO_STRUCT * carLeft,
    TiXmlElement* pDoc
)
{
    if (NULL == pDoc)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;
    int iRoad;
    GetInfoInt(pDoc, "RoadNumber", (DWORD32*)&iRoad);
    if (iRoad >= 0 && iRoad < MAX_ROAD_NUM)
    {
        m_rgEventInfoSum[iRoad].dwCarCount++;
        DWORD32 dwTmp;
        if (GetInfoInt(pDoc, "VideoScaleSpeed", &dwTmp) == S_OK)
        {
            m_rgEventInfoSum[iRoad].dwCarSpeed += dwTmp;
            if (dwTmp != 0)
            {
                m_rgEventInfoSum[iRoad].dwOccupancy += (DWORD32)(RUN_DISTANCE / dwTmp);
            }
        }
        else
        {
            HV_Trace(3, "Get 车速 Failed!\n");
        }
        if ((dwTmp = carLeft->cCoreResult.nFirstFrameTime) > 0)
        {
            if (m_rgdwLastCarArriveTime[iRoad] != 0)
            {
                m_rgEventInfoSum[iRoad].dwCarDistance += (dwTmp - m_rgdwLastCarArriveTime[iRoad]);
            }
            m_rgdwLastCarArriveTime[iRoad] = dwTmp;
        }
        else
        {
            HV_Trace(3, "Get 车辆检测时间 Failed!\n");
        }
    }
    else
    {
        HV_Trace(3, "EventChecker road num invalid!!!\n");
    }

    return hr;
}

HRESULT CEventChecker::ProcessOneFrame(IReferenceComponentImage *pImage)
{
    if (!m_pRsltSenderParam || !m_pResultSender)
    {
        return E_FAIL;
    }

    if ((pImage->GetRefTime() - m_dwLastOutputTime)
            > (DWORD32)m_pRsltSenderParam->iCheckEventTime * 1000 * 60)
    {
        m_dwLastOutputTime = pImage->GetRefTime();
    }
    else
    {
        return S_FALSE;
    }

    memset(m_szEventInfo, 0, sizeof(m_szEventInfo));
    for (int i = 0; i < MAX_ROAD_NUM; i++)
    {
        if (m_rgEventInfoSum[i].dwCarCount > 0)
        {
            sprintf(
                m_szEventInfoTmp,
                "车道:%d\n流量统计:%d\n占有率统计:%.2f\n平均速度统计:%dkm/h\n车头时距计算:%d秒\n",
                i,
                m_rgEventInfoSum[i].dwCarCount,
                (float)((float)m_rgEventInfoSum[i].dwOccupancy * 60 / (RUN_DISTANCE * m_pRsltSenderParam->iCheckEventTime * 1000)),
                m_rgEventInfoSum[i].dwCarSpeed / m_rgEventInfoSum[i].dwCarCount,
                m_rgEventInfoSum[i].dwCarDistance / (m_rgEventInfoSum[i].dwCarCount * 1000)
            );
            strcat(m_szEventInfo, m_szEventInfoTmp);
        }
    }
    memset(m_rgEventInfoSum, 0, sizeof(m_rgEventInfoSum));

    DWORD32 dwTimeMsLow, dwTimeMsHigh;
    ConvertTickToSystemTime(pImage->GetRefTime(), dwTimeMsLow, dwTimeMsHigh);

    // TODO:需要进行保存操作
    /*if( g_pISafeSaver != NULL )
    {
    	WORD16 wStrLen = (WORD16)strlen( m_szEventInfo );
    	m_dectectData.pString->m_dwCarID = 0;
    	m_dectectData.pString->m_dwLen = 16 + wStrLen + 1;
    	m_dectectData.pString->m_wType = _TYPE_DETECT_STR;
    	m_dectectData.pString->m_wVideoID = 0;
    	m_dectectData.pString->m_dwTimeLow = dwTimeMsLow;
    	m_dectectData.pString->m_dwTimeHigh = dwTimeMsHigh;
    	strcpy( m_dectectData.pString->m_pStr, m_szEventInfo );
    	g_pISafeSaver->SaveDetectData(&m_dectectData);
    }*/
    HV_Trace(5, "<<<<\n%s>>>>\n", m_szEventInfo);
    return m_pResultSender->PutString(0, _TYPE_DETECT_STR, dwTimeMsLow, dwTimeMsHigh, m_szEventInfo);
}

CEventChecker_EP::CEventChecker_EP()
{
    memset(m_szEventInfo, 0, sizeof(m_szEventInfo));
    memset(m_szEventInfoTmp, 0, sizeof(m_szEventInfoTmp));
    memset(m_rgEventInfoSum, 0, sizeof(m_rgEventInfoSum));
    m_dwLastOutputTime = 0;
    memset(m_rgdwLastCarArriveTime, 0, sizeof(m_rgdwLastCarArriveTime));
    m_iCurTurnAroundBufIndex = 0;
    CreateSemaphore(&m_semLock, 1, 1);
}

CEventChecker_EP::~CEventChecker_EP()
{
    DestroySemaphore(&m_semLock);
}

HRESULT CEventChecker_EP::GetInfoInt(TiXmlElement* pDoc, char* pszKey, DWORD32* pdwDes)
{
    TiXmlElement* pNode = pDoc->FirstChildElement(pszKey);
    if (!pNode)
    {
        return E_FAIL;
    }

    (*pdwDes) = (DWORD32)atol(pNode->Attribute("value"));
    return S_OK;
}

HRESULT CEventChecker_EP::GetInfoStr(TiXmlElement* pDoc, char* pszKey, char* pszDes)
{
    return E_NOTIMPL;
}

bool CEventChecker_EP::IsTurnAround(CARLEFT_INFO_STRUCT *pCarLeftInfo)
{
    if (NULL == pCarLeftInfo)
    {
        return false;
    }
    bool fIsTurnArround = false;
    // 判断调头
    for (int i = 0; i < MAX_TURN_AROUND_BUF; i++)
    {
        // 未检测不判断掉头
        if (*(pCarLeftInfo->cCoreResult.rgbContent) != 0
                && memcmp(pCarLeftInfo->cCoreResult.rgbContent, m_rgTurnArround[i].rgbPlateNo, 7) == 0
                && pCarLeftInfo->cCoreResult.fReverseRun != m_rgTurnArround[i].fIsReverseRun
                && pCarLeftInfo->cCoreResult.nFirstFrameTime - m_rgTurnArround[i].dwTick < (DWORD32)TURN_AROUND_TIME)
        {
            fIsTurnArround = true;
        }
    }

    if (++m_iCurTurnAroundBufIndex >= MAX_TURN_AROUND_BUF)
    {
        m_iCurTurnAroundBufIndex = 0;
    }
    memcpy(m_rgTurnArround[m_iCurTurnAroundBufIndex].rgbPlateNo, pCarLeftInfo->cCoreResult.rgbContent, 7);
    m_rgTurnArround[m_iCurTurnAroundBufIndex].fIsReverseRun = pCarLeftInfo->cCoreResult.fReverseRun;
    m_rgTurnArround[m_iCurTurnAroundBufIndex].dwTick = pCarLeftInfo->cCoreResult.nFirstFrameTime;
    return fIsTurnArround;
}

HRESULT CEventChecker_EP::GetOneEventInfo(
    CARLEFT_INFO_STRUCT * carLeft,
    TiXmlElement* pDoc
)
{
    if (NULL == pDoc || NULL == m_pRsltSenderParam)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;
    int iRoad;
    GetInfoInt(pDoc, "RoadNumber", (DWORD32*)&iRoad);
    if (iRoad >= m_pTrackerCfgParam->iStartRoadNum && iRoad < MAX_ROAD_NUM + m_pTrackerCfgParam->iStartRoadNum)
    {
        //修正车道号
        iRoad -= m_pTrackerCfgParam->iStartRoadNum;

        m_rgEventInfoSum[iRoad].dwCarCount++;
        DWORD32 dwTmp;
        if (GetInfoInt(pDoc, "VideoScaleSpeed", &dwTmp) == S_OK)
        {
            m_rgEventInfoSum[iRoad].dwCarSpeed += dwTmp;
        }
        else
        {
            HV_Trace(3, "Get 车速 Failed!\n");
        }

        //计算车辆占有率
        DWORD32 now = 0;
        if (carLeft->cCoreResult.cResultImg.pimgBestSnapShot
                && now < carLeft->cCoreResult.cResultImg.pimgBestSnapShot->GetRefTime())
        {
            now = carLeft->cCoreResult.cResultImg.pimgBestSnapShot->GetRefTime();
        }

        if (carLeft->cCoreResult.cResultImg.pimgLastCapture
                && now < carLeft->cCoreResult.cResultImg.pimgLastCapture->GetRefTime())
        {
            now = carLeft->cCoreResult.cResultImg.pimgLastCapture->GetRefTime();
        }

        if (carLeft->cCoreResult.cResultImg.pimgLastSnapShot
                && now < carLeft->cCoreResult.cResultImg.pimgLastSnapShot->GetRefTime())
        {
            now = carLeft->cCoreResult.cResultImg.pimgLastSnapShot->GetRefTime();
        }
        //用出牌时间代替
        dwTmp = now;
        if (m_rgdwLastCarArriveTime[iRoad] != 0)
        {
            m_rgEventInfoSum[iRoad].dwCarDistance += (dwTmp - m_rgdwLastCarArriveTime[iRoad]);
        }
        m_rgdwLastCarArriveTime[iRoad] = dwTmp;

        //校正时间
        DWORD32 dwNow = m_dwLastOutputTime;
        if (dwNow < carLeft->cCoreResult.nFirstFrameTime)
        {
            dwNow = carLeft->cCoreResult.nFirstFrameTime;
        }
        if (now < carLeft->cCoreResult.nFirstFrameTime)
        {
            now = carLeft->cCoreResult.nFirstFrameTime;
        }
        if (now < dwNow)
        {
            now = dwNow;
        }

        m_rgEventInfoSum[iRoad].dwOccupancy += (now - dwNow);
        //限制死了，没有输出附加信息就不会有大车的占有率
        if (m_pRsltSenderParam->fOutputAppendInfo
                && carLeft->cCoreResult.nCarType == CT_LARGE
                && PLATE_DOUBLE_MOTO != carLeft->cCoreResult.nType)
        {
            m_rgEventInfoSum[iRoad].dwCamionOccupancy += (now - dwNow);
        }
        char szPlateName[32];
        GetPlateNameAlpha(szPlateName, (PLATE_TYPE)carLeft->cCoreResult.nType, (PLATE_COLOR)carLeft->cCoreResult.nColor, carLeft->cCoreResult.rgbContent);
        REAL_TIME_STRUCT rt1, rt2;
        DWORD32 dwLow, dwHigh;
        ConvertTickToSystemTime(now, dwLow, dwHigh);
        ConvertMsToTime(dwLow, dwHigh, &rt1);
        ConvertTickToSystemTime(dwNow, dwLow, dwHigh);
        ConvertMsToTime(dwLow, dwHigh, &rt2);
        HV_Trace(5,
                 "\n车道:%d "
                 "\n车牌号码:%s "
                 "\n检测时间:%04d-%02d-%02d %02d:%02d:%02d:%03d "
                 "\n离开时间:%04d-%02d-%02d %02d:%02d:%02d:%03d "
                 "\n该车辆占有时间:%d ms "
                 "\n当前同期周期的总所有车占有时间:%d ms "
                 "\n大车占有时间:%d ms",
                 iRoad + m_pTrackerCfgParam->iStartRoadNum,
                 szPlateName,
                 rt2.wYear, rt2.wMonth, rt2.wDay, rt2.wHour, rt2.wMinute, rt2.wSecond, rt2.wMSecond,
                 rt1.wYear, rt1.wMonth, rt1.wDay, rt1.wHour, rt1.wMinute, rt1.wSecond, rt1.wMSecond,
                 now - dwNow,
                 m_rgEventInfoSum[iRoad].dwOccupancy,
                 m_rgEventInfoSum[iRoad].dwCamionOccupancy);
    }
    else
    {
        HV_Trace(5, "EventChecker road num invalid!!!\n");
    }

    return hr;
}

HRESULT CEventChecker_EP::ProcessOneFrame(IReferenceComponentImage *pImage)
{
    if (!m_pRsltSenderParam || !m_pTrackerCfgParam || !m_pResultSender)
    {
        return E_FAIL;
    }

    if (!m_dwLastOutputTime)
    {
        m_dwLastOutputTime = pImage->GetRefTime();
        return S_OK;
    }

    HRESULT hr = S_OK;
    CHvString str;
    int totalTime = pImage->GetRefTime() - m_dwLastOutputTime;
    if (totalTime > (int)m_pRsltSenderParam->iCheckEventTime * 1000 * 60)
    {
        if (m_dwLastOutputTime)
        {
            DWORD32 dwTimeLow1, dwTimeHigh1,dwTimeLow2, dwTimeHigh2;
            ConvertTickToSystemTime(m_dwLastOutputTime, dwTimeLow1, dwTimeHigh1);
            ConvertTickToSystemTime(pImage->GetRefTime(), dwTimeLow2, dwTimeHigh2);

            REAL_TIME_STRUCT start, end;
            ConvertMsToTime(dwTimeLow1, dwTimeHigh1, &start);
            ConvertMsToTime(dwTimeLow2, dwTimeHigh2, &end);
            str.Format(
                "统计日期:%04d-%02d-%02d\n统计时间:%02d:%02d:%02d -- %02d:%02d:%02d\n",
                start.wYear, start.wMonth, start.wDay, start.wHour, start.wMinute, start.wSecond,
                end.wHour, end.wMinute, end.wSecond
            );
        }
        m_dwLastOutputTime = pImage->GetRefTime();
    }
    else
    {
        return S_FALSE;
    }

    SemPend(&m_semLock);
    memset(m_szEventInfo, 0, sizeof(m_szEventInfo));
    strcat(m_szEventInfo, str);
    bool m_bOK = false;
    //遍历所有配置的车道
    for (int i = 0; i < m_pTrackerCfgParam->nRoadLineNumber - 1; i++)
    {
        float per1 = (float)(m_rgEventInfoSum[i].dwOccupancy * 100.0/ totalTime);
        float per2 = (float)(m_rgEventInfoSum[i].dwCamionOccupancy * 100.0/ totalTime);
        int avg = 0;
        int speed = 0;
        if (m_rgEventInfoSum[i].dwCarCount > 0)
        {
            speed = m_rgEventInfoSum[i].dwCarSpeed / m_rgEventInfoSum[i].dwCarCount;
            avg = m_rgEventInfoSum[i].dwCarDistance / (m_rgEventInfoSum[i].dwCarCount * 1000);
        }
        HV_Trace(5, "index:%d\n"
                 "CarCount:%d\n"
                 "Occupancy:%d\n"
                 "CamionOccupancy:%d\n"
                 "CarSpeed:%d"
                 "CarDistance:%d\n",
                 i,
                 m_rgEventInfoSum[i].dwCarCount,
                 m_rgEventInfoSum[i].dwOccupancy,
                 m_rgEventInfoSum[i].dwCamionOccupancy,
                 m_rgEventInfoSum[i].dwCarSpeed,
                 m_rgEventInfoSum[i].dwCarDistance
                );
        sprintf(
            m_szEventInfoTmp,
            "车道:%d\n流量统计:%d\n占有率统计:%.2f%%\n大车占有率统计:%.2f%%\n平均速度统计:%dkm/h\n车头时距计算:%d秒\n",
            i + m_pTrackerCfgParam->iStartRoadNum,
            m_rgEventInfoSum[i].dwCarCount,
            min(per1, 100),
            min(per2, 100),
            speed,
            min(avg, m_pRsltSenderParam->iCheckEventTime * 60)
        );
        strcat(m_szEventInfo, m_szEventInfoTmp);
        m_bOK = true;
    }
    memset(m_rgEventInfoSum, 0, sizeof(m_rgEventInfoSum));
    if (!m_bOK)
    {
        strcat(m_szEventInfo, "车道:0\n流量统计:0\n占有率统计:0%\n大车占有率统计:0%\n平均速度统计:0km/h\n车头时距计算:0秒\n");
    }
    DWORD32 dwTimeMsLow, dwTimeMsHigh;
    ConvertTickToSystemTime(pImage->GetRefTime(), dwTimeMsLow, dwTimeMsHigh);

    // TODO:需要进行保存操作
    /*if( g_pISafeSaver != NULL )
    {
    	WORD16 wStrLen = (WORD16)strlen( m_szEventInfo );
    	m_dectectData.pString->m_dwCarID = 0;
    	m_dectectData.pString->m_dwLen = 16 + wStrLen + 1;
    	m_dectectData.pString->m_wType = _TYPE_DETECT_STR;
    	m_dectectData.pString->m_wVideoID = 0;
    	m_dectectData.pString->m_dwTimeLow = dwTimeMsLow;
    	m_dectectData.pString->m_dwTimeHigh = dwTimeMsHigh;
    	strcpy( m_dectectData.pString->m_pStr, m_szEventInfo );
    	g_pISafeSaver->SaveDetectData(&m_dectectData);
    }*/
    HV_Trace(5, "\n<<<<\n%s\n>>>>\n", m_szEventInfo);
    hr = m_pResultSender->PutString(0, _TYPE_DETECT_STR, dwTimeMsLow, dwTimeMsHigh, m_szEventInfo);
    SemPost(&m_semLock);
    return hr;
}

bool CEventChecker_EP::GetEventInfo(char * szInfo)
{
    szInfo[0] = '\0';

    return true;
}
