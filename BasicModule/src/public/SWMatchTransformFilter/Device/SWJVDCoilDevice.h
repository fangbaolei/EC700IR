#ifndef __SW_JVD_COID_DEVICE_H__
#define __SW_JVD_COID_DEVICE_H__
#include "SWBaseDevice.h"

class CSWJVDCoilDevice : public CSWBaseDevice
{
	CLASSINFO(CSWJVDCoilDevice, CSWBaseDevice)
public:
	CSWJVDCoilDevice(DEVICEPARAM *pParam);
	virtual ~CSWJVDCoilDevice();
	virtual HRESULT Run(VOID);
protected:
	DWORD CalcSpeed(INT iTime1, INT iTime2, DWORD dwInstance);	
};
#endif