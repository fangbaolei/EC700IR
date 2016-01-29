// ¸ÃÎÄ¼þ±àÂë¸ñÊ½±ØÐëÎªWINDOWS-936¸ñÊ½

#include "ImgGatherer.h"

using namespace HiVideo;

void inline GetIpDWord(const char* pszIP, DWORD32& dwIP)
{
    DWORD32 Temp[4]={0};
    sscanf(pszIP, "%d.%d.%d.%d", Temp+3, Temp+2, Temp+1,Temp);
    dwIP = (Temp[3]<<24)|(Temp[2]<<16)|(Temp[1]<<8)|Temp[0];
}

CTftpPtl::CTftpPtl()
        : m_sktData(INVALID_SOCKET)
        , m_pstmData(NULL)
        , m_wBlockSize(1450)
        , m_dwRemoteAddr(0)
        , m_wRemotePort(SRV_PORT)
{

}

CTftpPtl::~CTftpPtl()
{
    DisConnect();
}

HRESULT CTftpPtl::Connect(const char *pszAddress)
{
    if (NULL == pszAddress) return E_POINTER;

    if (m_sktData != INVALID_SOCKET)
    {
        CloseSocket(m_sktData);
        m_sktData = INVALID_SOCKET;
    }

    m_sktData = HvCreateSocket(AF_INET, SOCK_DGRAM, 0);
    if (m_sktData == INVALID_SOCKET)
    {
        return E_FAIL;
    }

    int nZero = 0;
    setsockopt(m_sktData, SOL_SOCKET, SO_SNDBUF, (char *)&nZero, sizeof(nZero));

    m_pstmData = new CSocketStream(m_sktData);
    if (m_pstmData == NULL)
    {
        DisConnect();
        return E_FAIL;
    }

    GetIpDWord(pszAddress, m_dwRemoteAddr);

    return S_OK;
}

HRESULT CTftpPtl::DisConnect()
{
    if (m_sktData != INVALID_SOCKET)
    {
        CloseSocket(m_sktData);
        m_sktData = INVALID_SOCKET;
    }
    if (m_pstmData)
    {
        delete m_pstmData;
        m_pstmData = NULL;
    }

    return S_OK;
}

HRESULT CTftpPtl::SendRRQ(const char *pszFileName, const char *pszMode, const char *pszBlkSize, const char *pszSize, int iTimeout)
{
    if ((NULL == pszFileName) || (NULL == pszMode) || (NULL == pszSize) || (NULL == pszBlkSize)) return E_POINTER;
    if (NULL == m_pstmData) return E_HANDLE;

    HRESULT hr(S_OK);

    char szBuf[512] = {0};
    int iPos = 0;
    szBuf[iPos++] = 0;
    szBuf[iPos++] = TFTP_RRQ;

    strcpy(&szBuf[iPos], pszFileName);
    iPos += (int)strlen(pszFileName);
    szBuf[iPos++] = 0;

    strcpy(&szBuf[iPos], pszMode);
    iPos += (int)strlen(pszMode);
    szBuf[iPos++] = 0;

    strcpy(&szBuf[iPos], pszBlkSize);
    iPos += (int)strlen(pszBlkSize);
    szBuf[iPos++] = 0;

    strcpy(&szBuf[iPos], pszSize);
    iPos += (int)strlen(pszSize);
    szBuf[iPos++] = 0;

    if (0 >= m_pstmData->UDPSend((BYTE8*)szBuf, iPos, m_dwRemoteAddr, SRV_PORT))
    {
        HV_Trace(3, "tftp_ptl.cpp:TFTP::SendRRQÊ§°Ü£¡\n");
        DisConnect();
        hr = E_FAIL;
    }

    int iBlockSize = 0;
    sscanf(pszSize, "%d", &iBlockSize);
    m_wBlockSize = iBlockSize;

    return hr;
}

