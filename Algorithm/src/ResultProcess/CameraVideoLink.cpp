// 该文件编码格式必须是WIN936

#include "CameraVideoLink.h"
#include "SocketStream.h"
#include "hvutils.h"
#include "misc.h"

using namespace std;
using namespace HiVideo;

/* HvUtilsClass.cpp */
extern int g_nHddCheckStatus;

//比较两个时间的秒差
static int SecondDiff(const DWORD32 dwTimeLowL
                      ,const DWORD32 dwTimeHighL
                      ,const DWORD32 dwTimeLowR
                      ,const DWORD32 dwTimeHighR
                     )
{
    DWORD64 dw64TimeL = dwTimeHighL;
    dw64TimeL = (dw64TimeL << 32) | dwTimeLowL;
    dw64TimeL = dw64TimeL / 1000;

    DWORD64 dw64TimeR = dwTimeHighR;
    dw64TimeR = (dw64TimeR << 32) | dwTimeLowR;
    dw64TimeR = dw64TimeR / 1000;

    return (dw64TimeL - dw64TimeR);
}

static bool IsSameSecond(REAL_TIME_STRUCT* prtLeft, REAL_TIME_STRUCT* prtRight)
{
    if ( prtLeft == NULL || prtRight == NULL )
    {
        return false;
    }

    if ( prtLeft->wYear == prtRight->wYear
            && prtLeft->wMonth == prtRight->wMonth
            && prtLeft->wDay == prtRight->wDay
            && prtLeft->wHour == prtRight->wHour
            && prtLeft->wMinute == prtRight->wMinute
            && prtLeft->wSecond == prtRight->wSecond
       )
    {
        return true;
    }

    return false;
}

static HRESULT MakeNode(
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

//--------------=============== CCameraVideoLink ==================------------------

CCameraVideoLink::CCameraVideoLink(HV_SOCKET_HANDLE hSocket, SEND_VIDEO_PARAM& cSendVideoParam)
        : m_hSocket(hSocket)
        , m_pSocketStream(NULL)
        , m_cSendVideoParam(cSendVideoParam)
        , m_dwStartTimeLow(0)
        , m_dwStartTimeHigh(0)
        , m_dwEndTimeLow(0)
        , m_dwEndTimeHigh(0)
        , m_fSendHistroyEnable(false)
        , m_dwThreadStatus(0)
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

    if (CreateSemaphore(&m_hSemVideo, 0, MAX_CAMERA_VIDEO_COUNT) != 0)
    {
        HV_Trace(5, "<link>Link CreateSemaphore Failed!\n");
    }
    if (CreateSemaphore(&m_hSemListVideo, 1, 1) != 0)
    {
        HV_Trace(5, "<link>Link CreateSemaphore Failed!\n");
    }

    m_pbDataBuffer = NULL;
    m_iVideoState = 0;

    m_dwThreadIsOkMs = GetSystemTick();
}

CCameraVideoLink::~CCameraVideoLink()
{
    Stop(-1);
    DestroySemaphore(&m_hSemVideo);
    DestroySemaphore(&m_hSemListVideo);
    if ( INVALID_SOCKET != m_hSocket )
    {
        CloseSocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }

    if ( NULL != m_pbDataBuffer)
    {
        delete[] m_pbDataBuffer;
        m_pbDataBuffer = NULL;
    }
    m_iVideoState = 0;
}

bool CCameraVideoLink::IsSendHistroyVideo()
{
    return m_fSendHistroyEnable;
}

HRESULT CCameraVideoLink::SendVideo(CCameraDataReference* pRefImage, DWORD32 dwFrameID )
{
    if ( pRefImage == NULL ) return E_POINTER;

    if ( m_fExit ) return S_OK;
    if ( m_dwThreadStatus == 0 ) return S_OK;

    if (m_iVideoState == 0)
    {
        return S_OK;
    }

    SemPend(&m_hSemListVideo);
    CCameraData* pImage = NULL;
    pImage = pRefImage->GetItem();


    bool fIsWriteVideoError = VideoDataSend( (unsigned char *)&pImage->header,
                              sizeof(pImage->header),
                              pImage->pbInfo,
                              pImage->header.dwInfoSize,
                              (unsigned char *)&pImage->header.dwDataSize,
                              sizeof(pImage->header.dwDataSize),
                              pImage->pbData,
                              pImage->header.dwDataSize,
                              dwFrameID);
    if (false == fIsWriteVideoError)
    {
        HV_Trace(5, "<link>CCameraVideoLink()->SendVideo WriteVideoError uFrameID %d\n",dwFrameID);
    }

    SemPost(&m_hSemListVideo);


    return S_OK;
}

HRESULT CCameraVideoLink::SetSocket(HV_SOCKET_HANDLE& hSocket)
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
    g_cCameraConnectedLog.AddConnectedItem(m_dwConnectedLogId, m_hSocket, CAMERA_VIDEO_LINK_PORT);
    return S_OK;
}

