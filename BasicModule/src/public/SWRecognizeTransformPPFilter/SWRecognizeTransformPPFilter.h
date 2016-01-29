/**
* @file SWRecognizeTransformPPFilter.h 
* @brief 识别Filter
* @copyright Signalway All Rights Reserved
* @author zhaopy
* @date 2013-03-20
* @version 1.0
*/

#ifndef __SW_RECOGNIZE__TRANSFORM_PP_FILTER_H__
#define __SW_RECOGNIZE__TRANSFORM_PP_FILTER_H__

#include "SWBaseFilter.h"
#include "SWMessage.h"
#include "SWCarLeft.h"
#include "SWPosImage.h"
#include "SWCameraDataPDU.h"
#include "SWBaseLinkCtrl.h"
#include "EPAppDetCtrl.h"
#include "SWMatchTansformDataStruct.h"
#include "arch.h"
#include "SWDomeCameraCtrl.h"


#define BALLIMAGE_H 1080
#define BALLIMAGE_W 1920

#define DELETE_PLATE_TIME_OUT 600000    //10 分钟




enum ENUM_PROCESS_STATUS {
	EPROCESS_STATUS_WAITTING = 0,			//等待处理
	EPROCESS_STATUS_CAR_DETECTING,			//检测车框处理中
	EPROCESS_STATUS_DOME_ADJUSTING,			//球机调整中
	EPROCESS_STATUS_PLATE_DETECTING,		//检牌处理中
};

enum ENUM_RECOGNIZE_RUN_FLAG {
	RECOGNIZE_NOT_START = 0,
	RECOGNIZE_RUNNING,
	RECOGNIZE_STOP
};

typedef enum ENUM_DETECT_STATUS{
	EDET_STATUS_MISSING = 0,		//丢失
	EDET_STATUS_DETECTING,			//检测到
	EDET_STATUS_PARKING,			//正在停车
	EDET_STATUS_PECCANCY			//违章
}ENUM_DETECT_STATUS;



typedef struct _TAGDETAREA
{
    int iPresetNum;				//预置位号
    int iType;					//类型？
//    int iRect_x[2];			//检测区域矩形 x坐标
//    int iRect_y[2];			//检测区域矩形 y坐标
	HV_RECT DetectRect;			//检测区域矩形
    int iValidPointCount;		//有效区域点数
    int iValidArea_x[32];		//有效区域X坐标
    int iValidArea_y[32];		//有效区域Y坐标
    int irgInitCarWidth[20];	//初始化车宽数组
    int irgMaxCarWidth[20];		//最大车宽数组
    int irgScaleNum[20];		//刻度值？

    //区域内检测车宽
    int iMaxCarWidth;
    int iMinCarWidth;
    int iScaleNum;
    float fltScale;
    _TAGDETAREA()
    {
        memset( this , 0 , sizeof( _TAGDETAREA ));
        iMaxCarWidth = 280;
        iMinCarWidth = 160;
        iScaleNum = 2;
        fltScale = 0.8f;
    }
} DETAREA;


typedef struct _DOME_PRESET_INFO
{
	INT iPresetNum;					//当前预置位编号
	FLOAT fltPan;					//Pan值
	FLOAT fltTilt;					//Tile值
	FLOAT fltZoom;					//缩放倍数
	CHAR szName[256];				//预置位名字
}DOME_PRESET_INFO;



typedef struct CAR_BOX_INFO
{
	DOME_PRESET_INFO cDomePresetInfo;	//车框相关的预置位信息
	HV_RECT CarPosition;				//车框位置
	ENUM_DETECT_STATUS eDetectStatus;	//检测状态
	INT iDetectAreaNum;					//属于检测区域编号
	INT iDetectType;					//检测类型
	INT iDetectNum;						//检测次数
	INT iLimitTime;
	BOOL fDetectedPlate;			//检测到车牌
	BOOL fDetectedCar;	
}CAR_BOX_INFO;


