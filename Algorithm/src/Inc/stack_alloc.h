#ifndef __STACK_ALLOC_H__
#define __STACK_ALLOC_H__

#include "swbasetype.h"
#include "swwinerror.h"
#include <string.h>

const int g_iMaxAllocCount = 15;
class CFastMemAlloc
{
public:
	CFastMemAlloc(): m_iAllocCount( 0 ) {}
	~CFastMemAlloc();
	void *StackAlloc( int iSize, BOOL fFastMem = TRUE );
	void Clear();
	HRESULT Detach(CFastMemAlloc& stackDest);
private:
	void *m_rgpAllocMem[ g_iMaxAllocCount ];
	int m_rgiAllocSize[ g_iMaxAllocCount ];
	int	m_rgiFastMem[ g_iMaxAllocCount ];
	int m_iAllocCount;
};

#endif		// #ifndef __STACK_ALLOC_H__

