/**
* @file LinkManager.h 
* @brief 数据发送模块
*
* 负责往给定的流发送H264、JPEG、RECORD等数据\n
*
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-04-11
* @version 1.0
*/

#ifndef _DATA_TRANSMITTER_H_
#define _DATA_TRANSMITTER_H_

#include "SWStream.h"
#include "SWList.h"
#include "SWThread.h"
#include "SWMutex.h"
#include "SWDateTime.h"
#include "SWImage.h"
#include "SWCarLeft.h"
#include "SWPosImage.h"
#include "SWRecord.h"
#include "SWNetOpt.h"
#include "SWTransmittingOpt.h"


class CDataTransmitter : public CSWObject
{
	CLASSINFO(CDataTransmitter, CSWObject)
public:
	
	/**
	* @brief 构造函数
	*/
	CDataTransmitter(CSWStream* pStream);

	/**
	* @brief 析构函数
	*/
	virtual ~CDataTransmitter();

	/**
	* @brief 初始化函数
	* @param [in] dwDataType : 传输的数据类型
	* @param [in] sBegin : 传输的开始时间
	* @param [in] sEnd : 传输的结束时间
	* @retval S_OK : 成功
	* @retval E_FAIL : 失败
	* @retval E_INVALIDARG : 参数非法
	* @retval E_OUTOFMEMORY : 内存不足
	*/
	HRESULT Initialize(const DWORD dwDataType, const DWORD dwTransmittingType, const SWPA_TIME sBegin, const SWPA_TIME sEnd);

	/**
	* @brief 设置关联的In Pin 的ID
	* @param [in] dwInPin : Pin 的ID
	* @retval S_OK : 成功
	* @retval E_NOTIMPL : 对象未初始化
	*/
	HRESULT SetPinID(const DWORD dwInPin)
	{
		return (m_fInited && (m_dwPinID = dwInPin)) ? S_OK : E_NOTIMPL;
	};

	/**
	* @brief 获取关联的In Pin 的ID
	* @retval 0 : 对象未初始化
	* @retval 正整数: 关联的Pin ID 
	*/
	DWORD GetPinID()
	{
		return m_fInited ? m_dwPinID : 0;
	};

	/**
	* @brief 发送数据包
	* @param [in] pObj : 被发送的数据包
	* @retval S_OK : 成功
	* @retval E_FAIL : 失败
	* @retval E_INVALIDARG : 参数非法
	*/
	HRESULT Send(CSWObject* pObj);

	/**
	* @brief 获取当前状态
	* @retval 枚举值；可能的取值有:
	* - 0 : STATE_READY, 
	* - 1 : STATE_RUNNING,
	* - 2 : STATE_STOPPED,
	*/
	DWORD GetState() 
	{
		return m_dwState;
	};

	
	/**
	* @brief 判断是否已经发送完成
	* @retval 0 : 未完成
	* @retval 1 : 已完成
	*/
	BOOL Done() 
	{
		return (STATE_STOPPED == GetState());
	};

	/**
	* @brief 获取关联的流指针
	* @retval NULL : 对象未初始化
	* @retval 有效指针 : 关联的Stream指针
	*/
	CSWStream* GetStream()
	{
		return (m_fInited ? m_pStream : NULL);
	};

	/**
	* @brief 获取关联的数据类型
	*/
	DWORD GetTransmittingType()
	{
		return m_fInited ? m_dwTransmittingType : 0;
	};

	/**
	* @brief 设置关联的数据类型
	*/
	HRESULT SetTransmittingType(const DWORD dwTransmittingType)
	{
		if (m_fInited)
		{
			m_dwTransmittingType = dwTransmittingType;
			return S_OK;
		}

		return E_NOTIMPL;
	};

	/**
	* @brief 获取发送历史数据的起始时间字符串
	*/
	CHAR* GetBeginTimeString()
	{
		return m_fInited ? m_szTimeBegin : NULL;
	};

