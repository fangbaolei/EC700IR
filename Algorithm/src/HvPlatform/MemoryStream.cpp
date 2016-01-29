#include "MemoryStream.h"
#include "fastcrc32.h"

namespace HiVideo
{
	CMemoryStream::CMemoryStream()
	{
		m_pbBuf = NULL;

		m_nAllocSize=0;
		m_nCurPos=0;
		m_nStreamSize=0;

		m_fStaticMemory = FALSE;

		m_fInitialized=FALSE;
	}

	CMemoryStream::~CMemoryStream()
	{
		Close();
	}

	HRESULT CMemoryStream::Initialize(
		MEMORYSTREAM_PARAM* pParam
		)
	{
	    //第一种初始化参数
		DWORD32 nAllocSize = pParam->nAllocSize;

		//第二种初始化参数
		BYTE8* pbBuf = pParam->pbBuf;
		DWORD32 dwLen = pParam->dwLen;

		if ( 0 != nAllocSize )
		{
			if (nAllocSize == 0) return E_INVALIDARG;

			Close();

			m_pbBuf=new BYTE8[nAllocSize];
			if (!m_pbBuf) return E_OUTOFMEMORY;

			memset(m_pbBuf,0,nAllocSize);

			m_nAllocSize=nAllocSize;
			m_nCurPos=0;
			m_nStreamSize=0;

			m_fInitialized = TRUE;

			return S_OK;
		}
		else
		{
			if (pbBuf == NULL) return E_INVALIDARG;

			Close();

			m_pbBuf = pbBuf;

			m_nAllocSize = dwLen;
			m_nCurPos = 0;
			m_nStreamSize = dwLen;

			m_fStaticMemory = TRUE;
			m_fInitialized = TRUE;

			return S_OK;
		}
	}

	HRESULT CMemoryStream::Close(
		void
		)
	{
		if (!m_fInitialized) return S_OK;

		if ( !m_fStaticMemory && m_pbBuf != NULL)
		{
			delete[] m_pbBuf;
			m_pbBuf = NULL;
		}

		m_nCurPos = 0;
		m_nStreamSize = 0;

		m_fStaticMemory = FALSE;
		m_fInitialized = FALSE;

		return S_OK;
	}

	HRESULT CMemoryStream::Read(
		PVOID pv,
		UINT cb,
		PUINT pcbRead
		)
	{
		if (pcbRead) *pcbRead=0;

		if (cb == 0 ) return S_OK;
		if (m_nCurPos >= m_nStreamSize) return S_FALSE;

		if (!pv) return E_INVALIDARG;

		DWORD32 nReadLen = min(m_nStreamSize - m_nCurPos, cb);

		memcpy(pv,Point2CurPos(),nReadLen);

		m_nCurPos += nReadLen;

		if (pcbRead) *pcbRead = nReadLen;

		return nReadLen==cb?S_OK:S_FALSE;
	}

	HRESULT CMemoryStream::Write(
		LPCVOID pv,
		UINT cb,
		PUINT pcbWritten
		)
	{
		//HRESULT hr;

		if (pcbWritten) *pcbWritten=0;

		if (cb == 0) return S_OK;

		if (!pv) return E_INVALIDARG;

		DWORD32 old_cb = cb;

		if ( (m_nCurPos + cb) > m_nAllocSize )
		{
			if (m_fStaticMemory)
			{
				cb = m_nAllocSize - m_nCurPos;
			}
			else
			{
				RTN_HR_IF_FAILED( SetSize( (m_nCurPos + cb) * 6 / 5 ) );
			}
		}

		memcpy(Point2CurPos(),pv,cb);

		m_nCurPos+=cb;

		if (m_nCurPos > m_nStreamSize) m_nStreamSize = m_nCurPos;

		if (pcbWritten) *pcbWritten = cb;

		return (cb == old_cb)?S_OK:S_FALSE;
	}

