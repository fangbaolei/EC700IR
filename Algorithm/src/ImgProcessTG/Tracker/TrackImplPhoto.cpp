#include "TrackImplPhoto.h"
#include "hvutils.h"
#include "globaldatatbl.h"
#include <math.h>

#include "hvcrop.h"

#define CROP_COUNT 1
#define PRETREATMENT_COUNT 4

#define NORMAL_NUM 12
#define YELLOW_NUM 16
#define BIGSUN_NUM 10

#define MAX2(a, b)  ((a) > (b) ? (a) : (b))
#define MIN2(a, b)  ((a) < (b) ? (a) : (b))

namespace Plate
{
	// grammer of the characters
	#define DEFINE_PLATE_GRAMMER_NAME(a, b, r1, r2, r3, r4, r5, r6, r7)		r1, r2, r3, r4, r5, r6, r7,
	const int g_cnType[PLATE_TYPE_COUNT][7]=
	{
		PLATE_TYPE_LIST(DEFINE_PLATE_GRAMMER_NAME)
	};
}

HRESULT CreateTracker_Photo(ITracker** ppTracker)
{
	if (ppTracker == NULL)
	{
		return E_POINTER;
	}

	CTrackerImpl_Photo *pTrackerImpl = new CTrackerImpl_Photo();

	if (pTrackerImpl == NULL)
	{
		return E_OUTOFMEMORY;
	}

	//创建各功能组件
	RTN_HR_IF_FAILED(pTrackerImpl->OnCreate());

	// final assignment
	*ppTracker=pTrackerImpl;

	return S_OK;
}

CTrackerImpl_Photo::CTrackerImpl_Photo()
	: m_fInitialized(FALSE)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_nStride(0)
	, m_pParam(NULL)
	, m_pPlateDetector(NULL)
	, m_pProcessPlate(NULL)
	, m_pRecognition(NULL)
	, m_pInspector(NULL)
	, m_pHvModel(NULL)
	, m_nMaxCropWidth(2600)
	, m_nMaxCropHeight(810)
{
	srand(0);

	//可配置参数

	// 是否输出调试信息
	m_fOutputDebugInfo = 0;

	// 是否判断广西警标志
	m_iRecogGxPolice = 1;

	// 本地化汉字，58为“桂”
	m_nDefaultChnId = 58;

	//新WJ
	m_fEnableNewWJSegment = TRUE;
	m_fEnableDefaultWJ    = FALSE;
	HV_memset( m_strDefaultWJChar, 0, 3 );
	HV_memcpy( m_strDefaultWJChar, "16", 2);

	//校正开关
	m_fEnableRectify = 1;

	//车身颜色识别开关
	m_fEnableRecgCarColor = FALSE;

	//优先出牌方位
	m_nFirstPlatePos = 0;

	//摩托车检测阈值
	m_iMiddleWidth = 70;
	m_iBottomWidth = 90;
	//m_iMiddleWidth = 0;
	//m_iBottomWidth = 0;
}

void CTrackerImpl_Photo::Clear()
{
	SAFE_DELETE(m_pPlateDetector);
	SAFE_DELETE(m_pProcessPlate);
	SAFE_DELETE(m_pRecognition);
}

CTrackerImpl_Photo::~CTrackerImpl_Photo()
{
	Clear();
}

//创建功能模块
HRESULT CTrackerImpl_Photo::OnCreate()
{
	Clear();

	//初始化公用数据
	GlobleDataSrcInit();

	//组件创建
	RTN_HR_IF_FAILED(CreatePlateDetectorInstance(&m_pPlateDetector));
	RTN_HR_IF_FAILED(CreateRecognition(&m_pRecognition));
	RTN_HR_IF_FAILED(CreateProcessPlateInstance(&m_pProcessPlate));

	//建立组件关系
	RTN_HR_IF_FAILED(m_pProcessPlate->SetDetector(m_pPlateDetector));
	RTN_HR_IF_FAILED(m_pProcessPlate->SetRecognition(m_pRecognition));

	return S_OK;
}

HRESULT CTrackerImpl_Photo::OnInit()
{
	//清理

	//内部变量恢复初始值
	//可配置参数不需要恢复
	m_fInitialized = FALSE;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nStride = 0;

	return S_OK;
}

void CTrackerImpl_Photo::RoadInfoParamInit()
{
	m_iRoadNumber = 2;
	for(int i = 0; i < CTrackInfo::s_iMaxRoadNumber; ++i)
	{
		m_roadInfo[i].ptTop.x = 0;
		m_roadInfo[i].ptTop.y = 0;
		m_roadInfo[i].ptBottom.x = 0;
		m_roadInfo[i].ptBottom.y = 0;
		m_roadInfo[i].iLineType = 0;

		m_roadInfo[i].iRoadType = (int)(RRT_FORWARD | RRT_LEFT | RRT_RIGHT | RRT_TURN);
		m_roadInfo[i].iFilterRunType = 0;
	}
}

void CTrackerImpl_Photo::InitRoadInfoParam()
{
	RoadInfoParamInit();
}

//初始化TRACKER和各组件
HRESULT CTrackerImpl_Photo::InitTracker(TRACKER_CFG_PARAM* pCfgParam)
{
	m_fInitialized = FALSE;

	if (pCfgParam == NULL)
	{
		return E_INVALIDARG;
	}

	//恢复初始状态
	OnInit();

	PROCESS_PLATE_CFG_PARAM cfgProcessPlateParam;
	RECOGNITION_CFG_PARAM cfgRecognitionParam;
	PLATE_DETECT_CFG_PARAM cfgPlateDetectParam;

	memcpy(&m_cfgParam, pCfgParam, sizeof(m_cfgParam));

	cfgProcessPlateParam.nBlackPlate_S = m_cfgParam.nProcessPlate_BlackPlate_S;
	cfgProcessPlateParam.nProcessPlate_LightBlue = pCfgParam->nProcessPlate_LightBlue;
	cfgProcessPlateParam.fEnableDoublePlate = pCfgParam->fDoublePlateEnable;

	cfgRecognitionParam.fEnableAlphaRecog = pCfgParam->fAlpha7;

	cfgPlateDetectParam.nPlateDetect_Green = pCfgParam->nPlateDetect_Green;
	cfgPlateDetectParam.fltPlateDetect_StepAdj = pCfgParam->fltPlateDetect_StepAdj;

	m_nFirstPlatePos = pCfgParam->nFirstPlatePos;
	m_iMiddleWidth = pCfgParam->nMiddleWidth;
	m_iBottomWidth = pCfgParam->nBottomWidth;
	m_fEnableRecgCarColor = pCfgParam->fEnableRecgCarColor;

	m_cfgParam.fltMinConfForOutput = 1.0; //抓拍模式下只出一个结果

	InitRoadInfoParam();

	//配置其他参数
	//使用收费站模式
	//具体模式由应用层设置,标清=收费站模式,HVC=HVC模式
	m_PlateRecogParam = *GetCurrentParam();
	m_pParam = &m_PlateRecogParam;

	//各组件初始化
	m_nMaxCropWidth = 2600;
	if(GetCurrentMode() == PRM_HVC)
	{
#ifdef CHIP_DM642
		m_nMaxCropWidth = 1600;
		m_nMaxCropHeight = 800;
#else
    m_nMaxCropHeight = 1060;
#endif
	}
	else
	{
		//图片版
		m_nMaxCropHeight = 810;
	}
	RTN_HR_IF_FAILED(m_pPlateDetector->Initialize(m_pParam, &cfgPlateDetectParam, m_nMaxCropWidth, m_nMaxCropHeight));
	RTN_HR_IF_FAILED(m_pRecognition->InitRecognition(&cfgRecognitionParam));
	RTN_HR_IF_FAILED(m_pProcessPlate->Initialize(NO_PREDICT_PROCESS_PLATE_MODE, &cfgProcessPlateParam));

	m_fInitialized = TRUE;

	return S_OK;
}

HRESULT CTrackerImpl_Photo::SetInspector(
	IInspector *pInspector
)
{
	HRESULT hr(S_OK);

	m_pInspector = pInspector;
	if (m_pRecognition)
	{
		RTN_HR_IF_FAILED(m_pRecognition->SetInspector(pInspector));
	}
	if (m_pProcessPlate)
	{
		RTN_HR_IF_FAILED(m_pProcessPlate->SetInspector(m_pInspector));
	}
	return hr;
}

//设置参数接口
HRESULT CTrackerImpl_Photo::SetHvParam(HvCore::IHvParam2 *pHvParam)
{
	if (m_pRecognition) m_pRecognition->SetHvParam(NULL);
	if (m_pPlateDetector) m_pPlateDetector->SetHvParam(NULL);
	if (m_pProcessPlate) m_pProcessPlate->SetHvParam(NULL);

	return S_OK;
}

//设置模型加载接口
HRESULT CTrackerImpl_Photo::SetHvModel(HvCore::IHvModel* pHvModel)
{
	m_pHvModel = pHvModel;

	if (m_pPlateDetector) m_pPlateDetector->SetHvModel(m_pHvModel);
	if (m_pRecognition) m_pRecognition->SetHvModel(m_pHvModel);
	return S_OK;
}

//设置性能数据接口
HRESULT STDMETHODCALLTYPE CTrackerImpl_Photo::SetHvPerformance(
							HvCore::IHvPerformance* pHvPerf
							)
{
	m_pHvPerf = pHvPerf;

	if(m_pProcessPlate) m_pProcessPlate->SetHvPerformance(m_pHvPerf);
	if (m_pPlateDetector) m_pPlateDetector->SetHvPerformance(m_pHvPerf);
	if (m_pRecognition) m_pRecognition->SetHvPerformance(m_pHvPerf);
	return S_OK;
}

HRESULT CTrackerImpl_Photo::SetFrameProperty(
		const FRAME_PROPERTY& FrameProperty
)
{
	m_nWidth = FrameProperty.iWidth;
	m_nHeight = FrameProperty.iHeight;
	m_nStride = FrameProperty.iStride;

	return S_OK;
}

__inline DWORD32 GetValue(DWORD32* pBuf, int nRow, int nCol, int nStride)
{
	return *(pBuf + nRow * nStride + nCol);
}

HRESULT TrimImage(
				  HV_COMPONENT_IMAGE& imgSrc,
				  CRect& rcCrop,
				  float fltVRatio = 1.0,
				  float fltHRatio = 1.0
				  )
{
	int nWidth = imgSrc.iWidth;
	int nHeight = imgSrc.iHeight;

	CFastMemAlloc cStack;
	int nBufLen = nWidth * nHeight;
	BYTE8* pBuf = (BYTE8*)cStack.StackAlloc(nBufLen);
	HV_memset(pBuf, 0, nBufLen);

	//算垂直边缘
	IMAGE_sobel_V_NEW(GetHvImageData(&imgSrc, 0), pBuf, imgSrc.iWidth, imgSrc.iHeight, imgSrc.iStrideWidth[0]);

	int nItgWidth = nWidth + 1;
	int nItgHeight = nHeight + 1;

	nBufLen = nItgWidth * nItgHeight;
	DWORD32* pItgBuf = (DWORD32*)cStack.StackAlloc(nBufLen * sizeof(DWORD32));
	HV_memset(pItgBuf, 0, nBufLen * sizeof(DWORD32));

	//算垂直边缘积分图
	CreateItgImage(nItgWidth*4, pItgBuf, nHeight, nWidth, nWidth, pBuf);

	DWORD32 nTotal = GetValue(pItgBuf,nHeight,nWidth,nItgWidth);

	DWORD32 nV0,nV1,nV2,nV3;
	nV0 = DWORD32(nTotal * (1.0 - fltHRatio)/2);	//左
	nV1 = DWORD32(nTotal * (1.0 - fltVRatio)/2);	//上
	nV2 = DWORD32(nTotal * (1.0 + fltHRatio)/2);	//右
	nV3 = DWORD32(nTotal * (1.0 + fltVRatio)/2);	//下

	DWORD32 nMaxLeft = nWidth/2;
	DWORD32 nMaxTop = nHeight/2;
	DWORD32 nMinRight = nWidth/2;
	DWORD32 nMinBottom = nHeight/2;

	DWORD32 nV;

	int nLeft = 0;
	for(; nLeft < nWidth; nLeft++)
	{
		nV = GetValue(pItgBuf, nHeight, nLeft, nItgWidth);
		if( nV >= nV0) break;
	}
	nLeft = MIN_INT(nLeft, (int)nMaxLeft);

	int nTop = 0;
	for(;nTop < nHeight; nTop++)
	{
		nV = GetValue(pItgBuf, nTop, nWidth, nItgWidth);
		if(nV >= nV1) break;
	}
	nTop = MIN_INT(nTop, (int)nMaxTop);

	int nRight = nWidth;
	for(; nRight > nLeft; nRight --)
	{
		nV = GetValue(pItgBuf, nHeight, nRight, nItgWidth);
		if( nV <= nV2) break;
	}
	nRight = MAX_INT(nRight, (int)nMinRight);

	int nBottom = nHeight;
	for(;nBottom > nTop; nBottom --)
	{
		nV = GetValue(pItgBuf, nBottom, nWidth, nItgWidth);
		if(nV <= nV3) break;
	}
	nBottom = MAX_INT(nBottom, (int)nMinBottom);

	rcCrop = CRect(nLeft,nTop,nRight,nBottom);

	return S_OK;
}

//扩边截图
HRESULT CTrackerImpl_Photo::CropPlateImage(
									 const HV_COMPONENT_IMAGE& imgPhoto,
									 CRect& rcOriPos,
									 PLATE_TYPE nPlateType,
									 HV_COMPONENT_IMAGE* rgPlateImage,
									 int nCount	//指定截取图的结果数
									 )
{
	int nWidth(rcOriPos.Width());
	int nHeight(rcOriPos.Height());

	CRect rcFull(0,0,imgPhoto.iWidth,imgPhoto.iHeight);

	double nHRatio_Double = 80.0;
	double nHRation_Normal = 160.0;

	for( int i = 0; i < nCount; i++)
	{
		for(int j = 0; j < i; j++)
		{
			nHRatio_Double *= 1.1;
			nHRation_Normal *= 1.1;
		}

		CRect rcCrop = rcOriPos;

		if (( PLATE_DOUBLE_YELLOW == nPlateType )
			|| (PLATE_DOUBLE_MOTO == nPlateType))
		{
			rcCrop.left -= int(nWidth * nHRatio_Double) >> 10;
			rcCrop.right += int(nWidth * nHRatio_Double) >> 10;
			rcCrop.top -= int(nHeight * 80) >> 10;
		}
		else
		{
			rcCrop.left -= int(nWidth * nHRation_Normal) >> 10;
			rcCrop.right += int(nWidth * nHRation_Normal) >> 10;
		}

		nHeight = rcCrop.Height();
		if (nHeight >= MAX_SEG_IMG_HIGH)		//60为分割时积分图最大高度
		{
			rcCrop.top -= (nHeight - MAX_SEG_IMG_HIGH + 1) >> 1;
			rcCrop.bottom += nHeight - MAX_SEG_IMG_HIGH + 1 - ((nHeight - MAX_SEG_IMG_HIGH + 1) >> 1);
		}
		nWidth = rcCrop.Width();
		if (nWidth >= MAX_SEG_IMG_WIDTH)		//400为分割时积分图最大宽度
		{
			rcCrop.left -= ((nWidth - MAX_SEG_IMG_WIDTH + 1) >> 1);
			rcCrop.right += (nWidth - MAX_SEG_IMG_WIDTH + 1 - ((nWidth - MAX_SEG_IMG_WIDTH + 1) >> 1));
		}

		rcCrop.IntersectRect(&rcFull, &rcCrop);
		CropImage(imgPhoto, rcCrop, rgPlateImage + i);
	}

	return S_OK;
}

