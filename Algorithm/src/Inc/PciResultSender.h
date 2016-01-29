//该文件编码格式必须为WINDOWS-936格式

#ifndef _SENDPCIDATA_H
#define _SENDPCIDATA_H

#include "HvPciLinkApi.h"
#include "ResultSenderImpl.h"

const int MAX_RESULT_LIST_COUNT = 5; /**< 识别结果队列最大个数 */
const int MAX_VIDEO_LIST_COUNT = 10; /**< 视频流队列最大个数 */
const int MAX_QUEUE_LIST_COUNT = MAX_RESULT_LIST_COUNT + MAX_VIDEO_LIST_COUNT; /**< 发送队列最大个数 */

typedef struct tag_RecogResult_Info
{
    IReferenceComponentImage *pimgPlate;
    IReferenceComponentImage *pimgBestSnapShot;
    IReferenceComponentImage *pimgLastSnapShot;
    IReferenceComponentImage *pimgBeginCapture;
    IReferenceComponentImage *pimgBestCapture;
    IReferenceComponentImage *pimgLastCapture;
    IReferenceComponentImage *pimgPlateBin;
    DWORD32 dwTimeMsLow;
    DWORD32 dwTimeMsHigh;
    DWORD32 dwCarID;
    char szPlateInfo[MAX_PLATE_STRING_SIZE];
		char szPlate[32];
    HV_RECT rcBestPlatePos;
    HV_RECT rcLastPlatePos;
    HV_RECT rcFirstPos;
    HV_RECT rcSecondPos;
    HV_RECT rcThirdPos;
    HV_RECT rcRedLightPos[20];
    int     rcRedLightCount;
    HV_RECT rcFacePos[20];
    int     nFaceCount;

    tag_RecogResult_Info()
    {
        memset(this, 0, sizeof(*this));
    }
    ~tag_RecogResult_Info()
    {
        SAFE_RELEASE(pimgPlate);
        SAFE_RELEASE(pimgBestSnapShot);
        SAFE_RELEASE(pimgLastSnapShot);
        SAFE_RELEASE(pimgBeginCapture);
        SAFE_RELEASE(pimgBestCapture);
        SAFE_RELEASE(pimgLastCapture);
        SAFE_RELEASE(pimgPlateBin);
    }
}
RECOG_RESULT_INFO;

typedef struct tag_VideoResult_Info
{
    TRACK_RECT_INFO cTrackRectInfo;
    IReferenceComponentImage *pimgVideo;
    tag_VideoResult_Info()
    {
        memset(this, 0, sizeof(*this));
    }
    ~tag_VideoResult_Info()
    {
        SAFE_RELEASE(pimgVideo);
    }
}
VIDEO_RESULT_INFO;

class CPciResultSender : public IResultSender, public CHvThreadBase
{
public:
    CPciResultSender();
    ~CPciResultSender();

    //CHvThreadBase
    virtual const char* GetName()
    {
        static char szName[] = "CPciResultSender";
        return szName;
    }

    virtual HRESULT Run(void* pvParam);

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

    virtual HRESULT SendDevStateString(
        BOOL fIsCamConnected,
        float fltFrameRate,
        const char* pszHddStatus
    )
    {
        // 一体机从端目前只需发送处理帧率给主端即可
        HRESULT hrResult = g_cHvPciLinkApi.SendData(
                               PCILINK_DEVSTATUS_STRING,
                               (void*)&fltFrameRate,
                               sizeof(fltFrameRate)
                           );
        return hrResult;
    }

    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes)
    {
        DWORD32 dwCurTick = GetSystemTick();
        if (m_dwLastTick == 0 || dwCurTick < m_dwLastTick)
        {
            return S_OK;
        }
        if (dwCurTick - m_dwLastTick > 20000)
        {
            HV_Trace(5, "current time = %d, last time = %d, escape = %d\n", dwCurTick, m_dwLastTick, dwCurTick - m_dwLastTick);
            return E_FAIL;
        }
        return S_OK;
    }

    HRESULT Init(ResultSenderParam *pResultSenderParam, int camType = 0)
    {
        if (pResultSenderParam == NULL)
        {
            return E_POINTER;
        }
        m_nCamType = camType;
        m_pResultSenderParam = pResultSenderParam;
        BOOL fInited = m_resultFilter.InitRule(&(m_pResultSenderParam->cProcRule));
        Start(NULL);
        return fInited ? S_OK : E_FAIL;
    }

private:
    HRESULT CopyBigImageToShareBuffer(
        IReferenceComponentImage *pImage,
        int nImageType,
        DWORD32 dwCarID,
        HV_RECT rcPlate,
        HV_RECT *rcRedLight,
        int nRedLightCount,
        HV_RECT *rcFacePos,
        int      nFaceCount,
        int nPciHandle
    );

    HRESULT CopySmallImageToShareBuffer(
        IReferenceComponentImage *pImage,
        DWORD32 dwTimeMsLow,
        DWORD32 dwTimeMsHigh,
        DWORD32 dwCarID,
        int nPciHandle
    );

    HRESULT CopyBinImageToShareBuffer(
        IReferenceComponentImage *pImage,
        DWORD32 dwTimeMsLow,
        DWORD32 dwTimeMsHigh,
        DWORD32 dwCarID,
        int nPciHandle
    );

    HRESULT SendResult(RECOG_RESULT_INFO* pResultInfo);

    HRESULT SendVideo(VIDEO_RESULT_INFO* pVideoInfo);

    void LockResult()
    {
        SemPend(&m_hSemResult);
    }

    void UnLockResult()
    {
        SemPost(&m_hSemResult);
    }

    void LockVideo()
    {
        SemPend(&m_hSemVideo);
    }

    void UnLockVideo()
    {
        SemPost(&m_hSemVideo);
    }

protected:
    CHvList<RECOG_RESULT_INFO*> m_rgResultInfo;
    CHvList<VIDEO_RESULT_INFO*> m_rgVideoInfo;

private:
    ResultSenderParam *m_pResultSenderParam;
    CHvResultFilter m_resultFilter;
    DWORD32 m_dwCarID;
    DWORD32 m_dwLastTick;
    HV_SEM_HANDLE m_hSemResult;
    HV_SEM_HANDLE m_hSemVideo;
    HV_SEM_HANDLE m_hSemQueue;

    int  m_nCamType;
};

#endif
