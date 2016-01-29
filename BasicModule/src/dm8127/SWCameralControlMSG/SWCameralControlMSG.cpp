
#include "SWFC.h"
#include "SWCameralControlMSG.h"
#include "SWPTZParameter.h"

//#define SW_TRACE_DEBUG SW_TRACE_DEBUG
//#define SW_TRACE_DEBUG
//REGISTER_CLASS(CSWCameralControlMSG)

CSWCameralControlMSG::CSWCameralControlMSG()
    : CSWMessage( MSG_CAMERA_CONTROL_START , MSG_CAMERA_CONTROL_END )

{
    m_iManualShutter = 0;
    m_iManualGain    = 0;
    m_iManualGainR   = 0;
    m_iManualGainG   = 0;
    m_iManualGainB   = 0;
	m_dwLEDPolarity = 0;
	m_FilterStatus = 0;
	m_dwLEDOutputType = 0;
	m_dwTriggerOutPolarity = 0;
	m_dwTriggerOutOutputType = 0;
	m_dwAGCScene = 0;
	m_fIsDayNow = TRUE;
}

CSWCameralControlMSG::~CSWCameralControlMSG()
{

}
#define FAIL_RET( func ) if( S_OK != func ){return E_FAIL;}
/**
* @brief MSG_INITIAL_PARAM 设置设置相机初始化参数结构体
* @param [in] wParam （CamAppParam*结构指针类型）
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnInitialCameralParam( WPARAM wParam,LPARAM lParam )
{
    LPCAMERAPARAM_INFO pCameraParam = (LPCAMERAPARAM_INFO)wParam;

	SW_TRACE_DEBUG("%s %d pCameraParam = 0x%x\n",__FUNCTION__,__LINE__,pCameraParam);
    if( pCameraParam == NULL )
        return S_FALSE;

	//工作模式
	FAIL_RET(this->OnSetWorkMode((WPARAM)pCameraParam->iCameraWorkMode, 0));
	//白平衡模式
	FAIL_RET(this->OnSetAWBMode((WPARAM)pCameraParam->iAWBMode, 0));

    // 设置AGC的使能
    FAIL_RET(this->OnSetAGCEnable( pCameraParam->iAGCEnable , 0 ));
    if(!pCameraParam->iAGCEnable)
    {
        // 相机快门
        FAIL_RET(this->OnSetShutter( pCameraParam->iShutter , 0 ));
		// 相机增益
        FAIL_RET(this->OnSetAGCGain( pCameraParam->iGain , 0 ));
    }
    // 设置AWB使能
    FAIL_RET(this->OnSetAWBEnable( pCameraParam->iAWBEnable, 0));
    if(!pCameraParam->iAWBEnable)
    {
        int rgiValue[3] = {0};
        rgiValue[0] = pCameraParam->iGainR;
        rgiValue[1] = pCameraParam->iGainG;
        rgiValue[2] = pCameraParam->iGainB;
        //设置R增益,G增益,B增益
        FAIL_RET(this->OnSetRGBGain( (WPARAM)rgiValue , 0 ));
    }
    // 设置AGC的期望值
    FAIL_RET(this->OnSetAGCTh( pCameraParam->iAGCTh , 0 ));
    DWORD nValue= 0;
    //设置AGC快门范围
    nValue = (pCameraParam->iAGCShutterHOri << 16);
    nValue |= (pCameraParam->iAGCShutterLOri & 0x0000FFFF);
    FAIL_RET(this->OnSetAGCShutterRange( nValue , 0 ));

    // 设置AGC增益范围
    nValue = (pCameraParam->iAGCGainHOri << 16);
    nValue |= (pCameraParam->iAGCGainLOri & 0x0000FFFF);
    FAIL_RET(this->OnSetAGCGainRange( nValue , 0 ));
	//
	FAIL_RET(this->OnSetAGCScene(pCameraParam->iAGCScene, 0));

    FAIL_RET(this->OnSetAGCZone((WPARAM)pCameraParam->rgiAGCZone, 0 ));

    //设置WDR强度值

    FAIL_RET(this->OnSetWDRStrength(pCameraParam->iWDRStrength , 0));
    // 设置锐化值
    //    FAIL_RET(this->OnSetSharpeEnable(pCameraParam->iEnableSharpen , 0));
    //    FAIL_RET(this->OnSetSharpeThreshold( pCameraParam->iSharpenThreshold , 0));

    // 设置图像增强使能
    FAIL_RET(this->OnSetColoMaxtrixEnable( pCameraParam->fColorMatrixEnable , 0));

    // todo.
    // 目前饱和度由AE自动设置，在AE使能时,初始化时不需再设置此值，避免冲突。
    if(!pCameraParam->iAGCEnable)
    {		
		// 设置 饱和度
		FAIL_RET(this->OnSetSaturationThreshold( pCameraParam->iSaturationValue , 0));
    }
    // 设置对比度
	FAIL_RET(this->OnSetContrastThreshold( pCameraParam->iContrastValue , 0));

    //设置降噪TNF使能
    FAIL_RET(this->OnSetTNFEnable( pCameraParam->fTNFEnable , 0));
    //设置降噪SNF使能
    FAIL_RET(this->OnSetSNFEnable( pCameraParam->fSNFEnable , 0));
    //设置降噪强度值
    FAIL_RET(this->OnSetDeNoiseState( pCameraParam->iDenoiseState , 0));

    //设置红灯加红使能
    FAIL_RET(this->OnSetRedLightEnable( pCameraParam->fRedLightEnable , 0));

    FAIL_RET(this->OnSetRedLightThreshold( (WPARAM)pCameraParam->iLumLThreshold , 0));

	//设置绿灯加绿
	//2014-05-06: 目前绿灯加绿与红灯加红共用同一个使能标志
	//即使能红灯加红则同时使能绿灯加绿，叫"交通灯颜色加强"更合理一些
	FAIL_RET(this->OnSetGreenLightThreshold( (WPARAM)pCameraParam->iLumLThreshold , 0));

    // 设置DC光圈使能
    FAIL_RET(this->OnSetDCApertureEnable( pCameraParam->iEnableDCAperture , 0));
    //设置电网同步使能
    FAIL_RET(this->OnSetACSyncEnable( pCameraParam->iExternalSyncMode ,0));
    //设置电网同步延时
    FAIL_RET(this->OnSetACSyncDelay( pCameraParam->iSyncRelay,0 ));
    //设置滤光片切换
    // todo. 初始化都是不使能。
    //FAIL_RET(this->OnSetFilterSwitch( 0, 0));//发现有小概率卡住在这里，电警卡口方案不需要滤光片，所以暂时直接屏蔽
    //设置黑白图使能
    FAIL_RET(this->OnSetGrayImageEnable( pCameraParam->fGrayImageEnable , 0));

    //设置CVBS制式
    FAIL_RET(this->OnSetCVBSStd( pCameraParam->iCVBSMode , 0));

    // 去掉GAMMA的功能设置。
    //FAIL_RET(this->OnSetGammaStrength( pCameraParam->iGammaValue , 0));
    //FAIL_RET(this->OnSetGammaEnable( pCameraParam->iGammaEnable , 0));
    FAIL_RET(this->OnSetEdgeEnhance( pCameraParam->iEdgeValue, 0));

    // 先关闭灯
    OnSetLEDSwitch(0, 0);

	//补光灯控制 EXP IO
	DWORD dwParam[2];	//polarity output_type 
	dwParam[0] = pCameraParam->iLEDPolarity; //补光灯输出极性
	dwParam[1] =pCameraParam->iLEDOutputType;	//补光灯输出类型
	FAIL_RET(this->OnSetEXPIO((WPARAM)dwParam, 0));
	m_dwLEDPluseWidth = pCameraParam->iLEDPluseWidth;
	this->OnSetEXPPluseWidth(pCameraParam->iLEDPluseWidth, 0);

	//触发抓拍 TG IO
	FAIL_RET(this->OnSetTGIO(pCameraParam->iCaptureEdgeOne,0));

	//触发输出	F1 IO
	// dwParam[0] = pCameraParam->nTriggerOutNormalStatus;	//触发输出极性
	// dwParam[1] =pCameraParam->nCaptureSynOutputType;	//触发输出类型

	//分车道闪需要同时设置两个灯
	dwParam[0] = pCameraParam->iFlashEnable;
	dwParam[1] = pCameraParam->iFlashCoupling;
	dwParam[2] = pCameraParam->iFlashOutputType;
	dwParam[3] = pCameraParam->iFlashPolarity;
	dwParam[4] = pCameraParam->iFlashPluseWidth;
	FAIL_RET(this->OnSetF1TriggerOut((WPARAM)dwParam, 0));
	FAIL_RET(this->OnSetAlarmTriggerOut((WPARAM)dwParam, 0));
	
	// FAIL_RET(this->OnSetTriggerOut((WPARAM)dwParam,0));

	//报警输出 ALM IO
	// dwParam[0] = pCameraParam->iALMPolarity; //报警输出极性
	// dwParam[1] =pCameraParam->iALMOutputType;	//报警输出类型
	// FAIL_RET(this->OnSetALMIO((WPARAM)dwParam,0));

	//F1输出配置
	//FAIL_RET(this->OnSetF1OutputType((WPARAM)pCameraParam->iF1OutputType, 0));
	
	// 色阶
	FAIL_RET(this->OnSetColorGradation((WPARAM)pCameraParam->iColorGradation, 0));

	m_nCaptureR = pCameraParam->iCaptureGainR;
	m_nCaptureG = pCameraParam->iCaptureGainG;
	m_nCaptureB = pCameraParam->iCaptureGainB;
	m_nCaptureShutter = pCameraParam->iCaptureShutter;
	m_nCaptureGain = pCameraParam->iCaptureGain;
	m_nCapSharpen = pCameraParam->iCaptureSharpenThreshold;
	m_fCaptureAutoParamEnable = pCameraParam->iCaptureAutoParamEnable;
	m_fEnableCaptureRGBGain = pCameraParam->iCaptureRGBEnable;
	m_fEnableCaptureShutter = pCameraParam->iCaptureShutterEnable;
	m_fEnableCaptureGain = pCameraParam->iCaptureGainEnable;
	//m_fEnableCaptureSharpen = pCameraParam->iCaptureSharpenEnable;

	// OnSetCaptureRGBEnable(pCameraParam->iCaptureRGBEnable, 0xF0F0F0F0);
	// OnSetCaptureShutterEnable(pCameraParam->iCaptureShutterEnable, 0xF0F0F0F0);
	// OnSetCaptureGainEnable(pCameraParam->iCaptureGainEnable, 0xF0F0F0F0);
	//OnSetCaptureSharpenEnable(pCameraParam->iCaptureSharpenEnable, 0xF0F0F0F0);
	
	INT rgCaptureShutterValue[2] = {0};
	INT rgCaptureGainValue[2] = {0};

	rgCaptureShutterValue[0] = pCameraParam->iCaptureShutter;
	rgCaptureShutterValue[1] = 1;//pCameraParam->iCaptureShutterEnable;
	rgCaptureGainValue[0]    = pCameraParam->iCaptureGain;
	rgCaptureGainValue[1]	 = 1;//pCameraParam->iCaptureGainEnable;

	OnSetCaptureGain((WPARAM)rgCaptureGainValue, NULL);
	OnSetCaptureShutter((WPARAM)rgCaptureShutterValue, NULL);

	this->OnSetWDRLevel(0, 0);

	OnIncreaseBrightness();

    return S_OK;
}

/**
/**
* @brief MSG_SET_COLORMATRIX_ENABLE,使能图像增强
* @param [in] wParam 0:关闭，1：打开
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetColoMaxtrixEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_SATURATION_CONTRAST_STATE);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_SATURATION_CONTRAST_STATE ,(void*)&Value , sizeof(DWORD) , 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_COLORMATRIX_ENABLE,获取使能图像增强
* @param [in] wParam 保留
* @param [in] lParam 0:关闭，1：打开
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetColoMaxtrixEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_SATURATION_CONTRAST_STATE);
    return 0 == swpa_ipnc_control(0, CMD_GET_SATURATION_CONTRAST_STATE ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
}



/**
* @brief MSG_SET_SHARPEN_ENABLE,设置锐化使能
* @param [in] wParam 0为关闭锐化，1锐化所有帧
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetSharpeEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_SHARPNESS_STATE);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_SHARPNESS_STATE ,(void*)&Value, sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_SHARPEN_ENABLE,获取锐化使能
* @param [in] wParam 保留
* @param [in] lParam 类型： DWORD*， 0为关闭锐化，1锐化所有帧
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetSharpeEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_SHARPNESS_STATE);//
    // return 0 == swpa_ipnc_control(0, CMD_GET_SHARPNESS_STATE ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
    return E_FAIL;
}

/**
* @brief MSG_SET_SHARPENTHRESHOLD,锐化阀值
* @param [in] wParam 锐化阀值[0~100]，推荐使用7
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetSharpeThreshold(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_SHARPNESS);
    DWORD Value = (DWORD)wParam;
    //  return 0 == swpa_ipnc_control(0, CMD_SET_SHARPNESS ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
    return E_FAIL;
}

/**
* @brief MSG_GET_SHARPENTHRESHOLD,锐化阀值
* @param [in] wParam 保留
* @param [in] lParam 类型：DWORD*, 锐化阀值[0~100]
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetSharpeThreshold(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_SHARPNESS);
    // return 0 == swpa_ipnc_control(0, CMD_GET_SHARPNESS ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
    return E_FAIL;
}

/**
* @brief MSG_SET_CONTRASTTHRESHOLD,设置对比度
* @param [in] wParam 类型：DWORD ,[-100~100]
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetContrastThreshold(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_CONTRAST);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_CONTRAST ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_CONTRASTTHRESHOLD,获取对比度
* @param [in] wParam 保留
* @param [in] lParam 类型：DWORD* , [-100~100]
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetContrastThreshold(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_CONTRAST);
    return 0 == swpa_ipnc_control(0, CMD_GET_CONTRAST ,(void*)lParam , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_SET_SATURATIONTHRESHOLD,设置饱和度
* @param [in] wParam 饱和度[-100~100]
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetSaturationThreshold(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d  %d %d\n",__FUNCTION__,__LINE__,(DWORD*)wParam,CMD_SET_SATURATION);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_SATURATION ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_SET_SATURATIONTHRESHOLD,获取饱和度
* @param [in] wParam 保留
* @param [in] lParam 类型：DWORD* ,饱和度[-100~100]
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetSaturationThreshold(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_SATURATION);
    return 0 == swpa_ipnc_control(0, CMD_GET_SATURATION ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_SET_AGCTH 设置AGC图像亮度期望值
* @param [in] wParam 期望值
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetAGCTh( WPARAM wParam,LPARAM lParam )
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_AE_THRESHOLD);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_AE_THRESHOLD ,(void*)&Value , sizeof(DWORD), 0) ? S_OK :E_FAIL;
}

/**
* @brief MSG_SET_SHU_RANGE 设置AGC曝光时间范围
* @param [in] wParam 曝光范围DWORD类型：4字节,0~15：最小曝光时间16~32：最大曝光时间
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetAGCShutterRange( WPARAM wParam,LPARAM lParam )
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_AE_EXP_RANGE);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_AE_EXP_RANGE ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}
/**
* @brief MSG_GET_SHU_RANGE 获取AGC曝光时间范围
* @param [in] wParam 保留
* @param [out] lParam 曝光范围DWORD*类型：4字节0~15：最小曝光时间16~32：最大曝光时间
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetAGCShutterRange( WPARAM wParam,LPARAM lParam )
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AE_EXP_RANGE);
    return 0 == swpa_ipnc_control(0, CMD_GET_AE_EXP_RANGE ,(void*)lParam , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_SET_SHU_RANGE 设置AGC增益范围
* @param [in] wParam DWORD类型：4字节0~15：最小增益时间16~32：最大增益时间
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetAGCGainRange( WPARAM wParam,LPARAM lParam )
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_AE_GAIN_RANGE);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_AE_GAIN_RANGE ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
 * @brief MSG_SET_AGC_SCENE 设置AGC情景模式
 * @param [in] wParam DWORD类型：4字节，0：自动, 1: 情景1, 2: 情景2
 * @param [in] lParam 保留
 * @return 成功返回S_OK，其他值代表失败
 */
