#include "hvutils.h"

#include "recogmodel.h"

//原始模型数据

//TEMPLATE
extern SDWORD32 g_rgcfCharTemplateData[];

// 量化SVM模型
extern svm_model Model_WJ_14;				// 武警数字模型(2005.09.07)

// 量化NN模型模型
extern NET knetChnSF;				// 省份汉字模型
extern NET knetChnFull;             //全汉字模型
extern NET knetAll;
extern NET knetAlpha;
extern NET knetDigit;
extern NET knetDigitJ;
extern NET knetDigitGua;
extern NET knetGA;
extern NET knetChnFullMT;			// 摩托汉字模型
extern NET knetAlphaMT;				// 摩托字母模型
extern NET knetAllMT;				// 摩托字母模型
extern NET knetColor;               //车身颜色模型
extern NET knetColorNight;

extern NET knetChnAlpha;  
extern NET knetChnAlphaMT;
extern NET knetAlphaH;   //字母航
extern NET knetCarLogo;  //车标
//小模型
extern NET knet0DQCU;
extern NET knet8BH;
extern NET knet5S;			
extern NET knet2Z;
extern NET knetDigit5s;				// 摩托车5/S识别模型
extern NET knetT1;	
//红外模型
extern NET knetChnSF_IR;				// 省份汉字模型
extern NET knetChnFull_IR;             //全汉字模型
extern NET knetAll_IR;
extern NET knetAlpha_IR;
extern NET knetDigit_IR;
extern NET knetChnAlpha_IR;  
extern NET knet0DQCU_IR;
extern NET knet8BH_IR;
extern NET knet5S_IR;	
extern NET knetT1_IR;	
extern NET knetEF_IR;
extern NET knetChnFull_IRGX;
extern NET knetChnSF_IRGX;


// 模型序列号
#define SERIAL_TEMPLATE					31
#define SERIAL_SVM_WJ_14				32

#define SERIAL_NN_CHN_SF_16				33				// 省份汉字模型
#define SERIAL_NN_CHN_FULL_16			34
#define SERIAL_NN_ALL_14				35
#define SERIAL_NN_ALPHA_14				36
#define SERIAL_NN_DIGIT_14				37
#define SERIAL_NN_DIGITJ_16				38
#define SERIAL_NN_DIGITGUA_14			39

#define SERIAL_NN_GA_14					40
#define SERIAL_NN_MT_CHN_FULL_16		41				// 摩托汉字
#define SERIAL_NN_MT_ALPHA_14			42				// 摩托字母
#define SERIAL_NN_MT_ALL_14				43				// 摩托全能
#define SERIAL_NN_Color_4				44				// 颜色
#define SERIAL_NN_Color_NIGHT_4			45

#define SERIAL_NN_CHNALPHA_16           46
#define SERIAL_NN_CHNALPHAMT_16         47 
#define SERIAL_NN_ALPHAH_14             48
#define SERIAL_NN_CARLOGO_16            49
//小模型
#define SERIAL_NN_0DQCU_14				50
#define SERIAL_NN_8BH_14				51
#define SERIAL_NN_5S_14					52
#define SERIAL_NN_2Z_14                 53
#define SERIAL_NN_T1_14					54
#define SERIAL_NN_DIGITJ5S_14			55

//红外
#define SERIAL_NN_CHN_SF_IR_16				56				// 省份汉字模型
#define SERIAL_NN_CHN_FULL_IR_16			57
#define SERIAL_NN_ALL_IR_14				    58
#define SERIAL_NN_ALPHA_IR_14				59
#define SERIAL_NN_DIGIT_IR_14				60
#define SERIAL_NN_CHNALPHA_IR_16            61

#define SERIAL_NN_0DQCU_IR_14				62
#define SERIAL_NN_8BH_IR_14				    63
#define SERIAL_NN_5S_IR_14					64
#define SERIAL_NN_T1_IR_14					65
#define SERIAL_NN_EF_IR_14                  66					
#define SERIAL_NN_CHN_FULL_IR_16_GX		    67
#define SERIAL_NN_CHN_SF_IR_16_GX		    68



//信息
typedef struct _RECOG_MODEL_INFO
{
	DWORD32 dwType;
	const char* szName;
	void* pModel;
	DWORD32 dwFileID;
} RECOG_MODEL_INFO;

