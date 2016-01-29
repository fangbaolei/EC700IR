/**
* @file swpa_normal_file.c
* @brief 实现磁盘文件相关的操作函数
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-02-27
* @version 1.0
*/

#include "swpa_file.h"
#include "swpa_file_private.h"
#include "swpa_normal_file.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


typedef struct tagNORMAL_FILE_INFO
{
	int offset;	//文件偏移量
	int fd;     //文件句柄
}NORMAL_FILE_INFO;

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
int swpa_normal_file_open(
	const char*  filename, 
	const char * mode
)
{
	int ret = SWPAR_OK;
	SWPA_FILEHEADER * pheader = NULL;
	NORMAL_FILE_INFO* fInfo = NULL;
		
	//参数有效性检查
	SWPA_FILE_CHECK(NULL != filename); 	//filename指针非空
	SWPA_FILE_CHECK(0 != swpa_strcmp(filename, ""));//filename非空字符串
	SWPA_FILE_CHECK(NULL != mode); 		//mode指针非空
	SWPA_FILE_CHECK(0 != swpa_strcmp(mode, ""));	//mode非空字符串

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
	
	fInfo = (NORMAL_FILE_INFO *)swpa_mem_alloc(sizeof(NORMAL_FILE_INFO));
	if (NULL == fInfo)
	{
		ret = SWPAR_OUTOFMEMORY;
		SWPA_FILE_PRINT("Err: No enough memory for fInfo! [%d]\n", ret);

		goto _ERR_HANDLING;
	}
	fInfo->offset = 0;
	
	//设置文件读写模式标志
	ret = swpa_file_parse_mode(pheader, mode);
	if (SWPAR_OK != ret)
	{
		SWPA_FILE_PRINT("Err: swpa_file_parse_mode() failed! [%d]\n", ret);
		ret = SWPAR_FAIL;
		goto _ERR_HANDLING; 
	}	

	//打开文件
	int flag = 0;
	if(SWPA_FILE_IS_READ_MODE(pheader) && SWPA_FILE_IS_WRITE_MODE(pheader))
	{
		flag = O_RDWR;
	}
	else if(SWPA_FILE_IS_READ_MODE(pheader) && SWPA_FILE_IS_APPEND_MODE(pheader))
	{
		flag = O_RDWR | O_APPEND;
	}
	else if(SWPA_FILE_IS_READ_MODE(pheader))
	{
		flag = O_RDONLY;
	}
	else if(SWPA_FILE_IS_WRITE_MODE(pheader))
	{
		flag = O_WRONLY|O_CREAT;
	}
	else if(SWPA_FILE_IS_APPEND_MODE(pheader))
	{
		flag = O_APPEND;
	}

	if (SWPA_FILE_IS_CREATE_MODE(pheader))
	{
		flag |= O_CREAT;
	}
	
	fInfo->fd = open(filename, flag, 0777);
	if(fInfo->fd < 0)
	{
		perror("Normal file open failed");
		ret = SWPAR_FAIL;
		SWPA_FILE_PRINT("Err: failed to open %s ! [%d]\n", filename, ret);
		
		goto _ERR_HANDLING;
	}	
	

	pheader->device_param = (int)fInfo;	
	
	pheader->file_open	  = swpa_normal_file_open;
	pheader->file_close   = swpa_normal_file_close;
	pheader->file_seek	  = swpa_normal_file_seek;
	pheader->file_eof	  = swpa_normal_file_eof;
	pheader->file_tell	  = swpa_normal_file_tell;
	pheader->file_ioctl   = swpa_normal_file_ioctl;
	pheader->file_read	  = swpa_normal_file_read;
	pheader->file_write   = swpa_normal_file_write;

	if(SWPA_FILE_IS_APPEND_MODE(pheader))
	{
		swpa_normal_file_seek((int)pheader, 0, SWPA_SEEK_END);
		SWPA_FILE_PRINT("Info: L%d: offset = %d\n", __LINE__, fInfo->offset);
	}
		
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
		swpa_mem_free(pheader);
		pheader = NULL;
	}
	
	if (NULL != fInfo)
	{
		swpa_mem_free(fInfo);
		fInfo = NULL;
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
int swpa_normal_file_close(
	int fd
)
{
	int ret = SWPAR_OK;
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);		//fd 必须不等于0

	//参数打印
	SWPA_FILE_PRINT("fd=%d\n", fd);

	//关闭文件
	NORMAL_FILE_INFO* fInfo = (NORMAL_FILE_INFO *)pheader->device_param;
	SWPA_FILE_CHECK(0 != fInfo);
	fsync(fInfo->fd);
	close(fInfo->fd);


	//释放资源
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
	SWPA_FILE_CHECK_RET(swpa_mem_free(fInfo), SWPAR_FAIL);

	return ret;
}






