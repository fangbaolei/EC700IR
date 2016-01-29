#ifndef SWPROCESSXMLCMD_H
#define SWPROCESSXMLCMD_H

#include "SWFC.h"
#include "swpa.h"
#include "SWList.h"
#include "tinyxml.h"
#include "SWCOMTestDevice.h"

typedef DWORD WPARAM;
typedef DWORD LPARAM;

//命令响应映射宏定义
#define SW_BEGIN_COMMAND_MAP() virtual HRESULT OnCommand(DWORD dwID,WPARAM wParam,LPARAM lParam,INT *iExitCode, BOOL *fGoingToExit, BOOL *fSwitchingSystem){HRESULT hr = E_NOTIMPL;if(hr == E_NOTIMPL){switch(dwID){
#define SW_COMMAND_HANDLER(dwID, func) case dwID : hr = func(wParam,lParam,iExitCode,fGoingToExit,fSwitchingSystem); break;
#define SW_END_COMMAND_MAP()default: break;}}return hr;}

#define MAX_COMMAND_LENGTH 50

enum _DEVTYPE
{
    DEVTYPE_VENUS = 0,
    DEVTYPE_JUPITER,
    DEVTYPE_SATURN
};

enum _FPGATYPE
{
    TYPE_FAKEIMG = 0,   //假图
    TYPE_185,
    TYPE_178,
    TYPE_249
};

//XML命令ID
enum _COMMAND_ID{
    CMD_MIN = 0,

    CMD_ResetDevice,            //复位设备,切换系统
    CMD_OptResetMode,           //获取复位模式
    CMD_RestoreFactorySetting,  //恢复出厂设定
    CMD_UnlockFactorySetting,   //解锁工厂设定
    CMD_WriteSN,                //写设备序列号

    CMD_SetTime,                //设置时间
    CMD_GetTime,                //获取时间
    CMD_HDDStatus,              //SD卡状态
    CMD_GetDevType,             //获取设备类型

    CMD_SetDCAperture,          //启动自动DC光圈
    CMD_GetDCAperture,          //获取自动DC光圈
    CMD_ZoomDCIRIS,             //放大DC光圈
    CMD_ShrinkDCIRIS,           //缩小DC光圈

    CMD_SetF1IO,                //设置F1口输出
    CMD_GetF1IO,                //获取F1口输出
    CMD_SetALMIO,               //设置ALM口输出
    CMD_GetALMIO,               //获取ALM口输出
    CMD_SetEXPIO,               //设置补光灯输出
    CMD_GetEXPIO,               //获取补光灯输出
    CMD_SetTGIO,                //设置TG口
    CMD_GetTGIO,                //获取TG口设置
    CMD_SetCtrlCpl,             //滤光片控制

    CMD_GetCoilTiggerCount,     //获取抓拍计数
    CMD_ResetCoilTiggerCount,   //抓拍计数清零

    CMD_SetComTestEnable,       //启动串口测试
    CMD_SetComTestdisable,      //停止串口测试

    CMD_SetAGCEnable,           //设置AGC
    CMD_GetAGCEnable,           //获取AGC
    CMD_SetGain,                //设置增益
    CMD_GetGain,                //获取增益
    CMD_SetShutter,             //设置快门
    CMD_GetShutter,             //获取快门
    CMD_SetAWBEnable,           //设置AWB
    CMD_GetAWBEnable,           //获取AWB
    CMD_SetRGBGain,             //设置RGB
    CMD_GetRGBGain,             //获取RGB
    CMD_AutoTestCamera,         //自动化测试相机
    CMD_SetAF,                  //设置AF模式
    CMD_GetAF,                  //获取AF模式

    CMD_SwitchFPGA,             //切换FPGA
    CMD_GetFPGAMode,            //获取FPGA模式

    CMD_TuneFocus,              //手动聚焦
    CMD_GetFocus,               //当前的聚焦值
    CMD_TuneZoom,               //设置焦距
    CMD_GetZoom,                //获取焦距

    CMD_StartPan,               //开始水平运动
    CMD_StopPan,                //停止水平运动
    CMD_StartTilt,              //开始垂直运动
    CMD_StopTilt,               //停止垂直运动
    CMD_StartWiper,             //启动雨刮
    CMD_SetLEDModeEx,           //控制LED灯
    CMD_SetDefog,               //加热除雾

