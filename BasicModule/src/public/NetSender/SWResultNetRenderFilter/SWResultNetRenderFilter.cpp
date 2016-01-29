

#include "SWFC.h"
#include "SWResultNetRenderFilter.h"
#include "tinyxml.h"
#include "SWMessage.h"


#define PRINT SW_TRACE_DEBUG


#define HVXML_TIMEBEGIN "BeginTime"
#define HVXML_TIMEEND   "EndTime"
#define HVXML_INDEX	    "Index"


CSWResultNetRenderFilter::CSWResultNetRenderFilter()
	:CSWNetRenderFilter(3, 0)
	,m_fInited(FALSE)
{
	
	GetIn(0)->AddObject(CLASSID(CSWRecord));
	GetIn(1)->AddObject(CLASSID(CSWRecord));
	GetIn(2)->AddObject(CLASSID(CSWRecord));
}



CSWResultNetRenderFilter::~CSWResultNetRenderFilter()
{
	if (!m_fInited)
	{
		return;
	}
	
	m_fInited = FALSE;
}


HRESULT CSWResultNetRenderFilter::Initialize()
{
	return Initialize(CAMERA_RECORD_LINK_PORT, 0, NULL, 0);
}



HRESULT CSWResultNetRenderFilter::Initialize(const WORD wPassivePort, const WORD wActivePort,  const CHAR * szDstIp, const BOOL fSafeSaverEnabled)
{
	if (m_fInited)
	{
		return S_OK;
	}
	
	if (FAILED(CSWNetRenderFilter::Initialize(wPassivePort, wActivePort, szDstIp, fSafeSaverEnabled)))
	{
		PRINT("Err: failed to initialize CSWResultNetRenderFilter\n");
		return E_FAIL;
	}

	m_fInited = TRUE;
}




HRESULT CSWResultNetRenderFilter::RealtimeHandshake(TiXmlElement* pEle, CDataTransmitter* pTransmitter)
{
	if (NULL == pEle)
	{
		PRINT("Err: NULL == pEle\n");
		return E_FAIL;
	}
	
	SWPA_TIME sBegin, sEnd;
	swpa_memset(&sBegin, 0, sizeof(sBegin));
	swpa_memset(&sEnd, 0, sizeof(sEnd));

	PRINT("Info: got Result Realtime Handshake xml\n");
	if (FAILED(pTransmitter->Initialize(DATATYPE_RECORD, TRANSMITTING_REALTIME, sBegin, sEnd)))
	{
		PRINT("Err: failed to initialize pTransmitter\n");
		return E_FAIL;
	}

	return S_OK;
}



