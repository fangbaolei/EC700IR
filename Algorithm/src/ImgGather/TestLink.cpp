#include "ImgGatherer.h"

#define RCV_TIMEOUT 2000

using namespace HiVideo;

CTestLink::CTestLink()
        : m_hSocket(INVALID_SOCKET)
        , m_pStream(NULL)
        , m_dwRefTime(0)
        , m_fConnectFlag(FALSE)
{
    m_hSocket = HvCreateSocket();
}

CTestLink::~CTestLink()
{
    Close();
}

HRESULT CTestLink::Connect(const char* pszIP, WORD16 wPort)
{
    Close();

    if (pszIP == NULL)
    {
        return E_POINTER;
    }

    if (m_hSocket == INVALID_SOCKET)
    {
        m_hSocket = HvCreateSocket();
    }

    if (HvConnect(m_hSocket, pszIP, wPort, 2000) != S_OK)
    {
        return E_FAIL;
    }

    HvSetRecvTimeOut(m_hSocket, RCV_TIMEOUT);
    m_pStream = new CSocketStream(m_hSocket);

    if (m_pStream == NULL)
    {
        Close();
        return E_FAIL;
    }

    m_fConnectFlag = TRUE;

    return S_OK;
}

HRESULT CTestLink::GetImage(
    unsigned char* pcBuf,
    DWORD32* pdwSize,
    DWORD32* pdwTime,
    char* pszFileName
)
{
    if (!m_pStream || !pdwSize || !pdwTime || !pszFileName) return E_POINTER;

    HRESULT hr(S_OK);

    // 接收图片时标
    hr = m_pStream->Read(pdwTime, 4, NULL);
    if (FAILED(hr))
    {
        m_fConnectFlag = FALSE;
        return hr;
    }

    *pdwTime = m_dwRefTime;
    m_dwRefTime += 80;

    // 接收图片名字
    char szFileName[IMG_NAME_LEN] = {0};
    hr = m_pStream->Read(szFileName, IMG_NAME_LEN, NULL);
    if (FAILED(hr))
    {
        m_fConnectFlag = FALSE;
        return hr;
    }
    memcpy(pszFileName, szFileName, IMG_NAME_LEN);

    // 接收图片长度
    DWORD32 dwSize;
    hr = m_pStream->Read(&dwSize, 4, NULL);
    if (FAILED(hr))
    {
        m_fConnectFlag = FALSE;
        return hr;
    }
    if (dwSize > *pdwSize)
    {
        m_fConnectFlag = FALSE;
        return E_OUTOFMEMORY;
    }
    *pdwSize = dwSize;

    // 接收图片数据
    hr = m_pStream->Read(pcBuf, dwSize, NULL);
    if (FAILED(hr))
    {
        m_fConnectFlag = FALSE;
        return hr;
    }

    return hr;
}

HRESULT CTestLink::Close()
{
    if (m_hSocket != INVALID_SOCKET)
    {
        CloseSocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }
    if (m_pStream)
    {
        delete m_pStream;
        m_pStream = NULL;
    }

    m_fConnectFlag = FALSE;

    return S_OK;
}

BOOL CTestLink::IsConnect()
{
    return m_fConnectFlag;
}
