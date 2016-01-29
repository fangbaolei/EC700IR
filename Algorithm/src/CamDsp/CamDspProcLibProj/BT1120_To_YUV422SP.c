#include "CamDspProc.h"

extern DMA_HANDLE h1;
extern DMA_HANDLE h2;
extern DMA_HANDLE h3;
extern DMA_HANDLE h4;
extern DMA_HANDLE h5;
extern DMA_HANDLE h6;

extern unsigned char g_bUserOnChipRAM[];
#define g_bBufLine1 (g_bUserOnChipRAM)
#define g_bBufLine2 (g_bUserOnChipRAM+2448)
#define g_bBufLine3 (g_bUserOnChipRAM+4896)
#define g_bBufLine4 (g_bUserOnChipRAM+7344)
#define g_bBufLine5 (g_bUserOnChipRAM+9792)
#define g_bBufLine6 (g_bUserOnChipRAM+12240)

// H264
void BT1120_2448_1024_To_YUV422SP_1216_1024(
	PBYTE8 pbSrcY,
	PBYTE8 pbSrcUV,
	PBYTE8 pbDstY,
	PBYTE8 pbDstUV
)
{
	int i,j;

	PDWORD32 pdwYIn = NULL;
	PWORD16 pwYOut = NULL;
	DWORD32 dwYInTmp;

	PDWORD32 pdwUVIn = NULL;
	PWORD16 pwUVOut = NULL;
	DWORD32 dwUVInTmp;

	memcpy(g_bBufLine1, pbSrcY, 2448);
	memcpy(g_bBufLine4, pbSrcUV, 2448);
	pbSrcY += 2448;
	pbSrcUV += 2448;

	for ( i = 1; i < 1024; ++i )
	{
		//in
		DmaCopy1D(h2, g_bBufLine2, pbSrcY, 2448);
		DmaCopy1D(h5, g_bBufLine5, pbSrcUV, 2448);
		pbSrcY += 2448;
		pbSrcUV += 2448;

		//proc
		DmaWaitFinish(h3);
		DmaWaitFinish(h6);
		pdwYIn = (PDWORD32)g_bBufLine1;
		pwYOut = (PWORD16)g_bBufLine3;
		pdwUVIn = (PDWORD32)g_bBufLine4;
		pwUVOut = (PWORD16)g_bBufLine6;

		for ( j = 0; j < 608; ++j )
		{
			dwYInTmp = *(pdwYIn++);
			*(pwYOut++) = ((dwYInTmp&0xff) | ((dwYInTmp>>8) & 0xff00));

			dwUVInTmp = *(pdwUVIn++);
			*(pwUVOut++) = (WORD16)dwUVInTmp;
		}

		//out
		DmaCopy1D(h3, pbDstY, g_bBufLine3, 1216);
		DmaCopy1D(h6, pbDstUV, g_bBufLine6, 1216);
		pbDstY += 1216;
		pbDstUV += 1216;

		DmaWaitFinish(h2);
		DmaWaitFinish(h5);
		memcpy(g_bBufLine1, g_bBufLine2, 2448);
		memcpy(g_bBufLine4, g_bBufLine5, 2448);
	}

	// 处理最后一行

	//proc
	DmaWaitFinish(h3);
	DmaWaitFinish(h6);
	pdwYIn = (PDWORD32)g_bBufLine1;
	pwYOut = (PWORD16)g_bBufLine3;
	pdwUVIn = (PDWORD32)g_bBufLine4;
	pwUVOut = (PWORD16)g_bBufLine6;

	for ( j = 0; j < 608; ++j )
	{
		dwYInTmp = *(pdwYIn++);
		*(pwYOut++) = ((dwYInTmp&0xff) | ((dwYInTmp>>8) & 0xff00));

		dwUVInTmp = *(pdwUVIn++);
		*(pwUVOut++) = (WORD16)dwUVInTmp;
	}

	//out
	DmaCopy1D(h3, pbDstY, g_bBufLine3, 1216);
	DmaCopy1D(h6, pbDstUV, g_bBufLine6, 1216);

	DmaWaitFinish(h3);
	DmaWaitFinish(h6);
}
