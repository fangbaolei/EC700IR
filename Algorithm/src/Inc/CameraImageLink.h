// 该文件编码格式必须是WIN936

// cameraimagelink.h
// 定义相机图片传输接口

#ifndef _CAMERAIMAGELINK_H_
#define _CAMERAIMAGELINK_H_

#include "CameraLinkBase.h"
#include "HvCameraLink.h"
#include "hvenum.h"
#include "tinyxml.h"

#include <list>

//相机图片连接类
class CCameraImageLink : public CHvThreadBase
{
public:
    // CHvThreadBase
    virtual const char* GetName()
    {
        static char szName[] = "CCameraImageLink";
        return szName;
    }
    virtual HRESULT Run(void* pvParam);
    bool ThreadIsOk();

public:
    CCameraImageLink(HV_SOCKET_HANDLE hSocket);
    ~CCameraImageLink();

    HRESULT SendImage(CCameraDataReference* pRefImage);
    HRESULT IsEnableRecogVideo()
    {
        return m_fEnableRecogVideo;
    }

    DWORD32 GetStatus()
    {
        return m_dwThreadStatus;
    }
    HRESULT SetSocket(HV_SOCKET_HANDLE& hSocket);

    BOOL CheckIsPanoramicMode()
    {
        return m_fPanoramicMode;
    }

private:
    HRESULT ProcessCmd(HvCore::IHvStream* pSocketStream);
    HRESULT ProcessXMLInfo(unsigned char* pbInfo, const DWORD32& dwInfoSize);
    HRESULT SetImgType(const TiXmlElement* pCmdArgElement);
    HRESULT DisConnect();
    // 初始化参数,连接时才确定的参数。
    HRESULT Init();

private:
    static const int MAX_CAMERA_IMAGE_COUNT = 2;
    std::list<CCameraDataReference*> m_lstImage;

    CAMERA_INFO_HEADER m_cameraThrob;
    CAMERA_INFO_HEADER m_cameraThrobResponse;

    HV_SEM_HANDLE m_hSemImage;
    HV_SEM_HANDLE m_hSemListImage;  //Add by Shaorg: 为了m_lstImage变量的线程安全
    HV_SOCKET_HANDLE m_hSocket;
    HvCore::IHvStream* m_pSocketStream;

    static const int MAX_WAIT_MS = 200000;
    DWORD32 m_dwThreadIsOkMs;

    DWORD32 m_dwConnectedLogId;

    BOOL m_fEnableRecogVideo;
    BOOL m_fPanoramicMode;
    // 连接当前状态: 0 : 挂起，1： 正常运行
    DWORD32 m_dwThreadStatus;
};


// 相机图片连接枚举类
class CCameraImageLinkEnum : public CHvEnumBase<CCameraImageLink*>
{
protected:
    HRESULT OnDelNode(NODE* pNode)
    {
        SAFE_DELETE(pNode->Dat);
        return S_OK;
    }
};

// 相机图片连接控制类
class CCameraImageLinkCtrl : public HiVideo::ISendCameraImage
            , public CCameraLinkCtrlBase
{
public:
    // ISendCamImage
    virtual HRESULT SendCameraImage(const SEND_CAMERA_IMAGE* pImage);
    virtual HRESULT SetParam(const SEND_IMAGE_PARAM* pParam);
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes)
    {
        return (true == ThreadIsOk()) ? S_OK : E_FAIL;
    };
    virtual HRESULT GetIsCaptureMode(BOOL* fIsCaptureLink)
    {
        if(fIsCaptureLink == NULL)
        {
            return E_FAIL;
        }
        *fIsCaptureLink = FALSE;
        if(GetRunCaptureModeLinkCount() > 0)
        {
            *fIsCaptureLink = TRUE;
        }
        return S_OK;
    }

    // CCameraLinkCtrlBase
    virtual int GetLinkCount();
    // 删除空闲的连接，同时返回当前的连接数。
    virtual int DeleteFreeLink();
    // 清除所有的连接。
    virtual HRESULT ClearLink();
    // 创建总的连接线程
    virtual HRESULT CreateLinkThread();
    // 新的连接
    virtual HRESULT SetLink(HV_SOCKET_HANDLE& hSocket);
    //只获取抓拍图的连接个数
    virtual int GetRunCaptureModeLinkCount();

    bool ThreadIsOk();

public:
    CCameraImageLinkCtrl();
    ~CCameraImageLinkCtrl();

private:
    HRESULT MakeNode(
        const DWORD32& dwID,
        const DWORD32& dwNodeLen,
        const unsigned char* pbNode,
        unsigned char* pbInfo
    );

    HRESULT EnumLock();
    HRESULT EnumUnLock();
    HRESULT SendLock();
    HRESULT SendUnLock();

private:
    static const int MAX_LINK_COUNT = 2;
    CCameraImageLinkEnum m_cCameraImageLinkEnum;
    HV_SEM_HANDLE m_hSemEnum;
    HV_SEM_HANDLE m_hSemSendCameraImage;

    static const int MAX_WAIT_MS = 200000;
};

#endif
