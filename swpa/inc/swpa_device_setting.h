#ifndef SWPA_DEVICE_SETTING_H
#define SWPA_DEVICE_SETTING_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 写序列号
 *
 * @param sn [in] : 序列号字符串
 *
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL 参数非法：-2
 *
 * @note

 */
int swpa_device_write_sn(char* sn);

/**
 * 读序列号
 *
 * @param sn [out] : 序列号字符串
  * @param len [in/out] : 传入缓冲区的大小，传出实际写入的长度。
 *
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL 参数非法：-2
 *
 * @note
 *        sn由调用者申请内存，建议大小至少为256Bytes。
 */
int swpa_device_read_sn(char* sn,  unsigned int* len);

/**
 * 写IP地址、网关、子网掩码
 *
 * @param ip [in] : IP地址字符串
 * @param netmark [in] : 网关字符串
 * @param gateway [in] : 子网掩码字符串
 *
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL 参数非法：-2
 *
 * @note

 */
int swpa_device_write_ipinfo(char* ip_str, char* netmark_str, char* gateway_str);

/**
 * 读IP地址、网关、子网掩码
 *
 * @param ip [out] : IP地址字符串
 * @param ip_len [in] : IP地址字符串长度
 * @param netmark [out] : 网关字符串
 * @param netmark_len [in] : 网关字符串长度
 * @param gateway [out] : 子网掩码字符串
 * @param gateway_len [in] : 子网掩码字符串长度
 *
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL 参数非法：-2
 *
 * @note
 *         1、!!! IP地址等缓冲区由调用者申请，必须保证足够大小 !!!
 *         2、当缓冲区很小时，获取的字符串长度只有指定大小，如：\n
 *         指定IP地址缓冲区为5个字节，则ip_str值为“172.”。使用指定长度是为了避免内存出错。
 */
int swpa_device_read_ipinfo(char* ip_str, int ip_len, char* netmark_str, int netmark_len, char* gateway_str, int gateway_len);

/**
 * 锁定uboot
 *
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL
 *
 * @note
 *         出库时做此操作\n

 */
int swpa_device_lock_uboot(void);

/**
 * 解锁uboot
 *
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL
 *
 * @note

 */
int swpa_device_unlock_uboot(void);

/**
 * 关闭(锁定)linux信息打印功能
 *
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL
 *
 * @note
 *         出库时做此操作
 */
int swpa_device_lock_linuxinfo(void);

/**
 * 解锁linux信息打印功能
 *
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL
 *
 * @note
 */
int swpa_device_unlock_linuxinfo(void);

/**
 * 恢复uboot默认参数(请谨慎使用！！！)
 *
 *
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL 参数非法：-2
 *
 * @note
 *         包括：\n
 *         清空启动时间表、清空uboot日志\n
 *         调用此函数后，需要重启设备。uboot需要启动2次。因为有看门狗，应用层无需关注此过程。
 */
int swpa_device_restore_defaultinfo();

/**
 * 写uboot启动参数
 *
 * @param bootargs [in] : 启动参数字符串
 *
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL 参数非法：-2
 *
 * @note

 */
int swpa_device_write_bootargs(char* bootargs);

/**
 * 读uboot启动参数
 *
 * @param bootargs [out] : 启动参数字符串
 * @param len [in] : 输入缓冲区大小。
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL 参数非法：-2
 *
 * @note
 *        bootargs由调用者申请内存，建议大小至少为256Bytes。\n
 *        如果len太小，则bootargs只有len大小。
 */
int swpa_device_read_bootargs(char* bootargs, unsigned int len);

/**
 * 读启动时间表
 *
 * @param boottime [out] :
 * @param len [in] : 输入缓冲区大小
 *
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL 参数非法：-2
 *
 * @note
 *         1、!!!starttable缓冲区由调用者申请，len最大为2048Byte\n
 *         2、启动时间表由uboot写，应用层只能读
 */
int swpa_device_read_boottime(char* boottime, unsigned int len);


