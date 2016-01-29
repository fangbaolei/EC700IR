#ifndef _SWAGCAWBTASK_H_
#define _SWAGCAWBTASK_H_

#include "SWTask.h"
#include "SWImageStatisticsInfo.h"


//#define GAIN_BASELINE 200 // 增益基准值：为了解决图片偏色问题，将增益抬高至少6DB。

#define AWB_FREQ 10 // AWB每6帧调节一次
// AWB调整阀值
#define AWB_ADJUSTMENT_THRESHOLD 15

#define AWB_ADJUSTMENT_TIME      6

#define AWB_ADJUST(a,b,c,d,e)       \
    a = a + INT((b - 1.0f) * c);    \
    a = MAX(d, a);                  \
    a = MIN(e, a);                  \

// --- PID控制算法 ---

/*
 * (1) 比例系数（P）对系统性能的影响：比例系数加大，使系统的动作灵敏，速度加快，稳态误差减小。P偏大，振荡次数加多，调节时间加长。P太大时，系统会趋于不稳定。P太小，又会使系统的动作缓慢。
 * (2) 积分系数（I）对系统性能的影响：积分作用使系统的稳定性下降，I大（积分作用强）会使系统不稳定，但能消除稳态误差，提高系统的控制精度。
 * (3) 微分系数（D）对系统性能的影响：微分作用可以改善动态特性，D偏大时，超调量较大，调节时间较短。D偏小时，超调量较小，调节时间较长。只有D合适，才能使超调量较小，减短调节时间。
 */

typedef struct _PID
{
    DOUBLE Command;	//期望数值
    DOUBLE Proportion;	//比例系数
    DOUBLE Integral;	//积分系数
    DOUBLE Derivative;	//微分系数
    DOUBLE preErr;		//前次误差
    DOUBLE sumErr;		//误差累积
} PID;

typedef struct tagRGB_Sum
{
    INT iRSum;
    INT iGSum;
    INT iBSum;
} RGB_Sum;

class CSWAgcAwbTask : public CSWTask
{
CLASSINFO(CSWAgcAwbTask, CSWTask)

public:
    CSWAgcAwbTask();
    virtual ~CSWAgcAwbTask();

    virtual SInt64 Run( );

public:
    VOID ResetAGC();

    // 初始化参数，初始化的目的是获取系统默认设置
    VOID Initial( );

    // 设置图像亮度预期值
    VOID SetAGCTh(INT iAGCTh);
    // 设置图像亮度预期值
    VOID GetAGCTh(INT &iAGCTh);

    // 设在AGC最大最小约束值
    VOID SetAGCParam(INT iShutterMin, INT iShutterMax, INT iGainMin, INT iGainMax);
    // 设置大AGC快门值及增益值
    void SetAGCMaxParam( DWORD dwiAGCShutterHOri , DWORD iGainHMax);
    // 获取最大AGC快门值及增益值
    void GetAGCParam( DWORD &dwiAGCShutterHOri , DWORD &iGainMax);
    // 设置最小AGC快门值及增益值
    void SetAGCMinParam( DWORD &dwiAGCShutterLOri , DWORD &iGainLMax);
    // 获取最大AGC快门值及增益值
    void GetAGCMinParam( DWORD &dwiAGCShutterLOri , DWORD &iGainLMax);

    // 设置当前快门
    VOID SetAGCShutter(INT iShutter);
    // 获取当前快门
    VOID GetAGCShutter(INT& iShutter);
    // 设置当前增益
    VOID SetAGCGain(INT iGain);
    // 获取当前增益
    VOID GetAGCGain( INT& iGain);

    // 设置当前增益值，格式为0x00RGB,R为R通道增益，G为G通道增益值 ， B为B通道增益值
    VOID SetAWBGain( DWORD* pdwRGBGain );
     // 获取当前增益值，格式为0x00RGB,R为R通道增益，G为G通道增益值 ， B为B通道增益值
    VOID GetAWBGain( DWORD &dwRGBGain );

