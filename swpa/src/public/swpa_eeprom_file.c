/**
* @file swpa_eeprom_file.c
* @brief 实现EEPROM相关的操作函数
*
* 
*/


#include "swpa_file_private.h"
#include "swpa_eeprom_file.h"

#ifdef __SWPA_FILE_VERIFY
#include "swpa_eeprom_verify.h"
#else
#include "drv_device.h"
#include "drv_device_eeprom_hal.h"
#endif


//#ifdef SWPA_FILE_PRINT
//#undef SWPA_FILE_PRINT
//#endif

//#define SWPA_FILE_PRINT printf("[%s %d] ", __FILE__, __LINE__)&&printf


typedef struct _EEPROM_FILE_INFO
{
	unsigned int dev_id; // 文件所在的设备ID
	unsigned int file_id; // 文件编号ID
	unsigned int addr;	//文件的起始地址
	unsigned int len;	//文件的长度
	unsigned int seg_len;	//文件所在段的长度
	unsigned int offset;	//文件内部偏移指针
} EEPROM_FILE_INFO;



//EEPROM上存储段的定义
typedef struct _EEPROM_SEG_DEF
{
	char * 			name; // 段名
	unsigned int 	addr;	//段起始地址
	unsigned int 	len;	//段长度
	unsigned int	permission;
	unsigned int	dev_id; //所在设备的ID
	
} EEPROM_SEG_DEF;


//EEPROM段中文件信息
typedef struct _EEPROM_SEG_FILE_INFO
{
	//unsigned int 	addr;	//段起始地址
	unsigned int 	len;	//段长度
	unsigned int	permission;
	
} EEPROM_SEG_FILE_INFO;



#define PERMISSION_READ			(0x1<<0)
#define PERMISSION_WRITE		(0x1<<1)


#ifdef DM6467
  //8KB 产品组使用
  
  
  //2KB 网络参数
  #define NETWORK_INFO_ADDR		0x2000
  #define NETWORK_INFO_LEN		0x800
  
  
  //32KB 应用参数
  #define APP_PARAM_ADDR			0x2800
  #define APP_PARAM_LEN			0x8000
  
  
  //32KB 应用参数备份
  #define APP_PARAM_BAK_ADDR		0xA800
  #define APP_PARAM_BAK_LEN		0x8000
  
  
  //16KB黑匣子数据保存区
  #define BLACK_BOX_ADDR			0x12800
  #define BLACK_BOX_LEN			0x4000
  
  
  //1KB 保留设备重启相关数据
  #define RESET_INFO_ADDR			0x16800
  #define RESET_INFO_LEN			0x400
  
  //1KB 保留设备工作模式相关数据
  #define WORKINGMODE_INFO_ADDR	0x16C00
  #define WORKINGMODE_INFO_LEN	0x400
  
  //1KB 保留应用层复位信息
  #define APP_RESET_INFO_ADDR			0x17000
  #define APP_RESET_INFO_LEN			0x400
  
  //36KB 预留
  #define RESERVED_ADDR			0x17400
  #define RESERVED_LEN			0x8C00
  
  //UBL
  #define UBL_ADDR				0x0
  #define UBL_LEN					0x100
  
  
  //EEPROM 段分配表
  static EEPROM_SEG_DEF _eeprom_seg_info[] = 
  {	
  	//SPI_EEPROM
  	{"UBL", 				UBL_ADDR, 				UBL_LEN,				PERMISSION_READ | PERMISSION_WRITE,		DEVID_SPI_EEPROM},
  
  	{"NETWORK_INFO",		NETWORK_INFO_ADDR, 		NETWORK_INFO_LEN,		PERMISSION_READ, 						DEVID_IIC_EEPROM},
  	{"APP_PARAM", 			APP_PARAM_ADDR, 		APP_PARAM_LEN,			PERMISSION_READ | PERMISSION_WRITE, 	DEVID_IIC_EEPROM},
  	{"APP_PARAM_BAK", 		APP_PARAM_BAK_ADDR, 	APP_PARAM_BAK_LEN,		PERMISSION_READ | PERMISSION_WRITE, 	DEVID_IIC_EEPROM},
  	{"BLACK_BOX", 			BLACK_BOX_ADDR, 		BLACK_BOX_LEN,			PERMISSION_READ | PERMISSION_WRITE, 	DEVID_IIC_EEPROM},
  	{"RESET_INFO",			RESET_INFO_ADDR,		RESET_INFO_LEN, 		PERMISSION_READ | PERMISSION_WRITE, 	DEVID_IIC_EEPROM},
  	{"WORKINGMODE_INFO",	WORKINGMODE_INFO_ADDR,	WORKINGMODE_INFO_LEN, 	PERMISSION_READ | PERMISSION_WRITE, 	DEVID_IIC_EEPROM},
  	{"APP_RESET_INFO",		APP_RESET_INFO_ADDR,	 APP_RESET_INFO_LEN, 	PERMISSION_READ | PERMISSION_WRITE, 	DEVID_IIC_EEPROM},
  	/* Note for RESERVED: 
  	*   1. The last 1024 Bytes are used to store each file's info, including file size, r/w permission, etc.
  	*   2. 
  	*/
  	{"RESERVED", 			RESERVED_ADDR, 			RESERVED_LEN,			PERMISSION_READ | PERMISSION_WRITE, 	DEVID_IIC_EEPROM}
  };
#endif

