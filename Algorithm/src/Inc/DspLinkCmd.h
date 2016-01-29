// 该文件编码格式必须是WINDOWS-936格式

/**
* @file     DspLinkCmd.h
* @version  1.0
* @brief    DSPLink协议定义
*/

#ifndef _DSPLINKCMD_H_
#define _DSPLINKCMD_H_

#ifndef _NO_DSPBIOS_
#include "trackerdef.h"
#endif


typedef enum
{
    DCI_UNKNOWN = 0,		// 未知命令
    DCI_RECTIFY_TIME,		// 时钟矫正命令
    DCI_INIT_PHOTO_RECOGER, // 初始化图片识别对象命令
    DCI_PROCESS_PHOTO,		// 识别图片命令
    DCI_INIT_VIDEO_RECOGER, // 初始化视频流识别命令
    DCI_PROCESS_ONE_FRAME,  // 处理一帧视频流命令
    DCI_JPEG_ENCODE,		// Jpeg编码命令
    DCI_H264_ENCODE,		// H.264编码命令
    DCI_CAM_DSP,            // 相机平台信号处理命令
    DCI_IMAGE_EDDY_TYPE,    // 设置图片旋转标识命令
    DCI_STRING_OVERLAYINIT, // 初始化字符叠加命令
    DCI_MODIFY_VIDEO_PARAM, // 动态修改识别参数
    DCI_UPDATE_ALG_PARAM, // 动态修改算法参数
    DCI_UPDATE_MODEL_DATA, // 动态更新车辆检测模型数据
    DCI_COUNT
} DSP_CMD_ID;

typedef struct _DSP_RESPOND_HEAD
{
    DWORD32 dwCmdId;
    int iResult;
    char szErrorInfo[4096];
    int iAppendDataSize;
} DSP_RESPOND_HEAD;

#ifndef _NO_DSPBIOS_

typedef struct _IMAGE_EDDYTYPE_PARAM
{
    int nEddyType;
} IMAGE_EDDYTYPE_PARAM;

typedef struct _RECTIFY_TIME_PARAM
{
    DWORD32 dwArmCurTime;
} RECTIFY_TIME_PARAM;

typedef struct _INIT_PHOTO_RECOGER_PARAM
{
    int nPlateRecogParamIndex;
    TRACKER_CFG_PARAM cTrackerCfgParam;
} INIT_PHOTO_RECOGER_PARAM;

typedef struct _PROCESS_PHOTO_PARAM
{
    PR_PARAM prPram;
    IReferenceComponentImage* rgpSmallImage[MAX_EVENT_COUNT];
    IReferenceComponentImage* rgpBinImage[MAX_EVENT_COUNT];
#ifdef __cplusplus
    _PROCESS_PHOTO_PARAM()
    {
        memset(this, 0, sizeof(*this));
    }
#endif
} PROCESS_PHOTO_PARAM;

typedef struct _PROCESS_PHOTO_DATA
{
    HV_COMPONENT_IMAGE hvImage;
    HV_COMPONENT_IMAGE hvImageYuv;
    HV_COMPONENT_IMAGE rghvImageSmall[MAX_EVENT_COUNT];
    HV_COMPONENT_IMAGE rghvImageBin[MAX_EVENT_COUNT];
} PROCESS_PHOTO_DATA;

typedef struct _PROCESS_PHOTO_RESPOND
{
    int iResultCount;
    PROCESS_IMAGE_CORE_RESULT rgProcessPhotoResult[MAX_EVENT_COUNT];
#ifdef __cplusplus
    _PROCESS_PHOTO_RESPOND()
    {
        iResultCount = 0;
    }
#endif
} PROCESS_PHOTO_RESPOND;

typedef struct _TRIG_EVENT
{
    DWORD32 dwEventId;
    int iCarArriveCount;
    int iCarLeftCount;
    CARARRIVE_INFO_STRUCT rgCarArriveInfo[MAX_EVENT_COUNT];
    PROCESS_IMAGE_CORE_RESULT rgCarLeftCoreInfo[MAX_EVENT_COUNT];
#ifdef __cplusplus
    _TRIG_EVENT()
    {
        iCarArriveCount = 0;
        iCarLeftCount = 0;
        dwEventId = 0x00;
    }
#endif
} TRIG_EVENT;

typedef enum
{
    IOF_UNKNOWN = 0,	// 未知
    IOF_ADD_REF,		// 增加引用
    IOF_RELEASE,        // 释放内存
    IOF_COUNT
} IMG_OPER_FLAG;

