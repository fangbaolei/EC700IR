// 该文件编码必须是WINDOWS-936格式
#include <stdio.h>
#include <sys/mman.h>
#include <sys/statfs.h>
#include <unistd.h>
#include <dirent.h>
#include "hvutils.h"
#include "misc.h"
#include "FixFileStream.h"
#include "HvUtilsClass.h"

#define DATA_DIR "data"
#define FILE_DIR "file"
#define FIX_FILE ".fix.dat"

CFixFileStream::CFixFileStream()
{
    m_iStackSize = 32 * 1024;
    pthread_mutexattr_t attr;
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&m_mutex, &attr);
    m_fInitialize = false;
    strcpy(m_szDirectory, "");
    memset(&m_fh, 0, sizeof(m_fh));
}

CFixFileStream::~CFixFileStream()
{
    UnInitialize();
    pthread_mutex_destroy(&m_mutex);
}

HRESULT CFixFileStream::Run( void* pvParam )
{
    DWORD32 dwLastTime = 0;
    char szLastFile[255] = "", szCmd[255] = "";
    while (!m_fExit)
    {
        string strCmd = m_queDeleteFile.RemoveHead(1000);
        if (!strCmd.empty())
        {
            //若目录更换，则删除前一个目录
            if (!dwLastTime || NULL == strstr(strCmd.c_str(), szLastFile))
            {
                if (dwLastTime)
                {
                    sprintf(szCmd, "RM %s 1", szLastFile);
                    HV_System(szCmd);
                }
                //保存其上一层目录
                strcpy(szLastFile, strCmd.c_str());
                char *s = szLastFile;
                char *e = szLastFile + strlen(szLastFile) - 1;
                while (e != s)
                {
                    if (*e == '/')
                    {
                        *e = '\0';
                        break;
                    }
                    e--;
                }
            }
            //第一次，则删除比此文件旧的目录
            if (!dwLastTime || GetSystemTick() - dwLastTime > 3600000)
            {
                sprintf(szCmd, "RM %s 0", strCmd.c_str());
                HV_System(szCmd);
                dwLastTime = GetSystemTick();
            }
            //删除当前目录
            sprintf(szCmd, "RM %s 1", strCmd.c_str());
            HV_System(szCmd);

        }
    }
    return S_OK;
}

bool CFixFileStream::Initialize(const char* szDirecory, long long iTotalSize, int iFileSize)
{
    if (!szDirecory || !strlen(szDirecory))
    {
        return false;
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
        HV_Trace(5, "%s", strerror(errno));
        return false;
    }

    struct statfs fs;
    if (statfs(szDirecory, &fs))
    {
        HV_Trace(5, "%s", strerror(errno));
        return false;
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
        return false;
    }
    char szPath[255];
    sprintf(szPath, "%s/%s", m_szDirectory, FIX_FILE);
    if (access(szPath, 0))
    {
        HV_Trace(5, "create file %s", szPath);
        int fd = open(szPath, O_CREAT|O_RDWR);
        if (fd >= 0)
        {
            m_fh.iCount = llTotal/iFileSize;
            m_fh.iFileSize = iFileSize;
            m_fh.iIndex = 0;
            pwrite(fd, &m_fh, sizeof(m_fh), 0);
            close(fd);
        }
    }
    else
    {
        HV_Trace(5, "open file %s", szPath);
        int fd = open(szPath, O_RDWR);
        if (fd >= 0)
        {
            pread(fd, &m_fh, sizeof(m_fh), 0);
            /*
            m_fh.iCount = llTotal/iFileSize;
            m_fh.iFileSize = iFileSize;
            */
            close(fd);
        }
    }
    sprintf(szPath, "%s/%s/", m_szDirectory, DATA_DIR);
    MakeSureDirectoryPathExists(szPath);
    HV_Trace(5, "data:%s", szPath);
    sprintf(szPath, "%s/%s/", m_szDirectory, FILE_DIR);
    MakeSureDirectoryPathExists(szPath);
    HV_Trace(5, "file:%s", szPath);
    HV_Trace(5, "Initailize fix file stream success. disk total size = %dG, size of one file = %d, total file count = %d", int(llTotal/1024/1024/1024), m_fh.iFileSize, m_fh.iCount);
    m_fInitialize = true;
    Start(NULL);
    return true;
}

void CFixFileStream::UnInitialize(void)
{
    if (m_fInitialize)
    {
        char szPath[255];
        sprintf(szPath, "%s/%s", m_szDirectory, FIX_FILE);
        int fd = open(szPath, O_RDWR);
        if (fd >= 0)
        {
            pwrite(fd, &m_fh, sizeof(m_fh), 0);
            close(fd);
        }
        m_fInitialize = false;
    }
}

