#ifndef __SW_LPR_APPLICATION_H__
#define __SW_LPR_APPLICATION_H__
#include "SWLPRParameter.h"
#include "SWMessage.h"
#include "SWBaseFilter.h"
#include "SWBaseGraph.h"

//Filter模块初始化函数
#define BEGIN_INITIALIZE_OBJECT()       virtual HRESULT OnObjectInitialize(CSWObject *pObject){if(0){}
#define ON_INITIALIZE_OBJECT(cls, func) else if(!swpa_strcmp(#cls, pObject->Name())){return func(pObject);}
#define END_INITIALIZE_OBJECT()         return S_OK;}

class CSWLPRApplication : public CSWApplication, CSWBaseGraph, CSWMessage
{
	CLASSINFO(CSWLPRApplication, CSWApplication)
public:
	CSWLPRApplication();
	virtual ~CSWLPRApplication();
	virtual HRESULT InitInstance(const WORD wArgc, const CHAR** szArgv);
	virtual HRESULT ReleaseInstance();
	virtual HRESULT Run();
  virtual HRESULT OnCommand(CSWBaseFilter* pFilter, DWORD dwID, DWORD wParam, DWORD lParam);
  CSWLPRParameter& GetParam(VOID);
  CSWObject * CreateObjectByVariable(LPCSTR szVar, LPCSTR szDefaultClassName="");
protected:
	static  INT OnMSGCompare(PVOID key1, PVOID key2);
	HRESULT InitVariant(VOID);
	HRESULT InitParameter(VOID);
	HRESULT InitObject(VOID);
	HRESULT InitFilter(VOID);



	static  VOID OnDSPAlarm(PVOID pvParam, INT iInfo);
	static  PVOID OnReleaseInstance(PVOID pvParam);

	// reset info
	HRESULT WriteResetInfo();
	INT GetResetCount();
	CSWString GetResetInfo();

	HRESULT GetRedRectInfo(SW_RECT* pRect, INT& iRectCount);
	
	HRESULT GetGreenRectInfo(SW_RECT* pRect, INT& iRectCount);

	// AT88 check
	HRESULT InitAT88();
	HRESULT VerifyCrypt();

	// 启动计数
	VOID SetBootCountInfo();

	//读取备份系统的版本号
	HRESULT ReadBackupSystemVersion(CHAR* pszVersion);

	//读取前端型号
	HRESULT ReadSensorType(INT *piSensorType);

	//保存前端型号
	HRESULT SaveSensorType(INT iSensorType);

	//检查复位条件
	HRESULT CheckRebootEvent(void);

	//读取设备别名
	HRESULT ReadCustomizedDevName(LPSTR pszName);

	//保存设备别名
	HRESULT SaveCustomizedDevName(LPCSTR pszName);

	//判断设备类型是否为金星
	BOOL IsVenus(VOID);

