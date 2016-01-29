/**
* @file SWRecognizeTGTransformFilter.h 
* @brief 识别Filter
* @copyright Signalway All Rights Reserved
* @author zhaopy
* @date 2013-03-20
* @version 1.0
*/

#ifndef __SW_RECOGNIZE_TG_TRANSFORM_FILTER_H__
#define __SW_RECOGNIZE_TG_TRANSFORM_FILTER_H__

#include "SWBaseFilter.h"
#include "SWMessage.h"
#include "SWCarLeft.h"
#include "SWPosImage.h"
#include "SWCameraDataPDU.h"
#include "SWBaseLinkCtrl.h"
#include "AppDetCtrl.h"

class CSWRecognizeTGTransformFilter : public CSWBaseFilter, CSWMessage
{
	CLASSINFO(CSWRecognizeTGTransformFilter, CSWBaseFilter)
public:
	CSWRecognizeTGTransformFilter();
	virtual ~CSWRecognizeTGTransformFilter();

	/**
   *@brief 识别模块初始化。
   */
	virtual HRESULT Initialize(INT iGlobalParamIndex, 
		INT nLastLightType,INT nLastPulseLevel, INT nLastCplStatus, 
		PVOID pvParam, INT iMinPlateLight, INT iMaxPlateLight,BOOL fRecognizeEnable);
	
	virtual HRESULT RegisterCallBackFunction(PVOID OnDSPAlarm, PVOID pvParam);
	virtual HRESULT Run();
	virtual HRESULT Stop();
	//ARM端跟踪线程
	static VOID OnProcessProxy(PVOID pvParam);
	//DSP端识别线程
	static VOID OnProcessSyncProxy(PVOID pvParam);
	//降噪匹配线程
	static VOID OnMatchImageProxy(PVOID pvParam);
public:
	/**
   *@brief ISWPlateRecognitionEvent 回调接口
   */
	virtual HRESULT CarArriveEvent(CARARRIVE_INFO_STRUCT *pCarArriveInfo);
	virtual HRESULT CarLeftEvent(CARLEFT_INFO_STRUCT *pCarLeftInfo);
    virtual HRESULT GB28181Alarm(LPCSTR szMsg);

protected:
	/**
   *@brief 重载Receive接口，接收CSWImage数据
   */
	virtual HRESULT Receive(CSWObject* obj);	

	/**
   *@brief 提供图片识别命令
   */
	HRESULT OnRecognizePhoto(WPARAM wParam, LPARAM lParam);

	/**
   *@brief 设置识别开关
   */
	HRESULT OnRecognizeGetJPEG(WPARAM wParam, LPARAM lParam);

	/**
   *@brief 输出调试码流
   */
	HRESULT OnRecognizeOutPutDebug(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 设置触发使能
	 */
	HRESULT OnTriggerEnable(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief IO红绿灯触发事件
	 */
	HRESULT OnIOEvent(WPARAM wParam, LPARAM lParam);

	/**
	 *@brief 动态修改识别参数
	 */
	HRESULT OnModifyParam(WPARAM wParam, LPARAM lParam);

	/**
	 *@brief 获取车辆统计
	 */
	HRESULT OnGetCarLeftCount(WPARAM wParam, LPARAM lParam);

	/**
     *@brief 白天晚上状态变换
     *@brief wParam: 0表示白天，1表示晚上
     */
	HRESULT OnEnvPeriodChanged(WPARAM wParam, LPARAM lParam);

    /**
     *@brief 获取 DSP 环境状态
     *@brief lParam: （0白天，1傍晚，2晚上）
     */
    HRESULT OnGetDspEnvLightType(WPARAM wParam, LPARAM lParam);

	/**
   *@brief 识别主线程
   */
	HRESULT OnProcess();

	/**
	*@brief 识别DSP再处理线程
	*/
	HRESULT OnProcessSync();

	/**
	*@brief 降噪图片匹配线程
	*/
	HRESULT OnMatchImage();
	
	/**
	 *@brief 设置是否过滤逆行标志
	 */
	HRESULT SetReverseRunFilterFlag(const BOOL fFilter)
    {
    	m_fReverseRunFilterFlag = fFilter;
    }

	/**
	 *@brief 获取是否过滤逆行标志
	 */
	BOOL GetReverseRunFilterFlag()
    {
    	return m_fReverseRunFilterFlag;
    }

	/**
	   @brief 初始化FPGA对闪光灯的设置
	 */
//	HRESULT OnInitFpgaFlash();
	
protected:
	//自动化映射宏
	SW_BEGIN_DISP_MAP(CSWRecognizeTGTransformFilter, CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 8)
		SW_DISP_METHOD(RegisterCallBackFunction, 2)
		SW_DISP_METHOD(SetReverseRunFilterFlag, 1)
	SW_END_DISP_MAP()
	//消息映射宏
	SW_BEGIN_MESSAGE_MAP(CSWRecognizeTGTransformFilter, CSWMessage)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_PHOTO, OnRecognizePhoto)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_GETJPEG, OnRecognizeGetJPEG)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_OUTPUTDEBUG, OnRecognizeOutPutDebug)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_ENABLE_TRIGGER, OnTriggerEnable)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_IOEVENT, OnIOEvent)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_MODIFY_PARAM, OnModifyParam)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_GET_CARLEFT_COUNT, OnGetCarLeftCount)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_ENVPERIOD_CHANGED, OnEnvPeriodChanged)
        SW_MESSAGE_HANDLER(MSG_RECOGNIZE_GET_DSP_ENV_LIGHT_TYPE, OnGetDspEnvLightType)
	SW_END_MESSAGE_MAP()

