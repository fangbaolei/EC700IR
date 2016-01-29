#include "DataMux.h"
#include "HvDspLinkApi.h"
#include "DataCtrl.h"
#include "HvDebugStateInfo.h"

extern HV_SEM_HANDLE g_hSemEDMA;               // DSP端进行H.264与ARM端使用DMA互斥
extern "C" int EEPROM_Lock();
extern "C" int EEPROM_UnLock();

extern H264_FRAME_HEADER g_cH264FrameHeader;  // H.264流的帧头

extern int GetH264Header(H264_FRAME_HEADER& cH264FrameHeader, const PBYTE8 pbH264BitStream);

// 打开H.264编码器
extern HRESULT OpenH264Enc(
    CHvDspLinkApi& g_cHvDspLinkApi,
    int dwTargetBitRate,
    BOOL fIsSideInstall,
    int iENetSyn
);

// 关闭H.264编码器
extern HRESULT CloseH264Enc(CHvDspLinkApi& g_cHvDspLinkApi);

using namespace std;

//H264压缩线程实现
CH264EncodeThread::CH264EncodeThread()
{
    CreateSemaphore(&m_hSemLock, 1, 1);
    CreateSemaphore(&m_hSemCount, 0, MAX_RAW_H264_COUNT);

    m_pVideoSender = NULL;

    m_dwTargetBitRate = 6*1024*1024;
    m_fIsSideInstall = FALSE;
    m_iENetSyn = 1;
    m_dwIDRFrameInterval = 1;

    m_fEnableEncode = FALSE;
    m_fH264Opened = FALSE;
}

CH264EncodeThread::~CH264EncodeThread()
{
    DestroySemaphore(&m_hSemLock);
    DestroySemaphore(&m_hSemCount);
}

