#ifndef _CAMYU_LINK_OPT_H_
#define _CAMYU_LINK_OPT_H_
#include <swbasetype.h>
// 传输端口号
#define CY_DATA_PORT 8888
// 控制端口号
#define CY_CMD_PORT 8886
// 心跳应答
#define CY_THROB_ACK 0x55AA55AA

#define CY_CAMERA_INFO_LEN 0x160    /* 参数区长度 */

/* 
摄像机参数号定义，如果注释//后*号表示此参数号对网络摄像机无效
针对J系列: DC-1335J DC-2035J 
*/

//大华摄像机码流类型定义
#define DH_FRAME_I 10	// I帧
#define DH_FRAME_P 11	// P帧


//厂商信息区  (0x00--------0x1f)
#define  CY_ID   			0x00 	//设备码(0--无设备，!0--有设备)
#define  CY_WIDTH			0x01	//传感器水平像素数(宽度)
#define  CY_HEIGHT			0x02    //传感器垂直像素数(宽度)
#define  CY_BITS			0x03 	//图像数据的A/D位数	
#define  CY_COLOR			0x04    //图像数据格式(0--灰度，1--Bayer_RG，2--Bayer_GR，3--Bayer_BG，5--RGB，6--YUV422，7--JPEG)
#define  CY_HEART_BEAT     0x0F	//*保留*

#define  CY_SOFT_VERSION  	0x12	//固件程序版本
#define  CY_DEVICE_NO     	0x13    //摄像机编号

//单位信息区  (0x20--------0x2f)
#define  CY_GAIN_UNIT     	0x20	//当前增益值
#define  CY_OFFSET_UNIT    0x21	//当前偏置值
#define  CY_FREQ_UNIT    	0x22	//当前帧频值
#define  CY_TIME_UNIT     	0x23	//当前曝光时间值


//参数控制范围信息区  (0x30--------0x4f)
#define  CY_MAX_GAIN		0x30	//增益最大值
#define  CY_MIN_GAIN		0x31	//增益最小值
#define  CY_MAX_OFFSET		0x32	//偏置最大值
#define  CY_MIN_OFFSET		0x33	//偏置最小值
#define  CY_MAX_FREQ       0x34	//帧频最大值
#define  CY_MIN_FREQ       0x35	//帧频最小值
#define  CY_MAX_TIME       0x36	//曝光时间最大值
#define  CY_MIN_TIME       0x37	//曝光时间最小值
#define  CY_MAX_BALANCEMODE  0x38	//白平衡方式选择最大值
#define  CY_MAX_AGCLIMIT   0x39	//AGC门限值最大值
#define  CY_MAX_AGCSELECT  0x3a	//AGC取样区域选择最大值
//---- 只读信息区End -----

#define  CY_PROTECT_AREA   0x8f  // 0--0x13f区域为命令区，可以任意设置  

//---- 2. 可编程参数名宏定义(读/写) -----
//获得参数：参数值 = CYUSB_ReadCameraParam(参数名);
//修改参数：CYUSB_UpdateCameraParam(参数名,参数值);
#define  CY_GAIN      		0x90	//当前增益值
#define  CY_OFFSET      	0x91	//当前偏置值
#define  CY_FREQ      		0x92	//当前帧频值
#define  CY_TIME      		0x93	//当前曝光时间值
#define  CY_SYNC			0x94	//同步方式(0--外触发，1--单次触发)
//预留n个命令位置.......

//add by黄国超，海康相机抓拍命令,2010-11-09
#define  TRI_EXP_TIME   0x103 // 监控模式有效(抓拍时采用的曝光时间)
#define  TRI_GAIN       0x104 // 监控模式有效(抓拍时采用的增益)
//end added


#define  CY_AGCMODE		0xb0 //增益控制方式(1--自动(AGC)，0--手动)
#define  CY_AGCLIMIT		0xb1 //AGC门限值 (0-255)
#define  CY_AGCSELECT      0xb2 //AGC取样区域选择 低16位有效，分别对应4x4的16个区域，顺序为从左到右，从上到下
#define  CY_AGCTIME        0xb3 //AGC与电子快门连动
#define  CY_AGC_GAIN_MAX   0xb4 //AGC调整范围 0 ~ Max dB
#define  CY_AGC_TIME_MIN   0xb5 //AGC与电子快门连动时，电子快门调整最小值(us)
#define  CY_AGC_TIME_MAX   0xb6 //AGC与电子快门连动时，电子快门调整最大值(us)
//预留n个命令位置.......


#define  CY_GAIN_R         0xc9 //当前R增益值
#define  CY_GAIN_G			0xca //当前G增益值
#define  CY_GAIN_B			0xcb //当前B增益值
#define  CY_BALANCEMODE    0xd2 //白平衡校正方式选择 0--手动白平衡 1--自动白平衡.
//预留n个命令位置.......

#define  CY_AUTOGRAY		0xe0 //*自动灰度增强(1--自动，0--手动)
#define  CY_GRID	  		0xe1 //像素抽点值 (不抽点，1/2抽点)
#define  CY_GRID_1B1  0 //不抽点
#define  CY_GRID_1B2  1 //1/2抽点

//预留n个命令位置.......

