#include "SWFC.h"
#include "SWCSRIKRadar.h"
#include "SWMessage.h"

CSWCSRIKRadar::CSWCSRIKRadar(DEVICEPARAM *pParam,BOOL fRadarTrigger,BOOL fComServer):CSWBaseDevice(pParam)
{
	m_fRadarTrigger=fRadarTrigger;
	m_dwTime=0;
	m_fRadarTrigger?m_fComServer=fComServer:m_fComServer=FALSE;
}

CSWCSRIKRadar::~CSWCSRIKRadar()
{
}

HRESULT CSWCSRIKRadar::Run(VOID)
{	
	BYTE bResultRadar[2];
	
	if(!m_fRadarTrigger)
	{
		BYTE bCmdSend[5]={0xFA,0x38,0x31,0x30,0xFB};
		BYTE bCmdRecv[9];
		BYTE bCmdTrue[5]={0xFA,0x32,0x11,0x31,0xFB};

		while(TRUE)
		{
			if(S_OK==Write(bCmdSend,5,1000))
			{
				swpa_memset(bCmdRecv,0,9);
				if(S_OK==Read(bCmdRecv,9))
				{
					//≥…π¶…Ë÷√
					if(NULL!=swpa_strstr((const char*)bCmdRecv,(const char*)bCmdTrue))
					{
						break;
					}
				}
			}
			swpa_thread_sleep_ms(2000);
		}
		
		SW_TRACE_DEBUG("<SRIKRadar> run...\n");
		while(S_OK == IsValid())
		{
			swpa_memset(bResultRadar,0,2);
			if(S_OK == Read(bResultRadar, 2) && bResultRadar[0] > 0xF7 && bResultRadar[1] > 0 && NULL != m_pOnEvent)
			{
				DWORD dwSpeed = bResultRadar[1];
				DWORD dwRoad = 0xFF;
				DWORD dDirection = bResultRadar[0]-0xF7;
				SW_TRACE_DEBUG("receive radar speed:%d,roadno:%d,direction:%d.\n", dwSpeed, dwRoad, dDirection);
				
				DWORD rgParam[3] = {dwSpeed, dwRoad, dDirection};
				
				CSWMessage::PostMessage(MSG_APP_RADAR_TRIGGER, dwRoad);
				
				m_pOnEvent(m_pvParam, this, SPEED, CSWDateTime::GetSystemTick(), rgParam);
			}			
		}
	}
	else
	{
		if(!m_fComServer)
		{
			BYTE bSpeed=0;
			while(S_OK == IsValid())
			{
				if(S_OK == Read(&bSpeed, 1) && bSpeed > 0 && NULL != m_pOnEvent)
				{
					DWORD dwSpeed = bSpeed;
					DWORD dwRoad = m_pDevParam->iRoadNo;
					SW_TRACE_DEBUG("receive radar speed:%d,roadno:%d.\n", dwSpeed, dwRoad);

					DWORD rgParam[3] = {dwSpeed, dwRoad,0};

					CSWMessage::PostMessage(MSG_APP_RADAR_TRIGGER, dwRoad,m_dwTime++);
			
					m_pOnEvent(m_pvParam, this, SPEED, CSWDateTime::GetSystemTick(), rgParam);
				}
			}
		}
		else
		{
			Clear();
			while(S_OK == IsValid())
			{
				swpa_memset(bResultRadar,0,2);
				HRESULT hr=S_OK;
				if(S_OK == (hr=Read(&bResultRadar, 2)) && NULL != m_pOnEvent)
				{
					if(bResultRadar[0]<=0xF0)
					{
						Read(&bResultRadar, 1);
						continue;
					}

					if(bResultRadar[1]<=0)
					{
						continue;
					}

					DWORD dwSpeed = bResultRadar[1];
					DWORD dwRoad = 0;
					switch(bResultRadar[0]-0xF0)
					{
						case 1:
							dwRoad=m_pDevParam->iComNo2RoadNo1;
							break;
						case 2:
							dwRoad=m_pDevParam->iComNo2RoadNo2;
							break;
						case 3:
							dwRoad=m_pDevParam->iComNo2RoadNo3;
							break;
						case 4:
							dwRoad=m_pDevParam->iComNo2RoadNo4;
							break;
					}
						
					//SW_TRACE_DEBUG("receive radar speed:%d,roadno:%d.\n", dwSpeed, dwRoad);
						
					DWORD rgParam[3] = {dwSpeed, dwRoad,0};
						
					m_pOnEvent(m_pvParam, this, SPEED, CSWDateTime::GetSystemTick(), rgParam);
				}
				else
				{
					//SW_TRACE_DEBUG("receive radar error %lu.\n", hr);
				}
			}
		}
	}
	SW_TRACE_DEBUG("<SRIKRadar> exit.");
	return S_OK;
}

/*HRESULT CSWCSRIKRadar::Run(VOID)
{
	BYTE bSpeed = 0;
	SW_TRACE_DEBUG("<SRIKRadar> run...\n");
	while(S_OK == IsValid())
	{
		if(S_OK == Read(&bSpeed, 1) && bSpeed > 0 && NULL != m_pOnEvent)
		{
			DWORD dwSpeed = bSpeed;
			DWORD dwRoad = m_pDevParam->iRoadNo;
			SW_TRACE_DEBUG("receive radar speed:%d,roadno:%d.\n", dwSpeed, dwRoad);

			DWORD rgParam[3] = {dwSpeed, dwRoad,0};

			CSWMessage::PostMessage(MSG_APP_RADAR_TRIGGER, dwRoad,m_dwTime++);
			
			m_pOnEvent(m_pvParam, this, SPEED, CSWDateTime::GetSystemTick(), rgParam);
		}
	}
	SW_TRACE_DEBUG("<SRIKRadar> exit.");
	return S_OK;
}*/


