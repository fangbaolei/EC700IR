#include "SerialBase.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <linux/types.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include "swdrv.h"

//#define DEBUG_SERIAL
#ifdef	DEBUG_SERIAL
#define PRINTF printf
#else
#define PRINTF(...)
#endif


// class CSerialBase

pthread_mutex_t CSerialBase::s_SerialMutex = PTHREAD_MUTEX_INITIALIZER;
std::vector<SerialPort*> CSerialBase::s_SerialPtrVct;

CSerialBase::CSerialBase()
{
    m_pSerialPort = NULL;
}

CSerialBase::~CSerialBase()
{
    while (0 != Close())  continue;
}

int CSerialBase::SetAttr(int nBaudrate,
                         int nDatasize,
                         int nParity,
                         int iStopBit)
{
    fcntl(m_pSerialPort->fd, F_SETFL, 0);

    // 设置串口属性
    if (0 != SwSerialSetAttr(m_pSerialPort->fd, nBaudrate,
                        nDatasize, nParity, iStopBit) )
    {
        PRINTF("%d set attribute error\n", m_pSerialPort->fd);
        return -1;
    }

    return 0;
}

// 打开串口，并锁定.若处于串口加锁状态，则等待
// 参数：
//          szDev 		要打开的通道号，如/dev/ttyS0
// 			nTimeOMs    超时毫秒数
// 返回：
//          0		成功
//			-1		打开设备失败
int CSerialBase::Open(const char* szDev)
{
    int hr = -1;
    for (int i=0; i<3; i++)
    {
        if (0 == pthread_mutex_trylock(&s_SerialMutex))
        {
            hr = 0;
            break;
        }
        sleep(1);
    }
    if (hr != 0)
    {
        PRINTF("pthread_mutex_timedlock failed, returned: %d\n", hr);
        return -1;
    }

    for (int i=0; i<(int)s_SerialPtrVct.size(); i++)
    {
        if (strcmp(s_SerialPtrVct[i]->szDev, szDev) == 0)
        {
            m_pSerialPort = s_SerialPtrVct[i];
            m_pSerialPort->iOpenCount++;
        }
    }

    if (m_pSerialPort == NULL)
    {
        // no open before
        int fd = SwSerialOpen(szDev);
        if (fd < 0)
        {
            PRINTF("open %s error!\n", szDev);
            pthread_mutex_unlock(&s_SerialMutex);
            return -1;
        }

        SerialPort* pPort = new SerialPort;
        strcpy(pPort->szDev, szDev);
        pPort->fd = fd;
        pPort->iOpenCount = 1;
        pPort->iLockCount = 0;

        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&pPort->mutex, &attr);
        s_SerialPtrVct.push_back(pPort);
        m_pSerialPort = pPort;
    }
    PRINTF("open dev: %s fd:%d\n", szDev, m_pSerialPort->fd);
    pthread_mutex_unlock(&s_SerialMutex);

    return 0;
}

int CSerialBase::Close()
{
    if ((m_pSerialPort != NULL) && (m_pSerialPort->fd != 0))
    {
        if (m_pSerialPort->iOpenCount != 1)
        {
            m_pSerialPort->iOpenCount--;
        }
        else
        {
            int hr = -1;
            for (int i=0; i<3; i++)
            {
                if (0 == pthread_mutex_trylock(&s_SerialMutex))
                {
                    hr = 0;
                    break;
                }
                sleep(1);
            }

            if (hr != 0)
            {
                PRINTF("Close pthread_mutex_timedlock failed, returned: %d\n", hr);
                return -1;
            }

            std::vector<SerialPort*>::iterator iter;
            for ( iter = s_SerialPtrVct.begin( ) ; iter != s_SerialPtrVct.end( ) ; iter++ )
            {
                if (*iter == m_pSerialPort)
                    break;
            }

            if (iter != s_SerialPtrVct.end())
                s_SerialPtrVct.erase(iter);

            SwSerialClose(m_pSerialPort->fd);
            delete m_pSerialPort;
            m_pSerialPort = NULL;
            pthread_mutex_unlock(&s_SerialMutex);
        }
    }

    return 0;
}

int CSerialBase::Lock(unsigned long nTimeOMs)
{
    int hr = -1;

    if (nTimeOMs == 0)
    {
        hr = pthread_mutex_trylock(&m_pSerialPort->mutex);
    }
    else
    {
        for (unsigned long i=0; i<nTimeOMs; i++)
        {
            if (0 == pthread_mutex_trylock(&m_pSerialPort->mutex))
            {
                hr = 0;
                break;
            }
            usleep(1000);
        }
    }

    if (hr != 0)
    {
        PRINTF("Lock pthread_mutex_timedlock failed, returned: %d\n", hr);
        return -1;
    }

    m_pSerialPort->iLockCount++;
    return 0;
}

bool CSerialBase::IsLock()
{
    return (m_pSerialPort->iLockCount > 0) ? true : false;
}

int CSerialBase::UnLock()
{
    if (m_pSerialPort->iLockCount > 0)
    {
        m_pSerialPort->iLockCount--;
        pthread_mutex_unlock(&m_pSerialPort->mutex);
    }
    return 0;
}

int CSerialBase::Send(unsigned char* msg, int nLen)
{
    if (m_pSerialPort == NULL)
        return -1;

    int nSendCount = 0;
    int nSend = 0;

    while (nSendCount != nLen)
    {
        nSend = SwSerialSend(m_pSerialPort->fd, msg+nSendCount, nLen-nSendCount);
        if ((nSend == 0) || (nSend == -1))
            break;

        nSendCount += nSend;
    }
    return nSend;
}

int CSerialBase::Recv(unsigned char* msg, int nLen, unsigned long nTimeOMs)
{
    if (m_pSerialPort == NULL)
        return -1;

    int flags = 0;
    flags = fcntl(m_pSerialPort->fd, F_GETFL, 0);
    fcntl(m_pSerialPort->fd, F_SETFL, O_NONBLOCK|flags);

    int nReadCount = 0;
    fd_set rfds;

    struct timeval tv;
    tv.tv_sec = nTimeOMs / 1000;
    tv.tv_usec = (nTimeOMs % 1000) * 1000;

    while (1)
    {
        FD_ZERO(&rfds);
        FD_SET(m_pSerialPort->fd, &rfds);
        // select等待超时
        int retval = select(m_pSerialPort->fd+1, &rfds, NULL, NULL, &tv);
        if (retval ==-1)
        {
            PRINTF("select err");
            break;
        }
        else if (retval)
        {
            if (!FD_ISSET(m_pSerialPort->fd, &rfds))
            {
                continue;
            }

            int nRead;
            while (nReadCount != nLen)
            {
                nRead = SwSerialRecv(m_pSerialPort->fd, msg+nReadCount, nLen-nReadCount);

                if ((nRead == 0) || (nRead == -1))
                {
                    break;
                }
                nReadCount += nRead;
            }

            break;
        }
        else
        {
            break;
        }
    }
    return nReadCount;
}

void CSerialBase::Flush()
{
    SwSerialFlush(m_pSerialPort->fd, SW_MODE_SERIAL_IOFLUSH);
}

////////////////////////////////////////////////////////////
