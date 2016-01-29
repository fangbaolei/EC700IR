//#include <windows.h>
#include <semaphore.h>
#include <assert.h>
#include <errno.h>

#include "HvDeviceEx.h"
#include "HvCameraType.h"
#include "HVAPI_HANDLE_CONTEXT_EX.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>

#include<time.h>
#include "hvthreadex.h"

namespace HiVideo
{
	class CSemaphoreImpl: public ISemaphore
	{
	public:
		CSemaphoreImpl(int iInitCount, int iMaxCount)
			:m_fFlag(false)
		{
            if (0 == sem_init(&m_hSemaphore, 0, iInitCount))
			{
			    m_fFlag = true;
                m_iMaxCount = iMaxCount;
            }
		};
		virtual ~CSemaphoreImpl()
		{
            if (m_fFlag)
			{
				if (0 == sem_destroy(&m_hSemaphore))
				{
				    m_fFlag = false;
                }
			}
		};
		virtual long Pend( int iTimeOut = -1 )
		{
            assert(m_fFlag == true);
			int nRet(0);

			if (iTimeOut == -1)
			{
				//等待信号量，errno==EINTR屏蔽其他信号事件引起的等待中断
				while(((nRet = sem_wait(&m_hSemaphore)) != 0 ) && (errno == EINTR));

			}
			else
			{
				// 设置时间
				timespec ts;
				clock_gettime(CLOCK_REALTIME, &ts );    //获取当前时间
				ts.tv_sec += (iTimeOut / 1000 );			//加上等待时间的秒数
				ts.tv_nsec += ( iTimeOut % 1000 ) * 1000000; //加上等待时间纳秒数

				//等待信号量，errno==EINTR屏蔽其他信号事件引起的等待中断
				while(((nRet = sem_timedwait( &m_hSemaphore, &ts ))!= 0) && (errno == EINTR) );
			}
			return (nRet == 0) ? S_OK : S_FALSE;
		};
		virtual long Post(int iWaitMode)
		{

			int nVal(0);
			if (iWaitMode == NOWAIT)
			{
			    if (0 == sem_getvalue(&m_hSemaphore, &nVal))
			    {
			        if (nVal == m_iMaxCount)
			            return S_FALSE;

                    if (0 == sem_post(&m_hSemaphore))
                        return S_OK;
                    else
                        return S_FALSE;
                }
                else
                    return S_FALSE;
			}
			else
			{
			    nVal = m_iMaxCount;
			    while(nVal == m_iMaxCount)
			    {
                    if (0 == sem_getvalue(&m_hSemaphore, &nVal))
                    {
                        if (nVal == m_iMaxCount)
                        {
                            usleep(100000);
                        }
                        else
                        {
                            if (0 == sem_post(&m_hSemaphore))
                                return S_OK;
                            return S_FALSE;
                        }
                    }
                    else
                        return S_FALSE;
                }
			}

			return S_OK;
		};
	private:
		sem_t m_hSemaphore;
		bool m_fFlag;
		int m_iMaxCount;
	};

	HRESULT ISemaphore::CreateInstance(ISemaphore **ppSemaphore, int iInitCount, int iMaxCount)
	{
		if (ppSemaphore == NULL)
			return E_INVALIDARG;
		if ((*ppSemaphore = new CSemaphoreImpl(iInitCount, iMaxCount)) == NULL)
			return E_OUTOFMEMORY;
		return S_OK;
	}
}
