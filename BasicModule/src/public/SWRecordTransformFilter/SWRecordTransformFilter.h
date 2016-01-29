
#ifndef _SW_RECORD_TRANSFORM_FILTER_H_
#define _SW_RECORD_TRANSFORM_FILTER_H_

#include "SWBaseFilter.h"
#include "SWImage.h"


class CSWRecordTransformFilter : public CSWBaseFilter
{
	CLASSINFO(CSWRecordTransformFilter, CSWBaseFilter);
		
public :
	CSWRecordTransformFilter();
	
	virtual ~CSWRecordTransformFilter();

	///HRESULT Initialize();
	
	virtual HRESULT Receive(CSWObject* obj);

protected:

	SW_BEGIN_DISP_MAP(CSWRecordTransformFilter, CSWBaseFilter)
		//SW_DISP_METHOD(Receive, 1)
	SW_END_DISP_MAP();

	
	CSWImage * CreateMyYUVImage(CSWImage* pImage);

private:
	BOOL			m_fInited;
};

REGISTER_CLASS(CSWRecordTransformFilter)


#endif //_SW_RECORD_TRANSFORM_FILTER_H_




