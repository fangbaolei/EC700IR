#include "globalstackalloc.h"

#define MAX_MEM_ALLOC_SIZE 			( 2 * 512 * 1024 )

static BYTE8 GlobalStackBuf[ MAX_MEM_ALLOC_SIZE ] = {0};
DWORD32 dwNowMemPtr = 0;

BYTE8* GlobalStackAlloc( DWORD32 dwSize )
{
	static int flag = 0;
	BYTE8 *pTemp = NULL;
	DWORD32 dwTemp = dwSize;

	if ( flag == 0 )
	{
		dwNowMemPtr = 0;
		flag = 1;
	}
	if ( dwTemp & 0x07 ) dwTemp = ( dwTemp & ~0x07 ) + 8;
	if ( ( dwNowMemPtr + dwTemp ) > MAX_MEM_ALLOC_SIZE )
	{
		pTemp = NULL;
	}
	else
	{
		pTemp = ( BYTE8 * )&GlobalStackBuf[ dwNowMemPtr ];
		dwNowMemPtr += dwTemp;
	}
	return( pTemp );
}
