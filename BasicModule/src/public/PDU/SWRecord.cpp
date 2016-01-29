


#include "SWFC.h"
#include "SWRecord.h"


#define PRINT SW_TRACE_DEBUG

REGISTER_CLASS_LISTEN(CSWRecord)
CSWRecord::CSWRecord()
	:m_fInited(FALSE)
	,m_pXmlDocString(NULL)	
	,m_dwXmlDocSize(0)
	,m_dwImageTypeCount(7)
	,m_pImage(NULL)
	,m_pImageInfo(NULL)
	,m_dwRefTime(0)
	,m_iPTType(PT_NORMAL)
	,m_iUnSurePeccancy(0)
	,m_fTrafficInfo(FALSE)
	,m_dwVideoCount(0)
	,m_pvBuffer(NULL)
{
	CLASS_LISTEN_ADD(CSWRecord)
	swpa_memset(m_pVideo, 0, sizeof(m_pVideo));
}
	

CSWRecord::~CSWRecord()
{
	CLASS_LISTEN_RELEASE(CSWRecord)
	
	if (!m_fInited)
	{
		return ;
	}
	
	Deinitialize();
}



HRESULT CSWRecord::Initialize()
{
	if (m_fInited)
	{
		PRINT("Info: already inited \n");
		return S_OK;
	}

	m_pImage = (CSWImage**)swpa_mem_alloc(sizeof(CSWImage*) * GetImageTypeCount());
	if (NULL == m_pImage)
	{
		PRINT("Err: no memory for m_pImage\n");
		return E_OUTOFMEMORY;
	}
	swpa_memset(m_pImage, 0, sizeof(CSWImage*) * GetImageTypeCount());
	
	m_pImageInfo = (_RECORD_FILE_INFO**)swpa_mem_alloc(sizeof(_RECORD_FILE_INFO*) * GetImageTypeCount());
	if (NULL == m_pImageInfo)
	{
		PRINT("Err: no memory for m_pImageInfo\n");
		SAFE_MEM_FREE(m_pImage);
		return E_OUTOFMEMORY;
	}
	swpa_memset(m_pImageInfo, 0, sizeof(_RECORD_FILE_INFO*) * GetImageTypeCount());
	
	if (FAILED(m_cVideoLock.Create(1, 1)))
	{
		SW_TRACE_DEBUG("Err: failed to create video lock\n");
		SAFE_MEM_FREE(m_pImage);
		SAFE_MEM_FREE(m_pImageInfo);
		return E_FAIL;
	}

	if (FAILED(m_cBufferLock.Create(1, 1)))
	{
		SW_TRACE_DEBUG("Err: failed to create buffer lock\n");
		SAFE_MEM_FREE(m_pImage);
		SAFE_MEM_FREE(m_pImageInfo);
		m_cVideoLock.Delete();
		return E_FAIL;
	}
	
	m_fInited = TRUE;

	return S_OK;
}


HRESULT CSWRecord::Deinitialize()
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	if (NULL != m_pImage)
	{
		for (DWORD i=0; i<GetImageTypeCount(); i++)
		{
			SAFE_RELEASE(m_pImage[i]);
		}
		SAFE_MEM_FREE(m_pImage);
	}
	
	if (NULL != m_pImageInfo)
	{
		for (DWORD i=0; i<GetImageTypeCount(); i++)
		{
			SAFE_MEM_FREE(m_pImageInfo[i]);
		}
		SAFE_MEM_FREE(m_pImageInfo);
	}

	SAFE_MEM_FREE(m_pXmlDocString);


	m_cVideoLock.Pend();
#if 0	
	while (!m_lstVideo.IsEmpty())
	{
		CSWImage* pImage = (CSWImage*)m_lstVideo.RemoveHead();
		SW_TRACE_DEBUG("-----tobedeleted: Record 0x%x release video: 0x%x\n",
			this, pImage);
		SAFE_RELEASE(pImage);
	}
