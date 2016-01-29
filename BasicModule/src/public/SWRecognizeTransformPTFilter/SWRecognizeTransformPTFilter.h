/**
 * @file SWRecognizeTransformPTFilter.h
 * @brief 识别Filter
 * @copyright Signalway All Rights Reserved
 * @author zhaopy
 * @date 2013-03-20
 * @version 1.0
 */

#ifndef __SW_RECOGNIZE__TRANSFORM_FILTER_H__
#define __SW_RECOGNIZE__TRANSFORM_FILTER_H__

#include "SWBaseFilter.h"
#include "SWMessage.h"
#include "SWCarLeft.h"
#include "SWPosImage.h"
#include "SWCameraDataPDU.h"
#include "SWDspLinkCtrl.h"

class CSWRecognizeTransformPTFilter: public CSWBaseFilter, CSWMessage
{
CLASSINFO(CSWRecognizeTransformPTFilter, CSWBaseFilter)
    public:
    CSWRecognizeTransformPTFilter();
    virtual ~CSWRecognizeTransformPTFilter();

    /**
     *@brief 识别模块初始化。
     */
    virtual HRESULT Initialize(INT iGlobalParamIndex, PVOID pvFrameParam, PVOID pvCfgParam);
    virtual HRESULT RegisterCallBackFunction(PVOID OnDSPAlarm, PVOID pvParam);
    virtual HRESULT SetOutputImage(INT iBest, INT iLast);
    virtual HRESULT Run();
    virtual HRESULT Stop();

    static VOID OnProcessProxy(PVOID pvParam);

public:
    /**
     *@brief ISWPlateRecognitionEvent 回调接口
     */
    virtual HRESULT CarArriveEvent(CARARRIVE_INFO_STRUCT *pCarArriveInfo);
    virtual HRESULT CarLeftEvent(CARLEFT_INFO_STRUCT *pCarLeftInfo, const BOOL fEvasion, const DWORD dwEvasionTick);

protected:
    /**
     *@brief 重载Receive接口，接收CSWImage数据
     */
    virtual HRESULT Receive(CSWObject* obj);

    /**
     *@brief 识别主线程
     */
    HRESULT OnProcess();

    /**
     *@brief 设置识别开关
     */
    HRESULT OnRecognizeGetJPEG(WPARAM wParam, LPARAM lParam);

    /**
     *@brief 输出调试码流
     */
    HRESULT OnRecognizeOutPutDebug(WPARAM wParam, LPARAM lParam);

    /**
     *@brief 设置监测冲卡逃费标志
    */
    HRESULT SetTollEvasionDetectingFlag(const BOOL fDetectingFlag)
    {
        m_fDetectingTollEvasion = fDetectingFlag;
    return S_OK;
    }

    /**
     *@brief 获取监测冲卡逃费标志
    */
    BOOL GetTollEvasionDetectingFlag(VOID)
    {
        return m_fDetectingTollEvasion;
    }
    
    /**
    *@brief 设置跟车冲卡时间间隔阈值
    */
    HRESULT SetTailgatingTimeThreshold(const INT iTime)
    {
        if (0 < iTime )
        {
            m_iTailgatingTimeThreshold = iTime;
            return S_OK;
        }
        else
        {
            return E_INVALIDARG;
        }
    }

    /**
    *@brief 获取跟车冲卡时间间隔阈值
    */
    INT GetTailgatingTimeThreshold()
    {
        return m_iTailgatingTimeThreshold;
    }

    /**
    *@brief 设置栏杆机常态（落下状态）的输出
    */
    HRESULT SetBarrierNormalState(const INT iMode)
    {
        m_iBarrierNormalMode = iMode;
        return S_OK;
    }

    /**
    *@brief 获取栏杆机常态（落下状态）的输出
    */
    INT GetBarrierNormalState()
    {
        return m_iBarrierNormalMode;
    }

    /**
    *@brief 获取栏杆状态标志
    */
    HRESULT GetBarrierStatus(INT& iStatus);

    /**
    *@brief 判断是否冲卡逃费
    */
    HRESULT CheckTollEvasion(BOOL& fEvasion);

protected:
    //自动化映射宏
    SW_BEGIN_DISP_MAP(CSWRecognizeTransformFilter, CSWBaseFilter)
        SW_DISP_METHOD(Initialize, 3)
        SW_DISP_METHOD(RegisterCallBackFunction, 2)
        SW_DISP_METHOD(SetOutputImage, 2)
        SW_DISP_METHOD(SetTollEvasionDetectingFlag, 1)
        SW_DISP_METHOD(SetTailgatingTimeThreshold, 1)
        SW_DISP_METHOD(SetBarrierNormalState, 1)
    SW_END_DISP_MAP()
    //消息映射宏
    SW_BEGIN_MESSAGE_MAP(CSWRecognizeTransformPTFilter, CSWMessage)
        SW_MESSAGE_HANDLER(MSG_RECOGNIZE_GETJPEG, OnRecognizeGetJPEG)
        SW_MESSAGE_HANDLER(MSG_RECOGNIZE_OUTPUTDEBUG, OnRecognizeOutPutDebug)
    SW_END_MESSAGE_MAP()

private:
    VOID Clear();

public:
    static const INT MAX_IMAGE_COUNT = 3;

private:
    TRACKER_CFG_PARAM *m_pTrackerCfg;
    PR_PARAM m_cPlateRecognitionParam;
    BOOL m_fSendJPEG;
    BOOL m_fSendDebug;
    BOOL m_fInitialized;
    INT m_iCarArriveTriggerType;
    INT m_iIOLevel;

    INT m_iOutPutBestImage;
    INT m_iOutPutLastImage;

    CSWList<CSWImage*> m_lstImage;		// 图片对列
    CSWMutex m_cMutexImage;             // 访问临界区
    CSWThread* m_pThread;				// 识别线程
    CSWSemaphore m_cSemImage;

    BOOL m_fDetectingTollEvasion;  //是否监测冲卡逃费
    INT m_iTailgatingTimeThreshold; //判断跟车冲卡时间阈值
    INT m_iBarrierNormalMode; //栏杆机正常模式输出

    CSWList<BOOL> m_lstTollEvasionFlag;        // 冲卡标志对列
    CSWMutex m_cMutexTollEvasion;             // 访问临界区
};
REGISTER_CLASS(CSWRecognizeTransformPTFilter)
#endif

