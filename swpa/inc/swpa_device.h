/**
* @file  swpa_device.h   
* @brief  设备平台相关
* @author  
* @date  2013-2-20
* @version  V0.1
* @note     
*/ 


#ifndef _SWPA_DEVICE_H_
#define _SWPA_DEVICE_H_

#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
	SWPA_DEVICE_CAMERA_FPGA = 0,
	SWPA_DEVICE_DM368 ,
	SWPA_DEVICE_USB,
	
	SWPA_DEVICE_COUNT	
} SWPA_DEVICE_TYPE;



/**
* @brief 使能看门狗，同时设置狗超时时长
*
* 
* @param [in] timeout_ms 设置的超时时间，单位为毫秒（ms）。详细说明请看注解内容
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note 
* - timeout_ms如果为0，则超时时间为底层默认值：2000 ms，
* - timeout_ms如果大于0，则为所设置的时间
* - timeout_ms如果小于0，则为无限等待
* @see .
*/
int swpa_watchdog_enable(	
	int 	timeout_ms
);


/**
* @brief 关闭看门狗
*
* 
* @param .
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @note .
* @see .
*/
int swpa_watchdog_disable(	
	void
);


/**
* @brief 看门狗握手
*
* 
* @param .
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @note .
* @see .
*/
int swpa_watchdog_handshake(	
	void
);


/**
* @brief 判断DM368是否已可连接。
*
* 
* @retval 0 : 可连接
* @retval -1 : 不可连接
* @note .
* @see SWPA_DEVICE_TYPE.
*/
int swpa_device_dm368_ready();


/**
* @brief 复位附属设备
*
* 当主程序发现附属设备卡死或其他情况下需要将设备复位重启时，可以调用该函数完成复位设备。
* 异步操作，线程安全。
*
* 
* @param [in] dev_type 指明slave设备种类，取值范围有Camera FPGA，DM368， USB设备
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @retval -2 : 参数非法
* @note .
* @see SWPA_DEVICE_TYPE.
*/
int swpa_device_reset(	
  SWPA_DEVICE_TYPE dev_type
);


/**
* @brief 加载附属设备镜像文件
*
* 
* @param [in] dev_type 指明设备种类，目前取值范围有Camera FPGA，DM368， USB设备等
* @param [in] filename 指明设备要加载的镜像路径和文件名
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @retval -2 :  参数非法
* @note 
* - USB设备没有镜像文件，因此不能传入该设备类型.
* @see SWPA_DEVICE_TYPE.
*/
int swpa_device_load(	
  SWPA_DEVICE_TYPE dev_type,
  char * filename
);



/**
* @brief 开始蜂鸣响
*
* 
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @note 
* - 目前水星平台没有蜂鸣器，故该API留空，直接return -1.
* @see .
*/
	
int swpa_buzzer_on(	
	void
);


/**
* @brief 停止蜂鸣响
*
* 
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @note .
* - 目前水星平台没有蜂鸣器，故该API留空，直接return -1.
* @see .
*/
	
int swpa_buzzer_off(	
	void	
);



/**
* @brief 心跳LED灯点亮
*
* 
* @param .
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @note .
* @see .
*/	
int swpa_heart_led_on(	
	void
);


/**
* @brief 心跳LED灯熄灭
*
* 
* @param .
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @note .
* @see .
*/
	
int swpa_heart_led_off(
	void
);



/**
* @brief 报警LED灯点亮
*
* 
* @param .
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @note .
* @see .
*/	
int swpa_alarm_led_on(	
	void
);


/**
* @brief 报警LED灯熄灭
*
* 
* @param .
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @note .
* @see .
*/
	
int swpa_alarm_led_off(
	void
);


/**
* @brief 硬盘LED灯点亮
*
* 
* @param .
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @note .
* @see .
*/	
int swpa_hdd_led_on(	
	void
);


/**
* @brief 硬盘LED灯熄灭
*
* 
* @param .
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @note .
* @see .
*/
	
int swpa_hdd_led_off(
	void
);



