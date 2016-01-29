#ifndef __SW_LPR_CAPFACE_PARAMETER_H__
#define __SW_LPR_CAPFACE_PARAMETER_H__
#include "SWLPRParameter.h"

class CSWLPRCapFaceParameter : public CSWLPRParameter
{
	CLASSINFO(CSWLPRCapFaceParameter, CSWLPRParameter)
public:
	CSWLPRCapFaceParameter();
	virtual ~CSWLPRCapFaceParameter();
protected:
	
	/**
	 *@brief 
	 */
	virtual HRESULT InitTracker(VOID);
	
	/**
	 *@brief 
	 */
	virtual HRESULT InitHvDsp(VOID);
	
	virtual HRESULT InitCamApp(VOID);
	
	virtual HRESULT InitIPTCtrl(VOID);
};
REGISTER_CLASS(CSWLPRCapFaceParameter)
#endif

