// 该文件编码格式必须是WIN936

#include "CameraRecordLink.h"
#include "hvutils.h"
#include "misc.h"
#include "HvPciLinkApi.h"
#include "hvtarget_ARM.h"

using namespace std;
using namespace HiVideo;

/* HvPciLinkApi.cpp */
extern const int PCILINK_DATA_MAX_LEN;

/* HvUtilsClass.cpp */
extern int g_nHddCheckStatus;

// 工具函数：快速建立命令返回xml
extern int BuildHvCmdXml(
        char* pXmlBuf,
        char* pCmdName,
        int iArgCount,
        const char rgszName[][IVN_MAX_SIZE],
        const char rgszValue[][IVN_MAX_SIZE]
    );

// 按PCILink结果协议将图片打包
inline static void PutBigImgToRecord(
    PBYTE8 pbData,
    DWORD32& dwDataOffset,
    HVIO_BigImage* pBigimg,
    WORD16 wID
)
{
    if ( pBigimg->m_dwLen > 0 )
    {
        PCI_IMAGE_INFO cImgInfo;
        cImgInfo.dwImgType = wID;
        cImgInfo.dwImgWidth = pBigimg->m_wImageWidth;
        cImgInfo.dwImgHeight = pBigimg->m_wImageHeight;
        cImgInfo.dwTimeLow = pBigimg->m_dwTimeLow;
        cImgInfo.dwTimeHigh = pBigimg->m_dwTimeHigh;

        DWORD32 dwLenTmp = sizeof(PCI_IMAGE_INFO);
        memcpy(pbData+dwDataOffset, &dwLenTmp, 4);
        dwDataOffset += 4;

        memcpy(pbData+dwDataOffset, &cImgInfo, sizeof(PCI_IMAGE_INFO));
        dwDataOffset += sizeof(PCI_IMAGE_INFO);

        memcpy(pbData+dwDataOffset, &pBigimg->m_dwImageSize, 4);
        dwDataOffset += 4;

        memcpy(pbData+dwDataOffset, pBigimg->m_pImage, pBigimg->m_dwImageSize);
        dwDataOffset += pBigimg->m_dwImageSize;
    }
}

//--------------=============== CHistoryRecordProcess ================----------------
CHistoryRecordProcess::CHistoryRecordProcess(CCameraRecordLink* pRecordLink)
        : m_pRecordLink(pRecordLink)
        , m_dwThreadIsOkMs(0)
        , m_fIsEndOfRecord(FALSE)
{
}

CHistoryRecordProcess::~CHistoryRecordProcess()

{
}

