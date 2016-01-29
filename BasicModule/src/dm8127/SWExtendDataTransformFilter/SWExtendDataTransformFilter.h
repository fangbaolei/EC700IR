#ifndef __SW_EXTEND_DATA_TRANSFORM_FILTER_H__
#define __SW_EXTEND_DATA_TRANSFORM_FILTER_H__
#include "SWBaseFilter.h"

class CSWExtendDataTransformFilter : public CSWBaseFilter
{
	CLASSINFO(CSWExtendDataTransformFilter, CSWBaseFilter)
public:
	CSWExtendDataTransformFilter();
	virtual ~CSWExtendDataTransformFilter();
protected:
	//virtual HRESULT Receive(CSWObject* obj);
	virtual HRESULT Run();
	virtual HRESULT Stop();
	
protected:
	
	HRESULT Initialize(PVOID pvParam);

	/**
	*@brief 图像附加数据回调函数
	*
	*/
	static void ExtendDataCallback(VOID *pvContext, VOID *pData, INT nLen);	

	HRESULT ProcessExtendData(VOID *pData, DWORD dwLen);

	HRESULT SendExtendData();
	static PVOID SendExtendDataProxy(PVOID pvArg);

	
private:
	SW_BEGIN_DISP_MAP(CSWExtendDataTransformFilter, CSWObject)
		SW_DISP_METHOD(Initialize, 1)
	SW_END_DISP_MAP();
	
private:
 	BOOL m_fInited;
	typedef struct 
	{
		DWORD dwTimeStamp;
		DWORD dwDataLen;
		PBYTE pData;
	}tExtendDataNode;
	
	DWORD m_dwQueueSize;
	CSWList<tExtendDataNode *> m_lstExtendData;
	//CSWMutex m_mutexLock;
	CSWSemaphore * m_pSemaLock;
	CSWSemaphore m_semaFrame;
	CSWThread	m_cSendThread;

};
REGISTER_CLASS(CSWExtendDataTransformFilter)
#endif //__SW_CENTAURUS_H264_ENCODE_FILTER_H__

