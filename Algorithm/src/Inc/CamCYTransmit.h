#ifndef CAMCYTRANSMIT_H
#define CAMCYTRANSMIT_H

#define PORT_CAM_CY_CMD             8886
#define PORT_CAM_CY_DATA            8888
#define PORT_CAM_CY_CMD_TRANSMIT    18888
#define PORT_CAM_CY_DATA_TRANSMIT   18886
#define PORT_CAM_CY_CAPTURE         9999
#define PORT_CAM_CY_BROADCAST_SEND  5555
#define PORT_CAM_CY_BROADCAST_RECV  7777

#include "CamTransmitBase.h"
#include "CAMCYTcpTransmit.h"
#include "CAMCYBroadcastTransmit.h"

class CCamCYTransmit : public ICamTransmit
{
public:
    CCamCYTransmit();
    virtual ~CCamCYTransmit();

    virtual HRESULT Run(DWORD32 dwIPLAN2,
                DWORD32 dwMaskLAN2,
                DWORD32 dwIPLAN1,
                DWORD32 dwMaskLAN1,
                DWORD32 dwIPCam);
    virtual HRESULT Stop();
    virtual BOOL IsTransmitting()
    {
        if (m_pCamCYDataTransmit)
        {
            return m_pCamCYDataTransmit->IsTransmitting() || m_pCamCYCmdTransmit->IsTransmitting();
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

    CCAMCYTcpTransmit* m_pCamCYDataTransmit;
    CCAMCYTcpTransmit* m_pCamCYCmdTransmit;
    CCAMCYTcpTransmit* m_pCamCYCaptureTransmit;
    CCAMCYBroadcastTransmit* m_pCamCYBroadcastTransmit;

    static int CallBack_CmdConnComeIn(void* pUserData);
    static int CallBack_CmdConnExit(void* pUserData);
};

#endif // CAMCYTRANSMIT_H
