/**
 * @file   drv_device_hal.h
 * @author lijj
 * @date   2013-02-18
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
	DEVID_SPI_EEPROM          = 0xEF000001,     /**< SPI EEPROM(存6467的UBL，应用层不要随意写！) */
	DEVID_IIC_EEPROM          = 0xEF000002,     /**< I2C EEPROM(用参数日志等信息) */
	DEVID_SPI_FLASH_DM6467    = 0xEF000003,     /**< SPI Flash(存6467的升级程序) */
	DEVID_SPI_FLASH_DM368     = 0xEF000004,     /**< SPI Flash(存368的升级程序) */
	DEVID_SPI_CPLD            = 0xEF000005,     /**< CPLD */
	DEVID_SPI_FPGA            = 0xEF000006,     /**< FPGA */
	DEVID_IIC_AT88SC          = 0xEF000007,     /**< 加密芯片 */
	DEVID_IIC_LM75            = 0xEF000008,     /**< 温度传感器 */
	DEVID_IIC_RTC             = 0xEF000009,     /**< 实时时钟 */
	DEVID_LED_RED             = 0xEF00000A,     /**< LED，红灯 */
	DEVID_LED_GREEN           = 0xEF00000B,     /**< LED，绿灯 */
	DEVID_BUZZER              = 0xEF00000C,     /**< 蜂鸣器 */
	DEVID_PHY                 = 0xEF00000D,     /**< 网络设备 */
	DEVID_RS485_1             = 0xEF00000E,     /**< 第1路RS485 */
	DEVID_RS485_2             = 0xEF00000F,     /**< 第2路RS485 */
    DEVID_GPIO                = 0xEF000010,     /**< GPIO */
    DEVID_USB                 = 0xEF000011,     /**< USB设备 */
    DEVID_DM368               = 0xEF000012,     /**< dm368设备 */
    DEVID_DM6467              = 0xEF000013,     /**< dm6467设备 */
    DEVID_PWM                 = 0xEF000014,     /**< PWM设备 */
}DRV_DEVICE_TYPE;

/// 版本号ID
typedef enum DRV_VERID_TYPE_T {
    VER_UBL_DM6467      = 0xFF000001,   /**< DM6467 UBL版本号 */
    VER_UBOOT_DM6467    = 0xFF000002,   /**< DM6467 UBOOT版本号 */
    VER_KERNEL_DM6467   = 0xFF000003,   /**< DM6467 内核版本号 */
    VER_SLWDEV_DM6467   = 0xFF000004,   /**< DEV设备驱动版本号 */
    VER_SERIAL_VER      = 0xFF000005,   /**< 设备序列号 */
    //VER_HARDWARE_VER    = 0xFF000006,   /**< 硬件版本号 */ // 注：硬件版本号无人提及，暂去掉
    VER_MAC_ADDR        = 0xFF000007,   /**< MAC地址 */
}DRV_VERID_TYPE;

#ifdef __cplusplus
}
#endif

#endif /* DRV_DEVICE_HAL_H */
