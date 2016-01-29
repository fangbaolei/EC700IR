// 该文件编码格式必须为WINDOWS-936格式

/**
* @file	ImgGatherer.h
* @version	1.0
* @brief 图像采集模块接口定义
*/

#ifndef _IMG_GATHERER_H_
#define _IMG_GATHERER_H_

#include "hvutils.h"
#include "swimageobj.h"
#include "HvList.h"
#include "hvthreadbase.h"
#include "hvsocket.h"
#include "trackerdef.h"
#include "SocketStream.h"
#include "CamyuLinkOpt.h"
#include "HvSockUtils.h"
#include "OuterControlImpl_Linux.h"
#include "CameraController.h"
#include "CamTransmitBase.h"

const int IMG_LIST_COUNT = 2;
const int MAX_IMG_RECV_COUNT = 20;
const int MAX_CAM_COUNT = 8;

// 服务器端口号
#define SVR_PORT 8888
// 图片名字长度
#define IMG_NAME_LEN 128
// 获取图片命令
#define GET_IMG_CMD 0x0101

/**
*  视频帧数据结构
*/
typedef struct _tagImgFrame
{
    IReferenceComponentImage* pRefImage;
    int iVideoID;
    char szCamIp[32];

    DWORD32 dwFrameType;  //帧类型（暂时供H.264传输使用）
    FpgaRegInfo cFpgaRegInfo;
    AgcAwbInfo cAgcAwbInfo;
    HV_COMPONENT_IMAGE imgVPIF;
    DWORD32 dwTimeTick;

    BOOL fSplitUV;
    BOOL fRotateY;
    BOOL fUseImgVPIF;

    _tagImgFrame()
    {
        memset(this, 0, sizeof(*this));
    }

} IMG_FRAME;

/**
* @brief 前端图像采集模块参数结构体
*/
typedef struct _tagImgFrameParam
{
    int iCamNum;                            //相机个数
    char rgstrCamIP[MAX_CAM_COUNT][32];     //抓拍相机IP
    char rgstrHVCParm[MAX_CAM_COUNT][128];  //抓拍识别参数

    float fltCaptureFrameRate; //采集帧率
    int iOutputFrameRate;   // 输出帧率（1～15）

    _tagImgFrameParam()
    {
        iCamNum = 1;
        for ( int i=0; i<MAX_CAM_COUNT; ++i )
        {
            memset(rgstrCamIP[i], 0, 32);
            memset(rgstrHVCParm[i], 0, 128);
        }
        fltCaptureFrameRate = 12.5;
        iOutputFrameRate = 15;
    };

} IMG_FRAME_PARAM;

const int MAX_CAP_CAM_COUNT = 4;

/**
* @brief 抓拍相机模块参数结构体
*/
typedef struct _tagCapCamParam
{
    int nCamType;   //抓拍相机类型(0-创宇J系列相机)
    int nCamNum;    //抓拍相机个数
    int rgnSignalType[MAX_CAP_CAM_COUNT];   //抓拍相机信号源类型
    char rgszRoadIP[MAX_CAP_CAM_COUNT][20]; //抓拍相机IP

    _tagCapCamParam()
    {
        nCamType = 0;
        nCamNum = 0;
        for (int i = 0; i < MAX_CAP_CAM_COUNT; i++)
        {
            rgnSignalType[i] = -1;
            sprintf(rgszRoadIP[i], "172.18.10.10%d", i);
        }
    }
}CAP_CAM_PARAM;

