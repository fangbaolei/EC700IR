/**
* @file		misc.h
* @version	1.0
* @brief	杂项函数头文件，将所有难以分类，
* 以及暂时不易分类的函数及其它内容都放在这里，然后定期整理，再分类。
*/

#ifndef _MISC_H_
#define _MISC_H_

#include "hvutils.h"
#include "safesaver.h"

void LongMulti(
    WORD16 *pwData,
    int iWordCount,
    WORD16 wMulti
);

void MultiplicationDWORD64(
    DWORD32& dw32Low,
    DWORD32& dw32High,
    const DWORD32& dwIER
);

/**
* @brief 删除目录（可以为非空目录）
* @return 成功：0，失败：-1
*/
int DelDirContent(const char* szDir);

/**
* @brief 获取文件大小
* @return 成功：0，失败：-1
*/
int GetFileSize(
    const char* szFileName,
    unsigned long& ulFileSize
);

/**
* @brief 创建一个空文件
* @return 成功：0，失败：-1
*/
int CreateEmptyFile(const char* szFileName);

/**
* @brief 写入内存缓存区内容到指定文件
* @return 成功：0，失败：-1
*/
int WriteDataToFile(
    const char* szFileName,
    unsigned char* pbData,
    unsigned long ulDataSize
);

/**
* @brief 读取文件内容到指定内存缓存区
* @return 成功：0，失败：-1
*/
int ReadDataFromFile(
    const char* szFileName,
    unsigned char* pbData,
    unsigned long ulDataSize
);

int ShowSystemStatus(char* szStatus, int iSize);

/**
* @brief 确保指定路径中的各个目录都存在，否则自动创建。
* @param[in] DirPath  路径名
* @return 成功：true，失败：false
*/
bool MakeSureDirectoryPathExists(
    const char* DirPath
);

/**
* @brief 获取指定目录内的文件数
* @param[in] szDir  目录名
* @param[out] iCount  文件数。注：这里的文件数包括指定目录内的所有文件及目录，但不包括子目录。
* @return 成功：S_OK，失败：与ExecShellCmd函数返回值相同
*/
HRESULT GetDirFileCount(
    const char* szDir,
    int& iCount
);

/**
* @brief 获取目录占用的硬盘空间大小，单位：k
* @param[in] szDir 目录名
* @return 返回该目录占用硬盘空间的大小
*/

int GetDirSize(const char * szDir);

//TODO: 完善函数使用说明

bool ComputeHour(
    DWORD32 dwBTimeLow,
    DWORD32 dwBTimeHigh,
    DWORD32 dwETimeLow,
    DWORD32 dwETimeHigh
);

int ComputeTimeMs(
    DWORD32 dwBTimeLow,
    DWORD32 dwBTimeHigh,
    DWORD32 dwETimeLow,
    DWORD32 dwETimeHigh
);

void TimeMsAddOneSecond(
    DWORD32& dwTimeMsLow,
    DWORD32& dwTimeMsHigh
);

HRESULT GetPlateNumByXmlExtInfo(
    char* szXmlExtInfo,
    char* szPlateNum
);

void DebugPrintf(
    const char* szDebugInfo,
    DWORD dwDumpLen,
    const char* szID
);

void HV_Dump(
    unsigned char* buf,
    int len
);

void DebugShowTime(
    const char* lpszFlag,
    DWORD32 dwTimeMsLow,
    DWORD32 dwTimeMsHigh
);

void DebugSystemPrint(
    const char* szShellCmd,
    char* szBuf,
    int iBufLen
);

HRESULT GetSecondsSinceLinuxBoot(int* piSeconds);

#endif