	HRESULT CMemoryStream::Seek(
		INT dlibMove,
		STREAM_SEEK ssOrigin,
		PUINT plibNewPosition
		)
	{
		if (plibNewPosition)
		{
			*plibNewPosition = m_nCurPos;
		}

		LONG nNewPos = 0;

		switch (ssOrigin)
		{
		case STREAM_SEEK_SET:
			nNewPos = dlibMove;
			break;
		case STREAM_SEEK_END:
			nNewPos = m_nStreamSize + dlibMove;
			break;
		case STREAM_SEEK_CUR:
			nNewPos = m_nCurPos + dlibMove;
			break;
		default:
			break;
		}

		if (nNewPos < 0) nNewPos = 0;
		if ((nNewPos > (int)m_nStreamSize) && m_fStaticMemory) nNewPos = m_nStreamSize;

		m_nCurPos = nNewPos;

		if (plibNewPosition)
		{
			*plibNewPosition = m_nCurPos;
		}

		return S_OK;
	}

	HRESULT CMemoryStream::SetSize(
		UINT libNewSize
		)
	{
		if (!m_fInitialized) return E_FAIL;
		if (m_fStaticMemory) return E_NOTIMPL;

		BYTE8* pbTemp = new BYTE8[libNewSize];
		if (!pbTemp) return E_OUTOFMEMORY;

		memset(pbTemp,0,libNewSize);
		memcpy(pbTemp,m_pbBuf,m_nStreamSize);

		delete[] m_pbBuf;
		m_pbBuf=pbTemp;

		m_nAllocSize=libNewSize;

		return S_OK;
	}

	HRESULT CMemoryStream::GetBuf(
		BYTE8** ppBuf,
		DWORD32* pSize
		)
	{
		if (!ppBuf || !pSize) return E_INVALIDARG;
		*ppBuf = m_pbBuf;
		*pSize = m_nStreamSize;
		return S_OK;
	}

	HRESULT CMemoryStream::CopyTo(
		CMemoryStream *pstm,
		UINT cb,
		PUINT pcbRead,
		PUINT pcbWritten
		)
	{
		if (pcbWritten) pcbWritten=0;
		if (pcbRead) pcbRead=0;

		if ( !pstm ) return E_INVALIDARG;

		BYTE8 buf[g_kcCopyBuf]={0};

		UINT nReadLen,nWriteLen;
		nReadLen=0;
		nWriteLen=0;

		UINT nReadCount(0) , nWriteCount(0) , nMaxReadLen(0);

		do
		{
			nMaxReadLen = min((UINT)g_kcCopyBuf, cb);
			RTN_HR_IF_FAILED( Read(buf, nMaxReadLen, &nReadCount) );
			RTN_HR_IF_FAILED( pstm->Write(buf, nReadCount, &nWriteCount) );

			nReadLen += nReadCount;
			nWriteLen += nWriteCount;

			if ( nReadCount != nMaxReadLen || nWriteCount != nReadCount) break;

			cb -= nReadCount;
		}
		while(cb>0);

		if (pcbRead)
		{
			*pcbRead = nReadLen;
		}
		if (pcbWritten)
		{
			*pcbWritten = nWriteLen;
		}

		return S_OK;
	}

	HRESULT CMemoryStream::GetVerifyInfo(
		DWORD32* pCrc,
		DWORD32* pSize
		)
	{
		if (!m_fInitialized) return E_FAIL;
		if (!pCrc || !pSize) return E_INVALIDARG;

		*pCrc = 0;
		*pSize = 0;

		UINT nOldPos=0;
		Seek(0,STREAM_SEEK_CUR,&nOldPos);
		Seek(0,STREAM_SEEK_SET,NULL);

		UINT nCrc32=0;

		UINT nReadCount(0),nReadLen(0);

		BYTE8 buf[g_kcCopyBuf]={0};

		do
		{
			Read(buf,g_kcCopyBuf,&nReadCount);

			nReadLen+=nReadCount;
			nCrc32=CFastCrc32::CalcCrc32(nCrc32,buf,nReadCount);
		}
		while(nReadCount>0);

		if(pCrc != NULL) *pCrc = nCrc32;
		if(pSize != NULL) *pSize = nReadLen;

		Seek(nOldPos,STREAM_SEEK_SET,NULL);

		return S_OK;
	}
}
