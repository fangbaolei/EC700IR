#ifndef _MEMSTORAGE_H_
#define _MEMSTORAGE_H_

#include "hvinterface.h"

#define STG_E_INVALIDFUNCTION _HRESULT_TYPEDEF_(0x80030001L)
#define STG_E_MEDIUMFULL _HRESULT_TYPEDEF_(0x80030070L)

class CMemStorage : public HvCore::IHvStream
{
public:
    //IUnknown
    STDMETHOD(QueryInterface)(
        const IID& iid,
        void** ppv
    )
    {
        HRESULT hr = S_OK;
        if ( iid == HvCore::IID_IUnknown
                || iid == HvCore::IID_IHvStream )
        {
            *ppv = static_cast<IHvStream*>(this);
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

    //IHvStream
    STDMETHOD(Read)(
        PVOID pv,
        UINT cb,
        PUINT pcbRead
    );

    STDMETHOD(Write)(
        const void* pv,
        UINT cb,
        PUINT pcbWritten
    );

    STDMETHOD(Seek)(
        INT iOffset,
        HvCore::STREAM_SEEK ssOrigin,
        PUINT pnNewPosition
    );

    STDMETHOD(Commit)(
        DWORD32 grfCommitFlags = 0
    );

    STDMETHOD(UDPRead)(
        BYTE8 *pBuffer,
        int iBufferLen,
        DWORD32 *pRemoteAddr,
        WORD16 *pRemotePort,
        int iTimeout
    )
    {
        return E_FAIL;
    }

    STDMETHOD(UDPSend)(
        BYTE8 *pBuffer,
        int iBufferLen,
        DWORD32 dwRemoteAddr,
        WORD16 wRemotePort
    )
    {
        return E_FAIL;
    }

public:
    CMemStorage();
    virtual ~CMemStorage();

    HRESULT Initialize(BYTE8* pBuf, UINT nBufLen);

    UINT GetCurPos()
    {
        return m_nCurPos;
    };

protected:
    BYTE8* m_pBuf;
    UINT m_nBufLen;

    int m_nCurPos;
};

#endif
