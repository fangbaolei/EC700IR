#include <csl_cache.h>
#include "swBaseType.h"
#include "DmaCopyApi.h"

//说明：将宽高为1600*1200的CbYCrY格式图片逆时针旋转90度
void CbYCrY_1600_1200_Rotate90(
    const PBYTE8 pbSrc,
    PBYTE8 pbDst
)
{
    // 逆时针旋转90度核心算法示意
	//
    // Cb1 Y1 Cr1 Y2
    // Cb2 Y3 Cr2 Y4
    //--------------
    // Cb1 Y2 Cr1 Y4
    // Cb1 Y1 Cr1 Y3

    int i = 0;
	int h1 = -1;
	int h2 = -1;
	int h3 = -1;
	int h4 = -1;
	int h5 = -1;
	int h6 = -1;

    PBYTE8 pbSrcCur = pbSrc;
    PBYTE8 pbDstCur = pbDst+(1599*1200*2);  // 目标图起始位置

    memset(pbDst, 127, 1600*1200*2);
    CACHE_wbInvL2(pbSrc, 1600*1200*2, CACHE_WAIT);
    CACHE_wbInvL2(pbDst, 1600*1200*2, CACHE_WAIT);

    for ( i=0; i<600; ++i )  // 一次处理两行
    {
        h1 = HV_dmacpy2D_dm6467(pbDstCur+1, -2400, pbSrcCur+1, 2, 1, 1600);
        h2 = HV_dmacpy2D_dm6467(pbDstCur+3, -2400, pbSrcCur+1+3200, 2, 1, 1600);

        h3 = HV_dmacpy2D_dm6467(pbDstCur+0, -4800, pbSrcCur, 4, 1, 800);
        h4 = HV_dmacpy2D_dm6467(pbDstCur+0-2400, -4800, pbSrcCur, 4, 1, 800);

        h5 = HV_dmacpy2D_dm6467(pbDstCur+2, -4800, pbSrcCur+2, 4, 1, 800);
        h6 = HV_dmacpy2D_dm6467(pbDstCur+2-2400, -4800, pbSrcCur+2, 4, 1, 800);

        pbSrcCur += 6400;
        pbDstCur += 4;

        HV_dmawait_dm6467(h1);
        HV_dmawait_dm6467(h2);
        HV_dmawait_dm6467(h3);
        HV_dmawait_dm6467(h4);
        HV_dmawait_dm6467(h5);
        HV_dmawait_dm6467(h6);
    }
}
// BT1120 UV to SplitUV
void BT1120UV_To_SplitUV_ex(
	PBYTE8 pbSrcUV,
	int iWidth,
	int iHeight,
	PBYTE8 pbDstU,
	PBYTE8 pbDstV
)
{
	RESTRICT_PDWORD32 pdwSrcUV_Temp1 = (RESTRICT_PDWORD32)pbSrcUV;
	RESTRICT_PDWORD32 pdwSrcUV_Temp2 = (RESTRICT_PDWORD32)pbSrcUV + 1;

    RESTRICT_PDWORD32 pwDstUTemp1 = (RESTRICT_PDWORD32)pbDstU;
    RESTRICT_PDWORD32 pwDstVTemp1 = (RESTRICT_PDWORD32)pbDstV;

    DWORD32 dwUVTemp1;
    DWORD32 dwUVTemp2;
    DWORD32 dwUVTemp3;
    DWORD32 dwUVTemp4;

    DWORD32 wUTemp1;
    DWORD32 wUTemp2;
    DWORD32 wUTemp3;
    DWORD32 wUTemp4;

    DWORD32 wVTemp1;
    DWORD32 wVTemp2;
    DWORD32 wVTemp3;
    DWORD32 wVTemp4;

    int i = 0;
	int iWidthScale = iWidth / 8;//16;

    for ( i=0; i<iWidthScale*iHeight; ++i )
    {
        dwUVTemp1 = *( pdwSrcUV_Temp1  );
        wUTemp1 = ( dwUVTemp1 & 0xFF ) | ( ( dwUVTemp1 >> 8 ) & 0xFF00 );
        dwUVTemp2 = *( pdwSrcUV_Temp2  );
        wUTemp2 = ( dwUVTemp2 & 0xFF ) | ( ( dwUVTemp2 >> 8 ) & 0xFF00 );
        wUTemp1 = ( wUTemp1 & 0xFFFF ) | ( wUTemp2 << 16 );
        *( pwDstUTemp1 ++ ) = wUTemp1;

        wVTemp1 = ( ( dwUVTemp1 >> 8 ) & 0xFF ) | ( ( ( dwUVTemp1 >> 16 ) & 0xFF00 ) );
        wVTemp2 = ( ( dwUVTemp2 >> 8 ) & 0xFF ) | ( ( ( dwUVTemp2 >> 16 ) & 0xFF00 ) );
        wVTemp1 = ( wVTemp1 & 0xFFFF ) | ( wVTemp2 << 16 );
        *( pwDstVTemp1 ++ ) = wVTemp1;

		pdwSrcUV_Temp1 += 2;
		pdwSrcUV_Temp2 += 2;
    }
}

