#include "swpa.h"
#include "swpa_private.h"
#include "swpa_utils.h"
#include "drv_log.h"
#include "swpa_command.h"
#include <errno.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>


#ifdef SWPA_UTILS
#define SWPA_UTILS_PRINT(fmt, ...) SWPA_PRINT("[%s:%d]"fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define SWPA_UTILS_CHECK(arg)      {if (!(arg)){SWPA_PRINT("[%s:%d]Check failed : %s [%d]\n", __FILE__, __LINE__, #arg, SWPAR_INVALIDARG);return SWPAR_INVALIDARG;}}
#else
#define SWPA_UTILS_PRINT(fmt, ...)
#define SWPA_UTILS_CHECK(arg)
#endif


typedef struct tag_DLL
{
    void* handle;
}
DLL_HANDLE;

typedef struct tga_DIR
{
    DIR* dir;
    char* szFilter;
}DIR_HANDLE;

/**
 *@brief 执行shell脚本命令
 *@param [in] szCmd 命令字符串，shell脚本
 *@param [in] ... 变参参数,shell脚本中具体的函数名称
 */
int swpa_utils_shell(const char* szCmd, ...)
{
	SWPA_UTILS_CHECK(szCmd != NULL);
	
	TiXmlDocument xmlDoc;
	xmlDoc.LinkEndChild(new TiXmlDeclaration("1.0", "GB2312", "yes"));
	TiXmlElement* pRoot = new TiXmlElement("LprApp");	
	TiXmlElement* pElement = new TiXmlElement("Command");	
	pElement->SetAttribute("id", szCmd);
		
	char szTmp[255] = "";
	swpa_va_list marker;
	swpa_va_start(marker, szCmd);
	int i = 0;
	while(NULL != szCmd)
	{
		if(0 != i)
		{
			swpa_sprintf(szTmp + swpa_strlen(szTmp), " %s", szCmd);
		}
		szCmd = swpa_va_arg(marker, const char *);
		i++;
	}
	swpa_va_end(marker);	
	
	if(swpa_strlen(szTmp) > 0)
	{
		pElement->SetAttribute("param", szTmp);
	}
	pElement->SetAttribute("shell", "./command.sh");
	pRoot->LinkEndChild(pElement);
	xmlDoc.LinkEndChild(pRoot);
	
	if(SWPAR_OK == exec_command(xmlDoc))
	{
		return SWPAR_OK;
	}
	return SWPAR_FAIL;
}
/**
* @brief 打开动态链接库
* @param [in] filename 动态链接库文件名
* @param [out] dll 动态链接库结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dll_open(const char* filename, int* dll)
{
    SWPA_UTILS_CHECK(filename != NULL);
	SWPA_UTILS_CHECK(dll != NULL);
	SWPA_UTILS_PRINT("filename=%s\n", filename);
	SWPA_UTILS_PRINT("dll=0x%08x\n", (unsigned int)dll);
    DLL_HANDLE* handle = (DLL_HANDLE *)swpa_mem_alloc(sizeof(DLL_HANDLE));
    handle->handle = dlopen(filename, RTLD_LAZY);
    if(handle->handle == NULL)
    {
        return SWPAR_FAIL;
    }
    (*dll) = (int)handle;
    return SWPAR_OK;
}

/**
* @brief 获取动态链接库中提供的函数
* @param [in] dll 动态链接库结构体
* @param [in] funcname 要获取的函数的名称
* @param [out] func 获取到的函数指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dll_func(int dll, const char* funcname, void** func)
{
	SWPA_UTILS_CHECK(dll != 0 && funcname != NULL && func != NULL);
	SWPA_UTILS_PRINT("dll=%d\n", dll);
	SWPA_UTILS_PRINT("funcname=0x%08x\n", (unsigned int)funcname);
	SWPA_UTILS_PRINT("*func=0x%08x\n", (unsigned int)*func);
    DLL_HANDLE* handle = (DLL_HANDLE*)(dll);
    *func = dlsym(handle->handle, funcname);
    if(*func == NULL)
    {
        return SWPAR_FAIL;
    }
    return SWPAR_OK;
}

/**
* @brief 关闭动态链接库
* @param [in] dll 动态链接库结构体
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dll_close(int* dll)
{
    SWPA_UTILS_CHECK(dll != NULL);
	  SWPA_UTILS_PRINT("dll=0x%08x\n", (unsigned int)dll);
    DLL_HANDLE* handle = (DLL_HANDLE*)(*dll);
    int iret = -1;
    iret = dlclose(handle->handle);
    if(iret < 0)
    {
        return SWPAR_FAIL;
    }
    swpa_mem_free(dll);
    dll = NULL;
    return SWPAR_OK;
}

/**
* @brief 获取最新错误码
* @retval 最新错误码
* @see swpa_utils.h
*/
int swpa_utils_sys_getlasterr(void)
{
   return SWPAR_NOTIMPL;
}

