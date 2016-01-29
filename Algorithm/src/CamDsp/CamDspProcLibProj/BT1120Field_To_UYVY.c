#include "CamDspProc.h"

#define LINE_SIZE_YUV422 2448
#define IMAGE_HEIGHT 1024

extern DMA_HANDLE h1;
extern DMA_HANDLE h2;
extern DMA_HANDLE h1_next;
extern DMA_HANDLE h2_next;
extern DMA_HANDLE h1_output;
extern DMA_HANDLE h2_output;

extern unsigned char g_bUserOnChipRAM[];
#define g_bBufLine1 (g_bUserOnChipRAM)
#define g_bBufLine2 (g_bUserOnChipRAM+LINE_SIZE_YUV422)
#define g_bBufLine1_next (g_bUserOnChipRAM+LINE_SIZE_YUV422*2)
#define g_bBufLine2_next (g_bUserOnChipRAM+LINE_SIZE_YUV422*3)
#define g_bBufCbYCrYLine1 (g_bUserOnChipRAM+LINE_SIZE_YUV422*4)

static void ProcessDoubleLine(
	unsigned char* pData16_1, 
	unsigned char* pData16_2, 
	unsigned char* bCbYCrY
)
{
	int i;

	// 一次处理两个像素点
	for ( i=0; i<LINE_SIZE_YUV422; i+=2, pData16_1 += 2, pData16_2 += 2 )
	{
		// Y Y
		// U V
		//-----------
		// Cb Y Cr Y
		g_bBufCbYCrYLine1[0+i*2] = pData16_2[0];
		g_bBufCbYCrYLine1[1+i*2] = pData16_1[0];
		g_bBufCbYCrYLine1[2+i*2] = pData16_2[1];
		g_bBufCbYCrYLine1[3+i*2] = pData16_1[1];
	}

	// 输出
	CACHE_wbL2(g_bBufCbYCrYLine1, LINE_SIZE_YUV422*2, CACHE_WAIT);
	DmaCopy1D(h1_output, bCbYCrY, g_bBufCbYCrYLine1, LINE_SIZE_YUV422*2);
	DmaCopy1D(h2_output, bCbYCrY+LINE_SIZE_YUV422*2, g_bBufCbYCrYLine1, LINE_SIZE_YUV422*2);
}

//------------------------------------------
// 格式说明
// 该函数所需输入的YUV422的数据排列规则如下：
// Y Y Y Y
// ...
// U V U V
// ...
// 注：Y U V 均为8位数据。
//------------------------------------------

void BT1120Field_2448_1024_To_UYVY_2448_2048(
	const PBYTE8 pbImgSrcY, 
	const PBYTE8 pbImgSrcUV, 
	PBYTE8 pbCbYCrY
)
{
	int k = 1;  // pbCbYCrY的行偏移
	int i = 0;

	DmaCopy1D(h1, g_bBufLine1, pbImgSrcY, LINE_SIZE_YUV422);
	DmaCopy1D(h2, g_bBufLine2, pbImgSrcUV, LINE_SIZE_YUV422);

	DmaCopy1D(h1_next, g_bBufLine1_next, pbImgSrcY+LINE_SIZE_YUV422, LINE_SIZE_YUV422);
	DmaCopy1D(h2_next, g_bBufLine2_next, pbImgSrcUV+LINE_SIZE_YUV422, LINE_SIZE_YUV422);

	DmaWaitFinish(h1);
	DmaWaitFinish(h2);

	ProcessDoubleLine(
		(unsigned char *)g_bBufLine1, (unsigned char *)g_bBufLine2, 
		(unsigned char *)pbCbYCrY
	);

	for ( i=2; i<IMAGE_HEIGHT; i+=1, ++k )
	{
		DmaWaitFinish(h1_next);
		DmaWaitFinish(h2_next);
		memcpy(g_bBufLine1, g_bBufLine1_next, LINE_SIZE_YUV422);
		memcpy(g_bBufLine2, g_bBufLine2_next, LINE_SIZE_YUV422);

		DmaCopy1D(h1_next, g_bBufLine1_next, pbImgSrcY+LINE_SIZE_YUV422*(i), LINE_SIZE_YUV422);
		DmaCopy1D(h2_next, g_bBufLine2_next, pbImgSrcUV+LINE_SIZE_YUV422*(i), LINE_SIZE_YUV422);

		DmaWaitFinish(h1_output);
		DmaWaitFinish(h2_output);
		CACHE_invL2(pbCbYCrY + LINE_SIZE_YUV422*2*(k-1)*2, LINE_SIZE_YUV422*2*2, CACHE_WAIT);
		ProcessDoubleLine(
			(unsigned char *)g_bBufLine1, (unsigned char *)g_bBufLine2, 
			(unsigned char *)pbCbYCrY + LINE_SIZE_YUV422*2*(k)*2
		);
	}
	DmaWaitFinish(h1_next);
	DmaWaitFinish(h2_next);
	memcpy(g_bBufLine1, g_bBufLine1_next, LINE_SIZE_YUV422);
	memcpy(g_bBufLine2, g_bBufLine2_next, LINE_SIZE_YUV422);

	DmaWaitFinish(h1_output);
	DmaWaitFinish(h2_output);
	CACHE_invL2(pbCbYCrY + LINE_SIZE_YUV422*2*(k-1)*2, LINE_SIZE_YUV422*2*2, CACHE_WAIT);
	ProcessDoubleLine(
		(unsigned char *)g_bBufLine1, (unsigned char *)g_bBufLine2, 
		(unsigned char *)pbCbYCrY + LINE_SIZE_YUV422*2*(k)*2
	);
	DmaWaitFinish(h1_output);
	DmaWaitFinish(h2_output);
	CACHE_invL2(pbCbYCrY + LINE_SIZE_YUV422*2*(k)*2, LINE_SIZE_YUV422*2*2, CACHE_WAIT);
}
