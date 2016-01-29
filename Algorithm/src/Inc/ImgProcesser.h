/**
* @file	ImgProcesser.h
* @version	1.0
* @brief 图像处理模块接口定义
*/

#ifndef _IMG_PROCESSER_H_
#define _IMG_PROCESSER_H_

#include "hvutils.h"
#include "swimageobj.h"
#include "trackerdef.h"
#include "hvthread.h"
#include "TestServer.h"
//将YUV数据转换为JPEG，目前支持的YUV数据为BT1120,RAW12,CbYCrY
//如果数据源为JPEG数据，则直接返回S_OK
extern HRESULT ConvertToJpeg(IReferenceComponentImage **pRefImage, bool bReplace = false);

/**
*  IPhotoRecoger接口
*/
class IPhotoRecoger
{
public:
    virtual ~IPhotoRecoger() {};
    virtual HRESULT ProcessPhoto(
        int iVideoId,
        IReferenceComponentImage *pImage,
        PVOID pvParam,
        PROCESS_EVENT_STRUCT* pProcessEvent) = 0;
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes) = 0;

};

/**
*  IVideoRecoger接口
*/
class IVideoRecoger
{
public:
    virtual ~IVideoRecoger() {};
    virtual HRESULT ProcessOneFrame(
        int iVideoId,
        IReferenceComponentImage *pImage,
        IReferenceComponentImage *pCapImage,
        PVOID pvParam,
        PROCESS_EVENT_STRUCT* pProcessEvent) = 0;
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes) = 0;
};

/**
*  CPhotoRecoger实现类
*/
class CPhotoRecoger : public IPhotoRecoger
{
public:
    CPhotoRecoger();
    virtual ~CPhotoRecoger();
    HRESULT Init(TRACKER_CFG_PARAM& cTrackerCfgParam);
    // IPhotoRecoger接口
    virtual HRESULT ProcessPhoto(
        int iVideoId,
        IReferenceComponentImage *pImage,
        PVOID pvParam,
        PROCESS_EVENT_STRUCT* pProcessEvent
    );
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes);
private:
    int m_iEddyType;

};

/**
*  CVideoRecoger实现类
*/
class CVideoRecoger : public IVideoRecoger
{
public:
    CVideoRecoger();
    virtual ~CVideoRecoger();
    HRESULT Init(int iGlobalParamIndex, int nLastLightType,int nLastPulseLevel, int nLastCplStatus, TRACKER_CFG_PARAM& cTrackerCfgParam);
    // IVideoRecoger接口
    virtual HRESULT ProcessOneFrame(
        int iVideoID,
        IReferenceComponentImage *pImage,
        IReferenceComponentImage *pCapImage,
        PVOID pvParam,
        PROCESS_EVENT_STRUCT* pProcessEvent
    );
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes);
    virtual void    SetEncodeType(int iType){m_iEncodeType = iType;}
private:
    static const int s_knMaxTrackInfo = 30;
    static const int s_knMaxPlateCandidate = 200;
    static const int s_knMaxObjCandidate = 30;
    static const int s_knMaxVideoChannel = 4;
    HV_SEM_HANDLE m_hLockProcess;
    int m_nEddyType;
    int m_iSendTimes;

    CTestServer m_cTSServer;
    int m_iEncodeType;
};

#define CAM_LIGHTTYPE_FILENAME "LightType.dat"

typedef struct _tag_SaveLightType_Info
{
    int nLightType;
    int nPulseLevel;
    int nCplStatus;
    DWORD64 dw64LastTime;
}
SAVELIGHTTYPE_INFO;

class CCamLightTypeSaver
{
public:
    CCamLightTypeSaver(){};
    ~CCamLightTypeSaver(){};

    HRESULT GetLastLightType(int& nLightType, int& nPulseLevel , int& nCplStatus);
    HRESULT SaveLightType(int nLightType, int nPulseLevel ,int nCplStatus , UINT nCarArriveTime);
};

#endif
