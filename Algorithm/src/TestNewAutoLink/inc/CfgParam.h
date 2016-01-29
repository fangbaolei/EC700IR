#pragma once

const int NOT_FIND = 9999;

typedef struct
{
	char szBuildNo[256];
	char szDate[128];
	char szTime[128];
	char szSysName[128];
	char szIPBackUp[16];
	int  nWorkMode;
}SOFT_VERSION_PARAM;

typedef struct _SYS_CONFIG_STRUCT {
	BYTE8 SysTargetID[8];
	BYTE8 SysNameLen;
	BYTE8 SysWorkMode;
	BYTE8 SysRecogFontMode;
	DWORD32 SysResetCount;
	BYTE8 VideoTriggerMode;
	DWORD32 VideoTriggerDelay;
	char SysName[256];
	bool fIsSysInit;
} SYS_CONFIG_STRUCT;

typedef struct _AREA_CTRL_PARAM
{
	int iEnableAreaUserCtrl;
	int iDetectorAreaLeft;
	int iDetectorAreaRight;
	int iDetectorAreaTop;
	int iDetectorAreaBottom;
	int iDetectorMinScaleNum;
	int iDetectorMaxScaleNum;
}AREA_CTRL_PARAM;


typedef struct _TRIGGER_PARAM
{
	//触发输入开关
	int iTriggerInEnable;
	//触发类型
	int iTriggerType;
	//触发模式发送多结果
	int iMultiResult;
	//触发输入延时
	int iTriggerInDelay;
	//
	int iTriggerInNoCarFlag;
	//
	int iTriggerOutEnable;
	//
	int iTriggerOutNormalStatus; 
	// 
	int iTriggerOutPlusWidth;


	//触发输入开关
	int iTriggerInEnable_2;
	//触发类型
	int iTriggerType_2;
	//触发模式发送多结果
	int iMultiResult_2;
	//触发输入延时
	int iTriggerInDelay_2;
	//
	int iTriggerInNoCarFlag_2;
	//
	int iTriggerOutEnable_2;
	//
	int iTriggerOutNormalStatus_2; 
	// 
	int iTriggerOutPlusWidth_2;
}TRIGGER_PARAM;
#ifndef MAX_HVC_CAMCOUNT
#define MAX_HVC_CAMCOUNT 8
#endif

