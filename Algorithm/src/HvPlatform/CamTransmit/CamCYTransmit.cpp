#include "CamCYTransmit.h"
#include "hvtarget_ARM.h"

CCamCYTransmit::CCamCYTransmit()
{
    m_pCamCYDataTransmit = NULL;
    m_pCamCYCmdTransmit = NULL;
    m_pCamCYCaptureTransmit = NULL;
    m_pCamCYBroadcastTransmit = NULL;
}

CCamCYTransmit::~CCamCYTransmit()
{
    Stop();
}

HRESULT CCamCYTransmit::Run(DWORD32 dwIPLAN2,
                            DWORD32 dwMaskLAN2,
                            DWORD32 dwIPLAN1,
                            DWORD32 dwMaskLAN1,
                            DWORD32 dwIPCam)
{
    HRESULT hRet = E_FAIL;

    if (m_pCamCYDataTransmit == NULL)
    {
        m_pCamCYDataTransmit = new CCAMCYTcpTransmit;
    }
    if (m_pCamCYCmdTransmit == NULL)
    {
        m_pCamCYCmdTransmit = new CCAMCYTcpTransmit;
    }
    if (m_pCamCYBroadcastTransmit == NULL)
    {
        m_pCamCYBroadcastTransmit = new CCAMCYBroadcastTransmit;
    }

    if (   (m_pCamCYDataTransmit == NULL)
            || (m_pCamCYCmdTransmit == NULL)
            || (m_pCamCYBroadcastTransmit == NULL) )
    {
        Stop();
        return hRet;
    }

    m_pCamCYCmdTransmit->SetListen(dwIPLAN2, PORT_CAM_CY_CMD);
    m_pCamCYCmdTransmit->SetTransmit(dwIPCam,
                                     PORT_CAM_CY_CMD,
                                     dwIPLAN1,
                                     PORT_CAM_CY_CMD_TRANSMIT);

    m_pCamCYDataTransmit->SetListen(dwIPLAN2, PORT_CAM_CY_DATA);
    m_pCamCYDataTransmit->SetTransmit(dwIPCam,
                                      PORT_CAM_CY_DATA,
                                      dwIPLAN1,
                                      PORT_CAM_CY_DATA_TRANSMIT);

    m_pCamCYBroadcastTransmit->SetListen(dwIPLAN2,
                                         dwMaskLAN2,
                                         PORT_CAM_CY_BROADCAST_SEND);
    m_pCamCYBroadcastTransmit->SetTransmit(dwIPLAN1,
                                           dwMaskLAN1,
                                           PORT_CAM_CY_BROADCAST_SEND);


    if (!m_pCamCYCmdTransmit->Run())
    {
        HV_Trace(5, "CamCY Cmd Transmit failed!");
    }
    else if (!m_pCamCYDataTransmit->Run())
    {
        HV_Trace(5, "CamCY Data Transmit failed!");
    }
    else if (!m_pCamCYBroadcastTransmit->Run())
    {
        HV_Trace(5, "CamCY Broadcast Transmit failed!");
    }
    else
    {
        hRet = S_OK;
    }

    if (hRet != S_OK)
    {
        Stop();
    }

    return hRet;
}

HRESULT CCamCYTransmit::Stop()
{
    if (m_pCamCYCmdTransmit != NULL)
    {
        m_pCamCYCmdTransmit->Stop();
    }

    if (m_pCamCYDataTransmit != NULL)
    {
        m_pCamCYDataTransmit->Stop();
    }
    if (m_pCamCYCaptureTransmit != NULL)
    {
        m_pCamCYCaptureTransmit->Stop();
    }

    if (m_pCamCYBroadcastTransmit != NULL)
    {
        m_pCamCYBroadcastTransmit->Stop();
    }

    return S_OK;
}


int CCamCYTransmit::CallBack_CmdConnComeIn(void* pUserData)
{
    CCamCYTransmit* pThis = (CCamCYTransmit*)pUserData;

    if (pThis->m_pCamCYCaptureTransmit == NULL)
    {
        pThis->m_pCamCYCaptureTransmit = new CCAMCYTcpTransmit;
        if (pThis->m_pCamCYCaptureTransmit == NULL)
        {
            return 0;
        }
        pThis->m_pCamCYCaptureTransmit->Stop();
    }

    unsigned int ulClientIP;
    pThis->m_pCamCYCmdTransmit->GetClientNetInfo(&ulClientIP, NULL);

    pThis->m_pCamCYCaptureTransmit->SetListen(pThis->m_dwIPLAN1,
            PORT_CAM_CY_CAPTURE);
    pThis->m_pCamCYCaptureTransmit->SetTransmit(ulClientIP,
            PORT_CAM_CY_CAPTURE,
            pThis->m_dwIPLAN2,
            PORT_CAM_CY_CAPTURE);

    if (!pThis->m_pCamCYCaptureTransmit->Run())
    {
        delete pThis->m_pCamCYCaptureTransmit;
        pThis->m_pCamCYCaptureTransmit = NULL;
        HV_Trace(5, "CamCY Capture Transmit failed!");
        return 0;
    }

    return 1;
}

int CCamCYTransmit::CallBack_CmdConnExit(void* pUserData)
{
    CCamCYTransmit* pThis = (CCamCYTransmit*)pUserData;
    if (pThis->m_pCamCYCaptureTransmit != NULL)
    {
        pThis->m_pCamCYCaptureTransmit->Stop();
        delete pThis->m_pCamCYCaptureTransmit;
        pThis->m_pCamCYCaptureTransmit = NULL;
    }
    return 1;
}
