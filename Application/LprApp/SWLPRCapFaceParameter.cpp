#include "SWFC.h"
#include "SWLPRCapFaceParameter.h"

CSWLPRCapFaceParameter::CSWLPRCapFaceParameter()
{
	Get().nWorkModeIndex = PRM_CAP_FACE;
	m_strWorkMode.Format("卡口");


	//先初始化扫描区域，每种模式都有自己的扫描区域
	Get().cTrackerCfgParam.cDetectArea.DetectorAreaLeft = 0;
	Get().cTrackerCfgParam.cDetectArea.DetectorAreaTop = 15;
	Get().cTrackerCfgParam.cDetectArea.DetectorAreaRight = 100;
	Get().cTrackerCfgParam.cDetectArea.DetectorAreaBottom = 55;
	Get().cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum = 5;
	Get().cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum = 9;

	//初始化梯形区域
	Get().cTrackerCfgParam.cTrapArea.TopLeftX = Get().cTrackerCfgParam.cDetectArea.DetectorAreaLeft;
	Get().cTrackerCfgParam.cTrapArea.TopRightX = Get().cTrackerCfgParam.cDetectArea.DetectorAreaRight;
	Get().cTrackerCfgParam.cTrapArea.BottomLeftX = Get().cTrackerCfgParam.cDetectArea.DetectorAreaLeft;
	Get().cTrackerCfgParam.cTrapArea.BottomRightX = Get().cTrackerCfgParam.cDetectArea.DetectorAreaRight;

	Get().cTrackerCfgParam.cTrapArea.TopLeftY = Get().cTrackerCfgParam.cDetectArea.DetectorAreaTop;
	Get().cTrackerCfgParam.cTrapArea.TopRightY = Get().cTrackerCfgParam.cDetectArea.DetectorAreaTop;
	Get().cTrackerCfgParam.cTrapArea.BottomLeftY = Get().cTrackerCfgParam.cDetectArea.DetectorAreaBottom;
	Get().cTrackerCfgParam.cTrapArea.BottomRightY = Get().cTrackerCfgParam.cDetectArea.DetectorAreaBottom;

	Get().cTrackerCfgParam.cTrapArea.nDetectorMinScaleNum = Get().cTrackerCfgParam.cDetectArea.nDetectorMinScaleNum;
	Get().cTrackerCfgParam.cTrapArea.nDetectorMaxScaleNum = Get().cTrackerCfgParam.cDetectArea.nDetectorMaxScaleNum;

	Get().cCamCfgParam.iMaxExposureTime = 4200;
	Get().cCamCfgParam.iMinExposureTime = 0;
	
	//卡口默认的视频测速修正系数
	Get().cTrackerCfgParam.cScaleSpeed.fltAdjustCoef = 0.96;
}

CSWLPRCapFaceParameter::~CSWLPRCapFaceParameter()
{
}

