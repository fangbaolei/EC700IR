#ifndef __FAST_MATH_H__
#define __FAST_MATH_H__

#include "GlobalDataTbl.h"

#define MIN_INT( int1, int2 )	( ( int1 ) < ( int2 ) ? ( int1 ) : ( int2 ) )
#define MAX_INT( int1, int2 )	( ( int1 ) > ( int2 ) ? ( int1 ) : ( int2 ) )
#define HV_ABS( val )	( ( val ) > 0 ? ( val ) : -( val ) )

#define RANGE_INT( iVal, iMin, iMax )	\
	( ( ( iVal ) > ( iMin ) ) ? ( ( ( iVal ) <= ( iMax ) ) ? ( iVal ) : ( iMax ) ) : ( iMin ) )

#define ROUND_SHR_POSITIVE( Dividend, iShiftRightCount )	\
	( ( ( Dividend ) & ( 1 << ( ( iShiftRightCount ) - 1 ) ) ) ? ( ( Dividend ) >> ( iShiftRightCount ) ) + 1 : ( ( Dividend ) >> ( iShiftRightCount ) ) )

#define ROUND_SHR_NEGATIVE( Dividend, iShiftRightCount )	\
		( -( ( ( -( Dividend ) ) & ( 1 << ( ( iShiftRightCount ) - 1 ) ) ) ? ( ( -( Dividend ) ) >> ( iShiftRightCount ) ) + 1 : ( ( -( Dividend ) ) >> ( iShiftRightCount ) ) ) )

// iShiftRightCount >= 1
#define ROUND_SHR( Dividend, iShiftRightCount )	\
	( ( ( Dividend ) >= 0 ) ? ROUND_SHR_POSITIVE( Dividend, iShiftRightCount ) : ROUND_SHR_NEGATIVE( Dividend, iShiftRightCount ) )

inline int ROUND_DIV_POSITIVE( int iDividend, int iDivisor ) {
	int iRoundDivTemp;

	iRoundDivTemp = ( iDividend << 1 ) / ( iDivisor );
	return ( iRoundDivTemp & 1 ) ? ( ( iRoundDivTemp >> 1 ) + 1 ) : ( iRoundDivTemp >> 1 );
}

inline int ROUND_DIV_NEGATIVE( int iDividend, int iDivisor ) {
	int iRoundDivTemp;

	iRoundDivTemp = ( ( -iDividend ) << 1 ) / ( iDivisor );
	return -( ( iRoundDivTemp & 1 ) ? ( ( iRoundDivTemp >> 1 ) + 1 ) : ( iRoundDivTemp >> 1 ) );
}

inline int ROUND_DIV( int iDividend, int iDivisor ) {
	int iRoundDivTemp;

	iRoundDivTemp = ( iDividend << 1 ) / ( iDivisor );
	if ( iRoundDivTemp >= 0 ) return ( iRoundDivTemp & 1 ) ? ( ( iRoundDivTemp >> 1 ) + 1 ) : ( iRoundDivTemp >> 1 );
	iRoundDivTemp = -iRoundDivTemp;
	return -( ( iRoundDivTemp & 1 ) ? ( ( iRoundDivTemp >> 1 ) + 1 ) : ( iRoundDivTemp >> 1 ) );
}

// EXP_FROM_TABLE( iX ) = round( 1024.0f * exp( iX / 1024.0f ) )
// 本宏的目的是求exp(x), 其中 -7 < x <= 0.
// 通常如果你要算如下表达式: round( K * exp( -Dividend / Divisor ) ), 
// 其中 -7 < -Dividend / Divisor <= 0, K < 65536
//		ROUND_SHR( K * EXP_FROM_TABLE( ( Dividend << EXP_FLOAT_SHR ) / Divisor ), EXP_RESULT_SHR )
#define EXP_FLOAT_SHR				10
#define EXP_RESULT_SHR				15
#define EXP_FROM_TABLE( iX )		( ( iX >= 7168 ) ? 0 : waExpValue[ 7167 - iX ] )

#define SQRT_FROM_TABLE( dwX )		( sqrtTable[ dwX ] )

#ifndef __ASM_FILE__
	//extern WORD16 waExpValue[];
#endif		// #ifndef __ASM_FILE__


//黄学雷定义的量化算法使用的宏和函数
const DWORD32 QuanScale =  65536;			//量化因子
const BYTE8 QuanBit =  16;					//量化位移因子 2的16次方

#define QUAN_NUM(a)		((a) << QuanBit)		//量化一个数，两个量化数相除后也可以用这个宏保持量化
#define DEQUAN_NUM(a)	((a) >> QuanBit)		//取消量化,两个量化数相乘后可以使用这个宏保持量化

//带四舍五入的取消量化
//#define DEQUAN_NUM(a)	(((a) + QuanScale >> 1) >> QuanBit)		//取消量化,两个量化数相乘后可以使用这个宏保持量化

//量化exp
DWORD32 quanExpPositive(DWORD32 dwNum);		//量化exp(正数)
DWORD32 quanExpNegative(DWORD32 dwNum);		//量化exp(负数)

//SDWORD32 quanSqrt(SQWORD64 n);
DWORD32 quanSqrt(DWORD32 dwNum, DWORD32 iShift);

#endif		// #ifndef __FAST_MATH_H__

