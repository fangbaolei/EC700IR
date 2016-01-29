/**
* @file swpa_utils.h
* @brief 工具函数
* @author Shaorg
* @date 2013-02-26
* @version v0.1
* @note 包含各种工具类函数。
*/

#ifndef _SWPA_UTILS_H_
#define _SWPA_UTILS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define swpa_utils_srand srand
#define swpa_utils_rand  rand

/**
 *@brief 执行shell脚本命令
 *@param [in] szCmd 命令字符串，可以是shell脚本，也可以是其他的命令进程
 *@param [in] ... 变参参数，可以支持多条命令，用NULL作为结束符
 */
int swpa_utils_shell(const char *szCmd, ...);

/**
* @brief 打开动态链接库
* @param [in] filename 动态链接库文件名
* @param [out] dll 动态链接库结构体指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dll_open(const char* filename, int* dll);

/**
* @brief 获取动态链接库中提供的函数
* @param [in] dll 动态链接库结构体
* @param [in] funcname 要获取的函数的名称
* @param [out] func 获取到的函数指针
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dll_func(int dll, const char* funcname, void** func);

/**
* @brief 关闭动态链接库
* @param [in] dll 动态链接库结构体
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dll_close(int* dll);

/**
* @brief 获取最新错误码
* @retval 最新错误码
* @see swpa_utils.h
*/
int swpa_utils_sys_getlasterr(void);

/**
* @brief 设置最新错误码
* @param [in] err 具体错误码
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_sys_setlasterr(int err);

/**
* @brief 获取最新错误信息
* @param [out] err_str 错误信息缓存区
* @param [in] len 错误信息缓存区长度
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_sys_getlasterrstr(char* err_str, int len);

/**
* @brief 设置最新错误信息
* @param [out] err_str 错误信息
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_sys_setlasterrstr(const char* err_str);

/**
* @brief 创建目录
* @param [in] pathname 目录全名，需包含绝对路径。
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dir_create(const char* pathname);

/**
* @brief 删除目录
* @param [in] pathname 目录全名，需包含绝对路径。
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dir_remove(const char* pathname);

/**
* @brief 判断目录是否存在
* @param [in] pathname 目录全名，需包含绝对路径。
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dir_exist(const char* pathname);

/**
* @brief 获得目录占用的空间大小
* @param [in] pathname 目录全名，需包含绝对路径。
* @retval 空间大小
* @see swpa_utils.h
*/
int swpa_utils_dir_getsize(const char* pathname);

/**
* @brief 给目录创建快捷方式
* @param [in] dst 目地目录
* @param [in] src 源目录
* @param [in] mode 链接方式。0：硬连接；1：软连接。
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dir_link(const char* dst, const char* src, int mode);

/**
* @brief 打开目录
* @param [in] pathname 目地目录
* @param [in] filter 过滤的文件名，参考正则表达式，只有满足正则表达式的文件或目录才会读取出来，为NULL则读取所有的文件或目录。
* @retval 目录句柄。-1：失败；其它：成功。
* @see swpa_utils.h
*/
int swpa_utils_dir_open(const char* pathname, const char* filter);

/**
* @brief 浏览目录
* @param [in] handle 目录句柄
* @param [out] name 遍历这个目录得到的文件或目录名
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dir_next(int handle, char* name);

/**
* @brief 关闭目录
* @param [in] handle 目录句柄
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_dir_close(int handle);

/**
* @brief 创建文件
* @param [in] filename 文件名
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_file_create(const char* filename);

/**
* @brief 删除文件
* @param [in] filename 文件名
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_file_delete(const char* filename);

/**
* @brief 获取文件大小
* @param [in] filename 文件名
* @retval 文件大小。-1：失败；其它：成功。
* @see swpa_utils.h
*/
int swpa_utils_file_getsize(const char* filename);

/**
* @brief 重命名文件
* @param [in] filename 文件名
* @param [in] newname 新文件名
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_file_rename(const char* filename, const char* newname);

/**
* @brief 复制文件
* @param [in] dst 目地文件
* @param [in] src 源文件
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_file_copy(const char* dst, const char* src);

/**
* @brief 移动文件
* @param [in] dst 目地文件
* @param [in] src 源文件
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_file_move(const char* dst, const char* src);

/**
* @brief 判断文件是否存在
* @param [in] filename 文件名
* @retval 0 文件不存在
* @retval 1 文件存在
* @see swpa_utils.h
*/
int swpa_utils_file_exist(const char* filename);

