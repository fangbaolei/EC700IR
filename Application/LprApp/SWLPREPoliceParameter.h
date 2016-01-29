#ifndef __SW_LPR_EPOLICE_PARAMETER_H__
#define __SW_LPR_EPOLICE_PARAMETER_H__
#include "SWLPRParameter.h"

class CSWLPREPoliceParameter : public CSWLPRParameter
{
	CLASSINFO(CSWLPREPoliceParameter, CSWLPRParameter)
public:
	CSWLPREPoliceParameter();
	virtual ~CSWLPREPoliceParameter();
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
	virtual HRESULT InitScaleSpeed(VOID);
	
	virtual HRESULT InitCamApp(VOID);
	
	virtual HRESULT InitOuterCtrl(VOID);
	
protected:
	VOID LoadTrafficLightParam(VOID);
};
REGISTER_CLASS(CSWLPREPoliceParameter)
#endif

