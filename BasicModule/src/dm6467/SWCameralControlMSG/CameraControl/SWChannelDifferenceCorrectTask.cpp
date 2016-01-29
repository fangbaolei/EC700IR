#include "SWChannelDifferenceCorrectTask.h"
#include "SWImageStatisticsInfo.h"
#include "SWAgcAwbTask.h"
#include "SWCaptureTask.h"
#include "SWLog.h"


#define COLORABNORMAL 1.05
// 高敏度阀值
#define MINBOUNDER_VALUE 0.003
// 惰性阀值
#define INERTIA_MINBOUNDER_VALUE 0.008
// 一致性指标阀值
#define CONSISTENCYINDEX_VALUE   0.3
#define CONSISTENCYINDEX_VALUE_NEIGHT 1.5

#define EQUAL_VALUE 1.0
#define BCHANNELMAXALL_GAIN  110

#define MAXBLACK_VALUE  400
#define MAXGAIN_VALUE 110
#define THIRTLEVEL_VALUE 500
static UINT fuOldGain = 0 ;

CSWChannelDifferenceCorrectTask::CSWChannelDifferenceCorrectTask(CSWAgcAwbTask *pSWAgcAwbTask )
: m_pSWAgcAwbTask( pSWAgcAwbTask )
, m_iStepGainLevel( 1 )
, m_iStep( 2 )
, m_iCurrenCorGainA( 0 )
, m_iCurrenCorGainB( 0 )
, m_iValueTapA( 0 )
, m_iValueTapB( 0 )
{

}

CSWChannelDifferenceCorrectTask::~CSWChannelDifferenceCorrectTask()
{
    //dtor
}

SInt64 CSWChannelDifferenceCorrectTask::Run( )
{
 	EventFlags events = this->GetEvents( );
	if( events & CSWTask::kIdleEvent ){
	     this->ClearYinYangInfo( ); return 0;
	}

	static int siCount = 0;
	++siCount;
	while( !(events & CSWTask::kKillEvent) )
	{
	    if( m_pSWAgcAwbTask == NULL ) return 1000;

        FLOAT fDifferenceFactor = 1.0; FLOAT fConsisIndex = 0.0;
        CSWImageStatisticsInfo::GetVedioDiffStatistValue( fDifferenceFactor , fConsisIndex);

        int nCurrent = 0;  m_pSWAgcAwbTask->GetAGCGain( nCurrent );
        int iConsistenValue = nCurrent > BCHANNELMAXALL_GAIN ? CONSISTENCYINDEX_VALUE_NEIGHT : CONSISTENCYINDEX_VALUE;


        if( fConsisIndex > iConsistenValue)
            this->YinYangSignalCheck( fDifferenceFactor , fConsisIndex);


        if( siCount % 20 == 0 )
        {
        	SW_TRACE_DEBUG("<diff info>AG-B:%d,%d;BG-B:%d,%d.",
        				m_iCurrenCorGainA, m_iValueTapA, m_iCurrenCorGainB, m_iValueTapB);
        }
	    return 80;
	}

    return -1;
}
VOID CSWChannelDifferenceCorrectTask::ClearYinYangInfo( )
{

        m_iCurrenCorGainA = m_iCurrenCorGainB = 0;
        m_iValueTapA = m_iValueTapB = 0;

        // 清空黑电平
        swpa_camera_basicparam_set_clamp_level( m_iValueTapA , m_iValueTapB , 0 , 0);
        // 清空差异性增益
        swpa_camera_init_set_gain_correction( 0 , 0, 0 , 0  );

        if( fuOldGain != 0)
            swpa_camera_basicparam_set_gain( fuOldGain );

}
/**
 * @brief 调整阴阳面
 * @param [in] fYinyangSideFactor 通道差异因子
 * @成功返回VOID
 */
