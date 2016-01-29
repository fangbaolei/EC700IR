#ifndef __SW_DOME_CAMERA_CTRL_H__
#define __SW_DOME_CAMERA_CTRL_H__
#include "SWObject.h"
#include "SWMessage.h"
#include "SWDomeCameraBase.h"
#include "SWDomeCameraStruct.h"
#include "EPAppDetCtrl.h"
#include "swpa_camera.h"

#define pi 3.1314

typedef enum _AE_MODE_E
{
	ENUM_AE_AUTO = 0,	//自动模式
	ENUM_AE_MANUAL,		//手动模式
	ENUM_AE_SHUTTER,    //快门优先
	ENUM_AE_IRIS		//光圈优先
}AE_MODE_E;

typedef enum _AF_MODE_E
{
	ENUM_AF_AUTO = 0,
	ENUM_AF_MANUAL
}AF_MODE_E;

typedef enum _AWB_MODE_E
{
	ENUM_AWB_AUTO = 0,
	ENUM_AWB_INDOOR,
	ENUM_AWB_OUTDOR,
	ENUM_AWB_ONE_PUSH_WB,
	ENUM_AWB_ATW,
	ENUM_AWB_MANUAL,
	ENUM_AWB_OUTDOOR_AUTO,
	ENUM_AWB_SODIUM_LAMP_AUTO,
	ENUM_AWB_SODIUM_LAMP
}AWB_MODE_E;

typedef enum _FOCUS_MODE_E
{
	ENUM_FOCUS_NEAR = 0,
	ENUM_FOCUS_FAR,
	ENUM_FOCUS_ONE_PUSH,
	ENUM_FOCUS_INFINITY,
	ENUM_FOCUS_STOP
}FOCUS_MODE_E;

typedef enum _SHUTTER_E
{
	ENUM_SHUTTER_1_1 = 0,
	ENUM_SHUTTER_1_2,
	ENUM_SHUTTER_1_3,
	ENUM_SHUTTER_1_6,
	ENUM_SHUTTER_1_12,
	ENUM_SHUTTER_1_25,
	ENUM_SHUTTER_1_50,
	ENUM_SHUTTER_1_75,
	ENUM_SHUTTER_1_100,
	ENUM_SHUTTER_1_120,
	ENUM_SHUTTER_1_150,
	ENUM_SHUTTER_1_215,
	ENUM_SHUTTER_1_300,
	ENUM_SHUTTER_1_425,
	ENUM_SHUTTER_1_600,
	ENUM_SHUTTER_1_1000,
	ENUM_SHUTTER_1_1250,
	ENUM_SHUTTER_1_1750,
	ENUM_SHUTTER_1_2500,
	ENUM_SHUTTER_1_3500,
	ENUM_SHUTTER_1_6000,
	ENUM_SHUTTER_1_10000
}SHUTTER_E;

/*
typedef struct DOME_SRC_STATUS
{
    double dbMaxHViewAng;          // 最大水平视场角
    double dbMinHViewAng;          // 最小水平视场角
    double dbMaxVViewAng;          // 最大垂直视场角
    double dbMinVViewAng;          // 最小垂直视场角
    double dbMaxFocalDis;          // 最大焦距
    double dbMinFocalDis;          // 最小焦距
    double dbCurHViewAng;          // 当前的水平视场角
    double dbCurVViewAng;          // 当前的垂直视场角
    double dbCurHTurnAng;          // 当前水平转角
    double dbCurVTurnAng;          // 当前垂直转角
    double dbImageCenterX;         // 图像的中心坐标X
    double dbImageCenterY;         // 图像的中心坐标Y
    double dbInputLTopX;           // 输入方框的左上X坐标
    double dbInputLTopY;           // 输入方框的左上Y坐标
    double dbInputRLowerX;         // 输入方框的右下X坐标
    double dbInputRLowerY;         // 输入方框的右下Y坐标
	double dbInputPointX;			//点移动X坐标
	double dbInputPointY;			//点移动Y坐标
}DOME_SRC_STATUS;

// 输出参数结构
typedef struct DOME_DST_STATUS		
{
    double dbPan;
    double dbTilt;
    double dbZoom;
    double dbZoomPosition;			//放大后的焦距
}DOME_DST_STATUS;


// 焦距的对应表结构
typedef struct tag_ZoomRatio
{
int iRatio;
int iValue;
} ZOOM_RATIO;
*/


class CSWDomeCameraCtrl : public CSWObject
{
	CLASSINFO(CSWDomeCameraCtrl, CSWObject)
public:
	CSWDomeCameraCtrl();
	virtual ~CSWDomeCameraCtrl();

	INT GetMinPlateAvgY(){ return m_iMinPlateAvgY;}

	INT GetMaxPlateAvgY(){ return m_iMaxPlateAvgY;}
	/**
	   @brief 设置检测车辆配置
	 */
	HRESULT SetDetectCarConfig(BOOL fIsDay);

	/**
	   @brief 白天检测车辆配置
	 */
	HRESULT SetDayDetectCarConfig();


	/**
	   @brief 夜晚检测车辆配置
	 */
	HRESULT SetNightDetectCarConfig();