HRESULT CSWResultNetRenderFilter::HistoryHandshake(TiXmlElement* pEle, CDataTransmitter* pTransmitter)
{
	if (NULL == pEle)
	{
		PRINT("Err: NULL == pEle\n");
		return E_FAIL;
	}
	
	SWPA_TIME sBegin, sEnd;
	SWPA_DATETIME_TM sRealBegin, SRealEnd;

	swpa_memset(&sRealBegin, 0, sizeof(sRealBegin));
	swpa_memset(&SRealEnd, 0, sizeof(SRealEnd));

	const CHAR * szBeginTime = pEle->Attribute(HVXML_TIMEBEGIN);
	const CHAR * szEndTime = pEle->Attribute(HVXML_TIMEEND);

	if (NULL == szBeginTime)
	{
		PRINT("Err: NULL == szBeginTime \n");
		return E_FAIL;
	}

	CHAR strEndTime[16] = {0};
	if (4 != swpa_sscanf(szBeginTime, "%04hd.%02hd.%02hd_%02hd", &sRealBegin.year, &sRealBegin.month, &sRealBegin.day, &sRealBegin.hour))
	{
		PRINT("Err: failed to parse szBeginTime\n");
		return E_FAIL;
	}

	if (NULL == szEndTime || 0 == swpa_strcmp(szEndTime, "0"))
	{
		swpa_snprintf(strEndTime, sizeof(strEndTime)-1, "%04hd.%02hd.%02hd_%02hd", 2099, sRealBegin.month, sRealBegin.day, sRealBegin.hour);
		
		PRINT("Info: no TimeEnd info, set it to %s\n", strEndTime);
	}
	else
	{
		if (4 != swpa_sscanf(szEndTime,"%04hd.%02hd.%02hd_%02hd", &SRealEnd.year, &SRealEnd.month, &SRealEnd.day, &SRealEnd.hour))
		{
			PRINT("Err: failed to parse szEndTime\n");
			return E_FAIL;
		}

		swpa_strncpy(strEndTime, szEndTime, sizeof(strEndTime)-1);
	}


	sRealBegin.hour += swpa_datetime_gettimezone();
	swpa_datetime_tm2time(sRealBegin, &sBegin);

	SRealEnd.hour += swpa_datetime_gettimezone();
	swpa_datetime_tm2time(SRealEnd, &sEnd);

	if (FAILED(pTransmitter->Initialize(DATATYPE_RECORD, TRANSMITTING_HISTORY, sBegin, sEnd)))
	{
		PRINT("Err: failed to initialize pTransmitter\n");
		return E_FAIL;
	}


	pTransmitter->SetBeginTimeString(szBeginTime);
	pTransmitter->SetEndTimeString(strEndTime);

	const CHAR * szCarID = pEle->Attribute(HVXML_INDEX);
	if (NULL == szCarID)
	{
		PRINT("Err: NULL == szCarID\n");
		return E_FAIL;
	}

	PRINT("Info: CarID = %s\n", szCarID);

	pTransmitter->SetCarID(swpa_atoi(szCarID));


	/*
	pszAttribute = pCmdArgElement->Attribute("DevList");
    if (pszAttribute != NULL && strlen(pszAttribute) < sizeof(m_szDevList))
    {
        strcpy(m_szDevList, pszAttribute);
        HV_Trace(5, "Specify dev list:%s\n", m_szDevList);
    }
    pszAttribute = pCmdArgElement->Attribute("DataInfo");
    if (pszAttribute != NULL && strlen(pszAttribute) < 8)
    {
        m_dwDataInfo = atoi(pszAttribute);
        HV_Trace(5, "Specify DataInfo :%u\n", m_dwDataInfo);
    }*/
	const CHAR * szInfoOffset = pEle->Attribute("InfoOffset");
    if (szInfoOffset != NULL && swpa_strlen(szInfoOffset) < 8)
    {
        PRINT("Info: InfoOffset = %s\n", szInfoOffset);
		pTransmitter->SetInfoOffset((DWORD)swpa_atoi(szInfoOffset));
    }

	const CHAR * szDataOffset = pEle->Attribute("DataOffset");
    if (szDataOffset != NULL && swpa_strlen(szDataOffset) < 8)
    {
		PRINT("Info: DataOffset = %s\n", szDataOffset);
		pTransmitter->SetDataOffset((DWORD)swpa_atoi(szDataOffset));
    }

	return S_OK;
}