#endif
	for (INT i=0; i<sizeof(m_pVideo)/sizeof(m_pVideo[0]); i++)
	{
		//SW_TRACE_DEBUG("-----tobedeleted: Record 0x%x release video: 0x%x\n",
		//	this, m_pVideo[i]);
		SAFE_RELEASE(m_pVideo[i]);
	}
	m_cVideoLock.Post();

	m_cVideoLock.Delete();

	m_cBufferLock.Pend();
	SAFE_MEM_FREE(m_pvBuffer);
	m_cBufferLock.Post();
	
	m_cBufferLock.Delete();
	
	m_fInited = FALSE;

	return S_OK;
}




const CHAR* CSWRecord::GetXmlString()
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return NULL;
	}

	return m_pXmlDocString;
}



DWORD CSWRecord::GetXmlSize()
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return 0;
	}

	return m_dwXmlDocSize;
}

HRESULT CSWRecord::SetXml(CSWCarLeft* pCarLeft, const DWORD dwCarID)
{
	if (NULL == pCarLeft)
	{
		PRINT("Err: not inited yet\n");
		return E_INVALIDARG;
	}
	
	TiXmlDocument xmlDoc;
	
	if (FAILED((pCarLeft->BuildPlateString(xmlDoc))) 
		|| NULL == xmlDoc.RootElement())
	{
		PRINT("Err: Failed to parse CarLeft XML\n");
		return E_FAIL;
	}
	
	CHAR szText[32] = {0};

	TiXmlElement* pEleRoot = xmlDoc.RootElement();
	
	TiXmlElement* pResultSetEle = pEleRoot->FirstChildElement("ResultSet");
	if (NULL == pResultSetEle)
	{
		PRINT("Err: no ResultSet element found!\n");
		return E_FAIL;
	}
	
	TiXmlElement* pResultEle = pResultSetEle->FirstChildElement("Result");
	if (NULL == pResultEle)
	{
		PRINT("Err: no Result element found!\n");
		return E_FAIL;
	}
	

	//append a "CarID" element to the Result Element
	TiXmlElement* pCarIDEle = new TiXmlElement("CarID");
	if (NULL == pCarIDEle)
	{
		PRINT("Err: no memory for pCarIDEle\n");
		return E_OUTOFMEMORY;
	}	
	pCarIDEle->SetAttribute("value", dwCarID);	
	pResultEle->LinkEndChild(pCarIDEle);
	

	//append a "CarArriveTime" element to the Result Element
	DWORD dwTimeHigh = 0;
	DWORD dwTimeLow = 0;
	CSWDateTime::TimeConvert((DWORD)pCarLeft->GetCarArriveTime(), &dwTimeHigh, &dwTimeLow);
	TiXmlElement* pTimeHighEle = new TiXmlElement("TimeHigh");
	if (NULL == pTimeHighEle)
	{
		PRINT("Err: no memory for pTimeHighEle\n");
		return E_OUTOFMEMORY;
	}	
	pTimeHighEle->SetAttribute("value", dwTimeHigh);	
	pResultEle->LinkEndChild(pTimeHighEle);
	

	
	TiXmlElement* pTimeLowEle = new TiXmlElement("TimeLow");
	if (NULL == pTimeLowEle)
	{
		PRINT("Err: no memory for pTimeLowEle\n");
		return E_OUTOFMEMORY;
	}	
	pTimeLowEle->SetAttribute("value", dwTimeLow);	
	pResultEle->LinkEndChild(pTimeLowEle);
	


	//Create a "ResultExtInfo" element to the Root Element		
	TiXmlElement* pExtInfoEle = new TiXmlElement("ResultExtInfo");
	if (NULL == pExtInfoEle)
	{
		PRINT("Err: no memory for pExtInfoEle\n");
		return E_OUTOFMEMORY;
	}
	pEleRoot->LinkEndChild(pExtInfoEle);
	

	TiXmlElement* pTypeEle = new TiXmlElement("TransmittingType");
	if (NULL == pTypeEle)
	{
		PRINT("Err: no memory for pTypeEle\n");
		return E_OUTOFMEMORY;
	}
	pExtInfoEle->LinkEndChild(pTypeEle);
	

	swpa_snprintf(szText, sizeof(szText)-1, "Realtime");
	TiXmlText* pTypeEleText = new TiXmlText(szText);
	if (NULL == pTypeEleText)
	{
		PRINT("Err: no memory for pTypeEleText\n");
		return E_OUTOFMEMORY;
	}
	pTypeEle->LinkEndChild(pTypeEleText);
	
	

	for (INT i=0; i<pCarLeft->GetImageCount(); i++)
	{
		CSWImage* pImage = NULL;
		SW_RECT sRect;
		if (NULL == (pImage = pCarLeft->GetImage(i, &sRect)))
		{
			continue;
		}
		


		CHAR szEleName[16] = {0};
		swpa_snprintf(szEleName, sizeof(szEleName)-1, "Image%d", i);
		TiXmlElement* pImageInfoEle = new TiXmlElement(szEleName);
		if (NULL == pImageInfoEle)
		{
			PRINT("Err: no memory for pImageInfoEle %d\n", i);
			return E_OUTOFMEMORY;
		}		
		pExtInfoEle->LinkEndChild(pImageInfoEle);

		pImageInfoEle->SetAttribute("PlatePosLeft", sRect.left);
		pImageInfoEle->SetAttribute("PlatePosRight", sRect.right);
		pImageInfoEle->SetAttribute("PlatePosTop", sRect.top);
		pImageInfoEle->SetAttribute("PlatePosBottom", sRect.bottom);

		SW_COMPONENT_IMAGE sComponentImage;
		if (FAILED(pImage->GetImage(&sComponentImage)))
		{
			PRINT("Err: failed to get image component\n");
			return E_FAIL;
		}
		
		pImageInfoEle->SetAttribute("Time", pImage->GetRefTime());
		pImageInfoEle->SetAttribute("Width", pImage->GetWidth());
		pImageInfoEle->SetAttribute("Height", pImage->GetHeight());

		DWORD dwTimeHigh = dwTimeLow = 0;
		CSWDateTime::TimeConvert((DWORD)pImage->GetRefTime(), &dwTimeHigh, &dwTimeLow);
		pImageInfoEle->SetAttribute("TimeHigh", dwTimeHigh);
		pImageInfoEle->SetAttribute("TimeLow", dwTimeLow);
		

		if (1 == i) //big image, has face posistion info, "1" is the image id: RECORD_IMAGE_LAST_SNAPSHOT
		{
			DWORD dwFaceCount = pCarLeft->GetFaceCount();

			TiXmlElement* pEleFaceInfo = new TiXmlElement("FaceInfo"); //Note: will be delete in its parent's destructor
			if (NULL == pEleFaceInfo)
			{
				PRINT("Err: no memory for pEleFaceInfo\n");
				return E_OUTOFMEMORY;
			}
			

			pImageInfoEle->LinkEndChild(pEleFaceInfo);

			pEleFaceInfo->SetAttribute("Count", dwFaceCount);
			
			for (DWORD j=0; j<dwFaceCount; j++)
			{
				CHAR szFaceEleName[16] = {0};
				swpa_snprintf(szFaceEleName, sizeof(szFaceEleName)-1, "Face%d", j);
				TiXmlElement* pFaceRectEle = new TiXmlElement(szFaceEleName);
				if (NULL == pFaceRectEle)
				{
					PRINT("Err: no memory for pFaceRectEle %d\n", j);
					return E_OUTOFMEMORY;
				}
				pEleFaceInfo->LinkEndChild(pFaceRectEle);
				

				SW_RECT sFaceRect = pCarLeft->GetFaceRect(j);
				pFaceRectEle->SetAttribute("Left", sFaceRect.left);
				pFaceRectEle->SetAttribute("Right", sFaceRect.right);
				pFaceRectEle->SetAttribute("Top", sFaceRect.top);
				pFaceRectEle->SetAttribute("Bottom", sFaceRect.bottom);
			}
			
		}

	}
	

	return SetXml(&xmlDoc);
}



