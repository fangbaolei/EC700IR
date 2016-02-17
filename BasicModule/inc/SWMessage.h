///////////////////////////////////////////////////////////
//  SWMessage.h
//  Implementation of the Class CSWMessage
//  Created on:      27-二月-2013 16:29:45
//  Original author: huanggc
///////////////////////////////////////////////////////////

#if !defined(EA_3172651B_B5B4_4ca8_8467_58A8FE88DC5E__INCLUDED_)
#define EA_3172651B_B5B4_4ca8_8467_58A8FE88DC5E__INCLUDED_
#ifdef WIN32
#include <windows.h>
#endif
#include "SWObjectList.h"
#include "SWClassFactory.h"
#include "SWMessageStruct.h"

/**
 * @brief 消息处理基类，所有需要接收消息的模块都必须继承此模块
 */
typedef DWORD WPARAM;
typedef DWORD LPARAM;

/* BEGIN: Modified by huangdachang, 2015/8/14 
特别注意: 以后在下面的枚举变量中发布消息Id后，要在后面进行相应的注释，注释主要包括参数wParam、lParam的消息格式、
函数功能、消息结构体所在的位置等，同时对消息结构体的中的每一个字段，必须有详细的说明，使用
调用者一看就知道如何使用这个消息。注释格式如下:
enum{

//Set消息有以下两种注释:
消息ID1,//[type:SET][wParam:STPhoneMig*][lParam:sizeof(STPhoneMig)][local:<消息结构体所在的头文件>][desc:<功能描述>]
消息ID2,//[type:SET][wParam:INT][lParam:0][local:<消息结构体所在的头文件>][desc:<功能描述>]

//Get消息有以下两种注释:
消息ID1,//[type:GET][wParam:0][lParam:0][local:<消息结构体所在的头文件>][desc:<功能描述>]
消息ID2,//[type:GET][wParam:sizeof(STPhoneMig)][lParam:STPhoneMig*][local:<消息结构体所在的头文件>][desc:<功能描述>]

}
*/
enum{

    // 相机参数控制模块
    MSG_CAMERA_CONTROL_START,

    MSG_INITIAL_PARAM ,                      // 设置初始化参数
    MSG_SET_AGCTH ,                          // 设置AGC亮度期望值
    MSG_GET_AGCTH ,                          // 获取AGC亮度期望值

    MSG_SET_MAXSHU_GAIN,                    // 设置AGC最大快门及增益
    MSG_GET_MAXSHU_GAIN,                    // 获取AGC最大快门及增益

    MSG_SET_SHU_RANGE,                    // 设置AGC曝光时间范围
    MSG_GET_SHU_RANGE,                    // 获取AGC曝光时间范围

    MSG_SET_GAIN_RANGE,                    // 设置AGC增益范围
    MSG_GET_GAIN_RANGE,                    // 获取AGC增益范围

	MSG_SET_AGC_SCENE,						//设置AGC情景模式
	MSG_GET_AGC_SCENE,						//获取AGC情景模式

    MSG_SET_MINSHU_GAIN,                    // 设置AGC最小快门及增益
    MSG_GET_MINSHU_GAIN,                    // 获取AGC最小快门及增益

    MSG_SET_AGCZONE,						// 设置AGC测光区域
    MSG_GET_AGCZONE,						// 读取AGC测光区域

    MSG_SET_AGCGAIN,                        // 手动AGC增益指令
    MSG_GET_AGCGAIN,                        // 获取当前增益
    MSG_SET_AGCENABLE,                      // 自动增益使能
    MSG_GET_AGCENABLE,                      // 获取自动增益使能
    MSG_SET_SHUTTER,                        // 设置快门指令
    MSG_GET_SHUTTER,                        // 获取当前快门

    MSG_SET_AE_MODE,                        // AE
    MSG_GET_AE_MODE,                        // AE

    MSG_SET_RGBGAIN,                        // 手动白平衡指令
    MSG_GET_RGBGAIN,                        // 获取当前RGB增益
    MSG_SET_AWBENABLE,                      // 自动白平衡使能
    MSG_GET_AWBENABLE,                      // 获取白平衡使能
    MSG_SET_AWB_MODE,
    MSG_GET_AWB_MODE,

    MSG_SET_GAMMA,                          // 伽玛设置指令
    MSG_SET_GAMMA_MODE,						// 设置伽玛分流模式命令
    MSG_GET_GAMMA_MODE,						// 读取伽玛分流模式命令
    MSG_SET_GAMMA_STRENGTH,                 // 设置伽玛强度值
    MSG_GET_GAMMA_STRENGTH,                 // 读取伽玛强度值
    MSG_SET_GAMMA_ENABLE,
    MSG_GET_GAMMA_ENABLE,

    MSG_SET_DIFFCORRENABLE,                 // 差异性校正使能

    MSG_SET_SOFT_TOUCH,                     // 设置抓拍软触发

	MSG_SET_F1_OUTPUT_TYPE,					//设置F1输出类型

	MSG_SET_CAPTURE_AUTO_PARAM_ENABLE,		//自动抓拍参数使能
	MSG_GET_CAPTURE_AUTO_PARAM_ENABLE,		//自动抓拍参数使能
    MSG_CAPTURERGB_ENABLE ,                 // 抓拍RGB使能
    MSG_CAPTURESHUTTER_ENABLE,              // 抓拍快门使能
    MSG_CAPTUREGAIN_ENABLE,                 // 抓拍增益使能
    MSG_CAPTURESHARPEN_ENABLE,              // 抓拍锐化

    MSG_SET_CAPTURE_ENABLE,                 // 抓拍使能
    MSG_SET_CAPTURESHUTTER,                 // 抓拍快门
    MSG_GET_CAPTURESHUTTER,                 // 获取当前抓拍增益

    MSG_SET_CAPTURE_SHARPEN_ENABLE,         // 抓拍锐化使能
    MSG_SET_CAPTURE_SHARPEN_THR,            // 抓拍锐化阈值
    MSG_GET_CAPTURE_SHARPEN_THR,            // 获取抓拍锐化阈值

    MSG_SET_CAPTRUEGAIN,                    // 抓拍增益设置
    MSG_GET_CAPTUREGAIN,                    // 获取抓拍增益

    MSG_SET_CAPTRUERGBGAIN,                 // 设置抓拍RGB增益
    MSG_GET_CAPTRUERGBGAIN,                 // 获取抓拍RGB增益

    MSG_SET_AGCPARAM,
    MSG_GET_AGCPARAM,
    MSG_SET_DCAPERTUREENABLE,               // 设置自动DC光圈使能标志
    MSG_GET_DCAPERTUREENABLE,               // 读取自动DC光圈使能标志

    MSG_ZOOM_DCAPERTURE,       //放大DC光圈状态
    MSG_SHRINK_DCAPERTURE,       //缩小DC光圈状态

	MSG_SET_SHARPENMODE,                    // 设置锐化使能模式
    MSG_GET_SHARPENMODE,        //读取锐化使能模式

    MSG_SET_SHARPEN_ENABLE,                    // 设置锐化使能
    MSG_GET_SHARPEN_ENABLE,        //读取锐化使能

    MSG_SET_SHARPENTHRESHOLD,               // 设置锐化阀值
    MSG_GET_SHARPENTHRESHOLD,//读取锐化阈值

    MSG_SET_WDR_STRENGTH,                      // 设置WDR强度值
    MSG_GET_WDR_STRENGTH,//读取WDR强度值

    MSG_SET_NR_LEVEL,
    MSG_GET_NR_LEVEL,

	MSG_SET_COLORMATRIXMODE,                // 设置图像增强使能模式
    MSG_GET_COLORMATRIXMODE,//读取图像增强使能模式

    MSG_SET_COLORMATRIX_ENABLE,   // 设置图像增强使能（饱和度、对比度）
    MSG_GET_COLORMATRIX_ENABLE,    //读取图像增强使能（饱和度、对比度）

