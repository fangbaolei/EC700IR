#ifndef __SW_EEPROM_LOG_DEVICE_H__
#define __SW_EEPROM_LOG_DEVICE_H__
#include "SWLogDevice.h"

class CSWEEPROMLogDevice : public CSWLogDevice
{
	CLASSINFO(CSWEEPROMLogDevice, CSWLogDevice)
public:
  CSWEEPROMLogDevice();
	virtual ~CSWEEPROMLogDevice();
protected:
	virtual DWORD GetLevel(void){return ERROR;}
	virtual VOID ShowLog(LPCSTR szLog);
};
REGISTER_LOG_DEVICE(CSWEEPROMLogDevice)
#endif