HRESULT CSWRecord::SetXml(const TiXmlDocument* pXmlDoc)
{

	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}
	

	if (NULL == pXmlDoc)
	{
		PRINT("Err: NULL == pXmlDoc\n");
		return E_INVALIDARG;
	}
	

	TiXmlPrinter XmlPrinter;
	pXmlDoc->Accept(&XmlPrinter);
	
	return SetXml(XmlPrinter.CStr());
	
}


HRESULT CSWRecord::SetXml(const CHAR* pszXmlString)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}
	

	if (NULL == pszXmlString)
	{
		PRINT("Err: NULL == pszXmlString\n");
		return E_INVALIDARG;
	}
	

	if (NULL != m_pXmlDocString)
	{		
		SAFE_MEM_FREE(m_pXmlDocString);		

		m_dwXmlDocSize = 0;
	}
	
	m_dwXmlDocSize = swpa_strlen(pszXmlString) + 1;
	m_pXmlDocString = (CHAR*)swpa_mem_alloc(m_dwXmlDocSize);
	if (NULL == m_pXmlDocString)
	{
		PRINT("Err: no memory for m_pXmlDocString\n");
		return E_OUTOFMEMORY;
	}
	
	swpa_memset(m_pXmlDocString, 0x0, m_dwXmlDocSize);
	swpa_strncpy(m_pXmlDocString, pszXmlString, m_dwXmlDocSize-1);	

	//PRINT("Info: m_pXmlDocString = %s\n", m_pXmlDocString);
	
	return S_OK;
}