#define MAX_LEVEL_COUNT 14
/**
* @brief 前端相机参数结构体
*/
typedef struct _tagCamCfgParam
{
    char szIP[100];
    int iCamType; // 摄像机协议类型：0-一体化相机协议，1-专用测试协议，2-Camyu摄像机协议
    int iDynamicCfgEnable;
    int iDynamicTriggerEnable;
    int iTriggerDelay;
    int iTriggerPlateType; //卡口车尾模式下，哪些车牌类型需要触发抓拍，安通杰需求，黄国超增加，2012-01-04
    int iDynamicCfgEnableOnTrigger;

    // 摄像机接收缓冲帧数
    int iJpegBufferCount;
    int nTestProtocolPort;

    //自动填充参数
    int iAutoParamEnable;
    float fltDB;				//步长
    int iMinExposureTime;
    int iMinGain;
    int iMaxExposureTime;
    int iMaxGain;
    int iEnableAGC;
    int iMaxAGCLimit;
    int iMinAGCLimit;

    int irgExposureTime[MAX_LEVEL_COUNT];
    int irgAGCLimit[MAX_LEVEL_COUNT];
    int irgGain[MAX_LEVEL_COUNT];
    int irgBrightness[MAX_LEVEL_COUNT];

    char rgszMode[MAX_LEVEL_COUNT][10];

    _tagCamCfgParam()
    {
        sprintf(szIP, "192.168.1.10");
        iCamType = 0;
        iDynamicCfgEnable = 0;
        iDynamicTriggerEnable =0;
        iTriggerDelay = 0;
        iTriggerPlateType = 0;
        iDynamicCfgEnableOnTrigger = 0;
        iJpegBufferCount = 1;
        iEnableAGC = 0;
        iAutoParamEnable = 0;
        fltDB = 3.0;
        iMinExposureTime  = 100;
        iMinGain  = 60;
        iMaxExposureTime  = 6000;
        iMaxGain  = 220;
        iMaxAGCLimit = 140;
        iMinAGCLimit = 10;
        nTestProtocolPort = SVR_PORT;

        int irgExposureTimeTemp[MAX_LEVEL_COUNT] =
        {
            8, 8, 8, 8, 12, 17, 24, 34, 48, 68, 96, 96, 96, 96
        };

        int irgBrightnessTemp[MAX_LEVEL_COUNT] =
        {
            -75, -50, -25, 0, 0, 0, 0, 0, 0, 0, 0, 25, 50, 75
        };

        for (int i = 0; i < MAX_LEVEL_COUNT; i++)
        {
            irgExposureTime[i] = irgExposureTimeTemp[i];
            irgAGCLimit[i] = -1;
            irgGain[i] = -1;
            irgBrightness[i] = irgBrightnessTemp[i];
            strcpy(rgszMode[i], "快速");
        }
    }
}CAM_CFG_PARAM;

// 图片广播端口号
#define SVR_BROADCAST_PORT 8989
#define RCV_TIMEOUT 2000			// 接收超时

/**
*  CBroadcastLink接口类
*/
class IBroadcastLink
{
public:
    virtual ~IBroadcastLink(){};
    // 获取一帧图片
    virtual HRESULT GetImage(char* pcBuf, DWORD32* pdwSize, DWORD32* pdwTime, char* pszFileName) = 0;
    // 初始化
    virtual HRESULT Initialize() = 0;
};

/**
*  CBroadcastLink实现类
*/
class CBroadcastLink : public IBroadcastLink
{
public:
    CBroadcastLink();
    virtual ~CBroadcastLink();
    virtual HRESULT GetImage(char* pcBuf, DWORD32* pdwSize, DWORD32* pdwTime, char* pszFileName);
    virtual HRESULT Initialize();
private:
    HV_SOCKET_HANDLE m_sktData;
    CSocketStream* m_pstmData;
    DWORD32 m_dwRefTime;
};

#define TFTP_RRQ 0x0001		// 读请求报文
#define TFTP_WRQ 0x0002		// 写请求报文
#define TFTP_DATA 0x0003		// 数据传送报文
#define TFTP_ACK 0x0004		// 确认报文
#define TFTP_ERROR 0x0005	// 错误报文
#define TFTP_OACK 0x0006		// 选项报文
#define TFTP_UNKOWN 0x00FF	// 未知报文
#define SRV_PORT 69		// TFTP服务端口

// TFTP协议客户端
class CTftpPtl
{
public:
    CTftpPtl();
    ~CTftpPtl();
    HRESULT Connect(const char *pszAddress);
    HRESULT SendRRQ(const char *pszFileName, const char *pszMode, const char *pszBlkSize, const char *pszSize, int iTimeout);
    HRESULT SendWRQ(const char *pszFileName, const char *pszMode, const char *pszBlkSize, const char *pszSize, int iTimeout);
    HRESULT SendDATA(WORD16 wBlock, char *pcBuf, int iSize, int iTimeout);
    HRESULT SendACK(WORD16 wBlock, int iTimeout);
    HRESULT SendERROR();
    HRESULT DisConnect();
    HRESULT RecvData(char *pcBuf, int ilen, int *piRevLen, int iTimeout);
private:
    HV_SOCKET_HANDLE m_sktData;
    CSocketStream* m_pstmData;
    WORD16 m_wBlockSize;
    DWORD32 m_dwRemoteAddr;
    WORD16 m_wRemotePort;
};

/**
*  CNVCLink实现类
*/
#define RCV_BEGIN_TIMEOUT_NVC 100	// 接收开始超时
#define RCV_TIMEOUT_NVC 500		// 接收超时
#define MAX_RECV_SIZE_NVC 2048	// UDP最大包长度