HRESULT CSWCameralControlMSG::OnSetAGCScene( WPARAM wParam,LPARAM lParam )
{
	SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_SCENE);
    DWORD Value = (DWORD)wParam;
	if (0 != swpa_ipnc_control(0, CMD_SET_SCENE ,(void*)&Value , sizeof(DWORD), 0))
	{
		SW_TRACE_NORMAL("Set AGC Scene Failed\n");
		return E_FAIL;
	}
	
	m_dwAGCScene = wParam;
	return S_OK;
}

/**
 * @brief MSG_GET_AGC_SCENE 获取AGC情景模式
 * @param [in] wParam 保留
 * @param [in] lParam DWORD类型：4字节，0：自动, 1: 情景1, 2: 情景2
 * @return 成功返回S_OK，其他值代表失败
 */
HRESULT CSWCameralControlMSG::OnGetAGCScene( WPARAM wParam,LPARAM lParam )
{
	SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AE_EXP_RANGE);
	DWORD *pdwScene = (DWORD *)lParam;
	if (NULL == pdwScene)
	{
		SW_TRACE_NORMAL("OnGetAGCScene arg is NULL\n");
		return E_FAIL;
	}
	*pdwScene = m_dwAGCScene;
	
	return S_OK;
    //return 0 == swpa_ipnc_control(0, CMD_GET_SCENE ,(void*)lParam , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_SHU_RANGE 获取AGC增益范围
