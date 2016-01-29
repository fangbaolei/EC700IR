#include "hvthreadbase.h"

using namespace HiVideo;

bool CHvThreadBase::m_fInitialize = false;
HV_SEM_HANDLE CHvThreadBase::m_hSemThread;
list<CHvThreadBase *> CHvThreadBase::m_lstThread;

CHvThreadBase::CHvThreadBase()
        : m_fExit(TRUE)
        , m_pvParam(NULL)
        , m_iPriotiry(PRIORITY_NORMAL)
        , m_iStackSize(64*1024)
        , m_iStackLocation(MEM_NORMAL)
        , m_fIsRun(false)
        , m_hrLastStatus(S_FALSE)
{
    CreateSemaphore(&m_hSemJoin, 0, 1);

    if (!m_fInitialize)
    {
        m_fInitialize = true;
        CreateSemaphore(&m_hSemThread, 1, 1);
    }

    m_lstThread.push_back(this);
}

CHvThreadBase::~CHvThreadBase()
{
    DestroySemaphore(&m_hSemJoin);

    CSemLock slock(&m_hSemThread);
    for (list<CHvThreadBase *>::iterator itr = m_lstThread.begin(); itr != m_lstThread.end(); itr++)
    {
        if (*itr == this)
        {
            m_lstThread.erase(itr);
            break;
        }
    }
}

void CHvThreadBase::StopAllThread(void)
{
    HV_Trace(5, "stopping all thread...\n");

    SemPend(&m_hSemThread);
    int iInThreadCount = 0;
    for (list<CHvThreadBase *>::iterator itr = m_lstThread.begin(); itr != m_lstThread.end(); itr++)
    {
        if ((*itr)->m_hThread == pthread_self())
        {
            iInThreadCount++;
        }
        (*itr)->m_fExit = true;
    }
    SemPost(&m_hSemThread);

    int iTime = 0;
    while ((int)m_lstThread.size() > iInThreadCount)
    {
        HV_Sleep(100);
        if (!(++iTime)%10)
        {
            HV_Trace(5, "waiting all thread exit...\n");
        }

        if (iTime > 50)
        {
            break;
        }
    }

    HV_Trace(5, "all threads have exited\n");
}

HRESULT CHvThreadBase::Start(void* pvParam)
{
    m_pvParam = pvParam;
    m_fExit = FALSE;

    if (CreateThread(
                &m_hThread,
                m_iPriotiry,
                m_iStackSize,
                m_iStackLocation,
                ThreadFunction,
                this
            ) == 0)
    {
        m_fIsRun = true;
        m_hrLastStatus = S_OK;
    }
    else
    {
        m_hrLastStatus = E_FAIL;
    }

    HV_Trace(5,"Start %s %s, tid=%d, pid=%d\n",GetName(),m_hrLastStatus==S_OK?"OK":"FAIL",m_hThread,getpid());

    return m_hrLastStatus;
}

void* CHvThreadBase::ThreadFunction(LPVOID lpParameter)
{
    CHvThreadBase *pThis = (CHvThreadBase*)lpParameter;
    pThis->Run(pThis->m_pvParam);
    pThis->m_hrLastStatus = S_FALSE;
    SemPost(&(pThis->m_hSemJoin), 1);
    HV_Trace(5,"Thread %s exit\n",pThis->GetName());
    return 0;
}

HRESULT CHvThreadBase::Stop(int nTimeOut)
{
    HRESULT hr = S_OK;
    m_fExit = TRUE;

    HV_Trace(5,"Stop %s\n", GetName());

    if (m_fIsRun)
    {
        if (nTimeOut == -1)
        {
            SemPend(&m_hSemJoin);
            ThreadJoin(m_hThread);
        }
        else
#if RUN_PLATFORM == PLATFORM_WINDOWS //Win32
        {
            if (WAIT_TIMEOUT  == WaitForSingleObject(NULL,nTimeOut))
            {
                ExitThread(-1);
            }
        }
#else
        {
            //TODO: 增加实现
            hr = E_NOTIMPL;
        }
#endif
    }
    m_fIsRun = false;

    return hr;
}

HRESULT CHvThreadBase::GetLastStatus()
{
    return m_hrLastStatus;
}
