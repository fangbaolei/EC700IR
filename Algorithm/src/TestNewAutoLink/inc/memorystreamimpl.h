#pragma once

#include "HvStream.h"

namespace HiVideo
{

class CMemoryStreamImpl : public IMemoryStream
{
public:
	CMemoryStreamImpl(void);
	~CMemoryStreamImpl(void);

	HVMETHOD(Initialize)(
		DWORD32 nAllocSize
	);

	HVMETHOD(Initialize)(
		BYTE8* pbBuf,
		DWORD32 dwLen
	);

	HVMETHOD(Close)();

	HVMETHOD(GetBuf)(
		BYTE8** ppBuf,
		DWORD32* pSize
	);

	HVMETHOD(Read)(
		PVOID pv,
		UINT cb,
		PUINT pcbRead
	);

	HVMETHOD(Write)( 
		LPCVOID pv,
		UINT cb,
		PUINT pcbWritten
	);

	HVMETHOD(Seek)(
		INT dlibMove,
		STREAM_SEEK ssOrigin,
		PUINT plibNewPosition
	);
	
	HVMETHOD(GetVerifyInfo)(
		DWORD32* pCrc, 
		DWORD32* pSize
	);

	HVMETHOD(SetSize)( 
		UINT libNewSize
	);

	HVMETHOD(CopyTo)(
		IStream *pstm,
		UINT cb,
		PUINT pcbRead,
		PUINT pcbWritten
	);
	
	HVMETHOD(Release)();


protected:
	inline BYTE8* Point2CurPos()
	{
		return (m_pbBuf + m_nCurPos);
	}

	BYTE8* m_pbBuf;

	DWORD32 m_nAllocSize;
	DWORD32 m_nCurPos;
	DWORD32 m_nStreamSize;

	BOOL m_fStaticMemory;

	BOOL m_fInitialized;
};

}
