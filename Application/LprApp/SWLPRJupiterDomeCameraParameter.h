/*
 * SWLPRJupiterDomeCameraParameter.h
 *
 *  Created on: 2013Äê12ÔÂ27ÈÕ
 *      Author: zhaopy
 */

#ifndef __SW_LPR_JUPITER_DOMECAMERA_PARAMETER_H__
#define __SW_LPR_JUPITER_DOMECAMERA_PARAMETER_H__
#include "SWLPRParameter.h"

class CSWLPRJupiterDomeCameraParameter  : public CSWLPRParameter
{
	CLASSINFO(CSWLPRJupiterDomeCameraParameter, CSWLPRParameter)
public:
	CSWLPRJupiterDomeCameraParameter();
	virtual ~CSWLPRJupiterDomeCameraParameter();

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

	virtual HRESULT InitONVIF(VOID);

};
REGISTER_CLASS(CSWLPRJupiterDomeCameraParameter)
#endif /* __SW_LPR_JUPITER_DOMECAMERA_PARAMETER_H__ */
