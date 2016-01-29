// 该文件编码格式必须是WIN936

// cameralinkbase.h
//  连接控制基类

#ifndef _CAMERALINKBASE_H_
#define _CAMERALINKBASE_H_

#include "hvthreadbase.h"
#include "HvSockUtils.h"
#include "SocketStream.h"
#include "swimageobj.h"
#include <list>

class CCameraLinkCtrlBase : public CHvThreadBase
{
public:
    // CHvThreadBase
    virtual const char* GetName()
    {
        static char szName[] = "CCameraLinkCtrlBase";
        return szName;
    }
    virtual HRESULT Run(void* pvParam);

public:
    CCameraLinkCtrlBase(const WORD16 wPort, const int iMaxLinkCount);
    ~CCameraLinkCtrlBase();

public:
    // 以下函数子类继承实现

    // 获取当前连接个数
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
    // 设置相机类型
    virtual HRESULT SetCameraType(int iCameraType);

    HRESULT EnableAutoLink(bool fEnable);
    HRESULT EnableMTUSet(bool fEnable);

	// 设置可靠性保存方式 0:默认 1:没有连接时，才能存储
    virtual HRESULT SetSaveSafeType(const int iSaveSafeType);

protected:
    HV_SOCKET_HANDLE m_hSocket;
    WORD16 m_wPort;

    int m_iMaxLinkCount;
    BOOL m_fAutoLinkEnable;
    BOOL m_fMTUSetEnable;
    int m_iCameraType;
	int m_iSaveSafeType;

    DWORD32 m_dwThreadIsOkMs;
};

//--------------======================= HvReference ==================---------------------------

template<class T>
class CHvReferenceBase
{
public:
    CHvReferenceBase()
            : m_pData(NULL)
            , m_iRef(0)

    {
        if (CreateSemaphore(&m_hSemRef, 1, 1) != 0)
        {
            HV_Trace(5, "<CHvReferenceBase>CreateSemaphore Failed!\n");
        }
    }

    ~CHvReferenceBase()
    {
        DestroySemaphore(&m_hSemRef);

        if ( m_pData != NULL )
        {
            delete m_pData;
            m_pData = NULL;
        }
    }

public:
    HRESULT AddRef()
    {
        SemPend(&m_hSemRef);
        ++m_iRef;
        SemPost(&m_hSemRef);
        return S_OK;
    }

    HRESULT Release()
    {
        SemPend(&m_hSemRef);
        --m_iRef;
        if ( m_iRef <= 0 )
        {
            SemPost(&m_hSemRef);
            delete this;
            return S_OK;
        }
        SemPost(&m_hSemRef);
        return S_OK;
    }

    HRESULT Create()
    {
        if ( m_pData == NULL )
        {
            m_pData = new T;
            if ( m_pData != NULL )
            {
                m_iRef = 1;
            }
        }
        return m_pData == NULL ? E_FAIL : S_OK ;
    }

    T* GetItem()
    {
        return m_pData;
    }

private:
    T* m_pData;
    int m_iRef;
    HV_SEM_HANDLE m_hSemRef;
};


// 发送数据包头定义
typedef struct tag_cameraInfoHeader
{
    DWORD32 dwType;
    DWORD32 dwInfoSize;
    DWORD32 dwDataSize;
}
CAMERA_INFO_HEADER;

#ifndef SINGLE_BOARD_PLATFORM

/*CamApp's main.cpp*/
extern HV_SEM_HANDLE g_hSemCameraDataCount;

// 相机数据
class CCameraData
{
public:
    CAMERA_INFO_HEADER header;
    PBYTE8 pbInfo;
    PBYTE8 pbData;
    IReferenceComponentImage* pRefImage;
    IReferenceMemory* pRefMemory;

    static int s_iCount;

    static int GetCount()
    {
        int iCount;
        SemPend(&g_hSemCameraDataCount);
        iCount = s_iCount;
        SemPost(&g_hSemCameraDataCount);
        return iCount;
    }

    CCameraData()
    {
        memset(&header, 0, sizeof(header));
        pbInfo = NULL;
        pbData = NULL;
        // zhaopy
        pRefImage = NULL;
        pRefMemory = NULL;

        SemPend(&g_hSemCameraDataCount);
        ++s_iCount;
        SemPost(&g_hSemCameraDataCount);
    }

    ~CCameraData()
    {
        SAFE_DELETE_ARRAY(pbInfo);

        // zhaopy

        if ( pRefImage != NULL )
        {
            pRefImage->Release();
            pRefImage = NULL;
        }
        if ( pRefMemory != NULL )
        {
            pRefMemory->Release();
            pRefMemory = NULL;
        }

        SemPend(&g_hSemCameraDataCount);
        --s_iCount;
        SemPost(&g_hSemCameraDataCount);
    }
};

#else

// 相机数据
class CCameraData
{
public:
    CAMERA_INFO_HEADER header;
    PBYTE8 pbInfo;
    PBYTE8 pbData;
    IReferenceComponentImage* pRefImage;
    IReferenceMemory* pRefMemory;

    static int s_iCount;

    static int GetCount()
    {
        int iCount;
        iCount = s_iCount;
        return iCount;
    }

    CCameraData()
    {
        memset(&header, 0, sizeof(header));
        pbInfo = NULL;
        pbData = NULL;
        // zhaopy
        pRefImage = NULL;
        pRefMemory = NULL;

        ++s_iCount;
    }

    ~CCameraData()
    {
        SAFE_DELETE_ARRAY(pbInfo);

        // zhaopy

        if ( pRefImage != NULL )
        {
            pRefImage->Release();
            pRefImage = NULL;
        }
        if ( pRefMemory != NULL )
        {
            pRefMemory->Release();
            pRefMemory = NULL;
        }

        --s_iCount;
    }
};

#endif // SINGLE_BOARD_PLATFORM

// 发送数据引用类
class CCameraDataReference : public CHvReferenceBase<CCameraData>
{
public:
    // 图片引用的创建
    static HRESULT CreateCameraDataReference(CCameraDataReference** ppRefData);
};

//--------------=============== CameraConnectedLog ================--------------

#include "HvCameraLinkOpt.h"
#include "TcpipCfg.h"

/* ParseExecXmlExtCmd.cpp */
#define IVN_MAX_SIZE 128

typedef struct tagConnectedLog
{
    DWORD32 dwID;
    CAMERA_LINK_PORT eType;
    char szIp[16];
    short nPort;
} ConnectedLog;

// 相机连接日志类
class CameraConnectedLog
{
public:
    CameraConnectedLog();

public:
    HRESULT AddConnectedItem(DWORD32& dwLogId, HV_SOCKET_HANDLE hSocket, CAMERA_LINK_PORT t);
    HRESULT RemoveConnectedItem(DWORD32 dwLogId);
    int GetConnectedInfo(char rgszName[][IVN_MAX_SIZE], char rgszValue[][IVN_MAX_SIZE], int nArrayCount);

private:
    CHvList<ConnectedLog> m_listConnectedLog;
    DWORD32 m_dwLogId;
};

extern CameraConnectedLog g_cCameraConnectedLog;

extern void* GetNodeData(BYTE8* pInfoDat, int nInfoLen, DWORD32 dwID, int* pDatLen);

#endif
