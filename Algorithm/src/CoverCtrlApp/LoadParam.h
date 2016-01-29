/**
* @file		LoadParam.h
* @version	1.0
* @brief	载入各模块参数的API
*/

#ifndef _LOADPARAM_H_
#define _LOADPARAM_H_

#include "ResultSenderImpl.h"
#include "HvParamStore.h"
#include "eprfilestorage.h"
#include "TcpipCfg.h"

/**< 参数权限等级 */
enum ParamRank
{
    INNER_LEVEL = 1,    /**< 内部研发级 */
    PROJECT_LEVEL = 3,  /**< 工程级 */
    CUSTOM_LEVEL = 5    /**< 用户自定义级 */
};

typedef struct  _DevParam
{
    int        iFilterTime;     //过滤一段时间内的相同结果(单位:秒)
    int        iDevCount;       //设备数量
    char       szDevIP1[32];    //第一台设备IP
    char       szDevIP2[32];    //...
    char       szDevIP3[32];
    char       szDevIP4[32];
    char       szDevIP5[32];
    char       szDevIP6[32];
    char       szDevIP7[32];
    char       szDevIP8[32];

    _DevParam()
    {
        memset(this, 0, sizeof(*this));
    }
}DevParam;

/**< 功能模块总参数 */
typedef struct _ModuleParams
{
    int nWorkModeIndex;
    ResultSenderParam cResultSenderParam;
    TcpipParam cTcpipCfgParam_1;
    TcpipParam cTcpipCfgParam_2;
    DevParam    cDevParam;

    _ModuleParams()
    {
        nWorkModeIndex = 0;
    }
} ModuleParams;

/**
* @brief 载入功能模块总参数
* @return 成功：S_OK，失败：E_FAIL
*/
HRESULT LoadModuleParam(ModuleParams& cModuleParams);

/**
* @brief 载入工作模式参数
* @return 0
*/
int LoadWorkModeParam(
    CParamStore* pcParamStore,
    ModuleParams& cModuleParams
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
* @brief 载入接收设备模块的参数
* @return 0
*/
int LoadDevParam(
    CParamStore* pcParamStore,
    DevParam& cDevParam
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
* @brief 恢复默认参数，不包括IP参数
* @return 0
*/
int LoadDefaultParam(
    CParamStore* pcParamStore,
    TcpipParam& cTcpipCfgParam1,
    TcpipParam& cTcpipCfgParam2
);
#endif
