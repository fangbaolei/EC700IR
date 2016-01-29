//
//                 CAMYU CORPORATION
//  This file is supplied for Camyu Network Digital Camera 
//
//  File:
//    CYNET_API.h
//
//  Purpose:
//
//  Version:
//    1.2.2
//
#if (RUN_PLATFORM == PLATFORM_WINDOWS)
#define CAMERA_INFO_LEN 0x160    /* 参数区长度 */

/* 
 摄像机参数号定义，如果注释//后*号表示此参数号对网络摄像机无效
 */

//厂商信息区  (0x00--------0x1f)
#define  ID   			0x00 	//设备码(0--无设备，!0--有设备)
#define  WIDTH			0x01	//传感器水平像素数(宽度)
#define  HEIGHT			0x02    //传感器垂直像素数(宽度)
#define  BITS			0x03 	//图像数据的A/D位数	
#define  COLOR			0x04    //图像数据格式(0--灰度，1--Bayer_RG，2--Bayer_GR，3--Bayer_BG，5--RGB，6--YUV422)
#define  HEART_BEAT     0x0F	//*保留*

#define  SOFT_VERSION  	0x12	//固件程序版本

//单位信息区  (0x20--------0x2f)
#define  GAIN_UNIT     	0x20	//当前增益值
#define  OFFSET_UNIT    0x21	//当前偏置值
#define  FREQ_UNIT    	0x22	//当前帧频值
#define  TIME_UNIT     	0x23	//当前曝光时间值


//参数控制范围信息区  (0x30--------0x4f)
#define  MAX_GAIN		0x30	//增益最大值
#define  MIN_GAIN		0x31	//增益最小值
#define  MAX_OFFSET		0x32	//偏置最大值
#define  MIN_OFFSET		0x33	//偏置最小值
#define  MAX_FREQ       0x34	//帧频最大值
#define  MIN_FREQ       0x35	//帧频最小值
#define  MAX_TIME       0x36	//曝光时间最大值
#define  MIN_TIME       0x37	//曝光时间最小值
#define  MAX_BALANCEMODE  0x38	//白平衡方式选择最大值
#define  MAX_AGCLIMIT   0x39	//AGC门限值最大值
#define  MAX_AGCSELECT  0x3a	//AGC取样区域选择最大值
//---- 只读信息区End -----

#define  PROTECT_AREA   0x8f  // 0--0x13f区域为命令区，可以任意设置  

//---- 2. 可编程参数名宏定义(读/写) -----
//获得参数：参数值 = CYUSB_ReadCameraParam(参数名);
//修改参数：CYUSB_UpdateCameraParam(参数名,参数值);
#define  GAIN      		0x90	//当前增益值
#define  OFFSET      	0x91	//当前偏置值
#define  FREQ      		0x92	//当前帧频值
#define  TIME      		0x93	//当前曝光时间值
#define  SYNC			0x94	//同步方式(0--外触发，1--单次触发)
//预留n个命令位置.......


#define  AGCMODE		0xb0	//增益控制方式(1--自动(AGC)，0--手动)
#define  AGCLIMIT		0xb1	//AGC门限值
#define  AGCSELECT      0xb2    //AGC取样区域选择
#define  AGCTIME		0xb3	//AGC联动电子快门(1--开,0--关)
//预留n个命令位置.......

#define  GAIN_R       	0xc9	//当前R增益值
#define  GAIN_G			0xca	//当前G增益值
#define  GAIN_B			0xcb	//当前B增益值
#define  BALANCEMODE    0xd2    //白平衡校正方式选择 0--禁止白平衡  1---单次白平衡  2---自动白平衡测量.
//预留n个命令位置.......

#define  AUTOGRAY		0xe0	//自动灰度增强(1--自动，0--手动)
#define  GRID	  		0xe1	//像素抽点值 (不抽点，1/2抽点，1/4抽点)
  #define GRID_1B1  0 //不抽点
  #define GRID_1B2  1 //1/2抽点
  #define GRID_1B4  2 //1/4抽点
                                      
//预留n个命令位置.......


/* 网络摄像机特有 */
#define  IP_ADDR		0x120	// 摄像机IP地址
#define  GATEWAY		0x121	// 摄像机网关
#define	 NETMASK		0x122	// 网络地址掩码
#define  CTRL_PORT      0x123   // 控制端口
#define  TRAN_PORT		0x124	// 传输端口
#define	 SERVER_IP		0x125	// 服务器IP地址
#define  SERVER_PORT	0x126   // 触发传输端口(0 不抓拍)     
#define  TF_COUNT		0x127   // Trigger Frame Count 连续抓拍帧数
#define  JPEG_SET		0x128   // *最高位(0--不压缩，1--压缩)，低16位表示JPEG压缩品质
#define  TRIG_SET		0x129	// 触发信号消颤，单位us( 0us ~ 65ms ) 		
								// 曝光同步输出( 0--连续输出 1--有效帧时输出 )
								// 触发帧间隔 0－连续触发 1，2，3－间隔n帧输出
