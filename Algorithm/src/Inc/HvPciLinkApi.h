/*
 *	(C)版权所有 2011 北京信路威科技发展有限公司
 */

/**
* @file		HvPciLinkApi.h
* @version	1.0
* @brief	PCILink相关API
* @author	Shaorg
* @date		2011-3-25
*/

#ifndef _HVPCILINKAPI_H_
#define _HVPCILINKAPI_H_

#include "hvutils.h"
#include "hvthreadbase.h"
#include "slw_pci.h"
#include "CameraRecordLink.h"
#include "safesaver.h"
#include "ImgGatherer.h"
#include "IPT.h"

#include "TrafficLightParam.h"
#include "AutoLinkParam.h"
#include "HvSockUtils.h"
#include "SocketStream.h"
#include "RadarBase.h"
#include "CSRIRadar.h"
#include <linux/netlink.h>
#include "ResultSenderImpl.h"
#include "HvcPanoramicCapturer.h"

// 主从CPU之间发送状态信息的周期
#define CPU_THROB_PERIOD 8000

// 主从CPU之间任何一方在这段时间内未收到对方的任何有效信息时即认为对方异常
#define CPU_THROB_TIMEOUT 20000

// 数据长度
extern const int PCILINK_DATA_MAX_LEN;
extern const int PCILINK_STATE_INFO_LEN;
extern const int PCILINK_RESULT_INFO_LEN;
extern const int PCILINK_CLOCK_SYNC_DATA_LEN;
extern const int PCILINK_PARAM_DATA_LEN;

// PCI数据ID集
typedef enum
{
    // 通用PCI数据类型
    PCILINK_UNKNOW_DATA = 0xFFFF0000            // 未知数据
    , PCILINK_RETURN_VALUE = 0x00005000         // 返回结果
    , PCILINK_HV_EXIT = 0x00006000              // 退出程序

    // “从CPU”发往“主CPU”的PCI数据类型
    , PCILINK_SLAVE_CPU_STATE = 0x00001001      // 从CPU状态
    , PCILINK_RECOGNITION_RESULT = 0x00001002   // 识别结果数据
    , PCILINK_CAMERA_TYPE = 0x00001003          // 前端类型
    , PCILINK_SET_AGC_STATE = 0x00001004        // 设置AGC开关
    , PCILINK_SET_AWB_STATE = 0x00001005        // 设置AWB开关
    , PCILINK_SEND_STRING = 0x00001006          // 发送字符串到客户端
    , PCILINK_SEND_INLEFT_INFO = 0x00001007     // 发送车辆到达离开标识到客户端
    , PCILINK_SET_CAM_DYN_PARAM = 0x00001008    // 动态改变相机参数
    , PCILINK_DEBUG_SLAVE_IMAGE = 0x00001009    // 调试从端图片
    , PCILINK_RESET_HV = 0x00001010             // 复位设备
    , PCILINK_PCICOM = 0x00001011               // 串口数据
    , PCILINK_PCIIPTPARAM = 0x00001012          // IPT配置参数
    , PCILINK_OUTERDEVICE_TYPE = 0x00001013     // 外部设备类型
    , PCILINK_SLAVE_LOG = 0x00001014            // 从端的日志信息
    , PCILINK_DEVSTATUS_STRING = 0x00001015     // 从端设备状态字符串
	, PCILINK_RESULT_PARAM = 0x00001016         // 从端发送结果发送信息给主去配置发送结果参数
	, PCILINK_FLASHLIGHT_TYPE = 0x00001017      // 闪光灯工作模式:0-不闪;1-硬触发;2-软触发

    // “主CPU”发往“从CPU”的PCI数据类型
    , PCILINK_CLOCK_SYNC_DATA = 0x00002001      // 时钟同步
    , PCILINK_PARAM_SET_DATA = 0x00002002       // 设置xml参数
    , PCILINK_PARAM_GET_DATA = 0x00002003       // 获取xml参数
    , PCILINK_GET_CAMERA_PARAM = 0x00002004     // 获取相机参数，类型：0一体化相机，1测试协议，2创宇协议
    , PCILINK_ONE_FRAME_DATA = 0x00002005       // 一帧图片数据
    , PCILINK_GET_WORKMODEINDEX = 0x00002006    // 获取指定模式索引的模式名
    , PCILINK_GET_WORKMODE = 0x00002007         // 获取当前模式索引
    , PCILINK_GET_WORKMODE_COUNT = 0x00002008   // 获取模式个数
    , PCILINK_SET_WORKMODE = 0x00002009         // 设置工作模式
    , PCILINK_SHAKE_HANDS = 0x00002010          // PCI协议上层握手
    , PCILINK_SEND_SLAVE_IMAGE = 0x00002011     // 发送“调试从端图片”
    , PCILINK_FORCESEND = 0x00002012            // 强制出图
    , PCILINK_SET_JPEG_CR = 0x00002013          // JPEG压缩率
    , PCILINK_SET_CAPTURE_CR = 0x00002014       // 抓拍图压缩率
    , PCILINK_SET_DEVICE_STATE = 0x00002015     // 设备状态
    , PCILINK_PCIIPT = 0x00002016               // IPT事件信息
    , PCILINK_GET_SLAVE_DEBUG_INFO = 0x00002017 // 取从黑匣子信息
    , PCILINK_GET_SLAVE_RESET_LOG = 0x00002018  // 取从复位记录
    , PCILINK_MASTER_CPU_STATE = 0x00002019     // 主CPU状态
    , PCILINK_SENE_PANNEL_PARAM = 0x00002020    // 控制板类型
    , PCILINK_GET_AUTO_LINK_PARAM = 0x00002021  // 索取主动连接参数
    , PCILINK_SET_CAPTURE_SYNSIGNAL = 0x00002022 //设置抓拍信号输出
    , PCILINK_GET_TRAFFIC_LIGHT_PARAM = 0x00002022 // 索取灯组信息
    , PCILINK_GET_PANORAMIC_CAMARA_PARAM = 0x00002023 //获取全景相机参数
    , PCILINK_SEND_PANORAMIC_IMAGE = 0x00002024 //发送全景抓拍图
    , PCILINK_SET_JPEG_TYPE = 0x00002025
    , PCILINK_DATACTRL_SHAKE_HANDS = 0x00002026 // DataCtrl握手
}
PCILINK_DATA_TYPE;