/**
 * 读启动日志
 *
 * @param boottime [out] : 启动日志
 * @param len [in] : 输入缓冲区大小
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL 参数非法：-2
 *
 * @note
 *         1、len最大为2048Byte\n
 *         2、启动日志由uboot写，应用层只能读
 */
int swpa_device_read_bootlog(char* bootlog, unsigned int len);


/**
 * 设置启动次数
 *
 * @param count [out] : 启动次数
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL
 *
 * @note
 */
int swpa_device_set_bootcount(int count);



/**
 * 设置正常系统启动次数
 *
 * @param count [out] : 启动次数
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL
 *
 * @note
 */
int swpa_device_set_normal_bootcount(int count);

/**
 * 设置备份系统启动次数
 *
 * @param count [out] : 启动次数
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL
 *
 * @note
 */
int swpa_device_set_backup_bootcount(int count);



/**
 * 设置启动次数使能标志
 *
 * @param is_enable [in] : 启动次数使能标志(0：禁止 1：使能)
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL
 *
 * @note 只有在新版本uboot(v13)上此函数才有效。
 */
int swpa_device_set_bootcount_flag(int is_enable);

/**
 * 读取启动次数使能标志
 *
 * @param flag [flag] : 启动次数使能标志(0：禁止 1：使能)
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL
 *
 * @note 在旧版本uboot上，获取到的flag为无效值-1。
 */
int swpa_device_get_bootcount_flag(int* flag);

// 以下函数是高级使用，前面提供的接口已经满足应用需求，但还是保留下面的接口
/**
 * 设置uboot启动延时时间
 *
 * @param delaytime [out] : 延时时间，单位为秒，范围：[0~30]
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL 参数非法：-2
 *
 * @note
 */
int swpa_device_set_bootdelay(int delaytime);


// ---------------------- 设置硬盘标志 START-----------------------------
 /**
 * 设置硬盘标志
 *
 * @param has_hdd [in]   : 1: 设备上存在硬盘 0: 设备无硬盘
 *
 * @return 成功：0  失败：-1
 *
 * @note
 */
int swpa_device_set_hdd_flag(int has_hdd);

 /**
 * 设置硬盘标志
 *
 * @param has_hdd [out]   : 1: 设备上存在硬盘 0: 设备无硬盘 -1: 数据读取无效
 *
 * @return 成功：0  失败：-1
 *
 * @note
 */
int swpa_device_get_hdd_flag(int* has_hdd);


/**
 * 通过CPLD设置H264的分辨率
 *
 * @param resolution_sel [in] : 分辨率选择，0: 720P 1：1080P
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL 参数非法：-2
 *
 * @note
 寄存器0x0c
 01000       720P
 10000       1080P

 */
int swpa_device_set_resolution(int resolution_sel);



/**
* 检测恢复应用层默认参数标志
*
* @param flag [out] : 恢复默认参数标志  0 : 不恢复默认  1 : 恢复默认
*
* @return 成功：0  失败：-1 
*
*/
int swpa_device_get_default_param_flag(int *flag);


/**
* 清除恢复应用层默认参数标志
*
* @return 成功 : 0  失败:-1 
*/
int swpa_device_clear_default_param_flag(void);


/**
* @brief 获取金星设备后面板按键事件
*
*@param [out] event : 按键事件，取值有:0-按下；1-抬起；
* @retval 0 : 执行成功
* @retval 1 : 执行失败
* @note .
* @see .
*/
int swpa_device_get_reset_key_event(int * event);



/**
 * 设置启动启动系统
 *
 * @param sys_type [in] : 启动系统类型 0：正式系统  1：备份系统
 * @retval SWPAR_OK : 执行成功
 * @retval SWPAR_FAIL : 执行失败
 * @note
 */
int swpa_device_set_bootsystem(const int sys_type);



#ifdef __cplusplus
}
#endif

#endif // SWPA_DEVICE_SETTING_H
