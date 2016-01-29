#include <tistdtypes.h>
#include <std.h>
#include <sys.h>
#include <csl_intc.h>
#include <csl_cache.h>
#include "CamDsp.h"

#pragma DATA_ALIGN( g_bInBuffer, 128 )
BYTE8 g_bInBuffer[10*1024*1024] = {0}; // 10MB

#pragma DATA_ALIGN( g_bOutBuffer, 128 )
BYTE8 g_bOutBuffer[4*1024*1024] = {0};  // 4MB

#define USER_ONCHIP_RAM_SIZE (20*1024) // 20KB
#pragma DATA_SECTION(g_bUserOnChipRAM, ".userOnChipRAM");
unsigned char g_bUserOnChipRAM[USER_ONCHIP_RAM_SIZE];

extern void tskMessageDispatch(MsgDispatch dispatch);
extern void MessageProc(
	unsigned char *inbuf,
	unsigned int inlen,
	unsigned char *outbuf,
	unsigned int outlen
);
extern int DSPLinkWrite(Uint32 value);

void init_cache(void);
void InitEMIF(void);
void InitEMIF_32M(void);

DSPLinkObj linkObj;

DMA_HANDLE h1 = NULL;
DMA_HANDLE h2 = NULL;
DMA_HANDLE h3 = NULL;
DMA_HANDLE h4 = NULL;
DMA_HANDLE h5 = NULL;
DMA_HANDLE h6 = NULL;
DMA_HANDLE h1_next = NULL;
DMA_HANDLE h2_next = NULL;
DMA_HANDLE h1_output = NULL;
DMA_HANDLE h2_output = NULL;
DMA_HANDLE h3_output = NULL;
DMA_HANDLE h4_output = NULL;
DMA_HANDLE h5_output = NULL;
DMA_HANDLE h6_output = NULL;

static int iCameraDmaFlag = -1;

void HvCloseCameraDma()
{
	SAFE_CLOSE_DMA(h1);
	SAFE_CLOSE_DMA(h2);
	SAFE_CLOSE_DMA(h3);
	SAFE_CLOSE_DMA(h4);
	SAFE_CLOSE_DMA(h5);
	SAFE_CLOSE_DMA(h6);
	SAFE_CLOSE_DMA(h1_next);
	SAFE_CLOSE_DMA(h2_next);
	SAFE_CLOSE_DMA(h1_output);
	SAFE_CLOSE_DMA(h2_output);
	SAFE_CLOSE_DMA(h3_output);
	SAFE_CLOSE_DMA(h4_output);
	SAFE_CLOSE_DMA(h5_output);
	SAFE_CLOSE_DMA(h6_output);

	iCameraDmaFlag = -1;
}

void HvOpenCameraDma()
{
	if ( -1 == iCameraDmaFlag )
	{
		h1 = DmaOpen();
		h2 = DmaOpen();
		h3 = DmaOpen();
		h4 = DmaOpen();
		h5 = DmaOpen();
		h6 = DmaOpen();
		h1_next = DmaOpen();
		h2_next = DmaOpen();
		h1_output = DmaOpen();
		h2_output = DmaOpen();
		h3_output = DmaOpen();
		h4_output = DmaOpen();
		h5_output = DmaOpen();
		h6_output = DmaOpen();

		if ( NULL != h1
			&& NULL != h2
			&& NULL != h3
			&& NULL != h4
			&& NULL != h5
			&& NULL != h6
			&& NULL != h1_next
			&& NULL != h2_next
			&& NULL != h1_output 
			&& NULL != h2_output
			&& NULL != h3_output
			&& NULL != h4_output
			&& NULL != h5_output
			&& NULL != h6_output )
		{
			iCameraDmaFlag = 0;
		}
		else
		{
			HvCloseCameraDma();
		}
	}
}

