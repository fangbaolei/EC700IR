#include "eprfilestorage.h"
#include "hvutils.h"
#include "slw_dev.h"
#include "hvtarget_ARM.h"
#include "misc.h"

using namespace HvCore;
extern HV_SEM_HANDLE g_hSemEDMA;
CEprFileStorage::CEprFileStorage(DWORD32 dwStartAddr, DWORD32 dwMaxDatLen)
        : m_pEprBuf(NULL)
        , m_pStreamBuf(NULL)
        , m_nCurPos(0)
        , m_fInited(FALSE)
{
    // dwMaxDatLen为存放参数总的空间大小，主表参数和副表参数各占用一半空间
    // huanggr 2012-03-07
    m_dwStartAddr = dwStartAddr;
    m_dwBackupAddr = dwStartAddr + dwMaxDatLen / 2;
    m_dwBufLen = dwMaxDatLen / 2 - sizeof(STREAM_INFO);
}

CEprFileStorage::~CEprFileStorage()
{
    Commit();
    Clear();
}

HRESULT CEprFileStorage::Invalid()
{
    if (m_pStreamBuf && m_dwBufLen > 0)
    {
        HV_memset(m_pStreamBuf, 0, m_dwBufLen);
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
}

void CEprFileStorage::Clear()
{
    if (m_pEprBuf)
    {
        HV_FreeMem(m_pEprBuf, m_dwBufLen);
        m_pEprBuf = NULL;
    }

    if (m_pStreamBuf)
    {
        HV_FreeMem(m_pStreamBuf, m_dwBufLen);
        m_pStreamBuf = NULL;
    }
}

HRESULT CEprFileStorage::DataIsValid(BOOL& fValidTmp, BOOL& fBufLenChangedTmp)
{
    BOOL fValid = FALSE;
    BOOL fBufLenChanged = FALSE;

    DWORD32 dwHeadLen = sizeof(STREAM_INFO);

    //读流信息
    int iResult = EEPROM_Read(
                      m_dwStartAddr,                //读取EEPROM的首位置.
                      (BYTE8*)&m_cStreamInfo,       //指向读出的数据.
                      sizeof(STREAM_INFO)          //期望读取的数据字节长度.
                  );

    if (iResult < 0)
    {
        HV_Trace(5, "<CEprFileStorage> EEPROM_Read iResult = 0x%08x\n", iResult);
        return E_FAIL;
    }

    if (m_cStreamInfo.StreamLen != m_dwBufLen)
    {
        m_cStreamInfo.StreamLen = m_dwBufLen;
        fBufLenChanged = TRUE;
    }

    if (m_cStreamInfo.Tag == 0x7689)
    {
        iResult = EEPROM_Read(
                      m_dwStartAddr + dwHeadLen,	//主数据首地址
                      m_pEprBuf, 		            //指向读出的数据.
                      m_cStreamInfo.StreamLen	    //期望读取的数据字节长度.
                  );

        if (iResult < 0)
        {
            HV_Trace(5, "<CEprFileStorage> EEPROM_Read iResult = 0x%08x\n", iResult);
            return E_FAIL;
        }

        //计算校验值,如果长度变化则跳过校验
        if ( fBufLenChanged )
        {
            fValid = TRUE;
        }
        else
        {
            DWORD32 dwCrc32 = 0;
            CFastCrc32 cFastCrc;
            dwCrc32 = cFastCrc.CalcCrc32(0, m_pEprBuf, m_cStreamInfo.StreamLen);
            // 如果CRC校验失败则读取副表参数
            if (m_cStreamInfo.Crc32 != dwCrc32)
            {
                HV_Trace(5, "<CEprFileStorage>Mastertable param error, get from slavetable param!");
                iResult = EEPROM_Read(
                              m_dwBackupAddr + dwHeadLen,
                              m_pEprBuf,
                              m_cStreamInfo.StreamLen
                          );
                if (iResult < 0)
                {
                    HV_Trace(5, "<CEprFileStorage> EEPROM_Read iResult = 0x%08x\n", iResult);
                    return E_FAIL;
                }
                dwCrc32 = cFastCrc.CalcCrc32(0, m_pEprBuf, m_cStreamInfo.StreamLen);
            }
            fValid = (m_cStreamInfo.Crc32 == dwCrc32) ? TRUE : FALSE;
        }
    }
    else
    {
        HV_Trace(5, "<CEprFileStorage> Tag = 0x%08x\n", m_cStreamInfo.Tag);
    }

    fValidTmp = fValid;
    fBufLenChangedTmp = fBufLenChanged;
    return S_OK;
}

// 返回值：
// S_OK              从EPR中读取成功
// S_FALSE           数据校验失败,流为空
// E_FAIL            底层操作失败
// E_OUTOFMEMORY     内存分配失败
HRESULT CEprFileStorage::Initialize(LPCSTR lpszFileName)
{
    Clear();
    m_fInited = FALSE;

    m_pEprBuf = (BYTE8*)HV_AllocMem(m_dwBufLen);
    m_pStreamBuf = (BYTE8*)HV_AllocMem(m_dwBufLen);

    if ( (m_pEprBuf == NULL) || (m_pStreamBuf == NULL) )
    {
        Clear();
        return E_OUTOFMEMORY;
    }

    HV_memset(m_pEprBuf, 0, m_dwBufLen);
    HV_memset(m_pStreamBuf, 0, m_dwBufLen);

    DWORD32 dwHeadLen = sizeof(STREAM_INFO);

    BOOL fValid = FALSE;
    BOOL fBufLenChanged = FALSE;

    int iRetryCount = 3;  // 最大重试次数
    while ( iRetryCount-- )
    {
        if (S_OK != DataIsValid(fValid, fBufLenChanged))
        {
            HV_Trace(5, "<CEprFileStorage::Initialize> DataIsValid isn't S_OK!\n");
            WdtHandshake();
            HV_Sleep(100);
            continue;
        }

        if (fValid) //数据有效则同步缓存
        {
            if ( fBufLenChanged ) //如果长度有变化重写校验值
            {
                m_cStreamInfo.Tag = 0x7689;
                m_cStreamInfo.Flag = 0;
                m_cStreamInfo.StreamLen = m_dwBufLen;
                CFastCrc32 cFastCrc;
                m_cStreamInfo.Crc32 = cFastCrc.CalcCrc32(0, m_pEprBuf, m_cStreamInfo.StreamLen);

                EEPROM_Write(
                    m_dwStartAddr,
                    (BYTE8*)&m_cStreamInfo,
                    dwHeadLen
                );
                // 同步副表参数
                EEPROM_Write(
                    m_dwBackupAddr,
                    (BYTE8*)&m_cStreamInfo,
                    dwHeadLen
                );
            }

            //同步缓存
            HV_memcpy(m_pStreamBuf, m_pEprBuf, m_cStreamInfo.StreamLen);

            break;
        }
        else
        {
            HV_Trace(5, "<CEprFileStorage::Initialize> fValid is FALSE!\n");
            WdtHandshake();
            HV_Sleep(100);
        }
    }

    if (!fValid) // 数据无效则重置流数据
    {
        m_cStreamInfo.Tag = 0x7689;
        m_cStreamInfo.Flag = 0;
        m_cStreamInfo.StreamLen = m_dwBufLen;
        m_cStreamInfo.Crc32 = 0;

        m_nCurPos = 0;

        HV_memset(m_pEprBuf, 0, m_dwBufLen);
        HV_memset(m_pStreamBuf, 0, m_dwBufLen);

        EEPROM_Write(
            m_dwStartAddr,
            (BYTE8*)&m_cStreamInfo,
            dwHeadLen
        );

        EEPROM_Write(
            m_dwStartAddr + dwHeadLen,
            m_pEprBuf,
            m_cStreamInfo.StreamLen
        );

        // 同步副表参数
        EEPROM_Write(
            m_dwBackupAddr,
            (BYTE8*)&m_cStreamInfo,
            dwHeadLen
        );

        EEPROM_Write(
            m_dwBackupAddr + dwHeadLen,
            m_pEprBuf,
            m_cStreamInfo.StreamLen
        );

        HV_Trace(5, "Resets all parameters to the default values\n");
    }

    m_fInited = TRUE;

    return fValid ? S_OK : S_FALSE;
}

//S_OK: 读取指定长度数据成功
//S_FALSE: 读取到流尾
//E_FAIL: 读取失败
//E_POINTER: 指针错误
HRESULT STDMETHODCALLTYPE CEprFileStorage::Read(
    PVOID pv,
    UINT cb,
    PUINT pcbRead
)
{
    if (!m_fInited) return E_OBJ_NO_INIT;

    if (pv == NULL) return E_POINTER;

    if (pcbRead != NULL) *pcbRead = 0;

    if (cb == 0) return S_OK;

    int nMaxReadLen = MIN_INT(cb, m_cStreamInfo.StreamLen - m_nCurPos);

    if (nMaxReadLen <= 0) return E_FAIL;

    HV_memcpy(pv, m_pStreamBuf + m_nCurPos, nMaxReadLen);

    m_nCurPos += nMaxReadLen;

    if (pcbRead != NULL) *pcbRead = nMaxReadLen;

    return ((int)cb == nMaxReadLen) ? S_OK : E_FAIL;
}

//S_OK: 写入指定长度数据成功
//S_FALSE: 写入到达流长度限制
//E_FAIL: 写入失败
//E_POINTER: 指针错误
HRESULT STDMETHODCALLTYPE CEprFileStorage::Write(
    const void* pv,
    UINT cb,
    PUINT pcbWritten
)
{
    if (!m_fInited) return E_OBJ_NO_INIT;

    if (pv == NULL) return E_POINTER;

    if (pcbWritten != NULL) *pcbWritten = 0;

    if (cb == 0) return S_OK;

    if (m_cStreamInfo.StreamLen - m_nCurPos < cb)
        HvDebugStateInfo("CEprFileStorage::Write cut off");

    int nMaxWriteLen = MIN_INT(cb, m_cStreamInfo.StreamLen - m_nCurPos);

    if (nMaxWriteLen <= 0) return E_FAIL;

    HV_memcpy(m_pStreamBuf + m_nCurPos, pv, nMaxWriteLen);

    m_nCurPos += nMaxWriteLen;

    if (pcbWritten != NULL) *pcbWritten = nMaxWriteLen;

    return ((int)cb == nMaxWriteLen) ? S_OK : E_FAIL;
}

//S_OK: 操作成功
//STG_E_INVALIDFUNCTION: 指定位置错误, 操作失败
HRESULT STDMETHODCALLTYPE CEprFileStorage::Seek(
    INT iOffset,
    STREAM_SEEK ssOrigin,
    PUINT pnNewPosition
)
{
    if (!m_fInited) return E_OBJ_NO_INIT;

    if (pnNewPosition != NULL) *pnNewPosition = m_nCurPos;

    int nNewPos = 0;

    switch (ssOrigin)
    {
    case STREAM_SEEK_SET:
        nNewPos = iOffset;
        break;
    case STREAM_SEEK_CUR:
        nNewPos = m_nCurPos + iOffset;
        break;
    case STREAM_SEEK_END:
        nNewPos = m_cStreamInfo.StreamLen + iOffset;
        break;
    }

    if (nNewPos < 0 || nNewPos > (int)m_cStreamInfo.StreamLen)
    {
        return STG_E_INVALIDFUNCTION;
    }

    m_nCurPos = nNewPos;

    if (pnNewPosition != NULL) *pnNewPosition = m_nCurPos;

    return S_OK;
}

//S_OK: 操作成功
//E_MEDIUMFULL: 操作失败
HRESULT STDMETHODCALLTYPE CEprFileStorage::Commit(
    DWORD32 grfCommitFlags
)
{
#ifndef _CAM_APP_
    CSemLock sLock(&g_hSemEDMA);
#endif
    if (!m_fInited) return E_OBJ_NO_INIT;

    DWORD32 nCommitedLen = 0;
    int nCmpLen = 0;
    BOOL fEprChanged = FALSE;

    HRESULT hr(S_OK);

    //分块比较StreamBuf和EprBuf
    while (nCommitedLen < m_cStreamInfo.StreamLen)
    {
        nCmpLen = MIN_INT(CMP_BLOCK_LEN, m_cStreamInfo.StreamLen - nCommitedLen);

        //如果有差异则同步缓存并写入EPR
        if (HV_memcmp(m_pEprBuf + nCommitedLen, m_pStreamBuf + nCommitedLen, nCmpLen) != 0)
        {
            HV_memcpy(m_pEprBuf + nCommitedLen, m_pStreamBuf + nCommitedLen, nCmpLen);

            // 写入主数据区
            hr = EEPROM_Write(
                     m_dwStartAddr + sizeof(STREAM_INFO) + nCommitedLen,
                     m_pEprBuf + nCommitedLen,
                     nCmpLen
                 );

            if (FAILED(hr))
            {
                HV_Trace(5, "<CEprFileStorage::Commit> EEPROM_Write Error and break!\n");
                break;
            }

            // 写入副表数据区
            hr = EEPROM_Write(
                     m_dwBackupAddr + sizeof(STREAM_INFO) + nCommitedLen,
                     m_pEprBuf + nCommitedLen,
                     nCmpLen
                 );

            if (FAILED(hr))
            {
                HV_Trace(5, "<CEprFileStorage::Commit> EEPROM_Write Error and break!\n");
                break;
            }

            fEprChanged = TRUE;
        }

        nCommitedLen += nCmpLen;
    }

    if (FAILED(hr)) return STG_E_MEDIUMFULL;

    if (fEprChanged) //有写入操作则更新流信息
    {
        HRESULT hrMaster = E_FAIL;
        HRESULT hrSlave = E_FAIL;
        DWORD32 dwHead = sizeof(STREAM_INFO);

        CFastCrc32 cFastCrc;
        m_cStreamInfo.Crc32 = cFastCrc.CalcCrc32(0, m_pEprBuf, m_cStreamInfo.StreamLen);

        PBYTE8 pbEprBufNow = new BYTE8[m_cStreamInfo.StreamLen];

        for (int i = 0; i < 3; ++i)
        {
            if (hrMaster != S_OK)
            {
                WdtHandshake();
                memset(pbEprBufNow, 0, m_cStreamInfo.StreamLen);
                EEPROM_Read(m_dwStartAddr + dwHead, pbEprBufNow, m_cStreamInfo.StreamLen);
                if (m_cStreamInfo.Crc32 == cFastCrc.CalcCrc32(0, pbEprBufNow, m_cStreamInfo.StreamLen))
                {
                    hr = EEPROM_Write(m_dwStartAddr, (BYTE8*)&m_cStreamInfo, dwHead);
                    hrMaster = hr;
                }
                else
                {
                    EEPROM_Write(m_dwStartAddr + dwHead, m_pEprBuf, m_cStreamInfo.StreamLen);
                    HV_Trace(5, "<CEprFileStorage>Scal master's crc failed! Try to write all!");
                }
                HV_Sleep(10);
            }

            // 同步副表参数
            if (hrSlave != S_OK)
            {
                WdtHandshake();
                memset(pbEprBufNow, 0, m_cStreamInfo.StreamLen);
                EEPROM_Read(m_dwBackupAddr + dwHead, pbEprBufNow, m_cStreamInfo.StreamLen);
                if (m_cStreamInfo.Crc32 == cFastCrc.CalcCrc32(0, pbEprBufNow, m_cStreamInfo.StreamLen))
                {
                    hr = EEPROM_Write(m_dwBackupAddr, (BYTE8*)&m_cStreamInfo, dwHead);
                    hrSlave = hr;
                }
                else
                {
                    EEPROM_Write(m_dwBackupAddr + dwHead, m_pEprBuf, m_cStreamInfo.StreamLen);
                    HV_Trace(5, "<CEprFileStorage>Scal slave's crc failed! Try to write all!");
                }
                HV_Sleep(10);
            }

            if (hrMaster == S_OK && hrSlave == S_OK)
            {
                hr = S_OK;
                break;
            }
            else
            {
                hr = E_FAIL;
            }
        }

        SAFE_DELETE(pbEprBufNow);
    }

    return hr;
}
