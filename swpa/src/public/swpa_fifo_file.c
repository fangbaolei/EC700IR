/**
* @file swpa_fifo_file.c 
* @brief 实现FIFO相关的操作函数
*
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-11
* @version 1.0
*/

#include "swpa_file.h"
#include "swpa_file_private.h"
#include "swpa_fifo_file.h"

#include <stdio.h>
#include <errno.h>
#include <sys/io.h>
#include <unistd.h>
#include <sys/stat.h>  

#include  <sys/types.h> 
#include  <sys/time.h> 
#include  <sys/select.h> 
#include <fcntl.h>


typedef struct _FIFO_FILE_INFO
{
	char * 	filename;
	int  	pfile;//FILE * pfile;
	int	   	rd_timeout_ms;
	int	   	wr_timeout_ms;
	
}FIFO_FILE_INFO;




/**
* @brief 打开实名管道文件
*
* 
* @param [in] filename 文件名
* @param [in] mode 文件打开模式,有以下几个模式:
* - "r"  读方式打开
* - "w"  写方式打开
* - "r+"  读写方式打开
* - "w+"  读写方式打开
* @retval 文件描述符: 成功；(实际上就是SWPA_FILEHEADER结构体指针)
* @retval SWPAR_FAIL : 打开失败
* @retval SWPAR_INVALIDARG : 参数非法
*/
int swpa_fifo_file_open(
	const char * filename, 
	const char * mode
)
{
	int ret = SWPAR_OK;
	SWPA_FILEHEADER * pheader = NULL;
	FIFO_FILE_INFO * pinfo = NULL;
	//FILE * pfile = NULL;
	int modeval = 0;
	struct stat stat_info;
	
	//参数有效性检查
	SWPA_FILE_CHECK(NULL != filename);	//filename指针非空
	SWPA_FILE_CHECK(0 != swpa_strcmp(filename, ""));//filename非空字符串
	SWPA_FILE_CHECK(NULL != mode);		//mode指针非空
	SWPA_FILE_CHECK(0 != swpa_strcmp(mode, "")); //mode非空字符串

	//打印参数
	SWPA_FILE_PRINT("filename=%s\n", filename);
	SWPA_FILE_PRINT("mode=%s\n", mode);


	pheader = (SWPA_FILEHEADER *)swpa_mem_alloc(sizeof(*pheader));
	if (NULL == pheader)
	{
		ret = SWPAR_OUTOFMEMORY;
		SWPA_FILE_PRINT("Err: No enough memory for pheader! [%d]\n", ret);

		return ret;
	}
	swpa_memset(pheader, 0, sizeof(*pheader));
	
	ret = swpa_file_create_rwlock(pheader, filename);
	if (SWPAR_OK != ret)
	{
		SWPA_FILE_PRINT("Err: swpa_file_create_rwlock() failed! [%d]\n", ret);
		ret = SWPAR_FAIL;
		goto _ERR_HANDLING; 
	}
	

	pinfo = (FIFO_FILE_INFO *)swpa_mem_alloc(sizeof(*pinfo));
	if (NULL == pinfo)
	{
		ret = SWPAR_OUTOFMEMORY;
		SWPA_FILE_PRINT("Err: No enough memory for pinfo! [%d]\n", ret);

		goto _ERR_HANDLING;
	}
	swpa_memset(pinfo, 0, sizeof(*pinfo));
	

	pinfo->filename = (char *)swpa_mem_alloc(swpa_strlen(filename)+1);
	if (NULL == pinfo->filename)
	{
		ret = SWPAR_OUTOFMEMORY;
		SWPA_FILE_PRINT("Err: No enough memory for pinfo->filename! [%d]\n", ret);

		goto _ERR_HANDLING;
	}

	swpa_strcpy(pinfo->filename, filename);

	pinfo->rd_timeout_ms = -1; //默认阻塞方式读
	pinfo->wr_timeout_ms = -1; //默认阻塞方式写
	

	//设置文件读写模式标志
	ret = swpa_file_parse_mode(pheader, mode);
	if (SWPAR_OK != ret)
	{
		SWPA_FILE_PRINT("Err: swpa_file_parse_mode() failed! [%d]\n", ret);		
		goto _ERR_HANDLING; 
		
	}

	if (SWPA_FILE_IS_APPEND_MODE(pheader))
	{
		ret = SWPAR_INVALIDARG;
		SWPA_FILE_PRINT("Err: mode (%s) is invalid for fifo file! [%d]\n", mode, ret);		
		goto _ERR_HANDLING; 
	}


	//管道已存在
	if ( 0 == access(filename, F_OK))
	{
		if ( 0 != stat(filename,  &stat_info))
	 	{
			ret = SWPAR_FAIL;
			SWPA_FILE_PRINT("Err: stat(%s) failed! (%d: %s) [%d]\n", filename, errno, strerror(errno), ret);
			
			goto _ERR_HANDLING;
		}

		if (!(stat_info.st_mode & S_IFIFO))//(S_IFIFO != (stat_info.st_mode & S_IFIFO))
		{
			ret = SWPAR_FAIL;
			SWPA_FILE_PRINT("Err: %s exists and it is not a fifo file! [%d]\n", filename, ret);
			
			goto _ERR_HANDLING;
		}
	}
	//管道不存在，则先mkfifo
	else
	{
		if (0 != mkfifo(filename, S_IFIFO|0666))
		{
			ret = SWPAR_FAIL;
			SWPA_FILE_PRINT("Err: mkfifo(%s) failed! (%d: %s) [%d]\n", filename, errno, strerror(errno), ret);
			
			goto _ERR_HANDLING;
		}
	}

	
	//打开实名管道
	//modeval = O_NONBLOCK; //非阻塞式
	if (SWPA_FILE_IS_READ_MODE(pheader))
	{
		modeval |= (SWPA_FILE_IS_WRITE_MODE(pheader) ? O_RDWR : O_RDONLY);
	}
	else if (SWPA_FILE_IS_WRITE_MODE(pheader))
	{
		modeval |= O_WRONLY;
	}

	pinfo->pfile = open(filename, modeval); 
	if (-1 == pinfo->pfile)
	{
		ret = SWPAR_FAIL;
		SWPA_FILE_PRINT("Err: failed to open %s (%d: %s) [%d]\n", filename, errno, strerror(errno), ret);
		
		goto _ERR_HANDLING;
	}	
		
	pheader->device_param = (int)pinfo; 
	pheader->file_open	  = swpa_fifo_file_open;
	pheader->file_close   = swpa_fifo_file_close;
	pheader->file_seek	  = swpa_fifo_file_seek;
	pheader->file_eof	  = swpa_fifo_file_eof;
	pheader->file_tell	  = swpa_fifo_file_tell;
	pheader->file_ioctl   = swpa_fifo_file_ioctl;
	pheader->file_read	  = swpa_fifo_file_read;
	pheader->file_write   = swpa_fifo_file_write;

	return (int)pheader;

//错误处理并返回
_ERR_HANDLING:

	if (0 != pheader->rwlock)
	{
		swpa_mutex_delete(&pheader->rwlock);
		pheader->rwlock = 0;
	}
	
	if (NULL != pheader)
	{
		SWPA_FILE_CHECK_RET(swpa_mem_free(pheader), SWPAR_FAIL);
		pheader = NULL;
	}

	if (NULL != pinfo->filename)
	{
		SWPA_FILE_CHECK_RET(swpa_mem_free(pinfo->filename), SWPAR_FAIL);
		pinfo = NULL;
	}

	if (NULL != pinfo)
	{
		SWPA_FILE_CHECK_RET(swpa_mem_free(pinfo), SWPAR_FAIL);
		pinfo = NULL;
	}
	
	return ret;
	
}