HRESULT CHistoryRecordProcess::GetRefData(
    CCameraDataReference** ppRefRecord,
    CCameraData** ppCameraRecord,
    DWORD32 dwInfoSize,
    DWORD32 dwDataSize
)
{
    IReferenceMemory* pRefMemory = NULL;
    if ( S_OK != CreateReferenceMemory(&pRefMemory, dwDataSize) )
    {
        HV_Trace(5, "<link>Record CreateReferenceMemory failed!\n");
        return E_OUTOFMEMORY;
    }
    if ( S_OK != CCameraDataReference::CreateCameraDataReference(ppRefRecord) )
    {
        HV_Trace(5, "<link>Record CreateCameraDataReference failed!\n");
        pRefMemory->Release();
        return E_OUTOFMEMORY;
    }
    (*ppCameraRecord) = (*ppRefRecord)->GetItem();
    if ( (*ppCameraRecord) == NULL )
    {
        HV_Trace(5, "<link>Record GetItem == NULL!\n");
        (*ppRefRecord)->Release();
        pRefMemory->Release();
        return E_OUTOFMEMORY;
    }

    (*ppCameraRecord)->header.dwInfoSize = dwInfoSize;
    (*ppCameraRecord)->header.dwDataSize = dwDataSize;
    (*ppCameraRecord)->pbInfo = new BYTE8[dwInfoSize];
    (*ppCameraRecord)->pbData = NULL;
    // zhaopy
    (*ppCameraRecord)->pRefMemory = pRefMemory;
    (*ppCameraRecord)->pRefMemory->AddRef();
    (*ppCameraRecord)->pRefMemory->GetData(&((*ppCameraRecord)->pbData));
    pRefMemory->Release();

    if ( (*ppCameraRecord)->pbInfo == NULL || (*ppCameraRecord)->pbData == NULL )
    {
        HV_Trace(5, "<link>Record memory failed!\n");
        if ( (*ppCameraRecord)->pbInfo != NULL )
        {
            delete [] (*ppCameraRecord)->pbInfo;
            (*ppCameraRecord)->pbInfo = NULL;
        }

        (*ppRefRecord)->Release();
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

HRESULT CHistoryRecordProcess::Run(void* pvParam)
{
    if (m_pRecordLink == NULL)
    {
        HV_Trace(5, "<link>m_pRecordLink == NULL!");
        return E_POINTER;
    }

    HRESULT hr = S_OK;

#ifdef SINGLE_BOARD_PLATFORM
    const int siPlateDataLen = 4 * 1024 * 1024;
#else
    const int siPlateDataLen = 2.5 * 1024 * 1024;
#endif
    const int siPlateInfoLen = 48 * 1024;

    DWORD32 dwTimeTickMs;

    int nLastDiskID = -1;
    while (!m_fExit)
    {
        m_dwThreadIsOkMs = GetSystemTick();
        dwTimeTickMs = m_pRecordLink->m_cSendRecordParam.iSendRecordSpace * 100;
        HV_Sleep(dwTimeTickMs);

        if (1 != g_nHddCheckStatus || !m_pRecordLink->IsRecordListEmpty())
        {
            continue;
        }

        if ( m_pRecordLink->m_dwEndSendTimeLow != 0
                && m_pRecordLink->m_dwEndSendTimeHigh != 0
                && -1 == ComputeTimeMs(
                    m_pRecordLink->m_dwSendTimeLow,
                    m_pRecordLink->m_dwSendTimeHigh,
                    m_pRecordLink->m_dwEndSendTimeLow,
                    m_pRecordLink->m_dwEndSendTimeHigh
                )
           )
        {
            m_fIsEndOfRecord = TRUE;
            break;
        }

        // 将可靠性保存结果发送到发送队列
        if ( NULL != m_pRecordLink->m_cSendRecordParam.pcSafeSaver )
        {
            DWORD32 dwNowTimeLow, dwNowTimeHigh;
            GetSystemTime(&dwNowTimeLow, &dwNowTimeHigh);

            CCameraDataReference* pRefRecord = NULL;
            CCameraData* pCameraRecord = NULL;
            if (S_OK != GetRefData(&pRefRecord, &pCameraRecord, siPlateInfoLen, siPlateDataLen))
            {
                continue;
            }

            hr = m_pRecordLink->m_cSendRecordParam.pcSafeSaver->GetPlateRecord(
                     m_pRecordLink->m_dwSendTimeLow,
                     m_pRecordLink->m_dwSendTimeHigh,
                     m_pRecordLink->m_iRecordIndex,
                     &pCameraRecord->header,
                     pCameraRecord->pbInfo,
                     siPlateInfoLen,
                     pCameraRecord->pbData,
                     siPlateDataLen,
                     m_pRecordLink->m_szDevList,
                     nLastDiskID,
                     m_pRecordLink->m_dwDataInfo
                 );

            if (E_NOTIMPL == hr)
            {
                pRefRecord->Release();
                continue;
            }

            if (hr == S_OK)
            {
                bool fPeccancy = NULL != strstr((char *)(pCameraRecord->pbInfo + 20), "违章:是");
                switch (m_pRecordLink->m_cSendRecordParam.iOutputOnlyPeccancy)
                {
                case 1 : //只输出违章车辆
                    if (!fPeccancy)
                    {
                        m_pRecordLink->m_iRecordIndex++;
                        pRefRecord->Release();
                        continue;
                    }
                    break;
                case 2 : //只输出非违章结果
                    if (fPeccancy)
                    {
                        m_pRecordLink->m_iRecordIndex++;
                        pRefRecord->Release();
                        continue;
                    }
                    break;
                case 3 : //接收历史数据时,非违章输出文本,违章输出文本和图片
                    if (!fPeccancy)
                    {
                        pCameraRecord->header.dwDataSize = 0;
                    }
                    break;
                }

                if (m_pRecordLink->m_dwDataInfo && !fPeccancy)
                {
                    m_pRecordLink->m_iRecordIndex++;
                    pRefRecord->Release();
                    continue;
                }
            }
            if ( S_OK == hr )
            {
                hr = m_pRecordLink->SendRecord(pRefRecord);
                if (hr == S_OK)
                {
                    m_pRecordLink->m_iRecordIndex++;
                }
            }
            else if ( (int)E_RECORD_NONE == hr
                      && ComputeHour(
                          m_pRecordLink->m_dwSendTimeLow,
                          m_pRecordLink->m_dwSendTimeHigh,
                          dwNowTimeLow, dwNowTimeHigh
                      )
                    )
            {
                //如果找不到记录，且当前时间大于记录时间一个小时,则记录时间加一小时
                DWORD32 dwMs = 0xFFFFFFFF - (60 * 60 * 1000);
                if ( dwMs < m_pRecordLink->m_dwSendTimeLow )
                {
                    m_pRecordLink->m_dwSendTimeHigh++;
                    m_pRecordLink->m_dwSendTimeLow = (m_pRecordLink->m_dwSendTimeLow - dwMs + 1);
                }
                else
                {
                    m_pRecordLink->m_dwSendTimeLow += (60 * 60 * 1000);
                }
                m_pRecordLink->m_iRecordIndex = 0;
                hr = S_FALSE;

                HV_Trace(5, "<link> Plate Change Read Time.\n");
            }
            else if ( (int)E_RECORD_NONE == hr )
            {
                REAL_TIME_STRUCT rtBegin, rtEnd;
                ConvertMsToTime(m_pRecordLink->m_dwSendTimeLow, m_pRecordLink->m_dwSendTimeHigh, &rtBegin);
                ConvertMsToTime(dwNowTimeLow, dwNowTimeHigh, &rtEnd);
                if (rtBegin.wYear == rtEnd.wYear
                        && rtBegin.wMonth == rtEnd.wMonth
                        && rtBegin.wDay == rtEnd.wDay
                        && rtBegin.wHour == rtEnd.wHour)
                {
                    //读取完毕
                    HV_Sleep(1000);
                }
                else
                {
                    HV_Trace(5, "<link> Plate record is OutTime!\n");
                    HV_Sleep(1000);
                }
                hr = S_FALSE;
            }
            else if ( (int)E_RECORD_WRITING == (int)hr || E_RECORD_WAIT_TIMEOUT == hr)
            {
                hr = S_FALSE;
                HV_Sleep(1000);
            }
            else if ( E_FAIL == hr )
            {
                // 读取失败，则跳过该条记录
                hr = S_FALSE;
                ++(m_pRecordLink->m_iRecordIndex);
                HV_Trace(5, "<link> GetPlateRecord is Failed!!!!!\n");
            }

            pRefRecord->Release();

            if ( FAILED(hr) )
            {
                break;
            }
        }
    }

    return hr;
}

BOOL CHistoryRecordProcess::IsEndOfRecord()
{
    //去掉线程状态的判断，修正HVNAVI每次断开连接时都会提示接受历史数据完成的状态
    if (m_fIsEndOfRecord)
    {
        return TRUE;
    }
    return FALSE;
}

BOOL CHistoryRecordProcess::ThreadIsOk()
{
    DWORD32 dwCurTick = GetSystemTick();
    if ( dwCurTick < m_dwThreadIsOkMs || m_dwThreadIsOkMs == 0 )
    {
        return TRUE;
    }

    if ( int(dwCurTick - m_dwThreadIsOkMs) < MAX_WAIT_MS )
    {
        return TRUE;
    }
    else
    {
        HV_Trace(5, "CHistoryRecordProcess::GetCurStatus [CurTick = %dms, LastOkTime = %dms]\n", dwCurTick, m_dwThreadIsOkMs);
    }
    return FALSE;
}

//--------------=============== CCameraRecordLink ==================------------------

CCameraRecordLink::CCameraRecordLink(HV_SOCKET_HANDLE hSocket, SEND_RECORD_PARAM& cSendRecordParam)
        : m_cSendRecordParam(cSendRecordParam)
        , m_dwDataInfo(0)
        , m_dwThreadStatus(0)
        , m_pSocketStream(NULL)
        , m_fSendHistoryEnable(false)
        , m_pHistoryPro(NULL)
        , m_dwInfoOffset(0)
        , m_dwDataOffset(0)
        , m_hSocket(hSocket)
        , m_dwSendTimeLow(0)
        , m_dwSendTimeHigh(0)
        , m_dwEndSendTimeLow(0)
        , m_dwEndSendTimeHigh(0)
        , m_iRecordIndex(0)
{
    //Comment by Shaorg: 屏蔽SIGPIPE异常，该异常的默认处理将导致程序被Killed。
    sigset_t signal_mask;
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGPIPE);
    int rc = pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
    if (rc != 0)
    {
        HV_Trace(1, "pthread_sigmask is error.\n");
    }

    m_cameraThrob.dwType = CAMERA_THROB;
    m_cameraThrob.dwInfoSize = 0;
    m_cameraThrob.dwDataSize = 0;

    m_cHistoryEnd.dwType = CAMERA_HISTORY_END;
    m_cHistoryEnd.dwInfoSize = 0;
    m_cHistoryEnd.dwDataSize = 0;

    if (CreateSemaphore(&m_hSemRecord, 0, MAX_CAMERA_RECORD_COUNT) != 0)
    {
        HV_Trace(5, "<link>Link CreateSemaphore Failed!\n");
    }
    if (CreateSemaphore(&m_hSemListRecord, 1, 1) != 0)
    {
        HV_Trace(5, "<link>Link CreateSemaphore Failed!\n");
    }

    m_dwThreadIsOkMs = GetSystemTick();

    memset(m_szDevList, 0, sizeof(m_szDevList));
}

CCameraRecordLink::~CCameraRecordLink()
{
    Stop(-1);
    DestroySemaphore(&m_hSemRecord);
    DestroySemaphore(&m_hSemListRecord);
    if ( m_pSocketStream != NULL )
    {
        delete m_pSocketStream;
        m_pSocketStream = NULL;
    }
    if ( INVALID_SOCKET != m_hSocket )
    {
        CloseSocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }
}

bool CCameraRecordLink::IsSendHistoryPlate()
{
    return m_fSendHistoryEnable;
}

HRESULT CCameraRecordLink::SendRecord( CCameraDataReference* pRefRecord )
{
    if ( pRefRecord == NULL ) return E_POINTER;
    if ( m_fExit ) return S_OK;
    if ( m_dwThreadStatus == 0 ) return S_OK;

    SemPend(&m_hSemListRecord);
    if ( (int)m_lstRecord.size() >= MAX_CAMERA_RECORD_COUNT )
    {
        SemPost(&m_hSemListRecord);
        HV_Trace(5, "m_lstRecord can not contain more record result, lost it.\n");
        return E_OUTOFMEMORY;
    }
    pRefRecord->AddRef();
    m_lstRecord.push_back(pRefRecord);
    SemPost(&m_hSemListRecord);

    // 发送信号
    SemPost(&m_hSemRecord);

    return S_OK;
}

HRESULT CCameraRecordLink::SetSocket(HV_SOCKET_HANDLE& hSocket)
{
    if ( m_dwThreadStatus == 1 ) return E_FAIL;
    if ( m_pSocketStream != NULL )
    {
        delete m_pSocketStream;
        m_pSocketStream = NULL;
    }
    if ( INVALID_SOCKET != m_hSocket )
    {
        CloseSocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }
    m_hSocket = hSocket;
    m_pSocketStream = new CSocketStream(m_hSocket);
    m_dwThreadStatus = 1;
    g_cCameraConnectedLog.AddConnectedItem(m_dwConnectedLogId, m_hSocket, CAMERA_RECORD_LINK_PORT);
    return S_OK;
}

// 初始化参数,连接时才确定的参数。
HRESULT CCameraRecordLink::Init()
{
    m_dwSendTimeLow = 0;
    m_dwSendTimeHigh = 0;
    m_dwEndSendTimeLow = 0;
    m_dwEndSendTimeHigh = 0;
    m_iRecordIndex = 0;
    m_fSendHistoryEnable = false;
    m_dwDataInfo = 0;
    memset(m_szDevList, 0, sizeof(m_szDevList));

    return S_OK;
}

HRESULT CCameraRecordLink::DownloadRecordXML(const TiXmlElement* pCmdArgElement)
{
    if ( pCmdArgElement == NULL )
    {
        return E_POINTER;
    }
    // 例如： 2011.02.05_12
    char szEnable[32] = {0};
    char szBeginTime[32] = {0};
    char szIndex[32] = {0};
    char szEndTime[32] = {0};
    const char *pszAttribute = NULL;

    DWORD32 dwInfoOffset = 0;
    DWORD32 dwDataOffset = 0;

    m_dwInfoOffset = 0;
    m_dwDataOffset = 0;

    pszAttribute = pCmdArgElement->Attribute("Enable");
    if (pszAttribute != NULL && strlen(pszAttribute) < 32)
    {
        strcpy(szEnable, pszAttribute);
    }
    pszAttribute = pCmdArgElement->Attribute("BeginTime");
    if (pszAttribute != NULL && strlen(pszAttribute) < 32)
    {
        strcpy(szBeginTime, pszAttribute);
    }
    pszAttribute = pCmdArgElement->Attribute("Index");
    if (pszAttribute != NULL && strlen(pszAttribute) < 32)
    {
        strcpy(szIndex, pszAttribute);
    }
    pszAttribute = pCmdArgElement->Attribute("EndTime");
    if (pszAttribute != NULL && strlen(pszAttribute) < 32)
    {
        strcpy(szEndTime, pszAttribute);
    }
    pszAttribute = pCmdArgElement->Attribute("DevList");
    if (pszAttribute != NULL && strlen(pszAttribute) < sizeof(m_szDevList))
    {
        strcpy(m_szDevList, pszAttribute);
        HV_Trace(5, "Specify dev list:%s\n", m_szDevList);
    }
    pszAttribute = pCmdArgElement->Attribute("DataInfo");
    if (pszAttribute != NULL && strlen(pszAttribute) < 8)
    {
        m_dwDataInfo = atoi(pszAttribute);
        HV_Trace(5, "Specify DataInfo :%u\n", m_dwDataInfo);
    }
    pszAttribute = pCmdArgElement->Attribute("InfoOffset");
    if (pszAttribute != NULL && strlen(pszAttribute) < 8)
    {
        dwInfoOffset = atoi(pszAttribute);
        HV_Trace(5, "Specify InfoOffset :%u\n", dwInfoOffset);
    }
    pszAttribute = pCmdArgElement->Attribute("DataOffset");
    if (pszAttribute != NULL && strlen(pszAttribute) < 8)
    {
        dwDataOffset = atoi(pszAttribute);
        HV_Trace(5, "Specify DataOffset :%u\n", dwDataOffset);
    }

    if ( 1 == strlen(szEnable)
            && 13 == strlen(szBeginTime)
            && 0 <= strlen(szIndex)
            && 0 <= strlen(szEndTime) )
    {
        m_fSendHistoryEnable = ( atoi(szEnable) == 1 ? true : false );
        m_iRecordIndex = atoi(szIndex);

        m_dwInfoOffset = dwInfoOffset;
        m_dwDataOffset = dwDataOffset;

        REAL_TIME_STRUCT cRealTime;
        DWORD32 dwYear,dwMonth,dwDay,dwHour;

        sscanf(szBeginTime, "%04d.%02d.%02d_%02d",
               &dwYear,&dwMonth,&dwDay,&dwHour);
        cRealTime.wYear = dwYear;
        cRealTime.wMonth = dwMonth;
        cRealTime.wDay = dwDay;
        cRealTime.wHour = dwHour;
        cRealTime.wMinute = 0;
        cRealTime.wSecond = 0;
        ConvertTimeToMs(&cRealTime, &m_dwSendTimeLow, &m_dwSendTimeHigh);

        HV_Trace(5, "<link>Record Enable = %d %d.%d.%d_%d, index = %d, ",
                 (int)m_fSendHistoryEnable,
                 (int)cRealTime.wYear, (int)cRealTime.wMonth, (int)cRealTime.wDay, (int)cRealTime.wHour,
                 m_iRecordIndex);

        if ( strlen(szEndTime) != 13 )
        {
            m_dwEndSendTimeLow = m_dwEndSendTimeHigh = 0;
        }
        else
        {
            REAL_TIME_STRUCT cRealTime;
            DWORD32 dwYear,dwMonth,dwDay,dwHour;

            sscanf(szEndTime, "%04d.%02d.%02d_%02d",
                   &dwYear,&dwMonth,&dwDay,&dwHour);
            cRealTime.wYear = dwYear;
            cRealTime.wMonth = dwMonth;
            cRealTime.wDay = dwDay;
            cRealTime.wHour = dwHour;
            cRealTime.wMinute = 0;
            cRealTime.wSecond = 0;
            ConvertTimeToMs(&cRealTime, &m_dwEndSendTimeLow, &m_dwEndSendTimeHigh);

            if ( ComputeTimeMs(m_dwSendTimeLow, m_dwSendTimeHigh, m_dwEndSendTimeLow, m_dwEndSendTimeHigh) == -1 )
            {
                m_dwEndSendTimeLow = m_dwEndSendTimeHigh = 0;
            }
            else
            {
                HV_Trace(5, "end:%d.%d.%d_%d.\n", (int)cRealTime.wYear, (int)cRealTime.wMonth, (int)cRealTime.wDay, (int)cRealTime.wHour );
            }
        }
        HV_Trace(5, "end:%d,%d.\n", (int)m_dwEndSendTimeLow, (int)m_dwEndSendTimeHigh );
    }

    return S_OK;
}

HRESULT CCameraRecordLink::ProcessXMLInfo(unsigned char* pbInfo, const DWORD32& dwInfoSize)
{
    HRESULT hr = E_FAIL;
    if ( pbInfo == NULL )
    {
        return E_POINTER;
    }

    TiXmlDocument cXmlDoc;
    if ( cXmlDoc.Parse((const char*)pbInfo) )
    {
        const TiXmlElement* pRootElement = cXmlDoc.RootElement();
        if ( NULL != pRootElement && 0 == strcmp("2.0", pRootElement->Attribute("ver")) )
        {
            const TiXmlElement* pElement = pRootElement->FirstChildElement("CmdName");
            if (pElement != NULL && NULL != pElement->GetText() && 0 == strcmp("DownloadRecord", pElement->GetText()))
            {
                hr = DownloadRecordXML(pElement);
            }
        }
    }
    return hr;
}

HRESULT CCameraRecordLink::ProcessCmd(HvCore::IHvStream* pSocketStream)
{
    HRESULT hr = S_OK;
    if ( pSocketStream == NULL )
    {
        return E_POINTER;
    }

    CAMERA_CMD_HEADER cCmdHeader;
    CAMERA_CMD_RESPOND cRespondHeader;
    cRespondHeader.dwID = CAMERA_XML_EXT_CMD;
    cRespondHeader.dwInfoSize = 0;
    cRespondHeader.dwResult = S_OK;

    static const int siMaxInfoSize = 32 * 1024;
    unsigned char* pbInfo = NULL;

    DWORD32 dwRead = 0;
    hr = pSocketStream->Read(&cCmdHeader, sizeof(cCmdHeader), &dwRead);
    if ( S_OK == hr )
    {
        if ( cCmdHeader.dwInfoSize > 0 && (int)cCmdHeader.dwInfoSize < siMaxInfoSize
                && cCmdHeader.dwID == CAMERA_XML_EXT_CMD )
        {
            pbInfo = new unsigned char[cCmdHeader.dwInfoSize + 1];
            if ( pbInfo != NULL )
            {
                memset(pbInfo, 0, cCmdHeader.dwInfoSize);
                hr = pSocketStream->Read(pbInfo, cCmdHeader.dwInfoSize, NULL);
                if ( S_OK == hr )
                {
                    // 回应
                    hr = pSocketStream->Write(&cRespondHeader, sizeof(cRespondHeader), NULL);
                    ProcessXMLInfo(pbInfo, cCmdHeader.dwInfoSize);
                }
            }
            else
            {
                hr = E_OUTOFMEMORY;
            }
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }
    }
    else
    {
        hr = E_NETFAIL;
    }
    SAFE_DELETE_ARRAY(pbInfo);

    return hr;
}

HRESULT CCameraRecordLink::DisConnect()
{
    if ( m_pHistoryPro )
    {
        m_pHistoryPro->Stop();
        delete m_pHistoryPro;
        m_pHistoryPro = NULL;
    }
    if ( m_pSocketStream != NULL )
    {
        delete m_pSocketStream;
        m_pSocketStream = NULL;
    }
    if ( INVALID_SOCKET != m_hSocket )
    {
        CloseSocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }
    m_dwThreadStatus = 0;
    g_cCameraConnectedLog.RemoveConnectedItem(m_dwConnectedLogId);
    //删除所有的数据
    CCameraDataReference *pRefRecord = NULL;
    SemPend(&m_hSemListRecord);
    while (!m_lstRecord.empty())
    {
        pRefRecord = m_lstRecord.front();
        m_lstRecord.pop_front();
        if ( pRefRecord != NULL )
        {
            pRefRecord->Release();
            pRefRecord = NULL;
        }
    }
    SemPost(&m_hSemListRecord);

    HV_Trace(5, "<link>Record link disconnect!\n");
    return S_OK;
}

HRESULT CCameraRecordLink::Run(void* pvParam)
{
    HRESULT hr = S_OK;

    int iWaitResultMs = 1000;
    CCameraDataReference* pRefRecord = NULL;
    CCameraData* pRecord = NULL;
    bool fIsNewLink = true;
    DWORD32 dwLastSendThrobTime = GetSystemTick();
    while (!m_fExit)
    {
        m_dwThreadIsOkMs = GetSystemTick();
        if ( m_dwThreadStatus == 0 )
        {
            //删除所有的数据
            pRefRecord = NULL;
            SemPend(&m_hSemListRecord);
            while (!m_lstRecord.empty())
            {
                pRefRecord = m_lstRecord.front();
                m_lstRecord.pop_front();
                if ( pRefRecord != NULL )
                {
                    pRefRecord->Release();
                    pRefRecord = NULL;
                }
            }
            SemPost(&m_hSemListRecord);
            HV_Sleep(1000);
            continue;
        }

        if ( fIsNewLink && (m_hSocket != INVALID_SOCKET) && (m_pSocketStream != NULL) && (m_dwThreadStatus == 1))
        {
            // 新的连接需要初始化可变参数
            Init();
            // 处理连接命令
            hr = ProcessCmd(m_pSocketStream);
            if ( S_OK != hr )
            {
                HV_Trace(5, "<link>Record link ProcessCmd failed! use default setting\n");
            }
            else if ( hr == S_OK )
            {
                HV_Trace(5, "<link>Record link ProcessCmd ok.\n");
            }
            HV_Trace(5, "<link>Record link start...\n");
            fIsNewLink = false;
        }
        else if ( fIsNewLink )
        {
            HV_Trace(5, "<link>ERROR:Record link unknow status! %d, %d, %d\n", m_hSocket, m_pSocketStream, m_dwThreadStatus);
            HV_Sleep(2000);
            continue;
        }

        if ( m_fSendHistoryEnable
                && NULL != m_cSendRecordParam.pcSafeSaver
                && NULL == m_pHistoryPro )
        {
            m_pHistoryPro = new CHistoryRecordProcess(this);
            if (m_pHistoryPro == NULL)
            {
                HV_Trace(5, "<link>Create history processor failed!");
                DisConnect();
                fIsNewLink = true;
                HV_Sleep(1000);
                continue;
            }
            m_pHistoryPro->Start(NULL);
        }

        // 检测历史数据是否发送完毕
        if (m_pHistoryPro && m_pHistoryPro->IsEndOfRecord())
        {
            //提示接收历史结束
            if (FAILED(m_pSocketStream->Write(&m_cHistoryEnd, sizeof(m_cHistoryEnd), NULL)))
            {
                HV_Trace(5, "<link>Record send HistoryEnd failed!\n");
            }
            // 如果是接历史数据完成后就断开连接
            DisConnect();
            fIsNewLink = true;
            HV_Sleep(1000);
            continue;
        }

        Lan2LedLight();
        hr = SemPend(&m_hSemRecord, iWaitResultMs);
        //等待信号，处理实时发送结果
        if (S_OK == hr)
        {
            SemPend(&m_hSemListRecord);
            if ( m_lstRecord.empty() )
            {
                SemPost(&m_hSemListRecord);
                HV_Sleep(100);
                continue;
            }
            pRefRecord = m_lstRecord.front();
            m_lstRecord.pop_front();
            SemPost(&m_hSemListRecord);

            if ( pRefRecord == NULL )
            {
                HV_Sleep(100);
                continue;
            }
            pRecord = pRefRecord->GetItem();
            if ( pRecord == NULL )
            {
                pRefRecord->Release();
                pRefRecord = NULL;
                HV_Sleep(100);
                continue;
            }

            bool fPeccancy = NULL != strstr((char *)(pRecord->pbInfo + 20), "违章:是");
            bool fSend = true;
            switch (m_cSendRecordParam.iOutputOnlyPeccancy)
            {
            case 1 : //只输出违章车辆
                if (!fPeccancy)
                {
                    fSend = false;
                }
                break;
            case 2 : //只输出非违章结果
                if (fPeccancy)
                {
                    fSend = false;
                }
                break;
            }
#ifdef SINGLE_BOARD_PLATFORM
            const int siPlateDataLen = 4 * 1024 * 1024;
#else
            const int siPlateDataLen = 2.5 * 1024 * 1024;
#endif
            const int siPlateInfoLen = 48 * 1024;
            if(pRecord->header.dwInfoSize > siPlateInfoLen || pRecord->header.dwDataSize > siPlateDataLen)
            {
                HV_Trace(5, "<link>Record send failed because of error size, info size:%d, data size : %d\n", pRecord->header.dwInfoSize, pRecord->header.dwDataSize);
                fSend = false;
            }

            if ( fSend )  // 在不使能安全存储器的情况下，才发送实时结果。
            {
                if ( FAILED(m_pSocketStream->Write(&pRecord->header, sizeof(pRecord->header), NULL))
                        || FAILED(m_pSocketStream->Write(pRecord->pbInfo, pRecord->header.dwInfoSize, NULL))
                        || FAILED(m_pSocketStream->Write(pRecord->pbData, pRecord->header.dwDataSize, NULL)) )
                {
                    HV_Trace(5, "<link>Record send failed recordsize:%d...\n", pRecord->header.dwDataSize + sizeof(pRecord->header) + pRecord->header.dwInfoSize);
                    hr = E_NETFAIL;
                }
            }

            pRecord = NULL;
            pRefRecord->Release();
            pRefRecord = NULL;

            if ( FAILED(hr) )
            {
                DisConnect();
                fIsNewLink = true;
            }
        }
        //修改成10秒钟发送一次心跳包，因为拔掉网线是发送成功但是接收失败，故用心跳包来确认连接是否正常
        if (!fIsNewLink && (S_OK != hr || GetSystemTick() - dwLastSendThrobTime > 10000))	// 发送心跳包
        {
            dwLastSendThrobTime = GetSystemTick();
            if ( FAILED(m_pSocketStream->Write(&m_cameraThrob, sizeof(m_cameraThrob), NULL))
                    || FAILED(m_pSocketStream->Read(&m_cameraThrobResponse, sizeof(m_cameraThrobResponse), NULL))
                    || (m_cameraThrobResponse.dwType != CAMERA_THROB_RESPONSE))
            {
                HV_Trace(5, "<link>Record send throb failed!\n");
                DisConnect();
                hr = E_NETFAIL;
                fIsNewLink = true;
            }
        }
    }

    m_fExit = TRUE;

    HV_Trace(5, "<link>Record link exit!\n");

    if ( INVALID_SOCKET != m_hSocket )
    {
        CloseSocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }

    //删除所有的数据
    pRefRecord = NULL;
    SemPend(&m_hSemListRecord);
    while (!m_lstRecord.empty())
    {
        pRefRecord = m_lstRecord.front();
        m_lstRecord.pop_front();
        if ( pRefRecord != NULL )
        {
            pRefRecord->Release();
            pRefRecord = NULL;
        }
    }
    SemPost(&m_hSemListRecord);

    return hr;
}


bool CCameraRecordLink:: IsRecordListEmpty()
{
    SemPend(&m_hSemListRecord);
    bool fIsRecordListEmpty = m_lstRecord.empty();
    SemPost(&m_hSemListRecord);

    return fIsRecordListEmpty;
}

bool CCameraRecordLink::IsSafeSaverEnable()
{
    return NULL != m_cSendRecordParam.pcSafeSaver;
}

int CCameraRecordLink::RecordOutputType()
{
    return m_cSendRecordParam.iOutputOnlyPeccancy;
}

HRESULT CCameraRecordLink::GetCCameraDataReference(CCameraDataReference** ppRefRecord)
{
    CCameraDataReference* pRefRecord = NULL;

    SemPend(&m_hSemListRecord);
    if ( m_lstRecord.empty() )
    {
        SemPost(&m_hSemListRecord);
        *ppRefRecord = NULL;
        return S_OK;
    }
    pRefRecord = m_lstRecord.front();
    m_lstRecord.pop_front();
    SemPost(&m_hSemListRecord);

    *ppRefRecord = pRefRecord;
    return S_OK;
}

HRESULT CCameraRecordLink::ClearCCameraData()
{
    CCameraDataReference* pRefRecord = NULL;

    SemPend(&m_hSemListRecord);
    while (!m_lstRecord.empty())
    {
        pRefRecord = m_lstRecord.front();
        m_lstRecord.pop_front();
        if ( pRefRecord != NULL )
        {
            pRefRecord->Release();
            pRefRecord = NULL;
        }
    }
    SemPost(&m_hSemListRecord);

    return S_OK;
}

bool CCameraRecordLink::ThreadIsOk()
{
    if ( m_fExit ) return true;

    DWORD32 dwCurTick = GetSystemTick();
    if ( dwCurTick < m_dwThreadIsOkMs )
    {
        HV_Trace(5, "CCameraRecordLink::GetCurStatus [CurTick = %dms, LastOkTime = %dms]\n", dwCurTick, m_dwThreadIsOkMs);
        return true;
    }

    if ( int(dwCurTick - m_dwThreadIsOkMs) < MAX_WAIT_MS )
    {
        return true;
    }
    else
    {
        HV_Trace(5, "CCameraRecordLink::GetCurStatus [CurTick = %dms, LastOkTime = %dms]\n", dwCurTick, m_dwThreadIsOkMs);
    }
    return false;
}

//---------================= CCameraRecordAutoLink ======================-------------------------


HRESULT ConnectToServer(char* pServerIP , WORD16 wServerPort , HV_SOCKET_HANDLE& hSocket ,HvCore::IHvStream** ppSocketStream)
{
    if (hSocket != INVALID_SOCKET)
    {
        CloseSocket(hSocket);
        hSocket = INVALID_SOCKET;

        if (*ppSocketStream != NULL)
        {
            delete *ppSocketStream;
            *ppSocketStream = NULL;
        }
        HV_Sleep(100);
    }

    if (NULL == pServerIP || wServerPort <= 0)
    {
        return E_FAIL;
    }
    hSocket = HvCreateSocket();
    if (hSocket == INVALID_SOCKET)
    {
        return E_FAIL;
    }

    HRESULT hr = HvSetRecvTimeOut(hSocket , 4000);
    if (FAILED(hr))
    {
        return hr;
    }
    hr = HvSetSendTimeOut(hSocket , 4000);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = HvConnect(hSocket , pServerIP , wServerPort , 3000);
    if (FAILED(hr))
    {
        HV_Trace(5 , "HVConnect fail!\n");
        return hr;
    }

    *ppSocketStream = new CSocketStream(hSocket);

    return *ppSocketStream != NULL ?S_OK:E_OUTOFMEMORY;
}


CCameraRecordAutoLink::CCameraRecordAutoLink(HV_SOCKET_HANDLE hSocket,
        SEND_RECORD_PARAM& cSendRecordParam ,
        char* pServerIp ,
        WORD16 wPort)
        : CCameraRecordLink(hSocket, cSendRecordParam)
        , m_hAutoLinkSocket(INVALID_SOCKET)
        , m_pAutoLinkSocketStream(NULL)
        , m_wAutoLinkPort(wPort)
        , m_fConnected(false)
{
    if (strlen(pServerIp)< MAX_IP_LEN)
    {
        strcpy(m_szAutoLinkIP , pServerIp);
    }

}
CCameraRecordAutoLink::~CCameraRecordAutoLink()
{

}


HRESULT CCameraRecordAutoLink::SetConnectionRequst()
{
    const int ciXmlMaxLen = 1024 << 4; //16k
    static char* pSendBuf = new char[ciXmlMaxLen];
    if (NULL == pSendBuf)
    {
        return E_OUTOFMEMORY;
    }
    memset(pSendBuf , 0 , ciXmlMaxLen);
    int iSendBufLen = 0;

    char rgszArgId[10][IVN_MAX_SIZE];
    char rgszArgValue[10][IVN_MAX_SIZE];

    strcpy(rgszArgId[0] , "SN");
    GetSN(rgszArgValue[0] , sizeof(rgszArgValue[0]));

    strcpy(rgszArgId[1] , "SafeSaverEnable");
    sprintf(rgszArgValue[1] , "%d" , IsSafeSaverEnable());

    iSendBufLen = BuildHvCmdXml(pSendBuf , "ConnectionRequest" , 2, rgszArgId ,rgszArgValue);

    UINT iSendLen = 0;

    CAMERA_CMD_HEADER cCmdHeader;
    cCmdHeader.dwID = CAMERA_XML_EXT_CMD;
    cCmdHeader.dwInfoSize = iSendBufLen+1;

    HRESULT hr = m_pAutoLinkSocketStream->Write((char*)&cCmdHeader, sizeof(cCmdHeader), &iSendLen);
    if (S_OK != hr || iSendLen != sizeof(cCmdHeader))
    {
        HV_Trace(5 , "SetConnectionRequst cmdHeader: %d\n", iSendLen);
        return hr;
    }

    hr = m_pAutoLinkSocketStream->Write(pSendBuf, cCmdHeader.dwInfoSize, &iSendLen);
    if (S_OK != hr || iSendLen != cCmdHeader.dwInfoSize)
    {
        HV_Trace(5 , "read cRespondHeader len: %d\n", iSendLen);
        return hr;
    }
    else
    {
        UINT iRespone = 0;
        CAMERA_CMD_RESPOND cRespondHeader;

        hr = m_pAutoLinkSocketStream->Read((char*)&cRespondHeader, sizeof(cRespondHeader), &iRespone);
        if (S_OK != hr || iRespone != sizeof(cRespondHeader) || cRespondHeader.dwResult != 0)
        {
            HV_Trace(5 , "SetConnectionRequst read respone fail!\n");
        }
    }
    return hr;
}

bool CCameraRecordAutoLink::IsRecordListEmpty()
{
    SemPend(&m_hSemListRecord);
    bool fIsRecordListEmpty = m_lstRecord.empty();
    SemPost(&m_hSemListRecord);

    HV_Trace(5, "m_fSendHistoryEnable=%d,m_fConnected=%d,fIsRecordListEmpty=%d", m_fSendHistoryEnable, m_fConnected, fIsRecordListEmpty);

    if(!fIsRecordListEmpty)
    {
        return false;
    }
    if(!m_fConnected)
    {
        return false;
    }
    if(!m_fSendHistoryEnable)
    {
        return false;
    }
    return true;
}

HRESULT DisConnectClientLink(HV_SOCKET_HANDLE& hSocket ,HvCore::IHvStream** ppSocketStream)
{
    if ( NULL != *ppSocketStream )
    {
        delete *ppSocketStream;
        *ppSocketStream = NULL;
    }

    if ( INVALID_SOCKET != hSocket )
    {
        CloseSocket(hSocket);
        hSocket = INVALID_SOCKET;
    }

    return S_OK;
}

HRESULT CCameraRecordAutoLink::Run(void* pvParam)
{
    int iWaitResultMs = 1000;
    CCameraDataReference* pRefRecord = NULL;
    CCameraData* pRecord = NULL;

    HRESULT hr = E_FAIL;

    m_fConnected = false;

    DWORD32 dwLastThrodTime = GetSystemTick();
    while (!m_fExit)
    {
        if (0 == m_dwThreadStatus)
        {
            ClearCCameraData();
        }

        if (!m_fConnected)
        {
            HV_Trace(5 , "Reconnect! \n");
            HV_Sleep(5000);

            if (FAILED(ConnectToServer(m_szAutoLinkIP ,m_wAutoLinkPort ,m_hAutoLinkSocket , &m_pAutoLinkSocketStream)))
            {
                m_dwThreadStatus = 0;
                HV_Trace(5 , "ConnectToServer error!\n");
                continue;
            }
            else
            {
                HV_Trace(5 , "ConnectToServer OK!\n");
                dwLastThrodTime = GetSystemTick();
                m_fConnected = true;
                m_fSendHistoryEnable = false;
                m_dwThreadStatus = 1;
            }

            //g_cCameraConnectedLog.AddConnectedItem(m_dwAutoConnectedLogId, m_hAutoLinkSocket, m_wAutoLinkPort);
            if (FAILED(SetConnectionRequst()))
            {
                HV_Trace(5 , "SetConnectionRequest error!\n");
                m_fConnected = false;
                m_dwThreadStatus = 0;
                continue;
            }

            // 新的连接需要初始化可变参数
            Init();
            // 处理连接命令
            hr = ProcessCmd(m_pAutoLinkSocketStream);
            if ( S_OK != hr )
            {
                m_fConnected = false;
                m_dwThreadStatus = 0;
                HV_Trace(5, "<link>Record Autolink ProcessCmd failed! Reconnect!\n");
                continue;
            }
            else
            {
                HV_Trace(5, "<link>Record Autolink ProcessCmd ok.\n");
            }

            HV_Trace(5, "<link>Record Autolink start...\n");
        }

        if ( m_fSendHistoryEnable
                && NULL != m_cSendRecordParam.pcSafeSaver
                && NULL == m_pHistoryPro )
        {

            if (m_pHistoryPro == NULL)
            {
                m_pHistoryPro = new CHistoryRecordProcess(this);
                if (m_pHistoryPro == NULL)
                {
                    HV_Trace(5, "<link>Create history processor failed!");
                    m_fConnected = false;
                    m_dwThreadStatus = 0;
                    // 如果是接历史数据完成后就断开连接
                    DisConnectClientLink(m_hAutoLinkSocket , &m_pAutoLinkSocketStream);
                    continue;
                }
                HV_Trace(5 , "start history process!\n");
                m_pHistoryPro->Start(NULL);
            }
        }

        // 检测历史数据是否发送完毕
        if (m_fSendHistoryEnable && m_pHistoryPro && m_pHistoryPro->IsEndOfRecord())
        {
            //提示接收历史结束
            if (FAILED(m_pAutoLinkSocketStream->Write(&m_cHistoryEnd, sizeof(m_cHistoryEnd), NULL)))
            {
                HV_Trace(5, "<link>Record send HistoryEnd failed!\n");
            }
            // 如果是接历史数据完成后就断开连接
            m_fConnected = false;
            m_dwThreadStatus = 0;
            // 如果是接历史数据完成后就断开连接
            DisConnectClientLink(m_hAutoLinkSocket , &m_pAutoLinkSocketStream);
            delete m_pHistoryPro;
            m_pHistoryPro = NULL;
            HV_Trace(5, "<link>接历史数据完成!\n");
            continue;
        }

        Lan2LedLight();

        //等待信号，处理实时发送结果
        hr = SemPend(&m_hSemRecord, iWaitResultMs);
        if (S_OK == hr)
        {
            pRefRecord = NULL;
            if (S_OK != GetCCameraDataReference(&pRefRecord))
            {
                HV_Trace(5, "GetCCameraDataReference fail!\n");
                HV_Sleep(100);
                continue;
            }

            if ( pRefRecord == NULL )
            {
                HV_Trace(5, "pRefRecord == NULL !\n");
                HV_Sleep(100);
                continue;
            }
            pRecord = pRefRecord->GetItem();
            if ( pRecord == NULL )
            {
                HV_Trace(5, "pRecord == NULL !\n");
                pRefRecord->Release();
                pRefRecord = NULL;
                HV_Sleep(100);
                continue;
            }

            bool fPeccancy = NULL != strstr((char *)(pRecord->pbInfo + 20), "违章:是");
            bool fSend = true;
            switch (RecordOutputType())
            {
            case 1 : //只输出违章车辆
                if (!fPeccancy)
                {
                    fSend = false;
                }
                break;
            case 2 : //只输出非违章结果
                if (fPeccancy)
                {
                    fSend = false;
                }
                break;
            }
            hr = S_OK;
            if ( fSend )  // 在不使能安全存储器的情况下，才发送实时结果。
            {
                HV_Trace(5 ,"Send 结果！\n");

                pRecord->header.dwInfoSize -= m_dwInfoOffset;
                pRecord->header.dwDataSize -= m_dwDataOffset;

                if ( FAILED(m_pAutoLinkSocketStream->Write(&pRecord->header, sizeof(pRecord->header), NULL))
                        || FAILED(m_pAutoLinkSocketStream->Write(pRecord->pbInfo + m_dwInfoOffset, pRecord->header.dwInfoSize, NULL))
                        || FAILED(m_pAutoLinkSocketStream->Write(pRecord->pbData + m_dwDataOffset, pRecord->header.dwDataSize, NULL)))
                {
                    HV_Trace(5, "<link>Record send failed recordsize:%d...\n", pRecord->header.dwDataSize + sizeof(pRecord->header) + pRecord->header.dwInfoSize);
                    hr = E_NETFAIL;
                }

                m_dwInfoOffset = 0;
                m_dwDataOffset = 0;
            }

            pRecord = NULL;
            pRefRecord->Release();
            pRefRecord = NULL;

            if ( FAILED(hr) )
            {
                m_dwThreadStatus = 0;
                DisConnectClientLink(m_hAutoLinkSocket , &m_pAutoLinkSocketStream);
                //g_cCameraConnectedLog.RemoveConnectedItem(m_dwAutoConnectedLogId);
                HV_Trace(5, "<link>Record link disconnect by send 实时结果 fail!\n");
                m_fConnected = false;
                continue;
            }
        }
        if (S_OK != hr || GetSystemTick() - dwLastThrodTime > 10000)	// 发送心跳包
        {
            dwLastThrodTime = GetSystemTick();
            if ( FAILED(m_pAutoLinkSocketStream->Write(&m_cameraThrob, sizeof(m_cameraThrob), NULL))
                    || FAILED(m_pAutoLinkSocketStream->Read(&m_cameraThrobResponse, sizeof(m_cameraThrobResponse), NULL))
                    || m_cameraThrobResponse.dwType != CAMERA_THROB_RESPONSE)
            {
                HV_Trace(5, "<link>Record send throb failed!\n");
                hr = E_NETFAIL;

                m_dwThreadStatus = 0;
                DisConnectClientLink(m_hAutoLinkSocket , &m_pAutoLinkSocketStream);
                //g_cCameraConnectedLog.RemoveConnectedItem(m_dwAutoConnectedLogId);
                HV_Trace(5, "<link>Record link disconnect!\n");
                m_fConnected = false;
                continue;

            }
        }
    }

    m_fExit = TRUE;

    HV_Trace(5, "<link>Record link exit!\n");

    DisConnectClientLink(m_hAutoLinkSocket , &m_pAutoLinkSocketStream);

    ClearCCameraData();

    return hr;
}


//---------================= CCameraRecordLinkCtrl ======================-------------------------

CCameraRecordLinkCtrl::CCameraRecordLinkCtrl()
        : CCameraLinkCtrlBase(CAMERA_RECORD_LINK_PORT, MAX_LINK_COUNT)
        , m_cCameraRecordAutoLink(NULL)
{
    m_wAutoLinkPort = (WORD16)-1;
    memset(m_szAutoLinkIP , 0 , sizeof(m_szAutoLinkIP));

    if (CreateSemaphore(&m_hSemEnum, 1, 1) != 0)
    {
        HV_Trace(5, "<link>CreateSemaphore Failed!\n");
    }
}

CCameraRecordLinkCtrl::~CCameraRecordLinkCtrl()
{
    DestroySemaphore(&m_hSemEnum);
}

HRESULT CCameraRecordLinkCtrl::EnumLock()
{
    SemPend(&m_hSemEnum);
    return S_OK;
}

HRESULT CCameraRecordLinkCtrl::EnumUnLock()
{
    SemPost(&m_hSemEnum);
    return S_OK;
}

HRESULT CCameraRecordLinkCtrl::SetParam(SEND_RECORD_PARAM * pParam)
{
    if ( pParam != NULL )
    {
        if (m_cSendRecordParam.pcSafeSaver)
        {
            HV_Trace(5, "m_cSendRecordParam.iOutputOnlyPeccancy before = %d, after = %d\n", m_cSendRecordParam.iOutputOnlyPeccancy, pParam->iOutputOnlyPeccancy);
            m_cSendRecordParam.iOutputOnlyPeccancy = pParam->iOutputOnlyPeccancy;
            m_cSendRecordParam.iSendRecordSpace = pParam->iSendRecordSpace;
        }
        else
        {
            HV_Trace(5, "init CCameraRecordLinkCtrl");
            m_cSendRecordParam = *pParam;
        }
        return S_OK;
    }
    return E_FAIL;
}

HRESULT CCameraRecordLinkCtrl::SendRecord(const SEND_RECORD* pRecord, DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh)
{
    if ( pRecord == NULL ) return E_POINTER;

    //EnumLock();
    //BOOL fLinkIsEmpty = m_cCameraRecordLinkEnum.IsEmpty();
    //EnumUnLock();

    if ( GetLinkCount() == 0 && m_cSendRecordParam.pcSafeSaver == NULL )
    {
        return S_FALSE;
    }

    if ( pRecord->pRefMemory == NULL )
    {
        HV_Trace(5, "<link>Record pRefMemory == NULL!\n");
        return E_POINTER;
    }

    CCameraDataReference* pRefRecord = NULL;
    if ( S_OK != CCameraDataReference::CreateCameraDataReference(&pRefRecord) )
    {
        HV_Trace(5, "<link>Record CreateCameraDataReference failed!\n");
        return E_OUTOFMEMORY;
    }
    CCameraData* pCameraRecord = pRefRecord->GetItem();
    if ( pCameraRecord == NULL )
    {
        HV_Trace(5, "<link>Record GetItem == NULL!\n");
        pRefRecord->Release();
        return E_OUTOFMEMORY;
    }

    //格式转换
    switch (pRecord->dwRecordType)
    {
    case CAMERA_RECORD_NORMAL :
        pCameraRecord->header.dwType = CAMERA_RECORD;
        break;
    case CAMERA_RECORD_STRING :
        pCameraRecord->header.dwType = CAMERA_STRING;
        break;
    case CAMERA_RECORD_STATUS :
        pCameraRecord->header.dwType = CAMERA_STATE;
        break;
    }
    pCameraRecord->header.dwInfoSize = 12 + (8 + pRecord->dwXMLSize);
    pCameraRecord->header.dwDataSize = pRecord->dwRecordSize;
    pCameraRecord->pbInfo = new BYTE8[pCameraRecord->header.dwInfoSize];

    pCameraRecord->pbData = NULL;
    // zhaopy
    pCameraRecord->pRefMemory = pRecord->pRefMemory;
    pCameraRecord->pRefMemory->AddRef();
    pCameraRecord->pRefMemory->GetData(&pCameraRecord->pbData);

    if ( pCameraRecord->pbInfo == NULL || pCameraRecord->pbData == NULL )
    {
        HV_Trace(5, "<link>Record memory failed!\n");
        if ( pCameraRecord->pbInfo != NULL )
        {
            delete [] pCameraRecord->pbInfo;
            pCameraRecord->pbInfo = NULL;
        }
        if(pCameraRecord->pbData)
        {
            delete[] pCameraRecord->pbData;
            pCameraRecord->pbData = NULL;
        }

        pRefRecord->Release();
        return E_OUTOFMEMORY;
    }
    DWORD32 dwOffset = 0;
    DWORD32 dwBlockID = 0;
    DWORD32 dwNodeLen = 0;
    //type
    dwBlockID = BLOCK_RECORD_TYPE;
    dwNodeLen = 4;
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)&pRecord->dwRecordType, pCameraRecord->pbInfo + dwOffset);
    dwOffset += 12;
    //xml
    dwBlockID = BLOCK_XML_TYPE;
    dwNodeLen =  pRecord->dwXMLSize;
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)pRecord->pbXML, pCameraRecord->pbInfo + dwOffset);
    dwOffset += (8 + pRecord->dwXMLSize);

    // save plate record
    if ( m_cSendRecordParam.pcSafeSaver != NULL )
    {
        if ( GetRealTimeLinkCount() == 0 || m_iSaveSafeType == 1)
        {
            if ( 1 == g_nHddCheckStatus )
            {
                if ( CAMERA_RECORD_NORMAL == pRecord->dwRecordType )
                {
                    int index = pRecord->iCurCarId;
                    m_cSendRecordParam.pcSafeSaver->SavePlateRecord(dwTimeMsLow, dwTimeMsHigh, &index, pRefRecord);
                }
            }
            else
            {
                HV_Trace(5, "CCameraRecordLinkCtrl::SendRecord g_nHddCheckStatus != 1\n");
            }
        }
    }

    EnumLock();
    m_cCameraRecordLinkEnum.EnumBase_Reset();
    CCameraRecordLink* pLink = NULL;
    while (m_cCameraRecordLinkEnum.EnumBase_Next(1, &pLink, NULL) == S_OK)
    {
        // 字符串信息也需实时发送
        if ( !pLink->IsSendHistoryPlate()
                || pCameraRecord->header.dwType == CAMERA_STRING )
        {
            pLink->SendRecord(pRefRecord);
        }
    }
    EnumUnLock();

    if (NULL != m_cCameraRecordAutoLink)
    {
        if ( !m_cCameraRecordAutoLink->IsSendHistoryPlate()
                || pCameraRecord->header.dwType == CAMERA_STRING )
        {
            m_cCameraRecordAutoLink->SendRecord(pRefRecord);
        }
    }
    pRefRecord->Release();

    return S_OK;
}

