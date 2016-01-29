#ifndef _FILESTREAM_H_
#define _FILESTREAM_H_

#include "HvStream.h"

#if RUN_PLATFORM == PLATFORM_DSP_BIOS
#include "hvflashfile.h"
#else
#include <stdio.h>
#endif

namespace HiVideo
{

class CFileStreamImpl : public IFileStream
{
public:
	CFileStreamImpl(void);
	~CFileStreamImpl(void);

#if RUN_PLATFORM == PLATFORM_DSP_BIOS

	typedef enum {
		STORE_TYPE_FLASH		= 0,		// FLASH
		STORE_TYPE_EEPROM	= 1,		// EEPROM
		STORE_TYPE_UNKNOWN
	} 
	STORE_TYPE;

	HVMETHOD(Initialize)(
		WORD16 wFileSerialNum,
		BOOL fWrite
		);

#endif // RUN_PLATFORM == PLATFORM_DSP_BIOS
	HVMETHOD(Initialize)(
		const char* szFilePath,
		BOOL fWrite
		);

	HVMETHOD(Close)(void);

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

	HVMETHOD(Release)()
	{
		delete this;
		return HR_S_OK;
	}

protected:
	BOOL m_fWrite;

#if RUN_PLATFORM == PLATFORM_DSP_BIOS

	FILE_HANDLE m_fp;
	
	STORE_TYPE m_nStoreType;	

#else
	FILE* m_fp;
#endif

	BOOL m_fInitialized;
};

}

#endif