#define  AI_LEVEL		0x12A	// *Auto-Iris Level 自动光圈等级
#define  RADAR_SET_0	0x12B   // 设置获得雷达数据超时参数，步距20ms，单次触发模式时有效
#define  RADAR_SET_1	0x12C	// 预留	
#define  RADAR_SET_2	0x12D	// 测速雷达串口同步头字节定义，32B参数分成4个字节，可以使用4种同步字节
                                // 摄像机在收到抓拍触发后，接收RS232数据，如果接收的RS232数据字节与4个同步字节
                                // 的任意一个匹配时，记录其后的2个，并嵌入帧信息头回传

#define  CYNET_YEAR			0x130
#define  CYNET_MONTH			0x131
#define  CYNET_DAY			0x132
#define  CYNET_HOUR			0x133
#define  CYNET_MINUTE			0x134
#define  CYNET_SECOND			0x135   // 写入秒后更新系统时间 

/* 摄像机命令号 */
#define  UPGRADE_FIRMWARE	0xFF0001 // 升级固件命令
#define  SOFT_RESET		0xFF0002 // 软复位命令
#define  SYNC_PARAM		0xFF0003 // 同步参数命令
#define  SAVE_PARAM		0xFF0004 // 保存参数到摄像机

/* 返回值 */
#define RET_ERR	0
#define RET_OK	1

