// 该文件编码必须是WINDOWS-936格式
/**
 * @file   CameraController.h
 * @author Lijj
 * @date   Wed Feb 29 2012
 *
 * @brief
 *         相机功能接口类，所有设置相机相关的函数均在此类实现(如设置增益、快门)，
 *         本文件声明全局对象g_cCameraController，使用时需包含本文件。
 *
 */

#ifndef _CAMERACONTROL_H_
#define _CAMERACONTROL_H_

#ifndef WIN32

#include "hvutils.h"
#include "TcpipCfg.h"
#include "AgcAwbThread.h"

// frome HvPciLinkApi.h
// 动态更改的参数类型
typedef enum _DCP_TYPE
{
    DCP_SEND_TYPE = 0,
    DCP_CAM_OUTPUT,
    DCP_ENABLE_AGC,
    DCP_ENABLE_AWB,
    DCP_AGC_TH,
    DCP_SHUTTER,
    DCP_GAIN,
    DCP_JPEG_EXPECT_SIZE,
    DCP_JPEG_COMPRESS_RATE,
    DCP_JPEG_COMPRESS_RATE_CAPTURE,
    DCP_FLASH_RATE_SYN_SIGNAL_ENABLE,
    DCP_CAPTURE_SYN_SIGNAL_ENABLE,
    DCP_CPL,
    DCP_SETPULSEWIDTH
} DCP_TYPE;

#endif

// -------------------------------------------------------------

struct FpgaRegInfo_500w
{
    DWORD32 reg_video_SHT1;
    DWORD32 reg_capture_SHT2;
    DWORD32 reg_LED_FLASH_OUT_EN;
    DWORD32 reg_capture_mode;
    DWORD32 reg_ACSP;
    DWORD32 reg_soft_capture_trig;
    DWORD32 anti_flicker;
    DWORD32 Th_Custom_ab;
    DWORD32 Th_Count_ab;
    DWORD32 reg_fpga_version;
    DWORD32 reg_capture_edge_select;
    DWORD32 Gamma_Data_ab[8];
    DWORD32 time_cnt_out;
    DWORD32 pxga_addr_r_ab;
    DWORD32 pxga_addr_gr_ab;
    DWORD32 pxga_addr_gb_ab;
    DWORD32 pxga_addr_b_ab;

    DWORD32 data_b_Gr;
    DWORD32 data_k_Gr;
    DWORD32 data_b_Gb;
    DWORD32 data_k_Gb;
    DWORD32 data_b_R;
    DWORD32 data_k_R;
    DWORD32 data_b_B;
    DWORD32 data_k_B;

    DWORD32 time_clock_out;
    DWORD32 sampling_point_ab;
};

typedef struct _FpgaRegInfo
{
#ifdef _CAMERA_PIXEL_500W_
    struct FpgaRegInfo_500w cFpgaRegInfoA;
    struct FpgaRegInfo_500w cFpgaRegInfoB;
#else
    DWORD32 reg_video_SHT1;
    DWORD32 reg_capture_SHT2;
    DWORD32 reg_LED_FLASH_OUT_EN;
    DWORD32 reg_capture_mode;
    DWORD32 reg_ACSP;
    DWORD32 reg_soft_capture_trig;
    DWORD32 anti_flicker;
    DWORD32 Th_Custom;
    DWORD32 Th_Count;
    DWORD32 reg_fpga_version;
    DWORD32 reg_capture_edge_select;
    DWORD32 Gamma_Data[8];
    DWORD32 time_cnt_out;
    DWORD32 time_clock_out;
    DWORD32 sampling_point;
    DWORD32 capture_ID;

    // 红灯加红相关信息
    DWORD32 th_h_prt;
    DWORD32 th_l_prt;
    DWORD32 th_s_prt;
    DWORD32 colr_prt;
    DWORD32 th_l_reg_new;
    DWORD32 colr_reg_new_l;
    DWORD32 colr_reg_new_h;

    // 锐化
    DWORD32 sha_th_o;       // 锐化阀值
    DWORD32 sha_ce_o;       // 锐化使能开关
    DWORD32 sha_s_or_a_o;   // 锐化抓拍帧标志

    // 饱和度r/g/b分量值
    DWORD32 satu_r_f_max_o;
    DWORD32 satu_r_f_min_o;
    DWORD32 satu_g_f_max_o;
    DWORD32 satu_g_f_min_o;
    DWORD32 satu_b_f_max_o;
    DWORD32 satu_b_f_min_o;

    // 对比度乘法/加法因子
    DWORD32 cont_rgb_f_o;
    DWORD32 cont_add_f_o;
#endif // _CAMERA_PIXEL_500W_
} FpgaRegInfo;

