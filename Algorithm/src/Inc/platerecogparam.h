// 该文件编码必须是WINDOWS-936格式

#ifndef _PARAMETER_INCLUDED__
#define _PARAMETER_INCLUDED__

#include "swbasetype.h"
#include "swimage.h"
#include "swimageobj.h"

#define CHN_CHAR_SET_LIST(MACRO_NAME)			\
	MACRO_NAME(FULLNN, "全汉字模型")

#define DEFINE_CCS_ENUM(a, b) CCS_##a,

enum ChnCharSet
{
	CHN_CHAR_SET_LIST(DEFINE_CCS_ENUM)
	CCS_MaxSet
};

enum CarOrientation
{
	CO_HEAD=0,
	CO_TAIL,
	CO_COUNT
};

#define RECOG_MODE_LIST(MACRO_NAME)									\
	MACRO_NAME(TOLLGATE, "收费站")									\
	MACRO_NAME(HIGHWAY_HEAD, "卡口 - 车头")							\
	MACRO_NAME(HIGHWAY_TAIL, "卡口 - 车尾")							\
	MACRO_NAME(PATROL_LEFT, "移动稽查 - 左侧")						\
	MACRO_NAME(PATROL_RIGHT, "移动稽查 - 右侧")						\
	MACRO_NAME(PORTABLE, "随意拍")									\
	MACRO_NAME(ELECTRONIC_POLICE, "电子警察")						\
	MACRO_NAME(TRAFFICE_EVENT_HEAD, "交通事件检测-车头")			\
	MACRO_NAME(TRAFFICE_EVENT_TAIL, "交通事件检测-车尾")			\
	MACRO_NAME(HVC,	"抓拍识别")					                    \
	MACRO_NAME(PARK, "停车场双路")				                    \
	MACRO_NAME(PARK_SINGLE, "停车场单路")				            \
	MACRO_NAME(CAP_FACE, "卡口人脸抓拍")							\
    MACRO_NAME(COVER_CTRL, "断面控制器")						    \

#define DEFINE_PRM_ENUM(a, b) PRM_##a,

enum PlateRecogMode
{
	RECOG_MODE_LIST(DEFINE_PRM_ENUM)
	PRM_MaxMode
};

// 车牌识别的要求,快速和一般
enum PlateRecogSpeed
{
	PR_SPEED_NORMAL = 0,	// 卡口场合
	PR_SPEED_FAST			// 路段移动场合
};

// 车辆运动方向
enum MovingDirection
{
	MD_TOP2BOTTOM = 0,		// 由远及近
	MD_BOTTOM2TOP,			// 由近及远
	MD_LEFT2RIGHT,			// 由左及右
	MD_RIGHT2LEFT,			// 由右及左
	MD_NOTSURE,			    // 任意方向
	MD_COUNT
};

template<BYTE8 QuanBit>
class CQuanNum
{
	static const DWORD32 QuanScale = (1 << QuanBit);

	SDWORD32 m_dwQuan;

	//friend CQuanNum<QuanBit> operator*(int i, const CQuanNum<QuanBit>& a);

public:
	CQuanNum()
	{
		m_dwQuan = 0;
	}
	CQuanNum(const CQuanNum& a)
	{
		m_dwQuan = a.m_dwQuan;
	}
	CQuanNum(float f)
	{
		m_dwQuan = (SDWORD32)(f * QuanScale+0.5);
	}
	CQuanNum& operator=(const CQuanNum& a)
	{
		m_dwQuan = a.m_dwQuan;
		return *this;
	}
	CQuanNum& operator=(float f)
	{
		m_dwQuan = (SDWORD32)(f * QuanScale+0.5);
		return *this;
	}
	CQuanNum operator*(int i) const
	{
		CQuanNum q;
		q.m_dwQuan = m_dwQuan * i;
		return q;
	}
	CQuanNum& operator*=(int i)
	{
		m_dwQuan *= i;
		return *this;
	}
	CQuanNum& operator*=(float i)
	{
		CQuanNum a(i);
		(*this) *= a;
		return *this;
	}
	CQuanNum& operator/=(int i)
	{
		m_dwQuan /= i;
		return *this;
	}
	CQuanNum operator+(const CQuanNum& a) const
	{
		CQuanNum q;
		q.m_dwQuan = m_dwQuan + a.m_dwQuan;
		return q;
	}
	CQuanNum operator-(const CQuanNum& a) const
	{
		CQuanNum q;
		q.m_dwQuan = m_dwQuan - a.m_dwQuan;
		return q;
	}
	CQuanNum& operator+=(const CQuanNum& a)
	{
		m_dwQuan += a.m_dwQuan;
		return *this;
	}
	CQuanNum operator-=(const CQuanNum& a)
	{
		m_dwQuan -= a.m_dwQuan;
		return *this;
	}
	CQuanNum operator*(const CQuanNum& a) const
	{
		// TODO: May overflow
		CQuanNum q;
		q.m_dwQuan = (m_dwQuan * a.m_dwQuan) >> QuanBit;
		return q;
	}
	CQuanNum& operator*=(const CQuanNum& a)
	{
		// TODO: May overflow
		m_dwQuan = (m_dwQuan * a.m_dwQuan) >> QuanBit;
		return *this;
	}
	bool operator<(const CQuanNum& b) const
	{
		return (m_dwQuan<b.m_dwQuan);
	}
	bool operator>(const CQuanNum& b) const
	{
		return (m_dwQuan>b.m_dwQuan);
	}

