#ifndef __BIGFILE_H__
#define __BIGFILE_H__
#include "FileQueue.h"

class CBigFile : public CFileQueue
{
public:
    CBigFile();
    virtual ~CBigFile();
    HRESULT Initialize(const char* szDirecory, long long iTotalSize = 0, int iFileSize = 1024*1024, bool fRecord = true);
    void UnInitialize(void);
    bool IsValid(void)
    {
        return m_fInitialize;
    }
    HRESULT Write(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay, DWORD32 dwHour, DWORD32 dwMinute, DWORD32 dwSecond, void* pBuffer, int iSize);
    HRESULT Read(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay, DWORD32 dwHour, DWORD32 dwMinute, DWORD32 dwSecond, DWORD32 dwCarID, void* buffer, int iSize);
    DWORD32 GetFileCount(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay, DWORD32 dwHour);
    HRESULT Commit(bool fForce, bool fIsRecord);
private:
    pthread_mutex_t m_mutex;
    bool m_fInitialize;
    char m_szDirectory[255];
    DWORD32 m_dwOneFileSize;
    DWORD32 m_dwTotalFileCount;
    BYTE8* m_pIndexBuffer;
    DWORD32 m_dwCommitCount;
    DWORD32 m_dwMasteCommitCount;
    DWORD32 m_dwLastCommitTime;
    bool    m_fUpdate;
    int m_iCommitCount;
};
#endif