/**
* @brief 判断是否到了文件尾部
* @param [in] fd 文件描述符
* @retval 0  :  没有到了文件尾部
* @retval -1 :到文件尾部，
* @attention 这个函数的返回值比较特殊，请区别对待
*/
int swpa_normal_file_eof(
	int fd
)
{
	int ret = SWPAR_OK;
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;

	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);		//fd 必须不等于0

	//参数打印
	SWPA_FILE_PRINT("fd=%d\n", fd);
	
	NORMAL_FILE_INFO* fInfo = (NORMAL_FILE_INFO *)pheader->device_param;
	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fInfo);
	
	struct stat buf;
	if(fstat(fInfo->fd, &buf))
	{
		return SWPAR_FAIL;
	}
	
	return buf.st_size > fInfo->offset + 1 ? 0 /*means FALSE*/: -1 /*means TRUE*/;
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
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
*/
int swpa_normal_file_seek(
	int fd, 
	int offset, 
	int pos
)
{
	int ret = SWPAR_OK;
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);		//fd 必须不等于0
	SWPA_FILE_CHECK(SWPA_SEEK_SET == pos || SWPA_SEEK_CUR == pos || SWPA_SEEK_END == pos);		//pos所有的可能值

	//参数打印
	SWPA_FILE_PRINT("fd=%d\n", fd);
	SWPA_FILE_PRINT("offset=%d\n", offset);
	SWPA_FILE_PRINT("pos=%d\n", pos);
	
	NORMAL_FILE_INFO* fInfo = (NORMAL_FILE_INFO *)pheader->device_param;
	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fInfo);
	
	struct stat buf;
	if(fstat(fInfo->fd, &buf))
	{
		return SWPAR_FAIL;
	}
	
	switch(pos)
	{
	case SWPA_SEEK_SET : 
		{
			if(buf.st_size < offset)
			{
				ret = SWPAR_FAIL;
			}
			else
			{
				fInfo->offset  = offset; 
			}
			break;
		}
	case SWPA_SEEK_CUR : 
		{
			if(buf.st_size < fInfo->offset + offset)
			{
				ret = SWPAR_FAIL;
			}
			else
			{
				fInfo->offset += offset; 
			}
			break;
		}
	case SWPA_SEEK_END :
		{
			if(buf.st_size < offset)
			{
				ret = SWPAR_FAIL;
			}
			else
			{
				fInfo->offset = buf.st_size - offset; 
			}
		}break;
	}
	
	return ret;
}



/**
* @brief 获得当前文件指针位置
*
* 
* @param [in] fd 文件描述符
* @retval 文件指针位置: 执行成功
* @retval SWPAR_FAIL : 失败
* @attention 这个函数的返回值比较特殊，请区别对待
*/
int swpa_normal_file_tell(
	int fd
)
{
	int ret = SWPAR_OK;
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);		//fd 必须不等于0

	//参数打印
	SWPA_FILE_PRINT("fd=%d\n", fd);
	
	NORMAL_FILE_INFO* fInfo = (NORMAL_FILE_INFO *)pheader->device_param;
	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fInfo);
	
	return fInfo->offset;
}