typedef struct CAR_PANORAMIC_INFO
{
	CSWImage *pImage;						//预置位全景图 
	DWORD dwPanoramicTick;					//获取全景图的tick数
	SWPA_DATETIME_TM cPanoramicRealTime;	//全景图现实时间
}CAR_PANORAMIC_INFO;

typedef struct __IMAGE_FILE_INFO
{
#define MAX_FRAME_NAME_SIZE 256
	SW_COMPONENT_IMAGE sComponent;
	DWORD dwFrameNo;					
	DWORD dwRefTimeMS;				    
	CHAR szFrameName[MAX_FRAME_NAME_SIZE];	
	BOOL fIsCaptureImage;					
	SWPA_TIME	sTimeStamp;                 

	__IMAGE_FILE_INFO()
	{
		swpa_memset(this, 0, sizeof(*this));
	}
		
}_IMAGE_FILE_INFO;

typedef struct CAR_PLATE_INFO
{
	CSWCarLeft *pCarLeft;
	CSWImage *rgpVideo[30];					//首次发现车辆视频数据，一个对象是一秒
	IPC_SHARE_REGION rgeIpcShareRegion[30];
	INT iVideoCount;						//视频数量，即多少秒
	INT iMissCounter;						//丢失计数器
	DWORD dwPanoramicTick[2];				//两张全景图Tick数
	DWORD dwDetectParkingTick;				//检测到停车时间
	SWPA_DATETIME_TM cPanoramicRealTime[2]; //两张全景图时间
	SWPA_DATETIME_TM cPlateRealTime[2]; 	//检到牌时间
	ENUM_DETECT_STATUS eDetectStatus;
	HV_RECT CarRect;
}CAR_PLATE_INFO;


typedef struct _CAR_WIDTH_INFO
{
	INT iCurDetectAreaMinSide;			//四边形检测区域大于车的最小边
	INT iCurCarWidth;					//当前宽
	INT iCurMinCarWidth;				//当前最小车宽
	INT iCurMaxCarWidth;
	INT iLastCarWidthAverage;			//上次检到车宽平均数
	INT iDetectPlateCount;				//检到牌的车框数
	INT iDetectPlateWidth[32];			//检到牌的车宽
}CAR_WIDTH_INFO;


typedef struct _CAR_DETECTINFO
{
	DOME_PRESET_INFO cDomePresetInfo;
    DETAREA* pDetAreaInfo;  //第一次检测区域的指针

    CRect cBoxPos;          //第一次检测车框位置信息
    float fltBlowUphalfZoom;    //当前检测车框的缩小倍数

    int iDetType;              // 第一次检测类型（车头、车尾等）
    int iDetNum;               // 第一次检测数
    int iDetFrameCount;        // 第一次检测帧数

    int iMaxCarWidth;           //检测车宽
    int iScaleNum;              //检测Scale

    BOOL fDetect1Miss;
    int iDetect1MissNum;       //检测1丢失
    int iDetect2MissNum;       //检测2丢失
    DWORD32 dwDetect1Time;       //第一次检测到的时间
    DWORD32 dwDetect2Time;       //第二次检测到的时间
    DWORD32 dwRecogTime;

//    ILLEGAL_INFO* pMasterInfo;
//    ILLEGAL_INFO* pSlaveInfo;
    BOOL fHadTrigger;           //已经出过牌了
    _CAR_DETECTINFO()
    {
        memset( this , 0 , sizeof(_CAR_DETECTINFO) );
    }
}CAR_DETECTINFO;

class CSWRecognizeTransformPPFilter : public CSWBaseFilter, CSWMessage
{
	CLASSINFO(CSWRecognizeTransformPPFilter, CSWBaseFilter)
public:
	CSWRecognizeTransformPPFilter();
	virtual ~CSWRecognizeTransformPPFilter();

