#ifndef _DSPCMD_API_
#define _DSPCMD_API_

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief 初始化图片识别命令库
* @return 0:成功; 1:已经执行该操作; -1:失败
*/
int InitDspCmdLib_Photo(int nPlateRecogParamIndex, void* pvParam);

/**
* @brief 初始化视频流识别命令库
* @return 0:成功; 1:已经执行该操作; -1:失败
*/
int InitDspCmdLib_Video(int nPlateRecogParamIndex, void* pvParam);

/**
* @brief 处理DSP命令
* @param pszInbuf 输入数据
* @param nInlen 输入数据的长度
* @param pszOutbuf 输出数据
* @param nOutlen 输出数据的长度
* @return 无
*/
void ProcessDspCmd(unsigned char *pbInbuf, unsigned int nInlen, unsigned char *pbOutbuf, unsigned int nOutlen);

#ifdef __cplusplus
}
#endif

#endif

