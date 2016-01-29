#ifndef __SW_DOMECAMERA_PARAMETER_H__
#define __SW_DOMECAMERA_PARAMETER_H__


#include "SWFC.h"
#include "SWDomeCameraStruct.h"





typedef struct _tagDomeCameraParams
{
	INT		iPanCoordinate;
	INT		iTiltCoordinate;

	INT		iPanSpeed;
	INT		iMaxPanSpeed;
	INT		iTiltSpeed;
	INT		iMaxTiltSpeed;

	INT 	iFocusMode;

	INT		iLEDMode;
	INT		iLEDPower;

	INT		iLastTask;
	INT		iLastTaskID;
	
	PRESETPOS_PARAM_STRUCT	sPresetParam[MAX_PRESET];
	HSCAN_PARAM_STRUCT		sHScanParam[MAX_HSCAN_PATH];
	FSCAN_PARAM_STRUCT		sFScanParam[MAX_FSCAN_PATH];
	CRUISE_PARAM_STRUCT		sCruiseParam[MAX_CRUISE];
	WATCHKEEPING_PARAM_STRUCT	sWatchKeepingParam;
	PICTUREFREEZE_PARAM_STRUCT	sPictureFreezeParam;
	MASK_PARAM_STRUCT		sMaskParam[MAX_MASK];
	TIMER_PARAM_STRUCT		sTimerParam[MAX_TIMER];
	STANDBY_PARAM_STRUCT		sStandbyParam;

	_tagDomeCameraParams()
	{
		iPanCoordinate = 0;
		iTiltCoordinate = 0;
		iPanSpeed = 15;
        iMaxPanSpeed = MAX_SPEED;//0x3F
        iTiltSpeed = 15;
        iMaxTiltSpeed = MAX_SPEED;//0x3F
		iLEDMode = 0;
		iLEDPower = 100;
		iLastTask = -1;
		iLastTaskID = -1;
	}
} DomeCameraParams;



class CSWDomeCameraSetting : public CSWParameter<DomeCameraParams>
{
	CLASSINFO(CSWDomeCameraSetting, CSWObject)
public:
	

	CSWDomeCameraSetting();
	virtual ~CSWDomeCameraSetting();
	virtual HRESULT Initialize(CSWString& strFilePath);


	/**
	*@brief 初始化基本参数
	*@return 成功返回S_OK,其他值表示失败
	*/
	virtual HRESULT InitBasicParam();
	

	/**
	*@brief 初始化预置位
	*@return 成功返回S_OK,其他值表示失败
	*/
	virtual HRESULT InitPreset();

	/**
	*@brief 初始化水平扫描
	*@return 成功返回S_OK,其他值表示失败
	*/
	virtual HRESULT InitHScan();

	/**
	*@brief 初始化花样扫描
	*@return 成功返回S_OK,其他值表示失败
	*/
	virtual HRESULT InitFScan();

	/**
	*@brief 初始化巡航
	*@return 成功返回S_OK,其他值表示失败
	*/
	virtual HRESULT InitCruise();

	/**
	*@brief 初始化守望
	*@return 成功返回S_OK,其他值表示失败
	*/
	virtual HRESULT InitWatchKeeping();

	/**
	*@brief 初始化定时
	*@return 成功返回S_OK,其他值表示失败
	*/
	virtual HRESULT InitTimer();

	/**
	*@brief 初始化隐私区域
	*@return 成功返回S_OK,其他值表示失败
	*/
	virtual HRESULT InitMask();

	/**
	*@brief 初始化画面冻结
	*@return 成功返回S_OK,其他值表示失败
	*/
	virtual HRESULT InitPictureFreeze();

	/**
	*@brief 初始化待机
	*@return 成功返回S_OK,其他值表示失败
	*/
	virtual HRESULT InitSleep();

public:
	/**
	*@brief 恢复默认参数
	*@return 成功返回S_OK,其他值表示失败
	*/
	HRESULT ResetDefault();


protected:

	enum{DELETE_LEVEL = 0, INNER_LEVEL = 1, PROJECT_LEVEL = 3, CUSTOM_LEVEL = 5};

	CSWString m_strFilePath;
};
#endif

