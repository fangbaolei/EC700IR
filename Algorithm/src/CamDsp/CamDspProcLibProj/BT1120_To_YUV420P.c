#include "CamDspProc.h"

extern DMA_HANDLE h1;
extern DMA_HANDLE h2;
extern DMA_HANDLE h3;
extern DMA_HANDLE h4;

extern unsigned char g_bUserOnChipRAM[];
#define g_bBufLine1 (g_bUserOnChipRAM)
#define g_bBufLine2 (g_bUserOnChipRAM+2448)
#define g_bBufLine3 (g_bUserOnChipRAM+4896)
#define g_bBufLine4 (g_bUserOnChipRAM+7344)

static void FieldToFrame(PBYTE8 pbSrc, PBYTE8 pbDst, int iSrcWidth, int iSrcHeight)
{
	DmaCopy2D(h1, pbDst, 2*iSrcWidth, pbSrc, 1*iSrcWidth, iSrcWidth, iSrcHeight);
	DmaCopy2D(h2, pbDst+iSrcWidth, 2*iSrcWidth, pbSrc, 1*iSrcWidth, iSrcWidth, iSrcHeight);
	DmaWaitFinish(h1);
	DmaWaitFinish(h2);
}

static void SplitUV(PBYTE8 pbSrc, PBYTE8 pbDstU, PBYTE8 pbDstV)
{
	int i,j;

	PDWORD32 pdwUVIn = NULL;
	PWORD16 pwUOut = NULL;
	PWORD16 pwVOut = NULL;
	DWORD32 dwUVInTmp;

	memcpy(g_bBufLine1, pbSrc, 2448);
	pbSrc += 2448;

	for ( i = 1; i < 1024; ++i )
	{
		//in
		DmaCopy1D(h2, g_bBufLine2, pbSrc, 2448);
		pbSrc += 2448;

		//proc
		DmaWaitFinish(h3);
		DmaWaitFinish(h4);
		pdwUVIn = (PDWORD32)g_bBufLine1;
		pwUOut = (PWORD16)g_bBufLine3;
		pwVOut = (PWORD16)g_bBufLine4;

		for ( j = 0; j < 612; ++j )
		{
			dwUVInTmp = *(pdwUVIn++);

			*(pwUOut++) = ((dwUVInTmp&0xff) | ((dwUVInTmp>>8) & 0xff00));
			*(pwVOut++) = (((dwUVInTmp&0xff00) >> 8) | ((dwUVInTmp>>16) & 0xff00));
		}

		//out
		DmaCopy1D(h3, pbDstU, g_bBufLine3, 1224);
		DmaCopy1D(h4, pbDstV, g_bBufLine4, 1224);
		pbDstU += 1224;
		pbDstV += 1224;

		DmaWaitFinish(h2);
		memcpy(g_bBufLine1, g_bBufLine2, 2448);
	}

	// 处理最后一行

	//proc
	DmaWaitFinish(h3);
	DmaWaitFinish(h4);
	pdwUVIn = (PDWORD32)g_bBufLine1;
	pwUOut = (PWORD16)g_bBufLine3;
	pwVOut = (PWORD16)g_bBufLine4;

	for ( j = 0; j < 612; ++j )
	{
		dwUVInTmp = *(pdwUVIn++);

		*(pwUOut++) = ((dwUVInTmp&0xff) | ((dwUVInTmp>>8) & 0xff00));
		*(pwVOut++) = (((dwUVInTmp&0xff00) >> 8) | ((dwUVInTmp>>16) & 0xff00));
	}

	//out
	DmaCopy1D(h3, pbDstU, g_bBufLine3, 1224);
	DmaCopy1D(h4, pbDstV, g_bBufLine4, 1224);

	DmaWaitFinish(h3);
	DmaWaitFinish(h4);
}

// jpeg
void BT1120_2448_1024_To_YUV420P_2448_2048(
	PBYTE8 pbSrcY,
	PBYTE8 pbSrcUV,
	PBYTE8 pbDstY,
	PBYTE8 pbDstU,
	PBYTE8 pbDstV
)
{
	FieldToFrame(pbSrcY, pbDstY, 2448, 1024);
	SplitUV(pbSrcUV, pbDstU, pbDstV);
}
