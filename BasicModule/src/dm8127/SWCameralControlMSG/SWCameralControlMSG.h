#ifndef CSWCAMERALCONTROLMSG_H
#define CSWCAMERALCONTROLMSG_H
#include "SWObject.h"
#include "SWMessage.h"
#include "SWCameraControlParameter.h"


class CSWCameralControlMSG : public CSWMessage, public CSWObject
{
		CLASSINFO(CSWCameralControlMSG, CSWObject)
public:
    CSWCameralControlMSG();
    virtual ~CSWCameralControlMSG();

protected:
    /**
     * @brief MSG_INITIAL_PARAM 设置设置相机初始化参数结构体
     * @param [in] wParam (CAMERAPARAM_INFO* 结构指针类型）
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnInitialCameralParam( WPARAM wParam,LPARAM lParam );

    /**
     * @brief MSG_SET_SHU_RANGE 设置AGC曝光时间范围
     * @param [in] wParam 曝光范围DWORD类型：4字节,0~15：最小曝光时间 16~32：最大曝光时间
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetAGCShutterRange( WPARAM wParam,LPARAM lParam );
    /**
     * @brief MSG_GET_SHU_RANGE 获取AGC曝光时间范围
     * @param [in] wParam 保留
     * @param [out] lParam 曝光范围DWORD类型：4字节,0~15：最小曝光时间 16~32：最大曝光时间
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetAGCShutterRange( WPARAM wParam,LPARAM lParam );

    /**
     * @brief MSG_SET_GAIN_RANGE 设置AGC增益范围
     * @param [in] wParam DWORD类型：4字节，0~15：最小增益时间 16~32：最大增益时间
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetAGCGainRange( WPARAM wParam,LPARAM lParam );

    /**
     * @brief MSG_SET_AGC_SCENE 设置AGC情景模式
     * @param [in] wParam DWORD类型：4字节，0：自动, 1: 情景1, 2: 情景2
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
	HRESULT OnSetAGCScene( WPARAM wParam,LPARAM lParam );

	/**
     * @brief MSG_GET_AGC_SCENE 获取AGC情景模式
     * @param [in] wParam 保留
     * @param [in] lParam DWORD类型：4字节，0：自动, 1: 情景1, 2: 情景2
     * @return 成功返回S_OK，其他值代表失败
     */
	HRESULT OnGetAGCScene( WPARAM wParam,LPARAM lParam );

    /**
     * @brief MSG_GET_GAIN_RANGE 获取AGC增益范围
     * @param [in] wParam 保留
     * @param [out] lParam DWORD类型：4字节0~15：最小增益时间 16~32：最大增益时间
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetAGCGainRange( WPARAM wParam,LPARAM lParam );

    /**
     * @brief MSG_SET_AGCTH 设置AGC图像亮度期望值
     * @param [in] wParam 期望值
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetAGCTh( WPARAM wParam,LPARAM lParam );
    /**
     * @brief MSG_GET_AGCTH 获取AGC图像亮度期望值
     * @param [in] wParam 保留
     * @param [out] lParam （INT*类型） 期望值返回
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetAGCTh( WPARAM wParam,LPARAM lParam );

    /**
     * @brief MSG_SET_AGCGAIN 手动增益指令
     * @param [in] wParam 增益值(如200)
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetAGCGain(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_AGCGAIN 获取当前增益
     * @param [in] wParam 保留
     * @param [out] lParam（INT*类型） 增益返回值(200)
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetAGCGain(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_AGCENABLE 自动增益指令
     * @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
     * @param [in] 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetAGCEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_AGCENABLE 自动增益指令
     * @param [in] wParam 保留
     * @param [in] lParam 使能标记，0=不使能，1 = 使能
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetAGCEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_AGCZONE 设备AGC测光区域
     * @param [in] wParam AGC测光区域使能标志INT数据，16个数据。
     * @param [in] 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetAGCZone(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_AGCZONE 获取AGC测光区域
     * @param [in] wParam 保留
     * @param [in] lParam AGC测光区域使能标志INT数据，16个数据。
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetAGCZone(WPARAM wParam,LPARAM lParam);


    /**
     * @brief MSG_SET_AGCPARAM 获取AGC参数
     * @param [in] wParam  DWORD[4]
     * @param [out] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetAGCParam(WPARAM wParam,LPARAM lParam);
    /**
     * @brief MSG_SET_AGCPARAM 获取AGC参数
     * @param [in] wParam  保留
     * @param [out] lParam DWORD[4]
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetAGCParam(WPARAM wParam,LPARAM lParam);
    /**
     * @brief MSG_SHUTTER 设置快门指令
     * @param [in] wParam 快门值
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetShutter(WPARAM wParam,LPARAM lParam);
    /**
     * @brief MSG_SHUTTER 获取快门指令
     * @param [in] wParam 保留
     * @param [out] lParam（INT*类型） 快门值
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetShutter(WPARAM wParam,LPARAM lParam);


    /**
     * @brief MSG_SET_RGBGAIN 设置RGB增益
     * @param [in] wParam RGB增益(0到7位为r通道值，8到15位G通道值，16到23为B通道值
     * 如是使能消息，则为使能标记0=不使能，1 = 使能
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetRGBGain(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_RGBGAIN 消息接收函数
     * @param [in] wParam 保留
     * @param [out] lParam(DWORD*类型) RGB增益(0到7位为R通道值，8到15位G通道值，16到23为G通道值
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetRGBGain(WPARAM wParam,LPARAM lParam);


    /**
     * @brief MSG_SET_AWBENABLE 自动白平衡指令
     * @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
     * @param [in] 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetAWBEnable(WPARAM wParam,LPARAM lParam);
    /**
     * @brief MSG_GET_AWBENABLE 获取自动白平衡开关
     * @param [in] wParam 保留
     * @param [in] lParam 使能消息，使能标记，0=不使能，1 = 使能
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetAWBEnable(WPARAM wParam,LPARAM lParam);


    /**
     * @brief MSG_SET_DCAPERTUREENABLE,使能自动DC光圈
     * @param [in] wParam 1为使能，0为非使能
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetDCApertureEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_DCAPERTUREENABLE,获取自动DC光圈使能
     * @param [in] wParam  保留
     * @param [in] lParam 1为使能，0为非使能
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetDCApertureEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_ZOOM_DCAPERTURE,放大DC光圈
     * @param [in] wParam 保留
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnZoomDCAperture(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SHRINK_DCAPERTURE,缩小DC光圈
     * @param [in] wParam 保留
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnShrinkDCAperture(WPARAM wParam,LPARAM lParam);



    /**
     * @brief MSG_SET_SHARPENMODE,使能锐化
     * @param [in] wParam 1为关闭锐化，3锐化所有帧
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetSharpeMode(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_SHARPENMODE,获取锐化使能
     * @param [in] wParam 保留
     * @param [in] lParam  1为关闭锐化，3锐化所有帧
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetSharpeMode(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_SHARPEN_ENABLE,设置锐化使能
     * @param [in] wParam 0为关闭锐化，1锐化所有帧
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetSharpeEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_SHARPEN_ENABLE,获取锐化使能
     * @param [in] wParam 保留
     * @param [in] lParam 类型： DWORD*， 0为关闭锐化，1锐化所有帧
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetSharpeEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_SHARPENTHRESHOLD,设置锐化阀值
     * @param [in] wParam 锐化阀值[0~100]，推荐使用7
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetSharpeThreshold(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_SHARPENTHRESHOLD,读取锐化阀值
     * @param [in] wParam  保留
     * @param [in] lParam 锐化阀值[0~100]，推荐使用7
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetSharpeThreshold(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_COLORMATRIX_ENABLE,设置图像增强使能
     * @param [in] wParam 0为关闭，1：打开
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */

    HRESULT OnSetColoMaxtrixEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_COLORMATRIX_ENABLE,获取图像增强使能
     * @param [in] wParam  保留
     * @param [in] lParam 0为关闭，1：打开
     * @return 成功返回S_OK，其他值代表失败
     */

    HRESULT OnGetColoMaxtrixEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_CONTRASTTHRESHOLD,设置对比度
     * @param [in] wParam 对比度[-100~100]
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetContrastThreshold(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_CONTRASTTHRESHOLD,获取对比度
     * @param [in] wParam 保留
     * @param [in] lParam [-100~100]
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetContrastThreshold(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_SATURATIONTHRESHOLD,设置饱和度
     * @param [in] wParam 对比度[-100~100]
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetSaturationThreshold(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_SATURATIONTHRESHOLD,获取饱和度
     * @param [in] wParam 保留
     * @param [in] lParam [-100~100]
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetSaturationThreshold(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_FILTERSWITCH 滤光片切换
     * @param [in] wParam 1正常滤光片，2红外滤光片
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetFilterSwitch(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_FILTERSWITCH 滤光片切换
     * @param [in] wParam 保留
     * @param [in] lParam 1正常滤光片，2红外滤光片
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetFilterSwitch(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_REDLIGHT_ENABLE 设置红灯加红使能
     * @param [in] wParam 0为不使能，1为使能
     * @param [in] lParam 保留。
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetRedLightEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_REDLIGHT_ENABLE 获取红灯加红使能
     * @param [in] wParam 保留
     * @param [in] lParam 0为不使能，1为使能
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetRedLightEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_REDLIGHT_RECT 设置红灯加红区域
     * @param [in] wParam SW_RECT*
     * @param [in] lParam INT 红灯加红区域数。
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetRedLightRect(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_TRAFFIC_LIGTH_LUM_TH 设置红灯加红
     * @param [in] wParam
     * @param [in] lParam 。
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetRedLightThreshold(WPARAM wParam,LPARAM lParam);

	
    /**
     * @brief MSG_SET_REDLIGHT_RECT 设置绿灯加绿区域
     * @param [in] wParam SW_RECT*
     * @param [in] lParam INT 绿灯加红区域数。
     * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetGreenLightRect(WPARAM wParam,LPARAM lParam);
    
    /**
     * @brief MSG_SET_TRAFFIC_GREEN_LIGTH_LUM_TH 设置绿灯加绿
     * @param [in] wParam
     * @param [in] lParam 。
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetGreenLightThreshold(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_BARRIER_STATE 获取栏杆机状态
     * @param [in] 保留
     * @param [out] lParam : 栏杆机状态
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetBarrierState(WPARAM wParam,LPARAM lParam){return S_FALSE;};

    /**
     * @brief MSG_SET_WDR_STRENGTH 设置WDR阈值
     * @param [in] wParam 0为不使能，值越大强度越大。取值范围：0~4095
     * @param [out] 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetWDRStrength(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_WDR_STRENGTH 设置WDR阈值
     * @param [in] wParam 保留
     * @param [out] lParam 0为不使能，值越大强度越大。取值范围：0~4095
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetWDRStrength(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_TNF_ENABLE 设置TNF使能
     * @param [in] wParam 0为不使能，1为使能
     * @param [out] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetTNFEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_TNF_ENABLE 获取TNF使能
     * @param [in] wParam  保留
     * @param [out] lParam 0为不使能，1为使能
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetTNFEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_SNF_ENABLE 设置SNF使能
     * @param [in] wParam 0为不使能，1为使能
     * @param [out] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetSNFEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_SNF_ENABLE 获取SNF使能
     * @param [in] wParam  保留
     * @param [out] lParam 0为不使能，1为使能
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetSNFEnable(WPARAM wParam,LPARAM lParam);


    /**
     * @brief MSG_SET_DENOISE_MODE 设置降噪模式
     * @param [in] wParam 0为不使能，1为使能
     * @param [out] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetDeNoiseMode(WPARAM wParam,LPARAM lParam);
    /**
     * @brief MSG_GET_DENOISE_MODE 获取降噪模式
     * @param [in] wParam  保留
     * @param [out] lParam 0为不使能，1为使能
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetDeNoiseMode(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_DENOISE_STATE 设置降噪状态
     * @param [in] wParam 0~3:自动、低、中、高
     * @param [out] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetDeNoiseState(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_DENOISE_STATE 获取降噪状态
     * @param [in] wParam  保留
     * @param [out] lParam 0~3:自动、低、中、高
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetDeNoiseState(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_AC_SYNC_ENABLE 设置电网同步使能
     * @param [in] wParam 0：不使能，1：使能
     * @param [out] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetACSyncEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_AC_SYNC_ENABLE 获取电网同步使能
     * @param [in] wParam  保留
     * @param [out] lParam 0：不使能，1：使能
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetACSyncEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_AC_SYNC_DELAY 设置电网同步延时
     * @param [in] wParam 范围：0~25500
     * @param [out] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetACSyncDelay(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_AC_SYNC_DELAY 获取电网同步延时
     * @param [in] wParam  保留
     * @param [out] lParam 范围：0~25500
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetACSyncDelay(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_GRAY_IMAGE_ENABLE 设置黑白图使能
     * @param [in] wParam 0：不使能，1：使能
     * @param [out] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetGrayImageEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_GRAY_IMAGE_ENABLE 获取黑白图使能
     * @param [in] wParam  保留
     * @param [out] lParam 0：不使能，1：使能
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetGrayImageEnable(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_CVBS_STD 设置CVBS制式
     * @param [in] wParam 0：PAL，1：NTSC
     * @param [out] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetCVBSStd(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_CVBS_STD 读取CVBS制式
     * @param [in] wParam  保留
     * @param [out] lParam 0：PAL，1：NTSC
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetCVBSStd(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_CAM_TEST 设置相机自动化测试
     * @param [in] wParam 保留
     * @param [out] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetCamTest(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_GAMMA_STRENGTH 设置相机伽玛
     * @param [in] wParam 范围：10～22
     * @param [out] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetGammaStrength(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_GAMMA_STRENGTH 读取相机伽玛
     * @param [in] wParam 保留
     * @param [out] lParam 范围：10～22
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetGammaStrength(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_GAMMA_ENABLE 获取黑白图使能
     * @param [in] wParam  0：不使能，1：使能
     * @param [out] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetGammaEnable(WPARAM wParam,LPARAM lParam);


    /**
     * @brief MSG_GET_GRAY_IMAGE_ENABLE 获取黑白图使能
     * @param [in] wParam  保留
     * @param [out] lParam 0：不使能，1：使能
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetGammaEnable(WPARAM wParam,LPARAM lParam);
    /**
     * @brief MSG_SET_CAM_FPGA_REG 设置相机FGPA写
     * @param [in] wParam 范围不限
     * @param [out] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetCamFpgaReg(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_GAM_FPGA_REG 设置相FGPA读
     * @param [in] wParam 保留
     * @param [out] lParam 范围不限
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetCamFpgaReg(WPARAM wParam,LPARAM lParam);


    /**
     * @brief MSG_SET_EDGE_ENHANCE 图像边缘增强
     * @param [in] wParam 0为不使能，1为使能
     * @param [out] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetEdgeEnhance(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_EDGE_ENHANCE 图像边缘增强
     * @param [in] wParam 保留
     * @param [out] lParam 0为不使能，1为使能
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetEdgeEnhance(WPARAM wParam,LPARAM lParam);


    /**
     * @brief MSG_GET_MANUAL_VALUE 获取手动RGB 快门增益
     * @param [in] wParam 保留
     * @param [out] lParam
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetManualParamValue(WPARAM wParam,LPARAM lParam);


    /**
     * @brief 获取相机工作状态
     * @param [in] wParam 保留
     * @param [out] lParam
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetCameraWorkState(WPARAM wParam,LPARAM lParam);


	 /**
	 * @brief MSG_SET_LED_SWITCH 设置补光灯开关
	 * @param [in] wParam 保留
	 * @param [out] lParam
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnSetLEDSwitch(WPARAM wParam,LPARAM lParam);


	 /**
	 * @brief MSG_GET_LED_SWITCH 获取补光灯开关
	 * @param [in] wParam 保留
	 * @param [out] lParam
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnGetLEDSwitch(WPARAM wParam,LPARAM lParam);

	 /**
	 * @brief MSG_SET_TRIGGEROUT_START 触发输出开始
	 * @param [in] wParam 保留
	 * @param [out] lParam
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnSetTriggerOutState(WPARAM wParam,LPARAM lParam);


	 /**
	 * @brief MSG_SET_TRIGGEROUT 设置触发输出
	 * @param [in] polarity output_type 
	 * @param [out] lParam 保留
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnSetTriggerOut(WPARAM wParam,LPARAM lParam);

	 
	 /**
	 * @brief MSG_GET_EXP_IO 读取触发输出口状态
	 * @param [in] 保留
	 * @param [out] polarity output_type
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnGetTriggerOut(WPARAM wParam,LPARAM lParam);

	 /**
	 * @brief MSG_SET_F1_TRIGGEROUT 新的设置触发输出
	 * @param [in] 	DWORD enable;  1 使能 0不使能
	 				DWORD coupling;1 耦合 0不耦合
					DWORD output_type; 输出类型  1OC（开关）  0上拉（电平）
	 				DWORD polarity;1 正极性 0负极性
	 				DWORD pulse_width;脉宽 0~100
	 * @param [out] lParam 保留
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnSetF1TriggerOut(WPARAM wParam,LPARAM lParam);

	 /**
	 * @brief MSG_GET_F1_TRIGGEROUT 读取触发输出口状态
	 * @param [in] 保留
	   @param [out] DWORD enable;  1 使能 0不使能
	 				DWORD coupling;1 耦合 0不耦合
					DWORD output_type; 输出类型  1OC（开关）  0上拉（电平）
	 				DWORD polarity;1 正极性 0负极性
	 				DWORD pulse_width;脉宽 0~100
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnGetF1TriggerOut(WPARAM wParam,LPARAM lParam);

	 /**
	 * @brief MSG_SET_ALM_TRIGGEROUT 新的设置触发输出
	 * @param [in] 	DWORD enable;  1 使能 0不使能
	 				DWORD coupling;1 耦合 0不耦合
					DWORD output_type; 输出类型  1OC（开关）  0上拉（电平）
	 				DWORD polarity;1 正极性 0负极性
	 				DWORD pulse_width;脉宽 0~100
	 * @param [out] lParam 保留
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnSetAlarmTriggerOut(WPARAM wParam,LPARAM lParam);

	 /**
	 * @brief MSG_GET_ALM_TRIGGEROUT 读取触发输出口状态
	 * @param [in] 保留
	   @param [out] DWORD enable;  1 使能 0不使能
	 				DWORD coupling;1 耦合 0不耦合
					DWORD output_type; 输出类型  1OC（开关）  0上拉（电平）
	 				DWORD polarity;1 正极性 0负极性
	 				DWORD pulse_width;脉宽 0~100
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnGetAlarmTriggerOut(WPARAM wParam,LPARAM lParam);

	 /**
	 * @brief MSG_SET_EXP_IO 设置补光灯控制口输出
	 * @param [in] polarity output_type 
	 * @param [out] lParam 保留
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnSetEXPIO(WPARAM wParam,LPARAM lParam);

	 /**
	 * @brief MSG_GET_EXP_IO 读取补光灯控制口状态
	 * @param [in] 保留 
	 * @param [out] polarity output_type
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnGetEXPIO(WPARAM wParam,LPARAM lParam);

    /**
      * @brief MSG_SET_EXP_IO 设置补光灯控制口输出
      * @param [in] polarity output_type
      * @param [out] lParam 保留
      * @return 成功返回S_OK，其他值代表失败
      */
      HRESULT OnSetEXPPluseWidth(WPARAM wParam,LPARAM lParam);

	 /**
	 * @brief MSG_SET_ALARM_IO 设置告警控制口输出
	 * @param [in] polarity output_type 
	 * @param [out] lParam 保留
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnSetALMIO(WPARAM wParam,LPARAM lParam);

	 /**
	 * @brief MSG_GET_ALARM_IO 读取告警控制口状态
	 * @param [in] 保留
	 * @param [out] polarity output_type 
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnGetALMIO(WPARAM wParam,LPARAM lParam);

	 
	 /**
	 * @brief MSG_SET_TG_IO 设置触发抓拍口触发类型
	 * @param [in] 触发沿
	 * @param [out] lParam 保留
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnSetTGIO(WPARAM wParam,LPARAM lParam);

	 /**
	 * @brief MSG_GET_TG_IO 读取触发抓拍口触发类型
	 * @param [in] 保留
	 * @param [out] 触发沿
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnGetTGIO(WPARAM wParam,LPARAM lParam);

	 /**
	 * @brief MSG_SET_COLOR_GRADATION 设置色阶
	 * @param [in] 色阶
	 * @param [int] 保留
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnSetColorGradation(WPARAM wParam,LPARAM lParam);

	 /**
	 * @brief MSG_SET_CAPTURE_ENABLE 设置抓拍
	 * @param [in] 抓拍使能参数
	 * @param [int] 保留
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnSetCaptureEnable(WPARAM wParam,LPARAM lParam);
	 
	 /**
	 * @brief MSG_SET_WORK_MODE 设置工作模式
	 * @param [in] 工作模式
	 * @param [int] 保留
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnSetWorkMode(WPARAM wParam,LPARAM lParam);

	 /**
	 * @brief MSG_SET_AWB_MODE 设置白平衡模式
	 * @param [in] 工作模式
	 * @param [int] 保留
	 * @return 成功返回S_OK，其他值代表失败
	 */
	 HRESULT OnSetAWBMode(WPARAM wParam,LPARAM lParam);

	 /**
	 * @brief MSG_SET_SOFT_TOUCH 设置软触发抓拍
	 */
	 HRESULT OnSetCaptureSofttouch(WPARAM wParam,LPARAM lParam);

	 /**
	* @brief MSG_SET_F1_OUTPUT_TYPE 设置F1输出模式
	* @param [in] F1输出模式
	* @param [int] 保留
	* @return 成功返回S_OK，其他值代表失败
	*/
	 HRESULT OnSetF1OutputType(WPARAM wParam,LPARAM lParam);

	 /**
     *@brief 获取前端相机云台配置
     *@param [PVOID] pvBuffer PTZ控制参数
     *@param [PVOID] iSize 参数大小
     *@return 成功返回S_OK,其他值为错误代码
     */
	HRESULT OnGetPTZInfo(PVOID pvBuffer, INT iSize);

	/**
     * @brief MSG_CAPTURERGB_ENABLE 抓拍RGB使能
     * @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
     HRESULT OnSetCaptureRGBEnable(WPARAM wParam,LPARAM lParam);

	 /**
     * @brief MSG_SET_CAPTURE_AUTO_PARAM_ENABLE 抓拍自动设置参数
     * @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
     HRESULT OnSetCaptureAutoParamEnable(WPARAM wParam,LPARAM lParam);

	 /**
     * @brief MSG_GET_CAPTURE_AUTO_PARAM_ENABLE 获取抓拍自动设置参数
     * @param [in] wParam 保留
     * @param [out] lParam 使能消息，使能标记，0=不使能，1 = 使能
     * @return 成功返回S_OK，其他值代表失败
     */
     HRESULT OnGetCaptureAutoParamEnable(WPARAM wParam,LPARAM lParam);

     /**
     * @brief MSG_CAPTURESHUTTER_ENABLE 抓拍快门使能
     * @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
     HRESULT OnSetCaptureShutterEnable(WPARAM wParam,LPARAM lParam);

     /**
     * @brief MSG_CAPTUREGAIN_ENABLE 抓拍增益使能
     * @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
     HRESULT OnSetCaptureGainEnable(WPARAM wParam,LPARAM lParam);

     /**
      * @brief MSG_CAPTURESHARPEN_ENABLE 抓拍锐化使能
      * @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
      * @param [in] lParam 保留
      * @return 成功返回S_OK，其他值代表失败
      */
      HRESULT OnSetCaptureSharpenEnable(WPARAM wParam,LPARAM lParam);

     /**
     * @brief MSG_CAPTURESHUTTER,设置抓拍快门
     * @param [in] wParam 快门值
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
     HRESULT OnSetCaptureShutter(WPARAM wParam,LPARAM lParam);
     /**
     * @brief MSG_GET_CAPTURESHUTTER, 获取当前抓拍增益
     * @param [in] wParam 保留
     * @param [out] lParam(INT*类型) 快门值
     * @return 成功返回S_OK，其他值代表失败
     */
     HRESULT OnGetCaptureShutter(WPARAM wParam,LPARAM lParam);


     /**
     * @brief MSG_CAPTRUEGAIN, 抓拍增益设置
     * @param [in] wParam 增益值(200)
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
     HRESULT OnSetCaptureGain(WPARAM wParam,LPARAM lParam);
     /**
     * @brief MSG_GET_CAPTUREGAIN,获取抓拍增益
     * @param [in] wParam 保留
     * @param [out] lParam(INT* 类型) 增益值
     * @return 成功返回S_OK，其他值代表失败
     */
     HRESULT OnGetCaptureGain(WPARAM wParam,LPARAM lParam);
     /**
     * @brief MSG_CAPTRUERGBGAIN,设置抓拍RGB增益
     * @param [in] wParam RGB增益(0到7位为B通道值，8到15位G通道值，16到23为R通道值,例如:0x00AFAAFF
     * ,分别为FF为B通道增益，AA为G通道增益，AF为R通道增益值)
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
     HRESULT OnSetCaptureRGBGain(WPARAM wParam,LPARAM lParam);
     /**
     * @brief MSG_GET_CAPTRUERGBGAIN,获取抓拍RGB增益
     * @param [in] wParam 保留
     * @param [out] lParam(DWORD*类型) RGB增益(0到7位为B通道值，8到15位G通道值，16到23为R通道值,例如:0x00AFAAFF
     * ,分别为FF为B通道增益，AA为G通道增益，AF为R通道增益值)
     * @return 成功返回S_OK，其他值代表失败
     */
     HRESULT OnGetCaptureRGBGain(WPARAM wParam,LPARAM lParam);

     /**
      * @brief MSG_SET_CAPTURE_SHARPEN_THR 设置抓拍锐度指令
      * @param [in] wParam 锐度值
      * @param [in] lParam 保留
      * @return 成功返回S_OK，其他值代表失败
      */
     HRESULT OnSetCaptureSharpen(WPARAM wParam,LPARAM lParam);

     /**
        * @brief MSG_GET_CAPTURE_SHARPEN_THR 获取抓拍锐度指令
        * @param [in] wParam 保留
        * @param [out] lParam 锐度值
        * @return 成功返回S_OK，其他值代表失败
        */
     HRESULT OnGetCaptureSharpen(WPARAM wParam,LPARAM lParam);


    /**
     * @brief MSG_GET_M3_DAYNIGHT_STATUS 获取M3白天晚上标识
     * @param [in] wParam 保留
     * @param [out] lParam 标识值
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetM3DayNightStatus(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_WDR_LEVEL 设置WDR等级
     * @param [in] wParam 等级值，0表示关闭
     * @param [out] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetWDRLevel(WPARAM wParam,LPARAM lParam);


	/**
	   @brief 抓拍提亮
	 */
	BOOL OnIncreaseBrightness(VOID);

     //消息映射宏定义
    SW_BEGIN_MESSAGE_MAP(CSWCameralControlMSG, CSWMessage)
        //初始化模块
        SW_MESSAGE_HANDLER(MSG_INITIAL_PARAM, OnInitialCameralParam)

        SW_MESSAGE_HANDLER( MSG_SET_AGCPARAM , OnSetAGCParam)
        SW_MESSAGE_HANDLER( MSG_GET_AGCPARAM , OnGetAGCParam)

        // 设置或获取AGC的门限值
        SW_MESSAGE_HANDLER( MSG_SET_AGCTH , OnSetAGCTh)
        SW_MESSAGE_HANDLER( MSG_GET_AGCTH , OnGetAGCTh)

		SW_MESSAGE_HANDLER( MSG_SET_AGCZONE , OnSetAGCZone)
        SW_MESSAGE_HANDLER( MSG_GET_AGCZONE , OnGetAGCZone)
        // 设置AGC增益及AGC使能等
        SW_MESSAGE_HANDLER(MSG_SET_AGCGAIN , OnSetAGCGain )
        SW_MESSAGE_HANDLER(MSG_GET_AGCGAIN , OnGetAGCGain )
        SW_MESSAGE_HANDLER(MSG_SET_AGCENABLE , OnSetAGCEnable )
        SW_MESSAGE_HANDLER(MSG_GET_AGCENABLE , OnGetAGCEnable )
        // 设置获取曝光时间范围
        SW_MESSAGE_HANDLER(MSG_SET_SHU_RANGE , OnSetAGCShutterRange )
        SW_MESSAGE_HANDLER(MSG_GET_SHU_RANGE , OnGetAGCShutterRange )
        // 设置获取AGC增益范围
        SW_MESSAGE_HANDLER(MSG_SET_GAIN_RANGE , OnSetAGCGainRange )
        SW_MESSAGE_HANDLER(MSG_GET_GAIN_RANGE , OnGetAGCGainRange )

		// 设置AGC情景模式
	 	SW_MESSAGE_HANDLER(MSG_SET_AGC_SCENE , OnSetAGCScene )
	 	SW_MESSAGE_HANDLER(MSG_GET_AGC_SCENE , OnGetAGCScene )
		
        // 设置获取快门
        SW_MESSAGE_HANDLER(MSG_SET_SHUTTER , OnSetShutter )
        SW_MESSAGE_HANDLER(MSG_GET_SHUTTER , OnGetShutter)
        // 白平衡功能
        SW_MESSAGE_HANDLER(MSG_SET_RGBGAIN , OnSetRGBGain )
        SW_MESSAGE_HANDLER(MSG_GET_RGBGAIN , OnGetRGBGain )
        SW_MESSAGE_HANDLER(MSG_SET_AWBENABLE , OnSetAWBEnable )
        SW_MESSAGE_HANDLER(MSG_GET_AWBENABLE , OnGetAWBEnable )


        // 锐化使能
        SW_MESSAGE_HANDLER(MSG_SET_SHARPEN_ENABLE,OnSetSharpeEnable)
        SW_MESSAGE_HANDLER(MSG_GET_SHARPEN_ENABLE,OnGetSharpeEnable)
        // 锐化阈值
		SW_MESSAGE_HANDLER(MSG_SET_SHARPENTHRESHOLD,OnSetSharpeThreshold)
        SW_MESSAGE_HANDLER(MSG_GET_SHARPENTHRESHOLD,OnGetSharpeThreshold)

		// 图像增强
        SW_MESSAGE_HANDLER(MSG_SET_COLORMATRIX_ENABLE, OnSetColoMaxtrixEnable)
        SW_MESSAGE_HANDLER(MSG_GET_COLORMATRIX_ENABLE, OnGetColoMaxtrixEnable)

        // 红灯加红
        SW_MESSAGE_HANDLER(MSG_SET_REDLIGHT_ENABLE, OnSetRedLightEnable)
        SW_MESSAGE_HANDLER(MSG_GET_REDLIGHT_ENABLE, OnGetRedLightEnable)

		SW_MESSAGE_HANDLER(MSG_SET_REDLIGHT_RECT, OnSetRedLightRect)

        SW_MESSAGE_HANDLER(MSG_SET_TRAFFIC_LIGTH_LUM_TH, OnSetRedLightThreshold)

		SW_MESSAGE_HANDLER(MSG_SET_CONTRASTTHRESHOLD, OnSetContrastThreshold)
        SW_MESSAGE_HANDLER(MSG_GET_CONTRASTTHRESHOLD, OnGetContrastThreshold)

		SW_MESSAGE_HANDLER(MSG_SET_SATURATIONTHRESHOLD, OnSetSaturationThreshold)
        SW_MESSAGE_HANDLER(MSG_GET_SATURATIONTHRESHOLD, OnGetSaturationThreshold)

        SW_MESSAGE_HANDLER(MSG_SET_GREENLIGHT_RECT, OnSetGreenLightRect)

        SW_MESSAGE_HANDLER(MSG_SET_TRAFFIC_GREEN_LIGTH_LUM_TH, OnSetGreenLightThreshold)

		// 外部设备设置
		SW_MESSAGE_HANDLER(MSG_SET_FILTERSWITCH , OnSetFilterSwitch)
        SW_MESSAGE_HANDLER(MSG_GET_FILTERSWITCH , OnGetFilterSwitch)


        // DC光圈
		SW_MESSAGE_HANDLER(MSG_SET_DCAPERTUREENABLE , OnSetDCApertureEnable)
        SW_MESSAGE_HANDLER(MSG_GET_DCAPERTUREENABLE , OnGetDCApertureEnable)
        //放大光圈
        SW_MESSAGE_HANDLER(MSG_ZOOM_DCAPERTURE , OnZoomDCAperture)
        //缩小光圈
        SW_MESSAGE_HANDLER(MSG_SHRINK_DCAPERTURE , OnShrinkDCAperture)


		SW_MESSAGE_HANDLER(MSG_GET_BARRIER_STATE, OnGetBarrierState)

        //WDR
        SW_MESSAGE_HANDLER(MSG_SET_WDR_STRENGTH, OnSetWDRStrength)
        SW_MESSAGE_HANDLER(MSG_GET_WDR_STRENGTH, OnGetWDRStrength)
        //降噪
        SW_MESSAGE_HANDLER(MSG_SET_TNF_ENABLE, OnSetTNFEnable)
        SW_MESSAGE_HANDLER(MSG_GET_TNF_ENABLE, OnGetTNFEnable)

        SW_MESSAGE_HANDLER(MSG_SET_SNF_ENABLE, OnSetSNFEnable)
        SW_MESSAGE_HANDLER(MSG_GET_SNF_ENABLE, OnGetSNFEnable)

        SW_MESSAGE_HANDLER(MSG_SET_DENOISE_MODE, OnSetDeNoiseMode )
        SW_MESSAGE_HANDLER(MSG_GET_DENOISE_MODE, OnGetDeNoiseMode )

        SW_MESSAGE_HANDLER(MSG_SET_DENOISE_STATE, OnSetDeNoiseState)
        SW_MESSAGE_HANDLER(MSG_GET_DENOISE_STATE, OnGetDeNoiseState)
        //电网同步
        SW_MESSAGE_HANDLER(MSG_SET_AC_SYNC_ENABLE, OnSetACSyncEnable )
        SW_MESSAGE_HANDLER(MSG_GET_AC_SYNC_ENABLE, OnGetACSyncEnable)

        SW_MESSAGE_HANDLER(MSG_SET_AC_SYNC_DELAY, OnSetACSyncDelay )
        SW_MESSAGE_HANDLER(MSG_GET_AC_SYNC_DELAY, OnGetACSyncDelay)

        //黑白图
        SW_MESSAGE_HANDLER(MSG_SET_GRAY_IMAGE_ENABLE, OnSetGrayImageEnable)
        SW_MESSAGE_HANDLER(MSG_GET_GRAY_IMAGE_ENABLE, OnGetGrayImageEnable)

        //CVBS制式
        SW_MESSAGE_HANDLER(MSG_SET_CVBS_STD, OnSetCVBSStd)
        SW_MESSAGE_HANDLER(MSG_GET_CVBS_STD, OnGetCVBSStd)
        //相机自动化测试
        SW_MESSAGE_HANDLER(MSG_SET_CAM_TEST, OnSetCamTest)

        //伽玛
        SW_MESSAGE_HANDLER(MSG_SET_GAMMA_STRENGTH, OnSetGammaStrength)
        SW_MESSAGE_HANDLER(MSG_GET_GAMMA_STRENGTH, OnGetGammaStrength)
        SW_MESSAGE_HANDLER(MSG_SET_GAMMA_ENABLE, OnSetGammaEnable)
        SW_MESSAGE_HANDLER(MSG_GET_GAMMA_ENABLE, OnGetGammaEnable)

        //FPGA调式
        SW_MESSAGE_HANDLER(MSG_SET_CAM_FPGA_REG, OnSetCamFpgaReg)
        SW_MESSAGE_HANDLER(MSG_GET_CAM_FPGA_REG, OnGetCamFpgaReg)

        //图像边缘增强
        SW_MESSAGE_HANDLER(MSG_SET_EDGE_ENHANCE, OnSetEdgeEnhance)
        SW_MESSAGE_HANDLER(MSG_GET_EDGE_ENHANCE, OnGetEdgeEnhance)

        //获取手动参数 快门 增益 R G B
        SW_MESSAGE_HANDLER(MSG_GET_MANUAL_VALUE, OnGetManualParamValue)
        //获取相机工作状态
        SW_MESSAGE_HANDLER(MSG_GET_CAMERA_WORKSTATE, OnGetCameraWorkState)

		//补光灯开关
		SW_MESSAGE_HANDLER(MSG_SET_LED_SWITCH, OnSetLEDSwitch)
		SW_MESSAGE_HANDLER(MSG_GET_LED_SWITCH, OnGetLEDSwitch)

	 	//触发输出(内部命令)
	 	SW_MESSAGE_HANDLER(MSG_SET_TRIGGEROUT_STATE, OnSetTriggerOutState)
		
		//触发输出口
		SW_MESSAGE_HANDLER(MSG_SET_TRIGGEROUT, OnSetTriggerOut)
		SW_MESSAGE_HANDLER(MSG_GET_TRIGGEROUT, OnGetTriggerOut)

		//新的触发接口，可设置FPGA寄存器所有位段
		SW_MESSAGE_HANDLER(MSG_SET_F1_TRIGGEROUT, OnSetF1TriggerOut)
		SW_MESSAGE_HANDLER(MSG_GET_F1_TRIGGEROUT, OnGetF1TriggerOut)
		SW_MESSAGE_HANDLER(MSG_SET_ALM_TRIGGEROUT, OnSetAlarmTriggerOut)
		SW_MESSAGE_HANDLER(MSG_GET_ALM_TRIGGEROUT, OnGetAlarmTriggerOut)

		//补光灯输出口
		SW_MESSAGE_HANDLER(MSG_SET_EXP_IO, OnSetEXPIO)
		SW_MESSAGE_HANDLER(MSG_GET_EXP_IO, OnGetEXPIO)

		//告警输出口
		SW_MESSAGE_HANDLER(MSG_SET_ALARM_IO, OnSetALMIO)
		SW_MESSAGE_HANDLER(MSG_GET_ALARM_IO, OnGetALMIO)

	 	//触发抓拍口
	 	SW_MESSAGE_HANDLER(MSG_SET_TG_IO, OnSetTGIO)
	 	SW_MESSAGE_HANDLER(MSG_GET_TG_IO, OnGetTGIO)

	 	// 设置色阶 0 ： 0~255；1：16~235.
	 	SW_MESSAGE_HANDLER(MSG_SET_COLOR_GRADATION, OnSetColorGradation)

		//设置FPGA抓拍使能
		SW_MESSAGE_HANDLER(MSG_SET_CAPTURE_ENABLE, OnSetCaptureEnable)

	 	//设置工作模式
	 	SW_MESSAGE_HANDLER(MSG_SET_WORK_MODE, OnSetWorkMode)
	 	//设置白平衡模式
	 	SW_MESSAGE_HANDLER(MSG_SET_AWB_MODE, OnSetAWBMode)

        // 抓拍软触发
        SW_MESSAGE_HANDLER(MSG_SET_SOFT_TOUCH, OnSetCaptureSofttouch )

	 	//F1输出类型
        SW_MESSAGE_HANDLER(MSG_SET_F1_OUTPUT_TYPE, OnSetF1OutputType )

        // LED补光灯脉宽
        SW_MESSAGE_HANDLER(MSG_SET_EXP_PLUSEWIDTH, OnSetEXPPluseWidth)


		//抓拍参数自动设置
		SW_MESSAGE_HANDLER(MSG_SET_CAPTURE_AUTO_PARAM_ENABLE , OnSetCaptureAutoParamEnable)
		//获取抓拍参数自动设置
		SW_MESSAGE_HANDLER(MSG_GET_CAPTURE_AUTO_PARAM_ENABLE , OnGetCaptureAutoParamEnable)
        // 抓拍RGB使能
        SW_MESSAGE_HANDLER(MSG_CAPTURERGB_ENABLE , OnSetCaptureRGBEnable )
        // 抓拍快门使能
        SW_MESSAGE_HANDLER(MSG_CAPTURESHUTTER_ENABLE , OnSetCaptureShutterEnable )
        // 抓拍增益使能
        SW_MESSAGE_HANDLER(MSG_CAPTUREGAIN_ENABLE , OnSetCaptureGainEnable )
        // 抓拍锐化使能
        SW_MESSAGE_HANDLER(MSG_CAPTURESHARPEN_ENABLE , OnSetCaptureSharpenEnable )

        // 抓拍快门
        SW_MESSAGE_HANDLER(MSG_SET_CAPTURESHUTTER , OnSetCaptureShutter )
        SW_MESSAGE_HANDLER(MSG_GET_CAPTURESHUTTER , OnGetCaptureShutter )
        // 抓拍增益
        SW_MESSAGE_HANDLER(MSG_SET_CAPTRUEGAIN , OnSetCaptureGain )
        SW_MESSAGE_HANDLER(MSG_GET_CAPTUREGAIN , OnGetCaptureGain )
        // 抓拍白平衡
        SW_MESSAGE_HANDLER(MSG_SET_CAPTRUERGBGAIN , OnSetCaptureRGBGain )
        SW_MESSAGE_HANDLER(MSG_GET_CAPTRUERGBGAIN , OnGetCaptureRGBGain )

        // 抓拍锐度
        SW_MESSAGE_HANDLER(MSG_SET_CAPTURE_SHARPEN_THR, OnSetCaptureSharpen)
        SW_MESSAGE_HANDLER(MSG_GET_CAPTURE_SHARPEN_THR, OnGetCaptureSharpen)

        SW_MESSAGE_HANDLER(MSG_GET_M3_DAYNIGHT_STATUS, OnGetM3DayNightStatus)

        SW_MESSAGE_HANDLER(MSG_SET_WDR_LEVEL, OnSetWDRLevel);

    SW_END_MESSAGE_MAP()

	 //远程消息映射函数
    SW_BEGIN_REMOTE_MESSAGE_MAP(CSWCameralControlMSG, CSWMessage)
		SW_REMOTE_MESSAGE_HANDLER(MSG_APP_REMOTE_GET_PTZ_INFO, OnGetPTZInfo)
    SW_END_REMOTE_MESSAGE_MAP();
	 
protected:
        INT m_iManualShutter;
        INT m_iManualGain;
        INT m_iManualGainR;
        INT m_iManualGainG;
        INT m_iManualGainB;
        INT m_FilterStatus;

		enum FPGA_IO_TYPE
		{
			E_NORMAL_LED,	//类型：0-补光灯（地灯）1-告警输出 2-抓拍输出
			E_ALARM_OUT,
			E_TRIGGEROUT,
		};
		
		typedef struct _tFPGA_IO_ARG{
			DWORD type;
			DWORD enable;
			DWORD freq_num;
			DWORD polarity;
			DWORD pulse_width;
			DWORD output_type;
			DWORD coupling;
			_tFPGA_IO_ARG()
				: type(0)
				, enable(0)
				, freq_num(0)
				, polarity(0)
				, pulse_width(0)
				, output_type(0)
				, coupling(0)
			{
			}
		}tFPGA_IO_ARG;

		DWORD m_dwLEDPolarity;		//LED极性
		DWORD m_dwLEDOutputType;	//输出类型
		DWORD m_dwLEDPluseWidth;
		DWORD m_dwTriggerOutPolarity;	//触发输出极性
		DWORD m_dwTriggerOutOutputType;	//触发输出类型
		
		DWORD m_dwAGCScene;			//AGC情景模式

		BOOL m_fIsDayNow;
		BOOL m_fIsM3DayNow;
		BOOL m_fCaptureAutoParamEnable;
		BOOL m_fEnableCaptureRGBGain;
		BOOL m_fEnableCaptureShutter;
		BOOL m_fEnableCaptureGain;
		BOOL m_fEnableCaptureSharpen;

		int m_nCaptureShutter;
		int m_nCaptureGain;
		int m_nCaptureR;
		int m_nCaptureG;
		int m_nCaptureB;
		int m_nCapSharpen;
};
REGISTER_CLASS(CSWCameralControlMSG)
#endif // CSWCAMERALCONTROLMSG_H

