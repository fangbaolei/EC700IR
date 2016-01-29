#include "SWAgcAwbTask.h"
#include "SWImageStatisticsInfo.h"
#include "swpa_camera.h"
#include "SWLog.h"

// DC光圈相关常量值
#define YMAX_DIFF  5
// 增益基准值
#define GAIN_BASELINE 200




#ifndef MIN
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#endif

#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

static FLOAT fdbThresholdValue = 0.10;
// 初始化统计对象
CSWImageStatisticsInfo CSWAgcAwbTask::m_objSWImageStatisticsInfo;
// 最后更新的图像平均亮度值
int  CSWAgcAwbTask::fiLasttimeYValue = 0;

CSWAgcAwbTask::CSWAgcAwbTask()
: m_iAGCShutterLOri( 0 )
, m_iAGCShutterHOri( 0 )
, m_iAGCGainLOri( 0 )
, m_iAGCGainHOri( 0 )
, m_iCCDChannels( 0 )
, m_isEnableAGC( 0 )
, m_isEnableAWB( 0 )
, m_iAGCTh( 100 )
, m_iAwbStep( 100 )
, m_iAgcAccurate( 15 )
, m_iAGCEffectCounter( 0 )
, m_iNowShutter( 0 )
, m_iNowGain( 0 )
, m_iAwbMaxCount( 0 )
, m_PIDInitFlag( false )
, m_IsAutoAperture( false )
, m_nAWBFlatRate( 0 )
, m_nBeginTime( 0 )
{
    m_iAgcAccurate = 15;
    m_iAgcAdjustAccuracy = 5;
    m_iAgcMaxCount = 30;
    m_fltShutterProportion = 0.005;
    m_fltShutterIntegral = 0.005;
    m_fltShutterDerivative = 0.005;
    m_fltGainProportion = 0.01;
    m_fltGainIntegral = 0.01;
    m_fltGainDerivative = 0.01;
    m_fltAwbAccurate = 0.05;


    m_fltGainRSum = m_fltGainGSum = m_fltGainBSum = 0;
    m_iNowGainR =  m_iNowGainG = m_iNowGainB = 0;

    memset(&m_cPIDShutter, 0, sizeof(m_cPIDShutter));
    memset(&m_cPIDGain, 0, sizeof(m_cPIDGain));

    m_iAGCEffectCounter = m_iAGCAdjustDirection = 0;

    CSWImageStatisticsInfo::Initialization( );
}