CSWImage * CSWRecord::GetImage(const DWORD dwID)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return NULL;
	}

	if (dwID >= GetImageTypeCount())
	{
		PRINT("Err: dwID >= GetImageTypeCount()\n");
		return NULL;
	}

	return m_pImage[dwID];
}



HRESULT CSWRecord::SetImage(const DWORD dwID, CSWImage * pImage)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	if (NULL == pImage)
	{
		PRINT("Err: NULL == pImage\n");
		return E_INVALIDARG;
	}

	if (dwID >= GetImageTypeCount())
	{
		PRINT("Err: dwID >= GetImageTypeCount()\n");
		return E_INVALIDARG;
	}

	SAFE_RELEASE(m_pImage[dwID]);

	pImage->AddRef();
	m_pImage[dwID] = pImage;	

	return S_OK;
}





_RECORD_FILE_INFO * CSWRecord::GetImageInfo(const DWORD dwID)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return NULL;
	}

	if (dwID >= GetImageTypeCount())
	{
		PRINT("Err: dwID >= GetImageTypeCount()\n");
		return NULL;
	}

	return m_pImageInfo[dwID];
}




HRESULT CSWRecord::SetImageInfo(const DWORD dwID, const _RECORD_FILE_INFO * pImageInfo)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	if (NULL == pImageInfo)
	{
		PRINT("Err: NULL == pImage\n");
		return E_INVALIDARG;
	}

	if (dwID >= GetImageTypeCount())
	{
		PRINT("Err: dwID >= GetImageTypeCount()\n");
		return E_INVALIDARG;
	}


	SAFE_MEM_FREE(m_pImageInfo[dwID]);
	
	m_pImageInfo[dwID] = (_RECORD_FILE_INFO *)swpa_mem_alloc(sizeof(_RECORD_FILE_INFO));
	if (NULL == m_pImageInfo[dwID])
	{
		PRINT("Err: no memory for m_pImageInfo[%d]\n", dwID);
		return E_OUTOFMEMORY;
	}

	swpa_memcpy(m_pImageInfo[dwID], pImageInfo, sizeof(*pImageInfo));	

	return S_OK;
}



HRESULT CSWRecord::SetRefTime(const DWORD dwRefTime)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	m_dwRefTime = dwRefTime;

	return S_OK;
}




DWORD CSWRecord::GetRefTime()
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return 0;
	}

	return m_dwRefTime;
}

HRESULT CSWRecord::SetCarArriveTime(const DWORD dwRefTime)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	m_dwCarArriveTime = dwRefTime;

	return S_OK;
}

DWORD CSWRecord::GetCarArriveTime()
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return 0;
	}

	return m_dwCarArriveTime;
}


HRESULT CSWRecord::SetPTType(const INT iPTType)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	m_iPTType = iPTType;

	return S_OK;
}




INT CSWRecord::GetPTType()
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return -1;
	}

	return m_iPTType;
}


HRESULT CSWRecord::SetUnSurePeccancy(const INT iUnSurePeccancy)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	m_iUnSurePeccancy = iUnSurePeccancy;

	return S_OK;
}




INT CSWRecord::GetUnSurePeccancy()
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return -1;
	}

	return m_iUnSurePeccancy;
}



