#include "SWTask.h"
#include "SWTaskThreadPool.h"
#include <time.h>

DWORD CSWTask::m_dwShortTaskThreadPicker = 0;
DWORD CSWTask::m_dwBlockingTaskThreadPicker = 0;

SInt64 CSWTask::m_sMsecSince1970 = 0;
SInt64  CSWTask::m_sInitialMsec = 0;

CSWTask::CSWTask()
: m_dwPickerToUse( &CSWTask::m_dwShortTaskThreadPicker )
, m_uEvents( 0 )
{
    // 初始化全局变量
    if( CSWTask::m_sMsecSince1970 == 0 ){
        CSWTask::m_sMsecSince1970 = time(NULL)*1000;
        CSWTask::m_sInitialMsec = CSWTask::Milliseconds();
    }

    m_sLastRunTimeMilli = 0;
}

CSWTask::~CSWTask()
{
    //dtor
}
SInt64 CSWTask::Milliseconds()
{
    return (swpa_datetime_gettick() - CSWTask::m_sInitialMsec ) + CSWTask::m_sMsecSince1970;
}
void CSWTask::SetThreadPicker(unsigned int* picker)
{
    m_dwPickerToUse = picker;
}
CSWTask::EventFlags CSWTask::GetEvents()
{
    EventFlags events = m_uEvents & kAliveOff;
    (void)atomic_sub(&m_uEvents, events);
    return events;
}
HRESULT CSWTask::Signal(CSWTask::EventFlags events)
{
	events |= kAlive;
    EventFlags oldEvents = atomic_or(&m_uEvents, events);

    if ((!(oldEvents & kAlive)) && (CSWTaskThreadPool::m_dwNumTaskThreads > 0))
    {
        // 查找一个线程以绑定任务
        DWORD theThreadIndex = atomic_add((DWORD *)m_dwPickerToUse, 1);
        if (&CSWTask::m_dwShortTaskThreadPicker == m_dwPickerToUse)
        {
            theThreadIndex %= CSWTaskThreadPool::m_dwNumShortTaskThreads;
        }
        else if (&CSWTask::m_dwBlockingTaskThreadPicker == m_dwPickerToUse)
        {
            theThreadIndex %= CSWTaskThreadPool::m_dwNumBlockingTaskThreads;
            theThreadIndex += CSWTaskThreadPool::m_dwNumShortTaskThreads; //don't pick from lower non-blocking (short task) threads.
        }
        else return S_FALSE;

        CSWTaskThreadPool::m_sTaskThreadArray[theThreadIndex]->m_objTaskLink.Push( this );

        // 如果线程已经停止运行则重新启动
        if( CSWTaskThreadPool::m_sTaskThreadArray[theThreadIndex]->IsStopThread() )
            CSWTaskThreadPool::m_sTaskThreadArray[theThreadIndex]->Start( );
    }

    return S_OK;
}