CSWAgcAwbTask::~CSWAgcAwbTask()
{

}
VOID CSWAgcAwbTask::Initial( )
{
    swpa_camera_basicparam_get_shutter( &m_iNowShutter );
    swpa_camera_basicparam_get_gain( &m_iNowGain );
}
SInt64 CSWAgcAwbTask::Run( )
{
static INT fOldYarg = 0 ;
static INT fnDiffValue = 30;
static INT fnTime = 80;

	EventFlags events = this->GetEvents( );

	if( events & CSWTask::kIdleEvent ) return 0;
	if( !GetAWBEnable( ) && !GetAGCEnable() ) return 0;

	while( !(events & CSWTask::kKillEvent) )
	{
	    // 如果需要使能AGC
	    if( GetAGCEnable( ))
	    {
            INT iYArg = CSWImageStatisticsInfo::GetPartYArgValue();
            if( iYArg != -2)
            {
                fOldYarg = iYArg >= 0 ? iYArg : fOldYarg;
                if( swpa_abs( m_iAGCTh - iYArg ) > fnDiffValue ){
                     iYArg = fOldYarg; fnTime = 20;
                }
                else  fnTime = 80;

                if( iYArg >= 0 )
                {

                    if ( !m_IsAutoAperture && 1 == CalcShutterGain(
                                    iYArg,
                                    m_iAGCTh,  // Y预设值，即：期望亮度
                                    m_iAGCShutterLOri, m_iAGCShutterHOri,
                                    m_iAGCGainLOri, m_iAGCGainHOri,
                                    m_iNowShutter,
                                    m_iNowGain) ){

                            this->SetAGCShutter(m_iNowShutter);
                            this->SetAGCGain(m_iNowGain);

                            SW_TRACE_DEBUG( "<AGC> Y-th-ss-gg-s-g[%d, %d, %d-%d, %d-%d, %d, %d]\n",
                                     iYArg,
                                     m_iAGCTh,
                                     m_iAGCShutterLOri, m_iAGCShutterHOri,
                                     m_iAGCGainLOri, m_iAGCGainHOri,
                                     m_iNowShutter,
                                     m_iNowGain);
                        }
                        else if ( m_IsAutoAperture && 1 == AdjustAperture(
                                    iYArg,
                                    m_iAGCTh,  // Y预设值，即：期望亮度
                                    m_iAGCShutterLOri, m_iAGCShutterHOri,
                                    m_iAGCGainLOri, m_iAGCGainHOri,
                                    m_iNowShutter,
                                    m_iNowGain) )
                        {

                            this->SetAGCShutter(m_iNowShutter);
                            this->SetAGCGain(m_iNowGain);

                            SW_TRACE_DEBUG( "<DCaperture_AND_AGC> Y-th-ss-gg-s-g[%d, %d, %d-%d, %d-%d, %d, %d]\n",
                                     iYArg,
                                     m_iAGCTh,
                                     m_iAGCShutterLOri, m_iAGCShutterHOri,
                                     m_iAGCGainLOri, m_iAGCGainHOri,
                                     m_iNowShutter,
                                     m_iNowGain);
                        }
                    }
                }
	    }
        // 如果需要使能白平衡
        if( GetAWBEnable( )){

            INT iRSum , iGSum ,iBSum;
            CSWImageStatisticsInfo::GetRGBSum(iRSum , iGSum , iBSum);

            //AWB调整
            if ( iRSum > 0 && iBSum > 0 )
            {
                FLOAT fltR = 1.0*iGSum / iRSum;
                FLOAT fltB = 1.0*iGSum / iBSum;

                if ( 1 == CalcRGBGain(
                            fltR, m_iNowGainR,
                            1.0, m_iNowGainG,
                            fltB, m_iNowGainB,
                            -100, 511) )
                {

                    this->AdjustRGBGain(m_iNowGainR, m_iNowGainG, m_iNowGainB);
                    this->SetAWBGain( m_iNowGainR,m_iNowGainG, m_iNowGainB);

                    SW_TRACE_DEBUG( "<AWB> GainR[%d:%d]; GainG[%d:%d]; GainB[%d:%d]\n",
                             iRSum , m_iNowGainR, iGSum , m_iNowGainG, iBSum , m_iNowGainB);

                }
            }
        }

        return fnTime;
    }

    return -1;
}

void CSWAgcAwbTask::SetAGCTh(INT iAGCTh)
{
    m_iAGCTh = iAGCTh;
}
// 设置图像亮度预期值
VOID CSWAgcAwbTask::GetAGCTh(INT &iAGCTh)
{
    iAGCTh = m_iAGCTh;
}

