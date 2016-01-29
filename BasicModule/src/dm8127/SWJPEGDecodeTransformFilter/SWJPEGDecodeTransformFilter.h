#ifndef __SW_JPEG_DECODE_TRANSFORM_FILTER_H__
#define __SW_JPEG_DECODE_TRANSFORM_FILTER_H__
#include "SWBaseFilter.h"

class CSWJPEGDecodeTransformFilter : public CSWBaseFilter
{
	CLASSINFO(CSWJPEGDecodeTransformFilter,CSWBaseFilter)
public:
	CSWJPEGDecodeTransformFilter();
	virtual ~CSWJPEGDecodeTransformFilter();
protected:
	virtual HRESULT Run();
	virtual HRESULT Receive(CSWObject* obj);
	static INT OnResult(VOID *pContext, INT type, VOID *struct_ptr);	
private:
	INT    m_lstImage;
};
REGISTER_CLASS(CSWJPEGDecodeTransformFilter)
#endif