HRESULT CCameraVideoLink::ProcessHistroyVideo(HvCore::IHvStream* pSocketStream)
{
    HRESULT hr = S_OK;
    if ( pSocketStream == NULL )
    {
        return E_POINTER;
    }

    unsigned char* pVideoData = NULL;
    unsigned char* pVideoInfo = NULL;
    CAMERA_INFO_HEADER cVideoInfoHeader;
    const int siVideoDataLen = int(1.5 * 1024 * 1024);
    const int siVideoInfoLen = 32 * 1024;
#ifdef SINGLE_BOARD_PLATFORM
    IReferenceMemory* pRefMemory = NULL;
    if ( S_OK != CreateReferenceMemory(&pRefMemory, siVideoDataLen) )
    {
        HV_Trace(5, "can not alloc enough memory.\n");
        return E_OUTOFMEMORY;
    }
    PBYTE8 pbRecord = NULL;
    pRefMemory->GetData(&pbRecord);
    if (!pbRecord)
    {
        return S_FALSE;
    }
    pVideoData = pbRecord;
#else
    pVideoData = new unsigned char[siVideoDataLen];
    if ( pVideoData == NULL )
    {
        SAFE_DELETE_ARRAY(pVideoData);
        return E_OUTOFMEMORY;
    }
#endif
    pVideoInfo = new unsigned char[siVideoInfoLen];
    if ( pVideoInfo == NULL )
    {
        SAFE_DELETE_ARRAY(pVideoInfo);
        return E_OUTOFMEMORY;
    }

    DWORD32 dwSendThordMs = 0;

    while (!m_fExit)
    {
        m_dwThreadIsOkMs = GetSystemTick();
        HV_Sleep(m_cSendVideoParam.iSendHisVideoSpace * 100);
        dwSendThordMs += m_cSendVideoParam.iSendHisVideoSpace * 100;

        if ( dwSendThordMs > 1000 )
        {
            dwSendThordMs = 0;
            if ( FAILED(pSocketStream->Write(&m_cameraThrob, sizeof(m_cameraThrob), NULL)) )
            {
                HV_Trace(5, "<link>Video send throb failed!\n");
                hr = E_NETFAIL;
                break;
            }
            else
            {
                // 发送心跳包成功后需要接收心跳包回应
                if (pSocketStream->Read(&m_cameraThrobResponse, sizeof(m_cameraThrobResponse), NULL) != S_OK
                        || m_cameraThrobResponse.dwType != CAMERA_THROB_RESPONSE)
                {
                    HV_Trace(5, "<link>Video recv throb response failed!\n");
                    hr = E_NETFAIL;
                    break;
                }
            }
        }

        if (!g_nHddCheckStatus)
        {
            HV_Trace(5, "waiting hard disk check finish.\n");
            continue;
        }
        // 发送“可靠性保存”的视频数据
        if ( true == m_fSendHistroyEnable && NULL != m_cSendVideoParam.pcSafeSaver )
        {
            DWORD32 dwNowTimeLow = 0;
            DWORD32 dwNowTimeHigh = 0;
            GetSystemTime(&dwNowTimeLow, &dwNowTimeHigh);
            int iSecondDiff = SecondDiff(dwNowTimeLow, dwNowTimeHigh, m_dwStartTimeLow, m_dwStartTimeHigh);
            if (iSecondDiff < 10)//至少比当前时间小于10秒
            {
                continue;
            }

            if ( 1 == ComputeTimeMs(m_dwStartTimeLow, m_dwStartTimeHigh, m_dwEndTimeLow, m_dwEndTimeHigh) )
            {
                hr = m_cSendVideoParam.pcSafeSaver->GetVideoRecord(
                            m_dwStartTimeLow, m_dwStartTimeHigh,
                            &cVideoInfoHeader, pVideoInfo, siVideoInfoLen, pVideoData, siVideoDataLen);
                if ( S_OK ==  hr)
                {
                    if (cVideoInfoHeader.dwInfoSize > siVideoInfoLen || cVideoInfoHeader.dwDataSize > siVideoDataLen)
                    {
                        HV_Trace(5, "<link>video send faild because of error size, info size:%d, data size:%d", cVideoInfoHeader.dwInfoSize, cVideoInfoHeader.dwDataSize);
                    }
                    else
                    {
                        if ( FAILED(pSocketStream->Write(&cVideoInfoHeader, sizeof(cVideoInfoHeader), NULL))
                                || FAILED(pSocketStream->Write(pVideoInfo, cVideoInfoHeader.dwInfoSize, NULL))
                                || FAILED(pSocketStream->Write(pVideoData, cVideoInfoHeader.dwDataSize, NULL)) )
                        {
                            HV_Trace(5, "<link>video send faild videosize:%d...\n", cVideoInfoHeader.dwDataSize);
                            hr = E_NETFAIL;
                            break;
                        }
                    }
                    dwSendThordMs = 0;
                    TimeMsAddOneSecond(m_dwStartTimeLow, m_dwStartTimeHigh);
                }
                else if(E_RECORD_WAIT_TIMEOUT == hr)
                {
                    continue;
                }
                else
                {
                    if (S_OK != m_cSendVideoParam.pcSafeSaver->FindNextVideo(m_dwStartTimeLow, m_dwStartTimeHigh))
                    {
                        HV_Sleep(200);
                        continue;
                    }
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    m_fSendHistroyEnable = false;
#ifdef SINGLE_BOARD_PLATFORM
    pRefMemory->Release();
#else
    SAFE_DELETE_ARRAY(pVideoData);
#endif
    SAFE_DELETE_ARRAY(pVideoInfo);
    return hr;
}

// 初始化参数,连接时才确定的参数。
HRESULT CCameraVideoLink::Init()
{
    m_dwStartTimeLow = 0;
    m_dwStartTimeHigh = 0;
    m_dwEndTimeLow = 0;
    m_dwEndTimeHigh = 0;
    m_fSendHistroyEnable = false;

    return S_OK;
}

HRESULT CCameraVideoLink::DownloadVideoXML(const TiXmlElement* pCmdArgElement)
{
    if ( pCmdArgElement == NULL )
    {
        return E_POINTER;
    }
    // 例如： 2011.02.05_12:01:01
    char szEnable[32] = {0};
    char szBeginTime[32] = {0};
    char szEndTime[32] = {0};
    const char *pszAttribute = NULL;

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
    pszAttribute = pCmdArgElement->Attribute("EndTime");
    if (pszAttribute != NULL && strlen(pszAttribute) < 32)
    {
        strcpy(szEndTime, pszAttribute);
    }

    if ( 1 == strlen(szEnable)
            && 19 == strlen(szBeginTime)
            && 19 == strlen(szEndTime))
    {
        m_fSendHistroyEnable = ( atoi(szEnable) == 1 ? true : false );

        REAL_TIME_STRUCT cRealTime;
        DWORD32 dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond;
        sscanf(szEndTime, "%04d.%02d.%02d_%02d:%02d:%02d",
               &dwYear,&dwMonth,&dwDay,&dwHour, &dwMinute, &dwSecond);
        cRealTime.wYear = dwYear;
        cRealTime.wMonth = dwMonth;
        cRealTime.wDay = dwDay;
        cRealTime.wHour = dwHour;
        cRealTime.wMinute = dwMinute;
        cRealTime.wSecond = dwSecond;
        ConvertTimeToMs(&cRealTime, &m_dwEndTimeLow, &m_dwEndTimeHigh);

        sscanf(szBeginTime, "%04d.%02d.%02d_%02d:%02d:%02d",
               &dwYear,&dwMonth,&dwDay,&dwHour, &dwMinute, &dwSecond);
        cRealTime.wYear = dwYear;
        cRealTime.wMonth = dwMonth;
        cRealTime.wDay = dwDay;
        cRealTime.wHour = dwHour;
        cRealTime.wMinute = dwMinute;
        cRealTime.wSecond = dwSecond;
        ConvertTimeToMs(&cRealTime, &m_dwStartTimeLow, &m_dwStartTimeHigh);

        HV_Trace(5, "<link>Video Enable = %d %s, %s\n",
                 (int)m_fSendHistroyEnable, szBeginTime, szEndTime);
    }

    return S_OK;
}

HRESULT CCameraVideoLink::ProcessXMLInfo(unsigned char* pbInfo, const DWORD32& dwInfoSize)
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
            if (pElement != NULL && NULL != pElement->GetText() && 0 == strcmp("DownloadVideo", pElement->GetText()))
            {
                hr = DownloadVideoXML(pElement);
            }
        }
    }

    return hr;
}

HRESULT CCameraVideoLink::ProcessCmd(HvCore::IHvStream* pSocketStream)
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
    if ( S_OK == pSocketStream->Read(&cCmdHeader, sizeof(cCmdHeader), NULL) )
    {
        if ( cCmdHeader.dwInfoSize > 0 && cCmdHeader.dwInfoSize < (DWORD32)siMaxInfoSize
                && cCmdHeader.dwID == (DWORD32)CAMERA_XML_EXT_CMD )
        {
            pbInfo = new unsigned char[cCmdHeader.dwInfoSize];
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

HRESULT CCameraVideoLink::Run(void* pvParam)
{
    HRESULT hr = S_OK;

    bool fIsNewLink = true;
    DWORD32 dwLastSendThrobTime = GetSystemTick();
    while (!m_fExit)
    {
        m_dwThreadIsOkMs = GetSystemTick();
        if ( m_dwThreadStatus == 0 )
        {
            SemPend(&m_hSemListVideo);
            while (!m_lstVideoData.empty())
            {
                m_lstVideoData.pop_front();
            }
            SemPost(&m_hSemListVideo);
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
                HV_Trace(5, "<link>Video link ProcessCmd failed! use default setting\n");
            }
            else if ( hr == S_OK )
            {
                HV_Trace(5, "<link>Video link ProcessCmd ok.\n");
            }

            hr = S_OK;
            HV_Trace(5, "<link>Video link start...\n");
            m_iVideoState = 1;
            fIsNewLink = false;
        }
        else if ( fIsNewLink )
        {
            HV_Trace(5, "<link>ERROR:Video link unknow status! %d, %d, %d\n", m_hSocket, m_pSocketStream, m_dwThreadStatus);
            HV_Sleep(2000);
            continue;
        }
        // zhaopy 如果是接历史录像
        if ( m_fSendHistroyEnable
                && NULL != m_cSendVideoParam.pcSafeSaver )
        {
            hr = ProcessHistroyVideo(m_pSocketStream);

            if (S_OK == hr && !m_fExit)
            {
                //提示接收历史结束
                if (FAILED(m_pSocketStream->Write(&m_cHistoryEnd, sizeof(m_cHistoryEnd), NULL)))
                {
                    HV_Trace(5, "<link>Video send HistoryEnd failed!\n");
                }
            }

            // 如果是接历史数据完成后就断开连接
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
            fIsNewLink = true;
            g_cCameraConnectedLog.RemoveConnectedItem(m_dwConnectedLogId);
            SemPend(&m_hSemListVideo);
            while (!m_lstVideoData.empty())
            {
                m_lstVideoData.pop_front();
            }
            SemPost(&m_hSemListVideo);
            HV_Trace(5, "<link> Video link disconnect!\n");
            m_iVideoState = 0;
            HV_Sleep(1000);
            continue;
        }

        bool fHasData = false;
        SemPend(&m_hSemListVideo);
        SEND_VIDEO_DATA sendVideoData;
        if (m_lstVideoData.empty())
        {
            SemPost(&m_hSemListVideo);
            HV_Sleep(100);
        }
        else
        {
            fHasData = true;
            sendVideoData = m_lstVideoData.front();
            SemPost(&m_hSemListVideo);
        }

        if ( fHasData )
        {
            if ( false == m_fSendHistroyEnable )
            {
                if ( FAILED(m_pSocketStream->Write(sendVideoData.pbHeader, sendVideoData.dwHeaderSize, NULL))
                        || FAILED(m_pSocketStream->Write(sendVideoData.pbInfo, sendVideoData.dwInfoSize, NULL))
#ifdef SINGLE_BOARD_PLATFORM
                        || FAILED(m_pSocketStream->Write(sendVideoData.pbDataSize, sendVideoData.dwDataSizeSize, NULL))
#endif
                        || FAILED(m_pSocketStream->Write(sendVideoData.pbData, sendVideoData.dwDataSize, NULL)) )
                {
                    HV_Trace(5, "<send>CCameraVideoSend::Run video send error header:%d, info:%d, video:%d  FrameID: %d  \n",
                             sendVideoData.dwHeaderSize, sendVideoData.dwInfoSize, sendVideoData.dwDataSize,sendVideoData.dwFrameID);
                    hr = E_NETFAIL;
                }
            }

            SemPend(&m_hSemListVideo);
            m_lstVideoData.pop_front();
            SemPost(&m_hSemListVideo);
        }

        if ( FAILED(hr) )
        {
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
            fIsNewLink = true;
            m_iVideoState = 0;
            g_cCameraConnectedLog.RemoveConnectedItem(m_dwConnectedLogId);
            SemPend(&m_hSemListVideo);
            while (!m_lstVideoData.empty())
            {
                m_lstVideoData.pop_front();
            }
            SemPost(&m_hSemListVideo);
            HV_Trace(5, "<link>Video link disconnect!\n");

        }
        //修改成10秒钟发送一次心跳包，因为拔掉网线是发送成功但是接收失败，故用心跳包来确认连接是否正常
        if (!fIsNewLink && (S_OK != hr || GetSystemTick() - dwLastSendThrobTime > 10000))	// 发送心跳包
        {
            dwLastSendThrobTime = GetSystemTick();
            if ( FAILED(m_pSocketStream->Write(&m_cameraThrob, sizeof(m_cameraThrob), NULL)) )
            {
                HV_Trace(5, "<link>Video send throb failed!\n");
                hr = E_NETFAIL;
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
                fIsNewLink = true;
                m_iVideoState = 0;
                g_cCameraConnectedLog.RemoveConnectedItem(m_dwConnectedLogId);
                SemPend(&m_hSemListVideo);
                while (!m_lstVideoData.empty())
                {
                    m_lstVideoData.pop_front();
                }
                SemPost(&m_hSemListVideo);
                HV_Trace(5, "<link>Video link disconnect!\n");
            }
            else
            {
                // 发送心跳包成功后需要接收心跳包回应
                if (m_pSocketStream->Read(&m_cameraThrobResponse, sizeof(m_cameraThrobResponse), NULL) != S_OK
                        || m_cameraThrobResponse.dwType != CAMERA_THROB_RESPONSE)
                {
                    HV_Trace(5, "<link>Video recv throb response failed!\n");
                    hr = E_NETFAIL;
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
                    fIsNewLink = true;
                    m_iVideoState = 0;
                    g_cCameraConnectedLog.RemoveConnectedItem(m_dwConnectedLogId);
                    SemPend(&m_hSemListVideo);
                    while (!m_lstVideoData.empty())
                    {
                        m_lstVideoData.pop_front();
                    }
                    SemPost(&m_hSemListVideo);
                    HV_Trace(5, "<link>Video link disconnect!\n");
                }
            }
        }
    }

    m_fExit = TRUE;
    HV_Trace(5, "<link>Video link exit!\n");

    if ( INVALID_SOCKET != m_hSocket )
    {
        CloseSocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }

    SemPend(&m_hSemListVideo);
    while (!m_lstVideoData.empty())
    {
        m_lstVideoData.pop_front();
    }
    SemPost(&m_hSemListVideo);

    return hr;
}

bool CCameraVideoLink::ThreadIsOk()
{
    if ( m_fExit ) return true;

    DWORD32 dwCurTick = GetSystemTick();
    if ( dwCurTick < m_dwThreadIsOkMs )
    {
        HV_Trace(5, "CCameraVideoLink::GetCurStatus [CurTick = %dms, LastOkTime = %dms]\n", dwCurTick, m_dwThreadIsOkMs);
        return true;
    }
    if ( (dwCurTick - m_dwThreadIsOkMs) < (DWORD32)MAX_WAIT_MS )
    {
        return true;
    }
    else
    {
        HV_Trace(5, "CCameraVideoLink::GetCurStatus [CurTick = %dms, LastOkTime = %dms]\n", dwCurTick, m_dwThreadIsOkMs);
    }
    return false;
}

bool CCameraVideoLink::VideoDataSend( const unsigned char * pbHeader,const DWORD32 dwHeaderSize,
                                      const unsigned char * pbInfo,const DWORD32 dwInfoSize,
                                      const unsigned char * pbDataSize,const DWORD32 dwDataSizeSize,
                                      const unsigned char * pbData,const DWORD32 dwDataSize,
                                      const DWORD32 dwFrameID)
{

    if (NULL == m_pbDataBuffer)
    {
        if (m_cSendVideoParam.pcSafeSaver && 0 != g_nHddCheckStatus || NULL == m_cSendVideoParam.pcSafeSaver)
        {
            m_pbDataBuffer = new unsigned char[MAX_DATA_BUFFER_SIZE];
        }
        if (NULL == m_pbDataBuffer)
        {
            HV_Trace(5, "CCameraVideoLink::VideoDataSend m_pbDataBuffer == NULL");
            return false;
        }
    }
    SEND_VIDEO_DATA sendVideoData;
    sendVideoData.dwHeaderSize = dwHeaderSize;
    sendVideoData.dwInfoSize = dwInfoSize;
    sendVideoData.dwDataSizeSize = dwDataSizeSize;
    sendVideoData.dwDataSize = dwDataSize;
    sendVideoData.dwFrameID = dwFrameID;

    DWORD32 dwSize = dwHeaderSize + dwInfoSize + dwDataSizeSize + dwDataSize;
    unsigned char * pbCurPos;
    if (m_lstVideoData.empty() && dwSize < MAX_DATA_BUFFER_SIZE )
    {
        pbCurPos = m_pbDataBuffer;
        memcpy(pbCurPos,pbHeader,dwHeaderSize);
        sendVideoData.pbHeader = pbCurPos;

        pbCurPos = pbCurPos+dwHeaderSize;
        memcpy(pbCurPos,pbInfo,dwInfoSize);
        sendVideoData.pbInfo = pbCurPos;

        pbCurPos = pbCurPos+dwInfoSize;
        memcpy(pbCurPos,pbDataSize,dwDataSizeSize);
        sendVideoData.pbDataSize = pbCurPos;

        pbCurPos = pbCurPos+dwDataSizeSize;
        memcpy(pbCurPos,pbData,dwDataSize);
        sendVideoData.pbData = pbCurPos;

        m_lstVideoData.push_back(sendVideoData);
        return true;
    }
    else
    {
        SEND_VIDEO_DATA begin = m_lstVideoData.front();
        SEND_VIDEO_DATA end = m_lstVideoData.back();
        bool fIsLoop = false;
        if ( end.pbHeader < begin.pbHeader )
        {
            fIsLoop = true;
        }

        if ( false == fIsLoop )
        {
            if ( (m_pbDataBuffer+MAX_DATA_BUFFER_SIZE-1) - (end.pbData+end.dwDataSize-1) >= (int)dwSize )
            {
                pbCurPos = end.pbData+end.dwDataSize;
                memcpy(pbCurPos,pbHeader,dwHeaderSize);
                sendVideoData.pbHeader = pbCurPos;

                pbCurPos = pbCurPos+dwHeaderSize;
                memcpy(pbCurPos,pbInfo,dwInfoSize);
                sendVideoData.pbInfo = pbCurPos;

                pbCurPos = pbCurPos+dwInfoSize;
                memcpy(pbCurPos,pbDataSize,dwDataSizeSize);
                sendVideoData.pbDataSize = pbCurPos;

                pbCurPos = pbCurPos+dwDataSizeSize;
                memcpy(pbCurPos,pbData,dwDataSize);
                sendVideoData.pbData = pbCurPos;

                m_lstVideoData.push_back(sendVideoData);
                return true;
            }
            else
            {
                if ( begin.pbHeader - m_pbDataBuffer >= (int)dwSize )
                {
                    pbCurPos = m_pbDataBuffer;
                    memcpy(pbCurPos,pbHeader,dwHeaderSize);
                    sendVideoData.pbHeader = pbCurPos;

                    pbCurPos = pbCurPos+dwHeaderSize;
                    memcpy(pbCurPos,pbInfo,dwInfoSize);
                    sendVideoData.pbInfo = pbCurPos;

                    pbCurPos = pbCurPos+dwInfoSize;
                    memcpy(pbCurPos,pbDataSize,dwDataSizeSize);
                    sendVideoData.pbDataSize = pbCurPos;

                    pbCurPos = pbCurPos+dwDataSizeSize;
                    memcpy(pbCurPos,pbData,dwDataSize);
                    sendVideoData.pbData = pbCurPos;

                    m_lstVideoData.push_back(sendVideoData);
                    return true;
                }
                else
                {
                    HV_Trace(5, "<buffer> GetCameraVideoSend()->SendVideoData  begin end no empty  FrameID:%d\n",dwFrameID);
                    return false;
                }
            }
        }
        else
        {
            if ( begin.pbHeader - (end.pbData+end.dwDataSize-1) >= (int)dwSize)
            {
                pbCurPos = end.pbData+end.dwDataSize;
                memcpy(pbCurPos,pbHeader,dwHeaderSize);
                sendVideoData.pbHeader = pbCurPos;

                pbCurPos = pbCurPos+dwHeaderSize;
                memcpy(pbCurPos,pbInfo,dwInfoSize);
                sendVideoData.pbInfo = pbCurPos;

                pbCurPos = pbCurPos+dwInfoSize;
                memcpy(pbCurPos,pbDataSize,dwDataSizeSize);
                sendVideoData.pbDataSize = pbCurPos;

                pbCurPos = pbCurPos+dwDataSizeSize;
                memcpy(pbCurPos,pbData,dwDataSize);
                sendVideoData.pbData = pbCurPos;

                m_lstVideoData.push_back(sendVideoData);

                return true;
            }
            else
            {
                HV_Trace(5, "<buffer> GetCameraVideoSend()->SendVideoData  middle no empty  FrameID:%d\n",dwFrameID);
                return false;
            }
        }
    }
}

//---------================= CCameraVideoLinkCtrl ======================-------------------------

CCameraVideoLinkCtrl::CCameraVideoLinkCtrl()
        : CCameraLinkCtrlBase(CAMERA_VIDEO_LINK_PORT, MAX_LINK_COUNT)
        , m_pbVideoData(NULL)
        , m_iVideoSize(0)
        , m_dwVideoTimeLow(0)
        , m_dwVideoTimeHigh(0)

{
    if (CreateSemaphore(&m_hSemEnum, 1, 1) != 0)
    {
        HV_Trace(5, "<link>CreateSemaphore Failed!\n");
    }
    m_dwThreadIsOkMs = GetSystemTick();
}

CCameraVideoLinkCtrl::~CCameraVideoLinkCtrl()
{
    DestroySemaphore(&m_hSemEnum);
    if (m_pbVideoData)
    {
        delete []m_pbVideoData;
        m_pbVideoData = NULL;
    }
}

HRESULT CCameraVideoLinkCtrl::EnumLock()
{
    SemPend(&m_hSemEnum);
    return S_OK;
}

HRESULT CCameraVideoLinkCtrl::EnumUnLock()
{
    SemPost(&m_hSemEnum);
    return S_OK;
}

int CCameraVideoLinkCtrl::DeleteFreeLink()
{
    // 删除已断开的连接
    CCameraVideoLink* pLink = NULL;
    int iLinkCount = 0;
    EnumLock();
    m_cCameraVideoLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraVideoLinkEnum.EnumBase_Next(1, &pLink, NULL) )
    {
        iLinkCount++;
        if ( (pLink != NULL) && (pLink->GetStatus() == 0) )
        {
            iLinkCount--;
        }
        if ( (pLink != NULL) && (pLink->GetLastStatus() != S_OK) )
        {
            HV_Trace(5, "<link>Video link thread failed!\n");
        }
        pLink = NULL;
    }
    EnumUnLock();

    return iLinkCount;
}

HRESULT CCameraVideoLinkCtrl::ClearLink()
{
    // 删除所有的连接
    CCameraVideoLink* pLink = NULL;
    EnumLock();
    m_cCameraVideoLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraVideoLinkEnum.EnumBase_Next(1, &pLink, NULL) )
    {
        if ( pLink != NULL )
        {
            pLink->Stop(-1);
            m_cCameraVideoLinkEnum.EnumBase_Remove(pLink);
            pLink = NULL;
        }
    }
    EnumUnLock();

    return S_OK;
}

