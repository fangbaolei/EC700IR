/**
* @file swpa_file_private.h 
* @brief 文件模块内部通用变量定义
* 
* 定义了文件模块内部实现需要共享的通用信息
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-02-27
* @version 1.0
*/

#ifndef _SWPA_FILE_PRIVATE_H_
#define _SWPA_FILE_PRIVATE_H_

#include "swpa_private.h"
#include "swpa_file.h"
#include "swpa_string.h"
#include "swpa_sem.h"

#ifdef __cplusplus
extern "C"
{
#endif


//#define __SWPA_FILE_VERIFY


/**
* @name SWPA_FILEHEADER
* @brief 文件操作结构体，包含了设备描述符、打开、关闭、读写、控制的函数指针
* 
*/
typedef struct _SWPA_FILEHEADER
{
	int device_param; /**< 设备描述符 */
	int mode; /**< 读写模式。第0位表明读，第1位表明写 ，第3位表明追加*/
	int rwlock; /**< 文件读写锁 */
	int (*file_open)(const char* filename, const char *mode);/**< 打开文件函数指针 */
	int (*file_close)(int fd);/**< 关闭文件函数指针 */
	int (*file_seek)(int fd, int offset, int pos);/**< 文件描述符读写位置函数指针 */
	int (*file_eof)(int fd);/**< 判断文件指针是否到了尾部的函数指针 */
	int (*file_tell)(int fd);/**< 文件指针所在位置函数指针 */
	int (*file_ioctl)(int fd, int cmd, void* args);/**< 文件控制函数指针 */
	int (*file_read)(int fd, void* buf, int size, int* ret_size);/**< 读文件函数指针 */
	int (*file_write)(int fd, void* buf, int size, int* ret_size);	/**< 读文件函数指针 */
}SWPA_FILEHEADER;



#define SWPA_FILE_SET_MODE(pheader, md) (pheader)->mode |= (md)

#define SWPA_FILE_RESET_MODE(pheader, md) (pheader)->mode &= ~(md)



#define SWPA_FILE_READ_MODE 		(0x01<<0)
#define SWPA_FILE_WRITE_MODE 		(0x01<<1)
#define SWPA_FILE_APPEND_MODE 		(0x01<<2)
#define SWPA_FILE_CREATE_MODE 		(0x01<<3)


#define SWPA_FILE_IS_READ_MODE(pheader) 	(pheader->mode & SWPA_FILE_READ_MODE)

#define SWPA_FILE_IS_WRITE_MODE(pheader) 	(pheader->mode & SWPA_FILE_WRITE_MODE)

#define SWPA_FILE_IS_APPEND_MODE(pheader) 	(pheader->mode & SWPA_FILE_APPEND_MODE)

#define SWPA_FILE_IS_CREATE_MODE(pheader) 	(pheader->mode & SWPA_FILE_CREATE_MODE)



#ifdef SWPA_FILE
#define SWPA_FILE_PRINT(fmt, ...) SWPA_PRINT("[%s:%d]"fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define SWPA_FILE_CHECK(arg)      {if (!(arg)){SWPA_PRINT("[%s:%d]Check failed : %s [%d]\n", __FILE__, __LINE__, #arg, SWPAR_INVALIDARG);return SWPAR_INVALIDARG;}}
#else
#define SWPA_FILE_PRINT(fmt, ...)
#define SWPA_FILE_CHECK(arg)
#endif


#define SWPA_FILE_CHECK_RET(ret, ret_on_err) 							\
if ( SWPAR_OK != (ret) ) 												\
{ 																		\
	SWPA_FILE_PRINT("Err : %s = %d [%d]\n", #ret, ret, ret_on_err); 		\
	return ret_on_err;													\
}	\



/**
* @brief 创建文件读写锁
*
* 
* @param [inout] pheader 文件头结构体指针
* @param [in] filename  文件名
* @retval SWPAR_OK : 成功
* @retval SWPAR_INVALIDARG : 参数非法
* @retval SWPAR_FAIL : 失败
*/
int swpa_file_create_rwlock(
	SWPA_FILEHEADER * pheader, 
	const char * filename
);



/**
* @brief 分析并设置文件操作模式
*
* 
* @param [inout] pheader 文件头结构体指针
* @param [in] mode 操作模式字符串
* @retval SWPAR_OK : 成功
* @retval SWPAR_INVALIDARG : 参数非法
* @retval SWPAR_FAIL : 失败
*/
extern int swpa_file_parse_mode(
	SWPA_FILEHEADER * pheader, 
	const char * mode
);


#ifdef __cplusplus
}
#endif


#endif //_SWPA_FILE_PRIVATE_H_


