#include "TestServer.h"

CTestServer::CTestServer()
{
    m_fListen = false;
    m_iListenPort = 0;
    m_shServer = INVALID_SOCKET;
    CreateSemaphore(&m_hSemQueCtrl, 1, 1);
}

CTestServer::~CTestServer()
{
    Close();
    DestroySemaphore(&m_hSemQueCtrl);
}

bool CTestServer::IsValid(void)
{
    return m_fListen;
}

bool CTestServer::Create(const int iPort)
{
    HV_Trace(5, "test server create socket to listen port %d\n", iPort);
    m_iListenPort = iPort;
    m_shServer = HvCreateSocket();
    if (INVALID_SOCKET == m_shServer
            || FAILED(HvListen(m_shServer, iPort, 5)))
    {
        CloseSocket(m_shServer);
        HV_Trace(5, "create socket or listen port error.\n");
        return false;
    }
    return SUCCEEDED(Start(NULL));
}

void CTestServer::Close(void)
{
    SemPend(&m_hSemQueCtrl);
    if (INVALID_SOCKET != INVALID_SOCKET)
    {
        CloseSocket(m_shServer);
    }
    while (m_lstClientSocket.size())
    {
        CloseSocket(*m_lstClientSocket.begin());
        m_lstClientSocket.pop_front();
    }
    SemPost(&m_hSemQueCtrl);
    m_fExit = true;
    m_fListen = false;
}

bool CTestServer::Send(const void* pbData, int iSize)
{
    SemPend(&m_hSemQueCtrl);
    for (list<int>::iterator itr = m_lstClientSocket.begin(); itr != m_lstClientSocket.end();)
    {
        int s;
        BYTE8 * pbBuf = (BYTE8 *)pbData;
        while (iSize)
        {
            s = send(*itr, pbBuf, iSize, 0);
            if (s <= 0)
            {
                CloseSocket(*itr);
                itr = m_lstClientSocket.erase(itr);
                break;
            }
            iSize -= s;
            pbBuf += s;
        }
        if (!iSize)
        {
            ++itr;
        }
    }
    SemPost(&m_hSemQueCtrl);
    return true;
}

bool CTestServer::HasClient(void)
{
    SemPend(&m_hSemQueCtrl);
    int iSize= m_lstClientSocket.size();
    SemPost(&m_hSemQueCtrl);
    return iSize > 0;
}

HRESULT CTestServer::Run(void* pvParam)
{
    m_fListen = true;
    HV_SOCKET_HANDLE hNewSocket = INVALID_SOCKET;
    HV_Trace(5, "start listen test server, port=%d\n", m_iListenPort);
    while (!m_fExit)
    {
        if (S_OK == HvAccept(m_shServer, hNewSocket, 1000))
        {
            HV_Trace(5, "receive a test socket...");
            SemPend(&m_hSemQueCtrl);
            m_lstClientSocket.push_back(hNewSocket);
            SemPost(&m_hSemQueCtrl);
        }
    }
    return S_OK;
}