typedef struct _VIDEO_PROCESS_PARAM
{
	// 是否使用全景抓拍模块.
	int iFullScreenCaptureEnable;

	// 全景抓拍模块使用模式.
	int iBestSnapshotOutput;
	int iLastSnapshotOutput;
	int iOutputCaptureImage;

	int iFullScreenCaptureImageBeginOutput;
	int iFullScreenCaptureImageBestOutput;
	int iFullScreenCaptureImageLastOutput;

	// 全景抓拍视频源端口值,支持视频口0,1,2,如果源自网络,则视为端口5588(十进制).
	int iFullScreenCapturePort;
	// 全景抓拍模块视频缓冲区器个数.
	int iFullScreenCaptureBuffNum;
	int iFullScreenCaptureCtrlBuffNum;
	int iFullScreenCaptureCtrlBuffMode;
	// 抓拍模块源亮度值,对比度,饱和度,色度值.
	int iFullScreenCaptureBrightness;
	int iFullScreenCaptureContrast;
	int iFullScreenCaptureSaturation;
	int iFullScreenCaptureSaturation_U;
	int iFullScreenCaptureSaturation_V;
	int iFullScreenCaptureHue;
	int iFullScreenCaptureMode;

	// 抓拍模块视频图象大小.
	int iFullScreenCaptureImageWidth;
	int iFullScreenCaptureImageHeight;
	// 抓拍模块其他参数.
	int iFullScreenCaptureImageFormat;
	int iFullScreenCaptureImageMaxWidth;
	int iFullScreenCaptureImageMaxHeight;

	// 系统工作主视频源端口号,支持视频口0,1,2,如果源自网络,则视为端口5588(十进制).
	int iMainVideoProcessPort;
	// 系统工作主视频源缓冲区个数.
	int iMainVideoProcessBuffNum;
	int iMainVideoProcessCtrlBuffNum;
	int iMainVideoProcessCtrlBuffMode;
	// 系统工作主视频源亮度值,对比度,饱和度,色度值.
	int iMainVideoProcessBrightness;
	int iMainVideoProcessContrast;
	int iMainVideoProcessSaturation;
	int iMainVideoProcessSaturation_U;
	int iMainVideoProcessSaturation_V;
	int iMainVideoProcessHue;
	int iMainVideoProcessMode;
	// 系统工作主视频图象大小.
	int iMainVideoProcessImageWidth;
	int iMainVideoProcessImageHeight;
	// 系统工作主视频其他参数.
	int iMainVideoProcessImageFormat;
	int iMainVideoProcessImageMaxWidth;
	int iMainVideoProcessImageMaxHeight;

	int iVideoDetEnable;
	int iVideoDetLeft;
	int iVideoDetRight;
	int iVideoDetTop;
	int iVideoDetBottom;
	int iVideoDetNum;
	int iVideoDetMode;
	int iVideoDet_00Top;
	int iVideoDet_00Bottom;
	int iVideoDet_00Left;
	int iVideoDet_00Right;
	int iVideoDet_01Top;
	int iVideoDet_01Bottom;
	int iVideoDet_01Left;
	int iVideoDet_01Right;
	int iVideoDet_02Top;
	int iVideoDet_02Bottom;
	int iVideoDet_02Left;
	int iVideoDet_02Right;
	int iEnableDetReverseRun;
	int iSpan;
	int iOutputReverseRun;
	int iSaveType;
	int iWidth;
	int iHeigh;
	int iOutputFrameImage;//标清结果大图输出类型:0=场,1=帧
	
	int iCapCamType;		// 抓拍相机类型 0:创宇J系列相机，1:创宇1355N
	int iCapCamNum;		// 抓拍摄像机个数
	int iCapCamDynaCfgEnable;	// 使能动态改变抓拍相机参数
	char szRoadIP_00[20];	// 车道0的摄像机IP
	int iSignalType_00;		// 车道0的信号源类型
	int iDayPlus_00;		// 车道0相机白天增益
	int iDayExposure_00;	// 车道0相机白天曝光时间
	int iNightfallPlus_00;	// 车道0相机阴天增益
	int iNightfallExposure_00;// 车道0相机阴天曝光时间
	int iNightPlus_00;		// 车道0相机晚上增益
	int iNightExposure_00;	// 车道0相机晚上曝光时间
	char szRoadIP_01[20];	// 车道1的摄像机IP
	int iSignalType_01;		// 车道1的信号源类型
	int iDayPlus_01;		// 车道1相机白天增益
	int iDayExposure_01;	// 车道1相机白天曝光时间
	int iNightfallPlus_01;	// 车道1相机阴天增益
	int iNightfallExposure_01;// 车道1相机阴天曝光时间
	int iNightPlus_01;		// 车道1相机晚上增益
	int iNightExposure_01;	// 车道1相机晚上曝光时间
	char szRoadIP_02[20];	// 车道2的摄像机IP
	int iSignalType_02;		// 车道2的信号源类型
	int iDayPlus_02;		// 车道2相机白天增益
	int iDayExposure_02;	// 车道2相机白天曝光时间
	int iNightfallPlus_02;	// 车道2相机阴天增益
	int iNightfallExposure_02;// 车道2相机阴天曝光时间
	int iNightPlus_02;		// 车道2相机晚上增益
	int iNightExposure_02;	// 车道2相机晚上曝光时间
	char szRoadIP_03[20];	// 车道3的摄像机IP
	int iSignalType_03;		// 车道3的信号源类型
	int iDayPlus_03;		// 车道3相机白天增益
	int iDayExposure_03;	// 车道3相机白天曝光时间
	int iNightfallPlus_03;	// 车道3相机阴天增益
	int iNightfallExposure_03;// 车道3相机阴天曝光时间
	int iNightPlus_03;		// 车道3相机晚上增益
	int iNightExposure_03;	// 车道3相机晚上曝光时间
	int iTriggerCycle;		// 网络触发抓拍摄像机周期，当该值为0时不定时触发
	int iMovingDirection;   //视频1行驶方向
	int iMovingDirection_2; //视频2行驶方向

	int iVideoDisplayTime;  //发送视频时间间隔(MS)
	int iSafeSaveEnable;    //可靠性保存开关
	int iHDVideoEnable;		//硬盘录像开关
	int iFormatDisk;		//格式化硬盘
	int iFDiskType;			//硬盘分区方式
	int iSendRecordSpace;	//结果发送间隔
	int iSendVideoTime;		//硬盘录像发送时间间隔
	int iDrawRect;			//发送图象中是否画红框
	int iHvcCamNum;			//HVC相机个数
	char rgszHvcCamIP[MAX_HVC_CAMCOUNT][16];	//HVC相机IP数组
	char rgszHvcCamParam[MAX_HVC_CAMCOUNT][64];	//HVC相机参数数组

	int iAviVideoHalf;		//AVI录像宽高各砍一半
}VIDEO_PROCESS_PARAM;