static RECOG_MODEL_INFO g_rgRecogModelInfo[] =
{
	{TYPE_TEMP,	"TEMPLATE",				    g_rgcfCharTemplateData,	SERIAL_TEMPLATE	},
	{TYPE_SVM,		"SVM_WJ",				&Model_WJ_14,				SERIAL_SVM_WJ_14},

	{TYPE_NN,		"NN_CHNSF",			    &knetChnSF,						SERIAL_NN_CHN_SF_16},
	{TYPE_NN,		"NN_CHNFULL",			&knetChnFull,					SERIAL_NN_CHN_FULL_16},
	{TYPE_NN,		"NN_ALL",				&knetAll,						SERIAL_NN_ALL_14},
	{TYPE_NN,		"NN_ALPHA",				&knetAlpha,						SERIAL_NN_ALPHA_14},
	{TYPE_NN,		"NN_DIGIT",				&knetDigit,				        SERIAL_NN_DIGIT_14},
	{TYPE_NN,		"NN_DIGITJ",			&knetDigitJ,					SERIAL_NN_DIGITJ_16},
	{TYPE_NN,		"NN_DIGITGUA",		    &knetDigitGua,					SERIAL_NN_DIGITGUA_14},
	{TYPE_NN,		"NN_GA",				&knetGA,					    SERIAL_NN_GA_14	},
	{TYPE_NN,		"NN_CHNFULLMT",			&knetChnFullMT,					SERIAL_NN_MT_CHN_FULL_16},
	{TYPE_NN,		"NN_ALPHAMT",			&knetAlphaMT,					SERIAL_NN_MT_ALPHA_14},
	{TYPE_NN,		"NN_ALLMT",				&knetAllMT,						SERIAL_NN_MT_ALL_14	},
	//车身颜色
	{TYPE_NN,		"NN_COLOR",				&knetColor,					    SERIAL_NN_Color_4},
	{TYPE_NN,		"NN_COLORNIGHT",		&knetColorNight,			    SERIAL_NN_Color_NIGHT_4},

	{TYPE_NN,		"NN_CHNALPHA",			&knetChnAlpha,				    SERIAL_NN_CHNALPHA_16},
	{TYPE_NN,		"NN_CHNALPHAMT",		&knetChnAlphaMT,				SERIAL_NN_CHNALPHAMT_16	},
	{TYPE_NN,		"NN_ALPHAH",			&knetAlphaH,				    SERIAL_NN_ALPHAH_14	},
	{TYPE_NN,		"NN_CarLogo",			&knetCarLogo,			        SERIAL_NN_CARLOGO_16},
	//小模型
	{TYPE_NN,		"NN_8BH",				&knet8BH,				        SERIAL_NN_8BH_14},
	{TYPE_NN,		"NN_0DQCU",				&knet0DQCU,				        SERIAL_NN_0DQCU_14},
	{TYPE_NN,		"NN_DIGIT5S",			&knetDigit5s,					SERIAL_NN_DIGITJ5S_14},
	{TYPE_NN,		"NN_T1",				&knetT1,						SERIAL_NN_T1_14	},
	{TYPE_NN,		"NN_5S",			    &knet5S,			            SERIAL_NN_5S_14},
    {TYPE_NN,		"NN_2Z",				&knet2Z,				        SERIAL_NN_2Z_14	},
	
     //红外
	{TYPE_NN,		"NN_CHNSF_IR",			    &knetChnSF_IR,						SERIAL_NN_CHN_SF_IR_16},
	{TYPE_NN,		"NN_CHNFULL_IR",			&knetChnFull_IR,					SERIAL_NN_CHN_FULL_IR_16},
	{TYPE_NN,		"NN_ALL_IR",				&knetAll_IR,						SERIAL_NN_ALL_IR_14},
	{TYPE_NN,		"NN_ALPHA_IR",				&knetAlpha_IR,						SERIAL_NN_ALPHA_IR_14},
	{TYPE_NN,		"NN_DIGIT_IR",				&knetDigit_IR,				        SERIAL_NN_DIGIT_IR_14},
	{TYPE_NN,		"NN_CHNALPHA_IR",			&knetChnAlpha_IR,				    SERIAL_NN_CHNALPHA_IR_16},
	{TYPE_NN,		"NN_8BH_IR",				&knet8BH_IR,				        SERIAL_NN_8BH_IR_14},
	{TYPE_NN,		"NN_0DQCU_IR",				&knet0DQCU_IR,				        SERIAL_NN_0DQCU_IR_14},
	{TYPE_NN,		"NN_5S_IR",			        &knet5S_IR,			                SERIAL_NN_5S_IR_14},
	{TYPE_NN,		"NN_T1_IR",				    &knetT1_IR,						    SERIAL_NN_T1_IR_14	},
	{TYPE_NN,		"NN_EF_IR",			        &knetEF_IR,			                SERIAL_NN_EF_IR_14	},
	{TYPE_NN,		"NN_CHNFULL_IRGX",	        &knetChnFull_IRGX,	                SERIAL_NN_CHN_FULL_IR_16_GX},
	{TYPE_NN,		"NN_CHNSF_IRGX",		    &knetChnSF_IRGX,		            SERIAL_NN_CHN_SF_IR_16_GX	},

};