	/**
   *@brief 识别模块初始化。
   */
	virtual HRESULT Initialize(INT iGlobalParamIndex, INT nLastLightType,INT nLastPulseLevel, INT nLastCplStatus, PVOID pvParam, PVOID pvMatchParam);
	virtual HRESULT RegisterCallBackFunction(PVOID OnDSPAlarm, PVOID pvParam);
	virtual HRESULT Run();
	virtual HRESULT Stop();

	static VOID OnProcessProxy(PVOID pvParam);
	static VOID OnSendJpegProxy(PVOID pvParam);
//    static VOID OnProcessSyncProxy(PVOID pvParam);
public:
	/**
   *@brief ISWPlateRecognitionEvent 回调接口
   */
	virtual HRESULT CarArriveEvent(CARARRIVE_INFO_STRUCT *pCarArriveInfo);
	virtual HRESULT CarLeftEvent(CARLEFT_INFO_STRUCT *pCarLeftInfo,CSWImage *pImage);
    virtual HRESULT GB28181Alarm(LPCSTR szMsg);

protected:
	/**
   *@brief 重载Receive接口，接收CSWImage数据
   */
	virtual HRESULT Receive(CSWObject* obj);	

	/**
   *@brief 提供图片识别命令
   */
	HRESULT OnRecognizePhoto(WPARAM wParam, LPARAM lParam);

	/**
   *@brief 设置识别开关
   */
	HRESULT OnRecognizeGetJPEG(WPARAM wParam, LPARAM lParam);

