/**
* @file SWNetCommandProcess.cpp 
* @brief 网络命令处理模块
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-04-01
* @version 1.0
*/

#include "SWFC.h"
#include "swpa.h"
#include "SWMessage.h"
#include "tinyxml.h"
#include "SWNetCommandProcess.h"
#include "SWDomeCameraBase.h"

#define PRINT //SW_TRACE_NORMAL//printf("%s L%d:",__FUNCTION__, __LINE__) && printf
//#define PRINT printf
//#define PRINT_XML

enum {
	NET_CMD_PROTOCOL_XML,
	
};

// 命令包头
typedef struct tag_CameraCmdHeader
{
    DWORD dwID;
    DWORD dwInfoSize;
}
CAMERA_CMD_HEADER;



// 命令回应包
typedef struct tag_CameraCmdRespond
{
	DWORD dwID;
	DWORD dwInfoSize;
	INT iResult;
}
CAMERA_CMD_RESPOND;




//命令名字与ID映射表
typedef struct __NETCMDMAPPINGTABLE
{
	const CHAR * 	szCmdName;
	DWORD			dwCmdID;
}NET_CMD_MAPPING_TABLE;


typedef struct __NETCMDMPROCESSARG
{
	CSWNetCommandProcess* 	pThis;
	SWPA_SOCKET_T			sSock;
}_NETCMDMPROCESSARG;


#define HV_XML_CMD_VERSION_NO "3.0"


#define HVXML_VER 					"Ver"
#define HVXML_HVCMD					"HvCmd"
#define HVXML_HVCMDRESPOND 			"HvCmdRespond"
#define HVXML_RETCODE 				"RetCode"
#define HVXML_RETMSG 				"RetMsg"
#define HVXML_CMDNAME				"CmdName"
#define HVXML_TYPE 					"Type"
#define HVXML_VALUE 				"Value"
#define HVXML_CLASS 				"Class"
#define HVXML_GETTER 				"GETTER"
#define HVXML_SETTER 				"SETTER"
#define HVXML_RETURNLEN				"RetLen"


static struct __NETCMDMAPPINGTABLE g_XmlCmdMappingTable[] =
{
    // 相机参数控制模块
    //{"", MSG_CAMERA_CONTROL_START},
    {"SetGain", 			MSG_SET_AGCGAIN},//增益
    {"SetAGCEnable", 		MSG_SET_AGCENABLE},
    {"GetAGCEnable",        MSG_GET_AGCENABLE},
    {"GetGain",				MSG_GET_AGCGAIN},//增益
    {"SetShutter", 			MSG_SET_SHUTTER},// Shutter 快门
    {"GetShutter", 			MSG_GET_SHUTTER},
    {"SetRGBGain", 			MSG_SET_RGBGAIN},//设置RGB INT[3];
    {"SetAWBEnable", 		MSG_SET_AWBENABLE},	// AWB 白平衡
    {"GetAWBEnable",    MSG_GET_AWBENABLE},
    {"GetRGBGain", 			MSG_GET_RGBGAIN},//获取RGB INT[3];
    {"SetGammaData", 		MSG_SET_GAMMA},//lut
//    {"GetGammaData", 		MSG_GET_GAMMA},//new
//	{"SetEncodeMode",   MSG_SET_GAMMA_MODE},
//  {"GetEncodeMode",   MSG_GET_GAMMA_MODE},
    {"SetDiffCorrEnable", 	MSG_SET_DIFFCORRENABLE},//DC开关
    {"SetCaptureEnable", 	MSG_SET_CAPTURE_ENABLE},
	{"SetCaptureAutoParamEnable", MSG_SET_CAPTURE_AUTO_PARAM_ENABLE},
	{"GetCaptureAutoParamEnable", MSG_GET_CAPTURE_AUTO_PARAM_ENABLE},
    {"SetCaptureShutter", 	MSG_SET_CAPTURESHUTTER},
    {"GetCaptureShutter", 	MSG_GET_CAPTURESHUTTER},
    {"SetCaptureGain", 		MSG_SET_CAPTRUEGAIN},
    {"GetCaptureGain", 		MSG_GET_CAPTUREGAIN},
    {"SetCaptureRgbGain",	MSG_SET_CAPTRUERGBGAIN},
    {"GetCaptureRgbGain", 	MSG_GET_CAPTRUERGBGAIN},
    {"SetAGCParam", 		MSG_SET_AGCPARAM},
    {"GetAGCParam", 		MSG_GET_AGCPARAM},//new
    {"SetAgcLightBaseline", MSG_SET_AGCTH},
    {"GetAgcLightBaseline", MSG_GET_AGCTH},
    {"SetAGCZone", 			MSG_SET_AGCZONE},
    {"GetAGCZone", 			MSG_GET_AGCZONE},//new
    {"SetAEScene",			MSG_SET_AGC_SCENE},
    {"GetAEScene",			MSG_GET_AGC_SCENE},
    {"SetCtrlCpl",			MSG_SET_FILTERSWITCH},//滤光片控制new
    {"SetDCAperture",		MSG_SET_DCAPERTUREENABLE},//DC光圈
    {"GetDCAperture",MSG_GET_DCAPERTUREENABLE}, //NEW
    {"SetSharpness",        MSG_SET_SHARPENTHRESHOLD},//锐度new
    {"GetSharpness",        MSG_GET_SHARPENTHRESHOLD},//锐度new

    {"SetSharpnessEnable",MSG_SET_SHARPEN_ENABLE},//锐度使能new
    {"GetSharpnessEnable",MSG_GET_SHARPEN_ENABLE},//锐度使能new

    {"SetSaturation",       MSG_SET_SATURATIONTHRESHOLD},//饱和度new
    {"GetSaturation",       MSG_GET_SATURATIONTHRESHOLD},//饱和度new
    {"SetContrast",         MSG_SET_CONTRASTTHRESHOLD},//对比度new
    {"GetContrast",         MSG_GET_CONTRASTTHRESHOLD},

    {"SetWDRLevel",MSG_SET_WDR_STRENGTH},    //设置WDR等级
    {"GetWDRLevel",MSG_GET_WDR_STRENGTH},

    {"SetEnableGrayImage",MSG_SET_GRAY_IMAGE_ENABLE},               //开关黑白图模式
    {"GetEnableGrayImage",MSG_GET_GRAY_IMAGE_ENABLE},

    {"SetDeNoiseMode",MSG_SET_DENOISE_MODE}, //DNR模式
    {"GetDeNoiseMode",MSG_GET_DENOISE_MODE},
    {"SetDeNoiseLevel",MSG_SET_DENOISE_STATE},  //DNR等级
    {"GetDeNoiseLevel",MSG_GET_DENOISE_STATE},

    {"ZoomDCIRIS",MSG_ZOOM_DCAPERTURE},//new 光圈放大缩小
    {"ShrinkDCIRIS",MSG_SHRINK_DCAPERTURE},

    {"SetEnRedLightEnable",MSG_SET_REDLIGHT_ENABLE},//红灯
    {"GetEnRedLightEnable",MSG_GET_REDLIGHT_ENABLE},
    {"SetRedLightRect",MSG_SET_REDLIGHT_RECT},

    {"SetCVBSDisplayMode",MSG_SET_CVBS_STD},
    {"GetCVBSDisplayMode",MSG_GET_CVBS_STD},

	//IO测试
    {"SetF1IO",MSG_SET_TRIGGEROUT},
    {"GetF1IO",MSG_GET_TRIGGEROUT},
    {"SetEXPIO",MSG_SET_EXP_IO},

	{"SetF1TriggerOut",		MSG_SET_F1_TRIGGEROUT},		//F1抓拍输出
	{"GetF1TriggerOut",		MSG_GET_F1_TRIGGEROUT},		//F1抓拍输出
	{"SetAlarmTriggerOut",	MSG_SET_ALM_TRIGGEROUT},	//ALM口抓拍输出
	{"GetAlarmTriggerOut",	MSG_GET_ALM_TRIGGEROUT},	//ALM口抓拍输出


    // todo.
    // 不响应此接口。
  //  {"GetEXPIO",MSG_GET_EXP_IO},

    {"SetALMIO",MSG_SET_ALARM_IO},
    {"GetALMIO",MSG_GET_ALARM_IO},
    {"SetTGIO",MSG_SET_TG_IO},
    {"GetTGIO",MSG_GET_TG_IO},
		
    {"AutoTestCamera",MSG_SET_CAM_TEST},

    //{"SetEncodeMode",		MSG_RECOGNIZE_OUTPUTDEBUG},
    {"SetExpPluseWidth",  MSG_SET_EXP_PLUSEWIDTH},
    {"SoftTriggerCapture",	MSG_SET_SOFT_TOUCH},
    {"SetFlashRateSynSignalEnable",	MSG_SET_FLASHRATE_ENABLE},
    {"SetCaptureSynSignalEnable",	MSG_SET_CAPTURESYNC_ENABLE},//设置抓捕同步信号开关
    {"SendTriggerOut", MSG_TRIGGER_OUT_PROCESS},

    {"SetCaptureEdge", MSG_SET_CAPTUREEDGE},
    {"GetCaptureEdge", MSG_GET_CAPTUREEDGE},

    {"SetEnRedLightThreshold",MSG_SET_TRAFFIC_LIGTH_LUM_TH},  //设置红灯加红阈值    INT
//    {"GetEnRedLightThreshold ",MSG_GET_TRAFFIC_LIGTH_LUM_TH}, //获取红灯加红阈值    INT
//???

    {"SetImageEnhancementEnable",MSG_SET_COLORMATRIX_ENABLE},// 设置图像增强使能    INT
    {"GetImageEnhancementEnable",MSG_GET_COLORMATRIX_ENABLE},// 获取图像增强使能    INT

    {"SetGAMMAValue",MSG_SET_GAMMA_STRENGTH},
    {"GetGAMMAValue",MSG_GET_GAMMA_STRENGTH},

    {"SetGAMMAEnable",MSG_SET_GAMMA_ENABLE},
    {"GetGAMMAEnable",MSG_GET_GAMMA_ENABLE},


    {"WriteFPGA",MSG_SET_CAM_FPGA_REG},
    {"ReadFPGA",MSG_GET_CAM_FPGA_REG},

    {"SetEdgeEnhance",MSG_SET_EDGE_ENHANCE},//设置图像边缘增强
    {"GetEdgeEnhance",MSG_GET_EDGE_ENHANCE},

    {"GetCameraWorkState",MSG_GET_CAMERA_WORKSTATE},

    //{"", MSG_CAMERA_CONTROL_END},

    //识别模块
    {"SetImgType", MSG_RECOGNIZE_OUTPUTDEBUG},

	 //JPEG压缩命令
	{"JPEGEncode", 			MSG_JPEGENCODE},
	{"SetJpegCompressRate", MSG_JPEGENCODE_COMPRESSRATE},
	{"GetJpegCompressRate", MSG_JPEGENCODE_GET_COMPRESSRATE},
	{"SetJpegCompressRateCapture", MSG_JPEGENCODE_COMPRESSRATE_CAPTURE},
	{"GetJpegCompressRateCapture", MSG_JPEGENCODE_GET_COMPRESSRATE_CAPTURE},
	{"SetMJPEGRect", MSG_SET_JPEG_COMPRESS_RECT},
	{"SetColorGradation", MSG_SET_COLOR_GRADATION},
//    {"SetJpegCompressRate", MSG_SET_JPEG_QUALITY},
 //   {"GetJpegCompressRate", MSG_GET_JPEG_QUALITY},//临时
	
	//自动化控制命令
	//{"", MSG_AUTO_CONTROL_START},
	//{"", MSG_AUTO_CONTROL_END},

	//Application
	//{"", MSG_APP_START},
	{"SetTime", 			MSG_APP_SETTIME},
	{"DateTime", 			MSG_APP_GETTIME},
	{"SetIP", 				MSG_APP_SETIP},
	{"GetIP",	 			MSG_APP_GETIP},
	{"SetXML", 				MSG_APP_SETXML},
	{"GetXML", 				MSG_APP_GETXML},
	{"OptWorkModeCount",	MSG_APP_GETWORKMODE_COUNT},
	{"OptWorkMode", 		MSG_APP_GETWORKMODE},
	{"GetWorkModeIndex",    MSG_APP_GETWORKMODEINDEX},
	{"ResetDevice", 		MSG_APP_RESETDEVICE},
	{"RestoreDefaultParam", MSG_APP_RESTORE_DEFAULT},
	{"RestoreFactoryParam", MSG_APP_RESTORE_FACTORY},
	{"SetCharacterEnable", 	MSG_APP_SETCHARACTER_ENABLE},
	{"GetCharacterEnable", 	MSG_APP_GETCHARACTER_ENABLE},
	{"HvName", 				MSG_APP_GETHVNAME},
	{"GetHVID", 			MSG_APP_GETHVID},
	{"OptProductName", 		MSG_APP_GETPRODUCT_NAME},
	{"GetVideoCount", 		MSG_APP_GETVIDEO_COUNT},
	{"GetVersion", 			MSG_APP_GETVERSION},
	{"GetVersionString", 	MSG_APP_GETVRESION_STRING},
	{"GetConnectedIP", 		MSG_APP_GETCONNECTED_IP},
	{"HDDFdisk", 			MSG_APP_HDD_FDISK},
	{"HDDCheck", 			MSG_APP_HDD_CHECK},
	{"HDDStatus", 			MSG_APP_HDD_STATUS},
	{"OptResetCount", 		MSG_APP_RESET_COUNT},
	{"OptResetMode", 		MSG_APP_RESET_MODE},
	{"GetDevType", 			MSG_APP_GETDEVTYPE},
	{"GetCpuTemperature", 	MSG_APP_GETCPU_TEMPERATURE},
	{"GetResetReport",		MSG_APP_RESET_REPORT},//复位信息
	{"GetRunStatus",		MSG_APP_GET_RUN_STATUS},

    {"SetNTPServerIP",MAG_APP_SET_NTP_SERVER_IP},                    //ntp NEW
    {"GetNTPServerIP",MAG_APP_GET_NTP_SERVER_IP},
    {"SetTimeZone",MAG_APP_SET_NTP_TIMEZONE},
    {"GetTimeZone",MAG_APP_GET_NTP_TIMEZONE},
    {"SetNTPEnable",MAG_APP_SET_NTP_ENABLE},
    {"GetNTPEnable",MAG_APP_GET_NTP_ENABLE},
    {"SetNTPServerUpdateInterval",MAG_APP_SET_NTP_INTERVAL},
    {"GetNTPServerUpdateInterval",MAG_APP_GET_NTP_INTERVAL},

    {"SetTraceRank",MSG_APP_SET_LOG_LEVEL},
    {"GetTraceRank",MSG_APP_GET_LOG_LEVEL},
    {"GetBlackBoxMessage",	MSG_APP_GET_BLACKBOX_MESSAGE},

	{"SetCustomizedDevName",MSG_APP_SET_CUSTOMIZED_DEVNAME},
    {"GetCustomizedDevName",MSG_APP_GET_CUSTOMIZED_DEVNAME},

    {"SetAWBWorkMode", MSG_APP_SET_AWB_WORK_MODE},
    {"GetAWBWorkMode", MSG_APP_GET_AWB_WORK_MODE},
    
    {"SetAutoControlCammeraAll", MSG_APP_SET_AUTO_CONTROL_CAMMERA_ALL},
    {"GetAutoControlCammeraAll", MSG_APP_GET_AUTO_CONTROL_CAMMERA_ALL},
    
    {"SetH264Resolution", MSG_APP_SET_H264_RESOLUTION},
    {"GetH264Resolution", MSG_APP_GET_H264_RESOLUTION},

    {"SetComTestEnable",MSG_COM_TEST_ENABLE},
    {"SetComTestdisable",MSG_COM_TEST_DISABLE},



    //{"GetTime", 		MSG_APP_END},
    //用户管理
    {"DoLogin",         MSG_USER_LOGIN},
    {"AddUser",         MSG_USER_ADD},
    {"DelUser",         MSG_USER_DEL},
    {"ModUser",         MSG_USER_CHANGE},
    {"GetUsers",        MSG_USER_GETLIST},
    //字幕
	//第一路H264
    {"SetOSDh264Enable",MSG_OSD_SET_H264_ENABLE},//开关叠加OSD功能
    {"SetOSDh264PlateEnable",MSG_OSD_SET_H264_PLATE_ENABLE},
    {"SetOSDh264TimeEnable",MSG_OSD_SET_H264_TIME_ENABLE},//使能叠加时间
    {"SetOSDh264Text",MSG_OSD_SET_H264_TEXT},//设置叠加文字
    {"SetOSDh264Font",MSG_OSD_SET_H264_FONNTSIZE},//设置字体大小
    {"SetOSDh264FontRGB",MSG_OSD_SET_H264_FONT_RBG},
    {"SetOSDh264Pos",MSG_OSD_SET_H264_POX},//设置叠加坐标
	//第二路H264
	{"SetOSDh264SecondEnable",MSG_OSD_SET_H264_SECOND_ENABLE},//开关叠加OSD功能
    {"SetOSDh264SecondPlateEnable",MSG_OSD_SET_H264_SECOND_PLATE_ENABLE},
    {"SetOSDh264SecondTimeEnable",MSG_OSD_SET_H264_SECOND_TIME_ENABLE},//使能叠加时间
    {"SetOSDh264SecondText",MSG_OSD_SET_H264_SECOND_TEXT},//设置叠加文字
    {"SetOSDh264SecondFont",MSG_OSD_SET_H264_SECOND_FONNTSIZE},//设置字体大小
    {"SetOSDh264SecondFontRGB",MSG_OSD_SET_H264_SECOND_FONT_RBG},
    {"SetOSDh264SecondPos",MSG_OSD_SET_H264_SECOND_POX},//设置叠加坐标
    
    {"SetOSDjpegEnable",MSG_OSD_SET_JPEG_ENABLE},//开关叠加OSD功能
    {"SetOSDjpegPlateEnable",MSG_OSD_SET_JPEG_PLATE_ENABLE},
    {"SetOSDjpegTimeEnable",MSG_OSD_SET_JPEG_TIME_ENABLE},//使能叠加时间
    {"SetOSDjpegText",MSG_OSD_SET_JPEG_TEXT},//设置叠加文字
    {"SetOSDjpegFont",MSG_OSD_SET_JPEG_FONNTSIZE},//设置字体大小、
    {"SetOSDjpegFontRGB",MSG_OSD_SET_JPEG_FONT_RBG},	//设置字体颜色
    {"SetOSDjpegPos",MSG_OSD_SET_JPEG_POX},//设置叠加坐标

    {"GetH264Caption",MSG_GET_H264_CAPTION},
    {"GetH264SecondCaption",MSG_GET_H264_SECOND_CAPTION},	//获取第二路H264参数
    {"GetJPEGCaption",MSG_GET_JPEG_CAPTION},
	//H264码流设置
	//主码流
    {"SetH264BitRate",MSG_SET_H264_BITRATE},
    {"GetH264BitRate",MSG_GET_H264_BITRATE},
	{"SetH264IFrameInterval", MSG_SET_H264_I_FRAME_INTERVAL},
	{"GetH264IFrameInterval", MSG_GET_H264_I_FRAME_INTERVAL},
	{"SetH264BitRateControl", MSG_SET_H264_RATE_CONTROL},
	{"SetH264VbrDuration", MSG_SET_H264_VBR_DURATION},
	{"SetH264VbrSensitivity", MSG_SET_H264_VBR_SENSITIVITY},
	//第二路码流
    {"SetH264SecondBitRate",MSG_SET_H264_SECOND_BITRATE},
    {"GetH264SecondBitRate",MSG_GET_H264_SECOND_BITRATE},
	{"SetH264SecondIFrameInterval", MSG_SET_H264_SECOND_I_FRAME_INTERVAL},
	{"GetH264SecondIFrameInterval", MSG_GET_H264_SECOND_I_FRAME_INTERVAL},
	{"SetH264SecondBitRateControl", MSG_SET_H264_SECOND_RATE_CONTROL},
	{"SetH264SecondVbrDuration", MSG_SET_H264_SECOND_VBR_DURATION},
	{"SetH264SecondVbrSensitivity", MSG_SET_H264_SECOND_VBR_SENSITIVITY},

    {"GetDebugJpegStatus",MSG_GET_DEBUG_JPEG_STATUS},//获取调试码流开关
    {"SetDebugJpegStatus",MSG_SET_DEBUG_JPEG_STATUS},
    {"SetAutoJpegCompressEnable",MSG_SET_AUTO_JPEG_COMPRESS_ENABLE},
    {"GetAutoJpegCompressEnable",MSG_GET_AUTO_JPEG_COMPRESS_ENABLE},//自动jpeg压缩使能       INT
    {"SetAutoJpegCompressParam",MSG_SET_AUTO_JPEG_COMPRESS_PARAM},//获取自动压缩图片的上限、下限、图片大小  		一维数组 3
    {"GetAutoJpegCompressParam",MSG_GET_AUTO_JPEG_COMPRESS_PARAM},//获取自动压缩图片的上限、下限、图片大小  		一维数组 3

	{"SetMJPEGOverlay",MSG_OSD_SET_MJPEG_OVERLAY},	//使能或关闭MJPEG字符叠加，适用于MJPEG图片外叠加字符的情况，与字符叠加开关不是同一概念

     //网络模块
    {"SetSyncPower",MSG_SET_SYNC_POWER},
    {"GetSyncPower",MSG_GET_SYNC_POWER},
    {"GetDevState",MSG_GET_DEVSTATE},//获取设备CPU、内存、温度、连接的上位机IP列表、设备时间、硬盘/SD卡使用状态，复位次数。
    {"GetVideoState",MSG_GET_VIDEO_STATE},
    {"GetDevBasicInfo",MSG_GET_DEV_BASIC_INFO},
    {"GetCameraBasicInfo",MSG_GET_CAMERA_BASIC_INFO},
    {"GetCameraState",MSG_GET_CAMERA_STATE},

    {"SetVedioRequestControl",MSG_H264_DO_PREVIEW},


