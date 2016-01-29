#ifndef _TRACKERIMPL_INCLUDED__
#define _TRACKERIMPL_INCLUDED__

#include "tracker.h"
#include "platedetector.h"
#include "processplate.h"
#include "recognition.h"
#include "TrackInfoHigh.h"

using namespace HiVideo;

class CTrackerImpl_Photo : public ITracker
{
public:
	CTrackerImpl_Photo();

	// ITracker
	virtual ~CTrackerImpl_Photo();
	void Clear();

	STDMETHOD(ProcessOneFrame)(
		PROCESS_ONE_FRAME_PARAM* pParam,
		PROCESS_ONE_FRAME_DATA* pProcessData,
		PROCESS_ONE_FRAME_RESPOND* pProcessRespond
		)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(ProcessPhoto)(
		HV_COMPONENT_IMAGE imgCurFrame,
		PVOID pvParam,
		PVOID pvData,
		PROCESS_PHOTO_RESPOND* pProcessRespond
		);

	STDMETHOD(SetInspector)(
		IInspector *pInspector
		);

	STDMETHOD(SetHvParam)(
		HvCore::IHvParam2* pHvParam
		);

	STDMETHOD(SetHvModel)(
		HvCore::IHvModel* pModel
		);

	STDMETHOD(SetFirstPulseLevel)(
		int nPulseLevel)
	{
		return S_OK;
	}
	STDMETHOD(SetFirstCplStatus)(
		int nCplStatus)
	{
		return S_OK;
	}
	//图片版无须实现
	STDMETHOD(SetHvPerformance)(
		HvCore::IHvPerformance* pHvPerf
		);

	STDMETHOD(SetCharSet)(
		ChnCharSet nCharSet
		)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(SetCalibrateParam)(
		BOOL fEnable,
		int nRotate=0,
		int nTilt=0,
		BOOL fOutputImg = FALSE
		)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(SetPreSegmentParam)(
		BOOL fEnable
		)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(SetLPRMode)(PlateRecogMode nMode)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(SetVideoDet)(BOOL fEnabled, BOOL fSkipFrame = TRUE, int nDetLeft = 0, int nDetRight = 100)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(SetScaleSpeed)(
		BOOL fEnabled,
		float fltFullWidth,
		float fltCameraHigh,
		float fltLeftWidth,
		float fltAdjustCoef
		)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(SetDetArea)( const DETECTAREA& cDetAreaInfo )
	{
		return E_NOTIMPL;
	}
	STDMETHOD(ForceResult)(int i)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(GetTrackCount)(PDWORD32 pdwCount)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(GetPlatePos)(DWORD32 dwIndex, HiVideo::CRect *prcPlate, HiVideo::CRect *prcRect)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(ClearTrackInfo)()
	{
		return E_NOTIMPL;
	}
	STDMETHOD(IsVoted)(DWORD32 dwIndex, bool *pfVoted)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetDetectCount)(PDWORD32 pdwCount) {return S_OK;}
	STDMETHOD(GetDetectPos)(DWORD32 dwIndex, HiVideo::CRect *prcDetected) {return S_OK;}
	STDMETHOD(GetObjCount)(PDWORD32 pdwCount)  {return S_OK;}
	STDMETHOD(GetObjPos)(DWORD32 dwIndex, HiVideo::CRect *prcObj)  {return S_OK;}

	//为HVCORE实现的初始化函数
	STDMETHOD(InitTracker)(TRACKER_CFG_PARAM* pCfgParam);
	STDMETHOD(ModifyTracker)(TRACKER_CFG_PARAM* pCfgParam) {return S_OK;}

	// 车辆检测模型的更新接口
	STDMETHOD(UpdateModel)(unsigned int uModelId, unsigned char* pbData, unsigned int cbDataSize) {return S_OK;}
	// 动态更新算法参数接口
	STDMETHOD(UpdateAlgParam)(unsigned char* pbData, unsigned int cbDataSize) {return S_OK;}

	//测试检测模块的性能，测试的模块类型和测试参数通过全局参数列表设置,
	//全局参数列表通过INI文件加载.
	STDMETHOD(TestPerformance)(
		HV_COMPONENT_IMAGE *phciTest,	//测试数据
		char *pszReport,				//测试报告
		DWORD32 dwReportBufSize,		//测试报告缓冲区长度
		DWORD32 dwMaxTestCount,			//最大测试次数
		DWORD32 dwMaxTestDuration		//最大测试时间(毫秒)
		)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(ModuleTest)(
		MODULE_ID nModuleID,
		DWORD32 dwFlag,
		HV_COMPONENT_IMAGE *pImg,
		STD_RESULT* pStdResult,
		PVOID pvArgDat = NULL
		)
	{
		return E_NOTIMPL;
	}

/*
	// 得到环境亮度
	STDMETHOD(GetLightType)(LIGHT_TYPE* pLightType)
	{
		return E_NOTIMPL;
	}*/

	// 设置亮度级别初始值
	STDMETHOD(SetFirstLightType)(LIGHT_TYPE nLightType)
	{
		return E_NOTIMPL;
	}
	

	STDMETHOD(GetItgArea)(
		CItgArea** ppItgArea,
		DWORD32 dwRefTime
		)
	{
		*ppItgArea = NULL;
		return S_OK;
	}

		// zhaopy
	STDMETHOD(CheckEvent)() { return E_NOTIMPL; }

public:
	//创建功能组件
	HRESULT OnCreate();