	/**
   *@brief 输出调试码流
   */
	HRESULT OnRecognizeOutPutDebug(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief 设置触发使能
	 */
	HRESULT OnTriggerEnable(WPARAM wParam, LPARAM lParam);
	
	/**
	 *@brief IO红绿灯触发事件
	 */
	HRESULT OnIOEvent(WPARAM wParam, LPARAM lParam);

	/**
	 *@brief 动态修改识别参数
	 */
	HRESULT OnModifyParam(WPARAM wParam, LPARAM lParam);

	/**
	 *@brief 获取车辆统计
	 */
	HRESULT OnGetCarLeftCount(WPARAM wParam, LPARAM lParam);

	/**
	   @brief 球机到达预置位信号，触发识别信号
	 */
	HRESULT OnPresetPosRecognizeStart(WPARAM wParam, LPARAM lParam);

	/**
	   @brief 停止识别信号
	 */
	HRESULT OnPresetPosRecognizeStop(WPARAM wParam, LPARAM lParam);

	/**
	   @brief 更新参数
	 */
	HRESULT OnModifyPeccancyParkingParam(WPARAM wParam, LPARAM lParam);

	/**
   *@brief 识别主线程
   */
	HRESULT OnProcess();

    /**
    *@brief 异步识别主线程
    */
    HRESULT OnProcessSync();

	/**
	   @brief 发送JPEG图片线程
	 */
	HRESULT OnSendJpeg();




	/**
	   @brief 获取视频
	 */
	HRESULT GetVideo(CSWImage **ppVideo, INT *piVideoCount);

	/**
	   @brief 构建违章结果
	   @param [IN] pImage当前图像
	   @param [IN][OUT] pCarLeft 传入构建的carleft，传出构建好的carleft
	 */
	HRESULT BuildCarLeft(CSWImage *pDetPanoramicImage, 
						 CSWImage *pDetCarFocusImage,
						 CSWImage *pPeccancyPanoramicImage,
						 CSWImage *pPeccancyCarFocusImage,
						 SW_RECT *pDetCarFocusPlatePos,
						 SW_RECT *pPeccancyCarFocusPlatePos,
						 SWPA_DATETIME_TM *pDetCarFocusTime,
						 SWPA_DATETIME_TM *pPeccancyCarFocusTime,
						 LPCSTR szPresetName,
						 CSWCarLeft *pCarLeft);



	 /**
	 *@brief 设置GB28181使能
	 */
	HRESULT SetGB28181Enable(const BOOL fEnable)
   {
		m_fEnableGB28181 = fEnable;
   }


	/**
	   @brief 获取更靠近中心的车牌
	   @param [IN] rgpPlateRect 检测到的车牌位置
	   @param [IN] iPlateCount 车牌数量
	   @retval 返回X轴上距离中点最近的车牌索引
	 */
	INT GetCenterPlate(HV_RECT *rgpPlateRect, INT iPlateCount);


	/**
	   @brief 获取可信度最大的carleft
	 */
	HRESULT GetMaxConfidenceCarLeft(CSWCarLeft **ppCarLeft);


	/**
	  @brief 删除所有carleft
	 */
	HRESULT ReleaseAllCarLeft();

	/**
	   @brief 计算放大框的大小
	   @param [IN] pSrcRect 输入框大小
	   @param [OUT] pDstRect 放大框大小
	 */
	HRESULT CalcBlowUpBox(HV_RECT *pSrcRect, HV_RECT *pDstRect);

	/**
	   @brief 检测车框宽度
	   @param [IN] pRect 矩形框左上点和右下点
	   @param [IN] fltZoom 缩放大小值
	   @param [IN] fltTilt Tilt值
	   @param [OUT] piWidth 像素车宽
	   @param [OUT] piScaleNum 刻度值
	   @retval S_OK - 成功  S_FALSE - 失败
	*/
	HRESULT CalcCarWidthScale(HV_RECT *pRect, float fltZoom, float fltTilt, 
							   int* piWidth, int* piScaleNum);

	/**
	   @brief 获取在检测有效区域内的车框
	   @param [IN] pImage 检测图片
	 */
	HRESULT GetCarInDetectArea(CSWImage *pImage);


	/**
	   @brief 获取全景图片
	 */
	HRESULT GetPanoramicImage(CSWImage* pImage);

	/**
	   @brief 判断当前位置是否在detArea中
	   @param [IN] pRect 矩形区域
	   @param [IN] detArea 检测区域,结构体里包含有效检测区域
	   @retval 
	   TRUE - 在检测区域内
	   FALSE - 不在检测区域内
	*/
	BOOL IsInArea( HV_RECT *pRect, DETAREA *detArea);


	/**
	   @brief 判断是否在检测时间段内
	   @param [IN] CurTime 当前时间
	 */
	BOOL IsInPeccancyTime(SWPA_DATETIME_TM *pCurTime, PECCANCY_PARKING_AREA *pPeccancyParkingArea);

	/**
	   @brief 检测是否是相同区域
	   @param [IN] pCRect0 区域1
	   @param [IN] pCRect1 区域2
	   @retval TRUE - 相同 FALSE - 不相同
	*/
	BOOL IsSameRect( HV_RECT *pCRect0, HV_RECT *pCRect1, INT *pFlag);

	/**
	   @brief 两个区域是否重叠
	   @param [IN] pCRect0 区域1
	   @param [IN] pCRect1 区域2
	   @retval TRUE - 重叠 FALSE - 不重叠
	*/
	BOOL IsOverlap(HV_RECT *pCRect0, HV_RECT *pCRect1);

	/**
	   @brief 删除无效的车框，即删除在检测区域外的车框
	   @param pCarBoxInfo [IN] 车框信息
	   @param pDetectArea [IN] 检测区域
	 */
	HRESULT DeteleInvalidCarBox(PROCESS_DETECT_INFO *pCarBoxInfo, 
								DETAREA *pDetectArea);

	/**
	   @brief 通过点区域获取矩形
	   @param [IN] iPointCount 点数量
	   @param [IN] pPointX X坐标数组
	   @param [IN] pPointY Y坐标数组
	   @param [OUT] pDetectRect 输出矩形
 	 */
	HRESULT GetDetectRect(INT iPointCount, INT *pPointX,
						  INT *pPointY, HV_RECT *pDetectRect);

	/**
	   @brief 获取检测区域最小边值
	 */
	HRESULT GetDetectRectSide(INT iDetAreaPosCount, INT *rgiPosX, INT *rgiPosY,
							  HV_RECT *pRect, INT *pMinSide);

	/**
	   @brief 获取当前车宽
	 */
	HRESULT GetCurCarWidthScaleNum(INT iIndex, INT *pCurrentCarWidth, INT *pScaleNum);


	/**
	   @brief 更新车宽
	 */
	HRESULT UpdateCarWidth();

	/**
	   @brief 添加入检测到的车框链表
	   @param pCarBoxInfo [IN] 此次检测到的车框
	 */
	HRESULT AddToCarBoxList(PROCESS_DETECT_INFO *pCarBoxInfo, INT iLimitTime, INT iDetectAreaNum);


	/**
	   @brief 所有车框设为MISSING
	   @param [IN] eDetectStatus 设置状态枚举值
	 */
	HRESULT AllCarBoxSetStatus(ENUM_DETECT_STATUS eDetectStatus);

	/**
	   @brief 绘制车辆红框
	 */
	HRESULT SetDebugRedBox();

	/**
	   @brief 所有车牌丢失状态+1, 如果检到重设为0，否则到达一定次数就删掉车牌
	   
	 */
	HRESULT AllPlateMissIncrement();

	/**
	   @brief 删除所有MISSING状态的车框
	 */
	HRESULT DeleteMissCarBox(VOID);

	/**
	   @brief 初始化检测车框参数
	   @param pRect [IN] 检测矩形范围
	   @param iCarBoxWidth [IN] 检测车框的大小值
	   @param iScaleNum [IN] 刻度值？
	   @param pCarDetectParam [OUT] 初始化对象
	 */
	HRESULT InitCarDetectParam(HV_RECT *pRect, INT iCarBoxWidth, INT iScaleNum,
							   CAR_DETECT_PARAM *pCarDetectParam);

	/**
	   @brief 初始化检牌参数
	 */
	HRESULT InitPlateDetectParam(FRAME_RECOGNIZE_PARAM *pFrameRecogParam);


	/**
	   @brief 设置放大速度
	 */
	HRESULT SetDomeCameraZoomSpeed(INT iSpeed);

	/**
	   @brief 调整球机，(pan\tilt\zoom)
	   @param [IN] pImage 传入此参数的目的是为了调整位置后获取全景图片
	 */
	HRESULT DomeCameraAdjust(CSWImage *pImage);





	/**
	   @brief 检测是否达到违章
	   @param [IN] pCarLeft 车牌信息
	 */
	BOOL IsPeccancyParking(CSWCarLeft *pCarLeft, CSWImage *pImage);

	/**
	   @brief 删除丢失的车牌信息
	 */
	HRESULT DeleteMissPlate();


	/**
	   @brief 删除所有车牌信息
	 */
	HRESULT DeleteAllPlate();


	/**
	   @brief 删除无效的CarLeft,主要为了删除里面的车牌图片
	 */
	HRESULT DeleteCarLeft(CARLEFT_INFO_STRUCT *pCarLeftInfo);

	/**
	   @brief 是否开始检测车牌
	 */
	BOOL IsBeginDetectPlate();

	/**
	   @brief 检测车牌
	   @param [IN] pImage 图像
	   @param [OUT] pProcessEvent 检测结果事件
	 */
	HRESULT DetectPlate(CSWImage* pImage, PROCESS_EVENT_STRUCT *pProcessEvent);

	/**
	   @brief 检测是白天还是夜晚
	 */
	HRESULT DetectDayOrNight(CSWImage *pImage);

	/**
	   @brief 计算图片整体亮度
	 */
	HRESULT CalcImageBrightness(CSWImage *pImage, DWORD *pBrightness);

	/**
	   @brief 判断白天还是晚上
	 */
	BOOL IsDay();

	/**
	   @brief 是否时合适的亮度
	 */
	BOOL IsSuitableBrightness(INT iBrightness);

	/**
	   @brief 计算车牌亮度
	 */
	HRESULT CalcPlateBrightness(CSWImage *pImage, HV_RECT *pPlateRect, 
								DWORD *pBrightness);

	/**
	   @brief 是否是相似牌
	 */
	BOOL IsSimilarityPlate(CSWCarLeft *pCurCarLeft, HV_RECT *pCurCarRect,
						   CSWCarLeft *pSaveCarLeft, HV_RECT *pSaveCarRect);

	/**
	   @brief 是否移动中的车牌
	 */
	BOOL IsMovingPlate(CSWCarLeft *pCurCarLeft, HV_RECT *pCurCarRect,
						   CSWCarLeft *pSaveCarLeft, HV_RECT *pSaveCarRect);


	/**
	   @brief 设置所有车的检测状态
	 */	
	HRESULT AllCarBoxSetDetectStatus();

	/**
	   @brief 删除同一个位置的车牌信息
	 */
	HRESULT DeleteSameCarRectPlateInfo();


	/**
	   @brief 释放车牌信息结构占用的内存
	 */
	HRESULT ReleaseCarPlateInfo(CAR_PLATE_INFO *pCarPlateInfo);




	//



protected:
	//自动化映射宏
	SW_BEGIN_DISP_MAP(CSWRecognizeTransformPPFilter, CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 6)
		SW_DISP_METHOD(RegisterCallBackFunction, 2)
		SW_DISP_METHOD(SetGB28181Enable, 1)
	SW_END_DISP_MAP()
	//消息映射宏
	SW_BEGIN_MESSAGE_MAP(CSWRecognizeTransformIPFilter, CSWMessage)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_PHOTO, OnRecognizePhoto)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_GETJPEG, OnRecognizeGetJPEG)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_OUTPUTDEBUG, OnRecognizeOutPutDebug)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_ENABLE_TRIGGER, OnTriggerEnable)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_IOEVENT, OnIOEvent)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_MODIFY_PARAM, OnModifyParam)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_GET_CARLEFT_COUNT, OnGetCarLeftCount)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_CHECKPT, OnPresetPosRecognizeStart)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_STOP, OnPresetPosRecognizeStop)
		SW_MESSAGE_HANDLER(MSG_RECOGNIZE_MODIFY_PECCANCYPARKING_PARAM, OnModifyPeccancyParkingParam)
	SW_END_MESSAGE_MAP()

