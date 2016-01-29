#ifndef _TRACKER_INCLUDED__
#define _TRACKER_INCLUDED__

#include "swObjBase.h"
#include "swImage.h"
#include "TrackerCallback.h"
#include "PlateRecogParam.h"
#include "Inspector.h"
#include "swImageObj.h"
#include "CharQuotietyMatrix.h"

#include "HvInterface.h"

#include "ItgArea.h"

class ITracker
{
public:
	virtual ~ITracker() {};
	STDMETHOD(ProcessOneFrame)(
		int iVideoID,
		IReferenceComponentImage *pImage,
		IReferenceComponentImage *pCapImage
	)=0;
	STDMETHOD(ProcessPhoto)(
		int iCamID,
		IReferenceComponentImage *pImage,
		IReferenceComponentImage *pCapImage = NULL,
		PVOID pvParam = NULL
	)=0;
	STDMETHOD(SetCallBack)(
		ITrackerCallback *pCallback
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
	STDMETHOD(GetPlatePos)(DWORD32 dwIndex, HiVideo::CRect *prcPlate) = 0;
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
	STDMETHOD(SetCharQuotietyMatrix)(				// 设置字符权值矩阵
		ICharQuotietyMatrix *pCharQuotietyMatrix
	) = 0;
	STDMETHOD(SetDetArea)(
		const DETECTAREA& cDetAreaInfo
	) = 0;

	// 得到环境亮度
	STDMETHOD(GetLightType)(LIGHT_TYPE* pLightType) = 0;
	// 设置亮度级别初始值
	STDMETHOD(SetFirstLightType)(LIGHT_TYPE nLightType) = 0;

	//为HvCore实现的Init函数
	STDMETHOD(InitTracker)() = 0;

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

	//设置参数管理接口
	STDMETHOD(SetHvParam)(HvCore::IHvParam2* pHvParam) = 0;

	//设置模型加载接口
	STDMETHOD(SetHvModel)(HvCore::IHvModel* pModel) = 0;

	STDMETHOD(SetHvPerformance)(HvCore::IHvPerformance* pHvPerf) = 0;

	STDMETHOD(GetItgArea)(
		CItgArea** ppItgArea,
		DWORD32 dwRefTime
		) = 0;
};

extern HRESULT CreateTracker_Photo(ITracker** ppTracker);
extern HRESULT CreateTracker(ITracker** ppTracker, int iVideoID = 0);

#endif // _TRACKER_INCLUDED__
