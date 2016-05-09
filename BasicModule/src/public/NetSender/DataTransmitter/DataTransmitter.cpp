/**
* @file LinkManager.cpp
* @brief 数据发送模块的实现
*
* 负责往给定的流发送H264、JPEG、RECORD等数据\n
*
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-04-11
* @version 1.0
*/

#include "SWFC.h"
#include "DataTransmitter.h"
#include "SWImage.h"
#include "SWPosImage.h"
#include "SWCarLeft.h"
#include "tinyxml.h"


#define PRINT SW_TRACE_DEBUG

//#define DATATRANSMITTER_MAX_HALT_SECOND 4 //in sec


CDataTransmitter::CDataTransmitter(CSWStream* pStream)
	:m_fInited(FALSE)
	,m_pStream(pStream)
	,m_pListLock(NULL)
	,m_pThread(NULL)
	,m_dwPinID(0)
	,m_dwTransmittingType(TRANSMITTING_TYPE_COUNT)
	,m_dwDataType(DATATYPE_COUNT)
	,m_dwState(STATE_READY)
	,m_pSemaSync(NULL)
	,m_dwQueueSize(5)
	,m_dwMaxHaltTimeMs(4000)
	,m_dwInfoOffset(0)
	,m_dwDataOffset(0)
	,m_fSendIllegalVideo(TRUE)
	,m_pStreamLock(NULL)
	,m_fPending(FALSE)
{	

	SAFE_ADDREF(m_pStream);
	
	//swpa_memset(&m_sTimeBegin, 0, sizeof(m_sTimeBegin));
	//swpa_memset(&m_sTimeEnd, 0, sizeof(m_sTimeEnd));
	swpa_memset(m_szTimeBegin, 0, sizeof(m_szTimeBegin));
	swpa_memset(m_szTimeEnd, 0, sizeof(m_szTimeEnd));
}

CDataTransmitter::~CDataTransmitter()
{	
	if (!m_fInited)
	{
		return ;
	}

	Stop();

	//m_dwDataType = DATATYPE_COUNT;

	
	m_dwState = STATE_STOPPED;
	

	SAFE_RELEASE(m_pThread);

	m_pListLock->Lock();
	while (!m_lstData.IsEmpty())
	{
		CSWObject * pObj = (CSWObject *)m_lstData.GetHead();
		m_lstData.RemoveHead();
		if (NULL != pObj)
		{
			pObj->Release();
		}		
	}
	m_pListLock->Unlock();

	m_dwQueueSize = 0;
	if (NULL != m_pListLock)
	{
		delete m_pListLock;
		m_pListLock = NULL;
	}	

	SAFE_RELEASE(m_pSemaSync);

	m_pStreamLock->Lock();
	SAFE_RELEASE(m_pStream);
	m_pStreamLock->Unlock();

	SAFE_RELEASE(m_pStreamLock);

	m_fInited = FALSE;

	PRINT("Info: DataTransmitter (Type=%d) de-inited!\n", m_dwDataType);
}


HRESULT CDataTransmitter::Initialize(const DWORD dwDataType, const DWORD dwTransmittingType, const SWPA_TIME sBegin, const SWPA_TIME sEnd)
{
	if (m_fInited)
	{
		return S_OK;
	}

	if (NULL == m_pStream)
	{
		PRINT("Err: NULL == m_pStream\n");
		return E_INVALIDARG;
	}

	if (dwDataType >= DATATYPE_COUNT)
	{
		PRINT("Err: dwDataType >= DATATYPE_COUNT\n");
		return E_INVALIDARG;
	}
	
	m_dwDataType = dwDataType;
	
	if (dwTransmittingType >= TRANSMITTING_TYPE_COUNT)
	{
		PRINT("Err: dwTransmittingType >= TRANSMITTING_TYPE_COUNT\n");
		return E_INVALIDARG;
	}
	
	
	m_dwTransmittingType = dwTransmittingType;

	if (TRANSMITTING_HISTORY == m_dwTransmittingType)
	{
		m_dwQueueSize = 1;
	}
	else
	{
		if (DATATYPE_IMAGE == m_dwDataType)
		{			
			m_dwQueueSize = 3;
		}
		else if (DATATYPE_VIDEO == m_dwDataType)
		{
			m_dwQueueSize = 50;
		}
		else if (DATATYPE_RECORD == m_dwDataType)
		{
			m_dwQueueSize = 32;
		}
	}

	if (NULL == m_pListLock)
	{
		m_pListLock = new CSWMutex();
		if (NULL == m_pListLock)
		{
			PRINT("Err: no enough memory for m_pListLock\n");
			return E_OUTOFMEMORY;
		}
	}

	if (NULL == m_pStreamLock)
	{
		m_pStreamLock = new CSWMutex();
		if (NULL == m_pStreamLock)
		{
			PRINT("Err: no enough memory for m_pStreamLock\n");
			return E_OUTOFMEMORY;
		}
	}	

	if (NULL == m_pSemaSync)
	{
		m_pSemaSync = new CSWSemaphore(0, m_dwQueueSize);
		if (NULL == m_pSemaSync)
		{
			PRINT("Err: no enough memory for m_pSemaSync\n");
			return E_OUTOFMEMORY;
		}
	}	
	

	if (NULL == m_pThread)
	{
		m_pThread = new CSWThread();
		if (NULL == m_pThread)
		{
			PRINT("Err: no enough memory for m_pThread\n");
			return E_OUTOFMEMORY;
		}
	}
	
	// 设置最大的队列长度。
	m_lstData.SetMaxCount(m_dwQueueSize);
	
	m_fInited = TRUE;
	
	m_dwState = STATE_RUNNING;
	
	return m_pThread->Start(OnSendDataProxy, (VOID*)this);

}


HRESULT CDataTransmitter::Stop()
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	if (Done())
	{
		return S_OK;
	}

	m_dwState = STATE_STOPPED;
	
	if (TRANSMITTING_HISTORY == m_dwTransmittingType)
	{
		for(;;)
		{
			m_pListLock->Lock();
			BOOL fEmpty = m_lstData.IsEmpty();
			m_pListLock->Unlock();
			
			if(!fEmpty)
			{
				PRINT("Info: wait till all history objects to be sent...\n");
				CSWApplication::Sleep(1000);
			}
			else
			{
				break;
			}
		}
	}

	HRESULT hr = S_OK;
	if (NULL != m_pThread)
	{
		hr = m_pThread->Stop();
	}

	return hr;
}