/**
* @brief 设置最新错误码
* @param [in] err 具体错误码
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_sys_setlasterr(int err)
{
    return SWPAR_NOTIMPL;
}

/**
* @brief 获取最新错误信息
* @param [out] err_str 错误信息缓存区
* @param [in] len 错误信息缓存区长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_sys_getlasterrstr(char* err_str, int len)
{
    return SWPAR_NOTIMPL;
}

/**
* @brief 设置最新错误信息
* @param [out] err_str 错误信息
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_sys_setlasterrstr(const char* err_str)
{
    return SWPAR_NOTIMPL;
}

/**
* @brief 创建目录
* @param [in] pathname 目录全名，需包含绝对路径。
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dir_create(const char* pathname)
{
    SWPA_UTILS_CHECK(pathname != NULL);
	SWPA_UTILS_PRINT("pathname=%s\n", pathname);
	int iret = -1;
	iret = mkdir(pathname, S_IRWXU);
	if(iret < 0)
	{
	    return SWPAR_FAIL;

	}
    return SWPAR_OK;
}

/**
* @brief 删除目录
* @param [in] pathname 目录全名，需包含绝对路径。
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dir_remove(const char* pathname)
{
    SWPA_UTILS_CHECK(pathname != NULL);
	SWPA_UTILS_PRINT("pathname=%s\n", pathname);
	int iret = -1;
	iret = rmdir(pathname);
	if(iret < 0)
	{
	    return SWPAR_FAIL;
	}
    return SWPAR_OK;
}

/**
* @brief 判断目录是否存在
* @param [in] pathname 目录全名，需包含绝对路径。
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dir_exist(const char* pathname)
{
    SWPA_UTILS_CHECK(pathname != NULL);
	SWPA_UTILS_PRINT("pathname=%s\n", pathname);
	int iret = -1;
	struct stat file_info;
	iret = stat(pathname, &file_info);
	if(iret < 0)
	{
	    return SWPAR_FAIL;
	}
	if(S_ISDIR(file_info.st_mode))
	{
	    return SWPAR_OK;
	}
    return SWPAR_FAIL;
}

/**
* @brief 获得目录占用的空间大小
* @param [in] pathname 目录全名，需包含绝对路径。
* @retval 空间大小
* @see swpa_utils.h
*/
int swpa_utils_dir_getsize(const char* pathname)
{
    SWPA_UTILS_CHECK(pathname != NULL);
	SWPA_UTILS_PRINT("pathname=%s\n", pathname);
	int iret = -1;
    struct stat file_info;
	iret = stat(pathname, &file_info);
	if(iret < 0)
	{
	    return SWPAR_FAIL;
	}
	if(S_ISDIR(file_info.st_mode))
	{
        return file_info.st_size;
	}
    return SWPAR_FAIL;
}

/**
* @brief 给目录创建快捷方式
* @param [in] dst 目地目录
* @param [in] src 源目录
* @param [in] mode 链接方式。0：硬连接；1：软连接。
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dir_link(const char* dst, const char* src, int mode)
{
    SWPA_UTILS_CHECK(dst != NULL);
    SWPA_UTILS_CHECK(src != NULL);
	SWPA_UTILS_PRINT("dst=%s\n", dst);
	SWPA_UTILS_PRINT("src=%s\n", src);
	SWPA_UTILS_PRINT("mode=%d\n", mode);
	int iret = -1;
	if(mode == 1)
	{
	    iret = symlink(src, dst);
	}
	else
	{
	    iret = link(src, dst);
	}
	if(iret < 0)
	{
	    return SWPAR_FAIL;
	}
    return SWPAR_OK;
}

/**
* @brief 打开目录
* @param [in] pathname 目地目录
* @param [in] filter 过滤的文件名，参考正则表达式，只有满足正则表达式的文件或目录才会读取出来，为NULL则读取所有的文件或目录。
* @retval 目录句柄。-1：失败；其它：成功。
* @see swpa_utils.h
*/
int swpa_utils_dir_open(const char* pathname, const char* filter)
{
    SWPA_UTILS_CHECK(pathname != NULL);
	SWPA_UTILS_PRINT("pathname = %s\n", pathname);
	SWPA_UTILS_PRINT("filter=%s\n", filter);
    DIR_HANDLE* handle = (DIR_HANDLE *)swpa_mem_alloc(sizeof(DIR_HANDLE));
    handle->dir = opendir(pathname);
    if(handle->dir == NULL)
    {
    	swpa_mem_free(handle);
        return SWPAR_FAIL;
    }
    if(filter != NULL)
    {
        int filter_len = (int)strlen(filter);
        handle->szFilter = (char *)swpa_mem_alloc(filter_len+1);
        memcpy(handle->szFilter, filter, filter_len);
        handle->szFilter[filter_len] = '\0';
    }
    else
    {
        handle->szFilter = NULL;
    }
    return (int)handle;
}