HRESULT CH264EncodeThread::Run(void* pvParam)
{
    CReferenceFrame_VPIF* pFrame = NULL;
    IReferenceComponentImage* pRefImageH264Frame = NULL;

    HV_COMPONENT_IMAGE imgCbYCrY;
    HV_COMPONENT_IMAGE imgH264Frame;

    CAM_DSP_PARAM cCamDspParam;
    CAM_DSP_DATA cCamDspData;
    CAM_DSP_RESPOND cCamDspRespond;

    DWORD32 dwH264FrameLen = 0;
    DWORD32 dwH264FrameType = 0;

    unsigned long ulH264FrameCount = 0;

    BOOL fSkipSend = FALSE;

    HRESULT hr;
    while(!m_fExit)
    {
        //不编码时关掉H264
        if(m_fEnableEncode == FALSE && m_fH264Opened)
        {
            CloseH264Enc(g_cHvDspLinkApi);
            m_fH264Opened = FALSE;
        }

        //要编码时打开H264
        if(m_fEnableEncode && m_fH264Opened == FALSE)
        {
            hr = OpenH264Enc(
                     g_cHvDspLinkApi,
                     m_dwTargetBitRate,
                     m_fIsSideInstall,
                     m_iENetSyn
                 );

            if(FAILED(hr))
            {
                HV_Trace(5, "OpenH264Enc Failed\n");
            }
            else
            {
                m_fH264Opened = TRUE;
            }
        }

        hr = FetchFrame(&pFrame, 1000); //1秒超时
        if(hr != S_OK) continue;

        if(m_pVideoSender == NULL || m_fH264Opened == FALSE)
        {
            pFrame->Release();
            continue;
        }

        imgCbYCrY = *(pFrame->GetImage());
        if ( S_OK != CreateReferenceComponentImage(
                    &pRefImageH264Frame,
                    HV_IMAGE_H264,
                    imgCbYCrY.iWidth,
                    imgCbYCrY.iHeight,
                    0,
                    pFrame->GetTimeTick(),
                    0,
                    "NULL",
                    2 ))
        {
            HV_Trace(5,"<CH264EncodeThread>CreateReferenceComponentImage Failed!\n");
            pFrame->Release();
            continue;
        }

        // H.264编码
        pRefImageH264Frame->GetImage(&imgH264Frame);

        cCamDspParam.dwProcType = PROC_TYPE_H264ENC;
        cCamDspParam.cH264EncodeParam.dwOpType = OPTYPE_ENCODE;
        cCamDspData.cH264EncodeData.hvImageYuv = imgCbYCrY;
        cCamDspData.cH264EncodeData.hvImageFrame = imgH264Frame;

        EEPROM_Lock();
        SemPend(&g_hSemEDMA);
        hr = g_cHvDspLinkApi.SendCamDspCmd(&cCamDspParam, &cCamDspData, &cCamDspRespond);
        SemPost(&g_hSemEDMA);
        EEPROM_UnLock();

        pFrame->Release(); //原始数据已无用

        dwH264FrameLen = cCamDspRespond.cH264EncodeRespond.dwFrameLen;
        dwH264FrameType = cCamDspRespond.cH264EncodeRespond.dwFrameType;

        fSkipSend = FALSE;

        if(hr != S_OK || dwH264FrameLen > 1024*1024)
        {
            HV_Trace(5, "H264Enc: hr=%08x,size=%d, Skip\n", hr, dwH264FrameLen);
            fSkipSend = TRUE;
        }
        else if( FRAME_TYPE_H264_SKIP == dwH264FrameType )
        {
            fSkipSend = TRUE;
        }

        if(fSkipSend)
        {
            pRefImageH264Frame->Release();
            continue;
        }

        //==================------zhaopy 发放--------=========
        SEND_CAMERA_VIDEO videoInfo;
        videoInfo.dwVideoType = CAMERA_VIDEO_H264;
        videoInfo.dwFrameType =
            dwH264FrameType == FRAME_TYPE_H264_I ? CAMERA_FRAME_I : CAMERA_FRAME_P;
        GetSystemTime(&videoInfo.dwTimeLow, &videoInfo.dwTimeHigh);

        if ( g_pCamApp->AGCIsEnable() )
        {
            g_pCamApp->GetNowAGCShutterGain(
                videoInfo.cVideoExtInfo.iShutter,
                videoInfo.cVideoExtInfo.iGain
            );
        }
        else
        {
            videoInfo.cVideoExtInfo.iShutter = g_cModuleParams.cCamAppParam.iShutter;
            videoInfo.cVideoExtInfo.iGain = g_cModuleParams.cCamAppParam.iGain;
        }

        if ( g_pCamApp->AWBIsEnable() )
        {
            g_pCamApp->GetNowAWBGain(
                videoInfo.cVideoExtInfo.iGainR,
                videoInfo.cVideoExtInfo.iGainG,
                videoInfo.cVideoExtInfo.iGainB
            );
        }
        else
        {
            videoInfo.cVideoExtInfo.iGainR = g_cModuleParams.cCamAppParam.iGainR;
            videoInfo.cVideoExtInfo.iGainG = g_cModuleParams.cCamAppParam.iGainG;
            videoInfo.cVideoExtInfo.iGainB = g_cModuleParams.cCamAppParam.iGainB;
        }

        videoInfo.dwVideoSize = dwH264FrameLen;
        videoInfo.pbVideo = GetHvImageData(&imgH264Frame, 0);
        videoInfo.pRefImage = pRefImageH264Frame;

        // 发送视频
        if ( S_OK != m_pVideoSender->SendCameraVideo(&videoInfo) )
        {
            HV_Trace(5, "<link>SendCameraVideo failed!\n");
        }

        pRefImageH264Frame->Release(); //Send模块中已经加引用

        if(ulH264FrameCount%100 == 0)
        {
            PrintSize();
        }

        // Comment by Shaorg: 通过关开编码器的方式获取IDR帧
        ++ulH264FrameCount;
        if ( 0 == (ulH264FrameCount % m_dwIDRFrameInterval) && m_fH264Opened )
        {
            if ( S_OK != CloseH264Enc(g_cHvDspLinkApi) )
            {
                HV_Trace(5, "CloseH264Enc is Error! [FrameCount: %d]\n", ulH264FrameCount);
            }
            else
            {
                m_fH264Opened = FALSE;

                if ( S_OK != OpenH264Enc(
                          g_cHvDspLinkApi,
                          m_dwTargetBitRate,
                          m_fIsSideInstall,
                          m_iENetSyn) )
                {
                    HV_Trace(5, "OpenH264Enc is Error! [FrameCount: %d]\n", ulH264FrameCount);
                }
                else
                {
                    m_fH264Opened = TRUE;
                }
            }
        }
    }

    return S_OK;
}

