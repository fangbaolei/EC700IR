#ifndef _HV_OPT_H
#define _HV_OPT_H

#include "hv_opt_base.h"

//协议命令类型
#define GET_TIME_COMMAND			0x0101
#define SET_TIME_COMMAND			0x0102
#define GET_ID_COMMAND				0x0103
#define SET_NAME_COMMAND			0x0104
#define GET_MODE_COMMAND			0x0105
#define SET_MODE_COMMAND			0x0106
#define SET_IP_COMMAND				0x0107
#define SET_FILTER_COMMAND			0x0109
#define GET_VER_COMMAND				0x010A
#define RESET_COMMAND				0x010B
#define GET_VIDEO_NAME_COMMAND		0x010C
#define SET_VIDEO_NAME_COMMAND		0x010D
#define SET_OPTION_COMMAND			0x010F
#define GET_OPTION_COMMAND			0x0110
#define SEND_COM_COMMAND			0x0111
#define RECV_COM_COMMAND			0x0112
#define ERASE_FLASH_COMMAND			0x0113
#define WRITE_FLASH_COMMAND			0x0114
#define READ_FLASH_COMMAND			0x0115
#define GETHVFILESYSTEMINFO_COMMAND 0x0116
#define GETHVFILEINFO_COMMAND		0x0117
#define PUTHVFILE_COMMAND			0x0118
#define FORMATFS_COMMAND			0x0119
#define PACKFS_COMMAND				0x011A
#define RANDOM_WRITE_FLASH_COMMAND	0x011B
#define RANDOM_READ_FLASH_COMMAND	0x011C
#define GET_TESTINFO_COMMAND		0x011D
#define CLEAR_TESTINFO_COMMAND		0x011E
#define FORCE_SEND_COMMAND			0x011F
#define GET_INIFILE_COMMAND			0x0120
#define SET_INIFILE_COMMAND			0x0121
#define TEST_JEPG_COMMAND			0x0122
#define GET_RESETREPORT_COMMAND 	0x0123
#define TEST_PERFORMANCE_COMMAND	0x0124
#define TEST_SYSKEY_COMMAND			0x0125
#define GET_FILELIST_COMMAND		0x0126
#define GET_PARAMDAT_COMMAND		0x0127
#define SET_PARAMDAT_COMMAND		0x0128
#define GET_CONNECTED_IP			0x0129
#define AT88_INIT_CONFIG			0x012A
#define AT88_WRITE_FUSE				0x012B
#define SET_DEFAULT_CONFIG			0x012C
#define PROCESS_PHOTO_COMMAND 		0x012D
#define SET_PARAM_COMMAND			0x012E
#define SET_SAFEMODE_COMMAND		0x012F
#define GET_VERSION_COMMAND			0x0130
#define UPDATE_IPT_COMMAND			0x0131
#define GET_STAT_INFO_COMMAND		0x0132
#define CHECK_IDENTIFYPASSWORD_COMMAND 0x0133
// AVI文件使用的协议
#define GET_AVI_COMMAND				0x0140
//高清DSP硬盘录像
#define GET_HISTORY_VIDEO_COMMAND 0x0141
#define GET_DETECT_INFO_COMMAND		0x0150

// 上海交计一体化机独有协议
#define GET_JIAOJI_RECO_DEV_COUNT_COMMAND		0x2000
#define GET_JIAOJI_RECO_DEV_STATE_COMMAND		0x2001
#define GET_JIAOJI_RESULT_DATA_COMMAND			0x2002


#define THROB_COMMAND				0x01FF
#define UNKNOW_COMMAND				0x1000

//参数编码
const unsigned int HV_OPT_DEFAULT = 0x00010000;			//设置系统缺省选项，无参数，pParam为NULL, pdwParamSize为0。
const unsigned int HV_OPT_RESETCOUNT = 0x00020000;		//读取系统复位次数，pParam指向的参数为4字节无符号整数。
const unsigned int HV_OPT_FONTMODE = 0x00030000;		    //设置和读取系统字符集选项，pParam指向的参数为4字节无符号整数。
const unsigned int HV_OPT_WORKMODE = 0x00040000;		    //设置和读取系统工作模式选项，pParam指向的参数为4字节无符号整数。
const unsigned int HV_OPT_SYSLED = 0x00050000;			//设置和读取系统指示灯情况, pParam指向的参数为4字节无符号数。
														//参数的第12、13、14、15位分别表示系统的四个指示灯。
