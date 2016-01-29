/**
* @file FileQueueTable.cpp 
* @brief implementation of CFileQueueTable
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-21
* @version 1.0
*/

#include "FileQueueTable.h"
#include "swpa.h"


#define PRINT SW_TRACE_DEBUG



#define SHIFTADDR(pbAddr, dwShiftLen, pbBuf, dwRange)                                      \
    if ((DWORD)(pbAddr)+(dwShiftLen) >= (DWORD)(pbBuf)+(dwRange))                      \
    {                                                                                      \
        Clear();                                                                            \
        PRINT( "[FileQueueTable] Table size exceeds %d at L%d!!\n", (dwLen), __LINE__); \
        return E_FAIL;                                                                     \
    }                                                                                      \
    else                                                                                  \
    {                                                                                      \
        (pbAddr) += (dwShiftLen);                                                         \
    }                                                                                      \



HRESULT CFileQueueTable::LoadTable(BYTE* pbBuf, const DWORD dwLen)
{
    BYTE * pbAddr = NULL;
    WORD wIdx;
    RECORDNODE * prnNode = NULL;
    DWORD dwTrickNum = 0;

    Clear();//clear the tables first

    pbAddr = pbBuf;

    if (NULL == pbAddr)
    {
        m_dwRemained = m_dwTotalBlockCount;
        m_dwOffset = 0;
        //m_dwRecordLen = 0;

        return S_OK;
    }

    swpa_memcpy(&m_dwTotalBlockCount, pbAddr, sizeof(m_dwTotalBlockCount));
    SHIFTADDR(pbAddr, sizeof(m_dwTotalBlockCount), pbBuf, dwLen);

    swpa_memcpy(&m_dwRemained, pbAddr, sizeof(m_dwRemained));
    SHIFTADDR(pbAddr, sizeof(m_dwRemained), pbBuf, dwLen);

    swpa_memcpy(&m_dwOffset, pbAddr, sizeof(m_dwOffset));
    SHIFTADDR(pbAddr, sizeof(m_dwOffset), pbBuf, dwLen);

    //从表头开始逐条读取记录
    while (1)
    {
        swpa_memcpy(&dwTrickNum, pbAddr, sizeof(m_dwTRICKNUMBER));
		SHIFTADDR(pbAddr, sizeof(m_dwTRICKNUMBER), pbBuf, dwLen);
		
        if (m_dwTRICKNUMBER == dwTrickNum)
        {
            prnNode = new RECORDNODE;//use "new" to alloc memory for prnNode is because it has a CSWList class member 
            if (NULL == prnNode)
            {
                // destroy dequeue
                Clear();

                PRINT( "[FileQueueTable] Out of Memory @ L%d!!\n", __LINE__);
                return E_OUTOFMEMORY;
            }
			prnNode->lstInfo.RemoveAll();
			
            swpa_memcpy(&prnNode->dwYear, pbAddr, sizeof(prnNode->dwYear));
            SHIFTADDR(pbAddr, sizeof(prnNode->dwYear), pbBuf, dwLen);
			
            swpa_memcpy(&prnNode->dwMonth, pbAddr, sizeof(prnNode->dwMonth));
            SHIFTADDR(pbAddr, sizeof(prnNode->dwMonth), pbBuf, dwLen);
			
            swpa_memcpy(&prnNode->dwDay, pbAddr, sizeof(prnNode->dwDay));
            SHIFTADDR(pbAddr, sizeof(prnNode->dwDay), pbBuf, dwLen);
			
            swpa_memcpy(&prnNode->dwHour, pbAddr, sizeof(prnNode->dwHour));
            SHIFTADDR(pbAddr, sizeof((prnNode->dwHour)), pbBuf, dwLen);
			
            swpa_memcpy(&prnNode->dwDataStartID, pbAddr, sizeof(prnNode->dwDataStartID));
            SHIFTADDR(pbAddr, sizeof(prnNode->dwDataStartID), pbBuf, dwLen);
			
            swpa_memcpy(&prnNode->dwDataEndID, pbAddr, sizeof(prnNode->dwDataEndID));
            SHIFTADDR(pbAddr, sizeof(prnNode->dwDataEndID), pbBuf, dwLen);
			
            swpa_memcpy(&prnNode->dwFileStartIndex, pbAddr, sizeof(prnNode->dwFileStartIndex));
            SHIFTADDR(pbAddr, sizeof(prnNode->dwFileStartIndex), pbBuf, dwLen);
			
            swpa_memcpy(&prnNode->dwFileEndIndex, pbAddr, sizeof(prnNode->dwFileEndIndex));
            SHIFTADDR(pbAddr, sizeof(prnNode->dwFileEndIndex), pbBuf, dwLen);
			
            swpa_memcpy(&prnNode->dwInfoLen, pbAddr, sizeof(prnNode->dwInfoLen));
            SHIFTADDR(pbAddr, sizeof(prnNode->dwInfoLen), pbBuf, dwLen);
			
            for (wIdx = 0; wIdx < prnNode->dwInfoLen; wIdx++)
            {
                STORAGEINFO * psiInfo = (STORAGEINFO*)swpa_mem_alloc(sizeof(STORAGEINFO));//new STORAGEINFO;

                if (NULL == psiInfo)
                {
                    // destroy dequeue
                    Clear();

                    PRINT( "[FileQueueTable] Out of Memory @ L%d!!\n", __LINE__);
                    return E_OUTOFMEMORY;
                }

                swpa_memcpy(&psiInfo->wRepeatTimes, pbAddr, sizeof(psiInfo->wRepeatTimes));
                SHIFTADDR(pbAddr, sizeof(psiInfo->wRepeatTimes), pbBuf, dwLen);

                swpa_memcpy(&psiInfo->wBlockCnt, pbAddr, sizeof(psiInfo->wBlockCnt));
                SHIFTADDR(pbAddr, sizeof(psiInfo->wBlockCnt), pbBuf, dwLen);

                //用AddTail()保证与SaveTable()中数据顺序一致
                if (NULL == prnNode->lstInfo.AddTail(psiInfo))
            	{
            		PRINT("Err: failed to add psiInfo to lstInfo tail\n");
					SAFE_MEM_FREE(psiInfo);
					return E_FAIL;
            	}
            }

            //最新的记录位于表的前面，故要用AddTail()保证新的记录在前面
            if (NULL == m_lstRecords.AddTail(prnNode))
        	{
        		PRINT("Err: failed to add prnNode to Records tail\n");
				return E_FAIL;
        	}
            //m_dwRecordLen ++;
        }
        else
        {
        	PRINT("Warning: dwTrickNum = 0x%x (!= m_dwTRICKNUMBER, 0x%x), end of file queue table!\n", dwTrickNum, m_dwTRICKNUMBER);
            break; // finished
        }
    }

    return S_OK;
}