HRESULT CCameraVideoLinkCtrl::CreateLinkThread()
{
    HRESULT hr = S_OK;
    if ( !m_cCameraVideoLinkEnum.IsEmpty() )
    {
        ClearLink();
    }

    for ( int i = 0; i < MAX_LINK_COUNT; ++i )
    {
        CCameraVideoLink* pNewThread = new CCameraVideoLink( INVALID_SOCKET, m_cSendVideoParam );
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
                m_cCameraVideoLinkEnum.EnumBase_Add(pNewThread);
                EnumUnLock();
            }
        }
    }

    return hr;
}

// 新的连接
HRESULT CCameraVideoLinkCtrl::SetLink(HV_SOCKET_HANDLE& hSocket)
{
    HRESULT hr = E_FAIL;
    CCameraVideoLink* pLink = NULL;
    EnumLock();
    m_cCameraVideoLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraVideoLinkEnum.EnumBase_Next(1, &pLink, NULL) )
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

HRESULT CCameraVideoLinkCtrl::SendCameraVideo(const SEND_CAMERA_VIDEO* pVideo)
{
    if ( pVideo == NULL ) return E_POINTER;

    EnumLock();
    BOOL fLinkIsEmpty = m_cCameraVideoLinkEnum.IsEmpty();
    EnumUnLock();

    if ( fLinkIsEmpty && m_cSendVideoParam.pcSafeSaver == NULL )
    {
        return S_FALSE;
    }

    // zhaopy
    if ( pVideo->pRefImage == NULL )
    {
        return E_POINTER;
    }

    CCameraDataReference* pRefVideo = NULL;
    if ( S_OK != CCameraDataReference::CreateCameraDataReference(&pRefVideo) )
    {
        HV_Trace(5, "<link>Video CreateCameraDataReference failed!\n");
        return E_OUTOFMEMORY;
    }
    CCameraData* pCameraVideo = pRefVideo->GetItem();
    if ( pCameraVideo == NULL )
    {
        pRefVideo->Release();
        return E_OUTOFMEMORY;
    }

    //格式转换
    pCameraVideo->header.dwType = CAMERA_VIDEO;
    pCameraVideo->header.dwInfoSize = 12 + 12 + 16 + (8+sizeof(VideoExtInfo)) + 12 + 12 + 12;
    pCameraVideo->header.dwDataSize = pVideo->dwVideoSize;
    pCameraVideo->pbInfo = new BYTE8[pCameraVideo->header.dwInfoSize];

    pCameraVideo->pbData = NULL;
    HV_COMPONENT_IMAGE frame;
    pCameraVideo->pRefImage = pVideo->pRefImage;
    pCameraVideo->pRefImage->AddRef();
    pCameraVideo->pRefImage->GetImage(&frame);
    pCameraVideo->pbData = GetHvImageData(&frame, 0);
    if ( pCameraVideo->pbInfo == NULL || pCameraVideo->pbData == NULL )
    {
        HV_Trace(5, "<link>Video memory failed!\n");

        if ( pCameraVideo->pbInfo != NULL )
        {
            delete pCameraVideo->pbInfo;
            pCameraVideo->pbInfo = NULL;
        }

        pRefVideo->Release();
        return E_OUTOFMEMORY;
    }

    DWORD32 dwOffset = 0;
    DWORD32 dwBlockID = 0;
    DWORD32 dwNodeLen = 0;
    //video type
    dwBlockID = BLOCK_VIDEO_TYPE;
    dwNodeLen = 4;
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)&pVideo->dwVideoType, pCameraVideo->pbInfo + dwOffset);
    dwOffset += 12;
    //frame type
    dwBlockID = BLOCK_FRAME_TYPE;
    dwNodeLen = 4;
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)&pVideo->dwFrameType, pCameraVideo->pbInfo + dwOffset);
    dwOffset += 12;
    //time
    dwBlockID = BLOCK_VIDEO_TIME;
    dwNodeLen = 8;
    unsigned char rgbTime[8];
    memcpy(rgbTime, &pVideo->dwTimeLow, 4);
    memcpy(rgbTime + 4, &pVideo->dwTimeHigh, 4);
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)rgbTime, pCameraVideo->pbInfo + dwOffset);
    dwOffset += 16;
    //ExtInfo
    dwBlockID = BLOCK_VIDEO_EXT_INFO;
    dwNodeLen = sizeof(VideoExtInfo);
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)&pVideo->cVideoExtInfo, pCameraVideo->pbInfo + dwOffset);
    dwOffset += (8+sizeof(VideoExtInfo));
    //Y
    dwBlockID = BLOCK_VIDEO_Y;
    dwNodeLen = 4;
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)&pVideo->dwY, pCameraVideo->pbInfo + dwOffset);
    dwOffset += 12;
    //图像的宽度和高度
    dwBlockID = BLOCK_VIDEO_WIDTH;
    dwNodeLen = 4;
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)&pVideo->dwWidth, pCameraVideo->pbInfo + dwOffset);
    dwOffset += 12;

    dwBlockID = BLOCK_VIDEO_HEIGHT;
    dwNodeLen = 4;
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)&pVideo->dwHeight, pCameraVideo->pbInfo + dwOffset);
    dwOffset += 12;
    //是否要保存缓存中的历史数据
    bool fNewSeg = false;
