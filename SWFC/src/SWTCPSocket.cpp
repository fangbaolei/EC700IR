#include<stdio.h>
#include "SWTCPSocket.h"
#include "swpa.h"

CSWTCPSocket::CSWTCPSocket()
{
    //ctor
    m_fLocalSock = FALSE;
}

CSWTCPSocket::~CSWTCPSocket()
{
    //dtor
}
/**
 * @brief 创建TCP socket
 *
 * @param [in] wPort : 绑定的端口号,默认为0
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWTCPSocket::Create(  BOOL fLocalSock /*= FALSE*/ ){

	if (TRUE == fLocalSock)
	{
		m_fLocalSock = TRUE;
		return CSWSocket::Create( SWPA_SOCK_STREAM , SWPA_AF_UNIX);
	}
	else
	{
    	return CSWSocket::Create( SWPA_SOCK_STREAM );
	}
}
/**
* @brief 等待客户端链接
*
* @param [in] skt 套接字结构体
* @param [in] backlog 套接字监听队列所允许的最大长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_socket.h
*/
HRESULT CSWTCPSocket::Listen( INT backlog )
{
    if( m_objSockek == INVALID) return E_FAIL;

    return swpa_socket_listen(m_objSockek, backlog);
}
HRESULT CSWTCPSocket::Accept( SWPA_SOCKET_T &outSock )
{
    if( m_objSockek == INVALID) return E_FAIL;

    DWORD leng = sizeof(m_objAddr);

    if( SWPAR_FAIL == swpa_socket_accept(m_objSockek, &m_objAddr
                              , &leng , &outSock))
    {
      return E_FAIL;
    }

    return S_OK;
}

/**
 * @brief 数据接收
 *
 * @param [PVOID] pbBuf : 需要存储的数据缓存
 * @param [DWORD] dwSize : 缓存大小
 * @param [PDWORD] rcvDataLen : 实际接收的数据大小
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWTCPSocket::Read(VOID *pbBuf, DWORD dwSize , DWORD *rcvDataLen ){

    if( NULL == pbBuf || 0 == dwSize
       || !this->IsConnect( ) ) return E_FAIL;

    DWORD dwCurrentPos = 0 ;DWORD dwReadLen = dwSize;
    while( dwCurrentPos < dwSize )
    {
        if( SWPAR_FAIL == swpa_socket_recv( m_objSockek , (CHAR*)pbBuf + dwCurrentPos, &dwReadLen ))
        {
            if( swpa_socket_get_lastererrocode( ) != SWPA_EAGAIN && this->IsConnect( ))
                m_nState ^= CONNECTIONT;

            if( rcvDataLen != NULL ) *rcvDataLen = dwCurrentPos;
            return E_FAIL;
        }

        dwCurrentPos += dwReadLen; dwReadLen = dwSize - dwCurrentPos;
    }

    if( rcvDataLen != NULL ) *rcvDataLen = dwCurrentPos;

    return S_OK;
}
/**
 * @brief 数据传输
 *
 * @param [LPCVOID] pbBuf : 需要传输的数据缓存
 * @param [DWORD] dwSize : 需要传输的数据大小
 * @param [PDWORD] dwOutLengthSent : 实际传输的数据大小
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWTCPSocket::Send(const VOID* pbBuf, DWORD dwSize , DWORD *dwOutLengthSent){

    if( NULL == pbBuf || 0 == dwSize
       || !this->IsConnect( ) ) return E_FAIL;

    int nSendSize = 0; DWORD dwOutLength = 0;
    while( nSendSize < dwSize )
    {
       if( SWPAR_FAIL == swpa_socket_send(m_objSockek, (CHAR*)pbBuf + nSendSize
                                          , dwSize - nSendSize, &dwOutLength )
          || dwOutLength == 0)
        {
            if( /*swpa_socket_get_lastererrocode( ) != SWPA_EAGAIN &&*/ this->IsConnect( ))
                m_nState ^= CONNECTIONT;

            return E_FAIL;
        }

		   nSendSize += dwOutLength; dwOutLength = dwSize - nSendSize;
    }

    if( dwOutLengthSent != NULL ) *dwOutLengthSent = nSendSize;

    //printf( "Send data success len %d\n",dwSize);

    return S_OK;
}