private:
	VOID Clear();

public:
	static const INT MAX_IMAGE_COUNT = 3;

private:

	TRACKER_CFG_PARAM *m_pTrackerCfg;

	BOOL m_fSendJPEG;
	BOOL m_fSendDebug;
	BOOL m_fInitialized;

	INT  m_iCarArriveTriggerType;
	INT  m_iIOLevel;

	CSWList<CSWImage*> m_lstImage;		// 图片对列
	CSWMutex m_cMutexImage;             // 访问临界区
	CSWThread* m_pThread;				// 识别线程
	CSWSemaphore m_cSemImage;			


	DWORD m_dwPlateCount;		//车辆统计

	BOOL m_fEnableGB28181;

	/*
    // ganzz
    CSWThread* m_pProcQueueThread;              // 处理列队线程
	  INT m_nMaxProcQueueLen;
    struct PROC_QUEUE_ELEM
    {
        CSWImage* pImage;
        void* pData;    // 处理数据指针
    };
    CSWSemaphore m_cSemProcQueue;
	*/

    CSWMutex m_cMutexOutput;
    BOOL m_fModifyParam;

	//wujf
	//时间都是相对值
	const INT WAIT_DOME_READY;					//等待球机准备就绪,单位毫秒ms
	const INT DETECT_CARBOX_BEGIN_TIME;			//检测车框时间
	const INT DOME_MOVE_BLOCK_DONE_TIME;		//球机框选放大完成时间
	const INT DOME_CAMERA_DO_ONE_FOCUS_TIME;    //夜晚做触发聚焦时间
	const INT DAY_DETECT_CARPLATE_BEGIN_TIME;		//白天检测车牌时间
	const INT NIGHT_DETECT_CARPLATE_BEGIN_TIME; //夜晚检测车牌时间
	const INT DETECT_CARPLATE_END_TIME;			//检测车牌结束
	const INT DETECT_CARBOX_NUM;				//检测车辆次数
	const INT DETECT_CARPLATE_NUM;				//检车牌次数
	const INT DETECT_PLATE_IMAGE_MAX_NUM;		//检牌最多检的帧数
	const INT RECORD_VIDEO_BEGIN_TIME;
	const INT DAY_RECORD_VIDEO_END_TIME;
	const INT NIGHT_RECORD_VIDEO_END_TIME;      //夜晚录像结束时间
	const INT DELETE_CARPLATE_MISS_THRESHOLD;	//车牌丢失阈值

	INT m_iCurProcessStatus; 					//当前运行状态
	CSWSemaphore m_semProcess;
	CSWList<CAR_BOX_INFO *> m_lstCarBoxList[256];
	CSWList<CAR_PLATE_INFO *> m_lstCarPlateList[256];
	CSWList<CSWCarLeft *> m_lstCarLeftList;
	CAR_BOX_INFO m_cCurCarBoxInfo;
	DOME_PRESET_INFO m_cCurPreset;
	DWORD m_dwRecordVideoStartTick;           //开始录像时间
	DWORD m_dwRecordVideoEndTick;
	DWORD m_dwGetDetectSignalTick;
	DWORD m_dwLastDomeCameraAdjustTick;		//上次球机调整时间
	CAR_PANORAMIC_INFO m_cCurPanoramicInfo; //当前全景图的信息
	SW_POSITION m_cCurCarPosition;			//当前车信息
	INT m_iCurCarPositionIndex;
	INT m_iCurDetectCarCounter;				//当前检车计数
	INT m_iCurDetectPlateCounter;			//检测牌计数器
	INT m_iCurLosePlateCounter;				//检不到牌计数器
	INT m_iFrameDelayCount; 				//帧延迟数量
	BOOL m_fIsPresetPosition;
	BOOL m_fSendCarRedBox;					//发送红框
	BOOL m_fSendPlateRedBox;				//发送车牌红框
	INT m_iRecognizeRunFlag;				//识别进行状态