// 动态更改的参数类型
//typedef enum _DCP_TYPE
//{
//    DCP_SEND_TYPE = 0,
//    DCP_CAM_OUTPUT,
//    DCP_ENABLE_AGC,
//    DCP_ENABLE_AWB,
//    DCP_AGC_TH,
//    DCP_SHUTTER,
//    DCP_GAIN,
//    DCP_JPEG_EXPECT_SIZE,
//    DCP_JPEG_COMPRESS_RATE,
//    DCP_JPEG_COMPRESS_RATE_CAPTURE,
//    DCP_FLASH_RATE_SYN_SIGNAL_ENABLE,
//    DCP_CAPTURE_SYN_SIGNAL_ENABLE,
//    DCP_CPL,
//    DCP_SETPULSEWIDTH
//} DCP_TYPE;

typedef struct tag_One_Frame_Info
{
    HV_COMPONENT_IMAGE imgJpeg;
    DWORD32 dwRefTime;
    char szFrameName[IMG_NAME_LEN];
}
ONE_FRAME_INFO;

typedef struct tag_Panoramic_Image_Frame
{
    PANORAMIC_CAPTURER_FRAME_INFO cImageHeader;
    PBYTE8  pImageData;

}PANORAMIC_IMAGE;

// PCI图片类型
typedef enum
{
    PCILINK_IMAGE_BEST_SNAPSHOT = 0,
    PCILINK_IMAGE_LAST_SNAPSHOT,
    PCILINK_IMAGE_BEGIN_CAPTURE,
    PCILINK_IMAGE_BEST_CAPTURE,
    PCILINK_IMAGE_LAST_CAPTURE,
    PCILINK_IMAGE_SMALL_IMAGE,
    PCILINK_IMAGE_BIN_IMAGE
}
PCILINK_IMAGE_TYPE;

// 图片信息结构体
typedef struct tag_PciImage_Info
{
    DWORD32 dwCarID;
    DWORD32 dwImgType;
    DWORD32 dwImgWidth;
    DWORD32 dwImgHeight;
    DWORD32 dwTimeLow;
    DWORD32 dwTimeHigh;
    DWORD32 dwEddyType;
    HV_RECT rcPlate;
    HV_RECT rcRedLightPos[20];
    int nRedLightCount;
    HV_RECT rcFacePos[20];
    int nFaceCount;
    tag_PciImage_Info()
    {
        memset(this, 0, sizeof(*this));
    }
}
PCI_IMAGE_INFO;

// 从CPU状态信息结构体
typedef struct tag_SlaveCpu_Status
{
    DWORD32 dwTimeLow;
    DWORD32 dwTimeHigh;
    int nStatusID;
    char szErrorText[32];
}
SLAVE_CPU_STATUS;

// 主CPU状态信息结构体
typedef struct tag_MasterCpu_Status
{
    int iReserve;
}
MASTER_CPU_STATUS;