HRESULT CSWLPRCapFaceParameter::InitTracker(VOID)
{
	if(S_OK == CSWLPRParameter::InitTracker())
	{
    GetEnum("\\Tracker\\VideoDet"
    	, "ShadowDetMode"
    	, &Get().cTrackerCfgParam.cVideoDet.nShadowDetMode
    	, Get().cTrackerCfgParam.cVideoDet.nShadowDetMode
    	, "0:不使用阴影检测;1:使用阴影检测"
    	, "阴影检测模式"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\Tracker\\VideoDet"
    	, "ShadowDetEffect"
    	, &Get().cTrackerCfgParam.cVideoDet.nShadowDetEffect
    	, Get().cTrackerCfgParam.cVideoDet.nShadowDetEffect
    	, 0
    	, 100
    	, "阴影检测强弱程度值"
    	, "0:使用缺省值(不精确), 1:阴影检测强弱程度最小, 100:阴影检测强弱程度最大"
    	, CUSTOM_LEVEL
    );
    
    GetEnum("\\Tracker\\VideoDet"
    	, "DiffType"
    	, &Get().cTrackerCfgParam.cVideoDet.nDiffType
    	, Get().cTrackerCfgParam.cVideoDet.nDiffType
    	, "0:帧差提取;1:帧差后sobel前景"
    	, "前景提取方式"
    	, "值为0:帧差提取 值为1:帧差后sobel前景，针对高速路卡口两车道车身多检"
    	, CUSTOM_LEVEL
    );
    
    GetEnum("\\Tracker\\VideoDet"
    	, "MedFilter"
    	, &Get().cTrackerCfgParam.cVideoDet.nMedFilter
    	, Get().cTrackerCfgParam.cVideoDet.nMedFilter
    	, "0:关闭;1:打开"
    	, "抑制抖动开关"
    	, ""
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\Tracker\\VideoDet"
    	, "MedFilterUpLine"
    	, &Get().cTrackerCfgParam.cVideoDet.nMedFilterUpLine
    	, Get().cTrackerCfgParam.cVideoDet.nMedFilterUpLine
    	, 0
    	, 100
    	, "抑制抖动作用区域上区域"
    	, "值为0:屏幕最上方 值为100:屏幕最下方"
    	, CUSTOM_LEVEL
    );
    
    GetInt("\\Tracker\\VideoDet"
    	, "MedFilterDownLine"
    	, &Get().cTrackerCfgParam.cVideoDet.nMedFilterDownLine
    	, Get().cTrackerCfgParam.cVideoDet.nMedFilterDownLine
    	, 0
    	, 100
    	, "抑制抖动作用区域下区域"
    	, "值为0:屏幕最上方 值为100:屏幕最下方"
    	, CUSTOM_LEVEL
    );
    
   	GetEnum("\\Tracker\\Recognition"
   		, "EnableRecogCarColor"
   		, &Get().cTrackerCfgParam.fEnableRecgCarColor
   		, Get().cTrackerCfgParam.fEnableRecgCarColor
   		, "0:关;1:开"
   		, "车身颜色识别开关"
   		, ""
   		, CUSTOM_LEVEL
    );
    
    GetEnum("\\Tracker\\Recognition"
    	, "RecogFace"
    	, &Get().cTrackerCfgParam.fEnableDetFace
    	, Get().cTrackerCfgParam.fEnableDetFace
    	, "0:关;1:开"
    	, "人脸检测开关"
    	, ""
    	, CUSTOM_LEVEL
    );
    //当前只要开了人脸识别，就必须打开车身颜色识别,TODO
    if(Get().cTrackerCfgParam.fEnableDetFace)
    {
    	Get().cTrackerCfgParam.fEnableRecgCarColor = TRUE;
    }
    
    GetInt("\\Tracker\\Recognition"
    	, "FaceMin"
    	, &Get().cTrackerCfgParam.nMinFaceScale
    	, Get().cTrackerCfgParam.nMinFaceScale
    	, 0
    	, 20
    	, "最小人脸宽度(按图片宽度百分比计算)"
    	, ""
    	, CUSTOM_LEVEL
    );
    GetInt("\\Tracker\\Recognition"
    	, "FaceMax"
    	, &Get().cTrackerCfgParam.nMaxFaceScale
    	, Get().cTrackerCfgParam.nMaxFaceScale
    	, 0
    	, 20
    	, "最大人脸宽度(按图片宽度百分比计算)"
    	, ""
    	, CUSTOM_LEVEL
    );
    if (Get().cTrackerCfgParam.nMinFaceScale > Get().cTrackerCfgParam.nMaxFaceScale)
    {
        Get().cTrackerCfgParam.nMinFaceScale = 2;
        Get().cTrackerCfgParam.nMaxFaceScale = 20;
        
        UpdateInt("\\Tracker\\Recognition"
        	, "FaceMin"
        	, Get().cTrackerCfgParam.nMinFaceScale
        );  
        
        UpdateInt("\\Tracker\\Recognition"
        	, "FaceMax"
        	, Get().cTrackerCfgParam.nMinFaceScale
        );
    }

	//车身颜色识别控制	
	GetInt("\\Tracker\\ProcessPlate"
        , "EnableColorReRecog"
        , &Get().cTrackerCfgParam.nEnableColorReRecog
        , Get().cTrackerCfgParam.nEnableColorReRecog
        , 0
        , 7
        , "黑白灰重识别"
        , ""
        , CUSTOM_LEVEL
        );

	GetInt("\\Tracker\\ProcessPlate"
        , "nBColorReThr"
        , &Get().cTrackerCfgParam.nBColorReThr
        , Get().cTrackerCfgParam.nBColorReThr
        , -60
        , 60
        , "黑重识别阈值"
        , ""
        , PROJECT_LEVEL
        );

	GetInt("\\Tracker\\ProcessPlate"
        , "nWColorReThr"
        , &Get().cTrackerCfgParam.nWColorReThr
        , Get().cTrackerCfgParam.nWColorReThr
        , 0
        , 240
        , "白重识别阈值"
        , ""
        , PROJECT_LEVEL
        );

	GetInt("\\Tracker\\ProcessPlate"
        , "nWGColorReThr"
        , &Get().cTrackerCfgParam.nWGColorReThr
        , Get().cTrackerCfgParam.nWGColorReThr
        , 0
        , 240
        , "灰白重识别阈值"
        , ""
        , PROJECT_LEVEL
        );

	GetInt("\\Tracker\\ProcessPlate"
        , "EnableColorThr"
        , &Get().cTrackerCfgParam.fEnableCorlorThr
        , Get().cTrackerCfgParam.fEnableCorlorThr
        , 0
        , 1
        , "灰黑控制开关"
        , ""
        , CUSTOM_LEVEL
        );

	GetInt("\\Tracker\\ProcessPlate"
        , "nBGColorThr"
        , &Get().cTrackerCfgParam.nBGColorThr
        , Get().cTrackerCfgParam.nBGColorThr
        , 0
        , 240
        , "灰黑控制阈值"
        , ""
        , PROJECT_LEVEL
        );

	GetInt("\\Tracker\\ProcessPlate"
        , "nWGColorThr"
        , &Get().cTrackerCfgParam.nWGColorThr
        , Get().cTrackerCfgParam.nWGColorThr
        , 0
        , 240
        , "灰白控制阈值"
        , ""
        , PROJECT_LEVEL
        );

	
		return S_OK;
	}
	return E_FAIL;
}
	