/**
* @brief 关闭文件
*
* 
* @param [in] fd 文件描述符
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
*/
int swpa_fifo_file_close(
	int fd
)
{
	int ret = SWPAR_OK;
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	FIFO_FILE_INFO * pinfo = NULL;

	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);		//fd 必须不等于0

	//参数打印
	SWPA_FILE_PRINT("fd=%d\n", fd);

	pinfo = (FIFO_FILE_INFO *)pheader->device_param;


	//关闭文件
	SWPA_FILE_CHECK_RET(close(pinfo->pfile), SWPAR_FAIL);

	//如果是写入方关闭管道，则在关闭后删除该管道。合理吗?
	if (SWPA_FILE_IS_WRITE_MODE(pheader))
	{
	//	unlink(pinfo->filename);
	}	


	//释放资源
	pheader->device_param = (int)NULL;
	pheader->mode	 	  = 0;
	pheader->file_open	  = NULL;
	pheader->file_close   = NULL;
	pheader->file_seek	  = NULL;
	pheader->file_eof	  = NULL;
	pheader->file_tell	  = NULL;
	pheader->file_ioctl   = NULL;
	pheader->file_read	  = NULL;
	pheader->file_write   = NULL;

	if (0 != pheader->rwlock)
	{
		swpa_mutex_delete(&pheader->rwlock);
		pheader->rwlock = 0;
	}
	
	SWPA_FILE_CHECK_RET(swpa_mem_free(pheader), SWPAR_FAIL);
	pheader = NULL;

	SWPA_FILE_CHECK_RET(swpa_mem_free(pinfo->filename), SWPAR_FAIL);
	pinfo->filename = NULL;
	
	SWPA_FILE_CHECK_RET(swpa_mem_free(pinfo), SWPAR_FAIL);
	pinfo = NULL;

	return ret;
}





