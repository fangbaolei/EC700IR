#include "HvDspLinkApi.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "DspLinkCmd.h"
#include "hvtarget_ARM.h"
#include "misc.h"

#ifdef _CAM_APP_
#include "../CamApp/DataCtrl.h"
#endif

using namespace HiVideo;

CHvDspLinkApi g_cHvDspLinkApi;  // 全局唯一

CHvDspLinkApi::CHvDspLinkApi()
        : m_nJpegEncodeErrorCount(0)
        , m_nH264EncodeErrorCount(0)
        , m_nCamDspErrorCount(0)
        , m_nProcessOneFrameErrorCount(0)
{
    memset(m_szDspFileName, 0, sizeof(m_szDspFileName));
    CreateSemaphore(&m_semLock, 1, 1);
}

CHvDspLinkApi::~CHvDspLinkApi()
{
    DestroySemaphore(&m_semLock);
}

void CHvDspLinkApi::Init(char* pszDotOutName/* = NULL*/)
{
    if (NULL == pszDotOutName)
    {
        strcpy(m_szDspFileName, "HvDsp.out");
    }
    else
    {
        strcpy(m_szDspFileName, pszDotOutName);
    }

    DSPLinkSetup(m_szDspFileName);
    CreateDSPBuffer(&m_paramSendCmd, MAX_CMD_PARAM_LEN);
    CreateDSPBuffer(&m_paramRecvCmd, MAX_CMD_PARAM_LEN);

    if (!m_paramSendCmd.addr || !m_paramRecvCmd.addr)
    {
        HV_Trace(1, "Create DSPLink param ERROR!!!\n");
    }

#ifndef _CAM_APP_
    // 矫正DSP系统TICK
    RECTIFY_TIME_PARAM cParam;
    cParam.dwArmCurTime = GetSystemTick();
    SendRectifyTimeCmd(&cParam);
#endif
}

void CHvDspLinkApi::Reload()
{
    //DSPLinkSetup(m_szDspFileName);
    HV_Exit(HEC_FAIL|HEC_RESET_DEV, "CHvDspLinkApi::Reload");
    HV_Trace(5, "ReloadDspLink is finished.\n");

#ifdef _CAM_APP_
    g_pCamApp->ResetH264Enc(-1);
#endif
}

void CHvDspLinkApi::Uninit()
{
    FreeDSPBuffer(&m_paramSendCmd);
    FreeDSPBuffer(&m_paramRecvCmd);
    ExitDSPLink();
}

HRESULT CHvDspLinkApi::SendImageEddyTypeCmd(IMAGE_EDDYTYPE_PARAM *pEddyTypeParam)
{
    Lock();

    HRESULT hr = S_FALSE;
    if (!m_paramSendCmd.addr || !m_paramRecvCmd.addr)
    {
        UnLock();
        return E_INVALIDARG;
    }

    PBYTE8 pbCurAddr = (PBYTE8)m_paramSendCmd.addr;
    PDWORD32 pdwCmdId = (PDWORD32)pbCurAddr;
    *pdwCmdId = DCI_IMAGE_EDDY_TYPE;

    // 输入参数赋值
    pbCurAddr += 4;
    PDWORD32 pdwParamSize = (PDWORD32)pbCurAddr;
    *pdwParamSize = sizeof(RECTIFY_TIME_PARAM);

    pbCurAddr += 4;
    IMAGE_EDDYTYPE_PARAM* pParam = (IMAGE_EDDYTYPE_PARAM*)pbCurAddr;
    memcpy(pParam, pEddyTypeParam, sizeof(IMAGE_EDDYTYPE_PARAM));

    pbCurAddr += *pdwParamSize;
    PDWORD32 pdwDataSize = (PDWORD32)pbCurAddr;
    *pdwDataSize = 0;

    // 调用DSP处理
    if (do_process(&m_paramSendCmd, &m_paramRecvCmd, 4000) != 0)
    {
        hr = E_FAIL;
        HV_Trace(5, "<SendImageEddyTypeCmd> DspLink process failed!!!\n");
    }
    else
    {
        DSP_RESPOND_HEAD* pResHead = (DSP_RESPOND_HEAD*)m_paramRecvCmd.addr;
        if (pResHead->dwCmdId != DCI_IMAGE_EDDY_TYPE)
        {
            hr = E_NOTIMPL;
        }
        else
        {
            hr = pResHead->iResult;
            if (hr != S_OK)
            {
                // 输出DSP端信息
                HV_Trace(5, "<SendImageEddyTypeCmd> DSPLink message:\n%s", pResHead->szErrorInfo);
            }
        }
    }
    UnLock();
    return hr;
}

