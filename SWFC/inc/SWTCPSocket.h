#ifndef CSWTCPSOCKET_H
#define CSWTCPSOCKET_H

#include "SWSocket.h"

//todo...
#define ONACCEPT VOID*

class CSWTCPSocket : public CSWSocket
{
public:
    CSWTCPSocket();
    virtual ~CSWTCPSocket();

    // 创建套接字
	HRESULT Create( BOOL fLocalSock = FALSE);


    // 数据接收
	HRESULT Read(VOID *pbBuf, DWORD dwSize , DWORD *rcvDataLen = NULL);
	// 数据传输
	HRESULT Send(const VOID *pbBuf, DWORD dwSize , DWORD *dwOutLengthSent = NULL);
    // 监听
    HRESULT Listen( INT backlog = 128 );
    // 接收链接套接子
    HRESULT Accept( SWPA_SOCKET_T &outSock );

protected:
private:
	BOOL m_fLocalSock;
};

#endif // CSWTCPSOCKET_H