	//判断设备类型是否为木星
	BOOL IsJupiter(VOID);

protected:
	//Object初始化函数
	HRESULT OnCameralControlMSG(CSWObject *pObject);
	HRESULT OnCameraControl2AMSG(CSWObject *pObject);
	HRESULT OnDomeCameraControlMSG(CSWObject *pObject);
	HRESULT OnNetCommandProcessInitialize(CSWObject *pObject);
	HRESULT OnTriggerOutInitialize(CSWObject *pObject);
	//创建Filter
	CSWBaseFilter *CreateFilter(TiXmlElement *element);
	//Filter的初始化函数
	HRESULT OnH264HDDTransformFilterInitialize(CSWObject *pObject);
	HRESULT OnH264NetRenderFilterInitialize(CSWObject *pObject);
	HRESULT OnResultHDDTransformFilterInitialize(CSWObject *pObject);
	HRESULT OnResultNetRenderFilterInitialize(CSWObject *pObject);	
	HRESULT OnJPEGNetRenderFilterInitialize(CSWObject *pObject);	
	HRESULT OnNetSourceFilterInitialize(CSWObject *pObject);
	HRESULT OnH264SourceFilterInitialize(CSWObject *pObject);
	HRESULT OnCentaurusH264EncodeFilterInitialize(CSWObject *pObject);
	HRESULT OnCentaurusJpegEncodeFilterInitialize(CSWObject *pObject);
	HRESULT OnRecognizeTransformFilterInitialize(CSWObject *pObject);
    HRESULT OnRecognizeTransformTollGateFilterInitialize(CSWObject *pObject);
	HRESULT OnRecognizeTransformPTFilterInitialize(CSWObject *pObject);
	HRESULT OnRecognizeTransformEPFilterInitialize(CSWObject *pObject);
	HRESULT OnRecognizeTGTransformFilterInitialize(CSWObject *pObject);
	HRESULT OnJPEGEncodeTransformFilterInitialize(CSWObject *pObject);
	HRESULT OnVPIFSourceFilterInitialize(CSWObject *pObject);
	HRESULT OnMatchTransformFilterInitialize(CSWObject *pObject);	
	HRESULT OnAutoControlRenderFilterInitialize(CSWObject *pObject);
	HRESULT OnResultFilterInitialize(CSWObject *pObject);
	HRESULT OnEPoliceEventCheckerTransformFilter(CSWObject *pObject);
	HRESULT OnONVIFControlInitialize(CSWObject *pObject);
	HRESULT OnRTSPTransformFilterInitialize(CSWObject *pObject);
	HRESULT OnJPEGOverlayFilterInitialize(CSWObject *pObject);
	HRESULT OnH264OverlayFilterInitialize(CSWObject *pObject);	
	HRESULT OnH264SecondOverlayFilterInitialize(CSWObject *pObject);
    HRESULT OnGBH264TransformFilterInitialize(CSWObject *pObject);
    HRESULT OnH264RTPFilterInitialize(CSWObject *pObject);
    HRESULT OnJPEGCachTransformFilterInitialize(CSWObject *pObject);
    HRESULT OnCentaurusSourceFilterInitialize(CSWObject *pObject);
	HRESULT OnCSWResultFtpRenderFilterInitialize(CSWObject *pObject);
    HRESULT OnCSWDomeRockerControlMSGInitialize(CSWObject *pObject);
	HRESULT OnRecognizeTransformPPFilterInitialize(CSWObject *pObject);
    HRESULT OnCSWH264QueueRenderFilterInitialize(CSWObject *pObject);
    HRESULT OnCSWResultVideoTransformFilterInitialize(CSWObject *pObject);
	HRESULT OnMD5EncryptFilterInitialize(CSWObject *pObject);
	HRESULT OnCSWExtendDataTransformFilterInitialize(CSWObject *pObject);

