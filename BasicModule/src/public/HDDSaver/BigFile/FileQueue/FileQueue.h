/**
* @file FileQueue.h 
* @brief CFileQueue is used to calculate the result/video file read/wirte ranges
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-21
* @version 1.0
*/

#ifndef _FILE_QUEUE_H_
#define _FILE_QUEUE_H_

#include "FileQueueTable.h"
#include "SWFC.h"


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
    HRESULT Initialize(DWORD dwTotalBlockCount, DWORD dwOneBlockSize, FQ_TYPE fqType = FQ_TYPE_RESULT);

    HRESULT Load(BYTE * pbBuf, const DWORD dwLen);
	
    HRESULT Commit(BYTE * pbBuf, const DWORD dwLen);
    
    HRESULT Write(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
                   DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
                   DWORD dwSize, DWORD &dwCarID, DWORD &dwStartIndex, DWORD &dwEndIndex);
	
    HRESULT Read(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
                   DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
                   DWORD dwCarID, DWORD &dwStartIndex, DWORD &dwEndIndex);

    
    DWORD GetFileCount(DWORD dwYear, DWORD dwMonth, DWORD dwDay, DWORD dwHour);

    HRESULT Print();


	
	DWORD GetTotalCount()
	{
		if (m_fgInited && NULL != m_pTable)
		{
			return m_pTable->GetTotalCount();
		}

		return 0;
	}
	
	
	DWORD GetAvailableCount()
	{
		if (m_fgInited && NULL != m_pTable)
		{
			return m_pTable->GetAvailableCount();
		}

		return 0;
	}
	
	
	DWORD GetOffset()
	{
		if (m_fgInited && NULL != m_pTable)
		{
			return m_pTable->GetOffset();
		}

		return 0;
	}
    
private:

    
    BOOL    m_fgInited;
    FQ_TYPE m_fqType; // Result or Video
    CFileQueueTable * m_pTable;
    
};

#endif // _FILE_QUEUE_H_

