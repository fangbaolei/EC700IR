#ifndef _HVTHREAD_H_
#define _HVTHREAD_H_

namespace HiVideo
{
	//信号量接口
	class ISemaphore
	{
	public:
		virtual ~ISemaphore() {};
		enum {WAIT, NOWAIT};

		virtual HRESULT Pend(int iTimeOut = -1) = 0;
		virtual HRESULT Post(int iWaitMode = NOWAIT) = 0;

		static HRESULT CreateInstance(ISemaphore **ppSemaphore, int iInitCount, int iMaxCount);
	};

	//用户定义的线程类必须实现这个接口
	class IRunable
	{
	public:
		virtual HRESULT Run(void *pvParamter) = 0;
	};

	//线程接口
	class IThread
	{
	public:
		enum {PRIORITY_LOW = 0, PRIORITY_NORMAL, PRIORITY_HIGH, PRIORITY_HIGH_2, PRIORITY_KERNEL};
		enum {MEM_FAST = 0, MEM_NORMAL};

		virtual ~IThread() {};

		virtual HRESULT Start(void *pvParameter) = 0;
		virtual HRESULT Join() = 0;
		virtual HRESULT GetPriority(int *piPriority) = 0;
		virtual HRESULT SetPriority(int iPriority) = 0;
		virtual HRESULT GetLastStatus() = 0;
	
		static HRESULT Sleep(int iMs);
		static HRESULT CreateInstance(
				IThread **ppThread,
				int iPriority,
				int iStackSize,
				int iStackLocation,
				bool fStartImmediately,
				IRunable *pTarget	
			);
	};

	class IHvThreadBase
	{
	public:
		virtual HRESULT Start( void *pvParameter ) = 0;
		virtual HRESULT Join() = 0;
		virtual HRESULT GetPriority(int *piPriority) = 0;
		virtual HRESULT SetPriority(int iPriority) = 0;
	};

	class IHvThreadContrl
	{
	public: 
		virtual HRESULT StopThread(int nTimeOut = -1) = 0;
		virtual HRESULT Suspend() = 0;
		virtual HRESULT Resume() = 0;
		virtual HRESULT GetExitCode( DWORD32* pdwExitCode ) = 0;
	};
}

#endif