    MSG_SET_CONTRASTTHRESHOLD,              // 设置对比度阈值
    MSG_GET_CONTRASTTHRESHOLD,//读取对比度阈值

    MSG_SET_SATURATIONTHRESHOLD,            // 设置饱和度阈值
    MSG_GET_SATURATIONTHRESHOLD,//读取饱和度阈值

    MSG_SET_TEMPERATURETHRESHOLD,           // 色温
	MSG_SET_HUETHRESHOLD,                   // 色度

	MSG_SET_HAZEREMOVALMODE,                // 设置除雾使能模式
	MSG_SET_HAZEREMOVALTHRESHOLD,           // 除雾阀值

	MSG_SET_FLASHRATE_ENABLE,
	MSG_SET_CAPTURESYNC_ENABLE,
	MSG_SET_FLASHRATE_PULSE,

	MSG_SET_FILTERSWITCH,                   // 滤光片切换
    MSG_GET_FILTERSWITCH,                   // 滤光片切换

    MSG_SET_REDLIGHT_ENABLE,//设置红灯加红开关
    MSG_GET_REDLIGHT_ENABLE,//读取红灯加红开关

    MSG_SET_REDLIGHT_RECT,					// 设置红灯加红区域
    MSG_SET_TRAFFIC_LIGTH_LUM_TH, //设置红灯加红

	MSG_SET_GREENLIGHT_RECT,					// 设置绿灯加绿区域
    MSG_SET_TRAFFIC_GREEN_LIGTH_LUM_TH, //设置绿灯加绿

    MSG_SET_CAPTUREEDGE,    // 设置触发抓拍沿
	MSG_GET_CAPTUREEDGE,    // 获取触发抓拍沿

    MSG_GET_BARRIER_STATE,

    MSG_SET_SNF_ENABLE,//设置降噪SNF使能
    MSG_GET_SNF_ENABLE,//读取降噪SNF使能

    MSG_SET_TNF_ENABLE,//设置降噪TNF使能
    MSG_GET_TNF_ENABLE,//读取降噪TNF使能

    MSG_SET_DENOISE_MODE,//设置降噪模式
    MSG_GET_DENOISE_MODE,//读取降噪模式

    MSG_SET_DENOISE_STATE,//设置降噪
    MSG_GET_DENOISE_STATE,//读取降噪

    MSG_SET_AC_SYNC_ENABLE,//设置电网同步使能标志
    MSG_GET_AC_SYNC_ENABLE,//读取电网同步使能标志

    MSG_SET_AC_SYNC_DELAY,//设置电网同步延时值
    MSG_GET_AC_SYNC_DELAY,//读取电网同步延时值

    MSG_SET_GRAY_IMAGE_ENABLE,//设置黑白图使能标志
    MSG_GET_GRAY_IMAGE_ENABLE,//读取黑白图使能标志

    MSG_SET_CVBS_STD,//设置CVBS制式
    MSG_GET_CVBS_STD,//读取CVBS制式

    MSG_SET_FILL_LIGHT_ENABLE,//设置补光灯使能
    MSG_GET_FILL_LIGHT_ENABLE,//读取补光灯使能

    MSG_SET_CAM_TEST,       //相机自动化测试
    MSG_SET_CAM_FPGA_REG,       //相机设置FPGA寄存器
    MSG_GET_CAM_FPGA_REG,       //相机获取FPGA寄存器

//    MSG_GET_JPEG_QUALITY,//临时
//    MSG_SET_JPEG_QUALITY,//临时

    MSG_SET_EDGE_ENHANCE,//设置图像边缘增强
    MSG_GET_EDGE_ENHANCE,

    MSG_GET_MANUAL_VALUE,//获取手动RGB 快门增益

    MSG_GET_CAMERA_WORKSTATE,//获取相机工作状态

    MSG_GET_M3_DAYNIGHT_STATUS,//获取M3白天晚上标识
	//
	MSG_SET_LED_SWITCH,		//设置补光灯开关
	MSG_GET_LED_SWITCH,		//获取补光灯快关状态

	MSG_SET_WDR_LEVEL,  //设置WDR等级

	MSG_SET_TRIGGEROUT_STATE,	//设置触发输出状态
	
	MSG_SET_TRIGGEROUT,		//设置触发输出口 	F1
	MSG_GET_TRIGGEROUT,		//读取触发输出口状态(测试用) 	F1

	MSG_SET_F1_TRIGGEROUT,	//F1口闪光灯
	MSG_GET_F1_TRIGGEROUT,	//F1口闪光灯
	MSG_SET_ALM_TRIGGEROUT, //ALM口闪光灯
	MSG_GET_ALM_TRIGGEROUT, //ALM口闪光灯
	
	MSG_SET_EXP_IO,			//设置补光灯输出口(测试用) 	EXP
	MSG_GET_EXP_IO,			//读取补光灯输出口状态(测试用) 	EXP
	
	MSG_SET_ALARM_IO,		//设置报警输出口(测试用) 	ALM
	MSG_GET_ALARM_IO,		//读取报警输出口状态(测试用) 	ALM

	MSG_SET_TG_IO,			//设置抓拍输入口触发类型
	MSG_GET_TG_IO,			//读取抓拍输入口触发类型
	MSG_SET_COLOR_GRADATION, //设置色阶 0 ： 0~255；1：16~235.

	MSG_SET_WORK_MODE,		//设置工作模式 0-收费站\纯相机，1-高速路模式，6-电子警察，7-停车场

	MSG_SET_EXP_PLUSEWIDTH,

	MSG_SET_TNF_STATE,		//设置降噪TNF使能,M3真实开关
	MSG_GET_TNF_STATE,		//读取降噪TNF使能,M3真实开关
	MSG_SET_SNF_STATE,		//设置降噪SNF使能,M3真实开关
	MSG_GET_SNF_STATE,		//读取降噪SNF使能,M3真实开关
	// 球机相关。
	MSG_INITIAL_DOMECAMERA,
	MSG_RESETDEFAULT_DOMECAMERA,
	MSG_RESTOREFACTORY_DOMECAMERA,
	MSG_GET_DOME_VERSION,
	MSG_GET_CAMERA_VERSION,
	MSG_GET_CAMERA_MODEL,

	MSG_SET_PAN_COORDINATE,
	MSG_GET_PAN_COORDINATE,
	MSG_SET_TILT_COORDINATE,
	MSG_GET_TILT_COORDINATE,
	MSG_SET_PT_COORDINATE,
	MSG_GET_PT_COORDINATE,
	MSG_SET_ZOOM,
	MSG_GET_ZOOM,
	MSG_START_PAN,
	MSG_STOP_PAN,
	MSG_START_INFINITY_PAN,
	MSG_STOP_INFINITY_PAN,
	MSG_START_TILT,
	MSG_STOP_TILT,
	MSG_START_PAN_TILT,
	MSG_STOP_PAN_TILT,
	MSG_SET_PAN_SPEED,
	MSG_GET_PAN_SPEED,
	MSG_SET_TILT_SPEED,
	MSG_GET_TILT_SPEED,
	MSG_SET_FOCUS,
	MSG_GET_FOCUS,
	MSG_SET_FOCUS_MODE,
	MSG_GET_FOCUS_MODE,
	MSG_SET_AF_ENABLE,
	MSG_GET_AF_ENABLE,
	MSG_DO_ONE_FOCUS,
	MSG_MOVE_POINT_TO_CENTER,
	MSG_MOVE_BLOCK_TO_CENTER,
	MSG_TUNE_IRIS,
	MSG_TUNE_ZOOM,
	MSG_TUNE_FOCUS,
	MSG_START_ZOOM,
	MSG_STOP_ZOOM,
	MSG_SET_DEFOG,
	MSG_SET_EXPOCOMP_VALUE,
	MSG_GET_EXPOCOMP_VALUE,