	//球机控制
	{"SetPanCoordinate", MSG_SET_PAN_COORDINATE},
	{"GetPanCoordinate", MSG_GET_PAN_COORDINATE},
	{"SetTiltCoordinate", MSG_SET_TILT_COORDINATE},
	{"GetTiltCoordinate", MSG_GET_TILT_COORDINATE},
	{"SetZoom", MSG_SET_ZOOM},
	{"GetZoom", MSG_GET_ZOOM},
	{"StartPan", MSG_START_PAN},
	{"StopPan", MSG_STOP_PAN},
	{"StartInfinityPan", MSG_START_INFINITY_PAN},
	{"StopInfinityPan", MSG_STOP_INFINITY_PAN},
	{"StartTilt", MSG_START_TILT},
	{"StopTilt", MSG_STOP_TILT},
	{"StartPanAndTilt", MSG_START_PAN_TILT},
	{"StopPanAndTilt", MSG_STOP_PAN_TILT},
	{"SetPanSpeed", MSG_SET_PAN_SPEED},
	{"GetPanSpeed", MSG_GET_PAN_SPEED},
	{"SetTiltSpeed", MSG_SET_TILT_SPEED},
	{"GetTiltSpeed", MSG_GET_TILT_SPEED},
	{"SetFocus", MSG_SET_FOCUS},
	{"GetFocus", MSG_GET_FOCUS},
	{"SetAF", MSG_SET_AF_ENABLE},
	{"GetAF", MSG_GET_AF_ENABLE},
	{"SetFocusMode", MSG_SET_FOCUS_MODE},
	{"GetFocusMode", MSG_GET_FOCUS_MODE},
	{"SetIris", MSG_SET_IRIS},
	{"GetIris", MSG_GET_IRIS},
	{"SetIrisMode", MSG_SET_IRIS_MODE},
	{"GetIrisMode", MSG_GET_IRIS_MODE},
	{"StartVTurnOver", MSG_START_VTURNOVER},
	{"StopVTurnOver", MSG_STOP_VTURNOVER},
	{"StartWiper", MSG_START_WIPER},
	{"StopWiper", MSG_STOP_WIPER},
	{"SetWiperSpeed", MSG_SET_WIPER_SPEED},
	{"GetWiperSpeed", MSG_GET_WIPER_SPEED},
	{"SetWiperAutoStartTime", MSG_SET_WIPER_AUTOSTART_TIME},
	{"GetWiperAutoStartTime", MSG_GET_WIPER_AUTOSTART_TIME},
	{"SetMaxPanSpeed", MSG_SET_MAX_PAN_SPEED},
	{"GetMaxPanSpeed", MSG_GET_MAX_PAN_SPEED},
	{"SetMaxTiltSpeed", MSG_SET_MAX_TILT_SPEED},
	{"GetMaxTiltSpeed", MSG_GET_MAX_TILT_SPEED},
	{"TuneZoom", MSG_TUNE_ZOOM},
	{"TuneIris", MSG_TUNE_IRIS},
	{"TuneFocus", MSG_TUNE_FOCUS},
	{"ResetDomePosition", MSG_RESET_DOME_POSITION},
	{"StartZoom", MSG_START_ZOOM},
	{"StopZoom", MSG_STOP_ZOOM},


	{"SetPresetPosition", MSG_SET_PRESET_POS},
	{"GetPresetPosition", MSG_GET_PRESET_POS},
	{"CallPresetPosition", MSG_CALL_PRESET_POS},
	{"ClearPresetPosition", MSG_CLEAR_PRESET_POS},
	{"SetPresetCheckPTFlag", MSG_SET_PRESET_CHECKPT_FLAG},
	{"GetPresetCheckPTFlag", MSG_GET_PRESET_CHECKPT_FLAG},
	{"GetPresetNameList", MSG_GET_PRESET_NAME_LIST},


	{"SetHScanLeftPos", MSG_SET_HSCAN_LEFT_POS},
	{"GetHScanLeftPos", MSG_GET_HSCAN_LEFT_POS},
	{"SetHScanRightPos", MSG_SET_HSCAN_RIGHT_POS},
	{"GetHScanRightPos", MSG_GET_HSCAN_RIGHT_POS},
	{"SetHScanParam", MSG_SET_HSCAN_PARAM},
	{"GetHScanParam", MSG_GET_HSCAN_PARAM},
	{"StartHScan", MSG_START_HSCAN},
	{"StopHScan", MSG_STOP_HSCAN},
	{"ClearHScan", MSG_CLEAR_HSCAN},

	{"StartFScanRecord", MSG_START_FSCAN_RECORD},
	{"StopFScanRecord", MSG_STOP_FSCAN_RECORD},
	{"CallFScan", MSG_CALL_FSCAN},
	{"StopFScan", MSG_STOP_FSCAN},
	{"ClearFScan", MSG_CLEAR_FSCAN},
	{"SetFScanParam", MSG_SET_FSCAN_PARAM},
	{"GetFScanParam", MSG_GET_FSCAN_PARAM},
	{"GetFScanNameList", MSG_GET_FSCAN_NAME_LIST},

	{"SetCruise", MSG_SET_CRUISE},
	{"GetCruise", MSG_GET_CRUISE},
	{"CallCruise", MSG_CALL_CRUISE},
	{"StopCruise", MSG_STOP_CRUISE},
	{"ClearCruise", MSG_CLEAR_CRUISE},

	{"SetWatchKeeping", MSG_SET_WATCHKEEPING},
	{"GetWatchKeeping", MSG_GET_WATCHKEEPING},
	{"ClearWatchKeeping", MSG_CLEAR_WATCHKEEPING},
	{"StartWatchKeeping", MSG_START_WATCHKEEPING},
	{"StopWatchKeeping", MSG_STOP_WATCHKEEPING},

	{"SetMask", MSG_SET_MASK},
	{"GetMask", MSG_GET_MASK},
	{"ClearMask", MSG_CLEAR_MASK},
	{"ShowMask", MSG_SHOW_MASK},
	{"HideMask", MSG_HIDE_MASK},

	{"SetPictureFreeze", MSG_SET_FREEZEFLAG},
	{"GetPictureFreeze", MSG_GET_FREEZEFLAG},

	{"SetTimer", MSG_SET_TIMER},
	{"GetTimer", MSG_GET_TIMER},
	{"ClearTimer", MSG_CLEAR_TIMER},
	{"SetTimerFlag", MSG_SET_TIMER_FLAG},
	{"GetTimerFlag", MSG_GET_TIMER_FLAG},

	{"SetSleepFlag", MSG_SET_SLEEP_FLAG},
	{"GetSleepFlag", MSG_GET_SLEEP_FLAG},
	{"SetIdleTimeToSleep", MSG_SET_IDLE_TIME_TO_SLEEP},
	{"GetIdleTimeToSleep", MSG_GET_IDLE_TIME_TO_SLEEP},
	{"StartSleepCountDown", MSG_START_SLEEP_COUNTDOWN},

	{"SetAEMode", MSG_SET_AE_MODE},
	{"GetAEMode", MSG_GET_AE_MODE},
	
	{"SetAWBMode", MSG_SET_AWB_MODE},
	{"GetAWBMode", MSG_GET_AWB_MODE},

	{"SetLEDPower", MSG_SET_LED_POWER},
	{"GetLEDPower", MSG_GET_LED_POWER},

	{"SetLEDMode", MSG_SET_LED_MODE},
	{"GetLEDMode", MSG_GET_LED_MODE},

	{"TriggerFocusOnce", MSG_DO_ONE_FOCUS},
	{"MovePointToCenter", MSG_MOVE_POINT_TO_CENTER},
	{"MoveBlockToCenter", MSG_MOVE_BLOCK_TO_CENTER},

	{"GetCurrentTask", MSG_GET_CUR_TASK},

	{"SetDefog", MSG_SET_DEFOG},
};




CSWNetCommandProcess::CSWNetCommandProcess()
	:m_fInited(FALSE)
	,CSWMessage(MSG_NETCOMMAND_START, MSG_NETCOMMAND_END)
	,m_dwGETIP_COMMAND(0x00000101)
	,m_dwSETIP_COMMAND(0x00000102)
	,m_dwState(STATE_READY)
	,m_wProbePort(0)
	,m_wCMDProcessPort(0)
	,m_pProbeThread(NULL)
	,m_pCMDServerThread(NULL)
{
	Initialize(6666, 9910);
}


CSWNetCommandProcess::~CSWNetCommandProcess()
{
	if (!m_fInited)
	{
		return ;
	}

	m_dwState = STATE_STOPPED;
	
	PRINT("Info: deleting CSWNetCommandProcess......\n");

	SAFE_RELEASE(m_pProbeThread);

	SAFE_RELEASE(m_pCMDServerThread);
	
	while (!m_lstCMDProcessThread.IsEmpty())
	{
		CSWThread* pThread = m_lstCMDProcessThread.RemoveHead();
		if (NULL != pThread)
		{		
			SAFE_RELEASE(pThread);
		}
	}

	m_dwState = STATE_STOPPED;
	m_fInited = FALSE;
}



HRESULT CSWNetCommandProcess::Initialize(const WORD wProbePort, const WORD wCMDProcessPort)
{
	if (m_fInited)
	{
	    return S_OK;
	}

	m_wProbePort = wProbePort;	
	m_wCMDProcessPort = wCMDProcessPort;
	
	m_pProbeThread = new CSWThread();
	if (NULL == m_pProbeThread)
	{
		PRINT("Err: no memory for m_pProbeThread\n");
		return E_OUTOFMEMORY;
	}

	m_pCMDServerThread = new CSWThread();
	if (NULL == m_pCMDServerThread)
	{	
		PRINT("Err: no memory for m_pCMDServerThread\n");
		return E_OUTOFMEMORY;
	}
	
	m_lstCMDProcessThread.RemoveAll();

	m_fInited = TRUE;

	return S_OK;
}



HRESULT CSWNetCommandProcess::Run()
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
	    return E_NOTIMPL;
	}

	if (STATE_RUNNING == m_dwState)
	{
		Stop(); // or return fail!
	}

	HRESULT hr = S_OK;
	
	m_dwState = STATE_RUNNING;

	//开启设备侦测监听线程
	hr = m_pProbeThread->Start(OnProbeProxy, (VOID*)this);
	if (FAILED(hr))
	{
		PRINT("Err: failed to start probe thread\n");
		return E_FAIL;
	}

	//开启网络命令监听线程
	hr = m_pCMDServerThread->Start(OnNetCMDConnectProxy, (VOID*)this);
	if (FAILED(hr))
	{
		PRINT("Err: failed to start cmd connect thread\n");
		return E_FAIL;
	}

	return S_OK;	
}




HRESULT CSWNetCommandProcess::Stop()
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
	    return E_NOTIMPL;
	}

	
	if (STATE_RUNNING != m_dwState)
	{
		return S_OK;
	}


	//HRESULT hr = S_OK;
	m_dwState = STATE_STOPPED;

	//停止设备侦测监听线程
	if (FAILED(m_pProbeThread->Stop()))
	{
		PRINT("Err: failed to stop probe thread\n");
		return E_FAIL;
	}

	//停止网络命令监听线程
	if (FAILED(m_pCMDServerThread->Stop()))
	{
		PRINT("Err: failed to stop cmd process thread\n");
		return E_FAIL;
	}

	while (!m_lstCMDProcessThread.IsEmpty())
	{
		CSWThread* pThread = m_lstCMDProcessThread.RemoveHead();
		if (NULL != pThread)
		{
			SAFE_RELEASE(pThread);
		}
	}

	return S_OK;	
}





const CHAR* CSWNetCommandProcess::SearchCmdNameByID(const DWORD dwCMDID)
{
	for (DWORD i=0; i<sizeof(g_XmlCmdMappingTable)/sizeof((g_XmlCmdMappingTable)[0]); i++)
	{
		if (dwCMDID == g_XmlCmdMappingTable[i].dwCmdID)
		{
			return g_XmlCmdMappingTable[i].szCmdName;
		}
	}

	return NULL;
}


HRESULT CSWNetCommandProcess::SendDomeCameraCustomCMD(const DWORD dwCMDID, const CHAR * szValue, PVOID* ppvRetBuf)
{
	DWORD dwID = 0;
	CHAR szName[32] = {0};
	INT iLingerTime = 0;
	

#define GET_NAME(NAME)\
{\
	if (NULL == swpa_strstr(szValue, "Name:")) return E_INVALIDARG;\
	if (NULL == swpa_strstr(swpa_strstr(szValue, "Name:"), "|||")) return E_INVALIDARG;\
	INT iLen = (INT)swpa_strstr(swpa_strstr(szValue, "Name:"), "|||") - (INT)swpa_strstr(szValue, "Name:") - 5/*=strlen("Name:")*/;\
	swpa_strncpy((NAME), swpa_strstr(szValue, "Name:") + swpa_strlen("Name:"), iLen > sizeof((NAME)) - 1 ? sizeof((NAME)) - 1 : iLen);\
}


#define GET_MOVETYPE(INDEX, TYPE)\
{\
	CHAR szMove[32] = "Move:";	\
	CHAR szType[32] = {0};\
	if (INDEX >= 0) swpa_snprintf(szMove, 31, "Move%d:", INDEX);	\
	INT iLen = (INT)swpa_strstr(swpa_strstr(szValue, szMove), "|||") - (INT)swpa_strstr(szValue, szMove) - swpa_strlen(szMove);\
	swpa_strncpy(szType, swpa_strstr(szValue, szMove) + swpa_strlen(szMove), iLen > sizeof(szType) - 1 ? sizeof(szType) - 1 : iLen);\
	if (0 == swpa_strcmp(szType, "Preset"))	TYPE = MOVETYPE_PRESET;\
	else if (0 == swpa_strcmp(szType, "HScan"))	TYPE = MOVETYPE_HSCAN;\
	else if (0 == swpa_strcmp(szType, "FScan"))	TYPE = MOVETYPE_FSCAN;\
	else if (0 == swpa_strcmp(szType, "Cruise")) TYPE = MOVETYPE_CRUISE; \
	else { SW_TRACE_DEBUG("Err: unsupported motion type :%s\n", szType); return E_INVALIDARG;}\
}

#define GET_MOVEID(INDEX, ID)\
{\
	CHAR szID[32] = "MoveID:";	\
	if (INDEX >= 0) swpa_snprintf(szID,  31, "MoveID%d:", INDEX);	\
	swpa_sscanf(swpa_strstr(szValue, szID) + swpa_strlen(szID), "%d", &(ID));\
}

#define GET_TIME(INDEX, TIME)\
{\
	CHAR szTime[32] = "Time:";	\
	if (INDEX >= 0) swpa_snprintf(szTime,  31, "Time%d:", INDEX);	\
	swpa_sscanf(swpa_strstr(szValue, szTime) + swpa_strlen(szTime), "%d", &(TIME));\
}

#define GET_INT(STR, INT)\
{\
	swpa_sscanf(swpa_strstr(szValue, STR) + swpa_strlen(STR), "%d", &(INT));\
}


	switch (dwCMDID)
	{
        case MSG_SET_PRESET_POS:
		{
			PRESETPOS_PARAM_STRUCT sParam;
			swpa_memset(&sParam, 0, sizeof(sParam));
			
			GET_INT("ID:", dwID);
			GET_NAME(sParam.szName);
			GET_INT("CheckPT:", sParam.fCheckPT);

			//这里都用MSG_SET_PRESET_POS其实有点诡异
			if (FAILED(CSWMessage::SendMessage(MSG_SET_PRESET_POS, WPARAM(dwID),LPARAM(&sParam))))
			{
				SW_TRACE_DEBUG("Err: failed to set preset #%d\n", dwID);
				return E_FAIL;
			}
		}
		break;

		case MSG_GET_PRESET_POS:
		{
			GET_INT("ID:", dwID);
			PRESETPOS_PARAM_STRUCT sParam;
			swpa_memset(&sParam, 0, sizeof(sParam));
			
			*ppvRetBuf = swpa_mem_alloc(128 * sizeof(CHAR));
			if (NULL == *ppvRetBuf)
			{
				PRINT("Err: no memory for *ppvRetBuf\n");
				return E_FAIL;
			}
			swpa_memset(*ppvRetBuf, 0, 128 * sizeof(CHAR));
			
			if (FAILED(CSWMessage::SendMessage(MSG_GET_PRESET_POS, WPARAM(dwID),LPARAM(&sParam))))
			{
				SW_TRACE_DEBUG("Err: failed to get preset #%d\n", dwID);
				return E_FAIL;
			}
			swpa_snprintf((CHAR*)*ppvRetBuf, 128, "[Name:%s|||CheckPT:%d|||]", sParam.szName, sParam.fCheckPT);
		}
		break;

		case MSG_SET_HSCAN_PARAM:
		{
			HSCAN_PARAM_STRUCT sParam;
			swpa_memset(&sParam, 0 ,sizeof(sParam));
			
			GET_INT("ID:", dwID);
			GET_NAME(sParam.szName);
			
			sParam.fValid = TRUE; //default
			GET_INT("Speed:", sParam.iSpeed);
			//GET_INT("Direction:", sParam.iDirection);
			//swpa_strncpy(sParam.szName, szName, sizeof(sParam.szName) - 1);
			
			//这里都用MSG_SET_HSCAN_PARAM其实有点诡异
			if (FAILED(CSWMessage::SendMessage(MSG_SET_HSCAN_PARAM, WPARAM(dwID),LPARAM(&sParam))))
			{
				SW_TRACE_DEBUG("Err: failed to set HScan #%d\n", dwID);
				return E_FAIL;
			}
		}
		break;

		case MSG_GET_HSCAN_PARAM:
		{
			GET_INT("ID:", dwID);

			HSCAN_PARAM_STRUCT sParam;
			swpa_memset(&sParam, 0 ,sizeof(sParam));
			
			if (FAILED(CSWMessage::SendMessage(MSG_GET_HSCAN_PARAM, WPARAM(dwID),LPARAM(&sParam))))
			{
				SW_TRACE_DEBUG("Err: failed to get HScan #%d\n", dwID);
				return E_FAIL;
			}

			CHAR szBase[128] = {0};
			swpa_sprintf(szBase, "[Name:%s|||Speed:%d|||Direction:%d|||]",
				sParam.szName, sParam.iSpeed, sParam.iDirection);
			
			*ppvRetBuf = swpa_mem_alloc(128 * sizeof(CHAR));
			if (NULL == *ppvRetBuf)
			{
				PRINT("Err: no memory for *ppvRetBuf\n");
				return E_FAIL;
			}

			swpa_memcpy(*ppvRetBuf, szBase, 128);
		}
		break;

		case MSG_SET_FSCAN_PARAM:
		{
			GET_INT("ID:", dwID);
			GET_NAME(szName);

			//这里都用MSG_SET_FSCAN_PARAM其实有点诡异
			if (FAILED(CSWMessage::SendMessage(MSG_SET_FSCAN_PARAM, WPARAM(dwID),LPARAM(szName))))
			{
				SW_TRACE_DEBUG("Err: failed to set FScan #%d\n", dwID);
				return E_FAIL;
			}
		}
		break;

		case MSG_GET_FSCAN_PARAM:
		{
			GET_INT("ID:", dwID);
			
			*ppvRetBuf = swpa_mem_alloc(32 * sizeof(CHAR));
			if (NULL == *ppvRetBuf)
			{
				PRINT("Err: no memory for *ppvRetBuf\n");
				return E_FAIL;
			}
			if (FAILED(CSWMessage::SendMessage(MSG_GET_FSCAN_PARAM, WPARAM(dwID),LPARAM(*ppvRetBuf))))
			{
				SW_TRACE_DEBUG("Err: failed to get FScan #%d\n", dwID);
				return E_FAIL;
			}
		}
		break;

		case MSG_SET_CRUISE:
		{
			GET_INT("CruiseID:", dwID);
			GET_NAME(szName);			

			CRUISE_PARAM_STRUCT sCruiseParam;
			swpa_memset(&sCruiseParam, 0 ,sizeof(sCruiseParam));
			
			swpa_strncpy(sCruiseParam.szName, szName, sizeof(sCruiseParam.szName)-1);
			GET_INT("Count:", sCruiseParam.iMotionCount);
			GET_INT("CheckPT:", sCruiseParam.fCheckPT);

			if (sCruiseParam.iMotionCount > MAX_MOTION_IN_CRUISE)
			{
				SW_TRACE_DEBUG("Err: Count = %d, out of range (Max is %d).\n", sCruiseParam.iMotionCount,
					MAX_MOTION_IN_CRUISE);
				return E_INVALIDARG;
			}
			
			for (INT i=0; i<sCruiseParam.iMotionCount; i++)
			{
				swpa_memset(szName, 0, sizeof(szName));
				GET_MOVETYPE(i, sCruiseParam.sMotion[i].iMoveType);
				GET_MOVEID(i, sCruiseParam.sMotion[i].iMoveID);
				GET_TIME(i, sCruiseParam.sMotion[i].iLingerTime);
			}

			sCruiseParam.fValid = TRUE;			

			if (FAILED(CSWMessage::SendMessage(MSG_SET_CRUISE, WPARAM(dwID),LPARAM(&sCruiseParam))))
			{
				SW_TRACE_DEBUG("Err: failed to set Cruise #%d\n", dwID);
				return E_FAIL;
			}
		}
		break;

		case MSG_GET_CRUISE:
		{
			GET_INT("CruiseID:", dwID);

			CRUISE_PARAM_STRUCT sCruiseParam;
			swpa_memset(&sCruiseParam, 0 ,sizeof(sCruiseParam));
			
			if (FAILED(CSWMessage::SendMessage(MSG_GET_CRUISE, WPARAM(dwID),LPARAM(&sCruiseParam))))
			{
				SW_TRACE_DEBUG("Err: failed to get cruise #%d\n", dwID);
				return E_FAIL;
			}

			*ppvRetBuf = swpa_mem_alloc(2048 * sizeof(CHAR));
			if (NULL == *ppvRetBuf)
			{
				PRINT("Err: no memory for *ppvRetBuf\n");
				return E_FAIL;
			}

			CHAR szBase[2048] = {0};
			swpa_sprintf(szBase, "[Name:%s|||CheckPT:%d|||Count:%d|||]",
				sCruiseParam.szName, sCruiseParam.fCheckPT, sCruiseParam.iMotionCount);

			
			for (INT i=0; i<sCruiseParam.iMotionCount; i++)
			{
				CHAR szMotion[1024] = {0};
				CHAR* szMotionType = NULL;
				switch (sCruiseParam.sMotion[i].iMoveType)
				{
					case MOVETYPE_PRESET: szMotionType = "Preset"; break;
					case MOVETYPE_HSCAN: szMotionType = "HScan"; break;
					case MOVETYPE_FSCAN: szMotionType = "FScan"; break;
					default:
						SW_TRACE_DEBUG("Err: invalid move type %d!!\n", sCruiseParam.sMotion[i].iMoveType);
						szMotionType = "Unsupported";
					break;
						
				}

				swpa_sprintf(szMotion, "Move%d:%s|||MoveID%d:%d|||Time%d:%d|||",
							i, szMotionType, i, sCruiseParam.sMotion[i].iMoveID, i, sCruiseParam.sMotion[i].iLingerTime);

				swpa_strcat(szBase, szMotion);
			}

			swpa_memcpy(*ppvRetBuf, szBase, 2048);
		}
		break;

		case MSG_SET_WATCHKEEPING:
		{
			WATCHKEEPING_PARAM_STRUCT sParam;
			swpa_memset(&sParam, 0 ,sizeof(sParam));

			GET_MOVETYPE(-1, sParam.iMotionType);
			GET_MOVEID(-1, sParam.iMotionID);
			GET_TIME(-1, sParam.iWatchTime);
			
			GET_INT("Enable:", sParam.fEnable);

			if (FAILED(CSWMessage::SendMessage(MSG_SET_WATCHKEEPING, WPARAM(&sParam),LPARAM(0))))
			{
				SW_TRACE_DEBUG("Err: failed to set preset #%d\n", dwID);
				return E_FAIL;
			}
		}
		break;

		case MSG_GET_WATCHKEEPING:
		{
			*ppvRetBuf = swpa_mem_alloc(128 * sizeof(CHAR));
			if (NULL == *ppvRetBuf)
			{
				PRINT("Err: no memory for *ppvRetBuf\n");
				return E_FAIL;
			}

			WATCHKEEPING_PARAM_STRUCT sParam;
			swpa_memset(&sParam, 0 ,sizeof(sParam));
			
			if (FAILED(CSWMessage::SendMessage(MSG_GET_WATCHKEEPING, WPARAM(0),LPARAM(&sParam))))
			{
				SW_TRACE_DEBUG("Err: failed to get preset #%d\n", dwID);
				return E_FAIL;
			}

			CHAR szBase[128] = {0};
			CHAR* szMotionType = NULL;
			switch (sParam.iMotionType)
			{
				case MOVETYPE_PRESET: szMotionType = "Preset"; break;
				case MOVETYPE_HSCAN: szMotionType = "HScan"; break;
				case MOVETYPE_FSCAN: szMotionType = "FScan"; break;
				case MOVETYPE_CRUISE: szMotionType = "Cruise"; break;
				default:
					SW_TRACE_DEBUG("Err: invalid move type %d!!\n", sParam.iMotionType);
					szMotionType = "Unsupported";
				break;
					
			}
			
			swpa_sprintf(szBase, "[Enable:%d|||Time:%d|||Move:%s|||MoveID:%d|||]",
				sParam.fEnable, sParam.iWatchTime, szMotionType, sParam.iMotionID);

			swpa_memcpy(*ppvRetBuf, szBase, 128);
		}
		break;

		case MSG_SET_TIMER:
		{
			
			TIMER_PARAM_STRUCT sParam;
			swpa_memset(&sParam, 0, sizeof(sParam));

			GET_NAME(sParam.szName);
			GET_INT("TimerID:", dwID);
			GET_MOVETYPE(-1, sParam.iMotionType);
			GET_MOVEID(-1, sParam.iMotionID);

			GET_INT("BeginTime:", sParam.iBeginTime);
			GET_INT("EndTime:", sParam.iEndTime);
			GET_INT("Weekday:", sParam.iWeekday);
			GET_INT("Enable:", sParam.fEnable);
			sParam.fValid = TRUE;


			//这里都用MSG_SET_TIMER其实有点诡异
			if (FAILED(CSWMessage::SendMessage(MSG_SET_TIMER, WPARAM(dwID),LPARAM(&sParam))))
			{
				SW_TRACE_DEBUG("Err: failed to set Timer #%d\n", dwID);
				return E_FAIL;
			}
		}
		break;

		case MSG_GET_TIMER:
		{
			GET_INT("TimerID:", dwID);
			TIMER_PARAM_STRUCT sParam;
			swpa_memset(&sParam, 0, sizeof(sParam));
			
			*ppvRetBuf = swpa_mem_alloc(256 * sizeof(CHAR));
			if (NULL == *ppvRetBuf)
			{
				PRINT("Err: no memory for *ppvRetBuf\n");
				return E_FAIL;
			}
			if (FAILED(CSWMessage::SendMessage(MSG_GET_TIMER, WPARAM(dwID),LPARAM(&sParam))))
			{
				SW_TRACE_DEBUG("Err: failed to get Timer #%d\n", dwID);
				return E_FAIL;
			}

			CHAR szBase[256] = {0};
			CHAR* szMotionType = NULL;
			switch (sParam.iMotionType)
			{
				case MOVETYPE_PRESET: szMotionType = "Preset"; break;
				case MOVETYPE_HSCAN: szMotionType = "HScan"; break;
				case MOVETYPE_FSCAN: szMotionType = "FScan"; break;
				case MOVETYPE_CRUISE: szMotionType = "Cruise"; break;
				default:
					SW_TRACE_DEBUG("Err: invalid move type %d!!\n", sParam.iMotionType);
					szMotionType = "Unsupported";
				break;
					
			}

			swpa_sprintf(szBase, 
				"[Name:%s|||Enable:%d|||Weekday:%d|||BeginTime:%d|||EndTime:%d|||Move:%s|||MoveID:%d|||]",
				sParam.szName, sParam.fEnable, sParam.iWeekday, sParam.iBeginTime, sParam.iEndTime, szMotionType, sParam.iMotionID);

			swpa_memcpy(*ppvRetBuf, szBase, 256);
		}
		break;

		case MSG_SET_MASK:
		{
			MASK_PARAM_STRUCT sParam;
			swpa_memset(&sParam, 0, sizeof(sParam));

			GET_NAME(sParam.szName);
			GET_INT("MaskID:", dwID);

			GET_INT("Left:", sParam.iLeft);
			GET_INT("Top:", sParam.iTop);
			GET_INT("Width:", sParam.iWidth);
			GET_INT("Height:", sParam.iHeight);
			GET_INT("Color:", sParam.iColor);
			GET_INT("PresetID:", sParam.iPresetID);
			GET_INT("Enable:", sParam.fEnable);
			sParam.fValid = TRUE;


			//这里都用MSG_SET_MASK其实有点诡异
			if (FAILED(CSWMessage::SendMessage(MSG_SET_MASK, WPARAM(dwID),LPARAM(&sParam))))
			{
				SW_TRACE_DEBUG("Err: failed to set Mask #%d\n", dwID);
				return E_FAIL;
			}
		}
		break;

		case MSG_GET_MASK:
		{
			GET_INT("MaskID:", dwID);
			MASK_PARAM_STRUCT sParam;
			swpa_memset(&sParam, 0, sizeof(sParam));
			
			*ppvRetBuf = swpa_mem_alloc(256 * sizeof(CHAR));
			if (NULL == *ppvRetBuf)
			{
				PRINT("Err: no memory for *ppvRetBuf\n");
				return E_FAIL;
			}
			swpa_memset(*ppvRetBuf, 0, 256 * sizeof(CHAR));
			
			if (FAILED(CSWMessage::SendMessage(MSG_GET_MASK, WPARAM(dwID),LPARAM(&sParam))))
			{
				SW_TRACE_DEBUG("Err: failed to get Mask #%d\n", dwID);
				return E_FAIL;
			}

			swpa_sprintf((CHAR*)*ppvRetBuf, 
				"[Name:%s|||Enable:%d|||Left:%d|||Top:%d|||Width:%d|||Height:%d|||Color:%d|||PresetID:%d|||]",
				sParam.szName, sParam.fEnable, sParam.iLeft, sParam.iTop, 
				sParam.iWidth, sParam.iHeight, sParam.iColor, sParam.iPresetID);

		}
		break;

		case MSG_GET_CUR_TASK:
		{
			*ppvRetBuf = swpa_mem_alloc(128 * sizeof(CHAR));
			if (NULL == *ppvRetBuf)
			{
				PRINT("Err: no memory for *ppvRetBuf\n");
				return E_FAIL;
			}

			swpa_memset(*ppvRetBuf, 0, 128 * sizeof(CHAR));

			DWORD dwTaskInfo = 0;
			if (FAILED(CSWMessage::SendMessage(MSG_GET_CUR_TASK, WPARAM(0),LPARAM(&dwTaskInfo))))
			{
				SW_TRACE_DEBUG("Err: failed to get current task\n");
				return E_FAIL;
			}

			CHAR szBase[128] = {0};
			CHAR* szMotionType = NULL;
			switch (dwTaskInfo>>16)
			{
				case MOVETYPE_PRESET: szMotionType = "Preset"; break;
				case MOVETYPE_HSCAN: szMotionType = "HScan"; break;
				case MOVETYPE_FSCAN: szMotionType = "FScan"; break;
				case MOVETYPE_CRUISE: szMotionType = "Cruise"; break;
				case MOVETYPE_INFINITY_PAN: szMotionType = "InfinityPan"; break;
				case MOVETYPE_STANDBY: szMotionType = "Standby"; break;
				case MOVETYPE_RESET: szMotionType = "Reset"; break;
				case MOVETYPE_FSCANRECORD: szMotionType = "FScanRecord"; break;
				default:
					SW_TRACE_DEBUG("Err: invalid move type %d!!\n", dwTaskInfo>>16);
					szMotionType = "Idle";
				break;
					
			}
			
			swpa_sprintf(szBase, "[Task:%s|||TaskID:%d|||]",
				szMotionType, dwTaskInfo & 0xFFFF);

			swpa_memcpy(*ppvRetBuf, szBase, 128);
		}
		break;

		default:

		break;
	}


	return S_OK;
}