struct AgcAwbInfo_500w
{
    DWORD32 SumR_ab;
    DWORD32 SumG_ab;
    DWORD32 SumB_ab;
    DWORD32 SumY_ab;
    DWORD32 SumU_ab;
    DWORD32 SumV_ab;
    DWORD32 PointCount_ab;
    DWORD32 AvgY_ab[16];

    DWORD32 SUM_Gr_raw_Low_ab;
    DWORD32 SUM_Gr_raw_High_ab;
    DWORD32 SUM_Gb_raw_Low_ab;
    DWORD32 SUM_Gb_raw_High_ab;
    DWORD32 SUM_R_raw_Low_ab;
    DWORD32 SUM_R_raw_High_ab;
    DWORD32 SUM_B_raw_Low_ab;
    DWORD32 SUM_B_raw_High_ab;

    DWORD32 reg_gr_ab;
    DWORD32 reg_gb_ab;
    DWORD32 reg_r_ab;
    DWORD32 reg_b_ab;

    DWORD32 th_h_reg;
    DWORD32 th_l_reg;
    DWORD32 th_s_reg;
    DWORD32 colr_reg;
    DWORD32 addr_prt;
    DWORD32 cod_x_min_in;
    DWORD32 cod_x_max_in;
    DWORD32 cod_y_min_in;
    DWORD32 cod_y_max_in;
};

typedef struct _AgcAwbInfo
{
#ifdef _CAMERA_PIXEL_500W_
    struct AgcAwbInfo_500w cAgcAwbInfoA;
    struct AgcAwbInfo_500w cAgcAwbInfoB;
#else
    DWORD32 SumR;
    DWORD32 SumG;
    DWORD32 SumB;
    DWORD32 SumY;
    DWORD32 SumU;
    DWORD32 SumV;
    DWORD32 PointCount;
    DWORD32 AvgY[16];
#endif
} AgcAwbInfo;

// -------------------------------------------------------------
#ifndef WIN32

// 相机设置相关的接口
class ICameraUtils
{
public:
#ifdef _CAM_APP_
    // 获取编码模式
    virtual int GetEncodeMode() = 0;
#endif
    // 设置AGC区域
    virtual int SetAGCZone(int rgiAGCZone[16]) = 0;
    // 获取AGC区域
    virtual int GetAGCZone(int *prgiAGCZone) = 0;
    // 设置AGC参数
    virtual int SetAGCParam(int iShutterMin, int iShutterMax, int iGainMin, int iGainMax) = 0;
    // 保存当前参数
    virtual HRESULT SaveCurrentParam() = 0;
    // 动态改变参数
    virtual HRESULT DynChangeParam(DCP_TYPE eType, int iValue, bool fFromPCI = false) = 0;
};

// 字符叠加接口
class ICharacterOverlap
{
public:
    virtual HRESULT EnableCharacterOverlap(int iValue) = 0;
    virtual HRESULT SetCharacterOverlap(char* pLattice) = 0;
    virtual HRESULT SaveCharacterInfo(void) = 0;
    virtual HRESULT SetCharacterLightness(int iIsFixedLight) = 0;
};

class CCameraController
{
public:
    CCameraController();
    ~CCameraController();

