#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

// 获取IP地址，子网掩码，MAC地址
int GetLocalNetInfo(
    const char* lpszEth,
    char* szIpAddr,
    char* szNetmask,
    char* szMacAddr
)
{
    int ret = 0;

    struct ifreq req;
    struct sockaddr_in* host = NULL;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( -1 == sockfd )
    {
        return -1;
    }

    bzero(&req, sizeof(struct ifreq));
    strcpy(req.ifr_name, lpszEth);
    if ( ioctl(sockfd, SIOCGIFADDR, &req) >= 0 )
    {
        host = (struct sockaddr_in*)&req.ifr_addr;
        strcpy(szIpAddr, inet_ntoa(host->sin_addr));
    }
    else
    {
        ret = -1;
    }

    bzero(&req, sizeof(struct ifreq));
    strcpy(req.ifr_name, lpszEth);
    if ( ioctl(sockfd, SIOCGIFNETMASK, &req) >= 0 )
    {
        host = (struct sockaddr_in*)&req.ifr_addr;
        strcpy(szNetmask, inet_ntoa(host->sin_addr));
    }
    else
    {
        ret = -1;
    }

    bzero(&req, sizeof(struct ifreq));
    strcpy(req.ifr_name, lpszEth);
    if ( ioctl(sockfd, SIOCGIFHWADDR, &req) >= 0 )
    {
        sprintf(
            szMacAddr, "%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned char)req.ifr_hwaddr.sa_data[0],
            (unsigned char)req.ifr_hwaddr.sa_data[1],
            (unsigned char)req.ifr_hwaddr.sa_data[2],
            (unsigned char)req.ifr_hwaddr.sa_data[3],
            (unsigned char)req.ifr_hwaddr.sa_data[4],
            (unsigned char)req.ifr_hwaddr.sa_data[5]
        );
    }
    else
    {
        ret = -1;
    }

    if ( sockfd != -1 )
    {
        close(sockfd);
        sockfd = -1;
    }

    return ret;
}

// ------------------------------------------------------

#define BUFSIZE 8192

struct route_info
{
    char ifName[IF_NAMESIZE];
    u_int gateWay;
    u_int srcAddr;
    u_int dstAddr;
};