	// Converting to build-in types
	int ToInt() const
	{
		return (m_dwQuan >> QuanBit);
	}
	float ToFloat() const
	{
		return (float)m_dwQuan / QuanScale;
	}
};

typedef CQuanNum<13> CParamQuanNum;

/*
template<BYTE8 BitLen> CQuanNum<BitLen> operator*(int i, const CQuanNum<BitLen>& a);
static CQuanNum<13> operator*(int i, const CQuanNum<13>& a)
{
	CQuanNum<13> q;
	q.m_dwQuan = a.m_dwQuan * i;
	return q;
}
*/

//高清版本参数，软件狗使用的参数结构
class PlateRecogParam_High
{
public:
	// 应用场合
	PlateRecogMode g_PlateRcogMode;
	PlateRecogSpeed g_PlateRecogSpeed;
	MovingDirection	g_MovingDirection;	// 车辆运动方向

	// for tracking
	HV_RECT g_rectDetectionArea;	// 整体检测时的检测区域, percentage
	HV_RECT g_rectTrackArea;	    // track离开该区域后快速结束跟踪, percentage

	CParamQuanNum g_fltTrackInflateX;		// tracking时下一帧区域在X方向上的增大相对于trackrect_width的比例
	CParamQuanNum g_fltTrackInflateY;		// tracking时下一帧区域在Y方向上的增大相对于trackrect_height的比例

	int g_nContFrames_EstablishTrack;		// of continuously observed frames for establishing a track
	int g_nMissFrames_EndTrack;				// of missing frames for ending a track
	int g_nMissFrames_EndTrackQuick;		// of missing frames for ending a track in EndTrackArea
	CParamQuanNum g_fltThreshold_StartTrack;		// threshold of a good recognition for starting a track
	CParamQuanNum g_fltThreshold_BeObservation;	    // threshold of a good recognition to be an observation

	// for plate detection
	int g_nDetMinScaleNum;			// 车牌检测框的最小宽度=56*1.1^g_nDetMinScaleNum
	int g_nDetMaxScaleNum;			// 车牌检测框的最大宽度=56*1.1^g_nDetMaxScaleNum
	int g_nDetMinStdVar;			// 方差>g_nDetMinStdVar的区域才认为可能是车牌
	int	g_nMinStopPlateDetNum;		// 达到这个数量就停止检测
	bool g_fSubscanPredict;			// tracking时是否使用预测算法
	int g_nSubscanPredictMinScale;	// tracking预测时scale变化范围的最小值
	int g_nSubscanPredictMaxScale;	// tracking预测时scale变化范围的最大值

	CParamQuanNum g_kfltPlateInflateRate;	// 检测框增宽的比例
	CParamQuanNum g_kfltPlateInflateRateV;	// 检测框增高的比例
	CParamQuanNum g_kfltHighInflateArea;	// 车牌的y值>g_kfltHighInflateArea后，用第二套更大一点的比例膨胀检测框
	CParamQuanNum g_kfltPlateInflateRate2;	// 更大的检测框增宽的比例
	CParamQuanNum g_kfltPlateInflateRateV2;	// 更大的检测框增高的比例