// 设置最小AGC快门值及增益值
void CSWAgcAwbTask::SetAGCMaxParam( DWORD dwiAGCShutterHOri , DWORD iGainHMax)
{
    this->SetAGCParam( m_iAGCShutterLOri , dwiAGCShutterHOri , m_iAGCGainLOri , iGainHMax);
}
// 设置最小AGC快门值及增益值
void CSWAgcAwbTask::SetAGCMinParam( DWORD &dwiAGCShutterLOri , DWORD &iGainLMax)
{
    this->SetAGCParam( dwiAGCShutterLOri , m_iAGCShutterHOri , iGainLMax , m_iAGCGainHOri);
}
// 获取最大AGC快门值及增益值
void CSWAgcAwbTask::GetAGCMinParam( DWORD &dwiAGCShutterLOri , DWORD &iGainLMax)
{
    dwiAGCShutterLOri = m_iAGCShutterLOri; iGainLMax = m_iAGCGainLOri;
}
// 设置AGC使能标志
VOID CSWAgcAwbTask::SetAGCEnable( BOOL isEnable )
{
	m_isEnableAGC = isEnable;
	if( m_isEnableAGC )
	{
		m_iNowShutter = MIN(m_iAGCShutterHOri, m_iNowShutter);
		m_iNowShutter = MAX(m_iAGCShutterLOri, m_iNowShutter);
		m_iNowGain = MIN(m_iAGCGainHOri, m_iNowGain);
		m_iNowGain = MAX(m_iAGCGainLOri, m_iNowGain);
		this->SetAGCShutter( m_iNowShutter );
		this->SetAGCGain( m_iNowGain );
	}
}
void CSWAgcAwbTask::SetAGCParam(INT iShutterMin, INT iShutterMax, INT iGainMin, INT iGainMax)
{
    m_iAGCShutterLOri = iShutterMin;
    m_iAGCShutterHOri = iShutterMax;

    m_iAGCGainLOri = iGainMin;
    m_iAGCGainHOri = iGainMax;

    if( m_iAGCShutterHOri == 0 || m_iAGCGainHOri == 0 ) return ;

	if( GetAGCEnable() )
	{
		m_iNowShutter = MIN(m_iAGCShutterHOri, m_iNowShutter);
		m_iNowShutter = MAX(m_iAGCShutterLOri, m_iNowShutter);
		m_iNowGain = MIN(m_iAGCGainHOri, m_iNowGain);
		m_iNowGain = MAX(m_iAGCGainLOri, m_iNowGain);
	}

    this->SetAGCShutter( m_iNowShutter );
    this->SetAGCGain( m_iNowGain );
}
void CSWAgcAwbTask::GetAGCParam( DWORD &dwiAGCShutterHOri , DWORD &iGainMax)
{
    dwiAGCShutterHOri = m_iAGCShutterHOri ; iGainMax = m_iAGCGainHOri;
}

void CSWAgcAwbTask::SetAGCShutter(INT iShutter)
{
    swpa_camera_basicparam_set_shutter((m_iNowShutter = iShutter));
}

void CSWAgcAwbTask::GetAGCShutter(INT& iShutter)
{
    iShutter = m_iNowShutter;
}

void CSWAgcAwbTask::SetAGCGain(INT iGain)
{
    swpa_camera_basicparam_set_gain((m_iNowGain = iGain));
}

void CSWAgcAwbTask::GetAGCGain( INT& iGain)
{
    iGain = m_iNowGain;
}

// 设置当前增益值，格式为0x00RGB,R为R通道增益，G为G通道增益值 ， B为B通道增益值
VOID CSWAgcAwbTask::SetAWBGain( DWORD* pdwRGBGain )
{
    this->SetAWBGain( pdwRGBGain[0] , pdwRGBGain[1] , pdwRGBGain[2]);
}
 // 获取当前增益值，格式为0x00RGB,R为R通道增益，G为G通道增益值 ， B为B通道增益值
VOID CSWAgcAwbTask::GetAWBGain( DWORD &dwRGBGain )
{
    dwRGBGain = m_iNowGainB;
    dwRGBGain |= (m_iNowGainG << 8);
    dwRGBGain |= (m_iNowGainR << 16);
}
void CSWAgcAwbTask::SetAWBGain(BYTE iGainR, BYTE iGainG, BYTE iGainB)
{
    m_iNowGainR = iGainR;
    m_iNowGainG = iGainG;
    m_iNowGainB = iGainB;

    swpa_camera_basicparam_set_rgb_gain( m_iNowGainR , m_iNowGainG , m_iNowGainB );
}

void CSWAgcAwbTask::GetAWBGain(BYTE& iGainR, BYTE& iGainG, BYTE& iGainB)
{
    iGainR = m_iNowGainR;
    iGainG = m_iNowGainG;
    iGainB = m_iNowGainB;
}

