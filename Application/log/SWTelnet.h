#ifndef __SW_TELNET_H__
#define __SW_TELNET_H__
#include "SWObject.h"

class CSWTelnet : public  CSWThread
{
	CLASSINFO(CSWTelnet, CSWThread)
public:
	CSWTelnet();
	virtual ~CSWTelnet();
	HRESULT Create(const INT nPort = 23);
	VOID    Clear(VOID);
	HRESULT Log(INT iLevel, LPCSTR szLog);
	virtual HRESULT IsValid();
	virtual BOOL IsLogging();
protected:
	virtual HRESULT Run();	
private:
	BOOL m_fInitialize;
	CSWTCPSocket m_tcpTelnet;	
	INT m_iHandle;
	BOOL m_fLogging;
};
#endif

