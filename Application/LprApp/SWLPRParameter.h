#ifndef __SW_LPR_PARAMETER_H__
#define __SW_LPR_PARAMETER_H__
#include "trackerdef.h"
#include "SWClassFactory.h"
#include "ProcessRule.h"
#include "AutoLinkParam.h"
#include "SWMatchTansformDataStruct.h"
#include "SWFilterStruct.h"
#include "SWFtpParameter.h"
#include "SWDomeRockerParameter.h"
#include "SWOnvifRtspParameter.h"

#define MAX_CAP_CAM_COUNT      4
#define MAX_CAM_COUNT          8 
#define MAX_STREETINFO_LENGTH  255
#define MAX_LEVEL_COUNT        14
#define SVR_PORT               8888

/**
* @brief 抓拍相机模块参数结构体
*/
typedef struct _tagCapCamParam
{
	INT nCamType;   //抓拍相机类型(0-创宇J系列相机)
	INT nCamNum;    //抓拍相机个数
	INT rgnSignalType[MAX_CAP_CAM_COUNT];   //抓拍相机信号源类型
	CHAR rgszRoadIP[MAX_CAP_CAM_COUNT][20]; //抓拍相机IP

	_tagCapCamParam()
	{
		nCamType = 0;
		nCamNum = 0;
		for (INT i = 0; i < MAX_CAP_CAM_COUNT; i++)
		{
			rgnSignalType[i] = -1;
			sprintf(rgszRoadIP[i], "172.18.10.10%d", i);
		}
	}
}CAP_CAM_PARAM;

/**
* @brief 结果发送模块参数结构体
*/
typedef struct _ResultSenderParam
{
	//结果后处理模块的相关参数
	PROCESS_RULE cProcRule;     /**< 后处理规则 */

	BOOL fInitHdd;              /**< 是否初始化硬盘 */
	
	INT  iDiskType;				        /**< 网络硬盘类型 */
	

	CHAR szNetDiskIP[32];               /**< 网络存储的IP地址 */
	//CHAR szNetDiskUser[32];             /**< 网络存储的用户名称 */
	//CHAR szNetDiskPwd[32];		        /**< 网络存储的用户密码 */
	INT  iNetDiskSpace;   				/**< 网络存储的容量,单位G */
	CHAR szNFS[32];                     /**< 网络硬盘NFS路径 */
	CHAR szNFSParam[255];               /**< NFS协议的挂载参数*/
	//CHAR szFtpRoot[32];                 /**< 网络存储的FTP根路径 */

	
	//可靠性保存模块
	BOOL fIsSafeSaver;            /**< 是否启动安全存储 */
	CHAR szSafeSaverInitStr[64];  /**< 安全储存器初始化参数 */  //暂时不用，预留
	INT  iRecordFreeSpacePerDisk;
	INT  iVideoFreeSpacePerDisk;
	INT  iSaveSafeType;                 /**< 固态硬盘存储方式 0:没有客户端连接时才存储 1:一直存储 */
	INT  iFileSize;                     /**< 定长存储中单个文件的大小*/

	//发送视频相关参数
	BOOL fSaveVideo;         /**< 保存视频数据 */
	INT  iSaveVideoType;                 /**< 视频存储方式 0:一直存储 1:有违章结果时才存储 */
	INT iVideoDisplayTime;  /**< 发送视频时间间隔 */
	INT iDrawRect;          /**< 发送红框标识 */
	INT iEddyType;          /**< 图片旋转 */

	//附加信息模块参数
	BOOL fOutputAppendInfo;     //输出车牌附加信息开关
	BOOL fOutputObservedFrames; //输出有效帧数开关
	BOOL fOutputCarArriveTime;  //输出检测时间开关
	BOOL fOutputFilterInfo; //输出后处理信息
	CHAR szStreetName[MAX_STREETINFO_LENGTH];       //路口名称
	CHAR szStreetDirection[MAX_STREETINFO_LENGTH];  //路口方向

	INT iOutputOnlyPeccancy;//1:只输出违章车辆 2:只输出非违章结果 3:接收历史数据时,非违章输出文本,违章输出文本和图片
	INT iSendRecordSpace; //结果发送间隔(*100ms)
	INT iSendHisVideoSpace;//历史录像发送间隔(*100ms)

	INT iSpeedLimit;    //速度上限值(km/h)

	INT iCheckEventTime;    //事件检测周期(单位:分钟)

	INT iSaveType;    //结果保存方式
	INT iWidth;         //截图宽度
	INT iHeight;        //截图高度
	INT iBestSnapshotOutput;    //主视频最清晰大图输出
	INT iLastSnapshotOutput;    //主视频最后大图输出
	INT iOutputCaptureImage;    //非违章车辆输出三张抓拍图
    INT iLastSnapshotCropOutput;    //主视频最后大图截图输出
    INT iCropWidthLevel;
    INT iCropHeightLevel;

	INT iFilterUnSurePeccancy;  //过滤不合格的违章记录
	INT nOutputPeccancyType;    //输出违章类型方式
	CHAR szPeccancyPriority[256]; //违章类型优先级

	float fltCarAGCLinkage;  //车流量与AGC联动开关，0为关，非0表示单位时间车流量少于该值时启用AGC，大于则关闭
	INT iAWBLinkage;        //环境亮度与AWB联动开关
	INT nRedPosBrightness;

	INT iEnablePanoramicCamera;     //全景相机使能开关
	CHAR szPanoramicCameraIP[20];   //全景相机IP

	CAP_CAM_PARAM cCapCamParam; //抓拍相机参数
	AUTO_LINK_PARAM cAutoLinkParam; //主动连接参数
	FTP_PARAM cFtpParam;			//FTP连接参数
	

	BOOL fMTUSetEnable;

	INT iCapMode;             //抓拍模式 (0:抓拍一次; 1:超速违章抓拍2次; 2:全部抓拍2次)
	INT iFlashDifferentLane;  //闪光灯分车道闪
    INT iFlashOneByOne;         // 闪光灯轮闪开关

	BOOL fEnableTriggerOut; // 触发输出使能开关
	int nTriggerOutNormalStatus; // 触发输出常态控制(极性)
	int nTriggerOutPlusWidth; // 触发输出脉宽

	BOOL fEnableNtp;     //NTP使能开关
	CHAR szNtpServerIP[20];   //NTP服务器IP
	INT iNtpSyncInterval;     //NTP同步时间间隔,单位sec
	INT iTimeZone;			//时区信息

	INT iUseRushRule;	//使用冲杆规则开关
	INT iEnableBackupIO;	//启用备用IO
	INT iTimeIn;	//信号间隔时间
	INT iBarrierStatus;	//栏杆机抬起电平

	_ResultSenderParam()
	{
		iBestSnapshotOutput = 1;
		iLastSnapshotOutput = 1;
		iOutputCaptureImage = 0;
        iLastSnapshotCropOutput = 1;
        iCropWidthLevel = 6;
        iCropHeightLevel = 4;
		iRecordFreeSpacePerDisk = 10;
		iVideoFreeSpacePerDisk = 10;
		iVideoDisplayTime = 500;
		iDrawRect = 0;
		iEddyType = 0;
		iSpeedLimit = 1000;
		iSaveType = 0;
		iWidth = 720;
		iHeight = 288;
		iCheckEventTime = 1;
		iFilterUnSurePeccancy = 0;
		fOutputAppendInfo = TRUE;
		fOutputObservedFrames = TRUE;
		fOutputCarArriveTime = FALSE;
		swpa_strcpy(szSafeSaverInitStr, "NULL");
		swpa_strcpy(szStreetName, "NULL");
		swpa_strcpy(szStreetDirection, "NULL");
		iOutputOnlyPeccancy = 0;
		iSendRecordSpace = 5;
		iSendHisVideoSpace = 3;
		nOutputPeccancyType = 0;
		fltCarAGCLinkage = 0.0f;
		iAWBLinkage = 0;
		fOutputFilterInfo = 0;
		fInitHdd = FALSE;
		iFileSize = 512;		
		iDiskType = 2;
		fIsSafeSaver = FALSE;
		iSaveSafeType = 1;
		fSaveVideo = FALSE;
		iSaveVideoType = 0;
		fMTUSetEnable = FALSE;
		swpa_strcpy(szPeccancyPriority, "逆行-非机动车道-闯红灯-不按车道行驶-压线-超速");
		iCapMode = 0;
        iFlashDifferentLane = 0;
        iFlashOneByOne = 1;

		fEnableTriggerOut = 1;
		nTriggerOutNormalStatus = 1;
		nTriggerOutPlusWidth = 5000;

		fEnableNtp = 0;
		iNtpSyncInterval = 3600; // 1小时
		iTimeZone = 20;		//默认东8区，GMT+8,0为GMT-12
		swpa_memset(szNtpServerIP, 0, sizeof(szNtpServerIP));
		swpa_strcpy(szNtpServerIP, "210.72.145.44");

		swpa_strcpy(szNetDiskIP, "172.18.10.10");
		//strcpy(szFtpRoot, "/array1");
		//strcpy(szNetDiskUser, "No User");
		//strcpy(szNetDiskPwd, "123");
		swpa_strcpy(szNFS, "/volume1/nfs");
		swpa_strcpy(szNFSParam, "-o nolock,tcp");
		iNetDiskSpace = 1;

		iUseRushRule = 0;
		iEnableBackupIO = 0;
		iTimeIn = 1500;
		iBarrierStatus = 0;
	};

} ResultSenderParam;