HRESULT CHvDspLinkApi::SendRectifyTimeCmd(RECTIFY_TIME_PARAM* pRectifyParam)
{
    Lock();

    HRESULT hr = S_FALSE;
    if (!m_paramSendCmd.addr || !m_paramRecvCmd.addr)
    {
        UnLock();
        return E_INVALIDARG;
    }

    PBYTE8 pbCurAddr = (PBYTE8)m_paramSendCmd.addr;
    PDWORD32 pdwCmdId = (PDWORD32)pbCurAddr;
    *pdwCmdId = DCI_RECTIFY_TIME;

    // 输入参数赋值
    pbCurAddr += 4;
    PDWORD32 pdwParamSize = (PDWORD32)pbCurAddr;
    *pdwParamSize = sizeof(RECTIFY_TIME_PARAM);

    pbCurAddr += 4;
    RECTIFY_TIME_PARAM* pParam = (RECTIFY_TIME_PARAM*)pbCurAddr;
    memcpy(pParam, pRectifyParam, sizeof(RECTIFY_TIME_PARAM));

    pbCurAddr += *pdwParamSize;
    PDWORD32 pdwDataSize = (PDWORD32)pbCurAddr;
    *pdwDataSize = 0;

    // 调用DSP处理
    if (do_process(&m_paramSendCmd, &m_paramRecvCmd, 4000) != 0)
    {
        hr = E_FAIL;
        HV_Trace(5, "<SendRectifyTimeCmd> DspLink process failed!!!\n");
    }
    else
    {
        DSP_RESPOND_HEAD* pResHead = (DSP_RESPOND_HEAD*)m_paramRecvCmd.addr;
        if (pResHead->dwCmdId != DCI_RECTIFY_TIME)
        {
            hr = E_NOTIMPL;
        }
        else
        {
            hr = pResHead->iResult;
            if (hr != S_OK)
            {
                // 输出DSP端信息
                HV_Trace(5, "<SendRectifyTimeCmd> DSPLink message:\n%s", pResHead->szErrorInfo);
            }
        }
    }
    UnLock();
    return hr;
}

HRESULT CHvDspLinkApi::SendInitPhotoRecogerCmd(INIT_PHOTO_RECOGER_PARAM* pInitParam)
{
    Lock();

    HRESULT hr = S_FALSE;
    if (!m_paramSendCmd.addr || !m_paramRecvCmd.addr)
    {
        UnLock();
        return E_INVALIDARG;
    }

    PBYTE8 pbCurAddr = (PBYTE8)m_paramSendCmd.addr;
    PDWORD32 pdwCmdId = (PDWORD32)pbCurAddr;
    *pdwCmdId = DCI_INIT_PHOTO_RECOGER;

    // 输入参数赋值
    pbCurAddr += 4;
    PDWORD32 pdwParamSize = (PDWORD32)pbCurAddr;
    *pdwParamSize = sizeof(INIT_PHOTO_RECOGER_PARAM);

    pbCurAddr += 4;
    INIT_PHOTO_RECOGER_PARAM* pParam = (INIT_PHOTO_RECOGER_PARAM*)pbCurAddr;
    pParam->nPlateRecogParamIndex = pInitParam->nPlateRecogParamIndex;
    memcpy(&(pParam->cTrackerCfgParam), &(pInitParam->cTrackerCfgParam), sizeof(TRACKER_CFG_PARAM));

    pbCurAddr += *pdwParamSize;
    PDWORD32 pdwDataSize = (PDWORD32)pbCurAddr;
    *pdwDataSize = 0;

    // 调用DSP处理
    if (do_process(&m_paramSendCmd, &m_paramRecvCmd, 4000) != 0)
    {
        hr = E_FAIL;
        HV_Trace(5, "<SendInitPhotoRecogerCmd> DspLink process failed!!!\n");
    }
    else
    {
        DSP_RESPOND_HEAD* pResHead = (DSP_RESPOND_HEAD*)m_paramRecvCmd.addr;
        if (pResHead->dwCmdId != DCI_INIT_PHOTO_RECOGER)
        {
            hr = E_NOTIMPL;
        }
        else
        {
            hr = pResHead->iResult;
            if (hr != S_OK)
            {
                // 输出DSP端信息
                HV_Trace(5, "<SendInitPhotoRecogerCmd> DSPLink message:\n%s", pResHead->szErrorInfo);
            }
        }
    }
    UnLock();
    return hr;
}

