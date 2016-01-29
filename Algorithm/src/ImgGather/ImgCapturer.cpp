#include "ImgGatherer.h"
#include "CamyuLinkOpt.h"
#include "HvSockUtils.h"
#include "hvtarget_ARM.h"
#include "platerecogparam.h"

const int CAMYU_LISTEN_PORT = 9999; /**< 抓拍图像侦听端口 */

using namespace HiVideo;

CCamyuReceiver::CCamyuReceiver(void* pParent, HV_SOCKET_HANDLE hSock)
{
    m_pParent = pParent;
    m_hSock = hSock;
}

CCamyuReceiver::~CCamyuReceiver()
{
    Stop();
}

HRESULT CCamyuReceiver::Run(void* pvParam)
{
    CImgCapturer *pImgCapturer = (CImgCapturer*)m_pParent;
    IMG_FRAME_PARAM& cImgFrameParam = pImgCapturer->m_cImgFrameParam;
    int nEddyType = pImgCapturer->m_nEddyType;
    CAP_CAM_PARAM *pCapCamParam = pImgCapturer->m_pCapCamParam;
    COuterControlImpl *pSignalMatch = pImgCapturer->m_pSignalMatch;

    const int IMAGE_BUFFER_SIZE = 1024 * 1024;
    char* pcBuf = new char[IMAGE_BUFFER_SIZE];

    if (pcBuf == NULL)
    {
        HV_Trace(5, "ImgCapturer.cpp: image buffer alloc failed!\n");
        return E_FAIL;
    }

    CY_FRAME_HEADER nullFH;
    memset(&nullFH, 0, sizeof(CY_FRAME_HEADER)); // 全0帧头

    // J系列相机心跳包为16S
    HvSetRecvTimeOut(m_hSock, 20000);

    // 获取相机IP地址
    struct hv_sockaddr name;
    int namelen = sizeof(struct hv_sockaddr);
    GetPeerName(m_hSock, &name, &namelen);

    char szCamIp[16];
    memset(szCamIp, 0, sizeof(szCamIp));
    sprintf(szCamIp, "%s", inet_ntoa(((struct hv_sockaddr_in*)&name)->sin_addr));

    IMG_FRAME frame;
    strcpy(frame.szCamIp, szCamIp);

    BOOL fIsSingleBoard = TRUE;

#ifdef _CAM_APP_
    fIsSingleBoard = FALSE;
#endif

    /* 接收图象，先接收帧头，然后再根据帧头的len字段判断图象数据的长度
    然后再接收图象数据，接收完后在接收下一副图像，如果接收到的帧头长宽为0，
    就关闭这个连接，继续等待下一个连接 */
    while (!m_fExit)
    {
        if (RecvAll(m_hSock, pcBuf, sizeof(CY_FRAME_HEADER)) != sizeof(CY_FRAME_HEADER))
        {
            HV_Sleep(50);
            break;
        }
        CY_FRAME_HEADER* pstHead = (CY_FRAME_HEADER*)pcBuf;
        if ( pstHead->wFrameType == 2 && pstHead->dwLen == 0 ) /* 通信心跳帧 */
        {
            unsigned long ack = LIB_HB_ACK;
            if (Send(m_hSock, (char*)&ack, 4, 0) != 4)
            {
                HV_Sleep(50);
                break;
            }
            continue;
        }

        if (pstHead->dwLen > (IMAGE_BUFFER_SIZE - sizeof(CY_FRAME_HEADER)))
        {
            HV_Trace(5, "The image's size is overflow! Size = %d\n", pstHead->dwLen);
            HV_Sleep(50);
            continue;
        }

        // 接收图象数据，图象长度由帧头len指示
        if (RecvAll(m_hSock, pcBuf + sizeof(CY_FRAME_HEADER), (int)pstHead->dwLen) != (int)pstHead->dwLen)
        {
            HV_Sleep(50);
            break;
        }

        int iSignalType = 0;
        if (GetCurrentMode() == PRM_HVC || !fIsSingleBoard) //判断是否HvcCam
        {
            //检测该图片帧是否是已设置的相机发来的，否则丢弃。
            int i;
            for (i = 0; i < cImgFrameParam.iCamNum; i++)
            {
                if ( strcmp(frame.szCamIp, cImgFrameParam.rgstrCamIP[i]) == 0 )
                {
                    break;
                }
            }
            if ( i == cImgFrameParam.iCamNum )
            {
                HV_Trace(3, "%s:Others cam connected, ignored them...\n", frame.szCamIp);
                continue;
            }
        }
        else    //非HvcCAM则检查抓拍相机
        {
            char szIP[20] = {0};
            bool fFound = false;
            GetIpString(pstHead->dwIp, szIP);

            for (int i = 0; i < MAX_CAP_CAM_COUNT; i++)
            {
                if (pCapCamParam
                        && strcmp(szIP, pCapCamParam->rgszRoadIP[i]) == 0
                        && pCapCamParam->nCamNum >= (i + 1))
                {
                    iSignalType = pCapCamParam->rgnSignalType[i];
                    fFound = true;
                    break;
                }
            }

            if (!fFound)
            {
                HV_Sleep(50);
                continue;
            }
        }

        //////////////////////////////////////////////////////////////////////////
        // 传递到外总控模块
        SIGNAL_INFO tempSignalInfo;

        tempSignalInfo.dwSignalTime = GetSystemTick();
        tempSignalInfo.dwInputTime = GetSystemTick();
        tempSignalInfo.dwValue = 0;
        tempSignalInfo.nType = iSignalType;
        tempSignalInfo.dwRoad = 0; //必须初始化否则为随机值
        tempSignalInfo.dwFlag = 0;
        tempSignalInfo.iModifyRoad = 0;

        // 创建JPEG引用对象
        IReferenceComponentImage *pRefImage;
        HRESULT hr = CreateReferenceComponentImage(
                         &pRefImage,
                         HV_IMAGE_JPEG,
                         pstHead->wWidth,
                         pstHead->wHeight,
                         0,
                         tempSignalInfo.dwSignalTime,
                         0,
                         "NULL",
                         2
                     );
        if (S_OK == hr)
        {
            pRefImage->SetCaptureFlag(TRUE);
            HV_COMPONENT_IMAGE imgFrame;
            pRefImage->GetImage(&imgFrame);
            memcpy(GetHvImageData(&imgFrame, 0), pcBuf + sizeof(CY_FRAME_HEADER), pstHead->dwLen);
            // JPEG图片高宽特殊意义
            imgFrame.iWidth = pstHead->dwLen;
            // 图片旋转
            if (nEddyType == 1)
            {
                imgFrame.iHeight = pstHead->wHeight | (pstHead->wWidth << 16);
            }
            else
            {
                imgFrame.iHeight = pstHead->wWidth | (pstHead->wHeight << 16);
            }
            pRefImage->SetImageSize(imgFrame);

            Lan1LedLight();

            if (GetCurrentMode() == PRM_HVC || !fIsSingleBoard)
            {
                // 传递到缓冲队列
                frame.pRefImage = pRefImage;
                ((CImgCapturer*)m_pParent)->PutOneFrame(frame);
                pRefImage->Release();
            }
            else if (pSignalMatch)
            {
                tempSignalInfo.pImage = NULL;
                tempSignalInfo.pImageLast = pRefImage;
                // 将信号加入信号队列
                if ( S_OK != pSignalMatch->AppendSignal(&tempSignalInfo) && tempSignalInfo.pImageLast != NULL )
                {
                    tempSignalInfo.pImageLast->Release();
                    tempSignalInfo.pImageLast = NULL;
                }
            }
        }
        else
        {
            HV_Trace(5, "ImgCaturer.cpp create image failed!\n");
        }
    }
    CloseSocket(m_hSock);
    if (pcBuf)
    {
        delete [] pcBuf;
    }
    m_fExit = TRUE;

    SetLan1LedOff();

    return S_OK;
}

