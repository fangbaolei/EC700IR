#ifndef __SW_CSRIK_RADAR_H__
#define __SW_CSRIK_RADAR_H__
#include "SWBaseDevice.h"
/*
 *@brief ´¨ËÙÀ×´ï²âËÙ
**/

class CSWCSRIKRadar : public CSWBaseDevice
{
	CLASSINFO(CSWCSRIKRadar, CSWBaseDevice)
public:
	CSWCSRIKRadar(DEVICEPARAM *pParam,BOOL fRadarTrigger,BOOL fComServer);
	virtual ~CSWCSRIKRadar();
	virtual HRESULT Run(VOID);
private:
	BOOL m_fRadarTrigger;
	DWORD m_dwTime;
	BOOL m_fComServer;
};
#endif

