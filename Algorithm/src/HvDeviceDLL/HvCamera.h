#ifndef _HVCAMARA_H__
#define _HVCAMARA_H__

#include "HvDeviceBaseType.h"
#include "HvDeviceCommDef.h"

#ifdef HVDEVICE_EXPORTS
#define HV_API_EX extern "C" __declspec(dllexport)
#elif HVDEVICE_LIB
#define HV_API_EX
#else
#define HV_API_EX extern "C" __declspec(dllimport)
#endif

#define HVAPI_API_VERSION_EX	"2.0"
typedef PVOID HVAPI_HANDLE_EX;         /**< HvAPI句柄类型 */

/**
* @brief		设置视频流帧率
* @param[in]	hHandle				对应设备的有效句柄
* @param[in]	nStreamId			视频流ID，0：H264,1:MJPEG
* @param[in]	nFps				帧率
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetStreamFps(HVAPI_HANDLE_EX hHandle, INT nStreamId, INT nFps);

/**
* @brief		设置H264码率控制方式
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nType			0：VBR ,1 CBR
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetH264BitRateControl(HVAPI_HANDLE_EX hHandle, INT nType );

/**
* @brief		设置H264码率
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nBitRate		范围 32768~8388608(32Kbps-8Mbps)
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetH264BitRate(HVAPI_HANDLE_EX hHandle, INT nBitRate );

/**
* @brief		设置JPEG压缩质量
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nJpegCompressRate		范围 20～80
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetJpegCompressRate(HVAPI_HANDLE_EX hHandle, INT nJpegCompressRate );


/**
* @brief		设置字符叠加开关
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nStreamId		视频流ID，0：H264,1:MJPEG
* @param[in]	fOSDEnable		0：关闭，1：打开
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetOSDEnable(HVAPI_HANDLE_EX hHandle, INT nStreamId ,BOOL fOSDEnable);
/**
* @brief		设置车牌字符叠加开关
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nStreamId		视频流ID，0：H264,1:MJPEG
* @param[in]	fOSDEnable		0：关闭，1：打开
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetOSDPlateEnable(HVAPI_HANDLE_EX hHandle, INT nStreamId ,BOOL fOSDEnable);

/**
* @brief		设置字符叠加-时间叠加开关
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nStreamId		视频流ID，0：H264,1:MJPEG
* @param[in]	fEnable			字符叠加时间叠加开关，范围：0：关闭，1：打开
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetOSDTimeEnable(HVAPI_HANDLE_EX hHandle, INT nStreamId, BOOL fEnable);

/**
* @brief		设置字符叠加位置
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nStreamId		视频流ID，0：H264,1:MJPEG
* @param[in]	nPosX			叠加位置X坐标，范围: 0~图像宽
* @param[in]	nPosY			叠加位置Y坐标，范围：0~图像高
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetOSDPos( HVAPI_HANDLE_EX hHandle,INT nStreamId, INT nPosX ,INT nPosY );

/**
* @brief		设置字符叠加字体
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nStreamId		视频流ID，0：H264,1:MJPEG
* @param[in]	nFontSize		字体大小，范围：16~32
* @param[in]	nColorR			字体颜色R值，范围0～255
* @param[in]	nColorG			字体颜色G值，范围0～255
* @param[in]	nColorB			字体颜色B值，范围0～255
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetOSDFont(HVAPI_HANDLE_EX hHandle, INT nStreamId ,INT nFontSize ,
										 INT nColorR ,INT nColorG,INT nColorB);
/**
* @brief		设置字符叠加字符串
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nStreamId		视频流ID，0：H264,1:MJPEG
* @param[in]	szText			叠加字符串 长度范围：0～255
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetOSDText(HVAPI_HANDLE_EX hHandle, INT nStreamId,CHAR* szText  );

/**
* @brief		设置CVBS制式
* @param[in]	hHandle		对应设备的有效句柄
* @param[in]	nMode		CVBS制式，范围：0:PAL,1:NTSC
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetCVBDisPlayMode(HVAPI_HANDLE_EX hHandle, INT nMode );

/**
* @brief		设置手动快门
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nShutter		快门，范围：100~30000
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetManualShutter(HVAPI_HANDLE_EX hHandle, INT nShutter );

/**
* @brief		设置手动增益
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nGain			增益，范围：0~42
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetManualGain(HVAPI_HANDLE_EX hHandle, INT nGain );

/**
* @brief		设置AWB开关
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nEnable			AWB开关,范围：0：关闭，1：打开
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetAWBEnable(HVAPI_HANDLE_EX hHandle, INT nEnable );

/**
* @brief		设置手动RGB
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nGainR			R值，范围：0～255
* @param[in]	nGainG			G值，范围：0～255
* @param[in]	nGainB			B值，范围：0～255
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetManualRGB(HVAPI_HANDLE_EX hHandle, INT nGainR, INT nGainG, INT nGainB  );


/**
* @brief		设置AGC使能开关
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nEnable			开关，范围：0：关闭，1：打开
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetAGCEnable(HVAPI_HANDLE_EX hHandle, INT nEnable );


/**
* @brief		设置AGC基准值
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nLightBaseLine	基准值，范围：0～255
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetAGCLightBaseLine(HVAPI_HANDLE_EX hHandle, INT nLightBaseLine );


/**
* @brief		设置AGC区域
* @param[in]	hHandle		对应设备的有效句柄
* @param[in]	rgZone		AGC区域值，范围：0：区域外，1：区域内
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetAGCZone(HVAPI_HANDLE_EX hHandle, INT rgZone[16] );

/**
* @brief		获取AGC检测区域,16个值
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	rgZone[16]	    ACG区域
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_GetAGCZone(HVAPI_HANDLE_EX hHandle, INT rgZone[16]);


/**
* @brief		设置AGC参数
* @param[in]	hHandle		对应设备的有效句柄
* @param[in]	nShutterMin ,AGC最小快门，范围： 100~30000
* @param[in]	nShutterMax ,AGC最大快门，范围： 100~30000
* @param[in]	nGainMin    ,AGC最小增益，范围： 0~42
* @param[in]	nGainMax    ,AGC最大增益，范围： 0~42
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetAGCParam(HVAPI_HANDLE_EX hHandle,INT nShutterMin , INT nShutterMax ,
										  INT nGainMin , INT nGainMax );

/**
* @brief		设置亮度LUT
* @param[in]	hHandle				对应设备的有效句柄
* @param[in]	rgLUT				LUT点
* @param[in]	nLUTPointCount		LUT点数，范围：8
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetLUT(HVAPI_HANDLE_EX hHandle, CHvPoint rgLUT[] , INT nLUTPointCount );

/**
* @brief		获取亮度LUT状态
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	rgLUT			LUT点状态
* @param[in]	pnLUTPointCount	LUT点数量
** @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_GetLUT(HVAPI_HANDLE_EX hHandle, CHvPoint rgLUT[] , INT* pnLUTPointCount );


/**
* @brief		设置亮度值
* @param[in]	hHandle		 对应设备的有效句柄
* @param[in]	nBrightness ,亮度值，范围：0～255  
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetBrightness(HVAPI_HANDLE_EX hHandle, INT nBrightness );


/**
* @brief		设置对比度
* @param[in]	hHandle		 对应设备的有效句柄
* @param[in]	nContrast    对比度值，范围：0～255  
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetContrast(HVAPI_HANDLE_EX hHandle, INT nContrast );


/**
* @brief		设置锐度
* @param[in]	hHandle		 对应设备的有效句柄
* @param[in]	nSharpness    对比度值，范围：0～255  
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetSharpness(HVAPI_HANDLE_EX hHandle, INT nSharpness );


/**
* @brief		设置饱和度
* @param[in]	hHandle		 对应设备的有效句柄
* @param[in]	nSaturation   饱和度值，范围：0～255  
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetSaturation(HVAPI_HANDLE_EX hHandle, INT nSaturation );


/**
* @brief		设置宽动态开关
* @param[in]	hHandle	      对应设备的有效句柄
* @param[in]	fEnable		  宽动态开关，范围：0：关闭，1：打开  
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetWDREnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable  );


/**
* @brief		设置宽动态等级
* @param[in]	hHandle	      对应设备的有效句柄
* @param[in]	nLevel		  宽动态等级，范围：0：弱，1：中，2：强
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetWDRLevel(HVAPI_HANDLE_EX hHandle, INT nLevel );

/**
* @brief		设置视频降噪TNF开关
* @param[in]	hHandle	      对应设备的有效句柄
* @param[in]	fEnable		  视频降噪开关，范围：0：关闭，1：打开
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetDeNoiseTNFEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable );


/**
* @brief		设置视频降噪SNF开关
* @param[in]	hHandle	      对应设备的有效句柄
* @param[in]	fEnable		  视频降噪开关，范围：0：关闭，1：打开
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetDeNoiseSNFEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable );

/**
* @brief		设置视频降噪模式
* @param[in]	hHandle	      对应设备的有效句柄
* @param[in]	nMode		  视频降噪模式，范围：0：速度 ，1：质量 ，该接口设置需复位生效
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetDeNoiseMode(HVAPI_HANDLE_EX hHandle, INT nMode );

/**
* @brief		设置视频降噪组合开关
* @param[in]	hHandle	      对应设备的有效句柄
* @param[in]	nMode		  视频降噪开关，范围：0：SNF,1:TNF,2:TNF+SNF
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetDeNoiseSwitch(HVAPI_HANDLE_EX hHandle, INT nSwitch );

/**
* @brief		设置视频降噪等级
* @param[in]	hHandle	      对应设备的有效句柄
* @param[in]	nLevel		  视频降噪等级，范围：0: 自动 1：弱，2：中，3：强
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetDeNoiseLevel(HVAPI_HANDLE_EX hHandle,INT nLevel );


/**
* @brief		设置滤光片模式
* @param[in]	hHandle	      对应设备的有效句柄
* @param[in]	nMode		  滤光片模式，范围：0: 自动，1：过滤红外 ，2：不过滤红外
										 （若采用非自动模式 晚上采用：2 白天采用：1）
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetFilterMode(HVAPI_HANDLE_EX hHandle, INT nMode );


/**
* @brief		设置滤光片使能
* @param[in]	hHandle	      对应设备的有效句柄	
* @param[in]	fEnable		  滤光片使能，范围：0：关闭，1：打开
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetCtrlCplEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable );


/**
* @brief		设置DC光圈自动控制
* @param[in]	hHandle	      对应设备的有效句柄
* @param[in]	fEnable		  DC光圈自动控制开关，范围：0：关闭，1：打开
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetDCIRIS(HVAPI_HANDLE_EX hHandle, BOOL fEnable );


/**
* @brief		设置IP信息
* @param[in]	hHandle	      对应设备的有效句柄
* @param[in]	szIP		  IP字符串
* @param[in]	szMask		  子网掩码字符串
* @param[in]	szGateWay	  网关字符串
* @param[in]	szDNS		  DNS字符串
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetIPInfo(HVAPI_HANDLE_EX hHandle, CHAR* szIP 
										,CHAR* szMask 
										,CHAR* szGateWay 
										,CHAR* szDNS  );

/**
* @brief		设置RTSP多播开关
* @param[in]	hHandle	      对应设备的有效句柄
* @param[in]	fEnable		RTSP多播开关，范围：0：关闭，1：打开
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetRTSPMulticastEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable );


/**
* @brief		设置时间
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	nYear		年
* @param[in]	nMon		月
* @param[in]	nDay		日
* @param[in]	nHour		时
* @param[in]	nMin		分
* @param[in]	nSec		秒
* @param[in]	nMSec		毫秒
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetTime(HVAPI_HANDLE_EX hHandle, INT nYear , INT nMon , INT nDay , INT nHour ,INT nMin , INT nSec , INT nMSec );

/**
* @brief		设置时区
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	nZoneNum	 时区编号 0~24
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetTimeZone(HVAPI_HANDLE_EX hHandle, INT nZoneNum );

/**
* @brief		设置NTP服务器IP
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	szIP		 NTP服务器IP字符串
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetNTPServerIP(HVAPI_HANDLE_EX hHandle, CHAR* szIP );


/**
* @brief		设置NTP服务器IP
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	dw32UpdateIntervalMS		 更新NTP服务器时间间隔
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetNTPServerUpdateInterval(HVAPI_HANDLE_EX hHandle,DWORD32 dw32UpdateIntervalMS );


/**
* @brief		设置NTP开关
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	fEnable		 NTP服务开关 0：关闭，1：打开
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetNTPEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable );

/*
* @brief		设置OC门
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	fEnable		 开关，范围：0：关闭，1：打开
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetOCGate(HVAPI_HANDLE_EX hHandle, BOOL fEnable );

/*
* @brief		设置色温
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	nValue		 范围：0:（自动）; 32; 40 ;52 ;55 ; 60 ;70 
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetColor(HVAPI_HANDLE_EX hHandle, INT nValue );

/*
* @brief		设置2D降噪开关
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	fEnable		 范围：0：关闭，1：打开
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_Set2DDeNoiseEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable );

/*
* @brief		设置2D降噪强度
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	nValue		 范围：0～255
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_Set2DDeNoiseStrength(HVAPI_HANDLE_EX hHandle, INT nValue );

/*
* @brief		设置GAMMA开关
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	fEnable		 范围：0： 关闭， 1：打开
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetGammaEnable(HVAPI_HANDLE_EX hHandle, BOOL fValue );

/*
* @brief		设置GAMMA强度
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	fValue		 范围：1～5
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetGammaStrength(HVAPI_HANDLE_EX hHandle, INT nValue );

/**
* @brief		复位设备
* @param[in]	hHandle	     对应设备的有效句柄
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_ResetDevice(HVAPI_HANDLE_EX hHandle , INT nRetsetMode );

/**
* @brief		恢复默认设置
* @param[in]	hHandle	     对应设备的有效句柄
* @return		成功：S_OK；失败：E_FAIL  
*/
HV_API_EX HRESULT CDECL HVAPI_RestoreDefaultParam(HVAPI_HANDLE_EX hHandle);

