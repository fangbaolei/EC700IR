#ifndef _SWDOMECAMERASTANDBY_H_
#define _SWDOMECAMERASTANDBY_H_

#include "SWObject.h"
#include "SWDomeCameraBase.h"



class CSWDomeCameraStandby : public CSWDomeCameraBase
{
    CLASSINFO(CSWDomeCameraStandby, CSWDomeCameraBase)
public:
    CSWDomeCameraStandby();
    virtual ~CSWDomeCameraStandby();

	HRESULT Initialize(CSWDomeCameraSetting * pcSetting);
            
    HRESULT Set(const STANDBY_PARAM_STRUCT& sSetting);
    
    HRESULT Clear();
    
    HRESULT Call();

	HRESULT Stop();
    
    HRESULT Get(STANDBY_PARAM_STRUCT& sSetting);
	
	HRESULT GetStatus(INT& iStatus);

	HRESULT Update();

	HRESULT Pause();

	HRESULT Resume();

private:
	
	HRESULT SaveSetting();
	HRESULT CheckTime();
	static PVOID CheckTimeProxy(PVOID pvArg);
    
private:

	INT iTimer; //todo : need to implement the timer

	BOOL m_fCountingDown;
	BOOL m_fSleeping;
    BOOL m_fPaused;

	DWORD m_dwStartTick;

};

#endif // _SWDOMECAMERASTANDBY_H_

