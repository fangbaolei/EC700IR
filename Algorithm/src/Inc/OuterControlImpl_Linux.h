#ifndef _OUTERCONTROLIMPL_INCLUDED__
#define _OUTERCONTROLIMPL_INCLUDED__

#include "hvutils.h"
#include "hvthreadbase.h"
#include "OuterControl.h"
#include "IPTInterface.h"
#include "trackerdef.h"
#include "ImgProcesser.h"

// IPT 用到的一些定义
#define MAX_IPT_PORT_NUM	            4	        //最多4个IPT口
#define MAX_IPT_PIN_NUM		            2	        //每个IO口最多使用两个针脚

#define IPT_EVENT_IO_IN                 0x01        //IO口输入
#define IPT_EVENT_CIRCLE                0x02	    //线圈测速
#define IPT_EVENT_IO_OUT                0x04        //IO口输出
#define IPT_EVENT_RADAR				    0x08        //雷达测速

#define IPT_EVENT_IO_IN_VOLTAGE			0x01		// 高低电平
#define IPT_EVENT_IO_IN_PLUSE_UP	    0x02		// 上升沿脉冲
#define IPT_EVENT_IO_IN_PLUSE_DWON		0x03		// 下降沿脉冲

#define IPT_EVENT_CIRCLE_SPEED1		    0x01		// 线圈测速电平上升沿触发
#define IPT_EVENT_CIRCLE_SPEED2		    0x02		// 线圈测速电平下降沿触发

#define IPT_EVENT_IO_OUT_IO             0x01		// IO口单独控制
#define IPT_EVENT_IO_OUT_PC             0x02		// 上位机单独控制
#define IPT_EVENT_IO_OUT_IOANDPC        0x03		// IO口和上位机共同控制

#define IPT_EVENT_RADER_SPEED1			0x01		// 火花雷达
#define IPT_EVENT_RADER_SPEED2			0x02		// Si2雷达
#define IPT_EVENT_RADER_SPEED3			0x03		// SS125雷达
#define IPT_EVENT_RADER_SPEED4			0x14		// OWL雷达
#define IPT_EVENT_RADER_SPEED5			0x15		// ??雷达
#define IPT_EVENT_RADER_SPEED6			0x16		// ??雷达
#define IPT_EVENT_RADER_SPEED7			0x17		// ??雷达
#define IPT_EVENT_RADER_SPEED8			0x18		// ??雷达
#define IPT_EVENT_RADER_SPEED9			0x19		// ??雷达
#define IPT_EVENT_RADER_SPEEDA			0x1A		// ??雷达
#define IPT_EVENT_RADER_SPEEDB			0x1B		// ??雷达
#define IPT_EVENT_RADER_SPEEDC			0x1C		// ??雷达
#define IPT_EVENT_RADER_SPEEDD			0x1D		// ??雷达
#define IPT_EVENT_RADER_SPEEDE			0x1E		// ??雷达
#define IPT_EVENT_RADER_SPEEDF			0x1F		// ??雷达

//红绿灯最大场景数
#define MAX_SCENE_COUNT 10

#ifdef SINGLE_BOARD_PLATFORM
#define PLATE_LIST_COUNT		10				// 车牌队列长度
#define SIGNAL_LIST_COUNT		25				// 信号队列长度
#else
#define PLATE_LIST_COUNT		10				// 车牌队列长度
#define SIGNAL_LIST_COUNT		20				// 信号队列长度
#endif

#define CAPROAD_LIST_COUNT      50				//车道队列长度
#define MAX_CARLEFT_COUNT       5               //CarLeft队列最大长度


typedef struct tagIPT_INFO
{
    BOOL fUse;								// 使用标志(true:使用)
    int iComSource;							// 做为COM口(现只有速度),信号源编号
    int iOutSigSource;
    int iOutSigLevel;
    int iOutSigWidth;
    int rgSigSource[MAX_IPT_PIN_NUM];		// BIN口对应的信号源(现只有触发输入)
} IPT_INFO;

