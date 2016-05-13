
#include "SWFC.h"
#include "SWCameraControl2AMSG.h"
#include "SWPTZParameter.h"

//#define SW_TRACE_DEBUG SW_TRACE_DEBUG
//#define SW_TRACE_DEBUG
//REGISTER_CLASS(CSWCameraControl2AMSG)

CSWCameraControl2AMSG::CSWCameraControl2AMSG()
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
	m_IsDay=TRUE;
	swpa_memset(rgWriteRegParam,0,sizeof(DWORD)*2);
}

CSWCameraControl2AMSG::~CSWCameraControl2AMSG()
{

}
#define FAIL_RET( func ) if( S_OK != func ){SW_TRACE_DEBUG("=========ERROR!=========");return E_FAIL;}
/**
* @brief MSG_INITIAL_PARAM 设置设置相机初始化参数结构体
* @param [in] wParam （CamAppParam*结构指针类型）
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnInitialCameralParam( WPARAM wParam,LPARAM lParam )
{
    LPCAMERAPARAM_INFO pCameraParam = (LPCAMERAPARAM_INFO)wParam;

	SW_TRACE_DEBUG("%s %d pCameraParam = 0x%x\n",__FUNCTION__,__LINE__,pCameraParam);
    if( pCameraParam == NULL )
        return S_FALSE;

//    // 临时测试
//    {
//        DWORD dwParam[5];
//        //分车道闪需要同时设置两个灯
//        dwParam[0] = pCameraParam->iFlashEnable;
//        dwParam[1] = pCameraParam->iFlashCoupling;
//        dwParam[2] = pCameraParam->iFlashOutputType;
//        dwParam[3] = pCameraParam->iFlashPolarity;
//        dwParam[4] = pCameraParam->iFlashPluseWidth;
//    //	FAIL_RET(this->OnSetF1TriggerOut((WPARAM)dwParam, 0));
//    //	FAIL_RET(this->OnSetAlarmTriggerOut((WPARAM)dwParam, 0));

//        SW_TRACE_DEBUG("SetF1 Alarm Params : <FlashEnable:%d, FlashCoupling:%d, FlashOutputType:%d, FlashPolarity:%d, FlashPluseWidth:%d, F1OutputType:%d>.\n",
//                       pCameraParam->iFlashEnable, pCameraParam->iFlashCoupling, pCameraParam->iFlashOutputType,
//                       pCameraParam->iFlashPolarity, pCameraParam->iFlashPluseWidth, pCameraParam->iF1OutputType);

//        OnSetF1TriggerOut((WPARAM)dwParam, 0);
//        OnSetAlarmTriggerOut((WPARAM)dwParam, 0);
//    }

    // 设置 928  为  2， 完善红外白平衡白平衡
    DWORD dwRegValueAWB[2] = {0};
    dwRegValueAWB[0] = 0xa98;
    dwRegValueAWB[1] = 2;
    CSWMessage::SendMessage(MSG_SET_CAM_FPGA_REG, (WPARAM)dwRegValueAWB, NULL);

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

//    //设置红灯加红使能
//    FAIL_RET(this->OnSetRedLightEnable( pCameraParam->fRedLightEnable , 0));

//    FAIL_RET(this->OnSetRedLightThreshold( (WPARAM)pCameraParam->iLumLThreshold , 0));

//	//设置绿灯加绿
//	//2014-05-06: 目前绿灯加绿与红灯加红共用同一个使能标志
//	//即使能红灯加红则同时使能绿灯加绿，叫"交通灯颜色加强"更合理一些
//	FAIL_RET(this->OnSetGreenLightThreshold( (WPARAM)pCameraParam->iLumLThreshold , 0));

//    // 设置DC光圈使能
//    FAIL_RET(this->OnSetDCApertureEnable( pCameraParam->iEnableDCAperture , 0));
    //设置电网同步使能
    FAIL_RET(this->OnSetACSyncEnable( pCameraParam->iExternalSyncMode ,0));
    //设置电网同步延时
    FAIL_RET(this->OnSetACSyncDelay( pCameraParam->iSyncRelay,0 ));
    //设置滤光片切换
    // todo. 初始化都是不使能。
    //FAIL_RET(this->OnSetFilterSwitch( 0, 0));//发现有小概率卡住在这里，电警卡口方案不需要滤光片，所以暂时直接屏蔽
    //设置黑白图使能
    FAIL_RET(this->OnSetGrayImageEnable( pCameraParam->fGrayImageEnable , 0));

//    //设置CVBS制式
//    FAIL_RET(this->OnSetCVBSStd( pCameraParam->iCVBSMode , 0));

    // 去掉GAMMA的功能设置。
    //FAIL_RET(this->OnSetGammaStrength( pCameraParam->iGammaValue , 0));
    //FAIL_RET(this->OnSetGammaEnable( pCameraParam->iGammaEnable , 0));
    FAIL_RET(this->OnSetEdgeEnhance( pCameraParam->iEdgeValue, 0));

    // 先关闭灯
    OnSetLEDSwitch(0, 0);

	//补光灯控制 EXP IO
    DWORD dwParam[5];	//polarity output_type
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
//	FAIL_RET(this->OnSetF1TriggerOut((WPARAM)dwParam, 0));
//	FAIL_RET(this->OnSetAlarmTriggerOut((WPARAM)dwParam, 0));

    SW_TRACE_DEBUG("SetF1 Alarm Params : <FlashEnable:%d, FlashCoupling:%d, FlashOutputType:%d, FlashPolarity:%d, FlashPluseWidth:%d, F1OutputType:%d>.\n",
                   pCameraParam->iFlashEnable, pCameraParam->iFlashCoupling, pCameraParam->iFlashOutputType,
                   pCameraParam->iFlashPolarity, pCameraParam->iFlashPluseWidth, pCameraParam->iF1OutputType);

	//设置道路亮灯使能
    OnSetF1TriggerOut((WPARAM)dwParam, 0);
	
    OnSetAlarmTriggerOut((WPARAM)dwParam, 0);	
	
	// FAIL_RET(this->OnSetTriggerOut((WPARAM)dwParam,0));

	//报警输出 ALM IO
	// dwParam[0] = pCameraParam->iALMPolarity; //报警输出极性
	// dwParam[1] =pCameraParam->iALMOutputType;	//报警输出类型
	// FAIL_RET(this->OnSetALMIO((WPARAM)dwParam,0));

//    //F1输出配置
//    FAIL_RET(this->OnSetF1OutputType((WPARAM)pCameraParam->iF1OutputType, 0));
     SW_TRACE_DEBUG("SetF1 OutputType.\n");
    OnSetF1OutputType((WPARAM)pCameraParam->iF1OutputType, 0);
	
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
//     OnSetCaptureShutterEnable(pCameraParam->iCaptureShutterEnable, 0xF0F0F0F0);
//     OnSetCaptureGainEnable(pCameraParam->iCaptureGainEnable, 0xF0F0F0F0);
    //OnSetCaptureSharpenEnable(pCameraParam->iCaptureSharpenEnable, 0xF0F0F0F0);

    DWORD dwRegValue[2] = {0};
    dwRegValue[0] = 0x914;
    dwRegValue[1] = 0;
    CSWMessage::SendMessage(MSG_GET_CAM_FPGA_REG, NULL, (LPARAM)dwRegValue);

    if((dwRegValue[1] & 0x3) != 0x3)
    {
        dwRegValue[1] |= 0x3;	//强制使能抓拍快门和增益
        CSWMessage::SendMessage(MSG_SET_CAM_FPGA_REG, (WPARAM)dwRegValue, NULL);
    }

	DWORD WriteRegParam[2]={0x92f,0x0};
	OnSetCamFpgaReg((WPARAM)WriteRegParam, 0);
	
#if 0	
	INT rgCaptureShutterValue[2] = {0};
	INT rgCaptureGainValue[2] = {0};

	rgCaptureShutterValue[0] = pCameraParam->iCaptureShutter;
	rgCaptureShutterValue[1] = 1;//pCameraParam->iCaptureShutterEnable;
	rgCaptureGainValue[0]    = pCameraParam->iCaptureGain;
	rgCaptureGainValue[1]	 = 1;//pCameraParam->iCaptureGainEnable;

	OnSetCaptureGain((WPARAM)rgCaptureGainValue, NULL);
	OnSetCaptureShutter((WPARAM)rgCaptureShutterValue, NULL);
#else
    OnSetCaptureGain((WPARAM)&(pCameraParam->iCaptureGain), NULL);
    OnSetCaptureShutter((WPARAM)&(pCameraParam->iCaptureShutter), NULL);
#endif
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
HRESULT CSWCameraControl2AMSG::OnSetColoMaxtrixEnable(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
    SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_SATURATION_CONTRAST_STATE, Value);

	
	
	
	
	
	return SendMessage(MSG_2A_SET_SATURATION_CONTRAST_STATE, (WPARAM)&Value , sizeof(DWORD));
}

/**
* @brief MSG_GET_COLORMATRIX_ENABLE,获取使能图像增强
* @param [in] wParam 保留
* @param [in] lParam 0:关闭，1：打开
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetColoMaxtrixEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_SATURATION_CONTRAST_STATE);
	return SendMessage( MSG_2A_GET_SATURATION_CONTRAST_STATE, sizeof(DWORD),lParam);
}



/**
* @brief MSG_SET_SHARPEN_ENABLE,设置锐化使能
* @param [in] wParam 0为关闭锐化，1锐化所有帧
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetSharpeEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_SHARPNESS_STATE);
    DWORD Value = (DWORD)wParam;
	//todo 已经不支持
	SW_TRACE_DEBUG("%s not support!\n",__FUNCTION__);
	return E_FAIL;
	//return SendMessage( MSG_2A_SET_SHARPNESS_STATE, (WPARAM)&Value , sizeof(DWORD));
    //return 0 == swpa_ipnc_control(0, CMD_SET_SHARPNESS_STATE ,(void*)&Value, sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_SHARPEN_ENABLE,获取锐化使能
* @param [in] wParam 保留
* @param [in] lParam 类型： DWORD*， 0为关闭锐化，1锐化所有帧
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetSharpeEnable(WPARAM wParam,LPARAM lParam)
{
    //SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_SHARPNESS_STATE);//
    // return 0 == swpa_ipnc_control(0, CMD_GET_SHARPNESS_STATE ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
    return E_FAIL;
}

/**
* @brief MSG_SET_SHARPENTHRESHOLD,锐化阀值
* @param [in] wParam 锐化阀值[0~100]，推荐使用7
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetSharpeThreshold(WPARAM wParam,LPARAM lParam)
{
    //SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_SHARPNESS);
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
HRESULT CSWCameraControl2AMSG::OnGetSharpeThreshold(WPARAM wParam,LPARAM lParam)
{
    //SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_SHARPNESS);
    // return 0 == swpa_ipnc_control(0, CMD_GET_SHARPNESS ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
    return E_FAIL;
}

/**
* @brief MSG_SET_CONTRASTTHRESHOLD,设置对比度
* @param [in] wParam 类型：DWORD ,[-100~100]
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetContrastThreshold(WPARAM wParam,LPARAM lParam)
{
	DWORD Value = (DWORD)wParam;
    SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_CONTRAST, Value);

	
	
	
	
	
	return SendMessage( MSG_2A_SET_CONTRAST, (WPARAM)&Value , sizeof(DWORD));
}

/**
* @brief MSG_GET_CONTRASTTHRESHOLD,获取对比度
* @param [in] wParam 保留
* @param [in] lParam 类型：DWORD* , [-100~100]
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetContrastThreshold(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_CONTRAST);

	
	
	
	
	
	return SendMessage( MSG_2A_GET_CONTRAST, sizeof(DWORD), lParam );
}

/**
* @brief MSG_SET_SATURATIONTHRESHOLD,设置饱和度
* @param [in] wParam 饱和度[-100~100]
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetSaturationThreshold(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_SATURATION, Value);

	
	
	
	
	
	return SendMessage( MSG_2A_SET_SATURATION, (WPARAM)&Value , sizeof(DWORD));
}

/**
* @brief MSG_SET_SATURATIONTHRESHOLD,获取饱和度
* @param [in] wParam 保留
* @param [in] lParam 类型：DWORD* ,饱和度[-100~100]
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetSaturationThreshold(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_SATURATION);

	
	
	
	
	
	return SendMessage( MSG_2A_GET_SATURATION, sizeof(DWORD), lParam );
}

/**
* @brief MSG_SET_AGCTH 设置AGC图像亮度期望值
* @param [in] wParam 期望值
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetAGCTh( WPARAM wParam,LPARAM lParam )
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_AE_THRESHOLD, Value);

	
	
	
	
	
	return SendMessage( MSG_2A_SET_AE_THRESHOLD, (WPARAM)&Value , sizeof(DWORD));
}

/**
* @brief MSG_SET_SHU_RANGE 设置AGC曝光时间范围
* @param [in] wParam 曝光范围DWORD类型：4字节,0~15：最小曝光时间16~32：最大曝光时间
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetAGCShutterRange( WPARAM wParam,LPARAM lParam )
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_AE_EXP_RANGE, Value);
    SendMessage(MSG_AUTO_SET_MAXAGCSHUTTER, Value >> 16, 0);

	
	
	
	
	
	return SendMessage( MSG_2A_SET_AE_EXP_RANGE, (WPARAM)&Value , sizeof(DWORD));
}
/**
* @brief MSG_GET_SHU_RANGE 获取AGC曝光时间范围
* @param [in] wParam 保留
* @param [out] lParam 曝光范围DWORD*类型：4字节0~15：最小曝光时间16~32：最大曝光时间
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetAGCShutterRange( WPARAM wParam,LPARAM lParam )
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AE_EXP_RANGE);

	
	
	
	
	
	return SendMessage( MSG_2A_GET_AE_EXP_RANGE, sizeof(DWORD), lParam );
}

/**
* @brief MSG_SET_SHU_RANGE 设置AGC增益范围
* @param [in] wParam DWORD类型：4字节0~15：最小增益时间16~32：最大增益时间
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetAGCGainRange( WPARAM wParam,LPARAM lParam )
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_AE_GAIN_RANGE, Value);

	
	
	
	
	
	return SendMessage( MSG_2A_SET_AE_GAIN_RANGE, (WPARAM)&Value , sizeof(DWORD));
}

/**
 * @brief MSG_SET_AGC_SCENE 设置AGC情景模式
 * @param [in] wParam DWORD类型：4字节，0：自动, 1: 情景1, 2: 情景2
 * @param [in] lParam 保留
 * @return 成功返回S_OK，其他值代表失败
 */
