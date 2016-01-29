#ifndef _INCLUDE_BUILDPLATEINFO_H_
#define _INCLUDE_BUILDPLATEINFO_H_

#include "hvvartype.h"
#include "trackercallback.h"
#include "HvResultFilter.h"

extern "C" CHvResultFilter g_resultFilter;

extern HRESULT BuildPlateString(
								CHvString &strCarInfo,
								char* pszAppendInfo,
								char* pszPlateInfo,
								BOOL &fLeach,
								bool fIsTurnArround,
								CARLEFT_INFO_STRUCT *pCarLeftInfo,
								LPVOID pvUserData);

#endif