HRESULT CH264EncodeThread::Initialize(
    HiVideo::ISendCameraVideo* pVideoSender,
    DWORD32 dwTargetBitRate,
    BOOL fIsSideInstall,
    int iENetSyn,
    DWORD32 dwIDRFrameInterval
)
{
    m_pVideoSender = pVideoSender;
    m_dwTargetBitRate = dwTargetBitRate;
    m_fIsSideInstall = fIsSideInstall;
    m_iENetSyn = iENetSyn;
    m_dwIDRFrameInterval = dwIDRFrameInterval;

    return S_OK;
}

HRESULT CH264EncodeThread::PutFrame(
    CReferenceFrame_VPIF* pFrame,
    BOOL fEncode
)
{
    m_fEnableEncode = fEncode;
    if(pFrame == NULL || pFrame->IsCapture()) return S_OK;

    BOOL fPutOK = FALSE;

    SemPend(&m_hSemLock);
    if((int)m_queFrame.size() < MAX_RAW_H264_COUNT)
    {
        pFrame->AddRef();
        m_queFrame.push(pFrame);
        fPutOK = TRUE;
    }
    SemPost(&m_hSemLock);

    if(fPutOK)
    {
        SemPost(&m_hSemCount);
    }

    return S_OK;
}
HRESULT CH264EncodeThread::FetchFrame(CReferenceFrame_VPIF** ppFrame, int nTimeOut)
{
    if(ppFrame == NULL) return E_INVALIDARG;
    *ppFrame = NULL;

    int nRet = SemPend(&m_hSemCount, nTimeOut);
    if(nRet == 1) return S_FALSE;
    if(nRet == -1) return E_FAIL;

    SemPend(&m_hSemLock);
    if(!m_queFrame.empty())
    {
        *ppFrame = m_queFrame.front();
        m_queFrame.pop();
    }
    SemPost(&m_hSemLock);

    return (*ppFrame)?S_OK:S_FALSE;
}

//JPEG压缩线程实现
HRESULT EncodeJpeg(
    HV_COMPONENT_IMAGE imgCbYCrY,
    int nCompressRate,
    int iEddy,
    HV_COMPONENT_IMAGE imgJPEG,
    int& nEncodeSize
)
{
    CAM_DSP_PARAM cCamDspParam;
    CAM_DSP_DATA cCamDspData;
    CAM_DSP_RESPOND cCamDspRespond;

    cCamDspParam.dwProcType = PROC_TYPE_JPEGENC;
    cCamDspParam.cJpegEncodeParam.dwCompressRate = nCompressRate;
    cCamDspParam.cJpegEncodeParam.iEddy = iEddy;

    cCamDspData.cJpegEncodeData.hvImageYuv = imgCbYCrY;
    cCamDspData.cJpegEncodeData.hvImageJpg = imgJPEG;

    SemPend(&g_hSemEDMA);
    HRESULT hr = g_cHvDspLinkApi.SendCamDspCmd(&cCamDspParam,&cCamDspData,&cCamDspRespond);
    SemPost(&g_hSemEDMA);

    nEncodeSize = cCamDspRespond.cJpegEncodeRespond.dwJpegLen;

    return hr;
}

CJpegEncodeThread::CJpegEncodeThread()
{
    CreateSemaphore(&m_hSemFrameLock, 1, 1);
    CreateSemaphore(&m_hSemEncodeLock, 1, 1);
    CreateSemaphore(&m_hSemJpegLock, 1, 1);
    CreateSemaphore(&m_hSemEncodeCount, 0, MAX_RAW_JPEG_COUNT);

    m_iJpegCompressRateCapture = 86;
    m_iJpegCompressRate = 86;
    m_iJpegCompressRateL = 30;
    m_iJpegCompressRateH = 95;
    m_iJpegExpectSize = 0;
    m_iEddy = 0;
    m_dwFrameCount = 0;
}