/**
* 图片内存操作记录单元*/
typedef struct _IMG_MEM_OPER
{
    IReferenceComponentImage* pMemAddr;   // 操作地址
    int nOperFlag;
} IMG_MEM_OPER;

/**
* 图片内存操作记录
*/
typedef struct _IMG_MEM_OPER_LOG
{
    int nCount;
    IMG_MEM_OPER rgOperInfo[1024];
#ifdef __cplusplus
    _IMG_MEM_OPER_LOG()
    {
        memset(this, 0, sizeof(_IMG_MEM_OPER_LOG));
    }
#endif
} IMG_MEM_OPER_LOG;

/**
* 图片内存操作动作
*/
void ImageAddRef(IMG_MEM_OPER_LOG* pLog, IReferenceComponentImage* pRefImage);
void ImageRelease(IMG_MEM_OPER_LOG* pLog, IReferenceComponentImage* pRefImage);

typedef struct _INIT_VIDEO_RECOGER_PARAM
{
    int nPlateRecogParamIndex;
    int nLastLightType;
    int nLastPulseLevel;
	int nLastCplStatus;
	DWORD32 dwArmTick;
    TRACKER_CFG_PARAM cTrackerCfgParam;
} INIT_VIDEO_RECOGER_PARAM;

// 违停车辆检测的相关参数
typedef struct tag_CarDetectParam
{
	// 违停车辆检测参数
	BOOL fIsDetectStopBox; 			// 是否做车辆的检测
	int icarBoxwidth;				// 车辆检测最大宽度   像素
	float fltCarBoxleft;			// 检测框左上角X坐标   图像百分比
	float fltCarBoxTop;				// 检测框左上角Y坐标   图像百分比
	float fltCarBoxright;   		// 检测框右下角X坐标   图像百分比
	float fltCarBoxbottom;			// 检测框右下角Y坐标   图像百分比
	int iStepDivX;					// 车框检测步长系数X    像素
	int iStepDivY;					// 车框检测步长系数Y    像素
	int iScaleNum;					// 缩小检测次数         由最大值向小方向重复检测次数
	float fltScaleCoef;				// 缩小检测系数，          最大检测车框比例
	int iMergeNum;					// 重叠车框进行合并次数    合并次数
	int iMaxROI;					// 最大候选结果数       允许进行筛选的最大车框数
	int iDetType;					// 检测类型，进行一次检测还是进行二次检测   告诉dsp是否需要进行缩放和坐标还原
	int iDetNO;						// 同上存在dsp中
	float fltDetsalc;				// 取检测区域缩小进行除2的次数，方便坐标的还原。
	int iDetSelTypeNO;				// 检测模型的选择
	BOOL fIsDay;
} CAR_DETECT_PARAM;

// 单帧识别参数
typedef struct tag_FrameRecognizeParam
{
    DETECTAREA cRecogSnapArea; // 抓拍图识别参数
    int  iPlateUpExpend;        // 车牌向上扩展 （单位：占车牌的百分比）
	int  iPlateDownExpend;      // 车牌向下扩展 （单位：占车牌的百分比）
	bool fPlateExpendEnable;    // 车牌识别扩展开关
    int iEnvStatus;     // 环境状态，来自ARM端  3:day, 2:dusk, 1:night

    HV_RECT rectCaptureRegArea;     // 抓拍图识别区域
} FRAME_RECOGNIZE_PARAM;

typedef struct _PROCESS_ONE_FRAME_PARAM
{
    int iVideoID;
    BOOL fIsCaptureImage;
    DWORD32 dwFrameNo;
    DWORD32 dwImageTime;
    DWORD32 dwCurTick;
	int iDiffTick;
    BOOL fIsCheckLightType;
	BOOL fIsARMNight;		//Arm端判断为晚上

    IReferenceComponentImage* pCurFrame;
    IReferenceComponentImage* rgpSmallImage[MAX_EVENT_COUNT];
    IReferenceComponentImage* rgpBinImage[MAX_EVENT_COUNT];
	DWORD32 dwIOLevel;
	int iEnvStatus;         // 环境状态，来自ARM端  3:day, 2:dusk, 1:night
	int iCarArriveTriggerType;

	CAR_DETECT_PARAM cDetectParam;
	FRAME_RECOGNIZE_PARAM cFrameRecognizeParam;

#ifdef __cplusplus
    _PROCESS_ONE_FRAME_PARAM()
    {
        memset(this, 0, sizeof(*this));
    }
#endif
} PROCESS_ONE_FRAME_PARAM;