const DWORD32 JPEG_HEAD_SIZE = 595;

class CNVCLink
{
public:
    CNVCLink();
    ~CNVCLink();
    HRESULT Connect(const char* pszIP);
    HRESULT DisConnect();
    HRESULT ReadRegValue(BYTE8 bAddr, WORD16& wValue);
    HRESULT WriteRegValue(BYTE8 bAddr, WORD16 wValue);
    HRESULT GetImageHeight(DWORD32& dwHeight);
    HRESULT GetImageWidth(DWORD32& dwWidth);
    HRESULT GetDevVer(WORD16& wDevVer);
    HRESULT GetDevType(WORD16& wDevType);
    HRESULT SaveConfigForever();
    HRESULT RestoreDefaultConfig();
    HRESULT SetImgInfo(WORD16 wWidth = 2048, WORD16 wHeight = 1536);
    HRESULT SetQuality(WORD16 wQuality = 11);
    HRESULT GetJpgImge(char* pcBuf, DWORD32* pdwSize, DWORD32* pdwTime, WORD16 wRes = 1);
protected:
    HRESULT GetHeightFromCamera(WORD16& wHeight);
    HRESULT GetWidthFromCamera(WORD16& wWidth);
private:
    CTftpPtl m_tftp_ptl;
    BYTE8* m_pbRevBuf;
    int m_iQuality;
    WORD16 m_wWidth;
    WORD16 m_wHeight;
    BYTE8 m_rgbJpegHeader[JPEG_HEAD_SIZE];
};

const int MAX_RECV_BUF_SIZE = 1024 * 1024;

/**
*  CCamyuLink接口类
*/
class ICamyuLink
{
public:
    virtual ~ICamyuLink(){};
    // 获取图片开始
    virtual HRESULT GetImageStart(const char* pszIP) = 0;
    // 获取图片开始Ex，调用了该函数就不会调用GetImageStart
    virtual HRESULT GetImageStartEx(const char* pszIP, int iPort) = 0;
    // 获取图片结束
    virtual HRESULT GetImageStop() = 0;
    // 获取一帧图片
    virtual HRESULT GetImage(char* pcBuf, DWORD32* pdwSize, DWORD32* pdwRefTime) = 0;
    // 控制相机开始
    virtual HRESULT CtrtCamStart(const char* pszIP) = 0;
    // 控制相机结束
    virtual HRESULT CtrtCamStop() = 0;
    // 发送命令至相机
    virtual HRESULT SendCommand(DWORD32 dwCmdNo, DWORD32 dwParam1 = 0, DWORD32 dwParam2 = 0) = 0;
};

/**
*  CCamyuLink实现类
*/
class CCamyuLink : public ICamyuLink
{
public:
    CCamyuLink();
    virtual ~CCamyuLink();
    // 获取图片开始
    virtual HRESULT GetImageStart(const char* pszIP);
    // 获取图片开始Ex，调用了该函数就不会调用GetImageStart
    virtual HRESULT GetImageStartEx(const char* pszIP, int iPort);
    // 获取图片结束
    virtual HRESULT GetImageStop();
    // 获取一帧图片
    virtual HRESULT GetImage(char* pcBuf, DWORD32* pdwSize, DWORD32* pdwRefTime);
    // 控制相机开始
    virtual HRESULT CtrtCamStart(const char* pszIP);
    // 控制相机结束
    virtual HRESULT CtrtCamStop();
    // 发送命令至相机
    virtual HRESULT SendCommand(DWORD32 dwCmdNo, DWORD32 dwParam1 = 0, DWORD32 dwParam2 = 0);

public:
    //获取当前车道
    int GetCurRoadNum();
    // 在不接收相机数据的情况下,定时调用该函数来保持连接
    HRESULT ProcessData();
    // 获取当前帧类型，指定大华摄像机码流I帧或P帧
    int GetCurFrameType();

    HRESULT GetImageWidth(DWORD32& dwWidth);
    HRESULT GetImageHeight(DWORD32& dwHeight);
    HRESULT SetDataSktTimeout(DWORD32 dwTimeout);
    HRESULT SetCmdSktTimeout(DWORD32 dwTimeout);

private:
    HRESULT GetOneFrame(char* pcBuf, DWORD32* pdwSize, bool fMustData);
    HV_SOCKET_HANDLE m_sktData;
    HV_SOCKET_HANDLE m_sktCmd;
    CSocketStream* m_pstmData;
    CSocketStream* m_pstmCmd;
    char* m_pcTmpData;
    int m_iCurFrameType; // 大华摄像机码流类型
    int m_iRoadNum;		//当前车道号
    DWORD32 m_dwHeight;
    DWORD32 m_dwWidth;
};

