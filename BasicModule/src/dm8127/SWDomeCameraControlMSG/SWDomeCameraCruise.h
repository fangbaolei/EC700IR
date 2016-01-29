#ifndef _SWDOMECAMERACRUISE_H_
#define _SWDOMECAMERACRUISE_H_
#include "SWObject.h"
#include "SWDomeCameraBase.h"
#include "SWDomeCameraPreset.h"
#include "SWDomeCameraHScan.h"
#include "SWDomeCameraFScan.h"



class CSWDomeCameraCruise : public CSWDomeCameraBase
{
    CLASSINFO(CSWDomeCameraCruise, CSWDomeCameraBase)
	friend class CSWDomeCameraControlMSG;
public:

    CSWDomeCameraCruise();
    virtual ~CSWDomeCameraCruise();
            
    HRESULT Set(const DWORD& dwID, const CRUISE_PARAM_STRUCT& sParam);
    
    HRESULT Clear(const DWORD& dwID);
    
    HRESULT Call(const DWORD& dwID);
    
    HRESULT Get(const DWORD& dwID, CRUISE_PARAM_STRUCT& sParam);

	HRESULT Stop(const DWORD& dwID);

	HRESULT Pause(const DWORD& dwID);

	HRESULT Resume(const DWORD& dwID);

	DWORD GetRunningID(){return m_dwCruiseID;};
	
	HRESULT SetPresetInstance(CSWDomeCameraPreset* pPreset){m_pPreset = pPreset;};

	HRESULT SetHScanInstance(CSWDomeCameraHScan* pHScan){m_pHScan = pHScan;};

	HRESULT SetFScanInstance(CSWDomeCameraFScan* pFScan){m_pFScan = pFScan;};


protected:

	
	HRESULT SaveSetting(const DWORD& dwID);

	CSWDomeCameraPreset& GetPresetInstance(){return *m_pPreset;};
	CSWDomeCameraHScan& GetHScanInstance(){return *m_pHScan;};
	CSWDomeCameraFScan& GetFScanInstance(){return *m_pFScan;};


	HRESULT Cruise();

	static PVOID CruiseProxy(PVOID pvArg);


private:

    //CSWThread m_cThread;
	DWORD m_dwCruiseID;
	BOOL m_fPaused;

	CSWDomeCameraPreset* m_pPreset;
	CSWDomeCameraHScan* m_pHScan;
	CSWDomeCameraFScan* m_pFScan;

};

#endif // _SWDOMECAMERACRUISE_H_

