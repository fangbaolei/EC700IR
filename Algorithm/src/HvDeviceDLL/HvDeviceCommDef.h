#ifndef _HVDEVICECOMMDEF_H__
#define _HVDEVICECOMMDEF_H__

/* 图片类型 */
#define IMAGE_TYPE_UNKNOWN          0xffff0100	/**< 未知 */
#define IMAGE_TYPE_JPEG_NORMAL      0xffff0101	/**< Jpeg普通图 */
#define IMAGE_TYPE_JPEG_CAPTURE     0xffff0102	/**< Jpeg抓拍图 */
#define IMAGE_TYPE_JPEG_LPR         0xffff0103	/**< Jpeg调试码流 */

/* 视频类型 */
#define VIDEO_TYPE_UNKNOWN          0xffff0200	/**< 未知 */
#define VIDEO_TYPE_H264_NORMAL_I    0xffff0201	/**< H.264实时视频流I帧 */
#define VIDEO_TYPE_H264_NORMAL_P    0xffff0202	/**< H.264实时视频流P帧 */
#define VIDEO_TYPE_H264_HISTORY_I   0xffff0203	/**< H.264硬盘录像视频流I帧 */
#define VIDEO_TYPE_H264_HISTORY_P   0xffff0205	/**< H.264硬盘录像视频流I帧 */
#define VIDEO_TYPE_JPEG_HISTORY     0xffff0204	/**< JPEG硬盘录像视频流 */

/* 识别结果类型 */
#define RECORD_TYPE_UNKNOWN         0xffff0300	/**< 未知 */
#define RECORD_TYPE_NORMAL          0xffff0301	/**< 实时结果 */
#define RECORD_TYPE_HISTORY         0xffff0302	/**< 历史结果 */
#define RECORD_TYPE_STRING          0xffff0303	/**< 事件检测信息 */
#define RECORD_TYPE_INLEFT          0xffff0304	/**< 车牌到达离开标识 */

/* 连接状态 */
#define CONN_STATUS_UNKNOWN         0xffff0400	/**< 未知 */
#define CONN_STATUS_NORMAL          0xffff0401	/**< 正常 */
#define CONN_STATUS_DISCONN         0xffff0402	/**< 断开 */
#define CONN_STATUS_RECONN          0xffff0403	/**< 重连中 */
#define CONN_STATUS_RECVDONE        0xffff0404	/**< 历史数据接收完成 */
//以下两种状态为主动连接特有
#define CONN_STATUS_CONNFIRST           0xffff0405	/**< 初始连接*/
#define CONN_STATUS_CONNOVERTIME        0xffff0406	/**< 连接超时*/

/* 请求结果类型*/
#define RESULT_RECV_FLAG_REALTIME					0xffff0500  /**< 请求实时结果 */
#define RESULT_RECV_FLAG_HISTORY					0xffff0501  /**< 请求历史结果 */
#define RESULT_RECV_FLAG_HISTROY_ONLY_PECCANCY		0xffff0502  /**< 请求历史违章结果 */

/* 结果标志*/
#define RESULT_FLAG_INVAIL			0xffff0600  /**< 无效结果数据 */
#define RESULT_FLAG_VAIL			0xffff0601  /**< 有效结果数据 */
#define RESULT_FLAG_HISTROY_END		0xffff0602  /**< 历史结果结束标志 */

/* 请求H264类型*/
#define H264_RECV_FLAG_REALTIME				0xffff0700  /**< 请求实时视频 */
#define H264_RECV_FLAG_HISTORY				0xffff0701  /**< 请求历史视频 */

/* H264标志*/
#define H264_FLAG_INVAIL			0xffff0800  /**< 无效视频数据 */
#define H264_FLAG_VAIL				0xffff0801  /**< 有效视频数据 */
#define H264_FLAG_HISTROY_END		0xffff0802  /**< 历史结果结束标志 */


/* 请求MJPEG类型*/
#define MJPEG_RECV_FLAG_DEBUG				0xffff0900  /**< 接收调试码流 */
#define MJPEG_RECV_FLAG_REALTIME			0xffff0901  /**< 接收实时JPEG流*/

/* MJPEG标志*/
#define MJPEG_FLAG_INVAIL			0xffff0A00  /**< 无效MJPEG */
#define MJPEG_FLAG_VAIL				0xffff0A01  /**< 有效MJPEG帧 */
/* 设备类型 */
#define DEV_TYPE_UNKNOWN		0x00000000 /**< 未知设备 */
#define DEV_TYPE_HVCAM_200W		0x00000001 /**< 200万一体机 */
#define DEV_TYPE_HVCAM_500W		0x00000002 /**< 500万一体机 */
#define DEV_TYPE_HVSIGLE		0x00000004 /**< 高清2.0识别器 */
#define DEV_TYPE_HVCAM_SINGLE	0x00000008 /**< 简化版一体机 */
#define DEV_TYPE_HVCAMERA       0x00000010 /**< 纯相机 */
#define DEV_TYPE_HVMERCURY		0x00000020  /**< 水星设备 */
#define DEV_TYPE_HVEARTH		0x00000040  /**< 地球设备 */
#define DEV_TYPE_HVVENUS		0x00000080  /**< 金星设备 */
#define DEV_TYPE_HVJUPITER		0x00000100  /**< 木星设备 */
#define DEV_TYPE_ALL            0x0000ffff	/**< 所有设备 */