HRESULT CCameraRecordLinkCtrl::MakeNode(
    const DWORD32& dwID,
    const DWORD32& dwNodeLen,
    const unsigned char* pbNode,
    unsigned char* pbInfo
)
{
    if ( pbNode == NULL || pbInfo == NULL ) return E_POINTER;

    DWORD32 dwOffset = 0;
    memcpy(pbInfo + dwOffset, &dwID, 4);
    dwOffset += 4;
    memcpy(pbInfo + dwOffset, &dwNodeLen, 4);
    dwOffset += 4;
    memcpy(pbInfo + dwOffset, pbNode, dwNodeLen);

    return S_OK;
}

int CCameraRecordLinkCtrl::GetLinkCount()
{
    CCameraRecordLink* pLink = NULL;
    int iLinkCount = 0;
    EnumLock();
    m_cCameraRecordLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraRecordLinkEnum.EnumBase_Next(1, &pLink, NULL) )
    {
        if (pLink != NULL && pLink->GetStatus() == 1)
        {
            ++iLinkCount;
        }
    }
    if (m_fAutoLinkEnable)
    {
        ++iLinkCount;
    }

    EnumUnLock();
    return iLinkCount;
}

int CCameraRecordLinkCtrl::GetRealTimeLinkCount()
{
    CCameraRecordLink* pLink = NULL;
    int iLinkCount = 0;
    EnumLock();
    m_cCameraRecordLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraRecordLinkEnum.EnumBase_Next(1, &pLink, NULL) )
    {
        if (pLink != NULL && pLink->GetStatus() == 1 && pLink->IsSendHistoryPlate() == false)
        {
            ++iLinkCount;
        }
    }
    EnumUnLock();

    if(NULL != m_cCameraRecordAutoLink && m_cCameraRecordAutoLink->IsConnect() && !m_cCameraRecordAutoLink->IsSendHistoryPlate())
    {
        iLinkCount++;
    }

    return iLinkCount;
}