HRESULT CSWResultNetRenderFilter::ParseHandshakeXml(const CHAR * pszXmlBuf, CDataTransmitter* pTransmitter)
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
			else if (0 == swpa_strcmp(szCmdname, "DownloadRecord")
					 || 0 == swpa_strcmp(szCmdname, "downloadrecord"))
			{
				const CHAR* szEnable = Ele->Attribute("Enable");
				if (NULL == szEnable)
				{
					PRINT("Err: NULL == szEnable\n");
					return E_FAIL;
				}

				if (0 == swpa_strcmp(szEnable, "0"))	//realtime
				{
					if (FAILED(RealtimeHandshake(Ele, pTransmitter)))
					{
						PRINT("Err: Result Realtime Handshake -- Failed!!\n");
						return E_FAIL;
					}
					else
					{
						PRINT("Info: Result Realtime Handshake succeeded!!\n");
						return S_OK;
					}					
				}
				else if (0 == swpa_strcmp(szEnable, "1"))	//history
				{
					if (FAILED(HistoryHandshake(Ele, pTransmitter)))
					{
						PRINT("Err: Result History Handshake -- Failed!!\n");
						return E_FAIL;
					}
					else
					{
						PRINT("Info: Result History Handshake succeeded!!\n");
						return S_OK;
					}	
				}
				else
				{
					PRINT("Err: szEnable = %s, invalid\n", szEnable);
					return E_FAIL;
				}				
			}
			else
			{
				PRINT("Err: Unknown Cmdname: %s\n", szCmdname);
				return E_FAIL;
			}
		}
	}

OUT:
	
	return E_FAIL;
}



HRESULT CSWResultNetRenderFilter::SetUpstreamTransmittingType(const DWORD dwPinID, const DWORD dwType)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}
	
	return CSWMessage::SendMessage(MSG_RESULTHDD_FILTER_SET_TRANSMITTING_TYPE, (WPARAM)dwPinID, (LPARAM)dwType);
}


HRESULT CSWResultNetRenderFilter::StartUpstreamHistoryFileTransmitting(const DWORD dwPinID, const CHAR * szTimeBegin, const CHAR * szTimeEnd, const DWORD dwCarID)
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

	PRINT("Info: szTimeBegin = %s\n", szTimeBegin);
	PRINT("Info: szTimeEnd = %s\n", szTimeEnd);
	if (4 != swpa_sscanf(szTimeBegin,"%04hd.%02hd.%02hd_%02hd", &sRealBegin.year, &sRealBegin.month, &sRealBegin.day, &sRealBegin.hour)
		|| 4!= swpa_sscanf(szTimeEnd,"%04hd.%02hd.%02hd_%02hd", &sRealEnd.year, &sRealEnd.month, &sRealEnd.day, &sRealEnd.hour))
	{
		PRINT("Err: failed to parse szBeginTime or szEndTime\n");
		return E_FAIL;
	}

	sParam.psBeginTime = &sRealBegin;
	sParam.psEndTime = &sRealEnd;
	sParam.dwCarID = dwCarID;

	return CSWMessage::SendMessage(MSG_RESULTHDD_FILTER_HISTORY_FILE_TRANSMITTING_START, (WPARAM)dwPinID, (LPARAM)&sParam);
}


HRESULT CSWResultNetRenderFilter::StopUpstreamHistoryFileTransmitting(const DWORD dwPinID)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	
	return CSWMessage::SendMessage(MSG_RESULTHDD_FILTER_HISTORY_FILE_TRANSMITTING_STOP, (WPARAM)dwPinID, (LPARAM)0);
}



HRESULT CSWResultNetRenderFilter::StartUpstreamRealtimeTransmitting(const DWORD dwPinID)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}
	
	return CSWMessage::SendMessage(MSG_RESULTHDD_FILTER_REALTIME_TRANSMITTING_START, (WPARAM)dwPinID, (LPARAM)0);
}



HRESULT CSWResultNetRenderFilter::StopUpstreamRealtimeTransmitting(const DWORD dwPinID)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}
	
	return CSWMessage::SendMessage(MSG_RESULTHDD_FILTER_REALTIME_TRANSMITTING_STOP, (WPARAM)dwPinID, (LPARAM)0);
}




HRESULT CSWResultNetRenderFilter::GetUpstreamHistoryFileTransmittingStatus(const DWORD dwPinID, DWORD* pdwStatus)
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

	return CSWMessage::SendMessage(MSG_RESULTHDD_FILTER_GET_HISTORY_FILE_TRANSMITTING_STATUS, (WPARAM)dwPinID, (LPARAM)pdwStatus);
}





