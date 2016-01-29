#ifndef CSWCAMERALCONTROLMSG_H
#define CSWCAMERALCONTROLMSG_H
#include "SWObject.h"
#include "SWMessage.h"
#include "SWAutoLock.h"
#include "SWAgcAwbTask.h"
#include "SWCaptureTask.h"
#include "SWChannelDifferenceCorrectTask.h"
#include "SWImageQualitySet.h"
#include "SWCameraControlParameter.h"
#include "SWExtDevControl.h"


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
    * @brief MSG_GC 手动增益指令
    * @param [in] wParam 增益值(如200)
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetAGCGain(WPARAM wParam,LPARAM lParam);

    /**
    * @brief MSG_AGC 自动增益指令
    * @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
    * @param [in] 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetAGCEnable(WPARAM wParam,LPARAM lParam);
    
    HRESULT OnGetAGCEnable(WPARAM wParam,LPARAM lParam);

	/**
    * @brief MSG_SET_AGCZONE 设备AGC测光区域
    * @param [in] wParam AGC测光区域使能标志INT数据，16个数据。
    * @param [in] 保留
    * @return 成功返回S_OK，其他值代表失败
    */
	HRESULT OnSetAGCZone(WPARAM wParam,LPARAM lParam);

    /**
    * @brief MSG_GET_GC 获取当前增益
    * @param [in] wParam 保留
    * @param [out] lParam（INT*类型） 增益返回值(200)
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnGetAGCGain(WPARAM wParam,LPARAM lParam);
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
    * @brief MSG_WB 设置RGB增益
    * @param [in] wParam RGB增益(0到7位为B通道值，8到15位G通道值，16到23为R通道值,例如:0x00AFAAFF
    * ,分别为FF为B通道增益，AA为G通道增益，AF为R通道增益值)
    * 如是使能消息，则为使能标记0=不使能，1 = 使能
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetRGBGain(WPARAM wParam,LPARAM lParam);

    /**
    * @brief MSG_AWB 自动白平衡指令
    * @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
    * @param [in] 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetAWBEnable(WPARAM wParam,LPARAM lParam);

    /**
    * @brief MSG_GET_AWBENABLE 获取自动白平衡使能开关
    * @param [in] wParam 保留
    * @param [out] lParam(INT*类型) 使能消息，使能标记，0=不使能，1 = 使能
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnGetAWBEnable(WPARAM wParam,LPARAM lParam);
    /**
    * @brief MSG_GET_WB 获取当前RGB增益
    * @param [in] wParam 保留
    * @param [out] lParam(DWORD*类型) RGB增益(0到7位为B通道值，8到15位G通道值，16到23为R通道值,例如:0x00AFAAFF
    * ,分别为FF为B通道增益，AA为G通道增益，AF为R通道增益值)
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnGetRGBGain(WPARAM wParam,LPARAM lParam);

    /**
    * @brief MSG_GAMMA 伽玛设置指令
	* @param [in] wParam 传入Gamma值列表指针Gamma[8][2]
	* @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetGamma(WPARAM wParam,LPARAM lParam);


    /**
    * @brief MSG_DIFFCORR 差异性校正使能指令
    * @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetDiffCorrEnable(WPARAM wParam,LPARAM lParam);


    /**
    * @brief MSG_CAPTURE_ENABLE 抓拍差异性校正使能
    * @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */

    HRESULT OnSetCaptureEnable(WPARAM wParam,LPARAM lParam);


    /**
    * @brief MSG_CAPTURERGB_ENABLE 抓拍RGB使能
    * @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetCaptureRGBEnable(WPARAM wParam,LPARAM lParam);


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
    * @brief MSG_SET_SOFT_TOUCH,设置抓拍软触发
	* @param [in] wParam 高1字节为触发的闪光灯组合（目前支持3路闪光灯，从低位开始
	* ，第一位对应第一路闪光灯，第二位对应第二路闪光灯……该位为1则表示启用该路闪光灯）
	* @param [in] wParam 低3字节为抓拍信息（对应的抓拍图会带有此信息，可用于抓拍图的标识）
	* ，硬触发的帧号为累加方式
    *  例子: 0x000000013   1为闪光灯信息，3为抓拍信息
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetCaptureSofttouch(WPARAM wParam,LPARAM lParam);

    /**
    * @brief MSG_SET_MAXSHU_GAIN,设置设备信息
    * @param [in] wParam 高16位最大快门
    * @param [in] wParam 低16位为最大增益
    *  例子: 0x100300C8,1003为最大快门，00C8为最大增益
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetAGCMaxShuAndGain(WPARAM wParam,LPARAM lParam);

    /**
    * @brief MSG_GET_MAXSHU_GAIN,设置设备信息
    * @param [in] wParam 保留
    * @param [in] lParam lParam(DWORD*类型) 高16位最大快门
    * @param [in] lParam lParam(DWORD*类型) 低16位为最大增益
    *  例子: 0x100300C8,1003为最大快门，00C8为最大增益
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnGetAGCMaxShuAndGain(WPARAM wParam,LPARAM lParam);

    /**
    * @brief MSG_SET_MINSHU_GAIN,设置设备信息
    * @param [in] wParam 高16位最小快门
    * @param [in] wParam 低16位为最小增益
    *  例子: 0x100300C8,1003为最大快门，00C8为最大增益
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetAGCMinShuAndGain(WPARAM wParam,LPARAM lParam);

    /**
    * @brief MSG_GET_MINSHU_GAIN,设置设备信息
    * @param [in] wParam 保留
    * @param [in] lParam lParam(DWORD*类型) 高16位最小快门
    * @param [in] lParam lParam(DWORD*类型) 低16位为最小增益
    *  例子: 0x100300C8,1003为最大快门，00C8为最大增益
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnGetAGCMinShuAndGain(WPARAM wParam,LPARAM lParam);



	HRESULT OnSetAGCParam(WPARAM wParam,LPARAM lParam);

     /**
    * @brief MSG_SET_DCAPERTUREENABLE,使能DC光圈
    * @param [in] wParam 1为使能，0为非使能
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetDCApertureEnable(WPARAM wParam,LPARAM lParam);

    /**
    * @brief MSG_SET_SHARPENMODE,WDR强度值
    * @param [in] wParam 1为关闭锐化，2为锐化抓拍，3锐化所有帧
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetSharpeMode(WPARAM wParam,LPARAM lParam);


    /**
    * @brief MSG_SET_SHARPENMODE,使能锐化
    * @param [in] wParam 1为关闭锐化，2为锐化抓拍，3锐化所有帧
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetSharpeMode(WPARAM wParam,LPARAM lParam);

    /**
    * @brief MSG_SET_SHARPENTHRESHOLD,设置锐化阀值
    * @param [in] wParam 锐化阀值[0~100]，推荐使用7
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetSharpeThreshold(WPARAM wParam,LPARAM lParam);


    /**
    * @brief MSG_SET_COLORMATRIXMODE,使能图像增强
    * @param [in] wParam 1为关闭图像增强，2为图像增强抓拍，3图像增强所有帧
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetColoMaxtrixMode(WPARAM wParam,LPARAM lParam);

    /**
    * @brief MSG_SET_CONTRASTTHRESHOLD,设置对比度
    * @param [in] wParam 对比度[-100~100]
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetContrastThreshold(WPARAM wParam,LPARAM lParam);

    /**
    * @brief MSG_SET_SATURATIONTHRESHOLD,设置饱和度
    * @param [in] wParam 对比度[-100~100]
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetSaturationThreshold(WPARAM wParam,LPARAM lParam);


    /**
    * @brief MSG_SET_SATURATIONTHRESHOLD,设置色温
    * @param [in] wParam 色温5个级别[32,40,52,55,60,70]
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetTemperatureThreshold(WPARAM wParam,LPARAM lParam);
    /**
    * @brief MSG_SET_HUETHRESHOLD,设置色度
    * @param [in] wParam 对比度[-180~180]
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetHueThreshold(WPARAM wParam,LPARAM lParam);

    /**
    * @brief MSG_SET_HAZEREMOVALMODE,使能除雾模式
    * @param [in] wParam 1为关闭除雾，2为除雾抓拍，3为除雾所有帧
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetHazeremovalMode(WPARAM wParam,LPARAM lParam);

    /**
    * @brief MSG_SET_HUETHRESHOLD,设置除雾阀值
    * @param [in] wParam 除雾阀值[0~10]
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetHazeremovalThreshold(WPARAM wParam,LPARAM lParam);

	/**
    * @brief 设置频闪信号使能。
    * @param [in] wParam 0或1
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetFlashRateEnable(WPARAM wParam,LPARAM lParam);

	 /**
    * @brief 设置抓拍信号使能
    * @param [in] wParam 0或1
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetCaptrueSyncEnable(WPARAM wParam,LPARAM lParam);

	/**
    * @brief 设置频闪脉宽
    * @param [in] wParam 脉宽 100~20000，单位微秒
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetFlashRatePulse(WPARAM wParam,LPARAM lParam);


	/**
    * @brief MSG_SET_FILTERSWITCH 滤光片切换
    * @param [in] wParam 1正常滤光片，2红外滤光片
    * @param [in] lParam 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetFilterSwitch(WPARAM wParam,LPARAM lParam);

	/**
    * @brief MSG_SET_REDLIGHT_RECT 设置红灯加红区域
    * @param [in] wParam SW_RECT*
    * @param [in] lParam INT 红灯加红区域数。
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetRedLightRect(WPARAM wParam,LPARAM lParam);

	/**
    * @brief MSG_SET_GAMMA_MODE 设置GAMMA使能模式
    * @param [in] wParam INT 通道选择值：0：6467和368都无GAMMA，1：6467有368无，2：6467无368有，3：6467有368有
    * @param [in] 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetGammaMode(WPARAM wParam,LPARAM lParam);

	/**
    * @brief MSG_SET_GAMMA_MODE 设置GAMMA使能模式
    * @param [in] 保留
    * @param [out] INT* 返回GAMMA使能模式
    * @return 成功返回S_OK，其他值代表失败
    */
	HRESULT OnGetGammaMode(WPARAM wParam,LPARAM lParam);

	/**
    * @brief MSG_SET_CAPTUREEDGE 设置触发抓拍沿
    * @param [in] wParam 0~3
    * @param [out] 保留
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnSetCaptureEdge(WPARAM wParam,LPARAM lParam);

    /**
    * @brief MSG_GET_CAPTUREEDGE 获取触发抓拍沿
    * @param [in] 保留
    * @param [out] lParam 0~3
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnGetCaptureEdge(WPARAM wParam,LPARAM lParam);

     /**
    * @brief MSG_GET_BARRIER_STATE 获取栏杆机状态
    * @param [in] 保留
    * @param [out] lParam : 栏杆机状态
    * @return 成功返回S_OK，其他值代表失败
    */
    HRESULT OnGetBarrierState(WPARAM wParam,LPARAM lParam);

	 /**
     *@brief 获取前端相机云台配置
     *@param [PVOID] pvBuffer PTZ控制参数
     *@param [PVOID] iSize 参数大小
     *@return 成功返回S_OK,其他值为错误代码
     */
	HRESULT OnGetPTZInfo(PVOID pvBuffer, INT iSize);

     //消息映射宏定义
    SW_BEGIN_MESSAGE_MAP(CSWCameralControlMSG, CSWMessage)
        //初始化模块
        SW_MESSAGE_HANDLER(MSG_INITIAL_PARAM, OnInitialCameralParam)

        // 设置或获取AGC的门限值
        SW_MESSAGE_HANDLER( MSG_SET_AGCTH , OnSetAGCTh)
        SW_MESSAGE_HANDLER( MSG_GET_AGCTH , OnGetAGCTh)

		SW_MESSAGE_HANDLER( MSG_SET_AGCZONE , OnSetAGCZone)

        // 设置AGC增益及AGC使能等
        SW_MESSAGE_HANDLER(MSG_SET_AGCGAIN , OnSetAGCGain )
        SW_MESSAGE_HANDLER(MSG_GET_AGCGAIN , OnGetAGCGain );
        SW_MESSAGE_HANDLER(MSG_SET_AGCENABLE , OnSetAGCEnable )
        SW_MESSAGE_HANDLER(MSG_GET_AGCENABLE , OnGetAGCEnable )
		
        // 设置获取快门
        SW_MESSAGE_HANDLER(MSG_SET_SHUTTER , OnSetShutter );
        SW_MESSAGE_HANDLER(MSG_GET_SHUTTER , OnGetShutter);
        // 白平衡功能
        SW_MESSAGE_HANDLER(MSG_SET_RGBGAIN , OnSetRGBGain );
        SW_MESSAGE_HANDLER(MSG_GET_RGBGAIN , OnGetRGBGain );
        SW_MESSAGE_HANDLER(MSG_SET_AWBENABLE , OnSetAWBEnable );
        SW_MESSAGE_HANDLER(MSG_GET_AWBENABLE , OnGetAWBEnable );
        // GAMMA设置
        SW_MESSAGE_HANDLER(MSG_SET_GAMMA , OnSetGamma );
		SW_MESSAGE_HANDLER(MSG_SET_GAMMA_MODE , OnSetGammaMode );
		SW_MESSAGE_HANDLER(MSG_GET_GAMMA_MODE , OnGetGammaMode );

        // 差异性矫正使能
        SW_MESSAGE_HANDLER(MSG_SET_DIFFCORRENABLE , OnSetDiffCorrEnable )


        // 抓拍RGB使能
        SW_MESSAGE_HANDLER(MSG_CAPTURERGB_ENABLE , OnSetCaptureRGBEnable );
        // 抓拍快门使能
        SW_MESSAGE_HANDLER(MSG_CAPTURESHUTTER_ENABLE , OnSetCaptureShutterEnable );
        // 抓拍增益使能
        SW_MESSAGE_HANDLER(MSG_CAPTUREGAIN_ENABLE , OnSetCaptureGainEnable );

        // 抓拍差异性使能
        SW_MESSAGE_HANDLER(MSG_SET_CAPTURE_ENABLE , OnSetCaptureEnable );

		 // 抓拍快门
        SW_MESSAGE_HANDLER(MSG_SET_CAPTURESHUTTER , OnSetCaptureShutter );
        SW_MESSAGE_HANDLER(MSG_GET_CAPTURESHUTTER , OnGetCaptureShutter );
        // 抓拍增益
        SW_MESSAGE_HANDLER(MSG_SET_CAPTRUEGAIN , OnSetCaptureGain )
        SW_MESSAGE_HANDLER(MSG_GET_CAPTUREGAIN , OnGetCaptureGain );

        // 抓拍白平衡
        SW_MESSAGE_HANDLER(MSG_SET_CAPTRUERGBGAIN , OnSetCaptureRGBGain )
        SW_MESSAGE_HANDLER(MSG_GET_CAPTRUERGBGAIN , OnGetCaptureRGBGain );
        // 抓拍软触发
        SW_MESSAGE_HANDLER(MSG_SET_SOFT_TOUCH , OnSetCaptureSofttouch );

        // 设置和获取AGC最大快门及最大增益
        SW_MESSAGE_HANDLER( MSG_SET_MAXSHU_GAIN , OnSetAGCMaxShuAndGain);
        SW_MESSAGE_HANDLER( MSG_GET_MAXSHU_GAIN , OnGetAGCMaxShuAndGain);

        // 设置和获取AGC最小快门及最大增益
        SW_MESSAGE_HANDLER( MSG_SET_MINSHU_GAIN , OnSetAGCMinShuAndGain);
        SW_MESSAGE_HANDLER( MSG_GET_MINSHU_GAIN , OnGetAGCMinShuAndGain);

		SW_MESSAGE_HANDLER( MSG_SET_AGCPARAM , OnSetAGCParam);

		// 锐化
		SW_MESSAGE_HANDLER(MSG_SET_SHARPENMODE,OnSetSharpeMode);
		SW_MESSAGE_HANDLER(MSG_SET_SHARPENTHRESHOLD,OnSetSharpeThreshold);

		// 图像增强
		SW_MESSAGE_HANDLER(MSG_SET_COLORMATRIXMODE, OnSetColoMaxtrixMode);
		// 红灯加红
		SW_MESSAGE_HANDLER(MSG_SET_REDLIGHT_RECT, OnSetRedLightRect);

		SW_MESSAGE_HANDLER(MSG_SET_CONTRASTTHRESHOLD, OnSetContrastThreshold);
		SW_MESSAGE_HANDLER(MSG_SET_SATURATIONTHRESHOLD, OnSetSaturationThreshold);
		SW_MESSAGE_HANDLER(MSG_SET_TEMPERATURETHRESHOLD, OnSetTemperatureThreshold);
		SW_MESSAGE_HANDLER(MSG_SET_HUETHRESHOLD, OnSetHueThreshold);

		// 除雾功能
		SW_MESSAGE_HANDLER(MSG_SET_HAZEREMOVALMODE, OnSetHazeremovalMode);
		SW_MESSAGE_HANDLER(MSG_SET_HAZEREMOVALTHRESHOLD, OnSetHazeremovalThreshold);

		// IO功能
		SW_MESSAGE_HANDLER(MSG_SET_FLASHRATE_ENABLE, OnSetFlashRateEnable);
		SW_MESSAGE_HANDLER(MSG_SET_CAPTURESYNC_ENABLE, OnSetCaptrueSyncEnable);
		SW_MESSAGE_HANDLER(MSG_SET_FLASHRATE_PULSE, OnSetFlashRatePulse);


		// 外部设备设置
		SW_MESSAGE_HANDLER(MSG_SET_FILTERSWITCH , OnSetFilterSwitch);

		SW_MESSAGE_HANDLER(MSG_SET_DCAPERTUREENABLE , OnSetDCApertureEnable);
		
		// 触发抓拍沿
		SW_MESSAGE_HANDLER(MSG_SET_CAPTUREEDGE, OnSetCaptureEdge);
		SW_MESSAGE_HANDLER(MSG_GET_CAPTUREEDGE, OnGetCaptureEdge);

		SW_MESSAGE_HANDLER(MSG_GET_BARRIER_STATE, OnGetBarrierState);


    SW_END_MESSAGE_MAP()

	//远程消息映射函数
    SW_BEGIN_REMOTE_MESSAGE_MAP(CSWCameralControlMSG, CSWMessage)
		SW_REMOTE_MESSAGE_HANDLER(MSG_APP_REMOTE_GET_PTZ_INFO, OnGetPTZInfo)
    SW_END_REMOTE_MESSAGE_MAP();
	
private:
    // 差异矫正差异值矫正锁
    CSWMutex m_objSWMutex;
    // 增益及白平衡任务
    CSWAgcAwbTask *m_pSWAgcAwbTask;
    // 抓拍信息
    CSWCaptureTask *m_pSWCapture;
    // 差异性矫正任务
    CSWChannelDifferenceCorrectTask *m_pSWChanneCorrTask;
    // 图像增强等功能任务
    CSWImageQualitySet *m_pSWImageQualitySet;
    // 外部设备对象
    CSWExtDevControl *m_pSWExtDevControl;
};
REGISTER_CLASS(CSWCameralControlMSG)
#endif // CSWCAMERALCONTROLMSG_H