int CCameraRecordLinkCtrl::DeleteFreeLink()
{
    // 删除已断开的连接
    CCameraRecordLink* pLink = NULL;
    int iLinkCount = 0;
    EnumLock();
    m_cCameraRecordLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraRecordLinkEnum.EnumBase_Next(1, &pLink, NULL) )
    {
        if (pLink != NULL && pLink->GetStatus() == 1)
        {
            ++iLinkCount;
        }
        if ( (pLink != NULL) && (pLink->GetLastStatus() != S_OK) )
        {
            HV_Trace(5, "<link>Record link thread failed!\n");
        }
        pLink = NULL;
    }
    EnumUnLock();

    if (iLinkCount <= 0)
    {
        SetLan2LedOff();
    }

    return iLinkCount;
}

HRESULT CCameraRecordLinkCtrl::ClearLink()
{
    // 删除所有的连接
    CCameraRecordLink* pLink = NULL;
    EnumLock();
    m_cCameraRecordLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraRecordLinkEnum.EnumBase_Next(1, &pLink, NULL) )
    {
        if ( pLink != NULL )
        {
            pLink->Stop(-1);
            m_cCameraRecordLinkEnum.EnumBase_Remove(pLink);
            pLink = NULL;
        }
    }
    EnumUnLock();

    return S_OK;
}