void main()
{
	CSL_IntcGlobalEnableState state;

	CSL_sysInit();
	CSL_intcGlobalEnable(&state);

	*((volatile unsigned int *)DSPINTCLR)|=0xF; // clear all DSP INT
	init_cache();

	DmaInit();		// 初始化DMA库
	CLK_start();	// 启动时钟

	linkObj.armReg = (DSPLinkRegHandle)DSPLINK_MEM_ARM;
	linkObj.dspReg = (DSPLinkRegHandle)DSPLINK_MEM_DSP;
	linkObj.trigerInt = DSPINTARM;

	DSPLinkWrite(0);
	tskMessageDispatch(MessageProc);
}

#define MDMA_PRIARB_REG     (*(int *)0x0182020C)

void init_cache(void)
{
	CSL_sysInit();
	MDMA_PRIARB_REG = 0x00000000;
	CACHE_wbInvAllL2(CACHE_WAIT);
#ifdef _CAMERA_PIXEL_500W_
	InitEMIF_32M();
#else
	InitEMIF();
#endif
	CACHE_setL1dSize(CACHE_L1_32KCACHE);
	CACHE_setL1pSize(CACHE_L1_32KCACHE);
	CACHE_setL2Size(CACHE_32KCACHE);
	CACHE_wbInvAllL2(CACHE_WAIT);
}

// 128MB ~ 256MB
void InitEMIF( void )
{
    /*------------------------------------*/
    /* Cache Enable External Memory Space */
    /*------------------------------------*/


    /*------------------------------------*/
    /* Cache 0x80000000 --- 0x80FFFFFF    */
    /*------------------------------------*/
    //CACHE_enableCaching(CACHE_EMIFA_CE00);

    /*------------------------------------*/
    /* Cache 0x81000000 --- 0x81FFFFFF    */
    /*------------------------------------*/
    //CACHE_enableCaching(CACHE_EMIFA_CE01);

    /*------------------------------------*/
    /* Cache 0x82000000 --- 0x82FFFFFF    */
    /*------------------------------------*/
    //CACHE_enableCaching(CACHE_EMIFA_CE02);

    /*------------------------------------*/
    /* Cache 0x83000000 --- 0x83FFFFFF    */
    /*------------------------------------*/
    //CACHE_enableCaching(CACHE_EMIFA_CE03);


    /*------------------------------------*/
    /* Cache 0x84000000 --- 0x84FFFFFF    */
    /*------------------------------------*/
    //CACHE_enableCaching(CACHE_EMIFA_CE04);

    /*------------------------------------*/
    /* Cache 0x85000000 --- 0x85FFFFFF    */
    /*------------------------------------*/
    //CACHE_enableCaching(CACHE_EMIFA_CE05);

    /*------------------------------------*/
    /* Cache 0x86000000 --- 0x86FFFFFF    */
    /*------------------------------------*/
    //CACHE_enableCaching(CACHE_EMIFA_CE06);

    /*------------------------------------*/
    /* Cache 0x87000000 --- 0x87FFFFFF    */
    /*------------------------------------*/
    //CACHE_enableCaching(CACHE_EMIFA_CE07);


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

// 64MB ~ 256MB
void InitEMIF_32M( void )
{
	/*------------------------------------*/
    /* Cache Enable External Memory Space */
    /*------------------------------------*/


    /*------------------------------------*/
    /* Cache 0x80000000 --- 0x80FFFFFF    */
    /*------------------------------------*/
    //CACHE_enableCaching(CACHE_EMIFA_CE00);

    /*------------------------------------*/
    /* Cache 0x81000000 --- 0x81FFFFFF    */
    /*------------------------------------*/
    //CACHE_enableCaching(CACHE_EMIFA_CE01);

    /*------------------------------------*/
    /* Cache 0x82000000 --- 0x82FFFFFF    */
    /*------------------------------------*/
    //CACHE_enableCaching(CACHE_EMIFA_CE02);

    /*------------------------------------*/
    /* Cache 0x83000000 --- 0x83FFFFFF    */
    /*------------------------------------*/
    //CACHE_enableCaching(CACHE_EMIFA_CE03);


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
