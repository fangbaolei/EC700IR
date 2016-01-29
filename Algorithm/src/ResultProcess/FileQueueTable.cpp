
#include "FileQueueTable.h"

#define SHIFTADDR(pbAddr, dwShiftLen, pbBuf, dwRange)                                      \
    if ((DWORD32)(pbAddr)+(dwShiftLen) >= (DWORD32)(pbBuf)+(dwRange))                      \
    {                                                                                      \
        Clear();                                                                            \
        HV_Trace(5, "[FileQueueTable] Table size exceeds %d at L%d!!\n", (dwLen), __LINE__); \
        return E_FAIL;                                                                     \
    }                                                                                      \
    else                                                                                  \
    {                                                                                      \
        (pbAddr) += (dwShiftLen);                                                         \
    }                                                                                      \





HRESULT CFileQueueTable::LoadTable(BYTE8* pbBuf, const DWORD32 dwLen)
{
    BYTE8 * pbRecord = NULL, * pbAddr = NULL;
    WORD16 wIdx;
    RECORDNODE * prnNode = NULL;
    DWORD32 dwTrickNum = 0;

    Clear();//clear the tables first

    pbAddr = pbBuf;

    if (NULL == pbAddr)
    {
        m_dwRemained = m_dwTotalBlockCount;
        m_dwOffset = 0;
        //m_dwRecordLen = 0;

        return S_OK;
    }

    memcpy(&m_dwTotalBlockCount, pbAddr, sizeof(m_dwTotalBlockCount));
    //pbAddr += sizeof(m_dwTotalBlockCount);
    SHIFTADDR(pbAddr, sizeof(m_dwTotalBlockCount), pbBuf, dwLen);

    memcpy(&m_dwRemained, pbAddr, sizeof(m_dwRemained));
    //pbAddr += sizeof(m_dwRemained);
    SHIFTADDR(pbAddr, sizeof(m_dwRemained), pbBuf, dwLen);

    memcpy(&m_dwOffset, pbAddr, sizeof(m_dwOffset));
    //pbAddr += sizeof(m_dwOffset);
    SHIFTADDR(pbAddr, sizeof(m_dwOffset), pbBuf, dwLen);

    //从表头开始逐条读取记录
    while (1)
    {
        memcpy(&dwTrickNum, pbAddr, sizeof(dwTrickNum));
        if (m_dwTRICKNUMBER == dwTrickNum)
        {
            prnNode = new RECORDNODE;
            if (NULL == prnNode)
            {
                // destroy dequeue
                Clear();

                HV_Trace(5, "[FileQueueTable] Out of Memory @ L%d!!\n", __LINE__);
                return E_OUTOFMEMORY;
            }

            //pbAddr += sizeof(m_dwTRICKNUMBER); // for alignment
            SHIFTADDR(pbAddr, sizeof(m_dwTRICKNUMBER), pbBuf, dwLen);
            memcpy(&prnNode->dwYear, pbAddr, sizeof(prnNode->dwYear));

            //pbAddr += sizeof(prnNode->dwYear);
            SHIFTADDR(pbAddr, sizeof(prnNode->dwYear), pbBuf, dwLen);
            memcpy(&prnNode->dwMonth, pbAddr, sizeof(prnNode->dwMonth));

            //pbAddr += sizeof(prnNode->dwMonth);
            SHIFTADDR(pbAddr, sizeof(prnNode->dwMonth), pbBuf, dwLen);
            memcpy(&prnNode->dwDay, pbAddr, sizeof(prnNode->dwDay));

            //pbAddr += sizeof(prnNode->dwDay);
            SHIFTADDR(pbAddr, sizeof(prnNode->dwDay), pbBuf, dwLen);
            memcpy(&prnNode->dwHour, pbAddr, sizeof(prnNode->dwHour));

            //pbAddr += sizeof(prnNode->dwHour);
            SHIFTADDR(pbAddr, sizeof((prnNode->dwHour)), pbBuf, dwLen);
            memcpy(&prnNode->dwDataStartID, pbAddr, sizeof(prnNode->dwDataStartID));

            //pbAddr += sizeof(prnNode->dwDataStartID);
            SHIFTADDR(pbAddr, sizeof(prnNode->dwDataStartID), pbBuf, dwLen);
            memcpy(&prnNode->dwDataEndID, pbAddr, sizeof(prnNode->dwDataEndID));

            //pbAddr += sizeof(prnNode->dwDataEndID);
            SHIFTADDR(pbAddr, sizeof(prnNode->dwDataEndID), pbBuf, dwLen);
            memcpy(&prnNode->dwFileStartIndex, pbAddr, sizeof(prnNode->dwFileStartIndex));

            //pbAddr += sizeof(prnNode->dwFileStartIndex);
            SHIFTADDR(pbAddr, sizeof(prnNode->dwFileStartIndex), pbBuf, dwLen);
            memcpy(&prnNode->dwFileEndIndex, pbAddr, sizeof(prnNode->dwFileEndIndex));

            //pbAddr += sizeof(prnNode->dwFileEndIndex);
            SHIFTADDR(pbAddr, sizeof(prnNode->dwFileEndIndex), pbBuf, dwLen);
            memcpy(&prnNode->dwInfoLen, pbAddr, sizeof(prnNode->dwInfoLen));

            //pbAddr += sizeof(prnNode->dwInfoLen);
            SHIFTADDR(pbAddr, sizeof(prnNode->dwInfoLen), pbBuf, dwLen);
            for (wIdx = 0; wIdx < prnNode->dwInfoLen; wIdx++)
            {
                STORAGEINFO * psiInfo = new STORAGEINFO;

                if (NULL == psiInfo)
                {
                    // destroy dequeue
                    Clear();

                    HV_Trace(5, "[FileQueueTable] Out of Memory @ L%d!!\n", __LINE__);
                    return E_OUTOFMEMORY;
                }

                memcpy(&psiInfo->wRepeatTimes, pbAddr, sizeof(psiInfo->wRepeatTimes));
                //pbAddr += sizeof(psiInfo->wRepeatTimes);
                SHIFTADDR(pbAddr, sizeof(psiInfo->wRepeatTimes), pbBuf, dwLen);

                memcpy(&psiInfo->wBlockCnt, pbAddr, sizeof(psiInfo->wBlockCnt));
                //pbAddr += sizeof(psiInfo->wBlockCnt);
                SHIFTADDR(pbAddr, sizeof(psiInfo->wBlockCnt), pbBuf, dwLen);

                //用push_back()保证与SaveTable()中数据顺序一致
                prnNode->dqInfo.push_back(psiInfo);
            }

            //最新的记录位于表的前面，故要用push_back()保证新的记录在前面
            m_dqRecords.push_back(prnNode);
            //m_dwRecordLen ++;
        }
        else
        {
            break; // finished
        }
    }

    return S_OK;
}


