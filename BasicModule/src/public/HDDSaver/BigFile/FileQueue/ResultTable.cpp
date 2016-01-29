/**
* @file ResultTable.cpp
* @brief implementation of CResultTable
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-21
* @version 1.0
*/

#include "swpa.h"
#include "ResultTable.h"


#define PRINT SW_TRACE_DEBUG

CResultTable::CResultTable(
    DWORD dwTotalBlockCount,
    DWORD dwOneBlockSize)
{
    m_dwTRICKNUMBER = 0xEFDCCDFE;

    m_dwTotalBlockCount = dwTotalBlockCount;
    m_dwOneBlockSize = dwOneBlockSize;

    //m_dwRecordLen = 0;
    Clear();

	m_lstRecords.RemoveAll();
}

CResultTable::~CResultTable()
{
    // clear
    Clear();
}


HRESULT CResultTable::ReadTable(DWORD dwYear, DWORD dwMonth, DWORD dwDay,
                   DWORD dwHour, DWORD dwMinute, DWORD dwSecond,
                   DWORD dwCarID, DWORD &dwStartIndex, DWORD &dwEndIndex)
{
    HRESULT hr = S_OK;
    DWORD dwStart=0, dwEnd=0, dwID=0, dwOffset=0;
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
            if (dwCarID >= prnNode->dwFileStartIndex
                && dwCarID <= prnNode->dwFileEndIndex)
            {
                dwID = (dwCarID - prnNode->dwFileStartIndex);
                dwOffset = 0;
				
				SW_POSITION Position = prnNode->lstInfo.GetHeadPosition();
				while (prnNode->lstInfo.IsValid(Position))
                {                
					STORAGEINFO * psInfo = prnNode->lstInfo.GetNext(Position);
					
                    if (dwID+1 <= psInfo->wRepeatTimes) //找到
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
        PRINT( "[ResultTable] err: Record not found\n");
        return E_FAIL;
    }

}



HRESULT CResultTable::WriteTable(DWORD dwYear, DWORD dwMonth, DWORD dwDay,
               DWORD dwHour, DWORD dwMinute, DWORD dwSecond,
               DWORD dwSize, DWORD &dwCarID, DWORD &dwStartIndex, DWORD &dwEndIndex)
{
    HRESULT hr = S_OK;
    DWORD dwStart=0, dwEnd=0, dwID=0, dwOffset=0, dwCnt=0;
    BOOL fgFound = FALSE;
	DWORD dwNewRecordFileBeginIndex = 0;

    //计算所占的块数
    dwCnt = dwSize /(m_dwOneBlockSize+1) + 1;

    while (dwCnt > m_dwRemained)
    {
        if (m_dwRemained >= m_dwTotalBlockCount)
		{
		    PRINT( "[ResultTable] Err: m_dwRemained >= m_dwTotalBlockCount, overflowed!\n");
		    return E_FAIL;
		}

		PRINT("[Result Table]Info: save count (%d) > remained count (%d), recycling...\n", dwCnt, m_dwRemained);
		
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

        while (prnNode->lstInfo.GetCount() > 0)
        {
            STORAGEINFO* psiItem = prnNode->lstInfo.GetHead();
            prnNode->lstInfo.RemoveHead();
            swpa_mem_free(psiItem);//delete psiItem;
        }

		PRINT("[Result Table]Info: deleting data %4d/%02d/%02d %02d: Node[%d~%d]\n", 
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
            //特殊情况:
		    if (m_dwOffset != (prnNode->dwDataEndID+1) % m_dwTotalBlockCount)
	    	{
	    	    if (dwNewRecordFileBeginIndex < prnNode->dwFileEndIndex)
    	    	{
		    	    dwNewRecordFileBeginIndex = prnNode->dwFileEndIndex + 1;
    	    	}
				continue;
	    	}


            STORAGEINFO * psiInfo = prnNode->lstInfo.GetTail();
            if (psiInfo->wBlockCnt == dwCnt)
            {
                psiInfo->wRepeatTimes ++;
            }
            else
            {
                STORAGEINFO * psiInfo = (STORAGEINFO*)swpa_mem_alloc(sizeof(STORAGEINFO));//new STORAGEINFO;
                if (NULL == psiInfo)
                {
                    PRINT( "[ResultTable] Out of Memory!\n");
                    return E_OUTOFMEMORY;
                }

                psiInfo->wRepeatTimes = 1;
                psiInfo->wBlockCnt = dwCnt;

                if (NULL == prnNode->lstInfo.AddTail(psiInfo))
				{
		    		PRINT( "[ResultTable] failed to add psiInfo to lstInfo tail!\n");
					SAFE_MEM_FREE(psiInfo);
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
    }

    if (!fgFound)
    {
        RECORDNODE * prnNode = new RECORDNODE;//(RECORDNODE*)swpa_mem_alloc(sizeof(RECORDNODE));////use "new" to alloc memory for prnNode is because it has a CSWList class member 
        if (NULL == prnNode)
        {
            PRINT( "[ResultTable] err: Out of Memory\n");
            return E_OUTOFMEMORY;
        }

        prnNode->dwYear  = dwYear;
        prnNode->dwMonth = dwMonth;
        prnNode->dwDay   = dwDay;
        prnNode->dwHour  = dwHour;
        prnNode->dwFileStartIndex = dwNewRecordFileBeginIndex;
        prnNode->dwFileEndIndex   = dwNewRecordFileBeginIndex;
        prnNode->dwDataStartID = (m_dwOffset) % m_dwTotalBlockCount;
        prnNode->dwDataEndID = (prnNode->dwDataStartID + dwCnt - 1) % m_dwTotalBlockCount;
		prnNode->lstInfo.RemoveAll();

        STORAGEINFO * psiInfo = (STORAGEINFO*)swpa_mem_alloc(sizeof(STORAGEINFO));//new STORAGEINFO;
        if (NULL == psiInfo)
        {
            PRINT( "[ResultTable] err: Out of Memory\n");
			SAFE_DELETE(prnNode);
            return E_OUTOFMEMORY;
        }
        psiInfo->wBlockCnt = dwCnt;
        psiInfo->wRepeatTimes = 1;
        if (NULL == prnNode->lstInfo.AddTail(psiInfo))
		{
    		PRINT( "[ResultTable] failed to add psiInfo to lstInfo tail!\n");
			SAFE_MEM_FREE(psiInfo);
			SAFE_DELETE(prnNode);
            return E_FAIL;
    	}
        prnNode->dwInfoLen = 1;

        dwCarID = prnNode->dwFileEndIndex;

        m_dwOffset = (prnNode->dwDataEndID + 1) % m_dwTotalBlockCount;
        m_dwRemained -= dwCnt;

        dwStart = prnNode->dwDataStartID;
        dwEnd = prnNode->dwDataEndID;

        if (NULL == m_lstRecords.AddHead(prnNode))
		{
    		PRINT( "[ResultTable] failed to add prnNode to m_lstRecords head!\n");
			SAFE_MEM_FREE(psiInfo); 
			SAFE_DELETE(prnNode);
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
        PRINT( "[ResultTable] err: Record not found\n");
        return E_FAIL;
    }
}



DWORD CResultTable::GetFileCount(DWORD dwYear,
    DWORD dwMonth,
    DWORD dwDay,
    DWORD dwHour)
{
    DWORD dwFileCount = 0;
	
    SW_POSITION Pos = m_lstRecords.GetHeadPosition();
	while (m_lstRecords.IsValid(Pos))
    {     
		RECORDNODE * prnNode = m_lstRecords.GetNext(Pos);

        if (prnNode->dwYear == dwYear
            && prnNode->dwMonth == dwMonth
            && prnNode->dwDay == dwDay
            && prnNode->dwHour == dwHour)
        {
             dwFileCount += prnNode->dwFileEndIndex - prnNode->dwFileStartIndex + 1; // must +1
        }
	}

    return dwFileCount;
}



/*
HRESULT CResultTable::PrintTable()
{
    PRINT( "---- Result Table Info ----\n");
    PRINT( "-Total File Count = %d\n", m_dwTotalBlockCount);
    PRINT( "-Avail File Count = %d\n", m_dwRemained);
    PRINT( "-Offset = %d\n", m_dwOffset);

    for (std::deque<RECORDNODE*>::iterator it = m_lstRecords.begin();
         it != m_lstRecords.end();
         it++)
     {
         PRINT( "%4d/%02d/%02d %02d\nBlock: %d ~ %d (%d)\nCarID: %d ~ %d (%d)\n",
            (*it)->dwYear, (*it)->dwMonth, (*it)->dwDay, (*it)->dwHour,
            (*it)->dwDataStartID, (*it)->dwDataEndID,
            (*it)->dwDataEndID - (*it)->dwDataStartID + 1,
            (*it)->dwFileStartIndex, (*it)->dwFileEndIndex,
            (*it)->dwFileEndIndex - (*it)->dwFileStartIndex + 1);
        PRINT( "Mapping Table:\n");
        for (std::deque<STORAGEINFO*>::iterator itr = (*it)->lstInfo.begin();
             itr != (*it)->lstInfo.end();
             itr ++)
         {
             PRINT( "[%d][%d]-", (*itr)->wRepeatTimes, (*itr)->wBlockCnt);
         }
         PRINT( "[][]\n");
     }

    return S_OK;

}
*/



/*
HRESULT CResultTable::Clear()
{
    while ( m_lstRecords.size() > 0)
    {
        RECORDNODE * prnNode = m_lstRecords.back();

        while (prnNode->lstInfo.size() > 0)
        {
            STORAGEINFO* psiItem = prnNode->lstInfo.front();
            prnNode->lstInfo.pop_front();
            delete psiItem;
        }

        m_lstRecords.pop_back();
        delete prnNode;
    }

    m_dwRemained = 0;
    m_dwOffset = 0;
    //m_dwRecordLen = 0;

    return S_OK;
}
*/