/* 回调数据类型 */
#define CALLBACK_TYPE_RECORD_PLATE			0xFFFF0001
#define CALLBACK_TYPE_RECORD_BIGIMAGE		0xFFFF0002
#define CALLBACK_TYPE_RECORD_SMALLIMAGE		0xFFFF0003
#define CALLBACK_TYPE_RECORD_BINARYIMAGE	0xFFFF0004
#define CALLBACK_TYPE_RECORD_INFOBEGIN		0xFFFF0005
#define CALLBACK_TYPE_RECORD_INFOEND		0xFFFF0006
#define CALLBACK_TYPE_STRING				0xFFFF0007
#define CALLBACK_TYPE_JPEG_FRAME			0xFFFF0008
#define CALLBACK_TYPE_H264_VIDEO			0xFFFF0009
#define CALLBACK_TYPE_HISTORY_VIDEO			0xFFFF0010
#define CALLBACK_TYPE_TFD_STRING			0xFFFF0011
#define CALLBACK_TYPE_ADV_VIDEO			    0xFFFF0012
#define CALLBACK_TYPE_ADV_HISTORY_VIDEO		0xFFFF0013
#define CALLBACK_TYPE_ADV_MJPEG				0xFFFF0014
#define CALLBACK_TYPE_ADV_RECORD_RESULT		0xFFFF0015
#define CALLBACK_TYPE_ADV_HISTORY_RECORD_RESULT		0xFFFF0016
#define CALLBACK_TYPE_REDEVELOP_RESULT      0xFFFF0017
#define CALLBACK_TYPE_RECORD_ILLEGALVIDEO	0xFFFF0018
/* 识别结果大图类型定义 */
#define RECORD_BIGIMG_BEST_SNAPSHOT			0x0001	/**< 最清晰识别图 */
#define RECORD_BIGIMG_LAST_SNAPSHOT			0x0002	/**< 最后识别图 */
#define RECORD_BIGIMG_BEGIN_CAPTURE			0x0003	/**< 开始抓拍图 */
#define RECORD_BIGIMG_BEST_CAPTURE			0x0004	/**< 最清晰抓拍图 */
#define RECORD_BIGIMG_LAST_CAPTURE			0x0005	/**<  最后抓拍图 */
#define RECORD_BIGIMG_PLATE					0x0006  /**< 车牌小图 */
#define RECORD_BIGIMG_PLATE_BIN				0x0007  /**< 车牌二值图 */

/* 连接类型定义 */
#define CONN_TYPE_UNKNOWN         0xffff0000	/**< 未知 */
#define CONN_TYPE_IMAGE           0xffff0001	/**< 图片 */
#define CONN_TYPE_VIDEO           0xffff0002	/**< 视频 */
#define CONN_TYPE_RECORD          0xffff0003	/**< 识别结果 */

/* 扩展数据类型 */
#define PLATE_RECT_BEST_SNAPSHOT           0x0001        /**< 最清晰识别图车牌坐标 */
#define PLATE_RECT_LAST_SNAPSHOT           0x0002        /**< 最后识别图车牌坐标 */
#define PLATE_RECT_BEGIN_CAPTURE           0x0003        /**< 开始抓拍图车牌坐标 */
#define PLATE_RECT_BEST_CAPTURE            0x0004        /**< 最清晰抓拍图车牌坐标 */
#define PLATE_RECT_LAST_CAPTURE            0x0005        /**< 最后抓拍图车牌坐标 */

#define FACE_RECT_BEST_SNAPSHOT            0x0006        /**< 最清晰识别图人脸信息 */
#define FACE_RECT_LAST_SNAPSHOT            0x0007        /**< 最后识别图人脸信息 */
#define FACE_RECT_BEGIN_CAPTURE            0x0008        /**< 开始抓拍图人脸信息 */
#define FACE_RECT_BEST_CAPTURE             0x0009        /**< 最清晰抓拍图人脸信息 */
#define FACE_RECT_LAST_CAPTURE             0x000A        /**< 最后抓拍图人脸信息 */

#define STACK_SIZE 128*1024 // 设置线程栈为128K