typedef struct _HVC_PARAM
{
	int iCapCamNum;		// 抓拍摄像机个数
	char rgCamIP[MAX_HVC_CAMCOUNT][20];	//抓拍相机IP
	int rgiSignalNum[MAX_HVC_CAMCOUNT]; 	//对应的信号源
	char rgHVCParam[MAX_HVC_CAMCOUNT][64];	//字符串形式的抓拍识别参数
}HVC_PARAM;

typedef struct _RADAR_PARAM
{
	// 雷达使能开关
	int fRadarUsed;
	// 雷达类型
	int iRadarType;
	//IDL4车检器关心的车速( 0:1,2线圈;  1:3,4线圈 ), 默认为0;
	int iIDL4Road;
	//RAPIER雷达设置车辆相对行驶方向( 0: 同向(车尾); 1:相向(车头) )
	int iRAPIERDirection;
	//RAPIER雷达安装的水平角度
	int iRAPIERAngle;

	// 该参数用来限制被测运动物体的最高速度限制,默认为180km/h.
	int iRadarLimitSpeed;
	// 该参数用来指示是否将车辆速度信息输出至上位机,默认为使能.
	int iCarSpeedOutputFlag;

	//特殊控制,使用测速仪时
	BOOL fRadarSpecialControl;

	// 雷达灵敏度，目前只对Decatur_SI3雷达有效
	int iSensitivity;
	// 雷达关心的物体运动方向,目前只对Decatur_SI3雷达有效
	// 0-所有方向, 1- 驶近方向, 2- 驶远方向
	int iTargetWay;
	// 雷达波段，0-Ka, 1-K,目前只对Decatur_SI3雷达有效
	int iBand;
	// 速度修正系数，以百分比方式存在，比如该值为110的时候就是乘1.1
	// 目前只对Decatur_SI3雷达有效
	int iCoefficient;

	int iSpeedZeroTimes;
	int iRadarDirectionMode;
	int iRadarWorkMode;
	int iRadarWorkstation;
	int iRadarRateAttentionType;
}RADAR_PARAM;

typedef struct _BLUE_BLACK_PARAM
{
	int iH0;
	int iH1;
	int iS;
	int iL;

	_BLUE_BLACK_PARAM()
	{
		iH0	= 100;
		iH1	= 200;
		iS	= 10;
		iL	= 85;
	}
}BLUE_BLACK_PARAM;

typedef struct _MODE_SET_PARAM
{
	int iWorkMode;
	int iFontMode;

	_MODE_SET_PARAM()
	{
		iWorkMode = 0;
		iFontMode	  = 0;
	}
}MODE_SET_PARAM;

