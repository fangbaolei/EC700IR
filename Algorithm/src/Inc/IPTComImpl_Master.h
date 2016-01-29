#ifndef IPTCOMIMPL_MASTER_H
#define IPTCOMIMPL_MASTER_H

#include "hvutils.h"
#include "SerialBase.h"
#include "IPTInterface.h"


class CIPTComImpl_Master : public IComm
{
public:
    CIPTComImpl_Master();
    virtual ~CIPTComImpl_Master();

    int Lock(unsigned long nTimeOMs);
    int UnLock();

    HRESULT Open(const char* szDev);
	// 清空接收与写入缓存
	void Flush();

    HRESULT Send( const PBYTE8 pbData, DWORD32 dwDataSize );
    HRESULT Recv( PBYTE8 pbBuf, DWORD32 dwBufSize, PDWORD32 pdwRecvLen );

    protected:
    private:
    CSerialBase m_cSerialBase;
};

#endif // IPTCOMIMPL_MASTER_H
