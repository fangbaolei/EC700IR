#include "DmaCopyApi.h"

#include <std.h>
#include <stdlib.h>
#include <idma3.h>
#include <acpy3.h>
#include <dman3.h>

/*
#define DMA_heapInternal_Label (0x10006467)
#define DMA_heapExternal_Label (0x20006467)
*/

extern int dmaHeap;
extern int extHeap;

typedef struct _DmaHandleContext
{
	IDMA3_Handle handle;
	int groupId;
} DmaHandleContext;

static DmaHandleContext g_cDmaHandleContext[DMAN3_MAXGROUPS];
static int g_iDmaHandleContextIndex = 0;

void DmaInit()
{
	DMAN3_init();
    ACPY3_init();
}

void DmaExit()
{
	DMAN3_exit();
    ACPY3_exit();
}

DMA_HANDLE DmaOpen()
{
	DmaHandleContext* pcDmaContext = NULL;
	IDMA3_ChannelRec dmaTab;
	Int status;

	DMAN3_PARAMS.heapInternal = dmaHeap; //DMA_heapInternal_Label;
	DMAN3_PARAMS.heapExternal = extHeap; //DMA_heapExternal_Label;

	if ( g_iDmaHandleContextIndex >= DMAN3_MAXGROUPS )
	{
		return NULL;
	}
	else
	{
		pcDmaContext = &g_cDmaHandleContext[g_iDmaHandleContextIndex];
		pcDmaContext->groupId = g_iDmaHandleContextIndex;
		g_iDmaHandleContextIndex++;
	}

	dmaTab.numTransfers = 1;
	dmaTab.numWaits = 1;
	dmaTab.priority = IDMA3_PRIORITY_HIGH;
	dmaTab.protocol = &ACPY3_PROTOCOL;
	dmaTab.persistent = TRUE;

	status = DMAN3_createChannels(pcDmaContext->groupId, &dmaTab, 1);
	if (status == DMAN3_SOK )
	{
		pcDmaContext->handle = dmaTab.handle;
		ACPY3_activate(pcDmaContext->handle);
		return (DMA_HANDLE)pcDmaContext;
	}
	return NULL;
}

int DmaCopy1D(DMA_HANDLE hDma, unsigned char* pbDst, unsigned char* pbSrc, int iSize)
{
	IDMA3_Handle h = ((DmaHandleContext*)hDma)->handle;

	ACPY3_Params p;
	p.transferType = ACPY3_1D1D;
	p.dstAddr = (void *)pbDst;
	p.srcAddr = (void *)pbSrc;
	p.elementSize = iSize;
	p.numElements = 1;
	p.numFrames = 1;
	p.waitId = 0;

	ACPY3_configure(h, &p, 0);
	ACPY3_start(h);
	return 0;
}

int DmaCopy2D(
	DMA_HANDLE hDma, 
	unsigned char* pbDst, 
	int iDstStride, 
	unsigned char* pbSrc, 
	int iSrcStride, 
	int iElementSize, 
	int iElementsNum
)
{
	IDMA3_Handle h = ((DmaHandleContext*)hDma)->handle;

	ACPY3_Params p;
	p.transferType = ACPY3_2D2D;
	p.dstAddr = (void *)pbDst;
	p.srcAddr = (void *)pbSrc;
	p.elementSize = iElementSize;
	p.numElements = iElementsNum;
	p.numFrames = 1;
	p.srcElementIndex = iSrcStride;
	p.dstElementIndex = iDstStride;
	p.srcFrameIndex = 1;
	p.dstFrameIndex = 1;
	p.waitId = 0;

	ACPY3_configure(h, &p, 0);
	ACPY3_start(h);
	return 0;
}

int DmaWaitFinish(DMA_HANDLE hDma)
{
	IDMA3_Handle h = ((DmaHandleContext*)hDma)->handle;

	ACPY3_wait(h);
	return 0;
}

int DmaClose(DMA_HANDLE hDma)
{
	IDMA3_Handle h = ((DmaHandleContext*)hDma)->handle;

	if ( g_iDmaHandleContextIndex <= 0 )
	{
		return -1;
	}
	else
	{
		g_iDmaHandleContextIndex--;
	}

	ACPY3_wait(h);
	ACPY3_deactivate(h);
	if ( DMAN3_SOK == DMAN3_freeChannels(&h, 1) )
	{
		return 0;
	}
	return -1;
}

//-----------------兼容以前标清和高清的DMA封装模式-----------------------------

//#define USE_MEMCPY

#define DMAN3_MAXGROUPS_SW 10

typedef struct _DmaHandleContextEx
{
	DMA_HANDLE g_DmaHandle;
	int iUsed;
} DmaHandleContextEx;

#ifndef USE_MEMCPY
static DmaHandleContextEx g_DmaHandle[DMAN3_MAXGROUPS_SW];
static int g_iCurrentHandle = 0;
#endif

