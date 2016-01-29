#ifndef _TESTLINK_H_
#define _TESTLINK_H_

#include "TestLinkOpt.h"
#include "hvsocket.h"
#include "hvinterface.h"

#define RCV_TIMEOUT 2000			// Ω” ’≥¨ ±

class CTestLink
{
public:
	CTestLink();
	~CTestLink();
	HRESULT Connect(const char* pszIP);	
	HRESULT GetImge(char* pcBuf, DWORD32* pdwSize, DWORD32* pdwTime, char* pszFileName);
	HRESULT Close();
private:
	HiVideo::ISocket* m_pSocket;
	HvCore::IHvStream* m_pStream;
	DWORD32 m_dwRefTime;
};

#endif
