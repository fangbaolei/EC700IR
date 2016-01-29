// 该文件编码格式必须为WINDOWS-936格式

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

#include "config.h"

#ifndef __SLW_DEV__
#define __SLW_DEV__

#ifdef __cplusplus
extern "C" {
#endif

#include "slw_gpio.h"
#include "slw_spi.h"
#include "slw_gpio2spi.h"
#include "slw_gpio2iic.h"
#include "slw_vsc7385.h"
#include "slw_i2c_dev.h"

#define GPIO_WATCHDOG   5

enum {
  SWDEV_IOCTL_WATCHDOG = 0, // 喂狗

  SWDEV_IOCTL_CPLD_READ, // 读CPLD
  SWDEV_IOCTL_CPLD_WRITE, // 写CPLD

  SWDEV_IOCTL_LM75_GET_TEMP, // 取得温度

  SWDEV_IOCTL_EEPROM_READ, // 读EEPROM
  SWDEV_IOCTL_EEPROM_WRITE, // 写EEPROM

  SWDEV_IOCTL_AT88SC_INIT_CONFIG, // 初始化AT88
  SWDEV_IOCTL_AT88SC_EEPROM_INIT, // 初始化AT88的EEPROM
  SWDEV_IOCTL_AT88SC_STD_WRITE, // 标准写AT88
  SWDEV_IOCTL_AT88SC_STD_READ, // 标准读AT88
  SWDEV_IOCTL_AT88SC_EEPROM_CRYTP_WRITE, // 加密写AT88
  SWDEV_IOCTL_AT88SC_EEPROM_CRYTP_READ, // 加密读AT88

  SWDEV_IOCTL_DS2460_INIT, // 初始化DS2460
  SWDEV_IOCTL_DS2460_READ_SERIAL_NUMBER, // 读DS2460的序列号
  SWDEV_IOCTL_DS2460_COMPUTE_SHA, // 硬计算SHA
  SWDEV_IOCTL_DS2460_COMPUTE_SHAEE, // 软计算SHA
  SWDEV_IOCTL_DS2460_WRITE_INPUT_SHA, // 写入加密内容
  SWDEV_IOCTL_DS2460_READ_INPUT_SHA, // 读出加密内容
  SWDEV_IOCTL_DS2460_MATCH_MAC, // 匹配MAC
  SWDEV_IOCTL_DS2460_READ_MAC, // 读MAC
  SWDEV_IOCTL_DS2460_WRITE_MAC, // 写MAC
  SWDEV_IOCTL_DS2460_WRITE_SSECRET,  // 写SSECRET
  SWDEV_IOCTL_DS2460_WRITE_ESECRET1, // 写ESECRET1
  SWDEV_IOCTL_DS2460_WRITE_ESECRET2, // 写ESECRET2
  SWDEV_IOCTL_DS2460_WRITE_ESECRET3, // 写ESECRET3
  SWDEV_IOCTL_DS2460_TRANSFER_SECRET, // 改变SECRET的存储

  SWDEV_IOCTL_AD9949_WRITE, // 写AD9949

  SWDEV_IOCTL_DMA_CONVERT_IMAGE, // 通过DMA旋转YUV图像(使用CONVERT_IMAGE_STRUCT结构)
                                 // 宽x高=copy的长度, 宽、高不能大于64*1024

  SWDEV_IOCTL_DMA_COPY_JPEG, // 通过DMA结织数据给JPEG CODER(使用CONVERT_IMAGE_STRUCT结构)
                                 // 宽x高=copy的长度, 宽、高不能大于64*1024

  SWDEV_IOCTL_DMA_COPY_H264, // 通过DMA结织数据给H264 CODER(使用CONVERT_IMAGE_STRUCT结构)
                                 // 宽x高=copy的长度, 宽、高不能大于64*1024

  SWDEV_IOCTL_DMA_COPY_YUV, // YUV完整copy(使用CONVERT_IMAGE_STRUCT结构)
                            // 宽x高=copy的长度, 宽、高不能大于64*1024

  SWDEV_IOCTL_DMA_ROTATION_YUV, // Y copy 一半并进行旋转90度, UV copy所有的且不进行旋转
                                // (!!!使用ROTATION_YUV_STRUCT结构 !!!)
                                // 宽x高=copy的长度, 宽、高不能大于64*1024

  SWDEV_IOCTL_DMA_PCI_READ, // 通过DMA读PCI内容到指定地址(使用CONVERT_IMAGE_STRUCT结构)
                            // 宽x高=copy的长度, 宽、高不能大于64*1024

  SWDEV_IOCTL_DMA_PCI_WRITE, // 通过DMA把指定地址的内容写到PCI(使用CONVERT_IMAGE_STRUCT结构)
                             // 宽x高=copy的长度, 宽、高不能大于64*1024

  SWDEV_IOCTL_DMA_COPY, // DMA copy(!!! 使用DMACOPY_STRUCT结构 !!!)

  SWDEV_IOCTL_LOAD_FPGA_FROM_JTAG, // 配置FPGA

  SWDEV_IOCTL_DMA_COPY_UV, // copy UV到分别的U和V(使用COPY_UV_STRUCT结构)
                            // 宽x高=copy的长度, 宽、高不能大于64*1024

  SWDEV_IOCTL_GET_PHOTO_FROM_SLAVE, // 从CPU进行抓拍

  SWDEV_IOCTL_QDMA_COPY, // QDMA copy(!!! 使用DMACOPY_STRUCT结构 !!!)
  SWDEV_IOCTL_QDMA_COPY_JPEG, // 通过DMA结织数据给JPEG CODER(使用CONVERT_IMAGE_STRUCT结构)
                                 // 宽x高=copy的长度, 宽、高不能大于64*1024

#ifdef _CAMERA_PIXEL_500W_
  SWDEV_IOCTL_AD9974_WRITE, // 写AD9974
#elif defined(_CAMERA_PIXEL_200W_25FPS_)
  SWDEV_IOCTL_GET_PHY_STATUS,
  SWDEV_IOCTL_AD9974_WRITE,
#else
  SWDEV_IOCTL_GET_PHY_STATUS, // 取得PHY的状态
#endif

  SWDEV_IOCTL_LAST,
};

typedef struct {
  u32 addr;
  u32 data;
} REG_STRUCT;

typedef struct {
  u8 data[EEPROM_AT24C1024_PAGE_SIZE];
  u32 addr;
  u32 len;
} EEPROM_STRUCT;

typedef struct {
  u8 data;
  u8 addr;
} CPLD_STRUCT;

typedef struct {
  union {
    u8 buf[64];
    u8 sn[8];
    u8 key[8];
    u8 mac[20];
  } data;
} DS2460_STRUCT;

typedef struct {
  u8 data[32*4];
  u32 addr;
  u32 len;
  u32 ok_len;
} AT88SC_STRUCT;

typedef struct {
  u32 src_addr; // 源物理地址
  u32 dst_addr; // 目的物理地址
  u32 width; // 源图像宽
  u32 height; // 源图像高
} CONVERT_IMAGE_STRUCT;

typedef struct {
  u32 y_src; // Y的源物理地址
  u32 y_dst; // Y的目的物理地址
  u32 uv_src; // UV的源物理地址
  u32 uv_dst; // UV的目的物理地址
  u32 width; // 源图像宽
  u32 height; // 源图像高
} ROTATION_YUV_STRUCT;

typedef struct {
  u32 uv_src; // UV的源物理地址
  u32 u_dst; // U的目的物理地址
  u32 v_dst; // V的目的物理地址
  u32 width; // 源图像宽
  u32 height; // 源图像高
} COPY_UV_STRUCT;

/****************************
 |        cidx=24       |
 |bidx=8|
 |a=5|
 ooooo...ooooo...ooooo... -
 ooooo...ooooo...ooooo... c=3
 ooooo...ooooo...ooooo... -
 |          b=3         |
****************************/
typedef struct {
  u32 src; // 源物理地址
  u32 dst; // 目的物理地址
  short a; // 一维(元素大小)
  short b; // 二维(宽)
  short c; // 三维(高)
  short sbidx; // 源的一维(元素)的间隔
  short scidx; // 源的二维(宽)的间隔
  short dbidx; // 目的的一维(元素)的间隔
  short dcidx; // 目的的二维(宽)的间隔
} DMACOPY_STRUCT;

// PHY的状态结构
typedef struct {
	u8 linked[2]; // 0为无连接，1为连接
	u8 speed[2];  // 0x00 = 10Mpbs, 0x01 = 100Mpbs, 0x02 = 1000Mpbs, 0x03 = Reserved
} PHY_STATUS_STRUCT;

#ifdef __cplusplus
}
#endif

#endif // __SLW_DEV__
