
#include "SWTaskThreadPool.h"

CSWTaskThread**  CSWTaskThreadPool::m_sTaskThreadArray = NULL;
DWORD  CSWTaskThreadPool::m_dwNumTaskThreads = 0;
DWORD  CSWTaskThreadPool::m_dwNumShortTaskThreads = 0;
DWORD  CSWTaskThreadPool::m_dwNumBlockingTaskThreads = 0;


BOOL CSWTaskThreadPool::AddThreads(DWORD numToAdd)
{
    if( m_sTaskThreadArray != NULL ) delete []m_sTaskThreadArray;

    m_sTaskThreadArray = new CSWTaskThread*[numToAdd];

    for (DWORD x = 0; x < numToAdd; x++)
    {
        m_sTaskThreadArray[x] = new CSWTaskThread();
        m_sTaskThreadArray[x]->Start();
    }

    m_dwNumTaskThreads = numToAdd;

    if (0 == m_dwNumShortTaskThreads)
        m_dwNumShortTaskThreads = numToAdd;

    return true;
}
CSWTaskThread* CSWTaskThreadPool::GetThread(DWORD index)
{
  if (index >= m_dwNumTaskThreads) return NULL;

   return m_sTaskThreadArray[index];

}
void CSWTaskThreadPool::StopThreads()
{
    // 清理所有线程实例
    for (DWORD z = 0; z < m_dwNumTaskThreads; z++)
    {
        m_sTaskThreadArray[z]->SendStopThread( );
        while( !m_sTaskThreadArray[z]->IsExited( )){
            swpa_thread_sleep_ms( 10 );
        }

        delete m_sTaskThreadArray[z];
    }

	if( m_sTaskThreadArray )
        delete []m_sTaskThreadArray;
	m_sTaskThreadArray = NULL;

    m_dwNumTaskThreads = 0;
}

