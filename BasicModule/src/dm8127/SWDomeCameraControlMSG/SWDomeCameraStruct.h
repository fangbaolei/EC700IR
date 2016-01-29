
#ifndef __SW_DOMECAMERA_STRUCT_H__
#define __SW_DOMECAMERA_STRUCT_H__


#define MAX_PRESET  256
#define MAX_HSCAN_PATH  8
#define MAX_FSCAN_PATH  4
#define MAX_MOTION_IN_FSCAN 2000
#define MAX_CRUISE  8
#define MAX_MOTION_IN_CRUISE  14
#define MAX_TIMER  12
#define MAX_MASK  8//16
#define MAX_MASK_IN_ONE_PRESET  8


#define MAX_SPEED  0x3f



enum
{
    MOVETYPE_BEGIN = 0,
    
    MOVETYPE_PRESET,
    MOVETYPE_HSCAN,
    MOVETYPE_FSCAN,
	MOVETYPE_CRUISE,
	MOVETYPE_INFINITY_PAN,
	
	MOVETYPE_OPERATOR,
	MOVETYPE_STANDBY,
	MOVETYPE_RESET,
	MOVETYPE_FSCANRECORD,
	
    MOVETYPE_END
};


enum
{
	OP_FROM_USER = 0, //来自webnavi、onvif、gb28181等用户手动操作球机
	OP_FROM_PROGRAM   //LprApp程序内部模块控制球机
};




/**
* @brief 球机状态基本参数结构体
*/
typedef struct _tagPOSPARAM
{
	INT iPanCoordinate;
	INT iTiltCoordinate;
	INT iZoom;
	INT iIris;
	INT iFocus;
	INT iFocusMode;
	INT iPanSpeed;
	INT iTiltSpeed;
	
} POS_PARAM_STRUCT;



/**
* @brief 预置位参数结构体
*/
typedef struct _tagPRESETPOS
{
	CHAR  szName[32];
    INT iPanCoordinate;
    INT iTiltCoordinate;
    INT iZoom;
    INT iIris;
    INT iFocus;
    INT iFocusMode;

	BOOL fCheckPT;
	BOOL fValid;
	
} PRESETPOS_PARAM_STRUCT;


/**
* @brief 水平扫描参数结构体
*/
typedef struct _tagHSCANPARAM
{
	CHAR szName[32];
    INT iLeftPos;
    INT iRightPos;
    INT iDirection;
    INT iSpeed;

	BOOL fValid;
    
} HSCAN_PARAM_STRUCT;


/**
* @brief 花样扫描参数结构体
*/
typedef struct _tagFScanMotion
{
	INT iMove;
	/*union{ 
		INT iLingerTime;
		POS_PARAM_STRUCT sPos;
	}uArg;*/
	INT iArg;
} FSCAN_MOTION_STRUCT;

typedef struct _tagFSCAN
{
	CHAR szName[32];
	POS_PARAM_STRUCT sInitialState;
    FSCAN_MOTION_STRUCT sMotion[MAX_MOTION_IN_FSCAN];
	INT iMotionCount;
	
	BOOL fValid;

} FSCAN_PARAM_STRUCT;



/**
* @brief 巡航参数结构体
*/
typedef struct _tagCruiseMotion
{
	INT iMoveType;
    INT iMoveID;
    INT iLingerTime;
} CRUISE_MOTION_STRUCT;


typedef struct _tagCRUISEPARAM
{
	CHAR szName[32];
    
    CRUISE_MOTION_STRUCT sMotion[MAX_MOTION_IN_CRUISE]; 
	INT iMotionCount;

	BOOL fCheckPT;
	BOOL fValid;
    
} CRUISE_PARAM_STRUCT;



/**
* @brief 守望参数结构体
*/
typedef struct _tagWATCHKEEPING
{
	INT iWatchTime;
	INT iMotionType;
	INT iMotionID;

	BOOL fEnable;
}WATCHKEEPING_PARAM_STRUCT;




/**
* @brief 图像冻结参数结构体
*/
typedef struct _tagPICTUREFREEZE
{
	BOOL fEnable;
}PICTUREFREEZE_PARAM_STRUCT;


/**
* @brief 隐私区域参数结构体
*/
typedef struct _tagMASK
{
	CHAR szName[32];
	INT iTop;
	INT iLeft;
	INT iHeight;
	INT iWidth;
	INT iColor;
	INT iMaskPan;
	INT iMaskTilt;
	INT iMaskZoom;

	BOOL fEnable;
	BOOL fValid;
	INT iPresetID;
}MASK_PARAM_STRUCT;



/**
* @brief 定时参数结构体
*/
typedef struct _tagTIMER
{
	CHAR szName[32];
	INT iWeekday;
	INT iBeginTime; //in minute
	INT iEndTime;	//in minute
	INT iMotionType;
	INT iMotionID;
	
	BOOL fEnable;
	BOOL fValid;
}TIMER_PARAM_STRUCT;



/**
* @brief 待机参数结构体
*/
typedef struct _tagSleep
{
	BOOL fEnable;
	INT iWaitTime;
}STANDBY_PARAM_STRUCT;



#endif //__SW_DOMECAMERA_STRUCT_H__
