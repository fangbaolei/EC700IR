#include "SWFC.h"
#include "SWIPNCTansformFilter.h"

CSWIPNCTransformFilter::CSWIPNCTransformFilter():CSWBaseFilter(1,2)
{
	GetIn(0)->AddObject(CLASSID(CSWImage));
	GetOut(0)->AddObject(CLASSID(CSWImage));
	GetOut(1)->AddObject(CLASSID(CSWImage));
}

CSWIPNCTransformFilter::~CSWIPNCTransformFilter()
{
}

HRESULT CSWIPNCTransformFilter::Receive(CSWObject* obj)
{
	if(IsDecendant(CSWImage, obj))
	{
		CSWImage* pImage = (CSWImage *)obj;
	}
	return S_OK;
}

HRESULT CSWIPNCTransformFilter::Run()
{
	swpa_ipnc_setcallback(SWPA_LINK_VPSS, 0xFF, OnResult, this);
	return CSWBaseFilter::Run();
}

HRESULT CSWIPNCTransformFilter::Stop()
{
	//clear the callback 
	swpa_ipnc_setcallback(SWPA_LINK_VPSS, 0xFF, NULL, NULL);
	return CSWBaseFilter::Stop();
}

int CSWIPNCTransformFilter::OnResult(void *pContext, int type, void *struct_ptr)
{
	CSWIPNCTransformFilter* pThis = (CSWIPNCTransformFilter *)pContext;
	if(type == CALLBACK_TYPE_IMAGE)
	{
		IMAGE *image = (IMAGE *)struct_ptr;
		CSWImage* pImage = NULL;
		if(S_OK == CSWImage::CreateSWImage(
				  &pImage
				, SW_IMAGE_BT1120
				, image->pitch
				, image->width
				, image->height
				, CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY)
				, image->data.addr
				, image->data.phys
				, image->data.size
				, 0
				, 0
				, 0
				, "VPIF") )
		{
			static INT iFrameNum = 0;
			pImage->SetFrameNo(++iFrameNum);
			pImage->SetRefTime(CSWDateTime::GetSystemTick());
			pThis->GetOut(image->channel)->Deliver(pImage);
			pImage->Release();
		}
		else
		{
			SW_TRACE_NORMAL("CSWIPNCTransformFilter::OnResult, failed...");
		}
		return 1;
	}
	return 0;
}
