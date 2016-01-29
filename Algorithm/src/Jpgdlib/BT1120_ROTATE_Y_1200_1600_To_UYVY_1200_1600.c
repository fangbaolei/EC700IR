#include <csl_cache.h>
#include "swBaseType.h"
#include "DmaCopyApi.h"

static char g_buffer[1200*1600];


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

/*
static int HV_dmacpy2D_dm6467(unsigned char* pbDst, int iDstStride, unsigned char* pbSrc, int iSrcStride, int iWidth, int iHeight)
{
	int i,j;

	for ( i = 0; i < iHeight; ++i )
	{
		for ( j = 0; j < iWidth; ++j )
		{
			pbDst[j] = pbSrc[j];
		}

		pbDst += (iDstStride*iWidth);
		pbSrc += (iSrcStride*iWidth);
	}

	return 1;
}
*/

// 适用于iWidth等于1的情况。
static int HV_dmacpy2D_dm6467(unsigned char* pbDst, int iDstStride, unsigned char* pbSrc, int iSrcStride, int iWidth, int iHeight)
{
	int i;

	for ( i = 0; i < iHeight; ++i )
	{
		pbDst[0] = pbSrc[0];

		pbDst += iDstStride;
		pbSrc += iSrcStride;
	}

	return 1;
}


void BT1120_ROTATE_Y_1200_1600_To_UYVY_1200_1600(
    const PBYTE8 pbSrcY,
    const PBYTE8 pbSrcUV,
    PBYTE8 pbDstUYVY
)
{
    int i = 0;
    int h1 = -1;
    int h2 = -1;
    int h3 = -1;
    int h4 = -1;

    PBYTE8 pbSrcY_Tmp = pbSrcY;

    PBYTE8 pbDstU = g_buffer;//pbDstUYVY + 1200*1600;
    PBYTE8 pbDstV = g_buffer + 600*1600;//pbDstU + 600*1600;

    PBYTE8 pbSrcUV_Tmp = pbSrcUV;
    PBYTE8 pbDstU_Tmp = pbDstU + (600 * 1599);
    PBYTE8 pbDstV_Tmp = pbDstV + (600 * 1599);

    for ( i=0; i<600; ++i )
    {
        h1 = HV_dmacpy2D_dm6467(pbDstU_Tmp, -1200, pbSrcUV_Tmp, 2, 1, 800);
        h3 = HV_dmacpy2D_dm6467(pbDstV_Tmp, -1200, pbSrcUV_Tmp+1, 2, 1, 800);

        pbSrcUV_Tmp += 3200;
        pbDstU_Tmp += 1;
        pbDstV_Tmp += 1;

        HV_dmawait_dm6467(h1);
        HV_dmawait_dm6467(h3);
    }

    pbDstU_Tmp = pbDstU + 600;
    pbDstV_Tmp = pbDstV + 600;

    for ( i=0; i<800; ++i )
    {
        h1 = HV_dmacpy2D_dm6467(pbDstUYVY+1, 2, pbSrcY_Tmp, 1, 1, 1200);
        h2 = HV_dmacpy2D_dm6467(pbDstUYVY+0, 4, pbDstU_Tmp, 1, 1, 600);
        h3 = HV_dmacpy2D_dm6467(pbDstUYVY+2, 4, pbDstV_Tmp, 1, 1, 600);
        HV_dmawait_dm6467(h1);
        HV_dmawait_dm6467(h2);
        HV_dmawait_dm6467(h3);

        h4 = HV_dmacpy1D_dm6467(pbDstUYVY+2400, pbDstUYVY, 2400);
        pbDstUYVY += 4800;
        pbSrcY_Tmp += 1200;
        pbDstU_Tmp += 1200;
        pbDstV_Tmp += 1200;
        HV_dmawait_dm6467(h4);
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
