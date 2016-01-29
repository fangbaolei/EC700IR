#ifndef TCP_IP_CFG
#define TCP_IP_CFG

#include "hvutils.h"

// 注：主机字节序
inline BOOL MyGetIpString(DWORD32 dwIP, LPSTR lpszIP)
{
	if( lpszIP == NULL ) return FALSE;

	BYTE8* pByte=(BYTE8*)&dwIP;
	if( -1 == sprintf(lpszIP, "%d.%d.%d.%d", pByte[3],pByte[2],pByte[1],pByte[0]) )
	{
		return FALSE;
	}
	return TRUE;
}

// 注：主机字节序
inline BOOL MyGetIpDWord(LPSTR lpszIP, DWORD32& dwIP)
{
	if( lpszIP == NULL ) return FALSE;

	DWORD32 Temp[4]={0};
	if( 4 != sscanf(lpszIP,"%d.%d.%d.%d",Temp+3,Temp+2,Temp+1,Temp) )
	{
		return FALSE;
	}

	dwIP = (Temp[3]<<24)|(Temp[2]<<16)|(Temp[1]<<8)|Temp[0];
	return TRUE;
}

typedef struct _TcpipParam
{
	char szIp[32];
	char szNetmask[32];
	char szGateway[32];

	_TcpipParam()
	{
		sprintf(szIp, "100.100.100.101");
		sprintf(szNetmask, "255.255.0.0");
		sprintf(szGateway, "0.0.0.0");
	}
} TcpipParam;

HRESULT SetTcpipAddr_0(TcpipParam* pParam);
HRESULT SetTcpipAddr_1(TcpipParam* pParam);
HRESULT SetTcpipAddr_2(TcpipParam* pParam);
HRESULT CheckIPAddr(TcpipParam cTcpipCfgParam1, TcpipParam cTcpipCfgParam2);
void GetLocalTcpipAddr(const char* pszEth, char* pszIpAddr, char* pszNetmask, char* pszGateway, char* pszMac);

/* HvRoute.cpp */
extern int HvRoute(char* szArg);

/* HvIfconfig.cpp */
extern int GetLocalNetInfo(
    const char* lpszEth,
    char* szIpAddr,
    char* szNetmask,
    char* szMacAddr
);
extern int GetDefaultGateway(
    const char* lpszEth,
    char* szDefaultGateway
);
extern int SetLocalNetInfo(
    const char* lpszEth,
    const char* lpszIpAddr,
    const char* lpszNetmask
);
extern int SetMTU(const char* lpszEth, unsigned int mtu);
extern int SetIfDown(const char* lpszEth);
extern int SetIfUp(const char* lpszEth);

#endif