    int SetRgbGain(int iGainR, int iGainG, int iGainB);
    int SetGain(int iGain);
    int SetShutter(int iShutter);
    int SetCaptureRgbGain(int iGainR, int iGainG, int iGainB, int fEnable);
    int SetCaptureGain(int iGain, int fEnable);
    int SetCaptureShutter(int iShutter, int fEnable);
    int SetFpsMode(int iMode, BOOL fResetH264 = FALSE);
    int GetFpsMode(int& iMode);
    int SynSignalEnable(int iFlashRateEnable, int iCaptureEnable);
    int CaptureEdgeSelect(int iMode);
    int SoftTriggerCapture();
    int SetGammaData(int rgiDataXY[8][2]);
    int FpgaTimeRegClr();
    int SetRegDirect(DWORD32 addr, DWORD32 data);
    int SetTrafficLightEnhanceParam(int iHTh, int iLTh, int iSTh, int iColorFactor);
    int SetControlBoardIOOpt(int fEnable);
    // iLTh 亮度阈值
    // iLFactor 亮度乘法因子
    // iHFactor H信号乘法因子
    int SetTrafficLightEnhanceLumParam(int iLTh, int iLFactor, int iHFactor);
    // SetTrafficLightEnhanceZone说明：
    // iId为红灯加红区域ID值，合法值为0至7(注：该值表示红灯加红灯组数，A67版本FPGA已减少为8个)
    // 在Windows屏幕坐标系下（即：屏幕左上角为原点）：
    // （iX1，iY1）为图片中加红区域的左上角坐标点。
    // （iX2，iY2）为图片中加红区域的右下角坐标点。
    int SetTrafficLightEnhanceZone(int iId, int iX1, int iY1, int iX2, int iY2);

    //////////////////////////////
    int SetADSamplingValue(int iValueA, int iValueB);
    int SublaneCapture(int iPsdVersion);
    int FlashDifferentLane(int fEnalbe);
    int FlashDifferentLaneExt(int fEnalbe); // 硬触发抓拍分车道
    int SlaveSoftTriggerCapture();

    /*---------- A65或以上版本支持 BEGIN----------*/
    int ResetFpga();    // 复位FPGA
    int SetSmallShutterRaw(int iShutter);   // 小快门
    int SetOrgRgbGainToZero();      // 将原RGB增益寄存器设置为0
    /*---------- A65或以上版本支持 END----------*/

    //----------------A67 开始
    int EnableSharpen(int fEnable);
    int SetSharpenThreshold(int iThreshold);
    int SetSharpenCapture(int fEnable);
    int SetContrast(int nValue);
    int SetSaturation(int nValue);
    int SetRawRGBGain(int iRawR, int iRawGr, int iRawGb, int iRawB);
    //----------------A67 结束

    //----------------A68 开始
    int SetGammaOutputMode(int iMode);
    //----------------A68 结束

    //----------------A70 开始
    int SetSaturationMode(int iMode);
    int SetContrastMode(int iMode);
    int SetDpcThreshold(int iValue);
    //----------------A70 结束

    // 功能函数：设置相机的快门，增益，颜色增益以及抓拍快门，抓拍增益和抓拍颜色增益。
    int SetShutter_Camyu(int iShutter);
    int GetShutter_Camyu(int& iShutter);
    int SetGain_Camyu(int iGain);
    int GetGain_Camyu(int& iGain);
    int SetRgbGain_Camyu(int iGainR, int iGainG, int iGainB);
    int GetRgbGain_Camyu(int& iGainR, int& iGainG, int& iGainB);
    int SetCaptureShutter_Camyu(int iShutter, int fEnable);
    int GetCaptureShutter_Camyu(int& iShutter, int& fEnable);
    int SetCaptureGain_Camyu(int iGain, int fEnable);
    int GetCaptureGain_Camyu(int& iGain, int& fEnable);
    int SetCaptureRgbGain_Camyu(int iGainR, int iGainG, int iGainB, int fEnable);
    int GetCaptureRgbGain_Camyu(int& iGainR, int& iGainG, int& iGainB, int& fEnable);

    // 工具函数：相机单位转换
    int Shutter_Raw2Camyu(int iRawValue);
    int Shutter_Camyu2Raw(int iCamyuValue);
    int Gain_Raw2Camyu(int iRawValue);
    int Gain_Camyu2Raw(int iCamyuValue);
    int ColorGain_Raw2Camyu(int iRawValue);
    int ColorGain_Camyu2Raw(int iCamyuValue);
    int SmallShutter_Raw2Camyu(int iRawValue);
    int SmallShutter_Camyu2Raw(int iCamyuValue);

    // 获取FPGA嵌入在图像数据中的特殊值
    HRESULT GetExtInfoByImage(
        const PBYTE8 pbImageDataY,
        FpgaRegInfo& cFpgaRegInfo,
        AgcAwbInfo& cAgcAwbInfo
    );

    void MountCameraUtils(ICameraUtils* pCameraUtils)
    {
        m_pCameraUtils = pCameraUtils;
    }

    void MoutCharacterUtils(ICharacterOverlap* pCharacterUtils)
    {
        m_pCharacterUtils = pCharacterUtils;
    }

