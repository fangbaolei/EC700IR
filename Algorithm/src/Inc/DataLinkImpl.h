/**
* @file		DataLinkImpl.h
* @version	1.0
* @brief	数据连接核心部分
*/

#ifndef _DATA_LINK_IMPL_H_
#define _DATA_LINK_IMPL_H_

#include "hvutils.h"
#include "hvinterface.h"
#include "resultsend.h"
#include "hvenum.h"
#include "hvthreadbase.h"
#include "HvSockUtils.h"
#include "safesaver.h"
#include "SafeSaverImpl_CF.h"
using namespace HiVideo;

class CDataLinkThread : public CHvThreadBase
{
public:
    //用户接口：发送车辆信息开始接收标志
    STDMETHOD(SendInfoBegin)(
        const HVIO_CarInfoBegin* pCarInfoBegin,
        const int& index,
        DWORD32 dwFlag = 0
    );

    //用户接口：发送车辆信息结束接收标志
    STDMETHOD(SendInfoEnd)(
        const HVIO_CarInfoEnd* pCarInfoEnd,
        const int& index,
        DWORD32 dwFlag = 0
    );

    //用户接口：发送车辆到达离开标志
    STDMETHOD(SendInLeftFlag)(
        const HVIO_CarInLeft* pCarInLeft,
        const int& index,
        DWORD32 dwFlag = 0
    );

    //用户接口：发送车牌
    STDMETHOD(SendCarPlate)(
        const HVIO_CarPlate* pCarPlate,
        const int& index,
        DWORD32 dwFlag = 0
    );

    //用户接口：发送车牌附加信息
    STDMETHOD(SendAppendInfo)(
        const HVIO_AppendInfo* pAppendInfo,
        const int& index,
        DWORD32 dwFlag = 0
    );

    //用户接口：发送车辆YUV大图
    STDMETHOD(SendBigImage)(
        const HVIO_BigImage* pBigImage,
        const int& index,
        DWORD32 dwFlag = 0
    );

    //发送YUV小图
    STDMETHOD(SendSmallImageYUV)(
        const HVIO_SmallImage* pSmallImage,
        const int& index,
        DWORD32 dwFlag = 0
    );

    //发送二值化小图
    STDMETHOD(SendBinaryImage)(
        const HVIO_SmallImage* pSmallImage,
        const int& index,
        DWORD32 dwFlag = 0
    );

    //发送视频
    STDMETHOD(SendVideoYUV)(
        const HVIO_Video* pVideo,
        const int& index,
        DWORD32* pdwSendCount,
        DWORD32 dwFlag = 0
    );

    //用户接口：发送字符流
    STDMETHOD(SendString)(
        const HVIO_String* pString,
        const int& index,
        DWORD32 dwFlag = 0
    );

    //用户接口：发送二进制流
    STDMETHOD(SendBin)(
        const HVIO_Binary* pBinary,
        const int& index,
        DWORD32 dwFlag = 0
    );

    //设置过滤条件
    STDMETHOD(SetStreamID)(
        DWORD32 dwIP,
        WORD16 wPort,
        WORD16 wVideoID,
        WORD16 wStreamID
    );

    //删除过滤条件
    STDMETHOD(DeleteStreamID)(
        DWORD32 dwIP,
        WORD16 wPort,
        WORD16 wVideoID,
        WORD16 wStreamID
    );

    //清除所有过滤条件
    STDMETHOD(ClearAllStreamID)(
        DWORD32 dwIP,
        WORD16 wPort
    );

    //设置安全传送模式
    STDMETHOD(SetSafeMode)(
        DWORD32 dwIP,
        WORD16 wPort,
        DWORD32 dwTimeLow,
        DWORD32 dwTimeHigh,
        int index,
        int iPageIndex = -1,
        int iPageSize   = 0
    );

    //查询AVI文件
    STDMETHOD(GetAviByTime)(
        DWORD32 dwIP,
        WORD16 wPort,
        DWORD32 dwTimeLow,
        DWORD32 dwTimeHigh,
        DWORD32 *pdwAviTimeLow,
        DWORD32 *pdwAviTimeHigh,
        DWORD32 *pdwAviLen
    );

    //查询硬盘录像
    STDMETHOD(GetHistoryVideo)(
        DWORD32 dwIP,
        WORD16 wPort,
        DWORD32 dwTimeLow,
        DWORD32 dwTimeHigh,
        DWORD32 dwTimeSliceS
    );

public:
    virtual const char* GetName()
    {
        static char szName[] =  "CDataLinkThread";
        return szName;
    }