HRESULT CFileQueueTable::SaveTable(BYTE* pbBuf, const DWORD dwLen)
{
    BYTE * pbAddr = NULL;
	DWORD dwTableSize = 0;
	BOOL fgOverflowed = FALSE;
	DWORD dwValidRecordCount = 0;
	SW_POSITION Pos = NULL;


    pbAddr = pbBuf;

	if (NULL == pbBuf || 0 == dwLen)
	{
	    PRINT( "[FileQueueTable] Invalid args\n");
		return E_INVALIDARG;
	}


	//先计算内存中表的大小，如果超过给定的dwLen，
	//则先删除最旧的记录直至size在dwLen之内

    //表头数据占用空间
    dwTableSize = sizeof(m_dwTotalBlockCount)
		+ sizeof(m_dwRemained)
		+ sizeof(m_dwOffset);
	if (dwTableSize >= dwLen)
	{
	    fgOverflowed = TRUE;
	    PRINT( "[FileQueueTable] Table size exceeds %d at L%d!!\n", (dwLen), __LINE__);
		return E_FAIL;
	}

	//逐条累加各记录长度	， 找出需要保存的记录数
	fgOverflowed = FALSE;
	dwValidRecordCount = 0;
	Pos = m_lstRecords.GetHeadPosition();
	DWORD dwSize = sizeof(m_dwTRICKNUMBER)
			    + sizeof(DWORD)/*sizeof(prnNode->dwYear) */+ sizeof(DWORD)/*sizeof(prnNode->dwMonth) */ + sizeof(DWORD)/*sizeof(prnNode->dwDay) */ + sizeof(DWORD)/*sizeof(prnNode->dwHour) */
				+ sizeof(DWORD)/*sizeof(prnNode->dwDataStartID) */ + sizeof(DWORD)/*sizeof(prnNode->dwDataEndID) */
				+ sizeof(DWORD)/*sizeof(prnNode->dwFileStartIndex) */ + sizeof(DWORD)/*sizeof(prnNode->dwFileEndIndex) */
				+ sizeof(DWORD)/*sizeof(prnNode->dwInfoLen) */;
	while ( m_lstRecords.IsValid(Pos))
    {
        DWORD dwRecordSize = dwSize;
		RECORDNODE * prnNode = m_lstRecords.GetNext(Pos);

		//PRINT( "[FileQueueTable]  prnNode->lstInfo.GetCount()= %d!!\n",	prnNode->lstInfo.GetCount());

		dwRecordSize += (prnNode->lstInfo.GetCount() * sizeof(STORAGEINFO));

		//如果加上这条记录的长度就溢出
		if (dwTableSize + dwRecordSize > dwLen)
		{
		    fgOverflowed = TRUE;
		    break;
		}
		else
		{
			dwTableSize += dwRecordSize;
			dwValidRecordCount++;
		}
    }

	//PrintTable();

    //有溢出则从最旧的开始清除记录，且回收存储块
	if (fgOverflowed)
	{
	    PRINT( "[FileQueueTable] Warning: Table's size exceeds buffer's, delete old records in FileQueue Table until the buffer can save the table!!\n");

	    while ( m_lstRecords.GetCount() > dwValidRecordCount)
	    {
	        if (0 >= m_lstRecords.GetCount())
        	{
        	    PRINT( "[FileQueueTable] Crashed at L%d!!\n",  __LINE__);
				return E_FAIL;
        	}

	        RECORDNODE * prnNode = m_lstRecords.GetTail();

	        while (!prnNode->lstInfo.IsEmpty())
	        {	        
	            STORAGEINFO* psiItem = prnNode->lstInfo.GetHead();
				//回收
				m_dwRemained += psiItem->wBlockCnt * psiItem->wRepeatTimes;
				
	            prnNode->lstInfo.RemoveHead();
				
	            swpa_mem_free(psiItem);//delete psiItem;
	        }

			PRINT("[FileQueueTable]Info: deleting data %4d/%02d/%02d %02d: Node[%d~%d]\n", 
				prnNode->dwYear, prnNode->dwMonth, prnNode->dwDay, prnNode->dwHour, prnNode->dwDataStartID, prnNode->dwDataEndID);

	        m_lstRecords.RemoveTail();
	        delete prnNode; //swpa_mem_free(prnNode);
	    }
	}

    // clear
    swpa_memset(pbAddr, 0, sizeof(BYTE)*dwLen);

    //开始存储
    swpa_memcpy(pbAddr, &m_dwTotalBlockCount, sizeof(m_dwTotalBlockCount));
    SHIFTADDR(pbAddr, sizeof(m_dwTotalBlockCount), pbBuf, dwLen);

    swpa_memcpy(pbAddr, &m_dwRemained, sizeof(m_dwRemained));
    SHIFTADDR(pbAddr, sizeof(m_dwRemained), pbBuf, dwLen);

    swpa_memcpy(pbAddr, &m_dwOffset, sizeof(m_dwOffset));
    SHIFTADDR(pbAddr, sizeof(m_dwOffset), pbBuf, dwLen);


    //从表头开始逐条保存记录
    Pos = m_lstRecords.GetHeadPosition();
    while (m_lstRecords.IsValid(Pos))
    {
    	RECORDNODE * prnNode = m_lstRecords.GetNext(Pos);
		
        DWORD dwTrickNum = m_dwTRICKNUMBER;
        swpa_memcpy(pbAddr, &dwTrickNum, sizeof(dwTrickNum));
        SHIFTADDR(pbAddr, sizeof(dwTrickNum), pbBuf, dwLen);

        swpa_memcpy(pbAddr, &prnNode->dwYear, sizeof(prnNode->dwYear));
        SHIFTADDR(pbAddr, sizeof(prnNode->dwYear), pbBuf, dwLen);

        swpa_memcpy(pbAddr,    &prnNode->dwMonth, sizeof(prnNode->dwMonth));
        SHIFTADDR(pbAddr, sizeof(prnNode->dwMonth), pbBuf, dwLen);

        swpa_memcpy(pbAddr,    &prnNode->dwDay, sizeof(prnNode->dwDay));
        SHIFTADDR(pbAddr, sizeof(prnNode->dwDay), pbBuf, dwLen);

        swpa_memcpy(pbAddr,    &prnNode->dwHour, sizeof(prnNode->dwHour));
        SHIFTADDR(pbAddr, sizeof(prnNode->dwHour), pbBuf, dwLen);

        swpa_memcpy(pbAddr,    &prnNode->dwDataStartID, sizeof(prnNode->dwDataStartID));
        SHIFTADDR(pbAddr, sizeof(prnNode->dwDataStartID), pbBuf, dwLen);

        swpa_memcpy(pbAddr,    &prnNode->dwDataEndID, sizeof(prnNode->dwDataEndID));
        SHIFTADDR(pbAddr, sizeof(prnNode->dwDataEndID), pbBuf, dwLen);

        swpa_memcpy(pbAddr,    &prnNode->dwFileStartIndex, sizeof(prnNode->dwFileStartIndex));
        SHIFTADDR(pbAddr, sizeof(prnNode->dwFileStartIndex), pbBuf, dwLen);

        swpa_memcpy(pbAddr,    &prnNode->dwFileEndIndex, sizeof(prnNode->dwFileEndIndex));
        SHIFTADDR(pbAddr, sizeof(prnNode->dwFileEndIndex), pbBuf, dwLen);

        swpa_memcpy(pbAddr,    &prnNode->dwInfoLen, sizeof(prnNode->dwInfoLen));
        SHIFTADDR(pbAddr, sizeof(prnNode->dwInfoLen), pbBuf, dwLen);

		SW_POSITION Position = prnNode->lstInfo.GetHeadPosition();
        while (prnNode->lstInfo.IsValid(Position))
        {
        	STORAGEINFO * psInfo = prnNode->lstInfo.GetNext(Position);
			
            swpa_memcpy(pbAddr,    &psInfo->wRepeatTimes, sizeof(psInfo->wRepeatTimes));
            SHIFTADDR(pbAddr, sizeof(psInfo->wRepeatTimes), pbBuf, dwLen);

            swpa_memcpy(pbAddr,    &psInfo->wBlockCnt, sizeof(psInfo->wBlockCnt));
            SHIFTADDR(pbAddr, sizeof(psInfo->wBlockCnt), pbBuf, dwLen);
        }
    }

    return S_OK;
}



