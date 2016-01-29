#ifndef CSWTASKTHREADPOOL_H
#define CSWTASKTHREADPOOL_H
#include "SWTaskThread.h"

class CSWTaskThreadPool : CSWObject
{
CLASSINFO(CSWTaskThreadPool, CSWObject)
public:

    // Adds some threads to the pool
    static BOOL   AddThreads(DWORD numToAdd); // creates the threads: takes NumShortTaskThreads + NumBLockingThreads,  sets num short task threads.
    static void     SwitchPersonality( CHAR *user = NULL, CHAR *group = NULL);
    static CSWTaskThread* GetThread( DWORD index );
    static DWORD  GetNumThreads() { return m_dwNumTaskThreads; }
    static void SetNumShortTaskThreads(DWORD numToAdd) { m_dwNumShortTaskThreads = numToAdd; }
    static void SetNumBlockingTaskThreads(DWORD numToAdd) { m_dwNumBlockingTaskThreads = numToAdd; }
    // 停止所有线程及任务
    static void StopThreads();

private:

    static CSWTaskThread**  m_sTaskThreadArray;
    static DWORD           m_dwNumTaskThreads;
    static DWORD           m_dwNumShortTaskThreads;
    static DWORD           m_dwNumBlockingTaskThreads;

    friend class CSWTask;
    friend class CSWTaskThread;
};

#endif // CSWTASKTHREADPOOL_H

