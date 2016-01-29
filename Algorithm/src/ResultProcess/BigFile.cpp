#include <stdio.h>
#include <sys/mman.h>
#include <sys/statfs.h>
#include <unistd.h>
#include <dirent.h>
#include "hvutils.h"
#include "misc.h"
#include "BigFile.h"
#define  INDEX_FILE ".index.dat"
#define  INDEX_FILE_BAK ".index.dat.bak"

#define INDEX_SIZE 1048576
#define COMMIT_COUNT 10
#define MASTER_COMMIT_COUNT 5
#define COMMIT_INTERVAL 10000

#define MIN_RESERVE_SIZE 1
#define MAX_RESERVE_SIZE 4

#define BIG_FILE_DEBUG 0

CBigFile::CBigFile()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&m_mutex, &attr);
    m_fInitialize = false;
    m_dwOneFileSize = 0;
    m_dwTotalFileCount = 0;
    m_pIndexBuffer = new BYTE8[4 + INDEX_SIZE];
    m_dwCommitCount = 0;
    m_dwMasteCommitCount = 0;
    m_dwLastCommitTime = 0;
    m_fUpdate = false;
    m_iCommitCount = 0;
}

CBigFile::~CBigFile()
{
    UnInitialize();
    pthread_mutex_destroy(&m_mutex);
    if (m_pIndexBuffer)
    {
        delete []m_pIndexBuffer;
    }
}

HRESULT CBigFile::Initialize(const char* szDirecory, long long iTotalSize, int iFileSize, bool fRecord)
{
    if (!szDirecory || !strlen(szDirecory))
    {
        HV_Trace(5, "directory is empty.");
        return E_FAIL;
    }

    UnInitialize();
    strcpy(m_szDirectory, szDirecory);

    if (m_szDirectory[strlen(m_szDirectory) - 1] != '/')
    {
        strcat(m_szDirectory, "/");
    }
    MakeSureDirectoryPathExists(m_szDirectory);

    if (m_szDirectory[strlen(m_szDirectory) - 1] == '/')
    {
        m_szDirectory[strlen(m_szDirectory) - 1] = '\0';
    }

    if (access(szDirecory, 0))
    {
        HV_Trace(5, "directory %s access fail, error string:%s", szDirecory, strerror(errno));
        return E_FAIL;
    }

    //if (!IsValid())
    {
        struct statfs fs;
        if (statfs(szDirecory, &fs))
        {
            HV_Trace(5, "directory %s statfs fail, error string:%s", szDirecory, strerror(errno));
            return E_FAIL;
        }

        DWORD64 llTotal = (long long)fs.f_bsize * (long long)fs.f_blocks;
        if (iTotalSize > 0 && iTotalSize < llTotal)
        {
            llTotal = iTotalSize;
        }

        long long llFree = (llTotal/20)/1024/1024/1024;
        if (llFree < MIN_RESERVE_SIZE)
        {
            llFree = MIN_RESERVE_SIZE;
        }
        else if (llFree > MAX_RESERVE_SIZE)
        {
            llFree = MAX_RESERVE_SIZE;
        }
        llFree *= 1024*1024*1024;
        llTotal -= llFree;

        if (llTotal < 1024*1024*1024)
        {
            HV_Trace(5, "disk %s is too small, initailzie failed", szDirecory);
            return E_FAIL;
        }
        m_dwOneFileSize = iFileSize;
        m_dwTotalFileCount = llTotal/iFileSize;
        HV_Trace(5, "Path:%s,size:%dG, one file size:%dK,count:%d", m_szDirectory, int(llTotal/1024/1024/1024), m_dwOneFileSize/1024, m_dwTotalFileCount);
        CFileQueue::Initialize(m_dwTotalFileCount, m_dwOneFileSize, fRecord ? FQ_TYPE_RESULT : FQ_TYPE_VIDEO);
    }

    char szPath[255];
    sprintf(szPath, "%s/%s", m_szDirectory, INDEX_FILE);
    if (!access(szPath, 0))
    {
        int fd = open(szPath, O_RDONLY);
        if (fd >= 0)
        {
            HV_Trace(5, "open file:%s", szPath);
            pread(fd, m_pIndexBuffer, INDEX_SIZE + 4, 0);
            close(fd);

            CFastCrc32 cFastCrc;
            DWORD32 dwCrc32;
            memcpy(&dwCrc32, m_pIndexBuffer, 4);
            bool fOk = false;
            if (dwCrc32 != cFastCrc.CalcCrc32(0, m_pIndexBuffer + 4, INDEX_SIZE))
            {
                HV_Trace(5, "crc32 check failed in %s, use %s", INDEX_FILE, INDEX_FILE_BAK);
                sprintf(szPath, "%s/%s", m_szDirectory, INDEX_FILE_BAK);
                fd = open(szPath, O_RDONLY);
                if (fd >= 0)
                {
                    HV_Trace(5, "open file:%s", szPath);
                    pread(fd, m_pIndexBuffer, INDEX_SIZE + 4, 0);
                    close(fd);

                    memcpy(&dwCrc32, m_pIndexBuffer, 4);
                    if (dwCrc32 == cFastCrc.CalcCrc32(0, m_pIndexBuffer + 4, INDEX_SIZE))
                    {
                        fOk = true;
                    }
                }
            }
            else
            {
                fOk = true;
            }
            if (fOk)
            {
                m_fInitialize = (S_OK == CFileQueue::Load(m_pIndexBuffer + 4, INDEX_SIZE));
            }
        }
    }

    if (!IsValid())
    {
        m_fInitialize = (S_OK == CFileQueue::Load(NULL, 0));
    }
    HV_Trace(5, "initialize %s", IsValid() ? "ok" : "failed");

    if (IsValid())
    {
#if BIG_FILE_DEBUG == 1
        CFileQueue::Print();
#endif
    }
    return IsValid() ? S_OK : E_FAIL;
}