HRESULT CDataTransmitter::Send(CSWObject* pObj)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	if (NULL == pObj)
	{
		PRINT("Err: NULL == pObj\n");
		return E_INVALIDARG;
	}

	if (Done())
	{
		//do nothing when the Sending Thread is stopped.
		PRINT("Warning: Transmitter is stopped! discards this %s obj\n", pObj->Name());
		return S_OK;
	}

	//PRINT("Info: enqueue an %s obj\n", pObj->Name());

	CSWRecord* pNewRecord = NULL;
	if (0 == swpa_strcmp(pObj->Name(), "CSWRecord"))
	{
		if (FAILED(CopyRecordToSRMemory((CSWRecord*)pObj, &pNewRecord)) || NULL == pNewRecord)
		{
			SW_TRACE_DEBUG("Err: failed to CopyRecordToSRMemory\n");
			return E_FAIL;
		}
	
		pObj = pNewRecord;
		//SW_TRACE_DEBUG("(Record %p, refcount=%d)\n", pObj, pObj->GetRefCount());
	}
		
	
	m_pListLock->Lock();
	if (m_lstData.IsFull())
	{
		// there would be risk to remove adn release list head here because of timing issue caused by OnSendData
		// in case it being sending the list head obj
		//if (0 == swpa_strcmp(pObj->Name(), "CSWRecord"))
		//{
			// replace the oldest with the new one
			//CSWObject * pRecordObj = m_lstData.RemoveHead();
			//SAFE_RELEASE(pRecordObj);
			//m_lstData.AddTail(pObj);
		//}
		
		static DWORD dwLastTime = CSWDateTime::GetSystemTick();
		if(CSWDateTime::GetSystemTick() - dwLastTime > 30000
			|| 0 == swpa_strcmp(pObj->Name(), "CSWRecord"))
		{
			dwLastTime = CSWDateTime::GetSystemTick();
			PRINT("Warning: Record Send Queue is full(count : %d)!\n", m_lstData.GetCount(), pObj->Name());
		}
		m_pListLock->Unlock();
		return E_FAIL;
	}
	
	SAFE_ADDREF(pObj);
	m_lstData.AddTail(pObj);
	m_pListLock->Unlock();

	//Marked this due to the Object should be released after being sent in the Sending Thread. 
	//pObj->Release();

	if (0 == swpa_strcmp(pObj->Name(), "CSWRecord"))
	{		
		m_pListLock->Lock();
		SW_TRACE_DEBUG("Info: Record CDataTransmitter dataQ has %d objs (head:%p)\n", m_lstData.GetCount(), m_lstData.GetHead());
		m_pListLock->Unlock();
		SAFE_RELEASE(pObj);
	}
	
	m_pSemaSync->Post();

	return S_OK;
}


HRESULT CDataTransmitter::SendPackage(_PACKAGE * pPackage)
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	CSWAutoLock cAutoLock(m_pStreamLock);
	
	if (NULL == pPackage)
	{
		PRINT("Err: NULL == pPackage || NULL == pPackage->Header\n");
		return E_INVALIDARG;
	}
	
	//m_pStreamLock->Lock();
	if (NULL == m_pStream)
	{		
		//m_pStreamLock->Unlock();
		SW_TRACE_DEBUG("Err: Invalid stream handle \n");
		return E_FAIL;
	}

	if (0 == GetInfoOffset() && 0 == GetDataOffset())
	{
		if (FAILED(m_pStream->Write((VOID*)&pPackage->Header, sizeof(pPackage->Header), NULL)))
		{
			PRINT("Err: failed to send package header, size = %d\n", sizeof(pPackage->Header));
			return E_INVALIDARG;
		}
	}

	if (0 < pPackage->Header.dwInfoSize - GetInfoOffset())
	{
		if (NULL == pPackage->pbInfo)
		{
			PRINT("Err: NULL == pPackage->pbInfo\n");
			return E_INVALIDARG;
		}

		DWORD dwDataSentLen = 0;
		if (FAILED(m_pStream->Write((VOID*)(pPackage->pbInfo + GetInfoOffset()), pPackage->Header.dwInfoSize - GetInfoOffset(), &dwDataSentLen))
			|| dwDataSentLen != pPackage->Header.dwInfoSize - GetInfoOffset())
		{
			PRINT("Err: failed to send package info, size = %lu\n", pPackage->Header.dwInfoSize - GetInfoOffset());
			return E_FAIL;
		}
	}

	if (0 < pPackage->Header.dwDataSize - GetDataOffset())
	{
		if (NULL == pPackage->pbData)
		{
			PRINT("Err: NULL == pPackage->pbData\n");
			return E_INVALIDARG;
		}
		DWORD dwDataSentLen = 0;
		if (FAILED(m_pStream->Write((VOID*)(pPackage->pbData + GetDataOffset()), pPackage->Header.dwDataSize - GetDataOffset(), &dwDataSentLen))
			|| dwDataSentLen != pPackage->Header.dwDataSize - GetDataOffset())
		{
			PRINT("Err: failed to send package data, size = %lu\n", pPackage->Header.dwDataSize - GetDataOffset());
			return E_FAIL;
		}
		//PRINT("Info: dwDataSentLen = %u\n", dwDataSentLen);
	}


	SetInfoOffset(0);
	SetDataOffset(0);
	
	return S_OK;	
}





HRESULT CDataTransmitter::MakeNode(const DWORD dwBlockID, const DWORD dwNodeLen, const PBYTE pbData, PBYTE pbAddr)
{
	if (NULL == pbData || NULL == pbAddr)
	{
		PRINT("Err: NULL == pbData || NULL == pbAddr\n");
		return E_INVALIDARG;
	}

	swpa_memcpy(pbAddr, &dwBlockID, sizeof(dwBlockID));
	swpa_memcpy(pbAddr+sizeof(dwBlockID), &dwNodeLen, sizeof(dwNodeLen));
	swpa_memcpy(pbAddr+sizeof(dwBlockID)+sizeof(dwNodeLen), &pbData, dwNodeLen);
	
	return S_OK;
}



HRESULT CDataTransmitter::SendThrobPackage()
{
	
	_PACKAGE sThrobPackage;
	
	sThrobPackage.Header.dwType = CAMERA_THROB;
	sThrobPackage.Header.dwInfoSize = 0;
	sThrobPackage.Header.dwDataSize = 0;
	sThrobPackage.pbInfo = NULL;
	sThrobPackage.pbData = NULL;
	
	if (FAILED(SendPackage(&sThrobPackage)))
	{
		PRINT("Err: failed to send throb package\n");
		return E_FAIL;
	}

	//PRINT("Info: Send Throb package -- OK\n");
	
	return S_OK;
}



HRESULT CDataTransmitter::SendHistoryEndPackage()
{
	
	_PACKAGE sEndPackage;
	
	sEndPackage.Header.dwType = CAMERA_HISTORY_END;
	sEndPackage.Header.dwInfoSize = 0;
	sEndPackage.Header.dwDataSize = 0;
	sEndPackage.pbInfo = NULL;
	sEndPackage.pbData = NULL;
	
	if (FAILED(SendPackage(&sEndPackage)))
	{
		PRINT("Err: failed to send HistoryEnd package\n");
		return E_FAIL;
	}

	PRINT("Info: Send HistoryEnd package [DataType = %#x, TransmittingType = %#x] -- OK\n", 
		m_dwDataType, m_dwTransmittingType);
	
	return S_OK;
}



