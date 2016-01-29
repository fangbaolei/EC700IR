#ifndef __SW_TRAFFIC_GATE_CARLEFT_H__
#define __SW_TRAFFIC_GATE_CARLEFT_H__
#include "SWCarLeft.h"
class CSWTrafficGateCarLeft : public CSWCarLeft
{
	CLASSINFO(CSWTrafficGateCarLeft, CSWCarLeft)
public:
	CSWTrafficGateCarLeft();
	virtual ~CSWTrafficGateCarLeft();
protected:
	virtual  HRESULT   GetEventDetInfo(CSWString& strInfo);
	virtual  HRESULT   BuildPlateString(TiXmlDocument& xmlDoc);
};
REGISTER_CLASS(CSWTrafficGateCarLeft)
#endif