/**
*@brief 网络配置结构体
*/
typedef struct _TcpipParam
{
	CHAR szIp[32];
	CHAR szNetmask[32];
	CHAR szGateway[32];

	_TcpipParam()
	{
		sprintf(szIp, "100.100.100.101");
		sprintf(szNetmask, "255.0.0.0");
		sprintf(szGateway, "100.100.1.1");
	}
} TcpipParam;

/**
* @brief 前端图像采集模块参数结构体
*/
typedef struct _tagImgFrameParam
{
	INT iCamNum;                            //相机个数
	CHAR rgstrCamIP[MAX_CAM_COUNT][32];     //抓拍相机IP
	CHAR rgstrHVCParm[MAX_CAM_COUNT][128];  //抓拍识别参数

	float fltCaptureFrameRate; //采集帧率
	INT iOutputFrameRate;   // 输出帧率（1～15）

	_tagImgFrameParam()
	{
		iCamNum = 1;
		for ( INT i=0; i<MAX_CAM_COUNT; ++i )
		{
			memset(rgstrCamIP[i], 0, 32);
			memset(rgstrHVCParm[i], 0, 128);
		}
		fltCaptureFrameRate = 12.5;
		iOutputFrameRate = 15;
	};

} IMG_FRAME_PARAM;

/**
* @brief 前端相机参数结构体
*/
typedef struct _tagCamCfgParam
{
	CHAR szIP[100];
	INT iCamType; // 摄像机协议类型：0-一体化相机协议，1-专用测试协议，2-Camyu摄像机协议
	INT iCamPort; //测试协议端口
	INT iDynamicCfgEnable;
	INT iDynamicTriggerEnable;
	INT iTriggerDelay;
	INT iTriggerPlateType; //卡口车尾模式下，哪些车牌类型需要触发抓拍，安通杰需求，黄国超增加，2012-01-04
	INT iDynamicCfgEnableOnTrigger;
	INT iNightShutterThreshold;
    INT iNightAvgYThreshold;        // 晚上亮度阈值
    INT iDuskAvgYThreshold;         // 傍晚亮度阈值
    //INT iWorkType;	//正装侧装类型，0为正装(红外检车)，1是侧装(雷达检车)

	// 摄像机接收缓冲帧数
	INT iJpegBufferCount;
	INT nTestProtocolPort;

	//自动填充参数
	INT iAutoParamEnable;
	float fltDB;				//步长
	INT iMinExposureTime;
	INT iMinGain;
	INT iMaxExposureTime;
	INT iMaxGain;
	INT iEnableAGC;
	INT iMaxAGCLimit;
	INT iMinAGCLimit;
	INT iMaxPSD;
	INT iMinPSD;
	INT iMinPlateLight;
	INT iMaxPlateLight;

	INT irgExposureTime[MAX_LEVEL_COUNT];
	INT irgAGCLimit[MAX_LEVEL_COUNT];
	INT irgGain[MAX_LEVEL_COUNT];

	INT iCaptureAutoParamEnable;		/**< 自动抓拍参数设置使能*/
	INT iCaptureDayShutterMax;
	INT iCaptureDayGainMax;
	INT iCaptureNightShutterMax;
	INT iCaptureNightGainMax;

	INT iIPNCLogOutput;

	_tagCamCfgParam()
	{
		swpa_sprintf(szIP, "172.18.23.42");     
		iCamType = 0;
		iCamPort = 8888;
		iDynamicCfgEnable = 0;
		iDynamicTriggerEnable =0;
		iTriggerDelay = 0;
		iTriggerPlateType = 0;
		iDynamicCfgEnableOnTrigger = 0;
		iNightShutterThreshold = 50000;// 此参数仅金星使用，因此将默认值设置如此
		iNightAvgYThreshold = 0;
        iDuskAvgYThreshold = 50;

		//iWorkType=0;
		
		iJpegBufferCount = 1;
		iEnableAGC = 0;
		iAutoParamEnable = 0;
		fltDB = 3.0;
		iMinExposureTime  = 100;
		iMinGain  = 70;
		iMaxExposureTime  = 6000;
		iMaxGain  = 220;
		iMaxAGCLimit = 140;
		iMinAGCLimit = 10;
		iMaxPSD = 4200;
		iMinPSD = 1000;
		iMinPlateLight = 140;
		iMaxPlateLight = 160;
		nTestProtocolPort = SVR_PORT;

		iIPNCLogOutput = 0;
			
		INT irgExposureTimeTemp[MAX_LEVEL_COUNT] =
		{
			8, 8, 8, 8, 12, 17, 24, 34, 48, 68, 96, 96, 96, 96
		};

		for (INT i = 0; i < MAX_LEVEL_COUNT; i++)
		{
			irgExposureTime[i] = irgExposureTimeTemp[i];
			irgAGCLimit[i] = -1;
			irgGain[i] = -1;
		}

		iCaptureAutoParamEnable = 0;
		iCaptureDayShutterMax = 1500;
		iCaptureDayGainMax	= 180;
		iCaptureNightShutterMax = 2000;
		iCaptureNightGainMax = 180;

	}
}CAM_CFG_PARAM;

