/*
 *	(C)版权所有 2010 北京信路威科技发展有限公司
 */

/**
* @file		VideoGetter_VPIF.h
* @version	1.0
* @brief	CVideoGetter_VPIF类的定义
* @author	Shaorg
* @date		2010-11-24
*/

#ifndef _VIDEOGETTER_VPIF_H_
#define _VIDEOGETTER_VPIF_H_

#include "ImgGatherer.h"
#include "GetImgByVPIF.h"
#include "signal.h"
#include "CameraController.h"

typedef enum _RUN_MODE
{
    SRM_VIDEO = 0,  // 视频流模式（相机端使用）注：默认模式
    SRM_CAPTURE,    // 带视频流的触发抓拍模式（识别端使用）
    SRM_HVC         // 单触发抓拍模式（HVC专用）
}
RUN_MODE;

typedef enum _FRAME_STATE_FLAG
{
    FS_NORMAL = 0,
    FS_ENCODE,
    FS_REMOVE,
    FS_HOLD
}
FRAME_STATE_FLAG;

/*! @class	CVideoGetter_VPIF
* @brief	通过VPIF采集前端相机发来的BT1200格式的图片数据流。
*/
class CVideoGetter_VPIF :
            public IImgGatherer,
            public ICamTrigger,
            public CHvThreadBase
{
public:
    CVideoGetter_VPIF();
    virtual ~CVideoGetter_VPIF();

public:
    //IImgGatherer
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes);
    virtual HRESULT SetLightType(LIGHT_TYPE cLightType, int iCplStatus);
    virtual HRESULT SetCamCfgParam(LPVOID pCfgCamParam);
    virtual HRESULT MountTransmiter(ICamTransmit** ppCamTransmit)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT SetImgFrameParam(const IMG_FRAME_PARAM& cParam)
    {
        m_cImgFrameParam = cParam;
        return S_OK;
    }
    virtual HRESULT Play(LPVOID lpParam)
    {
        return Start(lpParam);
    }
    virtual HRESULT SetImageEddyType(int nEddyType)
    {
        m_iEddyType = nEddyType;
        if ( 1 == m_iEddyType )
        {
            // 因为逆时针旋转90度，所以需要调换宽高。
            int iTmp = 0;
            iTmp = m_iImageWidth;
            m_iImageWidth = m_iImageHeight;
            m_iImageHeight = iTmp;
        }
        return S_OK;
    }
    virtual BOOL IsCamConnected()
    {
        return TRUE;
    }

public:
    //ICamTrigger
    virtual HRESULT Trigger(LIGHT_TYPE nLightType, int iRoadNum = 0)
    {
        HRESULT hr = E_FAIL;
        if (m_pSignalMatch || m_eRunMode == SRM_HVC)
        {
            hr = SoftTrigger();
        }
        return hr;
    }
    virtual HRESULT HardTrigger(int iRoadNum)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT SetCamTriggerParam(CAM_CFG_PARAM *pCfgCamParam)
    {
        return E_NOTIMPL;
    }

public:
    //CHvThreadBase
    virtual const char* GetName()
    {
        static char szName[] = "CVideoGetter_VPIF";
        return szName;
    }

    virtual HRESULT Run(void* pvParam);

public:
    // 设置VPIF采集口运行模式
    HRESULT SetRunMode(RUN_MODE mode)
    {
        m_eRunMode = mode;
        return S_OK;
    }

    // 软触发抓拍
    HRESULT SoftTrigger()
    {
        HRESULT hr = E_FAIL;
#if !defined(SINGLE_BOARD_PLATFORM) && defined(_CAM_APP_)
        hr = (0 == g_cCameraController.SoftTriggerCapture()) ? S_OK : E_FAIL;
#elif !defined(SINGLE_BOARD_PLATFORM) && !defined(_CAM_APP_)
        hr = (0 == g_cCameraController.SlaveSoftTriggerCapture()) ? S_OK : E_FAIL;
#endif
        return hr;
    }

    // 挂载信号匹配器
    void MountSignalMatch(IOuterControler* pOuterControler)
    {
        m_pSignalMatch = pOuterControler;
        m_eRunMode = SRM_CAPTURE;
    }

    void PutCaptureBuffer(void* addr);

    bool CheckIfChangeParam(void)
    {
        return m_fIsChangeCamParam || !m_pCamCfgParam->iDynamicCfgEnable;
    }

    HRESULT SetPannelStatus(int iCplStatus, int iPulseLevel);

    HRESULT SyncPLMode(int iMode)
    {
        m_iCurCplStatus = iMode;
        return S_OK;
    }

