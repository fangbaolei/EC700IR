/*
 * arch.h
 *
 *  Created on: 2013年12月2日
 *      Author: liuc
 */

#ifndef ARCH_H_
#define ARCH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ti_media_common_def.h"

#define ENC_MAX_CH 4

typedef enum {
    USECASE_FPGA_IMX178,
    USECASE_FPGA_IMX185,
    USECASE_FPGA_IMX185_DUALVIDEO,
    USECASE_FPGA_IMX185_CVBSEXPORT,
    USECASE_FPGA_IMX178_TEST,
    USECASE_FPGA_IMX185_TEST,
    USECASE_IMX136,
    USECASE_IMX172,
    USECASE_JPEG_DEC,
    USECASE_FPGA_IMX178_WITHOUTCVBS,
    USECASE_FPGA_IMX185_DUALVIDEO_CVBSEXPORT,
    USECASE_FPGA_IMX178_WITHOUT_NSF,
    USECASE_DOME_CAMERA,
	USECASE_FPGA_IMX226,
	USECASE_FPGA_IMX249,
	USECASE_FPGA_IMX185_CROP_WITHOUTCVBS,
	USECASE_FPGA_IMX249_H264_VNF,
	USECASE_FPGA_ICX816_680W,
	USECASE_FPGA_ICX816_640W,
	USECASE_FPGA_ICX816_600W,
	USECASE_FPGA_ICX816_SINGLE,
    USECASE_MAX
} UseCase;

typedef enum {
    VIDEO_RES_1080P = 0,
    VIDEO_RES_720P,
    VIDEO_RES_540P,
    VIDEO_RES_576P,
    VIDEO_RES_480P
} VideoConfig;

typedef struct{
    void (*CbFunc)(void *);
    void* CbParam;
}IpcCbParams;

typedef struct{
    int width;
    int height;
}Jpeg_Dec_Config;


typedef struct{
    int nImageInputRate;
    int nImageOutputRate;   /// 用于图像跳帧信息 输出帧率等于实际输入帧率 x nImageInputRate/nImageOutputRate
    int nVideoInputRate;
    int nVideoOutputRate;   /// 用于视频跳帧信息 输出帧率等于实际输入帧率 x nVideoInputRate/nVideoOutputRate
    int nVideoInputRate2;
    int nVideoOutputRate2;  /// 用于第二路视频跳帧  输入输出必须为实际期望值
}SourceFpsConfig;

typedef struct {
    int cbSize; // 应等于结构体大小
    struct filePath{
    char* szFwVpssM3; // VPSS M3可执行文件路径（绝对路径，以下都是）
    char* szFwVideoM3; // Video M3可执行文件路径
    char* szFwDsp; // DSP可执行文件路径
    char* szBinRemoteDbg; // remote_debug_client
    char* szBinSysPri; // SysPri可执行文件路径
    char* szBinFwLoad; // 固件加载程序可执行文件路径
    char* szBinPrcm; // 电源管理程序可执行文件路径
    char* szBinMemrdwr; // 内存读写程序可执行文件路径
    char* szKoOsa; // OSA驱动文件路径
    char* szKoSyslink; // Syslink驱动文件路径
    char* szKoFb; // FB驱动文件路径
    char* szKoHdmi; // HDMI驱动文件路径
    char* szKoVpss; // VPSS驱动文件路径
    }FilePath;
    UseCase useCase; // Usecase，一种usecase对应一种Chain配置
    VideoConfig videoConfig; // 视频码流参数（即H.264编码参数与第二路YUV分辨率配置）
    VideoConfig videoConfig2;
    SourceFpsConfig sourceFpsConfig;
    int maxBitRate[ENC_MAX_CH];
    IpcCbParams vpssInCb;
    IpcCbParams outVpssCb;
    IpcCbParams videoInCb;
    IpcCbParams outVideoCb;
    IpcCbParams dspInCb;
    IpcCbParams outDspCb;
    IpcCbParams videoFrameInCb;
    IpcCbParams dspcallbackCb;
    int bExternal2A;
    void (*_2aMetadataCbFunc)(void *, unsigned int);
    Jpeg_Dec_Config dec_param;
    int bEnableBIOSLog;
    unsigned int capbufnum;//指定Cameralink buffer数量，若为0，则用默认值。
    unsigned int vnfbufnum;//指定vnf降噪的buffer数量，若为0，则用默认值。
    unsigned int extendJpegHight; // 扩展USECASE_FPGA_IMX185下VNF申请Buffer的高度，不应超过128行
} IpcInitParams;

