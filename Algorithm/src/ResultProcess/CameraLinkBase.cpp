// 该文件编码格式必须为WINDOWS-936格式

#include "CameraLinkBase.h"
#include "slw_dev.h"

//--------------=============== CCameraLinkCtrlBase ==================------------------

CCameraLinkCtrlBase::CCameraLinkCtrlBase(const WORD16 wPort, const int iMaxLinkCount)
        : m_wPort(wPort)
        , m_iMaxLinkCount(iMaxLinkCount)
        , m_fAutoLinkEnable(FALSE)
        , m_fMTUSetEnable(FALSE)
        , m_iCameraType(0)
        , m_iSaveSafeType(1)
{
    m_hSocket = INVALID_SOCKET;
}

CCameraLinkCtrlBase::~CCameraLinkCtrlBase()
{
    Stop(-1);
    if ( INVALID_SOCKET != m_hSocket )
    {
        CloseSocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }
}

int CCameraLinkCtrlBase::GetLinkCount()
{
    return 0;
}

int CCameraLinkCtrlBase::DeleteFreeLink()
{
    return 0;
}

HRESULT CCameraLinkCtrlBase::ClearLink()
{
    return E_NOTIMPL;
}

HRESULT CCameraLinkCtrlBase::CreateLinkThread()
{
    return E_NOTIMPL;
}

HRESULT CCameraLinkCtrlBase::SetLink(HV_SOCKET_HANDLE& hSocket)
{
    return E_NOTIMPL;
}
HRESULT CCameraLinkCtrlBase::StartAutoLink()
{
    return E_NOTIMPL;
}
HRESULT CCameraLinkCtrlBase::StopAutoLink()
{
    return E_NOTIMPL;
}

// 设置相机类型
HRESULT CCameraLinkCtrlBase::SetCameraType(int iCameraType)
{
    m_iCameraType = iCameraType;
    return S_OK;
}

HRESULT CCameraLinkCtrlBase::EnableAutoLink(bool fEnable)
{
    if(fEnable)
    {
        m_iMaxLinkCount--;
    }

    m_fAutoLinkEnable = fEnable;

    return S_OK;
}

HRESULT CCameraLinkCtrlBase::EnableMTUSet(bool fEnable)
{
    m_fMTUSetEnable = fEnable;
    return S_OK;
}

HRESULT CCameraLinkCtrlBase::SetSaveSafeType(const int iSaveSafeType)
{
    m_iSaveSafeType = iSaveSafeType;
    return S_OK;
}


