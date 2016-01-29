/**
* @file	tracker.h
* @version	1.0
* @brief Tracker接口
*/

#ifndef _TRACKER_INCLUDED__
#define _TRACKER_INCLUDED__

#include "swobjbase.h"
#include "swimage.h"
#include "trackerdef.h"
#include "platerecogparam.h"
#include "inspector.h"
#include "swimageobj.h"
#include "hvinterface.h"
#include "ItgArea.h"
#include "DspLinkCmd.h"


/**
* Tracker接口定义
*/
class ITracker
{
public:
	virtual ~ITracker() {};
	STDMETHOD(ProcessOneFrame)(
		PROCESS_ONE_FRAME_PARAM* pParam,
		PROCESS_ONE_FRAME_DATA* pProcessData,
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond
	)=0;
	STDMETHOD(ProcessPhoto)(
		HV_COMPONENT_IMAGE imgCurFrame,
		PVOID pvParam,
		PVOID pvData,
		PROCESS_PHOTO_RESPOND* pProcessRespond
		)=0;
	STDMETHOD(SetInspector)(
		IInspector *pInspector
	)=0;
	STDMETHOD(SetCharSet)(
		ChnCharSet nCharSet
	)=0;
	STDMETHOD(SetCalibrateParam)(
		BOOL fEnable ,
		int nRotate = 0,
		int nTilt = 0 ,
		BOOL fOutputImg = FALSE
	)=0;
	STDMETHOD(SetPreSegmentParam)(
		BOOL fEnable
	)=0;
	STDMETHOD(SetLPRMode)(PlateRecogMode nMode) = 0;
	STDMETHOD(ForceResult)(int iFlag = 0) = 0;
	STDMETHOD(GetTrackCount)(PDWORD32 pdwCount)=0;
	STDMETHOD(GetPlatePos)(DWORD32 dwIndex, HiVideo::CRect *prcPlate, HiVideo::CRect *prcRect) = 0;
	STDMETHOD(GetDetectCount)(PDWORD32 pdwCount) = 0;
	STDMETHOD(GetDetectPos)(DWORD32 dwIndex, HiVideo::CRect *prcDetected) = 0;

	STDMETHOD(GetObjCount)(PDWORD32 pdwCount) = 0;
	STDMETHOD(GetObjPos)(DWORD32 dwIndex, HiVideo::CRect *prcObj) = 0;

	STDMETHOD(IsVoted)(DWORD32 dwIndex, bool *pfVoted) = 0;
	STDMETHOD(SetVideoDet)(
		BOOL fEnabled,
		BOOL fSkipFrame = TRUE,
		int nDetLeft = 0,
		int nDetRight = 100
	) = 0;
	STDMETHOD(SetDetArea)(
		const DETECTAREA& cDetAreaInfo
	) = 0;

	// 得到环境亮度
//	STDMETHOD(GetLightType)(LIGHT_TYPE* pLightType) = 0;
	// 设置亮度级别初始值
	STDMETHOD(SetFirstLightType)(LIGHT_TYPE nLightType) = 0;

	STDMETHOD(SetFirstPulseLevel)(int nPulseLevel) = 0;

	STDMETHOD(SetFirstCplStatus)(int nCplStatus) = 0;
	//为HvCore实现的Init函数
	STDMETHOD(InitTracker)(TRACKER_CFG_PARAM* pCfgParam) = 0;
	//为HvCore实现的动态修改参数接口
	STDMETHOD(ModifyTracker)(TRACKER_CFG_PARAM* pCfgParam) = 0;

	// 车辆检测模型的更新接口
	STDMETHOD(UpdateModel)(unsigned int uModelId, unsigned char* pbData, unsigned int cbDataSize) = 0;
	// 动态更新算法参数接口
	STDMETHOD(UpdateAlgParam)(unsigned char* pbData, unsigned int cbDataSize) = 0;

	//测试检测模块的性能，测试的模块类型和测试参数通过全局参数列表设置,
	//全局参数列表通过INI文件加载.
	STDMETHOD(TestPerformance)(
		HV_COMPONENT_IMAGE *phciTest,	//测试数据
		char *pszReport,				//测试报告
		DWORD32 dwReportBufSize,		//测试报告缓冲区长度
		DWORD32 dwMaxTestCount,			//最大测试次数
		DWORD32 dwMaxTestDuration		//最大测试时间(毫秒)
		) = 0;

	//模块测试
	STDMETHOD(ModuleTest)(
		MODULE_ID nModuleID,
		DWORD32 dwFlag,
		HV_COMPONENT_IMAGE *pImg,
		STD_RESULT* pStdResult,
		PVOID pvArgDat = NULL
		) = 0;

	//设置模型加载接口
	STDMETHOD(SetHvModel)(HvCore::IHvModel* pModel) = 0;

	STDMETHOD(SetHvPerformance)(HvCore::IHvPerformance* pHvPerf) = 0;

	STDMETHOD(GetItgArea)(
		CItgArea** ppItgArea,
		DWORD32 dwRefTime
		) = 0;

	// zhaopy
	STDMETHOD(CheckEvent)() = 0;

	//红绿灯相关
	//取当前红绿灯的状态,通过IPT
	STDMETHOD(GetTrafficLightStatus)(
		unsigned char* pbStatus
		)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(TrafficLightStatusEx)(
		UINT iGroupIndex,			//对应哪个灯组的检测结果 
		UINT iRedLightCount,		//红灯数量
		HV_RECT* rgRedLightPos,		//红灯坐标组
		UINT iGreenLightCount,   //绿灯数量
		HV_RECT* rgGreenLightPos	//绿灯坐标组成
		)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(TrafficLightStatus)(
		UINT nLightCount,		//灯组数量
		DWORD32*	rgLightStatus,	//灯组状态
		INT*	pSceneStatus,	//用户根据灯组状态自定义的场景状态
		HV_RECT* rgLightPos,		//灯组坐标位置
		UINT nRedLightCount,   //红灯的数量
		HV_RECT* rgRedLightPos,	//红灯的坐标(用于红灯的加红)
		HV_COMPONENT_IMAGE* pSceneImage//全景图
		)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(TrafficSceneStatus)(
		TRAFFICLIGHT_SCENE tsSceneStatus,	//用户自定义的场景状态
		char* pLightStatus
		)
	{
		return E_NOTIMPL;
	}
};

extern HRESULT CreateTracker_Photo(ITracker** ppTracker);
extern HRESULT CreateTracker_Video(ITracker** ppTracker, int iVideoID = 0);
extern HRESULT DesdroyTracker(ITracker** ppTracker);

#endif // _TRACKER_INCLUDED__
