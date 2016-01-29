/*
  Date: 	2015/03/26
*/
#include "SWMuxer.h"

CSWMuxer::CSWMuxer()
{}

CSWMuxer::~CSWMuxer()
{}

INT CSWMuxer::InitMuxer(VOID *pParam)
{
	return S_OK;
}

INT CSWMuxer::Mux(BLOCK_T **pBlock)
{
	return S_OK;
}


INT CSWMuxer::BitInitWrite( BIT_BUFFER_T *pBuffer,
                                  INT iSize, VOID *pData )
{
    pBuffer->iSize = iSize;
    pBuffer->iData = 0;
    pBuffer->iMask = 0x80;
    pBuffer->pData = (unsigned char *)pData;
    if( !pBuffer->pData )
    {
        if( !( pBuffer->pData = new unsigned char[iSize] ) )
            return -1;
    }
    pBuffer->pData[0] = 0;
    return 0;
}


VOID CSWMuxer::BitWrite( BIT_BUFFER_T *pBuffer, INT iCount, unsigned long int iBits )
{
    while( iCount > 0 )
    {
        iCount--;

        if( ( iBits >> iCount ) & 0x01 )
        {
            pBuffer->pData[pBuffer->iData] |= pBuffer->iMask;
        }
        else
        {
            pBuffer->pData[pBuffer->iData] &= ~pBuffer->iMask;
        }
        pBuffer->iMask >>= 1;
        if( pBuffer->iMask == 0 )
        {
            pBuffer->iData++;
            pBuffer->iMask = 0x80;
        }
    }
}

VOID CSWMuxer::BlockChainAppend(BLOCK_T **pBlock, BLOCK_T *pAppBlock)
{
	if(*pBlock == NULL)
	{
		*pBlock = pAppBlock;
	}
	else
	{
		BLOCK_T *pTmpBlock = *pBlock;
		while(pTmpBlock->pNext != NULL)
			pTmpBlock = pTmpBlock->pNext;

		pTmpBlock->pNext = pAppBlock;
	}
}

VOID CSWMuxer::BlockChainFree(BLOCK_T *pBlock)
{
	BLOCK_T *pTmpBlock = NULL;
	while(pBlock != NULL)
	{
		pTmpBlock = pBlock;
		pBlock = pBlock->pNext;
		SAFE_DELETE(pTmpBlock->pBuffer);
		SAFE_DELETE(pTmpBlock);
	}
}