HRESULT CCameraLinkCtrlBase::Run(void* pvParam)
{
    HV_SOCKET_HANDLE hNewSocket = INVALID_SOCKET;
    int iWaitMs = 4000;
    int iTimeOutMs = 4000;
    int iLinkCount = 0;

    DWORD32 dwPrintStatusMs = GetSystemTick();

    // 创建所有的连接线程
    CreateLinkThread();
    if(m_fAutoLinkEnable)
    {
        StartAutoLink();
    }

#ifdef SINGLE_BOARD_PLATFORM

    unsigned char bLinkSpeed = 0xFF;
    int nFileID = -1;
    DWORD32 dwLastPhyStatusTime = 0;
    PHY_STATUS_STRUCT cPhyStatus;

    if(m_fMTUSetEnable)
    {
        if (m_wPort == 9902)
        {
        nFileID = SwDevOpen(O_RDWR);
        if (nFileID < 0)
        {
            HV_Trace(5, "Open swdev driver failed!");
            return E_FAIL;
        }
        }
    }

#endif

    while ( !m_fExit )
    {
        m_dwThreadIsOkMs = GetSystemTick();

#ifdef SINGLE_BOARD_PLATFORM

        if(m_fMTUSetEnable)
        {
            if (m_wPort == 9902
                && m_dwThreadIsOkMs - dwLastPhyStatusTime >= 2000
                && 1 == m_iCameraType)
            {
                if (SwNetGetPhyStatus(nFileID, &cPhyStatus) < 0)
                {
                    HV_Trace(5, "Get phy status failed!");
                    HV_Sleep(2000);
                    continue;
                }
                if (bLinkSpeed != cPhyStatus.speed[1]
                    && cPhyStatus.linked[1] == 1
                    && cPhyStatus.speed[1] < 3)
                {
                    int iMTU = 0;
                    bLinkSpeed = cPhyStatus.speed[1];
                    // 百兆
                    if (0x01 == cPhyStatus.speed[1])
                    {
                        iMTU = 400;
                    }
                    else // 非百兆
                    {
                        iMTU = 1500;
                    }
                    if ( 0 != SetMTU("eth0.20", iMTU) )
                    {
                        HV_Trace(5, "Set mtu size failed!");
                        bLinkSpeed = 0xFF;
                        HV_Sleep(1000);
                        continue;
                    }
                }
                dwLastPhyStatusTime = m_dwThreadIsOkMs;
            }
        }
#endif

        if ( INVALID_SOCKET == m_hSocket && iLinkCount < m_iMaxLinkCount )
        {
            m_hSocket = HvCreateSocket();
            if ( INVALID_SOCKET == m_hSocket )
            {
                HV_Trace(5, "<link>HvCreateSocket failed!\n");
                HV_Sleep(2000);
                continue;
            }
            if ( S_OK != HvListen(m_hSocket, m_wPort, m_iMaxLinkCount) )
            {
                HV_Trace(5, "<link>HvListen<%d, %d> failed!\n", m_wPort, m_iMaxLinkCount);
                HV_Sleep(2000);
                continue;
            }
            HV_Trace(3, "<link>Link ctrl run,port:%d.\n",  m_wPort);
        }
        // 删除已断开的连接
        iLinkCount = DeleteFreeLink();

        // 打印连接状态
        if ( dwPrintStatusMs < m_dwThreadIsOkMs && (m_dwThreadIsOkMs - dwPrintStatusMs) > 15000 )
        {
            char szLinkName[64] = {0};
            sprintf(szLinkName, "Unknow");
            if ( m_wPort == 9900 )
            {
                sprintf(szLinkName, "Image");
            }
            else if ( m_wPort == 9901 )
            {
                sprintf(szLinkName, "Video");
            }
            else if ( m_wPort == 9902 )
            {
                sprintf(szLinkName, "Record");
            }
            HV_Trace(5, "<link> %s: max:%d, count:%d.\n", szLinkName, m_iMaxLinkCount, iLinkCount);
            dwPrintStatusMs = m_dwThreadIsOkMs;
        }

        // 限制连接数
        if ( iLinkCount >= m_iMaxLinkCount )
        {
            if ( INVALID_SOCKET != m_hSocket )
            {
                CloseSocket(m_hSocket);
                m_hSocket = INVALID_SOCKET;
            }
            HV_Sleep(2000);
            continue;
        }

        if ( m_hSocket == INVALID_SOCKET )
        {
            HV_Sleep(2000);
            continue;
        }

        // 监听
        hNewSocket = INVALID_SOCKET;
        HRESULT hr = HvAccept(m_hSocket, hNewSocket, iWaitMs);
        if (S_OK == hr)
        {
            HV_Trace(3, "<link>Accept link, prot:%d.\n", m_wPort);
            // 创建新的连接
            HvSetRecvTimeOut(hNewSocket, iTimeOutMs);
            HvSetSendTimeOut(hNewSocket, iTimeOutMs);
            if ( S_OK != SetLink(hNewSocket) )
            {
                if ( INVALID_SOCKET != hNewSocket )
                {
                    CloseSocket(hNewSocket);
                    hNewSocket = INVALID_SOCKET;
                }
                HV_Trace(3, "<link>Create link failed! prot:%d.\n", m_wPort);
            }
        }
    }

#ifdef SINGLE_BOARD_PLATFORM
    SwDevClose(nFileID);
#endif

    // 断开连接
    if ( INVALID_SOCKET != m_hSocket )
    {
        CloseSocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }

    if(m_fAutoLinkEnable)
    {
        StopAutoLink();
    }

    // 删除所有的连接
    ClearLink();

    HV_Trace(3, "<link>Link ctrl exit, port:%d\n", m_wPort);

    m_fExit = TRUE;
    return S_OK;
}


//--------------======================= CCameraLinkBase ==================---------------------------

int CCameraData::s_iCount = 0;

HRESULT CCameraDataReference::CreateCameraDataReference(CCameraDataReference** ppRefData)
{
    if ( ppRefData == NULL ) return E_POINTER;

    HRESULT hr = S_OK;

    *ppRefData = new CCameraDataReference;
    if (*ppRefData != NULL)
    {
        hr = (*ppRefData)->Create();
        if ( hr != S_OK )
        {
            delete (*ppRefData);
            (*ppRefData) = NULL;
        }
    }
    else
    {
        hr = E_POINTER;
    }

    return hr;
}