bool CFixFileStream::Write(const char* szFileName, void* buffer, int iSize)
{
    if (!IsValid())
    {
        return false;
    }

    char szDstFile[255], szPath[255], szTemp[255];
    int i = 0;

    pthread_mutex_lock(&m_mutex);
    sprintf(szDstFile, "%s/%s/%s/", m_szDirectory, FILE_DIR, szFileName);
    MakeSureDirectoryPathExists(szDstFile);

    int index = m_fh.iIndex;
    char * pBuffer = (char *)buffer;
    int iTryTime = 3;
    int iDir1, iDir2, iDir3;
    while (iSize > 0)
    {
        iDir1 = m_fh.iIndex / 1024;
        iDir2 = m_fh.iIndex % 1024;
        iDir3 = iDir2 % 100;
        iDir2 /= 100;

        sprintf(szPath, "%s/%s/%d/%d/%d.dat", m_szDirectory, DATA_DIR, iDir1, iDir2, iDir3);
        MakeSureDirectoryPathExists(szPath);

        bool fExist = !access(szPath, 0);
        int fd = open(szPath, O_CREAT|O_RDWR);
        if (fd >= 0)
        {
            //指定文件大小
            if (fExist)
            {
                //判断文件是否存在，存在则删除文件
                szTemp[0] = '\0';
                pread(fd, szTemp, 255, 0);
                szTemp[254] = '\0';
                if (!access(szTemp, 0))
                {
                    if (szTemp[strlen(szTemp) - 1] == '/')
                    {
                        szTemp[strlen(szTemp) - 1] = '\0';
                    }
                    string str = szTemp;
                    if (m_queDeleteFile.IsFull())
                    {
                        m_queDeleteFile.RemoveHead();
                    }
                    m_queDeleteFile.AddTail(str);
                }
            }
            ftruncate(fd, m_fh.iFileSize);
            //先记录索引
            m_fh.iIndex = (m_fh.iIndex + 1)%m_fh.iCount;
            sprintf(szTemp, "%s/%s", m_szDirectory, FIX_FILE);
            int fdTemp = open(szTemp, O_RDWR);
            if (fdTemp >= 0)
            {
                pwrite(fdTemp, &m_fh, sizeof(m_fh), 0);
                close(fdTemp);
            }
            //记录大文件名
            pwrite(fd, szDstFile, strlen(szDstFile) + 1, 0);
            //实际的内存数据
            int s = m_fh.iFileSize - 255;
            if (s > iSize)
            {
                s = iSize;
            }
            if (pBuffer)
            {
                pwrite(fd, pBuffer, s, 255);
                //计算剩余的空间
                pBuffer += s;
                iSize -= s;
            }
            //关闭文件
            close(fd);
            //创建连接
            sprintf(szTemp, "%s%d.dat", szDstFile, i++);
            if (!access(szTemp, 0))
            {
                unlink(szTemp);
            }

            MakeSureDirectoryPathExists(szPath);
            MakeSureDirectoryPathExists(szTemp);

            if (!link(szPath, szTemp))
            {
                HV_Trace(5, "link %s-->%s success", szTemp, szPath);
            }
            else
            {
                HV_Trace(5, "link %s-->%s failed, error string:%s", szTemp, szPath, strerror(errno));
                break;
            }
        }
        else if (errno == 28 && iTryTime-- > 0)
        {
            if (m_fh.iCount > m_fh.iIndex + 1)
            {
                m_fh.iCount = m_fh.iIndex - 1;
            }
            else
            {
                m_fh.iCount--;
            }
            m_fh.iIndex = (m_fh.iIndex + 1)%m_fh.iCount;
            HV_Trace(5, "保存文件%s错误,错误代码:[%d]%s,再次尝试保存[idx=%d,count=%d]", szPath, errno, strerror(errno), m_fh.iIndex, m_fh.iCount);
        }
        else
        {
            HV_Trace(5, "保存文件%s错误,错误代码:[%d]%s", szPath, errno, strerror(errno));
            break;
        }
    }
    pthread_mutex_unlock(&m_mutex);
    if (index != m_fh.iIndex)
    {
        g_cHddOpThread.SetStatusCodeString("正常");
    }
    return index != m_fh.iIndex;
}

bool CFixFileStream::Read(const char* szFileName, void* buffer, int iSize)
{
    if (!IsValid())
    {
        return false;
    }

    char szPath[255];
    sprintf(szPath, "%s/%s/%s", m_szDirectory, FILE_DIR, szFileName);

    pthread_mutex_lock(&m_mutex);
    struct stat buf;
    bool fOk = false;
    char * pBuffer = (char *)buffer;
    if (!access(szPath, 0) && !stat(szPath, &buf) && S_ISDIR(buf.st_mode))
    {
        fOk = true;
        char szTemp[255];
        int i = 0;
        while (iSize > 0)
        {
            sprintf(szTemp, "%s/%d.dat", szPath, i);
            HV_Trace(5, "read file %s", szTemp);
            if (access(szTemp, 0) || stat(szTemp, &buf))
            {
                break;
            }
            int s = iSize;
            if (s > buf.st_size - 255)
            {
                s = buf.st_size - 255;
            }
            int fd = open(szTemp, O_RDONLY);
            if (fd >= 0)
            {
                //先读255个字节判断当前文件是否合理
                pread(fd, szPath, 255, 0);
                szPath[254] = '\0';
                if (!strstr(szTemp, szPath))
                {
                    fOk = false;
                    break;
                }
                else
                {
                    //在读后续的数据
                    pread(fd, pBuffer, s, 255);
                    pBuffer += s;
                    iSize -= s;
                }
                close(fd);
            }
            else
            {
                HV_Trace(5, "读取文件%s错误,错误代码:%s", szTemp, strerror(errno));
                break;
            }
            i++;
        }
    }
    pthread_mutex_unlock(&m_mutex);
    return fOk;
}

int CFixFileStream::GetFileCount(const char* szPath)
{
    char szTemp[255];
    if (szPath[0] != '/')
    {
        sprintf(szTemp, "%s/%s/%s", m_szDirectory, FILE_DIR, szPath);
    }
    else
    {
        strcpy(szTemp, szPath);
    }
    int iCount = 0;
    struct stat buf;

    if (!access(szTemp, 0) && !stat(szTemp, &buf) && S_ISDIR(buf.st_mode))
    {
        DIR* pDir = opendir(szTemp);
        struct dirent* ptr;
        while (NULL != (ptr = readdir(pDir)))
        {
            if (strcmp(ptr->d_name, ".") && strcmp(ptr->d_name, "..") && S_ISDIR(buf.st_mode))
            {
                iCount++;
            }
        }
        closedir(pDir);
    }
    return iCount;
}