//
// 数据流总信息头
typedef struct tag_head
{
    DWORD32 dwType;
    DWORD32 dwInfoLen;
    DWORD32 dwDataLen;
}
INFO_HEADER_HVCAM;

// 数据流扩展信息头
typedef struct tag_block
{
    DWORD32 dwID;
    DWORD32 dwLen;
}
BLOCK_HEADER_HVCAM;

// 命令包头
typedef struct tag_CameraCmdHeader
{
    DWORD32 dwID;
    DWORD32 dwInfoSize;
}
CAMERA_CMD_HEADER_HVCAM;

// 命令回应包
typedef struct tag_CameraCmdRespond
{
    DWORD32 dwID;
    DWORD32 dwInfoSize;
    int dwResult;
}
CAMERA_CMD_RESPOND_HVCAM;
//

/**
*  CHvCameraLink接口类
*/
class IHvCameraLink
{
public:
    virtual ~IHvCameraLink(){};
    // 获取图片开始
    virtual HRESULT GetImageStart(const char* pszIP) = 0;
    // 获取图片结束
    virtual HRESULT GetImageStop() = 0;
    // 获取一帧图片
    virtual HRESULT GetImage(char* pcBuf, DWORD32* pdwSize, DWORD32* pdwRefTime, DWORD32* pdwImgType) = 0;
    // 控制相机开始
    virtual HRESULT CtrtCamStart(const char* pszIP) = 0;
    // 控制相机结束
    virtual HRESULT CtrtCamStop() = 0;
    // 发送命令至相机
    virtual HRESULT SendCommand( LPCSTR szCmd ) = 0;
};

/**
*  CHvCameraLink实现类
*/
class CHvCameraLink:public IHvCameraLink
{
public:
    CHvCameraLink();
    virtual ~CHvCameraLink();
    // 获取图片开始
    virtual HRESULT GetImageStart(const char* pszIP);
    // 获取图片结束
    virtual HRESULT GetImageStop();
    // 获取一帧图片
    virtual HRESULT GetImage(char* pcBuf, DWORD32* pdwSize, DWORD32* pdwRefTime, DWORD32* pdwImgType);
    // 控制相机开始
    virtual HRESULT CtrtCamStart(const char* pszIP);
    // 控制相机结束
    virtual HRESULT CtrtCamStop();
    // 发送命令至相机
    virtual HRESULT SendCommand( LPCSTR szCmd );

public:
    //获取当前车道
    int GetCurRoadNum();
    // 在不接收相机数据的情况下,定时调用该函数来保持连接
    HRESULT ProcessData();

    HRESULT GetImageWidth(DWORD32& dwWidth);
    HRESULT GetImageHeight(DWORD32& dwHeight);
    HRESULT SetDataSktTimeout(DWORD32 dwTimeout);
    HRESULT SetCmdSktTimeout(DWORD32 dwTimeout);
    HRESULT SendImageCmd(char* pszCmd);

private:
    HRESULT GetOneFrame(char* pcBuf, DWORD32* pdwSize, bool fMustData, DWORD32* pdwImgType);
    HV_SOCKET_HANDLE m_sktData;
    HV_SOCKET_HANDLE m_sktCmd;
    CSocketStream* m_pstmData;
    CSocketStream* m_pstmCmd;
    char* m_pcTmpData;
    int m_iCurFrameType;
    DWORD32 m_dwHeight;
    DWORD32 m_dwWidth;
};

/**
*  触发抓拍相机接口类
*/
class ICamTrigger
{
public:
    virtual ~ICamTrigger(void){};
    virtual HRESULT Trigger(LIGHT_TYPE nLightType, int iRoadNum = 0) = 0;
    virtual HRESULT HardTrigger(int iRoadNum) = 0;
    virtual HRESULT SetCamTriggerParam(CAM_CFG_PARAM *pCfgCamParam) = 0;
};

/**
*  触发抓拍相机实现类
*/
class CHVTrigger : public ICamTrigger, public CHvThreadBase
{
public:
    CHVTrigger();
    virtual ~CHVTrigger(void);
    virtual const char* GetName()
    {
        static char szName[] = "CHVTrigger";
        return szName;
    }
    virtual HRESULT Run(void* pvParam);
    virtual HRESULT Trigger(LIGHT_TYPE nLightType, int iRoadNum = 0);
    virtual HRESULT HardTrigger(int iRoadNum);
    virtual HRESULT SetCamTriggerParam(CAM_CFG_PARAM *pCfgCamParam);
    HRESULT ProcessOnTrigger();
    bool ThreadIsOk();

private:
    int m_iCurRoadNum;

