#include "DiskFileStorage.h"

using namespace HvCore;

CDiskFileStorage::CDiskFileStorage(WORD16 wMaxDatLen)
: m_fInited(FALSE)
, m_pFile(NULL)
, m_nCurPos(0)
, m_wBufLen(wMaxDatLen)
, m_pStreamBuf(NULL)
{
	m_szFileName[0] = '\0';
}

CDiskFileStorage::~CDiskFileStorage()
{
	Commit();
	Clear();
}

void CDiskFileStorage::Clear()
{
	CloseFile(&m_pFile);

	if (m_pStreamBuf)
	{
		HV_FreeMem(m_pStreamBuf, m_wBufLen);
		m_pStreamBuf = NULL;
	}
}

HRESULT CDiskFileStorage::OpenFile(LPCSTR lpszFileName, LPCSTR lpszFlag)
{
	if ( NULL == lpszFileName || NULL == lpszFlag ) return E_FAIL;

	m_pFile = fopen(lpszFileName, lpszFlag);
	return (m_pFile != NULL) ? S_OK : E_FAIL;
}

void CDiskFileStorage::CloseFile(FILE** ppFile)
{
	if ( *ppFile != NULL )
	{
		fclose(*ppFile);
		*ppFile = NULL;
	}
}

//返回值：
//S_OK				从磁盘上读取参数文件成功
//S_FALSE			数据校验失败,流为空
//E_FAIL			底层操作失败
//E_OUTOFMEMORY		内存分配失败
HRESULT CDiskFileStorage::Initialize(LPCSTR lpszFileName)
{
	Clear();
	m_fInited = FALSE;

	CFastCrc32 cFastCrc;

	//分配内存空间
	m_pStreamBuf = (BYTE8*)HV_AllocMem(m_wBufLen);
	if ( NULL == m_pStreamBuf )
	{
		Clear();
		return E_OUTOFMEMORY;
	}
	HV_memset(m_pStreamBuf, 0, m_wBufLen);

	DWORD32 dwReadLen = 0;
	DWORD32 dwHeadLen = sizeof(STREAM_INFO);

	strcpy(m_szFileName, lpszFileName);

	//打开文件，读取流信息
	HRESULT hr = OpenFile(m_szFileName, "rb");
	if ( S_OK != hr )
	{
	    //还没有参数配置文件
		CloseFile(&m_pFile);
		m_fInited = TRUE;
		return S_FALSE;
	}
	else
	{
		dwReadLen = fread((void*)&m_cStreamInfo, 1, sizeof(STREAM_INFO), m_pFile);
		if ( dwReadLen != dwHeadLen )
		{
			Clear();
			return E_FAIL;
		}
	}

	BOOL fValid = FALSE;
	if ( tagHvParamDataStream == m_cStreamInfo.Tag )
	{
		if ( 0 != fseek(m_pFile, dwHeadLen, SEEK_SET) )
		{
			Clear();
			return E_FAIL;
		}
		else
		{
			dwReadLen = fread(m_pStreamBuf, 1, m_cStreamInfo.StreamLen, m_pFile);
			if ( dwReadLen != m_cStreamInfo.StreamLen )
			{
				Clear();
				return E_FAIL;
			}
			else
			{
                //计算校验值
                DWORD32 dwCrc32 = cFastCrc.CalcCrc32(0, m_pStreamBuf, m_cStreamInfo.StreamLen);
                fValid = (m_cStreamInfo.Crc32 == dwCrc32);
			}
		}
	}

	if (!fValid)
	{
		m_cStreamInfo.Tag = tagHvParamDataStream;
		m_cStreamInfo.Flag = 0;
		m_cStreamInfo.StreamLen = 0;
		m_cStreamInfo.Crc32 = 0;

		m_nCurPos = 0;
		HV_memset(m_pStreamBuf, 0, m_wBufLen);
	}

	CloseFile(&m_pFile);
	m_fInited = TRUE;

	return fValid?S_OK:S_FALSE;
}

