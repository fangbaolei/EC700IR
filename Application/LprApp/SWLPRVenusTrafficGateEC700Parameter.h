/*
 * SWLPRVenusTrafficGateEC700Parameter.h
 *
 *  Created on: 2013Äê12ÔÂ27ÈÕ
 *      Author: zhaopy
 */

#ifndef __SW_LPR_VENUS_TRAFFICGATE_EC700_PARAMETER_H__
#define __SW_LPR_VENUS_TRAFFICGATE_EC700_PARAMETER_H__
#include "SWLPRParameter.h"

class CSWLPRVenusTrafficGateEC700Parameter  : public CSWLPRParameter
{
	CLASSINFO(CSWLPRVenusTrafficGateEC700Parameter, CSWLPRParameter)
public:
	CSWLPRVenusTrafficGateEC700Parameter();
	virtual ~CSWLPRVenusTrafficGateEC700Parameter();

protected:
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
    virtual HRESULT InitScaleSpeed(VOID);

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
REGISTER_CLASS(CSWLPRVenusTrafficGateEC700Parameter)
#endif /* __SW_LPR_VENUS_TRAFFICGATE_EC700_PARAMETER_H__ */
