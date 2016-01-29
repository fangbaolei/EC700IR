#include "AgcAwbThread.h"

#ifdef _CAM_APP_

#include "hvbaseobj.h"
#include "math.h"
#include "CameraController.h"

using namespace HiVideo;

CAgcAwbThread::CAgcAwbThread()
{
    InitAgcAwbCoreParam();
    CreateSemaphore(&m_hSemListYArg, 1, 1);
    CreateSemaphore(&m_hSemListRgbSum, 1, 1);
    CreateSemaphore(&m_hSemEnvInfo, 1, 1);

    m_envType = ENV_UNKNOW;
    m_etLast = ENV_UNKNOW;
    m_dwLastETTick = 0;    
    Start(NULL);
}

CAgcAwbThread::~CAgcAwbThread()
{
    Stop(-1);
    DestroySemaphore(&m_hSemListYArg);
    DestroySemaphore(&m_hSemListRgbSum);
    DestroySemaphore(&m_hSemEnvInfo);    
}

HRESULT CAgcAwbThread::Run(void *pvParamter)
{
    RGB_Sum cRgbSum;
    int iYArg=0,iRSum=0,iGSum=0,iBSum=0;
    float fltR = 1.0;
    float fltG = 1.0;
    float fltB = 1.0;
    bool fNeedSleep = false;

    ENV_INFO envInfo;
    while ( !m_fExit )
    {
        fNeedSleep = true;

        if ( S_OK == GetYArg(iYArg) )
        {
            //AGC调整
            if ( m_iAGCTh >=0
                    && 1 == CalcShutterGain(
                        iYArg,
                        m_iAGCTh,  // Y预设值，即：期望亮度
                        m_iAGCShutterLOri, m_iAGCShutterHOri,
                        m_iAGCGainLOri, m_iAGCGainHOri,
                        m_iNowShutter,
                        m_iNowGain) )
            {
                g_cCameraController.SetShutter(m_iNowShutter);
                g_cCameraController.SetGain(m_iNowGain);
                HV_Trace(1, "<AGC> Y-th-ss-gg-s-g[%d, %d, %d-%d, %d-%d, %d, %d]\n",
                         iYArg,
                         m_iAGCTh,
                         m_iAGCShutterLOri, m_iAGCShutterHOri,
                         m_iAGCGainLOri, m_iAGCGainHOri,
                         m_iNowShutter,
                         m_iNowGain);
            }

            fNeedSleep = false;
        }

        if ( S_OK == GetRgbSum(cRgbSum) )
        {
            iRSum = cRgbSum.iRSum;
            iGSum = cRgbSum.iGSum;
            iBSum = cRgbSum.iBSum;

            //AWB调整
            if ( iRSum > 0 && iBSum > 0 )
            {
                fltR = (float)iGSum / iRSum;
                fltB = (float)iGSum / iBSum;

                if ( 1 == CalcRGBGain(
                            fltR, m_iNowGainR,
                            fltG, m_iNowGainG,
                            fltB, m_iNowGainB,
                            -100, 511) )
                {
                    AdjustRGBGain(m_iNowGainR, m_iNowGainG, m_iNowGainB);
                    g_cCameraController.SetRgbGain(m_iNowGainR, m_iNowGainG, m_iNowGainB);
                    HV_Trace(1, "<AWB> GainR[%d]; GainG[%d]; GainB[%d]\n",
                             m_iNowGainR,m_iNowGainG,m_iNowGainB);
                }
            }

            fNeedSleep = false;
        }

        // zhaopy auto...
        if ( S_OK == GetEnvInfo(envInfo) )
        {
            DWORD32 dwCurTick = GetSystemTick();
            ENV_TYPE etTemp = CheckEnvType(envInfo);

            if ( (m_envType == ENV_BLIGHT || m_envType == ENV_FLIGHT)
                    && (etTemp == ENV_DAY && envInfo.iYPlate == 0)
                    && (dwCurTick - m_dwLastETTick) < 600000 )
            {
                //...
            }
            else if ( etTemp == ENV_UNKNOW && m_envType != ENV_UNKNOW && (dwCurTick - m_dwLastETTick) < 600000 )
            {
                //...
            }
            else
            {
                m_envType = etTemp;
                m_dwLastETTick = dwCurTick;
            }
        }
        if ( fNeedSleep )
        {
            HV_Sleep(50);
        }
    }
    return S_OK;
}
HRESULT CAgcAwbThread::PutEnvInfo(ENV_INFO& envInfo)
{
    HRESULT hr = E_FAIL;

    SemPend(&m_hSemEnvInfo);
    if ( m_listEnvInfo.GetSize() < LIST_ENVINFO_MAX_COUNT )
    {
        m_listEnvInfo.AddTail(envInfo);
        hr = S_OK;
    }
    else
    {
        m_listEnvInfo.RemoveHead();
        m_listEnvInfo.AddTail(envInfo);
        hr = S_OK;
    }
    SemPost(&m_hSemEnvInfo);

    return hr;
}

