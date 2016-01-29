// 该文件编码格式必须是WIN936

#include "CameraImageLink.h"
#include "hvutils.h"
#include "HvPciLinkApi.h"
#include "hvtarget_ARM.h"

using namespace std;
using namespace HiVideo;

//--------------=============== CCameraImageLink ==================------------------

CCameraImageLink::CCameraImageLink(HV_SOCKET_HANDLE hSocket)
        : m_hSocket(hSocket)
        , m_pSocketStream(NULL)
        , m_fEnableRecogVideo(FALSE)
        , m_fPanoramicMode(FALSE)
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

    if (CreateSemaphore(&m_hSemImage, 0, MAX_CAMERA_IMAGE_COUNT) != 0)
    {
        HV_Trace(5, "<link>Link CreateSemaphore Failed!\n");
    }
    if (CreateSemaphore(&m_hSemListImage, 1, 1) != 0)
    {
        HV_Trace(5, "<link>Link CreateSemaphore Failed!\n");
    }

    m_dwThreadIsOkMs = GetSystemTick();
}

CCameraImageLink::~CCameraImageLink()
{
    Stop(-1);
    DestroySemaphore(&m_hSemImage);
    DestroySemaphore(&m_hSemListImage);
    if ( INVALID_SOCKET != m_hSocket )
    {
        CloseSocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }
}

HRESULT CCameraImageLink::SendImage(CCameraDataReference* pRefImage)
{
    if ( pRefImage == NULL ) return E_POINTER;

    if ( m_fExit ) return S_OK;
    if ( m_dwThreadStatus == 0 ) return S_OK;

    if (!m_fEnableRecogVideo)
    {
        CCameraData* pDat = pRefImage->GetItem();
        DWORD32* pdwType = (DWORD32*)GetNodeData(pDat->pbInfo, pDat->header.dwInfoSize, BLOCK_IMAGE_TYPE, NULL);

        if (pdwType)
        {
            if (*pdwType == CAMERA_IMAGE_JPEG_SLAVE)
            {
                return S_OK;
            }
        }
    }

    SemPend(&m_hSemListImage);
    if ( (int)m_lstImage.size() >= MAX_CAMERA_IMAGE_COUNT )
    {
        SemPost(&m_hSemListImage);
        return E_OUTOFMEMORY;
    }
    pRefImage->AddRef();
    m_lstImage.push_back(pRefImage);
    SemPost(&m_hSemListImage);

    // 发送信号
    SemPost(&m_hSemImage);

    return S_OK;
}

HRESULT CCameraImageLink::SetImgType(const TiXmlElement* pCmdArgElement)
{
    const char *pszType = pCmdArgElement->Attribute("EnableRecogVideo");
    if (pszType)
    {
        if (strlen(pszType) > 0 && atoi(pszType) == 1)
        {
            m_fEnableRecogVideo = TRUE;
#ifndef SINGLE_BOARD_PLATFORM
            PciSendSlaveImage(1);
#endif
            HV_Trace(5, "m_fEnableRecogVideo is TRUE.\n");
        }
        else
        {
            m_fEnableRecogVideo = FALSE;
#ifndef SINGLE_BOARD_PLATFORM
            PciSendSlaveImage(0);
#endif
            HV_Trace(5, "m_fEnableRecogVideo is FALSE.\n");
        }
        return S_OK;
    }

    return E_FAIL;
}

HRESULT CCameraImageLink::SetSocket(HV_SOCKET_HANDLE& hSocket)
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
    g_cCameraConnectedLog.AddConnectedItem(m_dwConnectedLogId, m_hSocket, CAMERA_IMAGE_LINK_PORT);
    return S_OK;
}

// 初始化参数,连接时才确定的参数。
HRESULT CCameraImageLink::Init()
{
    m_fEnableRecogVideo = FALSE;
    m_fPanoramicMode = FALSE;
    return S_OK;
}

