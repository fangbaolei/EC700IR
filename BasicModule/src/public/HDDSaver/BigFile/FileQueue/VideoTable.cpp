/**
* @file VideoTable.cpp
* @brief implementation of CVideoTable
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-21
* @version 1.0
*/

#include "swpa.h"
#include "VideoTable.h"


//todo: log output
#define PRINT printf



CVideoTable::CVideoTable(
    DWORD dwTotalBlockCount,
    DWORD dwOneBlockSize)
{
    m_dwTRICKNUMBER = 0xEFDCCDFE; // 标志一条记录的开始
    
    m_dwTotalBlockCount = dwTotalBlockCount;
    m_dwOneBlockSize = dwOneBlockSize;
    
    //m_dwRecordLen = 0;

    Clear();

	m_lstRecords.RemoveAll();
}

CVideoTable::~CVideoTable()
{
    Clear();    
}



HRESULT CVideoTable::ReadTable(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
                   DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
                   DWORD dwCarID, DWORD &dwStartIndex, DWORD &dwEndIndex)
{
    HRESULT hr = S_OK;
    DWORD dwStart=0, dwEnd=0, dwID=0, dwOffset=0, dwIndex = 0;
    BOOL fgFound = FALSE;

    //从新到旧逐条查找
    SW_POSITION Pos = m_lstRecords.GetHeadPosition();
	while (m_lstRecords.IsValid(Pos))
    {
     
		RECORDNODE * prnNode = m_lstRecords.GetNext(Pos);
		
        if (prnNode->dwYear == dwYear 
            && prnNode->dwMonth == dwMonth
            && prnNode->dwDay == dwDay 
            && prnNode->dwHour == dwHour)
        {
            dwIndex = dwMinute * 60 + dwSecond; // 转成索引
            
            if (dwIndex >= prnNode->dwFileStartIndex
                && dwIndex <= prnNode->dwFileEndIndex)
            {
                dwID = dwIndex - prnNode->dwFileStartIndex;
                dwOffset = 0;

				SW_POSITION Position = prnNode->lstInfo.GetHeadPosition();
                while (prnNode->lstInfo.IsValid(Position))
                {
                	STORAGEINFO * psInfo = prnNode->lstInfo.GetNext(Position);
					
                    if (dwID+1 <= psInfo->wRepeatTimes)
                    {                                 
                        dwStart = (dwOffset + prnNode->dwDataStartID + dwID * psInfo->wBlockCnt)% m_dwTotalBlockCount; 
                        dwEnd = (dwStart + psInfo->wBlockCnt - 1) % m_dwTotalBlockCount; 
                        
                        fgFound = TRUE;    
                        break;                        
                    }
                    else
                    {
                        dwID -= psInfo->wRepeatTimes;
                        dwOffset += (psInfo->wRepeatTimes * psInfo->wBlockCnt);
                    }
                 }

                if (fgFound)
                 {
                     break;
                 }                                    
            }
        }
     }

    if (fgFound)
    {
        dwStartIndex = dwStart;
        dwEndIndex = dwEnd;

        return S_OK;
    }
    else
    {
        PRINT( "[VideoTable] err: Record not found\n");
        return E_FAIL;
    }

}