HRESULT CHvDspLinkApi::SendProcessPhotoCmd(
    PROCESS_PHOTO_PARAM* pProcessParam,
    PROCESS_PHOTO_DATA* pProcessData,
    PROCESS_PHOTO_RESPOND* pProcessRespond
)
{
    Lock();

    HRESULT hr = S_FALSE;
    if (!m_paramSendCmd.addr || !m_paramRecvCmd.addr)
    {
        UnLock();
        return E_INVALIDARG;
    }

    PBYTE8 pbCurAddr = (PBYTE8)m_paramSendCmd.addr;
    PDWORD32 pdwCmdId = (PDWORD32)pbCurAddr;
    *pdwCmdId = DCI_PROCESS_PHOTO;

    // 输入参数赋值
    pbCurAddr += 4;
    PDWORD32 pdwParamSize = (PDWORD32)pbCurAddr;
    *pdwParamSize = sizeof(PROCESS_PHOTO_PARAM);

    pbCurAddr += 4;
    PROCESS_PHOTO_PARAM* pParam = (PROCESS_PHOTO_PARAM*)pbCurAddr;
    memcpy(pParam, pProcessParam, sizeof(PROCESS_PHOTO_PARAM));

    pbCurAddr += *pdwParamSize;
    PDWORD32 pdwDataSize = (PDWORD32)pbCurAddr;
    *pdwDataSize = sizeof(PROCESS_PHOTO_DATA);

    pbCurAddr += 4;
    PROCESS_PHOTO_DATA* pData = (PROCESS_PHOTO_DATA*)pbCurAddr;
    memcpy(pData, pProcessData, sizeof(PROCESS_PHOTO_DATA));

    // 调用DSP处理
    if (do_process(&m_paramSendCmd, &m_paramRecvCmd, 4000) != 0)
    {
        hr = E_FAIL;
        HV_Trace(5, "<SendProcessPhotoCmd> DspLink process failed!!!\n");
    }
    else
    {
        DSP_RESPOND_HEAD* pResHead = (DSP_RESPOND_HEAD*)m_paramRecvCmd.addr;
        if (pResHead->dwCmdId != DCI_PROCESS_PHOTO)
        {
            hr = E_NOTIMPL;
        }
        else
        {
            hr = pResHead->iResult;
        }
        if (pResHead->iAppendDataSize != sizeof(PROCESS_PHOTO_RESPOND))
        {
            hr = E_FAIL;
        }
        else
        {
            hr = S_OK;
            // 结果结构体赋值
            memcpy(
                pProcessRespond,
                (PBYTE8)m_paramRecvCmd.addr + sizeof(DSP_RESPOND_HEAD),
                sizeof(PROCESS_PHOTO_RESPOND));
        }
        // 输出DSP端信息
        if ( S_OK != hr )
        {
            HV_Trace(5, "<SendProcessPhotoCmd> DSPLink message[hr = 0x%08x]:\n%s", hr, pResHead->szErrorInfo);
        }
        else
        {
            HV_Trace(3, "<SendProcessPhotoCmd> DSPLink message:\n%s", pResHead->szErrorInfo);
        }
    }
    UnLock();
    return hr;
}

