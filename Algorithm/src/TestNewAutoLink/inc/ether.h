#ifndef __ETHER_H__
#define __ETHER_H__

#include "swBaseType.h"
#include "qtcpip.h"

namespace QTCPIP {

#define ETHERNET_ERROR_NO_MAC					-1
#define ETHERNET_ERROR_ALLOC_BUF				-2
	
#define ETHERNET_CTRL_INT_SEL					0x0000000F
	
//以太网初始化函数
int EthernetInit( BYTE8 *pbMacAddress, DWORD32 dwCtrl );
int EthernetOutput( COMMON_PACKET *pPacket );

}

#endif		// #ifndef __ETHER_H__

