#ifndef _FILELIST_H_
#define _FILELIST_H_

#include "Basecommon.h"

extern WORD16 GetFileList(DWORD32 dwType,DWORD32* pdwFileList);
extern HRESULT GetFileCRC(DWORD32 dwFileId,DWORD32* pdwCRC,DWORD32* pdwSize);

#endif

