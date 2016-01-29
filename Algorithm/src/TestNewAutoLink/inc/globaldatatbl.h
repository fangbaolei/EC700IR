#ifndef _GLOBAL_DATA_TBL_H
#define _GLOBAL_DATA_TBL_H

#ifdef __cplusplus
extern "C" {
#endif// def __cplusplus

#include "swBaseType.h"
#include "swWinError.h"
#include "config.h"

#define MAX_SQRT_NUM		65536
extern BYTE8 sqrtTable[];

#define MATH_QUAN_SCALE		65536									//量化因子
#define EXP_TABLE_LEN		1024
extern DWORD32 wLowBit[];
extern DWORD32 wHighBit[];
extern DWORD32 wLowBitN[];
extern DWORD32 wHighBitN[];

#define EXP_TABLE2_LEN		7168
extern WORD16 waExpValue[];

// ProcessPlate 使用的参数及数据
#define MIN_SEG_IMG_HIGH			9
#define MAX_SEG_IMG_HIGH			100
#define MAX_SLANT_NUM				2
#define PLATE_CHAR_NUM				7
#define MIN_CHAR_WIDTH				5
#define MAX_CHAR_WIDTH				50
#define NORMAL_SPACE1_WIDTH			11.0f / 16.0f
#define NORMAL_SPACE2_WIDTH			3.5f / 16.0f
#define WJ_SPACE1_WIDTH				14.5f / 16.5f
#define WJ_SPACE2_WIDTH				3.0f / 16.5f

extern BYTE8 baWJSpaceWidth[][ PLATE_CHAR_NUM + 1 ];
extern BYTE8 baNormalSpaceWidth[][ PLATE_CHAR_NUM + 1 ];
extern BYTE8 baPoliceSpaceWidth[][ PLATE_CHAR_NUM + 1 ];
extern SBYTE8 sbaSlantPixel[][ MAX_SLANT_NUM ][ PLATE_CHAR_NUM - 1 ];

extern int iaBarBinScoreLUT[];
extern int iaBarEdgeScoreLUT[];
extern int iaCharEdgeScoreLUT[];
extern int iaCharBinScoreLUT[];
extern int iaSpaceHueScoreLUT[];
extern int iaSpaceBinScoreLUT[];
extern BYTE8 g_hueColorTable[];				// HUE颜色表

void GlobleDataSrcInit( void );

#ifdef __cplusplus
}
#endif // def __cplusplus

#endif // ndef _GLOBAL_DATA_TBL_H