/*void EnhanceColor(HV_COMPONENT_IMAGE* pImg)
{
	if(!pImg) return;

	RESTRICT_PBYTE8 pbCbBuf = GetHvImageData(pImg, 1);
	int iWidth = pImg->iWidth / 2;
	int iHeight = pImg->iHeight;
	int iStrideWidth = pImg->iStrideWidth[0]/2;

	for(int i = 0; i < iHeight; i++, pbCbBuf += iStrideWidth)
	{
		for(int j = 0; j < iWidth; j++)
		{
			pbCbBuf[j] = MAX_INT(MIN_INT((pbCbBuf[j] - 128) * 2 + 128, 255), 0);
		}
	}
}
*/

int CaclAvgGray(HV_COMPONENT_IMAGE imgCropPlate, int iAdd)
{
	// iAdd 为截图间隔
	int nWidth = imgCropPlate.iWidth;
	int nHeight = imgCropPlate.iHeight;
	int nCount = nWidth * nHeight;
	if(nCount == 0) return 0;

	nCount = 0;
	int nSum = 0;
	RESTRICT_PBYTE8 pLine = GetHvImageData(&imgCropPlate, 0);
	for(int i = 0; i < nHeight; i += iAdd, pLine += imgCropPlate.iStrideWidth[0])
	{
		for(int j = 0; j < nWidth; j += iAdd)
		{
			nSum += pLine[j];
			nCount++;
		}
	}

	return nSum/nCount;
}

//HRESULT CTrackerImpl_Photo::PreTreatment(
//								   HV_COMPONENT_IMAGE& imgPlate,
//								   PLATE_TYPE nPlateType,
//								   PLATE_COLOR nPlateColor,
//								   BOOL fEnhance,
//								   CPersistentComponentImage* rgImgRectified,
//								   int nCount,	 //指定预处理生成的图数量,通过改变垂直校正角度
//								   CBinaryInfo &BinInfo
//								   )
//{
//	if(rgImgRectified == NULL || nCount < 1) return E_INVALIDARG;
//
//	rgImgRectified[0].Assign(imgPlate);
//
//	CPersistentComponentImage imgEnhance;
//	if(fEnhance)
//	{
//		imgEnhance.Assign(imgPlate);
//		PlateEnhance(&imgEnhance);
//		imgPlate = (HV_COMPONENT_IMAGE)imgEnhance;
//		EnhanceColor(&imgPlate);
//	}
//
//	CPersistentComponentImage imgDownSample;
//	if(	imgPlate.iWidth > MAX_SEG_IMG_WIDTH ||
//		imgPlate.iHeight > MAX_SEG_IMG_HIGH )
//	{
//		ImageDownSample(imgDownSample, imgPlate);
//		imgPlate = (HV_COMPONENT_IMAGE)imgDownSample;
//	}
//
//	CPersistentComponentImage imgUpSample;
//	if(imgPlate.iHeight < 18 && imgPlate.iHeight > 0 )
//	{
//		imgUpSample.Create(HV_IMAGE_YUV_422, imgPlate.iWidth * 30.0 / imgPlate.iHeight, 30);
//		PlateResize(imgPlate, imgUpSample);
//		imgPlate = imgUpSample;
//	}
//
//	CRectifyInfo RectifyInfo;
//
//	CPersistentComponentImage imgRectified;
//	imgRectified.Assign(imgPlate);
//	RTN_HR_IF_FAILED(m_pProcessPlate->CalcRectifyInfo(
//		&imgPlate,
//		&imgRectified,
//		&RectifyInfo,
//		&BinInfo,
//		&nPlateType,
//		&nPlateColor
//		));
////	RTN_HR_IF_FAILED(hvCalcRectifyInfo(imgPlate, RectifyInfo.fltVAngle, RectifyInfo.fltHAngle));
//
////	RectifyInfo.fltVAngle = 90;
////	RectifyInfo.fltHAngle = 0;
//
//	if(m_fEnableRectify)
//	{
//		RectifyInfo.fltVAngle += RectifyInfo.fltHAngle;
//
//		if(RectifyInfo.fltVAngle > 100 ||
//			RectifyInfo.fltVAngle < 80)
//		{
//			RectifyInfo.fltVAngle = (RectifyInfo.fltVAngle - 90)/2 + 90;
//		}
//
//		if(RectifyInfo.fltVAngle > 100) RectifyInfo.fltVAngle = 100;
//		if(RectifyInfo.fltVAngle < 80) RectifyInfo.fltVAngle = 80;
//
//		float fltMaxVAngle = (RectifyInfo.fltVAngle - 90), fltVAngle;
//		if( nCount > 1)
//		{
//			if(fltMaxVAngle < 0 && fltMaxVAngle > -5) fltMaxVAngle = -5;
//			if(fltMaxVAngle >= 0 && fltMaxVAngle < 5) fltMaxVAngle = 5;
//		}
//
//		CRectifyInfo cReRectifyInfo = RectifyInfo;
//		for(int i = 0; i < nCount; i++)
//		{
//			fltVAngle = fltMaxVAngle * ((nCount>1)?((float)(nCount - i - 1)/(nCount - 1)):1);
//			cReRectifyInfo.fltVAngle = fltVAngle + 90;
//
//			rgImgRectified[i].Assign(imgPlate);
//			RTN_HR_IF_FAILED(m_pProcessPlate->RectifyPlate(
//				&imgPlate, &cReRectifyInfo, &rgImgRectified[i], false ));
//		}
//	}
//	else

//	{
//		for(int i = 0; i < nCount; i++)
//		{
//			rgImgRectified[i].Assign(imgPlate);
//		}
//	}
//
//	return S_OK;
//}

HRESULT CTrackerImpl_Photo::PreTreatment(
	HV_COMPONENT_IMAGE& imgPlate,
	PLATE_TYPE nPlateType,
	PLATE_COLOR nPlateColor,
	BOOL fEnhance,
	CPersistentComponentImage* rgImgRectified,
	int nCount,	 //指定预处理生成的图数量,通过改变垂直校正角度
	CBinaryInfo &BinInfo
	)
{
	if(rgImgRectified == NULL || nCount < 1) return E_INVALIDARG;

	rgImgRectified[0].Assign(imgPlate);

	CPersistentComponentImage imgEnhance;
	if(fEnhance)
	{
		imgEnhance.Assign(imgPlate);
		PlateEnhance(&imgEnhance);
		imgPlate = (HV_COMPONENT_IMAGE)imgEnhance;
		EnhanceColor(&imgPlate);
	}

	CPersistentComponentImage imgDownSample;
	if(	imgPlate.iWidth > MAX_SEG_IMG_WIDTH ||
		imgPlate.iHeight > MAX_SEG_IMG_HIGH )
	{
		ImageDownSample(imgDownSample, imgPlate);
		imgPlate = (HV_COMPONENT_IMAGE)imgDownSample;
	}

	CPersistentComponentImage imgUpSample;
	if(imgPlate.iHeight < 18 && imgPlate.iHeight > 0 )
	{
		imgUpSample.Create(HV_IMAGE_YUV_422, imgPlate.iWidth * 30 / imgPlate.iHeight, 30);
		PlateResize(imgPlate, imgUpSample);
		imgPlate = imgUpSample;
	}

	CRectifyInfo RectifyInfo;

	CPersistentComponentImage imgRectified;
	imgRectified.Assign(imgPlate);
	RTN_HR_IF_FAILED(m_pProcessPlate->CalcRectifyInfo(
		&imgPlate,
		&imgRectified,
		&RectifyInfo,
		&BinInfo,
		&nPlateType,
		&nPlateColor
		));

	if(m_fEnableRectify)
	{
		//RectifyInfo.fltVAngle += RectifyInfo.fltHAngle;

		if(RectifyInfo.fltVAngle > 100 ||
			RectifyInfo.fltVAngle < 80)
		{
			RectifyInfo.fltVAngle = (RectifyInfo.fltVAngle - 90)/2 + 90;
		}

		float fltMaxVAngle = (RectifyInfo.fltVAngle - 90);

		if(fltMaxVAngle < 0)
		{
			fltMaxVAngle = fltMaxVAngle>-5?-5:fltMaxVAngle;
		}
		else
		{
			fltMaxVAngle = fltMaxVAngle<5?5:fltMaxVAngle;
		}


		CRectifyInfo cReRectifyInfo = RectifyInfo;
		for(int i = 0; i < nCount; i++)
		{
			if(fltMaxVAngle < 0)
			{
				cReRectifyInfo.fltVAngle = fltMaxVAngle + 5*i/nCount + 90;
			}
			else
			{
				cReRectifyInfo.fltVAngle = fltMaxVAngle - 5*i/nCount + 90;
			}

			rgImgRectified[i].Assign(imgPlate);
			RTN_HR_IF_FAILED(m_pProcessPlate->RectifyPlate(
				&imgPlate, &cReRectifyInfo, &rgImgRectified[i], false ));

			//{
			//	static int nCount = 0;
			//	wchar_t szPath[MAX_PATH]={0};
			//	swprintf(szPath, L"d:\\CropRectify\\%08d.bmp",nCount++);
			//	SaveImage(szPath, &rgImgRectified[i]);
			//}
		}
	}
	else
	{
		for(int i = 0; i < nCount; i++)
		{
			rgImgRectified[i].Assign(imgPlate);
		}
	}

	return S_OK;
}

const unsigned int knPlateStrLen = 7;// TODO: Magic Number

int g_rcTransMode[][3] = {
	{0,0,0},
	{-1,0,0},
	{1,0,0},
	{0,-1,0},
	{0,1,0}
};

#define _CHAR_TRANSMODE_COUNT sizeof(g_rcTransMode)/sizeof(g_rcTransMode[0])

HRESULT CTrackerImpl_Photo::RecognizeChar(
									HV_COMPONENT_IMAGE imgPlate,
									HV_RECT rcOriChar,
									RECOGNITION_TYPE RecogType,
									PlateInfo	&cPlateInfo,
									PBYTE8 pbResult,
									PSDWORD32 pdwProb,
									int iCharPos
									)
{
	if(pbResult) *pbResult = 0;
	if(pdwProb) *pdwProb = 700;

	BYTE8 rgIdx[_CHAR_TRANSMODE_COUNT] = {0};
	SDWORD32 rgConf[_CHAR_TRANSMODE_COUNT] ={0};

	CRect rcChar = rcOriChar;

	CRect rcPlate(0,0,imgPlate.iWidth,imgPlate.iHeight);
	HV_COMPONENT_IMAGE imgChar;
	imgChar.nImgType = HV_IMAGE_GRAY;

	int nTransCount = (iCharPos == 0)?3:_CHAR_TRANSMODE_COUNT;

	for(int i = 0; i < nTransCount; i++)
	{
		rcChar = rcOriChar;
		rcChar.OffsetRect(g_rcTransMode[i][0], g_rcTransMode[i][1]);
		rcChar.DeflateRect(g_rcTransMode[i][2], g_rcTransMode[i][2]);
		rcChar.IntersectRect(&rcPlate, &rcChar);

		SetHvImageData(&imgChar, 0, GetHvImageData(&imgPlate, 0) + rcChar.left + rcChar.top * imgPlate.iStrideWidth[0]);
		imgChar.iWidth = rcChar.Width();
		imgChar.iHeight = rcChar.Height();
		imgChar.iStrideWidth[0] = imgPlate.iStrideWidth[0];

		m_pRecognition->Evaluate(
			&imgChar,
			RecogType,
			cPlateInfo.nPlateType,
			cPlateInfo.color,
			&rgIdx[i],
			&rgConf[i],
			iCharPos
			);
	}

	int rgResult[_CHAR_TRANSMODE_COUNT] = {0};
	int rgResultConf[_CHAR_TRANSMODE_COUNT] ={0};
	int nResultCount = 0;
	int nTempPos = 0;

	for(int i = 0;i < nTransCount; i++)
	{
		nTempPos = -1;
		for(int j = 0; j < nResultCount; j++)
		{
			if(rgResult[j] == rgIdx[i])
			{
				nTempPos = j;
				break;
			}
		}

		if(nTempPos >= 0)
		{
			rgResultConf[nTempPos] += rgConf[i];
		}
		else
		{
			rgResult[nResultCount] = rgIdx[i];
			rgResultConf[nResultCount] = rgConf[i];
			nResultCount++;
		}
	}

	int nMaxConf = -1;
	int nIdx = 0;

	for(int i = 0; i < nResultCount; i++)
	{
		if(nMaxConf < rgResultConf[i])
		{
			nMaxConf = rgResultConf[i];
			nIdx = rgResult[i];
		}
	}

	nMaxConf = -1;

	for(int i = 0; i < nTransCount; i++)
	{
		if(nIdx == rgIdx[i] && nMaxConf < rgConf[i])
		{
			nMaxConf = rgConf[i];
		}
	}

	if(pbResult) *pbResult = nIdx;
	if(pdwProb) *pdwProb = nMaxConf;

	return S_OK;
}

