#include "stack_alloc.h"
#include "hvutils.h"

CFastMemAlloc::~CFastMemAlloc()
{
	Clear();
}

void *CFastMemAlloc::StackAlloc( int iSize, BOOL fFastMem )
{
	void *pTemp = NULL;
	if ( m_iAllocCount < g_iMaxAllocCount && iSize > 0 )
	{
		iSize = ( ( iSize + 1023 ) >> 10 ) << 10;
		if ( fFastMem )
		{
			pTemp = HV_AllocFastMem( iSize );
		}
		if (pTemp == NULL)
		{
			pTemp = HV_AllocMem( iSize );
			fFastMem = FALSE;
		}
		m_rgiAllocSize[ m_iAllocCount ] = iSize;
		m_rgpAllocMem[ m_iAllocCount ] = pTemp;
		m_rgiFastMem[ m_iAllocCount ] = fFastMem;
		m_iAllocCount++;
	}
	return pTemp;
}

void CFastMemAlloc::Clear()
{
	for ( int i = 0; i < m_iAllocCount; i++ )
	{
		if ( m_rgpAllocMem[ i ] != NULL )
		{
			if ( m_rgiFastMem[i] )
			{
				HV_FreeFastMem( m_rgpAllocMem[ i ], m_rgiAllocSize[ i ] );
			}
			else
			{
				HV_FreeMem( m_rgpAllocMem[ i ], m_rgiAllocSize[ i ]);
			}
            m_rgpAllocMem[ i ] = NULL;
		}
	}
	m_iAllocCount = 0;
}

HRESULT CFastMemAlloc::Detach( CFastMemAlloc& stackDest )
{
	for ( int i = 0; i < m_iAllocCount; i++ )
	{
		stackDest.m_rgpAllocMem[i] = m_rgpAllocMem[i];
		stackDest.m_rgiAllocSize[i] = m_rgiAllocSize[i];
		stackDest.m_rgiFastMem[i] = m_rgiFastMem[i];
	}
	stackDest.m_iAllocCount = m_iAllocCount;
	m_iAllocCount = 0;
	return S_OK;
}