HRESULT CVideoTable::WriteTable(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
               DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
               DWORD dwSize, DWORD &dwCarID, DWORD &dwStartIndex, DWORD &dwEndIndex)
{
    HRESULT hr = S_OK;
    DWORD dwStart=0, dwEnd=0, dwID=0, dwOffset=0, dwCnt=0, dwIndex;
    BOOL fgFound = FALSE;

    dwCnt = dwSize /(m_dwOneBlockSize+1) + 1;

        
    while ( dwCnt > m_dwRemained) 
    {
        if (m_dwRemained >= m_dwTotalBlockCount)
		{
		    PRINT( "[VideoTable] err: m_dwRemained >= m_dwTotalBlockCount, overflowed!\n");
		    return E_FAIL;
		}

		PRINT("[Video Table]Info: save count (%d) > remained count (%d), recycling...\n", dwCnt, m_dwRemained);
		
        // 取最旧的记录删除，回收空间直至够用        
        RECORDNODE * prnNode = m_lstRecords.GetTail();

        //m_dwRemained += (m_dwTotalBlockCount + prnNode->dwDataEndID - prnNode->dwDataStartID + 1) % m_dwTotalBlockCount;        
        if (prnNode->dwDataEndID >= prnNode->dwDataStartID)
        {
            m_dwRemained += (prnNode->dwDataEndID - prnNode->dwDataStartID + 1);
        }
        else
        {
            m_dwRemained += (m_dwTotalBlockCount + prnNode->dwDataEndID - prnNode->dwDataStartID + 1);
        }

        while (!prnNode->lstInfo.IsEmpty())
        {
            STORAGEINFO* psiItem = prnNode->lstInfo.GetHead();
            prnNode->lstInfo.RemoveHead();
            swpa_mem_free(psiItem);//delete psiItem;            
        }

		PRINT("[Video Table]Info: deleting data %4d/%02d/%02d %02d: Node[%d~%d]\n", 
			prnNode->dwYear, prnNode->dwMonth, prnNode->dwDay, prnNode->dwHour, prnNode->dwDataStartID, prnNode->dwDataEndID);
		
        m_lstRecords.RemoveTail();
        delete prnNode; //swpa_mem_free(prnNode);
    }

    
    //把最新的记录加在队列的最前面
    SW_POSITION Pos = m_lstRecords.GetHeadPosition();
	while (m_lstRecords.IsValid(Pos))
    {
     
		RECORDNODE * prnNode = m_lstRecords.GetNext(Pos);

        if (prnNode->dwYear == dwYear 
            && prnNode->dwMonth == dwMonth
            && prnNode->dwDay == dwDay 
            && prnNode->dwHour == dwHour)
        {
            dwIndex = dwMinute * 60 + dwSecond;

             //只有紧挨的数据才会放到同一条记录里面
            if (dwIndex == prnNode->dwFileEndIndex + 1)
            {
                STORAGEINFO * psiInfo = prnNode->lstInfo.GetTail();
                if (psiInfo->wBlockCnt == dwCnt)
                {
                    psiInfo->wRepeatTimes ++;                            
                }
                else
                {
                    STORAGEINFO * psiInfo = (STORAGEINFO *)swpa_mem_alloc(sizeof(STORAGEINFO));//new STORAGEINFO;
                    if (NULL == psiInfo)
                    {
                        PRINT( "[VideoTable] Out of Memory!\n");
                        return E_OUTOFMEMORY;
                    }
                    
                    psiInfo->wRepeatTimes = 1;
                    psiInfo->wBlockCnt = dwCnt;

                    if (NULL == prnNode->lstInfo.AddTail(psiInfo))
                	{
                		PRINT( "[VideoTable] failed to add psiInfo to lstInfo tail!\n");
                        return E_FAIL;
                	}
                    prnNode->dwInfoLen ++;                            
                }

                dwStart = (m_dwOffset) % m_dwTotalBlockCount;
                prnNode->dwDataEndID = (dwStart + dwCnt - 1) % m_dwTotalBlockCount;
                dwEnd = prnNode->dwDataEndID;
                m_dwOffset = (prnNode->dwDataEndID + 1) % m_dwTotalBlockCount;

                m_dwRemained -= dwCnt;

                prnNode->dwFileEndIndex ++;
                dwCarID = prnNode->dwFileEndIndex;

                fgFound = TRUE;
                break;
            }
            else
            {
                //no need
            }
            
        }
    }

    if (!fgFound)
    {    
        RECORDNODE * prnNode = new RECORDNODE;//(RECORDNODE *)swpa_mem_alloc(sizeof(RECORDNODE));//use "new" to alloc memory for prnNode is because it has a CSWList class member 
        if (NULL == prnNode)
        {            
            PRINT( "[VideoTable] err: Out of Memory\n");
            return E_OUTOFMEMORY;
        }

        prnNode->dwYear  = dwYear;
        prnNode->dwMonth = dwMonth;
        prnNode->dwDay   = dwDay;
        prnNode->dwHour  = dwHour;
        prnNode->dwFileStartIndex = dwMinute * 60 + dwSecond;
        prnNode->dwFileEndIndex   = dwMinute * 60 + dwSecond;
		prnNode->lstInfo.RemoveAll();
        

        STORAGEINFO * psiInfo = (STORAGEINFO *)swpa_mem_alloc(sizeof(STORAGEINFO));//new STORAGEINFO;
        if (NULL == psiInfo)
        {            
            PRINT( "[VideoTable] err: Out of Memory\n");
            return E_OUTOFMEMORY;
        }
        psiInfo->wBlockCnt = dwCnt;
        psiInfo->wRepeatTimes = 1;
        if (NULL == prnNode->lstInfo.AddTail(psiInfo))
		{
    		PRINT( "[VideoTable] failed to add psiInfo to lstInfo tail!\n");
            return E_FAIL;
    	}
        prnNode->dwInfoLen = 1;        

        prnNode->dwDataStartID = (m_dwOffset) % m_dwTotalBlockCount;
        prnNode->dwDataEndID = (prnNode->dwDataStartID + dwCnt - 1) % m_dwTotalBlockCount;                

        dwCarID = prnNode->dwFileEndIndex;
        
        m_dwOffset = (prnNode->dwDataEndID + 1) % m_dwTotalBlockCount;        
        m_dwRemained -= dwCnt;
        
        dwStart = prnNode->dwDataStartID;
        dwEnd = prnNode->dwDataEndID;

        if (NULL == m_lstRecords.AddHead(prnNode))
		{
    		PRINT( "[VideoTable] failed to add prnNode to m_lstRecords head!\n");
            return E_FAIL;
    	}
        fgFound = TRUE;        
    }


    if (fgFound)
    {
        dwStartIndex = dwStart;
        dwEndIndex = dwEnd;
        
        return S_OK;
    }
    else
    {
        PRINT( "[VideoTable] err: Record not found\n");
        return E_FAIL;
    }
}



DWORD CVideoTable::GetFileCount(DWORD dwYear, 
    DWORD dwMonth,
    DWORD dwDay, 
    DWORD dwHour)
{
    /* do nothing, return 0 directly */
    return 0;
}