	/**
	   @brief 设置检牌配置
	 */
	HRESULT SetDetectPlateConfig(INT iBrightness = 0);

	/**
	   @brief 设置检车牌flag
	 */
	VOID SetDetectPlateConfigFlag(BOOL fFlag);

	/**
	   @brief 是否已设置检车牌配置
	 */
	BOOL IsSetDetectPlateConfig();
	
	/**
	   @brief 设置白天检牌配置
	 */
	HRESULT SetDayDetectPlateConfig(INT iBrightness);

	/**
	   @brief 设置夜晚检牌配置
	 */
	HRESULT SetNightDetectPlateConfig(INT iBrightness);

	/**
	   @brief 设置车牌标识
	 */
	HRESULT SetNightDetectPlateConfigFlag(BOOL fFlag);

	/**
	   @brief 是否是设置了夜晚配置
	 */
	BOOL IsNightDetectPlateConfig();

	/**
	   @brief 获取当前的相机配置
	 */
	HRESULT GetCurrentCameraConfig();

	/**
	   @brief 恢复原来设置
	 */
	HRESULT RestoreConfig();

	/**
	   @brief 重置配置，主要时增益上限和曝光补偿
	 */
	HRESULT ResetConfig();

	/**
	   @brief 是否是白天配置
	 */
	BOOL IsDayDetectCarConfig();

	/**
	   @brief 是否时夜晚检车辆配置
	 */
	BOOL IsNightDetectCarConfig();

	/**
	   @brief 晚上使用LED灯光
	 */
	BOOL IsNightUseLedLight();

	/**
	   @brief 晚上设置LED灯光
	 */
	VOID SetNightUseLedLight(BOOL fFlag);

	/**
	   @brief 设置白天配置标识
	 */
	VOID SetDayDetectCarConfigFlag(BOOL fDayConfig);

	/**
	   @brief 设置夜晚配置标识
	 */
	VOID SetNightDetectCarConfigFlag(BOOL fNightConfig);


	/**
	  @brief 获取球机预置位PTZ信息
	  @param [OUT] iPan 水平值
	  @param [OUT] iTilt 垂直值
	  @param [OUT] iZoom 缩放值
	 */
	HRESULT GetDomeCameraPresetPTZ(FLOAT *iPan, FLOAT *iTilt, FLOAT *iZoom);

	/**
	  @brief 设置球机预置位PTZ信息
	  @param [IN] iPan 水平值
	  @param [IN] iTilt 垂直值
	  @param [IN] iZoom 缩放值
	 */
	HRESULT SetDomeCameraPresetPTZ(FLOAT fltPan, FLOAT fltTilt, FLOAT fltZoom);

	/**
	   @brief 获取巡航路线值
	   @param [OUT] iCruiseNum 巡航路线编号
	 */
	HRESULT GetDomeCameraCruiseNum(INT *iCruiseNum);

	/**
	   @brief 获取当前预置位值
	   @param [OUT] iPresetNum 当前巡航路线预置位编号
	 */
	HRESULT GetDomeCameraPresetNum(INT *iPresetNum);


	/**
	   @brief 获取预置位名字
	 */
	HRESULT GetDomeCameraPresetName(INT iPresetNum, CHAR *szName, INT iNameLen);

	/**
	  @brief 暂停巡航
	 */
	HRESULT DomeCameraCruisePause(VOID);

	/**
	   @brief 恢复球机巡航
	 */
	HRESULT DomeCameraCruiseResume(VOID);

	/**
	   @brief 调用预置位
	   @param [IN] iPresetNum  预置位号
	 */
	HRESULT CallDomeCameraPreset(INT iPresetNum);


	/**
	   @brief 计算球机移动的水平值和垂直值，及机芯的缩放值
	   @param [IN] pSrcParam 原状态参数
	   @param [OUT] pDstParam 目标状态参数
	   @retval TRUE - 正确  FALSE - 错误
	*/
	BOOL ComputeDemoCameraPTZ( DOME_SRC_STATUS *pSrcParam, 
							   DOME_DST_STATUS *pDstParam);

	/**
	   @brief 框坐标换算成PTZ值
	 */
	HRESULT PositionToPTZ(HV_RECT *pRect, DOME_DST_STATUS *pDst);

	/**
	   @brief 利用放大倍数来计算球机水平方向的角度
	   @param [IN] fltZoom 相机放大倍数
	   @retval 水平角度值
	*/	
	FLOAT ZoomToHViewAngle( FLOAT fltZoom );

	/**
	   @brief 利用放大倍数来计算球机和垂直方向的角度
	   @param [IN] iZoom 相机放大倍数
	   @retval 垂直角度值
	*/
	FLOAT ZoomToVViewAngle( FLOAT fltZoom );

	/**
	   @brief 换算机芯放大倍数值->放大倍数比例
	   @param iDomeZoomValue [IN] 机芯放大倍数值
	   @retval 放大的比率倍数的浮点数
	 */
	FLOAT ZoomValueToRatio(INT iDomeZoomValue);