typedef struct _SCALE_SPEED_PARAM
{
	int iScaleSpeedEnable;
	float fCameraHigh;
	float fFullDistance;
	float fLeftDistance;
	float fAdjustCoef;
	int iFlagMethods;
	int iEddyType;
	float fTransMatrix[11];

	_SCALE_SPEED_PARAM()
	{
		iScaleSpeedEnable = 0;
		fCameraHigh = 7.5f;
		fFullDistance = 75.0f;
		fLeftDistance = 25.0f;
		fAdjustCoef = 1.05f;
		iFlagMethods = 0;
		fTransMatrix[0] = 0.4970550537f;
		fTransMatrix[1] = -241.9865264893f;
		fTransMatrix[2] = -30.4937019348f;
		fTransMatrix[3] = 0.5036621094f;
		fTransMatrix[4] = -21.2472610474f;
		fTransMatrix[5] = -6.6555485725f;
		fTransMatrix[6] = -231.5502014160f;
		fTransMatrix[7] = 0.7746582031f;
		fTransMatrix[8] = 0.0300335791f;
		fTransMatrix[9] = -0.0085402671f;
		fTransMatrix[10] = -0.0195075944f;
	}
}SCALE_SPEED_PARAM;


typedef struct _CAM_CFG_PARAM
{
	char szIP[100];	
	int iPort; //测试协议连接端口
	int iCamType; // 摄像机协议类型：0-专用测试协议，1-Nvc摄像机协议，2-Camyu摄像机协议
	int iDynamicCfgEnable;
	int iDynamicTriggerEnable;
	int iTriggerDelay;
	int iDynamicCfgEnableOnTrigger;
	
	// 摄像机接收缓冲帧数
	int iJpegBufferCount;

	//自动填充参数(目前只对Camyu摄像机有效)
	int iAutoParamEnable;
	float fltDB;				//步长
	int iMinExposureTime;
	int iMinPlus;
	int iMaxExposureTime;
	int iMaxPlus;
	int iEnableAGC;
	int iMaxAGCLimit;
	int iMinAGCLimit;

	int iDayBrightness;
	int iDayAcutance;
	int iDaySturation;
	int iDayBlue;
	int iDayRed;
	char szDayScene[10];
	char szDayMode[10];
	int iDayExposureTime;
	int iDayWidth;
	int iDayHeight;
	int iDayAGCLimit;
	int iDayPlus;
	int iDayExposureTimeOnTrigger;
	int iDayPlusOnTrigger;

	int iDay2ExposureTime;
	int iDay2Plus;
	int iDay2Brightness;
	int iDay2AGCLimit;

	int iNightBrightness;
	int iNightAcutance;
	int iNightSturation;
	int iNightBlue;
	int iNightRed;
	char szNightScene[10];
	char szNightMode[10];
	int iNightExposureTime;
	int iNightWidth;
	int iNightHeight;
	int iNightAGCLimit;
	int iNightPlus;
	int iNightExposureTimeOnTrigger;
	int iNightPlusOnTrigger;

	int iNight2ExposureTime;
	int iNight2Plus;
	int iNight2Brightness;
	int iNight2AGCLimit;

	int iSuitlightBrightness;
	int iSuitlightAcutance;
	int iSuitlightSturation;
	int iSuitlightBlue;
	int iSuitlightRed;
	char szSuitlightScene[10];
	char szSuitlightMode[10];
	int iSuitlightExposureTime;
	int iSuitlightWidth;
	int iSuitlightHeight;
	int iSuitlightAGCLimit;
	int iSuitlightPlus;
	int iSuitlightExposureTimeOnTrigger;
	int iSuitlightPlusOnTrigger;

	int iSuitlight2ExposureTime;
	int iSuitlight2Plus;
	int iSuitlight2Brightness;
	int iSuitlight2AGCLimit;

	int iBacklightBrightness;
	int iBacklightAcutance;
	int iBacklightSturation;
	int iBacklightBlue;
	int iBacklightRed;
	char szBacklightScene[10];
	char szBacklightMode[10];
	int iBacklightExposureTime;
	int iBacklightWidth;
	int iBacklightHeight;
	int iBacklightAGCLimit;
	int iBacklightPlus;
	int iBacklightExposureTimeOnTrigger;
	int iBacklightPlusOnTrigger;

	int iBacklight2ExposureTime;
	int iBacklight2Plus;
	int iBacklight2Brightness;
	int iBacklight2AGCLimit;

	int iNightfallBrightness;
	int iNightfallAcutance;
	int iNightfallSturation;
	int iNightfallBlue;
	int iNightfallRed;
	char szNightfallScene[10];
	char szNightfallMode[10];
	int iNightfallExposureTime;
	int iNightfallWidth;
	int iNightfallHeight;
	int iNightfallAGCLimit;
	int iNightfallPlus;
	int iNightfallExposureTimeOnTrigger;
	int iNightfallPlusOnTrigger;

	int iNightfall2ExposureTime;
	int iNightfall2Plus;
	int iNightfall2Brightness;
	int iNightfall2AGCLimit;

	int iBigSuitlightBrightness;
	int iBigSuitlightAcutance;
	int iBigSuitlightSturation;
	int iBigSuitlightBlue;
	int iBigSuitlightRed;
	char szBigSuitlightScene[10];
	char szBigSuitlightMode[10];
	int iBigSuitlightExposureTime;
	int iBigSuitlightWidth;
	int iBigSuitlightHeight;
	int iBigSuitlightAGCLimit;
	int iBigSuitlightPlus;
	int iBigSuitlightExposureTimeOnTrigger;
	int iBigSuitlightPlusOnTrigger;

	int iBigSuitlight2ExposureTime;
	int iBigSuitlight2Plus;
	int iBigSuitlight2Brightness;
	int iBigSuitlight2AGCLimit;

	int iBigBacklightBrightness;
	int iBigBacklightAcutance;
	int iBigBacklightSturation;
	int iBigBacklightBlue;
	int iBigBacklightRed;
	char szBigBacklightScene[10];
	char szBigBacklightMode[10];
	int iBigBacklightExposureTime;
	int iBigBacklightWidth;
	int iBigBacklightHeight;
	int iBigBacklightAGCLimit;
	int iBigBacklightPlus;
	int iBigBacklightExposureTimeOnTrigger;
	int iBigBacklightPlusOnTrigger;

	int iBigBacklight2ExposureTime;
	int iBigBacklight2Plus;
	int iBigBacklight2Brightness;
	int iBigBacklight2AGCLimit;
}CAM_CFG_PARAM;