#ifndef SINGLE_BOARD_PLATFORM
    if (pVideo->dwFrameType == CAMERA_FRAME_I) //只有I帧时才判断时间是否同一秒
#endif
    {
        REAL_TIME_STRUCT rtsVideoTime;
        REAL_TIME_STRUCT rtsLastVideoTime;
        ConvertMsToTime(pVideo->dwTimeLow, pVideo->dwTimeHigh, &rtsVideoTime);
        ConvertMsToTime(m_dwVideoTimeLow, m_dwVideoTimeHigh, &rtsLastVideoTime);
        if (false == IsSameSecond(&rtsVideoTime, &rtsLastVideoTime))//不是同一秒
        {
            fNewSeg = true;
        }
    }

    if (NULL == m_pbVideoData)
    {
        if (NULL != m_cSendVideoParam.pcSafeSaver && 1 == g_nHddCheckStatus)
        {
            m_pbVideoData = new unsigned char[MAX_VIDEO_SIZE];
        }
    }
    // zhaopy save video
    if (fNewSeg && NULL != m_pbVideoData)//如果是新段，则保存前面的缓存
    {
        if (  m_iVideoSize > 0 && m_cSendVideoParam.pcSafeSaver != NULL )
        {
            if (GetRealTimeLinkCount() == 0 || m_iSaveSafeType == 1)
            {
                if ( 1 == g_nHddCheckStatus )
                {
                    static BYTE8 szInfo[64 * 1024];
                    PBYTE8 pbInfo = szInfo;
                    memcpy(pbInfo, pCameraVideo->pbInfo, pCameraVideo->header.dwInfoSize);
                    if (CAMERA_VIDEO_H264 == pVideo->dwVideoType)
                    {
                        DWORD32 dwBlockID = BLOCK_FRAME_TYPE;
                        DWORD32 dwNodeLen = 4;
                        DWORD32 dwFrameType = CAMERA_FRAME_IP_ONE_SECOND;
                        MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)&dwFrameType, pbInfo + 12);
                    }
                    int size = pCameraVideo->header.dwDataSize;
                    pCameraVideo->header.dwDataSize = m_iVideoSize;
                    //DWORD32 dwTickSaveBegin = GetSystemTick();
                    if ( S_OK == m_cSendVideoParam.pcSafeSaver->SaveVideoRecord(m_dwVideoTimeLow, m_dwVideoTimeHigh,
                            &pCameraVideo->header, pbInfo, m_pbVideoData/*pCameraVideo->pbData*/) )
                    {
                        //HV_Trace(5, "<link> Save video ok. time = %d\n", GetSystemTick() - dwTickSaveBegin);
                    }
                    pCameraVideo->header.dwDataSize = size;
                    //save
                    m_iVideoSize = 0;
                }
                else
                {
                    HV_Trace(5, "CCameraVideoLinkCtrl::SendCameraVideo g_nHddCheckStatus != 1\n");
                }
            }
        }
        m_dwVideoTimeLow = pVideo->dwTimeLow;
        m_dwVideoTimeHigh = pVideo->dwTimeHigh;
    }