#ifdef DM8127
  //8KB 产品组使用
  
  //32KB 应用参数
  #define APP_PARAM_ADDR			0x2000
  #define APP_PARAM_LEN			0x8000
  
  //32KB 应用参数备份
  #define APP_PARAM_BAK_ADDR		0xA000
  #define APP_PARAM_BAK_LEN		0x8000
  
  //16KB黑匣子数据保存区
  #define BLACK_BOX_ADDR			0x12000
  #define BLACK_BOX_LEN			0x4000
  
  //1KB 保留设备重启相关数据
  #define RESET_INFO_ADDR			0x16000
  #define RESET_INFO_LEN			0x400
  
  //1KB 保留设备工作模式相关数据
  #define WORKINGMODE_INFO_ADDR	0x16400
  #define WORKINGMODE_INFO_LEN	0x400
  
  //1KB 保留应用层复位信息
  #define APP_RESET_INFO_ADDR		0x16800
  #define APP_RESET_INFO_LEN		0x400
  
  //1KB 用户信息
  #define USER_MANAGER_INFO_ADDR	0x16c00
  #define USER_MANAGER_INFO_LEN		0x400

  //36KB 预留
  #define RESERVED_ADDR			0x17000
  #define RESERVED_LEN			0x9000
  
  
  //EEPROM 段分配表
  static EEPROM_SEG_DEF _eeprom_seg_info[] = 
  {	
  	{"APP_PARAM", 			APP_PARAM_ADDR, 		APP_PARAM_LEN,			PERMISSION_READ | PERMISSION_WRITE, 	DEVID_IIC_EEPROM},
  	{"APP_PARAM_BAK", 		APP_PARAM_BAK_ADDR, 	APP_PARAM_BAK_LEN,		PERMISSION_READ | PERMISSION_WRITE, 	DEVID_IIC_EEPROM},
  	{"BLACK_BOX", 			BLACK_BOX_ADDR, 		BLACK_BOX_LEN,			PERMISSION_READ | PERMISSION_WRITE, 	DEVID_IIC_EEPROM},
  	{"RESET_INFO",			RESET_INFO_ADDR,		RESET_INFO_LEN, 		PERMISSION_READ | PERMISSION_WRITE, 	DEVID_IIC_EEPROM},
  	{"WORKINGMODE_INFO",	WORKINGMODE_INFO_ADDR,	WORKINGMODE_INFO_LEN, 	PERMISSION_READ | PERMISSION_WRITE, 	DEVID_IIC_EEPROM},
  	{"APP_RESET_INFO",		APP_RESET_INFO_ADDR,	 APP_RESET_INFO_LEN, 	PERMISSION_READ | PERMISSION_WRITE, 	DEVID_IIC_EEPROM},
  	{"USER_MANAGER_INFO",	USER_MANAGER_INFO_ADDR,	USER_MANAGER_INFO_LEN,	PERMISSION_READ | PERMISSION_WRITE, 	DEVID_IIC_EEPROM},
  	/* Note for RESERVED: 
  	*   1. The last 1024 Bytes are used to store each file's info, including file size, r/w permission, etc.
  	*   2. 
  	*/
  	{"RESERVED", 			RESERVED_ADDR, 			RESERVED_LEN,			PERMISSION_READ | PERMISSION_WRITE, 	DEVID_IIC_EEPROM}
  };
#endif


//用于存放每个文件的信息。
// !!!注意，根据这里的定义，最多只能保存16个文件的信息!!!
#define IIC_EEPROM_FILEINFO_SEG_LEN			0x400
#define IIC_EEPROM_FILEINFO_SEG_ADDR		(RESERVED_ADDR+RESERVED_LEN-IIC_EEPROM_FILEINFO_SEG_LEN)

//单个文件信息的大小
#define IIC_EEPROM_FILEINFO_LEN			0x20


//段总数
#define EEPROM_SEG_NUM 	(sizeof(_eeprom_seg_info) / sizeof(_eeprom_seg_info[0]))


static int _fileinfo_lock = 0;

#define _CREATE_FILE_INFO_LOCK() \
	if (0 ==_fileinfo_lock && SWPAR_OK != swpa_mutex_create(&_fileinfo_lock, NULL))\
	{\
		SWPA_FILE_PRINT("Err: failed to create eeprom fileinfo lock! [%d]\n", SWPAR_FAIL);\
		return SWPAR_FAIL;	\
	}

#define _LOCK_FILE_INFO()	\
	if (0 !=_fileinfo_lock && SWPAR_OK != swpa_mutex_lock(&_fileinfo_lock, -1))\
	{\
		SWPA_FILE_PRINT("Err: failed to lock eeprom fileinfo! [%d]\n", SWPAR_FAIL);\
		return SWPAR_FAIL;	\
	}
	
#define _UNLOCK_FILE_INFO()	\
	if (0 !=_fileinfo_lock && SWPAR_OK != swpa_mutex_unlock(&_fileinfo_lock))\
	{\
		SWPA_FILE_PRINT("Err: failed to unlock eeprom fileinfo! [%d]\n", SWPAR_FAIL);\
		return SWPAR_FAIL;	\
	}

//zhaopy EEPROM表头信息内存缓冲区
// EEPROM 表头信息只有在第一次加载时从EEPROM读取。
unsigned char g_rgbFileInfo[EEPROM_SEG_NUM][IIC_EEPROM_FILEINFO_LEN] = {{0}};
int g_iFileInfoInit[EEPROM_SEG_NUM] = {0};

