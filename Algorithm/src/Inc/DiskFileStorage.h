/*
 *	(C)版权所有 2010 北京信路威科技发展有限公司
 */

/**
* @file		DiskFileStorage.h
* @version	1.0
* @brief	供CParamStore类进行底层读写的流类，用数据CRC校验及缓存功能。
* @author	Shaorg
* @date		2010-8-4
*/

#ifndef _DISKFILESTORAGE_H_
#define _DISKFILESTORAGE_H_

#include "hvutils.h"
#include "hvinterface.h"

#define tagHvParamDataStream 0x7689  //Comment by Shaorg : 之前的一个魔数，意义不明确，于是将其明确化。

class CDiskFileStorage : public HvCore::IHvStream
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

public:
    //特别提醒：“wMaxDatLen是WORD16类型，所以：“64*1024 - 1”是最大值！不要超过了。
	CDiskFileStorage(WORD16 wMaxDatLen);
	virtual ~CDiskFileStorage();

public:
	HRESULT Initialize(LPCSTR lpszFileName);

protected:
	void Clear();
	HRESULT OpenFile(LPCSTR lpszFileName, LPCSTR lpszFlag);
	void CloseFile(FILE** ppFile);

protected:
	BOOL m_fInited;

	FILE* m_pFile;
	CHAR m_szFileName[MAX_PATH];

	WORD16 m_nCurPos;
	WORD16 m_wBufLen;
	BYTE8* m_pStreamBuf;

	typedef struct _STREAM_INFO
	{
		WORD16 Tag;
		DWORD32 Flag;
		WORD16 StreamLen;
		DWORD32 Crc32;

		_STREAM_INFO()
		{
			Tag = tagHvParamDataStream;
			Flag = 0;
			StreamLen = 0;
			Crc32 = 0;
		}

	} STREAM_INFO;

	STREAM_INFO m_cStreamInfo;
};

#endif