* @param [in] wParam 保留
* @param [out] lParam DWORD类型：4字节0~15：最小增益时间16~32：最大增益时间
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetAGCGainRange( WPARAM wParam,LPARAM lParam )
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AE_GAIN_RANGE);
    return 0 == swpa_ipnc_control(0, CMD_GET_AE_GAIN_RANGE ,(void*)lParam , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_SET_AGCZONE 设备AGC测光区域
* @param [in] wParam AGC测光区域使能标志INT数据，16个数据。
* @param [in] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetAGCZone(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_AE_ZONE);
    DWORD *rgiAGCZone = (DWORD *)wParam;
    DWORD nValue = 0 ;
    for(int i = 0; i < 16; ++i)
    {
        nValue |= (rgiAGCZone[i] == 1) <<i ;
    }
	//底层做限制
	/*
	if (0 == nValue)		//AGC测光区域不能全都不选
	{
		return E_FAIL;
	}
	*/
    return 0 == swpa_ipnc_control(0, CMD_SET_AE_ZONE ,(void*)&nValue , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_AGCZONE 获取AGC测光区域
* @param [in] wParam 保留
* @param [in] lParam AGC测光区域使能标志INT数据，16个数据。
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetAGCZone(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AE_ZONE);
    DWORD *rgiAGCZone = (DWORD *)lParam;
    DWORD nValue = 0 ;

    INT Ret = swpa_ipnc_control(0, CMD_GET_AE_ZONE ,(void*)&nValue, sizeof(DWORD), 0);
    if(0 == Ret)
    {
        SW_TRACE_DEBUG("%s %d   nValue= %d \n",__FUNCTION__,__LINE__,nValue);
        for(int i = 0; i < 16; ++i)
        {
            rgiAGCZone[i] = (nValue >> i) & 0x1 ;
            SW_TRACE_DEBUG("%s %d   rgiAGCZone[%d] = %d \n",__FUNCTION__,__LINE__,i,(nValue >> i) & 0x1);
        }
        return S_OK;
    }
    SW_TRACE_DEBUG("%s %d  error Ret= %d \n",__FUNCTION__,__LINE__,Ret);
    return E_FAIL;

}


/**
* @brief MSG_SET_AGCTH 获取AGC图像亮度期望值
* @param [in] wParam 保留
* @param [out] lParam （DWORD*类型） 期望值返回
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetAGCTh( WPARAM wParam,LPARAM lParam )
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AE_THRESHOLD);
    return 0 == swpa_ipnc_control(0, CMD_GET_AE_THRESHOLD ,(void*)lParam , sizeof(DWORD*), 0) ? S_OK:E_FAIL;
}
/**
* @brief 消息接收函数
* @param [in] wParam 增益值(如200)
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetAGCGain(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_GAIN);
    DWORD Value = (DWORD)wParam;
   	m_iManualGain = Value;
    return 0 == swpa_ipnc_control(0, CMD_SET_GAIN ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief 消息接收函数
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetAGCEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_AE_STATE);
    BOOL Value = (BOOL)wParam;
    SW_TRACE_DEBUG("%s %d  %d \n",__FUNCTION__,__LINE__,Value);
    return 0 == swpa_ipnc_control(0, CMD_SET_AE_STATE ,(void*)&Value , sizeof(BOOL), 0)?S_OK:E_FAIL;
}
/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [in] lParam 使能消息，使能标记，0=不使能，1 = 使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetAGCEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AE_STATE);
    return 0 == swpa_ipnc_control(0, CMD_GET_AE_STATE ,(void*)lParam , sizeof(BOOL*), 0) ? S_OK: E_FAIL;
}

/**
* @brief 消息接收函数
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetAWBEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_AWB_STATE);
    DWORD Value = (DWORD)wParam;
    SW_TRACE_DEBUG("%s %d  %d  \n",__FUNCTION__,__LINE__,Value);
    return 0 == swpa_ipnc_control(0, CMD_SET_AWB_STATE ,(void*)&Value , sizeof(BOOL), 0)?S_OK:E_FAIL;
}
/**
* @brief MSG_GET_AWBENABLE 消息接收函数
* @param [in] wParam 保留
* @param [in] lParam 使能消息，使能标记，0=不使能，1 = 使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetAWBEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d  cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AWB_STATE);
    int Ret = swpa_ipnc_control(0, CMD_GET_AWB_STATE ,(void*)lParam , sizeof(BOOL*), 0);
    if(0 == Ret)
    {
        SW_TRACE_DEBUG("%s %d  %d \n",__FUNCTION__,__LINE__,*((INT*)lParam));
        return S_OK;
    }
    SW_TRACE_DEBUG("%s %d  error  ret= %d \n",__FUNCTION__,__LINE__,Ret);
    return  E_FAIL;
}


/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [out] lParam（INT*类型） 增益返回值(200)
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetAGCGain(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_GAIN);
    INT Ret = swpa_ipnc_control(0, CMD_GET_GAIN ,(void*)lParam , sizeof(INT*), 0);
    if(0 == Ret)
    {

        SW_TRACE_DEBUG("OnGetAGCGain pdwParam is %d !\n",*((INT *)lParam));
        return S_OK;
    }
    SW_TRACE_DEBUG("%s %d  error  ret= %d \n",__FUNCTION__,__LINE__,Ret);
    return E_FAIL;
}


/**
* @brief 消息接收函数
* @param [in] wParam RGB增益(0到7位为R通道值，8到15位G通道值，16到23为B通道值,例如:0x00AFAAFF
* ,分别为FF为B通道增益，AA为G通道增益，AF为R通道增益值)
* 如是使能消息，则为使能标记0=不使能，1 = 使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetRGBGain(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_RGB);
    DWORD *tmp = (DWORD *)wParam;
    if(tmp == NULL || tmp+1 == NULL || tmp+2== NULL)
    {
        SW_TRACE_DEBUG("%s %d Value error \n",__FUNCTION__,__LINE__);
        return E_FAIL;
    }
    SW_TRACE_DEBUG("%s %d  %d %d %d\n",__FUNCTION__,__LINE__,tmp[0],tmp[1],tmp[2]);
    DWORD Value;
    m_iManualGainR = tmp[0];
    m_iManualGainG = tmp[1];
    m_iManualGainB = tmp[2];
    Value = tmp[0]  | tmp[1] << 8 | tmp[2] << 16;
    SW_TRACE_DEBUG("%s %d  %d\n",__FUNCTION__,__LINE__,Value);
    int ret = swpa_ipnc_control(0, CMD_SET_RGB ,(void*)&Value , sizeof(DWORD), 0);
    if(0 == ret)
    {
        SW_TRACE_DEBUG("%s %d  %d\n",__FUNCTION__,__LINE__,Value);
        return S_OK;
    }
    else
    {
        SW_TRACE_DEBUG("%s %d  %d %d \n",__FUNCTION__,__LINE__,Value,ret);
        return E_FAIL;
    }
}
/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [out] lParam(DWORD*类型) RGB增益(0到7位为R通道值，8到15位G通道值，16到23为G通道值,例如:0x00AFAAFF
* ,分别为FF为B通道增益，AA为G通道增益，AF为R通道增益值)
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetRGBGain(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_RGB);
    DWORD *temp = (DWORD *)lParam;
    if(temp == NULL || temp+1 == NULL || temp +2 == NULL)
        return E_FAIL;
    int iValue;

    if(0 == swpa_ipnc_control(0, CMD_GET_RGB ,(void*)&iValue , sizeof(INT*), 0))
    {
        temp[0] = (iValue >> 0 ) & 0xff;
        temp[1] = (iValue >> 8 ) & 0xff;
        temp[2] = (iValue >> 16) & 0xff;
        return S_OK;
    }
    return E_FAIL;
}

/**
* @brief 消息接收函数
* @param [in] wParam 快门值
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetShutter(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_EXP);
    DWORD Value = (DWORD)wParam;
	m_iManualShutter = Value;
    return 0 == swpa_ipnc_control(0, CMD_SET_EXP ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_SHUTTER 获取快门指令
* @param [in] wParam 保留
* @param [out] lParam（INT*类型） 快门值
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetShutter(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_EXP);
    return 0 == swpa_ipnc_control(0, CMD_GET_EXP ,(void*)lParam , sizeof(INT*), 0)?S_OK:E_FAIL;
}


/**
* @brief MSG_SET_DCAPERTUREENABLE,使能DC光圈
* @param [in] wParam 1为使能，0为非使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetDCApertureEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_DCIRIS_AUTO_STATE);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_DCIRIS_AUTO_STATE ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_DCAPERTUREENABLE,获取自动DC光圈使能
* @param [in] wParam  保留
* @param [in] lParam 1为使能，0为非使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetDCApertureEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_DCIRIS_AUTO_STATE);
    return 0 == swpa_ipnc_control(0, CMD_GET_DCIRIS_AUTO_STATE ,(void*) lParam, sizeof(DWORD*), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_ZOOM_DCAPERTURE,放大DC光圈
* @param [in] wParam 保留
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnZoomDCAperture(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_ZOOM_DCIRIS);//未试成功
    INT Value = 1;//任意值
    INT Ret = swpa_ipnc_control(0, CMD_ZOOM_DCIRIS, (void *)&Value, sizeof(Value), 0);
    if(Ret == 0)
    {
        SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_ZOOM_DCIRIS);
        return S_OK;
    }
    SW_TRACE_DEBUG("%s %d Ret = %d \n",__FUNCTION__,__LINE__,Ret);
    return E_FAIL;
}

/**
* @brief MSG_SHRINK_DCAPERTURE,缩小DC光圈
* @param [in] wParam 保留
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnShrinkDCAperture(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SHRINK_DCIRIS);//为测试成功、
    INT Value = 1;//任意值
    return 0 == swpa_ipnc_control(0, CMD_SHRINK_DCIRIS, (void *)&Value, sizeof(Value), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_SET_FILTERSWITCH 滤光片切换
* @param [in] wParam 1正常滤光片，2红外滤光片
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetFilterSwitch(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_FILTER_STATE);
    DWORD Value = (DWORD)wParam;
    m_FilterStatus = (2 == Value) ? 1 : 0;
    return 0 == swpa_ipnc_control(0, CMD_SET_FILTER_STATE, (void*)&m_FilterStatus , sizeof(DWORD), 0)?S_OK:E_FAIL;
}
/**
* @brief MSG_GET_FILTERSWITCH 滤光片切换
* @param [in] wParam 保留
* @param [in] lParam 1正常滤光片，2红外滤光片
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetFilterSwitch(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_FILTER_STATE);
    DWORD *Value = (DWORD *)lParam;
    *Value = m_FilterStatus ? 2 : 1;
	
    return S_OK;
}

/**
* @brief MSG_SET_REDLIGHT_ENABLE 设置红灯加红使能
* @param [in] wParam 0为不使能，1为使能
* @param [in] lParam 保留。
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetRedLightEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_TRAFFIC_LIGTH_ENHANCE_STATE);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_TRAFFIC_LIGTH_ENHANCE_STATE ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_REDLIGHT_ENABLE 获取红灯加红使能
* @param [in] wParam 保留
* @param [in] lParam 0为不使能，1为使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetRedLightEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_TRAFFIC_LIGTH_ENHANCE_STATE);
    return 0 == swpa_ipnc_control(0, CMD_GET_TRAFFIC_LIGTH_ENHANCE_STATE ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_SET_REDLIGHT_RECT 设置红灯加红区域
* @param [in] wParam SW_RECT*
* @param [in] lParam INT 红灯加红区域数。
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetRedLightRect(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_TRAFFIC_LIGTH_ENHANCE_REGION);//未测
	const int MAX_RED_RECT_COUNT = 8;
	SW_RECT* rgcRect = (SW_RECT*)wParam;
	int iCount = (int)lParam;
	if( rgcRect == NULL || iCount <= 0 )
	{
		return E_POINTER;
	}
	DWORD rgdwRedInfo[6];
    for(int i = 0; i < iCount; ++i)
    {
    	SW_TRACE_DEBUG("<setred>%d(%d,%d,%d,%d).", i,
    			rgcRect[i].left, rgcRect[i].top, rgcRect[i].right, rgcRect[i].bottom);
    	rgdwRedInfo[0] = 0; //0:红灯
		rgdwRedInfo[1] = i;
    	rgdwRedInfo[2] = rgcRect[i].left;
    	rgdwRedInfo[3] = rgcRect[i].top / 2;
    	rgdwRedInfo[4] = rgcRect[i].right;
    	rgdwRedInfo[5] = rgcRect[i].bottom / 2;
    	swpa_ipnc_control(0, CMD_SET_TRAFFIC_LIGTH_ENHANCE_REGION ,(void*)rgdwRedInfo , sizeof(rgdwRedInfo), 0);
    }
    return S_OK;
}

/**
* @brief MSG_SET_TRAFFIC_LIGTH_LUM_TH 设置红灯加红
* @param [in] wParam
* @param [in] lParam 。
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetRedLightThreshold(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_TRAFFIC_LIGTH_LUM_TH);
    DWORD Value = (DWORD)wParam;
    // todo.
    // 178此值有修改，在这强转
    Value = (Value == 0x2d00 ? 0x0B33 : 0x0FFF);
	Value = Value | (0x0 << 28); //28~31位为0：红灯
    return 0 == swpa_ipnc_control(0, CMD_SET_TRAFFIC_LIGTH_LUM_TH ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}



/**
* @brief MSG_SET_GREENLIGHT_RECT 设置绿灯加绿区域
* @param [in] wParam SW_RECT*
* @param [in] lParam INT 绿灯加红区域数。
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetGreenLightRect(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_TRAFFIC_LIGTH_ENHANCE_REGION);
	const int MAX_GREEN_RECT_COUNT = 8;
	SW_RECT* rgcRect = (SW_RECT*)wParam;
	int iCount = (int)lParam;
	if( rgcRect == NULL || iCount <= 0 )
	{
		return E_POINTER;
	}
	DWORD rgdwGreenInfo[6];
    for(int i = 0; i < iCount; ++i)
    {
    	SW_TRACE_DEBUG("<setgreed>%d(%d,%d,%d,%d).", i,
    			rgcRect[i].left, rgcRect[i].top, rgcRect[i].right, rgcRect[i].bottom);
    	rgdwGreenInfo[0] = 1; //1:绿灯
		rgdwGreenInfo[1] = i;
    	rgdwGreenInfo[2] = rgcRect[i].left;
    	rgdwGreenInfo[3] = rgcRect[i].top / 2;
    	rgdwGreenInfo[4] = rgcRect[i].right;
    	rgdwGreenInfo[5] = rgcRect[i].bottom / 2;
    	swpa_ipnc_control(0, CMD_SET_TRAFFIC_LIGTH_ENHANCE_REGION ,(void*)rgdwGreenInfo , sizeof(rgdwGreenInfo), 0);
    }
    return S_OK;
}

/**
* @brief MSG_SET_TRAFFIC_GREEN_LIGTH_LUM_TH 设置绿灯加绿
* @param [in] wParam
* @param [in] lParam 。
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetGreenLightThreshold(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_TRAFFIC_LIGTH_LUM_TH);
    DWORD Value = (DWORD)wParam;
    // todo.
    // 178此值有修改，在这强转
    Value = (Value == 0x2d00 ? 0x0B32 : 0x0FFF); 
	Value = Value | (0x1 << 28); //28~31位为1: 绿灯
    return 0 == swpa_ipnc_control(0, CMD_SET_TRAFFIC_LIGTH_LUM_TH ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}




/**
* @brief MSG_SET_WDR_STRENGTH 设置WDR阈值
* @param [in] wParam 0为不使能，值越大强度越大。取值范围：0~4095
* @param [out] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetWDRStrength(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_WDR_STRENGTH);
    DWORD Value = (DWORD)wParam;
    SW_TRACE_DEBUG("%s %d Value = %d\n",__FUNCTION__,__LINE__,Value);
    return 0 == swpa_ipnc_control(0, CMD_SET_WDR_STRENGTH ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_WDR_THRESHOLD 设置WDR阈值
* @param [in] wParam 保留
* @param [out] lParam 0为不使能，值越大强度越大。取值范围：0~4095
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetWDRStrength(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_WDR_STRENGTH);
    return 0 == swpa_ipnc_control(0, CMD_GET_WDR_STRENGTH ,(void*)lParam , sizeof(DWORD), 0)?S_OK:E_FAIL;
}


/**
* @brief MSG_SET_TNF_ENABLE 设置TNF使能
* @param [in] wParam 0为不使能，1为使能
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetTNFEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_TNF_STATE);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_TNF_STATE ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_TNF_ENABLE 获取TNF使能
* @param [in] wParam  保留
* @param [out] lParam 0为不使能，1为使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetTNFEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_TNF_STATE);
    return 0 == swpa_ipnc_control(0, CMD_GET_TNF_STATE ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_SET_SNF_ENABLE 设置SNF使能
* @param [in] wParam 0为不使能，1为使能
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetSNFEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_SNF_STATE);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_SNF_STATE ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_SNF_ENABLE 获取SNF使能
* @param [in] wParam  保留
* @param [out] lParam 0为不使能，1为使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetSNFEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_SNF_STATE);
    return 0 == swpa_ipnc_control(0, CMD_GET_SNF_STATE ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_SET_DENOISE_MODE 设置降噪模式
* @param [in] wParam 0为不使能，1为使能
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetDeNoiseMode(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d  cmd1 = %d cmd2 = %d\n",__FUNCTION__,__LINE__,CMD_SET_TNF_STATE,CMD_SET_SNF_STATE);
    DWORD dwMode = (DWORD)wParam;
    DWORD dwSNFEnable = FALSE, dwTNFEnable=FALSE;
    switch (dwMode)
    {
    case 0:
        dwSNFEnable = FALSE;
        dwTNFEnable = FALSE;
        break;
	case 1:
        dwSNFEnable = FALSE;
        dwTNFEnable = TRUE;
        break;
    case 2:
        dwSNFEnable = TRUE;
        dwTNFEnable = FALSE;
        break;

    case 3:
        dwSNFEnable = TRUE;
        dwTNFEnable = TRUE;
        break;
    default:
        break;
    }

    if( 0 == swpa_ipnc_control(0, CMD_SET_TNF_STATE ,(void*)&dwTNFEnable , sizeof(DWORD), 0)
            && 0 == swpa_ipnc_control(0, CMD_SET_SNF_STATE ,(void*)&dwSNFEnable , sizeof(DWORD), 0)
            )
    {
        return S_OK;
    }
    return E_FAIL;
}
/**
* @brief MSG_GET_DENOISE_MODE 获取降噪模式
* @param [in] wParam  保留
* @param [out] lParam 0为不使能，1为使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetDeNoiseMode(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d  cmd1 = %d cmd2 = %d\n",__FUNCTION__,__LINE__,CMD_GET_SNF_STATE,CMD_GET_TNF_STATE);
    if( NULL == lParam )
    {
        return E_FAIL;
    }

    DWORD dwSNFEnable = 0, dwTNFEnable=0;
    DWORD dwMode = 0;
    if ( 0 == swpa_ipnc_control(0, CMD_GET_SNF_STATE ,(void*)&dwSNFEnable , sizeof(DWORD*), 0)
         && 0 == swpa_ipnc_control(0, CMD_GET_TNF_STATE ,(void*)&dwTNFEnable , sizeof(DWORD*), 0)
         )
    {
        if( 0 == dwSNFEnable && 0 == dwTNFEnable )
        {
            dwMode = 0;
        }
		else if( 0 == dwSNFEnable && 1 == dwTNFEnable )
        {
            dwMode = 1;
        }
        else if( 1 == dwSNFEnable && 0 == dwTNFEnable )
        {
            dwMode = 2;
        }
        else if( 1 == dwSNFEnable && 1 == dwTNFEnable )
        {
            dwMode = 3;
        }
        else
        {
            return E_FAIL;
        }
        *(DWORD*)lParam = dwMode;
        return S_OK;
    }
    return E_FAIL;
}

/**
* @brief MSG_SET_DENOISE_STATE 设置降噪状态
* @param [in] wParam 0~3:自动、低、中、高
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetDeNoiseState(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_TNF_SNF_STRENGTH);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_TNF_SNF_STRENGTH ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_DENOISE_STATE 获取降噪状态
* @param [in] wParam  保留
* @param [out] lParam 0~3:自动、低、中、高
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetDeNoiseState(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_TNF_SNF_STRENGTH);
    return 0 == swpa_ipnc_control(0, CMD_GET_TNF_SNF_STRENGTH ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
}


/**
* @brief MSG_SET_AC_SYNC_ENABLE 设置电网同步使能
* @param [in] wParam 0：不使能，1：使能
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetACSyncEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_AC_SYNC_STATE);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_AC_SYNC_STATE ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_AC_SYNC_ENABLE 获取电网同步使能
* @param [in] wParam  保留
* @param [out] lParam 0：不使能，1：使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetACSyncEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AC_SYNC_STATE);
    return 0 == swpa_ipnc_control(0, CMD_GET_AC_SYNC_STATE ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_SET_AC_SYNC_DELAY 设置电网同步延时
* @param [in] wParam 范围：0~25500
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetACSyncDelay(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_AC_SYNC_DELAY);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_AC_SYNC_DELAY ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}
/**
* @brief MSG_GET_AC_SYNC_DELAY 获取电网同步延时
* @param [in] wParam  保留
* @param [out] lParam 范围：0~25500
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetACSyncDelay(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AC_SYNC_DELAY);
    return 0 == swpa_ipnc_control(0, CMD_GET_AC_SYNC_DELAY ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_SET_GRAY_IMAGE_ENABLE 设置黑白图使能
* @param [in] wParam 0：不使能，1：使能
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetGrayImageEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_GRAY_IMAGE_STATE);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_GRAY_IMAGE_STATE ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}
/**
* @brief MSG_GET_GRAY_IMAGE_ENABLE 获取黑白图使能
* @param [in] wParam  保留
* @param [out] lParam 0：不使能，1：使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetGrayImageEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_GRAY_IMAGE_STATE);
    return 0 == swpa_ipnc_control(0, CMD_GET_GRAY_IMAGE_STATE ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
}
/**
* @brief MSG_SET_CVBS_STD 设置CVBS制式
* @param [in] wParam 0：PAL，1：NTSC
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCVBSStd(WPARAM wParam,LPARAM lParam)
{

    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_CVBS_STD);
    DWORD Value = (DWORD)wParam;
    SW_TRACE_DEBUG("%s %d  %d \n",__FUNCTION__,__LINE__,CMD_SET_CVBS_STD);
    if(0 == swpa_ipnc_control(0, CMD_SET_CVBS_STD ,(void*)&Value , sizeof(DWORD), 0))
    {
        SW_TRACE_DEBUG("%s %d Value\n",__FUNCTION__,__LINE__,Value);
        return S_OK;
    }
    SW_TRACE_DEBUG("%s %d Value\n",__FUNCTION__,__LINE__,Value);
    return E_FAIL;
}
/**
* @brief MSG_GET_CVBS_STD 读取CVBS制式
* @param [in] wParam  保留
* @param [out] lParam 0：PAL，1：NTSC
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetCVBSStd(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_CVBS_STD);
    return 0 == swpa_ipnc_control(0, CMD_GET_CVBS_STD ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
}
/**
* @brief MSG_SET_CAM_TEST 设置相机自动化测试
* @param [in] wParam 保留
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCamTest(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
    //获取AGC、AWB状态
    BOOL fAGCEnable = 0;
    BOOL fAWBEnable = 0;
    INT fAGCGain =0;
    INT fShutter =0;
    INT rgiValue[3] = {0};

    HRESULT hr = S_OK;

    FAIL_RET(OnGetAGCEnable(  NULL,(LPARAM)&fAGCEnable));
    FAIL_RET(OnGetAWBEnable(NULL, (LPARAM)&fAWBEnable));
    //获取R增益,G增益,B增益
    FAIL_RET(this->OnGetRGBGain(NULL,(LPARAM)rgiValue ));
    //获取增益
    FAIL_RET(this->OnGetAGCGain(NULL,(LPARAM)&fAGCGain ));
    //获取快门
    FAIL_RET(this->OnGetShutter(NULL,(LPARAM)&fShutter ));


    BOOL fEnable = 0;
    FAIL_RET(OnSetAGCEnable( fEnable , 0 ));
    FAIL_RET(OnSetAWBEnable( fEnable , 0 ));

    //调整快门最小到最大，步长：1/10 *（最大快门-最小快门），耗时：2秒
    for (int i = 1 ; i <= 50; i++)
    {
        FAIL_RET(OnSetShutter( 30000 *i/50 , 0));
        swpa_thread_sleep_ms(20);
    }
    //调整增益最小到最大；步长：1/10 *（最大增益-最小增益），耗时：2秒
    for (int i = 1 ; i <= 50; i++)
    {
        FAIL_RET(OnSetAGCGain( 420 *i/50 , 0));
        swpa_thread_sleep_ms(20);
    }
    //调整R最小到最大; 步长：1/10 *（最大R值-最小R值），耗时：2秒
    DWORD nValue[3];
    swpa_memset(&nValue,0,sizeof(nValue));
    for (int i = 1 ; i <= 50; i++)
    {
        nValue[0] = 255 *i/50;
        FAIL_RET(OnSetRGBGain((WPARAM)nValue  , 0));
        swpa_thread_sleep_ms(20);
    }
    //调整G最小到最大; 步长：1/10 *（最大G值-最小G值），耗时：2秒
    for (int i = 1 ; i <= 50; i++)
    {
        nValue[1] = 255 *i/50;
        FAIL_RET(OnSetRGBGain((WPARAM)nValue , 0));
        swpa_thread_sleep_ms(20);
    }
    //调整B最小到最大; 步长：1/10 *（最大B值-最小B值），耗时：2秒
    for (int i = 1 ; i <= 50; i++)
    {
        nValue[2] = 255 *i/50;
        FAIL_RET(OnSetRGBGain((WPARAM)nValue , 0));
        swpa_thread_sleep_ms(20);
    }

    //恢复增益
    FAIL_RET(this->OnSetAGCGain((WPARAM)fAGCGain,NULL));
    //恢复快门
    FAIL_RET(this->OnSetShutter((WPARAM)fShutter,NULL));

    //恢复 R增益,G增益,B增益
    FAIL_RET(this->OnSetRGBGain((WPARAM)rgiValue,NULL));
    //恢复AGC\AWB状态
    FAIL_RET(OnSetAGCEnable(fAGCEnable , 0));
    FAIL_RET(OnSetAWBEnable(fAWBEnable , 0));

	//补光灯
	DWORD dwLedState;
	FAIL_RET(OnGetLEDSwitch(0,(LPARAM)&dwLedState));
	
	FAIL_RET(OnSetLEDSwitch((0 ==dwLedState)?1:0 ,0));
	swpa_thread_sleep_ms(1000);
    FAIL_RET(OnSetAWBEnable(dwLedState , 0));
	
    SW_TRACE_DEBUG("%s %d end \n",__FUNCTION__,__LINE__);
    return S_OK;
}


/**
* @brief MSG_SET_GAMMA_STRENGTH 设置相机伽玛
* @param [in] wParam 范围：10～22
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetGammaStrength(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_GAMMA);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_GAMMA ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_GAMMA_STRENGTH 读取相机伽玛
* @param [in] wParam 保留
* @param [out] lParam 范围：10～22
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetGammaStrength(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_GAMMA);
    return 0 == swpa_ipnc_control(0, CMD_GET_GAMMA ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_SET_GAMMA_ENABLE 设置相机伽玛使能
* @param [in] wParam  保留
* @param [out] lParam 0：不使能，1：使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetGammaEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_GAMMA_ENABLE);
    DWORD Value = (DWORD)wParam;
    return 0 == swpa_ipnc_control(0, CMD_SET_GAMMA_ENABLE ,(void*)&Value , sizeof(DWORD*), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_GAMMA_ENABLE 获取相机伽玛使能
* @param [in] wParam  0：不使能，1：使能
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetGammaEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_GAMMA_ENABLE);
    return 0 == swpa_ipnc_control(0, CMD_GET_GAMMA_ENABLE ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
}



/**
* @brief MSG_SET_GAM_FPGA_REG 设置相机FGPA写
* @param [in] wParam 范围不限
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCamFpgaReg(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d  %x %x  cmd = %d \n",__FUNCTION__,__LINE__,((DWORD *)wParam)[0],((DWORD *)wParam)[1],CMD_SET_FPGA_REG);
    return  0 == swpa_ipnc_control(0, CMD_SET_FPGA_REG ,(void*)wParam , sizeof(DWORD ) *2, 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_GAM_FPGA_REG 设置相FGPA读
* @param [in] wParam 保留
* @param [out] lParam 范围不限
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetCamFpgaReg(WPARAM wParam,LPARAM lParam)
{

    SW_TRACE_DEBUG("%s %d  %x %x cmd = %d\n",__FUNCTION__,__LINE__,((INT *)lParam)[0],((INT *)lParam)[1],CMD_GET_FPGA_REG);
    HRESULT hd = swpa_ipnc_control(0, CMD_GET_FPGA_REG ,(void*)lParam , sizeof(DWORD*), 0);
    return hd;
}

/**
* @brief MSG_SET_AGCPARAM 设置AGC参数
* @param [in] wParam  DWORD[4]
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetAGCParam(WPARAM wParam,LPARAM lParam)
{
    DWORD* pdwParam = (DWORD*)wParam;
    SW_TRACE_DEBUG("%s %d %d %d %d %d \n ",__FUNCTION__,__LINE__,pdwParam[0],pdwParam[1],pdwParam[2],pdwParam[3]);
    DWORD Shutter = pdwParam[0] | pdwParam[1] << 16;
    DWORD Gain =  pdwParam[2] | pdwParam[3] << 16;
    OnSetAGCShutterRange(Shutter,lParam);
    OnSetAGCGainRange(Gain,lParam);
    return S_OK;
}

/**
* @brief MSG_GET_AGCPARAM 获取AGC参数
* @param [in] wParam  保留
* @param [out] lParam DWORD[4]
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetAGCParam(WPARAM wParam,LPARAM lParam)
{
    DWORD* pdwParam = (DWORD*)lParam;
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_SATURATION_CONTRAST_STATE);
    DWORD Shutter;
    DWORD Gain;
    OnGetAGCShutterRange(wParam,Shutter);
    OnGetAGCGainRange(wParam,Gain);
    pdwParam[0] = Shutter & 0xffff;
    pdwParam[1] = (Shutter & 0xffff0000) >>16;
    pdwParam[2] = Gain & 0xffff;
    pdwParam[3] = (Gain & 0xffff0000) >> 16;
    return S_OK;
}


/**
* @brief MSG_SET_EDGE_ENHANCE 图像边缘增强
* @param [in] wParam BOOL
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetEdgeEnhance(WPARAM wParam,LPARAM lParam)
{//新增待测
    DWORD Value = (DWORD)wParam;
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_EDGE_ENHANCE);
    return 0 == swpa_ipnc_control(0, CMD_SET_EDGE_ENHANCE ,(void*)&Value , sizeof(DWORD*), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_EDGE_ENHANCE 图像边缘增强
* @param [in] wParam 保留
* @param [out] lParam BOOL*
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetEdgeEnhance(WPARAM wParam,LPARAM lParam)
{//新增待测
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_EDGE_ENHANCE);
    return 0 == swpa_ipnc_control(0, CMD_GET_EDGE_ENHANCE ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_MANUAL_VALUE 获取手动RGB 快门增益
* @param [in] wParam 保留
* @param [out] lParam
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT  CSWCameralControlMSG::OnGetManualParamValue(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
    if(lParam == NULL)
        return E_FAIL;
    INT *iValue=(INT*)lParam;
    iValue[0] = m_iManualShutter;
    iValue[1] = m_iManualGain;
    iValue[2] = m_iManualGainR;
    iValue[3] = m_iManualGainG;
    iValue[4] = m_iManualGainB;
    return S_OK;
}
/**
* @brief 获取相机工作状态
* @param [in] wParam 保留
* @param [out] lParam
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetCameraWorkState(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
    if(lParam == NULL)
        return E_FAIL;
    CHAR *szStr = (CHAR *)lParam;

	FLOAT fltJpegFps = 0.0, fltH264Fps = 0.0;
	CSWMessage::SendMessage(MSG_SOURCE_GET_JPEG_RAW_FPS, 0, (LPARAM)&fltJpegFps);
	CSWMessage::SendMessage(MSG_SOURCE_GET_H264_RAW_FPS, 0, (LPARAM)&fltH264Fps);

	if (12.5 - 1 > fltJpegFps || 12.5 + 1 < fltJpegFps
		|| 25.0 - 2 > fltH264Fps || 25.0 + 2 < fltH264Fps)
	{
		SW_TRACE_NORMAL("Info: JPEG fps:%f, H264 %f\n",fltJpegFps, fltH264Fps);
		swpa_sprintf(szStr,"异常:jpeg帧率:%.1f H264帧率:%.1f.",fltJpegFps,fltH264Fps);
	}
	else
	{
		swpa_sprintf(szStr,"正常:jpeg帧率:%.1f H264帧率:%.1f.",fltJpegFps,fltH264Fps);
	}
    return S_OK;
}

/**
* @brief MSG_SET_LED_SWITCH 设置补光灯开关
* @param [in] wParam 保留
* @param [out] lParam
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetLEDSwitch(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);

	DWORD dwSwitch = (DWORD)wParam;
	
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_NORMAL_LED;
	if (0 != swpa_ipnc_control(0, CMD_GET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0))
	{
	    SW_TRACE_NORMAL("OnSetLEDSwitch CMD_GET_IO_ARG failed!");
	    return E_FAIL;
	}

	//使能的话输出相同极性，不使能输出相反极性
	tIOArg.enable = dwSwitch;
	tIOArg.coupling = 0;
	tIOArg.freq_num = 1;
	//tIOArg.polarity = m_dwLEDPolarity;	//LED极性
	//tIOArg.output_type = m_dwLEDOutputType;		//输出类型
	//tIOArg.pulse_width = m_dwLEDPluseWidth;

	if (0 != swpa_ipnc_control(0, CMD_SET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0))
	{
	    return E_FAIL;
	}
	return S_OK;
}


/**
* @brief MSG_GET_LED_SWITCH 获取补光灯状态
* @param [in] wParam 保留
* @param [out] lParam
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetLEDSwitch(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	if(lParam == NULL)
		 return E_FAIL;

	DWORD* pdwParam = (DWORD*)lParam;
	
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_NORMAL_LED;
	
	if(0 != swpa_ipnc_control(0, CMD_GET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0))
	{
		return E_FAIL;
	}
	
	pdwParam[0] = tIOArg.enable;//(tIOArg.polarity == m_dwLEDPolarity)?1:0;
	//SW_TRACE_DEBUG("Get LED switch %d,polarity %d outputtype %d\n",
	//	pdwParam[0],tIOArg.polarity,tIOArg.output_type);
	
	return S_OK;
}

/**
* @brief MSG_SET_TRIGGEROUT_STATE 触发输出
* @param [in] wParam 0-开始, 1-结束
* @param [out] lParam
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetTriggerOutState(WPARAM wParam,LPARAM lParam)
{
	DWORD dwState = (DWORD)wParam;
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_TRIGGEROUT;
	tIOArg.polarity = (0 == dwState) ? m_dwTriggerOutPolarity : (!m_dwTriggerOutPolarity);
	tIOArg.output_type = m_dwTriggerOutOutputType;

	SW_TRACE_DEBUG("Set Trigger out state: %d,polarity %d, outputtype %d m_dwTriggerOutPolarity=%d\n",
			dwState,tIOArg.polarity,tIOArg.output_type,m_dwTriggerOutPolarity);
	
	return 0 == swpa_ipnc_control(0, CMD_SET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0)?S_OK:E_FAIL;
}



/**
* @brief MSG_SET_TRIGGEROUT 设置触发输出
* @param [in] polarity output_type 
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetTriggerOut(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//polarity output_type
	DWORD *pParam = (DWORD *)wParam;
	DWORD dwPolarity = pParam[0];
	DWORD dwOutputType = pParam[1];
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_TRIGGEROUT;
	tIOArg.enable = 1;
	tIOArg.coupling = 0;
	tIOArg.polarity = dwPolarity;
	tIOArg.output_type = dwOutputType;
	tIOArg.pulse_width = m_dwLEDPluseWidth; //todo huanggr
	
	if (0 != swpa_ipnc_control(0, CMD_SET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0))
	{
		SW_TRACE_NORMAL("OnSetTriggerOut CMD_SET_IO_ARG Failed");
		return E_FAIL;
	}
	m_dwTriggerOutPolarity = dwPolarity;
	m_dwTriggerOutOutputType = dwOutputType;
	
	return S_OK;
}

/**
* @brief MSG_GET_EXP_IO 读取触发输出口状态
* @param [in] 保留
* @param [out] polarity output_type
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetTriggerOut(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//polarity output_type

	if(lParam == NULL)
		return E_FAIL;

	DWORD* pdwParam = (DWORD*)lParam;
	
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_TRIGGEROUT;
	
	if(0 != swpa_ipnc_control(0, CMD_GET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0))
	{
		SW_TRACE_NORMAL("OnGetTriggerOut CMD_GET_IO_ARG Failed\n");
		return E_FAIL;
	}
	
	pdwParam[0] = tIOArg.polarity;
	pdwParam[1] = tIOArg.output_type;
	//SW_TRACE_DEBUG("Get TriggerOut IO state polarity %d outputtype %d\n",
	//	tIOArg.polarity,tIOArg.output_type);
	
	return S_OK;
}

HRESULT CSWCameralControlMSG::OnSetF1TriggerOut(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//polarity output_type
	DWORD *pParam = (DWORD *)wParam;
	// DWORD dwPolarity = pParam[0];
	// DWORD dwOutputType = pParam[1];
	tFPGA_IO_ARG tIOArg;
	tIOArg.type 		= E_TRIGGEROUT;
	tIOArg.enable 		= pParam[0];
	tIOArg.coupling 	= pParam[1];
	tIOArg.output_type 	= pParam[2];
	tIOArg.polarity 	= pParam[3];
	tIOArg.pulse_width	= pParam[4];

//	tIOArg.pulse_width = m_dwLEDPluseWidth; //todo huanggr
	
	if (0 != swpa_ipnc_control(0, CMD_SET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0))
	{
		SW_TRACE_NORMAL("OnSetTriggerOut CMD_SET_IO_ARG Failed");
		return E_FAIL;
	}
	m_dwTriggerOutPolarity 		= tIOArg.polarity;
	m_dwTriggerOutOutputType 	= tIOArg.output_type;
	
	return S_OK;
}


HRESULT CSWCameralControlMSG::OnGetF1TriggerOut(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//polarity output_type

	if(lParam == NULL)
		return E_FAIL;

	DWORD* pdwParam = (DWORD*)lParam;
	
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_TRIGGEROUT;
	
	if(0 != swpa_ipnc_control(0, CMD_GET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0))
	{
		SW_TRACE_NORMAL("OnGetTriggerOut CMD_GET_IO_ARG Failed\n");
		return E_FAIL;
	}
	pdwParam[0] = tIOArg.enable;
	pdwParam[1] = tIOArg.coupling;
	pdwParam[2] = tIOArg.output_type;
	pdwParam[3] = tIOArg.polarity;
	pdwParam[4] = tIOArg.pulse_width;

	//SW_TRACE_DEBUG("Get TriggerOut IO state polarity %d outputtype %d\n",
	//	tIOArg.polarity,tIOArg.output_type);
	
	return S_OK;
}

HRESULT CSWCameralControlMSG::OnSetAlarmTriggerOut(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//polarity output_type
	DWORD *pParam = (DWORD *)wParam;
	// DWORD dwPolarity = pParam[0];
	// DWORD dwOutputType = pParam[1];
	tFPGA_IO_ARG tIOArg;
	tIOArg.type 		= E_ALARM_OUT;
	tIOArg.enable 		= pParam[0];
	tIOArg.coupling 	= pParam[1];
	tIOArg.output_type 	= pParam[2];
	tIOArg.polarity 	= pParam[3];
	tIOArg.pulse_width	= pParam[4];

//	tIOArg.pulse_width = m_dwLEDPluseWidth; //todo huanggr
	
	if (0 != swpa_ipnc_control(0, CMD_SET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0))
	{
		SW_TRACE_NORMAL("OnSetTriggerOut CMD_SET_IO_ARG Failed");
		return E_FAIL;
	}
	m_dwTriggerOutPolarity 		= tIOArg.polarity;
	m_dwTriggerOutOutputType 	= tIOArg.output_type;
	
	return S_OK;
}


HRESULT CSWCameralControlMSG::OnGetAlarmTriggerOut(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//polarity output_type

	if(lParam == NULL)
		return E_FAIL;

	DWORD* pdwParam = (DWORD*)lParam;
	
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_ALARM_OUT;
	
	if(0 != swpa_ipnc_control(0, CMD_GET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0))
	{
		SW_TRACE_NORMAL("OnGetTriggerOut CMD_GET_IO_ARG Failed\n");
		return E_FAIL;
	}
	pdwParam[0] = tIOArg.enable;
	pdwParam[1] = tIOArg.coupling;
	pdwParam[2] = tIOArg.output_type;
	pdwParam[3] = tIOArg.polarity;
	pdwParam[4] = tIOArg.pulse_width;

	//SW_TRACE_DEBUG("Get TriggerOut IO state polarity %d outputtype %d\n",
	//	tIOArg.polarity,tIOArg.output_type);
	
	return S_OK;
}

/**
* @brief MSG_SET_EXP_IO 设置补光灯控制口输出
* @param [in] polarity output_type 
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetEXPIO(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//polarity output_type
	DWORD *pParam = (DWORD *)wParam;
	DWORD dwPolarity = pParam[0];
	DWORD dwOutputType = pParam[1];
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_NORMAL_LED;
	int iRet = swpa_ipnc_control(0, CMD_GET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0);
    if (0 != iRet)
    {
        SW_TRACE_NORMAL("OnSetEXPIO CMD_GET_IO_ARG Failed!");
        return E_FAIL ;
    }

	tIOArg.polarity = dwPolarity;
	tIOArg.output_type = dwOutputType;
	//tIOArg.pulse_width = m_dwLEDPluseWidth;

	iRet = swpa_ipnc_control(0, CMD_SET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0);
	if (0 != iRet)
	{
		SW_TRACE_NORMAL("OnSetEXPIO CMD_SET_IO_ARG Failed!");
		return E_FAIL ;
	}
	
	m_dwLEDPolarity = dwPolarity;		//缓存其配置参数
	m_dwLEDOutputType = dwOutputType;
	
	return S_OK;
}


/**
* @brief MSG_GET_EXP_IO 读取补光灯控制口状态
* @param [in] 保留 
* @param [out] polarity output_type
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetEXPIO(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//polarity output_type

	if(lParam == NULL)
		return E_FAIL;

	DWORD* pdwParam = (DWORD*)lParam;
	
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_NORMAL_LED;
	
	if(0 != swpa_ipnc_control(0, CMD_GET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0))
	{
		SW_TRACE_NORMAL("OnGetEXPIO CMD_GET_IO_ARG Failed\n");
		return E_FAIL;
	}
	
	pdwParam[0] = tIOArg.polarity;
	pdwParam[1] = tIOArg.output_type;
	SW_TRACE_DEBUG("Get EXP IO state polarity %d outputtype %d\n",
		tIOArg.polarity,tIOArg.output_type);
	
	return S_OK;
}

/**
* @brief MSG_SET_EXP_PLUSEWIDTH 设置补光灯脉宽
* @param [in] pluse width
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetEXPPluseWidth(WPARAM wParam,LPARAM lParam)
{
    DWORD dwValue = (DWORD)wParam;

    tFPGA_IO_ARG tIOArg;
    tIOArg.type = E_NORMAL_LED;
    int iRet = swpa_ipnc_control(0, CMD_GET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0);
    if (0 != iRet)
    {
        SW_TRACE_NORMAL("OnSetEXPPluseWidth CMD_GET_IO_ARG Failed!");
        return E_FAIL ;
    }

    tIOArg.pulse_width = dwValue;

    iRet = swpa_ipnc_control(0, CMD_SET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0);
    if (0 != iRet)
    {
        SW_TRACE_NORMAL("OnSetEXPPluseWidth CMD_SET_IO_ARG Failed!");
        return E_FAIL ;
    }

    m_dwLEDPluseWidth = dwValue;

    // todo huanggr
    /*
    DWORD rgdwValue[2];
    rgdwValue[0] = m_dwTriggerOutPolarity;
    rgdwValue[1] = m_dwTriggerOutOutputType;
    OnSetTriggerOut((WPARAM)rgdwValue, 0);
*/
    return S_OK;
}

