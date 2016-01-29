#ifndef CSWCHANNELDIFFERENCECORRECT_H
#define CSWCHANNELDIFFERENCECORRECT_H
#include "SWTask.h"

class CSWAgcAwbTask;
class CSWChannelDifferenceCorrectTask : public CSWTask
{
CLASSINFO(CSWChannelDifferenceCorrectTask, CSWTask)
public:
    CSWChannelDifferenceCorrectTask( CSWAgcAwbTask *pSWAgcAwbTask );
    virtual ~CSWChannelDifferenceCorrectTask();

    virtual SInt64 Run( );

private:
    // 通道差异性调整
    VOID YinYangSignalCheck( FLOAT fYinyangSideFactor , FLOAT fConsisIndex);

    VOID ClearYinYangInfo( );
private:
    CSWAgcAwbTask *m_pSWAgcAwbTask;

    INT m_iValueTapA;
    INT m_iValueTapB;

    INT m_iCurrenCorGainA;
    INT m_iCurrenCorGainB;

    INT m_iStepGainLevel;
    INT m_iStep ;


private:
    VOID UpBlackA( );
    VOID UpBlackB( );

    VOID DownBlackA( );
    VOID DownBlackB( );

    VOID UpGainA( INT nBaseValue );
    VOID UpGainB( INT nBaseValue );

    VOID DownGainA( INT nBaseValue );
    VOID DownGainB( INT nBaseValue );
};

#endif // CSWCHANNELDIFFERENCECORRECT_H

