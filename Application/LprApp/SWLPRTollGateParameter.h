#ifndef __SW_LPR_TOLLGATE_PARAMETER_H__
#define __SW_LPR_TOLLGATE_PARAMETER_H__
#include "SWLPRParameter.h"

class CSWLPRTollGateParameter : public CSWLPRParameter
{
	CLASSINFO(CSWLPRTollGateParameter, CSWLPRParameter)
public:
	CSWLPRTollGateParameter();
	virtual ~CSWLPRTollGateParameter();
protected:
	/**
	 *@brief
	 */
	virtual HRESULT InitTracker(VOID);

	/**
	 *@brief
	 */
	virtual HRESULT InitHvDsp(VOID);

	/**
	 *
	 */
	virtual HRESULT InitOuterCtrl(VOID);
	
	/**
	 *
	 *
	 */
	virtual HRESULT InitScaleSpeed(VOID);
	
	/**
	 *
	 */
	virtual HRESULT InitIPTCtrl(VOID);
	
	virtual HRESULT Init368(VOID);
};
REGISTER_CLASS(CSWLPRTollGateParameter)
#endif
