#ifndef _MEMORYSTREAM_H_
#define _MEMORYSTREAM_H_

#include "HvStream.h"

namespace HiVideo
{
	typedef struct _MemoryStreamParam
	{
		//注：以下两种初始化参数只能设置一种

		//---第一种初始化参数
		DWORD32 nAllocSize;
		//---

		//---第二种初始化参数
		BYTE8* pbBuf;
		DWORD32 dwLen;
		//---

        _MemoryStreamParam()
		{
			nAllocSize = 0;
			pbBuf      = NULL;
			dwLen      = 0;
		};

	} MEMORYSTREAM_PARAM;  // 内存流的相关初始化参数

	//内存流
	class CMemoryStream : public CHvStream
	{
	public:
		CMemoryStream();
		virtual ~CMemoryStream();

	public:
		HRESULT Initialize(
			MEMORYSTREAM_PARAM* pParam
			);

		HRESULT Close(
			void
			);

		HRESULT Read(
			PVOID pv,
			UINT cb,
			PUINT pcbRead
			);

		HRESULT Write(
			LPCVOID pv,
			UINT cb,
			PUINT pcbWritten
			);

		HRESULT Seek(
			INT dlibMove,
			STREAM_SEEK ssOrigin,
			PUINT pnNewPosition
			);

		HRESULT SetSize(
			UINT nNewSize
			);

		HRESULT GetBuf(
			BYTE8** ppBuf,
			DWORD32* pSize
			);

		HRESULT CopyTo(
			CMemoryStream *pstm,
			UINT cb,
			PUINT pcbRead,
			PUINT pcbWritten
			);

		HRESULT GetVerifyInfo(
			DWORD32* pCrc,
			DWORD32* pSize
			);

	protected:
		inline BYTE8* Point2CurPos()
		{
			return (m_pbBuf + m_nCurPos);
		};

		BYTE8* m_pbBuf;

		DWORD32 m_nAllocSize;
		DWORD32 m_nCurPos;
		DWORD32 m_nStreamSize;

		BOOL m_fStaticMemory;

		BOOL m_fInitialized;
	};
}

#endif