/**
* @brief MSG_SET_ALARM_IO 设置告警控制口输出
* @param [in] polarity output_type 
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetALMIO(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//polarity output_type
	DWORD *pParam = (DWORD *)wParam;
	DWORD dwPolarity = pParam[0];
	DWORD dwOutputType = pParam[1];
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_ALARM_OUT;
	tIOArg.polarity = dwPolarity;
	tIOArg.output_type = dwOutputType;

	return 0 == swpa_ipnc_control(0, CMD_SET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0)?S_OK:E_FAIL;
}


/**
* @brief MSG_GET_ALARM_IO 读取告警控制口状态
* @param [in] 保留
* @param [out] polarity output_type 
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetALMIO(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//polarity output_type
	if(lParam == NULL)
		return E_FAIL;

	DWORD* pdwParam = (DWORD*)lParam;
	
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_ALARM_OUT;
	
	if(0 != swpa_ipnc_control(0, CMD_GET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0))
	{
		SW_TRACE_NORMAL("OnGetALMIO CMD_GET_IO_ARG Failed\n");
		return E_FAIL;
	}
	
	pdwParam[0] = tIOArg.polarity;
	pdwParam[1] = tIOArg.output_type;
	SW_TRACE_DEBUG("Get ALM IO state polarity %d outputtype %d\n",
		tIOArg.polarity,tIOArg.output_type);
	
	return S_OK;
}


/**
* @brief MSG_SET_TG_IO 设置触发抓拍口触发类型
* @param [in] 触发沿
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetTGIO(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//EgdeType
	DWORD dwEgdeType = (DWORD)wParam;
	SW_TRACE_DEBUG("OnSetTGIO EgdeType %d\n",dwEgdeType);

	return (0 == swpa_ipnc_control(0, CMD_SET_EDGE_MOD ,(void*)&dwEgdeType , sizeof(DWORD *), 0))?S_OK:E_FAIL;
}


/**
* @brief MSG_GET_TG_IO 读取触发抓拍口触发类型
* @param [in] 保留
* @param [out] 触发沿
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetTGIO(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//EgdeType
	if(lParam == NULL)
		return E_FAIL;

	DWORD* pdwParam = (DWORD*)lParam;
	DWORD dwEgdeType = 0;

	if(0 != swpa_ipnc_control(0, CMD_GET_EDGE_MOD ,(void*)&dwEgdeType , sizeof(DWORD *), 0))
	{
		SW_TRACE_NORMAL("OnGetTGIO CMD_GET_EDGE_MOD Failed\n");
		return E_FAIL;
	}
	
	pdwParam[0] = dwEgdeType;
	SW_TRACE_DEBUG("OnGetTGIO EgdeType %d\n",dwEgdeType);

	return S_OK;
}

HRESULT CSWCameralControlMSG::OnSetColorGradation(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//EgdeType
	DWORD dwColor = (DWORD)wParam;
	SW_TRACE_DEBUG("OnSetColorGradation value: %d\n",dwColor);

	DWORD rgdwParam[2];
	rgdwParam[0] = 0xa30;
	rgdwParam[1] = dwColor;
	return this->OnSetCamFpgaReg((WPARAM)rgdwParam,0);
}

HRESULT CSWCameralControlMSG::OnSetCaptureEnable(WPARAM wParam, LPARAM lParam)
{
	//TODO: complete it
	return S_OK;
}

/**
* @brief MSG_SET_WORK_MODE 设置工作模式
* @param [in] 工作模式
* @param [int] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetWorkMode(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//WorkMode
	DWORD dwWorkMode = (DWORD)wParam;
	SW_TRACE_DEBUG("OnSetWorkMode dwWorkMode %d\n",dwWorkMode);

	return (0 == swpa_ipnc_control(0, CMD_SET_WORKMODE,(void*)&dwWorkMode , sizeof(DWORD *), 0))?S_OK:E_FAIL;
}

/**
* @brief MSG_SET_AWB_MODE 设置白平衡模式
* @param [in] 工作模式
* @param [int] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetAWBMode(WPARAM wParam,LPARAM lParam)
{
	DWORD dwAWBMode = (DWORD)wParam;
	SW_TRACE_DEBUG("Set AWB Mode:%d",dwAWBMode);

	return (0 == swpa_ipnc_control(0,CMD_SET_AWB_MODE,(void*)&dwAWBMode,sizeof(DWORD *),0))?S_OK:E_FAIL;
}

HRESULT CSWCameralControlMSG::OnSetCaptureSofttouch(WPARAM wParam,LPARAM lParam)
{
	const DWORD iLaneInfSoftReg = 0x92f;
	DWORD rgWriteRegParam[2];
	INT iAGCEnable = 0;
	INT iSet = 0;
	INT iInfo = 0;
	iSet = ( wParam & 0xFF000000 ) >> 24;
	iInfo =( wParam & 0X00FFFFFF);

	iSet = (iSet == 0) ? 1 : 2;

	//SW_TRACE_DEBUG("<CameralControlMSG>set:%d, info:%d.\n", iSet, iInfo);
	//if((s_dwLastInfo + 1) != iInfo)
	//	SW_TRACE_DEBUG("**********ERROR:set:%d, info:%d.\n", iSet, iInfo);
	//s_dwLastInfo = iInfo;

	//自动设置抓拍参数
	/*if(m_fCaptureAutoParamEnable)
	{
		CSWMessage::SendMessage(MSG_AUTO_CONTROL_SET_AUTO_CAPTURE_PARAM, NULL, NULL);
	}*/

	//设置道路亮灯使能
	rgWriteRegParam[0] = iLaneInfSoftReg;
	rgWriteRegParam[1] = iSet;
	OnSetCamFpgaReg((WPARAM)rgWriteRegParam, 0);


	if((0 == swpa_ipnc_control(0, CMD_SOFT_CAPTURE,(void*)&iInfo , sizeof(iInfo), 0)))
	{
		return S_OK;
	}

	return E_FAIL;
}



