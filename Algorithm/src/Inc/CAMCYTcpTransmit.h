#ifndef CAMCYTCPTRANSMIT_H
#define CAMCYTCPTRANSMIT_H
#include "CamyuLinkOpt.h"
#include "CTcpTransmit.h"

class CCAMCYTcpTransmit : public CTcpTransmit
{
public:
    CCAMCYTcpTransmit();
    virtual ~CCAMCYTcpTransmit();

    int Run();
    int GetClientNetInfo(unsigned int* pulIP, unsigned short* pwPort);
    BOOL IsTransmitting()
    {
        return m_fIsTransmitting;
    }

protected:

private:
    // 包含IP的命令数据起始位置，用于替换IP
    int m_nCAMCYExpDataStart;

    int m_nCAMCYImgDataBytes;
    int m_nCAMCYImgDataLen;
    CY_FRAME_HEADER m_sFrameHead;

    void HandleCAMCYData(char* pBuf, int iDataLen);
    void HandleCAMCYImgData(char* pBuf, int iDataLen);
    int ReplaceImgIP(char* pBuf, int iLen);

    BOOL m_fIsTransmitting;

    static int CallBack_StartupCAMCY(void* pUserData);
    static int CallBack_GetTransmitDataCAMCY(char* pBuf, int* pLen, void* pUserData);
    static int CallBack_SendCAMCY(char* pBuf, int* pLen, void* pUserData);
    static int CallBack_ClientComin(void* pUserData)
    {
        CCAMCYTcpTransmit *pThis = (CCAMCYTcpTransmit*)pUserData;
        pThis->m_fIsTransmitting = TRUE;
        return 0;
    }
    static int CallBack_ClientExit(void* pUserData)
    {
        CCAMCYTcpTransmit *pThis = (CCAMCYTcpTransmit*)pUserData;
        pThis->m_fIsTransmitting = FALSE;
        return 0;
    }
};

#endif // CCAMCYTCPTRANSMIT_H
