#include "hvutils.h"
#include "memstorage.h"
using namespace HvCore;

CMemStorage::CMemStorage()
        :m_pBuf(NULL)
        ,m_nBufLen(0)
        ,m_nCurPos(0)
{
    return;
}

CMemStorage::~CMemStorage()
{
    return;
}

HRESULT CMemStorage::Initialize(BYTE8* pBuf, UINT nBufLen)
{
    if (pBuf == NULL || nBufLen == 0) return E_INVALIDARG;

    m_pBuf = pBuf;
    m_nBufLen = nBufLen;
    m_nCurPos = 0;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CMemStorage::Read(
    PVOID pv,
    UINT cb,
    PUINT pcbRead
)
{
    if (m_pBuf == NULL) return E_OBJ_NO_INIT;
    if ( pv == NULL) return E_POINTER;

    if ( pcbRead != NULL) *pcbRead = 0;
    if (cb == 0) return S_OK;

    int nMaxReadLen = MIN_INT( cb, m_nBufLen - m_nCurPos);
    if (nMaxReadLen <= 0) return S_FALSE;

    HV_memcpy(pv, m_pBuf + m_nCurPos, nMaxReadLen);
    m_nCurPos += nMaxReadLen;

    if ( pcbRead != NULL) *pcbRead = nMaxReadLen;
    return ((int)cb == nMaxReadLen)?S_OK:S_FALSE;
}

HRESULT STDMETHODCALLTYPE CMemStorage::Write(
    const void* pv,
    UINT cb,
    PUINT pcbWritten
)
{
    if ( m_pBuf == NULL ) return E_OBJ_NO_INIT;
    if ( pv == NULL) return E_POINTER;

    if ( pcbWritten != NULL ) *pcbWritten = 0;
    if ( cb == 0) return S_OK;

    int nMaxWriteLen = MIN_INT( cb, m_nBufLen - m_nCurPos);
    if (nMaxWriteLen <= 0) return S_FALSE;

    HV_memcpy(m_pBuf + m_nCurPos, pv, nMaxWriteLen);
    m_nCurPos += nMaxWriteLen;

    if ( pcbWritten != NULL ) *pcbWritten = nMaxWriteLen;
    return ((int)cb == nMaxWriteLen)?S_OK:S_FALSE;
}

HRESULT STDMETHODCALLTYPE CMemStorage::Seek(
    INT iOffset,
    HvCore::STREAM_SEEK ssOrigin,
    PUINT pnNewPosition
)
{
    if ( m_pBuf == NULL) return E_OBJ_NO_INIT;

    if ( pnNewPosition != NULL) *pnNewPosition = m_nCurPos;

    int nNewPos = 0;

    switch (ssOrigin)
    {
    case HvCore::STREAM_SEEK_SET:
        nNewPos = iOffset;
        break;
    case HvCore::STREAM_SEEK_CUR:
        nNewPos = m_nCurPos + iOffset;
        break;
    case HvCore::STREAM_SEEK_END:
        nNewPos = m_nBufLen + iOffset;
        break;
    }

    if ( nNewPos >= (int)m_nBufLen)
    {
        return STG_E_INVALIDFUNCTION;
    }

    m_nCurPos = nNewPos;

    if ( pnNewPosition != NULL) *pnNewPosition = m_nCurPos;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CMemStorage::Commit(
    DWORD32 grfCommitFlags
)
{
    return S_OK;
}