	MSG_SET_IRIS,
	MSG_GET_IRIS,
	MSG_SET_IRIS_MODE,
	MSG_GET_IRIS_MODE,
	MSG_START_VTURNOVER,
	MSG_STOP_VTURNOVER,
	MSG_START_WIPER,
	MSG_STOP_WIPER,
	MSG_SET_WIPER_SPEED,
	MSG_GET_WIPER_SPEED,
	MSG_SET_WIPER_AUTOSTART_TIME,
	MSG_GET_WIPER_AUTOSTART_TIME,
	MSG_SET_LED_MODE,
	MSG_GET_LED_MODE,
	MSG_SET_LED_POWER,
	MSG_GET_LED_POWER,
	MSG_SET_MAX_PAN_SPEED,
	MSG_GET_MAX_PAN_SPEED,
	MSG_SET_MAX_TILT_SPEED,
	MSG_GET_MAX_TILT_SPEED,
	MSG_RESET_DOME_POSITION,

	MSG_SET_PRESET_POS,
	MSG_GET_PRESET_POS,
	MSG_CALL_PRESET_POS,
	MSG_CLEAR_PRESET_POS,
	MSG_SET_PRESET_CHECKPT_FLAG,
	MSG_GET_PRESET_CHECKPT_FLAG,
	MSG_GET_PRESET_NAME_LIST,

	MSG_SET_HSCAN_LEFT_POS,
	MSG_GET_HSCAN_LEFT_POS,
	MSG_SET_HSCAN_RIGHT_POS,
	MSG_GET_HSCAN_RIGHT_POS,
	MSG_START_HSCAN,
	MSG_STOP_HSCAN,
	MSG_SET_HSCAN_PARAM,
	MSG_GET_HSCAN_PARAM,
	MSG_CLEAR_HSCAN,

	MSG_START_FSCAN_RECORD,
	MSG_STOP_FSCAN_RECORD,
	MSG_CALL_FSCAN,
	MSG_STOP_FSCAN,
	MSG_CLEAR_FSCAN,
	MSG_SET_FSCAN_PARAM,
	MSG_GET_FSCAN_PARAM,
	MSG_GET_FSCAN_NAME_LIST,

	MSG_SET_CRUISE,
	MSG_GET_CRUISE,
	MSG_CALL_CRUISE,
	MSG_STOP_CRUISE,
	MSG_CLEAR_CRUISE,

	MSG_SET_WATCHKEEPING,
	MSG_GET_WATCHKEEPING,
	MSG_CLEAR_WATCHKEEPING,
	MSG_START_WATCHKEEPING,
	MSG_STOP_WATCHKEEPING,

	MSG_SET_MASK,
	MSG_GET_MASK,
	MSG_CLEAR_MASK,
	MSG_SHOW_MASK,
	MSG_HIDE_MASK,

	MSG_SET_FREEZEFLAG,
	MSG_GET_FREEZEFLAG,
	MSG_SAVE_FREEZEFLAG,

	MSG_SET_TIMER,
	MSG_GET_TIMER,
	MSG_CLEAR_TIMER,
	MSG_SET_TIMER_FLAG,
	MSG_GET_TIMER_FLAG,

	MSG_SET_IDLE_TIME_TO_SLEEP,
	MSG_GET_IDLE_TIME_TO_SLEEP,
	MSG_SET_SLEEP_FLAG,
	MSG_GET_SLEEP_FLAG,
	MSG_START_SLEEP_COUNTDOWN,
	MSG_GET_SLEEP_STATUS,
	
    MSG_SET_DOME_XML,
	MSG_GET_DOME_XML,	

	MSG_PAUSE,
	MSG_RESUME,

	MSG_GET_CUR_TASK,
    MSG_TOUCH_DOME_CAMERA,
    MSG_CAMERA_DO_CMD,
    MSG_CAMERA_CONTROL_END,

    //JPEG压缩模块
    MSG_JPEGENCODE_START,
    MSG_JPEGENCODE,         //JPEG压缩
	MSG_JPEGENCODE_COMPRESSRATE,	//设置视频流压缩率
	MSG_JPEGENCODE_GET_COMPRESSRATE,
	MSG_JPEGENCODE_COMPRESSRATE_CAPTURE, //设置抓拍图压缩率
	MSG_JPEGENCODE_GET_COMPRESSRATE_CAPTURE,
	MSG_SET_AUTO_JPEG_COMPRESS_ENABLE,
    MSG_GET_AUTO_JPEG_COMPRESS_ENABLE,//自动jpeg压缩使能       INT
    MSG_SET_AUTO_JPEG_COMPRESS_PARAM,//获取自动压缩图片的上限、下限、图片大小  		一维数组 3
    MSG_GET_AUTO_JPEG_COMPRESS_PARAM,//获取自动压缩图片的上限、下限、图片大小  		一维数组 3
    MSG_SET_JPEG_COMPRESS_RECT, //设置JPEG压缩的截图区域。
    MSG_JPEGENCODE_END,

    //自动化控制模块
    MSG_AUTO_CONTROL_START,
    MSG_AUTO_CONTROL_READPARAM,
	MSG_AUTO_CONTROL_GET_REALPARAM,
	MSG_AUTO_CONTROL_GET_ENVPERIOD,	//获取时段信息，0白天，1晚上
	MSG_AUTO_SET_MAXAGCSHUTTER,
	MSG_AUTO_CONTROL_SET_AUTO_CAPTURE_PARAM,	//设置自动抓拍参数
    MSG_AUTO_CONTROL_END,

    //识别
    MSG_RECOGNIZE_CTRL_START,
    MSG_RECOGNIZE_PHOTO,
    MSG_RECOGNIZE_GETJPEG,
    MSG_RECOGNIZE_OUTPUTDEBUG,
    MSG_RECOGNIZE_ENABLE_TRIGGER,
    MSG_RECOGNIZE_IOEVENT,
    MSG_RECOGNIZE_MODIFY_PARAM,
    MSG_RECOGNIZE_GET_CARLEFT_COUNT,		//获取车辆统计计数
    MSG_RECOGNIZE_CHECKPT,
	MSG_RECOGNIZE_STOP,
	MSG_RECOGNIZE_MODIFY_PECCANCYPARKING_PARAM,
	MSG_RECOGNIZE_ENVPERIOD_CHANGED,
    MSG_RECOGNIZE_GET_DSP_ENV_LIGHT_TYPE,           // 获取 DSP 判断的环境（0白天，1傍晚，2晚上）
    MSG_RECOGNIZE_CTRL_END,

	//网络命令
    MSG_NETCOMMAND_START,
    MSG_NETCOMMAND_GET_PROTOCOL,
    MSG_SET_SYNC_POWER,
    MSG_GET_SYNC_POWER,
    MSG_GET_CAMERA_BASIC_INFO,
    MSG_GET_DEV_BASIC_INFO,
    MSG_GET_CAMERA_STATE,
    MSG_GET_VIDEO_STATE,
    MSG_GET_H264_CAPTION,
    MSG_GET_H264_SECOND_CAPTION,	//获取第二路H264参数
    MSG_GET_JPEG_CAPTION,
    MSG_GET_DEVSTATE,
  //  MSG_NULL,//带处理

    MSG_NETCOMMAND_END,

	//Application
	MSG_APP_START,
    MSG_APP_SETTIME,//设置时间
	MSG_APP_GETTIME,
    MSG_APP_SETIP, //设置IP
	MSG_APP_GETIP,
    MSG_APP_SETXML, //上传XML
	MSG_APP_GETXML,
    MSG_APP_GETWORKMODE_COUNT,
    MSG_APP_GETWORKMODE,                //获取工作模式
	MSG_APP_GETWORKMODEINDEX,
    MSG_APP_RESETDEVICE,                //重启设备
    MSG_APP_RESTORE_DEFAULT,            //恢复默认
    MSG_APP_RESTORE_FACTORY,            //恢复出厂
	MSG_APP_SETCHARACTER_ENABLE,
	MSG_APP_GETCHARACTER_ENABLE,
    MSG_APP_GETHVNAME,                  //获取设备名字
    MSG_APP_GETHVID,                    //获取设备ID
    MSG_APP_GETPRODUCT_NAME,            //获取产家名字
	MSG_APP_GETVIDEO_COUNT,
	MSG_APP_GETVERSION,