//HvDsp参数
typedef struct _HVDSP_PARAM 
{	
	BOOL fSysInitialized;

	BOOL fFirstSendPlate;

	BOOL fOutputReverseRun;

	BOOL fEnableDetReverseRun;
	BOOL fOutputAppendInfo;
	BOOL fOutputCarSize;		// 输出车辆尺寸

	BOOL fOutputObservedFrames;

	BOOL fOutputCarArriveTime;

	BOOL fOutputVoteCondition;

	BOOL fOutputVideoFrameName;//视频帧名输出开关

	int iBigImageCompressionRadio;

	int iVideoCompressionRadio;

	int fOutputFrameImage; 

	//输出逆行车辆
	int iOutputReverseRun;

	//是否在识别过程中显示视频.
	int iVideoTracker;		
	//从多少帧中显示一张视频图象.	
	int iVideoDisplayNum;
	// 多少毫秒发送一帧图像
	int iVideoDisplayTime;
	// 发送图象中是否画红框
	int iDrawRect;

	int iComTerminalOpen;

	//国标串口协议使能
	int iStdCommFlag;
	//国标串口协议使用的串口号
	int iStdCommNumber;
	//国标串口协议是否发送大图
	int iStdCommSendBigImageFlag;
	//国标串口协议是否发送二值图
	int iStdCommSendBinaryImageFlag;
	//国标串口协议是否发送小图
	int iStdCommSendSmallImageFlag;

	BOOL fDoLeach;
	BOOL fDoReplace;

	int nExportParamRank;
	int nExportRankInfo;

	BOOL fLoopReplace;

	// 界面参数
	int iLightBlueFlag;
	int iRecogGxPolice;
	int iOutputBrigtness;
	int iOutputFilterInfo;

	// 双板互联模式
	int iHostType;

	//GPS
	int iGPSEnable;			//使能开关
	int iGPSSyncSpaceS;		//校时的时间间隔(单位秒)
	
	//////////////////////////////////////////////////////////////////////////
	//上海交计特有参数
	// 连接识别器ＩＰ
	int iRecoDevCount;
	char rgszRecoDevAddr[8][20];
	// 是否过滤相同车牌
	int iTwinFilterFlag;
	int iTwinsTime;
	int iMaxPlateWait;
	char szSpecialFiltrateChar[20];

	int iOutputOnlyPeccancy;
	int iSaveVideoNum;

	//红绿灯图片加强参数
	int iImgTransEnable;
	int iTransBrightness;
	int iTransHueThreshold;
	int iTransCompressRate;

	//图片旋转开关
	int iEddyType;
	//过滤不合格违章结果
	int iFilterUnSurePeccancy;

	int iSendVideoTime;

	int iSendRecordSpace;

	//发送红灯坐标环境亮度阈值
	//环境亮度低于此值才发送红灯坐标
	int iRedPosBrightness;

	//主动连接参数
	int iAutoLinkEnable;
	char szAutoLinkIp[32];
	int iAutoLinkPort;

	// 事件检测周期，单位：分钟
	int iCheckEventTime; 
	int iEventOutputType; //是否实时输出事件检测信号

	//测试协议服务器开关
	int iTestServerEnable;

	//速度上限
	int iSpeedLimit;
	
	//输出车牌补光灯异常提示
	int iOutputFillLightStatus;

	//输出抓拍补光灯异常提示
	int iOutputFlashLampStatus;

	//车牌亮度最小值,小于这个值车牌补光灯可能异常
	int iPlateLightMin;

	//抓拍图和识别图亮度差值，小于这个值抓拍补光灯可能异常
	int iFlashLampDiff;

}HVDSP_PARAM;