HRESULT CDataTransmitter::BuildImageInfoXml(CSWPosImage* pPosImage, CHAR** pszXmlData, DWORD* pdwSize)
{
	
	if (NULL == pPosImage || NULL == pszXmlData || NULL == pdwSize)
	{
		PRINT("Err: NULL == pPosImage || NULL == pszXmlData || NULL == pdwSize\n");
		return E_INVALIDARG;
	}

	SW_COMPONENT_IMAGE sComponentImage;
	if (FAILED(pPosImage->GetImage()->GetImage(&sComponentImage)))
	{
		PRINT("Err: failed to get image component\n");
		return E_FAIL;
	}

	IMAGE_EXT_INFO cExtInfo;
	pPosImage->GetImage()->GetImageExtInfo(&cExtInfo);
	
	TiXmlDocument XmlDoc;
	TiXmlDeclaration* pDeclaration = new TiXmlDeclaration("1.0","GB2312","yes"); //Note: pDeclaration will be delete in its parent's destructor
	TiXmlElement* pRootEle = new TiXmlElement("ImageInfo"); //Note: pRootEle will be delete in its parent's destructor


	XmlDoc.LinkEndChild(pDeclaration);	

	pRootEle->SetAttribute("Ver", "3.0");
	XmlDoc.LinkEndChild(pRootEle);

	TiXmlElement* pEleImage = new TiXmlElement("Image"); //Note: pEleImage will be delete in its parent's destructor
	if (NULL == pEleImage)
	{
		PRINT("Err: no memory for pEleImage\n");
		return E_OUTOFMEMORY;
	}
	
	if( pPosImage->GetImage()->IsCaptureImage() )
	{
		pEleImage->SetAttribute("Type", "JPEG_CAPTURE");
	}
	else if (pPosImage->GetDebugFlag())
	{
		pEleImage->SetAttribute("Type", "JPEG_SLAVE");
	}
	else
	{
		pEleImage->SetAttribute("Type", "JPEG");
	}
	
	pEleImage->SetAttribute("Width", sComponentImage.iWidth);

	pEleImage->SetAttribute("Height", sComponentImage.iHeight);

	pEleImage->SetAttribute("Time", pPosImage->GetImage()->GetRefTime()/*todo*/);

	DWORD dwTimeHigh = 0;
	DWORD dwTimeLow = 0;
	CSWDateTime::TimeConvert((DWORD)pPosImage->GetImage()->GetRefTime(), &dwTimeHigh, &dwTimeLow);
	pEleImage->SetAttribute("TimeHigh", dwTimeHigh);
	pEleImage->SetAttribute("TimeLow", dwTimeLow);

	pEleImage->SetAttribute("Shutter", cExtInfo.iShutter);
	pEleImage->SetAttribute("Gain", cExtInfo.iGain);
	pEleImage->SetAttribute("r_Gain", cExtInfo.iRGain);
	pEleImage->SetAttribute("g_Gain", cExtInfo.iGGain);
	pEleImage->SetAttribute("b_Gain", cExtInfo.iBGain);
	
	pRootEle->LinkEndChild(pEleImage);

	TiXmlElement* pEleRectInfo = new TiXmlElement("RectInfo"); //Note: will be delete in its parent's destructor
	if (NULL == pEleRectInfo)
	{
		PRINT("Err: no memory for pEleRectInfo\n");
		return E_OUTOFMEMORY;
	}

	pEleImage->LinkEndChild(pEleRectInfo);
	

	DWORD dwRectCount = pPosImage->GetCount();

	pEleRectInfo->SetAttribute("Count", dwRectCount);

	
	for (DWORD i=0; i<dwRectCount; i++)
	{
		CHAR szRectEleName[16] = {0};
		swpa_snprintf(szRectEleName, sizeof(szRectEleName)-1, "Rect%d", i);
		
		TiXmlElement* pRectInfoEle = new TiXmlElement(szRectEleName);
		if (NULL == pRectInfoEle)
		{
			PRINT("Err: no memory for pRectInfoEle %d\n", i);
			return E_OUTOFMEMORY;
		}
		
		pEleRectInfo->LinkEndChild(pRectInfoEle);

		SW_RECT sRect = pPosImage->GetRect(i);
		pRectInfoEle->SetAttribute("Left", sRect.left);
		pRectInfoEle->SetAttribute("Right", sRect.right);
		pRectInfoEle->SetAttribute("Top", sRect.top);
		pRectInfoEle->SetAttribute("Bottom", sRect.bottom);
	}
	

	TiXmlPrinter XmlPrinter;

	XmlDoc.Accept(&XmlPrinter);

	*pdwSize = XmlPrinter.Size()+1;
	
	*pszXmlData = (CHAR*)swpa_mem_alloc(*pdwSize);
	if (NULL == pszXmlData)
	{
		PRINT("Err: no memory for pszXmlData\n");
		return E_OUTOFMEMORY;
	}

	swpa_memset(*pszXmlData, 0x0, *pdwSize);
	swpa_memcpy(*pszXmlData, XmlPrinter.CStr(), XmlPrinter.Size());

	//PRINT("Info: JPEG xml is \n%s\n", *pszXmlData);

	return S_OK;
}




HRESULT CDataTransmitter::SendImage(CSWPosImage* pPosImage)
{
	HRESULT hr = S_OK;
	
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}
	
	if (NULL == pPosImage)
	{
		PRINT("Err: NULL == pImage\n");
		return E_INVALIDARG;
	}
	
	//todo: send image	
	_PACKAGE sImagePackage;
	SW_COMPONENT_IMAGE sComponentImage;

	sImagePackage.Header.dwType = CAMERA_IMAGE;
	sImagePackage.Header.dwInfoSize = 0;
	sImagePackage.Header.dwDataSize = 0;
	sImagePackage.pbInfo = NULL;
	sImagePackage.pbData = NULL;
	

	if (FAILED(BuildImageInfoXml(pPosImage, (CHAR**)&sImagePackage.pbInfo, &sImagePackage.Header.dwInfoSize)))
	{
		PRINT("Err: failed to build image info xml\n");
		hr = E_FAIL;
		goto OUT;
	}
	
	
	if (FAILED(pPosImage->GetImage()->GetImage(&sComponentImage)))
	{
		PRINT("Err: failed to get image component\n");
		hr = E_FAIL;
		goto OUT;
	}


	sImagePackage.pbData = sComponentImage.rgpbData[0];
	sImagePackage.Header.dwDataSize = sComponentImage.iSize;

	if (NULL == sImagePackage.pbData)
	{
		PRINT("Err: NULL == sImagePackage.pbData\n");
		hr =  E_INVALIDARG;
		goto OUT;
	}
	
	if (FAILED(SendPackage(&sImagePackage)))
	{
		PRINT("Err: failed to send jpeg image package\n");
		hr = E_FAIL;
		goto OUT;
	}

	//PRINT("Info: Send image package -- OK\n");
	hr = S_OK;
OUT:
	
	SAFE_MEM_FREE(sImagePackage.pbInfo);
	
	return hr;
}