HRESULT CTrackerImpl_Photo::RecognizePlate(
									 HV_COMPONENT_IMAGE imgPlate,
									 PlateInfo &Info,
									 CBinaryInfo &BinaryInfo,
									 int nFiveCharWidth
									 )
{
	if ( (imgPlate.iHeight <= MIN_SEG_IMG_HIGH) ||
		(imgPlate.iWidth <= MIN_SEG_IMG_WIDTH) )
	{
		return S_FALSE;
	}

	SystemDog();

	int iLastCharHeight = -1;
	int iMaxScore = 0;
	int iFastSegMode(2);
	CRect rgrcCharPos[knPlateStrLen * 5];
	HV_RECT *pCharPos=rgrcCharPos;

	RTN_HR_IF_FAILED(m_pProcessPlate->SetWorkMode(NO_PREDICT_PROCESS_PLATE_MODE, FALSE, TRUE));
	RTN_HR_IF_FAILED(m_pProcessPlate->SegmentPlate(
		&imgPlate, &(Info.color), &(Info.nPlateType),
		&iMaxScore, &iLastCharHeight, &(Info.iLastVscale),
		&BinaryInfo, &pCharPos, iFastSegMode,
		FALSE,
		nFiveCharWidth
		));

	// 如果分割失败
	if ( iMaxScore<0 ||
		 Info.nPlateType==PLATE_UNKNOWN )
	{
		for (int i=0; i<knPlateStrLen; i++)
		{
			Info.rgbContent[i] = 0;
			Info.rgfltConfidence[i] = 0.0f;
		}
		Info.fltTotalConfidence = 0.0f;

		return S_FALSE;
	}

	Info.iLastCharHeight = iLastCharHeight;

	//将双军牌、个性牌颜色设置为白色
	if (Info.nPlateType == PLATE_INDIVIDUAL) Info.color = PC_WHITE;

	CRect rcPlatePos(imgPlate.iWidth+1, imgPlate.iHeight+1, -1, -1);
	Info.fltTotalConfidence = 1.0f;

	//字符识别
	for (int i=0; i<knPlateStrLen; i++)
	{
		if (rcPlatePos.left>rgrcCharPos[i].left)
		{
			rcPlatePos.left=rgrcCharPos[i].left;
		}
		if (rcPlatePos.top>rgrcCharPos[i].top)
		{
			rcPlatePos.top=rgrcCharPos[i].top;
		}
		if (rcPlatePos.right<rgrcCharPos[i].right)
		{
			rcPlatePos.right=rgrcCharPos[i].right;
		}
		if (rcPlatePos.bottom<rgrcCharPos[i].bottom)
		{
			rcPlatePos.bottom=rgrcCharPos[i].bottom;
		}

		HV_COMPONENT_IMAGE imgChar;
		imgChar.nImgType = HV_IMAGE_GRAY;

		SetHvImageData(&imgChar, 0, GetHvImageData(&imgPlate, 0)+rgrcCharPos[i].left+
			rgrcCharPos[i].top*imgPlate.iStrideWidth[0]);
		imgChar.iWidth=rgrcCharPos[i].Width();
		imgChar.iHeight=rgrcCharPos[i].Height();
		imgChar.iStrideWidth[0]=imgPlate.iStrideWidth[0];

		SDWORD32 swProb;
		bool fRecogChar = true;		// 识别字符开关

		RECOGNITION_TYPE rtToRecog = (RECOGNITION_TYPE)Plate::g_cnType[Info.nPlateType][i];

		// 建立补充 前处理规则
		switch(Info.nPlateType)
		{
		case PLATE_NORMAL:			// 蓝黑牌
			if ( 6 == i )
			{
				if ((Info.rgbContent[0] == 57 ) && (Info.rgbContent[1] == 36))
				{
					//增加对港澳牌的支持,港澳牌为"粤Z-****港"或"粤Z-****澳"
					rtToRecog = RECOG_GA;
				}
			}
			break;
		case PLATE_POLICE:							// 黄牌、广西警牌和白牌
			if ( 4 == i )
			{
				// 字母开关的支持
				rtToRecog = m_cfgParam.fAlpha5?RECOG_ALL:RECOG_DIGIT;
			}
			else if( 5 == i )
			{
				rtToRecog = m_cfgParam.fAlpha6?RECOG_ALL:RECOG_DIGIT;
			}
			if ((i >= 2) && (i <= 6))
			{
				if (IsMilitaryChn(Info.rgbContent[0]))
				{
					// 首字是军牌汉字
					rtToRecog = RECOG_DIGIT;		// 3~7 位全部使用数字识别
					break;
				}
				// 对京V 类型军牌的支?
				if ((Info.rgbContent[0] == 38) && (Info.rgbContent[1] == 32))
				{
					rtToRecog = RECOG_DIGIT;		// 3~7 位全部使用数字识别
				}
			}
			break;
		case PLATE_DOUBLE_YELLOW:					// 双层牌
		case PLATE_DOUBLE_MOTO:
			if ( 4 == i )
			{
				// 字母开关的支持
				rtToRecog = m_cfgParam.fAlpha5?RECOG_ALLMT:RECOG_DIGIT;
			}
			else if( 5 == i )
			{
				rtToRecog = m_cfgParam.fAlpha6?RECOG_ALLMT:RECOG_DIGIT;
			}
			else if( 6 == i && m_cfgParam.fAlpha7)
			{
				rtToRecog = RECOG_ALLMT;
			}
			if ((i >= 2) && (i <= 6))
			{
				if (IsMilitaryChn(Info.rgbContent[0]))
				{
					// 首字是军牌汉字
					rtToRecog = RECOG_DIGIT;		// 3~7 位全部使用数字识别
					break;
				}
				// 对京V 类型军牌的支持
				if ((Info.rgbContent[0] == 38) && (Info.rgbContent[1] == 32))
				{
					rtToRecog = RECOG_DIGIT;		// 3~7 位全部使用数字识别
				}
			}
			break;
		case PLATE_INDIVIDUAL:		// 个性化车牌
			if (i == 0)
			{
				//个性牌 i==0 时识别第二位的字母
				rtToRecog = RECOG_ALPHA;
			}
			break;
		case PLATE_WJ:				// 武警车牌
			if ( m_fEnableNewWJSegment && (2 == i || 3 == i))
			{
				// 新武警车牌支持
				rtToRecog = RECOG_ALL;
			}
			break;
		case PLATE_POLICE2:
			if (i ==6)
			{
				// 第7位不用识别
				Info.rgbContent[6] = 37;			// 强制设置成警字
				fRecogChar = false;					// 不识别
				swProb = 65535;						// 可信度
			}
			break;
		default:
			break;
		}

		int nRecogCount(0);
		while(fRecogChar)
		{
			RecognizeChar(
				imgPlate,
				rgrcCharPos[i],
				rtToRecog,
				Info,
				Info.rgbContent + i,
				&swProb,
				i
				);

			fRecogChar = false;
			nRecogCount++;

			if (nRecogCount > 1)
			{
				break;					// 第二次识别直接退出
			}

			// 需要二次识别的规则
			if ((PLATE_POLICE == Info.nPlateType) && (i == 6))
			{
				if (Info.rgbContent[6] == 37)
				{
					if (!((m_iRecogGxPolice == 1) && (Info.rgbContent[0] == 58)))
					{
						// 使用数字模型重新识别
						rtToRecog = RECOG_DIGIT;
						fRecogChar = true;
					}
				}
			} // if
		}
		// 缺省武警替换(和新规则无关)
		if (m_fEnableNewWJSegment && m_fEnableDefaultWJ
			&& PLATE_WJ == Info.nPlateType && strlen(m_strDefaultWJChar) > 1)
		{
			if (0 == i)
			{
				*(Info.rgbContent) = m_strDefaultWJChar[0] - 47;
			}
			if (1 == i)
			{
				*(Info.rgbContent + 1) = m_strDefaultWJChar[1] - 47;
			}
		}

		//识别双层牌时，对E误识别成AB时进行再次识别(和新规则无关)
		if (Info.nPlateType == PLATE_DOUBLE_YELLOW && i == 1 &&
			( Info.rgbContent[1] == 11 || Info.rgbContent[1] == 12 )
			&& imgChar.iHeight >= 5
			)
		{
			BYTE8 bContentTemp(Info.rgbContent[i]);
			SDWORD32 swProbTemp(swProb);

			imgChar.iHeight--;

			RecognizeChar(
				imgPlate,
				rgrcCharPos[i],
				rtToRecog,
				Info,
				Info.rgbContent + i,
				&swProb,
				i
				);

			if (Info.rgbContent[i] != 15)
			{
				Info.rgbContent[i] = bContentTemp;
				swProb = swProbTemp;
			}
		}

		Info.rgfltConfidence[i]=(float)swProb/(1<<QUANTIFY_DIGIT);

		// WJ格式不用中间的字，而是用第一个字
		if ( (Info.nPlateType == PLATE_WJ && i != 2) ||
			(Info.nPlateType != PLATE_WJ && i > 0) )
		{
			Info.fltTotalConfidence*=Info.rgfltConfidence[i];
		}

		if(Info.fltTotalConfidence < m_pParam->g_fltThreshold_BeObservation) return S_FALSE;
	}

	int nCharOneCount = 0;
	for(int i = 2; i < knPlateStrLen; i++)
	{
		if( Info.rgbContent[i] == 2 || //1
			Info.rgbContent[i] == 5 ||	//4
			Info.rgbContent[i] == 18 || //H
			Info.rgbContent[i] == 31 ) //U
		{
			nCharOneCount++;
		}
	}

	if(nCharOneCount >=4)
	{
		float fltNewTotalConf = 1.0;
		for(int i = 1; i < knPlateStrLen; i++)
		{
			Info.rgfltConfidence[i] = Info.rgfltConfidence[i].ToFloat() * 0.8f;
			fltNewTotalConf *= Info.rgfltConfidence[i].ToFloat();
		}

		Info.fltTotalConfidence = fltNewTotalConf;
	}

	if (Info.rgbContent[0] >= 103 && Info.rgbContent[0] <= 113
		&& (37 == Info.rgbContent[6] || PLATE_NORMAL == Info.nPlateType) )
	{
		Info.rgbContent[0] = m_nDefaultChnId;
	}

	Info.rcPlatePos = rcPlatePos;

	rcPlatePos.InflateRect(rcPlatePos.Width()/8, rcPlatePos.Height()/4);
	if (rcPlatePos.IntersectRect(rcPlatePos, CRect(0, 0, imgPlate.iWidth, imgPlate.iHeight)))
	{
		// 调整分割后坐标
		Info.rcPlatePos.OffsetRect(-rcPlatePos.left, -rcPlatePos.top);
	}

	//复制字符位置
	for(int i=0; i < knPlateStrLen; i++)
	{
		Info.rgrcChar[i] = rgrcCharPos[i];
	}

	return S_OK;
}

HRESULT DownSampleYUV422(HV_COMPONENT_IMAGE imgSrc, HV_COMPONENT_IMAGE imgDst)
{
	if (imgSrc.nImgType != HV_IMAGE_YUV_422 || imgDst.nImgType != HV_IMAGE_YUV_422)
	{
		return E_INVALIDARG;
	}
	int i, j;
	int iBlockNum = imgDst.iWidth >> 1;
	int iStepW = imgSrc.iWidth / iBlockNum;
	int iStepW2 = iStepW >> 1;
	int iStepH = imgSrc.iHeight / imgDst.iHeight;
	PBYTE8 pbSrcY = GetHvImageData(&imgSrc, 0);
	PBYTE8 pbSrcU = GetHvImageData(&imgSrc, 1);
	PBYTE8 pbSrcV = GetHvImageData(&imgSrc, 2);
	PBYTE8 pbDstY = GetHvImageData(&imgDst, 0);
	PBYTE8 pbDstU = GetHvImageData(&imgDst, 1);
	PBYTE8 pbDstV = GetHvImageData(&imgDst, 2);
	PBYTE8 pbSrcYTemp, pbSrcUTemp, pbSrcVTemp, pbDstYTemp, pbDstUTemp, pbDstVTemp;
	for (i = 0; i < imgDst.iHeight; i++)
	{
		pbSrcYTemp = pbSrcY; pbSrcUTemp = pbSrcU; pbSrcVTemp = pbSrcV;
		pbDstYTemp = pbDstY; pbDstUTemp = pbDstU; pbDstVTemp = pbDstV;
		for (j = 0; j < iBlockNum; j++)
		{
			*pbDstYTemp = *pbSrcYTemp;
			*(pbDstYTemp + 1) = *(pbSrcYTemp + 1);
			*pbDstUTemp = *pbSrcUTemp;
			*pbDstVTemp = *pbSrcVTemp;
			pbSrcYTemp += iStepW; pbSrcUTemp += iStepW2; pbSrcVTemp += iStepW2;
			pbDstYTemp += 2; pbDstUTemp++;	pbDstVTemp++;
		}
		pbSrcY += imgSrc.iStrideWidth[0] * iStepH;
		pbSrcU += imgSrc.iStrideWidth[1] * iStepH;
		pbSrcV += imgSrc.iStrideWidth[2] * iStepH;
		pbDstY += imgDst.iStrideWidth[0];
		pbDstU += imgDst.iStrideWidth[1];
		pbDstV += imgDst.iStrideWidth[2];
	}
	return S_OK;
}

/*HRESULT CheckColour(HV_COMPONENT_IMAGE imgPlateSelected,
					PlateInfo &Info,
					BOOL fIsNight,
					int fYellowPlate = 0)
{
	CFastMemAlloc FastStack;
	BYTE8 *pbHsb = (BYTE8 *)FastStack.StackAlloc(imgPlateSelected.iHeight * imgPlateSelected.iWidth * 3);
	ConvertYCbCr2HSB(imgPlateSelected.iWidth * 3, pbHsb, &imgPlateSelected, 0, 0, imgPlateSelected.iHeight, imgPlateSelected.iWidth);

	int iColour[CC_COUNT] = {0};
	int iColourTemp[CC_COUNT] = {0};
	int iImgSize = imgPlateSelected.iHeight * imgPlateSelected.iWidth * 3;
	for (int i = 0; i < iImgSize; i += 3)
	{
		if (pbHsb[i] <= 5 || pbHsb[i] > 170)//红
		{
			if (pbHsb[i + 2] > 220 ||
				(pbHsb[i + 2] > 200 && pbHsb[i + 1] < 100))
			{
				iColour[CC_WHITE]++;
			}
			else if (pbHsb[i + 2] > 150 && pbHsb[i + 1] < 30)
			{
				iColour[CC_GREY]++;
			}
			else if (pbHsb[i + 2] < 50 && pbHsb[i + 1] < 50)
			{
				iColour[CC_BLACK]++;
			}
			else if (pbHsb[i + 2] < 100 ||
				(pbHsb[i + 1] < 60 && pbHsb[i + 2] < 140))
			{
				iColour[CC_RED]++;
			}
			else
			{
				iColour[CC_LIGHTRED]++;
			}
		}
		else if (pbHsb[i] > 5 && pbHsb[i] <= 40)//黄
		{
			if (pbHsb[i + 2] > 220 ||
				(pbHsb[i + 2] > 200 && pbHsb[i + 1] < 100))
			{
				iColour[CC_WHITE]++;
			}
			else if (pbHsb[i + 2] > 110 && pbHsb[i + 1] < 30)
			{
				iColour[CC_GREY]++;
			}
			else if (pbHsb[i + 2] < 50 && pbHsb[i + 1] < 50)
			{
				iColour[CC_BLACK]++;
			}
			else if (pbHsb[i] <= 25)
			{
				iColour[CC_YELLOW]++;
			}
			else
			{
				iColour[CC_LIGHTYELLOW]++;
			}
		}
		else if (pbHsb[i] > 40 && pbHsb[i] <= 110)//绿
		{
			if (pbHsb[i + 2] > 220 ||
				(pbHsb[i + 2] > 200 && pbHsb[i + 1] < 100))
			{
				iColour[CC_WHITE]++;
			}
			else if (pbHsb[i + 2] > 100 && pbHsb[i + 1] < 30)
			{
				iColour[CC_GREY]++;
			}
			else if (pbHsb[i + 2] < 50 && pbHsb[i + 1] < 50)
			{
				iColour[CC_BLACK]++;
			}
			else if (pbHsb[i + 2] < 100 ||
				(pbHsb[i + 1] < 60 && pbHsb[i + 2] < 140))
			{
				iColour[CC_GREEN]++;
			}
			else
			{
				iColour[CC_LIGHTGREEN]++;
			}
		}
		else//(pbHsb[i] > 110 && pbHsb[i] <= 170) //蓝
		{
			if (pbHsb[i + 2] > 220 ||
				(pbHsb[i + 2] > 200 && pbHsb[i + 1] < 100))
			{
				iColour[CC_WHITE]++;
			}
			else if (pbHsb[i] < 130)
			{
				if (pbHsb[i + 2] > 100 && pbHsb[i + 1] < 30)
				{
					iColour[CC_GREY]++;
				}
				else if (pbHsb[i + 2] < 50 && pbHsb[i + 1] < 50)
				{
					iColour[CC_BLACK]++;
				}
				else if (pbHsb[i + 2] < 100 ||
				(pbHsb[i + 1] < 60 && pbHsb[i + 2] < 140))
				{
					iColour[CC_BLUE]++;
				}
				else
				{
					iColour[CC_LIGHTBLUE]++;
				}
			}
			else if ((pbHsb[i + 1] < 70 && pbHsb[i + 2] > 120) ||
				(pbHsb[i + 1] < 50 && pbHsb[i + 2] > 100))
			{
				iColour[CC_GREY]++;
			}
			else if ((pbHsb[i + 1] < 70 && pbHsb[i + 2] < 60) ||
				(pbHsb[i + 1] < 40 && pbHsb[i + 2] < 90) ||
				pbHsb[i + 2] < 30)
			{
				iColour[CC_BLACK]++;
			}
			else if (pbHsb[i + 2] < 120 ||
				(pbHsb[i + 1] < 60 && pbHsb[i + 2] < 140))
			{
				iColour[CC_BLUE]++;
			}
			else
			{
				iColour[CC_LIGHTBLUE]++;
			}
		}
	}

	int iIndexC(0), iMaxC(0), iIndexC2(0), iMaxC2(0);
	iImgSize = imgPlateSelected.iHeight * imgPlateSelected.iWidth;

	memcpy(iColourTemp, iColour, sizeof(int) * CC_COUNT);
	iColourTemp[CC_RED] += iColourTemp[CC_LIGHTRED];
	iColourTemp[CC_LIGHTRED] = 0;
	iColourTemp[CC_YELLOW] += iColourTemp[CC_LIGHTYELLOW];
	iColourTemp[CC_LIGHTYELLOW] = 0;
	iColourTemp[CC_GREEN] += iColourTemp[CC_LIGHTGREEN];
	iColourTemp[CC_LIGHTGREEN] = 0;
	iColourTemp[CC_BLUE] += iColourTemp[CC_LIGHTBLUE];
	iColourTemp[CC_LIGHTBLUE] = 0;

	for (int i = 1; i < CC_COUNT; i++)
	{
		iColourTemp[i] = iColourTemp[i] * 100 / iImgSize;
		if (iColourTemp[i] > iMaxC)
		{
			iMaxC2 = iMaxC;
			iIndexC2 = iIndexC;
			iMaxC = iColourTemp[i];
			iIndexC = i;
		}
		else if (iColourTemp[i] > iMaxC2)
		{
			iMaxC2 = iColourTemp[i];
			iIndexC2 = i;
		}
	}

	Info.nCarColour = (CAR_COLOR)iIndexC;
	if (fYellowPlate && CC_BLACK == Info.nCarColour && iMaxC2 > (iMaxC >> 1))
	{
		Info.nCarColour = (CAR_COLOR)iIndexC2;
	}
	if (CC_RED == Info.nCarColour ||
        CC_YELLOW == Info.nCarColour ||
		CC_GREEN == Info.nCarColour ||
		CC_BLUE == Info.nCarColour)
	{
		if (iColour[Info.nCarColour - 1] > iColour[Info.nCarColour])
		{
			Info.nCarColour = (CAR_COLOR)(Info.nCarColour - 1);
		}
	}

	return S_OK;
}*/