    MSG_APP_GETFIRMWAREVERSION, //获取固件包版本 new
    MSG_APP_GETNETPACKAGEVERSION, //获取net包版本 new
    MSG_APP_GETBACKUPTVERSION, //获取备份版本 new
    MSG_APP_GETFPGAVERSION, //获取fpga版本 new
    MSG_APP_GETKERNALVERSION, //获取KERNAL版本 new
    MSG_APP_GETUBOOTVERSION, //获取uboot版本 new
    MSG_APP_GETUBLVERSION, //获取 ubl 版本 new
    MSG_APP_GETSN, //获取SN new
    MSG_APP_GETCPU,//获取设备CPU状态 int 0-100 new
    MSG_APP_GETRAM,//获取设备内存使用率状态 int 0-100 new
 //   MSG_APP_GETDEVSTATE,//获取设备CPU、内存、温度、连接的上位机IP列表、设备时间、硬盘/SD卡使用状态，复位次数。 new
    MSG_APP_GET_LOG_LEVEL, //获取打印等级 new
    MSG_APP_SET_LOG_LEVEL, //设置打印等级
    MAG_APP_GET_NTP_SERVER_IP,//ntp new
    MAG_APP_SET_NTP_SERVER_IP,//ntp new
    MAG_APP_GET_NTP_ENABLE,
    MAG_APP_SET_NTP_ENABLE,
    MAG_APP_GET_NTP_TIMEZONE,
    MAG_APP_SET_NTP_TIMEZONE,
    MAG_APP_SET_NTP_INTERVAL,
    MAG_APP_GET_NTP_INTERVAL,
 	MSG_APP_HDD_CHECKREPORT, //从设备端获取硬盘状态报告new

	MSG_APP_GETVRESION_STRING,
	MSG_APP_GETCONNECTED_IP,
	MSG_APP_HDD_FDISK,
	MSG_APP_HDD_CHECK,
	MSG_APP_HDD_STATUS,
	MSG_APP_RESET_COUNT, //获取复位次数
	MSG_APP_RESET_REPORT, //从设备端获取整个复位日志文件
	MSG_APP_RESET_MODE,
	MSG_APP_GETDEVTYPE,
	MSG_APP_GETCPU_TEMPERATURE,
	MSG_APP_UPDATE_STATUS,
	MSG_APP_GET_RUN_STATUS,

	MSG_APP_RADAR_TRIGGER,	//雷达触发抓拍
	
	MSG_APP_COIL_STATUS,
	MSG_APP_COIL_TRIGGER,	
	MSG_APP_GET_BLACKBOX_MESSAGE,

	MSG_APP_SET_CUSTOMIZED_DEVNAME,	//设置设备名称
	MSG_APP_GET_CUSTOMIZED_DEVNAME,	//获取设备名称

	MSG_APP_REINIT_HDD,

	MSG_APP_SET_AWB_WORK_MODE,			//设置AWB工作模式
	MSG_APP_GET_AWB_WORK_MODE,			//获取AWB工作模式
	MSG_APP_SET_AUTO_CONTROL_CAMMERA_ALL,		//设置相机控制模式
	MSG_APP_GET_AUTO_CONTROL_CAMMERA_ALL,		//获取相机控制模式
	MSG_APP_SET_H264_RESOLUTION,						//设置H264分辨率
	MSG_APP_GET_H264_RESOLUTION,						//获取H264分辨率

	MSG_APP_REMOTE_PTZ_CONTROL,
	MSG_APP_REMOTE_GET_PTZ_INFO,
	//MSG_APP_REMOTE_SET_PTZ_INFO,

	MSG_APP_GET_28181_PARAM,//跨进程消息
    MSG_APP_SET_28181_QUIT,	//跨进程消息，设置GB28181模块退出,mark
    MSG_APP_REBOOT,			//跨进程消息
	MSG_APP_GET_RTSP_PARAM, //与RTSP进程通信
	MSG_APP_GET_ONVIF_PARAM,	//与ONVIF进程通信
	MSG_APP_SET_DENOISE_STATUS,	//跨进程消息 设置降噪参数
	
	
	MSG_APP_END,

	// DM368相关控制
	MSG_DM368_CTRL_START,
	MSG_DM368_INIT_MENU,
	MSG_DM368_MENU_LEFT,
	MSG_DM368_MENU_RIGHT,
	MSG_DM368_MENU_UP,
	MSG_DM368_MENU_DWON,
	MSG_DM368_SYNC_TIME,
	MSG_DM368_HEARTBEAT,
	MSG_DM368_CHAR_OVERLAY,
	MSG_DM368_MENU_OK,
	MSG_DM368_MENU_CANCEL,
	MSG_DM368_GET_VERSION,
	MSG_DM368_CTRL_END,

	//H264存储
	MSG_H264HDD_FILTER_CTRL_START,
	MSG_H264HDD_FILTER_SET_TRANSMITTING_TYPE,
	MSG_H264HDD_FILTER_HISTORY_FILE_TRANSMITTING_START,
	MSG_H264HDD_FILTER_HISTORY_FILE_TRANSMITTING_STOP,
	MSG_H264HDD_FILTER_REALTIME_TRANSMITTING_START,
	MSG_H264HDD_FILTER_REALTIME_TRANSMITTING_STOP,
	MSG_H264HDD_FILTER_GET_HISTORY_FILE_TRANSMITTING_STATUS,
	MSG_H264HDD_FILTER_TRIGGER_VIDEO_SAVING,	
	MSG_H264HDD_FILTER_GB28181_VIDEOSAVING_ENABLE,
	MSG_H264HDD_FILTER_GB28181_VIDEOSEND_SCALE,
    MSG_H264HDD_FILTER_GB28181_FILE_TRANSMITTING_START,
    MSG_H264HDD_FILTER_GB28181_FILE_TRANSMITTING_STOP,
	MSG_H264HDD_FILTER_CTRL_END,
	
	//H264GB28181FILTER
	MSG_GB28181_FILTER_BEGIN,
    MSG_GB2818_TRANSFORM_COMMAND,
	MSG_GB28181_FILTER_END,

	//识别结果存储
	MSG_RESULTHDD_FILTER_CTRL_START,
	MSG_RESULTHDD_FILTER_SET_TRANSMITTING_TYPE,
	MSG_RESULTHDD_FILTER_HISTORY_FILE_TRANSMITTING_START,
	MSG_RESULTHDD_FILTER_HISTORY_FILE_TRANSMITTING_STOP,
	MSG_RESULTHDD_FILTER_REALTIME_TRANSMITTING_START,
	MSG_RESULTHDD_FILTER_REALTIME_TRANSMITTING_STOP,
	MSG_RESULTHDD_FILTER_GET_HISTORY_FILE_TRANSMITTING_STATUS,
	MSG_RESULTHDD_FILTER_CTRL_END,

	// 12v触发输出
	MSG_TRIGGER_OUT_BEGIN,
	MSG_TRIGGER_OUT_PROCESS,
    MSG_TRIGGER_OUT_ENABLE,//new
	MSG_TRIGGER_OUT_END,
	
