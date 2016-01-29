/**
* @file ResultTable.h 
* @brief CResultTable is used to save the result file mapping table
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-21
* @version 1.0
*/

#ifndef _RESULT_TABLE_H_
#define _RESULT_TABLE_H_

#include "FileQueueTable.h"

class CResultTable : public CFileQueueTable
{
public:
    CResultTable(DWORD dwTotalBlockCount, DWORD dwOneBlockSize);
    ~CResultTable();
        
    HRESULT ReadTable(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
                   DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
                   DWORD dwCarID, DWORD &dwStartIndex, DWORD &dwEndIndex);    
    
    HRESULT WriteTable(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
               DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
               DWORD dwSize, DWORD &dwCarID, DWORD &dwStartIndex, DWORD &dwEndIndex);

    DWORD GetFileCount(DWORD dwYear, DWORD dwMonth, DWORD dwDay, DWORD dwHour);
    

private:
	
};

#endif //_RESULT_TABLE_H_




