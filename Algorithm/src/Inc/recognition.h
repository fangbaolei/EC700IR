#ifndef _RECOGNITION_INCLUDED__
#define _RECOGNITION_INCLUDED__

#include "swobjbase.h"
#include "swimage.h"
#include "swbasetype.h"
#include "swplate.h"
#include "inspector.h"
//#include "trackerdef.h"
//#include "charquotietymatrix.h"
#include "hvcoretype.h"
#include "hvinterface.h"

using namespace HiVideo;

// 工作模式, SetWorkMode()函数的nProcMode参数使用
#define RECOG_MODE_DEFAULT 0
#define RECOG_MODE_IR     1

typedef struct _RECOGNITION_CFG_PARAM
{
	BOOL fEnableAlphaRecog; 
	BOOL fEnableT1Model;
	BOOL fEnableAlpha_5;
	BOOL fEnableEnhanceIR_GX;  // 增强红外广西桂字模型开关
	_RECOGNITION_CFG_PARAM()
	{
		fEnableAlphaRecog = FALSE;
		fEnableT1Model = FALSE;
		fEnableAlpha_5 = FALSE;
		fEnableEnhanceIR_GX = FALSE;
	}
} RECOGNITION_CFG_PARAM;

#define QUANTIFY_DIGIT	16			//量化位数（2的幂），16表示65536

#define RECOGNITION_TYPE_LIST(MACRO_NAME)	\
	MACRO_NAME(ALL=0, "全")                 \
	MACRO_NAME(ALLMT, "摩托全")             \
	MACRO_NAME(DIGIT, "数字")	            \
	MACRO_NAME(ALPHA, "字母")	            \
	MACRO_NAME(ALPHAMT, "摩托字母")	            \
	MACRO_NAME(CHN, "汉字")					\
	MACRO_NAME(CHNMT, "摩托汉字")				\
	MACRO_NAME(CHNSF, "省份汉字")			\
	MACRO_NAME(DIGIT_J, "数字警")			\
	MACRO_NAME(MILITARYCHN , "军车汉字")	\
	MACRO_NAME(WJ, "武警")					\
	MACRO_NAME(DIGITGUA, "数字挂")			\
	MACRO_NAME(GA, "港澳")					\
	MACRO_NAME(COLOR, "颜色")				\
	MACRO_NAME(COLORNIGHT, "颜色晚上")					\
	MACRO_NAME(CHNALHPA, "汉字字母")		\
	MACRO_NAME(CHNALHPAMT, "双层汉字字母")		\
	MACRO_NAME(ALPHAH, "字母航")            \
	MACRO_NAME(CARLOGO, "车标")				   \

#define DEFINE_ROGTYPE_ENUM(a, b) RECOG_##a,

enum RECOGNITION_TYPE
{
	RECOGNITION_TYPE_LIST(DEFINE_ROGTYPE_ENUM)
	RECOG_MAXTYPE
};

//enum ALLCOLOR
//{
//	COLOR_HEI = 1,
//	COLOR_HUI,
//	COLOR_HONG,
//	COLOR_HUANG,
//	COLOR_BAI,
//	COLOR_LAN,
//	COLOR_LU,
//	COLOR_ZI,
//	COLOR_FEN,
//	COLOR_ZONG
//};

class IRecognition
{
public:
	virtual ~IRecognition() {};

	STDMETHOD(Evaluate)(
		HV_COMPONENT_IMAGE *pInput,				//反白后的图象
		RECOGNITION_TYPE RecogType,		//识别类型
		PLATE_TYPE nPlateType,			// 车牌类型
		PLATE_COLOR nPlateColor,		//车牌颜色
		PBYTE8 pbResult,				//返回结果，指向地址保存UNICODE字符编码
		PSDWORD32 pdwProb,				//量化的可信度，量化度由QUANTIFY_DIGIT决定
		int iCharPos=-1					//Optional. For outputing the normalized image
	)=0;

	STDMETHOD(EvaluateColor)(
		HV_COMPONENT_IMAGE *pInput,
		PBYTE8 pbResult,
		PSDWORD32 pdwProb,
		LIGHT_TYPE m_LightType
	)=0;
	//车标识别 change liangjf
	STDMETHOD(EvaluateCarLogo)(
		HV_COMPONENT_IMAGE *pInput,
		int *pbResult,                   
		PSDWORD32 pdwProb                 
		)=0;

	STDMETHOD(SetInspector)(
		IInspector *pInspector
	)=0;

	STDMETHOD(SetCharSet)(				//设置汉字字符集
		ChnCharSet nCharSet
	)=0;

	/*STDMETHOD(SetCharQuotietyMatrix)(				// 设置字符权值矩阵
		ICharQuotietyMatrix *pCharQuotietyMatrix
	) = 0;*/

	STDMETHOD(SetHvParam)(HvCore::IHvParam2* pHvParam) = 0;

	STDMETHOD(SetHvModel)(HvCore::IHvModel* pModel) = 0;

	STDMETHOD(SetHvPerformance)(
		HvCore::IHvPerformance* pHvPerf
	) = 0;

	STDMETHOD(InitRecognition)(RECOGNITION_CFG_PARAM* pCfgParam) = 0;

    STDMETHOD(SetWorkMode)(
        int iProcMode = RECOG_MODE_DEFAULT
    ) = 0;

	STDMETHOD(EraseNail)(
		bool fIsNail,
		HV_RECT rgrcNailPos,
		int* piBottomLine
	) = 0;

	STDMETHOD(CheckValid)(
		HV_COMPONENT_IMAGE imgChar,
		BOOL fIsWhiteChar,
		int nId,
		BOOL *pValid
	) = 0;

	STDMETHOD(GetVersion)(
		char* szBuf, int szBufLen
	) = 0;
};

HRESULT CreateRecognition(IRecognition **ppRecognition);

#endif // _RECOGNITION_INCLUDED__