static int _eeprom_check(const unsigned int dev_id, const unsigned int file_id, int* pvalid)
{
	int ret = 0;
	
	SWPA_FILE_PRINT("info: do check id[0X%08X]\n", dev_id);

	if (NULL == pvalid)
	{
		SWPA_FILE_PRINT("Err: NULL == pvalid! [%d]\n", SWPAR_INVALIDARG);
		return SWPAR_INVALIDARG;		
	}

	_CREATE_FILE_INFO_LOCK();
	
	if (DEVID_SPI_EEPROM == dev_id)
	{ 
		*pvalid = 1;
		return SWPAR_OK;
	}
	else if (DEVID_IIC_EEPROM == dev_id)
	{ 
		*pvalid = 0;

		if (file_id >= EEPROM_SEG_NUM)
		{
			SWPA_FILE_PRINT("Err: file id(%d) is invalid (>= total file count %d)\n", file_id, EEPROM_SEG_NUM);
			return SWPAR_INVALIDARG;
		}

		_LOCK_FILE_INFO();
		
		unsigned int index = file_id;
		if( g_iFileInfoInit[index] == 0 )
		{
			// zhaopy
			// 初始化表头信息。
			//for (index = 0; index < EEPROM_SEG_NUM; index ++)
			{
				unsigned int size = 0;
				unsigned int master_table_addr = IIC_EEPROM_FILEINFO_SEG_ADDR + IIC_EEPROM_FILEINFO_LEN * index;
				int iInfoMaxLen = IIC_EEPROM_FILEINFO_LEN;
				int iReadSize = 0;

				iReadSize = iInfoMaxLen;
				ret = drv_eeprom_read(dev_id, master_table_addr, g_rgbFileInfo[index], &iReadSize);
				if (0 != ret || iReadSize != iInfoMaxLen)
				{
					SWPA_FILE_PRINT("Err: failed (ret = %d) to get eeprom fileinfo(fileid=%d)! [%d]\n", ret, index, SWPAR_FAIL );
					_UNLOCK_FILE_INFO();
					return SWPAR_FAIL;
				}
				
				// CRC
				unsigned int nCrcStored = 0;
				swpa_memcpy(&nCrcStored, g_rgbFileInfo[index], sizeof(nCrcStored));
				// FILE count;
				int iFileCount = 0;
				swpa_memcpy(&iFileCount, g_rgbFileInfo[index] + sizeof(nCrcStored), sizeof(iFileCount));
				// check file count
				// 如果保存的文件数与当前的文件数不一致，即会重新初始化EEPROM表信息。
				if( iFileCount != EEPROM_SEG_NUM )
				{
					*pvalid = 0;
					SWPA_FILE_PRINT("Warning: EEPROM_SEG_NUM(%d) != filecount(%d)\n", EEPROM_SEG_NUM, iFileCount);
					_UNLOCK_FILE_INFO();
					return SWPAR_OK;
				}
				// FILE INFO size
				//int iFileInfoSize = iFileCount * sizeof(EEPROM_SEG_FILE_INFO);
				unsigned int nCurCrc32 = swpa_utils_calc_crc32(0, g_rgbFileInfo[index] + sizeof(nCrcStored), iInfoMaxLen - sizeof(nCrcStored));
				// check crc
				if( nCurCrc32 != nCrcStored )
				{
					SWPA_FILE_PRINT("Warning: master table crc failed, read bak table.\n");
					// read bak
					unsigned int backup_table_addr = IIC_EEPROM_FILEINFO_SEG_ADDR + IIC_EEPROM_FILEINFO_SEG_LEN / 2 + IIC_EEPROM_FILEINFO_LEN * index;
					unsigned int nCrcStoredBak = 0;
					iReadSize = iInfoMaxLen;

					ret = drv_eeprom_read(dev_id, backup_table_addr, g_rgbFileInfo[index], &iReadSize);
					if (0 != ret || iReadSize != iInfoMaxLen)
					{
						SWPA_FILE_PRINT("Err: failed (ret = %d) to get eeprom fileinfo(bak)! [%d]\n", ret, SWPAR_FAIL );
						_UNLOCK_FILE_INFO();
						return SWPAR_FAIL;
					}
					swpa_memcpy(&nCrcStoredBak, g_rgbFileInfo[index], sizeof(nCrcStoredBak));
					swpa_memcpy(&iFileCount, g_rgbFileInfo[index] + sizeof(nCrcStoredBak), sizeof(iFileCount));
					// check file count
					// 如果保存的文件数与当前的文件数不一致，即会重新初始化EEPROM表信息。
					if( iFileCount != EEPROM_SEG_NUM )
					{
						*pvalid = 0;
						SWPA_FILE_PRINT("Warning(bak): EEPROM_SEG_NUM(%d) != filecount(%d)\n", EEPROM_SEG_NUM, iFileCount);
						_UNLOCK_FILE_INFO();
						return SWPAR_OK;
					}
					//iFileInfoSize = iFileCount * sizeof(EEPROM_SEG_FILE_INFO);
					unsigned int nCurCrc32Bak = swpa_utils_calc_crc32(0, g_rgbFileInfo[index] + sizeof(nCrcStoredBak), iInfoMaxLen - sizeof(nCrcStored));
					if( nCurCrc32Bak != nCrcStoredBak )
					{
						SWPA_FILE_PRINT("Warning: bak table crc failed, reinit.\n");
						*pvalid = 0;
						_UNLOCK_FILE_INFO();
						return SWPAR_OK;
					}

					// 用从表更新主表
					//unsigned int nBufferSize = iInfoMaxLen;//sizeof(nCrcStoredBak) + sizeof(iFileCount) + iFileInfoSize;
					unsigned int nWriteLen = iInfoMaxLen;
					ret = drv_eeprom_write(dev_id, master_table_addr, g_rgbFileInfo[index], &nWriteLen);
					if( 0 != ret || nWriteLen != iInfoMaxLen )
					{
						SWPA_FILE_PRINT("Err: write master table failed (ret = %d).\n", ret );
						_UNLOCK_FILE_INFO();
						return SWPAR_FAIL;
					}

					g_iFileInfoInit[index] = 1;
					*pvalid = 1;
					_UNLOCK_FILE_INFO();
					return SWPAR_OK;
				}
			}
			
			*pvalid = 1;
			g_iFileInfoInit[index] = 1;
			_UNLOCK_FILE_INFO();
			return SWPAR_OK;
		}
		else
		{
			*pvalid = 1;
			_UNLOCK_FILE_INFO();
			return SWPAR_OK;
		}
	}
	else
	{
		SWPA_FILE_PRINT("Err: dev_id = %u is invalid! [%d]\n", dev_id, SWPAR_INVALIDARG);
		return SWPAR_INVALIDARG;
	}
}