    DWORD32 m_dwLastThreadIsOkTime;
    LIGHT_TYPE m_nCurLightType;
    CHvCameraLink m_cHvCameraLink;
    CAM_CFG_PARAM *m_pCfgCamParam;
    HV_SEM_HANDLE m_hSemNeedTrigger;
};

/**
*  触发抓拍相机实现类
*/
class CCamTrigger : public ICamTrigger, public CHvThreadBase
{
public:
    CCamTrigger();
    virtual ~CCamTrigger(void);
    virtual const char* GetName()
    {
        static char szName[] = "CCamTrigger";
        return szName;
    }
    virtual HRESULT Run(void* pvParam);
    virtual HRESULT Trigger(LIGHT_TYPE nLightType, int iRoadNum = 0);
    virtual HRESULT HardTrigger(int iRoadNum);
    virtual HRESULT SetCamTriggerParam(CAM_CFG_PARAM *pCfgCamParam);
    HRESULT ProcessOnTrigger();
    bool ThreadIsOk();

private:
    int m_iCurRoadNum;

    DWORD32 m_dwLastThreadIsOkTime;
    LIGHT_TYPE m_nCurLightType;
    CCamyuLink m_cCamyuLink;
    CAM_CFG_PARAM *m_pCfgCamParam;
    HV_SEM_HANDLE m_hSemNeedTrigger;
};

/**
*  IImgGatherer接口
*/
class IImgGatherer
{
public:
	IImgGatherer():m_pOnImage(NULL),m_pContext(NULL){}
    virtual ~IImgGatherer(){};
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes) = 0;
    virtual HRESULT SetImgFrameParam(const IMG_FRAME_PARAM& cParam) = 0;
    virtual HRESULT SetCamCfgParam(LPVOID pCfgCamParam) = 0;
    virtual HRESULT SetLightType(LIGHT_TYPE cLightType, int iCplStatus) = 0;
    virtual HRESULT Play(LPVOID lpParam) = 0;
    virtual HRESULT SetImageEddyType(int nEddyType) = 0;
    virtual HRESULT MountTransmiter(ICamTransmit** ppCamTransmit) = 0;
    virtual BOOL IsCamConnected() = 0;

public:
	void RegisterImageCallbackFunction(void (*OnImage)(void* pContext, IMG_FRAME imgFrame), void* pContext)
	{
		m_pOnImage = OnImage;
		m_pContext = pContext;
	}
	virtual HRESULT PutOneFrame(IMG_FRAME imgFrame)
	{
		if (m_pOnImage)
		{
			m_pOnImage(m_pContext, imgFrame);
			return S_OK;
		}
		return E_FAIL;
	}

private:
	void (*m_pOnImage)(void* pContext, IMG_FRAME imgFrame);
	void* m_pContext;
};

/**
*  抓拍图像接收类
*/
class CCamyuReceiver : public CHvThreadBase
{
public:
    CCamyuReceiver(void* pParent, HV_SOCKET_HANDLE hSock);
    ~CCamyuReceiver(void);

    // CHvThreadBase Interface
    virtual const char* GetName()
    {
        static char szName[] = "CCamyuReceiver";
        return szName;
    }
    virtual HRESULT Run(void* pvParam);

private:
    HV_SOCKET_HANDLE m_hSock;
    void* m_pParent;
    IMG_FRAME_PARAM m_cImgFrameParam;
};

/**
*  抓拍图像侦听类
*/
class CCamyuListener : public CHvThreadBase
{
public:
    CCamyuListener(void* pParent);
    ~CCamyuListener(void);
    // CHvThreadBase Interface
    virtual const char* GetName()
    {
        static char szName[] =  "CCamyuListener";
        return szName;
    }
    virtual HRESULT Run(void* pvParam);
    bool ThreadIsOk();

public:
    BOOL m_fIsCamConnected;

private:
    CCamyuReceiver* m_rgpCamyuReceiver[MAX_IMG_RECV_COUNT];
    void* m_pParent;
    int m_iStat;
    int m_iStatTimes;
};

