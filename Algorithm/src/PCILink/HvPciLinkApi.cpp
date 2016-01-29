#include "HvPciLinkApi.h"

#include <stdio.h>
#include <stdlib.h>
#include <linux/types.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <stropts.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "hvtarget_ARM.h"
#include "swdrv.h"
#include "misc.h"

using namespace HiVideo;

CHvPciLinkApi g_cHvPciLinkApi;  // 全局唯一

int g_nJpegCompressRate = 68;       // JPEG压缩率
int g_nCaptureCompressRate = 80;    // 抓拍图压缩率
int g_nJpegType = 0;                // JPEG类型
// 数据长度
const int PCILINK_DATA_MAX_LEN          = 4*1024*1024;  // 暂时限定最大只能一次接收 4MB 的数据
const int PCILINK_DATA_OFFSET           = 32;           // PCI数据偏移，为了保证用于PCI握手的内存绝对不会受到应用层的影响。
const int PCILINK_STATE_INFO_LEN        = 32*1024;
const int PCILINK_RESULT_INFO_LEN       = 32*1024;
const int PCILINK_CLOCK_SYNC_DATA_LEN   = 8;
const int PCILINK_PARAM_DATA_LEN        = 1*1024*1024;

CHvPciLinkApi::CHvPciLinkApi()
        : m_fd(-1)
        , m_nBufCount(0)
        , m_fShakeHandsFinish(false)
        , m_sktData(INVALID_SOCKET)
{
    m_dwCmdID = 0;
    if (CreateSemaphore(&m_hSemSendData, 1, 1) != 0)
    {
        HV_Trace(5, "<CHvPciLinkApi> m_hSemSendData Create Failed!\n");
    }
    for (int i = 0; i < MAX_PCI_BUF_COUNT; ++i)
    {
        if (CreateSemaphore(&m_rgApdInfo[i].hLock, 1, 1) != 0)
        {
            HV_Trace(5, "<CHvPciLinkApi> m_rgWaitLock[%d].hLock Create Failed!\n", i);
        }
        if (CreateSemaphore(&m_rgApdInfo[i].hSemaphore, 0, 1) != 0)
        {
            HV_Trace(5, "<CHvPciLinkApi> m_rgWaitLock[%d].hSemaphore Create Failed!\n", i);
        }
    }
}

CHvPciLinkApi::~CHvPciLinkApi()
{
    DestroySemaphore(&m_hSemSendData);
    for (int i = 0; i < MAX_PCI_BUF_COUNT; ++i)
    {
        DestroySemaphore(&m_rgApdInfo[i].hLock);
        DestroySemaphore(&m_rgApdInfo[i].hSemaphore);
    }
}