static int _eeprom_init(const unsigned int dev_id, const unsigned int file_id)
{
	int ret = 0;

	_CREATE_FILE_INFO_LOCK();

	SWPA_FILE_PRINT("info: do init id[%d]\n", dev_id);

	if (DEVID_SPI_EEPROM == dev_id)
	{
		return SWPAR_OK;
	}
	else if (DEVID_IIC_EEPROM == dev_id)
	{
		if (file_id >= EEPROM_SEG_NUM)
		{
			SWPA_FILE_PRINT("Err: file id(%d) is invalid (>= total file count %d)\n", file_id, EEPROM_SEG_NUM);
			return SWPAR_INVALIDARG;
		}
		
		if( g_iFileInfoInit[file_id])
		{
			return SWPAR_OK;
		}

		unsigned int filecount = EEPROM_SEG_NUM;
		unsigned int crc32 = 0;
		unsigned int size = 0;
		EEPROM_SEG_FILE_INFO info;

		unsigned int index = file_id;
		//for (index=0; index<filecount; index++)
		{
			unsigned int master_table_addr = IIC_EEPROM_FILEINFO_SEG_ADDR + IIC_EEPROM_FILEINFO_LEN * index;
			unsigned int backup_table_addr = IIC_EEPROM_FILEINFO_SEG_ADDR + IIC_EEPROM_FILEINFO_SEG_LEN / 2 + IIC_EEPROM_FILEINFO_LEN * index;
			

			unsigned int buf_size = IIC_EEPROM_FILEINFO_LEN;//sizeof(crc32) + sizeof(filecount) +  filecount*sizeof(info);
			swpa_memset(g_rgbFileInfo[index], 0, IIC_EEPROM_FILEINFO_LEN);
			swpa_memcpy(g_rgbFileInfo[index] + sizeof(crc32), &filecount, sizeof(filecount));
			
					
			info.len = (PERMISSION_WRITE & _eeprom_seg_info[index].permission) ? 0 : _eeprom_seg_info[index].len;
			info.permission = _eeprom_seg_info[index].permission;

			SWPA_FILE_PRINT("Info: info[%d].len = %#x\n", index, info.len);

			swpa_memcpy(g_rgbFileInfo[index] + sizeof(crc32) + sizeof(filecount), &info, sizeof(info));

			crc32 = swpa_utils_calc_crc32(0, (unsigned char*)g_rgbFileInfo[index] + sizeof(crc32), buf_size-sizeof(crc32));
			swpa_memcpy(g_rgbFileInfo[index], &crc32, sizeof(crc32));

			size = buf_size;
			ret = drv_eeprom_write(dev_id, 
				master_table_addr, 
				(unsigned char*)g_rgbFileInfo[index], 
				&size);
			if (SWPAR_OK != ret || size != buf_size)
			{
				SWPA_FILE_PRINT("Err: failed (ret = %d) to write master eeprom file info! [%d]\n", ret, SWPAR_FAIL );
				_UNLOCK_FILE_INFO();
				return SWPAR_FAIL;
			}

			size = buf_size;
			ret = drv_eeprom_write(dev_id, 
				backup_table_addr, 
				(unsigned char*)g_rgbFileInfo[index], 
				&size);
			if (SWPAR_OK != ret || size != buf_size)
			{
				SWPA_FILE_PRINT("Err: failed (ret = %d) to write backup eeprom file info! [%d]\n", ret, SWPAR_FAIL );
				_UNLOCK_FILE_INFO();
				return SWPAR_FAIL;
			}
		}
			
		g_iFileInfoInit[index] = 1;
		_UNLOCK_FILE_INFO();
		return SWPAR_OK;
	}
	else
	{
		SWPA_FILE_PRINT("Err: dev_id = %u is invalid [%d]\n", dev_id, SWPAR_INVALIDARG);
		return SWPAR_INVALIDARG;
	}
}

