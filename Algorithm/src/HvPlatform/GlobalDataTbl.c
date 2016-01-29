#include "globaldatatbl.h"
#include "math.h"
#include "string.h"

#define MAX_INT(x, y)	(x) > (y) ? (x) : (y)
#define robust_f2(a, x, c)				\
	{									\
		(a) = (x) * (x) * (c) + 0.1f;	\
		if ((a) > 1.0f)					\
		{								\
			(a) = 1.0f;					\
		}								\
	}

#if RUN_PLATFORM != PLATFORM_LINUX

#ifndef CHIP_8127
int round(double val) {
	int iResult;

	iResult = ( int )val;
	val -= iResult;
	if ( val >= 0.5 ) return iResult + 1;
	if ( val <= -0.5 ) return iResult - 1;
	return iResult;
}
#else
	double round(double v);
#endif

#else
    double round(double v);
#endif

void InitSqrtTable()
{
	int i;

	//实始化平方根表.对平方根表进行初始化,为后面检测框方差求方根来使用的.
	for ( i = 0; i < MAX_SQRT_NUM; i ++ )
	{
		int iValue = (int)(sqrt((float)i) + 0.5);
		sqrtTable[i] = (iValue > 255) ? 255 : iValue;
	}
}

void InitExpTable()
{
	int i;

	// 初始化Exp的LUT
	for (i = 0; i < EXP_TABLE_LEN; i++) {
		double dbValue = exp((double)i / MATH_QUAN_SCALE);
		wLowBit[i] = (int)(dbValue * MATH_QUAN_SCALE + 0.5);
	}

	// 初始化Exp的LUT
	for (i = 0; i < EXP_TABLE_LEN; i++) {
		double dbValue = exp((double)i / MATH_QUAN_SCALE);
		wLowBit[i] = (int)(dbValue * MATH_QUAN_SCALE + 0.5);
	}
	for (i = 0;i < EXP_TABLE_LEN;i++) {
		double dbValue = exp((((double)i * EXP_TABLE_LEN) / MATH_QUAN_SCALE));
		QWORD64 qwValue = (QWORD64)(dbValue * MATH_QUAN_SCALE + 0.5);
		wHighBit[i] = (qwValue > 0xffffffff) ? 0xffffffff: (DWORD32)qwValue;
	}
	for (i = 0; i < EXP_TABLE_LEN; i++) {
		double dbValue = exp((double)-i / MATH_QUAN_SCALE);
		wLowBitN[i] = (int)(dbValue * MATH_QUAN_SCALE + 0.5);
	}
	for (i = 0;i < EXP_TABLE_LEN;i++) {
		double dbValue = exp((((double)-i * EXP_TABLE_LEN) / MATH_QUAN_SCALE));
		wHighBitN[i] = (int)(dbValue * MATH_QUAN_SCALE + 0.5);
	}

	// 初始化杨舸的exp表
	for (i = -(EXP_TABLE2_LEN - 1); i <= 0; i++) {
		waExpValue[EXP_TABLE2_LEN + i - 1] = round( 32768 * exp( (double)i / 1024.0f ));
	}
}