HRESULT CCameraImageLink::ProcessXMLInfo(unsigned char* pbInfo, const DWORD32& dwInfoSize)
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
        if ( 0 == strcmp("2.0", pRootElement->Attribute("ver")) )
        {
            const TiXmlElement* pElement = pRootElement->FirstChildElement("CmdName");
            if (pElement != NULL && NULL != pElement->GetText() && 0 == strcmp("SetImgType", pElement->GetText()))
            {
                hr = SetImgType(pElement);
            }
            else if(pElement != NULL && NULL != pElement->GetText()&& 0 == strcmp("PanoramicMode", pElement->GetText()))
            {
                m_fPanoramicMode = TRUE;
            }
        }
    }

    return hr;
}

HRESULT CCameraImageLink::ProcessCmd(HvCore::IHvStream* pSocketStream)
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
        if (    cCmdHeader.dwInfoSize > 0
                && cCmdHeader.dwInfoSize < (DWORD32)siMaxInfoSize
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

HRESULT CCameraImageLink::DisConnect()
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
    g_cCameraConnectedLog.RemoveConnectedItem(m_dwConnectedLogId);
    HV_Trace(5, "<link>Image link disconnect!\n");
    return S_OK;
}

HRESULT CCameraImageLink::Run(void* pvParam)
{
    m_fEnableRecogVideo = FALSE; //default setting
    m_fPanoramicMode = FALSE;

    HRESULT hr = S_OK;
    int iWaitResultMs = 1000;
    CCameraDataReference* pRefImage = NULL;
    CCameraData* pImage = NULL;
    bool fIsNewLink = true;
    DWORD32 dwLastSendThrobTime = GetSystemTick();
    while (!m_fExit)
    {
        m_dwThreadIsOkMs = GetSystemTick();
        if ( m_dwThreadStatus == 0 )
        {
            //删除所有的数据
            pRefImage = NULL;
            SemPend(&m_hSemListImage);
            while (!m_lstImage.empty())
            {
                pRefImage = m_lstImage.front();
                m_lstImage.pop_front();
                if ( pRefImage != NULL )
                {
                    pRefImage->Release();
                    pRefImage = NULL;
                }
            }
            SemPost(&m_hSemListImage);
            HV_Sleep(1000);
            continue;
        }

        if ( fIsNewLink && (m_hSocket != INVALID_SOCKET) && (m_pSocketStream != NULL) && (m_dwThreadStatus == 1))
        {
            // 新的连接需要初始化可变参数
            Init();
            // 处理连接命令
            HRESULT hr = ProcessCmd(m_pSocketStream);
            if ( S_OK != hr )
            {
                HV_Trace(5, "<link>Image link ProcessCmd failed! use default setting\n");
            }
            else if ( hr == S_OK )
            {
                HV_Trace(5, "<link>Image link ProcessCmd ok.\n");
            }
            HV_Trace(5, "<link>Image link start...\n");
            fIsNewLink = false;
        }
        else if ( fIsNewLink )
        {
            HV_Trace(5, "<link>ERROR:Image link unknow status! %d, %d, %d\n", m_hSocket, m_pSocketStream, m_dwThreadStatus);
            HV_Sleep(2000);
            continue;
        }

        hr = SemPend(&m_hSemImage, iWaitResultMs);
        //等待信号
        if (S_OK == hr)
        {
            SemPend(&m_hSemListImage);
            if ( m_lstImage.empty() )
            {
                SemPost(&m_hSemListImage);
                HV_Sleep(100);
                continue;
            }
            pRefImage = m_lstImage.front();
            m_lstImage.pop_front();
            SemPost(&m_hSemListImage);

            if ( pRefImage == NULL )
            {
                HV_Sleep(100);
                continue;
            }
            pImage = pRefImage->GetItem();
            if ( pImage == NULL )
            {
                pRefImage->Release();
                pRefImage = NULL;
                HV_Sleep(100);
                continue;
            }

            Lan2LedLight();

            if ( FAILED(m_pSocketStream->Write(&pImage->header, sizeof(pImage->header), NULL))
                    || FAILED(m_pSocketStream->Write(pImage->pbInfo, pImage->header.dwInfoSize, NULL))
                    || FAILED(m_pSocketStream->Write(pImage->pbData, pImage->header.dwDataSize, NULL)) )
            {
                HV_Trace(5, "<link>Image send faild imagesize:%d...\n", pImage->header.dwDataSize + sizeof(pImage->header) + pImage->header.dwInfoSize);
                hr = E_NETFAIL;
            }

            pImage = NULL;
            pRefImage->Release();
            pRefImage = NULL;

            if ( FAILED(hr) )
            {
                fIsNewLink = true;
                DisConnect();
            }
        }
        //修改成10秒钟发送一次心跳包，因为拔掉网线是发送成功但是接收失败，故用心跳包来确认连接是否正常
        if(!fIsNewLink && (S_OK != hr || GetSystemTick() - dwLastSendThrobTime > 10000))	// 发送心跳包
        {
            dwLastSendThrobTime = GetSystemTick();
            // 发送心跳包，并接收心跳包回应
            if ( FAILED(m_pSocketStream->Write(&m_cameraThrob, sizeof(m_cameraThrob), NULL))
                || FAILED(m_pSocketStream->Read(&m_cameraThrobResponse, sizeof(m_cameraThrobResponse), NULL))
                || m_cameraThrobResponse.dwType != CAMERA_THROB_RESPONSE)
            {
                HV_Trace(5, "<link>Image send throb failed!\n");
                hr = E_NETFAIL;
                fIsNewLink = true;
                DisConnect();
            }
        }
    }

    m_fExit = TRUE;

    HV_Trace(5, "<link>Image link exit!\n");

    if ( INVALID_SOCKET != m_hSocket )
    {
        CloseSocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }

    //删除所有的数据
    pRefImage = NULL;
    SemPend(&m_hSemListImage);
    while (!m_lstImage.empty())
    {
        pRefImage = m_lstImage.front();
        m_lstImage.pop_front();
        if ( pRefImage != NULL )
        {
            pRefImage->Release();
            pRefImage = NULL;
        }
    }
    SemPost(&m_hSemListImage);

    return hr;
}