static int _eeprom_get_file_size(const unsigned int dev_id, const unsigned int file_id, unsigned int* filesize)
{
	int ret = SWPAR_OK;
	
	EEPROM_SEG_FILE_INFO info;

	if (NULL == filesize)
	{
		SWPA_FILE_PRINT("Err: NULL == filesize\n", SWPAR_INVALIDARG);
		return SWPAR_INVALIDARG;		
	}

	_CREATE_FILE_INFO_LOCK();

	*filesize = _eeprom_seg_info[file_id].len;
	
	if (DEVID_SPI_EEPROM == dev_id)
	{
		return SWPAR_OK;
	}
	else if (DEVID_IIC_EEPROM == dev_id)
	{
		int is_valid = 0;
		if (SWPAR_OK != _eeprom_check(DEVID_IIC_EEPROM, file_id, &is_valid) 
			|| !is_valid)
		{
			if (SWPAR_OK != _eeprom_init(DEVID_IIC_EEPROM, file_id))
			{
				SWPA_FILE_PRINT("Err: failed to restore eeprom info table ![%d]\n", SWPAR_FAIL);
				SWPA_FILE_PRINT("Info: got filesize = %#x\n", *filesize);
				return SWPAR_FAIL;
			}
		}

		_LOCK_FILE_INFO();
		if (g_iFileInfoInit[file_id])
		{
			swpa_memcpy(&info, g_rgbFileInfo[file_id] + sizeof(unsigned int/*crc32*/) + sizeof(unsigned int/*filecount*/), sizeof(info));
		}
#if 0
		else
		{
			unsigned int size = sizeof(info);
			ret = drv_eeprom_read(dev_id, 
				IIC_EEPROM_FILEINFO_SEG_ADDR + IIC_EEPROM_FILEINFO_LEN * file_id + sizeof(unsigned int/*crc32*/) + sizeof(unsigned int/*filecount*/),
				(unsigned char*)&info,
				&size);
			if (0 != ret || size != sizeof(info))
			{
				SWPA_FILE_PRINT("Err: failed to get file (id = %u) info [%d]\n", file_id, SWPAR_FAIL);
				SWPA_FILE_PRINT("Info: got filesize = %#x\n", *filesize);
				_UNLOCK_FILE_INFO();
				return SWPAR_FAIL;
			}
		}
#endif		
		_UNLOCK_FILE_INFO();
		
		*filesize = (info.len > _eeprom_seg_info[file_id].len) ? _eeprom_seg_info[file_id].len : info.len;

		//printf("Info: got eeprom file(%d) filesize = %#x\n", file_id, *filesize);
		
		return SWPAR_OK;
	}		
	else
	{
		SWPA_FILE_PRINT("Err: dev_id = %u is invalid [%d]\n", dev_id, SWPAR_INVALIDARG);
		return SWPAR_INVALIDARG;
	}
}

static int _eeprom_save_file_size(const unsigned int dev_id, const unsigned int file_id, const unsigned int filesize)
{
	int ret = SWPAR_OK;

	_CREATE_FILE_INFO_LOCK();

	if (DEVID_SPI_EEPROM == dev_id)
	{
		return SWPAR_OK;
	}
	else if (DEVID_IIC_EEPROM == dev_id)
	{
		unsigned int realsize = (filesize > _eeprom_seg_info[file_id].len) ? _eeprom_seg_info[file_id].len : filesize;		
		EEPROM_SEG_FILE_INFO info;

		int is_valid = 0;
		if (SWPAR_OK != _eeprom_check(DEVID_IIC_EEPROM, file_id, &is_valid) 
			|| !is_valid)
		{
			if (SWPAR_OK != _eeprom_init(DEVID_IIC_EEPROM, file_id))
			{
				SWPA_FILE_PRINT("Err: failed to restore eeprom info table ![%d]\n", SWPAR_FAIL);
				return SWPAR_FAIL;
			}
		}

		unsigned int master_table_addr = IIC_EEPROM_FILEINFO_SEG_ADDR + IIC_EEPROM_FILEINFO_LEN * file_id;
		unsigned int backup_table_addr = IIC_EEPROM_FILEINFO_SEG_ADDR + IIC_EEPROM_FILEINFO_SEG_LEN / 2 + IIC_EEPROM_FILEINFO_LEN * file_id;

		unsigned int filecount = EEPROM_SEG_NUM;
		unsigned int crc32 = 0;
		unsigned int buf_size = IIC_EEPROM_FILEINFO_LEN;//sizeof(filecount) + sizeof(crc32) + sizeof(info);//


		_LOCK_FILE_INFO();
		
		swpa_memcpy(&info, g_rgbFileInfo[file_id]+sizeof(crc32) + sizeof(filecount), sizeof(info));	
		info.len = realsize;
		printf("Info: saved filesize info[%d].len = %#x\n", file_id, info.len);
		swpa_memcpy(g_rgbFileInfo[file_id]+sizeof(crc32) + sizeof(filecount), &info, sizeof(info));	
		crc32 = swpa_utils_calc_crc32(0, (unsigned char*)g_rgbFileInfo[file_id] + sizeof(crc32), buf_size-sizeof(crc32));
		swpa_memcpy(g_rgbFileInfo[file_id], &crc32, sizeof(crc32));

		unsigned int size = buf_size;
		ret = drv_eeprom_write(dev_id, 
			master_table_addr,
			(unsigned char*)g_rgbFileInfo[file_id],
			&size);
		if (0 != ret || size != buf_size)
		{
			SWPA_FILE_PRINT("Err: failed to write file master (id = %u) info [%d]\n", file_id, SWPAR_FAIL);
			_UNLOCK_FILE_INFO();
			return SWPAR_FAIL;
		}

		size = buf_size;
		ret = drv_eeprom_write(dev_id, 
			backup_table_addr,
			(unsigned char*)g_rgbFileInfo[file_id],
			&size);
		if (0 != ret || size != buf_size)
		{
			SWPA_FILE_PRINT("Err: failed to write file backup (id = %u) info [%d]\n", file_id, SWPAR_FAIL);
			_UNLOCK_FILE_INFO();
			return SWPAR_FAIL;
		}

		_UNLOCK_FILE_INFO();
		return SWPAR_OK;
	}		
	else
	{
		SWPA_FILE_PRINT("Err: dev_id = %u is invalid [%d]\n", dev_id, SWPAR_INVALIDARG);
		return SWPAR_INVALIDARG;
	}
}