	//JPEG字符叠加
	MSG_OVERLAY_JPEG_BEGIN,
	MSG_OVERLAY_PROCESS,
	MSG_OVERLAY_DOPROCESS,
	MSG_OSD_GET_JPEG_ENABLE,
    MSG_OSD_GET_JPEG_PLATE_ENABLE,
    MSG_OSD_GET_JPEG_TIME_ENABLE,
    MSG_OSD_GET_JPEG_TEXT,
    MSG_OSD_GET_JPEG_FONNTSIZE,
    MSG_OSD_GET_JPEG_FONT_RBG,
    MSG_OSD_GET_JPEG_POX,
    MSG_OSD_SET_JPEG_ENABLE,
    MSG_OSD_SET_JPEG_PLATE_ENABLE,
    MSG_OSD_SET_JPEG_TIME_ENABLE,
    MSG_OSD_SET_JPEG_TEXT,
    MSG_OSD_SET_JPEG_FONNTSIZE,
    MSG_OSD_SET_JPEG_FONT_RBG,//int 0~7 R 8~15 B 16~23 G
    MSG_OSD_SET_JPEG_POX, //x y
    MSG_GET_DEBUG_JPEG_STATUS,//获取调试码流开关
    MSG_SET_DEBUG_JPEG_STATUS,
    MSG_OSD_ENABLE_JPEG_PTZ_OVERLAY,
    MSG_OSD_SET_JPEG_DOMEINFO_OVERLAY,
    MSG_OSD_SET_MJPEG_OVERLAY,		//时能或停止MJPEG字符叠加，0-关闭，1-打开(只为观察叠加效果)
    MSG_OSD_SET_JPEG_DEV_ID,		//设置用户自定义设备编号
	MSG_OVERLAY_JPEG_END,
	
	//H264字符叠加
	MSG_OVERLAY_H264_BEGIN,
	MSG_OSD_GET_H264_ENABLE,
    MSG_OSD_GET_H264_PLATE_ENABLE,
    MSG_OSD_GET_H264_TIME_ENABLE,
    MSG_OSD_GET_H264_TEXT,
    MSG_OSD_GET_H264_FONNTSIZE,
    MSG_OSD_GET_H264_FONT_RBG,
    MSG_OSD_GET_H264_POX,
    MSG_OSD_SET_H264_ENABLE,
    MSG_OSD_SET_H264_PLATE_ENABLE,
    MSG_OSD_SET_H264_TIME_ENABLE,
    MSG_OSD_SET_H264_TEXT,
    MSG_OSD_SET_H264_FONNTSIZE,
    MSG_OSD_SET_H264_FONT_RBG,
    MSG_OSD_SET_H264_POX,
    MSG_OSD_SET_H264_DEV_ID,	//设置用户自定义设备编号

    MSG_OSD_ENABLE_H264_PTZ_OVERLAY,
    MSG_OSD_SET_H264_DOMEINFO_OVERLAY,
 	MSG_OVERLAY_H264_END,

		//第二路H264字符叠加
	MSG_OVERLAY_H264_SECOND_BEGIN,
	MSG_OSD_GET_H264_SECOND_ENABLE,
    MSG_OSD_GET_H264_SECOND_PLATE_ENABLE,
    MSG_OSD_GET_H264_SECOND_TIME_ENABLE,
    MSG_OSD_GET_H264_SECOND_TEXT,
    MSG_OSD_GET_H264_SECOND_FONNTSIZE,
    MSG_OSD_GET_H264_SECOND_FONT_RBG,
    MSG_OSD_GET_H264_SECOND_POX,
    MSG_OSD_SET_H264_SECOND_ENABLE,
    MSG_OSD_SET_H264_SECOND_PLATE_ENABLE,
    MSG_OSD_SET_H264_SECOND_TIME_ENABLE,
    MSG_OSD_SET_H264_SECOND_TEXT,
    MSG_OSD_SET_H264_SECOND_FONNTSIZE,
    MSG_OSD_SET_H264_SECOND_FONT_RBG,
    MSG_OSD_SET_H264_SECOND_POX,
 	MSG_OVERLAY_H264_SECOND_END,

	//H264设置
	MSG_SET_H264_START,
    MSG_SET_H264_BITRATE,
    MSG_GET_H264_BITRATE,
	MSG_SET_H264_I_FRAME_INTERVAL,
	MSG_GET_H264_I_FRAME_INTERVAL,
	MSG_SET_H264_RATE_CONTROL,
	MSG_SET_H264_VBR_DURATION,
	MSG_SET_H264_VBR_SENSITIVITY,
    MSG_SET_H264_SECOND_BITRATE,
    MSG_GET_H264_SECOND_BITRATE,
	MSG_SET_H264_SECOND_I_FRAME_INTERVAL,
	MSG_GET_H264_SECOND_I_FRAME_INTERVAL,
	MSG_SET_H264_SECOND_RATE_CONTROL,
	MSG_SET_H264_SECOND_VBR_DURATION,
	MSG_SET_H264_SECOND_VBR_SENSITIVITY,
	MSG_SET_H264_END,
 	
	// 用户管理new
	MSG_USER_BEGIN,
	MSG_USER_LOGIN,
    MSG_USER_GETLIST,
    MSG_USER_ADD,
    MSG_USER_DEL,
    MSG_USER_CHANGE,
    MSG_USER_RESTORE_DEFAULT,
    MSG_USER_MANAGE,
	MSG_USER_GETPASSWORD,
	MSG_USER_END,
	
	    //串口测试new
    MSG_COM_TEST_BEGIN,
    MSG_COM_TEST_ENABLE,
    MSG_COM_TEST_DISABLE,
	MSG_COM_OUTPUT_CTRL_ENABLE,		//外总控动态开关
    MSG_COM_TEST_END,

    MSG_GB28181_BEGIN,
    MSG_GB28181_SEND_ALARM,
    MSG_GB28181_PARAMTER_CHANGE_ALARM,
    MSG_GB28181_END,
    MSG_GB28181_QUIT,
	MSG_RTSP_BEGIN,
	MSG_RTSP_SEND_PARAM,
	MSG_RTSP_APP_RESTART,
	MSG_RTSP_END,

	MSG_ONVIF_BEGIN,
	MSG_ONVIF_SET_PARAM,
	MSG_ONVIF_APP_RESTART,
	MSG_APP_REMOTE_SET_PTZ_INFO,
	MSG_APP_REMOTE_SET_USER_INFO,
	MSG_ONVIF_END,

	MSG_SOURCE_BEGIN,
    MSG_SOURCE_GET_JPEG_RAW_FPS,
    MSG_SOURCE_GET_H264_RAW_FPS,
    MSG_SOURCE_GET_H264_SECOND_RAW_FPS,
    MSG_SOURCE_END,

    MSG_H264_BEGIN,
    MSG_H264_DO_PREVIEW,
    MSG_H264_END,

	MSG_H264_QUEUE_BEGIN,
    MSG_H264_QUEUE_GET_VIDEO,
    MSG_H264_QUEUE_END,

	//MD5加密
    MSG_MD5_ENCRYPT_BEGIN,
    MSG_MD5_ENCRYPT_PROCESS,
    MSG_SET_ENCRYPT_PARAM,
    MSG_GET_ENCRYPT_CODE,
    MSG_MD5_ENCRYPT_END,
    
    //结果Filter
    MSG_RESULT_FILTER_BEGIN,
    MSG_RESULT_FILTER_OVERLAY_POS_CHANGED,
    MSG_RESULT_FILTER_END,