INT CSWAgcAwbTask::CalcRGBGain(
    FLOAT fltGainR, INT &iGainR,
    FLOAT fltGainG, INT &iGainG,
    FLOAT fltGainB, INT &iGainB,
    INT iMin, INT iMax
)
{
    if ( m_iAwbMaxCount++ < AWB_FREQ )
    {
        m_fltGainRSum += fltGainR;
        m_fltGainGSum += fltGainG;
        m_fltGainBSum += fltGainB;

        return 0;
    }
    else
    {
        m_fltGainRSum += fltGainR;
        m_fltGainGSum += fltGainG;
        m_fltGainBSum += fltGainB;

        fltGainR = m_fltGainRSum / AWB_FREQ;
        fltGainG = m_fltGainGSum / AWB_FREQ;
        fltGainB = m_fltGainBSum / AWB_FREQ;

        m_iAwbMaxCount = 0;
        m_fltGainRSum = m_fltGainGSum = m_fltGainBSum = 0;

        // 调整平衡后，进行惰性处理
        if( m_nBeginTime++ > AWB_ADJUSTMENT_TIME && (swpa_fabs(1.0 - fltGainR) >= fdbThresholdValue
           || swpa_fabs(1.0 - fltGainB) >= fdbThresholdValue) )
           {    // 设置惰性阀值
                if( m_nBeginTime >= 0xFFFFFEEB ) m_nBeginTime = AWB_ADJUSTMENT_TIME;
                if( m_nAWBFlatRate++ < AWB_ADJUSTMENT_THRESHOLD) return 0;
           }
        else if( m_nBeginTime > AWB_ADJUSTMENT_TIME ){m_nAWBFlatRate = 0; return 0;}
        // 清空标志
        if( m_nBeginTime > AWB_ADJUSTMENT_TIME ) m_nBeginTime = 0;

        INT iRet = 0;
        if ( swpa_fabs(1.0f - fltGainR) >= m_fltAwbAccurate )
        {
            AWB_ADJUST(iGainR, fltGainR, m_iAwbStep, iMin, iMax);
            iRet = 1;
        }
        if ( swpa_fabs(1.0f - fltGainG) >= m_fltAwbAccurate )
        {
            AWB_ADJUST(iGainG, fltGainG, m_iAwbStep, iMin, iMax);
            iRet = 1;
        }
        if ( swpa_fabs(1.0f - fltGainB) >= m_fltAwbAccurate )
        {
            AWB_ADJUST(iGainB, fltGainB, m_iAwbStep, iMin, iMax);
            iRet = 1;
        }

        m_nAWBFlatRate = 0; return iRet;
    }
}

INT CSWAgcAwbTask::AdjustRGBGain(INT &iGainR, INT &iGainG, INT &iGainB)
{
    INT iMin = MIN(iGainR, MIN(iGainG, iGainB));
    iGainR -= iMin;
    iGainG -= iMin;
    iGainB -= iMin;
    iGainR = MIN(iGainR, 511);
    iGainG = MIN(iGainG, 511);
    iGainB = MIN(iGainB, 511);
    return 0;
}

DOUBLE CSWAgcAwbTask::PIDCale(PID *p, DOUBLE feedback)
{
    DOUBLE Err=0,dErr=0;

    Err = p->Command - feedback;	//当前误差
    p->sumErr += Err;				//误差累加
    dErr = Err - p->preErr;			//误差微分
    p->preErr = Err;

    return (p->Proportion * Err		//比例项
            + p->Derivative * dErr	    //微分项
            + p->Integral * p->sumErr);	//积分项
}

void CSWAgcAwbTask::ResetAGC()
{
    m_iAGCEffectCounter = 0;
    m_iAGCAdjustDirection = 0;
}