HRESULT CDataTransmitter::BuildVideoInfoXml(CSWImage* pVideo, CHAR** pszXmlData, DWORD* pdwSize)
{
	
	if (NULL == pVideo || NULL == pszXmlData || NULL == pdwSize)
	{
		PRINT("Err: NULL == pVideo || NULL == pszXmlData || NULL == pdwSize\n");
		return E_INVALIDARG;
	}

	SW_COMPONENT_IMAGE sComponentImage;
	if (FAILED(pVideo->GetImage(&sComponentImage)))
	{
		PRINT("Err: failed to get image component\n");
		return E_FAIL;
	}

	IMAGE_EXT_INFO cExtInfo;
	pVideo->GetImageExtInfo(&cExtInfo);
	
	TiXmlDocument XmlDoc;	
	TiXmlDeclaration* pDeclaration = new TiXmlDeclaration("1.0","GB2312","yes"); //Note: pDeclaration will be delete in its parent's destructor
	TiXmlElement* pRootEle = new TiXmlElement("VideoInfo"); //Note: pRootEle will be delete in its parent's destructor


	XmlDoc.LinkEndChild(pDeclaration);	

	pRootEle->SetAttribute("Ver", "3.0");
	XmlDoc.LinkEndChild(pRootEle);

	TiXmlElement* pEleVideo = new TiXmlElement("Video"); //Note: pEleImage will be delete in its parent's destructor
	
	pEleVideo->SetAttribute("Type", "H264");
	pEleVideo->SetAttribute("FrameType", pVideo->GetFrameName());	
	pEleVideo->SetAttribute("Width", sComponentImage.iWidth);
	pEleVideo->SetAttribute("Height", sComponentImage.iHeight);
	pEleVideo->SetAttribute("Time", pVideo->GetRefTime());

	DWORD dwTimeHigh = 0;
	DWORD dwTimeLow = 0;
	//CSWDateTime::TimeConvert((DWORD)pVideo->GetRefTime(), &dwTimeHigh, &dwTimeLow);
	pVideo->GetRealTime(&dwTimeHigh, &dwTimeLow);
	//PRINT("Info: dwTimeHigh = %lu, dwTimeLow = %lu\n", dwTimeHigh, dwTimeLow);
	pEleVideo->SetAttribute("TimeHigh", dwTimeHigh);
	pEleVideo->SetAttribute("TimeLow", dwTimeLow);

	pEleVideo->SetAttribute("Shutter", cExtInfo.iShutter);
	pEleVideo->SetAttribute("Gain", cExtInfo.iGain);
	pEleVideo->SetAttribute("r_Gain",cExtInfo.iRGain);
	pEleVideo->SetAttribute("g_Gain", cExtInfo.iGGain);
	pEleVideo->SetAttribute("b_Gain", cExtInfo.iBGain);
	
	pRootEle->LinkEndChild(pEleVideo);


	TiXmlPrinter XmlPrinter;

	XmlDoc.Accept(&XmlPrinter);

	*pdwSize = XmlPrinter.Size()+1;
	
	*pszXmlData = (CHAR*)swpa_mem_alloc(*pdwSize);
	if (NULL == pszXmlData)
	{
		PRINT("Err: no memory for pszXmlData\n");
		return E_OUTOFMEMORY;
	}

	swpa_memset(*pszXmlData, 0x0, *pdwSize);
	swpa_memcpy(*pszXmlData, XmlPrinter.CStr(), XmlPrinter.Size());

	//PRINT("Info: Video Info Xml = \n%s\n", XmlPrinter.CStr());//todo: this is for debug, will be deleted

	return S_OK;
}




HRESULT CDataTransmitter::SendVideo(CSWImage* pVideo)
{
	HRESULT hr = S_OK;
	
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}
	
	if (NULL == pVideo)
	{
		PRINT("Err: NULL == pVideo\n");
		return E_INVALIDARG;
	}
	//todo: send video	
	_PACKAGE sVideoPackage;
	SW_COMPONENT_IMAGE sComponentImage;

	sVideoPackage.Header.dwType = CAMERA_VIDEO;
	sVideoPackage.Header.dwInfoSize = 0;
	sVideoPackage.Header.dwDataSize = 0;
	sVideoPackage.pbInfo = NULL;
	sVideoPackage.pbData = NULL;

	if (FAILED(BuildVideoInfoXml(pVideo, (CHAR**)&sVideoPackage.pbInfo, &sVideoPackage.Header.dwInfoSize)))
	{
		PRINT("Err: failed to build video info xml\n");
		hr = E_FAIL;
		goto OUT;
	}

	
	if (FAILED(pVideo->GetImage(&sComponentImage)))
	{
		PRINT("Err: failed to get image component\n");
		hr = E_FAIL;
		goto OUT;
	}
	

	sVideoPackage.pbData = sComponentImage.rgpbData[0];
	sVideoPackage.Header.dwDataSize = sComponentImage.iSize;
	if (NULL == sVideoPackage.pbData)
	{
		PRINT("Err: NULL == sVideoPackage.pbData\n");
		hr = E_FAIL;
		goto OUT;
	}

		
	if (FAILED(SendPackage(&sVideoPackage)))
	{
		PRINT("Err: failed to send H264 Video package\n");
		hr = E_FAIL;
		goto OUT;
	}

	//PRINT("Info: Send video package -- OK\n");
	hr = S_OK;
OUT:	

	SAFE_MEM_FREE(sVideoPackage.pbInfo);
	
	
	return hr;
}






HRESULT CDataTransmitter::BuildTrafficInfoXml(CSWRecord* pRecord, CHAR** pszXmlData, DWORD* pdwSize)
{
	
	if (NULL == pRecord || NULL == pszXmlData || NULL == pdwSize)
	{
		PRINT("Err: NULL == pRecord || NULL == pszXmlData || NULL == pdwSize\n");
		return E_INVALIDARG;
	}
	
	TiXmlDocument XmlDoc;	
	TiXmlDeclaration* pDeclaration = new TiXmlDeclaration("1.0","GB2312","yes"); //Note: pDeclaration will be delete in its parent's destructor
	TiXmlElement* pRootEle = new TiXmlElement("StringInfo"); //Note: pRootEle will be delete in its parent's destructor


	XmlDoc.LinkEndChild(pDeclaration);	

	pRootEle->SetAttribute("Ver", "3.0");
	XmlDoc.LinkEndChild(pRootEle);

	TiXmlElement* pEleType = new TiXmlElement("Type"); //Note: will be delete in its parent's destructor
	if (NULL == pEleType)
	{
		PRINT("Err: no memory for pEleType\n");
		return E_OUTOFMEMORY;
	}
	pRootEle->LinkEndChild(pEleType);
	
	
	TiXmlText* pTxtType = new TiXmlText("TrafficInfo");
	if (NULL == pTxtType)
	{
		PRINT("Err: no memory for pTxtType\n");
		return E_OUTOFMEMORY;
	}

	pEleType->LinkEndChild(pTxtType);

	TiXmlPrinter XmlPrinter;

	XmlDoc.Accept(&XmlPrinter);

	*pdwSize = XmlPrinter.Size()+1;
	
	*pszXmlData = (CHAR*)swpa_mem_alloc(*pdwSize);
	if (NULL == pszXmlData)
	{
		PRINT("Err: no memory for pszXmlData\n");
		return E_OUTOFMEMORY;
	}

	swpa_memset(*pszXmlData, 0x0, *pdwSize);
	swpa_memcpy(*pszXmlData, XmlPrinter.CStr(), XmlPrinter.Size());

	//PRINT("Info: result xml is \n%s\n", *pszXmlData);

	return S_OK;
}





HRESULT CDataTransmitter::SendTrafficInfo(CSWRecord* pRecord)
{
	HRESULT hr = S_OK;
	
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	if (NULL == pRecord)
	{
		PRINT("Err: NULL == pRecord\n");
		return E_INVALIDARG;
	}
	
	_PACKAGE sRecordPackage;

	sRecordPackage.Header.dwType = CAMERA_STRING;
	sRecordPackage.Header.dwInfoSize = 0;
	sRecordPackage.Header.dwDataSize = 0;
	sRecordPackage.pbInfo = NULL;
	sRecordPackage.pbData = NULL;

	
	if (FAILED(BuildTrafficInfoXml(pRecord, (CHAR**)&sRecordPackage.pbInfo, &sRecordPackage.Header.dwInfoSize)))
	{
		PRINT("Err: failed to build TrafficInfo info xml\n");
		return E_FAIL;
	}

	sRecordPackage.Header.dwDataSize = pRecord->GetXmlSize();
	sRecordPackage.pbData = (PBYTE)pRecord->GetXmlString();

	SW_TRACE_DEBUG("<SendTrafficInfo> datasize:%d.\n", sRecordPackage.Header.dwDataSize);
	
	if (FAILED(SendPackage(&sRecordPackage)))
	{
		PRINT("Err: failed to send TrafficInfo package\n");
		hr = E_FAIL;
		goto OUT;
	}

	PRINT("Info: Send TrafficInfo package -- OK\n");
	hr = S_OK;
	
OUT:

	SAFE_MEM_FREE(sRecordPackage.pbInfo);

	//SAFE_MEM_FREE(sRecordPackage.pbData);
	
	return hr;

}