	/**
	   @brief 换算放大倍数比率->机芯放大倍数值
	   @param fltZoomRatio [IN] 放大比率倍数值
	   @retval 机芯放大倍数值
	 */
	INT ZoomRatioToValue(FLOAT fltZoomRatio);

	/**
	   @brief 取得gamma数值
	 */
	HRESULT GetGammaMode(INT *pGammaMode);

	/**
	   @brief 设置gamma数值
	 */
	HRESULT SetGammaMode(INT iGammaMode);

	/**
	   @brief 框选放大
	 */
	HRESULT MoveBlockToCenter(HV_RECT *pBlowUpParam);

	/**
	   @brief 设置AWB模式
	 */
	HRESULT SetAWBMode(INT iAWBMode);

	/**
	   @brief 手动设置AWB RGB增益
	 */
	HRESULT SetRGBGain(INT *pRGBGain);

	/**
	   @brief 获取手动AWB RGB增益
	 */
	HRESULT GetRGBGain(INT *pRGBGain);

	/**
	   @brief 设置AE模式
	   @param [IN] iAEMode, 0自动 1手动 2快门优先 3光圈优先
	 */
	HRESULT SetAEMode(INT iAEMode);

	/**
	   @brief 获取AE模式
	 */
	HRESULT GetAEMode(INT *pAEMode);

	/**
	   @brief 设置光圈
	   @param [IN] iIrisValue 0~13 14个级别
	 */
	HRESULT SetIris(INT iIrisValue);

	/**
	   @brief 获取光圈
	   
	 */
	HRESULT GetIris(INT *pIrisValue);

	/**
	   @brief 获取AGC增益值
	 */
	HRESULT GetAGCGain(INT *pAGCGain);


	/**
	  @brief 设置快门能优先
	 */
	HRESULT SetShutter(INT iShutterValue);

	/**
	   @brief 获取快门值
	 */
	HRESULT GetShutter(INT *pShutterValue);

	/**
	   @brief 使能AF
	 */
	HRESULT SetAFEnable(INT iEnable);

	/**
	   @brief 获取AF
	 */
	HRESULT GetAFEnable(INT *pEnable);

	/**
	   @brief 设置手动聚焦模式
	 */
	HRESULT SetFocusMode(INT iFocusMode);

	/**
	   @brief 一次聚焦
	 */
	HRESULT DoOneFocus();

	/**
	   @brief 是否已经聚焦
	 */
	BOOL IsFocused();

	/**
	   @brief 设置是否已经聚焦
	 */
	HRESULT SetManualFocusFlag(BOOL iFlag);


	/**
	   @brief 设置饱和度
	 */
	HRESULT SetSaturationThreshold(INT iSaturationThreshold);

	/**
	   @brief 图像锐度增强
	 */
	HRESULT SetEdgeEnhance(INT iEdgeValue);

	/**
	   @brief 设置降噪等级
	 */
	HRESULT SetNRLevel(INT iNRLevel);

	/**
	   @brief 曝光补偿
	 */
	HRESULT SetExpoComp(INT iExpoCompValue);

	/**
	   @brief 获取曝光补偿
	 */
	HRESULT GetExpoComp(INT *pExpoComValue);

	/**
	   @brief 设置聚焦值
	 */
	HRESULT SetFocusValue(INT iFocusValue);

	/**
	   @brief 获取聚焦值
	 */
	HRESULT GetFocusValue(INT *pFocusValue);

	/**
	  @brief 设置LED等模式
	 */
	HRESULT SetLedMode(INT iLedMode);

	/**
	   @brief 估算球机到车距离
	   @param [IN] fltAngleOfHView 水平视场角
	   @param [OUT] pDistance 推算的距离，单位：米
	 */
	HRESULT EstimateDistance(FLOAT fltAngleOfHView, INT *pDistance);

	/**
	   @brief 根据Zoom值使用聚焦值
	   @param [IN] fltZoom 缩放值
	 */
	HRESULT ZoomToFocusValue(FLOAT fltZoom, INT *pFocusValue);


	/**
	   @brief 根据聚焦值计算Led功率
	 */
	HRESULT ZoomToLedPowerValue(FLOAT fltZoom, INT *pLedPowerValue);

	/**
	   @brief 设置LED等功率
	 */
	HRESULT SetLedPower(INT iPowerValue);

	/**
	   @brief 获取LED功率值
	 */
	HRESULT GetLedPower(INT *pPowerValue);

	/**
	   @brief 设置增益限制
	 */
	HRESULT SetGainLimit(INT iGainLimit);

	/**
	  @brief 设置相机
	 */
	HRESULT SetCameraModel(INT iCameraModel);

private:
	BOOL m_fDayDetectCarConfigFlag;
	BOOL m_fNightDetectCarConfigFlag;
	BOOL m_fNightDetectPlateConfigFlag;
	BOOL m_fIsFocusFlag;	//晚上使用
	BOOL m_fNightUseLedLight;
	BOOL m_fIsSetDetectPlateConfigFlag;
	BOOL m_fIsDay;
	INT m_iMinPlateAvgY;
	INT m_iMaxPlateAvgY;
	INT m_iGainLimit;
	INT m_iCameraModel;
	INT m_rgRGBGain[3];
};

#endif
