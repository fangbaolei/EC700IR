/*
 * SWLPRVenusTrafficGateParameter.h
 *
 *  Created on: 2013Äê12ÔÂ27ÈÕ
 *      Author: zhaopy
 */

#ifndef __SW_LPR_VENUS_TRAFFICGATE_PARAMETER_H__
#define __SW_LPR_VENUS_TRAFFICGATE_PARAMETER_H__
#include "SWLPRParameter.h"

class CSWLPRVenusTrafficGateParameter  : public CSWLPRParameter
{
	CLASSINFO(CSWLPRVenusTrafficGateParameter, CSWLPRParameter)
public:
	CSWLPRVenusTrafficGateParameter();
	virtual ~CSWLPRVenusTrafficGateParameter();

protected:
	virtual void ResetParam(VOID);

	
	virtual HRESULT Initialize(CSWString strFilePath);

	virtual HRESULT InitSystem(VOID);
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
	virtual HRESULT InitIPTCtrl(VOID);
	
	virtual HRESULT InitOuterCtrl(VOID);

	/**
	*
	*/
	virtual HRESULT InitCamApp(VOID);

	virtual HRESULT InitCamera(VOID);

	/**
	*
	*/
	virtual HRESULT InitCharacter(VOID);

	/**
    *
    */
    virtual HRESULT InitGB28181(VOID);

	
	/**
    *
    */	
	virtual HRESULT InitAutoReboot(VOID);

	/*
	 *@biref Init ONVIF Param
	 *@return On success, S_OK is returned 
	 */
	virtual HRESULT InitONVIF(VOID); 

};
REGISTER_CLASS(CSWLPRVenusTrafficGateParameter)
#endif /* __SW_LPR_VENUS_TRAFFICGATE_PARAMETER_H__ */
