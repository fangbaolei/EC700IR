#ifndef __SW_AUTO_CONTROL_RENDER_FILTER_H__
#define __SW_AUTO_CONTROL_RENDER_FILTER_H__
#include "SWBaseFilter.h"
#include "SWMessage.h"

class CSWAutoControlRenderFilter : public CSWBaseFilter, CSWMessage
{
	CLASSINFO(CSWAutoControlRenderFilter, CSWBaseFilter)
public:
	CSWAutoControlRenderFilter();
	virtual ~CSWAutoControlRenderFilter();
	/**
	 *@brief 是否使能模块和AGC
	 *@param [in] fEnable       是否使能此模块
	 *@param [in] fEnableAGC    是否使能AGC
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	HRESULT Enable(BOOL fEnable,
	        BOOL fEnableAGC,
	        INT iMinPSD,
	        INT iMaxPSD,
	        INT iEnableCpl,
	        INT iMinPlateY,
	        INT iMaxPlateY);
	/**
	 *@brief 设置每个等级的AGC门限、曝光时间和增益
	 *@param [in] iLevel 等级
	 *@param [in] iAGCLimit AGC门限
	 *@param [in] iExposureTime 曝光时间
	 *@param [in] iGain 增益
	 */
	HRESULT SetLevel(INT iLevel, INT iAGCLimit, INT iExposureTime, INT iGain);
	/**
	 *@brief 设置自动抓拍参数。
	 *@param [in] fEnable	是否使能
	 *@param [in] iDayShutter 白天最大抓拍快门
	 *@param [in] iDayGain	  白天最大抓拍增益
	 *@param [in] iNightShutter 晚上最大抓拍快门
	 *@param [in] iNightGain	晚上最大抓拍增益
	 */
	HRESULT SetAutoCaptureParam(BOOL fEnable, INT iDayShutter, INT iDayGain, INT iNightShutter, INT iNightGain);
    HRESULT SetCaptureShutterGain(INT iCaptureShutter, INT iCaptureGain);

protected:
	virtual HRESULT Run();
	virtual HRESULT Stop();	
	virtual HRESULT Receive(CSWObject* obj);	
	static void* OnProcessCameraPDU(void* pvParam);

	// 抓拍参数调整。
	VOID UpdateCaptureParam(const IMAGE_EXT_INFO& cInfo);
	// 场景第一次初始化确定后需要做的事情。
	VOID DoInit();
	
	HRESULT SaveParam(INT iLightType, INT iCplStatus, INT iPluseLevel);

    HRESULT SetNightThresholdArg(INT nNightShutter, INT nNightThreshold, INT nDuskThreshold, INT nMaxAgcShutter);

    HRESULT SetDayNightShutterHOri(INT iDayNightShutterEnable, INT iDayShutterHOri, INT iNightShutterHOri,INT iGainHOri, INT iNightGainHOri, INT iShutterLOri);
protected:
		//自动化映射宏
	SW_BEGIN_DISP_MAP(CSWAutoControlRenderFilter, CSWBaseFilter)
		SW_DISP_METHOD(Enable, 7)
		SW_DISP_METHOD(SetLevel, 4)
		SW_DISP_METHOD(SetAutoCaptureParam, 5)
        SW_DISP_METHOD(SetCaptureShutterGain, 2)
        SW_DISP_METHOD(SetNightThresholdArg, 4)
        SW_DISP_METHOD(SetDayNightShutterHOri, 6)
	SW_END_DISP_MAP()
	
	/**
	 *@breif 读取摄像机等级
	 *@param [in] wParam, INT*的大小为3的一维数组,0:iLightType;1:iCplStatus,2:iPluseLevel
	 *@param [in] lParam, 无
	 *@return 成功返回S_OK失败返回E_FAIL
	 */
	HRESULT OnReadParam(WPARAM wParam, LPARAM lParam);

	/**
	 *@breif 读取摄像机实时等级信息
	 *@param [in] wParam, 无
	 *@param [in] lParam, INT*的大小为3的一维数组,0:iLightType;1:iCplStatus,2:iPluseLevel
	 *@return 成功返回S_OK失败返回E_FAIL
	 */
	HRESULT OnGetRealParam(WPARAM wParam, LPARAM lParam);

	/**
	 *@breif 获取当前时段信息
	 *@param [in] wParam, 无
	 *@param [in] lParam, INT*,0:白天;1:晚上
	 *@return 成功返回S_OK失败返回E_FAIL
	 */
	HRESULT OnGetEnvPeriod(WPARAM wParam, LPARAM lParam);