// BT1120 UV to SplitUV
void BT1120UV_To_SplitUV(
	PBYTE8 pbSrcUV,
	int iWidth,
	int iHeight,
	PBYTE8 pbDstU,
	PBYTE8 pbDstV
)
{
	RESTRICT_PDWORD32 pdwSrcUV_Temp1 = (RESTRICT_PDWORD32)pbSrcUV;
    RESTRICT_PDWORD32 pwDstUTemp1 = (RESTRICT_PDWORD32)pbDstU;
    RESTRICT_PDWORD32 pwDstVTemp1 = (RESTRICT_PDWORD32)pbDstV;

    DWORD32 dwUVTemp1;
    DWORD32 dwUVTemp2;
    DWORD32 dwUVTemp3;
    DWORD32 dwUVTemp4;

    DWORD32 wUTemp1;
    DWORD32 wUTemp2;
    DWORD32 wUTemp3;
    DWORD32 wUTemp4;

    DWORD32 wVTemp1;
    DWORD32 wVTemp2;
    DWORD32 wVTemp3;
    DWORD32 wVTemp4;

    int i = 0;
	int iWidthScale = iWidth / 16;

    for ( i=0; i<iWidthScale*iHeight; ++i )
    {
        dwUVTemp1 = *( pdwSrcUV_Temp1 ++ );
        wUTemp1 = ( dwUVTemp1 & 0xFF ) | ( ( dwUVTemp1 >> 8 ) & 0xFF00 );
        dwUVTemp2 = *( pdwSrcUV_Temp1 ++ );
        wUTemp2 = ( dwUVTemp2 & 0xFF ) | ( ( dwUVTemp2 >> 8 ) & 0xFF00 );
        wUTemp1 = ( wUTemp1 & 0xFFFF ) | ( wUTemp2 << 16 );
        *( pwDstUTemp1 ++ ) = wUTemp1;

        wVTemp1 = ( ( dwUVTemp1 >> 8 ) & 0xFF ) | ( ( ( dwUVTemp1 >> 16 ) & 0xFF00 ) );
        wVTemp2 = ( ( dwUVTemp2 >> 8 ) & 0xFF ) | ( ( ( dwUVTemp2 >> 16 ) & 0xFF00 ) );
        wVTemp1 = ( wVTemp1 & 0xFFFF ) | ( wVTemp2 << 16 );
        *( pwDstVTemp1 ++ ) = wVTemp1;

        dwUVTemp3 = *( pdwSrcUV_Temp1 ++ );
        wUTemp3 = ( dwUVTemp3 & 0xFF ) | ( ( dwUVTemp3 >> 8 ) & 0xFF00 );
        dwUVTemp4 = *( pdwSrcUV_Temp1 ++ );
        wUTemp4 = ( dwUVTemp4 & 0xFF ) | ( ( dwUVTemp4 >> 8 ) & 0xFF00 );
        wUTemp3 = ( wUTemp3 & 0xFFFF ) | ( wUTemp4 << 16 );
        *( pwDstUTemp1 ++ ) = wUTemp3;
        
        wVTemp3 = ( ( dwUVTemp3 >> 8 ) & 0xFF ) | ( ( ( dwUVTemp3 >> 16 ) & 0xFF00 ) );
        wVTemp4 = ( ( dwUVTemp4 >> 8 ) & 0xFF ) | ( ( ( dwUVTemp4 >> 16 ) & 0xFF00 ) );
        wVTemp3 = ( wVTemp3 & 0xFFFF ) | ( wVTemp4 << 16 );
        *( pwDstVTemp1 ++ ) = wVTemp3;
    }
}