HRESULT CTftpPtl::SendWRQ(const char *pszFileName, const char *pszMode, const char *pszBlkSize, const char *pszSize, int iTimeout)
{
    if ((NULL == pszFileName) || (NULL == pszMode)) return E_POINTER;
    if (NULL == m_pstmData) return E_HANDLE;

    HRESULT hr(S_OK);

    char szBuf[512] = {0};
    int iPos = 0;
    szBuf[iPos++] = 0;
    szBuf[iPos++] = TFTP_WRQ;

    strcpy(&szBuf[iPos], pszFileName);
    iPos += (int)strlen(pszFileName);
    szBuf[iPos++] = 0;

    strcpy(&szBuf[iPos], pszMode);
    iPos += (int)strlen(pszMode);
    szBuf[iPos++] = 0;

    strcpy(&szBuf[iPos], pszBlkSize);
    iPos += (int)strlen(pszBlkSize);
    szBuf[iPos++] = 0;

    strcpy(&szBuf[iPos], pszSize);
    iPos += (int)strlen(pszSize);
    szBuf[iPos++] = 0;

    if (0 >= m_pstmData->UDPSend((BYTE8*)szBuf, iPos, m_dwRemoteAddr, SRV_PORT))
    {
        HV_Trace(3, "tftp_ptl.cpp:TFTP::SendWRQÊ§°Ü£¡\n");
        DisConnect();
        hr = E_FAIL;
    }

    int iBlockSize = 0;
    sscanf(pszSize, "%d", &iBlockSize);
    m_wBlockSize = iBlockSize;

    return hr;
}

HRESULT CTftpPtl::SendACK(WORD16 wBlock, int iTimeout)
{
    if (NULL == m_pstmData) return E_HANDLE;

    HRESULT hr(S_OK);

    char rgcAckBuf[4] = {0, TFTP_ACK, 0, 0};
    rgcAckBuf[2] = (wBlock >> 8) & 0x00FF;
    rgcAckBuf[3] = wBlock & 0x00FF;

    if (0 >= m_pstmData->UDPSend((BYTE8*)rgcAckBuf, 4, m_dwRemoteAddr, m_wRemotePort))
    {
        HV_Trace(3, "tftp_ptl.cpp:TFTP::SendACKÊ§°Ü£¡\n");
        DisConnect();
        hr = E_FAIL;
    }
    return hr;
}

HRESULT CTftpPtl::SendDATA(WORD16 wBlock, char *pcBuf, int iSize, int iTimeout)
{
    if (NULL == m_pstmData) return E_HANDLE;
    if (iSize > m_wBlockSize) return E_OUTOFMEMORY;
    if (NULL == pcBuf) return E_POINTER;

    HRESULT hr(S_OK);

    BYTE8 pbSendBuf[256] = {0};

    pbSendBuf[0] = (TFTP_DATA >> 8) & 0x00FF;
    pbSendBuf[1] = TFTP_DATA & 0x00FF;
    pbSendBuf[2] = (wBlock >> 8) & 0x00FF;
    pbSendBuf[3] = wBlock & 0x00FF;
    memcpy(&pbSendBuf[4], pcBuf, iSize);

    if (0 >= m_pstmData->UDPSend(pbSendBuf, 4 + iSize,  m_dwRemoteAddr, m_wRemotePort))
    {
        HV_Trace(3, "tftp_ptl.cpp:TFTP::SendDATAÊ§°Ü£¡\n");
        DisConnect();
        hr = E_FAIL;
    }
    return hr;
}

HRESULT CTftpPtl::RecvData(char *pcBuf, int ilen, int *piRevLen, int iTimeout)
{
    if (NULL == m_pstmData) return E_HANDLE;
    if (NULL == pcBuf) return E_POINTER;

    DWORD32 dwAddrRemote;
    WORD16 wPortRemote;

    int iRtn = m_pstmData->UDPRead((BYTE8*)pcBuf, ilen, &dwAddrRemote, &wPortRemote, iTimeout);
    if (iRtn == 0)
    {
        HV_Trace(3, "tftp_ptl.cpp:TFTP::RecvData³¬Ê±£¡\n");
        return S_FALSE;
    }
    else if (iRtn == -1)
    {
        HV_Trace(3, "tftp_ptl.cpp:TFTP::RecvDataÊ§°Ü£¡\n");
        DisConnect();
        return E_FAIL;
    }
    if (wPortRemote != SRV_PORT)
    {
        m_wRemotePort = wPortRemote;
    }
    if (piRevLen)
    {
        *piRevLen = iRtn;
    }
    return S_OK;
}

