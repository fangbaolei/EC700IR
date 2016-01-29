/**
* @file SWJPEGNetRenderFilter.h
* @brief 
*
*/

#ifndef _SW_JPEG_NET_RENDER_FILTER_H_
#define _SW_JPEG_NET_RENDER_FILTER_H_

#include "SWNetRenderFilter.h"

class CSWJPEGNetRenderFilter : public CSWNetRenderFilter
{
	CLASSINFO(CSWJPEGNetRenderFilter, CSWNetRenderFilter);
	
public :
	CSWJPEGNetRenderFilter();
	
	//CSWJPEGNetRenderFilter(DWORD dwInCount, DWORD dwOutCount);
	
	virtual ~CSWJPEGNetRenderFilter();

	HRESULT Initialize();

	HRESULT Initialize(const WORD wPassivePort, const WORD wActivePort,  const CHAR * szDstIp, const BOOL fSafeSaverEnabled);


	
	SW_BEGIN_DISP_MAP(CSWJPEGNetRenderFilter,CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 0)
		//SW_DISP_METHOD(Initialize, 4)
	SW_END_DISP_MAP();
	
	
	virtual HRESULT SetUpstreamTransmittingType(const DWORD dwPinID, const DWORD dwType);

	virtual HRESULT StartUpstreamRealtimeTransmitting(const DWORD dwPinID){return S_OK;};

	virtual HRESULT StopUpstreamRealtimeTransmitting(const DWORD dwPinID){return S_OK;};
	
	/* // No JPEG history file 
	virtual HRESULT StartUpstreamHistoryFileTransmitting(const DWORD dwPinID, const CHAR * szTimeBegin, const CHAR * szTimeEnd, const DWORD dwCarID);
	
	virtual HRESULT StopUpstreamHistoryFileTransmitting(const DWORD dwPinID);
	
	virtual HRESULT GetUpstreamHistoryFileTransmittingStatus(const DWORD dwPinID, DWORD* pdwStatus);
	*/
	
protected:
	virtual HRESULT ParseHandshakeXml(const CHAR * pszXmlBuf, CDataTransmitter* pTransmitter);


private:
	
	BOOL	m_fInited;	
};

REGISTER_CLASS(CSWJPEGNetRenderFilter)

#endif //_SW_JPEG_NET_RENDER_FILTER_H_




