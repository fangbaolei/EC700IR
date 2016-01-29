#include "SWFifo.h"
#include <unistd.h>

#include "SWFC.h"

#if 0
#define DEBUG printf
#else
#define DEBUG
#endif

CSWFifo::CSWFifo()
	:m_pBuf(NULL)
	,m_pWrite(NULL)
	,m_pRead(NULL)
	,m_unWrPos(0)
	,m_unRdPos(0)
	,m_unBufSize(0)
	,m_unTotalPush(0)
	,m_unTotalGet(0)
	,m_unPushTimeOMs(5000)
	,m_unGetTimeOMs(5000)
{
}

CSWFifo::~CSWFifo()
{

}

int CSWFifo::SetBufSize(unsigned int unSize)
{
	m_pBuf = (char *)swpa_mem_alloc(unSize);
	if (NULL == m_pBuf)
	{
		printf("set buf size failed %d\n",unSize);
		return -1;
	}
	m_pWrite = m_pBuf;
	m_pRead = m_pBuf;
	m_unBufSize = unSize;
	return 0;
}

void CSWFifo::DestroyFifo(void)
{
	if (NULL != m_pBuf)
	{
		swpa_mem_free(m_pBuf);
		m_pBuf = NULL;
	}
	
	m_pWrite = NULL;
	m_pRead = NULL;
	m_unWrPos = 0;
	m_unRdPos = 0;
	m_unBufSize = 0;

}

unsigned int CSWFifo::GetNullLen(void)
{
	unsigned int unRdPos = m_unRdPos;
	if (m_unWrPos < unRdPos)
	{
		return (unRdPos - m_unWrPos);
	}
	else if (m_unWrPos > unRdPos)
	{
		return (m_unBufSize - (m_unWrPos - unRdPos));
	}
	else	//相等，空的
	{
		return m_unBufSize;
	}
}

unsigned int CSWFifo::GetDataLen(void)
{
	unsigned int unWrPos = m_unWrPos;
	if (unWrPos < m_unRdPos)
	{
		return (m_unBufSize - (m_unRdPos - unWrPos));
	}
	else if (unWrPos > m_unRdPos)
	{
		return unWrPos - m_unRdPos;
	}
	else	//追上表示为空，没有满的时候
	{
		return 0;
	}
}

int CSWFifo::Push(void *pData,unsigned int unLen)
{
	unsigned int unLastTick = CSWDateTime::GetSystemTick();
	//buffer满
	while(GetNullLen() <= unLen)
	{
		//超时失败返回
		if ((CSWDateTime::GetSystemTick() - unLastTick) > m_unPushTimeOMs)
		{
			DEBUG("push data time out!no buf to push data\n");
			return -1;
		}
		CSWApplication::Sleep(10000);
	}

	unsigned int unWriteBufLen = m_unBufSize - m_unWrPos;

	if (unWriteBufLen <= unLen)//分段
	{
		swpa_memcpy(m_pWrite,pData,unWriteBufLen);//到最尾了
		char *pNextCopy = (char *)pData + unWriteBufLen;
		m_pWrite = m_pBuf;
		swpa_memcpy(m_pWrite,pNextCopy,unLen - unWriteBufLen);
		m_pWrite += (unLen - unWriteBufLen);
		m_unWrPos = unLen - unWriteBufLen;
	}
	else
	{
		swpa_memcpy(m_pWrite,pData,unLen);

		m_unWrPos += unLen;
		m_pWrite += unLen;
	}
	DEBUG("<<push data len %d,wpos %d,rdpos %d\n",unLen,m_unWrPos,m_unRdPos);
	m_unTotalPush += unLen;
	return unLen;
}

int CSWFifo::Get(void *pBuf,unsigned int unLen)
{
	unsigned int unNeedLen = unLen;
	unsigned int unLastTick = CSWDateTime::GetSystemTick();

	while(GetDataLen() < unNeedLen)
	{
		//超时失败返回
		if ((CSWDateTime::GetSystemTick() - unLastTick) > m_unGetTimeOMs)
		{
			DEBUG("get data time out!no data get\n");
			return -1;
		}
		CSWApplication::Sleep(10000);
	}

	unsigned int unCanReadLen = m_unBufSize - m_unRdPos;

	if (unCanReadLen <= unNeedLen)
	{
		swpa_memcpy(pBuf,m_pRead,unCanReadLen);
		m_pRead = m_pBuf;
		char *pNextCopyTo = (char *)pBuf + unCanReadLen;
		swpa_memcpy(pNextCopyTo,m_pRead,unNeedLen - unCanReadLen);
		m_pRead += unNeedLen - unCanReadLen;
		m_unRdPos = unNeedLen - unCanReadLen;
	}
	else
	{
		swpa_memcpy(pBuf,m_pRead,unNeedLen);
		m_pRead += unNeedLen;
		m_unRdPos += unNeedLen;
	}

	DEBUG(">>get data len %d,wpos %d,rdpos %d\n",unNeedLen,m_unWrPos,m_unRdPos);
	m_unTotalGet += unNeedLen;

	return unNeedLen;
}

void CSWFifo::PrintFifoState(void)
{
	//printf("\nTotal Packets:%lld\n",m_ullTotPkts);

	printf("\nTotal Insert:%d\n",m_unTotalPush);

	printf("Total Read:%d\n",m_unTotalGet);

	//printf("Total Lost:%lld\n",m_ullTotLost);

	printf("Insert off:%d\n",m_unWrPos);

	printf("Remove off:%d\n",m_unRdPos);

	printf("Total memory size:%d\n",m_unBufSize);

	//printf("User head length:%d\n",m_unUserHeadLen);

	//printf("Slot head length:%d\n",m_unSlotHeadLen);
}