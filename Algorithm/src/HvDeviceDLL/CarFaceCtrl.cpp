// 该文件编码格式必须为WINDOWS-936

#include "CarFaceCtrl.h"
#include "svBase/sv_error.h"

using namespace HiVideo;

#ifdef WIN32

#ifdef _CARFACE

#include "resource.h"

#ifdef _DEBUG
#pragma comment(lib, "CarFaceLib_Debug/CarFaceRecogApiD.lib")
#pragma comment(lib, "CarFaceLib_Debug/CarFaceCoreD.lib")
#pragma comment(lib, "CarFaceLib_Debug/svDet_vs2003D.lib")
#pragma comment(lib, "CarFaceLib_Debug/IQmath.x86.lib")
#pragma comment(lib, "CarFaceLib_Debug/svDetApi_vs2003D.lib")
#pragma comment(lib, "CarFaceLib_Debug/svLprCore_vs2003D.lib")
#pragma comment(lib, "CarFaceLib_Debug/svLprModel_vs2003D.lib")
#pragma comment(lib, "CarFaceLib_Debug/svBase_vs2003D.lib")
#else
#pragma comment(lib, "CarFaceLib/CarFaceRecogApi.lib")
#pragma comment(lib, "CarFaceLib/CarFaceCore.lib")
#pragma comment(lib, "CarFaceLib/svDet.lib")
#pragma comment(lib, "CarFaceLib/svDetApi.lib")
#pragma comment(lib, "CarFaceLib/svLprCore.lib")
#pragma comment(lib, "CarFaceLib/svLprModel.lib")
#pragma comment(lib, "CarFaceLib/svBase.lib")
#endif

HINSTANCE GetDllModuleHandle()
{
	static HINSTANCE hInstance = NULL;
	if (hInstance == NULL)
	{
		hInstance = GetModuleHandle("HvDevice.dll");
	}
	return hInstance;
}

int MyLoadResource(HINSTANCE hInstance, LPCSTR lpResName, LPCSTR lpResType, BYTE *pbData, DWORD *pdwDataSize)
{
	if (pbData == NULL || pdwDataSize == NULL)
	{
		(*pdwDataSize) = 0;
		return 1;
	}

	HRSRC hRes = FindResource(hInstance, lpResName, lpResType);
	if (hRes == NULL)
	{
		return 2;
	}

	HGLOBAL hResData = LoadResource(hInstance, hRes);
	if (hResData == NULL)
	{
		return 3;
	}

	void * lpvData = LockResource(hResData);
	if (lpvData == NULL)
	{
		FreeResource(hResData);
		return 4;
	}

	DWORD dwSize = SizeofResource(hInstance, hRes);
	if ((*pdwDataSize) < dwSize)
	{
		FreeResource(hResData);
		return 5;
	}

	memcpy(pbData, lpvData, dwSize);
	(*pdwDataSize) = dwSize;

	FreeResource(hResData);

	return 0;
}
#endif

#else

extern unsigned char g_rgCarFaceInfo[];
extern unsigned char g_rgCarFaceData[];
extern int g_nCarFaceInfoSize;
extern int g_nCarFaceDataSize;

#endif


#ifdef _CARFACE
CCarFaceCtrl::CCarFaceCtrl(void)
{
	m_fIsInit = FALSE;
	m_pbModule = NULL;
	m_pszInfo = NULL;
	m_dwModuleSize = 0;
	m_dwInfoSize = 0;
	m_pCarFaceCtrl = NULL;
	m_pCarFaceInfo = NULL;
	m_nCarFaceInfoCount = 0;
	ISemaphore::CreateInstance(&m_pLock, 1, 1);
}

CCarFaceCtrl::~CCarFaceCtrl(void)
{
	SAFE_DELETE_ARG(m_pbModule);
	SAFE_DELETE_ARG(m_pszInfo);
	SAFE_DELETE_ARG(m_pCarFaceInfo);


	if (m_pCarFaceCtrl)
	{
		CarFaceRecogApi::FreeCarFaceRecogCtrl(m_pCarFaceCtrl);
		m_pCarFaceCtrl = NULL;
	}

	if ( m_pLock != NULL )
	{
		delete m_pLock;
		m_pLock = NULL;
	}
}

