/**
* @file	hvsocket.h
* @version	1.0
* @brief	socket基本函数接口
*/

#ifndef _HV_SOCKET_H_
#define _HV_SOCKET_H_

#include "hvutils.h"

// TODO: 根据不同平台包含头文件及参数结构体定义
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define hv_sockaddr sockaddr
#define hv_sockaddr_in sockaddr_in
#define HV_SOCKET_HANDLE  int /**< Socket句柄定义 */

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif

/**
*  HiVideo名字空间
*/
namespace HiVideo
{
	/**
	* @brief	creates a socket that is bound to a specific service provider
	* @param af [in] Address family specification
	* @param type [in] Type specification for the new socket, SOCK_STREAM / SOCK_DGRAM
	* @param protocol [in] Protocol to be used with the socket that is specific to the indicated address family
	* @return If no error occurs, socket returns a descriptor referencing the new socket. Otherwise, a value of -1 is returned
	*/
	HV_SOCKET_HANDLE Socket(int af, int type, int protocol);

	/**
	* @brief	Establishes a connection to a specified socket
	* @param	 s [in] Descriptor identifying an unconnected socket.
	* @param name [in] Address to assign to the socket from the sockaddr structure
	* @param namelen [in] Length of name, in bytes
	* @return If no error occurs, connect returns zero. Otherwise, it returns -1
	*/
	int Connect(HV_SOCKET_HANDLE s, const struct hv_sockaddr* name, int namelen);

	/**
	* @brief	Associates a local address with a socket
	* @param	 s [in] Descriptor identifying an unconnected socket.
	* @param name [in] Address to assign to the socket from the sockaddr structure
	* @param namelen [in] Length of name, in bytes
	* @return If no error occurs, connect returns zero. Otherwise, it returns -1
	*/
	int Bind(HV_SOCKET_HANDLE s,  const struct hv_sockaddr* name, int namelen);

	/**
	* @brief	Places a socket in a state in which it is listening for an incoming connection
	* @param s [in] Descriptor identifying an unconnected socket.
	* @param backlog [in] Maximum length of the queue of pending connections.
	* @return If no error occurs, connect returns zero. Otherwise, it returns -1
	*/
	int Listen(HV_SOCKET_HANDLE s, int backlog);

	/**
	* @brief	Permits an incoming connection attempt on a socket
	* @param	 addr [out] Optional pointer to a buffer that receives the address of the connecting entity, as known to the communications layer. The exact format of the addr parameter is determined by the address family established when the socket was created.
	* @param addrlen [out] Optional pointer to an integer that contains the length of addr
	* @return If no error occurs, this function returns a value of type SOCKET that is a descriptor for the new socket. This returned value is a handle for the socket on which the actual connection is made
	*/
	HV_SOCKET_HANDLE Accept(HV_SOCKET_HANDLE s,	struct hv_sockaddr* addr, int* addrlen);

	/**
	* @brief	Sends data on a connected socket
	* @param	 s [in] Descriptor identifying an unconnected socket
	* @param buf [in] Buffer containing the data to be transmitted
	* @param len [in] Length of the data in buf, in bytes
	* @param flags [in] Indicator specifying the way in which the call is made.
	* @return If no error occurs, send returns the total number of bytes sent, which can be less than the number indicated by len. Otherwise, a value of -1 is returned
	*/
	int Send(HV_SOCKET_HANDLE s, const char* buf, int len, int flags);

	/**
	* @brief Receives data from a connected or bound socket
	* @param s [in] Descriptor identifying an unconnected socket
	* @param buf [in] Buffer for the incoming data
	* @param len [in] Length of the data in buf, in bytes
	* @param flags [in] Indicator specifying the way in which the call is made.
	* @return If no error occurs, recv returns the number of bytes received. If the connection has been gracefully closed, the return value is zero. Otherwise, a value of -1 is returned
	*/
	int Recv(HV_SOCKET_HANDLE s, char* buf, int len, int flags);

	/**
	* @brief	Sends data to a specific destination
	* @param	 s [in] Descriptor identifying a (possibly connected) socket
	* @param buf [in] Buffer containing the data to be transmitted
	* @param len [in] Length of the data in buf, in bytes
	* @param flags [in] Indicator specifying the way in which the call is made.
	* @param to [in] Optional pointer to a sockaddr structure that contains the address of the target socket
	* @param tolen [in] Size of the address in to, in bytes
	* @return If no error occurs, send returns the total number of bytes sent, which can be less than the number indicated by len. Otherwise, a value of -1 is returned
	*/
	int SendTo(HV_SOCKET_HANDLE s, const char* buf, int len, int flags, const struct hv_sockaddr* to, int tolen);