    CMD_MAX
};


class CSWProcessXMLCmd
{
public:
    /**
    * @brief 构造函数
    */
    CSWProcessXMLCmd();

    /**
    * @brief 初始化函数
    */
    HRESULT Initialize(BOOL fBackupMode,INT nDevType);

    /**
    * @brief 析构函数
    */
    ~CSWProcessXMLCmd();

    /**
    * @brief 设置运行系统
    */
    VOID SetBackupMode(BOOL fMode){ m_IsBackupMode = fMode;}

    /**
    * @brief 获取运行系统
    */
    BOOL GetBackupMode(){return m_IsBackupMode;}

    /**
    * @brief 返回上次处理命令字串
    */
    char* GetLastCommand(){return m_szLastXMLCmd;}

    /**
    * @brief 设置是否支持连续升级
    */
    VOID SetSupportNoBreakUpgrade(BOOL fSupport){m_fSupportNoBreakUpgrade = fSupport;}

    /**
    * @brief 设置当前平台
    */
    VOID SetDevType(INT nType){ m_dwDevType = nType;}

    /**
    * @brief 处理XML命令
    */
    HRESULT ProcessXMLCmd(const PBYTE pbBuf, const DWORD dwSize, PVOID* ppvOutBuf, DWORD* pdwOutSize,INT* iExitCode,BOOL* fGoingToExit,BOOL* fSwitchingSystem);
private:
    /*
    * @brief 生成响应XML的文件头
    * @param [out] ppXmlOutputDoc : 响应XML的文件指针
    * @retval S_OK : 成功
    * @retval E_FAIL: 失败
    * @retval E_OUTOFMEMORY: 内存不足
    */
    HRESULT GenerateReplyXMLHeader(TiXmlDocument ** ppXmlOutputDoc);

