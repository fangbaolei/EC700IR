#include "IPSearchThread.h"
#include "hvsocket.h"
#include "ControlFunc.h"
#include "TcpipCfg.h"
#include "hvtarget_ARM.h"

#ifdef SINGLE_BOARD_PLATFORM
#include "CamTransmit.h"
#endif

using namespace HiVideo;

CIPSearchThread::CIPSearchThread()
        : m_fRunStatus(false)
{
    m_dwLastThreadIsOkTime = GetSystemTick();
}

CIPSearchThread::~CIPSearchThread()
{
    Close();
}

bool CIPSearchThread::IsConnected()
{
    return m_fRunStatus;
}

HRESULT CIPSearchThread::Create()
{
    if ( m_fRunStatus ) return E_FAIL;

    return this->Start(NULL);
}

HRESULT CIPSearchThread::Close()
{
    this->Stop(-1);
    return S_OK;
}

HRESULT CIPSearchThread::Run( void *pvParamter )
{
    m_fRunStatus = true;

    const u_short GETIP_COMMAND_PORT = 6666;
    const DWORD32 GETIP_COMMAND = 0x00000101;
    const DWORD32 SETIP_COMMAND = 0x00000102;

    struct command_protocol_t cUDPCmdContext;

    HV_SOCKET_HANDLE hSocket = INVALID_SOCKET;
    hSocket = Socket(AF_INET, SOCK_DGRAM, 0);
    if ( INVALID_SOCKET == hSocket )
    {
        m_fRunStatus = false;
        m_fExit = TRUE;
        return E_FAIL;
    }

    BOOL on = 1;
    SetSockOpt(hSocket, SOL_SOCKET, SO_BROADCAST, (const char*)&on, sizeof(on));

    int iOpt = 1;
    SetSockOpt(hSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&iOpt, sizeof(iOpt));

    struct hv_sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(GETIP_COMMAND_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ( 0 != Bind(hSocket, (const struct hv_sockaddr*)&addr, sizeof(addr)) )
    {
        CloseSocket(hSocket);
        m_fRunStatus = false;
        m_fExit = TRUE;
        HV_Trace(5, "IPSearch bind failed!");
        return E_FAIL;
    }

    char rgBuf[24 + 32]; //新增加32位，发送序列号
    struct hv_sockaddr_in sender_addr;
    int iLen = sizeof(sender_addr);

    while (!m_fExit)
    {
    	memset(rgBuf, 0 ,sizeof(rgBuf));
        m_dwLastThreadIsOkTime = GetSystemTick();

        int iRecvLen = RecvFrom(hSocket, (char*)rgBuf, sizeof(rgBuf), 0, (struct hv_sockaddr*)&sender_addr, &iLen);

        //对收到的广播包进行分类处理
        if (iRecvLen == sizeof(DWORD32) && 0 == memcmp(&rgBuf[0], &GETIP_COMMAND, sizeof(GETIP_COMMAND)))
        {
            BYTE8 rgMac[6] = {0};
            DWORD32 dwIP = 0;
            DWORD32 dwMask = 0;
            DWORD32 dwGateway = 0;

            if ( S_OK == GetTcpipAddr(rgMac, &dwIP, &dwMask, &dwGateway) )
            {
                memcpy(&rgBuf[4], rgMac, 6);
                memcpy(&rgBuf[10], &dwIP, 4);
                memcpy(&rgBuf[14], &dwMask, 4);
                memcpy(&rgBuf[18], &dwGateway, 4);

				//取序列号
                char szSerialNo[128] = {0};
                GetSN(szSerialNo, sizeof(szSerialNo));
                int iSerialNo = strlen(szSerialNo);
                if (iSerialNo > 0 && iSerialNo < 32)
                {
                	memcpy(&rgBuf[22], szSerialNo, iSerialNo);
                }

                HV_SOCKET_HANDLE hSockBroadcast;

#ifdef SINGLE_BOARD_PLATFORM
                // 选定要发送的网口需要绑定其IP，否则可能发送到别的网口上
                hSockBroadcast = INVALID_SOCKET;
                hSockBroadcast = Socket(AF_INET, SOCK_DGRAM, 0);
                if ( INVALID_SOCKET != hSocket )
                {
                    BOOL on = 1;
                    SetSockOpt(hSockBroadcast, SOL_SOCKET, SO_BROADCAST, (const char*)&on, sizeof(on));

                    int iOpt = 1;
                    SetSockOpt(hSockBroadcast, SOL_SOCKET, SO_REUSEADDR, (const char*)&iOpt, sizeof(iOpt));

                    struct hv_sockaddr_in addr;
                    memset(&addr, 0, sizeof(addr));

                    addr.sin_family = AF_INET;
                    addr.sin_port = htons(GETIP_COMMAND_PORT);
                    addr.sin_addr.s_addr = htonl(dwIP);

                    if ( 0 != Bind(hSockBroadcast, (const struct hv_sockaddr*)&addr, sizeof(addr)) )
                    {
                        HV_Trace(5, "<CIPSearchThread>::Run handle search-cmd bind failed\n");
                        close(hSockBroadcast);
                        hSockBroadcast = INVALID_SOCKET;
                    }
                }
#else
                hSockBroadcast = hSocket;
#endif // #ifdef SINGLE_BOARD_PLATFORM

                if (hSockBroadcast != INVALID_SOCKET)
                {
                    sender_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

                    int i = SendTo(hSockBroadcast, (char*)rgBuf, 22 + 32, 0, (const struct hv_sockaddr*)&sender_addr, sizeof(sender_addr));
                    if (i < 22)
                    {
                        char szCmd[256] = {0};
                        char szIp[32] = {0};

                        MyGetIpString(dwIP, szIp);

                        // Linux下限制了255.255.255.255广播发送，
                        // 先临时把网关设为本身，再发广播。
                        HV_Trace(5, "waring:add the gateway to send broadcast!\n");
                        sprintf(szCmd, "add default gw %s", szIp);
                        if ( 0 != HvRoute(szCmd) )
                        {
                            HV_Trace(5, "%s error\n", szCmd);
                        }

                        i = SendTo(hSockBroadcast, (char*)rgBuf, 22 + 32, 0, (const struct hv_sockaddr*)&sender_addr, sizeof(sender_addr));
                        if ( i < 22 )
                        {
                            perror("sendto");
                            HV_Trace(5, "<CIPSearchThread::Run> SendTo is Error!\n");
                        }

                        sprintf(szCmd, "del default gw %s", szIp);
                        if ( 0 != HvRoute(szCmd) )
                        {
                            HV_Trace(5, "%s error\n", szCmd);
                        }
                    }
                }

#ifdef SINGLE_BOARD_PLATFORM
                if (hSockBroadcast != INVALID_SOCKET)
                {
                    close(hSockBroadcast);
                }
#endif
            }
        }
        else if (iRecvLen == 22 && 0 == memcmp(&rgBuf[0], &SETIP_COMMAND, sizeof(SETIP_COMMAND)))
        {
            BYTE8 rgMac[6] = {0};
            DWORD32 dwIP = 0;
            DWORD32 dwMask = 0;
            DWORD32 dwGateway = 0;

            memcpy(rgMac, &rgBuf[4], 6);
            memcpy(&dwIP, &rgBuf[10], 4);
            memcpy(&dwMask, &rgBuf[14], 4);
            memcpy(&dwGateway, &rgBuf[18], 4);

            if ( S_OK == SetTcpipAddr(rgMac, dwIP, dwMask, dwGateway) )
            {
                memset(&rgBuf[4], 0, 4);
                SendTo(hSocket, (char*)rgBuf, 4, 0, (const struct hv_sockaddr*)&sender_addr, sizeof(sender_addr));

#ifdef SINGLE_BOARD_PLATFORM
                DWORD32 dwIPHost = ntohl(dwIP);
                DWORD32 dwMaskHost = ntohl(dwMask);
                HRESULT hrResult = ChangeCamTransmit(dwIPHost, dwMaskHost);
                if (S_OK != hrResult && S_FALSE != hrResult)
                {
                    HV_Trace(5, "ChangeCamTransmit Failed !\n");
                }
#endif
            }
        }
        else if ( sizeof(cUDPCmdContext) == iRecvLen )
        {
            memcpy(&cUDPCmdContext, &rgBuf[0], sizeof(cUDPCmdContext));

            if ( COMMAND_PROTOCOL_PASS == htonl(cUDPCmdContext.pass) )
            {
                // 由于协议原因，这里要从网络序转为主机序（注：这里发现ntohl这个函数无效，所以只能违反语义而用htonl了）
                cUDPCmdContext.len = htonl(cUDPCmdContext.len);
                cUDPCmdContext.type = htonl(cUDPCmdContext.type);
                cUDPCmdContext.id = htonl(cUDPCmdContext.id);

                if ( 12 == cUDPCmdContext.len
                        && GET_STATUS_COMMAND_PROTOCOL_TYPE == cUDPCmdContext.type
                        && GET_STATUS_ID_GET == cUDPCmdContext.id )
                {
                    if ( S_OK == GetRunState((DWORD32*)&cUDPCmdContext.id) )
                    {
                        cUDPCmdContext.len = htonl(cUDPCmdContext.len);
                        cUDPCmdContext.type = htonl(cUDPCmdContext.type);
                        cUDPCmdContext.id = htonl(cUDPCmdContext.id);

                        SendTo(
                            hSocket,
                            (char*)&cUDPCmdContext,
                            sizeof(cUDPCmdContext),
                            0,
                            (const struct hv_sockaddr*)&sender_addr,
                            sizeof(sender_addr)
                        );
                    }
                }
                else if ( 12 == cUDPCmdContext.len
                          && RESET_COMMAND_PROTOCOL_TYPE == cUDPCmdContext.type )
                {
                    HvResetDevice(cUDPCmdContext.id);
                }
                else if ( 12 == cUDPCmdContext.len
                          && GETSN_COMMAND_PROTOCOL_TYPE == cUDPCmdContext.type
                          && GETSN_ID_GET == cUDPCmdContext.id )
                {
                    char szRespDat[sizeof(cUDPCmdContext) + 128] = {0};
                    if ( GetSN((char*)&szRespDat + sizeof(cUDPCmdContext), 128) >= 0 )
                    {
                        cUDPCmdContext.id = GETSN_ID_SUCC;
                        cUDPCmdContext.len = 12 + MACHINE_SN_LEN;
                    }
                    else
                    {
                        cUDPCmdContext.id = GETSN_ID_FAIL;
                    }

                    int nRespDatLen = 4 + cUDPCmdContext.len;

                    cUDPCmdContext.len = htonl(cUDPCmdContext.len);
                    cUDPCmdContext.type = htonl(cUDPCmdContext.type);
                    cUDPCmdContext.id = htonl(cUDPCmdContext.id);

                    memcpy(szRespDat, &cUDPCmdContext, sizeof(cUDPCmdContext));

                    SendTo(
                        hSocket,
                        (char*)&szRespDat,
                        nRespDatLen,
                        0,
                        (const struct hv_sockaddr*)&sender_addr,
                        sizeof(sender_addr)
                    );
                }
                else if ( 12 == cUDPCmdContext.len
                          && GET_NETINFO_COMMAND_PROTOCOL_TYPE == cUDPCmdContext.type
                          && GET_NETINFO_GET == cUDPCmdContext.id )
                {
                    BYTE8 rgbBuf[128];
                    int iBufLen;

                    cUDPCmdContext.type = htonl(cUDPCmdContext.type);

                    BYTE8 rgMac[6] = {0};
                    DWORD32 dwIP = 0;
                    DWORD32 dwMask = 0;
                    DWORD32 dwGateway = 0;

                    if ( S_OK == GetTcpipAddr(rgMac, &dwIP, &dwMask, &dwGateway) )
                    {
                        memcpy(&rgbBuf[sizeof(cUDPCmdContext)], rgMac, 6);
                        memcpy(&rgbBuf[sizeof(cUDPCmdContext)+6], &dwIP, 4);
                        memcpy(&rgbBuf[sizeof(cUDPCmdContext)+10], &dwMask, 4);
                        memcpy(&rgbBuf[sizeof(cUDPCmdContext)+14], &dwGateway, 4);

                        cUDPCmdContext.len = htonl(sizeof(cUDPCmdContext)+18-4);
                        cUDPCmdContext.id = htonl(GET_NETINFO_GET_SUCC);

                        iBufLen = sizeof(cUDPCmdContext)+18;
                    }
                    else
                    {
                        cUDPCmdContext.len = htonl(cUDPCmdContext.len);
                        cUDPCmdContext.id = htonl(GET_NETINFO_GET_FAIL);

                        iBufLen = sizeof(cUDPCmdContext);
                    }

                    memcpy(rgbBuf, &cUDPCmdContext, sizeof(cUDPCmdContext));

                    SendTo(
                        hSocket,
                        (char*)rgbBuf,
                        iBufLen,
                        0,
                        (const struct hv_sockaddr*)&sender_addr,
                        sizeof(sender_addr)
                    );
                }
            }
        }
        else
        {
            HV_Trace(1, "Other Broadcast:[IP: %s:%d]\n", inet_ntoa(sender_addr.sin_addr), ntohs(sender_addr.sin_port));
            continue;
        }
    }

    CloseSocket(hSocket);

    m_fRunStatus = false;
    m_fExit = TRUE;
    return S_OK;
}

bool CIPSearchThread::ThreadIsOk()
{
    bool fRtn = false;
    if (GetSystemTick() - m_dwLastThreadIsOkTime < 10000)
    {
        fRtn = true;
    }
    return fRtn;
}