	/**
	* @brief	Receives a datagram and stores the source address
	* @param	 s [in] Descriptor identifying a bound socket
	* @param buf [out] Buffer for the incoming data.
	* @param len [in] Length of buf, in bytes
	* @param flags [in] Indicator specifying the way in which the call is made.
	* @param from [out] Optional pointer to a buffer in a sockaddr structure that will hold the source address upon return
	* @param fromlen [in, out] Optional pointer to the size, in bytes, of the from buffer
	* @return If no error occurs, recvfrom returns the number of bytes received. If the connection has been gracefully closed, the return value is zero. Otherwise, a value of -1 is returned
	*/
	int RecvFrom(HV_SOCKET_HANDLE s, char* buf, int len, int flags, struct hv_sockaddr* from, int* fromlen);

	/**
	* @brief	Closes an existing socket
	* @param	 s [in] Descriptor identifying an unconnected socket
	* @return If no error occurs, connect returns zero. Otherwise, it returns -1
	*/
	int CloseSocket(HV_SOCKET_HANDLE s);

	/**
	* @brief	Sets a socket option
	* @param	 s [in] Descriptor identifying an unconnected socket
	* @param level [in] Level at which the option is defined
	* @param optname [in] Socket option for which the value is to be set
	* @param optval [in] Pointer to the buffer in which the value for the requested option is specified
	* @param optlen [in] Size of the optval buffer, in bytes.
	* @return If no error occurs, connect returns zero. Otherwise, it returns -1
	*/
	int SetSockOpt(HV_SOCKET_HANDLE s, int level, int optname, const char* optval, int optlen);

	/**
	* @brief	Retrieves the name of the peer to which a socket is connected
	* @param	 s [in] Descriptor identifying an unconnected socket
	* @param name [out] The hv_sockaddr structure that receives the name of the peer.
	* @param namelen [in, out] Pointer to the size of the name structure, in bytes
	* @return If no error occurs, connect returns zero. Otherwise, it returns -1
	*/
	int GetPeerName(HV_SOCKET_HANDLE s, struct hv_sockaddr* name, int* namelen);

	/**
	* @brief	Retrieves the local name for a socket
	* @param	 s [in] Descriptor identifying an unconnected socket
	* @param name [out] The hv_sockaddr structure that receives the name of the peer.
	* @param namelen [in, out] Pointer to the size of the name structure, in bytes
	* @return If no error occurs, connect returns zero. Otherwise, it returns -1
	*/
	int GetSockName(HV_SOCKET_HANDLE s, struct hv_sockaddr* name, int* namelen);

	/**
	* @brief Receives all data from a connected or bound socket
	* @param socket [in] Descriptor identifying an unconnected socket
	* @param pBuffer [in] Buffer for the incoming data
	* @param iRecvLen [in] Length of the data in buf, in bytes
	* @return If no error occurs, recv returns the number of bytes received. If the connection has been gracefully closed, the return value is zero. Otherwise, a value of -1 is returned
	*/
	inline int RecvAll(HV_SOCKET_HANDLE socket, char *pBuffer, int iRecvLen)
	{
		char *pTemp = pBuffer;
		int iRst = iRecvLen;

		while ( iRecvLen > 0 )
		{
			int iTemp = Recv( socket, pTemp, iRecvLen, 0 );
			if ( iTemp < 0 )
			{
				return iTemp;
			}
			if ( iTemp == 0 )
			{
				break;
			}
			pTemp += iTemp;
			iRecvLen -= iTemp;
		}
		return iRst - iRecvLen;
	}

	inline BOOL GetIpString(DWORD32 dwIP, LPSTR lpszIP)
    {
        if( lpszIP == NULL ) return FALSE;

        BYTE8* pByte=(BYTE8*)&dwIP;
        if( -1 == sprintf( lpszIP, "%d.%d.%d.%d", pByte[0],pByte[1],pByte[2],pByte[3]) )
        {
            return FALSE;
        }

        return TRUE;
    }
}

#endif
