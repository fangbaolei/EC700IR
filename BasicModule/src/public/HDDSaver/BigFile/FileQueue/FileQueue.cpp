/**
* @file FileQueue.cpp
* @brief implementation of CFileQueue
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-21
* @version 1.0
*/

#include "FileQueue.h"
#include "ResultTable.h"
#include "VideoTable.h"


#define PRINT SW_TRACE_DEBUG


CFileQueue::CFileQueue()
{
    m_pTable = NULL;    
    m_fqType = FQ_TYPE_CNT;
    
    m_fgInited = FALSE;
}


CFileQueue::~CFileQueue()
{
    if (NULL != m_pTable)
    {
        delete m_pTable;
        m_pTable = NULL;
    }
}

HRESULT CFileQueue::Initialize(
    DWORD dwTotalBlockCount, 
    DWORD dwOneBlockSize, 
    FQ_TYPE fqType)
{
    HRESULT hr = S_OK;
    
    if (m_fgInited)
    {
        return hr;
    }

    // check input args
    if (0 == dwTotalBlockCount 
        || 0 == dwOneBlockSize
        || FQ_TYPE_CNT <= fqType)
    {
        return E_INVALIDARG;
    }
     
        
    m_fqType = fqType; // Result or Video
    switch (m_fqType)
    {
    case FQ_TYPE_RESULT:
        {
            m_pTable = new CResultTable(dwTotalBlockCount, dwOneBlockSize);
        }
        break;
    case FQ_TYPE_VIDEO:
        {
            m_pTable = new CVideoTable(dwTotalBlockCount, dwOneBlockSize);
        }
        break;
    default:
        /* no way */
        return E_INVALIDARG;
        /* no break here*/
    }

    
    if (NULL == m_pTable)
    {
        return E_OUTOFMEMORY;
    }
    

    m_fgInited = TRUE;
    
    return hr;
}


HRESULT CFileQueue::Load(BYTE * pbBuf, const DWORD dwLen)
{
    HRESULT hr = S_OK;

    if (!m_fgInited)
    {
        return E_NOTIMPL;
    }

    //if (NULL == pbBuf || 0 ==dwLen)
    //{
        //特例，用于初始化表，不是参数错误
    //}

    hr = m_pTable->LoadTable(pbBuf, dwLen);

    return hr;
}



HRESULT CFileQueue::Commit(BYTE * pbBuf, const DWORD dwLen)
{
    HRESULT hr = S_OK;

    if (!m_fgInited)
    {
        return E_NOTIMPL;
    }

    if (NULL == pbBuf || 0 ==dwLen)
    {
        return E_INVALIDARG;
    }

    hr = m_pTable->SaveTable(pbBuf, dwLen);

    return hr;
}



HRESULT CFileQueue::Write(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
               DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
               DWORD dwSize, DWORD &dwCarID, DWORD &dwStartIndex, DWORD &dwEndIndex)
{
    if (!m_fgInited)
    {
        return E_NOTIMPL;
    }
    
    return m_pTable->WriteTable(dwYear, dwMonth, dwDay, 
               dwHour, dwMinute, dwSecond, 
               dwSize, dwCarID, dwStartIndex, dwEndIndex);
}




HRESULT CFileQueue::Read(DWORD dwYear, DWORD dwMonth, DWORD dwDay, 
                   DWORD dwHour, DWORD dwMinute, DWORD dwSecond, 
                   DWORD dwCarID, DWORD &dwStartIndex, DWORD &dwEndIndex)
{
    if (!m_fgInited)
    {
        return E_NOTIMPL;
    }
    
    return m_pTable->ReadTable(dwYear, dwMonth, dwDay, 
                   dwHour, dwMinute, dwSecond, 
                   dwCarID, dwStartIndex, dwEndIndex);
}



DWORD CFileQueue::GetFileCount(DWORD dwYear, 
    DWORD dwMonth, 
    DWORD dwDay, 
    DWORD dwHour)
{
    if (!m_fgInited)
    {
        return E_NOTIMPL;
    }
    
    return m_pTable->GetFileCount(dwYear, dwMonth, dwDay, dwHour);
}


HRESULT CFileQueue::Print()
{
    if (!m_fgInited)
    {
        return E_NOTIMPL;
    }

    switch (m_fqType)
    {
    case FQ_TYPE_RESULT:
        {
            PRINT("---- Result Table Info ----\n");
        }
        break;
    case FQ_TYPE_VIDEO:
        {
            PRINT("---- Video Table Info ----\n");
        }
        break;
    default:
        {
            /* no way */
            PRINT("[FileQueue] Invalid FileQueue Type!\n");
            return E_INVALIDARG;
        }        
        /* no break here*/
    }    
    
    return m_pTable->PrintTable();
}








