/**
* @file VideoTable.h 
* @brief CVideoTable is used to save the video file mapping table
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-21
* @version 1.0
*/

#ifndef _VIDEO_TABLE_H_
#define _VIDEO_TABLE_H_

#include "FileQueueTable.h"

class CVideoTable : public CFileQueueTable
{
public:
    CVideoTable(DWORD dwTotalBlockCount, DWORD dwOneBlockSize);
    ~CVideoTable();
     
    HRESULT ReadTable(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
                   DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
                   DWORD dwCarID, DWORD &dwStartIndex, DWORD &dwEndIndex);    
    
    HRESULT WriteTable(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
               DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
               DWORD dwSize, DWORD &dwCarID, DWORD &dwStartIndex, DWORD &dwEndIndex);

    DWORD GetFileCount(DWORD dwYear, DWORD dwMonth, DWORD dwDay, DWORD dwHour);
    

private:

    
};

#endif //_VIDEO_TABLE_H_




