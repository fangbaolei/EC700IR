#ifndef __SW_KND_RG_DEVICE_H__
#define __SW_KND_RG_DEVICE_H__
#include "SWBaseDevice.h"

class CSWKNDRGDevice : public CSWBaseDevice
{
public:
	CSWKNDRGDevice(DEVICEPARAM *pParam);
	virtual ~CSWKNDRGDevice();
	virtual HRESULT Run(VOID);
};
#endif