HRESULT CSWNetCommandProcess::SendCustomCMD(const DWORD dwCMDID, const CHAR * szValue, PVOID* ppvRetBuf)
{
	HRESULT hr = S_OK;
		
	switch (dwCMDID)
	{
        case MSG_GET_CAM_FPGA_REG:
        {
            INT Value[2];
            swpa_memset(Value,0,sizeof(Value));
            Value[0] = swpa_atoi(szValue);
            hr = CSWMessage::SendMessage(dwCMDID,
                (WPARAM)NULL,
                (LPARAM)&Value);
            if(hr == S_OK)
            {
                (*ppvRetBuf) = swpa_mem_alloc(sizeof(INT));
                swpa_memcpy((*ppvRetBuf),&Value[1],sizeof(INT));
            }
        }
        break;
		case MSG_APP_GETTIME:
		{
			SWPA_DATETIME_TM sRealTime;
			swpa_memset(&sRealTime, 0, sizeof(sRealTime));
			hr = CSWMessage::SendMessage(dwCMDID,
				(WPARAM)NULL,
				(LPARAM)&sRealTime);
			
			if (SUCCEEDED(hr))
			{
				CHAR szStr[256] = {0};
				swpa_snprintf(szStr, sizeof(szStr)-1, "Date[%4d.%02d.%02d],Time[%02d:%02d:%02d %03d]", 
					sRealTime.year, sRealTime.month, sRealTime.day, sRealTime.hour, sRealTime.min, sRealTime.sec, sRealTime.msec);
				*ppvRetBuf = swpa_mem_alloc(swpa_strlen(szStr)+1);
				if (NULL == *ppvRetBuf)
				{
					PRINT("Err: no memory for *ppvRetBuf\n");
					hr = E_OUTOFMEMORY;
					break;
				}
				swpa_memset(*ppvRetBuf, 0, swpa_strlen(szStr)+1);
				swpa_strncpy((CHAR*)*ppvRetBuf, szStr, swpa_strlen(szStr));
			}				
		}
		break;
		
		case MSG_APP_SETTIME:
		{
			SWPA_DATETIME_TM sRealTime;
			swpa_memset(&sRealTime, 0, sizeof(sRealTime));

			SW_TRACE_DEBUG("<settime>%s.\n", szValue);
            INT iY, iMon, iD, iH, iM, iS, iMS, iTZ=8;//水星iTZ=8 金星=0
            #ifdef DM8127
                iTZ=0;
            #endif
			swpa_sscanf(szValue, "Date[%4d.%2d.%2d],Time[%2d:%2d:%2d %03d]",// UTC%d", 
					&iY, &iMon, &iD, &iH, &iM, &iS, &iMS);//, &iTZ);
			sRealTime.year = (short)iY;
			sRealTime.month = (short)iMon;
			sRealTime.day = (short)iD;
			sRealTime.hour = (short)iH;
			sRealTime.min = (short)iM;
			sRealTime.sec = (short)iS;
			sRealTime.msec = (short)iMS;
            SW_TRACE_DEBUG("<settime>%d-%d-%d %d:%d:%d %d.\n",iY, iMon, iD, iH, iM, iS, iMS);
			if (iY < 1970)
			{
				SW_TRACE_DEBUG("<settime> cannot set Year to %d.\n",iY);
				hr = E_INVALIDARG;
			}
			else
			{
				/*if (-12 > iTZ || +12 < iTZ)
				{
					PRINT("Err: invalid TimeZone value %d\n");
					hr = E_INVALIDARG;
					break;
				}
				else
				{
					SWPA_TIME sTime;
					
					swpa_datetime_tm2time(sRealTime, &sTime);
					sTime.sec -= 3600*iTZ;
					swpa_datetime_time2tm(sTime, &sRealTime);
				}*/

				SWPA_TIME sTime, sCurTime;
				CSWDateTime cCurTime;

				cCurTime.GetTime(&sCurTime);
					
				swpa_datetime_tm2time(sRealTime, &sTime);

				if (sCurTime.sec <= sTime.sec + 3
					&& sCurTime.sec >= sTime.sec - 3)
				{
					SW_TRACE_DEBUG("Info: no need to sync time.\n");
					hr = S_OK;
				}
				else
				{
					hr = CSWMessage::SendMessage(dwCMDID,
						(WPARAM)&sRealTime,
						(LPARAM)NULL);		
				}	
			}
		}
		break;

		case MSG_APP_GETIP:
		{
            hr = E_NOTIMPL;
            CHAR* pszNetInfo[4];
            pszNetInfo[0] = (CHAR*)swpa_mem_alloc(256);
            pszNetInfo[1] = (CHAR*)swpa_mem_alloc(256);
            pszNetInfo[2] = (CHAR*)swpa_mem_alloc(256);
            pszNetInfo[3] = (CHAR*)swpa_mem_alloc(256);
			hr = CSWMessage::SendMessage(dwCMDID,
				(WPARAM)NULL,
                (LPARAM)pszNetInfo);
            PRINT("%s %d   %s  %s %s %s \n",__FUNCTION__,__LINE__,pszNetInfo[0],pszNetInfo[1],pszNetInfo[2],pszNetInfo[3]);


			if (SUCCEEDED(hr))
			{

                CHAR *szStr = (CHAR *)swpa_mem_alloc(sizeof(CHAR) *256);
                swpa_snprintf(szStr,255, "IP[%s],Mask[%s],Gateway[%s],Mac[%s]",
                    pszNetInfo[0], pszNetInfo[1], pszNetInfo[2], pszNetInfo[3]);
                *ppvRetBuf = szStr;
                PRINT("%s %d %s \n",__FUNCTION__,__LINE__,szStr);
			}
            swpa_mem_free(pszNetInfo[0]);
            swpa_mem_free(pszNetInfo[1]);
            swpa_mem_free(pszNetInfo[2]);
            swpa_mem_free(pszNetInfo[3]);

            //hr = E_NOTIMPL;
		}
		break;

		case MSG_APP_SETIP:
        {//IP[172.18.10.188],Mask[255.255.0.0],Gateway[172.18.10.1],Mac[]
            //CHAR aszNetInfo[4][256] = {{0}};
			CHAR* pszNetInfo[4];
			pszNetInfo[0] = (CHAR*)swpa_mem_alloc(64);
			pszNetInfo[1] = (CHAR*)swpa_mem_alloc(64);
			pszNetInfo[2] = (CHAR*)swpa_mem_alloc(64);
			pszNetInfo[3] = (CHAR*)swpa_mem_alloc(64);
            PRINT("%s %d  \n%s\n",__FUNCTION__,__LINE__,szValue);
			CSWString strTemp = szValue;
			CSWString strIp;
			int iIndex = strTemp.Find("IP");
			int iEnd = 0;
			if( iIndex >= 0 )
			{
				iIndex += 3;
				iEnd = strTemp.Find("]", iIndex);
				strIp = strTemp.Substr(iIndex, iEnd - iIndex);
			}
			CSWString strMask;
			iIndex = strTemp.Find("Mask");
			if( iIndex >= 0 )
			{
				iIndex += 5;
				iEnd = strTemp.Find("]", iIndex);
				strMask = strTemp.Substr(iIndex, iEnd - iIndex);
			}
			CSWString strGateway;
			iIndex = strTemp.Find("Gateway");
			if( iIndex >= 0 )
			{
				iIndex += 8;
				iEnd = strTemp.Find("]", iIndex);
				strGateway = strTemp.Substr(iIndex, iEnd - iIndex);
			}
			CSWString strMac;
			iIndex = strTemp.Find("Mac");
			if( iIndex >= 0 )
			{
				iIndex += 4;
				iEnd = strTemp.Find("]", iIndex);
				strMac = strTemp.Substr(iIndex, iEnd - iIndex);
			}
			swpa_strcpy(pszNetInfo[0], (const CHAR*)strIp);
			swpa_strcpy(pszNetInfo[1], (const CHAR*)strMask);
			swpa_strcpy(pszNetInfo[2], (const CHAR*)strGateway);
			swpa_strcpy(pszNetInfo[3], (const CHAR*)strMac);
			SW_TRACE_DEBUG("<Netcmd>setip:%s|%s|%s|%s.value:%s", (const CHAR*)strIp, (const CHAR*)strMask, (const CHAR*)strGateway, (const CHAR*)strMac, szValue);
			if( strIp.IsEmpty() || strMask.IsEmpty())
			{
				hr = E_INVALIDARG;
			}
			else
			{
				hr = CSWMessage::SendMessage(dwCMDID,
					(WPARAM)pszNetInfo,
					(LPARAM)NULL);	
			}
			swpa_mem_free(pszNetInfo[0]);
			swpa_mem_free(pszNetInfo[1]);
			swpa_mem_free(pszNetInfo[2]);
			swpa_mem_free(pszNetInfo[3]);
		}
		break;

		case MSG_APP_GETVERSION:
		{
			SW_TRACE_DEBUG("<netcmd>MSG_APP_GETVERSION.\n");
			// todo
			(*ppvRetBuf) = swpa_mem_alloc(256);
			if( *ppvRetBuf != NULL )
			{
				swpa_strcpy((CHAR*)(*ppvRetBuf), "SoftVersion=[1.0],ModelVersion=[1.0]");
			}
			hr = S_OK;
		}
		break;

		case MSG_APP_GETDEVTYPE:
		{
			SW_TRACE_DEBUG("<netcmd>MSG_APP_GETDEVTYPE.\n");
			// todo
			(*ppvRetBuf) = swpa_mem_alloc(256);
			if( *ppvRetBuf != NULL )
			{
				CHAR szVersion[255];
                CHAR szDevType[255];
				CSWMessage::SendMessage(MSG_APP_GETVERSION, 0, (LPARAM)szVersion);
                CSWMessage::SendMessage(MSG_APP_GETDEVTYPE, 0, (LPARAM)szDevType);
                swpa_sprintf((CHAR*)(*ppvRetBuf), "DevType=[%s],BuildNo=[%s]",szDevType,szVersion);
			}
			hr = S_OK;
		}
		break;
        case MSG_SET_SYNC_POWER:
        {
            CSWString strTemp = szValue;
            CSWString strDelay;
            CSWString strMode;
            INT iMode;
            INT iDelay;
            int iIndex = strTemp.Find("Mode");
            int iEnd = 0;
            if( iIndex >= 0 )
            {
                iIndex += 6;
                iEnd = strTemp.Find("]", iIndex);
                strMode = strTemp.Substr(iIndex, iEnd - iIndex);
                iMode = swpa_atoi(strMode);
            }
            iIndex = strTemp.Find("DelayMS");
            iEnd = 0;
            if( iIndex >= 0 )
            {
                iIndex += 9;
                iEnd = strTemp.Find("]", iIndex);
                strDelay = strTemp.Substr(iIndex, iEnd - iIndex);
                iDelay = swpa_atoi(strDelay);
            }
            SW_TRACE_DEBUG("<netcmd>MSG_SET_AC_SYNC_ENABLE Mode=%d Delay=%d.\n",iMode,iDelay);
            if( strDelay.IsEmpty() || strMode.IsEmpty())
            {
                hr = E_INVALIDARG;
            }
            else
            {
                hr = CSWMessage::SendMessage(MSG_SET_AC_SYNC_ENABLE,(WPARAM)iMode, (LPARAM)NULL);
                if(hr == S_OK)
                    hr = CSWMessage::SendMessage(MSG_SET_AC_SYNC_DELAY,(WPARAM)iDelay, (LPARAM)NULL);

            }
        }
        break;
        case MSG_GET_SYNC_POWER:
        {
            SW_TRACE_DEBUG("<netcmd>MSG_GET_SYNC_POWER.\n");
            (*ppvRetBuf) = swpa_mem_alloc(256);
            if( *ppvRetBuf != NULL )
            {
                INT iMode;
                INT iDelay;
                CSWMessage::SendMessage(MSG_GET_AC_SYNC_ENABLE,(WPARAM)NULL, (LPARAM)&iMode);
                CSWMessage::SendMessage(MSG_GET_AC_SYNC_DELAY,(WPARAM)NULL, (LPARAM)&iDelay);
                swpa_sprintf((CHAR*)(*ppvRetBuf), "Mode=[%d],Delay=[%d]", iMode,iDelay);
            }
            hr = S_OK;
        }
        break;
        case MSG_GET_CAMERA_BASIC_INFO:
        {
            SW_TRACE_DEBUG("<netcmd>MSG_GET_CAMERA_BASIC_INFO.\n");
            (*ppvRetBuf) = swpa_mem_alloc(1024);
            if( *ppvRetBuf != NULL )
            {
                INT iContrast = 0;
                INT iSaturation = 0;
                INT iSharpness = 0;
                INT iDCAperture = 0;
                INT iWDRLevel = 0;
                INT iAWBEnable = 0;
                INT iAGCEnable = 0;
                INT iACSyncMode = 0;
                INT iACSyncDelay = 0;
                INT iTNFEnable = 0;
                INT iSNFEnable = 0;
                INT iDeNoiseMode = 0;
                INT iDeNoiseLevel = 0;
                INT iGrayImageEnable = 0;

                INT iShutterRange = 0;
                INT iGainRange = 0;
                INT iAGCLightBaseLine = 0;

                //INT iFilterMode = 0;
               // INT iWDREnable = 0;
               // INT iDeNoiseEnable = 0;
                INT iSharpnessEnable = 0;
                INT iImageEnhancementEnable = 0;
                INT iGammaEnable = 0;
                INT iGammaStrength = 0;
                INT iEdgeEnhance = 0;

                INT iManualParamValue[5] = {0};//ManualShutter ManualGain ManualGainR ManualGainG ManualGainB
                INT iFilterMode = 0;
                CSWMessage::SendMessage(MSG_GET_CONTRASTTHRESHOLD,      (WPARAM)NULL, (LPARAM)&iContrast);
                CSWMessage::SendMessage(MSG_GET_SATURATIONTHRESHOLD,    (WPARAM)NULL, (LPARAM)&iSaturation);
                CSWMessage::SendMessage(MSG_GET_SHARPENTHRESHOLD,       (WPARAM)NULL, (LPARAM)&iSharpness);
                CSWMessage::SendMessage(MSG_GET_SHARPEN_ENABLE,         (WPARAM)NULL, (LPARAM)&iSharpnessEnable);
                CSWMessage::SendMessage(MSG_GET_DCAPERTUREENABLE,       (WPARAM)NULL, (LPARAM)&iDCAperture);
                CSWMessage::SendMessage(MSG_GET_WDR_STRENGTH,           (WPARAM)NULL, (LPARAM)&iWDRLevel);
                CSWMessage::SendMessage(MSG_GET_AWBENABLE,              (WPARAM)NULL, (LPARAM)&iAWBEnable);
                CSWMessage::SendMessage(MSG_GET_AGCENABLE,              (WPARAM)NULL, (LPARAM)&iAGCEnable);
                CSWMessage::SendMessage(MSG_GET_AC_SYNC_ENABLE,         (WPARAM)NULL, (LPARAM)&iACSyncMode);
                CSWMessage::SendMessage(MSG_GET_AC_SYNC_DELAY,          (WPARAM)NULL, (LPARAM)&iACSyncDelay);
                CSWMessage::SendMessage(MSG_GET_TNF_ENABLE,             (WPARAM)NULL, (LPARAM)&iTNFEnable);
                CSWMessage::SendMessage(MSG_GET_SNF_ENABLE,             (WPARAM)NULL, (LPARAM)&iSNFEnable);
                CSWMessage::SendMessage(MSG_GET_DENOISE_MODE,           (WPARAM)NULL, (LPARAM)&iDeNoiseMode);
                CSWMessage::SendMessage(MSG_GET_DENOISE_STATE,          (WPARAM)NULL, (LPARAM)&iDeNoiseLevel);
                CSWMessage::SendMessage(MSG_GET_GRAY_IMAGE_ENABLE,      (WPARAM)NULL, (LPARAM)&iGrayImageEnable);

                CSWMessage::SendMessage(MSG_GET_GAIN_RANGE,             (WPARAM)NULL, (LPARAM)&iGainRange);
                CSWMessage::SendMessage(MSG_GET_SHU_RANGE,              (WPARAM)NULL, (LPARAM)&iShutterRange);
                CSWMessage::SendMessage(MSG_GET_AGCTH,                  (WPARAM)NULL, (LPARAM)&iAGCLightBaseLine);
                CSWMessage::SendMessage(MSG_GET_COLORMATRIX_ENABLE,     (WPARAM)NULL, (LPARAM)&iImageEnhancementEnable);

                CSWMessage::SendMessage(MSG_GET_GAMMA_ENABLE,           (WPARAM)NULL, (LPARAM)&iGammaEnable);
                CSWMessage::SendMessage(MSG_GET_GAMMA_STRENGTH,         (WPARAM)NULL, (LPARAM)&iGammaStrength);
                CSWMessage::SendMessage(MSG_GET_EDGE_ENHANCE,           (WPARAM)NULL, (LPARAM)&iEdgeEnhance);

                CSWMessage::SendMessage(MSG_GET_MANUAL_VALUE,           (WPARAM)NULL, (LPARAM)iManualParamValue);
                CSWMessage::SendMessage(MSG_GET_FILTERSWITCH,           (WPARAM)NULL, (LPARAM)&iFilterMode);


                INT iGainMin = iGainRange & 0xffff;
                INT iGainMax= (iGainRange & 0xffff0000) >> 16;
                INT iShutterMin= iShutterRange & 0xffff;
                INT iShutterMax = (iShutterRange & 0xffff0000) >> 16;
                swpa_sprintf((CHAR*)(*ppvRetBuf), "Contrast=[%d],Saturation=[%d],Sharpness=[%d],DCEnable=[%d],WDRLevel=[%d],AWBEnable=[%d],"\
                             "AGCEnable=[%d],ACSyncMode=[%d],ACSyncDelay=[%d],DeNoiseTNFEnable=[%d],DeNoiseSNFEnable=[%d],"\
                             "DeNoiseMode=[%d],DeNoiseLevel=[%d],GrayImageEnable=[%d],AGCGainMin=[%d],AGCGainMax=[%d],AGCShutterMin=[%d],AGCShutterMax=[%d],AGCLightBaseLine=[%d],"\
                             "SharpnessEnable=[%d],ImageEnhancementEnable=[%d],GammaEnable=[%d],GammaValue=[%d],EdgeEnhance=[%d],"\
                             "ManualShutter=[%d],ManualGain=[%d],ManualGainR=[%d],ManualGainG=[%d],ManualGainB=[%d],FilterMode=[%d]",
                             iContrast,iSaturation,iSharpness,iDCAperture,iWDRLevel,iAWBEnable,
                             iAGCEnable,iACSyncMode,iACSyncDelay,iTNFEnable,iSNFEnable,
                             iDeNoiseMode,iDeNoiseLevel,iGrayImageEnable,iGainMin,iGainMax,iShutterMin,iShutterMax,iAGCLightBaseLine,
                             iSharpnessEnable,iImageEnhancementEnable,iGammaEnable,iGammaStrength,iEdgeEnhance,
                             iManualParamValue[0],iManualParamValue[1],iManualParamValue[2],iManualParamValue[3],iManualParamValue[4],iFilterMode);
            }
            hr = S_OK;
        }
        break;
        case MSG_GET_CAMERA_STATE:
        {
            SW_TRACE_DEBUG("<netcmd>MSG_GET_CAMERA_STATE.\n");
             (*ppvRetBuf) = swpa_mem_alloc(1024);
            if( *ppvRetBuf != NULL )
            {
                INT iAGCEnable = 0;
                INT iAWBEnable = 0;
                INT iShutter = 0;
                INT iGain = 0;
                INT GainRGB[3] = {0};

                CSWMessage::SendMessage(MSG_GET_AGCENABLE,(WPARAM)NULL, (LPARAM)&iAGCEnable);
                CSWMessage::SendMessage(MSG_GET_AWBENABLE,(WPARAM)NULL, (LPARAM)&iAWBEnable);
                CSWMessage::SendMessage(MSG_GET_SHUTTER,  (WPARAM)NULL, (LPARAM)&iShutter);
                CSWMessage::SendMessage(MSG_GET_AGCGAIN,  (WPARAM)NULL, (LPARAM)&iGain);
                CSWMessage::SendMessage(MSG_GET_RGBGAIN,  (WPARAM)NULL, (LPARAM)GainRGB);
                INT iGainB = GainRGB[2];
                INT iGainG = GainRGB[1];
                INT iGainR = GainRGB[0];
                swpa_sprintf((CHAR*)(*ppvRetBuf), "AGCEnable=[%d],AWBEnable=[%d],Shutter=[%d],Gain=[%d],GainR=[%d],GainG=[%d],"\
                             "GainB=[%d]",
                             iAGCEnable,iAWBEnable,iShutter,iGain,iGainR,iGainG,
                             iGainB);
            }
            hr = S_OK;
        }
        break;

        case MSG_GET_DEV_BASIC_INFO:
        {
            SW_TRACE_DEBUG("<netcmd>MSG_Get_DEV_BASIC_INFO.\n");
             (*ppvRetBuf) = swpa_mem_alloc(1024);
            if( *ppvRetBuf != NULL )
            {
                CHAR* pszNetInfo[4];
                CHAR szIP[256] = {0};
                CHAR szNetMask[256] = {0};
                CHAR szMAC[256] = {0};
                CHAR szGateway[256] = {0};
                pszNetInfo[0] = szIP;
                pszNetInfo[1] = szNetMask;
                pszNetInfo[2] = szGateway;
                pszNetInfo[3] = szMAC;

                CHAR szSN[256] = {0};
                CHAR szDevType[256]     = {0};
                CHAR szWorkMode[256]    = {0};
                INT  iMode              = 0;
                CHAR szDevVersion[256]  = {0};
                CHAR szBackupVersion[256]  = {0};
                CHAR szFPGAVersion[256]  = {0};
                CHAR szKernelVersion[256]  = {0};
                CHAR szUbootVersion[256]  = {0};
                CHAR szUBLVersion[256]  = {0};
				CHAR szFirmwareVersion[256]  = {0};
                CHAR szNetPackageVersion[256]  = {0};
                CSWMessage::SendMessage(MSG_APP_GETIP,              (WPARAM)NULL,   (LPARAM)pszNetInfo);
                CSWMessage::SendMessage(MSG_APP_GETSN,              (WPARAM)NULL,   (LPARAM)&szSN);
                CSWMessage::SendMessage(MSG_APP_GETDEVTYPE,         (WPARAM)NULL,   (LPARAM)szDevType);
                CSWMessage::SendMessage(MSG_APP_GETWORKMODE,        (WPARAM)NULL,   (LPARAM)&szWorkMode);
                CSWMessage::SendMessage(MSG_APP_RESET_MODE,         (WPARAM)NULL,   (LPARAM)&iMode);
                CSWMessage::SendMessage(MSG_APP_GETVERSION,         (WPARAM)NULL,   (LPARAM)&szDevVersion);
                CSWMessage::SendMessage(MSG_APP_GETBACKUPTVERSION,  (WPARAM)NULL,   (LPARAM)&szBackupVersion);
                CSWMessage::SendMessage(MSG_APP_GETFPGAVERSION,     (WPARAM)NULL,   (LPARAM)&szFPGAVersion); 
                CSWMessage::SendMessage(MSG_APP_GETKERNALVERSION,   (WPARAM)NULL,   (LPARAM)&szKernelVersion);
                CSWMessage::SendMessage(MSG_APP_GETUBOOTVERSION,    (WPARAM)NULL,   (LPARAM)&szUbootVersion);
                CSWMessage::SendMessage(MSG_APP_GETUBLVERSION,      (WPARAM)NULL,   (LPARAM)&szUBLVersion);
				CSWMessage::SendMessage(MSG_APP_GETFIRMWAREVERSION,	(WPARAM)NULL,	(LPARAM)&szFirmwareVersion);
				CSWMessage::SendMessage(MSG_APP_GETNETPACKAGEVERSION,	(WPARAM)NULL,	(LPARAM)&szNetPackageVersion);

                swpa_sprintf((CHAR*)(*ppvRetBuf), "IP=[%s],Mask=[%s],Gateway=[%s],Mac=[%s],SN=[%s],DevType=[%s],"\
                             "Mode=[%d],WorkMode=[%s],DevVersion=[%s],BackupVersion=[%s],FPGAVersion=[%s],"\
                             "KernelVersion=[%s],UbootVersion=[%s],UBLVersion=[%s],FirmwareVersion=[%s],NetPackageVersion=[%s]",
                             szIP,szNetMask,szGateway,szMAC,szSN,szDevType,
                             iMode,szWorkMode,szDevVersion,szBackupVersion,szFPGAVersion,
                             szKernelVersion,szUbootVersion,szUBLVersion,szFirmwareVersion,szNetPackageVersion);
                //printf("===%d\n %s",__LINE__,(CHAR*)(*ppvRetBuf));
            }
            hr = S_OK;
        }
        break;
        case MSG_GET_VIDEO_STATE:
        {
            SW_TRACE_DEBUG("<netcmd>MSG_GET_VIDEO_STATE.\n");
             (*ppvRetBuf) = swpa_mem_alloc(1024);
            if( *ppvRetBuf != NULL )
            {
                INT iH264BitRate = 0;
                INT iJpegCompressRate = 0;
                INT iCVBSDisplayMode = 0;
                INT iAutoJpegCompressEnable = 0;
                INT iJpegCompressParam[3] = {0};
                INT iDebugJpegStatus = 0;

                CSWMessage::SendMessage(MSG_GET_H264_BITRATE,               (WPARAM)NULL,   (LPARAM)&iH264BitRate);
                CSWMessage::SendMessage(MSG_JPEGENCODE_GET_COMPRESSRATE,    (WPARAM)NULL,   (LPARAM)&iJpegCompressRate);
                CSWMessage::SendMessage(MSG_GET_CVBS_STD,                   (WPARAM)NULL,   (LPARAM)&iCVBSDisplayMode);
                CSWMessage::SendMessage(MSG_GET_AUTO_JPEG_COMPRESS_ENABLE,  (WPARAM)NULL,   (LPARAM)&iAutoJpegCompressEnable);
                CSWMessage::SendMessage(MSG_GET_AUTO_JPEG_COMPRESS_PARAM,   (WPARAM)NULL,   (LPARAM)&iJpegCompressParam);
                CSWMessage::SendMessage(MSG_GET_DEBUG_JPEG_STATUS,          (WPARAM)NULL,   (LPARAM)&iDebugJpegStatus);
              
                INT iJpegCompressMaxRate    = iJpegCompressParam[0];
                INT iJpegCompressMinRate    = iJpegCompressParam[1];
				INT iJpegFileSize           = iJpegCompressParam[2];
                swpa_sprintf((CHAR*)(*ppvRetBuf), "H264BitRate=[%d],JpegCompressRate=[%d],CVBSDisplayMode=[%d],"\
                             "AutoJpegCompressEnable=[%d],JpegFileSize=[%d],JpegCompressMaxRate=[%d],JpegCompressMinRate=[%d],"\
                             "DebugJpegStatus=[%d]",
                             iH264BitRate,iJpegCompressRate,iCVBSDisplayMode,
                             iAutoJpegCompressEnable,iJpegFileSize,iJpegCompressMaxRate,iJpegCompressMinRate,
                             iDebugJpegStatus);

            }
            hr = S_OK;
        }
        break;
        case MSG_GET_H264_CAPTION:
        {
            (*ppvRetBuf) = swpa_mem_alloc(1024);
           if( *ppvRetBuf != NULL )
           {
                INT iOSDEnable = 0;
                INT iOSDPlateEnable = 0;
                INT iOSDTimeEnable = 0;
                CHAR szOSDText[256] = {0};
                INT iOSDFontSize = 0;
                INT iOSDFontColorRGB[3] = {0};
                INT iOSDPos[2] = {0}; //0 X 1 Y
                CSWMessage::SendMessage(MSG_OSD_GET_H264_ENABLE,        (WPARAM)NULL,   (LPARAM)&iOSDEnable);
                CSWMessage::SendMessage(MSG_OSD_GET_H264_PLATE_ENABLE,  (WPARAM)NULL,   (LPARAM)&iOSDPlateEnable);
                CSWMessage::SendMessage(MSG_OSD_GET_H264_TIME_ENABLE,   (WPARAM)NULL,   (LPARAM)&iOSDTimeEnable);
                CSWMessage::SendMessage(MSG_OSD_GET_H264_TEXT,          (WPARAM)NULL,   (LPARAM)szOSDText);
                CSWMessage::SendMessage(MSG_OSD_GET_H264_FONNTSIZE,     (WPARAM)NULL,   (LPARAM)&iOSDFontSize);
                CSWMessage::SendMessage(MSG_OSD_GET_H264_FONT_RBG,      (WPARAM)NULL,   (LPARAM)iOSDFontColorRGB);
                CSWMessage::SendMessage(MSG_OSD_GET_H264_POX,           (WPARAM)NULL,   (LPARAM)&iOSDPos);
                INT iOSDFontColorR = iOSDFontColorRGB[0];
                INT iOSDFontColorG = iOSDFontColorRGB[1];
                INT iOSDFontColorB = iOSDFontColorRGB[2];
                swpa_sprintf((CHAR*)(*ppvRetBuf), "OSDEnable=[%d],PlateEnable=[%d],TimeStampEnable=[%d],Text=[%s],FontSize=[%d],"\
                                                "ColorR=[%d],ColorG=[%d],ColorB=[%d],PosX=[%d],PosY=[%d]",
                                                iOSDEnable,iOSDPlateEnable,iOSDTimeEnable,szOSDText,iOSDFontSize,
                                                iOSDFontColorR,iOSDFontColorG,iOSDFontColorB,iOSDPos[0],iOSDPos[1]);


           }
            hr = S_OK;
        }
        break;
		case MSG_GET_H264_SECOND_CAPTION:
		{
			 (*ppvRetBuf) = swpa_mem_alloc(1024);
			if( *ppvRetBuf != NULL )
			{
				 INT iOSDEnable = 0;
				 INT iOSDPlateEnable = 0;
				 INT iOSDTimeEnable = 0;
				 CHAR szOSDText[256] = {0};
				 INT iOSDFontSize = 0;
				 INT iOSDFontColorRGB[3] = {0};
				 INT iOSDPos[2] = {0}; //0 X 1 Y
				 CSWMessage::SendMessage(MSG_OSD_GET_H264_SECOND_ENABLE,		 (WPARAM)NULL,	 (LPARAM)&iOSDEnable);
				 CSWMessage::SendMessage(MSG_OSD_GET_H264_SECOND_PLATE_ENABLE,  (WPARAM)NULL,	 (LPARAM)&iOSDPlateEnable);
				 CSWMessage::SendMessage(MSG_OSD_GET_H264_SECOND_TIME_ENABLE,	 (WPARAM)NULL,	 (LPARAM)&iOSDTimeEnable);
				 CSWMessage::SendMessage(MSG_OSD_GET_H264_SECOND_TEXT, 		 (WPARAM)NULL,	 (LPARAM)szOSDText);
				 CSWMessage::SendMessage(MSG_OSD_GET_H264_SECOND_FONNTSIZE,	 (WPARAM)NULL,	 (LPARAM)&iOSDFontSize);
				 CSWMessage::SendMessage(MSG_OSD_GET_H264_SECOND_FONT_RBG, 	 (WPARAM)NULL,	 (LPARAM)iOSDFontColorRGB);
				 CSWMessage::SendMessage(MSG_OSD_GET_H264_SECOND_POX,			 (WPARAM)NULL,	 (LPARAM)&iOSDPos);
				 INT iOSDFontColorR = iOSDFontColorRGB[0];
				 INT iOSDFontColorG = iOSDFontColorRGB[1];
				 INT iOSDFontColorB = iOSDFontColorRGB[2];
				 swpa_sprintf((CHAR*)(*ppvRetBuf), "OSDEnable=[%d],PlateEnable=[%d],TimeStampEnable=[%d],Text=[%s],FontSize=[%d],"\
												 "ColorR=[%d],ColorG=[%d],ColorB=[%d],PosX=[%d],PosY=[%d]",
												 iOSDEnable,iOSDPlateEnable,iOSDTimeEnable,szOSDText,iOSDFontSize,
												 iOSDFontColorR,iOSDFontColorG,iOSDFontColorB,iOSDPos[0],iOSDPos[1]);
			
			
			}
			 hr = S_OK;
		}
		break;
        case MSG_GET_JPEG_CAPTION:
        {
            (*ppvRetBuf) = swpa_mem_alloc(1024);
            if( *ppvRetBuf != NULL )
            {
                INT iOSDEnable = 0;
                INT iOSDPlateEnable = 0;
                INT iOSDTimeEnable = 0;
                CHAR szOSDText[256] = {0};
                INT iOSDFontSize = 0;
                INT iOSDFontColorRGB[3] = {0};
                INT iOSDPos[2] = {0}; //0 X 1 Y
                CSWMessage::SendMessage(MSG_OSD_GET_JPEG_ENABLE,        (WPARAM)NULL,   (LPARAM)&iOSDEnable);
                CSWMessage::SendMessage(MSG_OSD_GET_JPEG_PLATE_ENABLE,  (WPARAM)NULL,   (LPARAM)&iOSDPlateEnable);
                CSWMessage::SendMessage(MSG_OSD_GET_JPEG_TIME_ENABLE,   (WPARAM)NULL,   (LPARAM)&iOSDTimeEnable);
                CSWMessage::SendMessage(MSG_OSD_GET_JPEG_TEXT,          (WPARAM)NULL,   (LPARAM)szOSDText);
                CSWMessage::SendMessage(MSG_OSD_GET_JPEG_FONNTSIZE,     (WPARAM)NULL,   (LPARAM)&iOSDFontSize);
                CSWMessage::SendMessage(MSG_OSD_GET_JPEG_FONT_RBG,      (WPARAM)NULL,   (LPARAM)iOSDFontColorRGB);
                CSWMessage::SendMessage(MSG_OSD_GET_JPEG_POX,           (WPARAM)NULL,   (LPARAM)&iOSDPos);
                INT iOSDFontColorR = iOSDFontColorRGB[0];
                INT iOSDFontColorG = iOSDFontColorRGB[1];
                INT iOSDFontColorB = iOSDFontColorRGB[2];
                swpa_sprintf((CHAR*)(*ppvRetBuf), "OSDEnable=[%d],PlateEnable=[%d],TimeStampEnable=[%d],Text=[%s],FontSize=[%d],"\
                             "ColorR=[%d],ColorG=[%d],ColorB=[%d],PosX=[%d],PosY=[%d]",
                             iOSDEnable,iOSDPlateEnable,iOSDTimeEnable,szOSDText,iOSDFontSize,
                             iOSDFontColorR,iOSDFontColorG,iOSDFontColorB,iOSDPos[0],iOSDPos[1]);
            }
             hr = S_OK;
        }
        break;

        case MSG_GET_DEVSTATE://CPUUsage=[50],MemUsage=[39],Temperature=[40],IPCount=[2],IPList=[172.18.10.120,172.18.10.120],Date=[2013.12.08],Time=[08:50:38 627],HddState=[3],ResetCount=[23
        {
            SW_TRACE_DEBUG("<netcmd>MSG_GET_DEVSTATE.\n");
            (*ppvRetBuf) = swpa_mem_alloc(1024);
            if( *ppvRetBuf != NULL )
            {
                INT iCPUUsage = 0;
                INT iMemUsage = 0;
                INT iTemperature = 0;
                INT iResetCount = 0;
                CHAR szDate[256]    = {0};
                CHAR szTime[256]    = {0};
                CHAR szRecordLinkIP[256]= {0};
                CHAR szImageLinkIP[256]= {0};
                CHAR szVideoLinkIP[256]= {0};
                INT iTimeZone = 0;
                INT iNtpEnable = 0;
                CHAR szNtpServerIP[256]= {0};
                INT iNtpInterval = 0;
                INT iLogLevel = 0;
                CHAR szHddState[256]= {0};

                CSWMessage::SendMessage(MSG_APP_GETCPU,                 (WPARAM)NULL,   (LPARAM)&iCPUUsage);
                CSWMessage::SendMessage(MSG_APP_GETRAM,                 (WPARAM)NULL,   (LPARAM)&iMemUsage);
                CSWMessage::SendMessage(MSG_APP_GETCPU_TEMPERATURE,     (WPARAM)NULL,   (LPARAM)&iTemperature);
                CSWMessage::SendMessage(MSG_APP_HDD_STATUS,             (WPARAM)NULL,   (LPARAM)&szHddState);
                CSWMessage::SendMessage(MSG_APP_RESET_COUNT,            (WPARAM)NULL,   (LPARAM)&iResetCount);
                GetConnectedIPInfo(szRecordLinkIP,szImageLinkIP,szVideoLinkIP);
                CSWMessage::SendMessage(MAG_APP_GET_NTP_TIMEZONE,       (WPARAM)NULL,   (LPARAM)&iTimeZone);
                CSWMessage::SendMessage(MAG_APP_GET_NTP_ENABLE,         (WPARAM)NULL,   (LPARAM)&iNtpEnable);
                CSWMessage::SendMessage(MAG_APP_GET_NTP_SERVER_IP,      (WPARAM)NULL,   (LPARAM)&szNtpServerIP);
                CSWMessage::SendMessage(MAG_APP_GET_NTP_INTERVAL,       (WPARAM)NULL,   (LPARAM)&iNtpInterval);
                CSWMessage::SendMessage(MSG_APP_GET_LOG_LEVEL,           (WPARAM)NULL,   (LPARAM)&iLogLevel);

                SWPA_DATETIME_TM sRealTime;
                swpa_memset(&sRealTime, 0, sizeof(sRealTime));
                CSWMessage::SendMessage(MSG_APP_GETTIME,(WPARAM)NULL,(LPARAM)&sRealTime);
                swpa_snprintf(szDate, sizeof(szDate)-1, "%4d.%02d.%02d",sRealTime.year, sRealTime.month, sRealTime.day);
                swpa_snprintf(szTime, sizeof(szTime)-1, "%02d:%02d:%02d %03d",
                              sRealTime.hour, sRealTime.min, sRealTime.sec, sRealTime.msec);

                swpa_sprintf((CHAR*)(*ppvRetBuf), "CpuUsage=[%d],MemUsage=[%d],CpuTemperature=[%d],HddOpStatus=[%s],ResetCount=[%d],"
                             "Date=[%s],Time=[%s],RecordLinkIP=[%s],ImageLinkIP=[%s],VideoLinkIP=[%s],"
                             "TimeZone=[%d],NTPEnable=[%d],NTPServerIP=[%s],NTPServerUpdateInterval=[%d],TraceRank=[%d]",
                             iCPUUsage,iMemUsage,iTemperature,szHddState,iResetCount,
                             szDate,szTime,szRecordLinkIP,szImageLinkIP,szVideoLinkIP,
                             iTimeZone,iNtpEnable,szNtpServerIP,iNtpInterval,iLogLevel);



            }
            hr = S_OK;
        }
        break;

		case MSG_APP_GETCONNECTED_IP:
		{
			SW_TRACE_DEBUG("<netcmd>MSG_APP_GETCONNECTED_IP.\n");
			
			(*ppvRetBuf) = swpa_mem_alloc(256);
			if( *ppvRetBuf != NULL )
			{
				CHAR szInfo[256];
				CSWString strRet;
				CSWMessage::SendMessage(MSG_APP_GETCONNECTED_IP, (WPARAM)szInfo);

				if( swpa_strlen(szInfo) > 0 )
				{
					CSWString strInfo = szInfo;
					INT iCount = 0;

					INT iPrePos = strInfo.Find("结果链接:");
					INT iEndPos = -1;
					if( iPrePos != -1 )
					{
						INT iBeginPos = iPrePos;
						iEndPos = strInfo.Find("\n", iBeginPos);
						if( iEndPos != -1 )
						{
							INT iStrPos = strInfo.Find("_", iPrePos);
							if( iStrPos != -1 )
							{
								INT iEndStrPos = strInfo.Find("_", iStrPos + 1);
								while(iEndStrPos != -1 && iEndStrPos < iEndPos)
								{
									CSWString strIp;
									strIp.Format("[%s],", (const CHAR*)(strInfo.Substr(iStrPos + 1, iEndStrPos - iStrPos - 1)) );
									++iCount;
									iStrPos = iEndStrPos;
									iEndStrPos = strInfo.Find("_", iStrPos + 1);
									strRet += strIp;
								}
							}
						}
					}
					
					iPrePos = strInfo.Find("JPEG链接:");
					iEndPos = -1;
					if( iPrePos != -1 )
					{
						INT iBeginPos = iPrePos;
						iEndPos = strInfo.Find("\n", iBeginPos);
						if( iEndPos != -1 )
						{
							INT iStrPos = strInfo.Find("_", iPrePos);
							if( iStrPos != -1 )
							{
								INT iEndStrPos = strInfo.Find("_", iStrPos + 1);
								while(iEndStrPos != -1 && iEndStrPos < iEndPos)
								{
									CSWString strIp;
									strIp.Format("[%s],", (const CHAR*)(strInfo.Substr(iStrPos + 1, iEndStrPos - iStrPos - 1)) );
									++iCount;
									iStrPos = iEndStrPos;
									iEndStrPos = strInfo.Find("_", iStrPos + 1);
									strRet += strIp;
								}
							}
						}
					}

					iPrePos = strInfo.Find("H264链接:");
					iEndPos = -1;
					if( iPrePos != -1 )
					{
						INT iBeginPos = iPrePos;
						iEndPos = strInfo.Find("\n", iBeginPos);
						if( iEndPos != -1 )
						{
							INT iStrPos = strInfo.Find("_", iPrePos);
							if( iStrPos != -1 )
							{
								INT iEndStrPos = strInfo.Find("_", iStrPos + 1);
								while(iEndStrPos != -1 && iEndStrPos < iEndPos)
								{
									CSWString strIp;
									strIp.Format("[%s],", (const CHAR*)(strInfo.Substr(iStrPos + 1, iEndStrPos - iStrPos - 1)) );
									++iCount;
									iStrPos = iEndStrPos;
									iEndStrPos = strInfo.Find("_", iStrPos + 1);
									strRet += strIp;
								}
							}
						}
					}

					CSWString strResult;
					
					strResult.Format("COUNT[%d],%s", iCount, (const CHAR*)strRet);

					swpa_strcpy((CHAR*)(*ppvRetBuf), (const CHAR*)strResult);

				}
				else
				{
					swpa_strcpy((CHAR*)(*ppvRetBuf), "COUNT[1],[0.0.0.0,0]");
				}
				
			}
			hr = S_OK;
		}
		break;

		case MSG_USER_LOGIN:
		{
			SW_TRACE_DEBUG("<netcmd>MSG_USER_LOGIN.\n");
			(*ppvRetBuf) = swpa_mem_alloc(128);
			if ( *ppvRetBuf != NULL )
			{
				INT iAuthority = -1; 
				hr = CSWMessage::SendMessage(MSG_USER_LOGIN,(WPARAM)szValue,(LPARAM)&iAuthority);
				if (-1 == iAuthority)
				{
					SW_TRACE_NORMAL("Login Failed\n");
					hr = E_FAIL;
				}
				CSWString strResult;
				strResult.Format("Authority=[%d]", iAuthority);
				swpa_strcpy((CHAR *)(*ppvRetBuf),(const CHAR *)strResult);
			}
		}
		break;

		case MSG_USER_ADD:
		{
			SW_TRACE_DEBUG("<netcmd>MSG_USER_ADD.\n");
			hr = CSWMessage::SendMessage(MSG_USER_ADD,(WPARAM)szValue,(LPARAM)NULL);
		}
		break;
		
		case MSG_USER_DEL:
		{
			SW_TRACE_DEBUG("<netcmd>MSG_USER_DEL.\n");
			hr = CSWMessage::SendMessage(MSG_USER_DEL,(WPARAM)szValue,(LPARAM)NULL);		
		}
		break;

		case MSG_USER_CHANGE:
		{
			SW_TRACE_DEBUG("<netcmd>MSG_USER_CHANGE.\n");

			hr = CSWMessage::SendMessage(MSG_USER_CHANGE,(WPARAM)szValue,(LPARAM)NULL);	
		}
		break;

		case MSG_USER_GETLIST:
		{
			SW_TRACE_DEBUG("<netcmd>MSG_USER_GETLIST.\n");
			(*ppvRetBuf) = swpa_mem_alloc(2048);
            if( *ppvRetBuf != NULL )
            {
				hr = CSWMessage::SendMessage(MSG_USER_GETLIST,(WPARAM)NULL,(LPARAM)*ppvRetBuf);
			}
		}
		break;

		default:
		{
			hr = SendDomeCameraCustomCMD(dwCMDID, szValue, ppvRetBuf);
		}
		break;
	}

	return hr;
}


