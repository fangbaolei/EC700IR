/**
* @file swpa_hdd.h
* @brief 硬盘管理头文件
* 
* 定义了常规的硬盘管理
* @copyright Signalway All Rights Reserved
* @author Shaorg
* @date 2013-02-26
* @version v0.1
* @note .
*/

#ifndef _SWPA_HDD_H_
#define _SWPA_HDD_H_

#ifdef __cplusplus
#define DEFAULT(val) =val
extern "C"
{
#else	
#define DEFAULT(val)
#endif

/**
* @brief 获得硬盘目录的总容量大小
*
*
* @param [in] szDev 设备驱动名称或设备上路径名,如sda,sdb...,空则为系统的第一块硬盘
* @param [out] size 设备总容量大小，这里说的设备包括硬盘和usb,不包括网络硬盘
* @retval 0 成功
* @retval -1 失败
*/
int swpa_hdd_get_totalsize(const char* szDev, long long* size);





/**
* @brief 获得硬盘目录的可用容量大小
*
*
* @param [in] szDev 设备驱动名称或设备上路径名,如sda,sdb...,空则为系统的第一块硬盘
* @param [out] size 可用容量大小，这里说的设备包括硬盘和usb,不包括网络硬盘
* @retval 0 成功
* @retval -1 失败
*/
int swpa_hdd_get_availsize(const char* szDev, long long* size);


/**
 *brief 执行操作硬盘的命令
 *@param [in] id 命令id 0:初始化[分区、格式化];1:仅格式化
 *@retval 0 成功
 *@retval -1 失败
 */
enum{FDISK, FORMAT};
int swpa_hdd_command(int id, const char*szCmd DEFAULT(NULL));

/**
 *@brief 查询操作命令的状态
 *@param [in] id 命令id
 *@retval 0:此命令没有在运行;1:此命令正在运行;2:命令执行成功;3:命令执行失败
 */
int swpa_hdd_query(int id);

/**
* @brief 硬盘分区
*
*
* @param [in] szDev 设备驱动名称,如sda,sdb...,空则为系统的第一块硬盘
* @param [in] size 分区大小，可以指定多个大小，最后一个值必须为0，表明参数结束
* @retval 0 成功
* @retval -1 失败
*/
int swpa_hdd_fdisk(const char* szDev, long long size, ...);

/**
* @brief 硬盘格式化
*
*
* @param [in] szPartition 分区名称
* @retval 0 成功
* @retval -1 失败
*/
int swpa_hdd_format(const char* szPartition,...);


/**
* @brief 硬盘检测，检测硬盘有2中，一种是简单的检测，几秒钟即可检测完毕。一种是坏道检测和修复，这个需要耗时很久。
*
*
* @param [in] mode 检测模式，0表明简单的检测，1表明坏道检测
* @retval 0 成功
* @retval -1 失败
*/
int swpa_hdd_check(int mode);

/**
* @brief 硬盘的挂载，对于本地硬盘，swpa_hdd_format已经集成了挂载的功能，但是对于网络硬盘，需要手动挂载
*
*
* @param [in] szPartition 分区名称
* @param [in] szDir 挂载的目录
* @param [in] szParam 挂载相关的参数，如果没有额外的参数，填NULL即可。
* @retval 0 成功
* @retval -1 失败
*/
int swpa_hdd_mount(const char *szPartition, const char *szDir, const char *szParam);

/**
* @brief 硬盘的卸载
*
*
* @param [in] szPartition 分区名称
* @retval 0 成功
* @retval -1 失败
*/
int swpa_hdd_umount(const char *szPartition);


#ifdef __cplusplus
}
#endif

#endif //_SWPA_HDD_H_

