#ifndef _MODEL_H
#define _MODEL_H

#ifdef __cplusplus
extern "C" {
#endif//#ifdef __cplusplus

#include "config.h"
#include "swPlate.h"
#include "swBaseType.h"
#include "swObjBase.h"
#include "SysUpdate.h"

#define USE_NEW_WJ			// 使用新WJ模型

//模型转换参数
#define AlphaCoef			100000			//alpha由浮点数转换成定点数的权数.
#define SaveCoef			1000000.0		//将浮点数变换成定点数.
#define FinalHCoef			128				//FinalH由浮点数转换成定点数的权数(相对提高精度)而设置的.

#define FloatScaleSize			1.09f			//缩放比例
#define IntegerScaleSize		(WORD16)(FloatScaleSize * (1 << 5))

#define MAX_SCALE_NUM		    25				//最大的缩放级

/* 检测使用的模型框数据结构定义. */
typedef struct {
	BYTE8 		bPosX, bPosY, bWidth, bHeight;	//对应于框左上角坐标,框宽度,框高度
	BYTE8 		bType;							//框的类型(计算feature值运算方式).
} FinalType;

/* 模型layer的原始数据数据结构. */
typedef struct {
	DWORD32 	dwCascade;			//该layer到此为止共有多少个feauter模型检测框
	float 		fltFinalThreshold;	//该layer的阈值.
} _FeatureGroup;

/* 模型feature的原始数据数据结构. */
typedef struct {
	FinalType 	FinalType;
	char 		chFinalSign;		//必须为有符号的8位CHAR数据类型,由于在模型导入时会对其进行正负值比较.
	float 		fltFinalL;			//当前Feature的阈值,低阈值 
	float 		fltFinalH;			//当前Feature的阈值.
	float 		fltAlpha;			//当前Feature的权值.
} _Feature;

//原始检测模型结构
typedef struct _ModelInfo
{
	int nModelWidth, nModelHeight;
	int nMaxLayer, nMaxFeature;
	const _Feature *pFeature;
	const _FeatureGroup *pFeatureGroup;
	PLATE_TYPE PlateType;
} ModelInfo;

/* 检测用的layer层数据结构. */
typedef struct {
	DWORD32 	dwCascade;			//同上.
	SDWORD32 	fltFinalThreshold;	//量化后Layer的阈值.
} FeatureGroupInt;

/* 检测用的feature数据结构. */
typedef struct {
	FinalType 	FinalType;
	char 		chPositiveSign;		
	SDWORD32 	sdwFinalL;			//量化后的FinalL
	SDWORD32 	sdwFinalH;			//量化后的FinalH
	SDWORD32 	sdwAlpha;			//量化后的Alpha
} FeatureBuf;

/* 检测用的模型数据放置在FLASH的存储结构定义. */
typedef struct _DET_MODEL_ {
	DWORD32 	dwHeaderSize;			//文件头大小.
	DWORD32 	dwDetModelFileSize;		//模型数据文件大小.
	DWORD32 	dwFeatureHeaderSize;	//Feature头大小.
	DWORD32 	dwGroupHeaderSize;		//Group头大小.
	DWORD32 	dwLayerCount;			//模型分类器层数.
	DWORD32 	dwFeatureCount;			//模型属性总数目.
	DWORD32 	dwFeatureOffset;		//模型的Feature开始存放的偏移量.
	DWORD32 	dwGroupOffset;			//模型的Group属性开始存放的偏移量.
	DWORD32 	dwModelWidth;			//模型中车牌标准宽度.
	DWORD32 	dwModelHeight;			//模型中车牌标准高度.
	DWORD32 	dwModelPlateType;		//模型中车牌类型.
	DWORD32 	dwReserved[2];			//当前保留字.
} DET_MODEL;

/* 检测所使用的核心工作函数原型. */
typedef int ( *SCAN_scalePlateFxn )( 
	PDWORD32	pdwItgImage,		//指向一般积分图检测框起始数据.
	WORD16		nStrideWidth,		//积分图行数据保存的Stride宽度.
	void		*pModel,			//指向模型数据.
	WORD16		nScaleNum,			//当前使用哪个Scale进行检测.
	WORD16		nVariance			//检测框的Variance值.
);

/* 检测所使用的模型数据结构. */
typedef struct _DPModel {
	FeatureBuf 	*pFeatureBuf;			//指向所有SCALE下的feature数据结构数据.
	FeatureGroupInt *pFeatureGroup;		//指向所有SCALE下的layer数据结构数据.
	BOOL 		fInitialized;			//指示当前模型数据结构是否已初始化.
	int 		nMaxLayreNum;			//该模型最大的layer数.
	int 		nMaxFeatureNum;			//该模型最大的feature数.
	int 		nModelWidth;				//模型检测框的宽度.
	int 		nModelHeight;			//模型检测框的高度.
	PLATE_TYPE 	PlateType;				//该模型的车牌类型.
	PLATE_COLOR ModelColor;				//该模型的车牌色彩.
	int 		nWinSizeScaleWidth[MAX_SCALE_NUM];		//该模型在不同SCALE情况下的框最小宽度.
	int 		nWinSizeScaleHeight[MAX_SCALE_NUM];		//该模型在不同SCALE情况下的框最小高度.
	DWORD32 	dwAreaSizeScale[MAX_SCALE_NUM];			//该模型在不同SCALE情况下的框面积.

	FeatureBuf 	*pFastFeatureBuf;			//指向所有SCALE下的feature数据结构数据.
	int nMinFastScale;
	int nMaxFastScale;
} DPModel;

extern DPModel fiveModel;
extern DPModel blueModel;
extern DPModel yellowModel;
extern DPModel dYellowModel;
extern DPModel wjModel;
extern DPModel DBGreenModel;

extern ModelInfo originModel[];

#ifdef __cplusplus
}
#endif //#ifdef __cplusplus

#endif //#ifndef _MODEL_H