//车辆行驶车道类型
#define ROADTYPE_RRT_FORWARD	0x001 /** 直行*/
#define ROADTYPE_RRT_LEFT		0x002 /** 左转*/
#define	ROADTYPE_RRT_RIGHT		0x004	 /**右转*/
#define	ROADTYPE_RRT_TURN		0x008  /**掉头*/
#define ROADTYPE_RRT_ESTOP		0x010  /**非机动车*/
/* 触发监听回调数据类型 */
#define CALLBACK_TYPE_LISTEN_XML_DATE			0x00001  /** 回调触发类型*/

/* 通讯协议版本号 */
typedef enum
{
	PROTOCOL_VERSION_1,
	PROTOCOL_VERSION_2,
	PROTOCOL_VERSION_MERCURY,
	PROTOCOL_VERSION_EARTH,
	PROTOCOL_UNKNOWN
} PROTOCOL_VERSION;
 
typedef struct _CDevBasicInfo
{
	CHAR szIP[64];
	CHAR szMask[64];
	CHAR szGateway[64];
	CHAR szMac[128];
	CHAR szModelVersion[128];
	CHAR szSN[128];
	CHAR szWorkMode[128];
	CHAR szDevType[128];
	CHAR szDevVersion[128];
	CHAR szMode[128];
	CHAR szRemark[128];
	CHAR szBackupVersion[128];//新增
	CHAR szFPGAVersion[128];//新增
	CHAR szKernelVersion[128];//新增
	CHAR szUbootVersion[128];//新增
	CHAR szUBLVersion[128];//新增
	CHAR szFirmwareVersion[128];//新增
	CHAR szNetPackageVersion[128];//新增

	_CDevBasicInfo()
	{
		szIP[0]='\0';
		szMask[0]='\0';
		szGateway[0]='\0';
		szMac[0]='\0';
		szModelVersion[0]='\0';
		szSN[0]='\0';
		szWorkMode[0]='\0';
		szDevType[0]='\0';
		szDevVersion[0]='\0';
		szMode[0]='\0';
		szRemark[0]='\0';
		szBackupVersion[0]='\0';
		szFPGAVersion[0]='\0';
		szKernelVersion[0]='\0';
		szUbootVersion[0]='\0';
		szUBLVersion[0]='\0';
		szFirmwareVersion[0]='\0';
		szNetPackageVersion[0]='\0';
	}

}CDevBasicInfo;

typedef struct _CDevState
{
	INT nTemperature;		//温度    例：50 即 50摄氏度
	INT nCpuUsage;			//CPU使用率 例：50 即 50%
	INT nMemUsage;			//内存使用率 例：50 即 50%
	INT nHddState;			//硬盘使用状态 0：正常，1：异常
	BOOL fOCGateEnable;		//OC门使能状态
	BOOL fRTSPMulticastEnable;	//RTSP广播开关
	INT nTraceRank;

	CHAR szRecordLinkIP[512];
	CHAR szVideoLinkIP[512];
	CHAR szImageLinkIP[512];
	
	BOOL fNTPEnable;
	CHAR szNTPServerIP[64];
	INT nNTPServerUpdateInterval;
	//时区
	INT nTimeZone;
	//时间
	INT nYear;
	INT nMon;
	INT nDay;
	INT nHour;
	INT nMin;
	INT nSec;
	INT nMSec;

	_CDevState()
	{
		nTemperature=0;
		nCpuUsage=0;
		nMemUsage=0;
		nHddState=0;
		fOCGateEnable=0;
		fRTSPMulticastEnable=0;
		nTraceRank=0;
		szRecordLinkIP[0]='\0';
		szVideoLinkIP[0]='\0';
		szImageLinkIP[0]='\0';
		fNTPEnable=0;
		szNTPServerIP[0]='\0';
		nNTPServerUpdateInterval=0;
		nTimeZone=0;
		nYear=0;
		nMon=0;
		nDay=0;
		nHour=0;
		nMin=0;
		nSec=0;
		nMSec=0;
	}
}CDevState;

typedef struct _CCameraState
{
	BOOL fAGCEnable;
	INT nShutter;
	INT nGain;

	BOOL fAWBEnable;
	INT nGainR;
	INT nGainG;
	INT nGainB;


	_CCameraState()
	{
		fAGCEnable=0;
		nShutter=0;
		nGain=0;
		fAWBEnable=0;
		nGainR=0;
		nGainG=0;
		nGainB=0;

	}
}CCameraState;