HRESULT CHvDspLinkApi::SendJpegEncodeCmd(
    const JPEG_ENCODE_PARAM* pcParam,
    const JPEG_ENCODE_DATA* pcData,
    JPEG_ENCODE_RESPOND* pcRespond
)
{
    Lock();

    HRESULT hr = S_FALSE;
    if (!m_paramSendCmd.addr || !m_paramRecvCmd.addr)
    {
        UnLock();
        return E_INVALIDARG;
    }

    PBYTE8 pbCurAddr = (PBYTE8)m_paramSendCmd.addr;
    PDWORD32 pdwCmdId = (PDWORD32)pbCurAddr;
    *pdwCmdId = DCI_JPEG_ENCODE;

    // 输入参数赋值
    pbCurAddr += 4;
    PDWORD32 pdwParamSize = (PDWORD32)pbCurAddr;
    *pdwParamSize = sizeof(JPEG_ENCODE_PARAM);
    pbCurAddr += 4;
    JPEG_ENCODE_PARAM* pParam = (JPEG_ENCODE_PARAM*)pbCurAddr;
    memcpy(pParam, pcParam, sizeof(JPEG_ENCODE_PARAM));

    // 输入数据赋值
    pbCurAddr += *pdwParamSize;
    PDWORD32 pdwDataSize = (PDWORD32)pbCurAddr;
    *pdwDataSize = sizeof(JPEG_ENCODE_DATA);
    pbCurAddr += 4;
    JPEG_ENCODE_DATA* pData = (JPEG_ENCODE_DATA*)pbCurAddr;
    memcpy(pData, pcData, sizeof(JPEG_ENCODE_DATA));

    // 调用DSP处理
    if (do_process(&m_paramSendCmd, &m_paramRecvCmd, 4000) != 0)
    {
        hr = E_FAIL;
        HV_Trace(5, "<SendJpegEncodeCmd> DspLink process failed!!!\n");
        Reload();

        m_nJpegEncodeErrorCount++;
        if ( m_nJpegEncodeErrorCount > s_nMaxTryCount )
        {
            HV_Trace(5, "<SendJpegEncodeCmd> DspLink process failed,exit!!!\n");
            HV_Exit(HEC_FAIL, "<SendJpegEncodeCmd> DspLink process failed!");
        }
    }
    else
    {
        m_nJpegEncodeErrorCount = 0;

        DSP_RESPOND_HEAD* pResHead = (DSP_RESPOND_HEAD*)m_paramRecvCmd.addr;
        if (pResHead->dwCmdId != DCI_JPEG_ENCODE)
        {
            hr = E_NOTIMPL;
        }
        else
        {
            hr = pResHead->iResult;
        }

        if (pResHead->iAppendDataSize != sizeof(JPEG_ENCODE_RESPOND))
        {
            hr = E_FAIL;
        }
        else
        {
            JPEG_ENCODE_RESPOND* pJpegEncodeRespond = (JPEG_ENCODE_RESPOND*)((PBYTE8)m_paramRecvCmd.addr + sizeof(DSP_RESPOND_HEAD));
            memcpy(pcRespond, pJpegEncodeRespond, sizeof(JPEG_ENCODE_RESPOND));
        }

        // 输出DSP端信息
        pResHead->szErrorInfo[sizeof(pResHead->szErrorInfo)-1] = '\0';
        if ( S_OK != hr )
        {
            HV_Trace(5, "<SendJpegEncodeCmd> DSPLink message[hr = 0x%08x]:\n%s", hr, pResHead->szErrorInfo);
        }
        else
        {
            HV_Trace(3, "<SendJpegEncodeCmd> DSPLink message:\n%s", pResHead->szErrorInfo);
        }
    }
    UnLock();
    return hr;
}

HRESULT CHvDspLinkApi::SendH264EncodeCmd(
    const H264_ENCODE_PARAM* pcParam,
    const H264_ENCODE_DATA* pcData,
    H264_ENCODE_RESPOND* pcRespond
)
{
    Lock();

    HRESULT hr = S_FALSE;
    if (!m_paramSendCmd.addr || !m_paramRecvCmd.addr)
    {
        UnLock();
        return E_INVALIDARG;
    }

    PBYTE8 pbCurAddr = (PBYTE8)m_paramSendCmd.addr;
    PDWORD32 pdwCmdId = (PDWORD32)pbCurAddr;
    *pdwCmdId = DCI_H264_ENCODE;

    // 输入参数赋值
    pbCurAddr += 4;
    PDWORD32 pdwParamSize = (PDWORD32)pbCurAddr;
    *pdwParamSize = sizeof(H264_ENCODE_PARAM);
    pbCurAddr += 4;
    H264_ENCODE_PARAM* pParam = (H264_ENCODE_PARAM*)pbCurAddr;
    memcpy(pParam, pcParam, sizeof(H264_ENCODE_PARAM));

    // 输入数据赋值
    pbCurAddr += *pdwParamSize;
    PDWORD32 pdwDataSize = (PDWORD32)pbCurAddr;
    *pdwDataSize = sizeof(H264_ENCODE_DATA);
    pbCurAddr += 4;
    H264_ENCODE_DATA* pData = (H264_ENCODE_DATA*)pbCurAddr;
    memcpy(pData, pcData, sizeof(H264_ENCODE_DATA));

    // 调用DSP处理
    if (do_process(&m_paramSendCmd, &m_paramRecvCmd, 4000) != 0)
    {
        hr = E_FAIL;
        HV_Trace(5, "<SendH264EncodeCmd> DspLink process failed!!!\n");
        Reload();

        m_nH264EncodeErrorCount++;
        if ( m_nH264EncodeErrorCount > s_nMaxTryCount )
        {
            HV_Trace(5, "<SendH264EncodeCmd> DspLink process failed,exit!!!\n");
            HV_Exit(HEC_FAIL, "<SendH264EncodeCmd> DspLink process failed!");
        }
    }
    else
    {
        m_nH264EncodeErrorCount = 0;

        DSP_RESPOND_HEAD* pResHead = (DSP_RESPOND_HEAD*)m_paramRecvCmd.addr;
        if (pResHead->dwCmdId != DCI_H264_ENCODE)
        {
            hr = E_NOTIMPL;
        }
        else
        {
            hr = pResHead->iResult;
        }

        if (pResHead->iAppendDataSize != sizeof(H264_ENCODE_RESPOND))
        {
            hr = E_FAIL;
        }
        else
        {
            H264_ENCODE_RESPOND* pH264EncodeRespond = (H264_ENCODE_RESPOND*)((PBYTE8)m_paramRecvCmd.addr + sizeof(DSP_RESPOND_HEAD));
            memcpy(pcRespond, pH264EncodeRespond, sizeof(H264_ENCODE_RESPOND));
        }

        // 输出DSP端信息
        pResHead->szErrorInfo[sizeof(pResHead->szErrorInfo)-1] = '\0';
        if ( S_OK != hr )
        {
            HV_Trace(5, "<SendH264EncodeCmd> DSPLink message[hr = 0x%08x]:\n%s", hr, pResHead->szErrorInfo);
        }
        else
        {
            HV_Trace(3, "<SendH264EncodeCmd> DSPLink message:\n%s", pResHead->szErrorInfo);
        }
    }
    UnLock();
    return hr;
}