#define RECOG_MODEL_NUM ARRSIZE(g_rgRecogModelInfo)

RECOG_MODEL_INFO* GetRecogModelInfo( const char* szName )
{
	RECOG_MODEL_INFO* pModelInfo(NULL);

	for (int i = 0; i < RECOG_MODEL_NUM; i++)
	{
		if ( strcmp(szName, g_rgRecogModelInfo[i].szName) == 0 )
		{
			pModelInfo = &g_rgRecogModelInfo[i];
			break;
		}
	}

	return pModelInfo;
}

int GetRecogFileID( const char* szName)
{
	RECOG_MODEL_INFO* pModelInfo = GetRecogModelInfo( szName );

	return (pModelInfo == NULL)?-1:(int)pModelInfo->dwFileID;
}

HRESULT GetRecogModelList(LPCSTR* rgName, DWORD32* pdwCount )
{
	if (pdwCount == NULL) return E_POINTER;

	if (rgName == NULL)
	{
		*pdwCount = RECOG_MODEL_NUM;
		return S_FALSE;
	}

	DWORD32 dwCount = 0;
	DWORD32 dwMaxCount = *pdwCount;

	for ( int i = 0; i < RECOG_MODEL_NUM && dwCount < dwMaxCount; i++)
	{
		rgName[dwCount++] = g_rgRecogModelInfo[i].szName;
	}

	*pdwCount = dwCount;

	return S_OK;
}

HRESULT GetTempDat( RECOG_MODEL_INFO* pInfo, void* pDat,  DWORD32* pdwDatLen)
{
	SDWORD32 *tempData;
	BYTE8 *pBegin;
	DWORD32 LastNum;

	StFileHead FileHead;
	TEMPFileHead pTEMP;

	HV_memset(&FileHead,0,sizeof(StFileHead));
	HV_memset(&pTEMP,0,sizeof(TEMPFileHead));

	tempData = (SDWORD32 *)pInfo->pModel;

	// 设置文件头
	FileHead.modelFlag = TYPE_TEMP;
	strcpy(FileHead.name, pInfo->szName);
	FileHead.year = 2005;
	FileHead.month = 5;
	FileHead.day = 1;

	// 取模型数据开始地址
	pBegin = (BYTE8 *)tempData;

	// 计算最后一块数据长度(最后一块是模型结构)
	LastNum = g_cnStdCharLength * g_nSymbols * sizeof(SDWORD32);

	// 填充结构
	pTEMP.rgcfCharOff = 0;
	pTEMP.DataCount = pTEMP.rgcfCharOff + LastNum;

	// 计算数据总长度
	DWORD32 dwDatLen = sizeof(StFileHead) + sizeof(TEMPFileHead) + pTEMP.DataCount;
	if (dwDatLen > *pdwDatLen) return E_FAIL; //提供的写入内存不足
	*pdwDatLen = dwDatLen;

	BYTE8 *bptr = (BYTE8*)pDat;

	HV_memcpy(bptr,  &FileHead, sizeof(StFileHead));
	bptr += sizeof(StFileHead);

	// 生成TEMP数据头数据
	HV_memcpy(bptr, &pTEMP, sizeof(TEMPFileHead));
	bptr += sizeof(TEMPFileHead);

	// 生成模型数据
	HV_memcpy(bptr, pBegin, pTEMP.DataCount);

	return S_OK;
}