HRESULT CTrackerImpl_Photo::CalcCarSize(HV_COMPONENT_IMAGE *pImgCurFrame, OUTPUT_CAR_SIZE_INFO *pCarSize, CAR_TYPE nCarType)
{
	int nCarLenTop = 260;
	int nCarLenBottom = 680;

	int nCarWTop = 196;
	int nCarWBottom = 570;

	if (m_nWidth == 1920)
	{
		nCarLenTop = 266;
		nCarLenBottom = 534;

		nCarWTop = 202;
		nCarWBottom = 460;
	}
	else if (m_nWidth == 1600)
	{
		// 抓拍图1600 * 1200
		nCarLenTop = 420;
		nCarLenBottom = 690;

		nCarWTop = 320;
		nCarWBottom = 635;
	}

	// 没有视频测速，使用默认的经验值参数计算斜率
	//	float fltSlopeH =  (float)(nCarLenBottom - nCarLenTop) / (float)(m_nHeight * 2 - nCarLenTop);
	//	float fltSlopeW =  (float)(nCarWBottom - nCarWTop) / (float)(m_nHeight * 2);

	pCarSize->nOutType = 0;

	if ((pCarSize->iCarWidth < nCarWBottom) 
		|| (pCarSize->iCarHeight < nCarLenBottom))
	{
		float fltTempS = 1.1f;
		float fltRand = (float)(rand() % 11) / 100.0f;
		fltTempS += fltRand;

		if (nCarType == CT_LARGE)
		{
			fltTempS *= 1.75f;
		}
		else if (nCarType == CT_MID)
		{
			fltTempS *= 1.2f;
		}

		pCarSize->iCarWidth = nCarWBottom * fltTempS;
		pCarSize->iCarHeight = nCarLenBottom * fltTempS;
	}

	return S_OK;
}
HRESULT CTrackerImpl_Photo::RecognizeColorArea(
	const HV_COMPONENT_IMAGE imgInput, 
	HV_RECT rcCrop, 
	BYTE8 *cTempAdd, 
	BYTE8 *piResult,
	int iCutLeng,
	LIGHT_TYPE m_PlateLightType
	)
{
	HV_COMPONENT_IMAGE imgCutArea;

	CPersistentComponentImage CTempImg;
	CTempImg.Create(HV_IMAGE_BGR,iCutLeng + 2, iCutLeng + 2);

	if(GetHvImageData(&CTempImg, 0) == NULL)
	{
		*piResult = 0;
		return E_POINTER;
	}

	if(rcCrop.right - rcCrop.left <= 0 ||  rcCrop.bottom - rcCrop.top <= 0
		|| rcCrop.left < 0 || rcCrop.right >= imgInput.iWidth || rcCrop.top < 0 || rcCrop.bottom >= imgInput.iHeight
		//	|| rcCrop.right - rcCrop.left > iCutLeng || rcCrop.bottom - rcCrop.top > iCutLeng
		|| rcCrop.left >= imgInput.iWidth || rcCrop.top >= imgInput.iHeight 
		|| rcCrop.right < 0 || rcCrop.bottom < 0)
	{
		*piResult = 0;
		return S_OK;
	}

	RTN_HR_IF_FAILED(CropImage(imgInput, rcCrop, &imgCutArea));

	if(SUCCEEDED(ConvertYCbCr2BGR(CTempImg.iStrideWidth[0], GetHvImageData(&CTempImg, 0), &imgCutArea)))
	{
		HV_COMPONENT_IMAGE pInput;
		pInput.iHeight = 1;
		pInput.iWidth  = 7;
		SDWORD32 idwProb = 0;
		BYTE8 cResult = 0;
		int iTempAdd[4] = {0};
		int iTempCut = imgCutArea.iHeight * imgCutArea.iWidth;
		BYTE8 *pcRBG = GetHvImageData(&CTempImg, 0);
		for(int i = 0; i < imgCutArea.iHeight; i++, pcRBG += CTempImg.iStrideWidth[0])
		{
			BYTE8 *pbRGBTemp = pcRBG;				
			for(int j = 0; j < imgCutArea.iWidth; j++, pbRGBTemp += 3)
			{
				iTempAdd[1] += pbRGBTemp[0];
				iTempAdd[2] += pbRGBTemp[1];
				iTempAdd[3] += pbRGBTemp[2];
			}
		}

		cTempAdd[3] = iTempAdd[1] / iTempCut;		// B
		cTempAdd[2] = iTempAdd[2] / iTempCut;		// G
		cTempAdd[1] = iTempAdd[3] / iTempCut;		// R
		BGR2HSL(cTempAdd[3],cTempAdd[2], cTempAdd[1], &cTempAdd[4], &cTempAdd[5], &cTempAdd[6]);
		SetHvImageData(&pInput, 0, cTempAdd);
		//m_PlateLightType<=25用白天模型识别，否则用晚上模型识别
		m_pRecognition->EvaluateColor(&pInput, &cResult, &idwProb, m_PlateLightType);
		CAR_COLOR nCarColor = (CAR_COLOR)GetCarColorType(cResult);

		//颜色修正
		if(nCarColor == CC_BLUE)
		{
			if(cTempAdd[4] >=115 && cTempAdd[4] <= 130 && cTempAdd[6] >= 190)
			{
				nCarColor = CC_WHITE;
			}
			else if((cTempAdd[4] >=120 && cTempAdd[4] <= 135 && (cTempAdd[5]+cTempAdd[6]) < 120)
				||(cTempAdd[4] >=120 && cTempAdd[4] <= 125 && cTempAdd[6] <= 60 && cTempAdd[5]<=130)
				||(cTempAdd[4]<120 && cTempAdd[6]<150))
			{
				nCarColor = CC_GREEN;
			}
			else if(cTempAdd[4] >=140 && cTempAdd[4] <= 150 && cTempAdd[5] <= 65 && cTempAdd[6] <= 45)
			{
				nCarColor = CC_BLACK;
			}
		}
		if(nCarColor == CC_GREY )
		{
			if((cTempAdd[4] >=125 && cTempAdd[4] <= 140 && cTempAdd[6] >= 180) ||
				(cTempAdd[4] >=50 && cTempAdd[4] <= 140 && cTempAdd[5]<=10 && cTempAdd[6]>= 170))
			{
				nCarColor = CC_WHITE;
			}
			if(cTempAdd[4] >=140 && cTempAdd[4] <= 160 && cTempAdd[5]<=35 && cTempAdd[6]<=50)
			{
				nCarColor = CC_BLACK;
			}
		}
		if(nCarColor == CC_YELLOW && cTempAdd[5] < 30)
		{
			nCarColor = CC_GREY;
		}
		if(nCarColor == CC_RED)
		{
			if(cTempAdd[6] < 10 ||(cTempAdd[6] < 20 && cTempAdd[5]<120))
			{
				nCarColor = CC_BLACK;
			}
			else if((cTempAdd[4] > 20&& cTempAdd[4] < 30 && (cTempAdd[5] +cTempAdd[6]) > 200)|| (cTempAdd[4] >12 && cTempAdd[4]<=20 && cTempAdd[6]<=40))
			{
				nCarColor = CC_BROWN;
			}		
		}

		*piResult = nCarColor;
	}
	else
	{
		*piResult = 0;		// 未知结果置0
	}

	return S_OK;
}

