

#include "SWFC.h"
#include "SWH264NetRenderFilter.h"
#include "tinyxml.h"
#include "SWMessage.h"
#include "DataTransmitter.h"

#define PRINT SW_TRACE_DEBUG


#define HVXML_TIMEBEGIN "BeginTime"
#define HVXML_TIMEEND   "EndTime"

CSWH264NetRenderFilter::CSWH264NetRenderFilter()
	:CSWNetRenderFilter(3, 0)
	,m_fInited(FALSE)
{
	
	GetIn(0)->AddObject(CLASSID(CSWImage));
	GetIn(1)->AddObject(CLASSID(CSWImage));
	GetIn(2)->AddObject(CLASSID(CSWImage));
}



CSWH264NetRenderFilter::~CSWH264NetRenderFilter()
{
	if (!m_fInited)
	{
		return;
	}
	
	m_fInited = FALSE;
}


HRESULT CSWH264NetRenderFilter::Initialize()
{
	return Initialize(CAMERA_VIDEO_LINK_PORT, 0, NULL, 0);
}



HRESULT CSWH264NetRenderFilter::Initialize(const WORD wPassivePort, const WORD wActivePort,  const CHAR * szDstIp, const BOOL fSafeSaverEnabled)
{
	if (m_fInited)
	{
		return S_OK;
	}
	
	if (FAILED(CSWNetRenderFilter::Initialize(wPassivePort, wActivePort, szDstIp, fSafeSaverEnabled)))
	{
		PRINT("Err: failed to initialize CSWH264NetRenderFilter\n");
		return E_FAIL;
	}

	m_fInited = TRUE;
}



HRESULT CSWH264NetRenderFilter::ParseHandshakeXml(const CHAR * pszXmlBuf, CDataTransmitter* pTransmitter)
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
		return E_FAIL;
	}


	TiXmlDocument  XmlDoc;
	TiXmlElement * pEleRoot = NULL;	
	TiXmlDocument * pXmlOutputDoc = NULL;


	if (0 == XmlDoc.Parse((CHAR*)pszXmlBuf))
	{
		PRINT("Err: failed to parse XML:\n%s\n", (CHAR*)pszXmlBuf);
		hr = E_FAIL;
		goto OUT; 
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
			else if (0 == swpa_strcmp(szCmdname, "DownloadVideo")
					 || 0 == swpa_strcmp(szCmdname, "downloadvideo"))
			{
				const CHAR* szEnable = Ele->Attribute("Enable");
				if (NULL == szEnable)
				{
					PRINT("Err: NULL == szEnable\n");
					return E_FAIL;
				}

				if (0 == swpa_strcmp(szEnable, "0"))	//realtime
				{
					SWPA_TIME sBegin, sEnd;
					swpa_memset(&sBegin, 0, sizeof(sBegin));
					swpa_memset(&sEnd, 0, sizeof(sEnd));
					
					if (FAILED(pTransmitter->Initialize(DATATYPE_VIDEO, TRANSMITTING_REALTIME, sBegin, sEnd)))
					{
						PRINT("Err: failed to initialize pTransmitter\n");
						return E_FAIL;
					}

					PRINT("Info: H264 Realtime Handshake succeeded!!\n");
					return S_OK;
				}
				else if (0 == swpa_strcmp(szEnable, "1"))
				{
					SWPA_TIME sBegin, sEnd;
					SWPA_DATETIME_TM sRealBegin, SRealEnd;

					swpa_memset(&sRealBegin, 0, sizeof(sRealBegin));
					swpa_memset(&SRealEnd, 0, sizeof(SRealEnd));

					const CHAR * szBeginTime = Ele->Attribute(HVXML_TIMEBEGIN);
					const CHAR * szEndTime = Ele->Attribute(HVXML_TIMEEND);
					if (NULL == szBeginTime || NULL == szEndTime)
					{
						PRINT("Err: NULL == szBeginTime || NULL == szEndTime\n");
						return E_FAIL;
					}

					if (6 != swpa_sscanf(szBeginTime,"%04hd.%02hd.%02hd_%02hd:%02hd:%02hd", &sRealBegin.year, &sRealBegin.month, &sRealBegin.day, &sRealBegin.hour, &sRealBegin.min, &sRealBegin.sec)
						|| 6!= swpa_sscanf(szEndTime,"%04hd.%02hd.%02hd_%02hd:%02hd:%02hd", &SRealEnd.year, &SRealEnd.month, &SRealEnd.day, &SRealEnd.hour, &SRealEnd.min, &SRealEnd.sec))
					{
						PRINT("Err: failed to parse szBeginTime or szEndTime\n");
						return E_FAIL;
					}

					sRealBegin.hour += swpa_datetime_gettimezone();
					swpa_datetime_tm2time(sRealBegin, &sBegin);

					SRealEnd.hour += swpa_datetime_gettimezone();
					swpa_datetime_tm2time(SRealEnd, &sEnd);
					
					if (FAILED(pTransmitter->Initialize(DATATYPE_VIDEO, TRANSMITTING_HISTORY, sBegin, sEnd)))
					{
						PRINT("Err: failed to initialize pTransmitter\n");
						return E_FAIL;
					}

					pTransmitter->SetBeginTimeString(szBeginTime);
					pTransmitter->SetEndTimeString(szEndTime);

					PRINT("Info: H264 History Handshake succeeded!!\n");
					return S_OK;
				}
			}
		}
	}

