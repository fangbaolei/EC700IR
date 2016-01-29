#include "HvSerialLink.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

//#define DEBUG_SERIAL
#ifdef	DEBUG_SERIAL
#define PRINTF printf
#else
#define PRINTF(...)
#endif

CHvSerialLink::CHvSerialLink()
{
}

CHvSerialLink::~CHvSerialLink()
{
}

int CHvSerialLink::Open(const char* szDev)
{
    int iRet = CSerialBase::Open(szDev);
    if (iRet != 0)
    {
        return iRet;
    }

    if (SetAttr(9600, 8, 0, 1) != 0)
    {
        return -1;
    }

    return 0;
}

bool CHvSerialLink::ExecuteCommand(char bCmdId,
                                        unsigned char* pbSendData,
                                        unsigned int iSendLen,
                                        unsigned char* pReceiveData,
                                        unsigned int & iReceiveLen,
                                        unsigned int dwRelayMs)
{
    bool fOk = false;
    if (Lock(1000) != 0)
    {
        if(0 == SendCmdData(bCmdId, pbSendData, iSendLen))
        {
            unsigned char bRecvCmdId;
            if(0 == RecvCmdData(&bRecvCmdId, pReceiveData, &iReceiveLen, dwRelayMs))
            {
                fOk = bCmdId == bRecvCmdId;
            }
        }
        UnLock();
    }
    return fOk;
}

// 接收一个完整的串口数据包
int CHvSerialLink::RecvPacket(unsigned char* pbData,
                              unsigned int* pdwRecvLen,
                              unsigned int dwRelayMs)
{
    unsigned int dwRecvLen = 0;
    unsigned int dwByteCount = 0;

    int iIntervalMs = 10;
    int  iTimeMs = (int)(dwRelayMs > 3000 ? 3000 : dwRelayMs);

    while (true)
    {
        dwByteCount = Recv(&pbData[ dwRecvLen ], 1, 0);
        if ( dwByteCount != 1 )
        {
            if ( iTimeMs <= 0 )
            {
                PRINTF("time out exit\n");
                return -1;
            }
            else
            {
                usleep(iIntervalMs*1000);
                iTimeMs -= iIntervalMs;
                continue;
            }
        }
        if ( dwRecvLen == 0 )
        {
            if (pbData[ 0 ] == FALG_BEGIN)
            {
                dwRecvLen++;
            }
            continue;
        }
        if ( pbData[dwRecvLen] == FALG_END )
        {
            dwRecvLen++;
            break;
        }
        dwRecvLen++;
    }

    *pdwRecvLen = dwRecvLen;
    return 0;
}

// 使用串口协议发送数据
int CHvSerialLink::SendCmdData(const unsigned char bCmdId,
                               unsigned char* pbData,
                               unsigned int dwSendLen)
{
    if (( pbData == NULL ) && (dwSendLen != 0))
    {
        return -1;
    }

    static char s_cmdbuf[MAX_PACK_LEN] = {FALG_BEGIN, FALG_NUMBER, 0};
    unsigned int dwPacketLen = 0;
    // 组包
    MakePacket(bCmdId, FALG_NUMBER, (unsigned char*)pbData, dwSendLen, (unsigned char*)s_cmdbuf,
        (unsigned int*)&dwPacketLen);

    // 发送
    return ((int)dwPacketLen == Send((unsigned char*)s_cmdbuf, dwPacketLen)) ? 0 : -1;
}

// 使用串口协议接收数据
int CHvSerialLink::RecvCmdData(unsigned char* pbCmdId,
                               unsigned char* pbData,
                               unsigned int* pdwLen,
                               unsigned int dwRelayMs)
{
    static unsigned char s_recvData[MAX_PACK_LEN];

    unsigned int dwRecvLen = 0;

    if ( -1 == RecvPacket(s_recvData, &dwRecvLen, dwRelayMs))
    {
        return -1;
    }

    int hr = PickupDataFromPacket(s_recvData, dwRecvLen, pbCmdId, NULL, pbData,  pdwLen);
    if ( hr != 0 )
    {
        *pdwLen = 0;
    }
    return hr;
}

