/***********************************************************************
** Copyright (C) GUOG. All rights reserved.
** Author			: guog
** Date				: 2013.09.17
** Name				: onvif_function.c
** Version			: 1.0
** Description			: onvif模块功能函数
** Modify Record		:
1:Creat on 2013.09.17

***********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//#include "onvif_function.h"



int netGetMac(char *pInterface, unsigned char *pMac)
{
	struct ifreq ifreq;
	int sockfd = 0;
	unsigned char mac[6] = {0}; 

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("netGetMac socket");
		return -1;
	}

	strcpy(ifreq.ifr_name, pInterface);

	if(ioctl(sockfd, SIOCGIFHWADDR, &ifreq) < 0)
	{
		perror("netGetMac ioctl");
		close(sockfd);
		return -2;
	}

	memcpy(mac, ifreq.ifr_hwaddr.sa_data, 6);
	printf("MAC:%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	if(pMac != NULL)
	{
		memcpy(pMac, mac, 6);
	}

	close(sockfd);

	return 0;
}


int netGetIp(char *pInterface, unsigned int *ip)
{
	int sock = 0;
	struct ifreq ifr;

	if((pInterface == NULL) || (*pInterface == '\0'))
	{
		printf("get ip: pInterface == NULL\r\n");
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, pInterface, IFNAMSIZ);
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock <= 0)
	{
		printf("get ip: sock error, %s\r\n", strerror(errno));
		return -1;
	}

	((struct sockaddr_in*)&ifr.ifr_addr)->sin_family = PF_INET;
	if(ioctl(sock, SIOCGIFADDR, &ifr) < 0)
	{
		printf("get ip error: %s\r\n", strerror(errno));
		close(sock);
		return -1;
	}
	else
	{
		*ip = ((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr.s_addr;
		printf("get ip(%d:%d:%d:%d) success!\r\n", (*ip)&0xff, (*ip>>8)&0xff, (*ip>>16)&0xff, (*ip>>24)&0xff);
	}
	close(sock);

	return 0;
}