// 说明：将交织的UV格式分离出单独的U和V
void BT1120UV_1600_1200_SplitUV(
    const PBYTE8 pbSrcBT1120UV,
    PBYTE8 pbDstU,
    PBYTE8 pbDstV
)
{
    RESTRICT_PDWORD32 pdwSrcUV_Temp1 = (RESTRICT_PDWORD32)pbSrcBT1120UV;
    RESTRICT_PDWORD32 pwDstUTemp1 = (RESTRICT_PDWORD32)pbDstU;
    RESTRICT_PDWORD32 pwDstVTemp1 = (RESTRICT_PDWORD32)pbDstV;

    DWORD32 dwUVTemp1;
    DWORD32 dwUVTemp2;
    DWORD32 dwUVTemp3;
    DWORD32 dwUVTemp4;

    DWORD32 wUTemp1;
    DWORD32 wUTemp2;
    DWORD32 wUTemp3;
    DWORD32 wUTemp4;

    DWORD32 wVTemp1;
    DWORD32 wVTemp2;
    DWORD32 wVTemp3;
    DWORD32 wVTemp4;

    int i = 0;

    for ( i=0; i<100*1200; ++i )
    {
        dwUVTemp1 = *( pdwSrcUV_Temp1 ++ );
        wUTemp1 = ( dwUVTemp1 & 0xFF ) | ( ( dwUVTemp1 >> 8 ) & 0xFF00 );
        dwUVTemp2 = *( pdwSrcUV_Temp1 ++ );
        wUTemp2 = ( dwUVTemp2 & 0xFF ) | ( ( dwUVTemp2 >> 8 ) & 0xFF00 );
        wUTemp1 = ( wUTemp1 & 0xFFFF ) | ( wUTemp2 << 16 );
        *( pwDstUTemp1 ++ ) = wUTemp1;

        wVTemp1 = ( ( dwUVTemp1 >> 8 ) & 0xFF ) | ( ( ( dwUVTemp1 >> 16 ) & 0xFF00 ) );
        wVTemp2 = ( ( dwUVTemp2 >> 8 ) & 0xFF ) | ( ( ( dwUVTemp2 >> 16 ) & 0xFF00 ) );
        wVTemp1 = ( wVTemp1 & 0xFFFF ) | ( wVTemp2 << 16 );
        *( pwDstVTemp1 ++ ) = wVTemp1;

        dwUVTemp3 = *( pdwSrcUV_Temp1 ++ );
        wUTemp3 = ( dwUVTemp3 & 0xFF ) | ( ( dwUVTemp3 >> 8 ) & 0xFF00 );
        dwUVTemp4 = *( pdwSrcUV_Temp1 ++ );
        wUTemp4 = ( dwUVTemp4 & 0xFF ) | ( ( dwUVTemp4 >> 8 ) & 0xFF00 );
        wUTemp3 = ( wUTemp3 & 0xFFFF ) | ( wUTemp4 << 16 );
        *( pwDstUTemp1 ++ ) = wUTemp3;
        
        wVTemp3 = ( ( dwUVTemp3 >> 8 ) & 0xFF ) | ( ( ( dwUVTemp3 >> 16 ) & 0xFF00 ) );
        wVTemp4 = ( ( dwUVTemp4 >> 8 ) & 0xFF ) | ( ( ( dwUVTemp4 >> 16 ) & 0xFF00 ) );
        wVTemp3 = ( wVTemp3 & 0xFFFF ) | ( wVTemp4 << 16 );
        *( pwDstVTemp1 ++ ) = wVTemp3;
    }
}

