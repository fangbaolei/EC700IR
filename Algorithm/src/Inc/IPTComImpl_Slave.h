// 注意，应用于本类的协议返回数据最长只能2k

#ifndef IPTCOMIMPL_SLAVE_H
#define IPTCOMIMPL_SLAVE_H
#include "IPTInterface.h"

class CIPTComImpl_Slave : public IComm
{
    public:
    CIPTComImpl_Slave();
    virtual ~CIPTComImpl_Slave();

    virtual HRESULT Send( const PBYTE8 pbData, DWORD32 dwDataSize );

    virtual HRESULT Recv( PBYTE8 pbBuf, DWORD32 dwBufSize, PDWORD32 pdwRecvLen );

    protected:
    private:
    char m_szBuf[10*1024];
    int m_iBufLen;   // m_iBufLen 超过8K时空
    int m_iBufDataStart;
};

#endif // IPTCOMIMPL_SLAVE_H
