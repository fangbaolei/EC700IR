#include "IPTComImpl_Master.h"

CIPTComImpl_Master::CIPTComImpl_Master()
{

}

CIPTComImpl_Master::~CIPTComImpl_Master()
{
}

HRESULT CIPTComImpl_Master::Open(const char* szDev)
{
    HRESULT hr = m_cSerialBase.Open(szDev);
    if (hr == S_OK)
    {
        hr = m_cSerialBase.SetAttr(9600, 8, 0, 1);
    }
    return hr;
}

int CIPTComImpl_Master::Lock(unsigned long nTimeOMs)
{
    return m_cSerialBase.Lock(nTimeOMs);
}

int CIPTComImpl_Master::UnLock()
{
    return m_cSerialBase.UnLock();
}

void CIPTComImpl_Master::Flush()
{
    m_cSerialBase.Flush();
}

HRESULT CIPTComImpl_Master::Send( const PBYTE8 pbData, DWORD32 dwDataSize )
{
    return ((int)dwDataSize == m_cSerialBase.Send(pbData, (int)dwDataSize)) ? S_OK : E_FAIL;
}

HRESULT CIPTComImpl_Master::Recv( PBYTE8 pbBuf, DWORD32 dwBufSize, PDWORD32 pdwRecvLen )
{
    *pdwRecvLen = m_cSerialBase.Recv(pbBuf, (int)dwBufSize, 0);
    return (*pdwRecvLen == dwBufSize ) ? S_OK : E_FAIL;
}
