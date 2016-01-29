#ifndef _HVTHREAD_H_
#define _HVTHREAD_H_

#define PLATFORM_WINDOWS			1

#ifdef WIN32
#define RUN_PLATFORM				PLATFORM_WINDOWS
#endif

namespace HiVideo
{
	//信号量接口
	class ISemaphore
	{
	public:
		enum {WAIT, NOWAIT};
		virtual ~ISemaphore() {};
		virtual long Pend(int iTimeOut = -1) = 0;
		virtual long Post(int iWaitMode = NOWAIT) = 0;

		static long CreateInstance(ISemaphore **ppSemaphore, int iInitCount, int iMaxCount);
	};

}

#endif

