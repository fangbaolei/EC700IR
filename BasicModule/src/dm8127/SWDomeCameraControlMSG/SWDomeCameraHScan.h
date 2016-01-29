#ifndef _SWDOMECAMERAHSCAN_H_
#define _SWDOMECAMERAHSCAN_H_
#include "SWObject.h"
#include "SWDomeCameraBase.h"

class CSWDomeCameraHScan : public CSWDomeCameraBase
{
    CLASSINFO(CSWDomeCameraHScan, CSWDomeCameraBase)
public:
    CSWDomeCameraHScan();
    virtual ~CSWDomeCameraHScan();
            
    HRESULT Set(const DWORD& dwPathID, const HSCAN_PARAM_STRUCT& sHScanParam );

	HRESULT Get(const DWORD& dwPathID, HSCAN_PARAM_STRUCT& sHScanParam );

	HRESULT SetLeftPos(const DWORD& dwPathID);

	HRESULT GetLeftPos(const DWORD& dwPathID, INT& iLeftPos);
    
    HRESULT SetRightPos(const DWORD& dwPathID);
    
    HRESULT GetRightPos(const DWORD& dwPathID, INT& iRightPos);
	
    HRESULT Clear(const DWORD& dwPathID);
    
    HRESULT Save(const DWORD& dwPathID);
    
    HRESULT Call(const DWORD& dwPathID);
	
	HRESULT Walk(const DWORD& dwPathID, const INT& iDirection);
    
    HRESULT Stop(const DWORD& dwPathID);

	HRESULT Pause(const DWORD& dwID);

	HRESULT Resume(const DWORD& dwID);

	DWORD GetRunningID(){return m_dwScanningID;};

protected:

	HRESULT SaveSetting(const DWORD& dwPathID);
    
private:

    DWORD m_dwScanningID;

	
    //CSWThread m_cThread;
};

#endif // _SWDOMECAMERAHSCAN_H_