int IPC_Init(IpcInitParams *ipcInitParams);

int IPC_Deinit(void);

void IPC_Start(void);

void IPC_Stop(void);

enum {
//  相机基本参数
    IPC_CONTROL_CMD_SET_EXP = 100,              //  设置曝光时间
    IPC_CONTROL_CMD_GET_EXP = 101,              //  读取曝光时间
    IPC_CONTROL_CMD_SET_GAIN = 102,             //  设置增益
    IPC_CONTROL_CMD_GET_GAIN = 103,             //  读取增益
    IPC_CONTROL_CMD_SET_RGB = 104,              //  设置RGB
    IPC_CONTROL_CMD_GET_RGB = 105,              //  读取RGB
//  相机自动化控制
    IPC_CONTROL_CMD_SET_AE_STATE = 106,         //  设置AE使能
    IPC_CONTROL_CMD_GET_AE_STATE = 107,         //  读取AE使能标志
    IPC_CONTROL_CMD_SET_AE_THRESHOLD = 108,     //  设置AE门限值
    IPC_CONTROL_CMD_GET_AE_THRESHOLD = 109,     //  读取AE当前门限值
    IPC_CONTROL_CMD_SET_AE_EXP_RANGE = 110,     //  设置AE曝光时间范围
    IPC_CONTROL_CMD_GET_AE_EXP_RANGE = 111,     //  读取AE曝光时间范围
    IPC_CONTROL_CMD_SET_AE_ZONE = 112,          //  设置AE测光区域
    IPC_CONTROL_CMD_GET_AE_ZONE = 113,          //  读取AE测光区域
    IPC_CONTROL_CMD_SET_AE_GAIN_RANGE = 114,    //  设置AE增益范围
    IPC_CONTROL_CMD_GET_AE_GAIN_RANGE = 115,    //  读取AE增益范围
    IPC_CONTROL_CMD_SET_AWB_STATE = 116,        //  设置AWB使能
    IPC_CONTROL_CMD_GET_AWB_STATE = 117,        //  读取AWB使能
//  相机图像处理
    IPC_CONTROL_CMD_SET_WDR_STRENGTH = 118,     //  设置WDR强度值
    IPC_CONTROL_CMD_GET_WDR_STRENGTH = 119,     //  读取WDR强度值
    IPC_CONTROL_CMD_SET_SHARPNESS_STATE = 120,  //  设置锐化使能
    IPC_CONTROL_CMD_GET_SHARPNESS_STATE = 121,  //  读取锐化使能
    IPC_CONTROL_CMD_SET_SHARPNESS = 122,        //  设置锐化值
    IPC_CONTROL_CMD_GET_SHARPNESS = 123,        //  读取锐化值
    IPC_CONTROL_CMD_SET_SATURATION_CONTRAST_STATE = 124,        //  设置饱和度、对比度使能
    IPC_CONTROL_CMD_GET_SATURATION_CONTRAST_STATE = 125,        //  读取饱和度、对比度使能
    IPC_CONTROL_CMD_SET_SATURATION = 126,       //  设置饱和度值
    IPC_CONTROL_CMD_GET_SATURATION = 127,       //  读取饱和度值
    IPC_CONTROL_CMD_SET_CONTRAST = 128,         //  设置对比度值
    IPC_CONTROL_CMD_GET_CONTRAST = 129,         //  读取对比度值
    IPC_CONTROL_CMD_SET_TNF_STATE = 130,        //  设置降噪TNF使能
    IPC_CONTROL_CMD_GET_TNF_STATE = 131,        //  读取降噪TNF使能
    IPC_CONTROL_CMD_SET_SNF_STATE = 132,        //  设置降噪SNF使能
    IPC_CONTROL_CMD_GET_SNF_STATE = 133,        //  读取降噪SNF使能
    IPC_CONTROL_CMD_SET_TNF_SNF_STRENGTH = 134, //  设置降噪强度值
    IPC_CONTROL_CMD_GET_TNF_SNF_STRENGTH = 135, //  读取降噪强度值
    IPC_CONTROL_CMD_SET_TRAFFIC_LIGTH_ENHANCE_STATE = 136,      //  设置红灯加红使能
    IPC_CONTROL_CMD_GET_TRAFFIC_LIGTH_ENHANCE_STATE = 137,      //  读取红灯加红使能
    IPC_CONTROL_CMD_SET_TRAFFIC_LIGTH_ENHANCE_REGION = 138,     //  设置加红区域（无读取命令）
    IPC_CONTROL_CMD_SET_TRAFFIC_LIGTH_LUM_TH = 139,             // 设置红灯加红亮度域值
    IPC_CONTROL_CMD_SET_GAMMA_ENABLE = 140,     // 设置Gamma 使能
    IPC_CONTROL_CMD_GET_GAMMA_ENABLE = 141,     // 获取Gamma使能
    IPC_CONTROL_CMD_SET_GAMMA = 142,            // 设置Gamma
    IPC_CONTROL_CMD_GET_GAMMA = 143,            // 获取Gamma
    IPC_CONTROL_CMD_SET_EDGE_ENHANCE = 144,     // 设置图像边缘增强
    IPC_CONTROL_CMD_GET_EDGE_ENHANCE = 145,     //　获取图像边缘增强值

//  相机IO控制
    IPC_CONTROL_CMD_SET_DCIRIS_AUTO_STATE = 146,//  设置自动DC光圈使能
    IPC_CONTROL_CMD_GET_DCIRIS_AUTO_STATE = 147,//  读取自动DC光圈使能标志
    IPC_CONTROL_CMD_ZOOM_DCIRIS = 148,          //   放大DC光圈
    IPC_CONTROL_CMD_SHRINK_DCIRIS = 149,        //  缩小DC光圈
    IPC_CONTROL_CMD_SET_FILTER_STATE = 150,     //  滤光片切换
    IPC_CONTROL_CMD_SET_AC_SYNC_STATE = 151,    //  设置电网同步使能
    IPC_CONTROL_CMD_GET_AC_SYNC_STATE = 152,    //  读取电网同步使能
    IPC_CONTROL_CMD_SET_AC_SYNC_DELAY = 153,    //  设置电网同步延时
    IPC_CONTROL_CMD_GET_AC_SYNC_DELAY = 154,    //  读取电网同步延时值

