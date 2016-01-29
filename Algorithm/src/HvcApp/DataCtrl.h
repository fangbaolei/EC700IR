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
*  HVC控制类
*/
class CHVC : public CHvThreadBase, public IDataCtrl
{
public:
    CHVC();
    ~CHVC();
    virtual HRESULT Run(void* pvParam);  // CHvThreadBase

    // DataCtrlBase
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
        return 0.0f;
    }
    HRESULT DisposeUserData(
        UINT iFrameNo,
        UINT iRefTime,
        LPVOID pvUserData
    )
    {
        return E_NOTIMPL;
    }
    void MountImgCapturer(IImgGatherer* pImgCapturer)
    {
        m_pImgCapturer = pImgCapturer;
        if (NULL != m_pImgCapturer)
        {
        	m_pImgCapturer->RegisterImageCallbackFunction(OnImage, this);
        }
    }
    void MountResultSender(IResultSender* pResultSender)
    {
        m_pResultSender = pResultSender;
    }
    void MountPhotoRecoger(IPhotoRecoger* pPhotoRecoger)
    {
        m_pPhotoRecoger = pPhotoRecoger;
    }
    void MountTriggerCam(ICamTrigger* pTriggerCam)
    {
        m_pCamTrigger = pTriggerCam;
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
protected:
	static void OnImage(void* pContext, IMG_FRAME imgFrame);

private:
    void GetCarColor(CARLEFT_INFO_STRUCT *pCarLeftInfo, char *pszConf);

private:
    IPhotoRecoger* m_pPhotoRecoger;
    IImgGatherer* m_pImgCapturer;
    TiXmlDocument* m_pXmlDoc;
    IResultSender* m_pResultSender;
    ICamTrigger *m_pCamTrigger;
#ifdef SINGLE_BOARD_PLATFORM
	HiVideo::ISendCameraVideo* m_pVideoSender;
#endif
private:
    char m_szPlateString[MAX_PLATE_STRING_SIZE];
    ModuleParams m_cModuleParams;
    DWORD32 m_dwLastSendTime;

    CQueue<IMG_FRAME, IMG_LIST_COUNT> m_queImage;
};

extern CHVC *g_pHVC;

#endif
