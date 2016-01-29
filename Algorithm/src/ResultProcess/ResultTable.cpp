#include "ResultTable.h"

CResultTable::CResultTable(
    DWORD32 dwTotalBlockCount,
    DWORD32 dwOneBlockSize)
{
    m_dwTRICKNUMBER = 0xEFDCCDFE;

    m_dwTotalBlockCount = dwTotalBlockCount;
    m_dwOneBlockSize = dwOneBlockSize;

    //m_dwRecordLen = 0;
    Clear();
}

CResultTable::~CResultTable()
{
    // clear
    Clear();
}



/*
HRESULT CResultTable::LoadTable(BYTE8* pbBuf, const DWORD32 dwLen)
{
    BYTE8 * pbRecord = NULL, * pbAddr = NULL;
    WORD16 wIdx;
    RECORDNODE * prnNode = NULL;
    DWORD32 dwTrickNum = 0;

    pbAddr = pbBuf;

    if (NULL == pbAddr)
    {
        m_dwRemained = m_dwTotalBlockCount;
        m_dwOffset = 0;
        //m_dwRecordLen = 0;


        return S_OK;
    }

    Clear(); // clear all the tables first

    memcpy(&m_dwTotalBlockCount, pbAddr, sizeof(m_dwTotalBlockCount));
    pbAddr += sizeof(m_dwTotalBlockCount);

    memcpy(&m_dwRemained, pbAddr, sizeof(m_dwRemained));
    pbAddr += sizeof(m_dwRemained);

    memcpy(&m_dwOffset, pbAddr, sizeof(m_dwOffset));
    pbAddr += sizeof(m_dwOffset);

    while (1)
    {
        memcpy(&dwTrickNum, pbAddr, sizeof(dwTrickNum));
        if (m_dwTRICKNUMBER == dwTrickNum)
        {
            if (pbAddr - pbBuf >= dwLen)
            {
                HV_Trace(5, "[ResultTable] Table size exceeds %d!!\n", dwLen);
                return E_FAIL;
            }

            prnNode = new RECORDNODE;
            if (NULL == prnNode)
            {
                // TODO: destroy dequeue first

                HV_Trace(5, "[VideoTable] Out of Memory!!\n");
                return E_OUTOFMEMORY;
            }

            pbAddr += sizeof(dwTrickNum); // for alignment
            memcpy(&prnNode->dwYear, pbAddr, sizeof(prnNode->dwYear));

            pbAddr += sizeof(prnNode->dwYear);
            memcpy(&prnNode->dwMonth, pbAddr, sizeof(prnNode->dwMonth));

            pbAddr += sizeof(prnNode->dwMonth);
            memcpy(&prnNode->dwDay, pbAddr, sizeof(prnNode->dwDay));

            pbAddr += sizeof(prnNode->dwDay);
            memcpy(&prnNode->dwHour, pbAddr, sizeof(prnNode->dwHour));

            pbAddr += sizeof(prnNode->dwHour);
            memcpy(&prnNode->dwDataStartID, pbAddr, sizeof(prnNode->dwDataStartID));

            pbAddr += sizeof(prnNode->dwDataStartID);
            memcpy(&prnNode->dwDataEndID, pbAddr, sizeof(prnNode->dwDataEndID));

            pbAddr += sizeof(prnNode->dwDataEndID);
            memcpy(&prnNode->dwFileStartIndex, pbAddr, sizeof(prnNode->dwFileStartIndex));

            pbAddr += sizeof(prnNode->dwFileStartIndex);
            memcpy(&prnNode->dwFileEndIndex, pbAddr, sizeof(prnNode->dwFileEndIndex));

            pbAddr += sizeof(prnNode->dwFileEndIndex);
            memcpy(&prnNode->dwInfoLen, pbAddr, sizeof(prnNode->dwInfoLen));

            pbAddr += sizeof(prnNode->dwInfoLen);
            for (wIdx = 0; wIdx < prnNode->dwInfoLen; wIdx++)
            {
                STORAGEINFO * psiInfo = new STORAGEINFO;

                memcpy(&psiInfo->wRepeatTimes, pbAddr, sizeof(psiInfo->wRepeatTimes));
                pbAddr += sizeof(psiInfo->wRepeatTimes);
                memcpy(&psiInfo->wBlockCnt, pbAddr, sizeof(psiInfo->wBlockCnt));
                pbAddr += sizeof(psiInfo->wBlockCnt);

                prnNode->dqInfo.push_back(psiInfo);
            }

            m_dqRecords.push_back(prnNode);
            //m_dwRecordLen ++;
        }
        else
        {
            return S_OK; // finished
        }
    }

    return S_OK;
}
*/


