// 该文件编码格式必须是WIN936

// camerarecordlink.h
// 定义识别结果及其它信息传输接口

#ifndef _CAMERARECORDLINK_H_
#define _CAMERARECORDLINK_H_

#include "config.h"
#include "CameraLinkBase.h"
#include "HvCameraLink.h"
#include "hvenum.h"
#include "tinyxml.h"

#include <list>


#define MAX_IP_LEN 64


class CCameraRecordLink;
// 历史结果处理线程
class CHistoryRecordProcess : public CHvThreadBase
{
public:
    // CHvThreadBase
    virtual const char* GetName()
    {
        static char szName[] = "CHistoryRecordProcess";
        return szName;
    }
    virtual HRESULT Run(void* pvParam);

    CHistoryRecordProcess(CCameraRecordLink* pRecordLink);
    ~CHistoryRecordProcess();

    HRESULT GetRefData(
        CCameraDataReference** ppRefRecord,
        CCameraData** ppCameraRecord,
        DWORD32 dwInfoSize,
        DWORD32 dwDataSize
    );

    BOOL ThreadIsOk();
    BOOL IsEndOfRecord();

private:
    CCameraRecordLink* m_pRecordLink;
    DWORD32 m_dwThreadIsOkMs;
    BOOL m_fIsEndOfRecord;
    static const int MAX_WAIT_MS = 200000;
};


//识别结果连接类
class CCameraRecordLink : public CHvThreadBase
{
public:
    // CHvThreadBase
    virtual const char* GetName()
    {
        static char szName[] = "CCameraRecordLink";
        return szName;
    }
    virtual HRESULT Run(void* pvParam);
    bool ThreadIsOk();

public:
    CCameraRecordLink(HV_SOCKET_HANDLE hSocket, SEND_RECORD_PARAM& cSendRecordParam);
    ~CCameraRecordLink();

    HRESULT SendRecord(CCameraDataReference* pRefRecord);
    bool IsSendHistoryPlate();
    virtual bool IsRecordListEmpty();

    DWORD32 GetStatus()
    {
        return m_dwThreadStatus;
    }
    HRESULT SetSocket(HV_SOCKET_HANDLE& hSocket);

protected:
    HRESULT DisConnect();
    // 初始化参数,连接时才确定的参数。
    HRESULT Init();
    HRESULT ProcessCmd(HvCore::IHvStream* pSocketStream);
    HRESULT ProcessHistroyPlate(HvCore::IHvStream* pSocketStream);

    bool IsSafeSaverEnable();
    int RecordOutputType();
    HRESULT GetCCameraDataReference(CCameraDataReference** pRefRecord);
    HRESULT ClearCCameraData();

private:

    HRESULT ProcessXMLInfo(unsigned char* pbInfo, const DWORD32& dwInfoSize);
    HRESULT DownloadRecordXML(const TiXmlElement* pCmdArgElement);

protected:
    CAMERA_INFO_HEADER m_cameraThrob;
    CAMERA_INFO_HEADER m_cameraThrobResponse;

    SEND_RECORD_PARAM& m_cSendRecordParam;
    char    m_szDevList[256];   //指定接收某些设备的结果
    DWORD32 m_dwDataInfo;       //指定接收某种信息的数据，比如违章
    bool m_fSendHistroyEnable;

    HV_SEM_HANDLE m_hSemRecord;
    HV_SEM_HANDLE m_hSemListRecord;  //Add by Shaorg: 为了m_lstRecord变量的线程安全
    std::list<CCameraDataReference*> m_lstRecord;
    // 连接当前状态: 0 : 挂起，1： 正常运行
    DWORD32 m_dwThreadStatus;

    CAMERA_INFO_HEADER		m_cHistoryEnd;    //历史结果结束标志，下发客户端

    HvCore::IHvStream* m_pSocketStream;
    bool m_fSendHistoryEnable;
    DWORD32 m_dwConnectedLogId;

