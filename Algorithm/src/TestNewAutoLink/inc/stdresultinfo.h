#ifndef _STD_RESULT_INFO
#define _STD_RESULT_INFO

#include "swBaseType.h"
#include "swPlate.h"

typedef struct _LPR_RESULT
{
	BOOL fValid;

	DWORD32 dwFlag;

	HV_RECT rcDet;
	HV_RECT rcImg;

	INT nVDegree;
	INT nHDegree;

	BYTE8 rgCharID[10];
	HV_RECT rgrcChar[7];

	INT rgCharConf[12];
	
	INT nPlateType;
	INT nPlateColor;

	INT nConfidence;

	BYTE8 rgReserved[2048];	//预留空间

	_LPR_RESULT()
	{
		fValid = FALSE;

		dwFlag = 0;

		rcDet.left = 0;
		rcDet.top = 0;
		rcDet.right = 0;
		rcDet.bottom = 0;

		rcImg.left = 0;
		rcImg.top = 0;
		rcImg.right = 0;
		rcImg.bottom = 0;

		nVDegree = 90 * 65536;
		nHDegree = 0;

		nPlateType = PLATE_UNKNOWN;
		nPlateColor = PC_UNKNOWN;

		nConfidence = 0;

		for (int i = 0; i < 10; i++)
		{
			rgCharID[i] = 0;
			memset( &rgrcChar[i], 0, sizeof(HV_RECT) );
		}
	}
}
LPR_RESULT;

typedef struct _APPEND_DAT
{
	DWORD32 dwFlag;
	PVOID pDat;
	UINT nDatLen;
	
	_APPEND_DAT()
	{
		dwFlag = 0;
		pDat = NULL;
		nDatLen = 0;
	}
}
APPEND_DAT;

typedef struct _STD_RESULT
{
	static const int MAX_RESULT = 10;
	static const int MAX_APPEND_DAT = 255;

	DWORD32 dwFlag;
	UINT nResultSize;

	LPR_RESULT rgResult[MAX_RESULT];

	UINT nAppendDatCount;
	APPEND_DAT rgAppendDat[MAX_APPEND_DAT];

	_STD_RESULT()
	{
		dwFlag = 0;
		nAppendDatCount = 0;
		nResultSize = sizeof(LPR_RESULT);
	}
}
STD_RESULT;

//输出二进制类型
#define BIN_STD_RESULT 0x81000000 //标准结果数据
#define BIN_LPR_RESULT 0x82000000 //识别结果数据

//检测模块标志位
//模式
#define TF_DET_FASTMODE 0x10
//类型
#define TF_DET_ALL 0x01
#define TF_DET_BLUE 0x02
#define TF_DET_YELLOW 0x03
#define TF_DET_DYELLOW 0x04
#define TF_DET_WJ 0x05

//颜色测试标志位
#define TF_COLOR_TYPE 0x01

//附加数据有效位
#define APD_VALID 0x80000000

//附加数据类型
#define APD_TYPE_COMPIMG 0x01000000 //分量图, 按Y-Cr-Cb存储

#endif