#ifdef SINGLE_BOARD_PLATFORM
    int iRedLightCount = 0, iFlag = 0;
    while ((pVideo->rcRedLight[iRedLightCount].right > pVideo->rcRedLight[iRedLightCount].left)
            && (pVideo->rcRedLight[iRedLightCount].bottom > pVideo->rcRedLight[iRedLightCount].top))
    {
        ++iRedLightCount;
    }

    if (iRedLightCount > 0)
    {
        iFlag = strlen("redlight");
    }

    if (NULL != m_pbVideoData && ((int)pVideo->dwVideoSize + (int)sizeof(pVideo->dwVideoSize) + (int)sizeof(HV_RECT)*iRedLightCount) + iFlag
            <= (int)(MAX_VIDEO_SIZE - m_iVideoSize) )
#else
    DWORD32 dwSegHeadSize = sizeof(pVideo->dwFrameType) + sizeof(pVideo->dwVideoSize);
    if (NULL != m_pbVideoData && ((int)pVideo->dwVideoSize + (int)dwSegHeadSize) <= (int)(MAX_VIDEO_SIZE - m_iVideoSize) )
#endif
    {
#ifdef SINGLE_BOARD_PLATFORM
        //图片大小
        memcpy(m_pbVideoData + m_iVideoSize, &pVideo->dwVideoSize, sizeof(pVideo->dwVideoSize));
        m_iVideoSize += sizeof(pVideo->dwVideoSize);

        if (iRedLightCount > 0)
        {
            //红灯标志
            memcpy(m_pbVideoData + m_iVideoSize, "redlight", iFlag);
            m_iVideoSize += iFlag;
            //红灯数量
            memcpy(m_pbVideoData + m_iVideoSize, &iRedLightCount, sizeof(iRedLightCount));
            m_iVideoSize += sizeof(iRedLightCount);
            //红灯坐标
            memcpy(m_pbVideoData + m_iVideoSize, pVideo->rcRedLight, sizeof(HV_RECT) * iRedLightCount);
            m_iVideoSize += sizeof(HV_RECT) * iRedLightCount;
        }
#else
        memcpy(m_pbVideoData + m_iVideoSize, &pVideo->dwFrameType, sizeof(pVideo->dwFrameType));
        m_iVideoSize += sizeof(pVideo->dwFrameType);
        memcpy(m_pbVideoData + m_iVideoSize, &pVideo->dwVideoSize, sizeof(pVideo->dwVideoSize));
        m_iVideoSize += sizeof(pVideo->dwVideoSize);
#endif
        memcpy(m_pbVideoData + m_iVideoSize, pCameraVideo->pbData, pVideo->dwVideoSize);
        m_iVideoSize += pVideo->dwVideoSize;
    }