/**
* @brief 判断是否到了文件尾部
* @param [in] fd 文件描述符
* @retval 0 : 没有到文件尾部，
* @retval -1 : 到了文件尾部
* @attention 这个函数的返回值比较特殊，请区别对待
*/
int swpa_fifo_file_eof(
	int fd
)
{
#if 0
	int ret = SWPAR_OK;
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	FIFO_FILE_INFO * pinfo = NULL;

	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);		//fd 必须不等于0

	//参数打印
	SWPA_PRINT_INT(fd);
	
	pinfo = (FIFO_FILE_INFO *)pheader->device_param;

	//判断
	// 0 : 表明未到文件末尾
	//-1: 表明到文件末尾，而不是出错!
	return (feof(pinfo->pfile)) ? (-1) : (0);
#else
	return SWPAR_NOTIMPL;
#endif
}


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
* @retval SWPAR_NOTIMPL : 未实现
*/
int swpa_fifo_file_seek(
	int fd, 
	int offset, 
	int pos
)
{
	return SWPAR_NOTIMPL;
}





/**
* @brief 获得当前文件指针位置
*
* 
* @param [in] fd 文件描述符
* @retval SWPAR_NOTIMPL : 未实现
*/
int swpa_fifo_file_tell(
	int fd
)
{
	return SWPAR_NOTIMPL;
}






/**
* @brief 对文件描述符的控制
*
* 
* @param [in] fd 文件描述符
* @param [in] cmd 命令标识
* @param [in] args 命令标志参数
* @retval SWPAR_NOTIMPL : 未实现
*/
int swpa_fifo_file_ioctl(
	int fd, 
	int cmd, 
	void* args
)
{
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	FIFO_FILE_INFO * pinfo = (FIFO_FILE_INFO *)pheader->device_param;	

	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);    	//fd 必须不等于0
	//SWPA_FILE_CHECK(0 < cmd);    	//cmd取值没有限定，故mark掉
	//SWPA_FILE_CHECK(NULL != args);    	//args 在不同cmd下可以为NULL，故mark掉

	//参数打印
	SWPA_FILE_PRINT("fd=%d\n", fd);	
	SWPA_FILE_PRINT("cmd=%d\n", cmd);	
	SWPA_FILE_PRINT("args=0x%08x\n", (unsigned int)args);
	
	switch (cmd)
	{		
		case SWPA_FILE_SET_READ_TIMEOUT:
		{	
			int timeout = *(int*)args;

			//SWPA_FILE_CHECK(0 < timeout);    	//超时时长须大于零
			
			pinfo->rd_timeout_ms = timeout;
		}
			
		break;

		case SWPA_FILE_SET_WRITE_TIMEOUT:
		{	
			int timeout = *(int*)args;

			//SWPA_FILE_CHECK(0 < timeout);    	//超时时长须大于零
			
			pinfo->wr_timeout_ms = timeout;
		}
			
		break;

		default:
			SWPA_FILE_PRINT("Err: invalid cmd for fifo file! [%d]\n", SWPAR_INVALIDARG);
			return SWPAR_INVALIDARG;
	}

	
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
*/
int swpa_fifo_file_read(
	int fd, 
	void * buf, 
	int size, 
	int * ret_size
)
{
	int ret = SWPAR_OK;
	int bytes = 0;
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	FIFO_FILE_INFO * pinfo = NULL;
	fd_set  rdset;
	struct timeval timeout, *ptimeout = NULL;
	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);		//fd 必须不等于0
	SWPA_FILE_CHECK(NULL != buf);		//buf非空
	SWPA_FILE_CHECK(0 < size);		//size 必须大于0
	//SWPA_FILE_CHECK(NULL != ret_size);		//Note: ret_size可以是NULL，故注释掉
	
	//参数打印
	SWPA_FILE_PRINT("fd=%d\n", fd);
	SWPA_FILE_PRINT("buf=0x%08x\n", (unsigned int)buf);
	SWPA_FILE_PRINT("size=%d", size);
	SWPA_FILE_PRINT("ret_size=0x%08x\n", (unsigned int)ret_size);

	pinfo = (FIFO_FILE_INFO *)pheader->device_param;

	FD_ZERO(&rdset);
	FD_SET(pinfo->pfile, &rdset);
	if (0 <= pinfo->rd_timeout_ms)
	{
		timeout.tv_sec = pinfo->rd_timeout_ms / 1000; // ms to sec
		timeout.tv_usec = (pinfo->rd_timeout_ms % 1000) * 1000; // ms to usec

		ptimeout = &timeout;
	}
	switch (select(pinfo->pfile + 1, &rdset, NULL, NULL, ptimeout))
	{
		case 0: //timeout
		{
			ret = SWPAR_OUTOFTIME;
			SWPA_FILE_PRINT("Info: select timeout, nothing to read!\n");
		}
		break;
		
		case -1: //error occured
		{
			ret = SWPAR_FAIL;
			SWPA_FILE_PRINT("Err: select failed (%d:%s) [%d]!\n", errno, strerror(errno), ret);
		}
		break;
		
		default: //succeeded
		{			
			if (FD_ISSET(pinfo->pfile, &rdset))
			{
				//do read
				bytes = read(pinfo->pfile, buf, size);
				if (-1 == bytes)
				{
					ret = SWPAR_FAIL;
					SWPA_FILE_PRINT("Err: failed to read fifo %s (%d:%s) [%d]!\n", pinfo->filename, errno, strerror(errno), ret);
				}
				else if (0 == bytes)
				{
					ret = SWPAR_FAIL;
					SWPA_FILE_PRINT("Err: nothing to read %s [%d]!\n", pinfo->filename,  ret);
				}
				else if (bytes != size)
				{
					//if (0 == feof(pinfo->pfile))
					{
						ret = SWPAR_OK;//SWPAR_FAIL;
						SWPA_FILE_PRINT("Info: only read %d (!= %d)![%d]\n", bytes, size, ret);
					}
					//else
					{
						//到达文件末尾，读取到的数据量小于给定的大小是可能的
					}
				}

				if (NULL != ret_size)
				{
					*ret_size = bytes;
				}
			}
			else
			{		
				ret = SWPAR_FAIL;
				SWPA_FILE_PRINT("Err: select confused! [%d]\n",  ret);
			}
		}
		break;
	}

	
	return ret;
}




