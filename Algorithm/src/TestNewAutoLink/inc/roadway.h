#ifndef _ROADWAY_INCLUDED__
#define _ROADWAY_INCLUDED__

#include "swbasetype.h"
#include "HvInterface.h"
#include "trackercallback.h"

#define MAX_ROADWAY_NUM		4					// 最大车道数量

// 统计车辆信息
typedef struct _CAR_STAT_INFO
{
	BYTE8 nRoadway;					// 车道编号
	DWORD32 dwLeftTime;				// 车辆离开时间
	DWORD32 dwSpeed;				// 车辆速度
} CAR_STAT_INFO;

// 车道分割线
typedef struct TRACK_SEP_LINE_TAG
{
	HV_POINT poBegin;			// 分割线开始
	HV_POINT poEnd;				// 分割线结束
} TRACK_SEP_LINE;

// 车道接口
class IRoadway
{
public:
	virtual ~IRoadway() {};
/*
	STDMETHOD(ProcessOneFrame)(
		int iVideoID,
		IReferenceComponentImage *pImage,
		IReferenceComponentImage *pCapImage
	)=0;
	STDMETHOD(ProcessPhoto)(
		IReferenceComponentImage *pImage,
		PVOID pvParam
	)=0;
*/
	STDMETHOD(SetHvParam)(
		HvCore::IHvParam2* pHvParam
		) = 0;

	// 设置车道
	STDMETHOD(SetRoadway)(
		int nRoadwayNum,				// 车道数量
		TRACK_SEP_LINE *pLine,			// 车道分割线
		float fltRoadWidth				// 路面宽度
	)=0;

	STDMETHOD(SetFrameProperty)(
		const FRAME_PROPERTY& cFrameProperty
		) = 0;

	// 设置摄像机参数
	STDMETHOD(SetCameraParam)(
		float fltCameraHigh,				// 摄像机高度
		float fltFarDistance,				// 摄像机垂直点到摄像机视角远端的直线距离
		float fltNearDistance				// 摄像机垂直点到摄像机视角近端的直线距离
		) = 0;

	// 计算实际坐标
	virtual HV_POINT CalcAccCoordinate(
		HV_POINT &ptSrc							// 原始坐标
		) = 0;

	// 计算两点直线距离
	virtual float CalcDistance(
		HV_POINT ptBegin,				// 开始位置坐标
		HV_POINT ptEnd					// 结束位置坐标
		) = 0;

	// 计算中间车道得宽度(象素)
	virtual int GetRoadwayWidth(int nPosY) = 0;

	// 计算物体所属车道
	virtual int GetRoadwayNum(HV_RECT rcObj) = 0;


	// 计算物体宽度类型
	virtual int GetObjWidthType(HV_RECT rcObj) = 0;		// 0:超小 1:小车 2:大车 3:超大
	// 计算物体高度度类型
	virtual int GetObjHeightType(HV_RECT rcObj) = 0;	// 0:超小 1:小车 2:大车 3:超大

	// 取得车道统计信息数据
	STDMETHOD(GetTrackStatInfo)(int *pnTrackNum, TrackStatInfo *pStatData) = 0;

	// 更新车辆离开信息(统计用)
	STDMETHOD(UpdateCarLeft)(CAR_STAT_INFO *pLeftCar) = 0;

};

extern HRESULT CreateRoadwayInstance(IRoadway** ppRoadway);

#endif // _ROADWAY_INCLUDED__
