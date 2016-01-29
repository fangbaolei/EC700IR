
#include "SWTaskThread.h"
#include "SWTask.h"


CSWTaskThread::CSWTaskThread()
: m_bThreadIsStatus( true )
, m_bStopThread( false )
{

}

CSWTaskThread::~CSWTaskThread()
{
    //dtor
}

/**
 * @brief 线程主运行程序，各线程需重载该函数实现其功能。
 * @return - S_OK : 成功 - E_FAIL : 失败
 */
HRESULT CSWTaskThread::Run( )
{
    CSWTask* theTask = NULL; m_bThreadIsStatus = false;
    while (true)
    {
        theTask = this->WaitForTask();

        // 如果等待线程为空，则退出运行
        if (theTask == NULL ){ m_bThreadIsStatus = true; return S_OK; }

		BOOL doneProcessingEvent = false;
		while( !doneProcessingEvent )
		{
			INT theTimeout = theTask->Run();

            // 小于零则退出该线程
			if( theTimeout < 0 ){
				delete theTask; theTask = NULL;
				doneProcessingEvent = true;
			}
			else if( theTimeout == 0 ){
                 doneProcessingEvent = compare_and_store( CSWTask::kAlive , 0 , &theTask->m_uEvents);

			     if( doneProcessingEvent ) theTask = NULL;
			}
			else {
                // 否则将该任务加入任务池
				theTask->SetTimeValue(CSWTask::Milliseconds() + theTimeout);

				m_objTaskLink.Push( theTask );
				doneProcessingEvent = true;
			}
        }
	}

    return S_OK;
}
CSWTask* CSWTaskThread::WaitForTask( )
{
	while (true)
    {
		SInt64 theCurrentTime = CSWTask::Milliseconds();
		CSWTask *theTask = (CSWTask*)m_objTaskLink.Pop( 100 ); // 最大堵塞100毫秒
		if( theTask != NULL ){
			if( theTask->m_sLastRunTimeMilli < theCurrentTime )
			{
				return theTask;
            }
            else if( this->IsStopThread() ) return theTask;
		}
        else if ( this->IsStopThread()) return NULL;

		INT theTimeout = 0 ;
		if( theTask != NULL ) // 获取堵塞事件
			theTimeout = theTask->m_sLastRunTimeMilli - theCurrentTime;
		// 如果该任务阻塞大于10毫秒，则寻找其他任务
		if( theTimeout > 10 ){ swpa_thread_sleep_ms( 10 ); m_objTaskLink.Push( theTask ); theTask = NULL;}

		if( theTask ) return theTask;
	}
}

