#include "CamDspProc.h"

extern DMA_HANDLE h1;
extern DMA_HANDLE h2;
extern DMA_HANDLE h3;
extern DMA_HANDLE h4;
extern DMA_HANDLE h5;
extern DMA_HANDLE h6;

static BYTE8 g_buffer[1200*1600];

void BT1120_ROTATE_Y_1200_1600_To_UYVY_1200_1600(
    const PBYTE8 pbSrcY,
    const PBYTE8 pbSrcUV,
    PBYTE8 pbDstUYVY
)
{
    int i = 0;

    PBYTE8 pbSrcY_Tmp = pbSrcY;

    PBYTE8 pbDstU = g_buffer;//pbDstUYVY + 1200*1600;
    PBYTE8 pbDstV = g_buffer + 600*1600;//pbDstU + 600*1600;

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

    pbDstU_Tmp = pbDstU + 600;
    pbDstV_Tmp = pbDstV + 600;

    for ( i=0; i<800; ++i )
    {
        DmaCopy2D(h1, pbDstUYVY+1, 2, pbSrcY_Tmp, 1, 1, 1200);
        DmaCopy2D(h2, pbDstUYVY+0, 4, pbDstU_Tmp, 1, 1, 600);
        DmaCopy2D(h3, pbDstUYVY+2, 4, pbDstV_Tmp, 1, 1, 600);
        DmaWaitFinish(h1);
        DmaWaitFinish(h2);
        DmaWaitFinish(h3);

        DmaCopy1D(h4, pbDstUYVY+2400, pbDstUYVY, 2400);
        pbDstUYVY += 4800;
        pbSrcY_Tmp += 1200;
        pbDstU_Tmp += 1200;
        pbDstV_Tmp += 1200;
        DmaWaitFinish(h4);
    }
}

void YUV422P_To_UYVY(
    const unsigned char* pbSrcY,
    const unsigned char* pbSrcU,
    const unsigned char* pbSrcV,
    unsigned char* pbDstUYVY,
    int iWidth, int iHeight
)
{
    int i = 0;
    int iCount = iWidth * iHeight * 2;
    for ( i = 0; i < iCount; i += 4 )
    {
        pbDstUYVY[0] = pbSrcU[0];
        pbDstUYVY[1] = pbSrcY[0];
        pbDstUYVY[2] = pbSrcV[0];
        pbDstUYVY[3] = pbSrcY[1];

        pbDstUYVY += 4;
        pbSrcY += 2;
        pbSrcU += 1;
        pbSrcV += 1;
    }
}
