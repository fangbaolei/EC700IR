#include "HvExit.h"
#include "misc.h"
#include "HvPciLinkApi.h"
#include "HvDebugStateInfo.h"

extern "C" void HvExit(
    int iExitCode,
    const char* szExitInfo,
    const char* szFile,
    int nLine
)
{
    static pthread_mutex_t cHvExitMutex = PTHREAD_MUTEX_INITIALIZER;

    time_t iTime = 0;
    tm* pcTM = NULL;
    char pbDateTime[64] = {0};
    char pbBuf[1024] = {0};

    pthread_mutex_lock(&cHvExitMutex);
    //增加限制条件,确保程序只能调用一次Hv_Exit
    static bool fHasExit = false;
    if (fHasExit)
    {
        HV_Trace(5, "you have called HvExit already, will be egnore the call.\n");
        pthread_mutex_unlock(&cHvExitMutex);
        return;
    }
    fHasExit = true;

    iTime = time(NULL);
    pcTM = gmtime(&iTime);
    sprintf(pbDateTime, "%d/%02d/%02d %02d:%02d:%02d",
            pcTM->tm_year + 1900,
            pcTM->tm_mon + 1,
            pcTM->tm_mday,
            pcTM->tm_hour,
            pcTM->tm_min,
            pcTM->tm_sec);

    sprintf(pbBuf, "ver:[%s %s %s],datetime:[%s],file:[%s],line:[%d],info:[%s].",
            PSZ_DSP_BUILD_NO,
            DSP_BUILD_DATE,
            DSP_BUILD_TIME,
            pbDateTime,
            szFile,
            nLine,
            szExitInfo);

#ifndef SINGLE_BOARD_PLATFORM
    if ( 0 != PciSendHvExit(iExitCode) )
    {
        strcat(pbBuf, "PciSendHvExit is Error!");
        iExitCode = (HEC_FAIL|HEC_RESET_DEV);
    }
#endif

    WriteDataToFile("exit.txt", (unsigned char*)pbBuf, strlen(pbBuf));

    if ( HEC_FAIL == (iExitCode&0x7f) )
    {
        // 将失败退出信息写入黑盒子
        sprintf(pbBuf, "%s_%d_%s_%s", szFile, nLine, szExitInfo, PSZ_DSP_BUILD_NO);
        _HvDebugStateInfoWrite(szFile, nLine, pbBuf);
    }

    pthread_mutex_unlock(&cHvExitMutex);

    //退出程序之前先清除所有的线程
    CHvThreadBase::StopAllThread();

    // 卸载硬盘
    if (HV_System("dm 6 0 Result_disk"))
    {
        HV_Trace(5, "[dm 6 0 Result_disk] exec error.\n");
    }
    HV_Sleep(1000);

    exit(iExitCode);
}