HRESULT CDataTransmitter::BuildRecordInfoXml(CSWRecord* pRecord, CHAR** pszXmlData, DWORD* pdwSize)
{
	
	if (NULL == pRecord || NULL == pszXmlData || NULL == pdwSize)
	{
		PRINT("Err: NULL == pRecord || NULL == pszXmlData || NULL == pdwSize\n");
		return E_INVALIDARG;
	}

	if (0 == pRecord->GetVideoCount() || !m_fSendIllegalVideo)
	{
		*pdwSize = pRecord->GetXmlSize()+1;
	
		*pszXmlData = (CHAR*)swpa_mem_alloc(*pdwSize);
		if (NULL == pszXmlData)
		{
			PRINT("Err: no memory for pszXmlData\n");
			return E_OUTOFMEMORY;
		}

		swpa_memset(*pszXmlData, 0x0, *pdwSize);
		swpa_strncpy(*pszXmlData, pRecord->GetXmlString(), *pdwSize);
	}
	else
	{
		TiXmlDocument XmlDoc;

		XmlDoc.Parse(pRecord->GetXmlString());

		TiXmlNode *pExtInfoEle = XmlDoc.RootElement()->FirstChildElement("ResultExtInfo");
		if (!pExtInfoEle)
		{
			PRINT("Err: no valid xml\n");
			return E_INVALIDARG;
		}

		TiXmlElement* pVideo = new TiXmlElement("Video");
		if (NULL == pVideo)
		{	
			PRINT("Err: no memory for video\n");
			return E_OUTOFMEMORY;
		}
		pExtInfoEle->LinkEndChild(pVideo);

		CSWImage* pH264Video = NULL;
		pRecord->GetVideo(0, &pH264Video);
		
		DWORD dwTimeHigh = 0;
		DWORD dwTimeLow = 0;
		CSWDateTime::TimeConvert((DWORD)pH264Video->GetRefTime(), &dwTimeHigh, &dwTimeLow);
		pVideo->SetAttribute("TimeHigh", dwTimeHigh);
		pVideo->SetAttribute("TimeLow", dwTimeLow);
		pVideo->SetAttribute("Length", pRecord->GetVideoCount());
		pVideo->SetAttribute("Height", pH264Video->GetHeight());
		pVideo->SetAttribute("Width", pH264Video->GetWidth());


		TiXmlPrinter XmlPrinter;

		XmlDoc.Accept(&XmlPrinter);

		*pdwSize = XmlPrinter.Size()+1;
		
		*pszXmlData = (CHAR*)swpa_mem_alloc(*pdwSize);
		if (NULL == pszXmlData)
		{
			PRINT("Err: no memory for pszXmlData\n");
			return E_OUTOFMEMORY;
		}

		swpa_memset(*pszXmlData, 0x0, *pdwSize);
		swpa_strncpy(*pszXmlData, XmlPrinter.CStr(), XmlPrinter.Size());
	}
	//PRINT("Info: result xml is \n%s\n", *pszXmlData);

	return S_OK;
}




HRESULT CDataTransmitter::SendRecord(CSWRecord* pRecord)
{
	HRESULT hr = S_OK;
	DWORD dwPrevTick, dwCurTick;
	
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	if (NULL == pRecord)
	{
		PRINT("Err: NULL == pRecord\n");
		return E_INVALIDARG;
	}
	
	_PACKAGE sRecordPackage;

	sRecordPackage.Header.dwType = CAMERA_RECORD;
	sRecordPackage.Header.dwInfoSize = 0;
	sRecordPackage.Header.dwDataSize = 0;
	sRecordPackage.pbInfo = NULL;
	sRecordPackage.pbData = NULL;

	if (FAILED(BuildRecordInfoXml(pRecord, (CHAR**)&sRecordPackage.pbInfo, &sRecordPackage.Header.dwInfoSize)))
	{
		PRINT("Err: failed to build record info xml\n");
		return E_FAIL;
	}


	DWORD dwSize = 0;
	PBYTE pbBuf = NULL;
	for (DWORD i=0; i<pRecord->GetImageTypeCount(); i++)
	{
		if (NULL != pRecord->GetImage(i))
		{
			dwSize += sizeof(INT/*this is for image type, 0~6*/);
			dwSize += sizeof(INT/*this is for image size*/);
			dwSize += pRecord->GetImage(i)->GetSize();
			
		}
	}

	DWORD dwVideoCount = pRecord->GetVideoCount();
	DWORD dwVideoSize = 0;
	DWORD dwVideoType = 0x80000001;
	if (m_fSendIllegalVideo)
	{
    	if (dwVideoCount > 0)
    	{
    		dwSize += sizeof(dwVideoType/*this is for video type, 0x80000001*/);
    		dwSize += sizeof(dwVideoSize/*this is for video size*/);
    		for (DWORD i=0; i<dwVideoCount; i++)
    		{
    			CSWImage * pVideo = NULL;
    			if (SUCCEEDED(pRecord->GetVideo(i, &pVideo))
    				&& NULL != pVideo)
    			{
    				dwVideoSize += pVideo->GetSize();
    			}
    		}
    		dwSize += dwVideoSize;
    	}
	}


	SW_TRACE_DEBUG("Info: this record(0x%x) occupied %d B mem\n", pRecord, dwSize);
	
	if (dwSize > 0)
	{
		if (FAILED(pRecord->LockBuffer()))
		{
			SW_TRACE_NORMAL("Err: failed to lock record buffer\n");
			hr = E_FAIL;
			goto OUT;
		}
		
		hr = pRecord->GetBuffer(dwSize, (PVOID*)&pbBuf);
		if (FAILED(hr))
		{
			SW_TRACE_NORMAL("Err: failed to get record buffer\n");
			pRecord->UnlockBuffer();
			hr = E_FAIL;
			goto OUT;
		}
		else if (S_FALSE == hr)
		{		
			PBYTE pbOffset = pbBuf;
			
			for (DWORD i=0; i<pRecord->GetImageTypeCount(); i++)
			{
				if (NULL != pRecord->GetImage(i))
				{
					SW_COMPONENT_IMAGE sComponentImage;
					if (FAILED(pRecord->GetImage(i)->GetImage(&sComponentImage)))
					{
						PRINT("Err: failed to get image\n");
						pRecord->UnlockBuffer();
						hr = E_FAIL;
						goto OUT;
					}
					swpa_memcpy(pbOffset, &i, sizeof(INT)); /*Note: a little tricky here, i stands for the image type, too. 0~6 stands for snapshoot, best capture,..., correspondingly*/
					pbOffset += sizeof(INT);
					swpa_memcpy(pbOffset, &sComponentImage.iSize, sizeof(INT));
					pbOffset += sizeof(INT);

					if (SW_IMAGE_BIN == sComponentImage.cImageType) //Bin image 
					{
					    BYTE *pSrc = sComponentImage.rgpbData[0];//GetHvImageData(&imgTemp, 0);
					    INT iDestLine = sComponentImage.iWidth >> 3;
					    for (INT iHeight = 0; iHeight < sComponentImage.iHeight; iHeight++, pSrc += iDestLine)
					    {
					        swpa_memcpy(pbOffset, pSrc, iDestLine);
					        pbOffset += iDestLine;
					    }
					}
					else
					{			
						swpa_memcpy(pbOffset, sComponentImage.rgpbData[0], sComponentImage.iSize);
						pbOffset += sComponentImage.iSize;
					}
				}
			}

			if (dwVideoCount > 0 && m_fSendIllegalVideo)
			{
				
				swpa_memcpy(pbOffset, &dwVideoType, sizeof(dwVideoType));
				pbOffset += sizeof(dwVideoType);
				swpa_memcpy(pbOffset, &dwVideoSize, sizeof(dwVideoSize));
				pbOffset += sizeof(dwVideoSize);
				for (DWORD i=0; i<dwVideoCount; i++)
				{
					CSWImage * pVideo = NULL;
					if (SUCCEEDED(pRecord->GetVideo(i, &pVideo))
						&& NULL != pVideo)
					{
						SW_COMPONENT_IMAGE sComponentImage;
						pVideo->GetImage(&sComponentImage);
						swpa_memcpy(pbOffset, sComponentImage.rgpbData[0], sComponentImage.iSize);
						pbOffset += sComponentImage.iSize;
					}
				}
			}
		}
	}
	pRecord->UnlockBuffer();
	
	sRecordPackage.Header.dwDataSize = dwSize;
	sRecordPackage.pbData = pbBuf;

	dwPrevTick = CSWDateTime::GetSystemTick();
	if (FAILED(SendPackage(&sRecordPackage)))
	{
		PRINT("Err: failed to send record package\n");
		hr = E_FAIL;
		goto OUT;
	}
	dwCurTick = CSWDateTime::GetSystemTick();

	if (dwCurTick > dwPrevTick)
	{
		SW_TRACE_DEBUG("---tobedeleted: sent %d Byte, cost %d ms.\n", dwSize, dwCurTick - dwPrevTick);
	}
	
	PRINT("Info: Send record package -- OK\n");
	hr = S_OK;
	
OUT:

	SAFE_MEM_FREE(sRecordPackage.pbInfo);

	//该缓存由Record内部管理，这里不做释放
	//SAFE_MEM_FREE(sRecordPackage.pbData);
	
	return hr;
}



