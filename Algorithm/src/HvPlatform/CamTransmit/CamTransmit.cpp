#include "CamTransmit.h"

#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "hvtarget_ARM.h"
#include "CamCYTransmit.h"
#include "CamNVCTransmit.h"

#ifndef PRINTF
#define PRINTF printf
#else
#define PRINTF(...)
#endif

static int g_iCamType = TRANSMIT_TYPE_CAM_NULL;

static DWORD g_dwLAN1IP = 0;
static DWORD g_dwLAN1Mask = 0;
static DWORD g_dwCamIP = 0;

static CCamCYTransmit* g_pCamCYTransmit = NULL;
static CCamNVCTransmit* g_pCamNVCTransmit = NULL;

ICamTransmit* g_pCamTransmit = NULL;

/* TcpipCfg.cpp */
extern void ConvertStrIP2DWORD(char *pszIP, DWORD32 &dwIP);

static HRESULT CamTransmit(int iType,
                           DWORD32 dwIPLAN2,
                           DWORD32 dwMaskLAN2,
                           DWORD32 dwIPLAN1,
                           DWORD32 dwMaskLAN1,
                           DWORD32 dwIPCam)
{
    HRESULT hr = E_FAIL;

    switch (iType)
    {
    case TRANSMIT_TYPE_CAM_CY :
    {
        if (g_pCamCYTransmit == NULL)
        {
            g_pCamCYTransmit = new CCamCYTransmit;
        }

        if (g_pCamCYTransmit != NULL)
        {
            if (g_pCamCYTransmit->Run(dwIPLAN2,
                                     dwMaskLAN2,
                                     dwIPLAN1,
                                     dwMaskLAN1,
                                     dwIPCam) == S_OK)
            {
                hr = S_OK;
            }
        }
        g_pCamTransmit = g_pCamCYTransmit;
        break;
    }

    case TRANSMIT_TYPE_CAM_NVC :
    {
        if (g_pCamNVCTransmit == NULL)
        {
            g_pCamNVCTransmit = new CCamNVCTransmit;
        }

        if (g_pCamNVCTransmit != NULL)
        {
            if (g_pCamNVCTransmit->Run(dwIPLAN2,
                                      dwMaskLAN2,
                                      dwIPLAN1,
                                      dwMaskLAN1,
                                      dwIPCam) == S_OK)
            {
                hr = S_OK;
            }
        }
        g_pCamTransmit = g_pCamNVCTransmit;
        break;
    }

    default :
    {
        break;
    }
    }

    return hr;
}

void StopCamTransmit()
{
    switch (g_iCamType)
    {
    case TRANSMIT_TYPE_CAM_CY :
    {
        if (g_pCamCYTransmit != NULL)
        {
            g_pCamCYTransmit->Stop();
        }
        break;
    }

    case TRANSMIT_TYPE_CAM_NVC :
    {
        if (g_pCamNVCTransmit != NULL)
        {
            g_pCamNVCTransmit->Stop();
        }
        break;
    }

    default :
    {
        break;
    }
    }
}