HRESULT CSWNetCommandProcess::SendCMD(const DWORD dwCMDID, const CHAR * szValue, const CHAR * szType, const CHAR* szClass, PVOID* ppvRetBuf)
{	
	HRESULT hr = S_OK;
	CHAR * pszType = (CHAR*)szType;
	CHAR * pszValue = (CHAR*)szValue;

	SAFE_MEM_FREE(*ppvRetBuf); //make sure "NULL == *ppvRetBuf"

	//PRINT("Info: sending %s Cmd...\n", SearchCmdNameByID(dwCMDID));
	
	if (0 == swpa_strcmp(pszType, "INT"))
	{
		INT iVal = 0;
		
		if (0 == swpa_stricmp(szClass, "Getter"))
		{
			*ppvRetBuf = swpa_mem_alloc(sizeof(INT));
			if (NULL == *ppvRetBuf)
			{
				PRINT("Err: no memory for *ppvRetBuf\n");
				return E_FAIL;
			}
		}
		else if (0 == swpa_stricmp(szClass, "Setter"))
		{
			if (NULL != pszValue)
			{
                iVal = swpa_atoi(pszValue);
			}
			else
			{
				PRINT("Err: NULL == pszValue for Setter\n");
				return E_INVALIDARG;
			}
		}

		hr = CSWMessage::SendMessage(dwCMDID,
			(WPARAM)iVal,
            (LPARAM)*ppvRetBuf);
		
	}
	else if (0 == swpa_strcmp(pszType, "DOUBLE"))
	{
		DOUBLE dblVal = 0;
		if (0 == swpa_stricmp(szClass, "Getter"))
		{
			*ppvRetBuf = swpa_mem_alloc(sizeof(DOUBLE));
			if (NULL == *ppvRetBuf)
			{
				PRINT("Err: no memory for *ppvRetBuf\n");
				return E_FAIL;
			}
		}
		else if (0 == swpa_stricmp(szClass, "Setter"))
		{
			if (NULL != pszValue)
			{
				dblVal = swpa_atof(pszValue);
			}
			else
			{
				PRINT("Err: NULL == pszValue for Setter\n");
				return E_INVALIDARG;
			}
		}
		hr = CSWMessage::SendMessage(dwCMDID,
			(WPARAM)dblVal,
			(LPARAM)*ppvRetBuf);
	}
	else if (0 == swpa_strcmp(pszType, "FLOAT"))
	{
		FLOAT fltVal = 0;

		if (0 == swpa_stricmp(szClass, "Getter"))
		{
			*ppvRetBuf = swpa_mem_alloc(sizeof(FLOAT));
			if (NULL == *ppvRetBuf)
			{
				PRINT("Err: no memory for *ppvRetBuf\n");
				return E_FAIL;
			}
		}
		else if (0 == swpa_stricmp(szClass, "Setter"))
		{
			if (NULL != pszValue)
			{
				fltVal = swpa_atof(pszValue);
			}
			else
			{
				PRINT("Err: NULL == pszValue for Setter\n");
				return E_INVALIDARG;
			}
		}
		hr = CSWMessage::SendMessage(dwCMDID,
			(WPARAM)fltVal,
			(LPARAM)*ppvRetBuf);
	}
	else if (0 == swpa_strcmp(pszType, "BOOL"))
	{
		BOOL boolVal = 0;
		
		if (0 == swpa_stricmp(szClass, "Getter"))
		{
			*ppvRetBuf = swpa_mem_alloc(sizeof(BOOL));
			if (NULL == *ppvRetBuf)
			{
				PRINT("Err: no memory for *ppvRetBuf\n");
				return E_FAIL;
			}
		}
		else if (0 == swpa_stricmp(szClass, "Setter"))
		{
			if (NULL != pszValue)
			{
				boolVal = swpa_atoi(pszValue);
			}
			else
			{
				PRINT("Err: NULL == pszValue for Setter\n");
				return E_INVALIDARG;
			}
		}
		hr = CSWMessage::SendMessage(dwCMDID,
			(WPARAM)boolVal,
			(LPARAM)*ppvRetBuf);
	}
	else if (0 == swpa_strcmp(pszType, "DWORD"))
	{
		DWORD dwVal = 0;
		
		if (0 == swpa_stricmp(szClass, "Getter"))
		{
			*ppvRetBuf = swpa_mem_alloc(sizeof(DWORD));
			if (NULL == *ppvRetBuf)
			{
				PRINT("Err: no memory for *ppvRetBuf\n");
				return E_FAIL;
			}
		}
		else if (0 == swpa_stricmp(szClass, "Setter"))
		{
			if (NULL != pszValue)
			{
				dwVal = swpa_atoi(pszValue);
			}
			else
			{
				PRINT("Err: NULL == pszValue for Setter\n");
				return E_INVALIDARG;
			}
		}
		hr = CSWMessage::SendMessage(dwCMDID,
			(WPARAM)dwVal,
			(LPARAM)*ppvRetBuf);
	}
	else if (0 == swpa_strcmp(pszType, "STRING"))
	{
		if (0 == swpa_stricmp(szClass, "Getter"))
		{
			*ppvRetBuf = swpa_mem_alloc(16*1024); //16KB by default
			if (NULL == *ppvRetBuf)
			{
				PRINT("Err: no memory for *ppvRetBuf\n");
				return E_FAIL;
			}
			swpa_memset(*ppvRetBuf, 0, 16*1024);
		}
		else if (0 == swpa_stricmp(szClass, "Setter"))
		{
			if (NULL != pszValue)
			{
				//szVal = pszValue;
			}
			else
			{
				PRINT("Err: NULL == pszValue for Setter\n");
				return E_INVALIDARG;
			}
		}
		hr = CSWMessage::SendMessage(dwCMDID,
			(WPARAM)pszValue,
			(LPARAM)*ppvRetBuf);
	}
	else if (0 == swpa_strcmp(pszType, "NULL"))
	{
		hr = CSWMessage::SendMessage(dwCMDID,
			(WPARAM)NULL,
			(LPARAM)NULL);
	}
	else if (0 == swpa_strcmp(pszType, "CUSTOM"))
	{
		//PRINT("Info: got CUSTOM type...\n");

		if (FAILED(SendCustomCMD(dwCMDID, szValue, ppvRetBuf)))
		{
			SW_TRACE_DEBUG("Err: failed to send Custom CMD %d.\n", dwCMDID);
			return E_FAIL;
		}		
	}
	else if (NULL != swpa_strstr(pszType, "INTARRAY1D"))
	{
		INT iArraySize = 0;

		if (NULL == pszValue)
		{
			PRINT("Err: NULL == pszValue\n");
			return E_INVALIDARG;
		}

		swpa_sscanf(pszType, "INTARRAY1D,SIZE:%d", &iArraySize);

		INT * piInput = NULL;
        INT* piBuf = NULL;
		if (0 == swpa_stricmp(szClass, "Getter"))
		{
            piBuf = (INT*)swpa_mem_alloc(iArraySize * sizeof(INT) + sizeof(INT));
            if (NULL == piBuf)
			{
				PRINT("Err: no memory for piBuf\n");
				return E_FAIL;
			}
            piBuf[0] = iArraySize;
            *ppvRetBuf = piBuf;

			hr = CSWMessage::SendMessage(dwCMDID,
				(WPARAM)piInput,
				(LPARAM)(piBuf+1));
		}
		else if (0 == swpa_stricmp(szClass, "Setter"))
		{
			if (NULL != pszValue)
			{
				piInput = (INT*)swpa_mem_alloc(iArraySize * sizeof(INT));
				swpa_memset(piInput, 0, sizeof(iArraySize * sizeof(INT)));
				if (NULL == piInput)
				{
					PRINT("Err: no memory for piInput\n");
					hr = E_OUTOFMEMORY;
					return hr;
				}
				
				for (INT i = 0; i < iArraySize; i++)
				{
					if (NULL == pszValue)
					{
						hr = E_FAIL;
						SAFE_MEM_FREE(piInput);
						return hr;
					}
					
					swpa_sscanf(pszValue, "%d", piInput+i);
					if (i+1 < iArraySize)
					{
						pszValue = swpa_strstr(pszValue, ",") + 1;
					}
				}

				hr = CSWMessage::SendMessage(dwCMDID,
                    (WPARAM)piInput,
					0);
			}
			else
			{
				PRINT("Err: NULL == pszValue for Setter\n");
				return E_INVALIDARG;
			}
		}

       // PRINT("%s %d  %d %d %d \n",__FUNCTION__,__LINE__,piBuf[1],piBuf[2],piBuf[3]);
        SAFE_MEM_FREE(piInput);
        
	}
	else if (0 == swpa_strcmp(pszType, "INTARRAY2D"))
	{
		INT iWidth = 0, iHeight = 0;

		if (NULL == pszValue)
		{
			PRINT("Err: NULL == pszValue\n");
			return E_INVALIDARG;
		}

		swpa_sscanf(pszValue, "SIZE:%d*%d", &iHeight, &iWidth);
		pszValue = swpa_strstr(pszValue, "DATA:") + 5;
		INT * piInput = (INT*)swpa_mem_alloc(iHeight* iWidth * sizeof(INT));
		if (NULL == piInput)
		{
			PRINT("Err: no memory for piInput\n");
			hr = E_OUTOFMEMORY;
			return hr;
		}
		swpa_memset(piInput, 0, sizeof(iHeight* iWidth	* sizeof(INT)));
		
		for (INT i = 0; i < iHeight; i++)
		{
			for (INT j = 0; j < iWidth; j++)
			{
				if (NULL == pszValue)
				{
					hr = E_FAIL;
					PRINT("Err: NULL == pszValue\n");
					SAFE_MEM_FREE(piInput);
					return hr;
				}
				
				swpa_sscanf(pszValue, "%d", piInput+i);
				pszValue = swpa_strstr(pszValue+1, ",");
			}
		}
		
		hr = CSWMessage::SendMessage(dwCMDID,
			(WPARAM)piInput,
			(LPARAM)*ppvRetBuf);

		SAFE_MEM_FREE(piInput);
	}


	//PRINT("Info: %s() dwCMDID = %u, hr = %#x\n", __FUNCTION__, dwCMDID, (UINT)hr);

	return hr;
}