HRESULT CSWCameraControl2AMSG::OnSetAGCScene( WPARAM wParam,LPARAM lParam )
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_SCENE, Value);

	
	
	
	
	
	if (S_OK != SendMessage( MSG_2A_SET_SCENE, (WPARAM)&Value , sizeof(DWORD)))
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
HRESULT CSWCameraControl2AMSG::OnGetAGCScene( WPARAM wParam,LPARAM lParam )
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
HRESULT CSWCameraControl2AMSG::OnGetAGCGainRange( WPARAM wParam,LPARAM lParam )
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AE_GAIN_RANGE);

	
	
	
	
	
	return SendMessage( MSG_2A_GET_AE_GAIN_RANGE, sizeof(DWORD), lParam );
}

/**
* @brief MSG_SET_AGCZONE 设备AGC测光区域
* @param [in] wParam AGC测光区域使能标志INT数据，16个数据。
* @param [in] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetAGCZone(WPARAM wParam,LPARAM lParam)
{
    DWORD *rgiAGCZone = (DWORD *)wParam;
    DWORD Value = 0 ;
    for(int i = 0; i < 16; ++i)
    {
        Value |= (rgiAGCZone[i] == 1) <<i ;
    }
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_AE_ZONE, Value);

	
	
	
	

	return SendMessage( MSG_2A_SET_AE_ZONE, (WPARAM)&Value , sizeof(DWORD));
}

/**
* @brief MSG_GET_AGCZONE 获取AGC测光区域
* @param [in] wParam 保留
* @param [in] lParam AGC测光区域使能标志INT数据，16个数据。
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetAGCZone(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AE_ZONE);
    DWORD *rgiAGCZone = (DWORD *)lParam;
    DWORD nValue = 0 ;

	
	
	
	

	HRESULT hr = SendMessage( MSG_2A_GET_AE_ZONE, sizeof(DWORD), (LPARAM)&nValue);
    if(S_OK == hr)
    {
        //SW_TRACE_DEBUG("%s %d   nValue= %d \n",__FUNCTION__,__LINE__,nValue);
        for(int i = 0; i < 16; ++i)
        {
            rgiAGCZone[i] = (nValue >> i) & 0x1 ;
            //SW_TRACE_DEBUG("%s %d   rgiAGCZone[%d] = %d \n",__FUNCTION__,__LINE__,i,(nValue >> i) & 0x1);
        }
        return S_OK;
    }
    SW_TRACE_NORMAL("%s %d  error Ret= 0x%08x \n",__FUNCTION__,__LINE__,hr);
    return E_FAIL;

}


/**
* @brief MSG_SET_AGCTH 获取AGC图像亮度期望值
* @param [in] wParam 保留
* @param [out] lParam （DWORD*类型） 期望值返回
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetAGCTh( WPARAM wParam,LPARAM lParam )
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AE_THRESHOLD);

	
	
	
	
	
	return SendMessage( MSG_2A_GET_AE_THRESHOLD, sizeof(DWORD), lParam );
}
/**
* @brief 消息接收函数
* @param [in] wParam 增益值(如200)
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetAGCGain(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_GAIN, Value);
   	m_iManualGain = Value;
	
	
	
	
	
	
	return SendMessage( MSG_2A_SET_GAIN, (WPARAM)&Value , sizeof(DWORD));
}

/**
* @brief 消息接收函数
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetAGCEnable(WPARAM wParam,LPARAM lParam)
{
    BOOL Value = (BOOL)wParam;
    SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_AE_STATE, Value);

	
	
	
	

	return SendMessage( MSG_2A_SET_AE_STATE, (WPARAM)&Value , sizeof(BOOL));
}
/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [in] lParam 使能消息，使能标记，0=不使能，1 = 使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetAGCEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AE_STATE);

	
	
	
	
	
	return SendMessage( MSG_2A_GET_AE_STATE, sizeof(DWORD), lParam );
}

/**
* @brief 消息接收函数
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetAWBEnable(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
    SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_AWB_STATE, Value);

	
	
	
	
	
	return SendMessage( MSG_2A_SET_AWB_STATE, (WPARAM)&Value , sizeof(BOOL));
}
/**
* @brief MSG_GET_AWBENABLE 消息接收函数
* @param [in] wParam 保留
* @param [in] lParam 使能消息，使能标记，0=不使能，1 = 使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetAWBEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d  cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AWB_STATE);

	
	
	
	
	
	return SendMessage( MSG_2A_GET_AWB_STATE, sizeof(BOOL), lParam);
}


/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [out] lParam（INT*类型） 增益返回值(200)
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetAGCGain(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_GAIN);

	
	
	
	

	
	return SendMessage( MSG_2A_GET_GAIN, sizeof(DWORD), lParam);
}


/**
* @brief 消息接收函数
* @param [in] wParam RGB增益(0到7位为R通道值，8到15位G通道值，16到23为B通道值,例如:0x00AFAAFF
* ,分别为FF为B通道增益，AA为G通道增益，AF为R通道增益值)
* 如是使能消息，则为使能标记0=不使能，1 = 使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetRGBGain(WPARAM wParam,LPARAM lParam)
{
    DWORD *tmp = (DWORD *)wParam;
    if(tmp == NULL || tmp+1 == NULL || tmp+2== NULL)
    {
        //SW_TRACE_DEBUG("%s %d Value error \n",__FUNCTION__,__LINE__);
        return E_FAIL;
    }
    //SW_TRACE_DEBUG("%s %d  %d %d %d\n",__FUNCTION__,__LINE__,tmp[0],tmp[1],tmp[2]);
    DWORD Value;
    m_iManualGainR = tmp[0];
    m_iManualGainG = tmp[1];
    m_iManualGainB = tmp[2];
    Value = tmp[0]  | tmp[1] << 8 | tmp[2] << 16;
    SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_RGB, Value);

	
	
	
	

	
    return SendMessage( MSG_2A_SET_RGB, (WPARAM)&Value , sizeof(BOOL));

}
/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [out] lParam(DWORD*类型) RGB增益(0到7位为R通道值，8到15位G通道值，16到23为G通道值,例如:0x00AFAAFF
* ,分别为FF为B通道增益，AA为G通道增益，AF为R通道增益值)
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetRGBGain(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_RGB);
    DWORD *temp = (DWORD *)lParam;
    if(temp == NULL || temp+1 == NULL || temp +2 == NULL)
        return E_FAIL;
    int iValue;

	
	
	
	

	
	if (S_OK == SendMessage( MSG_2A_GET_RGB, sizeof(DWORD), (LPARAM)&iValue))
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
HRESULT CSWCameraControl2AMSG::OnSetShutter(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
	m_iManualShutter = Value;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_EXP, Value);

		
	
	
	
	return SendMessage( MSG_2A_SET_EXP, (WPARAM)&Value , sizeof(DWORD));
}

/**
* @brief MSG_SHUTTER 获取快门指令
* @param [in] wParam 保留
* @param [out] lParam（INT*类型） 快门值
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetShutter(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_EXP);

		
	
	
	
	
	return SendMessage( MSG_2A_GET_EXP, sizeof(DWORD), lParam);
}


/**
* @brief MSG_SET_DCAPERTUREENABLE,使能DC光圈
* @param [in] wParam 1为使能，0为非使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetDCApertureEnable(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_DCIRIS_AUTO_STATE, Value);

	
	
	
	

	
	return SendMessage( MSG_2A_SET_DCIRIS_AUTO_STATE, (WPARAM)&Value , sizeof(DWORD));
}

/**
* @brief MSG_GET_DCAPERTUREENABLE,获取自动DC光圈使能
* @param [in] wParam  保留
* @param [in] lParam 1为使能，0为非使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetDCApertureEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_DCIRIS_AUTO_STATE);
	return SendMessage( MSG_2A_GET_DCIRIS_AUTO_STATE, sizeof(DWORD), lParam );
}

/**
* @brief MSG_ZOOM_DCAPERTURE,放大DC光圈
* @param [in] wParam 保留
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnZoomDCAperture(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_ZOOM_DCIRIS);//未试成功
    INT Value = 1;//任意值

		
	
	
	
	
    return SendMessage( MSG_2A_ZOOM_DCIRIS, (WPARAM)&Value , sizeof(INT));
}

/**
* @brief MSG_SHRINK_DCAPERTURE,缩小DC光圈
* @param [in] wParam 保留
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnShrinkDCAperture(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SHRINK_DCIRIS);//为测试成功、
    INT Value = 1;//任意值

	
	
	
	

	
   	return SendMessage( MSG_2A_SHRINK_DCIRIS, (WPARAM)&Value , sizeof(INT));
}

/**
* @brief MSG_SET_FILTERSWITCH 滤光片切换
* @param [in] wParam 1正常滤光片，2红外滤光片
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetFilterSwitch(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
    m_FilterStatus = (2 == Value) ? 1 : 0;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_FILTER_STATE, m_FilterStatus);

	
	
	
	

	
	return SendMessage( MSG_2A_SET_FILTER_STATE, (WPARAM)&m_FilterStatus , sizeof(DWORD));
}
/**
* @brief MSG_GET_FILTERSWITCH 滤光片切换
* @param [in] wParam 保留
* @param [in] lParam 1正常滤光片，2红外滤光片
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetFilterSwitch(WPARAM wParam,LPARAM lParam)
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
HRESULT CSWCameraControl2AMSG::OnSetRedLightEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_SET_TRAFFIC_LIGTH_ENHANCE_STATE);
    DWORD Value = (DWORD)wParam;
	//todo已经不支持
	SW_TRACE_DEBUG("%s not support",__FUNCTION__);
	return E_FAIL;
	//return SendMessage( MSG_2A_SET_TRAFFIC_LIGTH_ENHANCE_STATE, (WPARAM)&Value , sizeof(DWORD));
    //return 0 == swpa_ipnc_control(0, CMD_SET_TRAFFIC_LIGTH_ENHANCE_STATE ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_REDLIGHT_ENABLE 获取红灯加红使能
* @param [in] wParam 保留
* @param [in] lParam 0为不使能，1为使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetRedLightEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_TRAFFIC_LIGTH_ENHANCE_STATE);
		//todo已经不支持
	SW_TRACE_DEBUG("%s not support",__FUNCTION__);
	return E_FAIL;
	//return SendMessage( MSG_2A_GET_TRAFFIC_LIGTH_ENHANCE_STATE, sizeof(DWORD), lParam );
    //return 0 == swpa_ipnc_control(0, CMD_GET_TRAFFIC_LIGTH_ENHANCE_STATE ,(void*)lParam , sizeof(DWORD*), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_SET_REDLIGHT_RECT 设置红灯加红区域
* @param [in] wParam SW_RECT*
* @param [in] lParam INT 红灯加红区域数。
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetRedLightRect(WPARAM wParam,LPARAM lParam)
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
    	//SW_TRACE_DEBUG("<setred>%d(%d,%d,%d,%d).", i,
    	//		rgcRect[i].left, rgcRect[i].top, rgcRect[i].right, rgcRect[i].bottom);
    	rgdwRedInfo[0] = 0; //0:红灯
		rgdwRedInfo[1] = i;
    	rgdwRedInfo[2] = rgcRect[i].left;
    	rgdwRedInfo[3] = rgcRect[i].top / 2;
    	rgdwRedInfo[4] = rgcRect[i].right;
    	rgdwRedInfo[5] = rgcRect[i].bottom / 2;
		SendMessage( MSG_2A_SET_TRAFFIC_LIGTH_ENHANCE_REGION, (WPARAM)rgdwRedInfo , sizeof(rgdwRedInfo));//mark
    }
    return S_OK;
}

/**
* @brief MSG_SET_TRAFFIC_LIGTH_LUM_TH 设置红灯加红
* @param [in] wParam
* @param [in] lParam 。
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetRedLightThreshold(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
    // todo.
    // 178此值有修改，在这强转
    Value = (Value == 0x2d00 ? 0x0B33 : 0x0FFF);
	Value = Value | (0x0 << 28); //28~31位为0：红灯
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_TRAFFIC_LIGTH_LUM_TH, Value);

	
	
	
	

	
	return SendMessage( MSG_2A_SET_TRAFFIC_LIGTH_LUM_TH, (WPARAM)&Value , sizeof(DWORD));
}



/**
* @brief MSG_SET_GREENLIGHT_RECT 设置绿灯加绿区域
* @param [in] wParam SW_RECT*
* @param [in] lParam INT 绿灯加红区域数。
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetGreenLightRect(WPARAM wParam,LPARAM lParam)
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
		SendMessage( MSG_2A_SET_TRAFFIC_LIGTH_ENHANCE_REGION, (WPARAM)rgdwGreenInfo , sizeof(rgdwGreenInfo));
    }
    return S_OK;
}

/**
* @brief MSG_SET_TRAFFIC_GREEN_LIGTH_LUM_TH 设置绿灯加绿
* @param [in] wParam
* @param [in] lParam 。
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetGreenLightThreshold(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
    // todo.
    // 178此值有修改，在这强转
    Value = (Value == 0x2d00 ? 0x0B32 : 0x0FFF); 
	Value = Value | (0x1 << 28); //28~31位为1: 绿灯
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_TRAFFIC_LIGTH_LUM_TH, Value);
	return SendMessage( MSG_2A_SET_TRAFFIC_LIGTH_LUM_TH, (WPARAM)&Value , sizeof(DWORD));
}




/**
* @brief MSG_SET_WDR_STRENGTH 设置WDR阈值
* @param [in] wParam 0为不使能，值越大强度越大。取值范围：0~4095
* @param [out] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetWDRStrength(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
    SW_TRACE_DEBUG("%s %cmd = %d value = %d\n",__FUNCTION__,CMD_SET_WDR_STRENGTH,Value);

	
	
	
	
	
	return SendMessage( MSG_2A_SET_WDR_STRENGTH, (WPARAM)&Value , sizeof(DWORD));
}

/**
* @brief MSG_GET_WDR_THRESHOLD 设置WDR阈值
* @param [in] wParam 保留
* @param [out] lParam 0为不使能，值越大强度越大。取值范围：0~4095
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetWDRStrength(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_WDR_STRENGTH);

	
	
	
	
	
	return SendMessage( MSG_2A_GET_WDR_STRENGTH, sizeof(DWORD), lParam );
}


/**
* @brief MSG_SET_TNF_ENABLE 设置TNF使能
* @param [in] wParam 0为不使能，1为使能
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetTNFEnable(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_TNF_STATE, Value);

	
	
	
		
	return SendMessage( MSG_2A_SET_TNF_STATE, (WPARAM)&Value, sizeof(DWORD));
}

/**
* @brief MSG_GET_TNF_ENABLE 获取TNF使能
* @param [in] wParam  保留
* @param [out] lParam 0为不使能，1为使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetTNFEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_TNF_STATE);

	
	
	
		
	return SendMessage( MSG_2A_GET_TNF_STATE, sizeof(DWORD), lParam );
}

/**
* @brief MSG_SET_SNF_ENABLE 设置SNF使能
* @param [in] wParam 0为不使能，1为使能
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetSNFEnable(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_SNF_STATE, Value);

	
	
	
		
	return SendMessage( MSG_2A_SET_SNF_STATE, (WPARAM)&Value, sizeof(DWORD));
}

/**
* @brief MSG_GET_SNF_ENABLE 获取SNF使能
* @param [in] wParam  保留
* @param [out] lParam 0为不使能，1为使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetSNFEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_SNF_STATE);

	
	
	
		
	return SendMessage( MSG_2A_GET_SNF_STATE, sizeof(DWORD), lParam );
}

/**
* @brief MSG_SET_DENOISE_MODE 设置降噪模式
* @param [in] wParam 0为不使能，1为使能
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetDeNoiseMode(WPARAM wParam,LPARAM lParam)
{
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

	SW_TRACE_DEBUG("%s cmd1 = %d value1 = %d, cmd1 = %d value1 = %d\n",
		__FUNCTION__,CMD_SET_TNF_STATE, dwTNFEnable,CMD_SET_SNF_STATE,dwSNFEnable);


	
	
	
	
	
    if (S_OK == SendMessage( MSG_2A_SET_TNF_STATE, (WPARAM)&dwTNFEnable, sizeof(DWORD))
		&& S_OK == SendMessage( MSG_2A_SET_SNF_STATE, (WPARAM)&dwSNFEnable, sizeof(DWORD)))
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
HRESULT CSWCameraControl2AMSG::OnGetDeNoiseMode(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d  cmd1 = %d cmd2 = %d\n",__FUNCTION__,__LINE__,CMD_GET_SNF_STATE,CMD_GET_TNF_STATE);
    if( NULL == lParam )
    {
        return E_FAIL;
    }

		
	
	
	

    DWORD dwSNFEnable = 0, dwTNFEnable=0;
    DWORD dwMode = 0;
    /*if ( 0 == swpa_ipnc_control(0, CMD_GET_SNF_STATE ,(void*)&dwSNFEnable , sizeof(DWORD), 0)
         && 0 == swpa_ipnc_control(0, CMD_GET_TNF_STATE ,(void*)&dwTNFEnable , sizeof(DWORD), 0)
         )*/
    if (S_OK == SendMessage( MSG_2A_GET_SNF_STATE, sizeof(DWORD), (LPARAM)&dwSNFEnable)
		&& S_OK == SendMessage( MSG_2A_GET_TNF_STATE, sizeof(DWORD), (LPARAM)&dwTNFEnable))
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
HRESULT CSWCameraControl2AMSG::OnSetDeNoiseState(WPARAM wParam,LPARAM lParam)
{
	DWORD Value = (DWORD)wParam;

    SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_TNF_SNF_STRENGTH, Value);
    
    return 0 == swpa_ipnc_control(0, CMD_SET_TNF_SNF_STRENGTH ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}

