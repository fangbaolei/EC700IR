#ifndef _DATACTRL_H_
#define _DATACTRL_H_

#include "LoadParam.h"
#include "hvthreadbase.h"
#include "IResultSender.h"
#include "HvCmdLink.h"
#include "ControlCommand.h"
#include "ResultSenderImpl.h"
#include "sendnetdata.h"
#include "VideoGetter_VPIF.h"
#include "CamyuServer.h"
#include "HvCameraLink.h"
#include "HvPciLinkApi.h"
#include "AgcAwbThread.h"
#include "hvtarget_ARM.h"
#include "HvUtilsClass.h"
#include "DataMux.h"
#include "FrontController.h"
#include "Queue.h"

#ifdef _CAMERA_PIXEL_500W_
#define IMG_FRAME_QUEUE_MAX_COUNT 3
#else
#define IMG_FRAME_QUEUE_MAX_COUNT 2
#endif

// 相机输出模式
#define CAM_OUTPUT_NONE -1      // 无码流
#define CAM_OUTPUT_JPEG_ONLY 0  // Jpeg流
#define CAM_OUTPUT_H264_ONLY 1  // H.264流
#define CAM_OUTPUT_JPEG_H264 2  // 双码流
#define CAM_OUTPUT_DEBUG 3      // 调试码流（即：主从端的Jpeg码流）

// H264_FRAME_MAX_COUNT意义：
// 目前使用的TI提供的H.264编码库有个问题，
// 即：如果在打开编码器后连续编码到一定帧数（具体帧数无法确定，大概在50000帧左右）后，
// 编出来的内容会开始出现坏块，刚开始只是一点点，之后会越来越支离破碎，
// 但是关闭之后再重新打开编码器就又可以正常编码了，所以，为了解决这个问题，
// 目前是在编码了H264_FRAME_MAX_COUNT帧图像后就关开一次编码器。
#define H264_FRAME_MAX_COUNT 2500
#define H264_HEADER_MAX_LEN 32
#define H264_HEADER_LEN 23

// H.264流的帧头结构
typedef struct _H264_FRAME_HEADER
{
    BYTE8 bH264Header[H264_HEADER_MAX_LEN];
    int iLenH264Header;

    _H264_FRAME_HEADER()
    {
        memset(bH264Header, 0, sizeof(bH264Header));
        iLenH264Header = 0;
    };

} H264_FRAME_HEADER;

typedef struct tagDCP
{
    DCP_TYPE eType;
    int iValue;
}DCP;

/**
*  相机数据流控制类
*/
class CCamApp : public CHvThreadBase, public ICameraUtils
{
public:
    CCamApp();
    virtual ~CCamApp();

public:
    // CHvThreadBase
    virtual const char* GetName()
    {
        static char szName[] = "CCamApp";
        return szName;
    }
    virtual HRESULT Run(void* pvParam);

public:
    void MountVideoGetter(IImgGatherer* pVideoGetter)
    {
        m_pVideoGetter = pVideoGetter;
    }

    void MountImageSender(HiVideo::ISendCameraImage* pImageSender)
    {
        m_pImageSender = pImageSender;
    }

    void MountVideoSender(HiVideo::ISendCameraVideo* pVideoSender)
    {
        m_pVideoSender = pVideoSender;
        if (NULL != m_pVideoGetter)
        {
            m_pVideoGetter->RegisterImageCallbackFunction(OnImage, this);
        }
    }

    void MountRecordSender(HiVideo::ISendRecord* pRecordSender)
    {
        m_pRecordSender = pRecordSender;
    }

    void MountFrontController(CFrontController* pFrontController)
    {
        m_pFrontController = pFrontController;
    }

    void MountSafeSaver(ISafeSaver* pSafeSaver)
    {
        m_pSafeSaver = pSafeSaver;
    }

public:
    void SetCameraType(int iCamType)
    {
        m_iCamType = iCamType;
    }

    virtual int SetAGCZone(int rgiAGCZone[16])
    {
        for (int i = 0; i < 16; i++)
        {
            m_rgiRawAGCZone[i] = rgiAGCZone[i];
            m_rgiAGCZone[i] = rgiAGCZone[i];
        }
        if (g_cModuleParams.cCamAppParam.fIsSideInstall)
        {
            const static int MatrixRotateFactor[] = {12, 8, 4, 0, 13, 9, 5, 1, 14, 10, 6, 2, 15, 11, 7, 3};
            for (int i = 0; i < 16; i++)
            {
                m_rgiAGCZone[i] = rgiAGCZone[MatrixRotateFactor[i]];
                HV_Trace(5, "%d==>%d", rgiAGCZone[i], m_rgiAGCZone[i]);
            }
        }
        return 0;
    }

    virtual int GetAGCZone(int rgiAGCZone[16])
    {
        for (int i = 0; i < 16; i++)
        {
            rgiAGCZone[i] = m_rgiRawAGCZone[i];
        }
        return 0;
    }

    virtual int SetAGCParam(int iShutterMin, int iShutterMax, int iGainMin, int iGainMax)
    {
        g_cModuleParams.cCamAppParam.iAGCShutterLOri = iShutterMin;
        g_cModuleParams.cCamAppParam.iAGCShutterHOri = iShutterMax;
        g_cModuleParams.cCamAppParam.iAGCGainLOri = iGainMin;
        g_cModuleParams.cCamAppParam.iAGCGainHOri = iGainMax;

        m_cAgcAwbThread.SetAGCParam(iShutterMin, iShutterMax, iGainMin, iGainMax);
        return 0;
    }
    virtual HRESULT SaveCurrentParam()
    {
        return SaveModuleParam(g_cModuleParams);
    }
    virtual HRESULT DynChangeParam(DCP_TYPE eType, int iValue, bool fFromPCI = false);
    void GetNowAGCShutterGain(int& iShutter, int& iGain)
    {
        m_cAgcAwbThread.GetAGCShutterGain(iShutter, iGain);
    }