HRESULT CSWNetCommandProcess::GenerateCMDReplyXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName, const CHAR * pszType, const CHAR * pszClass, const PVOID pvData)
{
	HRESULT hr = S_OK;
   
	if (NULL == pXmlOutputDoc || NULL == szCmdName || NULL == pszType || NULL == pszClass)//todo : NULL == pvData || 
	{
		PRINT("Err: pXmlOutputDoc = %p, szCmdName = %p, pvData = %p, pszType = %p, pszClass = %p\n",
			pXmlOutputDoc, szCmdName, pvData, pszType, pszClass);
		return E_INVALIDARG;
	}
    
	
	TiXmlElement *pRootEle = NULL;
	TiXmlElement *pReplyEle = NULL;	
	TiXmlText* pEleText = NULL;
	

	pRootEle = pXmlOutputDoc->RootElement();
	if (NULL == pRootEle)
	{		
		PRINT("Err: NULL == pRootEle\n");
		return E_INVALIDARG;
	}

	pReplyEle = new TiXmlElement(HVXML_CMDNAME);
	if (NULL == pReplyEle)
	{
		PRINT("Err: no memory for pReplyEle\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}

	
	pEleText = new TiXmlText(szCmdName);
	if (NULL == pEleText)
	{
		PRINT("Err: no memory for pEleText\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}
	pReplyEle->LinkEndChild(pEleText);
	
	pReplyEle->SetAttribute(HVXML_RETCODE, 0);
	

	if (0 == swpa_stricmp(pszClass, "Setter"))
	{
		pReplyEle->SetAttribute(HVXML_RETMSG, "OK");
	}
	else if (0 == swpa_stricmp(pszClass, "Getter"))
	{
		if (0 == swpa_strcmp(pszType, "INT"))
		{
			CHAR szVal[32] = {0};
			swpa_snprintf(szVal, sizeof(szVal)-1, "%d", *(INT*)pvData);
			pReplyEle->SetAttribute(HVXML_RETMSG, szVal);
		}
		else if (0 == swpa_strcmp(pszType, "DOUBLE"))
		{
			CHAR szVal[64] = {0};
			swpa_snprintf(szVal, sizeof(szVal)-1, "%lf", *(DOUBLE*)pvData);
			pReplyEle->SetAttribute(HVXML_RETMSG, szVal);
		}
		else if (0 == swpa_strcmp(pszType, "FLOAT"))
		{
			CHAR szVal[64] = {0};
			swpa_snprintf(szVal, sizeof(szVal)-1, "%f",  *(FLOAT*)pvData);
			pReplyEle->SetAttribute(HVXML_RETMSG, szVal);
		}
		else if (0 == swpa_strcmp(pszType, "BOOL"))
		{
			CHAR szVal[8] = {0};
			swpa_snprintf(szVal, sizeof(szVal)-1, "%d", *(BOOL*)pvData);
			pReplyEle->SetAttribute(HVXML_RETMSG, szVal);
		}
		else if (0 == swpa_strcmp(pszType, "DWORD"))
		{
			CHAR szVal[32] = {0};
			swpa_snprintf(szVal, sizeof(szVal)-1, "%u", *(DWORD*)pvData);
			pReplyEle->SetAttribute(HVXML_RETMSG, szVal);
		}
		else if (0 == swpa_strcmp(pszType, "STRING"))
		{
			CHAR* szVal = (CHAR*)pvData;
			//swpa_snprintf(szVal, sizeof(szVal)-1, "%s", (CHAR*)pvData);
			pReplyEle->SetAttribute(HVXML_RETMSG, szVal);
		}
		else if (0 == swpa_strcmp(pszType, "NULL"))
		{			
			pReplyEle->SetAttribute(HVXML_RETMSG, "OK");
		}
		else if (0 == swpa_strcmp(pszType, "CUSTOM"))
		{
		
			if (0 == swpa_strcmp("DateTime", szCmdName))
			{
				pReplyEle->SetAttribute(HVXML_RETMSG, (CHAR*)pvData);
			}
			else if (0 == swpa_strcmp("SetTime", szCmdName))
			{
				pReplyEle->SetAttribute(HVXML_RETMSG, "OK");
			}
			else if (0 == swpa_strcmp("GetIP", szCmdName))
			{
				pReplyEle->SetAttribute(HVXML_RETMSG, (CHAR*)pvData);
			}
			else if (0 == swpa_strcmp("SetIP", szCmdName))
			{
				pReplyEle->SetAttribute(HVXML_RETMSG, "OK");
			}
			else if (0 == swpa_strcmp("GetVersion", szCmdName))
			{
				pReplyEle->SetAttribute(HVXML_RETMSG, (CHAR*)pvData);
			}
			else if (0 == swpa_strcmp("GetDevType", szCmdName))
			{
				pReplyEle->SetAttribute(HVXML_RETMSG, (CHAR*)pvData);
			}
			else if( 0 == swpa_strcmp("GetConnectedIP", szCmdName) )
			{
				pReplyEle->SetAttribute(HVXML_RETMSG, (CHAR*)pvData);
			}
            else if( 0 == swpa_strcmp("ReadFPGA", szCmdName) )
            {
                CHAR szVal[32] = {0};
                swpa_snprintf(szVal, sizeof(szVal)-1, "%d", *(INT*)pvData);
                pReplyEle->SetAttribute(HVXML_RETMSG, (CHAR*)szVal);
            }
            else
                pReplyEle->SetAttribute(HVXML_RETMSG, (CHAR*)pvData);

			
		}
        else if (NULL != swpa_strstr(pszType, "INTARRAY1D"))
        {
            INT iArraySize = *((INT*)pvData);
            //PRINT("%s %d INTARRAY1D %d \n",__FUNCTION__,__LINE__,iArraySize);
			DWORD dwLen = (iArraySize) * 32 + (iArraySize)*sizeof(',') + 1;//todo
			CHAR* szVal = (CHAR*)swpa_mem_alloc(dwLen); 
			if (NULL == szVal)
			{
				PRINT("Err: no memory for szVal\n");
				return E_OUTOFMEMORY;
			}
			swpa_memset(szVal, 0, dwLen);
            //PRINT("%s %d INTARRAY1D size :%d \n",__FUNCTION__,__LINE__,iArraySize);
			for (INT i=0; i<iArraySize;  i++)
			{
				CHAR szValInt[16] = {0};
				swpa_memset(szValInt, 0, sizeof(szValInt));
				swpa_snprintf(szValInt, sizeof(szValInt),"%d,", *((INT*)pvData+1+i));
				swpa_strcat(szVal, szValInt);
			}
            //PRINT("%s %d INTARRAY1D szVal :%s \n",__FUNCTION__,__LINE__,szVal);
			pReplyEle->SetAttribute(HVXML_RETMSG, szVal);
			SAFE_MEM_FREE(szVal);
		}
        else if (NULL != swpa_strstr(pszType, "INTARRAY2D"))
		{
			//todo
		}

	}

	
	pRootEle->LinkEndChild(pReplyEle);

	
	return S_OK;
	
OUT:

	SAFE_DELETE(pReplyEle);
	
	return hr;
}



HRESULT CSWNetCommandProcess::GenerateCMDNotFoundXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName)
{
	HRESULT hr = S_OK;

	if (NULL == pXmlOutputDoc || NULL == szCmdName)
	{
		PRINT("Err: NULL == pXmlOutputDoc || NULL == szCmdName\n");
		return E_INVALIDARG;
	}

	
	TiXmlElement *pRootEle = NULL;
	TiXmlElement *pReplyEle = NULL;	
	CHAR szVal[256] = {0};


	pRootEle = pXmlOutputDoc->RootElement();
	if (NULL == pRootEle)
	{		
		PRINT("Err: NULL == pRootEle\n");
		return E_INVALIDARG;
	}

	pReplyEle = new TiXmlElement(HVXML_CMDNAME);
	TiXmlText* pTxtCmd = new TiXmlText(szCmdName);
	pReplyEle->LinkEndChild(pTxtCmd);
	if (NULL == pReplyEle)
	{
		PRINT("Err: no memory for pReplyEle\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}

	pReplyEle->SetAttribute(HVXML_RETCODE, -1);
	pReplyEle->SetAttribute(HVXML_RETMSG, "Command Not Found");
	
	pRootEle->LinkEndChild(pReplyEle);

	return S_OK;
	
OUT:

	SAFE_DELETE(pReplyEle);
	
	return hr;
}



HRESULT CSWNetCommandProcess::GenerateNoTypeErrorXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName)
{
	HRESULT hr = S_OK;

	if (NULL == pXmlOutputDoc || NULL == szCmdName)
	{
		PRINT("Err: NULL == pXmlOutputDoc || NULL == szCmdName\n");
		return E_INVALIDARG;
	}

	
	TiXmlElement *pRootEle = NULL;
	TiXmlElement *pReplyEle = NULL;	
	CHAR szVal[256] = {0};


	pRootEle = pXmlOutputDoc->RootElement();
	if (NULL == pRootEle)
	{		
		PRINT("Err: NULL == pRootEle\n");
		return E_INVALIDARG;
	}

	pReplyEle = new TiXmlElement(HVXML_CMDNAME);
	TiXmlText* pTxtCmd = new TiXmlText(szCmdName);
	pReplyEle->LinkEndChild(pTxtCmd);
	if (NULL == pReplyEle)
	{
		PRINT("Err: no memory for pReplyEle\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}

	pReplyEle->SetAttribute(HVXML_RETCODE, -1);
	pReplyEle->SetAttribute(HVXML_RETMSG, "No \"Type\" Attribute");
	
	pRootEle->LinkEndChild(pReplyEle);

	return S_OK;
	
OUT:

	SAFE_DELETE(pReplyEle);
	
	return hr;
}




HRESULT CSWNetCommandProcess::GenerateNoClassErrorXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName)
{
	HRESULT hr = S_OK;

	if (NULL == pXmlOutputDoc || NULL == szCmdName)
	{
		PRINT("Err: NULL == pXmlOutputDoc || NULL == szCmdName\n");
		return E_INVALIDARG;
	}

	
	TiXmlElement *pRootEle = NULL;
	TiXmlElement *pReplyEle = NULL;	
	CHAR szVal[256] = {0};


	pRootEle = pXmlOutputDoc->RootElement();
	if (NULL == pRootEle)
	{		
		PRINT("Err: NULL == pRootEle\n");
		return E_INVALIDARG;
	}

	pReplyEle = new TiXmlElement(HVXML_CMDNAME);
	TiXmlText* pTxtCmd = new TiXmlText(szCmdName);
	pReplyEle->LinkEndChild(pTxtCmd);
	if (NULL == pReplyEle)
	{
		PRINT("Err: no memory for pReplyEle\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}

	pReplyEle->SetAttribute(HVXML_RETCODE, -1);
	pReplyEle->SetAttribute(HVXML_RETMSG, "No \"Class\" Attribute");
	
	pRootEle->LinkEndChild(pReplyEle);

	return S_OK;
	
OUT:

	SAFE_DELETE(pReplyEle);
	
	return hr;
}





HRESULT CSWNetCommandProcess::GenerateNoValueErrorXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName)
{
	HRESULT hr = S_OK;

	if (NULL == pXmlOutputDoc || NULL == szCmdName)
	{
		PRINT("Err: NULL == pXmlOutputDoc || NULL == szCmdName\n");
		return E_INVALIDARG;
	}

	
	TiXmlElement *pRootEle = NULL;
	TiXmlElement *pReplyEle = NULL;	
	CHAR szVal[256] = {0};


	pRootEle = pXmlOutputDoc->RootElement();
	if (NULL == pRootEle)
	{		
		PRINT("Err: NULL == pRootEle\n");
		return E_INVALIDARG;
	}

	pReplyEle = new TiXmlElement(HVXML_CMDNAME);
	TiXmlText* pTxtCmd = new TiXmlText(szCmdName);
	pReplyEle->LinkEndChild(pTxtCmd);
	if (NULL == pReplyEle)
	{
		PRINT("Err: no memory for pReplyEle\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}

	pReplyEle->SetAttribute(HVXML_RETCODE, -1);
	pReplyEle->SetAttribute(HVXML_RETMSG, "No \"Value\" Attribute for SETTER");
	
	pRootEle->LinkEndChild(pReplyEle);

	return S_OK;
	
OUT:

	SAFE_DELETE(pReplyEle);
	
	return hr;
}





HRESULT CSWNetCommandProcess::GenerateNoRetLenErrorXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName)
{
	HRESULT hr = S_OK;

	if (NULL == pXmlOutputDoc || NULL == szCmdName)
	{
		PRINT("Err: NULL == pXmlOutputDoc || NULL == szCmdName\n");
		return E_INVALIDARG;
	}

	
	TiXmlElement *pRootEle = NULL;
	TiXmlElement *pReplyEle = NULL;	
	CHAR szVal[256] = {0};


	pRootEle = pXmlOutputDoc->RootElement();
	if (NULL == pRootEle)
	{		
		PRINT("Err: NULL == pRootEle\n");
		return E_INVALIDARG;
	}

	pReplyEle = new TiXmlElement(HVXML_CMDNAME);
	TiXmlText* pTxtCmd = new TiXmlText(szCmdName);
	pReplyEle->LinkEndChild(pTxtCmd);
	if (NULL == pReplyEle)
	{
		PRINT("Err: no memory for pReplyEle\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}

	pReplyEle->SetAttribute(HVXML_RETCODE, -1);
	pReplyEle->SetAttribute(HVXML_RETMSG, "No \"RetLen\" Attribute");
	
	pRootEle->LinkEndChild(pReplyEle);

	return S_OK;
	
OUT:

	SAFE_DELETE(pReplyEle);
	
	return hr;
}



HRESULT CSWNetCommandProcess::GenerateCMDProcessFailedXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName)
{
	HRESULT hr = S_OK;

	if (NULL == pXmlOutputDoc || NULL == szCmdName)
	{
		PRINT("Err: NULL == pXmlOutputDoc || NULL == szCmdName\n");
		return E_INVALIDARG;
	}

	
	TiXmlElement *pRootEle = NULL;
	TiXmlElement *pReplyEle = NULL;	
	CHAR szVal[256] = {0};


	pRootEle = pXmlOutputDoc->RootElement();
	if (NULL == pRootEle)
	{		
		PRINT("Err: NULL == pRootEle\n");
		return E_INVALIDARG;
	}

	pReplyEle = new TiXmlElement(HVXML_CMDNAME);
	TiXmlText* pTxtCmd = new TiXmlText(szCmdName);
	pReplyEle->LinkEndChild(pTxtCmd);
	if (NULL == pReplyEle)
	{
		PRINT("Err: no memory for pReplyEle\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}

	pReplyEle->SetAttribute(HVXML_RETCODE, -1);
	pReplyEle->SetAttribute(HVXML_RETMSG, "Failed to Process");
	
	pRootEle->LinkEndChild(pReplyEle);

	return S_OK;
	
OUT:

	SAFE_DELETE(pReplyEle);
	
	return hr;
}




HRESULT CSWNetCommandProcess::GenerateCMDParseFailedXml(TiXmlDocument * pXmlOutputDoc)
{
	HRESULT hr = S_OK;

	if (NULL == pXmlOutputDoc)
	{
		PRINT("Err: NULL == pXmlOutputDoc\n");
		return E_INVALIDARG;
	}

	
	TiXmlElement *pRootEle = NULL;
	//TiXmlElement *pReplyEle = NULL;	
	CHAR szVal[256] = {0};


	pRootEle = pXmlOutputDoc->RootElement();
	if (NULL == pRootEle)
	{		
		PRINT("Err: NULL == pRootEle\n");
		return E_INVALIDARG;
	}

	/*pReplyEle = new TiXmlElement(szCmdName);
	if (NULL == pReplyEle)
	{
		PRINT("Err: no memory for pReplyEle\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}*/

	pRootEle->SetAttribute(HVXML_RETCODE, -1);
	pRootEle->SetAttribute(HVXML_RETMSG, "Failed to Parse the XML");
	
	//pRootEle->LinkEndChild(pReplyEle);

	return S_OK;
	
OUT:

	/*SAFE_RELEASE(pReplyEle);*/
	
	return hr;
}






HRESULT CSWNetCommandProcess::GenerateReplyXMLHeader(TiXmlDocument ** ppXmlOutputDoc)
{
	HRESULT hr = S_OK;

	TiXmlDocument * pXmlOutputDoc = NULL;
	TiXmlDeclaration *pDeclaration = NULL;
	TiXmlElement *pRootEle = NULL;

	if (NULL == ppXmlOutputDoc)
	{
		hr = E_INVALIDARG;
		PRINT("Err: NULL == ppXmlOutputDoc\n");
		goto OUT;
	}
	
	pXmlOutputDoc = new TiXmlDocument();
	if (NULL == pXmlOutputDoc)
	{
		PRINT("Err: no memory for pXmlOutputDoc\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}	

	pDeclaration = new TiXmlDeclaration("1.0","GB2312","yes");
	if (NULL == pDeclaration)
	{
		PRINT("Err: no memory for pDeclaration\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}
	pXmlOutputDoc->LinkEndChild(pDeclaration);

	pRootEle = new TiXmlElement(HVXML_HVCMDRESPOND);
	if (NULL == pRootEle)
	{	
		PRINT("Err: no memory for pRootEle\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}

	pRootEle->SetAttribute(HVXML_VER, HV_XML_CMD_VERSION_NO);

	pXmlOutputDoc->LinkEndChild(pRootEle);

	*ppXmlOutputDoc = pXmlOutputDoc;

	return S_OK;

OUT:

	SAFE_DELETE(pXmlOutputDoc);

	return hr;	
	
}


HRESULT CSWNetCommandProcess::PackXml(TiXmlDocument* pXmlOutputDoc,  PVOID* ppvXMLOutBuf,  DWORD *pdwXMLOutSize)
{
	if (NULL == pXmlOutputDoc || NULL == ppvXMLOutBuf || NULL == pdwXMLOutSize)
	{
		PRINT("Err: NULL == pXmlOutputDoc || NULL == ppvXMLOutBuf || NULL == pdwXMLOutSize\n");
		return E_INVALIDARG;
	}
	
	CAMERA_CMD_RESPOND sCmdHeader;
	TiXmlPrinter XmlPrinter;
	pXmlOutputDoc->Accept(&XmlPrinter);

	sCmdHeader.dwID = CAMERA_XML_EXT_CMD;
	sCmdHeader.dwInfoSize = XmlPrinter.Size() + 1;
	sCmdHeader.iResult = 0;
	
	CHAR* pszXmlData = (CHAR*)swpa_mem_alloc(sCmdHeader.dwInfoSize + sizeof(sCmdHeader));
	if (NULL == pszXmlData)
	{
		PRINT("Err: no memory for pszXmlData\n");
		return E_OUTOFMEMORY;
	}
	

	swpa_memset(pszXmlData, 0x0, sCmdHeader.dwInfoSize + sizeof(sCmdHeader));
	swpa_memcpy(pszXmlData, &sCmdHeader, sizeof(sCmdHeader));
	swpa_memcpy(pszXmlData+sizeof(sCmdHeader), XmlPrinter.CStr(), sCmdHeader.dwInfoSize);
	

	*ppvXMLOutBuf = pszXmlData;
	*pdwXMLOutSize = sCmdHeader.dwInfoSize+sizeof(sCmdHeader);
	
	return S_OK;
}



HRESULT CSWNetCommandProcess::ProcessXmlCmd(const CHAR* pszXMLInBuf,  const DWORD dwXMLInSize, PVOID* ppvXMLOutBuf,  DWORD *pdwXMLOutSize, BOOL* pfReboot)
{
	HRESULT hr = S_OK;

	if (NULL == pszXMLInBuf || NULL == ppvXMLOutBuf || NULL == pdwXMLOutSize)
	{
		PRINT("Err: NULL == pvXMLInBuf || NULL == ppvXMLOutBuf || NULL == pdwXMLOutSize\n");
		return E_INVALIDARG;
	}

	
	TiXmlDocument  XmlDoc;

	*ppvXMLOutBuf = NULL;
	*pdwXMLOutSize = 0;
	
	TiXmlElement * pEleRoot = NULL;	
	TiXmlDocument * pXmlOutputDoc = NULL;
	
	hr = GenerateReplyXMLHeader(&pXmlOutputDoc);
	if (FAILED(hr) || NULL == pXmlOutputDoc)
	{
		PRINT("Err: failed to Generate Reply XML Header!!\n");

		SAFE_DELETE(pXmlOutputDoc);
		return E_FAIL;
	}	

	//PRINT("Info: dwXMLInSize = %u, pszXMLInBuf = \n%s\n", dwXMLInSize, pszXMLInBuf);

	if (0 != XmlDoc.Parse(pszXMLInBuf) 
		&& NULL != XmlDoc.RootElement() 
		&& NULL != XmlDoc.RootElement()->Value()
		&& 0 == swpa_strcmp(XmlDoc.RootElement()->Value(), HVXML_HVCMD)
		)
	{	
#ifdef PRINT_XML
		PRINT("Info: Got following XML:\n");
		XmlDoc.Print();
#endif
		pEleRoot = XmlDoc.RootElement();	
		for (TiXmlElement *Ele = pEleRoot->FirstChildElement(); Ele; Ele = Ele->NextSiblingElement())
		{
			BOOL fCmdProcessed = FALSE;
			for (INT i = 0; i < sizeof(g_XmlCmdMappingTable)/sizeof(g_XmlCmdMappingTable[0]); i++)
			{
				if (NULL != Ele->GetText()
					&& 0 == swpa_stricmp(Ele->GetText(), g_XmlCmdMappingTable[i].szCmdName))
				{	
					fCmdProcessed = TRUE;
					
					const CHAR * pszType = Ele->Attribute(HVXML_TYPE);
					if (NULL == pszType)
					{
						GenerateNoTypeErrorXml(pXmlOutputDoc, Ele->GetText());
						break;
					}

					const CHAR * pszClass = Ele->Attribute(HVXML_CLASS);
					if (NULL == pszClass)
					{
						GenerateNoClassErrorXml(pXmlOutputDoc, Ele->GetText());
						break;
					}

					const CHAR * pszValue = Ele->Attribute(HVXML_VALUE);
					if (NULL == pszValue)
					{						
						if (0 == swpa_strcmp(HVXML_SETTER, pszClass))
						{
							GenerateNoValueErrorXml(pXmlOutputDoc, Ele->GetText());
							break;
						}
					}
					

					PVOID pvRetBuf = NULL;
					/*const CHAR * pszRetLen = Ele->Attribute(HVXML_RETURNLEN);
					INT iLen = 0;
					if (NULL != pszRetLen)
					{
						iLen = swpa_atoi(pszRetLen);
					}

					if (iLen > 0)
					{
						pvRetBuf = (PBYTE)swpa_mem_alloc(iLen);
						if (NULL == pvRetBuf)
						{
							PRINT("Err: no memory for pvRetBuf (CMD=%s)\n",Ele->GetText());
							GenerateCMDProcessFailedXml(pXmlOutputDoc, Ele->GetText());
							continue;
						}
					}*/

					switch (g_XmlCmdMappingTable[i].dwCmdID)
					{
						case MSG_APP_RESTORE_FACTORY:
						case MSG_APP_SETIP:
							SW_TRACE_DEBUG("Info: got cmd:%s, need reboot\n", g_XmlCmdMappingTable[i].szCmdName);
							*pfReboot = TRUE;
							break;
						default:
							break;
					}
					
					hr = SendCMD(g_XmlCmdMappingTable[i].dwCmdID, pszValue, pszType, pszClass, &pvRetBuf);
					if (FAILED(hr))
					{
						PRINT("Err: failed to send cmd: %s\n", g_XmlCmdMappingTable[i].szCmdName);
						GenerateCMDProcessFailedXml(pXmlOutputDoc, Ele->GetText());
						SAFE_MEM_FREE(pvRetBuf);
						*pfReboot = FALSE;
						break;
					}
					CHAR * pszReplyXml = NULL;
					INT iReplyXmlLen = 0;
					if (FAILED(GenerateCMDReplyXml(pXmlOutputDoc, g_XmlCmdMappingTable[i].szCmdName, pszType, pszClass, pvRetBuf)))
					{
						SAFE_MEM_FREE(pvRetBuf);
						GenerateCMDProcessFailedXml(pXmlOutputDoc, Ele->GetText());
						break;						
					}
					SAFE_MEM_FREE(pvRetBuf);

				}
			}
			
			if (!fCmdProcessed)
			{
				GenerateCMDNotFoundXml(pXmlOutputDoc, Ele->GetText());
				continue;
			}
		}
	}
	else
	{
		GenerateCMDParseFailedXml(pXmlOutputDoc);
	}

#ifdef PRINT_XML
	PRINT("Info: response xml = \n");
	pXmlOutputDoc->Print();
#endif
	PackXml(pXmlOutputDoc, ppvXMLOutBuf,  pdwXMLOutSize);

	SAFE_DELETE(pXmlOutputDoc);

	return S_OK;
}



HRESULT CSWNetCommandProcess::OnNetCMDConnect(VOID)
{
	HRESULT hr = S_OK;
	
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	const DWORD dwTimeOutMS = 4000;
	CSWTCPSocket * pSockServer = new CSWTCPSocket();
	
	if (NULL == pSockServer)
	{
		PRINT("Err: no memory for pSockServer\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}

	if (FAILED(pSockServer->Create()))
	{
		PRINT("Err: failed to create cmd server socket\n");
		hr = E_FAIL;
		goto OUT;
	}
	
	while (FAILED(pSockServer->Bind(NULL, m_wCMDProcessPort)))
	{
		PRINT("Err: NetCommandFilter failed to bind to port #%d. Trying again...\n", m_wCMDProcessPort);
		CSWApplication::Sleep(500);
		//hr = E_FAIL;
		//goto OUT;
	}
	
	pSockServer->SetSendTimeout( dwTimeOutMS );
	pSockServer->SetRecvTimeout( dwTimeOutMS );
	pSockServer->Listen();
	
	while (STATE_RUNNING == m_dwState)
	{	
		// 限制最大连接数　todo.
		INT iThreadCount = m_lstCMDProcessThread.GetCount();
		if( iThreadCount > 2 )
		{
			SW_TRACE_DEBUG("<NetCommandProcess>cmd thread count:%d!\n", iThreadCount);
		}
		SW_POSITION Pos = m_lstCMDProcessThread.GetHeadPosition();		
		while (m_lstCMDProcessThread.IsValid(Pos))
		{
			SW_POSITION PrevPos = Pos;
			CSWThread * pThread = m_lstCMDProcessThread.GetNext(Pos);
			if (NULL != pThread)// && !pThread->IsValid())
			{
				m_lstCMDProcessThread.RemoveAt(PrevPos);
				SAFE_RELEASE(pThread);
			}
		}
		
		SWPA_SOCKET_T sInSock;
		if (!FAILED(pSockServer->Accept(sInSock)))
		{
			//PRINT("Info: got a connection\n");
			CSWThread * pThread = new CSWThread();
			if (NULL == pThread)
			{
				PRINT("Err: no memory for pThread\n");
				hr = E_OUTOFMEMORY;
				goto OUT;
			}

			_NETCMDMPROCESSARG* psArg = (_NETCMDMPROCESSARG*)swpa_mem_alloc(sizeof(_NETCMDMPROCESSARG));
			if (NULL == psArg)
			{
				PRINT("Err: no memory for psArg\n");

				SAFE_RELEASE(pThread);
				
				hr = E_OUTOFMEMORY;
				goto OUT;
			}
			psArg->pThis = this;
			psArg->sSock = sInSock;

			m_lstCMDProcessThread.AddTail(pThread);
			pThread->Start(OnNetCMDProcessProxy, (PVOID)psArg);
			
		}		
	}


OUT:

	SAFE_RELEASE(pSockServer);

	return hr;
	
}




VOID* CSWNetCommandProcess::OnNetCMDConnectProxy(VOID* pvParam)
{
	if (NULL == pvParam)
	{
		return (VOID*)E_INVALIDARG;
	}

	CSWNetCommandProcess * pThis = (CSWNetCommandProcess *)pvParam;

	return (VOID*)pThis->OnNetCMDConnect();
}


HRESULT CSWNetCommandProcess::ProcessProtocolCMD(PVOID* ppvOutBuf,  DWORD *pdwOutSize)
{
	if (NULL == ppvOutBuf || NULL == pdwOutSize)
	{
		PRINT("Err: NULL == ppvOutBuf || NULL == pdwOutSize\n");
		return E_INVALIDARG;
	}

	CAMERA_CMD_RESPOND sRespond;

	sRespond.dwID = CAMERA_PROTOCOL_COMMAND;
	sRespond.dwInfoSize = 0;
	sRespond.iResult = PROTOCOL_MERCURY;

	
	*pdwOutSize = sizeof(sRespond);
	*ppvOutBuf = swpa_mem_alloc(*pdwOutSize);
	if (NULL == *ppvOutBuf)
	{
		PRINT("Err: no memory for *ppvOutBuf\n");
		return E_INVALIDARG;
	}
	
	swpa_memcpy(*ppvOutBuf, &sRespond, *pdwOutSize);
	
	return S_OK;
	
}



HRESULT CSWNetCommandProcess::ProcessSetParamCMD(const VOID* pvInputBuf,  PVOID* ppvOutBuf,  DWORD *pdwOutSize)
{
	if (NULL == ppvOutBuf || NULL == pdwOutSize)
	{
		PRINT("Err: NULL == dwInputSize || NULL == ppvOutBuf || NULL == pdwOutSize\n");
		return E_INVALIDARG;
	}

	CAMERA_CMD_RESPOND sRespond;

	sRespond.dwID = CAMERA_SET_PARAM_CMD;
	sRespond.dwInfoSize = 0;
	if (NULL == pvInputBuf)
	{
		sRespond.iResult = -1;
		//PRINT("%s L%d:Err: NULL == pvInputBuf sRespond.iResult = -1\n", __FUNCTION__, __LINE__);
	}
	else if (FAILED(CSWMessage::SendMessage(MSG_APP_SETXML, (WPARAM)pvInputBuf, (LPARAM)0)))
	{
		sRespond.iResult = -1;
		//PRINT("%s L%d:Err: SendMessage(MSG_APP_SETXML) FAILED! sRespond.iResult = -1\n", __FUNCTION__, __LINE__);
	}
	else
	{
		sRespond.iResult = 0;
	}

	*pdwOutSize = sizeof(sRespond);
	*ppvOutBuf = swpa_mem_alloc(*pdwOutSize);
	if (NULL == *ppvOutBuf)
	{
		PRINT("Err: no memory for *ppvOutBuf\n");
		return E_OUTOFMEMORY;
	}
	swpa_memcpy(*ppvOutBuf, &sRespond, *pdwOutSize);
	
	return S_OK;
	
}



HRESULT CSWNetCommandProcess::ProcessGetParamCMD(PVOID* ppvOutBuf,  DWORD *pdwOutSize)
{
	if (NULL == ppvOutBuf || NULL == pdwOutSize)
	{
		PRINT("Err: NULL == ppvOutBuf || NULL == pdwOutSize\n");
		return E_INVALIDARG;
	}

	CAMERA_CMD_RESPOND sRespond;
	PBYTE pbBuf = NULL;
	CSWString strParamXml;

	sRespond.dwID = CAMERA_GET_PARAM_CMD;
	sRespond.iResult = -1;
	sRespond.dwInfoSize = 0;

	if (SUCCEEDED(CSWMessage::SendMessage(MSG_APP_GETXML, (WPARAM)0, (LPARAM)(&strParamXml))))
	{
		sRespond.iResult = 0;
		sRespond.dwInfoSize = strParamXml.Length() + 1;

		pbBuf = (PBYTE)swpa_mem_alloc(sizeof(sRespond) + sRespond.dwInfoSize);
		if (NULL == pbBuf)
		{
			SW_TRACE_DEBUG("Err: no memory for pbBuf\n");
			//return E_OUTOFMEMORY;
		}
		else
		{
			swpa_memset(pbBuf, 0, sizeof(sRespond) + sRespond.dwInfoSize);
			swpa_memcpy(pbBuf, &sRespond, sizeof(sRespond));
			swpa_memcpy(pbBuf+sizeof(sRespond), (const CHAR*)strParamXml, strParamXml.Length());

			*pdwOutSize = sizeof(sRespond) + sRespond.dwInfoSize;
			*ppvOutBuf = pbBuf;

			return S_OK;
		}		
	}
	else
	{
		SW_TRACE_NORMAL("Err: failed to get xml parameter.\n");
	}

	*pdwOutSize = sizeof(sRespond);
	
	SAFE_MEM_FREE(* ppvOutBuf);
	*ppvOutBuf = swpa_mem_alloc(*pdwOutSize);
	if (NULL == *ppvOutBuf)
	{
		SW_TRACE_DEBUG("Err: no memory for *ppvOutBuf\n");
		return E_OUTOFMEMORY;
	}

	swpa_memcpy(*ppvOutBuf, &sRespond, *pdwOutSize);

	return E_FAIL;
	
}


HRESULT CSWNetCommandProcess::ProcessSetDomeParamCMD(const VOID* pvInputBuf,  PVOID* ppvOutBuf,  DWORD *pdwOutSize)
{
	if (NULL == ppvOutBuf || NULL == pdwOutSize)
	{
		PRINT("Err: NULL == dwInputSize || NULL == ppvOutBuf || NULL == pdwOutSize\n");
		return E_INVALIDARG;
	}

	CAMERA_CMD_RESPOND sRespond;

	sRespond.dwID = CAMERA_SET_DOME_PARAM_CMD;
	sRespond.dwInfoSize = 0;
	if (NULL == pvInputBuf)
	{
		sRespond.iResult = -1;
		//PRINT("%s L%d:Err: NULL == pvInputBuf sRespond.iResult = -1\n", __FUNCTION__, __LINE__);
	}
	else if (FAILED(CSWMessage::SendMessage(MSG_SET_DOME_XML, (WPARAM)pvInputBuf, (LPARAM)0)))
	{
		sRespond.iResult = -1;
		//PRINT("%s L%d:Err: SendMessage(MSG_APP_SETXML) FAILED! sRespond.iResult = -1\n", __FUNCTION__, __LINE__);
	}
	else
	{
		sRespond.iResult = 0;
	}

	*pdwOutSize = sizeof(sRespond);
	*ppvOutBuf = swpa_mem_alloc(*pdwOutSize);
	if (NULL == *ppvOutBuf)
	{
		PRINT("Err: no memory for *ppvOutBuf\n");
		return E_OUTOFMEMORY;
	}
	swpa_memcpy(*ppvOutBuf, &sRespond, *pdwOutSize);
	
	return S_OK;
	
}



HRESULT CSWNetCommandProcess::ProcessGetDomeParamCMD(PVOID* ppvOutBuf,  DWORD *pdwOutSize)
{
	if (NULL == ppvOutBuf || NULL == pdwOutSize)
	{
		PRINT("Err: NULL == ppvOutBuf || NULL == pdwOutSize\n");
		return E_INVALIDARG;
	}

	CAMERA_CMD_RESPOND sRespond;
	PBYTE pbBuf = NULL;
	CSWString strParamXml;

	sRespond.dwID = CAMERA_GET_DOME_PARAM_CMD;
	sRespond.iResult = -1;
	sRespond.dwInfoSize = 0;

	if (!FAILED(CSWMessage::SendMessage(MSG_GET_DOME_XML, (WPARAM)0, (LPARAM)(&strParamXml))))
	{
		sRespond.iResult = 0;
		sRespond.dwInfoSize = strParamXml.Length() + 1;

		pbBuf = (PBYTE)swpa_mem_alloc(sizeof(sRespond) + sRespond.dwInfoSize);
		if (NULL == pbBuf)
		{
			PRINT("Err: no memory for pbBuf\n");
			//return E_OUTOFMEMORY;
		}
		else
		{
			swpa_memset(pbBuf, 0, sizeof(sRespond) + sRespond.dwInfoSize);
			swpa_memcpy(pbBuf, &sRespond, sizeof(sRespond));
			swpa_strncpy((CHAR*)(pbBuf+sizeof(sRespond)), (const CHAR*)strParamXml, sRespond.dwInfoSize);

			*pdwOutSize = sizeof(sRespond) + sRespond.dwInfoSize;
			*ppvOutBuf = pbBuf;

			return S_OK;
		}		
	}


	*pdwOutSize = sizeof(sRespond);

	SAFE_MEM_FREE(* ppvOutBuf);
	*ppvOutBuf = swpa_mem_alloc(*pdwOutSize);
	if (NULL == *ppvOutBuf)
	{
		PRINT("Err: no memory for *ppvOutBuf\n");
		return E_OUTOFMEMORY;
	}

	swpa_memcpy(*ppvOutBuf, &sRespond, *pdwOutSize);

	return E_FAIL;
	
}


HRESULT CSWNetCommandProcess::ProcessGetHDD_ReportCMD(PVOID* ppvOutBuf,  DWORD *pdwOutSize)
{
    if (NULL == ppvOutBuf || NULL == pdwOutSize)
    {
        PRINT("Err: NULL == ppvOutBuf || NULL == pdwOutSize\n");
        return E_INVALIDARG;
    }

    CHAR *pbBuf = (CHAR *)swpa_mem_alloc(sizeof(CHAR) *1024);
    swpa_memset(pbBuf,0,1024);
    CHAR strReport[512];
    swpa_memset(strReport,0,512);
    CSWMessage::SendMessage(MSG_APP_HDD_CHECKREPORT, (WPARAM)0, (LPARAM)(&strReport));
    PRINT("ProcessGetHDD_ReportCMD = %s\n",(LPCSTR)strReport);
    CAMERA_CMD_RESPOND sRespond;
    sRespond.dwID = CAMERA_GET_HDD_REPORT_LOG;
    sRespond.iResult = 0;
    sRespond.dwInfoSize = swpa_strlen(strReport) +1;
    swpa_memcpy(pbBuf,&sRespond,sizeof(sRespond));
    swpa_strcpy(pbBuf+sizeof(sRespond),(LPCSTR)strReport);
    *ppvOutBuf = pbBuf;
    *pdwOutSize = swpa_strlen(strReport) + sizeof(sRespond) + 1;
    return S_OK;
}


HRESULT CSWNetCommandProcess::GetConnectedIPInfo(CHAR * RecordLinkIP,CHAR * ImageLinkIP,CHAR *VideoLinkIP)
{
    if(RecordLinkIP == NULL || ImageLinkIP == NULL ||VideoLinkIP == NULL)
        return E_INVALIDARG;
    CHAR szInfo[1024];
    CSWMessage::SendMessage(MSG_APP_GETCONNECTED_IP, (WPARAM)szInfo);
    CSWString strRet;
    PRINT("%s %d\n",__FUNCTION__,__LINE__);
    PRINT("%s\n\n",szInfo);
    swpa_strncpy(RecordLinkIP,"(0,0,0,0)",256);
    swpa_strncpy(ImageLinkIP,"(0,0,0,0)",256);
    swpa_strncpy(VideoLinkIP,"(0,0,0,0)",256);

    if( swpa_strlen(szInfo) > 0 )
    {
        CSWString strInfo = szInfo;
        INT iCount = 0;

        INT iPrePos = strInfo.Find("结果链接:");
        INT iEndPos = -1;
        if( iPrePos != -1 )
        {
            INT iBeginPos = iPrePos;
            iEndPos = strInfo.Find("\n", iBeginPos);
            if( iEndPos != -1 )
            {
                INT iStrPos = strInfo.Find("_", iPrePos);
                if( iStrPos != -1 )
                {
                    INT iEndStrPos = strInfo.Find("_", iStrPos + 1);
                    while(iEndStrPos != -1 && iEndStrPos < iEndPos)
                    {
                        CSWString strIp;
                        strIp.Format("(%s),", (const CHAR*)(strInfo.Substr(iStrPos + 1, iEndStrPos - iStrPos - 1)) );
                        ++iCount;
                        iStrPos = iEndStrPos;
                        iEndStrPos = strInfo.Find("_", iStrPos + 1);
                        strRet += strIp;
                    }
                    swpa_strcpy(RecordLinkIP,(LPCSTR)strRet);
                }
            }
        }

        strRet.Clear();
        iPrePos = strInfo.Find("JPEG链接:");
        iEndPos = -1;
        if( iPrePos != -1 )
        {
            INT iBeginPos = iPrePos;
            iEndPos = strInfo.Find("\n", iBeginPos);
            if( iEndPos != -1 )
            {
                INT iStrPos = strInfo.Find("_", iPrePos);
                if( iStrPos != -1 )
                {
                    INT iEndStrPos = strInfo.Find("_", iStrPos + 1);
                    while(iEndStrPos != -1 && iEndStrPos < iEndPos)
                    {
                        CSWString strIp;
                        strIp.Format("(%s),", (const CHAR*)(strInfo.Substr(iStrPos + 1, iEndStrPos - iStrPos - 1)) );
                        ++iCount;
                        iStrPos = iEndStrPos;
                        iEndStrPos = strInfo.Find("_", iStrPos + 1);
                        strRet += strIp;
                    }
                    swpa_strcpy(ImageLinkIP,(LPCSTR)strRet);
                }
            }
        }

        strRet.Clear();
        iPrePos = strInfo.Find("H264链接:");
        iEndPos = -1;
        if( iPrePos != -1 )
        {
            INT iBeginPos = iPrePos;
            iEndPos = strInfo.Find("\n", iBeginPos);
            if( iEndPos != -1 )
            {
                INT iStrPos = strInfo.Find("_", iPrePos);
                if( iStrPos != -1 )
                {
                    INT iEndStrPos = strInfo.Find("_", iStrPos + 1);
                    while(iEndStrPos != -1 && iEndStrPos < iEndPos)
                    {
                        CSWString strIp;
                        strIp.Format("(%s),", (const CHAR*)(strInfo.Substr(iStrPos + 1, iEndStrPos - iStrPos - 1)) );
                        ++iCount;
                        iStrPos = iEndStrPos;
                        iEndStrPos = strInfo.Find("_", iStrPos + 1);
                        strRet += strIp;
                    }
                    swpa_strcpy(VideoLinkIP,(LPCSTR)strRet);
                }
            }
        }

        strRet.Clear();
        iPrePos = strInfo.Find("RTSP链接:");
        iEndPos = -1;
        if( iPrePos != -1 )
        {
            INT iBeginPos = iPrePos;
            iEndPos = strInfo.Find("\n", iBeginPos);
            if( iEndPos != -1 )
            {
                INT iStrPos = strInfo.Find("_", iPrePos);
                if( iStrPos != -1 )
                {
                    INT iEndStrPos = strInfo.Find("_", iStrPos + 1);
                    while(iEndStrPos != -1 && iEndStrPos < iEndPos)
                    {
                        CSWString strIp;
                        strIp.Format("(%s),", (const CHAR*)(strInfo.Substr(iStrPos + 1, iEndStrPos - iStrPos - 1)) );
                        ++iCount;
                        iStrPos = iEndStrPos;
                        iEndStrPos = strInfo.Find("_", iStrPos + 1);
                        strRet += strIp;
                    }
                    swpa_strcat(VideoLinkIP,(LPCSTR)strRet);
                }
            }
        }

        strRet.Clear();
        iPrePos = strInfo.Find("RTSP第二路链接:");
        iEndPos = -1;
        if( iPrePos != -1 )
        {
            INT iBeginPos = iPrePos;
            iEndPos = strInfo.Find("\n", iBeginPos);
            if( iEndPos != -1 )
            {
                INT iStrPos = strInfo.Find("_", iPrePos);
                if( iStrPos != -1 )
                {
                    INT iEndStrPos = strInfo.Find("_", iStrPos + 1);
                    while(iEndStrPos != -1 && iEndStrPos < iEndPos)
                    {
                        CSWString strIp;
                        strIp.Format("(%s),", (const CHAR*)(strInfo.Substr(iStrPos + 1, iEndStrPos - iStrPos - 1)) );
                        ++iCount;
                        iStrPos = iEndStrPos;
                        iEndStrPos = strInfo.Find("_", iStrPos + 1);
                        strRet += strIp;
                    }
                    swpa_strcat(VideoLinkIP,(LPCSTR)strRet);
                }
            }
        }
    }

    return   S_OK;

}

HRESULT CSWNetCommandProcess::OnNetCMDProcess(SWPA_SOCKET_T sInSock)
{
	if (0 == sInSock)
	{
		return E_INVALIDARG;
	}


	HRESULT hr = S_OK;
	CSWTCPSocket DataSock;
	DataSock.Attach(sInSock);

	DataSock.SetRecvTimeout(4000);
	DataSock.SetSendTimeout(4000);
	
	while (STATE_RUNNING == m_dwState)
	{			
		DWORD dwRcvDataLen;
		CAMERA_CMD_HEADER sCmdHeader;
		swpa_memset(&sCmdHeader, 0, sizeof(sCmdHeader));
		
		hr = DataSock.Read((PVOID)&sCmdHeader, sizeof(CAMERA_CMD_HEADER), &dwRcvDataLen);
		if (FAILED(hr) || 0 >= dwRcvDataLen)
		{
			PRINT("Err: failed to read socket\n");
			hr = E_FAIL;
			break;//continue;
		}
		

		PVOID pvReplyData = NULL;
		DWORD dwReplyDataSize = 0;
		BOOL fNeedReboot = FALSE;
		if (CAMERA_XML_EXT_CMD == sCmdHeader.dwID)
		{
#ifdef PRINT_XML
			PRINT("Info: Got xml cmds...\n");
#endif
			CHAR* pszInputBuf = NULL;	
			
			pszInputBuf = (CHAR*)swpa_mem_alloc(sCmdHeader.dwInfoSize);
			if (NULL == pszInputBuf)
			{
				PRINT("Err: no memory for pszInputBuf\n");
				hr = E_OUTOFMEMORY;
				break;
			}

			hr = DataSock.Read(pszInputBuf, sCmdHeader.dwInfoSize, &dwRcvDataLen);
			if (FAILED(hr) || 0 >= dwRcvDataLen)
			{
				PRINT("Err: failed to read socket\n");
				SAFE_MEM_FREE(pszInputBuf);
				break;
			}
			
			hr = ProcessXmlCmd(pszInputBuf, sCmdHeader.dwInfoSize, &pvReplyData, &dwReplyDataSize, &fNeedReboot);				
			if (FAILED(hr) || 0 == dwReplyDataSize)
			{	
				PRINT("Err: failed to process xml cmds\n");
				SAFE_MEM_FREE(pszInputBuf);
				SAFE_MEM_FREE(pvReplyData);
				
				break;
			}
			SAFE_MEM_FREE(pszInputBuf);
		}
		else if (CAMERA_PROTOCOL_COMMAND == sCmdHeader.dwID)
		{
			PRINT("Info: Got protocol cmd...\n");
			hr = ProcessProtocolCMD(&pvReplyData, &dwReplyDataSize);
			if (FAILED(hr) || 0 == dwReplyDataSize)
			{	
				PRINT("Err: failed to process protocol cmd\n");

				SAFE_MEM_FREE(pvReplyData);				
				break;
			}	
			PRINT("Info: Got protocol cmd return 0x%08x...\n", hr);
		}

		else if (CAMERA_SET_PARAM_CMD == sCmdHeader.dwID)
		{
			PRINT("Info: Got CAMERA_SET_PARAM_CMD cmd...\n");
			//PRINT("Info: Got paramxml size = %d...\n", sCmdHeader.dwInfoSize);
			PVOID pvBuf = swpa_mem_alloc(sCmdHeader.dwInfoSize + 1);
			if (NULL == pvBuf)
			{
				PRINT("Err: no memory for pvBuf\n");
				break;
			}
			else
			{
				DataSock.SetRecvTimeout(0);
				swpa_memset(pvBuf, 0, sCmdHeader.dwInfoSize + 1);
				hr = DataSock.Read(pvBuf, sCmdHeader.dwInfoSize, &dwRcvDataLen);
				DataSock.SetRecvTimeout(4000);
				if (FAILED(hr) || 0 >= dwRcvDataLen)
				{
					PRINT("Err: failed to read socket\n");
					SAFE_MEM_FREE(pvBuf);
					hr = E_FAIL;
					break;//continue;
				}
			}

			
			if (FAILED(ProcessSetParamCMD(pvBuf, &pvReplyData, &dwReplyDataSize)))//if (FAILED(ProcessSetDomeParamCMD(pvBuf, &pvReplyData, &dwReplyDataSize)))
			{
				PRINT("Err: failed to process CAMERA_SET_PARAM_CMD cmd\n");

				SAFE_MEM_FREE(pvBuf);
				SAFE_MEM_FREE(pvReplyData);	
				break;
			}
			SAFE_MEM_FREE(pvBuf);
		}
		else if (CAMERA_GET_PARAM_CMD == sCmdHeader.dwID)
		{
			PRINT("Info: Got CAMERA_GET_PARAM_CMD cmd...\n");
			if (FAILED(ProcessGetParamCMD(&pvReplyData, &dwReplyDataSize)))//if (FAILED(ProcessGetDomeParamCMD(&pvReplyData, &dwReplyDataSize)))//
			{
				PRINT("Err: failed to process CAMERA_GET_PARAM_CMD cmd\n");

				SAFE_MEM_FREE(pvReplyData);	
				break;
			}
		}
		else if (CAMERA_SET_DOME_PARAM_CMD == sCmdHeader.dwID)
		{
			PRINT("Info: Got CAMERA_SET_DOME_PARAM_CMD cmd...\n");
			//PRINT("Info: Got paramxml size = %d...\n", sCmdHeader.dwInfoSize);
			PVOID pvBuf = swpa_mem_alloc(sCmdHeader.dwInfoSize + 1);
			if (NULL == pvBuf)
			{
				PRINT("Err: no memory for pvBuf\n");
				break;
			}
			else
			{
				DataSock.SetRecvTimeout(0);
				swpa_memset(pvBuf, 0, sCmdHeader.dwInfoSize + 1);
				hr = DataSock.Read(pvBuf, sCmdHeader.dwInfoSize, &dwRcvDataLen);
				DataSock.SetRecvTimeout(4000);
				if (FAILED(hr) || 0 >= dwRcvDataLen)
				{
					PRINT("Err: failed to read socket\n");
					SAFE_MEM_FREE(pvBuf);
					hr = E_FAIL;
					break;//continue;
				}
			}

			
			if (FAILED(ProcessSetDomeParamCMD(pvBuf, &pvReplyData, &dwReplyDataSize)))
			{
				PRINT("Err: failed to process CAMERA_SET_PARAM_CMD cmd\n");

				SAFE_MEM_FREE(pvBuf);
				SAFE_MEM_FREE(pvReplyData);	
				break;
			}
			SAFE_MEM_FREE(pvBuf);
		}
		else if (CAMERA_GET_DOME_PARAM_CMD == sCmdHeader.dwID)
		{
			PRINT("Info: Got CAMERA_GET_DOME_PARAM_CMD cmd...\n");
			if (FAILED(ProcessGetDomeParamCMD(&pvReplyData, &dwReplyDataSize)))
			{
				PRINT("Err: failed to process CAMERA_GET_PARAM_CMD cmd\n");

				SAFE_MEM_FREE(pvReplyData);	
				break;
			}
		}
        else if (CAMERA_GET_HDD_REPORT_LOG == sCmdHeader.dwID)
        {
            PRINT("Info: Got CAMERA_GET_HDD_REPORT_LOG cmd...\n");
            if (FAILED(ProcessGetHDD_ReportCMD(&pvReplyData, &dwReplyDataSize)))
            {
                PRINT("Err: failed to process CAMERA_GET_HDD_REPORT_LOG cmd\n");

                SAFE_MEM_FREE(pvReplyData);
                break;
            }

        }
		else if (CAMERA_SET_TRANSPARENT_SERIAL_CMD == sCmdHeader.dwID)
		{
			SW_TRACE_DEBUG("Info: Got CAMERA_SET_TRANSPARENT_SERIAL_CMD cmd...\n");
			
			PVOID pvBuf = swpa_mem_alloc(sCmdHeader.dwInfoSize + 1);
			if (NULL == pvBuf)
			{
				SW_TRACE_DEBUG("Err: no memory for pvBuf\n");
				break;
			}
			else
			{
				DataSock.SetRecvTimeout(0);
				swpa_memset(pvBuf, 0, sCmdHeader.dwInfoSize + 1);
				hr = DataSock.Read(pvBuf, sCmdHeader.dwInfoSize, &dwRcvDataLen);
				DataSock.SetRecvTimeout(4000);
				if (FAILED(hr) || 0 >= dwRcvDataLen)
				{
					SW_TRACE_DEBUG("Err: failed to read socket\n");
					SAFE_MEM_FREE(pvBuf);
					hr = E_FAIL;
					break;
				}

				CAMERA_CMD_RESPOND sRespond;

				sRespond.dwID = CAMERA_SET_TRANSPARENT_SERIAL_CMD;
				sRespond.dwInfoSize = 0;
				sRespond.iResult = 0;

				SW_TRACE_DEBUG("tobedeleted: cmd = %s\n",(LPCSTR) pvBuf);
				
				BYTE bCmd[256] = {0xff};
				INT iCmdLen = CSWUtils::HexStringToBytes((LPCSTR)pvBuf, bCmd, dwRcvDataLen);
				if (0 >= iCmdLen)
				{
					SW_TRACE_DEBUG("Err: failed to convert HexString to Bytes.\n");
					sRespond.iResult = -1;
				}
				SAFE_MEM_FREE(pvBuf);
				
				switch (bCmd[0])
				{	
					case 0x0: //dome camera
					{
						if (FAILED(CSWMessage::SendMessage(MSG_CAMERA_DO_CMD, (WPARAM)&bCmd[1], (LPARAM)(iCmdLen-1))))
						{
							SW_TRACE_DEBUG("Err: failed to send camera cmd.\n");
							sRespond.iResult = -1;
						}
					}
					break;

					default:
						SW_TRACE_DEBUG("Err: invalid type 0x%02x.\n", bCmd[0]);
						sRespond.iResult = -1;
					break;
				}

				dwReplyDataSize = sizeof(sRespond);
				pvReplyData = swpa_mem_alloc(dwReplyDataSize);
				if (NULL == pvReplyData)
				{
					SW_TRACE_DEBUG("Err: no memory for pvReplyData\n");
					return E_OUTOFMEMORY;
				}
				swpa_memcpy(pvReplyData, &sRespond, dwReplyDataSize);
			}

		}

//		SW_TRACE_DEBUG("zydebug: Info: dwReplyDataSize = %u, pvReplyData = \n%s\n", dwReplyDataSize, (CHAR*)pvReplyData);
		DWORD dwSentLen = 0;
		hr = DataSock.Send(pvReplyData, dwReplyDataSize, &dwSentLen);
		if (FAILED(hr))
		{	
			PRINT("Err: failed to send response data\n");
			
			SAFE_MEM_FREE(pvReplyData);
			break;
		}
		//PRINT("Info: dwSentLen=%d\n", dwSentLen);

		SAFE_MEM_FREE(pvReplyData);

		if (fNeedReboot)
		{
			CSWMessage::SendMessage(MSG_APP_RESETDEVICE, (WPARAM)2, 0);
			break;
		}

		DWORD dwTemp = 0;
		if (FAILED(DataSock.Read(&dwTemp, sizeof(dwTemp)))) //this read is used to keep the link
		{
			break;
		}

		
	}

	DataSock.Close();

	//PRINT("Info: exiting...\n");

	return S_OK;
}





VOID* CSWNetCommandProcess::OnNetCMDProcessProxy(VOID* pvParam)
{
	if (NULL == pvParam)
	{
		return (VOID*)E_INVALIDARG;
	}

	_NETCMDMPROCESSARG * pArg = (_NETCMDMPROCESSARG *)pvParam;

	HRESULT hr = pArg->pThis->OnNetCMDProcess(pArg->sSock);	

	SAFE_MEM_FREE(pArg);

	return (VOID*)hr;
}




HRESULT CSWNetCommandProcess::GenerateProbeReplyXml(CHAR ** ppszXmlBuf, DWORD *pdwXmlLen)
{
	const DWORD _INFO_LEN = 256;
	const DWORD _MSG_LEN = _INFO_LEN * 2;
	HRESULT hr = S_OK;
	INT	iRet = SWPAR_OK;
	//CHAR szMsg[_MSG_LEN] = {0};
	CHAR szSN[_INFO_LEN] = "UnitTest";
	CHAR szMode[_INFO_LEN] = "TestMode";//{0};
	DWORD dwLen = _INFO_LEN;

	CHAR * pszXmlData = NULL;
	TiXmlDocument * pXmlOutputDoc = NULL;
	TiXmlDeclaration *pDeclaration = NULL;
	TiXmlElement *pRootEle = NULL;
	TiXmlElement *pReplyEle = NULL;	
    TiXmlPrinter * pXmlPrinter = NULL; 
	TiXmlText *pReplyText = NULL;
	
	CHAR szIP[32] = {0};
	CHAR szNetMask[32] = {0};
	CHAR szMAC[32] = {0};
	CHAR szGateway[32] = {0};


	if (NULL == ppszXmlBuf || NULL == pdwXmlLen)
	{
		PRINT("Err: NULL == ppszXmlBuf || NULL == pdwXmlLen\n");
		return E_INVALIDARG;
	}

	//CAMERA_CMD_RESPOND sCmdHeader;
	//swpa_memset(&sCmdHeader, 0, sizeof(sCmdHeader));
	
	
	DWORD dwNetID = 0;
	CHAR szEthernet[8] = {0};
	do {		
		swpa_sprintf(szEthernet, "eth%d", dwNetID);
		iRet = swpa_tcpip_getinfo(szEthernet, szIP, 32, szNetMask, 32, szGateway, 32, szMAC, 32);
		dwNetID++;
	} while ( SWPAR_OK != iRet && 5 > dwNetID );

	if (5 <= dwNetID)
	{
		PRINT("Err: failed to get net info! (eth0~eth%d)\n", dwNetID);
		return E_FAIL;
	}


	iRet = swpa_device_read_sn(szSN, &dwLen);
	if (SWPAR_OK != iRet)
	{
		PRINT("Err: failed to get device serial number\n");
		return E_FAIL;
	}
	
	INT iMode = 0;
	iRet = swpa_device_get_resetmode(&iMode);
	if (SWPAR_OK != iRet)
	{
		PRINT("Err: failed to get device working mode\n");
		return E_FAIL;
	}
	
	//swpa_snprintf(szMsg, _MSG_LEN - 1, "IP:%s;MAC:%s;SN:%s;MODE:%s;", szIP, szMAC, szSN, szMode);


	pXmlOutputDoc = new TiXmlDocument();
	if (NULL == pXmlOutputDoc)
	{
		PRINT("Err: no memory for pXmlOutputDoc\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}	

	pDeclaration = new TiXmlDeclaration("1.0","GB2312","yes");
	if (NULL == pDeclaration)
	{
		PRINT("Err: no memory for pDeclaration\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}
	pXmlOutputDoc->LinkEndChild(pDeclaration);

	pRootEle = new TiXmlElement(HVXML_HVCMDRESPOND);
	if (NULL == pRootEle)
	{
		PRINT("Err: no memory for pRootEle\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}
	pRootEle->SetAttribute(HVXML_VER, HV_XML_CMD_VERSION_NO);
	

	pReplyEle = new TiXmlElement(HVXML_CMDNAME);
	if (NULL == pReplyEle)
	{
		PRINT("Err: no memory for pReplyEle\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}
    CHAR szVersion[255];
    CHAR szDevType[255];
    CHAR szWorkMode[255];
	CHAR szDevName[255];
    CSWMessage::SendMessage(MSG_APP_GETVERSION, 0, (LPARAM)szVersion);
    CSWMessage::SendMessage(MSG_APP_GETDEVTYPE, 0, (LPARAM)szDevType);
    CSWMessage::SendMessage(MSG_APP_GETWORKMODE, 0, (LPARAM)szWorkMode);
	CSWMessage::SendMessage(MSG_APP_GET_CUSTOMIZED_DEVNAME, 0, (LPARAM)szDevName);
	pReplyEle->SetAttribute("IP", szIP);
	pReplyEle->SetAttribute("Mask", szNetMask);
	pReplyEle->SetAttribute("Gateway", szGateway);
	pReplyEle->SetAttribute("MAC", szMAC);
	pReplyEle->SetAttribute("SN", szSN);
	pReplyEle->SetAttribute("Mode", 0 == iMode ? "正常模式" : "升级模式");
    pReplyEle->SetAttribute("WorkMode", szWorkMode);
    pReplyEle->SetAttribute("DevVersion", szVersion);
    pReplyEle->SetAttribute("DevType", szDevType);
    pReplyEle->SetAttribute("Remark",szDevName);
	pReplyEle->SetAttribute(HVXML_RETCODE, 0);
	pReplyEle->SetAttribute(HVXML_RETMSG, "OK");

	pReplyText = new TiXmlText("Probe");  
	if (NULL == pReplyText)
	{
		PRINT("Err: no memory for pReplyText\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}
	pReplyEle->LinkEndChild(pReplyText);
	
	pRootEle->LinkEndChild(pReplyEle);
	
	pXmlOutputDoc->LinkEndChild(pRootEle);

	pXmlPrinter = new TiXmlPrinter();
	if (NULL == pXmlPrinter)
	{
		PRINT("Err: no memory for pXmlPrinter\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}
	
    pXmlOutputDoc->Accept(pXmlPrinter);
	
	pszXmlData = (CHAR*)swpa_mem_alloc(pXmlPrinter->Size() /* + sizeof(sCmdHeader)*/);
	if (NULL == pszXmlData)
	{
		PRINT("Err: no memory for pszXmlData\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}

	//sCmdHeader.dwID = CAMERA_XML_EXT_CMD;
	//sCmdHeader.dwInfoSize = pXmlPrinter->Size();
	//sCmdHeader.iResult = 0;
	
    swpa_memset(pszXmlData, 0x0, pXmlPrinter->Size() /*+ sizeof(sCmdHeader)*/);
	
	//swpa_memcpy(pszXmlData, &sCmdHeader, sizeof(sCmdHeader));
    swpa_memcpy(pszXmlData/*+sizeof(sCmdHeader)*/, pXmlPrinter->CStr(), pXmlPrinter->Size());

	*ppszXmlBuf = pszXmlData;
	*pdwXmlLen = pXmlPrinter->Size() /*+sizeof(sCmdHeader)*/;
		 
OUT:

	SAFE_DELETE(pXmlPrinter);
	SAFE_DELETE(pXmlOutputDoc);

	return hr;
}


HRESULT CSWNetCommandProcess::ParseProbeXmlMsg(CHAR * pszMsg)
{
	if (NULL == pszMsg)
	{
		PRINT("Err: NULL == pszMsg\n");
		return E_INVALIDARG;
	}
	
	TiXmlDocument  XmlDoc;

	XmlDoc.Parse(pszMsg);

	TiXmlElement * pEleRoot = XmlDoc.RootElement();
	if(NULL != pEleRoot)
	{
		for (TiXmlElement *Ele = pEleRoot->FirstChildElement(); NULL != Ele; Ele = Ele->NextSiblingElement())
		{			
			const CHAR * szText = Ele->GetText();
			//PRINT("Info: Ele->GetText() = %s\n", szText);
			if (NULL != szText)
			{
				if (0 == swpa_strcmp("Probe", szText)
					|| 0 == swpa_strcmp("probe", szText)
					|| 0 == swpa_strcmp("PROBE", szText))
				{
					return S_OK;
				}
			}
		}
	}

	return E_FAIL;
}




HRESULT CSWNetCommandProcess::GenerateGetIPCMDReplay(BYTE** pbReplyPack, DWORD* pdwPackLen)
{
	if (NULL == pbReplyPack || NULL == pdwPackLen)
	{
		return E_INVALIDARG;
	}

	INT iRet = SWPAR_OK;	
	CHAR szIP[32] = {0};
	CHAR szMask[32] = {0};
	CHAR szGateway[32] = {0};
	CHAR szMAC[32] = {0};
	
	BYTE bMsg[256] = {0};

	DWORD dwIP[4] = {0};
	DWORD dwMask[4] = {0};
	DWORD dwGateway[4] = {0};
	DWORD dwMAC[6] = {0};

	CHAR szSN[256] = {0};
	CHAR szMode[128] = {0};
	INT iLen = 0;

	DWORD dwNetID = 0;
	CHAR szEthernet[8] = {0};
	do {		
		swpa_sprintf(szEthernet, "eth%d", dwNetID);
		iRet = swpa_tcpip_getinfo(szEthernet, szIP, 32, szMask, 32, szGateway, 32, szMAC, 32);
		dwNetID++;
	} while ( SWPAR_OK != iRet && 5 > dwNetID );

	if (5 <= dwNetID)
	{
		PRINT("Err: failed to get net info! (eth0~eth%d)\n", dwNetID);
		return E_FAIL;
	}
	
	
	PRINT("Info: got ip info...\n");
	PRINT("Info: szIP: %s; szMask: %s; szMAC: %s; szGateway: %s\n", szIP,  szMask,  szMAC, szGateway);
	
	swpa_sscanf(szIP, "%d.%d.%d.%d", &dwIP[0], &dwIP[1], &dwIP[2], &dwIP[3]);
	swpa_sscanf(szMask, "%d.%d.%d.%d", &dwMask[0], &dwMask[1], &dwMask[2], &dwMask[3]);
	swpa_sscanf(szGateway, "%d.%d.%d.%d", &dwGateway[0], &dwGateway[1], &dwGateway[2], &dwGateway[3]);
	swpa_sscanf(szMAC, "%x:%x:%x:%x:%x:%x", &dwMAC[0], &dwMAC[1], &dwMAC[2], &dwMAC[3], &dwMAC[4], &dwMAC[5]);

	DWORD dwIPVal = 0;
	DWORD dwMaskVal = 0;
	DWORD dwGatewayVal = 0;
	BYTE bMACVal[6] = {0};
	//DWORD dwIPVal = 0;
	dwIPVal = dwIP[0]<<24 | dwIP[1]<<16 | dwIP[2]<<8 | dwIP[3];	
	dwMaskVal = dwMask[0]<<24 | dwMask[1]<<16 | dwMask[2]<<8 | dwMask[3];
	dwGatewayVal = dwGateway[0]<<24 | dwGateway[1]<<16 | dwGateway[2]<<8 | dwGateway[3];
	for (DWORD i=0; i<sizeof(bMACVal); i++)
	{
		bMACVal[i] = dwMAC[i] & 0xFF;
		//PRINT("Info: bMACVal[i] = %#x\n", bMACVal[i]);
	}
	swpa_memcpy(&bMsg[0], &m_dwGETIP_COMMAND, 4);
	swpa_memcpy(&bMsg[4], bMACVal, 6);
	swpa_memcpy(&bMsg[10], &dwIPVal, 4);
	swpa_memcpy(&bMsg[14], &dwMaskVal, 4);
	swpa_memcpy(&bMsg[18], &dwGatewayVal, 4);

		
	//todo: marked for unit test: 
#if 0//todo	
	iLen = sizeof(szSN);
	iRet = swpa_info_get_device_serial_no(szSN, &iLen);
	if (SWPAR_OK != iRet)
	{
		PRINT("Err: failed to get device serial number\n");
		return E_FAIL;
	}
#else
	//todo: added for unit test:
	iLen = 16;
	swpa_snprintf(szSN, iLen, "UnitTest");
	//todo: ended
#endif	
	if (iLen > 0 && iLen < 32)
	{
		swpa_memcpy(&bMsg[22], szSN, iLen);
	}

	*pdwPackLen = 22+iLen;
	*pbReplyPack = (BYTE*)swpa_mem_alloc(*pdwPackLen);
	if (NULL == *pbReplyPack)
	{
		PRINT("Err: no memory for *pszReplyPack\n");
		return E_OUTOFMEMORY;
	}
	swpa_memset(*pbReplyPack, 0, *pdwPackLen);
	swpa_memcpy(*pbReplyPack, bMsg, *pdwPackLen);
	
	return S_OK;
}


HRESULT CSWNetCommandProcess::GenerateSetIPCMDReplay(const BYTE* pbMsg, const DWORD dwMsgLen, BYTE** pbReplyPack, DWORD* pdwPackLen)
{
	
	if (NULL == pbMsg || NULL == pbReplyPack || NULL == pdwPackLen)
	{
		return E_INVALIDARG;
	}

	INT iRet = SWPAR_OK;	
	CHAR szIP[32] = {0};
	CHAR szMask[32] = {0};
	CHAR szGateway[32] = {0};
	CHAR szMAC[32] = {0};
	
	BYTE bMsg[256] = {0};

	BYTE bIP[4] = {0};
	BYTE bMask[4] = {0};
	BYTE bGateway[4] = {0};
	BYTE bMAC[6] = {0};
	BYTE bThisMAC[6] = {0};
	DWORD dwThisMAC[6] = {0};

	CHAR szSN[128] = {0};
	CHAR szMode[128] = {0};
	INT iLen = 0;

	DWORD dwNetID = 0;
	CHAR szEthernet[8] = {0};
	do {		
		swpa_sprintf(szEthernet, "eth%d", dwNetID);
		iRet = swpa_tcpip_getinfo(szEthernet, szIP, 32, szMask, 32, szGateway, 32, szMAC, 32);
		dwNetID++;
	} while ( SWPAR_OK != iRet && 5 > dwNetID );

	if (5 <= dwNetID)
	{
		PRINT("Err: failed to get net info! (eth0~eth%d)\n", dwNetID);
		return E_FAIL;
	}
	
	swpa_sscanf(szMAC, "%x:%x:%x:%x:%x:%x", &dwThisMAC[0], &dwThisMAC[1], &dwThisMAC[2], &dwThisMAC[3], &dwThisMAC[4], &dwThisMAC[5]);
	for (DWORD i=0; i<sizeof(bThisMAC); i++)
	{
		bThisMAC[i] = dwThisMAC[i] & 0xFF;
		//PRINT("Info: bThisMAC[i] = %#x\n", bThisMAC[i]);
	}
	swpa_memcpy(bMAC, &pbMsg[4], 6);

	if (0 == swpa_memcmp(bMAC, bThisMAC, sizeof(bMAC)))
	{
		swpa_memcpy(bIP, &pbMsg[10], 4);
		swpa_memcpy(bMask, &pbMsg[14], 4);
		swpa_memcpy(bGateway, &pbMsg[18], 4);

		swpa_snprintf(szIP, sizeof(szIP)-1, "%d.%d.%d.%d", bIP[3], bIP[2], bIP[1], bIP[0]);
		swpa_snprintf(szMask, sizeof(szMask)-1, "%d.%d.%d.%d", bMask[3], bMask[2], bMask[1], bMask[0]);
		swpa_snprintf(szGateway, sizeof(szGateway)-1, "%d.%d.%d.%d", bGateway[3], bGateway[2], bGateway[1], bGateway[0]);
		
		PRINT("Info: szIP = %s\n", szIP);
		PRINT("Info: szMask = %s\n", szMask);
		PRINT("Info: szGateway = %s\n", szGateway);


		CHAR* pszNetInfo[4];
		pszNetInfo[0] = szIP;
		pszNetInfo[1] = szMask;
		pszNetInfo[2] = szGateway;
		pszNetInfo[3] = NULL;

		if (FAILED(CSWMessage::SendMessage(MSG_APP_SETIP, (WPARAM)pszNetInfo, 0)))
		{
			SW_TRACE_DEBUG("Err: failed to save net info\n");
			return E_FAIL;
		}
		
		*pdwPackLen = 8;
		*pbReplyPack = (BYTE*)swpa_mem_alloc(*pdwPackLen);
		if (NULL == *pbReplyPack)
		{
			PRINT("Err: no memory for *pszReplyPack\n");
			return E_OUTOFMEMORY;
		}
		swpa_memset(*pbReplyPack, 0, *pdwPackLen);
		swpa_memcpy(*pbReplyPack, &m_dwSETIP_COMMAND, sizeof(m_dwSETIP_COMMAND));

		return S_OK;
	}


	return E_INVALIDARG;
}




HRESULT CSWNetCommandProcess::OnProbe(VOID)
{
	HRESULT hr = S_OK;

	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}	


	CSWUDPSocket * pUDPSock = NULL;
	pUDPSock = new CSWUDPSocket();
	if (NULL == pUDPSock)
	{
		PRINT("Err: no memory for pUDPSock\n");
		hr = E_OUTOFMEMORY;
		goto OUT;
	}
	pUDPSock->Create();

	hr = pUDPSock->Bind(NULL, m_wProbePort);
	if (FAILED(hr))
	{
		PRINT("Err: udpsock failed to bind \n");
		hr = E_FAIL;
		goto OUT;
	}

	pUDPSock->SetRecvTimeout(4000);
	pUDPSock->SetSendTimeout(4000);

	PRINT("Info: OnProbe Thread listening Port #%d\n", m_wProbePort);
	while (STATE_RUNNING == m_dwState)
	{
		BYTE bMsgIn[512] = {0};
		DWORD dwRecvLen = 0;
		DWORD dwSendLen = 0;
		BOOL  fNeedReboot = FALSE;

		hr = pUDPSock->RecvFrom((VOID*)bMsgIn, sizeof(bMsgIn), &dwRecvLen);
		if (FAILED(hr) || 0 >= dwRecvLen)
		{			
			//CSWApplication::Sleep(2000);
			continue;
		}

		//PRINT("Info: dwRecvLen = %d\n", dwRecvLen);
		
		BYTE* pbReplyPack = NULL;
		DWORD dwPackLen = 0;
		if (sizeof(DWORD) == dwRecvLen && (0 == swpa_memcmp(&bMsgIn[0], &m_dwGETIP_COMMAND, sizeof(m_dwGETIP_COMMAND))))
		{
			//PRINT("Info: Got GetIp CMD\n");
			
			if (FAILED(GenerateProbeReplyXml((CHAR**)&pbReplyPack, &dwPackLen)))
			{
				PRINT("Err: failed to generate probe reply xml\n");
				
				SAFE_MEM_FREE(pbReplyPack);
				continue;//return E_FAIL;
			}
			PRINT("Info: GetIp -- OK\n");
		}
		else if (22 == dwRecvLen && 0 == swpa_memcmp(&bMsgIn[0], &m_dwSETIP_COMMAND, sizeof(m_dwSETIP_COMMAND)))
		{
			//PRINT("Info: Got SetIp CMD\n");
			if (FAILED(GenerateSetIPCMDReplay(bMsgIn, dwRecvLen, &pbReplyPack, &dwPackLen)))
			{
				PRINT("Err: failed to generate SetIP Reply Msg\n");
				SAFE_MEM_FREE(pbReplyPack);
				continue;
			}
			PRINT("Info: SetIp -- OK\n");

			fNeedReboot = TRUE;
		}
		else
		{
			//PRINT("Err: discards unknown cmd\n");
			continue;
		}

 		if (NULL != pbReplyPack && 0 != dwPackLen)
		{
			WORD wDstPort = 0;
			pUDPSock->GetPeerName(NULL, &wDstPort);
			//网络内设备过多时，随机休眠一定时间后再回应，可以减小网络广播压力
			swpa_utils_srand(CSWDateTime::GetSystemTick());
			swpa_thread_sleep_ms(swpa_utils_rand()%800);
			if (FAILED(pUDPSock->Broadcast(wDstPort, pbReplyPack, dwPackLen)) )
			{
				PRINT("Err: failed to broadcast response data\n");
				
				SAFE_MEM_FREE(pbReplyPack);
				dwPackLen = 0;
				continue;				
			}
			PRINT("Info: Broadcast response -- OK\n");
			
			SAFE_MEM_FREE(pbReplyPack);
			dwPackLen = 0;
		}

		if (fNeedReboot)
		{
			CSWMessage::SendMessage(MSG_APP_RESETDEVICE, (WPARAM)2, 0);
		}
	}

OUT:


	SAFE_RELEASE(pUDPSock);

	return hr;
}


VOID* CSWNetCommandProcess::OnProbeProxy(VOID* pvParam)
{
	if (NULL == pvParam)
	{
		return (VOID*)E_INVALIDARG;
	}

	CSWNetCommandProcess * pThis = (CSWNetCommandProcess *)pvParam;

	return (VOID*)pThis->OnProbe();
}





HRESULT CSWNetCommandProcess::OnGetProtocol(WPARAM wParam, LPARAM lParam)
{
	INT* piProtocol = (INT*)lParam;

	if (NULL == piProtocol)
	{
		PRINT("Err: NULL == piProtocol\n");
		return E_INVALIDARG;
	}

	*piProtocol = PROTOCOL_MERCURY;

	return S_OK;
}




