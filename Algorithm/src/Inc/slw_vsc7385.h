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

#ifndef __SLW_VSC7385__
#define __SLW_VSC7385__

#ifdef __cplusplus
extern "C" {
#endif

#include "slw_gpio2spi.h"

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

#define VSC7385_SPI_CMD_LEN 8  // the length of start bit and opcode
#define VSC7385_SPI_ADD_LEN 8  // bits of Address
#define VSC7385_SPI_DAT_LEN 32 // bits of Data

void write_vsc7385(
	unsigned char block,
	unsigned char subblock,
	unsigned char addr,
	unsigned int value);

unsigned int read_vsc7385(
	unsigned char block,
	unsigned char subblock,
	unsigned char addr);

void phy_write(unsigned char port_no, unsigned char reg, unsigned int val);

unsigned int phy_read(unsigned char port_no,unsigned char reg);

void phy_write_masked(
	unsigned char port_no,
	unsigned char reg,
	unsigned int val,
	unsigned int mask);

/* VLAN table commands */
#define VLAN_TAB_IDLE  0
#define VLAN_TAB_READ  1
#define VLAN_TAB_WRITE 2
#define VLAN_TAB_CLEAR 3

/* ANALYZER registers */
#define ANA_VLANACES 0xD0
#define ANA_VLANINDX 0xE0

#define MAC_PORT_NUM 2

/* The forced speed, 10Mb, 100Mb, gigabit, 10GbE. */
#define SPEED_10		10
#define SPEED_100		100
#define SPEED_1000		1000
#define SPEED_10000		10000

/* Duplex, half or full. */
#define DUPLEX_HALF		0x00
#define DUPLEX_FULL		0x01

#define SUPPORTED_10baseT_Half		(1 << 0)
#define SUPPORTED_10baseT_Full		(1 << 1)
#define SUPPORTED_100baseT_Half		(1 << 2)
#define SUPPORTED_100baseT_Full		(1 << 3)
#define SUPPORTED_1000baseT_Half	(1 << 4)
#define SUPPORTED_1000baseT_Full	(1 << 5)
#define SUPPORTED_Autoneg		(1 << 6)

enum phy_state {
	LINK_DOWN = 0,
	LINK_UP   = 1
};

struct phy_info {
	int state;		/* state of phy */
	int state_change;	/* phy state change ? */
	unsigned int phy_mode;	/* requested phy mode */
	unsigned int speed;	/* current Speed - 10 / 100 / 1000 */
	unsigned int duplex;	/* 0=Auto Negotiate, Full=3; Half=2, Unknown=1 */
};

void do_command_4_vlan(unsigned long aces_value);

void phy_receiver_init (unsigned char port_no);

void init_seq_7385(unsigned char port_no) ;

int init_vsc7385(void);

int is_vsc7385(void);

int get_vsc7385_port_status(void);

unsigned int vsc7385_get_link_state(unsigned char port_no);
unsigned int vsc7385_get_speed(unsigned char port_no);
unsigned int vsc7385_get_duplex(unsigned char port_no);

void vsc7385_set_phy_led(
  unsigned char port_no,
  unsigned char led_no,
  unsigned char mode);

#ifdef __cplusplus
}
#endif

#endif // __SLW_VSC7385__