CJpegEncodeThread::~CJpegEncodeThread()
{
    DestroySemaphore(&m_hSemFrameLock);
    DestroySemaphore(&m_hSemEncodeLock);
    DestroySemaphore(&m_hSemJpegLock);
    DestroySemaphore(&m_hSemEncodeCount);
}

HRESULT CJpegEncodeThread::Run(void* pvParam)
{
    CReferenceFrame_VPIF* pFrame = NULL;
    IReferenceComponentImage* pRefImageJpeg = NULL;

    HV_COMPONENT_IMAGE imgCbYCrY;
    HV_COMPONENT_IMAGE imgJPEG;

    BOOL fIsCapture = FALSE;
    HRESULT hr = E_FAIL;

    while(!m_fExit)
    {
        hr = FetchEncodeFrame(&pFrame, 1000); //1秒超时
        if(hr != S_OK) continue;

        imgCbYCrY = *pFrame->GetImage();

        if ( S_OK != CreateReferenceComponentImage(
                    &pRefImageJpeg,
                    HV_IMAGE_JPEG,
                    imgCbYCrY.iWidth,
                    imgCbYCrY.iHeight*2,
                    m_dwFrameCount++,
                    pFrame->GetTimeTick(),
                    0,
                    "NULL",
                    2 ))
        {
            HV_Trace(5,"<CJpegEncodeThread>CreateReferenceComponentImage Failed!\n");
            pFrame->Release();
            continue;
        }

        pRefImageJpeg->GetImage(&imgJPEG);

        //设置Jpeg的真实宽高
        imgJPEG.iHeight = imgCbYCrY.iWidth | ((imgCbYCrY.iHeight*2) << 16);
        pRefImageJpeg->SetImageSize(imgJPEG);

        fIsCapture = pFrame->IsCapture();
        DWORD32 nCompressRate = (fIsCapture)?(m_iJpegCompressRateCapture):(m_iJpegCompressRate);
        int nEncodeSize = 0;

        hr = EncodeJpeg(imgCbYCrY, nCompressRate, m_iEddy, imgJPEG, nEncodeSize);

        pFrame->Release(); //原始数据已无用

        if(FAILED(hr) || nEncodeSize == 0)
        {
            HV_Trace(5,"Encode Jpeg Failed!, hr = %08x\n, size = %d", hr, nEncodeSize);
            pRefImageJpeg->Release();
            continue;
        }

        // 根据Jpeg图片期望大小，自动调节Jpeg压缩率。
        if ( !fIsCapture && m_iJpegExpectSize != 0 )
        {
            if ( (nEncodeSize+10240) < m_iJpegExpectSize )
            {
                if ( m_iJpegCompressRate < m_iJpegCompressRateH )
                {
                    m_iJpegCompressRate++;
                }
            }
            else if ( nEncodeSize > m_iJpegExpectSize )
            {
                if ( m_iJpegCompressRate > m_iJpegCompressRateL )
                {
                    m_iJpegCompressRate--;
                }
            }
        }

        //设置Jpeg图片数据的实际长度
        imgJPEG.iWidth = nEncodeSize;
        pRefImageJpeg->SetImageSize(imgJPEG);

        //放入JPEG队列
        while((int)m_listJpeg.size() >= MAX_JPEG_COUNT)
        {
            m_listJpeg.front()->Release();
            m_listJpeg.pop_front();
        }
        m_listJpeg.push_back(pRefImageJpeg);

        m_dwFrameCount++;
        if(m_dwFrameCount%100 == 0)
        {
            PrintSize();
        }
    }

    return S_OK;
}

HRESULT CJpegEncodeThread::Initialize(
    int iJpegCompressRateCapture,
    int iJpegCompressRate,
    int iJpegCompressRateL,
    int iJpegCompressRateH,
    int iJpegExpectSize,
    int iEddy
)
{
    m_iJpegCompressRateCapture = iJpegCompressRateCapture;
    m_iJpegCompressRate = iJpegCompressRate;
    m_iJpegCompressRateL = iJpegCompressRateL;
    m_iJpegCompressRateH = iJpegCompressRateH;
    m_iJpegExpectSize = iJpegExpectSize;
    m_iEddy = iEddy;

    return S_OK;
}