private:
	VOID Clear();

public:
	static const INT MAX_IMAGE_COUNT = 1;
	static const INT MAX_DECT_COUNT = 1;
	static const INT MAX_MATCH_COUNT = 5;
private:
	BOOL m_fRecognizeEnable;	//识别开关
	
	TRACKER_CFG_PARAM *m_pTrackerCfg;
	BOOL m_fSendJPEG;
	BOOL m_fSendDebug;
	BOOL m_fInitialized;
	INT  m_iCarArriveTriggerType;
	INT  m_iIOLevel;

	CSWList<CSWImage*> m_lstImage;		// 图片对列
	CSWMutex m_cMutexImage;             // 访问临界区
	CSWThread* m_pThread;				// 识别线程
	CSWSemaphore m_cSemImage;			

	BOOL m_fReverseRunFilterFlag;

	DWORD m_dwPlateCount;		//车辆统计
	
	CSWThread* m_pProcQueueThread;		//ARM端识别结果处理线程

	struct PROC_QUEUE_ELEM		//ARM端处理结果类型
	{
		CSWImage* pImage;
		PVOID pData;
	};
    CSWMutex m_cMutexProcQueue;
    CSWList<PROC_QUEUE_ELEM> m_lstProcQueueList;
    CSWSemaphore m_cSemProcQueue;

	CSWThread* m_pMatchThread;			//匹配线程
	CSWMutex m_cMutexMatch;
    CSWList<PROC_QUEUE_ELEM> m_lstMatchImage;
	CSWSemaphore m_cSemMatch;			

	swTgApp::CAppDetCtrl m_cAlgDetCtrl;
    BOOL m_fModifyParam;
	int m_nEnvLightType;

//#define PLATE_COUNT_MAX 5	//最大统计车牌个数
#define PULSE_WIDTH_LEVEL_MAX 8	//最大脉宽等级
#define PULSE_WIDTH_LEVEL_MIN 0	//最小脉宽等级

	INT m_iPlateLightCheckCount;// 调整间隔
	INT m_iExpectPlateLightMax;	//期望亮度最大值
	INT m_iExpectPlateLightMin;	//希望亮度最小值
	
	//根据车牌亮度进行LED脉宽控制
	INT m_iPlateLightSum;	//总车牌亮度
	INT m_iPlateCount;		//车牌计数
	INT m_iPlateLightMax;	//最大车牌亮度
	INT m_iPlateLightMin;	//最小车牌亮度
	INT m_iUpStep;			//往上调的步进值，连续往上跳的话该值累加，调节步进加快
	INT m_iDownStep;		//往下调的步进值，连续往下跳的话该值累加，调节步进加快

	//同时需要参考环境亮度，主要是白天时脉宽要求是0，晚上时脉宽为非零
	INT m_iEnvLightSum;		//总环境亮度
	INT m_iFrameCount;		//统计环境亮度帧计数
	
	INT m_iPulseWidthLevel;		//当前脉宽等级

	INT AdjustPulseWidth(INT iPlateLight);
	VOID ResetAdjustPulseWidthInfo(INT iDefPulseWidth);

    INT m_nCurEnvPeriod; // 白天、傍晚、晚上状态。3:day, 2:dusk, 1:night

    INT m_nEnvFromM3;       // 来自M3  3:day, 2:dusk, 1:night
};
REGISTER_CLASS(CSWRecognizeTGTransformFilter)
#endif

