#include "SWUDPSocket.h"
#include "SWString.h"
#include "SWLog.h"

CSWUDPSocket::CSWUDPSocket(const CHAR* outRemoteAddr ,const WORD wRemotePort,
                           const CHAR* stLocalAddr , const WORD wLocalPort )
{
    swpa_memset(&m_objRemoteAddr, 0, sizeof(SWPA_SOCKADDR));
	swpa_memset(&m_objAddr, 0, sizeof(SWPA_SOCKADDR));

    if( outRemoteAddr != NULL )
	{
        swpa_strncpy( m_objRemoteAddr.ip , outRemoteAddr , sizeof(m_objRemoteAddr.ip)-1);
	}
    m_objRemoteAddr.port = wRemotePort;

    if( stLocalAddr != NULL )
	{
        swpa_strncpy( m_objAddr.ip, stLocalAddr , sizeof(m_objAddr.ip)-1);
	}
    m_objAddr.port = wLocalPort;
}


CSWUDPSocket::CSWUDPSocket()
{
	swpa_memset(&m_objRemoteAddr, 0, sizeof(SWPA_SOCKADDR));
	swpa_memset(&m_objAddr, 0, sizeof(SWPA_SOCKADDR));
}

CSWUDPSocket::~CSWUDPSocket()
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
HRESULT CSWUDPSocket::Create( ){
    return CSWSocket::Create( SWPA_SOCK_DGRAM );
}
/**
 * @brief UDP数据传输
 *
 * @param [LPCVOID] pbBuf : 需要传输的数据缓存
 * @param [DWORD] dwSize : 需要传输的数据大小
 * @param [PDWORD] inRemoteAddr : 传输目的IP地址
 * @param [PDWORD] inRemotePort : 传输目的IP端口
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWUDPSocket::SendTo(const VOID* pbBuf, const DWORD dwSize
                          , const CHAR* inRemoteAddr, const DWORD inRemotePort , DWORD *dwSendlen ){

    if( NULL == pbBuf || 0 == dwSize) return E_FAIL;

    *dwSendlen = 0;
    struct SWPA_SOCKADDR dest_addr;

    bzero(&dest_addr,sizeof(SWPA_SOCKADDR));

    if( inRemoteAddr != NULL )
        swpa_memcpy( dest_addr.ip, inRemoteAddr , swpa_strlen(inRemoteAddr));

    dest_addr.port = inRemotePort;

    return swpa_socket_sendto(m_objSockek, pbBuf ,
                                dwSize, dwSendlen ,
                                 &dest_addr , sizeof(SWPA_SOCKADDR));
}
/**
 * @brief UDP数据传输
 *
 * @param [LPCVOID] pbBuf : 需要传输的数据缓存
 * @param [DWORD] dwSize : 需要传输的数据大小
 * @param [out] dwSendlen : 实际传输的数据大小
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWUDPSocket::SendTo(const VOID* pbBuf, const DWORD dwSize,  DWORD *dwSendlen)
{
    return this->SendTo( pbBuf, dwSize, (CHAR*)m_objRemoteAddr.ip
                   , m_objRemoteAddr.port , dwSendlen );
}

HRESULT CSWUDPSocket::RecvFrom(const CHAR* outSocketAddr, const WORD ouSocketPort,
                        VOID* ioBuffer, const DWORD inBufLen, DWORD* outRecvLen){


    DWORD nfromlen = sizeof(SWPA_SOCKADDR);
    *outRecvLen = inBufLen;

    struct SWPA_SOCKADDR dest_addr;

    swpa_memset(&dest_addr,0 , sizeof(SWPA_SOCKADDR));

    if( outSocketAddr != NULL )
        swpa_memcpy( dest_addr.ip, outSocketAddr , swpa_strlen(outSocketAddr));

    dest_addr.port = ouSocketPort;


    INT h = swpa_socket_recvfrom(m_objSockek, ioBuffer ,
                            outRecvLen,
                             &dest_addr , &nfromlen );
	swpa_memset( m_objRemoteAddr.ip, 0 , sizeof(m_objRemoteAddr.ip));
    swpa_strncpy( m_objRemoteAddr.ip, dest_addr.ip , sizeof(m_objRemoteAddr.ip)-1);

    m_objRemoteAddr.port = dest_addr.port;

    return h == 0 ? S_OK : E_FAIL;
}

HRESULT CSWUDPSocket::RecvFrom( VOID* ioBuffer, const DWORD inBufLen, DWORD* outRecvLen)
{
   return this->RecvFrom((CHAR*)m_objAddr.ip , m_objAddr.port,
                        ioBuffer, inBufLen, outRecvLen);
}



HRESULT CSWUDPSocket::GetPeerName(char* szName , WORD *pwPort )
{
	if (NULL != szName)
	{
		swpa_strncpy(szName, m_objRemoteAddr.ip, sizeof(m_objRemoteAddr.ip));
	}

	if (NULL != pwPort)
	{
		*pwPort = (WORD)m_objRemoteAddr.port;
	}	

	return S_OK;
}


HRESULT CSWUDPSocket::Broadcast(const WORD wDstPort, const BYTE *pbBuf, const DWORD dwSize)
{
	if (NULL == pbBuf || 0 == dwSize)
	{
		SW_TRACE_DEBUG("Err: NULL == pbBuf || 0 == dwSize\n");
		return E_FAIL;
	}
	
	BOOL fOn = 1;
    INT iOpt = 1;
	if (SWPAR_OK != swpa_socket_opt( m_objSockek , SWPA_SOL_SOCKET , SWPA_SO_BROADCAST, (VOID*)&fOn , sizeof(fOn))
		|| SWPAR_OK != swpa_socket_opt( m_objSockek , SWPA_SOL_SOCKET , SWPA_SO_REUSEADDR, (VOID*)&iOpt , sizeof(iOpt)))
	{
		SW_TRACE_DEBUG("Err: failed to set opt SWPA_SO_BROADCAST, SWPA_SO_REUSEADDR\n");
	    return E_FAIL;
	}
	
	
	DWORD dwSendLen = 0;
	if (FAILED(SendTo((VOID*)pbBuf, dwSize, "255.255.255.255", wDstPort, &dwSendLen))//broadcast
		|| 0 == dwSendLen)
	{
		#define NET_INFO_STR_LEN 32
		CHAR szIp[NET_INFO_STR_LEN] = {0};
		CHAR szMask[NET_INFO_STR_LEN] = {0};
		CHAR szGateway[NET_INFO_STR_LEN] = {0};
		CHAR szMAC[NET_INFO_STR_LEN] = {0};

		swpa_tcpip_getinfo("eth0",
			szIp, NET_INFO_STR_LEN-1,
			szMask, NET_INFO_STR_LEN-1,
			szGateway, NET_INFO_STR_LEN-1,
			szMAC, NET_INFO_STR_LEN-1);
		
		CSWString strCmd;
		strCmd.Format("route add default gw %s", szIp);
		int ret = swpa_utils_shell(strCmd, NULL);
		if (SWPAR_OK != ret) 
		{
			SW_TRACE_DEBUG("Err: failed to add %s to default gateway\n", szIp);
			return E_FAIL;
		}
		HRESULT hr = SendTo((VOID*)pbBuf, dwSize, "255.255.255.255", wDstPort, &dwSendLen); //try again
		strCmd.Format("route del default gw %s", szIp);
		ret = swpa_utils_shell(strCmd, NULL);
		return hr;
	}
	
	return S_OK;
}