HRESULT CCameraRecordLinkCtrl::CreateLinkThread()
{
    HRESULT hr = S_OK;
    if ( !m_cCameraRecordLinkEnum.IsEmpty() )
    {
        ClearLink();
    }

    for ( int i = 0; i < MAX_LINK_COUNT; ++i )
    {
        CCameraRecordLink* pNewThread = new CCameraRecordLink( INVALID_SOCKET, m_cSendRecordParam );
        if (pNewThread == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }
        else
        {
            if (FAILED(pNewThread->Start(NULL)))
            {
                delete pNewThread;
                pNewThread = NULL;
                hr = E_FAIL;
                break;
            }
            else
            {
                EnumLock();
                m_cCameraRecordLinkEnum.EnumBase_Add(pNewThread);
                EnumUnLock();
            }
        }
    }

    return hr;
}

// 新的连接
HRESULT CCameraRecordLinkCtrl::SetLink(HV_SOCKET_HANDLE& hSocket)
{
    HRESULT hr = E_FAIL;
    CCameraRecordLink* pLink = NULL;
    EnumLock();
    m_cCameraRecordLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraRecordLinkEnum.EnumBase_Next(1, &pLink, NULL) )
    {
        if ( pLink != NULL && (pLink->GetStatus() == 0) )
        {
            hr = pLink->SetSocket(hSocket);
            if ( S_OK == hr ) break;
        }
    }
    EnumUnLock();
    return hr;
}

