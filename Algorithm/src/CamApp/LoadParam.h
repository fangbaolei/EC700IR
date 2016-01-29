/**
* @file		LoadParam.h
* @version	1.0
* @brief	载入各模块参数的API
*/

#ifndef _LOADPARAM_H_
#define _LOADPARAM_H_

#include "HvParamStore.h"
#include "eprfilestorage.h"
#include "ResultSenderImpl.h"
#include "TcpipCfg.h"
#include "CameraController.h"
#include "FrontController.h"

/**< 参数权限等级 */
enum ParamRank
{
    INNER_LEVEL = 1,    /**< 内部研发级 */
    PROJECT_LEVEL = 3,  /**< 工程级 */
    CUSTOM_LEVEL = 5    /**< 用户自定义级 */
};

/**< 功能模块总参数 */
typedef struct _ModuleParams
{
    TcpipParam cTcpipCfgParam;
    CamAppParam cCamAppParam;
    DeviceInfoParam cDeviceInfoParam;
    FrontPannelParam cFrontPannelParam;
} ModuleParams;

// 全局模块参数结构体，用于存放实时参数信息。
extern ModuleParams g_cModuleParams;

/**
* @brief 载入功能模块总参数
* @return 成功：S_OK，失败：E_FAIL
*/
HRESULT LoadModuleParam(ModuleParams& cModuleParams);

/**
* @brief 载入“Tcpip”模块的参数,设置网口1地址
* @return 0
*/
int LoadTcpipParam(
    CParamStore* pcParamStore,
    TcpipParam& cTcpipCfgParam
);

/**
* @brief 载入“相机平台”的参数
* @return 0
*/
int LoadCamAppParam(
    CParamStore* pcParamStore,
    CamAppParam& cCamAppParam
);

/**
* @brief 载入“设备信息”的参数
* @return 0
*/
int LoadDeviceInfoParam(
    CParamStore* pcParamStore,
    DeviceInfoParam& cDeviceInfoParam
);

/**
* @brief 载入“控制板”的参数
* @return 0
*/
int LoadFrontPannelParam(
    CParamStore* pcParamStore,
    FrontPannelParam& cFrontPannelParam
);

//-----------------------------------------------------------

// 保存AGC使能情况值
int SaveAGCEnable(
    int iAGCEnable,
    BOOL fSaveNow=FALSE
);

// 保存AGC测光基准值
int SaveAGCTh(
    int iAGCTh,
    BOOL fSaveNow=FALSE
);

// 保存增益值
int SaveGain(
    int iGain,
    BOOL fSaveNow=FALSE
);

// 保存快门值
int SaveShutter(
    int iShutter,
    BOOL fSaveNow=FALSE
);

// 保存Gamma
int SaveGamma(
    int rgiDataXY[8][2],
    BOOL fSaveNow = FALSE
);

// 保存模块参数
HRESULT SaveModuleParam(ModuleParams& cModuleParams);

// 保存控制板补光脉宽范围
int SaveControllPannelPulseWidthRange(
    int iPulseWidthMin,
    int iPulseWidthMax,
    BOOL fSaveNow=FALSE
);

// 保存控制板抓拍信号设置
int SaveControllPannelFlashConfig(
    int iChannel,
    int iPolarity,
    int iTriggerType,
    int iPulseWidth,
    int iCoupling,
    BOOL fSaveNow=FALSE
);

// 保存控制板自动运行模式
int SaveControllPannelAutoRunStatus(
    int iStatus,
    BOOL fSaveNow=FALSE
);

#endif