/**
* @brief 对文件描述符的控制
*
* 
* @param [in] fd 文件描述符
* @param [in] cmd 命令标识
* @param [in] args 命令标志参数
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
* @retval SWPAR_NOTIMPL : 未实现
*/
int swpa_normal_file_ioctl(
	int fd, 
	int cmd, 
	void* args
)
{
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);		//fd 必须不等于0
	//SWPA_FILE_CHECK(cmd);		//
	//SWPA_FILE_CHECK(NULL != args);		//args非空
	
	//参数打印
	SWPA_FILE_PRINT("fd=%d\n", fd);
	SWPA_FILE_PRINT("cmd=%d\n", cmd);
	SWPA_FILE_PRINT("args=0x%08x\n", (unsigned int)args);

	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
		
	NORMAL_FILE_INFO* fInfo = (NORMAL_FILE_INFO *)pheader->device_param;

	switch (cmd)
	{
		case SWPA_FILE_IOCTL_TRANCATE:
		{
			SWPA_FILE_CHECK(NULL != args);		//args非空
			int* size = (int*)args;

			SWPA_FILE_CHECK(0 <= *size);		//size非负

			if (0 != ftruncate(fInfo->fd, *size))
			{
				perror("ftrancate failed");
				return SWPAR_FAIL;
			}
		}
		break;

		default:
			//没有实现
			return SWPAR_NOTIMPL;
	}

	
	return SWPAR_OK;
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
int swpa_normal_file_read(
	int fd, 
	void* buf, 
	int size, 
	int* ret_size
)
{
	int ret = SWPAR_OK;
	int bytes = 0;
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	
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
	
	NORMAL_FILE_INFO* fInfo = (NORMAL_FILE_INFO *)pheader->device_param;
	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fInfo);

	if(!SWPA_FILE_IS_READ_MODE(pheader))
	{
		SWPA_FILE_PRINT("Err: file was not opened as Read mode!\n");
		return SWPAR_FAIL;
	}
	
	bytes = pread(fInfo->fd, buf, size, fInfo->offset);
	fInfo->offset += bytes;
	if (NULL != ret_size)
	{		
		*ret_size = bytes;
	}
	
	if (bytes != size)
	{
		if (!swpa_normal_file_eof(fd))
		{
			SWPA_FILE_PRINT("Err: only read %d (!= %d)![%d]\n", bytes, size, SWPAR_FAIL);
			ret = SWPAR_FAIL;
		}
		else
		{
			//到达文件末尾，读取到的数据量小于给定的大小是可能的
		}
	}

	return ret;
}



/**
* @brief 写文件数据
*
* 
* @param [in] fd 文件描述符
* @param [in] buf 读数据缓冲区，必须非空
* @param [in] size 缓冲区大小，必须大于0，单位为字节
* @param [out] ret_size 返回写入的数据大小，若不关心该数值，可传NULL
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
*/
int swpa_normal_file_write(
	int fd, 
	void* buf, 
	int size, 
	int* ret_size
)
{
	int ret = SWPAR_OK;
	int bytes = 0;
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	
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


	NORMAL_FILE_INFO* fInfo = (NORMAL_FILE_INFO *)pheader->device_param;
	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fInfo);

	if(!SWPA_FILE_IS_WRITE_MODE(pheader)
		&& !SWPA_FILE_IS_APPEND_MODE(pheader))
	{
		SWPA_FILE_PRINT("Err: file was not opened as Write mode!\n");
		return SWPAR_FAIL;
	}
	
	//do write
	SWPA_FILE_PRINT("Info: L%d: offset = %d\n", __LINE__, fInfo->offset);
	bytes = pwrite(fInfo->fd, buf, size, fInfo->offset);

	if (bytes == 0)
	{
		SWPA_FILE_PRINT("Err: wrote %d (!= %d)! [%d]\n", bytes, size, SWPAR_FAIL);
		return SWPAR_FAIL;
	}

	fInfo->offset += bytes;
	if (NULL != ret_size)
	{		
		*ret_size = bytes;
	}
	
	return ret;

}