#define  CY_SOFT_TRIGGER	0xf0// 软件触发命令 1-主通道抓拍补光 0-副通道抓拍补光
#define  CY_TEST_IMAGE     0xf1 // 测试图形选择 1-测试图像 0-正常图像
#define  CY_TIME_STAMP 	0xf2 // 对时命令，参数为标准相差秒
//预留n个命令位置.......


#define  CY_SERVER_IP      0x100 // 服务器IP(监控模式有效)
#define  CY_SERVER_PORT    0x101 // 服务器PORT(监控模式有效)
#define  CY_TRI_JPEG_Q     0x102 // 抓拍时JPEG质量(监控模式有效)

/* 网络摄像机特有 */
#define  CY_IP_ADDR		0x120 // 摄像机IP地址
#define  CY_GATEWAY		0x121 // 摄像机网关
#define  CY_NETMASK		0x122 // 网络地址掩码
#define  CY_CTRL_PORT      0x123 // 控制端口
#define  CY_TRAN_PORT	    0x124 // 传输端口

#define  CY_TF_COUNT		0x125 // 连续抓拍帧数 (1-3)
#define  CY_JPEG_SET		0x126 // JPEG压缩品质 (0-100) 0--最差 100--最优
#define  CY_EXP_OUT_EN     0x127 // 曝光同步输出( d0--抓拍补光灯 d1--频闪 )
#define  CY_TRIG_SET		0x128 // 触发帧间隔 0－连续触发 1，2－间隔n帧输出

#define  CY_RADAR_SET_0	0x12B // 设置获得雷达数据超时参数(单位ms)
#define  CY_RADAR_SET_1	0x12C // *预留	
#define  CY_RADAR_SET_2	0x12D // 测速雷达串口同步头字节定义，32B参数分成4个字节，可以使用4种同步字节
// 摄像机在收到抓拍触发后，接收RS232数据，如果接收的RS232数据字节与4个同步字节
// 的任意一个匹配时，记录其后的n个，并嵌入帧信息头回传
#define  CY_RADAR_SET_3	0x12E // 设置串口波特率
#define  CY_RADAR_SET_4	0x12F // 设置同步字后信息长度 1~4字节

#define  CY_YEAR			0x130
#define  CY_MONTH		    0x131
#define  CY_DAY			0x132
#define  CY_HOUR			0x133
#define  CY_MINUTE		    0x134
#define  CY_SECOND		    0x135 // 写入秒后更新系统时间 

#define  CY_TRIGGER_DELAY  0x136 // 抓拍延时(us)
#define  CY_EXP_DELAY      0x137 // 曝光延时(us)
#define  CY_SELECT_IMAGE   0x13F // 选图命令
#define  CY_USER_INFO      0x140 // 0x140~0x14F 用户信息 64Byte


/* 摄像机命令号 */
#define  CY_SOFT_RESET		0xFF0002 // 软复位命令
#define  CY_SYNC_PARAM		0xFF0003 // 同步参数命令
#define  CY_SAVE_PARAM		0xFF0004 // 保存参数到摄像机

/*****************************************************************************/
/*                             图像帧头信息                                  */
/*****************************************************************************/
typedef struct
{
	WORD16 wWidth;       // 图像宽
	WORD16 wHeight;       // 图像高

	WORD16 wBits;      // 图像位数 8~16bits
	WORD16 wFormat;      // 图像类型代码(0--灰度，1--Bayer_RG，2--Bayer_GR，3--Bayer_BG，5--RGB，6--YUV422，7--JPEG)

	WORD16 wFrameType; // 帧类型(0--普通，1--抓拍图像，2--心跳帧，10--大华码流I帧，11--大华码流P帧)
	WORD16 wFrameRev;  // 保留

	DWORD32  dwFirmwareVersion; //固件程序版本
	DWORD32  dwDeviceNo; // 设备编号

	DWORD32  dwLen;     // 图像数据长度

	DWORD32  dwSpeed;   // 双线圈测速值(us)
	DWORD32  dwRs232;   // 串口信息(1~4字节)	

	WORD16 wYear; // 图像采集时间
	WORD16 wMonth;
	WORD16 wDay;
	WORD16 wHour;
	WORD16 wMinute;
	WORD16 wSecond;

	DWORD32 dwIp;		// 采集当前帧的摄像机IP
	DWORD32 dwFrameCount;	//总共的抓拍帧数目
	DWORD32 dwTriggerCount; //总共的触发数
	DWORD32 dwTriggerIndex; //触发组索引
	DWORD32 dwFrameNo; //帧号

	DWORD32 dwGain; //当前抓拍参数
	DWORD32 dwTime; //曝光时间
	DWORD32 dwGainR; //红增益
	DWORD32 dwGainG; //绿增益
	DWORD32 dwGainB; //蓝增益

	DWORD32 dwMode;  // 摄像机工作模式
	DWORD32 dwJpegQ; // JPEG压缩品质
	DWORD32 dwTd1;   // 抓拍延时(单位us)
	DWORD32 dwTd2;   // 曝光延时(单位us)

	DWORD32 dwtrig_chl; //触发通道
	DWORD32 dwmsecond; //ms计时
	DWORD32 dwyavg; //平均亮度
	DWORD32 dwmpeg_head;//MPEG4码流头标志
	DWORD32 dwSoftParam;//软触发参数
	DWORD32 dwrev[7]; //保留参数

	BYTE8 rgbUserInfo[64]; // 用户数据
} CY_FRAME_HEADER; // 帧头

#endif
