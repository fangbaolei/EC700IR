/**
* @file swpa_file_private.c
* @brief 实现文件模块内部使用 的相关函数
* 
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-02-27
* @version 1.0
*/


#include "swpa_file_private.h"
#include <stdlib.h>


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
)
{
	
	//参数有效性检查
	SWPA_FILE_CHECK(NULL != pheader);    	//pheader指针非空
	SWPA_FILE_CHECK(NULL != filename);			//filename指针非空
	SWPA_FILE_CHECK(swpa_strcmp(filename, ""));	//filename非空字符串

	//打印参数
	SWPA_FILE_PRINT("pheader=0x%08x\n", (unsigned int)pheader);
	SWPA_FILE_PRINT("filename=%s\n", filename);

	//if (SWPAR_OK != swpa_mutex_create(&pheader->rwlock, filename))
	if (SWPAR_OK != swpa_mutex_create(&pheader->rwlock, NULL))
	{
		SWPA_FILE_PRINT("Err: failed to create _rwlock %s ! [%d]\n", filename, SWPAR_FAIL);		
		return SWPAR_FAIL;
	}

	return SWPAR_OK;	
}



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
int swpa_file_parse_mode(
	SWPA_FILEHEADER * pheader, 
	const char * mode
)
{
	
	//参数有效性检查
	SWPA_FILE_CHECK(NULL != pheader);    	//pheader指针非空
	SWPA_FILE_CHECK(NULL != mode);			//mode指针非空
	SWPA_FILE_CHECK(swpa_strcmp(mode, ""));	//mode非空字符串

	//打印参数
	SWPA_FILE_PRINT("pheader=0x%08x\n", (unsigned int)pheader);
	SWPA_FILE_PRINT("mode=%s\n", mode);

	
	pheader->mode = 0;																
	if (0 == swpa_strcmp(mode, "r"))												
	{																				
		SWPA_FILE_SET_MODE((pheader), SWPA_FILE_READ_MODE); /*//设置为可读*/	
		return SWPAR_OK;
	}																				
	else if (0 == swpa_strcmp(mode, "r+"))											
	{																				
		SWPA_FILE_SET_MODE((pheader), SWPA_FILE_READ_MODE); /*//设置为可读*/		
		SWPA_FILE_SET_MODE(pheader, SWPA_FILE_WRITE_MODE); /*//设置为可写*/ 	
		
		return SWPAR_OK;
	}																				
	else if (0 == swpa_strcmp(mode, "w"))											
	{																				
		SWPA_FILE_SET_MODE(pheader, SWPA_FILE_WRITE_MODE); /*//设置为可写*/ 
		SWPA_FILE_SET_MODE(pheader, SWPA_FILE_CREATE_MODE); /*//设置为文件不存在则创建*/ 
		return SWPAR_OK;
	}																				
	else if (0 == swpa_strcmp(mode, "w+"))											
	{																				
		SWPA_FILE_SET_MODE((pheader), SWPA_FILE_READ_MODE); /*//设置为可读*/		
		SWPA_FILE_SET_MODE(pheader, SWPA_FILE_WRITE_MODE); /*//设置为可写*/ 	
		SWPA_FILE_SET_MODE(pheader, SWPA_FILE_CREATE_MODE); /*//设置为文件不存在则创建*/ 
		return SWPAR_OK;
	}
	else if (0 == swpa_strcmp(mode, "wt"))				//文本文件								
	{																				
		SWPA_FILE_SET_MODE(pheader, SWPA_FILE_WRITE_MODE); /*//设置为可写*/ 
		SWPA_FILE_SET_MODE(pheader, SWPA_FILE_CREATE_MODE); /*//设置为文件不存在则创建*/ 
		return SWPAR_OK;
	}																				
	else if (0 == swpa_strcmp(mode, "wt+"))				//文本文件			
	{																				
		SWPA_FILE_SET_MODE((pheader), SWPA_FILE_READ_MODE); /*//设置为可读*/		
		SWPA_FILE_SET_MODE(pheader, SWPA_FILE_WRITE_MODE); /*//设置为可写*/ 	
		SWPA_FILE_SET_MODE(pheader, SWPA_FILE_CREATE_MODE); /*//设置为文件不存在则创建*/ 
		return SWPAR_OK;
	}
	else if (0 == swpa_strcmp(mode, "wb"))				//二进制文件							
	{																				
		SWPA_FILE_SET_MODE(pheader, SWPA_FILE_WRITE_MODE); /*//设置为可写*/ 
		SWPA_FILE_SET_MODE(pheader, SWPA_FILE_CREATE_MODE); /*//设置为文件不存在则创建*/ 
		return SWPAR_OK;
	}																				
	else if (0 == swpa_strcmp(mode, "wb+"))				//二进制文件					
	{																				
		SWPA_FILE_SET_MODE((pheader), SWPA_FILE_READ_MODE); /*//设置为可读*/		
		SWPA_FILE_SET_MODE(pheader, SWPA_FILE_WRITE_MODE); /*//设置为可写*/ 	
		SWPA_FILE_SET_MODE(pheader, SWPA_FILE_CREATE_MODE); /*//设置为文件不存在则创建*/ 
		return SWPAR_OK;
	}			
	else if (0 == swpa_strcmp(mode, "a"))											
	{																				
		SWPA_FILE_SET_MODE(pheader, SWPA_FILE_APPEND_MODE); /*//设置为追加*/	
		return SWPAR_OK;
	}																				
	else if (0 == swpa_strcmp(mode, "a+"))											
	{
		SWPA_FILE_SET_MODE(pheader, SWPA_FILE_APPEND_MODE); /*//设置为追加*/	
		SWPA_FILE_SET_MODE(pheader, SWPA_FILE_READ_MODE); /*//设置为可读*/		
		/*SWPA_FILE_SET_MODE(pheader, SWPA_FILE_WRITE_MODE); *//*//设置为可写*/
		return SWPAR_OK;
	}
	else
	{
		SWPA_FILE_PRINT("Err: mode (%s) is invalid! [%d]\n", mode, SWPAR_INVALIDARG);
		return SWPAR_INVALIDARG;
	}
	
}



