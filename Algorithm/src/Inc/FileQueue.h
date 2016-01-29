

#ifndef _FILE_QUEUE_H_
#define _FILE_QUEUE_H_

#include "FileQueueTable.h"

typedef enum _FQ_TYPE
{
    FQ_TYPE_RESULT = 0,
    FQ_TYPE_VIDEO,

    FQ_TYPE_CNT
}FQ_TYPE;



class CFileQueue
{
public:
    CFileQueue();
    virtual ~CFileQueue();
    HRESULT Initialize(DWORD32 dwTotalBlockCount, DWORD32 dwOneBlockSize, FQ_TYPE fqType = FQ_TYPE_RESULT);

    HRESULT Load(BYTE8 * pbBuf, const DWORD32 dwLen);
    HRESULT Commit(BYTE8 * pbBuf, const DWORD32 dwLen);
    
    HRESULT Write(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay, 
                   DWORD32 dwHour, DWORD32 dwMinute, DWORD32 dwSecond, 
                   DWORD32 dwSize, DWORD32 &dwCarID, DWORD32 &dwStartIndex, DWORD32 &dwEndIndex);
    HRESULT Read(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay, 
                   DWORD32 dwHour, DWORD32 dwMinute, DWORD32 dwSecond, 
                   DWORD32 dwCarID, DWORD32 &dwStartIndex, DWORD32 &dwEndIndex);

    
    DWORD32 GetFileCount(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay, DWORD32 dwHour);

    HRESULT Print();
    
private:
    
    bool    m_fgInited;
    FQ_TYPE m_fqType; // Result or Video
    CFileQueueTable * m_pTable;
    
};

#endif // _FILE_QUEUE_H_
