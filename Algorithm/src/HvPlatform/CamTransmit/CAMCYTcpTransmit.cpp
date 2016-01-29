#include "CAMCYTcpTransmit.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "hvutils.h"
#include "NetTransmitUtils.h"

#define PORT_CAM_CY_CMD             8886
#define PORT_CAM_CY_DATA            8888
#define PORT_CAM_CY_CAPTURE         9999

CCAMCYTcpTransmit::CCAMCYTcpTransmit()
{
    m_fIsTransmitting = FALSE;
}

CCAMCYTcpTransmit::~CCAMCYTcpTransmit()
{
}

int CCAMCYTcpTransmit::Run()
{
    m_nCAMCYImgDataBytes = 0;
    m_nCAMCYExpDataStart = 0;
    m_fIsTransmitting = FALSE;

    if (InitListen() == 0)
    {
        return 0;
    }

    SetClientDefSockBufLen(8*1024, 32*1024);
    SetTransmitDefSockBufLen(32*1024, 8*1024);

    m_cTransmit.SetStartupCallback(CallBack_StartupCAMCY, this);
    m_cTransmit.SetRecvCallback(CallBack_Recv, this);
    m_cTransmit.SetSendCallback(CallBack_SendCAMCY, this);
    m_cTransmit.SetTransmitDataCallback(CallBack_TransmitData, this);
    m_cTransmit.SetClientCominDataCallback(CallBack_ClientComin, this);
    m_cTransmit.SetClientExitDataCallback(CallBack_ClientExit, this);
    m_cTransmit.SetGetTransmitDataCallback(CallBack_GetTransmitDataCAMCY, this);

    return m_cTransmit.Run();
}

int CCAMCYTcpTransmit::GetClientNetInfo(unsigned int* pulIP, unsigned short* pwPort)
{
    int iRet = 0;
    if (m_fdSockListenClient != -1)
    {
        sockaddr_in addr;
        socklen_t len;
        getpeername(m_fdSockListenClient, (sockaddr*)&addr, &len);

        if (pulIP != NULL)
        {
            *pulIP = ntohl(addr.sin_addr.s_addr);
        }
        if (pwPort != NULL)
        {
            *pwPort = ntohs(addr.sin_port);
        }

        iRet = 1;
    }
    return iRet;
}

void CCAMCYTcpTransmit::HandleCAMCYData(char* pBuf, int iDataLen)
{
    if (iDataLen == 0)
    {
        return ;
    }

    if (m_nCAMCYExpDataStart == 0)
    {
        if ((0x31 == pBuf[0]) && (0x30 == pBuf[1]))
        {
            unsigned long ulIP = htonl(m_ulListenIP);

            if (iDataLen >= 0x484)
            {
                memcpy(pBuf+0x480, &ulIP, 4);
            }
            else
            {
                if (iDataLen > 0x480)
                {
                    memcpy(pBuf+0x480, &ulIP, iDataLen - 0x480);
                }

                m_nCAMCYExpDataStart = iDataLen;
            }
        }
    }
    else
    {
        unsigned long ulIP = htonl(m_ulListenIP);

        if (m_nCAMCYExpDataStart < 0x480)
        {
            int nWlen = m_nCAMCYExpDataStart + iDataLen - 0x480;

            if (nWlen > 0)
            {
                memcpy(pBuf+0x480-m_nCAMCYExpDataStart, &ulIP, (nWlen > 4) ? 4 : nWlen);
                if (nWlen >= 4)
                {
                    m_nCAMCYExpDataStart = 0;
                }
                else
                {
                    m_nCAMCYExpDataStart += iDataLen;
                }
            }
            else if (nWlen <= 0)
            {
                m_nCAMCYExpDataStart += iDataLen;
            }
        }
        else
        {
            int nWriten = m_nCAMCYExpDataStart - 0x480;
            int nWlen = (iDataLen > (4-nWriten)) ? (4-nWriten) : iDataLen;

            char* pIp = (char*)&ulIP + nWriten;
            memcpy(pBuf, pIp, nWlen);

            if (m_nCAMCYExpDataStart+nWlen >= 0x484)
            {
                m_nCAMCYExpDataStart = 0;
            }
            else
            {
                m_nCAMCYExpDataStart += nWlen;
            }
        }
    }
}

int CCAMCYTcpTransmit::CallBack_SendCAMCY(char* pBuf, int* pLen, void* pUserData)
{
    if (*pLen == 0)
        return 1;

    CCAMCYTcpTransmit* pThis = (CCAMCYTcpTransmit*)pUserData;

    // 数据端口的数据在发送前进行IP替换处理
    if (PORT_CAM_CY_CMD == pThis->m_wTransPort)
    {
        pThis->HandleCAMCYData(pBuf, *pLen);
    }

    return pThis->CallBack_Send(pBuf, pLen, pUserData);
}

static bool IsImgHeadBuf(void* pData)
{
    CY_FRAME_HEADER* pFrameHead = (CY_FRAME_HEADER*)pData;
    if ( (pFrameHead->wFrameType > 7)
            || (pFrameHead->wFrameType > 11)
            || (pFrameHead->wFrameType > 2) && (pFrameHead->wFrameType < 10)
            || (pFrameHead->dwLen == 0)
            || (pFrameHead->dwLen > 2*1024*1024) )
    {
        return false;
    }
    else
    {
        return true;
    }
}