typedef struct _PROCESS_ONE_FRAME_DATA
{
    HV_COMPONENT_IMAGE hvImage;
    HV_COMPONENT_IMAGE hvImageYuv;
    HV_COMPONENT_IMAGE rghvImageSmall[MAX_EVENT_COUNT];
    HV_COMPONENT_IMAGE rghvImageBin[MAX_EVENT_COUNT];
    SYNC_DET_DATA cSyncDetData;
#ifdef __cplusplus
    _PROCESS_ONE_FRAME_DATA()
    {
       memset(this, 0, sizeof(*this));
    }
#endif
} PROCESS_ONE_FRAME_DATA;

typedef struct _PROCESS_ONE_FRAME_RESPOND
{
    TRIG_EVENT cTrigEvent;
    LIGHT_TYPE cLightType; // 亮度级别
    int nWDRLevel;	// WDR等级
	int iCplStatus; // 偏光镜状态 1:使能 0:不使能 -1:未启用
	int iPulseLevel;
	int iFrameAvgY; //帧平均亮度
	HV_RECT rcRedLight[20];
    TRACK_RECT_INFO cTrackRectInfo;
    IMG_MEM_OPER_LOG cImgMemOperLog;
    int nEnvLightType; // 0白天，1傍晚，2晚上
	int iDetType[20];
#ifdef __cplusplus
    _PROCESS_ONE_FRAME_RESPOND()
    {
       memset(this, 0, sizeof(*this));
    }
#endif
} PROCESS_ONE_FRAME_RESPOND;

#endif

// --- Jpeg编码 ---

typedef struct _JPEG_ENCODE_PARAM
{
    DWORD32 dwCompressRate;         // Jpeg压缩率
	DWORD32 dwJpegDataType;         // Jpeg数据类型（0：表示Jpeg数据段分三段；1：表示Jpeg数据段为一段；注：目前仅对500w模式下生效）
    INT iEddy;                      // 逆时针旋转90度

#ifdef __cplusplus
    _JPEG_ENCODE_PARAM()
    {
        memset(this, 0, sizeof(*this));
    }
#endif
} JPEG_ENCODE_PARAM;

typedef struct tagTEXT
{
	int iAlpha;
	int iSize;
	int iWidth;
	int iHeight;
#ifdef __cplusplus	
	tagTEXT()
	{
		memset(this, 0, sizeof(*this));
	}
#endif
}TEXT;

typedef struct _JPEG_ENCODE_DATA
{
    HV_COMPONENT_IMAGE hvImageYuv;  // 图片源数据，格式：CbYCrY
    HV_COMPONENT_IMAGE hvImageJpg;
    BYTE8 szDateTimeStrings[32];    // 要叠加到Jpeg图片中的日期时间字符串（为空则表示不启用叠加功能）
	
	PBYTE8 phys;    //叠加位图指针
	char   szTime[255]; //时间文字
	int    x;       //叠加的x位置
	int    y;		//叠加的y位置
	BYTE8  yColor;  //叠加位图的Y颜色分量
	BYTE8  uColor;  //叠加位图的U颜色分量
	BYTE8  vColor;  //叠加位图的V颜色分量

#ifdef __cplusplus
    _JPEG_ENCODE_DATA()
    {
        memset(this, 0, sizeof(*this));
    }
#endif
} JPEG_ENCODE_DATA;

typedef struct _JPEG_ENCODE_RESPOND
{
    DWORD32 dwJpegLen;
    DWORD32 dwExtErrCode;

#ifdef __cplusplus
    _JPEG_ENCODE_RESPOND()
    {
        memset(this, 0, sizeof(*this));
    }
#endif
} JPEG_ENCODE_RESPOND;

// --- H.264编码 ---

//H.264编码器操作类型定义
#define OPTYPE_OPEN 1
#define OPTYPE_ENCODE 2
#define OPTYPE_CLOSE 3

typedef struct _H264_ENCODE_PARAM
{
    DWORD32 dwOpType;               // 操作类型：开启，编码，关闭
    DWORD32 dwIntraFrameInterval;   // I帧间隔（例如：该值为3，则码流为：I P P I P P ...）
    DWORD32 dwTargetBitRate;        // 比特率
    DWORD32 dwFrameRate;            // 帧率
    DWORD32 dwInputWidth;           // 输入图像的宽度
    DWORD32 dwInputHeight;          // 输入图像的高度
    DWORD32 dwOutputBufSize;        // H.264帧的输出缓存区的大小
    BOOL fEnableH264BrightnessAdjust;   // 使能H264亮度调节
    int iAdjustPointX;                  // H264亮度调节点X值
    int iAdjustPointY;                  // H264亮度调节点Y值
#ifdef __cplusplus
    _H264_ENCODE_PARAM()
    {
        memset(this, 0, sizeof(*this));
    }
#endif
} H264_ENCODE_PARAM;