/**
* @brief 给文件创建快捷方式
* @param [in] dst 目地文件
* @param [in] src 源文件
* @param [in] mode 链接方式。0：硬连接；1：软连接。
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_file_link(const char* dst, const char* src, int mode);



/**
* @brief 获取文件路径字符串中的文件夹名
* @param [in] path 路径字符串
* @param [out] dir 文件夹字符串
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_path_getdirname(const char* path, char* dir);


/**
* @brief 获取文件路径字符串中的文件名
* @param [in] path 路径字符串
* @param [out] filename 文件名字符串
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_path_getfilename(const char* path, char* filename);


/**
* @brief 获取文件路径字符串中的文件扩展名
* @param [in] path 路径字符串
* @param [out] ext_name 文件扩展名字符串
* @retval 0 成功
* @retval -1 失败
* @see swpa_utils.h
*/
int swpa_utils_path_getextensionname(const char* path, char* extensionname);


/**
* @brief log输出函数
* @param [in] src : 输出日志的模块
* @parea [in] level : 日志等级
* @param [in] fmt : 格式字符串
* @param [in] ... : 变参
* @retval 实际输出的参数个数
* @see swpa_utils.h
*/
int swpa_print(const char * src, const int level, char * fmt, ...);
int swpa_vprint(const char * src, const int level, char * fmt, swpa_va_list va_args);


/**
* @brief 计算CRC32校验
* @param [in] crc32 : crc32初始值
* @parea [in] buf : 数据缓存
* @param [in] len : 数据缓存长度
* @retval 计算出的校验值
* @see swpa_utils.h
*/
unsigned int swpa_utils_calc_crc32(unsigned int crc32, unsigned char* buf, unsigned int len);

/**
* @brief 获取主CPU使用率
* @retval CPU的使用率
* @see swpa_utils.h
*/
int swpa_utils_get_cpu_usage(void);


/**
* @brief 获取主系统内存使用率
* @retval 内存使用率
* @see swpa_utils.h
*/
int swpa_utils_get_mem_usage(void);

/*****************************************************************************
 函 数 名  : swpa_utils_process_get_id
 功能描述  : 返回当前的进程id
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  : 
 注意事项  : 
 
 修改历史      :
  1.日    期   : 2015年9月4日
    作    者   : huangdch
    修改内容   : 新生成函数

*****************************************************************************/
int swpa_utils_process_get_id();

/*****************************************************************************
 函 数 名  : swpa_utils_process_name_by_id
 功能描述  : 根据进程id获取进程名
 输入参数  : unsigned int pid    
             char* process_name  
             unsigned int len    
 输出参数  : 无
 返 回 值  : int
 调用函数  : 
 注意事项  : 只在linux平台下使用
 
 修改历史      :
  1.日    期   : 2015年9月4日
    作    者   : huangdch
    修改内容   : 新生成函数

*****************************************************************************/
int swpa_utils_process_name_by_id(unsigned int pid, char* process_name, unsigned int len);

/*****************************************************************************
 函 数 名  : swpa_utils_process_id_by_name
 功能描述  : 根据进程名获取进程id
 输入参数  : const char* process_name  
             unsigned int pid[]        
 输出参数  : 无
 返 回 值  : int
 调用函数  : 
 注意事项  : 如果有子进程,可能会返回多个进程id
 
 修改历史      :
  1.日    期   : 2015年9月4日
    作    者   : huangdch
    修改内容   : 新生成函数

*****************************************************************************/
int swpa_utils_process_id_by_name(const char* process_name, unsigned int pid[]);

/*****************************************************************************
 函 数 名  : swpa_utils_shm_ftok
 功能描述  : 根据文件名生成一个整数值
 输入参数  : const char* file_name  
             int model  一般取一个【0~254】的固定值就可以            
 输出参数  : 无
 返 回 值  : int key值
 调用函数  : 
 注意事项  : 文件必须存在且唯文件创建后不被修改,才能返回唯一的key值
 			 内部调用系统函数ftok()
 
 修改历史      :
  1.日    期   : 2015年9月4日
    作    者   : huangdch
    修改内容   : 新生成函数

*****************************************************************************/
int swpa_utils_string_ftok(const char* file_name, int model);

/*****************************************************************************
 函 数 名  : swpa_utils_string_strtok
 功能描述  : 按分隔符号szCh对字符串sBuf进行分隔,并返回第num个字段的整数值
 输入参数  : const char* pbuf  
             const char* szch  
             const int num     
 输出参数  : 无
 返 回 值  : int
 调用函数  : 
 注意事项  : 
 
 修改历史      :
  1.日    期   : 2015年9月4日
    作    者   : huangdch
    修改内容   : 新生成函数

*****************************************************************************/
int swpa_utils_string_strtok(const char* pbuf, const char* szch, const int num);

#ifdef __cplusplus
}
#endif

#endif

