#ifndef CSWSOCKET_H
#define CSWSOCKET_H


#include "SWObject.h"
#include "swpa_socket.h"

class CSWSocket : public CSWObject
{
public:
    enum{
     INVALID = -1,        // 无效状态
     SOCKCREATE,          // 创建套接字
     CONNECTIONT,         // 连接状态
    };
public:


    CSWSocket();
    virtual ~CSWSocket();

	HRESULT Close();
	bool IsValid();
    bool IsConnect( );

	HRESULT Connect(const char * szIP, const WORD wPort);
	HRESULT Connect(const char * szPath);
    HRESULT Bind(const char * szIP, const WORD wPort);
	HRESULT Bind(const char * szPath);

    HRESULT Getsockname( char* szName );

	HRESULT GetPeerName(char* szName , WORD *wPort = NULL );
	HRESULT GetRecvTimeout(DWORD* pdwMS);
	HRESULT GetSendTimeout(DWORD* pdwMS);

	HRESULT SetRecvTimeout(const DWORD dwMS);
	HRESULT SetSendTimeout(const DWORD dwMS);

	HRESULT SetSendBufferSize(const DWORD dwSize);
	HRESULT SetRecvBufferSize(const DWORD dwSize);
  // 关联Socket
  HRESULT Attach( SWPA_SOCKET_T hSock );
  // 分离套接字
  HRESULT Detach( );
  HRESULT SetSockOpt(INT iLevel,INT iOptname ,PVOID pOptval, INT iOptlen);
protected:
	HRESULT Create( INT nSocketType = SWPA_SOCK_STREAM );
	HRESULT Create( INT nSocketType, INT nDomain);
protected:
	// 套接字结构体
	SWPA_SOCKET_T m_objSockek;
	// 连接信息
	SWPA_SOCKET_ATTR_T m_ConnectionAttrInfo;

	// 本地链接地址
	SWPA_SOCKADDR m_objAddr;
	// 远程地址
	SWPA_SOCKADDR m_objRemoteAddr;

	//本地套接字地址
	SWPA_SOCKADDR_UN m_objLocalAddr;

	//套接字类型
	INT m_iDomain;

    // 连接状态
    INT m_nState;
};

#endif // CSWSOCKET_H

