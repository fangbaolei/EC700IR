#include "CamyuServer.h"
#include "CamyuLinkOpt.h"
#include "HvSockUtils.h"
#include "hvtarget_ARM.h"
#include "DspLinkCmd.h"

#define CAMYUSERVER_IMG_QUE_MAX_COUNT 2

using namespace HiVideo;

CCamyuServer::CCamyuServer()
{
    //Comment by Shaorg: 屏蔽SIGPIPE异常
    sigset_t signal_mask;
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGPIPE);
    int rc = pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
    if (rc != 0)
    {
        HV_Trace(1, "pthread_sigmask is error.\n");
    }

    CreateSemaphore(&m_hSemQueCount, 0, CAMYUSERVER_IMG_QUE_MAX_COUNT);
    CreateSemaphore(&m_hSemQueCtrl, 1, 1);
    Start(NULL);
}

CCamyuServer::~CCamyuServer()
{
    Stop(-1);
    DestroySemaphore(&m_hSemQueCount);
    DestroySemaphore(&m_hSemQueCtrl);
}

HRESULT CCamyuServer::Run(void* pParam)
{
    HV_SOCKET_HANDLE hSockAccepted = INVALID_SOCKET;

    HV_SOCKET_HANDLE hSock = Socket(AF_INET, SOCK_STREAM, 0);
    struct hv_sockaddr_in addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_addr.s_addr = htonl(INADDR_ANY);
    addrSrv.sin_port = htons(CY_DATA_PORT);  //Camyu数据传输端口号
    bind(hSock, (hv_sockaddr*)&addrSrv, sizeof(struct hv_sockaddr));

    Listen(hSock, 1);  //仅支持单个连接
    while (!m_fExit)
    {
        if ( INVALID_SOCKET == hSockAccepted )
        {
            HV_Trace(5, "[CamyuServer]Waiting connect...\n");

            HRESULT hr = E_FAIL;
            hr = HvAccept(hSock, hSockAccepted, 4000);
            if ( E_FAIL == hr )
            {
                HV_Trace(5, "[CamyuServer]Accept is Error!\n");
                CloseSocket(hSockAccepted);
                hSockAccepted = INVALID_SOCKET;
                HV_Sleep(1000);
            }
            else if ( S_FALSE == hr )  // 等待超时
            {
                continue;
            }
            else if ( S_OK == hr )
            {
                int iSendTimeout = 1000;
                HvSetSendTimeOut(hSockAccepted, iSendTimeout);
                HV_Trace(5, "[CamyuServer]Connect is Ok!\n");
            }
        }
        else
        {
            IMG_FRAME imgFrame;
            PBYTE8 pbData = NULL;
            DWORD32 dwSize = 0;
            if ( S_OK == GetOneFrame(&imgFrame) )
            {
                CY_FRAME_HEADER cFrameHeader;
                memset(&cFrameHeader, 0 ,sizeof(cFrameHeader));

                HV_COMPONENT_IMAGE cImg;
                imgFrame.pRefImage->GetImage(&cImg);
                if ( HV_IMAGE_JPEG == cImg.nImgType )
                {
                    pbData = GetHvImageData(&cImg, 0);
                    dwSize = cImg.iWidth;

                    //将相关信息填入帧头
                    cFrameHeader.wWidth = (cImg.iHeight & 0x0000ffff);
                    cFrameHeader.wHeight = (cImg.iHeight & 0xffff0000) >> 16;
                    cFrameHeader.wBits = 8;
                    cFrameHeader.wFormat = 7;  //JPEG
                    cFrameHeader.wFrameType = 0;  //普通帧
                    cFrameHeader.dwLen = dwSize;
                }
                else if ( HV_IMAGE_H264 == cImg.nImgType )
                {
                    if ( FRAME_TYPE_H264_I == imgFrame.dwFrameType
                            || FRAME_TYPE_H264_P == imgFrame.dwFrameType )
                    {
                        pbData = GetHvImageData(&cImg, 0);
                        dwSize = cImg.iWidth;

                        cFrameHeader.wWidth = 1920;
                        cFrameHeader.wHeight = 1088;
                        cFrameHeader.wBits = 8;
                        cFrameHeader.wFormat = (WORD16)-1;
                        cFrameHeader.wFrameType = imgFrame.dwFrameType + 20;  //H.264帧类型（20--sw码流I帧，21--sw码流P帧）
                        cFrameHeader.dwLen = dwSize;
                    }
                }
                else
                {
                    HV_Trace(1, "Invalid Image Type!!!\n");

                    // 释放frame中的共享内存数据
                    SAFE_RELEASE(imgFrame.pRefImage);
                    continue;
                }

                if (Send(hSockAccepted, (char*)&cFrameHeader, sizeof(cFrameHeader), 0) != sizeof(cFrameHeader))
                {
                    CloseSocket(hSockAccepted);
                    hSockAccepted = INVALID_SOCKET;
                    HV_Sleep(1000);
                }
                else
                {
                    if (Send(hSockAccepted, (char*)pbData, dwSize, 0) != (int)dwSize)
                    {
                        CloseSocket(hSockAccepted);
                        hSockAccepted = INVALID_SOCKET;
                        HV_Sleep(1000);
                    }
                }

                // 传输完成，释放frame中的共享内存数据
                SAFE_RELEASE(imgFrame.pRefImage);
            }
        }
    }
    CloseSocket(hSockAccepted);
    CloseSocket(hSock);

    return S_OK;
}

HRESULT CCamyuServer::GetOneFrame(IMG_FRAME* pImgFrame)
{
    SemPend(&m_hSemQueCount);
    SemPend(&m_hSemQueCtrl);
    *pImgFrame = m_queImg.RemoveHead();
    SemPost(&m_hSemQueCtrl);
    return S_OK;
}

HRESULT CCamyuServer::PutOneFrame(IMG_FRAME imgFrame)
{
    SemPend(&m_hSemQueCtrl);
    if ( m_queImg.GetSize() < CAMYUSERVER_IMG_QUE_MAX_COUNT )
    {
        imgFrame.pRefImage->AddRef();
        m_queImg.AddTail(imgFrame);
        SemPost(&m_hSemQueCtrl);
        SemPost(&m_hSemQueCount, 1);
        return S_OK;
    }
    else
    {
        SemPost(&m_hSemQueCtrl);
        SAFE_RELEASE(imgFrame.pRefImage);
        return E_FAIL;
    }
}