HRESULT CTrackerImpl_Photo::RecognizeCarColourPlus(
	HV_COMPONENT_IMAGE imgPlateOri,
	PlateInfo &Info,
	int iAvgGray,
	BOOL fIsNight
	)
{
	Info.iCarColor = 0;
	if (Info.nPlateType == PLATE_DOUBLE_MOTO || Info.nCarType == CT_BIKE)
	{
		Info.nCarColour = CC_UNKNOWN;
		return S_OK;
	}

	if(imgPlateOri.iHeight < 200 || imgPlateOri.iWidth < 200)			
	{
		Info.nCarColour = CC_UNKNOWN;
		return S_OK;
	}

	//识别结果为警牌的车身颜色直接赋值为白色
	if(Info.rgbContent[6] == 37)
	{
		Info.nCarColour = CC_WHITE;
		Info.iCarColor = 1;
		return S_OK;
	}


	HV_COMPONENT_IMAGE imgCutOri;
	LIGHT_TYPE m_PlateLightType = DAY;
	BYTE8 cTempAdd[7];	
	CRect rcCharPos; 

	bool fIsSmallCar = (Info.color != PC_YELLOW && Info.rgbContent[6] != 114 && Info.rgbContent[6] != 115);
	int iWidth = Info.rgrcChar[6].right - Info.rgrcChar[0].left;
	int iHeight = Info.rcPos.bottom - Info.rcPos.top;   //车牌高度
	float fAdjTop = 3;             //高度调整系数
	CRect rcPositionPos;
	if(fIsSmallCar)  
	{
		rcPositionPos.top = (long)MAX2(Info.rcPos.top - iHeight*3,0);
		rcPositionPos.bottom = (long)MAX2(Info.rcPos.top - iHeight,0);
		rcPositionPos.left = (long)MAX2(Info.rcPos.left - iWidth/2,0);
		rcPositionPos.right = (long)MIN2(Info.rcPos.right + iWidth/2,imgPlateOri.iWidth-1);
	}
	else 
	{
		rcPositionPos.top = (long)MAX2(Info.rcPos.top - iHeight*4,0);
		rcPositionPos.bottom = Info.rcPos.top;
		rcPositionPos.left = (long)MAX2(Info.rcPos.left - iWidth*1.8,0);
		rcPositionPos.right = Info.rcPos.right;
	}
	CropImage(imgPlateOri,rcPositionPos,&imgCutOri);
	cTempAdd[0] = BYTE8(CaclAvgGray(imgCutOri, 2));	

	int DiffY = cTempAdd[0] - iAvgGray;  
	//港澳牌、车身区域亮度较小——fIsSmallCar为真
	if((Info.rgbContent[6] == 114 || Info.rgbContent[6] == 115) && cTempAdd[0] < 50)
	{
		fIsSmallCar = true;
	}

	//单层牌
	if (Info.nPlateType != PLATE_DOUBLE_YELLOW)  
	{	
		rcCharPos.top = Info.rcPos.top + Info.rgrcChar[0].top;
		rcCharPos.left = Info.rcPos.left + Info.rgrcChar[0].left;
		rcCharPos.bottom = Info.rcPos.top + Info.rgrcChar[0].bottom;
		rcCharPos.right = Info.rcPos.left + Info.rgrcChar[6].right;
	}
	//双层牌
	else
	{
		rcCharPos.bottom = Info.rcPos.top + Info.rgrcChar[3].bottom;
		rcCharPos.right = Info.rcPos.left + Info.rgrcChar[6].right;
		rcCharPos.top = Info.rcPos.top + Info.rgrcChar[3].top;
		rcCharPos.left = Info.rcPos.left + Info.rgrcChar[2].left;	
	}
	int iPlateH = rcCharPos.bottom - rcCharPos.top + 1;               //车牌高度
	int iPlateW = rcCharPos.right - rcCharPos.left + 1;		           //车牌宽度
	int iPlateMid = int((rcCharPos.right + rcCharPos.left) * 0.5);     //车牌宽度中点坐标

	int iCutLeng = iPlateH;
	if(iCutLeng > 14)  iCutLeng = 14;
	if(iCutLeng < 8)	iCutLeng = 8;
	if(iPlateH < 15) iPlateH =15;
	if(fIsSmallCar && (Info.rgbContent[6] == 114 || Info.rgbContent[6] == 115))
	{
		fAdjTop = 5;
	}

	CRect rcCutArea[YELLOW_NUM+4];            //车身颜色截取区域
	BYTE8 rgResult[2][YELLOW_NUM] = {0};   //车身颜色识别结果
	BYTE8 rgCutAreaL[2][YELLOW_NUM] = {0}; 
	int iPlateColor,iPlateColor2;
	int rgPlateColor[2];            //投票后的车身颜色识别结果
	int rgPlateColorProb[2] = {0};	// 可信度

	IReferenceComponentImage* pTmpImg = NULL;		

	CreateReferenceComponentImage(
		&pTmpImg,
		HV_IMAGE_BGR,
		iCutLeng + 1,
		iCutLeng + 1
		);


	// 小车（车牌颜色：非黄色）
	if(fIsSmallCar)
	{
		rcCutArea[0].top = (long)MAX2((Info.rcPos.top - fAdjTop*iPlateH),0);
		rcCutArea[0].left  = iPlateMid - iCutLeng/2;

		rcCutArea[1].top = rcCutArea[0].top;
		rcCutArea[1].left  = (long)MAX2((iPlateMid - iPlateW),0);
		rcCutArea[2].top = rcCutArea[0].top;
		rcCutArea[2].left  = (long)MIN2((iPlateMid + iPlateW), imgPlateOri.iWidth - iCutLeng - 1);

		rcCutArea[11].top = (long)MAX2((rcCutArea[0].top - iPlateH),0);
		rcCutArea[11].left  = rcCutArea[0].left;
		rcCutArea[3].top = (long)MAX2((rcCutArea[0].top -iPlateH),0);
		rcCutArea[3].left  = (long)MAX2((iPlateMid - iPlateW/2),0);
		rcCutArea[4].top = rcCutArea[3].top;
		rcCutArea[4].left  = (long)MAX2((rcCutArea[3].left  - iPlateW),0);

		rcCutArea[5].top = rcCutArea[3].top;
		rcCutArea[5].left  = (long)MIN2((iPlateMid + iPlateW/2), imgPlateOri.iWidth - iCutLeng - 1);
		rcCutArea[6].top = rcCutArea[3].top;
		rcCutArea[6].left  = (long)MIN2((rcCutArea[5].left  + iPlateW), imgPlateOri.iWidth - iCutLeng - 1);
		//
		rcCutArea[7].top = (long)MAX2((rcCutArea[0].top - 2*iPlateH),0);
		rcCutArea[7].left  = (long)MAX2((iPlateMid - iPlateW),0);
		rcCutArea[8].top = rcCutArea[7].top;
		rcCutArea[8].left  = (long)MAX2((iPlateMid  - 0.3*iPlateW),0);
		rcCutArea[9].top = rcCutArea[7].top;
		rcCutArea[9].left  = (long)MIN2((iPlateMid + 0.3*iPlateW), imgPlateOri.iWidth - iCutLeng - 1);
		rcCutArea[10].top = rcCutArea[7].top;
		rcCutArea[10].left  = (long)MIN2((iPlateMid  + iPlateW), imgPlateOri.iWidth - iCutLeng - 1);
		for(int i=0;i<NORMAL_NUM;i++)
		{
			rcCutArea[i].bottom = rcCutArea[i].top + iCutLeng;
			rcCutArea[i].right = rcCutArea[i].left + iCutLeng;
		}
		//车身颜色识别
		for(int i = 0; i < NORMAL_NUM; i++)
		{
			BYTE8 *piResult = &rgResult[0][i];
			RecognizeColorArea(imgPlateOri, rcCutArea[i], cTempAdd, piResult,iCutLeng, m_PlateLightType);
			rgCutAreaL[0][i] = cTempAdd[6];
		}
		//统计颜色个数
		int rgCountColor[11] = {0};
		for(int j = 0; j < NORMAL_NUM; j++)
		{
			rgCountColor[rgResult[0][j]]++;
		}
		//各颜色评分
		int nColorScore[11] = {0};
		int nColorScore2[11] = {0};
		for(int j=0;j<11;j++)
		{
			if(j<4)
			{
				nColorScore[j] = rgCountColor[j] * 10;
			}
			else
			{
				if(rgCountColor[j] <= 3)
				{
					nColorScore[j] =  rgCountColor[j] * 18;
				}
				else
				{
					nColorScore[j] =  rgCountColor[j] * 24;
				}
			}		
		}

		//保存分数最大的颜色作为车身颜色的结果
		int nColorResult=0,nSecondResult = 0;;
		for(int i=0;i<11;i++)
		{
			if(nColorScore[nColorResult] < nColorScore[i] || (nColorScore[nColorResult] == nColorScore[i] && i>3 && nColorResult < 4)) 
			{
				nSecondResult = nColorResult;
				nColorResult = i;
			}
			else if(nColorScore[nSecondResult] <= nColorScore[i] && nColorScore[i] > 0) 
			{
				nSecondResult = i;
			}
		}
		//黑色车一般不可能有彩色区域存在
		if(nColorResult == 3 && nSecondResult > 3 && nColorScore[nSecondResult] > 70)
		{
			nColorResult = nSecondResult;
		}

		iPlateColor = nColorResult;
		//深浅色区分
		int iAvgL = 0;
		int nCountL = 0;

		for(int i=0;i<NORMAL_NUM;i++)
		{
			if(rgResult[0][i] == iPlateColor)
			{
				iAvgL += rgCutAreaL[0][i];
				nCountL++;
			}
		}
		iAvgL = iAvgL/nCountL;

		if( iAvgL >=130)
			Info.iCarColor = 1;
		else
			Info.iCarColor = 2;

		rgPlateColor[0] = nColorResult;  //第一个识别结果
		rgPlateColorProb[0] = nColorScore[nColorResult];  //第一个识别结果可信度
		//识别成黑灰的重识别一次（主要是为了提高彩色的识别）
		if(nColorResult == 2 || nColorResult == 3)  
		{
			//向上移动一个高度再识别一次
			for(int i = 0; i < NORMAL_NUM; i++)
			{
				rcCutArea[i].top = MAX2(rcCutArea[i].top - iPlateH, 0);
				rcCutArea[i].bottom = rcCutArea[i].top + iCutLeng;

				BYTE8 *piResult = &rgResult[1][i];
				RecognizeColorArea(imgPlateOri, rcCutArea[i], cTempAdd, piResult,iCutLeng, m_PlateLightType);
				rgCutAreaL[1][i] = cTempAdd[6];
			}
			for(int j=0;j<11;j++)
			{
				rgCountColor[j] = 0;
			}
			//统计颜色个数
			for(int j = 0; j < NORMAL_NUM; j++)
			{
				rgCountColor[rgResult[1][j]]++;
			}
			//各颜色评分
			for(int j=0;j<11;j++)
			{
				if(j<4)
				{
					nColorScore2[j] = rgCountColor[j] * 10;
				}
				else
				{
					if(rgCountColor[j] < 3)
					{
						nColorScore2[j] =  rgCountColor[j] * 18;
					}
					else
					{
						nColorScore2[j] =  rgCountColor[j] * 24;
					}
				}		
			}

			//保存分数最大的颜色作为车身颜色的结果
			nColorResult=0;
			nSecondResult = 0;;
			for(int i=0;i<11;i++)
			{
				if(nColorScore2[nColorResult] < nColorScore2[i] || (nColorScore2[nColorResult] == nColorScore2[i] && i>3 && nColorResult < 4)) 
				{
					nSecondResult = nColorResult;
					nColorResult = i;
				}
				else if(nColorScore2[nSecondResult] <= nColorScore2[i] && nColorScore2[i] > 0) 
				{
					nSecondResult = i;
				}
			}
			//黑色车一般不可能有彩色区域存在
			if(nColorResult == 3 && nSecondResult > 3 && nColorScore2[nSecondResult] > 70)
			{
				nColorResult = nSecondResult;
			}

			rgPlateColor[1] = nColorResult;  //第2个识别结果
			rgPlateColorProb[1] = nColorScore2[nColorResult];  //第2个识别结果可信度
			//两次结果取分数
			if((rgPlateColorProb[0] < rgPlateColorProb[1] && rgPlateColor[1] != 3) || (rgPlateColor[1] > 3 && rgPlateColorProb[1] >= 96))
			{
				iPlateColor = rgPlateColor[1];
			}
		}

		//黑重识别，取车牌两边位置识别
		if(iPlateColor == CC_BLACK && (m_nEnableColorReRecog & 1) && DiffY < m_nBColorReThr) 
		{
			//计算车牌右边一小块区域的车牌亮度
			rcPositionPos.top = Info.rcPos.top;
			rcPositionPos.bottom = Info.rcPos.bottom;
			rcPositionPos.left =Info.rcPos.left + Info.rgrcChar[6].right + 15;
			rcPositionPos.right = (long)MIN2(rcPositionPos.left + iHeight*2,imgPlateOri.iWidth-1);
			CropImage(imgPlateOri,rcPositionPos,&imgCutOri);
			int TempY = BYTE8(CaclAvgGray(imgCutOri, 2));		

			rcCutArea[0].top = Info.rcPos.top;
			rcCutArea[0].left  = Info.rcPos.left;
			rcCutArea[1].top = rcCutArea[0].top;
			rcCutArea[1].left  = Info.rcPos.left-iPlateW/2;
			rcCutArea[2].top = rcCutArea[0].top;
			rcCutArea[2].left  = Info.rcPos.right;
			rcCutArea[3].top = rcCutArea[0].top;
			rcCutArea[3].left  = Info.rcPos.right + iPlateW/2;
			for(int i=0;i<4;i++)
			{
				rcCutArea[i].bottom = rcCutArea[i].top + iCutLeng;
				rcCutArea[i].right = rcCutArea[i].left + iCutLeng;
			}

			for(int i = 0; i < 4; i++)
			{
				BYTE8 *piResult = &rgResult[0][i];
				RecognizeColorArea(imgPlateOri, rcCutArea[i], cTempAdd, piResult,iCutLeng, m_PlateLightType);	
			}

			int rgCountColor[11] = {0};
			for(int j = 0; j < 4; j++)
			{
				rgCountColor[rgResult[0][j]]++;
			}
			for(int j=4;j<=7;j++)
			{
				if(rgCountColor[j] == 4 && j!= 5) //车牌两边4块区域都识别成红绿蓝
				{
					iPlateColor = j;
				}
			}
			if(iPlateColor <=3 && DiffY >= -10 && (rgCountColor[3] <= 2 || TempY >= 30)) 
			{
				iPlateColor = CC_GREY;
			}

		}
		//根据环境亮度和车身颜色设置黑和灰结果
		if(iPlateColor == CC_BLACK && m_fEnableCorlorThr && DiffY > m_nBGColorThr)
		{
			iPlateColor = CC_GREY;
		}
		//白重识别
		if(iPlateColor == CC_WHITE && (m_nEnableColorReRecog & 2) && cTempAdd[0]> m_nWColorReThr)
		{
			//计算车牌右边一小块区域的车牌亮度
			rcPositionPos.top = Info.rcPos.top;
			rcPositionPos.bottom = Info.rcPos.bottom;
			rcPositionPos.left =Info.rcPos.left + Info.rgrcChar[6].right + 15;
			rcPositionPos.right = (long)MIN2(rcPositionPos.left + iHeight*2,imgPlateOri.iWidth-1);
			CropImage(imgPlateOri,rcPositionPos,&imgCutOri);
			int TempY = BYTE8(CaclAvgGray(imgCutOri, 2));		

			rcCutArea[0].top = Info.rcPos.top - 3;
			rcCutArea[0].left  = rcCharPos.left - iPlateW/2;
			rcCutArea[1].top = rcCutArea[0].top - 2;
			rcCutArea[1].left  = rcCharPos.left-iPlateW;
			rcCutArea[2].top = rcCutArea[0].top;
			rcCutArea[2].left  = rcCharPos.right + iPlateW/2;
			rcCutArea[3].top = rcCutArea[1].top;
			rcCutArea[3].left  = rcCharPos.right + iPlateW;
			for(int i=0;i<4;i++)
			{
				rcCutArea[i].bottom = rcCutArea[i].top + 12;
				rcCutArea[i].right = rcCutArea[i].left + 12;
			}

			for(int i = 0; i < 4; i++)
			{
				BYTE8 *piResult = &rgResult[0][i];
				RecognizeColorArea(imgPlateOri, rcCutArea[i], cTempAdd, piResult,iCutLeng, m_PlateLightType);	

			}
			int rgCountColor[11] = {0};
			for(int j = 0; j < 4; j++)
			{
				rgCountColor[rgResult[0][j]]++;
			}

			for(int j=4;j<=7;j++)
			{
				if(rgCountColor[j] == 4 /*&& j!= 5*/) //车牌两边4块区域都识别成红绿蓝
				{
					iPlateColor = j;
				}
			}
			if(iPlateColor <=3 && TempY <= 160 && iAvgL-cTempAdd[0] <=60 
				&& rgCountColor[1] <= 2 && rgCountColor[3] < 4) 
			{
				iPlateColor = CC_GREY;
			}

		}

		if(fIsNight && iPlateColor == CC_GREY && m_fEnableCorlorThr && DiffY > m_nWGColorThr)
		{
			iPlateColor = CC_WHITE;
		}
		//识别成白或灰——修正成黑（收费站场景下），取车牌两边区域识别
		if((iPlateColor == CC_GREY || iPlateColor == CC_WHITE)&& (m_nEnableColorReRecog & 4) && (DiffY < m_nWGColorReThr || iAvgGray < (m_nWGColorReThr+40)))
		{
			//计算车牌右边一小块区域的车牌亮度
			rcPositionPos.top = Info.rcPos.top;
			rcPositionPos.bottom = Info.rcPos.bottom;
			rcPositionPos.left =Info.rcPos.left + Info.rgrcChar[6].right + 15;
			rcPositionPos.right = (long)MIN2(rcPositionPos.left + iHeight*2,imgPlateOri.iWidth-1);
			CropImage(imgPlateOri,rcPositionPos,&imgCutOri);
			int TempY = BYTE8(CaclAvgGray(imgCutOri, 2));

			rcCutArea[0].top = Info.rcPos.top - 3;
			rcCutArea[0].left  = rcCharPos.left - iPlateW/2;
			rcCutArea[1].top = rcCutArea[0].top - 2;
			rcCutArea[1].left  = rcCharPos.left-iPlateW;
			rcCutArea[2].top = rcCutArea[0].top;
			rcCutArea[2].left  = rcCharPos.right + iPlateW/2;
			rcCutArea[3].top = rcCutArea[1].top;
			rcCutArea[3].left  = rcCharPos.right + iPlateW;

			for(int i=0;i<4;i++)
			{
				rcCutArea[i].bottom = rcCutArea[i].top + iCutLeng;
				rcCutArea[i].right = rcCutArea[i].left + iCutLeng;
			}
			for(int i = 0; i < 4; i++)
			{
				BYTE8 *piResult = &rgResult[0][i];
				RecognizeColorArea(imgPlateOri, rcCutArea[i], cTempAdd, piResult,iCutLeng, m_PlateLightType);	
				//收费站下黑识别成灰
				if(*piResult == CC_GREY && cTempAdd[6] < 35 && TempY < 30 && (cTempAdd[0]-cTempAdd[6]) > 120)
				{
					*piResult = CC_BLACK;
				}
			}

			int rgCountColor[11] = {0};
			for(int j = 0; j < 4; j++)
			{
				rgCountColor[rgResult[0][j]]++;
			}

			for(int j=4;j<=7;j++)
			{
				if(rgCountColor[j] >= 3 && j!= 5) //车牌两边4块区域都识别成红绿蓝
				{
					iPlateColor = j;
				}
			}
			if(iPlateColor <=3 && (rgCountColor[3] >= 3 || TempY < 20 || (TempY < 35 && rgCountColor[3] >= 2)))   //车牌两边有3块以上识别成黑
			{
				iPlateColor = CC_BLACK;
			}
			else if(iPlateColor == CC_WHITE && rgCountColor[1] <= 2)
			{
				iPlateColor = CC_GREY;
			}
		}
	}

	else
	{
		rcCutArea[0].top = (long)MAX2((Info.rcPos.top - fAdjTop*iPlateH),0);
		rcCutArea[0].left  = (long)MAX2((iPlateMid - iPlateW),0);

		rcCutArea[1].top = rcCutArea[0].top;
		rcCutArea[1].left  = (long)MIN2((iPlateMid + iPlateW), imgPlateOri.iWidth - iCutLeng - 1);
		//
		rcCutArea[2].top = (long)MAX2((rcCutArea[0].top - iPlateH),0);
		rcCutArea[2].left  = (long)MAX2((iPlateMid - 1.5*iPlateW),0);

		rcCutArea[3].top = rcCutArea[2].top;
		rcCutArea[3].left  = (long)MIN2((iPlateMid + 1.5*iPlateW), imgPlateOri.iWidth - iCutLeng - 1);

		rcCutArea[4].top = rcCutArea[2].top;
		rcCutArea[4].left  = (long)MAX2((rcCutArea[2].left  - iPlateW/2),0);

		rcCutArea[5].top = rcCutArea[2].top;
		rcCutArea[5].left  =  (long)MIN2((rcCutArea[3].left  + iPlateW/2), imgPlateOri.iWidth - iCutLeng - 1);
		//
		rcCutArea[6].top = (long)MAX2((rcCutArea[0].top - 2*iPlateH),0);
		rcCutArea[6].left  = (long)MAX2((iPlateMid - 0.5*iPlateW),0);
		rcCutArea[7].top = rcCutArea[6].top;
		rcCutArea[7].left  = (long)MIN2((iPlateMid + 0.5*iPlateW), imgPlateOri.iWidth - iCutLeng - 1);

		rcCutArea[8].top = rcCutArea[6].top;
		rcCutArea[8].left  = (long)MAX2((rcCutArea[6].left  - iPlateW),0);
		rcCutArea[9].top = rcCutArea[6].top;
		rcCutArea[9].left  = (long)MIN2((rcCutArea[7].left + iPlateW), imgPlateOri.iWidth - iCutLeng - 1);

		rcCutArea[10].top = rcCutArea[6].top;
		rcCutArea[10].left  = (long)MAX2((rcCutArea[8].left  - iPlateW),0);
		rcCutArea[11].top = rcCutArea[6].top;
		rcCutArea[11].left  = (long)MIN2((rcCutArea[9].left  + iPlateW), imgPlateOri.iWidth - iCutLeng - 1);
		//
		rcCutArea[12].top = (long)MAX2((rcCutArea[0].top - 3*iPlateH),0);
		rcCutArea[12].left  = (long)MAX2((iPlateMid - iPlateW),0);
		rcCutArea[13].top = rcCutArea[12].top;
		rcCutArea[13].left  = (long)MIN2((iPlateMid + iPlateW), imgPlateOri.iWidth - iCutLeng - 1);

		rcCutArea[14].top = rcCutArea[12].top;
		rcCutArea[14].left  = (long)MAX2((rcCutArea[12].left  - iPlateW),0);
		rcCutArea[15].top = rcCutArea[12].top;
		rcCutArea[15].left  = (long)MIN2((rcCutArea[13].left  + iPlateW), imgPlateOri.iWidth - iCutLeng - 1);
		//车牌两边区域
		rcCutArea[16].top = Info.rcPos.top;
		rcCutArea[16].left  = (long)MAX2(rcCharPos.left - iPlateW/2,0);
		rcCutArea[17].top = rcCutArea[16].top;
		rcCutArea[17].left  = (long)MAX2(rcCharPos.left-iPlateW,0);
		rcCutArea[18].top = rcCutArea[16].top;
		rcCutArea[18].left  = (long)MIN2(rcCharPos.right + iPlateW/2,imgPlateOri.iWidth - iCutLeng -1);
		rcCutArea[19].top = rcCutArea[16].top;
		rcCutArea[19].left  = (long)MIN2(rcCharPos.right + iPlateW,imgPlateOri.iWidth - iCutLeng -1);

		for(int i=0;i<YELLOW_NUM;i++)
		{
			rcCutArea[i].bottom = rcCutArea[i].top + iCutLeng;
			rcCutArea[i].right = rcCutArea[i].left + iCutLeng;
		}

		for(int i = 0; i < YELLOW_NUM; i++)
		{
			BYTE8 *piResult = &rgResult[0][i];
			RecognizeColorArea(imgPlateOri, rcCutArea[i], cTempAdd, piResult,iCutLeng, m_PlateLightType);
			rgCutAreaL[0][i] = cTempAdd[6];
		}
		//统计颜色个数
		int rgCountColor[11] = {0};
		for(int j = 0; j < YELLOW_NUM; j++)
		{
			rgCountColor[rgResult[0][j]]++;
		}
		//各颜色评分
		int nColorScore[11] = {0};
		int nColorScore2[11] = {0};
		for(int j=0;j<11;j++)
		{	
			if(j==1 || j>3)
			{
				nColorScore[j] = rgCountColor[j] * 30;
			}
			else if(j==2)
			{
				nColorScore[j] = rgCountColor[j] * 12;
			}
			else if(j==3)
			{
				nColorScore[j] = rgCountColor[j] * 8;
			}
		}

		//保存分数最大的颜色作为车身颜色的结果
		int nColorResult=0,nSecondResult = 0;
		for(int i=0;i<11;i++)
		{
			if(nColorScore[nColorResult] < nColorScore[i] || (nColorScore[nColorResult] == nColorScore[i] && i>3 && nColorResult < 4)) 
			{
				nSecondResult = nColorResult;
				nColorResult = i;
			}
			else if(nColorScore[nSecondResult] <= nColorScore[i] && nColorScore[i] > 0) 
			{
				nSecondResult = i;
			}
		}
		//红色车识别成蓝色车修正
		if(nColorResult == 7 && nSecondResult == 4 && nColorScore[nSecondResult] > 120)
		{
			nColorResult = nSecondResult;
		}
		//黑色车一般不可能有彩色
		if(nColorResult == 3 && nSecondResult > 3 && nColorScore[nSecondResult] >= 90)
		{
			nColorResult = nSecondResult;
		}

		iPlateColor = nColorResult;
		rgPlateColor[0] = nColorResult;  //第一个识别结果
		rgPlateColorProb[0] = nColorScore[nColorResult];  //第一个识别结果可信度

		//部分蓝色大车车身较高，截取到车灯部分，晚上容易识别成红——设置成黑，向上一个高度重识别
		if(nColorResult == 4 && nSecondResult == 7 && nColorScore[nColorResult]- nColorScore[nColorResult] <= 60)
		{
			nColorResult = 3;
		}

		//深浅色区分
		int iAvgL = 0;
		int nCountL = 0;

		for(int i=0;i<YELLOW_NUM;i++)
		{
			if(rgResult[0][i] == iPlateColor)
			{
				iAvgL += rgCutAreaL[0][i];
				nCountL++;
			}
		}
		iAvgL = iAvgL/nCountL;

		if(iAvgL >=130)
			Info.iCarColor = 1;
		else
			Info.iCarColor = 2;


		//识别成黑灰的重识别一次
		if(nColorResult == 1 || nColorResult ==2 || nColorResult == 3 || nColorResult == 10)
		{
			//向上移动2个高度再识别一次
			for(int i = 0; i < YELLOW_NUM; i++)
			{
				rcCutArea[i].top = MAX(rcCutArea[i].top - 2 * iCutLeng, 0);
				rcCutArea[i].bottom = rcCutArea[i].top + iCutLeng;

				BYTE8 *piResult = &rgResult[1][i];
				RecognizeColorArea(imgPlateOri, rcCutArea[i], cTempAdd, piResult,iCutLeng, m_PlateLightType);
				rgCutAreaL[1][i] = cTempAdd[6];
			}

			//统计颜色个数
			for(int j=0;j<11;j++)
			{
				rgCountColor[j] = 0;
			}
			for(int j = 0; j < YELLOW_NUM; j++)
			{
				rgCountColor[rgResult[1][j]]++;
			}

			//各颜色评分
			for(int j=0;j<11;j++)
			{	
				if(j==1 || j>3)
				{
					nColorScore2[j] = rgCountColor[j] * 30;
				}
				else if(j==2)
				{
					nColorScore2[j] = rgCountColor[j] * 10;
				}
				else if(j==3)
				{
					nColorScore2[j] = rgCountColor[j] * 5;
				}
			}

			//保存分数最大的颜色作为车身颜色的结果
			nColorResult=0;
			nSecondResult = 0;;
			for(int i=0;i<11;i++)
			{
				if(nColorScore2[nColorResult] < nColorScore2[i] || (nColorScore2[nColorResult] == nColorScore2[i] && i>3 && nColorResult < 4)) 
				{
					nSecondResult = nColorResult;
					nColorResult = i;
				}
				else if(nColorScore2[nSecondResult] <= nColorScore2[i] && nColorScore2[i] > 0) 
				{
					nSecondResult = i;
				}
			}
			//黑色车一般不可能有彩色区域存在
			if(nColorResult == 3 && (nSecondResult > 3 && nColorScore2[nSecondResult] > 60))
			{
				nColorResult = nSecondResult;
			}

			rgPlateColor[1] = nColorResult;  //第2个识别结果
			rgPlateColorProb[1] = nColorScore2[nColorResult];  //第2个识别结果可信度

			if(rgPlateColor[0] == 1 || rgPlateColor[0] == 10)  //第一次识别成白色或棕色，向上移动一个位置识别成红黄绿蓝等并且分数大于120
			{
				if(rgPlateColorProb[1] >= 120 && rgPlateColor[1] >=4 && rgPlateColor[1] <= 7)
				{
					iPlateColor = rgPlateColor[1];
				}
				if((rgPlateColor[0] == 1 && rgPlateColorProb[0] < 100 && rgCountColor[1]<=2) ||
					(rgPlateColor[0] == 10 && rgPlateColorProb[0] < 100 && rgCountColor[10]<=2))
				{
					iPlateColor = 2;
				}
			}
			else if((rgPlateColorProb[0] < rgPlateColorProb[1] && rgPlateColor[1] != 3) || 
				((rgPlateColor[1] > 3 || rgPlateColor[1] == 1) &&rgPlateColorProb[1] > 90))
			{
				iPlateColor = rgPlateColor[1];
			}

		}
		//车身亮度大于环境亮度——黑设置成灰
		if(iPlateColor == CC_BLACK && m_fEnableCorlorThr && DiffY > m_nBGColorThr)
		{
			iPlateColor = CC_GREY;
		}
		if(fIsNight && iPlateColor == CC_GREY && m_fEnableCorlorThr&& DiffY > m_nWGColorThr)
		{
			iPlateColor = CC_WHITE;
		}
		//港澳牌大车一般为白色
		if((Info.rgbContent[6] == 114 || Info.rgbContent[6] == 115) && ((iPlateColor > 3 && rgPlateColorProb[0] < 150 &&  rgPlateColorProb[1] < 150)
			||(iPlateColor == 2 && (nColorScore[1] >=90 || nColorScore2[1] >= 90))))
		{
			iPlateColor = 1;
		}
	}
	Info.nCarColour = (CAR_COLOR)iPlateColor;
	if (iPlateColor == CC_BLACK)  Info.iCarColor = 2;
	if(iPlateColor == CC_WHITE)  Info.iCarColor = 1;

	pTmpImg->Release();
	pTmpImg = NULL;

	return S_OK;
}

