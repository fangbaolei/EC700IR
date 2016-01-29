#ifndef __SW_EPOLICE_EVENT_CHECKER_TRANSFORM_FILTER_H__
#define __SW_EPOLICE_EVENT_CHECKER_TRANSFORM_FILTER_H__
#include "SWBaseFilter.h"

#define MAX_ROAD_NUM        10
#define RUN_DISTANCE        10000
#define TURN_AROUND_TIME    30000
#define MAX_TURN_AROUND_BUF 50

class CSWEPoliceEventCheckerTransformFilter : public CSWBaseFilter
{
	CLASSINFO(CSWEPoliceEventCheckerTransformFilter, CSWBaseFilter)
	typedef struct _EVENT_INFO_SUM
	{
		DWORD dwCarCount;					// 总数
		DWORD dwCarSpeed;					// 总速度
		DWORD dwOccupancy;				// 总占有率，平均占有率=(S/v1 + S/v2...)/(C*S)
		DWORD dwCamionOccupancy;	//卡车占有率,2011-02-25
		DWORD dwCarDistance;			// 总车头时距
		_EVENT_INFO_SUM()
		{
			dwCarCount = 0;
			dwCarSpeed = 0;
			dwOccupancy = 0;
			dwCamionOccupancy = 0;
			dwCarDistance = 0;
		}
	} EVENT_INFO_SUM;
public:
	         CSWEPoliceEventCheckerTransformFilter();
	virtual ~CSWEPoliceEventCheckerTransformFilter();
protected:
	virtual HRESULT Initialize(DWORD dwInterval);
	virtual HRESULT Run();
	virtual HRESULT Stop();
	virtual HRESULT Receive(CSWObject* obj);	
protected:
	static PVOID OnEvent(PVOID pvParam);	
protected:
	//自动化映射宏
	SW_BEGIN_DISP_MAP(CSWEPoliceEventCheckerTransformFilter, CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 1)
	SW_END_DISP_MAP()
private:
	CSWThread m_cThreadEvent;
	CSWSemaphore m_semEvent;
	
	DWORD m_dwTotalRoadNum;
	DWORD m_dwStartRoadNum;
	DWORD m_dwInterval;
	DWORD m_dwLastOutputTime;
	DWORD m_rgdwLastCarArriveTime[MAX_ROAD_NUM];
	EVENT_INFO_SUM m_rgEventInfoSum[MAX_ROAD_NUM];
};
REGISTER_CLASS(CSWEPoliceEventCheckerTransformFilter)
#endif
