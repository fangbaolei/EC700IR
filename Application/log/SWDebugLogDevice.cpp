#include "SWFC.h"
#include "SWDebugLogDevice.h"

CSWDebugLogDevice::CSWDebugLogDevice()
{
}

CSWDebugLogDevice::~CSWDebugLogDevice()
{
}

VOID CSWDebugLogDevice::ShowLog(LPCSTR szLog)
{
#if 0	
	if(m_cFile.Open("EEPROM/0/DEVICE_LOG", "a+"))
	{
		DWORD iSize = swpa_strlen(szLog) + 1, iWriteSize = 0;
		if(S_OK == m_cFile.Write((PVOID)szLog, iSize, &iWriteSize))
		{
			m_cFile.Close();
		}
	}
#endif	
	// 直接通过串口输出
	printf("%s", szLog);
}

