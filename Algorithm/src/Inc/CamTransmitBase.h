#ifndef _CAM_TRANSMIT_BASE_H_
#define _CAM_TRANSMIT_BASE_H_

#include <swbasetype.h>

class ICamTransmit
{
public:
    virtual ~ICamTransmit(){};
    virtual HRESULT Run(DWORD32 dwIPLAN2,
                DWORD32 dwMaskLAN2,
                DWORD32 dwIPLAN1,
                DWORD32 dwMaskLAN1,
                DWORD32 dwIPCam) = 0;
    virtual HRESULT Stop() = 0;
    virtual BOOL IsTransmitting() = 0;
};

#endif