    IPC_CONTROL_CMD_SET_IO_ARG = 156,// 设置对外输出IO口参数
    IPC_CONTROL_CMD_GET_IO_ARG = 157,// 读取对外输出IO口参数

    IPC_CONTROL_CMD_SET_EDGE_MOD = 158,// 设置触发抓拍沿工作模式
    IPC_CONTROL_CMD_GET_EDGE_MOD = 159,// 读取触发抓拍沿工作模式

//  相机采集功能
    IPC_CONTROL_CMD_GET_H264_RES = 160,         //  读取H264分辨率
    IPC_CONTROL_CMD_SET_GRAY_IMAGE_STATE = 161, //  设置黑白图使能
    IPC_CONTROL_CMD_GET_GRAY_IMAGE_STATE = 162, //  读取黑白图使能标志
//  相机编码
    IPC_CONTROL_CMD_SET_H264_BITRATE = 163,     //  设置H264码率
    IPC_CONTROL_CMD_GET_H264_BITRATE = 164,     //  读取H264码率
    IPC_CONTROL_CMD_SET_H264_BITRATE2 = 165,     //  设置第二路H264码率
    IPC_CONTROL_CMD_GET_H264_BITRATE2 = 166,     //  读取第二路H264码率
    IPC_CONTROL_CMD_SET_H264_IFRAME_INTERVAL = 167,    //  设置H264 I帧间隔
    IPC_CONTROL_CMD_GET_H264_IFRAME_INTERVAL = 168,    //  获取H264 I帧间隔
    IPC_CONTROL_CMD_SET_JPEG_QUALITY = 169,     //  设置视频流JPEG压缩率
    IPC_CONTROL_CMD_GET_JPEG_QUALITY = 170,     //  获取视频流JPEG压缩率
    IPC_CONTROL_CMD_SET_CVBS_STD = 171,         //  设置CVBS制式
    IPC_CONTROL_CMD_GET_CVBS_STD = 172,         //  读取CVBS制式
//  DSP命令
    IPC_CONTROL_CMD_SET_VIDEO_STREAM_RECOGNIZE_PARAMS = 173,  //  视频流识别参数初始化
    IPC_CONTROL_CMD_SET_PICTURE_SNAP_RECOGNIZE_PARAMS = 174,  //  抓拍图识别参数初始化

