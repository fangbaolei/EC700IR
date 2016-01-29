#pragma once
#include "hvsocket.h"

// 图片广播端口号
#define SVR_BROADCAST_PORT 8989

#define RCV_TIMEOUT 2000			// 接收超时

class CBroadcastLink
{
public:
	CBroadcastLink();
	~CBroadcastLink();
	HRESULT GetImge(char* pcBuf, DWORD32* pdwSize, DWORD32* pdwTime, char* pszFileName);
	HRESULT Initialize();
private:
	HiVideo::ISocket* m_pSocket;
	DWORD32 m_dwRefTime;
};
