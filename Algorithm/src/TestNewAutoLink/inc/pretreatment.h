#ifndef _PRETREATMENT_INCLUDED__
#define _PRETREATMENT_INCLUDED__

#include "HvInterface.h"
#include "TrackerCallback.h"

class IPretreatment
{
public:
	virtual ~IPretreatment() {};

	STDMETHOD(Initialize)() = 0;

	STDMETHOD(SetHvParam)(HvCore::IHvParam2* pHvParam) = 0;

	STDMETHOD(PreCalibrate)(
		HV_COMPONENT_IMAGE Img,    //原始图像
		HV_COMPONENT_IMAGE *pImgCalibrated  //校正后的图像
	) = 0;

	STDMETHOD(SetParam)(
		INT nHRotate=0,				//水平旋转角度
		INT nVTilt=0				//垂直倾斜角度
	) = 0;

};

HRESULT CreatePretreatmentInstance(IPretreatment **ppPretreatment );


#endif // _PRETREATMENT_INCLUDED__

