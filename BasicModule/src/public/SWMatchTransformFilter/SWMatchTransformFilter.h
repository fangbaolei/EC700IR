#ifndef __SW_MATCH_TRANSFORM_FILTER_H__
#define __SW_MATCH_TRANSFORM_FILTER_H__
#include "SWBaseFilter.h"
#include "SWBaseDevice.h"
#include "SWCarLeft.h"
#include "SWMatchTansformDataStruct.h"
#define NOT_MATCH_TIME 0xFFFFFFFF
#define BEGIN_MAP_MATCH_FUNCTION()\
	typedef bool (CSWMatchTransformFilter::*pFunMatch)(SIGNAL_PARAM *pSignal, CARLEFT_MATCH_INFO *pMatchInfo);\
	pFunMatch GetFunction(int type)\
	{\
		switch(type)\
		{
#define MAP_MATCH_FUNCTION(id,func)  case id : return &CSWMatchTransformFilter::func; break;
#define END_MAP_MATCH_FUNCTION()\
			default : return NULL;\
		}\
	}

// 车辆信息结构体
typedef struct tag_CarLeftMatchInfo
{
	CSWCarLeft* pCarLeft;
	DWORD dwInputTime;
	DWORD dwMatchImageTime[3];
	DWORD dwMatchSpeedTime;
	DWORD dwOutputCondition; // 0:正常识别结果;1:主信号超时的结果。	
	
	tag_CarLeftMatchInfo()
	{
		pCarLeft = NULL;
		dwInputTime = CSWDateTime::GetSystemTick();
		for(int i = 0; i < 3; i++)
		{
			dwMatchImageTime[i] = NOT_MATCH_TIME;
		}
		dwMatchSpeedTime = NOT_MATCH_TIME;
		dwOutputCondition = 0;
	}
}
CARLEFT_MATCH_INFO;

class CSWMatchTransformFilter : public CSWBaseFilter, CSWMessage
{
	CLASSINFO(CSWMatchTransformFilter, CSWBaseFilter)
public:
	CSWMatchTransformFilter();
	virtual ~CSWMatchTransformFilter();
protected:	
	virtual HRESULT Initialize(PVOID pvParam1, PVOID pvParam2,BOOL fRadarTrigger);
	virtual HRESULT Run();
	virtual HRESULT Stop();
	virtual HRESULT Receive(CSWObject* obj);
protected:
  /**
   *@brief 外部设备信号处理线程
   */
	static void* OnProcessIPTSignal(void* pvParam);
	/**
	 *@brief 信号匹配线程
	 */
	static void* OnMatchSignal(void* pvParam);

	/**
	 *@brief 结果输出线程
	 */
	static void* OnOutputProxy(void* pvParam);
	HRESULT OnOutput();

	/**
   *@brief 启动串口测试服务
   */
	HRESULT OnStartCOMTest(WPARAM wParam, LPARAM lParam);

	/**
   *@brief 停止串口测试服务
   */
	HRESULT OnStopCOMTest(WPARAM wParam, LPARAM lParam);

	/**
   *@brief 雷达接收信号开关
   */
	HRESULT OnRadarCtrl(WPARAM wParam, LPARAM lParam);

		/**
		   @brief 外总控使能开关
		   @param [in] 1使能 0不使能
		 */
	HRESULT OnOutputCtrlEnable(WPARAM wParam, LPARAM lParam);

	//更新当前存在的所有触发且还没有结束跟踪的
	HRESULT OnUpdateAllCarTrigger(WPARAM wParam, LPARAM lParam);

protected:
	/**
	 *@brief 信号是否包含图像
	 */
	bool SignalIncludeImage(SIGNAL_PARAM* pSignal, bool fCapture = true);
	
	/**
	 *@brief 将图片信号添加到链表
	 */
	bool AppendImage(CSWImage *pImage);
	
