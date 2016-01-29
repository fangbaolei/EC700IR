/**
 * @file   drv_uboot.h
 * @author lujy
 * @date   2013-11-14
 *
 * @brief
 *         uboot及应用层共同接口
 * @note   本文件封装了如读、写序列号；读、写IP地址；设置uboot参数等等的接口。
 *
 * @todo   本文件实际是调用了eeprom操作函数，是否需要此文件接口，需要讨论确定。
 *
 *
 */
#ifndef DRV_UBOOT_H
#define DRV_UBOOT_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 写序列号
 *
 * @param sn [in] : 序列号字符串
 *
 * @return 成功：0  失败：-1  参数非法：-2
 *
 * @note

 */
int drv_device_write_sn(char* sn);

/**
 * 读序列号
 *
 * @param sn [out] : 序列号字符串
  * @param len [in/out] : 传入缓冲区的大小，传出实际写入的长度。
 *
 * @return 成功：0  失败：-1  参数非法：-2
 *
 * @note
 *        sn由调用者申请内存，建议大小至少为256Bytes。
 */
int drv_device_read_sn(char* sn,  unsigned int* len);


/**
 * 读MAC号
 *
 * @param mac [out] : MAC号字符串
  * @param len [in/out] : 传入缓冲区的大小，传出实际写入的长度。
 *
 * @return 成功：0  失败：-1  参数非法：-2
 *
 * @note
 *        sn由调用者申请内存，建议大小至少为256Bytes。
 */
int drv_device_read_mac(char* mac,  int* len);

/**
 * 写IP地址、网关、子网掩码
 *
 * @param ip_str [in] : IP地址字符串
 * @param netmark_str [in] : 网关字符串
 * @param gateway_str [in] : 子网掩码字符串
 *
 * @return 成功：0  失败：-1  参数非法：-2
 *
 * @note

 */
int drv_device_write_ipinfo(char* ip_str, char* netmark_str, char* gateway_str);

/**
 * 写IP地址
 *
 * @param ip_str [in] : IP地址字符串
 *
 * @return 成功：0  失败：-1  参数非法：-2
 *
 * @note

 */
int drv_device_write_ipaddr(char* ip_str);

/**
 * 写子网掩码
 *
 * @param netmark_str [in] : 子网掩码地址字符串
 *
 * @return 成功：0  失败：-1  参数非法：-2
 *
 * @note

 */
int drv_device_write_netmask(char* netmark_str);

/**
 * 写网关
 *
 * @param gateway_str [in] : 网关地址字符串
 *
 * @return 成功：0  失败：-1  参数非法：-2
 *
 * @note

 */
int drv_device_write_gateway(char* gateway_str);

/**
 * 读IP地址、网关、子网掩码
 *
 * @param ip_str [out] : IP地址字符串
 * @param ip_len [in] : IP地址字符串长度
 * @param netmark_str [out] : 网关字符串
 * @param netmark_len [in] : 网关字符串长度
 * @param gateway_str [out] : 子网掩码字符串
 * @param gateway_len [in] : 子网掩码字符串长度
 *
 * @return 成功：0  失败：-1  参数非法：-2
 *
 * @note
 *         1、!!! IP地址等缓冲区由调用者申请，必须保证足够大小 !!!
 *         2、当缓冲区很小时，获取的字符串长度只有指定大小，如：\n
 *         指定IP地址缓冲区为5个字节，则ip_str值为“172.”。使用指定长度是为了避免内存出错。
 */
int drv_device_read_ipinfo(char* ip_str, int ip_len, char* netmark_str, int netmark_len, char* gateway_str, int gateway_len);

/**
 * 锁定uboot
 *
 * @return 成功：0  失败：-1 
 *
 * @note
 *         出库时做此操作\n

 */
int drv_device_lock_uboot(void);

/**
 * 解锁uboot
 *
 * @return 成功：0  失败：-1 
 *
 * @note
 *         研发调试时要解锁uboot
 */
int drv_device_unlock_uboot(void);

/**
 * 关闭(锁定)linux信息打印功能
 *
 * @return 成功：0  失败：-1 
 *
 * @note
 *         出库时做此操作
 */
int drv_device_lock_linuxinfo(void);

/**
 * 解锁linux信息打印功能
 *
 * @return 成功：0  失败：-1 
 *
 * @note
 */
