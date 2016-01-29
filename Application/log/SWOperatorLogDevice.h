#ifndef __SW_OPERATOR_LOG_DEVICE_H__
#define __SW_OPERATOR_LOG_DEVICE_H__
#include "SWLogDevice.h"

class CSWOperatorLogDevice : public CSWLogDevice
{
	CLASSINFO(CSWOperatorLogDevice, CSWLogDevice)
public:
	CSWOperatorLogDevice();
	virtual ~CSWOperatorLogDevice();
protected:
	virtual DWORD GetLevel(void){return OPERATOR;}
	virtual VOID ShowLog(LPCSTR szLog);
};
REGISTER_LOG_DEVICE(CSWOperatorLogDevice)
#endif