/**
* @brief 自动AGC
* @param [in] iYArg 图像平均亮度
* @param [in] iAGCTh 图像期望平均亮度
* @param [in] iAGCShutterL 最低快门
* @param [in] iAGCShutterH 最大快门
* @param [in] iAGCGainL 最低增益
* @param [in] iAGCGainH 最大增益
* @param [out] iShutter 当前快门
* @param [out] iGain 当前增益
* @retval true 成功
* @retval 0 失败
*/
INT CSWAgcAwbTask::CalcShutterGain(
    INT iYArg,      //当前平均亮度（0~255）
    INT iAGCTh,     //期望平均亮度（0~255）
    INT iAGCShutterL,
    INT iAGCShutterH,
    INT iAGCGainL,
    INT iAGCGainH,
    INT &iShutter,
    INT &iGain
)
{
    static INT fCurrentCounts = 0 ;
    const INT iDiff = abs(iYArg - iAGCTh);


    // 当满足调节条件时，AGC生效计数器开始计数，否则归零。
    if (iDiff > m_iAgcAccurate)
    {
        m_iAGCEffectCounter++;
    }
    else if (m_iAGCEffectCounter > m_iAgcMaxCount && iDiff > m_iAgcAdjustAccuracy)
    {
        m_iAGCEffectCounter++;
    }
    else
    {
        m_iAGCEffectCounter = 0;
    }

    if (m_iAGCEffectCounter < m_iAgcMaxCount)
    {
        m_iAGCAdjustDirection = 0;

        m_cPIDShutter.preErr = 0;
        m_cPIDShutter.sumErr = 0;
        m_cPIDGain.preErr = 0;
        m_cPIDGain.sumErr = 0;
        return 0;  // 不进行AGC调节，因为还未满足启动调节的条件
    }

    // iAGCShutterMid是为了解决某些情况下
    // 不希望因为快门调到最大而导致拖尾现象发生的一个临时常量，
    // 其合法值介于iAGCShutterL到iAGCShutterH之间。
    const INT iAGCShutterMid = iAGCShutterL + INT((iAGCShutterH - iAGCShutterL) * 0.8);

    // 发现出现调节变向时，立即复位PID，避免PID深振荡。
    if (1 == m_iAGCAdjustDirection && iYArg < iAGCTh)
    {
        m_iAGCAdjustDirection = -1;

        m_cPIDShutter.preErr = 0;
        m_cPIDShutter.sumErr = 0;
        m_cPIDGain.preErr = 0;
        m_cPIDGain.sumErr = 0;
    }
    else if (-1 == m_iAGCAdjustDirection && iYArg > iAGCTh)
    {
        m_iAGCAdjustDirection = 1;

        m_cPIDShutter.preErr = 0;
        m_cPIDShutter.sumErr = 0;
        m_cPIDGain.preErr = 0;
        m_cPIDGain.sumErr = 0;
    }
    else // 0 == m_iAGCAdjustDirection
    {
        m_iAGCAdjustDirection = (iYArg > iAGCTh) ? (1) : (-1);
    }

    // 是否初始化PID的相关参数
    if (false == m_PIDInitFlag)
    {
        m_PIDInitFlag = true;

        m_cPIDShutter.Proportion = m_fltShutterProportion;
        m_cPIDShutter.Integral = m_fltShutterIntegral;
        m_cPIDShutter.Derivative = m_fltShutterDerivative;

        m_cPIDGain.Proportion = m_fltGainProportion;
        m_cPIDGain.Integral = m_fltGainIntegral;
        m_cPIDGain.Derivative = m_fltGainDerivative;
    }

    // 将期望值设入PID
    m_cPIDShutter.Command = iAGCTh;
    m_cPIDGain.Command = iAGCTh;

    if (iYArg < iAGCTh)  //如果当前图像暗
    {
        if (iShutter < iAGCShutterMid)
        {
            INT nmmm = (INT)PIDCale(&m_cPIDShutter, iYArg);

            iShutter += nmmm;//(INT)PIDCale(&m_cPIDShutter, iYArg);

            iShutter = MIN(iAGCShutterMid, iShutter);

            iShutter = MAX(iAGCShutterL, iShutter);

            if (iGain > iAGCGainL)
            {
                iGain -= ((iGain - iAGCGainL)>>4);
            }
        }
        else if (iGain < iAGCGainH)
        {
            iGain += (INT)PIDCale(&m_cPIDGain, iYArg);
            iGain = MIN(iAGCGainH, iGain);
            iGain = MAX(iAGCGainL, iGain);
        }
        else if (iShutter < iAGCShutterH)
        {
            iShutter += (INT)PIDCale(&m_cPIDShutter, iYArg);
            iShutter = MIN(iAGCShutterH, iShutter);
            iShutter = MAX(iAGCShutterL, iShutter);
        }
        else
        {
            return 0;  // 不进行AGC调节，因为已经调节到最亮了
        }
    }
    else if (iYArg > iAGCTh)  //如果当前图像亮
    {
        if (iShutter > iAGCShutterMid)
        {
            iShutter += (INT)PIDCale(&m_cPIDShutter, iYArg);
            iShutter = MIN(iAGCShutterH, iShutter);
            iShutter = MAX(iAGCShutterMid, iShutter);
        }
        else if (iGain > iAGCGainL)
        {
            iGain += (INT)PIDCale(&m_cPIDGain, iYArg);
            iGain = MIN(iAGCGainH, iGain);
            iGain = MAX(iAGCGainL, iGain);

            if (iShutter < iAGCShutterH)
            {
                iShutter += ((iAGCShutterH - iShutter)>>4);
            }
        }
        else if (iShutter > iAGCShutterL)
        {
            iShutter += (INT)PIDCale(&m_cPIDShutter, iYArg);
            iShutter = MIN(iAGCShutterMid, iShutter);
            iShutter = MAX(iAGCShutterL, iShutter);
        }
        else
        {
            return 0;  // 不进行AGC调节，因为已经调节到最暗了
        }
    }

    return 1;  // 进行AGC调节
}