bool CCameraImageLink::ThreadIsOk()
{
    if ( m_fExit ) return true;
    DWORD32 dwCurTick = GetSystemTick();
    if ( dwCurTick < m_dwThreadIsOkMs )
    {
        HV_Trace(5, "CCameraImageLink::GetCurStatus [CurTick = %dms, LastOkTime = %dms]\n", dwCurTick, m_dwThreadIsOkMs);
        return true;
    }
    if ( int(dwCurTick - m_dwThreadIsOkMs) < MAX_WAIT_MS )
    {
        return true;
    }
    else
    {
        HV_Trace(5, "CCameraImageLink::GetCurStatus [CurTick = %dms, LastOkTime = %dms]\n", dwCurTick, m_dwThreadIsOkMs);
    }

    return false;
}

//---------================= CCameraImageLinkCtrl ======================-------------------------

CCameraImageLinkCtrl::CCameraImageLinkCtrl()
        : CCameraLinkCtrlBase(CAMERA_IMAGE_LINK_PORT, MAX_LINK_COUNT)
{
    if (CreateSemaphore(&m_hSemEnum, 1, 1) != 0)
    {
        HV_Trace(5, "<link>CreateSemaphore m_hSemEnum Failed!\n");
    }
    if (CreateSemaphore(&m_hSemSendCameraImage, 1, 1) != 0)
    {
        HV_Trace(5, "<link>CreateSemaphore m_hSemSendCameraImage Failed!\n");
    }
}