/**
* @brief MSG_GET_DENOISE_STATE 获取降噪状态
* @param [in] wParam  保留
* @param [out] lParam 0~3:自动、低、中、高
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetDeNoiseState(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_TNF_SNF_STRENGTH);
    return 0 == swpa_ipnc_control(0, CMD_GET_TNF_SNF_STRENGTH ,(void*)lParam , sizeof(DWORD), 0)?S_OK:E_FAIL;
}


/**
* @brief MSG_SET_AC_SYNC_ENABLE 设置电网同步使能
* @param [in] wParam 0：不使能，1：使能
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetACSyncEnable(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_AC_SYNC_STATE, Value);

	
	
	
	

	
	return SendMessage( MSG_2A_SET_AC_SYNC_STATE, (WPARAM)&Value , sizeof(BOOL));
}

/**
* @brief MSG_GET_AC_SYNC_ENABLE 获取电网同步使能
* @param [in] wParam  保留
* @param [out] lParam 0：不使能，1：使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetACSyncEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AC_SYNC_STATE);

		
	
	
	
	
	return SendMessage( MSG_2A_GET_AC_SYNC_STATE, sizeof(BOOL), lParam);
}

/**
* @brief MSG_SET_AC_SYNC_DELAY 设置电网同步延时
* @param [in] wParam 范围：0~25500
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetACSyncDelay(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_AC_SYNC_DELAY, Value);

		
	
	
	
	
	return SendMessage( MSG_2A_SET_AC_SYNC_DELAY, (WPARAM)&Value , sizeof(DWORD));
}
/**
* @brief MSG_GET_AC_SYNC_DELAY 获取电网同步延时
* @param [in] wParam  保留
* @param [out] lParam 范围：0~25500
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetACSyncDelay(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_AC_SYNC_DELAY);

	
	
	
	

	
	return SendMessage( MSG_2A_GET_AC_SYNC_DELAY, sizeof(DWORD), lParam );
}

/**
* @brief MSG_SET_GRAY_IMAGE_ENABLE 设置黑白图使能
* @param [in] wParam 0：不使能，1：使能
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetGrayImageEnable(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_GRAY_IMAGE_STATE, Value);

	
	
	
	
	
	return SendMessage( MSG_2A_SET_GRAY_IMAGE_STATE, (WPARAM)&Value , sizeof(BOOL));
}
/**
* @brief MSG_GET_GRAY_IMAGE_ENABLE 获取黑白图使能
* @param [in] wParam  保留
* @param [out] lParam 0：不使能，1：使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetGrayImageEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_GRAY_IMAGE_STATE);

	
	
	
		
	return SendMessage( MSG_2A_GET_GRAY_IMAGE_STATE, sizeof(BOOL), lParam);
}
/**
* @brief MSG_SET_CVBS_STD 设置CVBS制式
* @param [in] wParam 0：PAL，1：NTSC
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetCVBSStd(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
    SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_CVBS_STD, Value);
    if(0 == swpa_ipnc_control(0, CMD_SET_CVBS_STD ,(void*)&Value , sizeof(DWORD), 0))
    {
        //SW_TRACE_DEBUG("%s %d Value\n",__FUNCTION__,__LINE__,Value);
        return S_OK;
    }
    SW_TRACE_NORMAL("%s %d Value = %d failed\n",__FUNCTION__,__LINE__,Value);
    return E_FAIL;
}
/**
* @brief MSG_GET_CVBS_STD 读取CVBS制式
* @param [in] wParam  保留
* @param [out] lParam 0：PAL，1：NTSC
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetCVBSStd(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_CVBS_STD);
    return 0 == swpa_ipnc_control(0, CMD_GET_CVBS_STD ,(void*)lParam , sizeof(DWORD), 0)?S_OK:E_FAIL;
}
/**
* @brief MSG_SET_CAM_TEST 设置相机自动化测试
* @param [in] wParam 保留
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetCamTest(WPARAM wParam,LPARAM lParam)
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
	
    //SW_TRACE_DEBUG("%s %d end \n",__FUNCTION__,__LINE__);
    return S_OK;
}


/**
* @brief MSG_SET_GAMMA_STRENGTH 设置相机伽玛
* @param [in] wParam 范围：10～22
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetGammaStrength(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_GAMMA, Value);

		
	
	
	
	
	return SendMessage( MSG_2A_SET_GAMMA, (WPARAM)&Value , sizeof(DWORD));
}

/**
* @brief MSG_GET_GAMMA_STRENGTH 读取相机伽玛
* @param [in] wParam 保留
* @param [out] lParam 范围：10～22
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetGammaStrength(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_GAMMA);

		
	
	
	
	
	return SendMessage( MSG_2A_GET_GAMMA, sizeof(DWORD), lParam );
}

/**
* @brief MSG_SET_GAMMA_ENABLE 设置相机伽玛使能
* @param [in] wParam  保留
* @param [out] lParam 0：不使能，1：使能
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetGammaEnable(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_GAMMA_ENABLE, Value);

		
	
	
	
	return SendMessage( MSG_2A_SET_GAMMA_ENABLE, (WPARAM)&Value , sizeof(BOOL));
}

/**
* @brief MSG_GET_GAMMA_ENABLE 获取相机伽玛使能
* @param [in] wParam  0：不使能，1：使能
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetGammaEnable(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_GAMMA_ENABLE);

		
	
	
	
	return SendMessage( MSG_2A_GET_GAMMA_ENABLE, sizeof(BOOL), lParam);
}



/**
* @brief MSG_SET_CAM_FPGA_REG 设置相机FGPA写
* @param [in] wParam 范围不限
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetCamFpgaReg(WPARAM wParam,LPARAM lParam)
{
	DWORD* pValue = (DWORD *)wParam;
	/*SW_TRACE_DEBUG("%s cmd = %d value[0] = 0x%x value[1] = 0x%x\n",
		__FUNCTION__,CMD_SET_FPGA_REG, pValue[0], pValue[1]);*/
	return SendMessage( MSG_2A_SET_FPGA_REG, (WPARAM)pValue , sizeof(DWORD)*2);
}