HRESULT CHvDspLinkApi::SendStringOverlayInitCmd(
    const STRING_OVERLAY_PARAM* pcParam,
    const STRING_OVERLAY_DATA* pcData,
    STRING_OVERLAY_RESPOND* pcRespond
)
{
    Lock();

    HRESULT hr = S_FALSE;
    if (!m_paramSendCmd.addr || !m_paramRecvCmd.addr)
    {
        UnLock();
        return E_INVALIDARG;
    }

    PBYTE8 pbCurAddr = (PBYTE8)m_paramSendCmd.addr;
    PDWORD32 pdwCmdId = (PDWORD32)pbCurAddr;
    *pdwCmdId = DCI_STRING_OVERLAYINIT;

    // 输入参数赋值
    pbCurAddr += 4;
    PDWORD32 pdwParamSize = (PDWORD32)pbCurAddr;
    *pdwParamSize = sizeof(STRING_OVERLAY_PARAM);
    pbCurAddr += 4;
    STRING_OVERLAY_PARAM* pParam = (STRING_OVERLAY_PARAM*)pbCurAddr;
    memcpy(pParam, pcParam, sizeof(STRING_OVERLAY_PARAM));

    // 输入数据赋值
    pbCurAddr += *pdwParamSize;
    PDWORD32 pdwDataSize = (PDWORD32)pbCurAddr;
    *pdwDataSize = sizeof(STRING_OVERLAY_DATA);
    pbCurAddr += 4;
    STRING_OVERLAY_DATA* pData = (STRING_OVERLAY_DATA*)pbCurAddr;
    memcpy(pData, pcData, sizeof(STRING_OVERLAY_DATA));

    // 调用DSP处理
    if (do_process(&m_paramSendCmd, &m_paramRecvCmd, 4000) != 0)
    {
        hr = E_FAIL;
        HV_Trace(5, "<SendStringOverlayInitCmd> DspLink process failed!!!\n");
        Reload();
    }
    else
    {
        DSP_RESPOND_HEAD* pResHead = (DSP_RESPOND_HEAD*)m_paramRecvCmd.addr;
        if (pResHead->dwCmdId != DCI_STRING_OVERLAYINIT)
        {
            hr = E_NOTIMPL;
        }
        else
        {
            hr = pResHead->iResult;
        }

        if (pResHead->iAppendDataSize != sizeof(STRING_OVERLAY_RESPOND))
        {
            hr = E_FAIL;
        }
        else
        {
            STRING_OVERLAY_RESPOND* pRespond = (STRING_OVERLAY_RESPOND*)((PBYTE8)m_paramRecvCmd.addr + sizeof(DSP_RESPOND_HEAD));
            memcpy(pcRespond, pRespond, sizeof(STRING_OVERLAY_RESPOND));
        }

        // 输出DSP端信息
        pResHead->szErrorInfo[sizeof(pResHead->szErrorInfo)-1] = '\0';
        if ( S_OK != hr )
        {
            HV_Trace(5, "<SendStringOverlayInitCmd> DSPLink message[hr = 0x%08x]:\n%s", hr, pResHead->szErrorInfo);
        }
        else
        {
            HV_Trace(3, "<SendStringOverlayInitCmd> DSPLink message:\n%s", pResHead->szErrorInfo);
        }
    }
    UnLock();
    return hr;
}

