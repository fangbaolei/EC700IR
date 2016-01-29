#ifndef _EPRFILESTORAGE_H_
#define _EPRFILESTORAGE_H_

#include "hvinterface.h"

static const WORD16 CMP_BLOCK_LEN = 1024;

#define STG_E_INVALIDFUNCTION  _HRESULT_TYPEDEF_(0x80030001L)

#define STG_E_MEDIUMFULL _HRESULT_TYPEDEF_(0x80030070L)

class CEprFileStorage : public HvCore::IHvStream
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

    // IHvStream
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
    CEprFileStorage( DWORD32 dwStartAddr, DWORD32 dwMaxDatLen);
    virtual ~CEprFileStorage();

    HRESULT Initialize(LPCSTR lpszFileName);
    HRESULT Invalid();

protected:
    BYTE8* m_pEprBuf;
    BYTE8* m_pStreamBuf;

    DWORD32 m_nCurPos;

    DWORD32 m_dwStartAddr;
    DWORD32 m_dwBackupAddr;
    DWORD32 m_dwBufLen;

    BOOL m_fInited;

    typedef struct _STREAM_INFO
    {
        DWORD32 Tag;
        DWORD32 Flag;
        DWORD32 StreamLen;
        DWORD32 Crc32;

        _STREAM_INFO()
        {
            Tag = 0x7689;
            Flag = 0;
            StreamLen = 0;
            Crc32 = 0;
        }
    }
    STREAM_INFO;

    STREAM_INFO m_cStreamInfo;

    void Clear();

    HRESULT DataIsValid(BOOL& fValidTmp, BOOL& fBufLenChangedTmp);
};

#endif
