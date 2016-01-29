
#ifndef _RESULT_TABLE_H_
#define _RESULT_TABLE_H_

#include "FileQueueTable.h"

class CResultTable : public CFileQueueTable
{
public:
    CResultTable(DWORD32 dwTotalBlockCount, DWORD32 dwOneBlockSize);
    ~CResultTable();
        
    HRESULT ReadTable(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay, 
                   DWORD32 dwHour, DWORD32 dwMinute, DWORD32 dwSecond, 
                   DWORD32 dwCarID, DWORD32 &dwStartIndex, DWORD32 &dwEndIndex);    
    
    HRESULT WriteTable(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay, 
               DWORD32 dwHour, DWORD32 dwMinute, DWORD32 dwSecond, 
               DWORD32 dwSize, DWORD32 &dwCarID, DWORD32 &dwStartIndex, DWORD32 &dwEndIndex);

    DWORD32 GetFileCount(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay, DWORD32 dwHour);
    

private:
	
};

#endif //_RESULT_TABLE_H_


