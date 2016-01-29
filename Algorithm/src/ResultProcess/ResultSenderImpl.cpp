#include "ResultSenderImpl.h"
#include "sendnetdata.h"
#include "tinyxml.h"

extern const int MAX_PLATE_STRING_SIZE;

ResultSenderParam CResultSender::m_cParam;

CResultSender::CResultSender()
{
    fInited = false;
    m_pcDataLinkCtrlThread = NULL;
    m_pcSafeSaver = NULL;
    m_pSafeSaver = NULL;
}

CResultSender::~CResultSender()
{
    SAFE_DELETE(m_pcSafeSaver);
}

HRESULT CResultSender::Init(
    const ResultSenderParam& cParam,
    CDataLinkCtrlThread* pcDataLinkCtrlThread
)
{
    if ( NULL == pcDataLinkCtrlThread )
    {
        return E_FAIL;
    }

    m_cParam = cParam;
    m_pcDataLinkCtrlThread = pcDataLinkCtrlThread;

    if ( true == m_cParam.fIsSafeSaver )
    {
        m_pcSafeSaver = new CSafeSaverDm6467Impl;
        if ( NULL == m_pcSafeSaver )
        {
            return E_FAIL;
        }
        else
        {
            //初始化并启动用于保存结果的ISafeSaver
            //m_pcSafeSaver->Init(CResultSender::m_cParam.szSafeSaverInitStr);
            m_pcSafeSaver->Init(NULL);
            m_pSafeSaver = m_pcSafeSaver;
        }
    }
    else
    {
        m_pSafeSaver = NULL;
    }

    //后处理规则采用成员变量 huanggr 2010-11-22
    fInited = m_resultFilter.InitRule(&(m_cParam.cProcRule));

    return (fInited) ? S_OK : E_FAIL;
}

HRESULT CResultSender::PutResult(
    LPCSTR szResultInfo,
    LPVOID lpcData
)
{
    if ( !szResultInfo || !lpcData ) return E_FAIL;
    if ( !fInited ) return E_FAIL;

    CARLEFT_INFO_STRUCT *lpData = (CARLEFT_INFO_STRUCT*)lpcData;

    DWORD32 dwTimeMsLow=0;
    DWORD32 dwTimeMsHigh=0;
    GetSystemTime(&dwTimeMsLow, &dwTimeMsHigh);

    //进行结果后处理
    if ( m_cParam.cProcRule.fLeach || m_cParam.cProcRule.fReplace )
    {
        // TODO: 加入后处理
        return E_NOTIMPL;
    }
    else
    {
        //发送结果（没有经过后处理）
        return SafeSendResult(
            m_pcDataLinkCtrlThread,
            m_pSafeSaver,
            szResultInfo,
            (RESULT_IMAGE_STRUCT*)&lpData->cCoreResult.cResultImg,
            dwTimeMsLow,
            dwTimeMsHigh
        );
    }
}

HRESULT CResultSender::PutVideo(
    DWORD32* pdwSendCount,
    LPVOID lpFrameData,
    int nRectCount/* = 0*/,
    HV_RECT *pRect/* = NULL*/
)
{
    HRESULT hr;
    HV_COMPONENT_IMAGE imgFrame;

    IMG_FRAME *pFrame = (IMG_FRAME *)lpFrameData;
    pFrame->pRefImage->GetImage(&imgFrame);
    PBYTE8 pImgData = GetHvImageData(&imgFrame, 0);

    hr = SendVideo(
             m_pcDataLinkCtrlThread,
             pFrame->iVideoID,
             pImgData,
             imgFrame.iWidth,
             pdwSendCount,
             pFrame->pRefImage->GetRefTime(),
             nRectCount,
             pRect
         );
    return hr;
}

HRESULT CResultSender::PutString(
    WORD16 wVideoID,
    WORD16 wStreamID,
    DWORD32 dwTimeLow,
    DWORD32 dwTimeHigh,
    const char *pString
)
{
    return SendString(
        m_pcDataLinkCtrlThread, wVideoID, wStreamID,
        dwTimeLow, dwTimeHigh, pString
        );
}

HRESULT CResultSender::PutCarEnterLeaveFlag(
        DWORD32 dwVideoID,
        WORD16 wInLeftFlag,
        DWORD32 dwTimeMsLow,
        DWORD32 dwTimeMsHigh
)
{
    return SendCarEnterLeaveFlag(
        m_pcDataLinkCtrlThread,
        dwVideoID,
        wInLeftFlag,
        dwTimeMsLow,
        dwTimeMsHigh
        );
}

HRESULT CResultSender::GetCurStatus(char* pszStatus, int nStatusSizes)
{
    if ( NULL == m_pSafeSaver )
    {
        return S_OK;
    }
    else
    {
        //return m_pSafeSaver->ThreadIsOk() ? S_OK : E_FAIL;
        // zhaopy
        return S_OK;
    }
}