// 从协议数据包里数据-1 解析失败 -2 缓冲不足
int CHvSerialLink::PickupDataFromPacket(
    const unsigned char* recvbuf,
    const unsigned int dwRecvLen,
    unsigned char* pbCmdId,
    unsigned char* pbSN,
    unsigned char* pbData,
    unsigned int* pdwDataLen)
{
    if (  (int)dwRecvLen > MAX_PACK_LEN
            || recvbuf[0] != FALG_BEGIN
            || recvbuf[dwRecvLen-1] != FALG_END)
    {
        return -1;
    }

    static unsigned char rgbData[MAX_PACK_LEN] = {0};
    int iPos = 0;
    rgbData[iPos++] = FALG_BEGIN;
    int i = 0;
    for ( i = 1; i < (int)(dwRecvLen-1); ++i, ++iPos)
    {
        rgbData[iPos] = recvbuf[i];
        if ( recvbuf[i] == FALG_EXCHANGE )
        {
            rgbData[iPos] += recvbuf[i+1];
            ++i;
        }
    }
    rgbData[iPos++] = FALG_END;

    unsigned char dwCheck = 0;
    for ( i = 0; i < iPos-2; ++i )
    {
        dwCheck += rgbData[i];
    }
    if ( dwCheck != rgbData[iPos-2] )
    {
        return -1;
    }

    if (pbCmdId != NULL)
    {
        *pbCmdId = rgbData[2];
    }

    if (pbSN != NULL)
    {
        *pbSN = rgbData[1];
    }

    unsigned int datalen = iPos - 5;
    if (pdwDataLen != NULL)
    {
        if (*pdwDataLen > datalen)
        {
            *pdwDataLen = datalen;
        }
        else
        {
            return -2;
        }
    }

    if (pbData != NULL)
    {
        memcpy(pbData, &rgbData[3], datalen);
    }

    return 0;
}

// 组包函数
void CHvSerialLink::MakePacket(
    const unsigned char bCmdId,
    const unsigned char bSN,
    const unsigned char* pbData,
    const unsigned int dwDataLen,
    unsigned char* pbPacketData,
    unsigned int* pdwPacketLen)
{
    pbPacketData[0] = FALG_BEGIN;
    pbPacketData[1] = bSN;
    pbPacketData[2] = bCmdId;

    unsigned char cCheck = 0;
    unsigned int dwLen = 3;

    if (pbData != NULL)
    {
        for (int i = 0;i < (int)dwDataLen;i++)
        {
            if (pbData[i] == FALG_EXCHANGE
                || pbData[i] == FALG_BEGIN
                || pbData[i] == FALG_END)
            {
                pbPacketData[dwLen++] = FALG_EXCHANGE;
                pbPacketData[dwLen++] = pbData[i] - FALG_EXCHANGE;
            }
            else
            {
                pbPacketData[dwLen++] = pbData[i];
            }
        }
    }

    for ( int i = 0; i < (int)dwLen; ++i )
    {
        cCheck += pbPacketData[i];
    }
    if (cCheck == FALG_EXCHANGE
            || cCheck == FALG_BEGIN
            || cCheck == FALG_END)
    {
        pbPacketData[dwLen++] = FALG_EXCHANGE;
        pbPacketData[dwLen++] = cCheck - FALG_EXCHANGE;
    }
    else
        pbPacketData[dwLen++] = cCheck;
    pbPacketData[dwLen++] = FALG_END;

    *pdwPacketLen = dwLen;
}

// 切换通道
// 返回值: 0 成功；-1 发送失败； -2 解析接收数据包失败
int CHvSerialLink::SendSetChannel(unsigned char bChannel, unsigned int nTimeOMs)
{
    return SendCmdData(0xA1, &bChannel, sizeof(bChannel));
}
