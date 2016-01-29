// 该文件编码格式必须为WINDOWS-936格式

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "hvutils.h"
#include "hvthread.h"
#include <deque>

using namespace std;

template<class T, int MAX_COUNT>
class CQueue
{
public:
    CQueue()
    {
        CreateSemaphore(&m_hSemQueCount, 0, MAX_COUNT);
        CreateSemaphore(&m_hSemQueCtrl, 1, 1);
    }

    virtual ~CQueue()
    {
        DestroySemaphore(&m_hSemQueCount);
        DestroySemaphore(&m_hSemQueCtrl);
    }

public:
    int GetSize(void)
    {
        CSemLock sLock(&m_hSemQueCtrl);
        return m_lstQueue.size();
    }

    bool IsEmpty(void)
    {
        CSemLock sLock(&m_hSemQueCtrl);
        return m_lstQueue.empty();
    }

    bool IsFull(void)
    {
        return GetSize() == MAX_COUNT;
    }

    bool AddTail(T& item, bool bWait = false)
    {
        if (bWait == false)
        {
            CSemLock sLock(&m_hSemQueCtrl);
            if (0 == SemPost(&m_hSemQueCount))
            {
                m_lstQueue.push_back(item);
                return true;
            }
        }
        else
        {
            // 不丢帧方式不能在临界区内Post信号量，否则会造成RemoveHead时卡住
            SemPend(&m_hSemQueCtrl);
            m_lstQueue.push_back(item);
            SemPost(&m_hSemQueCtrl);
            if (0 == SemPost(&m_hSemQueCount, 1))
            {
                return true;
            }
            SemPend(&m_hSemQueCtrl);
            m_lstQueue.pop_front();
            SemPost(&m_hSemQueCtrl);
        }
        return false;
    }

    T RemoveHead(int iTimeOut = -1)
    {
        T t;

        if (0 == SemPend(&m_hSemQueCount, iTimeOut))
        {
            CSemLock sLock(&m_hSemQueCtrl);
            if (!m_lstQueue.empty())
            {
                t = m_lstQueue.front();
                m_lstQueue.pop_front();
            }
        }

        return t;
    }

private:
    HV_SEM_HANDLE m_hSemQueCount;
    HV_SEM_HANDLE m_hSemQueCtrl;
    deque<T> m_lstQueue;
};

#endif // __QUEUE_H__
