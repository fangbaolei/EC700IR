#include "SWFC.h"
#include "SWEPoliceEventCheckerTransformFilter.h"
#include "SWCarLeft.h"

CSWEPoliceEventCheckerTransformFilter::CSWEPoliceEventCheckerTransformFilter()
	: CSWBaseFilter(1,1)
{
	GetIn(0)->AddObject(CLASSID(CSWCarLeft));	
	GetOut(0)->AddObject(CLASSID(CSWString));
	m_dwLastOutputTime = 0;
	swpa_memset(m_rgdwLastCarArriveTime, 0, sizeof(m_rgdwLastCarArriveTime));
	swpa_memset(m_rgEventInfoSum, 0, sizeof(m_rgEventInfoSum));
	m_dwTotalRoadNum = 0;
	m_dwStartRoadNum = 0;
	m_dwInterval = 300000;
	m_semEvent.Create(0, 1);
}

CSWEPoliceEventCheckerTransformFilter::~CSWEPoliceEventCheckerTransformFilter()
{
}

HRESULT CSWEPoliceEventCheckerTransformFilter::Initialize(DWORD dwInterval)
{
	if(dwInterval > 0)
	{
		m_dwInterval = dwInterval;
	}
	return S_OK;
}

HRESULT CSWEPoliceEventCheckerTransformFilter::Run()
{
	if(S_OK == CSWBaseFilter::Run())
	{
		return m_cThreadEvent.Start(OnEvent, this);
	}
	return E_FAIL;
}

HRESULT CSWEPoliceEventCheckerTransformFilter::Stop()
{
	m_semEvent.Post();
	m_cThreadEvent.Stop();
	return CSWBaseFilter::Stop();
}
	
HRESULT CSWEPoliceEventCheckerTransformFilter::Receive(CSWObject* obj)
{
	if(IsDecendant(CSWCarLeft, obj))
	{
		CSWCarLeft *pCarLeft = (CSWCarLeft *)obj;
		if (pCarLeft->GetRoadNo() >= 0 && pCarLeft->GetRoadNo() < MAX_ROAD_NUM)
		{	//保存车道总数和起始车道号
			m_dwTotalRoadNum = pCarLeft->GetTrackerParam()->nRoadLineNumber - 1;
			m_dwStartRoadNum = pCarLeft->GetTrackerParam()->iStartRoadNum;
			//过车总量
			m_rgEventInfoSum[pCarLeft->GetRoadNo()].dwCarCount++;
			//车辆总速度
			m_rgEventInfoSum[pCarLeft->GetRoadNo()].dwCarSpeed += pCarLeft->GetCarspeed();
			if(pCarLeft->GetCarspeed())
			{
				m_rgEventInfoSum[pCarLeft->GetRoadNo()].dwOccupancy += (DWORD32)(RUN_DISTANCE / pCarLeft->GetCarspeed());
			}
			//计算车辆占有率
      DWORD dwNow = 0;
      for(int i = 0; i < 5; i++)
      {
      	if (pCarLeft->GetImage(i) && dwNow < pCarLeft->GetImage(i)->GetRefTime())
      	{
          dwNow = pCarLeft->GetImage(i)->GetRefTime();
      	}
    	}
      if (m_rgdwLastCarArriveTime[pCarLeft->GetRoadNo()] != 0)
      {
          m_rgEventInfoSum[pCarLeft->GetRoadNo()].dwCarDistance += (dwNow - m_rgdwLastCarArriveTime[pCarLeft->GetRoadNo()]);
      }
      m_rgdwLastCarArriveTime[pCarLeft->GetRoadNo()] = dwNow;
      
      //校正时间
      DWORD32 dwTmp = m_dwLastOutputTime;
      if (dwTmp < pCarLeft->GetFirstFrameTime())
      {
          dwTmp = pCarLeft->GetFirstFrameTime();
      }
      if (dwNow < pCarLeft->GetFirstFrameTime())
      {
          dwNow = pCarLeft->GetFirstFrameTime();
      }
      if (dwNow < dwTmp)
      {
          dwNow = dwTmp;
      }
      m_rgEventInfoSum[pCarLeft->GetRoadNo()].dwOccupancy += (dwNow - dwTmp);      
      if (CT_LARGE == pCarLeft->GetCarType() && PLATE_DOUBLE_MOTO != pCarLeft->GetPlateType())
      {
          m_rgEventInfoSum[pCarLeft->GetRoadNo()].dwCamionOccupancy += (dwNow - dwTmp);
      }
        
      CSWDateTime dt1(dwNow), dt2(dwTmp);
      SW_TRACE_DEBUG(
      	"\n车道:%d "
      	"\n车牌号码:%s "
      	"\n检测时间:%04d-%02d-%02d %02d:%02d:%02d:%03d "
      	"\n离开时间:%04d-%02d-%02d %02d:%02d:%02d:%03d "
      	"\n该车辆占有时间:%d ms "
      	"\n当前同期周期的总所有车占有时间:%d ms "
      	"\n大车占有时间:%d ms",
      	pCarLeft->GetRoadNo(),
      	(LPCSTR)pCarLeft->GetPlateNo(),
      	dt2.GetYear(), dt2.GetMonth(), dt2.GetDay(), dt2.GetHour(), dt2.GetMinute(), dt2.GetSecond(), dt2.GetMSSecond(),
      	dt1.GetYear(), dt1.GetMonth(), dt1.GetDay(), dt1.GetHour(), dt1.GetMinute(), dt1.GetSecond(), dt1.GetMSSecond(),
      	dwNow - dwTmp,
      	m_rgEventInfoSum[pCarLeft->GetRoadNo()].dwOccupancy,
      	m_rgEventInfoSum[pCarLeft->GetRoadNo()].dwCamionOccupancy
      );
		}
	}
	return S_OK;
}