typedef struct _CVideoState
{
	INT nCVBSDisplayMode;	     //CVBS模式  0:PAL,1:NTSL
	INT nH264FPS;
	INT nH264BitRateControl;
	INT nH264BitRate;
	INT nJpegFPS;
	INT nJpegCompressRate;

	//auto jpeg compress
	BOOL fAutoJpegCompressEnable;
	INT nJpegFileSize;
	
	INT nJpegCompressMaxRate;
	INT nJpegCompressMinRate;

	INT nDebugJpegStatus;

	_CVideoState()
	{
		nCVBSDisplayMode=0; 
		nH264FPS=0;
		nH264BitRateControl=0;
		nH264BitRate=0;
		nJpegFPS=0;
		nJpegCompressRate=0;
		fAutoJpegCompressEnable=0;
		nJpegFileSize=0;
		nJpegCompressMaxRate=0;
		nJpegCompressMinRate=0;
		nDebugJpegStatus=0;
	}
}CVideoState;


#define  MAX_ZONE_NUM 16
typedef struct _CCameraBasicInfo
{
	INT nBrightness;

	BOOL fImageEnhancementEnable;
	INT nContrast;
	INT nSaturation;
	
	BOOL fSharpnessEnable;
	INT nSharpness;

	//宽动态
	BOOL fWDREnable;
	INT nWDRLevel;

	//降噪
	BOOL fDeNoiseTNFEnable;
	BOOL fDeNoiseSNFEnable;
	INT nDeNoiseMode;
	INT nDeNoiseLevel;

	//2DDenoise
	BOOL f2DDeNoiseEnable;
	INT n2DeNoiseStrength;

	//Gamma 
	BOOL fGammaEnable;
	INT nGammaStrength;

	//color
	INT nColor;		// 色温

	//Manual
	INT nManualShutter;
	INT nManualGain;
	INT nManualGainR;
	INT nManualGainG;
	INT nManualGainB;
	
	//AWB
	BOOL fAWBEnable;
	//AGC
	BOOL fAGCEnable;
	INT nAGCLightBaseLine;
	INT nAGCShutterMin;
	INT nAGCShutterMax;
	INT nAGCGainMin;
	INT nAGCGainMax;
	//ACSync
	INT nACSyncMode;
	INT nACSyncDelay;
	
	INT nFilterMode;
	BOOL fDCEnable;

	BOOL fGrayImageEnable;//灰度图开关
	INT nEnRedLightThreshold;//红灯加红阈值
	INT nEdgeEnhance;//边缘增强

	_CCameraBasicInfo()
	{
		nBrightness=0;
		fImageEnhancementEnable=0;
		nContrast=0;
		nSaturation=0;
		fSharpnessEnable=0;
		nSharpness=0;
		fWDREnable=0;
		nWDRLevel=0;
		fDeNoiseTNFEnable=0;
		fDeNoiseSNFEnable=0;
		nDeNoiseMode=0;
		nDeNoiseLevel=0;
		f2DDeNoiseEnable=0;
		n2DeNoiseStrength=0;
		fGammaEnable=0;
		nGammaStrength=0;
		nColor=0;		
		nManualShutter=0;
		nManualGain=0;
		nManualGainR=0;
		nManualGainG=0;
		nManualGainB=0;
		fAWBEnable=0;
		fAGCEnable=0;
		nAGCLightBaseLine=0;
		nAGCShutterMin=0;
		nAGCShutterMax=0;
		nAGCGainMin=0;
		nAGCGainMax=0;
		nACSyncMode=0;
		nACSyncDelay=0;
		nFilterMode=0;
		fDCEnable=0;
		fGrayImageEnable=0;
		nEnRedLightThreshold=0;
		nEdgeEnhance=0;
	}
}CCameraBasicInfo;

typedef struct _COSDInfo
{
	BOOL fEnable;
	BOOL fPlateEnable;
	BOOL fTimeStampEnable;
	INT nPosX;
	INT nPosY;

	INT nFontSize;
	INT nFontColorR;
	INT nFontColorG;
	INT nFontColorB;

	CHAR szText[256];

	_COSDInfo()
	{
		fEnable=0;
		fPlateEnable=0;
		fTimeStampEnable=0;
		nPosX=0;
		nPosY=0;
		nFontSize=0;
		nFontColorR=0;
		nFontColorG=0;
		nFontColorB=0;
		szText[0]='\0';
	}

}COSDInfo;

typedef struct _CUserInfo
{
	CHAR szUserName[128];
	INT nAuthority;
}CUserInfo;

typedef struct _CHvPoint
{
	INT nX;
	INT nY;
}CHvPoint;

//图片信息
typedef struct _CImageInfo
{
	DWORD wSize;			//结构体大小
	DWORD wImgType;			//图片类型
	DWORD wWidth;			//图片宽
	DWORD wHeight;			//图片高度
	PBYTE pbData;		    //图片数据指针
	DWORD dwDataLen;		//图片数据长度
	DWORD64 dw64TimeMS;		//图片时标
	DWORD fHasData;			//图片数据是否有效
}CImageInfo;
#endif