/**
* @brief 打开eeprom文件,主要是初始化SWPA_FILEHEADER结构体
*
* 
* @param [in] filename : EEPROM设备上文件名
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
int swpa_eeprom_file_open(
	const char *filename, 
	const char *mode
)
{
	int ret = 0;
	SWPA_FILEHEADER * pheader = NULL;
	EEPROM_FILE_INFO * pinfo = NULL;
	int dev_id = 0;
	char segname[64] = {0};
	int i;
		
	//参数有效性检查
	SWPA_FILE_CHECK(NULL != filename);    	//filename指针非空
	SWPA_FILE_CHECK(0 != swpa_strcmp(filename, ""));//filename非空字符串
	SWPA_FILE_CHECK(NULL != mode);			//mode指针非空
	SWPA_FILE_CHECK(0 != swpa_strcmp(mode, ""));	//mode非空字符串

	//打印参数
	SWPA_FILE_PRINT("filename=%s\n", filename);
	SWPA_FILE_PRINT("mode=%s\n", mode);

	//获取设备ID并保存到device_param
	swpa_sscanf(filename, "%d/%s", &dev_id, segname);

	if (0 == dev_id)
	{
		dev_id = DEVID_IIC_EEPROM;
	}
	else if (1 == dev_id)
	{
		dev_id = DEVID_SPI_EEPROM;
	}
	else
	{
		return SWPAR_INVALIDARG;
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
		return SWPAR_FAIL;
	}
	
	pinfo = (EEPROM_FILE_INFO *)swpa_mem_alloc(sizeof(*pinfo));
	if (NULL == pinfo)
	{
		ret = SWPAR_OUTOFMEMORY;
	    SWPA_FILE_PRINT("Err: No enough memory for pinfo! [%d]\n", ret);
		
		goto _ERR_HANDLING;
	}


	ret = swpa_file_parse_mode(pheader, mode);
	if (SWPAR_OK != ret)
	{
		SWPA_FILE_PRINT("Err: swpa_file_parse_mode() failed! [%d]\n", ret);		
		goto _ERR_HANDLING; 
	}

	
	for (i=0; i<EEPROM_SEG_NUM; i++)
	{
		if (0==swpa_strncmp(_eeprom_seg_info[i].name, segname, swpa_strlen(_eeprom_seg_info[i].name))
			&& dev_id == _eeprom_seg_info[i].dev_id)
		{
			//EEPROM 
			if (SWPA_FILE_IS_APPEND_MODE(pheader) ||
				SWPA_FILE_IS_WRITE_MODE(pheader))
			{
				if (!(PERMISSION_WRITE & _eeprom_seg_info[i].permission))
				{
					ret = SWPAR_INVALIDARG;
					SWPA_FILE_PRINT("Err: this file is not permitted to write! [%d]\n", ret);		
					goto _ERR_HANDLING; 
				}
			}

			
			int is_valid = 0;
			if (SWPAR_OK != _eeprom_check(dev_id, i, &is_valid) 
				|| !is_valid)
			{
				if (SWPAR_OK != _eeprom_init(dev_id, i))
				{
					ret = SWPAR_FAIL;
					SWPA_FILE_PRINT("Err: failed to init eeprom! [%d]\n", ret);
					return ret;
				}
			}

			if (SWPA_FILE_IS_WRITE_MODE(pheader)
				&& !SWPA_FILE_IS_READ_MODE(pheader)
				&& !SWPA_FILE_IS_APPEND_MODE(pheader))
			{
				pinfo->len = 0;
			}
			else
			{
				_eeprom_get_file_size(dev_id, i, &pinfo->len);
			}
			
			pinfo->offset 	= (SWPA_FILE_IS_APPEND_MODE(pheader)) ? pinfo->len : 0;			
			pinfo->addr 	= _eeprom_seg_info[i].addr;					
			pinfo->dev_id 	= dev_id;
			pinfo->seg_len	= _eeprom_seg_info[i].len;
			pinfo->file_id	= i;
			
			break;
		}
	}
	
	//没有找到对应的段名，报错返回
	if (EEPROM_SEG_NUM <= i)
	{
		ret = SWPAR_INVALIDARG;
		SWPA_FILE_PRINT("Err: %s not found! [%d]\n", filename, ret);		
		
		goto _ERR_HANDLING;
	}

	pheader->device_param = (int)pinfo;			
	pheader->file_open 	= swpa_eeprom_file_open;
	pheader->file_close = swpa_eeprom_file_close;
	pheader->file_seek 	= swpa_eeprom_file_seek;
	pheader->file_eof 	= swpa_eeprom_file_eof;
	pheader->file_tell 	= swpa_eeprom_file_tell;
	pheader->file_ioctl = swpa_eeprom_file_ioctl;
	pheader->file_read 	= swpa_eeprom_file_read;
	pheader->file_write	= swpa_eeprom_file_write;

	
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

	if (NULL != pinfo)
	{
		swpa_mem_free(pinfo);
		pinfo = NULL;
	}

	
	
	return ret;
	
}




/**
* @brief 关闭eeprom设备文件
*
* 
* @param [in] fd 文件描述符
* @retval SWPAR_OK :  成功
*/
int swpa_eeprom_file_close(
	int fd
)
{
	int ret = SWPAR_OK;
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	EEPROM_FILE_INFO * pinfo = NULL;
	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);    	//fd 必须不等于0
	

	//参数打印
	SWPA_FILE_PRINT("fd=%d\n", fd);	
	

	SWPA_FILE_CHECK(0 != pheader->device_param);
	
	pinfo = (EEPROM_FILE_INFO *)(pheader->device_param);
	SWPA_FILE_CHECK(NULL != pinfo);		//pinfo非空

	//关闭设备
	if (SWPA_FILE_IS_WRITE_MODE(pheader) ||
		SWPA_FILE_IS_APPEND_MODE(pheader))
	{
		_eeprom_save_file_size(pinfo->dev_id, pinfo->file_id, pinfo->len);
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

	SWPA_FILE_CHECK_RET(swpa_mem_free((void*)pinfo), SWPAR_FAIL);
	pheader->device_param = (int)NULL;

	SWPA_FILE_CHECK_RET(swpa_mem_free(pheader), SWPAR_FAIL);
	pheader = NULL;

	return ret;
}




