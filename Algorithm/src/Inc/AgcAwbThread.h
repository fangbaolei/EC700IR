#ifndef _AGCAWBTHREAD_H_
#define _AGCAWBTHREAD_H_

#include "hvthread.h"
#include "hvthreadbase.h"

#define GAIN_BASELINE 200 // 增益基准值：为了解决图片偏色问题，将增益抬高至少6DB。

#define AWB_FREQ 6 // AWB每6帧调节一次

#define AWB_ADJUST(a,b,c,d,e)       \
    a = a + int((b - 1.0f) * c);    \
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
    double Command;	//期望数值
    double Proportion;	//比例系数
    double Integral;	//积分系数
    double Derivative;	//微分系数
    double preErr;		//前次误差
    double sumErr;		//误差累积
} PID;

typedef struct tagRGB_Sum
{
    int iRSum;
    int iGSum;
    int iBSum;
} RGB_Sum;
// zhaopy auto
typedef struct _tagEnvInfo
{
    int iYSum;
    int iYPlate;
    int iShutter;
    int iGain;
}
ENV_INFO;

typedef enum
{
    ENV_UNKNOW = 0,
    ENV_CLOUDY,
    ENV_DAY,
    ENV_BLIGHT,
    ENV_FLIGHT,
    ENV_NEIGHT,
    ENV_COUNT
}
ENV_TYPE;
class CAgcAwbThread : public CHvThreadBase
{
public:
    CAgcAwbThread();
    ~CAgcAwbThread();

public:
    virtual const char* GetName()
    {
        static char szName[] = "CAgcAwbThread";
        return szName;
    }

    virtual HRESULT Run(void *pvParamter);

public:
    void ResetAGC();
    void SetAGCTh(int iAGCTh);
    void SetAGCParam(int iShutterMin, int iShutterMax, int iGainMin, int iGainMax);
    void SetAGCShutterGain(int iShutter, int iGain);
    void GetAGCShutterGain(int& iShutter, int& iGain);
    void SetAWBGain(int iGainR, int iGainG, int iGainB);
    void GetAWBGain(int& iGainR, int& iGainG, int& iGainB);

    HRESULT PutYArg(int iYArg);
    HRESULT PutRgbSum(RGB_Sum cRgbSum);

    // zhaopy auto capture param...
    ENV_TYPE GetEnvType()
    {
        return m_envType;
    }
    HRESULT PutEnvInfo(ENV_INFO& envInfo);
protected:
    HRESULT GetYArg(int& iYArg);
    HRESULT GetRgbSum(RGB_Sum& cRgbSum);
    // zhaopy auto...
    HRESULT GetEnvInfo(ENV_INFO& envInfo);
    ENV_TYPE CheckEnvType(ENV_INFO& envInfo);

    void InitAgcAwbCoreParam();

    //for awb
    int CalcRGBGain(
        float fltGainR, int &iGainR,
        float fltGainG, int &iGainG,
        float fltGainB, int &iGainB,
        int iMin, int iMax
    );
    int AdjustRGBGain(int &iGainR, int &iGainG, int &iGainB);

    //for agc
    double PIDCale(PID *p, double feedback);
    int CalcShutterGain(
        int iYArg,      //当前平均亮度（0~255）
        int iAGCTh,     //期望平均亮度（0~255）
        int iAGCShutterL,
        int iAGCShutterH,
        int iAGCGainL,
        int iAGCGainH,
        int &iShutter,
        int &iGain
    );

private:
    // zhaopy auto...
    HiVideo::CHvList<ENV_INFO> m_listEnvInfo;
    ENV_TYPE m_envType;
    ENV_TYPE m_etLast;
    DWORD32 m_dwLastETTick;
    HV_SEM_HANDLE m_hSemEnvInfo;	
    HiVideo::CHvList<int> m_listYArg;
    HiVideo::CHvList<RGB_Sum> m_listRgbSum;
    HV_SEM_HANDLE m_hSemListYArg;
    HV_SEM_HANDLE m_hSemListRgbSum;
    // 为了实时性，队列长度必须置为1。
    static const int LIST_YARG_MAX_COUNT = 1;
    static const int LIST_RGBSUM_MAX_COUNT = 1;
    static const int LIST_ENVINFO_MAX_COUNT = 10;
private:
    // AGC和AWB算法核心参数
    int m_iAgcAccurate;
    int m_iAgcAdjustAccuracy;
    int m_iAgcMaxCount;
    float m_fltShutterProportion;
    float m_fltShutterIntegral;
    float m_fltShutterDerivative;
    float m_fltGainProportion;
    float m_fltGainIntegral;
    float m_fltGainDerivative;
    float m_fltAwbAccurate;
    int m_iAwbStep;

    float m_fltGainRSum;
    float m_fltGainGSum;
    float m_fltGainBSum;

    int m_iAwbMaxCount;

    PID m_cPIDShutter;
    PID m_cPIDGain;
    bool m_PIDInitFlag;

    // AGC生效计数器
    int m_iAGCEffectCounter;

    // AGC当前调节方向。0：没有调节；1：往暗处调；-1：往亮处调。
    int m_iAGCAdjustDirection;

private:
    int m_iAGCTh;
    int m_iAGCShutterLOri;
    int m_iAGCShutterHOri;
    int m_iAGCGainLOri;
    int m_iAGCGainHOri;
    int m_iNowShutter;
    int m_iNowGain;
    int m_iNowGainR;
    int m_iNowGainG;
    int m_iNowGainB;
};

#endif
