#pragma once
#include "HvUtils.h"
#include "hvthread.h"
#include "swimageobj.h"
#include "hvvartype.h"

class CTestServer :
	public HiVideo::IRunable
{
public:
	CTestServer(void);
	virtual ~CTestServer(void);
	HRESULT Create();
	HRESULT Close();
	HRESULT SendVideo(IReferenceComponentImage *pRefImage);
	virtual HRESULT Run(void* pvParamter);
	virtual bool ThreadIsOk();
protected:
	CHvList<IReferenceComponentImage *,  10> m_lstJpg;
	HiVideo::IThread* m_pThread;
	bool m_fExit;
};
