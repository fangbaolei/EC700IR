/**
* @file	hvthreadbase.h
* @version 1.0
* @brief 线程基类接口及实现
*/

#ifndef _HV_THREAD_BASE_H_
#define _HV_THREAD_BASE_H_

#include "hvthread.h"
#include "hvutils.h"
#include "HvList.h"

/**
*  Hv线程实现体接口,用户定义的线程类必须实现这个接口
*/
class IRunable
{
public:
    virtual HRESULT Run(void* pvParam) = 0;
};

/**
*  Hv线程接口
*/
class IHvThreadBase
{
public:
    virtual HRESULT Start(void* pvParam) = 0;
    virtual HRESULT Stop(int nTimeOut = -1) = 0;
    virtual HRESULT GetLastStatus() = 0;
};

class CHvThreadBase : public IHvThreadBase, public IRunable
{
public:
    CHvThreadBase();
    virtual ~CHvThreadBase();
    static void StopAllThread(void);

public:	//IHvThread实现
    virtual HRESULT Start(void* pvParam);
    virtual HRESULT Stop(int nTimeOut = -1);
    virtual HRESULT GetLastStatus();

public:	//IRunable 实现
    virtual HRESULT Run(void* pvParam) = 0;

public:
    virtual const char* GetName()
    {
        static char szName[] = {"*"};
        return szName;
    }

public:
    BOOL m_fExit;

protected:
    static void* ThreadFunction(LPVOID lpParameter);
    void* m_pvParam;
    int m_iPriotiry;
    int m_iStackSize;
    int m_iStackLocation;
    HV_THREAD_HANDLE m_hThread;
    HV_SEM_HANDLE m_hSemJoin;
    bool m_fIsRun;
    HRESULT m_hrLastStatus;

    //管理所有的线程的队列，黄国超增加，目的是为了进程退出时能够正常的退出所有的线程
    static bool m_fInitialize;
    static HV_SEM_HANDLE m_hSemThread;
    static list<CHvThreadBase *> m_lstThread;
};

#endif
