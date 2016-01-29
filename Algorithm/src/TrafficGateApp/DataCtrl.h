/**
* @file	DataCtrl.h
* @version	1.0
* @brief 数据流控制
*/

#ifndef _DATACTRL_H_
#define _DATACTRL_H_

#include "hvthreadbase.h"
#include "tracker.h"
#include "ImgGatherer.h"
#include "ImgProcesser.h"
#include "hvmodelstore.h"
#include "tinyxml.h"
#include "VideoGetter_VPIF.h"

//结果传送及命令连接部分头文件
#include "IResultSender.h"
#include "HvCmdLink.h"
#include "ControlCommand.h"
#include "ResultSenderImpl.h"
#include "sendnetdata.h"

#include "IPSearchThread.h"
#include "LoadParam.h"
#include "misc.h"

#include "HvPciLinkApi.h"
#include "PciResultSender.h"
#include "DataCtrlBase.h"
#include "Queue.h"
extern CHvModelStore g_modelStore;
extern const int MAX_PLATE_STRING_SIZE;

/**
*  TrafficGate控制类
*/
class CTrafficGate : public CHvThreadBase, public IDataCtrl
{
public:
    CTrafficGate();
    ~CTrafficGate();
    HRESULT Run(void* pvParam);  // CHvThreadBase
    virtual HRESULT CarArrive(
        CARARRIVE_INFO_STRUCT *pCarArriveInfo,
        LPVOID pvUserData
    );
    virtual HRESULT CarLeft(
        CARLEFT_INFO_STRUCT *pCarLeftInfo,
        LPVOID pvUserData
    );
    virtual float GetFrameRate()
    {
        return m_fltFrameRate;
    }
    HRESULT DisposeUserData(
        UINT iFrameNo,
        UINT iRefTime,
        LPVOID pvUserData
    )
    {
        return E_NOTIMPL;
    }
    void MountVideoGetter(IImgGatherer* pVideoGetter)
    {
        m_pVideoGetter = pVideoGetter;
        if (NULL != m_pVideoGetter)
        {
        	m_pVideoGetter->RegisterImageCallbackFunction(OnImage, this);
        }
    }
    void MountResultSender(IResultSender* pResultSender)
    {
        m_pResultSender = pResultSender;
    }
    void MountVideoRecoger(IVideoRecoger* pVideoRecoger)
    {
        m_pVideoRecoger = pVideoRecoger;
    }
    void MountSignalMatch(IOuterControler* pOuterControler)
    {
        m_pSignalMatch = pOuterControler;
    }
    void MountImgCapturer(IImgGatherer* pImgCapturer)
    {
        m_pImgCapturer = pImgCapturer;
    }
    void MountTriggerCam(ICamTrigger* pTriggerCam)
    {
        m_pCamTrigger = pTriggerCam;
    }
    void MountLightTypeSaver(CCamLightTypeSaver* pLightTypeSaver)
    {
        m_pLightTypeSaver = pLightTypeSaver;
    }
#ifdef SINGLE_BOARD_PLATFORM
    void MountVideoSender(HiVideo::ISendCameraVideo* pVideoSender)
    {
        m_pVideoSender = pVideoSender;
    }
#endif
    HRESULT BuildPlateString(
        char* pszPlateString,
        int* piPlateStringSize,
        CARLEFT_INFO_STRUCT *pCarLeftInfo
    );

    HRESULT ForceSend(DWORD32 dwVideoID);
    HRESULT GetWorkModeInfo(HvSys::SYS_INFO* pInfo);
    HRESULT GetWorkModeList(HvSys::SYS_INFO* pInfo);

    bool ThreadIsOk(int* piErrCode);
    void SetModuleParams(const ModuleParams& cParam);
    void SetLastResultTime(DWORD32 dwTime)
    {
        m_dwLastSendTime = dwTime;
    }
    DWORD32 GetLastResultTime()
    {
        return m_dwLastSendTime;
    }

    void DspSoftTrigger();

    void SetFlashLampDiff(int iDiff)
    {
        m_nFlashLampDiff = iDiff;
    }
protected:
	static void OnImage(void* pContext, IMG_FRAME imgFrame);

private:
    void GetCarColor(CARLEFT_INFO_STRUCT *pCarLeftInfo, char *pszConf);
    void GetCarType(PROCESS_IMAGE_CORE_RESULT *pImgCoreInfo, char *pszConf);
    void GetEventDetInfo(CARLEFT_INFO_STRUCT *pCarLeftInfo, char *pszConf);
    HRESULT SendResult(
        LPCSTR szResultInfo,
        LPVOID lpcData
    );
    HRESULT ScaleCarFlowRate();
    HRESULT SetAWBFromLightType(LIGHT_TYPE emLightType);
    HRESULT SetCaptureSynSignalEnableFlash(bool fIsNight);

private:
    IVideoRecoger* m_pVideoRecoger;
    IImgGatherer* m_pVideoGetter;
    TiXmlDocument* m_pXmlDoc;
    IResultSender* m_pResultSender;
    ModuleParams m_cModuleParams;
    IOuterControler* m_pSignalMatch;
    IImgGatherer* m_pImgCapturer;
    ICamTrigger* m_pCamTrigger;
    CCamLightTypeSaver* m_pLightTypeSaver;
#ifdef SINGLE_BOARD_PLATFORM
	HiVideo::ISendCameraVideo* m_pVideoSender;
#endif
    int m_nCarLeftCount;
    int m_nLightType;
    char m_szPlateString[MAX_PLATE_STRING_SIZE];
    float m_fltFrameRate;
    DWORD32 m_dwLastSendTime;
    CCarLeftThread* m_pCarLeftThread;
    int m_nLastLightLevel;
    int m_nFlashLampDiff;
    int m_iFlashStatus;

    CQueue<IMG_FRAME, IMG_LIST_COUNT> m_queImage;
};

extern CTrafficGate *g_pTrafficGate;

#endif