HRESULT CJpegEncodeThread::PutFrame(
    CReferenceFrame_VPIF* pFrame,
    int iJpegCompressRateCapture,
    int iJpegCompressRate,
    int iJpegExpectSize
)
{
    m_iJpegCompressRateCapture = iJpegCompressRateCapture;
    m_iJpegExpectSize = iJpegExpectSize;

    if(iJpegExpectSize == 0)
    {
        m_iJpegCompressRate = iJpegCompressRate;
    }

    if(pFrame == NULL) return S_OK;

    BOOL fPutOK = FALSE;
    SemPend(&m_hSemFrameLock);

    //尝试移除旧数据
    if((int)m_listFrame.size() >= MAX_RAW_JPEG_COUNT)
    {
        CReferenceFrame_VPIF* pOldFrame = m_listFrame.front();
        if(pOldFrame->GetFlag() == FS_HOLD)
        {
            if(S_OK == PutEncodeFrame(pOldFrame))
            {
                m_listFrame.pop_front();
            }
        }

        if((int)m_listFrame.size() >= MAX_RAW_JPEG_COUNT)
        {
            list<CReferenceFrame_VPIF*>::iterator it;
            for(it = m_listFrame.begin(); it != m_listFrame.end(); ++it)
            {
                pOldFrame = *it;
                if(pOldFrame->GetFlag() != FS_HOLD)
                {
                    pOldFrame->Release();
                    m_listFrame.erase(it);
                    break;
                }
            }
        }
    }

    if((int)m_listFrame.size() < MAX_RAW_JPEG_COUNT)
    {
        pFrame->AddRef();
        m_listFrame.push_back(pFrame);
        fPutOK = TRUE;
    }
    else
    {
        HV_Trace(5,"JPEG Frame Full! Drop Frame\n");
    }
    SemPost(&m_hSemFrameLock);

    return fPutOK?S_OK:S_FALSE;
}

HRESULT CJpegEncodeThread::ProcFrameState(FRAME_STATE_CMD* pFrameStateCmd)
{
    if(pFrameStateCmd == NULL) return E_INVALIDARG;
    if(pFrameStateCmd->nCount <= 0) return S_FALSE;

    SemPend(&m_hSemFrameLock);

    list<CReferenceFrame_VPIF*>::iterator it;
    //更新状态
    for(it = m_listFrame.begin(); it != m_listFrame.end(); ++it)
    {
        for(int i = 0; i < pFrameStateCmd->nCount; ++i)
        {
            if ((*it)->GetTimeTick() == pFrameStateCmd->rgFrameState[i].dwTimeTick)
            {
                (*it)->SetFlag(pFrameStateCmd->rgFrameState[i].nState);
            }
        }
    }

    //FS_ENCODE
    CReferenceFrame_VPIF* pFrame = NULL;
    for( it = m_listFrame.begin(); it != m_listFrame.end(); )
    {
        pFrame = *it;
        if(pFrame->GetFlag() != FS_ENCODE)
        {
            ++it;
            continue;
        }

        if(S_OK == PutEncodeFrame(pFrame))
        {
            it = m_listFrame.erase(it);
        }
        else
        {
            ++it;
        }
    }

    //旧数据清理
    DWORD32 dwCurTick = pFrameStateCmd->rgFrameState[0].dwTimeTick;
    for( it = m_listFrame.begin(); it != m_listFrame.end(); )
    {
        pFrame = *it;
        if( pFrame->GetFlag() == FS_REMOVE ||
            (pFrame->GetTimeTick() < dwCurTick && pFrame->GetFlag() == FS_NORMAL) )
        {
            pFrame->Release();
            it = m_listFrame.erase(it);
        }
        else
        {
            it++;
        }
    }

    SemPost(&m_hSemFrameLock);

    PrintSize();

    return S_OK;
}

