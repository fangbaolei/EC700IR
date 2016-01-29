#ifndef _SWDOMECAMERAFSCAN_H_
#define _SWDOMECAMERAFSCAN_H_
#include "SWObject.h"
#include "SWDomeCameraBase.h"
#include "SWDomeCameraPreset.h"


enum
{
    FSCAN_M_BEGIN = 0,
    
    FSCAN_M_START_PAN,
    FSCAN_M_STOP_PAN,
    FSCAN_M_SET_PAN_SPEED,
    FSCAN_M_START_TILT,
    FSCAN_M_STOP_TILT,
    FSCAN_M_SET_TILT_SPEED,
    FSCAN_M_START_PAN_TILT,
    FSCAN_M_STOP_PAN_TILT,
    FSCAN_M_START_ZOOM,
    FSCAN_M_STOP_ZOOM,
    FSCAN_M_SET_ZOOM_SPEED,
    
    FSCAN_M_SET_PAN_COORDINATE,
    FSCAN_M_SET_TILT_COORDINATE,
    FSCAN_M_SET_ZOOM,
    FSCAN_M_SET_IRIS,
    FSCAN_M_SET_FOCUS,
    FSCAN_M_SET_FOCUS_MODE,

	FSCAN_M_START_WIPER,
	FSCAN_M_STOP_WIPER,

	FSCAN_M_CALL_PRESET,
	
    FSCAN_M_NOP,
    
    FSCAN_M_END
};



class CSWDomeCameraFScan : public CSWDomeCameraBase
{
    CLASSINFO(CSWDomeCameraFScan, CSWDomeCameraBase)

public:
    CSWDomeCameraFScan();
    virtual ~CSWDomeCameraFScan();
    
    HRESULT AddMotion(const DWORD& dwPathID, const INT& iMove, const INT& iArg);
    
    HRESULT ClearPath(const DWORD& dwPathID);

	HRESULT Call(const DWORD& dwPathID);

	HRESULT Stop(const DWORD& dwPathID);

	HRESULT Pause(const DWORD& dwID);

	HRESULT Resume(const DWORD& dwID);
    
    HRESULT SavePath(const DWORD& dwPathID);

	HRESULT SetName(const DWORD& dwPathID, const CSWString& strName);
    
    HRESULT GetName(const DWORD& dwPathID, CSWString& strName);	
    
    HRESULT Walk(const DWORD& dwPathID, const DWORD& dwStep);

	HRESULT BreakWalk(const BOOL& fBreak) {m_fBreak = fBreak; return S_OK;};

	HRESULT Get(const DWORD& dwID, FSCAN_PARAM_STRUCT& sFScanParam );
 
	HRESULT Set(const DWORD& dwID, const FSCAN_PARAM_STRUCT& sFScanParam );

	HRESULT SetPresetInstance(CSWDomeCameraPreset* pPreset){m_pPreset = pPreset;};

	DWORD GetRunningID(){return m_dwScanningID;};

	BOOL IsIdle(){return m_fIsIdle;};

protected:

	HRESULT SetInitialState(const DWORD& dwID);
	HRESULT GotoInitialState(const DWORD& dwID);

	HRESULT SaveSetting(const DWORD& dwID);

	HRESULT FScan();
	static PVOID FScanProxy(PVOID pvArg);

	CSWDomeCameraPreset& GetPresetInstance(){return *m_pPreset;};

	BOOL WalkBroken(){return m_fBreak;};

private:

	DWORD m_dwScanningID;
	DWORD m_dwLastTick;
	BOOL  m_fIsIdle;
	BOOL  m_fBreak;

	CSWDomeCameraPreset* m_pPreset;
	
};

#endif // _SWDOMECAMERAFSCAN_H_