const int MAX_ROAD_COUNT = 5;
typedef struct _OUTERCONTROL_PARAM
{
	int iEnable;
	int iPlateHoldMs;
	int iSignalHoldMs;
//	int iTackType;

	int iSignalTypeNumber;
	int iSignalSourceNumber;

	int iRoadNumber;
	int rgiRoadTopX[MAX_ROAD_COUNT];
	int rgiRoadTopY[MAX_ROAD_COUNT];
	int rgiRoadBottomX[MAX_ROAD_COUNT];
	int rgiRoadBottomY[MAX_ROAD_COUNT];

	int rgiSignalType[20];
	int iMainSignalType;

	int rgiSignalSoureMatchType[20];
	int rgiSignalSoureRoad[20];
	int rgiSignalSourePrevMs[20];
	int rgiSignalSourePostMs[20];
}OUTERCONTROL_PARAM;

typedef struct _IPT_PARAM
{
	//高清
	char szIPTCom[20];
	int rgiComSignalSource[4];
	int rgiOutSigSource[4];
	int rgiOutSigLevel[4];
	int rgiOutSigWidth[4];
	int rgiPortSignalSource0[4];
	int rgiPortSignalSource1[4];
	//标清
	int iCtrlTriggerSignalSource;
	int iNetTriggerSignalSource;
	int iCtrlSpeedSignalSource;

	//IPT配置参数
	BOOL fIPTPulse;             //判断IPT的脉冲宽度和电平这两个参数是否存在
	BOOL rgfDeviceUsed[4];	
	//IO口输入参数
	int iIoinType;				//IO口的类型
	int iIoinDelayTime;			//IO口的消除抖动延时
	int iIoinEffective;		//IO口有效输入位
	//线圈测速参数
	int iCoilType;				//线圈测速类型
	int iCoilDistance;			//前后线圈的距离
	int iCoilLimitSpeed;		//线圈测速的最小测量速度
	int iCoilCofSpeed;			//线圈测速的速度系数
	int iCoilDelayTime;			//线圈测速的消除抖动延时
	int iCoilTimeScale;			//线圈测速的时间时标
	//IO口输出参数
	int iIooutControlType;		//输出控制方式
	int iIooutEffective;		//有效输入位
	int iIooutDelayTime;		//消除抖动延时
	int iIooutControlSource;	//IO信号源和上位机控制源
	int iIooutSignalSource;		//IO输入源
	int iIooutOutDelay;			//输出延时
	int iIooutPolar;			//IO输入信号源极性
	int iIooutInitLevel;		//输出初始化电平
	int iIooutInput;			//IO输入反相设置
	//雷达测速参数
	int iRadarType;				//雷达测速类型
	int iRadarMeasureTime;		//测量时间间隔
	int iRadarCofSpeed;			//速度系数
	int iRadarLimitSpeed;		//最小数度限制
	int iRadarCarDirection;		//车辆运动方向
}IPT_PARAM;