CCameraImageLinkCtrl::~CCameraImageLinkCtrl()
{
    DestroySemaphore(&m_hSemEnum);
    DestroySemaphore(&m_hSemSendCameraImage);
}

HRESULT CCameraImageLinkCtrl::EnumLock()
{
    SemPend(&m_hSemEnum);
    return S_OK;
}

HRESULT CCameraImageLinkCtrl::EnumUnLock()
{
    SemPost(&m_hSemEnum);
    return S_OK;
}

HRESULT CCameraImageLinkCtrl::SendLock()
{
    SemPend(&m_hSemSendCameraImage);
    return S_OK;
}

HRESULT CCameraImageLinkCtrl::SendUnLock()
{
    SemPost(&m_hSemSendCameraImage);
    return S_OK;
}

HRESULT CCameraImageLinkCtrl::SetParam(const SEND_IMAGE_PARAM* pParam)
{
    return E_NOTIMPL;
}

HRESULT CCameraImageLinkCtrl::SendCameraImage(const SEND_CAMERA_IMAGE* pImage)
{
    if ( pImage == NULL ) return E_POINTER;

    // Comment by Shaorg: 由于传输图片有“调试码流”类型，
    // 即：这个接口会在不同线程中同时被用于传输主CPU的图片和从CPU的图片，所以需要加锁。
    SendLock();

    //EnumLock();
    //BOOL fLinkIsEmpty = m_cCameraImageLinkEnum.IsEmpty();
    //EnumUnLock();

    if ( GetLinkCount() == 0 )
    {
        SendUnLock();
        return S_FALSE;
    }

    CCameraDataReference* pRefImage = NULL;
    if ( S_OK != CCameraDataReference::CreateCameraDataReference(&pRefImage) )
    {
        HV_Trace(5, "<link>Image CreateCameraDataReference failed!\n");

        SendUnLock();
        return E_OUTOFMEMORY;
    }
    CCameraData* pCameraImage = pRefImage->GetItem();
    if ( pCameraImage == NULL )
    {
        pRefImage->Release();
        SendUnLock();
        return E_OUTOFMEMORY;
    }
    //格式转换
    pCameraImage->header.dwType = CAMERA_IMAGE;
    pCameraImage->header.dwInfoSize = 12 + 12 + 12 + 12 + 16 + (8+sizeof(ImageExtInfo)) + (8+sizeof(FpgaExtInfo)) + 12 + 12;
    pCameraImage->header.dwDataSize = pImage->dwImageSize;
    pCameraImage->pbInfo = new BYTE8[pCameraImage->header.dwInfoSize];

    pCameraImage->pbData = NULL;
    if ( pImage->pRefImage != NULL )
    {
        HV_COMPONENT_IMAGE image;
        pCameraImage->pRefImage = pImage->pRefImage;
        pCameraImage->pRefImage->AddRef();
        pCameraImage->pRefImage->GetImage(&image);
        pCameraImage->pbData = GetHvImageData(&image, 0);
    }
    else if ( pImage->pRefMemory != NULL )
    {
        pCameraImage->pRefMemory = pImage->pRefMemory;
        pCameraImage->pRefMemory->AddRef();
        pCameraImage->pRefMemory->GetData(&pCameraImage->pbData);
    }

    if ( pCameraImage->pbInfo == NULL || pCameraImage->pbData == NULL )
    {
        HV_Trace(5, "<link>Image memory failed!\n");
        pRefImage->Release();
        SendUnLock();
        return E_OUTOFMEMORY;
    }
    DWORD32 dwOffset = 0;
    DWORD32 dwBlockID = 0;
    DWORD32 dwNodeLen = 0;
    //type
    dwBlockID = BLOCK_IMAGE_TYPE;
    dwNodeLen = 4;
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)&pImage->dwImageType, pCameraImage->pbInfo + dwOffset);
    dwOffset += 12;
    //width
    dwBlockID = BLOCK_IMAGE_WIDTH;
    dwNodeLen = 4;
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)&pImage->dwWidth, pCameraImage->pbInfo + dwOffset);
    dwOffset += 12;
    //height
    dwBlockID = BLOCK_IMAGE_HEIGHT;
    dwNodeLen = 4;
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)&pImage->dwHeight, pCameraImage->pbInfo + dwOffset);
    dwOffset += 12;
    //time
    dwBlockID = BLOCK_IMAGE_TIME;
    dwNodeLen = 8;
    unsigned char rgbTime[8];
    memcpy(rgbTime, &pImage->dwTimeLow, 4);
    memcpy(rgbTime + 4, &pImage->dwTimeHigh, 4);
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)rgbTime, pCameraImage->pbInfo + dwOffset);
    dwOffset += 16;
    //image offset
    dwBlockID = BLOCK_IMAGE_OFFSET;
    dwNodeLen = 4;
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)&pImage->dwImageOffset, pCameraImage->pbInfo + dwOffset);
    dwOffset += 12;
    //ImageExtInfo
    dwBlockID = BLOCK_IMAGE_EXT_INFO;
    dwNodeLen = sizeof(ImageExtInfo);
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)&pImage->cImageExtInfo, pCameraImage->pbInfo + dwOffset);
    dwOffset += (8+sizeof(ImageExtInfo));
    //FpgaExtInfo
    dwBlockID = BLOCK_FPGA_EXT_INFO;
    dwNodeLen = sizeof(FpgaExtInfo);
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)&pImage->cFpgaExtInfo, pCameraImage->pbInfo + dwOffset);
    dwOffset += (8+sizeof(FpgaExtInfo));
    //JpegCompressRate
    dwBlockID = BLOCK_IMAGE_JPEG_CR;
    dwNodeLen = 4;
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)&pImage->dwJpegCompressRate, pCameraImage->pbInfo + dwOffset);
    dwOffset += 12;
    //Y
    dwBlockID = BLOCK_IMAGE_Y;
    dwNodeLen = 4;
    MakeNode(dwBlockID, dwNodeLen, (const unsigned char*)&pImage->dwY, pCameraImage->pbInfo + dwOffset);
    dwOffset += 12;

    //image data
    //memcpy(pCameraImage->pbData, pImage->pbImage, pImage->dwImageSize);

    EnumLock();
    m_cCameraImageLinkEnum.EnumBase_Reset();
    CCameraImageLink* pLink = NULL;
    while (m_cCameraImageLinkEnum.EnumBase_Next(1, &pLink, NULL) == S_OK)
    {
        pLink->SendImage(pRefImage);
    }
    EnumUnLock();
    pRefImage->Release();

    // test...
    //HV_Trace(1, "<CCameraImageLinkCtrl link>Global Data count:%d\n", CCameraData::GetCount());

    SendUnLock();
    return S_OK;
}