    IPC_CONTROL_CMD_SET_SCENE=175,//设置情景模式
	
//  后门
    IPC_CONTROL_CMD_SET_FPGA_REG = 176,         // 设置fpga寄存器
    IPC_CONTROL_CMD_GET_FPGA_REG = 177,         // 读取fpga寄存器

    IPC_CONTROL_CMD_SET_WORKMODE = 178,

    IPC_CONTROL_CMD_SOFT_CAPTURE = 179,

    IPC_CONTROL_CMD_SET_FLASH_GATE=180,

    IPC_CONTROL_CMD_GET_VERSION = 181,

    IPC_CONTROL_CMD_SET_CAP_SHUTTER = 182,
    IPC_CONTROL_CMD_SET_CAP_GAIN = 183,
    IPC_CONTROL_CMD_SET_CAP_SHARPEN = 184,
    IPC_CONTROL_CMD_SET_CAP_RGB = 185,
    
    IPC_CONTROL_CMD_GET_DAY_NIGHT = 186,
    IPC_CONTROL_CMD_SET_BACKLIGHT = 187,//暂不实现
    IPC_CONTROL_CMD_SET_AWB_MODE = 188,
    
    IPC_CONTROL_CMD_SET_H264_IFRAME_INTERVAL2 = 189,
    IPC_CONTROL_CMD_GET_H264_IFRAME_INTERVAL2 = 190,

    IPC_CONTROL_CMD_SET_RELAY_STATE = 191,     //  继电器开关

    IPC_CONTROL_CMD_GET_CAMERA_FPS = 192,

    IPC_CONTROL_CMD_END
};

typedef struct {
    int nAddr;
    int nValue;
}fpgaRegSet;

typedef enum IPC_ShareRegion_e
{
    IPC_SHARE_REGION_1 = 1,
    IPC_SHARE_REGION_2 
}IPC_SHARE_REGION;

typedef struct IPC_SHAREREGION_STATUS_t
{
    unsigned int TotalSize;
    unsigned int TotalFreeSize;
    unsigned int LargestFreeSize;
}IPC_SHAREREGION_STATUS;

/* 返回值：
 * 0：成功
 * -1：一般性错误，可能是这个函数本身参数的错误，可能是传输到m3错误。
 * -2：协议参数错误，比如，命令参数范围值：[0~10]，不在此范围返回-2
 * 注：为保持代码一致性，即使命令不带参数，也要设置一个非空的pPamram，建议为int类型
 */
int IPC_Control(int nCmd, void *pParam, int nSize);

int IPC_GetFullVpssM3Frames(VIDEO_FRAMEBUF_LIST_S * pFrameBufList);
int IPC_PutEmptyVpssM3Frames(VIDEO_FRAMEBUF_LIST_S *pFrameBufList);