HRESULT GetSVMDat( RECOG_MODEL_INFO* pInfo, void* pDat, DWORD32* pdwDatLen)
{
	svm_model *model;
	BYTE8 *pBegin;
	DWORD32 LastNum;

	StFileHead FileHead;
	SVMFileHead pSVM;

	HV_memset(&FileHead,0,sizeof(StFileHead));
	HV_memset(&pSVM,0,sizeof(SVMFileHead));

	model = (svm_model *)pInfo->pModel;

	// 设置文件头
	FileHead.modelFlag = TYPE_SVM;
	strcpy(FileHead.name, pInfo->szName);
	FileHead.year = model->info->year;
	FileHead.month = model->info->month;
	FileHead.day = model->info->day;

	pBegin = (BYTE8 *)(model->param);

	// 计算最后一块数据长度(最后一块是模型结构)
	LastNum = sizeof(svm_model);

	// 填充结构
	pSVM.ParaOff = 0;
	pSVM.rhoOff = (DWORD32)((BYTE8 *)(model->rho) - pBegin);
	pSVM.nSVOff = (DWORD32)((BYTE8 *)(model->nSV) - pBegin);
	pSVM.labelOff = (DWORD32)((BYTE8 *)(model->label) - pBegin);

	pSVM.NodeOff = (DWORD32)((BYTE8 *)(model->SV) - pBegin);
	pSVM.CoefOff = (DWORD32)((BYTE8 *)(model->sv_coef) - pBegin);
	pSVM.MeanOff = (DWORD32)((BYTE8 *)(model->Mean) - pBegin);
	pSVM.PrjMatrixOff = (DWORD32)((BYTE8 *)(model->PrjMatrix) - pBegin);

	pSVM.InfoOff = (DWORD32)((BYTE8 *)(model->info) - pBegin);
	pSVM.ModelOff = (DWORD32)((BYTE8 *)(model) - pBegin);

	pSVM.DataCount = pSVM.ModelOff + LastNum;

	model->param=NULL;
	model->SV=NULL;
	model->sv_coef=NULL;
	model->rho=NULL;
	model->label=NULL;
	model->nSV=NULL;
	model->Mean=NULL;
	model->PrjMatrix=NULL;
	model->info=NULL;

	// 计算数据总长度
	DWORD32 dwDatLen = sizeof(StFileHead) + sizeof(SVMFileHead) + pSVM.DataCount;
	if ( dwDatLen > *pdwDatLen) return E_OUTOFMEMORY;
	*pdwDatLen = dwDatLen;

	BYTE8 *bptr = (BYTE8*)pDat;

	//生成文件头
	HV_memcpy(bptr, &FileHead, sizeof(StFileHead));
	bptr += sizeof(StFileHead);

	// 生成SVM数据头数据
	HV_memcpy(bptr, &pSVM, sizeof(SVMFileHead));
	bptr += sizeof(SVMFileHead);

	// 生成模型数据
	HV_memcpy(bptr, pBegin, pSVM.DataCount);

	return S_OK;
}