// 说明：将BT1120场格式的图片转换为YUV422格式，并返回分离的Y、U、V
void BT1120Field_To_YUV422P(
    RESTRICT_PBYTE8 pbSrcY,
    RESTRICT_PBYTE8 pbSrcUV,
	int iWidth,
	int iHeight,
    RESTRICT_PBYTE8 pbDstY,
    RESTRICT_PBYTE8 pbDstU,
    RESTRICT_PBYTE8 pbDstV
)
{
    int i,j;
	int h1 = -1;
	int h2 = -1;

    RESTRICT_PBYTE8 pbDstY_Temp = pbDstY + iWidth;

	RESTRICT_PBYTE8 pbDstU_Start = pbDstU;
    RESTRICT_PBYTE8 pbDstV_Start = pbDstV;

    RESTRICT_PDWORD32 pdwSrcUV_Temp1;

    RESTRICT_PWORD16 pwDstUTemp1;
    RESTRICT_PWORD16 pwDstVTemp1;

    DWORD32 dwUVTemp1;
    DWORD32 dwUVTemp2;
    DWORD32 dwUVTemp3;
    DWORD32 dwUVTemp4;

    WORD16 wUTemp1;
    WORD16 wUTemp2;
    WORD16 wUTemp3;
    WORD16 wUTemp4;

    WORD16 wVTemp1;
    WORD16 wVTemp2;
    WORD16 wVTemp3;
    WORD16 wVTemp4;

	int iWidthScale =  iWidth / 16;
	int iWidthUV = iWidth / 2;
	int iLineOther = iWidthUV % 128;
	int iLineSize = iWidthUV / 128 * 128;

    for (i = 0; i < iHeight; i++, pbSrcUV += iWidth, pbDstU += iWidth, pbDstV += iWidth)
    {
        pdwSrcUV_Temp1 = (RESTRICT_PDWORD32)pbSrcUV;
        pwDstUTemp1 = (RESTRICT_PWORD16)pbDstU;
        pwDstVTemp1 = (RESTRICT_PWORD16)pbDstV;

        h1 = HV_dmacpy1D_dm6467(pbDstY, pbSrcY, iWidth);
        h2 = HV_dmacpy1D_dm6467(pbDstY_Temp, pbSrcY, iWidth);

        pbSrcY += iWidth;
        pbDstY += (iWidth * 2);
        pbDstY_Temp += (iWidth * 2);

        for (j = 0; j < iWidthScale; j++)
        {
            //1
            dwUVTemp1 = *( pdwSrcUV_Temp1 ++ );
            wUTemp1 = ( dwUVTemp1 & 0xFF );
            wUTemp1 = wUTemp1 | ( ( ( dwUVTemp1 >> 16 ) & 0xFF ) << 8 );
            *( pwDstUTemp1 ++ ) = wUTemp1;
            wVTemp1 = ( ( dwUVTemp1 >> 8 ) & 0xFF );
            wVTemp1 = wVTemp1 | ( ( ( dwUVTemp1 >> 24 ) & 0xFF ) << 8 );
            *( pwDstVTemp1 ++ ) = wVTemp1;

            //2
            dwUVTemp2 = *( pdwSrcUV_Temp1 ++ );
            wUTemp2 = ( dwUVTemp2 & 0xFF );
            wUTemp2 = wUTemp2 | ( ( ( dwUVTemp2 >> 16 ) & 0xFF ) << 8 );
            *( pwDstUTemp1 ++ ) = wUTemp2;
            wVTemp2 = ( ( dwUVTemp2 >> 8 ) & 0xFF );
            wVTemp2 = wVTemp2 | ( ( ( dwUVTemp2 >> 24 ) & 0xFF ) << 8 );
            *( pwDstVTemp1 ++ ) = wVTemp2;

            //3
            dwUVTemp3 = *( pdwSrcUV_Temp1 ++ );
            wUTemp3 = ( dwUVTemp3 & 0xFF );
            wUTemp3 = wUTemp3 | ( ( ( dwUVTemp3 >> 16 ) & 0xFF ) << 8 );
            *( pwDstUTemp1 ++ ) = wUTemp3;
            wVTemp3 = ( ( dwUVTemp3 >> 8 ) & 0xFF );
            wVTemp3 = wVTemp3 | ( ( ( dwUVTemp3 >> 24 ) & 0xFF ) << 8 );
            *( pwDstVTemp1 ++ ) = wVTemp3;

            //4
            dwUVTemp4 = *( pdwSrcUV_Temp1 ++ );
            wUTemp4 = ( dwUVTemp4 & 0xFF );
            wUTemp4 = wUTemp4 | ( ( ( dwUVTemp4 >> 16 ) & 0xFF ) << 8 );
            *( pwDstUTemp1 ++ ) = wUTemp4;
            wVTemp4 = ( ( dwUVTemp4 >> 8 ) & 0xFF );
            wVTemp4 = wVTemp4 | ( ( ( dwUVTemp4 >> 24 ) & 0xFF ) << 8 );
            *( pwDstVTemp1 ++ ) = wVTemp4;
        }

        HV_dmawait_dm6467(h1);
        HV_dmawait_dm6467(h2);
    }

	for ( i = 0; i < iHeight; ++i )
	{
		h1 = HV_dmacpy1D_dm6467(pbDstU_Start+iWidthUV, pbDstU_Start, iLineSize);
		h2 = HV_dmacpy1D_dm6467(pbDstV_Start+iWidthUV, pbDstV_Start, iLineSize);
		if( iLineOther > 0 )
		{
			memcpy(pbDstU_Start+iWidthUV+iLineSize, pbDstU_Start+iLineSize, iLineOther);
			memcpy(pbDstV_Start+iWidthUV+iLineSize, pbDstV_Start+iLineSize, iLineOther);
		}

		pbDstU_Start += iWidth;
		pbDstV_Start += iWidth;

        HV_dmawait_dm6467(h1);
        HV_dmawait_dm6467(h2);
	}
}