int IPC_PutFullVpssM3Frames(VIDEO_FRAMEBUF_LIST_S * pFrameBufList);
int IPC_GetEmptyVpssM3Frames(VIDEO_FRAMEBUF_LIST_S *pFrameBufList);

int IPC_GetFullVideoM3Frames(VIDEO_FRAMEBUF_LIST_S * pFrameBufList);
int IPC_PutEmptyVideoM3Frames(VIDEO_FRAMEBUF_LIST_S *pFrameBufList);

int IPC_PutFullDspFrames(VIDEO_FRAMEBUF_LIST_S *pFrameBufList);
int IPC_GetEmptyDspFrames(VIDEO_FRAMEBUF_LIST_S *pFrameBufList);
int IPC_GetFullDspData(VIDEO_FRAMEBUF_LIST_S *pFrameBufList);
int IPC_PutEmptyDspData(VIDEO_FRAMEBUF_LIST_S *pFrameBufList);

int IPC_PutFullVideoM3Frames(VIDEO_FRAMEBUF_LIST_S *pFrameBufList);
int IPC_GetEmptyVideoM3Frames(VIDEO_FRAMEBUF_LIST_S *pFrameBufList);
int IPC_GetFullVideoM3Bits(VCODEC_BITSBUF_LIST_S* pBitsBufList);
int IPC_PutEmptyVideoM3Bits(VCODEC_BITSBUF_LIST_S* pBitsBufList);

int IPC_GetEmptyVideoM3Bits(VCODEC_BITSBUF_LIST_S* pBitsBufList);
int IPC_PutFullVideoM3Bits(VCODEC_BITSBUF_LIST_S* pBitsBufList);

void IPC_FramesPrintInfo(VIDEO_FRAMEBUF_S *buf);
void IPC_FramesPrintListInfo(VIDEO_FRAMEBUF_LIST_S *bufList,
        char *listName);

void IPC_BitsPrintInfo(VCODEC_BITSBUF_S *buf);
void IPC_BitsPrintListInfo(VCODEC_BITSBUF_LIST_S *bufList,
        char *listName);

void IPC_printDetailedStatistics(void);
void IPC_printBufferStatistics(void);

/*获取vnf Link 剩余buffer数量，-1表示失败*/
Int32 IPC_GetVnfEmptyBufferNum(void);

void IPC_Cache_WbInv(void* blockPtr, unsigned int byteCnt, unsigned short type, unsigned short wait);
void IPC_Cache_Inv(void* blockPtr, unsigned int byteCnt, unsigned short type, unsigned short wait);
void IPC_Cache_Wb(void* blockPtr, unsigned int byteCnt, unsigned short type, unsigned short wait);

/*注意:   ShareRegion 相关接口，需要在调用IPC_Start接口后才起作用，在调用IPC_Stop之前要释放相关内存*/
void *IPC_ShareRegion_Malloc(IPC_SHARE_REGION sr_index,int size);

void IPC_ShareRegion_Free(IPC_SHARE_REGION sr_index,void *ptr,int size);

void IPC_ShareRegion_Get_status(IPC_SHARE_REGION sr_index,IPC_SHAREREGION_STATUS *sr_status);

int IPC_Enc_Set_TargetBitrate(int chId,int targetBitrate);

int IPC_Enc_Set_IframeIntervel(int chId,int Intervel);

int IPC_Enc_Set_RateControl(int chId,int rate_control);/*0 - VBR; 1 - CBR*/

int IPC_Enc_Set_VbrDuration(int chId,int Duration);/*1 - 3600*/

int IPC_Enc_Set_VbrSensitivity(int chId,int Sensitivity);/*1 - 8*/

int IPC_Enc_Get_TargetBitrate(int chId,int *targetBitrate);
    
int IPC_Enc_Get_IframeIntervel(int chId,int *intervel);

#ifdef __cplusplus
}
#endif

#endif /* ARCH_H_ */
