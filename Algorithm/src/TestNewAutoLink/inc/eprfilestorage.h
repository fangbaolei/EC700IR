#ifndef _EPRFILESTORAGE_H_
#define _EPRFILESTORAGE_H_

#include "HvInterface.h"

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
		if ( iid == HvCore::IID_IUnknown ||
			iid == HvCore::IID_IHvStream )
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
	CEprFileStorage( DWORD32 dwStartAddr, WORD16 wMaxDatLen);
	~CEprFileStorage();

	HRESULT Initialize(LPCSTR lpszFileName);
	HRESULT Invalide();
		
protected:
	BYTE8* m_pEprBuf;
	BYTE8* m_pStreamBuf;
	
	WORD16 m_nCurPos;
	
	DWORD32 m_dwStartAddr;
	WORD16 m_wBufLen;
	
	BOOL m_fInited;
	
	typedef struct _STREAM_INFO
	{
		WORD16 Tag;
		DWORD32 Flag;
		WORD16 StreamLen;
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
};

#endif
