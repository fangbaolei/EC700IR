/**
* @file swpa_normal_file.h 
*
* @brief 声明了普通磁盘文件操作的基本函数，此头文件不对外发布，仅为内部使用
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-02-27
* @version 1.0
*/

#ifndef _SWPA_NORMAL_FILE_H_
#define _SWPA_NORMAL_FILE_H_


#ifdef __cplusplus
extern "C"
{
#endif



/**
* @brief 打开磁盘文件
* 
* @param [in] filename 文件名
* @param [in] mode 文件打开模式,有以下几个模式:
* - "r"或"br"  : 读方式打开
* - "r+"或"br+"  : 读写方式打开
* - "w"或"bw"  :   写方式打开
* - "w+"或"bw+"  : 读写方式打开，如果文件不存在则先创建文件
* - "a"或"ba"  :   追加方式打开
* - "a+"或"ba+"  : 追加方式打开，可读可写
* @retval 文件描述符: 成功；(实际上就是SWPA_FILEHEADER结构体指针)
* @retval SWPAR_FAIL : 打开失败
* @retval SWPAR_INVALIDARG : 参数非法
*/
extern int swpa_normal_file_open(
	const char * filename, 
	const char * mode
);




/**
* @brief 关闭磁盘文件
*
* 
* @param [in] fd 文件描述符
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
*/
extern int swpa_normal_file_close(
	int fd
);





/**
* @brief 判断是否到了文件尾部
* @param [in] fd 文件描述符
* @retval 0 : 没有到文件尾部，
* @retval -1 : 到了文件尾部
* @attention 这个函数的返回值比较特殊，请区别对待
*/
extern int swpa_normal_file_eof(
	int fd
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
extern int swpa_normal_file_seek(
	int fd, 
	int offset, 
	int pos
);





/**
* @brief 获得当前文件指针位置
*
* 
* @param [in] fd 文件描述符
* @retval 文件指针位置: 执行成功
* @retval SWPAR_FAIL : 失败
* @attention 这个函数的返回值比较特殊，请区别对待
*/
extern int swpa_normal_file_tell(
	int fd
);






/**
* @brief 对文件描述符的控制
*
* 
* @param [in] fd 文件描述符
* @param [in] cmd 命令标识
* @param [in] args 命令标志参数
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
* @retval SWPAR_NOTIMPL : 没有实现
*/
extern int swpa_normal_file_ioctl(
	int fd, 
	int cmd, 
	void* args
);






/**
* @brief 读磁盘文件数据
*
* 
* @param [in] fd 文件描述符
* @param [in] buf 读数据缓冲区，必须非空
* @param [in] size 缓冲区大小，必须大于0，单位为字节
* @param [out] ret_size 返回读到的数据大小，若不关心该数值，可传NULL
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
*/
extern int swpa_normal_file_read(
	int fd, 
	void * buf, 
	int size, 
	int * ret_size
);



/**
* @brief 写磁盘文件数据
*
* 
* @param [in] fd 文件描述符
* @param [in] buf  写数据缓冲区
* @param [in] size 缓冲区大小
* @param [out] ret_size 返回写入的数据大小
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
*/
extern int swpa_normal_file_write(
	int fd, 
	void* buf, 
	int size, 
	int* ret_size
);


#ifdef __cplusplus
}
#endif


#endif //_SWPA_NORMAL_FILE_H_

