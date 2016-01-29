#ifndef _INCLUDE_IPTDEVICEIMPL_H_
#define _INCLUDE_IPTDEVICEIMPL_H_

#include "hvutils.h"
#include "IPTInterface.h"

class CIPTDeviceImpl : public IIPTDevice
{
public:
    CIPTDeviceImpl(IIPTControl *pIControl);
    ~CIPTDeviceImpl(void);

public:
    //IIPTDevice
    virtual HRESULT ReadEvent(
        PBYTE8 pbEventId,
        PBYTE8 pbType,
        PBYTE8 pbEventDeviceType,
        DWORD32* pdwTime,
        PBYTE8 pbData,
        PDWORD32 pdwSize
    );

    virtual HRESULT ReadEventEx(
        BYTE8* pbInData,
        DWORD32 dwInLen,
        BYTE8* pbEventId,
        BYTE8* pbType,PBYTE8 pbEventDeviceType,
        DWORD32* pdwTime,
        PBYTE8 pbData,
        PDWORD32 pdwSize
    );

    virtual HRESULT SyncTime( DWORD32 dwTime );
    virtual HRESULT SendSignal(int nIoID, int nLevel, int nSignalWidth);

private:
    IIPTControl* m_pIControl;

    static const int MAX_DATA_COUNT = 384;
    static const int FALG_OK = 0x00;
};

#endif