/* SDK API */
extern "C" {
/*****************************************************************************/
/*                             初始化库                                      */
/* 参数: 无                                                                  */
/*****************************************************************************/
void __declspec(dllexport) CYNET_Open(void);


/*****************************************************************************/
/*                        释放所有资源，关闭库                               */
/* 参数: 无                                                                  */
/*****************************************************************************/
void __declspec(dllexport) CYNET_Close(void);

/*****************************************************************************/
/* 对管理的所有摄像机发送心跳包命令                                          */
/* $摄像机的控制连接在5秒内没有收到命令，则自动断开连接                      */
/* $如果要长时间保持控制连接，定时调用该函数                                 */
/* 参数: 无                                                                  */
/*****************************************************************************/
void __declspec(dllexport) CYNET_SendHeartBeatPackage(void);

/*****************************************************************************/
/*                        添加指定IP摄像机                                   */
/* 参数: ip    准备添加到库的摄像机的IP地址                                  */
/*       portc 准备添加到库的摄像机的控制端口                                */
/*       ports 准备添加到库的摄像机的传输端口                                */
/*****************************************************************************/
int __declspec(dllexport) CYNET_AddCamera(char *ip, USHORT portc, USHORT ports);


/*****************************************************************************/
/*                        删除指定IP摄像机                                   */
/* 参数: ip    准备从库中删除的摄像机的IP地址                                */
/*****************************************************************************/
int __declspec(dllexport) CYNET_DelCamera(char *ip);


/*****************************************************************************/
/*             选中指定IP摄像机，以后操作都正对当前选中的摄像机              */
/* 参数: ip    要选中的摄像机的IP地址                                        */
/*****************************************************************************/
int __declspec(dllexport) CYNET_SelectCamera(char *ip);


/*****************************************************************************/
/*                       选中摄像机采集开始/停止                             */
/* 参数: bCapStart    TRUE  开始采集                                         */
/*                    FALSE 停止采集                                         */
/*****************************************************************************/
int __declspec(dllexport) CYNET_CapStart(BOOL bCapStart);


/*****************************************************************************/
/*                       等待获得摄像机采集图象                              */
/* 参数: pData    存放数据内存区，要足够存放一帧BAYER数据                    */
/*       pLen     存放采集数据的长度                                         */
/*****************************************************************************/
int __declspec(dllexport) CYNET_GetFrame(char *pData,int *pLen,char *ip=NULL);


/*****************************************************************************/
/*                     选中摄像机控制通道打开/关闭                           */
/* 参数: bCtrlStart   TRUE  打开控制通道                                     */
/*                    FALSE 关闭控制通道                                     */
/*****************************************************************************/
int __declspec(dllexport) CYNET_CtrlStart(BOOL bCtrlStart);


/*****************************************************************************/
/*                         读取摄像机参数                                    */
/* 参数: nName   参数定义号                                                  */
/* 返回: 读取的参数值                                                        */
/*****************************************************************************/
ULONG __declspec(dllexport) CYNET_ReadCameraParam(int nName);


/*****************************************************************************/
/*                         设置摄像机参数                                    */
/* 参数: nName   参数定义号                                                  */
/*       nParam  设置参数1                                                   */
/*       nParam  设置参数2                                                   */
/*****************************************************************************/
int __declspec(dllexport) CYNET_UpdateCameraParam(int nName, ULONG nParam,ULONG nParam2=0);


/*****************************************************************************/
/*                         发送摄像机命令                                    */
/* 参数: nName   命令定义号                                                  */
/*       nBuf    命令数据缓冲区指针                                          */
/*****************************************************************************/
int __declspec(dllexport) CYNET_SendCommand(int nCmd, char *nBuf=NULL);

/*****************************************************************************/
/*                         得到基本参数的单位                                */
/* 注:   实际值 = 参数值 x 单位                                              */ 
/*       获取增益单位时，返回的nB如果是0，表示dB，1表示倍率                  */
/*****************************************************************************/
double __declspec(dllexport) CYNET_GetTimeUnit(); //单位毫秒
double __declspec(dllexport) CYNET_GetFrameUnit();//单位帧/秒
double __declspec(dllexport) CYNET_GetGainUnit(int *nB); //单位nB (0 - dB 1 - 倍率)
int	   __declspec(dllexport) CYNET_GetOffsetUnit(); //单位灰度值


/*****************************************************************************/
/*                 标准彩色重建(Bayer转换到RGB)                              */
/* 参数: pRGB    用户分配用来存放转换后的RGB图像数据，RGB数据量是Bayer的3倍，*/
/*               注意分配足够的内存区                                        */
/*       pBayer  指向Bayer格式图像的缓存区                                   */
/*       w       图像宽                                                      */
/*       h       图像高                                                      */
/*****************************************************************************/
void __declspec(dllexport) CYNET_BayerToRGB(BYTE* pRGB,BYTE* pBayer,int w,int h);
void __declspec(dllexport) CYNET_BayerToRGB_12bit(USHORT* pRGB,USHORT* pBayer,int w,int h);

/*****************************************************************************/
/*                 高质量彩色重建(Bayer转换到RGB)                            */
/* 参数: pRGB    用户分配用来存放转换后的RGB图像数据，RGB数据量是Bayer的3倍，*/
/*               注意分配足够的内存区                                        */
/*       pBayer  指向Bayer格式图像的缓存区                                   */
/*       w       图像宽                                                      */
/*       h       图像高                                                      */
/*****************************************************************************/
void __declspec(dllexport) CYNET_BayerToRGB_HQI(BYTE* pRGB,BYTE* pBayer,int w,int h);
void __declspec(dllexport) CYNET_BayerToRGB_HQI_12bit(USHORT* pRGB,USHORT* pBayer,int w,int h);

/*****************************************************************************/
/*                             软件白平衡校正                                */
/* 参数: pRGB    指向RGB图像数据                                             */
/*       nW      图像宽                                                      */
/*       nH      图像高                                                      */
/*****************************************************************************/
void __declspec(dllexport) CYNET_WhiteBalance(unsigned char *pRGB,int nW,int nH);
void __declspec(dllexport) CYNET_WhiteBalance_12bit(unsigned short *pRGB,int nW,int nH);

/*****************************************************************************/
/*                             解码JPEG文件                                  */
/* 参数: lpSrcBuffer  指向JPEG数据源                                         */
/*       dwSrcSize    数据源长度                                             */
/*               注意分配足够的内存区                                        */
/*       lpDstBuffer  指向目定内存                                           */
/*       dwDstSize    用于获得解码后的图象数据长度                           */
/*       lpdwWidth    获得图像宽                                             */
/*       lpdwHeight   获得图像高                                             */
/*       lpBits       获得图像数据位数                                       */
/*       dwReserve1   获得一行图像数据的长度                                 */
/*****************************************************************************/
BOOL __declspec(dllexport) CYNET_JpegDecode(BYTE *lpSrcBuffer, DWORD dwSrcSize,
					                        BYTE *lpDstBuffer, DWORD *dwDstSize,
					                        DWORD *lpdwWidth, DWORD *lpdwHeight,
					                        DWORD *lpBits, DWORD *dwReserve1);
}

/*****************************************************************************/
/*                             图像帧头信息                                  */
/*****************************************************************************/
#endif
typedef struct
{
	unsigned short w;       // 图像宽
	unsigned short h;       // 图像高
	
	unsigned short ad;      // 图像位数 8~16bits
	unsigned short hi;      // 图像类型代码 1-Bayer 2-JPEG
	
	unsigned long  len;     // 图像数据长度

	unsigned long ip;		// 采集当前帧的摄像机IP
	unsigned long frame_count;	//总共的触发帧数目
	unsigned long trigger_info; //高16位触发组数＋低16位当前触发的第几帧
	
	unsigned long r4;		// 触发帧时，保存两次触发间隔时间(us)
	unsigned long r5;
	unsigned long r6;

} FRAME_HEADER; // 帧头


