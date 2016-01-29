#ifndef _SWDOMECAMERATIMER_H_
#define _SWDOMECAMERATIMER_H_

#include "SWObject.h"
#include "SWDomeCameraBase.h"
#include "SWDomeCameraPreset.h"
#include "SWDomeCameraHScan.h"
#include "SWDomeCameraFScan.h"
#include "SWDomeCameraCruise.h"



class CSWDomeCameraTimer : public CSWDomeCameraBase
{
    CLASSINFO(CSWDomeCameraTimer, CSWDomeCameraBase)
public:
    CSWDomeCameraTimer();
    virtual ~CSWDomeCameraTimer();

	HRESULT Initialize(CSWDomeCameraSetting * pcSetting);
	
    HRESULT Set(const DWORD& dwID, const TIMER_PARAM_STRUCT& sSetting);
    
    HRESULT Clear(const DWORD& dwID);
    
    HRESULT Call(const DWORD& dwID);

	HRESULT Stop(const DWORD& dwID);
    
    HRESULT Get(const DWORD& dwPathID, TIMER_PARAM_STRUCT& sSetting);

	HRESULT SetPresetInstance(CSWDomeCameraPreset* pPreset){m_pPreset = pPreset;};

	HRESULT SetHScanInstance(CSWDomeCameraHScan* pHScan){m_pHScan = pHScan;};

	HRESULT SetFScanInstance(CSWDomeCameraFScan* pFScan){m_pFScan = pFScan;};

	HRESULT SetCruiseInstance(CSWDomeCameraCruise* pCruise){m_pCruise = pCruise;};


protected:

	HRESULT StartTimer();
	HRESULT StopTimer();
	
	HRESULT StartMotion(const DWORD& dwMotionType, const DWORD& dwMotionID);
	HRESULT StopMotion(const DWORD& dwMotionType, const DWORD& dwMotionID);

	CSWDomeCameraPreset& GetPresetInstance(){return *m_pPreset;};
	CSWDomeCameraHScan& GetHScanInstance(){return *m_pHScan;};
	CSWDomeCameraFScan& GetFScanInstance(){return *m_pFScan;};
	CSWDomeCameraCruise& GetCruiseInstance(){return *m_pCruise;};
    


private:

	HRESULT CheckTime();
	static PVOID CheckTimeProxy(PVOID pvArg);
	
	HRESULT SaveSetting(const DWORD& dwID);
	
private:

	BOOL m_fWorking;

	
	CSWDomeCameraPreset* m_pPreset;
	CSWDomeCameraHScan* m_pHScan;
	CSWDomeCameraFScan* m_pFScan;
	CSWDomeCameraCruise* m_pCruise;
};

#endif // _SWDOMECAMERATIMER_H_

