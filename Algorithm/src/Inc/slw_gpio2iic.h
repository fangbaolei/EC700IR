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

#ifndef __SLW_GPIO2IIC__
#define __SLW_GPIO2IIC__

#ifdef __cplusplus
extern "C" {
#endif

#include "slw.h"
#include "slw_gpio.h"

#define GPIO_I2C0_SDA 0
#define GPIO_I2C0_SCL 1

#define GPIO_I2C1_SDA 6
#define GPIO_I2C1_SCL 7

#define IIC_NACK      0
#define IIC_ACK       1

#define SET_SDA_OUTPUT()   swdev_gpio_set_direction(GPIO_I2C0_SDA, GPIO_DIR_OUTPUT)
#define SET_SCL_OUTPUT()   swdev_gpio_set_direction(GPIO_I2C0_SCL, GPIO_DIR_OUTPUT)
#define SET_SDA_INPUT()    swdev_gpio_set_direction(GPIO_I2C0_SDA, GPIO_DIR_INPUT)
#define GET_SDA()          swdev_gpio_get_input(GPIO_I2C0_SDA)
#define SET_SDA()          swdev_gpio_set_output(GPIO_I2C0_SDA, 1)
#define CLR_SDA()          swdev_gpio_set_output(GPIO_I2C0_SDA, 0)
#define SET_SCL()          swdev_gpio_set_output(GPIO_I2C0_SCL, 1)
#define CLR_SCL()          swdev_gpio_set_output(GPIO_I2C0_SCL, 0)

#define SET_SDA_OUTPUT_1() swdev_gpio_set_direction(GPIO_I2C1_SDA, GPIO_DIR_OUTPUT)
#define SET_SCL_OUTPUT_1() swdev_gpio_set_direction(GPIO_I2C1_SCL, GPIO_DIR_OUTPUT)
#define SET_SDA_INPUT_1()  swdev_gpio_set_direction(GPIO_I2C1_SDA, GPIO_DIR_INPUT)
#define GET_SDA_1()        swdev_gpio_get_input(GPIO_I2C1_SDA)
#define SET_SDA_1()        swdev_gpio_set_output(GPIO_I2C1_SDA, 1)
#define CLR_SDA_1()        swdev_gpio_set_output(GPIO_I2C1_SDA, 0)
#define SET_SCL_1()        swdev_gpio_set_output(GPIO_I2C1_SCL, 1)
#define CLR_SCL_1()        swdev_gpio_set_output(GPIO_I2C1_SCL, 0)

#define EEPROM_IIC_ADDR  0xA0
#define LM75_IIC_ADDR    0x90
#define RTC_IIC_ADDR     0xD0
#define DS2460_IIC_ADDR  0x80
#define AT88SC_IIC_ADDR  0xBA


////////////////////////////////////////
void iic_start(void);
void iic_stop(void);
void iic_ack(void);
u8 iic_nack(void);
u8 iic_write_byte(u8 data);
u8 iic_read_byte(void);

////////////////////////////////////////
void iic_start_1(void);
void iic_stop_1(void);
void iic_ack_1(void);
u8 iic_nack_1(void);
u8 iic_write_byte_1(u8 data);
u8 iic_read_byte_1(void);

#ifdef __cplusplus
}
#endif

#endif // __SLW_GPIO2IIC__
