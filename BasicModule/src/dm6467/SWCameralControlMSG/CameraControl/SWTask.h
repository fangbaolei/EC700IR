#ifndef CSWTASK_H
#define CSWTASK_H
#include "SWObject.h"
#include "SWAutoLock.h"

class CSWTaskThread;
class CSWTask : public CSWObject
{
CLASSINFO(CSWTask, CSWObject)

private:
    static SInt64  m_sMsecSince1970;
    static SInt64  m_sInitialMsec;
public:
    typedef DWORD EventFlags;
	EventFlags  m_uEvents;

    // 任务事件
    enum
    {
        kKillEvent =    0x1 << 0x0, // 事件当前状态标志
        kIdleEvent =    0x1 << 0x1,
        kStartEvent =   0x1 << 0x2,
       //update event
        kAlive =        0x80000000, // 活跃事件标志
        kAliveOff =     0x7fffffff
    };

    static SInt64 Milliseconds();

public:
    CSWTask();
    virtual ~CSWTask();
    CSWTask::EventFlags GetEvents();


	virtual SInt64 Run() = 0;
	HRESULT Signal( CSWTask::EventFlags events );

	void SetTimeValue( SInt64 uLastRunTimeMilli ){
		m_sLastRunTimeMilli = uLastRunTimeMilli;}

    void SetThreadPicker(unsigned int* picker);
private:

	SInt64 m_sLastRunTimeMilli;



	static DWORD m_dwShortTaskThreadPicker; // 默认线程选择
    static DWORD m_dwBlockingTaskThreadPicker;

    DWORD *m_dwPickerToUse;

	friend class CSWTaskThread;
};

static CSWRWMutex fobjSWRWMutex;

static DWORD atomic_or(DWORD *area, DWORD val)
{
    CSWAutoLock objSWAutoLock( &fobjSWRWMutex );

    DWORD oldval;

    oldval=*area;
    *area = oldval | val;
    return oldval;
};
static DWORD atomic_add(DWORD *area, DWORD val)
{
    CSWAutoLock objSWAutoLock( &fobjSWRWMutex );
    *area += val;
    return *area;
}
static DWORD atomic_sub(DWORD *area,INT val)
{
    CSWAutoLock objSWAutoLock( &fobjSWRWMutex );
    return atomic_add(area,-val);
}
static DWORD compare_and_store(DWORD oval, DWORD nval, DWORD *area)
{
    CSWAutoLock objSWAutoLock( &fobjSWRWMutex );

    int rv = 0;
    if( oval == *area ){
        rv=1; *area = nval;

    }

    return rv;
}
#endif // CSWTASK_H

