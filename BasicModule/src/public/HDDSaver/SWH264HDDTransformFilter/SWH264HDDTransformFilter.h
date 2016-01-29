/**
* @file SWH264HDDTransformFilter.h 
* @brief H264保存模块
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-22
* @version 1.0
*/


#ifndef __SW_H264HDD_TRANSFORM_FILTER_H__
#define __SW_H264HDD_TRANSFORM_FILTER_H__

#include "SWBaseFilter.h"
#include "SWMessage.h"
#include "SWImage.h"
#include "SWList.h"
//#include "BigFile.h"
#include "SWHDDTransformFilter.h"


#define H264HDD_TRANSFORM_IN_COUNT  1
#define H264HDD_TRANSFORM_OUT_COUNT 4




/**
* @brief H264保存模块
*
*
* 两个功能：一个是保存数据，另一个是读取数据。
* 
*/
class CSWH264HDDTransformFilter : public CSWHDDTransformFilter, CSWMessage
{
	CLASSINFO(CSWH264HDDTransformFilter, CSWHDDTransformFilter)
		
public:

	/**
	* @brief 构造函数
	* 
	*/
	CSWH264HDDTransformFilter();

	/**
	* @brief 析构函数
	* 
	*/
	virtual ~CSWH264HDDTransformFilter();

	/**
	* @brief 初始化函数
	* @param [in] szDirecory : 存储历史文件的磁盘路径
	* @param [in] llTotalSize : 存储历史文件的磁盘总空间大小
	* @param [in] iFileSize : 单个历史文件的定长大小
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	* @retval E_FAIL : 初始化失败
	* 
	*/
	HRESULT Initialize(const CHAR * szDirecory, const LONGLONG llTotalSize, const INT iFileSize, const INT iSaveVideoType);

	
	
public:

	HRESULT SetSendInterval(const INT iTimeInterval)
	{
		if (0 > iTimeInterval)
		{
			return E_INVALIDARG;
		}
		
		m_iSendInterval = iTimeInterval;
		return S_OK;
	}

	INT GetSendInterval(VOID)
	{
		return m_iSendInterval;
	}
	HRESULT SetVideoSaveEnable(const BOOL fVideoSaveEnable );
	
	BOOL GetVideoSaveEnable(VOID);


	SW_BEGIN_DISP_MAP(CSWH264HDDTransformFilter,CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 4)
		SW_DISP_METHOD(SetSendInterval, 1)
		SW_DISP_METHOD(SetSaveType, 1)
		SW_DISP_METHOD(SetVideoSaveEnable, 1)
		//SW_DISP_METHOD(StopHistoryFileTransmitting, 1)
		//SW_DISP_METHOD(GetHistoryFileTransmittingStatus, 2)
	SW_END_DISP_MAP();

	virtual HRESULT Receive(CSWObject* obj);	

protected:


	
	HRESULT FrameEnqueue(CSWImage* pImage);

	/**
	* @brief 把关键帧放到帧队列
	* @param [in] pImage : 关键帧对象指针
	* @param [out] piBufSize : 保存该帧数据大小
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	* @retval E_FAIL : 失败
	*/
	HRESULT KeyFrameEnqueue(CSWImage* pImage, INT* piBufSize);


	/**
	* @brief 把非关键帧放到帧队列
	* @param [in] pImage : 非关键帧对象指针
	* @param [out] piBufSize : 保存该帧数据大小
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	* @retval E_FAIL : 失败
	*/
	HRESULT NonKeyFrameEnqueue(CSWImage* pImage, INT* piBufSize);

	/**
	* @brief 把文件放置保存队列
	* @param [in] sTimeStamp : 文件的时间戳
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	* @retval E_FAIL : 失败
	*/
	HRESULT FileEnqueue(const SWPA_TIME& sTimeStamp);

	/**
	* @brief H264视频文件保存线程主函数
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	* @retval E_FAIL : 失败
	*/
	HRESULT OnHistoryFileSaving(VOID);


	