HRESULT CCameraImageLinkCtrl::MakeNode(
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

int CCameraImageLinkCtrl::GetLinkCount()
{
    CCameraImageLink* pLink = NULL;
    int iLinkCount = 0;
    EnumLock();
    m_cCameraImageLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraImageLinkEnum.EnumBase_Next(1, &pLink, NULL) )
    {
        if (pLink->GetStatus() == 1)
        {
            iLinkCount++;
        }
    }
    EnumUnLock();
    return iLinkCount;
}

int CCameraImageLinkCtrl::GetRunCaptureModeLinkCount()
{
    CCameraImageLink* pLink = NULL;
    int iLinkCount = 0;
    EnumLock();
    m_cCameraImageLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraImageLinkEnum.EnumBase_Next(1, &pLink, NULL) )
    {
        if (pLink->GetStatus() == 1
            && pLink->CheckIsPanoramicMode())
        {
            iLinkCount++;
        }
    }
    EnumUnLock();
    return iLinkCount;
}

int CCameraImageLinkCtrl::DeleteFreeLink()
{
    static int nLastLinkCount = 0;

    BOOL fEnableRecogVideo = FALSE;

    // 删除已断开的连接
    CCameraImageLink* pLink = NULL;
    int iLinkCount = 0;
    EnumLock();
    m_cCameraImageLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraImageLinkEnum.EnumBase_Next(1, &pLink, NULL) )
    {
        iLinkCount++;
        if ( (pLink != NULL) && (pLink->GetStatus() == 0) )
        {
            iLinkCount--;
        }
        else if (pLink != NULL)
        {
            if (pLink->IsEnableRecogVideo())
            {
                fEnableRecogVideo = TRUE;
            }
        }
        if ( (pLink != NULL) && (pLink->GetLastStatus() != S_OK) )
        {
            HV_Trace(5, "<link>Image link thread failed!\n");
        }
        pLink = NULL;
    }

    if (nLastLinkCount != iLinkCount)
    {
        nLastLinkCount = iLinkCount;
#ifndef SINGLE_BOARD_PLATFORM
        if (nLastLinkCount == 0)
        {
            PciSendSlaveImage(0);
        }
        else
        {
            if (fEnableRecogVideo)
            {
                PciSendSlaveImage(1);
            }
            else
            {
                PciSendSlaveImage(0);
            }
        }
#endif
    }

    EnumUnLock();

    return iLinkCount;
}

