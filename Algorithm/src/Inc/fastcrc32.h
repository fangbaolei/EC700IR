#ifndef _FASTCRC32_H_
#define _FASTCRC32_H_

//from zlib

#include "swbasetype.h"

#ifdef __cplusplus

class CFastCrc32
{
public:
	static UINT CalcCrc32(UINT nCrc32,BYTE8* pbBuf,UINT nLen);
};

extern "C" UINT CalcFastCrc32(UINT nCrc32,BYTE8* pbBuf,UINT nLen);

#else

UINT CalcFastCrc32(UINT nCrc32,BYTE8* pbBuf,UINT nLen);

#endif

#endif
