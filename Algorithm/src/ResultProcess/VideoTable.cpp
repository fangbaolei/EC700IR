
#include "VideoTable.h"

CVideoTable::CVideoTable(
    DWORD32 dwTotalBlockCount,
    DWORD32 dwOneBlockSize)
{
    m_dwTRICKNUMBER = 0xEFDCCDFE; // 标志一条记录的开始

    m_dwTotalBlockCount = dwTotalBlockCount;
    m_dwOneBlockSize = dwOneBlockSize;

    //m_dwRecordLen = 0;

    Clear();
}

CVideoTable::~CVideoTable()
{
    Clear();
}



HRESULT CVideoTable::ReadTable(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay,
                               DWORD32 dwHour, DWORD32 dwMinute, DWORD32 dwSecond,
                               DWORD32 dwCarID, DWORD32 &dwStartIndex, DWORD32 &dwEndIndex)
{
    HRESULT hr = S_OK;
    DWORD32 dwStart=0, dwEnd=0, dwID=0, dwOffset=0, dwIndex = 0;
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
            dwIndex = dwMinute * 60 + dwSecond; // 转成索引

            if (dwIndex >= (*it)->dwFileStartIndex
                    && dwIndex <= (*it)->dwFileEndIndex)
            {
                dwID = dwIndex - (*it)->dwFileStartIndex;
                dwOffset = 0;

                for (std::deque<STORAGEINFO*>::iterator itr = (*it)->dqInfo.begin();
                        itr != (*it)->dqInfo.end();
                        itr ++)
                {
                    if (dwID+1 <= (*itr)->wRepeatTimes)
                    {
                        dwStart = (dwOffset + (*it)->dwDataStartID + dwID*(*itr)->wBlockCnt)% m_dwTotalBlockCount;
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
        HV_Trace(5, "[VideoTable] err: Record not found\n");
        return E_FAIL;
    }

}



HRESULT CVideoTable::WriteTable(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay,
                                DWORD32 dwHour, DWORD32 dwMinute, DWORD32 dwSecond,
                                DWORD32 dwSize, DWORD32 &dwCarID, DWORD32 &dwStartIndex, DWORD32 &dwEndIndex)
{
    HRESULT hr = S_OK;
    DWORD32 dwStart=0, dwEnd=0, dwID=0, dwOffset=0, dwCnt=0, dwIndex;
    bool fgFound = false;

    dwCnt = dwSize /(m_dwOneBlockSize+1) + 1;


    while ( dwCnt > m_dwRemained)
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

            HV_Trace(5, "<video>delete one bock:wRepeatTimes:%d,wBlockCnt:%d", psiItem->wRepeatTimes, psiItem->wBlockCnt);

            delete psiItem;
        }

        m_dqRecords.pop_back();

        HV_Trace(5, "<video>delete one node:date=[%04d-%02d-%02d %02d]block=[%d,%d],carid=[%d,%d]"
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
            dwIndex = dwMinute * 60 + dwSecond;

            //只有紧挨的数据才会放到同一条记录里面
            if (dwIndex == (*it)->dwFileEndIndex + 1)
            {
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
                        HV_Trace(5, "[VideoTable] Out of Memory!\n");
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
            else
            {
                //no need
            }

        }
    }

    if (!fgFound)
    {
        RECORDNODE * prnNode = new RECORDNODE;
        if (NULL == prnNode)
        {
            HV_Trace(5, "[VideoTable] err: Out of Memory\n");
            return E_OUTOFMEMORY;
        }

        prnNode->dwYear  = dwYear;
        prnNode->dwMonth = dwMonth;
        prnNode->dwDay   = dwDay;
        prnNode->dwHour  = dwHour;
        prnNode->dwFileStartIndex = dwMinute * 60 + dwSecond;
        prnNode->dwFileEndIndex   = dwMinute * 60 + dwSecond;


        STORAGEINFO * psiInfo = new STORAGEINFO;
        if (NULL == psiInfo)
        {
            HV_Trace(5, "[VideoTable] err: Out of Memory\n");
            return E_OUTOFMEMORY;
        }
        psiInfo->wBlockCnt = dwCnt;
        psiInfo->wRepeatTimes = 1;
        prnNode->dqInfo.push_back(psiInfo);
        prnNode->dwInfoLen = 1;

        prnNode->dwDataStartID = (m_dwOffset) % m_dwTotalBlockCount;
        prnNode->dwDataEndID = (prnNode->dwDataStartID + dwCnt - 1) % m_dwTotalBlockCount;

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
        HV_Trace(5, "[VideoTable] err: Record not found\n");
        return E_FAIL;
    }
}



DWORD32 CVideoTable::GetFileCount(DWORD32 dwYear,
                                  DWORD32 dwMonth,
                                  DWORD32 dwDay,
                                  DWORD32 dwHour)
{
    /* do nothing, return 0 directly */
    return 0;
}