#ifndef SINGLE_BOARD_PLATFORM
    EnumLock();
    m_cCameraVideoLinkEnum.EnumBase_Reset();
    CCameraVideoLink* pLink = NULL;
    while (m_cCameraVideoLinkEnum.EnumBase_Next(1, &pLink, NULL) == S_OK)
    {
        if ( !pLink->IsSendHistroyVideo() )
        {
            pLink->SendVideo(pRefVideo,pVideo->cVideoExtInfo.dwFrameID);
        }
    }
    EnumUnLock();
#endif
    pRefVideo->Release();

    // test...
    //HV_Trace(1, "<CCameraVideoLinkCtrl link>Global Data count:%d\n", CCameraData::GetCount());

    return S_OK;
}

bool CCameraVideoLinkCtrl::ThreadIsOk()
{
    bool fRet = true;
    if ( m_fExit ) return fRet;

    CCameraVideoLink* pLink = NULL;
    EnumLock();
    m_cCameraVideoLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraVideoLinkEnum.EnumBase_Next(1, &pLink, NULL) )
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
            HV_Trace(5, "CCameraVideoLinkCtrl::GetCurStatus [CurTick = %dms, LastOkTime = %dms]\n", dwCurTick, m_dwThreadIsOkMs);
            return true;
        }

        if ( (dwCurTick - m_dwThreadIsOkMs) < (int)MAX_WAIT_MS)
        {
            fRet = true;
        }
        else
        {
            HV_Trace(5, "CCameraVideoLinkCtrl::GetCurStatus [CurTick = %dms, LastOkTime = %dms]\n", dwCurTick, m_dwThreadIsOkMs);
            fRet = false;
        }
    }
    return fRet;
}

HRESULT CCameraVideoLinkCtrl::SetParam(const SEND_VIDEO_PARAM* pParam)
{
    if ( pParam != NULL )
    {
        if (m_cSendVideoParam.pcSafeSaver)
        {
            m_cSendVideoParam.iSendHisVideoSpace = pParam->iSendHisVideoSpace;
        }
        else
        {
            m_cSendVideoParam = *pParam;
        }
        return S_OK;
    }
    return E_FAIL;
}

int CCameraVideoLinkCtrl::GetRealTimeLinkCount()
{
    CCameraVideoLink* pLink = NULL;
    int iLinkCount = 0;
    EnumLock();
    m_cCameraVideoLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraVideoLinkEnum.EnumBase_Next(1, &pLink, NULL) )
    {
        if (pLink != NULL && pLink->GetStatus() == 1 && pLink->IsSendHistroyVideo() == false)
        {
            ++iLinkCount;
        }
    }
    EnumUnLock();
    return iLinkCount;
}

