/**
 * @file   drv_device_hal.h
 * @author lujy
 * @date   2013-11-12
 *
 * @brief
 *         设备信息ID号等
 *
 * @note   本头文件列表可能会根据实际情况进行修改
 *
 */

#ifndef DRV_DEVICE_HAL_H
#define DRV_DEVICE_HAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/// 设备ID号
typedef enum DRV_DEVICE_TYPE_T {
	DEVID_SPI_EEPROM          = 0xEF000001,     /**< SPI EEPROM(存6467的UBL，应用层不要随意写！)(金星保留) */
	DEVID_IIC_EEPROM          = 0xEF000002,     /**< I2C EEPROM(用参数日志等信息) */
	DEVID_SPI_FLASH_DM6467    = 0xEF000003,     /**< SPI Flash(存6467的升级程序)(金星保留) */
	DEVID_SPI_FLASH_DM368     = 0xEF000004,     /**< SPI Flash(存368的升级程序)(金星保留) */
	DEVID_SPI_CPLD            = 0xEF000005,     /**< CPLD(金星保留) */
	DEVID_SPI_FPGA            = 0xEF000006,     /**< FPGA */
	DEVID_IIC_AT88SC          = 0xEF000007,     /**< 加密芯片 */
	DEVID_IIC_LM75            = 0xEF000008,     /**< 温度传感器 */
	DEVID_IIC_RTC             = 0xEF000009,     /**< 实时时钟 */
	DEVID_LED_RED             = 0xEF00000A,     /**< LED，红灯 */
	DEVID_LED_GREEN           = 0xEF00000B,     /**< LED，绿灯 */
	DEVID_BUZZER              = 0xEF00000C,     /**< 蜂鸣器(金星保留) */
	DEVID_PHY                 = 0xEF00000D,     /**< 网络设备 */
	DEVID_RS485_1             = 0xEF00000E,     /**< 第1路RS485 */
	DEVID_RS485_2             = 0xEF00000F,     /**< 第2路RS485(金星保留) */
    DEVID_GPIO                = 0xEF000010,     /**< GPIO(金星保留) */
    DEVID_USB                 = 0xEF000011,     /**< USB设备(金星保留) */
    DEVID_DM368               = 0xEF000012,     /**< dm368设备(金星保留) */
    DEVID_DM6467              = 0xEF000013,     /**< dm6467设备(金星保留) */
    DEVID_PWM                 = 0xEF000014,     /**< PWM设备 */
    DEVID_GPIO0               = 0xEF000015,     /**< GPIO bank 0 */
    DEVID_GPIO1               = 0xEF000016,     /**< GPIO bank 1 */
    DEVID_GPIO2               = 0xEF000017,     /**< GPIO bank 2 */
    DEVID_GPIO3               = 0xEF000018,     /**< GPIO bank 3 */
    DEVID_DM8127              = 0xEF000019,     /**< dm8127设备 */
}DRV_DEVICE_TYPE;

/// 版本号ID
typedef enum DRV_VERID_TYPE_T {
    VER_UBOOT           = 0xFF000001,   /**< DM385 UBOOT版本号 */
    VER_KERNEL          = 0xFF000002,   /**< DM385 内核版本号 */
    VER_SERIAL_VER      = 0xFF000003,   /**< 设备序列号 */
    VER_MAC_ADDR        = 0xFF000004,   /**< MAC地址 */
}DRV_VERID_TYPE;


#ifdef __cplusplus
}
#endif

#endif /* DRV_DEVICE_HAL_H */
