#ifndef _PROCESSPLATE_INCLUDED__
#define _PROCESSPLATE_INCLUDED__

#include "swobjbase.h"
#include "swimage.h"
#include "hvinterface.h"
#include "swplate.h"
#include "inspector.h"
#include "platedetector.h"
#include "recognition.h"

#define NO_PREDICT_PLATE_TYPE_MODE				4
#define FORWARD_PREDICT_PROCESS_PLATE_MODE		3
#define BACKOFF_PREDICT_PROCESS_PLATE_MODE		1
#define NO_PREDICT_PROCESS_PLATE_MODE			0

// 工作模式, SetWorkMode()函数的nProcMode参数使用
#define PROC_PLATE_MODE_DEFAULT 0
#define PROC_PLATE_MODE_IR      1

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

typedef struct _PROCESS_PLATE_CFG_PARAM
{
	BOOL fBlackPlateThreshold_Enable; //黑牌判定规则使能 (黑牌可变蓝牌)
	int nBlackPlate_S; // 黑牌的饱和度上限
	int nBlackPlate_L; // 黑牌亮度上限
	int nBlackPlateThreshold_H0; // 蓝牌色度下限
	int nBlackPlateThreshold_H1; // 蓝牌色度上限
	int nProcessPlate_LightBlue; // 浅蓝牌开关
	BOOL fEnableDoublePlate; // 双层牌开关
	int iSegWhitePlate; //强制白牌分割
	BOOL fEnableShiGuanPlate;   // 使用使馆牌检测
	
	//最小宽高比系数。分割算法以默认最小宽高比乘以该系数作为最小宽高比计算。
	//默认1.0，使用默认宽高比。减小时能够支持更小的字符宽高比（如大角度、倾斜等）
	float fltMinWHRatioCoef;
	_PROCESS_PLATE_CFG_PARAM()
	{
		fBlackPlateThreshold_Enable = 0;
		nBlackPlate_S = 10;
		nBlackPlate_L = 85;
		nProcessPlate_LightBlue = 1;
		fEnableDoublePlate = 1;
		nBlackPlateThreshold_H0 = 100;
		nBlackPlateThreshold_H1 = 200;
		iSegWhitePlate = 0;
		fEnableShiGuanPlate = FALSE;
		fltMinWHRatioCoef = 1.f;
	}
} PROCESS_PLATE_CFG_PARAM;

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
	STDMETHOD(CalRectifyInfoKL)(
		HV_COMPONENT_IMAGE *pImgInput, 
		CRectifyInfo *pRectifyInfo,
		PLATE_TYPE *pPlateType,
		PLATE_COLOR *pColor
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
		int iWorkMode,
		PROCESS_PLATE_CFG_PARAM* pCfgParam
	) = 0;

	STDMETHOD(SetWorkMode)(
		int iWorkMode,
		BOOL fFramePlate = FALSE,
		BOOL fPhotoMode = FALSE,
		int iProcMode = PROC_PLATE_MODE_DEFAULT
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
	STDMETHOD(GetVersion)(
		char* szBuf, int szBufLen
	) = 0;
};

HRESULT CreateProcessPlateInstance(IProcessPlate **ppProcessPlate);

#endif // _PROCESSPLATE_INCLUDED__