    void GetNowAWBGain(int& iGainR, int& iGainG, int& iGainB)
    {
        m_cAgcAwbThread.GetAWBGain(iGainR, iGainG, iGainB);
    }

    int GetJpegStreamParam()
    {
        return m_iJpegStream;
    }

    int GetEncodeMode()
    {
        if ( 1 == m_iJpegStream && 0 == m_iH264Stream )
        {
            return CAM_OUTPUT_JPEG_ONLY;
        }
        else if ( 0 == m_iJpegStream && 1 == m_iH264Stream )
        {
            return CAM_OUTPUT_H264_ONLY;
        }
        else if ( 1 == m_iJpegStream && 1 == m_iH264Stream )
        {
            return CAM_OUTPUT_JPEG_H264;
        }
        else
        {
            return CAM_OUTPUT_NONE;
        }
    }

    bool AGCIsEnable()
    {
        return (1 == m_iEnableAGC) ? (true) : (false);
    }

    bool AWBIsEnable()
    {
        return (1 == m_iEnableAWB) ? (true) : (false);
    }

    void SetPlateY(int iYPlate, DWORD32 dwPlateTick)
    {
        m_iYPlate = iYPlate;
        m_dwPlateTick = dwPlateTick;
    }
public:
    bool ThreadIsOk(int* piErrCode);
    void ResetH264Enc(int iENetSyn);

    HRESULT GetHddStateInfo(char* szInfo, int& nLen);  // 获取硬盘状态信息
    HRESULT GetModStateInfo(char* szInfo, int& nLen);  // 获取模块状态信息
    HRESULT OpenPL();
    HRESULT ClosePL();
    HRESULT GetPannelVersion(unsigned char* rgbVersionBuf, int& iBufLen);
    HRESULT GetPannelTemperature(float& fTemperature);
    HRESULT GetPannelTime(unsigned int& uiTimeMS);
    HRESULT GetPannelStatus(int& iWorkMode, int& iWorkStatus);
    HRESULT GetPulseInfo(int& iPulseLevel, int& iPulseStep, int& iPulseWidthMin, int& iPulseWidthMax);
    HRESULT GetOutDeviceStatus(int& iPolarizingPrismStatus, int& iPalanceLightStatus);
    HRESULT GetFlashInfo(void* pBuffer, int iBufLen);
    HRESULT InitControllPannel(void);
    HRESULT SetControllPannelStatus(int iWorkMode, int iWorkStatus);
    HRESULT SetPulseWidthRange(int iPulseMin, int iPulseMax);
    HRESULT TestPLMode(int iMode);
    HRESULT SetFlashSingle(int iChannel, int iPolarity, int iTriggerType, int iPulseWidth, int iCoupling);
    HRESULT GetCurrentPulseLevel(int& iPulseLevel, int& iCplStatus);
    HRESULT UpdateControllPannel(unsigned char* pUpdateFile);
    HRESULT GetControllPannelUpdatingStatus(int& iUpdatingStatus, int& iUpdatePageIndex);
    HRESULT GetControllPannelCRCValue(int& iCRCValue);
    HRESULT ResetControllPannel(void);
    HRESULT GetControllPannelAutoRunStatus(int& iStatus);
    HRESULT SetControllPannelAutoRunStatus(int iMode);
    HRESULT SetTestModePulseLevel(int iPulseLevel);
    HRESULT GetControllPannelStyle(int& iStyle);
    HRESULT SetControllPannelDefaultParam(void);
    HRESULT GetControllPannelRunStatus(int& iRunStatus);
    HRESULT SetCaptureSynSignalStatus(int iStatus);

    HRESULT Initialize();

private:
    void InitH264WidthHeight(BOOL fIsSideInstall);
    static void OnImage(void* pContext, IMG_FRAME imgFrame);

private:
    IImgGatherer* m_pVideoGetter;
    HiVideo::ISendCameraImage* m_pImageSender;
    HiVideo::ISendCameraVideo* m_pVideoSender;
    HiVideo::ISendRecord* m_pRecordSender;
    CFrontController* m_pFrontController;
    ISafeSaver* m_pSafeSaver;
    // zhaopy auto...
    int m_iYPlate;
    DWORD32 m_dwPlateTick;

private:
    int m_iSendType;
    int m_iJpegStream;
    int m_iH264Stream;
    int m_iEnableAGC;
    int m_iEnableAWB;
    int m_iJpegCompressRateL;
    int m_iJpegCompressRateH;
    int m_iJpegExpectSize;
    int m_iJpegCompressRate;
    int m_iJpegCompressRateCapture;
    int m_iFlashRateSynSignalEnable;
    int m_iCaptureSynSignalEnable;
    int m_iENetSyn;
    int m_iCamType;
    int m_rgiAGCZone[16];
    int m_rgiRawAGCZone[16];
    bool m_fH264EncodeIsOpen;
    DWORD32 m_dwFrameCount;
    DWORD32 m_dwIDRFrameInterval;
    DWORD32 m_dwH264InputWidth;
    DWORD32 m_dwH264InputHeight;
    int m_iTestModePLMode;
    int m_iTestModePulseLevel;

private:
    HV_SEM_HANDLE m_hSemEncode;
    CDataMux m_cDataMux;
    CAgcAwbThread m_cAgcAwbThread;

    CQueue<IMG_FRAME, IMG_FRAME_QUEUE_MAX_COUNT> m_queImage;
    CQueue<DCP, 10>m_queDCP;
    bool m_fSendDebugImage;
};

extern CCamApp* g_pCamApp;

#endif
