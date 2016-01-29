#ifndef _EVENT_CHECKER_H
#define _EVENT_CHECKER_H

#include "hvutils.h"
#include "swimageobj.h"
#include "trackerdef.h"
#include "safesaver.h"
#include "hvthread.h"
#include "ResultSenderImpl.h"
#include "PciResultSender.h"
#include "trackerdef.h"
#include "HvPciLinkApi.h"
#include "tinyxml.h"

const int MAX_ROAD_NUM = 10;
const int RUN_DISTANCE = 10000;
const int TURN_AROUND_TIME = 30000;
const int MAX_TURN_AROUND_BUF = 50;

typedef struct _TURNAROUND_BUF
{
	BYTE8 rgbPlateNo[7];
	bool fIsReverseRun;
	DWORD32 dwTick;

	_TURNAROUND_BUF()
	{
		memset(rgbPlateNo, 0 , sizeof(rgbPlateNo));
		fIsReverseRun = false;
		dwTick = 0;
	}
} TURNAROUND_BUF;

typedef struct _EVENT_INFO_SUM
{
	DWORD32 dwCarCount;			// 总数
	DWORD32 dwCarSpeed;			// 总速度
	DWORD32 dwOccupancy;		// 总占有率，平均占有率=(S/v1 + S/v2...)/(C*S)
	DWORD32 dwCamionOccupancy;	//卡车占有率,2011-02-25
	DWORD32 dwCarDistance;		// 总车头时距
	_EVENT_INFO_SUM()
	{
		dwCarCount = 0;
		dwCarSpeed = 0;
		dwOccupancy = 0;
		dwCamionOccupancy = 0;
		dwCarDistance = 0;
	}
} EVENT_INFO_SUM;

class CEventChecker
{
public:
	CEventChecker();
	virtual ~CEventChecker(void);
public:
	virtual HRESULT ProcessOneFrame(IReferenceComponentImage *pImage);
	virtual HRESULT GetOneEventInfo(
		CARLEFT_INFO_STRUCT * carLeft,
		TiXmlElement* pDoc
		);
	virtual bool IsTurnAround(CARLEFT_INFO_STRUCT *pCarLeftInfo);
	virtual HRESULT GetInfoInt(TiXmlElement* pDoc, char* pszKey, DWORD32* pdwDes);
	virtual HRESULT GetInfoStr(TiXmlElement* pDoc, char* pszKey, char* pszDes);
	virtual HRESULT SetResultParam(ResultSenderParam *pRsltSenderParam)
	{
	    m_pRsltSenderParam = pRsltSenderParam;
	    return S_OK;
	}
	virtual HRESULT SetTrackerParam(TRACKER_CFG_PARAM *pTrackerCfgParam)
	{
	    m_pTrackerCfgParam = pTrackerCfgParam;
	    return S_OK;
	}
	virtual HRESULT SetResultSender(IResultSender *pResultSender)
	{
	    m_pResultSender = pResultSender;
	    return S_OK;
	}

protected:
	char m_szEventInfo[16 * 1024];
	char m_szEventInfoTmp[4 * 1024];
	DWORD32 m_dwLastOutputTime;
	DWORD32 m_rgdwLastCarArriveTime[MAX_ROAD_NUM];
	EVENT_INFO_SUM m_rgEventInfoSum[MAX_ROAD_NUM];
	CDetectData m_dectectData;
	TURNAROUND_BUF m_rgTurnArround[MAX_TURN_AROUND_BUF];
	ResultSenderParam *m_pRsltSenderParam;
	TRACKER_CFG_PARAM *m_pTrackerCfgParam;
	IResultSender *m_pResultSender;

	int m_iCurTurnAroundBufIndex;
};

class CEventChecker_EP : public CEventChecker
{
public:
	CEventChecker_EP();
	virtual ~CEventChecker_EP(void);
public:
	virtual HRESULT ProcessOneFrame(IReferenceComponentImage *pImage);
	virtual HRESULT GetOneEventInfo(
		CARLEFT_INFO_STRUCT * carLeft,
		TiXmlElement* pDoc
		);
	virtual bool IsTurnAround(CARLEFT_INFO_STRUCT *pCarLeftInfo);
	virtual HRESULT GetInfoInt(TiXmlElement* pDoc, char* pszKey, DWORD32* pdwDes);
	virtual HRESULT GetInfoStr(TiXmlElement* pDoc, char* pszKey, char* pszDes);
	virtual bool GetEventInfo(char * szInfo);

private:
	HV_SEM_HANDLE m_semLock;
};

#endif
