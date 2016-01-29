/**
* @file  swpa_camera.c
* @brief  相机相关
* @author luoqz@signalway.com.cn
* @date  2013-3-21
* @version  V0.1
* @note 原算法实现者为邓庭祥，本次主要是将算法从原设备代码中抽取出来，改成C语言语法，并把4种颜色参数合并到一个接口传入
*/

#include <math.h>
#include <string.h>

#ifndef MIN
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#endif

#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// CONTRAST_TYPE = 对比度，SATURATION_BYPE=饱和度，HUE_TYPE=色度
// COLORTEMP_TYPE=色温,MAXCOLOR_MATRIX矩阵大小
enum{ MAXCOLOR_MATRIX = 4, CONTRAST_TYPE , SATURATION_BYPE , HUE_TYPE , COLORTEMP_TYPE};

struct MARTRIX_DATA{
    float fColorMatrixResult[MAXCOLOR_MATRIX][MAXCOLOR_MATRIX];
};
typedef struct COLORMATRIX_st{
    struct MARTRIX_DATA arrMatrixData;
};

void ClearMatrix(struct COLORMATRIX_st * matrix){
	int i = 0;
    for( i = 0 ;i < MAXCOLOR_MATRIX; i++){
        memset( &matrix->arrMatrixData.fColorMatrixResult[i][0] , 0.0 , sizeof(float)*MAXCOLOR_MATRIX);
        matrix->arrMatrixData.fColorMatrixResult[i][i] = 1.0;
    }
}
void CopyMatrixData(struct COLORMATRIX_st * dest, struct MARTRIX_DATA *parrMatrixData ){
    memcpy( &dest->arrMatrixData , parrMatrixData , sizeof( struct MARTRIX_DATA ));
}

// 对比度常量矩阵
// note: 最后的10.2是加上去的，原来没有
static const double DELTA_INDEX[] =
{
    0, 0.01, 0.02, 0.04, 0.05, 0.06, 0.07, 0.08, 0.1,  0.11,
    0.12, 0.14, 0.15, 0.16, 0.17, 0.18, 0.20, 0.21, 0.22, 0.24,
    0.25, 0.27, 0.28, 0.30, 0.32, 0.34, 0.36, 0.38, 0.40, 0.42,
    0.44, 0.46, 0.48, 0.5,  0.53, 0.56, 0.59, 0.62, 0.65, 0.68,
    0.71, 0.74, 0.77, 0.80, 0.83, 0.86, 0.89, 0.92, 0.95, 0.98,
    1.0,  1.06, 1.12, 1.18, 1.24, 1.30, 1.36, 1.42, 1.48, 1.54,
    1.60, 1.66, 1.72, 1.78, 1.84, 1.90, 1.96, 2.0,  2.12, 2.25,
    2.37, 2.50, 2.62, 2.75, 2.87, 3.0,  3.2,  3.4,  3.6,  3.8,
    4.0,  4.3,  4.7,  4.9,  5.0,  5.5,  6.0,  6.5,  6.8,  7.0,
    7.3,  7.5,  7.8,  8.0,  8.4,  8.7,  9.0,  9.4,  9.6,  9.8,
    10.0, 10.2
};

/** 对比度计算
* @param nValue对比度阀值 ，objMatrix原算保存结果
*/
void ContrastAlgorithm( int nValue , struct COLORMATRIX_st *objMatrix)
{
    float x = 0.0;
	struct MARTRIX_DATA cMat;
    if( nValue < 0 ) x = 127 + nValue / 100.0*127;
    else{
        x = nValue / 100.0;
        if( x == 0 ) x = DELTA_INDEX[nValue];
        else x = DELTA_INDEX[nValue]*(1 - x) + DELTA_INDEX[nValue + 1]*x;
        x = x*127 + 127;
    }

    cMat.fColorMatrixResult[0][0] = x/127; cMat.fColorMatrixResult[0][1] = 0; cMat.fColorMatrixResult[0][2] = 0; cMat.fColorMatrixResult[0][3] = 0.5*(127-x);
    cMat.fColorMatrixResult[1][0] = 0; cMat.fColorMatrixResult[1][1] = x/127; cMat.fColorMatrixResult[1][2] = 0; cMat.fColorMatrixResult[1][3] = 0.5*(127-x);
    cMat.fColorMatrixResult[2][0] = 0; cMat.fColorMatrixResult[2][1] = 0; cMat.fColorMatrixResult[2][2] = x/127; cMat.fColorMatrixResult[2][3] = 0.5*(127-x);
    cMat.fColorMatrixResult[3][0] = 0; cMat.fColorMatrixResult[3][1] = 0; cMat.fColorMatrixResult[3][2] = 0; cMat.fColorMatrixResult[3][3] = 1;

	CopyMatrixData(objMatrix, & cMat );
}

