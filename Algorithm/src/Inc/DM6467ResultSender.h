#ifndef __DM6467_RESULT_SENDER_H__
#define __DM6467_RESULT_SENDER_H__

#include "PciResultSender.h"
#include "CameraRecordLink.h"
#include "CameraImageLink.h"
#include "DataCtrlBase.h"

class CDM6467ResultSender : public IResultSender
{
public:
    CDM6467ResultSender();
    virtual ~CDM6467ResultSender();

    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes)
    {
        return S_OK;
    }

    HRESULT Init(
        ResultSenderParam *pResultSenderParam,
        int camType = 0
    )
    {
        if (pResultSenderParam == NULL)
        {
            return E_POINTER;
        }
        m_pResultSenderParam = pResultSenderParam;
        BOOL fInited = m_resultFilter.InitRule(&(m_pResultSenderParam->cProcRule));
        return fInited ? S_OK : E_FAIL;
    }

    virtual HRESULT PutResult(
        LPCSTR szResultInfo,
        LPVOID lpcData
    );

    virtual HRESULT PutVideo(
        DWORD32* pdwSendCount,
        LPVOID lpFrameData,
        int nRectCount = 0,
        HV_RECT *pRect = NULL
    );

    virtual HRESULT PutString(
        WORD16 wVideoID,
        WORD16 wStreamID,
        DWORD32 dwTimeLow,
        DWORD32 dwTimeHigh,
        const char *pString
    );

    virtual HRESULT PutStatusString(const char * pString);

    virtual HRESULT SendDevStateString(
        BOOL fIsCamConnected,
        float fltFrameRate,
        const char* pszHddStatus
    )
    {
        char szString[1024] = {0};
        sprintf(
            szString,
            "摄像机状态:%s\n客户端状态:%s\n硬盘工作状态:%s\n处理帧率:%0.2f",
            fIsCamConnected ? "连接" : "断开",
            m_pRecordLinkCtrl->GetLinkCount() > 0 ? "连接" : "断开",
            pszHddStatus,
            fIsCamConnected ? fltFrameRate : 0.0f
            );
        return PutString(0, _TYPE_PERF_STR, 0, 0, szString);
    }

    bool IsSameHour(REAL_TIME_STRUCT* prtLeft, REAL_TIME_STRUCT* prtRight);

    void SetRecordSenderCtrl(CCameraRecordLinkCtrl * pRecordLinkCtrl)
    {
        m_pRecordLinkCtrl = pRecordLinkCtrl;
    }
    void SetImageSenderCtrl(CCameraImageLinkCtrl * pImageLinkCtrl)
    {
        m_pImageLinkCtrl = pImageLinkCtrl;
    }
    void SetSafeSaver(CSafeSaverDm6467Impl * pSafeSaver)
    {
        m_pSafeSaver = pSafeSaver;
    }
    void EnableRecordSend(void);
    void DisableRecordSend(void);

protected:
    int CalcShareMemorySize(CARLEFT_INFO_STRUCT * carLeft, LPCSTR szResultInfo);
    PBYTE8 CopyBigImageToBuffer(PBYTE8 buf, IReferenceComponentImage *pImage, int nImageType, DWORD32 dwCarID, HV_RECT rcPlate, HV_RECT *rcRedLight, int nRedLightCount);
    PBYTE8 CopySmallImageToBuffer(PBYTE8 buf, IReferenceComponentImage *pImage, DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh, DWORD32 dwCarID);
    PBYTE8 CopyBinImageToBuffer(PBYTE8 buf, IReferenceComponentImage *pImage, DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh, DWORD32 dwCarID);

private:
    ResultSenderParam *m_pResultSenderParam;
    CSafeSaverDm6467Impl *m_pSafeSaver;
    CHvResultFilter m_resultFilter;
    int m_iCarID;
    BOOL m_fCarIdSet;

    CCameraRecordLinkCtrl * m_pRecordLinkCtrl;
    CCameraImageLinkCtrl * m_pImageLinkCtrl;
    bool m_fSendRecord;
    REAL_TIME_STRUCT m_timeLastRecord;
};

#endif