/**
* @brief 浏览目录
* @param [in] handle 目录句柄
* @param [out] name 遍历这个目录得到的文件或目录名
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dir_next(int handle, char* name)
{
    SWPA_UTILS_CHECK(handle != 0);
    SWPA_UTILS_CHECK(name != NULL);
	SWPA_UTILS_PRINT("handle=%d\n", handle);
	SWPA_UTILS_PRINT("name=%s\n", name);
	DIR_HANDLE* dir_handle = (DIR_HANDLE*)(handle);
	struct dirent* dirp = NULL;
    while((dirp = readdir(dir_handle->dir)) != NULL)
    {
        if(strcmp(dirp->d_name, ".")==0 || strcmp(dirp->d_name, "..")==0)
        continue;
        strcpy(name, dirp->d_name);
        return SWPAR_OK;
    }
    return SWPAR_FAIL;
}

/**
* @brief 关闭目录
* @param [in] handle 目录句柄
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dir_close(int handle)
{
    SWPA_UTILS_CHECK(handle != 0);
	SWPA_UTILS_PRINT("handle=%d\n", handle);
	DIR_HANDLE* dir_handle = (DIR_HANDLE*)(handle);
    closedir(dir_handle->dir);
    if(dir_handle->szFilter)
    {
        swpa_mem_free(dir_handle->szFilter);
        dir_handle->szFilter = NULL;
    }
	swpa_mem_free((DIR_HANDLE*)handle);
    return SWPAR_OK;
}

/**
* @brief 创建文件
* @param [in] filename 文件名
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_file_create(const char* filename)
{
    SWPA_UTILS_CHECK(filename != NULL);
	SWPA_UTILS_PRINT("filename=%s\n", filename);
	int iret = -1;
	iret = creat(filename, O_RDWR | O_CREAT | O_TRUNC);
	if(iret < 0)
	{
	    return SWPAR_FAIL;
	}
	close(iret);
    return SWPAR_OK;
}

/**
* @brief 删除文件
* @param [in] filename 文件名
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_file_delete(const char* filename)
{
    SWPA_UTILS_CHECK(filename != NULL);
	SWPA_UTILS_PRINT("filename=%s\n", filename);
	int iret = -1;
    iret = remove(filename);
    if(iret < 0)
	{
	    return SWPAR_FAIL;
	}
    return SWPAR_OK;
}

/**
* @brief 获取文件大小
* @param [in] filename 文件名
* @retval 文件大小。-1：失败；其它：成功。
* @see swpa_utils.h
*/
int swpa_utils_file_getsize(const char* filename)
{
    SWPA_UTILS_CHECK(filename != NULL);
	SWPA_UTILS_PRINT("filename=%s\n", filename);
	int iret = -1;
	struct stat statbuff;
    iret = stat(filename, &statbuff);
    if(iret < 0)
	{
	    return SWPAR_FAIL;
	}
    return statbuff.st_size;
}

