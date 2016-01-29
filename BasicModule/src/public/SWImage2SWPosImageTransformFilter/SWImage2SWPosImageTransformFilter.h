#ifndef __SW_IMAGE_2_POS_IMAGE_TANSFORMFILTER_H__
#define __SW_IMAGE_2_POS_IMAGE_TANSFORMFILTER_H__
#include "SWBaseFilter.h"

class CSWImage2SWPosImageTransformFilter : public CSWBaseFilter
{
	CLASSINFO(CSWImage2SWPosImageTransformFilter, CSWBaseFilter)
public:
	CSWImage2SWPosImageTransformFilter();
	virtual ~CSWImage2SWPosImageTransformFilter();
protected:
	virtual HRESULT Receive(CSWObject* obj);	
};
REGISTER_CLASS(CSWImage2SWPosImageTransformFilter)
#endif