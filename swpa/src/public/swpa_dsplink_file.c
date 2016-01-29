/**
* @file swpa_dsplink_file.c 
* @brief 实现操作DSPLINK的相关函数
*
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-11
* @version 1.0
*/


#include "swpa_file.h"

#include "swpa_file_private.h"

#include "swpa_dsplink_file.h"

#ifdef __SWPA_FILE_VERIFY
#include "swpa_dsplink_verify.h"
#else
//#include "drv_device.h"
#include "drv_dsplink.h"
#endif


#define __DSPLINK_TIMEOUT_MS_DEFAULT 200 //in ms



typedef struct _DSPLINK_FILE_INFO
{
	unsigned int dev_id; // 文件所在的设备ID
	int timeout_ms;		//DSP处理的超时限制
} DSPLINK_FILE_INFO;




typedef struct _DSPLINK_DEV_INFO
{
	char * image_file;
	int 	len;
	int		fd;
} DSPLINK_DEV_INFO;



static DSPLINK_DEV_INFO _dsplink_dev_info[] =
{
	{"FLASH/0/DSPLINK_IMAGE",		1024,		0}, //note: 开发中测试用，正式镜像文件名需要按照FLASH的规划使用
	{"BLOCK/./MercuryDsp.out",		5*1024*1024,		0}, //note: 开发中测试用，正式镜像文件名需要按照FLASH的规划使用
	{"", 0x0}
};


