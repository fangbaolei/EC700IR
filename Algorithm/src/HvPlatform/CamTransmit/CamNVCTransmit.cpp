#include "CamNVCTransmit.h"

CCamNVCTransmit::CCamNVCTransmit()
{
    m_pCamNVCDataTransmit = NULL;
    m_pCamNVCBroadcastTransmit = NULL;
}

CCamNVCTransmit::~CCamNVCTransmit()
{
    Stop();
}

HRESULT CCamNVCTransmit::Run(DWORD32 dwIPLAN2,
                             DWORD32 dwMaskLAN2,
                             DWORD32 dwIPLAN1,
                             DWORD32 dwMaskLAN1,
                             DWORD32 dwIPCam)
{
    HRESULT hRet = E_FAIL;
    if (m_pCamNVCDataTransmit == NULL)
    {
        m_pCamNVCDataTransmit = new CCAMNVCUdpTransmit;
    }
    if (m_pCamNVCBroadcastTransmit == NULL)
    {
        m_pCamNVCBroadcastTransmit = new CCAMNVCBroadcastTransmit;
    }

    if ( (m_pCamNVCDataTransmit == NULL)
            || (m_pCamNVCBroadcastTransmit == NULL) )
    {
        Stop();
        return hRet;
    }

    m_pCamNVCDataTransmit->SetListen(dwIPLAN2, PORT_CAM_NVC);
    m_pCamNVCDataTransmit->SetTransmit(dwIPCam,
                                       PORT_CAM_NVC,
                                       dwIPLAN1);


    m_pCamNVCBroadcastTransmit->SetListen(dwIPLAN2,
                                          dwMaskLAN2,
                                          PORT_CAM_NVC);
    m_pCamNVCBroadcastTransmit->SetTransmit(dwIPLAN1,
                                            dwMaskLAN1,
                                            PORT_CAM_NVC);

    if ( !m_pCamNVCDataTransmit->Run() )
    {
        HV_Trace(5, "CamNVC Data Transmit failed!");
    }
    else if ( !m_pCamNVCBroadcastTransmit->Run() )
    {
        HV_Trace(5, "CamNVC Broadcast Transmit failed!");
    }
    else
    {
        HV_Trace(5, "CamNVC Transmit Run OK!");
        hRet = S_OK;
    }

    if (hRet != S_OK)
    {
        Stop();
    }

    return hRet;
}

HRESULT CCamNVCTransmit::Stop()
{
    if (m_pCamNVCDataTransmit != NULL)
    {
        m_pCamNVCDataTransmit->Stop();
        delete m_pCamNVCDataTransmit;
        m_pCamNVCDataTransmit = NULL;
    }

    if (m_pCamNVCBroadcastTransmit != NULL)
    {
        m_pCamNVCBroadcastTransmit->Stop();
        delete m_pCamNVCBroadcastTransmit;
        m_pCamNVCBroadcastTransmit = NULL;
    }

    return S_OK;
}