	bool g_fSegPredict;				// 是否使用切分的预测算法

	int g_nMinPlateWidth;			// 进行识别的最小车牌宽度（以分割后的结果为准）
	int g_nMaxPlateWidth;			// 进行识别的最大车牌宽度（以分割后的结果为准）
	bool g_fBestResultOnly;			// 如果图像中有多个识别结果，只输出最好的

	int g_nMinDownSampleWidthInSeg;	    // 分割时DownSample的最小宽度
	int g_nMinDownSampleHeightInSeg;   // 分割时DownSample的最小高度

	HV_RECT g_rectDetectionArea_DBL;
	int g_nDBLDetMinScaleNum_DBL;
	int g_nDBLDetMaxScaleNum_DBL;
};

class PlateRecogParam
{
public:
	// 应用场合
	PlateRecogMode g_PlateRcogMode;
	PlateRecogSpeed g_PlateRecogSpeed;
	MovingDirection	g_MovingDirection;	// 车辆运动方向

	// for tracking
	int g_cnDetStep;				// 是否每帧都做整体检测，还是隔g_cnDetStep做一次
	HV_RECT g_rectDetectionArea;	// 整体检测时的检测区域, percentage
	HV_RECT g_rectTrackArea;	    // track离开该区域后快速结束跟踪, percentage

	CParamQuanNum g_fltTrackInflateX;		// tracking时下一帧区域在X方向上的增大相对于trackrect_width的比例
	CParamQuanNum g_fltTrackInflateY;		// tracking时下一帧区域在Y方向上的增大相对于trackrect_height的比例

	int g_nContFrames_EstablishTrack;		        // of continuously observed frames for establishing a track
	int g_nMissFrames_EndTrack;				        // of missing frames for ending a track
	int g_nMissFrames_EndTrackQuick;		        // of missing frames for ending a track in EndTrackArea
	int g_nObservedFrames_ConsiderAsTrack;	        // of observed frames to be considered as a successful track
	CParamQuanNum g_fltThreshold_StartTrack;		// threshold of a good recognition for starting a track
	CParamQuanNum g_fltThreshold_BeObservation;	    // threshold of a good recognition to be an observation
	CParamQuanNum g_fltMinConfidenceForVote;		// minimal confidence for voting, when there are not enought observations

	// for plate detection
	int g_nDetMinScaleNum;			// 车牌检测框的最小宽度=56*1.1^g_nDetMinScaleNum
	int g_nDetMaxScaleNum;			// 车牌检测框的最大宽度=56*1.1^g_nDetMaxScaleNum
	int g_nDetMinStdVar;			// 方差>g_nDetMinStdVar的区域才认为可能是车牌
	int	g_nMinStopPlateDetNum;		// 达到这个数量就停止检测
	bool g_fSubscanPredict;			// tracking时是否使用预测算法
	int g_nSubscanPredictMinScale;	// tracking预测时scale变化范围的最小值
	int g_nSubscanPredictMaxScale;	// tracking预测时scale变化范围的最大值

	CParamQuanNum g_kfltPlateInflateRate;	// 检测框增宽的比例
	CParamQuanNum g_kfltPlateInflateRateV;	// 检测框增高的比例
	CParamQuanNum g_kfltHighInflateArea;	// 车牌的y值>g_kfltHighInflateArea后，用第二套更大一点的比例膨胀检测框
	CParamQuanNum g_kfltPlateInflateRate2;	// 更大的检测框增宽的比例
	CParamQuanNum g_kfltPlateInflateRateV2;	// 更大的检测框增高的比例

	bool g_fSegPredict;				// 是否使用切分的预测算法

	int g_nMinPlateWidth;			// 进行识别的最小车牌宽度（以分割后的结果为准）
	int g_nMaxPlateWidth;			// 进行识别的最大车牌宽度（以分割后的结果为准）
	bool g_fBestResultOnly;			// 如果图像中有多个识别结果，只输出最好的

	int g_nMinDownSampleWidthInSeg;	// 分割时DownSample的最小宽度
	int g_nMinDownSampleHeightInSeg;// 分割时DownSample的最小高度

	int m_fUsedTrafficLight;		//电子警察开关
	int m_fUseTemplet;				// 模版匹配开关