// 计算各种车牌空格宽度
void InitSpaceWidth()
{
	float fSpace1, fSpace2;
	int iSum, iChar;
	int iWidth;

	// baWJSpaceWidth 数组(武警牌上各个空格的宽度)的内容
	for (iWidth = MIN_CHAR_WIDTH; iWidth <= MAX_CHAR_WIDTH; iWidth ++ )
	{
		fSpace2 = (float)WJ_SPACE2_WIDTH * iWidth;
		iSum = round( fSpace2 );
		baWJSpaceWidth[iWidth - MIN_CHAR_WIDTH][0] = iSum;

		fSpace1 = (float)WJ_SPACE1_WIDTH * iWidth;
		for (iChar = 1; iChar < PLATE_CHAR_NUM - 1; iChar ++ ) {
			fSpace1 += fSpace2;
			baWJSpaceWidth[iWidth - MIN_CHAR_WIDTH][iChar] = MAX_INT( 1, round( fSpace1 ) - iSum );
			iSum += MAX_INT( 1, round( fSpace1 ) - iSum );
		}
		fSpace1 += fSpace2 / 2.0f;
		baWJSpaceWidth[iWidth - MIN_CHAR_WIDTH][iChar] = MAX_INT( 1, round( fSpace1 ) - iSum );

		iSum += MAX_INT( 1, round( fSpace1 ) - iSum );
		baWJSpaceWidth[iWidth - MIN_CHAR_WIDTH][iChar + 1] = iSum;
	}

	// baNormalSpaceWidth 数组(普通牌上各个空格的宽度)的内容
	for ( iWidth = MIN_CHAR_WIDTH; iWidth <= MAX_CHAR_WIDTH; iWidth ++ )
	{
		fSpace2 = NORMAL_SPACE2_WIDTH * iWidth;
		iSum = round( fSpace2 );
		baNormalSpaceWidth[iWidth - MIN_CHAR_WIDTH][0] = iSum;

		fSpace1 = NORMAL_SPACE1_WIDTH * iWidth;
		for ( iChar = 1; iChar < PLATE_CHAR_NUM - 1; iChar ++ ) {
			fSpace1 += fSpace2;
			baNormalSpaceWidth[iWidth - MIN_CHAR_WIDTH][iChar] = MAX_INT( 1, round( fSpace1 ) - iSum );
			iSum += MAX_INT( 1, round( fSpace1 ) - iSum );
		}
		fSpace1 += fSpace2 / 2.0f;
		baNormalSpaceWidth[iWidth - MIN_CHAR_WIDTH][iChar] = MAX_INT( 1, round( fSpace1 ) - iSum );
		iSum += MAX_INT( 1, round( fSpace1 ) - iSum );
		baNormalSpaceWidth[iWidth - MIN_CHAR_WIDTH][iChar + 1] = iSum;
	}

	// baPoliceSpaceWidth 数组(正规警牌上各个空格的宽度)的内容
	for ( iWidth = MIN_CHAR_WIDTH; iWidth <= MAX_CHAR_WIDTH; iWidth ++ )
	{
		fSpace1 = NORMAL_SPACE1_WIDTH * iWidth;
		iSum = round( fSpace1 );
		baPoliceSpaceWidth[iWidth - MIN_CHAR_WIDTH][0] = iSum;

		fSpace2 = NORMAL_SPACE2_WIDTH * iWidth;
		for ( iChar = 1; iChar < PLATE_CHAR_NUM - 1; iChar ++ ) {
			fSpace1 += fSpace2;
			baPoliceSpaceWidth[iWidth - MIN_CHAR_WIDTH][iChar] = MAX_INT( 1, round( fSpace1 ) - iSum );
			iSum += MAX_INT( 1, round( fSpace1 ) - iSum );
		}
		fSpace1 += fSpace2 / 2.0f;
		baPoliceSpaceWidth[iWidth - MIN_CHAR_WIDTH][iChar] = MAX_INT( 1, round( fSpace1 ) - iSum );
		iSum += MAX_INT( 1, round( fSpace1 ) - iSum );
		baPoliceSpaceWidth[iWidth - MIN_CHAR_WIDTH][iChar + 1] = iSum;
	}
	for ( iWidth = MIN_CHAR_WIDTH; iWidth <= MAX_CHAR_WIDTH; iWidth ++ )
	{
		iSum = 0;
		for( iChar=0;iChar<=2;iChar++)
		{
			baShiGuanSpaceWidth[iWidth - MIN_CHAR_WIDTH][iChar] =  MAX_INT(1,baNormalSpaceWidth[iWidth - MIN_CHAR_WIDTH][iChar+2] - 1);
			iSum += baShiGuanSpaceWidth[iWidth - MIN_CHAR_WIDTH][iChar];
		}
		baShiGuanSpaceWidth[iWidth - MIN_CHAR_WIDTH][3] =  MAX_INT(1,baNormalSpaceWidth[iWidth - MIN_CHAR_WIDTH][1] - 1);
		iSum += baShiGuanSpaceWidth[iWidth - MIN_CHAR_WIDTH][3];
		baShiGuanSpaceWidth[iWidth - MIN_CHAR_WIDTH][4] =  MAX_INT(1,baNormalSpaceWidth[iWidth - MIN_CHAR_WIDTH][0] - 1);
		iSum += baShiGuanSpaceWidth[iWidth - MIN_CHAR_WIDTH][4];
		for(iChar=5;iChar<7;iChar++)
		{
			baShiGuanSpaceWidth[iWidth - MIN_CHAR_WIDTH][iChar] =  MAX_INT(1,baNormalSpaceWidth[iWidth - MIN_CHAR_WIDTH][iChar] - 1);
			iSum += baShiGuanSpaceWidth[iWidth - MIN_CHAR_WIDTH][iChar];
		}
		baShiGuanSpaceWidth[iWidth - MIN_CHAR_WIDTH][7] = iSum;
	}
}

