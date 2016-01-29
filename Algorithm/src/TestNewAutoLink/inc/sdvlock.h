
#pragma once

#include "LockDef.h"
#include <windows.h>
#include "PlateRecogParam.h"

struct BASIC_INFO
{
	DWORD dwMajorVerNo;
	DWORD dwMinorVerNo;
	DWORD dwTimeLimit;
	WCHAR wszUsage[15];
	WCHAR wszContactInfo[30];
};

typedef BASIC_INFO *PBASIC_INFO;

void ShowBasicInfo(PBASIC_INFO pBasicInfo);