	//初始化函数映射
	BEGIN_INITIALIZE_OBJECT()
		ON_INITIALIZE_OBJECT(CSWCameralControlMSG,        OnCameralControlMSG)
		ON_INITIALIZE_OBJECT(CSWCameraControl2AMSG,		  OnCameraControl2AMSG)
		ON_INITIALIZE_OBJECT(CSWDomeCameraControlMSG,     OnDomeCameraControlMSG)
		ON_INITIALIZE_OBJECT(CSWNetCommandProcess,        OnNetCommandProcessInitialize)
		ON_INITIALIZE_OBJECT(CSWTriggerOut,               OnTriggerOutInitialize)
		ON_INITIALIZE_OBJECT(CSWH264HDDTransformFilter,   OnH264HDDTransformFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWH264NetRenderFilter,   	  OnH264NetRenderFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWResultHDDTransformFilter, OnResultHDDTransformFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWResultNetRenderFilter,    OnResultNetRenderFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWJPEGNetRenderFilter,      OnJPEGNetRenderFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWNetSourceFilter,          OnNetSourceFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWH264SourceFilter,         OnH264SourceFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWCentaurusH264EncodeFilter,OnCentaurusH264EncodeFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWCentaurusJpegEncodeFilter,OnCentaurusJpegEncodeFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWRecognizeTransformFilter, OnRecognizeTransformFilterInitialize)
        ON_INITIALIZE_OBJECT(CSWRecognizeTransformTollGateFilter, OnRecognizeTransformTollGateFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWRecognizeTransformPTFilter, OnRecognizeTransformPTFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWRecognizeTransformEPFilter, OnRecognizeTransformEPFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWRecognizeTGTransformFilter, OnRecognizeTGTransformFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWJPEGEncodeTransformFilter,OnJPEGEncodeTransformFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWVPIFSourceFilter,         OnVPIFSourceFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWMatchTransformFilter,     OnMatchTransformFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWAutoControlRenderFilter,  OnAutoControlRenderFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWResultFilter,             OnResultFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWEPoliceEventCheckerTransformFilter, OnEPoliceEventCheckerTransformFilter)
		ON_INITIALIZE_OBJECT(CSWONVIFControl,             OnONVIFControlInitialize)
		ON_INITIALIZE_OBJECT(CSWRTSPTransformFilter,      OnRTSPTransformFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWJPEGOverlayFilter,         OnJPEGOverlayFilterInitialize)
        ON_INITIALIZE_OBJECT(CSWH264OverlayFilter,         OnH264OverlayFilterInitialize)
        ON_INITIALIZE_OBJECT(CSWH264SecondOverlayFilter,	OnH264SecondOverlayFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWGBH264TransformFilter,    OnGBH264TransformFilterInitialize)
        ON_INITIALIZE_OBJECT(CSWH264Filter,    OnH264RTPFilterInitialize)
        ON_INITIALIZE_OBJECT(CSWJPEGCachTransformFilter,    OnJPEGCachTransformFilterInitialize)
        ON_INITIALIZE_OBJECT(CSWCentaurusSourceFilter,    OnCentaurusSourceFilterInitialize)
        ON_INITIALIZE_OBJECT(CSWResultFtpRenderFilter,    OnCSWResultFtpRenderFilterInitialize)
        ON_INITIALIZE_OBJECT(CSWDomeRockerControlMSG,    OnCSWDomeRockerControlMSGInitialize)
		ON_INITIALIZE_OBJECT(CSWRecognizeTransformPPFilter, OnRecognizeTransformPPFilterInitialize)
		ON_INITIALIZE_OBJECT(CSWH264QueueRenderFilter,    OnCSWH264QueueRenderFilterInitialize)
        ON_INITIALIZE_OBJECT(CSWResultVideoTransformFilter,    OnCSWResultVideoTransformFilterInitialize)
        ON_INITIALIZE_OBJECT(CSWMD5EncryptFilter,    		OnMD5EncryptFilterInitialize)
        ON_INITIALIZE_OBJECT(CSWExtendDataTransformFilter,OnCSWExtendDataTransformFilterInitialize)
	END_INITIALIZE_OBJECT()
	
