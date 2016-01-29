#ifndef __SW_TOLL_GATE_CARLEFT_H__
#define __SW_TOLL_GATE_CARLEFT_H__
#include "SWCarLeft.h"
class CSWTollGateCarLeft : public CSWCarLeft
{
	CLASSINFO(CSWTollGateCarLeft, CSWCarLeft)
public:
	CSWTollGateCarLeft();
	virtual ~CSWTollGateCarLeft();
protected:
	virtual  HRESULT   GetEventDetInfo(CSWString& strInfo);
	virtual  HRESULT   BuildPlateString(TiXmlDocument& xmlDoc);
};
REGISTER_CLASS(CSWTollGateCarLeft)
#endif
