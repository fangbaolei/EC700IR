/*
 * SWRAPIERRadar.cpp
 *
 *  Created on: 2013年12月4日
 *      Author: qinjj
 */
#include "SWFC.h"
#include "SWRAPIERRadar.h"

#define DEBUG_RAPIER_RADAR
#ifdef DEBUG_RAPIER_RADAR
#define SW_TRACE_DEBUG SW_TRACE_NORMAL
#endif

CSWRAPIERRadar::CSWRAPIERRadar(DEVICEPARAM *pParam):CSWBaseDevice(pParam)
{
	// TODO Auto-generated constructor stub
}

CSWRAPIERRadar::~CSWRAPIERRadar() {
	// TODO Auto-generated destructor stub
}

HRESULT CSWRAPIERRadar::Run(VOID)
{
	//BYTE bSpeed = 0;
	typedef struct
	{
		BYTE bTrigger;		//触发信号
		BYTE bSpeedHead;	//速度数据包前缀
		BYTE bSpeed;		//速度值，十六进制，单位KM/H
		BYTE bReserve;		//内部字节
	}tRAPIERRadarData;
	//example
	//FC FA E0 FF	车头模式 224km/h
	//FB FD C0 FF	车尾模式 192km/h
	tRAPIERRadarData buf;
	SW_TRACE_DEBUG("<CSWRAPIERRadar> running.................................\n");

	while(S_OK == IsValid())
	{
		if(S_OK != Read(&buf, sizeof(tRAPIERRadarData)))
		{
			SW_TRACE_NORMAL("<CSWRAPIERRadar> read com failed!..............\n");
			continue;
		}
		if( ((0xFC == buf.bTrigger && 0xFA == buf.bSpeedHead) 	//车头模式
				|| (0xFB == buf.bTrigger && 0xFD == buf.bSpeedHead))	//车尾模式
				&& buf.bSpeed > 0)
		{
			DWORD dwSpeed = buf.bSpeed;
			DWORD dwRoad = m_pDevParam->iRoadNo;
			SW_TRACE_DEBUG("receive radar speed:%d,roadno:%d.............\n", dwSpeed, dwRoad);

			DWORD rgParam[2] = {dwSpeed, dwRoad};
			
			if (NULL != m_pOnEvent)
				m_pOnEvent(m_pvParam, this, SPEED, CSWDateTime::GetSystemTick(), rgParam);
		}
		else
		{
			SW_TRACE_NORMAL("<CSWRAPIERRadar>  read data err: 0x%x,0x%x,0x%x,0x%x...........\n",
				buf.bTrigger,buf.bSpeedHead,buf.bSpeed,buf.bReserve);
		}
	}
	SW_TRACE_DEBUG("<CSWRAPIERRadar> exit..............................");

	return S_OK;
}
