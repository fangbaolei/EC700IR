#include "stdafx.h"
#include "TestNetWork.h"

#define ICMP_ECHO 8
#define ICMP_ECHOREPLY 0
#define ICMP_MIN 8
#define STATUS_FAILED 0xFFFF
#define DEF_PACKET_SIZE 32
#define DEF_PACKET_NUM 4
#define MAX_PACKET 1024
#define xmalloc(s) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (s))
#define xfree(p) HeapFree(GetProcessHeap(), 0, (p))

typedef struct iphdr
{
	unsigned int h_len:4;
	unsigned int version:4;
	unsigned char tos;
	unsigned short total_len;
	unsigned short ident;
	unsigned short frag_and_flag;
	unsigned char ttl;
	unsigned char proto;
	unsigned short checksum;
	unsigned int sourceip;
	unsigned int destip;
}IpHeader;

typedef struct icmphdr
{
	BYTE i_type;
	BYTE i_code;
	USHORT i_cksum;
	USHORT i_id;
	USHORT i_seq;
	ULONG timestamp;
}IcmpHeader;

CTestNetWork::CTestNetWork()
{
	m_fIsTesting = FALSE;
}

CTestNetWork::~CTestNetWork()
{
	while(m_fIsTesting)
	{
		Sleep(1000);
	}
	WSACleanup();
}

BOOL CTestNetWork::InitSock()
{
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 1), &wsaData) != 0)
	{
		return FALSE;
	}
	return TRUE;
}

void CTestNetWork::StartTest(char* pszIP, int& iSucessTimes)
{
	m_fIsTesting = TRUE;
	iSucessTimes = 0;
	struct sockaddr_in dest, from;
	int bread, datasize, times;
	int fromlen = sizeof(from);
	int timeout = 1000;
	char* dest_ip;
	char* icmp_data;
	char* recvbuf;
	unsigned int addr = 0;
	USHORT seq_no = 0;
	SOCKET sockRaw;
	sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(sockRaw == INVALID_SOCKET)
	{
		m_fIsTesting = FALSE;
		return;
	}
	bread = setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	if(bread == SOCKET_ERROR)
	{
		closesocket(sockRaw);
		m_fIsTesting = FALSE;
		return;
	}
	timeout = 1000;
	bread = setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
	if(bread == SOCKET_ERROR)
	{
		closesocket(sockRaw);
		m_fIsTesting = FALSE;
		return;
	}
	memset(&dest, 0, sizeof(dest));
	addr = inet_addr(pszIP);
	dest.sin_addr.S_un.S_addr = addr;
	dest.sin_family = AF_INET;
	dest_ip = inet_ntoa(dest.sin_addr);
	times = DEF_PACKET_NUM;
	datasize = DEF_PACKET_SIZE;
	datasize += sizeof(IcmpHeader);
	icmp_data = (char*)xmalloc(MAX_PACKET);
	recvbuf = (char*)xmalloc(MAX_PACKET);
	if(!icmp_data || !recvbuf)
	{
		closesocket(sockRaw);
		if(icmp_data)
		{
			xfree(icmp_data);
			icmp_data = NULL;
		}
		if(recvbuf)
		{
			xfree(recvbuf);
			recvbuf = NULL;
		}
		m_fIsTesting = FALSE;
		return;
	}
	memset(icmp_data, 0, MAX_PACKET);
	FillICMPPacket(icmp_data, datasize);
	int iTimes = 0;
	for(int i=0; i<DEF_PACKET_NUM; i++)
	{
		int bwrote;
		((IcmpHeader*)icmp_data)->i_cksum = 0;
		((IcmpHeader*)icmp_data)->timestamp = GetTickCount();
		((IcmpHeader*)icmp_data)->i_seq = seq_no++;
		((IcmpHeader*)icmp_data)->i_cksum = CheckSun((USHORT*)icmp_data, datasize);
		bwrote = sendto(sockRaw, icmp_data, datasize, 0, (struct sockaddr*)&dest, sizeof(dest));
		if(bwrote == SOCKET_ERROR)
		{
			if(WSAGetLastError() == WSAETIMEDOUT)
			{
				continue;
			}
			closesocket(sockRaw);
			xfree(icmp_data);
			icmp_data = NULL;
			xfree(recvbuf);
			recvbuf = NULL;
			m_fIsTesting = FALSE;
			return;
		}
		bread = recvfrom(sockRaw, recvbuf, MAX_PACKET, 0, (struct sockaddr*)&from, &fromlen);
		if(bread == SOCKET_ERROR)
		{
			if(WSAGetLastError() == WSAETIMEDOUT)
			{
				continue;
			}
			closesocket(sockRaw);
			xfree(icmp_data);
			icmp_data = NULL;
			xfree(recvbuf);
			recvbuf = NULL;
			m_fIsTesting = FALSE;
			return;
		}

		if(!DecodeResp(recvbuf, bread, &from))iTimes++;
		Sleep(200);
	}
	closesocket(sockRaw);
	xfree(icmp_data);
	icmp_data = NULL;
	xfree(recvbuf);
	recvbuf = NULL;
	iSucessTimes = iTimes;
	m_fIsTesting = FALSE;
}

void CTestNetWork::FillICMPPacket(char* pPacketPointer, int iPacketSize)
{
	IcmpHeader* icmp_hdr;
	char* datapart;
	icmp_hdr = (IcmpHeader*)pPacketPointer;
	icmp_hdr->i_type = ICMP_ECHO;
	icmp_hdr->i_code = 0;
	icmp_hdr->i_id = (USHORT)GetCurrentProcessId();
	icmp_hdr->i_cksum = 0;
	icmp_hdr->i_seq = 0;
	datapart = pPacketPointer + sizeof(IcmpHeader);
	memset(datapart, 'E', iPacketSize - sizeof(IcmpHeader));
}

int CTestNetWork::DecodeResp(char* pBuf, int iSize, struct sockaddr_in* cFromAddr)
{
	IpHeader* iphdr;
	IcmpHeader* icmphdr;
	unsigned short iphdrlen;
	iphdr = (IpHeader*)pBuf;
	iphdrlen = iphdr->h_len * 4;
	if(iSize < iphdrlen + ICMP_MIN)
	{
		return 1;
	}
	icmphdr = (IcmpHeader*)(pBuf + iphdrlen);
	if(icmphdr->i_type != ICMP_ECHOREPLY)
	{
		return 1;
	}
	if(icmphdr->i_id != (USHORT)GetCurrentProcessId())
	{
		return 1;
	}
	return 0;
}

USHORT CTestNetWork::CheckSun(USHORT* pData, int iDataSize)
{
	unsigned long cksum = 0;
	while(iDataSize > 1)
	{
		cksum += *pData++;
		iDataSize -= sizeof(USHORT);
	}
	if(iDataSize)
	{
		cksum += *(UCHAR*)pData;
	}
	cksum = (cksum >> 16) + (cksum & 0xFFFF);
	cksum += (cksum >> 16);
	return (USHORT)(~cksum);
}