//368端结构体
typedef struct _CamAppParam
{
	INT iJpegStream;        /**< 是否开启Jpeg流。0:否；1：是。 */
	INT iH264Stream;        /**< 是否开启H.264流。0:否；1：是。 */

	INT iIFrameInterval;    /**< H.264码流中的I帧间隔 */
	INT iTargetBitRate;     /**< H.264码流比特率 */
	INT iMaxBitRate;		/**< H.264最大码率 */
	INT iFrameRate;         /**< H.264码流帧率 */
	INT iResolution;        /**< H.264图像分辨率 */
	INT iRateControl;		/**< H.264码率控制*/
	INT iVbrDuration;		/**< H.264判断场景变化的采样时间*/
	INT iVbrSensivitity;	/**< H.264判断场景变化的灵敏度*/

	INT iIFrameIntervalSecond;    /**< 第二路H.264码流中的I帧间隔 */
	INT iTargetBitRateSecond;     /**< 第二路H.264码流比特率 */
	INT iMaxBitRateSecond;		  /**< 第二路H.264最大码率 */
	INT iFrameRateSecond;         /**< 第二路H.264码流帧率 */
	INT iResolutionSecond;        /**< 第二路H.264图像分辨率 */
	INT iRateControlSecond;
	INT iVbrDurationSecond;
	INT iVbrSensivititySecond;

	INT iAGCTh;             /**< AGC期望亮度 */
	INT iAGCShutterLOri;    /**< AGC快门调节下限 */
	INT iAGCShutterHOri;    /**< AGC快门调节上限 */
	INT iAGCGainLOri;       /**< AGC增益调节下限 */
	INT iAGCGainHOri;       /**< AGC增益调节上限 */
	INT iAGCScene;			/**< AGC情景模式 */
    INT iAGCDayNightShutterControl;     /**< AGC白天、晚上独立快门上限控制 */
    INT iAGCNightShutterHOri;    /**< AGC快门调节上限 */

	INT iGainR;             /**< R增益 */
	INT iGainG;             /**< G增益 */
	INT iGainB;             /**< B增益 */
	INT iGain;              /**< 相机增益 */
	INT iShutter;           /**< 快门时间 */

	INT iCaptureGainR;      /**< 抓拍R增益 */
	INT iCaptureGainG;      /**< 抓拍G增益 */
    INT iCaptureGainB;      /**< 抓拍B增益 */
	INT iCaptureGain;       /**< 抓拍相机增益 */
	INT iCaptureShutter;    /**< 抓拍快门时间 */
	INT iCaptureSharpenThreshold;       /**< 抓拍锐度值 */

	INT iCaptureAutoParamEnable;		/**< 自动抓拍参数设置使能*/
	INT iEnableCaptureGainRGB;          /**< 使能抓拍RGB颜色增益 */
	INT iEnableCaptureGain;             /**< 使能抓拍增益 */
	INT iEnableCaptureShutter;          /**< 使能抓拍快门 */
	INT iEnableCaptureSharpen;          /**< 使能抓拍锐化*/

	INT iJpegCompressRate;              /**< Jpeg压缩率 */
	INT iJpegCompressRateCapture;       /**< 抓拍Jpeg图压缩率 */

	CHAR szNFSParam[255];               /**< NFS协议的挂载参数*/
	CHAR szSafeSaverInitStr[64];        /**< 安全储存器初始化参数 */

	//    BOOL fIsSideInstall;                /**< 前端镜头是否为侧装 */
	INT iJpegExpectSize;                /**< Jpeg图片期望大小（单位：Byte），通过自动设置Jpeg压缩率实现 */
	INT iJpegCompressRateL;             /**< Jpeg压缩率自动调节下限 */
	INT iJpegCompressRateH;             /**< Jpeg压缩率自动调节上限 */

	INT iJpegType;                      /**< Jpeg图片格式 */
	//    INT iOutputFrameRate;               /**< 相机输出帧率 */

	INT iAGCEnable;                     /**< AGC使能 */
	INT iAWBEnable;                     /**< AWB使能 */
	INT iFlashRateSynSignalEnable;      /**< 频闪同步信号使能 */
	INT iFlashRateMultiplication;		/**< 频闪输出是否倍频，1：倍频输出 */
	INT iFlashRatePolarity;				/**< 频闪输出极性。1：正，0：负*/
	INT iFlashRatePulseWidth;			/**< 频闪输出脉宽。100 到 20000. 微秒*/
	INT iFlashRateOutputType;			/**< 频闪输出类型。1：上拉（电平）, 0:OC(开关) */
	INT iCaptureSynSignalEnable;        /**< 抓拍同步信号使能 */
	INT iCaptureSynPolarity;			/**< 抓拍输出极性。1：正，0：负*/
	INT iCaptureSynPulseWidth;			/**< 抓拍输出脉宽。100 到 20000. 微秒*/
	INT iCaptureSynOutputType;			/**< 抓拍输出类型。1：上拉（电平）, 0:OC(开关) */
	INT iCaptureSynCoupling;			/**< 频闪与抓拍是否耦合。 1：耦合，0：不耦合。*/
	//    INT iENetSyn;                       /**< 电网同步 0:15fps 1:12.5fps */
	INT iCaptureEdge;                   /**< 抓拍触发沿 0:外部下降沿触发 1:外部上升沿触发 2: 上升沿下降沿都触发 3：不触发 */
	INT iFlashDifferentLane;            /**< 闪光灯分车道闪 0:不分车道 1: 分车道 */
	INT iFlashDifferentLaneExt;         /**< 闪光灯分车道闪(硬触发) 0:不分车道 1: 分车道 */

	//CHAR szNetDiskIP[32];               /**< 网络存储的IP地址 */
	CHAR szFtpRoot[32];                 /**< 网络存储的FTP根路径 */
	CHAR szNetDiskUser[32];             /**< 网络存储的用户名称 */
	CHAR szNetDiskPwd[32];		        /**< 网络存储的用户密码 */
	//INT  iNetDiskSpace;   				/**< 网络存储的容量,单位G */
	//CHAR szNFS[32];                     /**< 网络硬盘NFS路径 */
	INT  iAvgY;                         /**< 晚上亮度阈值*/

	INT iGammaMode;						 /**< Gama 通道选择值：0：6467和368都无GAMMA，1：6467有368无，2：6467无368有，3：6467有368有*/
	INT  rgiDefGamma[8][2];             /**< 默认Gama值*/
	INT  rgiGamma[8][2];
	INT rgiAGCZone[16];

	INT iX;                             /**< 字符叠X坐标 */
	INT iY;                             /**< 字符叠y坐标 */
	INT iFontSize;                      /**< 字体大小(注：此大小为点阵字体规格，如规格为24*24点阵，则该值为24) */
	INT iDateFormat;                    /**< 日期格式 */
	INT iFontColor;                     /**< 字体颜色(RGB) */
	CHAR szCharater[255];               /**< 点阵缓冲区*/
	INT iEnableCharacterOverlap;        /**< 字符叠加使能 */
	INT iEnableFixedLight;              /**< 字体颜色是否固定亮度 */

	// A67
	INT iEnableSharpen;                 /**< 使能锐化开关 0: 不使能  1：使能 */
	INT iSharpenThreshold;              /**< 锐化阀值[0~100] */
	INT iSharpenCapture;                /**< 锐化抓拍图：1：锐化抓拍图 0：锐化所有图像*/
	INT iContrastValue;                 /**< 对比度值[-100~100] */
	INT iSaturationValue;               /**< 饱和度值[-100~100] */
	INT iEnableDCAperture;              /**< 使能DC光圈 0: 不使能  1：使能 */
	INT iColorMatrixMode;				/**< 使能图像增强功能，1 为关闭使能，2为使能抓拍，3为使能所有帧 */
	INT iTemThreshold;              	/**< 色温5个级别[32,40,52,55,60,70] */
	INT iHueThreshold;              	/**< 色度[-180~180] */
	INT iHazeRemovalMode;				/**< 使能图像除雾功能，1 为关闭使能，2为使能抓拍，3为使能所有帧 */
	INT iHazeRemovalThreshold;			/**< 图像除雾阀值[0~10] */

	// A67
	BOOL fEnableH264BrightnessAdjust;
	INT iAdjustPointX;
	INT iAdjustPointY;

	INT iEnableCCDSignalCheck;

	// 外同步参数
	INT iExternalSyncMode;		// 0-不开启;1-开启内部电网同步；2-开启外部电网同步（目前定义从第二路抓拍信号接口接收同步信号）
	INT iSyncRelay;				// 外同步信号延迟：范围0-20000，单位为1us.

    INT iSaturationEnable;              /**< 饱和度使能 */
    INT iContrastEnalbe;                /**< 对比度使能*/
    //    INT iSharpenEnable;                 /**< 锐化度使能*/
    INT iWDRValue;                      /**  WDR强度值 0为不使能，值越大强度越大。取值范围：0~4095*/
    INT iSNFTNFMode;                    /** 0:不使能;1:TNF模式;2:SNF模式;3:TNF与SNF模式*/
    INT iTNFSNFValue;                     /** 降噪强度值0~3:自动、低、中、高*/
    INT iGrayImageEnable;                     /** 黑白图使能  0:不使能;1:使能*/
    INT iCVBSMode;                     /** CVBS制式 范围:0~1   0：PAL，1：NTSC*/
    INT iJpegAutoCompressEnable;        /** Jpeg自动调节开关 */
    INT iGammaValue;                  /** 相机伽玛 */
    INT iGammaEnable;
    INT iEdgeValue;                /** 图像边缘增强*/

	//金星常亮补光灯参数
	INT iLEDPolarity;				/**< 补光灯开关输出极性。1：正，0：负*/
	INT iLEDOutputType;				/**< 补光灯输出类型。1：上拉（电平）, 0:OC(开关)*/
	INT iLEDPluseWidth;              /**< 补光灯脉宽。0~255*/
	INT iALMPolarity;				/**< 报警输出极性。1：正，0：负*/
	INT iALMOutputType;				/**< 报警输出类型。1：上拉（电平）, 0:OC(开关)*/
	INT iF1OutputType;				/**< F1输出类型。1:闪光灯输出信号;0:栏杆机输出信号*/

	INT iFlashEnable;
	INT iFlashPolarity;
	INT iFlashOutputType;
	INT iFlashCoupling;
	INT iFlashPluseWidth;

	INT iColorGradation;			/**<色阶 0 ： 0~255；1：16~235 */

	INT iCVBSExport;				/**< CVBS输出图像模式。0:缩小模式, 1: 裁剪模式*/
	INT	iCVBSCropStartX;				/**< CVBS裁剪模式起始横坐标 */
	INT iCVBSCropStartY;				/**< CVBS裁剪模式起始纵坐标 */


	INT iIris;	/**< 光圈设定值，只对木星有效 */
	INT iAutoFocus;	/**< 自动聚焦值，只对木星有效 */
	INT iAWBMode;                     /**< AWB模式 ，只对木星有效 */
	INT iAEMode;                     /**< AE模式 ，只对木星有效*/
	INT iWDREnable;                     /**< WDR开关 ，只对木星有效*/
	INT iNRLevel;                     /**< 降噪等级，只对木星有效*/

	INT iAWBWorkMode;					/**< AWB工作模式:0,偏白模式,1:偏黄模式>*/
	INT iAutoControlCammeraAll;			/*< 全自动控制开关，0-手动控制，1-全自动控制*/
	_CamAppParam()
	{
		iJpegStream = 1;
		iH264Stream = 0;

		iIFrameInterval = 10;
		iTargetBitRate = 6*1024; //Kbps
		iMaxBitRate = 16 * 1024;  //16 * 1024 Kbps
		iFrameRate = 12;
		iResolution = 2;
		iRateControl = 0;       //0:VBR 1:CBR
		iVbrDuration = 0;
		iVbrSensivitity = 0;
		
		iIFrameIntervalSecond = 10;	  /**< 第二路H.264码流中的I帧间隔 */
		iTargetBitRateSecond = 6*1024;	  /**< 第二路H.264码流比特率 */
		iMaxBitRateSecond = 16 * 1024;
		iFrameRateSecond = 25;		  /**< 第二路H.264码流帧率 */
		iResolutionSecond = 0;		  /**< 第二路H.264图像分辨率 */
		iRateControlSecond = 0;
		iVbrDurationSecond = 0;
		iVbrSensivititySecond = 0;

		iAGCTh = 100;
		iAGCShutterLOri = 100;
        iAGCShutterHOri = 3000;
		iAGCGainLOri = 70;
        iAGCGainHOri = 150;
		iAGCScene = 0;

        iAGCDayNightShutterControl = 0;
        iAGCNightShutterHOri = 5000;

		iGainR = 0;
		iGainG = 0;
		iGainB = 0;
		iGain = iAGCGainLOri;
		iShutter = 2000;

		iIris = 5;
		iAutoFocus = 2;
		iAWBMode = 0;
		iAEMode = 0;
		iWDREnable = 0;
		iNRLevel = 0;

		iCaptureGainR = 36;
		iCaptureGainG = 36;
        iCaptureGainB = 36;
        iCaptureGain = 80;
        iCaptureShutter = 1000;
		iCaptureSharpenThreshold = 0xa0;
        iCaptureAutoParamEnable = 0;
        iEnableCaptureGainRGB = 0;
        iEnableCaptureGain = 1;
        iEnableCaptureShutter = 1;
		iEnableCaptureSharpen = 0;

		iJpegCompressRate = 60;
		iJpegCompressRateCapture = 80;


		swpa_strcpy(szNFSParam, "-o timeo=1,soft,tcp,nolock,retry=1");
		memset(szSafeSaverInitStr, 0, sizeof(szSafeSaverInitStr));

		iJpegExpectSize = 0;
		iJpegCompressRateL = 30;
		iJpegCompressRateH = 90;

		iJpegType = 0;
		iAGCEnable = 0;
		iAWBEnable = 1;
		iFlashRateSynSignalEnable = 0;
		iFlashRateMultiplication = 1;
		iFlashRatePolarity = 1;
		iFlashRatePulseWidth = 4000;
		iFlashRateOutputType = 0;
		iCaptureSynPolarity = 1;			
		iCaptureSynPulseWidth = 1000;		
		iCaptureSynOutputType = 1;			
		iCaptureSynCoupling = 0;			
		iCaptureSynSignalEnable = 1;
		iCaptureEdge = 0;
		iFlashDifferentLane = 0;
		iFlashDifferentLaneExt = 0;
		iEnableDCAperture = 0;
		iEnableSharpen = 0;
		iSharpenCapture = 1;
		iSharpenThreshold = 7;
		iColorMatrixMode = 1;
		iContrastValue = 0;
		iSaturationValue = 8;
		iTemThreshold = 32;
		iHueThreshold = 0;
		iHazeRemovalMode = 1;
		iHazeRemovalThreshold = 0;

		//strcpy(szNetDiskIP, "172.18.10.10");
		swpa_strcpy(szFtpRoot, "/array1");
		swpa_strcpy(szNetDiskUser, "No User");
		swpa_strcpy(szNetDiskPwd, "123");
		//strcpy(szNFS, "/volume1/nfs");
		//iNetDiskSpace = 1;

		iAvgY = 30;

		iGammaMode = 2;
		rgiGamma[0][0] = 32;
		rgiGamma[0][1] = 32;

		rgiGamma[1][0] = 64;
		rgiGamma[1][1] = 64;

		rgiGamma[2][0] = 96;
		rgiGamma[2][1] = 96;

		rgiGamma[3][0] = 128;
		rgiGamma[3][1] = 128;

		rgiGamma[4][0] = 160;
		rgiGamma[4][1] = 160;

		rgiGamma[5][0] = 192;
		rgiGamma[5][1] = 192;

		rgiGamma[6][0] = 222;
		rgiGamma[6][1] = 222;

		rgiGamma[7][0] = 255;
		rgiGamma[7][1] = 255;

		swpa_memcpy(rgiDefGamma, rgiGamma, 64);
		for(INT i = 0; i < 16; i++)
		{
			rgiAGCZone[i] = 1;
        }
       // swpa_memset(rgiTrafficLigtZone,0,sizeof(rgiTrafficLigtZone));
        iSaturationEnable = 0;
        iContrastEnalbe = 0;
        //iSharpenEnable = 0;
        iWDRValue = 256;		//默认为256
		iSNFTNFMode = 3;
        iTNFSNFValue = 0;
        iGrayImageEnable = 0;
		iCVBSMode = 0;                     /** CVBS制式 范围:0~1   0：PAL，1：NTSC*/
    	iJpegAutoCompressEnable = 0;        /** Jpeg自动调节开关 */
		iGammaValue = 10;
		iGammaEnable = 0;
        iEdgeValue = 0x80;

		iX = 0;
		iY = 0;
		iFontSize = 32;             // 注：默认是32点阵
		iDateFormat = 0;
		iFontColor = 0x00ff0000;    // 注：临时将默认值改为红色(0x00ff0000)
		swpa_strcpy(szCharater, "");
		iEnableCharacterOverlap = 0;
		iEnableFixedLight = 0;      // 注：默认情况，字体随场景亮度而改变

		fEnableH264BrightnessAdjust = FALSE;
		iAdjustPointX = 10;
		iAdjustPointY = 25;
		iEnableCCDSignalCheck = 1;

		iExternalSyncMode = 1;
		iSyncRelay = 2200;

		iLEDPolarity = 1;
		iLEDOutputType = 0;
		iLEDPluseWidth = 5;
		
		iALMPolarity = 1;
		iALMOutputType = 0;

        iF1OutputType = 1;
	
        iFlashEnable		= 1;
		iFlashPolarity		= 0;
        iFlashOutputType	= 1;
		iFlashCoupling  	= 0;
		iFlashPluseWidth	= 0;
		
		iColorGradation = 0;

		iCVBSExport = 1;
		iCVBSCropStartX = 600;
		iCVBSCropStartY = 252;

        iAWBWorkMode = 2;
		iAutoControlCammeraAll = 1;

	};

} CamAppParam;

