#ifndef CSWTASKTHREAD_H
#define CSWTASKTHREAD_H
#include "SWFC.h"
#include "SWObjectList.h"


class CSWTask;
class CSWTaskThreadPool;

class CSWTaskThread : public CSWThread
{
CLASSINFO(CSWTaskThread, CSWThread)
public:
    CSWTaskThread();
    virtual ~CSWTaskThread();

    // 设置线程停止状态，默认为非停止状态
    void SendStopThread() { m_bStopThread = true; }
    // 线程是否需要停止停止
    BOOL IsStopThread() { return m_bStopThread; }
    // 线程释放已经退出，线程一旦运行则会自动设置该状态，
    // 如果为TRUE，则线程已经退出，否则为正在活跃线程
    BOOL IsExited( ){ return m_bThreadIsStatus;}

    /**
     * @brief 线程主运行程序，各线程需重载该函数实现其功能。
     * @return - S_OK : 成功 - E_FAIL : 失败
     */
    virtual HRESULT Run();

private:

	CSWTask* WaitForTask( );
	// 存储任务列表
	CSWObjectList m_objTaskLink;
private:
    // 线程是否退出状态
    BOOL m_bThreadIsStatus;
    // 线程是否需要停止状态
    BOOL m_bStopThread;

	friend class CSWTask;
	friend class CSWTaskThreadPool;
};

#endif // CSWTASKTHREAD_H

