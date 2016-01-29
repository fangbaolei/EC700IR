#ifndef _SCALESPEED_INCLUDED__
#define _SCALESPEED_INCLUDED__

#include "hvBaseObj.h"
#include "swObjBase.h"
#include "swImage.h"
#include "swImageObj.h"

#include "HvInterface.h"
#include "TrackerDef.h"

using namespace HiVideo;

typedef struct _SCALESPEED_CFG_PARAM
{
    int nMethodsFlag;   //计算模式，0:平面标定法, 1:空间标定法
    float fltLeftDistance;  //屏幕下沿到龙门架距离
    float fltFullDistance;  //屏幕上沿到龙门架距离
    float fltCameraHigh;    //摄像机高度
    float fltAdjustCoef;    //修正系数
    float rgfltTransMarix[11];  //计算图像坐标到空间坐标的变换矩阵

	_SCALESPEED_CFG_PARAM()
	{
        nMethodsFlag = 0;
        fltLeftDistance = 25.0f;
        fltFullDistance = 75.0f;
        fltCameraHigh = 7.5f;
        fltAdjustCoef = 1.050f;
        rgfltTransMarix[0] = 0.4970550537f;
        rgfltTransMarix[1] = -241.9865264893f;
        rgfltTransMarix[2] = -30.4937019348f;
        rgfltTransMarix[3] = 0.5036621094f;
        rgfltTransMarix[4] = -21.2472610474f;
        rgfltTransMarix[5] = -6.6555485725f;
        rgfltTransMarix[6] = -231.5502014160f;
        rgfltTransMarix[7] = 0.7746582031f;
        rgfltTransMarix[8] = 0.0300335791f;
        rgfltTransMarix[9] = -0.0085402671f;
        rgfltTransMarix[10] = -0.0195075944f;
	}
}SCALESPEED_CFG_PARAM;

class IScaleSpeed
{
public:
	virtual ~IScaleSpeed() {};

	//new initialize func
	STDMETHOD(InitScaleSpeed)(
		SCALESPEED_CFG_PARAM *pScaleSpeedParam
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
	virtual bool GetActurePoint(HV_POINT ptOrg, HV_POINT& ptDest, float fltDistance) = 0;
};

HRESULT CreateScaleSpeedInstance(IScaleSpeed** ppScaleSpeed, int iVideoID = 0);

#endif // _SCALESPEED_INCLUDED__