OUT:
	
	return E_FAIL;
}


HRESULT CSWH264NetRenderFilter::SetUpstreamTransmittingType(const DWORD dwPinID, const DWORD dwType)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}
	
	return CSWMessage::SendMessage(MSG_H264HDD_FILTER_SET_TRANSMITTING_TYPE, (WPARAM)dwPinID, (LPARAM)dwType);
}


HRESULT CSWH264NetRenderFilter::StartUpstreamHistoryFileTransmitting(const DWORD dwPinID, const CHAR * szTimeBegin, const CHAR * szTimeEnd, const DWORD dwCarID)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	HISTORY_FILE_TRANSMITTING_PARAM sParam;
	SWPA_DATETIME_TM	sRealBegin, sRealEnd;

	swpa_memset(&sRealBegin, 0, sizeof(sRealBegin));
	swpa_memset(&sRealEnd, 0, sizeof(sRealEnd));	

	if (6 != swpa_sscanf(szTimeBegin,"%04hd.%02hd.%02hd_%02hd:%02hd:%02hd", &sRealBegin.year, &sRealBegin.month, &sRealBegin.day, &sRealBegin.hour, &sRealBegin.min, &sRealBegin.sec)
		|| 6!= swpa_sscanf(szTimeEnd,"%04hd.%02hd.%02hd_%02hd:%02hd:%02hd", &sRealEnd.year, &sRealEnd.month, &sRealEnd.day, &sRealEnd.hour, &sRealEnd.min, &sRealEnd.sec))
	{
		PRINT("Err: failed to parse szBeginTime or szEndTime\n");
		return E_FAIL;
	}

	sParam.psBeginTime = &sRealBegin;
	sParam.psEndTime = &sRealEnd;
	

	return CSWMessage::SendMessage(MSG_H264HDD_FILTER_HISTORY_FILE_TRANSMITTING_START, (WPARAM)dwPinID, (LPARAM)&sParam);
}


HRESULT CSWH264NetRenderFilter::StopUpstreamHistoryFileTransmitting(const DWORD dwPinID)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}
	
	return CSWMessage::SendMessage(MSG_H264HDD_FILTER_HISTORY_FILE_TRANSMITTING_STOP, (WPARAM)dwPinID, (LPARAM)0);
}



HRESULT CSWH264NetRenderFilter::StartUpstreamRealtimeTransmitting(const DWORD dwPinID)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}
	
	return CSWMessage::SendMessage(MSG_H264HDD_FILTER_REALTIME_TRANSMITTING_START, (WPARAM)dwPinID, (LPARAM)0);
}



HRESULT CSWH264NetRenderFilter::StopUpstreamRealtimeTransmitting(const DWORD dwPinID)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}
	
	return CSWMessage::SendMessage(MSG_H264HDD_FILTER_REALTIME_TRANSMITTING_STOP, (WPARAM)dwPinID, (LPARAM)0);
}




HRESULT CSWH264NetRenderFilter::GetUpstreamHistoryFileTransmittingStatus(const DWORD dwPinID, DWORD* pdwStatus)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}
	
	if (NULL == pdwStatus)
	{
		PRINT("Err: NULL == pdwStatus\n");
		return E_INVALIDARG;
	}

	

	return CSWMessage::SendMessage(MSG_H264HDD_FILTER_GET_HISTORY_FILE_TRANSMITTING_STATUS, (WPARAM)dwPinID, (LPARAM)pdwStatus);
}