    int GetEncodeMode()
    {
#ifdef _CAM_APP_
        return m_pCameraUtils->GetEncodeMode();
#else
        return -1;
#endif
    }
    // 设置AGC区域
    int SetAGCZone(int rgiAGCZone[16])
    {
        return m_pCameraUtils->SetAGCZone(rgiAGCZone);
    }
    // 获取AGC区域
    int GetAGCZone(int *prgiAGCZone)
    {
        return m_pCameraUtils->GetAGCZone(prgiAGCZone);
    }

    // 设置AGC参数
    int SetAGCParam(int iShutterMin, int iShutterMax, int iGainMin, int iGainMax)
    {
        return m_pCameraUtils->SetAGCParam(iShutterMin, iShutterMax, iGainMin, iGainMax);
    }
    // 保存当前参数
    HRESULT SaveCurrentParam()
    {
        return m_pCameraUtils->SaveCurrentParam();
    }
    // 动态改变参数
    HRESULT DynChangeParam(DCP_TYPE eType, int iValue)
    {
        return m_pCameraUtils->DynChangeParam(eType, iValue);
    }

    // 字符叠加
    HRESULT EnableCharacterOverlap(int iValue)
    {
         return m_pCharacterUtils->EnableCharacterOverlap(iValue);
    }
    HRESULT SetCharacterOverlap(char* pLattice)
    {
        return m_pCharacterUtils->SetCharacterOverlap(pLattice);
    }
    HRESULT SaveCharacterInfo(void)
    {
        return m_pCharacterUtils->SaveCharacterInfo();
    }
    HRESULT SetCharacterLightness(int iIsFixedLight)
    {
        return m_pCharacterUtils->SetCharacterLightness(iIsFixedLight);
    }

private:
    int WriteAD(FPGA_STRUCT datas);
    void SetFpgaStructContext(FPGA_STRUCT& cFpgaStruct, DWORD32 dwAddr, DWORD32 dwData);

private:
    HV_SEM_HANDLE m_hSemSwDev;  //AD和FPGA芯片操作线程安全化
    int m_rgiNowGammaDataXY[8][2];
    ICameraUtils* m_pCameraUtils;   //相机操作接口
    ICharacterOverlap* m_pCharacterUtils;   // 字符叠加接口
};

extern CCameraController g_cCameraController;

/**
* @brief 设备参数结构体
*/
typedef struct _DeviceInfoParam
{
    char szMac[32];
    char szSn[128];
    char szVer[64];

    _DeviceInfoParam()
    {
        char szIpTmp[32] = {0};
        char szMaskTmp[32] = {0};
        char szGatewayTmp[32] = {0};
        GetLocalTcpipAddr("eth0", szIpTmp, szMaskTmp, szGatewayTmp, szMac);

        GetSN(szSn, sizeof(szSn));

        sprintf(szVer, "%s %s %s",
                PSZ_DSP_BUILD_NO,
                DSP_BUILD_DATE,
                DSP_BUILD_TIME
               );
    };

} DeviceInfoParam;

