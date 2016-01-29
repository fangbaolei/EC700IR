#pragma once
#include "winsock2.h"
#pragma comment(lib, "WS2_32.lib")

class CTestNetWork
{
public:
	CTestNetWork();
	~CTestNetWork();
	BOOL InitSock();
	void StartTest(char* pszIP, int& iSucessTimes);

private:
	void FillICMPPacket(char* pPacketPointer, int iPacketSize);
	USHORT CheckSun(USHORT* pData, int iDataSize);
	int DecodeResp(char* pBuf, int iSize, struct sockaddr_in* cFromAddr);

private:
	BOOL m_fIsTesting;
};