	int m_iDetectCarStopEnable;		    //车辆停止检测开关
	int m_iDetectReverseEnable;			//车辆逆行检测开关
	int m_iDetectOverYellowLineEnable;	//车辆压黄线检测开关
	int m_iDetectCrossLineEnable;		//车辆跨线检测开关
	PlateRecogParam &operator=(const PlateRecogParam_High &param)
	{
		g_PlateRcogMode = param.g_PlateRcogMode;
		g_PlateRecogSpeed = param.g_PlateRecogSpeed;
		g_MovingDirection = param.g_MovingDirection;

		g_cnDetStep = 0;
		g_rectDetectionArea = param.g_rectDetectionArea;
		g_rectTrackArea = param.g_rectTrackArea;

		g_fltTrackInflateX = param.g_fltTrackInflateX;
		g_fltTrackInflateY = param.g_fltTrackInflateY;

		g_nContFrames_EstablishTrack = param.g_nContFrames_EstablishTrack;
		g_nMissFrames_EndTrack = param.g_nMissFrames_EndTrack;
		g_nMissFrames_EndTrackQuick = param.g_nMissFrames_EndTrackQuick;
		g_nObservedFrames_ConsiderAsTrack = 0;
		g_fltThreshold_StartTrack = param.g_fltThreshold_StartTrack;
		g_fltThreshold_BeObservation = param.g_fltThreshold_BeObservation;
		g_fltMinConfidenceForVote = 0.0f;

		g_nDetMinScaleNum = param.g_nDetMinScaleNum;
		g_nDetMaxScaleNum = param.g_nDetMaxScaleNum;
		g_nDetMinStdVar = param.g_nDetMinStdVar;
		g_nMinStopPlateDetNum = param.g_nMinStopPlateDetNum;
		g_fSubscanPredict = param.g_fSubscanPredict;
		g_nSubscanPredictMinScale = param.g_nSubscanPredictMinScale;
		g_nSubscanPredictMaxScale = param.g_nSubscanPredictMaxScale;

		g_kfltPlateInflateRate = param.g_kfltPlateInflateRate;
		g_kfltPlateInflateRateV = param.g_kfltPlateInflateRateV;
		g_kfltHighInflateArea = param.g_kfltHighInflateArea;
		g_kfltPlateInflateRate2 = param.g_kfltPlateInflateRate2;
		g_kfltPlateInflateRateV2 = param.g_kfltPlateInflateRateV2;

		g_fSegPredict = param.g_fSegPredict;

		g_nMinPlateWidth = param.g_nMinPlateWidth;
		g_nMaxPlateWidth = param.g_nMaxPlateWidth;
		g_fBestResultOnly = param.g_fBestResultOnly;

		g_nMinDownSampleWidthInSeg = param.g_nMinDownSampleWidthInSeg;
		g_nMinDownSampleHeightInSeg = param.g_nMinDownSampleHeightInSeg;

		m_fUsedTrafficLight = 0;
		m_fUseTemplet = 0;

		m_iDetectCarStopEnable = 0;
		m_iDetectReverseEnable = 0;
		m_iDetectOverYellowLineEnable = 0;
		m_iDetectCrossLineEnable = 0;

		return *this;
	}
};

extern "C" HRESULT GetPlateRecogParamCount(PDWORD32 pdwCount);
HRESULT LoadPlateRecogParam(DWORD32 dwIndex);
int GetRecogParamIndexOnName(LPCSTR lpszName);
HRESULT GetRecogParamNameOnIndex(DWORD32 dwIndex, char *pszName);
extern "C" PlateRecogParam* GetCurrentParam();
extern "C" PlateRecogMode GetCurrentMode();

extern "C" PlateRecogParam g_rgPlateRecogParam[PRM_MaxMode];

#if RUN_PLATFORM == PLATFORM_DSP_BIOS
LPCSTR GetPlateRecogParamName(DWORD32 dwIndex);
#else
HRESULT GetPlateRecogParamName(LPWSTR lpszName, DWORD32 dwBufLen, DWORD32 dwIndex);
#endif

extern "C" DWORD32 GetChnCharSetCount();
LPCSTR GetChnCharSetName( DWORD32 dwIndex );

//工作场合配置文件序列号
#define SERIAL_CONFIG		100

#endif // _PARAMETER_INCLUDED__
