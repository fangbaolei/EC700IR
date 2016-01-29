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

#ifndef __SLW_SPI__
#define __SLW_SPI__

#ifdef __cplusplus
extern "C" {
#endif

#include "slw.h"

#define SPI_REG_BASE 0x01c66800

#define SPIGCR0  (SPI_REG_BASE + 0x00)
#define SPIGCR1  (SPI_REG_BASE + 0x04)
#define SPIINT   (SPI_REG_BASE + 0x08)
#define SPILVL   (SPI_REG_BASE + 0x0c)
#define SPIFLG   (SPI_REG_BASE + 0x10)
#define SPIPC0   (SPI_REG_BASE + 0x14)
#define SPIPC2   (SPI_REG_BASE + 0x1c)
#define SPIDAT1  (SPI_REG_BASE + 0x3c)
#define SPIBUF   (SPI_REG_BASE + 0x40)
#define SPIEMU   (SPI_REG_BASE + 0x44)
#define SPIDELAY (SPI_REG_BASE + 0x48)
#define SPIDEF   (SPI_REG_BASE + 0x4c)
#define SPIFMT0  (SPI_REG_BASE + 0x50)
#define SPIFMT1  (SPI_REG_BASE + 0x54)
#define SPIFMT2  (SPI_REG_BASE + 0x58)
#define SPIFMT3  (SPI_REG_BASE + 0x5c)
#define INTVEC0  (SPI_REG_BASE + 0x60)
#define INTVEC1  (SPI_REG_BASE + 0x61)

/* #define SPI_INTERRUPT_MODE 1 */
#define SPI_SPIFMT_PHASE_MASK        (0x00010000u)
#define SPI_SPIFMT_PHASE_SHIFT       (0x00000010u)
#define SPI_SPIFMT_PHASE_RESETVAL    (0x00000000u)

#define SPI_SPIFMT_POLARITY_MASK     (0x00020000u)
#define SPI_SPIFMT_POLARITY_SHIFT    (0x00000011u)
#define SPI_SPIFMT_POLARITY_RESETVAL (0x00000000u)

#define SPI_SPIFMT_SHIFTDIR_MASK     (0x00100000u)
#define SPI_SPIFMT_SHIFTDIR_SHIFT    (0x00000014u)
#define SPI_SPIFMT_SHIFTDIR_RESETVAL (0x00000000u)

/* SPIGCR1 */

#define SPI_SPIGCR1_SPIENA_MASK      (0x01000000u)
#define SPI_SPIGCR1_SPIENA_SHIFT     (0x00000018u)
#define SPI_SPIGCR1_SPIENA_RESETVAL  (0x00000000u)

#define SPI_INTLVL_1				         (0x000001FFu)
#define SPI_INTLVL_0				         (0x00000000u)

/* SPIPC0 */
#define SPI_SPIPC0_DIFUN_MASK        (0x00000800u)
#define SPI_SPIPC0_DIFUN_SHIFT       (0x0000000Bu)
#define SPI_SPIPC0_DIFUN_RESETVAL    (0x00000000u)

/*----DIFUN Tokens----*/
#define SPI_SPIPC0_DIFUN_DI          (0x00000001u)

#define SPI_SPIPC0_DOFUN_MASK        (0x00000400u)
#define SPI_SPIPC0_DOFUN_SHIFT       (0x0000000Au)
#define SPI_SPIPC0_DOFUN_RESETVAL    (0x00000000u)

/*----DOFUN Tokens----*/
#define SPI_SPIPC0_DOFUN_DO          (0x00000001u)

#define SPI_SPIPC0_CLKFUN_MASK       (0x00000200u)
#define SPI_SPIPC0_CLKFUN_SHIFT      (0x00000009u)
#define SPI_SPIPC0_CLKFUN_RESETVAL   (0x00000000u)

/*----CLKFUN Tokens----*/
#define SPI_SPIPC0_CLKFUN_CLK        (0x00000001u)

#define SPI_SPIPC0_EN1FUN_MASK       (0x00000002u)
#define SPI_SPIPC0_EN1FUN_SHIFT      (0x00000001u)
#define SPI_SPIPC0_EN1FUN_RESETVAL   (0x00000000u)

/*----EN1FUN Tokens----*/
#define SPI_SPIPC0_EN1FUN_EN1        (0x00000001u)

#define SPI_SPIPC0_EN0FUN_MASK       (0x00000001u)
#define SPI_SPIPC0_EN0FUN_SHIFT      (0x00000000u)
#define SPI_SPIPC0_EN0FUN_RESETVAL   (0x00000000u)

/*----EN0FUN Tokens----*/
#define SPI_SPIPC0_EN0FUN_EN0        (0x00000001u)

#define SPI_SPIPC0_RESETVAL          (0x00000000u)
#define SPI_SPIPC0_SPIENA		         (0x00000001u)
#define SPI_SPIPC0_SPIENA_SHIFT	     (0x00000008u)

#define SPI_SPIINT_MASKALL           (0x000001FF)

/* SPIDAT1 */

#define SPI_SPIDAT1_CSHOLD_MASK      (0x10000000u)
#define SPI_SPIDAT1_CSHOLD_SHIFT     (0x0000001Cu)
#define SPI_SPIDAT1_CSHOLD_RESETVAL  (0x00000000u)

#define SPI_SPIDAT1_CSNR_MASK        (0x00030000u)
#define SPI_SPIDAT1_CSNR_SHIFT       (0x00000010u)
#define SPI_SPIDAT1_CSNR_RESETVAL    (0x00000000u)

#define SPI_SPIDAT1_DFSEL_MASK       (0x03000000u)
#define SPI_SPIDAT1_DFSEL_SHIFT      (0x00000018u)
#define SPI_SPIDAT1_DFSEL_RESETVAL   (0x00000000u)

#define SPI_SPIFMT_CHARLEN_MASK      (0x0000001Fu)
#define SPI_SPIFMT_CHARLEN_SHIFT     (0x00000000u)
#define SPI_SPIFMT_CHARLEN_RESETVAL  (0x00000000u)

#define SPI_SPIGCR1_CLKMOD_MASK      (0x00000002u)
#define SPI_SPIGCR1_CLKMOD_SHIFT     (0x00000001u)
#define SPI_SPIGCR1_CLKMOD_RESETVAL  (0x00000000u)

#define SPI_SPIGCR1_MASTER_MASK      (0x00000001u)
#define SPI_SPIGCR1_MASTER_SHIFT     (0x00000000u)
#define SPI_SPIGCR1_MASTER_RESETVAL  (0x00000000u)

#define SPI_SPIGCR1_LOOPBACK_MASK     (0x00010000u)
#define SPI_SPIGCR1_LOOPBACK_SHIFT    (0x00000010u)
#define SPI_SPIGCR1_LOOPBACK_RESETVAL (0x00000000u)

#define SPI_SPIBUF_TXFULL_MASK       (0x20000000u)
#define SPI_SPIBUF_TXFULL_SHIFT      (0x0000001Du)
#define SPI_SPIBUF_TXFULL_RESETVAL   (0x00000000u)

/* SPIBUF */
#define SPI_SPIBUF_RXEMPTY_MASK      (0x80000000u)
#define SPI_SPIBUF_RXEMPTY_SHIFT     (0x0000001Fu)
#define SPI_SPIBUF_RXEMPTY_RESETVAL  (0x00000001u)

#define SPI_SPIFLG_BITERRFLG_MASK     (0x00000010u)
#define SPI_SPIFLG_BITERRFLG_SHIFT    (0x00000004u)
#define SPI_SPIFLG_BITERRFLG_RESETVAL (0x00000000u)

#define SPI_SPIFLG_OVRNINTFLG_MASK     (0x00000040u)
#define SPI_SPIFLG_OVRNINTFLG_SHIFT    (0x00000006u)
#define SPI_SPIFLG_OVRNINTFLG_RESETVAL (0x00000000u)

#define SPI_SPIFLG_RXINTFLAG_MASK     (0x00000100u)
#define SPI_SPIFLG_RXINTFLAG_SHIFT    (0x00000008u)
#define SPI_SPIFLG_RXINTFLAG_RESETVAL (0x00000000u)

#define SPI_SPIINT_DMA_REQ_EN    (0x00010000u)
#define SPI_SPIINT_RX_INTR       (0x00000100u)
#define SPI_SPIINT_TIMEOUT_INTR  (0x00000002u)
#define SPI_SPIINT_PARERR_INTR	 (0x00000004u)
#define SPI_SPIINT_DESYNC_INTR	 (0x00000008u)
#define SPI_SPIINT_BITERR_INTR	 (0x00000010u)
#define SPI_SPIINT_OVRRUN_INTR	 (0x00000040u)


#define    SPI_MASTER       1
#define    SPI_SLAVE        0

#define     EVEN            1
#define     ODD             2
#define     NOParity        0

#define     MSB1st          0
#define     LSB1st          1

#define     ClckHigh        1
#define     ClckLow         0

#define     PreEdge         0
#define     BackEdge        1

#define     SPI_CS0         2
#define     SPI_CS1         1
#define     SPI_CS0_CS1     0
#define     SPI_NoCS        3

#define 	CPLD_SELE       SPI_CS1
#define   VSC7385_SELE    SPI_CS0

/*--SPI控制器的数据格式结构体定义------------*/
typedef struct{
		u32 WDELAY;   // 两次数据传输间的延时，实际延时时间 = WDELAY*PSYSCLK3 + 2 *PSYSCLK3 ；
		u8  PARITY;   // 奇偶校验极性选择，1:even parity    2:odd parity 0:no parity
		u8  WAITENA;  // master 等待ENA信号; 0: does not wait for ENA ; 1: wait for ENA
		u8  SHIFTDIR; // 数据的移动方向，0:MSB shift first ; 1: LSB shift first
		u8  CSTIMER;  // 是否使能Transmit-end-to-chip-select-inactive-delay和Chip-select-active-to-transmit-start-delay；1: 使能，0:不使能
		u8  CLKPolar; // SPI 时钟极性 ; 0:low-inactive ; 1: high-inactive;
		u8  PHASE;    // SPI 时钟相位 ; 0:pre-edge; 1:back-edge
		u32 PreScale; // SPI的时钟预分频，此决定SPI的数据传输率；SPI_CLK frequency = [SYSCLK3 frequency ] / [PreScale + 1]
		u32 CharLen;  // SPI的数据长度。2 ~~ 16位。
} SPI_DataForm;

/*-----SPI总线模式的枚举说明--------------------------*/
typedef enum {
	SPI_OPMODE_3PIN = 0,        /**< SPI master 3 pin mode */
	SPI_OPMODE_SPISCS_4PIN = 1, /**< SPI master 4 pin mode uses SPISCS */
	SPI_OPMODE_SPIENA_4PIN = 2, /**< SPI master 4 pin mode uses SPIENA */
	SPI_OPMODE_5PIN = 3         /**< SPI master 5 pin mode */
} spiPinOpMode;

typedef struct _SPI_USER_CONFIG {
	u32          SPI_MODE;   //SPI的工作模式，可选MASTER,和SLAVE
	spiPinOpMode pinOpModes; // SPI的总线模式选择
	SPI_DataForm DForm1;     // SPI 数据格式1
	SPI_DataForm DForm2;     // SPI 数据格式2
	SPI_DataForm DForm3;     // SPI 数据格式3
	SPI_DataForm DForm4;     // SPI 数据格式4
} SPI_USER_CONFIG;

////////////////////////////////////////
HRESULT InitSysSpi(void);

HRESULT DM6467_SPI_initial(
	u32           SPI_MODE,   //SPI的工作模式，可选MASTER,和SLAVE
	u32           loopBack,   // 1:闭环测试； 0: DISABLE
	u32           intrLevel,  // SPI 中断 Level 选择; 0:SPINT0 ; 1:SPINT1
	spiPinOpMode  pinOpModes, // SPI的总线模式选择，
	SPI_DataForm* pForm1,     // 指向SPI 数据格式1
	SPI_DataForm* pForm2,     // 指向SPI 数据格式2
	SPI_DataForm* pForm3,     // 指向SPI 数据格式3
	SPI_DataForm* pForm4);   // 指向SPI 数据格式4
		
HRESULT SPI_Write(
	u8  CS,       //总线片选，可为SPI_CS0,SPI_CS1, SPI_CS0_CS1, SPI_NoCS
	u8  DFSEL,    //SPI的数据格式选择，有0~3 四个数据格式选择
	u8  CtrlAddr, //设备的控制,地址字节
	u8* pData);   //指向发送的数据8位

HRESULT SPI_Read(
	u8  CS,       //总线片选，可为SPI_CS0,SPI_CS1, SPI_CS0_CS1, SPI_NoCS
	u8  DFSEL,    //设备的控制,地址字节
	u8  CtrlAddr, //设备的控制,地址字节
	u8* pData);   // 指向读出的数据

HRESULT CPLD_SPI_Read(
	u8  addr,   //CPLD片内地址
	u8* pData); //指向读出的数据

HRESULT CPLD_SPI_Write(
	u8 addr,  //CPLD片内地址
	u8 Data); //写入的数据

#ifdef __cplusplus
}
#endif

#endif // __SLW_SPI__
