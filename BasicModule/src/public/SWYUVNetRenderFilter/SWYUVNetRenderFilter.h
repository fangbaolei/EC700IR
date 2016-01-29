#ifndef __SW_JPEG_NETRENDER_FILTER_TEST_H__
#define __SW_JPEG_NETRENDER_FILTER_TEST_H__
#include "SWBaseFilter.h"

class CSWYUVNetRenderFilter : public CSWBaseFilter
{
	CLASSINFO(CSWYUVNetRenderFilter, CSWBaseFilter)
public:
	CSWYUVNetRenderFilter();
	virtual ~CSWYUVNetRenderFilter();
protected:
	virtual HRESULT Run();
	virtual HRESULT Stop();
	virtual HRESULT Receive(CSWObject* obj);
protected:
	static  PVOID OnListen(PVOID pvParam);
	HRESULT  SendYUV422Image(CSWTCPSocket *pClient, CSWImage *pImage);
private:
	CSWThread m_cThreadServer;
	CSWTCPSocket m_tcpServer;
	INT       m_hClientList;
};
REGISTER_CLASS(CSWYUVNetRenderFilter)
#endif