/**
* @brief MSG_GET_GAM_FPGA_REG 设置相FGPA读
* @param [in] wParam 保留
* @param [out] lParam 范围不限
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetCamFpgaReg(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d  0x%x 0x%x cmd = %d\n",
		__FUNCTION__,__LINE__,((INT *)lParam)[0],((INT *)lParam)[1],CMD_GET_FPGA_REG);
	//todo:进程间通信只支持单条命令单向传送数据，因此类似于读取寄存器的命令需要先设置读取的地址，然后才读取
	if (S_OK == SendMessage(MSG_2A_SET_FPGA_READ_REG_ADDR, (WPARAM)&((PDWORD)lParam)[0], sizeof(DWORD))
		&& S_OK == SendMessage(MSG_2A_GET_FPGA_REG, sizeof(DWORD), (LPARAM)&((PDWORD)lParam)[1]))
	{
		return S_OK;
	}
	return E_FAIL;
	//return SendMessage( MSG_2A_GET_FPGA_REG, sizeof(DWORD)*2, lParam);
}

/**
* @brief MSG_SET_AGCPARAM 设置AGC参数
* @param [in] wParam  DWORD[4]
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetAGCParam(WPARAM wParam,LPARAM lParam)
{
    DWORD* pdwParam = (DWORD*)wParam;
    SW_TRACE_DEBUG("%s %d %d %d %d %d \n ",__FUNCTION__,__LINE__,pdwParam[0],pdwParam[1],pdwParam[2],pdwParam[3]);
    DWORD Shutter = pdwParam[0] | pdwParam[1] << 16;
    DWORD Gain =  pdwParam[2] | pdwParam[3] << 16;
	SendMessage(MSG_REALTIME_SET_MAXAGCSHUTTERGAIN,pdwParam[1],pdwParam[3]);
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
HRESULT CSWCameraControl2AMSG::OnGetAGCParam(WPARAM wParam,LPARAM lParam)
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
HRESULT CSWCameraControl2AMSG::OnSetEdgeEnhance(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_EDGE_ENHANCE, Value);

	
	
	
	

	
	return SendMessage( MSG_2A_SET_EDGE_ENHANCE, (WPARAM)&Value , sizeof(DWORD));
}

/**
* @brief MSG_GET_EDGE_ENHANCE 图像边缘增强
* @param [in] wParam 保留
* @param [out] lParam BOOL*
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetEdgeEnhance(WPARAM wParam,LPARAM lParam)
{
    SW_TRACE_DEBUG("%s %d cmd = %d\n",__FUNCTION__,__LINE__,CMD_GET_EDGE_ENHANCE);

		
	
	
	
	return SendMessage( MSG_2A_GET_EDGE_ENHANCE, sizeof(DWORD), lParam );
}

/**
* @brief MSG_GET_MANUAL_VALUE 获取手动RGB 快门增益
* @param [in] wParam 保留
* @param [out] lParam
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT  CSWCameraControl2AMSG::OnGetManualParamValue(WPARAM wParam,LPARAM lParam)
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
HRESULT CSWCameraControl2AMSG::OnGetCameraWorkState(WPARAM wParam,LPARAM lParam)
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
HRESULT CSWCameraControl2AMSG::OnSetLEDSwitch(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);

	DWORD dwSwitch = (DWORD)wParam;
	
	if(rgWriteRegParam[0]==0x927)
		OnSetCamFpgaReg((WPARAM)rgWriteRegParam, 0);
			
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_NORMAL_LED;
	if (S_OK != SendMessage( MSG_2A_GET_IO_ARG, sizeof(tIOArg), (LPARAM)&tIOArg))
	//if (0 != swpa_ipnc_control(0, CMD_GET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0))
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
	return SendMessage( MSG_2A_SET_IO_ARG, (WPARAM)&tIOArg , sizeof(tIOArg));

}


/**
* @brief MSG_GET_LED_SWITCH 获取补光灯状态
* @param [in] wParam 保留
* @param [out] lParam
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetLEDSwitch(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	if(lParam == NULL)
		 return E_FAIL;

		
	
	
	

	DWORD* pdwParam = (DWORD*)lParam;
	
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_NORMAL_LED;
	if (S_OK != SendMessage( MSG_2A_GET_IO_ARG, sizeof(tIOArg), (LPARAM)&tIOArg))
	//if(0 != swpa_ipnc_control(0, CMD_GET_IO_ARG ,(void*)&tIOArg , sizeof(tIOArg), 0))
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
HRESULT CSWCameraControl2AMSG::OnSetTriggerOutState(WPARAM wParam,LPARAM lParam)
{
	DWORD dwState = (DWORD)wParam;
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_TRIGGEROUT;
	tIOArg.polarity = (0 == dwState) ? m_dwTriggerOutPolarity : (!m_dwTriggerOutPolarity);
	tIOArg.output_type = m_dwTriggerOutOutputType;

	//SW_TRACE_DEBUG("Set Trigger out state: %d,polarity %d, outputtype %d m_dwTriggerOutPolarity=%d\n",
	//		dwState,tIOArg.polarity,tIOArg.output_type,m_dwTriggerOutPolarity);
	return SendMessage( MSG_2A_SET_IO_ARG, (WPARAM)&tIOArg , sizeof(tIOArg));
}



/**
* @brief MSG_SET_TRIGGEROUT 设置触发输出
* @param [in] polarity output_type 
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetTriggerOut(WPARAM wParam,LPARAM lParam)
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
	 //todo huanggr
	 rgWriteRegParam[1]&=~0xFF;
	 rgWriteRegParam[1]|=0xFF;
	tIOArg.pulse_width =(rgWriteRegParam[0]==0x927?(rgWriteRegParam[1]):m_dwLEDPluseWidth);
	
	
	
	

	
	if (S_OK != SendMessage( MSG_2A_SET_IO_ARG, (WPARAM)&tIOArg , sizeof(tIOArg)))
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
HRESULT CSWCameraControl2AMSG::OnGetTriggerOut(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//polarity output_type

	if(lParam == NULL)
		return E_FAIL;

	DWORD* pdwParam = (DWORD*)lParam;
	
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_TRIGGEROUT;

	
	
	
	

	
	if (S_OK != SendMessage( MSG_2A_GET_IO_ARG, sizeof(tIOArg), (LPARAM)&tIOArg))
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

HRESULT CSWCameraControl2AMSG::OnSetF1TriggerOut(WPARAM wParam,LPARAM lParam)
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
	if (S_OK != SendMessage( MSG_2A_SET_IO_ARG, (WPARAM)&tIOArg , sizeof(tIOArg)))
	{
		SW_TRACE_NORMAL("OnSetTriggerOut CMD_SET_IO_ARG Failed");
		return E_FAIL;
	}
	m_dwTriggerOutPolarity 		= tIOArg.polarity;
	m_dwTriggerOutOutputType 	= tIOArg.output_type;
	
	return S_OK;
}


HRESULT CSWCameraControl2AMSG::OnGetF1TriggerOut(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//polarity output_type

	if(lParam == NULL)
		return E_FAIL;

	DWORD* pdwParam = (DWORD*)lParam;

		
	
	
	
	
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_TRIGGEROUT;
	if (S_OK != SendMessage( MSG_2A_GET_IO_ARG, sizeof(tIOArg), (LPARAM)&tIOArg))
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

HRESULT CSWCameraControl2AMSG::OnSetAlarmTriggerOut(WPARAM wParam,LPARAM lParam)
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
		
	rgWriteRegParam[0]=0x927;
	rgWriteRegParam[1]=0;
	rgWriteRegParam[1] = ((pParam[0]<<11)|(pParam[1]<<10)|(pParam[2]<<9)|(pParam[3]<<8)|pParam[4]);		

	if (S_OK != SendMessage( MSG_2A_SET_IO_ARG, (WPARAM)&tIOArg , sizeof(tIOArg)))
	{
		SW_TRACE_NORMAL("OnSetTriggerOut CMD_SET_IO_ARG Failed");
		return E_FAIL;
	}
	m_dwTriggerOutPolarity 		= tIOArg.polarity;
	m_dwTriggerOutOutputType 	= tIOArg.output_type;
	
	return S_OK;
}


HRESULT CSWCameraControl2AMSG::OnGetAlarmTriggerOut(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//polarity output_type

	if(lParam == NULL)
		return E_FAIL;

	DWORD* pdwParam = (DWORD*)lParam;
	
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_ALARM_OUT;

		
	
	
	

	if (S_OK != SendMessage( MSG_2A_GET_IO_ARG, sizeof(tIOArg), (LPARAM)&tIOArg))
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
HRESULT CSWCameraControl2AMSG::OnSetEXPIO(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//polarity output_type
	DWORD *pParam = (DWORD *)wParam;
	DWORD dwPolarity = pParam[0];
	DWORD dwOutputType = pParam[1];

	rgWriteRegParam[1]&=~((1<<8)|(1<<9));
	rgWriteRegParam[1]|=((dwPolarity<<8)|(dwOutputType<<9));

	if(rgWriteRegParam[0]==0x927)
		OnSetCamFpgaReg((WPARAM)rgWriteRegParam, 0);
	
	/*tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_NORMAL_LED;

		
	
	
	
	
	if (S_OK != SendMessage( MSG_2A_GET_IO_ARG, sizeof(tIOArg), (LPARAM)&tIOArg))
    {
        SW_TRACE_NORMAL("OnSetEXPIO CMD_GET_IO_ARG Failed!");
        return E_FAIL ;
    }

	tIOArg.polarity = dwPolarity;
	tIOArg.output_type = dwOutputType;
	//tIOArg.pulse_width = m_dwLEDPluseWidth;
	if (S_OK != SendMessage( MSG_2A_SET_IO_ARG, (WPARAM)&tIOArg , sizeof(tIOArg)))
	{
		SW_TRACE_NORMAL("OnSetEXPIO CMD_SET_IO_ARG Failed!");
		return E_FAIL ;
	}
	
	m_dwLEDPolarity = dwPolarity;		//缓存其配置参数
	m_dwLEDOutputType = dwOutputType;*/
	
	return S_OK;
}


