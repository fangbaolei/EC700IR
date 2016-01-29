/*
 * SWJZRadar.cpp
 *
 *  Created on: 2013年12月4日
 *      Author: qinjj
 */

#include "SWFC.h"
#include "SWJZRadar.h"

#define DEBUG_JZ_RADAR
#ifdef DEBUG_JZ_RADAR
#define SW_TRACE_DEBUG SW_TRACE_NORMAL
#endif

CSWJZRadar::CSWJZRadar(DEVICEPARAM *pParam):CSWBaseDevice(pParam)
{
	// TODO Auto-generated constructor stub
}

CSWJZRadar::~CSWJZRadar() {
	// TODO Auto-generated destructor stub
}

HRESULT CSWJZRadar::Run(VOID)
{
	typedef struct
	{
		BYTE bDirection;	//方向
		BYTE bSpeed;		//速度
	}tJZRadarData;

	//example
	//F8 F0  240km/h
	//F9 B0  176km/h
	tJZRadarData buf;

	SW_TRACE_NORMAL("<CSWJZRadar> running...............................\n");

	while(S_OK == IsValid())
	{
		if(S_OK != Read(&buf, sizeof(tJZRadarData)))
		{
			SW_TRACE_NORMAL("<CSWJZRadar> read com failed!...............\n");
			continue;
		}
		if((0xF8 == buf.bDirection 		//
				|| 0xF9 == buf.bDirection)	//
				&& buf.bSpeed > 0)
		{
			DWORD dwSpeed = buf.bSpeed;
			DWORD dwRoad = m_pDevParam->iRoadNo;
			SW_TRACE_NORMAL("receive radar speed:%d,roadno:%d.............\n", dwSpeed, dwRoad);

			DWORD rgParam[2] = {dwSpeed, dwRoad};
			
			if( NULL != m_pOnEvent)
				m_pOnEvent(m_pvParam, this, SPEED, CSWDateTime::GetSystemTick(), rgParam);
		}
		else
		{
			SW_TRACE_NORMAL("<CSWJZRadar>  read data err: 0x%x,0x%x..............\n",
				buf.bDirection,buf.bSpeed);
		}
	}
	SW_TRACE_DEBUG("<CSWJZRadar> exit...........................");

	return S_OK;
}

