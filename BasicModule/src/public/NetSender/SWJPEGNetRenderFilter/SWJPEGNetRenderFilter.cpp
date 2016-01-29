

#include "SWFC.h"
#include "SWMessage.h"
#include "SWJPEGNetRenderFilter.h"
#include "tinyxml.h"
#include "SWPosImage.h"


#define PRINT SW_TRACE_DEBUG


#define HVXML_TIMEBEGIN "TimeBegin"
#define HVXML_TIMEEND   "TimeEnd"

CSWJPEGNetRenderFilter::CSWJPEGNetRenderFilter()
	:CSWNetRenderFilter(3, 0)
	,m_fInited(FALSE)
{
	
	GetIn(0)->AddObject(CLASSID(CSWPosImage));
	GetIn(1)->AddObject(CLASSID(CSWPosImage));
	GetIn(2)->AddObject(CLASSID(CSWPosImage));
}



CSWJPEGNetRenderFilter::~CSWJPEGNetRenderFilter()
{
	if (!m_fInited)
	{
		return;
	}
	
	m_fInited = FALSE;
}



HRESULT CSWJPEGNetRenderFilter::Initialize()
{
	return Initialize(CAMERA_IMAGE_LINK_PORT, 3400, NULL, 0);
}




HRESULT CSWJPEGNetRenderFilter::Initialize(const WORD wPassivePort, const WORD wActivePort,  const CHAR * szDstIp, const BOOL fSafeSaverEnabled)
{
	if (m_fInited)
	{
		return S_OK;
	}
	
	if (FAILED(CSWNetRenderFilter::Initialize(wPassivePort, wActivePort, szDstIp, fSafeSaverEnabled)))
	{
		PRINT("Err: failed to initialize CSWJPEGNetRenderFilter\n");
		return E_FAIL;
	}

	m_fInited = TRUE;
}



HRESULT CSWJPEGNetRenderFilter::ParseHandshakeXml(const CHAR * pszXmlBuf, CDataTransmitter* pTransmitter)
{
	HRESULT hr = S_OK;

	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}
	
	if (NULL == pszXmlBuf || NULL == pTransmitter)
	{
		PRINT("Err: NULL == pszXmlBuf || NULL == pTransmitter\n");
		return E_INVALIDARG;
	}
	
	TiXmlDocument  XmlDoc;
	TiXmlElement * pEleRoot = NULL;	
	TiXmlDocument * pXmlOutputDoc = NULL;

	if (0 == XmlDoc.Parse(pszXmlBuf))
	{	
		PRINT("Err: failed to parse xml:\n%s\n", pszXmlBuf);
		return E_FAIL;
	}

	
	pEleRoot = XmlDoc.RootElement();	
	if(NULL != pEleRoot)
	{
		TiXmlElement *Ele = pEleRoot->FirstChildElement("CmdName");
		if (NULL == Ele)
		{
			PRINT("Err: NULL == Ele\n");
			return E_FAIL;
		}
		else
		{
			const CHAR * szCmdname = Ele->GetText();
			if (NULL == szCmdname)
			{
				PRINT("Err: NULL == szCmdname\n");
				return E_FAIL;
			}
			else if (0 == swpa_strcmp(szCmdname, "DownloadImage")
					 || 0 == swpa_strcmp(szCmdname, "downloadimage")
					 || 0 == swpa_strcmp(szCmdname, "SetImgType"))
			{
				
				SWPA_TIME sBegin, sEnd;
				swpa_memset(&sBegin, 0, sizeof(sBegin));
				swpa_memset(&sEnd, 0, sizeof(sEnd));

				
				if (FAILED(pTransmitter->Initialize(DATATYPE_IMAGE, TRANSMITTING_REALTIME, sBegin, sEnd)))
				{
					PRINT("Err: failed to initialize pTransmitter\n");
					return E_FAIL;
				}
				
				return S_OK;
			}

			
		}
	}

	return E_FAIL;
}



HRESULT CSWJPEGNetRenderFilter::SetUpstreamTransmittingType(const DWORD dwPinID, const DWORD dwType)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	/*do nothing */
	return S_OK;//SendCommand(1, dwPinID, dwType);
}





