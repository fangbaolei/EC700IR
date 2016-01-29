#ifndef _QTCPIP_H_
#define _QTCPIP_H_

#include <assert.h>
#include "os.h"

namespace QTCPIP {
	//任务函数指针定义
	struct _COMMON_PACKET;
	typedef void ( *PROCESS_FUN )( struct _COMMON_PACKET *pPacket );
	//
	struct _TCB;
	//全局数据包定义
	const int MAX_PACKET_LENGTH = 128 * 13 - 24;
	const WORD CDATA_TYPE_ETHER = 0;
	const WORD CDATA_TYPE_MSG = 1;
	typedef struct _COMMON_PACKET {
		struct _COMMON_PACKET *pFront, *pRear;
		union {
			PROCESS_FUN ProcessFun;
			DWORD resend_timeout;
			BYTE *pCopyPos;
		} u;
		_TCB *tcb_ptr;
		WORD tcp_data_signal;
		WORD tcp_data_attempts;
		WORD data_type;
		WORD data_len;
		WORD data[ MAX_PACKET_LENGTH / 2 ];
	} COMMON_PACKET;
	//

	//地址宽度
	const int ETHER_ADDR_SIZE = 6;
	const int IP_ADDR_SIZE = 4;

	//默认地址信息
	extern const DWORD32 DefaultIpAddr;
	extern const DWORD32 DefaultMask;
	extern const DWORD32 DefaultGateway;

	extern const BYTE8 ucDefaultMode;
	extern const WORD16 wDefaultLocalPort;
	extern const WORD16 wDefaultRemotePort;
	extern const DWORD32 dwDefaultRemoteAddr;
	
	extern BYTE8 ucgMode;
	extern WORD16 wgLocalPort;
	extern WORD16 wgRemotePort;
	extern DWORD32 dwgRemoteAddr;
	
	//地址转换
	BOOL MyGetIpDWord(LPSTR lpszIP, DWORD32& dwIP);
	
	BOOL MyGetIpString(DWORD32 dwIP, LPSTR lpszIP);

	//设置MAC地址
	extern int SetMacAddr( WORD16* rgMac);

	//获取MAC地址
	extern int GetMacAddr( WORD16* rgMac);

	//设置IP地址
	extern int SetTcpipAddr( DWORD32 dwIP, DWORD32 dwMask, DWORD32 dwGateway );
	extern int SetTcpipAddrSecond(DWORD32 CommIPAddr, DWORD32 CommMask);

	//取得IP地址
	extern int GetTcpipAddr( DWORD32 *pdwIP, DWORD32 *pdwMask, DWORD32 *pdwGateway );

	//取得标准协议参数
	extern int GetStdPtlParam( BYTE8 *pucgMode, WORD16 *pwgLocalPort, WORD16 *pwRemotePort, DWORD32 *pdwRemoteAddr );

	//设置标准协议参数
	extern int SetStdPtlParam( BYTE8 ucMode, WORD16 wLocalPort, WORD16 wRemotePort, DWORD32 dwRemoteAddr );
	