/*//--------------------====================== CCameraMemBlock ====================--------------------------------

HV_SEM_HANDLE CCameraMemBlock::m_hSemVideo;
HV_SEM_HANDLE CCameraMemBlock::m_hSemImage;
HV_SEM_HANDLE CCameraMemBlock::m_hSemRecord;
std::list<DSPLinkBuffer> CCameraMemBlock::m_lstVideoMemBlock;
std::list<DSPLinkBuffer> CCameraMemBlock::m_lstImageMemBlock;
std::list<DSPLinkBuffer> CCameraMemBlock::m_lstRecordMemBlock;
bool CCameraMemBlock::m_fInitialized = false;

bool CCameraMemBlock::InitMemBlcok()
{
    if ( m_fInitialized )
    {
        return true;
    }

    if ( CreateSemaphore(&m_hSemVideo, 1, 1) != 0
            || CreateSemaphore(&m_hSemImage, 1, 1) != 0
            || CreateSemaphore(&m_hSemRecord, 1, 1) != 0 )
    {
        HV_Trace(5, "<CCameraMemBlock>CreateSemaphore Failed!\n");
        return false;
    }

    DSPLinkBuffer memVideo;
    for (int i = 0; i < MAX_MEM_BLOCK_VIDEO; ++i)
    {
        CreateShareMemOnPool(&memVideo, VIDEO_SIZE);
        if ( memVideo.addr != NULL )
        {
            m_lstVideoMemBlock.push_back(memVideo);
        }
        else
        {
            return false;
        }
    }

    DSPLinkBuffer memImage;
    for (int i = 0; i < MAX_MEM_BLOCK_IMAGE; ++i)
    {
        CreateShareMemOnPool(&memImage, IMAGE_SIZE);
        if ( memImage.addr != NULL )
        {
            m_lstImageMemBlock.push_back(memImage);
        }
        else
        {
            return false;
        }
    }

    DSPLinkBuffer memRecord;
    for (int i = 0; i < MAX_MEM_BLOCK_RECORD; ++i)
    {
        CreateShareMemOnPool(&memRecord, RECORD_SIZE);
        if ( memImage.addr != NULL )
        {
            m_lstRecordMemBlock.push_back(memRecord);
        }
        else
        {
            return false;
        }
    }

    m_fInitialized = true;
    return true;
}

bool CCameraMemBlock::UnInitMemBlcok()
{
    if ( !m_fInitialized )
    {
        return true;
    }

    DSPLinkBuffer memVideo;
    while (m_lstVideoMemBlock.size() > 0)
    {
        memVideo = m_lstVideoMemBlock.front();
        m_lstVideoMemBlock.pop_front();
        FreeShareMemOnPool(&memVideo);
    }

    DSPLinkBuffer memImage;
    while (m_lstImageMemBlock.size() > 0)
    {
        memImage = m_lstImageMemBlock.front();
        m_lstImageMemBlock.pop_front();
        FreeShareMemOnPool(&memImage);
    }

    DSPLinkBuffer memRecord;
    while (m_lstRecordMemBlock.size() > 0)
    {
        memRecord = m_lstRecordMemBlock.front();
        m_lstRecordMemBlock.pop_front();
        FreeShareMemOnPool(&memRecord);
    }

    m_fInitialized = false;
    return true;
}

bool CCameraMemBlock::CreateMemBlock(const CAMERA_MEM_TYPE& type, DSPLinkBuffer* pBuffer)
{
    if ( !m_fInitialized || pBuffer == NULL )
    {
        return false;
    }

    bool fRet = false;
    switch (type)
    {
    case MEM_VIDEO:
        SemPend(&m_hSemVideo);
        if ( !m_lstVideoMemBlock.empty() )
        {
            *pBuffer = m_lstVideoMemBlock.front();
            m_lstVideoMemBlock.pop_front();
            fRet = true;
        }
        SemPost(&m_hSemVideo);
        break;

    case MEM_IMAGE:
        SemPend(&m_hSemImage);
        if ( !m_lstImageMemBlock.empty() )
        {
            *pBuffer = m_lstImageMemBlock.front();
            m_lstImageMemBlock.pop_front();
            fRet = true;
        }
        SemPost(&m_hSemImage);
        break;

    case MEM_RECORD:
        SemPend(&m_hSemRecord);
        if ( !m_lstRecordMemBlock.empty() )
        {
            *pBuffer = m_lstRecordMemBlock.front();
            m_lstRecordMemBlock.pop_front();
            fRet = true;
        }
        SemPost(&m_hSemRecord);
        break;

    default:
        break;
    }

    return fRet;
}

bool CCameraMemBlock::FreeMemBlock(const CAMERA_MEM_TYPE& type, DSPLinkBuffer* pBuffer)
{
    if ( !m_fInitialized || pBuffer == NULL )
    {
        return false;
    }

    bool fRet = false;
    switch (type)
    {
    case MEM_VIDEO:
        SemPend(&m_hSemVideo);
        if ( m_lstVideoMemBlock.size() < MAX_MEM_BLOCK_VIDEO
                && pBuffer->addr != NULL )
        {
            m_lstVideoMemBlock.push_back(*pBuffer);
            fRet = true;
        }
        SemPost(&m_hSemVideo);
        break;

    case MEM_IMAGE:
        SemPend(&m_hSemImage);
        if ( m_lstImageMemBlock.size() < MAX_MEM_BLOCK_IMAGE
                && pBuffer->addr != NULL )
        {
            m_lstImageMemBlock.push_back(*pBuffer);
            fRet = true;
        }
        SemPost(&m_hSemImage);
        break;

    case MEM_RECORD:
        SemPend(&m_hSemRecord);
        if ( m_lstRecordMemBlock.size() < MAX_MEM_BLOCK_RECORD
                && pBuffer->addr != NULL )
        {
            m_lstRecordMemBlock.push_back(*pBuffer);
            fRet = true;
        }
        SemPost(&m_hSemRecord);
        break;

    default:
        break;
    }

    return fRet;
}

PBYTE8 CCameraMemBlock::GetVirtualAddr(DSPLinkBuffer* pBuffer)
{
    if ( pBuffer == NULL )
    {
        return NULL;
    }
    else
    {
        return (PBYTE8)(pBuffer->addr);
    }
}
*/

