#include "stdafx.h"
#include <assert.h>
#include "Semaphore.h"

class CSemaphoreImpl : public ISemaphore
{
public:
	CSemaphoreImpl(int iInitCount, int iMaxCount)
		:m_hSemaphore(NULL)
	{
		m_hSemaphore = CreateSemaphore(NULL, iInitCount, iMaxCount, NULL);
	}
	virtual ~CSemaphoreImpl()
	{
		if(m_hSemaphore != NULL)
			CloseHandle(m_hSemaphore);
	}
	virtual HRESULT Pend(int iTimeOut /* = -1 */)
	{
		assert(m_hSemaphore != NULL);
		DWORD dwTimeOut = (iTimeOut == -1) ? INFINITE : iTimeOut;
		DWORD dwRet = WaitForSingleObject(m_hSemaphore, dwTimeOut);
		return (dwRet == WAIT_OBJECT_0) ? S_OK : S_FALSE;
	}
	virtual HRESULT Post(int iWaitMode /* = NOWAIT */)
	{
		if(iWaitMode == NOWAIT)
		{
			if(ReleaseSemaphore(m_hSemaphore, 1, NULL))
				return S_OK;
			return S_FALSE;
		}
		while(!ReleaseSemaphore(m_hSemaphore, 1, NULL))
			Sleep(100);
		return S_OK;
	}
private:
	HANDLE m_hSemaphore;
};

HRESULT ISemaphore::CreateInstance(ISemaphore** ppSemaphore, int iInitCount, int iMaxCount)
{
	if(ppSemaphore == NULL)
		return E_INVALIDARG;
	if((*ppSemaphore = new CSemaphoreImpl(iInitCount, iMaxCount)) == NULL)
		return E_OUTOFMEMORY;
	return S_OK;
}