	//
	//以太网包定义
	const int ETHERNET_PACKET_MAXDATA = 1500;
	const int ETHERNET_PACKET_MINDATA = 46;
	const WORD EHTER_TYPE_ARP = 0x0806;
	const WORD ETHER_TYPE_IP = 0x0800;
	const WORD ETHER_BROADCAST_ADDR[ ETHER_ADDR_SIZE / 2 ] = { 0xffff, 0xffff, 0xffff };
	const WORD ETHER_HEAD_SIZE = 14;
	typedef struct _ETHERNET_PACKET {
		WORD d_mac[ ETHER_ADDR_SIZE / 2 ];
		WORD s_mac[ ETHER_ADDR_SIZE / 2 ];
		WORD next_protocol;
		WORD data[ ETHERNET_PACKET_MAXDATA / 2 ];
	} ETHERNET_PACKET;
	//
	//链表数据结构定义
	typedef struct _LIST {
		COMMON_PACKET *pHead;
	} LIST;
	//
	//在链表的某个位置的前部插入数据包;
	//pList表示要插入数据的链表;
	//pPosition表示要插入的位置;
	//pPacket表示要被插入的数据包;
	inline void ListInsertPacketFront( LIST *pList, COMMON_PACKET *pPosition, COMMON_PACKET *pPacket ) {
		assert((pList != NULL) && (pPacket != NULL));
		if ( pList->pHead == NULL ) {
			pPacket->pFront = pPacket;
			pPacket->pRear = pPacket;
			pList->pHead = pPacket;
		} else {
			pPosition->pFront->pRear = pPacket;
			pPacket->pFront = pPosition->pFront;
			pPosition->pFront = pPacket;
			pPacket->pRear = pPosition;
			if ( pList->pHead == pPosition ) {
				pList->pHead = pPacket;
			}
		}
	}
	//
	//在链表的某个位置的后部插入数据包;
	//pList表示要插入数据的链表;
	//pPosition表示要插入的位置;
	//pPacket表示要被插入的数据包;
	inline void ListInsertPacketRear( LIST *pList, COMMON_PACKET *pPosition, COMMON_PACKET *pPacket ) {
		assert((pList != NULL) && (pPacket != NULL));
		if ( pList->pHead == NULL ) {
			pPacket->pFront = pPacket;
			pPacket->pRear = pPacket;
			pList->pHead = pPacket;
		} else {
			pPosition->pRear->pFront = pPacket;
			pPacket->pRear = pPosition->pRear;
			pPosition->pRear = pPacket;
			pPacket->pFront = pPosition;
		}
	}
	//删除链表指定位置的数据包
	//pList表示要删除数据的链表;
	//pPosition表示要删除的位置;
	inline void ListDeletePacket( LIST *pList, COMMON_PACKET *pPosition ) {
		assert((pList != NULL) && (pPosition != NULL));
		if ( pPosition->pFront == pPosition ) {
			pList->pHead = NULL;
		} else {
			pPosition->pFront->pRear = pPosition->pRear;
			pPosition->pRear->pFront = pPosition->pFront;
			if ( pList->pHead == pPosition ) {
				pList->pHead = pPosition->pRear;
			}
		}
	}
	//分配一个数据包
	COMMON_PACKET *AllocPacket();
	//TCP数据块回收
	COMMON_PACKET *TCPReclaim( COMMON_PACKET *packet_ptr );
	//回收一个数据包
	void FreePacket( COMMON_PACKET *pPacket );
	
	//发送任务
	void SendMission( COMMON_PACKET *pPacket );
	//
	//ICMP输入函数
	void ICMPInput( COMMON_PACKET *packet_ptr );
	//
	//UDP输入函数
	void UDPInput( COMMON_PACKET *packet_ptr );
	//
	//UDP广播包处理函数
	void UDPGetIP( COMMON_PACKET *packet_ptr );
	//
	void UDPMainModule(COMMON_PACKET *packet_ptr);
	//
	//TCP输入函数
	void TCPMainModule( COMMON_PACKET *packet_ptr );
	//
	//ARP输入函数
	void ARPInput( COMMON_PACKET *packet_ptr );
	//
	//IP输入函数
	void IPInput( COMMON_PACKET *packet_ptr );
	//
	//TCP控制块定义
	typedef struct _TCB {
		DWORD state;
		
		//被阻塞的用户消息包
		COMMON_PACKET *user_packet_ptr;

		WORD local_port;
		DWORD local_addr;
		WORD remote_port;
		DWORD remote_addr;

		DWORD sending_sn;
		DWORD remote_ack_sn;
		WORD remote_window;

		DWORD receiving_sn;
		WORD local_window;

		DWORD connect_timeout;
		DWORD listen_timeout;
		DWORD close_timeout;
		DWORD remote_ack_timeout;

		//预先接收队列
		LIST wait_input_queue;
		//数据接收队列
		LIST input_queue;
		//数据接收指针
		BYTE *input_data_ptr;

		//数据发送指针
		BYTE *output_data_ptr;

		//数据重发队列
		LIST resend_queue;
	} TCB;
	typedef TCB *TCP_HANDLE;