/**
* @brief MSG_SET_F1_OUTPUT_TYPE 设置F1输出模式
* @param [in] F1输出模式
* @param [int] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetF1OutputType(WPARAM wParam,LPARAM lParam)
{
	INT iValue = (INT)wParam ? 1 : 0;

	SW_TRACE_DEBUG("<CameralControlMSG> iValue :%d.\n", iValue);

	return (0 == swpa_ipnc_control(0, CMD_SET_FLASH_GATE, (void*)&iValue , sizeof(iValue), 0))? S_OK : E_FAIL;
}

/**
* @brief MSG_CAPTURERGB_ENABLE 抓拍RGB使能
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCaptureRGBEnable(WPARAM wParam,LPARAM lParam)
{
    int fEnable = (int)wParam;
    int rgnValue[3] = {0};
    rgnValue[0] = fEnable ? m_nCaptureR : -1;
    rgnValue[1] = fEnable ? m_nCaptureG : -1;
    rgnValue[2] = fEnable ? m_nCaptureB : -1;
    OnSetCaptureRGBGain((WPARAM)rgnValue, lParam);
    return S_OK;
}

/**
* @brief MSG_CAPTURESHUTTER_ENABLE 抓拍快门使能
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCaptureShutterEnable(WPARAM wParam,LPARAM lParam)
{
    int fEnable = (int)wParam;
    int nValue = fEnable ? m_nCaptureShutter : -1;
    OnSetCaptureShutter((WPARAM)&nValue, lParam);
    return S_OK;
}

/**
* @brief MSG_CAPTURESHUTTER_ENABLE 抓拍增益使能
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCaptureGainEnable(WPARAM wParam,LPARAM lParam)
{
    int fEnable = (int)wParam;
    int nValue = fEnable ? m_nCaptureGain : -1;
    OnSetCaptureGain((WPARAM)&nValue, lParam);
    return S_OK;
}

/**
 * @brief MSG_CAPTURESHARPEN_ENABLE 抓拍锐化使能
 * @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
 * @param [in] lParam 保留
 * @return 成功返回S_OK，其他值代表失败
 */
