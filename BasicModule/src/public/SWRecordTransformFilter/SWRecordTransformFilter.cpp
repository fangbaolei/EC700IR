


#include "SWFC.h"
#include "SWRecordTransformFilter.h"
#include "SWRecord.h"
#include "SWCarLeft.h"

#define PRINT SW_TRACE_DEBUG

CSWRecordTransformFilter::CSWRecordTransformFilter()
	:m_fInited(FALSE)
	,CSWBaseFilter(1, 1)
{
	GetIn(0)->AddObject(CLASSID(CSWCarLeft));
	//GetIn(1)->AddObject(CLASSID(CSWString));
	GetOut(0)->AddObject(CLASSID(CSWRecord));
	//GetOut(1)->AddObject(CLASSID(CSWRecord));
	m_fInited = TRUE;
}
	

CSWRecordTransformFilter::~CSWRecordTransformFilter()
{
	if (!m_fInited)
	{
		return ;
	}
	
	
	m_fInited = FALSE;
}




HRESULT CSWRecordTransformFilter::Receive(CSWObject * pObj)
{
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return E_NOTIMPL;
	}
	
	if (IsDecendant(CSWCarLeft, pObj))
	{		
		PRINT("Info: RecordTransformFilter got a CSWCarLeft obj\n");
		static DWORD dwCarID = 0;
		
		CSWCarLeft * pCarLeft = (CSWCarLeft*)pObj;
		
		CSWRecord * pRecord = new CSWRecord();
		if (NULL == pRecord || FAILED(pRecord->Initialize()))
		{
			PRINT("Err: no memory for pRecord\n");
			SAFE_RELEASE(pRecord);
			return E_OUTOFMEMORY;
		}

		if (FAILED(pRecord->SetXml(pCarLeft, dwCarID)))
		{
			PRINT("Err: failed to set xml\n");
			SAFE_RELEASE(pRecord);
			return E_FAIL;
		}

		if (FAILED(pRecord->SetPTType(pCarLeft->GetPTType())))
		{
			PRINT("Err: failed to set PT type\n");
			SAFE_RELEASE(pRecord);
			return E_FAIL;
		}

		//PRINT("Record PTType = %d\n", pRecord->GetPTType());

		if (FAILED(pRecord->SetUnSurePeccancy(pCarLeft->GetUnSurePeccancy())))
		{
			PRINT("Err: failed to set UnSurePeccancy\n");
			SAFE_RELEASE(pRecord);
			return E_FAIL;
		}

		//PRINT("Record UnSurePeccancy = %d\n", pRecord->GetUnSurePeccancy());


		if (FAILED(pRecord->SetRefTime(CSWDateTime::GetSystemTick())))
		{
			PRINT("Err: failed to set RefTime\n");
			SAFE_RELEASE(pRecord);
			return E_FAIL;
		}

		if (FAILED(pRecord->SetCarArriveTime(pCarLeft->GetCarArriveTime())))
		{
			PRINT("Err: failed to set CarArriveTime\n");
			SAFE_RELEASE(pRecord);
			return E_FAIL;
		}

		for (DWORD i=0; i<pRecord->GetImageTypeCount(); i++)
		{
			if (NULL != pCarLeft->GetImage(i))
			{
				CSWImage * pImage = pCarLeft->GetImage(i);
				
				if (SW_IMAGE_YUV_422 == pImage->GetType())
				{
					pImage = CreateMyYUVImage(pCarLeft->GetImage(i));
					if (NULL == pImage)
					{
						PRINT("Err: failed to create YUV image #%d\n", i);
						SAFE_RELEASE(pRecord);
						return E_FAIL;
					}
				}

				if (FAILED(pRecord->SetImage(i, pImage)))
				{
					PRINT("Err: failed to set image #%d\n", i);
					SAFE_RELEASE(pRecord);
					return E_FAIL;
				}

				if (SW_IMAGE_YUV_422 == pImage->GetType())
				{
					SAFE_RELEASE(pImage);
				}
			}
		}

		for(INT i = 0; i < pCarLeft->GetVideoCount(); i++)
		{
			CSWImage *pVideo = pCarLeft->GetVideo(i);
			if(FAILED(pRecord->SetVideo(i, pVideo)))
			{
				PRINT("Err: failed to set video #%d\n", i);
				SAFE_RELEASE(pRecord);
				return E_FAIL;
			}
		}

		//deliver it immediately
		for (INT i=0; i<GetOutCount(); i++)
		{
			GetOut(i)->Deliver(pRecord);
			SW_TRACE_DEBUG("send pRecord i=%d\n",i);
		}

		SAFE_RELEASE(pRecord);

		dwCarID++;
	}
	else if (IsDecendant(CSWString, pObj))
	{
		if (NULL != pObj)
		{
			PRINT("Info: RecordTransformFilter got a CSWString obj\n");
			
			CSWString * pstrInfo = (CSWString*)pObj;

			TiXmlDocument xmlDoc;
			
			TiXmlDeclaration *pDeclaration = NULL;
			TiXmlElement *pRootEle = NULL;

			pDeclaration = new TiXmlDeclaration("1.0","GB2312","yes");
			if (NULL == pDeclaration)
			{
				PRINT("Err: no memory for pDeclaration\n");
				return E_OUTOFMEMORY;
			}
			xmlDoc.LinkEndChild(pDeclaration);
			

			pRootEle = new TiXmlElement("TrafficInfo");
			if (NULL == pRootEle)
			{	
				PRINT("Err: no memory for pRootEle\n");
				return E_OUTOFMEMORY;
			}
			xmlDoc.LinkEndChild(pRootEle);

//			PRINT("Info: TrafficInfo: %s\n", (const CHAR*)*pstrInfo);
			
			TiXmlText* pTxtInfo = new TiXmlText((const CHAR*)*pstrInfo);
			if (NULL == pTxtInfo)
			{
				PRINT("Err: no memory for pTxtInfo\n");
				return E_OUTOFMEMORY;
			}
			pRootEle->LinkEndChild(pTxtInfo);


			CSWRecord * pRecord = new CSWRecord();
			if (NULL == pRecord || FAILED(pRecord->Initialize()))
			{
				PRINT("Err: no memory for pRecord\n");
				SAFE_RELEASE(pRecord);
				return E_OUTOFMEMORY;
			}


			pRecord->SetXml(&xmlDoc);
			pRecord->SetTrafficInfoFlag(TRUE);
			pRecord->SetRefTime(CSWDateTime::GetSystemTick());
			pRecord->SetCarArriveTime(CSWDateTime::GetSystemTick());

			if (FAILED(pRecord->SetPTType(PT_NORMAL)))
			{
				PRINT("Err: failed to set PT type\n");
				SAFE_RELEASE(pRecord);
				return E_FAIL;
			}

			//PRINT("record PTType = %d\n", pRecord->GetPTType());

			//deliver it immediately
			for (INT i=0; i<GetOutCount(); i++)
			{
				GetOut(i)->Deliver(pRecord);
			}

			SAFE_RELEASE(pRecord);			
		}
		
	}

	return S_OK;
}