typedef struct _FrontPannelParam
{
	int iUsedAutoControllMode;
	int iAutoRunFlag;
	int iPulseWidthMin;
	int iPulseWidthMax;
	int iPulseStep;
	int iPulseLevel;
	int iPolarizingPrismMode;
	int iPalanceLightStatus;
	int iFlash1PulseWidth;
	int iFlash1Polarity;
	int iFlash1Coupling;
	int iFlash1ResistorMode;
	int iFlash2PulseWidth;
	int iFlash2Polarity;
	int iFlash2Coupling;
	int iFlash2ResistorMode;
	int iFlash3PulseWidth;
	int iFlash3Polarity;
	int iFlash3Coupling;
	int iFlash3ResistorMode;
	int iDivFrequency;
	int iOutPutDelay;

	_FrontPannelParam()
	{
		iUsedAutoControllMode = 0;
		iAutoRunFlag = 1;
		iPulseWidthMin = 5;
		iPulseWidthMax = 42;
		iPulseStep = 5;
		iPulseLevel = 0;
		iPolarizingPrismMode = -1;
		iPalanceLightStatus = 0;
		iFlash1PulseWidth = 100;
		iFlash1Polarity = 0;
		iFlash1Coupling = 0;
		iFlash1ResistorMode = 0;
		iFlash2PulseWidth = 100;
		iFlash2Polarity = 0;
		iFlash2Coupling = 0;
		iFlash2ResistorMode = 0;
		iFlash3PulseWidth = 100;
		iFlash3Polarity = 0;
		iFlash3Coupling = 0;
		iFlash3ResistorMode = 0;
		iDivFrequency = 1;
		iOutPutDelay = 0;
	};
}FrontPannelParam;

