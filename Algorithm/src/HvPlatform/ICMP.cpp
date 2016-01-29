#include "ICMP.h"

CRawSock:: CRawSock(int protocol )
{
    m_nSock = socket(AF_INET, SOCK_RAW, protocol);

    if ( m_nSock == -1 )
    {
        perror("create socket error");
        m_nError = 1;
    }
    else
    {
        struct timeval tv =
        {
            3, 0
        };
        setsockopt(m_nSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
        m_nError = 0;
    }
}

CRawSock:: ~CRawSock()
{
    close(m_nSock);
}

int CRawSock:: send(const void* msg, int msglen, struct sockaddr* to, unsigned int len)
{
    if (m_nError )
        return -1;
    int length = sendto(m_nSock, msg, msglen, 0, (const struct sockaddr *)to, len);
    if ( length == -1)
    {
        m_nError = 2;
        return -1;
    }
    return length;
}

int CRawSock:: send(const void* msg, int msglen, char *hostname)
{
    sockaddr_in sin;
    if (m_nError)
        return -1;

    if (hostname)
    {
        hostent *hostnm = gethostbyname(hostname);
        if ( hostnm == (struct hostent *)0)
        {
            return -1;
        }
        sin.sin_addr = *((struct in_addr *)hostnm->h_addr);
    }
    else
        return -1;
    sin.sin_family = AF_INET;
    return send(msg, msglen, (sockaddr *)&sin, sizeof(sin));
}

int CRawSock::receive(void *buf, int buflen, sockaddr* from, socklen_t *len)
{
    if (m_nError) return -1;
    while (1)
    {
        int length =recvfrom(m_nSock, buf, buflen, 0, from, len);
        if (length == -1)
            if ( m_nError == EINTR ) continue;
            else
            {
                m_nError = 3;
                return -1;
            }
        return length;
    }
}

/********************
* CICMP
*
*
* ********************/

CICMP::CICMP() : CRawSock(IPPROTO_ICMP)
{
    max_len = MAXICMPLEN;
    packet = (struct icmp *)new char [max_len];
    packet->icmp_code = 0;
    packet->icmp_id = 0;
    packet->icmp_seq = 0;
    packet->icmp_type = ICMP_ECHO;
}

CICMP::CICMP(int len) : CRawSock(IPPROTO_ICMP)
{
    max_len = len;
    packet = (struct icmp *) new char [max_len];
    packet->icmp_code = 0;
    packet->icmp_id = 0;
    packet->icmp_seq = 0;
    packet->icmp_type = ICMP_ECHO;
}

CICMP::~CICMP()
{
    delete [] (char *)packet;
}

uint16_t CICMP::checksum(uint16_t *addr, int len)
{
    int nleft = len;
    int sum =0;
    unsigned short *w = addr;
    unsigned short answer = 0;

    while (nleft > 1)
    {
        sum += *w++;
        nleft -=2;
    }

    if (nleft == 1)
    {
        *(unsigned char *) (&answer) = *(unsigned char *) w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return (answer);
}

int CICMP:: send_icmp(char *host, void *buf, int len)
{
    memcpy(packet->icmp_data, buf, len);
    packet->icmp_cksum =0;
    packet->icmp_cksum = checksum((uint16_t *)packet, ICMPHEAD + 6);
    int err = send(packet, MAXICMPLEN, host);
    return err;

}

int CICMP:: recv_icmp(sockaddr *from)
{
    char buf[MAXICMPLEN + 100];
    int hlen1, icmplen;
    struct ip *ip;
    struct icmp * pIcmp;

    if ( Error() )
    {
        printf("Error() = %d\n", Error());
        return -1;
    }

    socklen_t addrlen = 0;
    int len = receive(buf, MAXICMPLEN+100, from , &addrlen);

    if ( len == -1)
    {
        cout<<"Receive Failed\n";
        return -1;
    }

    ip = (struct ip *)buf;
    hlen1 = ip->ip_hl << 2;

    pIcmp = (struct icmp *) (buf + hlen1);

    if ( (icmplen = len -hlen1) < 8)
    {
        cout<<"Receive Fail\n";
        return -1;
    }

    memcpy(packet, pIcmp , icmplen);
    return 0;
}