	/**
	 *@brief 将速度信号添加到链表
	 */
	bool AppendSpeed(DWORD dwRoadID, DWORD dwTime, DWORD dwSpeed, DWORD dwSpeedType,DWORD dwDirection);
	
	/**
	 *@brief 速度匹配函数
	 */
	bool MatchSpeed(SIGNAL_PARAM *pSignal, CARLEFT_MATCH_INFO *pMatchInfo);
	/**
	 *@brief 抓拍图匹配函数
	 */
	bool MatchImage(SIGNAL_PARAM *pSignal, CARLEFT_MATCH_INFO *pMatchInfo);
	/**
	 *@brief 触发匹配函数
	 */
	bool MatchTrigger(SIGNAL_PARAM *pSignal, CARLEFT_MATCH_INFO *pMatchInfo);
	
	static VOID OnEvent(PVOID pvParam, CSWBaseDevice *pDevice, CSWBaseDevice::DEVICE_TYPE type, DWORD dwTime, PDWORD pdwValue);
	
	VOID Close(VOID);

	BOOL CheckTriggerIndexTracker(INT iTriggerIndex);
	
	/**
	 *@brief OLE初始化
	 */
	SW_BEGIN_DISP_MAP(CSWMatchTransformFilter,CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 3)
	SW_END_DISP_MAP()
	
	/**
	 *@brief 匹配函数映射表
	 */

	BEGIN_MAP_MATCH_FUNCTION()
		MAP_MATCH_FUNCTION(CSWBaseDevice::SPEED, MatchSpeed)
		MAP_MATCH_FUNCTION(CSWBaseDevice::IMAGE, MatchImage)
		MAP_MATCH_FUNCTION(CSWBaseDevice::TRIGGER, MatchTrigger)
	END_MAP_MATCH_FUNCTION()

	//消息映射宏
	SW_BEGIN_MESSAGE_MAP(CSWMatchTransformFilter, CSWMessage)
	SW_MESSAGE_HANDLER(MSG_COM_TEST_ENABLE, OnStartCOMTest)
 
	SW_MESSAGE_HANDLER(MSG_COM_OUTPUT_CTRL_ENABLE, OnOutputCtrlEnable)
 
	SW_MESSAGE_HANDLER(MSG_COM_TEST_DISABLE, OnStopCOMTest)

	SW_MESSAGE_HANDLER(MSG_SET_RADARENABLE, OnRadarCtrl)

	SW_MESSAGE_HANDLER(MSG_UPDATE_ALL_TRIGGER, OnUpdateAllCarTrigger)
	SW_END_MESSAGE_MAP()
private:
	
	BOOL        m_fInitialize;
	MATCHPARAM  m_cParam;
	DEVICEPARAM m_cDevParam[2];
	CSWSemaphore m_semMatch;
	CSWMutex  m_cMutex;
	CSWBaseDevice *m_pDevice[2];
	CSWThread m_matchThread;

	CSWBaseDevice *m_pComTestDevic;		//串口测试
	INT m_iTESTINGCOMID;				//正在测试的串口号

	BOOL m_fRadarTrigger;	//雷达触发抓拍

	// zhaopy
	CSWThread m_cOutputThread;
	CSWList<CSWCarLeft*, 6> m_cOutputList;
	CSWMutex  m_cOutputMutex;
	CSWSemaphore m_semOutput;

	CSWList<CARLEFT_MATCH_INFO*,6> m_cCarLeftList;
	CSWMutex m_cCarLeftMutex;
	CSWMutex m_cImportMutex;

	TRACKER_CFG_PARAM* m_pTrackerCfg;

	INT m_rgiAllCarTrigger[MAX_EVENT_COUNT];
	CSWMutex  m_cAllCarTriggerMutex;

	BOOL m_Radar;

//    INT m_iOutputThreadRestartTime;     // 结果输出线程重启次数
};
REGISTER_CLASS(CSWMatchTransformFilter)
#endif