/////////////////////////////////////////////
CCamyuListener::CCamyuListener(void* pParent)
{
    m_iStat = 1;
    m_iStatTimes = 0;
    m_fIsCamConnected = FALSE;

    m_pParent = pParent;
    for (int i = 0; i < MAX_IMG_RECV_COUNT; i++)
    {
        m_rgpCamyuReceiver[i] = NULL;
    }
}

CCamyuListener::~CCamyuListener()
{
    Stop();
    for (int i = 0; i < MAX_IMG_RECV_COUNT; i++)
    {
        if (m_rgpCamyuReceiver[i])
        {
            delete m_rgpCamyuReceiver[i];
        }
    }
    m_fIsCamConnected = FALSE;
}

HRESULT CCamyuListener::Run(void* pParam)
{
    HV_SOCKET_HANDLE hSock = Socket(AF_INET, SOCK_STREAM, 0);
    struct hv_sockaddr_in addrSrv;
    addrSrv.sin_addr.s_addr = htonl(INADDR_ANY);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(CAMYU_LISTEN_PORT);
    //强制绑定
    int iOpt = 1;
    int iOptLen = sizeof(iOpt);
    SetSockOpt(hSock, SOL_SOCKET, SO_REUSEADDR, (char*)&iOpt, iOptLen);

    if (-1 == bind(hSock, (hv_sockaddr*)&addrSrv, sizeof(struct hv_sockaddr)))
    {
        HV_Trace(3, "Bind socket error...\n");
        return E_FAIL;
    }

    if (Listen(hSock, MAX_IMG_RECV_COUNT) != 0)
    {
        HV_Trace(3, "Listen socket error...\n");
        return E_FAIL;
    }

    while (!m_fExit)
    {
        struct hv_sockaddr_in addrClient;
        int iSockLen = sizeof(struct hv_sockaddr_in);
        m_iStat = 1;
        HV_SOCKET_HANDLE hSockAccepted = Accept(hSock, (struct hv_sockaddr *)&addrClient, &iSockLen);
        m_iStat = 0;

        if (hSockAccepted < 0)
        {
            CloseSocket(hSockAccepted);
        }
        else
        {
            for (int i = 0; i < MAX_IMG_RECV_COUNT; i++)
            {
                if (NULL == m_rgpCamyuReceiver[i])
                {
                    m_rgpCamyuReceiver[i] = new CCamyuReceiver(m_pParent, hSockAccepted);
                    if (m_rgpCamyuReceiver[i])
                    {
                        m_rgpCamyuReceiver[i]->Start(NULL);
                    }
                    break;
                }
            }
        }

        int nCurConCount = 0;
        for (int i = 0; i < MAX_IMG_RECV_COUNT; i++)
        {
            if (m_rgpCamyuReceiver[i] && m_rgpCamyuReceiver[i]->m_fExit)
            {
                delete m_rgpCamyuReceiver[i];
                m_rgpCamyuReceiver[i] = NULL;
            }
            if (m_rgpCamyuReceiver[i])
            {
                ++nCurConCount;
            }
        }

        if (nCurConCount > 0)
        {
            m_fIsCamConnected = TRUE;
        }
        else
        {
            m_fIsCamConnected = FALSE;
        }
    }
    CloseSocket(hSock);

    return S_OK;
}