/**
* @brief 读eeprom文件数据
*
* 
* @param [in] fd 文件描述符
* @param [in] buf 读数据缓冲区
* @param [in] size 缓冲区大小
* @param [out] ret_size 返回读到的数据大小
* @retval SWPAR_OK :  成功
* @retval SWPAR_FAIL :  失败
*/
int swpa_eeprom_file_read(
	int fd, 
	void* buf, 
	int size, 
	int* ret_size
)
{
	int ret = SWPAR_OK;
	unsigned int bytes = 0;
	EEPROM_FILE_INFO * pinfo = NULL;
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
	

	pinfo = (EEPROM_FILE_INFO *)(pheader->device_param);
	SWPA_FILE_CHECK(NULL != pinfo);		//pinfo非空


	if (!SWPA_FILE_IS_READ_MODE(pheader))
	{
		SWPA_FILE_PRINT("Err: file is not in read mode![%d]\n", SWPAR_FAIL);
		return SWPAR_FAIL;
	}
	
	//do read
	bytes = (unsigned int)size;
	if (pinfo->offset+size >= pinfo->seg_len)
	{
		bytes = pinfo->seg_len - pinfo->offset;
	}	
	ret = drv_eeprom_read(pinfo->dev_id, pinfo->addr + pinfo->offset, (unsigned char *)buf, &bytes);
	if (0 != ret) 
	{
		SWPA_FILE_PRINT("Err: drv_eeprom_read() failed: %d ![%d]\n", ret, SWPAR_FAIL);
		return SWPAR_FAIL;
	}

	//调整文件内部读写指针
	pinfo->offset += bytes;
	pinfo->offset %= (pinfo->seg_len + 1);

	if (NULL != ret_size)
	{
		*ret_size = bytes;
	}
	
	return ret;
}