HRESULT CDataTransmitter::SendRecordEx(CSWRecord* pRecord)
{
	HRESULT hr = S_OK;
	DWORD dwPrevTick, dwCurTick;
	
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	if (NULL == pRecord)
	{
		SW_TRACE_DEBUG("Err: NULL == pRecord\n");
		return E_INVALIDARG;
	}
	
	_PACKAGE sRecordPackage;

	sRecordPackage.Header.dwType = CAMERA_RECORD;
	sRecordPackage.Header.dwInfoSize = 0;
	sRecordPackage.Header.dwDataSize = 0;
	sRecordPackage.pbInfo = NULL;
	sRecordPackage.pbData = NULL;

	if (FAILED(BuildRecordInfoXml(pRecord, (CHAR**)&sRecordPackage.pbInfo, &sRecordPackage.Header.dwInfoSize)))
	{
		SW_TRACE_DEBUG("Err: failed to build record info xml\n");
		SAFE_MEM_FREE(sRecordPackage.pbInfo);
		return E_FAIL;
	}


	DWORD dwSize = 0;
	PBYTE pbBuf = NULL;
	for (DWORD i=0; i<pRecord->GetImageTypeCount(); i++)
	{
		if (NULL != pRecord->GetImage(i))
		{
			dwSize += sizeof(INT/*this is for image type, 0~6*/);
			dwSize += sizeof(INT/*this is for image size*/);
			dwSize += pRecord->GetImage(i)->GetSize();
			
		}
	}

	DWORD dwVideoCount = pRecord->GetVideoCount();
	DWORD dwVideoSize = 0;
	DWORD dwVideoType = 0x80000001;
	

	if (m_fSendIllegalVideo)
	{
    	if (dwVideoCount > 0)
    	{
    		dwSize += sizeof(dwVideoType/*this is for video type, 0x80000001*/);
    		dwSize += sizeof(dwVideoSize/*this is for video size*/);
    		for (DWORD i=0; i<dwVideoCount; i++)
    		{
    			CSWImage * pVideo = NULL;
    			if (SUCCEEDED(pRecord->GetVideo(i, &pVideo))
    				&& NULL != pVideo)
    			{
    				dwVideoSize += pVideo->GetSize();
    			}
    		}
    		dwSize += dwVideoSize;
    	}
	}


	SW_TRACE_DEBUG("Info: this record(0x%x) occupied %d B mem\n", pRecord, dwSize);

	sRecordPackage.Header.dwDataSize = dwSize;

	DWORD dwDataSentLen = 0;
	
	//send header
	if (FAILED(m_pStream->Write((VOID*)&sRecordPackage.Header, sizeof(sRecordPackage.Header), NULL)))
	{
		SW_TRACE_DEBUG("Err: failed to send package header, size = %d\n", sizeof(sRecordPackage.Header));
		hr = E_FAIL;
		goto OUT;
	}

	//send info
	if (0 < sRecordPackage.Header.dwInfoSize)
	{
		if (NULL == sRecordPackage.pbInfo)
		{
			SW_TRACE_DEBUG("Err: NULL == pPackage->pbInfo\n");
			hr = E_FAIL;
			goto OUT;
		}

		if (FAILED(m_pStream->Write((VOID*)(sRecordPackage.pbInfo), sRecordPackage.Header.dwInfoSize, &dwDataSentLen))
			|| dwDataSentLen != sRecordPackage.Header.dwInfoSize)
		{
			SW_TRACE_DEBUG("Err: failed to send package info, size = %lu\n", sRecordPackage.Header.dwInfoSize);
			hr = E_FAIL;
			goto OUT;
		}
	}

	
	//send data
	if (0 < sRecordPackage.Header.dwDataSize)
	{
		//send images
		for (DWORD i=0; i<pRecord->GetImageTypeCount(); i++)
		{
			if (NULL != pRecord->GetImage(i))
			{
				SW_COMPONENT_IMAGE sComponentImage;
				if (FAILED(pRecord->GetImage(i)->GetImage(&sComponentImage)))
				{
					SW_TRACE_DEBUG("Err: failed to get image\n");
					hr = E_FAIL;
					goto OUT;
				}
				
				/*Note: a little tricky here, i stands for the image type, too. 0~6 stands for snapshoot, best capture,..., correspondingly*/
				if (FAILED(m_pStream->Write((VOID*)(&i), sizeof(i), &dwDataSentLen))
					|| dwDataSentLen != sizeof(i))
				{
					SW_TRACE_DEBUG("Err: failed to send image type, size = %lu\n", sizeof(i));
					hr = E_FAIL;
					goto OUT;
				}

				if (FAILED(m_pStream->Write((VOID*)(&sComponentImage.iSize), sizeof(sComponentImage.iSize), &dwDataSentLen))
					|| dwDataSentLen != sizeof(sComponentImage.iSize))
				{
					SW_TRACE_DEBUG("Err: failed to send image size, size = %lu\n", sizeof(sComponentImage.iSize));
					hr = E_FAIL;
					goto OUT;
				}

				
				if (SW_IMAGE_BIN == sComponentImage.cImageType) //Bin image 
				{
				    BYTE *pSrc = sComponentImage.rgpbData[0];//GetHvImageData(&imgTemp, 0);
				    INT iDestLine = sComponentImage.iWidth >> 3;
				    for (INT iHeight = 0; iHeight < sComponentImage.iHeight; iHeight++, pSrc += iDestLine)
				    {
						if (FAILED(m_pStream->Write((VOID*)(pSrc), iDestLine, &dwDataSentLen))
							|| dwDataSentLen != iDestLine)
						{
							SW_TRACE_DEBUG("Err: failed to send bin image, size = %lu\n", iDestLine);
							hr = E_FAIL;
							goto OUT;
						}
				    }
				}
				else
				{
					if (FAILED(m_pStream->Write((VOID*)(sComponentImage.rgpbData[0]), sComponentImage.iSize, &dwDataSentLen))
						|| dwDataSentLen != sComponentImage.iSize)
					{
						SW_TRACE_DEBUG("Err: failed to send image #%d data, size = %lu\n", i, sComponentImage.iSize);
						hr = E_FAIL;
						goto OUT;
					}
				}
			}
		}

		//send video
		if (dwVideoCount > 0 && m_fSendIllegalVideo)
		{
			if (FAILED(m_pStream->Write((VOID*)(&dwVideoType), sizeof(dwVideoType), &dwDataSentLen))
					|| dwDataSentLen != sizeof(dwVideoType))
			{
				SW_TRACE_DEBUG("Err: failed to send video type, size = %lu\n", sizeof(dwVideoType));
				hr = E_FAIL;
				goto OUT;
			}

			if (FAILED(m_pStream->Write((VOID*)(&dwVideoSize), sizeof(dwVideoSize), &dwDataSentLen))
				|| dwDataSentLen != sizeof(dwVideoSize))
			{
				SW_TRACE_DEBUG("Err: failed to send video size, size = %lu\n", sizeof(dwVideoSize));
				hr = E_FAIL;
				goto OUT;
			}
				
			for (DWORD i=0; i<dwVideoCount; i++)
			{
				CSWImage * pVideo = NULL;
				if (SUCCEEDED(pRecord->GetVideo(i, &pVideo))
					&& NULL != pVideo)
				{
					SW_COMPONENT_IMAGE sComponentImage;
					pVideo->GetImage(&sComponentImage);

					if (FAILED(m_pStream->Write((VOID*)(sComponentImage.rgpbData[0]), sComponentImage.iSize, &dwDataSentLen))
						|| dwDataSentLen != sComponentImage.iSize)
					{
						SW_TRACE_DEBUG("Err: failed to send video #%d data, size = %lu\n", i, sComponentImage.iSize);
						hr = E_FAIL;
						goto OUT;
					}
				}
			}
		}
	}

	
	SW_TRACE_DEBUG("Info: Send record -- OK\n");
	hr = S_OK;
	
OUT:

	SAFE_MEM_FREE(sRecordPackage.pbInfo);
	
	return hr;
}