//返回值：
//S_OK:			读取指定长度数据成功
//S_FALSE:		读取失败或者到流尾
//E_POINTER:	指针错误
HRESULT STDMETHODCALLTYPE CDiskFileStorage::Read(
	PVOID pv,
	UINT cb,
	PUINT pcbRead
	)
{
	if (!m_fInited) return E_OBJ_NO_INIT;
	if (pv == NULL) return E_POINTER;
	if (pcbRead != NULL) *pcbRead = 0;
	if (cb == 0) return S_OK;

	int nMaxReadLen = MIN_INT((int)cb, (m_cStreamInfo.StreamLen-m_nCurPos));
	if (nMaxReadLen <= 0) return S_FALSE;

	HV_memcpy(pv, m_pStreamBuf + m_nCurPos, nMaxReadLen);
	m_nCurPos += nMaxReadLen;

	if (pcbRead != NULL) *pcbRead = nMaxReadLen;

	return ((int)cb == nMaxReadLen)?S_OK:S_FALSE;
}

//返回值：
//S_OK:			写入指定长度数据成功
//S_FALSE:		写入失败或者到达流长度限制
//E_POINTER:	指针错误
HRESULT STDMETHODCALLTYPE CDiskFileStorage::Write(
	const void* pv,
	UINT cb,
	PUINT pcbWritten
	)
{
	if (!m_fInited) return E_OBJ_NO_INIT;
	if (pv == NULL) return E_POINTER;
	if (pcbWritten != NULL) *pcbWritten = 0;
	if (cb == 0) return S_OK;

	int nMaxWriteLen = MIN_INT((int)cb, (m_wBufLen-m_nCurPos));
	if (nMaxWriteLen <= 0) return S_FALSE;

	HV_memcpy(m_pStreamBuf + m_nCurPos, pv, nMaxWriteLen);
	m_nCurPos += nMaxWriteLen;
	m_cStreamInfo.StreamLen += nMaxWriteLen;

	if ( pcbWritten != NULL ) *pcbWritten = nMaxWriteLen;

	return ((int)cb == nMaxWriteLen)?S_OK:S_FALSE;
}

//返回值：
//S_OK:		操作成功
//E_FAIL:	指定位置错误, 操作失败
HRESULT STDMETHODCALLTYPE CDiskFileStorage::Seek(
	INT iOffset,
	STREAM_SEEK ssOrigin,
	PUINT pnNewPosition
	)
{
	if (!m_fInited) return E_OBJ_NO_INIT;

	if (pnNewPosition != NULL) *pnNewPosition = m_nCurPos;

	int nNewPos = 0;

	switch(ssOrigin)
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

	if (nNewPos < 0 || nNewPos >= m_wBufLen)
	{
		return E_FAIL;
	}
	else
	{
		m_nCurPos = nNewPos;
	}

	if (pnNewPosition != NULL) *pnNewPosition = m_nCurPos;

	return S_OK;
}

//返回值：
//S_OK:		操作成功
//E_FAIL:	操作失败
HRESULT STDMETHODCALLTYPE CDiskFileStorage::Commit(
	DWORD32 grfCommitFlags/*=0*/
	)
{
	if (!m_fInited) return E_OBJ_NO_INIT;

	WORD16 wCommitedLen = 0;

	CFastCrc32 cFastCrc;
	m_cStreamInfo.Crc32 = cFastCrc.CalcCrc32(0, m_pStreamBuf, m_cStreamInfo.StreamLen);

	HRESULT hr = OpenFile(m_szFileName, "wb");
	if(FAILED(hr)) return hr;
	wCommitedLen = fwrite(&m_cStreamInfo, 1, sizeof(STREAM_INFO), m_pFile);
	if ( wCommitedLen != sizeof(STREAM_INFO) )
	{
		return E_FAIL;
	}

	if ( 0 == fseek(m_pFile, sizeof(STREAM_INFO), SEEK_SET) )
	{
		wCommitedLen += fwrite(m_pStreamBuf, 1, m_cStreamInfo.StreamLen, m_pFile);
		if ( wCommitedLen != (sizeof(STREAM_INFO) + m_cStreamInfo.StreamLen) )
		{
			return E_FAIL;
		}
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}
