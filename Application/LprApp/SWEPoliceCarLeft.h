#ifndef __SW_EPOLICE_CARLEFT_H__
#define __SW_EPOLICE_CARLEFT_H__
#include "SWCarLeft.h"
class CSWEPoliceCarLeft : public CSWCarLeft
{
	CLASSINFO(CSWEPoliceCarLeft, CSWCarLeft)
public:
	CSWEPoliceCarLeft();
	virtual ~CSWEPoliceCarLeft();
protected:
	virtual  HRESULT   GetEventDetInfo(CSWString& strInfo);
	virtual  void      Done(void);
};
REGISTER_CLASS(CSWEPoliceCarLeft)
#endif