/**
*  CImgCapturer实现类
*/
class CImgCapturer : public IImgGatherer
{
public:
    CImgCapturer();
    virtual ~CImgCapturer();
    // IImgGatherer接口
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes);
    virtual HRESULT SetImgFrameParam(const IMG_FRAME_PARAM& cParam);
    virtual HRESULT SetLightType(LIGHT_TYPE cLightType, int iCplStatus)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT SetCamCfgParam(LPVOID pParam)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT MountTransmiter(ICamTransmit** ppCamTransmit)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT Play(LPVOID lpParam)
    {
        return m_pCamyuListener->Start(NULL);
    }
    virtual HRESULT SetImageEddyType(int nEddyType)
    {
        m_nEddyType = nEddyType;
        return S_OK;
    }
    virtual BOOL IsCamConnected()
    {
        if (m_pCamyuListener)
        {
            return m_pCamyuListener->m_fIsCamConnected;
        }
        return FALSE;
    }

public:
    HRESULT SetCapCamParam(
        int nEddyType,
        CAP_CAM_PARAM *pCapCamParam,
        COuterControlImpl *pSignalMatch
    );
private:
    CCamyuListener* m_pCamyuListener;
    IMG_FRAME_PARAM m_cImgFrameParam;
    CAP_CAM_PARAM *m_pCapCamParam;
    COuterControlImpl *m_pSignalMatch;
    int m_nEddyType;

    friend class CCamyuReceiver;
};

/**
*  CVideoGetter_Nvc实现类
*/
class CVideoGetter_Nvc : public IImgGatherer, public CHvThreadBase
{
public:
    CVideoGetter_Nvc();
    virtual ~CVideoGetter_Nvc();
    // IImgGatherer接口
    virtual HRESULT SetCamCfgParam(LPVOID pCfgCamParam);
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes);
    virtual HRESULT SetLightType(LIGHT_TYPE cLightType, int iCplStatus);
    virtual HRESULT SetImgFrameParam(const IMG_FRAME_PARAM& cParam)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT Play(LPVOID lpParam)
    {
        return Start(lpParam);
    }
    virtual HRESULT SetImageEddyType(int nEddyType)
    {
        m_nEddyType = nEddyType;
        return S_OK;
    }
    virtual HRESULT MountTransmiter(ICamTransmit** ppCamTransmit)
    {
        m_ppCamTransmit = ppCamTransmit;
        return S_OK;
    }
    virtual BOOL IsCamConnected()
    {
        return m_fIsCamConnected;
    }

    HRESULT SetCapCamCfgParam(LPVOID pCfgCamParam);
    HRESULT CamSetLight(WORD16 wValue);
    HRESULT CamSetMode(WORD16 wValue, WORD16 wTime);
    HRESULT MountSignalMatch(COuterControlImpl *pSignalMatch)
    {
        if (!pSignalMatch)
        {
            return E_POINTER;
        }
        m_pSignalMatch = pSignalMatch;
        return S_OK;
    }

public:
    //CHvThreadBase
    virtual const char* GetName()
    {
        static char szName[] =  "CVideoGetter_Nvc";
        return szName;
    }
    virtual HRESULT Run(void*pvParam);

public:
    bool SetCamParameter();

private:
    CNVCLink m_cNvcLink;
    CAM_CFG_PARAM *m_pCfgCamParam;
    CAP_CAM_PARAM *m_pCapCamParam;
    COuterControlImpl *m_pSignalMatch;
    DWORD32 m_dwLastTime;
    DWORD32 m_dwFrameCount;
    LIGHT_TYPE m_cPlateLightType;
    int m_nEddyType;
    bool m_fIsLightTypeChanged;
    bool m_bActive;
    BOOL m_fIsCamConnected;
    ICamTransmit** m_ppCamTransmit;
};

/**
*  CVideoGetter_Camyu实现类
*/
class CVideoGetter_Camyu : public IImgGatherer, public CHvThreadBase
{
public:
    CVideoGetter_Camyu();
    virtual ~CVideoGetter_Camyu();
    // IImgGatherer接口
    virtual HRESULT SetCamCfgParam(LPVOID pCfgCamParam);
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes);
    virtual HRESULT SetLightType(LIGHT_TYPE cLightType, int iCplStatus);
    virtual HRESULT SetImgFrameParam(const IMG_FRAME_PARAM& cParam)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT Play(LPVOID lpParam)
    {
        return Start(lpParam);
    }
    virtual HRESULT SetImageEddyType(int nEddyType)
    {
        m_nEddyType = nEddyType;
        return S_OK;
    }
    virtual HRESULT MountTransmiter(ICamTransmit** ppCamTransmit)
    {
        m_ppCamTransmit = ppCamTransmit;
        return S_OK;
    }
    virtual BOOL IsCamConnected()
    {
        return m_fIsCamConnected;
    }

    HRESULT SetCapCamCfgParam(LPVOID pCfgCamParam);
    HRESULT MountSignalMatch(COuterControlImpl *pSignalMatch)
    {
        if (!pSignalMatch)
        {
            return E_POINTER;
        }
        m_pSignalMatch = pSignalMatch;
        return S_OK;
    }