bool CCamyuListener::ThreadIsOk()
{
    if (0 == m_iStat)
    {
        if ( m_iStatTimes++ > 5 )
        {
            return false;
        }
    }
    else
    {
        m_iStatTimes = 0;
    }
    return true;
}

/////////////////////////////////////////////
CImgCapturer::CImgCapturer()
{
    m_pCamyuListener = new CCamyuListener(this);
}

CImgCapturer::~CImgCapturer()
{
    if (m_pCamyuListener)
    {
        delete m_pCamyuListener;
    }
}

HRESULT CImgCapturer::GetCurStatus(char* pszStatus, int nStatusSizes)
{
    return m_pCamyuListener->ThreadIsOk() ? S_OK : E_FAIL;
}

HRESULT CImgCapturer::SetImgFrameParam(const IMG_FRAME_PARAM& cParam)
{
    m_cImgFrameParam = cParam;
    return S_OK;
}

HRESULT CImgCapturer::SetCapCamParam(
    int nEddyType,
    CAP_CAM_PARAM *pCapCamParam,
    COuterControlImpl *pSignalMatch
)
{
    if (pCapCamParam == NULL || pSignalMatch == NULL)
    {
        return E_FAIL;
    }

    m_nEddyType = nEddyType;
    m_pCapCamParam = pCapCamParam;
    m_pSignalMatch = pSignalMatch;

    return S_OK;
}
