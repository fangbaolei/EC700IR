/**
* @file swpa_dsplink_file.h 
* @brief 声明操作DSPLINK的相关函数，此头文件不对外发布，仅为内部使用
*
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-11
* @version 1.0
*/


#ifndef _SWPA_DSPLINK_FILE_H_
#define _SWPA_DSPLINK_FILE_H_


#ifdef __cplusplus
extern "C"
{
#endif





/**
* @brief 打开dsplink文件,主要是初始化SWPA_FILEHEADER结构体
*
* 
* @param [in] filename 文件名
* @param [in] mode 文件打开模式
* - "w"  :写方式打开
* @retval 文件描述符(int型) :  成功；实际上就是SWPA_FILEHEADER结构体指针
* @retval SWPAR_FAIL : 打开失败
* @retval SWPAR_INVALIDARG : 参数非法
* @retval SWPAR_OUTOFMEMORY : 参数内存不够
*/
extern int swpa_dsplink_file_open(
	const char * filename, 
	const char * mode
);



/**
* @brief 关闭dsplink设备文件
*
* 
* @param [in] fd 文件描述符
* @retval SWPAR_OK :  成功
* @retval SWPAR_FAIL :  失败
*/
extern int swpa_dsplink_file_close(
	int fd
);





/**
* @brief 写dsplink文件数据
*
* 
* @param [in] fd 文件描述符
* @param [in] buf 写数据缓冲区
* @param [in] size 缓冲区大小
* @param [out] ret_size 返回写入的数据大小
* @retval SWPAR_OK :  成功
* @retval SWPAR_FAIL :  失败
*/
extern int swpa_dsplink_file_write(
	int fd, 
	void* buf, 
	int size, 
	int* ret_size
);




/**
* @brief 对文件描述符的控制
*
*
* 目前仅支持写的超时时间设置
* 
* @param [in] fd 文件描述符
* @param [in] cmd 命令标识，目前已定义的命令有
* - SWPA_FILE_IOCTL_DSPLINK_SET_TIMEOUT : 写的超时时间设置
* @param [in] args 命令标志参数
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
*/
extern int swpa_dsplink_file_ioctl(
	int fd, 
	int cmd, 
	void* args
);




#ifdef __cplusplus
}
#endif

#endif //_SWPA_DSPLINK_FILE_H_