HRESULT CHvDspLinkApi::SendCamDspCmd(
    const CAM_DSP_PARAM* pcParam,
    const CAM_DSP_DATA* pcData,
    CAM_DSP_RESPOND* pcRespond
)
{
    Lock();

    HRESULT hr = S_FALSE;
    if (!m_paramSendCmd.addr || !m_paramRecvCmd.addr)
    {
        UnLock();
        return E_INVALIDARG;
    }

    PBYTE8 pbCurAddr = (PBYTE8)m_paramSendCmd.addr;
    PDWORD32 pdwCmdId = (PDWORD32)pbCurAddr;
    *pdwCmdId = DCI_CAM_DSP;

    // 输入参数赋值
    pbCurAddr += 4;
    PDWORD32 pdwParamSize = (PDWORD32)pbCurAddr;
    *pdwParamSize = sizeof(CAM_DSP_PARAM);
    pbCurAddr += 4;
    CAM_DSP_PARAM* pParam = (CAM_DSP_PARAM*)pbCurAddr;
    memcpy(pParam, pcParam, sizeof(CAM_DSP_PARAM));

    // 输入数据赋值
    pbCurAddr += *pdwParamSize;
    PDWORD32 pdwDataSize = (PDWORD32)pbCurAddr;
    *pdwDataSize = sizeof(CAM_DSP_DATA);
    pbCurAddr += 4;
    CAM_DSP_DATA* pData = (CAM_DSP_DATA*)pbCurAddr;
    memcpy(pData, pcData, sizeof(CAM_DSP_DATA));

    // 调用DSP处理
    if (do_process(&m_paramSendCmd, &m_paramRecvCmd, 4000) != 0)
    {
        hr = E_FAIL;
        HV_Trace(5, "<SendCamDspCmd type:0x%08x> DspLink process failed!!!\n", pcParam->dwProcType);
        Reload();

        m_nCamDspErrorCount++;
        if ( m_nCamDspErrorCount > s_nMaxTryCount )
        {
            HV_Trace(5, "<SendCamDspCmd> DspLink process failed,exit!!!\n");
            HV_Exit(HEC_FAIL, "<SendCamDspCmd> DspLink process failed!");
        }
    }
    else
    {
        m_nCamDspErrorCount = 0;

        DSP_RESPOND_HEAD* pResHead = (DSP_RESPOND_HEAD*)m_paramRecvCmd.addr;
        if (pResHead->dwCmdId != DCI_CAM_DSP)
        {
            hr = E_NOTIMPL;
        }
        else
        {
            hr = pResHead->iResult;
        }

        if (pResHead->iAppendDataSize != sizeof(CAM_DSP_RESPOND))
        {
            hr = E_FAIL;
        }
        else
        {
            CAM_DSP_RESPOND* pCamDspRespond = (CAM_DSP_RESPOND*)((PBYTE8)m_paramRecvCmd.addr + sizeof(DSP_RESPOND_HEAD));
            memcpy(pcRespond, pCamDspRespond, sizeof(CAM_DSP_RESPOND));
        }

        // 输出DSP端信息
        pResHead->szErrorInfo[sizeof(pResHead->szErrorInfo)-1] = '\0';
        if ( S_OK != hr )
        {
            HV_Trace(5, "<SendCamDspCmd> DSPLink message[hr = 0x%08x]:\n%s", hr, pResHead->szErrorInfo);
        }
        else
        {
            HV_Trace(3, "<SendCamDspCmd> DSPLink message:\n%s", pResHead->szErrorInfo);
        }
    }
    UnLock();
    return hr;
}

