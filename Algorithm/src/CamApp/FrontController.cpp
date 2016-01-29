#include "FrontController.h"

CFrontController::CFrontController()
{
    m_pSerialLink = NULL;
    m_fIsConfig = false;
    m_fIsOpened = false;
    m_iPannelWorkMode = 1;
    m_iUpdatingStatus = -1;
    m_iUpdatePageIndex = 0;
    m_rgbUpdateFileBuffer = NULL;
    m_iRunStatus = 0;
    m_iCheckStatusFailedTimes = 0;
}

CFrontController::~CFrontController()
{
    if (m_pSerialLink)
    {
        delete m_pSerialLink;
        m_pSerialLink = NULL;
    }
}

bool CFrontController::OpenDevice()
{
    if (m_fIsOpened == true) return true;
    if (m_pSerialLink == NULL)
    {
        m_pSerialLink = new CHvSerialLink;
        if (m_pSerialLink == NULL)
        {
            return false;
        }
    }
    if (m_pSerialLink->Open("/dev/ttyS0") != S_OK)
    {
        return false;
    }
    HV_Trace(5, "Open Device Sucess\n");
    m_fIsOpened = true;

    char szTempPannelVersionString[256] = {0};
    int iBufLen = 256;
    int iRetryTimes = 0;
    while(iRetryTimes < 3)
    {
        if(GetPannelVersion((unsigned char*)szTempPannelVersionString, iBufLen) == true)
        {
            break;
        }
        iRetryTimes++;
    }

    if(iRetryTimes < 3)
    {
        if(strstr(szTempPannelVersionString, "PSD v3.0"))
        {
            m_iRunStatus = 1;
        }
    }

    return true;
}

HRESULT CFrontController::Run(void* pvParamter)
{
    m_fExit = false;
    int i = 0;
    while (!m_fExit)
    {
        if (!m_fIsOpened)
        {
            OpenDevice();
            HV_Sleep(1000);
            continue;
        }
        if (m_iUpdatingStatus != -1)
        {
            WriteUpdateFileToPannel();
        }
        if(CheckPannelStatus() == false)
        {
            m_iCheckStatusFailedTimes++;
            if(m_iCheckStatusFailedTimes >= 3)
            {
                m_iCheckStatusFailedTimes = 3;
                m_iRunStatus = 0;
            }
        }
        else
        {
            m_iCheckStatusFailedTimes = 0;
            if(m_iRunStatus == 0)
            {
                char szTempPannelVersionString[256] = {0};
                int iBufLen = 256;
                if(GetPannelVersion((unsigned char*)szTempPannelVersionString, iBufLen) == true)
                {
                    if(strstr(szTempPannelVersionString, "PSD v3.0"))
                    {
                        m_iRunStatus = 1;
                    }
                }
            }
        }
        HV_Sleep(2000);
        if(i++ > 10)
        {
            HV_Trace(5, "CheckPannelStatus...");
            i = 0;
        }
    }
    return S_OK;
}

