#include "string.h"
#include "stdio.h"
#include "stdlib.h"


class CSWFifo
{
public:
	CSWFifo();
	~CSWFifo();

	int SetBufSize(unsigned int unSize);

	void DestroyFifo(void);

	int Push(void *pData,unsigned int unLen);
	int Get(void *pBuf,unsigned int unLen);

	void SetPushTimeOutMs(unsigned int unMs)
	{
		m_unPushTimeOMs = unMs;
	};
	void SetGetTimeOutMs(unsigned int unMs)
	{
		m_unGetTimeOMs = unMs;
	};
	/*
	void ClearFifo(void)	//线程安全问题
	{
		m_unWrPos = 0;
		m_unRdPos = 0;
		m_pWrite = m_pBuf;
		m_pRead = m_pBuf;
	};*/

	void PrintFifoState(void);
private:
	unsigned int GetNullLen(void);

	unsigned int GetDataLen(void);

private:
	char *m_pBuf;

	char *m_pWrite;

	char *m_pRead;

	unsigned int m_unWrPos;
	unsigned int m_unRdPos;

	unsigned int m_unBufSize;

	unsigned int m_unTotalPush;
	unsigned int m_unTotalGet;

	unsigned int m_unPushTimeOMs;
	unsigned int m_unGetTimeOMs;

};