const unsigned int HV_OPT_RTCCLK = 0x00060000;			//设置实时时钟芯片中断输出时钟周期,pParam指向的参数为4字节无符号整数，
														//0为1Hz, 1为4KHz, 2为8KHz, 3为32KHz。

const unsigned int HV_OPT_FONTMODE_COUNT = 0x00070000;	//读取系统字符集模式数量，pParam指向的参数为4字节无符号整数。
const unsigned int HV_OPT_FONTMODE_INDEX = 0x00080000;	//读取指定编号字符集的名称，编号由GetHvOpt函数的dwOptID参数的低16位指出，
														//pParam指向的参数为不超过265字节的字符串，包括'\0'。

const unsigned int HV_OPT_WORKMODE_COUNT = 0x00090000;	//读取系统工作模式数量，pParam指向的参数为4字节的无符号整数。
const unsigned int HV_OPT_WORKMODE_INDEX = 0x000A0000;	//读取指定编号的工作模式名称，编号由GetHvOpt函数的dwOptID参数的低16位指出，
														//pParam指向的参数为不超过265字节的字符串，包括'\0'。

const unsigned int HV_OPT_RESETMODE = 0x000B0000;		//读取系统复位模式，pParam指向的参数为4字节的无符号整数。

const unsigned int HV_OPT_FILELIST = 0x000C0000;		    //读取指定类型的文件列表及校验信息
const unsigned int HV_OPT_PRODUCTNAME = 0x000D0000;	    //读取设备编号
const unsigned int HV_OPT_PARAM_DEFAULT = 0x000E0000;    //重置参数
const unsigned int HV_OPT_MISC = 0x000F0000;	            //读取其他相关信息

//最大视频流个数
const unsigned short MAX_VIDEO_COUNT = 16;
//设备支持的最多连接数
const unsigned short MAX_CONNECT_COUNT = 4;
// 设备支持的最多连接组数（一个命令连接+一个数据连接算一组）
const int MAX_CONNECT_TEAM_COUNT = 2;

//视频处理系统的SOCKET命令端口
const unsigned short HV_COMMAND_PORT = 6666;
//视频处理系统的SOCKET数据端口
const unsigned short HV_DATA_PORT = 6667;
//视频处理系统的SOCKET命令端口2
const unsigned short HV_COMMAND_2_PORT = 6668;
//图片处理系统的SOCKET命令端口
const unsigned short HPR_PROCESS_PORT = 6680;

//视频处理系统名字最大长度
#define MAX_HV_NAME_LEN		256

//数据流类型定义
#define _TYPE_THROB				 0x8000		   // 心跳数据，没有数据要发送的时候每一秒钟发送一次

#define  _TYPE_CAR_IN_AND_LEFT   0x8001        //  该数据所承载的信息流是"车辆到达和离开信号"
#define  _TYPE_CAR_CMP_IMAGE     0x8007        //  该数据所承载的信息流是"有车时的压缩视频流"（暂不支持）

#define  _TYPE_TRACE_STR         0x8101        //  该数据所承载的信息流是"与跟踪有关的调试字符流"
#define  _TYPE_DIVI_STR          0x8103        //  该数据所承载的信息流是"与分割有关的调试字符流"
#define  _TYPE_RECOG_STR         0x8104        //  该数据所承载的信息流是"与识别有关的调试字符流"
#define  _TYPE_ERROR_STR         0x8106        //  该数据所承载的信息流是"与错误有关的调试字符流"
#define  _TYPE_OTHER_STR         0x8107        //  该数据所承载的信息流是"其他调试字符流"

#define  _TYPE_TRACE_BIN         0x8108        //  该数据所承载的信息流是"与跟踪有关的调试二进制流"
#define  _TYPE_DETECT_BIN        0x810A        //  该数据所承载的信息流是"与检测有关的调试二进制流"
#define  _TYPE_DIVI_BIN          0x810B        //  该数据所承载的信息流是"与分割有关的调试二进制流"
#define  _TYPE_RECOG_BIN         0x810C        //  该数据所承载的信息流是"与识别有关的调试二进制流"

// 所连接到设备的IP信息定义
#define TCPIP_CONNECT	0
#define TELNET_CONNECT  1
#define TESTSPD_CONNECT	2

//身份验证用常量
#ifndef MAX_IDENTIFY_PASSWORD_LENGTH
#define MAX_IDENTIFY_PASSWORD_LENGTH 256
#endif

#endif