HRESULT CCarFaceCtrl::LoadCarFaceRecogModel()
{
	if( m_pCarFaceCtrl != NULL  )
	{
		unsigned char* pbDataTemp = m_pbModule;
		unsigned char *pbModelDataNormal = NULL;
		unsigned char *pbModelDataYellow = NULL;
		int iMaxDataSize = (int)m_dwModuleSize;

		sv::SV_RESULT ret = sv::RS_S_OK;

		while(iMaxDataSize > sizeof(MODEL_DAT_HEAD))
		{
			MODEL_DAT_HEAD* pcHeader = (MODEL_DAT_HEAD*)pbDataTemp;
			iMaxDataSize -= sizeof(MODEL_DAT_HEAD);
			pbDataTemp += sizeof(MODEL_DAT_HEAD);

			if( pcHeader->nSize != sizeof(MODEL_DAT_HEAD) )
			{
				return E_INVALIDARG;
			}

			if( pcHeader->nType == CFM_NORM && pcHeader->nDatLen <= iMaxDataSize)
			{
				pbModelDataNormal = pbDataTemp;

				sv::SV_RESULT ret = m_pCarFaceCtrl->LoadSvmModel(pcHeader->nType, pbModelDataNormal, pcHeader->nDatLen);

				if( ret != sv::RS_S_OK )
				{
					return E_FAIL;
				}
			}
			else if( pcHeader->nType == CFM_BIG && pcHeader->nDatLen <= iMaxDataSize)
			{
				pbModelDataYellow = pbDataTemp;

				sv::SV_RESULT ret = m_pCarFaceCtrl->LoadSvmModel(pcHeader->nType, pbModelDataYellow, pcHeader->nDatLen);

				if( ret != sv::RS_S_OK )
				{
					return E_FAIL;
				}
			}

			pbDataTemp += pcHeader->nDatLen;
			iMaxDataSize -= pcHeader->nDatLen;
		}
		return (HRESULT)ret;
	}
	return E_INVALIDARG;
}

char* FindLastString(char *pszSrc, char *pszDst)
{
	pszSrc = strstr(pszSrc, pszDst);
	char *pszResult = NULL;
	while (pszSrc)
	{
		pszResult = pszSrc;
		pszSrc = strstr(pszSrc + strlen(pszDst), pszDst);
	}
	return pszResult;
}

void SplitString(char *pszSrc, char *pszSplitor, char ppOut[][32], int *pnOutCount)
{
	strcat(pszSrc, pszSplitor);
	char *psz1 = pszSrc;
	char *psz2 = strstr(psz1, pszSplitor);
	int nCount = 0;
	int nSplitLen = strlen(pszSplitor);
	while (psz2)
	{
		strncpy(ppOut[nCount], psz1, psz2 - psz1);
		ppOut[nCount][psz2 - psz1] = 0;
		psz1 = psz2 + nSplitLen;
		psz2 = strstr(psz1, pszSplitor);
		nCount++;
		if (nCount >= (*pnOutCount))
		{
			break;
		}
	}
	(*pnOutCount) = nCount;
}

HRESULT CCarFaceCtrl::LoadCarFaceInfo()
{
	const int MAX_SPLIT_ARR_COUNT = 8;
	char *pszT1 = m_pszInfo;
	char *pszT2 = strstr(pszT1, "\n");
	char *pszTemp;
	char szInfo[128];
	char rgSplitInfo[MAX_SPLIT_ARR_COUNT][32];
	int nStrLen = 0;
	int nOffset = strstr(m_pszInfo, "\r") ? 1 : 0;
	if (m_pszInfo[strlen(m_pszInfo) - 1] != '\n')
	{
		strcat(m_pszInfo, nOffset ? "\r\n" : "\n");
	}
	m_nCarFaceInfoCount = 0;

	while (pszT2)
	{
		nStrLen = pszT2 - pszT1 - nOffset;
		if (nStrLen <= 0)
		{
			break;
		}
		strncpy(szInfo, pszT1, nStrLen);
		szInfo[nStrLen] = 0;
		/*FILE *pFile = fopen("c:\\mydata\\log.txt", "a+");
		if (pFile)
		{
			fwrite(szInfo, 1, strlen(szInfo), pFile);
			fwrite("\r\n", 1, 2, pFile);
			fclose(pFile);
		}*/
		int nSplitCount = MAX_SPLIT_ARR_COUNT;
		SplitString(szInfo, "_", rgSplitInfo, &nSplitCount);
		if (nSplitCount >= 4)
		{
			sscanf(rgSplitInfo[0], "%x", &m_pCarFaceInfo[m_nCarFaceInfoCount].dwId);
			strcpy(m_pCarFaceInfo[m_nCarFaceInfoCount].szCarType, rgSplitInfo[3]);
			//第一级和第二级名称一样的话只输出第一级
			if (strcmp(rgSplitInfo[1], rgSplitInfo[2]) == 0)
			{
				strcpy(m_pCarFaceInfo[m_nCarFaceInfoCount].szInfo, rgSplitInfo[1]);
			}
			else
			{
				sprintf(m_pCarFaceInfo[m_nCarFaceInfoCount].szInfo, "%s_%s", rgSplitInfo[1], rgSplitInfo[2]);
			}
			m_nCarFaceInfoCount++;
			if (m_nCarFaceInfoCount >= MAX_CF_INF_COUNT)
			{
				break;
			}
		}

		pszT1 = pszT2 + 1;
		pszT2 = strstr(pszT1, "\n");
	}
	return S_OK;
}