HRESULT CHvDspLinkApi::SendInitVideoRecogerCmd(INIT_VIDEO_RECOGER_PARAM* pInitParam)
{
    Lock();

    HRESULT hr = S_FALSE;
    if (!m_paramSendCmd.addr || !m_paramRecvCmd.addr)
    {
        UnLock();
        return E_INVALIDARG;
    }

    PBYTE8 pbCurAddr = (PBYTE8)m_paramSendCmd.addr;
    PDWORD32 pdwCmdId = (PDWORD32)pbCurAddr;
    *pdwCmdId = DCI_INIT_VIDEO_RECOGER;

    // 输入参数赋值
    pbCurAddr += 4;
    PDWORD32 pdwParamSize = (PDWORD32)pbCurAddr;
    *pdwParamSize = sizeof(INIT_VIDEO_RECOGER_PARAM);

    pbCurAddr += 4;
    INIT_VIDEO_RECOGER_PARAM* pParam = (INIT_VIDEO_RECOGER_PARAM*)pbCurAddr;
    memcpy(pParam, pInitParam, sizeof(INIT_VIDEO_RECOGER_PARAM));

    pbCurAddr += *pdwParamSize;
    PDWORD32 pdwDataSize = (PDWORD32)pbCurAddr;
    *pdwDataSize = 0;

    // 调用DSP处理
    if (do_process(&m_paramSendCmd, &m_paramRecvCmd, 4000) != 0)
    {
        hr = E_FAIL;
        HV_Trace(5, "<SendInitVideoRecogerCmd> DspLink process failed!!!\n");
    }
    else
    {
        DSP_RESPOND_HEAD* pResHead = (DSP_RESPOND_HEAD*)m_paramRecvCmd.addr;
        if (pResHead->dwCmdId != DCI_INIT_VIDEO_RECOGER)
        {
            hr = E_NOTIMPL;
        }
        else
        {
            hr = pResHead->iResult;
            // 输出DSP端信息
            HV_Trace(5, "<SendInitVideoRecogerCmd> DSPLink message:\nhr = 0x%08x [%s]\n", hr, pResHead->szErrorInfo);

            if ( S_OK != hr )
            {
                Trace("\n====================\n");
                HV_Dump((unsigned char*)pResHead, sizeof(DSP_RESPOND_HEAD));
                Trace("\n====================\n");
            }
        }
    }
    UnLock();
    return hr;
}

HRESULT CHvDspLinkApi::SendProcessOneFrameCmd(
    PROCESS_ONE_FRAME_PARAM* pProcessParam,
    PROCESS_ONE_FRAME_DATA* pProcessData,
    PROCESS_ONE_FRAME_RESPOND* pProcessRespond
)
{
    Lock();

    HRESULT hr = S_FALSE;
    if (!m_paramSendCmd.addr || !m_paramRecvCmd.addr)
    {
        UnLock();
        return E_INVALIDARG;
    }

    PBYTE8 pbCurAddr = (PBYTE8)m_paramSendCmd.addr;
    PDWORD32 pdwCmdId = (PDWORD32)pbCurAddr;
    *pdwCmdId = DCI_PROCESS_ONE_FRAME;

    // 输入参数赋值
    pbCurAddr += 4;
    PDWORD32 pdwParamSize = (PDWORD32)pbCurAddr;
    *pdwParamSize = sizeof(PROCESS_ONE_FRAME_PARAM);

    pbCurAddr += 4;
    PROCESS_ONE_FRAME_PARAM* pParam = (PROCESS_ONE_FRAME_PARAM*)pbCurAddr;
    pProcessParam->dwCurTick = GetSystemTick();
    memcpy(pParam, pProcessParam, sizeof(PROCESS_ONE_FRAME_PARAM));

    pbCurAddr += *pdwParamSize;
    PDWORD32 pdwDataSize = (PDWORD32)pbCurAddr;
    *pdwDataSize = sizeof(PROCESS_ONE_FRAME_DATA);

    pbCurAddr += 4;
    PROCESS_ONE_FRAME_DATA* pData = (PROCESS_ONE_FRAME_DATA*)pbCurAddr;
    memcpy(pData, pProcessData, sizeof(PROCESS_ONE_FRAME_DATA));

    // 调用DSP处理
    if (do_process(&m_paramSendCmd, &m_paramRecvCmd, 4000) != 0)
    {
        hr = E_FAIL;
        HV_Trace(5, "<SendProcessOneFrameCmd> DspLink process failed!!!\n");
        Reload();

        m_nProcessOneFrameErrorCount++;
        if ( m_nProcessOneFrameErrorCount > s_nMaxTryCount )
        {
            HV_Trace(5, "<SendProcessOneFrameCmd> DspLink process failed,exit!!!\n");
            HV_Exit(HEC_FAIL, "<SendProcessOneFrameCmd> DspLink process failed!");
        }
    }
    else
    {
        m_nProcessOneFrameErrorCount = 0;
        DSP_RESPOND_HEAD* pResHead = (DSP_RESPOND_HEAD*)m_paramRecvCmd.addr;
        if (pResHead->dwCmdId != DCI_PROCESS_ONE_FRAME)
        {
            hr = E_NOTIMPL;
        }
        else
        {
            hr = pResHead->iResult;
        }
        if (pResHead->iAppendDataSize != sizeof(PROCESS_ONE_FRAME_RESPOND))
        {
            hr = E_FAIL;
        }
        else
        {
            hr = pResHead->iResult;
            // 结果结构体赋值
            memcpy(
                pProcessRespond,
                (PBYTE8)m_paramRecvCmd.addr + sizeof(DSP_RESPOND_HEAD),
                sizeof(PROCESS_ONE_FRAME_RESPOND));
        }
        pResHead->szErrorInfo[sizeof(pResHead->szErrorInfo)-1] = '\0';
        if (hr != S_OK)
        {
            HV_Trace(5, "<SendProcessOneFrameCmd> DSPLink message[hr = 0x%08x]:\n%s", hr, pResHead->szErrorInfo);
        }
        else if (strlen(pResHead->szErrorInfo) > 0)
        {
            if(!strstr(pResHead->szErrorInfo, "Decode time"))
            {
                HV_Trace(5, "<SendProcessOneFrameCmd> DSPLink message:\n%s", pResHead->szErrorInfo);
            }
            else
            {
                HV_Trace(3, "<SendProcessOneFrameCmd> DSPLink message:\n%s", pResHead->szErrorInfo);
            }
        }
    }
    UnLock();
    return hr;
}

