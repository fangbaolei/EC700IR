#pragma once
#include "tracker.h"
#include "HvThreadBase.h"
#include "CamyuLink.h"


class CCamTrigger : public CHvThreadBase
{
public:
	CCamTrigger(void);
	~CCamTrigger(void);
	// CHvThreadBase Interface
	virtual HRESULT Run(void* pvParam);
public:
	HRESULT Trigger(LIGHT_TYPE nLightType, int iRoadNum = 0);
	HRESULT HardTrigger(int iRoadNum);
	HRESULT ProcessOnTrigger();
	bool ThreadIsOk();
private:
	LIGHT_TYPE m_nCurLightType;
	HiVideo::ISemaphore* m_pSemNeedTrigger;
	DWORD32 m_dwLastThreadIsOkTime;
	CCamyuLink m_cCamyuLink;
	int m_iCurRoadNum;
};
