#ifndef __ICMP_H__
#define __ICMP_H__

#include <iostream>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/types.h>

#define ICMPHEAD 8
#define MAXICMPLEN 200
using namespace std;

class CRawSock
{
public:
    CRawSock(int protocol =0);
    virtual ~CRawSock();
    int send(const void* msg, int msglen, struct sockaddr* addr, unsigned int len);
    int send(const void* msg, int msglen, char *addr);
    int receive(void *buf, int buflen, sockaddr *from, socklen_t *len);
    int Error()
    {
        return m_nError;
    }
private:
    int m_nSock;
    int m_nError;
};

class CICMP : public CRawSock
{
public:
    CICMP();
    CICMP(int len);
    virtual ~CICMP();
    uint16_t checksum(uint16_t *addr, int len);
    int send_icmp(char *to, void *buf, int len);
    int recv_icmp(sockaddr *from);
    void setCode(int c)
    {
        packet->icmp_code =c;
    }
    void setId(int i)
    {
        packet->icmp_id =i;
    }
    void setSeq(int s)
    {
        packet->icmp_seq = s;
    }
    void setType(int t)
    {
        packet->icmp_type = t;
    }
public:
    struct icmp * packet;
    int max_len;
    int length;
};
#endif