HRESULT CJpegEncodeThread::PutEncodeFrame(CReferenceFrame_VPIF* pFrame)
{
    BOOL fPutOK = FALSE;

    SemPend(&m_hSemEncodeLock);
    if((int)m_listEncode.size()<MAX_ENCODE_COUNT)
    {
        m_listEncode.push_back(pFrame);
        fPutOK = TRUE;
    }
    SemPost(&m_hSemEncodeLock);

    if(fPutOK)
    {
        SemPost(&m_hSemEncodeCount);
    }

    return fPutOK?S_OK:S_FALSE;
}

HRESULT CJpegEncodeThread::FetchEncodeFrame(CReferenceFrame_VPIF** ppFrame, int nTimeOut)
{
    if(ppFrame == NULL) return E_INVALIDARG;
    *ppFrame = NULL;

    int nRet = SemPend(&m_hSemEncodeCount, nTimeOut);
    if(nRet == 1) return S_FALSE;
    if(nRet == -1) return E_FAIL;

    SemPend(&m_hSemEncodeLock);
    if(!m_listEncode.empty())
    {
        *ppFrame = m_listEncode.front();
        m_listEncode.pop_front();
    }
    SemPost(&m_hSemEncodeLock);

    return (*ppFrame)?S_OK:S_FALSE;
}

HRESULT CJpegEncodeThread::FetchJpeg(DWORD32 dwTimeTick, IReferenceComponentImage** ppRefImage)
{
    if(ppRefImage == NULL) return E_INVALIDARG;
    *ppRefImage = NULL;

    SemPend(&m_hSemFrameLock);
    SemPend(&m_hSemEncodeLock);
    SemPend(&m_hSemJpegLock);

    //JPEG队列
    list<IReferenceComponentImage*>::iterator it;
    for(it=m_listJpeg.begin(); it!=m_listJpeg.end(); it++)
    {
        if( (*it)->GetRefTime() == dwTimeTick )
        {
            *ppRefImage = *it;
            (*ppRefImage)->AddRef();
            break;
        }
    }

    //未找到,从原始帧和压缩队列中找
    if(*ppRefImage == NULL)
    {
        CReferenceFrame_VPIF* pFrame = NULL;
        list<CReferenceFrame_VPIF*>::iterator it;
        for(it = m_listFrame.begin(); it != m_listFrame.end(); ++it)
        {
            if((*it)->GetTimeTick() == dwTimeTick)
            {
                pFrame = *it;
                m_listFrame.erase(it);
                break;
            }
        }

        if(pFrame == NULL)
        {
            for(it = m_listEncode.begin(); it != m_listEncode.end(); ++it)
            {
                if((*it)->GetTimeTick() == dwTimeTick)
                {
                    pFrame = *it;
                    m_listEncode.erase(it);
                    break;
                }
            }
        }

        //压缩
        if(pFrame)
        {
            HV_COMPONENT_IMAGE imgCbYCrY = *(pFrame->GetImage());
            IReferenceComponentImage* pRefImageJpeg = NULL;

            if ( S_OK != CreateReferenceComponentImage(
                        &pRefImageJpeg,
                        HV_IMAGE_JPEG,
                        imgCbYCrY.iWidth,
                        imgCbYCrY.iHeight*2,
                        m_dwFrameCount++,
                        pFrame->GetTimeTick(),
                        0,
                        "NULL",
                        2 ) )
            {
                HV_Trace(5,"<CJpegEncodeThread>CreateReferenceComponentImage Failed!\n");
                pFrame->Release();
            }
            else
            {
                HV_COMPONENT_IMAGE imgJPEG;
                pRefImageJpeg->GetImage(&imgJPEG);

                //设置Jpeg的真实宽高
                imgJPEG.iHeight = imgCbYCrY.iWidth | ((imgCbYCrY.iHeight*2) << 16);
                pRefImageJpeg->SetImageSize(imgJPEG);

                BOOL fIsCapture = pFrame->IsCapture();
                DWORD32 nCompressRate = (fIsCapture)?(m_iJpegCompressRateCapture):(m_iJpegCompressRate);
                int nEncodeSize = 0;

                HRESULT hr = EncodeJpeg(imgCbYCrY, nCompressRate, m_iEddy, imgJPEG, nEncodeSize);

                pFrame->Release(); //原始数据已无用

                if(FAILED(hr) || nEncodeSize == 0)
                {
                    HV_Trace(5,"Encode Jpeg Failed!, hr = %08x\n", hr);
                    pRefImageJpeg->Release();
                }
                else
                {
                    //设置Jpeg图片数据的实际长度
                    imgJPEG.iWidth = nEncodeSize;
                    pRefImageJpeg->SetImageSize(imgJPEG);

                    //放入JPEG队列
                    while((int)m_listJpeg.size() >= MAX_JPEG_COUNT)
                    {
                        m_listJpeg.front()->Release();
                        m_listJpeg.pop_front();
                    }
                    m_listJpeg.push_back(pRefImageJpeg);

                    //返回结果
                    *ppRefImage = pRefImageJpeg;
                    (*ppRefImage)->AddRef();
                }
            }
        }
    }

    SemPost(&m_hSemJpegLock);
    SemPost(&m_hSemEncodeLock);
    SemPost(&m_hSemFrameLock);

    PrintSize();

    return (*ppRefImage)?S_OK:S_FALSE;
}

