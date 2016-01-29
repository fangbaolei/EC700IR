/**
* @file swpa_file.c
* @brief 文件操作实现文件
* 
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-02-27
* @version 1.0
*/



#include "swpa_file_private.h"
#include "swpa_eeprom_file.h"
#include "swpa_flash_file.h"
#include "swpa_normal_file.h"
#include "swpa_dsplink_file.h"
#include "swpa_comm_file.h"
#include "swpa_fifo_file.h"




/**
* @brief 打开文件基函数
*
* 根据文件名规则调用不同设备的打开文件的函数
* 
* @param [in] filename 文件名。 目前支持 普通文件、有名管道文件，串口文件，EEPROM、FLASH、DSPLINK，规定文件名如下：
* - 文件名为“EEPROM/设备id/设备段名”的是访问EEPROM设备
* - 文件名为“FLASH/设备id/设备段名”的是访问FLASH设备
* - 文件名为“DSPLINK/设备id”为访问DSPLINK设备
* - 文件名为“COMM/设备id”为访问串口设备
* - 文件名为“FIFO/管道名”为访问FIFO文件
* - 文件名为“BLOCK/文件名”为访问普通磁盘文件
* - 其他文件名的为普通的磁盘文件
* @param [in] mode 文件打开模式,有以下几个模式:
* - "r"  : 只读方式打开
* - "r+" : 读写方式打开
* - "w" :  只写方式打开，文件不存在则先创建
* - "w+" :  读写方式打开，文件不存在则先创建
* - "a" :  追加方式打开
* - "a+" : 追加方式打开,可读
* @retval 文件描述符: 成功(实际上就是SWPA_FILEHEADER结构体指针)
* @retval SWPAR_FAIL : 打开失败
* @retval SWPAR_INVALIDARG : 参数非法
*/
int swpa_file_open(
	const char * filename, 
	const char * mode
)
{
	//参数有效性检查
	SWPA_FILE_CHECK(NULL != filename);    	//filename指针非空
	SWPA_FILE_CHECK(0 != swpa_strcmp(filename, ""));//filename非空字符串
	//SWPA_FILE_CHECK(NULL != mode);			//mode指针非空
	//SWPA_FILE_CHECK(0 != swpa_strcmp(mode, ""));	//mode非空字符串

	//打印参数
	SWPA_FILE_PRINT("filename=%s\n", filename);
	SWPA_FILE_PRINT("mode=%s\n", mode);


	//按照filename打开不同的文件	
	if (filename == swpa_strstr(filename, "EEPROM/"))
	{
		return swpa_eeprom_file_open(filename + swpa_strlen("EEPROM/"), mode);
	}
	else if (filename == swpa_strstr(filename, "FLASH/"))
	{
		return swpa_flash_file_open(filename + swpa_strlen("FLASH/"), mode);
	}
	else if (filename == swpa_strstr(filename, "FIFO/"))
	{
		return swpa_fifo_file_open(filename + swpa_strlen("FIFO/"), mode);
	}
	else if (filename == swpa_strstr(filename, "COMM/"))
	{
		return swpa_comm_file_open(filename + swpa_strlen("COMM/"), mode);
	}
	else if (filename == swpa_strstr(filename, "DSPLINK/"))
	{
		return swpa_dsplink_file_open(filename + swpa_strlen("DSPLINK/"), mode);
	}
	else if (filename == swpa_strstr(filename, "BLOCK/"))	//磁盘块文件，可以省略不写
	{
		return swpa_normal_file_open(filename + swpa_strlen("BLOCK/"), mode);
	}
	else	//默认都当磁盘块文件打开
	{
		return swpa_normal_file_open(filename, mode);
	}

	return SWPAR_FAIL;
}



/**
* @brief 关闭文件
*
* 
* @param [in] fd 文件描述符
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
* @retval SWPAR_NOTIMPL : 未实现
*/
int swpa_file_close(
	int fd
)
{
	SWPA_FILEHEADER * pfile = (SWPA_FILEHEADER*)fd;

	SWPA_FILE_CHECK(0 != fd);    	// 0 != fd
	
	SWPA_FILE_PRINT("fd=%d\n", fd);
	
	if (0 != pfile->rwlock)
	{
		swpa_mutex_delete(&pfile->rwlock);
		pfile->rwlock = 0;
	}

	return (NULL != pfile->file_close) ? (pfile->file_close(fd)) : SWPAR_NOTIMPL;
}



/**
* @brief 判断是否到了文件尾部
* @param [in] fd 文件描述符
* @retval 0 : 没有到文件尾部，
* @retval -1 : 到了文件尾部
*/
int swpa_file_eof(
	int fd
)
{
	int ret = 0;

	SWPA_FILEHEADER * pfile = (SWPA_FILEHEADER*)fd;

	SWPA_FILE_CHECK(0 != fd);		// 0 != fd
	
	SWPA_FILE_PRINT("fd=%d\n", fd);

	
	swpa_mutex_lock(&pfile->rwlock, -1);
	
	ret = (NULL != pfile->file_eof) ? (pfile->file_eof(fd)) : SWPAR_NOTIMPL;	
	
	swpa_mutex_unlock(&pfile->rwlock);

	return ret;
}





/**
* @brief 改变文件指针
*
* 
* @param [in] fd 文件描述符
* @param [in] offset 指针的偏移量
* @param [in] pos 指针偏移量的起始位置
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
* @retval SWPAR_NOTIMPL : 未实现
*/
int swpa_file_seek(
	int fd, 
	int offset, 
	int pos
)
{
	int ret = 0;

	SWPA_FILEHEADER * pfile = (SWPA_FILEHEADER*)fd;

	SWPA_FILE_CHECK(0 != fd);		// 0 != fd
	
	SWPA_FILE_PRINT("fd=%d\n", fd);
	
	swpa_mutex_lock(&pfile->rwlock, -1);

	ret = (NULL != pfile->file_seek) ? (pfile->file_seek(fd, offset, pos)) : SWPAR_NOTIMPL;

	swpa_mutex_unlock(&pfile->rwlock);

	return ret;
}