	MSG_2A_BEGIN,
			//  相机基本参数
    MSG_2A_SET_EXP,      		//  设置曝光时间
    MSG_2A_GET_EXP,            //  读取曝光时间
    MSG_2A_SET_GAIN,           //  设置增益
    MSG_2A_GET_GAIN,           //  读取增益
    MSG_2A_SET_RGB,            //  设置RGB
    MSG_2A_GET_RGB,            //  读取RGB
		//  相机自动化控制
    MSG_2A_SET_AE_STATE,       //  设置AE使能
    MSG_2A_GET_AE_STATE,       //  读取AE使能标志
    MSG_2A_SET_AE_THRESHOLD,   //  设置AE门限值
    MSG_2A_GET_AE_THRESHOLD,   //  读取AE当前门限值
    MSG_2A_SET_AE_EXP_RANGE,   //  设置AE曝光时间范围
    MSG_2A_GET_AE_EXP_RANGE,   //  读取AE曝光时间范围
    MSG_2A_SET_AE_ZONE,        //  设置AE测光区域
    MSG_2A_GET_AE_ZONE,        //  读取AE测光区域
    MSG_2A_SET_AE_GAIN_RANGE,  //  设置AE增益范围
    MSG_2A_GET_AE_GAIN_RANGE,  //  读取AE增益范围
    MSG_2A_SET_SCENE,		//
    //MSG_2A_GET_SCENE,		//
    MSG_2A_SET_AWB_STATE,      //  设置AWB使能
    MSG_2A_GET_AWB_STATE,      //  读取AWB使能
//  相机图像处理
    MSG_2A_SET_WDR_STRENGTH,   //  设置WDR强度值
    MSG_2A_GET_WDR_STRENGTH,   //  读取WDR强度值
    //MSG_2A_SET_SHARPNESS_STATE,//  设置锐化使能	//todo已经不支持
    //MSG_2A_GET_SHARPNESS_STATE,//  读取锐化使能
    //MSG_2A_SET_SHARPNESS,      //  设置锐化值		//todo已经不支持
    //MSG_2A_GET_SHARPNESS,      //  读取锐化值
    MSG_2A_SET_SATURATION_CONTRAST_STATE,  //  设置饱和度、对比度使能
    MSG_2A_GET_SATURATION_CONTRAST_STATE,  //  读取饱和度、对比度使能
    MSG_2A_SET_SATURATION,     //  设置饱和度值
    MSG_2A_GET_SATURATION,     //  读取饱和度值
    MSG_2A_SET_CONTRAST,       //  设置对比度值
    MSG_2A_GET_CONTRAST,       //  读取对比度值
    MSG_2A_SET_TNF_STATE,		//  设置降噪TNF使能,只是用户设置，最终是否生效由2A算法自动控制
    MSG_2A_GET_TNF_STATE,		//  读取降噪TNF使能
    MSG_2A_SET_SNF_STATE,		//  设置降噪SNF使能,只是用户设置，最终是否生效由2A算法自动控制
    MSG_2A_GET_SNF_STATE,		//  读取降噪SNF使能
    //MSG_2A_SET_TNF_SNF_STRENGTH,   //  设置降噪强度值
    //MSG_2A_GET_TNF_SNF_STRENGTH,   //  读取降噪强度值

	//MSG_2A_SET_TRAFFIC_LIGTH_ENHANCE_STATE,    //  设置红灯加红使能 //todo已经不支持
    //MSG_2A_GET_TRAFFIC_LIGTH_ENHANCE_STATE,    //  读取红灯加红使能

	MSG_2A_SET_TRAFFIC_LIGTH_ENHANCE_REGION,   	//  设置加红区域（无读取命令）
   	MSG_2A_SET_TRAFFIC_LIGTH_LUM_TH,    		// 设置红灯加红亮度域值
    MSG_2A_SET_GAMMA_ENABLE,  	// 设置Gamma 使能
    MSG_2A_GET_GAMMA_ENABLE,  	// 获取Gamma使能
    MSG_2A_SET_GAMMA,  			// 设置Gamma
    MSG_2A_GET_GAMMA,  			// 获取Gamma
    MSG_2A_SET_EDGE_ENHANCE, 	// 设置图像边缘增强
    MSG_2A_GET_EDGE_ENHANCE, 	//　获
//  相机IO控制
    MSG_2A_SET_DCIRIS_AUTO_STATE,   //  设置自动DC光圈使能
    MSG_2A_GET_DCIRIS_AUTO_STATE,   //  读取自动DC光圈使能标志
    MSG_2A_ZOOM_DCIRIS,   		//   放大DC光圈							//qinjj mark
    MSG_2A_SHRINK_DCIRIS,   	//  缩小DC光圈
    MSG_2A_SET_FILTER_STATE,   	//  滤光片切换
    MSG_2A_SET_AC_SYNC_STATE,  	//  设置电网同步使能
    MSG_2A_GET_AC_SYNC_STATE,  	//  读取电网同步使能
    MSG_2A_SET_AC_SYNC_DELAY,  	//  设置电网同步延时
    MSG_2A_GET_AC_SYNC_DELAY,  	//  读取电网同步延时值

	MSG_2A_SET_IO_ARG,			// 设置对外输出IO口参数
    MSG_2A_GET_IO_ARG,			// 读取对外输出IO口参数
		
	MSG_2A_SET_EDGE_MOD,		// 设置触发抓拍沿工作模式
	MSG_2A_GET_EDGE_MOD,		// 读取触发抓拍沿工作模式
	
		//  相机采集功能
    //MSG_2A_GET_H264_RES,       //  读取H264分辨率
    MSG_2A_SET_GRAY_IMAGE_STATE,   //  设置黑白图使能
    MSG_2A_GET_GRAY_IMAGE_STATE,   //  读取黑白图使能标志
		//  相机编码
    //MSG_2A_SET_H264_BITRATE,   //  设置H264码率
    //MSG_2A_GET_H264_BITRATE,   //  读取H264码率
    //MSG_2A_SET_H264_BITRATE2,	//	设置第二路H264码率
    //MSG_2A_GET_H264_BITRATE2,	//	读取第二路H264码率
    //MSG_2A_SET_H264_IFRAME_INTERVAL,    //  设置H264 I帧间隔
    //MSG_2A_GET_H264_IFRAME_INTERVAL,    //  设置H264 I帧间隔
    //MSG_2A_SET_JPEG_QUALITY,   //  设置视频流JPEG压缩率
    //MSG_2A_GET_JPEG_QUALITY,   //  设置视频流JPEG压缩率
    //MSG_2A_SET_CVBS_STD,       //  设置CVBS制式
    //MSG_2A_GET_CVBS_STD,       //  读取CVBS制式
		//  DSP命令
    //MSG_2A_SET_VIDEO_STREAM_RECOGNIZE_PARAMS,  //  视频流识别参数初始化
    //MSG_2A_SET_PICTURE_SNAP_RECOGNIZE_PARAMS,  //  抓拍图识别参数初始化

		//  后门
    MSG_2A_SET_FPGA_REG,
    MSG_2A_SET_FPGA_READ_REG_ADDR,				//todo:由于进程间通信的限制，需要读取寄存器之前通过一条命令设置所要读取的寄存器的地址
    MSG_2A_GET_FPGA_REG,

	//相机工作模式
	MSG_2A_SET_WORKMODE,

	MSG_2A_SOFT_CAPTURE,

	MSG_2A_SET_FLASH_GATE,
	
	MSG_2A_GET_VERSION,
	
	//MSG_2A_SET_CAP_AUTO_PARAM_ENABLE,			//抓拍参数自动化
	//MSG_2A_GET_CAP_AUTO_PARAM_ENABLE,
	MSG_2A_SET_CAP_SHUTTER,
	MSG_2A_SET_CAP_GAIN,
	MSG_2A_SET_CAP_SHARPEN,
	MSG_2A_SET_CAP_RGB,

	MSG_2A_GET_DAY_NIGHT,
	MSG_2A_SET_BACKLIGHT,
	
	MSG_2A_SET_AWB_MODE,

	//MSG_2A_SET_H264_IFRAME_INTERVAL2,
    //MSG_2A_GET_H264_IFRAME_INTERVAL2,
    MSG_2A_SET_RELAY_STATE,    //  继电器开关
    
	MSG_2A_END,
	
	MSG_MAX
};