HRESULT CSWCameralControlMSG::OnSetCaptureSharpenEnable(WPARAM wParam,LPARAM lParam)
{
    int fEnable = (int)wParam;
    int nValue = fEnable ? m_nCapSharpen : -1;
    OnSetCaptureSharpen((WPARAM)&nValue, lParam);
    return S_OK;
}

/**
* @brief 消息接收函数
* @param [in] wParam 快门值
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCaptureShutter(WPARAM wParam,LPARAM lParam)
{
	INT nEnable = ((INT *)wParam)[1];
    INT nValue = ((INT *)wParam)[0];
//	INT iAGCEnable = 0;
    DWORD dwReg[2];

    // lParam != 0xF0F0F0F0表示手动调节独立抓拍参数，此时才能修改Enable标识
    // if (lParam != 0xF0F0F0F0)
    // {
    //     m_fEnableCaptureShutter = (nValue != -1) ? TRUE : FALSE;
    // }

    //if (m_fEnableCaptureParamDay == FALSE && m_fIsDayNow)
    //{
    //    nValue = -1;
    //}
	//OnGetAGCEnable(NULL, (LPARAM)&iAGCEnable);

	//SW_TRACE_DEBUG("%s iAGCEnable = %d Done.", __FUNCTION__, iAGCEnable);

    dwReg[0] = 0x914;
    dwReg[1] = 0;
    swpa_ipnc_control(0, CMD_GET_FPGA_REG ,(void*)dwReg, sizeof(DWORD ) *2, 0);
    if (nEnable == 0)
    {
        dwReg[1] &= 0x0e;
        return swpa_ipnc_control(0, CMD_SET_FPGA_REG, (void*)dwReg, sizeof(DWORD ) *2, 0);
    }
    else
    {
        dwReg[1] |= 0x1;
        swpa_ipnc_control(0, CMD_SET_FPGA_REG, (void*)dwReg, sizeof(DWORD ) *2, 0);

        dwReg[0] = 0x915;
        dwReg[1] = (DWORD)((float)nValue / 29.6);
        m_nCaptureShutter = nValue;
        return swpa_ipnc_control(0, CMD_SET_FPGA_REG, (void*)dwReg, sizeof(DWORD ) *2, 0);
    }
}

/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [out] lParam(INT*类型) 快门值
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetCaptureShutter(WPARAM wParam,LPARAM lParam)
{
    //swpa_camera_basicparam_get_capture_shutter((int*)lParam);
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
	INT nEnable = ((INT *)wParam)[1];
    int nValue = ((INT *)wParam)[0];
//    INT nValue = (*(int*)wParam);
	//INT iAGCEnable = 0;
    DWORD dwReg[2];

    // lParam != 0xF0F0F0F0表示手动调节独立抓拍参数，此时才能修改Enable标识
    // if (lParam != 0xF0F0F0F0)
    // {
    //     m_fEnableCaptureGain = (nValue != -1) ? TRUE : FALSE;
    // }

    //if (m_fEnableCaptureParamDay == FALSE && m_fIsDayNow)
    //{
    //    nValue = -1;
    //}
	//OnGetAGCEnable(NULL, (LPARAM)&iAGCEnable);

    dwReg[0] = 0x914;
    dwReg[1] = 0;
    swpa_ipnc_control(0, CMD_GET_FPGA_REG ,(void*)dwReg, sizeof(DWORD ) *2, 0);
    if (nEnable == 0)
    {
        dwReg[1] &= 0x0d;
        return swpa_ipnc_control(0, CMD_SET_FPGA_REG, (void*)dwReg, sizeof(DWORD ) *2, 0);
    }
    else
    {
        dwReg[1] |= 0x2;
        swpa_ipnc_control(0, CMD_SET_FPGA_REG, (void*)dwReg, sizeof(DWORD ) *2, 0);

        dwReg[0] = 0x919;
        dwReg[1] = nValue;
        m_nCaptureGain = nValue;
        return swpa_ipnc_control(0, CMD_SET_FPGA_REG, (void*)dwReg, sizeof(DWORD ) *2, 0);
    }
}

/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [out] lParam(INT* 类型) 增益值
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetCaptureGain(WPARAM wParam,LPARAM lParam)
{
    //swpa_camera_basicparam_get_capture_gain((int*)lParam);
    return S_OK;
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
    int *prgnValue = (int*)wParam;
	
    int nEnable = ((INT *)wParam)[3];
    int nValue = 0;
    // lParam != 0xF0F0F0F0表示手动调节独立抓拍参数，此时才能修改Enable标识
    // if (lParam != 0xF0F0F0F0)
    // {
    //     m_fEnableCaptureRGBGain = (prgnValue[0] == -1 || prgnValue[1] == -1 || prgnValue[2] == -1) ? FALSE : TRUE;
    // }

    //if (m_fEnableCaptureParamDay == FALSE && m_fIsM3DayNow)
    //{
    //    prgnValue[0] = -1;
    //}

    if (prgnValue[0] == -1 || prgnValue[1] == -1 || prgnValue[2] == -1)
    {
        nEnable = 0;
    }
    else
    {
        m_nCaptureR = prgnValue[0];
        m_nCaptureG = prgnValue[1];
        m_nCaptureB = prgnValue[2];
    }

    DWORD dwReg[2];

    dwReg[0] = 0x914;
    dwReg[1] = 0;
    swpa_ipnc_control(0, CMD_GET_FPGA_REG ,(void*)dwReg, sizeof(DWORD ) *2, 0);
    if (nEnable == 0)
    {
        dwReg[1] &= 0x17;
        swpa_ipnc_control(0, CMD_SET_FPGA_REG, (void*)dwReg, sizeof(DWORD ) *2, 0);
        return 0;
    }
    else
    {
        dwReg[1] |= 0x8;
        swpa_ipnc_control(0, CMD_SET_FPGA_REG, (void*)dwReg, sizeof(DWORD ) *2, 0);

        dwReg[0] = 0x916;
        dwReg[1] = prgnValue[0] * 449;
        swpa_ipnc_control(0, CMD_SET_FPGA_REG, (void*)dwReg, sizeof(DWORD ) *2, 0);
        dwReg[0] = 0x917;
        dwReg[1] = prgnValue[1] * 449;
        swpa_ipnc_control(0, CMD_SET_FPGA_REG, (void*)dwReg, sizeof(DWORD ) *2, 0);
        dwReg[0] = 0x918;
        dwReg[1] = prgnValue[2] * 449;
        swpa_ipnc_control(0, CMD_SET_FPGA_REG, (void*)dwReg, sizeof(DWORD ) *2, 0);
    }
    return S_OK;
}

/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [out] lParam(INT数组类型)0:R,1:G,2:B
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetCaptureRGBGain(WPARAM wParam,LPARAM lParam)
{
    //int *prgnValue = (int*)lParam;
    //swpa_camera_basicparam_get_capture_rgb_gain(&prgnValue[0], &prgnValue[1], &prgnValue[2]);
    return S_OK;
}

/**
* @brief MSG_SET_CAPTURE_SHARPEN_THR 设置抓拍锐度指令
* @param [in] wParam 锐度值
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnSetCaptureSharpen(WPARAM wParam,LPARAM lParam)
{
    int nValue = (*(int*)wParam);
    DWORD dwReg[2];

    // lParam != 0xF0F0F0F0表示手动调节独立抓拍参数，此时才能修改Enable标识
    if (lParam != 0xF0F0F0F0)
    {
        m_fEnableCaptureSharpen = (nValue != -1) ? TRUE : FALSE;
    }

    //if (m_fEnableCaptureParamDay == FALSE && m_fIsDayNow)
    //{
    //    nValue = -1;
    //}

    dwReg[0] = 0x914;
    dwReg[1] = 0;
    swpa_ipnc_control(0, CMD_GET_FPGA_REG ,(void*)dwReg, sizeof(DWORD ) *2, 0);
    if (nValue == -1)
    {
        dwReg[1] &= 0xf;
        return swpa_ipnc_control(0, CMD_SET_FPGA_REG, (void*)dwReg, sizeof(DWORD ) *2, 0);
    }
    else
    {
        dwReg[1] |= 0x10;
        swpa_ipnc_control(0, CMD_SET_FPGA_REG, (void*)dwReg, sizeof(DWORD ) *2, 0);

        dwReg[0] = 0xa3c;
        dwReg[1] = nValue;
        m_nCapSharpen = nValue;
        return swpa_ipnc_control(0, CMD_SET_FPGA_REG, (void*)dwReg, sizeof(DWORD ) *2, 0);
    }
}

/**
* @brief MSG_GET_CAPTURE_SHARPEN_THR 获取抓拍锐度指令
* @param [in] wParam 保留
* @param [out] lParam 锐度值
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameralControlMSG::OnGetCaptureSharpen(WPARAM wParam,LPARAM lParam)
{
    //swpa_camera_basicparam_get_capture_sharpen((int*)lParam);
    return S_OK;
}

/**
 * @brief MSG_GET_M3_DAYNIGHT_STATUS 获取M3白天晚上标识
 * @param [in] wParam 保留
 * @param [out] lParam 标识值
 * @return 成功返回S_OK，其他值代表失败
 */