int CCAMCYTcpTransmit::CallBack_StartupCAMCY(void* pUserData)
{
    CCAMCYTcpTransmit* pThis = (CCAMCYTcpTransmit*)pUserData;
    pThis->m_nCAMCYExpDataStart = 0;
    pThis->m_nCAMCYImgDataBytes = 0;
    pThis->m_nCAMCYImgDataLen = 0;
    pThis->m_sFrameHead.dwLen = 0xFFFFFFFF;

    int iRet = CallBack_Startup(pUserData);
    return iRet;
}

int CCAMCYTcpTransmit::ReplaceImgIP(char* pBuf, int iLen)
{
    char* pImgHead;
    unsigned long ulIP = htonl(m_ulListenIP);
    static unsigned int s_fno = 0;
    int iDataRemian = iLen;
    bool fGetFrameHead = false;
    pImgHead = pBuf;

    while ((int)sizeof(m_sFrameHead) < iDataRemian)
    {
        // 避免内存对齐问题,出来再判断
        memcpy(&m_sFrameHead, pImgHead, sizeof(m_sFrameHead));
        if (!IsImgHeadBuf(&m_sFrameHead))
        {
            // 不合法的数据头，不记录长度
            m_nCAMCYImgDataBytes = 0;
            m_sFrameHead.dwLen = 0xFFFFFFFF;
            PRINTF("!! bad data2\n");

            return 0;
        }
        memcpy(pImgHead+44, &ulIP, 4);
        fGetFrameHead = true;
        PRINTF("New Frame Head, len:%d\n", m_sFrameHead.dwLen);

        if (++s_fno != m_sFrameHead.dwFrameNo)
        {
            printf("\n= lost %d =\n", m_sFrameHead.dwFrameNo - s_fno);
            s_fno = m_sFrameHead.dwFrameNo;
        }

        int iFrameLen = m_sFrameHead.dwLen + sizeof(m_sFrameHead);

        if (iDataRemian < iFrameLen)
        {
            break;
        }
        else
        {
            pImgHead += iFrameLen;
            iDataRemian -= iFrameLen;
        }
    }

    if ((m_nCAMCYImgDataBytes != 0)
        || fGetFrameHead )
    {
        m_nCAMCYImgDataBytes = iDataRemian;
    }

    return 1;
}

int CCAMCYTcpTransmit::CallBack_GetTransmitDataCAMCY(char* pBuf, int* pLen, void* pUserData)
{
    CCAMCYTcpTransmit* pThis = (CCAMCYTcpTransmit*)pUserData;

    // 命令端口直接转发，数据端口对帧头IP进行替换

    if (PORT_CAM_CY_CMD == pThis->m_wTransPort)
    {
        return CallBack_GetTransmitData(pBuf, pLen, pUserData);
    }

    int iRecvWant = 0;
    int ret = 1;
    int iRetLast = -1;
    int iBufLen = *pLen;

    // 接够或接不到数据才继续，以避免超短数据时混乱
    while (iRecvWant < (int)sizeof(pThis->m_sFrameHead))
    {
        ret = CallBack_GetTransmitData(pBuf+iRecvWant, pLen, pUserData);
        if (ret == 0)
        {
            return ret;
        }
        else if (*pLen == 0)
        {
            ret = (iRetLast == -1) ? ret : iRetLast;
            break;
        }
        else
        {
            iRecvWant += *pLen;
            iBufLen -= *pLen;
            *pLen = iBufLen;
            iRetLast = ret;
        }
    }

    *pLen = iRecvWant;
    if (*pLen < (int)sizeof(pThis->m_sFrameHead))
    {
        if (pThis->m_sFrameHead.dwLen != 0xFFFFFFFF)
            pThis->m_nCAMCYImgDataBytes += *pLen;    //tEnd(1);
        return ret;
    }

    int iLenDataRemain = *pLen;
    char* pDataStart = pBuf;
    while (true)
    {
        if ( pThis->m_nCAMCYImgDataBytes == 0 )
        {
            pThis->ReplaceImgIP(pDataStart, iLenDataRemain);
            break;
        }
        else if (pThis->m_sFrameHead.dwLen != 0xFFFFFFFF)
        {
            pThis->m_nCAMCYImgDataBytes += iLenDataRemain;
            int nAllLen = pThis->m_sFrameHead.dwLen + sizeof(pThis->m_sFrameHead);

            if (pThis->m_nCAMCYImgDataBytes >= nAllLen)
            {
                pDataStart += iLenDataRemain - (pThis->m_nCAMCYImgDataBytes - nAllLen);
                iLenDataRemain = pThis->m_nCAMCYImgDataBytes - nAllLen;
                pThis->m_nCAMCYImgDataBytes = 0;

                if (iLenDataRemain == 0)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            pThis->m_nCAMCYImgDataBytes = 0;
            break;
        }
    }

    return ret;
}
