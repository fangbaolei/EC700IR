/**
* @file	hvthread.h
* @version	1.0
* @brief	线程基本函数接口
*/

#ifndef _HV_THREAD_H_
#define _HV_THREAD_H_

#include "hvutils.h"

// TODO: 根据不同平台包含头文件及参数结构体定义
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#define HV_THREAD_HANDLE pthread_t /**< 线程句柄定义 */
/**< 信号量句柄定义 */
typedef struct _tagHvSemHandle
{
	sem_t hSem;
	DWORD32 dwMaxCount;
}HV_SEM_HANDLE;

/**
*  HiVideo名字空间
*/
namespace HiVideo
{
    enum
    {
        PRIORITY_LOW = 0,
        PRIORITY_NORMAL,
        PRIORITY_HIGH,
        PRIORITY_HIGH_2,
        PRIORITY_KERNEL
    };
	enum
	{
	    MEM_FAST = 0,
	    MEM_NORMAL
    };

	/**
	* @brief Create a new thread
	* @param pHandle  [in,out] Handle of the thread
	* @param dwStackSize [in] The stack size for the new thread.
	* @param pStartRoutine [in, out] The thread procedure of the new thread
	* @param pvParam [in, out] The parameter to be passed to the thread procedure
	* @return If the function succeeds, the return value is 0, else return -1
	*/
	int CreateThread(
        HV_THREAD_HANDLE* pHandle,
        DWORD32 dwPriotiry,
        DWORD32 dwStackLocation,
		DWORD32 dwStackSize,
		void *(*pStartRoutine)(void*),
		void* pvParam
		);

	/**
	* @brief Suspend  execution of the calling thread until the target thread terminates, unless the target thread has already  terminated
	* @param hHandle [in] Handle of the thread
	* @return 0-succeed -1-failed
	*/
	int ThreadJoin(HV_THREAD_HANDLE hHandle);

	/**
	* @brief Creates or opens a semaphore object.
	* @param pHandle  [in,out] Handle of the semaphore
	* @param dwInitialCount [in] Initial count for the semaphore object. This value must be greater than or equal to zero and less than or equal to dwMaxCount
	* @param dwMaxCount [in] Maximum count for the semaphore object. This value must be greater than zero.
	* @return If the function succeeds, the return value is 0, else return -1
	*/
	int CreateSemaphore(HV_SEM_HANDLE* pHandle, DWORD32 dwInitialCount, DWORD32 dwMaxCount);

	/**
	* @brief Destroy the specified semaphore.
	* @param pHandle [in] Descriptor a semaphore
	* @return If the function succeeds, the return value is 0, else return -1
	*/
	int DestroySemaphore(HV_SEM_HANDLE* pHandle);

	/**
	* @brief Pend the specified semaphore
	* @param pHandle [in] Descriptor a semaphore
	* @param iTimeOut [in] unit:MS
	* @return If the function succeeds, the return value is 0, if timeout return 1, other error occured return -1
	*/
	int SemPend(HV_SEM_HANDLE* pHandle, int iTimeOut = -1);

	/**
	* @brief Post the specified semaphore
	* @param pHandle [in] Descriptor a semaphore
	* @param iWaitMode [in] 0-NOWAIT, 1-WAIT
	* @return If the function succeeds, the return value is 0, if timeout return 1, other error occured return -1
	*/
	int SemPost(HV_SEM_HANDLE* pHandle, int iWaitMode = 0);

	/**
	* @brief Get the specified semaphore value
	* @param pHandle [in] Descriptor a semaphore
	* @return the value of semaphore
	*/
	int SemGetValue(HV_SEM_HANDLE* pHandle);

    /**
    * @brief auto lock/unlock sem
    * @param pHandle [in] Descriptor a semaphore
    */
    class CSemLock
    {
    public:
        CSemLock(HV_SEM_HANDLE* pHandle)
        {
            m_pHandle = pHandle;
            SemPend(m_pHandle);
        }
        virtual ~CSemLock()
        {
            SemPost(m_pHandle);
        }
    private:
        HV_SEM_HANDLE * m_pHandle;
    };
}
#endif