/**
* @brief MSG_GET_EXP_IO 读取补光灯控制口状态
* @param [in] 保留 
* @param [out] polarity output_type
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetEXPIO(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//polarity output_type

	if(lParam == NULL)
		return E_FAIL;

	DWORD* pdwParam = (DWORD*)lParam;
	
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_NORMAL_LED;

		
	
	
	
	
	if (S_OK != SendMessage( MSG_2A_GET_IO_ARG, sizeof(tIOArg), (LPARAM)&tIOArg))
	{
		SW_TRACE_NORMAL("OnGetEXPIO CMD_GET_IO_ARG Failed\n");
		return E_FAIL;
	}
	
	pdwParam[0] = tIOArg.polarity;
	pdwParam[1] = tIOArg.output_type;
	//SW_TRACE_DEBUG("Get EXP IO state polarity %d outputtype %d\n",
	//	tIOArg.polarity,tIOArg.output_type);
	
	return S_OK;
}

/**
* @brief MSG_SET_EXP_PLUSEWIDTH 设置补光灯脉宽
* @param [in] pluse width
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetEXPPluseWidth(WPARAM wParam,LPARAM lParam)
{
    DWORD dwValue = (DWORD)wParam;

	rgWriteRegParam[1]&=~0xFF;
	rgWriteRegParam[1]|=dwValue;

	if(rgWriteRegParam[0]==0x927)
		OnSetCamFpgaReg((WPARAM)rgWriteRegParam, 0);

    /*tFPGA_IO_ARG tIOArg;
    tIOArg.type = E_NORMAL_LED;
		
	if (S_OK != SendMessage( MSG_2A_GET_IO_ARG, sizeof(tIOArg), (LPARAM)&tIOArg))
    {
        SW_TRACE_NORMAL("OnSetEXPPluseWidth CMD_GET_IO_ARG Failed!");
        return E_FAIL ;
    }

    tIOArg.pulse_width = dwValue;
	if (S_OK != SendMessage( MSG_2A_SET_IO_ARG, (WPARAM)&tIOArg , sizeof(tIOArg)))
    {
        SW_TRACE_NORMAL("OnSetEXPPluseWidth CMD_SET_IO_ARG Failed!");
        return E_FAIL ;
    }

    m_dwLEDPluseWidth = dwValue;*/

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
HRESULT CSWCameraControl2AMSG::OnSetALMIO(WPARAM wParam,LPARAM lParam)
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

		
	
	
	
	
	return SendMessage( MSG_2A_SET_IO_ARG, (WPARAM)&tIOArg , sizeof(tIOArg));
}