// 说明：将BT1120场格式的图片转换为YUV422格式，并返回分离的Y、U、V
void BT1120Field_1600_600_To_YUV422P_1600_1200(
    RESTRICT_PBYTE8 pbSrcY,
    RESTRICT_PBYTE8 pbSrcUV,
    RESTRICT_PBYTE8 pbDstY,
    RESTRICT_PBYTE8 pbDstU,
    RESTRICT_PBYTE8 pbDstV
)
{
    int i,j;
	int h1 = -1;
	int h2 = -1;

    RESTRICT_PBYTE8 pbDstY_Temp = pbDstY + 1600;

	RESTRICT_PBYTE8 pbDstU_Start = pbDstU;
    RESTRICT_PBYTE8 pbDstV_Start = pbDstV;

    RESTRICT_PDWORD32 pdwSrcUV_Temp1;

    RESTRICT_PWORD16 pwDstUTemp1;
    RESTRICT_PWORD16 pwDstVTemp1;

    DWORD32 dwUVTemp1;
    DWORD32 dwUVTemp2;
    DWORD32 dwUVTemp3;
    DWORD32 dwUVTemp4;

    WORD16 wUTemp1;
    WORD16 wUTemp2;
    WORD16 wUTemp3;
    WORD16 wUTemp4;

    WORD16 wVTemp1;
    WORD16 wVTemp2;
    WORD16 wVTemp3;
    WORD16 wVTemp4;

    for (i = 0; i < 600; i++, pbSrcUV += 1600, pbDstU += 1600, pbDstV += 1600)
    {
        pdwSrcUV_Temp1 = (RESTRICT_PDWORD32)pbSrcUV;
        pwDstUTemp1 = (RESTRICT_PWORD16)pbDstU;
        pwDstVTemp1 = (RESTRICT_PWORD16)pbDstV;

        h1 = HV_dmacpy1D_dm6467(pbDstY, pbSrcY, 1600);
        h2 = HV_dmacpy1D_dm6467(pbDstY_Temp, pbSrcY, 1600);

        pbSrcY += 1600;
        pbDstY += 3200;
        pbDstY_Temp += 3200;

        for (j = 0; j < 100; j++)
        {
            //1
            dwUVTemp1 = *( pdwSrcUV_Temp1 ++ );
            wUTemp1 = ( dwUVTemp1 & 0xFF );
            wUTemp1 = wUTemp1 | ( ( ( dwUVTemp1 >> 16 ) & 0xFF ) << 8 );
            *( pwDstUTemp1 ++ ) = wUTemp1;
            wVTemp1 = ( ( dwUVTemp1 >> 8 ) & 0xFF );
            wVTemp1 = wVTemp1 | ( ( ( dwUVTemp1 >> 24 ) & 0xFF ) << 8 );
            *( pwDstVTemp1 ++ ) = wVTemp1;

            //2
            dwUVTemp2 = *( pdwSrcUV_Temp1 ++ );
            wUTemp2 = ( dwUVTemp2 & 0xFF );
            wUTemp2 = wUTemp2 | ( ( ( dwUVTemp2 >> 16 ) & 0xFF ) << 8 );
            *( pwDstUTemp1 ++ ) = wUTemp2;
            wVTemp2 = ( ( dwUVTemp2 >> 8 ) & 0xFF );
            wVTemp2 = wVTemp2 | ( ( ( dwUVTemp2 >> 24 ) & 0xFF ) << 8 );
            *( pwDstVTemp1 ++ ) = wVTemp2;

            //3
            dwUVTemp3 = *( pdwSrcUV_Temp1 ++ );
            wUTemp3 = ( dwUVTemp3 & 0xFF );
            wUTemp3 = wUTemp3 | ( ( ( dwUVTemp3 >> 16 ) & 0xFF ) << 8 );
            *( pwDstUTemp1 ++ ) = wUTemp3;
            wVTemp3 = ( ( dwUVTemp3 >> 8 ) & 0xFF );
            wVTemp3 = wVTemp3 | ( ( ( dwUVTemp3 >> 24 ) & 0xFF ) << 8 );
            *( pwDstVTemp1 ++ ) = wVTemp3;

            //4
            dwUVTemp4 = *( pdwSrcUV_Temp1 ++ );
            wUTemp4 = ( dwUVTemp4 & 0xFF );
            wUTemp4 = wUTemp4 | ( ( ( dwUVTemp4 >> 16 ) & 0xFF ) << 8 );
            *( pwDstUTemp1 ++ ) = wUTemp4;
            wVTemp4 = ( ( dwUVTemp4 >> 8 ) & 0xFF );
            wVTemp4 = wVTemp4 | ( ( ( dwUVTemp4 >> 24 ) & 0xFF ) << 8 );
            *( pwDstVTemp1 ++ ) = wVTemp4;
        }

        HV_dmawait_dm6467(h1);
        HV_dmawait_dm6467(h2);
    }

	for ( i = 0; i < 600; ++i )
	{
		h1 = HV_dmacpy1D_dm6467(pbDstU_Start+800, pbDstU_Start, 768);
		h2 = HV_dmacpy1D_dm6467(pbDstV_Start+800, pbDstV_Start, 768);
		memcpy(pbDstU_Start+800+768, pbDstU_Start+768, 32);
		memcpy(pbDstV_Start+800+768, pbDstV_Start+768, 32);

		pbDstU_Start += 1600;
		pbDstV_Start += 1600;

        HV_dmawait_dm6467(h1);
        HV_dmawait_dm6467(h2);
	}
}

