#ifndef __FIX_FILE_SYSTEM_H__
#define __FIX_FILE_SYSTEM_H__
#include "Queue.h"
#include <string>
#include <dirent.h>
#define MIN_RESERVE_SIZE 1 //单位G
#define MAX_RESERVE_SIZE 4 //单位G

using namespace std;

class CFixFileStream :public CHvThreadBase
{
    typedef struct tagFILE_HEADER
    {
        int iCount;
        int iIndex;
        int iFileSize;
    }FILE_HEADER;
public:
    CFixFileStream();
    virtual ~CFixFileStream();
    virtual const char *GetName(void){return "CFixFileStream";}
    virtual HRESULT Run( void* pvParam );
public:
    virtual bool Initialize(const char* szDirecory, long long iTotalSize = 0, int iFileSize = 1024*1024);
    virtual void UnInitialize(void);
    virtual bool IsValid(void){return m_fInitialize;}
    virtual bool Write(const char* szFileName, void* pBuffer, int iSize);
    virtual bool Read(const char* szFileName, void* buffer, int iSize);
    virtual int  GetFileCount(const char* szPath);
protected:
    pthread_mutex_t m_mutex;
    bool m_fInitialize;
    char m_szDirectory[255];
    FILE_HEADER m_fh;
    CQueue<string, 2>m_queDeleteFile;
};
#endif