typedef struct _OVERLAY
{
	BOOL fH264Eanble;
	BOOL fH264SecondEnable;	//第二路H264字符叠加使能
	H264_OVERLAY_INFO cH264Info;
	H264_OVERLAY_INFO cH264SecondInfo;	//第二路H264字符叠加参数
	JPEG_OVERLAY_INFO cJPEGInfo;
	_OVERLAY()
	{
		memset(this, 0, sizeof(*this));
		cJPEGInfo.iColor = 0x000000FF;
		cJPEGInfo.iFontSize = 32;
		swpa_strcpy(cH264Info.szInfo, "$(Time)  路口名称:$(RoadName)  路口方向:$(RoadDir)  设备编号:$(DevID)");
		swpa_strcpy(cH264SecondInfo.szInfo, "第二路H264流字符叠加样例");
		swpa_strcpy(cJPEGInfo.szInfo, "$(Time)  路口名称:$(RoadName)  路口方向:$(RoadDir)  $(PlateName)  设备编号:$(DevID)  防伪码:$(EncryptCode)");
	}
}OVERLAY;
#define NAME_LEN 32
typedef struct _GB28181PARAM
{
    BOOL fEnalbe;
    BOOL fVideoSaveEnable;
    CHAR szServerID[NAME_LEN];
    CHAR szServerRegion[NAME_LEN];
    CHAR szIPC_ID[NAME_LEN];
    CHAR szIPC_Region[NAME_LEN];
    CHAR szServerIP[NAME_LEN];
    CHAR szAlarmID[NAME_LEN];
    CHAR szPassWord[NAME_LEN];
    INT  iServerPort;
    INT  iIPC_Port;
    INT  iIPC_VideoPort;
    CHAR szLongitude[NAME_LEN];/*经度*/
    CHAR szLatitude[NAME_LEN];/*纬度*/
//    CHAR szDeviceName[NAME_LEN];
    CHAR szManufacturer[NAME_LEN];
//    CHAR szModel[NAME_LEN];/*设备型号*/
//    CHAR szFirmware[NAME_LEN];/*设备固件版本*/
    CHAR szBlock[NAME_LEN]; /*警区*/
    CHAR szAddress[NAME_LEN];/*安装地址*/
    CHAR szOwner[NAME_LEN]; /*设备归属*/
    CHAR szCivilCode[NAME_LEN];/*行政区域*/


    _GB28181PARAM()
    {
        swpa_memset(this, 0, sizeof(*this));
        fEnalbe = FALSE;
        fVideoSaveEnable = FALSE;

        swpa_strcpy(szServerID,"34020000002000000001");
        //swpa_strcpy(szServerRegion,"3402000000");
        swpa_strcpy(szIPC_ID,"34020000001320000001");
       // swpa_strcpy(szIPC_Region,"3402000000");
        swpa_strcpy(szServerIP,"172.18.11.123");
        swpa_strcpy(szAlarmID,"34020000001340000001");
        swpa_strcpy(szPassWord,"12345678");
        iServerPort = 5060;
        iIPC_Port = 5060;
        iIPC_VideoPort = 5080;

        swpa_strcpy(szLongitude,"0.0");
        swpa_strcpy(szLatitude,"0.0");

     //   swpa_strcpy(szDeviceName,"LS130200000");

        swpa_strcpy(szManufacturer,"NULL");
    //    swpa_strcpy(szModel,"EC200W");
   //     swpa_strcpy(szFirmware,"V1.0");
        swpa_strcpy(szBlock,"NULL");
        swpa_strcpy(szAddress,"NULL");
        swpa_strcpy(szOwner,"NULL");
        swpa_strcpy(szCivilCode,"NULL");

    }

}GB28181PARAM;