HRESULT CDataTransmitter::OnSendData()
{
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	BOOL fListIsEmpty = TRUE;
	BOOL fPending = FALSE;
	DWORD dwPendingTick = 0;

	while (STATE_RUNNING == m_dwState || !fListIsEmpty)
	{		
		HRESULT hr = S_OK;
		fListIsEmpty = TRUE;

		if (SUCCEEDED(IsPending(&fPending)) && fPending)
		{
			if (dwPendingTick++ > 120)
			{
				SW_TRACE_DEBUG("Info: record CDataTransmitter Pending too long, must be disconnected.");
				break;
			}
			CSWApplication::Sleep(500);
			//SW_TRACE_DEBUG("Info: record CDataTransmitter Pending...");
			continue;
		}

		dwPendingTick = 0;
		
		if (FAILED(m_pSemaSync->Pend(m_dwMaxHaltTimeMs)))
		{
			if (FAILED(SendThrobPackage()))
			{
				PRINT("Err: failed to send Throb package\n");
				
				if (DATATYPE_RECORD == m_dwDataType)
				{
					Pending(TRUE); continue;
				}
				else
				{
					break;
				}
			}
		}
		//else
		{
			m_pListLock->Lock();

			CSWObject* pObj = NULL;
			if( !m_lstData.IsEmpty() )
			{
				pObj = m_lstData.GetHead(); //m_lstData.RemoveHead();
			}
			else
			{
				//SW_TRACE_DEBUG("Info: data list empty.\n");
				m_pListLock->Unlock();
				continue;
			}

			DWORD dwObjCount = m_lstData.GetCount();

			m_pListLock->Unlock();

			//PRINT("Info: Got a obj = %s\n", pObj->Name());

			if (IsDecendant(CSWImage, pObj))
			{
				CSWImage* pImage = (CSWImage*)pObj;

				//PRINT("Info: got Image obj\n");
				if (0 == pImage->GetRefTime()
					&& 0 == pImage->GetWidth()
					&& 0 == pImage->GetHeight())
				{
					if (TRANSMITTING_HISTORY == m_dwTransmittingType)
					{
						if (FAILED(SendHistoryEndPackage()))
						{
							PRINT("Err: failed to send Video-End-Flag data\n");
							//SAFE_RELEASE(pObj);
							break;
						}
					}
				}
				else
				{
					hr = SendVideo(pImage);
					if (FAILED(hr))
					{
						PRINT("Err: failed to send video data\n");
						//SAFE_RELEASE(pObj);
						break;
					}
				}
			}
			else if (IsDecendant(CSWPosImage, pObj))
			{
				CSWPosImage* pJpeg = (CSWPosImage*)pObj;
				
				hr = SendImage(pJpeg);
				if (FAILED(hr))
				{
					PRINT("Err: failed to send JPEG data\n");
					//SAFE_RELEASE(pObj);
					break;
				}
			}
			else if (IsDecendant(CSWRecord, pObj))
			{
				CSWRecord* pRecord = (CSWRecord*)pObj;
				//PRINT("Info: L%d: got Record obj...\n", __LINE__);
				SW_TRACE_DEBUG("Info: %s SendQ obj count: %d\n", pObj->Name(), dwObjCount);
				if (!pRecord->IsValid())
				{
					if (TRANSMITTING_HISTORY == m_dwTransmittingType)
					{
						if (FAILED(SendHistoryEndPackage()))
						{
							PRINT("Err: failed to send Record-End-Flag data\n");
							SAFE_RELEASE(pObj);
							break;
						}
					}
				}
				else if (pRecord->GetTrafficInfoFlag())
				{
					hr = SendTrafficInfo(pRecord);
					if (FAILED(hr))
					{
						PRINT("Err: failed to send TrafficInfo data\n");

						Pending(TRUE); 
						m_pSemaSync->Post(); 
						continue;
					}
				}
				else
				{
					hr = SendRecord(pRecord);
					/*if (GetInfoOffset() > 0 || GetDataOffset() > 0)
					{
						//如果支持断点续传，则仍然用先打包再发送的方式
						SW_TRACE_DEBUG("SendRecord id=%d\n",pRecord->GetCarArriveTime());
						hr = SendRecord(pRecord);
					}
					else
					{
						//默认使用不打包直接发送的方式
						SW_TRACE_DEBUG("SendRecordEx id=%d\n",pRecord->GetCarArriveTime());
						hr = SendRecordEx(pRecord);
					}*/

					if (FAILED(hr))
					{
						PRINT("Err: failed to send Result data\n");

						if (TRANSMITTING_HISTORY == m_dwTransmittingType)
						{
							//SAFE_RELEASE(pObj);
							break;
						}
						else
						{
							Pending(TRUE); 
							m_pSemaSync->Post(); 
							continue;
						}
					}
				}
			}
			
			m_pListLock->Lock();
			m_lstData.RemoveHead();
			fListIsEmpty = m_lstData.IsEmpty();
			m_pListLock->Unlock();
			SAFE_RELEASE(pObj);
		}

		//m_pListLock->Lock();
		//fListIsEmpty = m_lstData.IsEmpty();
		//m_pListLock->Unlock();
	}

	
	m_dwState = STATE_STOPPED;

	m_pListLock->Lock();
	while (!m_lstData.IsEmpty())
	{
		CSWObject* pObj = (CSWObject*)m_lstData.RemoveHead();
		SAFE_RELEASE(pObj);
	}
	m_pListLock->Unlock();
	PRINT("Info: %s() [DataType = %#x, TransmittingType = %#x] exited\n", __FUNCTION__, m_dwDataType, m_dwTransmittingType);

	return S_OK;
}