// -------------------------------------------------------------------------

CameraConnectedLog g_cCameraConnectedLog;  // 全局唯一

CameraConnectedLog::CameraConnectedLog()
{
    m_listConnectedLog.RemoveAll();
    m_dwLogId = 1;
}

HRESULT CameraConnectedLog::AddConnectedItem(DWORD32& dwLogId, HV_SOCKET_HANDLE hSocket, CAMERA_LINK_PORT t)
{
    DWORD32 dwPeerIPv4;
    WORD16 wPeerPort;

    dwLogId = m_dwLogId++;

    if ( S_OK == HvGetPeerName(hSocket, &dwPeerIPv4, &wPeerPort) )
    {
        char szPeerIP[16];
        dwPeerIPv4 = ntohl(dwPeerIPv4);
        MyGetIpString(dwPeerIPv4, szPeerIP);

        ConnectedLog log;
        log.dwID = dwLogId;
        log.eType = t;
        memcpy(log.szIp, szPeerIP, 16);
        log.nPort = wPeerPort;
        m_listConnectedLog.AddTail(log);

        HV_Trace(5, "<%d> Connected with [%s:%d].\n", (int)t, szPeerIP, wPeerPort);
        return S_OK;
    }
    else
    {
        HV_Trace(5, "AddConnectedItem::HvGetPeerName is Error!\n");
        return E_FAIL;
    }
}

HRESULT CameraConnectedLog::RemoveConnectedItem(DWORD32 dwLogId)
{
    HRESULT hr = E_FAIL;
    ConnectedLog log;

    HVPOSITION pos = m_listConnectedLog.GetHeadPosition();
    HVPOSITION pos_cur;
    while ( -1 != pos )
    {
        pos_cur = pos;
        log = m_listConnectedLog.GetNext(pos);
        if ( dwLogId == log.dwID)
        {
            HV_Trace(5, "<%d> Remove Connected [%s:%d].\n",
                     (int)log.eType, log.szIp, log.nPort);

            m_listConnectedLog.RemoveAt(pos_cur);
            hr = S_OK;
            break;
        }
    }

    if ( S_OK != hr )
    {
        HV_Trace(5, "RemoveConnectedItem is Error!\n");
    }

    return hr;
}

int CameraConnectedLog::GetConnectedInfo(
    char rgszName[][IVN_MAX_SIZE],
    char rgszValue[][IVN_MAX_SIZE],
    int nArrayCount
)
{
    int iCount = m_listConnectedLog.GetSize();
    if ( nArrayCount < (iCount+1) )
    {
        return -1;
    }

    strcpy(rgszName[0], "COUNT");
    sprintf(rgszValue[0], "%d", iCount);

    ConnectedLog log;
    HVPOSITION pos = m_listConnectedLog.GetHeadPosition();
    for ( int i = 0; i < iCount; ++i )
    {
        log = m_listConnectedLog.GetNext(pos);
        sprintf(rgszName[i+1], "IP%d", i+1);
        sprintf(rgszValue[i+1], "%s,%d", log.szIp, (int)log.eType);
    }

    return (iCount+1);
}

// -------------------------------------------------------------------------

void* GetNodeData(BYTE8* pInfoDat, int nInfoLen, DWORD32 dwID, int* pDatLen)
{
    if (pDatLen) *pDatLen = 0;

    int nOffset = 0;
    DWORD32 dwDatID = 0;
    DWORD32 dwDatLen = 0;
    void* pDatPtr = NULL;

    while ((nOffset+8)<nInfoLen-1) //DatLen must greater than 0
    {
        dwDatID = *(DWORD32*)(pInfoDat+nOffset);
        dwDatLen = *(DWORD32*)(pInfoDat+nOffset+4);

        if (dwDatID == dwID)
        {
            pDatPtr = (void*)(pInfoDat + nOffset + 8);
            if (pDatLen) *pDatLen = (int)dwDatLen;
            break;
        }
        else
        {
            nOffset += (4 + dwDatLen);
        }
    }

    return pDatPtr;
}
