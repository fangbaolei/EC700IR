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

#ifndef __SLW_PCI__
#define __SLW_PCI__

#ifdef __cplusplus
extern "C" {
#endif

// PCI驱动提供的软中断信号
enum{
	SLW_SIG_NONE = 0,
	SLW_SIG_PCI_GETR, // 等待读中断
	SLW_SIG_PCI_WAIT, // 等待返回中断
	SLW_SIG_MAX,
};

// 注：以下的主端为主动请求端，从端为被打请求端
enum {
  SWPCI_IOCTL_LIST_BUFS = 2000, // 取得所有内存块的信息
  SWPCI_IOCTL_GETW_BUF, // 主端获取取得空闲内存块
  SWPCI_IOCTL_PUTW_BUF, // 主端写完，放回
  SWPCI_IOCTL_GETR_BUF, // 从端取得权限
  SWPCI_IOCTL_PUTR_BUF, // 从端写完，放回
  SWPCI_IOCTL_WAIT_BUF, // 主端等待返回
  SWPCI_IOCTL_FREE_BUF, // 主端释放
  SWPCI_IOCTL_LAST,
};

// PCI内存块的信息结构
typedef struct swpci_buffer {
  unsigned int index; // 序号
  unsigned int size; // 长度
  unsigned int stat; // 状态
  unsigned int req; // 申请端
  unsigned int sent; // 是否已通知
  unsigned long phys; // 物理地址
  unsigned long virt; // 虚拟地址
  void *user; // 用户层地址
} swpci_buffer;

#define NETLINK_SOCKET_PCI_EVENT 20

#define NETLINK_TYPE_PCI_PID   0x10
#define NETLINK_TYPE_PCI_CLOSE 0x20
#define NETLINK_TYPE_PCI_GETR  0x30
#define NETLINK_TYPE_PCI_RTN_VALUE  0x40

#ifdef __KERNEL__

#include <linux/config.h>

#define DM6467_PCI_BASEADDR      0x01C1A000
#define DM6467_PCIBE_PCIBAR4TRL (0x01D0 + DM6467_PCI_BASEADDR)
#define DM6467_PCIBE_PCIBAR5TRL (0x01D4 + DM6467_PCI_BASEADDR)

#define RBUFFER_PHYS_START "0x8A800000" // 从PCI在DDR2上的写地址 120~128M
#define RBUFFER_PHYS_END   "0x8B000000"
#define WBUFFER_PHYS_START "0x8B000000" // 从PCI在DDR2上的读地址 128~136M
#define WBUFFER_PHYS_END   "0x8B800000"

#define PCI_FLAG_WAIT  0x66bb66bb // 一级等待
#define PCI_FLAG_READY 0x77cc77cc // 二级准备
#define PCI_FLAG_DONE  0x88dd88dd // 三级发射

#define PCI_STATUS_WAIT  0x11111111 // 等待
#define PCI_STATUS_READY 0x22222222 // 准备
#define PCI_STATUS_DONE  0x33333333 // 完成

#define PCI_TIMER 1 // PCI定时器的时长(秒)

// master PCI
#ifdef CONFIG_PCI
#define GPIO_READ_INT  2
#define GPIO_WRITE_INT 3

#define MAX_RBUFFER_LEN   (8*1024*1024) // 主PCI读的缓冲区大小
#define MAX_WBUFFER_LEN   (8*1024*1024) // 主PCI写的缓冲区大小

#define PCI_DMA_PHYS_START "0x8B800000" // 主PCI在DDR2上的写地址 184~192M
#define PCI_DMA_PHYS_END   "0x8C000000"

#define PRD_START_PHYS_ADDR (pci_dma_phys + 0x000000)
#define PRD_START_VIRT_ADDR (pci_dma_virt + 0x000000)

#define DMA_START_PHYS_ADDR (pci_dma_phys + 0x400000)
#define DMA_START_VIRT_ADDR (pci_dma_virt + 0x400000)
#else
// slave PCI
#define GPIO_READ_INT  2
#define GPIO_WRITE_INT 3
#endif // CONFIG_PCI

#define BUFFER_TIMEOUT 5

#define GPIO_ACK       1 // 回复确认
#define GPIO_RST       0 // 归位

#define ENABLE_BUFFER  1
#define DISABLE_BUFFER 0

#endif // __KERNEL__

#ifdef __cplusplus
}
#endif

#endif // __SLW_PCI__