#define DSPLINK_DEV_NUM (sizeof(_dsplink_dev_info) / sizeof(_dsplink_dev_info[0]))



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
* @retval SWPAR_OUTOFMEMORY : 内存不够
*/
int swpa_dsplink_file_open(
	const char * filename, 
	const char * mode
)
{
	int ret = 0;
	SWPA_FILEHEADER * pheader = NULL;
	DSPLINK_FILE_INFO * pinfo = NULL;
	int dev_id = 0;
	int i = 0;
	int img_file = 0;
	int img_file_len = 0;
	unsigned char * img_buf = NULL;
	int ret_size = 0;
		
	//参数有效性检查
	SWPA_FILE_CHECK(NULL != filename);    	//filename指针非空
	SWPA_FILE_CHECK(0 != swpa_strcmp(filename, ""));//filename非空字符串
	SWPA_FILE_CHECK(NULL != mode);			//mode指针非空
	SWPA_FILE_CHECK(0 != swpa_strcmp(mode, ""));	//mode非空字符串

	//打印参数
	SWPA_FILE_PRINT("filename=%s\n", filename);
	SWPA_FILE_PRINT("mode=%s\n", mode);
	

	//获取设备ID
	swpa_sscanf(filename, "%d", &dev_id);
	
	for (i=0; i<DSPLINK_DEV_NUM; i++)
	{
		if (dev_id == i)
		{
			//该设备已经初始化过，直接返回fd
			if (0 != _dsplink_dev_info[dev_id].fd)
			{
				return _dsplink_dev_info[dev_id].fd;
			}
			else
			{
				break;
			}
		}
	}
	
	//没有找到对应的设备名，报错返回
	if (DSPLINK_DEV_NUM <= i)
	{
		ret = SWPAR_INVALIDARG;
		SWPA_FILE_PRINT("Err: %s not found! [%d]\n", filename, ret);		
		
		goto _ERR_HANDLING;
	}
	

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

	pinfo = (DSPLINK_FILE_INFO *)swpa_mem_alloc(sizeof(*pinfo));
	if (NULL == pinfo)
	{
		ret = SWPAR_OUTOFMEMORY;
	    SWPA_FILE_PRINT("Err: No enough memory for pinfo! [%d]\n", ret);
		
		goto _ERR_HANDLING;
	}

	//
	ret = swpa_file_parse_mode(pheader, mode);
	if (SWPAR_OK != ret)
	{
		SWPA_FILE_PRINT("Err: swpa_file_parse_mode() failed! [%d]\n", ret);		
		goto _ERR_HANDLING; 
		
	}
	else if (SWPA_FILE_IS_APPEND_MODE(pheader) || 
		SWPA_FILE_IS_READ_MODE(pheader))
	{
		ret = SWPAR_INVALIDARG;
		SWPA_FILE_PRINT("Err: mode (%s)  is invalid for dsplink files! [%d]\n", mode, ret);		
		goto _ERR_HANDLING; 
	}

	SWPA_FILE_PRINT("dsp:%s\n", _dsplink_dev_info[dev_id].image_file);

	//打开DSPLINK镜像文件
	img_file = swpa_file_open(_dsplink_dev_info[dev_id].image_file, "r");
	if (img_file <= 0 && img_file > SWPAR_MIN)
	{
		ret = SWPAR_FAIL;
		SWPA_FILE_PRINT("Err: open %s failed (%d)! [%d]\n", _dsplink_dev_info[dev_id].image_file, img_file, SWPAR_FAIL);		
		
		goto _ERR_HANDLING; 
	}

	//获取DSPLINK镜像文件的大小
	swpa_file_seek(img_file, 0, SWPA_SEEK_END);
	img_file_len = swpa_file_tell(img_file);
	swpa_file_seek(img_file, 0, SWPA_SEEK_SET);
	if (0 >= img_file_len)
	{
		ret = SWPAR_FAIL;
		SWPA_FILE_PRINT("Err: length (%d) of %s is invalid! [%d]\n", img_file_len, _dsplink_dev_info[dev_id].image_file, SWPAR_FAIL);		
		
		goto _ERR_HANDLING; 
	}

	//申请存放镜像文件的内存块
	img_buf = (unsigned char *)swpa_mem_alloc(img_file_len);
	if (NULL == img_buf)
	{
		ret = SWPAR_OUTOFMEMORY;
	    SWPA_FILE_PRINT("Err: No enough memory for img_buf! [%d]\n", ret);
		
		goto _ERR_HANDLING;
	}

	//读取DSPLINK镜像文件到内存，以便加载
	ret = swpa_file_read(img_file, img_buf, img_file_len, &ret_size);
	if (SWPAR_OK != ret || ret_size != img_file_len)
	{
		ret = SWPAR_FAIL;
	    SWPA_FILE_PRINT("Err: read %s failed! [%d]\n", _dsplink_dev_info[dev_id].image_file, ret);
		
		goto _ERR_HANDLING;
	}

	swpa_file_close(img_file);
	
	//加载DSPLINK镜像文件并初始化DSP设备
	ret = drv_dsplink_setup(img_buf, img_file_len);
	if (0 != ret)
	{
		ret = SWPAR_FAIL;
	    SWPA_FILE_PRINT("Err: drv_dsplink_setup() failed! [%d]\n", ret);
		
		goto _ERR_HANDLING;
	}

	//释放镜像文件占用的内存
	swpa_mem_free(img_buf);
	img_buf = NULL;

	pinfo->timeout_ms = __DSPLINK_TIMEOUT_MS_DEFAULT;
	pinfo->dev_id = dev_id;
	
	_dsplink_dev_info[dev_id].fd = (int)pheader;
	
	pheader->device_param = (int)pinfo;	
			
	pheader->file_open 	= swpa_dsplink_file_open;
	pheader->file_close = swpa_dsplink_file_close;
	pheader->file_seek 	= NULL;//swpa_dsplink_file_seek;
	pheader->file_eof 	= NULL;//swpa_dsplink_file_eof;
	pheader->file_tell 	= NULL;//swpa_dsplink_file_tell;
	pheader->file_ioctl = swpa_dsplink_file_ioctl;
	pheader->file_read 	= NULL;//swpa_dsplink_file_read;
	pheader->file_write	= swpa_dsplink_file_write;
	
	return (int)pheader;


//错误处理并返回
_ERR_HANDLING:

	if (0 < img_file)
	{
		swpa_file_close(img_file);
	}

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

	if (NULL != pinfo)
	{
		swpa_mem_free(pinfo);
		pinfo = NULL;
	}
	if (NULL != img_buf)
	{
		swpa_mem_free(img_buf);
		img_buf = NULL;
	}
	
	return ret;
	
}



