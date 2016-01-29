#ifndef _SCALESPEED_INCLUDED__
#define _SCALESPEED_INCLUDED__

#include "hvBaseObj.h"
#include "swObjBase.h"
#include "swImage.h"
#include "swImageObj.h"

#include "HvInterface.h"
#include "TrackerCallback.h"

using namespace HiVideo;

class IScaleSpeed
{
public:
	virtual ~IScaleSpeed() {};

	//new initialize func
	STDMETHOD(InitScaleSpeed)(
	)=0;

	STDMETHOD(SetHvParam)(
		HvCore::IHvParam2* pHvParam
	)=0;

	STDMETHOD(SetFrameProperty)(
		const FRAME_PROPERTY& cFrameProperty
	) = 0;

	// 计算车辆速度
	virtual float STDMETHODCALLTYPE CalcCarSpeed(		// 返回车辆速度
		CRect rcBegin,				// 开始位置坐标
		CRect rcEnd,				// 结束位置坐标
		DWORD32 dwDruTime,			// 时间(毫秒)
		bool fAdjustDistance,	// 是否通过车牌宽度矫正距离测量
		PLATE_TYPE PlateType,		//车牌类型
		float &fltScale			// 返回测量距离与实际距离的比例
	) = 0;

	virtual float STDMETHODCALLTYPE CalcCarSpeedNewMethod(		// 返回车辆速度
		CRect rcBegin,				// 开始位置坐标
		CRect rcEnd,				// 结束位置坐标
		DWORD32 dwDruTime,			// 时间(毫秒)
		bool fAdjustDistance,	// 是否通过车牌宽度矫正距离测量
		PLATE_TYPE PlateType,		//车牌类型
		float &fltScale			// 返回测量距离与实际距离的比例
		) = 0;

	//计算屏幕距离
	virtual float STDMETHODCALLTYPE CalcActureDistance(HV_POINT point1, HV_POINT point2) = 0;
};

HRESULT CreateScaleSpeedInstance(IScaleSpeed** ppScaleSpeed, int iVideoID = 0);

#endif // _SCALESPEED_INCLUDED__