/**< 信号匹配器总参数 */
typedef struct _SignalMatchParam
{
    DWORD32 dwPlateHoldTime;
    DWORD32 dwSignalHoldTime;
    int emOutPutPlateMode;
    int iCapCommandTime;
    int nSignalTypeNum;
    int nSignalSourceNum;
    int iMainSignalType;
    int iCapImgCount;
    //车道编号的方向0:从左开始,1:从右开始
    int iRoadNumberBegin;
    //起始车道号
    int iStartRoadNum;
    int iFlashLampDiff;

    SIGNAL_TYPE rgMatchSigType[10];
    SIGNAL_SOURCE rgSigSource[30];

    BOOL fSignalIsMain;
    BOOL fHardTriggerCap;
    BOOL fForcePlate;
    BOOL fEnableFlashLight;
    BOOL fEnableRecogSnapImage;
    BOOL fEnableIPT;
    int  iRadarType;
    bool fSpeedDecide;

    int nMainSigTypeNum;
    int nMainSourceNum;

    IPT_INFO rgIPTInfo[4];
    char szComStr[256];
    char szRecogArea[64];

    int nSoftTrigSignalSource;

    char bDeviceParam[1024];
    int  nDeviceLen;
    int  nPlateType;

    int iSpeedLimit;    //限速值
    int iCapMode;  //抓拍模式 (0:抓拍一次; 1:超速违章抓拍2次; 2:全部抓拍2次)
    BOOL fEPolice;
    bool fEnablePanoramicCamera;
    int nSwitchFlashLightCount;

    _SignalMatchParam()
    {
        memset(this, 0, sizeof(struct _SignalMatchParam));
        strcpy(szComStr, "/dev/ttyS0");
        nSoftTrigSignalSource = -1;
        fEnableRecogSnapImage = FALSE;
        iCapImgCount = 1;
        strcpy(szRecogArea, "[0,50,100,100],5,9");
        nDeviceLen = 1024;
        fEnableIPT = FALSE;
        iRadarType = 0;
        fSpeedDecide = true;
        nPlateType = 0;
        iSpeedLimit = 0;
        iCapMode = 0;
        fEnablePanoramicCamera = false;
        fEPolice = FALSE;
        nSwitchFlashLightCount = 0;
        //默认值
        BYTE8 bTemp[64] =
        {
            //设备类型  屏蔽位  消抖动延时
#ifdef SINGLE_BOARD_PLATFORM
            0x02,0xFF,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//IO上升沿
            //设备类型  线圈距离（2字节）  速度系数  最小速度  消抖延时  事件时标 最大速度 相机抓拍延时 触发哪个IO口
            0x01,0x3C,0x00,0x64,0x05,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//线圈上升沿
#else
            0x02,0xFF,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//IO上升沿
            //设备类型  线圈距离（2字节）  速度系数  最小速度  消抖延时  事件时标 最大速度 相机抓拍延时 触发哪个IO口
            0x01,0x3C,0x00,0x64,0x05,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//线圈上升沿
#endif
            // 设备类型   屏蔽位   消抖延时	 输出信号源（高4位Logic和CtrlLogic选择，低4位IO输入选择）输出延时	（2字节） 极性  输出初始化电平
            0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x5C,0x6E,0x00,0x00,0x00,0x00,0x00,//IO输出低电平////默认IN输入极性为直通
            //设备类型  测量时间间隔（2字节） 速度系数  最小速度  车辆运动方向
            0x01,0xf4,0x01,0x64,0x014,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00//火花雷达
        };
        bDeviceParam[0] = 0;
        bDeviceParam[1] = 0;
        bDeviceParam[2] = 0;
        bDeviceParam[3] = 0;
        nDeviceLen = 256;
        BYTE8 *ptr = (BYTE8 *)&bDeviceParam[4];
        for (int i = 0; i < nDeviceLen; i++)
        {
            *ptr++ = bTemp[i%64];
        }
    };

} SignalMatchParam;

// 针脚定义
typedef struct
{
    bool fUse;							    // 使用标志(true:使用)
    int nPluseType;						    // 脉冲类型  0: 上升沿  1：下降沿
} IPT_PIN_INFO;

//用于外总控CarArrive时记录CarArrive的相关信息
typedef struct
{
    DWORD32 dwOutTime;
    DWORD32 dwCarArriveTime;
    int iRoadNum;
} MATCH_CAP_ROAD;

//车牌处理线程
class CProcessQueueProxy : public CHvThreadBase
{
public:
    CProcessQueueProxy()
    {
        m_iStackSize = 32 * 1024;
    }

    virtual const char* GetName()
    {
        static char szName[] = "CProcessQueueProxy";
        return szName;
    }

    virtual HRESULT Run( void* pvParam );
};

//信号处理线程
class CProcessIPTProxy : public CHvThreadBase
{
public:
    CProcessIPTProxy()
    {
        m_iStackSize = 32 * 1024;
    }

    virtual const char* GetName()
    {
        static char szName[] = "CProcessIPTProxy";
        return szName;
    }

    virtual HRESULT Run( void* pvParam );
};