HRESULT CAgcAwbThread::GetEnvInfo(ENV_INFO& envInfo)
{
    HRESULT hr = E_FAIL;

    SemPend(&m_hSemEnvInfo);
    if ( !m_listEnvInfo.IsEmpty() )
    {
        envInfo = m_listEnvInfo.RemoveHead();
        hr = S_OK;
    }
    SemPost(&m_hSemEnvInfo);

    return hr;
}

ENV_TYPE CAgcAwbThread::CheckEnvType(ENV_INFO& envInfo)
{
    ENV_TYPE envType = ENV_UNKNOW;
    int iNeightShutter = 3200;
    int iNeightYSum = 55;
    int iNeightGain = 100;
    int iNeightPlate = 80;

    int iBLightDiff = 60;
    int iBLightShutter = 1800;
    int iBLightMaxYSum = 190;

    int iFLightDiff = 50;
    int iFLightShutter = 2000;

    int iDayMinYSum = 90;
    int iDayShutter = 1800;

    int iCloudyMinYSum = 80;
    int iCloudyShutter = 2000;


    if ( envInfo.iYPlate > 0 )
    {
        if ( envInfo.iYPlate > iNeightPlate && envInfo.iYSum < iNeightYSum && envInfo.iShutter > iNeightShutter && envInfo.iGain > iNeightGain )
        {
            envType = ENV_NEIGHT;
        }
        else if ( (envInfo.iYSum > envInfo.iYPlate)
                  && (envInfo.iYSum - envInfo.iYPlate) > iBLightDiff
                  && (envInfo.iShutter < iBLightShutter || ( envInfo.iShutter < (iBLightShutter + 600 ) && envInfo.iYSum > iBLightMaxYSum)) )
        {
            envType = ENV_BLIGHT;
        }
        else if ( (envInfo.iYPlate > envInfo.iYSum)
                  && (envInfo.iYPlate - envInfo.iYSum) > iFLightDiff && envInfo.iShutter < iFLightShutter )
        {
            envType = ENV_FLIGHT;
        }
        else if ( envInfo.iYSum > iDayMinYSum && envInfo.iShutter < iDayShutter )
        {
            envType = ENV_DAY;
        }
        else if ( envInfo.iYSum > iCloudyMinYSum && envInfo.iShutter > iCloudyShutter )
        {
            envType = ENV_CLOUDY;
        }
    }
    else
    {
        if ( envInfo.iYSum < iNeightYSum && envInfo.iShutter > iNeightShutter && envInfo.iGain > iNeightGain )
        {
            envType = ENV_NEIGHT;
        }
        else if ( envInfo.iYSum > iDayMinYSum && envInfo.iShutter < iDayShutter )
        {
            envType = ENV_DAY;
        }
        else if ( envInfo.iYSum > iCloudyMinYSum &&  envInfo.iShutter > iCloudyShutter )
        {
            envType = ENV_CLOUDY;
        }
    }

    return envType;
}

void CAgcAwbThread::SetAGCTh(int iAGCTh)
{
    m_iAGCTh = iAGCTh;
}

void CAgcAwbThread::SetAGCParam(int iShutterMin, int iShutterMax, int iGainMin, int iGainMax)
{
    m_iAGCShutterLOri = g_cCameraController.Shutter_Camyu2Raw(iShutterMin);
    m_iAGCShutterHOri = g_cCameraController.Shutter_Camyu2Raw(iShutterMax);
    m_iAGCGainLOri = GAIN_BASELINE;
    m_iAGCGainHOri = g_cCameraController.Gain_Camyu2Raw(iGainMax);

    m_iAGCShutterHOri = MAX(m_iAGCShutterHOri, m_iAGCShutterLOri);
    m_iAGCGainHOri = MAX(m_iAGCGainHOri, m_iAGCGainLOri);

    m_iNowShutter = MIN(m_iAGCShutterHOri, m_iNowShutter);
    m_iNowShutter = MAX(m_iAGCShutterLOri, m_iNowShutter);
    m_iNowGain = MIN(m_iAGCGainHOri, m_iNowGain);
    m_iNowGain = MAX(m_iAGCGainLOri, m_iNowGain);

    g_cCameraController.SetShutter(m_iNowShutter);
    g_cCameraController.SetGain(m_iNowGain);
}