//结果字符串信息结构体
typedef struct tag_PciString_Info
{
    WORD16 wVideoID;
    WORD16 wStreamID;
    DWORD32 dwTimeLow;
    DWORD32 dwTimeHigh;
    char szString[16 * 1024];
}
PCI_STRING_INFO;

//相机动态参数值
typedef struct tag_PciDynParam_Info
{
    DCP_TYPE eType;
    int nValue;
}
PCI_DYNPARAM_INFO;

//用来保存存放返回值数据指针
typedef struct tag_PciReturnData_Info
{
    LPVOID lpReturnData;
    int *pnDataSize;
    int nTimeOut;
}
PCI_RETURNDATA_INFO;

//视频流车牌红框信息结构体
typedef struct tag_PciPlateRect_Info
{
    DWORD32 dwImageOffset;
    unsigned char rgbRectInfo[1024];
    tag_PciPlateRect_Info()
    {
        memset(this, 0, sizeof(*this));
    }
}
PCI_PLATERECT_INFO;

typedef struct tag_PciCmdHead_Info
{
    PCILINK_DATA_TYPE emDataType;
    DWORD32 dwDataSize;
    DWORD32 dwCmdID;
}
PCI_CMDHEAD_INFO;

struct packet_info
{
	struct nlmsghdr hdr;
	int index;
};

typedef struct tag_PciBufAppend_Info
{
    DWORD32 dwRSize;
    DWORD32 dwWSize;
    DWORD32 dwCmdID;
    DWORD32 dwSemStatus;    // hSemaphore当前状态，0-未使用，1-pend状态，2-超时
    HV_SEM_HANDLE hLock;
    HV_SEM_HANDLE hSemaphore;
    tag_PciBufAppend_Info()
    {
        memset(this, 0, sizeof(tag_PciBufAppend_Info));
    }
}
PCI_BUF_APPEND_INFO;

#define MAX_CMDINFO_COUNT   10
class IOuterControler;
class CPciDataRecvThreadSlave;
typedef struct _tag_PciParamSlave
{
    CAM_CFG_PARAM *pCfgCamParam;
    IImgGatherer *pVideoGetter;
    IOuterControler * pOuterControler;
    AUTO_LINK_PARAM *pAutoLinkParam;
    TRAFFICLIGHT_CFG_PARAM *pTrafficCfgParam;
    PANORAMIC_CAPTURER_PARAM cPanoramicCamaraParam;

    _tag_PciParamSlave()
    {
        pCfgCamParam = NULL;
        pVideoGetter = NULL;
        pOuterControler = NULL;
        pAutoLinkParam = NULL;
        pTrafficCfgParam = NULL;
    }

}
PCI_PARAM_SLAVE;
/**
*  PCI命令处理类（供从CPU使用）
*/
class CPciCmdProcessSlave : public CHvThreadBase
{
public:
    CPciCmdProcessSlave();
    ~CPciCmdProcessSlave();

public:
    virtual const char* GetName()
    {
        static char szName[] = "CPciCmdProcessSlave";
        return szName;
    }

    virtual HRESULT Run(void* pvParam);  // CHvThreadBase

    HRESULT PutOneCmd(int nPciHandle);
    HRESULT IsThreadOk();

private:
    HRESULT GetOneCmd(int& nPciHandle);
    HRESULT ProcessCmd(int nPciHandle);

private:
    HiVideo::CHvList<int> m_queCmdHandle;
    HV_SEM_HANDLE m_hSemQueCount;
    HV_SEM_HANDLE m_hSemQueCtrl;
    DWORD32 m_dwLastTime;
    BYTE8 *m_pbRecvData;
    BOOL m_fShakeHandsSucceed;
    BOOL m_fDataCtrlHandsSucceed;
    PCI_PARAM_SLAVE m_cPciParam;
    friend class CPciDataRecvThreadSlave;
};

/**
*  PCI数据接收控制类（供从CPU使用）
*/
class CPciDataRecvThreadSlave : public CHvThreadBase
{
public:
    CPciDataRecvThreadSlave();
    ~CPciDataRecvThreadSlave();

public:
    virtual const char* GetName()
    {
        static char szName[] = "CPciDataRecvThreadSlave";
        return szName;
    }

    virtual HRESULT Run(void* pvParam);  // CHvThreadBase

    void SetPciParam(PCI_PARAM_SLAVE cPciParam);
    BOOL PciLinkShakeHands();
    BOOL DataCtrlHandshake();
    HRESULT GetCurStatus(char* pszStatus, int nStatusSizes);

private:
    DWORD32 m_dwLastTime;