void CBigFile::UnInitialize(void)
{
    m_fInitialize = false;
}

HRESULT CBigFile::Write(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay, DWORD32 dwHour, DWORD32 dwMinute, DWORD32 dwSecond, void* pBuffer, int iSize)
{
    if (!IsValid())
    {
        return E_NOTIMPL;
    }

    pthread_mutex_lock(&m_mutex);

    DWORD32 dwCarID = 0, dwStartIndex = 0, dwEndIndex = 0;
    if (S_OK == CFileQueue::Write(dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, iSize, dwCarID, dwStartIndex, dwEndIndex))
    {
        if (dwEndIndex < dwStartIndex)
        {
            dwEndIndex += m_dwTotalFileCount;
        }

        char szPath[255];
        int s;
        char *buf = (char *)pBuffer;
        for (int i = dwStartIndex; i <= dwEndIndex && iSize > 0; i++)
        {
            int index = (i % m_dwTotalFileCount);
            sprintf(szPath, "%s/%d/%d/%d.dat", m_szDirectory, (index/1000), ((index%1000)/100), ((index%1000)%100));
            HV_Trace(5, "start index:%d, end index:%d, now index:%d, write file:%s", dwStartIndex, (dwEndIndex % m_dwTotalFileCount), i, szPath);

            MakeSureDirectoryPathExists(szPath);
            int fd = open(szPath, O_CREAT|O_RDWR);
            if (fd >= 0)
            {
                ftruncate(fd, m_dwOneFileSize);
                int s = iSize;
                if (s > m_dwOneFileSize)
                {
                    s = m_dwOneFileSize;
                }
                pwrite(fd, buf, s, 0);
                close(fd);
                iSize -= s;
                buf += s;
            }
            else
            {
                pthread_mutex_unlock(&m_mutex);
                return E_FAIL;
            }
            m_fUpdate = true;
        }
        Commit(false, true);
        pthread_mutex_unlock(&m_mutex);
        return S_OK;
    }
    pthread_mutex_unlock(&m_mutex);
    return E_FAIL;
}