protected:
	BOOL m_fInitialized;
	INT m_nWidth, m_nHeight, m_nStride;

	PlateRecogParam *m_pParam;

	IInspector *m_pInspector;
	IPlateDetector *m_pPlateDetector;
	IProcessPlate *m_pProcessPlate;
	IRecognition *m_pRecognition;

	HvCore::IHvModel* m_pHvModel;
	HvCore::IHvPerformance* m_pHvPerf;

	static const int s_knMaxPlateCandidate = 200;
	RECTA m_rgTempPlatePos[s_knMaxPlateCandidate];

	int m_iRoadNumber;	//车道数量
	ROAD_INFO m_roadInfo[CTrackInfo::s_iMaxRoadNumber];

	//截图区域限制
	int m_nMaxCropWidth;
	int m_nMaxCropHeight;

	//初始化时进行清理
	HRESULT OnInit();

	HRESULT SetFrameProperty(
		const FRAME_PROPERTY& FrameProperty
		);

	HRESULT GetInflateRect(
		CRect &rect,		// in, out
		PLATE_COLOR color,
		PLATE_TYPE &type,
		INT maxRectHeight	// 判断是否用inflate rate2
		);

	//扩边截图
	HRESULT CropPlateImage(
		const HV_COMPONENT_IMAGE& imgPhoto,
		CRect& rcPlate,
		PLATE_TYPE nPlateType,
		HV_COMPONENT_IMAGE* rgPlateImage,
		int nCount	//指定截取图的结果数
		);

	//预处理
	HRESULT PreTreatment(
		HV_COMPONENT_IMAGE& imgPlate,
		PLATE_TYPE nPlateType,
		PLATE_COLOR nPlateColor,
		BOOL fEnhance,
		CPersistentComponentImage* rgImgRectified,
		int nCount,	 //指定预处理生成的图数量,通过改变垂直校正角度
		CBinaryInfo &BinInfo
		);

	HRESULT RecognizeChar(
		HV_COMPONENT_IMAGE imgPlate,
		HV_RECT rcChar,
		RECOGNITION_TYPE RecogType,
		PlateInfo	&cPlateInfo,
		PBYTE8 pbResult,
		PSDWORD32 pdwProb,
		int iCharPos
		);

	HRESULT RecognizePlate(
		HV_COMPONENT_IMAGE imgPlate,
		PlateInfo &Info,
		CBinaryInfo &BinaryInfo,
		int nFiveCharWidth
		);

	HRESULT RecognizeColorArea(
		const HV_COMPONENT_IMAGE imgInput, 
		//IReferenceComponentImage* pTempImg,
		HV_RECT rcCrop, 
		BYTE8 *cTempAdd, 
		BYTE8 *piResult,
		int iCutLeng,
		LIGHT_TYPE m_PlateLightType
		);

	HRESULT RecognizeCarColourPlus(
		HV_COMPONENT_IMAGE imgPlateOri,
		PlateInfo &Info,
		int iAvgGray,
		BOOL fIsNight = FALSE
		);

// 	HRESULT RecognizeCarColour(
// 		HV_COMPONENT_IMAGE imgPlate,
// 		PlateInfo &Info,
// 		BOOL fIsNight = FALSE
// 		);

	HRESULT CalcCarSize(
		HV_COMPONENT_IMAGE *pImgCurFrame,
		OUTPUT_CAR_SIZE_INFO *pCarSize,
		CAR_TYPE nCarType
		);

	HRESULT Vote(
		PlateInfo** rgInfo,
		int nCount,
		PlateInfo* pResult,
		CParamQuanNum* pfltConf
		);

	void RoadInfoParamInit();
	void InitRoadInfoParam();

	int LeftOrRight(int iX, int iY, int iLineNum, int* piDistance = NULL);
	int MatchRoad(int iX, int iY);

private:
	//可配置参数
	PlateRecogParam m_PlateRecogParam;

	// 是否输出调试信息
	int m_fOutputDebugInfo;

	// 是否判断广西警标志
	int m_iRecogGxPolice;

	// 本地化汉字，38~68, 58为“桂”
	int m_nDefaultChnId;

	//新WJ处理开关
	BOOL m_fEnableNewWJSegment;
	//新WJ字符替换
	BOOL  m_fEnableDefaultWJ;
	char  m_strDefaultWJChar[3];

	//允许校正开关
	BOOL m_fEnableRectify;

	//车身颜色识别开关
	BOOL m_fEnableRecgCarColor;

	//优先出牌方位
	int m_nFirstPlatePos;

	//摩托车检测阈值
	int m_iMiddleWidth;
	int m_iBottomWidth;

	BOOL m_fOutputCarSize;
	//车牌颜色识别控制
	int m_nEnableColorReRecog;  //小车是否取车牌两边区域重识别结果
	int m_nWGColorReThr;        //灰白重识别——偏向于识别黑
	int m_nWColorReThr;        //白重识别——灰白调整，偏向于识别灰
	int m_nBColorReThr;        //黑重识别——灰黑调整，偏向于识别灰
	bool m_fEnableCorlorThr;  //环境亮度与车身亮度比较开关
	int m_nBGColorThr;        //车身亮度比环境亮度大BGColorThr的黑色车，设置成灰色车
	int m_nWGColorThr;        //晚上车身亮度比环境亮度大WGColorThr的灰色车，设置成白色车

	TRACKER_CFG_PARAM m_cfgParam;
};

#endif // _TRACKERIMPL_INCLUDED__
