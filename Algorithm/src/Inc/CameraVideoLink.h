// 该文件编码格式必须是WIN936

// cameravideolink.h
// 定义相机视频传输接口

#ifndef _CAMERAVIDEOLINK_H_
#define _CAMERAVIDEOLINK_H_

#include "CameraLinkBase.h"
#include "HvCameraLink.h"
#include "hvenum.h"
#include "tinyxml.h"

#include <list>

typedef struct tag_CameraVideoData
{
    unsigned char * pbHeader;
    DWORD32 dwHeaderSize;
    unsigned char * pbInfo;
    DWORD32 dwInfoSize;
    unsigned char * pbDataSize;
    DWORD32 dwDataSizeSize;
    unsigned char * pbData;
    DWORD32 dwDataSize;
    DWORD32 dwFrameID;
}SEND_VIDEO_DATA;

//视频流连接类
class CCameraVideoLink : public CHvThreadBase
{
public:
    // CHvThreadBase
    virtual const char* GetName()
    {
        static char szName[] = "CCameraVideoLink";
        return szName;
    }
    virtual HRESULT Run(void* pvParam);
    bool ThreadIsOk();

public:
    CCameraVideoLink(HV_SOCKET_HANDLE hSocket, SEND_VIDEO_PARAM& cSendVideoParam);
    ~CCameraVideoLink();

    HRESULT SendVideo(CCameraDataReference* pRefImage, DWORD32 dwFrameID );
    bool IsSendHistroyVideo();

    DWORD32 GetStatus()
    {
        return m_dwThreadStatus;
    }

    bool VideoDataSend( const unsigned char * pbHeader,const DWORD32 dwHeaderSize,
                                 const unsigned char * pbInfo,const DWORD32 dwInfoSize,
                                 const unsigned char * pbDataSize,const DWORD32 dwDataSizeSize,
                                 const unsigned char * pbData,const DWORD32 dwDataSize,
                                 const DWORD32 dwFrameID);

    HRESULT SetSocket(HV_SOCKET_HANDLE& hSocket);

private:
    HRESULT ProcessHistroyVideo(HvCore::IHvStream* pSocketStream);
    HRESULT ProcessCmd(HvCore::IHvStream* pSocketStream);
    HRESULT ProcessXMLInfo(unsigned char* pbInfo, const DWORD32& dwInfoSize);
    HRESULT DownloadVideoXML(const TiXmlElement* pCmdArgElement);
    // 初始化参数,连接时才确定的参数。
    HRESULT Init();

private:
    static const int MAX_CAMERA_VIDEO_COUNT = 2;

    static const unsigned int MAX_DATA_BUFFER_SIZE = 2*1024*1024;
    unsigned char * m_pbDataBuffer;
    std::list<SEND_VIDEO_DATA> m_lstVideoData;
    int m_iVideoState;

    CAMERA_INFO_HEADER m_cameraThrob;
    CAMERA_INFO_HEADER m_cameraThrobResponse;

    HV_SEM_HANDLE m_hSemVideo;
    HV_SEM_HANDLE m_hSemListVideo;  //Add by Shaorg: 为了m_lstVideo变量的线程安全
    HV_SOCKET_HANDLE m_hSocket;
    HvCore::IHvStream* m_pSocketStream;
    //引用CameraVideoLinkCtrl的变量，避免参数在连接建立之后才赋值的情况下发送线程参数不生效
    SEND_VIDEO_PARAM & m_cSendVideoParam;

    static const int MAX_WAIT_MS = 200000;
    DWORD32 m_dwThreadIsOkMs;

    //可靠性连接方式相关
    DWORD32 m_dwStartTimeLow;
    DWORD32 m_dwStartTimeHigh;
    DWORD32 m_dwEndTimeLow;
    DWORD32 m_dwEndTimeHigh;
    bool m_fSendHistroyEnable;

    DWORD32 m_dwConnectedLogId;

    // 连接当前状态: 0 : 挂起，1： 正常运行
    DWORD32 m_dwThreadStatus;
    CAMERA_INFO_HEADER		m_cHistoryEnd;    //历史录像结束标志，下发客户端
};

// 视频流连接枚举类
class CCameraVideoLinkEnum : public CHvEnumBase<CCameraVideoLink*>
{
protected:
    HRESULT OnDelNode(NODE* pNode)
    {
        SAFE_DELETE(pNode->Dat);
        return S_OK;
    }
};

// 视频流连接控制类
class CCameraVideoLinkCtrl : public HiVideo::ISendCameraVideo
            , public CCameraLinkCtrlBase
{
public:
    // ISendCameraVideo
    HRESULT SendCameraVideo(const SEND_CAMERA_VIDEO* pVideo);
    HRESULT SetParam(const SEND_VIDEO_PARAM* pParam);
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes)
    {
        return (true == ThreadIsOk()) ? S_OK : E_FAIL;
    };

    // CCameraLinkCtrlBase
    // 删除空闲的连接，同时返回当前的连接数。
    virtual int DeleteFreeLink();
    // 清除所有的连接。
    virtual HRESULT ClearLink();
    // 创建总的连接线程
    virtual HRESULT CreateLinkThread();
    // 新的连接
    virtual HRESULT SetLink(HV_SOCKET_HANDLE& hSocket);

    bool ThreadIsOk();

	int GetRealTimeLinkCount();

public:
    CCameraVideoLinkCtrl();
    ~CCameraVideoLinkCtrl();

    HRESULT EnumLock();
    HRESULT EnumUnLock();

private:
    static const int MAX_LINK_COUNT = 2;
    CCameraVideoLinkEnum m_cCameraVideoLinkEnum;
    HV_SEM_HANDLE m_hSemEnum;

    static const int MAX_WAIT_MS = 200000;
    SEND_VIDEO_PARAM m_cSendVideoParam;

    // zhaopy 一秒视频流的大小
#ifndef SINGLE_BOARD_PLATFORM
    static const int MAX_VIDEO_SIZE = int(1.5 * 1024 * 1024);
#else
    static const int MAX_VIDEO_SIZE = 2 * 1024 * 1024;
#endif
    unsigned char* m_pbVideoData;
    int m_iVideoSize;
    DWORD32 m_dwVideoTimeLow;
    DWORD32 m_dwVideoTimeHigh;
};

#endif
