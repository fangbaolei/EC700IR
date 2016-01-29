#include "HvDebugStateInfo.h"
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "hvtarget_ARM.h"

// 超始地址
#define INFO_START_ADDR         DEV_STAT_LOG_START_ADDR
// 总的 FLASH LOG 空间容量
#define INFO_ALL_CAPACITY       DEV_STAT_LOG_SIZE
// 结束地址
#define INFO_END_ADDR           (INFO_START_ADDR + INFO_ALL_CAPACITY)

// 要写入数据的起始地址
#define INFO_WRITE_NUM_ADDR     INFO_START_ADDR

// 标志位
#define INFO_FORMAT_FLAG_OFFSET     125
#define FORMAT_FALG1                0xF1
#define FORMAT_FALG2                0x1F

// 数据起始地址偏移量
#define INFO_DATA_OFFSET       127  // 最后一位\0
// 数据起始地址
#define INFO_DATA_START_ADDR   (INFO_START_ADDR + INFO_DATA_OFFSET)
// 可用的数据容量
#define INFO_CAPACITY          (INFO_ALL_CAPACITY - INFO_DATA_OFFSET)

#define INFO_LINE_LEN          128

#define INFO_LINE_NUM          (INFO_CAPACITY / INFO_LINE_LEN)

#define TIME_SEG_FSET       0
#define TIME_SEG_LEN        12
#define FILE_SEG_FSET       (TIME_SEG_FSET+TIME_SEG_LEN)
#define FILE_SEG_LEN        44
#define LINE_SEG_FSET       (FILE_SEG_FSET+FILE_SEG_LEN)
#define LINE_SEG_LEN        8
#define INFO_SEG_FSET       (LINE_SEG_FSET+LINE_SEG_LEN)
#define INFO_SEG_LEN        (INFO_LINE_LEN-INFO_SEG_FSET)


#define eeprom_read(addr, data, len)  EEPROM_Read((DWORD32)(addr), (BYTE8*)(data), (DWORD32)(len))
#define eeprom_write(addr, data, len)  EEPROM_Write((DWORD32)(addr), (BYTE8*)(data), (DWORD32)(len))

static char g_rwbuf[256];

static volatile pthread_mutex_t* g_pMutex = NULL;
static volatile bool g_fCreateLock = false;

static pthread_mutex_t* GetLockMutex()
{
    if (g_fCreateLock)
        usleep(1000);

    if (g_pMutex == NULL)
    {
        g_fCreateLock = true;
        g_pMutex = new pthread_mutex_t;
        if (g_pMutex == NULL)
            return NULL;
        pthread_mutex_init((pthread_mutex_t*)g_pMutex, NULL);

        g_fCreateLock = false;
    }

    return (pthread_mutex_t*)g_pMutex;
}

// 格式化存储空间
int HvDebugStateInfoFormat()
{
    pthread_mutex_t* pLockMutex = GetLockMutex();
    if (pLockMutex == NULL)
        return -1;
    pthread_mutex_lock(pLockMutex);

    char buf[INFO_LINE_LEN];
    memset(buf, 0, INFO_LINE_LEN);
    // 全部清0
    int n = INFO_ALL_CAPACITY / INFO_LINE_LEN;
    for (int i=0; i<n; i++)
    {
        if (0 > eeprom_write(INFO_START_ADDR+INFO_LINE_LEN*i, (char*)buf, INFO_LINE_LEN))
        {
            pthread_mutex_unlock(pLockMutex);
            return -1;
        }
    }

    if (0 > eeprom_write(INFO_START_ADDR+INFO_LINE_LEN*n, (char*)buf, INFO_ALL_CAPACITY-INFO_LINE_LEN*n))
    {
        pthread_mutex_unlock(pLockMutex);
        return -1;
    }

    // write fmt flag
    buf[0] = FORMAT_FALG1;
    buf[1] = FORMAT_FALG2;
    if (0 > eeprom_write(INFO_START_ADDR+INFO_FORMAT_FLAG_OFFSET, (char*)buf, 2))
    {
        pthread_mutex_unlock(pLockMutex);
        return -1;
    }

    pthread_mutex_unlock(pLockMutex);
    return 0;
}

