#include "SWFC.h"
#include "SWKNDRGDevice.h"
#include "SWMessage.h"

CSWKNDRGDevice::CSWKNDRGDevice(DEVICEPARAM *pParam):CSWBaseDevice(pParam)
{
}

CSWKNDRGDevice::~CSWKNDRGDevice()
{
}

HRESULT CSWKNDRGDevice::Run(VOID)
{
	BYTE rgbData[] = {0x01, 0x03, 0x01, 0x04, 0x00, 0x08, 0x04, 0x31};
	BYTE rgbCmd[21];
	BYTE bLastLevel = 0, bLevel = 0;
	while(S_OK == IsValid())
	{
		if(S_OK == Write(rgbData, sizeof(rgbData)))
		{
			CSWApplication::Sleep(10); 
			swpa_memset(rgbCmd, 0, 21);
			if(S_OK == Read(rgbCmd, 21))
			{
				if(rgbCmd[0] == 0x01 
				&& rgbCmd[1] == 0x03 
				&& rgbCmd[2] == 0x10 
				&& rgbCmd[3] == 0x00 
				&& rgbCmd[5] == 0x00 
				&& rgbCmd[7] == 0x00 
				&& rgbCmd[9] == 0x00 
				&& rgbCmd[11] == 0x00 
				&& rgbCmd[13] == 0x00
				&& rgbCmd[15] == 0x00
				&& rgbCmd[17] == 0x00
				)
				{
					bLevel = (rgbCmd[4] & 0x01)|((rgbCmd[6] & 0x01)  << 1)|((rgbCmd[8] & 0x01)  << 2)|((rgbCmd[10] & 0x01) << 3)|((rgbCmd[12] & 0x01) << 4)|((rgbCmd[14] & 0x01) << 5)|((rgbCmd[16] & 0x01) << 6)|((rgbCmd[18] & 0x01) << 7);
					if(bLastLevel != bLevel)
					{
						bLastLevel = bLevel;
						CSWMessage::SendMessage(MSG_RECOGNIZE_IOEVENT, bLevel);
					}
				}
			}
			else
			{
				char szCmd[255] = "";
				for(int i = 0; i < 21; i++)
				{
					swpa_sprintf(szCmd + swpa_strlen(szCmd), "0x%02x ", rgbCmd[i]);
				}
				SW_TRACE_DEBUG("CSWKNDRGDevice::Read failed[%s]", szCmd);
			}
		}
		else
		{
			SW_TRACE_DEBUG("CSWKNDRGDevice::Write failed.");
		}
		CSWApplication::Sleep(500);
	}
}
