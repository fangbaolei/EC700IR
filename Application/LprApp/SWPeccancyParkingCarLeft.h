#ifndef __SW_PECCANCY_PARKING_CARLEFT_H__
#define __SW_PECCANCY_PARKING_CARLEFT_H__
#include "SWCarLeft.h"
//#include "swpa_datetime.h"

class CSWPeccancyParkingCarLeft : public CSWCarLeft
{
	CLASSINFO(CSWPeccancyParkingCarLeft, CSWCarLeft)
public:
	CSWPeccancyParkingCarLeft();
	virtual ~CSWPeccancyParkingCarLeft();

	virtual HRESULT SetDetectCarTime(SWPA_DATETIME_TM &cTime);
	virtual HRESULT SetPeccancyTime(SWPA_DATETIME_TM &cTime);
	virtual HRESULT SetPresetName(LPCSTR szName);
	virtual VOID SetVideo(INT index, CSWImage *pVideo);
	virtual CSWImage *GetVideo(INT index);
	virtual INT GetVideoCount();
protected:
	virtual  HRESULT   GetEventDetInfo(CSWString& strInfo);
	virtual  HRESULT   BuildPlateString(TiXmlDocument& xmlDoc);


//	virtual  void      Done(void);

private:
	SWPA_DATETIME_TM m_cDetectCarTime;
	SWPA_DATETIME_TM m_cPeccancyTime;
	CSWString m_strPresetName;
	CSWImage *m_rgpVideo[30];
	INT m_iVideoCount;
};
REGISTER_CLASS(CSWPeccancyParkingCarLeft)
#endif

