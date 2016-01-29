#include "SWFC.h"
#include "SWPosImage.h"
#include "SWImage2SWPosImageTransformFilter.h"

CSWImage2SWPosImageTransformFilter::CSWImage2SWPosImageTransformFilter():CSWBaseFilter(1,1)
{
	GetIn(0)->AddObject(CLASSID(CSWImage));
	GetOut(0)->AddObject(CLASSID(CSWPosImage));
}

CSWImage2SWPosImageTransformFilter::~CSWImage2SWPosImageTransformFilter()
{
}

HRESULT CSWImage2SWPosImageTransformFilter::Receive(CSWObject* obj)
{
	if(IsDecendant(CSWImage, obj))
	{
		CSWPosImage *pImage = new CSWPosImage((CSWImage *)obj);
		GetOut(0)->Deliver(pImage);
		SAFE_RELEASE(pImage);
	}
	return S_OK;
}