int drv_device_unlock_linuxinfo(void);

/**
 * 恢复uboot默认参数(请谨慎使用！！！)
 *
 *
 * @return 成功：0  失败：-1  参数非法：-2
 *
 * @note
 *         包括：\n
 *         <b>清空启动时间表、清空uboot日志</b>\n
 *         <b>调用此函数后，需要重启设备。uboot需要启动2次。因为有看门狗，应用层无需关注此过程。</b>
 */
int drv_device_restore_defaultinfo(void);

/**
 * 读启动时间表
 *
 * @param boottime [out] :
 * @param len [in] : 输入缓冲区大小
 *
 * @return 成功：0  失败：-1  参数非法：-2
 *
 * @note
 *         1、!!!starttable缓冲区由调用者申请，len最大为4096Byte\n
 *         2、启动时间表由uboot写，应用层只能读
 */
int drv_device_read_boottime(char* boottime, unsigned int len);

/**
 * 读设备启动次数
 *
 * @param count [out] :　次数
 *
 * @return 成功：0  失败：-1  参数非法：-2
 *
 */
int drv_device_read_boottime_count(int* count);

/**
 * 读启动日志
 *
 * @param bootlog [out] : 启动日志
 * @param len [in] : 输入缓冲区大小
 * @return 成功：0  失败：-1  参数非法：-2
 *
 * @note
 *         1、len最大为4096Byte\n
 *         2、启动日志由uboot写，应用层只能读
 */
int drv_device_read_bootlog(char* bootlog, unsigned int len);

/**
 * 设置正式系统的启动次数
 *
 * @param count [out] : 启动次数
 * @return 成功：0  失败：-1 
 *
 * @note
 */
int drv_device_set_normal_bootcount(int count);

/**
 * 设置备份系统的启动次数
 *
 * @param count [out] : 启动次数
 * @return 成功：0  失败：-1 
 *
 * @note
 */
int drv_device_set_backup_bootcount(int count);

/**
 * 设置启动次数使能标志
 *
 * @param is_enable [in] : 启动次数使能标志(0：禁止 1：使能)
 * @return 成功：0  失败：-1 
 *
 * @note
 */
int drv_device_set_bootcount_flag(int is_enable);

/**
 * 读取启动次数使能标志
 *
 * @param flag [out] : 启动次数使能标志(0：禁止 1：使能)
 * @return 成功：0  失败：-1 
 *
 * @note
 */
int drv_device_get_bootcount_flag(int* flag);

/**
 * 读取启动次数使能标志
 *
 * @param type [in] : 启动系统类型 0：正式系统  1：备份系统
 * @return 成功：0  失败：-1 
 *
 * @note
 */
int drv_device_set_bootsystem(int type);

////////////////////////////////////////////

// 以下函数是高级使用，请使用者慎用！！！

/**
 * 写uboot启动参数
 *
 * @param bootargs [in] : 启动参数字符串
 *
 * @return 成功：0  失败：-1  参数非法：-2
 *
 * @note
 */
int drv_device_write_bootargs(char* bootargs);

/**
 * 读uboot启动参数
 *
 * @param bootargs [out] : 启动参数字符串
 * @param len [in] : 输入缓冲区大小。
 * @return 成功：0  失败：-1  参数非法：-2
 *
 * @note
 *        bootargs由调用者申请内存，建议大小至少为512Bytes。\n
 *        如果len太小，则bootargs只有len大小。
 */
int drv_device_read_bootargs(char* bootargs, unsigned int len);

/**
 * 设置uboot启动延时时间
 *
 * @param delaytime [out] : 延时时间，单位为秒，范围：[0~30]
 * @return 成功：0  失败：-1  参数非法：-2
 *
 * @note
 */
int drv_device_set_bootdelay(int delaytime);

/**
* 检测恢复应用层默认参数标志
*
* @param flag [out] : 恢复默认参数标志  0 : 不恢复默认  1 : 恢复默认
*
* @return 成功：0  失败：-1 
*
*/
int drv_device_get_default_param_flag(int *flag);

/**
* 清除恢复应用层默认参数标志
*
* @return 成功 : 0  失败:-1 
*/
int drv_device_clear_default_param_flag(void);


#ifdef __cplusplus
}
#endif

#endif // DRV_UBOOT_H
