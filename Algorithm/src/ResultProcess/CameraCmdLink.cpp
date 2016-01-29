// 该文件编码格式必须是WIN936

#include "CameraCmdLink.h"

using namespace HiVideo;

//------------=============  CCameraCmdLink =================--------------------

CCameraCmdLink::CCameraCmdLink()
        : m_pStream(NULL)
{
}

CCameraCmdLink::~CCameraCmdLink()
{
    if ( NULL != m_pStream )
    {
        delete m_pStream;
        m_pStream = NULL;
    }
}

HRESULT CCameraCmdLink::ReceiveHeader(CAMERA_CMD_HEADER* pCmdHeader)
{
    if ( pCmdHeader == NULL ) return E_POINTER;
    if ( m_pStream == NULL ) return E_POINTER;

    HvCore::IHvStream* pSocketStream = NULL;
    pSocketStream = (HvCore::IHvStream*)m_pStream;

    UINT cbRead = 0;
    if ( S_OK != pSocketStream->Read(pCmdHeader, sizeof(CAMERA_CMD_HEADER), &cbRead)
            || sizeof(CAMERA_CMD_HEADER) != cbRead )
    {
        return E_NETFAIL;
    }

    return S_OK;
}

HRESULT CCameraCmdLink::SendRespond(const CAMERA_CMD_RESPOND* pCmdRespond)
{
    if ( pCmdRespond == NULL ) return E_POINTER;
    if ( m_pStream == NULL ) return E_POINTER;

    HvCore::IHvStream* pSocketStream = NULL;
    pSocketStream = (HvCore::IHvStream*)m_pStream;

    if ( S_OK != pSocketStream->Write(pCmdRespond, sizeof(CAMERA_CMD_RESPOND), NULL) )
    {
        return E_NETFAIL;
    }

    return S_OK;
}

HRESULT CCameraCmdLink::ReceiveData(PBYTE8 pbData, const DWORD32& dwSize, DWORD32* pdwRecvSize)
{
    if ( pbData == NULL || dwSize == 0 )
    {
        return E_POINTER;
    }
    if ( m_pStream == NULL ) return E_POINTER;
    if ( pdwRecvSize != NULL ) *pdwRecvSize = 0;

    HvCore::IHvStream* pSocketStream = NULL;
    pSocketStream = (HvCore::IHvStream*)m_pStream;

    UINT cbRead = 0;
    UINT cbSize = dwSize;
    if ( S_OK != pSocketStream->Read(pbData, cbSize, &cbRead)
            || cbRead != cbSize )
    {
        return E_NETFAIL;
    }

    if ( pdwRecvSize != NULL ) *pdwRecvSize = cbRead;

    return S_OK;
}

HRESULT CCameraCmdLink::SendData(const PBYTE8 pbData, const DWORD32& dwSize)
{
    if ( pbData == NULL || dwSize == 0 ) return E_POINTER;
    if ( m_pStream == NULL ) return E_POINTER;

    HvCore::IHvStream* pSocketStream = NULL;
    pSocketStream = (HvCore::IHvStream*)m_pStream;

    UINT cbSize = dwSize;
    if ( S_OK != pSocketStream->Write(pbData, cbSize, NULL) )
    {
        return E_NETFAIL;
    }

    return S_OK;
}

HRESULT CCameraCmdLink::Process(HV_SOCKET_HANDLE hSocket, HiVideo::ICameraCmdProcess* pCmdProcess)
{
    HRESULT hr = S_OK;
    if (NULL == pCmdProcess || INVALID_SOCKET == hSocket)
    {
    	return E_POINTER;
   	}
	m_pStream = new CSocketStream(hSocket);
	if (NULL == m_pStream)
	{
		return E_POINTER;
	}

    while (true)
    {
        CAMERA_CMD_HEADER cmdHeader;
        hr = ReceiveHeader(&cmdHeader);
        if ( hr != S_OK ) break;

        hr = pCmdProcess->Process(&cmdHeader, (ICameraCmdLink*)this);
        if ( hr != S_OK ) break;
    }

    if (NULL != m_pStream)
    {
        delete m_pStream;
        m_pStream = NULL;
    }

    return S_OK;
}



//------------------------------=============== CCameraCmdLinkCtrl =====================--------------------------

CCameraCmdLinkCtrl::CCameraCmdLinkCtrl(ICameraCmdProcess* pCmdProcess)
        : m_pCmdProcess(pCmdProcess)
{
    if (CreateSemaphore(&m_hSemEnum, 1, 1) != 0)
    {
        HV_Trace(5, "<CCameraCmdLinkCtrl>CreateSemaphore Failed!\n");
    }
}

CCameraCmdLinkCtrl::~CCameraCmdLinkCtrl()
{
    Stop(-1);

    DestroySemaphore(&m_hSemEnum);
    if ( INVALID_SOCKET != m_hSocket )
    {
        CloseSocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }
}

HRESULT CCameraCmdLinkCtrl::EnumLock()
{
    SemPend(&m_hSemEnum);
    return S_OK;
}

HRESULT CCameraCmdLinkCtrl::EnumUnLock()
{
    SemPost(&m_hSemEnum);
    return S_OK;
}

HRESULT CCameraCmdLinkCtrl::SetParam(HiVideo::ICameraCmdProcess* pCmdProcess)
{
    if( pCmdProcess != NULL ) m_pCmdProcess = pCmdProcess;

    return S_OK;
}

HRESULT CCameraCmdLinkCtrl::Run(void* pvParam)
{
    HV_SOCKET_HANDLE m_hSocket = HvCreateSocket();
    if ( INVALID_SOCKET == m_hSocket )
    {
        HV_Trace(5, "<link>HvCreateSocket failed!\n");
        return E_FAIL;
    }
    if ( S_OK != HvListen(m_hSocket, CAMERA_CMD_LINK_PORT, MAX_CMD_CONNECT_COUNT) )
    {
        HV_Trace(5, "<link>HvListen<%d, %d> failed!\n", CAMERA_CMD_LINK_PORT, MAX_CMD_CONNECT_COUNT);
        return E_FAIL;
    }

    HV_Trace(3, "<link>Link ctrl run,port:%d.\n",  CAMERA_CMD_LINK_PORT);

    HV_SOCKET_HANDLE hNewSocket = INVALID_SOCKET;
    int iWaitMs = 4000;
    int iTimeOutMs = 6000;
    while ( !m_fExit )
    {
        // 监听
        hNewSocket = INVALID_SOCKET;
        HRESULT hr = HvAccept(m_hSocket, hNewSocket, iWaitMs);
        if (S_OK == hr)
        {
            // 创建新的连接
            HvSetRecvTimeOut(hNewSocket, iTimeOutMs);
            // 处理该连接
            m_cCameraCmdLink.Process(hNewSocket, m_pCmdProcess);
            // 关闭连接
            CloseSocket(hNewSocket);
			hNewSocket = INVALID_SOCKET;
        }
    }

    // 断开连接
    if ( INVALID_SOCKET != m_hSocket )
    {
        CloseSocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }

    m_fExit = TRUE;
    return S_OK;
}