/*
HRESULT CResultTable::SaveTable(BYTE8* pbBuf, const DWORD32 dwLen)
{
    BYTE8 * pbAddr = NULL;

    pbAddr = pbBuf;

    // clear
    memset(pbAddr, 0, sizeof(BYTE8)*dwLen);

    memcpy(pbAddr, &m_dwTotalBlockCount, sizeof(m_dwTotalBlockCount));
    pbAddr += sizeof(m_dwTotalBlockCount);

    memcpy(pbAddr, &m_dwRemained, sizeof(m_dwRemained));
    pbAddr += sizeof(m_dwRemained);

    memcpy(pbAddr, &m_dwOffset, sizeof(m_dwOffset));
    pbAddr += sizeof(m_dwOffset);


    for (std::deque<RECORDNODE*>::iterator it = m_dqRecords.begin();
         it != m_dqRecords.end();
         it++)
     {
         //*((DWORD32*)pbAddr) = m_dwTRICKNUMBER;
        DWORD32 dwTrickNum = m_dwTRICKNUMBER;
         memcpy(pbAddr, &dwTrickNum, sizeof(dwTrickNum));
        pbAddr += 4;

        memcpy(pbAddr, &(*it)->dwYear, sizeof((*it)->dwYear));
        pbAddr += sizeof((*it)->dwYear);

        memcpy(pbAddr,  &(*it)->dwMonth, sizeof((*it)->dwMonth));
        pbAddr += sizeof((*it)->dwMonth);

        memcpy(pbAddr,  &(*it)->dwDay, sizeof((*it)->dwDay));
        pbAddr += sizeof((*it)->dwDay);

        memcpy(pbAddr,  &(*it)->dwHour, sizeof((*it)->dwHour));
        pbAddr += sizeof((*it)->dwHour);

        memcpy(pbAddr,  &(*it)->dwDataStartID, sizeof((*it)->dwDataStartID));
        pbAddr += sizeof((*it)->dwDataStartID);

        memcpy(pbAddr,  &(*it)->dwDataEndID, sizeof((*it)->dwDataEndID));
        pbAddr += sizeof((*it)->dwDataEndID);

        memcpy(pbAddr,  &(*it)->dwFileStartIndex, sizeof((*it)->dwFileStartIndex));
        pbAddr += sizeof((*it)->dwFileStartIndex);

        memcpy(pbAddr,  &(*it)->dwFileEndIndex, sizeof((*it)->dwFileEndIndex));
        pbAddr += sizeof((*it)->dwFileEndIndex);

        memcpy(pbAddr,  &(*it)->dwInfoLen, sizeof((*it)->dwInfoLen));
        pbAddr += sizeof((*it)->dwInfoLen);

        for (std::deque<STORAGEINFO*>::iterator itr = (*it)->dqInfo.begin();
             itr != (*it)->dqInfo.end();
             itr ++)
        {
            memcpy(pbAddr,  &(*itr)->wRepeatTimes, sizeof((*itr)->wRepeatTimes));
            pbAddr += sizeof((*itr)->wRepeatTimes);

            memcpy(pbAddr,  &(*itr)->wBlockCnt, sizeof((*itr)->wBlockCnt));
            pbAddr += sizeof((*itr)->wBlockCnt);
        }
     }

    return S_OK;
}
*/