static HRESULT MapParamRect(CRect& rcResult, CRect rcWhole, CRect rcParam, float fltScale=.01f)
{
	rcResult.left=rcWhole.left+(int)(rcWhole.Width()*rcParam.left*fltScale + 0.5);
	rcResult.top=rcWhole.top+(int)(rcWhole.Height()*rcParam.top*fltScale + 0.5);
	rcResult.right=rcWhole.left+(int)(rcWhole.Width()*rcParam.right*fltScale + 0.5);
	rcResult.bottom=rcWhole.top+(int)(rcWhole.Height()*rcParam.bottom*fltScale + 0.5);

	return S_OK;
}

//投票
//HRESULT CTrackerImpl_Photo::Vote(
//			 PlateInfo** rgInfo,
//			 int nCount,
//			 PlateInfo* pResult,
//			 CParamQuanNum* pfltConf
//			 )
//{
//	if( nCount < 1 || !pfltConf) return E_INVALIDARG;
//
//	EnableDebugDevice(DEBUG_STR_DISPLAY_FILE, 1);
//
//	*pResult = *rgInfo[0];
//	*pfltConf = 0.0;
//
//	static int nResultCount = 0;
//	HV_Trace("\nRESULT_%d\n", nResultCount++);
//
//	for(int i = 0; i < nCount; i++)
//	{
//		HV_Trace("%d: ", i);
//		for(int j = 0; j < 7; j++)
//		{
//			HV_Trace("%d,", rgInfo[i]->rgbContent[j]);
//		}
//		HV_Trace("\n");
//	}
//
//	for (int i = 1; i < nCount; i++)
//	{
//		if (rgInfo[i]->fltTotalConfidence > pResult->fltTotalConfidence)
//		{
//			*pResult = *rgInfo[i];
//			*pfltConf = pResult->fltTotalConfidence;
//		}
//	}
//
//	return S_OK;
//}

__inline int CheckPos(int nIdx, int* rgIdx, int nLen)
{
	int nPos = -1;
	for(int i = 0; i < nLen; i++)
	{
		if(nIdx == rgIdx[i])
		{
			nPos = i;
			break;
		}
	}

	return nPos;
}

