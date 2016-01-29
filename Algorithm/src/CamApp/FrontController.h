#ifndef _FRONT_CONTROLLER_H_
#define _FRONT_CONTROLLER_H_

typedef struct _FrontPannelParam
{
    int iUsedAutoControllMode;
    int iAutoRunFlag;
    int iPulseWidthMin;
    int iPulseWidthMax;
    int iPulseStep;
    int iPulseLevel;
    int iPolarizingPrismMode;
    int iPalanceLightStatus;
    int iFlash1PulseWidth;
    int iFlash1Polarity;
    int iFlash1Coupling;
    int iFlash1ResistorMode;
    int iFlash2PulseWidth;
    int iFlash2Polarity;
    int iFlash2Coupling;
    int iFlash2ResistorMode;
    int iFlash3PulseWidth;
    int iFlash3Polarity;
    int iFlash3Coupling;
    int iFlash3ResistorMode;
    int iDivFrequency;
    int iOutPutDelay;

    _FrontPannelParam()
    {
        iUsedAutoControllMode = 0;
        iAutoRunFlag = 1;
        iPulseWidthMin = 5;
        iPulseWidthMax = 42;
        iPulseStep = 5;
        iPulseLevel = 0;
        iPolarizingPrismMode = -1;
        iPalanceLightStatus = 0;
        iFlash1PulseWidth = 100;
        iFlash1Polarity = 0;
        iFlash1Coupling = 0;
        iFlash1ResistorMode = 0;
        iFlash2PulseWidth = 100;
        iFlash2Polarity = 0;
        iFlash2Coupling = 0;
        iFlash2ResistorMode = 0;
        iFlash3PulseWidth = 100;
        iFlash3Polarity = 0;
        iFlash3Coupling = 0;
        iFlash3ResistorMode = 0;
        iDivFrequency = 1;
        iOutPutDelay = 0;
    };
}FrontPannelParam;

#include "hvthreadbase.h"
#include "HvSerialLink.h"

class CFrontController : public CHvThreadBase
{
public:
    CFrontController();
    ~CFrontController();
    bool OpenDevice();
    virtual HRESULT Run(void* pvParamter);
    bool GetPannelVersion(unsigned char* rgbVersionBuf, int& iBufLen);
    bool SetPolarizingPrismMode(int iMode);
    bool SetPalaceLightMode(int iMode);
    bool SetPulseWidth(int iChannel, int iPulseWidth, int iFrequncyNum);
    bool SetPannelStatus(int iWorkMode, bool fIsConfig);
    bool UpDataPannelProgram(unsigned char* rgbProgramData, int iProgramDataLen);
    bool SetPannelTime(unsigned int uiTimeMs);
    bool GetPannelTime(unsigned int& uiTimeMs);
    bool GetPannelTemperature(unsigned short& usTemperature);
    bool SetCaptureFlash(int iChannel, int iPolarity, int iTriggerType, int iPulseWidth, bool fIsCoupling);
    bool SetGridSync(int iFrequencyParam, int iDelay);
    bool SetStrobeSignelSource(int iSource);
    int GetWorkMode(void)
    {
        return m_iPannelWorkMode;
    }
    bool GetWorkStatus(void)
    {
        return m_fIsConfig;
    }
    bool UpdatePannel(unsigned char* rgbUpdateFile);
    bool GetUpdatingInfo(int& iUpdatingStatus, int& iUpdatePageIndex)
    {
        iUpdatingStatus = m_iUpdatingStatus;
        iUpdatePageIndex = m_iUpdatePageIndex;
        return TRUE;
    }
    bool GetCRCValue(int& iValue);
    int GetControllPannelRunStatus(void) { return m_iRunStatus;}

private:
    bool CheckPannelStatus();
    bool WriteUpdateFileToPannel(void);

private:
    CHvSerialLink*  m_pSerialLink;      //串口通讯类
    int      m_iPannelWorkMode;       //控制板工作模式
    bool    m_fIsConfig;                //控制板工作状态
    bool    m_fIsOpened;              //串口状态
    int      m_iUpdatingStatus;
    int      m_iUpdatePageIndex;
    int      m_iRunStatus;
    int      m_iCheckStatusFailedTimes;
    unsigned char* m_rgbUpdateFileBuffer;
};

#endif // _FRONT_CONTROLLER_H_
