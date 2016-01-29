#ifndef _PROCESSPLATE_INCLUDED__
#define _PROCESSPLATE_INCLUDED__

#include "swObjBase.h"
#include "swImage.h"
#include "swPlate.h"
#include "Inspector.h"
#include "PlateDetector.h"
#include "Recognition.h"

#include "HvInterface.h"

#define MIN_SEG_IMG_HIGH			9
#define MAX_SEG_IMG_HIGH			100
#define MIN_SEG_IMG_WIDTH			50
#define MAX_SEG_IMG_WIDTH			400

//双层军牌图像的长宽范围
#define MIN_DM_SEG_IMG_HIGH			12
#define MAX_DM_SEG_IMG_HIGH			189
#define MIN_DM_SEG_IMG_WIDTH		50
#define MAX_DM_SEG_IMG_WIDTH		400

#define NO_PREDICT_PLATE_TYPE_MODE				4
#define FORWARD_PREDICT_PROCESS_PLATE_MODE		3
#define BACKOFF_PREDICT_PROCESS_PLATE_MODE		1
#define NO_PREDICT_PROCESS_PLATE_MODE			0

struct CRectifyInfo
{
	float fltHAngle, fltVAngle;
};

struct CBinaryInfo
{
	int iGrayThreshold;			// 二值化阈值
	int iBlackOnWhite;			// 白底黑字
	int iMaybeWhitePlate;		// 有可能是白牌
	CBinaryInfo() 
		: iGrayThreshold(128)
		, iBlackOnWhite(0)
		, iMaybeWhitePlate(-1)		
	{
	}
};

class IProcessPlate
{
public:
	virtual ~IProcessPlate(){};

	STDMETHOD(CalcRectifyInfo)(
		HV_COMPONENT_IMAGE *pImgInput,
		HV_COMPONENT_IMAGE *pImgRectified,
		CRectifyInfo *pRectifyInfo,
		CBinaryInfo *pBinaryInfo,
		PLATE_TYPE *pPlateType,
		PLATE_COLOR *pColor,
		bool fFilt = false,
		BOOL fBigPlate = FALSE
	) = 0;
	
	STDMETHOD(RectifyPlate)(
		HV_COMPONENT_IMAGE *pImgInput,
		CRectifyInfo *pRectifyInfo, 
		HV_COMPONENT_IMAGE *pImgOutput,
		BOOL fConservativeCut
	) = 0;

	STDMETHOD(SegmentPlate)(
		HV_COMPONENT_IMAGE *pImgInput,
		PLATE_COLOR *PlateColor,
		PLATE_TYPE *pPlateType, 
		int *piRefMaxScore, 
		int *piRefCharHeight, 
		int *piRefVScale,
		CBinaryInfo *pBinaryInfo,
		HV_RECT **ppRect,
		int iFastSegMode,
		BOOL fIsNight,
		int iFiveCharWidth
		) = 0;

	STDMETHOD(Initialize)(
		int iWorkMode = NO_PREDICT_PROCESS_PLATE_MODE
	) = 0;

	STDMETHOD(SetWorkMode)(
		int iWorkMode,
		BOOL fFramePlate = FALSE,
		BOOL fPhotoMode = FALSE
	) = 0;

	STDMETHOD(SetInspector)(
		IInspector *pInspector
	)=0;

	STDMETHOD(SetDetector)(
		IPlateDetector *pDetector
	) = 0;
	
	STDMETHOD(SetRecognition)(
		IRecognition *pRecognition
	) = 0;

	STDMETHOD(SetHvParam)(
		HvCore::IHvParam2* pHvParam
	) = 0;

	STDMETHOD(SetHvPerformance)(
		HvCore::IHvPerformance* pHvPerf
	) = 0;
	STDMETHOD(SetSrcImgGray)(
		int iAvgGray, bool fUseFlag
	) = 0;   
};

HRESULT CreateProcessPlateInstance(IProcessPlate **ppProcessPlate);

#endif // _PROCESSPLATE_INCLUDED__