CSWImage * CSWRecordTransformFilter::CreateMyYUVImage(CSWImage* pImage)
{	
	HRESULT hr = S_OK;
	
	if (!m_fInited)
	{
		PRINT("Err: not initialized yet\n");
		return NULL;
	}

	
	//create a new image for history image file transmitting
	CSWMemoryFactory * pMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
	if (NULL == pMemoryFactory)
	{
		PRINT("Err: no enough memory for pMemoryFactory\n");
		return NULL;
	}

	SW_COMPONENT_IMAGE sComponentImage;
	if (FAILED(pImage->GetImage(&sComponentImage)))
	{
		PRINT("Err: failed to get image\n");
		return NULL;
	}

	DWORD dwSize = pImage->GetHeight() * pImage->GetWidth() * 2;

	PBYTE pbData = (PBYTE)swpa_mem_alloc(dwSize);
	if (NULL == pbData)
	{
		PRINT("Err: no enough memory for pbData\n");
		return NULL;
	}
	

	PBYTE pbOffset = pbData;
    BYTE *pSrc = sComponentImage.rgpbData[0]; //Y
	for( int k = 0; k < sComponentImage.iHeight; ++k )
	{
		swpa_memcpy(pbOffset, pSrc, sComponentImage.iWidth);
		pbOffset += sComponentImage.iWidth;
		pSrc += sComponentImage.rgiStrideWidth[0];
		
	}
    
    pSrc = sComponentImage.rgpbData[1]; //U
	for( int k = 0; k < sComponentImage.iHeight; ++k )
	{
		swpa_memcpy(pbOffset, pSrc, sComponentImage.iWidth / 2);
		pbOffset += sComponentImage.iWidth / 2;
		pSrc += sComponentImage.rgiStrideWidth[1];
		
	}
    
    pSrc = sComponentImage.rgpbData[2]; //V
	for( int k = 0; k < sComponentImage.iHeight; ++k )
	{
		swpa_memcpy(pbOffset, pSrc, sComponentImage.iWidth / 2);
		pbOffset += sComponentImage.iWidth / 2;
		pSrc += sComponentImage.rgiStrideWidth[2];
		
	}

	CSWImage * pNewImage = NULL;		
	hr = CSWImage::CreateSWImage(&pNewImage, 
		pImage->GetType(), 
		pImage->GetWidth(), 
		pImage->GetHeight(),
		pMemoryFactory, 
		pImage->GetFrameNo(), 
		pImage->GetRefTime(), 
		pImage->IsCaptureImage(),
		pImage->GetFrameName(),
		dwSize);
	if (S_OK != hr)
	{
		PRINT("<H264 HDD> Err: failed to CreateSWImage() [%u]\n", hr);
		SAFE_MEM_FREE(pbData);
		return NULL;
	}

	SW_COMPONENT_IMAGE sNewComponentImage;
	pNewImage->GetImage(&sNewComponentImage);
	
	swpa_memcpy(sNewComponentImage.rgpbData[0], pbData, dwSize);

	SAFE_MEM_FREE(pbData);

	sNewComponentImage.iSize = dwSize;
	pNewImage->SetImage(sNewComponentImage);


	return pNewImage;

}