bool CFrontController::CheckPannelStatus()
{
    if (!m_fIsOpened)
    {
        m_iPannelWorkMode = 0;
        m_fIsConfig = false;
        return true;
    }
    if (m_pSerialLink->Lock(1000) != 0)
    {
        return false;
    }
    if (m_pSerialLink->SendCmdData(0x03, NULL, 0) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    unsigned char rgbRecvData[32] = {0};
    unsigned int iRecvLen = 32;
    if (m_pSerialLink->RecvPacket(rgbRecvData, &iRecvLen, 1000) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    if (rgbRecvData[2] != 0x03 || rgbRecvData[3] != 0)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    m_iPannelWorkMode = (int)rgbRecvData[4];
    if (rgbRecvData[5] == 1) m_fIsConfig = true;
    else m_fIsConfig = false;
    m_pSerialLink->UnLock();
    unsigned short usPulseWidth = 0;
    HV_Trace(3, "\n\n倍频:%d\n", rgbRecvData[6]);
    HV_Trace(3, "\n\n电网同步: 分频系数=%d, 输出延迟=%d, 输出脉宽=%d\n",
        rgbRecvData[7], rgbRecvData[8], rgbRecvData[9]);
    HV_Trace(3, "\n\n频闪脉宽: CHN1=%d, CHN2=%d, CHN3=%d, CHN4=%d\n",
        rgbRecvData[10], rgbRecvData[11], rgbRecvData[12], rgbRecvData[13]);
    HV_Trace(3, "\n\n抓拍系数:\n");
    usPulseWidth = (rgbRecvData[17]<<8) | rgbRecvData[16];
    HV_Trace(3, "CHN1:极性=%d, 触发类型=%d, 脉宽=%d, 耦合=%d\n",
        rgbRecvData[14], rgbRecvData[15], usPulseWidth, rgbRecvData[18]);
    usPulseWidth = (rgbRecvData[22]<<8) | rgbRecvData[21];
    HV_Trace(3, "CHN2:极性=%d, 触发类型=%d, 脉宽=%d, 耦合=%d\n",
        rgbRecvData[19], rgbRecvData[20], usPulseWidth, rgbRecvData[23]);
    usPulseWidth = (rgbRecvData[27]<<8) | rgbRecvData[26];
    HV_Trace(3, "CHN3:极性=%d, 触发类型=%d, 脉宽=%d, 耦合=%d\n",
        rgbRecvData[24], rgbRecvData[25], usPulseWidth, rgbRecvData[28]);
    return true;
}

bool CFrontController::GetPannelVersion(unsigned char* rgbVersionBuf, int& iBufLen)
{
    if (rgbVersionBuf == NULL)
    {
        return false;
    }
    if (!m_fIsOpened)
    {
        iBufLen = 0;
        return false;
    }
    if (m_pSerialLink->Lock(1000) != 0)
    {
        iBufLen = 0;
        return false;
    }
    unsigned char bData = 0;
    if (m_pSerialLink->SendCmdData(0x0B, &bData, 1) != S_OK)
    {
        iBufLen = 0;
        m_pSerialLink->UnLock();
        return false;
    }
    unsigned char rgbTempVersionString[256] = {0};
    unsigned char rgbRecvData[128] = {0};
    unsigned int iRecvLen = 128;
    if (m_pSerialLink->RecvPacket(rgbRecvData, &iRecvLen, 1000) != S_OK)
    {
        iBufLen = 0;
        m_pSerialLink->UnLock();
        return false;
    }
    if (rgbRecvData[2] != 0x0B || rgbRecvData[3] != 0 || rgbRecvData[4] != 0)
    {
        iBufLen = 0;
        m_pSerialLink->UnLock();
        return false;
    }
    memcpy(rgbTempVersionString, "引导程序版本号：", 16);
    memcpy(rgbTempVersionString+16, rgbRecvData+5, 64);
    memcpy(rgbTempVersionString+80, "    正式程序版本号：", 20);
    memset(rgbRecvData, 0, 128);
    iRecvLen = 128;
    bData = 1;
    if (m_pSerialLink->SendCmdData(0x0B, &bData, 1) != S_OK)
    {
        iBufLen = 0;
        m_pSerialLink->UnLock();
        return false;
    }
    if (m_pSerialLink->RecvPacket(rgbRecvData, &iRecvLen, 1000) != S_OK)
    {
        iBufLen = 0;
        m_pSerialLink->UnLock();
        return false;
    }
    if (rgbRecvData[2] != 0x0B || rgbRecvData[3] != 0 || rgbRecvData[4] != 1)
    {
        iBufLen = 0;
        m_pSerialLink->UnLock();
        return false;
    }
    memcpy(rgbTempVersionString+100, rgbRecvData+5, 64);
    if (iBufLen < 164)
    {
        iBufLen = 0;
        m_pSerialLink->UnLock();
        return false;
    }
    memset(rgbVersionBuf, 0, iBufLen);
    memcpy(rgbVersionBuf, rgbTempVersionString, 164);
    iBufLen = 164;
    m_pSerialLink->UnLock();
    return true;
}

bool CFrontController::SetPolarizingPrismMode(int iMode)
{
    if (iMode != 0 && iMode != 1)
    {
        return false;
    }
    if (!m_fIsOpened)
    {
        return false;
    }
    if(m_iRunStatus != 1)
    {
        return false;
    }
    if(m_iPannelWorkMode != 1)
    {
        return false;
    }
    if (m_pSerialLink->Lock(1000) != 0)
    {
        return false;
    }
    unsigned char rgbCmdData[2];
    rgbCmdData[0] = 0x03;
    if (iMode == 0)
        rgbCmdData[1] = 0;
    else
        rgbCmdData[1] = 1;
    if (m_pSerialLink->SendCmdData(0x0A, rgbCmdData, 2) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    unsigned char rgbRecvData[32] = {0};
    unsigned int iRecvLen = 32;
    if (m_pSerialLink->RecvPacket(rgbRecvData, &iRecvLen, 1000) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    if (rgbRecvData[2] != 0x0A || rgbRecvData[3] != 0)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    m_pSerialLink->UnLock();
    return true;
}

bool CFrontController::SetPalaceLightMode(int iMode)
{
    if (iMode != 0 && iMode != 1)
    {
        return false;
    }
    if (!m_fIsOpened)
    {
        return false;
    }
    if(m_iRunStatus != 1)
    {
        return false;
    }
    if(m_iPannelWorkMode != 1)
    {
        return false;
    }
    if (m_pSerialLink->Lock(1000) != 0)
    {
        return false;
    }
    unsigned char rgbCmdData[2];
    rgbCmdData[0] = 0x00;
    if (iMode == 0)
        rgbCmdData[1] = 0;
    else
        rgbCmdData[1] = 1;
    if (m_pSerialLink->SendCmdData(0x0A, rgbCmdData, 2) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    unsigned char rgbRecvData[32] = {0};
    unsigned int iRecvLen = 32;
    if (m_pSerialLink->RecvPacket(rgbRecvData, &iRecvLen, 1000) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    if (rgbRecvData[2] != 0x0A || rgbRecvData[3] != 0)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    m_pSerialLink->UnLock();
    return true;
}

bool CFrontController::SetPulseWidth(int iChannel, int iPulseWidth, int iFrequncyNum)
{
    if (iChannel < 0 || iChannel > 3)
    {
        return false;
    }
    if (iPulseWidth < 0 || iPulseWidth > 80)
    {
        return false;
    }
    if (iFrequncyNum < 0 || iFrequncyNum > 10)
    {
        return false;
    }
    if (!m_fIsOpened)
    {
        return false;
    }
    if(m_iRunStatus != 1)
    {
        return false;
    }
    if(m_iPannelWorkMode != 1)
    {
        return false;
    }
    if (m_pSerialLink->Lock(1000) != 0)
    {
        return false;
    }
    unsigned char rgbCmdData[3];
    rgbCmdData[0] = 0x01;
    rgbCmdData[1] = iChannel;
    rgbCmdData[2] = (int)((iPulseWidth < 1) ? 1 : iPulseWidth);
    //if (rgbCmdData[2] < 5) rgbCmdData[2] = 5;
    //rgbCmdData[3] = iFrequncyNum;
    if (m_pSerialLink->SendCmdData(0x06, rgbCmdData, 3) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    unsigned char rgbRecvData[32] = {0};
    unsigned int iRecvLen = 32;
    if (m_pSerialLink->RecvPacket(rgbRecvData, &iRecvLen, 1000) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    if (rgbRecvData[2] != 0x06 || rgbRecvData[3] != 0)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    m_pSerialLink->UnLock();
    return true;
}

bool CFrontController::SetPannelStatus(int iWorkMode, bool fIsConfig)
{
    if (iWorkMode != 1 && iWorkMode != 2)
    {
        return false;
    }
    if (!m_fIsOpened)
    {
        return false;
    }
    if(m_iRunStatus != 1)
    {
        return false;
    }
    if (m_pSerialLink->Lock(1000) != 0)
    {
        return false;
    }
    unsigned char rgbCmdData[2] = {0};
    rgbCmdData[0] = iWorkMode;
    if (fIsConfig)
        rgbCmdData[1] = 1;
    else
        rgbCmdData[1] = 0;
    if (m_pSerialLink->SendCmdData(0x02, rgbCmdData, 2) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    unsigned char rgbRecvData[32] = {0};
    unsigned int iRecvLen = 32;
    if (m_pSerialLink->RecvPacket(rgbRecvData, &iRecvLen, 1000) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    if (rgbRecvData[2] != 0x02 || rgbRecvData[3] != 0)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    m_pSerialLink->UnLock();
    CheckPannelStatus();
    return true;
}

bool CFrontController::UpDataPannelProgram(unsigned char* rgbProgramData, int iProgramDataLen)
{
    if (rgbProgramData == NULL || iProgramDataLen <= 0)
    {
        HV_Trace(5, "[Line:%d]UpDataPannelProgram rgbProgramData = 0x%08x, iProgramDataLen = %d, error.", __LINE__, rgbProgramData, iProgramDataLen);
        return false;
    }
    if (m_fIsConfig == true || m_iPannelWorkMode != 2)
    {
        HV_Trace(5, "[Line:%d]UpDataPannelProgram m_fIsConfig = %d, m_iPannelWorkMode = %d, error.", __LINE__, m_fIsConfig, m_iPannelWorkMode);
        return false;
    }
    if (!m_fIsOpened)
    {
        HV_Trace(5, "[Line:%d]UpDataPannelProgram m_fIsOpened = %d, error.", __LINE__, m_fIsOpened);
        return false;
    }
    if (m_pSerialLink->Lock(1000) != 0)
    {
        HV_Trace(5, "[Line:%d]UpDataPannelProgram m_pSerialLink->Lock(1000) time out, error.", __LINE__);
        return false;
    }
    int iRemainLen = iProgramDataLen;
    unsigned char* pTempIndex = rgbProgramData;
    unsigned char  rgbPageData[258] = {0};
    unsigned short usPageIndex = 0;
    unsigned char rgbRecvData[32] = {0};
    unsigned int iRecvLen = 32;
    m_iUpdatePageIndex = 0;
    while (iRemainLen > 0)
    {
        rgbPageData[1] = (usPageIndex>>8) & 0xFF;
        rgbPageData[0] = (usPageIndex) & 0xFF;
        memset(rgbPageData+2, 0xFF, 256);
        m_iUpdatePageIndex++;
        if (iRemainLen <= 256)
        {
            memcpy(rgbPageData+2, pTempIndex, iRemainLen);
            if (m_pSerialLink->SendCmdData(0x0C, rgbPageData, 258) != S_OK)
            {
                m_pSerialLink->UnLock();
                HV_Trace(5, "[Line:%d]UpDataPannelProgram m_pSerialLink->SendCmdData page index = %d, error.", __LINE__, m_iUpdatePageIndex);
                return false;
            }
            memset(rgbRecvData, 0, 32);
            iRecvLen = 32;
            if (m_pSerialLink->RecvPacket(rgbRecvData, &iRecvLen, 1000) != S_OK)
            {
                m_pSerialLink->UnLock();
                HV_Trace(5, "[Line:%d]UpDataPannelProgram m_pSerialLink->RecvPacket page index = %d, error.", __LINE__, m_iUpdatePageIndex);
                return false;
            }
            if (rgbRecvData[2] != 0x0C || rgbRecvData[3] != 0)
            {
                m_pSerialLink->UnLock();
                HV_Trace(5, "[Line:%d]UpDataPannelProgram rgbRecvData[2] = 0x02x, rgbRecvData[3] = %d, error.", __LINE__, rgbRecvData[2], rgbRecvData[3]);
                return false;
            }
            usPageIndex++;
            if (usPageIndex > 481)
            {
                m_pSerialLink->UnLock();
                HV_Trace(5, "[Line:%d]UpDataPannelProgram usPageIndex = %d, error.", __LINE__, usPageIndex);
                return false;
            }
            break;
        }
        memcpy(rgbPageData+2, pTempIndex, 256);
        if (m_pSerialLink->SendCmdData(0x0C, rgbPageData, 258) != S_OK)
        {
            m_pSerialLink->UnLock();
            HV_Trace(5, "[Line:%d]UpDataPannelProgram m_pSerialLink->SendCmdData, error.", __LINE__);
            return false;
        }
        memset(rgbRecvData, 0, 32);
        iRecvLen = 32;
        if (m_pSerialLink->RecvPacket(rgbRecvData, &iRecvLen, 1000) != S_OK)
        {
            m_pSerialLink->UnLock();
            HV_Trace(5, "[Line:%d]UpDataPannelProgram m_pSerialLink->RecvPacket, error.", __LINE__);
            return false;
        }
        if (rgbRecvData[2] != 0x0C || rgbRecvData[3] != 0)
        {
            m_pSerialLink->UnLock();
            HV_Trace(5, "[Line:%d]UpDataPannelProgram rgbRecvData[2] = 0x02x, rgbRecvData[3] = %d, error.", __LINE__, rgbRecvData[2], rgbRecvData[3]);
            return false;
        }
        usPageIndex++;
        if (usPageIndex > 481)
        {
            m_pSerialLink->UnLock();
            HV_Trace(5, "[Line:%d]UpDataPannelProgram usPageIndex = %d, error.", __LINE__, usPageIndex);
            return false;
        }
        pTempIndex += 256;
        iRemainLen -= 256;
    }
    m_pSerialLink->UnLock();
    if (usPageIndex > 481)
    {
        HV_Trace(5, "[Line:%d]UpDataPannelProgram usPageIndex = %d, error.", __LINE__, usPageIndex);
        return false;
    }
    return true;
}

bool CFrontController::SetPannelTime(unsigned int uiTimeMs)
{
    if (!m_fIsOpened)
    {
        return false;
    }
    if(m_iRunStatus != 1)
    {
        return false;
    }
    if(m_iPannelWorkMode != 1)
    {
        return false;
    }
    if (m_pSerialLink->Lock(1000) != 0)
    {
        return false;
    }
    unsigned char rgbCmdData[4];
    memcpy(rgbCmdData, &uiTimeMs, 4);
    if (m_pSerialLink->SendCmdData(0x07, rgbCmdData, 4) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    unsigned char rgbRecvData[32] = {0};
    unsigned int iRecvLen = 32;
    if (m_pSerialLink->RecvPacket(rgbRecvData, &iRecvLen, 1000) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    if (rgbRecvData[2] != 0x07 || rgbRecvData[3] != 0)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    m_pSerialLink->UnLock();
    return true;
}

bool CFrontController::GetPannelTime(unsigned int& uiTimeMs)
{
    if (!m_fIsOpened)
    {
        return false;
    }
    if (m_pSerialLink->Lock(1000) != 0)
    {
        return false;
    }
    if (m_pSerialLink->SendCmdData(0x08, 0, 0) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    unsigned char rgbRecvData[32] = {0};
    unsigned int iRecvLen = 32;
    if (m_pSerialLink->RecvPacket(rgbRecvData, &iRecvLen, 1000) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    if (rgbRecvData[2] != 0x08 || rgbRecvData[3] != 0)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    memcpy(&uiTimeMs, rgbRecvData+4, 4);
    m_pSerialLink->UnLock();
    return true;
}

bool CFrontController::GetPannelTemperature(unsigned short& usTemperature)
{
    if (!m_fIsOpened)
    {
        return false;
    }
    if(m_iRunStatus != 1)
    {
        return false;
    }
    if (m_pSerialLink->Lock(1000) != 0)
    {
        return false;
    }
    if (m_pSerialLink->SendCmdData(0x09, NULL, 0) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    unsigned char rgbRecvData[32] = {0};
    unsigned int iRecvLen = 32;
    if (m_pSerialLink->RecvPacket(rgbRecvData, &iRecvLen, 1000) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    if (rgbRecvData[2] != 0x09 || rgbRecvData[3] != 0)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    memcpy(&usTemperature, rgbRecvData+4, 2);
    m_pSerialLink->UnLock();
    return true;
}

bool CFrontController::SetCaptureFlash(int iChannel, int iPolarity, int iTriggerType, int iPulseWidth, bool fIsCoupling)
{
    if (!m_fIsOpened)
    {
        return false;
    }
    if(m_iRunStatus != 1)
    {
        return false;
    }
    if(m_iPannelWorkMode != 1)
    {
        return false;
    }
    if (iChannel < 0 || iChannel > 2)
    {
        return false;
    }
    if (iPolarity != 0 && iPolarity != 1)
    {
        return false;
    }
    if (iTriggerType != 0 && iTriggerType != 1)
    {
        return false;
    }
    unsigned char rgbCmdData[7] = {0};
    rgbCmdData[0] = 0x02;
    rgbCmdData[1] = iChannel;
    rgbCmdData[2] = iPolarity;
    rgbCmdData[3] = iTriggerType;
    rgbCmdData[5] = (iPulseWidth>>8) & 0xFF;
    rgbCmdData[4] = iPulseWidth & 0xFF;
    rgbCmdData[6] = (fIsCoupling == true ? 1 : 0);
    if (m_pSerialLink->Lock(1000) != 0)
    {
        return false;
    }
    if (m_pSerialLink->SendCmdData(0x06, rgbCmdData, 7) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    unsigned char rgbRecvData[32] = {0};
    unsigned int iRecvLen = 32;
    if (m_pSerialLink->RecvPacket(rgbRecvData, &iRecvLen, 1000) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    if (rgbRecvData[2] != 0x06 || rgbRecvData[3] != 0)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    m_pSerialLink->UnLock();
    return true;
}

bool CFrontController::SetGridSync(int iFrequencyParam, int iDelay)
{
    if (!m_fIsOpened)
    {
        return false;
    }
    if(m_iRunStatus != 1)
    {
        return false;
    }
    if(m_iPannelWorkMode != 1)
    {
        return false;
    }
    if (iFrequencyParam != 1 &&
            iFrequencyParam != 2 &&
            iFrequencyParam != 4 &&
            iFrequencyParam != 8)
    {
        return false;
    }
    unsigned char rgbCmdData[3] = {0};
    rgbCmdData[0] = 0x0;
    rgbCmdData[1] = iFrequencyParam;
    if (iDelay < 0)
    {
        rgbCmdData[2] = 0;
    }
    else if (iDelay > 150)
    {
        rgbCmdData[2] = 150;
    }
    else
    {
        rgbCmdData[2] = iDelay;
    }
    if (m_pSerialLink->Lock(1000) != 0)
    {
        return false;
    }
    if (m_pSerialLink->SendCmdData(0x06, rgbCmdData, 3) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    unsigned char rgbRecvData[32] = {0};
    unsigned int iRecvLen = 32;
    if (m_pSerialLink->RecvPacket(rgbRecvData, &iRecvLen, 1000) != S_OK);
    {
        m_pSerialLink->UnLock();
        return false;
    }
    if (rgbRecvData[2] != 0x06 || rgbRecvData[3] != 0)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    m_pSerialLink->UnLock();
    return true;
}

bool CFrontController::SetStrobeSignelSource(int iSource)
{
    if (!m_fIsOpened)
    {
        return false;
    }
    if(m_iRunStatus != 1)
    {
        return false;
    }
    if(m_iPannelWorkMode != 1)
    {
        return false;
    }
    if (iSource != 0 && iSource != 1)
    {
        return false;
    }
    unsigned char rgbCmdData[2] = {0};
    rgbCmdData[0] = 0x3;
    rgbCmdData[1] = iSource;
    if (m_pSerialLink->Lock(1000) != 0)
    {
        return false;
    }
    if (m_pSerialLink->SendCmdData(0x05, rgbCmdData, 2) != S_OK)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    unsigned char rgbRecvData[32] = {0};
    unsigned int iRecvLen = 32;
    if (m_pSerialLink->RecvPacket(rgbRecvData, &iRecvLen, 1000) != S_OK);
    {
        m_pSerialLink->UnLock();
        return false;
    }
    if (rgbRecvData[2] != 0x05 || rgbRecvData[3] != 0)
    {
        m_pSerialLink->UnLock();
        return false;
    }
    m_pSerialLink->UnLock();
    return true;
}

bool CFrontController::UpdatePannel(unsigned char* rgbUpdateFile)
{
    if (m_iUpdatingStatus != -1)
    {
        return TRUE;
    }
    if (m_rgbUpdateFileBuffer != NULL)
    {
        delete[] m_rgbUpdateFileBuffer;
        m_rgbUpdateFileBuffer = NULL;
    }
    m_rgbUpdateFileBuffer = new unsigned char[122880];
    if (m_rgbUpdateFileBuffer == NULL)
    {
        return FALSE;
    }
    memcpy(m_rgbUpdateFileBuffer, rgbUpdateFile, 122880);
    m_iUpdatingStatus = 1;
    return TRUE;
}

bool CFrontController::WriteUpdateFileToPannel(void)
{
    if (UpDataPannelProgram(m_rgbUpdateFileBuffer, 122880) != TRUE)
        return FALSE;
    m_iUpdatingStatus = -1;
    delete[] m_rgbUpdateFileBuffer;
    m_rgbUpdateFileBuffer = NULL;
    return FALSE;
}

bool CFrontController::GetCRCValue(int& iValue)
{
    if (!m_fIsOpened)
    {
        iValue = -1;
        return false;
    }
    if (m_pSerialLink->Lock(1000) != 0)
    {
        iValue = -1;
        return false;
    }
    if (m_pSerialLink->SendCmdData(0x0E, NULL, 0) != S_OK)
    {
        iValue = -1;
        m_pSerialLink->UnLock();
        return false;
    }
    unsigned char rgbRecvData[32] = {0};
    unsigned int iRecvLen = 32;
    if (m_pSerialLink->RecvPacket(rgbRecvData, &iRecvLen, 1000) != S_OK)
    {
        iValue = 0;
        m_pSerialLink->UnLock();
        return false;
    }
    if (rgbRecvData[2] != 0x0E || rgbRecvData[3] != 0)
    {
        iValue = -1;
        m_pSerialLink->UnLock();
        return false;
    }
    iValue = rgbRecvData[3];
    m_pSerialLink->UnLock();
    return TRUE;
}