/**
* @brief 重命名文件
* @param [in] filename 文件名
* @param [in] newname 新文件名
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_file_rename(const char* filename, const char* newname)
{
    SWPA_UTILS_CHECK(filename != NULL);
    SWPA_UTILS_CHECK(newname != NULL);
	SWPA_UTILS_PRINT("filename=%s\n", filename);
	SWPA_UTILS_PRINT("newname=%s\n", newname);
    int iret = -1;
    iret = rename(filename, newname);
    if(iret < 0)
	{
	    return SWPAR_FAIL;
	}
    return SWPAR_OK;
}

/**
* @brief 复制文件
* @param [in] dst 目地文件
* @param [in] src 源文件
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_file_copy(const char* dst, const char* src)
{
    SWPA_UTILS_CHECK(dst != NULL);
    SWPA_UTILS_CHECK(src != NULL);
	SWPA_UTILS_PRINT("dst=%s\n", dst);
	SWPA_UTILS_PRINT("src=%s\n", src);
    int src_fd, dst_fd;
    int byte_read, byte_write;
    src_fd = open(src, O_RDONLY);
    if(src_fd < 0)
    {
        return SWPAR_FAIL;
    }
    dst_fd = open(dst, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
    if(dst_fd < 0)
    {
        close(src_fd);
        return SWPAR_FAIL;
    }
    char buffer[1024] = {0};
    char* ptr;
    while(byte_read = read(src_fd, buffer, 1024))
    {
        if((byte_read == -1) && (errno != EINTR))
        {
            close(dst_fd);
            close(src_fd);
            return SWPAR_FAIL;
        }
        if(byte_read > 0)
        {
            ptr = buffer;
            while(byte_write = write(dst_fd, ptr, byte_read))
            {
                if((byte_write == -1) && (errno != EINTR))
                {
                    close(dst_fd);
                    close(src_fd);
                    return SWPAR_FAIL;
                }
                else if(byte_write == byte_read)
                {
                    break;
                }
                else if(byte_write > 0)
                {
                    ptr += byte_write;
                    byte_read -= byte_write;
                }
            }
        }
    }
    close(dst_fd);
    close(src_fd);
    return SWPAR_OK;
}

/**
* @brief 移动文件
* @param [in] dst 目地文件
* @param [in] src 源文件
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_file_move(const char* dst, const char* src)
{
    int iret = -1;
    iret = swpa_utils_file_rename(dst, src);
    if(iret < 0)
    {
        return SWPAR_FAIL;
    }
    return SWPAR_OK;
}

/**
* @brief 判断文件是否存在
* @param [in] filename 文件名
* @retval 0 文件不存在
* @retval 1 文件存在
* @see swpa_utils.h
*/
int swpa_utils_file_exist(const char* filename)
{
  SWPA_UTILS_CHECK(filename != NULL);
	SWPA_UTILS_PRINT("filename=%s\n", filename);
	return (0 == access(filename, 0));
}

/**
* @brief 给文件创建快捷方式
* @param [in] dst 目地文件
* @param [in] src 源文件
* @param [in] mode 链接方式。0：硬连接；1：软连接。
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_file_link(const char* dst, const char* src, int mode)
{
    return swpa_utils_dir_link(dst, src, mode);
}



/**
* @brief 获取文件路径字符串中的文件夹名
* @param [in] path 路径字符串
* @param [out] dir 文件夹字符串
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_path_getdirname(const char* path, char* dir)
{
    SWPA_UTILS_CHECK(path != NULL);
	SWPA_UTILS_CHECK(dir  != NULL);
	SWPA_UTILS_PRINT("path=%s\n", path);
	SWPA_UTILS_PRINT("dir=%s\n", dir);
	
	//int iret = -1;
	int pathlen = swpa_strlen(path);

	while (pathlen--)
	{
		if (0 == swpa_strncmp("/", path+pathlen, 1))
		{
			swpa_strncpy(dir, path, pathlen+1);
			return SWPAR_OK;
		}
	}
	
	swpa_strcpy(dir, "");
	return SWPAR_FAIL;
}


/**
* @brief 获取文件路径字符串中的文件名
* @param [in] path 路径字符串
* @param [out] filename 文件名字符串
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_path_getfilename(const char* path, char* filename)
{
    SWPA_UTILS_CHECK(path != NULL);
	SWPA_UTILS_CHECK(filename  != NULL);
	SWPA_UTILS_PRINT("path=%s\n", path);
	SWPA_UTILS_PRINT("filename=%s\n", filename);
	
	//int iret = -1;
	int pathlen = swpa_strlen(path);

	int i = pathlen;
	while (i--)
	{
		if (0 == swpa_strncmp("/", path+i, 1))
		{
			if (i == pathlen-1)
			{
				break;
			}
			
			swpa_strncpy(filename, path+i+1, pathlen-i);
			return SWPAR_OK;
		}
	}
	
	swpa_strcpy(filename, "");
	return SWPAR_FAIL;
}


/**
* @brief 获取文件路径字符串中的文件扩展名
* @param [in] path 路径字符串
* @param [out] ext_name 文件扩展名字符串
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_path_getextensionname(const char* path, char* ext_name)
{
    SWPA_UTILS_CHECK(path != NULL);
	SWPA_UTILS_CHECK(ext_name  != NULL);
	SWPA_UTILS_PRINT("path=%s\n", path);
	SWPA_UTILS_PRINT("ext_name=%s\n", ext_name);
	
	//int iret = -1;
	int pathlen = swpa_strlen(path);

	int i = pathlen;
	while (i--)
	{
		if (0 == swpa_strncmp(".", path+i, 1))
		{
			swpa_strncpy(ext_name, path+i+1, pathlen-i);
			return SWPAR_OK;
		}
	}
	
	swpa_strcpy(ext_name, "");
	return SWPAR_FAIL;
}



/**
* @brief log输出函数
* @param [in] src : 输出日志的模块
* @parea [in] level : 日志等级
* @param [in] fmt : 格式字符串
* @param [in] ... : 变参
* @retval 实际输出的参数个数
* @see swpa_utils.h
*/
int swpa_print(const char * src, const int level, char * fmt, ...)
{
	va_list pva;
	va_start(pva, fmt);
	swpa_vprint(src, level, fmt, pva);
	va_end(pva);
	return 0;
}