BOOL CSWRecord::IsNormal()
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return 0;
	}

	return PT_NORMAL == m_iPTType;
}





HRESULT CSWRecord::SetTransmittingType(const CHAR* szType)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_OBJ_NO_INIT;
	}

	if (NULL == szType)
	{
		PRINT("Err: NULL == szType\n");
		return E_INVALIDARG;
	}

	if (NULL == m_pXmlDocString)
	{
		PRINT("Err: NULL == m_pXmlDocString\n");
		return E_FAIL;
	}

	TiXmlDocument xmlDoc;
	
	if (FAILED(xmlDoc.Parse((const CHAR*)m_pXmlDocString)) 
		|| NULL == xmlDoc.RootElement())
	{
		PRINT("Err: Failed to parse m_pXmlDocString XML\n");
		return E_FAIL;
	}
	
	CHAR szText[32] = {0};

	TiXmlElement* pEleRoot = xmlDoc.RootElement();
	for (TiXmlElement* pEle = pEleRoot->FirstChildElement(); NULL != pEle; pEle = pEle->NextSiblingElement())
	{
		if (NULL != pEle->Value()
			&& 0 == swpa_strcmp("ResultExtInfo", pEle->Value()))
		{
			for (TiXmlElement* pSubEle = pEle->FirstChildElement(); NULL != pSubEle; pSubEle = pSubEle->NextSiblingElement())
			{
				if (NULL != pSubEle->Value()
					&& 0 == swpa_strcmp("TransmittingType", pSubEle->Value()))
				{
					TiXmlNode* pNodeOld = pSubEle->FirstChild();
					TiXmlText* pTypeEleText = new TiXmlText(szType);
					if (NULL == pTypeEleText)
					{
						PRINT("Err: no memory for pTypeEleText\n");
						return E_OUTOFMEMORY;
					}
					if (NULL == pSubEle->ReplaceChild(pNodeOld, *pTypeEleText))
					{
						PRINT("Err: failed to set Record Typt to %s\n", szType);
						delete pTypeEleText;
						return E_FAIL;
					}					
					
					delete pTypeEleText;

					return SetXml(&xmlDoc);
				}
			}
		}
	}
	
	PRINT("Err: found no TransmittingType Element\n");
	return E_FAIL;
}




HRESULT CSWRecord::SetCarID(const DWORD dwCarID)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_OBJ_NO_INIT;
	}

	if (NULL == m_pXmlDocString)
	{
		PRINT("Err: NULL == m_pXmlDocString\n");
		return E_FAIL;
	}

	TiXmlDocument xmlDoc;
	
	if (FAILED(xmlDoc.Parse((const CHAR*)m_pXmlDocString)) 
		|| NULL == xmlDoc.RootElement())
	{
		PRINT("Err: Failed to parse m_pXmlDocString XML\n");
		return E_FAIL;
	}
	
	CHAR szText[32] = {0};

	TiXmlElement* pEleRoot = xmlDoc.RootElement();
	for (TiXmlElement* pEle = pEleRoot->FirstChildElement(); NULL != pEle; pEle = pEle->NextSiblingElement())
	{
		if (NULL != pEle->Value()
			&& 0 == swpa_strcmp("ResultSet", pEle->Value()))
		{
			for (TiXmlElement* pSubEle = pEle->FirstChildElement(); NULL != pSubEle; pSubEle = pSubEle->NextSiblingElement())
			{
				if (NULL != pSubEle->Value()
					&& 0 == swpa_strcmp("Result", pSubEle->Value()))
				{
					for (TiXmlElement* pCarIDEle = pSubEle->FirstChildElement(); NULL != pCarIDEle; pCarIDEle = pCarIDEle->NextSiblingElement())
					{
						if (NULL != pCarIDEle->Value()
							&& 0 == swpa_strcmp("CarID", pCarIDEle->Value()))
						{						
							pCarIDEle->SetAttribute("value", dwCarID);
							return SetXml(&xmlDoc);
						}
					}
				}
			}
		}
	}
	
	PRINT("Err: found no CarID Element\n");
	return E_FAIL;
}