//消息映射宏定义
//#define SW_BEGIN_MESSAGE_MAP(theClass, baseClass) virtual HRESULT OnMessage(DWORD dwID,WPARAM wParam,LPARAM lParam){HRESULT hr = E_NOTIMPL;if(swpa_strcmp(#theClass, #baseClass)){hr = baseClass::OnMessage(dwID, wParam, lParam);}if(hr == E_NOTIMPL){switch(dwID){
//#define SW_MESSAGE_HANDLER(dwID, func) case dwID : hr = func(wParam,lParam); break;

/* BEGIN: Modified by huangdch, 2015/8/14 
*消息宏定义:处理消息映射和消息类型注册*/
#define SW_BEGIN_MESSAGE_MAP(theClass, baseClass) virtual HRESULT OnMessage(DWORD  mID,WPARAM wParam,LPARAM lParam){HRESULT hr = E_NOTIMPL;if(swpa_strcmp(#theClass, #baseClass)){hr = baseClass::OnMessage( mID, wParam, lParam);}if(hr == E_NOTIMPL){switch( mID){case 0 :
#define SW_MESSAGE_HANDLER(dwID, func) case dwID : if (mID) {hr = func(wParam,lParam);} else {hr=MessageRegister(dwID, #func);} if (mID) break;
#define SW_END_MESSAGE_MAP()default: break;}} return hr;}

/* BEGIN: Modified by huangdch, 2015/8/14 
*如果功能函数不符合OnSet或OnGet开头的规范,就通过下面这个宏的第三个参数来指定,支持MSG_TYPE_SET和MSG_TYPE_GET两个值*/
#define SW_MESSAGE_TYPE_HANDLER(dwID, func, msgType) case dwID : if (mID) {hr = func(wParam,lParam);} else {hr=MessageRegister(dwID, #func, msgType);} if (mID) break;

/* BEGIN: Modified by huangdch, 2015/8/14 
*特别注意:继承类要在构造函数中调用这个宏来完成消息Id类型的注册*/
#define CALL_MESSAGE_REGISTER_FUNCTION() this->OnMessage(0,0,0); this->SharedMemRegister();


//Remote消息映射宏定义
#define SW_BEGIN_REMOTE_MESSAGE_MAP(theClass, baseClass) virtual HRESULT OnRemoteMessage(DWORD dwID, PVOID pvBuffer, INT iSize){HRESULT hr = E_NOTIMPL;if(swpa_strcmp(#theClass, #baseClass)){hr = baseClass::OnRemoteMessage( dwID,  pvBuffer, iSize);}if(hr == E_NOTIMPL){switch(dwID){
#define SW_REMOTE_MESSAGE_HANDLER(dwID, func) case dwID : hr = func(pvBuffer,iSize); break;
#define SW_END_REMOTE_MESSAGE_MAP()default: break;}}return hr;}

/* BEGIN: Modified by 姓名, 2015/8/14 
*共享内存消息节点
*/

#define MSG_SHM_WAIT_RESP_TIME  2000  //等待消息应答的最大超时时间,单位毫秒,-1表示一直等待,这个要根据实际情况进行调整
#define MSG_SHM_UPDATE_TIME 	5     //更新平台内存的时间间隔,单位秒
#define MSG_SHM_DATA_BUF_LEN	1024  //进程间转发消息的最大长度
#define MSG_TYPE_SET  1 			  //消息处理方式:set方式
#define MSG_TYPE_GET  2  			  //消息处理方式:Get方式
#define MSG_SHM_SEM_FULL_START   0   //sem_full信号的起始位置[0],初始值为0
#define MSG_SHM_SEM_EMPTRY_START   1   //sem_full信号的起始位置[1],初始值为 MSG_SHM_NODE_NUMBER
#define MSG_SHM_SEM_FULL_RET_START   2  //sem_full_ret信号的起始位置[2~7],初始值为 0
#define MSG_SHM_SEM_EMPTRY_RET_START  8 //sem_full_ret信号的起始位置[8~13],初始值为 1

#define	MSG_SHM_PROCESS_MUTEX	 10002 
#define	MSG_SHM_REGISTER_KEY	 10003
#define	MSG_SHM_REGISTER_START_KEY     20000

typedef struct _STSharedMemRegisterNode
{
	char szProcName[MSG_SHM_PROCESS_LEN]; //进程名
	DWORD id;  //进程名对应的id
	int iProcessId; //进程id
}STSharedMemRegisterNode;


typedef struct _STSWShmMsg
{
	INT iProcessId;//进程Id
	ULONG lInsertTime;//消息写入的时间,time_t型,实现消息先到先处理,如果是返回非正常应答消息,这里填0
	INT iState;//节点状态 0 表示可写,1表示发送请求,2表示处理请求,3表示返回请求
	DWORD dwId;//消息ID
	DWORD iParamLen;//缓冲区的长度
	CHAR szParamBuf[MSG_SHM_DATA_BUF_LEN];//缓冲区
	INT iRet;//消息处理结果或发送的时候表示是否需要返回值0表示需要,1表示不需要返回值
}STSWShmMsg;

/* BEGIN: Modified by 姓名, 2015/8/14 
*共享内存消息管理类
*/
typedef struct _STSWShmMessage
{
	INT m_KeyId;  //共享内存的key值
	STSWShmMsg  m_shmMsgNode[MSG_SHM_NODE_NUMBER];//共享内存最大的节点数
}STSWShmMessage;

class CSWMessage
{	//消息体结构
	class CMSG : public CSWObject
	{
	public:
		CMSG(DWORD dwID, WPARAM wParam, LPARAM lParam):m_dwID(dwID),m_wParam(wParam),m_lParam(lParam){}
		virtual ~CMSG(){}
		DWORD m_dwID;		 //消息id
		WPARAM  m_wParam;  //消息体附件信息1
		LPARAM  m_lParam;  //消息体附件信息2
	};
public:
	CSWMessage(DWORD dwStartID, DWORD dwEndID);
	virtual ~CSWMessage();

	/**
	 * @brief 非阻塞式发送消息
	 * @param [in] id 消息的id号
	 * @param [in] wParam 消息体参数1
	 * @param [in] lParam 消息体参数2
	 * @return S_OK代表成功，其他值为错误代码
	 */
	static HRESULT PostMessage(DWORD id, WPARAM wParam = 0, LPARAM lParam = 0);
	/**
	 * @brief 阻塞式发送消息
	 * @param [in] id 消息的id号
	 * @param [in] wParam 消息体参数1
	 * @param [in] lParam 消息体参数2
	 * @return S_OK代表成功，其他值为错误代码
	 */
	static HRESULT SendMessage(DWORD id, WPARAM wParam = 0, LPARAM lParam = 0);
	
	static HRESULT SendRemoteMessage(DWORD id, PVOID pvBuffer = NULL, INT iSize = 0);
public:
	/**
	 * @brief 消息接收虚函数
	 * @param [in] wParam 消息体参数1
	 * @param [in] lParam 消息体参数2
	 * @return 成功返回S_OK，其他值代表失败
	 */
	virtual HRESULT OnMessage(DWORD id, WPARAM wParam, LPARAM lParam);

	virtual HRESULT OnRemoteMessage(DWORD id, PVOID pvBuffer, INT iSize);
	
protected:
	/**
	 * @brief 消息处理线程函数
	 * @param [in] pvParam 线程函数
	 */
	static void* OnMSG(void *pvParam);
	
	/**
	 * @brief 跨进程socket消息处理线程
	 * @param [in] pvParam 线程函数
	 */
	static void* OnRemoteMSG(void *pvParam);

protected:

	/*****************************************************************************
	 函 数 名  : CSWMessage.InitShmMsg
	 功能描述  : 共享内存初始化
	 输入参数  : 无
	 输出参数  : 无
	 返 回 值  : HRESULT
	 调用函数  : 
	 注意事项  : 
	 
	 修改历史      :
	  1.日    期   : 2015年9月4日
		作    者   : huangdch
		修改内容   : 新生成函数
	*****************************************************************************/
	HRESULT InitShmMsg();

