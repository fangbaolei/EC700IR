#include <csl_cache.h>
#include "swBaseType.h"

static int HV_dmawait_dm6467(int iHandle)
{
    return 0;
}

static int HV_dmacpy1D_dm6467(unsigned char* pbDst, unsigned char* pbSrc, int iSize)
{
    memcpy(pbDst, pbSrc, iSize);
    return 1;
}

static int h1;
static int h2;
static int h3;
static int h4;

static unsigned char g_bUserOnChipRAM[64 * 1024];
#define g_bBufLine1 (g_bUserOnChipRAM)
#define g_bBufLine2 (g_bUserOnChipRAM+1920)
#define g_bBufLine3 (g_bUserOnChipRAM+1920+1920)
#define g_bBufLine4 (g_bUserOnChipRAM+1920+1920+1920)

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
        h2 = HV_dmacpy1D_dm6467(g_bBufLine2, pbSrc, 1920);
        pbSrc += 1920;

        //proc
        HV_dmawait_dm6467(h3);
        HV_dmawait_dm6467(h4);
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
        h3 = HV_dmacpy1D_dm6467(pbDstU, g_bBufLine3, 960);
        h4 = HV_dmacpy1D_dm6467(pbDstV, g_bBufLine4, 960);
        pbDstU += 960;
        pbDstV += 960;

        HV_dmawait_dm6467(h2);
        memcpy(g_bBufLine1, g_bBufLine2, 1920);
    }

    // 处理最后一行

    //proc
    HV_dmawait_dm6467(h3);
    HV_dmawait_dm6467(h4);
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
    h3 = HV_dmacpy1D_dm6467(pbDstU, g_bBufLine3, 960);
    h4 = HV_dmacpy1D_dm6467(pbDstV, g_bBufLine4, 960);

    HV_dmawait_dm6467(h3);
    HV_dmawait_dm6467(h4);
}