    CHistoryRecordProcess* m_pHistoryPro;
    friend class CHistoryRecordProcess;

    //断点续传相关
    DWORD32 m_dwInfoOffset;
    DWORD32 m_dwDataOffset;

#ifdef SINGLE_BOARD_PLATFORM
    static const int MAX_CAMERA_RECORD_COUNT = 8;
#else
    static const int MAX_CAMERA_RECORD_COUNT = 2;
#endif

private:
    HV_SOCKET_HANDLE m_hSocket;

    static const int MAX_WAIT_MS = 200000;
    DWORD32 m_dwThreadIsOkMs;

    //可靠性连接方式相关
    DWORD32 m_dwSendTimeLow;
    DWORD32 m_dwSendTimeHigh;
    DWORD32 m_dwEndSendTimeLow;
    DWORD32 m_dwEndSendTimeHigh;
    int m_iRecordIndex;
};

//识别结果主动链接类
class CCameraRecordAutoLink : public CCameraRecordLink
{
public:
    virtual const char* GetName()
    {
        static char szName[] = "CCameraRecordAutoLink";
        return szName;
    }
    virtual HRESULT Run(void* pvParam);
public:
    CCameraRecordAutoLink(HV_SOCKET_HANDLE hSocket,
                        SEND_RECORD_PARAM& cSendRecordParam ,
                        char* pServerIp ,
                        WORD16 wPort);
    ~CCameraRecordAutoLink();
     bool IsConnect(void){return m_fConnected;}
private:
    HRESULT SetConnectionRequst();
    virtual bool IsRecordListEmpty();
private:
    HV_SOCKET_HANDLE m_hAutoLinkSocket;
    HvCore::IHvStream* m_pAutoLinkSocketStream;

    DWORD32 m_dwAutoConnectedLogId;

    WORD16 m_wAutoLinkPort;
    char m_szAutoLinkIP[MAX_IP_LEN];
    bool   m_fConnected;
};

// 识别结果连接枚举类
class CCameraRecordLinkEnum : public CHvEnumBase<CCameraRecordLink*>
{
protected:
    HRESULT OnDelNode(NODE* pNode)
    {
        SAFE_DELETE(pNode->Dat);
        return S_OK;
    }
};

// 识别结果连接控制类
class CCameraRecordLinkCtrl : public HiVideo::ISendRecord
            , public CCameraLinkCtrlBase
{
public:
    // ISendRecord
    virtual HRESULT SendRecord(const SEND_RECORD* pRecord, DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh);
    virtual HRESULT SetParam(SEND_RECORD_PARAM* pParam);
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes)
    {
        return (true == ThreadIsOk()) ? S_OK : E_FAIL;
    };

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

    // 启动主动连接
    virtual HRESULT StartAutoLink();
    // 停止主动连接
    virtual HRESULT StopAutoLink();

    bool ThreadIsOk();

	int GetRealTimeLinkCount();

public:
    CCameraRecordLinkCtrl();
    ~CCameraRecordLinkCtrl();

    HRESULT MakeNode(
        const DWORD32& dwID,
        const DWORD32& dwNodeLen,
        const unsigned char* pbNode,
        unsigned char* pbInfo
    );

    HRESULT SetAutoLink(bool fAutoLinkEnable ,char* pAutoLinkIP , WORD16 wAutoLinkPort);
    HRESULT EnumLock();
    HRESULT EnumUnLock();

private:
    static const int MAX_LINK_COUNT = 2;
    CCameraRecordLinkEnum m_cCameraRecordLinkEnum;
    HV_SEM_HANDLE m_hSemEnum;

    CCameraRecordAutoLink* m_cCameraRecordAutoLink;

    static const int MAX_WAIT_MS = 200000;
    SEND_RECORD_PARAM m_cSendRecordParam;

    bool m_fResumeBrokenTransfer;

    WORD16 m_wAutoLinkPort;
    char m_szAutoLinkIP[MAX_IP_LEN];
};

#endif
