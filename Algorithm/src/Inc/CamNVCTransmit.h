#ifndef CAMNVCTRANSMIT_H
#define CAMNVCTRANSMIT_H
#include "hvutils.h"
#include "CamTransmitBase.h"
#include "CAMNVCUdpTransmit.h"
#include "CAMNVCBroadcastTransmit.h"

#define PORT_CAM_NVC    69

class CCamNVCTransmit : public ICamTransmit
{
public:
    CCamNVCTransmit();
    virtual ~CCamNVCTransmit();

    virtual HRESULT Run(DWORD32 dwIPLAN2,
                DWORD32 dwMaskLAN2,
                DWORD32 dwIPLAN1,
                DWORD32 dwMaskLAN1,
                DWORD32 dwIPCam);
    virtual HRESULT Stop();
    virtual BOOL IsTransmitting()
    {
        if (m_pCamNVCDataTransmit)
        {
            return m_pCamNVCDataTransmit->IsTransmitting();
        }
        return FALSE;
    }

protected:

private:
    DWORD32 m_dwIPLAN1;
    DWORD32 m_dwIPLAN2;
    DWORD32 m_dwMaskLAN1;
    DWORD32 m_dwMaskLAN2;
    DWORD32 m_dwIPCam;

    CCAMNVCUdpTransmit* m_pCamNVCDataTransmit;
    CCAMNVCBroadcastTransmit* m_pCamNVCBroadcastTransmit;
};

#endif // CAMNVCTRANSMIT_H
