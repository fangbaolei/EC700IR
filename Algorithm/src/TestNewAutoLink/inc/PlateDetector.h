#ifndef _PLATEDETECTOR_INCLUDED__
#define _PLATEDETECTOR_INCLUDED__

#include "config.h"
#include "swBaseType.h"
#include "swPlate.h"
#include "swObjBase.h"
#include "swImage.h"
#include "PlateRecogParam.h"

#include "HvInterface.h"

#include "ItgArea.h"

typedef enum {
	ALL_DETECTOR_PHOTO = -2,	// 图片版用
	ALL_DETECTOR = -1,		// 使用所有检测器
	PLATE_BLUE_DETECTOR = 0,	// 使用单层黑底白字车牌检测器
	PLATE_YELLOW_DETECTOR,		// 使用单层白底黑字车牌检测器
	PLATE_YELLOW2_DETECTOR,		// 使用双层白底黑字车牌检测器
	WJ_DETECTOR,				// 使用WJ检测器
	PLATE_DBGREEN_DETECTOR,
	DETECTOR_COUNT
} DETECTOR_TYPE;

struct RECTA : public HV_RECT
{
	PLATE_COLOR color;
	PLATE_TYPE nPlateType;
	DWORD32 nVariance;
};

typedef struct _MULTI_ROI {
    int dwNumROI;   // number of Plates found; 
    RECTA  *rcROI;
} MULTI_ROI;

class IPlateDetector
{
public:
	virtual ~IPlateDetector(){};
	IPlateDetector(){};

	STDMETHOD(ScanPlatesComponentImage)(
		DETECTOR_TYPE nDetectorType,
		HV_COMPONENT_IMAGE *pComponetImage,
		MULTI_ROI *ppROIs,
		int	nDetMinScaleNum,				// 车牌检测框的最小宽度
		int	nDetMaxScaleNum	,			// 车牌检测框的最大宽度
		BOOL fFastMode = FALSE,			//是否使用快速扫描.
		int nRefPlateWidth = 0,
		BOOL fDownSample = FALSE,		// Downsampling or not (currently ignored)
		int	*piAvgY	= NULL,				//当前图像的平均亮度
		CItgArea* pItgArea = NULL
	) = 0;

	STDMETHOD(Initialize)(
		PlateRecogParam*	pParam,
		int 				iMaxWidth,
		int 				iMaxHeight		
	) = 0;

	STDMETHOD(SetRecParam)(
		PlateRecogParam*	pParam
	) = 0;

	STDMETHOD(SetImgSize)(
		int 				iMaxWidth,
		int 				iMaxHeight		
	) = 0;

	STDMETHOD(SetHvParam)(
		HvCore::IHvParam2* pHvParam
	) = 0;

	STDMETHOD(SetHvPerformance)(
		HvCore::IHvPerformance* pHvPerf
	) = 0;

	STDMETHOD(SetHvModel)(
		HvCore::IHvModel* pHvModel
	) = 0;
};

HRESULT CreatePlateDetectorInstance(IPlateDetector** ppDetector);
HRESULT InitializeDetectorModelInfo( void );

#endif // _PLATEDETECTOR_INCLUDED__
