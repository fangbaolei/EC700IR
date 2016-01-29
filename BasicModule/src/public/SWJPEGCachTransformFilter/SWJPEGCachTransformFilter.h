/**
* @file SWJPEGCachTransformFilter.h
* @brief JPEG图片缓存Filter
* @copyright Signalway All Rights Reserved
* @author quanjh
* @date 2013-03-20
* @version 1.0
*/

#ifndef __SW_JPEGCACH__TRANSFORM_FILTER_H__
#define __SW_JPEGCACH__TRANSFORM_FILTER_H__

#include "SWBaseFilter.h"
#include "SWMessage.h"
#include "SWCarLeft.h"
#include "SWPosImage.h"
#include "SWCameraDataPDU.h"
#include "SWBaseLinkCtrl.h"

class CSWJPEGCachTransformFilter : public CSWBaseFilter, CSWMessage
{
    CLASSINFO(CSWJPEGCachTransformFilter, CSWBaseFilter)
public:
    CSWJPEGCachTransformFilter();
    virtual ~CSWJPEGCachTransformFilter();

	/**
   *@brief 识别模块初始化。
   */
	virtual HRESULT Initialize(INT iGlobalParamIndex, INT nLastLightType,INT nLastPulseLevel, INT nLastCplStatus, PVOID pvParam);
	virtual HRESULT RegisterCallBackFunction(PVOID OnDSPAlarm, PVOID pvParam);
	virtual HRESULT Run();
	virtual HRESULT Stop();

	static VOID OnProcessProxy(PVOID pvParam);

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
   *@brief 识别主线程
   */
	HRESULT OnProcess();

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
	
protected:
	//自动化映射宏
    SW_BEGIN_DISP_MAP(CSWJPEGCachTransformFilter, CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 5)
		SW_DISP_METHOD(RegisterCallBackFunction, 2)
		SW_DISP_METHOD(SetReverseRunFilterFlag, 1)
	SW_END_DISP_MAP()
	//消息映射宏
    SW_BEGIN_MESSAGE_MAP(CSWJPEGCachTransformFilter, CSWMessage)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_PHOTO, OnRecognizePhoto)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_GETJPEG, OnRecognizeGetJPEG)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_OUTPUTDEBUG, OnRecognizeOutPutDebug)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_ENABLE_TRIGGER, OnTriggerEnable)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_IOEVENT, OnIOEvent)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_MODIFY_PARAM, OnModifyParam)
	SW_END_MESSAGE_MAP()

private:
	VOID Clear();

public:
	static const INT MAX_IMAGE_COUNT = 3;

private:
	TRACKER_CFG_PARAM *m_pTrackerCfg;
	BOOL m_fSendJPEG;
	BOOL m_fSendDebug;
	BOOL m_fInitialized;
	INT  m_iCarArriveTriggerType;
	INT  m_iIOLevel;

	CSWList<CSWImage*> m_lstImage;		// 图片对列
	CSWMutex m_cMutexImage;             // 访问临界区
    CSWThread* m_pThread;				// 发送线程
	CSWSemaphore m_cSemImage;			

	BOOL m_fReverseRunFilterFlag;
};
REGISTER_CLASS(CSWJPEGCachTransformFilter)
#endif