/** 饱和度
* @param nValue对比度阀值 ，objMatrix原算保存结果
*/
void SaturationAlgorithm( int nValue , struct COLORMATRIX_st *objMatrix)
{
	float x = 1+((nValue > 0) ? (3*nValue/100.0) : (nValue/100.0));

    const float lumR = 0.3086;
	const float lumG = 0.6094;
	const float lumB = 0.0820;
	struct MARTRIX_DATA cMat =
	{
		{
			{lumR*(1-x)+x, lumG*(1-x), lumB*(1-x), 0},
			{lumR*(1-x), lumG*(1-x)+x, lumB*(1-x), 0},
			{lumR*(1-x), lumG*(1-x), lumB*(1-x)+x, 0},
			{0, 0, 0, 1}
		}
	};

	CopyMatrixData(objMatrix, & cMat );
}

/** 色度
* @param nValue对比度阀值 ，objMatrix原算保存结果
* 注意：nValue的值范围为-180~180与其它值不一样
*/
void HuAlgorithm( int nValue , struct COLORMATRIX_st *objMatrix)
{
	float fltAngle = nValue / 180.0 * 3.1415926;

	float cosVal = cosf(fltAngle);
	float sinVal = sinf(fltAngle);
	const float lumR = 0.213;
	const float lumG = 0.715;
	const float lumB = 0.072;

	struct MARTRIX_DATA cMat =
	{
		{
			lumR+cosVal*(1-lumR)+sinVal*(-lumR), lumG+cosVal*(-lumG)+sinVal*(-lumG), lumB+cosVal*(-lumB)+sinVal*(1-lumB), 0,
			lumR+cosVal*(-lumR)+sinVal*(0.143), lumG+cosVal*(1-lumG)+sinVal*(0.140), lumB+cosVal*(-lumB)+sinVal*(-0.283), 0,
			lumR+cosVal*(-lumR)+sinVal*(-(1-lumR)), lumG+cosVal*(-lumG)+sinVal*(lumG), lumB+cosVal*(1-lumB)+sinVal*(lumB), 0,
			0, 0, 0, 1
		}
	};

	CopyMatrixData(objMatrix, & cMat );
}

// 色温保存对象
struct COLORTEMPQUERY_TABLE
{
    int  nIndex;
    unsigned int uR;
    unsigned int uG;
    unsigned int uB;
};
static struct COLORTEMPQUERY_TABLE fColortempQueryTable[] =
{
    {  32 , 255 , 187 , 120  }, // 3200K
    {  40 , 255 , 209 , 163  }, // 4000k
    {  52 , 255 , 232 , 213  }, // 5200K
    {  55 , 255 , 236 , 224  }, // 5500k
    {  60 , 255 , 243 , 239  }, // 6000K
    {  70 , 255 , 243 , 255  }, // 7000k
    {  -1 , 0   , 0   , 0  }    // 结束
};

/** 色温
* @param nValue对比度阀值 ，objMatrix原算保存结果
*/
int ColorTemperatureAlgorithm( int nValue , struct COLORMATRIX_st * objMatrix )
{
	struct MARTRIX_DATA cMat;
	float fFactorR = 0;
	float fFactorG = 0;
	float fFactorB = 0;
	int isQuery = 0;
    struct COLORTEMPQUERY_TABLE *pColorTemp = fColortempQueryTable;
    //printf( "pColorTempValue = %d" , nValue );
    while( pColorTemp->nIndex > -1 )
    {
        if( nValue == pColorTemp->nIndex ){ isQuery = 1; break;}
        pColorTemp++;
    }
    // 查不到直接返回
    if( isQuery == 0 ) return -1;

    fFactorR = 255.0 / pColorTemp->uR ;
    fFactorG = 255.0 / pColorTemp->uG ;
    fFactorB = 255.0 / pColorTemp->uB ;

    cMat.fColorMatrixResult[0][0] = fFactorR; cMat.fColorMatrixResult[0][1] = 0; cMat.fColorMatrixResult[0][2] = 0; cMat.fColorMatrixResult[0][3] = 0;
    cMat.fColorMatrixResult[1][0] = 0; cMat.fColorMatrixResult[1][1] = fFactorG; cMat.fColorMatrixResult[1][2] = 0; cMat.fColorMatrixResult[1][3] = 0;
    cMat.fColorMatrixResult[2][0] = 0; cMat.fColorMatrixResult[2][1] = 0; cMat.fColorMatrixResult[2][2] = fFactorB; cMat.fColorMatrixResult[2][3] = 0;
    cMat.fColorMatrixResult[3][0] = 0; cMat.fColorMatrixResult[3][1] = 0; cMat.fColorMatrixResult[3][2] = 0; cMat.fColorMatrixResult[3][3] = 1;

	CopyMatrixData(objMatrix, & cMat );
	
	return 0;
}