PVOID CSWEPoliceEventCheckerTransformFilter::OnEvent(PVOID pvParam)
{
	CSWEPoliceEventCheckerTransformFilter *pThis = (CSWEPoliceEventCheckerTransformFilter *)pvParam;
	while(S_OK == pThis->m_cThreadEvent.IsValid() && FILTER_RUNNING == pThis->GetState())
	{	//等待事件响应
		if(S_OK == pThis->m_semEvent.Pend(pThis->m_dwInterval))
		{
			break;
		}
		//发送交通信息采集
		if (pThis->m_dwLastOutputTime)
    {
    	CSWDateTime dt1(pThis->m_dwLastOutputTime);
    	CSWDateTime dt2;
    	CSWString strEvent;
    	//统计时间间隔
    	strEvent.Format("统计时间%04d-%02d-%02d %02d:%02d:%02d:%03d -- %04d-%02d-%02d %02d:%02d:%02d:%03d\n",
    		dt1.GetYear(), dt1.GetMonth(), dt1.GetDay(), dt1.GetHour(), dt1.GetMinute(), dt1.GetSecond(), dt1.GetMSSecond(),
      	dt2.GetYear(), dt2.GetMonth(), dt2.GetDay(), dt2.GetHour(), dt2.GetMinute(), dt2.GetSecond(), dt2.GetMSSecond()
    	);
    	DWORD dwTime = CSWDateTime::GetSystemTick() - pThis->m_dwLastOutputTime;
    	//统计每个车道的信息
    	for (int i = 0; i < pThis->m_dwTotalRoadNum; i++)
    	{
    	    FLOAT fltPer1 = (float)(pThis->m_rgEventInfoSum[i].dwOccupancy * 100.0/ dwTime);
    	    FLOAT fltPer2 = (float)(pThis->m_rgEventInfoSum[i].dwCamionOccupancy * 100.0/ dwTime);
    	    INT iAvg = 0;
    	    INT iSpeed = 0;
    	    if (pThis->m_rgEventInfoSum[i].dwCarCount > 0)
    	    {
    	        iSpeed = pThis->m_rgEventInfoSum[i].dwCarSpeed / pThis->m_rgEventInfoSum[i].dwCarCount;
    	        iAvg = pThis->m_rgEventInfoSum[i].dwCarDistance / (pThis->m_rgEventInfoSum[i].dwCarCount * 1000);
    	    }
    	    
    	    CSWString strTmp;
					strTmp.Format("车道:%d\n流量统计:%d\n占有率统计:%.2f%%\n大车占有率统计:%.2f%%\n平均速度统计:%dkm/h\n车头时距计算:%d秒\n",
    	        i + pThis->m_dwStartRoadNum,
    	        pThis->m_rgEventInfoSum[i].dwCarCount,
    	        swpa_min(fltPer1, 100),
    	        swpa_min(fltPer2, 100),
    	        iSpeed,
    	        swpa_min(iAvg, pThis->m_dwInterval)
    	    );
    	    strEvent += strTmp;
    	}
    	SW_TRACE_DEBUG("%s", (LPCSTR)strEvent);
    	CSWString *strTmp = new CSWString(strEvent);
    	pThis->GetOut(0)->Deliver(strTmp);
    	strTmp->Release();
    	swpa_memset(pThis->m_rgEventInfoSum, 0, sizeof(pThis->m_rgEventInfoSum));
    }
    pThis->m_dwLastOutputTime = CSWDateTime::GetSystemTick();
	}
	return 0;
}