    /*
    * @brief 生成处理命令失败的XML
    * @param [in] pXmlOutputDoc : 响应XML的文件指针
    * @param [in] szCmdName : 网络命令的名字
    * @retval S_OK : 成功
    * @retval E_FAIL: 失败
    * @retval E_OUTOFMEMORY: 内存不足
    */
    HRESULT GenerateCMDProcessFailedXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName);

    /*
    * @brief 生成命令没有带数据类型(Type)属性的XML
    * @param [in] pXmlOutputDoc : 响应XML的文件指针
    * @param [in] szCmdName : 网络命令的名字
    * @retval S_OK : 成功
    * @retval E_FAIL: 失败
    * @retval E_OUTOFMEMORY: 内存不足
    */
    HRESULT GenerateNoTypeErrorXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName);

    /*
    * @brief 生成命令没有带操作类型(Class)属性的XML
    * @param [in] pXmlOutputDoc : 响应XML的文件指针
    * @param [in] szCmdName : 网络命令的名字
    * @retval S_OK : 成功
    * @retval E_FAIL: 失败
    * @retval E_OUTOFMEMORY: 内存不足
    */
    HRESULT GenerateNoClassErrorXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName);

    /*
    * @brief 生成命令没有带数据值(Value)属性的XML
    * @param [in] pXmlOutputDoc : 响应XML的文件指针
    * @param [in] szCmdName : 网络命令的名字
    * @retval S_OK : 成功
    * @retval E_FAIL: 失败
    * @retval E_OUTOFMEMORY: 内存不足
    */
    HRESULT GenerateNoValueErrorXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName);

    /*
    * @brief 生成无法正确分析接收到的XML文件的答复文件
    * @param [in] pXmlOutputDoc : 响应XML的文件指针
    * @retval S_OK : 成功
    * @retval E_FAIL: 失败
    * @retval E_OUTOFMEMORY: 内存不足
    */
    HRESULT GenerateCMDParseFailedXml(TiXmlDocument * pXmlOutputDoc);

    /*
    * @brief 生成命令的响应XML
    * @param [in] pXmlOutputDoc : 响应XML的文件指针
    * @param [in] szCmdName : 网络命令的名字
    * @param [in] pszType : 网络命令的数据类型
    * @param [in] pvData : 网络命令的数据缓存区地址
    * @retval S_OK : 成功
    * @retval E_FAIL: 失败
    * @retval E_OUTOFMEMORY: 内存不足
    */
    HRESULT GenerateCMDReplyXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName, const CHAR * pszType, const CHAR * pszClass, const PVOID pvData);

    /*
    * @brief 生成没有找到命令的XML答复文件
    * @param [in] pXmlOutputDoc : 响应XML的文件指针
    * @param [in] szCmdName : 网络命令的名字
    * @retval S_OK : 成功
    * @retval E_FAIL: 失败
    * @retval E_OUTOFMEMORY: 内存不足
    */
    HRESULT GenerateCMDNotFoundXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName);

    /*
    * @brief 把XML打包转成字符串
    * @param [in] pXmlOutputDoc : XML的文件指针
    * @param [out] ppvXMLOutBuf : 字符串地址
    * @param [out] pdwXMLOutSize : 字符串的长度
    * @retval S_OK : 成功
    * @retval E_FAIL: 失败
    * @retval E_OUTOFMEMORY: 内存不足
    * @retval E_INVALIDARG: 参数非法
    */
    HRESULT PackXml(TiXmlDocument* pXmlOutputDoc,  PVOID* ppvXMLOutBuf,  DWORD *pdwXMLOutSize);

    /*
    * @brief 把网络命令转成消息ID发送给其它功能模块
    * @param [in] dwCMDID : 命令ID
    * @param [in] szValue : 命令数据值
    * @param [in] szType : 命令数据类型
    * @param [out] pvRetBuf : 命令返回值的缓存地址
    * @retval S_OK : 成功
    * @retval E_FAIL: 失败
    * @retval E_OUTOFMEMORY: 内存不足
    * @retval E_INVALIDARG: 参数非法
    */
    HRESULT SendCMD(const DWORD dwCMDID, const CHAR * szValue, const CHAR * szType, const CHAR* szClass, PVOID* ppvRetBuf,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);

    /*
    * @brief 调用自定义命令
    * @param [in] dwCMDID : 命令ID
    * @param [in] szValue : 命令数据值
    * @param [out] pvRetBuf : 命令返回值的缓存地址
    * @retval S_OK : 成功
    * @retval E_FAIL: 失败
    * @retval E_OUTOFMEMORY: 内存不足
    * @retval E_INVALIDARG: 参数非法
    */
    HRESULT SendCustomCMD(const DWORD dwCMDID, const CHAR * szValue, PVOID* ppvRetBuf,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);

    BOOL IsVenus(){return m_dwDevType==DEVTYPE_VENUS;}

    BOOL IsJupiter(){return m_dwDevType==DEVTYPE_JUPITER;}

    BOOL IsSaturn(){return m_dwDevType==DEVTYPE_SATURN;}

    HRESULT GetVersion(const CHAR* szTag,CHAR* pszVersion);

    HRESULT ipnc_cient_control(int linkId, int cmd, void *pPrm, int prmSize, int timeout);

    HRESULT InitDomeCamera();
