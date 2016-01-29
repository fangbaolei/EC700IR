#include <csl_cache.h>
#include "swBaseType.h"


static int HV_dmasetup_dm6467()
{
	return 0;
}

static int HV_dmawait_dm6467(int iHandle)
{
	return 0;
}

static int HV_dmacpy1D_dm6467(unsigned char* pbDst, unsigned char* pbSrc, int iSize)
{
	memcpy(pbDst, pbSrc, iSize);
	return 1;
}


#define LINE_SIZE_YUV422 1600
#define IMAGE_HEIGHT 600

static unsigned char g_bUserOnChipRAM[64 * 1024];
#define g_bBufLine1 (g_bUserOnChipRAM)
#define g_bBufLine2 (g_bUserOnChipRAM+LINE_SIZE_YUV422)
#define g_bBufLine1_next (g_bUserOnChipRAM+LINE_SIZE_YUV422*2)
#define g_bBufLine2_next (g_bUserOnChipRAM+LINE_SIZE_YUV422*3)
#define g_bBufCbYCrYLine1 (g_bUserOnChipRAM+LINE_SIZE_YUV422*4)

static int h1 = -1;
static int h2 = -1;
static int h1_next = -1;
static int h2_next = -1;
static int h1_output = -1;
static int h2_output = -1;

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
	//CACHE_wbL2(g_bBufCbYCrYLine1, LINE_SIZE_YUV422*2, CACHE_WAIT);
	h1_output = HV_dmacpy1D_dm6467(bCbYCrY, g_bBufCbYCrYLine1, LINE_SIZE_YUV422*2);
	h2_output = HV_dmacpy1D_dm6467(bCbYCrY+LINE_SIZE_YUV422*2, g_bBufCbYCrYLine1, LINE_SIZE_YUV422*2);
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

void BT1120Field_1600_600_To_UYVY_1600_1200(
	const PBYTE8 pbImgSrcY, 
	const PBYTE8 pbImgSrcUV, 
	PBYTE8 pbCbYCrY
)
{
	int k = 1;  // pbCbYCrY的行偏移
	int i = 0;

	h1 = HV_dmacpy1D_dm6467(g_bBufLine1, pbImgSrcY, LINE_SIZE_YUV422);
	h2 = HV_dmacpy1D_dm6467(g_bBufLine2, pbImgSrcUV, LINE_SIZE_YUV422);

	h1_next = HV_dmacpy1D_dm6467(g_bBufLine1_next, pbImgSrcY+LINE_SIZE_YUV422, LINE_SIZE_YUV422);
	h2_next = HV_dmacpy1D_dm6467(g_bBufLine2_next, pbImgSrcUV+LINE_SIZE_YUV422, LINE_SIZE_YUV422);

	HV_dmawait_dm6467(h1);
	HV_dmawait_dm6467(h2);

	ProcessDoubleLine(
		(unsigned char *)g_bBufLine1, (unsigned char *)g_bBufLine2, 
		(unsigned char *)pbCbYCrY
	);

	for ( i=2; i<IMAGE_HEIGHT; i+=1, ++k )
	{
		HV_dmawait_dm6467(h1_next);
		HV_dmawait_dm6467(h2_next);
		memcpy(g_bBufLine1, g_bBufLine1_next, LINE_SIZE_YUV422);
		memcpy(g_bBufLine2, g_bBufLine2_next, LINE_SIZE_YUV422);

		h1_next = HV_dmacpy1D_dm6467(g_bBufLine1_next, pbImgSrcY+LINE_SIZE_YUV422*(i), LINE_SIZE_YUV422);
		h2_next = HV_dmacpy1D_dm6467(g_bBufLine2_next, pbImgSrcUV+LINE_SIZE_YUV422*(i), LINE_SIZE_YUV422);

		HV_dmawait_dm6467(h1_output);
		HV_dmawait_dm6467(h2_output);
		//CACHE_invL2(pbCbYCrY + LINE_SIZE_YUV422*2*(k-1)*2, LINE_SIZE_YUV422*2*2, CACHE_WAIT);
		ProcessDoubleLine(
			(unsigned char *)g_bBufLine1, (unsigned char *)g_bBufLine2, 
			(unsigned char *)pbCbYCrY + LINE_SIZE_YUV422*2*(k)*2
		);
	}
	HV_dmawait_dm6467(h1_next);
	HV_dmawait_dm6467(h2_next);
	memcpy(g_bBufLine1, g_bBufLine1_next, LINE_SIZE_YUV422);
	memcpy(g_bBufLine2, g_bBufLine2_next, LINE_SIZE_YUV422);

	HV_dmawait_dm6467(h1_output);
	HV_dmawait_dm6467(h2_output);
	//CACHE_invL2(pbCbYCrY + LINE_SIZE_YUV422*2*(k-1)*2, LINE_SIZE_YUV422*2*2, CACHE_WAIT);
	ProcessDoubleLine(
		(unsigned char *)g_bBufLine1, (unsigned char *)g_bBufLine2, 
		(unsigned char *)pbCbYCrY + LINE_SIZE_YUV422*2*(k)*2
	);
	HV_dmawait_dm6467(h1_output);
	HV_dmawait_dm6467(h2_output);
	//CACHE_invL2(pbCbYCrY + LINE_SIZE_YUV422*2*(k)*2, LINE_SIZE_YUV422*2*2, CACHE_WAIT);
}
