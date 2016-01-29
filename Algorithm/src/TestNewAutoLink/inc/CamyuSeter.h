#pragma once
#include "tracker.h"
#include "HvThreadBase.h"

class CCamyuSeter : public CHvThreadBase
{
public:
	CCamyuSeter(void);
	~CCamyuSeter(void);
// CHvThreadBase Interface
	virtual HRESULT Run(void* pvParam);
public:
	HRESULT SetParamOnScene(LIGHT_TYPE nLightType);
	bool ThreadIsOk();
private:
	LIGHT_TYPE m_nCurLightType;
	bool m_fNeedSetParam;
	HiVideo::ISemaphore* m_pSemNeedSet;
	DWORD32 m_dwLastThreadIsOkTime;
};
