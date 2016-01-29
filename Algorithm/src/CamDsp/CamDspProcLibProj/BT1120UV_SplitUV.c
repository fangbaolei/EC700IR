#include "CamDspProc.h"

extern DMA_HANDLE h1;
extern DMA_HANDLE h2;
extern DMA_HANDLE h3;
extern DMA_HANDLE h4;

extern unsigned char g_bUserOnChipRAM[];
#define g_bBufLine1 (g_bUserOnChipRAM)
#define g_bBufLine2 (g_bUserOnChipRAM+1920)
#define g_bBufLine3 (g_bUserOnChipRAM+1920+1920)
#define g_bBufLine4 (g_bUserOnChipRAM+1920+1920+1920)

// 8ms
void BT1120UV_1600_1200_SplitUV(
	PBYTE8 pbSrc,
	PBYTE8 pbDstU,
	PBYTE8 pbDstV
)
{
	int i,j;

	PDWORD32 pdwUVIn = NULL;
	PWORD16 pwUOut = NULL;
	PWORD16 pwVOut = NULL;
	DWORD32 dwUVInTmp;

	memcpy(g_bBufLine1, pbSrc, 1600);
	pbSrc += 1600;

	for ( i = 1; i < 1200; ++i )
	{
		//in
		DmaCopy1D(h2, g_bBufLine2, pbSrc, 1600);
		pbSrc += 1600;

		//proc
		DmaWaitFinish(h3);
		DmaWaitFinish(h4);
		pdwUVIn = (PDWORD32)g_bBufLine1;
		pwUOut = (PWORD16)g_bBufLine3;
		pwVOut = (PWORD16)g_bBufLine4;

		for ( j = 0; j < 400; ++j )
		{
			dwUVInTmp = *(pdwUVIn++);

			*(pwUOut++) = ((dwUVInTmp&0xff) | ((dwUVInTmp>>8) & 0xff00));
			*(pwVOut++) = (((dwUVInTmp&0xff00) >> 8) | ((dwUVInTmp>>16) & 0xff00));
		}

		//out
		DmaCopy1D(h3, pbDstU, g_bBufLine3, 800);
		DmaCopy1D(h4, pbDstV, g_bBufLine4, 800);
		pbDstU += 800;
		pbDstV += 800;

		DmaWaitFinish(h2);
		memcpy(g_bBufLine1, g_bBufLine2, 1600);
	}

	// 处理最后一行

	//proc
	DmaWaitFinish(h3);
	DmaWaitFinish(h4);
	pdwUVIn = (PDWORD32)g_bBufLine1;
	pwUOut = (PWORD16)g_bBufLine3;
	pwVOut = (PWORD16)g_bBufLine4;

	for ( j = 0; j < 400; ++j )
	{
		dwUVInTmp = *(pdwUVIn++);

		*(pwUOut++) = ((dwUVInTmp&0xff) | ((dwUVInTmp>>8) & 0xff00));
		*(pwVOut++) = (((dwUVInTmp&0xff00) >> 8) | ((dwUVInTmp>>16) & 0xff00));
	}

	//out
	DmaCopy1D(h3, pbDstU, g_bBufLine3, 800);
	DmaCopy1D(h4, pbDstV, g_bBufLine4, 800);

	DmaWaitFinish(h3);
	DmaWaitFinish(h4);
}

void BT1120UV_1920_1080_SplitUV(
	PBYTE8 pbSrc,
	PBYTE8 pbDstU,
	PBYTE8 pbDstV
)
{
	int i,j;

	PDWORD32 pdwUVIn = NULL;
	PWORD16 pwUOut = NULL;
	PWORD16 pwVOut = NULL;
	DWORD32 dwUVInTmp;

	memcpy(g_bBufLine1, pbSrc, 1920);
	pbSrc += 1920;

	for ( i = 1; i < 1080; ++i )
	{
		//in
		DmaCopy1D(h2, g_bBufLine2, pbSrc, 1920);
		pbSrc += 1920;

		//proc
		DmaWaitFinish(h3);
		DmaWaitFinish(h4);
		pdwUVIn = (PDWORD32)g_bBufLine1;
		pwUOut = (PWORD16)g_bBufLine3;
		pwVOut = (PWORD16)g_bBufLine4;

		for ( j = 0; j < 480; ++j )
		{
			dwUVInTmp = *(pdwUVIn++);

			*(pwUOut++) = ((dwUVInTmp&0xff) | ((dwUVInTmp>>8) & 0xff00));
			*(pwVOut++) = (((dwUVInTmp&0xff00) >> 8) | ((dwUVInTmp>>16) & 0xff00));
		}

		//out
		DmaCopy1D(h3, pbDstU, g_bBufLine3, 960);
		DmaCopy1D(h4, pbDstV, g_bBufLine4, 960);
		pbDstU += 960;
		pbDstV += 960;

		DmaWaitFinish(h2);
		memcpy(g_bBufLine1, g_bBufLine2, 1920);
	}

	// 处理最后一行

	//proc
	DmaWaitFinish(h3);
	DmaWaitFinish(h4);
	pdwUVIn = (PDWORD32)g_bBufLine1;
	pwUOut = (PWORD16)g_bBufLine3;
	pwVOut = (PWORD16)g_bBufLine4;

	for ( j = 0; j < 480; ++j )
	{
		dwUVInTmp = *(pdwUVIn++);

		*(pwUOut++) = ((dwUVInTmp&0xff) | ((dwUVInTmp>>8) & 0xff00));
		*(pwVOut++) = (((dwUVInTmp&0xff00) >> 8) | ((dwUVInTmp>>16) & 0xff00));
	}

	//out
	DmaCopy1D(h3, pbDstU, g_bBufLine3, 960);
	DmaCopy1D(h4, pbDstV, g_bBufLine4, 960);

	DmaWaitFinish(h3);
	DmaWaitFinish(h4);
}