/**
* @brief MSG_GET_ALARM_IO 读取告警控制口状态
* @param [in] 保留
* @param [out] polarity output_type 
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetALMIO(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//polarity output_type
	if(lParam == NULL)
		return E_FAIL;

	DWORD* pdwParam = (DWORD*)lParam;
	
	tFPGA_IO_ARG tIOArg;
	tIOArg.type = E_ALARM_OUT;

		
	
	
	
	
	if (S_OK != SendMessage( MSG_2A_GET_IO_ARG, sizeof(tIOArg), (LPARAM)&tIOArg))
	{
		SW_TRACE_NORMAL("OnGetALMIO CMD_GET_IO_ARG Failed\n");
		return E_FAIL;
	}
	
	pdwParam[0] = tIOArg.polarity;
	pdwParam[1] = tIOArg.output_type;
	//SW_TRACE_DEBUG("Get ALM IO state polarity %d outputtype %d\n",
	//	tIOArg.polarity,tIOArg.output_type);
	
	return S_OK;
}


/**
* @brief MSG_SET_TG_IO 设置触发抓拍口触发类型
* @param [in] 触发沿
* @param [out] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetTGIO(WPARAM wParam,LPARAM lParam)
{
	//SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//EgdeType
	DWORD dwEgdeType = (DWORD)wParam;
	SW_TRACE_DEBUG("OnSetTGIO EgdeType %d\n",dwEgdeType);

	
	
	
	

	
	return SendMessage( MSG_2A_SET_EDGE_MOD, (WPARAM)&dwEgdeType , sizeof(DWORD));
}


/**
* @brief MSG_GET_TG_IO 读取触发抓拍口触发类型
* @param [in] 保留
* @param [out] 触发沿
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetTGIO(WPARAM wParam,LPARAM lParam)
{
	SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);

		
	
	
	

	return SendMessage( MSG_2A_GET_EDGE_MOD, sizeof(DWORD), lParam);
}

HRESULT CSWCameraControl2AMSG::OnSetColorGradation(WPARAM wParam,LPARAM lParam)
{
	//SW_TRACE_DEBUG("%s %d \n",__FUNCTION__,__LINE__);
	//EgdeType
	DWORD dwColor = (DWORD)wParam;
	SW_TRACE_DEBUG("OnSetColorGradation value: %d\n",dwColor);

	DWORD rgdwParam[2];
	rgdwParam[0] = 0xa30;
	rgdwParam[1] = dwColor;
	return this->OnSetCamFpgaReg((WPARAM)rgdwParam,0);
}

HRESULT CSWCameraControl2AMSG::OnSetCaptureEnable(WPARAM wParam, LPARAM lParam)
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
HRESULT CSWCameraControl2AMSG::OnSetWorkMode(WPARAM wParam,LPARAM lParam)
{
	DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_WORKMODE, Value);

		
	
	
	
	
	return SendMessage( MSG_2A_SET_WORKMODE, (WPARAM)&Value , sizeof(DWORD));
}

/**
* @brief MSG_SET_AWB_MODE 设置白平衡模式
* @param [in] 工作模式
* @param [int] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetAWBMode(WPARAM wParam,LPARAM lParam)
{
	DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_AWB_MODE, Value);

	
	
	
	

	
	return SendMessage( MSG_2A_SET_AWB_MODE, (WPARAM)&Value, sizeof(DWORD));
}

HRESULT CSWCameraControl2AMSG::OnSetCaptureSofttouch(WPARAM wParam,LPARAM lParam)
{
	//static DWORD s_dwLastInfo = 0;
	const DWORD iLaneInfSoftReg = 0x92f;
	DWORD crgWriteRegParam[2], dwReg[2];
	INT iAGCEnable = 0;
	INT iSet = 0;
	INT iInfo = 0;
	iSet = ( wParam & 0xFF000000 ) >> 24;
	iInfo =( wParam & 0x00FFFFFF);

    //iSet = (iSet == 0) ? 1 : 2;

	//SW_TRACE_DEBUG("<CameralControlMSG>set:%d, info:%d.\n", iSet, iInfo);
	//if((s_dwLastInfo + 1) != iInfo)
	//	SW_TRACE_DEBUG("**********ERROR:set:%d, info:%d.\n", iSet, iInfo);
	//s_dwLastInfo = iInfo;

	//自动设置抓拍参数
	/*if(m_fCaptureAutoParamEnable)
	{
		CSWMessage::SendMessage(MSG_AUTO_CONTROL_SET_AUTO_CAPTURE_PARAM, NULL, NULL);
	}*/

	crgWriteRegParam[0] = iLaneInfSoftReg;
	if(!m_IsDay)
	{			
		if(iInfo==0)
		{
			iSet=3;
		}		
		//设置道路亮灯使能		
		crgWriteRegParam[1] = iSet;
	}
	else
	{		
		crgWriteRegParam[1] = 0;
	}
	OnSetCamFpgaReg((WPARAM)crgWriteRegParam, 0);
	
	//SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SOFT_CAPTURE, iInfo);

	return SendMessage( MSG_2A_SOFT_CAPTURE, (WPARAM)&iInfo , sizeof(INT));
}

