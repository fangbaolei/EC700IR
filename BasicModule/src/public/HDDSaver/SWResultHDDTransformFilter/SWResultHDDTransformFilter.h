/**
* @file SWResultHDDTransformFilter.h 
* @brief Result保存模块
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-22
* @version 1.0
*/


#ifndef __SW_RESULT_HDDTRANSFORM_FILTER_H__
#define __SW_RESULT_HDDTRANSFORM_FILTER_H__

#include "SWHDDTransformFilter.h"

#include "SWCarLeft.h"
#include "SWRecord.h"

#include "SWList.h"
#include "BigFile.h"

#include "SWMessage.h"



/**
* @brief Result保存模块
*
*
* 两个功能：一个是保存数据，另一个是读取数据。
* 
*/
class CSWResultHDDTransformFilter : public CSWHDDTransformFilter, CSWMessage
{
	CLASSINFO(CSWResultHDDTransformFilter, CSWHDDTransformFilter)
		
public:
	//CSWResultHDDTransformFilter();

	CSWResultHDDTransformFilter();
	
	virtual ~CSWResultHDDTransformFilter();

	HRESULT Initialize(const CHAR * szDirecory, const LONGLONG llTotalSize, const INT iFileSize, const INT iSaveType);

	
	virtual HRESULT Receive(CSWObject* obj);
	
	
protected:

	virtual HRESULT ReportStatus(LPCSTR szInfo=NULL);
	
	SW_BEGIN_DISP_MAP(CSWResultHDDTransformFilter, CSWHDDTransformFilter)
		SW_DISP_METHOD(Initialize, 4)
		SW_DISP_METHOD(SetSaveType, 1)
		SW_DISP_METHOD(SetOutputType, 1)
		SW_DISP_METHOD(SetSendInterval, 1)
		SW_DISP_METHOD(SetFilterUnSurePeccancy, 1)
		SW_DISP_METHOD(ClearBuffer, 0)
	SW_END_DISP_MAP();

	
	virtual HRESULT OnHistoryFileSaving(VOID);
	
	virtual HRESULT OnHistoryFileTransmitting(const DWORD dwPinID);
	

	HRESULT OnSetTransmittingType(WPARAM wParam, LPARAM lParam);

	HRESULT OnStartHistoryFileTransmitting(WPARAM wParam, LPARAM lParam);
	
	HRESULT OnStopHistoryFileTransmitting(WPARAM wParam, LPARAM lParam);

	HRESULT OnStartRealtimeTransmitting(WPARAM wParam, LPARAM lParam);
	
	HRESULT OnStopRealtimeTransmitting(WPARAM wParam, LPARAM lParam);
	
	HRESULT OnGetHistoryFileTransmittingStatus(WPARAM wParam, LPARAM lParam);
	

	SW_BEGIN_MESSAGE_MAP(CSWResultHDDTransformFilter, CSWMessage)
		SW_MESSAGE_HANDLER(MSG_RESULTHDD_FILTER_SET_TRANSMITTING_TYPE, OnSetTransmittingType)
		SW_MESSAGE_HANDLER(MSG_RESULTHDD_FILTER_HISTORY_FILE_TRANSMITTING_START, OnStartHistoryFileTransmitting)
		SW_MESSAGE_HANDLER(MSG_RESULTHDD_FILTER_HISTORY_FILE_TRANSMITTING_STOP, OnStopHistoryFileTransmitting)
		SW_MESSAGE_HANDLER(MSG_RESULTHDD_FILTER_REALTIME_TRANSMITTING_START, OnStartRealtimeTransmitting)
		SW_MESSAGE_HANDLER(MSG_RESULTHDD_FILTER_REALTIME_TRANSMITTING_STOP, OnStopRealtimeTransmitting)
		SW_MESSAGE_HANDLER(MSG_RESULTHDD_FILTER_GET_HISTORY_FILE_TRANSMITTING_STATUS, OnGetHistoryFileTransmittingStatus)
	SW_END_MESSAGE_MAP();


	
	HRESULT SaveRecord(CSWRecord * pRecord);
	
	CSWRecord* CreateRecord(const PBYTE pbBuf, const INT iSize);

	
	BOOL RecordIsTrafficInfo(CSWRecord * pRecord);

	HRESULT SetOutputType(const INT iOutputType)
	{
		if (0 != iOutputType && 1 != iOutputType && 2 != iOutputType)
		{
			return E_INVALIDARG;
		}
		
		m_iOutputType = iOutputType;
		return S_OK;
	}
	
	INT GetOutputType(VOID)
	{
		return m_iOutputType;
	}

	HRESULT SetFilterUnSurePeccancy(const INT iFilterUnSurePeccancy)
	{
		if (m_fInited)
		{
			m_iFilterUnSurePeccancy = iFilterUnSurePeccancy;
			return S_OK;
		}

		return E_OBJ_NO_INIT;
	}

	INT GetFilterUnSurePeccancy(VOID)
	{
		return m_iFilterUnSurePeccancy;
	}


	virtual HRESULT ClearBuffer();
	
private:

	BOOL				m_fInited;
	CSWList<CSWRecord*>	m_lstFile;
	CSWMutex*			m_pFileMutex;

	CSWSemaphore*		m_pSemaSaveQueueSync;

	DWORD 				m_dwSaveQueueSize;
	INT					m_iOutputType;
	INT					m_iFilterUnSurePeccancy;

	CSWMemoryFactory * 	m_pcMemoryFactory;

	INT					m_iSaveResultType;
};

REGISTER_CLASS(CSWResultHDDTransformFilter)
	
#endif //__SW_RESULT_HDDTRANSFORM_FILTER_H__


