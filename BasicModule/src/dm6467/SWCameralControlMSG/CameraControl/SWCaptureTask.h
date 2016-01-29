#ifndef CSWCAPTURE_H
#define CSWCAPTURE_H
#include "SWTask.h"

class CSWCaptureTask : public CSWTask
{
CLASSINFO(CSWCaptureTask, CSWTask)

public:
    CSWCaptureTask();
    virtual ~CSWCaptureTask();


    // 设置增益使能
    INT SetRGBEnable( BOOL isEnable );
    // 设置快门使能
    INT SetShutterEnable( BOOL isEnable );
    // 设置增益使能
    INT SetGainEnable( BOOL isEnable );


    // 设置当前快门
    VOID SetShutter(INT iShutter);
    // 获取当前快门
    VOID GetShutter(INT& iShutter);
    // 设置当前增益
    VOID SetGain(INT iGain);
    // 获取当前增益
    VOID GetGain( INT& iGain);

    // 设置当前增益值，格式为0x00RGB,R为R通道增益，G为G通道增益值 ， B为B通道增益值
    VOID SetRGBGain( DWORD dwRGBGain );
     // 获取当前增益值，格式为0x00RGB,R为R通道增益，G为G通道增益值 ， B为B通道增益值
    VOID GetRGBGain( DWORD &dwRGBGain );

	virtual SInt64 Run();

private:


    // 抓拍差异性矫正
    void SetCaptureYinyangSideFactor( float fCaptureFactor );
private:
    INT m_iCaptureShutter;
    INT m_iCaptureGain;

    INT m_iNowGainR;
    INT m_iNowGainG;
    INT m_iNowGainB;

};

#endif // CSWCAPTURE_H