    /**
     *@breif 设置最大AGC快门值
     *@param [in] wParam, 快门值
     *@param [in] lParam, 无
     *@return 成功返回S_OK失败返回E_FAIL
     */
    HRESULT OnSetMaxAgcShutter(WPARAM wParam, LPARAM lParam);


	/**
	   @brief 自动控制抓拍参数
	   @param [in] wParam 无
	   @param [in] wParam 无
	   @return 成功返回S_OK失败返回E_FAIL
	 */
	HRESULT OnSetCaptureAutoParam(WPARAM wParam, LPARAM lParam);

	HRESULT SetRealTimeDayNightShutterHOri(WPARAM wParam, LPARAM lParam);

	//消息映射宏
	SW_BEGIN_MESSAGE_MAP(CSWAutoControlRenderFilter, CSWMessage)
		SW_MESSAGE_HANDLER(MSG_AUTO_CONTROL_READPARAM, OnReadParam)
		SW_MESSAGE_HANDLER(MSG_AUTO_CONTROL_GET_REALPARAM, OnGetRealParam)
		SW_MESSAGE_HANDLER(MSG_AUTO_CONTROL_GET_ENVPERIOD, OnGetEnvPeriod)
		SW_MESSAGE_HANDLER(MSG_AUTO_SET_MAXAGCSHUTTER, OnSetMaxAgcShutter)
		SW_MESSAGE_HANDLER(MSG_REALTIME_SET_MAXAGCSHUTTERGAIN, SetRealTimeDayNightShutterHOri)
		//SW_MESSAGE_HANDLER(MSG_AUTO_CONTROL_SET_AUTO_CAPTURE_PARAM, OnSetCaptureAutoParam)
	SW_END_MESSAGE_MAP()
public:
	INT  m_iLightType;
	BOOL m_fEnableAGC;
	INT  m_irgAGCLimit[14];

private:
	BOOL m_fEnable;
	INT  m_iMinPSD;
	INT  m_iMaxPSD;
	INT  m_iCplStatus;
	INT  m_iPluseLevel;
	INT  m_irgExposureTime[14];
	INT  m_irgGain[14];
	BOOL m_fEnableAutoCapture;
	INT m_iDayShutterMax;
	INT m_iDayGainMax;
	INT m_iNightShutterMax;
	INT m_iNightGainMax;
	INT m_iGainMin;
	INT m_iShutterMin;
	INT m_iEnableCpl;

	INT m_iIsDayCount;
    INT m_iIsDuskCount;
	BOOL m_fIsDay;
	BOOL m_fIsDayEx;
    INT m_iEnvType;     // 白天、傍晚、晚上状态。3:day, 2:dusk, 1:night
	BOOL m_fIsInit; // 是否已经做过白天晚上的判断。
	BOOL m_fForceChange;	// 在场景跳变时强制改变抓拍参数。
	INT m_iCaptureShutter;
	INT m_iCaptureGain;

	INT m_iTempCaptureShutter;
	INT m_iTempCaptureGain;
	
	BOOL m_fNeedUpdateCaptureParam;
	INT m_iCaptureImageCount;
	INT m_iTotalAvgY;
	
	CSWObjectList m_lstPDU;
	CSWThread m_thCameraPDU;

	INT m_iGammaMode;			// 保存GAMMA模式.
    INT m_nNightShutter;        // 夜晚判断快门阈值
    INT m_nNightThreshold;      // 夜晚判断环境亮度阈值
    INT m_nDuskThreshold;       // 傍晚判断环境亮度阈值

	int m_nAvgPlateY;

	INT m_nMinPlateY;
	INT m_nMaxPlateY;
	INT m_nWDRLevel;

	BOOL m_fUseMaxAgcShutter;

	DWORD m_dwAvgY;	//亮度

    // 白天、晚上 独立快门上限值
    INT m_iAGCDayNightShutterControl;       // 开关 0:关   1:开
    INT m_iAGCDayShutterHOri;       // 白天快门上限
    INT m_iAGCNightShutterHOri;     // 晚上快门上限
    INT m_iAGCShutterLOri;      // 快门下限
    INT m_iAGCNightGainHOri; 	//晚上增益上限
    INT m_iAGCGainHOri; 	//白天增益上限
};
REGISTER_CLASS(CSWAutoControlRenderFilter)
#endif

