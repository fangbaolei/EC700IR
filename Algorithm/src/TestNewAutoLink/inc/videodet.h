#ifndef _VIDEODET_INCLUDED__
#define _VIDEODET_INCLUDED__

#include "hvBaseObj.h"
#include "swObjBase.h"
#include "swImage.h"
#include "VideoDetCallback.h"
#include "swImageObj.h"

#include "HvInterface.h"
#include "TrackerCallback.h"

using namespace HiVideo;

class IVideoDet
{
public:
	virtual ~IVideoDet() {};
	STDMETHOD(ProcessOneFrame)(
		IReferenceComponentImage *pImage,
		IReferenceComponentImage *pCaptureImage,
		bool fDayNigh
	)=0;
	STDMETHOD(SetCallBack)(
		IVideoDetCallback *pCallback
	)=0;
	STDMETHOD(SetParam)(
		BOOL fSkipFrame,
		int nVideoDetLeft = 0,
		int nVideoDetRight = 100
	)=0;
	STDMETHOD(SetHvParam)(
		HvCore::IHvParam2* pHvParam
	)=0;
	//new initialize func
	STDMETHOD(InitVideoDet)(
		HV_RECT rcDetArea
	)=0;

	// 更新车辆离开信息
	STDMETHOD(UpdateCarLeft)(
		HiVideo::CRect rcPos,				// 车牌位置
		PLATE_COLOR color,					// 车牌颜色
		bool fNoPlate						// 是否无牌车
		)=0;

	// 更新车辆离开信息
	virtual bool IsNewCarMoving(
		)=0;

	// 是否有相近的车牌
	virtual bool IsCloseToPlate(
		HiVideo::CRect rcPos				// 车牌位置
		)=0;

	// 取得车辆离开计数值
	virtual int GetCarLeftCount(
		)=0;

	virtual void FireCarLeavingEvent(
		)=0;

};

HRESULT CreateVideoDetInstance(IVideoDet** ppVideoDet, int nID);

#endif // _VIDEODET_INCLUDED__