/* typedef struct _RTSP_PARAM */
/* { */
/* 	INT iMajorCommunicationMode;  // 主码流通迅模式  0:单播 1:组播 */
/* 	INT iMinorCommunicationMode;  // 副码流 */
/* 	INT iRTSPStreamNum; // RTSP传输Filter数量,即码流数量 */

/* 	_RTSP_PARAM() */
/* 	{ */
/* 		iMajorCommunicationMode = 0;  //默认是单播 */
/* 		iMinorCommunicationMode = 0;  //默认是单播 */
/* 		iRTSPStreamNum = 1; //默认是一路 */
/* 	} */
/* }RTSP_PARAM; */


typedef struct _AutoRebootPARAM
{
	BOOL fEnable;			/** 自动复位使能 范围:0~1   0：不使能，1：使能*/
	INT iCarLeftCountLimit;	/** 车流量限制 范围:0~1000 时间段内要求小于该值才能复位*/
	INT iSysUpTimeLimit;	/** 系统运行时长(单位小时h)限制 范围:24~720 系统运行时长超过该条件才考虑复位*/
	_AutoRebootPARAM()
	{
		fEnable = FALSE;			//默认不使能
		iCarLeftCountLimit = 50;	//平均一分钟5辆车
		iSysUpTimeLimit = 48;		//2*24
	}
	
}AutoRebootPARAM;