HRESULT CCameraImageLinkCtrl::ClearLink()
{
    // 删除所有的连接
    CCameraImageLink* pLink = NULL;
    EnumLock();
    m_cCameraImageLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraImageLinkEnum.EnumBase_Next(1, &pLink, NULL) )
    {
        if ( pLink != NULL )
        {
            pLink->Stop(-1);
            m_cCameraImageLinkEnum.EnumBase_Remove(pLink);
            pLink = NULL;
        }
    }
    EnumUnLock();

    return S_OK;
}

HRESULT CCameraImageLinkCtrl::CreateLinkThread()
{
    HRESULT hr = S_OK;
    if ( !m_cCameraImageLinkEnum.IsEmpty() )
    {
        ClearLink();
    }

    for ( int i = 0; i < MAX_LINK_COUNT; ++i )
    {
        CCameraImageLink* pNewThread = new CCameraImageLink( INVALID_SOCKET );
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
                m_cCameraImageLinkEnum.EnumBase_Add(pNewThread);
                EnumUnLock();
            }
        }
    }

    return hr;
}

// 新的连接
HRESULT CCameraImageLinkCtrl::SetLink(HV_SOCKET_HANDLE& hSocket)
{
    HRESULT hr = E_FAIL;
    CCameraImageLink* pLink = NULL;
    EnumLock();
    m_cCameraImageLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraImageLinkEnum.EnumBase_Next(1, &pLink, NULL) )
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

bool CCameraImageLinkCtrl::ThreadIsOk()
{
    bool fRet = true;
    if ( m_fExit ) return fRet;

    CCameraImageLink* pLink = NULL;
    EnumLock();
    m_cCameraImageLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cCameraImageLinkEnum.EnumBase_Next(1, &pLink, NULL) )
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
    	    HV_Trace(5, "CCameraImageLinkCtrl::GetCurStatus [CurTick = %dms, LastOkTime = %dms]\n", dwCurTick, m_dwThreadIsOkMs);
    	    return true;
    	}
    	if ( int(dwCurTick - m_dwThreadIsOkMs) < MAX_WAIT_MS )
    	{
    	    fRet = true;
    	}
    	else
    	{
    	    fRet = false;
    	    HV_Trace(5, "CCameraImageLinkCtrl::GetCurStatus [CurTick = %dms, LastOkTime = %dms]\n", dwCurTick, m_dwThreadIsOkMs);
    	}
	}
    return fRet;
}