HRESULT CSWCameralControlMSG::OnGetM3DayNightStatus(WPARAM wParam,LPARAM lParam)
{
    if (lParam == NULL)
    {
        return E_POINTER;
    }

    static int s_nLastStatus = -1;
    int *pnValue = (int*)lParam;

    swpa_ipnc_control(0, CMD_GET_DAY_NIGHT, (void*)pnValue, sizeof(DWORD), 0);

    if ((*pnValue) != s_nLastStatus)
    {
        s_nLastStatus = (*pnValue);
        if (s_nLastStatus == 1)
        {
            m_fIsM3DayNow = FALSE;
        }
        else
        {
            m_fIsM3DayNow = TRUE;
        }
        //OnSetCaptureRGBEnable(m_fEnableCaptureRGBGain, 0xF0F0F0F0);
    }
    return S_OK;
}

HRESULT CSWCameralControlMSG::OnSetWDRLevel(WPARAM wParam,LPARAM lParam)
{
    int nWDRLevel = (int)wParam;
    if (SWPAR_FAIL == swpa_ipnc_control(0, CMD_SET_BACKLIGHT, (void*)&nWDRLevel , sizeof(nWDRLevel), 0))
    {
        SW_TRACE_NORMAL("设置WDR等级失败！");
        return E_FAIL;
    }
    SW_TRACE_NORMAL("设置WDR等级成功[%d]", nWDRLevel);
    return S_OK;
}

