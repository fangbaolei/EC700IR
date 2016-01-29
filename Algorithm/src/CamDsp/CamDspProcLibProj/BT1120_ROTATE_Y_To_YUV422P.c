#include "CamDspProc.h"

extern DMA_HANDLE h1;
extern DMA_HANDLE h2;
extern DMA_HANDLE h3;
extern DMA_HANDLE h4;
extern DMA_HANDLE h5;
extern DMA_HANDLE h6;

void BT1120_ROTATE_Y_1200_1600_To_YUV422P_1200_1600(
    const PBYTE8 pbSrcY,
    const PBYTE8 pbSrcUV,
    PBYTE8 pbDstY,
    PBYTE8 pbDstU,
    PBYTE8 pbDstV
)
{
	int i = 0;

	PBYTE8 pbSrcY_Tmp = pbSrcY;
	PBYTE8 pbDstY_Tmp = pbDstY;

	PBYTE8 pbSrcUV_Tmp = pbSrcUV;
	PBYTE8 pbDstU_Tmp = pbDstU + (600 * 1599);
	PBYTE8 pbDstV_Tmp = pbDstV + (600 * 1599);

	for ( i=0; i<600; ++i )
	{
		DmaCopy2D(h1, pbDstU_Tmp, -1200, pbSrcUV_Tmp, 2, 1, 800);
		DmaCopy2D(h3, pbDstV_Tmp, -1200, pbSrcUV_Tmp+1, 2, 1, 800);

		pbSrcUV_Tmp += 3200;
		pbDstU_Tmp += 1;
		pbDstV_Tmp += 1;

		DmaWaitFinish(h1);
		DmaWaitFinish(h3);
	}

	pbDstU_Tmp = pbDstU;
	pbDstV_Tmp = pbDstV;
	for ( i=0; i<800; ++i )
	{
		DmaCopy1D(h2, pbDstU_Tmp, pbDstU_Tmp-600, 600);
		DmaCopy1D(h4, pbDstV_Tmp, pbDstV_Tmp-600, 600);

		DmaCopy1D(h5, pbDstY_Tmp, pbSrcY_Tmp, 1200);
		DmaCopy1D(h6, pbDstY_Tmp+1200, pbSrcY_Tmp, 1200);

		pbDstU_Tmp += 1200;
		pbDstV_Tmp += 1200;

		pbSrcY_Tmp += 1200;
		pbDstY_Tmp += 2400;

		DmaWaitFinish(h2);
		DmaWaitFinish(h4);

		DmaWaitFinish(h5);
		DmaWaitFinish(h6);
	}
}