HRESULT GetNNDat( RECOG_MODEL_INFO* pInfo, void* pDat, DWORD32* pdwDatLen)
{
	NET *net;
	BYTE8 *pBegin;
	DWORD32 LastNum;

	StFileHead FileHead;
	NNFileHead pNN;

	net = (NET *)pInfo->pModel;

	HV_memset(&FileHead,0,sizeof(StFileHead));
	HV_memset(&pNN,0,sizeof(NNFileHead));

	// 设置文件头
	FileHead.modelFlag = TYPE_NN;
	strcpy(FileHead.name, pInfo->szName);
	FileHead.year = net->info->year;
	FileHead.month = net->info->month;
	FileHead.day = net->info->day;

	// zhaopy
	int iSize = 0;
	int iHeadSize = (sizeof(StFileHead) + sizeof(NNFileHead));
	int iLayer1WSize = (net->Layer[0].Num_Units * net->Layer[1].Num_Units * sizeof(SDWORD32));
	int iLayer1BSize = (net->Layer[1].Num_Units * sizeof(SDWORD32));
	int iLayer2WSize = (net->Layer[1].Num_Units * net->Layer[2].Num_Units * sizeof(SDWORD32));
	int iLayer2BSize = (net->Layer[2].Num_Units * sizeof(SDWORD32));
	int iLayerArraySize = (sizeof(LAYER) * 3);
	int iLabelSize = (sizeof(int) * net->Layer[2].Num_Units);
	int iInfoSize = (sizeof(StModelInfo));
	int iNetSize = (sizeof(NET));    

	pNN.DataCount = (iLayer1WSize + iLayer1BSize + iLayer2WSize + iLayer2BSize + iLayerArraySize + iLabelSize + iInfoSize + iNetSize);
	iSize = (iHeadSize + pNN.DataCount);

	// 填充结构（根据内存存放次序设置偏移地址）
	pNN.rgiOutput0Off = (net->Layer)->Num_Units;      
	pNN.rgiOutput1Off = (net->Layer + 1)->Num_Units;  
	pNN.rgiOutput2Off = (net->Layer + 2)->Num_Units;  

	pNN.Weight1Off = 0;  
	pNN.Bias1Off = iLayer1WSize;     
	pNN.Weight2Off = pNN.Bias1Off +iLayer1BSize; 
	pNN.Bias2Off = pNN.Weight2Off +iLayer2WSize ;      

	pNN.rglayerOff =pNN.Bias2Off + iLayer2BSize;  
	pNN.rgiLabelOff =pNN.rglayerOff + iLayerArraySize; 
	pNN.InfoOff =pNN.rgiLabelOff +iLabelSize ;        
	pNN.knetOff =pNN.InfoOff + iInfoSize;  

	// zhaopy
	DWORD32 dwDatLen = iSize;
	if ( dwDatLen > *pdwDatLen ) return (0x80000000 | iSize);
	*pdwDatLen = dwDatLen;

	BYTE8 *bptr = (BYTE8*)pDat;

	HV_memcpy(bptr, &FileHead, sizeof(StFileHead));
	bptr += sizeof(StFileHead);

	// 生成NN数据头数据
	HV_memcpy(bptr, &pNN, sizeof(NNFileHead));
	bptr += sizeof(NNFileHead);

	// 生成模型数据
	//HV_memcpy(bptr, pBegin, pNN.DataCount);
	//zhaopy
	int iOffset = 0;
	HV_memcpy(bptr + iOffset, (BYTE8*)net->Layer[1].Weight, iLayer1WSize);
	iOffset += iLayer1WSize;
	HV_memcpy(bptr + iOffset, (BYTE8*)net->Layer[1].Bias, iLayer1BSize);
	iOffset += iLayer1BSize;

	HV_memcpy(bptr + iOffset, (BYTE8*)net->Layer[2].Weight, iLayer2WSize);
	iOffset += iLayer2WSize;
	HV_memcpy(bptr + iOffset, (BYTE8*)net->Layer[2].Bias, iLayer2BSize);
	iOffset += iLayer2BSize;

	HV_memcpy(bptr + iOffset, (BYTE8*)net->Layer, iLayerArraySize);
	iOffset += iLayerArraySize;

	HV_memcpy(bptr + iOffset, (BYTE8*)net->nLabel, iLabelSize);
	iOffset += iLabelSize;

	HV_memcpy(bptr + iOffset, (BYTE8*)net->info, iInfoSize);
	iOffset += iInfoSize;

	HV_memcpy(bptr + iOffset, (BYTE8*)net, iNetSize);
	iOffset += iNetSize;
 
	LAYER* pLayer = net->Layer;
	for (int i=0;i<3;i++)
	{
		pLayer[i].Output=NULL;
		pLayer[i].Weight=NULL;
		pLayer[i].Bias=NULL;
	}

	net->Layer=NULL;
	net->InputLayer=NULL;
	net->OutputLayer=NULL;
	net->nLabel=NULL;
	net->info=NULL;


	return S_OK;
}

HRESULT LoadRecogDat(
					 LPCSTR szName,
					 void* pDat,
					 DWORD32* pdwDatLen,
					 DWORD32 dwFlag = 0
					 )
{
	if ( pDat == NULL || pdwDatLen == NULL) return E_POINTER;

	RECOG_MODEL_INFO* pModelInfo = GetRecogModelInfo( szName );

	if (pModelInfo == NULL) return E_FAIL;

	HRESULT hr;

	switch( pModelInfo->dwType)
	{
	case TYPE_TEMP:
		hr = GetTempDat( pModelInfo, pDat, pdwDatLen);
		break;
	case TYPE_SVM:
		hr = GetSVMDat( pModelInfo, pDat, pdwDatLen);
		break;
	case TYPE_NN:
		hr = GetNNDat( pModelInfo, pDat, pdwDatLen);
		break;
	default:
		break;
	}

	return hr;
}