typedef struct _H264_ENCODE_DATA
{
    HV_COMPONENT_IMAGE hvImageYuv;  // 图片源数据，格式：CbYCrY
    HV_COMPONENT_IMAGE hvImageFrame;
    BYTE8 szDateTimeStrings[32];    // 要叠加到H.264码流中的日期时间字符串（为空则表示不启用叠加功能）
    BOOL fDoubleStream;             // fDoubleStream是为了解决双码流时字体颜色显示不一致而设置
#ifdef __cplusplus
    _H264_ENCODE_DATA()
    {
        memset(this, 0, sizeof(*this));
    }
#endif
} H264_ENCODE_DATA;

//帧类型定义
#define FRAME_TYPE_H264_I 0
#define FRAME_TYPE_H264_P 1
#define FRAME_TYPE_H264_SKIP 2

typedef struct _H264_ENCODE_RESPOND
{
    DWORD32 dwFrameLen;
    DWORD32 dwFrameType;  // 帧类型（0:I帧, 1:P帧, 2:忽略帧）
    DWORD32 dwExtErrCode;

#ifdef __cplusplus
    _H264_ENCODE_RESPOND()
    {
        memset(this, 0, sizeof(*this));
    }
#endif
} H264_ENCODE_RESPOND;

// --- 字符叠加 ---

typedef struct _STRING_OVERLAY_PARAM
{
    // x，y：在图片中开始叠加信息字符串的起始位置
    // w，h：待叠加信息字符串的点阵的总体宽高（以像素点为单位）
    // iIsFixedLight：字体固定亮度（1：固定 0：亮度随背景色变化而变化）
    // rgbDotMatrix：待叠加信息字符串的点阵数据
    // bFontR，bFontG，bFontB：待叠加信息字符串的字体颜色
    // iFontColor: 待叠加信息字符串的字体颜色
    // fIsSideInstall：侧装
    int x;
    int y;
    int w;
    int h;
    int iIsFixedLight;
    int iFontColor;
    BOOL fIsSideInstall;

#ifdef __cplusplus
    _STRING_OVERLAY_PARAM()
    {
        memset(this, 0, sizeof(*this));
    }
#endif
} STRING_OVERLAY_PARAM;

// 点阵数据，存储于共享内存中，用于ARM与DSP间传输
typedef struct _STRING_OVERLAY_DATA
{
    DSPLinkBuffer rgDotMatrix;

#ifdef __cplusplus
    _STRING_OVERLAY_DATA()
    {
        memset(this, 0, sizeof(*this));
    }
#endif
} STRING_OVERLAY_DATA;

typedef struct _STRING_OVERLAY_RESPOND
{
    DWORD32 dwExtErrCode;

#ifdef __cplusplus
    _STRING_OVERLAY_RESPOND()
    {
        dwExtErrCode = 0;
    }
#endif
} STRING_OVERLAY_RESPOND;

// --- 相机平台数据信号处理 ---

//CamDsp处理类型定义
#define PROC_TYPE_JPEGENC 1          // Jpeg编码
#define PROC_TYPE_H264ENC 2          // H.264编码
#define PROC_TYPE_JPEGH264ENC 3      // Jpeg和H.264编码，产生双码流

typedef struct _CAM_DSP_PARAM
{
    DWORD32 dwProcType;  // CamDsp处理类型
    JPEG_ENCODE_PARAM cJpegEncodeParam;
    H264_ENCODE_PARAM cH264EncodeParam;

#ifdef __cplusplus
    _CAM_DSP_PARAM()
    {
        memset(this, 0, sizeof(*this));
    }
#endif
} CAM_DSP_PARAM;

typedef struct _CAM_DSP_DATA
{
    JPEG_ENCODE_DATA cJpegEncodeData;
    H264_ENCODE_DATA cH264EncodeData;

#ifdef __cplusplus
    _CAM_DSP_DATA()
    {
        memset(this, 0, sizeof(*this));
    }
#endif
} CAM_DSP_DATA;

typedef struct _CAM_DSP_RESPOND
{
    JPEG_ENCODE_RESPOND cJpegEncodeRespond;
    H264_ENCODE_RESPOND cH264EncodeRespond;
    DWORD32 dwExtErrCode;

#ifdef __cplusplus
    _CAM_DSP_RESPOND()
    {
        memset(this, 0, sizeof(*this));
    }
#endif
} CAM_DSP_RESPOND;

#endif
