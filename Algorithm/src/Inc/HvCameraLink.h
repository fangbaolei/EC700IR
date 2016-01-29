// 该文件编码格式必须是WIN936
//	hvCameralink.h
//	定义数据传输接口及相关数据结构
//
//

#ifndef _HVCAMERALINK_H_
#define _HVCAMERALINK_H_

#ifndef WIN32
#include "HvCameraLinkOpt.h"
#include "hvutils.h"
#include "safesaver.h"
#include "swimageobj.h"
#endif

namespace HiVideo
{
#ifndef WIN32
    // 数据结构定义

    //相机图片
    typedef struct tag_CameraImage
    {
        DWORD32 dwImageType;
        DWORD32 dwWidth;
        DWORD32 dwHeight;
        DWORD32 dwTimeLow;
        DWORD32 dwTimeHigh;
        DWORD32 dwImageOffset;
        ImageExtInfo cImageExtInfo;
        FpgaExtInfo cFpgaExtInfo;
        DWORD32 dwJpegCompressRate;
        DWORD32 dwY;
        DWORD32 dwImageSize;
        PBYTE8	pbImage;
        IReferenceComponentImage* pRefImage;
        IReferenceMemory* pRefMemory;           // zhaopy 接创宇相机模拟时用到
        tag_CameraImage()
        {
            memset(this, 0, sizeof(*this));
        }
    }
    SEND_CAMERA_IMAGE;

    //H.264
    typedef struct tag_CamVideo
    {
        DWORD32 dwVideoType;
        DWORD32 dwFrameType;
        DWORD32 dwTimeLow;
        DWORD32 dwTimeHigh;
        VideoExtInfo cVideoExtInfo;
        DWORD32 dwY;
        DWORD32 dwWidth;
        DWORD32 dwHeight;
        DWORD32 dwOutputFrameRate;
        DWORD32 dwVideoSize;
        PBYTE8 pbVideo;
        HV_RECT rcRedLight[20];
        float   fltFPS;
        IReferenceComponentImage* pRefImage;
        tag_CamVideo()
        {
            memset(this, 0, sizeof(*this));
        }
    }
    SEND_CAMERA_VIDEO;

    //识别结果
    typedef struct tag_Record
    {
        int iCurCarId; // 当前识别结果索引
        DWORD32 dwRecordType;
        PBYTE8 pbXML;
        DWORD32 dwXMLSize;
        PBYTE8 pbRecord;
        DWORD32 dwRecordSize;
        IReferenceMemory* pRefMemory;
    }
    SEND_RECORD;

    // 参数
    // 相机图片参数
    typedef struct tag_SendImageParam
    {
    }
    SEND_IMAGE_PARAM;

    // 视频流参数
    typedef struct tag_SendVideoParam
    {
        //可靠性保存模块
        ISafeSaver* pcSafeSaver;    /**< 安全储存器 */
        int iSendHisVideoSpace;    /**< 发送历史录像的间隔 */

        tag_SendVideoParam()
        {
            pcSafeSaver = NULL;
            iSendHisVideoSpace = 1;
        };
    }
    SEND_VIDEO_PARAM;

    // 识别结果发送参数
    typedef struct tag_SendRecordParam
    {
        //可靠性保存模块
        ISafeSaver* pcSafeSaver;    /**< 安全储存器 */
		int iOutputOnlyPeccancy;
		int iSendRecordSpace;
		int nDiskType;
        tag_SendRecordParam()
        {
            pcSafeSaver = NULL;
            iOutputOnlyPeccancy = 0;
            iSendRecordSpace = 5;
            nDiskType = 0;
        };
    }
    SEND_RECORD_PARAM;

    //接口定义

    //图片传输接口
    class ISendCameraImage
    {
    public:
        virtual ~ISendCameraImage() {};
        virtual HRESULT SendCameraImage(const SEND_CAMERA_IMAGE* pImage) = 0;
        virtual HRESULT SetParam(const SEND_IMAGE_PARAM* pParam) = 0;
        virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes) = 0;
        virtual HRESULT GetIsCaptureMode(BOOL* fIsCaptureLink) = 0;
    };

    //H.264传输接口
    class ISendCameraVideo
    {
    public:
        virtual ~ISendCameraVideo() {};
        virtual HRESULT SendCameraVideo(const SEND_CAMERA_VIDEO* pVideo) = 0;
        virtual HRESULT SetParam(const SEND_VIDEO_PARAM* pParam) = 0;
        virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes) = 0;
    };

    //识别结果传输接口
    class ISendRecord
    {
    public:
        virtual ~ISendRecord() {};
        virtual HRESULT SendRecord(const SEND_RECORD* pRecord, DWORD32 dwTimeMsLow, DWORD32 dwTimeMsHigh) = 0;
        virtual HRESULT SetParam(SEND_RECORD_PARAM* pParam) = 0;
        virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes) = 0;
    };

    // 身份认证接口
    class ICheckPassword
    {
    public:
        virtual ~ICheckPassword(){};
        virtual HRESULT SendCheckCmd() = 0;
        virtual HRESULT ReceivePassword() = 0;
    };

#else

    // 数据流总信息头
    typedef struct tag_head
    {
        DWORD32 dwType;
        DWORD32 dwInfoLen;
        DWORD32 dwDataLen;
    }
    INFO_HEADER;

    // 数据流扩展信息头
    typedef struct tag_block
    {
        DWORD32 dwID;
        DWORD32 dwLen;
    }
    BLOCK_HEADER;

#endif // WIN32

    // 命令连接

    // 命令包头
    typedef struct tag_CameraCmdHeader
    {
        DWORD32 dwID;
        DWORD32 dwInfoSize;
    }
    CAMERA_CMD_HEADER;

    // 命令回应包
    typedef struct tag_CameraCmdRespond
    {
        DWORD32 dwID;
        DWORD32 dwInfoSize;
        int dwResult;
    }
    CAMERA_CMD_RESPOND;

	//字符数据包
	typedef struct tag_CHAR_DATA
	{
		int nTopLeftX;      //字符左上角坐标X值
		int nTopLeftY;      //字符左上角坐标Y值
		int nSize;        //字符大小
		int nDateType;      //时间显示类型
		int nRGB;            //RGB值
	}
	CHAR_DATA;

#ifndef WIN32

    // 命令解析接口
    class ICameraCmdLink
    {
    public:
        virtual ~ICameraCmdLink(){};

        virtual HRESULT ReceiveHeader(CAMERA_CMD_HEADER* pCmdHeader) = 0;
        virtual HRESULT SendRespond(const CAMERA_CMD_RESPOND* pCmdRespond) = 0;
        virtual HRESULT ReceiveData(PBYTE8 pbData, const DWORD32& dwSize, DWORD32* pdwRecvSize) = 0;
        virtual HRESULT SendData(PBYTE8 pbData, const DWORD32& dwSize) = 0;
    };

    // 命令处理接口
    class ICameraCmdProcess
    {
    public:
        virtual ~ICameraCmdProcess(){};
        virtual HRESULT Process( CAMERA_CMD_HEADER* pCmdHeader, ICameraCmdLink* pCmdLink ) = 0;
    };

#endif // WIN32
}

#endif