HRESULT CFileQueueTable::PrintTable()
{
    PRINT( "-Total File Count = %d\n", m_dwTotalBlockCount);
    PRINT( "-Avail File Count = %d\n", m_dwRemained);
    PRINT( "-Offset = %d\n", m_dwOffset);

    INT iCount = 20;
	SW_POSITION Pos = m_lstRecords.GetHeadPosition();
    while ( m_lstRecords.IsValid(Pos))
    {
    	RECORDNODE * prnNode = m_lstRecords.GetNext(Pos);
		
        PRINT("%4d/%02d/%02d %02d Block: %d ~ %d (%d) CarID: %d ~ %d (%d)\n",
            prnNode->dwYear, prnNode->dwMonth, prnNode->dwDay, prnNode->dwHour,
            prnNode->dwDataStartID, prnNode->dwDataEndID,
            (prnNode->dwDataEndID + m_dwTotalBlockCount - prnNode->dwDataStartID) % m_dwTotalBlockCount + 1,
            prnNode->dwFileStartIndex, prnNode->dwFileEndIndex,
            prnNode->dwFileEndIndex - prnNode->dwFileStartIndex + 1);

		if (--iCount == 0)
		{
			break;
		}
    }

	if ( m_lstRecords.IsValid(Pos))
	{
		PRINT("-- * Unprinted Items *--\n");
	}

	PRINT( "-- * Table End * --\n");

    return S_OK;
}




HRESULT CFileQueueTable::Clear()
{
    //从旧到新逐条清除
    while ( !m_lstRecords.IsEmpty())
    {
        RECORDNODE * prnNode = m_lstRecords.RemoveTail();

        while ( !prnNode->lstInfo.IsEmpty() )
        {
            STORAGEINFO* psiItem = prnNode->lstInfo.RemoveHead();
            swpa_mem_free(psiItem);//delete psiItem;
        }

		/*PRINT( "Info: deleting %4d/%02d/%02d %02d Block: %d ~ %d (%d) CarID: %d ~ %d (%d)\n",
            prnNode->dwYear, prnNode->dwMonth, prnNode->dwDay, prnNode->dwHour,
            prnNode->dwDataStartID, prnNode->dwDataEndID,
            prnNode->dwDataEndID - prnNode->dwDataStartID + 1,
            prnNode->dwFileStartIndex, prnNode->dwFileEndIndex,
            prnNode->dwFileEndIndex - prnNode->dwFileStartIndex + 1);
            */
		
        delete prnNode;
    }

    m_dwRemained = m_dwTotalBlockCount;
    m_dwOffset = 0;
    //m_dwRecordLen = 0;

    return S_OK;
}