    CPciCmdProcessSlave m_cPciCmdProcess;
};

class CPciDataRecvThreadMaster;
typedef struct _tag_PciParamMaster
{
    ISendCameraImage *pImageLink;
    ISendRecord *pRecordLink;
    ISafeSaver* pSafeSaver;
	ISendCameraVideo* pVideoLink;
    _tag_PciParamMaster()
    {
        memset(this, 0, sizeof(_tag_PciParamMaster));
    }
}
PCI_PARAM_MASTER;
/**
*  PCI命令处理类（供主CPU使用）
*/
class CPciCmdProcessMaster : public CHvThreadBase
{
public:
    CPciCmdProcessMaster();
    ~CPciCmdProcessMaster();

public:
    virtual const char* GetName()
    {
        static char szName[] = "CPciCmdProcessMaster";
        return szName;
    }

    virtual HRESULT Start(void *pvParamter);
    virtual HRESULT Run(void* pvParam);  // CHvThreadBase

    HRESULT PutOneCmd(int nHandle);
    HRESULT IsThreadOk();

private:
    HRESULT GetOneCmd(int& nPciHandle);
    HRESULT ProcessCmd(int nPciHandle);
    HRESULT PciSendString(PCI_STRING_INFO* pcStringInfo);

private:
    HiVideo::CHvList<int> m_queCmdHandle;
    HV_SEM_HANDLE m_hSemQueCount;
    HV_SEM_HANDLE m_hSemQueCtrl;
    DWORD32 m_dwLastTime;
    BOOL m_fSendRecord;
    PCI_PARAM_MASTER m_cPciParam;
    CIPT m_IPT;
    CRadarBase *m_pRadarBase;
    friend class CPciDataRecvThreadMaster;
	ResultSenderParam* m_pResultSenderParam;

	int m_nTriggerType;
};

/**
*  PCI数据接收控制类（供主CPU使用）
*/
class CPciDataRecvThreadMaster : public CHvThreadBase
{
public:
    CPciDataRecvThreadMaster();
    ~CPciDataRecvThreadMaster();

public:
    virtual const char* GetName()
    {
        static char szName[] = "CPciDataRecvThreadMaster";
        return szName;
    }
    virtual HRESULT Run(void *pvParamter);  // CHvThreadBase
    HRESULT GetCurStatus(char* pszStatus, int nStatusSizes);

public:
    void SetPciParam(PCI_PARAM_MASTER cPciParam);
    void EnableRecordSend();
    void DisableRecordSend();

private:
    DWORD32 m_dwLastTime;
    CPciCmdProcessMaster m_cPciCmdProcess;
};

#define MAX_PCI_BUF_COUNT   16

/**
* PCILink函数提供类，注意：一个系统中只能定义一个该对象
*/
class CHvPciLinkApi
{
public:
    CHvPciLinkApi();
    ~CHvPciLinkApi();

public:
    /**
    * @brief 初始化PCILink。
    */
    HRESULT Init();

    /**
    * @brief 卸载PCILink。
    */
    HRESULT Uninit();

    /**
    * @brief 通过PCI总线接收数据。
    */
    HRESULT BeginRecvData(int nPciHandle, PCI_CMDHEAD_INFO* pCmdHeadInfo);
    HRESULT RecvData(int nPciHandle, LPVOID pbBuf, DWORD32 dwDataSize);
    HRESULT EndRecvData(int nPciHandle, PCI_CMDHEAD_INFO cCmdHeadInfo);
    HRESULT WriteReturnValue(int nPciHandle, LPVOID pbBuf, DWORD32 dwDataSize);

    /**
    * @brief 通过PCI总线发送数据。
    */
    HRESULT SendData(
        PCILINK_DATA_TYPE emDataType,
        LPVOID pbBuf,
        DWORD32 dwLen,
        LPVOID lpReturnData = NULL,
        int *pnDataSize = 0,
        int nTimeOut = 6000
    );

    //直接将数据COPY至PCI共享内存，提高识别结果发送效率(主要用于识别结果发送)
    HRESULT BeginSendData(int* pnPciHandle, PCILINK_DATA_TYPE emDataType);
    HRESULT PutSendData(int nPciHandle, LPVOID pbBuf, DWORD32 dwLen);
    HRESULT EndSendData(
        int nPciHandle,
        LPVOID lpReturnData = NULL,
        int *pnDataSize = 0,
        int nTimeOut = 1000
        );

    /**
    * @brief 设置PCI上层握手状态
    */
    void SetShakeHandsStatus(bool fShakeHandsFinish);

    /**
    * @brief 接收PCI消息
    */
    HRESULT RecvMsg(int* pnPciHandle, DWORD32 *pdwMsg);

