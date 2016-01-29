#include "IPTComImpl_Slave.h"
#include "HvPciLinkApi.h"
#include "hvutils.h"
#include "HvSerialLink.h"

CIPTComImpl_Slave::CIPTComImpl_Slave()
{
    m_iBufLen = 0;
    m_iBufDataStart = 0;
}

CIPTComImpl_Slave::~CIPTComImpl_Slave()
{
}

HRESULT CIPTComImpl_Slave::Send( const PBYTE8 pbData, DWORD32 dwDataSize )
{
    if (m_iBufDataStart > 8*1024)  //m_iBufLen 大于8K时清理
    {
        m_iBufDataStart = 0;
        m_iBufLen = 0;
    }

    int iRecvLen = 2*1024;
    int iRt = PciSendComData(pbData, dwDataSize, &m_szBuf[m_iBufDataStart+m_iBufLen], &iRecvLen, 5000);
    if (iRt == S_OK)
    {
        m_iBufLen += iRecvLen;
    }
    return iRt;
}

HRESULT CIPTComImpl_Slave::Recv( PBYTE8 pbBuf, DWORD32 dwBufSize, PDWORD32 pdwRecvLen )
{
    DWORD32 dwRecvLen = m_iBufLen;
    if (dwBufSize < dwRecvLen)
    {
        dwRecvLen = dwBufSize;
    }

    memcpy(pbBuf, &m_szBuf[m_iBufDataStart], dwRecvLen);
    m_iBufLen -= dwRecvLen;
    m_iBufDataStart += dwRecvLen;

    if (m_iBufDataStart > 8*1024)  //m_iBufLen 大于8K时前移
    {
        if (m_iBufLen != 0)
        {
            memcpy(m_szBuf, &m_szBuf[m_iBufDataStart], m_iBufLen);
        }
        m_iBufDataStart = 0;
    }

    *pdwRecvLen = dwRecvLen;

    return S_OK;
}