// (倾斜时车牌上各个字在垂直方向上的位移)的内容
void InitSlantPixel()
{
	int iWidth, iHigh, iChar;
	float fSlantRatio[ MAX_SLANT_NUM ] = { 0.15f / 25.0f, -0.15f / 25.0f };

	for ( iHigh = MIN_SEG_IMG_HIGH; iHigh <= MAX_SEG_IMG_HIGH; iHigh ++ )
	{
		for ( iWidth = 0; iWidth < MAX_SLANT_NUM; iWidth ++ ) {
			for ( iChar = 1; iChar < PLATE_CHAR_NUM - 1; iChar ++ )
			{
				sbaSlantPixel[iHigh - MIN_SEG_IMG_HIGH][iWidth][iChar - 1] = round(iChar * iHigh * fSlantRatio[iWidth]);
			}
			sbaSlantPixel[iHigh - MIN_SEG_IMG_HIGH][iWidth][iChar - 1] =
								round((PLATE_CHAR_NUM - 1) * iHigh * fSlantRatio[iWidth]);
		}
	}
}

void InitScoreLUT()
{
	float fltTemp;
	float a;
	int iValue, i;
	for(i = 0; i < 256; i++)
	{
		fltTemp = ((float)i)/255;

		// ("字"边缘图评分表)的内容
		robust_f2(a, fltTemp, 1);
		iValue = round(1000000 * (1 + log10(a)));
		iaCharEdgeScoreLUT[i] = iValue;

		// ("字"黑白图评分表)的内容
		robust_f2(a, fltTemp, 1);
		iValue = round(1000000 * (1 + log10(a)));
		iaCharBinScoreLUT[i] = iValue;

		// ("空格"Hue图评分表)的内容
		robust_f2(a, fltTemp, 1);
		iValue = round(1000000 * (1 + log10(a)));
		iaSpaceHueScoreLUT[i] = iValue;

		fltTemp = 1 - fltTemp;

		// (Bar黑白图评分表)的内容
		robust_f2(a, fltTemp * fltTemp, 1);
		iValue = round(1000000 * (1 + log10(a)));
		iaBarBinScoreLUT[i] = iValue;

		// ("空格"黑白图评分表)的内容
		robust_f2(a, fltTemp * fltTemp, 1);
		iValue = round(1000000 * (1 + log10(a)));
		iaSpaceBinScoreLUT[i] = iValue;

		// (Bar边缘图评分表)的内容
		robust_f2(a, fltTemp * fltTemp, 1);
		iValue = round(1000000 * (1 + log10(a)));
		iaBarEdgeScoreLUT[i] = iValue;
	}
}

// 初始化Hue颜色表
void InitHueColorTable()
{
	int i;
	for (i = 0; i < 20; i++)
	{
		g_hueColorTable[i] = 4;					// 红色
	}
	for (i = 20; i < 60; i++)
	{
		g_hueColorTable[i] = 1;					// 黄色
	}
	for (i = 60; i < 100; i++)
	{
		g_hueColorTable[i] = 5;					// 绿色
	}
	for (i = 100; i < 190; i++)
	{
		g_hueColorTable[i] = 0;					// 蓝色
	}
	for (i = 190; i < 241; i++)
	{
		g_hueColorTable[i] = 4;					// 红色
	}
}

void GlobleDataSrcInit( void ) {
	static int flag = 0;

	if ( flag ) return;

	// 初始化平方根表
	InitSqrtTable();
	// 初始化两种Exp表
	InitExpTable();
	// 计算各种车牌空格宽度
	InitSpaceWidth();
	// (倾斜时车牌上各个字在垂直方向上的位移)的内容
	InitSlantPixel();
	// 计算各个评分表
	InitScoreLUT();
	// 初始化Hue颜色表
	InitHueColorTable();

	flag = 1;
}