HRESULT CSWCameralControlMSG::OnSetCaptureAutoParamEnable(WPARAM wParam,LPARAM lParam)
{
	m_fCaptureAutoParamEnable = ((INT)wParam == 1) ? TRUE : FALSE;
	return S_OK;
}

HRESULT CSWCameralControlMSG::OnGetCaptureAutoParamEnable(WPARAM wParam,LPARAM lParam)
{
	INT *pEnable = (INT *)lParam;
	*pEnable = (m_fCaptureAutoParamEnable ? 1 : 0);
	return S_OK;
}

BOOL CSWCameralControlMSG::OnIncreaseBrightness()
{
	DWORD rgFpgaParam[2] = {0};
	//以下为提亮图像设置， 不开放寄存器意义  __WRITE_FPGA_START__
	rgFpgaParam[0] = 0xb81;		//gamma 提亮
	rgFpgaParam[1] = 0x02;
	OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);


	rgFpgaParam[0] = 0xb83;		//数字增益提亮
    rgFpgaParam[1] = 0x00;
	OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);

	rgFpgaParam[0] = 0xa3c;
	rgFpgaParam[1] = 0xd0;
	OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);

	rgFpgaParam[0] = 0xa84;
	rgFpgaParam[1] = 0x03;
	OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);
	// __ WRITE_FPGA_END__

	return TRUE;
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


