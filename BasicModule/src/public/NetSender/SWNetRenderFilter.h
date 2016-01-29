/**
* @file SWNetRenderFilter.h
* @brief 
*
*/

#ifndef _SW_NET_RENDER_FILTER_H_
#define _SW_NET_RENDER_FILTER_H_

#include "SWBaseFilter.h"
#include "LinkManager.h"
#include "DataTransmitter.h"

#include "SWStream.h"

#include "SWDateTime.h"
#include "SWList.h"
#include "SWThread.h"
#include "SWMutex.h"
#include "SWImage.h"

#include "SWTransmittingOpt.h"


class CSWNetRenderPin : public CSWBasePin
{	
	CLASSINFO(CSWNetRenderPin, CSWBasePin);
	
public:

	CSWNetRenderPin(CSWBaseFilter* pFilter, DWORD dwDir, DWORD dwID);
	
	virtual ~CSWNetRenderPin();

	
	HRESULT SetTransmittingType(const DWORD dwTransmittingType)
	{
		m_dwTransmittingType = dwTransmittingType;
		return S_OK;
	};
	
	
	DWORD GetTransmittingType()
	{
		return m_dwTransmittingType;
	};


	/**
	 * @brief 接收输入的数据包
	 * @param [in] pdu 接收数据包
	 * @成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT Receive(CSWObject* obj);

	HRESULT Occupy(){m_fIdle = FALSE; return S_OK;};

	HRESULT Deoccupy(){m_fIdle = TRUE; return S_OK;};

	BOOL IsIdle(VOID){return m_fIdle;};

private:
	BOOL	m_fInited;
	DWORD	m_dwTransmittingType;
	BOOL	m_fIdle;
	DWORD	m_dwID;
};



class CSWNetRenderFilter : public CSWBaseFilter
{
	CLASSINFO(CSWNetRenderFilter, CSWBaseFilter);
	
public :
	CSWNetRenderFilter();
	
	CSWNetRenderFilter(DWORD dwInCount, DWORD dwOutCount);
	
	virtual ~CSWNetRenderFilter();

	virtual CSWBasePin* CreateIn(DWORD n);
	virtual CSWBasePin* GetIn(DWORD n);
	
	HRESULT Initialize(const WORD wPassivePort, const WORD wActivePort,  const CHAR * szDstIp, const BOOL fSafeSaverEnabled);
	HRESULT Receive(CSWObject* obj, const DWORD dwInPinID, const DWORD dwDataType = TRANSMITTING_REALTIME);
	
	HRESULT Run();
	HRESULT Stop();
	
protected:
	
	virtual HRESULT ParseHandshakeXml(const CHAR * pszXmlBuf, CDataTransmitter* pTransmitter){	printf("Err: ParseHandshakeXml in CSWNetRenderFilter!!\n");return E_NOTIMPL;};
	
	virtual HRESULT SetUpstreamTransmittingType(const DWORD dwPinID, const DWORD dwType){	return E_NOTIMPL;};
	
	virtual HRESULT StartUpstreamHistoryFileTransmitting(const DWORD dwPinID, const CHAR * szTimeBegin, const CHAR * szTimeEnd, const DWORD dwCarID){	return E_NOTIMPL; };

	virtual HRESULT StopUpstreamHistoryFileTransmitting(const DWORD dwPinID){	return E_NOTIMPL;};

	virtual HRESULT StartUpstreamRealtimeTransmitting(const DWORD dwPinID){	return E_NOTIMPL; };

	virtual HRESULT StopUpstreamRealtimeTransmitting(const DWORD dwPinID){	return E_NOTIMPL;};
	
	virtual HRESULT GetUpstreamHistoryFileTransmittingStatus(const DWORD dwPinID, DWORD* pdwStatus){return E_NOTIMPL;};

	virtual CSWString GetConnectInfo(VOID)
	{
		CSWString str = "";
		if(m_pLinkManager != NULL)
		{
			return m_pLinkManager->GetConnectInfo();
		}
		return str;
	}
	

private:
	
	HRESULT LinkCallback(CSWStream * pStream, PVOID pvHandshakeBuf);
	static VOID* LinkCallbackProxy(CSWStream * pStream, PVOID* pvArg, PVOID pvHandshakeBuf);

	HRESULT OnTransmitterManage();
	
	static VOID* OnTransmitterManageProxy(VOID* pvArg);
	
private:
	
	BOOL	m_fInited;
	CLinkManager*	m_pLinkManager;	

	CSWThread*		m_pTransmitterManagerThread;	
	CSWList<CDataTransmitter*>			m_lstTransmitter;
	CSWMutex*		m_pTransmitterListLock;	

	CSWNetRenderPin  **m_pIn;
	WORD m_wLocalPort;
	
};

#endif //_SW_NET_RENDER_FILTER_H_


