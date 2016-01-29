/**
* @file SWGBH264 .h
* @brief GBH264模块
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-04-01
* @version 1.0
*/


#ifndef __SW_GBH264_TRANSFORM_FILTER_H__
#define __SW_GBH264_TRANSFORM_FILTER_H__

#include "SWBaseFilter.h"
#include "SWClassFactory.h"
#include "SWFC.h"
#include "SWMessage.h"
#include "SWH264RTPParser.h"
/**
* @brief GBH264 模块
*
*
*
* 
*/
#define MAX_PREVIEW 3
class CSWH264Filter : public CSWBaseFilter,public CSWMessage
{
    CLASSINFO(CSWH264Filter, CSWBaseFilter);

public:
	/*
	* @brief 构造函数
    *
	*/
    CSWH264Filter();
	
	/*
	* @brief 析构函数
	*
	*/
    virtual ~CSWH264Filter();

	/*
	* @brief 初始化函数
	* @retval S_OK : 成功
	* @retval E_OUTOFMEMORY : 内存不足
	*/
	HRESULT Initialize();

	
	/**
	 * @brief 接收输入的数据包
	 * @param [in] pdu 接收数据包
	 * @成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT Receive(CSWObject* obj);

	/*
	* @brief 开始运行函数
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	*/
	virtual HRESULT Run();

	/*
	* @brief 停止运行函数
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	*/
	virtual HRESULT Stop();

    typedef struct {
        CHAR ip[32];
        DWORD port;
		DWORD dwHeartBeatTime;
    }PreviewObject;

protected:

    SW_BEGIN_DISP_MAP(CSWH264Filter,CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 0)
	SW_END_DISP_MAP();


protected:
        HRESULT OnH264DoPreview(WPARAM wParam, LPARAM lParam);
        SW_BEGIN_MESSAGE_MAP(CSWH264Filter, CSWMessage)
            SW_MESSAGE_HANDLER(MSG_H264_DO_PREVIEW,OnH264DoPreview)
        SW_END_MESSAGE_MAP()
protected:
	static PVOID OnSendDataProxy(PVOID pvArg);
	HRESULT OnSendData();

private:
	
	BOOL				m_fInited;

	DWORD				m_dwQueueSize;
	CSWSemaphore		m_cSemaSync;
	CSWThread			m_cThreadSend;
	CSWMutex			m_cMutexLock;

	CSWList<CSWImage*>	m_lstFrames;
    PreviewObject       m_lstPreview[MAX_PREVIEW];
    H264RTPParser       m_H264Rtp;
    CSWUDPSocket        m_Rtpsock;
    DWORD               m_PreviewCount;
    CSWMutex            m_cPreviewMutexLock;
};

REGISTER_CLASS(CSWH264Filter);
	
#endif //__SW_GBH264_TRANSFORM_FILTER_H__