/*int SearchCamCY(DWORD32 dwIPLAN, DWORD32 dwLANMask, DWORD32* pdwIPCam)
{
    int fdSock = socket(AF_INET, SOCK_DGRAM, 0);
    int iRet = -1;
    if (fdSock == -1)
    {
        HV_Trace(5, "CheckCamNVC socket error!\n");
        return -2;
    }

    sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_CAM_CY_BROADCAST_RECV);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int iOpt = 1;
    int iOptLen = sizeof(iOpt);
    setsockopt(fdSock, SOL_SOCKET, SO_REUSEADDR, (char*)&iOpt, iOptLen);
    setsockopt(fdSock, SOL_SOCKET, SO_BROADCAST, (char*)&iOpt, iOptLen);

    if (bind(fdSock, (struct sockaddr *) &addr, sizeof(addr)) != 0)
    {
        HV_Trace(5, "CBroadcastTransmit bind error %s!\n", strerror(errno));
        close(fdSock);
        return -2;
    }

    unsigned char rgbBuf[] =
        {  0x78, 0x56, 0x34, 0x12, 0x21, 0x43, 0x65, 0x87
           , 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00
           , 0xDD, 0xC0, 0x47, 0x77, 0xEE, 0x4C, 0x01, 0x4A
           , 0x60, 0x0B, 0xD8, 0x01, 0x02, 0x00, 0x00, 0x00
           , 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
           , 0x04, 0x0C, 0xD8, 0x01, 0x68, 0xF7, 0x12, 0x00
           , 0x00, 0x00, 0x00, 0x00, 0x74, 0xF7, 0x12, 0x00
           , 0x27, 0x93, 0xB1, 0x76, 0xEE, 0x4C, 0x01, 0x4A
        };

    char szRcvBuf[128];

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_CAM_CY_BROADCAST_SEND);
    addr.sin_addr.s_addr = inet_addr("255.255.255.255");

    HV_Trace(5, "send to %s\n", inet_ntoa(addr.sin_addr));

    if (0 >= TrsmBroadcastSend(dwIPLAN, PORT_CAM_CY_BROADCAST_RECV, rgbBuf,
                      sizeof(rgbBuf), 0, (sockaddr*)&addr, sizeof(addr)))
    {
        close(fdSock);
        return -2;
    }

    // set noblock
    int iFlags = fcntl(fdSock, F_GETFL, 0);
    fcntl(fdSock, F_SETFL, iFlags|O_NONBLOCK);

    if (1 == SocketWait(fdSock, true, false, 500*1000))
    {
        int iLen;
        DWORD32 dwIPCam;
        do
        {
            HV_Sleep(10);

            socklen_t addrlen = sizeof(addr);
            memset(&addr, 0, sizeof(addr));
            iLen = recvfrom(fdSock, szRcvBuf, sizeof(szRcvBuf), 0, (sockaddr*)&addr, &addrlen);
            if (iLen > 0)
            {
                dwIPCam = ntohl(addr.sin_addr.s_addr);
                if ((iLen == sizeof(rgbBuf))
                        && (memcmp(szRcvBuf, rgbBuf, iLen) == 0)
                        || (dwIPCam == dwIPLAN)
                        || ((dwIPCam & dwLANMask) != (dwIPLAN & dwLANMask)))
                {
                    HV_Trace(5, "%08X_%08X_%08X_\n", dwIPCam, dwIPLAN, dwLANMask);
                    continue;
                }
                *pdwIPCam = dwIPCam;
                iRet = 0;
                HV_Trace(5, "Recv Len=%d. from %s\n", iLen, inet_ntoa(addr.sin_addr));
            }
        }
        while ( iLen > 0 );
    }

    close(fdSock);
    return iRet;
}

static int SearchCamNVC(DWORD32 dwIPLAN, DWORD32 dwLANMask, DWORD32* pdwIPCam)
{
    int fdSock = socket(AF_INET, SOCK_DGRAM, 0);

    if (fdSock == -1)
    {
        HV_Trace(5, "CheckCamNVC socket error!\n");
        return -2;
    }

    sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_CAM_NVC);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int iOpt = 1;
    int iOptLen = sizeof(iOpt);
    setsockopt(fdSock, SOL_SOCKET, SO_REUSEADDR, (char*)&iOpt, iOptLen);
    setsockopt(fdSock, SOL_SOCKET, SO_BROADCAST, (char*)&iOpt, iOptLen);

    if (bind(fdSock, (struct sockaddr *) &addr, sizeof(addr)) == -1)
    {
        HV_Trace(5, "CBroadcastTransmit bind error!\n");
        close(fdSock);
        return -1;
    }

    unsigned char rgbBuf[] =
        {  0x41, 0x72, 0x65, 0x63, 0x6F, 0x6E, 0x74, 0x5F
           , 0x56, 0x69, 0x73, 0x69, 0x6F, 0x6E, 0x2D, 0x41
           , 0x56, 0x32, 0x30, 0x30, 0x30, 0x01
        };

    char szRcvBuf[256];
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_CAM_NVC);
    addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    HV_Trace(5, "send to %s\n", inet_ntoa(addr.sin_addr));

    if (0 >= TrsmBroadcastSend(dwIPLAN, PORT_CAM_NVC, rgbBuf,
                      sizeof(rgbBuf), 0, (sockaddr*)&addr, sizeof(addr)))
    {
        close(fdSock);
        return -2;
    }

    // set noblock
    int iFlags = fcntl(fdSock, F_GETFL, 0);
    fcntl(fdSock, F_SETFL, iFlags|O_NONBLOCK);

    int iRet = -1;
    DWORD32 dwIPCam;
    if (1 == SocketWait(fdSock, true, false, 500*1000))
    {
        int iLen;
        do
        {
            HV_Sleep(10);
            socklen_t addrlen = sizeof(addr);
            memset(&addr, 0, sizeof(addr));
            iLen = recvfrom(fdSock, szRcvBuf, sizeof(szRcvBuf), 0, (sockaddr*)&addr, &addrlen);
            if (iLen > 0)
            {
                dwIPCam = ntohl(addr.sin_addr.s_addr);
                if ((iLen == sizeof(rgbBuf))
                        && (memcmp(szRcvBuf, rgbBuf, iLen) == 0)
                        || (dwIPCam == dwIPLAN)
                        || ((dwIPCam & dwLANMask) != (dwIPLAN & dwLANMask)))
                {
                    continue;
                }
                *pdwIPCam = dwIPCam;
                iRet = 0;
                HV_Trace(5, "Recv Len=%d. from %s\n", iLen, inet_ntoa(addr.sin_addr));
            }
        }
        while ( iLen > 0 );
    }

    close(fdSock);
    return iRet;
}

static HRESULT GetCamType(DWORD32 dwIPLAN1, DWORD32 dwLAN1Mask, int* piType, DWORD32* pdwIPCam)
{
    if (SearchCamCY(dwIPLAN1, dwLAN1Mask, pdwIPCam) == 0)
    {
        *piType = TRANSMIT_TYPE_CAM_CY;
    }
    else
    {
        WdtHandshake();
        if (SearchCamNVC(dwIPLAN1, dwLAN1Mask, pdwIPCam) == 0)
        {
            *piType = TRANSMIT_TYPE_CAM_NVC;
        }
        else
        {
            return -1;
        }
    }

    return 0;
}*/

