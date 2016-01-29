#ifndef DATAMUX_H_INCLUDED
#define DATAMUX_H_INCLUDED

#include "hvutils.h"
#include "VideoGetter_VPIF.h"
#include "HvCameraLink.h"
#include "hvthread.h"
#include <queue>
#include <list>

#define MAX_RAW_H264_COUNT 12
#define MAX_RAW_JPEG_COUNT 12
#define MAX_ENCODE_COUNT 1
#define MAX_JPEG_COUNT 2
#define MAX_FRAME_STATE 10

typedef struct _FRAME_STATE
{
    DWORD32 dwTimeTick;
    FRAME_STATE_FLAG nState;
}
FRAME_STATE;

typedef struct FRAME_STATE_CMD
{
    int nCount;
    FRAME_STATE rgFrameState[MAX_FRAME_STATE];
}
FRAME_STATE_CMD;

class CH264EncodeThread : public CHvThreadBase
{
public:
    CH264EncodeThread();
    ~CH264EncodeThread();

public:
    virtual const char* GetName()
    {
        static char szName[] = "CH264EncodeThread";
        return szName;
    }
    virtual HRESULT Run(void* pvParam);

public:
    HRESULT Initialize(
        HiVideo::ISendCameraVideo* pVideoSender,
        DWORD32 dwTargetBitRate,
        BOOL fIsSideInstall,
        int iENetSyn,
        DWORD32 dwIDRFrameInterval
    );

public:
    HRESULT PutFrame(
        CReferenceFrame_VPIF* pFrame,
        BOOL fEncode
    );

private:
    HRESULT FetchFrame(CReferenceFrame_VPIF** ppFrame, int nTimeOut);

    void PrintSize()
    {
        printf("\n=====H264====\nFRAME[%d]\n==========\n",m_queFrame.size());
    }

private:
    HV_SEM_HANDLE m_hSemLock;
    HV_SEM_HANDLE m_hSemCount;

    std::queue<CReferenceFrame_VPIF*> m_queFrame;
    HiVideo::ISendCameraVideo* m_pVideoSender;

    DWORD32 m_dwTargetBitRate;
    BOOL m_fIsSideInstall;
    int m_iENetSyn;
    DWORD32 m_dwIDRFrameInterval;

    BOOL m_fEnableEncode;
    BOOL m_fH264Opened;
};

class CJpegEncodeThread : public CHvThreadBase
{
public:
    CJpegEncodeThread();
    ~CJpegEncodeThread();

public:
    virtual const char* GetName()
    {
        static char szName[] = "CJpegEncodeThread";
        return szName;
    }
    virtual HRESULT Run(void* pvParam);

public:
    HRESULT Initialize(
        int iJpegCompressRateCapture,
        int iJpegCompressRate,
        int iJpegCompressRateL,
        int iJpegCompressRateH,
        int iJpegExpectSize,
        int iEddy
    );

public:
    HRESULT PutFrame(
        CReferenceFrame_VPIF* pFrame,
        int iJpegCompressRateCapture,
        int iJpegCompressRate,
        int iJpegExpectSize
    );
    HRESULT ProcFrameState(FRAME_STATE_CMD* pFrameStateCmd);
    HRESULT FetchJpeg(DWORD32 dwTimeTick, IReferenceComponentImage** ppRefImage);
    HRESULT ClearDat();

private:
    HRESULT PutEncodeFrame(CReferenceFrame_VPIF* pFrame);
    HRESULT FetchEncodeFrame(CReferenceFrame_VPIF** ppFrame, int nTimeOut);

    void PrintSize()
    {
        printf("\n=====JPEG====\n");

        list<CReferenceFrame_VPIF*>::iterator it;
        printf("FRAME_TICK: ");
        for(it = m_listFrame.begin(); it != m_listFrame.end(); ++it)
        {
            printf("%d[%d],",(*it)->GetTimeTick(),(*it)->GetFlag());
        }
        printf("\n");

        printf("ENCODE_TICK: ");
        for(it = m_listEncode.begin(); it != m_listEncode.end(); ++it)
        {
            printf("%d,",(*it)->GetTimeTick());
        }
        printf("\n");

        list<IReferenceComponentImage*>::iterator itJPEG;
        printf("JPEG_TICK: ");
        for(itJPEG = m_listJpeg.begin(); itJPEG != m_listJpeg.end(); ++itJPEG)
        {
            printf("%d,",(*itJPEG)->GetRefTime());
        }
        printf("\n");

        printf("============\n");
    }

private:
    HV_SEM_HANDLE m_hSemFrameLock;
    HV_SEM_HANDLE m_hSemEncodeLock;
    HV_SEM_HANDLE m_hSemJpegLock;
    HV_SEM_HANDLE m_hSemEncodeCount;

    std::list<CReferenceFrame_VPIF*> m_listFrame;
    std::list<CReferenceFrame_VPIF*> m_listEncode;
    std::list<IReferenceComponentImage*> m_listJpeg;

    int m_iJpegCompressRateCapture;
    int m_iJpegCompressRate;
    int m_iJpegCompressRateL;
    int m_iJpegCompressRateH;
    int m_iJpegExpectSize;
    int m_iEddy;
    int m_dwFrameCount;
};

class CDataMux
{
public:
    CDataMux();
    ~CDataMux();

public:
    HRESULT Initalize(
        HiVideo::ISendCameraVideo* pVideoSender,
        DWORD32 dwTargetBitRate,
        BOOL fIsSideInstall,
        int iENetSyn,
        int iJpegCompressRateCapture,
        int iJpegCompressRate,
        int iJpegCompressRateL,
        int iJpegCompressRateH,
        int iJpegExpectSize,
        DWORD32 dwIDRFrameInterval
    );

    HRESULT PutFrame(
        CReferenceFrame_VPIF* pRefFrame_VPIF,
        BOOL fH264Enc,
        BOOL fJpegEnc,
        int iJpegCompressRateCapture,
        int iJpegCompressRate,
        int iJpegExpectSize
    );

    HRESULT ProcFrameState(FRAME_STATE_CMD* pFrameStateCmd);

    //取得结果已加引用,由调用方负责释放
    HRESULT FetchJpeg(DWORD32 dwTimeTick, IReferenceComponentImage** ppRefImage);

private:
    CH264EncodeThread m_cH264EncodeThread;
    CJpegEncodeThread m_cJpegEncodeThread;
};

#endif // DATAMUX_H_INCLUDED
