/*
* (C) Copyright 2000-2004
* DENX Software Engineering
* Wolfgang Denk, wd@denx.de
* All rights reserved.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston,
* MA 02111-1307 USA
*/
#include "HvUpdatePackCreater.h"

#include <stdio.h>
#include <stdlib.h>

#ifndef __WIN32__
#include <linux/types.h>
#include <unistd.h>
#endif

#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define WR_FILE_BUF_LEN 1024

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern u32 crc32(u32 crc, const u8 *buf, u32 len);

#define M_NUM 0x6D679BC7
#define MAKE_KEY(m1, m2, m3) ((m1*m2+m3) + (~m1)*(m1<<2)*((~m2)<<3) + m2 + (~m3) + (m3>>1))
#define MAKE_XOR_CODE(m1, m2, m3, m4) (m1*(M_NUM&m2)*m3*m4)
#define ENCODE(m1, m2, m3, m4) (m1 ^ MAKE_XOR_CODE(m2,m3,m4,M_NUM))

u32 k0 = 0xA2D1EABA;

int crypt(u8 *addr, u32 len, unsigned long* pos)
{
    u32 k1 = 0;
    u8 k2 = 0;
    k1 = crc32(0, (u8*)&k0, 4);
    k2 = MAKE_KEY(k1, (k1>>8), (k1>>16));
    while ( len-- )
    {
        *addr++ = ENCODE(*addr, k2, (unsigned char)*pos, (k1>>24));
        (*pos)++;
    }
    return 1;
}

/*
int crypt(u8 *addr, u32 len)
{
    u8 tmp[4], k3, k4, k20, k22;
    u32 k1, k2, i, j;

    for(i = 0; i < len; i++)
    {
        k1 = (k0 | position);
        for(j = 0; j < 4; j++)
        {
            tmp[j] = (u8)((k1 >> (j * 8)) && 0xff);
        }

        k2 = crc_r = crc32(crc_r, tmp, 4);
        k20 = (u8)((k2 >> 24) & 0xff);
        k22 = (u8)((k2 >> 8) & 0xff);
        k3 = k20 ^ k22;
        k4 = k3 ^ (*addr);
        (*addr) = k4;
        position ++;
        addr ++;
    }

    return 1;
}*/

//int
//main_crypt (int argc, char **argv)
//{
//    int ifd, ofd;
//    int len;
//    u8 buf[WR_FILE_BUF_LEN];
//
//    position = 0;
//    crc_r = 0;
//
//    ifd = open(argv[1], O_RDONLY); // 打开
//    if (ifd < 0) {
//        printf("open input file(%s) error!\n", argv[1]);
//        return -1;
//    }
//
//    ofd = open(argv[2], O_RDWR | O_CREAT, 0666); // 打开
//    if (ofd < 0) {
//        printf("open output file(%s) error!\n", argv[2]);
//        return -1;
//    }
//
//    do {
//        len = read(ifd, buf, WR_FILE_BUF_LEN);
//
//        crypt(buf, len);
//
//        write(ofd, buf, len);
//    } while (len > 0);
//
//    close(ifd);
//    close(ofd);
//
//    return 0;
//}

//------------------------------------------------------------------

int HvCryptFile(char* szInFile, char* szOutFile)
{
    int ifd, ofd;
    int len;
    u8 buf[WR_FILE_BUF_LEN];
	u32 position = 0;

    ifd = open(szInFile, O_RDONLY); // 打开
    if (ifd < 0) {
        printf("open input file(%s) error!\n", szInFile);
        return -1;
    }

    ofd = open(szOutFile, O_RDWR | O_CREAT, 0666); // 打开
    if (ofd < 0) {
        printf("open output file(%s) error!\n", szOutFile);
        return -1;
    }

    do {
        len = read(ifd, buf, WR_FILE_BUF_LEN);
        crypt(buf, len, &position);
        write(ofd, buf, len);
    } while (len > 0);

    close(ifd);
    close(ofd);

    return 0;
}