/**
* @brief 写文件数据
*
* 
* @param [in] fd 文件描述符
* @param [in] buf  写数据缓冲区
* @param [in] size 缓冲区大小
* @param [out] ret_size 返回写入的数据大小
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
*/
int swpa_fifo_file_write(
	int fd, 
	void* buf, 
	int size, 
	int* ret_size
)
{
	int ret = SWPAR_OK;
	int bytes = 0;
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	FIFO_FILE_INFO * pinfo = NULL;
	fd_set  wrset;
	struct timeval timeout, *ptimeout = NULL;
	
	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);		//fd 必须不等于0
	SWPA_FILE_CHECK(NULL != buf);		//buf非空
	SWPA_FILE_CHECK(0 < size);		//size 必须大于0
	//SWPA_FILE_CHECK(NULL != ret_size);		//Note: ret_size可以是NULL，故注释掉
	
	//参数打印
	SWPA_FILE_PRINT("fd=%d\n", fd);
	SWPA_FILE_PRINT("buf=0x%08x\n", (unsigned int)buf);
	SWPA_FILE_PRINT("size=%d\n", size);
	SWPA_FILE_PRINT("ret_size=0x%08x\n", (unsigned int)ret_size);

	pinfo = (FIFO_FILE_INFO *)pheader->device_param;
	

	FD_ZERO(&wrset);
	FD_SET(pinfo->pfile, &wrset);
	if (0 <= pinfo->rd_timeout_ms)
	{
		timeout.tv_sec = pinfo->wr_timeout_ms / 1000; // ms to sec
		timeout.tv_usec = (pinfo->wr_timeout_ms % 1000) * 1000; // ms to usec

		ptimeout = &timeout;
	}
	switch (select(pinfo->pfile + 1, NULL, &wrset, NULL, ptimeout))
	{
		case 0: //timeout
		{
			ret = SWPAR_OUTOFTIME;
			SWPA_FILE_PRINT("Info: select timeout, nothing to write!\n");
		}
		break;
		
		case -1: //error occured
		{
			ret = SWPAR_FAIL;
			SWPA_FILE_PRINT("Err: select failed (%d:%s) [%d]!\n", errno, strerror(errno), ret);
		}
		break;
		
		default: //succeeded
		{			
			if (FD_ISSET(pinfo->pfile, &wrset))
			{
				//do read
				bytes = write(pinfo->pfile, buf, size);
				if (-1 == bytes)
				{
					ret = SWPAR_FAIL;
					SWPA_FILE_PRINT("Err: failed to write fifo %s (%d:%s) [%d]!\n", pinfo->filename, errno, strerror(errno), ret);
				}
				else if (bytes != size)
				{
					ret = SWPAR_OK;;
					SWPA_FILE_PRINT("Info: only wrote %d (!= %d)![%d]\n", bytes, size, ret);
				}

				if (NULL != ret_size)
				{
					*ret_size = bytes;
				}
			}
			else
			{		
				ret = SWPAR_FAIL;
				SWPA_FILE_PRINT("Err: select confused! [%d]\n",  ret);
			}
		}
		break;
	}


	return ret;
}