/**
* @brief		恢复出厂设置
* @param[in]	hHandle	     对应设备的有效句柄
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_RestoreFactoryParam(HVAPI_HANDLE_EX hHandle);

/**
* @brief		获取设备基本信息
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	pbasicInfo	 设备基本信息
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetDevBasicInfo(HVAPI_HANDLE_EX hHandle, CDevBasicInfo* pbasicInfo  );

/**
* @brief		获取运行模式
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	pnRunMode	运行模式指针
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetRunMode(HVAPI_HANDLE_EX hHandle, INT* pnRunMode );

/**
* @brief		获取设备状态
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	pState	 状态结构体指针
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetDevState(HVAPI_HANDLE_EX hHandle, CDevState* pState  );

/**
* @brief		获取设备状态
* @param[in]	hHandle	     对应设备的有效句柄
* @param[in]	pReportBuf	 复位报告内存指针
* @param[in]	pnBufLen	 复位报告数据长度
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetResetReport(HVAPI_HANDLE_EX hHandle,CHAR* pReportBuf, INT* pnBufLen  );

/**
* @brief		获取设备状态
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	pnResetCount	复位次数
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetResetCount(HVAPI_HANDLE_EX hHandle, INT* pnResetCount );

/**
* @brief		获取设备状态
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	szLog			保存日志数据内存指针
* @param[in]	pnLogLen		保存日志数据长度
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetLog(HVAPI_HANDLE_EX hHandle, CHAR* szLog , INT* pnLogLen );

/**
* @brief		获取设备状态
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	pVideoState		视频状态
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetVideoState(HVAPI_HANDLE_EX hHandle, CVideoState* pVideoState );

/**
* @brief		获取相机状态
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	pCameraState	相机状态指针
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetCameraState(HVAPI_HANDLE_EX hHandle, CCameraState* pCameraState );


/**
* @brief		获取相机基本信息
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	pCameraBasicInfo	相机基本信息指针
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetCameraBasicInfo(HVAPI_HANDLE_EX hHandle, CCameraBasicInfo* pCameraBasicInfo );

/**
* @brief		获取硬盘检测报告
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	szReport		检测报告
* @param[in]	piReportLen		检测报告长度
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetHddCheckReport(HVAPI_HANDLE_EX hHandle, CHAR* szReport , INT *piReportLen  );

/**
* @brief		获取字符叠加信息
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	nStreamId		视频流ID
* @param[in]	pOSDInfo		字符叠加信息
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetOSDInfo(HVAPI_HANDLE_EX hHandle,  INT nStreamId , COSDInfo* pOSDInfo  );


/**
* @brief		登录
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	szUserName		用户名字符串  长度范围：4～16字节
* @param[in]	szPassword		密码字符串  长度范围：4～16字节
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_Login(HVAPI_HANDLE_EX hHandle, CHAR* szUserName ,  CHAR* szPassword  );

/**
* @brief		增加用户
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	szUserName		用户名字符串 长度范围：4～16字节
* @param[in]	szPassword		密码字符串  长度范围：4～16字节
* @param[in]	nAuthority		用户等级
* @return		成功：S_OK；失败：E_FAIL
* @Remark		此接口调用前，须成功调用HVAPI_Login
*/
HV_API_EX HRESULT CDECL HVAPI_AddUser( HVAPI_HANDLE_EX hHandle, CHAR* szUserName , CHAR* szPassword,  INT nAuthority );