int HV_dmasetup_dm6467()
{
#ifndef USE_MEMCPY
	DMA_HANDLE h = NULL;
	int i = 0;
	DmaInit();
	while ( 1 )
	{
		h = DmaOpen();
		if ( NULL != h )
		{
			g_DmaHandle[i].g_DmaHandle = h;
			g_DmaHandle[i].iUsed = 0;
			i++;
		}
		if ( i >= DMAN3_MAXGROUPS_SW )
		{
			break;
		}
	}
#endif
	return 0;
}

int HV_dmawait_dm6467(int iHandle)
{
#ifndef USE_MEMCPY
	if ( 1 == g_DmaHandle[iHandle].iUsed )
	{
		DmaWaitFinish(g_DmaHandle[iHandle].g_DmaHandle);
		g_DmaHandle[iHandle].iUsed = 0;
	}
#endif
	return 0;
}

int HV_dmacpy1D_dm6467(unsigned char* pbDst, unsigned char* pbSrc, int iSize)
{
#ifndef USE_MEMCPY
	int iHandle = g_iCurrentHandle;
	++g_iCurrentHandle;
	if ( DMAN3_MAXGROUPS_SW == g_iCurrentHandle )
	{
		g_iCurrentHandle = 0;
	}

	if ( 1 == g_DmaHandle[iHandle].iUsed )
	{
		HV_dmawait_dm6467(iHandle);
	}

	g_DmaHandle[iHandle].iUsed = 1;
	DmaCopy1D(g_DmaHandle[iHandle].g_DmaHandle, pbDst, pbSrc, iSize );
	return iHandle;
#else
	memcpy(pbDst, pbSrc, iSize);
	return 1;
#endif
}

int HV_dmacpy2D_dm6467(unsigned char* pbDst, int iDstStride, unsigned char* pbSrc, int iSrcStride, int iWidth, int iHeight)
{
#ifndef USE_MEMCPY
	int iHandle = g_iCurrentHandle;
	++g_iCurrentHandle;
	if ( DMAN3_MAXGROUPS_SW == g_iCurrentHandle )
	{
		g_iCurrentHandle = 0;
	}

	if ( 1 == g_DmaHandle[iHandle].iUsed )
	{
		HV_dmawait_dm6467(iHandle);
	}

	g_DmaHandle[iHandle].iUsed = 1;
	DmaCopy2D(g_DmaHandle[iHandle].g_DmaHandle, pbDst, iDstStride, pbSrc, iSrcStride, iWidth, iHeight);
	return iHandle;
#else
	memcpy(pbDst, pbSrc, iWidth);
	return 1;
#endif
}

//------------------------------------------------------------------------------------

// Comment by Shaorg: 以下代码是在不使用DSP/BIOS的情况下要使用的。

/* Trying to have an internal heap without BIOS support. */
/* Assumption is that, freeing and allocation in the internal memory wouldn't
 * be discontinuous. That is the allocation and freeing will happen in the same
 * orders. */

/*
#define DMA_INTERNAL_DATA_MEM_SIZE (3 * 1024)
#pragma DATA_SECTION(internalDmaDataMemory, ".intDmaDataMem");
unsigned char internalDmaDataMemory[DMA_INTERNAL_DATA_MEM_SIZE];
static int internalDmaDataMemorySizeUsed = 0;

int GetUsedDmaIntMemSize()
{
	return internalDmaDataMemorySizeUsed;
}

Void * DmaMemMalloc(Int segid, Uns alignSize)
{
	int internalDmaDataMemorySizeUsedOld = 0;
	internalDmaDataMemorySizeUsedOld = internalDmaDataMemorySizeUsed;

	if ( DMA_heapInternal_Label == segid )
	{
		internalDmaDataMemorySizeUsed += alignSize;
		if ( internalDmaDataMemorySizeUsed <= DMA_INTERNAL_DATA_MEM_SIZE )
		{
			return (Void*)((int)internalDmaDataMemory + internalDmaDataMemorySizeUsedOld);
		}
		else
		{
			internalDmaDataMemorySizeUsed -= alignSize;
			return NULL;
		}
	}
	else
	{
		return malloc(alignSize);
	}
}

void DmaMemFree(Int segid, Void *buf, size_t size)
{
	if ( DMA_heapInternal_Label == segid )
	{
		internalDmaDataMemorySizeUsed = 0;
	}
	else
	{
		free(buf);
	}
}

#define PTRSIZE sizeof(Void *)

Void * MEM_valloc(Int segid, size_t size, size_t align, Char val)
{
    Uns     alignSize;
    Uns     alignBytes;
    Uns     remainBytes;
    Void   *buf;
    Void   *alignBuf;
    Uns     tmpAlign;
    LgUns  *addrPtr;

    alignBytes = (align < PTRSIZE) ? PTRSIZE : align;

    alignSize = size;
    if ((remainBytes = alignSize & (PTRSIZE - 1)) != 0)
    {
        alignSize += PTRSIZE - remainBytes;
        remainBytes = PTRSIZE - remainBytes;
    }

    alignSize += PTRSIZE;

    alignSize += alignBytes;
    buf = DmaMemMalloc(segid, alignSize);

    tmpAlign = (Uns)buf & (alignBytes - 1);
    if (tmpAlign)
    {
        alignBuf = (Void *)((Uns)buf + alignBytes - tmpAlign);
    }
    else
    {
        alignBuf = buf;
    }

    addrPtr = (LgUns *)((LgUns)alignBuf + size + remainBytes);
    *addrPtr = (LgUns)buf;

    memset(alignBuf, val, size);

    return (alignBuf);
}

Bool MEM_free(Int segid, Void * alignBuf, size_t size)
{
    LgUns *addrPtr;
    Void *buf;
    Uns remainBytes = 0;

    if ((PTRSIZE - 1) & size)
    {
        remainBytes = PTRSIZE - ((PTRSIZE - 1) & size);
    }

    addrPtr = (LgUns *)((LgUns)alignBuf + size + remainBytes);
    buf = (Void *)*addrPtr;

    DmaMemFree(segid, buf, size);

    return (TRUE);
}

Void TSK_enable(Void)
{
}

Void TSK_disable(Void)
{
}

void SYS_abort(char *s, ...)
{
	exit(1);
}
*/

