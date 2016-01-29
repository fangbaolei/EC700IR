/**
* @file		LoadParam.h
* @version	1.0
* @brief	载入各模块参数的API
*/

#ifndef _LOADPARAM_H_
#define _LOADPARAM_H_

#include "HvParamInc.h"

/**
* @brief 载入功能模块总参数
* @return 成功：S_OK，失败：E_FAIL
*/
HRESULT LoadModuleParam(ModuleParams& cModuleParams);

/**
* @brief 载入“前端图像采集”模块的参数
* @return 0
*/
int LoadImageFrameParam(
    CParamStore* pcParamStore,
    IMG_FRAME_PARAM& cImgFrameParam,
    CAM_CFG_PARAM& cCamCfgParam
);

/**
* @brief 载入“结果发送”模块的参数
* @return 0
*/
int LoadResultSenderParam(
    CParamStore* pcParamStore,
    ResultSenderParam& cResultSenderParam
);

/**
* @brief 载入“Tcpip”模块的参数,设置网口1地址
* @return 0
*/
int LoadTcpipParam_1(
    CParamStore* pcParamStore,
    TcpipParam& cTcpipCfgParam
);

/**
* @brief 载入“Tcpip”模块的参数,设置网口2地址
* @return 0
*/
int LoadTcpipParam_2(
    CParamStore* pcParamStore,
    TcpipParam& cTcpipCfgParam
);

/**
* @brief 载入Tracker模块的参数
* @return 0
*/
int LoadTrackerCfgParam(
    CParamStore* pcParamStore,
    TRACKER_CFG_PARAM& cTrackerCfgParam
);

/**
* @brief 载入工作模式参数
* @return 0
*/
int LoadWorkModeParam(
    CParamStore* pcParamStore,
    ModuleParams& cModuleParams
);

/**
* @brief 恢复默认参数，不包括IP参数
* @return 0
*/
int LoadDefaultParam(
    CParamStore* pcParamStore,
    TcpipParam& cTcpipCfgParam1,
    TcpipParam& cTcpipCfgParam2
);

/**
* @brief 获取分型中可用模式个数
* @return 可用模式个数
*/
int GetRecogParamCountPart();

/**
* @brief 通过模式名获取分型中模式索引
* @return 分型中模式索引
*/
int GetRecogParamIndexOnNamePart(char *pszModeName);

/**
* @brief 通过分型中模式索引获取模式名
* @return S_OK 成功
*         E_FAIL 索引超出范围
*         E_POINTER 指针错误
*/
HRESULT GetRecogParamNameOnIndexPart(DWORD32 dwIndex, char *pszModeName);

/**
* @brief 设置模式
* @return S_OK 成功
*         E_FAIL 失败
*/
HRESULT SetWorkModePart(DWORD32 dwWorkMode);

/**
* @brief 加载版本信息
* @return 0
*/
int LoadBuildNoParam();
#endif
