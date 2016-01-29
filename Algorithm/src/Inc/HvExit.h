// 该文件编码必须是WINDOWS-936格式
#ifndef _HVEXIT_H_
#define _HVEXIT_H_

// HV_EXIT_CODE规范：
// 长度为1个字节；
// 最高位置0表示重载程序，最高位置1表示复位设备；
// 低7位用来表示退出原因；

#define HEC_RESET_DEV 0x80 // 掩码标记：复位设备

#define HEC_SUCC 0x00 // 正常退出
#define HEC_FAIL 0x01 // 异常退出

#ifdef __cplusplus
extern "C" {
#endif

void HvExit(
    int iExitCode,
    const char* szExitInfo,
    const char* szFile,
    int nLine
);

#ifdef __cplusplus
}
#endif

#define HV_Exit(iExitCode, szExitInfo) HvExit(iExitCode, szExitInfo, __FILE__, __LINE__)

#endif

