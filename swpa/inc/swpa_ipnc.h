#ifndef __SWPA_IPNC_H__
#define __SWPA_IPNC_H__
#ifdef __cplusplus
extern "C"
#ifndef DEFAULT
#define DEFAULT(val) =val
#endif
{
#endif
enum
{
	  SWPA_LINK_ALL
	, SWPA_LINK_A8
	, SWPA_LINK_VPSS
	, SWPA_LINK_VIDEO
	, SWPA_LINK_DSP
	, SWPA_LINK_MAX
};

enum
{
	  SWPA_IMAGE_NONE 
	, SWPA_IMAGE_YUV  
	, SWPA_IMAGE_H264 
	, SWPA_IMAGE_JPEG 
};

enum {
    SWPA_VIDEO_RES_1080P = 0,
    SWPA_VIDEO_RES_720P,
    SWPA_VIDEO_RES_540P,
    SWPA_VIDEO_RES_576P,
    SWPA_VIDEO_RES_480P
};

enum
{
	SWPA_VPSS_H264_CHANNEL = 0,
	SWPA_VPSS_JPEG_CHANNEL = 1,
	SWPA_VPSS_H264_SECOND_CHANNEL = 2,
	SWPA_VPSS_JPEG_SECOND_CHANNEL= 2,	//未降噪数据通道与H264第二路使用同一个通道号
	SWPA_VPSS_CHANNEL_MAX
};


#define CALLBACK_TYPE_IMAGE 0
#define CALLBACK_TYPE_DATA  1

typedef struct tagIPNC_DATA
{
	int   size; //image size
	void* addr[3];	//image virtual address
	void* phys[3];
	void* appData; 
	int   dataSize;
}IPNC_DATA;

typedef struct tagIMAGE
{
	int channel;
	int type;		//0:yuv,1:jpeg,2:h264
	int width;  //image width
	int height; //image height
	int pitch;  //image pitch
	int frameType; //0:I,1:P:2:B,only to h264
	int shutter;  //曝光时间
	int gain;     //增益 
	int rGain;    //白平衡的r值
	int gGain;    //白平衡的g值
	int bGain;    //白平衡的b值
	int isCapture;//是否抓拍图标志	
	int iAvgY; // 平均亮度
	int iRawAvgY; //ISP
	int iCaptureEdgeExt; //触发输入沿状态 0-无触发沿,1-下降沿,2-上升沿,3-下降沿或者上升沿
	int iCaptureFlag;	//抓拍计数
	int iCaptureEnableFlag;	//抓拍使能标志位， 0位快门 1位增益 2位黑电平 3位RGB
	int iCaptureShutter;	//抓拍快门
	int iCaptureGain;		//抓拍增益
	int iCaptureRGain;		//像素R增益
	int iCaptureGGain;		//像素H增益
	int iCaptureBGain;		//像素B增益
	IPNC_DATA data;	
	unsigned int uTimeStamp; //FPGA采集时标
}IMAGE;

enum 
{
		//  相机基本参数
    CMD_SET_EXP = 100,        //  设置曝光时间
    CMD_GET_EXP,            //  读取曝光时间
    CMD_SET_GAIN,           //  设置增益
    CMD_GET_GAIN,           //  读取增益
    CMD_SET_RGB,            //  设置RGB
    CMD_GET_RGB,            //  读取RGB
		//  相机自动化控制
    CMD_SET_AE_STATE,       //  设置AE使能
    CMD_GET_AE_STATE,       //  读取AE使能标志
    CMD_SET_AE_THRESHOLD,   //  设置AE门限值
    CMD_GET_AE_THRESHOLD,   //  读取AE当前门限值
    CMD_SET_AE_EXP_RANGE,   //  设置AE曝光时间范围
    CMD_GET_AE_EXP_RANGE,   //  读取AE曝光时间范围
    CMD_SET_AE_ZONE,        //  设置AE测光区域
    CMD_GET_AE_ZONE,        //  读取AE测光区域
    CMD_SET_AE_GAIN_RANGE,  //  设置AE增益范围
    CMD_GET_AE_GAIN_RANGE,  //  读取AE增益范围
    CMD_SET_SCENE,		//
    //CMD_GET_SCENE,		//
    CMD_SET_AWB_STATE,      //  设置AWB使能
    CMD_GET_AWB_STATE,      //  读取AWB使能
//  相机图像处理
    CMD_SET_WDR_STRENGTH,   //  设置WDR强度值
    CMD_GET_WDR_STRENGTH,   //  读取WDR强度值
    CMD_SET_SHARPNESS_STATE,//  设置锐化使能
    CMD_GET_SHARPNESS_STATE,//  读取锐化使能
    CMD_SET_SHARPNESS,      //  设置锐化值
    CMD_GET_SHARPNESS,      //  读取锐化值
    CMD_SET_SATURATION_CONTRAST_STATE,  //  设置饱和度、对比度使能
    CMD_GET_SATURATION_CONTRAST_STATE,  //  读取饱和度、对比度使能
    CMD_SET_SATURATION,     //  设置饱和度值
    CMD_GET_SATURATION,     //  读取饱和度值
    CMD_SET_CONTRAST,       //  设置对比度值
    CMD_GET_CONTRAST,       //  读取对比度值
    CMD_SET_TNF_STATE,//  设置降噪TNF使能
    CMD_GET_TNF_STATE,//  读取降噪TNF使能
    CMD_SET_SNF_STATE,//  设置降噪SNF使能
    CMD_GET_SNF_STATE,//  读取降噪SNF使能
    CMD_SET_TNF_SNF_STRENGTH,   //  设置降噪强度值
    CMD_GET_TNF_SNF_STRENGTH,   //  读取降噪强度值
    CMD_SET_TRAFFIC_LIGTH_ENHANCE_STATE,    //  设置红灯加红使能
    CMD_GET_TRAFFIC_LIGTH_ENHANCE_STATE,    //  读取红灯加红使能
    CMD_SET_TRAFFIC_LIGTH_ENHANCE_REGION,   //  设置加红区域（无读取命令）
    CMD_SET_TRAFFIC_LIGTH_LUM_TH,    // 设置红灯加红亮度域值
    CMD_SET_GAMMA_ENABLE,  // 设置Gamma 使能
    CMD_GET_GAMMA_ENABLE,  // 获取Gamma使能
    CMD_SET_GAMMA,  // 设置Gamma
    CMD_GET_GAMMA,  // 获取Gamma
    CMD_SET_EDGE_ENHANCE, // 设置图像边缘增强
    CMD_GET_EDGE_ENHANCE, //　获
//  相机IO控制
    CMD_SET_DCIRIS_AUTO_STATE,   //  设置自动DC光圈使能
    CMD_GET_DCIRIS_AUTO_STATE,   //  读取自动DC光圈使能标志
    CMD_ZOOM_DCIRIS,   //   放大DC光圈
    CMD_SHRINK_DCIRIS,   //  缩小DC光圈
    CMD_SET_FILTER_STATE,   //  滤光片切换
    CMD_SET_RELAY_STATE,    //  继电器通断控制
    CMD_SET_AC_SYNC_STATE,  //  设置电网同步使能
    CMD_GET_AC_SYNC_STATE,  //  读取电网同步使能
    CMD_SET_AC_SYNC_DELAY,  //  设置电网同步延时
    CMD_GET_AC_SYNC_DELAY,  //  读取电网同步延时值

	CMD_SET_IO_ARG,			// 设置对外输出IO口参数
    CMD_GET_IO_ARG,			// 读取对外输出IO口参数
		
	CMD_SET_EDGE_MOD,		// 设置触发抓拍沿工作模式
	CMD_GET_EDGE_MOD,		// 读取触发抓拍沿工作模式
	
		//  相机采集功能
    CMD_GET_H264_RES,       //  读取H264分辨率
    CMD_SET_GRAY_IMAGE_STATE,   //  设置黑白图使能
    CMD_GET_GRAY_IMAGE_STATE,   //  读取黑白图使能标志
		//  相机编码
    CMD_SET_H264_BITRATE,   //  设置H264码率
    CMD_GET_H264_BITRATE,   //  读取H264码率
    CMD_SET_H264_BITRATE2,	//	设置第二路H264码率
    CMD_GET_H264_BITRATE2,	//	读取第二路H264码率
    CMD_SET_H264_IFRAME_INTERVAL,    //  设置H264 I帧间隔
    CMD_GET_H264_IFRAME_INTERVAL,    //  设置H264 I帧间隔
    CMD_SET_JPEG_QUALITY,   //  设置视频流JPEG压缩率
    CMD_GET_JPEG_QUALITY,   //  设置视频流JPEG压缩率
    CMD_SET_CVBS_STD,       //  设置CVBS制式
    CMD_GET_CVBS_STD,       //  读取CVBS制式
		//  DSP命令
    CMD_SET_VIDEO_STREAM_RECOGNIZE_PARAMS,  //  视频流识别参数初始化
    CMD_SET_PICTURE_SNAP_RECOGNIZE_PARAMS,  //  抓拍图识别参数初始化

		//  后门
    CMD_SET_FPGA_REG,
    CMD_GET_FPGA_REG,

	//相机工作模式
	CMD_SET_WORKMODE,

	CMD_SOFT_CAPTURE,

	CMD_SET_FLASH_GATE,
	
	CMD_GET_VERSION,

	CMD_SET_CAP_SHUTTER,
	CMD_SET_CAP_GAIN,
	CMD_SET_CAP_SHARPEN,
	CMD_SET_CAP_RGB,

	CMD_GET_DAY_NIGHT,
	CMD_SET_BACKLIGHT,
	
	CMD_SET_AWB_MODE,

	CMD_SET_H264_IFRAME_INTERVAL2,
    CMD_GET_H264_IFRAME_INTERVAL2,

    CMD_END
};

enum 
{
    IMX178,
    IMX185,
    IMX185_DUALVIDEO,
    IMX185_CVBSEXPORT,
    IMX178_TEST,
    IMX185_TEST,
    IMX136,
    IMX172,
    JPEG_DEC,
	IMX178_WITHOUTCVBS,
	IMX185_DUALVIDEO_CVBSEXPORT,
	IMX178_WITHOUT_NSF,
	DOME_CAMERA,
	IMX226,
	IMX249,
	IMX185_CROP_WITHOUTCVBS,
	IMX249_H264_VNF,
	ICX816,			//680w
	ICX816_640W,
	ICX816_600W,
	ICX816_SINGLE,
    IMX_MAX
};

enum
{
	SENSOR_IMX178,
    SENSOR_IMX185,
    SENSOR_IMX174,
    SENSOR_IMX249,
    SENSOR_ICX816,
    SENSOR_MAX
};

typedef struct tagIPNCCreateConfig {
	int imx;							//前端类型
	int videoConfig;					//第一路H264分辨率(imx为IMX185_DUALVIDEO有效)
	int videoConfigSecond;				//第二路H264分辨率(imx为IMX185_DUALVIDEO有效)
	int secondVideoFrameRate;			//第二路H264帧率(imx为IMX185_DUALVIDEO有效)
	int verbose;						//日志输出开关，默认关闭
	int maxBitRate;						//第一路最大码率
	int secondMaxBitRate;				//第二路最大码率
}IPNCCreateConfig;

/**
 *@brief 设置接收数据回调函数
 *@param      pContext 回调函数的设备上下文
 *@param      pData    回调数据指针
 *@param      nLen     数据长度
 */
typedef void (*metadata_callback_func)(void *pContext, void *pData, int nLen);

/**
   @brief IPNC创建函数
   @param [IN] ipnc_create_config 创建时的配置参数
*/
int swpa_ipnc_create(IPNCCreateConfig *ipnc_create_config);


/**
 *@brief 设置接收数据回调函数
 *@param [in] link_id IPCLink的id号
 *@param [in] OnResult 回调函数,成功返回0，失败返回-1
 *@param      pContext 回调函数的设备上下文
 *@param      type     回调函数数据类型
 *@param               1:图片数据
 *@param      struct_ptr 结构体指针
 */
void swpa_ipnc_setcallback(
	int link_id
	, int channel_id
	, int (*OnResult)(void *pContext, int type, void *struct_ptr)
	, void *pContext
	);

/**
 *@brief 设置附加数据接收数据回调函数
 *@param [in] callbackfunc 回调函数,成功返回0，失败返回-1
 *@param      pContext 		回调函数的设备上下文
 */
void swpa_ipnc_setmetadatacallback(
	metadata_callback_func callbackfunc
	, void *pContext
	);

//0:185,1:178
int swpa_ipnc_mode(void);
int swpa_get_sensor_type(void);
int swpa_ipnc_start(void);
void swpa_ipnc_release(void *addr, int fphys DEFAULT(0));
void* swpa_ipnc_get_app_ptr(void *addr);
int swpa_ipnc_get_app_size(void *addr);
int swpa_ipnc_resample(void *addr, void**y DEFAULT(0), void**uv DEFAULT(0));
int swpa_ipnc_Crop(void *addr,
					int left, int top, int right, int bottom,
				    int* piDetWidth, int* piDetHeight);
// 用于截图之后对 YUV 图图像的还原
int swpa_ipnc_Crop_reset(void *addr,
                    int left, int top, int right, int bottom,
                    int iResetWidth, int iResetHeight,
                    int* piDetWidth, int* piDetHeight);
int swpa_ipnc_scale_up(void *addr, int up, int offset, int *pnewheight);
int swpa_ipnc_send_data (int link_id, void *addr, int size, void *appData DEFAULT(0), int dataSize DEFAULT(0));

int swpa_ipnc_get_vnf_frame(void *reg_frame_addr, IMAGE *vnf_image_info, int image_ts);


int swpa_ipnc_control(int link_id, int cmd, void *pPrm, int prmSize, int timeout DEFAULT(-1));
void swpa_ipnc_close(void);
void swpa_ipnc_cache_wbinv(void* blockPtr, unsigned int byteCnt, unsigned short type, unsigned short wait);
void swpa_ipnc_cache_inv(void* blockPtr, unsigned int byteCnt, unsigned short type, unsigned short wait);
void swpa_ipnc_cache_wb(void* blockPtr, unsigned int byteCnt, unsigned short type, unsigned short wait);
int  swpa_ipnc_add_jpg(void* buffer, int size);

void swpa_ipnc_printDetailedStatistics(void);
void swpa_ipnc_printBufferStatistics(void);
//M3内存共享区域接口
void* swpa_ipnc_share_region_malloc(int index, int size);
void swpa_ipnc_share_region_free(int index, void* ptr, int size);
void swpa_ipnc_share_region_get_total_size(int *size);
void swpa_ipnc_share_region_get_free_size(int *size);
//M3实时设置H264接口
int swpa_ipnc_set_target_bitrate(int chID, int target_bitrate);       //目标码率
int swpa_ipnc_set_iframe_interval(int chID, int interval);			//I帧间隔
int swpa_ipnc_set_rate_control(int chID, int rate_control);			//码率控制 0:VBR 1:CBR
int swpa_ipnc_set_vbr_duration(int chID, int duration);				//场景变化采样时间
int swpa_ipnc_set_vbr_sensitivity(int chID, int sensitivity);		//场景变化灵敏度
int swpa_ipnc_get_target_bitrate(int chID, int *target_bitrate);
int swpa_ipnc_get_iframe_interval(int chID, int *interval);

#ifdef __cplusplus
}
#endif
#endif