//------------------------DMA测试函数----------------------------------
/*
#include <csl_cache.h>

//#pragma DATA_SECTION(srcArr, ".BufSrcArr");
//#pragma DATA_SECTION(dstArr, ".BufDstArr");

#define TEST_TYPE 1

#define BUFSIZE 3200
unsigned char srcArr[BUFSIZE];
unsigned char dstArr[BUFSIZE];

DMA_HANDLE h1_test = NULL;
DMA_HANDLE h2_test = NULL;
DMA_HANDLE h3_test = NULL;
DMA_HANDLE h4_test = NULL;

int TestDmaCopy()
{
	Int i;
	Bool errorFlag = FALSE;

	for (i = 0; i< BUFSIZE; i++) {
		srcArr[i] = i;
		dstArr[i] = 0;
	}
	//CACHE_wbInvAllL2(CACHE_WAIT);
	//CACHE_wbL2(srcArr, BUFSIZE, CACHE_WAIT);

	if ( !h1_test || !h2_test || !h3_test || !h4_test )
	{
		h1_test = DmaOpen();
		h2_test = DmaOpen();
		h3_test = DmaOpen();
		h4_test = DmaOpen();

		if ( !h1_test || !h2_test || !h3_test || !h4_test ) return -1;
	}

	if ( 0 == TEST_TYPE )
	{
		DmaCopy1D(h1_test, dstArr, srcArr, 800);
		DmaWaitFinish(h1_test);
		DmaCopy1D(h2_test, dstArr+800, srcArr+800, 800);
		DmaWaitFinish(h2_test);
		DmaCopy1D(h3_test, dstArr+1600, srcArr+1600, 800);
		DmaWaitFinish(h3_test);
		DmaCopy1D(h4_test, dstArr+2400, srcArr+2400, 800);
		DmaWaitFinish(h4_test);
	}
	else if ( 1 == TEST_TYPE )
	{
		DmaCopy1D(h1_test, dstArr, srcArr, 800);
		DmaCopy1D(h2_test, dstArr+800, srcArr+800, 800);
		DmaCopy1D(h3_test, dstArr+1600, srcArr+1600, 800);
		DmaCopy1D(h4_test, dstArr+2400, srcArr+2400, 800);

		DmaWaitFinish(h1_test);
		DmaWaitFinish(h2_test);
		DmaWaitFinish(h3_test);
		DmaWaitFinish(h4_test);
	}
	else if ( 2 == TEST_TYPE )
	{
		DmaCopy1D(h1_test, dstArr, srcArr, 800);
		DmaWaitFinish(h1_test);

		DmaCopy1D(h1_test, dstArr+800, srcArr+800, 800);
		DmaWaitFinish(h1_test);

		DmaCopy1D(h1_test, dstArr+1600, srcArr+1600, 800);
		DmaWaitFinish(h1_test);

		DmaCopy1D(h1_test, dstArr+2400, srcArr+2400, 800);
		DmaWaitFinish(h1_test);
	}

	//if ( -1 == DmaClose(h1_test) ) return -1;
	//if ( -1 == DmaClose(h2_test) ) return -1;
	//if ( -1 == DmaClose(h3_test) ) return -1;
	//if ( -1 == DmaClose(h4_test) ) return -1;

	//CACHE_invL2(dstArr, BUFSIZE, CACHE_WAIT);
	for (i = 0; i < BUFSIZE; i++)
	{
		if (dstArr[i] != srcArr[i]) {
			errorFlag = TRUE;
			break;
		}
	}
	if (errorFlag == FALSE) {
		return 0;
	}
	else {
		return -1;
	}
}
*/
