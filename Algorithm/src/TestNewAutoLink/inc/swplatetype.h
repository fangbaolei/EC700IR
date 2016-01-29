#ifndef _SWPLATETYPE_H_
#define _SWPLATETYPE_H_

// 车型定义
typedef enum _CAR_TYPE
{
	CT_UNKNOWN = 0,			// 未知车型
	CT_SMALL,				// 小型车
	CT_MID,					// 中型车
	CT_LARGE,				// 大型车
	CT_WALKMAN,				// 行人
	CT_BIKE,				// 非机动车
	CT_VEHICLE,				// 机动车
	CT_COUNT
} CAR_TYPE;

//定义车牌颜色
#define PLATE_COLOR_LIST(MACRO_NAME)						\
	MACRO_NAME(UNKNOWN, "未知")								\
	MACRO_NAME(BLUE, "蓝")									\
	MACRO_NAME(YELLOW, "黄")								\
	MACRO_NAME(BLACK, "黑")									\
	MACRO_NAME(WHITE, "白")									\
	MACRO_NAME(LIGHTBLUE, "浅蓝")							\
	MACRO_NAME(GREEN, "绿")

#define DEFINE_PLATE_COLOR_ENUM(a, b)		PC_##a,

typedef enum {
	PLATE_COLOR_LIST(DEFINE_PLATE_COLOR_ENUM)
	PC_COUNT
} PLATE_COLOR;

//定义车牌类型
#define PLATE_TYPE_LIST(MACRO_NAME)		\
	MACRO_NAME(		\
		UNKNOWN,		\
		"未知类型",		\
		RECOG_CHN,		\
		RECOG_ALPHA,		\
		RECOG_ALL,		\
		RECOG_ALL,		\
		RECOG_DIGIT,		\
		RECOG_DIGIT,		\
		RECOG_DIGIT		\
	)		\
	MACRO_NAME(		\
		NORMAL,		\
		"所有蓝牌，黑牌",		\
		RECOG_CHNSF,		\
		RECOG_ALPHA,		\
		RECOG_ALL,		\
		RECOG_ALL,		\
		RECOG_ALL,		\
		RECOG_ALL,		\
		RECOG_ALL		\
	)		\
	MACRO_NAME(		\
		WJ,		\
		"武警车牌",		\
		RECOG_DIGIT,		\
		RECOG_DIGIT,		\
		RECOG_WJ,		\
		RECOG_DIGIT,		\
		RECOG_DIGIT,		\
		RECOG_DIGIT,		\
		RECOG_DIGIT		\
	)		\
	MACRO_NAME(		\
		POLICE,		\
		"包括所有单行黄牌，及与黄牌格式相同的警车",		\
		RECOG_CHN,		\
		RECOG_ALPHA,		\
		RECOG_ALL,		\
		RECOG_ALL,		\
		RECOG_DIGIT,		\
		RECOG_DIGIT,		\
		RECOG_DIGIT_J		\
	)		\
	MACRO_NAME(		\
		POLICE2,		\
		"辽-A1234-警",		\
		RECOG_CHNSF,		\
		RECOG_ALPHA,		\
		RECOG_ALL,		\
		RECOG_ALL,		\
		RECOG_DIGIT,		\
		RECOG_DIGIT,		\
		RECOG_DIGIT_J		\
	)		\
	MACRO_NAME(		\
		MILITARY,		\
		"军货",		\
		RECOG_MILITARYCHN,		\
		RECOG_ALPHA,		\
		RECOG_DIGIT,		\
		RECOG_DIGIT,		\
		RECOG_DIGIT,		\
		RECOG_DIGIT,		\
		RECOG_DIGIT		\
	)		\
	MACRO_NAME(		\
		DOUBLE_YELLOW,		\
		"双行黄牌，双行白牌",		\
		RECOG_CHNMT,		\
		RECOG_ALPHAMT,		\
		RECOG_ALLMT,		\
		RECOG_ALLMT,		\
		RECOG_DIGIT,		\
		RECOG_DIGIT,		\
		RECOG_DIGITGUA		\
	)		\
	MACRO_NAME(		\
		DOUBLE_MOTO,		\
		"双行摩托牌",		\
		RECOG_CHNMT,		\
		RECOG_ALPHAMT,		\
		RECOG_ALLMT,		\
		RECOG_ALLMT,		\
		RECOG_DIGIT,		\
		RECOG_DIGIT,		\
		RECOG_DIGIT		\
	)		\
	MACRO_NAME(		\
		INDIVIDUAL,		\
		"个性化车牌",		\
		RECOG_ALL,		\
		RECOG_ALL,		\
		RECOG_ALL,		\
		RECOG_ALL,		\
		RECOG_ALL,		\
		RECOG_ALL,		\
		RECOG_ALL		\
	)					\
	MACRO_NAME(			\
		DOUBLE_GREEN,	\
		"双层绿牌",		\
		RECOG_CHNSF,	\
		RECOG_DIGIT,	\
		RECOG_DIGIT,	\
		RECOG_ALL,		\
		RECOG_DIGIT,	\
		RECOG_DIGIT,	\
		RECOG_DIGIT		\
	)					\
	MACRO_NAME(			\
	   DOUBLE_WJ,		\
	   "武警车牌",		\
	   RECOG_DIGIT,		\
	   RECOG_DIGIT,		\
	   RECOG_WJ,		\
	   RECOG_DIGIT,		\
	   RECOG_DIGIT,		\
	   RECOG_DIGIT,		\
	   RECOG_DIGIT		\
   )

#define DEFINE_PLATE_TYPE_ENUM(a, b, r1, r2, r3, r4, r5, r6, r7)		PLATE_##a,

typedef enum {
	PLATE_TYPE_LIST(DEFINE_PLATE_TYPE_ENUM)
	PLATE_TYPE_COUNT
} PLATE_TYPE;

typedef enum
{
	CC_UNKNOWN = 0,			//未知
	CC_WHITE,				//白
	CC_GREY,				//灰 
	CC_BLACK,				//黑 
	CC_RED,					//红
	CC_YELLOW,				//黄
	CC_GREEN,				//绿
	CC_BLUE,				//蓝
	CC_PURPLE,				//紫
	CC_PINK,				//粉
	CC_BROWN,				//棕
	CC_COUNT
} CAR_COLOR;

//车牌最大的字符数
#define g_cnPlateChars 7

#endif
