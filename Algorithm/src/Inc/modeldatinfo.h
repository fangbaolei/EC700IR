#ifndef _MODEL_DAT_INFO_H
#define _MODEL_DAT_INFO_H

#include "swbasetype.h"

extern int GetRecogFileID( LPCSTR szName);
extern int GetDetFileID( LPCSTR szName);

extern HRESULT GetRecogModelList(LPCSTR* rgName, DWORD32* pdwCount );
extern HRESULT LoadRecogDat(
					 LPCSTR szName,
					 void* pDat,
					 DWORD32* pdwDatLen,
					 DWORD32 dwFlag = 0
					 );

extern HRESULT GetDetModelList(LPCSTR* rgName, DWORD32* pdwCount );
extern HRESULT LoadDetDat(
					LPCSTR szName,
					 void* pDat,
					 DWORD32* pdwDatLen,
					 DWORD32 dwFlag = 0
					 );

extern DWORD32 g_dwDetectorModelVersion;

#define ENCRYPTMODEL_TAG 0x69677768

typedef struct _ENCRYPTMODEL_HEAD
{
	DWORD32 dwTag;
	DWORD32 dwDatLen;
	DWORD32 dwFlag;
	DWORD32 dwOffset;	//实际数据相对文件起始位置的偏移量

	_ENCRYPTMODEL_HEAD()
	{
		dwTag = 0;
		dwDatLen = 0;
		dwFlag = 0;
		dwOffset = 0;
	};
}
ENCRYPTMODEL_HEAD;

#endif