HRESULT CCameraRecordLinkCtrl::StartAutoLink()
{
    HRESULT hr = S_OK;
    StopAutoLink();

    CCameraRecordAutoLink* pNewThread = new CCameraRecordAutoLink(INVALID_SOCKET , m_cSendRecordParam , m_szAutoLinkIP , m_wAutoLinkPort);

    if (pNewThread == NULL)
    {
        hr = E_OUTOFMEMORY;
    }
    else
    {
        if (FAILED(pNewThread->Start(NULL)))
        {
            delete pNewThread;
            pNewThread = NULL;
            hr = E_FAIL;
        }
        else
        {
            m_cCameraRecordAutoLink = pNewThread;
        }
    }
    return hr;
}
HRESULT CCameraRecordLinkCtrl::StopAutoLink()
{
    if (NULL != m_cCameraRecordAutoLink )
    {
        m_cCameraRecordAutoLink->Stop(-1);
        m_cCameraRecordAutoLink = NULL;
    }
    return S_OK;
}

HRESULT CCameraRecordLinkCtrl::SetAutoLink(bool fAutoLinkEnable ,char* pAutoLinkIP , WORD16 wAutoLinkPort)
{
    if (fAutoLinkEnable)
    {
        if (strlen(pAutoLinkIP) < MAX_IP_LEN)
        {
            memset(m_szAutoLinkIP , 0 , MAX_IP_LEN);
            strcpy(m_szAutoLinkIP , pAutoLinkIP);
            m_wAutoLinkPort = wAutoLinkPort;

            CCameraLinkCtrlBase::EnableAutoLink(true);

            return S_OK;
        }
        else
        {
            return E_FAIL;
        }
    }

    return S_OK;
}