HRESULT CFileQueueTable::SaveTable(BYTE8* pbBuf, const DWORD32 dwLen)
{
    BYTE8 * pbAddr = NULL;
	DWORD32 dwTableSize = 0;
	bool fgOverflowed = false;
	DWORD32 dwValidRecordCount = 0;


    pbAddr = pbBuf;

	if (NULL == pbBuf || 0 == dwLen)
	{
	    HV_Trace(5, "[FileQueueTable] Invalid args\n");
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
	    fgOverflowed = true;
	    HV_Trace(5, "[FileQueueTable] Table size exceeds %d at L%d!!\n", (dwLen), __LINE__);
		return E_FAIL;
	}

	//逐条累加各记录长度	， 找出需要保存的记录数
	fgOverflowed = false;
	dwValidRecordCount = 0;
	for (std::deque<RECORDNODE*>::iterator it = m_dqRecords.begin();
         it != m_dqRecords.end();
         it++)
    {
        DWORD32 dwRecordSize = 0;

		dwRecordSize = (sizeof(m_dwTRICKNUMBER)
			    + sizeof((*it)->dwYear) + sizeof((*it)->dwMonth) + sizeof((*it)->dwDay) + sizeof((*it)->dwHour)
				+ sizeof((*it)->dwDataStartID) + sizeof((*it)->dwDataEndID)
				+ sizeof((*it)->dwFileStartIndex) + sizeof((*it)->dwFileEndIndex)
				+ sizeof((*it)->dwInfoLen) + (*it)->dqInfo.size() * sizeof(STORAGEINFO));

		//如果加上这条记录的长度就溢出
		if (dwTableSize + dwRecordSize > dwLen)
		{
		    fgOverflowed = true;
		    break;
		}
		else
		{
			dwTableSize += dwRecordSize;
			dwValidRecordCount++;
		}
    }

    //有溢出则从最旧的开始清除记录，且回收存储块
	if (fgOverflowed)
	{
	    HV_Trace(5, "[FileQueueTable] Warning: Table's size exceeds buffer's, delete old records in FileQueue Table until the buffer can save the table!!\n",  __LINE__);

	    while ( m_dqRecords.size() > dwValidRecordCount)
	    {
	        if (0 >= m_dqRecords.size())
        	{
        	    HV_Trace(5, "[FileQueueTable] Crashed at L%d!!\n",  __LINE__);
				return E_FAIL;
        	}

	        RECORDNODE * prnNode = m_dqRecords.back();

	        while (prnNode->dqInfo.size() > 0)
	        {
	            STORAGEINFO* psiItem = prnNode->dqInfo.front();

				//回收
				m_dwRemained += psiItem->wBlockCnt * psiItem->wRepeatTimes;
	            prnNode->dqInfo.pop_front();
	            delete psiItem;
	        }

	        m_dqRecords.pop_back();
	        delete prnNode;
	    }
	}

    // clear
    memset(pbAddr, 0, sizeof(BYTE8)*dwLen);

    //开始存储
    memcpy(pbAddr, &m_dwTotalBlockCount, sizeof(m_dwTotalBlockCount));
    //pbAddr += sizeof(m_dwTotalBlockCount);
    SHIFTADDR(pbAddr, sizeof(m_dwTotalBlockCount), pbBuf, dwLen);

    memcpy(pbAddr, &m_dwRemained, sizeof(m_dwRemained));
    //pbAddr += sizeof(m_dwRemained);
    SHIFTADDR(pbAddr, sizeof(m_dwRemained), pbBuf, dwLen);

    memcpy(pbAddr, &m_dwOffset, sizeof(m_dwOffset));
    //pbAddr += sizeof(m_dwOffset);
    SHIFTADDR(pbAddr, sizeof(m_dwOffset), pbBuf, dwLen);


    //从表头开始逐条保存记录
    for (std::deque<RECORDNODE*>::iterator it = m_dqRecords.begin();
         it != m_dqRecords.end();
         it++)
    {
        DWORD32 dwTrickNum = m_dwTRICKNUMBER;
        memcpy(pbAddr, &dwTrickNum, sizeof(dwTrickNum));
        //pbAddr += sizeof(dwTrickNum);
        SHIFTADDR(pbAddr, sizeof(dwTrickNum), pbBuf, dwLen);

        memcpy(pbAddr, &(*it)->dwYear, sizeof((*it)->dwYear));
        //pbAddr += sizeof((*it)->dwYear);
        SHIFTADDR(pbAddr, sizeof((*it)->dwYear), pbBuf, dwLen);

        memcpy(pbAddr,    &(*it)->dwMonth, sizeof((*it)->dwMonth));
        //pbAddr += sizeof((*it)->dwMonth);
        SHIFTADDR(pbAddr, sizeof((*it)->dwMonth), pbBuf, dwLen);

        memcpy(pbAddr,    &(*it)->dwDay, sizeof((*it)->dwDay));
        //pbAddr += sizeof((*it)->dwDay);
        SHIFTADDR(pbAddr, sizeof((*it)->dwDay), pbBuf, dwLen);

        memcpy(pbAddr,    &(*it)->dwHour, sizeof((*it)->dwHour));
        //pbAddr += sizeof((*it)->dwHour);
        SHIFTADDR(pbAddr, sizeof((*it)->dwHour), pbBuf, dwLen);

        memcpy(pbAddr,    &(*it)->dwDataStartID, sizeof((*it)->dwDataStartID));
        //pbAddr += sizeof((*it)->dwDataStartID);
        SHIFTADDR(pbAddr, sizeof((*it)->dwDataStartID), pbBuf, dwLen);

        memcpy(pbAddr,    &(*it)->dwDataEndID, sizeof((*it)->dwDataEndID));
        //pbAddr += sizeof((*it)->dwDataEndID);
        SHIFTADDR(pbAddr, sizeof((*it)->dwDataEndID), pbBuf, dwLen);

        memcpy(pbAddr,    &(*it)->dwFileStartIndex, sizeof((*it)->dwFileStartIndex));
        //pbAddr += sizeof((*it)->dwFileStartIndex);
        SHIFTADDR(pbAddr, sizeof((*it)->dwFileStartIndex), pbBuf, dwLen);

        memcpy(pbAddr,    &(*it)->dwFileEndIndex, sizeof((*it)->dwFileEndIndex));
        //pbAddr += sizeof((*it)->dwFileEndIndex);
        SHIFTADDR(pbAddr, sizeof((*it)->dwFileEndIndex), pbBuf, dwLen);

        memcpy(pbAddr,    &(*it)->dwInfoLen, sizeof((*it)->dwInfoLen));
        //pbAddr += sizeof((*it)->dwInfoLen);
        SHIFTADDR(pbAddr, sizeof((*it)->dwInfoLen), pbBuf, dwLen);

        for (std::deque<STORAGEINFO*>::iterator itr = (*it)->dqInfo.begin();
             itr != (*it)->dqInfo.end();
             itr ++)
        {
            memcpy(pbAddr,    &(*itr)->wRepeatTimes, sizeof((*itr)->wRepeatTimes));
            //pbAddr += sizeof((*itr)->wRepeatTimes);
            SHIFTADDR(pbAddr, sizeof((*itr)->wRepeatTimes), pbBuf, dwLen);

            memcpy(pbAddr,    &(*itr)->wBlockCnt, sizeof((*itr)->wBlockCnt));
            //pbAddr += sizeof((*itr)->wBlockCnt);
            SHIFTADDR(pbAddr, sizeof((*itr)->wBlockCnt), pbBuf, dwLen);
        }
    }

    return S_OK;
}



