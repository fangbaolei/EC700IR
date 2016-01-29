/*
 * SWLPRVenusTollGateParameter.h
 *
 *  Created on: 2013Äê12ÔÂ27ÈÕ
 *      Author: qinjj
 */

#ifndef __SW_LPR_VENUS_CAMERA_PARAMETER_H__
#define __SW_LPR_VENUS_CAMERA_PARAMETER_H__
#include "SWLPRParameter.h"

class SWLPRVenusCameraParameter  : public CSWLPRParameter
{
    CLASSINFO(SWLPRVenusCameraParameter, CSWLPRParameter)
public:
    SWLPRVenusCameraParameter();
    virtual ~SWLPRVenusCameraParameter();

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
    virtual HRESULT InitOuterCtrl(VOID);


    /**
     *
     */
    virtual HRESULT InitIPTCtrl(VOID);

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

	/*
	 *@biref Init ONVIF Param
	 *@return On success, S_OK is returned 
	 */
	virtual HRESULT InitONVIF(VOID); 
};
REGISTER_CLASS(SWLPRVenusCameraParameter)
#endif /* __SW_LPR_VENUS_TOLLGATE_PARAMETER_H__ */
