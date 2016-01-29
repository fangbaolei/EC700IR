#ifndef _VIDEODETCALLBACK_INCLUDED__
#define _VIDEODETCALLBACK_INCLUDED__

#include "swObjBase.h"
#include "swPlate.h"
#include "swImage.h"
#include "swImageObj.h"

class IVideoDetCallback
{
public:
	STDMETHOD(CarLeaving)(
		IReferenceComponentImage* pimgLastSnapShot,
		IReferenceComponentImage* pimgLastCaptureShot,
		UINT nID,
		UINT iFrameNo,
		UINT iRefTime,
		LPVOID pvUserData
	)=0;
};

#endif // _VIDEODETCALLBACK_INCLUDED__