VOID CSWChannelDifferenceCorrectTask::YinYangSignalCheck( FLOAT fYinyangSideFactor
                                                         , FLOAT fConsisIndex )
{
   // 如果增益变化则重新设置微调增益及黑店平
    int nCurrent = 0;  m_pSWAgcAwbTask->GetAGCGain( nCurrent );

    if( nCurrent != fuOldGain){
        // 防止增益不断切换
       if( (nCurrent >= BCHANNELMAXALL_GAIN && fuOldGain < BCHANNELMAXALL_GAIN)
            || (nCurrent < BCHANNELMAXALL_GAIN && fuOldGain >= BCHANNELMAXALL_GAIN))
       {
            m_iStepGainLevel = 1; m_iStep = 1;
            // 重新复位黑电平
            swpa_camera_basicparam_set_clamp_level( 0 , 0 , 0 , 0);
            m_iValueTapA = m_iValueTapB = 0;
            m_iCurrenCorGainA = m_iCurrenCorGainB = 0;

            // 重新清空API增益偏差值
            swpa_camera_init_set_gain_correction( m_iCurrenCorGainA, m_iCurrenCorGainB , 0 ,0 );

            swpa_camera_basicparam_set_gain( nCurrent );
        }

        fuOldGain = nCurrent;
    }

    /* dengtx(2012-09-20)
     增加获取统计信息，进行阴阳面修正
    */
    float fDiffValue = swpa_fabs(1.0 - fYinyangSideFactor);


    // 高敏度最小阀值
    double dbMinBounderValue = MINBOUNDER_VALUE;
    // 最小惰性阀值
    if( nCurrent < MAXGAIN_VALUE )
        dbMinBounderValue = INERTIA_MINBOUNDER_VALUE;

    if( fDiffValue < dbMinBounderValue){
       // printf("Current yinyang side factor %f; the differe = %f\n",fYinyangSideFactor,fDiffValue);
        return ;
    }

    // 获取当前增益信息
    int A = m_iCurrenCorGainA;
    int B = m_iCurrenCorGainB;

    UINT uCurrentBMaXGain = fuOldGain < BCHANNELMAXALL_GAIN ? 100 : 0;
    float fYinYangValue = fuOldGain <= BCHANNELMAXALL_GAIN ? 1.0 : EQUAL_VALUE;

    if( fYinyangSideFactor > fYinYangValue ){
         // 在增加大于150以上B值需要改成小于0才可以调过来，否则会出现严重的色偏
         // B值小于100可调的话实用于白天
        if( fuOldGain >= MAXGAIN_VALUE){
             if( m_iValueTapB < MAXBLACK_VALUE && m_iValueTapA <= 0){
                  this->UpBlackB( );

            }
             else{
                  this->DownBlackA( );
             }
        }
        else{
            if( B < MAXBLACK_VALUE && A < -50){
                this->UpGainB( fuOldGain );}
            else{
                this->DownGainA( fuOldGain );
            }
        }
    }
    else{
        if( fuOldGain >= MAXGAIN_VALUE){
          if( m_iValueTapA <  MAXBLACK_VALUE && m_iValueTapB <= 0 ){

              this->UpBlackA( );
            }
            else {

                this->DownBlackB();}
        }
        else{
            if( A < MAXBLACK_VALUE && B <= 0 ){

             this->UpGainA( fuOldGain );
            }
            else
            {
                this->DownGainB( fuOldGain );
            }
        }
    }

    // 调试信息
    SW_TRACE_NORMAL("<diff> ConsisIndex= %f; BaseGai = %d; A Gian = %d ; \
     A back = %d; B Gian = %d ; B back = %d , diff=%f\n"
    , fConsisIndex , fuOldGain , A , m_iValueTapA,B,m_iValueTapB,fYinyangSideFactor);

}

/**
 * @brief 抬高A通道黑电平
 * @成功返回VOID
 */
VOID CSWChannelDifferenceCorrectTask::UpBlackA( )
{
  m_iValueTapA += m_iStep;
  swpa_camera_basicparam_set_clamp_level( m_iValueTapA , m_iValueTapB , 0 , 0);
}
/**
 * @brief 抬高B通道黑电平
 * @成功返回VOID
 */
VOID CSWChannelDifferenceCorrectTask::UpBlackB( )
{
  m_iValueTapB += m_iStep;
  swpa_camera_basicparam_set_clamp_level( m_iValueTapA , m_iValueTapB , 0 , 0);
}
/**
 * @brief 减小A通道黑电平
 * @param [in] fYinyangSideFactor 通道差异因子
 * @成功返回VOID
 */
VOID CSWChannelDifferenceCorrectTask::DownBlackA( )
{
  m_iValueTapA -= m_iStep;
  m_iValueTapA = m_iValueTapA < 0 ? 0 : m_iValueTapA;
  swpa_camera_basicparam_set_clamp_level( m_iValueTapA , m_iValueTapB , 0 , 0);
}
/**
 * @brief 减小B通道黑电平
 * @param [in] fYinyangSideFactor 通道差异因子
 * @成功返回VOID
 */
VOID CSWChannelDifferenceCorrectTask::DownBlackB( )
{
  m_iValueTapB -= m_iStep;
  m_iValueTapB = m_iValueTapB < 0 ? 0 : m_iValueTapB;
  swpa_camera_basicparam_set_clamp_level( m_iValueTapA , m_iValueTapB , 0 , 0);
}
/**
 * @brief 抬高A通道增益
 * @param [in] nBaseValue 通道本身原有设置增益
 * @成功返回VOID
 */
VOID CSWChannelDifferenceCorrectTask::UpGainA( INT nBaseValue )
{
  m_iCurrenCorGainA += m_iStepGainLevel;

  swpa_camera_init_set_gain_correction( m_iCurrenCorGainA , m_iCurrenCorGainB ,
                                       0 , 0 );
  swpa_camera_basicparam_set_gain( nBaseValue );

}
/**
 * @brief 抬高B通道增益
 * @param [in] nBaseValue 通道本身原有设置增益
 * @成功返回VOID
 */
VOID CSWChannelDifferenceCorrectTask::UpGainB( INT nBaseValue )
{
  m_iCurrenCorGainB += m_iStepGainLevel;
  swpa_camera_init_set_gain_correction( m_iCurrenCorGainA , m_iCurrenCorGainB ,
                                       0 , 0  );

  swpa_camera_basicparam_set_gain( nBaseValue );
}
/**
 * @brief 减小A通道增益
 * @param [in] nBaseValue 通道本身原有设置增益
 * @成功返回VOID
 */
VOID CSWChannelDifferenceCorrectTask::DownGainA( INT nBaseValue)
{
  m_iCurrenCorGainA -= m_iStepGainLevel;
  swpa_camera_init_set_gain_correction( m_iCurrenCorGainA , m_iCurrenCorGainB ,
                                       0 , 0  );

    swpa_camera_basicparam_set_gain( nBaseValue );
}
/**
 * @brief 减小B通道增益
 * @param [in] nBaseValue 通道本身原有设置增益
 * @成功返回VOID
 */
VOID CSWChannelDifferenceCorrectTask::DownGainB( INT nBaseValue )
{
  m_iCurrenCorGainB -= m_iStepGainLevel;
  swpa_camera_init_set_gain_correction( m_iCurrenCorGainA , m_iCurrenCorGainB ,
                                       0 , 0  );

  swpa_camera_basicparam_set_gain( nBaseValue );
}








