#ifndef __SWCAMERA_CONTROL_PARAMETER_H__
#define __SWCAMERA_CONTROL_PARAMETER_H__
typedef struct _CameraParam_info
{
    INT iDiffCorEnable; 				/**< AGC差异性校正使能 */
    INT iCapiffCorEnable;               /**< 抓拍AGC差异性校正使能 */
    INT iAGCEnable;                     /**< AGC使能 */
    INT iAWBEnable;                     /**< AWB使能 */

    INT iAGCTh;                         /**< AGC期望亮度 */
    INT iAGCShutterLOri;                /**< AGC快门调节下限 */
    INT iAGCShutterHOri;                /**< AGC快门调节上限 */
    INT iAGCGainLOri;                   /**< AGC增益调节下限 */
    INT iAGCGainHOri;                   /**< AGC增益调节上限 */
	INT rgiAGCZone[16];					/**< AGC测光区域 */
	INT iAGCScene;						/**< AGC情景模式 */

    INT iGainR;                         /**< R增益 */
    INT iGainG;                         /**< G增益 */
    INT iGainB;                         /**< B增益 */
    INT iGain;                          /**< 相机增益 */
    INT iShutter;                       /**< 快门时间 */

	// zhaopy 以下三个参数使能了下面的抓拍相关配置才有效，
	// 这三个都要增加对应的命令。
	INT iCaptureAutoParamEnable;		/**< 使能抓拍自动参数 */
	INT iCaptureRGBEnable;				/**< 使能抓拍RGB独立参数 */
	INT iCaptureShutterEnable;			/**< 使能抓拍独立快门时间 */
	INT iCaptureGainEnable;				/**< 使能抓拍独立增益 */
	INT iCaptureSharpenEnable;          /**< 使能抓拍独立锐度 */

	INT iEnableCaptureDay;              /**< 白天使能抓拍开关 */
	INT iEnableCaptureParamDay;         /**< 白天使能独立抓拍参数开关 */
    INT iCaptureGainR;                  /**< 抓拍R增益 */
    INT iCaptureGainG;                  /**< 抓拍G增益 */
    INT iCaptureGainB;                  /**< 抓拍B增益 */
    INT iCaptureGain;                   /**< 抓拍相机增益 */
    INT iCaptureShutter;                /**< 抓拍快门时间 */
    INT iCaptureSharpenThreshold;       /**< 抓拍锐度值 */

    // DC光圈
    INT iEnableDCAperture;              /**< 使能DC光圈 0: 不使能  1：使能 */
    // 图像增强等
    INT iEnableSharpen;                 /**< 使能锐化开关 0: 不使能  1：使能 */
    INT iSharpenCapture;                /**< 锐化抓拍图：1：锐化抓拍图 0：锐化所有图像*/
    INT iSharpenThreshold;              /**< 锐化阀值[0~100] */

	BOOL fColorMatrixEnable;			/**< 使能图像增强功能，0为关闭，1为打开 */
    INT iColorMatrixMode;				/**< 使能图像增强功能，1 为关闭使能，2为使能抓拍，3为使能所有帧 */
    INT iContrastValue;                 /**< 对比度值[-100~100] */
    INT iSaturationValue;               /**< 饱和度值[-100~100] */
    INT iTemThreshold;              	/**< 色温5个级别[32,40,52,55,60,70] */
    INT iHueThreshold;              	/**< 色度[-180~180] */

    INT iHazeRemovalMode;				/**< 使能图像除雾功能，1 为关闭使能，2为使能抓拍，3为使能所有帧 */
    INT iHazeRemovalThreshold;			/**< 图像除雾阀值[0~10] */


   	// zhaopy
	// 以下参数只需要初始化时设置即可。
	INT iFlashRateSynSignalEnable;      /**< 频闪同步信号使能 */
	INT iFlashRateMultiplication;		    /**< 频闪输出是否倍频，1：倍频输出 */
	INT iFlashRatePolarity;				     /**< 频闪输出极性。1：正，0：负*/
	INT iFlashRatePulseWidth;			    /**< 频闪输出脉宽。100 到 20000. 微秒*/
	INT iFlashRateOutputType;			    /**< 频闪输出类型。1：上拉（电平）, 0:OC(开关) */
	INT iCaptureSynSignalEnable;      /**< 抓拍同步信号使能 */
	INT iCaptureSynPolarity;			   	/**< 抓拍输出极性。1：正，0：负*/
	INT iCaptureSynPulseWidth;				/**< 抓拍输出脉宽。100 到 20000. 微秒*/
	INT iCaptureSynOutputType;				/**< 抓拍输出类型。1：上拉（电平）, 0:OC(开关) */
	INT iCaptureSynCoupling;					/**< 频闪与抓拍是否耦合。 1：耦合，0：不耦合。*/

	// 红绿灯相关参数。
	//
	BOOL fRedLightEnable;		//红灯加红开关
	SW_RECT rgRedLightRect[8];					/**<红灯加红区域 */
	INT  nRedLightCount;
	INT iHThreshold;			// （与黄灯加红有关）HSL空间，色相阈值，范围0-184320，精度为1/512度，即表达范围是0-360度
	INT iLThreshold;			//（与黄灯加红有关）HSL空间，亮度阈值，范围0-16383
	INT iSThreshold;			// （与黄灯加红有关）HSL空间，饱和度阈值，范围0-16383
	INT iColorFactor;			//（与黄灯加红有关）色相乘法因子，范围0-511
	INT iLumLThreshold;		//（与白灯加红有关）亮度阈值，范围0-16383
	INT iLumLFactor;			// （与白灯加红有关）亮度乘法因子，范围0-511
	INT iLumHFactor;			// （与白灯加红有关）色相乘法因子，范围0-511

	// 电网同步参数
	INT iExternalSyncMode;		// 0-不开启;1-开启内部电网同步；2-开启外部电网同步（目前定义从第二路抓拍信号接口接收同步信号）
	INT iSyncRelay;				// 外同步信号延迟：范围0-20000，单位为1us.

	// 硬触发参数
	INT iCaptureEdgeOne;			// 第一路硬触发抓拍沿的工作模式：0-外部下降沿触发；1-外部上升沿触发；2-上升沿下降沿都触发；3-不触发
	INT iCaptureEdgeTwo;			// 第二路硬触发抓拍沿的工作模式：0-外部下降沿触发；1-外部上升沿触发；2-上升沿下降沿都触发；3-不触发
	INT iFlashDifferentLaneExt;     // 是否分车道：0-不分车道，两路硬触发都会使所有闪光灯工作；1-分车道，第一路硬触发只会使第一路闪光灯工作，第二路硬触发只会使第二路闪光灯工作

	// 触发输出
	BOOL fEnableTriggerOut; // 触发输出使能开关
	INT nTriggerOutNormalStatus; // 触发输出常态控制(极性)
	INT nCaptureSynOutputType;		//触发抓拍输出类型

    //wdr强度值
    INT iWDRStrength;
    //降噪
    BOOL fTNFEnable;
    BOOL fSNFEnable;
    INT iDenoiseState;
	//黑白图开关
	BOOL fGrayImageEnable;	//黑白图开关
	//CVBS
    INT iCVBSMode;			//CVBS 模式

    INT iFilterSwitchState;	//滤光片

    INT iSaturationEnable;//饱和度使能
    INT iContrastEnalbe;//对比度使能
    INT iGammaValue;
    INT iGammaEnable;
    INT iEdgeValue;//图像边缘增强

	INT iLEDPolarity;	//补光灯控制极性
	INT iLEDOutputType;		//输出类型
	INT iLEDPluseWidth;  //输出脉宽
	INT iALMPolarity;				/**< 报警输出极性。1：正，0：负*/
	INT iALMOutputType;				/**< 报警输出类型。1：上拉（电平）, 0:OC(开关)*/
	INT iFlashEnable;		//闪光灯使能
	INT iFlashPolarity;		//闪光灯极性 1:正 0：负
    INT iFlashOutputType;	//闪光灯输出类型 1：OC开关 0：上拉电平
    INT iFlashCoupling;		//闪光灯耦合类型  1：耦合 0：不耦合
	INT iFlashPluseWidth;	//闪光灯买宽 0～100
	
	INT iColorGradation;			/**<色阶 0 ： 0~255；1：16~235 */
	INT iF1OutputType;				/**< F1输出类型。1:闪光灯输出信号;0:栏杆机输出信号*/

	INT iCameraWorkMode;					/**<工作模式 0-收费站/纯相机 1-高速路模式 6-电子警察 7-停车场*/
//  INT iSharpenEnable;//锐化使能

	INT iIris;	/**< 光圈设定值，只对木星有效 */
	INT iAutoFocus;	/**< 自动聚焦值，只对木星有效 */
	INT iAWBMode;                     /**< AWB模式，金星木星都使用到 */
	INT iAEMode;                     /**< AE模式 ，只对木星有效*/
	INT iWDREnable;                     /**< WDR开关 ，只对木星有效*/
	INT iNRLevel;                     /**< 降噪等级，只对木星有效*/
}CAMERAPARAM_INFO,*LPCAMERAPARAM_INFO;
#endif