//每一帧处理线程
class CProcessOneFrameProxy : public CHvThreadBase
{
public:
    CProcessOneFrameProxy()
    {
        m_iStackSize = 32 * 1024;
    }

    virtual const char* GetName()
    {
        static char szName[] = "CProcessOneFrameProxy";
        return szName;
    }

    virtual HRESULT Run( void* pvParam );
};

class COuterControlImpl : public IOuterControler
{
public:
    COuterControlImpl(const SignalMatchParam& cSignalMatchParam);
    virtual ~COuterControlImpl();

    //IOuterControler
    STDMETHOD(InitOuterControler)();
    STDMETHOD(Run)();
    STDMETHOD(Stop)();
    STDMETHOD(AppendSignal)(SIGNAL_INFO *pSignalInfo);  //添加信号
    STDMETHOD(CarArrive)(
        CARARRIVE_INFO_STRUCT *pCarArriveInfo,
        LPVOID pvUserData
    );
    STDMETHOD(CarLeft)(
        CARLEFT_INFO_STRUCT *pCarLeftInfo,
        LPVOID pvUserData
    );
    //取当前红绿灯的状态,通过IPT
    STDMETHOD(GetTrafficLightStatus)(
        int* pbStatus
    )
    {
        if ( pbStatus == NULL )
        {
            return E_POINTER;
        }
        *pbStatus = 0;
        (*pbStatus) = m_bIOLevel;

        return S_OK;
    }
    STDMETHOD(ProcessOneFrame)(
        int iVideoID,
        IReferenceComponentImage *pImage,
        IReferenceComponentImage *pCapImage,
        PVOID pvParam,
        PROCESS_EVENT_STRUCT* pProcessEvent
    );

    virtual bool ThreadIsOk();

    int GetSignalNum();
    //因为图片的信号源没有IP地址对应，因此无法得到多个图片信号源的下标，目前只能暂时只能添加1个图片信号源
    int GetImageSourceIndex(void);
    int GetSoftTrigSignalSource();

    void Clear();
    void ReleasePlateInfo(PLATE_INFO_hxl *pPlateInfo);
    void ReleaseSignalInfo(SIGNAL_INFO *pSignalInfo);
    HRESULT AddPanoramicSignal(SIGNAL_INFO *pSignalInfo);  //添加信号

    BOOL IsNeedToRecogSnapImage()
    {
        return m_cSignalMatchParam.fEnableRecogSnapImage;
    }

    int NeedProcessCarArriveTrigger(void)
    {
        if(!m_cSignalMatchParam.nSwitchFlashLightCount)
        {
            return 0;
        }
        return !m_fSwitchToVideoDetect ? 1 : 2;
    }

    HRESULT ClearSignalQueue();
    HRESULT OutputSignal(OUTPUT_SIGNAL output_signal);
    HRESULT SetFrameProperty(
        int iWidth,
        int iHeight
    );

    HRESULT SendMatchInfo(
        CARLEFT_INFO_STRUCT *pCarLeftInfo,
        LPVOID pvUserData
    );

    void MountVideoRecoger(IVideoRecoger* pVideoRecoger)
    {
        m_pVideoRecoger = pVideoRecoger;
    }
    void SetIPTParam(BYTE8 * pbData, int size);
    void SetEventData(BYTE8 * pbData, int size);

public:
    HRESULT ProcessQueue();
    HRESULT ProcessIPTSignal();
    HRESULT ProcessOneFrameProxy();

protected:
    OUTPUT_PLATE_MODE m_emOutPutPlateMode;		// 出牌模式
    WORK_MODE_TYPE m_emTrackType;				// 信号类型
    char m_szComStr[256];						// 串口字符串

    static const int MAX_DEVICEPARAM_LEN = 512;
    BYTE8 m_rgbDeviceParam[MAX_DEVICEPARAM_LEN];

    int m_iDeviceParamLen;

    //实时的IO输入状态
    BYTE8 m_bIOLevel;

    //保存红绿灯位置信息
    static const int MAX_LIGHT_BUFFER = 20;
    int m_iLightCount;
    HV_RECT m_rgLightPos[MAX_LIGHT_BUFFER];
    int m_iRedLightCount;
    HV_RECT m_rgRedLightPos[MAX_LIGHT_BUFFER];

private:
    // 判断是否匹配
    bool IsMatchPlate(
        PLATE_INFO_hxl *pPlateInfo,
        SIGNAL_INFO *pSignalInfo,
        long &lTimeDiff,
        DWORD32 &dwPosDiff
    );