// 说明：将BT1120旋转Y格式的图片转换为YUV422格式，并返回分离的Y、U、V
void BT1120_ROTATE_Y_1200_1600_To_YUV422P_1200_1600(
    const PBYTE8 pbSrcY,
    const PBYTE8 pbSrcUV,
    PBYTE8 pbDstY,
    PBYTE8 pbDstU,
    PBYTE8 pbDstV
)
{
	int i = 0;
	int h1 = -1;
	int h2 = -1;
	int h3 = -1;
	int h4 = -1;
	int h5 = -1;
	int h6 = -1;

	PBYTE8 pbSrcY_Tmp = pbSrcY;
	PBYTE8 pbDstY_Tmp = pbDstY;

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

	pbDstU_Tmp = pbDstU;
	pbDstV_Tmp = pbDstV;
	for ( i=0; i<800; ++i )
	{
		h2 = HV_dmacpy1D_dm6467(pbDstU_Tmp, pbDstU_Tmp-600, 600);
		h4 = HV_dmacpy1D_dm6467(pbDstV_Tmp, pbDstV_Tmp-600, 600);

		h5 = HV_dmacpy1D_dm6467(pbDstY_Tmp, pbSrcY_Tmp, 1200);
		h6 = HV_dmacpy1D_dm6467(pbDstY_Tmp+1200, pbSrcY_Tmp, 1200);

		pbDstU_Tmp += 1200;
		pbDstV_Tmp += 1200;

		pbSrcY_Tmp += 1200;
		pbDstY_Tmp += 2400;

		HV_dmawait_dm6467(h2);
		HV_dmawait_dm6467(h4);

		HV_dmawait_dm6467(h5);
		HV_dmawait_dm6467(h6);
	}
}

// ---------------------------------------------------------------------------------------

#define INTERNAL_DATA_MEM_SIZE (64*1024)
static unsigned char internalDataMemory[INTERNAL_DATA_MEM_SIZE];
#define g_bBufLine1 (internalDataMemory)
#define g_bBufLine2 (internalDataMemory+3200)
#define g_bBufLine3 (internalDataMemory+6400)
#define g_bBufLine4 (internalDataMemory+9600)