HRESULT CResultTable::ReadTable(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay,
                                DWORD32 dwHour, DWORD32 dwMinute, DWORD32 dwSecond,
                                DWORD32 dwCarID, DWORD32 &dwStartIndex, DWORD32 &dwEndIndex)
{
    HRESULT hr = S_OK;
    DWORD32 dwStart=0, dwEnd=0, dwID=0, dwOffset=0;
    bool fgFound = false;

    //从新到旧逐条查找
    for (std::deque<RECORDNODE*>::iterator it = m_dqRecords.begin();
            it != m_dqRecords.end();
            it++)
    {
        if ((*it)->dwYear == dwYear
                && (*it)->dwMonth == dwMonth
                && (*it)->dwDay == dwDay
                && (*it)->dwHour == dwHour)
        {
            if (dwCarID >= (*it)->dwFileStartIndex
                    && dwCarID <= (*it)->dwFileEndIndex)
            {
                dwID = (dwCarID - (*it)->dwFileStartIndex);
                dwOffset = 0;

                for (std::deque<STORAGEINFO*>::iterator itr = (*it)->dqInfo.begin();
                        itr != (*it)->dqInfo.end();
                        itr ++)
                {
                    if (dwID+1 <= (*itr)->wRepeatTimes) //找到
                    {
                        dwStart = (dwOffset + (*it)->dwDataStartID + dwID * (*itr)->wBlockCnt)% m_dwTotalBlockCount;
                        dwEnd = (dwStart + (*itr)->wBlockCnt - 1) % m_dwTotalBlockCount;

                        fgFound = true;
                        break;
                    }
                    else
                    {
                        dwID -= (*itr)->wRepeatTimes;
                        dwOffset += ((*itr)->wRepeatTimes * (*itr)->wBlockCnt);
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
        HV_Trace(5, "[ResultTable] err: Record not found\n");
        return E_FAIL;
    }

}



HRESULT CResultTable::WriteTable(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay,
                                 DWORD32 dwHour, DWORD32 dwMinute, DWORD32 dwSecond,
                                 DWORD32 dwSize, DWORD32 &dwCarID, DWORD32 &dwStartIndex, DWORD32 &dwEndIndex)
{
    HRESULT hr = S_OK;
    DWORD32 dwStart=0, dwEnd=0, dwID=0, dwOffset=0, dwCnt=0;
    bool fgFound = false;
    DWORD32 dwNewRecordFileBeginIndex = 0;

    //计算所占的块数
    dwCnt = dwSize /(m_dwOneBlockSize+1) + 1;

    while (dwCnt > m_dwRemained)
    {
        if (m_dwRemained >= m_dwTotalBlockCount)
        {
            HV_Trace(5, "[ResultTable] err: m_dwRemained >= m_dwTotalBlockCount, overflowed!\n");
            return E_FAIL;
        }

        // 取最旧的记录删除，回收空间直至够用
        RECORDNODE * prnNode = m_dqRecords.back();

        //m_dwRemained += (m_dwTotalBlockCount + prnNode->dwDataEndID - prnNode->dwDataStartID + 1) % m_dwTotalBlockCount;
        if (prnNode->dwDataEndID >= prnNode->dwDataStartID)
        {
            m_dwRemained += (prnNode->dwDataEndID - prnNode->dwDataStartID + 1);
        }
        else
        {
            m_dwRemained += (m_dwTotalBlockCount + prnNode->dwDataEndID - prnNode->dwDataStartID + 1);
        }

        while (prnNode->dqInfo.size() > 0)
        {
            STORAGEINFO* psiItem = prnNode->dqInfo.front();
            prnNode->dqInfo.pop_front();

            HV_Trace(5, "<result>delete one bock:wRepeatTimes:%d,wBlockCnt:%d", psiItem->wRepeatTimes, psiItem->wBlockCnt);

            delete psiItem;
        }

        m_dqRecords.pop_back();

        HV_Trace(5, "<result>delete one node:date=[%04d-%02d-%02d %02d]block=[%d,%d],carid=[%d,%d]"
                 ,prnNode->dwYear
                 ,prnNode->dwMonth
                 ,prnNode->dwDay
                 ,prnNode->dwHour
                 ,prnNode->dwDataStartID
                 ,prnNode->dwDataEndID
                 ,prnNode->dwFileStartIndex
                 ,prnNode->dwFileEndIndex);

        delete prnNode;
    }


    //把最新的记录加在队列的最前面
    for (std::deque<RECORDNODE*>::iterator it = m_dqRecords.begin();
            it != m_dqRecords.end();
            it++)
    {
        if ((*it)->dwYear == dwYear
                && (*it)->dwMonth == dwMonth
                && (*it)->dwDay == dwDay
                && (*it)->dwHour == dwHour)
        {
            //特殊情况:
            if (m_dwOffset != ((*it)->dwDataEndID+1) % m_dwTotalBlockCount)
            {
                if (dwNewRecordFileBeginIndex < (*it)->dwFileEndIndex)
                {
                    dwNewRecordFileBeginIndex = (*it)->dwFileEndIndex + 1;
                }
                continue;
            }


            STORAGEINFO * psiInfo = (*it)->dqInfo.back();
            if (psiInfo->wBlockCnt == dwCnt)
            {
                psiInfo->wRepeatTimes ++;
            }
            else
            {
                STORAGEINFO * psiInfo = new STORAGEINFO;
                if (NULL == psiInfo)
                {
                    HV_Trace(5, "[ResultTable] Out of Memory!\n");
                    return E_OUTOFMEMORY;
                }

                psiInfo->wRepeatTimes = 1;
                psiInfo->wBlockCnt = dwCnt;

                (*it)->dqInfo.push_back(psiInfo);
                (*it)->dwInfoLen ++;
            }

            dwStart = (m_dwOffset) % m_dwTotalBlockCount;
            (*it)->dwDataEndID = (dwStart + dwCnt - 1) % m_dwTotalBlockCount;
            dwEnd = (*it)->dwDataEndID;
            m_dwOffset = ((*it)->dwDataEndID + 1) % m_dwTotalBlockCount;

            m_dwRemained -= dwCnt;

            (*it)->dwFileEndIndex ++;
            dwCarID = (*it)->dwFileEndIndex;

            fgFound = true;
            break;
        }
    }

    if (!fgFound)
    {
        RECORDNODE * prnNode = new RECORDNODE;
        if (NULL == prnNode)
        {
            HV_Trace(5, "[ResultTable] err: Out of Memory\n");
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


        STORAGEINFO * psiInfo = new STORAGEINFO;
        if (NULL == psiInfo)
        {
            HV_Trace(5, "[ResultTable] err: Out of Memory\n");
            return E_OUTOFMEMORY;
        }
        psiInfo->wBlockCnt = dwCnt;
        psiInfo->wRepeatTimes = 1;
        prnNode->dqInfo.push_back(psiInfo);
        prnNode->dwInfoLen = 1;

        dwCarID = prnNode->dwFileEndIndex;

        m_dwOffset = (prnNode->dwDataEndID + 1) % m_dwTotalBlockCount;
        m_dwRemained -= dwCnt;

        dwStart = prnNode->dwDataStartID;
        dwEnd = prnNode->dwDataEndID;

        m_dqRecords.push_front(prnNode);
        fgFound = true;
    }


    if (fgFound)
    {
        dwStartIndex = dwStart;
        dwEndIndex = dwEnd;

        return S_OK;
    }
    else
    {
        HV_Trace(5, "[ResultTable] err: Record not found\n");
        return E_FAIL;
    }
}



DWORD32 CResultTable::GetFileCount(DWORD32 dwYear,
                                   DWORD32 dwMonth,
                                   DWORD32 dwDay,
                                   DWORD32 dwHour)
{
    DWORD32 dwFileCount = 0;

    for (std::deque<RECORDNODE*>::iterator it = m_dqRecords.begin();
            it != m_dqRecords.end();
            it++)
    {
        if ((*it)->dwYear == dwYear
                && (*it)->dwMonth == dwMonth
                && (*it)->dwDay == dwDay
                && (*it)->dwHour == dwHour)
        {
            dwFileCount += (*it)->dwFileEndIndex - (*it)->dwFileStartIndex + 1; // must +1
        }
    }

    return dwFileCount;
}



/*
HRESULT CResultTable::PrintTable()
{
    HV_Trace(5, "---- Result Table Info ----\n");
    HV_Trace(5, "-Total File Count = %d\n", m_dwTotalBlockCount);
    HV_Trace(5, "-Avail File Count = %d\n", m_dwRemained);
    HV_Trace(5, "-Offset = %d\n", m_dwOffset);

    for (std::deque<RECORDNODE*>::iterator it = m_dqRecords.begin();
         it != m_dqRecords.end();
         it++)
     {
         HV_Trace(5, "%4d/%02d/%02d %02d\nBlock: %d ~ %d (%d)\nCarID: %d ~ %d (%d)\n",
            (*it)->dwYear, (*it)->dwMonth, (*it)->dwDay, (*it)->dwHour,
            (*it)->dwDataStartID, (*it)->dwDataEndID,
            (*it)->dwDataEndID - (*it)->dwDataStartID + 1,
            (*it)->dwFileStartIndex, (*it)->dwFileEndIndex,
            (*it)->dwFileEndIndex - (*it)->dwFileStartIndex + 1);
        HV_Trace(5, "Mapping Table:\n");
        for (std::deque<STORAGEINFO*>::iterator itr = (*it)->dqInfo.begin();
             itr != (*it)->dqInfo.end();
             itr ++)
         {
             HV_Trace(5, "[%d][%d]-", (*itr)->wRepeatTimes, (*itr)->wBlockCnt);
         }
         HV_Trace(5, "[][]\n");
     }

    return S_OK;

}
*/



/*
HRESULT CResultTable::Clear()
{
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

    m_dwRemained = 0;
    m_dwOffset = 0;
    //m_dwRecordLen = 0;

    return S_OK;
}
*/


