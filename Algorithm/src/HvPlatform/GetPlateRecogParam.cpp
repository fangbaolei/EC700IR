#include "hvutils.h"
#include "platerecogparam.h"
#include "swwinerror.h"

#define ARRSIZE(a) sizeof(a)/sizeof(a[0])

static LPCSTR rgkszParamName[] =
{
#define DEFINE_PARAM_NAME_CHAR(a, b) b,
    RECOG_MODE_LIST(DEFINE_PARAM_NAME_CHAR)
};

static PlateRecogParam g_CurrentParam;
PlateRecogParam* GetCurrentParam()
{
	return &g_CurrentParam;
}

PlateRecogMode GetCurrentMode()
{
	return g_CurrentParam.g_PlateRcogMode;
}

HRESULT LoadPlateRecogParam(DWORD32 dwIndex)
{
	if (dwIndex < ARRSIZE(g_rgPlateRecogParam))
	{
		g_CurrentParam = g_rgPlateRecogParam[dwIndex];
		return S_OK;
	}
	else
	{
		return E_INVALIDARG;
	}
}

HRESULT GetPlateRecogParamCount(PDWORD32 pdwCount)
{
	if (pdwCount == NULL)
	{
		return E_POINTER;
	}
	*pdwCount=ARRSIZE(g_rgPlateRecogParam);
	return S_OK;
}

int GetRecogParamIndexOnName(LPCSTR lpszName)
{
    int iIndex = 0;
    int iParamCount = ARRSIZE(g_rgPlateRecogParam);
    for (iIndex = 0; iIndex < iParamCount; iIndex++)
    {
        if (strcmp(lpszName, rgkszParamName[iIndex]) == 0)
        {
            break;
        }
    }
    if (iIndex >= iParamCount)
    {
        iIndex = 0;
    }
    return iIndex;
}

HRESULT GetRecogParamNameOnIndex(DWORD32 dwIndex, char *pszName)
{
    DWORD32 dwParamCount = ARRSIZE(g_rgPlateRecogParam);

    if (dwIndex >= dwParamCount)
    {
        return E_FAIL;
    }

    if (pszName == NULL)
	{
		return E_POINTER;
	}

    strcpy(pszName, rgkszParamName[dwIndex]);

    return S_OK;
}