int swpa_vprint(const char * src, const int level, char * fmt, swpa_va_list va_args)
{
#define MSG_LEN 512
  char msg[MSG_LEN] = {0};    	
	vsnprintf(msg, MSG_LEN-1, fmt, va_args);
	drv_log(src, level, "%s", msg);	
	return 0;
}


static unsigned int crc_table[256] =
{
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419,
	0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4,
	0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07,
	0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856,
	0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4,
	0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3,
	0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a,
	0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599,
	0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190,
	0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f,
	0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e,
	0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed,
	0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3,
	0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a,
	0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5,
	0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010,
	0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17,
	0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6,
	0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615,
	0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344,
	0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a,
	0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1,
	0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c,
	0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef,
	0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe,
	0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31,
	0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c,
	0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b,
	0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1,
	0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278,
	0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7,
	0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66,
	0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605,
	0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8,
	0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b,
	0x2d02ef8d
};

#define DO1 crc32 = crc_table[((int)crc32 ^ (*buf++)) & 0xff] ^ (crc32 >> 8)

#define DO8 DO1; DO1; DO1; DO1; DO1; DO1; DO1; DO1

/**
* @brief 计算CRC32校验
* @param [in] crc32 : crc32初始值
* @parea [in] buf : 数据缓存
* @param [in] len : 数据缓存长度
* @retval 计算出的校验值
* @see swpa_utils.h
*/
unsigned int swpa_utils_calc_crc32(unsigned int crc32, unsigned char* buf, unsigned int len)
{
	if (buf == 0) return 0;

	crc32 = crc32 ^ 0xffffffff;

	while (len >= 8)
	{
		DO8;
		len -= 8;
	}

	if (len) do
	{
		DO1;
	}
	while (--len);

	return crc32 ^ 0xffffffff;
}

/**
* @brief 获取主CPU使用率
* @retval CPU的使用率
* @see swpa_utils.h
*/
int swpa_utils_get_cpu_usage(void)
{
    FILE *fp = NULL;
    char caBuff[128];
    char caName[32];
    static unsigned int unLastTotal = 0;
    static unsigned int unLastIdle = 0;
    unsigned int uUser = 0;
    unsigned int uNice = 0;
    unsigned int uSys = 0;
    unsigned int uIdle = 0;

    fp = fopen("/proc/stat", "r");
    if (NULL == fp)
    {
        printf("/proc/stat open error\n");
        return SWPAR_FAIL;
    }

    fgets(caBuff, sizeof(caBuff), fp);
    sscanf(caBuff, "%s %u %u %u %u", caName, &uUser, &uNice,
        &uSys, &uIdle);

    unsigned int uTotalTmp = uUser + uNice + uSys + uIdle;
    unsigned int uTmp = uTotalTmp - unLastTotal;
    unsigned int unCpuOccupy = 0;
    if (uTmp > 0)
    {
        //printf("==================uTotalTmp:%d,unLastTotal:%d\n",uTotalTmp,unLastTotal);
        unCpuOccupy = (uIdle - unLastIdle)*100/uTmp;
    }
    unCpuOccupy = 100 - unCpuOccupy;
    //printf("%s , %u %u %u %u cpu %u%%\n", caName, uUser, uNice, uSys, uIdle, unCpuOccupy);
        
    unLastTotal = uTotalTmp;
    unLastIdle = uIdle;
    
    fclose(fp);
    
    return unCpuOccupy;
}


