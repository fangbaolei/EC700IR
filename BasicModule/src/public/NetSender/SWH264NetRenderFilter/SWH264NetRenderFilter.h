/**
* @file SWH264NetRenderFilter.h
* @brief 
*
*/

#ifndef _SW_H264_NET_RENDER_FILTER_H_
#define _SW_H264_NET_RENDER_FILTER_H_

#include "SWNetRenderFilter.h"

class CSWH264NetRenderFilter : public CSWNetRenderFilter
{
	CLASSINFO(CSWH264NetRenderFilter, CSWNetRenderFilter);
	
public :
	CSWH264NetRenderFilter();
	
	//CSWH264NetRenderFilter(DWORD dwInCount, DWORD dwOutCount);
	
	virtual ~CSWH264NetRenderFilter();

	
	HRESULT Initialize();

	HRESULT Initialize(const WORD wPassivePort, const WORD wActivePort,  const CHAR * szDstIp, const BOOL fSafeSaverEnabled);

	
	SW_BEGIN_DISP_MAP(CSWH264NetRenderFilter,CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 0)
		//SW_DISP_METHOD(Initialize, 4)
	SW_END_DISP_MAP()
	
	
	virtual HRESULT SetUpstreamTransmittingType(const DWORD dwPinID, const DWORD dwType);
	
	virtual HRESULT StartUpstreamHistoryFileTransmitting(const DWORD dwPinID, const CHAR * szTimeBegin, const CHAR * szTimeEnd, const DWORD dwCarID);
	
	virtual HRESULT StopUpstreamHistoryFileTransmitting(const DWORD dwPinID);

	virtual HRESULT StartUpstreamRealtimeTransmitting(const DWORD dwPinID);

	virtual HRESULT StopUpstreamRealtimeTransmitting(const DWORD dwPinID);
	
	virtual HRESULT GetUpstreamHistoryFileTransmittingStatus(const DWORD dwPinID, DWORD* pdwStatus);

protected:
	
	virtual HRESULT ParseHandshakeXml(const CHAR * pszXmlBuf, CDataTransmitter* pTransmitter);


private:
	
	BOOL	m_fInited;	
};

REGISTER_CLASS(CSWH264NetRenderFilter)

#endif //_SW_H264_NET_RENDER_FILTER_H_


