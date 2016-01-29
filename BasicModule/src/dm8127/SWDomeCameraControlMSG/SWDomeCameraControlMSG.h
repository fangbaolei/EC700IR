// 该文件编码格式必须是WIN936

#ifndef _SWDOMECAMERACONTROLMSG_H_
#define _SWDOMECAMERACONTROLMSG_H_
#include "SWObject.h"
#include "SWMessage.h"
#include "SWDomeCameraPreset.h"
#include "SWDomeCameraHScan.h"
#include "SWDomeCameraFScan.h"
#include "SWDomeCameraCruise.h"
#include "SWDomeCameraTimer.h"
#include "SWDomeCameraMask.h"
#include "SWDomeCameraPictureFreeze.h"
#include "SWDomeCameraWatchkeeping.h"
#include "SWDomeCameraTimer.h"
#include "SWDomeCameraStandby.h"



class CSWDomeCameraControlMSG : public CSWMessage, public CSWObject
{
    CLASSINFO(CSWDomeCameraControlMSG, CSWObject)

public:
    
    CSWDomeCameraControlMSG();
    virtual ~CSWDomeCameraControlMSG();
    
protected:
    /**
     * @brief MSG_INITIAL_DOMECAMERA 设置设置相机初始化参数结构体
     * @param [in] wParam (CAMERAPARAM_INFO* 结构指针类型）
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnInitialDomeCamera( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [out] lParam
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnResetDomePosition( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  设置LED灯模式
     * @param [in] wParam 0：关，1：开，3：自动
     * @param [out] lParam
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetLedMode( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  读取LED灯模式
     * @param [in] wParam 保留
     * @param [out] lParam 0：关，1：开，3：自动
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetLedMode( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam  BIT 0~7:远焦  8~15:中  16~23:近
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
	HRESULT OnSetLedPower(WPARAM wParam,LPARAM lParam);

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
	HRESULT OnGetLedPower(WPARAM wParam,LPARAM lParam);
	    
    /**
     * @brief  设置水平角度 0~3599. 单位0.1度
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetPanCoordinate( WPARAM wParam,LPARAM lParam );
    /**
     * @brief  读取水平角度 0~3599. 单位0.1度
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetPanCoordinate( WPARAM wParam,LPARAM lParam );
    
    /**
     * @brief 设置垂直角度 0~900. 单位0.1度
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetTiltCoordinate( WPARAM wParam,LPARAM lParam );
    /**
     * @brief  读取垂直角度 0~900. 单位0.1度
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetTiltCoordinate( WPARAM wParam,LPARAM lParam );

	/**
     * @brief 设置水平和垂直角度 
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
	HRESULT OnSetPanTiltCoordinate( WPARAM wParam,LPARAM lParam );

	/**
     * @brief 获取水平和垂直角度 
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
	HRESULT OnGetPanTiltCoordinate( WPARAM wParam,LPARAM lParam );
    
    /**
     * @brief  设置焦距
     * @param [in] wParam  0~31424
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetZoom( WPARAM wParam,LPARAM lParam );

    /**
     * @brief 读取焦距
     * @param [in] wParam 保留
     * @param [out] lParam 0~31424
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetZoom( WPARAM wParam,LPARAM lParam );

    /**
     * @brief 水平垂直转动
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStartPanTilt( WPARAM wParam,LPARAM lParam );
    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStopPanTilt( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  设置水平转动速度  0 ~ MAX_SPEED
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetPanSpeed( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetPanSpeed( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  设置最大水平转动速度  0 ~ MAX_SPEED
     * @param [in] wParam
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetMaxPanSpeed( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetMaxPanSpeed( WPARAM wParam,LPARAM lParam );


	/**
     * @brief  设置垂直转动速度  0 ~ MAX_SPEED
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetTiltSpeed( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetTiltSpeed( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  设置最大垂直转动速度  0 ~ MAX_SPEED
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetMaxTiltSpeed( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetMaxTiltSpeed( WPARAM wParam,LPARAM lParam );

	/**
     * @brief 设置聚集值
     * @param [in] wParam 0x1000 - 0xc000
     *              0x1000: Over Inf
	 *				0x2000: 20 m
					0x3000: 10 m
					0x4000: 6 m
					0x5000: 4.2 m
					0x6000: 3.1 m
					0x7000: 2.5 m
					0x8000: 2.0 m
					0x9000: 1.65 m
					0xA000: 1.4 m
					0xB000: 1.2 m
					0xC000: 0.8 m
					0xD000: 30 cm
					(initial setting)
					0xE000: 11 cm
					0xF000: 1 cm
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetFocus( WPARAM wParam,LPARAM lParam );

    /**
     * @brief 读取聚集值
     * @param [in] wParam 保留
     * @param [out] lParam 0x1000 - 0xd000
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetFocus( WPARAM wParam,LPARAM lParam );

	/**
     * @brief 设置聚焦模式
     * @param [in] wParam 0:近焦，1：远焦，3：无穷远焦
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetFocusMode( WPARAM wParam,LPARAM lParam );

    /**
     * @brief 读取聚焦模式
     * @param [in] wParam 保留
     * @param [out] lParam 0:近焦，1：远焦，3：无穷远焦
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetFocusMode( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  自动聚焦使能
     * @param [in] wParam 1：手动，0：自动
     * @param [out] lParam
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetAFEnable( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  读取自动聚焦使能位
     * @param [in] wParam 保留
     * @param [out] lParam 1：手动，0：自动
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetAFEnable( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  执行一次触发聚焦
     * @param [in] wParam 无
     * @param [out] lParam 无
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnDoOneFocus( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 无
     * @param [out] lParam 无
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnMovePointToCenter( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 无
     * @param [out] lParam 无
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnMoveBlockToCenter( WPARAM wParam,LPARAM lParam );


	/**
     * @brief  
     * @param [in] wParam 0:close ,1~0xD
     * 				0D 	F1.6
					0C 	F2
					0B	F2.4
					0A	F2.8
					09	F3.4
					08	F4
					07	F4.8
					06	F5.6
					05	F6.8
					04	F8
					03	F9.6
					02	F11
					01	F14
					00	CLOSE
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetIris( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 0:close ,1~0xD
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetIris( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetIrisMode( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetIrisMode( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  调节光圈
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnTuneIris( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  调节焦距
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnTuneZoom( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  调节聚焦
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnTuneFocus( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  启动变焦
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStartZoom(WPARAM wParam,LPARAM lParam);

    /**
     * @brief  停止变焦
     * @param [in] wParam 保留
     * @param [out] lParam
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStopZoom(WPARAM wParam,LPARAM lParam);
    
    /**
     * @brief  启动水平转动
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStartPan(WPARAM wParam,LPARAM lParam);

    /**
     * @brief  停止水平转动
     * @param [in] wParam 保留
     * @param [out] lParam
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStopPan(WPARAM wParam,LPARAM lParam);

	/**
     * @brief  启动无限水平转动
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStartInfinityPan(WPARAM wParam,LPARAM lParam);

    /**
     * @brief  停止无限水平转动
     * @param [in] wParam 保留
     * @param [out] lParam
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStopInfinityPan(WPARAM wParam,LPARAM lParam);

    /**
     * @brief  垂直转动
     * @param [in] wParam 
     * @param [in] 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStartTilt(WPARAM wParam,LPARAM lParam);

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [in] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStopTilt(WPARAM wParam,LPARAM lParam);

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStartVTurnOver(WPARAM wParam,LPARAM lParam);

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [in] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStopVTurnOver(WPARAM wParam,LPARAM lParam);

	/**
     * @brief  启动雨刷
     * @param [in] wParam 
     * @param [in] 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStartWiper(WPARAM wParam,LPARAM lParam);

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [in] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStopWiper(WPARAM wParam,LPARAM lParam);

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetWiperSpeed( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetWiperSpeed( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetWiperAutoStartTime( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetWiperAutoStartTime( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetPresetPos( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetPresetPos( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnCallPresetPos( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnClearPresetPos( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetPresetNameList( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetHScanLeftPos( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetHScanLeftPos( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetHScanRightPos( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetHScanRightPos( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetHScanParam( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetHScanParam( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnCallHScan( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStopHScan( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnClearHScan( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStartFScanRecord( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStopFScanRecord( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnCallFScan( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStopFScan( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnClearFScan( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetFScanParam( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetFScanParam( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetFScanNameList( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetCruise( WPARAM wParam,LPARAM lParam );

    /**
     * @brief 
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetCruise( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnCallCruise( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStopCruise( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnClearCruise( WPARAM wParam,LPARAM lParam );


	/**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetWatchKeeping( WPARAM wParam,LPARAM lParam );


	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetWatchKeeping( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnClearWatchKeeping( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStartWatchKeeping( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnStopWatchKeeping( WPARAM wParam,LPARAM lParam );


	/**
     * @brief  设置隐私区域
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetMask( WPARAM wParam,LPARAM lParam );


	/**
     * @brief  获取隐私区域
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetMask( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  清除隐私区域
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnClearMask( WPARAM wParam,LPARAM lParam );

	/**
     * @brief 显示隐私区域的遮罩
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnShowMask( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  隐藏隐私区域的遮罩
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnHideMask( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetMaskFlag( WPARAM wParam,LPARAM lParam );

    /**
     * @brief 
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetMaskFlag( WPARAM wParam,LPARAM lParam );

	/**
     * @brief 
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetTimer( WPARAM wParam,LPARAM lParam );


	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetTimer( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnClearTimer( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetTimerFlag( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetTimerFlag( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetPictureFreeze( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetPictureFreeze( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSavePictureFreeze( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetStandbyFlag( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetStandbyFlag( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
	HRESULT OnSetIdleTimeToStandby( WPARAM wParam,LPARAM lParam );
	
    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
	HRESULT OnGetIdleTimeToStandby( WPARAM wParam,LPARAM lParam );

	
	/**
	 * @brief  
	 * @param [in] wParam 保留
	 * @param [out] lParam 
	 * @return 成功返回S_OK，其他值代表失败
	 */
	HRESULT OnStartStandbyCountDown( WPARAM wParam,LPARAM lParam );

