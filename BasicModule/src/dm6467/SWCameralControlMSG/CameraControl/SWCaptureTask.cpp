#include "SWCaptureTask.h"
#include "swpa_camera.h"
#include "SWLog.h"
#include "SWImageStatisticsInfo.h"

CSWCaptureTask::CSWCaptureTask()
: m_iCaptureShutter( 0 )
, m_iCaptureGain( 0 )
, m_iNowGainR( 0 )
, m_iNowGainG( 0 )
, m_iNowGainB( 0 )

{
    //ctor
}

CSWCaptureTask::~CSWCaptureTask()
{
    //dtor
}

// 设置增益使能
INT CSWCaptureTask::SetRGBEnable( BOOL isEnable )
{
    if( !isEnable )
        return swpa_camera_basicparam_set_capture_rgb_gain( -1 , -1 , -1);
    else
        return swpa_camera_basicparam_set_capture_rgb_gain( m_iNowGainR , m_iNowGainG , m_iNowGainB);
}
// 设置快门使能
INT CSWCaptureTask::SetShutterEnable( BOOL isEnable )
{
    return swpa_camera_basicparam_set_capture_shutter( (isEnable ? m_iCaptureShutter : -1));
}
// 设置是否使能
INT CSWCaptureTask::SetGainEnable( BOOL isEnable )
{
    return swpa_camera_basicparam_set_capture_gain( (isEnable ? m_iCaptureGain: -1));
}
// 设置当前快门
VOID CSWCaptureTask::SetShutter(INT iShutter)
{
    swpa_camera_basicparam_set_capture_shutter( m_iCaptureShutter = iShutter );
}
// 获取当前快门
VOID CSWCaptureTask::GetShutter(INT& iShutter)
{
    iShutter = m_iCaptureShutter;
}
// 设置当前增益
VOID CSWCaptureTask::SetGain(INT iGain)
{
    swpa_camera_basicparam_set_capture_gain( (m_iCaptureGain = iGain) );
}
// 获取当前增益
VOID CSWCaptureTask::GetGain( INT& iGain)
{
    iGain = m_iCaptureGain;
}
// 设置当前增益值，格式为0x00RGB,R为R通道增益，G为G通道增益值 ， B为B通道增益值
VOID CSWCaptureTask::SetRGBGain( DWORD dwRGBGain )
{
	if( dwRGBGain == 0xFF000000 )
	{
		m_iNowGainR = -1;
		m_iNowGainG = -1;
		m_iNowGainB = -1;
	}
	else
	{
		m_iNowGainR = ((dwRGBGain & 0x00FF0000) >> 16);
		m_iNowGainG = ((dwRGBGain & 0x0000FF00) >> 8);
		m_iNowGainB = (dwRGBGain & 0x000000FF);
	}

    swpa_camera_basicparam_set_capture_rgb_gain( m_iNowGainR , m_iNowGainG , m_iNowGainB);
}
 // 获取当前增益值，格式为0x00RGB,R为R通道增益，G为G通道增益值 ， B为B通道增益值
VOID CSWCaptureTask::GetRGBGain( DWORD &dwRGBGain )
{
    dwRGBGain = m_iNowGainB;
    dwRGBGain |= (m_iNowGainG << 8);
    dwRGBGain |= (m_iNowGainR << 16);
}
SInt64 CSWCaptureTask::Run()
{
 	EventFlags events = this->GetEvents( );
	if( events & CSWTask::kIdleEvent ) return 0;

	while( !(events & CSWTask::kKillEvent) )
	{
        FLOAT fDifferenceFactor ; BOOL IsUpdate = false;
        CSWImageStatisticsInfo::GetCaptureDiffStatistValue( fDifferenceFactor , IsUpdate);
        // 调整抓拍差异性
        if( IsUpdate ) this->SetCaptureYinyangSideFactor( fDifferenceFactor );

        return 200;
	}

    return -1;
}
void CSWCaptureTask::SetCaptureYinyangSideFactor( float fCaptureFactor )
{
#define MINBOUNDER_VALUE 0.005
static int fnAChannelCurrCaptureGain = 0 ;
static int fnBChannelCurrCaptureGain = 0 ;
static int fCurrCaptureGainEnable = 0;
static int fnOldGain = 0;

    int nCurrentCaptureGain = 0; this->GetGain( nCurrentCaptureGain );

    // 如果增益改变则重新调整
    if( fnOldGain != nCurrentCaptureGain ){
        fnOldGain = nCurrentCaptureGain;
        fnAChannelCurrCaptureGain = fnBChannelCurrCaptureGain = 0;

        swpa_camera_init_set_capture_gain_correction( fnAChannelCurrCaptureGain
                                                  , fnBChannelCurrCaptureGain , 0 , 0);
    }

    /* dengtx(2013-01-08)
     增加获取统计信息，进行抓拍阴阳面修正
    */
    float fDiffValue = 0.0f;

    fDiffValue = fabs(1.0 - fCaptureFactor);

    if( fDiffValue < MINBOUNDER_VALUE){
      //  SW_TRACE(1,"Current Capture yinyang side factor %f; the differe = %f\n",fCaptureFactor,fDiffValue);
        return ;
    }

        if( fCaptureFactor > 1.0 ){
        if( fnAChannelCurrCaptureGain > 0 ){
            fnAChannelCurrCaptureGain -= 1;
        }
        else{
            fnBChannelCurrCaptureGain += 1;
        }
    }
    else{

        if( fnBChannelCurrCaptureGain > 0 ){
            fnBChannelCurrCaptureGain -= 1;
        }
        else{
            fnAChannelCurrCaptureGain += 1;
        }
    }

    // 修正增益
    swpa_camera_init_set_capture_gain_correction( fnAChannelCurrCaptureGain
                                                  , fnBChannelCurrCaptureGain , 0 , 0);

    // 重新设置增益
    this->SetGain(m_iCaptureGain );

    SW_TRACE_DEBUG( "【Capture Gain】 A=%d;B=%d;fDiffValue=%f;fCaptureFactor=%f;nCurrentCaptureGain=%d;fnOldGain=%d\r\n"
           ,fnAChannelCurrCaptureGain,fnBChannelCurrCaptureGain,fDiffValue
           ,fCaptureFactor,nCurrentCaptureGain,fnOldGain);

}