/**
* @brief 获取主系统内存使用率
* @retval 内存使用率
* @see swpa_utils.h
*/
int swpa_utils_get_mem_usage(void)
{
	FILE *fp = NULL;
    char caBuff[128];
    char caName[32];
    unsigned int uTotalMem = 0, uFreeMem = 0, uBufferMem = 0, uCacheMem = 0;
    fp = fopen("/proc/meminfo", "r");
    if (NULL == fp)
    {
        printf("/proc/meminfo open error\n");
        return SWPAR_FAIL;
    }
    fgets(caBuff, sizeof(caBuff), fp);
    sscanf(caBuff, "%s %u", caName, &uTotalMem);
    //printf("%s uTotalMem %u uFreeMem %u\n",caBuff, uTotalMem, uFreeMem);

    fgets(caBuff, sizeof(caBuff), fp);
    sscanf(caBuff, "%s %u", caName,  &uFreeMem);

	fgets(caBuff, sizeof(caBuff), fp);
    sscanf(caBuff, "%s %u", caName,  &uBufferMem);

	fgets(caBuff, sizeof(caBuff), fp);
    sscanf(caBuff, "%s %u", caName,  &uCacheMem);

    //printf("%s uTotalMem %u uFreeMem %u\n",caBuff, uTotalMem, uFreeMem);

    fclose(fp);

    return (uTotalMem-uFreeMem-uBufferMem-uCacheMem)*100/uTotalMem;
}

int swpa_utils_process_get_id()
{
	return getpid();
}

int swpa_utils_process_name_by_id(unsigned int pid, char* process_name, unsigned int len)
{
	char cmd_f[256] =
	{
		0
	};
	FILE* fcmd = NULL;

	if (pid <= 0)
	{
		return NULL;
	}

	swpa_snprintf(cmd_f, 256, "/proc/%d/cmdline", pid);
	fcmd = fopen(cmd_f, "r");
	if (fcmd)
	{
		char buf[256];
		if (fgets(buf, sizeof(buf), fcmd))
		{
			swpa_strncpy(process_name, buf, len);	
			process_name[len - 1] = 0;
		}
		fclose(fcmd);
	}
	
	return 0;
}

int swpa_utils_process_id_by_name(const char* process_name, unsigned int pid[])
{
	char buf[10];
	char cmd[100];
	int i = 0;
	FILE* p;
	
	swpa_memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "%s\"%s\"%s", "ps -ef|grep -v grep|grep -w ", process_name, "|awk '{print $2}'");
	cmd[swpa_strlen(cmd)] = '\0';
	p = popen(cmd, "r");
	while (1)
	{
		swpa_memset(buf, 0, sizeof(buf));
		fgets(buf, sizeof(buf), p);
		if (swpa_strlen(buf) == 0)
		{
			break;
		}
		pid[i] = swpa_atoi(buf);
		i++;
	}
	pclose(p);
	
	return 0;
}


int swpa_utils_string_ftok(const char* file_name, int model)
{
	return ftok(file_name, model);
}

int swpa_utils_string_strtok(const char* pbuf, const char* szch, const int num)
{
	if (pbuf == NULL || swpa_strlen(pbuf) < 1 || num < 1)
	{
		return 0;
	}

	int ret = 0;
	int cile = 0;
	char* str1 = NULL;
	char* token = NULL;
	char* saveptr1 = NULL;

	int iLen = swpa_strlen(pbuf);
	char* pstr = (char*)swpa_mem_alloc(iLen + 1);
	if (NULL == pstr)
	{
		return -1;
	}
	swpa_strncpy(pstr, pbuf, iLen);

	int j = 1;
	for (j = 1, str1 = pstr; ; j++, str1 = NULL)
	{
		token = strtok_r(str1, szch, &saveptr1);
		if (token == NULL)
		{
			break;
		}
		cile++;
		if (cile == num)
		{
			ret = atoi(token);
			break;
		}
	}

	swpa_mem_free(pstr);

	return ret;
}