typedef struct _OTHER_SIGNAL_SOURCE_PARAM
{
	// 软触发信号源编号
	int iSoftTriggerSignalSource;
}OTHER_SIGNAL_SOURCE_PARAM;

typedef struct _VIDEO_DETECTOR_PARAM
{
	int iRoadNumber;
	float ftRoadWidth;	
	int rgiGapLine_Top_X[6];
	int rgiGapLine_Bottom_X[6];
	int rgiGapLine_Top_Y[6];
	int rgiGapLine_Bottom_Y[6];

	int iVideoMaskNumber;
	int rgiVideoMask_Left[4];
	int rgiVideoMask_Right[4];
	int rgiVideoMask_Top[4];
	int rgiVideoMask_Bottom[4];

	int iPulseType;
	int iPulseWidth;
	int iOutputSignalLow;
	int iOutputSignalHight;

}VIDEO_DETECTOR_PARAM;

#ifndef MAX_LIGHT_COUNT
#define MAX_LIGHT_COUNT 10
#endif
#ifndef  MAX_SCENE_COUNT
#define MAX_SCENE_COUNT 10
#endif

typedef struct _TRAFFICLIGHT_PARAM
{
	int iEnable;
	int nLightCount;
	int nValidSceneStatusCount;
	int iStopLine;
	int iLeftStopLine;
	int iOnePos;
	int iTwoPos;
	int iThreePos;
	int iCheckType;
	char szIOConfig[64];
	char rgszLight[MAX_LIGHT_COUNT][64];
	int nSceneCount;
	char rgszScene[MAX_SCENE_COUNT][64];	
}TRAFFICLIGHT_PARAM;
typedef struct _ROADINFO_PARAM
{
	int iRoadNum;
	int iRoadNumBegin;			//车道编号起始，0：从左开始，1：从右开始
	int iStartRoadNum;			//起始车道号,0:从0开始,1:从1开始
	int rgiTop_X[5];
	int rgiTop_Y[5];
	int rgiBottom_X[5];
	int rgiBottom_Y[5];
	int rgiRoadType[5];
}ROADINFO_PARAM;

//#ifndef MAX_IDENTIFY_PASSWORD_LENGTH
//#define MAX_IDENTIFY_PASSWORD_LENGTH 256
//#endif
#ifndef MAX_STREETINFO_LENGTH
#define MAX_STREETINFO_LENGTH 255
#endif
typedef struct _IDENTIFYPASSWORD_PARAM
{
	int iPasswordLength;
	char rgszPassword[MAX_IDENTIFY_PASSWORD_LENGTH];
	char rgszStreetName[MAX_STREETINFO_LENGTH];
	char rgszStreetDirection[MAX_STREETINFO_LENGTH];
}IDENTIFYPASSWORD_PARAM;

typedef struct _ACTIONDETECT_PARAM
{
	int iDetectCarStopEnable;
	int iDetectReverseEnable;
	int iOutputReverseEnable;
	int iDetectOverYellowLineEnable;
	int iIsYellowLine[5];		//0-01车道，1-12车道，以此类推
	int iDetectCrossLineEnable;
	int iIsCrossLine[5];		//0-01车道，1-12车道，以此类推
	float ftOverLineSensitivity;   //压线检测灵敏度
}ACTIONDETECT_PARAM;


