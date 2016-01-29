#ifndef __SW_DEBUG_LOG_DEVICE_H__
#define __SW_DEBUG_LOG_DEVICE_H__
#include "SWLogDevice.h"

class CSWDebugLogDevice : public CSWLogDevice
{
	CLASSINFO(CSWDebugLogDevice, CSWLogDevice)
public:
	CSWDebugLogDevice();
	virtual ~CSWDebugLogDevice();
protected:
	virtual DWORD GetLevel(void){return DEBUG;}
	virtual VOID ShowLog(LPCSTR szLog);
protected:
	CSWFile m_cFile;
};
REGISTER_LOG_DEVICE(CSWDebugLogDevice)
#endif