	/**
	* @brief H264视频历史文件发送线程主函数
	* @param [in] dwPinID : 历史文件发送Pin ID
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	* @retval E_FAIL : 失败
	*/
	HRESULT OnHistoryFileTransmitting(const DWORD dwPinID);


	
private:

	virtual HRESULT ReportStatus(LPCSTR szInfo=NULL);
	
	virtual HRESULT ClearBuffer();
	
	HRESULT SaveVideo(BYTE* pVideoFile);

	CSWImage * CreateVideo(PBYTE pbRawData);
	
	/**
	* @brief 设置输出pin的传输数据类型
	* @param [in] dwPinID : Pin ID，取值为0或1
	* @param [in] dwOutType : 输出类型，取值为H264HDD_TRANSMITTING_HISTORY 或H264HDD_TRANSMITTING_REALTIME
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_FAIL : 失败
	* 
	*/
	HRESULT OnSetTransmittingType(WPARAM wParam, LPARAM lParam);
	

	/**
	* @brief 开启某个Pin传输历史文件数据
	* @param [in] dwPinID : Pin ID，取值为0或1
	* @param [in] szTimeBegin : 历史文件的起始时间
	* @param [in] szTimeEnd : 历史文件的结束时间
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	* @retval E_FAIL : 失败
	* 
	*/
	HRESULT OnStartHistoryFileTransmitting(WPARAM wParam, LPARAM lParam);


	/**
	* @brief 停止某个Pin传输历史文件数据
	* @param [in] dwPinID : Pin ID，取值为0或1
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	* @retval E_FAIL : 失败
	* 
	*/
	HRESULT OnStopHistoryFileTransmitting(WPARAM wParam, LPARAM lParam);


	/**
	* @brief ?a??GB28181 Pin′?ê?àúê·???têy?Y
	* @param [in] dwPinID : Pin ID
	* @param [in] szTimeBegin : àúê·???tμ??eê?ê±??
	* @param [in] szTimeEnd : àúê·???tμ??áê?ê±??
	* @retval S_OK : 3é1|
	* @retval E_INVALIDARG : 2?êy·?·¨
	* @retval E_OUTOFMEMORY : ?ú′?2?×?
	* @retval E_FAIL : ê§°ü
	* 
	*/
	HRESULT OnStartGB28181FileTransmitting(WPARAM wParam, LPARAM lParam);


	/**
	* @brief í￡?1GB28181  Pin′?ê?àúê·???têy?Y
	* @param [in] dwPinID : Pin ID
	* @retval S_OK : 3é1|
	* @retval E_INVALIDARG : 2?êy·?·¨
	* @retval E_OUTOFMEMORY : ?ú′?2?×?
	* @retval E_FAIL : ê§°ü
	* 
	*/
	HRESULT OnStopGB28181FileTransmitting(WPARAM wParam, LPARAM lParam);


	HRESULT OnStartRealtimeTransmitting(WPARAM wParam, LPARAM lParam);
	
	HRESULT OnStopRealtimeTransmitting(WPARAM wParam, LPARAM lParam);
	

	/**
	* @brief 获取某个Pin传输历史文件数据的状态
	* @param [in] dwPinID : Pin ID，取值为0或1
	* @param [out] pdwStatus : 保存状态的指针，必须非空，其回传的值范围是
	* - H264HDD_HISTORY_TRANSMITTING_NOTSTARTED,
	* - H264HDD_HISTORY_TRANSMITTING_ONGOING,	
	* - H264HDD_HISTORY_TRANSMITTING_FINISHED,
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	* @retval E_FAIL : 失败
	* 
	*/
	HRESULT OnGetHistoryFileTransmittingStatus(WPARAM wParam, LPARAM lParam);

	
	HRESULT OnTriggerVideoSaving(WPARAM wParam, LPARAM lParam);

	
	HRESULT SendFrameToGB28181(const DWORD dwPinID, CSWImage * pFrame);
	