/**
* @brief 获得当前文件指针位置
*
* 
* @param [in] fd 文件描述符
* @retval 成功返回当前文件指针位置，
* @retval SWPAR_FAIL : 失败
* @retval SWPAR_NOTIMPL : 未实现
* @attention 这个函数的返回值比较特殊，请区别对待
*/
int swpa_file_tell(
	int fd
)
{
	int ret = 0;
		
	SWPA_FILEHEADER * pfile = (SWPA_FILEHEADER*)fd;

	SWPA_FILE_CHECK(0 != fd);		// 0 != fd

	SWPA_FILE_PRINT("fd=%d\n", fd);
	
	swpa_mutex_lock(&pfile->rwlock, -1);

	ret = (NULL != pfile->file_tell) ? (pfile->file_tell(fd)) : SWPAR_NOTIMPL;

	swpa_mutex_unlock(&pfile->rwlock);

	return ret;
}




/**
* @brief 对文件描述符的控制，比如串口波特率、读写的超时时间设置等等
*
* 
* @param [in] fd 文件描述符
* @param [in] cmd 命令标识
* @param [in] args 命令标志参数
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
* @retval SWPAR_NOTIMPL : 未实现
*/
int swpa_file_ioctl(
	int fd, 
	int cmd, 
	void* args
)
{
	int ret = 0;

	SWPA_FILEHEADER * pfile = (SWPA_FILEHEADER*)fd;

	//参数检查
	SWPA_FILE_CHECK(0 != fd);		// 0 != fd
	//SWPA_FILE_CHECK(0 != cmd);			//Note: cmd可以是任意值，故注释掉
	//SWPA_FILE_CHECK(NULL != args);		//Note: args可以是NULL，故注释掉

	//打印参数
	SWPA_FILE_PRINT("fd=%d\n", fd);
	SWPA_FILE_PRINT("cmd=%d\n", cmd);
	SWPA_FILE_PRINT("args=0x%08x\n", (unsigned int)args);

	
	swpa_mutex_lock(&pfile->rwlock, -1);

	ret = (NULL != pfile->file_ioctl) ? (pfile->file_ioctl(fd, cmd, args)) : SWPAR_NOTIMPL;
	
	swpa_mutex_unlock(&pfile->rwlock);

	return ret;
		
}





/**
* @brief 读文件数据
*
* 
* @param [in] fd 文件描述符
* @param [in] buf 读数据缓冲区，必须非空
* @param [in] size 缓冲区大小，必须大于0，单位为字节
* @param [out] ret_size 返回读到的数据大小，若不关心该数值，可传NULL
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
* @retval SWPAR_NOTIMPL : 未实现
*/
int swpa_file_read(
	int fd, 
	void* buf, 
	int size, 
	int* ret_size
)
{
	int ret = 0;

	SWPA_FILEHEADER * pfile = (SWPA_FILEHEADER*)fd;

	//参数检查
	SWPA_FILE_CHECK(0 != fd);		// 0 != fd
	SWPA_FILE_CHECK(NULL != buf);		// NULL != buf
	SWPA_FILE_CHECK(0 < size);		
	//SWPA_FILE_CHECK(NULL != ret_size);	//Note: ret_size可以是NULL，故注释掉

	//打印参数
	SWPA_FILE_PRINT("fd=%d\n", fd);
	SWPA_FILE_PRINT("buf=0x%08x\n", (unsigned int)buf);
	SWPA_FILE_PRINT("size=%d\n", size);
	SWPA_FILE_PRINT("ret_size=0x%08x\n", (unsigned int)ret_size);

	
	swpa_mutex_lock(&pfile->rwlock, -1);

	ret = (NULL != pfile->file_read) ? (pfile->file_read(fd, buf, size, ret_size)) : SWPAR_NOTIMPL;

	swpa_mutex_unlock(&pfile->rwlock);

	return ret;
}




/**
* @brief 写文件数据
*
* 
* @param [in] fd 文件描述符
* @param [in] buf 写数据缓冲区
* @param [in] size 缓冲区大小
* @param [out] ret_size 返回写入的数据大小
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
* @retval SWPAR_NOTIMPL : 未实现
*/
int swpa_file_write(
	int fd, 
	void* buf, 
	int size, 
	int* ret_size
)
{
	int ret = 0;

	SWPA_FILEHEADER * pfile = (SWPA_FILEHEADER*)fd;

	//参数检查
	SWPA_FILE_CHECK(0 != fd);		// 0 != fd
	SWPA_FILE_CHECK(NULL != buf);		
	SWPA_FILE_CHECK(0 < size);		
	//SWPA_FILE_CHECK(NULL != ret_size);	//Note: ret_size可以是NULL，故注释掉

	//打印参数
	SWPA_FILE_PRINT("fd=%d\n", fd);
	SWPA_FILE_PRINT("buf=0x%08x\n", (unsigned int)buf);
	SWPA_FILE_PRINT("size=%d\n", size);
	SWPA_FILE_PRINT("ret_size=0x%08x\n", (unsigned int)ret_size);

	
	swpa_mutex_lock(&pfile->rwlock, -1);

	ret = (NULL != pfile->file_write) ? (pfile->file_write(fd, buf, size, ret_size)) : SWPAR_NOTIMPL;
	
	swpa_mutex_unlock(&pfile->rwlock);

	return ret;
}



