/*
 * gpio.c
 * author: pxw
 * date: 2010-06-13
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SLW_GPIO__
#define __SLW_GPIO__

#ifdef __cplusplus
extern "C" {
#endif

#include "slw.h"

#define GPIO_EDGE_FLAG	   1

#define GPIO_DIR_INPUT     0
#define GPIO_DIR_OUTPUT    1

#define GPIO_REG_BASE              (0x01c67000)
#define GPIO_REG_PCR               (GPIO_REG_BASE + 0x4)
#define GPIO_REG_BINTEN            (GPIO_REG_BASE + 0x8)
#define GPIO_REG_BANK_01_BASE      (GPIO_REG_BASE + 0x10) // GPIO[0:31]
#define GPIO_REG_BANK_23_BASE      (GPIO_REG_BASE + 0x38) // GPIO[32:47]

typedef struct {
    volatile u32 DIR;
    volatile u32 OUT_DATA;
    volatile u32 SET_DATA;
    volatile u32 CLR_DATA;
    volatile u32 IN_DATA;
    volatile u32 SET_RIS_TRIG;
    volatile u32 CLR_RIS_TRIG;
    volatile u32 SET_FAL_TRIG;
    volatile u32 CLR_FAL_TRIG;
    volatile u32 INTSTAT;
} GPIO_REGS;

// 初始化
void swdev_gpio_init();

// 设置方向
void swdev_gpio_set_direction(u32 pin, u32 direction);

// 读
u32 swdev_gpio_get_input(u32 pin);

// 写
// value: 0=low, 1=high
void swdev_gpio_set_output(u32 pin, u32 value);

#ifdef __cplusplus
}
#endif

#endif // __SLW_GPIO__