// 判断是否已格式化
bool HvDebugStateInfoIsFormat()
{
    pthread_mutex_t* pLockMutex = GetLockMutex();
    if (pLockMutex == NULL)
        return -1;
    pthread_mutex_lock(pLockMutex);

    bool ret = false;

    char szBuf[2];

    if ((eeprom_read(INFO_START_ADDR+INFO_FORMAT_FLAG_OFFSET, szBuf, 2) >= 0)
        && (szBuf[0] == FORMAT_FALG1)
        && (szBuf[1] == FORMAT_FALG2))
    {
        ret = true;
    }

    pthread_mutex_unlock(pLockMutex);

    return ret;
}

extern "C" int _HvDebugStateInfoWrite(const char* szFile, int nCodeLine, const char* szInfo)
{
    if (!HvDebugStateInfoIsFormat())
    {
        int iRet = HvDebugStateInfoFormat();
        if (iRet != 0)
            return iRet;
    }

    if (szInfo == NULL)
        return -1;

    int write_num;
    pthread_mutex_t* pLockMutex = GetLockMutex();
    if (pLockMutex == NULL)
        return -1;
    pthread_mutex_lock(pLockMutex);

    // 读要写入的数据位置号数
    if (0 > eeprom_read(INFO_WRITE_NUM_ADDR, (char*)&write_num, 4))
    {
        pthread_mutex_unlock(pLockMutex);
        return -1;
    }

    struct tm *tm;
    time_t curtime;
    time(&curtime);
    tm = localtime(&curtime);

    // time
    char* const pBuf = g_rwbuf;
    int len = strftime(pBuf, 30, "%y%m%d%H%M%S", tm);

    // filename
    // 只取文件名
    char* pFileName = strrchr((char*)szFile, '/');

    pFileName = (pFileName == NULL) ? (char*)szFile : pFileName+1;

    len = strlen(pFileName);
    if (len > FILE_SEG_LEN) // 文件名过长处理
    {
        // 保留后8位
        memcpy(pBuf+FILE_SEG_FSET+FILE_SEG_LEN-8, pFileName+len-8, 8);
        pBuf[FILE_SEG_FSET+FILE_SEG_LEN-9] = '~';
        len = FILE_SEG_LEN;
    }
    memcpy(pBuf+FILE_SEG_FSET, pFileName, len);
    memset(pBuf+FILE_SEG_FSET+len, ' ', FILE_SEG_LEN-len);  // 补空

    // line
    len = sprintf(pBuf+LINE_SEG_FSET, "%d", nCodeLine);
    memset(pBuf+LINE_SEG_FSET+len, ' ', LINE_SEG_LEN-len-2); // 补空
    pBuf[LINE_SEG_FSET+LINE_SEG_LEN-2] = '\r';  // 回车
    pBuf[LINE_SEG_FSET+LINE_SEG_LEN-1] = '\n';  // 换行

    // info
    len = strlen(szInfo);
    int overlen = len - (INFO_SEG_LEN-2);

    len = len - ((overlen > 0) ? overlen : 0 ); // 过长截断

    memcpy(pBuf+INFO_SEG_FSET, szInfo, len);
    pBuf[INFO_SEG_FSET+INFO_SEG_LEN-2] = '\r'; // 回车
    pBuf[INFO_SEG_FSET+INFO_SEG_LEN-1] = '\n'; // 换行

    if (overlen < 0)    // 没有超出则补空
    {
        memset(pBuf+INFO_SEG_FSET+len, ' ', -overlen);
    }

    // 写入
    if (0 > eeprom_write(INFO_DATA_START_ADDR+INFO_LINE_LEN*write_num, g_rwbuf, INFO_LINE_LEN))
    {
        pthread_mutex_unlock(pLockMutex);
        return -1;
    }

    write_num++;

    if (write_num == INFO_LINE_NUM)
    {
        write_num = 0;
    }
    if (0 > eeprom_write(INFO_WRITE_NUM_ADDR, (char*)&write_num, 4))
    {
        pthread_mutex_unlock(pLockMutex);
        return -1;
    }

    pthread_mutex_unlock(pLockMutex);
    return 0;
}

extern "C" int HvDebugStateInfoReadAll(char* szBuf, int* piInfoLen)
{
    pthread_mutex_t* pLockMutex = GetLockMutex();
    if (pLockMutex == NULL)
        return -1;
    pthread_mutex_lock(pLockMutex);

    int ret = (0 > eeprom_read(INFO_DATA_START_ADDR, szBuf, INFO_CAPACITY))
                ? -1 : 0;

    pthread_mutex_unlock(pLockMutex);

    if (piInfoLen != NULL)
    {
        *piInfoLen = INFO_LINE_LEN * INFO_LINE_NUM + 1;
    }

    return ret;
}