/**
* @brief 设置DC光圈及自动AGC
* @param [in] iYArg 图像平均亮度
* @param [in] iAGCTh 图像期望平均亮度
* @param [in] iAGCShutterL 最低快门
* @param [in] iAGCShutterH 最大快门
* @param [in] iAGCGainL 最低增益
* @param [in] iAGCGainH 最大增益
* @param [out] iShutter 当前快门
* @param [out] iGain 当前增益
* @retval true 成功
* @retval 0 失败
*/
bool CSWAgcAwbTask::AdjustAperture(
    int iYArg,    //当前平均亮度（0~255）
    int iAGCTh,   //期望平均亮度（0~255）
    int iAGCShutterL,
    int iAGCShutterH,
    int iAGCGainL,
    int iAGCGainH,
    int &iShutter,
    int &iGain )
{
    int iDiff = iYArg - CSWAgcAwbTask::fiLasttimeYValue;
    if( iYArg < 0 ){ return false;}

    if( iYArg < iAGCTh ){
        if( iDiff > YMAX_DIFF){
             // 设置打开状态
             swpa_camera_io_control_dc_iris( 1 );
             CSWAgcAwbTask::fiLasttimeYValue = iYArg; return false;
        }
        else if( iYArg < iAGCTh )
        {
            if( iShutter != m_iAGCShutterHOri )
                return CalcShutterGain(
                                iYArg,
                                iAGCTh,  // Y预设值，即：期望亮度
                                m_iAGCShutterLOri, m_iAGCShutterHOri,
                                m_iAGCGainLOri, m_iAGCGainHOri,
                                iShutter,
                                iGain);
            else{
                // 设置停止状态
                swpa_camera_io_control_dc_iris( 0 );
            }
        }
    }
    else{
        if( iGain > GAIN_BASELINE ){

            iShutter = iShutter < m_iAGCShutterHOri ? m_iAGCShutterHOri : iShutter;
             // 设置停止状态
            swpa_camera_io_control_dc_iris( 0 );
            // 改变增益
            UINT uStep = ((iGain - m_iAGCGainLOri)>>4);
            uStep = uStep == 0 ? 1 : uStep;
            iGain -= uStep; return true;
        }
        else if( (iYArg - iAGCTh) > YMAX_DIFF )
        {
            // 设置关闭状态
            swpa_camera_io_control_dc_iris( 2 );

            // 打开光圈的时间
            UINT uSleepTime = 0 ; int nDiffYCTH = (iYArg - iAGCTh);
            if( nDiffYCTH > 100 ) uSleepTime = 1000*120;
            else if( nDiffYCTH > 50 ) uSleepTime = 1000*100;
            else if( nDiffYCTH > 35 ) uSleepTime = 1000*80;
            else if( nDiffYCTH > 25 ) uSleepTime = 1000*60;
            else if( nDiffYCTH > 15 ) uSleepTime = 1000*50;
            else if( nDiffYCTH > 10 ) uSleepTime = 1000*40;
            else if( nDiffYCTH > 5 ) uSleepTime = 1000*30;
            else uSleepTime = 1000*10;

            // 设置光圈暂停不动时间
            swpa_thread_sleep_ms( uSleepTime / 1000 );

            SW_TRACE_DEBUG( "[AdjustAperture====== %d , %d ; %d ; %d, %d ; %d]\n"
            , iYArg , iDiff , iShutter , iGain , m_iAGCShutterHOri , uSleepTime / 1000);

            // 设置停止状态
            swpa_camera_io_control_dc_iris( 0 );
        }
    }

    return false;
}