HRESULT CJpegEncodeThread::ClearDat()
{
    SemPend(&m_hSemFrameLock);
    SemPend(&m_hSemEncodeLock);
    SemPend(&m_hSemJpegLock);

    list<CReferenceFrame_VPIF*>::iterator it;
    for(it = m_listFrame.begin(); it != m_listFrame.end();)
    {
        (*it)->Release();
        it = m_listFrame.erase(it);
    }

    for(it = m_listEncode.begin(); it != m_listEncode.end();)
    {
        (*it)->Release();
        it = m_listEncode.erase(it);
    }

    list<IReferenceComponentImage*>::iterator itJpeg;
    for(itJpeg = m_listJpeg.begin(); itJpeg != m_listJpeg.end();)
    {
        (*itJpeg)->Release();
        itJpeg = m_listJpeg.erase(itJpeg);
    }

    SemPost(&m_hSemJpegLock);
    SemPost(&m_hSemEncodeLock);
    SemPost(&m_hSemFrameLock);

    return S_OK;
}

//数据调制器实现
CDataMux::CDataMux()
{
}

CDataMux::~CDataMux()
{
    m_cH264EncodeThread.Stop(-1);
    m_cJpegEncodeThread.Stop(-1);
}

HRESULT CDataMux::Initalize(
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
)
{
    m_cH264EncodeThread.Initialize(pVideoSender, dwTargetBitRate, fIsSideInstall, iENetSyn, dwIDRFrameInterval);

    m_cJpegEncodeThread.Initialize(
        iJpegCompressRateCapture,
        iJpegCompressRate,
        iJpegCompressRateL,
        iJpegCompressRateH,
        iJpegExpectSize,
        fIsSideInstall?1:0
    );

    m_cH264EncodeThread.Start(NULL);
    m_cJpegEncodeThread.Start(NULL);

    return S_OK;
}

HRESULT CDataMux::PutFrame(
    CReferenceFrame_VPIF* pRefFrame_VPIF,
    BOOL fH264Enc,
    BOOL fJpegEnc,
    int iJpegCompressRateCapture,
    int iJpegCompressRate,
    int iJpegExpectSize
)
{
    //H264压缩线程
    m_cH264EncodeThread.PutFrame(
        fH264Enc?pRefFrame_VPIF:NULL,
        fH264Enc
    );

    if(fJpegEnc == FALSE)
    {
        m_cJpegEncodeThread.ClearDat();
    }

    //Jpeg结果图压缩线程
    m_cJpegEncodeThread.PutFrame(
        fJpegEnc?pRefFrame_VPIF:NULL,
        iJpegCompressRateCapture,
        iJpegCompressRate,
        iJpegExpectSize
    );

    return S_OK;
}

HRESULT CDataMux::ProcFrameState(FRAME_STATE_CMD* pFrameStateCmd)
{
    return m_cJpegEncodeThread.ProcFrameState(pFrameStateCmd);
}

HRESULT CDataMux::FetchJpeg(DWORD32 dwTimeTick, IReferenceComponentImage** ppRefImage)
{
    return m_cJpegEncodeThread.FetchJpeg(dwTimeTick, ppRefImage);
}
