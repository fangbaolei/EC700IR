#include <tistdtypes.h>
#include <std.h>
#include <sys.h>
#include <Csl_intc.h>
#include <csl_cache.h>
#include <DSPLink.h>
#include "DspCmdApi.h"

#include "swBaseType.h"
#include "DmaCopyApi.h"

#pragma DATA_ALIGN( g_bOutBuffer, 128 )
BYTE8 g_bOutBuffer[1024*1024*4] = {0};

int g_iResetFlag;

void init_cache();

extern void MessageProc(
	unsigned char *inbuf,
	unsigned int inlen,
	unsigned char *outbuf,
	unsigned int outlen
);


void main()
{
	CSL_IntcGlobalEnableState state;

	CSL_sysInit();
	
	CSL_intcGlobalEnable(&state);
	CSL_intcInterruptEnable(5);

	init_cache(); //init cache

	*((volatile unsigned int *)DSPINTCLR)|=0xF; // clear all DSP INT

	HV_dmasetup_dm6467();

	DSPLinkSetup(MessageProc);
}

void InitEMIF( void )
{
	/*------------------------------------*/
    /* Cache Enable External Memory Space */
    /*------------------------------------*/

    /*------------------------------------*/
    /* Cache 0x80000000 --- 0x80FFFFFF    */
    /*------------------------------------*/
//    CACHE_enableCaching(CACHE_EMIFA_CE00);

    /*------------------------------------*/
    /* Cache 0x81000000 --- 0x81FFFFFF    */
    /*------------------------------------*/
//    CACHE_enableCaching(CACHE_EMIFA_CE01);

    /*------------------------------------*/
    /* Cache 0x82000000 --- 0x82FFFFFF    */
    /*------------------------------------*/
//    CACHE_enableCaching(CACHE_EMIFA_CE02);

    /*------------------------------------*/
    /* Cache 0x83000000 --- 0x83FFFFFF    */
    /*------------------------------------*/
//    CACHE_enableCaching(CACHE_EMIFA_CE03);

	// 从共享内存起始地址开始enable
    /*------------------------------------*/
    /* Cache 0x84000000 --- 0x84FFFFFF    */
    /*------------------------------------*/
    CACHE_enableCaching(CACHE_EMIFA_CE04);

    /*------------------------------------*/
    /* Cache 0x85000000 --- 0x85FFFFFF    */
    /*------------------------------------*/
    CACHE_enableCaching(CACHE_EMIFA_CE05);

    /*------------------------------------*/
    /* Cache 0x86000000 --- 0x86FFFFFF    */
    /*------------------------------------*/
    CACHE_enableCaching(CACHE_EMIFA_CE06);

    /*------------------------------------*/
    /* Cache 0x87000000 --- 0x87FFFFFF    */
    /*------------------------------------*/
    CACHE_enableCaching(CACHE_EMIFA_CE07);

    /*------------------------------------*/
    /* Cache 0x88000000 --- 0x88FFFFFF    */
    /*------------------------------------*/
    CACHE_enableCaching(CACHE_EMIFA_CE08);

    /*------------------------------------*/
    /* Cache 0x89000000 --- 0x89FFFFFF    */
    /*------------------------------------*/
    CACHE_enableCaching(CACHE_EMIFA_CE09);

    /*------------------------------------*/
    /* Cache 0x8A000000 --- 0x8AFFFFFF    */
    /*------------------------------------*/
    CACHE_enableCaching(CACHE_EMIFA_CE010);

    /*------------------------------------*/
    /* Cache 0x8B000000 --- 0x8BFFFFFF    */
    /*------------------------------------*/
    CACHE_enableCaching(CACHE_EMIFA_CE011);


    /*------------------------------------*/
    /* Cache 0x8C000000 --- 0x8CFFFFFF    */
    /*------------------------------------*/
    CACHE_enableCaching(CACHE_EMIFA_CE012);

    /*------------------------------------*/
    /* Cache 0x8D000000 --- 0x8DFFFFFF    */
    /*------------------------------------*/
    CACHE_enableCaching(CACHE_EMIFA_CE013);

    /*------------------------------------*/
    /* Cache 0x8E000000 --- 0x8EFFFFFF    */
    /*------------------------------------*/
    CACHE_enableCaching(CACHE_EMIFA_CE014);

    /*------------------------------------*/
    /* Cache 0x8F000000 --- 0x8FFFFFFF    */
    /*------------------------------------*/
    CACHE_enableCaching(CACHE_EMIFA_CE015);
}

void init_cache(void)
{
    CACHE_wbInvAllL2(CACHE_WAIT);
    CACHE_setL2Size(CACHE_32KCACHE);
    CACHE_wbInvAllL2(CACHE_WAIT);    

	InitEMIF();

    CACHE_setL1dSize(CACHE_L1_32KCACHE);
    CACHE_setL1pSize(CACHE_L1_32KCACHE);
    CACHE_wbInvAllL2(CACHE_WAIT);
} /* DoCacheSettings */


	