HRESULT CSWRecord::SetVideo(const DWORD& dwID, CSWImage* pImage)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_OBJ_NO_INIT;
	}
	
	if (dwID > GetVideoCount())
	{
		SW_TRACE_DEBUG("Err: invalid arg dwID(%d)\n", dwID);
		return E_INVALIDARG;
	}

	if (NULL == pImage)
	{
		SW_TRACE_DEBUG("Err: invalid arg pImage(%d)\n", pImage);
		return E_INVALIDARG;
	}

	//SW_TRACE_DEBUG("-----tobedeleted\n");
	
	CSWImage * pOldImage = NULL;
	m_cVideoLock.Pend();
	#if 0
	SW_POSITION pos = m_lstVideo.GetHeadPosition();
	for (INT i=0; i<dwID; i++)
	{
		if (m_lstVideo.IsValid(pos))
		{
			m_lstVideo.GetNext(pos);
		}
	}

	pOldImage = m_lstVideo.GetAt(pos);
	if (NULL != pOldImage)
	{
		SAFE_RELEASE(pOldImage);
	}

	SAFE_ADDREF(pImage);
	SW_TRACE_DEBUG("-----tobedeleted\n");
	m_lstVideo.SetAt(pos, pImage);
	#endif

	if (NULL != m_pVideo[dwID])
	{
		SAFE_RELEASE(m_pVideo[dwID]);
	}
	else
	{
		m_dwVideoCount++;
	}
	SAFE_ADDREF(pImage);
	m_pVideo[dwID] = pImage;
	
	//SW_TRACE_DEBUG("-----tobedeleted: Record 0x%x add video #%d: 0x%x\n",
	//	this, dwID, pImage);
	
	m_cVideoLock.Post();

	return S_OK;
}

HRESULT CSWRecord::GetVideo(const DWORD& dwID, CSWImage** ppImage)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_OBJ_NO_INIT;
	}
	
//	SW_TRACE_DEBUG("-----tobedeleted(%d,%d)\n", dwID ,GetVideoCount());
	if (dwID >= GetVideoCount())
	{
		SW_TRACE_DEBUG("Err: invalid arg dwID(%d)\n", dwID);
		return E_INVALIDARG;
	}

	if (NULL == ppImage)
	{
		SW_TRACE_DEBUG("Err: invalid arg ppImage(%d)\n", ppImage);
		return E_INVALIDARG;
	}

	CSWImage * pImage = NULL;
	m_cVideoLock.Pend();
	#if 0
	SW_POSITION pos = m_lstVideo.GetHeadPosition();
	for (INT i=0; i<dwID+1; i++)
	{
		if (m_lstVideo.IsValid(pos))
		{
			pImage = m_lstVideo.GetNext(pos);
		}
	}
	#endif
	pImage = m_pVideo[dwID];
	m_cVideoLock.Post();
	
	*ppImage = pImage;

	return S_OK;

}


DWORD CSWRecord::GetVideoCount()
{
	m_cVideoLock.Pend();
	//DWORD dwCount = m_lstVideo.GetCount();
	DWORD dwCount = m_dwVideoCount;
	m_cVideoLock.Post();

	return dwCount;
}


HRESULT CSWRecord::LockBuffer()
{
	if (!m_fInited)
	{
		return E_OBJ_NO_INIT;
	}

	return m_cBufferLock.Pend();
}


HRESULT CSWRecord::UnlockBuffer()
{
	if (!m_fInited)
	{
		return E_OBJ_NO_INIT;
	}

	return m_cBufferLock.Post();
}



HRESULT CSWRecord::GetBuffer(const DWORD dwSize, PVOID* ppvBuffer)
{
	if (NULL == ppvBuffer)
	{
		return E_INVALIDARG;
	}

	*ppvBuffer = NULL;
	if (!m_fInited)
	{
		return E_OBJ_NO_INIT;
	}

	HRESULT hr = S_OK;
	
	if (NULL == m_pvBuffer)
	{
	    m_pvBuffer = swpa_mem_alloc(dwSize);
		if (NULL == m_pvBuffer)
		{
			return E_OUTOFMEMORY;
		}
		swpa_memset(m_pvBuffer, 0, dwSize);
		hr = S_FALSE;
	}
	*ppvBuffer = m_pvBuffer;

	return hr;
}



