#ifndef __SW_MATCH_TRANSFORM_DATA_STRUCT_H__
#define __SW_MATCH_TRANSFORM_DATA_STRUCT_H__

enum
{
	E_SIG_RADAR_SPEED = 0,
	E_SIG_COIL_SPEED
};
//信号
typedef struct tagSIGNAL
{
	DWORD dwTime;       //信号时间
	DWORD dwSpeed;      //信号速度
	DWORD dwSpeedType;	//速度类型
	DWORD dwDirection;	//雷达方向
	CSWImage* pImage;   //信号附带的图片
	BOOL fIsMatch;		//是否已经匹配过。
	tagSIGNAL()
	{
		swpa_memset(this, 0, sizeof(*this));
	}
}SIGNAL;
//信号参数
typedef struct tagSIGNAL_PARAM
{
	DWORD dwRoadID;		  //信号所属的车道,0xFF为全覆盖
	DWORD dwType;       //信号类型
	DWORD dwSignalID;	  //同一个信号的索引
	BOOL    fTrigger;   //是否要触发抓拍,对于视频检测信号，必定触发抓拍
	BOOL    fRecognize; //是否识别
	DWORD dwCondition;  //匹配条件，0:此信号可有可无，只要有车牌即可出牌，1:必须有此信号和车牌才能出结果,2:只要有此信号，就必定出牌
	SIGNAL  signal[6]; //此信号类型的信号数据
	
	tagSIGNAL_PARAM()
	{
		swpa_memset(this, 0, sizeof(*this));
	}
}SIGNAL_PARAM;

#define DETECT_AREA_COUNT_MAX	32			//最大检测区域数
#define DETECT_AREA_POS_COUNT_MAX 32		//一个预置位最大的点数	
#define PRESET_COUNT_MAX 256				//最大预置位数	

typedef struct _PECCANCY_PARKING_AREA
{
	BOOL fPeccancyAreaEnable;	//违章区域使能
	INT iPresetNum;				//当前预置位值 
	INT iDetAreaNum;			//检测区域编号
	INT iDetAreaPosCount;	//[DETECT_AREA_COUNT_MAX];		//检测区域数量
	INT rgiPosX[DETECT_AREA_POS_COUNT_MAX];				//检测区域X坐标
	INT rgiPosY[DETECT_AREA_POS_COUNT_MAX];				//检测区域Y坐标
//	INT rgiWeekDay[7];				//表示星期
	INT iBeginTime[2];				//开始 [0]时[1]分
	INT iEndTime[2];				//结束 [0]时[1]分
	INT iLimitTime;				//限制停车时间  单位：秒
}PECCANCY_PARKING_AREA;

typedef struct _PECCANCY_PARKING_PARAM
{
	BOOL fPeccancyPresetEnable[PRESET_COUNT_MAX];
	INT iNightUseLedLight;
	FLOAT fltConfidenceFilter;		//可信度过滤器
	PECCANCY_PARKING_AREA cPeccancyParkingArea[DETECT_AREA_COUNT_MAX];
}PECCANCY_PARKING_PARAM;

//匹配参数
typedef struct tagMATCHPARAM
{
	BOOL  fEnable;             //外总控使能开关
	DWORD dwSignalKeepTime;    //信号保留时间
	DWORD dwPlateKeepTime;     //车牌保留时间
	DWORD dwMatchMinTime;      //车牌时间-信号时间的最小差距
	DWORD dwMatchMaxTime;      //信号时间-车牌时间的最大差距
	DWORD dwSignalNum;         //信号数量
	SIGNAL_PARAM  signal[6];  //单个信号的属性

	BOOL fCaptureRecong;		// 抓拍图重识别开关
	int iDetectorAreaLeft;
	int iDetectorAreaRight;
	int iDetectorAreaTop;
	int iDetectorAreaBottom;
	int iDetectorMinScaleNum;
	int iDetectorMaxScaleNum;
	PECCANCY_PARKING_PARAM cPeccancyParkingParam;	//违章停车相关参数
	
	tagMATCHPARAM()
	{
		swpa_memset(this, 0, sizeof(*this));
		fEnable = TRUE;
		fCaptureRecong = FALSE;
		dwPlateKeepTime = 1500;
		dwSignalKeepTime = 2000;
		dwMatchMaxTime = 1500;
		dwMatchMinTime = 1500;
	}
}MATCHPARAM;

//线圈参数结构体
typedef struct tagCOIL_PARAM
{
	INT iTowCoilInstance;     //两个线圈的距离
	INT iCoilEgdeType;        //上升沿触发还是下降沿触发
	BYTE bTrigger;            //触发抓拍的线圈,按位来操作
	                          //位0: ------第1个线圈触发抓拍
	                          //位1: ------第2个线圈触发抓拍
	                          //位2: ------第3个线圈触发抓拍
	                          //位3: ------第4个线圈触发抓拍...
	
	tagCOIL_PARAM()
	{
		iTowCoilInstance = 50;
		iCoilEgdeType = 0;
		bTrigger = 0;
	}
}COIL_PARAM;

//外部设备参数
typedef struct tagDEVICEPARAM
{
	INT iBaudrate;
	INT iDeviceType;          //设备类型,"0:无;1:川速雷达;2:苏江车检器;3:康耐德红绿灯转换器;4:奥利维亚雷达;5:四川九洲雷达"
	INT iRoadNo;			        // 所属车道号
	INT iCommType;			      // 串口类型：0：RS232 1：RS485
	INT iComNo2RoadNo1;	//串口1对应车道
	INT iComNo2RoadNo2;	//串口2对应车道
	INT iComNo2RoadNo3;	//串口3对应车道
	INT iComNo2RoadNo4;	//串口4对应车道
	
	COIL_PARAM cCoilParam;     //车检器参数
	tagDEVICEPARAM()
	{
		iBaudrate = 9600;
		iDeviceType = 0;
		iRoadNo = 0;
		//金星只支持RS485
		iCommType = 1;
		iComNo2RoadNo1=0;
		iComNo2RoadNo2=1;
		iComNo2RoadNo1=0;
		iComNo2RoadNo2=0;
	}
}DEVICEPARAM;
#endif