private:
    /*
     * @brief 写设备序列号
     * @param [in] szSN:新设备序列号
     * @retval S_OK:成功
     * @retval E_FAIL:失败
     */
    HRESULT OnWriteSN(const CHAR* szSN);

    /*
     * @brief 复位设备，切换系统
     */
    HRESULT OnResetDevice(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取设备当前模式
     */
    HRESULT OnOptResetMode(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 锁串口,恢复出厂设置
     */
    HRESULT OnRestoreFactorySetting(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 解锁串口，恢复设置
     */
    HRESULT OnUnlockFactorySetting(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 设置系统时间
     */
    HRESULT OnSetTime(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取系统时间
     */
    HRESULT OnGetTime(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取SD卡状态
     */
    HRESULT OnHDDStatus(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取SD卡状态
     */
    HRESULT OnGetDevType(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 使能自动DC
     */
    HRESULT OnSetDCAperture(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取自动DC状态
     */
    HRESULT OnGetDCAperture(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 放大DC光圈
     */
    HRESULT OnZoomDCIRIS(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 缩小DC光圈
     */
    HRESULT OnShrinkDCIRIS(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 设置F1口输出
     */
    HRESULT OnSetF1IO(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取F1口输出
     */
    HRESULT OnGetF1IO(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 设置ALM口输出
     */
    HRESULT OnSetALMIO(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取ALM口输出
     */
    HRESULT OnGetALMIO(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 设置EXP口输出
     */
    HRESULT OnSetEXPIO(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取EXP口输出
     */
    HRESULT OnGetEXPIO(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 设置TG口输入
     */
    HRESULT OnSetTGIO(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取TG口输入
     */
    HRESULT OnGetTGIO(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 滤光片控制
     */
    HRESULT OnSetCtrlCpl(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 启动串口测试
     */
    HRESULT OnSetComTestEnable(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 停止串口测试
     */
    HRESULT OnSetComTestdisable(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取抓拍计数
     */
    HRESULT OnGetCoilTiggerCount(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 抓拍计数清零
     */
    HRESULT OnResetCoilTiggerCount(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 设置AGC
     */
    HRESULT OnSetAGCEnable(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取AGC
     */
    HRESULT OnGetAGCEnable(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 手动设置增益
     */
    HRESULT OnSetGain(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取手动增益
     */
    HRESULT OnGetGain(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 手动设置快门
     */
    HRESULT OnSetShutter(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取手动快门
     */
    HRESULT OnGetShutter(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 设置AWB
     */
    HRESULT OnSetAWBEnable(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取AWB设置
     */
    HRESULT OnGetAWBEnable(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 设置RGB
     */
    HRESULT OnSetRGBGain(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取RGB
     */
    HRESULT OnGetRGBGain(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 自动测试相机
     */
    HRESULT OnAutoTestCamera(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);

    /*
     * @brief 切换FPGA
     */
    HRESULT OnSwitchFPGA(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取FPGA模式
     */
    HRESULT OnGetFPGAMode(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 聚焦模式
     */
    HRESULT OnSetAF(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取聚焦模式
     */
    HRESULT OnGetAF(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 手动聚焦
     */
    HRESULT OnTuneFocus(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取聚焦值
     */
    HRESULT OnGetFocus(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 设置焦距
     */
    HRESULT OnTuneZoom(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 获取焦距
     */
    HRESULT OnGetZoom(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 启动球机水平转动
     */
    HRESULT OnStartPan(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 停止球机水平转动
     */
    HRESULT OnStopPan(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 启动球机垂直转动
     */
    HRESULT OnStartTilt(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 停止球机垂直转动
     */
    HRESULT OnStopTilt(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 启动球机雨刮
     */
    HRESULT OnStartWiper(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 设置球机LED灯
     */
    HRESULT OnSetLEDModeEx(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);
    /*
     * @brief 启动除雾功能
     */
    HRESULT OnSetDefog(WPARAM wParam,LPARAM lParam,INT* iExitCode, BOOL* fGoingToExit, BOOL* fSwitchingSystem);

    //命令映射表
    SW_BEGIN_COMMAND_MAP()
        SW_COMMAND_HANDLER(CMD_ResetDevice,OnResetDevice)
        SW_COMMAND_HANDLER(CMD_OptResetMode,OnOptResetMode)
        SW_COMMAND_HANDLER(CMD_RestoreFactorySetting,OnRestoreFactorySetting)
        SW_COMMAND_HANDLER(CMD_UnlockFactorySetting,OnUnlockFactorySetting)
        SW_COMMAND_HANDLER(CMD_SetTime,OnSetTime)
        SW_COMMAND_HANDLER(CMD_GetTime,OnGetTime)
        SW_COMMAND_HANDLER(CMD_HDDStatus,OnHDDStatus)
        SW_COMMAND_HANDLER(CMD_GetDevType,OnGetDevType)


        SW_COMMAND_HANDLER(CMD_SetDCAperture,OnSetDCAperture)
        SW_COMMAND_HANDLER(CMD_GetDCAperture,OnGetDCAperture)
        SW_COMMAND_HANDLER(CMD_ZoomDCIRIS,OnZoomDCIRIS)
        SW_COMMAND_HANDLER(CMD_ShrinkDCIRIS,OnShrinkDCIRIS)

        SW_COMMAND_HANDLER(CMD_SetF1IO,OnSetF1IO)
        SW_COMMAND_HANDLER(CMD_GetF1IO,OnGetF1IO)
        SW_COMMAND_HANDLER(CMD_SetALMIO,OnSetALMIO)
        SW_COMMAND_HANDLER(CMD_GetALMIO,OnGetALMIO)
        SW_COMMAND_HANDLER(CMD_SetEXPIO,OnSetEXPIO)
        SW_COMMAND_HANDLER(CMD_GetEXPIO,OnGetEXPIO)
        SW_COMMAND_HANDLER(CMD_SetTGIO,OnSetTGIO)
        SW_COMMAND_HANDLER(CMD_GetTGIO,OnGetTGIO)
        SW_COMMAND_HANDLER(CMD_SetCtrlCpl,OnSetCtrlCpl)

        SW_COMMAND_HANDLER(CMD_GetCoilTiggerCount,OnGetCoilTiggerCount)
        SW_COMMAND_HANDLER(CMD_ResetCoilTiggerCount,OnResetCoilTiggerCount)

        SW_COMMAND_HANDLER(CMD_SetComTestEnable,OnSetComTestEnable)
        SW_COMMAND_HANDLER(CMD_SetComTestdisable,OnSetComTestdisable)

        SW_COMMAND_HANDLER(CMD_SetAGCEnable,OnSetAGCEnable)
        SW_COMMAND_HANDLER(CMD_GetAGCEnable,OnGetAGCEnable)
        SW_COMMAND_HANDLER(CMD_SetGain,OnSetGain)
        SW_COMMAND_HANDLER(CMD_GetGain,OnGetGain)
        SW_COMMAND_HANDLER(CMD_SetShutter,OnSetShutter)
        SW_COMMAND_HANDLER(CMD_GetShutter,OnGetShutter)
        SW_COMMAND_HANDLER(CMD_SetAWBEnable,OnSetAWBEnable)
        SW_COMMAND_HANDLER(CMD_GetAWBEnable,OnGetAWBEnable)
        SW_COMMAND_HANDLER(CMD_SetRGBGain,OnSetRGBGain)
        SW_COMMAND_HANDLER(CMD_GetRGBGain,OnGetRGBGain)
        SW_COMMAND_HANDLER(CMD_AutoTestCamera,OnAutoTestCamera)

        SW_COMMAND_HANDLER(CMD_SwitchFPGA,OnSwitchFPGA)
        SW_COMMAND_HANDLER(CMD_GetFPGAMode,OnGetFPGAMode)

        SW_COMMAND_HANDLER(CMD_SetAF,OnSetAF)
        SW_COMMAND_HANDLER(CMD_GetAF,OnGetAF)

        SW_COMMAND_HANDLER(CMD_TuneFocus,OnTuneFocus)
        SW_COMMAND_HANDLER(CMD_GetFocus,OnGetFocus)
        SW_COMMAND_HANDLER(CMD_TuneZoom,OnTuneZoom)
        SW_COMMAND_HANDLER(CMD_GetZoom,OnGetZoom)

        SW_COMMAND_HANDLER(CMD_StartPan,OnStartPan)
        SW_COMMAND_HANDLER(CMD_StopPan,OnStopPan)
        SW_COMMAND_HANDLER(CMD_StartTilt,OnStartTilt)
        SW_COMMAND_HANDLER(CMD_StopTilt,OnStopPan)
        SW_COMMAND_HANDLER(CMD_StartWiper,OnStartWiper)
        SW_COMMAND_HANDLER(CMD_SetLEDModeEx,OnSetLEDModeEx)
        SW_COMMAND_HANDLER(CMD_SetDefog,OnSetDefog)
    SW_END_COMMAND_MAP()

private:
    //是否备份系统
    BOOL m_IsBackupMode;
    //支持不复位设备连续升级
    BOOL m_fSupportNoBreakUpgrade;
    //最后执行命令
    CHAR m_szLastXMLCmd[MAX_COMMAND_LENGTH];
    //是否支持命令
    BOOL m_fSupportCommand;
    //设备类型
    INT m_dwDevType;
    //串口测试设备
    CSWCOMTestDevice* m_pComTestDevic;
    //IPC_Client通信SOCKET
    CSWTCPSocket m_cLocalTcpSock;
    //连接字符串
    CHAR m_szXMLCommandSockFile[32];
};

#endif // SWPROCESSXMLCMD_H