	//消息处理函数
	/**
	 *@brief 设置时间
	 *@param [in] wParam SWPA_DATETIME_TM*指针
	 *@param [in] lParam 无
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	HRESULT OnSetTime(WPARAM wParam, LPARAM lParam);
	/**
	 *@brief 设置时间
	 *@param [in]  wParam 无
	 *@param [out] lParam SWPA_DATETIME_TM*指针
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	HRESULT OnGetTime(WPARAM wParam, LPARAM lParam);
	/**
	 *@brief 设置IP地址
	 *@param [in] wParam 数组为3个元素的指针,array[0]=MAC地址;array[1]=IP地址;array[2]=子网掩码;array[3]=网关
	 *@param [in] lParam 无
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	HRESULT OnSetIP(WPARAM wParam, LPARAM lParam);	
	/**
	 *@brief 设置IP地址
	 *@param [in] wParam 无
	 *@param [out] lParam 数组为3个元素的指针,array[0]=MAC地址;array[1]=IP地址;array[2]=子网掩码;array[3]=网关
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	HRESULT OnGetIP(WPARAM wParam, LPARAM lParam);
	/**
	 *@brief 设置XML
	 *@param [in] wParam XML数据指针
	 *@param [in] lParam 无
	 *@return 成功返回S_OK,其他值为错误代码
	 */	
	HRESULT OnSetXML(WPARAM wParam, LPARAM lParam);
	/**
	 *@brief 获取XML
	 *@param [in]   wParam 无
	 *@param [out]  lParam CSWString*指针，保存xml数据
	 *@return 成功返回S_OK,其他值为错误代码
	 */	
	HRESULT OnGetXML(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 取得工作模式总数
	 *@param [in] wParam 无
	 *@param [out] lParam，int* 类型,保存工作模式数量
	 *@return 成功返回S_OK,其他值为错误代码
	 */
	HRESULT OnGetWorkModeCount(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 获取设备工作模式
	 *@param [out] wParam 工作模式索引指针,int* 类型
	 *@param [out] lParam 工作模式字符串，CSWString* 类型
	 *@return 成功返回S_OK,其他值为错误代码
	 */
	HRESULT OnGetWorkMode(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 复位设备
	 *@param [in] wParam 保留
	 *@param [in] lParam 保留
	 *@return 成功返回S_OK,其他值为错误代码
	 */
	HRESULT OnResetDevice(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 恢复参数默认
	 *@param [in] wParam 保留
	 *@param [in] lParam 保留
	 *@return 成功返回S_OK,其他值为错误代码
	 */
	HRESULT OnRestoreDefault(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 恢复出厂默认
	 *@param [in] wParam 保留
	 *@param [in] lParam 保留
	 *@return 成功返回S_OK,其他值为错误代码
	 */
	HRESULT OnRestoreFactory(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 设置字符叠加
	 */
	//HRESULT OnSetCharacterEnable(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 获取是否字符叠加
	 */
	//HRESULT OnGetCharacterEnable(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 获取设备名字
	 *@param [in] wParam 无
	 *@param [out] lParam,CSWString*指针，保存设备名称
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	HRESULT OnGetHvName(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 获取设备id
	 *@param [in] wParam 无
	 *@param [out] lParam, INT*指针，保存设备id
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	HRESULT OnGetHvID(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 获取产品名称
	 *@param [in] wParam 无
	 *@param [out] lParam,CSWString*指针，保存产品名称
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	HRESULT OnGetProductName(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 获得视频总数
	 *@param [in] wParam 无
	 *@param [out] lParam, INT*指针，保存视频总数
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	HRESULT OnGetVideoCount(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 取得版本
	 *@param [in] wParam 无
	 *@param [out] lParam, INT*指针，保存版本号
	 */
	HRESULT OnGetVersion(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 取得版本字符串
	 *@param [in] wParam 无
	 *@param [out] lParam, CSWString*指针，保存版本号字符串
	 */
	HRESULT OnGetVersionString(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 取得连接的IP地址
	 */
	HRESULT OnGetConnectedIP(WPARAM wParam, LPARAM lParam);
	 
	/**
	 *@brief 硬盘分区
	 *@param wParam [in] 要分区的设备字符串指针，如"/dev/sda"
	 *@param lParam [in] 需要划分分区的数量，每个分区大小一致
	 */
	HRESULT OnHDDFDisk(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 硬盘检测
	 *@param [in] wParam 检测模式,0:坏道检测,1简单检测
	 *@param [in] lParam 无
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	HRESULT OnHDDCheck(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 获取硬盘状态
	 *@param [in] wParam 无
	 *@param [out] lParam,CSWString*指针，保存硬盘状态
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	
	HRESULT OnHDDStatus(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 获得复位次数
	 *@param [in] wParam 无
	 *@param [out] lParam,INT*指针，保存复位次数
	 *@return 成功返回S_OK，其他值为错误代码
	 */	
	HRESULT OnResetCount(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 获得复位记录
	 *@param [in] wParam 保存复位记录缓冲区,char*指针
	 *@param [in] lParam 保存复位记录缓冲区大小
	 */
	HRESULT OnResetReport(WPARAM wParam, LPARAM lParam);
	/**
	 *@brief 获得复位模式
	 *@param [in] wParam 无
	 *@param [out] lParam,INT*指针
	 *@return 成功返回S_OK，其他值为错误代码
	 */	
	HRESULT OnResetMode(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 取得设备类型
	 *@param [in] wParam 无
	 *@param [out] lParam,CSWString*指针，保存设备类型
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	HRESULT OnGetDevType(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 取得CPU温度
	 *@param [in] wParam 无
	 *@param [out] lParam,INT*指针，保存CPU温度
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	HRESULT OnGetCpuTemperature(WPARAM wParam, LPARAM lParam);

	/**
	 *@brief 更新状态，每个FILTER定时为状态给到APP
	 *@param [in] wParam 状态字符串。
	 *@param [out] lParam 
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	HRESULT OnUpdateStatus(WPARAM wParam, LPARAM lParam);

	/**
	 *@brief 更新状态，每个FILTER定时为状态给到APP
	 *@param [in] wParam 无
	 *@param [out] lParam,CHAR*指针，状态字符串.
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	HRESULT OnGetRunStatus(WPARAM wParam, LPARAM lParam);

	/**
	 *@brief 雷达触发抓拍事件
	 *@param [in] wParam 线圈号
	 *@param [in] lParam 不用
	 *@return 成功返回S_OK,其他值为错误代码
	 */
	HRESULT OnRadarTrigger(WPARAM wParam, LPARAM lParam);

	/**
	 *@brief 线圈的状态
	 *@param [in] wParam 线圈的状态，每一位表明每个线圈，0表明正常，1表明异常
	 *@param [in] lParam 线圈的触发标志，每一位表明每个线圈，0表明不触发,1表明触发
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	HRESULT OnCoilStatus(WPARAM wParam, LPARAM lParam);
	 
	/**
	 *@brief 线圈触发抓拍事件
	 *@param [in] wParam 线圈号
	 *@param [in] lParam 不用
	 *@return 成功返回S_OK,其他值为错误代码
	 */
	HRESULT OnCoilTrigger(WPARAM wParam, LPARAM lParam);

	/**
	 *@brief 获取黑匣子记录的消息
	 *@param [in] wParam 无
	 *@param [out] lParam,CHAR*指针，状态字符串.
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	HRESULT OnGetBlackBoxMessage(WPARAM wParam, LPARAM lParam);

	/**
	 *@brief 获取设备工作模式
	 *@param [in] wParam 工作模式索引指针,int 类型
	 *@param [out] lParam 工作模式字符串，CHAR* 类型
	 *@return 成功返回S_OK,其他值为错误代码
	 */
	HRESULT OnGetWorkModeFromIndex(WPARAM wParam, LPARAM lParam);

	/**
	 *@brief NTP同步时间函数
	 *@return 成功返回S_OK,其他值为错误代码
	 */
	HRESULT NtpTimeSync();

	/**
	 *@brief 初始化硬盘函数
	 *@return 成功返回S_OK,其他值为错误代码
	 */
	HRESULT InitHdd(VOID);	
	/**
	 *@brief 卸载硬盘函数
	 *@return 成功返回S_OK,其他值为错误代码
	 */
	HRESULT DeinitHdd(VOID);	
	HRESULT OnGetNetPackageVersion(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetFirmwareVersion(WPARAM wParam, LPARAM lParam);
    HRESULT OnGetBackUpVersion(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetFPGAVersion(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetKernelVersion(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetUBootVersion(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetUBLVersion(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetSN(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetCPU(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetRAM(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetLogLevel(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetLogLevel(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetNTPServerIP(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetNTPServerIP(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetNTPEnable(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetNTPEnable(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetNTPTimeZone(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetNTPTimeZone(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetNTPInterval(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetNTPInterval(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetHDDCheckReport(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetCustomizedDevName(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetCustomizedDevName(WPARAM wParam, LPARAM lParam);
	HRESULT OnReInitHDD(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetAWBWorkMode(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetAWBWorkMode(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetAutoControlCammeraAll(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetAutoControlCammeraAll(WPARAM wParam, LPARAM lParam);
	HRESULT OnSetH264Resolution(WPARAM wParam, LPARAM lParam);
	HRESULT OnGetH264Resolution(WPARAM wParam, LPARAM lParam);
    /**
     *@brief 设置降噪参数，由2A进程控制
     *@param [PVOID] pvBuffer 降噪参数
     *@param [PVOID] iSize 保留
     *@return 成功返回S_OK,其他值为错误代码
     */
	HRESULT OnSetDenoiseStatus(WPARAM wParam, LPARAM lParam);

	//消息映射函数
	SW_BEGIN_MESSAGE_MAP(CSWLPRApplication, CSWMessage)
		SW_MESSAGE_HANDLER(MSG_APP_SETTIME, OnSetTime)
		SW_MESSAGE_HANDLER(MSG_APP_GETTIME, OnGetTime)
		SW_MESSAGE_HANDLER(MSG_APP_SETIP,   OnSetIP)
		SW_MESSAGE_HANDLER(MSG_APP_GETIP,   OnGetIP)
		SW_MESSAGE_HANDLER(MSG_APP_SETXML,  OnSetXML)
		SW_MESSAGE_HANDLER(MSG_APP_GETXML,  OnGetXML)
		SW_MESSAGE_HANDLER(MSG_APP_GETWORKMODE_COUNT, OnGetWorkModeCount)
		SW_MESSAGE_HANDLER(MSG_APP_GETWORKMODE, OnGetWorkMode)
		SW_MESSAGE_HANDLER(MSG_APP_GETWORKMODEINDEX, OnGetWorkModeFromIndex)
		SW_MESSAGE_HANDLER(MSG_APP_RESETDEVICE, OnResetDevice)
		SW_MESSAGE_HANDLER(MSG_APP_RESTORE_DEFAULT, OnRestoreDefault)
		SW_MESSAGE_HANDLER(MSG_APP_RESTORE_FACTORY, OnRestoreFactory)
//		SW_MESSAGE_HANDLER(MSG_APP_SETCHARACTER_ENABLE, OnSetCharacterEnable)
//		SW_MESSAGE_HANDLER(MSG_APP_GETCHARACTER_ENABLE, OnGetCharacterEnable)
		SW_MESSAGE_HANDLER(MSG_APP_GETHVNAME, OnGetHvName)
		SW_MESSAGE_HANDLER(MSG_APP_GETHVID, OnGetHvID)
		SW_MESSAGE_HANDLER(MSG_APP_GETPRODUCT_NAME, OnGetProductName)
		SW_MESSAGE_HANDLER(MSG_APP_GETVIDEO_COUNT, OnGetVideoCount)
		SW_MESSAGE_HANDLER(MSG_APP_GETVERSION, OnGetVersion)
		SW_MESSAGE_HANDLER(MSG_APP_GETVRESION_STRING, OnGetVersionString)
		SW_MESSAGE_HANDLER(MSG_APP_GETCONNECTED_IP, OnGetConnectedIP)
		SW_MESSAGE_HANDLER(MSG_APP_HDD_FDISK, OnHDDFDisk)
		SW_MESSAGE_HANDLER(MSG_APP_HDD_CHECK, OnHDDCheck)
		SW_MESSAGE_HANDLER(MSG_APP_HDD_STATUS, OnHDDStatus)
		SW_MESSAGE_HANDLER(MSG_APP_RESET_COUNT, OnResetCount)
		SW_MESSAGE_HANDLER(MSG_APP_RESET_REPORT, OnResetReport)
		SW_MESSAGE_HANDLER(MSG_APP_RESET_MODE, OnResetMode)
		SW_MESSAGE_HANDLER(MSG_APP_GETDEVTYPE, OnGetDevType)
		SW_MESSAGE_HANDLER(MSG_APP_GETCPU_TEMPERATURE, OnGetCpuTemperature)
		SW_MESSAGE_HANDLER(MSG_APP_UPDATE_STATUS, OnUpdateStatus)
		SW_MESSAGE_HANDLER(MSG_APP_GET_RUN_STATUS, OnGetRunStatus)
		
		SW_MESSAGE_HANDLER(MSG_APP_RADAR_TRIGGER, OnRadarTrigger)	//雷达触发抓拍
		
		SW_MESSAGE_HANDLER(MSG_APP_COIL_STATUS, OnCoilStatus)
		SW_MESSAGE_HANDLER(MSG_APP_COIL_TRIGGER, OnCoilTrigger)
		SW_MESSAGE_HANDLER(MSG_APP_GET_BLACKBOX_MESSAGE, OnGetBlackBoxMessage)
		SW_MESSAGE_HANDLER(MSG_APP_GETFIRMWAREVERSION, OnGetFirmwareVersion)
		SW_MESSAGE_HANDLER(MSG_APP_GETNETPACKAGEVERSION, OnGetNetPackageVersion)
        SW_MESSAGE_HANDLER(MSG_APP_GETBACKUPTVERSION, OnGetBackUpVersion)
		SW_MESSAGE_HANDLER(MSG_APP_GETFPGAVERSION, OnGetFPGAVersion)
		SW_MESSAGE_HANDLER(MSG_APP_GETKERNALVERSION, OnGetKernelVersion)
		SW_MESSAGE_HANDLER(MSG_APP_GETUBOOTVERSION, OnGetUBootVersion)
		SW_MESSAGE_HANDLER(MSG_APP_GETUBLVERSION, OnGetUBLVersion)
		SW_MESSAGE_HANDLER(MSG_APP_GETSN, OnGetSN)
		SW_MESSAGE_HANDLER(MSG_APP_GETCPU, OnGetCPU)
		SW_MESSAGE_HANDLER(MSG_APP_GETRAM, OnGetRAM)
		SW_MESSAGE_HANDLER(MSG_APP_GET_LOG_LEVEL, OnGetLogLevel)
		SW_MESSAGE_HANDLER(MSG_APP_SET_LOG_LEVEL, OnSetLogLevel)
		SW_MESSAGE_HANDLER(MAG_APP_GET_NTP_SERVER_IP, OnGetNTPServerIP)
		SW_MESSAGE_HANDLER(MAG_APP_SET_NTP_SERVER_IP, OnSetNTPServerIP)
		SW_MESSAGE_HANDLER(MAG_APP_GET_NTP_ENABLE, OnGetNTPEnable)
		SW_MESSAGE_HANDLER(MAG_APP_SET_NTP_ENABLE, OnSetNTPEnable)
		SW_MESSAGE_HANDLER(MAG_APP_GET_NTP_TIMEZONE, OnGetNTPTimeZone)
		SW_MESSAGE_HANDLER(MAG_APP_SET_NTP_TIMEZONE, OnSetNTPTimeZone)
		SW_MESSAGE_HANDLER(MAG_APP_GET_NTP_INTERVAL, OnGetNTPInterval)
		SW_MESSAGE_HANDLER(MAG_APP_SET_NTP_INTERVAL, OnSetNTPInterval)
		SW_MESSAGE_HANDLER(MSG_APP_HDD_CHECKREPORT, OnGetHDDCheckReport)
		SW_MESSAGE_HANDLER(MSG_APP_SET_CUSTOMIZED_DEVNAME, OnSetCustomizedDevName)
		SW_MESSAGE_HANDLER(MSG_APP_GET_CUSTOMIZED_DEVNAME, OnGetCustomizedDevName)
		SW_MESSAGE_HANDLER(MSG_APP_REINIT_HDD, OnReInitHDD)
		SW_MESSAGE_HANDLER(MSG_APP_SET_AWB_WORK_MODE, OnSetAWBWorkMode)
		SW_MESSAGE_HANDLER(MSG_APP_GET_AWB_WORK_MODE, OnGetAWBWorkMode)
		SW_MESSAGE_HANDLER(MSG_APP_SET_AUTO_CONTROL_CAMMERA_ALL, OnSetAutoControlCammeraAll)
		SW_MESSAGE_HANDLER(MSG_APP_GET_AUTO_CONTROL_CAMMERA_ALL, OnGetAutoControlCammeraAll)
		SW_MESSAGE_HANDLER(MSG_APP_SET_H264_RESOLUTION, OnSetH264Resolution)
		SW_MESSAGE_HANDLER(MSG_APP_GET_H264_RESOLUTION, OnGetH264Resolution)
		SW_MESSAGE_HANDLER(MSG_APP_SET_DENOISE_STATUS, OnSetDenoiseStatus)
	SW_END_MESSAGE_MAP()

    HRESULT OnGetGB28181Param(PVOID pvBuffer, INT iSize);
    /**
     *@brief 复位设备
     *@param [PVOID] pvBuffer 保留
     *@param [PVOID] iSize 保留
     *@return 成功返回S_OK,其他值为错误代码
     */
    HRESULT OnReBoot(PVOID pvBuffer, INT iSize);

	HRESULT OnSendRtspParam(PVOID pvBuffer, INT iSize);

	HRESULT OnGetOnvifParam(PVOID pvBuffer, INT iSize);


    //远程消息映射函数
    SW_BEGIN_REMOTE_MESSAGE_MAP(CSWLPRApplication, CSWMessage)
	    SW_REMOTE_MESSAGE_HANDLER(MSG_APP_GET_28181_PARAM,OnGetGB28181Param)
	    SW_REMOTE_MESSAGE_HANDLER(MSG_APP_REBOOT,OnReBoot)
		SW_REMOTE_MESSAGE_HANDLER(MSG_APP_GET_RTSP_PARAM, OnSendRtspParam)
		SW_REMOTE_MESSAGE_HANDLER(MSG_APP_GET_ONVIF_PARAM, OnGetOnvifParam)
		//SW_REMOTE_MESSAGE_HANDLER(MSG_APP_SET_DENOISE_STATUS, OnSetDenoiseStatus)
    SW_END_REMOTE_MESSAGE_MAP()
private:
	TiXmlDocument m_xmlDoc;
	CSWLPRParameter *m_pParam;
	INT    m_hObject;

	struct tag_resetInfoHeader
	{
		DWORD dwFlag;
		DWORD dwCount;
		DWORD dwHead;
		DWORD dwTail;
		DWORD dwCRC;
	} m_cResetInfoHeader;
	static const DWORD RESET_INFO_FLAG = 0xFFEE0004;

	enum
	{
		DEVTYPE_MERCURY = 0,
		DEVTYPE_VENUS,
		DEVTYPE_JUPITER
	};

	CSWString m_strH264Status;
	CSWString m_strVPIFStatus;
	CSWString m_strRecogStatus;
	CSWString m_strDM6467Status;
	CSWString m_strRunStatus;

	CSWString m_strResultNetStatus;
	CSWString m_strJpegNetStatus;
	CSWString m_strH264NetStatus;

	CSWString m_strResultHDDStatus;
	CSWString m_strFTPStatus;
	CSWString m_strH264HDDStatus;
	CSWString m_strRTSPConnStatus;
	CSWString m_strRTSPSecondConnStatus;

	BOOL	m_fNetDiskMounted;
	BOOL	m_fHardDiskReady;
	CHAR	m_szHDDPath[64];
	DWORD	m_dwDevType;

	
	// AT88
	DWORD m_rgdwNC[4];
	
	//DSP触发开关,用来标志是视频触发还是外部触发
	BOOL  m_fDSPTrigger;	//触发抓拍，0表示不抓拍，1表示触发抓拍
    INT m_iFlashDiffFlag;           // 闪光灯轮闪标记，0：闪其中一个灯，1：闪另外一个灯
	CSWString m_strCoilStatus;

	//SD卡错误记录
	struct SdErrRecord
	{
		UINT id;
		DWORD time;
	};
	CSWList<SdErrRecord*>	m_lstSdErrRecord;
	INT m_MaxSdErrNum;
	INT m_SdErrNum;

	BOOL m_fIsAppReady;		//APP运行正常
};
extern CSWLPRApplication *theApp;
#endif

