/**
* @file		SocketStream.h
* @version	1.0
* @brief	套接字读写流
*/
#ifndef SOCKET_STREAM_H_INCLUDED
#define SOCKET_STREAM_H_INCLUDED


#include "hvutils.h"
#include "hvinterface.h"
#include "hvsocket.h"

#include "resultsend.h"

class CSocketStream : public HvCore::IHvStream
{
public:
    // IUnknown
    STDMETHOD(QueryInterface)(
        const IID& iid,
        void** ppv
    )
    {
        HRESULT hr = S_OK;
        if ( iid == HvCore::IID_IUnknown || iid == HvCore::IID_IHvStream )
        {
            *ppv = static_cast<HvCore::IHvStream*>(this);
            AddRef();
        }
        else
        {
            *ppv = NULL;
            hr = E_NOINTERFACE;
        }
        return hr;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return 1;
    }

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        return 0;
    }

public:
    //IHvStream
    HRESULT STDMETHODCALLTYPE Read(
        PVOID pv,
        UINT cb,
        PUINT pcbRead
    )
    {
        if ( pv == NULL ) return E_INVALIDARG;
        if ( pcbRead != NULL ) *pcbRead = 0;

        int nReceivedLen = RecvAll( m_hSocket, (char*)pv, cb);

        if ( pcbRead != NULL ) *pcbRead = nReceivedLen;
        return (nReceivedLen == (int)cb)?S_OK:E_FAIL;
    }

    HRESULT STDMETHODCALLTYPE Write(
        const void* pv,
        UINT cb,
        PUINT pcbWritten
    )
    {
        if ( pv == NULL )
        {
            return E_INVALIDARG;
        }
        char * buf = (char *)pv;
        if ( pcbWritten != NULL )
        {
            *pcbWritten = cb;
        }
        while (cb)
        {
            int nLen = Send(m_hSocket, buf, cb, 0);
            if (nLen <= 0)
            {
                break;
            }
            buf += nLen;
            cb -= nLen;
        }
        if (pcbWritten != NULL && cb > 0)
        {
            *pcbWritten -= cb;
        }
        return !cb ? S_OK : E_FAIL;
    }

    HRESULT STDMETHODCALLTYPE Seek(
        INT iOffset,
        HvCore::STREAM_SEEK ssOrigin,
        PUINT pnNewPosition
    )
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Commit(
        DWORD32 grfCommitFlags = 0
    )
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE UDPRead(
        BYTE8 *pBuffer,
        int iBufferLen,
        DWORD32 *pRemoteAddr,
        WORD16 *pRemotePort,
        int iTimeout
    )
    {
        if (m_hSocket == INVALID_SOCKET) return -1;
        if ((!pBuffer) || (!pRemoteAddr) || (!pRemotePort)) return -1;

        hv_sockaddr_in addrRemote;
        int iAddlen = sizeof(addrRemote), iRtn = -1;

        struct timeval tv;
        tv.tv_sec = iTimeout / 1000;
        tv.tv_usec = (iTimeout % 1000) * 1000;
        fd_set rdfds;
        FD_ZERO(&rdfds);
        FD_SET(m_hSocket, &rdfds);

        iRtn = select(m_hSocket + 1, &rdfds, NULL, NULL, &tv);
        if (iRtn <= 0)
        {
            return iRtn;
        }

        iRtn = RecvFrom(m_hSocket, (char*)pBuffer, iBufferLen, 0, (hv_sockaddr*)&addrRemote, &iAddlen);
        *pRemoteAddr = ntohl(addrRemote.sin_addr.s_addr);
        *pRemotePort = ntohs(addrRemote.sin_port);

        return iRtn;
    }

    HRESULT STDMETHODCALLTYPE UDPSend(
        BYTE8 *pBuffer,
        int iBufferLen,
        DWORD32 dwRemoteAddr,
        WORD16 wRemotePort
    )
    {
        if (m_hSocket == INVALID_SOCKET) return -1;
        if (!pBuffer) return -1;

        hv_sockaddr_in addrRemote;
        memset(&addrRemote, 0, sizeof(addrRemote));
        addrRemote.sin_addr.s_addr = htonl(dwRemoteAddr);
        addrRemote.sin_family = AF_INET;
        addrRemote.sin_port = htons(wRemotePort);

        int iAddlen = sizeof(hv_sockaddr_in);
        int isend = SendTo(m_hSocket, (char*)pBuffer, iBufferLen, 0, (hv_sockaddr*)&addrRemote, iAddlen);

        return isend;
    }

public:
    CSocketStream( HV_SOCKET_HANDLE hSocket )
            : m_hSocket(hSocket)
    {
        sigset_t signal_mask;
        sigemptyset(&signal_mask);
        sigaddset(&signal_mask, SIGPIPE);
        int rc = pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
        if (rc != 0)
        {
            HV_Trace(1, "pthread_sigmask is error.\n");
        }
    };
    virtual ~CSocketStream() {};

protected:
    HV_SOCKET_HANDLE m_hSocket;
};

#endif
