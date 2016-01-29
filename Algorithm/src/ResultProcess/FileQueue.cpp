
#include "FileQueue.h"
#include "ResultTable.h"
#include "VideoTable.h"

CFileQueue::CFileQueue()
{
    m_pTable = NULL;    
    m_fqType = FQ_TYPE_CNT;
    
    m_fgInited = false;
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
    DWORD32 dwTotalBlockCount, 
    DWORD32 dwOneBlockSize, 
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
    

    m_fgInited = true;
    
    return hr;
}


HRESULT CFileQueue::Load(BYTE8 * pbBuf, const DWORD32 dwLen)
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



HRESULT CFileQueue::Commit(BYTE8 * pbBuf, const DWORD32 dwLen)
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



HRESULT CFileQueue::Write(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay, 
               DWORD32 dwHour, DWORD32 dwMinute, DWORD32 dwSecond, 
               DWORD32 dwSize, DWORD32 &dwCarID, DWORD32 &dwStartIndex, DWORD32 &dwEndIndex)
{
    if (!m_fgInited)
    {
        return E_NOTIMPL;
    }
    
    return m_pTable->WriteTable(dwYear, dwMonth, dwDay, 
               dwHour, dwMinute, dwSecond, 
               dwSize, dwCarID, dwStartIndex, dwEndIndex);
}




HRESULT CFileQueue::Read(DWORD32 dwYear, DWORD32 dwMonth, DWORD32 dwDay, 
                   DWORD32 dwHour, DWORD32 dwMinute, DWORD32 dwSecond, 
                   DWORD32 dwCarID, DWORD32 &dwStartIndex, DWORD32 &dwEndIndex)
{
    if (!m_fgInited)
    {
        return E_NOTIMPL;
    }
    
    return m_pTable->ReadTable(dwYear, dwMonth, dwDay, 
                   dwHour, dwMinute, dwSecond, 
                   dwCarID, dwStartIndex, dwEndIndex);
}



DWORD32 CFileQueue::GetFileCount(DWORD32 dwYear, 
    DWORD32 dwMonth, 
    DWORD32 dwDay, 
    DWORD32 dwHour)
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
            HV_Trace(5, "---- Result Table Info ----\n");
        }
        break;
    case FQ_TYPE_VIDEO:
        {
            HV_Trace(5, "---- Video Table Info ----\n");
        }
        break;
    default:
        {
            /* no way */
            HV_Trace(5, "[FileQueue] Invalid FileQueue Type!\n");
            return E_INVALIDARG;
        }        
        /* no break here*/
    }    
    
    return m_pTable->PrintTable();
}