/**
* @brief 关闭dsplink设备文件
*
* 
* @param [in] fd 文件描述符
* @retval SWPAR_OK :  成功
* @retval SWPAR_FAIL :  失败
*/
int swpa_dsplink_file_close(
	int fd
)
{
	int ret = SWPAR_OK;
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);    	//fd 必须不等于0
	SWPA_FILE_CHECK(0 != pheader->device_param);

	//参数打印
	SWPA_FILE_PRINT("fd=%d\n", fd);	


	//关闭DSP设备
	ret = drv_dsplink_exit();
	if (0 != ret)
	{
		SWPA_FILE_PRINT("Err: drv_dsplink_exit failed! [%d]\n", ret);
		return ret;
	}

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

	SWPA_FILE_CHECK_RET(swpa_mem_free((void*)pheader->device_param), SWPAR_FAIL);
	pheader->device_param = (int)NULL;

	SWPA_FILE_CHECK_RET(swpa_mem_free(pheader), SWPAR_FAIL);
	pheader = NULL;

	return ret;
}





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
int swpa_dsplink_file_write(
	int fd, 
	void* buf, 
	int size, 
	int* ret_size
)
{
	int ret = SWPAR_OK;
	unsigned int bytes = 0;
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	DSPLINK_FILE_INFO * pinfo = NULL;
	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);		//fd 必须不等于0
	SWPA_FILE_CHECK(NULL != buf);		//buf非空
	SWPA_FILE_CHECK(0 < size);		//size 必须大于0
	//SWPA_FILE_CHECK(NULL != ret_size);		//Note: ret_size可以是NULL，故注释掉
	
	//参数打印
	SWPA_FILE_PRINT("fd=%d\n", fd);
	SWPA_FILE_PRINT("buf=0x%08x\n", buf);
	SWPA_FILE_PRINT("size=%d\n", size);
	SWPA_FILE_PRINT("ret_size=0x%08x\n", ret_size);
	

	pinfo = (DSPLINK_FILE_INFO *)pheader->device_param;
	SWPA_FILE_CHECK(NULL != pinfo);		//pinfo非空


	if (!SWPA_FILE_IS_WRITE_MODE(pheader))
	{
		SWPA_FILE_PRINT("Err: file is not in write mode![%d]\n", SWPAR_FAIL );
		return SWPAR_FAIL;
	}

	//do write
	ret =  drv_dsplink_doprocess((unsigned int)buf, pinfo->timeout_ms);
	if (0 != ret)
	{
		SWPA_FILE_PRINT("Err: drv_dsplink_doprocess() failed (%d)![%d]\n", ret, SWPAR_FAIL);
		return SWPAR_FAIL;
	}

	
	if (NULL != ret_size)
	{
		*ret_size = bytes;
	}

	return SWPAR_OK;
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
int swpa_dsplink_file_ioctl(
	int fd, 
	int cmd, 
	void* args
)
{
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	DSPLINK_FILE_INFO * pinfo = (DSPLINK_FILE_INFO *)pheader->device_param;	

	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);    	//fd 必须不等于0
	//SWPA_FILE_CHECK(0 < cmd);    	//cmd取值没有限定，故mark掉
	//SWPA_FILE_CHECK(NULL != args);    	//args 在不同cmd下可以为NULL，故mark掉

	//参数打印
	SWPA_FILE_PRINT("fd=%d\n", fd);	
	SWPA_FILE_PRINT("cmd=%d\n", cmd);	
	SWPA_FILE_PRINT("args=0x%08x\n", args);


	switch (cmd)
	{
		case SWPA_FILE_SET_READ_TIMEOUT:
		case SWPA_FILE_SET_WRITE_TIMEOUT:
		{
			int timeout = *(int*)args;

			SWPA_FILE_CHECK(0 < timeout);    	//超时时长须大于零
			
			pinfo->timeout_ms = timeout;
		}
		break;
		
		default:
			SWPA_FILE_PRINT("Err: invalid cmd (%d) for dsplink ioctl! [%d]\n", cmd, SWPAR_INVALIDARG);
			return SWPAR_INVALIDARG;
	}

	return SWPAR_OK;
}