/**
* @brief 网络连接LED灯点亮
*
* 
* @param .
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @note .
* @see .
*/	
int swpa_network_led_on(	
	void
);


/**
* @brief 网络连接LED灯熄灭
*
* 
* @param .
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @note .
* @see .
*/
	
int swpa_network_led_off(
	void
);




/**
* @brief 获取CPU温度
*
* 
* @param [out] temperature  获取到的温度值
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @retval -2 :  参数非法
* @note 
* - 因水星平台所用CPU无温感设备，故该API实现为空，直接返回-1！
* @see .
*/
	
int swpa_info_get_cpu_temperature(	
	int * temperature
);



/**
* @brief 获取环境温度
*
* 
* @param [out] temperature  获取到的温度值
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @retval -2 :  参数非法
* @note .
* @see .
*/	
int swpa_info_get_env_temperature(	
	int * temperature
);




/**
* @brief 获取当前复位模式
*
* 
* @param [out] pmode  获取到的复位模式，合法值有:
* - 0 : 正常工作模式
* - 1 : 升级工作模式
* - 2 : 升级工作模式
* @retval SWPAR_OK : 执行成功
* @retval SWPAR_FAIL :  读取复位模式失败
* @note .
* @see .
*/	
int swpa_device_get_resetmode(int* pmode);



/**
* @brief 设置当前复位模式
*
* 
* @param [out] pmode  将要设置的复位模式，合法值有:
* - 0 : 正常工作模式
* - 1 : 升级工作模式
* - 2 : 升级工作模式
* @retval SWPAR_OK : 执行成功
* @retval SWPAR_FAIL :  设置复位模式失败
* @note .
* @see .
*/	
int swpa_device_set_resetmode(const int mode);





/**
* @brief 初始化加密芯片
* 
* @retval SWPAR_OK : 执行成功
* @retval SWPAR_FAIL :  底层执行失败
* @note .
* @see .
*/	
int swpa_device_crypt_init();



/**
 *
 * @brief 加密写
 *
 * @param addr [in]    : 地址
 * @param data [out]   : 数据
 * @param len [in/out] : 长度(传出的len为写入的实际长度)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 * 只能在初始化加密功能(swpa_device_crypt_init)之后进行!!!
 */
int swpa_device_crypt_write(unsigned int addr, unsigned char* data, unsigned int* len);


/**
 *
 * @brief 加密读
 *
 * @param addr [in]    : 地址
 * @param data [out]   : 数据
 * @param len [in/out] : 长度(传出的len为写入的实际长度)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 * 只能在初始化加密功能(swpa_device_crypt_init)之后进行!!!
 */
int swpa_device_crypt_read(unsigned int addr, unsigned char* data, unsigned int* len);




/**
 * 烧写加密芯片(烧NC和KEY)
 *
 * @param nc [in]       : NC值
 * @param key [in]      : 只读数据KEY
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *       1、熔丝位断了后，不能再调用此函数烧写！！
 *       2、nc长度为7，key长度为16
 */
int swpa_device_init_crypt_config(unsigned char* nc, unsigned char* key);




/**
* @brief 获取当前加密芯片熔丝状态
* 
* @param [out] pvalue  读取到的熔丝状态，合法值有:
* - 0 : 已经熔断
* - 非0 : 尚未熔断
* @retval SWPAR_OK : 执行成功
* @retval SWPAR_FAIL :  底层执行失败
* @note .
* @see .
*/	
int swpa_device_get_fuse_status(unsigned char* pvalue);




/**
* @brief 烧断加密芯片熔丝
* 
* @retval SWPAR_OK : 执行成功
* @retval SWPAR_FAIL :  底层执行失败
* @note .
* @see .
*/	
int swpa_device_fuse_write(void);





