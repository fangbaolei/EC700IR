/**
 * @file   drv_device_flash_hal.h
 * @author lijj
 * @date   2013-03-19
 *
 * @brief
 *         Flash规划表
 * @note   注意，本头文件根据《Mercury设备升级管理.docx》编写，请参考。\n
 *         1、DM6467的Flash大小为32MB\n
 *         2、DM368的Flash大小为16MB\n
 */

#ifndef DRV_DEVICE_FLASH_HAL_H
#define DRV_DEVICE_FLASH_HAL_H

#ifdef __cplusplus
extern "C"
{
#endif

// 命名规则：
// 地址：F_DM6467_ADDR_XX 长度：F_DM6467_LEN_XX
// 地址：F_DM368_ADDR_XX 长度：F_DM368_LEN_XX

#define F_DM6467_ADDR_UBOOT         0x00000000 /**< Uboot */
#define F_DM6467_LEN_UBOOT          0x00080000 /**< Uboot长度：512KB */

#define F_DM6467_ADDR_KERNEL_BAK    0x00080000 /**< 内核备份区 */
#define F_DM6467_LEN_KERNEL_BAK     0x00280000 /**< 内核备份区长度：2.5MB */

#define F_DM6467_ADDR_ROOTFS_BAK    0x00300000 /**< 根文件系统备份区 */
#define F_DM6467_LEN_ROOTFS_BAK     0x00600000 /**< 根文件系统备份区长度：6MB */

#define F_DM6467_ADDR_FPGA_ROM      0x00900000 /**< FPGA镜像文件 */
#define F_DM6467_LEN_FPGA_ROM       0x00280000 /**< FPGA镜像文件长度：2.5MB */

#define F_DM6467_ADDR_KERNEL        0x00B80000 /**< 内核镜像 */
#define F_DM6467_LEN_KERNEL         0x00300000 /**< 内核镜像长度：3MB */

#define F_DM6467_ADDR_ROOTFS        0x00E80000 /**< 根文件系统 */
#define F_DM6467_LEN_ROOTFS         0x00800000 /**< 根文件系统长度：8MB */

#define F_DM6467_ADDR_ARM_APP       0x01680000 /**< ARM端可执行程序 */
#define F_DM6467_LEN_ARM_APP        0x00180000 /**< ARM端可执行程序长度：1.5MB */

#define F_DM6467_ADDR_DSP_APP       0x01800000 /**< DSP端可执行程序 */
#define F_DM6467_LEN_DSP_APP        0x00500000 /**< DSP端可执行程序长度：5MB */

#define F_DM6467_ADDR_RESERVED      0x01D00000 /**< 保留区 */
#define F_DM6467_LEN_RESERVED       0x00200000 /**< 保留区长度：2MB */

#define F_DM6467_ADDR_APP_LOG       0x01F00000 /**< 应用层日志 */
#define F_DM6467_LEN_APP_LOG        0x00080000 /**< 应用层日志：512KB */

#define F_DM6467_ADDR_SYS_LOG       0x01F80000 /**< 系统日志 */
#define F_DM6467_LEN_SYS_LOG        0x00080000 /**< 系统日志长度：512KB */

// DM6467 Flash结束

// DM368 Flash

#define F_DM368_ADDR_UBL            0x00000000 /**< UBL */
#define F_DM368_LEN_UBL             0x00040000 /**< UBL长度：256KB */

#define F_DM368_ADDR_PARAMS         0x00040000 /**< 参数区(参数区规划参考dm368 driverlib的头文件) */
#define F_DM368_LEN_PARAMS          0x00040000 /**< 参数区长度：256KB */

#define F_DM368_ADDR_UBOOT          0x00080000 /**< Uboot */
#define F_DM368_LEN_UBOOT           0x00080000 /**< Uboot长度：512KB */

#define F_DM368_ADDR_KERNEL         0x00100000 /**< 内核镜像 */
#define F_DM368_LEN_KERNEL          0x00300000 /**< 内核镜像长度：3MB */

#define F_DM368_ADDR_ROOTFS         0x00400000 /**< 根文件系统 */
#define F_DM368_LEN_ROOTFS          0x00A00000 /**< 根文件系统长度：10MB */

#define F_DM368_ADDR_RESERVED       0x00E00000 /**< 保留区 */
#define F_DM368_LEN_RESERVED        0x00100000 /**< 保留区长度：1MB */

#define F_DM368_ADDR_APP_LOG        0x00F00000 /**< 应用层日志 */
#define F_DM368_LEN_APP_LOG         0x00080000 /**< 应用层日志：512KB */

#define F_DM368_ADDR_SYS_LOG        0x00F80000 /**< 系统日志 */
#define F_DM368_LEN_SYS_LOG         0x00080000 /**< 系统日志长度：512KB */

// DM368 Flash结束

// 结束

#ifdef __cplusplus
}
#endif

#endif /* DRV_DEVICE_FLASH_HAL_H */
