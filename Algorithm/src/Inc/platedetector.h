#ifndef _PLATEDETECTOR_INCLUDED__
#define _PLATEDETECTOR_INCLUDED__

#include "config.h"
#include "swbasetype.h"
#include "swplate.h"
#include "swobjbase.h"
#include "swimage.h"
#include "platerecogparam.h"

#include "hvinterface.h"

#include "ItgArea.h"

// 工作模式, SetWorkMode()函数的nProcMode参数使用
#define PLATE_DET_MODE_DEFAULT 0
#define PLATE_DET_MODE_IR      1

typedef struct _PLATE_DETECT_CFG_PARAM
{
	int nPlateDetect_Green; // 绿牌识别开关
	float fltPlateDetect_StepAdj; // 步长调整系数
	int nPlateDetect_WJMergeNum;
	_PLATE_DETECT_CFG_PARAM()
	{
		nPlateDetect_Green = 0;
		fltPlateDetect_StepAdj = 1.0f;
		nPlateDetect_WJMergeNum = 20;
	}
} PLATE_DETECT_CFG_PARAM;

typedef enum {
	ALL_DETECTOR_PHOTO = -2,	// 图片版用
	ALL_DETECTOR = -1,		// 使用所有检测器
	PLATE_BLUE_DETECTOR = 0,	// 使用单层黑底白字车牌检测器
	PLATE_YELLOW_DETECTOR,		// 使用单层白底黑字车牌检测器
	PLATE_YELLOW2_DETECTOR,		// 使用双层白底黑字车牌检测器
	WJ_DETECTOR,				// 使用WJ检测器
	PLATE_DBGREEN_DETECTOR,
    CAR_DETECTOR,
	FACE_DETECTOR,				// 人脸检测器
	DETECTOR_COUNT
} DETECTOR_TYPE;

struct RECTA : public HV_RECT
{
	PLATE_COLOR color;
	PLATE_TYPE nPlateType;
	DWORD32 nVariance;
	int nConf;			// 可信度,越大越好
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

        	/*
	函数说明:该函数用来从给定的图象中检测车牌,检测车牌的结果放置在ppROIs中.
	函数返回值:
		返回S_OK表示检测成功执行.
		返回E_POINTER表示pComponetImage为NULL.
		返回S_FALSE表示检测失败. 
		
        用法：
             nStep = 1 时只做一级检测,ppROIsIn可为空; 
             nStep = 2 时做二级检测,ppROIsIn为一级检测数据 
		*/
    STDMETHOD(ScanPlatesComponentImagePhotoStep)(
        int nStep,                                      // 检测步骤 
        HV_COMPONENT_IMAGE 	*pComponetImage,			//指向需要检测的大图.
        MULTI_ROI 			*ppROIsIn,					//指向车牌传入的队列.
        MULTI_ROI 			*ppROIs,					//指向车牌结果队列.
        int				nDetMinScaleNum,				// 车牌检测框的最小宽度
        int				nDetMaxScaleNum	,			// 车牌检测框的最大宽度
        BOOL 				fFastMode = FALSE,			//是否使用快速扫描.
        int 				nRefPlateWidth = 0,			//参考车牌宽度.
        BOOL 				fDownSample = FALSE,		//Downsampling or not (currently ignored)
        int					*piAvgY	= NULL,				//当前图像的平均亮度
        CItgArea*		pItgArea = NULL
        ) = 0;

	STDMETHOD(Initialize)(
		PlateRecogParam*	pParam,
		PLATE_DETECT_CFG_PARAM *pDetectParam,
		int 				iMaxWidth,
		int 				iMaxHeight
	) = 0;

    STDMETHOD(SetWorkMode)(
        int iProcMode = PLATE_DET_MODE_DEFAULT
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

	STDMETHOD(GetVersion)(
		char* szBuf, int szBufLen
	) = 0;
};

HRESULT CreatePlateDetectorInstance(IPlateDetector** ppDetector);
HRESULT InitializeDetectorModelInfo( void );

#endif // _PLATEDETECTOR_INCLUDED__