    virtual HRESULT Run( void* pvParam );
    bool ThreadIsOk();

public:
    static G_QUEUE< HVIO_CarInfoBegin, HVIO_INFOBEGIN_COUNT > s_queCarInfoBegin;
    static G_QUEUE< HVIO_CarInfoEnd, HVIO_INFOEND_COUNT > s_queCarInfoEnd;
    static G_QUEUE< HVIO_CarInLeft, HVIO_INLEFT_COUNT > s_queCarInLeft;
    static G_QUEUE< HVIO_CarPlate, HVIO_PLATE_COUNT > s_queCarPlate;
    static G_QUEUE< HVIO_BigImage, HVIO_BIGIMG_COUNT > s_queBigImage;
    static G_QUEUE< HVIO_SmallImage, HVIO_SMALLIMG_COUNT > s_queSmallImage;
    static G_QUEUE< HVIO_Video, HVIO_VIDEO_COUNT > s_queVideo;
    static G_QUEUE< HVIO_String, HVIO_STR_COUNT > s_queString;

protected:
    Q_STYPE< SENDSTYLE, MAX_TYPE_COUNT > m_queStype;  //发送类型队列

    DWORD32 m_dwCurCarID;  //当前发送的记录集车辆ID

    //过滤条件
    WORD16 m_pAllowVideoID[MAX_STREAMID_COUNT];
    WORD16 m_pAllowStreamID[MAX_STREAMID_COUNT];
    WORD16 m_wStreamCount;

    //心跳数据包
    HVIO_Throb m_hvioThrob;

public:
    CDataLinkThread( HV_SOCKET_HANDLE hSocket )
            : m_dwCurCarID( 0 )
            , m_wStreamCount( 0 )
            , m_hSocket(hSocket)
            , m_fSafeEnable(false)
            , m_pcSafeSaver(NULL)
            , m_pcSafeSaver_Dm6467(NULL)
    {
        //Comment by Shaorg: 屏蔽SIGPIPE异常
        //在用gdb调试运行时，如果对方终止连接，而本方如果还继续向已终止的连接发送数据的话，
        //则会发生该异常。
        //在Dm6467上调试时发现，如果注释掉这段，
        //则当HvSetup断开与Dm6467的连接时，VvdHprApp.exe将会退出。
        sigset_t signal_mask;
        sigemptyset(&signal_mask);
        sigaddset(&signal_mask, SIGPIPE);
        int rc = pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
        if (rc != 0)
        {
            HV_Trace(1, "pthread_sigmask is error.\n");
        }

        memset(m_pAllowVideoID, 0, sizeof(m_pAllowVideoID));
        memset(m_pAllowStreamID, 0, sizeof(m_pAllowStreamID));

        if ( CreateSemaphore(&m_hSemResult, 0, MAX_TYPE_COUNT) != 0 )
        {
            HV_Trace(1, "\nDataLink Thread Create SemResult Failed!");
        }

        if ( CreateSemaphore(&m_hSemStreamID, 1, 1) != 0 )
        {
            HV_Trace(1, "\nDataLink Thread Create SemStreamID Failed!");
        }

        m_hvioThrob.m_dwLen = 10;
        m_hvioThrob.m_wType = _TYPE_THROB;
        m_hvioThrob.m_wVideoID = 0;
        if ( m_hSocket != INVALID_SOCKET )
        {
            HvGetPeerName(m_hSocket, &m_hvioThrob.m_dwIp, &m_hvioThrob.m_wPort);
        }
        else
        {
            m_hvioThrob.m_dwIp = 0;
            m_hvioThrob.m_wPort = 0;
        }

        m_dwLastThreadIsOkTime = GetSystemTick();
    };

    ~CDataLinkThread()
    {
        DestroySemaphore(&m_hSemResult);
        DestroySemaphore(&m_hSemStreamID);

        if ( INVALID_SOCKET != m_hSocket )
        {
            CloseSocket(m_hSocket);
            m_hSocket = INVALID_SOCKET;
        }

        SAFE_DELETE(m_pcSafeSaver_Dm6467);
        m_pcSafeSaver = NULL;
    };

protected:
    //检查是否匹配
    HRESULT CheckLink(
        DWORD32 dwIP,
        WORD16 wPort
    );

    //判断是否符合过滤条件
    HRESULT CheckStreamID(
        WORD16 wVideoID,
        WORD16 wStreamID
    );

    BOOL IsFree()
    {
        return m_fExit;
    };

    //处理发送队列
    HRESULT ProcessQueue();

protected:
    HV_SOCKET_HANDLE m_hSocket;
    HV_SEM_HANDLE m_hSemResult;
    HV_SEM_HANDLE m_hSemStreamID;

    //安全连接方式相关
    bool m_fSafeEnable;
    DWORD32 m_dwTimeLow;
    DWORD32 m_dwTimeHigh;
    int m_SS_RecodeIndex;

    ISafeSaver* m_pcSafeSaver;
    CSafeSaverDm6467Impl* m_pcSafeSaver_Dm6467;

    DWORD32 m_dwLastThreadIsOkTime;
};

//-----------------------------------------------------------------------

class CDataLinkEnum : public CHvEnumBase<CDataLinkThread*>
{
protected:
    HRESULT OnDelNode(NODE* pNode)
    {
        SAFE_DELETE(pNode->Dat);
        return S_OK;
    };
};

