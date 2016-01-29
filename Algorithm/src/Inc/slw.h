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

#ifndef __SLW__
#define __SLW__

#include "swbasetype.h"

#ifdef __cplusplus
extern "C" {
#endif


//#define __UBOOT__ 1 // u-boot
//#define __TI_CCS_ARM__ 1 // ccs
//#define __LINUX_KERNEL__ 1 // linux kernel
#define __LINUX_APP__ 1 // linux app


#ifdef __LINUX_KERNEL__
  #include <linux/config.h>
  #include <linux/device.h>
  #include <linux/errno.h>
  #include <linux/kernel.h>
  #include <linux/module.h>
  #include <linux/input.h>
  #include <linux/init.h>
  #include <linux/interrupt.h>
  #include <linux/wait.h>
  #include <linux/suspend.h>
  #include <linux/slab.h>
  #include <linux/moduleparam.h>
  #include <linux/types.h>
  #include <linux/fs.h>
  #include <linux/miscdevice.h>
  #include <linux/notifier.h>
  #include <linux/ioctl.h>
  #include <linux/delay.h>
  #include <linux/timer.h>

  #include <asm/mach-types.h>
  #include <asm/arch/irqs.h>
  #include <asm/arch/hardware.h>
  #include <asm/io.h>
  #include <asm/system.h>
  #include <asm/uaccess.h>

  #define PRINTF printk
#elif __LINUX_APP__
  #include <stdio.h>
  #include <stdlib.h>
  #include <linux/types.h>
  #include <sys/types.h>
  #include <fcntl.h>
  #include <unistd.h>

  typedef unsigned char  u8;
  typedef unsigned short u16;
  typedef unsigned int   u32;
#elif __UBOOT__
  #define mdelay(n) udelay((n)*1000)

  #define PRINTF printf

  typedef unsigned char  u8;
  typedef unsigned short u16;
  typedef unsigned int   u32;
#elif __TI_CCS_ARM__
  #define udelay(n) _waitusec(n)
  #define mdelay(n) _waitmsec(n)

  #define Target_DelayTNS(n) _wait(n)
  #define Target_DelayUS(n)  _waitusec(n)
  #define Target_DelayMS(n)  _waitmsec(n)

  #define PRINTF printf

  typedef unsigned char  u8;
  typedef unsigned short u16;
  typedef unsigned int   u32;
#else
  #error "no define system type"
#endif // __LINUX_KERNEL__

typedef unsigned char  Uint8;
typedef unsigned short Uint16;
typedef unsigned int   Uint32;
typedef int            Int32;
typedef short          Int16;
typedef char           Int8;

////////////////////////////////////////
u32 swdev_get_reg(u32 addr);
void swdev_set_reg(u32 addr, u32 data);

#ifdef __cplusplus
}
#endif

#endif // __SLW__