static void FieldToFrame(PBYTE8 pbSrc, PBYTE8 pbDst, int iSrcWidth, int iSrcHeight)
{
	int h1 = -1;
	int h2 = -1;
	h1 = HV_dmacpy2D_dm6467(pbDst, 2*iSrcWidth, pbSrc, 1*iSrcWidth, iSrcWidth, iSrcHeight);
	h2 = HV_dmacpy2D_dm6467(pbDst+iSrcWidth, 2*iSrcWidth, pbSrc, 1*iSrcWidth, iSrcWidth, iSrcHeight);
	HV_dmawait_dm6467(h1);
	HV_dmawait_dm6467(h2);
}

static void SplitUV(PBYTE8 pbSrc, PBYTE8 pbDstU, PBYTE8 pbDstV)
{
	int i,j;
	int h2 = -1;
	int h3 = -1;
	int h4 = -1;

	PDWORD32 pdwUVIn = NULL;
	PWORD16 pwUOut = NULL;
	PWORD16 pwVOut = NULL;
	DWORD32 dwUVInTmp;

	memcpy(g_bBufLine1, pbSrc, 2448);
	pbSrc += 2448;

	// 处理第一行

	//in
	CACHE_wbL2(pbSrc, 2448, CACHE_WAIT);
	h2 = HV_dmacpy1D_dm6467(g_bBufLine2, pbSrc, 2448);
	pbSrc += 2448;

	//proc
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
	CACHE_wbL2(g_bBufLine3, 1224, CACHE_WAIT);
	CACHE_wbL2(g_bBufLine4, 1224, CACHE_WAIT);
	h3 = HV_dmacpy1D_dm6467(pbDstU, g_bBufLine3, 1224);
	h4 = HV_dmacpy1D_dm6467(pbDstV, g_bBufLine4, 1224);
	pbDstU += 1224;
	pbDstV += 1224;

	HV_dmawait_dm6467(h2);
	CACHE_invL2(g_bBufLine2, 2448, CACHE_WAIT);
	memcpy(g_bBufLine1, g_bBufLine2, 2448);

	// 处理中间行

	for ( i = 2; i < 1024; ++i )
	{
		//in
		CACHE_wbL2(pbSrc, 2448, CACHE_WAIT);
		h2 = HV_dmacpy1D_dm6467(g_bBufLine2, pbSrc, 2448);
		pbSrc += 2448;

		//proc
		HV_dmawait_dm6467(h3);
		HV_dmawait_dm6467(h4);
		CACHE_invL2(pbDstU-1224, 1224, CACHE_WAIT);
		CACHE_invL2(pbDstV-1224, 1224, CACHE_WAIT);
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
		CACHE_wbL2(g_bBufLine3, 1224, CACHE_WAIT);
		CACHE_wbL2(g_bBufLine4, 1224, CACHE_WAIT);
		h3 = HV_dmacpy1D_dm6467(pbDstU, g_bBufLine3, 1224);
		h4 = HV_dmacpy1D_dm6467(pbDstV, g_bBufLine4, 1224);
		pbDstU += 1224;
		pbDstV += 1224;

		HV_dmawait_dm6467(h2);
		CACHE_invL2(g_bBufLine2, 2448, CACHE_WAIT);
		memcpy(g_bBufLine1, g_bBufLine2, 2448);
	}

	// 处理最后一行

	//proc
	HV_dmawait_dm6467(h3);
	HV_dmawait_dm6467(h4);
	CACHE_invL2(pbDstU-1224, 1224, CACHE_WAIT);
	CACHE_invL2(pbDstV-1224, 1224, CACHE_WAIT);
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
	CACHE_wbL2(g_bBufLine3, 1224, CACHE_WAIT);
	CACHE_wbL2(g_bBufLine4, 1224, CACHE_WAIT);
	h3 = HV_dmacpy1D_dm6467(pbDstU, g_bBufLine3, 1224);
	h4 = HV_dmacpy1D_dm6467(pbDstV, g_bBufLine4, 1224);

	HV_dmawait_dm6467(h3);
	HV_dmawait_dm6467(h4);
	CACHE_invL2(pbDstU, 1224, CACHE_WAIT);
	CACHE_invL2(pbDstV, 1224, CACHE_WAIT);
}

// 说明：将500W前端传来的BT1120场格式的图片转换为YUV420帧格式，并返回分离的Y、U、V
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