public:
    //CHvThreadBase
    virtual const char* GetName()
    {
        static char szName[] =  "CVideoGetter_Camyu";
        return szName;
    }
    virtual HRESULT Run(void*pvParam);

public:
    bool SetCamParameter();

private:
    CCamyuLink m_cCamyuLink;
    CAM_CFG_PARAM *m_pCfgCamParam;
    CAP_CAM_PARAM *m_pCapCamParam;
    COuterControlImpl *m_pSignalMatch;
    DWORD32 m_dwLastTime;
    DWORD32 m_dwFrameCount;
    LIGHT_TYPE m_cPlateLightType;
    int m_nEddyType;
    bool m_fIsLightTypeChanged;
    bool m_bActive;
    BOOL m_fIsCamConnected;
    ICamTransmit** m_ppCamTransmit;
};

/**
*  CVideoGetter_HvCamer实现类
*/
class CVideoGetter_HvCamera : public IImgGatherer, public CHvThreadBase
{
public:
    CVideoGetter_HvCamera();
    virtual ~CVideoGetter_HvCamera();
    // IImgGatherer接口
    virtual HRESULT SetCamCfgParam(LPVOID pCfgCamParam);
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes);
    virtual HRESULT SetLightType(LIGHT_TYPE cLightType, int iCplStatus);
    virtual HRESULT SetImgFrameParam(const IMG_FRAME_PARAM& cParam)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT Play(LPVOID lpParam)
    {
        return Start(lpParam);
    }
    virtual HRESULT SetImageEddyType(int nEddyType)
    {
        m_nEddyType = nEddyType;
        return S_OK;
    }
    virtual HRESULT MountTransmiter(ICamTransmit** ppCamTransmit)
    {
        m_ppCamTransmit = ppCamTransmit;
        return S_OK;
    }
    virtual BOOL IsCamConnected()
    {
        return m_fIsCamConnected;
    }

    HRESULT SetCapCamCfgParam(LPVOID pCfgCamParam);
    HRESULT MountSignalMatch(COuterControlImpl *pSignalMatch)
    {
        if (!pSignalMatch)
        {
            return E_POINTER;
        }
        m_pSignalMatch = pSignalMatch;
        return S_OK;
    }
    HRESULT SetCaptureMode(LPVOID pCfgCamParam);

public:
    //CHvThreadBase
    virtual const char* GetName()
    {
        static char szName[] =  "CVideoGetter_HvCamera";
        return szName;
    }
    virtual HRESULT Run(void*pvParam);
    HRESULT RunCommMode();
    HRESULT RunCaptureMode();

public:
    bool SetCamParameter();

private:
    CHvCameraLink m_cHvCameraLink;
    CAM_CFG_PARAM *m_pCfgCamParam;
    CAP_CAM_PARAM *m_pCapCamParam;
    COuterControlImpl *m_pSignalMatch;
    DWORD32 m_dwLastTime;
    DWORD32 m_dwFrameCount;
    LIGHT_TYPE m_cPlateLightType;
    int m_nEddyType;
    bool m_fIsLightTypeChanged;
    bool m_bActive;
    BOOL m_fIsCamConnected;
    ICamTransmit** m_ppCamTransmit;
    bool m_fCaptureMode;
};

/**
*  CVideoGetter_Camyu实现类
*/
class CVideoGetter_Broadcast : public IImgGatherer, public CHvThreadBase
{
public:
    CVideoGetter_Broadcast();
    virtual ~CVideoGetter_Broadcast();
    // IImgGatherer接口
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes);
    virtual HRESULT Play(LPVOID lpParam)
    {
        return Start(lpParam);
    }
    virtual HRESULT SetImageEddyType(int nEddyType)
    {
        m_nEddyType = nEddyType;
        return S_OK;
    }
    virtual HRESULT SetLightType(LIGHT_TYPE cLightType, int iCplStatus)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT SetCamCfgParam(LPVOID pCfgCamParam)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT SetImgFrameParam(const IMG_FRAME_PARAM& cParam)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT MountTransmiter(ICamTransmit** ppCamTransmit)
    {
        return E_NOTIMPL;
    }
    virtual BOOL IsCamConnected()
    {
        return TRUE;
    }
public:
    //CHvThreadBase
    virtual const char* GetName()
    {
        static char szName[] =  "CVideoGetter_Broadcast";
        return szName;
    }
    virtual HRESULT Run(void*pvParam);