void CAgcAwbThread::SetAGCShutterGain(int iShutter, int iGain)
{
    m_iNowShutter = g_cCameraController.Shutter_Camyu2Raw(iShutter);
    m_iNowGain = g_cCameraController.Gain_Camyu2Raw(iGain);
}

void CAgcAwbThread::GetAGCShutterGain(int& iShutter, int& iGain)
{
    iShutter = g_cCameraController.Shutter_Raw2Camyu(m_iNowShutter);
    iGain = g_cCameraController.Gain_Raw2Camyu(m_iNowGain);
}

void CAgcAwbThread::SetAWBGain(int iGainR, int iGainG, int iGainB)
{
    m_iNowGainR = g_cCameraController.ColorGain_Camyu2Raw(iGainR);
    m_iNowGainG = g_cCameraController.ColorGain_Camyu2Raw(iGainG);
    m_iNowGainB = g_cCameraController.ColorGain_Camyu2Raw(iGainB);
}

void CAgcAwbThread::GetAWBGain(int& iGainR, int& iGainG, int& iGainB)
{
    iGainR = g_cCameraController.ColorGain_Raw2Camyu(m_iNowGainR);
    iGainG = g_cCameraController.ColorGain_Raw2Camyu(m_iNowGainG);
    iGainB = g_cCameraController.ColorGain_Raw2Camyu(m_iNowGainB);
}

HRESULT CAgcAwbThread::PutYArg(int iYArg)
{
    HRESULT hr = E_FAIL;

    SemPend(&m_hSemListYArg);
    if ( m_listYArg.GetSize() < LIST_YARG_MAX_COUNT )
    {
        m_listYArg.AddTail(iYArg);
        hr = S_OK;
    }
    else
    {
        m_listYArg.RemoveHead();
        m_listYArg.AddTail(iYArg);
        hr = S_OK;
    }
    SemPost(&m_hSemListYArg);

    return hr;
}

HRESULT CAgcAwbThread::PutRgbSum(RGB_Sum cRgbSum)
{
    HRESULT hr = E_FAIL;

    SemPend(&m_hSemListRgbSum);
    if ( m_listRgbSum.GetSize() < LIST_RGBSUM_MAX_COUNT )
    {
        m_listRgbSum.AddTail(cRgbSum);
        hr = S_OK;
    }
    else
    {
        m_listRgbSum.RemoveHead();
        m_listRgbSum.AddTail(cRgbSum);
        hr = S_OK;
    }
    SemPost(&m_hSemListRgbSum);

    return hr;
}

HRESULT CAgcAwbThread::GetYArg(int& iYArg)
{
    HRESULT hr = E_FAIL;

    SemPend(&m_hSemListYArg);
    if ( !m_listYArg.IsEmpty() )
    {
        iYArg = m_listYArg.RemoveHead();
        hr = S_OK;
    }
    SemPost(&m_hSemListYArg);

    return hr;
}

HRESULT CAgcAwbThread::GetRgbSum(RGB_Sum& cRgbSum)
{
    HRESULT hr = E_FAIL;

    SemPend(&m_hSemListRgbSum);
    if ( !m_listRgbSum.IsEmpty() )
    {
        cRgbSum = m_listRgbSum.RemoveHead();
        hr = S_OK;
    }
    SemPost(&m_hSemListRgbSum);

    return hr;
}

void CAgcAwbThread::InitAgcAwbCoreParam()
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
    m_iAwbStep = 100;

    m_fltGainRSum = 0;
    m_fltGainGSum = 0;
    m_fltGainBSum = 0;

    m_iAwbMaxCount = 0;

    memset(&m_cPIDShutter, 0, sizeof(m_cPIDShutter));
    memset(&m_cPIDGain, 0, sizeof(m_cPIDGain));
    m_PIDInitFlag = false;

    m_iAGCEffectCounter = 0;
    m_iAGCAdjustDirection = 0;

    m_iAGCTh = 0;
    m_iAGCShutterLOri = 0;
    m_iAGCShutterHOri = 0;
    m_iAGCGainLOri = 0;
    m_iAGCGainHOri = 0;
    m_iNowShutter = 0;
    m_iNowGain = 0;
    m_iNowGainR = 0;
    m_iNowGainG = 0;
    m_iNowGainB = 0;
}