	//TCP应用程序接口
	
	//TCP主动连接, 返回连接句柄，返回NULL表示失败。
	TCP_HANDLE TCPConnect( DWORD remote_addr, WORD remote_port );
	//TCP侦听,返回连接句柄, 返回NULL表示超时。
	//timeout输入为0表示无限等待。
	TCP_HANDLE TCPListen( WORD local_port, int timeout = 0 );
	//查询连接的远端地址端口和本机地址端口
	int TCPGetAddrPort( TCP_HANDLE tcp_handle, DWORD *pRemoteAddr, WORD *pRemotePort, DWORD *pLocalAddr, WORD *pLocalPort );
	//TCP发送数据,返回实际接收到的数据。如果tcp_handle无效则返回-1，如果对端已经关闭则返回-2。
	int TCPSend( TCP_HANDLE tcp_handle, BYTE *data_ptr, int data_len );
	//TCP接收数据,返回接收到的字节.如果tcp_handle无效则返回-1，
	//如果对端已经关闭则返回-2,超时返回-3。
	//timeout输入为0表示无限等待。
	int TCPReceive( TCP_HANDLE tcp_handle, BYTE *data_ptr, int data_len, int timeout = 0);
	//接收数据是否已经准备好
	bool TCPRecvReady( TCP_HANDLE tcp_handle );
	//TCP关闭连接。
	void TCPClose( TCP_HANDLE tcp_handle );

	
	//UDP控制块定义
	typedef struct _UCB {
		DWORD state;
		WORD local_port;
		DWORD local_addr;
		//被阻塞的用户消息包
		COMMON_PACKET *user_packet_ptr;
		//数据接收队列
		LIST input_queue;
		int buffer_count;
	} UCB;
	typedef UCB *UDP_HANDLE;
	
	
	//UDP应用程序接口
	
	//UDP绑定本地端口
	UDP_HANDLE UDPConnect(WORD wLocalPort, DWORD dwLocalAddrIndex = 0);
	//UDP发送数据
	int UDPSend(UDP_HANDLE hUDP, BYTE *pData, int iDataLen, DWORD dwRemoteAddr, WORD wRemotePort);
	//UDP接收数据
	int UDPReceive(UDP_HANDLE hUDP, BYTE *pBuffer, int iBufferLen, DWORD *pRemoteAddr, WORD *pRemotePort, int iTimeout);
	//UDP关闭端口
	void UDPClose(UDP_HANDLE hUDP);

	//
	//初始化TCPIP协议
	void InitTCPIP();
	//
}

void ShowTCB();
int GetAllConnectedIP(DWORD32* pdwCount, DWORD32* pdwIP, WORD16* pwPort);

inline int SaveRecv( QTCPIP::TCP_HANDLE hHandle, void *pBuf, int iLen, int iTimeout = 2000) {
	int iTemp, iCount;
	QTCPIP::BYTE *pTemp = reinterpret_cast< QTCPIP::BYTE* >( pBuf );

	iCount = 0;
	do {
		if ( ( iTemp = QTCPIP::TCPReceive( hHandle, &pTemp[ iCount ], iLen - iCount, iTimeout ) ) <= 0 )
		{
		 	return iTemp;
		}
		iCount += iTemp;
	} while ( iCount < iLen );
	return iLen;
}

#define SaveSend( hHandle, pBuf, iLen ) QTCPIP::TCPSend( reinterpret_cast< QTCPIP::TCP_HANDLE >( hHandle ), reinterpret_cast< QTCPIP::BYTE* >( pBuf ), iLen )

#endif
