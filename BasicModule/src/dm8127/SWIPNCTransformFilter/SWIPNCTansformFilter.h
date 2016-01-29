#ifndef __SW_IPNC_TANSFORM_FILTER_H__
#define __SW_IPNC_TANSFORM_FILTER_H__
#include "SWBaseFilter.h"

class CSWIPNCTransformFilter : public CSWBaseFilter
{
	CLASSINFO(CSWIPNCTransformFilter, CSWBaseFilter)
public:
	CSWIPNCTransformFilter();
	virtual ~CSWIPNCTransformFilter();
protected:
	virtual HRESULT Receive(CSWObject* obj);
	virtual HRESULT Run();
	virtual HRESULT Stop();
protected:
	static int OnResult(void *pContext, int type, void *struct_ptr);	
};
REGISTER_CLASS(CSWIPNCTransformFilter)
#endif