/**
* @brief		删除用户
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	szUserName		用户名字符串  长度范围：4～16字节
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
* @Remark		此接口调用前，须成功调用HVAPI_Login
*/
HV_API_EX HRESULT CDECL HVAPI_DelUser(HVAPI_HANDLE_EX hHandle, CHAR* szUserName );

/**
* @brief		获取用户列表
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	rgUserInfo		用户信息列表
* @param[in]	pnUserCount		用户数
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
* @Remark		此接口调用前，须成功调用HVAPI_Login
*/
HV_API_EX HRESULT CDECL HVAPI_GetUsersList(HVAPI_HANDLE_EX hHandle, CUserInfo rgUserInfo[] , INT* pnUserCount );

/**
* @brief		修改用户信息
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	szUserName		用户名字符串  长度范围：4～16字节
* @param[in]	szOldPassword	密码字符串	  长度范围：4～16字节
* @param[in]	szNewPassword	密码字符串	  长度范围：4～16字节
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
* @Remark		此接口调用前，须成功调用HVAPI_Login
*/
HV_API_EX HRESULT CDECL HVAPI_ModUser(HVAPI_HANDLE_EX hHandle,CHAR* szUserName ,CHAR* szOldPassword, CHAR* szNewPassword );
/**
* @brief		触发图片
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	pCapImage		抓拍图内存指针
* @param[in]	pnCapImageSize	抓拍图内存大小
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_TriggerImage(HVAPI_HANDLE_EX hHandle, PBYTE pCapImage , INT* pnCapImageSize );


/**
* @brief		启动设备串口检测功能
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	nCOMNum			调试码流开关
* @param[in]	fEnable	        开关标识
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_StartCOMCheck(HVAPI_HANDLE_EX hHandle, INT nCOMNum, BOOL fEnable );


/**
* @brief		设置锐度使能
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetSharpnessEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable);

/**
* @brief		设置相机光圈放大 
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_ZoomDCIRIS(HVAPI_HANDLE_EX hHandle); 

/**
* @brief		设置相机光圈缩小 
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_ShrinkDCIRIS(HVAPI_HANDLE_EX hHandle); 



/**
* @brief		黑白图模式
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	fEnable	        启动标志
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetGrayImageEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable); 


/**
* @brief		电网同步
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	nLevel	        电网同步模式
* @param[in]	dwDelayMS	    同步延时
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetACSync(HVAPI_HANDLE_EX hHandle, INT nLevel, DWORD32 dw32DelayMS); 

/**
* @brief		设置调试码流开关
* @param[in]	hHandle	        对应设备的有效句柄
* @param[in]	fEnable			调试码流开关
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetDebugJpegStatus(HVAPI_HANDLE_EX hHandle, BOOL fEnable);  

/**
* @brief		启动自动测试相机功能
* @param[in]	hHandle	        对应设备的有效句柄
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_StartCameraTest(HVAPI_HANDLE_EX hHandle);  

/**
* @brief		设置jpeg自动压缩使能
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetAutoJpegCompressEnable(HVAPI_HANDLE_EX hHandle,  BOOL fEnable);

/**
* @brief		获取调试等级
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetTraceRank(HVAPI_HANDLE_EX hHandle, INT nlevel);

/**
* @brief		设置jpeg自动压缩参数
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetAutoJpegCompressParam (HVAPI_HANDLE_EX hHandle, INT nMaxCompressRate, INT nMinCompressRate, INT nImageSize);

/**
* @brief		设置图像增强使能
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetImageEnhancementEnable(HVAPI_HANDLE_EX hHandle, BOOL fEnable);

/**
* @brief		设置红灯加红阈值
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetEnRedLightThreshold(HVAPI_HANDLE_EX hHandle, INT nValue);

/**
* @brief		设置增加边缘增强
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetEdgeEnhance(HVAPI_HANDLE_EX hHandle, INT nValue);

/**
* @brief		获取视频采集数据信息
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetRunStatusString(HVAPI_HANDLE_EX hHandle, CHAR* szRunStatusString, INT* pnRunStatusStringLen  );

/**
* @brief		获取增加摄像机工作状态
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetCameraWorkState(HVAPI_HANDLE_EX hHandle, CHAR* szStatus, INT* pnStatusStringLen  );

/**
* @brief		获取硬盘状态
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetHDDStatus(HVAPI_HANDLE_EX hHandle, CHAR* szStatus, INT* pnStatusStringLen  );


/**
* @brief		设置触发抓拍输入IO口触发类型
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetTGIO(HVAPI_HANDLE_EX hHandle, INT nValue  );

/**
* @brief		获取触发抓拍输入IO口触发类型
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetTGIO(HVAPI_HANDLE_EX hHandle, INT* pnValue  );

/**
* @brief		设置触发输出IO口状态 
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetF1IO(HVAPI_HANDLE_EX hHandle, INT nPolarity, INT nType);

/**
* @brief		获取触发输出IO口状态
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetF1IO(HVAPI_HANDLE_EX hHandle, INT* pnPolarity, INT* pnType);


/**
* @brief		设置补光灯IO口状态 
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetEXPIO(HVAPI_HANDLE_EX hHandle, INT nPolarity, INT nType);


/**
* @brief		获取补光灯IO口状态 
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetEXPIO(HVAPI_HANDLE_EX hHandle, INT* pnPolarity, INT* pnType);


/**
* @brief		设置报警IO口状态 
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetALMIO(HVAPI_HANDLE_EX hHandle, INT nPolarity, INT nType);


/**
* @brief		获取报警IO口状态 
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetALMIO(HVAPI_HANDLE_EX hHandle, INT* pnPolarity, INT* pnType);

/**
* @brief		设置MJPEG区域
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetMJPEGRect(HVAPI_HANDLE_EX hHandle, CHvPoint PosTopLeft, CHvPoint PosLowerRight);


/**
* @brief		设置色阶 
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetColorGradation(HVAPI_HANDLE_EX hHandle, INT nValue);

/**
* @brief		设置视频请求控制 
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetVedioRequestControl(HVAPI_HANDLE_EX hHandle, BOOL fEnable,  DWORD32 dwIP, INT nPort);

/**
* @brief		设置AE场景 
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetAEScene(HVAPI_HANDLE_EX hHandle, INT nMode);

/**
* @brief		获取AE场景 
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetAEScene(HVAPI_HANDLE_EX hHandle, INT* pnMode);


/**
* @brief		获取设备名 
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetCustomizedDevName(HVAPI_HANDLE_EX hHandle, CHAR* pszDevName, INT* pnDevNameLen);


/**
* @brief		设置设备名 
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetCustomizedDevName(HVAPI_HANDLE_EX hHandle, CHAR* pszDevName);


/**
* @brief		设置H264码率
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nBitRate		
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_SetH264SecondBitRate(HVAPI_HANDLE_EX hHandle, INT nBitRate );


/**
* @brief		获取H264码率
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	pnBitRate		
* @return		成功：S_OK；失败：E_FAIL  传入参数异常：S_FALSE
*/
HV_API_EX HRESULT CDECL HVAPI_GetH264SecondBitRate(HVAPI_HANDLE_EX hHandle, INT* pnBitRate );