	/**
	* @brief 设置发送历史数据的起始时间字符串
	*/
	HRESULT SetBeginTimeString(const CHAR* szTimeString)
	{
		if (NULL == szTimeString)
		{
			return E_INVALIDARG;
		}
		
		swpa_memset(m_szTimeBegin, 0, sizeof(m_szTimeBegin));
		swpa_strncpy(m_szTimeBegin, szTimeString, sizeof(m_szTimeBegin)-1);
		
		return S_OK;
	};

	/**
	* @brief 获取发送历史数据的结束时间字符串
	*/
	CHAR* GetEndTimeString()
	{
		return m_fInited ? m_szTimeEnd: NULL;
	};

	/**
	* @brief 获取发送历史数据的结束时间字符串
	*/
	HRESULT SetEndTimeString(const CHAR* szTimeString)
	{
		if (NULL == szTimeString)
		{
			return E_INVALIDARG;
		}

		swpa_memset(m_szTimeEnd, 0, sizeof(m_szTimeEnd));
		swpa_strncpy(m_szTimeEnd, szTimeString, sizeof(m_szTimeEnd)-1);
		
		return S_OK;
	}

	/**
	* @brief 获取发送历史数据的结束时间字符串
	*/
	DWORD GetCarID()
	{
		return m_fInited ? m_dwCarID: 0xFFFFFFFF;
	};

	/**
	* @brief 设置发送历史数据的结束时间字符串
	*/
	VOID SetCarID(const DWORD dwCarID)
	{
		m_dwCarID = dwCarID;
	}

	/**
	* @brief 设置Info的传输偏移
	*/
	VOID SetInfoOffset(const DWORD dwOffset)
	{
		m_dwInfoOffset = dwOffset;
	}

	/**
	* @brief 获取Info的传输偏移
	*/
	DWORD GetInfoOffset(VOID)
	{
		return m_dwInfoOffset;
	}

	
	/**
	* @brief 设置Data的传输偏移
	*/
	VOID SetDataOffset(const DWORD dwOffset)
	{
		m_dwDataOffset = dwOffset;
	}

	/**
	* @brief 获取Data的传输偏移
	*/
	DWORD GetDataOffset(VOID)
	{
		return m_dwDataOffset;
	}


	/**
	* @brief 获取Data的传输偏移
	*/
	DWORD GetDataType(VOID)
	{
		return m_dwDataType;
	}
	
	/**
	* @brief 设置是否发送违法录像
	*/
	VOID SetIllegalVideoFlag(BOOL fSendIllegalVideo)
	{
		m_fSendIllegalVideo = fSendIllegalVideo;
	}
	
	HRESULT Stop();

	/**
	* @brief 设置结果发送模块进入挂起状态
	*
	* 当一个链接断开之后，发送模块会自动进入该状态；
	* 该状态只会持续一分钟，用于等待上位机端重新建立连接；当一分钟超时后，模块认为连接是主动断开，发送模块停止
	* 在该状态中，发送模块仍然接收结果，并将其缓存；当链接重新建立后，会从缓存中取出数据发送；缓存队列长度为32
	* 
	* @note 该状态只对结果发送有效，与图片、视频等发送无关；
	*/
	HRESULT Pending(const BOOL fPend);
	
	/**
	* @brief 判断结果发送模块是否处于挂起状态
	*
	*/
	HRESULT IsPending(BOOL * fPending);

	/**
	* @brief 更新数据发送的流
	* 
	* 一般用于唤醒挂起状态中的发送模块，并使用新的流发送数据
	*
	*/
	HRESULT UpdateStream(CSWStream* pStream);
	
	/**
	* @brief 拷贝结果数据到SR2内存中做缓存
	* 
	* 若SR2内存被划并到共享内存，则可删除该函数
	*
	*/
	HRESULT CopyRecordToSRMemory(CSWRecord * pSrcRecord, CSWRecord** ppDestRecord);

protected:

	
	HRESULT BuildImageInfoXml(CSWPosImage* pPosImage, CHAR** pszXml, DWORD* pdwSize);

