#ifndef _SRIO_H_
#define _SRIO_H_

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

bool SrioQueueInit(char *pszXml, int &iBufferLen);
int SrioSendData(PBYTE8 pData, int iLen);
int SrioRecvData(PBYTE8 pData, int iLen);

inline int SerializeData(PBYTE8 pData, int iLen, bool fOut)
{
#ifdef CHIP_6455
	if (fOut)
	{
		return SrioSendData(pData, iLen);
	}
	else
	{
		return SrioRecvData(pData, iLen);
	}
#else
	return 0;
#endif
}

void SrioSetLightType(int iLightType);
void SrioGetLightType(int &iLightType);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif
