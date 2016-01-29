#ifndef CAMNVCUDPTRANSMIT_H
#define CAMNVCUDPTRANSMIT_H
#include "CUdpTransmit.h"

class CCAMNVCUdpTransmit : public CUdpTransmit
{
public:
    CCAMNVCUdpTransmit();
    virtual ~CCAMNVCUdpTransmit();
    virtual BOOL IsTransmitting()
    {
        return CCAMNVCUdpTransmit::m_fIsTransmitting;
    }

    int Run();

protected:
    unsigned short m_wNVCDataPort;  // DataPort
    BOOL m_fIsTransmitting;

private:
    static int CallBack_StartupCAMNVC(void* pUserData);
    static int CallBack_RecvNVC(char* buf, int* pLen, void* pUserData);
    static int CallBack_GetTransmitDataNVC(char* buf, int* pLen, void* pUserData);
    static int CallBack_TransmitDataNVC(char* buf, int* pLen, void* pUserData);
    static int CallBack_ClientComin(void* pUserData)
    {
        CCAMNVCUdpTransmit *pThis = (CCAMNVCUdpTransmit*)pUserData;
        pThis->m_fIsTransmitting = TRUE;
        return 0;
    }
    static int CallBack_ClientExit(void* pUserData)
    {
        CCAMNVCUdpTransmit *pThis = (CCAMNVCUdpTransmit*)pUserData;
        pThis->m_fIsTransmitting = FALSE;
        return 0;
    }
};

#endif // CAMNVCUDPTRANSMIT_H