HRESULT CFileQueueTable::PrintTable()
{
    HV_Trace(5, "-Total File Count = %d\n", m_dwTotalBlockCount);
    HV_Trace(5, "-Avail File Count = %d\n", m_dwRemained);
    HV_Trace(5, "-Offset = %d\n", m_dwOffset);

    char szMsg[256];
    szMsg[0] = 0;
    for (std::deque<RECORDNODE*>::iterator it = m_dqRecords.begin();
         it != m_dqRecords.end();
         it++)
    {
        HV_Trace(5, "%4d/%02d/%02d %02d Block: %d ~ %d (%d) CarID: %d ~ %d (%d)\n",
            (*it)->dwYear, (*it)->dwMonth, (*it)->dwDay, (*it)->dwHour,
            (*it)->dwDataStartID, (*it)->dwDataEndID,
            (*it)->dwDataEndID - (*it)->dwDataStartID + 1,
            (*it)->dwFileStartIndex, (*it)->dwFileEndIndex,
            (*it)->dwFileEndIndex - (*it)->dwFileStartIndex + 1);
        /*
        HV_Trace(5, "Mapping Table:\n");
        szMsg[0] = 0;
        if( (*it)->dqInfo.size() < 32 )
        {
            for (std::deque<STORAGEINFO*>::iterator itr = (*it)->dqInfo.begin();
                 itr != (*it)->dqInfo.end();
                 itr ++)
            {
                char szTemp[32];
                sprintf( szTemp, "[%d][%d]-", (*itr)->wRepeatTimes, (*itr)->wBlockCnt );
                if( strlen(szMsg) == 0 )
                {
                    strcpy( szMsg, szTemp );
                }
                else
                {
                    strcat( szMsg, szTemp );
                }
            }
        }
        else
        {
            sprintf( szMsg, "[]info szie:%d.[]", (*it)->dqInfo.size() );
        }
        strcat( szMsg, "[][]\n" );
        HV_Trace(5, szMsg);
        */
    }

    return S_OK;
}




HRESULT CFileQueueTable::Clear()
{
    //从旧到新逐条清除
    while ( m_dqRecords.size() > 0)
    {
        RECORDNODE * prnNode = m_dqRecords.back();

        while (prnNode->dqInfo.size() > 0)
        {
            STORAGEINFO* psiItem = prnNode->dqInfo.front();
            prnNode->dqInfo.pop_front();
            delete psiItem;
        }

        m_dqRecords.pop_back();
        delete prnNode;
    }

    m_dwRemained = m_dwTotalBlockCount;
    m_dwOffset = 0;
    //m_dwRecordLen = 0;

    return S_OK;
}



