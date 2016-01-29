#pragma once

class ISemaphore
{
public:
	virtual ~ISemaphore() {};
	enum {WAIT, NOWAIT};

	virtual HRESULT Pend(int iTimeOut = -1) = 0;
	virtual HRESULT Post(int iWaitMode = NOWAIT) = 0;

	static HRESULT CreateInstance(ISemaphore** ppSemaphore, int iInitCount, int iMaxCount);
};