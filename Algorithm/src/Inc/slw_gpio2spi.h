/*
 * gpio_spi.h
 * author: wyq
 * date: 2010-11-23 15:12
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

#include "config.h"

#if defined(_CAMERA_PIXEL_500W_) || defined(_CAMERA_PIXEL_200W_25FPS_)
#define AD9974_USED
#else
#define AD9949_USED
#endif

#ifndef __SLW_GPIO2SPI__
#define __SLW_GPIO2SPI__

#ifdef __cplusplus
extern "C" {
#endif

#include "slw.h"
#include "slw_gpio.h"


//#define GPIO_EECS 41 /* EECS: cs */
//#define GPIO_EECK 26 /* EECK: clk */
//#define GPIO_MOSI 42 /* EEDO: send to 6467 */
//#define GPIO_MISO 25 /* EEDI: receive from 6467 */

#ifdef CONFIG_GPIO2SPI_FLASH // gpio2spi read/write CPLD/nor flash 2
#define SPI2_CS0	23
#define SPI2_CS1	26
#define SPI2_CS2	25
#define SPI2_CLK	24
#define SPI2_SIMO	39
#define SPI2_SOMI	40

#define SPI3_CS0	8
#define SPI3_CLK	37
#define SPI3_SIMO	36
#define SPI3_SOMI	38
#else // 第二版
#define SPI2_CS0	38 // FPGA
#define SPI2_CS1	36 // CPLD
#define SPI2_CS2	37 // FLASH2
#define SPI2_CLK	8
#define SPI2_SIMO	39
#define SPI2_SOMI	40

#define SPI3_CS0	26
#define SPI3_CLK	25
#define SPI3_SIMO	24
#define SPI3_SOMI	23 // FPGA
#endif

#define SPI2		2
#define SPI3		3


#define SpiFlash0	0
#define SpiFlash1	1


enum {
  SPI_CS = 0,
  SPI_CLK,
  SPI_SIMO,
  SPI_SOMI
};

enum {
  FLASH0_SPI_BANK = 0,
  CPLD_SPI_BANK,
  FLASH1_SPI_BANK,
  FPGA_SPI_BANK,
  MAX_SPI_BANK
};

#define MAX_FPGA_REG 32

typedef struct {
#if (defined(AD9974_USED))
  u16 addr;
#elif (defined(AD9949_USED))
  u8 addr;
#else
  #error AD99xx_USED not defined
#endif
  u32 data;
} FPGA_REG;

typedef struct {
  u32 count;
  FPGA_REG regs[MAX_FPGA_REG];
} FPGA_STRUCT;


void spi_init(int bank);

HRESULT CPLD_SPI_Read(
	u8  addr,  //CPLD片内地址
	u8* pData); //指向读出的数据


HRESULT CPLD_SPI_Write(
	u8 addr, //CPLD片内地址
	u8 Data); //写入的数据

#ifdef __cplusplus
}
#endif

#endif // __SLW_GPIO2SPI__