	/**
	 * @brief  
	 * @param [in] wParam 保留
	 * @param [out] lParam 
	 * @return 成功返回S_OK，其他值代表失败
	 */
	HRESULT OnGetStandbyStatus( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetXml( WPARAM wParam,LPARAM lParam );

    /**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetXml( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
	HRESULT OnPause( WPARAM wParam,LPARAM lParam );

	/**
     * @brief  
     * @param [in] wParam 保留
     * @param [out] lParam 
     * @return 成功返回S_OK，其他值代表失败
     */
	HRESULT OnResume( WPARAM wParam,LPARAM lParam );
	

	/**
     * @brief 设置AE为使能
     * @param [in] wParam 0: 自动 0x3:手动，0xa：快门优先，0xb：光圈优先
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetAEMode( WPARAM wParam,LPARAM lParam );

    /**
     * @brief 读取AE
     * @param [in] wParam 保留
     * @param [out] lParam 0: 自动 0x3:手动，0xa：快门优先，0xb：光圈优先
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetAEMode( WPARAM wParam,LPARAM lParam );

	/**
     * @brief 设置快门等级
     * @param [in] wParam 单位：秒
     * 				级别         时间（秒）
     * 				15		1/10000
					14		1/6000
					13		1/3500
					12		1/2500
					11		1/1750
					10		1/1250
					0F		1/1000
					0E		1/600
					0D		1/425
					0C		1/300
					0B		1/215
					0A		1/150
					09		1/120
					08		1/100
					07		1/75
					06		1/50
					05		1/25
					04		1/12
					03		1/6
					02		1/3
					01		1/2
					00		1/1
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetShutter( WPARAM wParam,LPARAM lParam );

    /**
     * @brief 读取快门等级
     * @param [in] wParam 保留
     * @param [out] lParam 单位：微秒
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetShutter( WPARAM wParam,LPARAM lParam );
	/**
     * @brief 设置增益等级
     * @param [in] wParam  0 ~ 0xf
     * 				等级		DB
     * 				0F		+28 dB
					0E		+26 dB
					0D		+24 dB
					0C		+22 dB
					0B		+20 dB
					0A		+18 dB
					09		+16 dB
					08		+14 dB
					07		+12 dB
					06		+10 dB
					05		+8 dB
					04		+6 dB
					03		+4 dB
					02		+2 dB
					01		0 dB
					00		-3 dB
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetAGCGain( WPARAM wParam,LPARAM lParam );

    /**
     * @brief 读取增益
     * @param [in] wParam 保留
     * @param [out] lParam
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetAGCGain( WPARAM wParam,LPARAM lParam );

	/**
     * @brief 设置AWB模式
     * @param [in] wParam 0~8，0为自动
     * 			等级		说明
     * 			0		自动
     * 			1		ATW(Auto Tracing White balance)
     * 			2		室内
     * 			3		室外
     * 			4		触发一次调节(用白板调时可用到，断电后调节的值才会丢失)
     * 			5		手动
     * 			6		自动户外
     * 			7		自动纳灯
     * 			8		纳灯
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetAWBMode( WPARAM wParam,LPARAM lParam );

    /**
     * @brief 读取AWB模式
     * @param [in] wParam 保留
     * @param [out] lParam 0~8，0为自动
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetAWBMode( WPARAM wParam,LPARAM lParam );

    /**
     * @brief MSG_SET_RGBGAIN 设置RGB增益
     * @param [in] wParam DWORD rgb[3]
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetRGBGain(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_RGBGAIN 消息接收函数
     * @param [in] wParam 保留
     * @param [out] lParam DWORD rgb[3];
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetRGBGain(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_SATURATIONTHRESHOLD,设置饱和度
     * @param [in] wParam 饱和度0x0 -0x0e  0:60% ~ E:200%
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetSaturationThreshold(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_SATURATIONTHRESHOLD,获取饱和度
     * @param [in] wParam 保留
     * @param [in] lParam 饱和度0x0 -0x0e
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetSaturationThreshold(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_SET_SHARPENTHRESHOLD,设置锐化阀值
     * @param [in] wParam 锐化阀值[0~15]
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetSharpeThreshold(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_SHARPENTHRESHOLD,读取锐化阀值
     * @param [in] wParam  保留
     * @param [in] lParam 锐化阀值[0~15]
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetSharpeThreshold(WPARAM wParam,LPARAM lParam);

	/**
     * @brief MSG_SET_SHARPENTHRESHOLD,设置边缘增强
     * @param [in] wParam 锐化阀值[0~15]
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetEdgeEnhance(WPARAM wParam,LPARAM lParam);

    /**
     * @brief MSG_GET_SHARPENTHRESHOLD,读取边缘增强
     * @param [in] wParam  保留
     * @param [in] lParam 锐化阀值[0~15]
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetEdgeEnhance(WPARAM wParam,LPARAM lParam);

    /**
     * @brief 设置GAMMA模式
     * @param [in] wParam ：0:	Standard,	1	to	4
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetGammaMode(WPARAM wParam,LPARAM lParam);

    /**
     * @brief 读取GAMMA模式
     * @param [in] wParam  保留
     * @param [in] lParam ：0:	Standard,	1	to	4
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetGammaMode(WPARAM wParam,LPARAM lParam);

    /**
     * @brief 设置WDR
     * @param [in] wParam ：1:开,	0：关
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetWDRStrength(WPARAM wParam,LPARAM lParam);

    /**
     * @brief 读取WDR
     * @param [in] wParam  保留
     * @param [in] lParam ：1:开,	0：关
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetWDRStrength(WPARAM wParam,LPARAM lParam);

    /**
     * @brief 设置降噪等级
     * @param [in] wParam ：0:	OFF,	level	1	to	5
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetNRLevel(WPARAM wParam,LPARAM lParam);

    /**
     * @brief 读取降噪等级
     * @param [in] wParam  保留
     * @param [in] lParam ：0:	OFF,	level	1	to	5
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetNRLevel(WPARAM wParam,LPARAM lParam);

    /**
     * @brief 恢复参数默认
     * @param [in] wParam  保留
     * @param [in] lParam  保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnResetDefaultDomeCamera(WPARAM wParam,LPARAM lParam);

	
	/**
	 * @brief 恢复出厂设置
	 * @param [in] wParam  保留
	 * @param [in] lParam  保留
	 * @return 成功返回S_OK，其他值代表失败
	 */
	HRESULT OnRestoreFactorySetting(WPARAM wParam,LPARAM lParam);

	/**
     * @brief 获取当前进行的任务
     * @param [in] wParam  保留
     * @param [in] lParam  保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetCurTask(WPARAM wParam,LPARAM lParam);

	/**
     * @brief 更新动作信息到球机控制模块
     * @param [in] wParam  保留
     * @param [in] lParam  保留
     * @return 成功返回S_OK，其他值代表失败
     */
	HRESULT OnTouchDomeCamera(WPARAM wParam,LPARAM lParam);

	/**
     * @brief 设置除雾
     * @param [in] wParam  保留
     * @param [in] lParam  保留
     * @return 成功返回S_OK，其他值代表失败
     */
	HRESULT OnSetDefog(WPARAM wParam,LPARAM lParam);

	/**
     * @brief 设置曝光补偿
     * @param [in] wParam ：补偿值: 0x0 ~ 0xE
     *      0E | +7 | +10.5 dB
     *      0D | +6 | +9 dB
     *      0C | +5 | +7.5 dB
     *      0B | +4 | +6 dB
     *      0A | +3 | +4.5 dB
     *      09 | +2 | +3 dB
     *      08 | +1 | +1.5 dB
     *      07 |  0  |  0 dB
     *      06 | -1 | -1.5 dB
     *      05 | -2 | -3 dB
     *      04 | -3 | -4.5 dB
     *      03 | -4 | -6 dB
     *      02 | -5 | -7.5 dB
     *      01 | -6 | -9 dB
     *      00 | -7 | -10.5 dB
     * @param [in] lParam 保留
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnSetExpoCompValue(WPARAM wParam,LPARAM lParam);

    /**
     * @brief 读取曝光补偿
     * @param [in] wParam  保留
     * @param [out] lParam ：补偿值: 0x0 ~ 0xE
     * @return 成功返回S_OK，其他值代表失败
     */
    HRESULT OnGetExpoCompValue(WPARAM wParam,LPARAM lParam);

	/**
     * @brief 执行相机机芯命令
     * @param [in] wParam  : 命令字节
     * @param [in] lParam ：命令字节数
     * @return 成功返回S_OK，其他值代表失败
     */
	HRESULT OnCameraDoCmd(WPARAM wParam,LPARAM lParam);


	/**
	 @brief 获取球机固件版本
	 @param [in] wParam : 保留
	 @param [out] lParam : bit 0-7 低版本号 8-15高版本号
	 @return 成功返回S_OK，其他值代表失败
	 */
	HRESULT OnGetDomeVersion(WPARAM wParam, LPARAM lParam);

	/**
	 @brief 获取机芯版本, lParam须大于6个字节
	 @param [in] wParam : 保留
	 @param [out] lParam : byte0-1:VenderID byte2-3:ModelID byte4-5:ROM version
	 @return 成功返回S_OK，其他值代表失败
	 */
	HRESULT OnGetCameraVersion(WPARAM wParam, LPARAM lParam);

	/**
	 @brief 获取机芯型号
	 @param [in] wParam : 保留
	 @param [out] lParam : 型号枚举值
	 @return 成功返回S_OK，其他值代表失败
	 */
	HRESULT OnGetCameraModel(WPARAM wParam, LPARAM lParam);


     //消息映射宏定义
    SW_BEGIN_MESSAGE_MAP(CSWDomeCameraControlMSG, CSWMessage)
        //初始化模块
        SW_MESSAGE_HANDLER(MSG_INITIAL_DOMECAMERA, OnInitialDomeCamera)
        //恢复参数默认
        SW_MESSAGE_HANDLER(MSG_RESETDEFAULT_DOMECAMERA, OnResetDefaultDomeCamera)
        //恢复出厂设置
        SW_MESSAGE_HANDLER(MSG_RESTOREFACTORY_DOMECAMERA, OnRestoreFactorySetting)
        
		//basic
        SW_MESSAGE_HANDLER(MSG_SET_PAN_COORDINATE , OnSetPanCoordinate)
        SW_MESSAGE_HANDLER(MSG_GET_PAN_COORDINATE , OnGetPanCoordinate)

        SW_MESSAGE_HANDLER(MSG_SET_TILT_COORDINATE , OnSetTiltCoordinate)
        SW_MESSAGE_HANDLER(MSG_GET_TILT_COORDINATE , OnGetTiltCoordinate)

        SW_MESSAGE_HANDLER(MSG_SET_PT_COORDINATE , OnSetPanTiltCoordinate)
        SW_MESSAGE_HANDLER(MSG_GET_PT_COORDINATE , OnGetPanTiltCoordinate)
        
        SW_MESSAGE_HANDLER(MSG_RESET_DOME_POSITION , OnResetDomePosition)

        //PTZ
		SW_MESSAGE_HANDLER(MSG_SET_ZOOM , OnSetZoom)
        SW_MESSAGE_HANDLER(MSG_GET_ZOOM , OnGetZoom)
        SW_MESSAGE_HANDLER(MSG_START_PAN , OnStartPan )
        SW_MESSAGE_HANDLER(MSG_STOP_PAN , OnStopPan )
        SW_MESSAGE_HANDLER(MSG_START_ZOOM , OnStartZoom )
        SW_MESSAGE_HANDLER(MSG_STOP_ZOOM , OnStopZoom )
        SW_MESSAGE_HANDLER(MSG_START_TILT , OnStartTilt )
        SW_MESSAGE_HANDLER(MSG_STOP_TILT , OnStopTilt )
        SW_MESSAGE_HANDLER(MSG_START_PAN_TILT , OnStartPanTilt )
        SW_MESSAGE_HANDLER(MSG_STOP_PAN_TILT , OnStopPanTilt )
        SW_MESSAGE_HANDLER(MSG_SET_PAN_SPEED , OnSetPanSpeed )
        SW_MESSAGE_HANDLER(MSG_GET_PAN_SPEED , OnGetPanSpeed )
        SW_MESSAGE_HANDLER(MSG_SET_MAX_PAN_SPEED , OnSetMaxPanSpeed )
        SW_MESSAGE_HANDLER(MSG_GET_MAX_PAN_SPEED , OnGetMaxPanSpeed )
        SW_MESSAGE_HANDLER(MSG_SET_TILT_SPEED , OnSetTiltSpeed )
	 	SW_MESSAGE_HANDLER(MSG_GET_TILT_SPEED , OnGetTiltSpeed )
        SW_MESSAGE_HANDLER(MSG_SET_MAX_TILT_SPEED , OnSetMaxTiltSpeed )
        SW_MESSAGE_HANDLER(MSG_GET_MAX_TILT_SPEED , OnGetMaxTiltSpeed )		
		SW_MESSAGE_HANDLER(MSG_START_INFINITY_PAN , OnStartInfinityPan )
		SW_MESSAGE_HANDLER(MSG_STOP_INFINITY_PAN , OnStopInfinityPan )
	 
		
        // 聚焦
        SW_MESSAGE_HANDLER(MSG_SET_FOCUS , OnSetFocus )
        SW_MESSAGE_HANDLER(MSG_GET_FOCUS , OnGetFocus )
        SW_MESSAGE_HANDLER(MSG_SET_FOCUS_MODE , OnSetFocusMode )
        SW_MESSAGE_HANDLER(MSG_GET_FOCUS_MODE , OnGetFocusMode )
        SW_MESSAGE_HANDLER(MSG_SET_AF_ENABLE , OnSetAFEnable )
        SW_MESSAGE_HANDLER(MSG_GET_AF_ENABLE , OnGetAFEnable )
        SW_MESSAGE_HANDLER(MSG_DO_ONE_FOCUS , OnDoOneFocus )
        // 光圈
        SW_MESSAGE_HANDLER(MSG_SET_IRIS , OnSetIris )
        SW_MESSAGE_HANDLER(MSG_GET_IRIS , OnGetIris )
        SW_MESSAGE_HANDLER(MSG_SET_IRIS_MODE,OnSetIrisMode)
        SW_MESSAGE_HANDLER(MSG_GET_IRIS_MODE,OnGetIrisMode)

		SW_MESSAGE_HANDLER(MSG_TUNE_IRIS,OnTuneIris)
        SW_MESSAGE_HANDLER(MSG_TUNE_ZOOM,OnTuneZoom)
        SW_MESSAGE_HANDLER(MSG_TUNE_FOCUS,OnTuneFocus)
	 
		SW_MESSAGE_HANDLER(MSG_START_VTURNOVER,OnStartVTurnOver)
        SW_MESSAGE_HANDLER(MSG_STOP_VTURNOVER,OnStopVTurnOver)

        // 雨刷
        SW_MESSAGE_HANDLER(MSG_START_WIPER, OnStartWiper)
        SW_MESSAGE_HANDLER(MSG_STOP_WIPER, OnStopWiper)
        SW_MESSAGE_HANDLER(MSG_SET_WIPER_SPEED, OnSetWiperSpeed)
        SW_MESSAGE_HANDLER(MSG_GET_WIPER_SPEED, OnGetWiperSpeed)
		SW_MESSAGE_HANDLER(MSG_SET_WIPER_AUTOSTART_TIME, OnSetWiperAutoStartTime)
        SW_MESSAGE_HANDLER(MSG_GET_WIPER_AUTOSTART_TIME, OnGetWiperAutoStartTime)

        // LED灯
        SW_MESSAGE_HANDLER(MSG_SET_LED_MODE, OnSetLedMode)
        SW_MESSAGE_HANDLER(MSG_GET_LED_MODE, OnGetLedMode)
        SW_MESSAGE_HANDLER(MSG_SET_LED_POWER, OnSetLedPower)
        SW_MESSAGE_HANDLER(MSG_GET_LED_POWER, OnGetLedPower)

		//preset
		SW_MESSAGE_HANDLER(MSG_SET_PRESET_POS, OnSetPresetPos)
        SW_MESSAGE_HANDLER(MSG_GET_PRESET_POS, OnGetPresetPos)
		SW_MESSAGE_HANDLER(MSG_CALL_PRESET_POS, OnCallPresetPos)
        SW_MESSAGE_HANDLER(MSG_CLEAR_PRESET_POS, OnClearPresetPos)
        SW_MESSAGE_HANDLER(MSG_GET_PRESET_NAME_LIST, OnGetPresetNameList)

		//hscan
        SW_MESSAGE_HANDLER(MSG_SET_HSCAN_LEFT_POS, OnSetHScanLeftPos)
        SW_MESSAGE_HANDLER(MSG_GET_HSCAN_LEFT_POS, OnGetHScanLeftPos)
        SW_MESSAGE_HANDLER(MSG_SET_HSCAN_RIGHT_POS, OnSetHScanRightPos)
        SW_MESSAGE_HANDLER(MSG_GET_HSCAN_RIGHT_POS, OnGetHScanRightPos)
        SW_MESSAGE_HANDLER(MSG_SET_HSCAN_PARAM, OnSetHScanParam)
        SW_MESSAGE_HANDLER(MSG_GET_HSCAN_PARAM, OnGetHScanParam)
        SW_MESSAGE_HANDLER(MSG_START_HSCAN, OnCallHScan)
        SW_MESSAGE_HANDLER(MSG_STOP_HSCAN, OnStopHScan)
        SW_MESSAGE_HANDLER(MSG_CLEAR_HSCAN, OnClearHScan)

		// fscan
		SW_MESSAGE_HANDLER(MSG_START_FSCAN_RECORD , OnStartFScanRecord)
        SW_MESSAGE_HANDLER(MSG_STOP_FSCAN_RECORD , OnStopFScanRecord)
        SW_MESSAGE_HANDLER(MSG_CALL_FSCAN , OnCallFScan)
        SW_MESSAGE_HANDLER(MSG_STOP_FSCAN , OnStopFScan)
        SW_MESSAGE_HANDLER(MSG_CLEAR_FSCAN , OnClearFScan)
        SW_MESSAGE_HANDLER(MSG_SET_FSCAN_PARAM , OnSetFScanParam)
        SW_MESSAGE_HANDLER(MSG_GET_FSCAN_PARAM , OnGetFScanParam)
        SW_MESSAGE_HANDLER(MSG_GET_FSCAN_NAME_LIST, OnGetFScanNameList)

	 	//cruise
        SW_MESSAGE_HANDLER(MSG_SET_CRUISE , OnSetCruise)
        SW_MESSAGE_HANDLER(MSG_GET_CRUISE , OnGetCruise)
        SW_MESSAGE_HANDLER(MSG_CALL_CRUISE , OnCallCruise)
        SW_MESSAGE_HANDLER(MSG_STOP_CRUISE , OnStopCruise)
        SW_MESSAGE_HANDLER(MSG_CLEAR_CRUISE , OnClearCruise)

        //watchkeeping
        SW_MESSAGE_HANDLER(MSG_SET_WATCHKEEPING , OnSetWatchKeeping)
        SW_MESSAGE_HANDLER(MSG_GET_WATCHKEEPING , OnGetWatchKeeping)
        SW_MESSAGE_HANDLER(MSG_CLEAR_WATCHKEEPING , OnClearWatchKeeping)
        SW_MESSAGE_HANDLER(MSG_START_WATCHKEEPING , OnStartWatchKeeping)
        SW_MESSAGE_HANDLER(MSG_STOP_WATCHKEEPING , OnStopWatchKeeping)

		//mask
        SW_MESSAGE_HANDLER(MSG_SET_MASK , OnSetMask)
        SW_MESSAGE_HANDLER(MSG_GET_MASK , OnGetMask)
        SW_MESSAGE_HANDLER(MSG_CLEAR_MASK , OnClearMask)
        SW_MESSAGE_HANDLER(MSG_SHOW_MASK , OnShowMask)//todo: msgid
        SW_MESSAGE_HANDLER(MSG_HIDE_MASK , OnHideMask)//todo: msgid


		//timer
        SW_MESSAGE_HANDLER(MSG_SET_TIMER , OnSetTimer)
        SW_MESSAGE_HANDLER(MSG_GET_TIMER , OnGetTimer)
        SW_MESSAGE_HANDLER(MSG_CLEAR_TIMER , OnClearTimer)
        SW_MESSAGE_HANDLER(MSG_SET_TIMER_FLAG , OnSetTimerFlag)
        SW_MESSAGE_HANDLER(MSG_GET_TIMER_FLAG , OnGetTimerFlag)


		//picturefreeze
        SW_MESSAGE_HANDLER(MSG_SET_FREEZEFLAG , OnSetPictureFreeze)
        SW_MESSAGE_HANDLER(MSG_GET_FREEZEFLAG , OnGetPictureFreeze)
		SW_MESSAGE_HANDLER(MSG_SAVE_FREEZEFLAG, OnSavePictureFreeze)

	 	//sleep
        SW_MESSAGE_HANDLER(MSG_SET_SLEEP_FLAG , OnSetStandbyFlag)
        SW_MESSAGE_HANDLER(MSG_GET_SLEEP_FLAG , OnGetStandbyFlag)
        SW_MESSAGE_HANDLER(MSG_SET_IDLE_TIME_TO_SLEEP, OnSetIdleTimeToStandby)
        SW_MESSAGE_HANDLER(MSG_GET_IDLE_TIME_TO_SLEEP , OnGetIdleTimeToStandby)
        SW_MESSAGE_HANDLER(MSG_START_SLEEP_COUNTDOWN, OnStartStandbyCountDown)
        SW_MESSAGE_HANDLER(MSG_GET_SLEEP_STATUS, OnGetStandbyStatus)

		SW_MESSAGE_HANDLER(MSG_MOVE_POINT_TO_CENTER, OnMovePointToCenter)
        SW_MESSAGE_HANDLER(MSG_MOVE_BLOCK_TO_CENTER, OnMoveBlockToCenter)	 
        

		SW_MESSAGE_HANDLER(MSG_SET_DOME_XML, OnSetXml)
        SW_MESSAGE_HANDLER(MSG_GET_DOME_XML , OnGetXml)

		SW_MESSAGE_HANDLER(MSG_PAUSE, OnPause)
        SW_MESSAGE_HANDLER(MSG_RESUME, OnResume)

		SW_MESSAGE_HANDLER(MSG_GET_CUR_TASK, OnGetCurTask)
		SW_MESSAGE_HANDLER(MSG_TOUCH_DOME_CAMERA, OnTouchDomeCamera)
		SW_MESSAGE_HANDLER(MSG_SET_DEFOG, OnSetDefog)
	 	
        // 相机参数
        // AE
        SW_MESSAGE_HANDLER(MSG_SET_AE_MODE , OnSetAEMode )
        SW_MESSAGE_HANDLER(MSG_GET_AE_MODE , OnGetAEMode )
        SW_MESSAGE_HANDLER(MSG_SET_SHUTTER , OnSetShutter )
        SW_MESSAGE_HANDLER(MSG_GET_SHUTTER , OnGetShutter)
        SW_MESSAGE_HANDLER(MSG_SET_AGCGAIN , OnSetAGCGain )
        SW_MESSAGE_HANDLER(MSG_GET_AGCGAIN , OnGetAGCGain )
        // AWB
        SW_MESSAGE_HANDLER(MSG_SET_AWB_MODE , OnSetAWBMode )
        SW_MESSAGE_HANDLER(MSG_GET_AWB_MODE , OnGetAWBMode )
        SW_MESSAGE_HANDLER(MSG_SET_RGBGAIN , OnSetRGBGain )
        SW_MESSAGE_HANDLER(MSG_GET_RGBGAIN , OnGetRGBGain )
        // 饱和度
		SW_MESSAGE_HANDLER(MSG_SET_SATURATIONTHRESHOLD, OnSetSaturationThreshold)
        SW_MESSAGE_HANDLER(MSG_GET_SATURATIONTHRESHOLD, OnGetSaturationThreshold)
        // 锐化阈值
		//SW_MESSAGE_HANDLER(MSG_SET_SHARPENTHRESHOLD,OnSetSharpeThreshold)
        //SW_MESSAGE_HANDLER(MSG_GET_SHARPENTHRESHOLD,OnGetSharpeThreshold)
        SW_MESSAGE_HANDLER(MSG_SET_SHARPENTHRESHOLD,OnSetEdgeEnhance)
        SW_MESSAGE_HANDLER(MSG_GET_SHARPENTHRESHOLD,OnGetEdgeEnhance)
        // 设置GAMMA模式  0:	Standard,	1	to	4
		SW_MESSAGE_HANDLER(MSG_SET_GAMMA_STRENGTH,OnSetGammaMode)
        SW_MESSAGE_HANDLER(MSG_GET_GAMMA_STRENGTH,OnGetGammaMode)
        // 设置WDR
        SW_MESSAGE_HANDLER(MSG_SET_WDR_STRENGTH, OnSetWDRStrength)
        SW_MESSAGE_HANDLER(MSG_GET_WDR_STRENGTH, OnGetWDRStrength)
        // 设置降噪等级  0:	OFF,	level	1	to	5
        SW_MESSAGE_HANDLER(MSG_SET_DENOISE_STATE, OnSetNRLevel)
        SW_MESSAGE_HANDLER(MSG_GET_DENOISE_STATE, OnGetNRLevel)	 

		// 曝光补偿
        SW_MESSAGE_HANDLER(MSG_SET_EXPOCOMP_VALUE, OnSetExpoCompValue)
        SW_MESSAGE_HANDLER(MSG_GET_EXPOCOMP_VALUE, OnGetExpoCompValue)

		// 获取球机固件版本
		SW_MESSAGE_HANDLER(MSG_GET_DOME_VERSION, OnGetDomeVersion)
		// 获取机芯版本信息
		SW_MESSAGE_HANDLER(MSG_GET_CAMERA_VERSION, OnGetCameraVersion)
		// 获取机芯型号
		SW_MESSAGE_HANDLER(MSG_GET_CAMERA_MODEL, OnGetCameraModel)
		
	 	SW_MESSAGE_HANDLER(MSG_CAMERA_DO_CMD, OnCameraDoCmd)

    SW_END_MESSAGE_MAP();


	/**
     *@brief 控制球机云台
     *@param [PVOID] pvBuffer PTZ控制参数
     *@param [PVOID] iSize 参数大小
     *@return 成功返回S_OK,其他值为错误代码
     */
	HRESULT OnPTZControl(PVOID pvBuffer, INT iSize);

	/**
     *@brief 获取球机云台配置
     *@param [PVOID] pvBuffer PTZ控制参数
     *@param [PVOID] iSize 参数大小
     *@return 成功返回S_OK,其他值为错误代码
     */
	HRESULT OnGetPTZInfo(PVOID pvBuffer, INT iSize);


	
	//远程消息映射函数
    SW_BEGIN_REMOTE_MESSAGE_MAP(CSWDomeCameraControlMSG, CSWMessage)
		SW_REMOTE_MESSAGE_HANDLER(MSG_APP_REMOTE_PTZ_CONTROL, OnPTZControl)
		SW_REMOTE_MESSAGE_HANDLER(MSG_APP_REMOTE_GET_PTZ_INFO, OnGetPTZInfo)
    SW_END_REMOTE_MESSAGE_MAP();
    
private:
    CSWDomeCameraSetting& GetSetting(){ return m_cSetting;};
	VOID Wait(){swpa_thread_sleep_ms(500);};

	HRESULT HandleOperation(const INT& iOperation, const INT& iArg = 0);

	HRESULT SetLastTask(const INT& iTask, const INT& iTaskID);

	HRESULT GetLastTask(INT& iTask, INT& iTaskID);

	HRESULT SwitchTask(const BOOL& fSwitchOn, const INT& iTask = -1, const INT& iArg = -1);

	static PVOID InfinityPan(PVOID pvArg);
	
	static PVOID VTurnOver(PVOID pvArg);

	static PVOID ResetDome(PVOID pvArg);
	
	static PVOID SyncZoom(PVOID pvArg);

	static PVOID Defog(PVOID pvArg);
	
protected:

	BOOL m_fInited;
	DWORD m_dwFScanRecordingID;

	CSWDomeCameraSetting m_cSetting;
	
	CSWDomeCameraPreset m_cPreset;
	CSWDomeCameraHScan m_cHScan;
	CSWDomeCameraFScan m_cFScan;
	CSWDomeCameraCruise m_cCruise;

	CSWDomeCameraPictureFreeze m_cPictureFreeze;
	CSWDomeCameraWatchKeeping m_cWatchKeeping;
	CSWDomeCameraTimer m_cTimer;
	CSWDomeCameraStandby m_cStandby;
	CSWDomeCameraMask m_cMask;

	CSWThread m_cInfinityPanThread;
	BOOL m_fInfinityPanStarted;

	CSWThread m_cDefogThread;
	BOOL m_fDefogStarted;

	
	// 有些参数需要在此模块保存。
	int m_iLedMode;	// LED 模式

	BOOL m_fVTurnOver;
	CSWThread m_cVTurnOverThread;
	CSWThread m_cResetDomeThread;

	BOOL m_fSyncZoom;
	CSWThread m_cSyncZoomThread;

	BOOL m_fRepeatPanTilt;

	CSWCamera *m_pCamera;	
public:
	// 焦距对应表。
	// 物理变焦30倍。
	static const int MAX_ZOOM_COUNT = 30;
	static ZOOM_RATIO s_cZoomTable[MAX_ZOOM_COUNT];
};
//!!!!!!!!!!!!!!!!!!!!!!!! testonly::::::::::: REGISTER_CLASS(CSWDomeCameraControlMSG)
#endif // _SWDOMECAMERACONTROLMSG_H_