int CAgcAwbThread::CalcRGBGain(
    float fltGainR, int &iGainR,
    float fltGainG, int &iGainG,
    float fltGainB, int &iGainB,
    int iMin, int iMax
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
        m_fltGainRSum = 0;
        m_fltGainGSum = 0;
        m_fltGainBSum = 0;

        int iRet = 0;

        if ( fabs(1.0f - fltGainR) >= m_fltAwbAccurate )
        {
            AWB_ADJUST(iGainR, fltGainR, m_iAwbStep, iMin, iMax);
            iRet = 1;
        }
        if ( fabs(1.0f - fltGainG) >= m_fltAwbAccurate )
        {
            AWB_ADJUST(iGainG, fltGainG, m_iAwbStep, iMin, iMax);
            iRet = 1;
        }
        if ( fabs(1.0f - fltGainB) >= m_fltAwbAccurate )
        {
            AWB_ADJUST(iGainB, fltGainB, m_iAwbStep, iMin, iMax);
            iRet = 1;
        }

        return iRet;
    }
}

int CAgcAwbThread::AdjustRGBGain(int &iGainR, int &iGainG, int &iGainB)
{
    int iMin = MIN(iGainR, MIN(iGainG, iGainB));
    iGainR -= iMin;
    iGainG -= iMin;
    iGainB -= iMin;
    iGainR = MIN(iGainR, 511);
    iGainG = MIN(iGainG, 511);
    iGainB = MIN(iGainB, 511);
    return 0;
}

double CAgcAwbThread::PIDCale(PID *p, double feedback)
{
    double Err=0,dErr=0;

    Err = p->Command - feedback;	//当前误差
    p->sumErr += Err;				//误差累加
    dErr = Err - p->preErr;			//误差微分
    p->preErr = Err;

    return (p->Proportion * Err		//比例项
            + p->Derivative * dErr	    //微分项
            + p->Integral * p->sumErr);	//积分项
}

void CAgcAwbThread::ResetAGC()
{
    m_iAGCEffectCounter = 0;
    m_iAGCAdjustDirection = 0;
}

int CAgcAwbThread::CalcShutterGain(
    int iYArg,      //当前平均亮度（0~255）
    int iAGCTh,     //期望平均亮度（0~255）
    int iAGCShutterL,
    int iAGCShutterH,
    int iAGCGainL,
    int iAGCGainH,
    int &iShutter,
    int &iGain
)
{
    const int iDiff = abs(iYArg - iAGCTh);
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
    const int iAGCShutterMid = iAGCShutterL + int((iAGCShutterH - iAGCShutterL) * 0.8);

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
            iShutter += (int)PIDCale(&m_cPIDShutter, iYArg);
            iShutter = MIN(iAGCShutterMid, iShutter);
            iShutter = MAX(iAGCShutterL, iShutter);

            if (iGain > iAGCGainL)
            {
                iGain -= ((iGain - iAGCGainL)>>4);
            }
        }
        else if (iGain < iAGCGainH)
        {
            iGain += (int)PIDCale(&m_cPIDGain, iYArg);
            iGain = MIN(iAGCGainH, iGain);
            iGain = MAX(iAGCGainL, iGain);
        }
        else if (iShutter < iAGCShutterH)
        {
            iShutter += (int)PIDCale(&m_cPIDShutter, iYArg);
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
            iShutter += (int)PIDCale(&m_cPIDShutter, iYArg);
            iShutter = MIN(iAGCShutterH, iShutter);
            iShutter = MAX(iAGCShutterMid, iShutter);
        }
        else if (iGain > iAGCGainL)
        {
            iGain += (int)PIDCale(&m_cPIDGain, iYArg);
            iGain = MIN(iAGCGainH, iGain);
            iGain = MAX(iAGCGainL, iGain);

            if (iShutter < iAGCShutterH)
            {
                iShutter += ((iAGCShutterH - iShutter)>>4);
            }
        }
        else if (iShutter > iAGCShutterL)
        {
            iShutter += (int)PIDCale(&m_cPIDShutter, iYArg);
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

#endif // _CAM_APP_
