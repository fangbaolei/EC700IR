/**
* @file swpa_flash_file.h 
* @brief 声明操作FLASH的相关函数，此头文件不对外发布，仅为内部使用
*
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-11
* @version 1.0
*/


#ifndef _SWPA_FLASH_FILE_H_
#define _SWPA_FLASH_FILE_H_


#ifdef __cplusplus
extern "C"
{
#endif




/**
* @brief 打开FLASH文件,主要是初始化SWPA_FILEHEADER结构体
*
* 
* @param [in] filename 文件名
* @param [in] mode 文件打开模式
* - "r"  读方式打开
* - "w"  写方式打开
* - "r+"  读写方式打开
* - "w+"  读写方式打开
* @retval 文件描述符(int型) :  成功；实际上就是SWPA_FILEHEADER结构体指针
* @retval SWPAR_FAIL : 打开失败
* @retval SWPAR_INVALIDARG : 参数非法
* @retval SWPAR_OUTOFMEMORY : 参数内存不够
*/
extern int swpa_flash_file_open(
	const char *filename, 
	const char *mode
);



/**
* @brief 关闭flash设备文件
*
* 
* @param [in] fd 文件描述符
* @retval SWPAR_OK :  成功
* @retval SWPAR_FAIL :  失败
*/
extern int swpa_flash_file_close(
	int fd
);




/**
* @brief 读flash文件数据
*
* 
* @param [in] fd 文件描述符
* @param [in] buf 读数据缓冲区
* @param [in] size 缓冲区大小
* @param [out] ret_size 返回读到的数据大小
* @retval SWPAR_OK :  成功
* @retval SWPAR_FAIL :  失败
*/
extern int swpa_flash_file_read(
	int fd, 
	void* buf, 
	int size, 
	int* ret_size
);



/**
* @brief 写flash文件数据
*
* 
* @param [in] fd 文件描述符
* @param [in] buf 写数据缓冲区
* @param [in] size 缓冲区大小
* @param [out] ret_size 返回写入的数据大小
* @retval SWPAR_OK :  成功
* @retval SWPAR_FAIL :  失败
*/
extern int swpa_flash_file_write(
	int fd, 
	void* buf, 
	int size, 
	int* ret_size
);



/**
* @brief 改变文件指针
*
* 
* @param [in] fd 文件描述符
* @param [in] offset 指针的偏移量
* @param [in] pos 指针偏移量的起始位置，取值范围是: 
* - 文件头0(SWPA_SEEK_SET)
* - 当前位置1(SWPA_SEEK_CUR)
* - 文件尾2(SWPA_SEEK_END)
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
*/
extern int swpa_flash_file_seek(
	int fd, 
	int offset, 
	int pos
);



/**
* @brief 判断是否到了文件尾部
* @param [in] fd 文件描述符
* @retval 0 : 没有到文件尾部，
* @retval -1 : 到了文件尾部
* @attention 这个函数的返回值比较特殊，请区别对待
*/
int swpa_flash_file_eof(
	int fd
);






/**
* @brief 获得当前文件指针位置
*
* 
* @param [in] fd 文件描述符
* @retval 文件指针位置: 成功
* @retval SWPAR_INVALIDARG : 参数非法
* @retval SWPAR_FAIL : 失败
* @attention 这个函数的返回值比较特殊，请区别对待
*/
extern int swpa_flash_file_tell(
	int fd
);



/**
* @brief 对文件描述符的控制
*
*
* 目前仅支持获取文件最大size
* 
* @param [in] fd 文件描述符
* @param [in] cmd 命令标识，目前已定义的命令有
* - SWPA_FILE_IOCTL_FLASH_GET_MAX_SIZE : 获取文件最大size
* @param [in] args 命令标志参数
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
*/
int swpa_flash_file_ioctl(
	int fd, 
	int cmd, 
	void* args
);



#ifdef __cplusplus
}
#endif

#endif //_SWPA_FLASH_FILE_H_

