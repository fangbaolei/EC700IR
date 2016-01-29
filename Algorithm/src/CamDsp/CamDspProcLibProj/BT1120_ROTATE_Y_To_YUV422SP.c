#include "CamDspProc.h"

extern DMA_HANDLE h1;
extern DMA_HANDLE h2;
extern DMA_HANDLE h3;

void BT1120_ROTATE_Y_1200_1600_To_YUV422SP_1184_800(
	PBYTE8 pbBT1120Y,
	PBYTE8 pbBT1120UV,
	PBYTE8 pbYUV422SP_Y,
	PBYTE8 pbYUV422SP_UV
)
{
	PBYTE8 pbBT1120Y_Tmp = pbBT1120Y;
	PBYTE8 pbBT1120UV_Tmp = pbBT1120UV;
	PBYTE8 pbYUV422SP_Y_Tmp = pbYUV422SP_Y;
	PBYTE8 pbYUV422SP_UV_Tmp = pbYUV422SP_UV + 1184*799;

	int i = 0;

	for ( i = 0; i < 592; ++i )
	{
		DmaCopy1D(h1, pbYUV422SP_Y_Tmp, pbBT1120Y_Tmp, 1184);
		DmaCopy2D(h2, pbYUV422SP_UV_Tmp, -1184, pbBT1120UV_Tmp, 2, 1, 800);
		DmaCopy2D(h3, pbYUV422SP_UV_Tmp+1, -1184, pbBT1120UV_Tmp+1, 2, 1, 800);

		pbBT1120Y_Tmp += 1200;
		pbYUV422SP_Y_Tmp += 1184;
		pbBT1120UV_Tmp += 3200;
		pbYUV422SP_UV_Tmp += 2;

		DmaWaitFinish(h1);
		DmaWaitFinish(h2);
		DmaWaitFinish(h3);
	}
	for ( ; i < 800; ++i )
	{
		DmaCopy1D(h1, pbYUV422SP_Y_Tmp, pbBT1120Y_Tmp, 1184);
		pbBT1120Y_Tmp += 1200;
		pbYUV422SP_Y_Tmp += 1184;
		DmaWaitFinish(h1);
	}
}
