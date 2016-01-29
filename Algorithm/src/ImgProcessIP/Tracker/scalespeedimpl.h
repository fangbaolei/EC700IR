#pragma once

#ifndef _SCALESPEEDIMPL_INCLUDED__
#define _SCALESPEEDIMPL_INCLUDED__

#include "ScaleSpeed.h"

class CScaleSpeedImpl : public IScaleSpeed
{
public:
	CScaleSpeedImpl();
	virtual ~CScaleSpeedImpl();

	STDMETHOD(InitScaleSpeed)(
		SCALESPEED_CFG_PARAM *pScaleSpeedParam
		);

	STDMETHOD(SetHvParam)(
		HvCore::IHvParam2* pHvParam
		);

	STDMETHOD(SetFrameProperty)(
		const FRAME_PROPERTY& cFrameProperty
		);

	virtual float STDMETHODCALLTYPE CalcCarSpeed(		// 返回车辆速度
		CRect rcBegin,				// 开始位置坐标
		CRect rcEnd,				// 结束位置坐标
		DWORD32 dwDruTime,			// 时间(毫秒)
		bool fAdjustDistance,	// 是否通过车牌宽度矫正距离测量
		PLATE_TYPE PlateType,		//车牌类型
		float &fltScale			// 返回测量距离与实际距离的比例
	);

	// 新方法
	virtual float STDMETHODCALLTYPE CalcCarSpeedNewMethod(	// 返回车辆速度
		CRect rcBegin,				// 开始位置坐标
		CRect rcEnd,				// 结束位置坐标
		DWORD32 dwDruTime,			// 时间(毫秒)
		bool fAdjustDistance,	// 是否通过车牌宽度矫正距离测量
		PLATE_TYPE PlateType,		//车牌类型
		float &fltScale			// 返回测量距离与实际距离的比例
		);

	//计算屏幕距离
	virtual float STDMETHODCALLTYPE CalcActureDistance(HV_POINT point1, HV_POINT point2);
	HRESULT SetVideoID(int iVideoID);
	bool GetActurePoint(HV_POINT ptOrg, HV_POINT& ptDest, float fltDistance);
private:
	HvCore::IHvParam2* m_pHvParam;

	BOOL m_fInitialized;
	float *m_prgfltPixelsDisTable;			// 屏幕象素距离对照表


	// 测速使用的参数
	float m_fltCameraHigh;			// 摄像机高度	= 7.50f;
	float m_fltDistanceAll;			// 总距离 = 75.00f;
	float m_fltDistanceLeft;		// 不在摄像机范围内的距离 = 25.00f;
	float m_fltAdjustCoef;			// 速度调整系数
	float m_fltTransMatrix[11];		// 计算图像坐标到空间坐标的变换矩阵

	// 测试方法选择标志
	int m_nFlagMethods;			// 选择计算坐标变换的方式，默认为0。0:旧方法，1:新方法：用坐标变换矩阵计算

	// 方法
	void Clear();				// 释放申请的空间

	// 使用的一些中间参数
	float m_fltX0;
	float m_fltY0;

	//zhaopy
	DWORD32 m_dwHeight;
	// 中间角度
	float m_anBeta;					// 角度 atan(m_fltDistanceLeft / m_fltCameraHigh)
	float m_anACF;

	// 中间距离
	float m_disCG;
	float m_disAC;

	void CalcActurePoint(HV_POINT poScreen, float &fltCD, float &fltW);		// 根据屏幕坐标计算实际坐标
	int m_iVideoID;
};

#endif // _SCALESPEEDIMPL_INCLUDED__