void MultiplyMatrixMerger( struct COLORMATRIX_st * objOutMatrix , const struct COLORMATRIX_st * objInputMatrix)
{
	int i = 0, j = 0, k = 0;
	float fltSum = 0;
    // 复制原有数据
	struct COLORMATRIX_st temp; CopyMatrixData(&temp, &objOutMatrix->arrMatrixData );

	for (i = 0; i < MAXCOLOR_MATRIX; i++) //
	{
		for(j = 0; j < MAXCOLOR_MATRIX; j++) //
		{
            fltSum = 0;
			for(k = 0; k < MAXCOLOR_MATRIX; k++)
			{
				fltSum += objOutMatrix->arrMatrixData.fColorMatrixResult[i][k] * objInputMatrix->arrMatrixData.fColorMatrixResult[k][j];
			}

			temp.arrMatrixData.fColorMatrixResult[i][j] = fltSum;
		}
	}

    // 最终数据输出
	CopyMatrixData(objOutMatrix, &temp.arrMatrixData );
}

// 颜色矩阵合并算法
int ColorMartrixIntegrateAlg(int nContrast, int nSaturation, int nHue, int nColorTemperature, int nOutput[MAXCOLOR_MATRIX - 1][MAXCOLOR_MATRIX])
{
    int index = 0;
	int i = 0, j = 0;
	int nResult = 0;
	int nColorTemperatureResult = 0;
    // 对比度
    struct COLORMATRIX_st fContrastMartrix;
    // 饱和度
    struct COLORMATRIX_st fSaturationMartrix;
    // 色度
    struct COLORMATRIX_st fHueMartrix;
    // 色温
    struct COLORMATRIX_st fColorTemperatureMartrix;

    // 4X4矩阵运算保存结果叠加
    struct COLORMATRIX_st fColorMartrixObject;

    ContrastAlgorithm( nContrast , &fContrastMartrix );
    SaturationAlgorithm(nSaturation , &fSaturationMartrix);
    HuAlgorithm( nHue , &fHueMartrix );
    nColorTemperatureResult = ColorTemperatureAlgorithm( nColorTemperature , &fColorTemperatureMartrix );

    // 矩阵合并,先清理旧数据
    ClearMatrix(&fColorMartrixObject);
    // 对比度合并
    MultiplyMatrixMerger( &fColorMartrixObject, &fContrastMartrix);
     // 饱和度合并
    MultiplyMatrixMerger( &fColorMartrixObject, &fSaturationMartrix);
     // 色度合并
    MultiplyMatrixMerger( &fColorMartrixObject, &fHueMartrix);
     // 色温合并
    if (nColorTemperatureResult == 0) // 如果是-1则不需合并色温
    {
	    MultiplyMatrixMerger( &fColorMartrixObject, &fColorTemperatureMartrix);
	}

    // 输出结果
    for( i = 0 ; i < (MAXCOLOR_MATRIX - 1); i++)
    {
        for( j = 0 ; j < MAXCOLOR_MATRIX;j++)
        {
            nResult = 0;
            if( j < (MAXCOLOR_MATRIX - 1)){
                nResult = 1024*fColorMartrixObject.arrMatrixData.fColorMatrixResult[i][j];
                // fix: 范围：17位有符号数
                nResult = MAX(-65536, nResult);
                nResult = MIN(nResult, 65535);

            }
            else{
                nResult = fColorMartrixObject.arrMatrixData.fColorMatrixResult[i][j];
                nResult = MAX(-1024, nResult);
                nResult = MIN(nResult, 1023);
            }

            // 左移动4位，腾出空间存储下脚标
            nResult = (nResult << 4);
            // 下脚标
            nResult |= index++;

			nOutput[i][j] = nResult;
        }
    }

    return 0;
}

#ifdef __cplusplus
}
#endif