/**
* @brief		软触发抓拍
* @param[in]	hHandle			对应设备的有效句柄	
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SoftTriggerCapture(HVAPI_HANDLE_EX hHandle);

/**
* @brief		设置抓拍快门
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nEnable			使能开关
* @param[in]	nValue			抓拍快门值
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetManualCaptureShutter(HVAPI_HANDLE_EX hHandle, INT nEnable, INT nValue);

/**
* @brief		设置抓拍增益
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nEnable			使能开关
* @param[in]	nValue			抓拍增益值
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetManualCaptureGain(HVAPI_HANDLE_EX hHandle, INT nEnable, INT nValue);

/**
* @brief		设置抓拍RGB增益
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nEnable			使能开关
* @param[in]	nValueR			抓拍R增益
* @param[in]	nValueG			抓拍G增益
* @param[in]	nValueB			抓拍B增益
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetManualCaptureRGB(HVAPI_HANDLE_EX hHandle, INT nEnable, INT nValueR, INT nValueG, INT nValueB);

/**
* @brief		设置抓拍锐度
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nEnable			使能开关
* @param[in]	nValue			抓拍锐度值
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetManualCaptureSharpen(HVAPI_HANDLE_EX hHandle, INT nEnable, INT nSharpen);

/**
* @brief		设置LED脉宽
* @param[in]	hHandle			对应设备的有效句柄
* @param[in]	nValue			脉宽值
* @return		成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetExpPluseWidth(HVAPI_HANDLE_EX hHandle, INT nValue);

/**
* @brief			设置人脸库
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		nNum			要设置的序号   0标准库  1增强库    2 人脸信息库
* @param[in]		szUploadDate		要保存到设备的数据
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetFaceDataAdv(HVAPI_HANDLE_EX hHandle, INT nNum, CHAR* szUploadDate, DWORD32 dwDataLen);

/**
* @brief			设置DSP参数
* @param[in]		hHandle			对应设备的有效句柄
* @param[in]		szUploadDate		要保存到设备的数据
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetDSPParam(HVAPI_HANDLE_EX hHandle, CHAR* szUploadDate, DWORD32 dwDataLen);

/**
* @brief			设置JPEG输出类型
* @param[in]		hHandle		对应设备的有效句柄
* @param[in]		nMode		要保存到设备的数据
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetJpegOutType(HVAPI_HANDLE_EX hHandle,  INT nMode);
/**
* @brief			获取JPEG输出类型
* @param[in]		hHandle		对应设备的有效句柄
* @param[in]		pnMode		要保存到设备的数据
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetJpegOutType(HVAPI_HANDLE_EX hHandle,  INT* pnMode);
/**
* @brief			设置视频触发结果输出选择
* @param[in]		hHandle		对应设备的有效句柄
* @param[in]		nMode		要保存到设备的数据
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_SetCoilEnable(HVAPI_HANDLE_EX hHandle,  INT nMode);
/**
* @brief			获取视频触发结果输出选择
* @param[in]		hHandle		对应设备的有效句柄
* @param[in]		pnMode		要保存到设备的数据
* @return			成功：S_OK；失败：E_FAIL
*/
HV_API_EX HRESULT CDECL HVAPI_GetCoilEnable(HVAPI_HANDLE_EX hHandle,  INT* pnMode);

//获取AWB工作模式
HV_API_EX HRESULT CDECL HVAPI_GetAWBWorkMode(HVAPI_HANDLE_EX hHandle,  BOOL* pfEnable);
//设置AWB工作模式
HV_API_EX HRESULT CDECL HVAPI_SetAWBWorkMode(HVAPI_HANDLE_EX hHandle, INT nEnable );
//获取相机全控制
HV_API_EX HRESULT CDECL HVAPI_GetAutoControlCammeraEnable(HVAPI_HANDLE_EX hHandle,  BOOL* pfEnable);
//设置相机全控制
HV_API_EX HRESULT CDECL HVAPI_SetAutoControlCammeraEnable(HVAPI_HANDLE_EX hHandle, INT nEnable );
//获取H264分辨率
HV_API_EX HRESULT CDECL HVAPI_GetH264Resolution(HVAPI_HANDLE_EX hHandle,  INT* piValue);
//设置H264分辨率
HV_API_EX HRESULT CDECL HVAPI_SetH264Resolution(HVAPI_HANDLE_EX hHandle, INT nEnable );

#endif