	SW_BEGIN_MESSAGE_MAP(CSWH264HDDTransformFilter, CSWMessage)
		SW_MESSAGE_HANDLER(MSG_H264HDD_FILTER_SET_TRANSMITTING_TYPE, OnSetTransmittingType)
		SW_MESSAGE_HANDLER(MSG_H264HDD_FILTER_HISTORY_FILE_TRANSMITTING_START, OnStartHistoryFileTransmitting)
		SW_MESSAGE_HANDLER(MSG_H264HDD_FILTER_HISTORY_FILE_TRANSMITTING_STOP, OnStopHistoryFileTransmitting)
		SW_MESSAGE_HANDLER(MSG_H264HDD_FILTER_REALTIME_TRANSMITTING_START, OnStartRealtimeTransmitting)
		SW_MESSAGE_HANDLER(MSG_H264HDD_FILTER_REALTIME_TRANSMITTING_STOP, OnStopRealtimeTransmitting)
		SW_MESSAGE_HANDLER(MSG_H264HDD_FILTER_GET_HISTORY_FILE_TRANSMITTING_STATUS, OnGetHistoryFileTransmittingStatus)
		SW_MESSAGE_HANDLER(MSG_H264HDD_FILTER_TRIGGER_VIDEO_SAVING, OnTriggerVideoSaving)
		SW_MESSAGE_HANDLER(MSG_H264HDD_FILTER_GB28181_FILE_TRANSMITTING_START, OnStartGB28181FileTransmitting)
		SW_MESSAGE_HANDLER(MSG_H264HDD_FILTER_GB28181_FILE_TRANSMITTING_STOP, OnStopGB28181FileTransmitting)
	SW_END_MESSAGE_MAP();
	
protected:
    HRESULT OnGB28181VideoSavingEnable (PVOID pvBuffer, INT iSize);
    HRESULT OnGB28181VideoSendScale(PVOID pvBuffer, INT iSize);
 
    SW_BEGIN_REMOTE_MESSAGE_MAP(CSWH264HDDTransformFilter, CSWMessage)
        SW_REMOTE_MESSAGE_HANDLER(MSG_H264HDD_FILTER_GB28181_VIDEOSAVING_ENABLE, OnGB28181VideoSavingEnable)
        SW_REMOTE_MESSAGE_HANDLER(MSG_H264HDD_FILTER_GB28181_VIDEOSEND_SCALE, OnGB28181VideoSendScale)
    SW_END_REMOTE_MESSAGE_MAP();

private:

	BOOL				m_fInited;
	
	//CSWList<BYTE*>		m_lstFrameBuf;	/// 帧队列
	CSWList<CSWImage*>		m_lstFrame;	/// 帧队列
	
	CSWList<CSWMemory*>	m_lstFile;		/// 文件保存队列	
	//CSWMutex*			m_pFileLock;	/// 文件保存队列锁	
	CSWSemaphore*			m_pFileLock;	/// 文件保存队列锁	
	DWORD				m_dwSaveQueueSize;
	CSWSemaphore*		m_pSemaSaveQueueSync;

	INT					m_iSendInterval;
	INT					m_iSendPauseTime;
	INT					m_iRecordSendInterval;

	CSWMemoryFactory * 	m_pcMemoryFactory;

	INT					m_iSaveVideoType;
	CSWList<DWORD>		m_lstTriggerTick;
	CSWSemaphore*       m_pcTickLock;	/// 触发存储时间队列锁	
	
	BOOL m_fGBVideoSaveCtlEnable;
	BOOL m_fGBVideoSaveRecord;
	int m_iGBVideoSkipBlock;    // 录像回放拖放，跳过的数据块数
	//CSWFile* m_VideoRecordList;
	INT m_iIsGB28181Saveing;
	CSWDateTime m_TempEndTime;
	BOOL				m_fGB28181Backward;
};

REGISTER_CLASS(CSWH264HDDTransformFilter)
	
#endif //__SW_H264HDD_TRANSFORM_FILTER_H__