HRESULT CTrackerImpl_Photo::Vote(
								 PlateInfo** rgInfo,
								 int nCount,
								 PlateInfo* pResult,
								 CParamQuanNum* pfltConf
								 )
{
	if(  !rgInfo || !pResult || nCount < 1 || !pfltConf) return E_INVALIDARG;

	static int rgIdx[knPlateStrLen][g_cnPlateChars] = {0}; //各位字符结果集
	static int rgIdxCount[knPlateStrLen][g_cnPlateChars] = {0}; //字符结果计数
	static CParamQuanNum rgIdxConf[knPlateStrLen][g_cnPlateChars] = {0}; //各位字符结果可信度和
	static int rgResultCount[knPlateStrLen] = {0}; //各位字符结果种类计数
	static CParamQuanNum rgColor[PC_COUNT] = {0}; //颜色可信度统计
	static CParamQuanNum rgType[PLATE_TYPE_COUNT] = {0}; //车牌类型可信度统计

	HV_memset(rgIdx, 0, sizeof(rgIdx));
	HV_memset(rgIdxCount, 0, sizeof(rgIdxCount));
	HV_memset(rgIdxConf, 0, sizeof(rgIdxConf));
	HV_memset(rgResultCount, 0, sizeof(rgResultCount));
	HV_memset(rgColor, 0, sizeof(rgColor));
	HV_memset(rgType, 0, sizeof(rgType));

	*pResult = *rgInfo[0];
	*pfltConf = 0.0;

	int nWJCount = 0;

	//统计颜色和类型的可信度
	float fltMaxConf = -1;
	int nMaxConfID = 0; //记录可信度最高的结果
	PlateInfo* pCurInfo = NULL;
	for(int i = 0; i < nCount; i++)
	{
		pCurInfo = rgInfo[i];
		rgColor[pCurInfo->color] += pCurInfo->fltTotalConfidence;
		rgType[pCurInfo->nPlateType] += pCurInfo->fltTotalConfidence;

		if(pCurInfo->nPlateType == PLATE_WJ)
		{
			nWJCount++;
		}

		if(fltMaxConf < pCurInfo->fltTotalConfidence.ToFloat())
		{
			fltMaxConf = pCurInfo->fltTotalConfidence.ToFloat();
			nMaxConfID = i;
		}
	}

	//颜色投票
	PLATE_COLOR nColor = rgInfo[0]->color;
	CParamQuanNum fltConf = 0.0;
	for(int i = 0; i < PC_COUNT; i++)
	{
		if(fltConf < rgColor[i])
		{
			fltConf = rgColor[i];
			nColor = (PLATE_COLOR)i;
		}
	}

	//车牌类型投票
	PLATE_TYPE nType = rgInfo[0]->nPlateType;
	if(nWJCount >= 2)
	{
		nType = PLATE_WJ;
	}
	else
	{
		fltConf = 0.0;
		for(int i = 0; i < PLATE_TYPE_COUNT; i++)
		{
			if(fltConf < rgType[i])
			{
				fltConf = rgType[i];
				nType = (PLATE_TYPE)i;
			}
		}
	}

	//字符投票
	int nPos;
	for(int i = 0; i < nCount; i++)
	{
		if(rgInfo[i]->nPlateType != nType) continue;

		pCurInfo = rgInfo[i];

		for(int  j = 0; j < knPlateStrLen; j++) //统计各位结果
		{
			nPos = CheckPos(pCurInfo->rgbContent[j], rgIdx[j], rgResultCount[j]);
			if(nPos == -1)
			{
				nPos = rgResultCount[j];
				rgIdx[j][nPos] = pCurInfo->rgbContent[j];
//				rgIdxCount[j][nPos] = 1; //结果计数不可能为0
//				rgIdxConf[j][nPos] = pCurInfo->rgfltConfidence[j];
				rgResultCount[j]++;
			}

			rgIdxConf[j][nPos] += pCurInfo->rgfltConfidence[j];
			rgIdxCount[j][nPos]++;

			if(i == nMaxConfID) //对总可信度最高的结果增加权重
			{
				rgIdxConf[j][nPos] += pCurInfo->rgfltConfidence[j];
				rgIdxCount[j][nPos]++;
			}
		}
	}

	int rgContent[knPlateStrLen] = {0};
	CParamQuanNum rgConf[knPlateStrLen] = {0};
	CParamQuanNum nMaxConf;

	//首位特殊处理,投票以结果个数优先
	//rgContent[0] = rgIdx[0][0];
	//rgConf[0] = rgIdxConf[0][0];
	//int nMaxIdxCount = rgIdxCount[0][0];
	//for(int i = 1; i < rgResultCount[0]; i++)
	//{
	//	if(nMaxIdxCount > rgIdxCount[0][i]) continue;

	//	//如果个数一致则看可信度
	//	if(nMaxIdxCount == rgIdxCount[0][i] &&
	//	   rgConf[0] > rgIdxConf[0][i])
	//	{
	//		continue;
	//	}
	//
	//	nMaxIdxCount = rgIdxCount[0][i];
	//	rgContent[0] = rgIdx[0][i];
	//	rgConf[0] = rgIdxConf[0][i];
	//}

	//for(int i = 0; i < knPlateStrLen; i++)
	for(int i = 0; i < knPlateStrLen; i++)
	{
		rgContent[i] = rgIdx[i][0];
		rgConf[i] = rgIdxConf[i][0].ToFloat()/rgIdxCount[i][0];
		nMaxConf = rgIdxConf[i][0];
		for(int j = 1; j < rgResultCount[i];j++)
		{
			if(nMaxConf < rgIdxConf[i][j])
			{
				nMaxConf = rgIdxConf[i][j];
				rgContent[i] = rgIdx[i][j];
				rgConf[i] = rgIdxConf[i][j].ToFloat()/rgIdxCount[i][j];
			}
		}
	}

	//复制投票结果
	for(int i = 0; i < knPlateStrLen; i++)
	{
		pResult->rgbContent[i] = rgContent[i];
		pResult->rgfltConfidence[i] = rgConf[i];
	}

	//计算可信度
	pResult->fltTotalConfidence = 1.0;
	for(int i = 1; i < knPlateStrLen; i++)
	{
		pResult->fltTotalConfidence *= pResult->rgfltConfidence[i];
	}

	pResult->color = nColor;
	pResult->nPlateType = nType;

	*pfltConf = pResult->fltTotalConfidence;

	return S_OK;
}

HRESULT CTrackerImpl_Photo::GetInflateRect(
									 CRect &rect,			// in, out.
									 PLATE_COLOR color,
									 PLATE_TYPE &PlateType,
									 INT nMaxWidth		//大于此宽度的牌增加扩边范围
									 )
{
	int nWidth(rect.Width());
	int nHeight(rect.Height());

	if (( PLATE_DOUBLE_YELLOW == PlateType )
		|| (PLATE_DOUBLE_MOTO == PlateType))
	{
		rect.left -= (nWidth * 80) >> 10;
		rect.right += (nWidth * 80) >> 10;
		rect.top -= (nHeight * 80) >> 10;
	}
	else
	{
		rect.left -= (nWidth * 160) >> 10;
		rect.right += (nWidth * 160) >> 10;
	}

	nHeight = rect.Height();
	if (nHeight >= 60)		//60为分割时积分图最大高度
	{
		rect.top += (nHeight - 59) >> 1;
		rect.bottom -= nHeight - 59 - ((nHeight - 59) >> 1);
	}
	nWidth = rect.Width();
	if (nWidth >= 400)		//400为分割时积分图最大宽度
	{
		rect.left += (nWidth - 399) >> 1;
		rect.right -= nWidth - 399 - ((nWidth - 399) >> 1);
	}

	return S_OK;
}

int CTrackerImpl_Photo::LeftOrRight(int iX, int iY, int iLineNum, int* piDistance)
{
	int iRet = 0;
	if( iLineNum < 0 || iLineNum >= m_iRoadNumber ) return iRet;
	if( piDistance != NULL ) *piDistance = 0;

	int a, b, c, d;
	a = m_roadInfo[iLineNum].ptBottom.x;
	b = m_roadInfo[iLineNum].ptBottom.y;
	c = m_roadInfo[iLineNum].ptTop.x;
	d = m_roadInfo[iLineNum].ptTop.y;

	if( (c - a) == 0 ) c++;
	if( (d - b) == 0 ) d++;

	int iTX = -1;

	float fTmpT = float(d - b);
	float fTmpB = float(c - a);
	float fTmpTT = float(iY - b);
	iTX = (int) ((fTmpTT / (fTmpT / fTmpB)) + a + 0.5);

	if( iTX >= iX ) iRet = -1;
	else iRet = 1;

	int iA, iB, iC;
	if( piDistance != NULL )
	{
		iA = d - b;
		iB = a - c;
		iC = c * b - a * d;

		if( (iA * iA + iB * iB) != 0 )
		{
			*piDistance = (int)(abs(iA * iX + iB * iY + iC) / sqrt((float)(iA * iA + iB * iB)));
		}
	}

	return iRet;
}

int CTrackerImpl_Photo::MatchRoad(int iX, int iY)
{
	int iRet = -1;
	if( m_iRoadNumber <= 0 ) return iRet;

	int i = 0;
	for( i = 0; i < (m_iRoadNumber - 1) ; ++i )
	{
		if( -1 == LeftOrRight(iX, iY, i) ) break;
	}

	iRet = (i == 0)? 0 : i - 1;

	return iRet;
}

#define TRIM_PARAM(v, min, max) ( v = MIN_INT(MAX_INT((min), v), (max)) )