HRESULT CCarFaceCtrl::InitCtrl(void)
{
	if (m_fIsInit)
	{
		return S_OK;
	}

	SAFE_DELETE_ARG(m_pbModule);
	SAFE_DELETE_ARG(m_pszInfo);
	SAFE_DELETE_ARG(m_pCarFaceInfo);

	m_pbModule = new BYTE[MAX_CF_DAT_SIZE];
	m_pszInfo = new char[MAX_CF_INF_SIZE];
	m_pCarFaceInfo = new CAR_FACE_INFO[MAX_CF_INF_COUNT];
	if (m_pbModule == NULL || m_pszInfo == NULL || m_pCarFaceInfo == NULL)
	{
		goto ERR;
	}

	m_dwModuleSize = MAX_CF_DAT_SIZE;
	m_dwInfoSize = MAX_CF_INF_SIZE;

#ifdef WIN32
	HINSTANCE hInstance = GetDllModuleHandle();
	if (hInstance == NULL)
	{
		goto ERR;
	}

	MyLoadResource(hInstance, MAKEINTRESOURCE(IDR_DAT_CARFACE), "DAT", m_pbModule, &m_dwModuleSize);
	memset(m_pszInfo, 0, m_dwInfoSize);
	MyLoadResource(hInstance, MAKEINTRESOURCE(IDR_TXT_CARFACE), "TXT", (BYTE*)m_pszInfo, &m_dwInfoSize);
#else

	m_dwModuleSize = (DWORD)g_nCarFaceDataSize;
	m_dwInfoSize = (DWORD)g_nCarFaceInfoSize;
	memset(m_pszInfo, 0, m_dwInfoSize);
	memcpy(m_pbModule, g_rgCarFaceData, m_dwModuleSize);
	memcpy(m_pszInfo, g_rgCarFaceInfo, m_dwInfoSize);
#endif

	if (m_dwModuleSize == 0 || m_dwInfoSize == 0)
	{
		goto ERR;
	}

	if (m_pCarFaceCtrl)
	{
		CarFaceRecogApi::FreeCarFaceRecogCtrl(m_pCarFaceCtrl);
		m_pCarFaceCtrl = NULL;
	}

	m_pCarFaceCtrl = CarFaceRecogApi::CreateCarFaceRecogCtrl();

	if (m_pCarFaceCtrl == NULL)
	{
		goto ERR;
	}

	if (S_OK != LoadCarFaceRecogModel())
	{
		goto ERR;
	}

	if (S_OK != LoadCarFaceInfo())
	{
		goto ERR;
	}

	m_fIsInit = TRUE;

	return S_OK;

ERR:
	SAFE_DELETE_ARG(m_pbModule);
	SAFE_DELETE_ARG(m_pszInfo);
	SAFE_DELETE_ARG(m_pCarFaceInfo);
	return E_FAIL;
}

HRESULT CCarFaceCtrl::FindCarFaceString(DWORD dwCarFaceID, char *pszCarFaceName, char *pszCarTpye)
{
	BOOL fFound = FALSE;
	for (int i = 0; i < m_nCarFaceInfoCount; ++i)
	{
		if (m_pCarFaceInfo[i].dwId == dwCarFaceID)
		{
			strcpy(pszCarFaceName, m_pCarFaceInfo[i].szInfo);
			strcpy(pszCarTpye, m_pCarFaceInfo[i].szCarType);
			fFound = TRUE;
			break;
		}
	}
	return fFound ? S_OK : E_FAIL;
}

HRESULT CCarFaceCtrl::RecogCarFace(sv::SV_IMAGE* pImgSrc, sv::SV_BOOL fIsNight,sv::SV_BOOL fNoPlate,sv::SV_RECT* pPlatePos,sv::SV_BOOL fYellowPlate, sv::SV_RECT* pCarFacePos, int nIDCount,sv::SV_UINT32* rgRegID)
{
	if (sv::RS_S_OK == m_pCarFaceCtrl->RecogCarFace(pImgSrc,fIsNight,fNoPlate,pPlatePos,fYellowPlate,pCarFacePos,nIDCount,rgRegID))
	{
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}

}

HRESULT CCarFaceCtrl::MatchCarFace(BYTE *pbFeature, int nFeatureLen, char *pszCarFace, char *pszCarTpye)
{
	if (pbFeature == NULL || nFeatureLen == 0 || pszCarFace == NULL || nFeatureLen < 3000)
	{
		return E_POINTER;
	}

	if (m_fIsInit == FALSE)
	{
		m_pLock->Pend();
		InitCtrl();
		m_pLock->Post();
	}

	if (m_fIsInit == FALSE)
	{
		return E_FAIL;
	}

	sv::SV_UINT32 rgMatches[1];
	sv::SV_INT32 rgConfs[1];
	sv::SV_RESULT svhr = m_pCarFaceCtrl->MatchFeatures(pbFeature, nFeatureLen, 1, rgMatches, rgConfs);
	if (svhr != sv::RS_S_OK)
	{
		return E_FAIL;
	}
	return FindCarFaceString((DWORD)rgMatches[0], pszCarFace, pszCarTpye);
}
#endif