VOID* CDataTransmitter::OnSendDataProxy(VOID* pvArg)
{
	if (NULL == pvArg)
	{
		PRINT("Err: NULL == pvArg\n");
		return (VOID*)E_INVALIDARG;
	}
	
	CDataTransmitter* pThis = (CDataTransmitter*)pvArg;

	return (VOID*)pThis->OnSendData();
}


HRESULT CDataTransmitter::Pending(const BOOL fPend)
{	
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	m_fPending = fPend;

	//UpdateStream(NULL);
	
	return S_OK;
}


HRESULT CDataTransmitter::IsPending(BOOL * fPending)
{	
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	if (NULL == fPending)
	{
		return E_INVALIDARG;
	}

	*fPending = m_fPending;	
	
	return S_OK;
}



HRESULT CDataTransmitter::UpdateStream(CSWStream* pStream)
{	
	if (!m_fInited)
	{
		PRINT("Err: not inited yet\n");
		return E_NOTIMPL;
	}

	CSWAutoLock cAutoLock(m_pStreamLock);

	if (NULL == pStream)
	{
		//PRINT("Err: invalid arg pStream == NULL\n");
		//return E_INVALIDARG;
	}

	//m_pStreamLock->Lock();
	SAFE_RELEASE(m_pStream);
	
	m_pStream = pStream;
	SAFE_ADDREF(m_pStream);
	//m_pStreamLock->Unlock();

	Pending(FALSE);
	
	return S_OK;
}


HRESULT CDataTransmitter::CopyRecordToSRMemory(CSWRecord * pSrcRecord, CSWRecord** ppDestRecord)
{
	if (NULL == pSrcRecord || NULL == ppDestRecord)
	{
		SW_TRACE_DEBUG("Err: invalid args\n");
		return E_INVALIDARG;
	}

	CSWRecord* pNewRecord = new CSWRecord();
	if (NULL == pNewRecord || FAILED(pNewRecord->Initialize()))
	{
		SW_TRACE_DEBUG("Err: failed to create new record\n");
		SAFE_RELEASE(pNewRecord);
		return E_OUTOFMEMORY;
	}


	// 1. xml info
	pNewRecord->SetXml(pSrcRecord->GetXmlString());
	

	// 2. Images
	for (DWORD i = 0; i < pSrcRecord->GetImageTypeCount(); i++)
	{
		CSWImage * pSrcImage = pSrcRecord->GetImage(i);
		if (NULL != pSrcImage)
		{
			SW_COMPONENT_IMAGE sSrcComponent;
			if (FAILED(pSrcImage->GetImage(&sSrcComponent)))
			{
				SW_TRACE_DEBUG("Err: failed to get image\n");
				SAFE_RELEASE(pNewRecord);
				return E_FAIL;
			}

			CSWImage* pNewImage = NULL;
			if ( S_OK == CSWImage::CreateSWImage(&pNewImage, 
					pSrcImage->GetType(), 
					pSrcImage->GetWidth(), 
					pSrcImage->GetHeight(), 
					CSWMemoryFactory::GetInstance(SW_SHARED_SR_MEMORY),
					pSrcImage->GetFrameNo(), 
					pSrcImage->GetRefTime(), 
					pSrcImage->GetFlag(), 
					pSrcImage->GetFrameName(),
					pSrcImage->GetSize())
				)
			{
				SW_COMPONENT_IMAGE sNewComponent;
				pNewImage->GetImage(&sNewComponent);
				sNewComponent.iSize = sSrcComponent.iSize;
				swpa_memcpy(sNewComponent.rgpbData[0], sSrcComponent.rgpbData[0], sNewComponent.iSize);
				pNewImage->SetImage(sNewComponent);

				pNewRecord->SetImage(i, pNewImage);
				SAFE_RELEASE(pNewImage);
			}
			else
			{
				SW_TRACE_DEBUG("Err: failed to create image...\n");
				SAFE_RELEASE(pNewRecord);
				return E_FAIL;
			}
		}
	}
	

	// 3. Videos
	for (DWORD i = 0; i < pSrcRecord->GetVideoCount(); i++)
	{
		CSWImage * pSrcVideo = NULL;
		if (SUCCEEDED(pSrcRecord->GetVideo(i, &pSrcVideo))
			&& NULL != pSrcVideo)
		{
			SW_COMPONENT_IMAGE sSrcComponent;
			if (FAILED(pSrcVideo->GetImage(&sSrcComponent)))
			{
				SW_TRACE_DEBUG("Err: failed to get video\n");
				SAFE_RELEASE(pNewRecord);
				return E_FAIL;
			}

			CSWImage* pNewVideo = NULL;
			if ( S_OK == CSWImage::CreateSWImage(&pNewVideo, 
					pSrcVideo->GetType(), 
					pSrcVideo->GetWidth(), 
					pSrcVideo->GetHeight(), 
					CSWMemoryFactory::GetInstance(SW_SHARED_SR_MEMORY), 
					pSrcVideo->GetFrameNo(), 
					pSrcVideo->GetRefTime(), 
					pSrcVideo->GetFlag(), 
					pSrcVideo->GetFrameName(),
					pSrcVideo->GetSize())
				)
			{
				SW_COMPONENT_IMAGE sNewComponent;
				pNewVideo->GetImage(&sNewComponent);
				sNewComponent.iSize = sSrcComponent.iSize;
				swpa_memcpy(sNewComponent.rgpbData[0], sSrcComponent.rgpbData[0], sNewComponent.iSize);
				pNewVideo->SetImage(sNewComponent);

				pNewRecord->SetVideo(i, pNewVideo);
				SAFE_RELEASE(pNewVideo);
			}
			else
			{
				SW_TRACE_DEBUG("Err: failed to create video...\n");
				SAFE_RELEASE(pNewRecord);
				return E_FAIL;
			}
		}
	}

	*ppDestRecord = pNewRecord;

	return S_OK;
}