private:
    CBroadcastLink m_cBroadcastLink;
    DWORD32 m_dwLastTime;
    DWORD32 m_dwFrameCount;
    int m_nEddyType;
};

//////////////////////////////////////////////////////////////////////////
// 发送图片数据格式
// | 图片时标 | 图片名字 |  图片长度 | 图片数据 |
// | 4 BYTE |128 BYTE | 4 BYTE  | n BYTE   |
//////////////////////////////////////////////////////////////////////////

/**
*  测试协议相机实现类
*/
class CTestLink
{
public:
    CTestLink();
    ~CTestLink();
    HRESULT Connect(const char* pszIP, WORD16 wPort);
    HRESULT GetImage(
        unsigned char* pcBuf,
        DWORD32* pdwSize,
        DWORD32* pdwTime,
        char* pszFileName
    );
    HRESULT Close();
    BOOL IsConnect();

private:
    HV_SOCKET_HANDLE m_hSocket;
    CSocketStream* m_pStream;
    DWORD32 m_dwRefTime;
    BOOL m_fConnectFlag;
};

/**
*  CVideoGetter_Test实现类
*/
class CVideoGetter_Test : public IImgGatherer, public CHvThreadBase
{
public:
    CVideoGetter_Test();
    virtual ~CVideoGetter_Test();
    // IImgGatherer接口
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes);
    virtual HRESULT SetCamCfgParam(LPVOID pCfgCamParam);
    virtual HRESULT SetLightType(LIGHT_TYPE cLightType, int iCplStatus)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT SetImgFrameParam(const IMG_FRAME_PARAM& cParam)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT MountTransmiter(ICamTransmit** ppCamTransmit)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT Play(LPVOID lpParam)
    {
        return Start(lpParam);
    }
    virtual HRESULT SetImageEddyType(int nEddyType)
    {
        m_nEddyType = nEddyType;
        return S_OK;
    }
    virtual BOOL IsCamConnected()
    {
        return m_fIsCamConnected;
    }
public:
    //CHvThreadBase
    virtual const char* GetName()
    {
        static char szName[] =  "CVideoGetter_Test";
        return szName;
    }
    virtual HRESULT Run(void*pvParam);

private:
    CTestLink m_cTestLink;
    CAM_CFG_PARAM *m_pCfgCamParam;
    LIGHT_TYPE m_cPlateLightType;

    DWORD32 m_dwLastTime;
    DWORD32 m_dwImgWidth;
    DWORD32 m_dwImgHeight;
    DWORD32 m_dwFrameCount;

    int m_nEddyType;

    bool m_fIsLightTypeChanged;
    BOOL m_fIsCamConnected;
};

/**
*  CVideoGetter_Pci实现类
*/
class CVideoGetter_Pci : public IImgGatherer, public CHvThreadBase
{
public:
    CVideoGetter_Pci();
    virtual ~CVideoGetter_Pci();

    HRESULT PutOneFrame(IMG_FRAME imgFrame);
    // IImgGatherer接口
    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes)
    {
        DWORD32 dwCurTime = GetSystemTick();
        if (dwCurTime < m_dwLastTime)
        {
            return S_OK;
        }
        if (dwCurTime - m_dwLastTime > 20000 && m_dwLastTime > 0)
        {
            HV_Trace(5, "current time = %d, last time = %d, escape = %d\n", dwCurTime, m_dwLastTime, dwCurTime - m_dwLastTime);
            return E_FAIL;
        }
        return S_OK;
    }
    virtual HRESULT SetLightType(LIGHT_TYPE cLightType, int iCplStatus)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT SetImgFrameParam(const IMG_FRAME_PARAM& cParam)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT SetCamCfgParam(LPVOID pCfgCamParam)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT MountTransmiter(ICamTransmit** ppCamTransmit)
    {
        return E_NOTIMPL;
    }
    virtual HRESULT Play(LPVOID lpParam)
    {
        return Start(lpParam);
    }
    virtual HRESULT SetImageEddyType(int nEddyType)
    {
        m_nEddyType = nEddyType;
        return S_OK;
    }
    virtual BOOL IsCamConnected()
    {
        return TRUE;
    }

public:
    //CHvThreadBase
    virtual const char* GetName()
    {
        static char szName[] =  "CVideoGetter_PCI";
        return szName;
    }
    virtual HRESULT Run(void*pvParam)
    {
        while (!m_fExit)
        {
            m_dwLastTime = GetSystemTick();
            HV_Sleep(5000);
        }
        return S_OK;
    }

private:
    DWORD32 m_dwLastTime;
    int m_nEddyType;
};

#endif