HRESULT CSWCameraControl2AMSG::OnSetDspFlag(WPARAM wParam,LPARAM lParam)
{
	INT iStatus=(INT)wParam;
	if(iStatus==0)
		m_IsDay=TRUE;
	else
		m_IsDay=FALSE;
	return S_OK;
}

/**
* @brief MSG_SET_F1_OUTPUT_TYPE 设置F1输出模式
* @param [in] F1输出模式
* @param [int] 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetF1OutputType(WPARAM wParam,LPARAM lParam)
{
	INT Value = (INT)wParam ? 1 : 0;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_FLASH_GATE, Value);

		
	
	
	
	
	return SendMessage( MSG_2A_SET_FLASH_GATE, (WPARAM)&Value , sizeof(INT));
}

/**
* @brief MSG_CAPTURERGB_ENABLE 抓拍RGB使能
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetCaptureRGBEnable(WPARAM wParam,LPARAM lParam)
{

    /*int fEnable = (int)wParam;
    int rgnValue[3] = {0};
    rgnValue[0] = fEnable ? m_nCaptureR : -1;
    rgnValue[1] = fEnable ? m_nCaptureG : -1;
    rgnValue[2] = fEnable ? m_nCaptureB : -1;
    OnSetCaptureRGBGain((WPARAM)rgnValue, lParam);*/

	DWORD rgFpgaParam[2] = {0};
	rgFpgaParam[0] = 0x914;     // 使能抓拍快门
	int fEnable = (int)wParam;
	if(fEnable==0)
		rgFpgaParam[1] = 0x03;
	else
    	rgFpgaParam[1] = 0x0b;
    OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);

	if(fEnable!=0)
	{
	    rgFpgaParam[0] = 0xb0e;		// R 抓拍
	    rgFpgaParam[1] = 0x4000;
	    OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);

	    rgFpgaParam[0] = 0xb0f;		// G 抓拍
	    rgFpgaParam[1] = 0x7200;
	    OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);

	    rgFpgaParam[0] = 0xb10;		// G 抓拍
	    rgFpgaParam[1] = 0x7200;
	    OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);

	    rgFpgaParam[0] = 0xb11;		// B 抓拍
	    rgFpgaParam[1] = 0xa500;
	    OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);
	}
    
    return S_OK;
}