bool CCameraRecordLinkCtrl::ThreadIsOk()
{
    bool fRet = true;
    if ( m_fExit ) return fRet;

    CCameraRecordLink* pLink = NULL;
    EnumLock();
    m_cCameraRecordLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraRecordLinkEnum.EnumBase_Next(1, &pLink, NULL) )
    {
        if ( pLink != NULL && !pLink->ThreadIsOk() )
        {
            fRet = false;
            break;
        }
    }
    EnumUnLock();

    if (fRet && m_dwThreadIsOkMs > 0)
    {
        DWORD32 dwCurTick = GetSystemTick();
        if ( dwCurTick < m_dwThreadIsOkMs )
        {
            HV_Trace(5, "CCameraRecordLinkCtrl::GetCurStatus [CurTick = %dms, LastOkTime = %dms]\n", dwCurTick, m_dwThreadIsOkMs);
            return true;
        }
        if ( int(dwCurTick - m_dwThreadIsOkMs) < MAX_WAIT_MS )
        {
            fRet = true;
        }
        else
        {
            fRet = false;
            HV_Trace(5, "CCameraRecordLinkCtrl::GetCurStatus [CurTick = %dms, LastOkTime = %dms]\n", dwCurTick, m_dwThreadIsOkMs);
        }
    }
    return fRet;
}