static int ReadNlSock(
    int sockFd,
    char* bufPtr,
    int seqNum,
    int pId
)
{
    struct nlmsghdr* nlHdr = NULL;
    int readLen = 0, msgLen = 0;

    while (true)
    {
        if ( (readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0 )
        {
            return -1;
        }

        nlHdr = (struct nlmsghdr *)bufPtr;

        if ( (NLMSG_OK(nlHdr, (unsigned int)readLen) == 0)
                || (nlHdr->nlmsg_type == NLMSG_ERROR) )
        {
            return -1;
        }

        if ( nlHdr->nlmsg_type == NLMSG_DONE )
        {
            break;
        }
        else
        {
            bufPtr += readLen;
            msgLen += readLen;
        }

        if ( (nlHdr->nlmsg_flags & NLM_F_MULTI) == 0 )
        {
            break;
        }

        if ( (nlHdr->nlmsg_seq != (unsigned int)seqNum)
                || (nlHdr->nlmsg_pid != (unsigned int)pId) )
        {
            break;
        }
    }

    return msgLen;
}

static int ParseRoutes(
    struct nlmsghdr* nlHdr,
    struct route_info* rtInfo,
    char* default_gateway
)
{
    int rtLen = 0;
    struct in_addr dst;
    struct in_addr gate;
    struct rtmsg* rtMsg = NULL;
    struct rtattr* rtAttr = NULL;

    rtMsg = (struct rtmsg*)NLMSG_DATA(nlHdr);

    if ( (rtMsg->rtm_family != AF_INET)
            || (rtMsg->rtm_table != RT_TABLE_MAIN) )
    {
        return -1;
    }

    rtAttr = (struct rtattr*)RTM_RTA(rtMsg);
    rtLen = RTM_PAYLOAD(nlHdr);
    for ( ; RTA_OK(rtAttr, rtLen); rtAttr = RTA_NEXT(rtAttr, rtLen) )
    {
        switch (rtAttr->rta_type)
        {
        case RTA_OIF:
            if_indextoname(*(int*)RTA_DATA(rtAttr), rtInfo->ifName);
            break;
        case RTA_GATEWAY:
            rtInfo->gateWay = *(u_int*)RTA_DATA(rtAttr);
            break;
        case RTA_PREFSRC:
            rtInfo->srcAddr = *(u_int*)RTA_DATA(rtAttr);
            break;
        case RTA_DST:
            rtInfo->dstAddr = *(u_int*)RTA_DATA(rtAttr);
            break;
        }
    }

    dst.s_addr = rtInfo->dstAddr;
    if (strstr((char*)inet_ntoa(dst), "0.0.0.0"))
    {
        gate.s_addr = rtInfo->gateWay;
        strcpy(default_gateway, (char*)inet_ntoa(gate));
    }

    return 0;
}

// 获取默认网关
int GetDefaultGateway(
    const char* lpszEth,
    char* szDefaultGateway
)
{
    static char szGatewayTemp[32] = {0};
    static char msgBuf[BUFSIZE] = {0};
    static struct route_info ri;

    int ret = -1;

    struct nlmsghdr* nlMsg = NULL;
    struct rtmsg* rtMsg = NULL;
    struct route_info* rtInfo = &ri;

    int len = 0, msgSeq = 0;

    int sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
    if ( -1 == sock )
    {
        goto END;
    }

    nlMsg = (struct nlmsghdr*)msgBuf;
    rtMsg = (struct rtmsg*)NLMSG_DATA(nlMsg);

    nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    nlMsg->nlmsg_type = RTM_GETROUTE;
    nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;
    nlMsg->nlmsg_seq = msgSeq++;
    nlMsg->nlmsg_pid = getpid();

    if ( send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0 )
    {
        goto END;
    }

    if ( (len = ReadNlSock(sock, msgBuf, msgSeq, getpid())) < 0 )
    {
        goto END;
    }

    if ( rtInfo != NULL )
    {
        for ( ; NLMSG_OK(nlMsg, (unsigned int)len); nlMsg = NLMSG_NEXT(nlMsg, len) )
        {
            memset(szGatewayTemp, 0, 32);
            memset(rtInfo, 0, sizeof(struct route_info));
            if ( 0 == ParseRoutes(nlMsg, rtInfo, szGatewayTemp) )
            {
                if ( strcmp(rtInfo->ifName, lpszEth) == 0
                        && strcmp(szGatewayTemp, "0.0.0.0") != 0
                        && strlen(szGatewayTemp) > 0 )
                {
                    strcpy(szDefaultGateway, szGatewayTemp);
                    ret = 0;
                }
            }
        }
    }

END:

    if ( sock != -1 )
    {
        close(sock);
        sock = -1;
    }

    return ret;
}

// ------------------------------------------------------

// 设置IP地址和子网掩码
int SetLocalNetInfo(
    const char* lpszEth,
    const char* lpszIpAddr,
    const char* lpszNetmask
)
{
    int ret = -1;

    struct ifreq req;
    struct sockaddr_in* host = NULL;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( -1 == sockfd )
    {
        goto END;
    }

    bzero(&req, sizeof(struct ifreq));
    strcpy(req.ifr_name, lpszEth);
    host = (struct sockaddr_in*)&req.ifr_addr;
    host->sin_family = AF_INET;
    if ( 1 != inet_pton(AF_INET, lpszIpAddr, &(host->sin_addr)) )
    {
        goto END;
    }
    if ( ioctl(sockfd, SIOCSIFADDR, &req) < 0 )
    {
        goto END;
    }

    bzero(&req, sizeof(struct ifreq));
    strcpy(req.ifr_name, lpszEth);
    host = (struct sockaddr_in*)&req.ifr_addr;
    host->sin_family = AF_INET;
    if ( 1 != inet_pton(AF_INET, lpszNetmask, &(host->sin_addr)) )
    {
        goto END;
    }
    if ( ioctl(sockfd, SIOCSIFNETMASK, &req) < 0 )
    {
        goto END;
    }

    ret = 0;

END:

    if ( sockfd != -1 )
    {
        close(sockfd);
        sockfd = -1;
    }

    return ret;
}

// 设置指定网口的mtu值
int SetMTU(const char* lpszEth, unsigned int mtu)
{
    int ret = -1;

	struct ifreq ifr;

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( -1 == sockfd )
    {
        goto END;
    }

	strcpy(ifr.ifr_name, lpszEth);
	ifr.ifr_ifru.ifru_mtu = mtu;
	if ( ioctl(sockfd, SIOCSIFMTU, &ifr) < 0 )
	{
		goto END;
	}

	ret = 0;

END:

    if ( sockfd != -1 )
    {
        close(sockfd);
        sockfd = -1;
    }

    return ret;
}

// 关闭指定网口
int SetIfDown(const char* lpszEth)
{
    int ret = -1;

	struct ifreq ifr;

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( -1 == sockfd )
    {
        goto END;
    }

	strcpy(ifr.ifr_name, lpszEth);

	if ( ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0 )
	{
		goto END;
	}

	ifr.ifr_flags &= ~IFF_UP;

	if ( ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0 )
	{
		goto END;
	}

	ret = 0;

END:

    if ( sockfd != -1 )
    {
        close(sockfd);
        sockfd = -1;
    }

    return ret;
}

// 打开指定网口
int SetIfUp(const char* lpszEth)
{
    int ret = -1;

	struct ifreq ifr;

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( -1 == sockfd )
    {
        goto END;
    }

	strcpy(ifr.ifr_name, lpszEth);

	if ( ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0 )
	{
		goto END;
	}

	ifr.ifr_flags |= IFF_UP;

	if ( ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0 )
	{
		goto END;
	}

	ret = 0;

END:

    if ( sockfd != -1 )
    {
        close(sockfd);
        sockfd = -1;
    }

    return ret;
}