private:
    HRESULT OpenVPIF();
    HRESULT CloseVPIF();
    HRESULT RunVideoMode(void* pvParam);
    HRESULT RunCaptureMode(void* pvParam);
    HRESULT RunHvcMode(void* pvParam);
    HRESULT UpdateCamParam();
    HRESULT UpdatePannelStatus();
    void OnRecvCaptureImage(const IMG_FRAME& imgFrame);

private:
    DWORD32 m_dwLastThreadIsOkTime;

    IMG_FRAME_PARAM m_cImgFrameParam;
    int m_iRunCaptureMode;

    RUN_MODE m_eRunMode;

    IOuterControler* m_pSignalMatch;
    CAM_CFG_PARAM *m_pCamCfgParam;
    LIGHT_TYPE m_nCurLightType;
    int m_iCurCplStatus;
    int m_iPulseLevel;
    BOOL m_fNeedToUpdateParam;

    DWORD32 m_dwFrameCount;

    // 源图片的宽高
    int m_iImageWidth;
    int m_iImageHeight;

    int m_iEddyType;

    int m_VpifFd;
    int m_SwDevFd;
    int m_irgAGCLimit[14];

    bool m_fIsNeedToUpdatePannelStatus;
    bool m_fIsChangeCamParam;
    bool m_fPCISending;
};

class CReferenceFrame_VPIF
{
public:
    CReferenceFrame_VPIF(
        CVideoGetter_VPIF* pVPIF,
        HV_COMPONENT_IMAGE imgVPIF,
        DWORD32 dwTick,
        BOOL fIsCapture
    )
    {
        m_pVPIF = pVPIF;
        m_imgVPIF = imgVPIF;
        m_dwTick = dwTick;
        m_fIsCapture = fIsCapture;
        m_dwFrameFlag = FS_NORMAL;
        m_nRef = 1;
        CreateSemaphore(&m_hSemLock, 1, 1);        
    }
    ~CReferenceFrame_VPIF()
    {
        if(m_pVPIF)
        {
            m_pVPIF->PutCaptureBuffer(m_imgVPIF.rgImageData[0].addr);
        }
        DestroySemaphore(&m_hSemLock);        
    }
public:
    int AddRef()
    {
        SemPend(&m_hSemLock);
        ++m_nRef;
        SemPost(&m_hSemLock);
        return m_nRef;
    }
    void Release()
    {
        BOOL fNeedDel = FALSE;
        SemPend(&m_hSemLock);
        --m_nRef;
        fNeedDel = (m_nRef <= 0);
        SemPost(&m_hSemLock);
        if (fNeedDel)
        {
            delete this;
        }
    }
public:
    const HV_COMPONENT_IMAGE* GetImage()
    {
        return &m_imgVPIF;
    }
    DWORD32 GetTimeTick()
    {
        return m_dwTick;
    }
    BOOL IsCapture()
    {
        return m_fIsCapture;
    }
    DWORD32 GetFlag()
    {
        return m_dwFrameFlag;
    }
    void SetFlag(DWORD32 dwFlag)
    {
       m_dwFrameFlag = dwFlag;
    }
    void SetCaptureFlag(BOOL fIsCapture)
    {
        m_fIsCapture = fIsCapture;
    }    
private:
    CReferenceFrame_VPIF(); //禁止默认构造函数
private:
    sig_atomic_t m_nRef;
    CVideoGetter_VPIF* m_pVPIF;
    HV_COMPONENT_IMAGE m_imgVPIF;
    DWORD32 m_dwTick;
    BOOL m_fIsCapture;
    DWORD32 m_dwFrameFlag;
    HV_SEM_HANDLE m_hSemLock;    
};

HRESULT CreateReferenceFrame_VPIF(
    CReferenceFrame_VPIF** ppRefFrame_VPIF,
    CVideoGetter_VPIF* pVPIF,
    HV_COMPONENT_IMAGE imgVPIF,
    DWORD32 dwTimeTick,
    BOOL fIsCapture
);

#endif