//识别设备最大的结构体
typedef struct _ModuleParams
{
	INT               nLogLevel;
	INT               nWorkModeIndex;
	INT 			  nCameraWorkMode;		//相机工作模式
	INT				  nMCFWUsecase;			//MCFW Usecase
	ResultSenderParam cResultSenderParam;
	TcpipParam        cTcpipCfgParam;
	IMG_FRAME_PARAM   cImgFrameParam;
	TRACKER_CFG_PARAM cTrackerCfgParam;
	CAM_CFG_PARAM     cCamCfgParam;

	CamAppParam       cCamAppParam;
	FrontPannelParam  cFrontPannelParam;

	MATCHPARAM        cMatchParam;
	DEVICEPARAM       cDevParam[2];

	OVERLAY           cOverlay;

    GB28181PARAM      cGb28181;
	RTSP_PARAM_INFO   cRtspParam;
	ONVIF_PARAM_INFO  cOnvifParam;
    INT               nMJPEGWidth;
    INT               nMJPEGHeight;
	AutoRebootPARAM	  cAutoRebootParam;
   	DOMEROCKER_PARAM cDomeRockerParam;  // 球机摇杆参数
	_ModuleParams()
	{
		nLogLevel = SW_LOG_LV_NORMAL;
		nWorkModeIndex = 0;
		nCameraWorkMode = 0;
		nMCFWUsecase = 0;
        nMJPEGWidth = 1920;
        nMJPEGHeight = 1080;
	}
} ModuleParams;