//-----------------------------------------------------------------------

class CDataLinkCtrlThread : public CHvThreadBase
                           , public IResultIO
{
public:
    //用户接口：发送车辆信息开始接收标志
    STDMETHOD(SendInfoBegin)(
        const int& index,
        DWORD32 dwFlag = 0
    );

    //用户接口：发送车辆信息结束接收标志
    STDMETHOD(SendInfoEnd)(
        const int& index,
        DWORD32 dwFlag = 0
    );

    //用户接口：发送车辆到达离开标志
    STDMETHOD(SendInLeftFlag)(
        const int& index,
        DWORD32 dwFlag = 0
    );

    //用户接口：发送车牌
    STDMETHOD(SendCarPlate)(
        const int& index,
        DWORD32 dwFlag = 0
    );

    //用户接口：发送车牌附加信息
    STDMETHOD(SendAppendInfo)(
        const int& index,
        DWORD32 dwFlag
    );

    //用户接口：发送车辆YUV大图
    STDMETHOD(SendBigImage)(
        const int& index,
        DWORD32 dwFlag = 0
    );

    //发送YUV小图
    STDMETHOD(SendSmallImageYUV)(
        const int& index,
        DWORD32 dwFlag = 0
    );

    //发送二值化小图
    STDMETHOD(SendBinaryImage)(
        const int& index,
        DWORD32 dwFlag = 0
    );

    //发送视频
    STDMETHOD(SendVideoYUV)(
        const int& index,
        DWORD32* pdwSendCount,
        DWORD32 dwFlag = 0
    );

    //用户接口：发送字符流
    STDMETHOD(SendString)(
        const int& index,
        DWORD32 dwFlag = 0
    );

    //用户接口：发送二进制流
    STDMETHOD(SendBin)(
        const int& index,
        DWORD32 dwFlag = 0
    );

    //设置过滤条件
    STDMETHOD(SetStreamID)(
        DWORD32 dwIP,
        WORD16 wPort,
        WORD16 wVideoID,
        WORD16 wStreamID
    );

    //删除过滤条件
    STDMETHOD(DeleteStreamID)(
        DWORD32 dwIP,
        WORD16 wPort,
        WORD16 wVideoID,
        WORD16 wStreamID
    );

    //清除所有过滤条件
    STDMETHOD(ClearAllStreamID)(
        DWORD32 dwIP,
        WORD16 wPort
    );

    //设置安全传送模式
    STDMETHOD(SetSafeMode)(
        DWORD32 dwIP,
        WORD16 wPort,
        DWORD32 dwTimeLow,
        DWORD32 dwTimeHigh,
        int index,
        int iPageIndex = -1,
        int iPageSize   = 0
    );

    //查询AVI文件
    STDMETHOD(GetAviByTime)(
        DWORD32 dwIP,
        WORD16 wPort,
        DWORD32 dwTimeLow,
        DWORD32 dwTimeHigh,
        DWORD32 *pdwAviTimeLow,
        DWORD32 *pdwAviTimeHigh,
        DWORD32 *pdwAviLen
    );

    //查询硬盘录像
    STDMETHOD(GetHistoryVideo)(
        DWORD32 dwIP,
        WORD16 wPort,
        DWORD32 dwTimeLow,
        DWORD32 dwTimeHigh,
        DWORD32 dwTimeSliceS
    );

public:
    virtual const char* GetName()
    {
        static char szName[] =  "CDataLinkCtrlThread";
        return szName;
    }
    virtual HRESULT Run( void* pvParam );

public:
    CDataLinkCtrlThread()
            : m_fInitialized(FALSE)
            , m_hSocket(INVALID_SOCKET)
    {
        m_dwLastThreadIsOkTime = GetSystemTick();
    };

    ~CDataLinkCtrlThread()
    {
        Stop(-1);

        DestroySemaphore(&m_hSemDataLink);

        if ( INVALID_SOCKET != m_hSocket )
        {
            CloseSocket(m_hSocket);
            m_hSocket = INVALID_SOCKET;
        }
    };

    HRESULT Initialize()
    {
        if ( 0 != CreateSemaphore(&m_hSemDataLink, 1, 1) )
        {
            return E_FAIL;
        }

        m_iClientStatus = 0;

        m_fInitialized = TRUE;
        return S_OK;
    };

    int GetClientStatus();

    bool ThreadIsOk();

protected:
    HRESULT EnumLock();
    HRESULT EnumUnLock();

protected:
    BOOL m_fInitialized;
    int m_iClientStatus;	            // 客户端连接状态，只要有一个连接就算正常

    CDataLinkEnum m_cDataLinkEnum;
    HV_SEM_HANDLE m_hSemDataLink;
    HV_SOCKET_HANDLE m_hSocket;

    DWORD32 m_dwLastThreadIsOkTime;     // 运行状态标志，控制喂狗操作
};

#endif