HRESULT StartCamTransmit(int nCamType, char* pszCamIP, TcpipParam cTcpipCfgParam1, TcpipParam cTcpipCfgParam2)
{
    if (nCamType != 1 && nCamType != 2)
    {
        HV_Trace(5, "Error camera type, stop camtransmit!");
        return S_FALSE;
    }

    if (pszCamIP == NULL)
    {
        HV_Trace(5, "Camera ip is NULL!");
        return E_POINTER;
    }

    if (nCamType == 1)
    {
        g_iCamType = TRANSMIT_TYPE_CAM_NVC;
    }
    else
    {
        g_iCamType = TRANSMIT_TYPE_CAM_CY;
    }

    // 搜索相机,设置转发
    DWORD32 dwIP1, dwIP2, dwszNetmask1, dwszNetmask2;
    ConvertStrIP2DWORD(cTcpipCfgParam1.szIp, dwIP1);
    ConvertStrIP2DWORD(cTcpipCfgParam1.szNetmask, dwszNetmask1);
    ConvertStrIP2DWORD(cTcpipCfgParam2.szIp, dwIP2);
    ConvertStrIP2DWORD(cTcpipCfgParam2.szNetmask, dwszNetmask2);

    g_dwLAN1IP = dwIP1;
    g_dwLAN1Mask = dwszNetmask1;
    g_dwCamIP = ntohl(inet_addr(pszCamIP));

    HRESULT hRet = S_OK;

    WdtHandshake();

    if (S_OK != CamTransmit(g_iCamType, dwIP2, dwszNetmask2, dwIP1, dwszNetmask1, g_dwCamIP))
    {
        HV_Trace(5, "Transmit svr run failed!\n");
        hRet = E_FAIL;
    }

    return hRet;
}

HRESULT ChangeCamTransmit(DWORD dwLAN2IP, DWORD dwLAN2Mask)
{
    if (g_iCamType == TRANSMIT_TYPE_CAM_NULL)
    {
        return S_FALSE;
    }

    HV_Trace(5, "=== Change Cam Transmit.");

    // 先停止转发
    StopCamTransmit();

    HRESULT hRet = S_OK;

    in_addr inaddr;
    inaddr.s_addr = htonl(dwLAN2IP);

    // 运行转发线程
    if (S_OK != CamTransmit(g_iCamType, dwLAN2IP, dwLAN2Mask, g_dwLAN1IP, g_dwLAN1Mask, g_dwCamIP))
    {
        HV_Trace(5, "=== Transmit svr run failed!\n");
        hRet = E_FAIL;
    }

    HV_Trace(5, "=== Change Cam Transmit end.");
    return hRet;
}