HRESULT CHvDspLinkApi::SetSoftTriggerCallback(SOFTTRIGGER_CALLBACK_FUNC pSoftTriggerCallbackFunc)
{
    if (NULL == pSoftTriggerCallbackFunc)
    {
        return E_POINTER;
    }

    m_nSoftTriggerFD = SwDevOpen(O_RDWR);
    if (m_nSoftTriggerFD < 0)
    {
        return E_FAIL;
    }

    int oflags;
    signal(SIGIO, pSoftTriggerCallbackFunc);
    fcntl(m_nSoftTriggerFD, F_SETOWN, getpid());
    oflags = fcntl(m_nSoftTriggerFD, F_GETFL);
    fcntl(m_nSoftTriggerFD, F_SETFL, oflags | FASYNC);

    return S_OK;
}

// --------------------------------------------------------------

#if defined(_CAM_APP_) || defined(SINGLE_BOARD_PLATFORM)
static void InitAT88Verifier(unsigned int* rgNC)
{
    if ( 0 != write_crypt(rgNC) )
    {
        HV_Trace(5, "write_crypt is failed!\n");
        HV_Exit(HEC_FAIL|HEC_RESET_DEV, "write_crypt is failed!");
    }
    else
    {
        HV_Trace(5, "write_crypt is succeed.\n");
    }
}
#endif

/* LoadDspFromFlash.c */
extern "C" void SetDspKey(unsigned int* rgNC, unsigned int* rgKey);

HRESULT InitDspLink(int argc, char** argv)
{
    const char* szDspFileName = (argc == 1) ? "" : (const char*)argv[1];
    if ( strlen(szDspFileName) == 0 )
    {
#ifdef _CAM_APP_
        g_cHvDspLinkApi.Init("CamDsp.out");
#else
        g_cHvDspLinkApi.Init(NULL);
#endif
    }
    else // 正式版启动，通过Flash读取.out文件
    {
        unsigned int rgdwNC[4] = {0};
        unsigned int rgdwKey[4] = {0};
        if ( 0 == get_nc_key(&rgdwNC, &rgdwKey) )
        {
            SetDspKey(rgdwNC, rgdwKey);

            g_cHvDspLinkApi.Init((char*)szDspFileName);

#if defined(_CAM_APP_) || defined(SINGLE_BOARD_PLATFORM)
            InitAT88Verifier(rgdwNC);  // 初始化AT88加密认证机制
#endif
        }
        else
        {
            HV_Exit(HEC_FAIL|HEC_RESET_DEV, "GetDspKey is failed!");
            return E_FAIL;
        }
    }

    HV_Trace(5, "InitDspLink is finished.\n");
    return S_OK;
}