/**
* @brief MSG_CAPTURESHUTTER_ENABLE 抓拍快门使能
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetCaptureShutterEnable(WPARAM wParam,LPARAM lParam)
{
/*
    int fEnable = (int)wParam;
    int nValue = fEnable ? m_nCaptureShutter : -1;
    OnSetCaptureShutter((WPARAM)&nValue, lParam);
    */
    return S_OK;
}

/**
* @brief MSG_CAPTURESHUTTER_ENABLE 抓拍增益使能
* @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetCaptureGainEnable(WPARAM wParam,LPARAM lParam)
{
/*
    int fEnable = (int)wParam;
    int nValue = fEnable ? m_nCaptureGain : -1;
    OnSetCaptureGain((WPARAM)&nValue, lParam);
    */
    return S_OK;
}

/**
 * @brief MSG_CAPTURESHARPEN_ENABLE 抓拍锐化使能
 * @param [in] wParam 使能消息，使能标记，0=不使能，1 = 使能
 * @param [in] lParam 保留
 * @return 成功返回S_OK，其他值代表失败
 */
HRESULT CSWCameraControl2AMSG::OnSetCaptureSharpenEnable(WPARAM wParam,LPARAM lParam)
{
/*
    int fEnable = (int)wParam;
    int nValue = fEnable ? m_nCapSharpen : -1;
    OnSetCaptureSharpen((WPARAM)&nValue, lParam);
    */
    return S_OK;
}

/**
* @brief 消息接收函数
* @param [in] wParam 快门值
* @param [in] lParam 保留
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnSetCaptureShutter(WPARAM wParam,LPARAM lParam)
{
    DWORD* Value = (DWORD*)wParam;
    SW_TRACE_DEBUG("Set Capture Shutter:%d",*Value);

		
	
	
	

    return SendMessage( MSG_2A_SET_CAP_SHUTTER, (WPARAM)Value , sizeof(DWORD));
}

/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [out] lParam(INT*类型) 快门值
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetCaptureShutter(WPARAM wParam,LPARAM lParam)
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
HRESULT CSWCameraControl2AMSG::OnSetCaptureGain(WPARAM wParam,LPARAM lParam)
{
    DWORD* Value = (DWORD*)wParam;
    SW_TRACE_DEBUG("Set Capture Gain:%d",*Value);

		
	
	
	

    return SendMessage( MSG_2A_SET_CAP_GAIN, (WPARAM)Value , sizeof(DWORD));
}

/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [out] lParam(INT* 类型) 增益值
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetCaptureGain(WPARAM wParam,LPARAM lParam)
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
HRESULT CSWCameraControl2AMSG::OnSetCaptureRGBGain(WPARAM wParam,LPARAM lParam)
{
    int *prgnValue = (int*)wParam;
	
    int nEnable = ((INT *)wParam)[3];
    int nValue = 0;

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
	DWORD Value = prgnValue[0]  | prgnValue[1] << 8 | prgnValue[2] << 16;

		
	
	
	
	
    return SendMessage(MSG_2A_SET_CAP_RGB,(WPARAM)&Value,sizeof(DWORD));
}

/**
* @brief 消息接收函数
* @param [in] wParam 保留
* @param [out] lParam(INT数组类型)0:R,1:G,2:B
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetCaptureRGBGain(WPARAM wParam,LPARAM lParam)
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
HRESULT CSWCameraControl2AMSG::OnSetCaptureSharpen(WPARAM wParam,LPARAM lParam)
{
	DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("Set Capture Sharpen:%d",Value);

		
	
	
	

	return SendMessage(MSG_2A_SET_CAP_SHARPEN, (WPARAM)&Value , sizeof(DWORD));
}

/**
* @brief MSG_GET_CAPTURE_SHARPEN_THR 获取抓拍锐度指令
* @param [in] wParam 保留
* @param [out] lParam 锐度值
* @return 成功返回S_OK，其他值代表失败
*/
HRESULT CSWCameraControl2AMSG::OnGetCaptureSharpen(WPARAM wParam,LPARAM lParam)
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
HRESULT CSWCameraControl2AMSG::OnGetM3DayNightStatus(WPARAM wParam,LPARAM lParam)
{
	return SendMessage( MSG_2A_GET_DAY_NIGHT, sizeof(DWORD), lParam);
}

HRESULT CSWCameraControl2AMSG::OnSetWDRLevel(WPARAM wParam,LPARAM lParam)
{
    int Value = (int)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_BACKLIGHT, Value);

	
	
	
	

	
	return SendMessage( MSG_2A_SET_BACKLIGHT, (WPARAM)&Value , sizeof(INT));
}

HRESULT CSWCameraControl2AMSG::OnSetTNFState(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_TNF_STATE, Value);
    return 0 == swpa_ipnc_control(0, CMD_SET_TNF_STATE ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}
HRESULT CSWCameraControl2AMSG::OnGetTNFState(WPARAM wParam,LPARAM lParam)
{
	//todo
	return E_FAIL;
}
HRESULT CSWCameraControl2AMSG::OnSetSNFState(WPARAM wParam,LPARAM lParam)
{
    DWORD Value = (DWORD)wParam;
	SW_TRACE_DEBUG("%s cmd = %d value = %d\n",__FUNCTION__,CMD_SET_SNF_STATE, Value);
    return 0 == swpa_ipnc_control(0, CMD_SET_SNF_STATE ,(void*)&Value , sizeof(DWORD), 0)?S_OK:E_FAIL;
}
HRESULT CSWCameraControl2AMSG::OnGetSNFState(WPARAM wParam,LPARAM lParam)
{
	//todo
	return E_FAIL;
}


HRESULT CSWCameraControl2AMSG::OnSetCaptureAutoParamEnable(WPARAM wParam,LPARAM lParam)
{
	m_fCaptureAutoParamEnable = ((INT)wParam == 1) ? TRUE : FALSE;

	//CSWMessage::SendMessage(MSG_AUTO_CONTROL_SET_AUTO_CAPTURE_PARAM, wParam, NULL);

	//SW_TRACE_DEBUG("OnSetCaptureAutoParamEnable value:%d \n",m_fCaptureAutoParamEnable);

	//send remote msg
	return S_OK;
}

HRESULT CSWCameraControl2AMSG::OnGetCaptureAutoParamEnable(WPARAM wParam,LPARAM lParam)
{
	INT *pEnable = (INT *)lParam;
	*pEnable = (m_fCaptureAutoParamEnable ? 1 : 0);

	//send remote msg
	return S_OK;
}

HRESULT CSWCameraControl2AMSG::OnGetPTZInfo(PVOID pvBuffer, INT iSize)
{
    SW_TRACE_DEBUG("Info: got GetPTZInfo msg...\n");

    //send remote msg to notify the ptz info
    PTZ_INFO sInfo;

	swpa_memset(&sInfo, 0, sizeof(sInfo));

	/* not support any PTZ feature */

		
	
	
	
        
    return SendRemoteMessage(MSG_APP_REMOTE_SET_PTZ_INFO, (PVOID)&sInfo, sizeof(sInfo));
}


BOOL CSWCameraControl2AMSG::OnIncreaseBrightness()
{
	DWORD rgFpgaParam[2] = {0};
	//以下为提亮图像设置， 不开放寄存器意义  __WRITE_FPGA_START__
	rgFpgaParam[0] = 0xb81;		//gamma 提亮
    rgFpgaParam[1] = 0x00;
	OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);


	rgFpgaParam[0] = 0xb83;		//数字增益提亮
    rgFpgaParam[1] = 0x02;
	OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);

	rgFpgaParam[0] = 0xa3c;
	rgFpgaParam[1] = 0xd0;
	OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);

	rgFpgaParam[0] = 0xa84;
	rgFpgaParam[1] = 0x03;
	OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);

    rgFpgaParam[0] = 0x914;     // 使能抓拍快门
    rgFpgaParam[1] = 0x0b;
    OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);

    rgFpgaParam[0] = 0xb0e;		// R 抓拍
    rgFpgaParam[1] = 0x4000;
    OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);

    rgFpgaParam[0] = 0xb0f;		// G 抓拍
    rgFpgaParam[1] = 0x7200;
    OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);

    rgFpgaParam[0] = 0xb10;		// G 抓拍
    rgFpgaParam[1] = 0x7200;
    OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);

    rgFpgaParam[0] = 0xb11;		// B 抓拍
    rgFpgaParam[1] = 0xa500;
    OnSetCamFpgaReg((WPARAM)rgFpgaParam, 0);
	// __ WRITE_FPGA_END__

	return TRUE;
}
