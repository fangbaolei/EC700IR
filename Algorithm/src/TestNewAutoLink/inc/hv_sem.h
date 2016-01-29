#ifndef _HV_SEM_H_
#define _HV_SEM_H_

#include "std.h"
#include "sem.h"
#include "assert.h"
#include "hvtarget.h"

typedef struct _HV_SEM_ITEM {
	SEM_Handle m_hSem;
	struct _HV_SEM_ITEM *m_pNext;
} HV_SEM_ITEM;

const int SEM_MAX_COUNT = 20;
extern HV_SEM_ITEM *g_pSemStack;

void HV_InitSem();

inline HV_SEM_ITEM *HV_AllocSem()
{
	assert( g_pSemStack != NULL );
	DWORD32 dwRestore = CloseGlobalInterrupt();	
	HV_SEM_ITEM *pResult = g_pSemStack;
	g_pSemStack = g_pSemStack->m_pNext;
	RestoreGlobalInterrupt( dwRestore );
	return pResult;
}

inline void HV_FreeSem( HV_SEM_ITEM *pHvSemItem )
{
	assert( pHvSemItem != NULL );
	DWORD32 dwRestore = CloseGlobalInterrupt();	
	pHvSemItem->m_pNext = g_pSemStack;
	g_pSemStack = pHvSemItem;
	RestoreGlobalInterrupt( dwRestore );
}

#endif