//相机平台模块参数结构体
//注：因为此结构体使用了前面声明的类对象，所以需要放到该类后面
typedef struct _CamAppParam
{
    int iSendType;          /**< 传输模式。-1：不传输；0：传输。 */
    int iJpegStream;        /**< 是否开启Jpeg流。0:否；1：是。 */
    int iH264Stream;        /**< 是否开启H.264流。0:否；1：是。 */

    int iIFrameInterval;    /**< H.264码流中的I帧间隔 */
    int iTargetBitRate;     /**< H.264码流比特率 */
    int iFrameRate;         /**< H.264码流帧率 */

    int iAGCTh;             /**< AGC期望亮度 */
    int iAGCShutterLOri;    /**< AGC快门调节下限 */
    int iAGCShutterHOri;    /**< AGC快门调节上限 */
    int iAGCGainLOri;       /**< AGC增益调节下限 */
    int iAGCGainHOri;       /**< AGC增益调节上限 */

    int iGainR;             /**< R增益 */
    int iGainG;             /**< G增益 */
    int iGainB;             /**< B增益 */
    int iGain;              /**< 相机增益 */
    int iShutter;           /**< 快门时间 */

    int iCaptureGainR;      /**< 抓拍R增益 */
    int iCaptureGainG;      /**< 抓拍G增益 */
    int iCaptureGainB;      /**< 抓拍B增益 */
    int iCaptureGain;       /**< 抓拍相机增益 */
    int iCaptureShutter;    /**< 抓拍快门时间 */

    int iEnableCaptureGainRGB;          /**< 使能抓拍RGB颜色增益 */
    int iEnableCaptureGain;             /**< 使能抓拍增益 */
    int iEnableCaptureShutter;          /**< 使能抓拍快门 */

    int iJpegCompressRate;              /**< Jpeg压缩率 */
    int iJpegCompressRateCapture;       /**< 抓拍Jpeg图压缩率 */

    BOOL fIsSafeSaver;                  /**< 是否启动安全存储 */
    BOOL fSaveVideo;                    /**< 是否启动录像存储 */
    BOOL fInitHdd;                      /**< 是否初始化硬盘 */
    int  iFileSize;                     /**< 定长存储中单个文件的大小*/
    char szNFSParam[255];               /**< NFS协议的挂载参数*/
    char szSafeSaverInitStr[64];        /**< 安全储存器初始化参数 */

    BOOL fIsSideInstall;                /**< 前端镜头是否为侧装 */
    int iJpegExpectSize;                /**< Jpeg图片期望大小（单位：Byte），通过自动设置Jpeg压缩率实现 */
    int iJpegCompressRateL;             /**< Jpeg压缩率自动调节下限 */
    int iJpegCompressRateH;             /**< Jpeg压缩率自动调节上限 */

    int iJpegType;                      /**< Jpeg图片格式 */
    int iOutputFrameRate;               /**< 相机输出帧率 */

    int iAGCEnable;                     /**< AGC使能 */
    int iAWBEnable;                     /**< AWB使能 */
    int iFlashRateSynSignalEnable;      /**< 频闪同步信号使能 */
    int iCaptureSynSignalEnable;        /**< 抓拍同步信号使能 */
    int iENetSyn;                       /**< 电网同步 0:15fps 1:12.5fps */
    int iCaptureEdge;                   /**< 抓拍触发沿 0:外部下降沿触发 1:外部上升沿触发 2: 上升沿下降沿都触发 3：不触发 */
    int iFlashDifferentLane;            /**< 闪光灯分车道闪 0:不分车道 1: 分车道 */
    int iFlashDifferentLaneExt;         /**< 闪光灯分车道闪(硬触发) 0:不分车道 1: 分车道 */

    char szNetDiskIP[32];               /**< 网络存储的IP地址 */
    char szFtpRoot[32];                 /**< 网络存储的FTP根路径 */
    char szNetDiskUser[32];             /**< 网络存储的用户名称 */
    char szNetDiskPwd[32];		        /**< 网络存储的用户密码 */
    int  iNetDiskSpace;   				/**< 网络存储的容量,单位G */
    int  iDiskType;				        /**< 网络硬盘类型 */
    char szNFS[32];                     /**< 网络硬盘NFS路径 */
    int  iAvgY;                         /**< 晚上亮度阈值*/
    int  rgiDefGamma[8][2];             /**< 默认Gama值*/
    int  rgiGamma[8][2];
    int  iSaveSafeType;                 /**< 固态硬盘存储方式 0:没有客户端连接时才存储 1:一直存储 */

    int iX;                             /**< 字符叠X坐标 */
    int iY;                             /**< 字符叠y坐标 */
    int iFontSize;                      /**< 字体大小(注：此大小为点阵字体规格，如规格为24*24点阵，则该值为24) */
    int iDateFormat;                    /**< 日期格式 */
    int iFontColor;                     /**< 字体颜色(RGB) */
    int iLatticeLen;                    /**< 点阵长度 */
    char* pbLattice;                    /**< 点阵缓冲区*/
    int iEnableCharacterOverlap;        /**< 字符叠加使能 */
    int iEnableFixedLight;              /**< 字体颜色是否固定亮度 */

    int iEnableAutoCapture;
    int iShutterCloudy;
    int iGainCloudy;
    int iShutterDay;
    int iGainDay;
    int iShutterBLight;
    int iGainBLight;
    int iShutterFLight;
    int iGainFLight;
    int iShutterNeight;
    int iGainNeight;

    // A67
    int iEnableSharpen;                 /**< 使能锐化开关 0: 不使能  1：使能 */
    int iSharpenThreshold;              /**< 锐化阀值[0~100] */
    int iSharpenCapture;                /**< 锐化抓拍图：1：锐化抓拍图 0：锐化所有图像*/
    int iContrastValue;                 /**< 对比度值[-100~100] */
    int iSaturationValue;               /**< 饱和度值[-100~100] */

    int iAWBRawRGain;
    int iAWBRawGbGain;
    int iAWBRawGrGain;
    int iAWBRawBGain;
    // A67

    BOOL fEnableH264BrightnessAdjust;
    int iAdjustPointX;
    int iAdjustPointY;

    _CamAppParam()
    {
        iSendType = 0;
        iJpegStream = 1;
        iH264Stream = 0;

        iIFrameInterval = 10;
        iTargetBitRate = 6*1024*1024;
        iFrameRate = 12;

        iAGCTh = 100;
        iAGCShutterLOri = 100;
        iAGCShutterHOri = 4200;
        iAGCGainLOri = g_cCameraController.Gain_Raw2Camyu(GAIN_BASELINE) + 10;
        iAGCGainHOri = 220;

        iGainR = 0;
        iGainG = 0;
        iGainB = 0;
        iGain = iAGCGainLOri;
        iShutter = 2000;

        iCaptureGainR = 0;
        iCaptureGainG = 0;
        iCaptureGainB = 0;
        iCaptureGain = iAGCGainLOri;
        iCaptureShutter = 2000;

        iEnableCaptureGainRGB = 0;
        iEnableCaptureGain = 0;
        iEnableCaptureShutter = 0;

        iJpegCompressRate = 60;
        iJpegCompressRateCapture = 80;

        fIsSafeSaver = FALSE;
        fSaveVideo = FALSE;
        fInitHdd = FALSE;
        iFileSize = 512;
        strcpy(szNFSParam, "-o timeo=1,soft,tcp,nolock,retry=1");
        memset(szSafeSaverInitStr, 0, sizeof(szSafeSaverInitStr));

        fIsSideInstall = FALSE;
        iJpegExpectSize = 0;
        iJpegCompressRateL = 30;
        iJpegCompressRateH = 90;

        iOutputFrameRate = 15;
        iJpegType = 0;
        iAGCEnable = 0;
        iAWBEnable = 1;
        iFlashRateSynSignalEnable = 1;
        iCaptureSynSignalEnable = 1;
        iENetSyn = 0;
        iCaptureEdge = 0;
        iFlashDifferentLane = 0;
        iFlashDifferentLaneExt = 0;

        strcpy(szNetDiskIP, "172.18.10.10");
        strcpy(szFtpRoot, "/array1");
        strcpy(szNetDiskUser, "No User");
        strcpy(szNetDiskPwd, "123");
        strcpy(szNFS, "/volume1/nfs");
        iNetDiskSpace = 1;
        iDiskType = 0;
        iSaveSafeType = 0;

        iAvgY = 30;
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

        memcpy(rgiDefGamma, rgiGamma, 64);

        iX = 0;
        iY = 0;
        iFontSize = 24;             // 注：默认是24点阵
        iDateFormat = 0;
        iFontColor = 0x00ff0000;    // 注：临时将默认值改为红色(0x00ff0000)
        iLatticeLen = 0;
        pbLattice = NULL;
        iEnableCharacterOverlap = 0;
        iEnableFixedLight = 0;      // 注：默认情况，字体随场景亮度而改变

        iEnableAutoCapture = 0;
        iShutterCloudy = 600;
        iGainCloudy = 140;
        iShutterDay = 400;
        iGainDay = 110;
        iShutterBLight = 500;
        iGainBLight = 140;
        iShutterFLight = 400;
        iGainFLight = 110;
        iShutterNeight = 800;
        iGainNeight = 160;

        iEnableSharpen = 0;
        iSharpenThreshold = 7;
        iSharpenCapture = 1;
        iContrastValue = 0;
        iSaturationValue = 0;

        iAWBRawRGain = 16384;
        iAWBRawGbGain = 16384;
        iAWBRawGrGain = 16384;
        iAWBRawBGain = 16384;

        fEnableH264BrightnessAdjust = FALSE;
        iAdjustPointX = 10;
        iAdjustPointY = 25;
    };

} CamAppParam;

#endif // WIN32

#endif // _CAMERACONTROL_H_