HRESULT CBigFile::Read(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay, DWORD32 dwHour, DWORD32 dwMinute, DWORD32 dwSecond, DWORD32 dwCarID, void* buffer, int iSize)
{
    if (!IsValid())
    {
        return E_NOTIMPL;
    }

    pthread_mutex_lock(&m_mutex);
    DWORD32 dwStartIndex = 0, dwEndIndex = 0;
    if (S_OK == CFileQueue::Read(dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, dwCarID, dwStartIndex, dwEndIndex))
    {
        HV_Trace(5, "read file:%04d-%02d-%02d %02d:%02d:%02d(%d)[%d-%d]", dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond, dwCarID, dwStartIndex, dwEndIndex);
        if (dwEndIndex < dwStartIndex)
        {
            dwEndIndex += m_dwTotalFileCount;
        }

        char szPath[255];
        int s;
        char *buf = (char *)buffer;
        for (int i = dwStartIndex; i <= dwEndIndex && iSize > 0; i++)
        {
            int index = (i % m_dwTotalFileCount);
            sprintf(szPath, "%s/%d/%d/%d.dat", m_szDirectory, (index/1000), ((index%1000)/100), ((index%1000)%100));

            int fd = open(szPath, O_RDONLY);
            if (fd >= 0)
            {
                HV_Trace(5, "start index:%d, end index:%d, now index:%d, read file:%s", dwStartIndex, (dwEndIndex % m_dwTotalFileCount), i, szPath);
                int s = iSize;
                if (s > m_dwOneFileSize)
                {
                    s = m_dwOneFileSize;
                }
                pread(fd, buf, s, 0);
                close(fd);
                iSize -= s;
                buf += s;
            }
            else
            {
                pthread_mutex_unlock(&m_mutex);
                return dwCarID < GetFileCount(dwYear, dwMonth, dwDay, dwHour) ? E_FAIL : E_RECORD_NONE;
            }
        }
        pthread_mutex_unlock(&m_mutex);
        return S_OK;
    }
    pthread_mutex_unlock(&m_mutex);
    return dwCarID < GetFileCount(dwYear, dwMonth, dwDay, dwHour) ? E_FAIL : E_RECORD_NONE;
}

DWORD32 CBigFile::GetFileCount(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay, DWORD32 dwHour)
{
    pthread_mutex_lock(&m_mutex);
    DWORD32 dwCnt = CFileQueue::GetFileCount(dwYear, dwMonth, dwDay, dwHour);
    pthread_mutex_unlock(&m_mutex);
    return dwCnt;
}

HRESULT CBigFile::Commit(bool fForce, bool fIsRecord)
{
    pthread_mutex_lock(&m_mutex);
    if (!IsValid())
    {
        pthread_mutex_unlock(&m_mutex);
        return E_NOTIMPL;
    }

    if (m_fUpdate)
    {
        if( fIsRecord )
        {
            m_dwCommitCount = (m_dwCommitCount + 1)% COMMIT_COUNT;
        }
        if ((!m_dwCommitCount || GetSystemTick() - m_dwLastCommitTime > COMMIT_INTERVAL || fForce)
                && S_OK == CFileQueue::Commit(m_pIndexBuffer + 4, INDEX_SIZE))
        {
            HV_Trace(5, "<BigFile> m_dwCommitCount:%d, m_dwLastCommitTime:%d, now:%d,fForce:%d", m_dwCommitCount, m_dwLastCommitTime, GetSystemTick(), fForce);

            m_dwCommitCount = 0;
            m_dwLastCommitTime = GetSystemTick();

            m_fUpdate = false;
            CFastCrc32 cFastCrc;
            DWORD32 dwCrc32 = cFastCrc.CalcCrc32(0, m_pIndexBuffer + 4, INDEX_SIZE);
            memcpy(m_pIndexBuffer, &dwCrc32, 4);

            char szPath[255];
            sprintf(szPath, "%s/%s", m_szDirectory, INDEX_FILE);
            int fd = open(szPath, O_CREAT|O_RDWR);
            if (fd >= 0)
            {
                HV_Trace(5, "save index to %s", szPath);
                pwrite(fd, m_pIndexBuffer, INDEX_SIZE + 4, 0);
                close(fd);
            }
            m_dwMasteCommitCount = (m_dwMasteCommitCount + 1)% MASTER_COMMIT_COUNT;
            if (fForce || !m_dwMasteCommitCount)
            {
                sprintf(szPath, "%s/%s", m_szDirectory, INDEX_FILE_BAK);
                fd = open(szPath, O_CREAT|O_RDWR);
                if (fd >= 0)
                {
                    HV_Trace(5, "save index to %s", szPath);
                    pwrite(fd, m_pIndexBuffer, INDEX_SIZE + 4, 0);
                    close(fd);
                }
            }

            m_iCommitCount++;
            if( m_iCommitCount > 30 )
            {
                m_iCommitCount = 0;
                Print();
            }
            pthread_mutex_unlock(&m_mutex);
            return S_OK;
        }
    }
    pthread_mutex_unlock(&m_mutex);
    return E_FAIL;
}
