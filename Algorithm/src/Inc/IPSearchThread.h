/**
* @file		IPSearchThread.h
* @version	1.0
* @brief	使客户机可通过广播包搜索到该设备。
*/

#ifndef _IPSEARCHTHREAD_H_
#define _IPSEARCHTHREAD_H_

#include "hvutils.h"
#include "hvthreadbase.h"

//以太网搜索线程
class CIPSearchThread : public CHvThreadBase
{
public:
	CIPSearchThread();
	virtual ~CIPSearchThread();

	HRESULT Create();
	HRESULT Close();

	bool IsConnected();
	bool ThreadIsOk();

    virtual const char* GetName()
    {
        static char szName[] = "CIPSearchThread";
        return szName;
    }
	virtual HRESULT Run(void *pvParamter);

protected:
	bool m_fRunStatus;
	DWORD32 m_dwLastThreadIsOkTime;
};

#endif
