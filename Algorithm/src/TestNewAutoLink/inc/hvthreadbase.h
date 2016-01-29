#ifndef _CJTHREAD_H_
#define _CJTHREAD_H_

#include "HvThread.h"
#include "HvUtils.h"
#include <assert.h>

//Hv线程基类
class CHvThreadBase : public HiVideo::IHvThreadBase
					, public HiVideo::IRunable
					, public HiVideo::IHvThreadContrl
{  
public:	//IHvThread实现
	virtual HRESULT Start( void* pvParam )
	{ 
		//如果已有线程运行,先关闭再重新创建
		StopThread();

		m_pvParam = pvParam;
		m_fExit = FALSE;
		HRESULT hr = HiVideo::IThread::CreateInstance(
			&m_pThread,
			m_iPriotiry,
			m_iStackSize,
			m_iStackLocation,
			false,
			m_pRunObj ? m_pRunObj : this);
		if (hr == S_OK)
		{
			hr = m_pThread->Start(pvParam);
		}
		return hr;
	}
	virtual HRESULT Join()//等待线程退出
	{
		if ( m_pThread == NULL ) return S_FALSE;
		return m_pThread->Join();
	};
	virtual HRESULT GetPriority(int *piPriority)
	{
		if ( m_pThread == NULL ) return S_FALSE;
		return m_pThread->GetPriority(piPriority);
	};
	virtual HRESULT SetPriority(int iPriority)
	{
		if ( m_pThread == NULL ) return S_FALSE;
		return m_pThread->SetPriority(iPriority);
	};
	virtual HRESULT GetLastStatus()
	{
		if ( m_pThread == NULL ) return S_FALSE;
		return m_pThread->GetLastStatus();
	}

public:	//IRunable 实现
	virtual HRESULT Run(void* pThreadInfo) = 0;//{ return 0 ; }

public:	//IHvThreadControl实现
	virtual HRESULT StopThread(int nTimeOut = -1)
	{
		m_fExit = TRUE;
	
		if(m_pThread == NULL) return S_OK;

		if(nTimeOut == -1)
		{
				m_pThread->Join();
				delete m_pThread;
				m_pThread = NULL;
		}
#if RUN_PLATFORM == PLATFORM_WINDOWS //Win32
		else 
		{
			DWORD32 dwExitTime = GetSystemTick() + nTimeOut;
			while(S_OK == m_pThread->GetLastStatus())
			{
				if( GetSystemTick() > dwExitTime) //如果线程还在运行,则强制退出
				{
					ExitThread(-1);
				}
				Sleep(10);
			}
		}
#endif

		return S_OK;
	}

	virtual HRESULT Suspend()
	{
		return E_NOTIMPL;
	}

	virtual HRESULT Resume()
	{
		return E_NOTIMPL;
	}

	virtual HRESULT GetExitCode( DWORD32* pdwExitCode )
	{
		//if ( pdwExitCode == NULL ) return E_INVALIDARG;

		//HRESULT hr = S_OK;

		//if ( m_hThread == NULL )
		//{
		//	*pdwExitCode = (DWORD32)-1;
		//	hr = E_FAIL;
		//}
		//else
		//{
		//	hr = GetExitCodeThread( m_hThread, (LPDWORD)pdwExitCode )?S_OK:E_FAIL;
		//}

		//return hr;
		return E_NOTIMPL;
	}

public:
	CHvThreadBase(int iPriotiry = HiVideo::IThread::PRIORITY_NORMAL, int iStackSize = 0, int iStackLocation = HiVideo::IThread::MEM_NORMAL, HiVideo::IRunable* pRunObj = NULL, PVOID pvParam = NULL )
		:m_iPriotiry(iPriotiry)
		,m_iStackSize(iStackSize)
		,m_iStackLocation(iStackLocation)
		,m_pRunObj(pRunObj)
		,m_pvParam(pvParam)
		,m_pThread(NULL)
		,m_fExit(TRUE)
		{}

	virtual ~CHvThreadBase()
	{
		StopThread();
	}

public:
	void* m_pvParam;
	BOOL m_fExit;

protected:
	int m_iPriotiry;
	int m_iStackSize;
	int m_iStackLocation;

	IRunable *m_pRunObj;
	HiVideo::IThread* m_pThread;
};

//template <class T>
//class CHvThread : public CHvThreadBase
//{
//	typedef DWORD32 (T::*ThreadFunc)(PVOID pvParam);
//	ThreadFunc ThreadBody;
//	T *m_TheadFuncObj;
//
//protected:
//	virtual DWORD32 Run(PVOID pvParam)
//	{
//		return (DWORD32)((m_TheadFuncObj->*ThreadBody)(pvParam));
//	}
//
//public:
//	CHvThread (T &obj,ThreadFunc _threadfunc)
//		:m_TheadFuncObj(&obj)
//		,ThreadBody(_threadfunc) 
//	{}
//};

class CSemLock
{
public:
	CSemLock(HiVideo::ISemaphore* pSem, BOOL fInitLock = FALSE)
		: m_pSem(pSem)
		, m_fLocked(FALSE)
	{
		if (fInitLock) Lock();
	}

	~CSemLock()
	{
		if ( m_fLocked ) UnLock();
	}

	BOOL IsLocked()
	{
		return m_fLocked;
	}

	BOOL Lock( int nTimeOut = -1)
	{
		if ( m_pSem == NULL ) return FALSE;

		if ( 0 == m_pSem->Pend(nTimeOut) )
		{
			m_fLocked = TRUE;
		}

		return m_fLocked?TRUE:FALSE;
	}

	BOOL UnLock()
	{
		if ( m_pSem == NULL ) return FALSE;

		if ( 0 == m_pSem->Post() )
		{
			m_fLocked = FALSE;
		}

		return m_fLocked?FALSE:TRUE;
	}

protected:
	HiVideo::ISemaphore* m_pSem;
	BOOL m_fLocked;
};

//class CHvEvent
//{
//public:
//	CHvEvent( BOOL fManualReset, BOOL fInitState = FALSE)
//		: m_hEvent(NULL)
//	{
//		m_hEvent = CreateEvent( NULL, fManualReset, fInitState, NULL);
//	}
//
//	~CHvEvent()
//	{
//		if ( m_hEvent != NULL ) CloseHandle(m_hEvent);
//	}
//
//	BOOL SetEvent()
//	{
//		assert(m_hEvent != NULL);
//		return ::SetEvent(m_hEvent);
//	}
//
//	BOOL Reset()
//	{
//		assert(m_hEvent != NULL);
//		return ResetEvent(m_hEvent);
//	}
//
//	HRESULT WaitEvent( int nTimeOut )
//	{
//		assert(m_hEvent != NULL);
//		return (WAIT_OBJECT_0 == WaitForSingleObject(m_hEvent, nTimeOut))?S_OK:S_FALSE;
//	}
//
//protected:
//	HANDLE m_hEvent;
//};

#endif