//	BOOL m_fIsFirstTimeDetectCar;			//收到信号第一次检车辆
	BOOL m_fIsGammaMode;
	BOOL m_fIsDay;							//是否时白天

	MATCHPARAM *m_pMatchParam;
	CSWThread* m_pJpegThread;				// 发送JPEG线程
	CSWSemaphore m_cSemJpegImage;			
	CSWList<CSWImage*> m_lstJpegImage;		// 图片对列
	CSWMutex m_cMutexJpegImage;             // 访问临界区
	INT m_iCarDebugCount;
	INT m_iPlateDebugCount;					//车牌调试框数
	INT m_iCurPeccancyAreaLimitTime;
	INT m_iGammaMode;
	HV_RECT m_rgcCarDebugRect[32];
	HV_RECT m_rgcPlateDebugRect[32];		//车牌调试框
	HV_RECT m_CurDetectCarRect;
	DETAREA m_rgcCurDetectArea[8];		 //检测区域坐标
	INT m_iCurDetectAreaCount;
	CSWDomeCameraCtrl m_cDomeCameraCtrl;
	CAR_WIDTH_INFO m_rgcCarWidthInfo[DETECT_AREA_COUNT_MAX];
	INT m_iCameraModel;
//	MSG_SET_GAMMA_STRENGTH,OnSetGammaMode
	
};
REGISTER_CLASS(CSWRecognizeTransformPPFilter)
#endif