HRESULT CHvPciLinkApi::Init()
{
    m_fd = SwPciOpen();
    if (m_fd < 0)
    {
        return E_FAIL;
    }

    if ( S_OK != InitRWBuffer()
            || S_OK != InitRecvSocket() )
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CHvPciLinkApi::Uninit()
{
    HRESULT hr = E_FAIL;
    hr = UnInitRecvSocket();
    hr |= UnInitRWBuffer();
    if ( m_fd >= 0 )
    {
        hr |= ((-1 == SwPciClose(m_fd)) ? (E_FAIL) : (S_OK));
        m_fd = -1;
    }

    return hr;
}

HRESULT CHvPciLinkApi::InitRWBuffer()
{
    if (m_nBufCount)
    {
        UnInitRWBuffer();
    }

    memset(&m_rgRWBufs, 0, sizeof(m_rgRWBufs));

    // 取得rbuf在kernek中的地址
    m_nBufCount = SwInitPciRWBuffers(m_fd, m_rgRWBufs);
    if (m_nBufCount <= 0)
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CHvPciLinkApi::UnInitRWBuffer()
{
    for (int i = 0; i < m_nBufCount; ++i)
    {
        if (m_rgRWBufs[i].user != MAP_FAILED)
        {
            munmap(m_rgRWBufs[i].user, m_rgRWBufs[i].size);
        }
    }
    m_nBufCount = 0;
    return S_OK;
}

HRESULT CHvPciLinkApi::InitRecvSocket()
{
    UnInitRecvSocket();
    m_sktData = HvCreateSocket(AF_NETLINK, SOCK_RAW, NETLINK_SOCKET_PCI_EVENT);
    if (m_sktData == INVALID_SOCKET)
    {
        return E_FAIL;
    }

    m_nProcID = getpid();
    struct sockaddr_nl cSockAddr;
    memset(&cSockAddr, 0, sizeof(cSockAddr));
    cSockAddr.nl_family = AF_NETLINK;
    cSockAddr.nl_pid = m_nProcID; // 设置pid为自己的pid值
    cSockAddr.nl_groups = 0;

    if (Bind(m_sktData, (struct sockaddr*)&cSockAddr, sizeof(cSockAddr)) != 0)
    {
        return E_FAIL;
    }

    return SendToKernel(NETLINK_TYPE_PCI_PID);
}

HRESULT CHvPciLinkApi::UnInitRecvSocket()
{
    if (m_sktData != INVALID_SOCKET)
    {
        SendToKernel(NETLINK_TYPE_PCI_CLOSE);
        CloseSocket(m_sktData);
        m_sktData = INVALID_SOCKET;
    }
    return S_OK;
}

int CHvPciLinkApi::LockWait(int nPciHandle)
{
    return SemPend(&(m_rgApdInfo[nPciHandle].hLock));
}

int CHvPciLinkApi::UnLockWait(int nPciHandle)
{
    return SemPost(&(m_rgApdInfo[nPciHandle].hLock));
}

int CHvPciLinkApi::PendWait(int nPciHandle, int nTimeOut)
{
    return SemPend(&(m_rgApdInfo[nPciHandle].hSemaphore), nTimeOut);
}

int CHvPciLinkApi::PostWait(int nPciHandle)
{
    return SemPost(&(m_rgApdInfo[nPciHandle].hSemaphore));
}

HRESULT CHvPciLinkApi::SendToKernel(DWORD32 dwMsg)
{
    struct sockaddr_nl kpeer;
    struct nlmsghdr hdr;

    memset(&kpeer, 0, sizeof(kpeer));
    kpeer.nl_family = AF_NETLINK;
    kpeer.nl_pid    = 0;
    kpeer.nl_groups = 0;

    memset(&hdr, 0, sizeof(hdr));
    hdr.nlmsg_len = NLMSG_LENGTH(0); // 计算消息
    hdr.nlmsg_flags = 0;
    hdr.nlmsg_type  = dwMsg; // 设置自定义消息类型
    hdr.nlmsg_pid  = m_nProcID; // 设置发送者的PID

    int nSend = SendTo(
                    m_sktData,
                    (const char*)&hdr,
                    hdr.nlmsg_len,
                    0,
                    (struct sockaddr*)&kpeer,
                    sizeof(kpeer)
                );
    if (nSend != (int)hdr.nlmsg_len)
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CHvPciLinkApi::RecvMsg(int* pnPciHandle, DWORD32 *pdwMsg)
{
    if (m_sktData == INVALID_SOCKET)
    {
        return E_FAIL;
    }

    struct sockaddr_nl kpeer;
    struct packet_info info;
    int kpeerlen = sizeof(kpeer);
    int recvlen;

    recvlen = recvfrom(
                  m_sktData,
                  &info,
                  sizeof(info),
                  0,
                  (struct sockaddr*)&kpeer,
                  (socklen_t *)&kpeerlen
              );

    if (recvlen != sizeof(info))
    {
        return E_FAIL;
    }

    (*pnPciHandle) = info.index;
    (*pdwMsg) = info.hdr.nlmsg_type;
    return S_OK;
}

HRESULT CHvPciLinkApi::SendData(
    PCILINK_DATA_TYPE emDataType,
    LPVOID pbBuf,
    DWORD32 dwLen,
    LPVOID lpReturnData/* = NULL */,
    int *pnDataSize/* = 0 */,
    int nTimeOut/* = 2000 */
)
{
#ifdef _HV_CAMERA_PLATFORM_
    if (lpReturnData)
    {
        HRESULT hrTmp = S_OK;
        memcpy(lpReturnData, &hrTmp, sizeof(hrTmp));
    }
    return S_OK;
#endif

    if ((NULL == pbBuf && dwLen != 0)
            || (int)dwLen > PCILINK_DATA_MAX_LEN
            || (lpReturnData != NULL && pnDataSize == NULL))
    {
        return E_FAIL;
    }

    LockSendData();

    int nPciHandle = -1;
    HRESULT hrResult = E_FAIL;

    if (GetPciWriteHandle(&nPciHandle, (int)dwLen, 500) != S_OK)
    {
        HV_Trace(5, "<PciLink> GetPciWriteHandle failed! 0x%08X\n", emDataType);
        UnLockSendData();
        return E_FAIL;
    }

    unsigned char *pbDst = GetHeadPtr(nPciHandle);
    PCI_CMDHEAD_INFO cCmdHeadInfo;
    cCmdHeadInfo.emDataType = emDataType;
    cCmdHeadInfo.dwDataSize = dwLen;
    cCmdHeadInfo.dwCmdID = m_dwCmdID++;
    m_rgApdInfo[nPciHandle].dwCmdID = cCmdHeadInfo.dwCmdID;

    // 拷贝数据信息
    memcpy(pbDst, &cCmdHeadInfo, sizeof(cCmdHeadInfo));
    pbDst += sizeof(cCmdHeadInfo);

    // 拷贝数据
    memcpy(pbDst, pbBuf, dwLen);
    pbDst += dwLen;

    // 发送数据
    if (S_OK != PutPciWriteHandle(nPciHandle))
    {
        HV_Trace(5, "<PciLink> PutPciWriteHandle failed! 0x%08X\n", emDataType);
        UnLockSendData();
        goto Err;
    }

    UnLockSendData();

    // 等待并拷贝返回值
    if (S_OK != GetReturnValue(nPciHandle, lpReturnData, pnDataSize, nTimeOut))
    {
        HV_Trace(5, "<PciLink> GetReturnValue failed! CurType = 0x%08X\n", emDataType);
        goto Err;
    }

    hrResult = S_OK;

Err:
    // 释放缓存区
    if (S_OK != FreePciWriteHandle(nPciHandle))
    {
        HV_Trace(5, "<PciLink> FreePciWriteHandle failed! 0x%08X\n", emDataType);
        return E_FAIL;
    }

    return hrResult;
}

HRESULT CHvPciLinkApi::BeginSendData(int* pnPciHandle, PCILINK_DATA_TYPE emDataType)
{
    LockSendData();

    // 由于无法预知写入的数据大小，因此先填充6M
    if (S_OK != GetPciWriteHandle(pnPciHandle, 6 * 1024 * 1024, 500))
    {
        HV_Trace(5, "<PciLink> GetPciWriteHandle failed! 0x%08X\n", emDataType);
        UnLockSendData();
        return E_FAIL;
    }

    memset(&m_cCmdHeadInfo, 0, sizeof(m_cCmdHeadInfo));
    m_cCmdHeadInfo.emDataType = emDataType;
    return S_OK;
}

HRESULT CHvPciLinkApi::PutSendData(int nPciHandle, LPVOID pbBuf, DWORD32 dwLen)
{
    if (pbBuf == NULL)
    {
        return E_POINTER;
    }

    memcpy(GetDataPtr(nPciHandle) + m_cCmdHeadInfo.dwDataSize, pbBuf, dwLen);
    m_cCmdHeadInfo.dwDataSize += dwLen;

    return S_OK;
}

HRESULT CHvPciLinkApi::EndSendData(
    int nPciHandle,
    LPVOID lpReturnData/* = NULL*/,
    int *pnDataSize/* = 0*/,
    int nTimeOut/* = 1000*/
)
{
    HRESULT hrResult = E_FAIL;
    PCILINK_DATA_TYPE emDataType = m_cCmdHeadInfo.emDataType;

    if (lpReturnData != NULL && pnDataSize == NULL)
    {
        goto Err;
    }

    // 拷贝数据信息
    memcpy(GetHeadPtr(nPciHandle), &m_cCmdHeadInfo, sizeof(m_cCmdHeadInfo));

    // 发送数据
    if (S_OK != PutPciWriteHandle(nPciHandle))
    {
        HV_Trace(5, "<PciLink> PutPciWriteHandle failed! 0x%08X\n", emDataType);
        UnLockSendData();
        goto Err;
    }

    UnLockSendData();

    // 等待并拷贝返回值
    if (S_OK != GetReturnValue(nPciHandle, lpReturnData, pnDataSize, nTimeOut))
    {
        HV_Trace(5, "<PciLink> GetReturnValue failed! CurType = 0x%08X\n", emDataType);
        hrResult = S_OK;
        goto Err;
    }

    hrResult = S_OK;

Err:
    // 释放缓存区
    if (S_OK != FreePciWriteHandle(nPciHandle))
    {
        HV_Trace(5, "<PciLink> FreePciWriteHandle failed! 0x%08X\n", emDataType);
        return E_FAIL;
    }

    return hrResult;
}

HRESULT CHvPciLinkApi::BeginRecvData(int nPciHandle, PCI_CMDHEAD_INFO* pCmdHeadInfo)
{
    m_rgApdInfo[nPciHandle].dwRSize = 0;
    m_rgApdInfo[nPciHandle].dwWSize = 0;

    // 数据信息
    RecvData(nPciHandle, pCmdHeadInfo, sizeof(*pCmdHeadInfo));
    m_rgApdInfo[nPciHandle].dwRSize = sizeof(*pCmdHeadInfo);

    return S_OK;
}

HRESULT CHvPciLinkApi::RecvData(int nPciHandle, LPVOID pbBuf, DWORD32 dwDataSize)
{
    if (pbBuf == NULL)
    {
        return E_FAIL;
    }

    memcpy(pbBuf, GetHeadPtr(nPciHandle) + m_rgApdInfo[nPciHandle].dwRSize, dwDataSize);
    m_rgApdInfo[nPciHandle].dwRSize += dwDataSize;

    return S_OK;
}

HRESULT CHvPciLinkApi::EndRecvData(int nPciHandle, PCI_CMDHEAD_INFO cCmdHeadInfo)
{
    cCmdHeadInfo.dwDataSize = m_rgApdInfo[nPciHandle].dwWSize;
    memcpy(GetHeadPtr(nPciHandle), &cCmdHeadInfo, sizeof(cCmdHeadInfo));
    return PutPciReadHandle(nPciHandle);
}

HRESULT CHvPciLinkApi::WriteReturnValue(int nPciHandle, LPVOID pbBuf, DWORD32 dwDataSize)
{
    if (pbBuf == NULL)
    {
        return E_POINTER;
    }
    memcpy(GetDataPtr(nPciHandle) + m_rgApdInfo[nPciHandle].dwWSize, pbBuf, dwDataSize);
    m_rgApdInfo[nPciHandle].dwWSize += dwDataSize;
    return S_OK;
}

HRESULT CHvPciLinkApi::PutPciReadHandle(int nPciHandle)
{
    int ret = SwPciPutRecvBuf(m_fd, &m_rgRWBufs[nPciHandle]);

    int iTryTimes = 5;
    char szMsg[128] = {0};
    while( ret < 0 )
    {
        sprintf(szMsg, "SWPCI_IOCTL_PUT_RBUF(%d)! retry.", ret);
        perror("szMsg");
        if( --iTryTimes < 0 )
        {
            break;
        }
        ret = SwPciPutRecvBuf(m_fd, &m_rgRWBufs[nPciHandle]);
    }

    if( ret == 0 )
    {
        HV_Trace(5, "<pci>PutPciReadHandle failed(0)!");
        return E_FAIL;
    }
    if (ret < 0)
    {
        perror("SWPCI_IOCTL_PUT_RBUF!!!!!");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CHvPciLinkApi::GetPciWriteHandle(int* pnPciHandle, int nDataSize, int nTryTimes)
{
    if ( m_fShakeHandsFinish != true )
    {
        return S_FALSE;
    }

    if (m_fd < 0)
    {
        return S_FALSE;
    }

    if (NULL == pnPciHandle || nTryTimes <= 0)
    {
        return E_INVALIDARG;
    }

    int ret = 0;
    swpci_buffer cPciBufInfo;
    memset(&cPciBufInfo, 0, sizeof(cPciBufInfo));
    cPciBufInfo.size = nDataSize + sizeof(PCI_CMDHEAD_INFO);

    // 因为低层驱动机制修改后，导致获取写缓冲块的失败概率较大，所以加入重试机制。
    for (int i = 0; i < nTryTimes; ++i)
    {
        ret = SwPciGetSendBuf(m_fd, &cPciBufInfo);
        if (ret <= 0)
        {
            if (i == (nTryTimes - 1))
            {
                return E_FAIL;
            }
            // 出错或超时
            WdtHandshake();
            usleep(10000);
            continue;
        }
        break;
    }

    (*pnPciHandle) = ret;
    m_rgApdInfo[ret].dwSemStatus = 1;

    return S_OK;
}

HRESULT CHvPciLinkApi::PutPciWriteHandle(int nPciHandle)
{
    int ret = -1;
    int nTryTimes = 10;
    for (int i = 0; i < nTryTimes; ++i)
    {
        ret = SwPciPutSendBuf(m_fd, &m_rgRWBufs[nPciHandle]);
        if (ret <= 0)
        {
            if (i == (nTryTimes - 1))
            {
                return E_FAIL;
            }
            usleep(10000);
            continue;
        }
        break;
    }
    return S_OK;
}

HRESULT CHvPciLinkApi::FreePciWriteHandle(int nPciHandle)
{
    m_rgApdInfo[nPciHandle].dwSemStatus = 0;
    int ret = -1;
    int nTryTimes = 10;
    for (int i = 0; i < nTryTimes; ++i)
    {
        ret = SwPciFreeSendBuf(m_fd, &m_rgRWBufs[nPciHandle]);
        if (ret <= 0)
        {
            if (i == (nTryTimes - 1))
            {
                return E_FAIL;
            }
            usleep(10000);
            continue;
        }
        break;
    }
    return S_OK;
}

HRESULT CHvPciLinkApi::GetReturnValue(int nPciHandle, LPVOID lpBuf, int* pnBufSize, int nTimeOut)
{
    int nRet = PendWait(nPciHandle, nTimeOut);
    if (nRet < 0)
    {
        HV_Trace(5, "PendWait failed %d_%d", nRet, nPciHandle);
        return E_FAIL;
    }
    BOOL fIsDataValid = TRUE;
    // 超时
    if (nRet == 1)
    {
        LockWait(nPciHandle);
        if (SemGetValue(&(m_rgApdInfo[nPciHandle].hSemaphore)) == 0)
        {
            fIsDataValid = FALSE;
            m_rgApdInfo[nPciHandle].dwSemStatus = 2;
        }
        else
        {
            PendWait(nPciHandle, -1);
        }
        UnLockWait(nPciHandle);
    }

    if (fIsDataValid == FALSE)
    {
        HV_Trace(5, "wait failed %d_%d", nRet, nPciHandle);
        return E_FAIL;
    }

    if (lpBuf && pnBufSize)
    {
        PCI_CMDHEAD_INFO cCmdInfo;
        memcpy(&cCmdInfo, GetHeadPtr(nPciHandle), sizeof(cCmdInfo));
        if ((*pnBufSize) < (int)cCmdInfo.dwDataSize)
        {
            HV_Trace(5, "Destbuf size is not enough!");
            return E_FAIL;
        }
        memcpy(lpBuf, GetDataPtr(nPciHandle), cCmdInfo.dwDataSize);
        (*pnBufSize) = (int)cCmdInfo.dwDataSize;
    }

    return S_OK;
}

HRESULT CHvPciLinkApi::PutReturnValue(int nPciHandle)
{
    LockWait(nPciHandle);
    if (m_rgApdInfo[nPciHandle].dwSemStatus == 1)
    {
        PCI_CMDHEAD_INFO cCmdInfo;
        memcpy(&cCmdInfo, GetHeadPtr(nPciHandle), sizeof(cCmdInfo));
        if (cCmdInfo.dwCmdID == m_rgApdInfo[nPciHandle].dwCmdID)
        {
            PostWait(nPciHandle);
        }
        else
        {
            HV_Trace(5, "Command's id is not equal! %d_%d", cCmdInfo.dwCmdID, m_rgApdInfo[nPciHandle].dwCmdID);
        }
    }
    UnLockWait(nPciHandle);
    return S_OK;
}

void CHvPciLinkApi::SetShakeHandsStatus(bool fShakeHandsFinish)
{
    m_fShakeHandsFinish = fShakeHandsFinish;
}

inline void CHvPciLinkApi::LockSendData()
{
    SemPend(&m_hSemSendData);
}

inline void CHvPciLinkApi::UnLockSendData()
{
    SemPost(&m_hSemSendData);
}

unsigned char* CHvPciLinkApi::GetHeadPtr(int nPciHandle)
{
    if (nPciHandle <= 0 || nPciHandle >= MAX_PCI_BUF_COUNT)
    {
        HV_Trace(5, "CHvPciLinkApi::GetHeadPtr(0x%08x) is not valid", nPciHandle);
        return NULL;
    }
    return ((unsigned char*)m_rgRWBufs[nPciHandle].user);
}

unsigned char* CHvPciLinkApi::GetDataPtr(int nPciHandle)
{
    if (nPciHandle <= 0 || nPciHandle >= MAX_PCI_BUF_COUNT)
    {
        HV_Trace(5, "CHvPciLinkApi::GetDataPtr(0x%08x) is not valid", nPciHandle);
        return NULL;
    }
    return ((unsigned char*)m_rgRWBufs[nPciHandle].user + sizeof(PCI_CMDHEAD_INFO));
}

//-----------------------------------------------------

int PciSendClockSyncData(DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh)
{
    char szData[8];
    memcpy(szData, &dwTimeMsLow, 4);
    memcpy(szData+4, &dwTimeMsHigh, 4);

    HRESULT hr = E_FAIL;
    int nSize = sizeof(hr);
    if ( S_OK == g_cHvPciLinkApi.SendData(PCILINK_CLOCK_SYNC_DATA, szData, 8, &hr, &nSize)
            && hr == S_OK )
    {
        return 0;
    }

    return -1;
}

int PciSendParamSetData(PBYTE8 szXmlParam, int iLen)
{
    if ( iLen > PCILINK_PARAM_DATA_LEN )
    {
        HV_Trace(5, "<PciSendParamSetData> iLen is too long!\n");
        return -1;
    }

    HRESULT hr = E_FAIL;
    int nSize = sizeof(hr);
    if ( S_OK == g_cHvPciLinkApi.SendData(
                PCILINK_PARAM_SET_DATA,
                szXmlParam,
                iLen,
                &hr,
                &nSize,
                20000)  // Comment by Shaorg: 保存参数比较耗时。
            && hr == S_OK )
    {
        return 0;
    }

    HV_Trace(5, "<PciSendParamSetData> Error!\n");
    return -1;
}

int PciSendSlaveImage(int iEnable)
{
    if ( S_OK == g_cHvPciLinkApi.SendData(PCILINK_SEND_SLAVE_IMAGE, &iEnable, 4) )
    {
        return 0;
    }

    return -1;
}

int PciShakeHands()
{
    DWORD32 dwTimeMsLow = 0;
    DWORD32 dwTimeMsHigh = 0;
    GetSystemTime(&dwTimeMsLow, &dwTimeMsHigh);

    char szData[8];
    memcpy(szData, &dwTimeMsLow, 4);
    memcpy(szData + 4, &dwTimeMsHigh, 4);

    HRESULT hrReturn = E_FAIL;
    int nReturnSize = sizeof(HRESULT);

    if ( S_OK == g_cHvPciLinkApi.SendData(
                PCILINK_SHAKE_HANDS,
                szData, 8,
                &hrReturn, &nReturnSize) )
    {
        if ( S_OK == hrReturn )
        {
            return 0;
        }
    }

    return -1;
}

int PciSendComData(
    LPVOID pbBuf,
    DWORD32 dwLen,
    LPVOID lpReturnData/* = NULL */,
    int *pnDataSize/* = 0 */,
    int nTimeOut/* = 3000 */
)
{
    HRESULT hr = E_FAIL;
    hr = g_cHvPciLinkApi.SendData(
             PCILINK_PCICOM,
             pbBuf,
             dwLen,
             lpReturnData,
             pnDataSize,
             nTimeOut
         );
    return ( S_OK == hr ) ? 0 : -1;
}

int PciSetJpegCompressRate(int nRate)
{
    HRESULT hr = E_FAIL;
    HRESULT hrReturn = E_FAIL;
    int nReturnSize = sizeof(HRESULT);

    hr = g_cHvPciLinkApi.SendData(
             PCILINK_SET_JPEG_CR,
             &nRate, sizeof(int),
             &hrReturn,
             &nReturnSize
         );

    if (hr == S_OK && hrReturn == S_OK)
    {
        return 0;
    }

    return -1;
}

int PciSetJpegType(int nType)
{
    HRESULT hr = E_FAIL;
    HRESULT hrReturn = E_FAIL;
    int nReturnSize = sizeof(HRESULT);

    hr = g_cHvPciLinkApi.SendData(
             PCILINK_SET_JPEG_TYPE,
             &nType, sizeof(int),
             &hrReturn,
             &nReturnSize
         );

    if (hr == S_OK && hrReturn == S_OK)
    {
        return 0;
    }

    return -1;
}

int PciSetCaptureCompressRate(int nRate)
{
    HRESULT hr = E_FAIL;
    HRESULT hrReturn = E_FAIL;
    int nReturnSize = sizeof(HRESULT);

    hr = g_cHvPciLinkApi.SendData(
             PCILINK_SET_CAPTURE_CR,
             &nRate, sizeof(int),
             &hrReturn,
             &nReturnSize
         );

    if (hr == S_OK && hrReturn == S_OK)
    {
        return 0;
    }

    return -1;
}

int PciSetDeviceState(DWORD32 dwResetMode)
{
    HRESULT hr = E_FAIL;
    HRESULT hrReturn = E_FAIL;
    int nReturnSize = sizeof(HRESULT);

    hr = g_cHvPciLinkApi.SendData(
             PCILINK_SET_DEVICE_STATE,
             &dwResetMode,
             sizeof(DWORD32),
             &hrReturn,
             &nReturnSize
         );

    if (hr == S_OK && hrReturn == S_OK)
    {
        return 0;
    }
    else if (hr == S_OK && hrReturn != S_OK)
    {
        return -1;
    }
    else
    {
        return -2;
    }
}

int PciSendSlaveCpuState(const SLAVE_CPU_STATUS* pcSlaveCpuStatus)
{
    HRESULT hr = E_FAIL;

    hr = g_cHvPciLinkApi.SendData(
             PCILINK_SLAVE_CPU_STATE,
             (void*)pcSlaveCpuStatus,
             sizeof(SLAVE_CPU_STATUS)
         );
    if (hr != S_OK)
    {
        HV_Trace(5, "Warning:Pci First Send Throb Failed\n");
        hr = g_cHvPciLinkApi.SendData(
             PCILINK_SLAVE_CPU_STATE,
             (void*)pcSlaveCpuStatus,
             sizeof(SLAVE_CPU_STATUS)
         );
    }

    return (S_OK == hr) ? (0) : (-1);
}

int PciSendMasterCpuState(const MASTER_CPU_STATUS* pcMasterCpuStatus)
{
    HRESULT hr = E_FAIL;

    hr = g_cHvPciLinkApi.SendData(
             PCILINK_MASTER_CPU_STATE,
             (void*)pcMasterCpuStatus,
             sizeof(MASTER_CPU_STATUS)
         );

    if (hr != S_OK)
    {
        HV_Trace(5, "Warning:Pci First Send Throb Failed\n");
        hr = g_cHvPciLinkApi.SendData(
             PCILINK_MASTER_CPU_STATE,
             (void*)pcMasterCpuStatus,
             sizeof(MASTER_CPU_STATUS)
         );
    }

    return (S_OK == hr) ? (0) : (-1);
}

int PciGetSlaveDebugInfo(LPVOID lpReturnInfo, int *pnInfoLen)
{
    HRESULT hr = E_FAIL;
    // 必须发数据，虽然无意义，否则出错
    int iData = 0;
    hr = g_cHvPciLinkApi.SendData(
             PCILINK_GET_SLAVE_DEBUG_INFO,
             &iData,
             sizeof(iData),
             lpReturnInfo,
             pnInfoLen
         );

    if ( S_OK == hr )
    {
        if ( *pnInfoLen == sizeof(HRESULT) )
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return -2;
    }
}

int PciGetResetLog(LPVOID lpReturnLog, int *pnLogLen)
{
    HRESULT hr = E_FAIL;
    // 必须发数据，虽然无意义，否则出错
    int iData = 0;
    hr = g_cHvPciLinkApi.SendData(
             PCILINK_GET_SLAVE_RESET_LOG,
             &iData,
             sizeof(iData),
             lpReturnLog,
             pnLogLen
         );

    if ( S_OK == hr )
    {
        if ( *pnLogLen == sizeof(HRESULT) )
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return -2;
    }
}

int PciSendHvExit(int iHvExitCode)
{
    HRESULT hr = g_cHvPciLinkApi.SendData(PCILINK_HV_EXIT, &iHvExitCode, sizeof(iHvExitCode));
    return (S_OK == hr) ? (0) : (-1);
}

HRESULT StartPciLink(
    CPciDataRecvThreadSlave *pPciDataRecv,
    PCI_PARAM_SLAVE cPciParam
)
{
#ifndef _CAM_APP_
    struct mytv
    {
        int iTick;
        int fMaster;
    }tmp;
    // 从CPU端初始化PCILink
    int iRetryCount = 0;
    while ( S_OK != g_cHvPciLinkApi.Init() )
    {
        if ( iRetryCount++ > 60 )  // 30（60*500）秒后仍然握手不成功，则复位设备。
        {
            HV_Trace(5, "waiting pci timeout,ResetHv.\n");
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "waiting pci timeout,ResetHv.");
        }
        if(!(iRetryCount % 20))
        {
            tmp.iTick = GetSystemTick();
#ifdef SINGLE_BOARD_PLATFORM
        tmp.fMaster = 1;
#else
        tmp.fMaster = 0;
#endif
            WriteDataToFile("/.running", (unsigned char *)&tmp, sizeof(tmp));
        }
        HV_Sleep(500);
        HV_Trace(5, "waiting pci...\n");
    }


    pPciDataRecv->SetPciParam(cPciParam);
    if ( S_OK != pPciDataRecv->Start(NULL) )
    {
        return E_FAIL;
    }
    return (TRUE == pPciDataRecv->PciLinkShakeHands()) ? (S_OK) : (E_FAIL);
#else
    return E_NOTIMPL;
#endif
}

HRESULT DataCtrlHandshake(CPciDataRecvThreadSlave *pPciDataRecv)
{
#ifndef _CAM_APP_
    return (TRUE == pPciDataRecv->DataCtrlHandshake()) ? (S_OK) : (E_FAIL);
#else
    return E_NOTIMPL;
#endif
}

HRESULT SendDataCtrlHandshake()
{
    HRESULT hrResult, hrReturn;
    int nData = 0;
    int nReturnLen = sizeof(hrReturn);
    for (int i = 0; i < 200; ++i)
    {
        hrResult = g_cHvPciLinkApi.SendData(
                       PCILINK_DATACTRL_SHAKE_HANDS,
                       &nData,
                       sizeof(nData),
                       &hrReturn,
                       &nReturnLen
                   );
        if (hrResult == S_OK && hrReturn == S_OK)
        {
            return S_OK;
        }
        if (i % 10 == 0)
        {
            HV_Trace(5, "Waitting datactrl...");

            //通过写文件通知守护进程当前进程依旧活着
            struct mytv
            {
                int iTick;
                int fMaster;
            }tmp;
            tmp.iTick = GetSystemTick();
            tmp.fMaster = 1;
            WriteDataToFile("/.running", (unsigned char *)&tmp, sizeof(tmp));
        }
        HV_Sleep(50);
    }
    return E_FAIL;
}

HRESULT PciSendSlaveDebugInfo(const char * szMsg, ...)
{
//#ifdef SINGLE_BOARD_PLATFORM
#if 0
    char szTemp[255];

    va_list arglist;
    va_start(arglist, szMsg);
    vsprintf(szTemp, szMsg, arglist);
    va_end(arglist);

    int iData = strlen(szTemp) + 1;
    return g_cHvPciLinkApi.SendData(
             PCILINK_SLAVE_LOG,
             (void *)szTemp,
             iData
         );
#else
    return E_FAIL;
#endif
}