enum
{
	PRM_TOLLGATE,               //收费站
	PRM_HIGHWAY_HEAD,           //卡口 - 车头
	PRM_HIGHWAY_TAIL,           //卡口 - 车尾
	PRM_PATROL_LEFT,            //移动稽查 - 左侧
	PRM_PATROL_RIGHT,           //移动稽查 - 右侧
	PRM_PORTABLE,               //随意拍
	PRM_ELECTRONIC_POLICE,      //电子警察
	PRM_TRAFFICE_EVENT_HEAD,    //交通事件检测-车头
	PRM_TRAFFICE_EVENT_TAIL,    //交通事件检测-车尾
	PRM_HVC,	                  //抓拍识别
	PRM_PARK,                   //停车场双路
	PRM_PARK_SINGLE,            //停车场单路
	PRM_CAP_FACE,               //卡口人脸
	PRM_COVER_CTRL,             //断面控制器
	PRM_CAMERA,
	PRM_DOME_CAMERA,					//球形违停抓拍
	PRM_TRAFFICE_FACE,			//249 卡口人脸
	PRM_MAX
};

class CSWLPRParameter : public CSWParameter<ModuleParams>
{
	CLASSINFO(CSWLPRParameter, CSWObject)
public:
	enum{DELETE_LEVEL = 0, INNER_LEVEL = 1, PROJECT_LEVEL = 3, CUSTOM_LEVEL = 5};

	CSWLPRParameter();
	virtual ~CSWLPRParameter();
	virtual HRESULT Initialize(CSWString strFilePath);
	virtual HRESULT GetWorkMode(INT &iWorkMode, CSWString & strWorkKode);
	virtual LPCSTR  GetVersion(void){return (LPCSTR)m_strVersion;}

	virtual void ResetParam(VOID);
	/**
	*@brief 初始化system节点
	*@return 成功返回S_OK,其他值表示失败
	*/
	virtual HRESULT InitSystem();

	/**
	*@brief 
	*/
	virtual HRESULT InitTracker(VOID);

	/**
	*@brief 
	*/
	virtual HRESULT InitHvDsp(VOID);

	/**
	*
	*/
	virtual HRESULT InitOuterCtrl(VOID);

	/**
	*
	*/
	virtual HRESULT InitIPTCtrl(VOID);

	/**
	*
	*/
	virtual HRESULT InitCamApp(VOID);

	/**
	*
	*/
	virtual HRESULT Init368(VOID);

	/**
	*
	*/
	virtual HRESULT InitFrontPannel(VOID);

	/**
	*
	*/
	virtual HRESULT InitScaleSpeed(VOID);
	
	/**
	*
	*/
	virtual HRESULT InitCharacter(VOID);
    /**
    *
    */
    virtual HRESULT InitGB28181(VOID);

	/*
	 *@biref Init ONVIF Param
	 *@return On success, S_OK is returned 
	 */
	virtual HRESULT InitONVIF(VOID);

protected:
	HRESULT InitCamCfgParam(CAM_CFG_PARAM *pCfgCamParam, const CHAR **szLightType);
	BOOL CheckArea(INT nLeft, INT nTop, INT nRight, INT nBottom);
	void SetDefaultTracker();
	void SetDefaultCamApp();
	void SetDefaultCamCfgParam();
	void SetDefaultCharacter();
	void SetDefaultGB28181();
	void SetDefaultOnvif();
	void SetDefaultFrontPannel();
	void SetDefaultHvDSP();
	void SetDefaultIPTCtrl();
	void SetDefaultOuterCtrl();
	
	
	
protected:
	CSWString m_strWorkMode;
	CSWString m_strVersion;
};
#endif

