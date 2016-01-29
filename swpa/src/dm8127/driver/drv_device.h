/**
 * @file   drv_device.h
 * @author lujy
 * @date   2013-11-12
 *
 * @brief
 *         设备相关API。
 * @note
 *         1、目前包括的模块有：eeprom、加密认证、看门狗、
 *         LED灯、温度、获取版本号\n
 *         2、如函数注释无特别说明，均是阻塞式访问。\n
 *		   3、为了兼容其他平台，flash、cpld模块的接口仍保留，内容为空实现。
 *
 */
#ifndef DRV_DEVICE_H
#define DRV_DEVICE_H

#include "drv_device_hal.h"
#include "drv_device_eeprom_hal.h"

#ifdef __cplusplus
extern "C"
{
#endif

//---------------------- EEPROM  START-----------------------------

/**
 * 读取EEPROM数据
 *
 * @param dev_id [in]  : EEPROM设备序号
 * @param addr [in]    : EEPROM地址
 * @param buffer [out] : 读出数据缓冲区
 * @param len [in/out] : 读取数据长度(传出的len为读到的实际长度)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前有且仅有DEVID_SPI_EEPROM、DEVID_IIC_EEPROM\n
 *         2、不能同一时间操作同一IIC总线上的设备，IIC EEPROM与LM75温度传感器、RTC实时钟芯片为同一IIC总线。底层驱动已经实现互斥。
 *         3、地址范围参见drv_device_eeprom_hal.h文件
 *         4、当读取长度为0时，本函数并不执行读操作
 *         5、当函数返回错误时，缓冲区数据和长度是无效的
 */
int drv_eeprom_read(DRV_DEVICE_TYPE dev_id, unsigned int addr, unsigned char* buffer, unsigned int* len);

/**
 * 写EEPROM数据
 *
 * @param dev_id [in]  : EEPROM设备序号
 * @param addr [in]    : EEPROM地址
 * @param buffer [in]  : 写入EEPROM的数据缓冲区
 * @param len [in/out] : 数据长度(传出的len为写入的实际长度)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前有且仅有DEVID_SPI_EEPROM、DEVID_IIC_EEPROM\n
 *         2、地址范围参见drv_device_eeprom_hal.h文件
 */
int drv_eeprom_write(DRV_DEVICE_TYPE dev_id, unsigned int addr, unsigned char* buffer, unsigned int* len);

//---------------------- EEPROM  END-----------------------------

//---------------------- 加密认证 START-----------------------------
// 注：因为AT88的特殊性和保密性，许多地址及名词解释均无法知道，所以请按一体机原来的流程来操作AT88！！！
/*
操作流程：
1、标准读NC值：drv_sec_std_read　地址：0x19，长度：0x07
2、初始化加密：drv_sec_crypt_init
3、加密读KEY值：drv_sec_crypt_read　地址：0x40，长度：0x10
4、加密写NC值：drv_sec_crypt_write 地址：0x0，长度：0x07
5、加密读NC值：drv_sec_crypt_read 地址：0x0，长度：0x07


自动化测试：
1、标准写drv_sec_std_write 地址：0x0a，值：任意一个字节数据
2、标准读drv_sec_std_read  地址：0x0a，得到的值与写入的值对比

*/

/**
 * 烧写AT88(烧NC和KEY)

 * @param dev_id [in]   : 设备序号
 * @param nc [in]       : NC值
 * @param key [in]      : 只读数据KEY
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *       1、熔丝位断了后，不能再调用此函数烧写！！
 *       2、nc长度为7，key长度为16
 */
int drv_sec_init_config(DRV_DEVICE_TYPE dev_id, unsigned char* nc, unsigned char* key);

/**
 * 初始化加密功能

 * @param dev_id [in]   : 设备序号
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *        <b>标准读/写只能在调用本函数前进行</b>，\n
 *        <b>调用本函数后，只能进行加密读/写、读熔丝状态、熔断熔丝等操作
 *
 */
int drv_sec_crypt_init(DRV_DEVICE_TYPE dev_id);

/**
 * 标准读
 *
 * @param dev_id [in]  : 设备序号
 * @param addr [in]    : 地址
 * @param data [out]   : 数据
 * @param len [in/out] : 长度(传出的len为读到的实际长度)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前有且仅有DEVID_IIC_AT88SC\n
 *            只能在初始化加密功能(drv_sec_crypt_init)之前进行!!!
 */
int drv_sec_std_read(DRV_DEVICE_TYPE dev_id, unsigned int addr, unsigned char* data, unsigned int* len);

/**
 * 标准写
 *
 * @param dev_id [in]  : 设备序号
 * @param addr [in]    : 地址
 * @param data [in]   : 数据
 * @param len [in/out] : 长度(传出的len为读到的实际长度)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前有且仅有DEVID_IIC_AT88SC\n
 *         2、只能在初始化加密功能(drv_sec_crypt_init)之前进行!!!\n
 *         3、目前该函数只应用于自动化测试中！！
 */
int drv_sec_std_write(DRV_DEVICE_TYPE dev_id, unsigned int addr, unsigned char* data, unsigned int* len);

/**
 * 加密读
 *
 * @param dev_id [in] : 设备序号
 * @param addr [in]    : 地址
 * @param data [out]   : 数据
 * @param len [in/out] : 长度(传出的len为读到的实际长度)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         只能在初始化加密功能(drv_sec_crypt_init)之后进行!!!
 */
int drv_sec_crypt_read(DRV_DEVICE_TYPE dev_id, unsigned int addr, unsigned char* data, unsigned int* len);

/**
 * 加密写
 *
 * @param dev_id [in]  : 设备序号
 * @param addr [in]    : 地址
 * @param data [out]   : 数据
 * @param len [in/out] : 长度(传出的len为写入的实际长度)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 * 只能在初始化加密功能(drv_sec_crypt_init)之后进行!!!
 */
int drv_sec_crypt_write(DRV_DEVICE_TYPE dev_id, unsigned int addr, unsigned char* data, unsigned int* len);

/**
 * 读取熔丝状态
 *
 * @param dev_id [in] : 设备序号
 * @param data [out]  : 熔丝状态：0表示已经熔断，非0表示未熔断
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note   任何情况下都可以读熔丝状态
 */
int drv_sec_fuse_read(DRV_DEVICE_TYPE dev_id, unsigned char* data);

/**
 * 熔断熔丝
 *
 * @param dev_id [in]   : 设备序号
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note   !!!只有确认已经烧写了AT88才能熔断熔丝位!!，\n
 *         一旦调用本函数，不能再调用drv_sec_init_config函数，\n
 *         也就是说，不能再次烧写AT88了。
 */
int drv_sec_fuse_write(DRV_DEVICE_TYPE dev_id);
//---------------------- 加密认证 END-----------------------------

//---------------------- 看门狗 START -----------------------------
/**
 * 使能看门狗
 *
 * @param timeout_ms [in] : 超时时间(单位为毫秒)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *      1、timeout_ms单位为毫秒，接口内部使用ceil函数转换成秒，\
 *          如果设置的不是秒的整数倍，则向上取整数，例：500ms实际转换为1s。
 *       2、timeout_ms如果为0，则超时时间为默认值：10s，非0则为所设置的时间
 *       3、超时时间范围：0 < time <= 60s
 */
int drv_watchdog_enable(int timeout_ms);

/**
 * 禁止看门狗
 *
 *
 * @return 成功：0  失败：-1
 */
int drv_watchdog_disable(void);

/**
 * 看门狗握手(喂狗)
 *
 *
 * @return 成功：0  失败：-1
 */
int drv_watchdog_handshake(void);
//---------------------- 看门狗 END -----------------------------

//---------------------- LED START -----------------------------
/**
 * LED灯亮
 *
 * @param dev_id [in]   : 设备ID号
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id有且仅有: DEVID_LED_RED、DEVID_LED_GREEN
 */
int drv_led_on(DRV_DEVICE_TYPE dev_id);

/**
 * LED灯灭
 *
 * @param dev_id [in]   : 设备ID号
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id有且仅有: DEVID_LED_RED、DEVID_LED_GREEN
 */
int drv_led_off(DRV_DEVICE_TYPE dev_id);
//---------------------- LED END -----------------------------

//---------------------- GPIO START -----------------------------
/**
 * 设置GPIO输出值
 *
 * @param dev_id [in]  : 设备序号
 * @param gpio_id [in] : GPIO编号
 * @param value [in]   : 该GPIO对应的值
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *        1、dev_id: DEVID_GPIO0、DEVID_GPIO1、DEVID_GPIO2、DEVID_GPIO3\n
 *        2、value只能是0或1
 */
int drv_gpio_set_output(DRV_DEVICE_TYPE dev_id, int gpio_id, int value);

/**
 * 获取GPIO输入值
 *
 * @param dev_id [in]  : 设备序号
 * @param gpio_id [in] : GPIO编号
 * @param value [out]  : 该GPIO对应的值
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *        1、dev_id: DEVID_GPIO0、DEVID_GPIO1、DEVID_GPIO2、DEVID_GPIO3\n
 *        2、value为获取的值：0或1\n
 *        3、注意！！GPIO作为输入口，不能在程序中设置其值，只能读取该值
 */
int drv_gpio_get_input(DRV_DEVICE_TYPE dev_id, int gpio_id, int* value);

/**
 * 获取GPIO状态(保留接口，做空实现)
 *
 * @param dev_id [in]  : 设备序号
 * @param gpio_id [in] : GPIO编号
 * @param status [out]  : 该GPIO 的状态   输入状态:0     输出状态:1
 *
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *        1、dev_id: DEVID_GPIO0、DEVID_GPIO1、DEVID_GPIO2、DEVID_GPIO3\n
 *
 */
int drv_gpio_get_status(DRV_DEVICE_TYPE dev_id, int gpio_id,int *status);

/**
 * 获取GPIO状态(保留接口，做空实现)
 *
 * @param dev_id [in]  : 设备序号
 * @param gpio_id [in] : GPIO编号
 * @param status [out]  : 该GPIO 的值，0或1
 *
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *        1、dev_id: DEVID_GPIO0、DEVID_GPIO1、DEVID_GPIO2、DEVID_GPIO3\n
 *
 */
int drv_gpio_get_value(DRV_DEVICE_TYPE dev_id, int gpio_id,int *value);


//---------------------- GPIO END-----------------------------

//---------------------- FPGA START -----------------------------

/// 一次设置FPGA寄存器最大个数
#define MAX_FPGA_REG 64

/// FPGA寄存器结构体
/**
@note
<pre>
addr由4位命令及12位地址组成，请参考FPGA说明文档自行组织地址格式
格式：  |   16     |   4  |          12       |
        |    0     | cmd  |       addr        |
可使用下列的宏：
</pre>
\code
// 地址转换
// addr：地址
#define FPGA_W(addr) ((0x1 << 12) | (addr))
#define FPGA_R(addr) ((0x9 << 12) | (addr))

// num：AD芯片序号(1~4) addr：地址
#define AD_W(num, addr) ((0 << 15) | (((num)-1) << 13) | (addr))
#define AD_R(num, addr) ((1 << 15) | (((num)-1) << 13) | (addr))
\endcode
示例：AD_W(1, 0x4)表示写第1片AD的0x4地址

*/
typedef struct {
    unsigned int addr;  /**< FPGA寄存器地址 */
    unsigned int data;  /**< FPGA寄存器对应的数值 */
} FPGA_REG;

/// FPGA信息结构体，占用516字节空间
typedef struct _FPGA_STRUCT{
    unsigned int count;           /**< 寄存器个数 */
    FPGA_REG regs[MAX_FPGA_REG];  /**< 寄存器结构体 */
#ifdef _cpluscplus
    _FPGA_STRUCT
    {
        count = 0;
    };
#endif
} FPGA_STRUCT;

/**
 * 加载FPGA镜像
 *
 * @param dev_id [in]   : FPGA设备序号
 * @param fpga_img [in] : FPGA镜像缓冲区指针(并非FPGA文件)
 * @param fpga_len [in] : 缓冲区大小
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前有且仅有DEVID_SPI_FPGA
 */
int drv_fpga_load(DRV_DEVICE_TYPE dev_id, unsigned char* fpga_img, int fpga_len);

/**
 * 读FPGA寄存器
 *
 * @param dev_id [in]      : FPGA设备序号
 * @param fpga_info [out]  : FPGA信息结构体，参考FPGA_STRUCT
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前有且仅有DEVID_SPI_FPGA\n
 *         2、一次最多可读64个寄存器\n
 *         3、本函数关注的是寄存器地址，对于传入的数据，可以是任意值，\
 *            返回该结构体时，数据是寄存器实际值
 *
 */
int drv_fpga_read(DRV_DEVICE_TYPE dev_id, FPGA_STRUCT* fpga_info);

/**
 * 一次读取一个FPGA寄存器
 *
 * @param dev_id [in] : FPGA设备序号
 * @param addr [in]   : 寄存器地址
 * @param data [out]   : 数据
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前有且仅有DEVID_SPI_FPGA
 *
 */
int drv_fpga_read_single(DRV_DEVICE_TYPE dev_id, unsigned int fpga_reg, unsigned int* fpga_data);

/**
 * 写FPGA
 *
 * @param dev_id [in]    : FPGA设备序号
 * @param fpga_info [in] : FPGA信息结构体，参考FPGA_STRUCT
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前有且仅有DEVID_SPI_FPGA\n
 *         2、所有设置FPGA寄存器都通过本函数进行设置
 *         3、一次最多可设置64个寄存器
 *
 * @sa 结构体见 FPGA_REG
 */
int drv_fpga_write(DRV_DEVICE_TYPE dev_id, FPGA_STRUCT* fpga_info);

/**
 * 获取FPGA版本号
 *
 * @param dev_id [in]   : FPGA设备序号
 * @param version [out] : FPGA版本号
 * @param len [in]      : 长度
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前有且仅有DEVID_SPI_FPGA
 *         2、传入的长度至少16字节，实际长度请使用strlen获取
 *
 */
int drv_fpga_get_version(DRV_DEVICE_TYPE dev_id, unsigned char* version, int len);


// 调试使用
int drv_fpga_get_reset(DRV_DEVICE_TYPE dev_id);

//---------------------- FPGA END -----------------------------

//---------------------- 外设模块 START -----------------------------
/**
 * 获取网络状态
 *
 * @param dev_id [in]  : 设备序号
 * @param status [out] : 连接状态(0：无接连 1：连接)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *        dev_id: DEVID_PHY
 *
 */
int drv_phy_get_status(DRV_DEVICE_TYPE dev_id, unsigned int* status);

/**
 * 设置485方向
 *
 * @param dev_id [in] : 设备序号
 * @param dir [in]    : 方向(0: 输入  1: 输出)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *        dev_id有且仅有: DEVID_RS485_1、DEVID_RS485_2
 *
 */
int drv_device_set_rs485_direction(DRV_DEVICE_TYPE dev_id, int dir);

/**
 * 复位指定设备
 *
 * @param dev_id [in] : 设备序号
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *        1、根据不同ID号复位不同的设备
 */
int drv_device_reset(DRV_DEVICE_TYPE dev_id);

/**
 * 获取设备温度
 *
 * @param dev_id [in]       : 设备序号
 * @param temperature [out] : 温度
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前只能是：DEVID_IIC_LM75
 *         2、不能同一时间操作同一IIC总线上的设备，IIC EEPROM与LM75温度传感器、RTC实时钟芯片为同一IIC总线。底层驱动已经实现互斥。
 */
int drv_device_get_temperature(DRV_DEVICE_TYPE dev_id, int* temperature);

/**
 * 使能PWM异步通知信号
 *
 * @param dev_id [in]  : 设备序号
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前只能是：DEVID_PWM
 */
int drv_device_get_enable_pwm_signal(DRV_DEVICE_TYPE dev_id);

/**
 * 获取PWM寄存器值
 *
 * @param dev_id [in]  : 设备序号
 * @param info [out]   : 寄存器值
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前只能是：DEVID_PWM
 *         2、info在目前的应用主要是共享内存的地址值。
 */
int drv_device_get_pwminfo(DRV_DEVICE_TYPE dev_id, unsigned int* info);

 /**
 * 获取固件版本号
 *
 * @param ver_id [in]   : 各种版本号对应的ID
 * @param version [out] : 版本号
 * @param len [in/out]  : 长度(返回的len为实际的版本长度)
 *
 * @return 成功：0  失败：-1
 *
 * @note
 *        1、通过不同的ID号获取不同的版本号，包括UBL、UBOOT、内核、驱动，具体参考DRV_VERID_TYPE列出的所有ID\n
 *        2、传入的len长度至少为256字节
 *
 * @sa DRV_VERID_TYPE
 */
int drv_device_get_firmaware_version(DRV_VERID_TYPE ver_id, unsigned char* version, int* len);

/**
 * 检测sd卡状态(插上或拔出)
 *
 * @param dev_id [in]  : 设备序号
 * @param status [out] : 连接状态(1：拔出 0：插上)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *        dev_id: DEVID_SD_CARD
 *
 */
int drv_sdcard_check_status(DRV_DEVICE_TYPE dev_id,int *status);


//---------------------- 外设模块 END -----------------------------

//---------------------- 自动光圈控制 START -----------------------------
/**
 * 利用pwm控制自动光圈打开或关闭
 *
 * @param dev_id [in]  : 设备序号
 * @param duty_val [in]    : 占空比值(单位为千分之一)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *        dev_id: DEVID_PWM\n
 *        duty_val为占空比，单位为千分之一，即duty_val为900表示千分之九百\n
 *        打开：低于约750，可以设置为0\n
 *　　　　关闭：高于约750，可以设置为1000\n
 *        注意！！本函数只用于控制自动光圈打开或关闭，不具备自动调节功能。\n
 *        M3端使用自动控制，调用本函数可能会对M3的控制造成影响。请注意。
 */
int drv_autoiris_pwm(DRV_DEVICE_TYPE dev_id, int duty_val);

//---------------------- 自动光圈控制 END -----------------------------

//---------------------- CPLD 、flash、等空实现接口 START-----------------------------

//---------------------- CPLD START-----------------------------

/**
 * 读取CPLD数据
 *
 * @param dev_id [in] : CPLD设备序号
 * @param addr [in]   : CPLD地址
 * @param data [out]  : 读取的数据(注：数据为8位)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前有且仅有DEVID_SPI_CPLD\n
 */
int drv_cpld_read(DRV_DEVICE_TYPE dev_id, unsigned int addr, unsigned char* data);

/**
 * 写CPLD数据
 *
 * @param dev_id [in] : CPLD设备序号
 * @param addr [in]   : CPLD地址
 * @param data [in]   : 写入的数据(注：数据为8位)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前有且仅有DEVID_SPI_CPLD\n
 */
int drv_cpld_write(DRV_DEVICE_TYPE dev_id, unsigned int addr, unsigned char data);

/**
 * 获取CPLD版本号
 *
 * @param dev_id [in]   : CPLD设备序号
 * @param version [out] : CPLD版本号
 * @param len [in]      : 长度
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前有且仅有DEVID_SPI_CPLD\n
 *         2、长度必须为1字节
 */
int drv_cpld_get_version(DRV_DEVICE_TYPE dev_id, unsigned char* version, int len);

//---------------------- CPLD END-----------------------------

//---------------------- FLASH START-----------------------------

/**
 * 读取Flash数据
 *
 * @param dev_id [in]  : Flash设备序号
 * @param addr [in]    : Flash地址
 * @param buffer [out] : 读出数据缓冲区
 * @param len [in/out] : 读取数据长度(传出的len为读到的实际长度)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前有且仅有DEVID_SPI_FLASH_DM6467、DEVID_SPI_FLASH_DM368\n
 */
int drv_flash_read(DRV_DEVICE_TYPE dev_id, unsigned int addr, unsigned char* buffer, unsigned int* len);

/**
 * 写Flash数据
 *
 * @param dev_id [in]  : Flash设备序号
 * @param addr [in]    : Flash地址
 * @param buffer [in]  : 写入Flash的数据缓冲区
 * @param len [in/out] : 数据长度(传出的len为写入的实际长度)
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前有且仅有DEVID_SPI_FLASH_DM6467、DEVID_SPI_FLASH_DM368\n
 */
int drv_flash_write(DRV_DEVICE_TYPE dev_id, unsigned int addr, unsigned char* buffer, unsigned int* len);

/**
 * 擦除Flash
 *
 * @param dev_id [in]  : Flash设备序号
 * @param addr [in]    : 要擦除的Flash地址
 * @param len [in/out] : 擦除的Flash长度
 *
 * @return 成功：0  失败：-1 参数非法：-2
 *
 * @note
 *         1、dev_id目前有且仅有DEVID_SPI_FLASH_DM6467、DEVID_SPI_FLASH_DM368\n
 */
int drv_flash_erase(DRV_DEVICE_TYPE dev_id, unsigned int addr, unsigned int* len);


//---------------------- FLASH END -----------------------------

// ---------------------- 硬盘标志 START-----------------------------
 /**
 * 设置硬盘标志
 *
 * @param has_hdd [in]   : 1: 设备上存在硬盘 0: 设备无硬盘
 *
 * @return 成功：0  失败：-1
 *
 * @note
 */
int drv_set_hdd_flag(int has_hdd);

 /**
 * 获取硬盘标志
 *
 * @param has_hdd [out]   : 1: 设备上存在硬盘 0: 设备无硬盘 -1: 标志非法，无法判断是否有硬盘
 *
 * @return 成功：0  失败：-1
 *
 * @note
 */
int drv_get_hdd_flag(int* has_hdd);

// ---------------------- 硬盘标志　END-----------------------------

//---------------------- CPLD 、flash、等空实现接口 END-----------------------------

#ifdef __cplusplus
}
#endif

#endif
