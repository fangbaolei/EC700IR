#include "ImgGatherer.h"

using namespace HiVideo;

CCamyuLink::CCamyuLink()
        : m_sktData(INVALID_SOCKET)
        , m_sktCmd(INVALID_SOCKET)
        , m_pstmData(NULL)
        , m_pstmCmd(NULL)
        , m_iRoadNum(0)
{
    m_pcTmpData = (char*)HV_AllocMem(MAX_RECV_BUF_SIZE);
}

CCamyuLink::~CCamyuLink()
{
    GetImageStop();
    CtrtCamStop();
    if (m_pcTmpData)
    {
        HV_FreeMem(m_pcTmpData, MAX_RECV_BUF_SIZE);
    }
}

HRESULT CCamyuLink::GetImageStart(const char* pszIP)
{
    GetImageStop();

    m_sktData = HvCreateSocket();

    if (m_sktData == INVALID_SOCKET)
    {
        return E_FAIL;
    }

    if (S_OK != HvConnect(m_sktData, pszIP, CY_DATA_PORT, 4000))
    {
        GetImageStop();
        return E_FAIL;
    }

    //设置超时
    if (S_OK != HvSetRecvTimeOut(m_sktData, 5000)
            || S_OK != HvSetSendTimeOut(m_sktData, 5000))
    {
        GetImageStop();
        return E_FAIL;
    }

    m_pstmData = new CSocketStream(m_sktData);

    if (m_pstmData == NULL)
    {
        GetImageStop();
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CCamyuLink::GetImageStartEx(const char* pszIP, int iPort)
{
    GetImageStop();

    m_sktData = HvCreateSocket();

    if (m_sktData == INVALID_SOCKET)
    {
        return E_FAIL;
    }

    if (S_OK != HvConnect(m_sktData, pszIP, iPort, 4000))
    {
        GetImageStop();
        return E_FAIL;
    }

    //设置超时
    if (S_OK != SetDataSktTimeout(5000))
    {
        GetImageStop();
        return E_FAIL;
    }

    m_pstmData = new CSocketStream(m_sktData);

    if (m_pstmData == NULL)
    {
        GetImageStop();
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CCamyuLink::GetImageStop()
{
    if (m_sktData != INVALID_SOCKET)
    {
        CloseSocket(m_sktData);
        m_sktData = INVALID_SOCKET;
    }
    if (m_pstmData)
    {
        delete m_pstmData;
        m_pstmData = NULL;
    }

    return S_OK;
}

HRESULT CCamyuLink::GetOneFrame(char* pcBuf, DWORD32* pdwSize, bool fMustData)
{
    if (!m_pstmData || !pdwSize || !pcBuf) return E_POINTER;

    HRESULT hr = E_FAIL;
    CY_FRAME_HEADER cCyFrameHeader;
    DWORD32 dwThrobAck = CY_THROB_ACK;

    // 接收包头
    hr = m_pstmData->Read(&cCyFrameHeader, sizeof(cCyFrameHeader), NULL);
    if (FAILED(hr)) return hr;

    // 如果是通信心跳帧，返回心跳应答
    while (cCyFrameHeader.wFrameType == 2)
    {
        hr = m_pstmData->Write(&dwThrobAck, 4, NULL);
        if (FAILED(hr)) return hr;
        // 如果不需要得到图片数据，则返回
        if (!fMustData) return S_OK;
        // 继续接收下一包头
        hr = m_pstmData->Read(&cCyFrameHeader, sizeof(cCyFrameHeader), NULL);
        if (FAILED(hr)) return hr;
    }
    // 是普通帧或者抓拍帧，接收图像帧数据
    if (*pdwSize < cCyFrameHeader.dwLen)
    {
        return E_OUTOFMEMORY;
    }

    *pdwSize = cCyFrameHeader.dwLen;
    hr = m_pstmData->Read(pcBuf, cCyFrameHeader.dwLen, NULL);
    m_iCurFrameType = cCyFrameHeader.wFrameType;
    m_iRoadNum = cCyFrameHeader.dwSoftParam;
    m_dwHeight = cCyFrameHeader.wHeight;
    m_dwWidth = cCyFrameHeader.wWidth;
    return hr;
}

HRESULT CCamyuLink::GetImage(char* pcBuf, DWORD32* pdwSize, DWORD32* pdwRefTime)
{
    if (!pcBuf || !pdwSize || !pdwRefTime) return E_POINTER;
    HRESULT hr = GetOneFrame(pcBuf, pdwSize, true);
    *pdwRefTime = GetSystemTick() - 80;
    return hr;
}

HRESULT CCamyuLink::ProcessData()
{
    DWORD32 dwDataLen = MAX_RECV_BUF_SIZE;
    return GetOneFrame(m_pcTmpData, &dwDataLen, false);
}

// 获取当前帧类型，指定大华摄像机码流I帧或P帧
int CCamyuLink::GetCurFrameType()
{
    return m_iCurFrameType;
}
//获取当前帧对应车道,抓拍回来的帧才带有车道号,
int CCamyuLink::GetCurRoadNum()
{
    return m_iRoadNum;
}

HRESULT CCamyuLink::GetImageWidth(DWORD32& dwWidth)
{
    dwWidth = m_dwWidth;
    return 0;
}

HRESULT CCamyuLink::GetImageHeight(DWORD32& dwHeight)
{
    dwHeight = m_dwHeight;
    return 0;
}

HRESULT CCamyuLink::CtrtCamStart(const char* pszIP)
{
    CtrtCamStop();

    m_sktCmd = HvCreateSocket();

    if (m_sktCmd == INVALID_SOCKET)
    {
        return E_FAIL;
    }

    if (S_OK != HvConnect(m_sktCmd, pszIP, CY_CMD_PORT, 4000))
    {
        CtrtCamStop();
        return E_FAIL;
    }

    //设置超时
    if (S_OK != SetCmdSktTimeout(5000))
    {
        CtrtCamStop();
        return E_FAIL;
    }

    m_pstmCmd = new CSocketStream(m_sktCmd);

    if (m_pstmCmd == NULL)
    {
        CtrtCamStop();
        return E_FAIL;
    }

    DWORD32 rgdwCamParam[CY_CAMERA_INFO_LEN];

    if (S_OK != SendCommand(CY_SYNC_PARAM))
    {
        CtrtCamStop();
        return E_FAIL;
    }

    if (S_OK != m_pstmCmd->Read(rgdwCamParam, sizeof(rgdwCamParam), NULL))
    {
        CtrtCamStop();
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CCamyuLink::CtrtCamStop()
{
    if (m_sktCmd != INVALID_SOCKET)
    {
        CloseSocket(m_sktCmd);
        m_sktCmd = INVALID_SOCKET;
    }
    if (m_pstmCmd)
    {
        delete m_pstmCmd;
        m_pstmCmd = NULL;
    }

    return S_OK;
}

HRESULT CCamyuLink::SendCommand(DWORD32 dwCmdNo, DWORD32 dwParam1, DWORD32 dwParam2)
{
    if (!m_pstmCmd) return E_FAIL;

    BYTE8 rgbCmd[12] = {0};
    DWORD32* pdwCmdNo = (DWORD32*)&rgbCmd[0];
    DWORD32* pdwParam1 = (DWORD32*)&rgbCmd[4];
    DWORD32* pdwParam2 = (DWORD32*)&rgbCmd[8];

    *pdwCmdNo = dwCmdNo;
    *pdwParam1 = dwParam1;
    *pdwParam2 = dwParam2;

    return m_pstmCmd->Write(rgbCmd, 12, NULL);
}

HRESULT CCamyuLink::SetDataSktTimeout(DWORD32 dwTimeout)
{
    if (S_OK != HvSetRecvTimeOut(m_sktData, dwTimeout)
            || S_OK != HvSetSendTimeOut(m_sktData, dwTimeout))
    {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT CCamyuLink::SetCmdSktTimeout(DWORD32 dwTimeout)
{
    if (S_OK != HvSetRecvTimeOut(m_sktCmd, dwTimeout)
            || S_OK != HvSetSendTimeOut(m_sktCmd, dwTimeout))
    {
        return E_FAIL;
    }
    return S_OK;
}
