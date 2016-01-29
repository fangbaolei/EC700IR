
#include "SWFC.h"
#include "SWCameralControlMSG.h"
#include "SWTaskThreadPool.h"
#include "SWPTZParameter.h"

//REGISTER_CLASS(CSWCameralControlMSG)

CSWCameralControlMSG::CSWCameralControlMSG()
: CSWMessage( MSG_CAMERA_CONTROL_START , MSG_CAMERA_CONTROL_END )
, m_pSWAgcAwbTask( new CSWAgcAwbTask())
, m_pSWCapture( new CSWCaptureTask())
, m_pSWChanneCorrTask( new CSWChannelDifferenceCorrectTask( m_pSWAgcAwbTask ))
, m_pSWImageQualitySet( new CSWImageQualitySet( ) )
, m_pSWExtDevControl( new CSWExtDevControl )
{
    // 初始化线程池
    if( !CSWTaskThreadPool::GetNumThreads()){
        CSWTaskThreadPool::SetNumShortTaskThreads( 2 );
        CSWTaskThreadPool::SetNumBlockingTaskThreads( 0 );
        CSWTaskThreadPool::AddThreads( 2 );
    }
}

CSWCameralControlMSG::~CSWCameralControlMSG()
{
    // 释放AGC任务
    if( m_pSWAgcAwbTask != NULL )
        m_pSWAgcAwbTask->Signal( CSWTask::kKillEvent );
    m_pSWAgcAwbTask = NULL;
    // 释放抓拍阴阳面任务
     if( m_pSWCapture != NULL )
        m_pSWCapture->Signal( CSWTask::kKillEvent );
    m_pSWCapture = NULL;
    // 释放视频流阴阳面任务
     if( m_pSWChanneCorrTask != NULL )
        m_pSWChanneCorrTask->Signal( CSWTask::kKillEvent );
    m_pSWChanneCorrTask = NULL;

    if( m_pSWImageQualitySet != NULL )
        delete m_pSWImageQualitySet;
    m_pSWImageQualitySet = NULL;

    if( m_pSWExtDevControl != NULL )
        delete m_pSWExtDevControl;
    m_pSWExtDevControl = NULL;

    // 退出所有任务线程
    CSWTaskThreadPool::StopThreads( );
}
/**
* @brief MSG_INITIAL_PARAM 设置设置相机初始化参数结构体
* @param [in] wParam （CamAppParam*结构指针类型）
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnInitialCameralParam( WPARAM wParam,LPARAM lParam )
{
    LPCAMERAPARAM_INFO pCameraParam = (LPCAMERAPARAM_INFO)wParam;

    if( pCameraParam == NULL ) return S_FALSE;

    // 设置AGC的期望值
    this->OnSetAGCTh( pCameraParam->iAGCTh , 0 );


    // 相机增益
    this->OnSetAGCGain( pCameraParam->iGain , 0 );
    // 相机快门
    this->OnSetShutter( pCameraParam->iShutter , 0 );

    INT nValue = (pCameraParam->iAGCShutterLOri << 16);
    nValue |= (pCameraParam->iAGCGainLOri & 0x0000FFFF);

    // AGC快门调节下限,AGC增益调节下限
    this->OnSetAGCMinShuAndGain( nValue , 0 );

    nValue = (pCameraParam->iAGCShutterHOri << 16);
    nValue |= (pCameraParam->iAGCGainHOri & 0x0000FFFF);

    // AGC快门调节上限,AGC增益调节上限
    this->OnSetAGCMaxShuAndGain( nValue , 0 );

	// AGC 测光区域
	for(int i = 0; i < 16; ++i)
	{
		CSWImageStatisticsInfo::SetImageListInfo((pCameraParam->rgiAGCZone[i] == 1), i);
	}
	
    DWORD parrRGB[3];

    parrRGB[0] = pCameraParam->iGainR;
    parrRGB[1] = pCameraParam->iGainG;
    parrRGB[2] = pCameraParam->iGainB;
    // R增益,G增益,B增益
    this->OnSetRGBGain( (DWORD)parrRGB , 0 );

    // 初始化抓拍信息

    // 抓拍RGB增益使能
    this->OnSetCaptureRGBEnable( pCameraParam->iCaptureRGBEnable , 0 );
    // 抓拍快门使能
    this->OnSetCaptureShutterEnable( pCameraParam->iCaptureShutterEnable, 0 );
    // 抓拍增益使能
    this->OnSetCaptureGainEnable( pCameraParam->iCaptureGainEnable , 0 );

	// 只有使能的情况下才设置
	if( pCameraParam->iCaptureGainEnable == 1 )
	{
		this->OnSetCaptureGain( (WPARAM)&pCameraParam->iCaptureGain , 0);
	}
	if( pCameraParam->iCaptureShutterEnable == 1 )
	{
		this->OnSetCaptureShutter( (WPARAM)&pCameraParam->iCaptureShutter , 0 );
	}

	if( pCameraParam->iCaptureRGBEnable == 1 )
	{
		// 设置抓拍RGB增益
		INT rgRGB[3];
		rgRGB[0] = pCameraParam->iCaptureGainR;
		rgRGB[1] = pCameraParam->iCaptureGainG;
		rgRGB[2] = pCameraParam->iCaptureGainB;
		this->OnSetCaptureRGBGain( (WPARAM)rgRGB ,0);
	}

    // 设置锐化值
    this->OnSetSharpeThreshold( pCameraParam->iSharpenThreshold , 0);
    // 设置对比度
    this->OnSetContrastThreshold( pCameraParam->iContrastValue , 0);
    // 设置 饱和度
    this->OnSetSaturationThreshold( pCameraParam->iSaturationValue , 0);
    // 设置色温
    this->OnSetTemperatureThreshold( pCameraParam->iTemThreshold , 0);
    // 设置色度
    this->OnSetHueThreshold( pCameraParam->iHueThreshold , 0);
    // 设置除雾阀值
    this->OnSetHazeremovalThreshold( pCameraParam->iHazeRemovalThreshold , 0);

    // 使能抓拍差异性校正
    this->OnSetCaptureEnable( pCameraParam->iCapiffCorEnable , 0);
    // 使能DC光圈
    this->OnSetDCApertureEnable( pCameraParam->iEnableDCAperture , 0);
    // 使能AGC、AWB
    this->OnSetAGCEnable( pCameraParam->iAGCEnable , 0 );
    this->OnSetAWBEnable( pCameraParam->iAWBEnable, 0);
    // 差异性校正使能
    this->OnSetDiffCorrEnable(pCameraParam->iDiffCorEnable , 0 );

    // 设置锐化使能模式
    INT nShareMode = pCameraParam->iSharpenCapture == 0 ? 3 : 2;
    nShareMode = pCameraParam->iEnableSharpen == 0 ? 1 : nShareMode;
    this->OnSetSharpeMode( nShareMode , 0);

    // 使能图像增强
    this->OnSetColoMaxtrixMode( pCameraParam->iColorMatrixMode , 0);
    // 使能除雾
    this->OnSetHazeremovalMode( pCameraParam->iHazeRemovalMode , 0);
    // 初始化外部设备
    m_pSWExtDevControl->InitialExtDevParam( pCameraParam );

	// 初始偏光镜为不使能。
	this->OnSetFilterSwitch(1, 0);

    return S_OK;
}

/**
* @brief MSG_CAPTURERGB_ENABLE 抓拍RGB使能
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCaptureRGBEnable(WPARAM wParam,LPARAM lParam)
{
    return m_pSWCapture->SetRGBEnable( wParam );
}

HRESULT CSWCameralControlMSG::OnSetGammaMode(WPARAM wParam,LPARAM lParam)
{
	INT iMode = (INT)wParam;
	INT iRet = swpa_camera_imgproc_set_gamma_mode(iMode);
	return iRet == SWPAR_OK ? S_OK : E_FAIL;
}

HRESULT CSWCameralControlMSG::OnGetGammaMode(WPARAM wParam,LPARAM lParam)
{
	INT* piMode = (INT*)lParam;
	HRESULT hr = S_OK;
	if( piMode != NULL )
	{
		INT iRet = swpa_camera_imgproc_get_gamma_mode(piMode);
		hr = (iRet == SWPAR_OK ? S_OK : E_FAIL);
	}
	else
	{
		hr = E_INVALIDARG;
	}
	return hr;
}

/**
* @brief MSG_CAPTURESHUTTER_ENABLE 抓拍快门使能
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCaptureShutterEnable(WPARAM wParam,LPARAM lParam)
{
    return m_pSWCapture->SetShutterEnable( wParam );
}

/**
* @brief MSG_CAPTURESHUTTER_ENABLE 抓拍增益使能
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCaptureGainEnable(WPARAM wParam,LPARAM lParam)
{
    HRESULT hr = m_pSWCapture->SetGainEnable( wParam );
	// 抓拍增益的使能要与抓拍差异校正同步。
	// 使能抓拍差异性校正
	hr = OnSetCaptureEnable( wParam , 0);

	return hr;
}
/**
* @brief MSG_SET_COLORMATRIXMODE,使能图像增强
* @param [in] wParam 1为关闭图像增强，2为图像增强抓拍，3图像增强所有帧
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetColoMaxtrixMode(WPARAM wParam,LPARAM lParam)
{
    return m_pSWImageQualitySet->SetColorParamMode( wParam );
}
/**
* @brief MSG_SET_SHARPENMODE,锐化使能
* @param [in] wParam 1为关闭锐化，2为锐化抓拍，3锐化所有帧
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetSharpeMode(WPARAM wParam,LPARAM lParam)
{
    m_pSWImageQualitySet->SetSharpenMode( wParam ); return S_OK;
}
/**
* @brief MSG_SET_SHARPENTHRESHOLD,锐化阀值
* @param [in] wParam 锐化阀值[0~100]，推荐使用7
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetSharpeThreshold(WPARAM wParam,LPARAM lParam)
{
    return m_pSWImageQualitySet->SetSharpenParam( wParam );
}
/**
* @brief 消息接收函数
* @param [in] wParam 传入Gamma值列表指针Gamma[8][2]
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetGamma(WPARAM wParam,LPARAM lParam)
{
    SWFC_DEBUG_ARG("(%d,%d)", wParam, lParam);
    m_pSWImageQualitySet->SetGamma( (INT*)wParam ); return S_OK;
}
/**
* @brief MSG_SET_CONTRASTTHRESHOLD,设置对比度
* @param [in] wParam 对比度[-100~100]
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetContrastThreshold(WPARAM wParam,LPARAM lParam)
{
    return m_pSWImageQualitySet->SetImageContrast( wParam );
}

/**
* @brief MSG_SET_SATURATIONTHRESHOLD,设置饱和度
* @param [in] wParam 对比度[-100~100]
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetSaturationThreshold(WPARAM wParam,LPARAM lParam)
{
    return m_pSWImageQualitySet->SetImageSaturation( wParam );
}


/**
* @brief MSG_SET_SATURATIONTHRESHOLD,设置色温
* @param [in] wParam 色温5个级别[32,40,52,55,60,70]
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetTemperatureThreshold(WPARAM wParam,LPARAM lParam)
{
    return m_pSWImageQualitySet->SetImageTemperature( wParam );
}

/**
* @brief MSG_SET_HUETHRESHOLD,设置色度
* @param [in] wParam 对比度[-180~180]
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetHueThreshold(WPARAM wParam,LPARAM lParam)
{
    return m_pSWImageQualitySet->SetImageHue( wParam );
}

/**
* @brief MSG_SET_HAZEREMOVALMODE,使能除雾模式
* @param [in] wParam 1为关闭除雾，2为除雾抓拍，3为除雾所有帧
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetHazeremovalMode(WPARAM wParam,LPARAM lParam)
{
    return m_pSWImageQualitySet->SetHazeRemovalMode( wParam );
}

/**
* @brief MSG_SET_HUETHRESHOLD,设置除雾阀值
* @param [in] wParam 除雾阀值[0~10]
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetHazeremovalThreshold(WPARAM wParam,LPARAM lParam)
{
    return m_pSWImageQualitySet->SetHazeRemovalParam( wParam );
}
/**
* @brief MSG_SET_AGCTH 设置AGC图像亮度期望值
* @param [in] wParam 期望值
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetAGCTh( WPARAM wParam,LPARAM lParam )
{
    m_pSWAgcAwbTask->SetAGCTh( wParam ); return S_OK;
}

/**
* @brief MSG_SET_AGCZONE 设备AGC测光区域
* @param [in] wParam AGC测光区域使能标志INT数据，16个数据。
* @param [in] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetAGCZone(WPARAM wParam,LPARAM lParam)
{
	INT* piZone = (INT*)wParam;
	if( piZone != NULL )
	{
		for(int i = 0; i < 16; ++i)
		{
			CSWImageStatisticsInfo::SetImageListInfo((piZone[i] == 1), i);
		}
		SW_TRACE_DEBUG("<CameralControlMSG>set agc zone.\n");
		return S_OK;
	}
	return E_INVALIDARG;
}

/**
* @brief MSG_SET_AGCTH 获取AGC图像亮度期望值
* @param [in] wParam 保留
* @param [out] lParam （INT*类型） 期望值返回
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetAGCTh( WPARAM wParam,LPARAM lParam )
{
    m_pSWAgcAwbTask->GetAGCTh( *((INT*)lParam));return S_OK;
}
/**
* @brief 消息接收函数
* @param [in] wParam 增益值(如200)
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetAGCGain(WPARAM wParam,LPARAM lParam)
{
   SWFC_DEBUG_ARG("(%d,%d)", wParam, lParam);
   m_pSWAgcAwbTask->SetAGCGain(wParam) ;
   return S_OK;
}

/**
* @brief 消息接收函数
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetAGCEnable(WPARAM wParam,LPARAM lParam)
{
	SWFC_DEBUG_ARG("(%d,%d)", wParam, lParam);
    m_pSWAgcAwbTask->SetAGCEnable( wParam );

    if( wParam == 1 ) m_pSWAgcAwbTask->Signal( CSWTask::kStartEvent  );

    return S_OK;
}

HRESULT CSWCameralControlMSG::OnGetAGCEnable(WPARAM wParam,LPARAM lParam)
{
	SWFC_DEBUG_ARG("(%d,%d)", wParam, lParam);
	*(INT *)lParam = m_pSWAgcAwbTask->GetAGCEnable();
	return S_OK;
}

/**
* @brief 消息接收函数
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetAWBEnable(WPARAM wParam,LPARAM lParam)
{
    SWFC_DEBUG_ARG("(%d,%d)", wParam, lParam);
    m_pSWAgcAwbTask->SetAWBEnable( wParam );

    if( wParam == 1 ) m_pSWAgcAwbTask->Signal( CSWTask::kStartEvent  );

    return S_OK;
}

HRESULT CSWCameralControlMSG::OnGetAWBEnable(WPARAM wParam,LPARAM lParam)
{
	SWFC_DEBUG_ARG("(%d,%d)", wParam, lParam);
	*(INT *)lParam = m_pSWAgcAwbTask->GetAWBEnable();
	return S_OK;
}


/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [out] lParam（INT*类型） 增益返回值(200)
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetAGCGain(WPARAM wParam,LPARAM lParam)
{
    m_pSWAgcAwbTask->GetAGCGain( *((INT*)lParam) );

    return S_OK;
}


/**
* @brief 消息接收函数
* @param [in] wParam RGB增益(0到7位为B通道值，8到15位G通道值，16到23为R通道值,例如:0x00AFAAFF
* ,分别为FF为B通道增益，AA为G通道增益，AF为R通道增益值)
* 如是使能消息，则为使能标记0=不使能，1 = 使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetRGBGain(WPARAM wParam,LPARAM lParam)
{
    SWFC_DEBUG_ARG("(%d,%d)", wParam, lParam);
    m_pSWAgcAwbTask->SetAWBGain( (DWORD*)wParam );
    return S_OK;
}
/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [out] lParam(DWORD*类型) RGB增益(0到7位为B通道值，8到15位G通道值，16到23为R通道值,例如:0x00AFAAFF
* ,分别为FF为B通道增益，AA为G通道增益，AF为R通道增益值)
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetRGBGain(WPARAM wParam,LPARAM lParam)
{
    m_pSWAgcAwbTask->GetAWBGain( *((DWORD*)lParam) );
    return S_OK;
}

/**
* @brief 消息接收函数
* @param [in] wParam 快门值
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetShutter(WPARAM wParam,LPARAM lParam)
{
    SWFC_DEBUG_ARG("(%d,%d)", wParam, lParam);
    m_pSWAgcAwbTask->SetAGCShutter(wParam) ; return S_OK;
}

/**
* @brief MSG_SHUTTER 获取快门指令
* @param [in] wParam 保留
* @param [out] lParam（INT*类型） 快门值
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetShutter(WPARAM wParam,LPARAM lParam)
{
    m_pSWAgcAwbTask->GetAGCShutter( *((INT*)lParam) ); return S_OK;
}


/**
* @brief 消息接收函数
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetDiffCorrEnable(WPARAM wParam,LPARAM lParam)
{
    SWFC_DEBUG_ARG("(%d,%d)", wParam, lParam);
	SW_TRACE_DEBUG("<cameramsg>set video diff %d.", (INT)wParam);
    return m_pSWChanneCorrTask->Signal((wParam == 1
                    ? CSWTask::kStartEvent : CSWTask::kIdleEvent));
}

/**
* @brief 消息接收函数
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCaptureEnable(WPARAM wParam,LPARAM lParam)
{
	SWFC_DEBUG_ARG("(%d,%d)", wParam, lParam);
	SW_TRACE_DEBUG("<cameramsg>set captrue diff %d.", (INT)wParam);
    return m_pSWCapture->Signal((wParam == 1
                    ? CSWTask::kStartEvent : CSWTask::kIdleEvent));
}
/**
* @brief 消息接收函数
* @param [in] wParam 快门值
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCaptureShutter(WPARAM wParam,LPARAM lParam)
{
    SWFC_DEBUG_ARG("(%d,%d)", (*(INT*)wParam), lParam);
    m_pSWCapture->SetShutter( (*(INT*)wParam) ); return S_OK;
}

/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [out] lParam(INT*类型) 快门值
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetCaptureShutter(WPARAM wParam,LPARAM lParam)
{
    SWFC_DEBUG_ARG("(%d,%d)", wParam, (INT*)lParam);
    m_pSWCapture->GetShutter((*(INT*)lParam));
	return S_OK;
}
/**
* @brief 消息接收函数
* @param [in] wParam 增益值(200)
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCaptureGain(WPARAM wParam,LPARAM lParam)
{
    SWFC_DEBUG_ARG("(%d,%d)", (*((INT*)wParam)), lParam);
     m_pSWCapture->SetGain( (*((INT*)wParam)) );
	 // 抓拍增益的使能要与抓拍差异校正同步。
	 // 使能抓拍差异性校正
	 if( (*((INT*)wParam)) != -1 )
	 {
		 OnSetCaptureEnable( 1 , 0);
	 }
	 return S_OK;
}
/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [out] lParam(INT* 类型) 增益值
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetCaptureGain(WPARAM wParam,LPARAM lParam)
{
    SWFC_DEBUG_ARG("(%d,%d)", wParam, lParam);
    m_pSWCapture->GetGain( *((INT*)lParam )); return S_OK;
}

/**
* @brief 消息接收函数
* @param [in] wParam RGB增益数组.0:R,1:G,2:B
* @param [in] lParam 保留
* note 里面会转的成(0到7位为B通道值，8到15位G通道值，16到23为R通道值,例如:0x00AFAAFF
*   分别为FF为B通道增益，AA为G通道增益，AF为R通道增益值)
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCaptureRGBGain(WPARAM wParam,LPARAM lParam)
{
	INT* piRGB = (INT*)wParam;
	if( piRGB != NULL )
	{
		INT iRGB = 0;
		if( piRGB[0] == -1 || piRGB[1] == -1 || piRGB[2] == -1 )
		{
			iRGB = 0xFF000000;
		}
		else
		{
			iRGB = (((piRGB[0] & 0x000000FF) << 16) | ((piRGB[1] & 0x000000FF) << 8) | (piRGB[2] & 0x000000FF));
		}
		SW_TRACE_DEBUG("<CameralControlMSG>Setcapture RGB:(%d,%d,%d)==>0x%08x.\n", piRGB[0], piRGB[1], piRGB[2], iRGB);
		m_pSWCapture->SetRGBGain( iRGB );
		return S_OK;
	}
	return E_FAIL;
}
/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [out] lParam(INT数组类型)0:R,1:G,2:B
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetCaptureRGBGain(WPARAM wParam,LPARAM lParam)
{
    SWFC_DEBUG_ARG("(%d,%d)", wParam, lParam);
	INT* piRGB = (INT*)lParam;
	if( NULL != piRGB )
	{
		DWORD dwRGB = 0;
		m_pSWCapture->GetRGBGain(dwRGB);

		piRGB[0] = ((dwRGB & 0x00FF0000) >> 16);
		piRGB[1] = ((dwRGB & 0x0000FF00) >> 8);
		piRGB[2] = (dwRGB & 0x000000FF);
		return S_OK;
	}
	return E_FAIL;
}
/**
* @brief MSG_SET_SOFTTOUCH,设置抓拍软触发
* @param [in] wParam 高1字节为触发的闪光灯组合（目前支持3路闪光灯，从低位开始
* ，第一位对应第一路闪光灯，第二位对应第二路闪光灯……该位为1则表示启用该路闪光灯）
* @param [in] wParam 低3字节为抓拍信息（对应的抓拍图会带有此信息，可用于抓拍图的标识）
* ，硬触发的帧号为累加方式
*  例子: 0x00000013,1为闪光灯信息，3为抓拍信息
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCaptureSofttouch(WPARAM wParam,LPARAM lParam)
{
	int iSet = 0;
	int iInfo = 0;
	iSet = ( wParam & 0xFF000000 ) >> 24;
	iInfo =( wParam & 0X00FFFFFF);
	if(iSet == 0 || iSet > 7 ){
		iSet = 7;
	}

	SW_TRACE_DEBUG("<CameralControlMSG>set:%d, info:%d.\n", iSet, iInfo);
    return swpa_camera_io_soft_capture(iSet , iInfo);
}


/**
* @brief 设置频闪信号使能。
* @param [in] wParam 0或1
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetFlashRateEnable(WPARAM wParam,LPARAM lParam)
{
	INT iEnable = (INT)wParam;
	SW_TRACE_DEBUG("<CameralControlMSG>SetFlashRateEnable:%d.\n", iEnable);
	return (0 == swpa_camera_io_set_strobe_signal(iEnable)) ? S_OK : E_FAIL;
}

/**
* @brief 设置抓拍信号使能
* @param [in] wParam 0或1
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCaptrueSyncEnable(WPARAM wParam,LPARAM lParam)
{
	INT iEnable = (INT)wParam;
	SW_TRACE_DEBUG("<CameralControlMSG>SetCaptrueSyncEnable:%d.\n", iEnable);
	INT iValue = (iEnable == 1) ? 7 : 0;
	return (0 == swpa_camera_io_set_flash_signal(iValue)) ? S_OK : E_FAIL;
}

/**
* @brief 设置频闪脉宽
* @param [in] wParam 脉宽 100~20000，单位微秒
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
	*/