    HRESULT GetReturnValue(int nPciHandle, LPVOID lpBuf, int* pnBufSize, int nTimeOut);
    HRESULT PutReturnValue(int nPciHandle);

private:
    HRESULT InitRWBuffer();
    HRESULT UnInitRWBuffer();
    HRESULT InitRecvSocket();
    HRESULT UnInitRecvSocket();
    HRESULT GetPciWriteHandle(int* pnPciHandle, int nDataSize, int nTryTimes);
    HRESULT PutPciWriteHandle(int nPciHandle);
    HRESULT FreePciWriteHandle(int nPciHandle);

    HRESULT GetPciReadHandle(int* pnPciHandle);
    HRESULT PutPciReadHandle(int nPciHandle);

    HRESULT SendToKernel(DWORD32 dwMsg);

    // 取数据头读写指针
    unsigned char* GetHeadPtr(int nPciHandle);
    // 取数据读写指针
    unsigned char* GetDataPtr(int nPciHandle);

    void LockSendData();
    void UnLockSendData();
    int LockWait(int nPciHandle);
    int UnLockWait(int nPciHandle);
    int PendWait(int nPciHandle, int nTimeOut);
    int PostWait(int nPciHandle);

private:
    int m_fd;
    int m_nBufCount;
    PCI_CMDHEAD_INFO m_cCmdHeadInfo;
    swpci_buffer m_rgRWBufs[MAX_PCI_BUF_COUNT];
    PCI_BUF_APPEND_INFO m_rgApdInfo[MAX_PCI_BUF_COUNT];
    HV_SEM_HANDLE m_hSemSendData;
    bool m_fShakeHandsFinish;
    HV_SOCKET_HANDLE m_sktData;
    pid_t m_nProcID;
    DWORD32 m_dwCmdID;
};

extern CHvPciLinkApi g_cHvPciLinkApi;
extern int g_nJpegCompressRate;
extern int g_nCaptureCompressRate;
extern int g_nJpegType;
/**
* @brief 向从CPU发送时钟同步数据
*/
int PciSendClockSyncData(DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh);

/**
* @brief 向从CPU发送识别参数数据
*/
int PciSendParamSetData(PBYTE8 szXmlParam, int iLen);

/**
* @brief 向从CPU发送调试图片使能命令
*/
int PciSendSlaveImage(int iEnable);

/**
* @brief PCI握手命令
*/
int PciShakeHands();

/**
* @brief 向主CPU发送串口数据
*/
int PciSendComData(
    LPVOID pbBuf,
    DWORD32 dwLen,
    LPVOID lpReturnData = NULL,
    int *pnDataSize = 0,
    int nTimeOut = 3000
);

/**
* @brief 向从CPU发送JPEG压缩率
*/
int PciSetJpegCompressRate(int nRate);

/**
* @brief 向从CPU发送JPEG类型
*/
int PciSetJpegType(int nType);

/**
* @brief 向从CPU发送抓拍图压缩率
*/
int PciSetCaptureCompressRate(int nRate);

/**
* @brief 向从CPU发送设置状态命令
*/
int PciSetDeviceState(DWORD32 dwResetMode);

/**
* @brief 向主CPU发送当前从CPU的状态
*/
int PciSendSlaveCpuState(const SLAVE_CPU_STATUS* pcSlaveCpuStatus);

/**
* @brief 向从CPU发送当前主CPU的状态
*/
int PciSendMasterCpuState(const MASTER_CPU_STATUS* pcMasterCpuStatus);

/**
* @brief 向从CPU发送取黑匣子信息命令
*/
int PciGetSlaveDebugInfo(LPVOID lpReturnInfo, int *pnInfoLen);

/**
* @brief 向主CPU发送日志
*/
HRESULT PciSendSlaveDebugInfo(const char * szMsg,...);

/**
* @brief 向从CPU发送取ResetLog命令
*/
int PciGetResetLog(LPVOID lpReturnLog, int *pnLogLen);

/**
* @brief 通过PCI通知对方退出程序
*/
int PciSendHvExit(int iHvExitCode);

/**
* @brief 从CPU端初始化PCILink
*/
HRESULT StartPciLink(
    CPciDataRecvThreadSlave *pPciDataRecv,
    PCI_PARAM_SLAVE cPciParam
);
/**
* @brief 从CPU等待DataCtrl握手
*/
HRESULT DataCtrlHandshake(CPciDataRecvThreadSlave *pPciDataRecv);

/**
* @brief 主CPU发送DataCtrl握手
*/
HRESULT SendDataCtrlHandshake();

#endif