HRESULT CTrackerImpl_Photo::ProcessPhoto(
	HV_COMPONENT_IMAGE imgCurFrame,
	PVOID pvParam,
	PVOID pvData,
	PROCESS_PHOTO_RESPOND* pProcessRespond
	)
{
	if (!m_fInitialized)
	{
		return E_UNEXPECTED;
	}

	if( !pvParam )	//必须指定参数
	{
		return E_INVALIDARG;
	}

	if( !pvData )	//必须指定数据
	{
		return E_INVALIDARG;
	}

	if( !pProcessRespond )	//必须指定处理回应
	{
		return E_INVALIDARG;
	}

#ifdef RELEASE_TO_MARKET
	DWORD dwLockRet=CheckLock();
	if (dwLockRet!=ERROR_SUCCESS)
	{
		return E_UNEXPECTED;
	}
#endif // def RELEASE_TO_MARKET

	//设置参数
	PR_PARAM* pParam = (PR_PARAM*)pvParam;

	PROCESS_ONE_FRAME_DATA* pProcessData = (PROCESS_ONE_FRAME_DATA*)pvData;

	TRIM_PARAM( pParam->nMinPlateWidth, 56, 400);
	TRIM_PARAM( pParam->nMaxPlateWidth, pParam->nMinPlateWidth, 400);
	TRIM_PARAM( pParam->nVariance, 9, 1000 );
	for(int i = 0; i < pParam->nDetAreaCount; i++)
	{
		TRIM_PARAM( pParam->rgDetArea[i].right, 0, 100);
		TRIM_PARAM( pParam->rgDetArea[i].left, 0, pParam->rgDetArea[i].right);
		TRIM_PARAM( pParam->rgDetArea[i].bottom, 0, 100);
		TRIM_PARAM( pParam->rgDetArea[i].top, 0, pParam->rgDetArea[i].bottom);
	}

	float fltMinConf = m_cfgParam.fltMinConfForOutput;

	m_PlateRecogParam = *GetCurrentParam();

	m_PlateRecogParam.g_nDetMinStdVar = pParam->nVariance;

	//根据车牌宽度范围算SCALE
	m_PlateRecogParam.g_nDetMinScaleNum = MAX_INT((int)( 10.5 * log((float)pParam->nMinPlateWidth) - 42.23) + 1, 0);
	m_PlateRecogParam.g_nDetMaxScaleNum = MIN_INT( 1 + (int)( 10.5 * log((float)pParam->nMaxPlateWidth) - 42.23), 25 );
	m_PlateRecogParam.g_nDetMaxScaleNum = MAX_INT(m_PlateRecogParam.g_nDetMinScaleNum + 1, m_PlateRecogParam.g_nDetMaxScaleNum);

	if (imgCurFrame.iWidth != m_nWidth || imgCurFrame.iHeight != m_nHeight)
	{
		FRAME_PROPERTY cFrameProperty;
		cFrameProperty.iWidth = imgCurFrame.iWidth;
		cFrameProperty.iHeight = imgCurFrame.iHeight;
		cFrameProperty.iStride = imgCurFrame.iStrideWidth[0];

		SetFrameProperty(cFrameProperty);
	}

	//切图
	CRect rcMax(0, 0, m_nWidth, m_nHeight);

	//检测
	MULTI_ROI MultiROI=
	{
		0,
		m_rgTempPlatePos
	};
	int nAllPlateCount(0),nFrameYSum(0), nFrameAvgY(0);

	for(int i = 0; i < pParam->nDetAreaCount; i++)
	{
		CRect rcSearchRegion;
		HV_RECT rcCrop = pParam->rgDetArea[i];
		RTN_HR_IF_FAILED(MapParamRect(rcSearchRegion, rcMax, rcCrop));

		if(rcSearchRegion.Width() > m_nMaxCropWidth ||
			rcSearchRegion.Height() > m_nMaxCropHeight)
		{
			continue;
		}

		//扩展
		rcSearchRegion.left = MAX_INT(0, rcSearchRegion.left - 10);
		rcSearchRegion.top = MAX_INT(0, rcSearchRegion.top - 20); //因底端扩边需补偿
		rcSearchRegion.right = MIN_INT(rcMax.Width(), rcSearchRegion.right + 10);
		rcSearchRegion.bottom = MIN_INT(rcMax.Height(), rcSearchRegion.bottom + 10);

		if(rcSearchRegion.Width() > m_nMaxCropWidth)
		{
			rcSearchRegion.right -= (m_nMaxCropWidth - rcSearchRegion.Width());
		}

		if(rcSearchRegion.Height() > m_nMaxCropHeight)
		{
			rcSearchRegion.top += (m_nMaxCropHeight-rcSearchRegion.Height());
		}

		HV_COMPONENT_IMAGE imgSearchRegion;
		RTN_HR_IF_FAILED(CropImage(imgCurFrame, rcSearchRegion, &imgSearchRegion));

		MULTI_ROI TempMultiROI =
		{
			ARRSIZE(m_rgTempPlatePos) - nAllPlateCount,
			m_rgTempPlatePos + nAllPlateCount
		};

		int nFrameY = 0;
		RTN_HR_IF_FAILED(m_pPlateDetector->ScanPlatesComponentImage(
			ALL_DETECTOR_PHOTO,
			&imgSearchRegion,
			&TempMultiROI,
			0,
			0,
			TRUE,
			0,
			FALSE,
			&nFrameY
			));
		nFrameYSum += nFrameY;

		//调整检出位置为绝对坐标
		for(int j = 0; j < TempMultiROI.dwNumROI; j++)
		{
			RECTA* pROI = TempMultiROI.rcROI + j;
			pROI->left += rcSearchRegion.left;
			pROI->top += rcSearchRegion.top;
			pROI->right += rcSearchRegion.left;
			pROI->bottom += rcSearchRegion.top;
		}

		nAllPlateCount += TempMultiROI.dwNumROI;
	}
	if(pParam->nDetAreaCount != 0) nFrameAvgY = nFrameYSum/pParam->nDetAreaCount;
	MultiROI.dwNumROI = nAllPlateCount;

	//识别结果
	PlateInfo* aInfo[128] ={0};
	int nInfoCount = 0;

	for (int i = 0; i < MIN_INT(MultiROI.dwNumROI, 128); i++)
	{
		RECTA *pROI = MultiROI.rcROI + i;
		CRect rcCurPlate(*pROI);
		if (!rcCurPlate.IntersectRect(&rcCurPlate, &rcMax)) continue;

		int nPlateVar = pROI->nVariance;

		HV_COMPONENT_IMAGE rgCrop[CROP_COUNT];
		CPersistentComponentImage rgPlate[CROP_COUNT * PRETREATMENT_COUNT];

		//确定投票数
		int nCropCount = CROP_COUNT;
		int nPlateCount = PRETREATMENT_COUNT;

		if(!m_cfgParam.fEnableVote)
		{
			nCropCount = 1;
			nPlateCount = 1;
		}
		if(!m_fEnableRectify)
		{
			nPlateCount = 1;

		}
		int nAllCount = nCropCount * nPlateCount;

		CropPlateImage(imgCurFrame, rcCurPlate, pROI->nPlateType, rgCrop, nCropCount);

		BOOL fEnhance = ((int)pROI->nVariance < m_cfgParam.nPlateEnhanceThreshold);
		CBinaryInfo BinaryInfo;

		for(int j = 0; j < nCropCount; j++)
		{
			// 如果不是YUV422格式，需要转换一次
			CPersistentComponentImage imgYUVPlate;
			if (rgCrop[j].nImgType != HV_IMAGE_YUV_422)
			{
				imgYUVPlate.Create(HV_IMAGE_YUV_422, rgCrop[j].iWidth, rgCrop[j].iHeight, 1);
				imgYUVPlate.Convert(rgCrop[j]);
				rgCrop[j] = (HV_COMPONENT_IMAGE)imgYUVPlate;
			}
			PreTreatment(
				rgCrop[j],
				pROI->nPlateType,
				pROI->color,
				fEnhance,
				rgPlate + j * nPlateCount,
				nPlateCount,
				BinaryInfo
				);
		}

		GetInflateRect(
			rcCurPlate,			// in, out.
			pROI->color,
			pROI->nPlateType,
			200		//大于此宽度的牌增加扩边范围
			);
		rcCurPlate.IntersectRect(&rcMax,&rcCurPlate);

		PlateInfo cInfo;
		cInfo.rcPos = rcCurPlate;
		cInfo.color = pROI->color;
		cInfo.nPlateType = pROI->nPlateType;
		cInfo.iLastDetWidth = pROI->right-pROI->left;
		cInfo.nVariance = nPlateVar;

		int nPosX = rcCurPlate.CenterPoint().x;
		int nPosY = rcCurPlate.CenterPoint().y;

		int nBottomLine = imgCurFrame.iHeight * 2 / 3;
		int nFiveCharWidth = (nPosY > nBottomLine)?m_iBottomWidth:m_iMiddleWidth;

		PlateInfo* aInfo_1[128] = {0};
		PlateInfo* aInfo_2[128] = {0};
		int nInfoCount_1 = 0;
		int nInfoCount_2 = 0;

		for(int j = 0; j < nAllCount; j++)
		{
			PlateInfo* pInfo = new PlateInfo;
			if(pInfo == NULL) continue;

			*pInfo = cInfo;
			if(S_OK != RecognizePlate(rgPlate[j], *pInfo, BinaryInfo, nFiveCharWidth))
			{
				SAFE_DELETE(pInfo);
				continue;
			}
			aInfo_1[nInfoCount_1++] = pInfo;
		}

		//投票
		PlateInfo* pResult_1 = new PlateInfo;
		CParamQuanNum fltConf1 = 0.0;
		if(S_OK != Vote(aInfo_1, nInfoCount_1,pResult_1,&fltConf1))
		{
			SAFE_DELETE(pResult_1);
		}

		//是否改变字符颜色重识别
		BOOL fReRecog = TRUE;
		if( pResult_1 != NULL )
		{
			if( pROI->nPlateType == PLATE_DOUBLE_YELLOW ||
				pResult_1->nPlateType == PLATE_WJ )	//双层牌和WJ牌不重识别
			{
				fReRecog = FALSE;
			}
			else if( pROI->nPlateType == PLATE_NORMAL && //可信度高的蓝牌不重识别
				 pResult_1->fltTotalConfidence > 0.25f)
			{
				fReRecog = FALSE;
			}
			else if(pResult_1->fltTotalConfidence > 0.35f)
			{
				fReRecog = FALSE;
			}

			//SaveDebugText("C:\\Conf.log","Conf=%8.3f",pResult_1->fltTotalConfidence.ToFloat());
		}

		if( fReRecog )
		{
			if(pROI->nPlateType == PLATE_UNKNOWN)
			{
				cInfo.rcPos = rcCurPlate;
				cInfo.color = PC_BLUE;
				cInfo.nPlateType = PLATE_NORMAL;
				cInfo.iLastDetWidth = pROI->right-pROI->left;
			}
			else if(pROI->nPlateType == PLATE_NORMAL)
			{
				cInfo.rcPos = rcCurPlate;
				cInfo.color = PC_YELLOW;
				cInfo.nPlateType = PLATE_UNKNOWN;
				cInfo.iLastDetWidth = pROI->right-pROI->left;
			}

			for(int j = 0; j < nAllCount; j++)
			{
				PlateInfo* pInfo = new PlateInfo;
				if(pInfo == NULL) continue;

				*pInfo = cInfo;
				if(S_OK != RecognizePlate(rgPlate[j],  *pInfo, BinaryInfo, nFiveCharWidth))
				{
					SAFE_DELETE(pInfo);
					continue;
				}
				aInfo_2[nInfoCount_2++] = pInfo;
			}
		}

		PlateInfo* pResult_2 = new PlateInfo;
		CParamQuanNum fltConf2 = 0.0;
		if(S_OK != Vote(aInfo_2, nInfoCount_2,pResult_2,&fltConf2))
		{
			SAFE_DELETE(pResult_2);
		}

		PlateInfo* pResult = pResult_1;
		if(pResult_2)
		{
			if(	pResult_1 == NULL )
			{
				pResult = pResult_2;
			}
			else if(pResult_2->fltTotalConfidence.ToFloat() > (1.5 * pResult_1->fltTotalConfidence.ToFloat()))
			{
				pResult = pResult_2;
			}

			if(pResult == pResult_2)
			{
				SAFE_DELETE(pResult_1);
			}
			else
			{
				SAFE_DELETE(pResult_2);
			}
		}

		if(pResult)
		{
			pResult->fValid = TRUE;
			aInfo[nInfoCount++] = pResult;
		}

		//清理
		for(int j = 0; j < nInfoCount_1; j++)
		{
			SAFE_DELETE(aInfo_1[j]);
		}

		for(int j = 0; j < nInfoCount_2; j++)
		{
			SAFE_DELETE(aInfo_2[j]);
		}
	}

	//对同一个位置的多个track只保留一个
	for (int i = 0; i < nInfoCount - 1; i++)
	{
		for (int j = i + 1; j < nInfoCount; j++)
		{
			CRect rect1(aInfo[i]->rcPos);
			CRect rect2(aInfo[j]->rcPos);
			rect1.InflateRect(
                rect1.Width()*((m_pParam->g_fltTrackInflateX).ToInt()),
				rect1.Height()*((m_pParam->g_fltTrackInflateY).ToInt())
				);
			rect2.InflateRect(
				rect2.Width()*((m_pParam->g_fltTrackInflateX).ToInt()),
				rect2.Height()*((m_pParam->g_fltTrackInflateY).ToInt())
				);
			if (rect1.IntersectsWith(rect2))
			{
				if (aInfo[i]->fltTotalConfidence > aInfo[j]->fltTotalConfidence)
				{
					aInfo[j]->fValid = false;
				}
				else
				{
					aInfo[i]->fValid = false;
				}
			}
		}
	}

	// 按照可信度排序
	PlateInfo* tmpInfo;
	for (int i = 0; i < nInfoCount - 1; i++)
	{
		for (int j = i + 1; j < nInfoCount; j++)
		{
			BOOL fChange = FALSE;
			if(m_nFirstPlatePos == 1)
			{
				CRect rcCur = aInfo[i]->rcPos;
				CRect rcDest = aInfo[j]->rcPos;
				if(rcCur.CenterPoint().y < rcDest.CenterPoint().y)
				{
					fChange = TRUE;
				}
			}
			else if(aInfo[i]->fltTotalConfidence < aInfo[j]->fltTotalConfidence)
			{
				fChange = TRUE;
			}

			if (fChange)
			{
				tmpInfo = aInfo[i];
				aInfo[i] = aInfo[j];
				aInfo[j] = tmpInfo;
			}
		}
	}

	for(int i = 0; i < nInfoCount; i++)
	{
		float fltConf = exp(log(aInfo[i]->fltTotalConfidence.ToFloat()) * 0.143f);
		aInfo[i]->fltTotalConfidence = fltConf;
		if(	i != 0 && fltConf < fltMinConf)
		{
			aInfo[i]->fValid = FALSE;
		}
	}

	for (int i = 0; i < nInfoCount; i++)
	{
		if(aInfo[i]->fValid != TRUE) continue;

		for ( int j = 0; j < 7; j++)
		{
			if(aInfo[i]->rgbContent[j] == 0)
			{
				aInfo[i]->rgbContent[j] = 1;
			}
		}

		//---------------------------------------------------------------------------------------------
		// 车牌类型验证
		// 针对各种类型的车牌进行规则检查，修正预投票结果
		//---------------------------------------------------------------------------------------------
		switch(aInfo[i]->nPlateType)
		{
		case PLATE_NORMAL:				// 蓝牌或黑牌
			if ((aInfo[i]->rgbContent[0] == 57) && (aInfo[i]->rgbContent[1] == 36))
			{
				// 前两位为“粤Z”
				if ((aInfo[i]->rgbContent[6] != 114) && (aInfo[i]->rgbContent[6] != 115))		// 港澳字尾
				{
					aInfo[i]->rgbContent[6] = 115;
				}
				aInfo[i]->color = PC_BLACK;			// 强制设为黑色
			}
			break;
		case PLATE_POLICE:				// 、军牌或广西警牌
			if (IsMilitaryChn(aInfo[i]->rgbContent[0]) || ((aInfo[i]->rgbContent[0] == 38) && (aInfo[i]->rgbContent[1] == 32)))
			{
				aInfo[i]->color = PC_WHITE;			// 强制设为白色
				break;
			}

			// (字头中已经没有军牌了，只能是黄牌和广西警牌)
			if (aInfo[i]->rgbContent[6] == 37)		// 最后一位为“警”
			{
				if (aInfo[i]->rgbContent[0] != 58)	// 但第一位非“桂”
				{
					// 标准黄牌或浅蓝牌
					if (aInfo[i]->color == PC_WHITE)
					{
						aInfo[i]->color = PC_YELLOW;			// 强制设为黄色,也可能是浅蓝牌
					}
				}
				else
				{
					// 广西警牌
					aInfo[i]->color = PC_WHITE;			// 强制设为白色
					aInfo[i]->nPlateType = PLATE_POLICE2;
				}
			}
			else
			{
				// 标准黄牌或浅蓝牌
				if (aInfo[i]->color == PC_WHITE)
				{
					aInfo[i]->color = PC_YELLOW;			// 强制设为黄色,也可能是浅蓝牌
				}
			}
			break;
		case PLATE_POLICE2:							// 标准警牌
			aInfo[i]->rgbContent[6] = 37;					// 强制设为警字
			aInfo[i]->color = PC_WHITE;					// 强制设为白色
			break;
		case PLATE_DOUBLE_YELLOW:					// 双层牌
			if (IsMilitaryChn(aInfo[i]->rgbContent[0]) || ((aInfo[i]->rgbContent[0] == 38) && (aInfo[i]->rgbContent[1] == 32)))
			{
				aInfo[i]->color = PC_WHITE;				// 强制设为白色
			}
			else
			{
				// 双黄牌
				aInfo[i]->color = PC_YELLOW;			// 强制设为黄色
			}
			break;
		default:
			break;
		}
		//---------------------------------------------------------------------------------------------
		// 车牌类型验证结束
		//---------------------------------------------------------------------------------------------


		//确定车型
		CAR_TYPE nCarType = CT_SMALL;
		switch(aInfo[i]->nPlateType)
		{
		case PLATE_NORMAL:						// 蓝牌或黑牌
			nCarType = CT_SMALL;
			break;
		case PLATE_POLICE:						// 黄牌、军牌或广西警牌
			nCarType = CT_LARGE;
			if (aInfo[i]->color == PC_WHITE)
			{
				nCarType = CT_SMALL;
			}
			break;
		case PLATE_POLICE2:						// 标准警牌
			nCarType = CT_SMALL;
			break;
		case PLATE_DOUBLE_YELLOW:				// 双层牌
			nCarType = CT_LARGE;
			break;
		default:
			nCarType = CT_SMALL;
			break;
		}

		int iRoadNum = -1;
		iRoadNum = MatchRoad(aInfo[i]->rcPos.CenterPoint().x, aInfo[i]->rcPos.CenterPoint().y);
		if(iRoadNum == -1) iRoadNum = 0xff;

		//识别车身颜色
		if(m_fEnableRecgCarColor)
		{
			RecognizeCarColourPlus(imgCurFrame,	*aInfo[i], nFrameAvgY);
		}
		//RecognizeCarColour(imgCurFrame,	*aInfo[i]);

		// 车辆尺寸
		OUTPUT_CAR_SIZE_INFO stCarSize;
		if (m_fOutputCarSize == TRUE)
		{
			memset(&stCarSize, 0, sizeof(stCarSize));
			CalcCarSize(&imgCurFrame, &stCarSize, nCarType);
		}

		if (pProcessRespond->iResultCount + 1 >= MAX_EVENT_COUNT)
		{
			HV_Trace(5, "\nOutof result count!");
			break;
		}

		// Copy小图到共享内存
		HV_COMPONENT_IMAGE imgPlate;
		imgPlate.nImgType = imgCurFrame.nImgType;
		HiVideo::CRect& rect = aInfo[i]->rcPos;

		RTN_HR_IF_FAILED(CropImage(imgCurFrame, rect, &imgPlate));

		CPersistentComponentImage imgYUVPlate;
		if (imgPlate.nImgType != HV_IMAGE_YUV_422)
		{
			imgYUVPlate.Create(HV_IMAGE_YUV_422, imgPlate.iWidth, imgPlate.iHeight);
			imgYUVPlate.Convert(imgPlate);
			imgPlate = (HV_COMPONENT_IMAGE)imgYUVPlate;
		}

		for (int k = 0; k < 3; k++)
		{
			PBYTE8 pSrc = GetHvImageData(&imgPlate, k);
			PBYTE8 pDes = GetHvImageData(&pProcessData->rghvImageSmall[pProcessRespond->iResultCount], k);
			if (pSrc && pDes)
			{
				memcpy(pDes, pSrc, imgPlate.iStrideWidth[k] * imgPlate.iHeight);
			}
		}

		//二值图
		if (GetHvImageData(&imgPlate, 0) != NULL)
		{
			BYTE8 rgbResizeImg[BINARY_IMAGE_WIDTH * BINARY_IMAGE_HEIGHT];

			int iWidth = BINARY_IMAGE_WIDTH;
			int iHeight = BINARY_IMAGE_HEIGHT;
			int iStride = BINARY_IMAGE_WIDTH;

			// 只输出中心部分二值化图
			ScaleGrey(rgbResizeImg, iWidth, iHeight, iStride,
				GetHvImageData(&imgPlate, 0),
				rect.right - rect.left,
				rect.bottom - rect.top,
				imgPlate.iStrideWidth[0]
				);

			// 计算二值化阈值
			HiVideo::CRect rcArea(0, 0, iWidth, iHeight);
			int iThre = CalcBinaryThreByCenter(
				rgbResizeImg,
				iWidth,
				iHeight,
				iStride,
				&rcArea);

			// 根据车牌颜色设置二值化方向
			BOOL fIsWhiteChar = FALSE;
			if ((aInfo[i]->color == PC_BLUE) ||
				(aInfo[i]->color == PC_BLACK) ||
				(aInfo[i]->color == PC_GREEN))
			{
				fIsWhiteChar = TRUE;
			}
			GrayToBinByThre(
				fIsWhiteChar,
				rgbResizeImg,
				iWidth,
				iHeight,
				iStride,
				iThre,
				GetHvImageData(&pProcessData->rghvImageBin[pProcessRespond->iResultCount], 0)
				);
		}

		PROCESS_IMAGE_CORE_RESULT* pCurResult = &(pProcessRespond->rgProcessPhotoResult[pProcessRespond->iResultCount]);
		memcpy(pCurResult->rgbContent, aInfo[i]->rgbContent, g_cnPlateChars);
		pCurResult->nType = aInfo[i]->nPlateType;
		pCurResult->nColor = aInfo[i]->color;
		pCurResult->fltAverageConfidence = aInfo[i]->fltTotalConfidence.ToFloat();
		pCurResult->fltFirstAverageConfidence = aInfo[i]->rgfltConfidence[0].ToFloat();
		pCurResult->rcBestPlatePos = aInfo[i]->rcPos;
		pCurResult->iAvgY = nFrameAvgY;
		pCurResult->nCarType = nCarType;
		pCurResult->iCarVariance = aInfo[i]->nVariance;
		pCurResult->nRoadNo = iRoadNum;
		pCurResult->nCarColor = aInfo[i]->nCarColour;
		//pCurResult->fOutputCarColor = m_fEnableRecgCarColor == TRUE;
		(pProcessRespond->iResultCount)++;
	}

	if(nInfoCount == 0)
	{
		pProcessRespond->iResultCount = 1;
		PROCESS_IMAGE_CORE_RESULT* pCurResult = &(pProcessRespond->rgProcessPhotoResult[0]);
		memset(pCurResult->rgbContent, 0, sizeof(pCurResult->rgbContent));
		pCurResult->nRoadNo =  0xff;
		pCurResult->nCarColor = CC_UNKNOWN;
	}

	for(int i = 0; i < nInfoCount; i++)
	{
		SAFE_DELETE(aInfo[i]);
	}

	return S_OK;
}


