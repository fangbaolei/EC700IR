#ifndef _SWDOMECAMERAWATCHKEEPING_H_
#define _SWDOMECAMERAWATCHKEEPING_H_

#include "SWObject.h"
#include "SWDomeCameraBase.h"
#include "SWDomeCameraPreset.h"
#include "SWDomeCameraHScan.h"
#include "SWDomeCameraFScan.h"
#include "SWDomeCameraCruise.h"

class CSWDomeCameraWatchKeeping : public CSWDomeCameraBase
{
    CLASSINFO(CSWDomeCameraWatchKeeping, CSWDomeCameraBase)
public:
    CSWDomeCameraWatchKeeping();
    virtual ~CSWDomeCameraWatchKeeping();
	
	HRESULT Initialize(CSWDomeCameraSetting * pcSetting);

    HRESULT Set(const WATCHKEEPING_PARAM_STRUCT& sSetting);
    
    HRESULT Clear();
    
    HRESULT Call();

	HRESULT Stop();
    
    HRESULT Get(WATCHKEEPING_PARAM_STRUCT& sSetting);

	HRESULT Update();

	HRESULT Pause();

	HRESULT Resume();

	HRESULT GetStatus(INT& iStatus);

	HRESULT SetPresetInstance(CSWDomeCameraPreset* pPreset){m_pPreset = pPreset;};

	HRESULT SetHScanInstance(CSWDomeCameraHScan* pHScan){m_pHScan = pHScan;};

	HRESULT SetFScanInstance(CSWDomeCameraFScan* pFScan){m_pFScan = pFScan;};

	HRESULT SetCruiseInstance(CSWDomeCameraCruise* pCruise){m_pCruise = pCruise;};

private:
	
	HRESULT SaveSetting();

	HRESULT CheckTime();
	static PVOID CheckTimeProxy(PVOID pvArg);

protected:

	CSWDomeCameraPreset& GetPresetInstance(){return *m_pPreset;};
	CSWDomeCameraHScan& GetHScanInstance(){return *m_pHScan;};
	CSWDomeCameraFScan& GetFScanInstance(){return *m_pFScan;};
	CSWDomeCameraCruise& GetCruiseInstance(){return *m_pCruise;};
    
private:

	BOOL m_fCountingDown;
	BOOL m_fWatching;
    BOOL m_fPaused;

	DWORD m_dwStartTick;

	CSWDomeCameraPreset* m_pPreset;
	CSWDomeCameraHScan* m_pHScan;
	CSWDomeCameraFScan* m_pFScan;
	CSWDomeCameraCruise* m_pCruise;
};

#endif // _SWDOMECAMERAWATCHKEEPING_H_