/**
* @brief 写eeprom文件数据
*
* 
* @param [in] fd 文件描述符
* @param [in] buf 写数据缓冲区
* @param [in] size 缓冲区大小
* @param [out] ret_size 返回写入的数据大小
* @retval SWPAR_OK :  成功
* @retval SWPAR_FAIL :  失败
*/
int swpa_eeprom_file_write(
	int fd, 
	void* buf, 
	int size, 
	int* ret_size
)
{
	int ret = SWPAR_OK;
	unsigned int bytes = 0;
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	EEPROM_FILE_INFO * pinfo = NULL;
	
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
	

	pinfo = (EEPROM_FILE_INFO *)pheader->device_param;
	SWPA_FILE_CHECK(NULL != pinfo);		//pinfo非空


	if (!SWPA_FILE_IS_WRITE_MODE(pheader) &&
		!SWPA_FILE_IS_APPEND_MODE(pheader))
	{
		SWPA_FILE_PRINT("Err: file is not in write mode![%d]\n", SWPAR_FAIL);
		return SWPAR_FAIL;
	}

	
	//do write
	bytes = (unsigned int)size;
	if (pinfo->offset+size >= pinfo->seg_len)
	{
		bytes = pinfo->seg_len - pinfo->offset;
	}
	ret = drv_eeprom_write(
		pinfo->dev_id, 
		pinfo->addr + pinfo->offset, 
		(unsigned char *)buf, 
		&bytes
	);
	if (0 != ret) 
	{
		SWPA_FILE_PRINT("Err: drv_eeprom_write() failed: %d ![%d]\n", ret, SWPAR_FAIL);
		return SWPAR_FAIL;
	}

	//调整文件内部读写指针
	pinfo->offset += bytes;
	pinfo->offset %= (pinfo->seg_len+1);

	if (SWPA_FILE_IS_APPEND_MODE(pheader))
	{
		if (pinfo->offset > pinfo->len)
		{
			pinfo->len = pinfo->offset;
		}
	}
	else
	{
		pinfo->len = pinfo->offset;
	}

	if (NULL != ret_size)
	{
		*ret_size = bytes;
	}
	
	return ret;
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
* @retval SWPAR_INVALIDARG : 参数非法
*/
int swpa_eeprom_file_seek(
	int fd, 
	int offset, 
	int pos
)
{
	int ret = SWPAR_OK;
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	EEPROM_FILE_INFO * pinfo = NULL;
	
	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);		// fd 必须不等于0
	//SWPA_FILE_CHECK(0 != offset);		//offset可以取任意值
	SWPA_FILE_CHECK(SWPA_SEEK_SET == pos || SWPA_SEEK_CUR == pos || SWPA_SEEK_END == pos);		//pos

	//参数打印
	SWPA_FILE_PRINT("fd=%d\n", fd);
	SWPA_FILE_PRINT("offset=%d\n", offset);
	SWPA_FILE_PRINT("pos=%d\n", pos);

	pinfo = (EEPROM_FILE_INFO *) pheader->device_param;

	switch (pos)
	{
	case SWPA_SEEK_SET:
		if (offset>=0 && (unsigned int)offset <= pinfo->len)
		{
			pinfo->offset = offset;
		}
		else
		{
			SWPA_FILE_PRINT("Err: offset(%d) out of range! (file len = %d)\n", offset, pinfo->len);
			return SWPAR_FAIL;
		}
	break;

	case SWPA_SEEK_CUR:
		if (pinfo->offset+offset >= 0 && pinfo->offset+offset <= pinfo->len)
		{
			pinfo->offset += offset;
		}
		else
		{
			SWPA_FILE_PRINT("Err: offset(%d) out of range! (file len = %d)\n", offset, pinfo->len);
			return SWPAR_FAIL;
		}
	break;

	case SWPA_SEEK_END:
		if (offset<=0 && pinfo->len + offset >= 0)
		{
			pinfo->offset = pinfo->len + offset;
		}
		else
		{
			SWPA_FILE_PRINT("Err: offset(%d) out of range! (file len = %d)\n", offset, pinfo->len);
			return SWPAR_FAIL;
		}
	break;
	
		
	default:
		return SWPAR_INVALIDARG;
	}
	
	
	return ret;
}


/**
* @brief 判断是否到了文件尾部
* @param [in] fd 文件描述符
* @retval 0 : 没有到文件尾部，
* @retval -1 : 到了文件尾部
* @attention 这个函数的返回值比较特殊，请区别对待
*/
int swpa_eeprom_file_eof(
	int fd
)
{
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	EEPROM_FILE_INFO * pinfo = NULL;
	
	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);		// fd 必须不等于0

	//参数打印
	SWPA_FILE_PRINT("fd=0x%08x\n", fd);

	pinfo = (EEPROM_FILE_INFO *) pheader->device_param;
	
	return (pinfo->offset == pinfo->len) ? (-1) : (0);
}



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
int swpa_eeprom_file_tell(
	int fd
)
{
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	EEPROM_FILE_INFO * pinfo = NULL;
	
	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);		// fd 必须不等于0

	//参数打印
	SWPA_FILE_PRINT("fd=0x%08x\n", fd);

	pinfo = (EEPROM_FILE_INFO *) pheader->device_param;

	return (pinfo->offset < 0 || pinfo->offset > pinfo->len) ? (SWPAR_FAIL) : (pinfo->offset);
}




/**
* @brief 对文件描述符的控制
*
*
* 目前仅支持获取文件最大size
* 
* @param [in] fd 文件描述符
* @param [in] cmd 命令标识，目前已定义的命令有
* - SWPA_FILE_IOCTL_GET_MAX_SIZE : 获取文件最大size
* @param [in] args 命令标志参数
* @retval SWPAR_OK : 成功
* @retval SWPAR_FAIL : 失败
*/
int swpa_eeprom_file_ioctl(
	int fd, 
	int cmd, 
	void* args
)
{
	SWPA_FILEHEADER * pheader = (SWPA_FILEHEADER *)fd;
	EEPROM_FILE_INFO * pinfo = (EEPROM_FILE_INFO *)pheader->device_param;	

	
	//参数有效性检查
	SWPA_FILE_CHECK(0 != fd);    	//fd 必须不等于0
	//SWPA_CHECK(0 < cmd);    	//cmd取值没有限定，故mark掉
	//SWPA_CHECK(NULL != args);    	//args 在不同cmd下可以为NULL，故mark掉

	//参数打印
	SWPA_FILE_PRINT("fd=%d\n", fd);	
	SWPA_FILE_PRINT("cmd=%d\n", cmd);	
	SWPA_FILE_PRINT("args=0x%08x\n", (unsigned int)args);
	
	switch (cmd)
	{
		case SWPA_FILE_IOCTL_GET_MAX_SIZE:
		{
			unsigned int *psize = (unsigned int *)args;
			if (NULL == psize)
			{
				SWPA_FILE_PRINT("Err: invalid cmd arg (NULL) ! [%d]\n", SWPAR_INVALIDARG);
				return SWPAR_INVALIDARG;
			}
			
			*psize = pinfo->seg_len;
		}
		break;
		
		default:
			SWPA_FILE_PRINT("Err: invalid cmd for eeprom file! [%d]\n", SWPAR_INVALIDARG);
			return SWPAR_INVALIDARG;
	}

	return SWPAR_OK;
	
}