HRESULT CSWCameralControlMSG::OnSetFlashRatePulse(WPARAM wParam,LPARAM lParam)
{
	INT iValue = (INT)wParam;
	iValue /= 100;
	INT iFlashRateMultiplication = 0;
	INT iFlashRatePolarity = 0;
	INT iFlashRatePulseWidth = 0;
	INT iFlashRateOutputType = 0;
	INT iCaptureSynCoupling = 0;

	HRESULT hr = E_FAIL;

	if( 0 == swpa_camera_io_get_strobe_param(
		&iFlashRateMultiplication,
		&iFlashRatePolarity,
		&iFlashRatePulseWidth,
		&iFlashRateOutputType,
		&iCaptureSynCoupling) )
	{
		if( 0 == swpa_camera_io_set_strobe_param(
			iFlashRateMultiplication,
			iFlashRatePolarity,
			iValue,
			iFlashRateOutputType,
			iCaptureSynCoupling) )
		{
			hr = S_OK;
		}
	}

	SW_TRACE_DEBUG("<CameralControlMSG>SetFlashRatePulse:%d. ret=0x%08x\n", iValue, hr);

	return hr;
}


/**
* @brief MSG_SET_MAXSHU_GAIN,设置设备信息
* @param [in] wParam 高16位最大快门
* @param [in] wParam 低16位为最大增益
*  例子: 0x100300C8,1003为最大快门，00C8为最大增益
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetAGCMaxShuAndGain(WPARAM wParam,LPARAM lParam)
{
    DWORD dwMaxShutter = ((0xFFFF0000&wParam) >> 16);
    DWORD dwMaxGain = (0x0000FFFF&wParam);

    SWFC_DEBUG_ARG("(%d,%d)", dwMaxShutter, dwMaxGain);

   m_pSWAgcAwbTask->SetAGCMaxParam( (INT)dwMaxShutter , (INT)dwMaxGain );

   return S_OK;
}


/**
* @brief MSG_GET_MAXSHU_GAIN,设置设备信息
* @param [in] wParam 保留
* @param [in] lParam lParam(DWORD*类型) 高16位最大快门
* @param [in] lParam lParam(DWORD*类型) 低16位为最大增益
*  例子: 0x100300C8,1003为最大快门，00C8为最大增益
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetAGCMaxShuAndGain( WPARAM wParam,LPARAM lParam )
{
    DWORD dwMaxShuttor = 0 ; DWORD dwMaxGain = 0 ;
    m_pSWAgcAwbTask->GetAGCParam( dwMaxShuttor , dwMaxGain );

    *((DWORD*)lParam) = ( dwMaxShuttor << 16 );
    *((DWORD*)lParam) |= dwMaxGain;

    return S_OK;
}

/**
* @brief MSG_SET_MINSHU_GAIN,设置设备信息
* @param [in] wParam 高16位最小快门
* @param [in] wParam 低16位为最小增益
*  例子: 0x100300C8,1003为最大快门，00C8为最大增益
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetAGCMinShuAndGain(WPARAM wParam,LPARAM lParam)
{
    DWORD dwMinShutter = ((0xFFFF0000&wParam) >> 16);
    DWORD dwMinGain = (0x0000FFFF&wParam);

    m_pSWAgcAwbTask->SetAGCMinParam( dwMinShutter , dwMinGain );
}

/**
* @brief MSG_GET_MINSHU_GAIN,设置设备信息
* @param [in] wParam 保留
* @param [in] lParam lParam(DWORD*类型) 高16位最小快门
* @param [in] lParam lParam(DWORD*类型) 低16位为最小增益
*  例子: 0x100300C8,1003为最大快门，00C8为最大增益
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetAGCMinShuAndGain(WPARAM wParam,LPARAM lParam)
{
    DWORD dwMinShuttor = 0 ; DWORD dwinGain = 0 ;
    m_pSWAgcAwbTask->GetAGCMinParam( dwMinShuttor , dwinGain );

    *((DWORD*)lParam) = ( dwMinShuttor << 16 );
    *((DWORD*)lParam) |= dwinGain;

    return S_OK;
}



HRESULT CSWCameralControlMSG::OnSetAGCParam(WPARAM wParam,LPARAM lParam)
{
	DWORD* pdwParam = (DWORD*)wParam;

	m_pSWAgcAwbTask->SetAGCMinParam( pdwParam[0] , pdwParam[2] );
	m_pSWAgcAwbTask->SetAGCMaxParam( pdwParam[1] , pdwParam[3] );

	return S_OK;
}



 /**
* @brief MSG_SET_DCAPERTUREENABLE,使能DC光圈
* @param [in] wParam 1为使能，0为非使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetDCApertureEnable(WPARAM wParam,LPARAM lParam)
{
		SW_TRACE_DEBUG("CSWCameralControlMSG::OnSetDCApertureEnable(0x%08x, 0x%08x)", wParam, lParam);
    m_pSWAgcAwbTask->SetAutoAperture( wParam ); return S_OK;
}
/**
* @brief MSG_SET_FILTERSWITCH 滤光片切换
* @param [in] wParam 1正常滤光片，2红外滤光片
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetFilterSwitch(WPARAM wParam,LPARAM lParam)
{
    // 初始化外部设备
    m_pSWExtDevControl->FilterSwitch( wParam );
}


/**
* @brief MSG_SET_REDLIGHT_RECT 设置红灯加红区域
* @param [in] wParam SW_RECT*
* @param [in] lParam INT 红灯加红区域数。
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetRedLightRect(WPARAM wParam,LPARAM lParam)
{
	// 初始化外部设备
	return m_pSWExtDevControl->SetRedLightRect((SW_RECT*)wParam, (INT)lParam);
}

/**
* @brief MSG_SET_CAPTUREEDGE 设置触发抓拍沿
* @param [in] wParam 0~3
* @param [out] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCaptureEdge(WPARAM wParam,LPARAM lParam)
{
    return m_pSWExtDevControl->SetCaptureEdge((int)wParam);
}

/**
* @brief MSG_GET_CAPTUREEDGE 获取触发抓拍沿
* @param [in] 保留
* @param [out] lParam 0~3
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetCaptureEdge(WPARAM wParam,LPARAM lParam)
{
    return m_pSWExtDevControl->GetCaptureEdge((int*)(&lParam));
}

/*
* @brief MSG_GET_BARRIER_STATE 获取栏杆机状态
* @param [in] 保留
* @param [out] lParam ，INT型
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetBarrierState(WPARAM wParam,LPARAM lParam)
{
    return m_pSWExtDevControl->GetBarrierState((int*)(lParam));
}


HRESULT CSWCameralControlMSG::OnGetPTZInfo(PVOID pvBuffer, INT iSize)
{
    SW_TRACE_DEBUG("Info: got GetPTZInfo msg...\n");

    //send remote msg to notify the ptz info
    PTZ_INFO sInfo;

	swpa_memset(&sInfo, 0, sizeof(sInfo));

	/* not support any PTZ feature */
        
    return SendRemoteMessage(MSG_APP_REMOTE_SET_PTZ_INFO, (PVOID)&sInfo, sizeof(sInfo));
}


