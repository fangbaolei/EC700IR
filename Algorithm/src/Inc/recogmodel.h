#ifndef _MODEL_INCLUDED__
#define _MODEL_INCLUDED__

#include "swbasetype.h"
#include "hvutils.h"

// TODO: 如果超过了这些限制怎么办？
#define MAX_LAYER_UNITS_NUM 256			// NN模型层最大单元数
#define MAX_SVM_FEATURE_NUM	80			// SVM 最大特征数
#define MAX_SVM_CLASS_NUM	45			// SVM 最大识别类型
#define MAX_SVM_SV_NUM	2100			// SVM 最大支持的向量数量

// 模型类型定义
#define TYPE_SVM 0
#define TYPE_NN	 1
#define TYPE_TEMP 2

// 模型信息定义表
typedef struct
{
	char name[20];					// 模型名
	INT year;						// 年月日，以下3各表示版本信息
	INT month;
	INT day;
} StModelInfo;


// 量化NN模型用到的结构
typedef struct                    /* A LAYER OF A NET:                     */
{
	INT				Num_Units;	  /* - number of units in this layer       */
	SDWORD32*		Output;       /* - output of ith unit                  */
	SDWORD32*		Weight;       /* - connection weights to ith unit      */
	SDWORD32*		Bias;         /* - Bias added to each units            */
} LAYER;

typedef struct                    /* A NET:                                */
{
	INT           Num_Layers;     /* The number of layers                  */
	LAYER*        Layer;          /* - layers of this net                  */
	LAYER*        InputLayer;     /* - input layer                         */
	LAYER*        OutputLayer;    /* - output layer                        */
	INT*          nLabel;         /* 字符下标*/
	StModelInfo*  info;
} NET;

// 量化SVM模型用到的结构
struct svm_parameter
{
	SDWORD32 gamma;				  /* for poly/rbf/sigmoid */
};

struct svm_model
{
	svm_parameter *param;	// parameter
	INT nr_class;			// number of classes, = 2 in regression/one class svm
	INT l;					// total #SV
	SDWORD32 *SV;			// SVs (SV[l])
	SDWORD32 *sv_coef;		// coefficients for SVs in decision functions (sv_coef[n-1][l])
	SDWORD32 *rho;			// constants in decision functions (rho[n*(n-1)/2])
	// for classification only
	INT *label;				// label of each class (label[n])
	INT *nSV;				// number of SVs for each class (nSV[n])
							// nSV[0] + nSV[1] + ... + nSV[n-1] = l
	INT size;				// size of input image
	INT nFeatureNum;		// number of Features
	SDWORD32 *Mean;			// PCA mean vector
	SDWORD32 *PrjMatrix;	// PCA projection matrix
	StModelInfo*  info;
};

// 模型文件头信息
typedef struct
{
	char modelFlag;					// 模型标志	0:SVM 1:NN
	char name[19];					// 模型名
	INT year;						// 年月日，以下3个数字表示版本信息
	INT month;
	INT day;
} StFileHead;

//模型数据块头信息定义
typedef struct
{
	DWORD32 DataCount;					// 数据总数(不包括数据头)

	DWORD32 ParaOff;
	DWORD32 rhoOff;
	DWORD32 nSVOff;
	DWORD32 labelOff;

	DWORD32 NodeOff;					// 以下4个为各个数据块的偏移
	DWORD32 CoefOff;
	DWORD32 MeanOff;
	DWORD32 PrjMatrixOff;

	DWORD32 ModelOff;

	DWORD32 InfoOff;

	DWORD32 Res[9];						// 保留
} SVMFileHead;

typedef struct
{
	DWORD32 DataCount;					// 数据总数(不包括数据头)

	DWORD32 rgiOutput0Off;
	DWORD32 rgiOutput1Off;
	DWORD32 rgiOutput2Off;

	DWORD32 Weight1Off;
	DWORD32 Bias1Off;
	DWORD32 Weight2Off;
	DWORD32 Bias2Off;

	DWORD32 rglayerOff;					// 以下4个为各个数据块的偏移
	DWORD32 rgiLabelOff;
	DWORD32 knetOff;

	DWORD32 InfoOff;
	DWORD32 Res[8];						// 保留
} NNFileHead;

typedef struct
{
	DWORD32 DataCount;					// 数据总数(不包括数据头)

	DWORD32 rgcfCharOff;
	DWORD32 Res[18];					//保留
} TEMPFileHead;

// 模型使用的全局变量

// #、数字、字母、警，其中#、I的模版用数字0的模版填充
const int g_nSymbols = 38;
const int g_cnStdCharWidth = 14;
const int g_cnStdCharHeight = 14;
const int g_cnStdCharLength = g_cnStdCharWidth * g_cnStdCharHeight;

const int g_cnStdChnWidth = 16;
const int g_cnStdChnHeight = 16;
const int g_cnStdChnLength = g_cnStdChnWidth * g_cnStdChnHeight;

const int g_cnLogoChnWidth = 24;
const int g_cnLogoChnHeight = 24;
const int g_cnLogoChnLength = g_cnLogoChnWidth * g_cnLogoChnHeight;
const int g_cnNomalizedMean = 80;	// for image normalized
const int g_cnNomalizedStdVar = 128;

// 字符集定义
extern const WORD16 g_wszCharSet[];			// 字符集合
extern const int g_cnChars ;				// 字符集合长度
extern int g_nDagJun_BCEGHJKLNSVZ_;

// 模型函数集
// 分发
void Distribute(
	const SDWORD32* pfBuf,
	int nWidth, int nHeight, int nOffset,
	SDWORD32& fMean,
	SDWORD32& fVar
);
// 分发
void DistributeGray(
	const SDWORD32* pfBuf,
	int nWidth, int nHeight, int nOffset,
	SDWORD32& fMean,
	SDWORD32& fVar
);
// 计算水平中心
SDWORD32 GetHorizonCenter(
	const SDWORD32* pfSample,
	int nWidth, int nHeight);

// SVM 核心求值
SDWORD32 kernelEvaluation(
	const SDWORD32 *x,
	const SDWORD32 *y,
	const svm_parameter* param,
	const int nFeatureNum
);


// 可信度模板表
extern SDWORD32 * g_rgcfCharTemplate;

#endif // _MODEL_INCLUDED__