HRESULT CSWLPRCapFaceParameter::InitHvDsp(VOID)
{
	if(S_OK == CSWLPRParameter::InitHvDsp())
	{
		GetEnum("\\HvDsp\\Misc"
		 	, "OutputCarSize"
		 	, &Get().cTrackerCfgParam.fOutputCarSize
		 	, Get().cTrackerCfgParam.fOutputCarSize
		 	, "0:不输出;1:输出"
  	  , "输出车辆尺寸"
  	  , ""
  	  , PROJECT_LEVEL
  	);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CSWLPRCapFaceParameter::InitCamApp(VOID)
{
	if(S_OK == CSWLPRParameter::InitCamApp())
	{
		GetEnum("\\HvDsp\\Camera\\Ctrl"
			, "EnableAutoCaptureParam"
			, &Get().cCamCfgParam.iCaptureAutoParamEnable
			, Get().cCamCfgParam.iCaptureAutoParamEnable
			, "0:关闭;1:打开"
			, "抓拍独立参数自动调整"
			, "0:关闭;1:打开"
			, CUSTOM_LEVEL
			);
		GetInt("\\HvDsp\\Camera\\Ctrl"
			, "CaptureDayShutterMax"
			, &Get().cCamCfgParam.iCaptureDayShutterMax
			, Get().cCamCfgParam.iCaptureDayShutterMax
			, 400
			, 3000
			, "抓拍白天最大快门"
			, ""
			, CUSTOM_LEVEL
			);
		GetInt("\\HvDsp\\Camera\\Ctrl"
			, "CaptureDayGainMax"
			, &Get().cCamCfgParam.iCaptureDayGainMax
			, Get().cCamCfgParam.iCaptureDayGainMax
			, 100
			, 280
			, "抓拍白天最大增益"
			, ""
			, CUSTOM_LEVEL
			);
		GetInt("\\HvDsp\\Camera\\Ctrl"
			, "CaptureNightShutterMax"
			, &Get().cCamCfgParam.iCaptureNightShutterMax
			, Get().cCamCfgParam.iCaptureNightShutterMax
			, 400
			, 3000
			, "抓拍晚上最大快门"
			, ""
			, CUSTOM_LEVEL
			);
		GetInt("\\HvDsp\\Camera\\Ctrl"
			, "CaptureNightGainMax"
			, &Get().cCamCfgParam.iCaptureNightGainMax
			, Get().cCamCfgParam.iCaptureNightGainMax
			, 100
			, 280
			, "抓拍晚上最大增益"
			, ""
			, CUSTOM_LEVEL
			);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CSWLPRCapFaceParameter::InitIPTCtrl(VOID)
{
    if (S_OK == CSWLPRParameter::InitIPTCtrl())
    {		//去掉外接红绿灯的设备
    		CSWString strCOMM;
				for(int i = 0; i < 2; i++)
				{	
					strCOMM.Format("\\Device[外部设备]\\COM%d[端口%02d]", i, i);
					GetEnum((LPCSTR)strCOMM
  				  	, "DeviceType"
  				  	, &Get().cDevParam[i].iDeviceType
  				  	, 0
  				  	, "0:无;1:川速雷达;2:苏江车检器;4:奥利维亚雷达;5:四川九洲雷达"
  				  	, "外部设备类型"
  				  	, ""
  				  	, CUSTOM_LEVEL
  				);
  			}
        return S_OK;
    }
    return E_FAIL ;
}