    bool IsMatchSignal(SIGNAL_INFO *pSignalInfoMain, SIGNAL_INFO *pSignalInfo, long &lTimeDiff);
    bool IsPlateOutTime(PLATE_INFO_hxl *pPlateInfo);						// 判断车牌是否过期
    bool IsSignalOutTime(SIGNAL_INFO *pSignalInfo);							// 判断信号是否过期
    bool PreProcessQueue();													// 队列预处理

    int Match();
    void GetSignalAttchInfo(MATCH_INFO *pMatchInfo, SIGNAL_INFO *pSignalInfo, SIGNAL_INFO *pSignalInfo_2 = NULL);
    bool MatchPlate(PLATE_INFO_hxl *pPlateInfo);							// 按车牌匹配所有信号
    bool MatchNoPlate(SIGNAL_INFO *pSignalInfoMain);						// 按主信号匹配无牌车

    void OutputMatchPlate();							// 输出匹配车牌
    void OutPutPlate(MATCH_INFO *pMatchInfo);			// 输出有牌车信息
    void OutPutNoPlate(MATCH_INFO *pMatchInfo);		    // 输出无牌车信息

    void ReleaseOldElement();
    void ClearAllQueue();

    int MatchRoad(CARLEFT_INFO_STRUCT *pPlateInfo);		//判断车牌是哪个车道的

    int MatchRoad(int iX1, int iY1, int iX2, int iY2);  //判断车牌是哪个车道的
    HRESULT MatchPanoramicSignal(DWORD32 dwPlateTime, IReferenceComponentImage** pRetImageFRame);

private:
    DWORD32 m_dwOuterThreadIsOkTime;		// 外总控线程正常时间
    DWORD32 m_dwIPTThreadIsOkTime;			// IPT线程正常时间
    DWORD32 m_dwFramThreadIsOkTime;			// 处理当前帧线程正常时间

    bool m_fInitialized;					// 初始化标志
    bool m_fRunFlag;						// 线程运行标志

    bool m_fSpeedDecide;					// 启用软件测速校正 true：使用速度 false:不使用速度
    int m_nWidth;							// 处理图片宽度
    int m_nHeight;							// 处理图片高度

    IReferenceComponentImage* m_pImage;	    // 当前帧图片
    bool m_fIsProcessImage;					// 当前帧是否已经处理

    bool m_fForcePlate;						// 只要有车牌,不管有没有主信号都输出.

    // 外总控线程
    CProcessQueueProxy m_cProcessQueueProxy;
    CProcessIPTProxy m_cProcessIPTProxy;
    CProcessOneFrameProxy m_cProcessOneFramProxy;

    bool m_fBeginIPT;						// IPT线程启动标志，IPT线程必须在外总控之后才开始运行
    DWORD32 m_dwLastSyncTime;

    // IPT用到的参数
    IComm *m_pComm;
    IIPTControl *m_pIPTControl;
    IIPTDevice *m_pIPTDevice;

    IPT_INFO m_rgIPTInfo[MAX_IPT_PORT_NUM];

    int m_nMainSigTypeNum;													// 主信号类型数量
    int *m_prgMainSigType;													// 保存主信号类型的数组

    int m_nMainSourceNum;													// 主信号源数量
    int *m_prgMainSource;													// 保存主信号的数组

    int m_nMatchInfoNum;								                    // 本次匹配信息数量
    MATCH_INFO m_rgMatchInfo[SIGNAL_LIST_COUNT];		                    // 匹配信息，使用信号队列程度

    int MatchMainSignal(SIGNAL_INFO *pMainSignal);							// 匹配主信号

    LIGHT_TYPE m_nLastLightType;
    int        m_iCplStatus;
    int        m_iPulseLevel;
    //判断车道信息
    int m_rgdwCarArriveTime[5];
    int m_iRoadNumberBegin;
    int m_iStartRoadNum;

    //抓拍时间间隔
    int m_iCapCommandTime;

    //硬触发抓拍开关
    bool m_fHardTriggerCap;

    DWORD32 m_dwLastCarArriveTime;

    SignalMatchParam m_cSignalMatchParam;

    CHvList<PLATE_INFO_hxl*> m_queCarLeft;		// 车辆离开信息队列

    BOOL m_fUsedFlashLight;

    IVideoRecoger* m_pVideoRecoger;

    DWORD32 m_dwCaptureImageCount;
    DWORD32 m_dwCarLeftCount;
    BOOL    m_fSwitchToVideoDetect;
};

#endif // _OUTERCONTROLIMPL_INCLUDED__