	HRESULT BuildVideoInfoXml(CSWImage* pVideo, CHAR** pszXml, DWORD* pdwSize);

	HRESULT BuildRecordInfoXml(CSWRecord* pRecord, CHAR** pszXml, DWORD* pdwSize);

	HRESULT BuildTrafficInfoXml(CSWRecord* pRecord, CHAR** pszXmlData, DWORD* pdwSize);

private:

	
	typedef struct __PACKAGE_HEADER
	{
		DWORD dwType;
		DWORD dwInfoSize;
		DWORD dwDataSize;
		
	}_PACKAGE_HEADER;
	
	
	typedef struct __PACKAGE
	{
		_PACKAGE_HEADER 	Header;
		PBYTE				pbInfo;
		PBYTE				pbData;
		
	}_PACKAGE;

	
	
	

	/**
	* @brief 发送数据的主函数
	*/
	HRESULT OnSendData();

	/**
	* @brief 发送数据的主函数的静态代理函数
	*/
	static VOID* OnSendDataProxy(VOID* pvArg);

	/**
	* @brief 发送心跳包
	*/
	HRESULT SendThrobPackage();

	/**
	* @brief 发送标志历史数据发送完成的包
	*/
	HRESULT SendHistoryEndPackage();

	/**
	* @brief 发送JPEG图像数据
	*
	*/
	HRESULT SendImage(CSWPosImage* pImage);

	/**
	* @brief 发送识别结果数据
	* @note 先打包到缓存，再一次性发送。支持断点续传，但会占用较多内存
	*/
	HRESULT SendRecord(CSWRecord* pRecord);

	/**
	* @brief 发送识别结果数据增强版
	* @note 不打包到缓存，逐段发送数据。不支持断点续传，不占用较多内存
	*/
	HRESULT SendRecordEx(CSWRecord* pRecord);

	/**
	* @brief 发送交通采集信息
	*/
	HRESULT SendTrafficInfo(CSWRecord* pRecord);

	/**
	* @brief 发送H264视频数据
	*/
	HRESULT SendVideo(CSWImage* pImage);

	/**
	* @brief 创建数据包中"Info"段的一个节点
	*/
	HRESULT MakeNode(const DWORD dwBlockID, const DWORD dwNodeLen, const PBYTE pbData, PBYTE pbAddr);

	/**
	* @brief 发送数据包
	*/
	HRESULT SendPackage(_PACKAGE * pPackage);

private:
	#define _TIME_STRING_LEN 32
	
	enum {
		STATE_READY = 0,
		STATE_RUNNING,
		STATE_STOPPED,
	
		STATE_COUNT
	};		
	
	BOOL		m_fInited;
	DWORD		m_dwPinID;
	DWORD		m_dwTransmittingType;
	DWORD		m_dwDataType;
	CSWStream*	m_pStream;
	CSWMutex*				m_pStreamLock;
	DWORD		m_dwState;
	CSWList<CSWObject*>		m_lstData;
	CSWMutex*				m_pListLock;
	CSWThread*				m_pThread;
	CSWSemaphore*			m_pSemaSync;
	DWORD		m_dwMaxHaltTimeMs;

	DWORD		m_dwQueueSize;
	CHAR		m_szTimeBegin[_TIME_STRING_LEN];	
	CHAR		m_szTimeEnd[_TIME_STRING_LEN];
	//SWPA_TIME	m_sTimeBegin;
	//SWPA_TIME	m_sTimeEnd;	
	SWPA_TIME	m_sTimeCur;

	DWORD		m_dwCarID;

	DWORD		m_dwInfoOffset;
	DWORD		m_dwDataOffset;
	
	BOOL 		m_fSendIllegalVideo;

	BOOL		m_fPending;
};

#endif //_DATA_TRANSMITTER_H_