	/*****************************************************************************
	 函 数 名  : CSWMessage.MessageRegister
	 功能描述  : 消息处理函数注册接口
	 输入参数  : INT id 		   
				 CHAR* szFunName  OnSet开头表示Set消息,OnGet开头表示Get消息   
				 INT msgFlag = -1  自定义消息类型标识,默认根据函数名识别,1表示set消息,2表示get消息
	 输出参数  : 无
	 返 回 值  : virtual
	 调用函数  : 
	 注意事项  : 如果函数名不满足要求,就要传入第三个参数
	 
	 修改历史      :
	  1.日    期   : 2015年9月4日
		作    者   : huangdch
		修改内容   : 新生成函数
	*****************************************************************************/
	virtual HRESULT MessageRegister(INT id, const CHAR* szFunName, INT msgFlag = -1);

	/*****************************************************************************
	 函 数 名  : CSWMessage.SharedMemRegister
	 功能描述  : 共享内存Id注册接口
	 输入参数  : 无
	 输出参数  : 无
	 返 回 值  : virtual
	 调用函数  : 
	 注意事项  : 
	 
	 修改历史      :
	  1.日    期   : 2015年9月4日
		作    者   : huangdch
		修改内容   : 新生成函数
	*****************************************************************************/
	virtual HRESULT SharedMemRegister();

	/*****************************************************************************
	 函 数 名  : CSWMessage.SendProcessMessage
	 功能描述  : 进程间消息通信接口
	 输入参数  : DWORD id 消息id	  
				 DWORD wParam  
				 DWORD lParam  
				 INT& iRet     
	 输出参数  : 无
	 返 回 值  : static
	 调用函数  : 
	 注意事项  : 
	 
	 修改历史      :
	  1.日    期   : 2015年9月4日
		作    者   : huangdch
		修改内容   : 新生成函数
	*****************************************************************************/
	static HRESULT SendProcessMessage(DWORD id, DWORD wParam, DWORD lParam, INT& iRet);

	/*****************************************************************************
	 函 数 名  : CSWMessage.FindKey
	 功能描述  : 根据消息Id查找对应的共享内存key值
	 输入参数  : DWORD id  
	 输出参数  : 无
	 返 回 值  : HRESULT -1表示查找失败 其它值成功
	 调用函数  : 
	 注意事项  : 
	 
	 修改历史      :
	  1.日    期   : 2015年9月4日
		作    者   : huangdch
		修改内容   : 新生成函数
	*****************************************************************************/
	static HRESULT FindKey(DWORD id);

	/*****************************************************************************
	 函 数 名  : CSWMessage.SingleSend
	 功能描述  : 向指定进程发送消息
	 输入参数  : INT iKeyId    
				 DWORD id      
				 DWORD wParam  
				 DWORD lParam  
				 INT result    
	 输出参数  : 无
	 返 回 值  : HRESULT 0成功 非0失败
	 调用函数  : 
	 注意事项  : 
	 
	 修改历史      :
	  1.日    期   : 2015年9月4日
		作    者   : huangdch
		修改内容   : 新生成函数
	*****************************************************************************/
	static HRESULT SingleSend(INT iKeyId, DWORD id, DWORD wParam, DWORD lParam, INT &result);

	/*****************************************************************************
	 函 数 名  : CSWMessage.MassSend
	 功能描述  : 向平台内所有的进程发送消息
	 输入参数  : INT& result  
	 输出参数  : 无
	 返 回 值  : HRESULT 0成功 非0失败
	 调用函数  : 
	 注意事项  : 
	 
	 修改历史      :
	  1.日    期   : 2015年9月4日
		作    者   : huangdch
		修改内容   : 新生成函数
	*****************************************************************************/
	static HRESULT MassSend(INT& result);

	/*****************************************************************************
	 函 数 名  : CSWMessage.AnalyResponse
	 功能描述  : 解析广播应答的消息
	 输入参数  : CHAR* szResult MassSend()返回的消息  
	 输出参数  : 无
	 返 回 值  : static
	 调用函数  : 
	 注意事项  : 
	 
	 修改历史      :
	  1.日    期   : 2015年9月4日
		作    者   : huangdch
		修改内容   : 新生成函数
	*****************************************************************************/
	static HRESULT AnalyResponse(CHAR* szResult);

	/*****************************************************************************
	 函 数 名  : CSWMessage.FindProcByKeyId
	 功能描述  : 根据KeyId查找相关进程的信息
	 输入参数  : const int KeyId	 

	 输出参数  : 无
	 返 回 值  : HRESULT <0失败 其它值相应数组的下标
	 调用函数  : 
	 注意事项  : 
	 
	 修改历史      :
	  1.日    期   : 2015年9月4日
		作    者   : huangdch
		修改内容   : 新生成函数
	*****************************************************************************/
	static HRESULT FindProcByKeyId(const DWORD dwKeyId);

	/*****************************************************************************
	 函 数 名  : CSWMessage.UpdateProcByKeyId
	 功能描述  : 根据id从注册区强制更新平台的信息
	 输入参数  : const int KeyId
	 			 int flag  < 0 表示不再查一次注册区 >=0 表示注册区对应的数组下标
	 输出参数  : 无
	 返 回 值  : static
	 调用函数  : 
	 注意事项  : 
	 
	 修改历史      :
	  1.日    期   : 2015年9月5日
	    作    者   : huangdch
	    修改内容   : 新生成函数

	*****************************************************************************/
	static HRESULT UpdateProcByKeyId(const DWORD dwKeyId, INT iFlag = -1);

	/*****************************************************************************
	 函 数 名  : CSWMessage.OnProMSG
	 功能描述  : 进程间消息处理线程
	 输入参数  : void* pvParam  
	 输出参数  : 无
	 返 回 值  : static
	 调用函数  : 
	 注意事项  : 
	 
	 修改历史      :
	  1.日    期   : 2015年9月4日
		作    者   : huangdch
		修改内容   : 新生成函数
	*****************************************************************************/
	static void* OnProMSG(void* pvParam);
	
private:
	static CSWThread* m_pMsgThread;
	static CSWThread* m_pRemoteMsgThread;	//处理跨进程socket消息
	static CSWObjectList m_objMSGList;

	static CSWList<CSWMessage *>m_clsMSGList;
	static CSWRWMutex m_rwLock;
	DWORD m_dwStartID;
	DWORD m_dwEndID;
	
	static CSWUDPSocket m_udpSocket;

private:
	/* BEGIN: Modified by huangdch, 2015/8/14 
	*进程间通信用到的相关变量
	*/
	static CSWMutex m_tMsgMutex; //进程消息注册锁
	static STSharedMemRegisterNode* m_SharedSharedMemRegister; //共享内存注册信息

	static CSWList<STMessageRangNode*>m_MSGRangList;//其它进程消息id对应的key范围,再根据范围到vec_prcess查找出对应的共享内存连接指针

	static STSWShmMessage* m_pShm; //共享内存数据交换区
	static STProcessManageNode* m_ProcessManageNode; //当前平台的所有进程相关的信息

	//当前进程相关的信息
	static CSWMutex tMutex; 	 //进程间互斥锁
	static CSWSemaphore m_ArySem[MSG_SHM_SEM_TOTAIL];//进程间信号量
	static CSWRWMutex m_rwProcLock;  //当前进程更新和查询读写锁
	static STProcessManageCur m_ProcessManageCur; //当前进程节点

	static CSWThread* m_pMsgProcThread; //专门再搞一个线程来处理进程间的消息

	static char m_ProcessName[MSG_SHM_PROCESS_LEN]; //当前平台的进程名

};
#endif // !defined(EA_3172651B_B5B4_4ca8_8467_58A8FE88DC5E__INCLUDED_)


