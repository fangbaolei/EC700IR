/**
* @file	HvDspLinkApi.h
* @version	1.0
* @brief	DSPLink Arm端API
*/
#ifndef _HV_DSPLINK_H_
#define _HV_DSPLINK_H_

#include "hvutils.h"
#include "DSPLink.h"
#include "swimageobj.h"
#include "tracker.h"
#include "hvthread.h"

const int MAX_CMD_PARAM_LEN = 1024 * 1024;

// DSP软触发回调函数
typedef void (*SOFTTRIGGER_CALLBACK_FUNC)(int unused);

/**
* DSPLink函数提供类，注意：一个系统中只能定义一个该对象
*/
class CHvDspLinkApi
{
public:
    CHvDspLinkApi();
    ~CHvDspLinkApi();

public:
    /**
    * @brief 初始化DSPLink
    * @param pszDotOutName [in] 指定DSP编译出的.out文件.
    */
    void Init(char* pszDotOutName);

    /**
    * @brief 卸载DSPLink
    */
    void Uninit();

    /**
    * @brief 设置图片旋转标识命令
    */
    HRESULT SendImageEddyTypeCmd(IMAGE_EDDYTYPE_PARAM *pEddyTypeParam);

    /**
    * @brief 矫正DSP端系统TICK命令
    */
    HRESULT SendRectifyTimeCmd(RECTIFY_TIME_PARAM* pInitParam);

    /**
    * @brief 初始化图片识别对象命令
    */
    HRESULT SendInitPhotoRecogerCmd(INIT_PHOTO_RECOGER_PARAM* pInitParam);

    /**
    * @brief 图片识别命令
    */
    HRESULT SendProcessPhotoCmd(
        PROCESS_PHOTO_PARAM* pProcessParam,
        PROCESS_PHOTO_DATA* pProcessData,
        PROCESS_PHOTO_RESPOND* pProcessRespond
    );

    /**
    * @brief 初始化视频识别对象识别命令
    */
    HRESULT SendInitVideoRecogerCmd(INIT_VIDEO_RECOGER_PARAM* pInitParam);

    /**
    * @brief 处理一帧视频命令
    */
    HRESULT SendProcessOneFrameCmd(
        PROCESS_ONE_FRAME_PARAM* pProcessParam,
        PROCESS_ONE_FRAME_DATA* pProcessData,
        PROCESS_ONE_FRAME_RESPOND* pProcessRespond
    );

    /**
    * @brief JPGE编码命令
    */
    HRESULT SendJpegEncodeCmd(
        const JPEG_ENCODE_PARAM* pcParam,
        const JPEG_ENCODE_DATA* pcData,
        JPEG_ENCODE_RESPOND* pcRespond
    );

    /**
    * @brief H.264编码命令
    */
    HRESULT SendH264EncodeCmd(
        const H264_ENCODE_PARAM* pcParam,
        const H264_ENCODE_DATA* pcData,
        H264_ENCODE_RESPOND* pcRespond
    );

    /**
    * @brief 初始化字符叠加命令
    */
    HRESULT SendStringOverlayInitCmd(
        const STRING_OVERLAY_PARAM* pcParam,
        const STRING_OVERLAY_DATA* pcData,
        STRING_OVERLAY_RESPOND* pcRespond
    );

    /**
    * @brief 相机平台信号处理命令
    */
    HRESULT SendCamDspCmd(
        const CAM_DSP_PARAM* pcParam,
        const CAM_DSP_DATA* pcData,
        CAM_DSP_RESPOND* pcRespond
    );

    HRESULT SetSoftTriggerCallback(SOFTTRIGGER_CALLBACK_FUNC pSoftTriggerCallbackFunc);

private:
    HRESULT Lock()
    {
        SemPend(&m_semLock);
        return S_OK;
    }

    HRESULT UnLock()
    {
        SemPost(&m_semLock);
        return S_OK;
    }

    void Reload();

private:
    DSPLinkBuffer m_paramSendCmd;
    DSPLinkBuffer m_paramRecvCmd;
    HV_SEM_HANDLE m_semLock;

    int m_nJpegEncodeErrorCount;
    int m_nH264EncodeErrorCount;
    int m_nCamDspErrorCount;
    int m_nProcessOneFrameErrorCount;
    int m_nSoftTriggerFD;
    char m_szDspFileName[256];

    static const int s_nMaxTryCount = 3;
};

extern CHvDspLinkApi g_cHvDspLinkApi;

#endif
