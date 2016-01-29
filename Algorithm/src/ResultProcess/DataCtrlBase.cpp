#include "DataCtrlBase.h"

CCarLeftThread::CCarLeftThread(IDataCtrl *pDataCtrl)
{
    m_dwLastTick = GetSystemTick();
    m_pDataCtrl = pDataCtrl;
    CreateSemaphore(&m_hSemLock, 1, 1);
    CreateSemaphore(&m_hSemResultCnt, 0, MAX_CARLEFT_LIST_COUNT);
}

CCarLeftThread::~CCarLeftThread()
{
    Stop(-1);
    DestroySemaphore(&m_hSemLock);
    DestroySemaphore(&m_hSemResultCnt);
}

HRESULT CCarLeftThread::Run(void *pvParamter)
{
    CARLEFT_INFO_STRUCT cCarLeft;

    if (m_pDataCtrl == NULL)
    {
        return E_POINTER;
    }

    DWORD32 dwLastSendTick = 0;
    while (!m_fExit)
    {
        m_dwLastTick = GetSystemTick();

        if (0 != SemPend(&m_hSemResultCnt, 1000) || S_OK != GetOneResult(&cCarLeft))
        {
            continue;
        }
        DWORD32 ms = GetSystemTick() - dwLastSendTick;
        if(ms < 500)
        {
            HV_Sleep(500 - ms);
        }

        m_pDataCtrl->CarLeft(&cCarLeft, NULL);
        SAFE_RELEASE(cCarLeft.cCoreResult.cResultImg.pimgPlate);
        SAFE_RELEASE(cCarLeft.cCoreResult.cResultImg.pimgBestSnapShot);
        SAFE_RELEASE(cCarLeft.cCoreResult.cResultImg.pimgLastSnapShot);
        SAFE_RELEASE(cCarLeft.cCoreResult.cResultImg.pimgBeginCapture);
        SAFE_RELEASE(cCarLeft.cCoreResult.cResultImg.pimgBestCapture);
        SAFE_RELEASE(cCarLeft.cCoreResult.cResultImg.pimgLastCapture);
        SAFE_RELEASE(cCarLeft.cCoreResult.cResultImg.pimgPlateBin);

        dwLastSendTick = GetSystemTick();
    }

    return S_OK;
}

HRESULT CCarLeftThread::PutOneResult(CARLEFT_INFO_STRUCT cCarLeftInfo)
{
    DWORD32 dwResult = 0;
    SemPend(&m_hSemLock);
    if (m_cCarLeftList.GetSize() >= MAX_CARLEFT_LIST_COUNT)
    {
        dwResult = 1;
    }
    //必须要等到对方处理才能返回，否则会丢结果
    else if (0 != SemPost(&m_hSemResultCnt, 1))
    {
        dwResult = 2;
    }
    if (dwResult != 0)
    {
        if (dwResult == 2)
        {
            HV_Trace(5, "PutOneResult: semaphore post failed!");
        }
        else
        {
            HV_Trace(5, "PutOneResult: carleft list is full, put one result failed!");
        }
        SAFE_RELEASE(cCarLeftInfo.cCoreResult.cResultImg.pimgPlate);
        SAFE_RELEASE(cCarLeftInfo.cCoreResult.cResultImg.pimgBestSnapShot);
        SAFE_RELEASE(cCarLeftInfo.cCoreResult.cResultImg.pimgLastSnapShot);
        SAFE_RELEASE(cCarLeftInfo.cCoreResult.cResultImg.pimgBeginCapture);
        SAFE_RELEASE(cCarLeftInfo.cCoreResult.cResultImg.pimgBestCapture);
        SAFE_RELEASE(cCarLeftInfo.cCoreResult.cResultImg.pimgLastCapture);
        SAFE_RELEASE(cCarLeftInfo.cCoreResult.cResultImg.pimgPlateBin);
        SemPost(&m_hSemLock);
        return E_FAIL;
    }
    m_cCarLeftList.AddTail(cCarLeftInfo);
    SemPost(&m_hSemLock);
    return S_OK;
}

HRESULT CCarLeftThread::GetOneResult(CARLEFT_INFO_STRUCT *pCarLeftInfo)
{
    HRESULT hrResult = E_FAIL;
    SemPend(&m_hSemLock);
    if (m_cCarLeftList.GetSize() > 0)
    {
        (*pCarLeftInfo) = m_cCarLeftList.RemoveHead();
        hrResult = S_OK;
    }
    SemPost(&m_hSemLock);
    return hrResult;
}

HRESULT CCarLeftThread::IsThreadOK()
{
    DWORD32 dwCurTick = GetSystemTick();
    if (dwCurTick < m_dwLastTick)
    {
        return S_OK;
    }
    if (m_dwLastTick != 0 && dwCurTick - m_dwLastTick > 10000)
    {
        HV_Trace(5, "Carleft thread timeout! Curtick = %d, lasttick = %d.", dwCurTick, m_dwLastTick);
        return E_FAIL;
    }
    return S_OK;
}
