/**
* @file SWResultNetRenderFilter.h
* @brief 
*
*/

#ifndef _SW_RESULT_NET_RENDER_FILTER_H_
#define _SW_RESULT_NET_RENDER_FILTER_H_

#include "SWNetRenderFilter.h"
#include "tinyxml.h"

class CSWResultNetRenderFilter : public CSWNetRenderFilter
{
	CLASSINFO(CSWResultNetRenderFilter, CSWNetRenderFilter);
	
public :
	CSWResultNetRenderFilter();
		
	virtual ~CSWResultNetRenderFilter();

	
	HRESULT Initialize();

	HRESULT Initialize(const WORD wLocalPort, const WORD wDstPort,  const CHAR * szDstIp, const BOOL fSafeSaverEnabled);


	
	SW_BEGIN_DISP_MAP(CSWResultNetRenderFilter,CSWBaseFilter)
		//SW_DISP_METHOD(Initialize, 0)
		SW_DISP_METHOD(Initialize, 4)
	SW_END_DISP_MAP();
	
	
	virtual HRESULT SetUpstreamTransmittingType(const DWORD dwPinID, const DWORD dwType);
	
	virtual HRESULT StartUpstreamHistoryFileTransmitting(const DWORD dwPinID, const CHAR * szTimeBegin, const CHAR * szTimeEnd, const DWORD dwCarID);
	
	virtual HRESULT StopUpstreamHistoryFileTransmitting(const DWORD dwPinID);

	virtual HRESULT StartUpstreamRealtimeTransmitting(const DWORD dwPinID);

	virtual HRESULT StopUpstreamRealtimeTransmitting(const DWORD dwPinID);
	
	virtual HRESULT GetUpstreamHistoryFileTransmittingStatus(const DWORD dwPinID, DWORD* pdwStatus);

protected:
	virtual HRESULT ParseHandshakeXml(const CHAR * pszXmlBuf, CDataTransmitter* pTransmitter);

	HRESULT RealtimeHandshake(TiXmlElement* pEle, CDataTransmitter* pTransmitter);	
	
	HRESULT HistoryHandshake(TiXmlElement* pEle, CDataTransmitter* pTransmitter);


private:
	
	BOOL	m_fInited;	
};

REGISTER_CLASS(CSWResultNetRenderFilter)

#endif //_SW_RESULT_NET_RENDER_FILTER_H_



