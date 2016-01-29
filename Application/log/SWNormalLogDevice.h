#ifndef __SW_NORMAL_LOG_DEVICE_H__
#define __SW_NORMAL_LOG_DEVICE_H__
#include "SWLogDevice.h"

class CSWNormalLogDevice : public CSWLogDevice
{
	CLASSINFO(CSWNormalLogDevice, CSWLogDevice)
public:
	CSWNormalLogDevice();
	virtual ~CSWNormalLogDevice();
protected:	
	virtual DWORD GetLevel(void){return NORMAL;}
	virtual VOID ShowLog(LPCSTR szLog);
};
REGISTER_LOG_DEVICE(CSWNormalLogDevice)
#endif