/**
* @brief 获取固件版本信息
*
* 
* @param [in] version 存放版本信息的字符串指针
* @param [inout] len 带入预先分配给version的空间大小，带出实际获取到的字符串的大小
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @retval -2 :  参数非法
* @note 
* - 该函数把uboot，kernel，ubl，camera fpga， cpld的版本信息全部组合放在version字串里
* - 字串中每个模块版本信息的摆放结构是“模块名：模块版本字串[换行符]”，如“uboot:vxxxxx\n”
* - 如果version为NULL，或len小于真实版本信息的长度，该函数通过len返回version字串的真实长度，version指向的内存内容不变。
* @see .
*/
int swpa_info_get_firmware_version(
	char * version,
	int * len
);



/**
* @brief 获取硬件主板版本
*
* 
* @param [out] version 存放版本字符串
* @param [inout] len 带入version缓存区的大小，并返回实际version字串的长度
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @retval -2 :  参数非法
* @note .
* @see .
*/	
int swpa_info_get_hardware_version(	
	char * version,
	int * len
);


/**
* @brief 获取设备序列编号
*
* 
* @param [out] serial_no 存放设备序列编号字符串
* @param [inout] len 带入serial_no缓存区的大小，并返回实际serial_no字串的长度
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @retval -2 :  参数非法
* @note .
* @see .
*/	
int swpa_info_get_device_serial_no(	
	char * serial_no,
	int * len
);

/**
 *@brief 注册回调函数,负责dsp通知arm
 *@param [in] dsp_alarm 回调函数
                        puser 用户自定义参数
                        info  dsp通知arm的信息
 */
int swpa_device_register_callback( void (*dsp_alarm)(void *puser, int info), void *puser);

/**
 *@brief 注册回调函数,负责dsp通知arm
 *@param [in] dsp_alarm 回调函数
                        puser 用户自定义参数
                        info  dsp通知arm的信息
 */
int swpa_device_register_callback_venus( void (*dsp_alarm)(void *puser, int info), void *puser);

/**
* @brief 获取复位键是否长按5S事件。
*
* @retval 0 : 有事件
* @retval 1 :  无事件
* @note .
* @see .
*/	
int swpa_device_get_reset_flag();



/**
* @brief 获取SD卡是否接入设备。
*
* @param [out] status SD卡接入状态：0：已接入；1：已移除
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @retval -2 :  参数非法
* @note .
* @see .
*/	
int swpa_device_get_sdcard_status(int* status);

/**
* @brief 设置485方向
*
* @param [in] dir 方向状态：0：输入；1：输出
* @retval 0 : 执行成功
* @retval -1 :  执行失败
* @retval -2 :  参数非法
* @note 金星、木星平台只有一个485，故不添加“设备类型”
* @see .
*/	
int swpa_device_set_rs485(int dir);

/**
* @brief 重新初始化SD卡
*
* @retval 0 : 初始化成功
* @retval -1 : 初始化失败
* @note .
* @see .
*/	
int swpa_device_sd_reinit();

/**
* @brief 读取FPGA寄存器
*
* @retval 0 : 读取成功
* @retval -1 : 读取失败
* @note .
* @see .
*/	
int swpa_fpga_reg_read(unsigned int addr,unsigned int *value);

/**
* @brief 写单个FPGA寄存器
*
* @retval 0 : 写入成功
* @retval -1 : 写入失败
* @note .
* @see .
*/																	
int swpa_fpga_reg_write(unsigned int addr,unsigned int value);

/**
* @brief 写多个FPGA寄存器
*	count 写入寄存器个数，不能大于MAX_FPGA_REG
*	addr  写入寄存器地址数组
*	value 写入寄存器值数组，下标与地址数组对应
* @retval 0 : 写入成功
* @retval -1 : 写入失败
* @note .
* @see .
*/																	
int swpa_fpga_multi_reg_write(int count, unsigned int *addr,unsigned int *value);

/**
* @brief DC光圈控制
*	duty 为占空比，单位为千分之一，即duty_val为900表示千分之九百\n
* @retval 0 : 写入成功
* @retval -1 : 写入失败
* @note .
* @see .
*/	
int spwa_autoiris_pwm(int duty);


#ifdef __cplusplus
}
#endif

#endif // _SWPA_DEVICE_H_


