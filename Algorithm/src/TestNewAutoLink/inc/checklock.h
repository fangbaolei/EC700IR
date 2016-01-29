#ifndef _CHECKLOCK_H_
#define _CHECKLOCK_H_

#include "swBaseType.h"

#if defined(RELEASE_TO_MARKET) || defined(RELEASE_TO_IN)

#include "sense4.h"
#include "SenseLock.h"
#include "crypt2.h"
#include "LockDef.h"
#include "SdvLock.h"

#define USE_SENTINELKEY

#ifdef USE_SENTINELKEY
#include "SentinelKeys.h"
extern SP_HANDLE g_licHandle;
extern DWORD32 g_dwStringID;
#endif

extern R_RSA_PUBLIC_KEY rPubKey;

// 检查是否支持RDTSC指令
inline static int isRDTSCsupported(void)
{
	int f;
	_asm 
	{
		mov eax,    01h;
		cpuid;
		mov f,      edx
	}
	return ((f & 0x10) >> 4);
}

inline static void GetRandom(PBYTE pBuf, int iSize)
{
	unsigned int clockL, clockH;

	if (isRDTSCsupported())
	{
		__asm // 读取CPU上电以来的CLOCK计数
		{ 
			rdtsc;
			mov clockL, eax;
			mov clockH, edx;
		}
	} else
	{
		clockL = GetTickCount(); // 使用GetTickCount取数据
	}

	srand(clockL);
	for (int i = 0; i < iSize; i++)
	{
		pBuf[i] = rand();
	}
}

extern BOOL g_fLockInitialized;
extern CRITICAL_SECTION g_csSenseLock;
extern SENSE4_CONTEXT LockContext;

void InitializeLock();
void ReleaseLock();
DWORD GetRunDuration();

#endif

void InitializeLockCriticalSection();
void ReleaseLockCriticalSection();

inline DWORD32 CheckLock(WCHAR *pszLockID = NULL)
{
#if defined(RELEASE_TO_MARKET) || defined(RELEASE_TO_IN)
	EnterCriticalSection(&g_csSenseLock);

	if (!g_fLockInitialized) InitializeLock();

	int nRet = ERROR_INSTALL_PACKAGE_VERSION;
	char rgbInput[250]={0};
	char rgbOutput[250]={0};
	DWORD dwRet(S4_SUCCESS), dwReturned(0);
	BASIC_INFO BasicInfo;

	rgbInput[0]=LI_GETINFO;
	DWORD32 dwRunDuration = GetRunDuration();
	memcpy(&rgbInput[1], &dwRunDuration, sizeof(DWORD32));
	GetRandom((PBYTE)&rgbInput[1 + sizeof(DWORD32)], RAND_DATA_SIZE);
	dwRet=S4Execute(
		&LockContext, GETINFOFILENAME, 
		rgbInput, 1 + sizeof(DWORD32) + RAND_DATA_SIZE, 
		rgbOutput, sizeof rgbOutput, &dwReturned
		);
	if (dwRet != S4_SUCCESS)
	{
		ReleaseLock();
		InitializeLock();
		dwRet=S4Execute(
			&LockContext, GETINFOFILENAME, 
			rgbInput, 5, 
			rgbOutput, sizeof rgbOutput, &dwReturned
			);
	}
	if (dwRet==S4_SUCCESS)
	{
		if (rgbOutput[0]==0)
		{
			memcpy(&BasicInfo, &rgbOutput[1], sizeof(BasicInfo));
			if (BasicInfo.dwMajorVerNo>MAJORVER ||
				BasicInfo.dwMajorVerNo==MAJORVER &&
				BasicInfo.dwMinorVerNo>=MINORVER)
			{
				DWORD dwSignatureSize = dwReturned - 1 - sizeof(BasicInfo);
				if (dwSignatureSize == RAND_DATA_SIZE)
				{
					//校验签名
					PBYTE pbPlain = (PBYTE)&rgbInput[1 + sizeof(DWORD32)];
					PBYTE pbSignature = (PBYTE)&rgbOutput[1 + sizeof(BasicInfo)];
					if (Verify(DA_SHS, pbPlain, RAND_DATA_SIZE, pbSignature, RAND_DATA_SIZE, &rPubKey) == RE_SUCCESS)
					{
						nRet = ERROR_SUCCESS;
						if (pszLockID != NULL)
						{
							memcpy(pszLockID, BasicInfo.wszContactInfo, sizeof(BasicInfo.wszContactInfo));
						}
					}
				}
			}
		}
		else
		{
			if (dwReturned>=sizeof(rgbOutput))
			{
				dwReturned=sizeof(rgbOutput)-1;
			}
			rgbOutput[dwReturned]=0;
		}
	}
	else
	{
		ReleaseLock();
	}
#ifdef USE_SENTINELKEY
	if (nRet == ERROR_SUCCESS)
	{
		SP_CHAR szBuffer[100];
		memset(szBuffer, 0, sizeof(szBuffer));
		if (SFNTReadString(g_licHandle, g_dwStringID, szBuffer, sizeof(szBuffer)) == SP_SUCCESS)
		{
			SPP_CHAR pszCheckID = "Signalway Product HighApp300 RTM.";
			if (strcmp(pszCheckID, szBuffer) != 0)
			{
				nRet = ERROR_INSTALL_PACKAGE_VERSION;
			}
		}
		else
		{
			ReleaseLock();
			nRet = ERROR_INSTALL_PACKAGE_VERSION;
		}
	}
#endif
	LeaveCriticalSection(&g_csSenseLock);
	return nRet;
#else
	return 0;
#endif
}

#endif
