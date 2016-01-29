// 该文件名必须是WINDOWS-936
/**
 * @file   drv_device_cpld_hal.h
 * @author lijj
 * @date   2013-02-18
 *
 * @brief
 *         CPLD寄存器
 *
 * @note   参考《MERCURY主板CPLD寄存器说明.docx》
 *
 */

#ifndef DRV_DEVICE_CPLD_HAL_H
#define DRV_DEVICE_CPLD_HAL_H

#ifdef __cplusplus
extern "C"
{
#endif

// 命名：CPLD_ADDR_XXX
// 注：以硬件给出的CPLD寄存器为准

/// CPLD寄存器，无特别说明，寄存器为可读、可写， 所有寄存器为8位

/// 复位控制寄存器
#define CPLD_ADDR_RESET     0x00

/// 状态寄存器(只读)
#define CPLD_ADDR_STAT      0x01

/// FPGA和DM368 Flash配置寄存器
// 为3时，配置FPGA，
#define CPLD_ADDR_CONF      0x02

/// FPGA配置寄存器(未使用)
#define CPLD_ADDR_FPGA      0x03

/// 保留(不使用)
#define CPLD_ADDR_REV       0x04

/// 看门狗使能控制寄存器
#define CPLD_ADDR_WDT_EN    0x05

/// 看门狗定时计数寄存器1(低8位)
#define CPLD_ADDR_WDT_CNT1  0x06

/// 看门狗定时计数寄存器2(高8位)
#define CPLD_ADDR_WDT_CNT2  0x07

/// 出厂默认寄存器(只读)
#define CPLD_ADDR_DEFAULT   0x08

/// LED控制寄存器
#define CPLD_ADDR_LED       0x09

/// CPLD版本寄存器(只读)
#define CPLD_ADDR_VERSION   0x0A

/// 通用寄存器
#define CPLD_ADDR_GPREG     0x0B

/// GPIO寄存器1
#define CPLD_ADDR_GPIO1     0x0C

/// GPIO寄存器2
#define CPLD_ADDR_GPIO2     0x0D

// 0x0E 0x0F为保留寄存器
#ifdef __cplusplus
}
#endif

#endif /* DRV_DEVICE_CPLD_HAL_H */
