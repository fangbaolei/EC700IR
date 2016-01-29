#include "CSRIRadar.h"
#include "HvPciLinkApi.h"

CCSRIRadar::CCSRIRadar()
{
}

CCSRIRadar::~CCSRIRadar()
{
}

bool CCSRIRadar::Initialize(void)
{
    if (S_OK == CSerialBase::Open("/dev/ttyS0"))
    {
    	HV_Trace(5, "open /dev/ttyS0 ok\n");
        CSerialBase::SetAttr(9600, 8, 0,1);
        Start(NULL);
        return true;
    }
    else
    {
    	HV_Trace(5, "open /dev/ttyS0 failed\n");
    }
    return false;
}

HRESULT CCSRIRadar::Run(void* pvParam)
{
    BYTE8* pbData = new BYTE8[100];
    BYTE8* buf = pbData;
    BYTE8* bEventId = buf;
    *bEventId = 0;
    buf++;
    BYTE8* bType = buf;
    *bType = 0x08;
    buf++;
    BYTE8* bEventDeviceType = buf;
    *bEventDeviceType = 0x01;
    buf++;
    BYTE8* dwTimePos = buf;
    buf += 4;
    BYTE8* rgbData = buf;

    DWORD32 dwTime, dwSize = buf - pbData + 1;
	DWORD32 dwLastTimeTick = GetSystemTick();

    while (!m_fExit)
    {
        if (1 == CSerialBase::Recv(&rgbData[0], 1, 3000))
        {
            if (rgbData[0] > 0)
            {
            	HV_Trace(5, "receive a speed from CSR-I Radar, value=%d\n", rgbData[0]);
                dwTime = GetSystemTick();
                memcpy(dwTimePos, &dwTime, sizeof(DWORD32));
                //send to slave.
                g_cHvPciLinkApi.SendData(PCILINK_PCIIPT, pbData, dwSize);
            }
        }
        if(GetSystemTick() - dwLastTimeTick >= 3000)
        {
        	dwLastTimeTick = GetSystemTick();
        	HV_Trace(5, "CCSRIRadar working....\n");
        }
    }
    delete []pbData;
    return S_OK;
}
