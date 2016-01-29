#include "hvthread.h"
#include <time.h>
#include <errno.h>

namespace HiVideo
{

int CreateThread(
        HV_THREAD_HANDLE* pHandle,
        DWORD32 dwPriotiry,
		DWORD32 dwStackSize,
		DWORD32 dwStackLocation,
		void *(*pStartRoutine)(void*),
		void* pvParam
		)
{
	pthread_attr_t attrThread;
	pthread_attr_init (&attrThread);				//初始化属性
	if (dwStackSize > 0)
	{
		pthread_attr_setstacksize(&attrThread, dwStackSize);		// 设置堆栈
	}

	int nRet = pthread_create(
		pHandle,
		&attrThread,
		pStartRoutine,
		pvParam
		);

	if (nRet != 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int ThreadJoin(HV_THREAD_HANDLE hHandle)
{
	return pthread_join(hHandle, NULL);
}

int CreateSemaphore(HV_SEM_HANDLE* pHandle, DWORD32 dwInitialCount, DWORD32 dwMaxCount)
{
	if (0 == sem_init(&pHandle->hSem, 0, dwInitialCount))
	{
		pHandle->dwMaxCount = dwMaxCount;
		return 0;
	}
	else
	{
	    return -1;
	}
}

int DestroySemaphore(HV_SEM_HANDLE* pHandle)
{
	return sem_destroy(&(pHandle->hSem));
}

int SemPend(HV_SEM_HANDLE* pHandle, int iTimeOut/* = -1*/)
{
    int nRet(0);
	if (iTimeOut == -1)
	{
		//等待信号量，errno==EINTR屏蔽其他信号事件引起的等待中断
		int i = sem_wait(&(pHandle->hSem));
		return i;
	}
	else
	{
		// 设置时间
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);    //获取当前时间
		ts.tv_sec += (iTimeOut / 1000);			//加上等待时间的秒数
		ts.tv_nsec += (iTimeOut % 1000) * 1000000; //加上等待时间纳秒数
		// 纳秒数超过1秒时需放到秒里
		ts.tv_sec += ts.tv_nsec / 1000000000;
		ts.tv_nsec = ts.tv_nsec % 1000000000;

		//等待信号量，errno==EINTR屏蔽其他信号事件引起的等待中断
		while(((nRet = sem_timedwait(&(pHandle->hSem), &ts))!= 0) && (errno == EINTR) );
	}
	return (nRet == 0) ? 0 : 1;
}

int SemPost(HV_SEM_HANDLE* pHandle, int iWaitMode/* = 0*/)
{
	int nVal(0);
	if (iWaitMode == 0)
	{
		if (0 == sem_getvalue(&(pHandle->hSem), &nVal))
		{
			if (nVal == (int)pHandle->dwMaxCount)
			{
				return 1;
			}
			if (0 == sem_post(&(pHandle->hSem)))
			{
				return 0;
			}
			else
			{
			    HV_Trace(5, "sem_post error==%s\n", strerror(errno));
				return -1;
			}
		}
		else
		{
		    HV_Trace(5, "sem_getvalue error==%s\n", strerror(errno));
			return -1;
		}
	}
	else
	{
		nVal = pHandle->dwMaxCount;
		while(nVal == (int)pHandle->dwMaxCount)
		{
			if (0 == sem_getvalue(&(pHandle->hSem), &nVal))
			{
				if (nVal >= (int)pHandle->dwMaxCount)
				{
					usleep(50000);
				}
				else
				{
					if (0 == sem_post(&(pHandle->hSem)))
					{
						return 0;
					}
					else
					{
					    HV_Trace(5, "sem_post error==%s\n", strerror(errno));
						return -1;
					}
				}
			}
			else
			{
			    HV_Trace(5, "sem_getvalue error==%s\n", strerror(errno));
				return -1;
			}
		}
	}
	return 0;
}

int SemGetValue(HV_SEM_HANDLE* pHandle)
{
    int nVal(0);
    if (pHandle)
    {
        if (sem_getvalue(&(pHandle->hSem), &nVal))
        {
            HV_Trace(5, "sem_getvalue error==%s\n", strerror(errno));
            nVal = 0;
        }
    }
    return nVal;
}
}