    // 设置白平衡增益
    VOID SetAWBGain(BYTE iGainR, BYTE iGainG, BYTE iGainB);
    // 获取白平衡增益
    VOID GetAWBGain(BYTE& iGainR, BYTE& iGainG, BYTE& iGainB);

	// 设置AGC使能标志
	VOID SetAGCEnable( BOOL isEnable );
    BOOL GetAGCEnable(){ return m_isEnableAGC;}
    // 设置AWB使能标志
    VOID SetAWBEnable( BOOL isEnable ){
        m_isEnableAWB = isEnable;
        if( m_isEnableAWB ){
            m_nBeginTime = m_nAWBFlatRate = 0; this->SetAWBGain( 0 , 0 , 0);
        }
    }
    BOOL GetAWBEnable(){ return m_isEnableAWB;}

    // 设置自动光圈
    void SetAutoAperture( bool IsAutoAperture ){ m_IsAutoAperture = IsAutoAperture;}

protected:
    //for awb
    INT CalcRGBGain(
        FLOAT fltGainR, INT &iGainR,
        FLOAT fltGainG, INT &iGainG,
        FLOAT fltGainB, INT &iGainB,
        INT iMin, INT iMax
    );
    INT AdjustRGBGain(INT &iGainR, INT &iGainG, INT &iGainB);

    //for agc
    DOUBLE PIDCale(PID *p, DOUBLE feedback);
    INT CalcShutterGain(
        INT iYArg,      //当前平均亮度（0~255）
        INT iAGCTh,     //期望平均亮度（0~255）
        INT iAGCShutterL,
        INT iAGCShutterH,
        INT iAGCGainL,
        INT iAGCGainH,
        INT &iShutter,
        INT &iGain
    );
private:
    // 调节DC光圈
    bool AdjustAperture(
        int iYArg,      //当前平均亮度（0~255）
        int iAGCTh,     //期望平均亮度（0~255）
        int iAGCShutterL,
        int iAGCShutterH,
        int iAGCGainL,
        int iAGCGainH,
        int &iShutter,
        int &iGain );
private:
    // AGC和AWB算法核心参数
    INT m_iAgcAccurate;
    INT m_iAgcAdjustAccuracy;
    INT m_iAgcMaxCount;
    FLOAT m_fltShutterProportion;
    FLOAT m_fltShutterIntegral;
    FLOAT m_fltShutterDerivative;
    FLOAT m_fltGainProportion;
    FLOAT m_fltGainIntegral;
    FLOAT m_fltGainDerivative;
    FLOAT m_fltAwbAccurate;
    INT m_iAwbStep;

    FLOAT m_fltGainRSum;
    FLOAT m_fltGainGSum;
    FLOAT m_fltGainBSum;

    FLOAT m_fltOldGainRSum;
    FLOAT m_fltOldGainGSum;
    FLOAT m_fltOldGainBSum;

    INT m_iAwbMaxCount;

    PID m_cPIDShutter;
    PID m_cPIDGain;
    bool m_PIDInitFlag;

    // AGC生效计数器
    INT m_iAGCEffectCounter;

    // AGC当前调节方向。0：没有调节；1：往暗处调；-1：往亮处调。
    INT m_iAGCAdjustDirection;

private:
    // 是否自动调节DC光圈
    BOOL m_IsAutoAperture;
    // 最后更新的图像平均亮度值
    static int  fiLasttimeYValue;

    INT m_iAGCTh;
    INT m_iAGCShutterLOri;
    INT m_iAGCShutterHOri;
    INT m_iAGCGainLOri;
    INT m_iAGCGainHOri;
    INT m_iNowShutter;
    INT m_iNowGain;

    // AWB增益值，最大为255，最小为0
    INT m_iNowGainR;
    INT m_iNowGainG;
    INT m_iNowGainB;

    // 设备CCD通道数
    INT m_iCCDChannels;

    // AGC使能标志
    BOOL m_isEnableAGC;
    // AWB使能参数
    BOOL m_isEnableAWB;
    // AWB调整平率
    WCHAR m_nAWBFlatRate;
    DWORD m_nBeginTime;

    // 初始化统计对象
    static CSWImageStatisticsInfo m_objSWImageStatisticsInfo;
};

#endif

